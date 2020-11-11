#include <bcm/error.h>

#include "ptin_hapi_xconnect.h"
#include "ptin_hapi_l2.h"
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
  int unit, error;

  /* Run all units */
  BCM_UNIT_ITER(unit)
  {
    PT_LOG_TRACE(LOG_CTX_HAPI, "bcm_switch_control_set(%d,%d,1)", unit, bcmSwitchSharedVlanEnable);

    /* Enable Shared Vlan for using one single vlan in E-Trees when learning MACs */
    error = bcm_switch_control_set(unit, bcmSwitchSharedVlanEnable, L7_TRUE);
    if ( error != BCM_E_NONE )
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "Error enabling shared vlan at unit %d: error=%d (%s)", unit, error, bcm_errmsg(error));
      return L7_FAILURE;
    }
  }

  #if 0
  error = ptin_hapi_maclimit_init();
  if ( error != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Error initializing MAC learning control: error=%d (%s)", error, bcm_errmsg(error));
    return L7_FAILURE;
  }
  #endif

  PT_LOG_TRACE(LOG_CTX_HAPI, "ptin_hapi_bridge_init returned success");

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
    PT_LOG_TRACE(LOG_CTX_HAPI, "Free cross-connections consulted: %u", *crossconnects);
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
  int unit, error;
  bcm_vlan_control_vlan_t control;

  /* Run all units */
  BCM_UNIT_ITER(unit)
  {
    /* Get current control definitions for this vlan */
    bcm_vlan_control_vlan_t_init(&control);

    error = bcm_vlan_control_vlan_get(unit, vlanId, &control);
    if (error != BCM_E_NONE)
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "unit=%d: Error getting vlan control structure! error=%d (%s)\r\n", unit, error, bcm_errmsg(error));
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
    error = bcm_vlan_control_vlan_set(unit, vlanId, control);
    if ( error != BCM_E_NONE )
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "unit=%d: Error with bcm_vlan_control_vlan_set: error=%d (%s)", unit, error, bcm_errmsg(error));
      return L7_FAILURE;
    }
  }

  PT_LOG_TRACE(LOG_CTX_HAPI, "ptin_hapi_bridge_vlan_mode_set returned success");

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
  int unit, error;
  bcm_vlan_control_vlan_t control;

  /* Forwarding vlan, for MAC learning purposes (only if fwdvlan is valid) */
  if ( fwdVlanId == 0 || fwdVlanId > 4095 )
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Invalid fwd vlan id: %u",fwdVlanId);
    return L7_FAILURE;
  }

  /* Run all units */
  BCM_UNIT_ITER(unit)
  {
    /* Get current control definitions for this vlan */
    bcm_vlan_control_vlan_t_init(&control);

    error = bcm_vlan_control_vlan_get(unit, vlanId, &control);
    if (error !=BCM_E_NONE)
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "unit %d: Error getting vlan control structure! error=%d (%s)", unit, error, bcm_errmsg(error));
      return L7_FAILURE;
    }
    
    /* Reset forward vlan */
    control.forwarding_vlan = fwdVlanId;

    /* Apply new control definitions to this vlan */
    error = bcm_vlan_control_vlan_set(unit, vlanId, control);
    if ( error != BCM_E_NONE )
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "unit %d: Error with bcm_vlan_control_vlan_set: error=%d (%s)", unit, error, bcm_errmsg(error));
      return L7_FAILURE;
    }
  }

  PT_LOG_TRACE(LOG_CTX_HAPI, "ptin_hapi_bridge_vlan_mode_fwdVlan_set returned success");

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
  int unit, error;
  bcm_vlan_control_vlan_t control;

  /* Validate outer TPID */
  if ( outer_tpid == 0x0000 )
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "ERROR: TPID is null");
    return L7_FAILURE;
  }

  /* Run all units */
  BCM_UNIT_ITER(unit)
  {
    /* Get current control definitions for this vlan */
    bcm_vlan_control_vlan_t_init(&control);

    error = bcm_vlan_control_vlan_get(unit, vlanId, &control);
    if (error != BCM_E_NONE)
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "unit %d: Error getting vlan control structure! error=%d (%s)\r\n", unit, error, bcm_errmsg(error));
      return L7_FAILURE;
    }

    /* Forwarding mode */
    control.outer_tpid = outer_tpid;

    /* Apply new control definitions to this vlan */
    error = bcm_vlan_control_vlan_set(unit, vlanId, control);
    if ( error != BCM_E_NONE )
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "unit %d: Error with bcm_vlan_control_vlan_set: error=%d (%s)", unit, error, bcm_errmsg(error));
      return L7_FAILURE;
    }
  }

  PT_LOG_TRACE(LOG_CTX_HAPI, "ptin_hapi_bridge_vlan_mode_outerTpId_set returned success");

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
  int unit, error;
  bcm_vlan_control_vlan_t control;

  /* Run all units */
  BCM_UNIT_ITER(unit)
  {
    /* Get current control definitions for this vlan */
    bcm_vlan_control_vlan_t_init(&control);

    error = bcm_vlan_control_vlan_get(unit, vlanId, &control);
    if (error != BCM_E_NONE)
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "unit %d: Error getting vlan control structure! error=%d (%s)\r\n", unit, error, bcm_errmsg(error));
      return L7_FAILURE;
    }

    PT_LOG_TRACE(LOG_CTX_HAPI, "Control flags = 0x%08x",control.flags);

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
    error = bcm_vlan_control_vlan_set(unit, vlanId, control);
    if ( error != BCM_E_NONE )
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "unit %d: Error with bcm_vlan_control_vlan_set: error=%d (%s)", unit, error, bcm_errmsg(error));
      return L7_FAILURE;
    }
  }

  PT_LOG_TRACE(LOG_CTX_HAPI, "ptin_hapi_bridge_vlan_mode_macLearn_set returned success");

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
  int unit, error;
  bcm_vlan_control_vlan_t control;

  /* Run all units */
  BCM_UNIT_ITER(unit)
  {
    /* Get current control definitions for this vlan */
    bcm_vlan_control_vlan_t_init(&control);

    error = bcm_vlan_control_vlan_get(unit, vlanId, &control);
    if (error != BCM_E_NONE)
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "unit %d: Error getting vlan control structure! error=%d (%s)\r\n", unit, error, bcm_errmsg(error));
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
    error = bcm_vlan_control_vlan_set(unit, vlanId, control);
    if ( error != BCM_E_NONE )
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "unit %d: Error with bcm_vlan_control_vlan_set: error=%d (%s)", unit, error, bcm_errmsg(error));
      return L7_FAILURE;
    }
  }

  PT_LOG_TRACE(LOG_CTX_HAPI, "ptin_hapi_bridge_vlan_mode_crossconnect_set returned success (enable=%u, double=%u)", cross_connects_apply, double_tag);

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
  int unit, error = BCM_E_NONE;
  DAPI_PORT_t  *dapiPortPtr1, *dapiPortPtr2;
  BROAD_PORT_t *hapiPortPtr1, *hapiPortPtr2;

  PT_LOG_TRACE(LOG_CTX_HAPI, "oVlanId=%u iVlanId=%u port1={%d,%d,%d} port2={%d,%d,%d}",
            outerVlanId,innerVlanId,
            dapiPort1->usp->unit,dapiPort1->usp->slot,dapiPort1->usp->port,
            dapiPort2->usp->unit,dapiPort2->usp->slot,dapiPort2->usp->port);

  if ( (dapiPort1->usp->unit<0 || dapiPort1->usp->slot<0 || dapiPort1->usp->port<0) ||
       (dapiPort2->usp->unit<0 || dapiPort2->usp->slot<0 || dapiPort2->usp->port<0) )
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "ERROR: Invalid interfaces");
    return L7_FAILURE;
  }

  /* Get port pointers */
  DAPIPORT_GET_PTR(dapiPort1, dapiPortPtr1, hapiPortPtr1);
  DAPIPORT_GET_PTR(dapiPort2, dapiPortPtr2, hapiPortPtr2);

  /* Accept only physical and lag interfaces */
  if ( ( !IS_PORT_TYPE_PHYSICAL(dapiPortPtr1) && !IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr1) ) ||
       ( !IS_PORT_TYPE_PHYSICAL(dapiPortPtr2) && !IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr2) ) )
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "ERROR: Ports ({%d,%d,%d} and {%d,%d,%d}) are not physical neither lag",
            dapiPort1->usp->unit,dapiPort1->usp->slot,dapiPort1->usp->port,
            dapiPort2->usp->unit,dapiPort2->usp->slot,dapiPort2->usp->port);
    return L7_FAILURE;
  }

  /* Validate vlans */
  if ( outerVlanId==0 || outerVlanId>4095 )
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Outer vlan is invalid: %u",outerVlanId);
    return L7_FAILURE;
  }

  /* If inner vlan is not valid, use the BCM_VLAN_INVALID */
  if ( innerVlanId == 0 || innerVlanId > 4095 )
  {
    innerVlanId = BCM_VLAN_INVALID;
  }

  PT_LOG_TRACE(LOG_CTX_HAPI, "bcm_vlan_cross_connect_add(%d,%u,%u,%u({%d,%d,%d}), %u({%d,%d,%d}))",
            bcm_unit, outerVlanId, innerVlanId,
            hapiPortPtr1->bcm_gport, dapiPort1->usp->unit, dapiPort1->usp->slot, dapiPort1->usp->port,
            hapiPortPtr2->bcm_gport, dapiPort2->usp->unit, dapiPort2->usp->slot, dapiPort2->usp->port);

  /* Run all units */
  BCM_UNIT_ITER(unit)
  {
    error = bcm_vlan_cross_connect_add(unit, outerVlanId, innerVlanId, hapiPortPtr1->bcm_gport, hapiPortPtr2->bcm_gport);
    if ( error != BCM_E_NONE && error != BCM_E_EXISTS )
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "unit %d: Error with bcm_vlan_cross_connect_add: %d (%s)", unit, error, bcm_errmsg(error));
      break;
    }
  }

  /* Manager resources meter */
  if ( error == BCM_E_NONE )
  {
    if (resources_crossconnects<FREE_RESOURCES_CROSSCONNECTS)
      resources_crossconnects++;
  }

  PT_LOG_TRACE(LOG_CTX_HAPI, "Finished successfully");

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
  int unit, error = BCM_E_NONE;

  PT_LOG_TRACE(LOG_CTX_HAPI, "oVlanId=%u iVlanId=%u",outerVlanId,innerVlanId);

  /* Validate vlans */
  if ( outerVlanId==0 || outerVlanId>4095 )
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Outer vlan is invalid: %u",outerVlanId);
    return L7_FAILURE;
  }

  /* If inner vlan is not valid, use the BCM_VLAN_INVALID */
  if ( innerVlanId == 0 || innerVlanId > 4095 )
  {
    innerVlanId = BCM_VLAN_INVALID;
  }

  PT_LOG_TRACE(LOG_CTX_HAPI, "bcm_vlan_cross_connect_delete(%d,%u,%u)",bcm_unit,outerVlanId,innerVlanId);

  /* Run all units */
  BCM_UNIT_ITER(unit)
  {
    error = bcm_vlan_cross_connect_delete(unit, outerVlanId, innerVlanId);
    if ( error != BCM_E_NONE && error != BCM_E_NOT_FOUND )
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "unit %d: Error with bcm_vlan_cross_connect_delete: %d (%s)", unit, error, bcm_errmsg(error));
      break;
    }
  }

  /* Manager resources meter */
  if ( error == BCM_E_NONE )
  {
    if (resources_crossconnects>0)
      resources_crossconnects--;
  }

  PT_LOG_TRACE(LOG_CTX_HAPI, "Finished successfully");

  return L7_SUCCESS;
}

