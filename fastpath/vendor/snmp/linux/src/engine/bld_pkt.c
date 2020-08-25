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

#include <errno.h>

#include <string.h>

#include "sr_snmp.h"
#include "diag.h"
SR_FILENAME





/*
 * BuildPdu:
 *
 *   This routine is called with the PDU pointer being used to
 *   create the PDU.  It traces down the structure of varbinds
 *   that has been added to it and builds the ASN.1 packet in the
 *   *packlet pointer of the PDU pointer's data structure.  At
 *   this point, PDU processing is complete and the structure is
 *   ready to be passed onto the authentication layers.
 *
 *   It is the main part of the packet builder.  It traverses the
 *   varbind list to determine the packet size and then calls the
 *   add_* routines to bind the item's constructs into the packet.
 *   The 'packlet' is an OctetString in pdu-ptr->packlet.
 *
 *   The former name (pre-snmp12.1.0.0 release) was build_pdu().
 */

short
BuildPdu(pdu_ptr)
    Pdu            *pdu_ptr;
{
    FNAME("BuildPdu")
    SR_INT32        varbindlen;
    SR_INT32        varbind_tot_len;
    SR_INT32        varbindlenlen;
    SR_INT32        datalen, temp_data_len;
    SR_INT32        lenlen;
    unsigned char  *working_ptr;
    SR_INT32        sequence_type;

    /* find out length of whole PDU */
           if ((varbindlen = FindLenVarBind(pdu_ptr->var_bind_list)) == -1) { 
       	       DPRINTF((APPACKET, "%s: varbindlen:\n", Fname));
	       return (-1);		/* abort */
           }
           if ((varbindlenlen = DoLenLen(varbindlen)) == -1) {
                DPRINTF((APPACKET, "%s: varbindlenlen:\n", Fname));
                return (-1);
           }

           /* calculate total varbind list length, sequence tag is 1 octet */
           varbind_tot_len = 1 + varbindlenlen + varbindlen;

    switch (pdu_ptr->type) {
        case GET_REQUEST_TYPE:
        case GET_NEXT_REQUEST_TYPE:
        case GET_RESPONSE_TYPE:
        case SET_REQUEST_TYPE:
#ifdef SR_SNMPv2_PDU
        case INFORM_REQUEST_TYPE:
        case SNMPv2_TRAP_TYPE:
        case REPORT_TYPE:
#endif /* SR_SNMPv2_PDU */
	    datalen = FindLenInt(pdu_ptr->u.normpdu.request_id) +
	        FindLenInt(pdu_ptr->u.normpdu.error_status) +
	        FindLenInt(pdu_ptr->u.normpdu.error_index) +
	        varbind_tot_len;
	    break;

#ifdef SR_SNMPv2_PDU
        case GET_BULK_REQUEST_TYPE:
	    datalen = FindLenInt(pdu_ptr->u.bulkpdu.request_id) +
	        FindLenInt(pdu_ptr->u.bulkpdu.non_repeaters) +
	        FindLenInt(pdu_ptr->u.bulkpdu.max_repetitions) +
	        varbind_tot_len;
	    break;
#endif /* SR_SNMPv2_PDU */

#ifdef SR_SNMPv1_PACKET
        case TRAP_TYPE:
	    datalen = FindLenInt(pdu_ptr->u.trappdu.generic_trap) +
	        FindLenInt(pdu_ptr->u.trappdu.specific_trap) +
	        FindLenUInt(pdu_ptr->u.trappdu.time_ticks) +
	        varbind_tot_len;
	    temp_data_len = FindLenOID(pdu_ptr->u.trappdu.enterprise);
	    if (temp_data_len == -1) {
	        DPRINTF((APPACKET, "%s: temp_data_len (1)\n", Fname));
	        return (-1);
	    }
	    datalen = datalen + temp_data_len;
	    temp_data_len = FindLenOctetString(pdu_ptr->u.trappdu.agent_addr);
	    if (temp_data_len == -1) {
	        DPRINTF((APPACKET, "%s: temp_data_len (2)\n", Fname));
	        return (-1);
	    }
	    datalen = datalen + temp_data_len;
	    break;
#endif /* SR_SNMPv1_PACKET */

        default:
	    DPRINTF((APPACKET, "%s: bad pdu type: %x\n", Fname, pdu_ptr->type));
	    return (-1);
    };				/* end of switch */

    if ((lenlen = DoLenLen(datalen)) == -1) {
	DPRINTF((APPACKET, "%s: lenlen:\n", Fname));
	return (-1);
    }

    /* now allocate memory for PDU packlet */
    if (pdu_ptr->packlet != NULL) {
        FreeOctetString(pdu_ptr->packlet);
    }
    if ((pdu_ptr->packlet =
	 MakeOctetString(NULL, 1 + lenlen + datalen)) == NULL) {
	DPRINTF((APWARN, "%s: pdu_ptr->packlet MakeOctetString failed\n", Fname));
	return (-1);
    }

    working_ptr = pdu_ptr->packlet->octet_ptr;

    *working_ptr++ = (unsigned char) (0xff & pdu_ptr->type);
    AddLen(&working_ptr, lenlen, datalen);

    switch (pdu_ptr->type) {
        case GET_REQUEST_TYPE:
        case GET_NEXT_REQUEST_TYPE:
        case GET_RESPONSE_TYPE:
        case SET_REQUEST_TYPE:
#ifdef SR_SNMPv2_PDU
        case INFORM_REQUEST_TYPE:
        case SNMPv2_TRAP_TYPE:
        case REPORT_TYPE:
#endif /* SR_SNMPv2_PDU */
	    (void) AddInt(&working_ptr, INTEGER_TYPE,
                          pdu_ptr->u.normpdu.request_id);
	    (void) AddInt(&working_ptr, INTEGER_TYPE,
                          pdu_ptr->u.normpdu.error_status);
	    (void) AddInt(&working_ptr, INTEGER_TYPE,
                          pdu_ptr->u.normpdu.error_index);
	    break;

#ifdef SR_SNMPv2_PDU
        case GET_BULK_REQUEST_TYPE:
	    (void) AddInt(&working_ptr, INTEGER_TYPE,
                          pdu_ptr->u.bulkpdu.request_id);
	    (void) AddInt(&working_ptr, INTEGER_TYPE,
                          pdu_ptr->u.bulkpdu.non_repeaters);
	    (void) AddInt(&working_ptr, INTEGER_TYPE,
                          pdu_ptr->u.bulkpdu.max_repetitions);
	    break;
#endif /* SR_SNMPv2_PDU */

#ifdef SR_SNMPv1_PACKET
        case TRAP_TYPE:
	    if (AddOID(&working_ptr, OBJECT_ID_TYPE,
		        pdu_ptr->u.trappdu.enterprise) == -1) {
	        DPRINTF((APPACKET, "%s: AddOID\n", Fname));
	        return (-1);
	    }
	    if (AddOctetString(&working_ptr, IP_ADDR_PRIM_TYPE,
                               pdu_ptr->u.trappdu.agent_addr) == -1) {
	        DPRINTF((APPACKET, "%s: AddOctetString\n", Fname));
	        return -1;
	    }
	    (void) AddInt(&working_ptr, INTEGER_TYPE,
                          pdu_ptr->u.trappdu.generic_trap);
	    (void) AddInt(&working_ptr, INTEGER_TYPE,
                          pdu_ptr->u.trappdu.specific_trap);
	    (void) AddUInt(&working_ptr, TIME_TICKS_TYPE,
                          pdu_ptr->u.trappdu.time_ticks);
	    break;
#endif /* SR_SNMPv1_PACKET */

        default:
	    DPRINTF((APPACKET, "%s: bad pdu_ptr->type - II. :%x\n",
		     Fname, pdu_ptr->type));
	    FreeOctetString(pdu_ptr->packlet);
	    NULLIT(pdu_ptr->packlet);
	    return (-1);
    };				/* end of switch II */

      sequence_type = SEQUENCE_TYPE;
      *working_ptr++ = sequence_type;
      AddLen(&working_ptr, varbindlenlen, varbindlen);
  

    if (AddVarBind(&working_ptr, pdu_ptr->var_bind_list, pdu_ptr) == -1) {
	FreeOctetString(pdu_ptr->packlet);
	NULLIT(pdu_ptr->packlet);
	return (-1);
    }


    return (0);
}				/* end of BuildPdu() */

