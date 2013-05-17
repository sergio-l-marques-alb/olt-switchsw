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

#include "sr_conf.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include "sr_snmp.h"
#include "sr_trans.h"
#include "comunity.h"
#include "v2clssc.h"
#include "context.h"
#include "method.h"
#include "makevb.h"
#include "mibout.h"
#include "lookup.h"
#include "v2table.h"
#include "tmq.h"
#include "rowstatf.h"
#include "sr_cfg.h"
#include "scan.h"
#include "mapctx.h"
#include "diag.h"
#include "snmpv3/v3type.h"
#include "v3_vta.h"
#include "findcom.h"
SR_FILENAME

#ifdef SR_SNMPv3_ADMIN
extern SnmpV2Table agt_snmpCommunityTable; 
extern SnmpV2Table agt_snmpTargetAddrTable;
extern OctetString *agt_local_snmpID;

snmpCommunityEntry_t * 
FindSnmpCommunityEntry(
    const OctetString *contextName,
    snmpTargetParamsEntry_t *stpe,
    TransportInfo *ti)
{
    return FindSnmpCommEntryWithCtxEngID(contextName,
                                         stpe,
                                         ti,
                                         agt_local_snmpID);
}

snmpCommunityEntry_t * 
FindSnmpCommEntryWithCtxEngID(
    const OctetString *contextName,
    snmpTargetParamsEntry_t *stpe,
    TransportInfo *ti,
    OctetString *contextEngineID)
{
    FNAME("FindSnmpCommEntryWithCtxEngID")
    int i;
    snmpCommunityEntry_t *sce;
#ifdef SR_DEBUG
    char *securityName_token = NULL;
    char *engineID_token = NULL;
    char *contextName_token = NULL;
    char buf[64];

    ConvToken_textOctetString(PARSER_CONVERT_TO_TOKEN,
                          &securityName_token,
                          &stpe->snmpTargetParamsSecurityName);
    ConvToken_octetString(PARSER_CONVERT_TO_TOKEN,
                          &engineID_token,
                          &contextEngineID);
    ConvToken_octetString(PARSER_CONVERT_TO_TOKEN,
                          &contextName_token,
                          (OctetString *) &contextName);
    if (securityName_token != NULL && engineID_token != NULL) {
        DPRINTF((APCONFIG,
                 "%s: scanning snmpCommunityTable\n"
                 "      securityName = %s\n"
                 "   contextEngineID = %s\n"
                 "       contextName = %s\n"
                 "   network address = %s\n",
                 Fname,
                 securityName_token,
                 engineID_token,
                 contextName_token,
		 FormatTransportString(buf, 64, ti))); 
    }
    else {
        DPRINTF((APCONFIG, "%s: scanning snmpCommunityTable\n", Fname));
    }    
    if (securityName_token != NULL) {
        free(securityName_token);
        securityName_token = NULL;
    }
    if (engineID_token != NULL) {
        free(engineID_token);
        engineID_token = NULL;
    }
    if (contextName_token != NULL) {
        free(contextName_token);
        contextName_token = NULL;
    }
#endif /* SR_DEBUG */

    for (i = 0; i < agt_snmpCommunityTable.nitems; i++) {
        DPRINTF((APVERBOSE, "%s: scanning row %d\n", Fname, i));
        sce = (snmpCommunityEntry_t *)agt_snmpCommunityTable.tp[i];

        /* Make sure row is active */
        if (sce->snmpCommunityStatus != RS_ACTIVE) {
            DPRINTF((APVERBOSE,
                "%s: row %d does not have an active RowStatus value\n",
                Fname, i));
            continue;
        }

        /* Make sure security names match */
        if (sce->snmpCommunitySecurityName->length !=
                stpe->snmpTargetParamsSecurityName->length) {
            DPRINTF((APVERBOSE,
                "%s: snmpCommunitySecurityName does not match in row %d\n",
                Fname, i));
            continue;
        }
        if (memcmp(sce->snmpCommunitySecurityName->octet_ptr,
                   stpe->snmpTargetParamsSecurityName->octet_ptr,
                   stpe->snmpTargetParamsSecurityName->length)) {
            DPRINTF((APVERBOSE,
                "%s: snmpCommunitySecurityName does not match in row %d\n",
                Fname, i));
            continue;
        }

        /* Make sure contextEngineID values match */
        if (sce->snmpCommunityContextEngineID->length !=
                contextEngineID->length) {
            DPRINTF((APVERBOSE,
                "%s: snmpCommunityContextEngineID does not match in row %d\n",
                Fname, i));
            continue;
        }
        if (memcmp(sce->snmpCommunityContextEngineID->octet_ptr,
                   contextEngineID->octet_ptr, contextEngineID->length)) {
            DPRINTF((APVERBOSE,
                "%s: snmpCommunityContextEngineID does not match in row %d\n",
                Fname, i));
            continue;
        }
    
        /* Make sure contextName values match */
        if (contextName != NULL) {
            if (sce->snmpCommunityContextName->length != contextName->length) {
                DPRINTF((APVERBOSE,
                    "%s: snmpCommunityContextName does not match in row %d\n",
                    Fname, i));
                 continue;
             }
             if (contextName->length > 0) {
                 if (memcmp(sce->snmpCommunityContextName->octet_ptr,
                            contextName->octet_ptr, contextName->length)) {
                DPRINTF((APVERBOSE,
                    "%s: snmpCommunityContextName does not match in row %d\n",
                    Fname, i));
                     continue;
                 }
             }
         } else {
             if (sce->snmpCommunityContextName->length != 0) {
                 DPRINTF((APVERBOSE,
                     "%s: snmpCommunityContextName does not match in row %d\n",
                     Fname, i));
                 continue;
             }
         }

         if (ti != NULL) {
             /* Make sure transport matches */
             if (sce->snmpCommunityTransportTag->length > 0) {
                 if (!SrV3ValidateTransportAddress(
                         &agt_snmpTargetAddrTable, ti,
                         sce->snmpCommunityTransportTag)) {
                     DPRINTF((APVERBOSE,
                         "%s: snmpCommunityTransportTag does not "
                         "match in row %d\n",
                         Fname, i));
                     continue;
                 }
             }
         }
        
         DPRINTF((APCONFIG, "%s: row %d matched search criteria\n", Fname, i));
         DPRINTF((APCONFIG,
                  "%s: finished scanning snmpCommunityTable\n", Fname));
         return sce;
     }             
  
     DPRINTF((APCONFIG, "%s: NO MATCH FOUND\n", Fname, i));
     DPRINTF((APCONFIG, "%s: finished scanning snmpCommunityTable\n", Fname));
     return NULL;
}
#endif /* SR_SNMPv3_ADMIN */

