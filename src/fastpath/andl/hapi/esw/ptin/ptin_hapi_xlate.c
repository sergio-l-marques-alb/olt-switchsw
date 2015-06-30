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
  L7_RC_t rc = L7_SUCCESS;

  /* Change Ingress Vlan Translate Keys for all physical ports (LAGs are included through their physical ports) */
  for (port=0; port<ptin_sys_number_of_ports; port++)
  {
    if (hapi_ptin_bcmPort_get(port, &bcm_port) != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_HAPI, "Error getting bcm unit id");
      rc = L7_FAILURE;
      continue;
    }

    /* First key: First do a lookup for port + outerVlan + innerVlan.
       Second key: If failed do a second lookup for port + outerVlan */
    if ( (bcm_vlan_control_port_set( bcm_unit, bcm_port,bcmVlanPortTranslateKeyFirst, bcmVlanTranslateKeyPortDouble) != L7_SUCCESS) ||
         (bcm_vlan_control_port_set( bcm_unit, bcm_port,bcmVlanPortTranslateKeySecond, bcmVlanTranslateKeyPortOuter) != L7_SUCCESS) )
    {
      LOG_ERR(LOG_CTX_PTIN_HAPI, "Error setting translation keys");
      rc = L7_FAILURE;
      continue;
    }

    /* Enable ingress and egress translations.
       Also, drop packets that do not fullfil any translation entry. */
    if ((bcm_vlan_control_port_set( bcm_unit, bcm_port, bcmVlanTranslateIngressEnable,   L7_TRUE) != L7_SUCCESS) ||
        (bcm_vlan_control_port_set( bcm_unit, bcm_port, bcmVlanTranslateIngressMissDrop, L7_TRUE) != L7_SUCCESS) ||
        (bcm_vlan_control_port_set( bcm_unit, bcm_port, bcmVlanTranslateEgressEnable,    L7_TRUE) != L7_SUCCESS) ||
        (bcm_vlan_control_port_set( bcm_unit, bcm_port, bcmVlanTranslateEgressMissDrop,  L7_TRUE) != L7_SUCCESS) )
    {
      LOG_ERR(LOG_CTX_PTIN_HAPI, "Error setting translation enables");
      rc = L7_FAILURE;
      continue;
    }
  }

  /* Setting egress xlate class ids */
  if ( ptin_hapi_xlate_egress_portsGroup_init()!=L7_SUCCESS )
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Error initing class ids");
    rc = L7_FAILURE;
  }

  /* Init available resources */
  resources_xlate_ingress = FREE_RESOURCES_XLATE_INGRESS;
  resources_xlate_egress  = FREE_RESOURCES_XLATE_EGRESS;

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "ptin_hapi_xlate_init finished: %d", rc);

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
    LOG_TRACE(LOG_CTX_PTIN_HAPI, "Free ingress translation resources consulted: %u", *ingress);
  }

  if (egress != L7_NULLPTR)
  {
    *egress = resources_xlate_egress;
    LOG_TRACE(LOG_CTX_PTIN_HAPI, "Free egress translation resources consulted: %u", *egress);
  }

  return L7_SUCCESS;
}



/******************************** 
 * 1ST GENERATION FUNCTIONS
 ********************************/

/**
 * Translate only the outer-vlan (to another outer-vlan) at the 
 * ingress stage 
 * 
 * @param dapiPort: port reference (physical or LAG)
 * @param oVlanId: outer vlan id
 * @param newOVlanId: new outer vlan id
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_hapi_xlate_singletag_action_add(ptin_dapi_port_t *dapiPort, L7_uint16 oVlanId, L7_uint16 newOVlanId)
{
  int error;
  bcm_vlan_translate_key_t keyType;
  bcm_vlan_action_set_t action;

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "dapiPort={%d,%d,%d} oVlanId=%u newOVlanId=%u",
            dapiPort->usp->unit, dapiPort->usp->slot, dapiPort->usp->port, oVlanId, newOVlanId);

  /* Validate dapiPort */
  if (dapiPort->usp->unit<0 || dapiPort->usp->slot<0 || dapiPort->usp->port<0)
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Invalid interface");
    return L7_FAILURE;
  }

  /* Get port pointers */
  DAPIPORT_GET_PTR(dapiPort, dapiPortPtr, hapiPortPtr);
  /* Accept only physical and lag interfaces */
  if ( !IS_PORT_TYPE_PHYSICAL(dapiPortPtr) && !IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr) )
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Port {%d,%d,%d} is not physical neither logical lag",
            dapiPort->usp->unit, dapiPort->usp->slot, dapiPort->usp->port);
    return L7_FAILURE;
  }

  keyType = bcmVlanTranslateKeyPortOuter;

  bcm_vlan_action_set_t_init(&action);
  action.dt_outer      = bcmVlanActionReplace;
  action.dt_inner      = bcmVlanActionNone;
  action.dt_outer_prio = bcmVlanActionNone;
  action.dt_inner_prio = bcmVlanActionNone;

  action.ot_outer      = bcmVlanActionReplace;
  action.ot_inner      = bcmVlanActionNone;
  action.ot_outer_prio = bcmVlanActionNone;

  action.new_outer_vlan = newOVlanId;

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "bcm_vlan_translate_action_add(0, 0x%08X[%d], %u, %u, %u, &action)",
            hapiPortPtr->bcmx_lport, hapiPortPtr->bcm_port, keyType, oVlanId, 0);

  error = bcmx_vlan_translate_action_add(hapiPortPtr->bcmx_lport, keyType, oVlanId, 0, &action);

  if (error != BCM_E_NONE && error != BCM_E_EXISTS)
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Error calling bcm_vlan_translate_action_add function: %d (\"%s\")", error, bcm_errmsg(error));
    return L7_FAILURE;
  }

  /* Update resources availability */
  if (error == BCM_E_NONE)
  {
    if (resources_xlate_ingress < FREE_RESOURCES_XLATE_INGRESS)
      resources_xlate_ingress++;
  }

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "Translation entry added successfully: newOuterVlan=%u", newOVlanId);

  return L7_SUCCESS;
}

/**
 * Remove the outer-vlan translation (ingress stage)
 * 
 * @param dapiPort: port reference (physical or LAG)
 * @param oVlanId: outer vlan id
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_hapi_xlate_singletag_action_delete(ptin_dapi_port_t *dapiPort, L7_uint16 oVlanId)
{
  int error;
  bcm_vlan_translate_key_t keyType;

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "dapiPort={%d,%d,%d} oVlanId=%u",
            dapiPort->usp->unit, dapiPort->usp->slot, dapiPort->usp->port, oVlanId);

  /* Validate dapiPort */
  if (dapiPort->usp->unit<0 || dapiPort->usp->slot<0 || dapiPort->usp->port<0)
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Invalid interface");
    return L7_FAILURE;
  }

  /* Get port pointers */
  DAPIPORT_GET_PTR(dapiPort, dapiPortPtr, hapiPortPtr);
  /* Accept only physical and lag interfaces */
  if ( !IS_PORT_TYPE_PHYSICAL(dapiPortPtr) && !IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr) )
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Port {%d,%d,%d} is not physical neither logical lag",
            dapiPort->usp->unit, dapiPort->usp->slot, dapiPort->usp->port);
    return L7_FAILURE;
  }

  keyType = bcmVlanTranslateKeyPortOuter;

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "bcm_vlan_translate_action_delete(0, 0x%08X[%d], %u, %u, %u)",
            hapiPortPtr->bcmx_lport, hapiPortPtr->bcm_port, keyType, oVlanId, 0);

  error = bcmx_vlan_translate_action_delete(hapiPortPtr->bcmx_lport, keyType, oVlanId, 0);

  if (error != BCM_E_NONE && error != BCM_E_NOT_FOUND )
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Error calling bcm_vlan_translate_action_delete function: %d (\"%s\")",error,bcm_errmsg(error));
    return L7_FAILURE;
  }

  /* Update resources availability */
  if ( error == BCM_E_NONE )
  {
    if (resources_xlate_ingress > 0 )
      resources_xlate_ingress--;
  }

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "Translation entry removed successfully");

  return L7_SUCCESS;
}

