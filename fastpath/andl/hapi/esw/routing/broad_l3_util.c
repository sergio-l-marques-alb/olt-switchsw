/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
********************************************************************************
*
* @filename  broad_l3_util.c
*
* @purpose   This file contains helper functions for HAPI L3 module and do not
*            involve host or route or tunnel processing.
*
* @component HAPI
*
* @comments  none
*
* @create    10/1/2007
*
* @author    sdoke
*
* @end
*
*******************************************************************************/
#define L7_MAC_ENET_VRRP

#include "broad_l3_int.h"
#include "broad_policy.h"
#include "broad_l2_vlan.h"
#include "commdefs.h"
#include "bcm/tunnel.h"
#include "bcm/error.h"
#include "bcmx/port.h"
#include "l7_usl_bcmx_l2.h"

/* Maximum number of HOST policies allowed.
* All L3 interfaces will use the same router MAC address,
* but VRRP interfaces will have different router MAC address for
* every VRRP router intf. As we cannot afford to burn CAP entries
* to handle every possible router MAC address, limit the number of HOST
* policies that can be installed. This number is arbitrary and derived
* from max VRRP routers. Allow max of 4 different router MAC addresses.
*/
#define BROAD_L3_HOST_POLICIES_MAX  (L7_VRRP_MAX_VIRT_ROUTERS + 4)

typedef struct BROAD_L3_HOST_POLICY_ID_s
{
  L7_uchar8 mac[L7_MAC_ADDR_LEN];
  L7_uint8  useCount;
  BROAD_POLICY_t  sysId;
}BROAD_L3_HOST_POLICY_ID_t;

static BROAD_L3_HOST_POLICY_ID_t localHostPolicies[BROAD_L3_HOST_POLICIES_MAX];

static L7_RC_t hapiBroadL3HostPolicyApplyToLag(DAPI_USP_t *usp,
                                               BROAD_POLICY_t sysId,
                                               L7_uint8 action,  /* 0 = add, 1 = del */
                                               DAPI_t *dapi_g);

extern L7_VLAN_MASK_t routedVlanMask;

/*******************************************************************************
* @purpose  Insert an entry into AVL tree. Wrapper for avlInsertEntry()
*
* @param    tree - pointer to the AVL tree structure
* @param    item - pointer to the item to be inserted
*
* @returns  see avlInsertEntry() and avlSearchLVL7() for return pointer.
*
* @comments
*
* @end
*******************************************************************************/
void * hapiBroadL3AvlInsert(avlTree_t *tree, void *item)
{
  void *entry;

  entry = avlInsertEntry(tree, item);

  if (entry != L7_NULL) /* Item was not inserted */
  {
    HAPI_BROAD_L3_LOG_ERROR(0);
  }

  /* Search the entry to find the item inserted */
  entry = avlSearchLVL7(tree, item, AVL_EXACT);
  if (L7_NULL == entry)
  {
    HAPI_BROAD_L3_LOG_ERROR(0); /* Cannot find the inserted item */
  }

  return entry;
}

/*******************************************************************************
* @purpose  Checks if there is room for a new host entry
*
* @param    cmd - if host is being added/modified
* @param    pHostInfo - Host IP address
* @param    pMac -  MAC Address
* @param    dapi_g - driver object pointer
*
*
* @returns  L7_TRUE - If host table is full
*           L7_FALSE - If there is room in host table.
*
* @comments
*
* @end
*******************************************************************************/
L7_BOOL hapiBroadL3HostTableFull(BROAD_L3_HOST_CMD_t cmd,
                                 BROAD_L3_HOST_KEY_t *pHostInfo,
                                 L7_enetMacAddr_t    *pMac,
                                 DAPI_t *dapi_g)
{
  BROAD_L3_NH_KEY_t  nhKey;
  BROAD_L3_MAC_KEY_t  macKey;
  DAPI_PORT_t           *dapiPortPtr;
  BROAD_PORT_t          *hapiPortPtr;

  dapiPortPtr = DAPI_PORT_GET(&pHostInfo->addrUsp.usp, dapi_g);
  hapiPortPtr = HAPI_PORT_GET(&pHostInfo->addrUsp.usp, dapi_g);

  HAPI_BROAD_L3_SEMA_TAKE(hapiBroadL3Sema, L7_WAIT_FOREVER);

  memset(&nhKey, 0, sizeof(BROAD_L3_NH_KEY_t));
  memset(&macKey, 0, sizeof(BROAD_L3_MAC_KEY_t));

  if (cmd == BROAD_L3_HOST_CMD_ADD)
  {
    if (hapiBroadL3HostTree.count == HAPI_BROAD_L3_HOST_TBL_SIZE)
    {
      HAPI_BROAD_L3_SEMA_GIVE(hapiBroadL3Sema);
      return L7_TRUE;
    }

    /* Check for Nexthop */
    nhKey.family = pHostInfo->family;
    memcpy(&nhKey.addrUsp, &pHostInfo->addrUsp, sizeof(BROAD_L3_ADDR_USP_t));

    if (avlSearchLVL7(&hapiBroadL3NhopTree, &nhKey, AVL_EXACT) == L7_NULLPTR)
    {
      if (hapiBroadL3NhopTree.count == HAPI_BROAD_L3_NH_TBL_SIZE)
      {
        HAPI_BROAD_L3_SEMA_GIVE(hapiBroadL3Sema);
        return L7_TRUE;
      }
    }
  }

  /* Check for MAC table */
  if (IS_PORT_TYPE_PHYSICAL(dapiPortPtr) == L7_TRUE)
  {
    macKey.vlanId = hapiPortPtr->port_based_routing_vlanid;
  }
  else if (IS_PORT_TYPE_LOGICAL_VLAN(dapiPortPtr) == L7_TRUE)
  {
    macKey.vlanId = dapiPortPtr->modeparm.router.vlanID;
  }
  else
  {
    macKey.vlanId = HPC_STACKING_VLAN_ID;
  }

  memcpy(&macKey.macAddr, pMac->addr, L7_ENET_MAC_ADDR_LEN);
  if (avlSearchLVL7(&hapiBroadL3MacTree, &macKey, AVL_EXACT) == L7_NULLPTR)
  {
    if (hapiBroadL3MacTree.count == HAPI_BROAD_L3_MAC_TBL_SIZE)
    {
      HAPI_BROAD_L3_SEMA_GIVE(hapiBroadL3Sema);
      return L7_TRUE;
    }
  }

  HAPI_BROAD_L3_SEMA_GIVE(hapiBroadL3Sema);
  return L7_FALSE;
}

