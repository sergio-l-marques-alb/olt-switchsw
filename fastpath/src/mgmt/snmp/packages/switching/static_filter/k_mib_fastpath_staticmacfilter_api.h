/*********************************************************************
*
* (C) Copyright Broadcom Corporation 1999-2007
*
**********************************************************************
*
* @filename   k_mib_fastpathdvlan.c
*
* @purpose    System-Specific code to support Static Filtering component
*
* @component  SNMP
*
* @comments
*
* @create     4/6/2008
*
* @author     akulkarni
* @end
*
**********************************************************************/


#ifndef __K_MIB_STATIC_MAC_FILTER__
#define __K_MIB_STATIC_MAC_FILTER__

L7_RC_t
snmpAgentSwitchStaticMacFilteringEntryCreate(L7_uint32 UnitIndex, L7_char8 *macAddr, L7_uint32 vlanId);


#endif /* __K_MIB_STATIC_MAC_FILTER__*/