/**
 * Translate double-tagged packets (outer+inner vlan) to another
 * outer-vlan at the ingress stage
 * 
 * @param dapiPort: port reference (physical or LAG)
 * @param oVlanId: outer vlan id 
 * @param iVlanId: inner vlan id 
 * @param newOVlanId: new outer vlan id
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_hapi_xlate_doubletag_action_add(ptin_dapi_port_t *dapiPort, L7_uint16 oVlanId, L7_uint16 iVlanId, L7_uint16 newOVlanId)
{
  int error;
  bcm_vlan_translate_key_t keyType;
  bcm_vlan_action_set_t action;

  LOG_TRACE(LOG_CTX_PTIN_HAPI,"dapiPort={%d,%d,%d} oVlanId=%u iVlanId=%u newOVlanId=%u",
            dapiPort->usp->unit, dapiPort->usp->slot, dapiPort->usp->port, oVlanId, iVlanId, newOVlanId);

  /* Validate dapiPort */
  if (dapiPort->usp->unit<0 || dapiPort->usp->slot<0 || dapiPort->usp->port<0)
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI,"Invalid interface");
    return L7_FAILURE;
  }

  /* Get port pointers */
  DAPIPORT_GET_PTR(dapiPort, dapiPortPtr, hapiPortPtr);
  /* Accept only physical and lag interfaces */
  if ( !IS_PORT_TYPE_PHYSICAL(dapiPortPtr) && !IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr) )
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, " ERROR: Port {%d,%d,%d} is not physical neither logical lag",
            dapiPort->usp->unit, dapiPort->usp->slot, dapiPort->usp->port);
    return L7_FAILURE;
  }

  keyType = bcmVlanTranslateKeyPortDouble;

  bcm_vlan_action_set_t_init(&action);
  action.dt_outer      = bcmVlanActionReplace;
  action.dt_inner      = bcmVlanActionNone;
  action.dt_outer_prio = bcmVlanActionNone;
  action.dt_inner_prio = bcmVlanActionNone;

  action.ot_outer      = bcmVlanActionReplace;
  action.ot_inner      = bcmVlanActionNone;
  action.ot_outer_prio = bcmVlanActionNone;

  action.new_outer_vlan = newOVlanId;

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "bcm_vlan_translate_action_add(0, 0x%08X[%d], %u, %u, %u, &action)",
            hapiPortPtr->bcmx_lport, hapiPortPtr->bcm_port, keyType, oVlanId, iVlanId);

  error = bcmx_vlan_translate_action_add(hapiPortPtr->bcmx_lport, keyType, oVlanId, iVlanId, &action);

  if (error != BCM_E_NONE && error != BCM_E_EXISTS)
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Error calling bcm_vlan_translate_action_add function: %d (\"%s\")", error, bcm_errmsg(error));
    return L7_FAILURE;
  }

  if (error == BCM_E_NONE)
  {
    if (resources_xlate_ingress < FREE_RESOURCES_XLATE_INGRESS)
      resources_xlate_ingress++;
  }

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "Translation entry added successfully: newOuterVlan=%u", newOVlanId);

  /* Update resources availability */
  return L7_SUCCESS;
}

/**
 * Remove a double-vlan translation (ingress stage)
 * 
 * @param dapiPort: port reference (physical or LAG)
 * @param oVlanId: outer vlan id 
 * @param iVlanId: inner vlan id 
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_hapi_xlate_doubletag_action_delete(ptin_dapi_port_t *dapiPort, L7_uint16 oVlanId, L7_uint16 iVlanId)
{
  int error;
  bcm_vlan_translate_key_t keyType;

  LOG_TRACE(LOG_CTX_PTIN_HAPI,"dapiPort={%d,%d,%d} oVlanId=%u iVlanId=%u",
            dapiPort->usp->unit, dapiPort->usp->slot, dapiPort->usp->port, oVlanId,iVlanId);

  /* Validate dapiPort */
  if (dapiPort->usp->unit<0 || dapiPort->usp->slot<0 || dapiPort->usp->port<0)
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Invalid interface");
    return L7_FAILURE;
  }

  /* Get port pointers */
  DAPIPORT_GET_PTR(dapiPort, dapiPortPtr, hapiPortPtr);
  /* Accept only physical and lag interfaces */
  if ( !IS_PORT_TYPE_PHYSICAL(dapiPortPtr) && !IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr) )
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Port {%d,%d,%d} is not physical neither logical lag",
            dapiPort->usp->unit, dapiPort->usp->slot, dapiPort->usp->port);
    return L7_FAILURE;
  }

  keyType = bcmVlanTranslateKeyPortDouble;

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "bcm_vlan_translate_action_delete(0, 0x%08X[%d], %u, %u, %u)",
            hapiPortPtr->bcmx_lport, hapiPortPtr->bcm_port, keyType, oVlanId, iVlanId);

  error = bcmx_vlan_translate_action_delete(hapiPortPtr->bcmx_lport, keyType, oVlanId, iVlanId);

  if (error != BCM_E_NONE && error != BCM_E_NOT_FOUND )
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Error calling bcm_vlan_translate_action_delete function: %d (\"%s\")", error, bcm_errmsg(error));
    return L7_FAILURE;
  }

  if ( error == BCM_E_NONE )
  {
    if (resources_xlate_ingress > 0 )
      resources_xlate_ingress--;
  }

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "Translation entry removed successfully");

  /* Update resources availability */
  return L7_SUCCESS;
}



/******************************** 
 * 2ND GENERATION FUNCTIONS
 ********************************/

/**
 * Get ingress translation configuration for the given port, 
 * outer vlan and inner vlan. 
 * 
 * @param dapiPort: port reference (physical or LAG)
 * @param oVlanId: outer vlanId 
 * @param iVlanId: inner vlanId (0 for single-vlan translations)
 * @param *newOVlanId: new outer vlan id
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_hapi_xlate_ingress_action_get(ptin_dapi_port_t *dapiPort, L7_uint16 oVlanId, L7_uint16 iVlanId, L7_uint16 *newOVlanId)
{
  int error;
  bcm_vlan_translate_key_t keyType;
  bcm_vlan_action_set_t action;

  LOG_TRACE(LOG_CTX_PTIN_HAPI,"dapiPort={%d,%d,%d} oVlanId=%u iVlanId=%u",
            dapiPort->usp->unit, dapiPort->usp->slot, dapiPort->usp->port, oVlanId, iVlanId);

  /* Validate dapiPort */
  if (dapiPort->usp->unit<0 || dapiPort->usp->slot<0 || dapiPort->usp->port<0)
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI,"Invalid interface");
    return L7_FAILURE;
  }

  /* Get port pointers */
  DAPIPORT_GET_PTR(dapiPort, dapiPortPtr, hapiPortPtr);
  /* Accept only physical and lag interfaces */
  if ( !IS_PORT_TYPE_PHYSICAL(dapiPortPtr) && !IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr) )
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Port {%d,%d,%d} is not physical neither logical lag",
            dapiPort->usp->unit, dapiPort->usp->slot, dapiPort->usp->port);
    return L7_FAILURE;
  }

  /* For double tagged packets */
  if ( iVlanId != 0 )
  {
    keyType = bcmVlanTranslateKeyPortDouble;
  }
  /* For single tagged packets */
  else
  {
    keyType = bcmVlanTranslateKeyPortOuter;
  }

  bcm_vlan_action_set_t_init(&action);

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "bcm_vlan_translate_action_add(0, 0x%08X[%d], %u, %u, %u, &action)",
            hapiPortPtr->bcmx_lport, hapiPortPtr->bcm_port, keyType, oVlanId, iVlanId);

  error = bcmx_vlan_translate_action_get(hapiPortPtr->bcmx_lport, keyType, oVlanId, iVlanId, &action);

  if (error != BCM_E_NONE)
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Error calling bcm_vlan_translate_action_get function: %d (\"%s\")", error, bcm_errmsg(error));
    return L7_FAILURE;
  }

  /* Extract new outer vlan */
  if ( newOVlanId != L7_NULLPTR )
  {
    *newOVlanId = action.new_outer_vlan;
    LOG_TRACE(LOG_CTX_PTIN_HAPI, "Translation entry read successfully: newOuterVlan=%u", *newOVlanId);
  }

  /* Update resources availability */
  return L7_SUCCESS;
}

