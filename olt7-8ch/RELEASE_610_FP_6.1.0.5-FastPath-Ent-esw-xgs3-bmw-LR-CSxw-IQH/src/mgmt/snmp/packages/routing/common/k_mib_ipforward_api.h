/*********************************************************************
*
* (C) Copyright Broadcom Corporation 1999-2007
*
**********************************************************************
*
* @filename   k_mib_ipforward_api.h
*
* @purpose    Wrapper functions for IP Forward MIB ( RFC 2096 Draft 7)
*
* @component  SNMP
*
* @comments
*
* @create     07/22/2004
*
* @author     kmanish
* @end
*
**********************************************************************/

#ifndef K_MIB_IPFORWARD_API_H
#define K_MIB_IPFORWARD_API_H

/* Begin Function Prototypes */

/****************************************************************************************/
L7_RC_t
snmpInetCidrRouteEntryValidate(L7_uint32 routeDest,L7_uint32 prefixLen,
                               L7_uint32 routeNextHop);

L7_RC_t
snmpInetCidrRouteEntryGetNext(L7_uint32 *routeDest,L7_uint32* prefixLenNext,
                              L7_uint32* routeNextHop);

L7_RC_t
snmpInetCidrRouteIfIndexGet(L7_uint32 routeDest,L7_uint32 prefixLen,
                            L7_uint32 routeNextHop, L7_uint32* intIfNum);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      


L7_RC_t
snmpInetCidrRouteTypeGet(L7_uint32 routeDest,L7_uint32 prefixLen,
                         L7_uint32 routeNextHop, L7_uint32* routeType);

L7_RC_t
snmpInetCidrRouteProtoGet(L7_uint32 routeDest,L7_uint32 prefixLen,
                          L7_uint32 routeNextHop, L7_uint32* routeProto);

L7_RC_t
snmpInetCidrRouteAgeGet(L7_uint32 routeDest,L7_uint32 prefixLen,
                        L7_uint32 routeNextHop, L7_uint32* routeAge);

L7_RC_t
snmpInetCidrRouteMetric1Get(L7_uint32 routeDest,L7_uint32 prefixLen,
                            L7_uint32 routeNextHop, L7_uint32* routeMetric1);

L7_RC_t
snmpInetCidrRouteMetric2Get(L7_uint32 routeDest,L7_uint32 prefixLen,
                            L7_uint32 routeNextHop, L7_uint32* routeMetric2);

L7_RC_t
snmpInetCidrRouteMetric3Get(L7_uint32 routeDest,L7_uint32 prefixLen,
                            L7_uint32 routeNextHop, L7_uint32* routeMetric3);

L7_RC_t
snmpInetCidrRouteMetric4Get(L7_uint32 routeDest,L7_uint32 prefixLen,
                            L7_uint32 routeNextHop, L7_uint32* routeMetric4);

L7_RC_t
snmpInetCidrRouteMetric5Get(L7_uint32 routeDest,L7_uint32 prefixLen,
                            L7_uint32 routeNextHop, L7_uint32* routeMetric5);


/****************************************************************************************/

/* End Function Prototypes */

#endif /* K_MIB_IPFORWARD_API_H */
