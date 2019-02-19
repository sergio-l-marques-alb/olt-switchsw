/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename l3end_api.h
*
* @purpose API interface for the rounting interface management.
*
* @component 
*
* @create 03/13/01
*
* @author alt
* @end
*
*
*********************************************************************/
/*************************************************************
                    
*************************************************************/



#ifndef L3END_API_H
#define L3END_API_H

#include "l7_common.h"
#include "sysnet_api.h"

/* Interface type.
*/
#define L3END_DIX   1
#define L3END_802_3 2


/* Begin Function Prototypes */

/*********************************************************************
* @purpose  Do phase 1 initialization actions for the LAYER3 component.
*
* @param    max_interfaces    Maximum number of router interfaces.
*
* @returns  L7_SUCCESS  If successful
* @returns  L7_ERROR  If not successful
*
* @notes    For simplicity, the instance for each end driver MUST
* @notes    correspond to the associated instance for each internal
* @notes    router interface number (rtrIfNum) inside the IP Mapping
* @notes    layer. However, this number (end driver instance, rtrIfNum)
* @notes    has meaning only within the associated component.  The 
* @notes    system-wide internal interface number is used in all 
* @notes    external APIs. Thus the IP Mapping APIs ipMapRtrIntfToIntIfNum() 
* @notes    and ipMapIntIfNumToRtrIntf() can be used to toggle between the 
* @notes    driver instance and the system-wide intIfNum instance. 
*       
* @end
*********************************************************************/
L7_RC_t ipmRouterIfInitPhase1Process(L7_uint32 max_interfaces);

/*********************************************************************
* @purpose  De-initialize router drivers. Phase 1
*
* @param    max_interfaces    Maximum number of router interfaces.
*
* @returns  L7_SUCCESS  If successful
* @returns  L7_ERROR  If not successful
*
* @notes   
*       
* @end
*********************************************************************/
L7_RC_t ipmRouterIfFiniPhase1Process(L7_uint32 max_interfaces);


/*********************************************************************
* @purpose  Do phase 2 initialization actions for the LAYER3 component.
*
* @param    max_interfaces    Maximum number of router interfaces.
*
* @returns  L7_SUCCESS  If successful
* @returns  L7_ERROR  If not successful
*
* @end
*********************************************************************/
L7_RC_t ipmRouterIfInitPhase2Process(L7_uint32 max_interfaces);


/*********************************************************************
* @purpose  De-initialize router drivers. Phase 2
*
* @param    void
*
* @returns  L7_SUCCESS  If successful
* @returns  L7_ERROR  If not successful
*
*       
* @end
*********************************************************************/
L7_RC_t ipmRouterIfFiniPhase2Process(void);

/*********************************************************************
*
* @purpose  Associate a router interface with an IP address and MAC address.
*
* @param    index       Router interface index.
* @param    intIfNum    Internal Interface Number associated with this router port.
* @param    ip          IP address to be used for the interface
* @param    netmask     Network mask.
* @param    mac         MAC Address
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes    If the interface is unnumbered, ip is the IP address on 
*           the corresponding numbered interface and netmask is also
*           for the numbered interface. We may choose to use a netmask
*           of 0xFFFFFFFF in the IP stack for unnumbered interfaces. If
*           so, that adjustment is made here. 
*
* @end
*********************************************************************/
L7_RC_t
ipmRouterIfCreate ( L7_uint32 index, L7_uint32 intIfNum, L7_uint32 ip, 
                    L7_uint32 netmask, L7_BOOL unnumbered,
                    L7_uchar8 *mac );

/*********************************************************************
* @purpose  Associate a secondary ip address with a router interface
*
* @param    intIfNum    Internal Interface Number associated with this router port.
* @param    ip          Secondary IP Address to be associated with this router 
*                       interface
* @param    netmask     Network mask.
* @param    index       Index to addrs[] array on interface
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    Incorrect parameters.
* @returns  L7_FAILURE  if other failure
*
* @notes    none 
*
* @end
*********************************************************************/
L7_RC_t
ipmRouterIfSecondaryAddrAdd( L7_uint32 intIfNum, L7_uint32 ip, 
                             L7_uint32 netmask, L7_uint32 index);

