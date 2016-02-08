/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
*
* @filename rlim_api.h
*
* @purpose Contains LVL7 standard rlim offerings
*
* @component 
*
* @comments 
*
* @create 02/16/2005
*
* @author eberge
* @end
*
**********************************************************************/

#ifndef INCLUDE_RLIM_API_H
#define INCLUDE_RLIM_API_H

#include "usmdb_rlim_api.h"


/*********************************************************************
 * RLIM interface name prefices and max length
*********************************************************************/
#define RLIM_INTF_LOOPBACK_NAME_PREFIX  "loopback"
#define RLIM_INTF_TUNNEL_NAME_PREFIX    "tunnel"

#define RLIM_INTF_MAX_NAME_LENGTH       32


/*********************************************************************
* @purpose  Create a loopback interface
*
* @param    loopbackId
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
extern L7_RC_t rlimLoopbackCreate(L7_uint32 loopbackId);

/*********************************************************************
* @purpose  Delete a loopback interface
*
* @param    intIfNum
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
extern L7_RC_t rlimLoopbackDelete(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Convert an internal interface number to a loopbackId
*
* @param    intIfNum     internal interface number
* @param    pLoopbackId  pointer to Loopback ID return value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*********************************************************************/
extern L7_RC_t rlimLoopbackIdGet(L7_uint32 intIfNum, L7_uint32 *pLoopbackId);

/*********************************************************************
* @purpose  Convert a loopbackId to an internal interface number
*
* @param    loopbackId   Loopback ID
* @param    pIntIfNum    return value for internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*********************************************************************/
extern L7_RC_t rlimLoopbackIntIfNumGet(L7_uint32 loopbackId,
                                       L7_uint32 *pIntIfNum);

/*********************************************************************
*
* @purpose Get the first valid loopback Id.
*
* @param pLoopbackId   return value for the loopbackId
*
* @returns L7_SUCCESS  If able to find a valid loopback
* @returns L7_FAILURE  If no valid loopbacks
*
* @notes none
*       
* @end
*
*********************************************************************/
extern L7_RC_t rlimLoopbackIdFirstGet(L7_uint32 *pLoopbackId);

/*********************************************************************
*
* @purpose Get the next valid loopback Id.
*
* @param loopbackId       loopbackId to start after
* @param pNextLoopbackId  return value for the next loopbackId
*
* @returns L7_SUCCESS  If able to find a succeeding valid loopback
* @returns L7_FAILURE  If no succeeding valid loopbacks
*
* @notes none
*       
* @end
*
*********************************************************************/
extern L7_RC_t rlimLoopbackIdNextGet(L7_uint32 loopbackId,
                                     L7_uint32 *pNextLoopbackId);

/*********************************************************************
* @purpose  Create a tunnel interface
*
* @param    tunnelId
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
extern L7_RC_t rlimTunnelCreate(L7_uint32 tunnelId);

/*********************************************************************
* @purpose  Delete a tunnel interface
*
* @param    intIfNum
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
extern L7_RC_t rlimTunnelDelete(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Convert an internal interface number to a tunnelId
*
* @param    intIfNum     internal interface number
* @param    pTunnelId    pointer to Tunnel ID return value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*********************************************************************/
extern L7_RC_t rlimTunnelIdGet(L7_uint32 intIfNum, L7_uint32 *pTunnelId);

/*********************************************************************
* @purpose  Convert a tunnelId to an internal interface number
*
* @param    tunnelId     Tunnel ID
* @param    pIntIfNum    return value for internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*********************************************************************/
extern L7_RC_t rlimTunnelIntIfNumGet(L7_uint32 tunnelId, L7_uint32 *pIntIfNum);

/*********************************************************************
* @purpose Set the tunnel mode of a tunnel.
*
* @param intIfNum       internal interface identifier
* @param tunnelMode     Tunnel ID of the desired tunnel interface
*
* @returns L7_SUCCESS  If able to find tunnel
* @returns L7_FAILURE  If tunnel does not exist
*
* @notes none
*       
* @end
*
*********************************************************************/
extern L7_RC_t rlimTunnelModeSet(L7_uint32 intIfNum,
                                 L7_TUNNEL_MODE_t tunnelMode);

/*********************************************************************
*
* @purpose Set the local/source Ip4 address of a tunnel.
*
* @param intIfNum     internal interface identifier
* @param addr         address
*
* @returns L7_SUCCESS  If able to find tunnel
* @returns L7_FAILURE  If tunnel does not exist
*
* @notes none
*       
* @end
*
*********************************************************************/
extern L7_RC_t rlimTunnelLocalIp4AddrSet(L7_uint32 intIfNum, L7_uint32 addr);

