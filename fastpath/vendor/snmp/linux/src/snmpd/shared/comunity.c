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

#include <string.h>


#include <sys/types.h>


#include <stddef.h>


#include <malloc.h>

#include <stdlib.h>        /* needed for malloc proto */


#include <netinet/in.h>

#include <arpa/inet.h>     /* needed for inet_addr  */




#include "sr_type.h"
#include "sr_proto.h"
#include "sr_snmp.h"
#include "sr_trans.h"
#include "oid_lib.h"
#include "diag.h"
SR_FILENAME
#include "sr_cfg.h"
#include "lookup.h"
#include "v2table.h"
#include "scan.h"
#include "comunity.h"
#include "v2clssc.h"
#include "context.h"
#include "method.h"
#include "snmptype.h"
#include "snmpd.h"
#include "compat.h"


Index_t communitiesIndex[] = {
    { offsetof(COMMUNITY, community_name),  T_octet},
    { offsetof(COMMUNITY, ip_addr),  T_uint}
};

SnmpV2Table communities = {
    NULL,
    communitiesIndex,
    0,
    2,
    sizeof(COMMUNITY)
};

const SnmpType communitiesType[] = {
    { OCTET_PRIM_TYPE, SR_NOT_ACCESSIBLE, offsetof(COMMUNITY, community_name), 0 }, /* community_name */
    { INTEGER_TYPE, SR_NOT_ACCESSIBLE, offsetof(COMMUNITY, ip_addr), 1 },    /* ip_addr */
    { INTEGER_TYPE, SR_NOT_ACCESSIBLE, offsetof(COMMUNITY, privs), -1 },   /* commprivs */
    { INTEGER_TYPE, SR_NOT_ACCESSIBLE,  offsetof(COMMUNITY, id), -1 },   /* id */
    { -1, -1, (unsigned short) -1, -1 }
};

#ifdef SR_CONFIG_FP
const PARSER_CONVERTER communitiesConverters[] = {
    { ConvToken_textOctetString , NULL },
    { ConvToken_ipaddress , NULL },
    { ConvToken_commprivs , NULL },
    { ConvToken_integer, NULL },
    { NULL, NULL }
};
const PARSER_RECORD_TYPE communitiesRecord = {
    PARSER_TABLE,
    "community ",
    communitiesConverters,
    communitiesType,
    &communities,
    NULL,
    -1,
    -1,
    -1
#ifndef SR_NO_COMMENTS_IN_CONFIG
    , NULL
#endif	/* SR_NO_COMMENTS_IN_CONFIG */
};
#endif /* SR_CONFIG_FP */

Index_t trapCommunitiesIndex[] = {
    { offsetof(TRAP_COMMUNITY, community_name),  T_octet},
    { offsetof(TRAP_COMMUNITY, ip_addr),  T_uint}
};

SnmpV2Table trapCommunities = {
    NULL,
    trapCommunitiesIndex,
    0,
    2,
    sizeof(TRAP_COMMUNITY)
};

const SnmpType trapCommunitiesType[] = {
    { OCTET_PRIM_TYPE, SR_NOT_ACCESSIBLE, offsetof(TRAP_COMMUNITY, community_name), 0 }, /* community_name */
    { INTEGER_TYPE, SR_NOT_ACCESSIBLE, offsetof(TRAP_COMMUNITY, ip_addr), 1 },    /* ip_addr */
    { -1, -1, (unsigned short) -1, -1 }
};
#ifdef SR_CONFIG_FP
const PARSER_CONVERTER trapCommunitiesConverters[] = {
    {ConvToken_textOctetString, NULL},
    {ConvToken_ipaddress, NULL},
    {NULL, NULL}
};
const PARSER_RECORD_TYPE trapCommunitiesRecord = {
    PARSER_TABLE,
    "trap ",
    trapCommunitiesConverters,
    trapCommunitiesType,
    &trapCommunities,
    NULL,
    -1,
    -1,
    -1
#ifndef SR_NO_COMMENTS_IN_CONFIG
    , NULL
#endif	/* SR_NO_COMMENTS_IN_CONFIG */
};
#endif /* SR_CONFIG_FP */