/**
 * Deletes all configured cross-connections
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_hapi_bridge_crossconnect_delete_all(void)
{
  int unit, error;

  PT_LOG_TRACE(LOG_CTX_HAPI, "bcm_vlan_cross_connect_delete_all(%d)",bcm_unit);

  /* Run all units */
  BCM_UNIT_ITER(unit)
  {
    error = bcm_vlan_cross_connect_delete_all(unit);
    if ( error != BCM_E_NONE && error != BCM_E_NOT_FOUND )
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "unit %d: Error with bcm_vlan_cross_connect_delete: %d (%s)", unit, error, bcm_errmsg(error));
      break;
    }
  }

  PT_LOG_TRACE(LOG_CTX_HAPI, "Finished successfully");

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
                            L7_int *virtual_gport, L7_int port_id, L7_int type)
{
  DAPI_PORT_t  *dapiPortPtr;
  BROAD_PORT_t *hapiPortPtr;
  bcm_vlan_port_t vlan_port;
  bcm_vlan_action_set_t action;
  bcm_multicast_t mc_group;
  bcm_error_t error;
  bcm_multicast_t encap_id;
  int unit;

  PT_LOG_TRACE(LOG_CTX_HAPI, "port={%d,%d,%d}, oVlanId=%u iVlanId=%u => oVlanId=%u iVlanId=%u",
            dapiPort->usp->unit,dapiPort->usp->slot,dapiPort->usp->port,
            match_ovid, match_ivid, egress_ovid, egress_ivid);

  /* Validate interface */
  if ( dapiPort->usp->unit<0 || dapiPort->usp->slot<0 || dapiPort->usp->port<0 )
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "ERROR: Invalid interfaces");
    return L7_FAILURE;
  }

  /* Get port pointers */
  DAPIPORT_GET_PTR(dapiPort, dapiPortPtr, hapiPortPtr);

  /* Accept only physical interfaces */
  if ( !IS_PORT_TYPE_PHYSICAL(dapiPortPtr) )
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "ERROR: Port {%d,%d,%d} are not physical",
            dapiPort->usp->unit,dapiPort->usp->slot,dapiPort->usp->port);
    return L7_FAILURE;
  }

  /* create the virtual ports */
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

  PT_LOG_TRACE(LOG_CTX_HAPI, "bcm_port=%d vlan_port.port=%d vport=%d", hapiPortPtr->bcm_port, vlan_port.port, vlan_port.vlan_port_id);

  /* Run all units */
  BCM_UNIT_ITER(unit)
  {
    /* If unit beyond 0, use gport given for unit 0 */
    if (unit > 0)
    {
      vlan_port.flags |= BCM_VLAN_PORT_WITH_ID;
    }

    error = bcm_vlan_port_create(unit, &vlan_port);
    if (error != BCM_E_NONE)
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "unit %d: Error with bcm_vlan_port_create: error=%d (\"%s\")", unit, error, bcm_errmsg(error));
      return L7_FAILURE;
    }

    PT_LOG_TRACE(LOG_CTX_HAPI, "unit %d: vport=0x%x", unit, vlan_port.vlan_port_id);

    #if 0
    /* MAC learning */
    error = BCM_E_NONE;
    do
    {
      if ((error = bcm_port_learn_set(unit, vlan_port.vlan_port_id, BCM_PORT_LEARN_PENDING | BCM_PORT_LEARN_ARL )) != BCM_E_NONE)
      {
        PT_LOG_ERR(LOG_CTX_HAPI, "unit %d: Error with bcm_port_learn_set: error=%d (\"%s\")", unit, error, bcm_errmsg(error));
        break;
      }
      if ((error = bcm_port_control_set(unit, vlan_port.vlan_port_id, bcmPortControlL2Learn, BCM_PORT_LEARN_PENDING | BCM_PORT_LEARN_ARL)) != BCM_E_NONE)
      {
        PT_LOG_ERR(LOG_CTX_HAPI, "unit %d: Error with bcm_port_control_set: error=%d (\"%s\")", unit, error, bcm_errmsg(error));
        break;
      }
      if ((error = bcm_port_control_set(unit, vlan_port.vlan_port_id, bcmPortControlL2Move,  BCM_PORT_LEARN_PENDING | BCM_PORT_LEARN_ARL)) != BCM_E_NONE)
      {
        PT_LOG_ERR(LOG_CTX_HAPI, "unit %d: Error with bcm_port_control_set: error=%d (\"%s\")", unit, error, bcm_errmsg(error));
        break;
      }
      if ((error = bcm_port_control_set(unit, vlan_port.vlan_port_id, bcmPortControlLearnClassEnable, L7_ENABLE)) != BCM_E_NONE)
      {
        PT_LOG_ERR(LOG_CTX_HAPI, "unit %d: Error with bcm_port_control_set: error=%d (\"%s\")", unit, error, bcm_errmsg(error));
        break;
      }
      if ((error = bcm_l2_learn_class_set(unit, 1, 1, BCM_L2_LEARN_CLASS_MOVE)) != BCM_E_NONE)
      {
        PT_LOG_ERR(LOG_CTX_HAPI, "unit %d: Error with bcm_l2_learn_class_set: error=%d (\"%s\")", unit, error, bcm_errmsg(error));
        break;
      }
      if ((error = bcm_l2_learn_port_class_set(unit, vlan_port.vlan_port_id, 1)) != BCM_E_NONE)
      {
        PT_LOG_ERR(LOG_CTX_HAPI, "unit %d: Error with bcm_l2_learn_port_class_set: error=%d (\"%s\")", unit, error, bcm_errmsg(error));
        break;
      }
    } while (0);

    if (error != BCM_E_NONE)
    {
      bcm_vlan_port_destroy(unit, vlan_port.vlan_port_id);
      PT_LOG_ERR(LOG_CTX_HAPI, "unit %d: error=%d (\"%s\")", unit, error, bcm_errmsg(error));
      return L7_FAILURE;
    }
    #endif

    /* create egress translation entries for virtual ports to do VLAN tag manipulation 
     * i.e. client -> gem_id + some_c_vlan */
    bcm_vlan_action_set_t_init(&action);
    
    /* for outer tagged packet => outer tag replaced with gem_id */
    action.ot_outer = bcmVlanActionReplace;
    action.dt_outer = bcmVlanActionReplace;
    action.new_outer_vlan = match_ovid;
    
    /* for outer tagged packet => inner tag added with cvid */
    action.ot_inner = bcmVlanActionAdd;
    action.dt_inner = bcmVlanActionReplace;
    action.new_inner_vlan = match_ivid;

    error = bcm_vlan_translate_egress_action_add(unit, vlan_port.vlan_port_id, egress_ovid, 0, &action);
    if (error != BCM_E_NONE)
    {
      bcm_vlan_port_destroy(unit, vlan_port.vlan_port_id);
      PT_LOG_ERR(LOG_CTX_HAPI, "unit %d: Error with bcm_vlan_translate_egress_action_add(%d, %d, %d, %d, &action): error=%d (\"%s\")",
                 unit, unit, vlan_port.vlan_port_id, egress_ovid, 0, error, bcm_errmsg(error));
      return L7_FAILURE;
    }

    /* Get given MC group id */
    mc_group = *mcast_group;

    /* Create a multicast group, if given multicast group is not valid */
    if ( mc_group <= 0 )
    {
      error = bcm_multicast_create(unit, BCM_MULTICAST_TYPE_VLAN, &mc_group);
      if (error != BCM_E_NONE)
      {
        PT_LOG_ERR(LOG_CTX_HAPI,"unit %d: Error with bcm_multicast_create(%d, %d, &mcast_group): error=%d (\"%s\")",
                   unit, unit, BCM_MULTICAST_TYPE_VLAN, error, bcm_errmsg(error));
        return L7_FAILURE;
      }
      *mcast_group = mc_group;
    }

    PT_LOG_TRACE(LOG_CTX_HAPI, "unit %d: mc_group=%d vlan_port.port=%d vport=%d", unit, mc_group, vlan_port.port, vlan_port.vlan_port_id);

    /* Add virtual port to multicast group */
    error = bcm_multicast_vlan_encap_get(unit, mc_group, vlan_port.port, vlan_port.vlan_port_id, &encap_id);
    if (error != BCM_E_NONE)
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "unit %d: Error with bcm_multicast_vlan_encap_get: error=%d (\"%s\")", unit, error, bcm_errmsg(error));
      return L7_FAILURE;
    }
    error = bcm_multicast_egress_add(unit, mc_group, vlan_port.port, encap_id);
    if (error != BCM_E_NONE)
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "unit %d: Error with bcm_multicast_egress_add: error=%d (\"%s\")", unit, error, bcm_errmsg(error));
      return L7_FAILURE;
    }
  }

  /* Configures the information needed to generate alarms related to MAC Limit */
  ptin_hapi_l2intf_maclimit_alarmconfig(vlan_port.vlan_port_id, hapiPortPtr->bcm_port, match_ovid, port_id, type);

  /* Return vport id */
  if (virtual_gport != L7_NULLPTR)
    *virtual_gport = vlan_port.vlan_port_id;

  PT_LOG_DEBUG(LOG_CTX_HAPI, "ptin_hapi_vp_create: vport 0x%x created", vlan_port.vlan_port_id);

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
  int unit;
  DAPI_PORT_t  *dapiPortPtr;
  BROAD_PORT_t *hapiPortPtr;
  bcm_vlan_port_t vlan_port;
  bcm_multicast_t encap_id;
  bcm_error_t error;

  PT_LOG_TRACE(LOG_CTX_HAPI, "port={%d,%d,%d}, oVlanId=%u iVlanId=%u",
            dapiPort->usp->unit,dapiPort->usp->slot,dapiPort->usp->port,
            match_ovid, match_ivid);

  /* Validate interface */
  if ( dapiPort->usp->unit<0 || dapiPort->usp->slot<0 || dapiPort->usp->port<0 )
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "ERROR: Invalid interfaces");
    return L7_FAILURE;
  }

  /* Get port pointers */
  DAPIPORT_GET_PTR(dapiPort, dapiPortPtr, hapiPortPtr);

  /* Accept only physical interfaces */
  if ( !IS_PORT_TYPE_PHYSICAL(dapiPortPtr) )
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "ERROR: Port {%d,%d,%d} are not physical",
            dapiPort->usp->unit,dapiPort->usp->slot,dapiPort->usp->port);
    return L7_FAILURE;
  }

  /* Forwarding vlan, for MAC learning purposes (only if fwdvlan is valid) */
  if ( mcast_group <= 0 )
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Invalid MC group (%d)", mcast_group);
    return L7_FAILURE;
  }

  /* create the virtual ports */
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

  /* Run all units */
  BCM_UNIT_ITER(unit)
  {
    error = bcm_vlan_port_find(unit, &vlan_port);
    if (error != BCM_E_NONE)
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "unit %d: Error with bcm_vlan_port_find: error=%d (\"%s\")", unit, error, bcm_errmsg(error));
      return L7_FAILURE;
    }

    PT_LOG_TRACE(LOG_CTX_HAPI, "unit %d: gport=0x%x, bcm_port=%d => vlan_port.port=%d vport=%d",
                 unit, virtual_gport, hapiPortPtr->bcm_port, vlan_port.port, vlan_port.vlan_port_id);
    PT_LOG_TRACE(LOG_CTX_HAPI, "unit %d: flags=0x%08x criteria=0x%08x match_vlan=%u match_ivlan=%u egress_vlan=%u egress_ivlan=%u",
                 unit, vlan_port.flags, vlan_port.criteria, vlan_port.match_vlan, vlan_port.match_inner_vlan, vlan_port.egress_vlan, vlan_port.egress_inner_vlan);

    /* Remove virtual port from multicast group */
    error = bcm_multicast_vlan_encap_get(unit, mcast_group, vlan_port.port, vlan_port.vlan_port_id, &encap_id);
    if (error != BCM_E_NONE)
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "unit %d: Error with bcm_multicast_vlan_encap_get: error=%d (\"%s\")", unit, error, bcm_errmsg(error));
      return L7_FAILURE;
    }
    error = bcm_multicast_egress_delete(unit, mcast_group, vlan_port.port, encap_id);
    if (error != BCM_E_NONE && error != BCM_E_NOT_FOUND)
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "unit %d: Error with bcm_multicast_egress_delete: error=%d (\"%s\")", unit, error, bcm_errmsg(error));
      return L7_FAILURE;
    }

    /* Remove egress translation entries */
    error = bcm_vlan_translate_egress_action_delete(unit, vlan_port.vlan_port_id, vlan_port.egress_vlan, 0);
    if (error != BCM_E_NONE && error != BCM_E_NOT_FOUND)
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "unit %d: Error with bcm_vlan_translate_egress_action_delete(%d, %d, %d, %d, &action): error=%d (\"%s\")",
                 unit, unit, vlan_port.vlan_port_id, vlan_port.egress_vlan, 0, error, bcm_errmsg(error));
      return L7_FAILURE;
    }

    /* Destroy virtual port */
    error = bcm_vlan_port_destroy(unit, vlan_port.vlan_port_id);
    if (error != BCM_E_NONE && error != BCM_E_NOT_FOUND)
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "unit %d: Error with bcm_vlan_port_destroy: error=%d (\"%s\")", unit, error, bcm_errmsg(error));
      return L7_FAILURE;
    }

    /* Remove MAC addresses related to this virtual port */
    error = bcm_l2_addr_delete_by_port(unit, -1, vlan_port.vlan_port_id, 0);
    if (error != BCM_E_NONE && error != BCM_E_NOT_FOUND)
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "unit %d: Error removing MAC addresses related to this vport: error=%d (\"%s\")", unit, error, bcm_errmsg(error));
    }

    /* Remove the Pending MAC addresses related to this virtual port */
    error = bcm_l2_addr_delete_by_port(unit, -1, vlan_port.vlan_port_id, BCM_L2_DELETE_PENDING);
    if (error != BCM_E_NONE && error != BCM_E_NOT_FOUND)
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "unit %d: Error removing MAC addresses related to this vport: error=%d (\"%s\")", unit, error, bcm_errmsg(error));
    }
  }

  PT_LOG_TRACE(LOG_CTX_HAPI, "ptin_hapi_vp_remove: vport 0x%x removed", vlan_port.vlan_port_id);

  return L7_SUCCESS;
}