/*********************************************************************
*
* @purpose Set the local/source Ip6 address of a tunnel.
*
* @param intIfNum     internal interface identifier
* @param addr         address
*
* @returns L7_SUCCESS  If able to find tunnel
* @returns L7_FAILURE  If tunnel does not exist
*
* @notes none
*       
* @end
*
*********************************************************************/
extern L7_RC_t rlimTunnelLocalIp6AddrSet(L7_uint32 intIfNum,
                                         L7_in6_addr_t *addr);

/*********************************************************************
*
* @purpose Set the interface to get the local address from
*
* @param intIfNum       internal interface identifier of the tunnel
* @param localIntIfNum  internal interface identifier of the interface
*                       to get the local address from
*
* @returns L7_SUCCESS  If able to find tunnel
* @returns L7_FAILURE  If tunnel does not exist
*
* @notes none
*       
* @end
*
*********************************************************************/
extern L7_RC_t rlimTunnelLocalIntfSet(L7_uint32 intIfNum,
                                      L7_uint32 localIntIfNum);

/*********************************************************************
*
* @purpose Clear the local/source address of a tunnel.
*
* @param intIfNum     internal interface identifier
*
* @returns L7_SUCCESS  If able to find tunnel
* @returns L7_FAILURE  If tunnel does not exist
*
* @notes none
*       
* @end
*
*********************************************************************/
extern L7_RC_t rlimTunnelLocalAddrClear(L7_uint32 intIfNum);

/*********************************************************************
*
* @purpose Set the remote/destination Ip4 address of a tunnel.
*
* @param intIfNum     internal interface identifier
* @param addr         address
*
* @returns L7_SUCCESS  If able to find tunnel
* @returns L7_FAILURE  If tunnel does not exist
*
* @notes none
*       
* @end
*
*********************************************************************/
extern L7_RC_t rlimTunnelRemoteIp4AddrSet(L7_uint32 intIfNum, L7_uint32 addr);

/*********************************************************************
*
* @purpose Set the remote/destination Ip6 address of a tunnel.
*
* @param intIfNum     internal interface identifier
* @param addr         address
*
* @returns L7_SUCCESS  If able to find tunnel
* @returns L7_FAILURE  If tunnel does not exist
*
* @notes none
*       
* @end
*
*********************************************************************/
extern L7_RC_t rlimTunnelRemoteIp6AddrSet(L7_uint32 intIfNum,
                                          L7_in6_addr_t *addr);

/*********************************************************************
*
* @purpose Clear the remote/destination address of a tunnel.
*
* @param intIfNum     internal interface identifier
*
* @returns L7_SUCCESS  If able to find tunnel
* @returns L7_FAILURE  If tunnel does not exist
*
* @notes none
*       
* @end
*
*********************************************************************/
extern L7_RC_t rlimTunnelRemoteAddrClear(L7_uint32 intIfNum);

/*********************************************************************
*
* @purpose Get the tunnel mode of a tunnel.
*
* @param intIfNum       internal interface identifier
* @param pTunnelMode    return value for tunnel mode
*
* @returns L7_SUCCESS  If able to find tunnel
* @returns L7_FAILURE  If tunnel does not exist
*
* @notes none
*       
* @end
*
*********************************************************************/
extern L7_RC_t rlimTunnelModeGet(L7_uint32 intIfNum,
                                 L7_TUNNEL_MODE_t *pTunnelMode);

/*********************************************************************
*
* @purpose Get the active tunnel mode of a tunnel.
*
* @param intIfNum       internal interface identifier
* @param pTunnelMode    return value for tunnel mode
*
* @returns L7_SUCCESS  If able to find tunnel
* @returns L7_FAILURE  If tunnel does not exist
*
* @notes this must remain constant during life of tunnel created in stack
*
* @end
*
*********************************************************************/
extern L7_RC_t rlimTunnelActiveModeGet(L7_uint32 intIfNum, L7_TUNNEL_MODE_t *pTunnelMode);

