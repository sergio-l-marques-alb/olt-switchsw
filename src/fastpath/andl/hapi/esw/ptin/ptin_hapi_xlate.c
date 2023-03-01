#include <bcm/error.h>

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
  L7_BOOL enable;
  L7_uint32     bcm_port, bcm_unit;
  DAPI_USP_t    usp;
  BROAD_PORT_t  *hapiPortPtr;
  extern DAPI_t *dapi_g;
  L7_RC_t rc = L7_SUCCESS;

  /* Validate dapi_g pointers */
  if (dapi_g == L7_NULLPTR)
  {
      PT_LOG_ERR(LOG_CTX_STARTUP, "dapi_g is not initialized");
      return L7_FAILURE;
  }

  /* Change Ingress Vlan Translate Keys for all physical ports (LAGs are included through their physical ports) */
  /* Run all usp ports */
  USP_PHYPORT_ITERATE(usp, dapi_g)
  {
    hapiPortPtr = HAPI_PORT_GET(&usp, dapi_g);
    bcm_port    = hapiPortPtr->bcm_port;
    bcm_unit    = hapiPortPtr->bcm_unit;

    /* Enable translations? */
    enable = L7_TRUE;
    /* Disable for FPGA port of OLT1T0 */
    #if (PTIN_BOARD == PTIN_BOARD_OLT1T0 || PTIN_BOARD == PTIN_BOARD_OLT1T0F)
    if (usp.port == PTIN_PORT_FPGA)
    {
      enable = L7_FALSE;
    }
    #endif

    /* First key: First do a lookup for port + outerVlan + innerVlan.
       Second key: If failed do a second lookup for port + outerVlan */
    if ( (bcm_vlan_control_port_set( bcm_unit, bcm_port,bcmVlanPortTranslateKeyFirst, bcmVlanTranslateKeyPortDouble) != L7_SUCCESS) ||
         (bcm_vlan_control_port_set( bcm_unit, bcm_port,bcmVlanPortTranslateKeySecond, bcmVlanTranslateKeyPortOuter) != L7_SUCCESS) )
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "Error setting translation keys");
      rc = L7_FAILURE;
      continue;
    }

    /* Enable ingress and egress translations.
       Also, drop packets that do not fullfil any translation entry. */
    if ((bcm_vlan_control_port_set( bcm_unit, bcm_port, bcmVlanTranslateIngressEnable,   enable) != L7_SUCCESS) ||
        (bcm_vlan_control_port_set( bcm_unit, bcm_port, bcmVlanTranslateIngressMissDrop, enable) != L7_SUCCESS) ||
        (bcm_vlan_control_port_set( bcm_unit, bcm_port, bcmVlanTranslateEgressEnable,    enable) != L7_SUCCESS) ||
        (bcm_vlan_control_port_set( bcm_unit, bcm_port, bcmVlanTranslateEgressMissDrop,  enable) != L7_SUCCESS) )
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "Error setting translation enables");
      rc = L7_FAILURE;
      continue;
    }
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

  PT_LOG_TRACE(LOG_CTX_HAPI, "dapiPort={%d,%d,%d} oVlanId=%u newOVlanId=%u",
            dapiPort->usp->unit, dapiPort->usp->slot, dapiPort->usp->port, oVlanId, newOVlanId);

  /* Validate dapiPort */
  if (dapiPort->usp->unit<0 || dapiPort->usp->slot<0 || dapiPort->usp->port<0)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Invalid interface");
    return L7_FAILURE;
  }

  /* Get port pointers */
  DAPIPORT_GET_PTR(dapiPort, dapiPortPtr, hapiPortPtr);
  /* Accept only physical and lag interfaces */
  if ( !IS_PORT_TYPE_PHYSICAL(dapiPortPtr) && !IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr) )
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Port {%d,%d,%d} is not physical neither logical lag",
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

  PT_LOG_TRACE(LOG_CTX_HAPI, "bcm_vlan_translate_action_add(unit=%u, port=%u, keyType=%u, ovlan=%u, ivlan=%u, &action)",
               hapiPortPtr->bcm_unit, hapiPortPtr->bcm_gport, keyType, oVlanId, 0);

  error = bcm_vlan_translate_action_add(hapiPortPtr->bcm_unit, hapiPortPtr->bcm_gport, keyType, oVlanId, 0, &action);

  if (error == BCM_E_EXISTS)
  {
    PT_LOG_WARN(LOG_CTX_HAPI, "Entry already exists: %d (\"%s\")", error, bcm_errmsg(error));
    return L7_ALREADY_CONFIGURED;
  }
  else if (error == BCM_E_FULL)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Translation resources exhausted: rv=%d (%s)", error, bcm_errmsg(error));
    return L7_NO_RESOURCES;
  }
  else if (error != BCM_E_NONE)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Error calling bcm_vlan_translate_action_add function: %d (\"%s\")", error, bcm_errmsg(error));
    return L7_FAILURE;
  }

  /* Update resources availability */
  if (error == BCM_E_NONE)
  {
    if (resources_xlate_ingress < FREE_RESOURCES_XLATE_INGRESS)
      resources_xlate_ingress++;
  }

  PT_LOG_TRACE(LOG_CTX_HAPI, "Translation entry added successfully: newOuterVlan=%u [rc=%d]", newOVlanId, error);

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

  PT_LOG_TRACE(LOG_CTX_HAPI, "dapiPort={%d,%d,%d} oVlanId=%u",
            dapiPort->usp->unit, dapiPort->usp->slot, dapiPort->usp->port, oVlanId);

  /* Validate dapiPort */
  if (dapiPort->usp->unit<0 || dapiPort->usp->slot<0 || dapiPort->usp->port<0)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Invalid interface");
    return L7_FAILURE;
  }

  /* Get port pointers */
  DAPIPORT_GET_PTR(dapiPort, dapiPortPtr, hapiPortPtr);
  /* Accept only physical and lag interfaces */
  if ( !IS_PORT_TYPE_PHYSICAL(dapiPortPtr) && !IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr) )
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Port {%d,%d,%d} is not physical neither logical lag",
            dapiPort->usp->unit, dapiPort->usp->slot, dapiPort->usp->port);
    return L7_FAILURE;
  }

  keyType = bcmVlanTranslateKeyPortOuter;

  PT_LOG_TRACE(LOG_CTX_HAPI, "bcm_vlan_translate_action_delete(unit=%u, port=%u, keyType=%u, oVlan=%u, ivlan=%u)",
               hapiPortPtr->bcm_unit, hapiPortPtr->bcm_gport, keyType, oVlanId, 0);

  error = bcm_vlan_translate_action_delete(hapiPortPtr->bcm_unit, hapiPortPtr->bcm_gport, keyType, oVlanId, 0);

  if (error != BCM_E_NONE && error != BCM_E_NOT_FOUND )
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Error calling bcm_vlan_translate_action_delete function: %d (\"%s\")",error,bcm_errmsg(error));
    return L7_FAILURE;
  }

  /* Update resources availability */
  if ( error == BCM_E_NONE )
  {
    if (resources_xlate_ingress > 0 )
      resources_xlate_ingress--;
  }

  PT_LOG_TRACE(LOG_CTX_HAPI, "Translation entry removed successfully [rc=%d]", error);

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

  PT_LOG_TRACE(LOG_CTX_HAPI,"dapiPort={%d,%d,%d} oVlanId=%u iVlanId=%u newOVlanId=%u",
            dapiPort->usp->unit, dapiPort->usp->slot, dapiPort->usp->port, oVlanId, iVlanId, newOVlanId);

  /* Validate dapiPort */
  if (dapiPort->usp->unit<0 || dapiPort->usp->slot<0 || dapiPort->usp->port<0)
  {
    PT_LOG_ERR(LOG_CTX_HAPI,"Invalid interface");
    return L7_FAILURE;
  }

  /* Get port pointers */
  DAPIPORT_GET_PTR(dapiPort, dapiPortPtr, hapiPortPtr);
  /* Accept only physical and lag interfaces */
  if ( !IS_PORT_TYPE_PHYSICAL(dapiPortPtr) && !IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr) )
  {
    PT_LOG_ERR(LOG_CTX_HAPI, " ERROR: Port {%d,%d,%d} is not physical neither logical lag",
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

  PT_LOG_TRACE(LOG_CTX_HAPI, "bcm_vlan_translate_action_add(unit=%u, port%u, keyType=%u, ovlan=%u, ivlan=%u, &action)",
               hapiPortPtr->bcm_unit, hapiPortPtr->bcm_gport, keyType, oVlanId, iVlanId);

  error = bcm_vlan_translate_action_add(hapiPortPtr->bcm_unit, hapiPortPtr->bcm_gport, keyType, oVlanId, iVlanId, &action);

  if (error == BCM_E_EXISTS)
  {
    PT_LOG_WARN(LOG_CTX_HAPI, "Entry already exists: %d (\"%s\")", error, bcm_errmsg(error));
    return L7_ALREADY_CONFIGURED;
  }
  else if (error == BCM_E_FULL)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Translation resources exhausted: rv=%d (%s)", error, bcm_errmsg(error));
    return L7_NO_RESOURCES;
  }
  else if (error != BCM_E_NONE)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Error calling bcm_vlan_translate_action_add function: %d (\"%s\")", error, bcm_errmsg(error));
    return L7_FAILURE;
  }

  if (error == BCM_E_NONE)
  {
    if (resources_xlate_ingress < FREE_RESOURCES_XLATE_INGRESS)
      resources_xlate_ingress++;
  }

  PT_LOG_TRACE(LOG_CTX_HAPI, "Translation entry added successfully: newOuterVlan=%u [rc=%d]", newOVlanId, error);

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

  PT_LOG_TRACE(LOG_CTX_HAPI,"dapiPort={%d,%d,%d} oVlanId=%u iVlanId=%u",
            dapiPort->usp->unit, dapiPort->usp->slot, dapiPort->usp->port, oVlanId,iVlanId);

  /* Validate dapiPort */
  if (dapiPort->usp->unit<0 || dapiPort->usp->slot<0 || dapiPort->usp->port<0)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Invalid interface");
    return L7_FAILURE;
  }

  /* Get port pointers */
  DAPIPORT_GET_PTR(dapiPort, dapiPortPtr, hapiPortPtr);
  /* Accept only physical and lag interfaces */
  if ( !IS_PORT_TYPE_PHYSICAL(dapiPortPtr) && !IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr) )
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Port {%d,%d,%d} is not physical neither logical lag",
            dapiPort->usp->unit, dapiPort->usp->slot, dapiPort->usp->port);
    return L7_FAILURE;
  }

  keyType = bcmVlanTranslateKeyPortDouble;

  PT_LOG_TRACE(LOG_CTX_HAPI, "bcm_vlan_translate_action_delete(unit=%u, port=%u, keyType=%u, ovlan=%u, ivlan=%u)",
               hapiPortPtr->bcm_unit, hapiPortPtr->bcm_gport, keyType, oVlanId, iVlanId);

  error = bcm_vlan_translate_action_delete(hapiPortPtr->bcm_unit, hapiPortPtr->bcm_gport, keyType, oVlanId, iVlanId);

  if (error != BCM_E_NONE && error != BCM_E_NOT_FOUND )
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Error calling bcm_vlan_translate_action_delete function: %d (\"%s\")", error, bcm_errmsg(error));
    return L7_FAILURE;
  }

  if ( error == BCM_E_NONE )
  {
    if (resources_xlate_ingress > 0 )
      resources_xlate_ingress--;
  }

  PT_LOG_TRACE(LOG_CTX_HAPI, "Translation entry removed successfully [rc=%d]", error);

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

  PT_LOG_TRACE(LOG_CTX_HAPI,"dapiPort={%d,%d,%d} oVlanId=%u iVlanId=%u",
            dapiPort->usp->unit, dapiPort->usp->slot, dapiPort->usp->port, oVlanId, iVlanId);

  /* Validate dapiPort */
  if (dapiPort->usp->unit<0 || dapiPort->usp->slot<0 || dapiPort->usp->port<0)
  {
    PT_LOG_ERR(LOG_CTX_HAPI,"Invalid interface");
    return L7_FAILURE;
  }

  /* Get port pointers */
  DAPIPORT_GET_PTR(dapiPort, dapiPortPtr, hapiPortPtr);
  /* Accept only physical and lag interfaces */
  if ( !IS_PORT_TYPE_PHYSICAL(dapiPortPtr) && !IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr) )
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Port {%d,%d,%d} is not physical neither logical lag",
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

  PT_LOG_TRACE(LOG_CTX_HAPI, "bcm_vlan_translate_action_get(unit=%u, port=%u, keyType=%u, ovlan=%u, ivlan=%u, &action)",
               hapiPortPtr->bcm_unit, hapiPortPtr->bcm_gport, keyType, oVlanId, iVlanId);

  error = bcm_vlan_translate_action_get(hapiPortPtr->bcm_unit, hapiPortPtr->bcm_gport, keyType, oVlanId, iVlanId, &action);

  if (error != BCM_E_NONE)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Error calling bcm_vlan_translate_action_get function: %d (\"%s\")", error, bcm_errmsg(error));
    return L7_FAILURE;
  }

  /* Extract new outer vlan */
  if ( newOVlanId != L7_NULLPTR )
  {
    *newOVlanId = action.new_outer_vlan;
    PT_LOG_TRACE(LOG_CTX_HAPI, "Translation entry read successfully: newOuterVlan=%u", *newOVlanId);
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

  PT_LOG_TRACE(LOG_CTX_HAPI,"dapiPort={%d,%d,%d} oVlanId=%u iVlanId=%u newOVlanId=%u",
            dapiPort->usp->unit, dapiPort->usp->slot, dapiPort->usp->port, oVlanId, iVlanId, newOVlanId);

  /* Validate dapiPort */
  if (dapiPort->usp->unit<0 || dapiPort->usp->slot<0 || dapiPort->usp->port<0)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Invalid interface");
    return L7_FAILURE;
  }

  /* Get port pointers */
  DAPIPORT_GET_PTR(dapiPort, dapiPortPtr, hapiPortPtr);
  /* Accept only physical and lag interfaces */
  if ( !IS_PORT_TYPE_PHYSICAL(dapiPortPtr) && !IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr) )
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Port {%d,%d,%d} is not physical neither logical lag (usp=)",
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

  PT_LOG_TRACE(LOG_CTX_HAPI, "bcm_vlan_translate_action_add(unit=%u, port=%u, keyType=%u, ovlan=%u, ivlan=%u, &action)",
               hapiPortPtr->bcm_unit, hapiPortPtr->bcm_gport, keyType, oVlanId, iVlanId);

  error = bcm_vlan_translate_action_add(hapiPortPtr->bcm_unit, hapiPortPtr->bcm_gport, keyType, oVlanId, iVlanId, &action);

  if (error == BCM_E_EXISTS)
  {
    PT_LOG_WARN(LOG_CTX_HAPI, "Entry already exists: %d (\"%s\")", error, bcm_errmsg(error));
    return L7_ALREADY_CONFIGURED;
  }
  else if (error == BCM_E_FULL)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Translation resources exhausted: rv=%d (%s)", error, bcm_errmsg(error));
    return L7_NO_RESOURCES;
  }
  else if (error != BCM_E_NONE)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Error calling bcm_vlan_translate_action_add function: %d (\"%s\")", error, bcm_errmsg(error));
    return L7_FAILURE;
  }

  /* Update resources availability */
  if (error == BCM_E_NONE)
  {
    if (resources_xlate_ingress < FREE_RESOURCES_XLATE_INGRESS)
      resources_xlate_ingress++;
  }

  PT_LOG_TRACE(LOG_CTX_HAPI, "Translation entry added successfully: newOuterVlan=%u [%d]", newOVlanId, error);

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

  PT_LOG_TRACE(LOG_CTX_HAPI, "dapiPort={%d,%d,%d} oVlanId=%u iVlanId=%u",
            dapiPort->usp->unit, dapiPort->usp->slot, dapiPort->usp->port, oVlanId, iVlanId);

  /* Validate dapiPort */
  if (dapiPort->usp->unit<0 || dapiPort->usp->slot<0 || dapiPort->usp->port<0)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Invalid interface");
    return L7_FAILURE;
  }

  /* Get port pointers */
  DAPIPORT_GET_PTR(dapiPort, dapiPortPtr, hapiPortPtr);
  /* Accept only physical and lag interfaces */
  if ( !IS_PORT_TYPE_PHYSICAL(dapiPortPtr) && !IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr) )
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Port {%d,%d,%d} is not physical neither logical lag",
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

  PT_LOG_TRACE(LOG_CTX_HAPI, "bcm_vlan_translate_action_delete(unit=%u, port=%u, keyType=%u, ovlan=%u, ivlan=%u)",
               hapiPortPtr->bcm_unit, hapiPortPtr->bcm_gport, keyType, oVlanId, iVlanId);

  error = bcm_vlan_translate_action_delete(hapiPortPtr->bcm_unit, hapiPortPtr->bcm_gport, keyType, oVlanId, iVlanId);

  if (error != BCM_E_NONE && error != BCM_E_NOT_FOUND )
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Error calling bcm_vlan_translate_action_delete function: %d (\"%s\")", error, bcm_errmsg(error));
    return L7_FAILURE;
  }

  /* Update resources availability */
  if ( error == BCM_E_NONE )
  {
    if (resources_xlate_ingress > 0 )
      resources_xlate_ingress--;
  }

  PT_LOG_TRACE(LOG_CTX_HAPI, "Translation entry removed successfully [rc=%d]", error);

  return L7_SUCCESS;
}

