/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename usmdb_iputil_api.h
*
* @purpose Provide interface to hardware API's for unitmgr components
*
* @component unitmgr
*
* @comments This is a common header file for usmdb_iputil.c in 
*           FASTPATH 201 & 300
*
* @create   06/26/01
*
* @author   anayar
* @end
*
**********************************************************************/

/*********************************************************************
                         
**********************************************************************
*********************************************************************/

#ifndef USMDB_IPUTIL_API_H
#define USMDB_IPUTIL_API_H

/* Begin Function Prototypes */

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
L7_RC_t usmDbIpCpuStaticArpAdd(L7_uint32 unitIndex, L7_uint32 oldIpAddr, L7_uint32 ipAddr);

/*********************************************************************
* @purpose  Add a static ARP entry for the service port
*
* @param    unitIndex   Unit Index
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
L7_RC_t usmDbIpServPortStaticArpAdd(L7_uint32 unitIndex, L7_uint32 oldIpAddr, L7_uint32 ipAddr);

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
                           L7_uint32 *intIfNum);

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
                                L7_uint32 *intIfNum);

/*********************************************************************
*
* @purpose determine if the specified interface is valid for routing 
*
* @param    unitIndex          Unit Index
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
L7_RC_t usmDbValidateRtrIntf(L7_uint32 unitIndex, L7_uint32 iface);
#ifdef L7_ROUTING_PACKAGE
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
extern L7_RC_t usmDbArpMapPacketDebugTraceFlagSet(L7_BOOL flag);

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
extern L7_BOOL usmDbArpPacketDebugTraceFlagGet();

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
extern L7_RC_t usmDbIpMapPacketDebugTraceFlagSet(L7_uint32 aclnum, L7_BOOL flag);

/*********************************************************************
* @purpose  Get the current status of displaying ip packet debug info
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
extern L7_RC_t usmDbIpPacketDebugTraceFlagGet( L7_uint32 aclnum, L7_BOOL *flag);
#endif

#endif
/* End Function Prototypes */

#endif /* USMDB_IPUTIL_API_H */
