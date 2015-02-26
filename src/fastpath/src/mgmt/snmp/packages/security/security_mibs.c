/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* Name: security_mibs.c
*
* Purpose: Inserts Security MIB information into Supported MIB table.
*
* Created by: Colin Verne
*
* Component: SNMP
*
*********************************************************************/

#include "l7_common.h"          /* /src/system/public/ */
              /* /src/l7public/por/public/ */

#include "k_private_base.h"

int
k_private_security_initialize(void)
{
  SnmpSupportedMibTableEntryCreate("IEEE8021-PAE-MIB",
                                   "Port Access Entity module for managing IEEE 802.1X.",
                                   "paeMIBObjects");

  SnmpSupportedMibTableEntryCreate("OLTSWITCH-RADIUS-AUTH-CLIENT-MIB",
                                   "Broadcom OLTSWITCH Radius MIB",
                                   "OLTSWITCHRadius");

  SnmpSupportedMibTableEntryCreate("RADIUS-ACC-CLIENT-MIB",
                                   "RADIUS Accounting Client MIB",
                                   "radiusAccClientMIB");

  SnmpSupportedMibTableEntryCreate("RADIUS-AUTH-CLIENT-MIB",
                                   "RADIUS Authentication Client MIB",
                                   "radiusAuthClientMIB");

  SnmpSupportedMibTableEntryCreate("OLTSWITCH-CAPTIVE-PORTAL-MIB",
                                   "OLTSWITCH Captive Portal MIB",
                                   "OLTSWITCHCaptivePortal");

  return 1;
}

