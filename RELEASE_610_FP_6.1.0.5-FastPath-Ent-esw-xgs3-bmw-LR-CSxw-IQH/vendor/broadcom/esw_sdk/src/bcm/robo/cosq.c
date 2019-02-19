/*
 * $Id: cosq.c,v 1.1 2011/04/18 17:11:03 mruas Exp $
 * $Copyright: Copyright 2009 Broadcom Corporation.
 * This program is the proprietary software of Broadcom Corporation
 * and/or its licensors, and may only be used, duplicated, modified
 * or distributed pursuant to the terms and conditions of a separate,
 * written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized
 * License, Broadcom grants no license (express or implied), right
 * to use, or waiver of any kind with respect to the Software, and
 * Broadcom expressly reserves all rights in and to the Software
 * and all intellectual property rights therein.  IF YOU HAVE
 * NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
 * IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
 * ALL USE OF THE SOFTWARE.  
 *  
 * Except as expressly set forth in the Authorized License,
 *  
 * 1.     This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use
 * all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of
 * Broadcom integrated circuit products.
 *  
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
 * PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
 * OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 * 
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 * BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
 * INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
 * ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
 * TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
 * THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR USD 1.00,
 * WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
 * ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
 *
 * COS Queue Management
 * Purpose: API to set different cosq, priorities, and scheduler registers.
 */

#include <bcm/error.h>
#include <bcm/types.h>
#include <bcm/cosq.h>
#include <bcm/eav.h>

#include <sal/types.h>
#include <sal/appl/io.h>

#include <shared/pbmp.h>
#include <shared/types.h>

#include <soc/types.h>
#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/debug.h>

/*
 * Function:
 *      bcm_robo_cosq_detach
 * Purpose:
 *      Discard all COS schedule/mapping state.
 * Parameters:
 *      unit - RoboSwitch unit number.
 * Returns:
 *      BCM_E_XXX
 * Note:
 *      This API is designed mainly for Tucana Chip and not suitable
 *      for Robo Chip.
 */
int 
bcm_robo_cosq_detach(int unit)
{
    soc_cm_debug(DK_VERBOSE, 
                "BCM API : bcm_robo_cosq_detach()..\n");
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_robo_cosq_init
 * Purpose:
 *      Initialize (clear) all COS schedule/mapping state.
 * Parameters:
 *      unit - RoboSwitch unit number.
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_robo_cosq_init(int unit)
{  
    bcm_cos_t   prio;
    int         num_cos;
    int         map_queue = 0;
    pbmp_t      t_pbm;

    soc_cm_debug(DK_VERBOSE, 
                "BCM API : bcm_robo_cosq_init()..\n");
    /* 
     *  Setting default queue number 
     */
    num_cos = soc_property_get(unit, spn_BCM_NUM_COS, BCM_COS_DEFAULT);

    if (num_cos < 1) {
        num_cos = 1;
    } else if (num_cos > NUM_COS(unit)) {
        num_cos = NUM_COS(unit);
    }

    BCM_IF_ERROR_RETURN(bcm_cosq_config_set(unit, num_cos));

    /* Qos 1P Enable */
    BCM_PBMP_CLEAR(t_pbm); 
    BCM_PBMP_ASSIGN(t_pbm, PBMP_ALL(unit));
    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->queue_mapping_type_set)
                    (unit, t_pbm, 
                    DRV_QUEUE_MAP_PRIO, TRUE));
    /* 
     *  Setting default priority mapping 
     */
    for (prio = 0; prio <= BCM_PRIO_MAX; prio++) {
        map_queue = prio / ((int)(BCM_COS_COUNT/num_cos));
        BCM_IF_ERROR_RETURN(bcm_cosq_mapping_set(unit, prio, map_queue));
    }

    /* 
     * Setting default Priority Threshold and the queuing mode
     */
    /* this section is not implemented in Robo5324/5338 */
    
    /* Setting default mapping of reason code and cosq, this feature so far 
     * is supported on bcm5395/53115 only
     */
    if (SOC_IS_ROBO5395(unit) || SOC_IS_ROBO53115(unit)) {
        /* default mapping on reason_code will be :
         *  1. Mirroring, Default: 0
         *  2. SA Learning, Default: 0
         *  3. Switching /Flooding, Default: 1
         *  4. Protocol Termination, Default: 3
         *  5. Protocol Snooping, Default: 2
         *  6. Exception Processing, Default: 2 
         */
        SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->queue_rx_reason_set)
                (unit, DRV_RX_REASON_MIRRORING, 0));
        SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->queue_rx_reason_set)
                (unit, DRV_RX_REASON_SA_LEARNING, 0));
        SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->queue_rx_reason_set)
                (unit, DRV_RX_REASON_SWITCHING, 1));
        SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->queue_rx_reason_set)
                (unit, DRV_RX_REASON_MIRRORING, 3));
        SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->queue_rx_reason_set)
                (unit, DRV_RX_REASON_PROTO_SNOOP, 2));
        SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->queue_rx_reason_set)
                (unit, DRV_RX_REASON_EXCEPTION, 2));
    }
    
    return BCM_E_NONE;

}