/**
 * Translate packets (outer+inner vlan) to another outer-vlan at
 * the ingress stage 
 * 
 * @param dapiPort: port reference (physical or LAG)
 * @param oVlanId: outer vlanId 
 * @param iVlanId: inner vlanId (0 for single-vlan translations)
 * @param newOVlanId: new outer vlan id
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_hapi_xlate_ingress_action_add(ptin_dapi_port_t *dapiPort, L7_uint16 oVlanId, L7_uint16 iVlanId, L7_uint16 newOVlanId)
{
  int error;
  bcm_vlan_translate_key_t keyType;
  bcm_vlan_action_set_t action;

  LOG_TRACE(LOG_CTX_PTIN_HAPI,"dapiPort={%d,%d,%d} oVlanId=%u iVlanId=%u newOVlanId=%u",
            dapiPort->usp->unit, dapiPort->usp->slot, dapiPort->usp->port, oVlanId, iVlanId, newOVlanId);

  /* Validate dapiPort */
  if (dapiPort->usp->unit<0 || dapiPort->usp->slot<0 || dapiPort->usp->port<0)
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Invalid interface");
    return L7_FAILURE;
  }

  /* Get port pointers */
  DAPIPORT_GET_PTR(dapiPort, dapiPortPtr, hapiPortPtr);
  /* Accept only physical and lag interfaces */
  if ( !IS_PORT_TYPE_PHYSICAL(dapiPortPtr) && !IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr) )
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Port {%d,%d,%d} is not physical neither logical lag (usp=)",
            dapiPort->usp->unit, dapiPort->usp->slot, dapiPort->usp->port);
    return L7_FAILURE;
  }

  /* For double tagged packets */
  if ( iVlanId != 0 )
  {
    keyType = bcmVlanTranslateKeyPortDouble;
  }
  /* For single tagged packets */
  else
  {
    keyType = bcmVlanTranslateKeyPortOuter;
  }

  bcm_vlan_action_set_t_init(&action);
  action.dt_outer      = bcmVlanActionReplace;
  action.dt_inner      = bcmVlanActionNone;
  action.dt_outer_prio = bcmVlanActionNone;
  action.dt_inner_prio = bcmVlanActionNone;

  action.ot_outer      = bcmVlanActionReplace;
  action.ot_inner      = bcmVlanActionNone;
  action.ot_outer_prio = bcmVlanActionNone;

  action.new_outer_vlan = newOVlanId;

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "bcm_vlan_translate_action_add(0, 0x%08X[%d], %u, %u, %u, &action)",
            hapiPortPtr->bcmx_lport, hapiPortPtr->bcm_port, keyType, oVlanId, iVlanId);

  error = bcmx_vlan_translate_action_add(hapiPortPtr->bcmx_lport, keyType, oVlanId, iVlanId, &action);

  if (error != BCM_E_NONE && error != BCM_E_EXISTS)
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Error calling bcm_vlan_translate_action_add function: %d (\"%s\")", error, bcm_errmsg(error));
    return L7_FAILURE;
  }

  /* Update resources availability */
  if (error == BCM_E_NONE)
  {
    if (resources_xlate_ingress < FREE_RESOURCES_XLATE_INGRESS)
      resources_xlate_ingress++;
  }

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "Translation entry added successfully: newOuterVlan=%u", newOVlanId);

  return L7_SUCCESS;
}

/**
 * Remove a translation (ingress stage)
 * 
 * @param dapiPort: port reference (physical or LAG)
 * @param oVlanId: outer vlanId 
 * @param iVlanId: inner vlanId (0 for single-vlan translations)
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_hapi_xlate_ingress_action_delete(ptin_dapi_port_t *dapiPort, L7_uint16 oVlanId, L7_uint16 iVlanId)
{
  int error;
  bcm_vlan_translate_key_t keyType;

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "dapiPort={%d,%d,%d} oVlanId=%u iVlanId=%u",
            dapiPort->usp->unit, dapiPort->usp->slot, dapiPort->usp->port, oVlanId, iVlanId);

  /* Validate dapiPort */
  if (dapiPort->usp->unit<0 || dapiPort->usp->slot<0 || dapiPort->usp->port<0)
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Invalid interface");
    return L7_FAILURE;
  }

  /* Get port pointers */
  DAPIPORT_GET_PTR(dapiPort, dapiPortPtr, hapiPortPtr);
  /* Accept only physical and lag interfaces */
  if ( !IS_PORT_TYPE_PHYSICAL(dapiPortPtr) && !IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr) )
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Port {%d,%d,%d} is not physical neither logical lag",
            dapiPort->usp->unit, dapiPort->usp->slot, dapiPort->usp->port);
    return L7_FAILURE;
  }

  /* For double tagged packets */
  if ( iVlanId != 0 )
  {
    keyType = bcmVlanTranslateKeyPortDouble;
  }
  /* For single tagged packets */
  else
  {
    keyType = bcmVlanTranslateKeyPortOuter;
  }

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "bcm_vlan_translate_action_delete(0, 0x%08X[%d], %u, %u, %u)",
            hapiPortPtr->bcmx_lport, hapiPortPtr->bcm_port, keyType, oVlanId, iVlanId);

  error = bcmx_vlan_translate_action_delete(hapiPortPtr->bcmx_lport, keyType, oVlanId, iVlanId);

  if (error != BCM_E_NONE && error != BCM_E_NOT_FOUND )
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Error calling bcm_vlan_translate_action_delete function: %d (\"%s\")", error, bcm_errmsg(error));
    return L7_FAILURE;
  }

  /* Update resources availability */
  if ( error == BCM_E_NONE )
  {
    if (resources_xlate_ingress > 0 )
      resources_xlate_ingress--;
  }

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "Translation entry removed successfully");

  return L7_SUCCESS;
}

/**
 * Get egress translation configuration for a given class id, 
 * outer vlan and inner vlan 
 * 
 * @param portgroup: port class id
 * @param oVlanId: outer vlan id 
 * @param iVlanId: inner vlan id (0 to not use inner vlan)
 * @param *newOVlanId: new outer vlan id
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_hapi_xlate_egress_action_get(L7_uint32 portgroup, L7_uint16 oVlanId, L7_uint16 iVlanId, L7_uint16 *newOVlanId)
{
  int error;
  bcm_vlan_action_set_t action;

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "portgroup=%u oVlanId=%u iVlanId=%u", portgroup, oVlanId, iVlanId);

  bcm_vlan_action_set_t_init(&action);

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "bcm_vlan_translate_egress_action_add(0, %d, %u, %u, &action)", portgroup, oVlanId, iVlanId);

  error = bcmx_vlan_translate_egress_action_get(portgroup, oVlanId, iVlanId, &action);

  if (error != BCM_E_NONE)
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Error calling bcm_vlan_translate_egress_action_get function: %d (\"%s\")", error, bcm_errmsg(error));
    return L7_FAILURE;
  }

  /* Extract new outer vlan */
  if ( newOVlanId != L7_NULLPTR )
  {
    *newOVlanId = action.new_outer_vlan;
    LOG_TRACE(LOG_CTX_PTIN_HAPI, "Translation entry read successfully: newOuterVlan=%u", *newOVlanId);
  }

  return L7_SUCCESS;
}

