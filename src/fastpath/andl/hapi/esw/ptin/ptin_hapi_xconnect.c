#include <bcm/error.h>
#include <bcmx/vlan.h>
#include <bcmx/switch.h>
#include <bcmx/multicast.h>

#include "ptin_hapi_xconnect.h"
#include "logger.h"
#include "ipc.h"

/********************************************************************
 * DEFINES
 ********************************************************************/

/* Max number of translations */
#define FREE_RESOURCES_CROSSCONNECTS  8000

/********************************************************************
 * TYPES DEFINITION
 ********************************************************************/

/********************************************************************
 * INTERNAL VARIABLES
 ********************************************************************/

/* Available resources for translation: This is for the 5668x switches */
L7_uint16 resources_crossconnects = FREE_RESOURCES_CROSSCONNECTS;

#if 1
typedef struct
{
  L7_uint8  mac_counter;
  L7_uint16 mac_total;
  L7_uint8  mac_maximum;

  L7_BOOL     trap_sent;
  ptin_intf_t ptin_intf;    // PON interface
  L7_uint16   uni_ovid;     // GEM id
} mac_learn_info_t;

#define MAX_VLANS   4096
#define MAX_GPORTS  8192

static mac_learn_info_t macLearn_info_vlan[MAX_VLANS+1];
static mac_learn_info_t macLearn_info_flow[MAX_GPORTS+1];
#endif

/********************************************************************
 * EXTERNAL FUNCTIONS IMPLEMENTATION
 ********************************************************************/

/**
 * Initialize bridge module
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_hapi_bridge_init(void)
{
  int error;

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "bcm_switch_control_set(%d,%d,1)", bcm_unit, bcmSwitchSharedVlanEnable);

  /* Enable Shared Vlan for using one single vlan in E-Trees when learning MACs */
  if ( (error = bcmx_switch_control_set( bcmSwitchSharedVlanEnable, L7_TRUE )) != BCM_E_NONE )
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Error enabling shared vlan: error=%d (%s)", error, bcm_errmsg(error));
    return L7_FAILURE;
  }

  #if 0
  if ( (error=ptin_hapi_macaddr_init()) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Error initializing MAC learning control: error=%d (%s)", error, bcm_errmsg(error));
    return L7_FAILURE;
  }
  #endif

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "ptin_hapi_bridge_init returned success");

  return L7_SUCCESS;
}

/**
 * Get available resources for cross-connections
 * 
 * @param crossconnects: pointer to variable where will contain 
 *               the number of cross-connections.
 * @return L7_RC_t: always L7_SUCCESS
 */
L7_RC_t ptin_hapi_bridge_free_resources(L7_uint16 *crossconnects)
{
  if (crossconnects != L7_NULLPTR)
  {
    *crossconnects = resources_crossconnects;
    LOG_TRACE(LOG_CTX_PTIN_HAPI, "Free cross-connections consulted: %u", *crossconnects);
  }

  return L7_SUCCESS;
}

/**
 * Define vlan related parameters, for bridging purposes
 * 
 * @param vlanId :    vlan to be configured
 * @param fwdVlanId : forward vlan to be used when learning MAC 
 *                    addresses
 * @param cross_connects_apply: Use cross-connects to this vlan?
 * @param mac_learning_apply:   Apply mac learning to this vlan?
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_hapi_bridge_vlan_mode_set(L7_uint16 vlanId, L7_uint16 fwdVlanId, L7_BOOL cross_connects_apply, L7_BOOL mac_learning_apply, L7_int mc_group)
{
  int error;
  bcm_vlan_control_vlan_t control;

  /* Get current control definitions for this vlan */
  bcm_vlan_control_vlan_t_init(&control);
  if ((error = bcmx_vlan_control_vlan_get(vlanId, &control))!=BCM_E_NONE)
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Error getting vlan control structure! error=%d (%s)\r\n", error, bcm_errmsg(error));
    return L7_FAILURE;
  }

  /* Forwarding vlan, for MAC learning purposes (only if fwdvlan is valid) */
  if ( fwdVlanId > 0 && fwdVlanId <= 4095 )
  {
    control.forwarding_vlan = fwdVlanId;
  }

  /* Learn on this vlan */
  if (!mac_learning_apply)
  {
    control.flags |= BCM_VLAN_LEARN_DISABLE;
  }
  else
  {
    control.flags &= ~((uint32) BCM_VLAN_LEARN_DISABLE);
  }

  /* Non unicast packets will be droped */
  //control.flags |= BCM_VLAN_NON_UCAST_DROP;

  /* Associate a MC group */
  if (mc_group > 0)
  {
    control.broadcast_group = mc_group;
    control.unknown_multicast_group = mc_group;
    control.unknown_unicast_group = mc_group;
  }

  /* Forwarding mode */
  control.forwarding_mode = (cross_connects_apply) ?  bcmVlanForwardDoubleCrossConnect : bcmVlanForwardBridging;

  /* Apply new control definitions to this vlan */
  if ( (error = bcmx_vlan_control_vlan_set(vlanId, control)) != BCM_E_NONE )
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Error with bcm_vlan_control_vlan_set: error=%d (%s)", error, bcm_errmsg(error));
    return L7_FAILURE;
  }

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "ptin_hapi_bridge_vlan_mode_set returned success");

  return L7_SUCCESS;
}

/**
 * Define forward vlanId for a specific vlan for bridging 
 * purposes 
 * 
 * @param vlanId :    vlan to be configured
 * @param fwdVlanId : forward vlan to be used when learning MAC 
 *                    addresses
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_hapi_bridge_vlan_mode_fwdVlan_set(L7_uint16 vlanId, L7_uint16 fwdVlanId)
{
  int error;
  bcm_vlan_control_vlan_t control;

  /* Forwarding vlan, for MAC learning purposes (only if fwdvlan is valid) */
  if ( fwdVlanId == 0 || fwdVlanId > 4095 )
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Invalid fwd vlan id: %u",fwdVlanId);
    return L7_FAILURE;
  }

  /* Get current control definitions for this vlan */
  bcm_vlan_control_vlan_t_init(&control);
  if ((error = bcmx_vlan_control_vlan_get(vlanId, &control))!=BCM_E_NONE)
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Error getting vlan control structure! error=%d (%s)", error, bcm_errmsg(error));
    return L7_FAILURE;
  }
  
  /* Reset forward vlan */
  control.forwarding_vlan = fwdVlanId;

  /* Apply new control definitions to this vlan */
  if ( (error = bcmx_vlan_control_vlan_set(vlanId, control)) != BCM_E_NONE )
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Error with bcm_vlan_control_vlan_set: error=%d (%s)", error, bcm_errmsg(error));
    return L7_FAILURE;
  }

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "ptin_hapi_bridge_vlan_mode_fwdVlan_set returned success");

  return L7_SUCCESS;
}

/**
 * Define the outer tpid of a specific vlan, for bridging 
 * purposes 
 * 
 * @param vlanId :    vlan to be configured
 * @param outer_tpid : TPID of the outer tag
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_hapi_bridge_vlan_mode_outerTpId_set(L7_uint16 vlanId, L7_uint16 outer_tpid)
{
  int error;
  bcm_vlan_control_vlan_t control;

  /* Validate outer TPID */
  if ( outer_tpid == 0x0000 )
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "ERROR: TPID is null");
    return L7_FAILURE;
  }

  /* Get current control definitions for this vlan */
  bcm_vlan_control_vlan_t_init(&control);
  if ((error = bcmx_vlan_control_vlan_get(vlanId, &control))!=BCM_E_NONE)
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Error getting vlan control structure! error=%d (%s)\r\n", error, bcm_errmsg(error));
    return L7_FAILURE;
  }

  /* Forwarding mode */
  control.outer_tpid = outer_tpid;

  /* Apply new control definitions to this vlan */
  if ( (error = bcmx_vlan_control_vlan_set(vlanId, control)) != BCM_E_NONE )
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Error with bcm_vlan_control_vlan_set: error=%d (%s)", error, bcm_errmsg(error));
    return L7_FAILURE;
  }

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "ptin_hapi_bridge_vlan_mode_outerTpId_set returned success");

  return L7_SUCCESS;
}

