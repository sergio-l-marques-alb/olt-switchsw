/*~~~~~~~~~~~~~~~~~~~~~~~~~~COSQ Applications~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* $Id: 
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$ 
 *  
 * File: 
 * Purpose: Example shows setup configuration of VOQ and its attributes
 *  
 *  Main function: cint_dnx_queue_attributes_config
 * 
 * Uses the following cints:
 *  cint cint_dram_bound_thresh.c
 *  cint cint_rate_class_thresholds.c
 */

/**
 * \brief - configure queue attributes
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit ID
 *   \param [in] queue_gport - queue gport to be configured
 *   \param [in] numq - number of queues in queue bundle
 * \par DIRECT OUTPUT:
 *   shr_error_e 
 */
int cint_dnx_queue_attributes_config(int unit, bcm_gport_t queue_gport, int numq)
{
    bcm_gport_t rate_class_gport;
    bcm_switch_profile_mapping_t profile_mapping;
    int cosq;
    int result;


    int rate_class_id = 1; /** from 1 to 63. Rate class zero keeps SDK default values */

    BCM_GPORT_PROFILE_SET(rate_class_gport, rate_class_id);
    
    /** Configure rate class parameters */
    result = cint_dnx_rate_class_thresholds_config(unit, rate_class_gport, 0);
    if (BCM_E_NONE != result) {
        printf("cint_dnx_rate_class_thresholds_config failed \n"); 
        return result;
    }

    /** assign queues to the rate class */
    profile_mapping.mapped_profile = rate_class_gport;
    profile_mapping.profile_type = bcmCosqIngressQueueToRateClass; 
    for (cosq = 0; cosq < numq; cosq++)
    {
        result = bcm_cosq_profile_mapping_set(unit, queue_gport, cosq, 0, profile_mapping);
        if (BCM_E_NONE != result) {
            printf("bcm_cosq_profile_mapping_set failed \n"); 
            return result;
        }
    }

    return result;
}
