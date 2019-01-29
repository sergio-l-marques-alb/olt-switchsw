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

#ifndef SR_OID_LIB_H
#define SR_OID_LIB_H

#ifdef  __cplusplus
extern "C" {
#endif

SR_INT32 FindMaxBitStringVal(const char *name);

SR_INT32 GetEnumVal(const char *name, const char *value);

OctetString *MakeBitsValue(const char *name, const char *value);

char *GetType(const char *string, SR_INT32 value);

char *GetBitsType(const char *string, const OctetString *value);

char *GetBitsTypeWithOID(const OID *oid, const OctetString *value);

int ParseSubIdHex(const unsigned char **temp_ptr, SR_UINT32 *value);

int ParseSubIdOctal(const unsigned char **temp_ptr, SR_UINT32 *value);

int ParseSubIdDecimal(const unsigned char **temp_ptr, SR_UINT32 *value);

short MakeDotFromOID(const struct _OID *oid_ptr, char *buffer);

short GetStringFromSubIds(char *string_buffer, SR_UINT32 *sid, int sid_counter);

#ifndef WANT_2L_FUNCS
int DecChar2L(const char *str, SR_UINT32 *value);

int HexChar2L(const char *str, SR_UINT32 *value);

int OctChar2L(const char *str, SR_UINT32 *value);

int Str2L(const char *str, int base, SR_UINT32 *value);
#endif /* WANT_2L_FUNCS */

int CmpNOID(const struct _OID *oida, const struct _OID *oidb, SR_INT32 length);

int Inst2IP(
    const SR_UINT32 *inst,
    int instLength,
    SR_UINT32 *ipAddr,
    int searchType);

int CmpOIDClass(const struct _OID *ptr1, const struct _OID *ptr2);

int CmpOID(const struct _OID *ptr1, const struct _OID *ptr2);

int CmpOIDWithLen(const struct _OID *oid1, const struct _OID *oid2);

int CheckOID(const struct _OID *oid1_ptr, const struct _OID *oid2_ptr);

OID *OctetStringToOID(struct _OctetString *os, int firstSIDIsLength);

#ifdef  __cplusplus
}
#endif

#endif				/* !defined SR_OID_LIB_H_ */