/**
 * Add port to egress multicast group
 * 
 * @param mcast_group : MC group id (-1 to create) 
 * @param multicast_flag: Multicast Flags 
 * @param virtual_gport      : VLAN Identifier 
 * @param dapiPort    : Port to be added 
 *  
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_hapi_multicast_egress_port_add(L7_int *mcast_group, L7_uint32 multicast_flag, L7_uint32 virtual_gport, ptin_dapi_port_t *dapiPort)
{
  int unit;
  bcm_multicast_t mc_group;
  DAPI_PORT_t    *dapiPortPtr;
  BROAD_PORT_t   *hapiPortPtr;
  bcm_multicast_t encap_id;
  bcm_error_t     error;

  /* Forwarding vlan, for MAC learning purposes (only if fwdvlan is valid) */
  if ( mcast_group == L7_NULLPTR )
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Invalid MC group");
    return L7_FAILURE;
  }

  PT_LOG_TRACE(LOG_CTX_HAPI, "port={%d,%d,%d}, mcast_group=%d",
            dapiPort->usp->unit,dapiPort->usp->slot,dapiPort->usp->port, mcast_group);

  /* Validate interface */
  if ( dapiPort->usp->unit<0 || dapiPort->usp->slot<0 || dapiPort->usp->port<0 )
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "ERROR: Invalid interfaces");
    return L7_FAILURE;
  }

  /* Get port pointers */
  DAPIPORT_GET_PTR(dapiPort, dapiPortPtr, hapiPortPtr);

  /* Accept only physical interfaces */
  if ( !IS_PORT_TYPE_PHYSICAL(dapiPortPtr) && !IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr) )
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "ERROR: Port {%d,%d,%d} are not physical nor logical",
            dapiPort->usp->unit,dapiPort->usp->slot,dapiPort->usp->port);
    return L7_FAILURE;
  }

  /* Get given MC group id */
  mc_group = *mcast_group;

  /* Run all units */
  BCM_UNIT_ITER(unit)
  {
    /* Create a multicast group, if given multicast group is not valid */
    if ( mc_group <= 0 )
    {
      error = bcm_multicast_create(unit, multicast_flag, &mc_group);
      if (error != BCM_E_NONE)
      {
        PT_LOG_ERR(LOG_CTX_HAPI,"unit %d: Error with bcm_multicast_create(%d, %d, &mcast_group): error=%d (\"%s\")",
                   unit, unit, BCM_MULTICAST_TYPE_VLAN, error, bcm_errmsg(error));
        return L7_FAILURE;
      }
      *mcast_group = mc_group;
    }

    switch (multicast_flag)
    {
    case BCM_MULTICAST_TYPE_L2:  
      /* get the encapsulation id */
      error = bcm_multicast_l2_encap_get(unit, mc_group, hapiPortPtr->bcm_gport, -1, &encap_id);
      if (error != BCM_E_NONE)
      {
        PT_LOG_ERR(LOG_CTX_HAPI, "unit %d: Error with bcm_multicast_l2_encap_get: error=%d (\"%s\")", unit, error, bcm_errmsg(error));
        return L7_FAILURE;
      }    
      break;      
    case BCM_MULTICAST_TYPE_L3:
      /* get the encapsulation id */   
      error = bcm_multicast_l3_encap_get(unit, mc_group, hapiPortPtr->bcm_gport, virtual_gport, &encap_id);
      if (error != BCM_E_NONE)
      {
        PT_LOG_ERR(LOG_CTX_HAPI, "unit %d: Error with bcm_multicast_l3_encap_get: error=%d (\"%s\")", unit, error, bcm_errmsg(error));
        return L7_FAILURE;
      }    
      break;    
    default:
      PT_LOG_ERR(LOG_CTX_HAPI, "unit %d: Multicast Flag Not Supported :0x%08X", unit, multicast_flag);
      return L7_NOT_SUPPORTED;
    }  

    /* add network port to multicast group */
    error = bcm_multicast_egress_add(unit, mc_group, hapiPortPtr->bcm_gport, encap_id);
    if (error != BCM_E_NONE)
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "unit %d: Error with bcm_multicast_egress_add: error=%d (\"%s\")", unit, error, bcm_errmsg(error));
      return L7_FAILURE;
    }
  }

  PT_LOG_TRACE(LOG_CTX_HAPI, "ptin_hapi_multicast_port_add returned success");

  return L7_SUCCESS;
}

