/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* Name: broad_l3_mcast.c
*
* Purpose: This file contains the debug routines for Layer 3 IP Multicast
*          hapi interface
*
* Component: hapi
*
* Comments:
*
* Created by:
*
*********************************************************************/
#include <string.h>

#include "l7_common.h"
#include "sysapi.h"
#include "osapi.h"

#include "broad_common.h"
#include "broad_l3_mcast.h"
#include "broad_l3_mcast_debug.h"

#include "bcmx/ipmc.h"
/* PTin added: SDK 6.3.0 */
#include "ptin_globaldefs.h"
#if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
#include "bcmx/multicast.h"
#endif

#define MCAST_PRINT(fmt,args...)  {printf("\n%s [%d]: ", __FUNCTION__,__LINE__);printf(fmt,##args);}

extern DAPI_t    *dapi_g;

/*********************************************************************
*
* @purpose To hold the actions for the mcast debug routine
*
* @end
*
*********************************************************************/
typedef enum
{
  L7_DEBUG_MCAST_ACTION_ADD      = 1,
  L7_DEBUG_MCAST_ACTION_DELETE   = 2,
  L7_DEBUG_MCAST_ACTION_USEGET   = 3,
} L7_MCAST_DEBUG_ACTION_t;



/*********************************************************************
*
* @purpose Init debug routines for IP Mcast package
*
* @param   
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadL3McastDebugInit()
{
  /* nothing to do here, we just needed to link in this file */
  return L7_SUCCESS;
}

void dumpMcastCounters(bcm_port_t dport)
{
    bcm_ipmc_counters_t counters;
    L7_uint32            rv;

    L7_ulong64                rmca_l;
    L7_ulong64                tmca_l;
    L7_ulong64                imbp_l;
    L7_ulong64                imrp_l;
    L7_ulong64                rimdr_l;
    L7_ulong64                timdr_l;

    rv = bcm_ipmc_counters_get(0,
                               dport, /* port */
                               &counters);
    if (L7_BCMX_OK(rv) != L7_TRUE) {
      SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                     "\nError: ipmc counters get: %s\n",
                     bcm_errmsg(rv));
      /* HACK - this will be a LOG_ERROR return L7_FAILURE; */
    }
    hapiBroadStatsConvert(&rmca_l, counters.rmca);
    hapiBroadStatsConvert(&tmca_l, counters.tmca);
    hapiBroadStatsConvert(&imbp_l, counters.imbp);
    hapiBroadStatsConvert(&imrp_l, counters.imrp);
    hapiBroadStatsConvert(&rimdr_l, counters.rimdr);
    hapiBroadStatsConvert(&timdr_l, counters.timdr);

    printf("rx l2: %u %u\n", rmca_l.high, rmca_l.low);
    printf("tx l2: %u %u\n", tmca_l.high, tmca_l.low);
    printf("ipmc bridged: %u %u\n", imbp_l.high, imbp_l.low);
    printf("ipmc routed: %u %u\n", imrp_l.high, imrp_l.low);
    printf("ipmc ing dropped: %u %u\n", rimdr_l.high, rimdr_l.low);
    printf("ipmc egr dropped: %u %u\n", timdr_l.high, timdr_l.low);
}