extern ConfigFile *sfp;         /* declared in k_fileio.c */

int
init_communities()
{
    /* initialize index descriptor */
    if ((communities.tp = (void **) malloc (sizeof(COMMUNITY *))) == NULL) {
        return 0;
    }

    return 1;
}

int
init_trapCommunities()
{
    /* initialize index descriptor */
    if ((trapCommunities.tp = (void **) malloc (sizeof(TRAP_COMMUNITY *))) == NULL) {
        return 0;
    }

    return 1;
}

/* convert an unsigned long into a dotted decimal ip address */
char *
IPAddrToString(addr)
    unsigned long addr;
{
  static char printbuff[40];

  sprintf(printbuff,"%ld.%ld.%ld.%ld",
  ((addr >> 24) & 0x000000FF),
  ((addr >> 16) & 0x000000FF),
  ((addr >> 8) & 0x000000FF),
  (addr & (long)0x000000FF));

  return(printbuff);
}     /* IPAddrToString() */

SR_INT32
ConvToken_ipaddress(direction, token, value)
    SR_INT32 direction;
    char **token;
    void *value;
{
    SR_UINT32 *local = (SR_UINT32 *)value;
    switch (direction) {
        case PARSER_CONVERT_FROM_TOKEN:
            *local = (SR_UINT32)inet_addr(*token);
            if (*local == (SR_UINT32)-1) {
                return -1;
            }
            return 0;
        case PARSER_CONVERT_TO_TOKEN:
            *token = (char *)malloc(16);
            if (*token == NULL) {
                return -1;
            }
            sprintf(*token, INT32_FORMAT "." INT32_FORMAT "."
		    INT32_FORMAT "." INT32_FORMAT,
                    (ntohl(*local) & 0xff000000) >> 24,
                    (ntohl(*local) & 0xff0000) >> 16,
                    (ntohl(*local) & 0xff00) >> 8,
                    (ntohl(*local) & 0xff));
            return 0;
        default:
            return -1;
    }
}

SR_INT32
ConvToken_commprivs(direction, token, value)
    SR_INT32 direction;
    char **token;
    void *value;
{
    SR_INT32 *local = (SR_INT32 *)value;
    switch (direction) {
        case PARSER_CONVERT_FROM_TOKEN:
            if (strcmp(*token, "read") == 0) {
                *local = SR_READ_ONLY;
                return 0;
            }
            if (strcmp(*token, "READ") == 0) {
                *local = SR_READ_ONLY;
                return 0;
            }
            if (strcmp(*token, "read_write") == 0) {
                *local = SR_READ_WRITE;
                return 0;
            }
            if (strcmp(*token, "write") == 0) {
                *local = SR_READ_WRITE;
                return 0;
            }
            if (strcmp(*token, "WRITE") == 0) {
                *local = SR_READ_WRITE;
                return 0;
            }
            DPRINTF((APWARN, "Unknown commprivs string '%s'\n", *token));
            return -1;
        case PARSER_CONVERT_TO_TOKEN:
            *token = (char *)malloc(8);
            if (*token == NULL) {
                return -1;
            }
            if (*local == SR_READ_ONLY) {
               strcpy(*token, "read");
            } else if (*local == SR_READ_WRITE) {
               strcpy(*token, "write");
            }
            return 0;
        default:
            return -1;
    }
}

COMMUNITY *
LookupCOMMUNITY(comm)
    OctetString *comm;
{
    int index;
    communities.tip[0].value.octet_val = comm;
    index = SearchTable(&communities, EXACT);
    if (index == -1) {
        return NULL;
    }
    return (COMMUNITY *)communities.tp[index];
}

TRAP_COMMUNITY *
LookupTRAP_COMMUNITY(comm)
    OctetString *comm;
{
    int index;
    trapCommunities.tip[0].value.octet_val = comm;
    index = SearchTable(&trapCommunities, EXACT);
    if (index == -1) {
        return NULL;
    }
    return (TRAP_COMMUNITY *)trapCommunities.tp[index];
}