/*******************************************************************************
* @purpose  Checks if there is room for a route entry
*
* @param    cmd - if route is being added/modified
* @param    family - IPv4 or IPv6 family of route
* @param    pRtNhop - list of next hops for the route
* @param    dapi_g - driver object pointer
*
* @returns  L7_TRUE - If  route table is full.
*           L7_FALSE - If there is room in route table.
*
* @comments The reason for this sort of check is because driver AVL trees and
*           application tables do not always have the same limits (say ECMP).
*
* @end
*******************************************************************************/
L7_BOOL hapiBroadL3RouteTableFull(BROAD_L3_ROUTE_CMD_t cmd, L7_uint8 family,
                                  DAPI_ROUTING_ROUTE_ENTRY_t * pRtNhop,
                                  DAPI_t *dapi_g)
{
  L7_uint32 i;
  L7_uint32 numNextHops = 0;
  BROAD_L3_NH_KEY_t  nhKey;
  BROAD_L3_ECMP_ENTRY_t ecmp;

  HAPI_BROAD_L3_SEMA_TAKE(hapiBroadL3Sema, L7_WAIT_FOREVER);

  memset(&ecmp, 0, sizeof(BROAD_L3_ECMP_ENTRY_t));

  if ((cmd == BROAD_L3_ROUTE_CMD_ADD) &&
      (hapiBroadL3RouteTree.count == HAPI_BROAD_L3_ROUTE_TBL_SIZE))
  {
    HAPI_BROAD_L3_SEMA_GIVE(hapiBroadL3Sema);
    return L7_TRUE;
  }

  /* Check for Nexthop table */
  for (i=0; i < L7_RT_MAX_EQUAL_COST_ROUTES; i++)
  {
    if (L7_TRUE == pRtNhop->equalCostRoute[i].valid)
    {
      if (isValidUsp(&(pRtNhop->equalCostRoute[i].usp), dapi_g) == L7_TRUE)
      {
        memset(&nhKey, 0, sizeof(BROAD_L3_NH_KEY_t));
        nhKey.family = family;
        nhKey.addrUsp.usp = pRtNhop->equalCostRoute[i].usp;

        if (L7_AF_INET6 == nhKey.family)
        {
           memcpy(&nhKey.addrUsp.addr, &pRtNhop->equalCostRoute[i].ip6Addr,
             sizeof(bcm_ip6_t));
        }
        else
        {
           nhKey.addrUsp.addr.ipv4  = pRtNhop->equalCostRoute[i].ipAddr;
        }

        memcpy(&ecmp.key.addrUsp[numNextHops],
               &nhKey.addrUsp,
               sizeof(BROAD_L3_ADDR_USP_t));
        numNextHops++;

        if (IS_USP_TYPE_TUNNEL(&nhKey.addrUsp.usp) == L7_FALSE)
        {
          if (avlSearchLVL7(&hapiBroadL3NhopTree, &nhKey, AVL_EXACT) == L7_NULL)
          {
            /* here if route requires a new nexthop */
            if (hapiBroadL3NhopTree.count == HAPI_BROAD_L3_NH_TBL_SIZE)
            {
              HAPI_BROAD_L3_SEMA_GIVE(hapiBroadL3Sema);
              return L7_TRUE;
            }
          }
        }
      }
    }
  }

  /* Check for ECMP table */
  if (numNextHops > 1)
  {
    ecmp.key.family = family;
    if (avlSearchLVL7(&hapiBroadL3EcmpTree, &ecmp, AVL_EXACT) == L7_NULL)
    {
      /* here if route requires a new ECMP group */
      if (hapiBroadL3EcmpTree.count == HAPI_BROAD_L3_MAX_ECMP_GROUPS)
      {
        HAPI_BROAD_L3_SEMA_GIVE(hapiBroadL3Sema);
        return L7_TRUE;
      }
    }
  }

  HAPI_BROAD_L3_SEMA_GIVE(hapiBroadL3Sema);
  return L7_FALSE;
}


/******************************************************************************
* Get the CPU HAPI Port Pointer
*******************************************************************************/
BROAD_PORT_t * hapiBroadL3CpuHapiPortGet(DAPI_t *dapi_g)
{
  DAPI_USP_t cpuUsp;

  if (CPU_USP_GET(&cpuUsp) != L7_SUCCESS)
  {
    HAPI_BROAD_L3_LOG_ERROR(-1);
  }

  return HAPI_PORT_GET(&cpuUsp, dapi_g);
}


/*******************************************************************************
* @purpose  Compute a suitable value for the maximum number of ECMP
*           paths based on the software limit and the constraints of
*           the hardware i.e. it must be a power of 2.
*
* @param    void
*
* @returns  the number of ECMP paths to be used
*
* @comments
*
* @end
*******************************************************************************/
static L7_uint32 hapiBroadL3MaxEcmpPathsGet(void)
{
  extern L7_uint32 platRtrRouteMaxEqualCostEntriesGet(void);
  L7_uint32 sw_val = platRtrRouteMaxEqualCostEntriesGet();
  L7_uint32 ret_val = 1;
  L7_uint32 b;

  if ((sw_val & (sw_val - 1)) == 0)
  {
    /* The software value is already a power of 2, just return it. */
    return sw_val;
  }
  for (b = 0; b < 32; ++b)
  {
    if (ret_val > sw_val)
    {
      return ret_val;
    }
    ret_val <<= 1;
  }

  /* The number of paths exceeded 2^31 so we are out of bits. */
  HAPI_BROAD_L3_LOG_ERROR(sw_val);

  /* NOTREACHED */
  return (1 << 31);
}


