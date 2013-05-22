/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename    pimsm_vend_ctrl.h
*
* @purpose     PIM-SM vendor-specific internal definitions
*
* @component   PIM-SM Mapping Layer
*
* @comments    none
*
* @create      03/08/2002
*
* @author      Ratnakar
*
* @end
*
**********************************************************************/

#ifndef _PIMSM_VEND_CTRL_H_
#define _PIMSM_VEND_CTRL_H_

#include "l3_comm_structs.h"
#include "l3_mcast_commdefs.h"
/*********************************************************************
* @purpose  To get the vendor CB handle based on family Type
*
* @param    familyType   @b{(input)}   Address Family type.
*
* @returns  cbHandle     return cbHandle, 
* @returns  L7_NULLPTR   on failure
*
* @comments none
*
* @end
*********************************************************************/
MCAST_CB_HNDL_t pimsmMapProtocolCtrlBlockGet(L7_uchar8 familyType);


/*********************************************************************
* @purpose  To send the event to Message Queue
*
* @param    cbHandle    @b{(input)} pimdm CB Handle.
* @param    eventyType  @b{(input)} Event Type
* @param    msgLen      @b{(input)} Message Length.
* @param    pMsg        @b{(input)} Message
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE 
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapMessageQueueSend(L7_uchar8 familyType, L7_uint32 eventType,
                                 L7_uint32 msgLen, L7_VOIDPTR pMsg);

/*********************************************************************
*
* @purpose  RTO Best Route Callback for IPv4
*
* @param    none
*                            
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments none
*
* @end
*********************************************************************/

void
pimsmMapRto4BestRouteClientCallback (void);
/*********************************************************************
*
* @purpose  RTO Best Route Callback for IPv6
*
* @param    none
*                            
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments none
*
* @end
*********************************************************************/

void
pimsmMapRto6BestRouteClientCallback (void);



#endif /* _PIMSM_VEND_CTRL_H_ */