/**
 * Get egress translation configuration for a given class id, 
 * outer vlan and inner vlan 
 *  
 * @param unit : bcm_unit 
 * @param portgroup: port class id
 * @param oVlanId: outer vlan id 
 * @param iVlanId: inner vlan id (0 to not use inner vlan)
 * @param *newOVlanId: new outer vlan id
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_hapi_xlate_egress_action_get(int unit, L7_uint32 portgroup, L7_uint16 oVlanId, L7_uint16 iVlanId, L7_uint16 *newOVlanId)
{
  int error;
  bcm_vlan_action_set_t action;

  PT_LOG_TRACE(LOG_CTX_HAPI, "unit=%u portgroup=%u oVlanId=%u iVlanId=%u",
               unit, portgroup, oVlanId, iVlanId);

  bcm_vlan_action_set_t_init(&action);

  PT_LOG_TRACE(LOG_CTX_HAPI, "bcm_vlan_translate_egress_action_add(unit=%u, portgroup=%u, ovlan=%u, ivlan=%u, &action)",
               unit, portgroup, oVlanId, iVlanId);

  error = bcm_vlan_translate_egress_action_get(unit, portgroup, oVlanId, iVlanId, &action);

  if (error != BCM_E_NONE)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Error calling bcm_vlan_translate_egress_action_get function: %d (\"%s\")", error, bcm_errmsg(error));
    return L7_FAILURE;
  }

  /* Extract new outer vlan */
  if ( newOVlanId != L7_NULLPTR )
  {
    *newOVlanId = action.new_outer_vlan;
    PT_LOG_TRACE(LOG_CTX_HAPI, "Translation entry read successfully: newOuterVlan=%u", *newOVlanId);
  }

  return L7_SUCCESS;
}