/*******************************************************************************
* @purpose  Set the max ECMP paths for a unit
*
* @param    unit - bcm unit number for which ECMP max path needs to be set.
*
* @returns  L7_RC_t
*
* @comments This function needs to be called after bcm_l3_init() is done and
*           before any ECMP routes are configured.
*
* @end
*******************************************************************************/
L7_RC_t hapiBroadL3MaxEcmpPathsSet(L7_uint32 unit)
{
  L7_uint32 maxEcmpPaths = hapiBroadL3MaxEcmpPathsGet();
  L7_int32  rv;

  /* Set the max ECMP paths for local units */
  if (maxEcmpPaths > 1)
  {
    if (!SOC_IS_XGS_FABRIC(unit))
    {
      if (!SOC_IS_RAPTOR(unit))
      {
        rv = bcm_l3_route_max_ecmp_set(unit, maxEcmpPaths);
        if (rv != BCM_E_NONE)
        {
          HAPI_BROAD_L3_LOG_ERROR (rv);
        }
      }
    }
  }

  return L7_SUCCESS;
}


/******************************************************************************
*
* @purpose Check if a given VLAN has routing enabled
*
* @param  vlanId    VLAN to be checked
* @param  vlanUsp   USP of the VLAN routing interface - output param
* @param *dapi_g    DAPI driver object
*
* @returns L7_BOOL
*
* @notes none
*
* @end
*
*******************************************************************************/
L7_BOOL hapiBroadL3VlanRoutingEnabled (L7_ushort16 vlanId,
                                       DAPI_USP_t *vlanUsp,
                                       DAPI_t *dapi_g)
{
  DAPI_USP_t   usp;
  DAPI_PORT_t *dapiPortPtr;

  if (vlanId <= 0 || L7_VLAN_ISMASKBITSET(routedVlanMask, vlanId) == L7_FALSE)
  {
    return L7_FALSE;
  }

  usp.unit = L7_LOGICAL_UNIT;
  usp.slot = L7_VLAN_SLOT_NUM;

  for (usp.port = 0; usp.port < L7_MAX_NUM_ROUTER_INTF; usp.port++)
  {
    if (isValidUsp(&usp,dapi_g) == L7_FALSE)
    {
      continue;
    }

    dapiPortPtr = GET_DAPI_PORT(dapi_g, &usp);

    if ((dapiPortPtr->modeparm.router.vlanID == vlanId) &&
        (dapiPortPtr->modeparm.router.routerIntfEnabled == L7_TRUE))
    {
      *vlanUsp = usp;
      return L7_TRUE;
    }
  }

  return L7_FALSE;
}


/******************************************************************************
*
* @purpose Set the IP MTU for the given routing interface
*
* @param  usp       USP of the routing interface
* @param  mtu       MTU of the routing interface
* @param *dapi_g    DAPI driver object
*
* @returns L7_RC_t
*
* @notes   L3 MTU support varies between devices. XGS3 and Scorpion only support
*          L3 MTU on a per port basis (limitations with VLAN routing interfaces, 
*          where a port can be part of multiple routed VLANs).
*
*          Triumph (or later) devices removed the per-port L3 MTU settings and
*          made it a per L3-Interface (VLAN) setting. One caveat is that, to 
*          configure different L3 MTU values for IPv4 and IPv6, we need to create
*          2 L3 interfaces (one for IPv4 and one for IPv6) and set L3 MTU on the 
*          corresponding L3  interface (cumbersome from HAPI standpoint, and hence 
*          not supported).
*
* @end
*
*******************************************************************************/
L7_RC_t hapiBroadL3IntfMtuSet (DAPI_USP_t *usp, L7_ulong32  mtu, DAPI_t *dapi_g)
{
  L7_RC_t result = L7_SUCCESS;
  DAPI_PORT_t    *dapiPortPtr;
  BROAD_PORT_t   *hapiPortPtr;
  DAPI_USP_t      tUsp;
  L7_ushort16     vlanId;
  L7_int32        rv;

  dapiPortPtr = DAPI_PORT_GET(usp,dapi_g);
  hapiPortPtr = HAPI_PORT_GET(usp,dapi_g);

  /* Note: we could use 'soc_feature_egr_l3_mtu' to check the L3 MTU support.
   * But, such a check would fail on mixed stacking
   */

  /* For devices which support per-port L3 MTU, set the MTU here */
  if (L7_TRUE == IS_PORT_TYPE_PHYSICAL(dapiPortPtr))
  {
    dapiPortPtr->modeparm.physical.mtu = mtu;

    rv = bcmx_port_l3_mtu_set(hapiPortPtr->bcmx_lport, (L7_int32)mtu);

    /* Ignore the E_UNAVAIL error code */
    if ((L7_BCMX_OK(rv) != L7_TRUE) && (rv != BCM_E_UNAVAIL))
    {
      HAPI_BROAD_L3_BCMX_DBG(rv, "bcmx_port_l3_mtu_set: returned %s",
                              bcm_errmsg(rv));
      result = L7_FAILURE;
    }
  }
  else if (IS_PORT_TYPE_LOGICAL_VLAN(dapiPortPtr) == L7_TRUE)
  { 
    /* When device supports per-port L3 MTU, it is difficult to handle L3 MTU
     * on VLAN routing interfaces, as a port can be part of multiple VLANs.
     * Also, port's VLAN membership changes need to be tracked, and the L3 MTU
     * needs to be updated accordingly. TBD.
     */
    dapiPortPtr->modeparm.router.mtu = mtu;

    /* Update the MTU on all members of the routed VLAN */
    vlanId = dapiPortPtr->modeparm.router.vlanID;

    /* Find the current list of ports in the routed VLAN */
    for (tUsp.unit=0; tUsp.unit < dapi_g->system->totalNumOfUnits; tUsp.unit++)
    {
      for (tUsp.slot=0; tUsp.slot<dapi_g->unit[tUsp.unit]->numOfSlots; tUsp.slot++)
      {
        if ((dapi_g->unit[tUsp.unit]->slot[tUsp.slot]->cardPresent == L7_TRUE) &&
            (IS_SLOT_TYPE_PHYSICAL(&tUsp, dapi_g) == L7_TRUE))
        {
          for (tUsp.port=0;
               tUsp.port<dapi_g->unit[tUsp.unit]->slot[tUsp.slot]->numOfPortsInSlot;
               tUsp.port++)
          {
            if (BROAD_IS_HW_VLAN_MEMBER(&tUsp, vlanId, dapi_g))
            {
              hapiPortPtr = HAPI_PORT_GET(&tUsp,dapi_g);

              rv = bcmx_port_l3_mtu_set(hapiPortPtr->bcmx_lport, (L7_int32)mtu);
              if ((L7_BCMX_OK(rv) != L7_TRUE) && (rv != BCM_E_UNAVAIL))
              {
                 HAPI_BROAD_L3_BCMX_DBG(rv, "bcmx_port_l3_mtu_set: returned %s",
                                         bcm_errmsg(rv));
                 result = L7_FAILURE;
              }
            }
          }
        }
      }
    }
  }

  return result;
}

