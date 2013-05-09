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

#include <string.h>


#include "sr_snmp.h"
#include "mib_tabl.h"
#include "oid_lib.h"
#include "diag.h"
SR_FILENAME

#include "sr_proto.h"
#include "inst_lib.h"



#define MAX_INT32 0x7FFFFFFF
#define MAX_UINT32 ((SR_UINT32) 0xFFFFFFFF)

/*
 * InstToInt:
 *
 * Extract an integer index from the incoming OID instance starting 
 * at the postion indicated by the index parameter. returns 1 on success,
 * 0 on failure. 
 * 
 * ** On success, the normal return value will be '1' unless for a NEXT type 
 * search, if the index value is out-of-range then the return value will be
 * '2' to indicate that any forward instances should be cleared (zero-ed out) 
 */
int
InstToInt(inst, index, val, searchType, carry)
    const OID      *inst;
    int             index;
    SR_INT32       *val;
    int             searchType;
    int            *carry;
{
    SR_UINT32       tmp;
    int             out_of_range_increment = 0;

    if (val == NULL) {
	return -1;
    }
    if ((searchType == GET_REQUEST_TYPE) && *carry) {
	return -1;
    }
    if (index >= inst->length) {
	if (searchType == GET_REQUEST_TYPE) {
	    return -1;
	}
    }

    if (index < inst->length) {
	tmp = inst->oid_ptr[index];
	if ((tmp > (SR_UINT32) MAX_INT32) || 
	    (tmp + *carry) > (SR_UINT32) MAX_INT32) {
	    if (searchType == GET_REQUEST_TYPE) {
		return -1;
	    }
	    tmp = ((SR_INT32)0);
	    *carry = 1;
	    out_of_range_increment = 1;
	}
	else {
	    tmp += *carry;
	    *carry = 0;
	}
	*val = (SR_INT32) tmp;
    }
    else {
	*val = ((SR_INT32)0);
	*carry = 0;
    }

    if (!out_of_range_increment) {
        /* normal case */
        return 1;
    } else {
        /* return indication that forward indices should be cleared */
        return 2;
    }
}

/*
 * InstToUInt:
 *
 * Extract an unsigned integer index from the incoming OID instance starting 
 * at the postion indicated by the index parameter. returns 1 on success,
 * 0 on failure. 
 * 
 * ** On success, the normal return value will be '1' unless for a NEXT type 
 * search, if the index value is out-of-range then the return value will be
 * '2' to indicate that any forward instances should be cleared (zero-ed out) 
 */
int
InstToUInt(inst, index, val, searchType, carry)
    const OID      *inst;
    int             index;
    SR_UINT32      *val;
    int             searchType;
    int            *carry;
{
    SR_UINT32       tmp;
    int             out_of_range_increment = 0;

    if (val == NULL) {
	return -1;
    }
    if ((searchType == GET_REQUEST_TYPE) && *carry) {
	return -1;
    }
    if (index >= inst->length) {
	if (searchType == GET_REQUEST_TYPE) {
	    return -1;
	}
    }

    if (index < inst->length) {
	tmp = inst->oid_ptr[index];
	if ((tmp == MAX_UINT32) && *carry) {
	    tmp = ((SR_INT32)0);
	    *carry = 1;
	    out_of_range_increment = 1;
	}
	else {
	    tmp += *carry;
	    *carry = 0;
	}

	*val = tmp;
    }
    else {
	*val = (SR_UINT32) 0;
	*carry = 0;
    }

    if (!out_of_range_increment) {
        /* normal case */
        return 1;
    } else {
        /* return indication that forward indices should be cleared */
        return 2;
    }
}

/*
 * InstToIP:
 *
 * Extract an IP address index from the incoming OID instance starting
 * at the postion indicated by the index parameter. 
 * 
 * Returns: > 0 on success     
 *          -1 on failure
 * 
 * ** On success, the normal return value will be '1' unless for a NEXT type 
 * search, if the index value is out-of-range then the return value will be
 * '2' to indicate that any forward instances should be cleared (zero-ed out) 
 *
 */