/**
 * (Dis)Enable MAC learning for a specific vlan
 * 
 * @param vlanId :    vlan to be configured
 * @param mac_learning_apply:   Apply mac learning to this vlan?
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_hapi_bridge_vlan_mode_macLearn_set(L7_uint16 vlanId, L7_BOOL mac_learning_apply)
{
  int error;
  bcm_vlan_control_vlan_t control;

  /* Get current control definitions for this vlan */
  bcm_vlan_control_vlan_t_init(&control);
  if ((error = bcmx_vlan_control_vlan_get(vlanId, &control))!=BCM_E_NONE)
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Error getting vlan control structure! error=%d (%s)\r\n", error, bcm_errmsg(error));
    return L7_FAILURE;
  }

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "Control flags = 0x%08x",control.flags);

  /* Learn on this vlan */
  if (!mac_learning_apply)
  {
    control.flags |= BCM_VLAN_LEARN_DISABLE /*| BCM_VLAN_L2_LOOKUP_DISABLE*/;

    /* Let multicast addresses to be flooded */
    //control.l2_mcast_flood_mode  = BCM_VLAN_MCAST_FLOOD_ALL;
    //control.ip4_mcast_flood_mode = BCM_VLAN_MCAST_FLOOD_ALL;
    //control.ip6_mcast_flood_mode = BCM_VLAN_MCAST_FLOOD_ALL;
  }
  else
  {
    control.flags &= ~((uint32) BCM_VLAN_LEARN_DISABLE /*| BCM_VLAN_L2_LOOKUP_DISABLE*/);

    /* Let multicast addresses to be flooded */
    //control.l2_mcast_flood_mode  = BCM_VLAN_MCAST_FLOOD_NONE;
    //control.ip4_mcast_flood_mode = BCM_VLAN_MCAST_FLOOD_NONE;
    //control.ip6_mcast_flood_mode = BCM_VLAN_MCAST_FLOOD_NONE;
  }

  /* Apply new control definitions to this vlan */
  if ( (error = bcmx_vlan_control_vlan_set(vlanId, control)) != BCM_E_NONE )
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Error with bcm_vlan_control_vlan_set: error=%d (%s)", error, bcm_errmsg(error));
    return L7_FAILURE;
  }

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "ptin_hapi_bridge_vlan_mode_macLearn_set returned success");

  return L7_SUCCESS;
}

/**
 * Change bridging vlan mode (cross-connects enabler)
 * 
 * @param vlanId :    vlan to be configured
 * @param cross_connects_apply: Use cross-connects to this vlan?
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_hapi_bridge_vlan_mode_crossconnect_set(L7_uint16 vlanId, L7_BOOL cross_connects_apply, L7_BOOL double_tag)
{
  int error;
  bcm_vlan_control_vlan_t control;

  /* Get current control definitions for this vlan */
  bcm_vlan_control_vlan_t_init(&control);
  if ((error = bcmx_vlan_control_vlan_get(vlanId, &control))!=BCM_E_NONE)
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Error getting vlan control structure! error=%d (%s)\r\n", error, bcm_errmsg(error));
    return L7_FAILURE;
  }

  /* Forwarding mode */
  if (cross_connects_apply)
  {
    control.flags |= BCM_VLAN_LEARN_DISABLE;
    control.forwarding_mode = (double_tag) ? bcmVlanForwardDoubleCrossConnect : bcmVlanForwardSingleCrossConnect;
  }
  else
  {
    control.flags &= ~((uint32) BCM_VLAN_LEARN_DISABLE);
    control.forwarding_mode = bcmVlanForwardBridging;
  }

  /* Apply new control definitions to this vlan */
  if ( (error = bcmx_vlan_control_vlan_set(vlanId, control)) != BCM_E_NONE )
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Error with bcm_vlan_control_vlan_set: error=%d (%s)", error, bcm_errmsg(error));
    return L7_FAILURE;
  }

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "ptin_hapi_bridge_vlan_mode_crossconnect_set returned success (enable=%u, double=%u)", cross_connects_apply, double_tag);

  return L7_SUCCESS;
}

/**
 * Add a cross-connection between two ports
 * 
 * @param outerVlanId: outer vlan to look for
 * @param innerVlanId: inner vlan to look for (0 to not use)
 * @param dapiPort1: First port
 * @param dapiPort2: Second port 
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_hapi_bridge_crossconnect_add(L7_uint16 outerVlanId, L7_uint16 innerVlanId, ptin_dapi_port_t *dapiPort1, ptin_dapi_port_t *dapiPort2)
{
  int error;
  DAPI_PORT_t  *dapiPortPtr1, *dapiPortPtr2;
  BROAD_PORT_t *hapiPortPtr1, *hapiPortPtr2;

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "oVlanId=%u iVlanId=%u port1={%d,%d,%d} port2={%d,%d,%d}",
            outerVlanId,innerVlanId,
            dapiPort1->usp->unit,dapiPort1->usp->slot,dapiPort1->usp->port,
            dapiPort2->usp->unit,dapiPort2->usp->slot,dapiPort2->usp->port);

  if ( (dapiPort1->usp->unit<0 || dapiPort1->usp->slot<0 || dapiPort1->usp->port<0) ||
       (dapiPort2->usp->unit<0 || dapiPort2->usp->slot<0 || dapiPort2->usp->port<0) )
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "ERROR: Invalid interfaces");
    return L7_FAILURE;
  }

  /* Get port pointers */
  DAPIPORT_GET_PTR(dapiPort1, dapiPortPtr1, hapiPortPtr1);
  DAPIPORT_GET_PTR(dapiPort2, dapiPortPtr2, hapiPortPtr2);

  /* Accept only physical and lag interfaces */
  if ( ( !IS_PORT_TYPE_PHYSICAL(dapiPortPtr1) && !IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr1) ) ||
       ( !IS_PORT_TYPE_PHYSICAL(dapiPortPtr2) && !IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr2) ) )
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "ERROR: Ports ({%d,%d,%d} and {%d,%d,%d}) are not physical neither lag",
            dapiPort1->usp->unit,dapiPort1->usp->slot,dapiPort1->usp->port,
            dapiPort2->usp->unit,dapiPort2->usp->slot,dapiPort2->usp->port);
    return L7_FAILURE;
  }

  /* Validate vlans */
  if ( outerVlanId==0 || outerVlanId>4095 )
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Outer vlan is invalid: %u",outerVlanId);
    return L7_FAILURE;
  }

  /* If inner vlan is not valid, use the BCM_VLAN_INVALID */
  if ( innerVlanId == 0 || innerVlanId > 4095 )
  {
    innerVlanId = BCM_VLAN_INVALID;
  }

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "bcm_vlan_cross_connect_add(%d,%u,%u,%u({%d,%d,%d}), %u({%d,%d,%d}))",
            bcm_unit, outerVlanId, innerVlanId,
            hapiPortPtr1->bcmx_lport, dapiPort1->usp->unit, dapiPort1->usp->slot, dapiPort1->usp->port,
            hapiPortPtr2->bcmx_lport, dapiPort2->usp->unit, dapiPort2->usp->slot, dapiPort2->usp->port);

  error = bcmx_vlan_cross_connect_add(outerVlanId, innerVlanId, hapiPortPtr1->bcmx_lport, hapiPortPtr2->bcmx_lport);
  if ( error != BCM_E_NONE && error != BCM_E_EXISTS )
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Error with bcm_vlan_cross_connect_add: %d (%s)", error, bcm_errmsg(error));
  }

  /* Manager resources meter */
  if ( error == BCM_E_NONE )
  {
    if (resources_crossconnects<FREE_RESOURCES_CROSSCONNECTS)
      resources_crossconnects++;
  }

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "Finished successfully");

  return L7_SUCCESS;
}

