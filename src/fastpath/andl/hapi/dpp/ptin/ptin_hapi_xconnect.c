#include <bcm/error.h>
#include <bcmx/vlan.h>
#include <bcmx/switch.h>
#include <bcmx/multicast.h>

#include "ptin_hapi_xconnect.h"
#include "ptin_hapi_l2.h"
#include "logger.h"
#include "ipc.h"
#include "bcm/vswitch.h"

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
  /* Insert init code here! */

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
  int error;
  bcm_vlan_control_vlan_t control;

  /* Get current control definitions for this vlan */
  bcm_vlan_control_vlan_t_init(&control);
  if ((error = bcmx_vlan_control_vlan_get(vlanId, &control))!=BCM_E_NONE)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Error getting vlan control structure! error=%d (%s)\r\n", error, bcm_errmsg(error));
    return L7_FAILURE;
  }

  /* Forwarding vlan, for MAC learning purposes (only if fwdvlan is valid) */
  if (fwdVlanId > 0)
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
    PT_LOG_ERR(LOG_CTX_HAPI, "Error with bcm_vlan_control_vlan_set: error=%d (%s)", error, bcm_errmsg(error));
    return L7_FAILURE;
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
  int error;
  bcm_vlan_control_vlan_t control;

  /* Forwarding vlan, for MAC learning purposes (only if fwdvlan is valid) */
  if (fwdVlanId == 0)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Invalid fwd vlan id: %u",fwdVlanId);
    return L7_FAILURE;
  }

  /* Get current control definitions for this vlan */
  bcm_vlan_control_vlan_t_init(&control);
  if ((error = bcmx_vlan_control_vlan_get(vlanId, &control))!=BCM_E_NONE)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Error getting vlan control structure! error=%d (%s)", error, bcm_errmsg(error));
    return L7_FAILURE;
  }
  
  /* Reset forward vlan */
  control.forwarding_vlan = fwdVlanId;

  /* Apply new control definitions to this vlan */
  if ( (error = bcm_vlan_control_vlan_set(bcm_unit, vlanId, control)) != BCM_E_NONE )
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Error with bcm_vlan_control_vlan_set: error=%d (%s)", error, bcm_errmsg(error));
    return L7_FAILURE;
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
  int error;
  bcm_vlan_control_vlan_t control;

  /* Validate vlan */
  if (vlanId == 0)
  {
    PT_LOG_ERR(LOG_CTX_HAPI,"Invalid vlan (%u)", vlanId);
    return L7_FAILURE;
  }

  /* Validate outer TPID */
  if ( outer_tpid == 0x0000 )
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "ERROR: TPID is null");
    return L7_FAILURE;
  }

  /* Get current control definitions for this vlan */
  bcm_vlan_control_vlan_t_init(&control);
  if ((error = bcmx_vlan_control_vlan_get(vlanId, &control))!=BCM_E_NONE)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Error getting vlan control structure! error=%d (%s)\r\n", error, bcm_errmsg(error));
    return L7_FAILURE;
  }

  /* Forwarding mode */
  control.outer_tpid = outer_tpid;

  /* Apply new control definitions to this vlan */
  if ( (error = bcmx_vlan_control_vlan_set(vlanId, control)) != BCM_E_NONE )
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Error with bcm_vlan_control_vlan_set: error=%d (%s)", error, bcm_errmsg(error));
    return L7_FAILURE;
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
  int error;
  bcm_vlan_control_vlan_t control;

  /* Validate vlan */
  if (vlanId == 0)
  {
    PT_LOG_ERR(LOG_CTX_HAPI,"Invalid vlan (%u)", vlanId);
    return L7_FAILURE;
  }

  /* Get current control definitions for this vlan */
  bcm_vlan_control_vlan_t_init(&control);
  if ((error = bcmx_vlan_control_vlan_get(vlanId, &control))!=BCM_E_NONE)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Error getting vlan control structure! error=%d (%s)\r\n", error, bcm_errmsg(error));
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
  if ( (error = bcm_vlan_control_vlan_set(bcm_unit, vlanId, control)) != BCM_E_NONE )
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Error with bcm_vlan_control_vlan_set: error=%d (%s)", error, bcm_errmsg(error));
    return L7_FAILURE;
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
  int error;
  bcm_vlan_control_vlan_t control;

  /* Validate vlan */
  if (vlanId == 0)
  {
    PT_LOG_ERR(LOG_CTX_HAPI,"Invalid vlan (%u)", vlanId);
    return L7_FAILURE;
  }

  /* Get current control definitions for this vlan */
  bcm_vlan_control_vlan_t_init(&control);
  if ((error = bcmx_vlan_control_vlan_get(vlanId, &control))!=BCM_E_NONE)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Error getting vlan control structure! error=%d (%s)\r\n", error, bcm_errmsg(error));
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
  if ( (error = bcm_vlan_control_vlan_set(bcm_unit, vlanId, control)) != BCM_E_NONE )
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Error with bcm_vlan_control_vlan_set: error=%d (%s)", error, bcm_errmsg(error));
    return L7_FAILURE;
  }

  PT_LOG_TRACE(LOG_CTX_HAPI, "ptin_hapi_bridge_vlan_mode_crossconnect_set returned success (enable=%u, double=%u)", cross_connects_apply, double_tag);

  return L7_SUCCESS;
}

