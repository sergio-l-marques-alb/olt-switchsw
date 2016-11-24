
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename cable_status.h  
*
* @purpose System Status tool function prototypes
*
* @component status
*
* @comments none
*
* @create 09/19/2002
*
* @author Jill Flanagan
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/




#ifndef STATUS_H
#define STATUS_H

#include <datatypes.h>
#include "osapi.h"
/*********************************************************************
* @purpose  Returns the cable status
*
* @param L7_uint32          intIfNum           @b((input))  
* @param L7_CABLE_STATUS_t *cableStatus        @b((output))  
* @param L7_BOOL           *lengthKnown        @b((output))  
* @param L7_uint32         *shortestLength     @b((output))  
* @param L7_uint32         *longestLength      @b((output))  
* @param L7_uint32         *cableFailureLength @b((output))  
* @param L7_clocktime      *timestamp          @b((output))  
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    Is only valid for copper cables
*
* @end
*********************************************************************/
L7_RC_t statusCableGet(L7_uint32               intIfNum,
                       L7_CABLE_STATUS_t      *cableStatus,
                       L7_BOOL                *lengthKnown,
                       L7_uint32              *shortestLength,
                       L7_uint32              *longestLength,
                       L7_uint32              *cableFailureLength,
                       L7_clocktime           *timestamp);

/*********************************************************************
* @purpose  Returns the result of the latest test, if any.
* 
* @param L7_uint32 intIfNum @b((input))  
* @param L7_CABLE_STATUS_t *cableStatus        @b((output))  
* @param L7_BOOL   *lengthKnown @b((output))  
* @param L7_uint32 *shortestLength @b((output))  
* @param L7_uint32 *longestLength @b((output))  
* @param L7_uint32 *cableFailureLength @b((output))  
* @param L7_clocktime      *timestamp          @b((output))  
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
* @returns  L7_NOT_EXIST  if not copper
*
* @notes    Is only valid for copper cables
*           If the link is not tested, then all other outputs are undefined
*
* @end
*********************************************************************/
L7_RC_t cableStatusCopperResult( L7_uint32               intIfNum,
                                 L7_CABLE_STATUS_t      *cableStatus,
                                 L7_BOOL                *lengthKnown,
                                 L7_uint32              *shortestLength,
                                 L7_uint32              *longestLength,
                                 L7_uint32              *cableFailureLength,
                                 L7_clocktime           *timestamp);

/*********************************************************************
* @purpose  Tests the Fiber cable and returns the result
*
* @param    L7_uint32     intIfNum    @b((input))  Internal interface number
* @param    L7_uint32     temperature @b{(output)} degrees C
* @param    L7_uint32     voltage     @b{(output)} milliVolts
* @param    L7_uint32     current     @b{(output)} milliAmps
* @param    L7_uint32     powerOut    @b{(output)} microWatts
* @param    L7_uint32     powerIn     @b{(output)} microWatts
* @param    L7_uint32     txFault     @b((output)) true or false
* @param    L7_uint32     LOS         @b((output)) true or false
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist or is not fiber
* @returns  L7_FAILURE  if other failure
*
* @notes    Is only valid for Fiber cables
*           If the link is not tested, then all other outputs are undefined
*
* @end
*********************************************************************/
L7_RC_t cableStatusFiberTest( L7_uint32  intIfNum,
                              L7_int32  *temperature,
                              L7_uint32 *voltage,
                              L7_uint32 *current,
                              L7_uint32 *powerOut,
                              L7_uint32 *powerIn,
                              L7_BOOL   *txFault,
                              L7_BOOL   *los );

/*********************************************************************
* @purpose  Tests the Fiber cable and returns the result
*
* @param    L7_uint32     intIfNum    @b((input))  Internal interface number
* @param    L7_uint32     temperature @b{(output)} degrees C
* @param    L7_uint32     voltage     @b{(output)} milliVolts
* @param    L7_uint32     current     @b{(output)} milliAmps
* @param    L7_uint32     powerOut    @b{(output)} microWatts
* @param    L7_uint32     powerIn     @b{(output)} microWatts
* @param    L7_uint32     txFault     @b((output)) true or false
* @param    L7_uint32     LOS         @b((output)) true or false
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist or is not copper
* @returns  L7_FAILURE  if other failure
*
* @notes    Is only valid for copper cables
*           If the link is not tested, then all other outputs are undefined
*
* @end
*********************************************************************/
L7_RC_t cableStatusFiberResult( L7_uint32  intIfNum,
                                L7_int32 *temperature,
                                L7_uint32 *voltage,
                                L7_uint32 *current,
                                L7_uint32 *powerOut,
                                L7_uint32 *powerIn,
                                L7_BOOL   *txFault,
                                L7_BOOL   *los );


#endif /* STATUS_H */
