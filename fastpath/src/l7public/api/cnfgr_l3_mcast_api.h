/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename cnfgr_l3_mcast_api.h
*
* @purpose  Provide for System Integration APIs for IP Multicast
*
* @component cnfgr
*
* @comments These routines are provided for system integration purposes.
*           These routines are used to customize an offering by providing a 
*           single place for box integration of a component.  Developers are
*           encouraged to use this layer rather than making box-specific changes
*           in the private layers of a component.
*
* @create  07/15/2002
*
* @author w. jacobs
* @end
*
**********************************************************************/



#ifndef _CNFGR_L3_MCAST_API_H_
#define _CNFGR_L3_MCAST_API_H_

    

/*********************************************************************
*
* @purpose Determine if multicast routes associated with the protocol are forwarded 
* @purpose only in the host CPU.
*
* @param L7_uint32 protocol   @b((input)) one of L7_MCAST_IANA_MROUTE_* definitions
*
* @returns L7_TRUE      If forwarding is supported only in the host CPU
* @returns L7_FALSE     Otherwise.
*
* @notes  If the protocol is not recognized, L7_FALSE is returned.
*       
* @end
*
*********************************************************************/
L7_BOOL cnfgrIntegrateIpMcastForwardingInHostGet(L7_uint32 protocol);


/*********************************************************************
*
* @purpose Determine if the interface is a valid IP Multicast interface
*
* @param L7_uint32 intIfNum   @b((input)) internal interface number (Always 0)
*
* @returns L7_TRUE      If interface is supported by this component
* @returns L7_FALSE     Otherwise.
*
* @notes  None
*       
* @end
*
*********************************************************************/
L7_BOOL cnfgrIntegrateIpMcastIsSupportedInterface(L7_uint32 intIfNum);


#endif /* _CNFGR_L3_MCAST_API_H_ */