/**
 * Translate single or double-tagged packets to another 
 * outer-vlan at the egress stage 
 * 
 * @param portgroup: port class id
 * @param oVlanId: outer vlan id 
 * @param iVlanId: inner vlan id (0 to not use inner vlan)
 * @param newOVlanId: new outer vlan id
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_hapi_xlate_egress_action_add(L7_uint32 portgroup, L7_uint16 oVlanId, L7_uint16 iVlanId, L7_uint16 newOVlanId)
{
  int error;
  bcm_vlan_action_set_t action;

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "portgroup=%u oVlanId=%u iVlanId=%u newOVlanId=%u", portgroup, oVlanId, iVlanId, newOVlanId);

  /* Add translation entry */
  bcm_vlan_action_set_t_init(&action);
  action.dt_outer      = bcmVlanActionReplace;
  action.dt_inner      = bcmVlanActionNone;
  action.dt_outer_prio = bcmVlanActionNone;
  action.dt_inner_prio = bcmVlanActionNone;

  action.ot_outer      = bcmVlanActionReplace;
  action.ot_inner      = bcmVlanActionNone;
  action.ot_outer_prio = bcmVlanActionNone;

  action.new_outer_vlan = newOVlanId;

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "bcm_vlan_translate_egress_action_add(0, %d, %u, %u, &action)", portgroup, oVlanId, iVlanId);

  error = bcmx_vlan_translate_egress_action_add(portgroup, oVlanId, iVlanId, &action);

  if (error != BCM_E_NONE && error != BCM_E_EXISTS)
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Error calling bcm_vlan_translate_egress_action_add function: %d (\"%s\")", error, bcm_errmsg(error));
    return L7_FAILURE;
  }

  /* Update resources availability */
  if (error == BCM_E_NONE)
  {
    if (resources_xlate_egress < FREE_RESOURCES_XLATE_EGRESS)
      resources_xlate_egress++;
  }

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "Translation entry added successfully: newOuterVlan=%u", newOVlanId);

  return L7_SUCCESS;
}

/**
 * Remove a single/double-vlan translation at the egress stage
 * 
 * @param portgroup: port class id
 * @param oVlanId: outer vlan id 
 * @param iVlanId: inner vlan id (use 0 for single vlan) 
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_hapi_xlate_egress_action_delete(L7_uint32 portgroup, L7_uint16 oVlanId, L7_uint16 iVlanId)
{
  int error;

  LOG_TRACE(LOG_CTX_PTIN_HAPI,"portgroup=%u oVlanId=%u iVlanId=%u", portgroup, oVlanId, iVlanId);

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "bcm_vlan_translate_egress_action_delete(0,%d,%u,%u)", portgroup, oVlanId, iVlanId);

  error = bcmx_vlan_translate_egress_action_delete(portgroup, oVlanId, iVlanId);

  if (error != BCM_E_NONE && error != BCM_E_NOT_FOUND )
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Error calling bcm_vlan_translate_egress_action_delete function: %d (\"%s\")", error, bcm_errmsg(error));
    return L7_FAILURE;
  }

  /* Update resources availability */
  if (resources_xlate_egress > 0)
    resources_xlate_egress--;

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "Translation entry removed successfully");

  return L7_SUCCESS;
}

/**
 * Remove all ingress translations
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_hapi_xlate_ingress_action_delete_all(void)
{
  int error;

  error = bcmx_vlan_translate_delete_all();

  if (error != BCM_E_NONE && error != BCM_E_NOT_FOUND )
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Error calling bcm_vlan_translate_delete_all function: %d (\"%s\")", error, bcm_errmsg(error));
    return L7_FAILURE;
  }

  resources_xlate_ingress = FREE_RESOURCES_XLATE_INGRESS;

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "All ingress translations were removed");

  /* Update resources availability */
  return L7_SUCCESS;
}

/**
 * Remove all egress translations
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_hapi_xlate_egress_action_delete_all(void)
{
  int error;

  error = bcmx_vlan_translate_egress_delete_all();

  if (error != BCM_E_NONE && error != BCM_E_NOT_FOUND )
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Error calling bcm_vlan_translate_egress_delete_all function: %d (\"%s\")", error, bcm_errmsg(error));
    return L7_FAILURE;
  }

  /* Setting egress xlate class ids */
  if ( ptin_hapi_xlate_egress_portsGroup_init()!=L7_SUCCESS )
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Error setting class ids");
    return L7_FAILURE;
  }

  resources_xlate_egress = FREE_RESOURCES_XLATE_EGRESS;

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "All egress translations were removed");

  /* Update resources availability */
  return L7_SUCCESS;
}



/******************************** 
 * 3RD GENERATION FUNCTIONS
 ********************************/

/**
 * Replace outer VID and add a new inner VID
 * 
 * @param dapiPort: port reference (physical or LAG)
 * @param oVlanId: outer vlanId 
 * @param newOVlanId: new outer vlan id 
 * @param newIVlanId: new inner vlan id (0 to not add ivid)
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_hapi_xlate_ingress_replaceOVid_addIVid(ptin_dapi_port_t *dapiPort, L7_uint16 oVlanId, L7_uint16 newOVlanId, L7_uint16 newIVlanId)
{
  int error;
  bcm_vlan_translate_key_t keyType;
  bcm_vlan_action_set_t action;

  LOG_TRACE(LOG_CTX_PTIN_HAPI,"dapiPort={%d,%d,%d} oVlanId=%u newOVlanId=%u newIVlanId=%u",
            dapiPort->usp->unit, dapiPort->usp->slot, dapiPort->usp->port, oVlanId, newOVlanId, newIVlanId);

  /* Validate dapiPort */
  if (dapiPort->usp->unit<0 || dapiPort->usp->slot<0 || dapiPort->usp->port<0)
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Invalid interface");
    return L7_FAILURE;
  }

  /* Get port pointers */
  DAPIPORT_GET_PTR(dapiPort, dapiPortPtr, hapiPortPtr);
  /* Accept only physical and lag interfaces */
  if ( !IS_PORT_TYPE_PHYSICAL(dapiPortPtr) && !IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr) )
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Port {%d,%d,%d} is not physical neither logical lag (usp=)",
            dapiPort->usp->unit, dapiPort->usp->slot, dapiPort->usp->port);
    return L7_FAILURE;
  }

  /* Only look for interface and outar tag */
  keyType = bcmVlanTranslateKeyPortOuter;

  bcm_vlan_action_set_t_init(&action);
  action.dt_outer      = bcmVlanActionReplace;
  action.dt_inner      = /*(newIVlanId!=0) ? bcmVlanActionAdd :*/ bcmVlanActionNone;
  action.dt_outer_prio = bcmVlanActionNone;
  action.dt_inner_prio = bcmVlanActionNone;

  action.ot_outer      = bcmVlanActionReplace;
  action.ot_inner      = (newIVlanId!=0) ? bcmVlanActionAdd : bcmVlanActionNone;
  action.ot_outer_prio = bcmVlanActionNone;

  action.new_outer_vlan = newOVlanId;
  action.new_inner_vlan = newIVlanId;

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "bcm_vlan_translate_action_add(0, 0x%08X[%d], %u, %u, %u, &action)",
            hapiPortPtr->bcmx_lport, hapiPortPtr->bcm_port, keyType, oVlanId, 0);

  error = bcmx_vlan_translate_action_add(hapiPortPtr->bcmx_lport, keyType, oVlanId, 0, &action);

  if (error != BCM_E_NONE && error != BCM_E_EXISTS)
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Error calling bcm_vlan_translate_action_add function: %d (\"%s\")", error, bcm_errmsg(error));
    return L7_FAILURE;
  }

  /* Update resources availability */
  if (error == BCM_E_NONE)
  {
    if (resources_xlate_ingress < FREE_RESOURCES_XLATE_INGRESS)
      resources_xlate_ingress++;
  }

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "Translation entry added successfully: newOuterVlan=%u newInnerVlan=%u",newOVlanId,newIVlanId);

  return L7_SUCCESS;
}