#define HOST_POLICY_ADD     0
#define HOST_POLICY_DELETE  1
/******************************************************************************
*
* @purpose Add/delete a list of ports to the host policy
*
* @param   usp    - unit/slot/port of L3 interface/physical port/LAG
* @param   sysId  - Id of the ARP system policy to be applied
* @param   action - 0 to apply policy to the port. 1 to remove policy.
*
* @returns L7_RC_t
*
* @notes none
*
* @end
*
*******************************************************************************/
static L7_RC_t hapiBroadL3HostPolicyUpdate(DAPI_USP_t *usp,
                                           BROAD_POLICY_t sysId,
                                           L7_uint8 action,  /* 0 = add, 1 = del */
                                           DAPI_t *dapi_g)
{
  BROAD_PORT_t *hapiPortPtr;
  DAPI_PORT_t  *dapiPortPtr;
  DAPI_USP_t    tUsp;
  L7_ushort16   vlanId;

  dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);
  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);

  /* If the USP is a physical port, update the policy only on that port.
   * If the USP is a LAG, update the policy on all ports of the LAG
   * If the USP is a logical VLAN, update the policy for all ports in that VLAN.
   */

  if (L7_TRUE == IS_PORT_TYPE_PHYSICAL(dapiPortPtr))
  {
    /* Update the policy on the given physical port */
    if (action == HOST_POLICY_ADD)
    {
      hapiBroadPolicyApplyToIface(sysId, hapiPortPtr->bcmx_lport);
    }
    else
    {
      hapiBroadPolicyRemoveFromIface(sysId, hapiPortPtr->bcmx_lport);
    }
  }
  else if (IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr) == L7_TRUE)
  {
    hapiBroadL3HostPolicyApplyToLag(usp, sysId, action, dapi_g);
  }
  else if (IS_PORT_TYPE_LOGICAL_VLAN(dapiPortPtr) == L7_TRUE)
  {
    /* Update the policy on all members of the routed VLAN */
    vlanId = dapiPortPtr->modeparm.router.vlanID;

    /* Find the current list of ports in the routed VLAN */
    for (tUsp.unit=0; tUsp.unit < dapi_g->system->totalNumOfUnits; tUsp.unit++)
    {
      for (tUsp.slot=0; tUsp.slot<dapi_g->unit[tUsp.unit]->numOfSlots; tUsp.slot++)
      {
        if ((dapi_g->unit[tUsp.unit]->slot[tUsp.slot]->cardPresent == L7_TRUE) &&
            (IS_SLOT_TYPE_PHYSICAL(&tUsp, dapi_g) == L7_TRUE))
        {
          for (tUsp.port=0;
               tUsp.port<dapi_g->unit[tUsp.unit]->slot[tUsp.slot]->numOfPortsInSlot;
               tUsp.port++)
          {
            if (BROAD_IS_HW_VLAN_MEMBER(&tUsp, vlanId, dapi_g))
            {
              hapiPortPtr = HAPI_PORT_GET(&tUsp,dapi_g);
              if (action == HOST_POLICY_ADD)
              {
                hapiBroadPolicyApplyToIface(sysId, hapiPortPtr->bcmx_lport);
              }
              else
              {
                hapiBroadPolicyRemoveFromIface(sysId, hapiPortPtr->bcmx_lport);
              }
            }
          }
        }
        else if (IS_SLOT_TYPE_LOGICAL_LAG(&tUsp, dapi_g) == L7_TRUE)
        {
          for (tUsp.port=0;
               tUsp.port<dapi_g->unit[tUsp.unit]->slot[tUsp.slot]->numOfPortsInSlot;
               tUsp.port++)
          {
            if (BROAD_IS_VLAN_MEMBER(&tUsp, vlanId, dapi_g))
            {
              hapiBroadL3HostPolicyApplyToLag(&tUsp, sysId, action, dapi_g);
            }
          }
        }
      }
    }
  }

  return L7_SUCCESS;
}


