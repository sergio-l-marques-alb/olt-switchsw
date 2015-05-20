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


/*
 * Arguments used to create this file:
 * -apo -f mibcompflags -userpart -v_stubs_only 
 */

#include "sr_conf.h"

#include <stdio.h>

#include <stdlib.h>

#include <malloc.h>

#include <string.h>


#include <stddef.h>
#include "sr_snmp.h"
#include "sr_trans.h"
#include "context.h"
#include "method.h"
#include "makevb.h"
#include "inst_lib.h"
#include "oid_lib.h"
#include "lookup.h"
#include "v2table.h"
#include "diag.h"
SR_FILENAME
#include "min_v.h"
#include "mibout.h"

static const SR_UINT32 udp_last_sid_array[] = {
    1, /* udpInDatagrams */
    2, /* udpNoPorts */
    3, /* udpInErrors */
    4  /* udpOutDatagrams */
};

const SnmpType udpTypeTable[] = {
    { COUNTER_TYPE, SR_READ_ONLY, offsetof(udp_t, udpInDatagrams), -1 },
    { COUNTER_TYPE, SR_READ_ONLY, offsetof(udp_t, udpNoPorts), -1 },
    { COUNTER_TYPE, SR_READ_ONLY, offsetof(udp_t, udpInErrors), -1 },
    { COUNTER_TYPE, SR_READ_ONLY, offsetof(udp_t, udpOutDatagrams), -1 },
    { -1, -1, (unsigned short) -1, -1 }
};

const SrGetInfoEntry udpGetInfo = {
    (SR_KGET_FPTR) new_k_udp_get,
    (SR_FREE_FPTR) NULL,
    (int) sizeof(udp_t),
    I_udp_max,
    (SnmpType *) udpTypeTable,
    NULL,
    (short) offsetof(udp_t, valid)
};

/*---------------------------------------------------------------------
 * Retrieve data from the udp family.
 *---------------------------------------------------------------------*/
VarBind *
udp_get(OID *incoming, ObjectInfo *object, int searchType,
        ContextInfo *contextInfo, int serialNum)
{
    return (v_get(incoming, object, searchType, contextInfo, serialNum,
                  (SrGetInfoEntry *) &udpGetInfo));
}

udp_t *
new_k_udp_get(int serialNum, ContextInfo *contextInfo,
              int nominator, int searchType,
              udp_t *data)
{
    return k_udp_get(serialNum, contextInfo, nominator);
}

static const SR_UINT32 udpEntry_last_sid_array[] = {
    1, /* udpLocalAddress */
    2  /* udpLocalPort */
};

const SnmpType udpEntryTypeTable[] = {
    { IP_ADDR_PRIM_TYPE, SR_READ_ONLY, offsetof(udpEntry_t, udpLocalAddress), 0 },
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(udpEntry_t, udpLocalPort), 1 },
    { -1, -1, (unsigned short) -1, -1 }
};

const SrIndexInfo udpEntryIndexInfo[] = {
#ifdef I_udpLocalAddress
    { I_udpLocalAddress, T_uint, -1 },
#endif /* I_udpLocalAddress */
#ifdef I_udpLocalPort
    { I_udpLocalPort, T_uint, -1 },
#endif /* I_udpLocalPort */
    { -1, -1, -1 }
};

const SrGetInfoEntry udpEntryGetInfo = {
    (SR_KGET_FPTR) new_k_udpEntry_get,
    (SR_FREE_FPTR) NULL,
    (int) sizeof(udpEntry_t),
    I_udpEntry_max,
    (SnmpType *) udpEntryTypeTable,
    udpEntryIndexInfo,
    (short) offsetof(udpEntry_t, valid)
};

/*---------------------------------------------------------------------
 * Retrieve data from the udpEntry family.
 *---------------------------------------------------------------------*/
VarBind *
udpEntry_get(OID *incoming, ObjectInfo *object, int searchType,
             ContextInfo *contextInfo, int serialNum)
{
#if (! ( defined(I_udpLocalAddress) ) || ! ( defined(I_udpLocalPort) ))
    return NULL;
#else	/* (! ( defined(I_udpLocalAddress) ) || ! ( defined(I_udpLocalPort) )) */
    return (v_get(incoming, object, searchType, contextInfo, serialNum,
                  (SrGetInfoEntry *) &udpEntryGetInfo));
#endif	/* (! ( defined(I_udpLocalAddress) ) || ! ( defined(I_udpLocalPort) )) */
}

udpEntry_t *
new_k_udpEntry_get(int serialNum, ContextInfo *contextInfo,
                   int nominator, int searchType,
                   udpEntry_t *data)
{
    if (data == NULL) {
        return NULL;
    }
    return k_udpEntry_get(serialNum, contextInfo, nominator,
                          searchType, data->udpLocalAddress,
                          data->udpLocalPort);
}


