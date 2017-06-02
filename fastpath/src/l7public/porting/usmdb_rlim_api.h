/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
*
* @filename usmdb_rlim_api.h
*
* @purpose externs for USMDB layer rlim APIs
*
* @component Routing Logical Interface Manager
*
* @comments none
*
* @create 02/16/2005
*
* @author eberge
* @end
*
**********************************************************************/

#ifndef USMDB_RLIM_API_H
#define USMDB_RLIM_API_H

#include "l7_common.h"
#include "l3_addrdefs.h"


/*
 * Address Type values
 */
typedef enum
{
  RLIM_ADDRTYPE_UNDEFINED = 1,
  RLIM_ADDRTYPE_IP4,
  RLIM_ADDRTYPE_IP6,
  RLIM_ADDRTYPE_INTERFACE
} rlimAddrType_t;

/*
 * Security values
 */
typedef enum
{
  RLIM_SECURITY_NONE = 1,
  RLIM_ADDRTYPE_IPSEC
} rlimSecurity_t;

/*********************************************************************
*
* @purpose Create a loopback interface
*
* @param loopbackId   loopback interface identifier
*
* @returns L7_SUCCESS  If loopback was created
* @returns L7_FAILURE  If other failure
*
* @notes none
*       
* @end
*
*********************************************************************/
extern L7_RC_t usmDbRlimLoopbackCreate(L7_uint32 loopbackId);

/*********************************************************************
*
* @purpose Delete a loopback interface
*
* @param unit       unit index
* @param intIfNum   internal interface identifier
*
* @returns L7_SUCCESS  If loopback was successfully deleted
* @returns L7_FAILURE  If other failure
*
* @notes none
*       
* @end
*
*********************************************************************/
extern L7_RC_t usmDbRlimLoopbackDelete(L7_uint32 unit, L7_uint32 intIfNum);

/*********************************************************************
*
* @purpose Return the internal interface identifier for a loopback
*
* @param loopbackId   loopback interface identifier
* @param pIntIfNum    return value for internal interface identifier
*
* @returns L7_SUCCESS  If loopback exists
* @returns L7_FAILURE  If other failure
*
* @notes none
*       
* @end
*
*********************************************************************/
extern L7_RC_t usmDbRlimLoopbackIntIfNumGet(L7_uint32 loopbackId,
                                            L7_uint32 *pIntIfNum);

/*********************************************************************
* @purpose  Convert an internal interace number to a loopbackId
*
* @param    intIfNum     internal interface number
* @param    pLoopbackId  pointer to Loopback ID return value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*********************************************************************/
extern L7_RC_t usmDbRlimLoopbackIdGet(L7_uint32 intIfNum,
                                      L7_uint32 *ploopbackId);

/*********************************************************************
 * *
 * * @purpose Return the loopback identifier for a loopback
 * *
 * * @param loopbackId  pointer to loopback identifier
 * *
 * * @returns L7_SUCCESS  If loopback exists
 * * @returns L7_FAILURE  If other failure
 * *
 * * @notes none
 * *
 * * @end
 * *
 * *********************************************************************/
extern L7_RC_t usmDbRlimLoopbackIdFirstGet(L7_uint32 *pLoopbackId);
																						

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
extern L7_RC_t usmDbRlimLoopbackIdFirstGet(L7_uint32 *pLoopbackId);

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
extern L7_RC_t usmDbRlimLoopbackIdNextGet(L7_uint32 loopbackId,
		                                   L7_uint32 *pNextLoopbackId);

/*********************************************************************
*
* @purpose Create a Tunnel identified by Tunnel ID
*
* @param tunnelId
*
* @returns L7_SUCCESS  If able to create tunnel
* @returns L7_FAILURE  If other failure
*
* @notes none
*       
* @end
*
*********************************************************************/
extern L7_RC_t usmDbRlimTunnelCreate(L7_uint32 tunnelId);

/*********************************************************************
*
* @purpose Delete a Tunnel
*
* @param unit       unit index
* @param intIfNum   internal interface identifier
*
* @returns L7_SUCCESS  If able to delete tunnel
* @returns L7_FAILURE  If other failure
*
* @notes none
*       
* @end
*
*********************************************************************/
extern L7_RC_t usmDbRlimTunnelDelete(L7_uint32 unit, L7_uint32 intIfNum);

