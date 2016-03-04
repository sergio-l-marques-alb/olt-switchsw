#include <bcm/error.h>
#include <bcmx/vlan.h>
#include <bcmx/port.h>

#include "broad_policy.h"
#include "ptin_hapi_xlate.h"
#include "logger.h"

/********************************************************************
 * DEFINES
 ********************************************************************/

/* Max number of translations */
#define FREE_RESOURCES_XLATE_INGRESS  8000
#define FREE_RESOURCES_XLATE_EGRESS   8000

/********************************************************************
 * TYPES DEFINITION
 ********************************************************************/

/********************************************************************
 * INTERNAL VARIABLES
 ********************************************************************/

/* Available resources for translation: This is for the 5668x switches */
static L7_uint16 resources_xlate_ingress = FREE_RESOURCES_XLATE_INGRESS;
static L7_uint16 resources_xlate_egress  = FREE_RESOURCES_XLATE_EGRESS;

/* Variables used in all functions (saves stack allocations) */
static DAPI_PORT_t  *dapiPortPtr;
static BROAD_PORT_t *hapiPortPtr;

/* Advanced translations active? */
L7_uint32 advanced_vlan_editing = 0;
                                                                                                                                                                                      
L7_int tagformat_single_class_id = 2;  /* Single tagged packets */
L7_int tagformat_double_class_id = 6;  /* Double tagged packets */

L7_int action_ingress_oswap_inone_id = 4;  /* Egress action 1: swap+none */
L7_int action_ingress_oswap_iswap_id = 5;  /* Egress action 1: swap+swap */
L7_int action_egress_oswap_inone_id  = 2;  /* Egress action 1: swap+none */
L7_int action_egress_oswap_iswap_id  = 3;  /* Egress action 1: swap+swap */

L7_int vep_ingress_single_class_id = 2;  /* VLAN EDIT PROFILE class id */
L7_int vep_ingress_double_class_id = 3;  /* VLAN EDIT PROFILE class id */
L7_int vep_egress_single_class_id  = 4;  /* VLAN EDIT PROFILE class id */
L7_int vep_egress_double_class_id  = 5;  /* VLAN EDIT PROFILE class id */



/********************************************************************
 * MACROS AND INLINE FUNCTIONS
 ********************************************************************/

/********************************************************************
 * INTERNAL FUNCTIONS PROTOTYPES
 ********************************************************************/

/**
 * Define class ports for egress translations. 
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
static L7_RC_t ptin_hapi_xlate_egress_portsGroup_init(void);


/********************************************************************
 * EXTERNAL FUNCTIONS IMPLEMENTATION
 ********************************************************************/

