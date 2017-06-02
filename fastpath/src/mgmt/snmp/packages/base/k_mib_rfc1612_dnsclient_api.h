
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename   k_mib_rfc1612_dnsclient_api.h
*
* @purpose    Wrapper functions for Fastpath DNS Client (rfc 1612) MIB
*
* @component  SNMP
*
* @comments
*
* @create     3/29/2005
*
* @author     I. Kiran
* @end
*
**********************************************************************/

#ifndef __K_MIB_RFC1612_DNSCLIENT_API_H__
#define __K_MIB_RFC1612_DNSCLIENT_API_H__
#include "usmdb_dns_client_api.h"
/******************************************************************************/

L7_RC_t
snmpdnsResConfigResetGet(L7_int32 *val);

L7_RC_t
snmpdnsResConfigImplementIdentGet(L7_char8 *val);

L7_RC_t
snmpDNSClientCacheRRNextGet(L7_uchar8    *rrName,
                            dnsRRType_t  *rrType,
                            L7_uint32    *rrIndex,
                            dnsRRClass_t *rrClass,
                            L7_uchar8    *qname);
/******************************************************************************/
#endif /* __K_MIB_RFC1612_DNSCLIENT_API_H__ */