/**
 * Create crossconnection between 2 leafs
 * 
 * @param lif1_id 
 * @param lif2_id 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_hapi_crossconnect_add(L7_int unit, L7_uint32 lif1_id, L7_uint32 lif2_id)
{
  bcm_error_t rv;            
  bcm_vswitch_cross_connect_t cross_connect;

  /* Add ports to VSWITCH instance */
  cross_connect.port1   = lif1_id;
  cross_connect.port2   = lif2_id;
  cross_connect.encap1  = 0;
  cross_connect.encap2  = 0;
  cross_connect.flags   = 0;

  rv = bcm_vswitch_cross_connect_add(unit, &cross_connect);
  if (rv != BCM_E_NONE) {
    PT_LOG_ERR(LOG_CTX_HAPI, "Error creating crossconnection between LIFs 0x%x and 0x%x", lif1_id, lif2_id);
    return L7_FAILURE;
  }
  PT_LOG_INFO(LOG_CTX_HAPI, "Created crossconnection between LIFs 0x%x and 0x%x", lif1_id, lif2_id);

  return L7_SUCCESS;
}

/**
 * Create crossconnection between 2 leafs
 * 
 * @param lif1_id 
 * @param lif2_id 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_hapi_crossconnect_remove(L7_int unit, L7_uint32 lif1_id, L7_uint32 lif2_id)
{
  bcm_error_t rv;            
  bcm_vswitch_cross_connect_t cross_connect;
              
  /* Add ports to VSWITCH instance */
  cross_connect.port1   = lif1_id;
  cross_connect.port2   = lif2_id;
  cross_connect.encap1  = 0;
  cross_connect.encap2  = 0;
  cross_connect.flags   = 0;

  rv = bcm_vswitch_cross_connect_delete(unit, &cross_connect);
  if (rv != BCM_E_NONE) {
    PT_LOG_ERR(LOG_CTX_HAPI, "Error removing crossconnection between LIFs 0x%x and 0x%x", lif1_id, lif2_id);
    return L7_FAILURE;
  }
  PT_LOG_INFO(LOG_CTX_HAPI, "Removed crossconnection between LIFs 0x%x and 0x%x", lif1_id, lif2_id);

  return L7_SUCCESS;
}

/**
 * Create VSI (Virtual Switch Instance)
 * 
 * @param vsi 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_hapi_vsi_create(L7_int unit, L7_uint16 vsi)
{
  bcm_error_t rv;

  /* Create VSWITCH instance */
  rv = bcm_vswitch_create_with_id(unit, vsi);
  if (rv != BCM_E_NONE && rv != BCM_E_EXISTS)
  {
    PT_LOG_ERR(LOG_CTX_HAPI,"error: bcm_vswitch_create_with_id: rv=%d", rv);
    return L7_FAILURE;
  }

  PT_LOG_TRACE(LOG_CTX_HAPI,"VSI %u created!", vsi);
  return L7_SUCCESS;
}

/**
 * Destroy VSI (Virtual Switch Instance)
 * 
 * @param vsi 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_hapi_vsi_destroy(L7_int unit, L7_uint16 vsi)
{
  bcm_error_t rv;

  /* Create VSWITCH instance */
  rv = bcm_vswitch_destroy(unit, vsi);
  if (rv != BCM_E_NONE && rv != BCM_E_NOT_FOUND)
  {
    PT_LOG_ERR(LOG_CTX_HAPI,"error: bcm_vswitch_destroy: rv=%d", rv);
    return L7_FAILURE;
  }

  PT_LOG_TRACE(LOG_CTX_HAPI,"VSI %u destroyed!", vsi);
  return L7_SUCCESS;
}

