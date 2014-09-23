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

#ifndef SR_SRI_MAKE_LIB_H
#define SR_SRI_MAKE_LIB_H

#ifdef  __cplusplus
extern "C" {
#endif


struct _OctetString *MakeOctetString_track
    SR_PROTOTYPE((const unsigned char *string,
                  SR_INT32 length, char *file, unsigned int line));
#define MakeOctetString(string, length) MakeOctetString_track(string, length, __FILE__, __LINE__)

struct _OctetString *MakeOctetString2
    SR_PROTOTYPE((const unsigned char *string,
                  SR_INT32 length));

struct _ObjectSyntax *CloneObjectSyntax
    SR_PROTOTYPE((const struct _ObjectSyntax *os));


OctetString *CloneOctetString_track
    SR_PROTOTYPE((const OctetString *os_ptr, char * file, unsigned int line));
#define CloneOctetString(os_ptr) CloneOctetString_track(os_ptr, __FILE__, __LINE__)

struct _OID *MakeOID_track
    SR_PROTOTYPE((const SR_UINT32 sid[],
                  SR_INT32 length, char *file, unsigned int line));

#define MakeOID(sid, length) MakeOID_track(sid, length, __FILE__, __LINE__)

struct _VarBind *MakeVarBindWithValue
    SR_PROTOTYPE((const struct _OID *object,
                  const struct _OID *instance,
                  int type,
                  void *value));

struct _VarBind *MakeVarBindWithNull
    SR_PROTOTYPE((const struct _OID *object,
                  const struct _OID *instance));

struct _VarBind *MakeVarBindWithObjectSyntax
    SR_PROTOTYPE((const OID *object,
                  const OID *instance,
                  ObjectSyntax *value));


struct _Pdu *MakePdu
    SR_PROTOTYPE((int type,
                  SR_INT32 request_id,
                  SR_INT32 error_status_or_non_repeaters,
                  SR_INT32 error_index_or_max_repetitions,
                  struct _OID *enterprise,
                  struct _OctetString *agent_addr,
                  SR_INT32 generic_trap,
                  SR_INT32 specific_trap,
                  SR_UINT32 time_ticks));

short LinkVarBind
    SR_PROTOTYPE((struct _Pdu *pdu_ptr,
                  struct _VarBind *vb_ptr));

struct _OctetString *MakeOctetStringFromText_track
    SR_PROTOTYPE((const char *text_str, char * file, unsigned int line));

#define MakeOctetStringFromText(text_str) MakeOctetStringFromText_track(text_str, __FILE__, __LINE__)

struct _OctetString *MakeOctetStringFromHex_track
    SR_PROTOTYPE((const char *text_str, char * file, unsigned int line));

#define MakeOctetStringFromHex(text_str) MakeOctetStringFromHex_track(text_str, __FILE__, __LINE__)

struct _OID *MakeOIDFromHex
    SR_PROTOTYPE((const unsigned char *text_str));

struct _OID *CloneOID_track
    SR_PROTOTYPE((const struct _OID *oid_ptr, char *file, unsigned int line));

#define CloneOID(oid_ptr) CloneOID_track(oid_ptr, __FILE__, __LINE__)

struct _OID *ClonePartialOID_track
    SR_PROTOTYPE((const struct _OID *oid_ptr,
                  int start,
                  int len, char *file, unsigned int line));

#define ClonePartialOID(oid_ptr, start, len) ClonePartialOID_track(oid_ptr, start, len, __FILE__, __LINE__)

struct _OID *MakeLenOID
    SR_PROTOTYPE((const struct _OID *oida));

struct _OID *MakeOIDSubLen
    SR_PROTOTYPE((const struct _OID *oida));

struct _OID *CatOID
    SR_PROTOTYPE((const struct _OID *oida,
                  const struct _OID *oidb));

struct _VarBind *CloneVarBindList
    SR_PROTOTYPE((const struct _VarBind *in_vb));

struct _VarBind *CloneVarBind
    SR_PROTOTYPE((const struct _VarBind *in_vb));

#ifdef SR_SNMPv2_PDU
struct _UInt64 *CloneUInt64
    SR_PROTOTYPE((const struct _UInt64 *value));
#endif	/* SR_SNMPv2_PDU */

char *MakeTextFromOctetString
    SR_PROTOTYPE((const OctetString *os));

struct _BitString *CloneBitString
    SR_PROTOTYPE((struct _BitString *_bitstring_value));

struct _OID *MakeOIDFromDot_track
    SR_PROTOTYPE((const char *_text_str, char * file, unsigned int line));
#define MakeOIDFromDot(_text_str) MakeOIDFromDot_track(_text_str, __FILE__, __LINE__)

struct _OID *MakeOIDFragFromDot_track
    SR_PROTOTYPE((const char *_text_str, char * file, unsigned int line));
#define MakeOIDFragFromDot(_text_str) MakeOIDFragFromDot_track(_text_str, __FILE__, __LINE__)

int CmpOctetStrings
    SR_PROTOTYPE((const OctetString *o1,
                  const OctetString *o2));

int CmpOctetStringsWithLen
    SR_PROTOTYPE((const OctetString *o1,
                  const OctetString *o2));

char *GetBaseOIDString
    SR_PROTOTYPE((const OID *oid));


struct _UInt64 *MakeCounter64_track
	SR_PROTOTYPE((SR_UINT32 value, char *file, unsigned int line ));

#define MakeCounter64(value) MakeCounter64_track(value, __FILE__, __LINE__)

struct _UInt64 *MakeUInt64_track(SR_UINT32 low, SR_UINT32 high, char *file, unsigned int line);

#define MakeUInt64(low, high) MakeUInt64_track(low, high, __FILE__, __LINE__)

VarBind *FindLastVarBind
         (VarBind *vb);







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
UInt64 *MakeUInt64FromText_track
       SR_PROTOTYPE((const char *string, UInt64 *result, char *file, unsigned int line));

#define MakeUInt64FromText(string, result) MakeUInt64FromText_track(string, result, __FILE__, __LINE__)

char *OctetStringToTextString
       SR_PROTOTYPE((OctetString *os_ptr));


#ifdef  __cplusplus
}
#endif

/* DO NOT PUT ANYTHING AFTER THIS ENDIF */
#endif				/* SR_SRI_MAKE_LIB_H */
