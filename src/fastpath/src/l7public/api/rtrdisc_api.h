/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
* @filename  rtrdisc_api.h
*
* @purpose   Router Discovery API functions
*
* @component 
*
* @comments  
*
* @create    11/05/2001
*
* @author    ksdesai
*
* @end
*             
**********************************************************************/

/*************************************************************
        
*************************************************************/

#ifndef _RTR_DISC_API_H_
#define _RTR_DISC_API_H_

/*********************************************************************
* @purpose  Gets the router discovery mode on the interface
*          
* @param    intIfNum   @b{(input)}  internal interface number
* @param    *flag      @b{(output)} advertisement flag L7_TRUE or L7_FALSE
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE The interface doesnot exists.
*
* @comments
*        
* @end
*********************************************************************/
extern L7_RC_t rtrDiscAdvertiseGet(L7_uint32 intIfNum, L7_BOOL *flag);

/*********************************************************************
* @purpose  Sets the router discovery mode on the interface
*          
* @param    intIfNum   @b{(input)} internal interface number
* @param    flag       @b{(input)} advertisement flag L7_TRUE or L7_FALSE
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE
*
* @comments
*        
* @end
*********************************************************************/
extern L7_RC_t rtrDiscAdvertiseSet(L7_uint32 intIfNum, L7_BOOL flag);

/*********************************************************************
* @purpose  Gets the advertisement address that is used as the destination
*           ip address in the advertisement packet
*                
*          
* @param    intIfNum     @b{(input)}  internal interface number
* @param    *ipAddr      @b{(output)} advertisement address
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE  The interface doesnot exists.
*
* @comments
*        
* @end
*********************************************************************/
extern L7_RC_t rtrDiscAdvAddrGet(L7_uint32 intIfNum, L7_IP_ADDR_t *ipAddr);

/*********************************************************************
* @purpose  Sets the advertisement address that is used as the destination
*           ip address in the advertisement packet
*          
* @param    intfNum     @b{(input)} internal interface number
* @param    ipAddr      @b{(input)} advertisement address
*
* @returns  L7_SUCCESS 
*           L7_FAILURE
*
* @comments
*        
* @end
*********************************************************************/
extern L7_RC_t rtrDiscAdvAddrSet(L7_uint32 intIfNum, L7_IP_ADDR_t ipAddr);

/*********************************************************************
* @purpose  Gets the maximum time allowed between sending router  
*           advertisements from the interface.
*          
* @param    intIfNum    @b{(input)} internal interface number
* @param    *time       @b{(output)} time in seconds
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE  The interface doesnot exists.
*
* @comments
*        
* @end
*********************************************************************/
extern L7_RC_t rtrDiscMaxAdvIntervalGet(L7_uint32 intIfNum, L7_uint32 *time);

/*********************************************************************
* @purpose  Sets the maximum time allowed between sending router  
*           advertisements from the interface.
*          
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
extern L7_RC_t rtrDiscMaxAdvIntervalSet(L7_uint32 intIfNum, L7_uint32 time);

/*********************************************************************
* @purpose  Gets the minimum time allowed between sending router
*           advertisements from the interface.
*          
* @param    intIfNum   @b{(input)}  internal interface number
* @param    *time      @b{(output)} time in seconds
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE  The interface doesnot exists.
*
* @comments
*        
* @end
*********************************************************************/
extern L7_RC_t rtrDiscMinAdvIntervalGet(L7_uint32 intIfNum, L7_uint32 *time);

/*********************************************************************
* @purpose  Sets the minimum time allowed between sending router
*           advertisements from the interface.
*          
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
extern L7_RC_t rtrDiscMinAdvIntervalSet(L7_uint32 intIfNum, L7_uint32 time);

/*********************************************************************
* @purpose  Get the default MinAdvertisementInterval.
*          
* @param    intIfNum     @b{(input)}  internal interface number
* @param    defMinAdvInt @b{(output)} default MinAdvertisementInterval in seconds
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE  
*
* @comments The default value is dynamic and depends on the current value
*           of MaxAdvertisementInterval.
*        
* @end
*********************************************************************/
extern L7_RC_t rtrDiscMinAdvIntervalDefaultGet(L7_uint32 intIfNum, L7_uint32 *defMinAdvInt);