/**
 * Add a LIF to a VSI
 * 
 * @param unit 
 * @param vsi 
 * @param vlan_port_id 
 * 
 * @return int 
 */
L7_RC_t ptin_hapi_vsi_add(L7_int unit, L7_uint16 vsi, L7_uint32 vlan_port_id)
{
  bcm_error_t rv;

  /* Add ports to VSWITCH instance */
  rv = bcm_vswitch_port_add(unit, vsi, vlan_port_id);
  if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
    PT_LOG_ERR(LOG_CTX_HAPI,"Error, bcm_vswitch_port_add: rv=%d", rv);
    return rv;
  }

  PT_LOG_TRACE(LOG_CTX_HAPI,"Vlan_port_id 0x%x added to VSI %u!", vlan_port_id, vsi);

  return BCM_E_NONE;
}

/**
 * Remove a LIF from a VSI
 * 
 * @param unit 
 * @param vsi 
 * @param vlan_port_id 
 * 
 * @return int 
 */
L7_RC_t ptin_hapi_vsi_remove(L7_int unit, L7_uint16 vsi, L7_uint32 vlan_port_id)
{
  bcm_error_t rv;

  /* Remove port from VSWITCH instance */
  rv = bcm_vswitch_port_delete(unit, vsi, vlan_port_id);
  if (rv != BCM_E_NONE && rv != BCM_E_NOT_FOUND) {
    PT_LOG_ERR(LOG_CTX_HAPI,"Error, bcm_vswitch_port_delete: rv=%d", rv);
    return rv;
  }

  PT_LOG_TRACE(LOG_CTX_HAPI,"vlan_port_id 0x%x removed from VSI %u!", vlan_port_id, vsi);

  return BCM_E_NONE;
}

L7_uint32 vp_flags = 0;

void set_vp_flags(L7_uint32 flags)
{
  vp_flags = flags;

  printf("vp_flags set to 0x%x\r\n", vp_flags);
}

/**
 * Internal routine to create a LIF
 * 
 * @param lif_id (base id)
 * @param vsi 
 * @param bcm_port 
 * @param match_ovid 
 * @param match_ivid 
 * @param mcast_group 
 * @param egress_ovid 
 * @param egress_ivid 
 * @param pcp 
 * @param ethertype 
 * 
 * @return L7_uint32 : New LIF id (0 if any error)
 */
