/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename l7netapi.h
*
* @purpose DHCP and Bootp API Functions
*
* @component bootp and DHCP
*
* @comments none
*
* @create 06/21/2001
*
* @author bmutz
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/

#ifndef INCLUDE_L7NETAPI_H
#define INCLUDE_L7NETAPI_H

#include "datatypes.h"
#include "commdefs.h"
#include "comm_structs.h"

/*********************************************************************
* @purpose  Notification Registration 
*
* @param    L7_uint32  component_ID   @b((input)) componant id
* @param    L7_uint32  (*notify)(L7_bootp_dhcp_t)   @b((input))  function
*           to callback
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
extern L7_RC_t dhcpNotificationRegister( L7_COMPONENT_IDS_t component_ID, 
                                         L7_uint32 requestedOptionsMask,
                                         L7_RC_t (*notifyFuncPtr)(L7_bootp_dhcp_t *networkParamsPtr));

/*********************************************************************
* @purpose Deregister the routine to be called when a DHCP request completes.
*
* @param   L7_COMPONENT_IDS_t componentId  @b((input)) component ID      
*
* @returns L7_SUCCESS
* @returns L7_FAILURE, invalid argument
*
* @comments
*                                 
* @end
*
*********************************************************************/
extern L7_RC_t dhcpNotificationDeregister(L7_COMPONENT_IDS_t componentId);

/*********************************************************************
* @purpose  Notification Registration 
*
* @param    L7_uint32  component_ID   @b((input)) componant id
* @param    L7_uint32  (*notify)(L7_bootp_dhcp_t)   @b((input))  function to callback
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t bootpNotificationRegister( L7_COMPONENT_IDS_t component_ID, L7_RC_t (*notifyFuncPtr)(L7_bootp_dhcp_t *networkParamsPtr));

/*********************************************************************
* @purpose Deregister the routine to be called when a BOOTP request completes.
*
* @param   L7_COMPONENT_IDS_t componentId  @b((input)) component ID      
*
* @returns L7_SUCCESS
* @returns L7_FAILURE, invalid argument
*
* @comments
*                                 
* @end
*
*********************************************************************/
L7_RC_t bootpNotificationDeregister(L7_COMPONENT_IDS_t componentId);

/**************************************************************************
* @purpose  Get the configuration semaphore.
*
* @comments 
*
* @end
*************************************************************************/
void dhcpConfigSemGet (void);

/**************************************************************************
* @purpose  Get the configuration semaphore.
*
* @comments 
*
* @end
*************************************************************************/
void dhcpConfigSemFree (void);

/**************************************************************************
* @purpose  Get the configuration semaphore.
*
* @comments 
*
* @end
*************************************************************************/
void bootpConfigSemGet (void);

/**************************************************************************
* @purpose  Get the configuration semaphore.
*
* @comments 
*
* @end
*************************************************************************/
void bootpConfigSemFree (void);




#endif