L7_RC_t debugMcastAdd(L7_int32   in_unit,
                      L7_int32   in_slot,
                      L7_int32   in_port,
                      L7_ulong32 ipMcastAddr,
                      L7_ulong32 ipSourceAddr,
                      DAPI_MCAST_RPF_CHECK_FAIL_ACTION_t rpfAction,
                      L7_int32   num_downstream_if,
                      L7_int32   out_unit1,
                      L7_int32   out_slot1,
                      L7_int32   out_port1,
                      L7_int32   out_unit2,
                      L7_int32   out_slot2,
                      L7_int32   out_port2)
{
  DAPI_USP_t usp;
  DAPI_ROUTING_INTF_MGMT_CMD_t dapiCmd;
  DAPI_USP_t out_usp[5];


  usp.unit = in_unit;
  usp.slot = in_slot;
  usp.port = in_port;

  dapiCmd.cmdData.mcastAdd.mcastGroupAddr.addr.ipv4.s_addr    = ipMcastAddr;
  dapiCmd.cmdData.mcastAdd.getOrSet          = DAPI_CMD_SET;
  dapiCmd.cmdData.mcastAdd.rpfCheckEnable    = L7_TRUE;
  dapiCmd.cmdData.mcastAdd.rpfType           = L7_TRUE;
  dapiCmd.cmdData.mcastAdd.rpfCheckFailAction = rpfAction;

  out_usp[0].unit = out_unit1;
  out_usp[0].slot = out_slot1;
  out_usp[0].port = out_port1;
  out_usp[1].unit = out_unit2;
  out_usp[1].slot = out_slot2;
  out_usp[1].port = out_port2;

  dapiCmd.cmdData.mcastAdd.outUspCount = num_downstream_if;
  dapiCmd.cmdData.mcastAdd.outGoingIntfPresent = L7_TRUE;
  dapiCmd.cmdData.mcastAdd.outUspList = &out_usp[0];


  dapiCmd.cmdData.mcastAdd.matchSrcAddr   = L7_TRUE ;
  dapiCmd.cmdData.mcastAdd.srcIpAddr.addr.ipv4.s_addr     = ipSourceAddr;

  dapiCtl(&usp, DAPI_CMD_ROUTING_INTF_MCAST_ADD, &dapiCmd);

  return 0;

}

L7_RC_t debugMcastDelete(L7_uint32   in_unit,
                         L7_uint32   in_slot,  
                         L7_uint32   in_port,
                         L7_ulong32  ipMcastAddr,
                         L7_ulong32  ipSourceAddr)

{
  DAPI_USP_t usp;
  DAPI_ROUTING_INTF_MGMT_CMD_t dapiCmd;


  usp.unit = in_unit;
  usp.slot = in_slot;
  usp.port = in_port;

  dapiCmd.cmdData.mcastDelete.mcastGroupAddr.addr.ipv4.s_addr    = ipMcastAddr;
  dapiCmd.cmdData.mcastDelete.getOrSet          = DAPI_CMD_SET;

  dapiCmd.cmdData.mcastDelete.matchSrcAddr     = L7_TRUE ;
  dapiCmd.cmdData.mcastDelete.srcIpAddr.addr.ipv4.s_addr       = ipSourceAddr;

  dapiCtl(&usp, DAPI_CMD_ROUTING_INTF_MCAST_DELETE, &dapiCmd);

  return 0;

}

