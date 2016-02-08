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


#include <ctype.h>

#include <errno.h>

#include <stdlib.h>

#include <malloc.h>


#include <string.h>


#ifdef MAX
#undef MAX
#endif

#define MAX(a,b) ((a)>(b)?(a):(b))

#include "sr_snmp.h"
#include "diag.h"
#include "oid_lib.h"
#include "sr_math.h"
SR_FILENAME


#ifndef isxdigit
#define	isxdigit(c) (((c) >= '0' && (c) <= '9') || ((c) >= 'a' && (c) <= 'f') || ((c) >= 'A' && (c) <= 'F'))
#endif /* !isxdigit */


 


/*
 * MakeOctetString: This routine produces a library OctetString
 *          construct from a passed byte-string and length. This
 *          construct is usually passed to other library calls, such
 *          as MakeVarBindWithNull() or MakeAuthentication(), and
 *          linked into a larger library construct of an ASN.1 entity.
 *          FreeOctetString() recovers all memory malloc'ed by
 *          MakeOctetString, but should not be used if the octetstring
 *          is passed to another library routine. The free_*()
 *          counterparts to those routine free up all memory that has
 *          been linked to the higher level ASN.1 structure.
 *
 *   The former name (pre-snmp12.1.0.0 release) was make_octetstring().
 */

OctetString *
MakeOctetString(string, length)
    const unsigned char  *string;
    SR_INT32        length;
{
    OctetString    *os_ptr;

    if (length > sr_pkt_size) {
        DPRINTF((APPACKET, "MakeOctetString: Bad octetstring length: %d\n", length));
        return (NULL);
    }

    if ((os_ptr = (OctetString *) malloc(sizeof(OctetString) + length)) == NULL) {
	DPRINTF((APWARN, "MakeOctetString, os_ptr malloc\n"));
	return (NULL);
    }

    os_ptr->octet_ptr =
      (unsigned char *) (((char *)os_ptr) + sizeof(OctetString));

     
    if(string != NULL) {
	memcpy((char *) os_ptr->octet_ptr, (char *) string, (size_t) length);
    }
#ifdef SR_CLEAR_MALLOC
    else {
	memset((char *)os_ptr->octet_ptr, 0, (size_t)length);
    }
#endif	/* SR_CLEAR_MALLOC */
    os_ptr->length = length;

    return (os_ptr);
}				/* end of MakeOctetString */

/* 
 * MakeOctetString2():
 * This function is almost like MakeOctetString().
 * It does the same thing, but allocates N extra bytes on the end of
 * the octet string set by the complie-time constant EXTRA, below
 */

OctetString *
MakeOctetString2(string, length)
    const unsigned char  *string;
    SR_INT32        length;
{
    OctetString    *os_ptr;

#define EXTRA 6

    if (length > sr_pkt_size) {
        DPRINTF((APPACKET, 
                 "MakeOctetString2: Bad octetstring length: %d\n", length));
        return (NULL);
    }

    if ((os_ptr = (OctetString *) malloc(sizeof(OctetString) + length + EXTRA)) == NULL)
 {
        DPRINTF((APWARN, "MakeOctetString2, os_ptr malloc\n"));
        return (NULL);
    }

    os_ptr->octet_ptr =
      (unsigned char *) (((char *)os_ptr) + sizeof(OctetString));


    if (string != NULL) {
        memset( (char *)(&os_ptr->octet_ptr[length]), 0, EXTRA);
        memcpy((char *) os_ptr->octet_ptr, (char *) string, (size_t) length);
    }
#ifdef SR_CLEAR_MALLOC
    else {
        memset((char *)os_ptr->octet_ptr, 0, (size_t)length + EXTRA);
    }
#endif  /* SR_CLEAR_MALLOC */
    os_ptr->length = length;

    return (os_ptr);
}                               /* end of MakeOctetString2 */


/*
 * CloneObjectSyntax:
 *
 * Make a copy of an ObjectSyntax structure.
 */
ObjectSyntax *
CloneObjectSyntax(os)
    const ObjectSyntax *os;
{
    ObjectSyntax *new_os;

    if (os == NULL) {
        return(NULL);
    }

    new_os = (ObjectSyntax *)malloc(sizeof(ObjectSyntax));
    if (new_os == NULL) {
        return(NULL);
    }
    new_os->type = os->type;

    switch (os->type) {
        case INTEGER_TYPE:
            new_os->sl_value = os->sl_value;
            break;
        case COUNTER_TYPE:
        case GAUGE_TYPE:
        case TIME_TICKS_TYPE:
            new_os->ul_value = os->ul_value;
            break;
        case OCTET_PRIM_TYPE:
        case IP_ADDR_PRIM_TYPE:
        case OPAQUE_PRIM_TYPE:
            new_os->os_value = CloneOctetString(os->os_value);
            break;
        case NULL_TYPE:
            break;
        case OBJECT_ID_TYPE:
            new_os->oid_value = CloneOID(os->oid_value);
            break;
        case SEQUENCE_TYPE:
            break;
#ifdef SR_SNMPv2_PDU
        case COUNTER_64_TYPE:
            new_os->uint64_value = CloneUInt64(os->uint64_value);
            break;

#endif /* SR_SNMPv2_PDU */
        default:
            free((char *)new_os);
            new_os = NULL;
    }

    return(new_os);
}

/* this routine compares two octet strings (which cannot be NULL pointers)
 * that will have the length encoded when used as an index
 * and returns zero if they are equal in length and values, and a "difference"
 * value similar to strcmp()'s if they are not. */
int
CmpOctetStringsWithLen(o1, o2)
const OctetString *o1;
const OctetString *o2;
{
    SR_INT32 i, n;

    if (o1->length != o2->length) {
	return o1->length - o2->length;
    }
    n = MIN(o1->length, o2->length);
    for(i = 0; i < n; i++) {
	if(o1->octet_ptr[i] != o2->octet_ptr[i]) {
	    return o1->octet_ptr[i] - o2->octet_ptr[i];
	}
    }
    return o1->length - o2->length;
}

/* this routine compares two octet strings (which cannot be NULL pointers)
 * and returns zero if they are equal in length and values, and a "difference"
 * value similar to strcmp()'s if they are not. */
int
CmpOctetStrings(o1, o2)
    const OctetString *o1;
    const OctetString *o2;
{
    SR_INT32 i, n;

    n = MIN(o1->length, o2->length);
    for(i = 0; i < n; i++) {
	if(o1->octet_ptr[i] != o2->octet_ptr[i]) {
	    return o1->octet_ptr[i] - o2->octet_ptr[i];
	}
    }
    return o1->length - o2->length;
}