/**
 * Translate single or double-tagged packets to another 
 * outer-vlan at the egress stage 
 *  
 * @param unit : bcm_unit 
 * @param portgroup: port class id
 * @param oVlanId: outer vlan id 
 * @param iVlanId: inner vlan id (0 to not use inner vlan)
 * @param newOVlanId: new outer vlan id
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_hapi_xlate_egress_action_add(int unit, L7_uint32 portgroup, L7_uint16 oVlanId, L7_uint16 iVlanId, L7_uint16 newOVlanId)
{
  int error;
  bcm_vlan_action_set_t action;

  PT_LOG_TRACE(LOG_CTX_HAPI, "unit=%u portgroup=%u oVlanId=%u iVlanId=%u newOVlanId=%u",
               unit, portgroup, oVlanId, iVlanId, newOVlanId);

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

  PT_LOG_TRACE(LOG_CTX_HAPI, "bcm_vlan_translate_egress_action_add(unit=%d, portgroup=%u, ovlan=%u, ivlan=%u, &action)",
               unit, portgroup, oVlanId, iVlanId);

  error = bcm_vlan_translate_egress_action_add(unit, portgroup, oVlanId, iVlanId, &action);

  if (error == BCM_E_EXISTS)
  {
    PT_LOG_WARN(LOG_CTX_HAPI, "Entry already exists: %d (\"%s\")", error, bcm_errmsg(error));
    return L7_ALREADY_CONFIGURED;
  }
  else if (error == BCM_E_FULL)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Translation resources exhausted: rv=%d (%s)", error, bcm_errmsg(error));
    return L7_NO_RESOURCES;
  }
  else if (error != BCM_E_NONE)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Error calling bcm_vlan_translate_egress_action_add function: %d (\"%s\")", error, bcm_errmsg(error));
    return L7_FAILURE;
  }

  /* Update resources availability */
  if (error == BCM_E_NONE)
  {
    if (resources_xlate_egress < FREE_RESOURCES_XLATE_EGRESS)
      resources_xlate_egress++;
  }

  PT_LOG_TRACE(LOG_CTX_HAPI, "Translation entry added successfully: newOuterVlan=%u [rc=%d]", newOVlanId, error);

  return L7_SUCCESS;
}

