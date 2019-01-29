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

#ifndef SR_SRI_PRSE_PKT_H
#define SR_SRI_PRSE_PKT_H

#ifdef  __cplusplus
extern "C" {
#endif

struct _Pdu *SrParsePdu(
    const unsigned char *packet,
    const SR_INT32 packet_len);

struct _VarBind *ParseVarBind
    SR_PROTOTYPE((const unsigned char **working_ptr,
                  const unsigned char *end_ptr));


struct _OctetString *ParseOctetString
    SR_PROTOTYPE((const unsigned char **working_ptr,
                  const unsigned char *end_ptr,
                  short *type));

struct _OID *ParseOID
    SR_PROTOTYPE((const unsigned char **working_ptr,
                  const unsigned char *end_ptr));


struct _UInt64 *ParseUInt64
    SR_PROTOTYPE((const unsigned char **working_ptr,
                  const unsigned char *end_ptr));

struct _BitString *ParseBitString
    SR_PROTOTYPE((const unsigned char **working_ptr,
                  const unsigned char *end_ptr,
                  short *type));

SR_UINT32 ParseUInt
    SR_PROTOTYPE((const unsigned char **working_ptr,
                  const unsigned char *end_ptr,
                  short *type));

SR_INT32 ParseInt
    SR_PROTOTYPE((const unsigned char **working_ptr,
                  const unsigned char *end_ptr,
                  short *type));

short ParseNull
    SR_PROTOTYPE((const unsigned char **working_ptr,
                  const unsigned char *end_ptr,
                  short *type));

SR_INT32 ParseSequenceType
    SR_PROTOTYPE((const unsigned char **working_ptr,
                  const unsigned char *end_ptr));

SR_INT32 ParseVarBindSequence
    SR_PROTOTYPE((const unsigned char **working_ptr,
                  const unsigned char *end_ptr,
                  SR_INT32 *type));

SR_INT32 ParseSequence
    SR_PROTOTYPE((const unsigned char **working_ptr,
                  const unsigned char *end_ptr,
                  short *type));

short ParseType
    SR_PROTOTYPE((const unsigned char **working_ptr,
                  const unsigned char *end_ptr));

SR_INT32 ParseLength
    SR_PROTOTYPE((const unsigned char **working_ptr,
                  const unsigned char *end_ptr));

SR_INT32 ParseImplicitSequence
    SR_PROTOTYPE((const unsigned char **working_ptr,
                  const unsigned char *end_ptr,
                  short *type));

#ifdef  __cplusplus
}
#endif

/* DO NOT PUT ANYTHING AFTER THIS ENDIF */
#endif				/* SR_SRI_PRSE_PKT_H */
