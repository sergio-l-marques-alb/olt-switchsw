#ifndef _PTIN_HAPI_XLATE__H
#define _PTIN_HAPI_XLATE__H

#include "ptin_hapi.h"

typedef struct {
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
extern L7_RC_t ptin_hapi_xlate_singletag_action_add(ptin_dapi_port_t *dapiPort, L7_uint16 oVlanId, L7_uint16 newOVlanId);

/**
 * Remove the outer-vlan translation (ingress stage)
 * 
 * @param dapiPort: port reference (physical or LAG)
 * @param oVlanId: outer vlan id
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_hapi_xlate_singletag_action_delete(ptin_dapi_port_t *dapiPort, L7_uint16 oVlanId);

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
extern L7_RC_t ptin_hapi_xlate_doubletag_action_add(ptin_dapi_port_t *dapiPort, L7_uint16 oVlanId, L7_uint16 iVlanId, L7_uint16 newOVlanId);

/**
 * Remove a double-vlan translation (ingress stage)
 * 
 * @param dapiPort: port reference (physical or LAG)
 * @param oVlanId: outer vlan id 
 * @param iVlanId: inner vlan id 
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_hapi_xlate_doubletag_action_delete(ptin_dapi_port_t *dapiPort, L7_uint16 oVlanId, L7_uint16 iVlanId);



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
extern L7_RC_t ptin_hapi_xlate_ingress_action_get(ptin_dapi_port_t *dapiPort, L7_uint16 oVlanId, L7_uint16 iVlanId, L7_uint16 *newOVlanId);

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
extern L7_RC_t ptin_hapi_xlate_ingress_action_add(ptin_dapi_port_t *dapiPort, L7_uint16 oVlanId, L7_uint16 iVlanId, L7_uint16 newOVlanId);

/**
 * Remove a translation (ingress stage)
 * 
 * @param dapiPort: port reference (physical or LAG)
 * @param oVlanId: outer vlanId 
 * @param iVlanId: inner vlanId (0 for single-vlan translations)
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_hapi_xlate_ingress_action_delete(ptin_dapi_port_t *dapiPort, L7_uint16 oVlanId, L7_uint16 iVlanId);

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
extern L7_RC_t ptin_hapi_xlate_egress_action_get(int unit, L7_uint32 portgroup, L7_uint16 oVlanId, L7_uint16 iVlanId, L7_uint16 *newOVlanId);

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
extern L7_RC_t ptin_hapi_xlate_egress_action_add(int unit, L7_uint32 portgroup, L7_uint16 oVlanId, L7_uint16 iVlanId, L7_uint16 newOVlanId);

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
extern L7_RC_t ptin_hapi_xlate_egress_action_delete(int unit, L7_uint32 portgroup, L7_uint16 oVlanId, L7_uint16 iVlanId);

/**
 * Remove all ingress translations
 *  
 * @param unit : bcm_unit 
 *  
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_hapi_xlate_ingress_action_delete_all(int unit);

/**
 * Remove all egress translations
 *  
 * @param unit : bcm_unit 
 *  
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_hapi_xlate_egress_action_delete_all(int unit);



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
extern L7_RC_t ptin_hapi_xlate_ingress_replaceOVid_addIVid(ptin_dapi_port_t *dapiPort, L7_uint16 oVlanId, L7_uint16 newOVlanId, L7_uint16 newIVlanId);

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
extern L7_RC_t ptin_hapi_xlate_egress_replaceOVid_deleteIVid(int unit, L7_uint32 portgroup, L7_uint16 oVlanId, L7_uint16 iVlanId, L7_uint16 newOVlanId);



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
 * @param unit : bcm_unit
 *  
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_hapi_xlate_ingress_delete_all(int unit);

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
extern L7_RC_t ptin_hapi_xlate_egress_get(int unit, L7_uint32 portgroup, ptin_hapi_xlate_t *xlate);

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
extern L7_RC_t ptin_hapi_xlate_egress_add(int unit, L7_uint32 portgroup, ptin_hapi_xlate_t *xlate);

/**
 * Remove a single/double-vlan translation at the egress stage
 *  
 * @param unit : bcm_unit 
 * @param portgroup: port class id
 * @param xlate : vlans and actions
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_hapi_xlate_egress_delete(int unit, L7_uint32 portgroup, ptin_hapi_xlate_t *xlate);

/**
 * Remove all egress translations
 *  
 * @param unit : bcm_unit 
 *  
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_hapi_xlate_egress_delete_all(int unit);


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
extern L7_RC_t ptin_hapi_xlate_egress_portsGroup_set(L7_uint32 portgroup, DAPI_USP_t *usp, DAPI_t *dapi_g);

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