/*
 * Function:
 *      bcm_robo_cosq_config_set
 * Purpose:
 *      Set the number of COS queues
 * Parameters:
 *      unit - RoboSwitch unit number.
 *      numq - number of COS queues (2, 3, or 4).
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_robo_cosq_config_set(int unit, bcm_cos_queue_t numq)
{   
    uint8   drv_value;
    
    soc_cm_debug(DK_VERBOSE, 
                "BCM API : bcm_robo_cosq_config_set()..\n");
    drv_value = numq;
    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->queue_count_set)
                    (unit, 0, drv_value));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_robo_cosq_config_get
 * Purpose:
 *      Get the number of cos queues
 * Parameters:
 *      unit - RoboSwitch unit number.
 *      numq - (Output) number of cosq
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_robo_cosq_config_get(int unit, bcm_cos_queue_t *numq)
{

    uint8   drv_value;
    
    soc_cm_debug(DK_VERBOSE, 
                "BCM API : bcm_robo_cosq_config_get()..\n");
    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->queue_count_get)
                    (unit, 0, &drv_value));
                    
    *numq = (bcm_cos_queue_t)drv_value;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_robo_cosq_mapping_set
 * Purpose:
 *      Set which cosq a given priority should fall into
 * Parameters:
 *      unit - RoboSwitch unit number.
 *      priority - Priority value to map
 *      cosq - COS queue to map to
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_robo_cosq_mapping_set(int unit, bcm_cos_t priority, bcm_cos_queue_t cosq)
{
    soc_cm_debug(DK_VERBOSE, 
                "BCM API : bcm_robo_cosq_mapping_set()..\n");
    if (!BCM_COSQ_QUEUE_VALID(unit, cosq)) {
        return (BCM_E_PARAM);
    }
    
    if (priority < 0 || priority >= BCM_COS_COUNT ||
                    cosq < 0 || cosq >= NUM_COS(unit)) {
        return (BCM_E_PARAM);
    }
    
    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->queue_prio_set)
                (unit, -1, priority, cosq));

    return (BCM_E_NONE);
}
                
/*
 * Function:
 *      bcm_robo_cosq_mapping_get
 * Purpose:
 *      Determine which COS queue a given priority currently maps to.
 * Parameters:
 *      unit - RoboSwitch unit number.
 *      priority - Priority value
 *      cosq - (Output) COS queue number
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_robo_cosq_mapping_get(int unit, bcm_cos_t priority, bcm_cos_queue_t *cosq)
{
    uint8       t_cosq = 0;
    
    soc_cm_debug(DK_VERBOSE, 
                "BCM API : bcm_robo_cosq_mapping_get()..\n");
    if (priority < 0 || priority > 7 ) {
        return (BCM_E_PARAM);
    }
    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->queue_prio_get)
                (unit, -1, priority, &t_cosq));

    *cosq = t_cosq;
    return (BCM_E_NONE);
}

int
bcm_robo_cosq_port_mapping_set(int unit, bcm_port_t port,
                   bcm_cos_t priority, bcm_cos_queue_t cosq)
{
    soc_cm_debug(DK_VERBOSE, 
                "BCM API : bcm_robo_cosq_port_mapping_set()..\n");
    if (!BCM_COSQ_QUEUE_VALID(unit, cosq)) {
        return (BCM_E_PARAM);
    }
    
    if (priority < 0 || priority >= BCM_COS_COUNT ||
                    cosq < 0 || cosq >= NUM_COS(unit)) {
        return (BCM_E_PARAM);
    }
    
    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->queue_prio_set)
                (unit, port, priority, cosq));

    return (BCM_E_NONE);
}
                
int
bcm_robo_cosq_port_mapping_get(int unit, bcm_port_t port,
                   bcm_cos_t priority, bcm_cos_queue_t *cosq)
{
    uint8       t_cosq = 0;
    
    soc_cm_debug(DK_VERBOSE, 
                "BCM API : bcm_robo_cosq_port_mapping_get()..\n");
    if (priority < 0 || priority > 7 ) {
        return (BCM_E_PARAM);
    }
    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->queue_prio_get)
                (unit, port, priority, &t_cosq));

    *cosq = t_cosq;
    return (BCM_E_NONE);
}

/*
 * Function:
 *      bcm_robo_cosq_port_bandwidth_get
 * Purpose:
 *      Retrieve bandwidth values for given COS policy.
 * Parameters:
 *      unit - RoboSwitch unit number.
 *  port - port to configure, -1 for any port.
 *      cosq - COS queue to configure, -1 for any COS queue.
 *      kbits_sec_min - (OUT) minimum bandwidth, kbits/sec.
 *      kbits_sec_max - (OUT) maximum bandwidth, kbits/sec.
 *      flags - (OUT) may include:
 *              BCM_COSQ_BW_EXCESS_PREF
 *              BCM_COSQ_BW_MINIMUM_PREF
 * Returns:
 *      BCM_E_XXX
 * Note:
 *      This API is designed mainly for Tucana Chip and not suitable
 *      for Robo Chip.
 */