/******************************************************************************
*
* @purpose Apply the policy to all the ports in a LAG interface
*
* @param   usp    - unit/slot/port of L3 interface/physical port/LAG
* @param   sysId  - Id of the ARP system policy to be applied
* @param   action - 0 to apply policy to the port. 1 to remove policy.
*
* @returns L7_RC_t
*
* @notes none
*
* @end
*
*******************************************************************************/
static L7_RC_t hapiBroadL3HostPolicyApplyToLag(DAPI_USP_t *usp,
                                               BROAD_POLICY_t sysId,
                                               L7_uint8 action,  /* 0 = add, 1 = del */
                                               DAPI_t *dapi_g)
{
  L7_uint8     i;
  BROAD_PORT_t *hapiLagPortPtr;
  DAPI_PORT_t  *dapiPortPtr;

  dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);

  /* Update the policy on all ports of the LAG */
  for (i = 0; i < L7_MAX_MEMBERS_PER_LAG; i++)
  {
    if (dapiPortPtr->modeparm.lag.memberSet[i].inUse == L7_TRUE)
    {
      hapiLagPortPtr = HAPI_PORT_GET(&dapiPortPtr->modeparm.lag.memberSet[i].usp, dapi_g);

      if (action == HOST_POLICY_ADD)
      {
        hapiBroadPolicyApplyToIface(sysId, hapiLagPortPtr->bcmx_lport);
      }
      else
      {
        hapiBroadPolicyRemoveFromIface(sysId, hapiLagPortPtr->bcmx_lport);
      }
    }
  }

  return L7_SUCCESS;
}

/******************************************************************************
*
* @purpose Add local host MAC address policy to trap ARP replies to CPU.
*          Idea is to avoid tunneling ARP replies. If protocol_pkt_control
*          register is used, packets will be tunneled.
*
* @param   usp - unit/slot/port of L3 interface
* @param   mac - MAC Address of the L3 interface.
*
* @returns L7_RC_t
*
* @notes  The policy is added when L3 interface is created
*         and when new unit joins the stack.
*
* @end
*
*******************************************************************************/
L7_RC_t hapiBroadL3HostPolicyInstall (DAPI_USP_t *usp, L7_uchar8 *mac, DAPI_t *dapi_g)
{
  L7_uint8            i, idx=255;
  BROAD_POLICY_t      sysId;
  BROAD_POLICY_RULE_t ruleId;
  L7_ushort16 arp =  L7_ETYPE_ARP;
  L7_uchar8   exact_match[] = {FIELD_MASK_NONE, FIELD_MASK_NONE, FIELD_MASK_NONE,
                              FIELD_MASK_NONE, FIELD_MASK_NONE, FIELD_MASK_NONE};
  BROAD_PORT_t *hapiPortPtr;
  DAPI_PORT_t  *dapiPortPtr;

  dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);
  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);

  /* Check if we have added a Host policy with the same MAC address */
  for (i=0; i < BROAD_L3_HOST_POLICIES_MAX; i++)
  {
    if ((localHostPolicies[i].sysId != BROAD_POLICY_INVALID) &&
        (localHostPolicies[i].useCount))
    {
      if (memcmp(localHostPolicies[i].mac, mac, L7_MAC_ADDR_LEN) == 0)
      {
         /* Entry already exists. Do not create, just apply the policy */
         hapiBroadL3HostPolicyUpdate (usp, localHostPolicies[i].sysId,
                                      HOST_POLICY_ADD, dapi_g);
         localHostPolicies[i].useCount++;
         return L7_SUCCESS;
      }
    }
    else if (255 == idx)
    {
      idx = i; /* Note unused index */
    }
  }

  if (255 == idx)
  {
     L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DRIVER_COMPONENT_ID,
             "hapiBroadL3HostPolicyInstall: MAX policies already installed\n");
     return L7_FAILURE;
  }

  hapiBroadPolicyCreate(BROAD_POLICY_TYPE_SYSTEM);
  hapiBroadPolicyRuleAdd(&ruleId);
  hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_ETHTYPE,
                                  (L7_uchar8 *)&arp, exact_match);

  hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_MACDA,
                                  mac, exact_match);

  hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_SET_COSQ,
                               HAPI_BROAD_INGRESS_MED_PRIORITY_COS, 0, 0);

  /* Trap to CPU. Only ARP packets destined to our router MAC address will
   * be trapped. So, this action will not affect ARP packets going through
   * the switch (which are not destined to our router)
   */
  hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_TRAP_TO_CPU, 0, 0, 0);

  if (hapiBroadPolicyCommit(&sysId) == L7_SUCCESS)
  {
    localHostPolicies[idx].sysId = sysId;
    memcpy(localHostPolicies[idx].mac, mac, L7_MAC_ADDR_LEN);
    localHostPolicies[idx].useCount = 1;

    /* We want system policy type, but we don't want to apply to all ports. This
     * policy should be applied to routed ports only (and not switching ports).
     */
    if (hapiBroadPolicyRemoveFromAll(sysId) != L7_SUCCESS)
    {
      HAPI_BROAD_L3_LOG_ERROR(0);
    }

    /* Apply the policy to the routed port */
    hapiBroadL3HostPolicyUpdate(usp, sysId, HOST_POLICY_ADD, dapi_g);
  }
  else
  {
    HAPI_BROAD_L3_LOG_ERROR(0);
  }

  return L7_SUCCESS;
}


