#include "ptin_globaldefs.h"
#include "logger.h"
#include "ptin_hapi.h"
#include "l7_usl_bcmx_l3.h"

#include <bcmx/switch.h>
#include <bcmx/port.h>
#include <bcmx/l3.h>

/**
 * Add L3 host
 * 
 * @param dapiPort 
 * @param intf_id 
 * @param dstMacAddr 
 * @param dstIpAddr 
 * 
 * @return RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_hapi_l3_host_add(ptin_dapi_port_t *dapiPort, st_ptin_l3 *data)
{

    int           unit;
    DAPI_PORT_t  *dapiPortPtr;
    BROAD_PORT_t *hapiPortPtr;

    bcm_module_t      module = 0;
    bcm_port_t        port = 0;
    bcm_trunk_t       trunk = -1;
    bcm_mac_t         mac;
    bcm_if_t          interface = -1;
    int               l2tocpu = 0;
    int               rv = L7_SUCCESS;


    /* Input parameters */

    /* Input port/trunk */
    PT_LOG_TRACE(LOG_CTX_HAPI, "dapiPort={%d,%d,%d}",
              dapiPort->usp->unit, dapiPort->usp->slot, dapiPort->usp->port);

    /* Validate dapiPort */
    if (dapiPort->usp->unit<0 || dapiPort->usp->slot<0 || dapiPort->usp->port<0)
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "Invalid interface");
      return L7_FAILURE;
    }

    /* Get port pointers */
    DAPIPORT_GET_PTR(dapiPort, dapiPortPtr, hapiPortPtr);

    unit    = hapiPortPtr->bcm_unit;
    module  = hapiPortPtr->bcm_modid;

    /* Accept only physical and lag interfaces */
    if ( IS_PORT_TYPE_PHYSICAL(dapiPortPtr) )
    {
      port = hapiPortPtr->bcm_port;
    }
    else if ( IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr) )
    {
      trunk = hapiPortPtr->hapiModeparm.lag.tgid;
    }
    else
    {
      PT_LOG_ERR(LOG_CTX_HAPI,"Interface has a not valid type: error!");
      return L7_FAILURE;
    }



    /* _l3_cmd_egress_object_create */

    bcm_l3_egress_t   egress_object;             
    int               copytocpu = 0;
    int               drop = 0;
    int               mpls_label = BCM_MPLS_LABEL_INVALID;
    int               object_id = -1;
    uint32            flags = 0;

    bcm_l3_egress_t_init(&egress_object);
    sal_memset(mac, 0, sizeof(bcm_mac_t));

    /* DMAC */
    sal_memcpy(mac, data->dstMacAddr, sizeof(L7_uint8)*L7_MAC_ADDR_LEN);

    /* Intf id */
    interface = data->l3_intf;

    egress_object.intf   = interface;
    egress_object.mpls_label = mpls_label;
    sal_memcpy(egress_object.mac_addr, mac, sizeof(mac));

    if (BCM_GPORT_IS_SET(port)) {
        egress_object.port = port;
    } else {
        egress_object.module = module;
    if (trunk >= 0) {
        egress_object.flags |= BCM_L3_TGID;
        egress_object.trunk = trunk;
    } else {
        egress_object.port = port;
    }
    }

    if (l2tocpu) {
        egress_object.flags |= BCM_L3_L2TOCPU;
    }

    if (copytocpu) {
        egress_object.flags |= (BCM_L3_COPY_TO_CPU);
    }

    if (drop) {
        egress_object.flags |= (BCM_L3_DST_DISCARD);
    }

    if (object_id >= 0) {
        flags = (BCM_L3_WITH_ID | BCM_L3_REPLACE);
    }


    rv = bcm_l3_egress_create(unit, flags, &egress_object, &object_id);
    if (BCM_FAILURE(rv)) {
        PT_LOG_ERR(LOG_CTX_HAPI,"Error creating egress object entry: %s\n", bcm_errmsg(rv));
        return L7_FAILURE;
    }


    /* _l3_cmd_host_add */

    bcm_l3_host_t   host_info;
    int             host_as_route = 0;
    bcm_ip_t        ip_addr = 0;
    int             untag = 0;
    int             hits = 0;
    int             replace = 0;
    int             rpe = 0;
    int             v6  = 0;
    int             pri = 0;
    int             vrf = 0;
    int             lookup_class = 0;
    bcm_ip6_t       ip6_addr;


    /* Init struct */
    bcm_l3_host_t_init(&host_info);

    host_info.l3a_vrf     = vrf;
    host_info.l3a_pri     = pri;
    host_info.l3a_lookup_class = lookup_class;

    /* Intf id */
    interface = object_id;  /* Object ID from _l3_cmd_egress_object_create */

    host_as_route = data->host_as_route;
    ip_addr       = data->dstIpAddr;    /* IP address */
    untag         = data->untag;
    hits          = data->hits;
    replace       = data->replace;
    l2tocpu       = data->l2ToCpu;
    rpe           = data->rpe;
    v6            = 0;
    pri           = data->pri;
    vrf           = data->vrf;
    lookup_class  = data->lookup_class;

    if (rpe)
    {
      host_info.l3a_flags |= BCM_L3_RPE;
    }

    if (v6)
    {
        memcpy(host_info.l3a_ip6_addr, ip6_addr, BCM_IP6_ADDRLEN);
        host_info.l3a_flags |= BCM_L3_IP6;
    }
    else 
    {
        host_info.l3a_ip_addr = ip_addr;
    } 

    if (host_as_route) {
        host_info.l3a_flags |= BCM_L3_HOST_AS_ROUTE;
    } 
    host_info.l3a_intf = interface;
    sal_memcpy(host_info.l3a_nexthop_mac, mac, sizeof(mac));

    if (BCM_GPORT_IS_SET(port)) {
        host_info.l3a_port_tgid = port;
    }
    else
    {
      host_info.l3a_modid = module;
      if (trunk >= 0)
      {
          host_info.l3a_flags |= BCM_L3_TGID;
          host_info.l3a_port_tgid = trunk;
      }
      else
      {
          host_info.l3a_port_tgid = port;
      }
    }

    if (hits) {
        host_info.l3a_flags |= BCM_L3_HIT;
    }
    if (untag) {
        host_info.l3a_flags |= BCM_L3_UNTAG;
    }

    if (replace) {
        host_info.l3a_flags |= BCM_L3_REPLACE;
    }