/**
 * Delete a cross-connection
 * 
 * @param outerVlanId: Outer vlan to look for
 * @param innerVlanId: Inner vlan to look for
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_hapi_bridge_crossconnect_delete(L7_uint16 outerVlanId, L7_uint16 innerVlanId)
{
  int error;

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "oVlanId=%u iVlanId=%u",outerVlanId,innerVlanId);

  /* Validate vlans */
  if ( outerVlanId==0 || outerVlanId>4095 )
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Outer vlan is invalid: %u",outerVlanId);
    return L7_FAILURE;
  }

  /* If inner vlan is not valid, use the BCM_VLAN_INVALID */
  if ( innerVlanId == 0 || innerVlanId > 4095 )
  {
    innerVlanId = BCM_VLAN_INVALID;
  }

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "bcm_vlan_cross_connect_delete(%d,%u,%u)",bcm_unit,outerVlanId,innerVlanId);

  error = bcmx_vlan_cross_connect_delete(outerVlanId, innerVlanId);
  if ( error != BCM_E_NONE && error != BCM_E_NOT_FOUND )
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Error with bcm_vlan_cross_connect_delete: %d (%s)", error, bcm_errmsg(error));
  }

  /* Manager resources meter */
  if ( error == BCM_E_NONE )
  {
    if (resources_crossconnects>0)
      resources_crossconnects--;
  }

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "Finished successfully");

  return L7_SUCCESS;
}

/**
 * Deletes all configured cross-connections
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_hapi_bridge_crossconnect_delete_all(void)
{
  int error;

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "bcm_vlan_cross_connect_delete_all(%d)",bcm_unit);

  error = bcmx_vlan_cross_connect_delete_all();
  if ( error != BCM_E_NONE && error != BCM_E_NOT_FOUND )
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Error with bcm_vlan_cross_connect_delete: %d (%s)", error, bcm_errmsg(error));
  }

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "Finished successfully");

  return L7_SUCCESS;
}



/**
 * Create Virtual port
 * 
 * @param dapiPort      : PON port
 * @param match_ovid    : external outer vlan (GEMid)
 * @param match_ivid    : external inner vlan (UNIVLAN)
 * @param egress_ovid   : outer vlan inside switch
 * @param egress_ivid   : inner vlan inside switch 
 * @param mcast_group   : mc group (-1 to create) 
 * @param virtual_gport : vport id (to be returned) 
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_hapi_vp_create(ptin_dapi_port_t *dapiPort,
                            L7_uint16 match_ovid, L7_uint16 match_ivid,
                            L7_uint16 egress_ovid, L7_uint16 egress_ivid,
                            L7_int *mcast_group,
                            L7_int *virtual_gport)
{
  DAPI_PORT_t  *dapiPortPtr;
  BROAD_PORT_t *hapiPortPtr;
  bcm_multicast_t mc_group;
  bcm_error_t error;
  bcm_multicast_t encap_id;

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "port={%d,%d,%d}, oVlanId=%u iVlanId=%u => oVlanId=%u iVlanId=%u",
            dapiPort->usp->unit,dapiPort->usp->slot,dapiPort->usp->port,
            match_ovid, match_ivid, egress_ovid, egress_ivid);

  /* Validate interface */
  if ( dapiPort->usp->unit<0 || dapiPort->usp->slot<0 || dapiPort->usp->port<0 )
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "ERROR: Invalid interfaces");
    return L7_FAILURE;
  }

  /* Get port pointers */
  DAPIPORT_GET_PTR(dapiPort, dapiPortPtr, hapiPortPtr);

  /* Accept only physical interfaces */
  if ( !IS_PORT_TYPE_PHYSICAL(dapiPortPtr) )
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "ERROR: Port {%d,%d,%d} are not physical",
            dapiPort->usp->unit,dapiPort->usp->slot,dapiPort->usp->port);
    return L7_FAILURE;
  }

  /* create the virtual ports */
  bcm_vlan_port_t vlan_port;
  bcm_vlan_port_t_init(&vlan_port);

  /* in direction PON -> network, match on stacked VLAN, translate to client ID on ingress */
  if (egress_ivid > 0 && egress_ivid < 4096)
  {
    vlan_port.flags = BCM_VLAN_PORT_INNER_VLAN_ADD | BCM_VLAN_PORT_INNER_VLAN_REPLACE;
  }
  vlan_port.match_vlan = match_ovid;
  vlan_port.match_inner_vlan = match_ivid;
  vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED;
  vlan_port.egress_vlan = egress_ovid;
  vlan_port.egress_inner_vlan = egress_ivid;
  BCM_GPORT_LOCAL_SET(vlan_port.port, hapiPortPtr->bcm_port);

  LOG_NOTICE(LOG_CTX_PTIN_HAPI, "bcm_port=%d vlan_port.port=%d vport=%d", hapiPortPtr->bcm_port, vlan_port.port, vlan_port.vlan_port_id);

  if ((error=bcm_vlan_port_create(0, &vlan_port)) != BCM_E_NONE)
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Error with bcm_vlan_port_create: error=%d (\"%s\")", error, bcm_errmsg(error));
    return L7_FAILURE;
  }

  LOG_NOTICE(LOG_CTX_PTIN_HAPI, "vport=0x%x", vlan_port.vlan_port_id);

  #if 0
  /* MAC learning */
  error = BCM_E_NONE;
  do
  {
    if ((error=bcm_port_learn_set(0, vlan_port.vlan_port_id, BCM_PORT_LEARN_PENDING | BCM_PORT_LEARN_ARL )) != BCM_E_NONE)
    {
      LOG_ERR(LOG_CTX_PTIN_HAPI, "Error with bcm_port_learn_set: error=%d (\"%s\")", error, bcm_errmsg(error));
      break;
    }
    if ((error=bcm_port_control_set(0, vlan_port.vlan_port_id, bcmPortControlL2Learn, BCM_PORT_LEARN_PENDING | BCM_PORT_LEARN_ARL)) != BCM_E_NONE)
    {
      LOG_ERR(LOG_CTX_PTIN_HAPI, "Error with bcm_port_control_set: error=%d (\"%s\")", error, bcm_errmsg(error));
      break;
    }
    if ((error=bcm_port_control_set(0, vlan_port.vlan_port_id, bcmPortControlL2Move,  BCM_PORT_LEARN_PENDING | BCM_PORT_LEARN_ARL)) != BCM_E_NONE)
    {
      LOG_ERR(LOG_CTX_PTIN_HAPI, "Error with bcm_port_control_set: error=%d (\"%s\")", error, bcm_errmsg(error));
      break;
    }
    if ((error=bcm_port_control_set(0, vlan_port.vlan_port_id, bcmPortControlLearnClassEnable, L7_ENABLE)) != BCM_E_NONE)
    {
      LOG_ERR(LOG_CTX_PTIN_HAPI, "Error with bcm_port_control_set: error=%d (\"%s\")", error, bcm_errmsg(error));
      break;
    }
    if ((error=bcm_l2_learn_class_set(0, 1, 1, BCM_L2_LEARN_CLASS_MOVE)) != BCM_E_NONE)
    {
      LOG_ERR(LOG_CTX_PTIN_HAPI, "Error with bcm_l2_learn_class_set: error=%d (\"%s\")", error, bcm_errmsg(error));
      break;
    }
    if ((error=bcm_l2_learn_port_class_set(0, vlan_port.vlan_port_id, 1)) != BCM_E_NONE)
    {
      LOG_ERR(LOG_CTX_PTIN_HAPI, "Error with bcm_l2_learn_port_class_set: error=%d (\"%s\")", error, bcm_errmsg(error));
      break;
    }
  } while (0);

  if (error != BCM_E_NONE)
  {
    bcm_vlan_port_destroy(0, vlan_port.vlan_port_id);
    LOG_ERR(LOG_CTX_PTIN_HAPI, "error=%d (\"%s\")", error, bcm_errmsg(error));
    return L7_FAILURE;
  }
  #endif

  /* Configures the information needed to generate alarms related to MAC Limit */
  ptin_hapi_macaddr_config(match_ovid, vlan_port.vlan_port_id, hapiPortPtr->bcm_port, match_ovid);

  /* create egress translation entries for virtual ports to do VLAN tag manipulation 
   * i.e. client -> gem_id + some_c_vlan */
  bcm_vlan_action_set_t action;
  bcm_vlan_action_set_t_init(&action);
  
  /* for outer tagged packet => outer tag replaced with gem_id */
  action.ot_outer = bcmVlanActionReplace;
  action.dt_outer = bcmVlanActionReplace;
  action.new_outer_vlan = match_ovid;
  
  /* for outer tagged packet => inner tag added with cvid */
  action.ot_inner = bcmVlanActionAdd;
  action.dt_inner = bcmVlanActionReplace;
  action.new_inner_vlan = match_ivid;

  if ((error=bcm_vlan_translate_egress_action_add(0, vlan_port.vlan_port_id, egress_ovid, 0, &action))!=BCM_E_NONE)
  {
    bcm_vlan_port_destroy(0, vlan_port.vlan_port_id);
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Error with bcm_vlan_translate_egress_action_add(0, %d, %d, %d, &action): error=%d (\"%s\")",
           vlan_port.vlan_port_id, egress_ovid, 0, error, bcm_errmsg(error));
    return L7_FAILURE;
  }

  /* Get given MC group id */
  mc_group = *mcast_group;

  /* Create a multicast group, if given multicast group is not valid */
  if ( mc_group <= 0 )
  {
    if ((error=bcm_multicast_create(0, BCM_MULTICAST_TYPE_VLAN, &mc_group)) != BCM_E_NONE)
    {
      LOG_ERR(LOG_CTX_PTIN_HAPI,"Error with bcm_multicast_create(0, %d, &mcast_group): error=%d (\"%s\")",
              BCM_MULTICAST_TYPE_VLAN, error, bcm_errmsg(error));
      return L7_FAILURE;
    }
    *mcast_group = mc_group;
  }

  LOG_NOTICE(LOG_CTX_PTIN_HAPI, "mc_group=%d vlan_port.port=%d vport=%d", mc_group, vlan_port.port, vlan_port.vlan_port_id);

  /* Add virtual port to multicast group */
  if ((error=bcm_multicast_vlan_encap_get(0, mc_group, vlan_port.port, vlan_port.vlan_port_id, &encap_id))!=BCM_E_NONE)
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Error with bcm_multicast_vlan_encap_get: error=%d (\"%s\")",error, bcm_errmsg(error));
    return L7_FAILURE;
  }
  if ((error=bcm_multicast_egress_add(0, mc_group, vlan_port.port, encap_id))!=BCM_E_NONE)
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Error with bcm_multicast_egress_add: error=%d (\"%s\")",error, bcm_errmsg(error));
    return L7_FAILURE;
  }

  /* Return vport id */
  if (virtual_gport != L7_NULLPTR)
    *virtual_gport = vlan_port.vlan_port_id;

  LOG_INFO(LOG_CTX_PTIN_HAPI, "ptin_hapi_vp_create: vport 0x%x created", vlan_port.vlan_port_id);

  return L7_SUCCESS;
}