/**
 * Initialize translations for XGS4 switches
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_hapi_xlate_init(void)
{
  L7_int port, bcm_port;
  bcm_port_tpid_class_t port_tpid_class;
  bcm_vlan_action_set_t action;
  bcm_vlan_translate_action_class_t action_class;
  bcm_error_t rv = BCM_E_NONE;
  L7_RC_t rc = L7_SUCCESS;

  /* Is advanced translations active? */
  advanced_vlan_editing = soc_property_get(bcm_unit,"bcm886xx_vlan_translate_mode",0);

  /* Change Ingress Vlan Translate Keys for all physical ports (LAGs are included through their physical ports) */
  for (port=0; port<ptin_sys_number_of_ports; port++)
  {
    if (hapi_ptin_bcmPort_get(port, &bcm_port) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "Error getting bcm unit id");
      rc = L7_FAILURE;
      continue;
    }

    /* Egress Class set */
    rv = bcm_port_class_set(bcm_unit, bcm_port, bcmPortClassId, bcm_port);
    if (rv != BCM_E_NONE)
    {
      PT_LOG_ERR(LOG_CTX_STARTUP, "error: bcm_port_class_set to bcm_port %d: rv=%d (%s)", bcm_port, rv, bcm_errmsg(rv));
      return L7_FAILURE;
    }
    PT_LOG_TRACE(LOG_CTX_STARTUP, "bcm_port_class_set to bcm_port %d defined", bcm_port);

    /* Define tag structure for this port */
    if (advanced_vlan_editing)
    {
      /* Single tag format */
      bcm_port_tpid_class_t_init(&port_tpid_class);
      port_tpid_class.port = bcm_port;
      port_tpid_class.tpid1 = 0x8100;
      port_tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_ANY;
      port_tpid_class.tag_format_class_id = tagformat_single_class_id;
      port_tpid_class.flags = BCM_PORT_TPID_CLASS_INNER_C;
      port_tpid_class.vlan_translation_action_id = 0;

      rv = bcm_port_tpid_class_set(bcm_unit, &port_tpid_class);
      if (rv != BCM_E_NONE) {
        PT_LOG_ERR(LOG_CTX_STARTUP, "Error, in bcm_port_tpid_class_set, bcm_port=%d: rv=%d", bcm_port, rv);
        return L7_FAILURE;
      }
      PT_LOG_TRACE(LOG_CTX_STARTUP, "bcm_port %u: Created new tag format for single-tagged traffic tag_format_class_id=%u",
                   bcm_port, port_tpid_class.tag_format_class_id);

      /* Double tag format */
      bcm_port_tpid_class_t_init(&port_tpid_class);
      port_tpid_class.port = bcm_port;
      port_tpid_class.tpid1 = 0x8100;
      port_tpid_class.tpid2 = 0x8100;
      port_tpid_class.tag_format_class_id = tagformat_double_class_id;
      port_tpid_class.flags = 0;
      port_tpid_class.vlan_translation_action_id = 0;

      rv = bcm_port_tpid_class_set(bcm_unit, &port_tpid_class);
      if (rv != BCM_E_NONE) {
        PT_LOG_ERR(LOG_CTX_STARTUP, "Error, in bcm_port_tpid_class_set, bcm_port=%d: rv=%d", bcm_port, rv);
        return L7_FAILURE;
      }
      PT_LOG_TRACE(LOG_CTX_STARTUP, "bcm_port %u: Created new tag format for double-tagged traffic tag_format_class_id=%u",
                   bcm_port, port_tpid_class.tag_format_class_id);
    }
  }

  /* Define Action for Replace (Pop+Push) */
  if (advanced_vlan_editing)
  {
    /* Create ingress/egress translation actions: swap+none */
    rv = bcm_vlan_translate_action_id_create(bcm_unit, BCM_VLAN_ACTION_SET_INGRESS | BCM_VLAN_ACTION_SET_WITH_ID,
                                             &action_ingress_oswap_inone_id);
    if (rv != BCM_E_NONE) {
     PT_LOG_ERR(LOG_CTX_STARTUP, "Error, bcm_vlan_translate_action_id_create: rv=%d", rv);
     return L7_FAILURE;
    }
    /* Create egress translation action: swap+swap */
    rv = bcm_vlan_translate_action_id_create(bcm_unit, BCM_VLAN_ACTION_SET_EGRESS | BCM_VLAN_ACTION_SET_WITH_ID,
                                             &action_egress_oswap_inone_id);
    if (rv != BCM_E_NONE) {
     PT_LOG_ERR(LOG_CTX_STARTUP, "Error, bcm_vlan_translate_action_id_create: rv=%d", rv);
     return L7_FAILURE;
    }

    bcm_vlan_action_set_t_init(&action);
    action.ut_outer = bcmVlanActionAdd;
    action.ut_inner = bcmVlanActionNone;
    action.ot_outer = bcmVlanActionReplace;
    action.ot_inner = bcmVlanActionNone;
    action.dt_outer = bcmVlanActionReplace;
    action.dt_inner = bcmVlanActionNone;
    action.outer_tpid = 0x8100;
    action.inner_tpid = 0x8100;

    rv = bcm_vlan_translate_action_id_set(bcm_unit, BCM_VLAN_ACTION_SET_INGRESS, action_ingress_oswap_inone_id, &action);
    if (rv != BCM_E_NONE)
    {
      PT_LOG_ERR(LOG_CTX_STARTUP, "Error, bcm_vlan_translate_action_id_set swap: rv=%d", rv);
      return L7_FAILURE;
    }
    PT_LOG_TRACE(LOG_CTX_STARTUP, "Created new ingress xlate action with for swap operation: action_id=%u", action_ingress_oswap_inone_id);

    rv = bcm_vlan_translate_action_id_set(bcm_unit, BCM_VLAN_ACTION_SET_EGRESS, action_egress_oswap_inone_id, &action);
    if (rv != BCM_E_NONE)
    {
      PT_LOG_ERR(LOG_CTX_STARTUP, "Error, bcm_vlan_translate_action_id_set swap: rv=%d", rv);
      return L7_FAILURE;
    }
    PT_LOG_TRACE(LOG_CTX_STARTUP, "Created new egress xlate action with for swap operation: action_id=%u", action_egress_oswap_inone_id);


    /* Create ingress/egress translation actions: swap+none */
    rv = bcm_vlan_translate_action_id_create(bcm_unit, BCM_VLAN_ACTION_SET_INGRESS | BCM_VLAN_ACTION_SET_WITH_ID,
                                             &action_ingress_oswap_iswap_id);
    if (rv != BCM_E_NONE) {
     PT_LOG_ERR(LOG_CTX_STARTUP, "Error, bcm_vlan_translate_action_id_create: rv=%d", rv);
     return L7_FAILURE;
    }
    /* Create egress translation action: swap+swap */
    rv = bcm_vlan_translate_action_id_create(bcm_unit, BCM_VLAN_ACTION_SET_EGRESS | BCM_VLAN_ACTION_SET_WITH_ID,
                                             &action_egress_oswap_iswap_id);
    if (rv != BCM_E_NONE) {
     PT_LOG_ERR(LOG_CTX_STARTUP, "Error, bcm_vlan_translate_action_id_create: rv=%d", rv);
     return L7_FAILURE;
    }

    bcm_vlan_action_set_t_init(&action);
    action.ut_outer = bcmVlanActionAdd;
    action.ut_inner = bcmVlanActionAdd;
    action.ot_outer = bcmVlanActionReplace;
    action.ot_inner = bcmVlanActionAdd;
    action.dt_outer = bcmVlanActionReplace;
    action.dt_inner = bcmVlanActionReplace;
    action.outer_tpid = 0x8100;
    action.inner_tpid = 0x8100;

    rv = bcm_vlan_translate_action_id_set(bcm_unit, BCM_VLAN_ACTION_SET_INGRESS, action_ingress_oswap_iswap_id, &action);
    if (rv != BCM_E_NONE)
    {
      PT_LOG_ERR(LOG_CTX_STARTUP, "Error, bcm_vlan_translate_action_id_set swap: rv=%d", rv);
      return L7_FAILURE;
    }
    PT_LOG_TRACE(LOG_CTX_STARTUP, "Created new ingress xlate action with for swap operation: action_id=%u", action_ingress_oswap_iswap_id);

    rv = bcm_vlan_translate_action_id_set(bcm_unit, BCM_VLAN_ACTION_SET_EGRESS, action_egress_oswap_iswap_id, &action);
    if (rv != BCM_E_NONE)
    {
      PT_LOG_ERR(LOG_CTX_STARTUP, "Error, bcm_vlan_translate_action_id_set swap: rv=%d", rv);
      return L7_FAILURE;
    }
    PT_LOG_TRACE(LOG_CTX_STARTUP, "Created new egress xlate action with for swap operation: action_id=%u", action_egress_oswap_iswap_id);

    /* Associate TAG class, EDIT VLAN profile and ACTION class */

    /* Ingress VEP: Single tagged packets, swap operation */
    bcm_vlan_translate_action_class_t_init(&action_class);
    action_class.tag_format_class_id        = tagformat_single_class_id;
    action_class.vlan_edit_class_id         = vep_ingress_single_class_id;
    action_class.vlan_translation_action_id = action_ingress_oswap_inone_id;
    action_class.flags = BCM_VLAN_ACTION_SET_INGRESS;

    rv = bcm_vlan_translate_action_class_set(bcm_unit, &action_class);
    if (rv != BCM_E_NONE) {
      PT_LOG_ERR(LOG_CTX_STARTUP, "Error, bcm_vlan_translate_action_class_set: rv=%d", rv);
      return L7_FAILURE;
    }
    PT_LOG_INFO(LOG_CTX_HAPI,"Created INGRESS XLATE rule with tag_format_class_id=%u, edit_class_id=%u, action_id=%u", tagformat_single_class_id, vep_ingress_single_class_id, action_egress_oswap_inone_id);

    /* Ingress VEP: Double tagged packets, swap+swap operations */
    bcm_vlan_translate_action_class_t_init(&action_class);
    action_class.tag_format_class_id        = tagformat_double_class_id;
    action_class.vlan_edit_class_id         = vep_ingress_double_class_id;
    action_class.vlan_translation_action_id = action_ingress_oswap_iswap_id;
    action_class.flags = BCM_VLAN_ACTION_SET_INGRESS;

    rv = bcm_vlan_translate_action_class_set(bcm_unit, &action_class);
    if (rv != BCM_E_NONE) {
      PT_LOG_ERR(LOG_CTX_STARTUP, "Error, bcm_vlan_translate_action_class_set: rv=%d", rv);
      return L7_FAILURE;
    }
    PT_LOG_INFO(LOG_CTX_HAPI,"Created INGRESS XLATE rule with tag_format_class_id=%u, edit_class_id=%u, action_id=%u", tagformat_double_class_id, vep_ingress_double_class_id, action_egress_oswap_iswap_id);


    /* Egress VEP: Single tagged packets, swap operation */
    bcm_vlan_translate_action_class_t_init(&action_class);
    action_class.tag_format_class_id        = tagformat_single_class_id;
    action_class.vlan_edit_class_id         = vep_egress_single_class_id;
    action_class.vlan_translation_action_id = action_egress_oswap_inone_id;
    action_class.flags = BCM_VLAN_ACTION_SET_EGRESS;

    rv = bcm_vlan_translate_action_class_set(bcm_unit, &action_class);
    if (rv != BCM_E_NONE) {
      PT_LOG_ERR(LOG_CTX_STARTUP, "Error, bcm_vlan_translate_action_class_set: rv=%d", rv);
      return L7_FAILURE;
    }
    PT_LOG_INFO(LOG_CTX_HAPI,"Created EGRESS XLATE rule with tag_format_class_id=%u, edit_class_id=%u, action_id=%u", tagformat_single_class_id, vep_egress_single_class_id, action_egress_oswap_inone_id);

    /* Egress VEP: Double tagged packets, swap+swap operations */
    bcm_vlan_translate_action_class_t_init(&action_class);
    action_class.tag_format_class_id        = tagformat_double_class_id;
    action_class.vlan_edit_class_id         = vep_egress_double_class_id;
    action_class.vlan_translation_action_id = action_egress_oswap_iswap_id;
    action_class.flags = BCM_VLAN_ACTION_SET_EGRESS;

    rv = bcm_vlan_translate_action_class_set(bcm_unit, &action_class);
    if (rv != BCM_E_NONE) {
      PT_LOG_ERR(LOG_CTX_STARTUP, "Error, bcm_vlan_translate_action_class_set: rv=%d", rv);
      return L7_FAILURE;
    }
    PT_LOG_INFO(LOG_CTX_HAPI,"Created EGRESS XLATE rule with tag_format_class_id=%u, edit_class_id=%u, action_id=%u", tagformat_double_class_id, vep_egress_double_class_id, action_egress_oswap_iswap_id);
  }

  /* Setting egress xlate class ids */
  if ( ptin_hapi_xlate_egress_portsGroup_init()!=L7_SUCCESS )
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Error initing class ids");
    rc = L7_FAILURE;
  }

  /* Init available resources */
  resources_xlate_ingress = FREE_RESOURCES_XLATE_INGRESS;
  resources_xlate_egress  = FREE_RESOURCES_XLATE_EGRESS;

  PT_LOG_TRACE(LOG_CTX_HAPI, "ptin_hapi_xlate_init finished: %d", rc);

  return rc;
}

