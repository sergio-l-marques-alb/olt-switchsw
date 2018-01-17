/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename ipv6_provisioning_api.h
*
* @purpose Contains LVL7 ipv6 provisioning API prototypes
*
* @component 
*
* @comments 
*
* @create 09/03/2003
*
* @author nshrivastav
* @end
*
**********************************************************************/



#ifndef INCLUDE_IPV6_PROVISIONING_API_H
#define INCLUDE_IPV6_PROVISIONING_API_H


#ifdef IPV6_PROVISIONING_API_GLOBALS
  #define IPV6_PROVISIONING_API_EXT
#else
  #define IPV6_PROVISIONING_API_EXT extern
#endif 



/*********************************************************************
* @purpose To enable/disable the forwarding of packets between two ports
*          or copying the packets to user application
*
* @param   srcIfNum    @b{(input)} The internal interface number from which
*                                 the IPv6 packet must be copied
*          dstIfNum    @b{(input)} The internal interface number to which
*                                 IPv6 packets must be copied to
*          mode        @b{(input)} L7_ENABLE or L7_DISABLE
*
*          copyToCpu   @b{(input)} L7_ENABLE - To enable copying of packets
*                                  to CPU when bridging between ports
*                                  L7_DISABLE - Copying of packets to CPU
*                                  will not be enabled while bridging between
*                                  ports
*
*
* @returns  L7_SUCCESS
*           L7_FAILURE
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t ipv6ProvBridgeSrcIfNumDstIfNum(L7_uint32 srcIfNum, L7_uint32 dstIfNum, 
                                              L7_uint32 mode, L7_uint32 copyToCpu);

/*********************************************************************
* @purpose Return the internal interface number to which IPv6 packets are
*          being forwarded from an interface
*
*
* @param    srcIfNum    @b{(input)}  The source interface number
*           dstIfNum    @b{(output)} The destination interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t ipv6ProvGetDstIfNum(L7_uint32 srcIfNum, L7_uint32 *dstIfNum);

/*********************************************************************
* @purpose Returns the total number of IPv6 packets passed to application
*
*
* @param    none
*
* @returns  Number of IPv6 packets passed to application component
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_uint32 ipv6ProvGetStats(void);

/*********************************************************************
* @purpose Clears the IPv6 packets statistics
*
*
* @param    none
*
* @returns  
*
* @notes    none
*
* @end
*********************************************************************/
extern void ipv6ProvClearStats(void);

/*********************************************************************
* @purpose  Determine if the interface type is valid to participate in IPv^ Provisioning
*
* @param    sysIntfType              @b{(input)} interface type
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments
*
* @end
*********************************************************************/
L7_BOOL ipv6ProvIsValidIntfType(L7_uint32 sysIntfType);

/*********************************************************************
* @purpose  Determine if the interface is valid to participate in IPv^ Provisioning
*
* @param    intIfNum              @b{(input)} internal interface number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments
*       
* @end
*********************************************************************/
extern L7_BOOL ipv6ProvIsValidIntf(L7_uint32 intIfNum);

#endif /* INCLUDE_IPV6_PROVISIONING_API_H */
