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

#include <stdlib.h>

#include <malloc.h>


#include <string.h>


#include "sr_snmp.h"
#include "mib_tree.h"


/*
 * The following tables are used to translate type/access from the SNMPINFO
 * file into appropriate constants.
 */
static const char *type_names[] = {
        "Null",
        "nonLeaf",
        "Aggregate",
        "DisplayString",
        "OctetString",
        "PhysAddress",
        "NetworkAddress",
        "IpAddress",
        "ObjectID",
        "TimeTicks",
        "INTEGER",
        "Gauge",
        "Counter",
        "Services",
        "Counter32",
        "Gauge32",
        "Counter64",
        "Integer32",
        "Bits",
        "Unsigned32",
        (char *)0
};
static const short type_values[] = {
        NULL_TYPE,
        NON_LEAF_TYPE,
        AGGREGATE_TYPE,
        DISPLAY_STRING_TYPE,
        OCTET_PRIM_TYPE,
        PHYS_ADDRESS_TYPE,
        NETWORK_ADDRESS_TYPE,
        IP_ADDR_PRIM_TYPE,
        OBJECT_ID_TYPE,
        TIME_TICKS_TYPE,
        INTEGER_TYPE,
        GAUGE_TYPE,
        COUNTER_TYPE,
        SERVICES_TYPE,
#ifdef SR_SNMPv2_PDU
        COUNTER_32_TYPE,
        GAUGE_32_TYPE,
        COUNTER_64_TYPE,
        INTEGER_32_TYPE,
        BITS_TYPE,
        GAUGE_32_TYPE,
#else /* SR_SNMPv2_PDU */
        COUNTER_32_TYPE,
        GAUGE_32_TYPE,
        COUNTER_64_TYPE,
        INTEGER_TYPE,
        OCTET_PRIM_TYPE,
        GAUGE_32_TYPE,
#endif /* SR_SNMPv2_PDU */
        0
};
static const char *access_names[] = {
        "read-only",
        "write-only",
        "read-write",
        "read-create",
        "not-accessible",
        "accessible-for-notify",
        (char *)0
};
static const short access_values[] = {
        READ_ONLY_ACCESS,
        READ_WRITE_ACCESS,
        READ_WRITE_ACCESS,
        READ_CREATE_ACCESS,
        NOT_ACCESSIBLE_ACCESS,
        ACCESSIBLE_FOR_NOTIFY_ACCESS,
        0
};

/*
 * TypeStringToShort
 */
short
TypeStringToShort(str)
    const char *str;
{
    int i;
    for (i=0; type_names[i]!=NULL; i++) {
        if (!strcmp(type_names[i], str)) {
            return(type_values[i]);
        }
    }
    return(0);
}

/*
 * TypeShortToString
 */
const char *
TypeShortToString(shrt)
    int shrt;
{
    int i;
    for (i=0; type_values[i]!=0; i++) {
        if (type_values[i] == shrt) {
            return(type_names[i]);
        }
    }
    return(0);
}

/*
 * AccessStringToShort
 */
short
AccessStringToShort(str)
    const char *str;
{
    int i;
    for (i=0; access_names[i]!=NULL; i++) {
        if (!strcmp(access_names[i], str)) {
            return(access_values[i]);
        }
    }
    return(0);
}

/*
 * AccessShortToString
 */
const char *
AccessShortToString(shrt)
    int shrt;
{
    int i;
    for (i=0; access_values[i]!=0; i++) {
        if (access_values[i] == shrt) {
            return(access_names[i]);
        }
    }
    return(0);
}
