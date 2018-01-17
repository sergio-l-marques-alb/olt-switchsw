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


#include "sr_conf.h"
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
#define SNMP_IP4_STR_LEN 20

#define SNMP_AGENT_MAX_PKT_LEN 2048

L7_BOOL
SafeMakeOctetString_track(OctetString **os_ptr, L7_uchar8 *string, L7_uint32 length, char * file, unsigned int line);
#define SafeMakeOctetString(os_ptr, string, length) SafeMakeOctetString_track(os_ptr, string, length, __FILE__, __LINE__)

L7_BOOL
SafeMakeOctetStringFromText_track(OctetString **os_ptr, L7_uchar8 *string, char * file, unsigned int line);
#define SafeMakeOctetStringFromText(os_ptr, string) SafeMakeOctetStringFromText_track(os_ptr, string, __FILE__, __LINE__)

L7_BOOL
SafeMakeOctetStringFromTextExact_track(OctetString **os_ptr, L7_uchar8 *string, char * file, unsigned int line);
#define SafeMakeOctetStringFromTextExact(os_ptr, string) SafeMakeOctetStringFromTextExact_track(os_ptr, string, __FILE__, __LINE__)

L7_BOOL
SafeMakeOIDFromDot_track(OID **oid_ptr, L7_uchar8 *string, char * file, unsigned int line);
#define SafeMakeOIDFromDot(oid_ptr, string) SafeMakeOIDFromDot_track(oid_ptr, string, __FILE__, __LINE__)

L7_BOOL
SafeMakeTimeTicksFromSeconds(L7_uint32 seconds, L7_uint32 *time_ticks);

L7_BOOL
SafeMakeDateAndTime_track(OctetString **os_ptr, L7_uint32 time, char * file, unsigned int line);
#define SafeMakeDateAndTime(os_ptr, time) SafeMakeDateAndTime_track(os_ptr, time, __FILE__, __LINE__)

/*********************************************************************
* @purpose  Add an entry for auditing
*
* @param    L7_char8 *strInput  - input string
* @param    L7_char8 *userName  - user name for SNMP v3 , community otherwise
* @param    L7_char8 *ipAddr    - string that contain IP address
*
* @returns 
*
* @notes
*
* @end
*********************************************************************/
void SnmpCmdLoggerAuditEntryAdd(ContextInfo *contextInfo, const L7_char8 *strInput);

/*********************************************************************
 * @purpose  Helper function for retrieving transfer info during 
 *           upload/download file to/from the switch
 *
 * @param    L7_BOOL    isUpload    determine if upload/download
 *                                  transfer info should be retrieved
 * @param    L7_char8   *outputStr  pointer to string where transfer info 
 *                                  will be stored
 * @param    L7_uint32  size        maximum size of outputStr
 *
 * @returns  L7_SUCCESS or L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t snmpTransferInfoGet(L7_BOOL isUpload, L7_char8 *outputStr, L7_uint32 size);

L7_BOOL
SafeMakeTimeRangeAbsoluteDateAndTime_track(OctetString **os_ptr,
                                          L7_uint32 year,
                                          L7_uchar8 month,
                                          L7_uchar8 day,
                                          L7_uchar8 hour,
                                          L7_uchar8 minute,
                                          L7_char8 *file, 
                                          L7_uint32 line);
#define SafeMakeTimeRangeAbsoluteDateAndTime(os_ptr, year, month, day, hour, minute) \
        SafeMakeTimeRangeAbsoluteDateAndTime_track(os_ptr, year, month, day, hour, minute, __FILE__, __LINE__)

L7_BOOL
SafeMakeTimeRangePeriodicTime_track(OctetString **os_ptr,
                                      L7_uchar8 hour,
                                      L7_uchar8 minute,
                                      L7_char8 *file, 
                                      L7_uint32 line);

#define SafeMakeTimeRangePeriodicTime(os_ptr, hour, minute) \
        SafeMakeTimeRangePeriodicTime_track(os_ptr, hour, minute, __FILE__, __LINE__)
#endif /* K_PRIVATE_BASE_H */
