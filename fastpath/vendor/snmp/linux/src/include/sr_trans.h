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

#ifndef SR_SR_TRANS_H
#define SR_SR_TRANS_H

#ifdef  __cplusplus
extern "C" {
#endif

extern const OID      *rfc1157DomainOID;
extern const OID      *snmpUDPDomainOID;
extern const OID      *snmpTCPDomainOID;

#define SR_UNDEFINED_TRANSPORT  0

typedef struct transportInfo TransportInfo;

#define DEFAULT_IPV4 0
#define IPV4_ONLY    1

#define BIND_DEFAULT 0
#define BIND_IP_ALL  1

#define TSTR_SIZE 64
#define IFREQ_MAX 100
#define IF_STR_MAX 64

#define	SR_UDP_TRANSPORT	1
#define	SR_IP_TRANSPORT		SR_UDP_TRANSPORT
#define	SR_TCP_TRANSPORT	6
/* LVL7 */
#define	SR_IP6_TRANSPORT	2
/* LVL7 */


typedef struct ipTransportInfo IpTransportInfo;

struct ipTransportInfo {
    SR_UINT32 addr;       /* In network byte order */
    unsigned short port;  /* In network byte order */
};

/* LVL7 */
typedef struct ip6TransportInfo Ip6TransportInfo;

struct ip6TransportInfo {
    unsigned char  addr[16];       /* In network byte order */
    unsigned short port;  /* In network byte order */
};
/* LVL7 */




/*
 * This structure is used to contain generalized transport information.
 * The type member indicates the type of transport represented, and the
 * union contains the specific transport information.
 */
struct transportInfo {
    SR_INT32 type;			/* type of transport */
    SR_INT32 flags;                     /* OS specific flags */
    SR_INT32 fd;
    union {
        IpTransportInfo ip;
/* LVL7 */
        Ip6TransportInfo ip6;
/* LVL7 */
        unsigned char foo1[8];
        unsigned char foo3[30];
        unsigned char foo2[12];
        unsigned char foo4[38];      /* place holder for future purpose */
    } ti;
};

/* Access Macros */
#define	t_ipAddr	ti.ip.addr
#define	t_ipPort	ti.ip.port
/* LVL7 */
#define	t_ip6Addr	ti.ip6.addr
#define	t_ip6Port	ti.ip6.port
/* LVL7 */

/*
 * struct to hold multiple IP addr info
 * support both IPv4 and IPv6
 */
typedef struct _transport {
    TransportInfo tInfo;
    struct _transport *next;
    int scope_id; /* for IPv6 */
    int flowInfo; /* for IPv6 */
}TRANS_INFO;


void CopyTransportInfo(TransportInfo *dst, const TransportInfo *src);

void CopyTransportAddr(TransportInfo *dst, const TransportInfo *src);

int CmpTransportInfo(const TransportInfo *ti1, const TransportInfo *ti2);

int CmpTransportInfoWithMask(
    const TransportInfo *ti1,
    const TransportInfo *ti2,
    const TransportInfo *mask);
 
int TransportInfoToOIDAndOctetString(
    const TransportInfo *ti,
    OID **domain,
    OctetString **address);

int OIDAndOctetStringToTransportInfo(
    TransportInfo *ti,
    OID *domain,
    const OctetString *address);

int TMaskOIDAndOctetStringToTransportInfo(
    TransportInfo *ti,
    OID *domain,
    const OctetString *address);

int SendToTransport(const char *data, int len, const TransportInfo *sti, const TransportInfo *dti);

int ReceiveFromTransport(char *data, int len, TransportInfo *sti, TransportInfo *dti);

char *FormatTransportString(char *buf, int len, const TransportInfo *ti);

int OpenTransport(TransportInfo *ti);

int BindTransport(TransportInfo *ti, int tries);

int BindTransportGivenPortRange(TransportInfo *ti);

void CloseTransport(TransportInfo *ti);

int GetTransportInfo(int bind_type, void **tInfo,
		       char *ipv4_bind_addr, char *ipv6_bind_addr);

void FreeTransinfoList(TRANS_INFO *ti_list);
TRANS_INFO *CloneTransinfoList(TRANS_INFO *ti_list);




#ifdef  __cplusplus
}
#endif

#endif	/* SR_SR_TRANS_H */
