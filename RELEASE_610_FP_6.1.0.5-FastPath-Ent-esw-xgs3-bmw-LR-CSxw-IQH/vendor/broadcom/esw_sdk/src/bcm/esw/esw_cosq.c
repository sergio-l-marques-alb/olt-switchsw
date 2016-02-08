/*
 * $Id: esw_cosq.c,v 1.1 2011/04/18 17:11:01 mruas Exp $
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

#include <sal/core/libc.h>

#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/debug.h>

#include <bcm/error.h>
#include <bcm/cosq.h>

#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/triumph.h>
#include <bcm_int/esw/triumph2.h>

#include <bcm_int/esw_dispatch.h>

#define BCM_COSQ_QUEUE_VALID(unit, numq) \
	((numq) >= 0 && (numq) < NUM_COS(unit))

/*
 * Function:
 *      bcm_esw_cosq_init
 * Purpose:
 *      Initialize (clear) all COS schedule/mapping state.
 * Parameters:
 *      unit - StrataSwitch unit number.
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_cosq_init(int unit)
{
    return (mbcm_driver[unit]->mbcm_cosq_init(unit));
}

/*
 * Function:
 *      bcm_esw_cosq_detach
 * Purpose:
 *      Discard all COS schedule/mapping state.
 * Parameters:
 *      unit - StrataSwitch unit number.
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_cosq_detach(int unit)
{
    return (mbcm_driver[unit]->mbcm_cosq_detach(unit, 0));
}

int bcm_esw_cosq_deinit(int unit)
{
    return (mbcm_driver[unit]->mbcm_cosq_detach(unit, 1));
}

/*
 * Function:
 *      bcm_esw_cosq_config_set
 * Purpose:
 *      Set the number of COS queues
 * Parameters:
 *      unit - StrataSwitch unit number.
 *      numq - number of COS queues (1, 2, or 4).
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_cosq_config_set(int unit, bcm_cos_queue_t numq)
{
    if (!BCM_COSQ_QUEUE_VALID(unit, numq-1)) {
	return (BCM_E_PARAM);
    }
    return (mbcm_driver[unit]->mbcm_cosq_config_set(unit, numq));
}

/*
 * Function:
 *      bcm_esw_cosq_config_get
 * Purpose:
 *      Get the number of cos queues
 * Parameters:
 *      unit - StrataSwitch unit number.
 *      numq - (Output) number of cosq
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_cosq_config_get(int unit, bcm_cos_queue_t *numq)
{
    return (mbcm_driver[unit]->mbcm_cosq_config_get(unit, numq));
}

/*
 * Function:
 *      bcm_esw_cosq_mapping_set
 * Purpose:
 *      Set which cosq a given priority should fall into
 * Parameters:
 *      unit - StrataSwitch unit number.
 *      priority - Priority value to map
 *      cosq - COS queue to map to
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_cosq_mapping_set(int unit, bcm_cos_t priority, bcm_cos_queue_t cosq)
{
    if (!BCM_COSQ_QUEUE_VALID(unit, cosq)) {
	return (BCM_E_PARAM);
    }

    return (mbcm_driver[unit]->mbcm_cosq_mapping_set(unit, -1,
						     priority, cosq));
}

int
bcm_esw_cosq_port_mapping_set(int unit, bcm_port_t port,
			      bcm_cos_t priority, bcm_cos_queue_t cosq)
{
    if (!BCM_COSQ_QUEUE_VALID(unit, cosq)) {
	return (BCM_E_PARAM);
    }

    return (mbcm_driver[unit]->mbcm_cosq_mapping_set(unit, port,
						     priority, cosq));
}

/*
 * Function:
 *      bcm_esw_cosq_mapping_get
 * Purpose:
 *      Determine which COS queue a given priority currently maps to.
 * Parameters:
 *      unit - StrataSwitch unit number.
 *      priority - Priority value
 *      cosq - (Output) COS queue number
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_cosq_mapping_get(int unit, bcm_cos_t priority, bcm_cos_queue_t *cosq)
{
    return (mbcm_driver[unit]->mbcm_cosq_mapping_get(unit, -1,
						     priority, cosq));
}

int
bcm_esw_cosq_port_mapping_get(int unit, bcm_port_t port,
			      bcm_cos_t priority, bcm_cos_queue_t *cosq)
{
    return (mbcm_driver[unit]->mbcm_cosq_mapping_get(unit, port,
						     priority, cosq));
}

/*
 * Function:
 *      bcm_esw_cosq_sched_set
 * Purpose:
 *      Set up class-of-service policy and corresponding weights and delay
 * Parameters:
 *      unit - StrataSwitch unit number.
 *      mode - Scheduling mode, one of BCM_COSQ_xxx
 *	weights - Weights for each COS queue
 *		Unused if mode is BCM_COSQ_STRICT.
 *		Indicates number of packets sent before going on to
 *		the next COS queue.
 *	delay - Maximum delay in microseconds before returning the
 *		round-robin to the highest priority COS queue
 *		(Unused if mode other than BCM_COSQ_BOUNDED_DELAY)
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_cosq_sched_set(int unit, int mode, const int weights[], int delay)
{
    return (mbcm_driver[unit]->mbcm_cosq_port_sched_set(unit,
							PBMP_ALL(unit),
						    mode, weights, delay));
}

int
bcm_esw_cosq_port_sched_set(int unit, bcm_pbmp_t pbm,
			int mode, const int weights[], int delay)
{
    bcm_pbmp_t pbm_all = PBMP_ALL(unit);
    BCM_PBMP_AND(pbm_all, pbm);
    if (BCM_PBMP_NEQ(pbm_all, pbm)) {
        return BCM_E_PARAM;
    }
    return (mbcm_driver[unit]->mbcm_cosq_port_sched_set(unit,
							pbm,
							mode, weights, delay));
}

/*
 * Function:
 *      bcm_esw_cosq_sched_get
 * Purpose:
 *      Retrieve class-of-service policy and corresponding weights and delay
 * Parameters:
 *      unit - StrataSwitch unit number.
 *      mode_ptr - (output) Scheduling mode, one of BCM_COSQ_xxx
 *	weights - (output) Weights for each COS queue
 *		Unused if mode is BCM_COSQ_STRICT.
 *	delay - (output) Maximum delay in microseconds before returning
 *		the round-robin to the highest priority COS queue
 *		Unused if mode other than BCM_COSQ_BOUNDED_DELAY.
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_cosq_sched_get(int unit, int *mode, int weights[], int *delay)
{
    return (mbcm_driver[unit]->mbcm_cosq_port_sched_get(unit,
							PBMP_ALL(unit),
							mode, weights, delay));
}

int
bcm_esw_cosq_port_sched_get(int unit, bcm_pbmp_t pbm,
			int *mode, int weights[], int *delay)
{
    bcm_pbmp_t pbm_all = PBMP_ALL(unit);
    BCM_PBMP_AND(pbm_all, pbm);
    if (BCM_PBMP_NEQ(pbm_all, pbm)) {
        return BCM_E_PARAM;
    }
    return (mbcm_driver[unit]->mbcm_cosq_port_sched_get(unit,
							pbm,
							mode, weights, delay));
}

/*
 * Function:
 *      bcm_esw_cosq_sched_weight_max_get
 * Purpose:
 *      Retrieve maximum weights for given COS policy.
 * Parameters:
 *      unit - StrataSwitch unit number.
 *      mode - Scheduling mode, one of BCM_COSQ_xxx
 *	weight_max - (output) Maximum weight for COS queue.
 *		0 if mode is BCM_COSQ_STRICT.
 *		1 if mode is BCM_COSQ_ROUND_ROBIN.
 *		-1 if not applicable to mode.
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_cosq_sched_weight_max_get(int unit, int mode, int *weight_max)
{
    return (mbcm_driver[unit]->mbcm_cosq_sched_weight_max_get(unit,
							      mode,
							      weight_max));
}

/*
 * Function:
 *      bcm_esw_cosq_port_bandwidth_set
 * Purpose:
 *      Set bandwidth values for given COS policy.
 * Parameters:
 *      unit - StrataSwitch unit number.
 *	port - port to configure, -1 for all ports.
 *      cosq - COS queue to configure, -1 for all COS queues.
 *      kbits_sec_min - minimum bandwidth, kbits/sec.
 *      kbits_sec_max - maximum bandwidth, kbits/sec.
 *      flags - may include:
 *              BCM_COSQ_BW_EXCESS_PREF
 *              BCM_COSQ_BW_MINIMUM_PREF
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_cosq_port_bandwidth_set(int unit, bcm_port_t port,
                                bcm_cos_queue_t cosq,
                                uint32 kbits_sec_min,
                                uint32 kbits_sec_max,
                                uint32 flags)
{
    int rv = BCM_E_NONE;
    bcm_pbmp_t pbmp;
    bcm_port_t loc_port;
    bcm_cos_queue_t start_cos, end_cos, loc_cos;

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
        start_cos = 0;
        end_cos = NUM_COS(unit) - 1;
    } else {
        if (cosq < NUM_COS(unit)) {
            start_cos = end_cos = cosq;
        } else {
            return BCM_E_PARAM;
        }
    }

    BCM_PBMP_ITER(pbmp, loc_port) {
        for (loc_cos = start_cos; loc_cos <= end_cos; loc_cos++) {
            if ((rv = mbcm_driver[unit]->mbcm_cosq_port_bandwidth_set(unit,
                           loc_port, loc_cos, kbits_sec_min, kbits_sec_max,
                            kbits_sec_max, flags)) < 0) {
                return rv;
            }
        }
    }

    return rv;
}

/*
 * Function:
 *      bcm_esw_cosq_port_bandwidth_get
 * Purpose:
 *      Retrieve bandwidth values for given COS policy.
 * Parameters:
 *      unit - StrataSwitch unit number.
 *	port - port to configure, -1 for any port.
 *      cosq - COS queue to configure, -1 for any COS queue.
 *      kbits_sec_min - (OUT) minimum bandwidth, kbits/sec.
 *      kbits_sec_max - (OUT) maximum bandwidth, kbits/sec.
 *      flags - (OUT) may include:
 *              BCM_COSQ_BW_EXCESS_PREF
 *              BCM_COSQ_BW_MINIMUM_PREF
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_cosq_port_bandwidth_get(int unit, bcm_port_t port, 
                                bcm_cos_queue_t cosq,
                                uint32 *kbits_sec_min,
                                uint32 *kbits_sec_max,
                                uint32 *flags)
{
    bcm_port_t loc_port;
    bcm_cos_queue_t loc_cos;
    uint32 kbits_sec_burst;    /* Dummy variable */

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
        if (cosq < NUM_COS(unit)) {
            loc_cos = cosq;
        } else {
            return BCM_E_PARAM;
        }
    }

    return (mbcm_driver[unit]->mbcm_cosq_port_bandwidth_get(unit,
                           loc_port, loc_cos, kbits_sec_min, kbits_sec_max,
                                              &kbits_sec_burst, flags));
}