L7_RC_t hapiBroadDebugL3McastRepl(ip_addr_t  groupIp,
                                  ip_addr_t  srcAddr,
                                  bcm_vlan_t vid,
                                  L7_uint32  unit,
                                  L7_uint32  slot,
                                  L7_uint32  port)
{
    DAPI_USP_t       usp;
    bcmx_ipmc_addr_t ipmc_data;
    L7_uint32        i;
    L7_uint32        rv;
    bcm_vlan_vector_t ipmc_vlan_vec;
    BROAD_PORT_t    *hapiPortPtr;

    usp.unit = unit;
    usp.slot = slot;
    usp.port = port;

    hapiPortPtr = HAPI_PORT_GET(&usp, dapi_g);

    memset (&ipmc_data, 0, sizeof (ipmc_data));

    /* PTin modified: SDK 6.3.0 */
    #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
    ipmc_data.vid         = vid;
    ipmc_data.s_ip_addr   = srcAddr;
    ipmc_data.mc_ip_addr  = groupIp;
    rv = bcmx_ipmc_find(&ipmc_data);
    #else
    rv = bcmx_ipmc_get(srcAddr, groupIp, vid, &ipmc_data);
    #endif
    if (L7_BCMX_OK(rv) != L7_TRUE)
    {
      bcmx_ipmc_addr_free(&ipmc_data);
      return L7_SUCCESS;
    }
    /* PTin modified: SDK 6.3.0 */
    #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
    rv = bcmx_multicast_repl_get(ipmc_data.ipmc_index, hapiPortPtr->bcmx_lport,
                                 ipmc_vlan_vec);
    #else
    rv = bcmx_ipmc_repl_get(ipmc_data.ipmc_index, hapiPortPtr->bcmx_lport,
                            ipmc_vlan_vec);
    #endif
    if (rv != BCM_E_UNAVAIL)
    {
      if (L7_BCMX_OK(rv) != L7_TRUE)
      {
        bcmx_ipmc_addr_free(&ipmc_data);
        return L7_SUCCESS;
      }
      printf("For group %x, source %x, usp = %d %d %d, replication enabled on vlans:\n",
             groupIp, srcAddr, unit, slot, port);
      for (i=0; i<L7_PLATFORM_MAX_VLAN_ID; i++)
      {
        if (BCM_VLAN_VEC_GET(ipmc_vlan_vec, i))
        {
          printf("%d ", i);
        }
      }
    }
    printf("\n");
    bcmx_ipmc_addr_free(&ipmc_data);

    return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose modify the TTL value for the interface
*
* @param  inIntIfNum     - interface number
* @param  mcastTtlValue  - TTL value
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiDebugMcastTTLSet(L7_uint32 inIntIfNum, L7_uint32 mcastTtlValue)
{
  L7_RC_t rc;
  L7_RC_t dr;
  DAPI_ROUTING_INTF_MGMT_CMD_t dapiCmd;
  nimUSP_t nimusp;
  DAPI_USP_t ddusp;
  
  if (nimGetUnitSlotPort(inIntIfNum, &nimusp) != L7_SUCCESS)
  {
    MCAST_PRINT("nimGetUnitSlotPort failed for intf %d", inIntIfNum);
    return L7_FAILURE;
  }

  ddusp.unit = nimusp.unit;
  ddusp.slot = nimusp.slot;
  ddusp.port = nimusp.port - 1;

  dapiCmd.cmdData.ttlMcastVal.getOrSet    = DAPI_CMD_SET;
  dapiCmd.cmdData.ttlMcastVal.ttlVal      = mcastTtlValue;


  dr = dapiCtl(&ddusp, DAPI_CMD_ROUTING_INTF_MCAST_TTL_SET, &dapiCmd);

  if (dr == L7_SUCCESS)
  {
    MCAST_PRINT("DAPI_CMD_ROUTING_INTF_MCAST_TTL_SET successful");
    rc = L7_SUCCESS;
  }
  else
  {
    MCAST_PRINT("DAPI_CMD_ROUTING_INTF_MCAST_TTL_SET failed");
    rc = L7_FAILURE;
  }
  return rc;
}

/*********************************************************************
*
* @purpose enable/diables the MLD in the silicon
*
* @param  family   - IPv6 or IPv4 family
* @param  enable   - enable/diables the MLD in the silicon
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiDebugMcastMldEnable(int family, int enable)
{

  L7_RC_t rc;
  L7_RC_t dr;
  DAPI_ROUTING_MGMT_CMD_t dapiCmd;
  DAPI_USP_t ddusp;

  if (dapiCpuUspGet(&ddusp) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  dapiCmd.cmdData.mcastIgmpConfig.getOrSet = DAPI_CMD_SET;
  dapiCmd.cmdData.mcastIgmpConfig.enable = (enable? L7_TRUE:L7_FALSE);
  dapiCmd.cmdData.mcastIgmpConfig.family = (family? L7_AF_INET6: L7_AF_INET);

  dr = dapiCtl(&ddusp, DAPI_CMD_ROUTING_ROUTE_MCAST_IGMP_CONFIG, &dapiCmd);

  if (dr == L7_SUCCESS)
  {
    MCAST_PRINT("DAPI_CMD_ROUTING_ROUTE_MCAST_IGMP_CONFIG successful");
    rc = L7_SUCCESS;
  }
  else
  {
    MCAST_PRINT("DAPI_CMD_ROUTING_ROUTE_MCAST_IGMP_CONFIG failed");
    rc = L7_FAILURE;
  }
  return rc;
}

/*********************************************************************
*
* @purpose enable/diables the mcast in the silicon
*
* @param  enable   - enable/diables the mcast in the silicon
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t  hapiDebugMcastEnable(int enable)
{
  L7_RC_t rc;
  L7_RC_t dr;
  DAPI_ROUTING_MGMT_CMD_t dapiCmd;
  DAPI_USP_t ddusp;

  if (dapiCpuUspGet(&ddusp) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  dapiCmd.cmdData.mcastforwardConfig.getOrSet = DAPI_CMD_SET;
  dapiCmd.cmdData.mcastforwardConfig.enable = enable;
  dapiCmd.cmdData.mcastforwardConfig.family = L7_AF_INET6;

  dr = dapiCtl(&ddusp, DAPI_CMD_ROUTING_ROUTE_MCAST_FORWARDING_CONFIG, &dapiCmd);

  if (dr == L7_SUCCESS)
  {
    MCAST_PRINT("DAPI_CMD_ROUTING_ROUTE_MCAST_FORWARDING_CONFIG successful");
    rc = L7_SUCCESS;
  }
  else
  {
    MCAST_PRINT("DAPI_CMD_ROUTING_ROUTE_MCAST_FORWARDING_CONFIG failed");
    rc = L7_SUCCESS;
  }

  return rc;
}

static int hapiDebugMcastDelete(L7_inet_addr_t *srcIp, L7_inet_addr_t * mcastIp, int srcIntf)
{
   L7_RC_t rc;
   L7_RC_t dr;
   DAPI_ROUTING_INTF_MGMT_CMD_t dapiCmd;
   nimUSP_t nimusp;
   DAPI_USP_t ddusp;

   if (nimGetUnitSlotPort(srcIntf, &nimusp) != L7_SUCCESS)
   {
      return L7_FAILURE;
   }

   ddusp.unit = nimusp.unit;
   ddusp.slot = nimusp.slot;
   ddusp.port = nimusp.port - 1;

   dapiCmd.cmdData.mcastDelete.getOrSet       = DAPI_CMD_SET;
   inetCopy(&dapiCmd.cmdData.mcastDelete.mcastGroupAddr, mcastIp);
   dapiCmd.cmdData.mcastDelete.rpfCheckEnable    = 0;

   if (inetIsInAddressAny(srcIp) == L7_FALSE)
   {
      dapiCmd.cmdData.mcastDelete.matchSrcAddr   = L7_TRUE;
      inetCopy(&dapiCmd.cmdData.mcastDelete.srcIpAddr,srcIp);
   } else
   {
      dapiCmd.cmdData.mcastDelete.matchSrcAddr   = L7_FALSE ;
   }

   dr = dapiCtl(&ddusp, DAPI_CMD_ROUTING_INTF_MCAST_DELETE, &dapiCmd);

   if (dr == L7_SUCCESS)
   {
      MCAST_PRINT("Address Successfully Deleted");
      rc = L7_SUCCESS;
   } else if (dr == L7_NOT_SUPPORTED)
   {
      MCAST_PRINT("Not supported");
      rc = L7_SUCCESS;
   } else
   {
      MCAST_PRINT("Address Deletion Failed");
      rc = L7_FAILURE;
   }
   return rc;
}

static int hapiDebugMcastAdd(L7_inet_addr_t *srcIp, L7_inet_addr_t * mcastIp, int srcIntf, int *interface, int count)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_RC_t dr = L7_SUCCESS;
  DAPI_ROUTING_INTF_MGMT_CMD_t dapiCmd;
  nimUSP_t   uspIn;
  nimUSP_t   uspOut;
  DAPI_USP_t ddusp;
  DAPI_USP_t ddoutusp;
  L7_uint32   i;
  L7_uint32   oifList[L7_MAX_INTERFACE_COUNT+1];
  L7_uint32   oifCount = 0;
  DAPI_USP_t  oifUsp[L7_MAX_INTERFACE_COUNT+1];
  L7_inet_addr_t  sourceIpAddress;
  L7_inet_addr_t  mcastGroupDestAddress;

  MCAST_PRINT("Entered");
  if (nimGetUnitSlotPort(srcIntf, &uspIn) != L7_SUCCESS)
  {
    MCAST_PRINT("nimGetUnitSlotPort failed");
    return L7_FAILURE;
  }

  ddusp.unit = uspIn.unit;
  ddusp.slot = uspIn.slot;
  ddusp.port = uspIn.port - 1;

  dapiCmd.cmdData.mcastAdd.getOrSet          = DAPI_CMD_SET;
  inetCopy(&dapiCmd.cmdData.mcastAdd.mcastGroupAddr, mcastIp);
  inetCopy(&mcastGroupDestAddress, mcastIp);
  inetCopy(&dapiCmd.cmdData.mcastAdd.srcIpAddr,srcIp);
  inetCopy(&sourceIpAddress,srcIp);

  dapiCmd.cmdData.mcastAdd.getOrSet          = DAPI_CMD_SET;
  dapiCmd.cmdData.mcastAdd.rpfCheckEnable    = L7_TRUE;
  dapiCmd.cmdData.mcastAdd.rpfType  = DAPI_MCAST_RPF_CHECK_METHOD_IIF_MATCH;
  dapiCmd.cmdData.mcastAdd.rpfCheckFailAction  = DAPI_MCAST_RPF_CHECK_FAIL_ACTION_COPY_TO_CPU;

  if (inetIsInAddressAny(&sourceIpAddress) == L7_FALSE)
  {
     inetCopy(&dapiCmd.cmdData.mcastAdd.srcIpAddr,
                                &sourceIpAddress);
     dapiCmd.cmdData.mcastAdd.matchSrcAddr   = L7_TRUE ;
  }
  else
  {
     dapiCmd.cmdData.mcastAdd.matchSrcAddr   = L7_FALSE ;
  }
 
  if (count)
  {
     /* if there are any outgoing interfaces, create array of USPs, one for eact out router interface */
     int counter = 0;
     for (; counter < count; counter++)
     {
        oifList[counter] = interface[counter];
        oifCount = count;
     }
     for(i=0;i<oifCount;i++)
     {
      if(nimGetUnitSlotPort(oifList[i], &uspOut) != L7_SUCCESS)
      {
         MCAST_PRINT("nimGetUnitSlotPort failed for %d", oifList[i]);
         return L7_FAILURE;
      }
      oifUsp[i].unit = uspOut.unit;
      oifUsp[i].slot = uspOut.slot;
      oifUsp[i].port = uspOut.port - 1;
     }

     dapiCmd.cmdData.mcastAdd.outUspList = oifUsp;
     dapiCmd.cmdData.mcastAdd.outUspCount = oifCount;
     dapiCmd.cmdData.mcastAdd.outGoingIntfPresent = L7_TRUE;
  }
  else
  {
     dapiCmd.cmdData.mcastAdd.outGoingIntfPresent = L7_FALSE;
  }

  dr = dapiCtl(&ddusp, DAPI_CMD_ROUTING_INTF_MCAST_ADD, &dapiCmd);
  if (dr == L7_FAILURE)
  {
    MCAST_PRINT("DAPI_CMD_ROUTING_INTF_MCAST_ADD failed");
    rc = L7_FAILURE;
  }
  else if (dr == L7_NOT_SUPPORTED) /*dtl returns this if entry options not supported on platform, skip port add */
  {
     rc = L7_SUCCESS;
  }
 else if (dr == L7_SUCCESS)
  {
      /* only need to set up egress port masks if there are one or more outgoing interfaces */
      if (dapiCmd.cmdData.mcastAdd.outGoingIntfPresent == L7_TRUE)
      {
         for (i=0;i<oifCount;i++)
         {
            /* setup outport masks for egress interface */

           /* note: unit and port number adjustments between NIM and dapi already taken care of when oifUsp[]
                    was populated above
          */
            ddoutusp.unit = oifUsp[i].unit;
            ddoutusp.slot = oifUsp[i].slot;
            ddoutusp.port = oifUsp[i].port;

            dapiCmd.cmdData.mcastPortAdd.getOrSet          = DAPI_CMD_SET;
            inetCopy(&dapiCmd.cmdData.mcastAdd.mcastGroupAddr,
                                &mcastGroupDestAddress);
            if (inetIsInAddressAny(&sourceIpAddress) == L7_FALSE)
            {
               dapiCmd.cmdData.mcastPortAdd.matchSrcAddr   = L7_TRUE ;
               inetCopy(&dapiCmd.cmdData.mcastAdd.srcIpAddr,
                                &sourceIpAddress);
            } else
            {
               dapiCmd.cmdData.mcastPortAdd.matchSrcAddr   = L7_FALSE ;
            }
            /* if outgoing router port is a physical port, add that port to the mcast route */
            dapiCmd.cmdData.mcastPortAdd.outPortUsp.unit = oifUsp[i].unit;
            dapiCmd.cmdData.mcastPortAdd.outPortUsp.slot = oifUsp[i].slot;
            dapiCmd.cmdData.mcastPortAdd.outPortUsp.port = oifUsp[i].port;
            /* invoke DAPI command to add interface to outgoing port bitmask for this ip mcast route */
            dr = dapiCtl(&ddoutusp, DAPI_CMD_ROUTING_INTF_MCAST_PORT_ADD, &dapiCmd);
            MCAST_PRINT("dr for DAPI_CMD_ROUTING_INTF_MCAST_PORT_ADD %d",dr) ;
           }
      }
   }
   else
         rc = L7_FAILURE;

  MCAST_PRINT("Exit");
  return rc;
}

