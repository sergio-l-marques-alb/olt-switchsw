/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename wio_outcalls.c
*
* @purpose Captive portal wired interface owner calls to components that may
* or may not be part of the build. Hard to imagine the wired interface owner
* would be in the build when CP is not, but there you are.
*
* @component captive portal wired interface owner
*
* @comments none
*
* @create 2/25/2008
*
* @author rrice
* @end
*
**********************************************************************/

#include "l7_common.h"
#include "intf_cb_api.h"

#ifdef L7_CAPTIVE_PORTAL_PACKAGE
#include "cpcm_api.h"
#endif 

#ifdef L7_ROUTING_PACKAGE
#include "l7_ip_api.h"
#endif


/*********************************************************************
*
* @purpose  Send a client authentication request to Captive Portal.
*
* @param    intIfNum     @b{(input)} internal interface number of interface where
*                                    packets from client arrive
* @param    clientMac    @b{(input)} client's MAC addr
* @param    clientIpAddr @b{(input)} client's IPv4 addr
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wioClientAuthReq(L7_uint32 intIfNum,
                         L7_enetMacAddr_t *clientMac,
                         L7_uint32 clientIpv4Addr)
{
#ifdef L7_CAPTIVE_PORTAL_PACKAGE
  
  /* assume IPv4 for now */
  cpRedirIpAddr_t clientIpAddr;
  clientIpAddr.redirIpAddrType = CP_IPV4;
  clientIpAddr.ipAddr.redirIpv4Addr = clientIpv4Addr;
  return cpcmPrepareToAuthenticate_NIMWrapper(intIfNum, *clientMac, clientIpAddr);

#else

  return L7_FAILURE;

#endif
}

/*********************************************************************
*
* @purpose  Ask IP MAP for the interface where the auth server address is configured.
*
* @param    authServerAddr @b{(input)} IPv4 address used by authentication server
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_uint32 wioAuthServerIntfGet(L7_uint32 authServerAddr)
{
#ifdef L7_ROUTING_PACKAGE
  L7_uint32 intIfNum;

  if (ipMapRouterIfResolve(authServerAddr, &intIfNum) == L7_SUCCESS)
    return intIfNum;
  else
    return 0;
#else
  return 0;
#endif
}