/*********************************************************************
*
* @purpose Return the internal interface identifier of the tunnel
*          with the supplied Tunnel ID.
*
* @param tunnelId   Tunnel ID of the desired tunnel interface
* @param pIntIfNum  return value for internal interface number
*
* @returns L7_SUCCESS  If able to find tunnel
* @returns L7_FAILURE  If tunnel does not exist
*
* @notes none
*       
* @end
*
*********************************************************************/
extern L7_RC_t usmDbRlimTunnelIntIfNumGet(L7_uint32 tunnelId,
                                          L7_uint32 *pIntIfNum);


/*********************************************************************
*
* @purpose Return the Tunnel ID of the tunnel
*          with the supplied internal interface identifier.
*
* @param intIfNum  internal interface number of the Tunnel
* @param ptunnelId   return value for the Tunnel ID 
*
* @returns L7_SUCCESS  If able to find tunnel
* @returns L7_FAILURE  If tunnel does not exist
*
* @notes none
*       
* @end
*
*********************************************************************/
extern L7_RC_t usmDbRlimTunnelIdGet(L7_uint32 intIfNum, L7_uint32 *ptunnelId);

/*********************************************************************
*
* @purpose Set the tunnel mode of a tunnel.
*
* @param unit           unit index
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
extern L7_RC_t usmDbRlimTunnelModeSet(L7_uint32 unit, L7_uint32 intIfNum,
                                      L7_TUNNEL_MODE_t tunnelMode);

/*********************************************************************
*
* @purpose Set the local/source Ip4 address of a tunnel.
*
* @param unit         unit index
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
extern L7_RC_t usmDbRlimTunnelLocalIp4AddrSet(L7_uint32 unit,
                                              L7_uint32 intIfNum,
                                              L7_uint32 addr);

/*********************************************************************
*
* @purpose Set the local/source Ip6 address of a tunnel.
*
* @param unit         unit index
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
extern L7_RC_t usmDbRlimTunnelLocalIp6AddrSet(L7_uint32 unit,
                                              L7_uint32 intIfNum,
                                              L7_in6_addr_t *addr);

/*********************************************************************
*
* @purpose Set the interface to get the local address from
*
* @param unit           unit index
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
extern L7_RC_t usmDbRlimTunnelLocalIntfSet(L7_uint32 unit, L7_uint32 intIfNum,
                                           L7_uint32 localIntIfNum);

/*********************************************************************
*
* @purpose Clear the local/source address of a tunnel.
*
* @param unit         unit index
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
extern L7_RC_t usmDbRlimTunnelLocalAddrClear(L7_uint32 unit,
                                             L7_uint32 intIfNum);

/*********************************************************************
*
* @purpose Set the remote/destination Ip4 address of a tunnel.
*
* @param unit         unit index
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
extern L7_RC_t usmDbRlimTunnelRemoteIp4AddrSet(L7_uint32 unit,
                                               L7_uint32 intIfNum,
                                               L7_uint32 addr);

/*********************************************************************
*
* @purpose Set the remote/destination Ip6 address of a tunnel.
*
* @param unit         unit index
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
extern L7_RC_t usmDbRlimTunnelRemoteIp6AddrSet(L7_uint32 unit,
                                               L7_uint32 intIfNum,
                                               L7_in6_addr_t *addr);

/*********************************************************************
*
* @purpose Clear the remote/destination address of a tunnel.
*
* @param unit         unit index
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
extern L7_RC_t usmDbRlimTunnelRemoteAddrClear(L7_uint32 unit,
                                              L7_uint32 intIfNum);

/*********************************************************************
*
* @purpose Get the tunnel mode of a tunnel.
*
* @param unit           unit index
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
extern L7_RC_t usmDbRlimTunnelModeGet(L7_uint32 unit, L7_uint32 intIfNum,
                                      L7_TUNNEL_MODE_t *pTunnelMode);

/*********************************************************************
*
* @purpose Get the configured local address type of a tunnel.
*
* @param unit           unit index
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
extern L7_RC_t usmDbRlimTunnelCfgLocalAddrTypeGet(L7_uint32 unit,
                                                  L7_uint32 intIfNum,
                                                  rlimAddrType_t *pAddrType);

/*********************************************************************
*
* @purpose Get the type of the local address of a tunnel.
*
* @param unit           unit index
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
extern L7_RC_t usmDbRlimTunnelLocalAddrTypeGet(L7_uint32 unit,
                                               L7_uint32 intIfNum,
                                               rlimAddrType_t *pAddrType);

/*********************************************************************
*
* @purpose Get the type of the remote address of a tunnel.
*
* @param unit           unit index
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
extern L7_RC_t usmDbRlimTunnelRemoteAddrTypeGet(L7_uint32 unit,
                                                L7_uint32 intIfNum,
                                                rlimAddrType_t *pAddrType);

/*********************************************************************
*
* @purpose Get the local/source Ip4 address of a tunnel.
*
* @param unit         unit index
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
extern L7_RC_t usmDbRlimTunnelLocalIp4AddrGet(L7_uint32 unit,
                                              L7_uint32 intIfNum,
                                              L7_uint32 *addr);

/*********************************************************************
*
* @purpose Get the local/source Ip6 address of a tunnel.
*
* @param unit         unit index
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
extern L7_RC_t usmDbRlimTunnelLocalIp6AddrGet(L7_uint32 unit,
                                              L7_uint32 intIfNum,
                                              L7_in6_addr_t *addr);

/*********************************************************************
*
* @purpose Get the local/source interface of a tunnel.
*
* @param unit         unit index
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
extern L7_RC_t usmDbRlimTunnelLocalIntfGet(L7_uint32 unit, L7_uint32 intIfNum,
                                           L7_uint32 *pIntIfNum);

/*********************************************************************
*
* @purpose Get the remote/destination Ip4 address of a tunnel.
*
* @param unit         unit index
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
extern L7_RC_t usmDbRlimTunnelRemoteIp4AddrGet(L7_uint32 unit,
                                               L7_uint32 intIfNum,
                                               L7_uint32 *addr);

/*********************************************************************
*
* @purpose Get the remote/destination Ip6 address of a tunnel.
*
* @param unit         unit index
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
extern L7_RC_t usmDbRlimTunnelRemoteIp6AddrGet(L7_uint32 unit,
                                               L7_uint32 intIfNum,
                                               L7_in6_addr_t *addr);

/*********************************************************************
*
* @purpose Get the hop limit for the given tunnel
*
* @param unit           unit index
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
extern L7_RC_t usmDbRlimTunnelHopLimitGet(L7_uint32 unit, L7_uint32 intIfNum,
                                          L7_uint32 *pHopLimit);

/*********************************************************************
*
* @purpose Get the security value for the given tunnel
*
* @param unit           unit index
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
extern L7_RC_t usmDbRlimTunnelSecurityGet(L7_uint32 unit, L7_uint32 intIfNum,
                                          L7_uint32 *pSecurity);

/*********************************************************************
*
* @purpose Get the TOS value for the given tunnel
*
* @param unit           unit index
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
extern L7_RC_t usmDbRlimTunnelTOSGet(L7_uint32 unit, L7_uint32 intIfNum,
                                     L7_int32 *pTOS);

/*********************************************************************
*
* @purpose Get the flow label for the given tunnel
*
* @param unit           unit index
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
extern L7_RC_t usmDbRlimTunnelFlowLabelGet(L7_uint32 unit, L7_uint32 intIfNum,
                                           L7_int32 *pFlowLabel);

/*********************************************************************
*
* @purpose Get the encapsulation limit for the given tunnel
*
* @param unit           unit index
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
extern L7_RC_t usmDbRlimTunnelEncapsLimitGet(L7_uint32 unit,
                                             L7_uint32 intIfNum,
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
extern L7_RC_t usmDbRlimTunnelIdFirstGet(L7_uint32 *pTunnelId);

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
extern L7_RC_t usmDbRlimTunnelIdNextGet(L7_uint32 tunnelId,
                                        L7_uint32 *pNextTunnelId);



/*********************************************************************
*
* @purpose Get the next valid Loopback Id.
*
* @param tunnelId       loopbackId to start after
* @param pNextTunnelId  return value for the next loopbackId
*
* @returns L7_SUCCESS  If able to find a succeeding valid tunnel
* @returns L7_FAILURE  If no succeeding valid loopback
*
* @notes none
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbRlimLoopbackIdNextGet(L7_uint32 loopbackId,
                                        L7_uint32 *pNextLoopbackId);
																				


#endif /* !defined(USMDB_RLIM_API_H) */