/*
 * CloneOctetString:
 *
 * The former name (pre-snmp12.1.0.0 release) was clone_octetstring().
 */
OctetString *
CloneOctetString(os_ptr)
    const OctetString *os_ptr;
{
    if (os_ptr == NULL) {
	return NULL;
    } else {
	return (MakeOctetString(os_ptr->octet_ptr, os_ptr->length));
    }
}

/*
 * MakeOID: This routine produces a library Object Identifier
 *          construct from a passed sub-identifier array and length.
 *          The sub-identifier array is an array of unsigned long
 *          integers, with each element of the array corresponding to
 *          the value of each integer at each position in the
 *          dot-notation display of an object identifier. For example,
 *          the Object Identifier 1.3.6.1.2.1.1 would have a value 1
 *          in sid_array[0], 3 in sid_array[1], etc. length is the
 *          number of sub-identifier elements present in the array.
 *          The construct returned can be free'ed by passing the
 *          pointer to FreeOID(). This is usually unnecessary as the
 *          construct is most often passed to another library routine
 *          for inclusion in a large ASN.1 construct and that library
 *          routine's free'ing counterpart will do the memory
 *          recovery.
 *
 * The former name (pre-snmp12.1.0.0 release) was make_oid().
 */

OID            *
MakeOID(sid, length)
    const SR_UINT32 sid[];
    SR_INT32        length;
{
    OID            *oid;
    int             i;

    if (length > MAX_OID_SIZE) {
	DPRINTF((APPACKET, "MakeOID: Bad sid string length: %d\n", length));
	return (NULL);
    }

    if ((oid = 
	 (OID *) malloc(sizeof(OID) + length * sizeof(SR_UINT32))) == NULL) {
	DPRINTF((APWARN, "MakeOID, oid malloc failed\n"));
	return (NULL);
    }

    oid->oid_ptr = NULL;

    oid->oid_ptr = (SR_UINT32 *)(((char *) oid) +  sizeof(OID));

    if(sid != NULL) {
	for (i = 0; i < length; i++)
	  oid->oid_ptr[i] = (SR_UINT32) sid[i];
    }
    else {
	for (i = 0; i < length; i++)
	  oid->oid_ptr[i] = (SR_UINT32) 0;
    }
    oid->length = length;

    return (oid);
}				/* end of MakeOID */





/* ----------------------------------------------------------------------
 * MakeVarBindWithValue: This routine is used by a manager (ex: command-line
 *                       utilities) to make a varbind given the name and
 *                       value.  This routine should return a pointer to the
 *                       new varbind on success and NULL on failure.
 *
 * Arguments:
 *  i   (const OID *) object
 * 	the object class portion of the variable name.
 *  i   (const OID *) instance
 *	the object instance portion of the variable name.
 *  i	(int) type
 *      the ASN.1 type of this variable.
 *  i	(void *) value 
 *      the value for this variable.  The type of this value depends
 *	on the type argument (above).
 *  o	(VarBind *) (return value) *
 * 	returns: the newly created varbind on success, NULL on failure.
 *
 * Notes:
 *    .  The caller is responsible for freeing the variable binding returned.
 *    .  object and instance are copied.
 */
VarBind *
MakeVarBindWithValue(
    const OID      *object,
    const OID      *instance,
    int             type,
    void           *value)
{
    FNAME("MakeVarBindWithValue")
    VarBind        *vb;

    if (object == NULL) {
	DPRINTF((APWARN, "%s: Null object OID.\n", Fname));
	return (NULL);
    }

    if ((vb = (VarBind *) malloc(sizeof(VarBind))) == NULL) {
	DPRINTF((APWARN, "%s: vb malloc\n", Fname));
	return (NULL);
    }
#ifdef SR_CLEAR_MALLOC
    memset((char *) vb, 0, sizeof(VarBind));
#endif	/* SR_CLEAR_MALLOC */

    if (instance != NULL) {
	vb->name = CatOID(object, instance);	/* name */
    }
    else {
	vb->name = CloneOID(object);	/* name */
    }
    if (vb->name == NULL) {
	DPRINTF((APWARN, "%s: cannot malloc vb->name.\n", Fname));
	FreeVarBind(vb);
	return (NULL);
    }
    vb->name_size = FindLenOID(vb->name);
    vb->value.type = (short)type;
    
    /* combine the Choices from simple and application */
    switch (type) {
        case COUNTER_TYPE:		/* includes COUNTER_32_TYPE */
        case GAUGE_TYPE:		/* includes GAUGE_32_TYPE */
        case TIME_TICKS_TYPE:
	    vb->value.ul_value = *((SR_UINT32 *) value);
	    vb->value_size = FindLenUInt(vb->value.ul_value);
	    break;

        case INTEGER_TYPE:		/* includes INTEGER_32_TYPE */
	    vb->value.sl_value = *((SR_INT32 *) value);
	    vb->value_size = FindLenInt(vb->value.sl_value);
	    break;

        case OBJECT_ID_TYPE:
	    vb->value.oid_value = (OID *) value;
	    vb->value_size = FindLenOID(vb->value.oid_value);
	    break;

        case OCTET_PRIM_TYPE:
        case IP_ADDR_PRIM_TYPE:
        case OPAQUE_PRIM_TYPE:
	    vb->value.os_value = (OctetString *) value;
	    vb->value_size = FindLenOctetString(vb->value.os_value);
	    break;

        case NULL_TYPE:
#ifdef SR_SNMPv2_PDU
        case NO_SUCH_OBJECT_EXCEPTION:
        case NO_SUCH_INSTANCE_EXCEPTION:
        case END_OF_MIB_VIEW_EXCEPTION:
#endif /* SR_SNMPv2_PDU */
	    vb->value_size = 2;
	    break;

#ifdef SR_SNMPv2_PDU
        case COUNTER_64_TYPE:
	    vb->value.uint64_value = (UInt64 *) value;
	    vb->value_size = FindLenUInt64(vb->value.uint64_value);
	    break;

#endif /* SR_SNMPv2_PDU */

        default:
	    DPRINTF((APWARN, "%s: Illegal type: 0x%x\n", Fname, type));
	    FreeVarBind(vb);
	    return (NULL);
    }				/* end of switch */

    return (vb);
}				/* MakeVarBindWithValue() */


