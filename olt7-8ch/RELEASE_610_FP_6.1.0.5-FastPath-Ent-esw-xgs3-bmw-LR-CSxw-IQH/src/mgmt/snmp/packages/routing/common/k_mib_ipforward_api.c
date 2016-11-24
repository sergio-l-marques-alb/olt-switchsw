#include <k_private_base.h>

#include <usmdb_mib_ipfwd_api.h>
#include <l3_commdefs.h>
#include "usmdb_common.h"
#include "usmdb_mib_ipfwd_api.h"
L7_RC_t
snmpInetCidrRouteEntryValidate(L7_uint32 routeDest,L7_uint32 prefixLen,
                               L7_uint32 routeNextHop)
{
  L7_uint32 routeTos = 0;
  L7_uint32 netMask = 0xffffffff; 
  L7_RC_t rc = L7_FAILURE;

  /* Converting Prefix Length to Netmask */
  netMask = netMask << (32- prefixLen);

  /* routeTos is passed just to use below function call */
  rc = usmDbIpCidrRouteEntryGet(USMDB_UNIT_CURRENT,routeDest,
                                netMask,routeTos,routeNextHop);

  return rc;
}


L7_RC_t
snmpInetCidrRouteEntryGetNext(L7_uint32 *routeDest,L7_uint32* prefixLenNext,
                              L7_uint32* routeNextHop)
{
  L7_uint32 netMask = 0xffffffff;
  L7_uint32 bitValue = 0X80000000;
  L7_uint32 routeTosNext = 0;
  L7_uint32 length = 0;
  L7_RC_t rc = L7_FAILURE;

  /* Converting Prefix Length to Netmask */
  netMask = netMask << (32- *prefixLenNext); 

  /* routeTos is passed just to use below function call */
  rc = usmDbIpCidrRouteEntryGetNext(USMDB_UNIT_CURRENT,routeDest,
                                    &netMask,&routeTosNext,routeNextHop);

  while(netMask & bitValue)
  {
    length ++;
    bitValue = bitValue >> 1;
  }

  *prefixLenNext = length;

  return rc;
}


L7_RC_t
snmpInetCidrRouteIfIndexGet(L7_uint32 routeDest,L7_uint32 prefixLen,
                            L7_uint32 routeNextHop, L7_uint32* intIfNum)
{
  L7_uint32 routeTos = 0;
  L7_uint32 netMask = 0xffffffff; 
  L7_RC_t rc = L7_FAILURE;
  /* Converting Prefix Length to Netmask */
  netMask = netMask << (32- prefixLen);

  /* routeTos is passed just to use below function call */
  rc = usmDbIpCidrRouteIfIndexGet(USMDB_UNIT_CURRENT,routeDest,netMask,
                                  routeTos,routeNextHop,intIfNum);
  return rc;
}

L7_RC_t
snmpInetCidrRouteTypeGet(L7_uint32 routeDest,L7_uint32 prefixLen,
                         L7_uint32 routeNextHop, L7_uint32* routeType)
{
  L7_uint32 routeTos = 0;
  L7_uint32 netMask = 0xffffffff; 
  L7_RC_t rc = L7_FAILURE;

  /* Converting Prefix Length to Netmask */
  netMask = netMask << (32- prefixLen);

  /* routeTos is passed just to use below function call */
  rc = usmDbIpCidrRouteTypeGet(USMDB_UNIT_CURRENT,routeDest,netMask,
                                  routeTos,routeNextHop,routeType);

  return rc;
}

L7_RC_t
snmpInetCidrRouteProtoGet(L7_uint32 routeDest,L7_uint32 prefixLen,
                          L7_uint32 routeNextHop, L7_uint32* routeProto)
{
  L7_uint32 routeTos = 0;
  L7_uint32 routeProtoTemp = 0;
  L7_uint32 netMask = 0xffffffff; 
  L7_RC_t rc = L7_FAILURE;

  /* Converting Prefix Length to Netmask */
  netMask = netMask << (32- prefixLen);

  /* routeTos is passed just to use below function call */
  rc = usmDbIpCidrRouteProtoGet(USMDB_UNIT_CURRENT,routeDest,netMask,
                                  routeTos,routeNextHop,&routeProtoTemp);
   
  if (rc == L7_SUCCESS)
  {

    switch (routeProtoTemp)
    {
    case L7_IP_CIDR_ROUTE_PROTO_OTHER :
      *routeProto = D_inetCidrRouteProto_other;
    break;

    case L7_IP_CIDR_ROUTE_PROTO_LOCAL :
      *routeProto = D_inetCidrRouteProto_local;
    break;

    case L7_IP_CIDR_ROUTE_PROTO_NETMGMT :
      *routeProto = D_inetCidrRouteProto_netmgmt;
    break;

    case L7_IP_CIDR_ROUTE_PROTO_ICMP :
      *routeProto = D_inetCidrRouteProto_icmp;
    break;

    case L7_IP_CIDR_ROUTE_PROTO_EGP :
      *routeProto = D_inetCidrRouteProto_egp;
    break;

    case L7_IP_CIDR_ROUTE_PROTO_GGP :
      *routeProto = D_inetCidrRouteProto_ggp;
    break;

    case L7_IP_CIDR_ROUTE_PROTO_HELLO :
      *routeProto = D_inetCidrRouteProto_hello;
    break;

    case L7_IP_CIDR_ROUTE_PROTO_RIP :
      *routeProto = D_inetCidrRouteProto_rip;
    break;

    case L7_IP_CIDR_ROUTE_PROTO_ISIS :
      *routeProto = D_inetCidrRouteProto_isIs;
    break;

    case L7_IP_CIDR_ROUTE_PROTO_ESIS :
      *routeProto = D_inetCidrRouteProto_esIs;
    break;

    case L7_IP_CIDR_ROUTE_PROTO_CISCOIGRP :
      *routeProto = D_inetCidrRouteProto_ciscoIgrp;
    break;

    case L7_IP_CIDR_ROUTE_PROTO_BBNSPFIGP :
      *routeProto = D_inetCidrRouteProto_bbnSpfIgp;
    break;

    case L7_IP_CIDR_ROUTE_PROTO_OSPF :
      *routeProto = D_inetCidrRouteProto_ospf;
    break;

    case L7_IP_CIDR_ROUTE_PROTO_BGP :
      *routeProto = D_inetCidrRouteProto_bgp;
    break;

    case L7_IP_CIDR_ROUTE_PROTO_IDPR :
      *routeProto = D_inetCidrRouteProto_idpr;
    break;

    case L7_IP_CIDR_ROUTE_PROTO_CISCOEIGRP :
      *routeProto = D_inetCidrRouteProto_ciscoEigrp;
    break;

    default:
      /* unknown value */
      rc = L7_FAILURE;
    break;
    }
  }


  return rc;
}