/*********************************************************************
*
* @purpose Get the configured local address type of a tunnel.
*
* @param intIfNum       internal interface identifier
* @param pAddrType      return value for address type
*
* @returns L7_SUCCESS  If able to find tunnel
* @returns L7_FAILURE  If tunnel does not exist
*
* @notes Use this interface to see how the tunnel address type is
*        configured.  If, instead, you wish to we the operationally
*        active address type (IP4 or IP6), use rlimTunnelLocalAddrTypeGet.
*        This is relevant in the case where the configured address type
*        is RLIM_ADDRTYPE_INTERFACE in which case this interfaces will
*        return RLIM_ADDRTYPE_INTERFACE, whereas rlimTunnelLocalAddrTypeGet
*        returns RLIM_ADDRTYPE_IP4 or RLIM_ADDRTYPE_IP6 depending on
*        the tunnel mode and valid addresses on the source interface.
*       
* @end
*
*********************************************************************/
extern L7_RC_t rlimTunnelCfgLocalAddrTypeGet(L7_uint32 intIfNum,
                                             rlimAddrType_t *pAddrType);

/*********************************************************************
*
* @purpose Get the type of the local address of a tunnel.
*
* @param intIfNum       internal interface identifier
* @param pAddrType      return value for address type
*
* @returns L7_SUCCESS  If able to find tunnel
* @returns L7_FAILURE  If tunnel does not exist
*
* @notes Returns the type of interface used operationally on the
*        tunnel.  This is the same as the configured value
*        unless the type is RLIM_ADDRTYPE_INTERFACE, in which case
*        the actually protocol (IP4 or IP6) is returned if the
*        address is operationally available.
*       
* @end
*
*********************************************************************/
extern L7_RC_t rlimTunnelLocalAddrTypeGet(L7_uint32 intIfNum,
                                          rlimAddrType_t *pAddrType);

/*********************************************************************
*
* @purpose Get the type of the remote address of a tunnel.
*
* @param intIfNum       internal interface identifier
* @param pAddrType      return value for address type
*
* @returns L7_SUCCESS  If able to find tunnel
* @returns L7_FAILURE  If tunnel does not exist
*
* @notes none
*       
* @end
*
*********************************************************************/
extern L7_RC_t rlimTunnelRemoteAddrTypeGet(L7_uint32 intIfNum,
                                           rlimAddrType_t *pAddrType);

/*********************************************************************
*
* @purpose Get the local/source Ip4 address of a tunnel.
*
* @param intIfNum     internal interface identifier
* @param addr         return value for address
*
* @returns L7_SUCCESS  If able to find tunnel and the source address
*                      is an Ip4 address
* @returns L7_FAILURE  Otherwise
*
* @notes none
*       
* @end
*
*********************************************************************/
extern L7_RC_t rlimTunnelLocalIp4AddrGet(L7_uint32 intIfNum, L7_uint32 *addr);

/*********************************************************************
*
* @purpose Get the local/source Ip6 address of a tunnel.
*
* @param intIfNum     internal interface identifier
* @param addr         return value for address
*
* @returns L7_SUCCESS  If able to find tunnel and the source address
*                      is an Ip4 address
* @returns L7_FAILURE  Otherwise
*
* @notes none
*       
* @end
*
*********************************************************************/
extern L7_RC_t rlimTunnelLocalIp6AddrGet(L7_uint32 intIfNum,
                                         L7_in6_addr_t *addr);

/*********************************************************************
*
* @purpose Get the local/source interface of a tunnel.
*
* @param intIfNum     internal interface identifier
* @param pIntIfNum    return value for local interface of the tunnel
*
* @returns L7_SUCCESS  If able to find tunnel and the source address
*                      is specified by a valid interface
* @returns L7_FAILURE  Otherwise
*
* @notes none
*       
* @end
*
*********************************************************************/
extern L7_RC_t rlimTunnelLocalIntfGet(L7_uint32 intIfNum,
                                      L7_uint32 *pIntfIfNum);

/*********************************************************************
*
* @purpose Get the remote/destination Ip4 address of a tunnel.
*
* @param intIfNum     internal interface identifier
* @param addr         return value for address
*
* @returns L7_SUCCESS  If able to find tunnel and the remote address
*                      is an Ip4 address
* @returns L7_FAILURE  If tunnel does not exist
*
* @notes none
*       
* @end
*
*********************************************************************/
extern L7_RC_t rlimTunnelRemoteIp4AddrGet(L7_uint32 intIfNum, L7_uint32 *addr);

