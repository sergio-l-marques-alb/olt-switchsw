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
 * any actual or intended publication of such source code.
 *
 */

/*
 * The routines in this file use the SR_OFFSET macro defined in snmpd.h as:
 *
 *     #define SR_OFFSET(x,off)   (void *)((char *)x + (off * sizeof(long)))
 *
 * These routines may be used to reduce the agent's code size, however, they
 * are not portable to all platforms. In particular, if sizeof(long) is not
 * equal to sizeof(void *), then the these routines should not be used.
 *
 * To generate method routines which use these functions, use the -off option
 * to postmosy when generating the method routine stubs.
 */

#include "sr_conf.h"

#include <malloc.h>

#include "snmpd.h"
#include "sroffset.h"

/*
 * AssignValue() copies a value from a field in a structure defined in
 * snmptype.h to the ObjectSyntax structure. The field from the snmptype.h
 * structure is determined by the nominator from the ObjectSyntax structure.
 */
int
AssignValue (object, data, value)
    const ObjectInfo *object;
    void *data;
    const ObjectSyntax *value;
{
    SR_INT32 *tmpInt;
    void **tmpPtr;

    switch (object->oidtype) {
	case INTEGER_TYPE:   	/* includes Integer32 */
            tmpInt = SR_OFFSET(data, object->nominator);
	    *tmpInt = value->sl_value;
            break;
	case COUNTER_TYPE:	/* includes COUNTER_32_TYPE */
	case GAUGE_TYPE:	/* includes GAUGE_32_TYPE */
	case TIME_TICKS_TYPE:
            tmpInt = SR_OFFSET(data, object->nominator);
	    *tmpInt = value->ul_value;
	    break;
	case OCTET_PRIM_TYPE:
            tmpPtr = (void **) SR_OFFSET(data, object->nominator);
            if (*tmpPtr != NULL) {
                FreeOctetString((OctetString *) *tmpPtr);
            }
            *tmpPtr = CloneOctetString(value->os_value);
	    break;
	case OBJECT_ID_TYPE:
            tmpPtr = (void **) SR_OFFSET(data, object->nominator);
            if (*tmpPtr != NULL) {
                FreeOID((OID *) *tmpPtr);
            }
            *tmpPtr = CloneOID(value->oid_value);
	    break;
	case IP_ADDR_PRIM_TYPE:
            if (value->os_value->length != 4) {
                return WRONG_LENGTH_ERROR;
            }
            tmpInt = SR_OFFSET(data, object->nominator);
	    *tmpInt = OctetStringToIP(value->os_value);
	    break;
#ifdef SR_SNMPv2_PDU
#endif /* SR_SNMPv2_PDU */
        default:
	    return GEN_ERROR;
            break;
    }
    return NO_ERROR;
} 