/*
 * MakeVarBindWithNull: This routine is used by a manager (ex: command-line
 *                      utilities) to make a varbind given the name
 *                      The type field is set to NULL_TYPE and the value
 *                      field is set to NULL.  This should only be called
 *                      when doing a get, get-next, or get-bulk request.
 *                      This routine should return a pointer to the
 *                      new varbind on success and NULL on failure.
 *
 * arguments: object - the object class portion of the variable name.
 *            instance - the object instance portion of the variable name.
 *
 * returns: the newly created varbind on success, 
 *          NULL on failure.
 *
 */
VarBind        *
MakeVarBindWithNull(object, instance)
    const OID *object;
    const OID *instance;
{
    VarBind        *vb;
    if (object == NULL) {
	DPRINTF((APWARN, "MakeVarBindWithNull: Null object OID.\n"));
	return (NULL);
    }

    if ((vb = (VarBind *) malloc(sizeof(VarBind))) == NULL) {
	DPRINTF((APWARN, "MakeVarBindWithNull, vb malloc\n"));
	return (NULL);
    }
#ifdef SR_CLEAR_MALLOC
    memset((char *) vb, 0, sizeof(VarBind));
#endif	/* SR_CLEAR_MALLOC */

    if (instance != NULL) {
	vb->name = CatOID(object, instance);	/* name */
    }
    else {
	vb->name = CloneOID(object);	/* name */
    }
    if (vb->name == NULL) {
	DPRINTF((APWARN, "MakeVarBindWithNull: cannot malloc vb->name.\n"));
        free(vb);    
	return (NULL);
    }
    vb->name_size = FindLenOID(vb->name);
    vb->value.type = NULL_TYPE;

    vb->value_size = 2;		/* one for the type 0x05, and */
    /* one for the length 0x00 */

    return (vb);
}				/* MakeVarBindWithNull() */























/*
 * MakePdu:
 *
 *   This routine is called to create the initial header block for
 *   building the SNMP ASN.1 data structure, which upon completion is
 *   used to build the actual SNMP packet.  It returns a pointer to a
 *   malloc'ed data structure of type Pdu:
 *
 *   typedef struct _Pdu {
 *     OctetString *packlet;
 *     short type;
 *     union {
 *       NormPdu normpdu;
 *       TrapPdu trappdu;
 *     } u;
 *     VarBind *var_bind_list;
 *     VarBind *var_bind_end_ptr;
 *   } Pdu;
 *
 *   The type is one of GET_REQUEST_TYPE, GET_NEXT_REQUEST_TYPE,
 *   GET_RESPONSE_TYPE, SET_REQUEST_TYPE, GET_BULK_REQUEST,
 *   INFORM_REQUEST_TYPE, SNMPv2_TRAP_TYPE, or TRAP_TYPE.  The request
 *   ID is the id number assigned to the particular packet by the
 *   application.  Since the application is UDP based, retry is solely
 *   controlled by the network management application.  The
 *   error_status is set to other than 0 in GET_RESPONSE_TYPE only to
 *   indicate that this response is in reply to a bad request.  The
 *   error_index is used only by GET_RESPONSE_TYPE and points to the
 *   var-bind entry in the PDU that offends the agent.  The enterprise
 *   is used by the TRAP_TYPE pdu and is an object identifier
 *   associated with the trap generating entity.  The agent_addr is
 *   used by the TRAP_TYPE pdu and consists of an octet string
 *   containing the IP address of the trap generating entity.  The
 *   generic_trap and specific_trap are used by the TRAP_TYPE pdu and
 *   consist of integers that indicate which type of trap this PDU
 *   represents.  The time_ticks is the TRAP_TYPE emiting entity's
 *   sense of time since the agent has restarted.
 *
 *   This routine is called once for each packet to be generated.  The
 *   pdu pointer is then passed to the routine LinkVarBind repeatedly
 *   to string var_binds into the packet.  Build_pdu() is then called
 *   to perform the ASN.1 encoding of the pdu and place that result in
 *   the pdu pointer's *packlet field.  After the packlet has been
 *   wrapped in an authentication envelope, it is free'ed by passing
 *   the pointer to FreePdu().
 *
 *
 *   The former name (pre-snmp12.1.0.0 release) was make_pdu().
 */

Pdu *
MakePdu(type, request_id, error_status_or_non_repeaters,
	 error_index_or_max_repetitions, enterprise,
	 agent_addr, generic_trap, specific_trap, time_ticks)
    int             type;
    SR_INT32    request_id;
    SR_INT32        error_status_or_non_repeaters;
    SR_INT32        error_index_or_max_repetitions;
    OID            *enterprise;	/* trap stuff */
    OctetString    *agent_addr;	/* trap stuff */
    SR_INT32        generic_trap;	/* trap stuff */
    SR_INT32        specific_trap;	/* trap stuff */
    SR_UINT32   time_ticks;	/* trap stuff */
{
    Pdu            *pdu_ptr;

    if ((pdu_ptr = (Pdu *) malloc(sizeof(Pdu))) == NULL) {
	DPRINTF((APWARN, "MakePdu, pdu_ptr malloc\n"));
	return (NULL);
    }

#ifdef SR_CLEAR_MALLOC
    memset(pdu_ptr, 0, sizeof(Pdu));
#endif	/* SR_CLEAR_MALLOC */
    pdu_ptr->type = type;

    /* sanity check type */
    switch (type) {
    case GET_REQUEST_TYPE:
    case GET_NEXT_REQUEST_TYPE:
    case GET_RESPONSE_TYPE:
    case SET_REQUEST_TYPE:
    case INFORM_REQUEST_TYPE:
    case SNMPv2_TRAP_TYPE:
    case REPORT_TYPE:
	pdu_ptr->u.normpdu.request_id = request_id;
	pdu_ptr->u.normpdu.error_status = error_status_or_non_repeaters;
	pdu_ptr->u.normpdu.error_index = error_index_or_max_repetitions;
	break;

    case GET_BULK_REQUEST_TYPE:
	pdu_ptr->u.bulkpdu.request_id = request_id;
	pdu_ptr->u.bulkpdu.non_repeaters = error_status_or_non_repeaters;
	pdu_ptr->u.bulkpdu.max_repetitions = error_index_or_max_repetitions;
	break;

    case TRAP_TYPE:
	/* sanity check octet strings */
	if (enterprise == NULL) {
	    DPRINTF((APPACKET, "MakePdu: Trap type but no Enterprise.\n"));
	    FreePdu(pdu_ptr);
	    NULLIT(pdu_ptr);
	    return (NULL);
	}
	if (agent_addr == NULL) {
	    DPRINTF((APPACKET, "MakePdu: Trap type but no Agent_addr.\n"));
	    FreePdu(pdu_ptr);
	    NULLIT(pdu_ptr);
	    return (NULL);
	}
	pdu_ptr->u.trappdu.enterprise = enterprise;
	pdu_ptr->u.trappdu.agent_addr = agent_addr;
	pdu_ptr->u.trappdu.generic_trap = generic_trap;
	pdu_ptr->u.trappdu.specific_trap = specific_trap;
	pdu_ptr->u.trappdu.time_ticks = time_ticks;
	break;

    default:
	DPRINTF((APPACKET, "MakePdu: illegal type: 0x%x\n", type));
	FreePdu(pdu_ptr);
	NULLIT(pdu_ptr);
	return (NULL);
    };				/* end of switch */

    return (pdu_ptr);
}				/* end of make_normpdu */