/**
 * Remove a single/double-vlan translation at the egress stage
 *  
 * @param unit : bcm_unit 
 * @param portgroup: port class id
 * @param oVlanId: outer vlan id 
 * @param iVlanId: inner vlan id (use 0 for single vlan) 
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_hapi_xlate_egress_action_delete(int unit, L7_uint32 portgroup, L7_uint16 oVlanId, L7_uint16 iVlanId)
{
  int error;

  PT_LOG_TRACE(LOG_CTX_HAPI,"unit=%u portgroup=%u oVlanId=%u iVlanId=%u",
               unit, portgroup, oVlanId, iVlanId);

  PT_LOG_TRACE(LOG_CTX_HAPI, "bcm_vlan_translate_egress_action_delete(unit=%u, portgroup=%u, ovlan=%u, ivlan=%u)",
               unit, portgroup, oVlanId, iVlanId);

  error = bcm_vlan_translate_egress_action_delete(unit, portgroup, oVlanId, iVlanId);

  if (error != BCM_E_NONE && error != BCM_E_NOT_FOUND )
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Error calling bcm_vlan_translate_egress_action_delete function: %d (\"%s\")", error, bcm_errmsg(error));
    return L7_FAILURE;
  }

  /* Update resources availability */
  if (resources_xlate_egress > 0)
    resources_xlate_egress--;

  PT_LOG_TRACE(LOG_CTX_HAPI, "Translation entry removed successfully [rc=%d]", error);

  return L7_SUCCESS;
}

/**
 * Remove all ingress translations
 *  
 * @param unit : bcm_unit 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_hapi_xlate_ingress_action_delete_all(int unit)
{
  int error;

  error = bcm_vlan_translate_delete_all(unit);

  if (error != BCM_E_NONE && error != BCM_E_NOT_FOUND )
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Error calling bcm_vlan_translate_delete_all function: %d (\"%s\")", error, bcm_errmsg(error));
    return L7_FAILURE;
  }

  resources_xlate_ingress = FREE_RESOURCES_XLATE_INGRESS;

  PT_LOG_TRACE(LOG_CTX_HAPI, "All ingress translations were removed [rc=%d]", error);

  /* Update resources availability */
  return L7_SUCCESS;
}

/**
 * Remove all egress translations
 *  
 * @param unit : bcm_unit 
 *  
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_hapi_xlate_egress_action_delete_all(int unit)
{
  int error;

  error = bcm_vlan_translate_egress_delete_all(unit);

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

  PT_LOG_TRACE(LOG_CTX_HAPI, "All egress translations were removed [rc=%d]", error);

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

  PT_LOG_TRACE(LOG_CTX_HAPI,"dapiPort={%d,%d,%d} oVlanId=%u newOVlanId=%u newIVlanId=%u",
            dapiPort->usp->unit, dapiPort->usp->slot, dapiPort->usp->port, oVlanId, newOVlanId, newIVlanId);

  /* Validate dapiPort */
  if (dapiPort->usp->unit<0 || dapiPort->usp->slot<0 || dapiPort->usp->port<0)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Invalid interface");
    return L7_FAILURE;
  }

  /* Get port pointers */
  DAPIPORT_GET_PTR(dapiPort, dapiPortPtr, hapiPortPtr);
  /* Accept only physical and lag interfaces */
  if ( !IS_PORT_TYPE_PHYSICAL(dapiPortPtr) && !IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr) )
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Port {%d,%d,%d} is not physical neither logical lag (usp=)",
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

  PT_LOG_TRACE(LOG_CTX_HAPI, "bcm_vlan_translate_action_add(unit=%u, port=%u, keyType=%u, ovlan=%u, ivlan=%u, &action)",
               hapiPortPtr->bcm_unit, hapiPortPtr->bcm_gport, keyType, oVlanId, 0);

  error = bcm_vlan_translate_action_add(hapiPortPtr->bcm_unit, hapiPortPtr->bcm_gport, keyType, oVlanId, 0, &action);

  if (error == BCM_E_EXISTS)
  {
    PT_LOG_WARN(LOG_CTX_HAPI, "Entry already exists: %d (\"%s\")", error, bcm_errmsg(error));
    return L7_ALREADY_CONFIGURED;
  }
  else if (error == BCM_E_FULL)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Translation resources exhausted: rv=%d (%s)", error, bcm_errmsg(error));
    return L7_NO_RESOURCES;
  }
  else if (error != BCM_E_NONE)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Error calling bcm_vlan_translate_action_add function: %d (\"%s\")", error, bcm_errmsg(error));
    return L7_FAILURE;
  }

  /* Update resources availability */
  if (error == BCM_E_NONE)
  {
    if (resources_xlate_ingress < FREE_RESOURCES_XLATE_INGRESS)
      resources_xlate_ingress++;
  }

  PT_LOG_TRACE(LOG_CTX_HAPI, "Translation entry added successfully: newOuterVlan=%u newInnerVlan=%u [rc=%d]",
               newOVlanId, newIVlanId, error);

  return L7_SUCCESS;
}