#ifdef BCM_XGS_SWITCH_SUPPORT
    if (l2tocpu)
    {
        host_info.l3a_flags |= BCM_L3_L2TOCPU;
    }
#endif

    /* Add host */
    rv = bcm_l3_host_add(unit, &host_info);

    if (BCM_FAILURE(rv))
    {
        PT_LOG_ERR(LOG_CTX_HAPI,"Error creating entry in L3 host table: rv=\"%s\"", bcm_errmsg(rv));
        return L7_FAILURE;
    }

    return L7_SUCCESS;
} 

/**
 * Remove L3 host
 * 
 * @param dapiPort 
 * @param intf_id 
 * @param dstMacAddr 
 * @param dstIpAddr 
 * 
 * @return RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_hapi_l3_host_remove(ptin_dapi_port_t *dapiPort, st_ptin_l3 *data)
{
  PT_LOG_ERR(LOG_CTX_HAPI,"Not implemented yet");
  return L7_NOT_IMPLEMENTED_YET;
}

/**
 * Add L3 route
 * 
 * @param dapiPort 
 * @param intf_id 
 * @param dstMacAddr 
 * @param dstIpAddr 
 * 
 * @return RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_hapi_l3_route_add(ptin_dapi_port_t *dapiPort, st_ptin_l3 *data)
{
  PT_LOG_ERR(LOG_CTX_HAPI,"Not implemented yet");
  return L7_NOT_IMPLEMENTED_YET;
}

/**
 * Remove L3 route
 * 
 * @param dapiPort 
 * @param intf_id 
 * @param dstMacAddr 
 * @param dstIpAddr 
 * 
 * @return RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_hapi_l3_route_remove(ptin_dapi_port_t *dapiPort, st_ptin_l3 *data)
{
  PT_LOG_ERR(LOG_CTX_HAPI,"Not implemented yet");
  return L7_NOT_IMPLEMENTED_YET;
}

/**********************Multicast L3 Forwarding**********************************************/
typedef struct 
{
  bcm_ip6_t   s_ip_addr;              /* This will hold both ipv4 and ipv6 addresses*/
  bcm_ip6_t   mc_ip_addr;             /* This will hold both ipv4 and ipv6 addresses*/
  bcm_vlan_t  vid;
  bcm_vrf_t vrf;                      /* Virtual Router Instance. */
  bcm_cos_t cos;                      /* COS based on dst IP multicast addr. */  
  int ts;                             /* Source port or TGID bit. */
  int port_tgid;                      /* Source port or TGID. */
  int v;                              /* Valid bit. */
  int mod_id;                         /* Module ID. */
  int ipmc_index;                     /* Use this index to program IPMC table
                                         for XGS chips based on flags value.
                                           For SBX chips it is the Multicast
                                           Group index */
  uint32 flags;                       /* See BCM_IPMC_XXX flag definitions. */
  int lookup_class;                   /* Classification lookup class ID. */
  bcm_fabric_distribution_t distribution_class; /* Fabric Distribution Class. */
} ptin_bcm_ipmc_addr_t;