/******************************************************************************
*
* @purpose Delete the local host MAC address policy.
*
* @param   usp - unit/slot/port of L3 interface
* @param   mac - MAC Address of the L3 interface.
*
* @returns L7_RC_t
*
* @notes  The policy is deleted when L3 interface is deleted
*
* @end
*
*******************************************************************************/
L7_RC_t hapiBroadL3HostPolicyRemove (DAPI_USP_t *usp, L7_uchar8 *mac, DAPI_t *dapi_g)
{
  L7_uint8  i;

  /* Check if we have added a Host policy with the MAC address */
  for (i=0; i < BROAD_L3_HOST_POLICIES_MAX; i++)
  {
    if ((localHostPolicies[i].sysId != BROAD_POLICY_INVALID) &&
        (localHostPolicies[i].useCount))
    {
      if (memcmp(localHostPolicies[i].mac, mac, L7_MAC_ADDR_LEN) == 0)
      {
         /* Found the policy for this MAC */
         localHostPolicies[i].useCount--;

         /* The USP is no longer a routed port, remove policy */
         if (localHostPolicies[i].useCount == 0)
         {
            hapiBroadPolicyDelete(localHostPolicies[i].sysId);
            localHostPolicies[i].sysId = BROAD_POLICY_INVALID;
         }
         else
         {
           /* The policy is in use by other L3 interfaces. Just update */
           hapiBroadL3HostPolicyUpdate(usp, localHostPolicies[i].sysId,
                                       HOST_POLICY_DELETE, dapi_g);
         }
         return L7_SUCCESS;
      }
    }
  }

  return L7_SUCCESS;
}


/******************************************************************************
*
* @purpose Notification for VLAN port add/delete
*
* @param   DAPI_USP_t  *usp    - unit/slot/port
* @param   L7_ushort16 vlanId  - VLAN Id
* @param   L7_BOOL addDel      - 0 implies port is added to vlan. 1 for delete.
* @param   DAPI_t      *dapi_g
*
* @returns L7_RC_t result
*
* @notes   The callback is used for VLAN routing interfaces. As ports can be
*          added/deleted from routed VLAN, the host policy needs to be updated.
*
* @end
*
*******************************************************************************/
L7_RC_t hapiBroadL3PortVlanNotify(DAPI_USP_t  *usp,
                                  L7_ushort16 vlanId,
                                  L7_BOOL     addDel,
                                  DAPI_t      *dapi_g)
{
  L7_uint8 i;
  L7_ushort16 vrid;
  L7_int32  rv, mtu;
  DAPI_USP_t   vlanUsp;
  BROAD_PORT_t *hapiPortPtr;
  DAPI_PORT_t  *dapiPortPtr;
  


  /* Check if the VLAN is enabled for routing */
  if (hapiBroadL3VlanRoutingEnabled(vlanId, &vlanUsp, dapi_g) == L7_FALSE)
  {
    return L7_SUCCESS; /* Ignore VLANs that are not enabled for routing */
  }
  else
  {
    /* Here if the VLAN is enabled for routing */
    HAPI_BROAD_L3_DEBUG(broadL3Debug, "hapiBroadL3PortVlanNotify: USP %d:%d:%d "
        "vlan %d, addDel %d\n", usp->unit, usp->slot, usp->port, vlanId, addDel);

    dapiPortPtr = DAPI_PORT_GET(&vlanUsp, dapi_g); /* L3 interface */
    hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);     /* Physical port */

    /* Update the L3 IP MTU for devices that support per-port L3 MTU */
    if (addDel)
    {
      /* Port is no longer part of routed VLAN. Reset IP MTU to max */
      mtu = (L7_int32)(L7_MAX_FRAME_SIZE-4); 
    }
    else
    {
      /* Port is added to routed VLAN, set IP MTU as per L3 interface.
       * If port is part of multiple router VLANs, only one L3 MTU will be
       * enforced (ambiguity!!)
       */
      mtu = (L7_int32)dapiPortPtr->modeparm.router.mtu;
    }
 
    /* hapiBroadL3IntfMtuSet cannot be used here as it is tied to L3 USP */
    rv = bcmx_port_l3_mtu_set(hapiPortPtr->bcmx_lport, mtu);
    if ((L7_BCMX_OK(rv) != L7_TRUE) && (rv != BCM_E_UNAVAIL))
    {
      HAPI_BROAD_L3_BCMX_DBG(rv, "bcmx_port_l3_mtu_set: returned %s",
                             bcm_errmsg(rv));
    }

    /* Update Host policy */
    for (i=0; i < BROAD_L3_HOST_POLICIES_MAX; i++)
    {
      if ((localHostPolicies[i].sysId != BROAD_POLICY_INVALID) &&
          (localHostPolicies[i].useCount))
      {
        if (memcmp(localHostPolicies[i].mac, &(L7_ENET_VRRP_MAC_ADDR),
                   L7_MAC_ADDR_LEN - 1) != 0)
        {
          /* If this is non-VRRP MAC address, then it is routed MAC address */
          hapiBroadL3HostPolicyUpdate(usp, localHostPolicies[i].sysId,
                                      addDel ? HOST_POLICY_DELETE: HOST_POLICY_ADD,
                                      dapi_g);
        }
        else
        {
          /* Apply the host policy only if the VLAN has VRRP enabled */
          hapiPortPtr = HAPI_PORT_GET(&vlanUsp, dapi_g);

          vrid = localHostPolicies[i].mac[5];
          if (hapiPortPtr->vrrp_interface_id[vrid])
          {
            hapiBroadL3HostPolicyUpdate(usp, localHostPolicies[i].sysId,
                                        addDel ? HOST_POLICY_DELETE: HOST_POLICY_ADD,
                                        dapi_g);
          }
        }
      }
    }
  }

  return L7_SUCCESS;
}