/* 
 * AddLen:
 *
 * The former name (pre-snmp12.1.0.0 release) was add_len().
 */
void
AddLen(working_ptr, lenlen, data_len)
    unsigned char **working_ptr;
    SR_INT32        lenlen, data_len;
{
    SR_INT32        i;

    if (lenlen == 1) {		/* short form? */
	*(*working_ptr)++ = (unsigned char) data_len;
	return;
    }
    /* oh well, long form time */
    *(*working_ptr)++ = ((unsigned char) 0x80 + (unsigned char) lenlen - 1);
    for (i = 1; i < lenlen; i++)
	*(*working_ptr)++ = (unsigned char) ((data_len >> (8 * (lenlen - i - 1))) & 0x0FF);
}

/* 
 * AddVarBind:
 *
 * The former name (pre-snmp12.1.0.0 release) was add_varbind().
 */
short
AddVarBind(working_ptr, vb_ptr, pdu_ptr)
    unsigned char **working_ptr;
    const VarBind  *vb_ptr;
    Pdu            *pdu_ptr;
{
    FNAME("AddVarBind")
    SR_INT32        lenlen;
    int             sequence_type;


    if (pdu_ptr == NULL) {
	return (0);
    }

           sequence_type = SEQUENCE_TYPE;

    pdu_ptr->num_varbinds = 0;
    while (vb_ptr != NULL) {
	    if ((lenlen = DoLenLen(vb_ptr->data_length)) == -1) {
	        DPRINTF((APPACKET, "%s: lenlen:\n", Fname));
	        return (-1);
	    }
                *(*working_ptr)++ = sequence_type;
	        AddLen(working_ptr, lenlen, vb_ptr->data_length);
                   if (AddOID(working_ptr,OBJECT_ID_TYPE, vb_ptr->name) == -1) {
	               DPRINTF((APPACKET, "%s: AddOID\n", Fname));
	               return (-1);
                   }
	switch (vb_ptr->value.type) {
	    case COUNTER_TYPE:	/* handle unsigned integers including
				 * COUNTER_32 */
	    case GAUGE_TYPE:	/* includes GAUGE_32 */
	    case TIME_TICKS_TYPE:
	        (void) AddUInt(working_ptr, vb_ptr->value.type,
                               vb_ptr->value.ul_value);
	        break;

	    case INTEGER_TYPE:	/* handle signed integers including
				 * INTEGER_32 */
	        (void) AddInt(working_ptr, vb_ptr->value.type,
                              vb_ptr->value.sl_value);
	        break;

	    case OBJECT_ID_TYPE:	/* handle quasi-octet strings */
	        if (AddOID(working_ptr, vb_ptr->value.type,
		           vb_ptr->value.oid_value) == -1) {
		    DPRINTF((APPACKET, "%s: AddOID\n", Fname));
		    return (-1);
	        }
	        break;

#ifdef SR_SNMPv2_PDU
	    case COUNTER_64_TYPE:	/* handle big unsigned integers */
	        (void) AddUInt64(working_ptr, vb_ptr->value.type,
                                 vb_ptr->value.uint64_value);
	        break;

#endif /* SR_SNMPv2_PDU */

	    case OCTET_PRIM_TYPE:
	    case IP_ADDR_PRIM_TYPE:
	    case OPAQUE_PRIM_TYPE:
	        if (AddOctetString(working_ptr, vb_ptr->value.type,
                                   vb_ptr->value.os_value) == -1) {
		    DPRINTF((APPACKET, "%s: AddOctetString\n", Fname));
		    return -1;
	        }
	        break;

	    case NULL_TYPE:
#ifdef SR_SNMPv2_PDU
	    case NO_SUCH_OBJECT_EXCEPTION:
	    case NO_SUCH_INSTANCE_EXCEPTION:
	    case END_OF_MIB_VIEW_EXCEPTION:
#endif /* SR_SNMPv2_PDU */
	        AddNull(working_ptr, vb_ptr->value.type);
	        break;

	    default:
	        DPRINTF((APPACKET, "%s: value: Illegal type: 0x%x\n",
                         Fname, vb_ptr->value.type));
	        return (-1);
	};				/* end of switch */

	pdu_ptr->num_varbinds++;
	vb_ptr = vb_ptr->next_var;
    }
    return (0);
}


