/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename  vrrp_vend_ctrl.c
*
* @purpose   VRRP vendor-specific control routines
*
* @component VRRP Mapping Layer
*
* @comments  none
*
* @create    01/16/2002
*
* @author    jeffr
*
* @end
*             
**********************************************************************/

/*************************************************************
                    
*************************************************************/

#include "l7_common.h"
#include "std.h"

extern L7_RC_t vrrpGetVMac(L7_uchar8 *ipAddress, L7_uchar8 *tVMAC, L7_uchar8 *vrId);

/*********************************************************************
* @purpose  Uses utility function in VRRP to search VRRP data 
*           structures to determine if the provided IP address is
*           associated with a VRRP virtual router for which we are
*           master.
*
* @param    ipAddress  ip address
* @param    vmac       VRRP mac address
* @param    vrId       VRRP Id
*
* @returns  L7_SUCCESS  success
* @returns  L7_FALIURE  failure
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t vrrpMapGetVMac(L7_uchar8 *ipAddress, L7_uchar8 *vmac, L7_uchar8 *vrId)
{                             
   return (vrrpGetVMac(ipAddress, vmac, vrId));
}

