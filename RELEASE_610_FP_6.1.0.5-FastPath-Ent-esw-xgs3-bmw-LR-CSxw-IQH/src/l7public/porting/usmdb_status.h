/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename usmdb_status.h
*
* @purpose  Prototypes for calls to usmdb_status
*
* @component usmb
*
* @comments 
*
* @create 09/19/2002
*
* @author Jill Flanagan
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/



#ifndef USMDB_STATUS_H
#define USMDB_STATUS_H

#include <datatypes.h>
#include "osapi.h"
/*********************************************************************
*
* @purpose Test the status of the cable
*  
* @param L7_uint32 intIfNum @b((input))  
* @param L7_CABLE_STATUS_t *cableStatus        @b((output))  
* @param L7_BOOL *lengthKnown @b((output))  
* @param L7_uint32 *shortestLength @b((output))  
* @param L7_uint32 *longestLength @b((output))  
* @param L7_uint32 *cableFailureLength @b((output))  
* @param L7_clocktime      *timestamp          @b((output))  
*
* @returns L7_SUCCESS
*          L7_ERROR - the cable is not of the proper type for the test
*                      or the interface does not exist
*          L7_FAILURE
*
* @notes Only valid for copper cables
*
* @end
*********************************************************************/
extern L7_RC_t usmDbStatusCableGet(L7_uint32          intIfNum, 
                                   L7_CABLE_STATUS_t *cableStatus,
                                   L7_BOOL           *lengthKnown, 
                                   L7_uint32         *shortestLength,
						           L7_uint32         *longestLength, 
                                   L7_uint32         *cableFailureLength,
                                   L7_clocktime      *timestamp);

/*********************************************************************
* @purpose  Returns the result of the latest test, if any.
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
* @returns  L7_NOT_EXIST  if not copper
*
* @notes    Is only valid for copper cables
*           If the link is not tested, then all other outputs are undefined
*
* @end
*********************************************************************/
extern L7_RC_t usmDbCableStatusCopperResult( L7_uint32               intIfNum,
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
* @param    L7_BOOL       linkUp      @b((output)) L7_TRUE if link is up
* @param    L7_uint32     temperature @b{(output)} degrees C
* @param    L7_uint32     voltage     @b{(output)} milliVolts
* @param    L7_uint32     current     @b{(output)} milliAmps
* @param    L7_double64   powerOut    @b{(output)} db
* @param    L7_double64   powerIn     @b{(output)} db
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
L7_RC_t usmDbCableStatusFiberTest( L7_uint32  intIfNum,
                                   L7_int32  *temperature,
                                   L7_uint32 *voltage,
                                   L7_uint32 *current,
                                   L7_double64 *powerOut,
                                   L7_double64 *powerIn,
                                   L7_BOOL   *txFault,
                                   L7_BOOL   *los );

/*********************************************************************
* @purpose  Returns the result of the latest test, if any.
*
* @param    L7_uint32     intIfNum    @b((input))  Internal interface number
* @param    L7_uint32     temperature @b{(output)} degrees C
* @param    L7_uint32     voltage     @b{(output)} milliVolts
* @param    L7_uint32     current     @b{(output)} milliAmps
* @param    L7_double64   powerOut    @b{(output)} db
* @param    L7_double64   powerIn     @b{(output)} db
* @param    L7_uint32     txFault     @b((output)) true or false
* @param    L7_uint32     los         @b((output)) true or false
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
L7_RC_t usmDbCableStatusFiberResult( L7_uint32  intIfNum,
                                     L7_int32  *temperature,
                                     L7_uint32 *voltage,
                                     L7_uint32 *current,
                                     L7_double64 *powerOut,
                                     L7_double64 *powerIn,
                                     L7_BOOL   *txFault,
                                     L7_BOOL   *los );

/*********************************************************************
*
* @purpose Test the link of the SFP on a combo port
* 
* @param L7_uint32 UnitIndex @b((input))         
* @param L7_uint32 intIfNum @b((input))  
* @param L7_BOOL  *sfpLinkUp @b((output))  
*
* @returns L7_SUCCESS
*          L7_ERROR - the port is not a combo port
*                      or the interface does not exist
*          L7_FAILURE
*
* @notes 
*
* @end
*********************************************************************/
L7_RC_t usmDbStatusSFPLinkUpGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_BOOL *sfpLinkUp);

#endif /* USMDB_STATUS_H */




