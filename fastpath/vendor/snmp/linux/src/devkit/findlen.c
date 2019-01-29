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

#include "sr_snmp.h"
#include "diag.h"
SR_FILENAME


/* 
 * FindLenVarBind:
 *
 * The former name (pre-snmp12.1.0.0 release) was find_len_varbind().
 */
SR_INT32
FindLenVarBind(vb_ptr)
    VarBind        *vb_ptr;
{
    SR_INT32        lenlen;
    SR_INT32        total = 0;

    while (vb_ptr != NULL) {
	vb_ptr->data_length = vb_ptr->name_size + vb_ptr->value_size;
  
	   lenlen = DoLenLen(vb_ptr->data_length);
	   if (lenlen == -1) {
	       DPRINTF((APPACKET, "FindLenVarBind, lenlen\n"));
	       return (-1);
	   }
   	       total += 1 + lenlen + vb_ptr->data_length;
	vb_ptr = vb_ptr->next_var;
    }

    return (total);
}




/* 
 * FindLenOctetString:
 *
 * The former name (pre-snmp12.1.0.0 release) was find_len_octetstring().
 */
SR_INT32
FindLenOctetString(os_ptr)
    const OctetString *os_ptr;
{
    SR_INT32        lenlen;

    /* begin fix 02-Jan-91 */
    if (os_ptr == NULL) {
	DPRINTF((APPACKET, "FindLenOctetString, NULL input\n"));
	return (-1);
    }
    /* end fix 02-Jan-91 */

    if ((lenlen = DoLenLen(os_ptr->length)) == -1) {
	DPRINTF((APPACKET, "FindLenOctetString, lenlen\n"));
	return (-1);
    }
    return (1 + lenlen + os_ptr->length);
}

/* 
 * FindLenOID:
 *
 * The former name (pre-snmp12.1.0.0 release) was find_len_oid().
 */
short
FindLenOID(oid_ptr)
    const OID *oid_ptr;
{
    SR_INT32        lenlen;
    short           i;
    SR_INT32        encoded_len;

    encoded_len = 1;		/* for first two SID's */

    for (i = 2; i < oid_ptr->length; i++) {
	if (oid_ptr->oid_ptr[i] < 0x80)	/* 0 - 0x7f */
	    encoded_len += 1;
	else if (oid_ptr->oid_ptr[i] < 0x4000)	/* 0x80 - 0x3fff */
	    encoded_len += 2;
	else if (oid_ptr->oid_ptr[i] < 0x200000)	/* 0x4000 - 0x1FFFFF */
	    encoded_len += 3;
	else if (oid_ptr->oid_ptr[i] < 0x10000000)	/* 0x200000 - 0x0fffffff */
	    encoded_len += 4;
	else
	    encoded_len += 5;
    }

    if ((lenlen = DoLenLen(encoded_len)) == -1) {
	DPRINTF((APPACKET, "FindLenOID, lenlen\n"));
	return (-1);
    }
    return (short) (1 + lenlen + encoded_len);
}

/* 
 * FindLenUInt:
 *
 * The former name (pre-snmp12.1.0.0 release) was find_len_unsignedinteger().
 */
short
FindLenUInt(value)
    SR_UINT32       value;
{
    SR_INT32        datalen;

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

    /* length of length  < 127 octets */

    return (short) (1 + 1 + datalen);
}

/* 
 * FindLenInt:
 *
 * The former name (pre-snmp12.1.0.0 release) was find_len_signedinteger().
 */
short
FindLenInt(value)
    SR_INT32        value;
{
    SR_INT32        datalen;

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

    return (short) (1 + 1 + datalen);
}

#ifdef SR_SNMPv2_PDU
/* 
 * FindLenUInt64:
 *
 * The former name (pre-snmp12.1.0.0 release) was find_len_uint64().
 */
short
FindLenUInt64(value)
    const UInt64 *value;
{
    SR_INT32        datalen;

    /* if high bit one, must use 5 octets (first with 00) */
    if (((value->big_end >> 24) & 0x0ff) != 0) {
	datalen = 8;
	if (((value->big_end >> 24) & 0x080) != 0) {
	    datalen++;
	}
    }
    else if (((value->big_end >> 16) & 0x0ff) != 0) {
	datalen = 7;
	if (((value->big_end >> 16) & 0x080) != 0) {
	    datalen++;
	}
    }
    else if (((value->big_end >> 8) & 0x0ff) != 0) {
	datalen = 6;
	if (((value->big_end >> 8) & 0x080) != 0) {
	    datalen++;
	}
    }
    else if (((value->big_end) & 0x0ff) != 0) {
	datalen = 5;
	if (((value->big_end) & 0x080) != 0) {
	    datalen++;
	}
    }
    else if (((value->little_end >> 24) & 0x0ff) != 0) {
	datalen = 4;
	if (((value->little_end >> 24) & 0x080) != 0) {
	    datalen++;
	}
    }
    else if (((value->little_end >> 16) & 0x0ff) != 0) {
	datalen = 3;
	if (((value->little_end >> 16) & 0x080) != 0) {
	    datalen++;
	}
    }
    else if (((value->little_end >> 8) & 0x0ff) != 0) {
	datalen = 2;
	if (((value->little_end >> 8) & 0x080) != 0) {
	    datalen++;
	}
    }
    else {
	datalen = 1;
	if (((value->little_end) & 0x080) != 0) {
	    datalen++;
	}
    }

    /* length of length  < 127 octets */

    return (short) (1 + 1 + datalen);
}
#endif /* SR_SNMPv2_PDU */

/* 
 * DoLenLen:
 *
 * The former name (pre-snmp12.1.0.0 release) was dolenlen().
 */
short
DoLenLen(len)
    SR_INT32        len;
{
    /* short form? */
    if (len < 128)
	return (1);
    if (len < 0x0100)
	return (2);
    if (len < 0x010000)
	return (3);
    if (len < 0x01000000)
	return (4);
    DPRINTF((APPACKET, "Lenlen: Length greater than 0x01000000???\n"));
    return (-1);
}