/**
 * Remove virtual port
 * 
 * @param dapiPort      : PON port
 * @param match_ovid    : external Outer vlan (GEMid)
 * @param match_ivid    : external inner vlan (UNIVLAN) 
 * @param virtual_gport : vport id 
 * @param mcast_group   : multicast group
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_hapi_vp_remove(ptin_dapi_port_t *dapiPort,
                            L7_uint16 match_ovid, L7_uint16 match_ivid,
                            L7_int virtual_gport,
                            L7_int mcast_group)
{
  DAPI_PORT_t  *dapiPortPtr;
  BROAD_PORT_t *hapiPortPtr;
  bcm_multicast_t encap_id;
  bcm_error_t error;
  L7_RC_t rc = L7_SUCCESS;

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "port={%d,%d,%d}, oVlanId=%u iVlanId=%u",
            dapiPort->usp->unit,dapiPort->usp->slot,dapiPort->usp->port,
            match_ovid, match_ivid);

  /* Validate interface */
  if ( dapiPort->usp->unit<0 || dapiPort->usp->slot<0 || dapiPort->usp->port<0 )
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "ERROR: Invalid interfaces");
    return L7_FAILURE;
  }

  /* Get port pointers */
  DAPIPORT_GET_PTR(dapiPort, dapiPortPtr, hapiPortPtr);

  /* Accept only physical interfaces */
  if ( !IS_PORT_TYPE_PHYSICAL(dapiPortPtr) )
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "ERROR: Port {%d,%d,%d} are not physical",
            dapiPort->usp->unit,dapiPort->usp->slot,dapiPort->usp->port);
    return L7_FAILURE;
  }

  /* Forwarding vlan, for MAC learning purposes (only if fwdvlan is valid) */
  if ( mcast_group <= 0 )
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Invalid MC group (%d)", mcast_group);
    return L7_FAILURE;
  }

  /* create the virtual ports */
  bcm_vlan_port_t vlan_port;
  bcm_vlan_port_t_init(&vlan_port);

  /* If virtual port id is provided, use it */
  if (virtual_gport > 0)
  {
    vlan_port.flags = BCM_VLAN_PORT_WITH_ID;
    vlan_port.vlan_port_id = virtual_gport;
  }
  else
  {
    /* in direction PON -> network, match on stacked VLAN, translate to client ID on ingress */
    vlan_port.flags = BCM_VLAN_PORT_INNER_VLAN_ADD | BCM_VLAN_PORT_INNER_VLAN_REPLACE;
    vlan_port.match_vlan = match_ovid;
    vlan_port.match_inner_vlan = match_ivid;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED;
    BCM_GPORT_LOCAL_SET(vlan_port.port, hapiPortPtr->bcm_port);
  }

  if ((error=bcm_vlan_port_find(0, &vlan_port)) != BCM_E_NONE)
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Error with bcm_vlan_port_find: error=%d (\"%s\")", error, bcm_errmsg(error));
    return L7_FAILURE;
  }

  LOG_NOTICE(LOG_CTX_PTIN_HAPI, "gport=0x%x, bcm_port=%d => vlan_port.port=%d vport=%d",
             virtual_gport, hapiPortPtr->bcm_port, vlan_port.port, vlan_port.vlan_port_id);
  LOG_NOTICE(LOG_CTX_PTIN_HAPI, "flags=0x%08x criteria=0x%08x match_vlan=%u match_ivlan=%u egress_vlan=%u egress_ivlan=%u",
             vlan_port.flags, vlan_port.criteria, vlan_port.match_vlan, vlan_port.match_inner_vlan, vlan_port.egress_vlan, vlan_port.egress_inner_vlan);

  /* Remove virtual port from multicast group */
  error = bcm_multicast_vlan_encap_get(0, mcast_group, vlan_port.port, vlan_port.vlan_port_id, &encap_id);
  if (error != BCM_E_NONE)
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Error with bcm_multicast_vlan_encap_get: error=%d (\"%s\")",error, bcm_errmsg(error));
    return L7_FAILURE;
  }
  error = bcm_multicast_egress_delete(0, mcast_group, vlan_port.port, encap_id);
  if (error != BCM_E_NONE && error != BCM_E_NOT_FOUND)
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Error with bcm_multicast_egress_delete: error=%d (\"%s\")",error, bcm_errmsg(error));
    return L7_FAILURE;
  }

  /* Remove egress translation entries */
  error = bcm_vlan_translate_egress_action_delete(0, vlan_port.vlan_port_id, vlan_port.egress_vlan, 0);
  if (error != BCM_E_NONE && error != BCM_E_NOT_FOUND)
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Error with bcmx_vlan_translate_egress_action_delete(%d, %d, %d, &action): error=%d (\"%s\")",
            vlan_port.vlan_port_id, vlan_port.egress_vlan, 0, error, bcm_errmsg(error));
    return L7_FAILURE;
  }

  /* Destroy virtual port */
  error = bcm_vlan_port_destroy(0, vlan_port.vlan_port_id);
  if (error != BCM_E_NONE && error != BCM_E_NOT_FOUND)
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Error with bcm_vlan_port_destroy: error=%d (\"%s\")", error, bcm_errmsg(error));
    return L7_FAILURE;
  }

  /* Remove MAC addresses related to this virtual port */
  error = bcm_l2_addr_delete_by_port(0, -1, vlan_port.vlan_port_id, 0);
  if (error != BCM_E_NONE && error != BCM_E_NOT_FOUND)
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Error removing MAC addresses related to this vport: error=%d (\"%s\")", error, bcm_errmsg(error));
  }

  LOG_INFO(LOG_CTX_PTIN_HAPI, "ptin_hapi_vp_remove: vport 0x%x removed", vlan_port.vlan_port_id);

  return L7_SUCCESS;
}

