/*********************************************************************
*
* (C) Copyright Broadcom Corporation 1999-2007
*
**********************************************************************
*
* @filename   k_mib_fastpath_portsecurity_api.h
*
* @purpose    Wrapper functions for Fastpath Port Security MIB
*
* @component  SNMP
*
* @comments
*
* @create     06/09/2004
*
* @author     kmanish
* @end
*
**********************************************************************/

#ifndef K_MIB_FASTPATH_PORTSECURITY_API_H
#define K_MIB_FASTPATH_PORTSECURITY_API_H 

/* Begin Function Prototypes */

/****************************************************************************************/
L7_RC_t snmpAgentGlobalPortSecurityModeGet(L7_uint32 *val);

L7_RC_t snmpAgentGlobalPortSecurityModeSet(L7_uint32 val);

L7_RC_t snmpAgentPortSecurityEntryGet(L7_uint32 UnitIndex, L7_uint32 extIfNum);

L7_RC_t snmpAgentPortSecurityEntryGetNext(L7_uint32 UnitIndex, L7_uint32 *extIfNum);

L7_RC_t snmpAgentPortSecurityModeGet(L7_uint32 intfNum, L7_uint32 *val);

L7_RC_t snmpAgentPortSecurityModeSet(L7_uint32 intfNum, L7_uint32 val);

L7_RC_t snmpAgentPortSecurityViolationTrapModeGet(L7_uint32 intfNum, L7_uint32 *val);

L7_RC_t snmpAgentPortSecurityViolationTrapModeSet(L7_uint32 intfNum, L7_uint32 val);

L7_RC_t snmpAgentPortSecurityDynamicMACsGet( L7_uint32 intIfNum, L7_uchar8 *dynamicMACList);

L7_RC_t snmpAgentPortSecurityStaticMACsGet( L7_uint32 intIfNum, L7_uchar8 *staticMACList);

L7_RC_t snmpAgentPortSecurityLastDiscardedMACGet( L7_uint32 intIfNum, L7_uchar8 *discardedMAC);

L7_BOOL snmpPmlConvertMac(L7_uchar8 *buf, L7_enetMacAddr_t *macAddr);

L7_RC_t snmpAgentPortSecurityMACAddressAdd(L7_uint32 intIfNum, L7_uchar8* VidMACAddress);

L7_RC_t snmpAgentPortSecurityMACAddressRemove(L7_uint32 intIfNum, L7_uchar8* VidMACAddress);

L7_RC_t snmpAgentPortSecurityMACAddressMove(L7_uint32 intfNum, L7_uint32 val);

L7_RC_t snmpAgentPortSecurityDynamicEntryGet(L7_uint32 UnitIndex, L7_uint32 extIfNum,
                                             L7_uchar8* macAddress, L7_ushort16 vlanId);

L7_RC_t snmpAgentPortSecurityDynamicEntryGetNext(L7_uint32 UnitIndex, L7_uint32 *extIfNum,
                                             L7_uchar8* macAddress, L7_uint32 *vlanId);


/****************************************************************************************/

/* End Function Prototypes */

#endif /* K_MIB_FASTPATH_PORTSECURITY_API_H */
