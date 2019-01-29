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
 * get-responses based on Set requests and errors that turn the PDU's
 * around may have improper length values in name_size and value_size.
 *
 * On parsing incoming PDU's,the agent may have to deal with broken 
 * managers may have encoded lengths with non-minimal values. These 
 * non-minimal values are parsed and put in name_size and value_size.
 * These are used to determine the number of octets to skip to find the
 * next element to parse.
 *
 * On building an outgoing PDU, these values assume that length encoding was
 * done using minimal length encoding.  They are normally set to this value
 * by way of the MakeVarBind() call.  However, when a PDU is simply turned
 * around, the VarBinds could have the erroneous lengths in them.  This
 * routine should be called by any function that turns the packet around
 * to normalize those lengths.
 */

void
NormalizeVarBindListLengths(vb_ptr)
    VarBind     *vb_ptr;
{

    while (vb_ptr != NULL) {

	/* Fix up name size */
	   vb_ptr->name_size = FindLenOID(vb_ptr->name);

	/* Now fix up value size */
	/* combine the Choices from simple and application */
	switch (vb_ptr->value.type) {
	    case COUNTER_TYPE:	/* includes COUNTER_32_TYPE */
	    case GAUGE_TYPE:	/* includes GAUGE_32_TYPE */
	    case TIME_TICKS_TYPE:
	        vb_ptr->value_size = FindLenUInt(vb_ptr->value.ul_value);
	        break;

	    case INTEGER_TYPE:	/* includes INTEGER_32_TYPE */
	        vb_ptr->value_size = FindLenInt(vb_ptr->value.sl_value);
	        break;

	    case OBJECT_ID_TYPE:
	        vb_ptr->value_size = FindLenOID(vb_ptr->value.oid_value);
	        break;

	    case OCTET_PRIM_TYPE:
	    case IP_ADDR_PRIM_TYPE:
	    case OPAQUE_PRIM_TYPE:
	        vb_ptr->value_size = FindLenOctetString(vb_ptr->value.os_value);
	        break;

#ifdef SR_SNMPv2_PDU
	    case COUNTER_64_TYPE:
	        vb_ptr->value_size = FindLenUInt64(vb_ptr->value.uint64_value);
	        break;

#endif /* SR_SNMPv2_PDU */

	    case NULL_TYPE:
#ifdef SR_SNMPv2_PDU
	    case NO_SUCH_OBJECT_EXCEPTION:
	    case NO_SUCH_INSTANCE_EXCEPTION:
	    case END_OF_MIB_VIEW_EXCEPTION:
#endif /* SR_SNMPv2_PDU */
	        vb_ptr->value_size = 2;
	        break;
	}
	vb_ptr->data_length = vb_ptr->name_size + vb_ptr->value_size;
	vb_ptr = vb_ptr->next_var;
    }
}
