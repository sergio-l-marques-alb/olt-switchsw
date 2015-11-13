#ifndef _PTIN_HAPI_XLATE__H
#define _PTIN_HAPI_XLATE__H

#include "ptin_hapi.h"

typedef struct 
{
  L7_uint32 lif_id;           /* Logical interface */
  L7_uint16 outerVlanId;
  L7_uint16 innerVlanId;
  L7_uint16 newOuterVlanId;
  L7_uint16 newInnerVlanId;
  ptin_vlanXlate_action_enum outerVlanAction;
  ptin_vlanXlate_action_enum innerVlanAction;

  L7_uint8  outerPrio;
  L7_uint8  innerPrio;
  L7_uint8  newOuterPrio;
  L7_uint8  newInnerPrio;
  ptin_vlanXlate_action_enum outerPrioAction;
  ptin_vlanXlate_action_enum innerPrioAction;
} ptin_hapi_xlate_t;

/* Advanced translations active? */
extern L7_uint32 advanced_vlan_editing;

/**
 * Initialize translations for XGS4 switches
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_hapi_xlate_init(void);

/**
 * Get available resources for translation
 * 
 * @param ingress: pointer to variable where will contain the 
 *               number of free ingress translations.
 * @param egress: pointer to variable where will contain the 
 *               number of free egress translations.
 * @return L7_RC_t: always L7_SUCCESS
 */
extern L7_RC_t ptin_hapi_xlate_free_resources(L7_uint16 *ingress, L7_uint16 *egress);


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
extern L7_RC_t ptin_hapi_xlate_ingress_get(ptin_dapi_port_t *dapiPort, ptin_hapi_xlate_t *xlate);

/**
 * Translate packets (outer+inner vlan) to another outer-vlan at
 * the ingress stage 
 * 
 * @param dapiPort: port reference (physical or LAG)
 * @param xlate : vlans and actions
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_hapi_xlate_ingress_add(ptin_dapi_port_t *dapiPort, ptin_hapi_xlate_t *xlate);

/**
 * Remove a translation (ingress stage)
 * 
 * @param dapiPort: port reference (physical or LAG)
 * @param xlate : vlans and actions
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_hapi_xlate_ingress_delete(ptin_dapi_port_t *dapiPort, ptin_hapi_xlate_t *xlate);

/**
 * Remove all ingress translations
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_hapi_xlate_ingress_delete_all(void);

/**
 * Get egress translation configuration for a given class id, 
 * outer vlan and inner vlan 
 * 
 * @param portgroup: port class id
 * @param xlate : vlans and actions
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_hapi_xlate_egress_get(L7_uint32 portgroup, ptin_hapi_xlate_t *xlate);

/**
 * Translate single or double-tagged packets to another 
 * outer-vlan at the egress stage 
 * 
 * @param portgroup: port class id
 * @param xlate : vlans and actions
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_hapi_xlate_egress_add(ptin_dapi_port_t *dapiPort, ptin_hapi_xlate_t *xlate);

/**
 * Remove a single/double-vlan translation at the egress stage
 * 
 * @param portgroup: port class id
 * @param xlate : vlans and actions
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_hapi_xlate_egress_delete(ptin_dapi_port_t *dapiPort, ptin_hapi_xlate_t *xlate);

/**
 * Remove all egress translations
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_hapi_xlate_egress_delete_all(void);


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
extern L7_RC_t ptin_hapi_xlate_egress_portsGroup_set(L7_uint32 portgroup, DAPI_USP_t *usp_list[], L7_uint8 usp_list_size, DAPI_t *dapi_g);

/**
 * Get the class id attributed to a specific port
 * 
 * @param portgroup: Class id returned
 * @param usp: Port reference
 * @param dapi_g: System descriptor
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_hapi_xlate_egress_portsGroup_get(L7_uint32 *portgroup, DAPI_USP_t *usp, DAPI_t *dapi_g);

#endif /* _PTIN_HAPI_XLATE__H */