int 
bcm_robo_cosq_port_bandwidth_get(int unit, bcm_port_t port,
                                   bcm_cos_queue_t cosq,
                                   uint32 *kbits_sec_min,
                                   uint32 *kbits_sec_max,
                                   uint32 *flags)
{
    int rv = BCM_E_NONE;
    uint32 temp, temp2;
    bcm_port_t loc_port;
    bcm_cos_queue_t loc_cos;
    
    soc_cm_debug(DK_VERBOSE, 
                "BCM API : bcm_robo_cosq_port_bandwidth_get()\n");

    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

    if (port < 0) {
        loc_port = SOC_PORT_MIN(unit,all);
    } else {
        if (SOC_PORT_VALID(unit, port)) {
            loc_port = port;
        } else {
            return BCM_E_PORT; 
        }
    }

    if (cosq < 0) {
        loc_cos = 0;
    } else {
        loc_cos = cosq;
    }

    switch (loc_cos) {
        case 0:
        case 1:
        case 2:
        case 3:
            /*
             * Per queue egress rate control is implemented in this API
             * for Robo chips. 
             * Egress rate control need 2 parameters, rate limit and 
             * bucket size. Although the meaning is not exactly the same,
             * the kbits_sec_min and kbits_sec_max parameters of this API 
             * are used to represent the rate limit and bucket size, respectively.
             */
            BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->rate_get)
                                (unit, loc_port, loc_cos,
                                DRV_RATE_CONTROL_DIRECTION_EGRESSS_PER_QUEUE,
                                kbits_sec_min, kbits_sec_max));
            break;
        case 4:
            if (soc_feature(unit, soc_feature_eav_support)) {
                /* Get current Macro Slot time */
                rv = (DRV_SERVICES(unit)->eav_time_sync_get)
                    (unit, DRV_EAV_TIME_SYNC_MACRO_SLOT_PERIOD, &temp, &temp2);
                /*
                 * kbit/sec =(bytes/slot time) * 8 * 1000 / (macro slot time * 1024)
                 */
                rv = (DRV_SERVICES(unit)->eav_queue_control_get)
                    (unit, loc_port, DRV_EAV_QUEUE_Q4_BANDWIDTH, &temp2);
                *kbits_sec_min = (temp2 * 8 * 1000) / (temp * 1024); 

                    /* unused parameter kbits_sec_max : return (-1) */
                    *kbits_sec_max = -1;
                    *flags = 0;
            } else {
                return BCM_E_UNAVAIL;
            }
            break;
        case 5:
            if (soc_feature(unit, soc_feature_eav_support)) {
                /*
                 * Class 5 slot time is 125 us.
                 * kbits/sec = (bytes/slot * 8 * 8000) / 1024
                 */
                rv = (DRV_SERVICES(unit)->eav_queue_control_get)
                    (unit, loc_port, DRV_EAV_QUEUE_Q5_BANDWIDTH, &temp);
                *kbits_sec_min = (temp * 8 * 8000) / 1024;

                /* Get the jitter control of Class5 ResE transmission */
                rv = (DRV_SERVICES(unit)->eav_queue_control_get)
                    (unit, loc_port, DRV_EAV_QUEUE_Q5_WINDOW, &temp);
                if (temp) {
                    *flags |= BCM_COSQ_BW_CLASS5_WINDOW;
                } else {
                    *flags = 0;
                }
                /* unused parameter kbits_sec_max : return (-1) */
                *kbits_sec_max = -1;
            } else {
                return BCM_E_UNAVAIL;
            }
            break;
        default:
            return BCM_E_PARAM;
    }

    return rv;
}

