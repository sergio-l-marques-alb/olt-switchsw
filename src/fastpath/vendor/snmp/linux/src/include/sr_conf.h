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

/*
 * This file in turn includes OS-specific .h configuration files as well
 * as the feature-based config file.
 *
 * This file should be included FIRST in every file (even before system
 * .h files) with a "#include "sr_conf.h"" directive.
 */

#ifndef SR_SR_CONF_H
#define SR_SR_CONF_H

#ifdef  __cplusplus
extern "C" {
#endif


#include "sr_vrsn.h"

#ifdef lint
#define const
#endif /* lint */

#if defined(SR_MGR)
/* This file includes common defines needed for a particular compile */
/* in order to save command line space when compiling on DOS systems. */
#ifdef VERSION_INCLUDE_FILE
#include VERSION_INCLUDE_FILE
#else /* VERSION_INCLUDE_FILE */
#include "tri.h"
#endif /* VERSION_INCLUDE_FILE */
#endif	/* defined(SR_MGR) */


#ifdef SR_SNMPv1_ADMIN
#ifdef SR_SNMPv1_PACKET_MIB
#undef SR_SNMPv1_PACKET_MIB
#endif /* SR_SNMPv1_PACKET_MIB */
#endif /* SR_SNMPv1_ADMIN */

#ifdef SR_SNMP_ADMIN_MIB
#ifdef SR_SNMPv3_ADMIN
#define SR_SNMPv3_ADMIN_MIB
#endif /* SR_SNMPv3_ADMIN_MIB */
#endif /* SR_SNMP_ADMIN_MIB */

#ifdef SR_NOTIFY_BASIC_WITH_FILTERS_COMPLIANCE
#define SR_NOTIFY_FILTERING
#endif /* SR_NOTIFY_BASIC_WITH_FILTERS_COMPLIANCE */
#ifdef SR_NOTIFY_FULL_COMPLIANCE
#define SR_NOTIFY_FILTERING
#endif /* SR_NOTIFY_FULL_COMPLIANCE */










#include "osconf/linux.h"








































/*
 * For now, if SELECT_BITMASK_TYPE is undefined, define it to fd_set*.
 * In the future, this may appear in each OS-specific *.h file.
 */
#ifndef SELECT_BITMASK_TYPE
#define SELECT_BITMASK_TYPE fd_set *
#endif /* SELECT_BITMASK_TYPE */

/*
 * SR_AGT_CONF_FILE_PREFIX: the directory (without the ending /) holding the
 * agent configuration files
 */


#ifndef SR_AGT_CONF_FILE_PREFIX
#define SR_AGT_CONF_FILE_PREFIX "/etc/srconf/agt"
#endif	/* SR_AGT_CONF_FILE_PREFIX */

/*
 * SR_MGR_CONF_FILE_PREFIX: the directory (without the trailing /)
 * holding the management configuration files
 */


#ifndef SR_MGR_CONF_FILE_PREFIX
#define SR_MGR_CONF_FILE_PREFIX "/etc/srconf/mgr"
#endif	/* SR_MGR_CONF_FILE_PREFIX */

/*
 * SCHEDULE_LINK_TIMEOUTS:  whether it is necessary to do timeout setups
 * after InitSubagent() is called, as it calls subagent initialization
 * routines before the timer initialization routines.
 */
#ifndef SCHEDULE_LINK_TIMEOUTS
#endif	/* SCHEDULE_LINK_TIMEOUTS */

/*
 * POLL_INTERFACES:  whether to include code to periodically poll the
 * interface configuration
 */
#ifndef POLL_INTERFACES
#if (defined(SR_SOL64) || defined(SR_LINUX))
/*#define POLL_INTERFACES */ /* LVL7 */
#endif	/* (defined(SR_SOL64) || defined(SR_LINUX)) */
#endif	/* POLL_INTERFACES */

/*
 * POLL_INTERVAL:  the number of seconds between polling of interfaces
 */
#ifndef POLL_INTERVAL
#define POLL_INTERVAL 5
#endif	/* POLL_INTERVAL */

#include "sr_featr.h"

#ifndef SR_CLEAR_MALLOC
#define SR_CLEAR(x)
#define SR_CLEAR_N(x,n)
#else /* SR_CLEAR_MALLOC */
#define SR_CLEAR(x) (memset((char *)(x), 0, sizeof(*(x))))
#define SR_CLEAR_N(x,n) (memset((char *)(x), 0, sizeof(*(x)) * (n)))
#endif /* SR_CLEAR_MALLOC */

/* This macro can be used for debugging purposes */
#define SR_DBGPRT printf("%s %d\n",__FILE__,__LINE__);

#ifdef  __cplusplus
}
#endif

/* DO NOT PUT ANYTHING AFTER THIS #endif */
#endif	/* SR_SR_CONF_H */