/**
 * Get available resources for translation
 * 
 * @param ingress: pointer to variable where will contain the 
 *               number of free ingress translations.
 * @param egress: pointer to variable where will contain the 
 *               number of free egress translations.
 * @return L7_RC_t: always L7_SUCCESS
 */
L7_RC_t ptin_hapi_xlate_free_resources(L7_uint16 *ingress, L7_uint16 *egress)
{
  if (ingress != L7_NULLPTR)
  {
    *ingress = resources_xlate_ingress;
    PT_LOG_TRACE(LOG_CTX_HAPI, "Free ingress translation resources consulted: %u", *ingress);
  }

  if (egress != L7_NULLPTR)
  {
    *egress = resources_xlate_egress;
    PT_LOG_TRACE(LOG_CTX_HAPI, "Free egress translation resources consulted: %u", *egress);
  }

  return L7_SUCCESS;
}


/******************************** 
 * 4TH GENERATION FUNCTIONS
 ********************************/

/**
 * Get ingress translation configuration for the given port, 
 * outer vlan and inner vlan. 
 * 
 * @param dapiPort: port reference (physical or LAG)
 * @param xlate: Vlans and actions
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_hapi_xlate_ingress_get(ptin_dapi_port_t *dapiPort, ptin_hapi_xlate_t *xlate)
{
  /* Update resources availability */
  return L7_SUCCESS;
}