int
InstToIP(inst, index, ipAddr, searchType, carry)
    const OID      *inst;
    int             index;
    SR_UINT32      *ipAddr;
    int             searchType;
    int            *carry;
{
    int             i;
    int             length;
    SR_UINT32       tmp;
    int             out_of_range_increment = 0;

    if (ipAddr == NULL) {
	return -1;
    }
    *ipAddr = ((SR_INT32)0);
    length = inst->length - index;

    if (searchType == GET_REQUEST_TYPE) {
	if ((length < 4) || *carry) {
	    return -1;
	}

        *ipAddr = ((SR_INT32)0);
        if ((inst->oid_ptr[index] > 255) ||
            (inst->oid_ptr[index+1] > 255) ||
            (inst->oid_ptr[index+2] > 255) ||
            (inst->oid_ptr[index+3] > 255)) {
            return -1;
        }
        *ipAddr = (inst->oid_ptr[index] << 24) +
                  (inst->oid_ptr[index+1] << 16) +
                  (inst->oid_ptr[index+2] << 8) +
                  inst->oid_ptr[index+3];

	return 1;
    }
    else {			/* must be a NEXT */
	if (length < 4)
	    *carry = 0;
	*ipAddr = ((SR_INT32)0);
	for (i = ((length > 4) ? 3 : length - 1); i >= 0; i--) {
	    tmp = inst->oid_ptr[index + i];
	    if ((tmp > 255) || ((tmp + *carry) > 255)) {
		*ipAddr = ((SR_INT32)0);
		*carry = 1;
		out_of_range_increment = 1;
	    }
	    else {
		tmp += *carry;
		*ipAddr |= tmp << ((3 - i) * 8);
		*carry = 0;
	    }
	}
        if (!out_of_range_increment) {
            /* normal case */
    	    return 1;
        } else {
            /* return indication that forward indices should be cleared */
            return 2;
        }
    }
}

/*
 * InstToFixedOctetString:
 *
 * Extract an OctetString index from the incoming OID instance starting
 * at the postion indicated by the index parameter. returns 1 on success,
 * 0 on failure.
 * 
 * ** On success, the normal return value will be '1' unless for a NEXT type 
 * search, if the index value is out-of-range then the return value will be
 * '2' to indicate that any forward instances should be cleared (zero-ed out) 
 */

int
InstToFixedOctetString(inst, index, os, searchType, carry, size)
    const OID      *inst;
    int             index;
    OctetString   **os;
    int             searchType;
    int            *carry;
    int             size;
{
    int             length;
    int             i, j;
    int             out_of_range_increment = 0;
    SR_UINT32       tmp;

    if (os == NULL) {
	goto fail;
    }
    if (searchType == GET_REQUEST_TYPE && *carry) {
	*os = NULL;
	goto fail;
    }

    if(((*os) = MakeOctetString(NULL, size)) == NULL) {
	DPRINTF((APWARN, "InstToFixedOctetString: MakeOctetString failed\n"));
	goto fail;
    }

    (*os)->length = 0;
    if (index >= inst->length) {
	if (searchType == GET_REQUEST_TYPE) {
	    goto fail;
	}
    }

    length = inst->length - index;
    (*os)->length = size;

    if (searchType == GET_REQUEST_TYPE) {
	if ((length < size) || *carry) {
	    goto fail;
	}

	for (i = size - 1; i >= 0; i--) {
	    if (inst->oid_ptr[index + i] > 255) {
		goto fail;
	    }
	    (*os)->octet_ptr[i] = (unsigned char) inst->oid_ptr[index + i];
	}
	return 1;
    }
    else {			/* must be a NEXT */
	for (i = size - 1; i >= 0; i--) {
            if ((index + i) < inst->length) {
		tmp = inst->oid_ptr[index + i];
            } else {
		tmp = ((SR_INT32)0);
		*carry = 0;
            }
	    if ((tmp > 255) || ((tmp + *carry) > 255)) {
		for (j = i; j < size; j++) {
		    (*os)->octet_ptr[j] = (unsigned char) 0;
		}
		*carry = 1;
		out_of_range_increment = 1;
	    }
	    else {
		(*os)->octet_ptr[i] = (unsigned char) (tmp + *carry);
		*carry = 0;
	    }
	}
    }

    if (!out_of_range_increment) {
        /* normal case */
        return 1;
    } else {
        /* return indication that forward indices should be cleared */
        return 2;
    }

fail:
    if (os != NULL && *os != NULL) {
	FreeOctetString(*os);
	*os = NULL;
    }
    return -1;
}

