#ifndef _PTIN_QOS__H
#define _PTIN_QOS__H

#include "datatypes.h"
#include "ptin_structs.h"


/**
 * Initialize QoS module
 * 
 * @author mruas (06/01/21)
 * 
 * @return L7_RC_t 
 */
extern void ptin_qos_init(void);

/**
 * Apply default QoS configurations to provided interface
 * 
 * @param ptin_port : interface
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_qos_intf_default(L7_uint32 ptin_port);

/**
 * Apply a policer for interface/CoS
 * 
 * @author mruas (4/2/2015)
 * 
 * @param ptin_port
 * @param cos 
 * @param meter 
 * 
 * @return L7_RC_t 
 */
extern 
L7_RC_t ptin_qos_cos_policer_set(L7_uint32 ptin_port, L7_uint8 cos, ptin_bw_meter_t *meter);

/**
 * Remove a policer for interface/CoS
 * 
 * @author mruas (4/2/2015)
 * 
 * @param ptin_port
 * @param cos 
 * 
 * @return L7_RC_t 
 */
extern 
L7_RC_t ptin_qos_cos_policer_clear(L7_uint32 ptin_port, L7_uint8 cos);

/**
 * Configures priority mapping to classes of services 
 * 
 * @param ptin_port : interface
 * @param intfQos : priority map
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_qos_intf_config_set(L7_uint32 ptin_port, ptin_QoS_intf_t *intfQos);

/**
 * Read interface properties for QoS
 * 
 * @param ptin_port : interface
 * @param intfQos : interface configuration
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_qos_intf_config_get(L7_uint32 ptin_port, ptin_QoS_intf_t *intfQos);

/**
 * Configures a class of service
 * 
 * @param ptin_port : interface 
 * @param cos : Class of Service id
 * @param qosConf: configuration
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_qos_cos_config_set(L7_uint32 ptin_port, L7_uint8 cos, ptin_QoS_cos_t *qosConf);

/**
 * Reads a class of service QoS configuration
 * 
 * @param ptin_port : interface 
 * @param cos : Class of Service id
 * @param qosConf: configuration
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_qos_cos_config_get(L7_uint32 ptin_port, L7_uint8 cos, ptin_QoS_cos_t *qosConf);

/**
 * Configures a class of service for QoS
 * 
 * @param ptin_port : interface 
 * @param cos : Class of Service id
 * @param qosConf: configuration
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_qos_drop_config_set(L7_uint32 ptin_port, L7_uint8 cos, ptin_QoS_drop_t *qosConf);

/**
 * Reads a class of service QoS configuration
 * 
 * @param ptin_port : interface 
 * @param cos : Class of Service id
 * @param qosConf: configuration
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_qos_drop_config_get(L7_uint32 ptin_port, L7_uint8 cos, ptin_QoS_drop_t *qosConf);

/**
 * Set the maximum rate for a port
 * 
 * @author mruas (16/08/17)
 * 
 * @param ptin_port
 * @param max_rate : Percentage
 * 
 * @return L7_RC_t 
 */
extern L7_RC_t ptin_qos_intf_shaper_set(L7_uint32 ptin_port, L7_uint32 max_rate, L7_uint32 burst_size);

#endif /* #define _PTIN_QOS__H */