/*
 * LinkVarBind:
 *
 *   This routine adds the varbind entry created by a call to
 *   make_varbind() to a PDU previously started by a call to MakePdu()
 *   to flesh out the PDU.  This should only be called once for each
 *   varbind_ptr being associated with the PDU as it is this
 *   association that is used to reclaim memory when the PDU is
 *   free'ed.
 *
 *   The former name (pre-snmp12.1.0.0 release) was link_varbind().
 */

/*
 * Thread a vb_ptr onto the end of the list
 */
short
LinkVarBind(pdu_ptr, vb_ptr)
    Pdu            *pdu_ptr;
    VarBind        *vb_ptr;
{
    if (vb_ptr == NULL) {
	DPRINTF((APPACKET, "LinkVarBind: Trying to add Null Variable\n"));
	return (-1);
    }

    /* first variable? */
    if (pdu_ptr->var_bind_end_ptr == NULL) {
	pdu_ptr->var_bind_list = vb_ptr;
	pdu_ptr->var_bind_end_ptr = vb_ptr;
	return (0);
    }

    /* nope, so just add to end of list and update end ptr */
    pdu_ptr->var_bind_end_ptr->next_var = vb_ptr;
    pdu_ptr->var_bind_end_ptr = vb_ptr;
    return (0);
}				/* end of LinkVarBind */

/*
 * MakeOctetStringFromText:
 *
 *   This routine is used to create a library format octet string data
 *   structure for use by calls to make_varbind() and
 *   MakeAuthentication() from text strings.
 *
 * The former name (pre-snmp12.1.0.0 release) was make_octet_from_text().
 */

OctetString *
MakeOctetStringFromText(text_str)
    const char *text_str;
{
    OctetString *os;
    SR_INT32 ol, i, j;
    unsigned char *op;

    os = MakeOctetString(NULL, (text_str == NULL)? 0 : strlen(text_str));
    if (os == NULL) {
        return NULL;
    }
    if (os->length == 0) {
        return os;
    }

    ol = os->length - 1;
    op = os->octet_ptr;

    for (i = 0, j = 0; i < ol; i++, j++) {
        if (text_str[i] == '\\') {

            switch (text_str[i+1]) {
                case 'n':
                    op[j] = '\n';
                    i++;
                    os->length--;
                    break;
                case 'r':
                    op[j] = '\r';
                    i++;
                    os->length--;
                    break;
                case 't':
                    op[j] = '\t';
                    i++;
                    os->length--;
                    break;	
                default:
                    op[j] = text_str[i+1];
                    i++;
                    os->length--;
                    break;
            }
        } else {
            op[j] = text_str[i];
        }
    }
    op[j] = text_str[i];

    return os;
}


/*
 * MakeOctetStringFromHex:
 *
 *   This routine is used to create a library format octet string data
 *   structure for use by calls to make_varbind() and
 *   MakeAuthentication() from hex text strings.  The format of these
 *   strings is a hex value (1 or more hex digits, upper or lower
 *   case) followed by a space, with more hex values and spaces to
 *   finish out the string.  For example, to create an octet string of
 *   length two with a hex 15 in the first octet and a hex 3D in the
 *   second octet, the text string could be "15 3d" or "0x15 0x3D".
 *   The OctetString construct returned by this call can be free'ed by
 *   a call to FreeOctetString.  This is usually unnecessary as the
 *   construct is most often passed to another library routine for
 *   inclusion in a larger ASN.1 construct and that library's free
 *   counterpart will take care of memory recovery.
 *
 * The former name (pre-snmp12.1.0.0 release) was make_octet_from_hex().
 */
OctetString *
MakeOctetStringFromHex(text_str)
    const char *text_str;
{
    OctetString    *os_ptr;

    SR_UINT32       value;
    short           i;
    const char     *temp_ptr;
    int             temp_len;

    if(text_str == NULL) {
	return NULL;
    }
    temp_len = ((int) strlen((char *) text_str) / 2) + 1;
    if ((os_ptr = MakeOctetString(NULL, temp_len)) == NULL) {
	DPRINTF((APWARN, "MakeOctetStringFromHex, os_ptr MakeOctetString\n"));
	return (NULL);
    }
    os_ptr->length = ((SR_INT32)0);

    i = 0;
    temp_ptr = text_str;

    /* eat any leading blank characters */
    while ((temp_ptr[0] == ' ') || (temp_ptr[0] == '\t')) {
        temp_ptr++;
    }

    while (1) {
        if (*(temp_ptr) == '\0') {
	  break;
	}

        if ((temp_ptr[0] == '0') && (temp_ptr[1] == 'x')) {
            temp_ptr += 2;
            if (*(temp_ptr) == '\0') {
	      break;
	    }
        }

        value = 0;
        while (1) {
            if ((temp_ptr[0] >= '0') && (temp_ptr[0] <= '9')) {
                value = (value * 16) + temp_ptr[0] - '0';
            } else
            if ((temp_ptr[0] >= 'a') && (temp_ptr[0] <= 'f')) {
                value = (value * 16) + temp_ptr[0] - 'a' + 10;
            } else
            if ((temp_ptr[0] >= 'A') && (temp_ptr[0] <= 'F')) {
                value = (value * 16) + temp_ptr[0] - 'A' + 10;
            } else {
                break;
            }
            temp_ptr++;
        }

	/* plug value into the octet string */
	os_ptr->octet_ptr[i] = (unsigned char) value;
	i++;

        /* scan for next octet.  can be separated by spaces or one colon */
        if (temp_ptr[0] == ':') {
            temp_ptr++;
            continue;
        }
        while ((temp_ptr[0] == '\t') ||
               (temp_ptr[0] == ' ')) {
            temp_ptr++;
        }
        if ((temp_ptr[0] >= '0') && (temp_ptr[0] <= '9')) continue;
        if ((temp_ptr[0] >= 'a') && (temp_ptr[0] <= 'f')) continue;
        if ((temp_ptr[0] >= 'A') && (temp_ptr[0] <= 'F')) continue;
        if (*(temp_ptr) == '\0') {
	  break;
	}
        FreeOctetString(os_ptr);
        return NULL;
    }

    if (i == 0) {
        FreeOctetString(os_ptr);
        return NULL;
    }

    os_ptr->length = i;

    return (os_ptr);
}