L7_uint32 ptin_hapi_lif_create(L7_uint32 lif_id, L7_uint16 vsi,
                               bcm_port_t bcm_port, L7_uint16 match_ovid, L7_uint16 match_ivid, L7_int mcast_group,
                               L7_uint16 egress_ovid, L7_uint16 egress_ivid,
                               L7_int pcp, L7_int ethertype)
{
  bcm_error_t error;
  L7_int criteria;
  bcm_if_t encap_id;

  /* create the virtual ports */
  bcm_vlan_port_t vlan_port;
  bcm_vlan_port_t_init(&vlan_port);

  if (match_ivid >= 1 && match_ivid <= 4095) 
  {
    if (pcp >= 0 && pcp <= 7)
      criteria = BCM_VLAN_PORT_MATCH_PORT_PCP_VLAN_STACKED;
    else
      criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED; 
  }
  else if (match_ovid > 0 && match_ovid < 4096)
  {
    if (pcp >= 0 && pcp <= 7)
      criteria = BCM_VLAN_PORT_MATCH_PORT_PCP_VLAN;
    else
      criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
  }
  else
  {
    criteria = BCM_VLAN_PORT_MATCH_PORT;
  }

  /* in direction PON -> network, match on stacked VLAN, translate to client ID on ingress */
  vlan_port.match_vlan        = match_ovid;
  vlan_port.match_inner_vlan  = match_ivid;
  vlan_port.match_pcp         = pcp & 0x7;
  vlan_port.match_ethertype   = ethertype & 0xffff;
  vlan_port.criteria          = criteria;
  vlan_port.egress_vlan       = egress_ovid;
  vlan_port.egress_inner_vlan = egress_ivid;
  vlan_port.port              = bcm_port;
  vlan_port.vlan_port_id      = lif_id;
  vlan_port.vsi = vsi;
  vlan_port.flags = (lif_id != 0) ? BCM_VLAN_PORT_WITH_ID : 0;

  PT_LOG_TRACE(LOG_CTX_HAPI, "criteria=0x%x (flags=0x%x) bcm_port=%d vlan_port.port=%d match_vlans=%u+%u pcp=%d etherType=0x%x vport=%d -> VSI %u",
               vlan_port.criteria, vlan_port.flags,
               bcm_port, vlan_port.port,
               vlan_port.match_vlan, vlan_port.match_inner_vlan, 
               vlan_port.match_pcp, vlan_port.match_ethertype,
               vlan_port.vlan_port_id, vlan_port.vsi);

  error = bcm_vlan_port_create(0, &vlan_port);
  if (error != BCM_E_NONE && error != BCM_E_EXISTS)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Error with bcm_vlan_port_create: error=%d (\"%s\")", error, bcm_errmsg(error));
    return 0;
  }

  PT_LOG_TRACE(LOG_CTX_HAPI, "vport=0x%x -> VSI=%u", vlan_port.vlan_port_id, vlan_port.vsi);

  /* For BCM_VLAN_PORT_MATCH_PORT_VLAN criteria, inner VLAN should not be considered */
  error = bcm_vlan_control_port_set(bcm_unit, vlan_port.vlan_port_id, bcmVlanPortDoubleLookupEnable, (criteria != BCM_VLAN_PORT_MATCH_PORT));
  if (error != BCM_E_NONE)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Error configuring bcmVlanPortDoubleLookupEnable=%u to lif 0x%x: error=%d (\"%s\")",
               (criteria != BCM_VLAN_PORT_MATCH_PORT_VLAN), vlan_port.vlan_port_id, error, bcm_errmsg(error));
    return 0;
  }

  /* Configure replication lists */
  if (mcast_group != 0)
  {
    error = bcm_multicast_vlan_encap_get(bcm_unit, mcast_group, vlan_port.port, vlan_port.vlan_port_id, &encap_id);
    if (error != BCM_E_NONE)
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "Error obtaining encap_id from mcast_group %u, port %u, lif 0x%x: error=%d (\"%s\")",
                 mcast_group, vlan_port.port, vlan_port.vlan_port_id, error, bcm_errmsg(error));
      return 0;
    }

    error = bcm_multicast_egress_add(bcm_unit, mcast_group, vlan_port.port, encap_id);
    if (error == BCM_E_EXISTS)
    {
      PT_LOG_WARN(LOG_CTX_HAPI, "Port %u already exists at mcgroup %u: error=%d (\"%s\")",
                  vlan_port.port, mcast_group, error, bcm_errmsg(error));
    }
    else if (error != BCM_E_NONE)
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "Error adding port %u to mcgroup %u: error=%d (\"%s\")",
                 vlan_port.port, mcast_group, error, bcm_errmsg(error));
      return 0;
    }

    PT_LOG_TRACE(LOG_CTX_HAPI, "Port %u added to mcgroup %u", vlan_port.port, mcast_group);
  }

  /* Configures the information needed to generate alarms related to MAC Limit */
  ptin_hapi_vport_maclimit_alarmconfig(vlan_port.vlan_port_id, bcm_port, match_ovid);

  PT_LOG_DEBUG(LOG_CTX_HAPI, "ptin_hapi_vp_create: vport 0x%x created for VSI %u", vlan_port.vlan_port_id, vlan_port.vsi);

  /* Return new VLAN PORT ID */
  return vlan_port.vlan_port_id;
}

