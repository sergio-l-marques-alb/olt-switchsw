
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename   k_mib_fastpathdnsclient_control_api.h
*
* @purpose    Wrapper functions for Fastpath DNS Client Control MIB
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

#ifndef __K_MIB_FASTPATHDNSCLIENT_CONTROL_API_H__
#define __K_MIB_FASTPATHDNSCLIENT_CONTROL_API_H__

/******************************************************************************/

L7_RC_t
snmpDnsResCtlglobalAdminModeGet(L7_int32 *adminmode);

L7_RC_t
snmpDnsResCtlglobalAdminModeSet(L7_int32 adminmode); 
L7_RC_t
snmpDNSClientStaticHostEntryNextGet( L7_char8  *hostname,
                                     L7_inet_addr_t *inetAddr);
/******************************************************************************/
#endif /* __K_MIB_FASTPATHDNSCLIENT_COTNROL_API_H__ */