/**
 * Translate packets (outer+inner vlan) to another outer-vlan at
 * the ingress stage 
 * 
 * @param dapiPort: port reference (physical or LAG)
 * @param xlate : vlans and actions
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_hapi_xlate_ingress_add(ptin_dapi_port_t *dapiPort, ptin_hapi_xlate_t *xlate)
{
  bcm_vlan_port_translation_t port_xlate;
  bcm_error_t rv;

  PT_LOG_TRACE(LOG_CTX_HAPI, "lif=0x%x newOVlanId=%u(%u) newIVlanId=%u(%u)",
               xlate->lif_id,
               xlate->newOuterVlanId, xlate->outerVlanAction,
               xlate->newInnerVlanId, xlate->innerVlanAction);

  if (xlate->innerVlanAction == PTIN_XLATE_ACTION_NONE)
  {
    bcm_vlan_port_translation_t_init(&port_xlate);
    port_xlate.new_outer_vlan = xlate->newOuterVlanId;
    port_xlate.new_inner_vlan = 0;
    port_xlate.gport = xlate->lif_id;
    port_xlate.flags = BCM_VLAN_ACTION_SET_INGRESS;
    port_xlate.vlan_edit_class_id = vep_ingress_single_class_id;

    rv = bcm_vlan_port_translation_set(bcm_unit, &port_xlate);
    if (rv != BCM_E_NONE) {
      PT_LOG_ERR(LOG_CTX_HAPI,"Error, bcm_vlan_port_translate_set: rv=%d", rv);
      return rv;
    }

    PT_LOG_TRACE(LOG_CTX_HAPI,"Added translation to VLAN Edit Profile with vlan_edit_class_id=%u\n", vep_egress_single_class_id);
  }
  else
  {
    bcm_vlan_port_translation_t_init(&port_xlate);
    port_xlate.new_outer_vlan = xlate->newOuterVlanId;
    port_xlate.new_inner_vlan = xlate->newInnerVlanId;
    port_xlate.gport = xlate->lif_id;
    port_xlate.flags = BCM_VLAN_ACTION_SET_INGRESS;
    port_xlate.vlan_edit_class_id = vep_ingress_double_class_id;

    rv = bcm_vlan_port_translation_set(bcm_unit, &port_xlate);
    if (rv != BCM_E_NONE) {
      PT_LOG_ERR(LOG_CTX_HAPI,"Error, bcm_vlan_port_translate_set: rv=%d", rv);
      return rv;
    }

    PT_LOG_TRACE(LOG_CTX_HAPI,"Added translation to VLAN Edit Profile with vlan_edit_class_id=%u\n", vep_egress_double_class_id);
  }

  return L7_SUCCESS;
}

/**
 * Remove a translation (ingress stage)
 * 
 * @param dapiPort: port reference (physical or LAG)
 * @param xlate : vlans and actions
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_hapi_xlate_ingress_delete(ptin_dapi_port_t *dapiPort, ptin_hapi_xlate_t *xlate)
{
  bcm_vlan_port_translation_t port_xlate;
  bcm_error_t rv;

  PT_LOG_TRACE(LOG_CTX_HAPI, "lif=0x%x", xlate->lif_id);

  bcm_vlan_port_translation_t_init(&port_xlate);
  port_xlate.new_outer_vlan = 0;
  port_xlate.new_inner_vlan = 0;
  port_xlate.gport = xlate->lif_id;
  port_xlate.flags = BCM_VLAN_ACTION_SET_INGRESS;
  port_xlate.vlan_edit_class_id = 0;

  rv = bcm_vlan_port_translation_set(bcm_unit, &port_xlate);

  if (rv == BCM_E_NOT_FOUND)
  {
    PT_LOG_TRACE(LOG_CTX_HAPI,"No translation found: rv=%d", rv);
  }
  else if (rv != BCM_E_NONE)
  {
    PT_LOG_ERR(LOG_CTX_HAPI,"Error, bcm_vlan_port_translate_set: rv=%d", rv);
    return rv;
  }

  PT_LOG_TRACE(LOG_CTX_HAPI,"Removed ingress translation from LIF 0x%x", xlate->lif_id);

  return L7_SUCCESS;
}

/**
 * Remove all ingress translations
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_hapi_xlate_ingress_delete_all(void)
{
  int error;

  /* TODO */
  return L7_SUCCESS;

  error = bcmx_vlan_translate_delete_all();

  if (error != BCM_E_NONE && error != BCM_E_NOT_FOUND )
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Error calling bcm_vlan_translate_delete_all function: %d (\"%s\")", error, bcm_errmsg(error));
    return L7_FAILURE;
  }

  resources_xlate_ingress = FREE_RESOURCES_XLATE_INGRESS;

  PT_LOG_TRACE(LOG_CTX_HAPI, "All ingress translations were removed");

  /* Update resources availability */
  return L7_SUCCESS;
}