/**
 * Remove port from egress multicast group
 * 
 * @param mcast_group : MC group id (must be valid) 
 * @param multicast_flag: Multicast Flags   
 * @param virtual_gport      : VLAN Identifier  
 * @param dapiPort    : Port to be removed
 *  
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_hapi_multicast_egress_port_remove(L7_int mcast_group, L7_uint32 multicast_flag, L7_uint32 virtual_gport, ptin_dapi_port_t *dapiPort)
{
  int unit;
  DAPI_PORT_t    *dapiPortPtr;
  BROAD_PORT_t   *hapiPortPtr;
  bcm_multicast_t encap_id;
  bcm_error_t     error;

  /* Forwarding vlan, for MAC learning purposes (only if fwdvlan is valid) */
  if ( mcast_group <= 0 )
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Invalid MC group (%d)", mcast_group);
    return L7_FAILURE;
  }

  PT_LOG_TRACE(LOG_CTX_HAPI, "port={%d,%d,%d}, mcast_group=%d",
            dapiPort->usp->unit,dapiPort->usp->slot,dapiPort->usp->port, mcast_group);

  /* Validate interface */
  if ( dapiPort->usp->unit<0 || dapiPort->usp->slot<0 || dapiPort->usp->port<0 )
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "ERROR: Invalid interfaces");
    return L7_FAILURE;
  }

  /* Get port pointers */
  DAPIPORT_GET_PTR(dapiPort, dapiPortPtr, hapiPortPtr);

  /* Accept only physical interfaces */
  if ( !IS_PORT_TYPE_PHYSICAL(dapiPortPtr) && !IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr) )
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "ERROR: Port {%d,%d,%d} are not physical nor logical",
            dapiPort->usp->unit,dapiPort->usp->slot,dapiPort->usp->port);
    return L7_FAILURE;
  }

  /* Run all units */
  BCM_UNIT_ITER(unit)
  {
    switch (multicast_flag)
    {
    case BCM_MULTICAST_TYPE_L2:  
      /* get the encapsulation id */   
      error = bcm_multicast_l2_encap_get(unit, mcast_group, hapiPortPtr->bcm_gport, -1, &encap_id);
      if (error != BCM_E_NONE)
      {
        PT_LOG_ERR(LOG_CTX_HAPI, "unit %d: Error with bcm_multicast_l2_encap_get: error=%d (\"%s\")", unit, error, bcm_errmsg(error));
        return L7_FAILURE;
      }
      break;  
    case BCM_MULTICAST_TYPE_L3:
      /* get the encapsulation id */   
      error = bcm_multicast_l3_encap_get(unit, mcast_group, hapiPortPtr->bcm_gport, virtual_gport, &encap_id);
      if (error != BCM_E_NONE)
      {
        PT_LOG_ERR(LOG_CTX_HAPI, "unit %d: Error with bcm_multicast_l3_encap_get: error=%d (\"%s\")", unit, error, bcm_errmsg(error));
        return L7_FAILURE;
      }
      break;
    default:
      PT_LOG_ERR(LOG_CTX_HAPI, "unit %d: Multicast Flag Not Supported :0x%08X", unit, multicast_flag);
      return L7_NOT_SUPPORTED;
    }
    
    error = bcm_multicast_egress_delete(unit, mcast_group, hapiPortPtr->bcm_gport, encap_id);
    if (error != BCM_E_NONE && error != BCM_E_NOT_FOUND)
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "unit %d: Error with bcm_multicast_egress_delete: error=%d (\"%s\")", unit, error, bcm_errmsg(error));
      return L7_FAILURE;
    }
  }

  PT_LOG_TRACE(LOG_CTX_HAPI, "ptin_hapi_multicast_port_add returned success");

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
  int unit;
  bcm_error_t error;

  /* Forwarding vlan, for MAC learning purposes (only if fwdvlan is valid) */
  if ( mcast_group <= 0 )
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Invalid MC group (%d)", mcast_group);
    return L7_FAILURE;
  }

  PT_LOG_TRACE(LOG_CTX_HAPI, "mcast_group=%d", mcast_group);

  /* Run all units */
  BCM_UNIT_ITER(unit)
  {
    error = bcm_multicast_egress_delete_all(unit, mcast_group);
    if (error != BCM_E_NONE && error != BCM_E_NOT_FOUND)
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "unit %d: Error with bcm_multicast_egress_delete_all: error=%d (\"%s\")", unit, error, bcm_errmsg(error));
      return L7_FAILURE;
    }

    /* Destroy MC group */
    if (destroy_mcgroup)
    {
      error = bcm_multicast_destroy(unit, mcast_group);
      if (error != BCM_E_NONE && error != BCM_E_NOT_FOUND)
      {
        PT_LOG_ERR(LOG_CTX_HAPI,"unit %d: Error with bcm_multicast_create(%d, %d, &mcast_group): error=%d (\"%s\")",
                   unit, unit, BCM_MULTICAST_TYPE_VLAN, error, bcm_errmsg(error));
        return L7_FAILURE;
      }
    }
  }

  PT_LOG_TRACE(LOG_CTX_HAPI, "bcm_multicast_egress_delete_all returned success");

  return L7_SUCCESS;
}