/* 
 * AddOctetString:
 *
 * The former name (pre-snmp12.1.0.0 release) was add_octetstring().
 */
short
AddOctetString(working_ptr, type, os_ptr)
    unsigned char **working_ptr;
    int             type;
    const OctetString *os_ptr;
{
    SR_INT32        i;
    SR_INT32        lenlen;

    if ((lenlen = DoLenLen(os_ptr->length)) == -1) {
	DPRINTF((APPACKET, "AddOctetString,lenlen\n"));
	return (-1);
    }


    *(*working_ptr)++ = (unsigned char) (0xff & type);
    AddLen(working_ptr, lenlen, os_ptr->length);

    for (i = 0; i < os_ptr->length; i++)
	*(*working_ptr)++ = os_ptr->octet_ptr[i];

    return (0);
}




/* 
 * AddOID:
 *
 * The former name (pre-snmp12.1.0.0 release) was add_oid().
 */
short
AddOID(working_ptr, type, oid_ptr)
    unsigned char **working_ptr;
    int             type;
    const OID      *oid_ptr;
{
    FNAME("AddOID")
    short           i;
    SR_INT32        lenlen;
    SR_INT32        encoded_len;

    if(oid_ptr->length < 2) {
	DPRINTF((APWARN, 
		 "%s: Attempting to encode oid with length < 2\n", Fname));
        return -1;
    }

        if(oid_ptr->oid_ptr[0] > 2 || oid_ptr->oid_ptr[1] >= 40) {
    	   DPRINTF((APWARN, "%s: illegal value for first two subids\n", Fname));
	   return -1;
        }

    encoded_len = 1;		/* for first two SID's */
    for (i = 2; i < oid_ptr->length; i++) {
	if (oid_ptr->oid_ptr[i] < 0x80)	/* 0 - 0x7f */
	    encoded_len += 1;
	else if (oid_ptr->oid_ptr[i] < 0x4000)	/* 0x80 - 0x3fff */
	    encoded_len += 2;
	else if (oid_ptr->oid_ptr[i] < 0x200000)  /* 0x4000 - 0x1FFFFF */
	    encoded_len += 3;
	else if (oid_ptr->oid_ptr[i] < 0x10000000)  /* 0x200000 - 0x0fffffff */
	    encoded_len += 4;
	else
	    encoded_len += 5;
    }

    if ((lenlen = DoLenLen(encoded_len)) == -1) {
	DPRINTF((APPACKET, "%s: lenlen\n", Fname));
	return (-1);
    }

    *(*working_ptr)++ = (unsigned char) (0xff & type);
    AddLen(working_ptr, lenlen, encoded_len);

    *(*working_ptr)++ = (unsigned char) ((oid_ptr->oid_ptr[0] * 40) +
					 oid_ptr->oid_ptr[1]);

    for (i = 2; i < oid_ptr->length; i++) {
	if (oid_ptr->oid_ptr[i] < 0x80) {	/* 0 - 0x7f */
	    *(*working_ptr)++ = (unsigned char) oid_ptr->oid_ptr[i];
	}
	else if (oid_ptr->oid_ptr[i] < 0x4000) {	/* 0x80 - 0x3fff */
	    *(*working_ptr)++ = (unsigned char)
		(((oid_ptr->oid_ptr[i]) >> 7) | 0x80);	/* set high bit */
	    *(*working_ptr)++ = (unsigned char) (oid_ptr->oid_ptr[i] & 0x7f);
	}
	else if (oid_ptr->oid_ptr[i] < 0x200000) {	/* 0x4000 - 0x1FFFFF */
	    *(*working_ptr)++ = (unsigned char)
		(((oid_ptr->oid_ptr[i]) >> 14) | 0x80);	/* set high bit */
	    *(*working_ptr)++ = (unsigned char)
		(((oid_ptr->oid_ptr[i]) >> 7) | 0x80);	/* set high bit */
	    *(*working_ptr)++ = (unsigned char) (oid_ptr->oid_ptr[i] & 0x7f);
	}
	else if (oid_ptr->oid_ptr[i] < 0x10000000) {	/* 0x200000 - 0x0fffffff */
	    *(*working_ptr)++ = (unsigned char)
		(((oid_ptr->oid_ptr[i]) >> 21) | 0x80);	/* set high bit */
	    *(*working_ptr)++ = (unsigned char)
		(((oid_ptr->oid_ptr[i]) >> 14) | 0x80);	/* set high bit */
	    *(*working_ptr)++ = (unsigned char)
		(((oid_ptr->oid_ptr[i]) >> 7) | 0x80);	/* set high bit */
	    *(*working_ptr)++ = (unsigned char) (oid_ptr->oid_ptr[i] & 0x7f);
	}
	else {
	    *(*working_ptr)++ = (unsigned char)
		(((oid_ptr->oid_ptr[i]) >> 28) | 0x80);	/* set high bit */
	    *(*working_ptr)++ = (unsigned char)
		(((oid_ptr->oid_ptr[i]) >> 21) | 0x80);	/* set high bit */
	    *(*working_ptr)++ = (unsigned char)
		(((oid_ptr->oid_ptr[i]) >> 14) | 0x80);	/* set high bit */
	    *(*working_ptr)++ = (unsigned char)
		(((oid_ptr->oid_ptr[i]) >> 7) | 0x80);	/* set high bit */
	    *(*working_ptr)++ = (unsigned char) (oid_ptr->oid_ptr[i] & 0x7f);
	}
    }				/* end of second for */

    return (0);
}				/* end of AddOID */







