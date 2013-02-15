/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename   traceroute6_map_api.h
*
* @component  Traceroute6
*
* @comments   Header file for mapping traceroute6 function.
*
* @create  05/21/2004
*
* @author  jpp
* @end
*
**********************************************************************/

/*************************************************************

*************************************************************/

#ifndef TRACEROUTE6_MAP_API_H
#define TRACEROUTE6_MAP_API_H




/*********************************************************************
* @purpose  To trace the route to a destination on a hop-by-hop basis
*
* @param    destinationIP   IP Address of the destination, to which the route is sought
* @param    port            Port number to which the udp probe packets will be sent
* @param    hop             hop count
* @param    *buf            output buffer, which will have the route route information
* @param    *tracertReply   Output buffer for storing the reply time
* @param    *status         Flag to indicate that the destination has been reached or not
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t L7_traceroute6(L7_in6_addr_t *destinationIP, L7_uint32 port, L7_uint32 hop,
                      L7_char8 *buf, tracertReply_t *tracertReply, L7_BOOL *status);

#endif