/**
 * Replace Outer VID and remove inner VID
 * 
 * @param portgroup: port class id
 * @param oVlanId: outer vlan id 
 * @param iVlanId: inner vlan id (0 to not use inner vlan)
 * @param newOVlanId: new outer vlan id
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_hapi_xlate_egress_replaceOVid_deleteIVid(L7_uint32 portgroup, L7_uint16 oVlanId, L7_uint16 iVlanId, L7_uint16 newOVlanId)
{
  int error;
  bcm_vlan_action_set_t action;

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "portgroup=%u oVlanId=%u iVlanId=%u newOVlanId=%u", portgroup, oVlanId, iVlanId, newOVlanId);

  /* Add translation entry */
  bcm_vlan_action_set_t_init(&action);
  action.dt_outer      = bcmVlanActionReplace;
  action.dt_inner      = (iVlanId!=0) ? bcmVlanActionDelete : bcmVlanActionNone;
  action.dt_outer_prio = bcmVlanActionNone;
  action.dt_inner_prio = bcmVlanActionNone;

  action.ot_outer      = bcmVlanActionReplace;
  action.ot_inner      = bcmVlanActionNone;
  action.ot_outer_prio = bcmVlanActionNone;

  action.new_outer_vlan = newOVlanId;

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "bcm_vlan_translate_egress_action_add(0, %d, %u, %u, &action)", portgroup, oVlanId, iVlanId);

  error = bcmx_vlan_translate_egress_action_add(portgroup, oVlanId, iVlanId, &action);

  if (error != BCM_E_NONE && error != BCM_E_EXISTS)
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Error calling bcm_vlan_translate_egress_action_add function: %d (\"%s\")", error, bcm_errmsg(error));
    return L7_FAILURE;
  }

  /* Update resources availability */
  if (error == BCM_E_NONE)
  {
    if (resources_xlate_egress < FREE_RESOURCES_XLATE_EGRESS)
      resources_xlate_egress++;
  }

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "Translation entry added successfully: newOuterVlan=%u", newOVlanId);

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
  int error;
  bcm_vlan_translate_key_t keyType;
  bcm_vlan_action_set_t action;

  LOG_TRACE(LOG_CTX_PTIN_HAPI,"dapiPort={%d,%d,%d} oVlanId=%u iVlanId=%u",
            dapiPort->usp->unit, dapiPort->usp->slot, dapiPort->usp->port, xlate->outerVlanId, xlate->innerVlanId);

  /* Validate dapiPort */
  if (dapiPort->usp->unit<0 || dapiPort->usp->slot<0 || dapiPort->usp->port<0)
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI,"Invalid interface");
    return L7_FAILURE;
  }

  /* Get port pointers */
  DAPIPORT_GET_PTR(dapiPort, dapiPortPtr, hapiPortPtr);
  /* Accept only physical and lag interfaces */
  if ( !IS_PORT_TYPE_PHYSICAL(dapiPortPtr) && !IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr) )
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Port {%d,%d,%d} is not physical neither logical lag",
            dapiPort->usp->unit, dapiPort->usp->slot, dapiPort->usp->port);
    return L7_FAILURE;
  }

  /* For double tagged packets */
  if ( xlate->innerVlanId != 0 )
  {
    keyType = bcmVlanTranslateKeyPortDouble;
  }
  /* For single tagged packets */
  else
  {
    keyType = bcmVlanTranslateKeyPortOuter;
  }

  bcm_vlan_action_set_t_init(&action);

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "bcm_vlan_translate_action_add(0, 0x%08X[%d], %u, %u, %u, &action)",
            hapiPortPtr->bcmx_lport, hapiPortPtr->bcm_port, keyType, xlate->outerVlanId, xlate->innerVlanId);

  error = bcmx_vlan_translate_action_get(hapiPortPtr->bcmx_lport, keyType, xlate->outerVlanId, xlate->innerVlanId, &action);

  if (error != BCM_E_NONE)
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Error calling bcm_vlan_translate_action_get function: %d (\"%s\")", error, bcm_errmsg(error));
    return L7_FAILURE;
  }

  /* Extract new outer vlan */
  xlate->newOuterVlanId = action.new_outer_vlan;
  xlate->newInnerVlanId = action.new_inner_vlan;
  xlate->outerVlanAction = (xlate->innerVlanId!=0) ? action.dt_outer : action.ot_outer;
  xlate->innerVlanAction = (xlate->innerVlanId!=0) ? action.dt_inner : action.ot_inner;

  xlate->newOuterPrio   = action.priority;
  xlate->newInnerPrio   = action.new_inner_pkt_prio;
  xlate->outerPrioAction = (xlate->innerVlanId!=0) ? action.dt_outer_prio : action.ot_outer_prio;
  xlate->innerPrioAction = (xlate->innerVlanId!=0) ? action.dt_inner_prio : action.ot_inner_pkt_prio;

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "Translation entry read successfully: newOVid=%u.%u (OAction=%u.%u), newIVid=%u.%u (IAction=%u.%u)",
            xlate->newOuterVlanId , xlate->newOuterPrio,
            xlate->outerVlanAction, xlate->outerPrioAction,
            xlate->newInnerVlanId , xlate->newInnerPrio,
            xlate->innerVlanAction, xlate->innerPrioAction);

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
  int error;
  bcm_vlan_translate_key_t keyType;
  bcm_vlan_action_set_t action;

  LOG_TRACE(LOG_CTX_PTIN_HAPI,"dapiPort={%d,%d,%d} oVlanId=%u iVlanId=%u newOVlanId=%u (%u) newIVlanId=%u (%u)",
            dapiPort->usp->unit, dapiPort->usp->slot, dapiPort->usp->port,
            xlate->outerVlanId, xlate->innerVlanId,
            xlate->newOuterVlanId, xlate->outerVlanAction, xlate->newInnerVlanId, xlate->innerVlanAction);

  /* Validate dapiPort */
  if (dapiPort->usp->unit<0 || dapiPort->usp->slot<0 || dapiPort->usp->port<0)
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Invalid interface");
    return L7_FAILURE;
  }

  /* Get port pointers */
  DAPIPORT_GET_PTR(dapiPort, dapiPortPtr, hapiPortPtr);
  /* Accept only physical and lag interfaces */
  if ( !IS_PORT_TYPE_PHYSICAL(dapiPortPtr) && !IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr) )
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Port {%d,%d,%d} is not physical neither logical lag (usp=)",
            dapiPort->usp->unit, dapiPort->usp->slot, dapiPort->usp->port);
    return L7_FAILURE;
  }

  /* For double tagged packets */
  if ( xlate->innerVlanId != 0 )
  {
    keyType = bcmVlanTranslateKeyPortDouble;
    /* For double tagged packets we cannot use the ADD action */
//  if (xlate->outerAction==PTIN_XLATE_ACTION_ADD || xlate->outerAction==PTIN_XLATE_ACTION_ADD)
//  {
//    LOG_ERR(LOG_CTX_PTIN_HAPI, "Cannot use PTIN_XLATE_ACTION_ADD action for double tagged packets!");
//    return L7_FAILURE;
//  }
  }
  /* For single tagged packets */
  else
  {
    keyType = bcmVlanTranslateKeyPortOuter;
  }

  bcm_vlan_action_set_t_init(&action);

  /* VLAN actions */
  /* If it already exists, does not make sense to add: do not allow addition for double tagged packets */
  action.dt_outer      = (xlate->outerVlanAction!=PTIN_XLATE_ACTION_ADD) ? xlate->outerVlanAction : bcmVlanActionNone;  /* If it already exists, does not make sense to add */
  action.dt_inner      = (xlate->innerVlanAction!=PTIN_XLATE_ACTION_ADD) ? xlate->innerVlanAction : bcmVlanActionNone;  /* If it already exists, does not make sense to add. Instead replace */
  action.dt_outer_prio      = xlate->outerPrioAction;
  action.dt_outer_pkt_prio  = xlate->outerPrioAction;
  action.dt_inner_prio      = xlate->innerPrioAction;
  action.dt_inner_pkt_prio  = xlate->innerPrioAction;

  action.ot_outer      = (xlate->outerVlanAction!=PTIN_XLATE_ACTION_ADD) ? xlate->outerVlanAction : bcmVlanActionNone;  /* If it already exists, does not make sense to add */
  action.ot_inner      = (xlate->innerVlanAction==PTIN_XLATE_ACTION_ADD) ? xlate->innerVlanAction : bcmVlanActionNone;  /* If it does not exist, it only make sense to add */
  action.ot_outer_prio      = xlate->outerPrioAction;
  action.ot_outer_pkt_prio  = xlate->outerPrioAction;
  action.ot_inner_pkt_prio  = xlate->innerPrioAction;

  action.new_outer_vlan     = xlate->newOuterVlanId;
  action.new_inner_vlan     = xlate->newInnerVlanId;
  action.priority           = xlate->newOuterPrio;
  action.new_inner_pkt_prio = xlate->newInnerPrio;

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "bcm_vlan_translate_action_add(0, 0x%08X[%d], %u, %u, %u, &action)",
            hapiPortPtr->bcmx_lport, hapiPortPtr->bcm_port, keyType, xlate->outerVlanId, xlate->innerVlanId);

  LOG_TRACE(LOG_CTX_PTIN_HAPI,"action.new_outer_vlan     = %u", action.new_outer_vlan);
  LOG_TRACE(LOG_CTX_PTIN_HAPI,"action.new_inner_vlan     = %u", action.new_inner_vlan);
  LOG_TRACE(LOG_CTX_PTIN_HAPI,"action.priority           = %u", action.priority);
  LOG_TRACE(LOG_CTX_PTIN_HAPI,"action.new_inner_pkt_prio = %u", action.new_inner_pkt_prio);

  LOG_TRACE(LOG_CTX_PTIN_HAPI,"action.dt_outer           = %u", action.dt_outer);
  LOG_TRACE(LOG_CTX_PTIN_HAPI,"action.dt_inner           = %u", action.dt_inner);
  LOG_TRACE(LOG_CTX_PTIN_HAPI,"action.dt_outer_prio      = %u", action.dt_outer_prio);
  LOG_TRACE(LOG_CTX_PTIN_HAPI,"action.dt_outer_pkt_prio  = %u", action.dt_outer_pkt_prio);
  LOG_TRACE(LOG_CTX_PTIN_HAPI,"action.dt_inner_prio      = %u", action.dt_inner_prio);
  LOG_TRACE(LOG_CTX_PTIN_HAPI,"action.dt_inner_pkt_prio  = %u", action.dt_inner_pkt_prio);
  LOG_TRACE(LOG_CTX_PTIN_HAPI,"action.ot_outer           = %u", action.ot_outer);
  LOG_TRACE(LOG_CTX_PTIN_HAPI,"action.ot_inner           = %u", action.ot_inner);
  LOG_TRACE(LOG_CTX_PTIN_HAPI,"action.ot_outer_prio      = %u", action.ot_outer_prio);
  LOG_TRACE(LOG_CTX_PTIN_HAPI,"action.ot_outer_pkt_prio  = %u", action.ot_outer_pkt_prio);
  LOG_TRACE(LOG_CTX_PTIN_HAPI,"action.ot_inner_pkt_prio  = %u", action.ot_inner_pkt_prio);
  LOG_TRACE(LOG_CTX_PTIN_HAPI,"action.it_outer           = %u", action.it_outer);
  LOG_TRACE(LOG_CTX_PTIN_HAPI,"action.it_inner           = %u", action.it_inner);
  LOG_TRACE(LOG_CTX_PTIN_HAPI,"action.it_outer_prio      = %u", action.it_outer_pkt_prio);
  LOG_TRACE(LOG_CTX_PTIN_HAPI,"action.it_inner_pkt_prio  = %u", action.it_inner_pkt_prio);
  LOG_TRACE(LOG_CTX_PTIN_HAPI,"action.ut_outer           = %u", action.ut_outer);
  LOG_TRACE(LOG_CTX_PTIN_HAPI,"action.ut_inner           = %u", action.ut_inner);
  LOG_TRACE(LOG_CTX_PTIN_HAPI,"action.ut_outer_prio      = %u", action.ut_outer_pkt_prio);
  LOG_TRACE(LOG_CTX_PTIN_HAPI,"action.ut_inner_pkt_prio  = %u", action.ut_inner_pkt_prio);

  error = bcmx_vlan_translate_action_add(hapiPortPtr->bcmx_lport, keyType, xlate->outerVlanId, xlate->innerVlanId, &action);

  if (error != BCM_E_NONE && error != BCM_E_EXISTS)
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Error calling bcm_vlan_translate_action_add function: %d (\"%s\")", error, bcm_errmsg(error));
    return L7_FAILURE;
  }

  /* Update resources availability */
  if (error == BCM_E_NONE)
  {
    if (resources_xlate_ingress < FREE_RESOURCES_XLATE_INGRESS)
      resources_xlate_ingress++;
  }

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "Translation entry added successfully: newOVlanId=%u.%u (Oaction %u.%u) newIVlanId=%u.%u (Iaction %u.%u)",
            xlate->newOuterVlanId , xlate->newOuterPrio,
            xlate->outerVlanAction, xlate->outerPrioAction,
            xlate->newInnerVlanId , xlate->newInnerPrio,
            xlate->innerVlanAction, xlate->innerPrioAction);

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
  int error;
  bcm_vlan_translate_key_t keyType;

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "dapiPort={%d,%d,%d} oVlanId=%u iVlanId=%u",
            dapiPort->usp->unit, dapiPort->usp->slot, dapiPort->usp->port, xlate->outerVlanId, xlate->innerVlanId);

  /* Validate dapiPort */
  if (dapiPort->usp->unit<0 || dapiPort->usp->slot<0 || dapiPort->usp->port<0)
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Invalid interface");
    return L7_FAILURE;
  }

  /* Get port pointers */
  DAPIPORT_GET_PTR(dapiPort, dapiPortPtr, hapiPortPtr);
  /* Accept only physical and lag interfaces */
  if ( !IS_PORT_TYPE_PHYSICAL(dapiPortPtr) && !IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr) )
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Port {%d,%d,%d} is not physical neither logical lag",
            dapiPort->usp->unit, dapiPort->usp->slot, dapiPort->usp->port);
    return L7_FAILURE;
  }

  /* For double tagged packets */
  if ( xlate->innerVlanId != 0 )
  {
    keyType = bcmVlanTranslateKeyPortDouble;
  }
  /* For single tagged packets */
  else
  {
    keyType = bcmVlanTranslateKeyPortOuter;
  }

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "bcm_vlan_translate_action_delete(0, 0x%08X[%d], %u, %u, %u)",
            hapiPortPtr->bcmx_lport, hapiPortPtr->bcm_port, keyType, xlate->outerVlanId, xlate->innerVlanId);

  error = bcmx_vlan_translate_action_delete(hapiPortPtr->bcmx_lport, keyType, xlate->outerVlanId, xlate->innerVlanId);

  if (error != BCM_E_NONE && error != BCM_E_NOT_FOUND )
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Error calling bcm_vlan_translate_action_delete function: %d (\"%s\")", error, bcm_errmsg(error));
    return L7_FAILURE;
  }

  /* Update resources availability */
  if ( error == BCM_E_NONE )
  {
    if (resources_xlate_ingress > 0 )
      resources_xlate_ingress--;
  }

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "Translation entry removed successfully");

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

  error = bcmx_vlan_translate_delete_all();

  if (error != BCM_E_NONE && error != BCM_E_NOT_FOUND )
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Error calling bcm_vlan_translate_delete_all function: %d (\"%s\")", error, bcm_errmsg(error));
    return L7_FAILURE;
  }

  resources_xlate_ingress = FREE_RESOURCES_XLATE_INGRESS;

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "All ingress translations were removed");

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

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "portgroup=%u oVlanId=%u iVlanId=%u", portgroup, xlate->outerVlanId, xlate->innerVlanId);

  bcm_vlan_action_set_t_init(&action);

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "bcm_vlan_translate_egress_action_add(0, %d, %u, %u, &action)", portgroup, xlate->outerVlanId, xlate->innerVlanId);

  error = bcmx_vlan_translate_egress_action_get(portgroup, xlate->outerVlanId, xlate->innerVlanId, &action);

  if (error != BCM_E_NONE)
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Error calling bcm_vlan_translate_egress_action_get function: %d (\"%s\")", error, bcm_errmsg(error));
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

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "Translation entry read successfully: newOuterVlan=%u.%u (Oaction %u.%u) newInnerVlan=%u.%u (Iaction %u.%u)",
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
L7_RC_t ptin_hapi_xlate_egress_add(L7_uint32 portgroup, ptin_hapi_xlate_t *xlate)
{
  int error;
  bcm_vlan_action_set_t action;

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "portgroup=%u oVlanId=%u iVlanId=%u newOVlanId=%u(%u) newIVlanId=%u(%u)",portgroup,
            xlate->outerVlanId,xlate->innerVlanId,
            xlate->newOuterVlanId,xlate->outerVlanAction,
            xlate->newInnerVlanId,xlate->innerVlanAction);

  /* Do not allow ADD operation for double-tagged packets */