/*
 * Function:
 *      bcm_esw_cosq_discard_set
 * Purpose:
 *      Set the COS queue WRED parameters
 * Parameters:
 *      unit  - StrataSwitch unit number.
 *      flags - BCM_COSQ_DISCARD_*
 * Returns:
 *      BCM_E_XXX
 */
 
int     
bcm_esw_cosq_discard_set(int unit, uint32 flags)
{
    return (mbcm_driver[unit]->mbcm_cosq_discard_set(unit, flags));
}
 
/*
 * Function:
 *      bcm_esw_cosq_discard_get
 * Purpose:
 *      Get the COS queue WRED parameters
 * Parameters:
 *      unit  - StrataSwitch unit number.
 *      flags - (OUT) BCM_COSQ_DISCARD_*
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_cosq_discard_get(int unit, uint32 *flags)
{
    return (mbcm_driver[unit]->mbcm_cosq_discard_get(unit, flags));
}
    
/*
 * Function:
 *      bcm_esw_cosq_discard_port_set
 * Purpose:
 *      Set the COS queue WRED parameters
 * Parameters:
 *      unit  - StrataSwitch unit number.
 *      port  - port to configure (-1 for all ports).
 *      cosq  - COS queue to configure (-1 for all queues).
 *      color - BCM_COSQ_DISCARD_COLOR_*
 *      drop_start -  percentage of queue
 *      drop_slope -  degress 0..90
 *      average_time - in microseconds
 *
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_cosq_discard_port_set(int unit, bcm_port_t port,
                          bcm_cos_queue_t cosq,
                          uint32 color,
                          int drop_start,
                          int drop_slope,
                          int average_time)
{
    int rv = BCM_E_NONE;
    bcm_pbmp_t pbmp;
    bcm_port_t loc_port;
    
    if (BCM_GPORT_IS_SET(port)) {
        rv = mbcm_driver[unit]->mbcm_cosq_discard_port_set(unit,
                       port, cosq, color, drop_start, drop_slope,
                       average_time);
        return rv;
    } else if (port < 0) {
        BCM_PBMP_ASSIGN(pbmp, PBMP_ALL(unit));
    } else {
        if (SOC_PORT_VALID(unit, port)) {
            BCM_PBMP_PORT_SET(pbmp, port);
        } else {
            return BCM_E_PORT;
        }
    }
    
    BCM_PBMP_ITER(pbmp, loc_port) {
        if ((rv = mbcm_driver[unit]->mbcm_cosq_discard_port_set(unit,
                       loc_port, cosq, color, drop_start, drop_slope,
                       average_time)) < 0) {
            return rv;
        }
    }

    return rv;
}

/*
 * Function:
 *      bcm_esw_cosq_discard_port_get
 * Purpose:
 *      Set the COS queue WRED parameters
 * Parameters:
 *      unit  - StrataSwitch unit number.
 *      port  - port to get (-1 for any).
 *      cosq  - COS queue to get (-1 for any).
 *      color - BCM_COSQ_DISCARD_COLOR_*
 *      drop_start - (OUT) percentage of queue
 *      drop_slope - (OUT) degress 0..90
 *      average_time - (OUT) in microseconds
 *
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_cosq_discard_port_get(int unit, bcm_port_t port,
                          bcm_cos_queue_t cosq,
                          uint32 color,
                          int *drop_start,
                          int *drop_slope,
                          int *average_time)
{
    bcm_port_t loc_port;
    bcm_cos_queue_t loc_cos;
 
    if (BCM_GPORT_IS_SET(port)) {
        return (mbcm_driver[unit]->mbcm_cosq_discard_port_get(unit,
                               port, cosq, color, drop_start, 
                               drop_slope, average_time));
    } else if (port < 0) {
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
        if (cosq < NUM_COS(unit)) {
            loc_cos = cosq;
        } else {
            return BCM_E_PARAM;
        }
    }

    return (mbcm_driver[unit]->mbcm_cosq_discard_port_get(unit,
                           loc_port, loc_cos, color, drop_start, 
                           drop_slope, average_time));

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
bcm_esw_cosq_gport_discard_set(int unit, bcm_gport_t port, bcm_cos_queue_t cosq,
                               bcm_cosq_gport_discard_t *discard)
{
#ifdef BCM_TRIUMPH2_SUPPORT
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
        SOC_IS_VALKYRIE2(unit)) {
        return bcm_tr2_cosq_gport_discard_set(unit, port, cosq, discard);
    }
#endif
#ifdef BCM_TRIUMPH_SUPPORT
    if (SOC_IS_TRX(unit)) {
        return bcm_tr_cosq_gport_discard_set(unit, port, cosq, discard);
    }
#endif
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
bcm_esw_cosq_gport_discard_get(int unit, bcm_gport_t port, bcm_cos_queue_t cosq,
                               bcm_cosq_gport_discard_t *discard)
{
#ifdef BCM_TRIUMPH2_SUPPORT
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
        SOC_IS_VALKYRIE2(unit)) {
        return bcm_tr2_cosq_gport_discard_get(unit, port, cosq, discard);
    }
#endif
#ifdef BCM_TRIUMPH_SUPPORT
    if (SOC_IS_TRX(unit)) {
        return bcm_tr_cosq_gport_discard_get(unit, port, cosq, discard);
    }
#endif
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
bcm_esw_cosq_gport_add(int unit, bcm_gport_t port, int numq, 
                       uint32 flags, bcm_gport_t *gport)
{
#ifdef BCM_TRIUMPH2_SUPPORT
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
        SOC_IS_VALKYRIE2(unit)) {
        return bcm_tr2_cosq_gport_add(unit, port, numq, flags, gport);
    }
#endif
#ifdef BCM_TRIUMPH_SUPPORT
    if (SOC_IS_TR_VL(unit)) {
        return bcm_tr_cosq_gport_add(unit, port, numq, flags, gport);
    }
#endif
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
bcm_esw_cosq_gport_delete(int unit, bcm_gport_t gport)
{
#ifdef BCM_TRIUMPH2_SUPPORT
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
        SOC_IS_VALKYRIE2(unit)) {
        return bcm_tr2_cosq_gport_delete(unit, gport);
    }
#endif
#ifdef BCM_TRIUMPH_SUPPORT
    if (SOC_IS_TR_VL(unit)) {
        return bcm_tr_cosq_gport_delete(unit, gport);
    }
#endif
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
bcm_esw_cosq_gport_traverse(int unit, bcm_cosq_gport_traverse_cb cb,
                            void *user_data)
{
#ifdef BCM_TRIUMPH2_SUPPORT
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
        SOC_IS_VALKYRIE2(unit)) {
        return bcm_tr2_cosq_gport_traverse(unit, cb, user_data);
    }
#endif
#ifdef BCM_TRIUMPH_SUPPORT
    if (SOC_IS_TR_VL(unit)) {
        return bcm_tr_cosq_gport_traverse(unit, cb, user_data);
    }
#endif
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
bcm_esw_cosq_gport_bandwidth_set(int unit, bcm_gport_t gport, 
                                 bcm_cos_queue_t cosq, uint32 kbits_sec_min, 
                                 uint32 kbits_sec_max, uint32 flags)
{
#ifdef BCM_TRIUMPH2_SUPPORT
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
        SOC_IS_VALKYRIE2(unit)) {
        return bcm_tr2_cosq_gport_bandwidth_set(unit, gport, 
                                               cosq, kbits_sec_min, 
                                               kbits_sec_max, flags);
    }
#endif
#ifdef BCM_TRIUMPH_SUPPORT
    if (SOC_IS_TR_VL(unit)) {
        return bcm_tr_cosq_gport_bandwidth_set(unit, gport, 
                                               cosq, kbits_sec_min, 
                                               kbits_sec_max, flags);
    }
#endif
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
bcm_esw_cosq_gport_bandwidth_get(int unit, bcm_gport_t gport, 
                                 bcm_cos_queue_t cosq, uint32 *kbits_sec_min, 
                                 uint32 *kbits_sec_max, uint32 *flags)
{
#ifdef BCM_TRIUMPH2_SUPPORT
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
        SOC_IS_VALKYRIE2(unit)) {
        return bcm_tr2_cosq_gport_bandwidth_get(unit, gport, 
                                               cosq, kbits_sec_min, 
                                               kbits_sec_max, flags);
    }
#endif
#ifdef BCM_TRIUMPH_SUPPORT
    if (SOC_IS_TR_VL(unit)) {
        return bcm_tr_cosq_gport_bandwidth_get(unit, gport, 
                                               cosq, kbits_sec_min, 
                                               kbits_sec_max, flags);
    }
#endif
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
 *	weight - (IN) Weight for the specified COS queue(s)
 *               Unused if mode is BCM_COSQ_STRICT.
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_esw_cosq_gport_sched_set(int unit, bcm_gport_t gport, 
                             bcm_cos_queue_t cosq, int mode, int weight)
{
#ifdef BCM_TRIUMPH2_SUPPORT
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
        SOC_IS_VALKYRIE2(unit)) {
        return bcm_tr2_cosq_gport_sched_set(unit, gport, cosq, mode, weight);
    }
#endif
#ifdef BCM_TRIUMPH_SUPPORT
    if (SOC_IS_TR_VL(unit)) {
        return bcm_tr_cosq_gport_sched_set(unit, gport, cosq, mode, weight);
    }
#endif
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
 *	weight - (OUT) Weight for the specified COS queue(s)
 *               Unused if mode is BCM_COSQ_STRICT.
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_esw_cosq_gport_sched_get(int unit, bcm_gport_t gport, 
                             bcm_cos_queue_t cosq, int *mode, int *weight)
{
#ifdef BCM_TRIUMPH2_SUPPORT
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
        SOC_IS_VALKYRIE2(unit)) {
        return bcm_tr2_cosq_gport_sched_get(unit, gport, cosq, mode, weight);
    }
#endif
#ifdef BCM_TRIUMPH_SUPPORT
    if (SOC_IS_TR_VL(unit)) {
        return bcm_tr_cosq_gport_sched_get(unit, gport, cosq, mode, weight);
    }
#endif
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_cosq_gport_attach
 * Purpose:
 *      
 * Parameters:
 *      unit       - (IN) Unit number.
 *      sched_port - (IN) Scheduler GPORT ID.
 *      input_port - (IN) GPORT to attach to.
 *      cosq       - (IN) COS queue to attach to.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_esw_cosq_gport_attach(int unit, bcm_gport_t sched_gport, 
                          bcm_gport_t input_gport, bcm_cos_queue_t cosq)
{
#ifdef BCM_TRIUMPH2_SUPPORT
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
        SOC_IS_VALKYRIE2(unit)) {
        return bcm_tr2_cosq_gport_attach(unit, sched_gport, 
                                        input_gport, cosq);
    }
#endif
#ifdef BCM_TRIUMPH_SUPPORT
    if (SOC_IS_TR_VL(unit)) {
        return bcm_tr_cosq_gport_attach(unit, sched_gport, 
                                        input_gport, cosq);
    }
#endif
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_cosq_gport_detach
 * Purpose:
 *
 * Parameters:
 *      unit       - (IN) Unit number.
 *      sched_port - (IN) Scheduler GPORT ID.
 *      input_port - (IN) GPORT to detach from.
 *      cosq       - (IN) COS queue to detach from.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_esw_cosq_gport_detach(int unit, bcm_gport_t sched_gport,
                          bcm_gport_t input_gport, bcm_cos_queue_t cosq)
{
#ifdef BCM_TRIUMPH2_SUPPORT
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
        SOC_IS_VALKYRIE2(unit)) {
        return bcm_tr2_cosq_gport_detach(unit, sched_gport,
                                        input_gport, cosq);
    }
#endif
#ifdef BCM_TRIUMPH_SUPPORT
    if (SOC_IS_TR_VL(unit)) {
        return bcm_tr_cosq_gport_detach(unit, sched_gport,
                                        input_gport, cosq);
    }
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_cosq_gport_attach_get
 * Purpose:
 *
 * Parameters:
 *      unit       - (IN) Unit number.
 *      sched_port - (IN) Scheduler GPORT ID.
 *      input_port - (OUT) GPORT attached to.
 *      cosq       - (OUT) COS queue attached to.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_esw_cosq_gport_attach_get(int unit, bcm_gport_t sched_gport,
                              bcm_gport_t *input_gport, bcm_cos_queue_t *cosq)
{
#ifdef BCM_TRIUMPH2_SUPPORT
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
        SOC_IS_VALKYRIE2(unit)) {
        return bcm_tr2_cosq_gport_attach_get(unit, sched_gport, 
                                            input_gport, cosq);
    }
#endif
#ifdef BCM_TRIUMPH_SUPPORT
    if (SOC_IS_TR_VL(unit)) {
        return bcm_tr_cosq_gport_attach_get(unit, sched_gport, 
                                            input_gport, cosq);
    }
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_cosq_gport_get
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_esw_cosq_gport_get(
    int unit, 
    bcm_gport_t gport, 
    bcm_gport_t *physical_port, 
    int *num_cos_levels, 
    uint32 *flags)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_cosq_fabric_distribution_add
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_esw_cosq_fabric_distribution_add(
    int unit, 
    bcm_fabric_distribution_t ds_id, 
    int num_cos_levels, 
    uint32 flags, 
    bcm_gport_t *req_gport)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_cosq_fabric_distribution_get
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_esw_cosq_fabric_distribution_get(
    int unit, 
    bcm_fabric_distribution_t ds_id, 
    int *num_cos_levels, 
    bcm_gport_t *req_gport, 
    uint32 *flags)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_cosq_gport_size_set
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_esw_cosq_gport_size_set(
    int unit, 
    bcm_gport_t gport, 
    bcm_cos_queue_t cosq, 
    uint32 bytes_min, 
    uint32 bytes_max)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_cosq_gport_size_get
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_esw_cosq_gport_size_get(
    int unit, 
    bcm_gport_t gport, 
    bcm_cos_queue_t cosq, 
    uint32 *bytes_min, 
    uint32 *bytes_max)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_cosq_gport_enable_set
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_esw_cosq_gport_enable_set(
    int unit, 
    bcm_gport_t gport, 
    bcm_cos_queue_t cosq, 
    int enable)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_cosq_gport_enable_get
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_esw_cosq_gport_enable_get(
    int unit, 
    bcm_gport_t gport, 
    bcm_cos_queue_t cosq, 
    int *enable)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_cosq_gport_stat_enable_set
 * Purpose:
 *      ?
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_esw_cosq_gport_stat_enable_set(
    int unit, 
    bcm_gport_t gport, 
    int enable)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_cosq_gport_stat_enable_get
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_esw_cosq_gport_stat_enable_get(
    int unit, 
    bcm_gport_t gport, 
    int *enable)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_cosq_gport_stat_get
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_esw_cosq_gport_stat_get(
    int unit, 
    bcm_gport_t gport, 
    bcm_cos_queue_t cosq, 
    bcm_cosq_gport_stats_t stat, 
    uint64 *value)
{
    return BCM_E_UNAVAIL; 
}

/*
 * Function:
 *      bcm_cosq_gport_stat_set
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_esw_cosq_gport_stat_set(
    int unit, 
    bcm_gport_t gport, 
    bcm_cos_queue_t cosq, 
    bcm_cosq_gport_stats_t stat, 
    uint64 value)
{
    return BCM_E_UNAVAIL; 
}



/*
 * Function:
 *      bcm_esw_cosq_control_set
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
bcm_esw_cosq_control_set(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                         bcm_cosq_control_t type, int arg)
{
    uint32 kbits_sec_min, kbits_sec_max, kbits_sec_burst,
        kbits_burst, flags;
    bcm_pbmp_t pbmp;
    bcm_port_t port;
    bcm_cos_queue_t start_cos, end_cos, loc_cos;

    switch (type) {
    case bcmCosqControlBandwidthBurstMax:
        kbits_burst = arg & 0x7fffffff; /* Convert to uint32  */