/**
 * Get egress translation configuration for a given class id, 
 * outer vlan and inner vlan 
 * 
 * @param portgroup: port class id
 * @param xlate : vlans and actions
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_hapi_xlate_egress_get(L7_uint32 portgroup, ptin_hapi_xlate_t *xlate)
{
  int error;
  bcm_vlan_action_set_t action;

  /* TODO */
  return L7_SUCCESS;

  PT_LOG_TRACE(LOG_CTX_HAPI, "portgroup=%u oVlanId=%u iVlanId=%u", portgroup, xlate->outerVlanId, xlate->innerVlanId);

  bcm_vlan_action_set_t_init(&action);

  PT_LOG_TRACE(LOG_CTX_HAPI, "bcm_vlan_translate_egress_action_add(0, %d, %u, %u, &action)", portgroup, xlate->outerVlanId, xlate->innerVlanId);

  error = bcmx_vlan_translate_egress_action_get(portgroup, xlate->outerVlanId, xlate->innerVlanId, &action);

  if (error != BCM_E_NONE)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Error calling bcm_vlan_translate_egress_action_get function: %d (\"%s\")", error, bcm_errmsg(error));
    return L7_FAILURE;
  }

  /* Extract data */
  xlate->newOuterVlanId = action.new_outer_vlan;
  xlate->newInnerVlanId = action.new_inner_vlan;
  xlate->outerVlanAction = (xlate->innerVlanId!=0) ? action.dt_outer : action.ot_outer;
  xlate->innerVlanAction = (xlate->innerVlanId!=0) ? action.dt_inner : action.ot_inner;

  xlate->newOuterPrio   = action.priority;
  xlate->newInnerPrio   = action.new_inner_pkt_prio;
  xlate->outerPrioAction = (xlate->innerVlanId!=0) ? action.dt_outer_prio : action.ot_outer_prio;
  xlate->innerPrioAction = (xlate->innerVlanId!=0) ? action.dt_inner_prio : action.ot_inner_pkt_prio;

  PT_LOG_TRACE(LOG_CTX_HAPI, "Translation entry read successfully: newOuterVlan=%u.%u (Oaction %u.%u) newInnerVlan=%u.%u (Iaction %u.%u)",
            xlate->newOuterVlanId , xlate->newOuterPrio,
            xlate->outerVlanAction, xlate->outerPrioAction,
            xlate->newInnerVlanId , xlate->newInnerPrio,
            xlate->innerVlanAction, xlate->innerPrioAction);

  return L7_SUCCESS;
}