/*
 * InstToNetworkAddress:
 *
 * Extract an NetworkAddress index from the incoming OID instance starting
 * at the postion indicated by the index parameter. returns 1 on success,
 * 0 on failure.
 * 
 * ** On success, the normal return value will be '1' unless for a NEXT type 
 * search, if the index value is out-of-range then the return value will be
 * '2' to indicate that any forward instances should be cleared (zero-ed out) 
 */

int
InstToNetworkAddress(inst, index, os, searchType, carry)
    const OID      *inst;
    int             index;
    OctetString   **os;
    int             searchType;
    int            *carry;
{
    int             ret_val;
    int             out_of_range_increment = 0;

    ret_val = InstToFixedOctetString(inst, index, os, searchType, carry, 5);
    if (ret_val < 0) {
        return ret_val;
    } else if (ret_val == 2) {
        out_of_range_increment = 1;
    }
    
    if (searchType == GET_REQUEST_TYPE) {
        if ((*os)->octet_ptr[0] != 1) {
            return -1;
        }
    } else {
        if ((*os)->octet_ptr[0] != 1) {
            if ((*os)->octet_ptr[0] > 1) {
                *carry = 1;
            }
            (*os)->octet_ptr[0] = 1;
            (*os)->octet_ptr[1] = 0;
            (*os)->octet_ptr[2] = 0;
            (*os)->octet_ptr[3] = 0;
            (*os)->octet_ptr[4] = 0;
        }
    }

    if (!out_of_range_increment) {
        /* normal case */
        return 1;
    } else {
        /* return indication that forward indices should be cleared */
        return 2;
    }
}

/*
 * InstToImpliedOctetString:
 *
 * Extract an OctetString index from the incoming OID instance starting
 * at the postion indicated by the index parameter. returns 1 on success,
 * 0 on failure.
 * 
 * ** On success, the normal return value will be '1' unless for a NEXT type 
 * search, if the index value is out-of-range then the return value will be
 * '2' to indicate that any forward instances should be cleared (zero-ed out) 
 */
int
InstToImpliedOctetString(inst, index, os, searchType, carry)
    const OID      *inst;
    int             index;
    OctetString   **os;
    int             searchType;
    int            *carry;
{
    int             expected_length;
    int             i;
    SR_UINT32       tmp;
    int             out_of_range_increment = 0;

    if (os == NULL) {
	goto fail;
    }
    if (searchType == GET_REQUEST_TYPE && *carry) {
	*os = NULL;
	goto fail;
    }

    if (index >= inst->length) {
	if (searchType == GET_REQUEST_TYPE) {
	    goto fail;
	}
	*carry = 0;
        if(((*os) = MakeOctetString(NULL, 0)) == NULL) {
    	    DPRINTF((APWARN,
		     "InstToImpliedOctetString: MakeOctetString failed\n"));
	    goto fail;
        }
	return 1;
    }

    expected_length = inst->length - index;

    if(((*os) = MakeOctetString(NULL, expected_length + 1)) == NULL) {
	DPRINTF((APWARN,"InstToImpliedOctetString: MakeOctetString failed\n"));
	goto fail;
    }

    if (searchType == GET_REQUEST_TYPE) {
	for (i = expected_length - 1; i >= 0; i--) {
	    if (inst->oid_ptr[index + i] > 255) {
		goto fail;
	    }
	    (*os)->octet_ptr[i] = (unsigned char) inst->oid_ptr[index + i];
	}
        (*os)->length = expected_length;
	return 1;
    }
    else {			/* must be a NEXT */
	(*os)->octet_ptr[expected_length] = (unsigned char) 0;
	*carry = 0;

	((*os)->length) = expected_length + 1;
	for (i = expected_length - 1; i >= 0; i--) {
            if ((index + i) < inst->length) {
                tmp = inst->oid_ptr[index + i];
            } else {
                tmp = ((SR_INT32)0);
                *carry = 0;
            }
	    if ((tmp > 255) || ((tmp + *carry) > 255)) {
		((*os)->length) = i;
		*carry = 1;
		out_of_range_increment = 1;
	    }
	    else {
		(*os)->octet_ptr[i] = (unsigned char) (tmp + *carry);
		*carry = 0;
	    }
	}
    }

    if (!out_of_range_increment) {
        /* normal case */
        return 1;
    } else {
        /* return indication that forward indices should be cleared */
        return 2;
    }

fail:
    if (os != NULL && *os != NULL) {
	FreeOctetString(*os);
	*os = NULL;
    }
    return -1;
}

