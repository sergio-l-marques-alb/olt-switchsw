/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2009
*
**********************************************************************
*
* @filename  dtl_helper.h
*
* @purpose   Technology specific functions and data for the hardware
*            platform control component
*
* @component
*
* @create    02/04/2009
*
* @author    colinw
* @end
*
*********************************************************************/
#ifndef INCLUDE_DTL_HELPER_H
#define INCLUDE_DTL_HELPER_H

/*********************************************************************
* @purpose  Routine for application helpers to send packets on the local
*           unit.
*          
* @param    frame              @b{(input)} 
* @param    frameSize          @b{(input)} 
* @param    priority           @b{(input)} 
* @param    slot               @b{(input)} 
* @param    port               @b{(input)} 
* @param    ignoreEgressRules  @b{(input)} 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t dtlHelperSend(L7_uchar8   *frame,
			          L7_uint32    frameSize,
			          L7_uint8     priority,
			          L7_uint8     slot,
			          L7_ushort16  port,
			          L7_BOOL      ignoreEgressRules);

#endif