/**
 * Replace Outer VID and remove inner VID
 *  
 * @param unit : bcm_unit 
 * @param portgroup: port class id
 * @param oVlanId: outer vlan id 
 * @param iVlanId: inner vlan id (0 to not use inner vlan)
 * @param newOVlanId: new outer vlan id
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_hapi_xlate_egress_replaceOVid_deleteIVid(int unit, L7_uint32 portgroup, L7_uint16 oVlanId, L7_uint16 iVlanId, L7_uint16 newOVlanId)
{
  int error;
  bcm_vlan_action_set_t action;

  PT_LOG_TRACE(LOG_CTX_HAPI, "unit=%u portgroup=%u oVlanId=%u iVlanId=%u newOVlanId=%u",
               unit, portgroup, oVlanId, iVlanId, newOVlanId);

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

  PT_LOG_TRACE(LOG_CTX_HAPI, "bcm_vlan_translate_egress_action_add(unit=%u, port=%d, ovlan=%u, ivlan=%u, &action)",
               unit, portgroup, oVlanId, iVlanId);

  error = bcm_vlan_translate_egress_action_add(unit, portgroup, oVlanId, iVlanId, &action);

  if (error == BCM_E_EXISTS)
  {
    PT_LOG_WARN(LOG_CTX_HAPI, "Entry already exists: %d (\"%s\")", error, bcm_errmsg(error));
    return L7_ALREADY_CONFIGURED;
  }
  else if (error == BCM_E_FULL)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Translation resources exhausted: rv=%d (%s)", error, bcm_errmsg(error));
    return L7_NO_RESOURCES;
  }
  else if (error != BCM_E_NONE)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Error calling bcm_vlan_translate_egress_action_add function: %d (\"%s\")", error, bcm_errmsg(error));
    return L7_FAILURE;
  }

  /* Update resources availability */
  if (error == BCM_E_NONE)
  {
    if (resources_xlate_egress < FREE_RESOURCES_XLATE_EGRESS)
      resources_xlate_egress++;
  }

  PT_LOG_TRACE(LOG_CTX_HAPI, "Translation entry added successfully: newOuterVlan=%u [rc=%d]", newOVlanId, error);

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

  PT_LOG_TRACE(LOG_CTX_HAPI,"dapiPort={%d,%d,%d} oVlanId=%u iVlanId=%u",
            dapiPort->usp->unit, dapiPort->usp->slot, dapiPort->usp->port, xlate->outerVlanId, xlate->innerVlanId);

  /* Validate dapiPort */
  if (dapiPort->usp->unit<0 || dapiPort->usp->slot<0 || dapiPort->usp->port<0)
  {
    PT_LOG_ERR(LOG_CTX_HAPI,"Invalid interface");
    return L7_FAILURE;
  }

  /* Get port pointers */
  DAPIPORT_GET_PTR(dapiPort, dapiPortPtr, hapiPortPtr);
  /* Accept only physical and lag interfaces */
  if ( !IS_PORT_TYPE_PHYSICAL(dapiPortPtr) && !IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr) )
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Port {%d,%d,%d} is not physical neither logical lag",
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

  PT_LOG_TRACE(LOG_CTX_HAPI, "bcm_vlan_translate_action_get(unit=%u, port=%u, keyType=%u, ovlan=%u, ivlan=%u, &action)",
               hapiPortPtr->bcm_unit, hapiPortPtr->bcm_gport, keyType, xlate->outerVlanId, xlate->innerVlanId);

  error = bcm_vlan_translate_action_get(hapiPortPtr->bcm_unit, hapiPortPtr->bcm_gport, keyType, xlate->outerVlanId, xlate->innerVlanId, &action);

  if (error != BCM_E_NONE)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Error calling bcm_vlan_translate_action_get function: %d (\"%s\")", error, bcm_errmsg(error));
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

  PT_LOG_TRACE(LOG_CTX_HAPI, "Translation entry read successfully: newOVid=%u.%u (OAction=%u.%u), newIVid=%u.%u (IAction=%u.%u) [rc=%d]",
            xlate->newOuterVlanId , xlate->newOuterPrio,
            xlate->outerVlanAction, xlate->outerPrioAction,
            xlate->newInnerVlanId , xlate->newInnerPrio,
            xlate->innerVlanAction, xlate->innerPrioAction,
            error);

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

  PT_LOG_TRACE(LOG_CTX_HAPI,"dapiPort={%d,%d,%d} oVlanId=%u iVlanId=%u newOVlanId=%u (%u) newIVlanId=%u (%u)",
            dapiPort->usp->unit, dapiPort->usp->slot, dapiPort->usp->port,
            xlate->outerVlanId, xlate->innerVlanId,
            xlate->newOuterVlanId, xlate->outerVlanAction, xlate->newInnerVlanId, xlate->innerVlanAction);

  /* Validate dapiPort */
  if (dapiPort->usp->unit<0 || dapiPort->usp->slot<0 || dapiPort->usp->port<0)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Invalid interface");
    return L7_FAILURE;
  }

  /* Get port pointers */
  DAPIPORT_GET_PTR(dapiPort, dapiPortPtr, hapiPortPtr);
  /* Accept only physical and lag interfaces */
  if ( !IS_PORT_TYPE_PHYSICAL(dapiPortPtr) && !IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr) )
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Port {%d,%d,%d} is not physical neither logical lag (usp=)",
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
//    PT_LOG_ERR(LOG_CTX_HAPI, "Cannot use PTIN_XLATE_ACTION_ADD action for double tagged packets!");
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
  action.dt_outer      = xlate->outerVlanAction;
  action.dt_inner      = (xlate->innerVlanAction!=PTIN_XLATE_ACTION_ADD) ? xlate->innerVlanAction : bcmVlanActionNone;  /* Push not allowed to inner VLAN */
  action.dt_outer_prio      = xlate->outerPrioAction;
  action.dt_outer_pkt_prio  = xlate->outerPrioAction;
  action.dt_inner_prio      = xlate->innerPrioAction;
  action.dt_inner_pkt_prio  = xlate->innerPrioAction;

  action.ot_outer      = xlate->outerVlanAction;
  action.ot_inner      = (xlate->innerVlanAction==PTIN_XLATE_ACTION_ADD) ? xlate->innerVlanAction : bcmVlanActionNone;  /* Inner VLAN can only be added for single tagged packets */
  action.ot_outer_prio      = xlate->outerPrioAction;
  action.ot_outer_pkt_prio  = xlate->outerPrioAction;
  action.ot_inner_pkt_prio  = xlate->innerPrioAction;

  action.new_outer_vlan     = xlate->newOuterVlanId;
  action.new_inner_vlan     = xlate->newInnerVlanId;
  action.priority           = xlate->newOuterPrio;
  action.new_inner_pkt_prio = xlate->newInnerPrio;
  
  PT_LOG_TRACE(LOG_CTX_HAPI, "bcm_vlan_translate_action_add(unit=%u, gport=%u, keyType=%u, ovlan=%u, ivlan=%u, &action)",
               hapiPortPtr->bcm_unit, hapiPortPtr->bcm_gport, keyType, xlate->outerVlanId, xlate->innerVlanId);

  PT_LOG_TRACE(LOG_CTX_HAPI,"action.new_outer_vlan     = %u", action.new_outer_vlan);
  PT_LOG_TRACE(LOG_CTX_HAPI,"action.new_inner_vlan     = %u", action.new_inner_vlan);
  PT_LOG_TRACE(LOG_CTX_HAPI,"action.priority           = %u", action.priority);
  PT_LOG_TRACE(LOG_CTX_HAPI,"action.new_inner_pkt_prio = %u", action.new_inner_pkt_prio);

  PT_LOG_TRACE(LOG_CTX_HAPI,"action.dt_outer           = %u", action.dt_outer);
  PT_LOG_TRACE(LOG_CTX_HAPI,"action.dt_inner           = %u", action.dt_inner);
  PT_LOG_TRACE(LOG_CTX_HAPI,"action.dt_outer_prio      = %u", action.dt_outer_prio);
  PT_LOG_TRACE(LOG_CTX_HAPI,"action.dt_outer_pkt_prio  = %u", action.dt_outer_pkt_prio);
  PT_LOG_TRACE(LOG_CTX_HAPI,"action.dt_inner_prio      = %u", action.dt_inner_prio);
  PT_LOG_TRACE(LOG_CTX_HAPI,"action.dt_inner_pkt_prio  = %u", action.dt_inner_pkt_prio);
  PT_LOG_TRACE(LOG_CTX_HAPI,"action.ot_outer           = %u", action.ot_outer);
  PT_LOG_TRACE(LOG_CTX_HAPI,"action.ot_inner           = %u", action.ot_inner);
  PT_LOG_TRACE(LOG_CTX_HAPI,"action.ot_outer_prio      = %u", action.ot_outer_prio);
  PT_LOG_TRACE(LOG_CTX_HAPI,"action.ot_outer_pkt_prio  = %u", action.ot_outer_pkt_prio);
  PT_LOG_TRACE(LOG_CTX_HAPI,"action.ot_inner_pkt_prio  = %u", action.ot_inner_pkt_prio);
  PT_LOG_TRACE(LOG_CTX_HAPI,"action.it_outer           = %u", action.it_outer);
  PT_LOG_TRACE(LOG_CTX_HAPI,"action.it_inner           = %u", action.it_inner);
  PT_LOG_TRACE(LOG_CTX_HAPI,"action.it_outer_prio      = %u", action.it_outer_pkt_prio);
  PT_LOG_TRACE(LOG_CTX_HAPI,"action.it_inner_pkt_prio  = %u", action.it_inner_pkt_prio);
  PT_LOG_TRACE(LOG_CTX_HAPI,"action.ut_outer           = %u", action.ut_outer);
  PT_LOG_TRACE(LOG_CTX_HAPI,"action.ut_inner           = %u", action.ut_inner);
  PT_LOG_TRACE(LOG_CTX_HAPI,"action.ut_outer_prio      = %u", action.ut_outer_pkt_prio);
  PT_LOG_TRACE(LOG_CTX_HAPI,"action.ut_inner_pkt_prio  = %u", action.ut_inner_pkt_prio);

  error = bcm_vlan_translate_action_add(hapiPortPtr->bcm_unit, hapiPortPtr->bcm_gport, keyType, xlate->outerVlanId, xlate->innerVlanId, &action);

  if (error == BCM_E_EXISTS)
  {
    PT_LOG_WARN(LOG_CTX_HAPI, "Entry already exists: %d (\"%s\")", error, bcm_errmsg(error));
    return L7_ALREADY_CONFIGURED;
  }
  else if (error == BCM_E_FULL)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Translation resources exhausted: rv=%d (%s)", error, bcm_errmsg(error));
    return L7_NO_RESOURCES;
  }
  else if (error != BCM_E_NONE)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Error calling bcm_vlan_translate_action_add function (hapiPortPtr->bcm_port=%u): %d (\"%s\")", hapiPortPtr->bcm_port, error, bcm_errmsg(error));
    return L7_FAILURE;
  }

  /* Update resources availability */
  if (error == BCM_E_NONE)
  {
    if (resources_xlate_ingress < FREE_RESOURCES_XLATE_INGRESS)
      resources_xlate_ingress++;
  }

  PT_LOG_TRACE(LOG_CTX_HAPI, "Translation entry added successfully: newOVlanId=%u.%u (Oaction %u.%u) newIVlanId=%u.%u (Iaction %u.%u) [rc=%d]",
            xlate->newOuterVlanId , xlate->newOuterPrio,
            xlate->outerVlanAction, xlate->outerPrioAction,
            xlate->newInnerVlanId , xlate->newInnerPrio,
            xlate->innerVlanAction, xlate->innerPrioAction,
            error);

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

  PT_LOG_TRACE(LOG_CTX_HAPI, "dapiPort={%d,%d,%d} oVlanId=%u iVlanId=%u",
            dapiPort->usp->unit, dapiPort->usp->slot, dapiPort->usp->port, xlate->outerVlanId, xlate->innerVlanId);

  /* Validate dapiPort */
  if (dapiPort->usp->unit<0 || dapiPort->usp->slot<0 || dapiPort->usp->port<0)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Invalid interface");
    return L7_FAILURE;
  }

  /* Get port pointers */
  DAPIPORT_GET_PTR(dapiPort, dapiPortPtr, hapiPortPtr);
  /* Accept only physical and lag interfaces */
  if ( !IS_PORT_TYPE_PHYSICAL(dapiPortPtr) && !IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr) )
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Port {%d,%d,%d} is not physical neither logical lag",
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

  PT_LOG_TRACE(LOG_CTX_HAPI, "bcm_vlan_translate_action_delete(unit=%u, port=%u, keyType=%u, ovlan=%u, ivlan=%u)",
               hapiPortPtr->bcm_unit, hapiPortPtr->bcm_gport, keyType, xlate->outerVlanId, xlate->innerVlanId);

  error = bcm_vlan_translate_action_delete(hapiPortPtr->bcm_unit, hapiPortPtr->bcm_gport, keyType, xlate->outerVlanId, xlate->innerVlanId);

  if (error != BCM_E_NONE && error != BCM_E_NOT_FOUND )
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Error calling bcm_vlan_translate_action_delete function: %d (\"%s\")", error, bcm_errmsg(error));
    return L7_FAILURE;
  }

  /* Update resources availability */
  if ( error == BCM_E_NONE )
  {
    if (resources_xlate_ingress > 0 )
      resources_xlate_ingress--;
  }

  PT_LOG_TRACE(LOG_CTX_HAPI, "Translation entry removed successfully [rc=%d]", error);

  return L7_SUCCESS;
}