static int hapiDebugMcastUseGet(L7_inet_addr_t *srcIp, L7_inet_addr_t * mcastIp, int srcIntf)
{
   L7_RC_t rc = L7_FAILURE;
   L7_RC_t dr;
   DAPI_ROUTING_INTF_MGMT_CMD_t dapiCmd;
   nimUSP_t nimusp;
   DAPI_USP_t ddusp;

   if (nimGetUnitSlotPort(srcIntf, &nimusp) != L7_SUCCESS)
   {
      MCAST_PRINT("Invalid interface nunber");
      return L7_FAILURE;
   }

   ddusp.unit = nimusp.unit;
   ddusp.slot = nimusp.slot;
   ddusp.port = nimusp.port - 1;

   dapiCmd.cmdData.mcastUseQuery.getOrSet = DAPI_CMD_GET;

   inetCopy(&dapiCmd.cmdData.mcastUseQuery.mcastGroupAddr, mcastIp);
   dapiCmd.cmdData.mcastUseQuery.rpfCheckEnable = L7_FALSE;

   if (inetIsInAddressAny(srcIp) == L7_FALSE)
   {
      dapiCmd.cmdData.mcastUseQuery.matchSrcAddr   = L7_TRUE;
      inetCopy(&dapiCmd.cmdData.mcastUseQuery.srcIpAddr, srcIp);
   } else
   {
      dapiCmd.cmdData.mcastUseQuery.matchSrcAddr   = L7_FALSE ;
   }

   dr = dapiCtl(&ddusp, DAPI_CMD_ROUTING_INTF_MCAST_USE_GET, &dapiCmd);

   if (dr == L7_SUCCESS)
   {
     MCAST_PRINT("Entry: %s", (dapiCmd.cmdData.mcastUseQuery.entryUsed? "IN USE":"NOT IN USE"));
     rc = L7_SUCCESS;
   }
   else
   {
      MCAST_PRINT("DAPI_CMD_ROUTING_INTF_MCAST_USE_GET failed rv %d", rc);
      rc = L7_FAILURE;
   }
   return rc;
}

