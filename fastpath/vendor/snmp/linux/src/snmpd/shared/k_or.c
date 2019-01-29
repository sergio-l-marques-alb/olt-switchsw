/*
 *
 * Copyright (C) 1992-2001 by SNMP Research, Incorporated.
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

#include <stdio.h>
#include <stdlib.h>

#include "sr_conf.h"
#include "sr_snmp.h"
#include "sr_proto.h"
#include "comunity.h"
#include "v2clssc.h"
#include "sr_trans.h"
#include "context.h"
#include "method.h"
#include "mibout.h"
/* LVL7 start */
#include "usmdb_common.h"
#include "usmdb_snmp_api.h"
#include "k_private_base.h"

#define SNMP_SYSOR_BUFFER_LEN 128
/* LVL7 end */

int
k_or_initialize(void)
{
   return 1;
}

int
k_or_terminate(void)
{
   return 1;
}

/* LVL7 start */
sysOREntry_t *
k_sysOREntry_get(serialNum, contextInfo, nominator, searchType, sysORIndex)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
   int searchType;
   SR_INT32 sysORIndex;
{
   static sysOREntry_t sysOREntryData;
  static L7_BOOL firstTime = L7_TRUE;
  L7_char8 snmp_buffer[SNMP_SYSOR_BUFFER_LEN];

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;
    sysOREntryData.sysORDescr = MakeOctetString(NULL, 0);
    sysOREntryData.sysORID = MakeOIDFromDot("0.0");
  }

  ZERO_VALID(sysOREntryData.valid);
  sysOREntryData.sysORIndex = sysORIndex;
  SET_VALID(I_sysORIndex, sysOREntryData.valid);

  if ( searchType == EXACT ? ( usmDbSnmpSupportedMibTableEntryGet(USMDB_UNIT_CURRENT, sysOREntryData.sysORIndex) != L7_SUCCESS) :
       ( usmDbSnmpSupportedMibTableEntryGet(USMDB_UNIT_CURRENT, sysOREntryData.sysORIndex) != L7_SUCCESS &&
         usmDbSnmpSupportedMibTableEntryNextGet(USMDB_UNIT_CURRENT, &sysOREntryData.sysORIndex) != L7_SUCCESS ) )
  {
    ZERO_VALID(sysOREntryData.valid);
    return(NULL);
  }

  switch (nominator)
  {
  case -1:
  case I_sysORIndex:
    break;

  case I_sysORID:
    bzero(snmp_buffer, SNMP_SYSOR_BUFFER_LEN);
    FreeOID(sysOREntryData.sysORID);
    if ( usmDbSnmpSupportedMibTableMibOIDGet(USMDB_UNIT_CURRENT, sysOREntryData.sysORIndex, snmp_buffer) == L7_SUCCESS )
    {
      if ( (sysOREntryData.sysORID = MakeOIDFromDot(snmp_buffer)) != NULL )
        SET_VALID(I_sysORID, sysOREntryData.valid);
    }
    else
    {
      sysOREntryData.sysORID = MakeOIDFromDot("0.0");
    }
    break;

  case I_sysORDescr:
    bzero(snmp_buffer, SNMP_SYSOR_BUFFER_LEN);
    if (usmDbSnmpSupportedMibTableMibDescriptionGet(USMDB_UNIT_CURRENT, sysOREntryData.sysORIndex, snmp_buffer) == L7_SUCCESS &&
        (SafeMakeOctetStringFromTextExact_track(&sysOREntryData.sysORDescr, snmp_buffer, __FILE__, __LINE__) == L7_TRUE))
      SET_VALID(I_sysORDescr, sysOREntryData.valid);
    break;

  case I_sysORUpTime:
    if (usmDbSnmpSupportedMibTableMibEntryCreationTimeGet(USMDB_UNIT_CURRENT, sysOREntryData.sysORIndex,
                                             &sysOREntryData.sysORUpTime) == L7_SUCCESS)
      SET_VALID(I_sysORUpTime, sysOREntryData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, sysOREntryData.valid))
    return(NULL);

   return(&sysOREntryData);
}
/* LVL7 end */