/**
 * Remove all ingress translations
 *  
 * @param unit : bcm_unit 
 *  
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_hapi_xlate_ingress_delete_all(int unit)
{
  int error;

  error = bcm_vlan_translate_delete_all(unit);

  if (error != BCM_E_NONE && error != BCM_E_NOT_FOUND )
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Error calling bcm_vlan_translate_delete_all function: %d (\"%s\")", error, bcm_errmsg(error));
    return L7_FAILURE;
  }

  resources_xlate_ingress = FREE_RESOURCES_XLATE_INGRESS;

  PT_LOG_TRACE(LOG_CTX_HAPI, "All ingress translations were removed [rc=%d]", error);

  /* Update resources availability */
  return L7_SUCCESS;
}

/**
 * Get egress translation configuration for a given class id, 
 * outer vlan and inner vlan 
 *  
 * @param unit : bcm_unit 
 * @param portgroup: port class id
 * @param xlate : vlans and actions
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_hapi_xlate_egress_get(int unit, L7_uint32 portgroup, ptin_hapi_xlate_t *xlate)
{
  int error;
  bcm_vlan_action_set_t action;

  PT_LOG_TRACE(LOG_CTX_HAPI, "unit=%u portgroup=%u oVlanId=%u iVlanId=%u",
               unit, portgroup, xlate->outerVlanId, xlate->innerVlanId);

  bcm_vlan_action_set_t_init(&action);

  PT_LOG_TRACE(LOG_CTX_HAPI, "bcm_vlan_translate_egress_action_add(unit=%u, port=%u, ovlan=%u, ivlan=%u, &action)",
               unit, portgroup, xlate->outerVlanId, xlate->innerVlanId);

  error = bcm_vlan_translate_egress_action_get(unit, portgroup, xlate->outerVlanId, xlate->innerVlanId, &action);

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

  PT_LOG_TRACE(LOG_CTX_HAPI, "Translation entry read successfully: newOuterVlan=%u.%u (Oaction %u.%u) newInnerVlan=%u.%u (Iaction %u.%u) [rc=%d]",
            xlate->newOuterVlanId , xlate->newOuterPrio,
            xlate->outerVlanAction, xlate->outerPrioAction,
            xlate->newInnerVlanId , xlate->newInnerPrio,
            xlate->innerVlanAction, xlate->innerPrioAction,
            error);

  return L7_SUCCESS;
}

/**
 * Translate single or double-tagged packets to another 
 * outer-vlan at the egress stage 
 *  
 * @param unit : bcm_unit 
 * @param portgroup: port class id
 * @param xlate : vlans and actions
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_hapi_xlate_egress_add(int unit, L7_uint32 portgroup, ptin_hapi_xlate_t *xlate)
{
  int error;
  bcm_vlan_action_set_t action;

  PT_LOG_TRACE(LOG_CTX_HAPI, "unit=%u portgroup=%u oVlanId=%u iVlanId=%u newOVlanId=%u(%u) newIVlanId=%u(%u)",
               unit, portgroup,
               xlate->outerVlanId,xlate->innerVlanId,
               xlate->newOuterVlanId,xlate->outerVlanAction,
               xlate->newInnerVlanId,xlate->innerVlanAction);

  /* Do not allow ADD operation for double-tagged packets */