/*
 * MakeOctetStringFromFormattedHex:
 *
 *   This routine is used to create a library format octet string data
 *   structure for use by calls to make_varbind() and
 *   MakeAuthentication() from hex text strings.  
 *  
 *   This routine differs from MakeOctetStringFromHex() by requiring
 *   that the input string begins with "0x" to disambiguate the value
 *   from an ascii HEX string.
 *
 *   The remainder of the input string must consist of a hex value, 
 *   with 1 or more hex digits delimited with by spaces or a colon.
 */
OctetString *
MakeOctetStringFromFormattedHex(text_str)
    const char *text_str;
{
    OctetString    *os_ptr;

    SR_UINT32       value;
    short           i;
    const char     *temp_ptr;
    int             temp_len;

    if(text_str == NULL) {
	return NULL;
    }
    temp_len = ((int) strlen((char *) text_str) / 2) + 1;
    if ((os_ptr = MakeOctetString(NULL, temp_len)) == NULL) {
	DPRINTF((APWARN, "MakeOctetStringFromHex, os_ptr MakeOctetString\n"));
	return (NULL);
    }
    os_ptr->length = ((SR_INT32)0);

    i = 0;
    temp_ptr = text_str;

    /* eat any leading blank characters */
    while ((temp_ptr[0] == ' ') || (temp_ptr[0] == '\t')) {
        temp_ptr++;
    }

    /* string must begin with '0x' for formatted hex */
    if ((temp_ptr[0] == '0') && (temp_ptr[1] == 'x')) {
        temp_ptr += 2;
    } else {
        /* input string is not formatted hex */
        FreeOctetString(os_ptr);
        return NULL;
    }    

    while (1) {
        if (*(temp_ptr) == '\0') {
	  break;
	}

        value = 0;
        while (1) {
            if ((temp_ptr[0] >= '0') && (temp_ptr[0] <= '9')) {
                value = (value * 16) + temp_ptr[0] - '0';
            } else
            if ((temp_ptr[0] >= 'a') && (temp_ptr[0] <= 'f')) {
                value = (value * 16) + temp_ptr[0] - 'a' + 10;
            } else
            if ((temp_ptr[0] >= 'A') && (temp_ptr[0] <= 'F')) {
                value = (value * 16) + temp_ptr[0] - 'A' + 10;
            } else {
                break;
            }
            temp_ptr++;
        }

	/* plug value into the octet string */
	os_ptr->octet_ptr[i] = (unsigned char) value;
	i++;

        /* scan for next octet.  can be separated by spaces or one colon */
        if (temp_ptr[0] == ':') {
            temp_ptr++;
            continue;
        }
        while ((temp_ptr[0] == '\t') ||
               (temp_ptr[0] == ' ')) {
            temp_ptr++;
        }
        if ((temp_ptr[0] >= '0') && (temp_ptr[0] <= '9')) continue;
        if ((temp_ptr[0] >= 'a') && (temp_ptr[0] <= 'f')) continue;
        if ((temp_ptr[0] >= 'A') && (temp_ptr[0] <= 'F')) continue;
        if (*(temp_ptr) == '\0') {
	  break;
	}
        FreeOctetString(os_ptr);
        return NULL;
    }

    if (i == 0) {
        FreeOctetString(os_ptr);
        return NULL;
    }

    os_ptr->length = i;

    return (os_ptr);
}

/*
 * MakeOIDFromHex:
 *
 *   This routine is used to create a library format Object Identifier
 *   data structure for use by calls to MakeVarBind() and
 *   MakeAuthentication() from hex text strings.  The format of these
 *   strings is a hex value (1 or more hex digits, upper or lower
 *   case) followed by a space, with more hex values and spaces to
 *   finish out the string.  For example, to create an Object
 *   Identifier consisting of three sub-identifiers (say, 1.21.51)
 *   text string could be "1 15 3d" or "0x1 0x15 0x3D".  The Object
 *   Identifier construct returned by this call can be free'ed by a
 *   call to FreeOID.  This is usually unnecessary as the construct is
 *   most often passed to another library routine for inclusion in a
 *   larger ASN.1 construct and that library's free counterpart will
 *   take care of memory recovery.
 *
 * The former name (pre-snmp12.1.0.0 release) was make_obj_id_from_hex().
 */

OID *
MakeOIDFromHex(text_str)
    const unsigned char  *text_str;
{
    OID            *oid_ptr;
    int             i;
    const unsigned char *temp_ptr;

    SR_UINT32   value;

    if ((oid_ptr =
	 MakeOID(NULL, ((int)strlen((char *) text_str) / 2) + 1)) == NULL) {
	DPRINTF((APWARN, "MakeOIDFromHex, oid_ptr MakeOID failed\n"));
	return (NULL);
    }

    for (i = 0, temp_ptr = text_str; (i < 40) && (temp_ptr[0] != '\0'); i++) {
        if ((temp_ptr[0] == '0') && (temp_ptr[1] == 'x')) {
            temp_ptr += 2;
        }
        value = 0;
        while (1) {
            if ((temp_ptr[0] >= '0') && (temp_ptr[0] <= '9')) {
                value = (value * 16) + temp_ptr[0] - '0';
                temp_ptr++;
                continue;
            } else
            if ((temp_ptr[0] >= 'a') && (temp_ptr[0] <= 'f')) {
                value = (value * 16) + temp_ptr[0] - 'a' + 10;
                temp_ptr++;
                continue;
            } else
            if ((temp_ptr[0] >= 'A') && (temp_ptr[0] <= 'F')) {
                value = (value * 16) + temp_ptr[0] - 'A' + 10;
                temp_ptr++;
                continue;
            } else
            if (temp_ptr[0] == '\0') {
                break;
            }
            if (temp_ptr[0] != ' ') {
                FreeOID(oid_ptr);
                return NULL;
            } else {
                break;
            }
        }

	oid_ptr->oid_ptr[i] = value;

	while (temp_ptr[0] == ' ') {
	    temp_ptr++;
        }
    }

    oid_ptr->length = i;
    return (oid_ptr);
}


 /*
  * MakeLenOID: MakeLenOID allocates space for the incoming oid plus one to 
  *             contain the length. The length is the first element of the 
  *             oid, and the rest is copied from the incoming oid.  The new 
  *             oid is then returned.
  *
  * The former name (pre-snmp12.1.0.0 release) was make_len_oid().
  */
