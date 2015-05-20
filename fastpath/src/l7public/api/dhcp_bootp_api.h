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


/*********************************************************************
* @purpose  If the network port has leased a DHCP address, send a DHCP 
*           release message on the network port.
*
* @param   void
*
* @returns void
*
* @comments
*                                 
* @end
*
*********************************************************************/
extern void dhcpc_netport_release(void);

/*********************************************************************
* @purpose  If the service port has leased a DHCP address, send a DHCP 
*           release message on the service port.
*
* @param   void
*
* @returns void
*
* @comments
*                                 
* @end
*
*********************************************************************/
extern void dhcpc_svcport_release(void);

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
