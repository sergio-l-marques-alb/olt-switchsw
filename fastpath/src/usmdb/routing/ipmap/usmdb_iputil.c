/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename usmdb_iputil.c
*
* @purpose Provide interface to hardware API's for unitmgr components
*
* @component unitmgr
*
* @comments ESA 300 version
*
* @create   06/26/01
*
* @author   anayar
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/



#include <string.h>
#include <l7_common.h>
#include <l7_packet.h>

#include <l7_ip_api.h>
#include <l3_comm_structs.h>
#include "vrrp_debug_api.h"
#include "arp_debug_api.h"
#include "l7_ip_api.h"
#include "usmdb_1213_api.h"
#include "usmdb_iputil_api.h"
#include "usmdb_util_api.h"

/*********************************************************************
* @purpose  Add a static ARP entry for the cpu interface
*
* @param    unitIndex   Unit Index
* @param    oldIpAddr   old ip address of the cpu interface
* @param    ipAddress   newly configured IP Address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none 
*                                                   
* @end
*********************************************************************/
L7_RC_t usmDbIpCpuStaticArpAdd(L7_uint32 unitIndex, L7_uint32 oldIpAddr, L7_uint32 ipAddr)
{
  L7_linkLayerAddr_t lladdr;
  L7_char8 mac[L7_MAC_ADDR_LEN];

  /* If this is the case the entry already exists in the arp table */
  if (oldIpAddr == ipAddr)
    return L7_SUCCESS;

  /* Clear contents of structure before using it */
  bzero((L7_char8 *)&lladdr, sizeof(L7_linkLayerAddr_t));

  /* If the old ip address is not null, remove the old entry before
     adding a new entry in the arp table */
  if (oldIpAddr != L7_NULL_IP_ADDR)
    ipMapStaticArpDelete(oldIpAddr, L7_INVALID_INTF);

  /* Get the system mac address */
  simGetSystemIPBurnedInMac(mac);

  /* If both the ip address & mac address are valid add the static
     arp entry */
  if (ipAddr != L7_NULL_IP_ADDR && usmDbIsMacZero(mac) != L7_TRUE)
  {
    /* Populate the link layer addr structure to call ipmap */
    lladdr.len = L7_MAC_ADDR_LEN;
    lladdr.type = L7_LL_ETHERNET;
    memcpy((L7_char8*)lladdr.addr.enetAddr.addr, (L7_char8*)mac,
           L7_MAC_ADDR_LEN);

    return ipMapStaticArpAdd((L7_IP_ADDR_t)ipAddr, &lladdr, L7_INVALID_INTF);
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Add a static ARP entry for the service port
*
* @param    UnitIndex   Unit Index
* @param    oldIpAddr   old service port ip address
* @param    ipAddress   newly configured IP Address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none 
*                                                   
* @end
*********************************************************************/
L7_RC_t usmDbIpServPortStaticArpAdd(L7_uint32 UnitIndex, L7_uint32 oldIpAddr, L7_uint32 ipAddr)
{
  L7_linkLayerAddr_t lladdr;
  L7_char8 mac[L7_MAC_ADDR_LEN];

  /* If this is the case the entry already exists in the arp table */
  if (oldIpAddr == ipAddr)
    return L7_SUCCESS;

  /* Clear contents of structure before using it */
  bzero((L7_char8 *)&lladdr, sizeof(L7_linkLayerAddr_t));

  /* If the old ip address is not null, remove the old entry before
     adding a new entry in the arp table */
  if (oldIpAddr != L7_NULL_IP_ADDR)
    ipMapStaticArpDelete(oldIpAddr, L7_INVALID_INTF);

  /* Get the service port mac address */
  simGetServicePortBurnedInMac(mac);

  /* If both the ip address & mac address are valid add the static
     arp entry */
  if (ipAddr != L7_NULL_IP_ADDR && usmDbIsMacZero(mac) != L7_TRUE)
  {
    /* Populate the link layer addr structure to call ipmap */
    lladdr.len = L7_MAC_ADDR_LEN;
    lladdr.type = L7_LL_ETHERNET;
    memcpy((L7_char8*)lladdr.addr.enetAddr.addr, (L7_char8*)mac,
           L7_MAC_ADDR_LEN);

    /* Right now I'm using an internal interface number of 0 for
       the service port */
    return ipMapStaticArpAdd((L7_IP_ADDR_t)ipAddr, &lladdr, L7_INVALID_INTF);
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Convert an interface IP address to an internal interface number
*
* @param    UnitIndex   Unit for this operation
* @param    IpAddr      @{(input)} IP Address of interest
* @param    *intIfNum   @{(output)} Internal interface number for the 
*                                   specified ip address
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t usmDbIPAddrToIntIf(L7_uint32 UnitIndex, L7_uint32 IpAddr,
                           L7_uint32 *intIfNum)
{
  if (ipMapIpAddressToIntf((L7_IP_ADDR_t)IpAddr, intIfNum) == L7_SUCCESS)
    return L7_SUCCESS;

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Determine if IP Address is a configured Router Interface
*
* @param    UnitIndex   Unit for this operation
* @param    ipAddr      IP address of desired router interface
* @param    *intIfNum   pointer to internal interface number of routing intf
*
* @returns  L7_SUCCESS  router interface found for specified IP address
* @returns  L7_FAILURE  router interface not found for specified IP address
*
* @notes    This differs from usmDbIPAddrToIntf() in that it works
*           from the configuration data, and does not depend on the 
*           current admin mode setting or whether the configuration 
*           is applied or not.
*       
* @end
*********************************************************************/
L7_RC_t usmDbIpAddrIsCfgRtrIntf(L7_uint32 UnitIndex, L7_IP_ADDR_t ipAddr, 
                                L7_uint32 *intIfNum)
{
  return ipMapIpAddressToIntf(ipAddr, intIfNum);
}

/*********************************************************************
*
* @purpose determine if the specified interface is valid for routing 
*
* @param UnitIndex     Unit Index
* @param iface (input) interface to validate
*                     
* @returns  L7_SUCCESS  The interface is valid for routing
* @returns  L7_FAILURE  The interface is not valid for routing
*     
* @notes   If port-based routing is supported, any slot.port is 
*          considered valid.  This routine assumes the range for
*          slot and port have already been verified.
* @end
*
*********************************************************************/
L7_RC_t usmDbValidateRtrIntf(L7_uint32 UnitIndex, L7_uint32 iface)
{
  L7_uint32 itype;

  if (nimGetIfType(iface, &itype) == L7_SUCCESS)
  {
    if (itype == L7_IANA_OTHER_CPU)
      return L7_FAILURE;

    if ( usmDbComponentPresentCheck(UnitIndex, L7_IP_MAP_COMPONENT_ID) == L7_FALSE )
    {
      if ( !((itype == L7_IANA_L2_VLAN) || 
             (itype == L7_IANA_LOGICAL_DESC) ) )
        return L7_FAILURE;
    }
    else
      if ((itype == L7_IANA_LAG_DESC) &&
          (usmDbFeaturePresentCheck(UnitIndex, L7_IP_MAP_COMPONENT_ID,
	                                       L7_IPMAP_FEATURE_LAG_RTING_SUPPORT_FEATURE_ID) == L7_FALSE))
    {
      return L7_FAILURE;
    }
    return L7_SUCCESS;
  }
  return L7_FAILURE;

}
/*********************************************************************
* @purpose  Turns on/off the displaying of arp packet debug info
*
* @param    flag         new value of the Packet Debug flag
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbArpMapPacketDebugTraceFlagSet(L7_BOOL flag)
{
  return arpDebugPacketTraceFlagSet(flag);
}

/*********************************************************************
* @purpose  Get the current status of displaying arp packet debug info
*
* @param    none
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL usmDbArpPacketDebugTraceFlagGet()
{
 return arpDebugPacketTraceFlagGet();
}

#ifdef L7_QOS_FLEX_PACKAGE_ACL

/*********************************************************************
* @purpose  Turns on/off the displaying of ip packet debug info
*
* @param    flag         new value of the Packet Debug flag
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbIpMapPacketDebugTraceFlagSet(L7_uint32 acl, L7_BOOL flag)
{
  return ipMapDebugPacketTraceFlagSet(acl, flag);
}
/*********************************************************************
* @purpose  Get the current status of displaying arp packet debug info
*
* @param    none
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbIpPacketDebugTraceFlagGet(L7_uint32 acl, L7_BOOL *flag)
{
  return ipMapDebugPacketTraceFlagGet(acl, flag);
}
#endif /*L7_QOS_FLEX_PACKAGE_ACL*/