/**
 * Define MC group for a specific vlan for bridging 
 * 
 * @param vlanId :    vlan to be configured
 * @param mcast_group : MC group id (if invalid, create a new 
 *                    one, and return it)
 * @param multicast_flags: Multicast Flags 
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_hapi_bridgeVlan_multicast_set(L7_uint16 vlanId, L7_int *mcast_group, L7_uint32 multicast_flag)
{
  int unit, error;
  bcm_multicast_t mc_group;
  bcm_vlan_control_vlan_t control;

  /* Forwarding vlan, for MAC learning purposes (only if fwdvlan is valid) */
  if ( mcast_group == L7_NULLPTR )
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Invalid MC group");
    return L7_FAILURE;
  }

  /* Get given MC group id */
  mc_group = *mcast_group;

  /* Run all units */
  BCM_UNIT_ITER(unit)
  {
    /* Create a multicast group, if given multicast group is not valid */
    if ( mc_group <= 0 )
    {
      error = bcm_multicast_create(unit, multicast_flag, &mc_group);
      if (error != BCM_E_NONE)
      {
        PT_LOG_ERR(LOG_CTX_HAPI,"unit %d: Error with bcm_multicast_create(%d,0x%x, &mcast_group): error=%d (\"%s\")",
                   unit, unit, multicast_flag, error, bcm_errmsg(error));
        return L7_FAILURE;
      }
      *mcast_group = mc_group;

      PT_LOG_DEBUG(LOG_CTX_HAPI, "unit %d: mc_group=0x%08x created!", unit, mc_group);
    }

    if (multicast_flag & BCM_MULTICAST_TYPE_VLAN)
    {
      /* Only for valid vlans */
      if (vlanId>0 && vlanId<4095)
      {
        /* Get current control definitions for this vlan */
        bcm_vlan_control_vlan_t_init(&control);

        error = bcm_vlan_control_vlan_get(unit, vlanId, &control);
        if (error != BCM_E_NONE)
        {
          PT_LOG_ERR(LOG_CTX_HAPI, "unit %d: Error getting vlan control structure! error=%d (%s)", unit, error, bcm_errmsg(error));
          return L7_FAILURE;
        }
        
        /* Associate a MC group */
        control.broadcast_group = mc_group;
        control.unknown_multicast_group = mc_group;
        control.unknown_unicast_group = mc_group;

        /* Apply new control definitions to this vlan */
        error = bcm_vlan_control_vlan_set(unit, vlanId, control);
        if ( error != BCM_E_NONE )
        {
          PT_LOG_ERR(LOG_CTX_HAPI, "unit %d: Error with bcm_vlan_control_vlan_set: error=%d (%s)", unit, error, bcm_errmsg(error));
          return L7_FAILURE;
        }
      }
    }
  }

  PT_LOG_TRACE(LOG_CTX_HAPI, "ptin_hapi_bridge_vlan_mode_mcast_set returned success");

  return L7_SUCCESS;
}