/*********************************************************************
*
* @purpose Get the remote/destination Ip6 address of a tunnel.
*
* @param intIfNum     internal interface identifier
* @param addr         return value for address
*
* @returns L7_SUCCESS  If able to find tunnel and the remote address
*                      is an Ip6 address
* @returns L7_FAILURE  If tunnel does not exist
*
* @notes none
*       
* @end
*
*********************************************************************/
extern L7_RC_t rlimTunnelRemoteIp6AddrGet(L7_uint32 intIfNum,
                                          L7_in6_addr_t *addr);

/*********************************************************************
*
* @purpose Get the hop limit for the given tunnel
*
* @param intIfNum       internal interface identifier
* @param pHopLimit      return value for the hopLimit
*
* @returns L7_SUCCESS  If able to find tunnel
* @returns L7_FAILURE  If tunnel does not exist
*
* @notes none
*       
* @end
*
*********************************************************************/
extern L7_RC_t rlimTunnelHopLimitGet(L7_uint32 intIfNum, L7_uint32 *pHopLimit);

/*********************************************************************
*
* @purpose Get the security value for the given tunnel
*
* @param intIfNum       internal interface identifier
* @param pSecurity      return value for the security value
*
* @returns L7_SUCCESS  If able to find tunnel
* @returns L7_FAILURE  If tunnel does not exist
*
* @notes none
*       
* @end
*
*********************************************************************/
extern L7_RC_t rlimTunnelSecurityGet(L7_uint32 intIfNum, L7_uint32 *pSecurity);

/*********************************************************************
*
* @purpose Get the TOS value for the given tunnel
*
* @param intIfNum       internal interface identifier
* @param pTOS           return value for the TOS value
*
* @returns L7_SUCCESS  If able to find tunnel
* @returns L7_FAILURE  If tunnel does not exist
*
* @notes none
*       
* @end
*
*********************************************************************/
extern L7_RC_t rlimTunnelTOSGet(L7_uint32 intIfNum, L7_int32 *pTOS);

/*********************************************************************
*
* @purpose Get the flow label for the given tunnel
*
* @param intIfNum       internal interface identifier
* @param pFlowLabel     return value for the flow label
*
* @returns L7_SUCCESS  If able to find tunnel
* @returns L7_FAILURE  If tunnel does not exist
*
* @notes none
*       
* @end
*
*********************************************************************/
extern L7_RC_t rlimTunnelFlowLabelGet(L7_uint32 intIfNum,
                                      L7_int32 *pFlowLabel);

/*********************************************************************
*
* @purpose Get the encapsulation limit for the given tunnel
*
* @param intIfNum       internal interface identifier
* @param pEncapsLimit   return value for the encapsulation limit
*
* @returns L7_SUCCESS  If able to find tunnel
* @returns L7_FAILURE  If tunnel does not exist
*
* @notes none
*       
* @end
*
*********************************************************************/
extern L7_RC_t rlimTunnelEncapsLimitGet(L7_uint32 intIfNum,
                                        L7_int32 *pEncapsLimit);

/*********************************************************************
*
* @purpose Get the first valid tunnel Id.
*
* @param pTunnelId      return value for the tunnelId
*
* @returns L7_SUCCESS  If able to find a valid tunnel
* @returns L7_FAILURE  If no valid tunnels
*
* @notes none
*       
* @end
*
*********************************************************************/
extern L7_RC_t rlimTunnelIdFirstGet(L7_uint32 *pTunnelId);

/*********************************************************************
*
* @purpose Get the next valid tunnel Id.
*
* @param tunnelId       tunnelId to start after
* @param pNextTunnelId  return value for the next tunnelId
*
* @returns L7_SUCCESS  If able to find a succeeding valid tunnel
* @returns L7_FAILURE  If no succeeding valid tunnels
*
* @notes none
*       
* @end
*
*********************************************************************/
extern L7_RC_t rlimTunnelIdNextGet(L7_uint32 tunnelId,
                                   L7_uint32 *pNextTunnelId);

/*********************************************************************
* @purpose notification of v6 neighbor on 6to4 network. If possible, create
*          a 48 bit route.
*         
* @param   intIfNum        @b{(input)} The tunnel internal interface
* @param   remote          @b{(input)} remote tunnel endpoint ipv4 addr
* @param   enable          @b{(input)} if enable, route is added, else deleted
*
*
*
* @returns  L7_SUCCESS  
*           L7_FAILURE
* @notes    none
*       
* @end
*********************************************************************/
extern L7_RC_t  rlimTunnel6to4NbrNotify(L7_uint32 intIfNum, L7_in6_addr_t *nbr, L7_uint32 enable);

#endif /* !defined(INCLUDE_RLIM_API_H) */