/* 
 * AddUInt:
 *
 * The former name (pre-snmp12.1.0.0 release) was add_unsignedinteger().
 */
short
AddUInt(working_ptr, type, value)
    unsigned char **working_ptr;
    int             type;
    SR_UINT32       value;
{
    SR_INT32        i;
    SR_INT32        datalen;
    SR_INT32        lenlen;


    /* if high bit one, must use 5 octets (first with 00) */
    if (((value >> 24) & 0x0ff) != 0)
	datalen = 4;

    else if (((value >> 16) & 0x0ff) != 0)
	datalen = 3;
    else if (((value >> 8) & 0x0ff) != 0)
	datalen = 2;
    else
	datalen = 1;

    if (((value >> (8 * (datalen - 1))) & 0x080) != 0)
	datalen++;

    lenlen = 1;			/* < 127 octets */

    *(*working_ptr)++ = (unsigned char) (0xff & type);
    AddLen(working_ptr, lenlen, datalen);

    if (datalen == 5) {		/* gotta put a 00 in first octet */
	*(*working_ptr)++ = (unsigned char) 0;
	for (i = 1; i < datalen; i++)	/* bug fix 4/24/89, change 0 -> 1 */
	    *(*working_ptr)++
		= (unsigned char) (value >> (8 * ((datalen - 1) - i) & 0x0ff));
    }
    else {
	for (i = 0; i < datalen; i++)
	    *(*working_ptr)++
		= (unsigned char) (value >> (8 * ((datalen - 1) - i) & 0x0ff));
    }

    return (0);
}

