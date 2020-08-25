/*~~~~~~~~~~~~~~~~~~~~~~~~~~COSQ Applications~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* $Id: 
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$ 
 *  
 * File: cint_rate_class_thresholds.c
 * Purpose: Example shows setup configuration of VOQ rate class thresholds
 *
 * Main function: cint_dnx_rate_class_thresholds_config - configures all thresholds of the rate class
 * 
 * Helper functions: 
 * * cint_dnx_tail_drop_and_guaranteed_thresholds_config - configures Tail Drop and Guaranteed thresholds
 * * cint_dnx_wred_and_ecn_thresholds_config -  configures WRED and ECN thresholds
 *  
 * Uses the following cints:
 *  cint cint_dram_bound_thresh.c
 */

/**
 * \brief - configure Tail Drop and Guaranteed thresholds for VOQ rate class
 *
 */
int cint_dnx_tail_drop_and_guaranteed_thresholds_config(int unit, bcm_gport_t rate_class_gport, bcm_cos_queue_t cosq)
{
    int result = 0;
    bcm_cosq_gport_size_t tail_drop_and_guaranteed_params;
    int dp;

    /*
     * ----------------------------------------
     * Configure parameters for Bytes 
     * ----------------------------------------
     */

    /** guaranteed */
    tail_drop_and_guaranteed_params.size_min = 0;

    /** FADT drop */
    tail_drop_and_guaranteed_params.size_max = 64*1024;
    tail_drop_and_guaranteed_params.size_fadt_min = 64*1024;
    tail_drop_and_guaranteed_params.size_alpha_max = 0;

    for (dp = 0; dp < 4; dp++)
    {
        result = bcm_cosq_gport_color_size_set(unit, rate_class_gport, cosq, dp, BCM_COSQ_GPORT_SIZE_BYTES, &tail_drop_and_guaranteed_params);
        if (BCM_E_NONE != result) {
            printf("bcm_cosq_gport_color_size_set failed \n"); 
            return result;
        }
    }

    /*
     * ----------------------------------------
     * Configure parameters for OCB Buffes
     * ----------------------------------------
     */

    /** guaranteed */
    tail_drop_and_guaranteed_params.size_min = 0;

    /** FADT drop */
    tail_drop_and_guaranteed_params.size_max = 8 * 1024;
    tail_drop_and_guaranteed_params.size_fadt_min = 8 * 1024;
    tail_drop_and_guaranteed_params.size_alpha_max = 0;

    for (dp = 0; dp < 4; dp++)
    {
        result = bcm_cosq_gport_color_size_set(unit, rate_class_gport, cosq, dp, BCM_COSQ_GPORT_SIZE_BUFFERS | BCM_COSQ_GPORT_SIZE_OCB, &tail_drop_and_guaranteed_params);
        if (BCM_E_NONE != result) {
            printf("bcm_cosq_gport_color_size_set failed \n"); 
            return result;
        }
    }

    /*
     * ----------------------------------------
     * Configure parameters for OCB Packet Descriptors
     * ----------------------------------------
     */

    /** guaranteed */
    tail_drop_and_guaranteed_params.size_min = 0;

    /** FADT drop */
    tail_drop_and_guaranteed_params.size_max = 8 * 1024;
    tail_drop_and_guaranteed_params.size_fadt_min = 8 * 1024;
    tail_drop_and_guaranteed_params.size_alpha_max = 0;

    for (dp = 0; dp < 4; dp++)
    {
        result = bcm_cosq_gport_color_size_set(unit, rate_class_gport, cosq, dp, BCM_COSQ_GPORT_SIZE_PACKET_DESC | BCM_COSQ_GPORT_SIZE_OCB, &tail_drop_and_guaranteed_params);
        if (BCM_E_NONE != result) {
            printf("bcm_cosq_gport_color_size_set failed \n"); 
            return result;
        }
    }

    return result;
}

