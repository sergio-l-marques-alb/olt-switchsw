/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
* @filename   usmdb_rtrdisc.c
*
* @purpose    USMDB Router Discovery API functions
*
* @component  unitmgr 
*
* @comments
*
* @create     11/19/2001
*
* @author     ksdesai
*
* @end
*             
**********************************************************************/

/*************************************************************
        
*************************************************************/

#include "l7_common.h"
#include "usmdb_rtrdisc_api.h"
#include "l3_comm_structs.h"
#include "rtrdisc_api.h"


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
L7_RC_t usmDbRtrDiscAdvertiseGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_BOOL *flag)
{
  return rtrDiscAdvertiseGet(intIfNum, flag); 
}


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
L7_RC_t usmDbRtrDiscAdvertiseSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_BOOL flag)
{
  return rtrDiscAdvertiseSet(intIfNum, flag);
}


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
L7_RC_t usmDbRtrDiscAdvAddrGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_IP_ADDR_t *ipAddr)
{
  return rtrDiscAdvAddrGet(intIfNum, ipAddr);
}


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
L7_RC_t usmDbRtrDiscAdvAddrSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_IP_ADDR_t ipAddr)
{
  return rtrDiscAdvAddrSet(intIfNum, ipAddr);
}


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
L7_RC_t usmDbRtrDiscMaxAdvIntervalGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *time)
{
  return rtrDiscMaxAdvIntervalGet(intIfNum, time);
}


/*********************************************************************
* @purpose  Sets the maximum time allowed between sending router  
*           advertisements from the interface.
*          
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    intIfNum   @b{(input)} internal interface number
* @param    time       @b{(input)} time in seconds
*
* @returns  L7_SUCCESS 
*           L7_ERROR - if maxAdvInt is out of range
*           L7_REQUEST_DENIED - if maxAdvInt < configured minAdvInt
*           L7_FAILURE - any other failure
*
* @comments
*        
* @end
*********************************************************************/
L7_RC_t usmDbRtrDiscMaxAdvIntervalSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 time)
{
  return rtrDiscMaxAdvIntervalSet(intIfNum, time);
}


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
L7_RC_t usmDbRtrDiscMinAdvIntervalGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *time)
{
  return rtrDiscMinAdvIntervalGet(intIfNum, time);
}


/*********************************************************************
* @purpose  Sets the minimum time allowed between sending router
*           advertisements from the interface.
*          
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    intIfNum   @b{(input)} internal interface number
* @param    time       @b{(input)} time in seconds
*
* @returns  L7_SUCCESS 
*           L7_ERROR - min adv interval is out of range
*           L7_REQUEST_DENIED - If minAdvInt > maxAdvInt
*           L7_FAILURE - any other error
*
* @comments
*        
* @end
*********************************************************************/
L7_RC_t usmDbRtrDiscMinAdvIntervalSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 time)
{
  return rtrDiscMinAdvIntervalSet(intIfNum, time);
}

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
L7_RC_t usmDbRtrDiscMinAdvIntervalDefaultGet(L7_uint32 intIfNum, L7_uint32 *defMinAdvInt)
{
  return rtrDiscMinAdvIntervalDefaultGet(intIfNum, defMinAdvInt);
}

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
L7_RC_t usmDbRtrDiscMinAdvIntervalRevert(L7_uint32 intIfNum)
{
  return rtrDiscMinAdvIntervalRevert(intIfNum);
}

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
L7_RC_t usmDbRtrDiscAdvLifetimeGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *time)
{
  return rtrDiscAdvLifetimeGet(intIfNum, time);
}


/*********************************************************************
* @purpose  Sets the value of lifetime field of router advertisement sent 
*           from the interface.
*          
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    intIfNum   @b{(input)} internal interface number
* @param    time       @b{(input)} time in seconds
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
L7_RC_t usmDbRtrDiscAdvLifetimeSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 time)
{
  return rtrDiscAdvLifetimeSet(intIfNum, time);
}

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
L7_RC_t usmDbRtrDiscAdvLifetimeDefaultGet(L7_uint32 intIfNum, L7_uint32 *defLifetime)
{
  return rtrDiscAdvLifetimeDefaultGet(intIfNum, defLifetime);
}

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
L7_RC_t usmDbRtrDiscAdvLifetimeRevert(L7_uint32 intIfNum)
{
  return rtrDiscAdvLifetimeRevert(intIfNum);
}

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
L7_RC_t usmDbRtrDiscPreferenceLevelGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_int32 *integer)
{
  return rtrDiscPreferenceLevelGet(intIfNum, integer);
}


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
L7_RC_t usmDbRtrDiscPreferenceLevelSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_int32 integer)
{
  return rtrDiscPreferenceLevelSet(intIfNum, integer);
}


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
L7_BOOL usmDbRtrDiscIsValidIntf(L7_uint32 UnitIndex, L7_uint32 intIfNum)
{
  return rtrDiscIntfIsValid(intIfNum);
}
