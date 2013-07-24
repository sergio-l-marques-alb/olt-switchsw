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
 *                   
 ********************************************************************/

/* lvl7_@p0095 start */
              /* /src/l7public/por/public/ */
/* lvl7_@p0095 end */
#include <l7_common_l3.h>       

#include "k_private_base.h"

int
k_private_routing_initialize(void)
{
  SnmpSupportedMibTableEntryCreate("IANA-ADDRESS-FAMILY-NUMBERS-MIB",
                                   "The MIB module defines the AddressFamilyNumbers textual convention.",
                                   "ianaAddressFamilyNumbers");
#ifdef L7_RIP_PACKAGE
  SnmpSupportedMibTableEntryCreate("RFC 1724 - RIPv2-MIB",
                                   "RIP Version 2 MIB Extension",
                                   "rip2");
#endif

#ifdef L7_OSPF_PACKAGE
  SnmpSupportedMibTableEntryCreate("RFC 1850 - OSPF-MIB",
                                   "OSPF Version 2 Management Information Base",
                                   "ospf");
  
  SnmpSupportedMibTableEntryCreate("RFC 1850 - OSPF-TRAP-MIB",
                                   "The MIB module to describe traps for the OSPF Version 2 Protocol.",
                                   "ospfTrap");
#endif

#ifdef L7_VRRP_PACKAGE
  SnmpSupportedMibTableEntryCreate("RFC 2787 - VRRP-MIB",
                                   "Definitions of Managed Objects for the Virtual Router Redundancy Protocol",
                                   "vrrpMIB");
#endif
  SnmpSupportedMibTableEntryCreate("FASTPATH-ROUTING-MIB",
                                   "FASTPATH Routing - Layer 3",
                                   "fastPathRouting");
  return 1;
}

