/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
* @filename k_mib_fastpathrouting_api.h
*
* @purpose SNMP specific value conversion
*
* @component SNMP
*
* @create 04/26/2001
*
* @author soma, cpverne
*
* @end
*             
**********************************************************************/
/*********************************************************************
 *                   
 ********************************************************************/

#include <l7_common.h>
#include <l3_comm_structs.h>

#include <usmdb_l3.h>
#include <defaultconfig.h>
#include <sysapi.h>

#ifdef L7_IPV6_PACKAGE
#include "usmdb_ip6_api.h"
#endif

#include "defaultconfig.h"
#include "sysapi.h"
#include "usmdb_1213_api.h"
#include "usmdb_ip_api.h"
#include "usmdb_iputil_api.h"
#include "usmdb_mib_relay_api.h"
#include "usmdb_mib_vlan_api.h"
#include "usmdb_rtrdisc_api.h"
#include "usmdb_util_api.h"
#include "usmdb_dhcp_client.h"
#include "usmdb_sim_api.h"

/**************************************************************************************************************/

L7_RC_t
snmpagentSwitchIntfArpEntryGetCheck(L7_uint32 UnitIndex, L7_uint32 ipAddress, L7_uint32 ifIndex)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_arpEntry_t pArp;
  L7_uint32 intIfNum;

  if (ifIndex != L7_INVALID_INTF)
  {
  rc = usmDbIntIfNumFromExtIfNum(ifIndex, &intIfNum);
  }
  else
  {
    intIfNum = L7_INVALID_INTF;
  }
  if (rc == L7_SUCCESS)
    rc = usmDbIpArpEntryGet(UnitIndex, ipAddress, intIfNum, &pArp);

  return rc;
}
    
L7_RC_t
snmpagentSwitchIntfArpEntryGetNextCheck(L7_uint32 UnitIndex, L7_uint32 *ipAddress, L7_uint32 *ifIndex)
{
  L7_RC_t rc;
  L7_arpEntry_t pArp;
  L7_uint32 intIfNum;

  rc = usmDbIntIfNumFromExtIfNum(*ifIndex, &intIfNum);

  if (rc != L7_SUCCESS)
  {
    /* find the next valid internal interface by first finding the next valid external interface */
    if (usmDbGetNextVisibleExtIfNumber(*ifIndex, ifIndex) == L7_SUCCESS &&
        usmDbIntIfNumFromExtIfNum(*ifIndex, &intIfNum) == L7_SUCCESS)
      rc = L7_SUCCESS;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbIpArpEntryNext(UnitIndex, *ipAddress, intIfNum, &pArp);

    if (rc == L7_SUCCESS)
    {
      *ipAddress = pArp.ipAddr;
      intIfNum = pArp.intIfNum;
      rc = usmDbExtIfNumFromIntIfNum(intIfNum, ifIndex);
    }
  }

  return rc;
}

L7_RC_t
snmpagentSwitchIntfArpMacAddressGet(L7_uint32 UnitIndex, L7_uint32 ipAddress, L7_uint32 ifIndex, L7_uchar8 *buf)
{
  L7_RC_t rc;
  L7_arpEntry_t pArp;
  L7_uint32 intIfNum;

  rc = usmDbIntIfNumFromExtIfNum(ifIndex, &intIfNum);

  if (rc == L7_SUCCESS)
    rc = usmDbIpArpEntryGet(UnitIndex, ipAddress, intIfNum, &pArp);

  if (rc == L7_SUCCESS)
  {
    if (pArp.macAddr.type == 1)
    {
      memcpy(buf, pArp.macAddr.addr.enetAddr.addr, pArp.macAddr.len);
    }
    else
    {
      rc = L7_FAILURE;
    }
  }

  return rc;
}
         

L7_RC_t
snmpagentSwitchIntfArpAgeGet(L7_uint32 UnitIndex, L7_uint32 ipAddress, L7_uint32 ifIndex, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_arpEntry_t pArp;
  L7_uint32 intIfNum;

  rc = usmDbIntIfNumFromExtIfNum(ifIndex, &intIfNum);

  if (rc == L7_SUCCESS)
    rc = usmDbIpArpEntryGet(UnitIndex, ipAddress, intIfNum, &pArp);

  if (rc == L7_SUCCESS)
    *val = pArp.ageSecs;

  return rc;
}





L7_RC_t
snmpagentSwitchIntfArpTypeGet(L7_uint32 UnitIndex, L7_uint32 ipAddress, L7_uint32 ifIndex, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_arpEntry_t pArp;

  L7_uint32 intIfNum;

  rc = usmDbIntIfNumFromExtIfNum(ifIndex, &intIfNum);

  if (rc == L7_SUCCESS)
    rc = usmDbIpArpEntryGet(UnitIndex, ipAddress, intIfNum, &pArp);

  if (rc == L7_SUCCESS)
  {
    if ((pArp.flags & L7_ARP_LOCAL) != 0)
    {
      *val = D_agentSwitchIntfArpType_local;
    }
    else if ((pArp.flags & L7_ARP_GATEWAY) != 0)
    {
      *val = D_agentSwitchIntfArpType_gateway;
    }
    else if ((pArp.flags & L7_ARP_STATIC) != 0)
    {
      *val = D_agentSwitchIntfArpType_static;
    }
    else
    {
      *val = D_agentSwitchIntfArpType_dynamic;
    }
  }

  return rc;
}

L7_RC_t
snmpagentSwitchIntfArpStatusSet(L7_uint32 UnitIndex, L7_uint32 ipAddress, L7_uint32 ifIndex, L7_uint32 val)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 intIfNum = ifIndex;
  switch (val)
  {
  case D_agentSwitchIntfArpStatus_active:
    rc = L7_SUCCESS;
    break;
  case D_agentSwitchIntfArpStatus_destroy:
    if (usmDbIpMapStaticArpDelete(UnitIndex, ipAddress, intIfNum) == L7_SUCCESS)
    {
      rc = L7_SUCCESS;
    }
    break;
  }

  return rc;
}


/**************************************************************************************************************/

L7_RC_t
snmpAgentSwitchArpAgeoutTimeGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  return usmDbIpArpAgeTimeGet(UnitIndex, val);
}

L7_RC_t
snmpAgentSwitchArpAgeoutTimeSet(L7_uint32 UnitIndex, L7_uint32 val)
{
  return usmDbIpArpAgeTimeSet(UnitIndex, val);
}

L7_RC_t
snmpAgentSwitchArpResponseTimeGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  return usmDbIpArpRespTimeGet(UnitIndex, val);
}

L7_RC_t
snmpAgentSwitchArpResponseTimeSet(L7_uint32 UnitIndex, L7_uint32 val)
{
  return usmDbIpArpRespTimeSet(UnitIndex, val);
}

L7_RC_t
snmpAgentSwitchArpMaxRetriesGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  return usmDbIpArpRetriesGet(UnitIndex, val);
}