/**
 * Internal routine to delete a LIF
 * 
 * @param virtual_gport 
 * @param mcast_group 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_hapi_lif_delete(L7_int virtual_gport, L7_int mcast_group)
{
  bcm_if_t encap_id;
  bcm_vlan_port_t vlan_port;
  bcm_error_t error;

  PT_LOG_TRACE(LOG_CTX_HAPI, "virtual_port=0x%u, mcast_group=%u", virtual_gport, mcast_group);

  /* Validate interface */
  if ( virtual_gport == 0 )
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "ERROR: Invalid interfaces");
    return L7_FAILURE;
  }

  /* Find virtual ports */
  bcm_vlan_port_t_init(&vlan_port);

  /* If virtual port id is provided, use it */
  vlan_port.flags = BCM_VLAN_PORT_WITH_ID;
  vlan_port.vlan_port_id = virtual_gport;

  error=bcm_vlan_port_find(0, &vlan_port);
  if (error != BCM_E_NONE)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Error with bcm_vlan_port_find: error=%d (\"%s\")", error, bcm_errmsg(error));
    return L7_FAILURE;
  }

  PT_LOG_TRACE(LOG_CTX_HAPI, "gport=0x%x => vlan_port.port=%d vport=%d",
               virtual_gport, vlan_port.port, vlan_port.vlan_port_id);
  PT_LOG_TRACE(LOG_CTX_HAPI, "flags=0x%08x criteria=0x%08x match_vlan=%u match_ivlan=%u egress_vlan=%u egress_ivlan=%u",
               vlan_port.flags, vlan_port.criteria, vlan_port.match_vlan, vlan_port.match_inner_vlan, vlan_port.egress_vlan, vlan_port.egress_inner_vlan);

  /* Configure replication lists */
  if (0 /*mcast_group != 0*/)
  {
    error = bcm_multicast_vlan_encap_get(bcm_unit, mcast_group, vlan_port.port, vlan_port.vlan_port_id, &encap_id);
    if (error != BCM_E_NONE)
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "Error obtaining encap_id from mcast_group %u, port %u, lif 0x%x: error=%d (\"%s\")",
                 mcast_group, vlan_port.port, vlan_port.vlan_port_id, error, bcm_errmsg(error));
      return L7_FAILURE;
    }

    error = bcm_multicast_egress_delete(bcm_unit, mcast_group, vlan_port.port, encap_id);
    if (error == BCM_E_NOT_FOUND)
    {
      PT_LOG_WARN(LOG_CTX_HAPI, "Port %u don't exist at mcgroup %u: error=%d (\"%s\")",
                  vlan_port.port, mcast_group, error, bcm_errmsg(error));
    }
    if (error != BCM_E_NONE) 
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "Error adding port %u to mcgroup %u: error=%d (\"%s\")",
                 vlan_port.port, mcast_group, error, bcm_errmsg(error));
      return L7_FAILURE;
    }

    PT_LOG_TRACE(LOG_CTX_HAPI, "Port %u removed from mcgroup %u", vlan_port.port, mcast_group);
  }

  /* Destroy virtual port */
  error = bcm_vlan_port_destroy(0, virtual_gport);
  if (error != BCM_E_NONE && error != BCM_E_NOT_FOUND)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Error with bcm_vlan_port_destroy: error=%d (\"%s\")", error, bcm_errmsg(error));
    return L7_FAILURE;
  }

  /* Remove MAC addresses related to this virtual port */
  error = bcm_l2_addr_delete_by_port(0, -1, virtual_gport, 0);
  if (error != BCM_E_NONE && error != BCM_E_NOT_FOUND)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Error removing MAC addresses related to this vport: error=%d (\"%s\")", error, bcm_errmsg(error));
  }

  PT_LOG_TRACE(LOG_CTX_HAPI, "ptin_hapi_vp_remove: vport 0x%x removed", virtual_gport);

  return L7_SUCCESS;
}

/**
 * Create Virtual port
 * 
 * @param dapiPort      : PON port 
 * @param vsi           : VSI 
 * @param match_ovid    : external outer vlan (GEMid)
 * @param match_ivid    : external inner vlan (UNIVLAN)
 * @param egress_ovid   : outer vlan inside switch
 * @param egress_ivid   : inner vlan inside switch 
 * @param pcp           : Packet's priority 
 * @param ethertype     : packet's ethertype 
 * @param mcast_group   : mc group (-1 to create) 
 * @param virtual_gport : vport id (to be returned) 
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_hapi_vp_create(ptin_dapi_port_t *dapiPort, L7_uint16 vsi,
                            L7_uint16 match_ovid, L7_uint16 match_ivid,
                            L7_uint16 egress_ovid, L7_uint16 egress_ivid,
                            L7_int pcp, L7_int ethertype,
                            L7_int mcast_group,
                            L7_int *virtual_gport)
{
  DAPI_PORT_t  *dapiPortPtr;
  BROAD_PORT_t *hapiPortPtr;
  L7_uint32 lif_id;

  PT_LOG_TRACE(LOG_CTX_HAPI, "port={%d,%d,%d}, oVlanId=%u iVlanId=%u => oVlanId=%u iVlanId=%u (mcgroup=%u)",
            dapiPort->usp->unit,dapiPort->usp->slot,dapiPort->usp->port,
            match_ovid, match_ivid, egress_ovid, egress_ivid, mcast_group);

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

  /* Reference LIF ID */
  lif_id = 0;
  if (virtual_gport != L7_NULLPTR && *virtual_gport > 0)
  {
    lif_id = *virtual_gport;
  }

  /* Create LIF */
  lif_id = ptin_hapi_lif_create(lif_id, vsi, hapiPortPtr->bcm_port, match_ovid, match_ivid, mcast_group, egress_ovid, egress_ivid, pcp, ethertype);

  /* Error? */
  if (lif_id == 0)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Error creating LIF");
    return L7_FAILURE;
  }

  /* Return vport id */
  if (virtual_gport != L7_NULLPTR)
  {
    *virtual_gport = lif_id;
  }

  PT_LOG_DEBUG(LOG_CTX_HAPI, "ptin_hapi_vp_create: vport 0x%x created for VSI %u", lif_id, vsi);

  return L7_SUCCESS;
}