/*
 * InstToVariableOctetString:
 *
 * Extract an OctetString index from the incoming OID instance starting
 * at the postion indicated by the index parameter. returns 1 on success,
 * 0 on failure.
 * 
 * ** On success, the normal return value will be '1' unless for a NEXT type 
 * search, if the index value is out-of-range then the return value will be
 * '2' to indicate that any forward instances should be cleared (zero-ed out) 
 */
int
InstToVariableOctetString(inst, index, os, searchType, carry)
    const OID      *inst;
    int             index;
    OctetString   **os;
    int             searchType;
    int            *carry;
{
    int             length, expected_length;
    int             i, j;
    SR_UINT32       tmp;
    int             out_of_range_increment = 0;

    if (os == NULL) {
	goto fail;
    }
    *os = NULL;
    if (searchType == GET_REQUEST_TYPE && *carry) {
	goto fail;
    }

    if (index >= inst->length) {
	if (searchType == GET_REQUEST_TYPE) {
	    goto fail;
	}
	*carry = 0;
        if (((*os) = MakeOctetString(NULL, 0)) == NULL) {
	    goto fail;
        }
	return 1;
    }

    expected_length = inst->oid_ptr[index];

    if ((expected_length < 0) || (expected_length > MAX_OID_SIZE)) {
        if (searchType == GET_REQUEST_TYPE) {
            goto fail;
        } else {
            /* value invalid, any forward indices should be zero-ed out */
	    *carry = 1;
            if (((*os) = MakeOctetString(NULL, 0)) == NULL) {
	        goto fail;
            }
	    return 2;
        }
    }

    if (((*os) = MakeOctetString(NULL, expected_length + 1)) == NULL) {
       DPRINTF((APWARN,"InstToVariableOctetString: MakeOctetString failed\n"));
       goto fail;
    }
    ((*os)->length) = expected_length;

    length = inst->length - index - 1;

    if (searchType == GET_REQUEST_TYPE) {
	if ((length < expected_length) || *carry) {
	    goto fail;
	}

	for (i = expected_length; i > 0; i--) {
	    if (inst->oid_ptr[index + i] > 255) {
		goto fail;
	    }
	    (*os)->octet_ptr[i - 1] = (unsigned char) inst->oid_ptr[index + i];
	}
	return 1;
    }
    else {			/* must be a NEXT */
	for (i = expected_length; i > 0; i--) {
            if ((index + i) < inst->length) {
                tmp = inst->oid_ptr[index + i];
            } else {
                tmp = ((SR_INT32)0);
                *carry = 0;
            }

	    if ((tmp > 255) || ((tmp + *carry) > 255)) {
		for (j = i; j <= expected_length; j++) {
		    (*os)->octet_ptr[j-1] = (unsigned char) 0;
		}
		*carry = 1;
		out_of_range_increment = 1;
	    }
	    else {
		(*os)->octet_ptr[i - 1] = (unsigned char) (tmp + *carry);
		*carry = 0;
	    }
	}
        if (*carry) {
            ((*os)->length)++;
            if (((*os)->length) > MAX_OID_SIZE) {
               ((*os)->length) = ((SR_INT32)0);
            } else {
                *carry = 0;
            }
	    for (j = 0; j < ((*os)->length); j++) {
	        (*os)->octet_ptr[j] = (unsigned char) 0;
    	    }
        }
    }

    if (!out_of_range_increment) {
        /* normal case */
        return 1;
    } else {
        /* return indication that forward indices should be cleared */
        return 2;
    }

fail:
    if (os != NULL && *os != NULL) {
	FreeOctetString(*os);
	*os = NULL;
    }
    return -1;
}