/*
 * Function:
 *      bcm_robo_cosq_port_bandwidth_set
 * Purpose:
 *      Set bandwidth values for given COS policy.
 * Parameters:
 *      unit - RoboSwitch unit number.
 *  port - port to configure, -1 for all ports.
 *      cosq - COS queue to configure, -1 for all COS queues.
 *      kbits_sec_min - minimum bandwidth, kbits/sec.
 *      kbits_sec_max - maximum bandwidth, kbits/sec.
 *      flags - may include:
 *              BCM_COSQ_BW_EXCESS_PREF
 *              BCM_COSQ_BW_MINIMUM_PREF
 * Returns:
 *      BCM_E_XXX
 * Note:
 *      This API is designed mainly for Tucana Chip and not suitable
 *      for Robo Chip.
 */
int 
bcm_robo_cosq_port_bandwidth_set(int unit, bcm_port_t port,
                                   bcm_cos_queue_t cosq,
                                   uint32 kbits_sec_min,
                                   uint32 kbits_sec_max,
                                   uint32 flags)
{
    int rv = BCM_E_NONE;
    uint32 temp, temp2;
    bcm_pbmp_t pbmp;
    bcm_port_t loc_port;
    int i;
    
    soc_cm_debug(DK_VERBOSE, 
                "BCM API : bcm_robo_cosq_port_bandwidth_set()\n");

    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

    if (port < 0) {
        BCM_PBMP_ASSIGN(pbmp, PBMP_ALL(unit));
    } else {
        if (SOC_PORT_VALID(unit, port)) {
            BCM_PBMP_PORT_SET(pbmp, port);
        } else {
            return BCM_E_PORT; 
        }
    }

    if (cosq < 0) {
        for (i = 0; i < NUM_COS(unit) - 1; i++) {
            BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->rate_set)
                                (unit, pbmp, i,
                                DRV_RATE_CONTROL_DIRECTION_EGRESSS_PER_QUEUE,
                                kbits_sec_min, kbits_sec_max));
        }
        return BCM_E_NONE;
    }

    switch (cosq) {
        case 0:
        case 1:
        case 2:
        case 3:
            /*
             * Per queue egress rate control is implemented in this API
             * for Robo chips. 
             * Egress rate control need 2 parameters, rate limit and 
             * bucket size. Although the meaning is not exactly the same,
             * the kbits_sec_min and kbits_sec_max parameters of this API 
             * are used to represent the rate limit and bucket size, respectively.
             */
            BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->rate_set)
                                (unit, pbmp, cosq,
                                DRV_RATE_CONTROL_DIRECTION_EGRESSS_PER_QUEUE,
                                kbits_sec_min, kbits_sec_max));
            break;
        case 4:
            if (soc_feature(unit, soc_feature_eav_support)) {
                
                 /* Check the maximum valid bandwidth value for EAV Class 4 */
                rv = (DRV_SERVICES(unit)->eav_queue_control_get)
                    (unit, port, DRV_EAV_QUEUE_Q4_BANDWIDTH_MAX_VALUE, &temp);

                if (kbits_sec_min > temp) {
                    soc_cm_debug(DK_ERR, 
                        "bcm_robo_cosq_port_bandwidth_set : BW value unsupported. \n");
                    return  SOC_E_PARAM;
                }
                /* Get current Macro Slot time */
                rv = (DRV_SERVICES(unit)->eav_time_sync_get)
                    (unit, DRV_EAV_TIME_SYNC_MACRO_SLOT_PERIOD, &temp, &temp2);
                /*
                 * bytes/slot = kbits/sec * 1024 / (8 * macro slot time * 1000)
                 */
                temp2 = (kbits_sec_min * 1024 * temp) / (8 * 1000);
                BCM_PBMP_ITER(pbmp, loc_port) {
                    rv = (DRV_SERVICES(unit)->eav_queue_control_set)
                        (unit, loc_port, DRV_EAV_QUEUE_Q4_BANDWIDTH, temp2);
                }
            } else {
                return BCM_E_UNAVAIL;
            }
            break;
        case 5:
            if (soc_feature(unit, soc_feature_eav_support)) {

                 /* Check the maximum valid bandwidth value for EAV Class 5 */
                rv = (DRV_SERVICES(unit)->eav_queue_control_get)
                    (unit, port, DRV_EAV_QUEUE_Q5_BANDWIDTH_MAX_VALUE, &temp);

                if (kbits_sec_min > temp) {
                    soc_cm_debug(DK_ERR, 
                        "bcm_robo_cosq_port_bandwidth_set : BW value unsupported. \n");
                    return  SOC_E_PARAM;
                }
                /*
                 * Class 5 slot time is 125 us.
                 * bytes/125us = kbit/sec * 1024 /(8 * 8000) 
                 */
                temp = (kbits_sec_min * 1024) / (8 * 8000); 
                BCM_PBMP_ITER(pbmp, loc_port) {
                    rv = (DRV_SERVICES(unit)->eav_queue_control_set)
                        (unit, loc_port, DRV_EAV_QUEUE_Q5_BANDWIDTH, temp);
                }
                /* Configure the jitter control of Class5 ResE transmission */
                if (flags & BCM_COSQ_BW_CLASS5_WINDOW) {
                    temp = 1;
                } else {
                    temp = 0;
                }
                BCM_PBMP_ITER(pbmp, loc_port) {
                    rv = (DRV_SERVICES(unit)->eav_queue_control_set)
                        (unit, loc_port, DRV_EAV_QUEUE_Q5_WINDOW, temp);
                }
            } else {
                return BCM_E_UNAVAIL;
            }
            break;
        default:
            return BCM_E_PARAM;
    }

    return rv;
}

