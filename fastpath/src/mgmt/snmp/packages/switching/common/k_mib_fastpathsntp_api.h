
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename   k_mib_fastpathsntp_api.h
*
* @purpose    Wrapper functions for Fastpath SNTP MIB
*
* @component  SNMP
*
* @comments
*
* @create     2/24/2004
*
* @author     cpverne
* @end
*
**********************************************************************/

#ifndef __K_MIB_FASTPATHSNTP_API_H__
#define __K_MIB_FASTPATHSNTP_API_H__

/****************************************************************************************/

L7_RC_t
snmpAgentSntpClientVersionGet( L7_uint32 *val );

L7_RC_t
snmpAgentSntpClientSupportedModeGet( L7_char8 *val );

L7_RC_t
snmpAgentSntpClientModeGet( L7_uint32 *val );

L7_RC_t
snmpAgentSntpClientModeTest( L7_uint32 val );

L7_RC_t
snmpAgentSntpClientModeSet( L7_uint32 val );

L7_RC_t
snmpAgentSntpClientLastUpdateTimeGet( L7_uint32 *val );

L7_RC_t
snmpAgentSntpClientLastAttemptTimeGet( L7_uint32 *val );

L7_RC_t
snmpAgentSntpClientLastAttemptStatusGet( L7_uint32 *val );

L7_RC_t
snmpAgentSntpClientServerAddressTypeGet( L7_uint32 *val );

L7_RC_t
snmpAgentSntpClientServerModeGet( L7_uint32 *val );

/****************************************************************************************/

L7_RC_t
snmpAgentSntpClientUcastServerEntryGet( L7_uint32 agentSntpClientUcastServerIndex);

L7_RC_t
snmpAgentSntpClientUcastServerEntryNextGet( L7_uint32 *agentSntpClientUcastServerIndex);

L7_RC_t
snmpAgentSntpClientUcastServerAddressTypeGet( L7_uint32 agentSntpClientUcastServerIndex, L7_uint32 *val );

L7_RC_t
snmpAgentSntpClientUcastServerAddressTypeSet( L7_uint32 agentSntpClientUcastServerIndex, L7_uint32 val );

L7_RC_t
snmpAgentSntpClientUcastServerVersionGet( L7_uint32 agentSntpClientUcastServerIndex, L7_uint32 *val );

L7_RC_t
snmpAgentSntpClientUcastServerVersionSet( L7_uint32 agentSntpClientUcastServerIndex, L7_uint32 val );

L7_RC_t
snmpAgentSntpClientUcastServerLastAttemptStatusGet( L7_uint32 agentSntpClientUcastServerIndex, L7_uint32 *val );

L7_RC_t
snmpAgentSntpClientUcastServerRowStatusGet( L7_uint32 agentSntpClientUcastServerIndex, L7_uint32 *val );

L7_RC_t
snmpAgentSntpClientUcastServerRowStatusSet( L7_uint32 agentSntpClientUcastServerIndex, L7_uint32 val );

/****************************************************************************************/

#endif /* __K_MIB_FASTPATHSNTP_API_H__ */