OID            *
MakeLenOID(oida)
    const OID     *oida;
{
    OID            *oid;
    int             i, length;

    if (oida == (OID *) NULL) {
	DPRINTF((APWARN, "MakeLenOID: passed in null oid\n"));
	return (NULL);
    }

    if (((length = oida->length) + 1) > MAX_OID_SIZE) {
	DPRINTF((APPACKET, "MakeLenOID: Too long: %d\n", length + 1));
	return (NULL);
    }

    if (length == 0) {
	DPRINTF((APPACKET, "MakeLenOID: Zero length OID\n"));
	return (NULL);
    }

    if ((oid = MakeOID(NULL, length + 1)) == NULL) {
	DPRINTF((APWARN, "MakeLenOID: cannot malloc oid\n"));
	return (NULL);
    }

    oid->oid_ptr[0] = length;

    for (i = 0; i < length; i++) {
	oid->oid_ptr[i + 1] = oida->oid_ptr[i];
    }

    oid->length = length + 1;

    return (oid);
}

 /*
  * MakeOIDSubLen: allocates space for the incoming oid minus one.  The
  *                first element is ignored, and a new oid is made equal to 
  *                all the remaining values. This new oid is then returned.
  *
  * The former name (pre-snmp12.1.0.0 release) was make_oid_sub_len().
  */
OID            *
MakeOIDSubLen(oida)
    const OID      *oida;
{
    OID            *oid;
    int             i, length;

    if (oida == (OID *) NULL) {
	DPRINTF((APPACKET, "MakeOIDSubLen:  A null OID was passed in\n"));
	return (NULL);
    }
    if (((length = oida->length) - 1) > MAX_OID_SIZE) {
	DPRINTF((APPACKET, "MakeOIDSubLen: Too long %d\n", length - 1));
	return (NULL);
    }

    if (length <= 1) {
	DPRINTF((APPACKET, "MakeOIDSubLen: Cannot make Zero-Length OID\n"));
	return (NULL);
    }

    if ((oid = MakeOID(NULL, length-1)) == NULL) {
	DPRINTF((APWARN, "MakeOIDSubLen: cannot malloc oid\n"));
	return (NULL);
    }

    for (i = 1; i < length; i++) {
	oid->oid_ptr[i - 1] = oida->oid_ptr[i];
    }

    oid->length = length - 1;
    return (oid);
}



/*
 * CloneVarBindList: clones the incoming varbind list and returns a pointer 
 *                   to the new copy of the varbind list.
 *
 * Arguments: in_vb - The varbind list to be cloned.
 *
 * Returns: Pointer to the new copy of the varbind list on success,
 *          NULL on failure.
 *
 * The former name (pre-snmp12.1.0.0 release) was clone_varbind_list().
 */
VarBind        *
CloneVarBindList(in_vb)
    const VarBind *in_vb;
{

    VarBind        *out_vb = NULL;
    VarBind        *return_vb = NULL;
    VarBind        *cur_vb = NULL;
    const VarBind  *vb = NULL;

    if (in_vb == NULL) {
	DPRINTF((APPACKET, "CloneVarBindList: value passed in is NULL\n"));
	return (NULL);
    }
    vb = in_vb;

    while (vb != NULL) {
	if ((out_vb = CloneVarBind(vb)) == NULL) {
	    DPRINTF((APWARN, "CloneVarBindList:  NULL return from CloneVarBind.\n"));
	    FreeVarBindList(return_vb);
	    return_vb = NULL;
	    return (NULL);
	}
	/*
	 * We want return_vb to point to the beginning of the varbind list.
	 */
	if (return_vb == NULL) {
	    return_vb = out_vb;
	}
	if (cur_vb == NULL) {
	    cur_vb = out_vb;
	}
	else {
	    cur_vb->next_var = out_vb;
	    cur_vb = out_vb;
	}

	vb = vb->next_var;
    }				/* while (vb != NULL) */

    return (return_vb);

}				/* CloneVarBindList() */

/*
 * CloneVarBind: clones the incoming varbind and returns a pointer to
 *               the new copy of the varbind.
 *
 * Arguments: in_vb - The varbind to be cloned.
 *
 * Returns: Pointer to the new copy of the varbind on success,
 *          NULL on failure.
 *
 * The former name (pre-snmp12.1.0.0 release) was clone_varbind().
 */
VarBind        *
CloneVarBind(in_vb)
    const VarBind *in_vb;
{
    FNAME("CloneVarBind")
    VarBind        *out_vb;
    const VarBind  *vb;


    if (in_vb == NULL) {
	DPRINTF((APPACKET, "%s: value passed in is NULL\n", Fname));
	return (NULL);
    }
    vb = in_vb;

    if ((out_vb = (VarBind *) malloc(sizeof(VarBind))) == NULL) {
	DPRINTF((APWARN, "%s: Cannot malloc out_vb\n", Fname));
	return (NULL);
    }
#ifdef SR_CLEAR_MALLOC
    memset(out_vb, 0, sizeof(VarBind));
#endif	/* SR_CLEAR_MALLOC */

    out_vb->data_length = vb->data_length;
    out_vb->name_size = vb->name_size;
    out_vb->value_size = vb->value_size;
    out_vb->value.type = vb->value.type;
    out_vb->next_var   = NULL;



       if ((out_vb->name = (OID *) CloneOID(vb->name)) == NULL) {
   	   DPRINTF((APWARN, "%s: Unable to clone vb->name\n", Fname));
 	   FreeVarBind(out_vb);
	   out_vb = NULL;
	   return (NULL);
       }


    /*
     * Assign value for the varbind
     */

    switch (vb->value.type) {

    case COUNTER_TYPE:
    case GAUGE_TYPE:
    case TIME_TICKS_TYPE:	/* unsigned integers */
	out_vb->value.ul_value = vb->value.ul_value;
	break;

    case INTEGER_TYPE:		/* signed integers, including INTEGER_32 */
	out_vb->value.sl_value = vb->value.sl_value;
	break;

    case OBJECT_ID_TYPE:	/* OIDs */
	if ((out_vb->value.oid_value =
	     (OID *) CloneOID(vb->value.oid_value)) == (OID *) NULL) {
	    DPRINTF((APWARN,
		   "%s: Unable to clone vb->value.oid_value\n", Fname));
	    FreeVarBind(out_vb);
	    out_vb = NULL;
	    return (NULL);
	}
	break;

#ifdef SR_SNMPv2_PDU
    case COUNTER_64_TYPE:	/* big unsigned integer */
	if ((out_vb->value.uint64_value =
	     CloneUInt64(vb->value.uint64_value)) ==
	    (UInt64 *) NULL) {
	    DPRINTF((APWARN,
		"%s: Unable to clone vb->value.uint64_value\n", Fname));
	    FreeVarBind(out_vb);
	    out_vb = NULL;
	    return (NULL);
	}
	break;
              
#endif /* SR_SNMPv2_PDU */

    case OCTET_PRIM_TYPE:
    case IP_ADDR_PRIM_TYPE:
    case OPAQUE_PRIM_TYPE:
	if ((out_vb->value.os_value =
	     (OctetString *) CloneOctetString(vb->value.os_value)) ==
	    (OctetString *) NULL) {
	    DPRINTF((APWARN,
		     "%s: Unable to clone vb->value.os_value\n", Fname));
	    FreeVarBind(out_vb);
	    out_vb = NULL;
	    return (NULL);
	}
	break;
    case NULL_TYPE:
#ifdef SR_SNMPv2_PDU
    case NO_SUCH_OBJECT_EXCEPTION:
    case NO_SUCH_INSTANCE_EXCEPTION:
    case END_OF_MIB_VIEW_EXCEPTION:
#endif /* SR_SNMPv2_PDU */
	break;
    default:
	DPRINTF((APWARN,
		 "%s: Illegal type: 0x%x\n", Fname, out_vb->value.type));
	FreeVarBind(out_vb);
	out_vb = NULL;
	return (NULL);
    }				/* end of switch */

    return (out_vb);

}				/* CloneVarBind() */