/*
 * Function:
 *      bcm_robo_cosq_sched_weight_max_get
 * Purpose:
 *      Retrieve maximum weights for given COS policy.
 * Parameters:
 *      unit - RoboSwitch unit number.
 *      mode - Scheduling mode, one of BCM_COSQ_xxx
 *  weight_max - (output) Maximum weight for COS queue.
 *      0 if mode is BCM_COSQ_STRICT.
 *      1 if mode is BCM_COSQ_ROUND_ROBIN.
 *      -1 if not applicable to mode.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_robo_cosq_sched_weight_max_get(int unit, int mode,
                     int *weight_max)
{
    uint32 prop_val = 0;
    
    soc_cm_debug(DK_VERBOSE, 
                "BCM API : bcm_robo_cosq_sched_weight_max_get()..\n");
    switch (mode) {
    case BCM_COSQ_STRICT:
        *weight_max = BCM_COSQ_WEIGHT_STRICT;
        break;
    case BCM_COSQ_WEIGHTED_ROUND_ROBIN:
        BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->dev_prop_get)
                        (unit, DRV_DEV_PROP_COSQ_MAX_WEIGHT_VALUE, &prop_val));
        *weight_max = (int)prop_val;
        break;
    default:
        *weight_max = BCM_COSQ_WEIGHT_UNLIMITED;
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

int
bcm_robo_cosq_port_sched_set(int unit, bcm_pbmp_t pbm,
            int mode, const int weights[], int delay)
{
    uint32      drv_value, weight_value;
    int         i, sp_num;
    
    soc_cm_debug(DK_VERBOSE, 
                "BCM API : bcm_robo_cosq_port_sched_set()..\n");
    /* exclude CPU port in this API */
    BCM_PBMP_REMOVE(pbm, PBMP_CMIC(unit));
    
    /* for Robo Chip, we support Strict and weight round robin mode only */
    drv_value = (mode == BCM_COSQ_STRICT) ? DRV_QUEUE_MODE_STRICT : 
                (mode == BCM_COSQ_WEIGHTED_ROUND_ROBIN) ? 
                DRV_QUEUE_MODE_WRR : 0;
    
    /* the COSQ mode for Robo allowed Strict & WRR mode only */
    if (drv_value == 0){
        return BCM_E_PARAM;
    }

    if (mode == BCM_COSQ_STRICT) {
    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->queue_mode_set)
                    (unit, SOC_PBMP_WORD_GET(pbm, 0), drv_value));
        return BCM_E_NONE;
    }
    
    /* set the weight if mode is WRR */
    sp_num = 0;
    for (i = 0; i < NUM_COS(unit); i++){
        weight_value = weights[i];
        /* COSQ3 or COSQ2 is Strict priority if ROBO chip support STRICT/WRR scheduler */
        if (((i == NUM_COS(unit) - 1) || (i == NUM_COS(unit) - 2)) && 
            (weight_value == BCM_COSQ_WEIGHT_STRICT)) {
            sp_num ++;
        } else {
        BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->queue_WRR_weight_set)
                            (unit, 0, i, weight_value));
        }
    }

    /* Set scheduling combination mode if ROBO chip support it */
    if (sp_num == 1) { /* 1STRICT/3WRR : COSQ3>COS2/COS1/COS0 */
       if (weights[NUM_COS(unit) - 1] == BCM_COSQ_WEIGHT_STRICT) {
           drv_value = DRV_QUEUE_MODE_1STRICT_3WRR;
       } else {
           return BCM_E_PARAM;
       }
    } else if ((sp_num == 2)) { /* 2STRICT/2WRR : COSQ3>COS2>COS1/COS0 */
        drv_value = DRV_QUEUE_MODE_2STRICT_2WRR;
    }

    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->queue_mode_set)
                    (unit, SOC_PBMP_WORD_GET(pbm, 0), drv_value));
    
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_robo_cosq_sched_set
 * Purpose:
 *      Set up class-of-service policy and corresponding weights and delay
 * Parameters:
 *      unit - RoboSwitch unit number.
 *      mode - Scheduling mode, one of BCM_COSQ_xxx
 *  weights - Weights for each COS queue
 *      Unused if mode is BCM_COSQ_STRICT.
 *      Indicates number of packets sent before going on to
 *      the next COS queue.
 *  delay - Maximum delay in microseconds before returning the
 *      round-robin to the highest priority COS queue
 *      (Unused if mode other than BCM_COSQ_BOUNDED_DELAY)
 * Returns:
 *      BCM_E_XXX
 * Note :
 *      1. Not recommend user to set queue threshold at 
 *          Robo5338/5324/5380/5388.
 *      2. the Strict Mode in RobSwitch actually strict at the higest 
 *          Queue only. the other lower queues will still working at 
 *          WRR mode if set BCM_COSQ_STRICT.
 */