/**
 * Add port to egress multicast group
 * 
 * @param mcast_group : MC group id (-1 to create)
 * @param dapiPort    : Port to be added 
 *  
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_hapi_multicast_egress_port_add(L7_int *mcast_group, ptin_dapi_port_t *dapiPort)
{
  bcm_multicast_t mc_group;
  DAPI_PORT_t  *dapiPortPtr;
  BROAD_PORT_t *hapiPortPtr;
  bcm_multicast_t encap_id;
  bcm_error_t error;

  /* Forwarding vlan, for MAC learning purposes (only if fwdvlan is valid) */
  if ( mcast_group == L7_NULLPTR )
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Invalid MC group");
    return L7_FAILURE;
  }

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "port={%d,%d,%d}, mcast_group=%d",
            dapiPort->usp->unit,dapiPort->usp->slot,dapiPort->usp->port, mcast_group);

  /* Validate interface */
  if ( dapiPort->usp->unit<0 || dapiPort->usp->slot<0 || dapiPort->usp->port<0 )
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "ERROR: Invalid interfaces");
    return L7_FAILURE;
  }

  /* Get port pointers */
  DAPIPORT_GET_PTR(dapiPort, dapiPortPtr, hapiPortPtr);

  /* Accept only physical interfaces */
  if ( !IS_PORT_TYPE_PHYSICAL(dapiPortPtr) && !IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr) )
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "ERROR: Port {%d,%d,%d} are not physical nor logical",
            dapiPort->usp->unit,dapiPort->usp->slot,dapiPort->usp->port);
    return L7_FAILURE;
  }

  /* Get given MC group id */
  mc_group = *mcast_group;

  /* Create a multicast group, if given multicast group is not valid */
  if ( mc_group <= 0 )
  {
    if ((error=bcm_multicast_create(0, BCM_MULTICAST_TYPE_VLAN, &mc_group)) != BCM_E_NONE)
    {
      LOG_ERR(LOG_CTX_PTIN_HAPI,"Error with bcm_multicast_create(0, %d, &mcast_group): error=%d (\"%s\")",
              BCM_MULTICAST_TYPE_VLAN, error, bcm_errmsg(error));
      return L7_FAILURE;
    }
    *mcast_group = mc_group;
  }

  /* add network port to multicast group as L2 member */
  if ((error=bcm_multicast_l2_encap_get(0, mc_group, hapiPortPtr->bcmx_lport, -1, &encap_id))!=BCM_E_NONE)
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Error with bcm_multicast_l2_encap_get: error=%d (\"%s\")",error, bcm_errmsg(error));
    return L7_FAILURE;
  }
  if ((error=bcm_multicast_egress_add(0, mc_group, hapiPortPtr->bcmx_lport, encap_id))!=BCM_E_NONE)
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Error with bcm_multicast_egress_add: error=%d (\"%s\")",error, bcm_errmsg(error));
    return L7_FAILURE;
  }

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "ptin_hapi_multicast_port_add returned success");

  return L7_SUCCESS;
}

/**
 * Remove port from egress multicast group
 * 
 * @param mcast_group : MC group id (must be valid) 
 * @param dapiPort    : Port to be removed
 *  
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_hapi_multicast_egress_port_remove(L7_int mcast_group, ptin_dapi_port_t *dapiPort)
{
  DAPI_PORT_t  *dapiPortPtr;
  BROAD_PORT_t *hapiPortPtr;
  bcm_multicast_t encap_id;
  bcm_error_t error;

  /* Forwarding vlan, for MAC learning purposes (only if fwdvlan is valid) */
  if ( mcast_group <= 0 )
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Invalid MC group (%d)", mcast_group);
    return L7_FAILURE;
  }

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "port={%d,%d,%d}, mcast_group=%d",
            dapiPort->usp->unit,dapiPort->usp->slot,dapiPort->usp->port, mcast_group);

  /* Validate interface */
  if ( dapiPort->usp->unit<0 || dapiPort->usp->slot<0 || dapiPort->usp->port<0 )
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "ERROR: Invalid interfaces");
    return L7_FAILURE;
  }

  /* Get port pointers */
  DAPIPORT_GET_PTR(dapiPort, dapiPortPtr, hapiPortPtr);

  /* Accept only physical interfaces */
  if ( !IS_PORT_TYPE_PHYSICAL(dapiPortPtr) && !IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr) )
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "ERROR: Port {%d,%d,%d} are not physical nor logical",
            dapiPort->usp->unit,dapiPort->usp->slot,dapiPort->usp->port);
    return L7_FAILURE;
  }

  /* add network port to multicast group as L2 member */
  error = bcm_multicast_l2_encap_get(0, mcast_group, hapiPortPtr->bcmx_lport, -1, &encap_id);
  if (error != BCM_E_NONE)
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Error with bcm_multicast_l2_encap_get: error=%d (\"%s\")",error, bcm_errmsg(error));
    return L7_FAILURE;
  }
  error = bcm_multicast_egress_delete(0, mcast_group, hapiPortPtr->bcmx_lport, encap_id);
  if (error != BCM_E_NONE && error != BCM_E_NOT_FOUND)
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Error with bcm_multicast_egress_delete: error=%d (\"%s\")",error, bcm_errmsg(error));
    return L7_FAILURE;
  }

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "ptin_hapi_multicast_port_add returned success");

  return L7_SUCCESS;
}

/**
 * Remove all entries associated to a MC group
 * 
 * @param mcast_group     : MC group id (must be valid) 
 * @param destroy_mcgroup : Destroy MC group
 *  
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_hapi_multicast_egress_clean(L7_int mcast_group, L7_BOOL destroy_mcgroup)
{
  bcm_error_t error;

  /* Forwarding vlan, for MAC learning purposes (only if fwdvlan is valid) */
  if ( mcast_group <= 0 )
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Invalid MC group (%d)", mcast_group);
    return L7_FAILURE;
  }

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "mcast_group=%d", mcast_group);

  error = bcm_multicast_egress_delete_all(0, mcast_group);
  if (error != BCM_E_NONE && error != BCM_E_NOT_FOUND)
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Error with bcm_multicast_egress_delete_all: error=%d (\"%s\")",error, bcm_errmsg(error));
    return L7_FAILURE;
  }

  /* Destroy MC group */
  if (destroy_mcgroup)
  {
    error = bcm_multicast_destroy(0, mcast_group);
    if (error != BCM_E_NONE && error != BCM_E_NOT_FOUND)
    {
      LOG_ERR(LOG_CTX_PTIN_HAPI,"Error with bcm_multicast_create(0, %d, &mcast_group): error=%d (\"%s\")",
              BCM_MULTICAST_TYPE_VLAN, error, bcm_errmsg(error));
      return L7_FAILURE;
    }
  }

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "bcm_multicast_egress_delete_all returned success");

  return L7_SUCCESS;
}


