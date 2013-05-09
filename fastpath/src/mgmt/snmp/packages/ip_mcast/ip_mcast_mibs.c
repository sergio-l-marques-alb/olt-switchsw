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
/* lvl7_@p0095 start */
              /* /src/l7public/por/public/ */
/* lvl7_@p0095 end */

#include <l7_common_l3.h>       
#include "snmp_api.h"

int
k_private_ip_mcast_initialize(void)
{
  SnmpSupportedMibTableEntryCreate("RFC 2932 - IPMROUTE-MIB",
                                   "IPv4 Multicast Routing MIB",
                                   "ipMRouteStdMIB");

  /*
  SnmpSupportedMibTableEntryCreate("RFC 2933 - IGMP-STD-MIB",
                                   "Internet Group Management Protocol MIB",
                                   "igmpStdMIB");
  */

  SnmpSupportedMibTableEntryCreate("draft-ietf-magma-mgmd-mib-03",
                                   "MGMD MIB, includes IGMPv3 and MLDv2.",
                                   "mgmdStdMIB");

  SnmpSupportedMibTableEntryCreate("RFC 5060 - PIM-STD-MIB",
                                   "Protocol Independent Multicast MIB",
                                   "pimStdMIB");

  SnmpSupportedMibTableEntryCreate("RFC 5240 - PIM-BSR-MIB",
                                   "Bootstrap Router mechanism for PIM routers",
                                   "pimBsrMIB");

  SnmpSupportedMibTableEntryCreate("DVMRP-STD-MIB",
                                   "Distance-Vector Multicast Routing Protocol MIB",
                                   "dvmrpStdMIB");

  SnmpSupportedMibTableEntryCreate("IANA-RTPROTO-MIB",
                                   "IANA IP Route Protocol and IP MRoute Protocol Textual Conventions",
                                   "ianaRtProtoMIB");

/*  SnmpSupportedMibTableEntryCreate("FASTPATH-MULTICAST-MIB",
                                   "FASTPATH Flex Multicast Routing Private MIB",
                                   "fastPathMulticast"); */
  return 1;
}
