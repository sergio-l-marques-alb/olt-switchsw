#ifndef _PTIN_HAPI_QOS__H
#define _PTIN_HAPI_QOS__H

#include "ptin_hapi.h"

/**
 * Init QoS
 * 
 * @return L7_RC_t 
 */
extern L7_RC_t ptin_hapi_qos_init(void);

/**
 * Enable or disable Pbits remarking at egress port
 * 
 * @param dapiPort 
 * @param enable 
 * 
 * @return L7_RC_t 
 */
extern L7_RC_t ptin_hapi_qos_egress_pbits_remark(ptin_dapi_port_t *dapiPort, L7_BOOL enable);

/**
 * Add a QoS entry
 * 
 * @param dapiPort 
 * @param qos_cfg 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_hapi_qos_entry_add(ptin_dapi_port_t *dapiPort, ptin_dtl_qos_t *qos_cfg);

/**
 * Remove a QoS entry
 * 
 * @param qos_cfg 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_hapi_qos_entry_remove(ptin_dtl_qos_t *qos_cfg);

/**
 * Removes all entries belonging to a VLAN
 * 
 * @author mruas (10/27/2015)
 * 
 * @param ext_vlan 
 * @param int_vlan 
 * @param leaf_side : Root or leaf interfaces? 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_hapi_qos_vlan_remove(L7_uint16 ext_vlan, L7_uint16 int_vlan, L7_BOOL leaf_side);

/**
 * Flush several entries of the QoS table
 * 
 * @param dapiPort 
 * @param qos_cfg 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_hapi_qos_table_flush(ptin_dapi_port_t *dapiPort, ptin_dtl_qos_t *qos_cfg);

/**
 * Update a QoS entry
 * 
 * @param dapiPort 
 * @param qos_cfg 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_hapi_qos_entry_update(ptin_dapi_port_t *dapiPort, ptin_dtl_qos_t *qos_cfg);

#endif /* _PTIN_HAPI_QOS__H */
