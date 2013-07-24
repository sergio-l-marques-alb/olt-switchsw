#include <bcm/error.h>
#include <bcmx/vlan.h>
#include <bcmx/switch.h>

#include "ptin_hapi_xconnect.h"
#include "logger.h"

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
  int error;

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "bcm_switch_control_set(%d,%d,1)", bcm_unit, bcmSwitchSharedVlanEnable);

  /* Enable Shared Vlan for using one single vlan in E-Trees when learning MACs */
  if ( (error = bcmx_switch_control_set( bcmSwitchSharedVlanEnable, L7_TRUE )) != BCM_E_NONE )
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Error enabling shared vlan: error=%d (%s)", error, bcm_errmsg(error));
    return L7_FAILURE;
  }

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
L7_RC_t ptin_hapi_bridge_vlan_mode_set(L7_uint16 vlanId, L7_uint16 fwdVlanId, L7_BOOL cross_connects_apply, L7_BOOL mac_learning_apply)
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

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "ptin_hapi_bridge_vlan_mode_crossconnect_set returned success");

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