L7_RC_t
snmpAgentSwitchArpMaxRetriesSet(L7_uint32 UnitIndex, L7_uint32 val)
{
  return usmDbIpArpRetriesSet(UnitIndex, val);
}

L7_RC_t
snmpAgentSwitchArpCacheSizeGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  return usmDbIpArpCacheSizeGet(UnitIndex, val);
}

L7_RC_t
snmpAgentSwitchArpCacheSizeSet(L7_uint32 UnitIndex, L7_uint32 val)
{
  return usmDbIpArpCacheSizeSet(UnitIndex, val);
}

L7_RC_t
snmpAgentSwitchArpDynamicRenewGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_uint32 temp_val;

  rc = usmDbIpArpDynamicRenewGet(UnitIndex, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *val = D_agentSwitchArpDynamicRenew_enable; 
      break;

    case L7_DISABLE:
      *val = D_agentSwitchArpDynamicRenew_disable;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}

L7_RC_t
snmpAgentSwitchArpDynamicRenewSet(L7_uint32 UnitIndex, L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 temp_val = 0;

  switch (val)
  {
  case D_agentSwitchArpDynamicRenew_enable:
    temp_val = L7_ENABLE;
    break;

  case D_agentSwitchArpDynamicRenew_disable:
    temp_val = L7_DISABLE;
    break;

  default:
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbIpArpDynamicRenewSet(UnitIndex, temp_val);
  }

  return rc;
}

L7_RC_t
snmpAgentSwitchArpTotalEntryCountCurrentGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_arpCacheStats_t pArpCache; 
  rc = usmDbIpArpCacheStatsGet(UnitIndex, &pArpCache);

  if (rc == L7_SUCCESS)
  {
      *val = pArpCache.cacheCurrent; 
  }
  return rc;

}

L7_RC_t
snmpAgentSwitchArpTotalEntryCountPeakGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_arpCacheStats_t pArpCache; 
  rc = usmDbIpArpCacheStatsGet(UnitIndex, &pArpCache);

  if (rc == L7_SUCCESS)
  {
     *val = pArpCache.cachePeak; 
  }
  return rc;

}

L7_RC_t
snmpAgentSwitchArpStaticEntryCountCurrentGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_arpCacheStats_t pArpCache; 
  rc = usmDbIpArpCacheStatsGet(UnitIndex, &pArpCache);

  if (rc == L7_SUCCESS)
  {
     *val = pArpCache.staticCurrent; 
  }
  return rc;
}

L7_RC_t
snmpAgentSwitchArpStaticEntryCountMaxGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_arpCacheStats_t pArpCache; 
  rc = usmDbIpArpCacheStatsGet(UnitIndex, &pArpCache);

  if (rc == L7_SUCCESS)
  {
     *val = pArpCache.staticMax; 
  }
  return rc;

}

/**************************************************************************************************************/

L7_RC_t
snmpAgentSwitchIpRoutingModeGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_uint32 temp_val;

  rc = usmDbIpRtrAdminModeGet(UnitIndex, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *val = D_agentSwitchIpRoutingMode_enable;
      break;

    case L7_DISABLE:
      *val = D_agentSwitchIpRoutingMode_disable;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}

L7_RC_t
snmpAgentSwitchIpRoutingModeSet(L7_uint32 UnitIndex, L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 temp_val = 0;

  switch (val)
  {
  case D_agentSwitchIpRoutingMode_enable:
    temp_val = L7_ENABLE;
    break;

  case D_agentSwitchIpRoutingMode_disable:
    temp_val = L7_DISABLE;
    break;

  default:
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbIpRtrAdminModeSet(UnitIndex, temp_val);
  }

  return rc;
}

/**************************************************************************************************************/

L7_RC_t
snmpAgentSwitchIpInterfaceGet(L7_uint32 UnitIndex, L7_uint32 extIfNum)
{
  L7_uint32 intIfNum;
  L7_int32 temp_val;

  /* check to see if this is a valid routing interface */
  if (usmDbIntIfNumFromExtIfNum(extIfNum, &intIfNum) == L7_SUCCESS &&
      usmDbValidateRtrIntf(UnitIndex, intIfNum) == L7_SUCCESS &&
      usmDbVisibleInterfaceCheck(UnitIndex, intIfNum, &temp_val) == L7_SUCCESS)
    return L7_SUCCESS;

  return L7_FAILURE;
}

L7_RC_t
snmpAgentSwitchIpInterfaceNextGet(L7_uint32 UnitIndex, L7_uint32 *extIfNum)
{
  /* loop through available external interface numbers */
  while (nimGetNextExtIfNumber(*extIfNum, extIfNum) == L7_SUCCESS)
  {
    /* check to see if this is a valid routing interface */
    if (snmpAgentSwitchIpInterfaceGet(UnitIndex, *extIfNum) == L7_SUCCESS)
      return L7_SUCCESS;
  }

  return L7_FAILURE;
}

L7_RC_t
snmpAgentSwitchIpInterfaceIpAddressGet(L7_uint32 UnitIndex, L7_uint32 intIfIndex, L7_uint32 *val)
{
  L7_uint32 temp_netmask;
  return usmDbIpRtrIntfIpAddressGet(UnitIndex, intIfIndex, val, &temp_netmask);
}

L7_RC_t
snmpAgentSwitchIpInterfaceNetMaskGet(L7_uint32 UnitIndex, L7_uint32 intIfIndex, L7_uint32 *val)
{
  L7_uint32 temp_ip;
  return usmDbIpRtrIntfIpAddressGet(UnitIndex, intIfIndex, &temp_ip, val);
}

/* lvl7_@p1516 start */
L7_RC_t
snmpAgentSwitchIpInterfaceClearIpSet(L7_uint32 UnitIndex, L7_uint32 intIfIndex, L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 temp_ip;
  L7_uint32 temp_netmask;

  switch (val)
  {
  case D_agentSwitchIpInterfaceClearIp_enable:
    /* clear IP configuration */
    rc = usmDbIpRtrIntfIpAddressGet(UnitIndex, intIfIndex, &temp_ip, &temp_netmask);
    if (rc == L7_SUCCESS)
      rc = usmDbIpRtrIntfIPAddressRemove(UnitIndex, intIfIndex, temp_ip, temp_netmask);
    break;

  case D_agentSwitchIpInterfaceClearIp_disable:
    /* allow set to disable */
    rc = L7_SUCCESS;
    break;

  default:
    rc = L7_FAILURE;
  }

  return rc;
}
/* lvl7_@p1516 end */