/*********************************************************************
*
* @purpose perform the debug action on the interface for a given srcIP
*          and grpIp for IPv4 or IPv6 address
*
* @param  family   - interface number
* @param  src1     - last byte of the src IP
* @param  mcastIp  - last byte of grp IP
* @param  srcIntf  - src intf on the switch, packet will come on 
* @param  intf1    - out going interface 1  
* @param  intf2    - out going interface 2
* @param  count    - count for how many interfaces to be used
* @param  action   - acrion to be done - add/delete/use get
*
* @returns none
*
* @notes   none
*
* @end
*
*********************************************************************/
void hapiDebugMcastAction(int family, unsigned int src1, unsigned int mcastIp, 
                            int srcIntf, int intf1, int intf2, int count, 
                            L7_MCAST_DEBUG_ACTION_t action)
{
  int interface[10];
  L7_inet_addr_t *src     = NULL;
  L7_inet_addr_t *mcast   = NULL;
  L7_inet_addr_t srcip4   = {L7_AF_INET, {{0x0}}};
  L7_inet_addr_t mcastip4 = {L7_AF_INET, {{0x0}}};
  L7_inet_addr_t srcip6   = {L7_AF_INET6, {{0x0}}};
  L7_inet_addr_t mcastip6 = {L7_AF_INET6, {{0x0}}};

  if (family == 0)
  {
    srcip4.family = L7_AF_INET;
    mcastip4.family = L7_AF_INET;

    srcip4.addr.ipv4.s_addr   = src1;
    mcastip4.addr.ipv4.s_addr = mcastIp;

    src   = &srcip4;
    mcast = &mcastip4;
  }
  else
  {
    srcip6.family = L7_AF_INET6;
    mcastip6.family = L7_AF_INET6;
   
    if(src1 == 0) 
    {
      srcip6.addr.ipv6.in6.addr32[0] = 0;
      srcip6.addr.ipv6.in6.addr32[1] = 0;
      srcip6.addr.ipv6.in6.addr32[2] = 0;
      srcip6.addr.ipv6.in6.addr32[3] = 0;
    }
    else
    {
      srcip6.addr.ipv6.in6.addr32[0] = 0x20010000;
      srcip6.addr.ipv6.in6.addr32[1] = 0x0;
      srcip6.addr.ipv6.in6.addr32[2] = 0x0;
      srcip6.addr.ipv6.in6.addr32[3] = src1;
    }

    mcastip6.addr.ipv6.in6.addr32[0] = 0xff0E0000;
    mcastip6.addr.ipv6.in6.addr32[1] = 0x0;
    mcastip6.addr.ipv6.in6.addr32[2] = 0x0;
    mcastip6.addr.ipv6.in6.addr32[3] = mcastIp;

    src   = &srcip6;
    mcast = &mcastip6;
  }

  memset(interface, 0x0, 10);
  interface[0] = intf1;
  interface[1] = intf2;

  switch(action)
  {
    case L7_DEBUG_MCAST_ACTION_ADD: 
         hapiDebugMcastAdd(src, mcast, srcIntf, interface, count);break;
    case L7_DEBUG_MCAST_ACTION_DELETE: 
         hapiDebugMcastDelete(src, mcast, srcIntf); break;
    case L7_DEBUG_MCAST_ACTION_USEGET: 
         hapiDebugMcastUseGet(src, mcast, srcIntf);break;
    default: 
         MCAST_PRINT("Wrong action value: %d", action);break;
  }
  return;
}