/**
 * Remove MC group for a specific vlan for bridging 
 * 
 * @param vlanId :    vlan to be configured
 * @param mcast_group : MC group id (if invalid, create a new 
 *                    one, and return it)
 * @param multicast_flags: Multicast Flags 
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_hapi_bridgeVlan_multicast_reset(L7_uint16 vlanId, L7_int mcast_group, L7_uint32 multicast_flag, L7_BOOL destroy_mcgroup)
{
  int unit, error;
  bcm_vlan_control_vlan_t control;

  /* Forwarding vlan, for MAC learning purposes (only if fwdvlan is valid) */
  if ( mcast_group <= 0 )
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Invalid MC group (%d)", mcast_group);
    return L7_FAILURE;
  }

  /* Run all units */
  BCM_UNIT_ITER(unit)
  {
    if (multicast_flag & BCM_MULTICAST_TYPE_VLAN)
    {
      /* Only for valid vlans */
      if (vlanId>0 && vlanId<4095)
      {
        /* Get current control definitions for this vlan */
        bcm_vlan_control_vlan_t_init(&control);

        error = bcm_vlan_control_vlan_get(unit, vlanId, &control);
        if (error != BCM_E_NONE)
        {
          PT_LOG_ERR(LOG_CTX_HAPI, "unit %d: Error getting vlan control structure! error=%d (%s)", unit, error, bcm_errmsg(error));
          return L7_FAILURE;
        }
        
        /* Associate a MC group */
        control.broadcast_group         = -1;
        control.unknown_multicast_group = -1;
        control.unknown_unicast_group   = -1;

        /* Apply new control definitions to this vlan */
        error = bcm_vlan_control_vlan_set(unit, vlanId, control);
        if ( error != BCM_E_NONE )
        {
          PT_LOG_ERR(LOG_CTX_HAPI, "unit %d: Error with bcm_vlan_control_vlan_set: error=%d (%s)", unit, error, bcm_errmsg(error));
          //return L7_FAILURE;
        }
      }
    }

    /* Destroy MC group */
    if (destroy_mcgroup)
    {
      error = bcm_multicast_destroy(unit, mcast_group);
      if (error != BCM_E_NONE)
      {
        PT_LOG_ERR(LOG_CTX_HAPI,"unit %d: Error with bcm_multicast_destroy (mcast_group=0x%08x): error=%d (\"%s\")",
                   unit, mcast_group, error, bcm_errmsg(error));
        return ptin_bcm_to_fp_error_code(error);
      }
    }
  }

  PT_LOG_TRACE(LOG_CTX_HAPI, "ptin_hapi_bridge_vlan_mode_mcast_set returned success");

  return L7_SUCCESS;
}

