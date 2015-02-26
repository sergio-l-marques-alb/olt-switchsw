/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* Name: k_private_utils.c
*
* Purpose: Util functions shared across all agents.
*
* Created by: Colin Verne
*
* Component: SNMP
*
*********************************************************************/
/*********************************************************************
                         FASTPATHNP Release 2.0
**********************************************************************
*********************************************************************/

#include "l7_common.h"          /* /src/system/public/ */
              /* /src/l7public/por/public/ */

#include "k_private_base.h"

int
k_private_switching_initialize(void)
{
  SnmpSupportedMibTableEntryCreate("LAG-MIB",
                                   "The Link Aggregation module for managing IEEE 802.3ad",
                                   "lagMIB");

  SnmpSupportedMibTableEntryCreate("RFC 1213 - RFC1213-MIB",
                                   "Management Information Base for Network Management of TCP/IP-based internets: MIB-II",
                                   "mgmt.1");

  SnmpSupportedMibTableEntryCreate("RFC 1493 - BRIDGE-MIB",
                                   "Definitions of Managed Objects for Bridges (dot1d)",
                                   "dot1dBridge");
  
  SnmpSupportedMibTableEntryCreate("RFC 2674 - P-BRIDGE-MIB",
                                   "The Bridge MIB Extension module for managing Priority and Multicast Filtering, defined by IEEE 802.1D-1998.",
                                   "pBridgeMIB");
  
  SnmpSupportedMibTableEntryCreate("RFC 2674 - Q-BRIDGE-MIB",
                                   "The VLAN Bridge MIB module for managing Virtual Bridged Local Area Networks",
                                   "qBridgeMIB");

  SnmpSupportedMibTableEntryCreate("RFC 2737 - ENTITY-MIB",
                                   "Entity MIB (Version 2)",
                                   "entityMIB");

  SnmpSupportedMibTableEntryCreate("RFC 2863 - IF-MIB",
                                   "The Interfaces Group MIB using SMIv2",
                                   "ifMIB");

  SnmpSupportedMibTableEntryCreate("RFC 3635 - Etherlike-MIB",
                                   "Definitions of Managed Objects for the Ethernet-like Interface Types",
                                   "etherMIB");
  
  SnmpSupportedMibTableEntryCreate("OLTSWITCH-SWITCHING-MIB",
                                   "OLTSWITCH Switching - Layer 2",
                                   "OLTSWITCHSwitching");
/* Chassis package will not have inventory mib */
#ifndef L7_CHASSIS 
  SnmpSupportedMibTableEntryCreate("OLTSWITCH-INVENTORY-MIB",
                                   "Unit and Slot configuration.",
                                   "OLTSWITCHInventory");
#endif
  SnmpSupportedMibTableEntryCreate("OLTSWITCH-PORTSECURITY-PRIVATE-MIB",
                                   "Port Security MIB.",
                                   "OLTSWITCHPortSecurity");
  return 1;
}