//if (xlate->innerVlanId!=0 &&
//    (xlate->outerAction==PTIN_XLATE_ACTION_ADD || xlate->innerAction==PTIN_XLATE_ACTION_ADD))
//{
//  PT_LOG_ERR(LOG_CTX_HAPI, "Cannot use PTIN_XLATE_ACTION_ADD action for double tagged packets!");
//  return L7_FAILURE;
//}

#if 0
  /* Check if this entry already exists */
  if (bcm_vlan_translate_egress_action_get(unit, portgroup, xlate->outerVlanId, xlate->innerVlanId, &action) == BCM_E_NONE)
  {
    PT_LOG_WARN(LOG_CTX_HAPI, "This entry already exists");
    return L7_ALREADY_CONFIGURED;
  }
#endif

  /* Add translation entry */
  bcm_vlan_action_set_t_init(&action);
  action.dt_outer      = (xlate->outerVlanAction!=PTIN_XLATE_ACTION_ADD) ? xlate->outerVlanAction : bcmVlanActionNone;  /* Push not allowed for double tagged packets */
  action.dt_inner      = (xlate->innerVlanAction!=PTIN_XLATE_ACTION_ADD) ? xlate->innerVlanAction : bcmVlanActionNone;  /* Push not allowed for double tagged packets */
  action.dt_outer_prio      = xlate->outerPrioAction;
  action.dt_outer_pkt_prio  = xlate->outerPrioAction;
  action.dt_inner_prio      = xlate->innerPrioAction;
  action.dt_inner_pkt_prio  = xlate->innerPrioAction;

  action.ot_outer      = (xlate->outerVlanAction!=PTIN_XLATE_ACTION_ADD) ? xlate->outerVlanAction : bcmVlanActionNone;  /* Push not allowed for single tagged packets */
  action.ot_inner      = (xlate->innerVlanAction==PTIN_XLATE_ACTION_ADD) ? xlate->innerVlanAction : bcmVlanActionNone;  /* Inner VLAN can only be added for single tagged packets */
  action.ot_outer_prio      = xlate->outerPrioAction;
  action.ot_outer_pkt_prio  = xlate->outerPrioAction;
  action.ot_inner_pkt_prio  = xlate->innerPrioAction;

  action.new_outer_vlan     = xlate->newOuterVlanId;
  action.new_inner_vlan     = xlate->newInnerVlanId;
  action.priority           = xlate->newOuterPrio;
  action.new_inner_pkt_prio = xlate->newInnerPrio;

  PT_LOG_TRACE(LOG_CTX_HAPI, "bcm_vlan_translate_egress_action_add(unit=%u, portgroup=%u, %u.%u, %u.%u, &action)",
               unit, portgroup,
               xlate->outerVlanId, xlate->outerPrio,
               xlate->innerVlanId, xlate->innerPrio);

  PT_LOG_TRACE(LOG_CTX_HAPI,"action.new_outer_vlan     = %d", action.new_outer_vlan);
  PT_LOG_TRACE(LOG_CTX_HAPI,"action.new_inner_vlan     = %d", action.new_inner_vlan);
  PT_LOG_TRACE(LOG_CTX_HAPI,"action.priority           = %d", action.priority);
  PT_LOG_TRACE(LOG_CTX_HAPI,"action.new_inner_pkt_prio = %d", action.new_inner_pkt_prio);

  PT_LOG_TRACE(LOG_CTX_HAPI,"action.dt_outer           = %d", action.dt_outer);
  PT_LOG_TRACE(LOG_CTX_HAPI,"action.dt_inner           = %d", action.dt_inner);
  PT_LOG_TRACE(LOG_CTX_HAPI,"action.dt_outer_prio      = %d", action.dt_outer_prio);
  PT_LOG_TRACE(LOG_CTX_HAPI,"action.dt_outer_pkt_prio  = %d", action.dt_outer_pkt_prio);
  PT_LOG_TRACE(LOG_CTX_HAPI,"action.dt_inner_prio      = %d", action.dt_inner_prio);
  PT_LOG_TRACE(LOG_CTX_HAPI,"action.dt_inner_pkt_prio  = %d", action.dt_inner_pkt_prio);
  PT_LOG_TRACE(LOG_CTX_HAPI,"action.ot_outer           = %d", action.ot_outer);
  PT_LOG_TRACE(LOG_CTX_HAPI,"action.ot_inner           = %d", action.ot_inner);
  PT_LOG_TRACE(LOG_CTX_HAPI,"action.ot_outer_prio      = %d", action.ot_outer_prio);
  PT_LOG_TRACE(LOG_CTX_HAPI,"action.ot_outer_pkt_prio  = %d", action.ot_outer_pkt_prio);
  PT_LOG_TRACE(LOG_CTX_HAPI,"action.ot_inner_pkt_prio  = %d", action.ot_inner_pkt_prio);
  PT_LOG_TRACE(LOG_CTX_HAPI,"action.it_outer           = %u", action.it_outer);
  PT_LOG_TRACE(LOG_CTX_HAPI,"action.it_inner           = %u", action.it_inner);
  PT_LOG_TRACE(LOG_CTX_HAPI,"action.it_outer_prio      = %u", action.it_outer_pkt_prio);
  PT_LOG_TRACE(LOG_CTX_HAPI,"action.it_inner_pkt_prio  = %u", action.it_inner_pkt_prio);
  PT_LOG_TRACE(LOG_CTX_HAPI,"action.ut_outer           = %u", action.ut_outer);
  PT_LOG_TRACE(LOG_CTX_HAPI,"action.ut_inner           = %u", action.ut_inner);
  PT_LOG_TRACE(LOG_CTX_HAPI,"action.ut_outer_prio      = %u", action.ut_outer_pkt_prio);
  PT_LOG_TRACE(LOG_CTX_HAPI,"action.ut_inner_pkt_prio  = %u", action.ut_inner_pkt_prio);

  error = bcm_vlan_translate_egress_action_add(unit, portgroup, xlate->outerVlanId, xlate->innerVlanId, &action);

  if (error == BCM_E_EXISTS)
  {
    PT_LOG_WARN(LOG_CTX_HAPI, "Entry already exists: %d (\"%s\")", error, bcm_errmsg(error));
    return L7_ALREADY_CONFIGURED;
  }
  else if (error == BCM_E_FULL)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Translation resources exhausted: rv=%d (%s)", error, bcm_errmsg(error));
    return L7_NO_RESOURCES;
  }
  else if (error != BCM_E_NONE)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Error calling bcm_vlan_translate_egress_action_add function: %d (\"%s\")", error, bcm_errmsg(error));
    return L7_FAILURE;
  }

  /* Update resources availability */
  if (error == BCM_E_NONE)
  {
    if (resources_xlate_egress < FREE_RESOURCES_XLATE_EGRESS)
      resources_xlate_egress++;
  }

  PT_LOG_TRACE(LOG_CTX_HAPI, "Translation entry added successfully: newOVlanId=%u.%u (Oaction %u.%u) newIVlanId=%u.%u (Iaction %u.%u) [rc=%d]",
            xlate->newOuterVlanId , xlate->newOuterPrio,
            xlate->outerVlanAction, xlate->outerPrioAction,
            xlate->newInnerVlanId , xlate->newInnerPrio,
            xlate->innerVlanAction, xlate->innerPrioAction,
            error);

  return L7_SUCCESS;
}

