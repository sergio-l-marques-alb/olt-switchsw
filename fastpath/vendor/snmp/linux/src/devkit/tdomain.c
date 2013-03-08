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
 */

#include "sr_conf.h"

#include <stdio.h>

#include <stdlib.h>

#include <sys/types.h>


#include <string.h>

#include <malloc.h>

#include <stddef.h>

#include "sr_type.h"
#include "sr_time.h"

#include "sr_snmp.h"
#include "sr_proto.h"
#include "sr_trans.h"
#include "lookup.h"
#include "v2table.h"
#include "sr_cfg.h"
#include "scan.h"
#include "oid_lib.h"
#include "tdomain.h"
#include "sr_ip.h"

static OID *tdomain = NULL;

SR_INT32
ConvToken_TDomain(direction, token, value)
    SR_INT32 direction;
    char **token;
    void *value;
{
    OID **local = (OID **)value;
    SR_INT32 retval;

    tdomain = NULL;

    retval = ConvToken_OID(direction, token, value);
    if (!retval) {
        if (CmpOID(snmpUDPDomainOID, *local) == 0) {
            tdomain = (OID *) snmpUDPDomainOID;
        }
    }
    return retval;
}

SR_INT32
ConvToken_TAddr(direction, token, value)
    SR_INT32 direction;
    char **token;
    void *value;
{
    if (tdomain == snmpUDPDomainOID) {
        return ConvToken_IP_TAddr(direction, token, value);
    }
    return -1;
}

SR_INT32
ConvToken_TMask(direction, token, value)
    SR_INT32 direction;
    char **token;
    void *value;
{
    if (tdomain == snmpUDPDomainOID) {
        return ConvToken_IP_TAddr(direction, token, value);
    }
    return -1;
}

SR_INT32
ConvToken_TAddrMask(direction, token, value)
    SR_INT32 direction;
    char **token;
    void *value;
{
    if (tdomain == snmpUDPDomainOID) {
        return ConvToken_IP_TAddrMask(direction, token, value);
    }
    return -1;
}