#define L7_IPV6_ADDRESS_SIZE  16   /* 16 Bytes */
#define L7_IPV4_ADDRESS_SIZE  4    /* 4 Bytes  */

#if 0
static void ptin_hapi_broad_ip_set(L7_inet_addr_t *srcIp, L7_inet_addr_t *grpIp, bcm_ipmc_addr_t *ipmc)
{
  memset(ipmc->s_ip_addr,  0, sizeof(ipmc->s_ip_addr));
  memset(ipmc->mc_ip_addr, 0, sizeof(ipmc->mc_ip_addr));

  if ((srcIp->family == L7_AF_INET6) && (grpIp->family == L7_AF_INET6))
  {
    memcpy((ipmc)->s_ip_addr, (L7_uchar8 *)(srcIp)->addr.ipv6.in6.addr8, 16);
    memcpy((ipmc)->mc_ip_addr,(L7_uchar8 *)(grpIp)->addr.ipv6.in6.addr8, 16);
    (ipmc)->flags |= BCM_IPMC_IP6;
  }
  else
  {
    memcpy((ipmc)->s_ip_addr, &(srcIp)->addr.ipv4.s_addr, 4);
    memcpy((ipmc)->mc_ip_addr, &(grpIp)->addr.ipv4.s_addr, 4);
  }
}
#endif