/**
 * Remove virtual port
 * 
 * @param dapiPort      : PON port
 * @param virtual_gport : vport id 
 * @param mcast_group   : multicast group
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_hapi_vp_remove(ptin_dapi_port_t *dapiPort,
                            L7_int virtual_gport,
                            L7_int mcast_group)
{
  PT_LOG_TRACE(LOG_CTX_HAPI, "port={%d,%d,%d}, virtual_port=0x%u, mcast_group=%u",
            dapiPort->usp->unit,dapiPort->usp->slot,dapiPort->usp->port,
            virtual_gport, mcast_group);

  return ptin_hapi_lif_delete(virtual_gport, mcast_group);
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

  /* Create a multicast group, if given multicast group is not valid */
  if ( mc_group <= 0 )
  {
    if ((error=bcm_multicast_create(0, multicast_flag, &mc_group)) != BCM_E_NONE)
    {
      PT_LOG_ERR(LOG_CTX_HAPI,"Error with bcm_multicast_create(0, %d, &mcast_group): error=%d (\"%s\")",
              BCM_MULTICAST_TYPE_VLAN, error, bcm_errmsg(error));
      return L7_FAILURE;
    }
    *mcast_group = mc_group;
  }

  switch (multicast_flag)
  {
  case BCM_MULTICAST_TYPE_L2:  
    /* get the encapsulation id */
    if ((error=bcm_multicast_l2_encap_get(0, mc_group, hapiPortPtr->bcmx_lport, -1, &encap_id))!=BCM_E_NONE)
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "Error with bcm_multicast_l2_encap_get: error=%d (\"%s\")",error, bcm_errmsg(error));
      return L7_FAILURE;
    }    
    break;      
  case BCM_MULTICAST_TYPE_L3:
    /* get the encapsulation id */   
    if ((error=bcm_multicast_l3_encap_get(0, mc_group, hapiPortPtr->bcmx_lport, virtual_gport, &encap_id))!=BCM_E_NONE)
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "Error with bcm_multicast_l3_encap_get: error=%d (\"%s\")",error, bcm_errmsg(error));
      return L7_FAILURE;
    }    
    break;    
  default:
    PT_LOG_ERR(LOG_CTX_HAPI, "Multicast Flag Not Supported :0x%08X", multicast_flag);
    return L7_NOT_SUPPORTED;
  }  

  /* add network port to multicast group */
  if ((error=bcm_multicast_egress_add(0, mc_group, hapiPortPtr->bcmx_lport, encap_id))!=BCM_E_NONE)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Error with bcm_multicast_egress_add: error=%d (\"%s\")",error, bcm_errmsg(error));
    return L7_FAILURE;
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

  switch (multicast_flag)
  {
  case BCM_MULTICAST_TYPE_L2:  
    /* get the encapsulation id */   
    error = bcm_multicast_l2_encap_get(0, mcast_group, hapiPortPtr->bcmx_lport, -1, &encap_id);
    if (error != BCM_E_NONE)
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "Error with bcm_multicast_l2_encap_get: error=%d (\"%s\")",error, bcm_errmsg(error));
      return L7_FAILURE;
    }
    break;  
  case BCM_MULTICAST_TYPE_L3:
    /* get the encapsulation id */   
    if ((error=bcm_multicast_l3_encap_get(0, mcast_group, hapiPortPtr->bcmx_lport, virtual_gport, &encap_id))!=BCM_E_NONE)
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "Error with bcm_multicast_l3_encap_get: error=%d (\"%s\")",error, bcm_errmsg(error));
      return L7_FAILURE;
    }
    break;
  default:
    PT_LOG_ERR(LOG_CTX_HAPI, "Multicast Flag Not Supported :0x%08X", multicast_flag);
    return L7_NOT_SUPPORTED;
  }
  
  error = bcm_multicast_egress_delete(0, mcast_group, hapiPortPtr->bcmx_lport, encap_id);
  if (error != BCM_E_NONE && error != BCM_E_NOT_FOUND)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Error with bcm_multicast_egress_delete: error=%d (\"%s\")",error, bcm_errmsg(error));
    return L7_FAILURE;
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
  bcm_error_t error;

  /* Forwarding vlan, for MAC learning purposes (only if fwdvlan is valid) */
  if ( mcast_group <= 0 )
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Invalid MC group (%d)", mcast_group);
    return L7_FAILURE;
  }

  PT_LOG_TRACE(LOG_CTX_HAPI, "mcast_group=%d", mcast_group);

  error = bcm_multicast_egress_delete_all(0, mcast_group);
  if (error != BCM_E_NONE && error != BCM_E_NOT_FOUND)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Error with bcm_multicast_egress_delete_all: error=%d (\"%s\")",error, bcm_errmsg(error));
    return L7_FAILURE;
  }

  /* Destroy MC group */
  if (destroy_mcgroup)
  {
    error = bcm_multicast_destroy(0, mcast_group);
    if (error != BCM_E_NONE && error != BCM_E_NOT_FOUND)
    {
      PT_LOG_ERR(LOG_CTX_HAPI,"Error with bcm_multicast_create(0, %d, &mcast_group): error=%d (\"%s\")",
              BCM_MULTICAST_TYPE_VLAN, error, bcm_errmsg(error));
      return L7_FAILURE;
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
  int error;
  bcm_multicast_t mc_group;

  /* Validate vlan */
  if (vlanId == 0)
  {
    PT_LOG_ERR(LOG_CTX_HAPI,"Invalid vlan (%u)", vlanId);
    return L7_FAILURE;
  }

  /* Flags */
  multicast_flag |= BCM_MULTICAST_EGRESS_GROUP | BCM_MULTICAST_WITH_ID;

  /* Get given MC group id */
  mc_group = vlanId;

  /* Create Multicast Group */
  PT_LOG_TRACE(LOG_CTX_HAPI, "Going to create MC group: vsi=%u flags=0x%x", vlanId, multicast_flag);

  /* Create a multicast group, if given multicast group is not valid */
  error=bcm_multicast_create(0, multicast_flag, &mc_group);

  if (error != BCM_E_NONE && error != BCM_E_EXISTS)
  {
    PT_LOG_ERR(LOG_CTX_HAPI,"Error with bcm_multicast_create(0,0x%x, &mcast_group): error=%d (\"%s\")",
            multicast_flag, error, bcm_errmsg(error));
    return L7_FAILURE;
  }

  if (error == BCM_E_NONE)
  {
    PT_LOG_DEBUG(LOG_CTX_HAPI, "mc_group=0x%08x created!", mc_group);
  }
  else
  {
    PT_LOG_WARN(LOG_CTX_HAPI, "mc_group=0x%08x already exists!", mc_group);
  }

  /* Return MC group */
  if (mcast_group != L7_NULLPTR)
  {
    *mcast_group = mc_group; 
  }

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
L7_RC_t ptin_hapi_bridgeVlan_multicast_reset(L7_uint16 vlanId, L7_int mcast_group)
{
  int error;

  /* Forwarding vlan, for MAC learning purposes (only if fwdvlan is valid) */
  if ( mcast_group <= 0 )
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "No MC group to be removed (%d)", mcast_group);
    return L7_SUCCESS;
  }

  if (vlanId != 0)
  {
    /* Reset flooding settings */
    error = ptin_hapi_bridgeVlan_flood_set(0, vlanId, 0, 0, 0); 
    if (error != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "Error with ptin_hapi_bridgeVlan_flood_set: error=%d");
    }
  }

  /* Destroy MC group */
  error=bcm_multicast_destroy(0, mcast_group);

  if (error == BCM_E_NONE) 
  {
    PT_LOG_TRACE(LOG_CTX_HAPI,"bcm_multicast_destroy (mcast_group=0x%08x) successfull", mcast_group);
  }
  else if (error == BCM_E_NOT_FOUND)
  {
    PT_LOG_WARN(LOG_CTX_HAPI,"mcast_group=0x%08x does not exist: error=%d (\"%s\")", mcast_group, error, bcm_errmsg(error));
  }
  else
  {
    PT_LOG_ERR(LOG_CTX_HAPI,"Error with bcm_multicast_destroy (mcast_group=0x%08x): error=%d (\"%s\")", mcast_group, error, bcm_errmsg(error));
    return L7_FAILURE;
  }

  PT_LOG_TRACE(LOG_CTX_HAPI, "ptin_hapi_bridge_vlan_mode_mcast_set returned success");

  return L7_SUCCESS;
}


