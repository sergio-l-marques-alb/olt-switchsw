/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename  broad_bcmx.c
*
* @purpose   Custom BCMX functions added to support Fastpath.
*
* @component hapi
*
* @comments
*
* @create    9/16/2004
*
* @author    Andrey Tsigler
*
* @end
*
**********************************************************************/
#include "l7_common.h"

#include "bcm/types.h"
#include "bcmx/custom.h"

#include "l7_rpc_policy.h"
#include "l7_rpc_port.h"

#ifdef L7_MCAST_PACKAGE
#include "l7_rpc_ipmcast.h"
#endif

/*********************************************************************
*
* @purpose Dispatch function to handle custom BCMX port requests.
*
* @param
*
* @returns BCM Error Code
*
* @notes   
*
*
* @end
*
*********************************************************************/
/* PTin modified: SDK 6.3.0 */
#if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
int custom_bcmx_port_handler(int unit, bcm_port_t port, int setget, int type,
                             int length, uint32 *args, int *actual_length, void *user_data)
#else
int custom_bcmx_port_handler(int unit, bcm_port_t port, int setget, int type, uint32 *args)
#endif
{
  int     rv = BCM_E_NOT_FOUND;
  L7_BOOL handled = L7_FALSE;

  
  handled = l7_rpc_server_port_handler(unit, port, setget, type, args, &rv);
  if (handled == L7_TRUE)
  {
    return rv;      
  }

#ifdef L7_MCAST_PACKAGE
  handled = l7_rpc_server_ipmc_handler(unit, port, setget, type, args, &rv);
  if (handled == L7_TRUE)
  {
    return rv;      
  }
#endif

  handled = l7_rpc_server_port_policy_handler(unit, port, setget, type, args, &rv);
  if (handled == L7_TRUE)
  {
    return rv;      
  }

  return rv;
}

