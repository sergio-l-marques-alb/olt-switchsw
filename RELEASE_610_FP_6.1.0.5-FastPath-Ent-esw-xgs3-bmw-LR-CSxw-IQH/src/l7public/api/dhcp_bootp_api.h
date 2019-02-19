/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename    dhcp_bootp_api.h
*
* @purpose     File contains all the APIs implimentation
*
* @component 
*
* @comments 
*
* @create      8/10/2000
*
* @author      bmutz
* @end
*
**********************************************************************/
#ifndef _DHCP_BOOTP_API_H_
#define _DHCP_BOOTP_API_H_

/**************************************************************************
* @purpose  Release (and set state to DHCPSTATE_FAILED)for all interfaces
* @param    void
*      
* @comments None.
*
* @end
*************************************************************************/
extern void l7_dhcp_release( void );

/*********************************************************************
* @purpose  Re-init bootp protocol.
*
* @param    none
*
* @returns  void
*
* @notes    none 
*       
* @end
*********************************************************************/
extern void bootpTaskReInit();

#endif