L7_RC_t
snmpInetCidrRouteAgeGet(L7_uint32 routeDest, L7_uint32 prefixLen,
                        L7_uint32 routeNextHop, L7_uint32 *routeAge)
{
  L7_uint32 netMask = 0xffffffff; 
  L7_uint32 routeTos = 0;

  /* Converting Prefix Length to Netmask */
  netMask = netMask << (32 - prefixLen);

  /* routeTos is passed just to use below function call */
  return usmDbIpCidrRouteAgeGet(USMDB_UNIT_CURRENT, routeDest, netMask,
                                routeTos, routeNextHop, routeAge);
}

L7_RC_t
snmpInetCidrRouteMetric1Get(L7_uint32 routeDest,L7_uint32 prefixLen,
                            L7_uint32 routeNextHop, L7_uint32* routeMetric1)
{
  L7_uint32 routeTos = 0;
  L7_uint32 netMask = 0xffffffff; 
  L7_RC_t rc = L7_FAILURE;

  /* Converting Prefix Length to Netmask */
  netMask = netMask << (32- prefixLen);

  /* routeTos is passed just to use below function call */
  rc = usmDbIpCidrRouteMetric1Get(USMDB_UNIT_CURRENT,routeDest,netMask,
                                  routeTos,routeNextHop,routeMetric1);

  return rc;
}

L7_RC_t
snmpInetCidrRouteMetric2Get(L7_uint32 routeDest,L7_uint32 prefixLen,
                            L7_uint32 routeNextHop, L7_uint32* routeMetric2)
{
  L7_uint32 routeTos = 0;
  L7_uint32 netMask = 0xffffffff; 
  L7_RC_t rc = L7_FAILURE;

  /* Converting Prefix Length to Netmask */
  netMask = netMask << (32- prefixLen);

  /* routeTos is passed just to use below function call */
  rc = usmDbIpCidrRouteMetric2Get(USMDB_UNIT_CURRENT,routeDest,netMask,
                                  routeTos,routeNextHop,routeMetric2);

  return rc;
}

L7_RC_t
snmpInetCidrRouteMetric3Get(L7_uint32 routeDest,L7_uint32 prefixLen,
                            L7_uint32 routeNextHop, L7_uint32* routeMetric3)
{
  L7_uint32 routeTos = 0;
  L7_uint32 netMask = 0xffffffff; 
  L7_RC_t rc = L7_FAILURE;

  /* Converting Prefix Length to Netmask */
  netMask = netMask << (32- prefixLen);
  
  /* routeTos is passed just to use below function call */
  rc = usmDbIpCidrRouteMetric3Get(USMDB_UNIT_CURRENT,routeDest,netMask,
                                  routeTos,routeNextHop,routeMetric3);

  return rc;
}

L7_RC_t
snmpInetCidrRouteMetric4Get(L7_uint32 routeDest,L7_uint32 prefixLen,
                            L7_uint32 routeNextHop, L7_uint32* routeMetric4)
{
  L7_uint32 routeTos = 0;
  L7_uint32 netMask = 0xffffffff; 
  L7_RC_t rc = L7_FAILURE;

  /* Converting Prefix Length to Netmask */
  netMask = netMask << (32- prefixLen);

  /* routeTos is passed just to use below function call */
  rc = usmDbIpCidrRouteMetric4Get(USMDB_UNIT_CURRENT,routeDest,netMask,
                                  routeTos,routeNextHop,routeMetric4);

  return rc;
}

L7_RC_t
snmpInetCidrRouteMetric5Get(L7_uint32 routeDest,L7_uint32 prefixLen,
                            L7_uint32 routeNextHop, L7_uint32* routeMetric5)
{
  L7_uint32 routeTos = 0;
  L7_uint32 netMask = 0xffffffff; 
  L7_RC_t rc = L7_FAILURE;

  /* Converting Prefix Length to Netmask */
  netMask = netMask << (32- prefixLen);

  /* routeTos is passed just to use below function call */
  rc = usmDbIpCidrRouteMetric5Get(USMDB_UNIT_CURRENT,routeDest,netMask,
                                  routeTos,routeNextHop,routeMetric5);

  return rc;
}