/**
 * Define MC group for a specific vlan for bridging 
 * 
 * @param vlanId      : vlan to be configured
 * @param mcast_group : MC group id (-1 to create)
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_hapi_bridgeVlan_multicast_set(L7_uint16 vlanId, L7_int *mcast_group)
{
  int error;
  bcm_multicast_t mc_group;
  bcm_vlan_control_vlan_t control;

  /* Forwarding vlan, for MAC learning purposes (only if fwdvlan is valid) */
  if ( mcast_group == L7_NULLPTR )
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Invalid MC group");
    return L7_FAILURE;
  }

  /* Get given MC group id */
  mc_group = *mcast_group;

  /* Create a multicast group, if given multicast group is not valid */
  if ( mc_group <= 0 )
  {
    if ((error=bcm_multicast_create(0, BCM_MULTICAST_TYPE_VLAN, &mc_group)) != BCM_E_NONE)
    {
      LOG_ERR(LOG_CTX_PTIN_HAPI,"Error with bcm_multicast_create(0,0x%x, &mcast_group): error=%d (\"%s\")",
              BCM_MULTICAST_TYPE_VLAN, error, bcm_errmsg(error));
      return L7_FAILURE;
    }
    *mcast_group = mc_group;

    LOG_NOTICE(LOG_CTX_PTIN_HAPI, "mc_group=%d created!", mc_group);
  }

  /* Only for valid vlans */
  if (vlanId>0 && vlanId<4095)
  {
    /* Get current control definitions for this vlan */
    bcm_vlan_control_vlan_t_init(&control);
    if ((error = bcm_vlan_control_vlan_get(0, vlanId, &control))!=BCM_E_NONE)
    {
      LOG_ERR(LOG_CTX_PTIN_HAPI, "Error getting vlan control structure! error=%d (%s)", error, bcm_errmsg(error));
      return L7_FAILURE;
    }
    
    /* Associate a MC group */
    control.broadcast_group = mc_group;
    control.unknown_multicast_group = mc_group;
    control.unknown_unicast_group = mc_group;

    /* Apply new control definitions to this vlan */
    if ( (error = bcm_vlan_control_vlan_set(0, vlanId, control)) != BCM_E_NONE )
    {
      LOG_ERR(LOG_CTX_PTIN_HAPI, "Error with bcm_vlan_control_vlan_set: error=%d (%s)", error, bcm_errmsg(error));
      return L7_FAILURE;
    }
  }

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "ptin_hapi_bridge_vlan_mode_mcast_set returned success");

  return L7_SUCCESS;
}

/**
 * Rempove MC group for a specific vlan for bridging 
 * 
 * @param vlanId          : vlan to be configured
 * @param mcast_group     : MC group id 
 * @param destroy_mcgroup : Destroy MC group
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_hapi_bridgeVlan_multicast_reset(L7_uint16 vlanId, L7_int mcast_group, L7_BOOL destroy_mcgroup)
{
  int error;
  bcm_vlan_control_vlan_t control;

  /* Forwarding vlan, for MAC learning purposes (only if fwdvlan is valid) */
  if ( mcast_group <= 0 )
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Invalid MC group (%d)", mcast_group);
    return L7_FAILURE;
  }

  /* Only for valid vlans */
  if (vlanId>0 && vlanId<4095)
  {
    /* Get current control definitions for this vlan */
    bcm_vlan_control_vlan_t_init(&control);
    if ((error = bcm_vlan_control_vlan_get(0, vlanId, &control))!=BCM_E_NONE)
    {
      LOG_ERR(LOG_CTX_PTIN_HAPI, "Error getting vlan control structure! error=%d (%s)", error, bcm_errmsg(error));
      return L7_FAILURE;
    }
    
    /* Associate a MC group */
    control.broadcast_group         = -1;
    control.unknown_multicast_group = -1;
    control.unknown_unicast_group   = -1;

    /* Apply new control definitions to this vlan */
    if ( (error = bcm_vlan_control_vlan_set(0, vlanId, control)) != BCM_E_NONE )
    {
      LOG_ERR(LOG_CTX_PTIN_HAPI, "Error with bcm_vlan_control_vlan_set: error=%d (%s)", error, bcm_errmsg(error));
      //return L7_FAILURE;
    }
  }

  /* Destroy MC group */
  if (destroy_mcgroup)
  {
    if ((error=bcm_multicast_destroy(0, mcast_group)) != BCM_E_NONE)
    {
      LOG_ERR(LOG_CTX_PTIN_HAPI,"Error with bcm_multicast_destroy (mcast_group=%u): error=%d (\"%s\")",
              mcast_group, error, bcm_errmsg(error));
      return L7_FAILURE;
    }
  }

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "ptin_hapi_bridge_vlan_mode_mcast_set returned success");

  return L7_SUCCESS;
}

#if 1
/************************************
 * MAC Learning Control
 ************************************/
/**
 * Init MAC learning data structures
 * 
 * @return L7_RC_t : L7_SUCCESS
 */
L7_RC_t ptin_hapi_macaddr_init(void)
{
  L7_uint i;

  /* MAC learning control at vlan level */
  for (i=0; i<MAX_VLANS; i++)
  {
    macLearn_info_vlan[i].mac_counter = 0;
    macLearn_info_vlan[i].mac_total   = 0;
    macLearn_info_vlan[i].mac_maximum = (L7_uint8)-1; /* Unlimited */

    macLearn_info_vlan[i].trap_sent           = L7_FALSE;
    macLearn_info_vlan[i].ptin_intf.intf_type = 0;
    macLearn_info_vlan[i].ptin_intf.intf_id   = 0;
    macLearn_info_vlan[i].uni_ovid            = 0;
  }

  /* MAC learning control at virtual port level */
  for (i=0; i<MAX_GPORTS; i++)
  {
    macLearn_info_flow[i].mac_counter = 0;
    macLearn_info_flow[i].mac_total   = 0;
    macLearn_info_flow[i].mac_maximum = (L7_uint8)-1; /* Unlimited */

    macLearn_info_flow[i].trap_sent           = L7_FALSE;
    macLearn_info_flow[i].ptin_intf.intf_type = 0;
    macLearn_info_flow[i].ptin_intf.intf_id   = 0;
    macLearn_info_flow[i].uni_ovid            = 0;
  }

  return L7_SUCCESS;
}


/**
 * Increment number of learned MAC addresses
 * 
 * @param bcmx_l2_addr : MAC info
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_hapi_macaddr_inc(bcmx_l2_addr_t *bcmx_l2_addr)
{
  L7_uint vport_id = 0;
  L7_uint vlan_id = 0;

  if (BCM_GPORT_IS_VLAN_PORT(bcmx_l2_addr->lport))
  {
    vport_id = bcmx_l2_addr->lport & 0xffff;

    /* Virtual port ID is valid? */
    if (vport_id > MAX_GPORTS)
    {
      LOG_NOTICE(LOG_CTX_PTIN_HAPI, "GPORT is out of range! (vport_id=%u max=%u)", vport_id, MAX_GPORTS);
      return L7_FAILURE;
    }

    /* Feature enabled? */
    if (macLearn_info_flow[vport_id].mac_maximum == (L7_uint8)-1)
    {
      return L7_FAILURE;
    }

    /* Do not accept more mac addresses, if maximum was reached */
    if (macLearn_info_flow[vport_id].mac_counter >= macLearn_info_flow[vport_id].mac_maximum)
    {
      LOG_TRACE(LOG_CTX_PTIN_HAPI, "%s: MAC %02x:%02x:%02x:%02x:%02x:%02x on VID %d and GPORT 0x%x rejected (flags 0x%x)",
              __FUNCTION__, 
              bcmx_l2_addr->mac[0], bcmx_l2_addr->mac[1], bcmx_l2_addr->mac[2], bcmx_l2_addr->mac[3], bcmx_l2_addr->mac[4], bcmx_l2_addr->mac[5], 
              bcmx_l2_addr->vid, bcmx_l2_addr->lport, bcmx_l2_addr->flags);

      macLearn_info_flow[vport_id].mac_total++;

      if (macLearn_info_flow[vport_id].trap_sent == L7_FALSE)
      {
        send_trap_switch_event(macLearn_info_flow[vport_id].ptin_intf.intf_type, macLearn_info_flow[vport_id].ptin_intf.intf_id, TRAP_ALARM_MAC_LIMIT, TRAP_ALARM_STATUS_START, macLearn_info_flow[vport_id].uni_ovid);
        macLearn_info_flow[vport_id].trap_sent = L7_TRUE;
      }

      return L7_FAILURE;
    }

    LOG_TRACE(LOG_CTX_PTIN_HAPI, "%s: MAC %02x:%02x:%02x:%02x:%02x:%02x on VID %d and GPORT 0x%x accepted (flags 0x%x)\r\n",
              __FUNCTION__, 
              bcmx_l2_addr->mac[0], bcmx_l2_addr->mac[1], bcmx_l2_addr->mac[2], bcmx_l2_addr->mac[3], bcmx_l2_addr->mac[4], bcmx_l2_addr->mac[5], 
              bcmx_l2_addr->vid, bcmx_l2_addr->lport, bcmx_l2_addr->flags);

    macLearn_info_flow[vport_id].mac_counter++;
    macLearn_info_flow[vport_id].mac_total++;

    /* Check if maximum was reached */
    if (macLearn_info_flow[vport_id].mac_counter >= macLearn_info_flow[vport_id].mac_maximum)
    {
      /* Enable the use of Pending Mechanism but disable FWD */
      LOG_NOTICE(LOG_CTX_PTIN_HAPI, "Disabling FWD (GPORT=0x%x)", bcmx_l2_addr->lport);
      bcm_port_learn_set(0, bcmx_l2_addr->lport, /*BCM_PORT_LEARN_CPU |*/ BCM_PORT_LEARN_PENDING | BCM_PORT_LEARN_ARL );
    }

  }
  else
  {
    vlan_id = bcmx_l2_addr->vid;

    /* VLAN ID is valid? */
    if (vlan_id > MAX_VLANS)
    {
      LOG_NOTICE(LOG_CTX_PTIN_HAPI, "VLAN is out of range! (vlan_id=%u max=%u)", vlan_id, MAX_VLANS);
      return L7_FAILURE;
    }

    /* Feature enabled? */
    if (macLearn_info_flow[vport_id].mac_maximum == (L7_uint8)-1)
    {
      return L7_FAILURE;
    }

    /* Do not accept more MAC addresses, if maximum was reached */
    if (macLearn_info_vlan[vlan_id].mac_counter >= macLearn_info_vlan[vlan_id].mac_maximum)
    {
      return L7_FAILURE;
    }

    macLearn_info_vlan[vlan_id].mac_counter++;
  }

  return L7_SUCCESS;
}

