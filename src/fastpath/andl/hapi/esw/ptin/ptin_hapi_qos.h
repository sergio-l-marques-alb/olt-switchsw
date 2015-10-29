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
