/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
* @filename  l7freeBSD_api.c
*
* @purpose   Free BSD API functions
*
* @component IP Mapping Layer
*
* @comments  none
*
* @create    09/19/2001
*
* @author    skanchi
*
* @end
*             
**********************************************************************/

/*************************************************************
                    
*************************************************************/



#include "l7_common.h"
#include "rtip.h"
#include "rtip_var.h"
#include "l7freebsd_api.h"

/*********************************************************************
* @purpose  Set the mode of IP forwarding in free BSD code
*
* @param    mode    @b{(input)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t freeBSDIPForwardingSet(L7_uint32 mode)
{  
  rtipforwarding = mode;
  return L7_SUCCESS;
}
