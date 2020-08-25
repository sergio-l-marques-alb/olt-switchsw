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
 * -f mib.cnf -o v3 -per_file_init -stubs_only -search_table (implies -row_status) 
 * -test_and_incr -parser -row_status -userpart -storage_type 
 */


/*
 * File Description:
 */

#include "sr_conf.h"

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <stddef.h>

#include "sr_snmp.h"
#include "sr_trans.h"
#include "lookup.h"
#include "v2table.h"
#include "scan.h"
#include "snmpv2.h"
#include "diag.h"
SR_FILENAME
#include "v3type.h"

const SnmpType snmpEngineBootsType[] = {
    { INTEGER_TYPE, SR_READ_WRITE, (unsigned short) -1, -1 },
    { -1, -1, (unsigned short) -1, -1 }
};
#ifdef SR_CONFIG_FP
const PARSER_CONVERTER snmpEngineBootsConverters[] = {
    { ConvToken_integer, NULL },
    { NULL, NULL }
};
#endif /* SR_CONFIG_FP */

const SnmpType snmpEngineMaxMessageSizeType[] = {
    { INTEGER_TYPE, SR_READ_WRITE, (unsigned short) -1, -1 },
    { -1, -1, (unsigned short) -1, -1 }
};
#ifdef SR_CONFIG_FP
const PARSER_CONVERTER snmpEngineMaxMessageSizeConverters[] = {
    { ConvToken_integer, NULL },
    { NULL, NULL }
};
#endif /* SR_CONFIG_FP */

/* initialize support for snmpEngine objects */
int
i_snmpEngine_initialize(
    OctetString *snmpEngineID,
    snmpEngine_t *se)
{
    se->snmpEngineID = snmpEngineID;
    se->snmpEngineBoots = 0;
    se->snmpEngineTime = 0;
    se->snmpEngineMaxMessageSize = sr_pkt_size;
    return 1;
}

/* terminate support for snmpEngine objects */
int
i_snmpEngine_terminate(void)
{
    return 1;
}

int
i_arch_initialize(
    OctetString *snmpEngineID,
    snmpEngine_t *se)
{
    int status = 1;

    if (i_snmpEngine_initialize(snmpEngineID, se) != 1) {
        status = -1;
    }

    return status;
}

int
i_arch_terminate(void)
{
    int status = 1;

    if (i_snmpEngine_terminate() != 1) {
        status = -1;
    }

    return status;
}