int
bcm_robo_cosq_sched_set(int unit, int mode, const int weights[], int delay)
{
    soc_cm_debug(DK_VERBOSE, 
                "BCM API : bcm_robo_cosq_sched_set()..\n");
    /* no CPU port been assigned in this API */
    return (bcm_cosq_port_sched_set(unit,
                            PBMP_ALL(unit),
                            mode, weights, delay));
}


int
bcm_robo_cosq_port_sched_get(int unit, bcm_pbmp_t pbm,
                int *mode, int weights[], int *delay)
{
    uint32      drv_value;
    int         i, port;
    int count;
    
    soc_cm_debug(DK_VERBOSE, 
                "BCM API : bcm_robo_cosq_port_sched_get()..\n");
    /* exclude CPU port in this API */
    BCM_PBMP_REMOVE(pbm, PBMP_CMIC(unit));
    
    BCM_PBMP_COUNT(pbm, count);

    if (count == 0) {
        return BCM_E_PARAM;
    }
    
    /* get the cosq schedule at the first port only in pbm */
    BCM_PBMP_ITER(pbm, port){
        BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->queue_mode_get)
                        (unit, port, &drv_value));
        /* for Robo Chip, we support Strict and weight round robin mode only */
        /* Robo Chip shuld have strict or WRR mode only */
        *mode = (drv_value == DRV_QUEUE_MODE_STRICT) ? 
                BCM_COSQ_STRICT : BCM_COSQ_WEIGHTED_ROUND_ROBIN;
        break;
    }
            
    /* set the weight if mode is WRR */
    for (i = 0; i < NUM_COS(unit); i++){
        BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->queue_WRR_weight_get)
                        (unit, 0, i, &drv_value));
                        
        weights[i] = drv_value;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_robo_cosq_sched_get
 * Purpose:
 *      Retrieve class-of-service policy and corresponding weights and delay
 * Parameters:
 *      unit - RoboSwitch unit number.
 *      mode_ptr - (output) Scheduling mode, one of BCM_COSQ_xxx
 *  weights - (output) Weights for each COS queue
 *      Unused if mode is BCM_COSQ_STRICT.
 *  delay - (output) Maximum delay in microseconds before returning
 *      the round-robin to the highest priority COS queue
 *      Unused if mode other than BCM_COSQ_BOUNDED_DELAY.
 * Returns:
 *      BCM_E_XXX
 * Note :
 *      1. Not recommend user to set queue threshold at 
 *          Robo5338/5324/5380/5388.
 *      2. the Strict Mode in RobSwitch actually strict at the higest 
 *          Queue only. the other lower queues will still working at 
 *          WRR mode if set BCM_COSQ_STRICT.
 */