#ifdef BCM_TRIUMPH2_SUPPORT
        if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
            SOC_IS_VALKYRIE2(unit)) {
            return bcm_tr2_cosq_gport_bandwidth_burst_set(unit, gport,
                                                       cosq, kbits_burst);
        }
#endif  
#ifdef BCM_TRIUMPH_SUPPORT
        if (SOC_IS_TR_VL(unit)) {
            return bcm_tr_cosq_gport_bandwidth_burst_set(unit, gport,
                                                          cosq, kbits_burst);
        }
#endif  
        if (gport < 0) {
            BCM_PBMP_ASSIGN(pbmp, PBMP_ALL(unit));
        } else {
            /* Must use local ports on legacy devices */
            BCM_IF_ERROR_RETURN
                (bcm_esw_port_local_get(unit, gport, &port));
            if (SOC_PORT_VALID(unit, port)) {
                BCM_PBMP_PORT_SET(pbmp, port);
            } else {
                return BCM_E_PORT;
            }
        }

        if (cosq < 0) {
            start_cos = 0;
            end_cos = NUM_COS(unit) - 1;
        } else {
            if (cosq < NUM_COS(unit)) {
                start_cos = end_cos = cosq;
            } else {
                return BCM_E_PARAM;
            }
        }

        BCM_PBMP_ITER(pbmp, port) {
            for (loc_cos = start_cos; loc_cos <= end_cos; loc_cos++) {
                BCM_IF_ERROR_RETURN
                    (mbcm_driver[unit]->
                     mbcm_cosq_port_bandwidth_get(unit, port, loc_cos,
                                                  &kbits_sec_min,
                                                  &kbits_sec_max,
                                                  &kbits_sec_burst, &flags));
                BCM_IF_ERROR_RETURN
                    (mbcm_driver[unit]->
                     mbcm_cosq_port_bandwidth_set(unit, port, loc_cos,
                                                  kbits_sec_min,
                                                  kbits_sec_max,
                                                  kbits_burst, flags));
            }
        }
        return BCM_E_NONE;
    default:
        break;
    }

    return BCM_E_UNAVAIL;
}

