/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
* @filename  ip6map_binds.c
*
* @purpose   stubs for packages that do not support ipv6
*
* @component ip6 Mapping Layer
*
* @comments  none
*
* @create    
*
* @author    eberge
*
* @end
*
**********************************************************************/


#include "l7_common.h"
#include "l3_addrdefs.h"


/*********************************************************************
* @purpose  Get the administrative mode of the router
*
* @param    void
*
* @returns  mode     @b{(output)} L7_ENABLE or L7_DISABLE
*
* @notes   This routine returns the user-configured state of the router.
*
*
* @end
*********************************************************************/
L7_uint32  ip6MapRtrAdminModeGet(void)
{
  return L7_DISABLE;
}

/*********************************************************************
* @purpose  Get the administrative state of an interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ip6MapRtrIntfModeGet( L7_uint32 intIfNum, L7_uint32 *mode)
{
  *mode = L7_DISABLE;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Enable or disable interface participation in the router
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    mode     @b{(input)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ip6MapRtrIntfModeSet( L7_uint32 intIfNum, L7_uint32 mode)
{
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get configured addresses of interface
*
* @param    rtrIntf      router interface
*           numAddr      number of addresses: in max, out actual
*           pAddr        ptr to array of addresses
*           pPlen        ptr to array of prefix lens
*           llena_flag   ptr to link_local_enable flag
*           pEui_flag    ptr to eui flags
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
*
* @end
*********************************************************************/
L7_RC_t ip6MapRtrCfgIntfAddressesGet(L7_uint32 intIfNum, L7_uint32 *numAddr,
                                     L7_in6_addr_t *pAddr, L7_uint32 *pPlen,
                                     L7_uint32 *llena_flag,
                                     L7_uint32 *pEui_flag)
{
  *numAddr = 0;
  *llena_flag = 0;
  return L7_SUCCESS;
}
