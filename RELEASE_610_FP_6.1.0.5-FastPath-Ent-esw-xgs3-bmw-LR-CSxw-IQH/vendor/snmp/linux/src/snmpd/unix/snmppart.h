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
 */

#ifndef SR_SNMPPART_H
#define SR_SNMPPART_H

#ifdef SR_IPv6_MIBPART
#define SR_STipv6_EXTERN
#ifdef SNMPPART_DECL
#undef SNMPPART_DECL
#ifdef SR_QUOTE
#include SR_QUOTE(SR_IPv6_MIBPART)
#else /* SR_QUOTE */
#include SR_IPv6_MIBPART
#endif /* SR_QUOTE */
#define SNMPPART_DECL
#else /* SNMPPART_DECL */
#ifdef SR_QUOTE
#include SR_QUOTE(SR_IPv6_MIBPART)
#else /* SR_QUOTE */
#include SR_IPv6_MIBPART
#endif /* SR_QUOTE */
#endif /* SNMPPART_DECL */
#undef SR_STipv6_EXTERN
#ifdef STdeclare
#undef STdeclare
#endif /* STdeclare */
#endif /* SR_IPv6_MIBPART */

#include "corepart.h"




#include "coex/coexpart.h"
  
#include "snmpv3/v3part.h"
 
 
#ifdef EXTEND_BASE_PART
#ifdef SR_QUOTE
#include SR_QUOTE(EXTEND_BASE_PART)
#else /* SR_QUOTE */
#include EXTEND_BASE_PART
#endif /* SR_QUOTE */
#endif /* EXTEND_BASE_PART */


/* DO NOT PUT ANYTHING AFTER THIS #endif */
#endif /* SR_SNMPPART_H */