/*********************************************************************
* @purpose  Convert from DTL to SDK BCM format
*
* @param    ipmc    @{(input)} DTL address
* @param    bcmAddr @{(output)} SDK BCM ipmc address
*
* @returns  BCM Return code
*
* @end
*********************************************************************/
static int ptin_ipmc_to_bcm(ptin_dtl_ipmc_addr_t  *dtl_ipmc, bcm_ipmc_addr_t *bcm_ipmc)
{
  int rv = BCM_E_NONE;

  bcm_ipmc_addr_t_init(bcm_ipmc);
 
  bcm_ipmc->vid = dtl_ipmc->vid;
   
  /* This addr are initialized explicitly */
  if (dtl_ipmc->flags & PTIN_BCM_IPMC_IP6)
  {
    memcpy(&bcm_ipmc->s_ip6_addr,  &dtl_ipmc->s_ip_addr.addr.ipv6.in6.addr8,  L7_IPV6_ADDRESS_SIZE);
    memcpy(&bcm_ipmc->mc_ip6_addr, &dtl_ipmc->mc_ip_addr.addr.ipv6.in6.addr8, L7_IPV6_ADDRESS_SIZE);
    bcm_ipmc->flags |= BCM_IPMC_IP6; /* Set the flag for ipv6 */
  }
  else
  {
    memcpy(&(bcm_ipmc->s_ip_addr),  &dtl_ipmc->s_ip_addr.addr.ipv4.s_addr,  L7_IPV4_ADDRESS_SIZE);
    memcpy(&(bcm_ipmc->mc_ip_addr), &dtl_ipmc->mc_ip_addr.addr.ipv4.s_addr, L7_IPV4_ADDRESS_SIZE);
  }
    
//bcm_ipmc->cos = ipmc->cos;
//bcm_ipmc->ts = ipmc->ts;
//bcm_ipmc->port_tgid = ipmc->port_tgid;
//bcm_ipmc->mod_id = ipmc->mod_id;
  bcm_ipmc->v = 1;   /* VALID */

  /*Flags*/  
  if ( (dtl_ipmc->flags & PTIN_BCM_IPMC_REPLACE) == PTIN_BCM_IPMC_REPLACE )
    bcm_ipmc->flags |= BCM_IPMC_REPLACE;

  //Always Force to Not Check Source Port 
  bcm_ipmc->flags |= BCM_IPMC_SOURCE_PORT_NOCHECK;
  /*End Flags*/

  bcm_ipmc->group = dtl_ipmc->group_index;

//bcm_ipmc->vrf = ipmc->vrf;
//bcm_ipmc->lookup_class = ipmc->lookup_class;
//bcm_ipmc->distribution_class = ipmc->distribution_class;

  /* Invalid id */
  if (dtl_ipmc->group_index <= 0)
  {
    rv = BCM_E_BADID;
  }  

  return rv;
}

/*********************************************************************
* @purpose  Add a IP Mcast entry
*
* @param    data     @{(input)} The address and info to add to IP Mcast Table
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
L7_int32 ptin_debug_bcm_ipmc_add(L7_int ipmc_index, L7_uint32 vlanId, L7_uint32 groupAddr, L7_uint32 sourceAddr, L7_BOOL portNoCheck, L7_BOOL replace)
{
  ptin_dtl_ipmc_addr_t ptin_ipmc;
  bcm_ipmc_addr_t      bcm_ipmc;

  /*Initialize Struct*/
  memset(&ptin_ipmc, 0X00, sizeof(ptin_ipmc));

//hapiBroadL3McastSetIpAddress(&(entry->srcAddr),&(entry->groupIp),&ipmc_info);

  memcpy(&ptin_ipmc.s_ip_addr.addr.ipv4.s_addr, &sourceAddr, sizeof(sourceAddr));
  memcpy(&ptin_ipmc.mc_ip_addr.addr.ipv4.s_addr, &groupAddr, sizeof(groupAddr));
  ptin_ipmc.vid = vlanId;
  ptin_ipmc.group_index = ipmc_index;

  if (portNoCheck)
    ptin_ipmc.flags |= PTIN_BCM_IPMC_SOURCE_PORT_NOCHECK;

  if (replace)
    ptin_ipmc.flags |= PTIN_BCM_IPMC_REPLACE;

  ptin_ipmc_to_bcm(&ptin_ipmc, &bcm_ipmc);

  return bcm_ipmc_add(0, &bcm_ipmc);        
}

static uint32 numberOfIpmcEntries = 0;

uint32 ptin_hapi_l3_ipmc_number_of_entries_get(void)
{
  return numberOfIpmcEntries;
}