/**
 * Translate single or double-tagged packets to another 
 * outer-vlan at the egress stage 
 * 
 * @param portgroup: port class id
 * @param xlate : vlans and actions
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_hapi_xlate_egress_add(ptin_dapi_port_t *dapiPort, ptin_hapi_xlate_t *xlate)
{
  bcm_vlan_port_translation_t port_xlate;
  bcm_error_t rv;

  PT_LOG_TRACE(LOG_CTX_HAPI, "lif=0x%x newOVlanId=%u(%u) newIVlanId=%u(%u)",
               xlate->lif_id,
               xlate->newOuterVlanId, xlate->outerVlanAction,
               xlate->newInnerVlanId, xlate->innerVlanAction);

  if (xlate->innerVlanAction == PTIN_XLATE_ACTION_NONE)
  {
    bcm_vlan_port_translation_t_init(&port_xlate);
    port_xlate.new_outer_vlan = xlate->newOuterVlanId;
    port_xlate.new_inner_vlan = 0;
    port_xlate.gport = xlate->lif_id;
    port_xlate.flags = BCM_VLAN_ACTION_SET_EGRESS;
    port_xlate.vlan_edit_class_id = vep_egress_single_class_id;

    rv = bcm_vlan_port_translation_set(bcm_unit, &port_xlate);
    if (rv != BCM_E_NONE) {
      PT_LOG_ERR(LOG_CTX_HAPI,"Error, bcm_vlan_port_translate_set: rv=%d", rv);
      return rv;
    }

    PT_LOG_TRACE(LOG_CTX_HAPI,"Added translation to VLAN Edit Profile with vlan_edit_class_id=%u\n", vep_egress_single_class_id);
  }
  else
  {
    bcm_vlan_port_translation_t_init(&port_xlate);
    port_xlate.new_outer_vlan = xlate->newOuterVlanId;
    port_xlate.new_inner_vlan = xlate->newInnerVlanId;
    port_xlate.gport = xlate->lif_id;
    port_xlate.flags = BCM_VLAN_ACTION_SET_EGRESS;
    port_xlate.vlan_edit_class_id = vep_egress_double_class_id;

    rv = bcm_vlan_port_translation_set(bcm_unit, &port_xlate);
    if (rv != BCM_E_NONE) {
      PT_LOG_ERR(LOG_CTX_HAPI,"Error, bcm_vlan_port_translate_set: rv=%d", rv);
      return rv;
    }

    PT_LOG_TRACE(LOG_CTX_HAPI,"Added translation to VLAN Edit Profile with vlan_edit_class_id=%u\n", vep_egress_double_class_id);
  }

  return L7_SUCCESS;
}

/**
 * Remove a single/double-vlan translation at the egress stage
 * 
 * @param portgroup: port class id
 * @param xlate : vlans and actions
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_hapi_xlate_egress_delete(ptin_dapi_port_t *dapiPort, ptin_hapi_xlate_t *xlate)
{
  bcm_vlan_port_translation_t port_xlate;
  bcm_error_t rv;

  PT_LOG_TRACE(LOG_CTX_HAPI, "lif=0x%x", xlate->lif_id);

  bcm_vlan_port_translation_t_init(&port_xlate);
  port_xlate.new_outer_vlan = 0;
  port_xlate.new_inner_vlan = 0;
  port_xlate.gport = xlate->lif_id;
  port_xlate.flags = BCM_VLAN_ACTION_SET_EGRESS;
  port_xlate.vlan_edit_class_id = 0;

  rv = bcm_vlan_port_translation_set(bcm_unit, &port_xlate);

  if (rv == BCM_E_NOT_FOUND)
  {
    PT_LOG_TRACE(LOG_CTX_HAPI,"No translation found: rv=%d", rv);
  }
  else if (rv != BCM_E_NONE)
  {
    PT_LOG_ERR(LOG_CTX_HAPI,"Error, bcm_vlan_port_translate_set: rv=%d", rv);
    return rv;
  }

  PT_LOG_TRACE(LOG_CTX_HAPI,"Removed egress translation from LIF 0x%x", xlate->lif_id);

  return L7_SUCCESS;
}

/**
 * Remove all egress translations
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_hapi_xlate_egress_delete_all(void)
{
  int error;

  error = bcmx_vlan_translate_egress_delete_all();

  if (error != BCM_E_NONE && error != BCM_E_NOT_FOUND )
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Error calling bcm_vlan_translate_egress_delete_all function: %d (\"%s\")", error, bcm_errmsg(error));
    return L7_FAILURE;
  }

  /* Setting egress xlate class ids */
  if ( ptin_hapi_xlate_egress_portsGroup_init()!=L7_SUCCESS )
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Error setting class ids");
    return L7_FAILURE;
  }

  resources_xlate_egress = FREE_RESOURCES_XLATE_EGRESS;

  PT_LOG_TRACE(LOG_CTX_HAPI, "All egress translations were removed");

  /* Update resources availability */
  return L7_SUCCESS;
}



