/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   usmdb_rtrdisc_api.h
*
* @purpose    USMDB Router Discovery API functions
*
* @component  unitmgr
*
* @comments 
*
* @create     11/19/2001
*
* @author     kdesai
*
* @end
*
**********************************************************************/

/*************************************************************
                                        
*************************************************************/

#ifndef _USMDB_RTR_DISC_API_H_
#define _USMDB_RTR_DISC_API_H_

/*********************************************************************
* @purpose  Gets the router discovery mode on the interface
*          
* @param    UnitIndex  @b{(input)}  the unit for this operation
* @param    intIfNum   @b{(input)}  internal interface number
* @param    *flag      @b{(output)} advertisement flag 
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE
*
* @comments
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbRtrDiscAdvertiseGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_BOOL *flag);

/*********************************************************************
* @purpose  Sets the router discovery mode on the interface
*          
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    intIfNum   @b{(input)} internal interface number
* @param    flag       @b{(input)} advertisement flag
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE
*
* @comments
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbRtrDiscAdvertiseSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_BOOL flag);

/*********************************************************************
* @purpose  Gets the advertisement address that is used as the destination
*           ip address in the advertisement packet
*                
*          
* @param    UnitIndex    @b{(input)}  the unit for this operation
* @param    intIfNum     @b{(input)}  internal interface number
* @param    *IpAddress   @b{(output)} advertisement address
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE
*
* @comments
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbRtrDiscAdvAddrGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_IP_ADDR_t *ipAddr);

/*********************************************************************
* @purpose  Sets the advertisement address that is used as the destination
*           ip address in the advertisement packet
*          
* @param    UnitIndex   @b{(input)} the unit for this operation
* @param    intfNum     @b{(input)} internal interface number
* @param    IpAddress   @b{(input)} advertisement address
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE
*
* @comments
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbRtrDiscAdvAddrSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_IP_ADDR_t ipAddr);

/*********************************************************************
* @purpose  Gets the maximum time allowed between sending router  
*           advertisements from the interface.
*          
* @param    UnitIndex   @b{(input)} the unit for this operation
* @param    intIfNum    @b{(input)} internal interface number
* @param    *time       @b{(output)} time in seconds
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE
*
* @comments
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbRtrDiscMaxAdvIntervalGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *time);

/*********************************************************************
* @purpose  Sets the maximum time allowed between sending router  
*           advertisements from the interface.
*          
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    intIfNum   @b{(input)} internal interface number
* @param    time       @b{(input)} time in seconds
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE
*
* @comments
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbRtrDiscMaxAdvIntervalSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 time);

/*********************************************************************
* @purpose  Gets the minimum time allowed between sending router
*           advertisements from the interface.
*          
* @param    UnitIndex  @b{(input)}  the unit for this operation
* @param    intIfNum   @b{(input)}  internal interface number
* @param    *time      @b{(output)} time in seconds ( 3 < t < MaxAdvertisementInterval )
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE
*
* @comments
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbRtrDiscMinAdvIntervalGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *time);

/*********************************************************************
* @purpose  Sets the minimum time allowed between sending router
*           advertisements from the interface.
*          
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    intIfNum   @b{(input)} internal interface number
* @param    time       @b{(input)} time in seconds
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE
*
* @comments
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbRtrDiscMinAdvIntervalSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 time);

/*********************************************************************
* @purpose  Gets the value of lifetime field of router advertisement sent 
*           from the interface.
*          
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    intIfNum   @b{(input)} internal interface number
* @param    *time      @b{(output)} time in seconds
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE
*
* @comments
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbRtrDiscAdvLifetimeGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *time);

/*********************************************************************
* @purpose  Sets the value of lifetime field of router advertisement sent 
*           from the interface.
*          
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    intIfNum   @b{(input)} internal interface number
* @param    time       @b{(input)} time in seconds
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE
*
* @comments
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbRtrDiscAdvLifetimeSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 time);

/*********************************************************************
* @purpose  Gets the preferability of the address as a default router 
*           address, relative to other router addresses on the same subnet.  
*          
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    intIfNum   @b{(input)} internal interface number
* @param    *integer   @b{(output)} preference level
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE
*
* @comments
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbRtrDiscPreferenceLevelGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_int32 *integer);

/*********************************************************************
* @purpose  Sets the preferability of the address as a default router 
*           address, relative to other router addresses on the same subnet.  
*          
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    intIfNum   @b{(input)} internal interface number
* @param    integer    @b{(input)} preference level
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE
*
* @comments
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbRtrDiscPreferenceLevelSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_int32 integer);

/*********************************************************************
* @purpose  Check if an interface is valid for rtrdisc
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    intIfNum   @b{(input)} internal interface number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    none
*                                 
* @end
*********************************************************************/
extern L7_BOOL usmDbRtrDiscIsValidIntf(L7_uint32 UnitIndex, L7_uint32 intIfNum);

#endif /* _USMDB_RTR_DISC_API_H_ */