/*********************************************************************
* @purpose  Add a IP Mcast entry
*
* @param    ptin_ipmc     @{(input)} The address and info to add to IP Mcast Table
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
L7_RC_t ptin_hapi_l3_ipmc_add(ptin_dtl_ipmc_addr_t *ptin_ipmc)
{  
  bcm_ipmc_addr_t      bcm_ipmc;
  bcm_multicast_t      group;
  int                  rv = L7_SUCCESS;
  L7_BOOL              create_group;
  L7_uint32            flags;

  /* Default flags */
  flags = BCM_MULTICAST_TYPE_L3;
        
  /* Create group, by default */
  create_group = L7_TRUE;

  /* Check if provided group is valid */
  if ( ptin_ipmc->group_index > 0)
  {
    #if 0//Not Working!
    group = ptin_ipmc->group_index;
    rv = bcm_multicast_group_is_free(0, group);
    if (rv != BCM_E_NONE || rv != BCM_E_EXISTS)
    {
      PT_LOG_ERR(LOG_CTX_HAPI,"Invalid group index:%0x%08x rv:%d (\"%s\")", group, rv, bcm_errmsg(rv));
      return L7_FAILURE;
    }

    /* If group id exists, no need to create it */
    if (rv == BCM_E_EXISTS)
    {
      create_group = L7_FALSE;
    }
    /* Otherwise, create group with specific group id */
    else
    {
      flags |= BCM_MULTICAST_WITH_ID;
    }
    #else
    create_group = L7_FALSE;
    #endif
  }

  PT_LOG_TRACE(LOG_CTX_HAPI,"Group id %d will %s created (flags=0x%08x)",ptin_ipmc->group_index,((create_group) ? "BE" : "NOT be"),flags);

  /* Create group, if necessary */
  if (create_group)
  {
    rv = bcm_multicast_create(0, flags, &group);
    if ( (rv != BCM_E_NONE && rv != BCM_E_EXISTS) || group <= 0)
    {
      PT_LOG_ERR(LOG_CTX_HAPI,"Error with bcm_multicast_create(0x%x, group_index:0x%08x rv:%d) (\"%s\")",
              BCM_MULTICAST_TYPE_L3, group, rv, bcm_errmsg(rv));
      return ptin_bcm_to_fp_error_code(rv);
    }
    PT_LOG_TRACE(LOG_CTX_HAPI,"Group id 0x%x created (with flags=0x%08x)",group,flags);
    ptin_ipmc->group_index = group;
  }

  rv = ptin_ipmc_to_bcm(ptin_ipmc, &bcm_ipmc);

  if (BCM_FAILURE(rv))
  {
    PT_LOG_ERR(LOG_CTX_HAPI,"Error converting from dtl to bcm: group_index:0x%x rv:%d rv=\"%s\"", bcm_ipmc.group, rv, bcm_errmsg(rv));
    return ptin_bcm_to_fp_error_code(rv);
  }

  rv = bcm_ipmc_add(0, &bcm_ipmc);        

  if (BCM_FAILURE(rv))
  {
    PT_LOG_ERR(LOG_CTX_HAPI,"Error adding Channel to IPMC Table: group_index:0x%x rv:%d rv=\"%s\" numberOfIpmcEntries=%u", bcm_ipmc.group, rv, bcm_errmsg(rv), numberOfIpmcEntries);
    return ptin_bcm_to_fp_error_code(rv);
  }
  else
  {
    numberOfIpmcEntries++;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Remove a IP Mcast entry
*
* @param    ptin_ipmc     @{(input)} The address and info to add to IP Mcast Table
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
L7_RC_t ptin_hapi_l3_ipmc_remove(ptin_dtl_ipmc_addr_t *ptin_ipmc)
{  
  bcm_ipmc_addr_t      bcm_ipmc;
  int                  rv = L7_SUCCESS;

  rv = ptin_ipmc_to_bcm(ptin_ipmc, &bcm_ipmc);

  if (BCM_FAILURE(rv))
  {
    PT_LOG_ERR(LOG_CTX_HAPI,"Error converting from dtl to bcm: group_index:0x%x rv:%d rv=\"%s\"", bcm_ipmc.group, rv, bcm_errmsg(rv));
    return ptin_bcm_to_fp_error_code(rv);
  }

  /*Always Remove Entry*/
  bcm_ipmc.flags &= ~BCM_IPMC_KEEP_ENTRY;

//bcm_ipmc.group = 0;

  rv = bcm_ipmc_remove(0, &bcm_ipmc);        

  if (BCM_FAILURE(rv))
  {
    PT_LOG_ERR(LOG_CTX_HAPI,"Error removing Channel from IPMC Table: rv=\"%s\" ipmc_index:0x%x numberOfIpmcEntries:%u", bcm_errmsg(rv), ptin_ipmc->group_index, numberOfIpmcEntries);
    return ptin_bcm_to_fp_error_code(rv);
  }
  else
  {
    numberOfIpmcEntries--;
  }


  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get a IP Mcast entry
*
* @param    ptin_ipmc     @{(input)} The address and info to add to IP Mcast Table
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
L7_RC_t ptin_hapi_l3_ipmc_get(ptin_dtl_ipmc_addr_t *ptin_ipmc)
{  
   return L7_NOT_IMPLEMENTED_YET;
}

/*********************************************************************
* @purpose  Reset IPMC Table
*
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
L7_RC_t ptin_hapi_l3_ipmc_reset(void)
{
  int   rv = L7_SUCCESS;
 
  /*Initialize IPMC Table*/   
  rv = bcm_ipmc_remove_all(0);
  if (BCM_FAILURE(rv))
  {
    PT_LOG_ERR(LOG_CTX_HAPI,"Error initializing IPMC Table: rv=\"%s\" (rv:%u)", bcm_errmsg(rv));
    return ptin_bcm_to_fp_error_code(rv);
  }
  return rv;
}
  
#include <bcm/qos.h>
#undef BCM_IF_ERROR_RETURN
#define BCM_IF_ERROR_RETURN(op) {bcm_error_t r; r=op; PT_LOG_ERR(LOG_CTX_HAPI,"error: %d", r); if (r) return r;}
  
/*********************************************************************
* @purpose  Create an L3 Interface
*
* @param    intf          @{(input)} Interface attributes
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
L7_RC_t ptin_hapi_l3_intf_create (ptin_dtl_l3_intf_t *intf)
{
  usl_bcm_l3_intf_t intfInfo;
  int               rv = L7_SUCCESS;

  /*Validate Input Parameters*/
  if (intf == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_HAPI,"Invalid Arguments :%p", intf);
    return L7_FAILURE;
  }

  /*Initialize Struct*/
  bcm_l3_intf_t_init(&(intfInfo.bcm_data));

  /*Mac Address*/  
  memcpy(&intfInfo.bcm_data.l3a_mac_addr, &intf->mac_addr, sizeof(intfInfo.bcm_data.l3a_mac_addr));

  /*VLAN ID*/
  intfInfo.bcm_data.l3a_vid = intf->vid;

  /*MTU*/
  intfInfo.bcm_data.l3a_mtu = intf->mtu;

  /*Flags*/
  if ( (intf->flags & PTIN_BCM_L3_ADD_TO_ARL) == PTIN_BCM_L3_ADD_TO_ARL)
  {
    intfInfo.bcm_data.l3a_intf_flags |= BCM_L3_ADD_TO_ARL;
  }

