/*********************************************************************
*
* (C) Copyright Broadcom Corporation 1999-2007
*
**********************************************************************
*
* @filename   k_mib_fastpath_denialofservice_api.h
*
* @purpose    Wrapper functions for Fastpath Denial of Service MIB
*
* @component  SNMP
*
* @comments
*
* @create     09/1/2005
*
* @author     esmiley
* @end
*
**********************************************************************/

#ifndef K_MIB_FASTPATH_DENIALOFSERVICE_API_H
#define K_MIB_FASTPATH_DENIALOFSERVICE_API_H 

/* Begin Function Prototypes */

L7_RC_t snmpAgentSwitchDenialOfServiceSIPDIPModeGet ( L7_uint32 UnitIndex, L7_int32 *val );

L7_RC_t snmpAgentSwitchDenialOfServiceSIPDIPModeSet  ( L7_uint32 UnitIndex, L7_int32 val );

L7_RC_t snmpAgentSwitchDenialOfServiceSMACDMACModeGet ( L7_uint32 UnitIndex, L7_int32 *val );

L7_RC_t snmpAgentSwitchDenialOfServiceSMACDMACModeSet  ( L7_uint32 UnitIndex, L7_int32 val );

L7_RC_t snmpAgentSwitchDenialOfServiceFirstFragModeGet ( L7_uint32 UnitIndex, L7_int32 *val );

L7_RC_t snmpAgentSwitchDenialOfServiceFirstFragModeSet  ( L7_uint32 UnitIndex, L7_int32 val );

L7_RC_t snmpAgentSwitchDenialOfServiceTCPHdrSizeGet ( L7_uint32 UnitIndex, L7_int32 *val );

L7_RC_t snmpAgentSwitchDenialOfServiceTCPHdrSizeSet  ( L7_uint32 UnitIndex, L7_int32 val );

L7_RC_t snmpAgentSwitchDenialOfServiceTCPFragModeGet ( L7_uint32 UnitIndex, L7_int32 *val );

L7_RC_t snmpAgentSwitchDenialOfServiceTCPFragModeSet  ( L7_uint32 UnitIndex, L7_int32 val );

L7_RC_t snmpAgentSwitchDenialOfServiceTCPFlagModeGet ( L7_uint32 UnitIndex, L7_int32 *val );

L7_RC_t snmpAgentSwitchDenialOfServiceTCPFlagModeSet  ( L7_uint32 UnitIndex, L7_int32 val );

L7_RC_t snmpAgentSwitchDenialOfServiceTCPFlagSeqModeGet ( L7_uint32 UnitIndex, L7_int32 *val );

L7_RC_t snmpAgentSwitchDenialOfServiceTCPFlagSeqModeSet  ( L7_uint32 UnitIndex, L7_int32 val );

L7_RC_t snmpAgentSwitchDenialOfServiceTCPOffsetModeGet ( L7_uint32 UnitIndex, L7_int32 *val );

L7_RC_t snmpAgentSwitchDenialOfServiceTCPOffsetModeSet  ( L7_uint32 UnitIndex, L7_int32 val );

L7_RC_t snmpAgentSwitchDenialOfServiceTCPSynModeGet ( L7_uint32 UnitIndex, L7_int32 *val );

L7_RC_t snmpAgentSwitchDenialOfServiceTCPSynModeSet  ( L7_uint32 UnitIndex, L7_int32 val );

L7_RC_t snmpAgentSwitchDenialOfServiceTCPSynFinModeGet ( L7_uint32 UnitIndex, L7_int32 *val );

L7_RC_t snmpAgentSwitchDenialOfServiceTCPSynFinModeSet  ( L7_uint32 UnitIndex, L7_int32 val );

L7_RC_t snmpAgentSwitchDenialOfServiceTCPFinUrgPshModeGet ( L7_uint32 UnitIndex, L7_int32 *val );

L7_RC_t snmpAgentSwitchDenialOfServiceTCPFinUrgPshModeSet  ( L7_uint32 UnitIndex, L7_int32 val );

L7_RC_t snmpAgentSwitchDenialOfServiceL4PortModeGet ( L7_uint32 UnitIndex, L7_int32 *val );
                                                                                          
L7_RC_t snmpAgentSwitchDenialOfServiceL4PortModeSet  ( L7_uint32 UnitIndex, L7_int32 val );

L7_RC_t snmpAgentSwitchDenialOfServiceUDPPortModeGet ( L7_uint32 UnitIndex, L7_int32 *val );

L7_RC_t snmpAgentSwitchDenialOfServiceTCPPortModeGet ( L7_uint32 UnitIndex, L7_int32 *val );
                                                                                          
L7_RC_t snmpAgentSwitchDenialOfServiceTCPPortModeSet  ( L7_uint32 UnitIndex, L7_int32 val );

L7_RC_t snmpAgentSwitchDenialOfServiceUDPortModeGet ( L7_uint32 UnitIndex, L7_int32 *val );
                                                                                          
L7_RC_t snmpAgentSwitchDenialOfServiceUDPPortModeSet  ( L7_uint32 UnitIndex, L7_int32 val );

L7_RC_t snmpAgentSwitchDenialOfServiceICMPModeGet ( L7_uint32 UnitIndex, L7_int32 *val );

L7_RC_t snmpAgentSwitchDenialOfServiceICMPModeSet  ( L7_uint32 UnitIndex, L7_int32 val );

L7_RC_t snmpAgentSwitchDenialOfServiceICMPSizeGet ( L7_uint32 UnitIndex, L7_int32 *val );

L7_RC_t snmpAgentSwitchDenialOfServiceICMPSizeSet  ( L7_uint32 UnitIndex, L7_int32 val );

L7_RC_t snmpAgentSwitchDenialOfServiceICMPv6SizeGet ( L7_uint32 UnitIndex, L7_int32 *val );

L7_RC_t snmpAgentSwitchDenialOfServiceICMPv6SizeSet  ( L7_uint32 UnitIndex, L7_int32 val );

L7_RC_t snmpAgentSwitchDenialOfServiceICMPFragModeGet ( L7_uint32 UnitIndex, L7_int32 *val );

L7_RC_t snmpAgentSwitchDenialOfServiceICMPFragModeSet  ( L7_uint32 UnitIndex, L7_int32 val );


/****************************************************************************************/

/* End Function Prototypes */

#endif /* K_MIB_FASTPATH_DENIALOFSERVICE_API_H */