/*
 * InstToImpliedOID:
 *
 * Extract an OID index from the incoming OID instance starting
 * at the postion indicated by the index parameter. returns 1 on success,
 * 0 on failure.
 * 
 * ** On success, the normal return value will be '1' unless for a NEXT type 
 * search, if the index value is out-of-range then the return value will be
 * '2' to indicate that any forward instances should be cleared (zero-ed out) 
 */
int
InstToImpliedOID(inst, index, os, searchType, carry)
    const OID      *inst;
    int             index;
    OID           **os;
    int             searchType;
    int            *carry;
{
    int             expected_length;
    int             i;
    SR_UINT32       tmp;
    int             out_of_range_increment = 0;

    if (os == NULL) {
	goto fail;
    }
    if (searchType == GET_REQUEST_TYPE && *carry) {
	goto fail;
    }

    if (index >= inst->length) {
	if (searchType == GET_REQUEST_TYPE) {
	    goto fail;
	}
        *carry = 0;
        if(((*os) = MakeOID(NULL, 0)) == NULL) {
            DPRINTF((APWARN,"InstToImpliedOID: MakeOID failed\n"));
            goto fail;
        }
	return 1;
    }

    expected_length = inst->length - index;

    if(((*os) = MakeOID(NULL, expected_length + 1)) == NULL) {
	DPRINTF((APWARN, "InstToImpliedOID: MakeOID failed\n"));
	goto fail;
    }
    (*os)->length = 0;

    if (searchType == GET_REQUEST_TYPE) {
	for (i = expected_length - 1; i >= 0; i--) {
	    (*os)->oid_ptr[i] = inst->oid_ptr[index + i];
	}
        (*os)->length = expected_length;
	return 1;
    }
    else {			/* must be a NEXT */
	(*os)->oid_ptr[expected_length] = ((SR_INT32)0);
	*carry = 0;

	((*os)->length) = expected_length + 1;
	for (i = expected_length - 1; i >= 0; i--) {
	    tmp = ((index + i) < inst->length) ? 
		inst->oid_ptr[index + i] : ((SR_INT32)0);
	    if ((tmp == MAX_UINT32) && (*carry)) {
		((*os)->length) = i;
		*carry = 1;
		out_of_range_increment = 1;
	    }
	    else {
		(*os)->oid_ptr[i] = tmp + *carry;
		*carry = 0;
	    }
	}
    }

    if (!out_of_range_increment) {
        /* normal case */
        return 1;
    } else {
        /* return indication that forward indices should be cleared */
        return 2;
    }

fail:
    if (os != NULL && *os != NULL) {
	FreeOID(*os);
	*os = NULL;
    }
    return -1;
}

/*
 * InstToVariableOID:
 *
 * Extract an OID index from the incoming OID instance starting
 * at the postion indicated by the index parameter. returns 1 on success,
 * 0 on failure.
 * 
 * ** On success, the normal return value will be '1' unless for a NEXT type 
 * search, if the index value is out-of-range then the return value will be
 * '2' to indicate that any forward instances should be cleared (zero-ed out) 
 */