/**
 * Decrement number of learned MAC addresses
 * 
 * @param bcmx_l2_addr : MAC info
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_hapi_macaddr_dec(bcmx_l2_addr_t *bcmx_l2_addr)
{
  L7_uint vport_id = 0;
  L7_uint vlan_id = 0;

  if (BCM_GPORT_IS_VLAN_PORT(bcmx_l2_addr->lport))
  {
    vport_id = bcmx_l2_addr->lport & 0xffff;

    /* Virtual port ID is valid? */
    if (vport_id > MAX_GPORTS)
    {
      LOG_NOTICE(LOG_CTX_PTIN_HAPI, "GPORT is out of range! (vport_id=%u max=%u)", vport_id, MAX_GPORTS);
      return L7_FAILURE;
    }

    /* Feature enabled? */
    if (macLearn_info_flow[vport_id].mac_maximum == (L7_uint8)-1)
    {
      return L7_FAILURE;
    }

    LOG_TRACE(LOG_CTX_PTIN_HAPI, "%s: MAC %02x:%02x:%02x:%02x:%02x:%02x on VID %d and GPORT 0x%x cleared (flags 0x%x)",
              __FUNCTION__, 
              bcmx_l2_addr->mac[0], bcmx_l2_addr->mac[1], bcmx_l2_addr->mac[2], bcmx_l2_addr->mac[3], bcmx_l2_addr->mac[4], bcmx_l2_addr->mac[5], 
              bcmx_l2_addr->vid, bcmx_l2_addr->lport, bcmx_l2_addr->flags);

    /* Do not accept more MAC addresses, if maximum was reached */
    /* if BCM_L2_PENDING is cleared, it means it is a aging of a learned MAC in the L2 table */
    if ( ((bcmx_l2_addr->flags & BCM_L2_PENDING) && !(bcmx_l2_addr->flags & BCM_L2_MOVE)) )
    {
      LOG_TRACE(LOG_CTX_PTIN_HAPI, "%s: MAC %02x:%02x:%02x:%02x:%02x:%02x/VID %d with PENDING flag",__FUNCTION__,
                bcmx_l2_addr->mac[0], bcmx_l2_addr->mac[1], bcmx_l2_addr->mac[2], bcmx_l2_addr->mac[3], bcmx_l2_addr->mac[4], bcmx_l2_addr->mac[5], 
                bcmx_l2_addr->vid);

      /* Decrement, but only if greater than 0 */
      if ((macLearn_info_flow[vport_id].mac_total > macLearn_info_flow[vport_id].mac_counter) && (macLearn_info_flow[vport_id].mac_total > 0))
      {
        macLearn_info_flow[vport_id].mac_total--;
      }

      return L7_FAILURE;
    }

    /* Decrement, but only if greater than 0 */
    if (macLearn_info_flow[vport_id].mac_counter > 0)
    {
      macLearn_info_flow[vport_id].mac_counter--;
      macLearn_info_flow[vport_id].mac_total--;
    }

    /* Check if maximum was reached */
    if ( (macLearn_info_flow[vport_id].mac_counter <= macLearn_info_flow[vport_id].mac_maximum) )
    {
      if (macLearn_info_flow[vport_id].trap_sent == L7_TRUE)
      {      
        send_trap_switch_event(macLearn_info_flow[vport_id].ptin_intf.intf_type, macLearn_info_flow[vport_id].ptin_intf.intf_id, TRAP_ALARM_MAC_LIMIT, TRAP_ALARM_STATUS_END, macLearn_info_flow[vport_id].uni_ovid);
        macLearn_info_flow[vport_id].trap_sent = L7_FALSE;
      }
    }

    #if 0
    /* Check if maximum was reached */
    if ( (macLearn_info_flow[vport_id].mac_counter < macLearn_info_flow[vport_id].mac_maximum) && (macLearn_info_flow[vport_id].mac_total < macLearn_info_flow[vport_id].mac_maximum))
    {
      /* Enable the use of Pending Mechanism and enable FWD */
      LOG_NOTICE(LOG_CTX_PTIN_HAPI, "Enabling FWD (GPORT=0x%x)", bcmx_l2_addr->lport);
      bcm_port_learn_set(0, bcmx_l2_addr->lport, BCM_PORT_LEARN_FWD | /*BCM_PORT_LEARN_CPU |*/ BCM_PORT_LEARN_PENDING | BCM_PORT_LEARN_ARL );
    }
    #endif
  }
  else
  {
    vlan_id = bcmx_l2_addr->vid;

    /* VLAN ID is valid? */
    if (vlan_id > MAX_VLANS)
    {
      LOG_NOTICE(LOG_CTX_PTIN_HAPI, "VLAN is out of range! (vlan_id=%u max=%u)", vlan_id, MAX_VLANS);
      return L7_FAILURE;
    }

    /* Feature enabled? */
    if (macLearn_info_flow[vport_id].mac_maximum == (L7_uint8)-1)
    {
      return L7_FAILURE;
    }

    /* Decrement, but only if greater than 0 */
    if (macLearn_info_vlan[vlan_id].mac_counter > 0)
    {
      macLearn_info_vlan[vlan_id].mac_counter--;
    }
  }

  return L7_SUCCESS;
}

/**
 * Reset number of learned MAC addresses
 * 
 * @param vlanId : VLAN id 
 * @param gport  : GPort (virtual port)
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_hapi_macaddr_reset(bcm_vlan_t vlan_id, bcm_gport_t gport)
{
  L7_uint vport_id = 0;

  if (BCM_GPORT_IS_VLAN_PORT(gport))
  {
    vport_id = gport & 0xffff;

    /* Virtual port ID is valid? */
    if (vport_id > MAX_GPORTS)
    {
      LOG_NOTICE(LOG_CTX_PTIN_HAPI, "GPORT is out of range! (vport_id=%u max=%u)", vport_id, MAX_GPORTS);
      return L7_FAILURE;
    }

    LOG_NOTICE(LOG_CTX_PTIN_HAPI, "Disabling Pending Mechanism (GPORT=0x%x)", gport);

    /* Disable the use of Pending Mechanism */
    bcm_port_learn_set(0, gport, BCM_PORT_LEARN_FWD );

    macLearn_info_flow[vport_id].mac_counter = 0;
    macLearn_info_flow[vport_id].mac_total = 0;
    macLearn_info_flow[vport_id].mac_maximum = (L7_uint8)-1; /* Unlimited */

  }
  else
  {
    /* VLAN ID is valid? */
    if (vlan_id > MAX_VLANS)
    {
      LOG_NOTICE(LOG_CTX_PTIN_HAPI, "VLAN is out of range! (vlan_id=%u max=%u)", vlan_id, MAX_VLANS);
      return L7_FAILURE;
    }

    LOG_NOTICE(LOG_CTX_PTIN_HAPI, "Disabling Pending Mechanism (VLAN ID=0x%x)", vlan_id);

    macLearn_info_vlan[vlan_id].mac_counter = 0;
    macLearn_info_vlan[vlan_id].mac_maximum = (L7_uint8)-1; /* Unlimited */
  }

  return L7_SUCCESS;
}