#if 0
  bcm_qos_map_t l2_map;
  int l2_map_id;
  L7_uint32 flags, pri;

  /* Egress mapping profiles */
  flags = BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_L3;
  BCM_IF_ERROR_RETURN(bcm_qos_map_create(0, flags, &l2_map_id));

  for (pri = 0; pri < 8; pri++) {

    bcm_qos_map_t_init(&l2_map);
    /* In */
    l2_map.color = bcmColorGreen;
    l2_map.int_pri = 0;

    /* Out */
    l2_map.pkt_pri = pri;
    /* If packet is not green, set the CFI bit. */
    l2_map.pkt_cfi = 0;

    BCM_IF_ERROR_RETURN(bcm_qos_map_add(0, flags, &l2_map, l2_map_id));
  }

  intfInfo.bcm_data.vlan_qos.flags = BCM_L3_INTF_QOS_OUTER_VLAN_PRI_SET;
  intfInfo.bcm_data.vlan_qos.pri  = 0;
  intfInfo.bcm_data.vlan_qos.cfi  = 0;
  intfInfo.bcm_data.vlan_qos.dscp = 0;
  intfInfo.bcm_data.vlan_qos.qos_map_id = l2_map_id;
#endif

  rv = usl_bcmx_l3_intf_create(&intfInfo);
  if (BCM_FAILURE(rv) || intfInfo.bcm_data.l3a_intf_id == HAPI_BROAD_INVALID_L3_INTF_ID)
  {
    PT_LOG_ERR(LOG_CTX_HAPI,"Error creating L3 interface: rv=\"%s\" l3_intf_id:%d", bcm_errmsg(rv), intfInfo.bcm_data.l3a_intf_id);
    return ptin_bcm_to_fp_error_code(rv);
  }

  /*Save L3 Interface Interface Id*/
  intf->l3_intf_id = intfInfo.bcm_data.l3a_intf_id;

  return L7_SUCCESS;  
}


