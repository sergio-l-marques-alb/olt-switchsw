/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename wio_outcalls.h
*
* @purpose   Captive portal wired interface owner calls to components that may
* or may not be part of the build. Hard to imagine the wired interface owner
* would be in the build when CP is not, but there you are.
*
* @component captive portal wired interface owner
*
* @comments none
*
* @create 2/25/2008
*
* @author  rrice
* @end
*
**********************************************************************/


#ifndef _WIO_OUTCALLS_H_
#define _WIO_OUTCALLS_H_

#include "l7_common.h"

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
                         L7_uint32 clientIpv4Addr);

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
L7_uint32 wioAuthServerIntfGet(L7_uint32 authServerAddr);

#endif  /* _WIO_OUTCALLS_H_ */

