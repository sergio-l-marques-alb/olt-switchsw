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


/*
 * Arguments used to create this file:
 * -aggregate -row_status -userpart -separate_type_file 
 */

/*
 * If your implementation does NOT support a MIB variable, add a line in
 * the undefs.h file containing #undef I_that-variable-name.  For example,
 * if you do not support the sysDescr variable in the system family
 * add a line:
 *
 * #undef I_sysDescr
 *
 *
 */


#ifndef SR_SNMPSUPP_H
#define SR_SNMPSUPP_H

#ifdef SR_IPv6_MIBSUPP
#ifdef SR_QUOTE
#include SR_QUOTE(SR_IPv6_MIBSUPP)
#else /* SR_QUOTE */
#include SR_IPv6_MIBSUPP
#endif /* SR_QUOTE */
#include "ipv6/undefs.h"
#ifdef I_ifEntry_max
#undef I_ifEntry_max
#endif /* I_ifEntry_max */
#ifdef I_ip_max
#undef I_ip_max
#endif /* I_ip_max */
#ifdef I_tcp_max
#undef I_tcp_max
#endif /* I_tcp_max */
#ifdef I_udp_max
#undef I_udp_max
#endif /* I_udp_max */
#endif /* SR_IPv6_MIBSUPP */

#include "coresupp.h"

 


#include "coex/coexsupp.h"
 
#include "snmpv3/v3supp.h" 
 
 
#ifdef EXTEND_BASE_SUPP 
#ifdef SR_QUOTE
#include SR_QUOTE(EXTEND_BASE_SUPP)
#else /* SR_QUOTE */
#include EXTEND_BASE_SUPP
#endif /* SR_QUOTE */
#endif /* EXTEND_BASE_SUPP */



#endif /* SR_SNMPSUPP_H */