int ptin_hapi_debug_bcm_l3_intf_create(L7_uint32 vid, L7_uint32 mac_addr, L7_uint32 intf_id, L7_uint32 mtu)
{
  ptin_dtl_l3_intf_t intf;

  /*Initialize Struct*/
  memset(&intf, 0X00, sizeof(intf));

  memcpy(&intf.mac_addr, &mac_addr, sizeof(mac_addr));
 
  intf.vid = vid;

  intf.mtu = mtu;

  if (intf_id != (L7_uint32) -1)
  {
    intf.flags |= PTIN_BCM_L3_WITH_ID;
    intf.l3_intf_id = intf_id;
  }
  else
  {
    intf.l3_intf_id = HAPI_BROAD_INVALID_L3_INTF_ID;
  }

  intf.flags |= PTIN_BCM_L3_ADD_TO_ARL;

  return (ptin_hapi_l3_intf_create(&intf));
}

/*********************************************************************
* @purpose  Delete an L3 Interface
*
* @param    intf          @{(input)} Interface attributes
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
L7_RC_t ptin_hapi_l3_intf_delete (ptin_dtl_l3_intf_t *intf)
{
  usl_bcm_l3_intf_t intfInfo;
  int               rv = L7_SUCCESS;

  /*Validate Input Parameters*/
  if (intf == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_HAPI,"Invalid Arguments :%p", intf);
    return L7_FAILURE;
  }

  /*Initialize Struct*/
  bcm_l3_intf_t_init(&(intfInfo.bcm_data));

  if ( (intf->flags & PTIN_BCM_L3_WITH_ID) == PTIN_BCM_L3_WITH_ID)
  {
    intfInfo.bcm_data.l3a_intf_flags |= BCM_L3_WITH_ID;
  }
  else
  {
    PT_LOG_ERR(LOG_CTX_HAPI,"Flag Not set: l3_intf_id:%d", intf->l3_intf_id);
    return L7_FAILURE;
  }

  /*L3 Interface Id*/
  intfInfo.bcm_data.l3a_intf_id = intf->l3_intf_id;

  rv = usl_bcmx_l3_intf_delete(&intfInfo);
  if (BCM_FAILURE(rv))
  {
    PT_LOG_ERR(LOG_CTX_HAPI,"Error removing L3 interface: (rv=%d) rv=\"%s\" l3_intf_id:%d", rv, bcm_errmsg(rv), intf->l3_intf_id);
    return ptin_bcm_to_fp_error_code(rv);
  }

  return L7_SUCCESS;  
}

/*********************************************************************
* @purpose  Get an L3 Interface
*
* @param    intf          @{(input)} Interface attributes
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
L7_RC_t ptin_hapi_l3_intf_get (ptin_dtl_l3_intf_t *intf)
{
  return L7_NOT_IMPLEMENTED_YET;
}