/******************************** 
 * CLASSPORT MANAGEMENT
 ********************************/

/**
 * Define a group of ports identified by a class id
 * 
 * @param portgroup: class id (positive value)
 * @param usp_list: List of ports that make part of the group 
 * @param usp_list_size: Number of ports
 * @param dapi_g: System descriptor
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_hapi_xlate_egress_portsGroup_set(L7_uint32 portgroup, DAPI_USP_t *usp_list[], L7_uint8 usp_list_size, DAPI_t *dapi_g)
{
  L7_uint8 index;
  L7_RC_t rc = L7_SUCCESS;

  PT_LOG_TRACE(LOG_CTX_HAPI,"portgroup=%u usp_list[0]={%d,%d,%d} usp_list_size=%u",
            portgroup, usp_list[0]->unit, usp_list[0]->slot, usp_list[0]->port, usp_list_size);

  /* If group is empty, there is nothing to be done */
  if (usp_list_size==0)
  {
    return L7_SUCCESS;
  }

  /* Change Ingress Vlan Translate Keys for all physical ports (LAGs are included through their physical ports) */
  for (index=0; index<usp_list_size; index++)
  {
    /* Validate usp */
    if (usp_list[index]->unit<0 || usp_list[index]->slot<0 || usp_list[index]->port<0)
    {
      PT_LOG_ERR(LOG_CTX_HAPI,"Invalid interface: {%d,%d,%d}", usp_list[index]->unit, usp_list[index]->slot, usp_list[index]->port);
      rc = L7_FAILURE;
      continue;
    }

    /* Get port pointers *of the group port */
    dapiPortPtr = DAPI_PORT_GET( usp_list[index], dapi_g );
    hapiPortPtr = HAPI_PORT_GET( usp_list[index], dapi_g );

    /* Accept only physical interfaces */
    if ( !IS_PORT_TYPE_PHYSICAL(dapiPortPtr) /*&& !IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr)*/ )
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "Port {%d,%d,%d} is not physical", usp_list[index]->unit, usp_list[index]->slot, usp_list[index]->port);
      rc = L7_FAILURE;
      continue;
    }

    PT_LOG_TRACE(LOG_CTX_HAPI, "Setting class id %d to single port {%d,%d,%d}",
              portgroup, usp_list[index]->unit, usp_list[index]->slot, usp_list[index]->port);

    #if 0
    if (bcmx_port_class_set(hapiPortPtr->bcmx_lport, bcmPortClassVlanTranslateEgress, (L7_uint32) portgroup ) != BCM_E_NONE)
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "Error setting class id %d to single port {%d,%d,%d} [VLAN XLATE]",
              portgroup, usp_list[index]->unit, usp_list[index]->slot, usp_list[index]->port);
      rc = L7_FAILURE;
      continue;
    }
    /* PTin removed: Let Fastpath to manage EFP Class ids */
    #if 1
    if (bcmx_port_class_set(hapiPortPtr->bcmx_lport, bcmPortClassFieldEgress, (L7_uint32) portgroup + EFP_STD_CLASS_ID_MAX) != BCM_E_NONE)
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "Error setting class id %d to single port {%d,%d,%d} [ECAP]",
              portgroup, usp_list[index]->unit, usp_list[index]->slot, usp_list[index]->port);
      rc = L7_FAILURE;
      continue;
    }
    #endif
    #endif
  }

  PT_LOG_TRACE(LOG_CTX_HAPI, "Class id %d successfully assigned to the given ports", portgroup);

  return rc;
}