/**
 * \brief - configure WRED and ECN thresholds for VOQ rate class
 *
 */
int cint_dnx_wred_and_ecn_thresholds_config(int unit, bcm_gport_t rate_class_gport, bcm_cos_queue_t cosq)
{
    int result = 0;
    bcm_cosq_gport_discard_t discard_params;
    int dp;
    uint32 color_flags[4] = {BCM_COSQ_DISCARD_COLOR_GREEN, BCM_COSQ_DISCARD_COLOR_YELLOW, BCM_COSQ_DISCARD_COLOR_RED, BCM_COSQ_DISCARD_COLOR_BLACK};

    /*
     * ----------------------------------------
     * Configure WRED parameters -- Bytes
     * ----------------------------------------
     */
    discard_params.min_thresh = 10*1024;
    discard_params.max_thresh = 50*1024;
    discard_params.drop_probability = 30;
    discard_params.gain = 4; /** gain should be the same for all colors and ECN */

    for (dp = 0; dp < 4; dp++)
    {
        discard_params.flags = BCM_COSQ_DISCARD_ENABLE | BCM_COSQ_DISCARD_BYTES | color_flags[dp];
        result = bcm_cosq_gport_discard_set(unit, rate_class_gport, cosq, &discard_params);
        if (BCM_E_NONE != result) {
            printf("bcm_cosq_gport_discard_set failed \n"); 
            return result;
        }

    }
    
    /*
     * ----------------------------------------
     * Configure ECN parameters -- Bytes
     * ----------------------------------------
     */
    discard_params.min_thresh = 10*1024;
    discard_params.max_thresh = 25*1024;
    discard_params.drop_probability = 30;
    discard_params.gain = 4; /** gain should be the same for all colors and ECN */
    discard_params.ecn_thresh = 50*1024;
    discard_params.flags = BCM_COSQ_DISCARD_MARK_CONGESTION | BCM_COSQ_DISCARD_ENABLE | BCM_COSQ_DISCARD_BYTES;
    result = bcm_cosq_gport_discard_set(unit, rate_class_gport, cosq, &discard_params);
    if (BCM_E_NONE != result) {
        printf("bcm_cosq_gport_discard_set failed \n"); 
        return result;
    }

    return result;
}

/**
 * \brief - configure VOQ rate class thresholds 
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit ID
 *   \param [in] rate_class_gport - gport to be configured
 *   \param [in] cosq - COS queue index -- not in use with rate class gport - must be 0
 * \par DIRECT OUTPUT:
 *   shr_error_e 
 */
int cint_dnx_rate_class_thresholds_config(int unit, bcm_gport_t rate_class_gport, bcm_cos_queue_t cosq)
{
    int result = BCM_E_NONE;

    /** Configure Guaranteed and tail drop */
    result = cint_dnx_tail_drop_and_guaranteed_thresholds_config(unit, rate_class_gport, cosq);
    if (BCM_E_NONE != result) {
        printf("cint_dnx_tail_drop_and_guaranteed_thresholds_config failed \n"); 
        return result;
    }

    /** Configure WRED and ECN */
    result = cint_dnx_wred_and_ecn_thresholds_config(unit, rate_class_gport, cosq);
    if (BCM_E_NONE != result) {
        printf("cint_dnx_wred_and_ecn_thresholds_config failed \n"); 
        return result;
    }

    /** Configure DRAM bound */
    result = cint_dnx_dram_bound_thresholds_config(unit, rate_class_gport, cosq, PORT_SPEED_100G);
    if (BCM_E_NONE != result) {
        printf("cint_dnx_dram_bound_thresholds_config failed \n"); 
        return result;
    }

    /** Configure OCB only */
    result = bcm_cosq_control_set(unit, rate_class_gport, cosq, bcmCosqControlOCBOnly, 0);
    if (BCM_E_NONE != result) {
        printf("bcm_cosq_control_set failed \n"); 
        return result;
    }

    return result;
}
