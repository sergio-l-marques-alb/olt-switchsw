
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename   k_mib_fastpathdvlan_api.h
*
* @purpose    Wrapper functions for Fastpath DVLAN
*
* @component  SNMP
*
* @comments
*
* @create     3/6/2007
*
* @author     nshrivastav
* @end
*
**********************************************************************/

#ifndef __K_MIB_FASTPATHDVLAN_API_H__
#define __K_MIB_FASTPATHDVLAN_API_H__

L7_RC_t
snmpAgentSwitchDvlanTagEthertypeCurrentValueGet(L7_uint32 unit,
                                                L7_uint32 *etherType);
L7_RC_t
snmpAgentSwitchDvlanTagEthertypeGet(L7_uint32 UnitIndex, L7_uint32 val);
L7_RC_t
snmpAgentSwitchDvlanTagEthertypeNextGet(L7_uint32 UnitIndex, L7_uint32 val, L7_uint32 *val1);
L7_RC_t
snmpAgentSwitchDvlanTagIsPrimaryTPid(L7_uint32 UnitIndex, L7_uint32 val);
L7_RC_t
snmpAgentPortDVlanTagModeGet(L7_uint32 UnitIndex, L7_uint32 intIfIndex, L7_uint32 *val);
L7_RC_t
snmpAgentPortDVlanTagModeSet(L7_uint32 UnitIndex, L7_uint32 intIfIndex, L7_uint32 val);
L7_RC_t
snmpAgentSwitchPortDvlanTagEntryGet(L7_uint32 UnitIndex, L7_uint32 intIfIndex, L7_uint32 etherType);
L7_RC_t
snmpAgentSwitchPortDvlanTagNextEntryGet(L7_uint32 UnitIndex, L7_uint32 intIfIndex, 
                                                  L7_uint32 *nextIntfIndex, L7_uint32 etherType,
                                                  L7_uint32 *nextEtherType);
L7_RC_t
snmpAgentSwitchDvlanTagIntfEthertypeNextGet(L7_uint32 UnitIndex, L7_uint32 interface, 
                                                      L7_uint32 etherType, L7_uint32 *nextEtherType);

#endif /* __K_MIB_FASTPATHDVLAN_API_H__ */