L7_RC_t
snmpAgentSwitchIpInterfaceRoutingModeGet(L7_uint32 UnitIndex, L7_uint32 intIfIndex, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_uint32 temp_val;

  rc = usmDbIpRtrIntfModeGet(UnitIndex, intIfIndex, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *val = D_agentSwitchIpInterfaceRoutingMode_enable;
      break;

    case L7_DISABLE:
      *val = D_agentSwitchIpInterfaceRoutingMode_disable;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;

}

L7_RC_t
snmpAgentSwitchIpInterfaceRoutingModeSet(L7_uint32 UnitIndex, L7_uint32 intIfIndex, L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 temp_val = 0;

  switch (val)
  {
  case D_agentSwitchIpInterfaceRoutingMode_enable:
    temp_val = L7_ENABLE;
    break;

  case D_agentSwitchIpInterfaceRoutingMode_disable:
    temp_val = L7_DISABLE;
    break;

  default:
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbValidateRtrIntf(UnitIndex, intIfIndex);

    if (rc == L7_SUCCESS)
    {
      rc = usmDbIpRtrIntfModeSet(UnitIndex, intIfIndex, temp_val);
      if(rc == L7_SUCCESS)
      {
        #ifdef L7_IPV6_PACKAGE
        rc = usmDbIp6RtrIntfModeSet(intIfIndex,temp_val);
        #endif
      }

    }
  }

  return rc;
}

L7_RC_t
snmpAgentSwitchIpInterfaceMtuValueGet(L7_uint32 UnitIndex, L7_uint32 intIfIndex, L7_uint32 *val)
{
  L7_RC_t rc;

  rc = usmDbIntfIpMtuGet(UnitIndex, intIfIndex, val);

  if (rc != L7_SUCCESS)
  {
    *val = 0;
    rc = L7_SUCCESS;
  }

  return rc;

}

L7_RC_t
snmpAgentSwitchIpInterfaceMtuValueSet(L7_uint32 UnitIndex, L7_uint32 intIfIndex, L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 temp_val;

  rc = usmDbIntfIpMtuGet(UnitIndex, intIfIndex, &temp_val);

  if (rc != L7_SUCCESS && val == 0)
  {
    rc = L7_SUCCESS;
  }
  else
  {
    rc = usmDbIntfIpMtuSet(UnitIndex, intIfIndex, val);
  }

  return rc;
}

L7_RC_t
snmpAgentSwitchIpInterfaceProxyARPModeGet(L7_uint32 UnitIndex, L7_uint32 intIfIndex, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_uint32 temp_val;

  rc = usmDbProxyArpGet(UnitIndex, intIfIndex, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *val = D_agentSwitchIpInterfaceProxyARPMode_enable;
      break;

    case L7_DISABLE:
      *val = D_agentSwitchIpInterfaceProxyARPMode_disable;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;

}

L7_RC_t
snmpAgentSwitchIpInterfaceProxyARPModeSet(L7_uint32 UnitIndex, L7_uint32 intIfIndex, L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 temp_val = 0;

  switch (val)
  {
  case D_agentSwitchIpInterfaceProxyARPMode_enable:
    temp_val = L7_ENABLE;
    break;

  case D_agentSwitchIpInterfaceProxyARPMode_disable:
    temp_val = L7_DISABLE;
    break;

  default:
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbValidateRtrIntf(UnitIndex, intIfIndex);

    if (rc == L7_SUCCESS)
    {
      rc = usmDbProxyArpSet(UnitIndex, intIfIndex, temp_val);
    }
  }

  return rc;
}

L7_RC_t
snmpAgentSwitchIpInterfaceUnnumberedIfIndexGet(L7_uint32 UnitIndex, L7_uint32 intIfIndex, L7_uint32 *val)
{
  L7_RC_t rc = L7_FAILURE;
  L7_BOOL isUnnumbered = L7_FALSE;
  L7_uint32 numberedIntIfIndex = 0;


  rc = usmDbIpUnnumberedGet(intIfIndex, &isUnnumbered, &numberedIntIfIndex);

  if (rc == L7_SUCCESS && isUnnumbered == L7_TRUE)
  {
    if (usmDbExtIfNumFromIntIfNum(numberedIntIfIndex, val) != L7_SUCCESS)
      *val = 0;
  }
  else
  {
    *val = 0;
  }

  return L7_SUCCESS;
}

L7_RC_t
snmpAgentSwitchIpInterfaceUnnumberedIfIndexSet(L7_uint32 UnitIndex, L7_uint32 intIfIndex, L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_BOOL isUnnumbered = L7_FALSE;
  L7_uint32 numberedIntIfIndex = 0;

  sysapiPrintf("SNMP: %d = %d, %d\n", intIfIndex, isUnnumbered, numberedIntIfIndex);

  if (val != 0)
  {
    isUnnumbered = L7_TRUE;
    rc = usmDbIntIfNumFromExtIfNum(val, &numberedIntIfIndex);
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbIpUnnumberedSet(intIfIndex, isUnnumbered, numberedIntIfIndex);
  }

  return L7_SUCCESS;
}

/* ICMP Unreachables Mode */

L7_RC_t
snmpAgentSwitchIpInterfaceIcmpUnreachablesGet(L7_uint32 UnitIndex, L7_uint32 intIfIndex, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_uint32 temp_val;

  rc = usmDbIpMapICMPUnreachablesModeGet(UnitIndex, intIfIndex, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *val = D_agentSwitchIpInterfaceIcmpUnreachables_enable;
      break;

    case L7_DISABLE:
      *val = D_agentSwitchIpInterfaceIcmpUnreachables_disable;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;

}

L7_RC_t
snmpAgentSwitchIpInterfaceIcmpUnreachablesSet(L7_uint32 UnitIndex, L7_uint32 intIfIndex, L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 temp_val = 0;

  switch (val)
  {
    case D_agentSwitchIpInterfaceIcmpUnreachables_enable:
      temp_val = L7_ENABLE;
      break;

    case D_agentSwitchIpInterfaceIcmpUnreachables_disable:
      temp_val = L7_DISABLE;
      break;

    default:
      rc = L7_FAILURE;
  }
  if (rc == L7_SUCCESS)
  {
    rc = usmDbIpMapICMPUnreachablesModeSet (UnitIndex, intIfIndex, temp_val);
  }

  return rc;
}
/* ICMP Redirects Mode on Interface */

L7_RC_t
snmpAgentSwitchIpInterfaceIcmpRedirectsGet(L7_uint32 UnitIndex, L7_uint32 intIfIndex, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_uint32 temp_val;

  rc = usmDbIpMapIfICMPRedirectsModeGet(UnitIndex, intIfIndex, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *val = D_agentSwitchIpInterfaceIcmpRedirects_enable;
      break;

    case L7_DISABLE:
      *val = D_agentSwitchIpInterfaceIcmpRedirects_disable;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;

}

L7_RC_t
snmpAgentSwitchIpInterfaceIcmpRedirectsSet(L7_uint32 UnitIndex, L7_uint32 intIfIndex, L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 temp_val = 0;

  switch (val)
  {
    case D_agentSwitchIpInterfaceIcmpRedirects_enable:
      temp_val = L7_ENABLE;
      break;

    case D_agentSwitchIpInterfaceIcmpRedirects_disable:
      temp_val = L7_DISABLE;
      break;

     default:
      rc = L7_FAILURE;
  }
  if (rc == L7_SUCCESS)
  {
    rc = usmDbIpMapIfICMPRedirectsModeSet (UnitIndex, intIfIndex, temp_val);
  }

  return rc;
}

/* Echo Reply Mode */

L7_RC_t
snmpAgentSwitchIpIcmpEchoReplyModeGet(L7_uint32 *val)
{
  L7_RC_t rc;
  L7_uint32 temp_val;

  rc = usmDbIpMapRtrICMPEchoReplyModeGet(&temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *val = D_agentSwitchIpIcmpEchoReplyMode_enable;
      break;

    case L7_DISABLE:
      *val = D_agentSwitchIpIcmpEchoReplyMode_disable;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;

}

L7_RC_t
snmpAgentSwitchIpIcmpEchoReplyModeSet(L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 temp_val = 0;

  switch (val)
  {
    case D_agentSwitchIpIcmpEchoReplyMode_enable:
      temp_val = L7_ENABLE;
      break;

    case D_agentSwitchIpIcmpEchoReplyMode_disable:
      temp_val = L7_DISABLE;
      break;

    default:
      rc = L7_FAILURE;
  }
  if (rc == L7_SUCCESS)
  {
    rc = usmDbIpMapRtrICMPEchoReplyModeSet (temp_val);
  }

  return rc;
}

L7_RC_t
snmpAgentSwitchIpIcmpRatelimitIntervalSet(L7_uint32 interval)
{

  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 val, burstsize;

  if (usmDbIpMapRtrICMPRatelimitGet (&burstsize, &val) == L7_SUCCESS)
  {
     if (val != interval)
     {
       rc = usmDbIpMapRtrICMPRatelimitSet (burstsize, interval);
     }
  }

  return rc;
} 

L7_RC_t
snmpAgentSwitchIpIcmpRatelimitBurstSizeSet(L7_uint32 burstsize)
{

  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 val, interval;

  if (usmDbIpMapRtrICMPRatelimitGet (&val, &interval) == L7_SUCCESS)
  {
     if (val != burstsize)
     {
       rc = usmDbIpMapRtrICMPRatelimitSet (burstsize, interval);
     }
  }

  return rc;
}

L7_RC_t
snmpAgentSwitchIpIcmpRedirectsGet( L7_uint32 *val)
{
  L7_RC_t rc;
  L7_uint32 temp_val;

  rc = usmDbIpMapRtrICMPRedirectsModeGet(&temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *val = D_agentSwitchIpIcmpRedirectsMode_enable;
      break;

    case L7_DISABLE:
      *val = D_agentSwitchIpIcmpRedirectsMode_disable;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;

}

L7_RC_t
snmpAgentSwitchIpIcmpRedirectsSet(L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 temp_val = 0;

  switch (val)
  {
    case D_agentSwitchIpIcmpRedirectsMode_enable:
      temp_val = L7_ENABLE;
      break;

    case D_agentSwitchIpIcmpRedirectsMode_disable:
      temp_val = L7_DISABLE;
      break;

     default:
      rc = L7_FAILURE;
  }
  if (rc == L7_SUCCESS)
  {
    rc = usmDbIpMapRtrICMPRedirectsModeSet (temp_val);
  }

  return rc;
}

/**************************************************************************************************************/
L7_RC_t
snmpAgentSwitchIpRouterDiscoveryAdvertiseModeGet(L7_uint32 UnitIndex, L7_uint32 intIfIndex, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_BOOL temp_val;

  rc = usmDbRtrDiscAdvertiseGet(UnitIndex, intIfIndex, &temp_val);
  
  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_TRUE:
      *val = D_agentSwitchIpRouterDiscoveryAdvertiseMode_enable;
      break;
    case L7_FALSE:
      *val = D_agentSwitchIpRouterDiscoveryAdvertiseMode_disable;
      break;
    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}

L7_RC_t
snmpAgentSwitchIpRouterDiscoveryAdvertiseModeSet(L7_uint32 UnitIndex, L7_uint32 intIfIndex, L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_BOOL temp_val = L7_FALSE;

  switch (val)
  {
  case D_agentSwitchIpRouterDiscoveryAdvertiseMode_enable:
    temp_val = L7_TRUE;
    break;

  case D_agentSwitchIpRouterDiscoveryAdvertiseMode_disable:
    temp_val = L7_FALSE;
    break;

  default:
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbRtrDiscAdvertiseSet(UnitIndex, intIfIndex, temp_val);
  }

  return rc;
}

L7_RC_t
snmpAgentSwitchIpRouterDiscoveryMaxAdvertisementIntervalSet(L7_uint32 UnitIndex, L7_uint32 intIfIndex, L7_uint32 val)
{
  return usmDbRtrDiscMaxAdvIntervalSet(UnitIndex, intIfIndex, val);
}

L7_RC_t
snmpAgentSwitchIpRouterDiscoveryMinAdvertisementIntervalSet(L7_uint32 UnitIndex, L7_uint32 intIfIndex, L7_uint32 val)
{
  return usmDbRtrDiscMinAdvIntervalSet(UnitIndex, intIfIndex, val);
}

L7_RC_t
snmpAgentSwitchIpRouterDiscoveryAdvertisementLifetimeSet(L7_uint32 UnitIndex, L7_uint32 intIfIndex, L7_uint32 val)
{
  return usmDbRtrDiscAdvLifetimeSet(UnitIndex, intIfIndex, val);
}

/**************************************************************************************************************/

L7_RC_t
snmpAgentSwitchSecondaryAddressEntryGet(L7_uint32 UnitIndex, L7_uint32 ifIndex, L7_uint32 ipAddress, L7_uint32 *netMask)
{
  L7_uint32 intIfNum;
  L7_int32 temp_val;
  L7_rtrIntfIpAddr_t ipAddrList[L7_L3_NUM_IP_ADDRS];
  L7_uint32 i; /* address loop index */

  /* check to see if this is a valid routing interface */
  if (usmDbIntIfNumFromExtIfNum(ifIndex, &intIfNum) == L7_SUCCESS &&
      usmDbValidateRtrIntf(UnitIndex, intIfNum) == L7_SUCCESS &&
      usmDbVisibleInterfaceCheck(UnitIndex, intIfNum, &temp_val) == L7_SUCCESS &&
      usmDbIpRtrIntfCfgIpAddrListGet(UnitIndex, intIfNum, ipAddrList) == L7_SUCCESS)
  {
    for (i=1; i<L7_L3_NUM_IP_ADDRS; i++)
    {
      if (ipAddrList[i].ipAddr == ipAddress)
      {
        *netMask = ipAddrList[i].ipMask;
        return L7_SUCCESS;
      }
    }
  }

  return L7_FAILURE;
}

L7_RC_t
snmpAgentSwitchSecondaryAddressEntryNextGet(L7_uint32 UnitIndex, L7_uint32 *ifIndex, L7_uint32 *ipAddress, L7_uint32 *netMask)
{
  L7_uint32 intIfNum;
  L7_int32 temp_val;
  L7_rtrIntfIpAddr_t ipAddrList[L7_L3_NUM_IP_ADDRS];
  L7_rtrIntfIpAddr_t tempIpAddr;
  L7_uint32 i; /* address loop index */

  do
  {
    /* clear the temp value */
    tempIpAddr.ipAddr = 0;
    tempIpAddr.ipMask = 0;

    /* check to see if this is a valid routing interface */
    if (usmDbIntIfNumFromExtIfNum(*ifIndex, &intIfNum) == L7_SUCCESS &&
        usmDbValidateRtrIntf(UnitIndex, intIfNum) == L7_SUCCESS &&
        usmDbVisibleInterfaceCheck(UnitIndex, intIfNum, &temp_val) == L7_SUCCESS &&
        usmDbIpRtrIntfCfgIpAddrListGet(UnitIndex, intIfNum, ipAddrList) == L7_SUCCESS)
    {
      /* find the next greater address for this interface */
      for (i=1; i<L7_L3_NUM_IP_ADDRS; i++)
      {
        /* if greater than requested address */
        if (ipAddrList[i].ipAddr > *ipAddress)
        {
          /* if first greater, or less than first found greater address */
          if (tempIpAddr.ipAddr == 0 ||
              ipAddrList[i].ipAddr < tempIpAddr.ipAddr)
          {
            tempIpAddr.ipAddr = ipAddrList[i].ipAddr;
            tempIpAddr.ipMask = ipAddrList[i].ipMask;
          }
        }
      }

      /* if an address is found, return */
      if (tempIpAddr.ipAddr != 0)
      {
        *ipAddress = tempIpAddr.ipAddr;
        *netMask = tempIpAddr.ipMask;
        return L7_SUCCESS;
      }
    }
  }
  /* loop through available external interface numbers */
  while (nimGetNextExtIfNumber(*ifIndex, ifIndex) == L7_SUCCESS);

  return L7_FAILURE;
}

L7_RC_t
snmpAgentSwitchIntfIpHelperAddressEntryGet(L7_uint32 ifIndex, 
                                           L7_uint32 serverAddress,
                                           L7_ushort16 udpPort,
                                           L7_BOOL discard,
                                           L7_uint32 *hitCount)
{
  L7_uint32 intIfNum;
  L7_int32 temp_val;
  L7_uint32 unit = 1;
  L7_BOOL discardEntry = L7_FALSE;

  /* check to see if interface could be configured as a routing interface */
  if ((usmDbIntIfNumFromExtIfNum(ifIndex, &intIfNum) != L7_SUCCESS) ||
      (usmDbValidateRtrIntf(unit, intIfNum) != L7_SUCCESS) ||
      (usmDbVisibleInterfaceCheck(unit, intIfNum, &temp_val) != L7_SUCCESS))
  {
    return L7_FAILURE;
  }

  /* A server address of 0.0.0.0 indicates a discard entry for the SNMP agent. */
  if (serverAddress == 0)
  {
    discardEntry = L7_TRUE;
  }

  return usmDbIpHelperAddressGet(intIfNum, udpPort, serverAddress, discardEntry, hitCount);
}

L7_RC_t
snmpAgentSwitchIntfIpHelperAddressEntryNextGet(L7_uint32 *ifIndex,
                                               L7_uint32 *serverAddress,
                                               L7_uint32 *udpPort,
                                               L7_int32 *discard,
                                               L7_uint32 *hitCount)
{
  L7_uint32 intIfNum;
  L7_uint32 nextIntIfNum;
  L7_int32 temp_val;
  L7_uint32 unit = 1;
  L7_BOOL nextDisc = (L7_BOOL) *discard;
  L7_ushort16 nextUport = (L7_ushort16) *udpPort;
  L7_uint32 nextSvrAddr = *serverAddress;
  L7_RC_t rc;

  do
  {
    /* check to see if this is a valid routing interface */
    if ((usmDbIntIfNumFromExtIfNum(*ifIndex, &intIfNum) == L7_SUCCESS) &&
        (usmDbValidateRtrIntf(unit, intIfNum) == L7_SUCCESS) &&
        (usmDbVisibleInterfaceCheck(unit, intIfNum, &temp_val) == L7_SUCCESS))
    {
      nextIntIfNum = intIfNum;
      if ((usmDbIpHelperAddressNext(&nextIntIfNum, &nextUport, &nextSvrAddr, &nextDisc, 
                                    hitCount) == L7_SUCCESS) &&
          (nextIntIfNum == intIfNum))
      {
        *serverAddress = nextSvrAddr;
        *udpPort = (L7_uint32) nextUport;
        *discard = (L7_int32) nextDisc;
        return L7_SUCCESS;
      }
    }
    /* Go to next interface. Reset other values.  */
    rc = usmDbGetNextExtIfNumber(*ifIndex, ifIndex);
    nextUport = 0;
    nextSvrAddr = 0;
    nextDisc = L7_FALSE;
  }
  /* loop through available external interface numbers */
  while (rc == L7_SUCCESS);

  return L7_FAILURE;
}

L7_RC_t
snmpAgentIpHelperAdminModeGet(L7_uint32 *val)
{
  L7_uint32 adminMode;

  L7_RC_t rc = usmDbIpHelperAdminModeGet(&adminMode);

  if (rc == L7_SUCCESS)
  {
    switch (adminMode)
    {
    case L7_ENABLE:
      *val = D_agentIpHelperAdminMode_enable;
      break;
    case L7_DISABLE:
      *val = D_agentIpHelperAdminMode_disable;
      break;
    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }
  return rc;
}

L7_RC_t
snmpAgentSwitchIpHelperAddressEntryGet(L7_uint32 serverAddress,
                                       L7_ushort16 udpPort,
                                       L7_uint32 *hitCount)
{
  return usmDbIpHelperAddressGet(IH_INTF_ANY, udpPort, serverAddress, L7_FALSE, hitCount);
}

L7_RC_t
snmpAgentSwitchIpHelperAddressEntryNextGet(L7_uint32 *serverAddress,
                                           L7_uint32 *udpPort,
                                           L7_uint32 *hitCount)
{
  L7_uint32 intIfNum = IH_INTF_ANY;
  L7_BOOL disc;
  L7_ushort16 uport = (L7_ushort16) *udpPort;
  L7_RC_t rc;

  rc = usmDbIpHelperAddressNext(&intIfNum, &uport, serverAddress, &disc, hitCount);
  while (rc == L7_SUCCESS)
  {
    if (intIfNum == IH_INTF_ANY)
    {
      *udpPort = (L7_uint32) uport;
      return L7_SUCCESS;
    }
    rc = usmDbIpHelperAddressNext(&intIfNum, &uport, serverAddress, &disc, hitCount);
  }

  return L7_FAILURE;
}

L7_RC_t
snmpAgentIpHelperAdminModeSet(L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 temp_val = 0;

  switch (val)
  {
    case D_agentIpHelperAdminMode_enable:
      temp_val = L7_ENABLE;
      break;

    case D_agentIpHelperAdminMode_disable:
      temp_val = L7_DISABLE;
      break;

    default:
      rc = L7_FAILURE;
  }
  if (rc == L7_SUCCESS)
  {
    rc = usmDbIpHelperAdminModeSet(temp_val);
  }

  return rc;
}

/* Find routing interface that uses a given VLAN ID */
L7_RC_t
snmpAgentSwitchInternalVlanEntryGet(L7_uint32 UnitIndex,
                                    L7_uint32 vlanId, 
                                    L7_uint32 *ifIndex)
{
  L7_uint32 intIfNum;

  if (usmDbIpRtrInternalVlanIdToIntIfNum(vlanId, &intIfNum) != L7_SUCCESS)
    return L7_FAILURE;

  if (usmDbExtIfNumFromIntIfNum(intIfNum, ifIndex) != L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;
}

L7_RC_t
snmpAgentSwitchInternalVlanEntryNextGet(L7_uint32 UnitIndex,
                                        L7_uint32 *vlanId,
                                        L7_uint32 *ifIndex)
{
  L7_uint32 intIfNum;

  if (usmDbIpRtrInternalVlanFindNext(*vlanId, vlanId, &intIfNum) != L7_SUCCESS)
    return L7_FAILURE;

  if (usmDbExtIfNumFromIntIfNum(intIfNum, ifIndex) != L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;
}


/**************************************************************************************************************/





/*********************************************************************
* @purpose  Gets the Forward Mode of DHCP relay agent
*
*
*
* @param    UnitIndex  @b{(input)}  Unit
* @param    val  	   @b{(output)} Forward Mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/

L7_RC_t
snmpBootpDhcpRelayForwardModeGet ( L7_uint32 UnitIndex, L7_int32 *val )
{
  L7_int32 temp_val;
  L7_RC_t rc;

  rc = L7_FAILURE;

  rc = usmDbIpHelperAdminModeGet (&temp_val);

  if (rc == L7_SUCCESS)
  {
      switch (temp_val)
      {
      case L7_ENABLE:
        *val = D_agentBootpDhcpRelayForwardMode_enable;
        break;

      case L7_DISABLE:
        *val = D_agentBootpDhcpRelayForwardMode_disable;
        break;

      default:
        *val = 0;
        rc = L7_FAILURE;
      }
  }

  return rc;
}

/*********************************************************************
* @purpose  Gets the Relay Agent Circuit ID option mode
*
*
*
* @param    UnitIndex  @b{(input)}  Unit
* @param    val  	   @b{(output)} Circuit ID Optin Mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/

L7_RC_t
snmpBootpDhcpRelayCircuitIdOptionModeGet ( L7_uint32 UnitIndex, L7_int32 *val )
{
  L7_int32 temp_val;
  L7_RC_t rc;

  rc = L7_FAILURE;

  rc = usmDbBootpDhcpRelayCircuitIdOptionModeGet (UnitIndex, &temp_val);

  if (rc == L7_SUCCESS)
  {
      switch (temp_val)
      {
      case L7_ENABLE:
        *val = D_agentBootpDhcpRelayCircuitIdOptionMode_enable;
        break;

      case L7_DISABLE:
        *val = D_agentBootpDhcpRelayCircuitIdOptionMode_disable;
        break;

      default:
        *val = 0;
        rc = L7_FAILURE;
      }
  }

  return rc;
}

#ifdef I_agentBootpDhcpRelayCheckReply
/*********************************************************************
* @purpose  Gets the Relay Agent Circuit ID check option mode
*
*
*
* @param    val            @b{(output)} Circuit ID Option check  Mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t snmpBootpDhcpRelayCheckReplyModeGet(L7_int32 *val)
{
  L7_uint32 temp_val; 
  L7_RC_t rc;

  rc = L7_FAILURE;

  rc = usmDbBootpDhcpRelayCIDOptionCheckModeGet(&temp_val);

  if (rc == L7_SUCCESS)
  {
      switch (temp_val)
      {
      case L7_ENABLE:
        *val = D_agentBootpDhcpRelayCheckReply_enable;
        break;

      case L7_DISABLE:
        *val = D_agentBootpDhcpRelayCheckReply_disable;
        break;

      default:
        *val = 0;
        rc = L7_FAILURE;
      }
  }

  return rc;
}
#endif

#ifdef I_agentBootpDhcpRelayIntfCircuitIdOptionMode
/************************************************************************
* @purpose  Gets the Relay Agent router interface  Circuit ID option mode
*
* @param    intIfNum       @b{(input)} internal interface number
* @param    val            @b{(output)} Circuit ID Option check  Mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
**************************************************************************/
L7_RC_t snmpAgentBootpDhcpRelayIntfEntryCIDOptionGet(L7_uint32 intIfNum, L7_int32 *val)
{
  dhcpRelayIntfValue_t temp_val;
  L7_RC_t rc;
  rc = usmDbBootpDhcpRelayrtrCIDOptionModeGet (intIfNum, &temp_val);

  if(rc == L7_SUCCESS)
  {
    switch(temp_val)
    {
     case L7_RELAY_INTF_DISABLE:
          *val = D_agentBootpDhcpRelayIntfCircuitIdOptionMode_disable;
          break;

     case L7_RELAY_INTF_ENABLE:
          *val = D_agentBootpDhcpRelayIntfCircuitIdOptionMode_enable;
          break;

     case L7_RELAY_INTF_NOT_CONFIGURED:
          *val = D_agentBootpDhcpRelayIntfCircuitIdOptionMode_none;
          break; 

     default:
          rc = L7_FAILURE;    
    }
  }
  return rc;
}
#endif

#ifdef I_agentBootpDhcpRelayIntfCheckReply
/************************************************************************
* @purpose  Gets the Relay Agent router interface Circuit ID check reply mode
*
* @param    intIfNum       @b{(input)} internal interface number
* @param    val            @b{(output)} Circuit ID Option check  Mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
**************************************************************************/
L7_RC_t snmpAgentBootpDhcpRelayIntfEntryCIDCheckReplyGet(L7_uint32 intIfNum, L7_int32 *val)
{
  dhcpRelayIntfValue_t temp_val;
  L7_RC_t rc;
  rc = usmDbBootpDhcpRelayrtrCIDOptionCheckModeGet (intIfNum, &temp_val);

  if(rc == L7_SUCCESS)
  {
    switch(temp_val)
    {
     case L7_RELAY_INTF_DISABLE:
          *val = D_agentBootpDhcpRelayIntfCheckReply_disable;
          break;

     case L7_RELAY_INTF_ENABLE:
          *val = D_agentBootpDhcpRelayIntfCheckReply_enable;
          break;

     case L7_RELAY_INTF_NOT_CONFIGURED:
          *val = D_agentBootpDhcpRelayIntfCheckReply_none;
          break;

     default:
          rc = L7_FAILURE;
    }
  }
  return rc;
}
#endif

/*********************************************************************
* @purpose  Sets the Forward mode of Relay
*
*
*
* @param    UnitIndex  @b{(input)}  Unit
* @param    val  	   @b{(input)} Forward Mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/

L7_RC_t
snmpBootpDhcpRelayForwardModeSet ( L7_uint32 UnitIndex, L7_int32 val )
{
    L7_int32 temp_val = 0;
    L7_RC_t rc = L7_SUCCESS;

    switch (val)
    {
    case D_agentBootpDhcpRelayForwardMode_enable:
      temp_val = L7_ENABLE;
      break;

    case D_agentBootpDhcpRelayForwardMode_disable:
      temp_val = L7_DISABLE;
      break;

    default:
      rc = L7_FAILURE;
    }

    if (rc == L7_SUCCESS)
    {
        rc = usmDbIpHelperAdminModeSet(temp_val);
    }

  return rc;
}

/*********************************************************************
* @purpose  Sets the Circuit ID Option mode
*
*
*
* @param    UnitIndex  @b{(input)}  Unit
* @param    val  	   @b{(input)} Circit ID Option Mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/

L7_RC_t
snmpBootpDhcpRelayCircuitIdOptionModeSet ( L7_uint32 UnitIndex, L7_int32 val )
{
    L7_int32 temp_val = 0;
    L7_RC_t rc = L7_SUCCESS;

    switch (val)
    {
    case D_agentBootpDhcpRelayCircuitIdOptionMode_enable:
      temp_val = L7_ENABLE;
      break;

    case D_agentBootpDhcpRelayCircuitIdOptionMode_disable:
      temp_val = L7_DISABLE;
      break;

    default:
      rc = L7_FAILURE;
    }

    if (rc == L7_SUCCESS)
    {
        rc = usmDbBootpDhcpRelayCircuitIdOptionModeSet(UnitIndex, temp_val);
    }

  return rc;
}

#ifdef I_agentBootpDhcpRelayCheckReply
/*********************************************************************
* @purpose  Sets the Circuit ID check reply mode
*
*
*
* @param    UnitIndex  @b{(input)}  Unit
* @param    val            @b{(input)} Circit ID Option Mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/

L7_RC_t snmpBootpDhcpRelayCheckReplyModeSet(L7_int32 val)
{
  L7_int32 temp_val = 0;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
   case D_agentBootpDhcpRelayCheckReply_enable:
     temp_val = L7_ENABLE;
     break;

   case D_agentBootpDhcpRelayCheckReply_disable:
     temp_val = L7_DISABLE;
     break;

   default:
     rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
     rc = usmDbBootpDhcpRelayCIDOptionCheckModeSet(temp_val);
  }

  return rc;

}
#endif

#ifdef I_agentBootpDhcpRelayIntfCircuitIdOptionMode
/************************************************************************
* @purpose  Sets the Relay Agent router interface  Circuit ID option mode
*
* @param    intIfNum       @b{(input)} internal interface number
* @param    val            @b{(input)} Circuit ID Option check  Mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
**************************************************************************/

L7_RC_t snmpAgentBootpDhcpRelayIntfEntryCIDOptionSet(L7_uint32 intIfNum, L7_int32 val)
{
  dhcpRelayIntfValue_t temp_val;
  L7_RC_t rc = L7_SUCCESS;

  switch(val)
  {
   case D_agentBootpDhcpRelayIntfCircuitIdOptionMode_disable:
        temp_val = L7_RELAY_INTF_DISABLE;
        break;

   case D_agentBootpDhcpRelayIntfCircuitIdOptionMode_enable:
        temp_val = L7_RELAY_INTF_ENABLE;
        break;

   case D_agentBootpDhcpRelayIntfCircuitIdOptionMode_none:
        temp_val = L7_RELAY_INTF_NOT_CONFIGURED;
        break;  

   default:
        rc = L7_FAILURE;
  }
  
  if(rc == L7_SUCCESS)
  {
   rc = usmDbBootpDhcpRelayrtrCIDOptionModeSet(intIfNum, temp_val);
  }

  return rc; 
}
#endif

#ifdef I_agentBootpDhcpRelayIntfCheckReply
/************************************************************************
* @purpose  Sets the Relay Agent router interface Circuit ID check mode
*
* @param    intIfNum       @b{(input)} internal interface number
* @param    val            @b{(input)} Circuit ID Option check  Mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
**************************************************************************/

L7_RC_t snmpAgentBootpDhcpRelayIntfEntryCIDCheckReplySet(L7_uint32 intIfNum, L7_uint32 val)
{
  dhcpRelayIntfValue_t temp_val;
  L7_RC_t rc = L7_SUCCESS;
 
  switch(val)
  {
    case D_agentBootpDhcpRelayIntfCheckReply_disable:
         temp_val = L7_RELAY_INTF_DISABLE;
         break;
 
    case D_agentBootpDhcpRelayIntfCheckReply_enable:
         temp_val = L7_RELAY_INTF_ENABLE;
         break;
    
    case D_agentBootpDhcpRelayIntfCheckReply_none:
         temp_val = L7_RELAY_INTF_NOT_CONFIGURED;
         break;

    default:
         rc = L7_FAILURE;
  }
  
  if(rc == L7_SUCCESS)
    rc = usmDbBootpDhcpRelayrtrCIDOptionCheckModeSet(intIfNum, val); 
 
  return rc;
}
#endif
 
/*********************************************************************
* @purpose  Set the local proxy ARP mode for the interface
*
* @param    UnitIndex       @b{(input)} Unit for this operation
* @param    intIfNum        @b{(input)} internal interface number
* @param    mode            @b{(input)} Local Proxy ARP Mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  Check return code to determine result.
*
* @end
*********************************************************************/
L7_RC_t snmpAgentSwitchLocalProxyArpModeSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 mode)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 temp_val;

  if( usmDbValidateRtrIntf(UnitIndex,intIfNum) == L7_FAILURE)
    return rc;


  switch (mode)
  {
    case D_agentSwitchLocalProxyArpMode_enable:
      temp_val = L7_ENABLE;
      break;

    case D_agentSwitchLocalProxyArpMode_disable:
      temp_val = L7_DISABLE;
      break;

    default:
      return rc;
      break;
  }
  return (usmDbLocalProxyArpSet(UnitIndex, intIfNum, temp_val));
}

/*********************************************************************
* @purpose  Get the local proxy ARP mode for the interface
*
* @param    UnitIndex       @b{(input)} Unit for this operation
* @param    intIfNum        @b{(input)} internal interface number
* @param    *mode           @b{(output)} Local Proxy ARP Mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  Check return code to determine result.
*
* @end
*********************************************************************/
L7_RC_t snmpAgentSwitchLocalProxyArpModeGet(L7_uint32 UnitIndex, L7_uint32 intIfNum,L7_uint32 *mode)
{
  L7_RC_t rc;
  L7_uint32 temp_val;

  rc = usmDbLocalProxyArpGet(UnitIndex, intIfNum, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
      case L7_ENABLE:
        *mode = D_agentSwitchLocalProxyArpMode_enable ;
        break;
          
      case L7_DISABLE:
        *mode = D_agentSwitchLocalProxyArpMode_disable ;
        break;

      default:
        *mode = 0;
        rc = L7_FAILURE;
    }
  }
  return(rc);
}

/*********************************************************************
* @purpose  Validate the inerface number and return the internal interface number
*
* @param    UnitIndex       @b{(input)} Unit for this operation
* @param    extIfNum        @b{(input)} external interface number
* @param    *intIfNum       @b{(output)} internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  Check return code to determine result.
*
* @end
*********************************************************************/
L7_RC_t snmpAgentSwitchLocalProxyArpEntryValidate(L7_uint32 UnitIndex, L7_uint32 extIfNum, L7_uint32 *intIfNum)
{

  L7_uint32 temp_val;

  /* check to see if this is a valid routing interface */
  if (usmDbIntIfNumFromExtIfNum(extIfNum, intIfNum) == L7_SUCCESS &&
      usmDbValidateRtrIntf(UnitIndex, *intIfNum) == L7_SUCCESS &&
      usmDbVisibleInterfaceCheck(UnitIndex, *intIfNum, &temp_val) == L7_SUCCESS)
    return L7_SUCCESS;

  return L7_FAILURE;


}

/*********************************************************************
* @purpose  Validate the inerface number and return the internal interface number
*
* @param    UnitIndex       @b{(input)} Unit for this operation
* @param    extIfNum        @b{(input)} external interface number
* @param    *intIfNum       @b{(output)} internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  Check return code to determine result.
*
* @end
*********************************************************************/
L7_RC_t snmpAgentSwitchLocalProxyArpEntryGetNext(L7_uint32 UnitIndex, L7_uint32 *extIfNum, L7_uint32 *intIfNum)
{
 
  /* loop through available external interface numbers */
  while (nimGetNextExtIfNumber(*extIfNum, extIfNum) == L7_SUCCESS)
  {
    /* check to see if this is a valid routing interface */
    if (snmpAgentSwitchLocalProxyArpEntryValidate(UnitIndex, *extIfNum, intIfNum) == L7_SUCCESS)
      return L7_SUCCESS;
  }
  return L7_FAILURE;


}

/*********************************************************************
* @purpose  Set the IP Address Configuration Method
*
* @param    UnitIndex       @b{(input)} Unit for this operation
* @param    intIfIndex      @b{(input)} internal interface number
* @param    val             @b{(input)} IP Address Method of router
*                                       interface
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  Check return code to determine result.
*
* @end
*********************************************************************/
L7_RC_t snmpAgentRtrIntfIpAddressMethodSet(L7_uint32 UnitIndex,
                                           L7_uint32 intIfIndex,
                                           L7_uint32 val)
{
  L7_INTF_IP_ADDR_METHOD_t method = 0;

  switch (val)
  {
    case D_agentSwitchIPAddressConfigMethod_none:
      method = L7_INTF_IP_ADDR_METHOD_NONE;
      break;

    case D_agentSwitchIPAddressConfigMethod_manual:
      method = L7_INTF_IP_ADDR_METHOD_CONFIG;
      break;

    case D_agentSwitchIPAddressConfigMethod_dhcp:
      method = L7_INTF_IP_ADDR_METHOD_DHCP;
      break;  
  }
  return usmDbIntfIpAddressMethodSet (intIfIndex, method, L7_MGMT_IPPORT,
                                      L7_FALSE);
}

/*********************************************************************
* @purpose  Get the IP Address Configuration Method
*
* @param    UnitIndex       @b{(input)} Unit for this operation
* @param    intIfIndex      @b{(input)} internal interface number
* @param    *method         @b{(output)} IP Address Method of router
*                                       interface
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  Check return code to determine result.
*
* @end
*********************************************************************/
L7_RC_t snmpAgentRtrIntfIpAddressMethodGet(L7_uint32 UnitIndex,
                                           L7_uint32 intIfIndex,
                                           L7_uint32 *method)
{
  return usmDbIpRtrIntfIpAddressMethodGet(intIfIndex,
                                          (L7_INTF_IP_ADDR_METHOD_t *) method);
}

/*********************************************************************
* @purpose  Renews or Releases the DHCP Client IP Address on an interface
*
* @param    UnitIndex       @b{(input)} Unit for this operation
* @param    intIfIndex      @b{(input)} internal interface number
* @param    val             @b{(input)} Operation performed on router
*                                       interface.{none | renew | release}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  Check return code to determine result.
*
* @end
*********************************************************************/
L7_RC_t snmpAgentDhcpClientOperation(L7_uint32 UnitIndex, L7_uint32 intIfIndex,
                                     L7_uint32 val)
{
  L7_INTF_IP_ADDR_METHOD_t method;
  L7_RC_t rc = L7_FAILURE;

  if(val == D_agentSwitchDhcpOperation_none)
    rc = L7_SUCCESS;

  else if(usmDbIpRtrIntfIpAddressMethodGet(intIfIndex, &method)
                                           == L7_SUCCESS)
  {
    if(method == L7_INTF_IP_ADDR_METHOD_DHCP)
    {
      if (val == D_agentSwitchDhcpOperation_renew)
      {
        rc = usmDbIntfIpAddressMethodSet (intIfIndex, L7_INTF_IP_ADDR_RENEW,
                                          L7_MGMT_IPPORT, L7_FALSE);
      }
      else if (val == D_agentSwitchDhcpOperation_release)
      {
        rc = usmDbIntfIpAddressMethodSet (intIfIndex, L7_INTF_IP_ADDR_RELEASE,
                                          L7_MGMT_IPPORT, L7_FALSE);
      }
    }
  }
  return rc;
}

/*********************************************************************
* @purpose  Get the Global Default Gateway Address
*
* @param    UnitIndex       @b{(input)} Unit for this operation
* @param    *defGwAddr      @b{(output)} Pointer to the default global
*                                       gateway address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  Check return code to determine result.
*
* @end
*********************************************************************/
L7_RC_t snmpAgentDefaultGlobalGatewayAddressGet(L7_uint32 UnitIndex, L7_uint32 *defGwAddr)
{
  usmDbIpGlobalDefaultGatewayAddressGet((L7_IP_ADDR_t *) defGwAddr, L7_FALSE);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the Global Default Gateway Address
*
* @param    UnitIndex       @b{(input)} Unit for this operation
* @param    defGwAddr       @b{(input)} Address of the global default
*                                       gateway
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  Check return code to determine result.
*
* @end
*********************************************************************/
L7_RC_t snmpAgentDefaultGlobalGatewayAddressSet(L7_uint32 UnitIndex, L7_uint32 defGwAddr)
{
  return usmDbIpGlobalDefaultGatewayAddressSet(defGwAddr);
}

/*********************************************************************
* @purpose  Reset the Global Default Gateway Address
*
* @param    val       @b{(input)} Enable or Disable
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t snmpAgentGlobalDefaultGatewayAddressReset(L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_agentSwitchClearIpDefaultGateway_enable:
    /* clear Default Gateway configuration */
    rc = usmDbIpGlobalDefaultGatewayAddressReset();
    break;

  case D_agentSwitchClearIpDefaultGateway_disable:
    /* allow set to disable */
    rc = L7_SUCCESS;
    break;

  default:
    rc = L7_FAILURE;
  }

  return rc;
}