/**
 * Remove a single/double-vlan translation at the egress stage
 *  
 * @param unit : bcm_unit 
 * @param portgroup: port class id
 * @param xlate : vlans and actions
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_hapi_xlate_egress_delete(int unit, L7_uint32 portgroup, ptin_hapi_xlate_t *xlate)
{
  int error;

  PT_LOG_TRACE(LOG_CTX_HAPI,"unit=%u portgroup=%u oVlanId=%u iVlanId=%u",
               unit, portgroup, xlate->outerVlanId, xlate->innerVlanId);

  PT_LOG_TRACE(LOG_CTX_HAPI, "bcm_vlan_translate_egress_action_delete(unit=%u, port=%u, ovlan=%u, ivlan=%u)",
               unit, portgroup, xlate->outerVlanId, xlate->innerVlanId);

  error = bcm_vlan_translate_egress_action_delete(unit, portgroup, xlate->outerVlanId, xlate->innerVlanId);

  if (error != BCM_E_NONE && error != BCM_E_NOT_FOUND )
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Error calling bcm_vlan_translate_egress_action_delete function: %d (\"%s\")", error, bcm_errmsg(error));
    return L7_FAILURE;
  }

  /* Update resources availability */
  if (resources_xlate_egress > 0)
    resources_xlate_egress--;

  PT_LOG_TRACE(LOG_CTX_HAPI, "Translation entry removed successfully [rc=%d]", error);

  return L7_SUCCESS;
}

/**
 * Remove all egress translations
 *  
 * @param unit : bcm_unit 
 *  
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_hapi_xlate_egress_delete_all(int unit)
{
  int error;

  error = bcm_vlan_translate_egress_delete_all(unit);

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

  PT_LOG_TRACE(LOG_CTX_HAPI, "All egress translations were removed [rc=%d]", error);

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
 * @param usp: port belonging to group 
 * @param dapi_g: System descriptor
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_hapi_xlate_egress_portsGroup_set(L7_uint32 portgroup, DAPI_USP_t *usp, DAPI_t *dapi_g)
{
  PT_LOG_TRACE(LOG_CTX_HAPI,"portgroup=%u usp={%d,%d,%d}",
               portgroup, usp->unit, usp->slot, usp->port);

  /* Change Ingress Vlan Translate Keys for all physical ports (LAGs are included through their physical ports) */
  /* Validate usp */
  if (usp->unit<0 || usp->slot<0 || usp->port<0)
  {
    PT_LOG_ERR(LOG_CTX_HAPI,"Invalid interface: {%d,%d,%d}", usp->unit, usp->slot, usp->port);
    return L7_FAILURE;
  }

  /* Get port pointers *of the group port */
  dapiPortPtr = DAPI_PORT_GET( usp, dapi_g );
  hapiPortPtr = HAPI_PORT_GET( usp, dapi_g );

  /* Accept only physical interfaces */
  if ( !IS_PORT_TYPE_PHYSICAL(dapiPortPtr) /*&& !IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr)*/ )
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Port {%d,%d,%d} is not physical", usp->unit, usp->slot, usp->port);
    return L7_FAILURE;
  }

  PT_LOG_TRACE(LOG_CTX_HAPI, "Setting class id %d to single port {%d,%d,%d}",
               portgroup, usp->unit, usp->slot, usp->port);
  if (bcm_port_class_set(hapiPortPtr->bcm_unit, hapiPortPtr->bcm_gport, bcmPortClassVlanTranslateEgress, (L7_uint32) portgroup ) != BCM_E_NONE)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Error setting class id %d to single port {%d,%d,%d} [VLAN XLATE]",
               portgroup, usp->unit, usp->slot, usp->port);
    return L7_FAILURE;
  }
  /* PTin removed: Let Fastpath to manage EFP Class ids */
  #if 0
  if (bcm_port_class_set(hapiPortPtr->bcm_unit, hapiPortPtr->bcm_gport, bcmPortClassFieldEgress, (L7_uint32) portgroup + EFP_STD_CLASS_ID_MAX) != BCM_E_NONE)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Error setting class id %d to single port {%d,%d,%d} [ECAP]",
               portgroup, usp->unit, usp->slot, usp->port);
    return L7_FAILURE;
  }
  #endif

  PT_LOG_TRACE(LOG_CTX_HAPI, "Class id %d successfully assigned to the given ports", portgroup);

  return L7_SUCCESS;
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

  /* Extract class id */
  if (bcm_port_class_get(hapiPortPtr->bcm_unit, hapiPortPtr->bcm_gport, bcmPortClassVlanTranslateEgress, &classId ) != BCM_E_NONE)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Error getting class id from port {%d,%d,%d}", usp->unit, usp->slot, usp->port);
    return L7_FAILURE;
  }

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
  L7_uint32     bcm_unit, bcm_port;
  DAPI_USP_t    usp;
  BROAD_PORT_t  *hapiPortPtr;
  extern DAPI_t *dapi_g;
  L7_RC_t rc = L7_SUCCESS;

  /* Validate dapi_g pointers */
  if (dapi_g == L7_NULLPTR)
  {
      PT_LOG_ERR(LOG_CTX_STARTUP, "dapi_g is not initialized");
      return L7_FAILURE;
  }

  /* Change Ingress Vlan Translate Keys for all physical ports (LAGs are included through their physical ports) */
  USP_PHYPORT_ITERATE(usp, dapi_g)
  {
    hapiPortPtr = HAPI_PORT_GET(&usp, dapi_g);
    bcm_port    = hapiPortPtr->bcm_port;
    bcm_unit    = hapiPortPtr->bcm_unit;

    /* Default class ids is port+1 */
    if (bcm_port_class_set(bcm_unit, bcm_port, bcmPortClassVlanTranslateEgress, usp.port+1 ) != BCM_E_NONE)
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "Error setting class id %u to port %d [VLAN XLATE]", usp.port+1, usp.port);
      rc = L7_FAILURE;
      continue;
    }
    /* PTin removed: Let Fastpath to manage EFP Class ids */
    #if 0
    if (bcm_port_class_set(bcm_unit, bcm_port, bcmPortClassFieldEgress, usp.port+1 ) != BCM_E_NONE)
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "Error setting class id %u to port %d [ECAP]", usp.port+1, usp.port);
      rc = L7_FAILURE;
      continue;
    }
    #endif
  }

  PT_LOG_TRACE(LOG_CTX_HAPI, "Class ids assigned: rc=%d", rc);

  return rc;
}

