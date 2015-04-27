
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

#ifndef SR_INST_LIB_H
#define SR_INST_LIB_H

#ifdef  __cplusplus
extern "C" {
#endif

/* Function  prototypes for the functions defined in inst_lib.c */


int InstToInt(
    const struct _OID *inst,
    int index,
    SR_INT32 *val,
    int searchType,
    int *carry);

int InstToUInt(
    const struct _OID *inst,
    int index,
    SR_UINT32 *val,
    int searchType,
    int *carry);

int InstToIP(
    const struct _OID *inst,
    int index,
    SR_UINT32 *ipAddr,
    int searchType,
    int *carry);

int InstToFixedOctetString(
    const struct _OID *inst,
    int index,
    struct _OctetString **os,
    int searchType,
    int *carry,
    int size);

int InstToNetworkAddress(
    const struct _OID *inst,
    int index,
    struct _OctetString **os,
    int searchType,
    int *carry);

int InstToImpliedOctetString(
    const struct _OID *inst,
    int index,
    struct _OctetString **os,
    int searchType,
    int *carry);

int InstToVariableOctetString(
    const struct _OID *inst,
    int index,
    struct _OctetString **os,
    int searchType,
    int *carry);

int InstToImpliedOID(
    const OID *inst,
    int index,
    OID **os,
    int searchType,
    int *carry);

int InstToVariableOID(
    const OID *inst,
    int index,
    OID **os,
    int searchType,
    int *carry);

int GetVariableIndexLength(const struct _OID *inst, int index);

int GetImpliedIndexLength(const struct _OID *inst, int index);

int CmpOIDInst(const struct _OID *ptr1, const struct _OID *ptr2, int index);

OctetString *IPToOctetString(SR_UINT32 IPAddr);

SR_UINT32 OctetStringToIP(const OctetString *os);
void OctetStringToIP6(const OctetString *os, unsigned char *buff);

SR_UINT32 NetworkAddressToIP(const OctetString *os);

SR_UINT32 OctetStringToUlong(const OctetString *os);


/* Function prototypes and macros specific to the ARL */

#define ITVB_CONVERT_LOCAL_ACCESSIBLE          0x00000001
#define ITVB_CONVERT_LOCAL_NOT_ACCESSIBLE      0x00000010
#define ITVB_CONVERT_EXTERNAL_ACCESSIBLE       0x00000100
#define ITVB_CONVERT_EXTERNAL_NOT_ACCESSIBLE   0x00001000
#define ITVB_CONVERT_LOCAL                     0x00000011
#define ITVB_CONVERT_EXTERNAL                  0x00001100
#define ITVB_CONVERT_ACCESSIBLE                0x00000101
#define ITVB_CONVERT_NOT_ACCESSIBLE            0x00001010
#define ITVB_CONVERT_ALL                       0x00001111

#define ITVB_SEARCH_BY_NAME                    0x01000000
#define ITVB_SEARCH_BY_POSITION                0x10000000

int
InstToVarBind(
    OID *incoming,
    VarBind **append_vblist,
    SR_UINT32 algorithm,
    OID *index_name_to_match,
    SR_UINT32 position_to_match);

int ExtractInstanceOID(OID *oida, OID **instance);


#ifdef  __cplusplus
}
#endif

/* DO NOT PUT ANYTHING AFTER THIS #endif */
#endif	/* SR_INST_LIB_H */