/*********************************************************************
*
* @purpose  Remove router IP interface.
*
* @param    intIfNum    Internal Interface Number associated with this router port.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes    none 
*
* @end
*********************************************************************/
L7_RC_t
ipmRouterIfRemove ( L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Remove a previously configured secondary ip address from the
*           specified router interface
*
* @param    intIfNum    Internal Interface Number associated with this router port.
* @param    ip          Secondary IP Address to be associated with this router 
*                       interface
* @param    netmask     Network mask.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    Incorrect parameters.
* @returns  L7_FAILURE  if other failure
*
* @notes    none 
*
* @end
*********************************************************************/
L7_RC_t
ipmRouterIfSecondaryAddrDelete( L7_uint32 intIfNum, L7_uint32 ip, 
                                L7_uint32 netmask, L7_uint32 index);

/*********************************************************************
*
* @purpose  Get maximum number of routing interfaces
*
* @returns  max_if  Maximum number of routing interfaces.
*
* @notes    none 
*
* @end
*********************************************************************/
L7_uint32
ipmRouterIfMaxCountGet (void);


/*********************************************************************
*
* @purpose  Get default gateway for the specified interface.
*
* @param    intIfNum    Internal Interface Number
*
* @returns  0    No valid default gateway defined for this interface.
* @returns  gateway_ip Default gateway IP address.
*
* @notes    none 
*
* @end
*********************************************************************/
L7_uint32
ipmRouterIfDefaultGatewayGet (L7_uint32 intIfNum);

/*********************************************************************
*
* @purpose  Get name string prefix for the driver
*
* @param    buf        (output) Buffer to store name string.
*
* @returns  L7_SUCCESS 
*
* @notes    none 
*
* @end
*********************************************************************/
L7_RC_t
ipmRouterIfDrvNamePrefixGet( L7_uchar8 *buf);


/*********************************************************************
*
* @purpose  Get cost metric for the specified interface.
*
* @param    intIfNum   Internal Interface Number
* @param    metric     (output) Interface metric.
*
* @returns  L7_SUCCESS Interface is valid.
* @returns  L7_FAILURE Interface is not valid.
*
* @notes    none 
*
* @end
*********************************************************************/
L7_RC_t
ipmRouterIfMetricGet (L7_uint32 ifIndex, L7_uint32 *metric);

/*********************************************************************
*
* @purpose  Create task to service ARP requests.
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t 
ipmArpTaskCreate(void);

/*********************************************************************
*
* @purpose  Delete task servicing ARP requests.
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ipmArpTaskDelete(void);

/*********************************************************************
*
* @purpose  Transmit OSAPI network buffer on the specified IP interface.
*
* @param    ipIfIndex    IP Interface index. 
* @param    bufHandle     VxWorks buffer containing the frame.
*
* @notes    This function is used by routing code and ARP code to 
*           send frames on the network.
*
* @end
*********************************************************************/
L7_RC_t
ipmRouterIfBufSend (L7_uint32 ipIfIndex, L7_netBufHandle  bufHandle);

/*********************************************************************
*
* @purpose  Process incoming IP and ARP frames.
*
* @param    bufHandle   Network buffer handle for the incoming message.
* @param    *pduInfo    pointer to PDU info structure which stores intIfNum and vlanId
*
* @returns  L7_SUCCESS  Frame has been consumed.
* @returns  L7_FAILURE  Frame has not been consumed.
*
* @notes    none 
*
* @end
*********************************************************************/
L7_RC_t
ipmRouterIfFrameProcess (L7_netBufHandle bufHandle, sysnet_pdu_info_t *pduInfo);

/*********************************************************************
*
* @purpose  Process local IP packets
*
* @param    intIfNum       internal Interface Number
* @param    bufHandle     VxWorks buffer containing the frame.
*
* @returns  L7_SUCCESS      if packet is consumed by the local IP stack
* @returns  L7_FAILURE      if packet is not consumed by the local IP stack
*
* @notes    This function is used to send multicast and broadcast packets
*           to the underlying vxWorks stack.
*
* @end
*********************************************************************/
L7_RC_t
ipmLocalPacketProcess(L7_uint32 intIfNum, L7_netBufHandle  bufHandle);



/*********************************************************************
*
* @purpose  associate router intf with intIfNum in driver.
*
* @param    index       Router interface index.
* @param    intIfNum    Internal Interface Number associated with this router port.
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    Incorrect parameters.
* @returns  L7_FAILURE  if other failure
*
* @notes    none 
*
* @end
*********************************************************************/
L7_RC_t
ipmRouterIfMapCreate ( L7_uint32 index, L7_uint32 intIfNum );


/*********************************************************************
*
* @purpose  de-associate router intf with intIfNum in driver.
*
* @param    index       Router interface index.
* @param    intIfNum    Internal Interface Number associated with this router port.
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    Incorrect parameters.
* @returns  L7_FAILURE  if other failure
*
* @notes    none 
*
* @end
*********************************************************************/
L7_RC_t
ipmRouterIfMapDelete ( L7_uint32 index, L7_uint32 intIfNum );


/*********************************************************************
*
* @purpose  driver up
*
* @param    index       Router interface index.
* @param    intIfNum    Internal Interface Number associated with this router port.
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    Incorrect parameters.
* @returns  L7_FAILURE  if other failure
*
* @notes    none 
*
* @end
*********************************************************************/
L7_RC_t
ipmRouterIfDrvUp ( L7_uint32 index);

/*********************************************************************
*
* @purpose  driver down
*
* @param    index       Router interface index.
* @param    intIfNum    Internal Interface Number associated with this router port.
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    Incorrect parameters.
* @returns  L7_FAILURE  if other failure
*
* @notes    none 
*
* @end
*********************************************************************/
L7_RC_t
ipmRouterIfDrvDown ( L7_uint32 index);


/* End Function Prototypes */
#endif /* L3END_API */
