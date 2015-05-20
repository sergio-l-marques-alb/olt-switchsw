/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* Name: base_mibs.c
*
* Purpose: Inserts MIB information into Supported MIB table.
*
* Created by: Colin Verne
*
* Component: SNMP
*
*********************************************************************/

#include <l7_common.h>          /* /src/system/public/ */
              /* /src/l7public/por/public/ */

#include <k_private_base.h>

#ifdef SNMP_SWITCHING
extern int k_private_switching_initialize(void);
#endif /* SNMP_SWITCHING */

#ifdef SNMP_SECURITY
extern int k_private_security_initialize(void);
#endif /* SNMP_SECURITY */

#ifdef SNMP_MGMT_SECURITY
extern int k_private_mgmt_security_initialize(void);
#endif /* SNMP_MGMT_SECURITY */

#ifdef SNMP_ROUTING
extern int k_private_routing_initialize(void);
#endif /* SNMP_ROUTING */

#ifdef SNMP_BGP
extern int k_private_bgp_initialize(void);
#endif /* SNMP_BGP */

#ifdef SNMP_QOS
extern int k_private_qos_initialize(void);
#endif /* SNMP_QOS */

#ifdef SNMP_IP_MCAST
extern int k_private_ip_mcast_initialize(void);
#endif /* SNMP_IP_MCAST */

#ifdef SNMP_STACKING
extern int k_private_stacking_initialize(void);
#endif /* SNMP_STACKING */

#ifdef SNMP_IPV6
extern int k_private_ipv6_initialize(void);
#endif /* SNMP_IPV6 */

#ifdef SNMP_WIRELESS
extern int k_private_wireless_initialize(void);
#endif /* SNMP_WIRELESS */

#ifdef SNMP_METRO
extern int k_private_metro_initialize();
#endif /* SNMP_METRO */

#ifdef L7_CHASSIS
extern int k_private_chassis_initialize(void);
#endif /* L7_CHASSIS */



int
k_private_initialize(void)
{
  
  static L7_BOOL firstTime = L7_TRUE;

  if (firstTime == L7_FALSE)
    return 1;

  firstTime = L7_FALSE;
    
  SnmpSupportedMibTableEntryCreate("RFC 1907 - SNMPv2-MIB",
                                   "The MIB module for SNMPv2 entities",
                                   "snmpMIB");

  SnmpSupportedMibTableEntryCreate("RFC 2819 - RMON-MIB",
                                   "Remote Network Monitoring Management Information Base",
                                   "rmonMibModule");

  SnmpSupportedMibTableEntryCreate("Broadcom-REF-MIB",
                                   "Broadcom Reference",
                                   "broadcom");

  SnmpSupportedMibTableEntryCreate("SNMP-COMMUNITY-MIB",
                                   "This MIB module defines objects to help support coexistence between SNMPv1, SNMPv2, and SNMPv3.",
                                   "snmpCommunityMIB");
  
  SnmpSupportedMibTableEntryCreate("SNMP-FRAMEWORK-MIB",
                                   "The SNMP Management Architecture MIB",
                                   "snmpFrameworkMIB");

  SnmpSupportedMibTableEntryCreate("SNMP-MPD-MIB",
                                   "The MIB for Message Processing and Dispatching",
                                   "snmpMPDMIB");

  SnmpSupportedMibTableEntryCreate("SNMP-NOTIFICATION-MIB",
                                   "The Notification MIB Module",
                                   "snmpTargetMIB");

  SnmpSupportedMibTableEntryCreate("SNMP-TARGET-MIB",
                                   "The Target MIB Module",
                                   "snmpTargetMIB");

  SnmpSupportedMibTableEntryCreate("SNMP-USER-BASED-SM-MIB",
                                   "The management information definitions for the SNMP User-based Security Model.",
                                   "snmpUsmMIB");

  SnmpSupportedMibTableEntryCreate("SNMP-VIEW-BASED-ACM-MIB",
                                   "The management information definitions for the View-based Access Control Model for SNMP.",
                                   "snmpVacmMIB");

  SnmpSupportedMibTableEntryCreate("USM-TARGET-TAG-MIB",
                                   "SNMP Research, Inc.",
                                   "usmTargetTagMIB");

  SnmpSupportedMibTableEntryCreate("OLTSWITCH-POWER-ETHERNET-MIB",
                                   "OLTSWITCH Power Ethernet Extensions MIB",
                                   "OLTSWITCHpowerEthernetMIB");

  SnmpSupportedMibTableEntryCreate("POWER-ETHERNET-MIB",
                                   "Power Ethernet MIB",
                                   "powerEthernetMIB");
  
  SnmpSupportedMibTableEntryCreate("SFLOW-MIB",
                                   "sFlow MIB",
                                   "sFlowMIB");
  
#ifdef  L7_ISDP_PACKAGE
  SnmpSupportedMibTableEntryCreate("OLTSWITCH-ISDP-MIB",
                                   "Industry Standard Discovery Protocol MIB",
                                   "OLTSWITCHIsdp");
#endif

  #ifdef SNMP_SWITCHING
  (void)k_private_switching_initialize();
  #endif /* SNMP_SWITCHING */
  
  #ifdef SNMP_SECURITY
  (void)k_private_security_initialize();
  #endif /* SNMP_SECURITY */

  #ifdef SNMP_MGMT_SECURITY
  (void)k_private_mgmt_security_initialize();
  #endif /* SNMP_MGMT_SECURITY */

  #ifdef SNMP_ROUTING
  (void)k_private_routing_initialize();
  #endif /* SNMP_ROUTING */

  #ifdef SNMP_BGP
  (void)k_private_bgp_initialize();
  #endif /* SNMP_BGP */

  #ifdef SNMP_QOS
  (void)k_private_qos_initialize();
  #endif /* SNMP_QOS */
  
  #ifdef SNMP_IP_MCAST
  (void)k_private_ip_mcast_initialize();
  #endif /* SNMP_IP_MCAST */
  
  #ifdef SNMP_STACKING
  k_private_stacking_initialize();
  #endif /* SNMP_STACKING */

  #ifdef SNMP_IPV6
  k_private_ipv6_initialize();
  #endif /* SNMP_IPV6 */
  
  #ifdef SNMP_WIRELESS
  k_private_wireless_initialize(); 
  #endif /* SNMP_WIRELESS */

  #ifdef SNMP_METRO
  k_private_metro_initialize();
  #endif /* SNMP_METRO */


#ifdef L7_CHASSIS
 k_private_chassis_initialize();
#endif /* L7_CHASSIS */
  
  return 1;
}

int
k_private_terminate(void)
{
  return 1;
}
