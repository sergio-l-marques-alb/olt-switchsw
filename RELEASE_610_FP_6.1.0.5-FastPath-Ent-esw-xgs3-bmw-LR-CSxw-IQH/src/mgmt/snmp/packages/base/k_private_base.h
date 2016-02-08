/*
 *
 * Copyright (C) 1992-1999 by SNMP Research, Incorporated.
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

#ifndef K_PRIVATE_BASE_H
#define K_PRIVATE_BASE_H

#include "sr_conf.h"

#ifdef HAVE_STDIO_H
  #include <stdio.h>
#endif /* HAVE_STDIO_H */
#ifdef HAVE_STDLIB_H
  #include <stdlib.h>
#endif /* HAVE_STDLIB_H */
#ifdef HAVE_STRING_H
  #include <string.h>
#endif /* HAVE_STRING_H */
#ifdef HAVE_MEMORY_H
  #include <memory.h>
#endif /* HAVE_MEMORY_H */
#ifdef HAVE_STDDEF_H
  #include <stddef.h>
#endif /* HAVE_STDDEF_H */
#include "sr_snmp.h"
#include "sr_trans.h"
#include "context.h"
#include "method.h"
#include "makevb.h"
#include "mibout.h"
#include "oid_lib.h"

#include "l7_common.h"
#include "l7_common_l3.h"
#include "snmp_api.h"

#include "usmdb_l3.h"
#include "osapi.h"

#define SNMP_BUFFER_LEN 512

#define SNMP_AGENT_MAX_PKT_LEN 2048

L7_BOOL
SafeMakeOctetString(OctetString **os_ptr, L7_uchar8 *string, L7_uint32 length);

L7_BOOL
SafeMakeOctetStringFromText(OctetString **os_ptr, L7_uchar8 *string);

L7_BOOL
SafeMakeOctetStringFromTextExact(OctetString **os_ptr, L7_uchar8 *string);

L7_BOOL
SafeMakeOIDFromDot(OID **oid_ptr, L7_uchar8 *string);

L7_BOOL
SafeMakeTimeTicksFromSeconds(L7_uint32 seconds, L7_uint32 *time_ticks);

L7_BOOL
SafeMakeDateAndTime(OctetString **os_ptr, L7_uint32 time);

#endif /* K_PRIVATE_BASE_H */