/**
 * Define flooding settings 
 *  
 * @param lif_id :    LIF id
 * @param vlanId :    vlan to be configured
 * @param mcast_group : MC group id (if invalid, create a new 
 *                    one, and return it)
 * @param mcgroup_flood_unkn_uc : flood unknown Unicast 
 * @param mcgroup_flood_unkn_mc : flood unknown Multicast 
 * @param mcgroup_flood_bc : flood Broadcast 
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_hapi_bridgeVlan_flood_set(L7_uint32 lif_id, L7_uint16 vlanId,
                                       L7_int mcgroup_flood_unkn_uc, L7_int mcgroup_flood_unkn_mc, L7_int mcgroup_flood_bc)
{
  bcm_vlan_control_vlan_t control;
  bcm_error_t rv;

  PT_LOG_TRACE(LOG_CTX_HAPI, "lif=0x%x, vlanId %u: mcgroup_unkn_uc=%d mcgroup_unkn_mc=%d mcgroup_unkn_bc=%d", lif_id, vlanId,
               mcgroup_flood_unkn_uc, mcgroup_flood_unkn_mc, mcgroup_flood_bc);

  /* If LIF id is specified */
  if (lif_id != 0)
  {
    /* Unknown Unicast */
    rv = bcm_port_control_set(bcm_unit, lif_id, bcmPortControlFloodUnknownUcastGroup, mcgroup_flood_unkn_uc);
    if (rv != BCM_E_NONE)
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "Error with bcm_port_control_set(FloodUnknownUcastGroup): rv=%d (%s)", rv, bcm_errmsg(rv));
      return L7_FAILURE;
    }

    /* Unknown Multicast */
    rv = bcm_port_control_set(bcm_unit, lif_id, bcmPortControlFloodUnknownMcastGroup, mcgroup_flood_unkn_mc);
    if (rv != BCM_E_NONE)
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "Error with bcm_port_control_set(FloodUnknownMcastGroup): rv=%d (%s)", rv, bcm_errmsg(rv));
      return L7_FAILURE;
    }

    /* Unknown Multicast */
    rv = bcm_port_control_set(bcm_unit, lif_id, bcmPortControlFloodBroadcastGroup, mcgroup_flood_bc);
    if (rv != BCM_E_NONE)
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "Error with bcm_port_control_set(FloodBroadcastGroup): rv=%d (%s)", rv, bcm_errmsg(rv));
      return L7_FAILURE;
    }

    PT_LOG_TRACE(LOG_CTX_HAPI, "ptin_hapi_bridgeVlan_flood_set for LIF 0x%x returned success", lif_id);
  }

  /* If VLAN ID / VSI is specified */
  if (vlanId != 0)
  {
    /* Get current control definitions for this vlan */
    bcm_vlan_control_vlan_t_init(&control);

    rv = bcm_vlan_control_vlan_get(bcm_unit, vlanId, &control);
    if (rv != BCM_E_NONE)
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "Error getting vlan control structure! rv=%d (%s)", rv, bcm_errmsg(rv));
      return L7_FAILURE;
    }
    
    /* Associate a MC group */
    control.broadcast_group         = mcgroup_flood_bc;
    control.unknown_multicast_group = mcgroup_flood_unkn_mc;
    control.unknown_unicast_group   = mcgroup_flood_unkn_uc;

    /* Apply new control definitions to this vlan */
    rv = bcm_vlan_control_vlan_set(bcm_unit, vlanId, control);
    if (rv != BCM_E_NONE)
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "Error with bcm_vlan_control_vlan_set: rv=%d (%s)", rv, bcm_errmsg(rv));
      return L7_FAILURE;
    }
    PT_LOG_TRACE(LOG_CTX_HAPI, "ptin_hapi_bridgeVlan_flood_set for VSI %u returned success", vlanId);
  }

  return L7_SUCCESS;
}