/******************************************************************************
*
* @purpose Remove all protocol VLAN associations for this port
*
* @param   DAPI_USP_t *usp
* @param   DAPI_t     *dapi_g
*
* @returns L7_RC_t result
*
* @notes   Typically called when port based routing is enabled on a port.
*
* @end
*
*******************************************************************************/
L7_RC_t hapiBroadL3ProtocolVlanRemove(DAPI_USP_t *usp, DAPI_t *dapi_g)
{
  L7_uint32     i;
  BROAD_PORT_t *hapiPortPtr;
  BROAD_SYSTEM_t       *hapiSystemPtr;

  hapiSystemPtr = (BROAD_SYSTEM_t *)dapi_g->system->hapiSystem;

  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);

  for (i = 0; i < L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS; i++)
  {
    if (hapiPortPtr->pbvlan_table[i] != 0)
    {
      hapiBroadProtocolVlanRemove(usp, hapiSystemPtr->etherProtocol[i], dapi_g);
    }
  }

  return L7_SUCCESS;
}


/******************************************************************************
*
* @purpose Restore all protocol VLAN associations for this port
*
* @param   DAPI_USP_t *usp
* @param   DAPI_t     *dapi_g
*
* @returns L7_RC_t result
*
* @notes   Typically called when port based routing is disabled on a port.
*
* @end
*
*******************************************************************************/
L7_RC_t hapiBroadL3ProtocolVlanAdd (DAPI_USP_t *usp, DAPI_t *dapi_g)
{
  L7_uint32     i;
  BROAD_PORT_t *hapiPortPtr;
  BROAD_SYSTEM_t       *hapiSystemPtr;

  hapiSystemPtr = (BROAD_SYSTEM_t *)dapi_g->system->hapiSystem;

  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);

  for (i = 0; i < L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS; i++)
  {
    if (hapiPortPtr->pbvlan_table[i] != 0)
    {
      hapiBroadProtocolVlanAdd(usp, hapiSystemPtr->etherProtocol[i],
                               hapiPortPtr->pbvlan_table[i], dapi_g);
    }
  }

  return L7_SUCCESS;
}

/******************************************************************************
*
* @purpose  Check if v6 prefix lengths > 64 are supported or not. Helix/FB/FB2
*           devices supported prefixes upto 64bits and v6 routes with prefix
*           lengths /65 to /127 were not supported (routes with /128 prefix
*           were handled as v6 host entry instead of v6 route entry).
*
*           Some of the new XGS3 devices, Triumph/Valyrie/Scorpion have support
*           for upto 128bit prefix lengths.
*
* @param    none
*
* @returns  L7_BOOL - True if 128bit prefix is supported. Else False.
*
* @notes
*
* @end
*
*******************************************************************************/
L7_BOOL hapiBroadL3Ipv6Prefix128Supported(void)
{
  L7_short16 bcm_unit;

  /* Instead of relying on the family type, use the SOC property
   * 'soc_feature_lpm_prefix_length_max_128' to check the prefix len support.
   * This way any future XGS3 devices are handled automatically.
   */
  for (bcm_unit=0; bcm_unit < soc_ndev; bcm_unit++)
  {
    /* Note: We query the local units on manager. All units in a stack are
     * assumed to have similar properties.
     */
    if (soc_feature(bcm_unit, soc_feature_lpm_prefix_length_max_128))
    {
      return L7_TRUE;
    }
  }

  return L7_FALSE;
}

/******************************************************************************
*
* @purpose  Given L7 tunnel mode, return the BCM tunnel type
*
* @param    none
*
* @returns  L7_BOOL - True if 128bit prefix is supported. Else False.
*
* @notes
*
* @end
*
*******************************************************************************/
L7_RC_t hapiBroadL3BcmTunnelType(L7_TUNNEL_MODE_t l7TunnelType,
                                 L7_uint32 *bcmTunnelType)
{
  L7_RC_t result = L7_SUCCESS;

  switch (l7TunnelType)
  {
    /* 6o4 tunnels */
    case L7_TUNNEL_MODE_6OVER4:
      *bcmTunnelType = bcmTunnelTypeIp6In4;
      break;

    /* 4o4 tunnels */
    case L7_TUNNEL_MODE_4OVER4:
      *bcmTunnelType = bcmTunnelTypeIp4In4;
      break;

    /* 6to4 tunnels */
    case L7_TUNNEL_MODE_6TO4:
      /* Note: There are 2 modes for 6to4 tunnel type. Secured and unchecked.
       * See bcmTunnelType6In4 vs bcmTunnelType6In4Uncheck.
       * We want to configure secured 6to4 tunnel when supported [this mode
       * has enhanced support for handling DoS attacks on tunnels]. But not
       * all devices support this secure mode. To make things worse, Rev A0
       * of 56504 doesn't support, but Rev B0 of 56504 supports. This mode is
       * identified by soc_feature_tunnel_6to4_secure feature.
       */
      *bcmTunnelType = bcmTunnelType6In4;
      break;

    default:
      result = L7_FAILURE;
      HAPI_BROAD_L3_DEBUG(broadL3Debug, "hapiBroadL3BcmTunnelType: Invalid tunnel"
                          "type %d\n", l7TunnelType);
      break;
  }

  return result;
}

