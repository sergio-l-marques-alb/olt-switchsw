/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename   ping6_map_api.h
*
* @component  ping6
*
* @comments   Header file for mapping tping6 function.
*
* @create  05/21/2004
*
* @author  jpp
* @end
*
**********************************************************************/

/*************************************************************

*************************************************************/

#ifndef PING6_MAP_API_H
#define PING6_MAP_API_H



/*********************************************************************
* @purpose  ipv6 ping interface
*
* Parameters:   hostname:       ascii dns name or ascii dst address
*               daddr:          in- if !hostname, dst address
*                               out- dst address
*               ifindex:        interface index for link local dst
*               saddr:          out- responder's src address (usually daddr)
*               timeout:        in - request timeout in msec
*                               out - actual time
*               req_size        ping length
*               intIfName       interface name (used only when intIfNum is 0)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t L7_ping6( L7_in6_addr_t *daddr, int intIfNum,
             L7_in6_addr_t *saddr, L7_uint32 *timeout,
             L7_uint32 req_size, L7_char8 *intIfName);


/*********************************************************************
* @purpose  send single request, used to force NbrDisc and called 
*           indirectly by user ping.
*
 * Parameters:   sock            socket descriptor 
 * Parameters:   to              pointer to v6 sockaddr
 * Parameters:   id              ping identifier (sequence number)
 * Parameters:   dlen            data length   
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ping6SendRequest(int sock, L7_sockaddr_in6_t *to,
                              L7_uint32 id, int dlen);


#endif