/*********************************************************************
* @purpose  Revert MinAdvertisementInterval to its default value
*          
* @param    intIfNum     @b{(input)}  internal interface number
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE  
*
* @comments The default value is dynamic and depends on the current value
*           of MaxAdvertisementInterval.
*        
* @end
*********************************************************************/
extern L7_RC_t rtrDiscMinAdvIntervalRevert(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Gets the value of lifetime field of router advertisement sent 
*           from the interface.
*          
* @param    intIfNum   @b{(input)} internal interface number
* @param    *time      @b{(output)} time in seconds
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE  The interface doesnot exists.
*
* @comments
*        
* @end
*********************************************************************/
extern L7_RC_t rtrDiscAdvLifetimeGet(L7_uint32 intIfNum, L7_uint32 *time);

/*********************************************************************
* @purpose  Sets the value of lifetime field of router advertisement sent 
*           from the interface.
*          
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
extern L7_RC_t rtrDiscAdvLifetimeSet(L7_uint32 intIfNum, L7_uint32 time);

/*********************************************************************
* @purpose  Sets the value of lifetime field of router advertisement sent 
*           from the interface.
*          
* @param    intIfNum     @b{(input)} internal interface number
* @param    defLifetime  @b{(input)} lifetime in seconds
*
* @returns  L7_SUCCESS 
*           L7_ERROR - if lifetime is > max value
*           L7_REQUEST_DENIED - if lifetime is < max adv interval 
*           L7_FAILURE - any other failure
*
* @comments
*        
* @end
*********************************************************************/
extern L7_RC_t rtrDiscAdvLifetimeDefaultGet(L7_uint32 intIfNum, L7_uint32 *defLifetime);

/*********************************************************************
* @purpose  Revert AdvertisementLifetime to its default value
*          
* @param    intIfNum     @b{(input)}  internal interface number
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE  
*
* @comments The default value is dynamic and depends on the current value
*           of MaxAdvertisementInterval.
*        
* @end
*********************************************************************/
extern L7_RC_t rtrDiscAdvLifetimeRevert(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Gets the preferability of the address as a default router 
*           address, relative to other router addresses on the same subnet.  
*          
* @param    intIfNum   @b{(input)} internal interface number
* @param    *integer   @b{(output)} preference level
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE  The interface doesnot exists.
*
* @comments
*        
* @end
*********************************************************************/
extern L7_RC_t rtrDiscPreferenceLevelGet(L7_uint32 intfNum, L7_int32 *integer);

/********************************************************************
* @purpose  Sets the preferability of the address as a default router 
*           address, relative to other router addresses on the same subnet.  
*          
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
extern L7_RC_t rtrDiscPreferenceLevelSet(L7_uint32 intIfNum, L7_int32 integer);

/**************************************************************************
* @purpose  Apply the max adv interval
*
* @param    intIfNum    Internal interface number
* @param    time        Max adv interval time in sec
*
* @returns  
*
* @comments    
*
* @end
*************************************************************************/
extern void rtrDiscMaxAdvIntervalApply(L7_uint32 intIfNum, L7_uint32 time);

/*********************************************************************
* @purpose  Checks to see if the interface type is valid
*
* @param    sysIntfType   @b{(input)} interface type
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments
*
* @end
*********************************************************************/
extern L7_BOOL rtrDiscIntfIsValidType(L7_uint32 sysIntfType);

/*********************************************************************
* @purpose  Checks to see if the interface is valid
*          
* @param    intIfNum   @b{(input)} internal interface number
*
* @returns  L7_TRUE 
* @returns  L7_FALSE
*
* @comments
*        
* @end
*********************************************************************/
extern L7_BOOL rtrDiscIntfIsValid(L7_uint32 intIfNum);

#endif /* _RTR_DISC_API_H_ */