/* 
 * Function:
 *      bcm_esw_cosq_control_get
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
bcm_esw_cosq_control_get(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                         bcm_cosq_control_t type, int *arg)
{
    uint32 kbits_sec_min, kbits_sec_max, kbits_burst, flags;
    bcm_port_t port;
    bcm_cos_queue_t loc_cos;

    switch (type) {
    case bcmCosqControlBandwidthBurstMax:
#ifdef BCM_TRIUMPH2_SUPPORT
        if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
            SOC_IS_VALKYRIE2(unit)) {
            BCM_IF_ERROR_RETURN
                (bcm_tr2_cosq_gport_bandwidth_burst_get(unit, gport,
                                                        cosq, &kbits_burst));
        } else 
#endif  
#ifdef BCM_TRIUMPH_SUPPORT
        if (SOC_IS_TR_VL(unit)) {
            BCM_IF_ERROR_RETURN
                (bcm_tr_cosq_gport_bandwidth_burst_get(unit, gport,
                                                       cosq, &kbits_burst));
        } else 
#endif  
        {
            if (gport < 0) {
                port = SOC_PORT_MIN(unit,all);
            } else {
                /* Must use local ports on legacy devices */
                BCM_IF_ERROR_RETURN
                    (bcm_esw_port_local_get(unit, gport, &port));
                if (!SOC_PORT_VALID(unit, port)) {
                    return BCM_E_PORT;
                }
            }

            if (cosq < 0) {
                loc_cos = 0;
            } else {
                if (cosq < NUM_COS(unit)) {
                    loc_cos = cosq;
                } else {
                    return BCM_E_PARAM;
                }
            }

            BCM_IF_ERROR_RETURN
                (mbcm_driver[unit]->
                 mbcm_cosq_port_bandwidth_get(unit, port, loc_cos,
                                              &kbits_sec_min, &kbits_sec_max,
                                              &kbits_burst, &flags));
        }
        *arg = kbits_burst & 0x7fffffff; /* Convert to int  */
        return BCM_E_NONE;
    default:
        break;
    }

    return BCM_E_UNAVAIL;
}



#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
/*
 * Function:
 *     _bcm_cosq_sw_dump
 * Purpose:
 *     Displays COS Queue information maintained by software.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
void
_bcm_cosq_sw_dump(int unit)
{
    bcm_cos_queue_t  num_cosq = 0;

    bcm_esw_cosq_config_get(unit, &num_cosq);
    soc_cm_print("\nSW Information COSQ - Unit %d\n", unit);
    soc_cm_print("    Number : %d\n", num_cosq);

    return;
}
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */
