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

#include "sr_conf.h"

#include <stdio.h>

#include <sys/types.h>

#include "sr_proto.h"
#include "sr_type.h"
#include "sr_snmp.h"
#include "comunity.h"
#include "v2clssc.h"
#include "sr_trans.h"
#include "context.h"
#include "method.h"
#include "inst_lib.h"
#include "makevb.h"
#include "lookup.h"
#include "v2table.h"
#include "scan.h"
#include "min_v.h"

#define SNMPPART_DECL
#include "mibout.h"
#undef  SNMPPART_DECL

/* the objects internal to the agent */
#ifndef OIDLISTNAME
ObjectInfo      OidList[] = {
#else /* OIDLISTNAME */
ObjectInfo      OIDLISTNAME[] = {
#endif /* OIDLISTNAME */

#ifdef MIBOID
#ifdef SR_QUOTE
#include SR_QUOTE(MIBOID)
#else /* SR_QUOTE */
#include MIBOID
#endif /* SR_QUOTE */
#endif /* MIBOID */

    {{0, NULL},			/* The special "null"-terminator. */
#ifndef LIGHT
    NULL,
#endif /* LIGHT */
    0, 0, 0, 0,
    NULL, NULL}
};