L7_RC_t ptin_hapi_macaddr_fdbFlush(bcm_vlan_t vlan_id, bcm_gport_t gport, BROAD_FLUSH_TYPE_t type)
{
  L7_uint vport_id = 0;

  vport_id = gport & 0xffff;

  macLearn_info_flow[vport_id].mac_total -= macLearn_info_flow[vport_id].mac_counter;
  macLearn_info_flow[vport_id].mac_counter = 0;

  /* Flush FDB */
  BROAD_L2ADDR_FLUSH_t  l2addr_vlan;
  /* Fill in the structure */
  l2addr_vlan.bcmx_lport = gport;
  l2addr_vlan.vlanID = vlan_id;
  l2addr_vlan.flushtype = type;
  l2addr_vlan.port_is_lag = L7_FALSE;
  l2addr_vlan.tgid        = 0;
  memset(l2addr_vlan.mac.addr, 0, L7_ENET_MAC_ADDR_LEN);

  /* Send a message to L2 address flushing task with the vlan info */
  hapiBroadL2FlushRequest(l2addr_vlan);

  return L7_SUCCESS;
}

/**
 * Set maximum number of learned MAC addresses
 * 
 * @param vlanId : VLAN id 
 * @param gport  : GPort (virtual port)
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_hapi_macaddr_setmax(bcm_vlan_t vlan_id, bcm_gport_t gport, L7_uint8 max_value)
{
  L7_uint vport_id = 0;
  L7_uint8 max_value_old;
  L7_RC_t rc = L7_SUCCESS;


  if (BCM_GPORT_IS_VLAN_PORT(gport))
  {
    vport_id = gport & 0xffff;

    /* Virtual port ID is valid? */
    if (vport_id > MAX_GPORTS)
    {
      LOG_NOTICE(LOG_CTX_PTIN_HAPI, "GPORT is out of range! (vport_id=%u max=%u)", vport_id, MAX_GPORTS);
      return L7_FAILURE;
    }

    if (max_value == (L7_uint8)-1)
    {
      if (macLearn_info_flow[vport_id].mac_maximum != (L7_uint8)-1)
      {
        rc = ptin_hapi_macaddr_reset(vlan_id, gport);
      }
      return rc;
    }

    LOG_NOTICE(LOG_CTX_PTIN_HAPI, "Enabling Pending Mechanism (GPORT=0x%x) with MAC Learned limit set to %u", gport, max_value);

    max_value_old = macLearn_info_flow[vport_id].mac_maximum;
    macLearn_info_flow[vport_id].mac_maximum = max_value;

    /* Check if maximum was reached */
    if (macLearn_info_flow[vport_id].mac_counter >= macLearn_info_flow[vport_id].mac_maximum)
    {
      /* Enable the use of Pending Mechanism but disable FWD*/
      bcm_port_learn_set(0, gport, /*BCM_PORT_LEARN_CPU |*/ BCM_PORT_LEARN_PENDING | BCM_PORT_LEARN_ARL );
    }
    else
    {
      /* Enable the use of Pending Mechanism and enable FWD */
      bcm_port_learn_set(0, gport, BCM_PORT_LEARN_FWD | /*BCM_PORT_LEARN_CPU |*/ BCM_PORT_LEARN_PENDING | BCM_PORT_LEARN_ARL );
    }

    #if 0
    /* New MAX value lower than old value. Clear the L2 table. */
    if (max_value < max_value_old)
    {

      LOG_TRACE(LOG_CTX_PTIN_HAPI, "Performing fdbFlush");

      ptin_hapi_macaddr_fdbFlush(vlan_id, gport, BROAD_FLUSH_BY_VLAN);
    }
    #endif

  }
  else
  {
    /* VLAN ID is valid? */
    if (vlan_id > MAX_VLANS)
    {
      LOG_NOTICE(LOG_CTX_PTIN_HAPI, "VLAN is out of range! (vlan_id=%u max=%u)", vlan_id, MAX_VLANS);
      return L7_FAILURE;
    }

    if (max_value == (L7_uint8)-1)
    {
      if (macLearn_info_vlan[vport_id].mac_maximum != (L7_uint8)-1)
      {
        rc = ptin_hapi_macaddr_reset(vlan_id, gport);
      }
      return rc;
    }

    LOG_NOTICE(LOG_CTX_PTIN_HAPI, "Enabling Pending Mechanism (VLAN ID=%d) with MAC Learned limit set to %u", vlan_id, max_value);

    macLearn_info_vlan[vlan_id].mac_maximum = max_value;
  }

  return L7_SUCCESS;
}


/**
 * Configures the information needed to generate alarms
 *  
 * @param vlanId    : VLAN id 
 * @param gport     : GPort (virtual port)
 * @param outer_vid : VLAN id 
 * @param intf_id   : Physical port
 *  
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_hapi_macaddr_config(bcm_vlan_t vlan_id, bcm_gport_t gport, int bcm_port, L7_uint16 outer_vid)
{
  L7_uint vport_id = 0;
  L7_int  port;
  
  if (BCM_GPORT_IS_VLAN_PORT(gport))
  {
    vport_id = gport & 0xffff;

    /* Virtual port ID is valid? */
    if (vport_id > MAX_GPORTS)
    {
      LOG_NOTICE(LOG_CTX_PTIN_HAPI, "GPORT is out of range! (vport_id=%u max=%u)", vport_id, MAX_GPORTS);
      return L7_FAILURE;
    }

    LOG_NOTICE(LOG_CTX_PTIN_HAPI, "(GPORT=0x%x) MAC Learned limit information %u, bcm_port %u, outer_vid %d", gport, bcm_port, outer_vid);

    hapi_ptin_port_get(bcm_port, &port);

    macLearn_info_flow[vport_id].ptin_intf.intf_type =  PTIN_EVC_INTF_PHYSICAL;
    macLearn_info_flow[vport_id].ptin_intf.intf_id =    port;                   // PON interface
    macLearn_info_flow[vport_id].uni_ovid =             outer_vid;              // GEM id

    LOG_NOTICE(LOG_CTX_PTIN_HAPI, "port %u", port);
  }
  else
  {
    /* VLAN ID is valid? */
    if (vlan_id > MAX_VLANS)
    {
      LOG_NOTICE(LOG_CTX_PTIN_HAPI, "VLAN is out of range! (vlan_id=%u max=%u)", vlan_id, MAX_VLANS);
      return L7_FAILURE;
    }
  }

  return L7_SUCCESS;
}

/**
 * Dump MAC limit tables
 * 
 * @author alex (3/5/2014)
 */
void ptin_maclimit_dump(void)
{
  L7_uint i;

  printf("MAC limit tables\n");

  /* MAC learning control at vlan level */
  for (i=0; i<MAX_VLANS; i++)
  {
    if (macLearn_info_vlan[i].mac_maximum != (L7_uint8)-1)
    {
      printf(" VLAN=%.4u   %u of %u (total: %u)\n", i, macLearn_info_vlan[i].mac_counter, macLearn_info_vlan[i].mac_maximum, macLearn_info_vlan[i].mac_total);
    }
  }

  /* MAC learning control at virtual port level */
  for (i=0; i<MAX_GPORTS; i++) 
  {
    if (macLearn_info_flow[i].mac_maximum != (L7_uint8)-1)
    {
      printf(" GPORT=0x%.4x  %u of %u (total: %u)\n", i, macLearn_info_flow[i].mac_counter, macLearn_info_flow[i].mac_maximum, macLearn_info_flow[i].mac_total);
    }
  }

  printf("\n");
  fflush(stdout);
}

#endif

