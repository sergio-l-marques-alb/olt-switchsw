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
 * Shaper max rate and burst configuration
 * 
 * @author mruas (06/01/21)
 * 
 * @param dapiPort 
 * @param queueSet : l7_cosq_set_t
 * @param tc : Traffic class
 * @param rate_min 
 * @param rate_max 
 * @param burst_size 
 * 
 * @return L7_RC_t 
 */
L7_RC_t 
ptin_hapi_qos_shaper_set(ptin_dapi_port_t *dapiPort, l7_cosq_set_t queueSet, L7_int tc,
                         L7_uint32 rate_min, L7_uint32 rate_max, L7_uint32 burst_size);


/**
 * Shaper max rate and burst configuration
 * 
 * @author mruas (06/01/21)
 * 
 * @param dapiPort 
 * @param queueSet : l7_cosq_set_t
 * @param tc : Traffic class
 * @param rate_min 
 * @param rate_max 
 * @param burst_size 
 * 
 * @return L7_RC_t 
 */
L7_RC_t 
ptin_hapi_qos_shaper_get(ptin_dapi_port_t *dapiPort, l7_cosq_set_t queueSet, L7_int tc,
                         L7_uint32 *rate_min, L7_uint32 *rate_max, L7_uint32 *burst_size);

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

/**
 * 
 * @param usp_port 
 *  Pointer to ptin_dapi_port_t (that gives us "usp_port" and L0
 *  SE for boards other than TC16SXG
 * @param queue_set 
 *  Parameter integrating scheduling coordinates L0, L1.x
 * @param TC 
 *  Traffic Class
 * @param gport 
 *  Pointer to function output gport we're getting
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_hapi_qos_gport_get(ptin_dapi_port_t *dapiPort,
                                int queue_set, L7_uint32 TC,
                                bcm_gport_t *gport);
#endif /* _PTIN_HAPI_QOS__H */