#ifdef SR_SNMPv2_PDU
/*
 * CloneUInt64:
 *
 *   The former name (pre-snmp12.1.0.0 release) was clone_uint64().
 */
UInt64         *
CloneUInt64(value)
    const UInt64 *value;
{

    UInt64         *return_val;

    return_val = (UInt64 *) NULL;

    if (value == (UInt64 *) NULL) {
	DPRINTF((APPACKET, "CloneUInt64: value passed in is NULL\n"));
	return (NULL);
    }

    if ((return_val = (UInt64 *) malloc(sizeof(UInt64))) == (UInt64 *) NULL) {
	DPRINTF((APWARN, "CloneUInt64: Cannot malloc return_val\n"));
	return (NULL);
    }

    return_val->big_end = value->big_end;
    return_val->little_end = value->little_end;

    return (return_val);

}				/* CloneUInt64() */
#endif /* SR_SNMPv2_PDU */

/*
 * Make a null terminated C string from an OctetString. 
 */
char *
MakeTextFromOctetString(os)
    const OctetString *os;
{
    char *text;

    if (os == NULL) {
	return NULL;
    }

    if (os->octet_ptr == NULL) {
	return NULL;
    }

    if ((text = (char *) malloc(os->length + 1)) == NULL) {
	return NULL;
    }

    if (os->length > 0) {
        memcpy(text, (char *)os->octet_ptr, (size_t)os->length);
    }

    text[os->length] = '\0';
    return text;
}



/*
 * MakeCounter64: This routine mallocs the memory for a counter64 variable
 *                 type, and inserts the passed long int value into the
 *                 bottom 32 bits of the structure.
 *
 */

UInt64 *
MakeCounter64(SR_UINT32 value)
{

  UInt64 *new64;

    if((new64 = (UInt64 *)malloc(sizeof(UInt64))) == NULL) {  
	DPRINTF((APWARN, "MakeCounter64, counter64 malloc failed\n"));
        return(NULL);
     }

     new64->big_end = 0;
     new64->little_end = value;

     return(new64);

}				/* end of function  */

/*
 * MakeUInt64: This routine mallocs the memory for a UInt64 variable
 *             type, inserts the first parameter (low) into the
 *             low order 32 bits of the structure, and inserts the
 *             second parameter (high) into the high order 32 bits
 */
UInt64 *
MakeUInt64(SR_UINT32 low, SR_UINT32 high)
{
    UInt64 *new64;

    if((new64 = (UInt64 *)malloc(sizeof(UInt64))) == NULL) {  
	DPRINTF((APWARN, "MakeUInt64: malloc failed\n"));
        return(NULL);
     }

     new64->big_end = high;
     new64->little_end = low;

     return new64;

}				/* end of function  */


/*
 *  MakeUInt64FromText()
 *
 *  Function:  Converts a Base-10 string representation of a UInt64 and
 *             returns a UInt64 value.
 *
 *  Inputs:    string - specifies the Base-10 representation of the UInt64,
 *                      e.g.; string values from "0" to "18446744073709551615".
 *
 *             result - specifies a UInt64 in which to return the parsed value.
 *                      This may be specified as NULL.  In this case, the
 *                      function will allocate a UInt64 that must be freed
 *                      by the caller.
 *
 *  Outputs:   On failure, returns NULL.  On success, returns a UInt64 value
 *             for the parsed string.  If a UInt64 is passed in, this function
 *             will return the pointer to the passed-in UInt64.
 *
 *  Notes:     This function attempts to carefully error-check the argument.
 *
 *             This function depends on the Counter64 math functions in the
 *             file  devkit/sr_math.c.  These seem to work.
 *
 *             This function currently only converts Base-10 representations
 *             of UInt64 values.  It could be extended to convert other bases
 *             such as hexadecimal representations.
 */
