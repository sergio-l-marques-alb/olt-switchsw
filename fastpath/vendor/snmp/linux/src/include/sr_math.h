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

#ifndef SR_MATH_H
#define SR_MATH_H


#ifdef  __cplusplus
extern "C" {
#endif

UInt64 *Counter64Add
    SR_PROTOTYPE((UInt64 *c1, UInt64 *c2));

UInt64 *Counter64Subtract
    SR_PROTOTYPE((UInt64 *c1, UInt64 *c2));

UInt64 *Counter64Multiply
    SR_PROTOTYPE((UInt64 *c1, UInt64 *c2));   
 
SR_UINT32 DoMultiply
    SR_PROTOTYPE((SR_UINT32 c1_bits, SR_UINT32 c2_bits));

UInt64 *Counter64Divide
    SR_PROTOTYPE((UInt64 *c1, UInt64 *c2));   
 
SR_UINT32 DoDivide
    SR_PROTOTYPE((SR_UINT32 c1_bits, SR_UINT32 c2_bits));

int SetCounter64FromInt
    SR_PROTOTYPE((UInt64 *c1, SR_UINT32 value));

int SetCounter64FromC64
    SR_PROTOTYPE((UInt64 *c1, UInt64 *c2));

int IncrementCounter64
    SR_PROTOTYPE((UInt64 *c1));

int IncreaseCounter64
    SR_PROTOTYPE((UInt64 *c1, SR_UINT32 value));

UInt64 *Counter64Mod
    SR_PROTOTYPE((UInt64 *c1, UInt64 *c2));

#ifdef  __cplusplus
}
#endif

/* DO NOT PUT ANYTHING AFTER THIS ENDIF */
#endif				/* ! SR_MATH_H */
