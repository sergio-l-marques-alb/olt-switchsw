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

#ifndef SR_PRNT_LIB_H
#define SR_PRNT_LIB_H

#ifdef  __cplusplus
extern "C" {
#endif

void PrintPacket(const unsigned char *ptr, SR_INT32 len);

void PrintOctetString(const struct _OctetString *oct_ptr, int wrap);

int PrintVarBindList(const struct _VarBind *vb_ptr);

int PrintVarBind(const struct _VarBind *vb_ptr);

int PrintVarBindValue(const struct _VarBind *vb_ptr, const char *);

int PrintAscii(const struct _OctetString *os_ptr);

void PrintErrorCode(SR_INT32 error_index, SR_INT32 error_status);

int PrintCounter64(UInt64 *c64_ptr);

void math_loop(unsigned long int a, unsigned long int nums[7]);

int PrintFormattedVarBindList(const VarBind *vb_ptr, 
                              const int format,
                              int nested_level);

int PrintFormattedVarBind(const VarBind *vb_ptr,
                          const VarBind *prev_vb,
                          const int format,
                          int nested_level);

int PrintFormattedVarBindValue(const VarBind *vb_ptr,
                          const char *name,
                          const int format,
                          int nested_level);

#ifdef SR_DEBUG
void PrintOID(const OID *oid);
#endif	/* SR_DEBUG */

char *inet_ltoa(SR_UINT32 l);

#ifdef  __cplusplus
}
#endif

#endif				/* !defined SR_PRNT_LIB_H */
