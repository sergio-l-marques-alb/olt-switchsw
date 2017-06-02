/*
 *
 * Copyright (C) 1992-2003 by SNMP Research, Incorporated.
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

/*
 * undefs.h: This file should contain a list of the variables not supported
 *            by the agent. 
 *
 * If your implementation does NOT support a MIB variable, add a line to
 * this file that contains #undef I_that-variable-name.  For example,
 * if you do not support the sysDescr variable in the system family
 * add a line:
 *
 * #undef I_sysDescr
 *
 * The initial list of supported variables is in snmpsupp.h.
 */


#ifndef SR_UNDEFS_H
#define SR_UNDEFS_H

#ifdef  __cplusplus
extern "C" {
#endif

/* atEntry_table_get and atEntry_row_get does not yet work with atTable */
#undef I_atEntry
#undef I_atTable

#include "../mibs/snmpv3/undefs.h"
#include "../mibs/coex/undefs.h"

#ifndef SR_SYS_OR_TABLE
#undef I_sysORLastChange
#undef I_sysORIndex
#undef I_sysORID
#undef I_sysORDescr
#undef I_sysORUpTime
#endif /* SR_SYS_OR_TABLE */






























#undef I_egpInMsgs
#undef I_egpInErrors
#undef I_egpOutMsgs
#undef I_egpOutErrors
#undef I_egpAs

#undef I_egpNeighTable
#undef I_egpNeighEntry
#undef I_egpNeighState
#undef I_egpNeighAddr
#undef I_egpNeighAs
#undef I_egpNeighInMsgs
#undef I_egpNeighInErrs
#undef I_egpNeighOutMsgs
#undef I_egpNeighOutErrs
#undef I_egpNeighInErrMsgs
#undef I_egpNeighOutErrMsgs
#undef I_egpNeighStateUps
#undef I_egpNeighStateDowns
#undef I_egpNeighIntervalHello
#undef I_egpNeighIntervalPoll
#undef I_egpNeighMode
#undef I_egpNeighEventTrigger



/* LVL7
#undef I_ipAdEntReasmMaxSize
#undef I_ipRoutingDiscards
#undef I_ipRouteAge

#undef I_ifInUcastPkts
#undef I_ifInNUcastPkts
#undef I_ifInUnknownProtos
#undef I_ifOutUcastPkts
#undef I_ifOutNUcastPkts
#undef I_ifOutQLen
*/




#ifdef  __cplusplus
}
#endif

/* DO NOT PUT ANYTHING AFTER THIS #endif */
#endif	/* SR_UNDEFS_H */