/**
 * Get the class id assigned to a specific port
 * 
 * @param portgroup: Class id returned
 * @param usp: Port reference
 * @param dapi_g: System descriptor
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_hapi_xlate_egress_portsGroup_get(L7_uint32 *portgroup, DAPI_USP_t *usp, DAPI_t *dapi_g)
{
  L7_uint32 classId=0;

  PT_LOG_TRACE(LOG_CTX_HAPI,"usp={%d,%d,%d}",usp->unit,usp->slot,usp->port);

  /* Validate port group */
  if (usp->unit<0 || usp->slot<0 || usp->port<0)
  {
    PT_LOG_ERR(LOG_CTX_HAPI,"Invalid portgroup");
    return L7_FAILURE;
  }

  /* Get port pointers *of the group port */
  dapiPortPtr = DAPI_PORT_GET( usp, dapi_g );
  hapiPortPtr = HAPI_PORT_GET( usp, dapi_g );

  /* Accept only physical and lag interfaces */
  if ( !IS_PORT_TYPE_PHYSICAL(dapiPortPtr) /*&& !IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr)*/ )
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Port {%d,%d,%d} is not physical",usp->unit,usp->slot,usp->port);
    return L7_FAILURE;
  }

  #if 0
  /* Extract class id */
  if (bcmx_port_class_get(hapiPortPtr->bcmx_lport, bcmPortClassVlanTranslateEgress, &classId ) != BCM_E_NONE)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Error getting class id from port {%d,%d,%d}", usp->unit, usp->slot, usp->port);
    return L7_FAILURE;
  }
  #endif

  /* Output value */
  if (portgroup!=L7_NULLPTR)
  {
    *portgroup = (L7_int) classId;
  }

  PT_LOG_TRACE(LOG_CTX_HAPI, "Class id %u successfully extracted from port {%d,%d,%d}", classId, usp->unit, usp->slot, usp->port);

  return L7_SUCCESS;
}

/********************************************************************
 * INTERNAL FUNCTIONS IMPLEMENTATION
 ********************************************************************/

/**
 * Define class ports for egress translations. 
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
static L7_RC_t ptin_hapi_xlate_egress_portsGroup_init(void)
{
  L7_int port;
  bcm_port_t bcm_port;
  L7_RC_t rc = L7_SUCCESS;
  
  /* Change Ingress Vlan Translate Keys for all physical ports (LAGs are included through their physical ports) */
  for (port=0; port<ptin_sys_number_of_ports; port++)
  {
    if (hapi_ptin_bcmPort_get(port, &bcm_port) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "Error getting bcm unit id");
      rc = L7_FAILURE;
      continue;
    }

    /* Insert init code here! */
    #if 0
    /* Default class ids is port+1 */
    if (bcm_port_class_set(bcm_unit, bcm_port, bcmPortClassVlanTranslateEgress, port+1 ) != BCM_E_NONE)
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "Error setting class id %u to port %d [VLAN XLATE]", port+1, port);
      rc = L7_FAILURE;
      continue;
    }
    /* PTin removed: Let Fastpath to manage EFP Class ids */
    #if 0
    if (bcm_port_class_set(bcm_unit, bcm_port, bcmPortClassFieldEgress, port+1 ) != BCM_E_NONE)
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "Error setting class id %u to port %d [ECAP]", port+1, port);
      rc = L7_FAILURE;
      continue;
    }
    #endif
    #endif
  }

  PT_LOG_TRACE(LOG_CTX_HAPI, "Class ids assigned: rc=%d", rc);

  return rc;
}

