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

#include "sr_conf.h"

#include <stdio.h>


#include <ctype.h>

#include <stdlib.h>

#include <malloc.h>

#include <string.h>


#include <stddef.h>

#include "sr_snmp.h"
#include "sr_trans.h"
#include "lookup.h"
#include "v2table.h"
#include "diag.h"
SR_FILENAME
#include "snmpv3/v3type.h"
#include "v3_vta.h"
#include "v3_tag.h"

SR_INT32
SrV3ValidateTransportAddress(
    void *vta_data,
    TransportInfo *srcTI,
    OctetString *selector)
{
    SnmpV2Table *tt;
    snmpTargetAddrEntry_t *stae;
    int i;
    TransportInfo ti, mask;

    tt = (SnmpV2Table *)vta_data;
    for (i = 0; i < tt->nitems; i++) {
        stae = (snmpTargetAddrEntry_t *)tt->tp[i];
        if (!SrTagInTagList(selector, stae->snmpTargetAddrTagList)) {
            continue;
        }
        memset(&ti, '\0', sizeof(ti));
        memset(&mask, '\0', sizeof(mask));
        if (OIDAndOctetStringToTransportInfo(&ti,
                stae->snmpTargetAddrTDomain,
                stae->snmpTargetAddrTAddress)) {
            continue;
        }
        if (TMaskOIDAndOctetStringToTransportInfo(&mask,
                stae->snmpTargetAddrTDomain,
                stae->snmpTargetAddrTMask)) {
            continue;
        }
        if (CmpTransportInfoWithMask(srcTI, &ti, &mask) == 0) {
            return stae->snmpTargetAddrMMS;
        }
    }

    return 0;
}
