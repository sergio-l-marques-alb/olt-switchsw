
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* Name: dtl_bw.c
*
* Purpose: This file contains the functions to setup bandwidth    
*          management calls to the driver.
*
* Component: Device Transformation Layer (DTL)
*
* Commnets: 
*
* Created by: djohnson 1/14/2002 
*
*********************************************************************/
/*********************************************************************
 *                    
 ********************************************************************/



#ifndef INCLUDE_DTLCTRL_L2_DOT1Q_H
#define INCLUDE_DTLCTRL_L2_DOT1Q_H


/*********************************************************************
* @purpose  Create a new traffic class
*
* @param    tcID    @b{(input)} Traffic class ID
* @param    intf    @b{(input)} interface to manager
* @param    vlanID  @b{(input)} VLAN ID (1..4095)
* @param    minBW   @b{(input)} Min guaranteed in Mb/s (0..8000)
* @param    maxBW   @b{(input)} Max datarate in Mb/s (0..8000)
* @param    weight  @b{(input)} Weight (0..7) 
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dtlBwTcCreate(L7_uint32 tcID, L7_uint32 intf,
                      L7_uint32 vlanID, L7_uint32 minBW, L7_uint32 maxBW,
                      L7_uint32 weight);

/*********************************************************************
* @purpose  Delete a traffic class
*
* @param    tcID    @b{(input)} Traffic class ID
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dtlBwTcDelete(L7_ushort16 tcID);


/*********************************************************************
* @purpose  Modify a traffic class
*
* @param    tcID    @b{(input)} Traffic class ID
* @param    intf    @b{(input)} interface to manager
* @param    vlanID  @b{(input)} VLAN ID (1..4095)
* @param    minBW   @b{(input)} Min guaranteed in Mb/s (0..8000)
* @param    maxBW   @b{(input)} Max datarate in Mb/s (0..8000)
* @param    weight  @b{(input)} Weight (0..7) 
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dtlBwTcModify(L7_uint32 tcID, L7_uint32 intf,                    
                      L7_uint32 vlanID, L7_uint32 minBW, L7_uint32 maxBW,
                      L7_uint32 weight);
#endif