//if (xlate->innerVlanId!=0 &&
//    (xlate->outerAction==PTIN_XLATE_ACTION_ADD || xlate->innerAction==PTIN_XLATE_ACTION_ADD))
//{
//  LOG_ERR(LOG_CTX_PTIN_HAPI, "Cannot use PTIN_XLATE_ACTION_ADD action for double tagged packets!");
//  return L7_FAILURE;
//}

  /* Add translation entry */
  bcm_vlan_action_set_t_init(&action);
  action.dt_outer      = (xlate->outerVlanAction!=PTIN_XLATE_ACTION_ADD) ? xlate->outerVlanAction : bcmVlanActionNone;  /* If it already exists, does not make sense to add */
  action.dt_inner      = (xlate->innerVlanAction!=PTIN_XLATE_ACTION_ADD) ? xlate->innerVlanAction : bcmVlanActionNone;  /* If it already exists, does not make sense to add. Instead replace */
  action.dt_outer_prio      = xlate->outerPrioAction;
  action.dt_outer_pkt_prio  = xlate->outerPrioAction;
  action.dt_inner_prio      = xlate->innerPrioAction;
  action.dt_inner_pkt_prio  = xlate->innerPrioAction;

  action.ot_outer      = (xlate->outerVlanAction!=PTIN_XLATE_ACTION_ADD) ? xlate->outerVlanAction : bcmVlanActionNone;  /* If it already exists, does not make sense to add */
  action.ot_inner      = (xlate->innerVlanAction==PTIN_XLATE_ACTION_ADD) ? xlate->innerVlanAction : bcmVlanActionNone;  /* If it does not exist, it only make sense to add */
  action.ot_outer_prio      = xlate->outerPrioAction;
  action.ot_outer_pkt_prio  = xlate->outerPrioAction;
  action.ot_inner_pkt_prio  = xlate->innerPrioAction;

  action.new_outer_vlan     = xlate->newOuterVlanId;
  action.new_inner_vlan     = xlate->newInnerVlanId;
  action.priority           = xlate->newOuterPrio;
  action.new_inner_pkt_prio = xlate->newInnerPrio;

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "bcm_vlan_translate_egress_action_add(0, %d, %u.%u, %u.%u, &action)", portgroup,
            xlate->outerVlanId, xlate->outerPrio,
            xlate->innerVlanId, xlate->innerPrio);

  LOG_TRACE(LOG_CTX_PTIN_HAPI,"action.new_outer_vlan     = %d", action.new_outer_vlan);
  LOG_TRACE(LOG_CTX_PTIN_HAPI,"action.new_inner_vlan     = %d", action.new_inner_vlan);
  LOG_TRACE(LOG_CTX_PTIN_HAPI,"action.priority           = %d", action.priority);
  LOG_TRACE(LOG_CTX_PTIN_HAPI,"action.new_inner_pkt_prio = %d", action.new_inner_pkt_prio);

  LOG_TRACE(LOG_CTX_PTIN_HAPI,"action.dt_outer           = %d", action.dt_outer);
  LOG_TRACE(LOG_CTX_PTIN_HAPI,"action.dt_inner           = %d", action.dt_inner);
  LOG_TRACE(LOG_CTX_PTIN_HAPI,"action.dt_outer_prio      = %d", action.dt_outer_prio);
  LOG_TRACE(LOG_CTX_PTIN_HAPI,"action.dt_outer_pkt_prio  = %d", action.dt_outer_pkt_prio);
  LOG_TRACE(LOG_CTX_PTIN_HAPI,"action.dt_inner_prio      = %d", action.dt_inner_prio);
  LOG_TRACE(LOG_CTX_PTIN_HAPI,"action.dt_inner_pkt_prio  = %d", action.dt_inner_pkt_prio);
  LOG_TRACE(LOG_CTX_PTIN_HAPI,"action.ot_outer           = %d", action.ot_outer);
  LOG_TRACE(LOG_CTX_PTIN_HAPI,"action.ot_inner           = %d", action.ot_inner);
  LOG_TRACE(LOG_CTX_PTIN_HAPI,"action.ot_outer_prio      = %d", action.ot_outer_prio);
  LOG_TRACE(LOG_CTX_PTIN_HAPI,"action.ot_outer_pkt_prio  = %d", action.ot_outer_pkt_prio);
  LOG_TRACE(LOG_CTX_PTIN_HAPI,"action.ot_inner_pkt_prio  = %d", action.ot_inner_pkt_prio);
  LOG_TRACE(LOG_CTX_PTIN_HAPI,"action.it_outer           = %u", action.it_outer);
  LOG_TRACE(LOG_CTX_PTIN_HAPI,"action.it_inner           = %u", action.it_inner);
  LOG_TRACE(LOG_CTX_PTIN_HAPI,"action.it_outer_prio      = %u", action.it_outer_pkt_prio);
  LOG_TRACE(LOG_CTX_PTIN_HAPI,"action.it_inner_pkt_prio  = %u", action.it_inner_pkt_prio);
  LOG_TRACE(LOG_CTX_PTIN_HAPI,"action.ut_outer           = %u", action.ut_outer);
  LOG_TRACE(LOG_CTX_PTIN_HAPI,"action.ut_inner           = %u", action.ut_inner);
  LOG_TRACE(LOG_CTX_PTIN_HAPI,"action.ut_outer_prio      = %u", action.ut_outer_pkt_prio);
  LOG_TRACE(LOG_CTX_PTIN_HAPI,"action.ut_inner_pkt_prio  = %u", action.ut_inner_pkt_prio);

  error = bcmx_vlan_translate_egress_action_add(portgroup, xlate->outerVlanId, xlate->innerVlanId, &action);

  if (error != BCM_E_NONE && error != BCM_E_EXISTS)
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Error calling bcm_vlan_translate_egress_action_add function: %d (\"%s\")", error, bcm_errmsg(error));
    return L7_FAILURE;
  }

  /* Update resources availability */
  if (error == BCM_E_NONE)
  {
    if (resources_xlate_egress < FREE_RESOURCES_XLATE_EGRESS)
      resources_xlate_egress++;
  }

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "Translation entry added successfully: newOVlanId=%u.%u (Oaction %u.%u) newIVlanId=%u.%u (Iaction %u.%u)",
            xlate->newOuterVlanId , xlate->newOuterPrio,
            xlate->outerVlanAction, xlate->outerPrioAction,
            xlate->newInnerVlanId , xlate->newInnerPrio,
            xlate->innerVlanAction, xlate->innerPrioAction);

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
L7_RC_t ptin_hapi_xlate_egress_delete(L7_uint32 portgroup, ptin_hapi_xlate_t *xlate)
{
  int error;

  LOG_TRACE(LOG_CTX_PTIN_HAPI,"portgroup=%u oVlanId=%u iVlanId=%u", portgroup, xlate->outerVlanId, xlate->innerVlanId);

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "bcm_vlan_translate_egress_action_delete(0,%d,%u,%u)", portgroup, xlate->outerVlanId, xlate->innerVlanId);

  error = bcmx_vlan_translate_egress_action_delete(portgroup, xlate->outerVlanId, xlate->innerVlanId);

  if (error != BCM_E_NONE && error != BCM_E_NOT_FOUND )
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Error calling bcm_vlan_translate_egress_action_delete function: %d (\"%s\")", error, bcm_errmsg(error));
    return L7_FAILURE;
  }

  /* Update resources availability */
  if (resources_xlate_egress > 0)
    resources_xlate_egress--;

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "Translation entry removed successfully");

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
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Error calling bcm_vlan_translate_egress_delete_all function: %d (\"%s\")", error, bcm_errmsg(error));
    return L7_FAILURE;
  }

  /* Setting egress xlate class ids */
  if ( ptin_hapi_xlate_egress_portsGroup_init()!=L7_SUCCESS )
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Error setting class ids");
    return L7_FAILURE;
  }

  resources_xlate_egress = FREE_RESOURCES_XLATE_EGRESS;

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "All egress translations were removed");

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

  LOG_TRACE(LOG_CTX_PTIN_HAPI,"portgroup=%u usp_list[0]={%d,%d,%d} usp_list_size=%u",
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
      LOG_ERR(LOG_CTX_PTIN_HAPI,"Invalid interface: {%d,%d,%d}", usp_list[index]->unit, usp_list[index]->slot, usp_list[index]->port);
      rc = L7_FAILURE;
      continue;
    }

    /* Get port pointers *of the group port */
    dapiPortPtr = DAPI_PORT_GET( usp_list[index], dapi_g );
    hapiPortPtr = HAPI_PORT_GET( usp_list[index], dapi_g );

    /* Accept only physical interfaces */
    if ( !IS_PORT_TYPE_PHYSICAL(dapiPortPtr) /*&& !IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr)*/ )
    {
      LOG_ERR(LOG_CTX_PTIN_HAPI, "Port {%d,%d,%d} is not physical", usp_list[index]->unit, usp_list[index]->slot, usp_list[index]->port);
      rc = L7_FAILURE;
      continue;
    }

    LOG_TRACE(LOG_CTX_PTIN_HAPI, "Setting class id %d to single port {%d,%d,%d}",
              portgroup, usp_list[index]->unit, usp_list[index]->slot, usp_list[index]->port);
    if (bcmx_port_class_set(hapiPortPtr->bcmx_lport, bcmPortClassVlanTranslateEgress, (L7_uint32) portgroup ) != BCM_E_NONE)
    {
      LOG_ERR(LOG_CTX_PTIN_HAPI, "Error setting class id %d to single port {%d,%d,%d} [VLAN XLATE]",
              portgroup, usp_list[index]->unit, usp_list[index]->slot, usp_list[index]->port);
      rc = L7_FAILURE;
      continue;
    }
    /* PTin removed: Let Fastpath to manage EFP Class ids */
    #if 0
    if (bcmx_port_class_set(hapiPortPtr->bcmx_lport, bcmPortClassFieldEgress, (L7_uint32) portgroup + EFP_STD_CLASS_ID_MAX) != BCM_E_NONE)
    {
      LOG_ERR(LOG_CTX_PTIN_HAPI, "Error setting class id %d to single port {%d,%d,%d} [ECAP]",
              portgroup, usp_list[index]->unit, usp_list[index]->slot, usp_list[index]->port);
      rc = L7_FAILURE;
      continue;
    }
    #endif
  }

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "Class id %d successfully assigned to the given ports", portgroup);

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
  L7_uint32 classId;

  LOG_TRACE(LOG_CTX_PTIN_HAPI,"usp={%d,%d,%d}",usp->unit,usp->slot,usp->port);

  /* Validate port group */
  if (usp->unit<0 || usp->slot<0 || usp->port<0)
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI,"Invalid portgroup");
    return L7_FAILURE;
  }

  /* Get port pointers *of the group port */
  dapiPortPtr = DAPI_PORT_GET( usp, dapi_g );
  hapiPortPtr = HAPI_PORT_GET( usp, dapi_g );

  /* Accept only physical and lag interfaces */
  if ( !IS_PORT_TYPE_PHYSICAL(dapiPortPtr) /*&& !IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr)*/ )
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Port {%d,%d,%d} is not physical",usp->unit,usp->slot,usp->port);
    return L7_FAILURE;
  }

  /* Extract class id */
  if (bcmx_port_class_get(hapiPortPtr->bcmx_lport, bcmPortClassVlanTranslateEgress, &classId ) != BCM_E_NONE)
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Error getting class id from port {%d,%d,%d}", usp->unit, usp->slot, usp->port);
    return L7_FAILURE;
  }

  /* Output value */
  if (portgroup!=L7_NULLPTR)
  {
    *portgroup = (L7_int) classId;
  }

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "Class id %u successfully extracted from port {%d,%d,%d}", classId, usp->unit, usp->slot, usp->port);

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
      LOG_ERR(LOG_CTX_PTIN_HAPI, "Error getting bcm unit id");
      rc = L7_FAILURE;
      continue;
    }

    /* Default class ids is port+1 */
    if (bcm_port_class_set(bcm_unit, bcm_port, bcmPortClassVlanTranslateEgress, port+1 ) != BCM_E_NONE)
    {
      LOG_ERR(LOG_CTX_PTIN_HAPI, "Error setting class id %u to port %d [VLAN XLATE]", port+1, port);
      rc = L7_FAILURE;
      continue;
    }
    /* PTin removed: Let Fastpath to manage EFP Class ids */
    #if 0
    if (bcm_port_class_set(bcm_unit, bcm_port, bcmPortClassFieldEgress, port+1 ) != BCM_E_NONE)
    {
      LOG_ERR(LOG_CTX_PTIN_HAPI, "Error setting class id %u to port %d [ECAP]", port+1, port);
      rc = L7_FAILURE;
      continue;
    }
    #endif
  }

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "Class ids assigned: rc=%d", rc);

  return rc;
}