int
bcm_robo_cosq_sched_get(int unit, int *mode, int weights[], int *delay)
{   
    soc_cm_debug(DK_VERBOSE, 
                "BCM API : bcm_robo_cosq_sched_get()..\n");
    /* no CPU port been assigned in this API */
    return (bcm_cosq_port_sched_get(unit,
                            PBMP_ALL(unit),
                            mode, weights, delay));
}

int
bcm_robo_cosq_discard_set(int unit, uint32 flags)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_cosq_discard_get(int unit, uint32 *flags)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_cosq_discard_port_set(int unit, bcm_port_t port,
                                 bcm_cos_queue_t cosq,
                                 uint32 color,
                                 int drop_start,
                                 int drop_slope,
                                 int average_time)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_cosq_discard_port_get(int unit, bcm_port_t port,
                                 bcm_cos_queue_t cosq,
                                 uint32 color,
                                 int *drop_start,
                                 int *drop_slope,
                                 int *average_time)
{
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_cosq_gport_discard_set
 * Purpose:
 *
 * Parameters:
 *      unit    - (IN) Unit number.
 *      port    - (IN) GPORT ID.
 *      cosq    - (IN) COS queue to configure
 *      discard - (IN) Discard settings
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_robo_cosq_gport_discard_set(int unit, bcm_gport_t port, bcm_cos_queue_t cosq,
                               bcm_cosq_gport_discard_t *discard)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_cosq_gport_discard_get
 * Purpose:
 *
 * Parameters:
 *      unit    - (IN) Unit number.
 *      port    - (IN) GPORT ID.
 *      cosq    - (IN) COS queue to get
 *      discard - (IN/OUT) Discard settings
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_robo_cosq_gport_discard_get(int unit, bcm_gport_t port, bcm_cos_queue_t cosq,
                               bcm_cosq_gport_discard_t *discard)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_cosq_gport_add
 * Purpose:
 *       
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) Physical port.
 *      numq - (IN) Number of COS queues.
 *      flags - (IN) Flags.
 *      gport - (IN/OUT) GPORT ID.
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_robo_cosq_gport_add(int unit, bcm_gport_t port, int numq, 
                       uint32 flags, bcm_gport_t *gport)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_cosq_gport_delete
 * Purpose:
 *      
 * Parameters:
 *      unit - (IN) Unit number.
 *      gport - (IN) GPORT ID.
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_robo_cosq_gport_delete(int unit, bcm_gport_t gport)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_cosq_gport_traverse
 * Purpose:
 *      Walks through the valid COSQ GPORTs and calls
 *      the user supplied callback function for each entry.
 * Parameters:
 *      unit       - (IN) bcm device.
 *      trav_fn    - (IN) Callback function.
 *      user_data  - (IN) User data to be passed to callback function.
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_XXX
 */