/******************************************************************************
*
* @purpose  Send a message to ASYNC task and wait for Wlist processing to
*           complete
*
* @param    DAPI_USP_t
* @param    DAPI_CMD_t
*
* @returns  L7_RC_t
*
* @notes    Input params are for debug purpose only
*
* @end
*
*******************************************************************************/
L7_RC_t hapiBroadL3AsyncWait (DAPI_USP_t *usp, DAPI_CMD_t cmd)
{
  L7_RC_t rc;
  L7_clocktime tStart, tCurr;
  void   *waitSema;

  HAPI_BROAD_L3_DEBUG(broadL3WaitDebug, "hapiBroadL3AsyncWait: USP %d:%d:%d CMD %d\n",
                  usp->unit, usp->slot, usp->port, cmd);
  /*
   * Create a lock that will be released by the async task to
   * serialize this request with the async processing.
   */
  HAPI_BROAD_L3_SEMA_CREATE(waitSema, OSAPI_SEM_Q_FIFO, OSAPI_SEM_EMPTY);

  rc = osapiMessageSend (hapiBroadL3WlistWaitQueue,
                         (void*)&waitSema,
                         sizeof (waitSema),
                         L7_WAIT_FOREVER,
                         L7_MSG_PRIORITY_NORM);
  if (rc != L7_SUCCESS)
  {
    HAPI_BROAD_L3_LOG_ERROR (rc);
  }

  /* Wake up Async task and wait on the sema */
  HAPI_BROAD_L3_WAKE_UP_ASYNC_TASK;

  HAPI_BROAD_L3_DEBUG(broadL3WaitDebug, "hapiBroadL3AsyncWait: Waiting on"
                      " work list sema %p\n", waitSema);

  osapiClockTimeRaw(&tStart);
  HAPI_BROAD_L3_SEMA_TAKE(waitSema, L7_WAIT_FOREVER);
  osapiClockTimeRaw(&tCurr);

  HAPI_BROAD_L3_DEBUG(broadL3WaitDebug, "hapiBroadL3AsyncWait: Done in %d s\n",
                      tCurr.seconds - tStart.seconds);

  HAPI_BROAD_L3_SEMA_DELETE(waitSema);

  return L7_SUCCESS;
}


/******************************************************************************
*
* @purpose Add an L3-enabled MAC address entry to hardware
*
* @param  usp       unit slot port
* @param  pMacAddr  MAC address
* @param *dapi_g    system information
*
* @returns L7_RC_t result
*
* @notes none
*
* @end
*
*******************************************************************************/
L7_RC_t hapiBroadL3MacAddressAdd(DAPI_USP_t *usp,
                                 L7_enetMacAddr_t *pMacAddr,
                                 DAPI_t *dapi_g)
{
  DAPI_PORT_t *dapiPortPtr;
  BROAD_PORT_t *hapiPortPtr;
  bcmx_l2_addr_t l2addr;
  bcm_vlan_t vid = 0;
  int rv;

  dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);
  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);

  if (IS_PORT_TYPE_PHYSICAL(dapiPortPtr) == L7_TRUE)
  {
    vid = hapiPortPtr->port_based_routing_vlanid;
  }
  else if (IS_PORT_TYPE_LOGICAL_VLAN(dapiPortPtr) == L7_TRUE)
  {
    vid = dapiPortPtr->modeparm.router.vlanID;
  }
  else
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DRIVER_COMPONENT_ID,
            "hapiBroadL3MacAddressAdd: Invalid USP %u\n", dapiPortPtr->type);
    return L7_FAILURE;
  }

  bcmx_l2_addr_t_init(&l2addr, pMacAddr->addr, vid);
  l2addr.flags = (BCM_L2_L3LOOKUP | BCM_L2_STATIC | BCM_L2_REPLACE_DYNAMIC);

  /* Set the port for the L2 entry to CPU port */
  hapiPortPtr =  hapiBroadL3CpuHapiPortGet(dapi_g);
  l2addr.lport = hapiPortPtr->bcmx_lport;

  rv = usl_bcmx_l2_addr_add(&l2addr, L7_NULL);
  if (rv != BCM_E_NONE)
  {
    HAPI_BROAD_L3_ASYNC_DBG("hapiBroadL3MacAddressAdd: Mac %x:%x:%x:%x:%x:%x"
                            " vlan %d\n", l2addr.mac[0], l2addr.mac[1],
                            l2addr.mac[2], l2addr.mac[3], l2addr.mac[4],
                            l2addr.mac[5], vid);
    HAPI_BROAD_L3_ASYNC_DBG("hapiBroadL3MacAddressAdd: "
                 "usl_bcmx_l2_addr_add returned %d (%s)\n", rv, bcm_errmsg(rv));

    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*******************************************************************************
*
* @purpose Delete an L3-enabled MAC address entry from hardware
*
* @param  usp       unit slot port
* @param  pMacAddr  MAC address
* @param *dapi_g    system information
*
* @returns L7_RC_t result
*
* @notes none
*
* @end
*
*******************************************************************************/
L7_RC_t hapiBroadL3MacAddressDelete(DAPI_USP_t *usp,
                                    L7_enetMacAddr_t *pMacAddr,
                                    DAPI_t *dapi_g)
{
  DAPI_PORT_t *dapiPortPtr;
  BROAD_PORT_t *hapiPortPtr;
  bcm_vlan_t vid = 0;
  int rv;

  dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);
  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);

  if (IS_PORT_TYPE_PHYSICAL(dapiPortPtr) == L7_TRUE)
  {
    vid = hapiPortPtr->port_based_routing_vlanid;
  }
  else if (IS_PORT_TYPE_LOGICAL_VLAN(dapiPortPtr) == L7_TRUE)
  {
    vid = dapiPortPtr->modeparm.router.vlanID;
  }
  else
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DRIVER_COMPONENT_ID,
            "hapiBroadL3MacAddressDelete: Invalid USP %u\n", dapiPortPtr->type);
    return L7_FAILURE;
  }

  rv = usl_bcmx_l2_addr_delete(pMacAddr->addr, vid);
  if (rv != BCM_E_NONE)
  {
    HAPI_BROAD_L3_ASYNC_DBG("hapiBroadL3MacAddressDelete: Mac %x:%x:%x:%x:%x:%x"
                            " vlan %d\n", pMacAddr->addr[0], pMacAddr->addr[1],
                            pMacAddr->addr[2], pMacAddr->addr[3],
                            pMacAddr->addr[4], pMacAddr->addr[5], vid);

    HAPI_BROAD_L3_ASYNC_DBG("hapiBroadL3MacAddressDelete: "
               "usl_bcmx_l2_addr_delete returned %d (%s)\n", rv, bcm_errmsg(rv));

    return L7_FAILURE;
  }

  return L7_SUCCESS;
}