int
InstToVariableOID(inst, index, os, searchType, carry)
    const OID      *inst;
    int             index;
    OID           **os;
    int             searchType;
    int            *carry;
{
    int             length, expected_length;
    int             i, j;
    int             out_of_range_increment = 0;
    SR_UINT32       tmp;

    if (os == NULL) {
	goto fail;
    }
    if (searchType == GET_REQUEST_TYPE && *carry) {
	goto fail;
    }

    if (index >= inst->length) {
	if (searchType == GET_REQUEST_TYPE) {
	    goto fail;
	}
	*carry = 0;
        if(((*os) = MakeOID(NULL, 0)) == NULL) {
    	    DPRINTF((APWARN, "InstToVariableOID: MakeOID failed\n"));
	    goto fail;
        }
	return 1;
    }

    expected_length = inst->oid_ptr[index];

    if ((expected_length < 0) || (expected_length > MAX_OID_SIZE)) {
        if (searchType == GET_REQUEST_TYPE) {
            goto fail;
        } else {
            *carry = 1;
            if (((*os) = MakeOID(NULL, 0)) == NULL) {
                goto fail;
            }
            /* return indication that forward indices should be cleared */
            return 2;
        }
    }

    if(((*os) = MakeOID(NULL, expected_length + 1)) == NULL) {
	DPRINTF((APWARN, "InstToVariableOID: MakeOID failed\n"));
	goto fail;
    }
    (*os)->length = 0;

    length = inst->length - index;

    if (searchType == GET_REQUEST_TYPE) {
	if ((length < expected_length) || *carry) {
	    goto fail;
	}

        ((*os)->length) = expected_length;
	for (i = expected_length; i > 0; i--) {
	    (*os)->oid_ptr[i-1] = inst->oid_ptr[index + i];
	}
	return 1;
    }
    else {			/* must be a NEXT */
        ((*os)->length) = expected_length;
	for (i = expected_length; i > 0; i--) {
	    tmp = ((index + i) < inst->length) ? 
		inst->oid_ptr[index + i] : ((SR_INT32)0);
	    if ((i == 0) && (*carry)) {
		((*os)->length)++;
	    }
	    if ((tmp == MAX_UINT32) && (*carry)) {
		for (j = i; j <= expected_length; j++) {
		    (*os)->oid_ptr[j-1] = (SR_UINT32) 0;
		}
		*carry = 1;
		out_of_range_increment = 1;
		if (i == 0) {
		    ((*os)->length) = ((SR_INT32)1);
		}
	    }
	    else {
		(*os)->oid_ptr[i-1] = (SR_UINT32) (tmp + *carry);
		*carry = 0;
	    }
	}
    }

    if (!out_of_range_increment) {
        /* normal case */
        return 1;
    } else {
        /* return indication that forward indices should be cleared */
        return 2;
    }

fail:
    if (os != NULL && *os != NULL) {
	FreeOID(*os);
	*os = NULL;
    }
    return -1;
}

/*
 * GetVariableIndexLength:
 *
 * return the length of a variable length OctetString
 */
int
GetVariableIndexLength(inst, index)
    const OID      *inst;
    int             index;
{
    if (index >= inst->length) {
	return 0;
    }

    return (inst->oid_ptr[index] + 1);
}

/*
 * GetImpliedIndexLength:
 *
 * return the length of an IMPLIED OctetString
 */
int
GetImpliedIndexLength(inst, index)
    const OID      *inst;
    int             index;
{
    if (index >= inst->length) {
	return 0;
    }

    return (inst->length - index);
}

/*
 * CmpOIDInst: compares instance information (from INDEX clause) of ptr1 and 
 *             ptr2 where the instance starts at index. Returns 0 if instance 
 *             of ptr1 have the same length and values as the instance of ptr2,
 *             < 0 if instance of ptr1 is less than (or "smaller") than the 
 *             instance of ptr2, otherwise > 0.
 */
int
CmpOIDInst(ptr1, ptr2, index)
    const OID      *ptr1, *ptr2;
    int             index;
{
    int             i;
    int             min;

    if (!ptr1 && !ptr2) {
	return 0;
    }
    else if (!ptr1) {
	return -1;
    }
    else if (!ptr2) {
	return 1;
    }

    min = (int) ((ptr1->length < ptr2->length) ? ptr1->length : ptr2->length);

    /* check for mismatched values */
    for (i = index; i < min; i++) {
	if (ptr1->oid_ptr[i] != ptr2->oid_ptr[i])
	    return ((int) (ptr1->oid_ptr[i] - ptr2->oid_ptr[i]));
    }

    /*
     * equal for as long as the shorter one holds out.  The longer should be
     * considered bigger
     */
    if (ptr1->length > ptr2->length)
	return (1);
    if (ptr1->length < ptr2->length)
	return (-1);

    /* they are equal for their entire mutual lengths */
    return (0);
}