int
bcm_robo_cosq_gport_traverse(int unit, bcm_cosq_gport_traverse_cb cb,
                            void *user_data)
{
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_cosq_gport_bandwidth_set
 * Purpose:
 *       
 * Parameters:
 *      unit - (IN) Unit number.
 *      gport - (IN) GPORT ID.
 *      cosq - (IN) COS queue to configure, -1 for all COS queues.
 *      kbits_sec_min - (IN) minimum bandwidth, kbits/sec.
 *      kbits_sec_max - (IN) maximum bandwidth, kbits/sec.
 *      flags - (IN) BCM_COSQ_BW_*
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_robo_cosq_gport_bandwidth_set(int unit, bcm_gport_t gport, 
                                 bcm_cos_queue_t cosq, uint32 kbits_sec_min, 
                                 uint32 kbits_sec_max, uint32 flags)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_cosq_gport_bandwidth_get
 * Purpose:
 *       
 * Parameters:
 *      unit - (IN) Unit number.
 *      gport - (IN) GPORT ID.
 *      cosq - (IN) COS queue to configure, -1 for all COS queues.
 *      kbits_sec_min - (OUT) minimum bandwidth, kbits/sec.
 *      kbits_sec_max - (OUT) maximum bandwidth, kbits/sec.
 *      flags - (OUT) BCM_COSQ_BW_*
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_robo_cosq_gport_bandwidth_get(int unit, bcm_gport_t gport, 
                                 bcm_cos_queue_t cosq, uint32 *kbits_sec_min, 
                                 uint32 *kbits_sec_max, uint32 *flags)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_cosq_gport_sched_set
 * Purpose:
 *      
 * Parameters:
 *      unit - (IN) Unit number.
 *      gport - (IN) GPORT ID.
 *      cosq - (IN) COS queue to configure, -1 for all COS queues.
 *      mode - (IN) Scheduling mode, one of BCM_COSQ_xxx
 *  weight - (IN) Weight for the specified COS queue(s)
 *               Unused if mode is BCM_COSQ_STRICT.
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_robo_cosq_gport_sched_set(int unit, bcm_gport_t gport, 
                             bcm_cos_queue_t cosq, int mode, int weight)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_cosq_gport_sched_get
 * Purpose:
 *      
 * Parameters:
 *      unit - (IN) Unit number.
 *      gport - (IN) GPORT ID.
 *      cosq - (IN) COS queue
 *      mode - (OUT) Scheduling mode, one of BCM_COSQ_xxx
 *  weight - (OUT) Weight for the specified COS queue(s)
 *               Unused if mode is BCM_COSQ_STRICT.
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_robo_cosq_gport_sched_get(int unit, bcm_gport_t gport, 
                             bcm_cos_queue_t cosq, int *mode, int *weight)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_cosq_gport_attach
 * Purpose:
 *      
 * Parameters:
 *      unit - (IN) Unit number.
 *      sched_port - (IN) GPORT ID.
 *      cosq - (IN) COS queue.
 *      input_port - (IN) GPORT to attach to.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_cosq_gport_attach(int unit, bcm_gport_t sched_gport, 
                          bcm_cos_queue_t cosq, bcm_gport_t input_gport)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_cosq_gport_detach
 * Purpose:
 *
 * Parameters:
 *      unit - (IN) Unit number.
 *      sched_port - (IN) GPORT ID.
 *      cosq - (IN) COS queue.
 *      input_port - (IN) GPORT to detach from.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_robo_cosq_gport_detach(int unit, bcm_gport_t sched_gport,
                          bcm_cos_queue_t cosq, bcm_gport_t input_gport)
{
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_cosq_gport_attach_get
 * Purpose:
 *
 * Parameters:
 *      unit - (IN) Unit number.
 *      sched_port - (IN) GPORT ID.
 *      cosq - (OUT) COS queue.
 *      input_port - (OUT) GPORT to attach to.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_robo_cosq_gport_attach_get(int unit, bcm_gport_t sched_gport,
                          bcm_cos_queue_t *cosq, bcm_gport_t *input_gport)
{
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_robo_cosq_control_set
 * Purpose:
 *      Set specified feature configuration
 *
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) GPORT ID.
 *      cosq - (IN) COS queue.
 *      type - (IN) feature
 *      arg  - (IN) feature value
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_robo_cosq_control_set(int unit, bcm_gport_t port, bcm_cos_queue_t cosq,
                                           bcm_cosq_control_t type, int arg)
{
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_robo_cosq_control_set
 * Purpose:
 *      Get specified feature configuration
 *
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) GPORT ID.
 *      cosq - (IN) COS queue.
 *      type - (IN) feature
 *      arg  - (OUT) feature value
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_robo_cosq_control_get(int unit, bcm_gport_t port, bcm_cos_queue_t cosq,
                                          bcm_cosq_control_t type, int *arg)
{
    return BCM_E_UNAVAIL;
}


