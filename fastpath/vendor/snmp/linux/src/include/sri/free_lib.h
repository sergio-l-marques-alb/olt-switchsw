/*
 *
 * Copyright (C) 1992-2002 by SNMP Research, Incorporated.
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

#ifndef SR_SRI_FREE_LIB_H
#define SR_SRI_FREE_LIB_H

#ifdef  __cplusplus
extern "C" {
#endif

void FreeOctetString
    SR_PROTOTYPE((struct _OctetString *os_ptr));

void FreeObjectSyntax
    SR_PROTOTYPE((struct _ObjectSyntax *os));

void FreeOID
    SR_PROTOTYPE((struct _OID *oid_ptr));

void FreeUInt64
    SR_PROTOTYPE((struct _UInt64 *uint64_ptr));

void FreeBitString
    SR_PROTOTYPE((struct _BitString *bitstring_ptr));

void FreeVarBind
    SR_PROTOTYPE((struct _VarBind *vb_ptr));

void FreePdu
    SR_PROTOTYPE((struct _Pdu *pdu_ptr));

void FreeVarBindList
    SR_PROTOTYPE((struct _VarBind *vb_ptr));

void FreeCounter64
    SR_PROTOTYPE((struct _UInt64 *Counter64_ptr));

#ifdef  __cplusplus
}
#endif

/* DO NOT PUT ANYTHING AFTER THIS ENDIF */
#endif				/* SR_SRI_FREE_LIB_H */