/* 
 * AddInt:
 *
 * The former name (pre-snmp12.1.0.0 release) was add_signedinteger().
 */
short
AddInt(working_ptr, type, value)
    unsigned char **working_ptr;
    int             type;
    SR_INT32        value;
{
    SR_INT32        i;
    SR_INT32        datalen;
    SR_INT32        lenlen;

    switch ((unsigned char) ((value >> 24) & 0x0ff)) {
    case 0x00:
	if (((value >> 16) & 0x0ff) != 0)
	    datalen = 3;
	else if (((value >> 8) & 0x0ff) != 0)
	    datalen = 2;
	else
	    datalen = 1;
	if (((value >> (8 * (datalen - 1))) & 0x080) != 0)
	    datalen++;

	break;
    case 0xff:
	if (((value >> 16) & 0x0ff) != 0xFF)
	    datalen = 3;
	else if (((value >> 8) & 0x0ff) != 0xFF)
	    datalen = 2;
	else
	    datalen = 1;
	if (((value >> (8 * (datalen - 1))) & 0x080) == 0)
	    datalen++;
	break;

    default:
	datalen = 4;
    };				/* end of switch */

    lenlen = 1;			/* < 127 octets */

    *(*working_ptr)++ = (unsigned char) (0xff & type);
    AddLen(working_ptr, lenlen, datalen);

    for (i = 0; i < datalen; i++)
	*(*working_ptr)++
	    = (unsigned char) (value >> (8 * ((datalen - 1) - i) & 0x0ff));

    return (0);
}

#ifdef SR_SNMPv2_PDU
/* 
 * AddUInt64:
 *
 * The former name (pre-snmp12.1.0.0 release) was add_uint64().
 */
short
AddUInt64(working_ptr, type, value)
    unsigned char **working_ptr;
    int             type;
    const UInt64   *value;
{
    SR_INT32        i;
    SR_INT32        datalen;
    SR_INT32        lenlen;


    datalen = FindLenUInt64(value) - 2;

    lenlen = 1;			/* < 127 octets */

    *(*working_ptr)++ = (unsigned char) (0xff & type);
    AddLen(working_ptr, lenlen, datalen);

    if (datalen == 9) {		/* gotta put a 00 in first octet */
	*(*working_ptr)++ = (unsigned char) 0;
	datalen--;
    }

    for (i = datalen; i > 4; i--) {
	*(*working_ptr)++ =
	    (unsigned char) (value->big_end >> (8 * (i - 5) & 0x0ff));
    }
    for (; i > 0; i--) {
	*(*working_ptr)++ =
	    (unsigned char) (value->little_end >> (8 * (i - 1) & 0x0ff));
    }

    return (0);
}
#endif /* SR_SNMPv2_PDU */

/* 
 * AddNull:
 *
 * The former name (pre-snmp12.1.0.0 release) was add_null().
 */
void
AddNull(working_ptr, type)
    unsigned char **working_ptr;
    int             type;
{
    *(*working_ptr)++ = (unsigned char) (0xff & type);
    *(*working_ptr)++ = 0x00;
}

/* NormalizeVarBindListLengths() moved to devkit/nrmvblst.c */
