/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* Name: security_mibs.c
*
* Purpose: Inserts Mgmt Security MIB information into Supported MIB table.
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
k_private_mgmt_security_initialize(void)
{
  SnmpSupportedMibTableEntryCreate("FASTPATH-MGMT-SECURITY-MIB",
                                   "The Broadcom Private MIB for FastPath Mgmt Security",
                                   "fastPathMgmtSecurity");

  return 1;
}