UInt64 *
MakeUInt64FromText(const char *string, UInt64 *result)
{
    int     i, len;
    char    buffer[2], *local_string = NULL;
    UInt64  current_digit, power_of_base, base;
    UInt64  *osum = NULL, *sum = NULL, *temp = NULL;

    /* --- Initialize a passed in UInt64 --- */
    if (result != NULL) {
        result->little_end = 0;
        result->big_end    = 0;
    }

    /* --- Return if no argument --- */
    if (string == NULL) {
        sum = NULL;
        goto done;
    }

    /* --- Allocate a buffer for sscanf to parse into --- */
    len = strlen(string) + 1;
    local_string = (char *) malloc(len);
    if (local_string == NULL) {
        sum = NULL;
        goto done;
    }
    memset(local_string, 0x00, len);

    /* --- Use sscanf to remove leading/trailing blanks --- */
    if (sscanf((char *)string, "%s", local_string) != 1) {
        sum = NULL;
        goto done;
    }

    /* --- Return if wrong string length --- */
    len = strlen(local_string);
    if ((len < 1) || (len > 20)) {
        sum = NULL;
        goto done;
    }

    /* --- Return if the string is not all digits --- */
    for (i = 0; i < len; i++) {
        if (!isdigit(local_string[i])) {
            sum = NULL;
            goto done;
        }
    }

    /*
     *  Initialize a UInt64 with the mathematical base for the argument
     *  string.  Initialize another UInt64 to the power of the base by
     *  which the current digit should be multiplied.
     */
    base.little_end = 10;
    base.big_end    = 0;
    power_of_base.little_end = 1;
    power_of_base.big_end    = 0;

    /* --- Initialize the sum --- */
    sum = MakeUInt64((SR_UINT32) 0, (SR_UINT32) 0);

    /*
     *  Iterate through the digits in the argument string, starting with
     *  the least significant digit to the most significant digit.  Pare
     *  of each digit, convert it to a UInt64, multiply it by the proper
     *  power of 10, and add it to the sum.
     */
    for (i = len-1; i >= 0; i--) {

        /* --- Extract the current digit --- */
        buffer[0] = local_string[i];
        buffer[1] = '\0';
        current_digit.little_end = atoi(buffer);
        current_digit.big_end    = 0;

        /* --- Multiply by the current power of 10 --- */
        temp = Counter64Multiply(&power_of_base, &current_digit);

        /* --- The current digit to the sum --- */
        osum = sum;
        sum  = Counter64Add(temp, osum);

        /* --- Free the intermediate UInt64 values --- */
        free(osum);
        free(temp);

        /* --- Raise power of 10 to multiply by the next digit --- */
        temp = Counter64Multiply(&power_of_base, &base);
        power_of_base.little_end = temp->little_end;
        power_of_base.big_end    = temp->big_end;

        /* --- Raise power of 10 to multiply by the next digit --- */
        free(temp);
    }

    /* --- Copy the results into the passed in UInt64 if desired --- */
    if (result != NULL) {
        result->little_end = sum->little_end;
        result->big_end    = sum->big_end;
        free(sum);
        sum = result;
    }

    /* --- Clean up and return --- */
done:
    if (local_string != NULL) {
        free(local_string);
    }

    return sum;
}   /* MakeUInt64FromText() */


/*
 * FindLastVarBind:
 *	Find the last VarBind in the given VarBind list.
 *
 * Returns:
 *	NULL - error or no varbind
 *      ptr  - to last VarBind in the VarBind list.
 *
 */
VarBind *
FindLastVarBind(VarBind *vb)
{
    VarBind *posVB;
   
    if (vb == NULL) {
        return NULL;
    }

    for (posVB = vb; posVB->next_var != NULL; posVB = posVB->next_var);

    return posVB;

}


/*
 * MakeVarBindWithObjectSyntax(): make a VarBind given an object syntax.
 * This is a simple wrapper around MakeVarBindWithValue().
 */
VarBind        *
MakeVarBindWithObjectSyntax(object, instance, value)
    const OID      *object;
    const OID      *instance;
    ObjectSyntax   *value;
{
    FNAME("MakeVarBindWithObjectSyntax")

    /* combine the Choices from simple and application */
    switch (value->type) {
        case COUNTER_TYPE:		/* includes COUNTER_32_TYPE */
        case GAUGE_TYPE:		/* includes GAUGE_32_TYPE */
        case TIME_TICKS_TYPE:
            return MakeVarBindWithValue(object, instance, 
                                        value->type, &(value->ul_value));

        case INTEGER_TYPE:		/* includes INTEGER_32_TYPE */
            return MakeVarBindWithValue(object, instance, 
                                        value->type, &(value->sl_value));

        case OBJECT_ID_TYPE:
            return MakeVarBindWithValue(object, instance, 
                                        value->type, value->oid_value);

        case OCTET_PRIM_TYPE:
        case IP_ADDR_PRIM_TYPE:
        case OPAQUE_PRIM_TYPE:
            return MakeVarBindWithValue(object, instance, 
                                        value->type, value->os_value);

        case NULL_TYPE:
#ifdef SR_SNMPv2_PDU
        case NO_SUCH_OBJECT_EXCEPTION:
        case NO_SUCH_INSTANCE_EXCEPTION:
        case END_OF_MIB_VIEW_EXCEPTION:
#endif /* SR_SNMPv2_PDU */
            return MakeVarBindWithValue(object, instance, 
                                        value->type, NULL);

#ifdef SR_SNMPv2_PDU
        case COUNTER_64_TYPE:
            return MakeVarBindWithValue(object, instance, value->type, 
                                        value->uint64_value);

#endif /* SR_SNMPv2_PDU */

        default:
	    DPRINTF((APWARN, "%s: Illegal type: 0x%x\n", Fname, value->type));
	    return (NULL);
    }				/* end of switch */
}




/*
 * OctetStringToTextString():  Returns a char string copy of the passed
 *      in OctetString.  The caller is responsible for freeing the
 *      memory allocated by this routine.
 */

char *
OctetStringToTextString(OctetString *os_ptr)
{
   char *newstring = NULL;
   int i = 0;
 
   FNAME("OctetStringToTextString")

   if (os_ptr == NULL ) {
      DPRINTF((APWARN, "%s:  Unable to create text string from NULL octet string\n", Fname));
      return NULL;
   }

   if (os_ptr->length < 0 ) {
      DPRINTF((APWARN, "%s: Octet string length < 0\n", Fname));
      return NULL;
   }

   newstring = (char *) malloc(os_ptr->length + 1);

   if (os_ptr->length == 0 ) {
      newstring[0] = '\0';
      return(newstring);
   }

   /* check that all characters are printable characters */

   for (i = 0; i < os_ptr->length; i++) {
       /* ASCII space is not a printable character in MSC */
       if (((!isprint((int) os_ptr->octet_ptr[i])) &&
            (os_ptr->octet_ptr[i] != 0x20) &&
            (os_ptr->octet_ptr[i] != 0x09) &&
            (os_ptr->octet_ptr[i] != 0x0a) &&
            (os_ptr->octet_ptr[i] != 0x0d)) &&
            (!((os_ptr->octet_ptr[i] == 0x00) && (i == os_ptr->length - 1)))) {
            DPRINTF((APWARN, "%s: Octet string contains non-printable character\n", Fname));
            return (NULL);
        }
    }

    for (i = 0; i < os_ptr->length; i++) {
      newstring[i] = os_ptr->octet_ptr[i];
    }

   newstring[i] = '\0';
 
   return(newstring);
 
}
