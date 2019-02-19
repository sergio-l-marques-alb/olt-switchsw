/*********************************************************************
*
* (C) Copyright Broadcom Corporation 1999-2007
*
**********************************************************************
*
* @filename   k_mib_fastpath_staticmacfilter_api.c
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

#include "l7_common.h"
#include "usmdb_dot1q_api.h"
#include"usmdb_dot1q_api.h"
#include "usmdb_filter_api.h"

L7_RC_t
snmpAgentSwitchStaticMacFilteringEntryCreate(L7_uint32 UnitIndex, L7_char8 *macAddr, L7_uint32 vlanId)
{
  if (usmDbVlanIDGet(UnitIndex, vlanId) == L7_SUCCESS &&
      usmDbFilterIsRestrictedFilterMac(macAddr) == L7_FALSE &&
      usmDbFilterCreate(UnitIndex, macAddr, vlanId) == L7_SUCCESS)
    return L7_SUCCESS;

  return L7_FAILURE;
}

