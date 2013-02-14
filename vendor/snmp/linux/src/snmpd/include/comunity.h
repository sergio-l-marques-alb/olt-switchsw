/*
 *
 * Copyright (C) 1992-2002 by SNMP Research, Incorporated.
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

#ifndef SR_COMUNITY_H
#define SR_COMUNITY_H

#ifdef  __cplusplus
extern "C" {
#endif

#define MAXCOMMLEN 64 

#define NUM_TRAP_COMMUNITIES    25
#define NUM_COMMUNITIES         25

typedef struct _COMMUNITY {
    OctetString    *community_name;
    SR_UINT32       ip_addr;	/* In Network Byte Order */
    SR_INT32        privs;
    SR_INT32        id;
}               COMMUNITY;

typedef struct _TRAP_COMMUNITY {
    OctetString    *community_name;
    SR_UINT32       ip_addr;	/* In Network Byte Order */
}               TRAP_COMMUNITY;

COMMUNITY *LookupCOMMUNITY
    (OctetString *comm);

TRAP_COMMUNITY *LookupTRAP_COMMUNITY
    (OctetString *comm);

SR_INT32 ConvToken_ipaddress
    (SR_INT32 direction, char **token, void *value);

SR_INT32 ConvToken_commprivs
    (SR_INT32 direction, char **token, void *value);

char *IPAddrToString(unsigned long addr);
int init_communities(void);
int init_trapCommunities(void);

#ifdef  __cplusplus
}
#endif

#endif				/* SR_COMUNITY_H */
