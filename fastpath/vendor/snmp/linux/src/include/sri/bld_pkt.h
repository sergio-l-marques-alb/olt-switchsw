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

#ifndef SR_SRI_BLD_PKT_H
#define SR_SRI_BLD_PKT_H

#ifdef  __cplusplus
extern "C" {
#endif

short BuildPdu(
    struct _Pdu *pdu_ptr);

SR_INT32 FindLenVarBind(
    struct _VarBind *vb_ptr);


short FindLenBitString(
    struct _BitString *bitstring_ptr);

SR_INT32 FindLenOctetString(
    const struct _OctetString *os_ptr);

short FindLenOID(
    const struct _OID *oid_ptr);

short FindLenUInt(
    SR_UINT32 value);

short FindLenInt(
    SR_INT32 value);

short FindLenUInt64(
    const struct _UInt64 *value);

short DoLenLen(
    SR_INT32 len);

void AddLen(
    unsigned char **working_ptr,
    SR_INT32 lenlen,
    SR_INT32 data_len);

short AddVarBind(
    unsigned char **working_ptr,
    const struct _VarBind *vb_ptr,
    struct _Pdu *pdu_ptr);

short AddOctetString(
    unsigned char **working_ptr,
    int type,
    const struct _OctetString *os_ptr);

short AddOID(
    unsigned char **working_ptr,
    int type,
    const struct _OID *oid_ptr);


short AddUInt(
    unsigned char **working_ptr,
    int type,
    SR_UINT32 value);

short AddInt(
    unsigned char **working_ptr,
    int type,
    SR_INT32 value);

short AddUInt64(
    unsigned char **working_ptr,
    int type,
    const struct _UInt64 *value);

void AddNull(
    unsigned char **working_ptr,
    int type);





void NormalizeVarBindListLengths(
    struct _VarBind *vb_ptr);

#ifdef  __cplusplus
}
#endif

/* DO NOT PUT ANYTHING AFTER THIS ENDIF */
#endif				/* SR_SRI_BLD_PKT_H */
