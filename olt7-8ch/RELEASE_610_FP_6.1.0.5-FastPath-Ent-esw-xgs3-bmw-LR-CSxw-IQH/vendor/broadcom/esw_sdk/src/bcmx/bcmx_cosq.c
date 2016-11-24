/*
 * $Id: bcmx_cosq.c,v 1.1 2011/04/18 17:11:03 mruas Exp $
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
 * File:    bcmx/cosq.c
 * Purpose: BCMX Class of Service Queue APIs
 */

#include <bcm/types.h>

#include <bcmx/cosq.h>
#include <bcmx/lport.h>
#include <bcmx/bcmx.h>
#include <bcmx/lplist.h>

#include "bcmx_int.h"

#define BCMX_COSQ_GPORT_DISCARD_T_PTR_TO_BCM(_discard)    \
    ((bcm_cosq_gport_discard_t *)(_discard))

/*
 * Function:
 *      bcmx_cosq_init
 */

int
bcmx_cosq_init(void)
{
    int rv = BCM_E_NONE, tmp_rv;
    int i, bcm_unit;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_cosq_init(bcm_unit);
        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
    }

    return rv;
}


/*
 * Function:
 *      bcmx_cosq_config_set
 */

int
bcmx_cosq_config_set(int numq)
{
    int rv = BCM_E_NONE, tmp_rv;
    int i, bcm_unit;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_cosq_config_set(bcm_unit, numq);
        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
    }

    return rv;
}


/*
 * Function:
 *      bcmx_cosq_config_get
 */

int
bcmx_cosq_config_get(int *numq)
{
    int rv, i, bcm_unit;

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_cosq_config_get(bcm_unit, numq);
        if (rv >= 0) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}


/*
 * Function:
 *      bcmx_cosq_mapping_set
 */

int
bcmx_cosq_mapping_set(bcm_cos_t priority,
                      bcm_cos_queue_t cosq)
{
    int rv = BCM_E_NONE, tmp_rv;
    int i, bcm_unit;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_cosq_mapping_set(bcm_unit, priority, cosq);
        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
    }

    return rv;
}


/*
 * Function:
 *      bcmx_cosq_mapping_get
 */

int
bcmx_cosq_mapping_get(bcm_cos_t priority,
                      bcm_cos_queue_t *cosq)
{
    int rv, i, bcm_unit;

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_cosq_mapping_get(bcm_unit, priority, cosq);
        if (rv >= 0) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_cosq_port_mapping_set
 */

int
bcmx_cosq_port_mapping_set(bcmx_lport_t port,
                           bcm_cos_t priority,
                           bcm_cos_queue_t cosq)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_cosq_port_mapping_set(bcm_unit, bcm_port, priority, cosq);
}


/*
 * Function:
 *      bcmx_cosq_port_mapping_get
 */

int
bcmx_cosq_port_mapping_get(bcmx_lport_t port,
                           bcm_cos_t priority,
                           bcm_cos_queue_t *cosq)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_cosq_port_mapping_get(bcm_unit, bcm_port, priority, cosq);
}

/*
 * Function:
 *      bcmx_cosq_sched_set
 */

int
bcmx_cosq_sched_set(int mode,
                    const int weights[BCM_COS_COUNT],
                    int delay)
{
    int rv = BCM_E_NONE, tmp_rv;
    int i, bcm_unit;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_cosq_sched_set(bcm_unit, mode, weights, delay);
        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
    }

    return rv;
}


/*
 * Function:
 *      bcmx_cosq_port_sched_set
 */

int
bcmx_cosq_port_sched_set(bcmx_lplist_t lplist,
                         int mode,
                         const int weights[BCM_COS_COUNT],
                         int delay)
{
    int rv = BCM_E_NONE, tmp_rv;
    int i, bcm_unit;
    bcm_pbmp_t pbmp;

    BCMX_UNIT_ITER(bcm_unit, i) {
        BCMX_LPLIST_TO_PBMP(lplist, bcm_unit, pbmp);
        if (BCM_PBMP_NOT_NULL(pbmp)) {
            tmp_rv = bcm_cosq_port_sched_set(bcm_unit, pbmp, mode,
                                             weights, delay);
            BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
        }
    }

    return rv;
}


/*
 * Function:
 *      bcmx_cosq_sched_get
 */

int
bcmx_cosq_sched_get(int *mode,
                    int weights[BCM_COS_COUNT],
                    int *delay)
{
    int rv, i, bcm_unit;

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_cosq_sched_get(bcm_unit, mode, weights, delay);
        if (rv >= 0) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}


/*
 * Function:
 *      bcmx_cosq_port_sched_get
 */

int
bcmx_cosq_port_sched_get(bcmx_lplist_t lplist,
                         int *mode,
                         int weights[BCM_COS_COUNT],
                         int *delay)
{
    int rv, i, bcm_unit;
    bcm_pbmp_t pbmp;

    BCMX_UNIT_ITER(bcm_unit, i) {
        BCMX_LPLIST_TO_PBMP(lplist, bcm_unit, pbmp);
        if (BCM_PBMP_NOT_NULL(pbmp)) {
            rv = bcm_cosq_port_sched_get(bcm_unit, pbmp, mode,
                                         weights, delay);
            if (rv >= 0) {
                return rv;
            }
        }
    }

    return BCM_E_UNAVAIL;
}

int
bcmx_cosq_sched_weight_max_get(int mode,
                               int *weight_max)
{
    int rv, i, bcm_unit;

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_cosq_sched_weight_max_get(bcm_unit, mode, weight_max);
        if (rv >= 0) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

int
bcmx_cosq_port_bandwidth_set(bcmx_lport_t port, bcm_cos_queue_t cosq,
                             uint32 kbits_sec_min, uint32 kbits_sec_max,
                             uint32 flags)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_cosq_port_bandwidth_set(bcm_unit, bcm_port, cosq,
                                       kbits_sec_min, kbits_sec_max, flags);
}

int
bcmx_cosq_port_bandwidth_get(bcmx_lport_t port, bcm_cos_queue_t cosq,
                             uint32 *kbits_sec_min, uint32 *kbits_sec_max,
                             uint32 *flags)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_cosq_port_bandwidth_get(bcm_unit, bcm_port, cosq,
                                       kbits_sec_min, kbits_sec_max, flags);
}

/*
 * Function:
 *      bcmx_cosq_discard_set
 */

int
bcmx_cosq_discard_set(uint32 flags)
{
    int rv = BCM_E_NONE, tmp_rv;
    int i, bcm_unit;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_cosq_discard_set(bcm_unit, flags);
        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
    }

    return rv;
}

/*
 * Function:
 *      bcmx_cosq_discard_get
 */

int
bcmx_cosq_discard_get(uint32 *flags)
{
    int rv, i, bcm_unit;

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_cosq_discard_get(bcm_unit, flags);
        if (rv >= 0) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

int
bcmx_cosq_discard_port_set(bcmx_lport_t port,
                           bcm_cos_queue_t cosq,
                           uint32 color,
                           int drop_start,
                           int drop_slope,
                           int average_time)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_cosq_discard_port_set(bcm_unit, bcm_port, cosq, color,
                                     drop_start, drop_slope, average_time);
}

int
bcmx_cosq_discard_port_get(bcmx_lport_t port,
                           bcm_cos_queue_t cosq,
                           uint32 color,
                           int *drop_start,
                           int *drop_slope,
                           int *average_time)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_cosq_discard_port_get(bcm_unit, bcm_port, cosq, color,
                                     drop_start, drop_slope, average_time);
}


/*
 * Function:
 *     bcmx_cosq_gport_add
 */
int
bcmx_cosq_gport_add(bcm_gport_t port, int numq, uint32 flags,
                    bcm_gport_t *gport)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PARAM_NULL_CHECK(gport);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_cosq_gport_add(bcm_unit, port, numq, flags, gport);
}

/*
 * Function:
 *     bcmx_cosq_gport_delete
 */
int
bcmx_cosq_gport_delete(bcm_gport_t gport)
{
    int         rv = BCM_E_NONE, tmp_rv;
    int         i;
    int         bcm_unit;
    bcm_port_t  bcm_port;

    /*
     * 'gport' can be a physical local port or a system-wide value.
     */
    if (BCM_SUCCESS(_bcmx_dest_to_unit_port(gport, &bcm_unit, &bcm_port,
                                            BCMX_DEST_CONVERT_DEFAULT))) {
        return bcm_cosq_gport_delete(bcm_unit, gport);
    }

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_cosq_gport_delete(bcm_unit, gport);
        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
    }

    return rv;
}

/*
 * Function:
 *     bcmx_cosq_gport_bandwidth_set
 */
int
bcmx_cosq_gport_bandwidth_set(bcm_gport_t gport, bcm_cos_queue_t cosq, 
                              uint32 kbits_sec_min, uint32 kbits_sec_max, 
                              uint32 flags)
{
    int         rv = BCM_E_NONE, tmp_rv;
    int         i;
    int         bcm_unit;
    bcm_port_t  bcm_port;

    /*
     * 'gport' can be a physical local port or a system-wide value.
     */
    if (BCM_SUCCESS(_bcmx_dest_to_unit_port(gport, &bcm_unit, &bcm_port,
                                            BCMX_DEST_CONVERT_DEFAULT))) {
        return bcm_cosq_gport_bandwidth_set(bcm_unit, gport, cosq, 
                                            kbits_sec_min, kbits_sec_max, 
                                            flags);
    }

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_cosq_gport_bandwidth_set(bcm_unit, gport, cosq, 
                                              kbits_sec_min, kbits_sec_max, 
                                              flags);
        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
    }

    return rv;
}

/*
 * Function:
 *     bcmx_cosq_gport_bandwidth_get
 */
int
bcmx_cosq_gport_bandwidth_get(bcm_gport_t gport, bcm_cos_queue_t cosq, 
                              uint32 *kbits_sec_min, uint32 *kbits_sec_max, 
                              uint32 *flags)
{
    int         rv;
    int         i;
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PARAM_NULL_CHECK(kbits_sec_min);
    BCMX_PARAM_NULL_CHECK(kbits_sec_max);
    BCMX_PARAM_NULL_CHECK(flags);

    /*
     * 'gport' can be a physical local port or a system-wide value.
     */
    if (BCM_SUCCESS(_bcmx_dest_to_unit_port(gport, &bcm_unit, &bcm_port,
                                            BCMX_DEST_CONVERT_DEFAULT))) {
        return bcm_cosq_gport_bandwidth_get(bcm_unit, gport, cosq, 
                                            kbits_sec_min, kbits_sec_max, 
                                            flags);
    }

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_cosq_gport_bandwidth_get(bcm_unit, gport, cosq, 
                                          kbits_sec_min, kbits_sec_max, 
                                          flags);
        if (BCM_SUCCESS(rv)) {
            return rv;
        }
    }

    return BCM_E_NOT_FOUND;
}

/*
 * Function:
 *     bcmx_cosq_gport_sched_set
 */
int
bcmx_cosq_gport_sched_set(bcm_gport_t gport, bcm_cos_queue_t cosq, 
                          int mode, int weight)
{
    int         rv = BCM_E_NONE, tmp_rv;
    int         i;
    int         bcm_unit;
    bcm_port_t  bcm_port;

    /*
     * 'gport' can be a physical local port or a system-wide value.
     */
    if (BCM_SUCCESS(_bcmx_dest_to_unit_port(gport, &bcm_unit, &bcm_port,
                                            BCMX_DEST_CONVERT_DEFAULT))) {
        return bcm_cosq_gport_sched_set(bcm_unit, gport, cosq, 
                                        mode, weight);
    }

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_cosq_gport_sched_set(bcm_unit, gport, cosq, 
                                          mode, weight);
        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
    }

    return rv;
}

/*
 * Function:
 *     bcmx_cosq_gport_sched_get
 */
int
bcmx_cosq_gport_sched_get(bcm_gport_t gport, bcm_cos_queue_t cosq, 
                          int *mode, int *weight)
{
    int         rv;
    int         i;
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PARAM_NULL_CHECK(mode);
    BCMX_PARAM_NULL_CHECK(weight);

    /*
     * 'gport' can be a physical local port or a system-wide value.
     */
    if (BCM_SUCCESS(_bcmx_dest_to_unit_port(gport, &bcm_unit, &bcm_port,
                                            BCMX_DEST_CONVERT_DEFAULT))) {
        return bcm_cosq_gport_sched_get(bcm_unit, gport, cosq, 
                                        mode, weight);
    }

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_cosq_gport_sched_get(bcm_unit, gport, cosq, 
                                      mode, weight);
        if (BCM_SUCCESS(rv)) {
            return rv;
        }
    }

    return BCM_E_NOT_FOUND;
}

/*
 * Function:
 *     bcmx_cosq_gport_attach
 */
int
bcmx_cosq_gport_attach(bcm_gport_t sched_port, bcm_gport_t input_port, 
                       bcm_cos_queue_t cosq)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    /*
     * 'sched_port' is a system-wide value (scheduler)
     * 'input_port' is a physical local port
     */
    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(input_port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_cosq_gport_attach(bcm_unit, sched_port, input_port, cosq);
}

/*
 * Function:
 *     bcmx_cosq_gport_detach
 */
int
bcmx_cosq_gport_detach(bcm_gport_t sched_port, bcm_gport_t input_port, 
                       bcm_cos_queue_t cosq)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    /*
     * 'sched_port' is a system-wide value (scheduler)
     * 'input_port' is a physical local port
     */
    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(input_port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_cosq_gport_detach(bcm_unit, sched_port, input_port, cosq);
}

/*
 * Function:
 *     bcmx_cosq_gport_attach_get
 */
int
bcmx_cosq_gport_attach_get(bcm_gport_t sched_port, bcm_gport_t *input_port, 
                           bcm_cos_queue_t *cosq)
{
    int         rv;
    int         i;
    int         bcm_unit;

    BCMX_PARAM_NULL_CHECK(input_port);
    BCMX_PARAM_NULL_CHECK(cosq);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_cosq_gport_attach_get(bcm_unit, sched_port, input_port, 
                                       cosq);
        if (BCM_SUCCESS(rv)) {
            return rv;
        }
    }

    return BCM_E_NOT_FOUND;
}


/*
 * Function:
 *     bcmx_cosq_gport_discard_t_init
 */
void
bcmx_cosq_gport_discard_t_init(bcmx_cosq_gport_discard_t *discard)
{
    if (discard != NULL) {
        bcm_cosq_gport_discard_t_init
            (BCMX_COSQ_GPORT_DISCARD_T_PTR_TO_BCM(discard));
    }
}

/*
 * Function:
 *     bcmx_cosq_gport_discard_set
 */
int
bcmx_cosq_gport_discard_set(bcm_gport_t gport, bcm_cos_queue_t cosq, 
                            bcmx_cosq_gport_discard_t *discard)
{
    int         rv = BCM_E_NONE, tmp_rv;
    int         i;
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PARAM_NULL_CHECK(discard);

    /*
     * If gport is a valid BCMX logical port, then gport refers to
     * a specific physical port (mod/port or dev/port).
     * Else, gport is a system-wide value.
     */
    if (BCM_SUCCESS(_bcmx_dest_to_unit_port(gport, &bcm_unit, &bcm_port,
                                            BCMX_DEST_CONVERT_DEFAULT))) {
        return bcm_cosq_gport_discard_set(bcm_unit, gport, cosq,
                                          BCMX_COSQ_GPORT_DISCARD_T_PTR_TO_BCM
                                          (discard));
    }

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_cosq_gport_discard_set(bcm_unit, gport, cosq,
                                            BCMX_COSQ_GPORT_DISCARD_T_PTR_TO_BCM
                                            (discard));
        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
    }

    return rv;
}

/*
 * Function:
 *     bcmx_cosq_gport_discard_get
 */
int
bcmx_cosq_gport_discard_get(bcm_gport_t gport, bcm_cos_queue_t cosq, 
                            bcmx_cosq_gport_discard_t *discard)
{
    int         rv;
    int         i;
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PARAM_NULL_CHECK(discard);

    /*
     * If gport is a valid BCMX logical port, then gport refers to
     * a specific physical port (mod/port or dev/port).
     * Else, gport is a system-wide value.
     */
    if (BCM_SUCCESS(_bcmx_dest_to_unit_port(gport, &bcm_unit, &bcm_port,
                                            BCMX_DEST_CONVERT_DEFAULT))) {
        return bcm_cosq_gport_discard_get(bcm_unit, gport, cosq,
                                          BCMX_COSQ_GPORT_DISCARD_T_PTR_TO_BCM
                                          (discard));
    }

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_cosq_gport_discard_get(bcm_unit, gport, cosq,
                                        BCMX_COSQ_GPORT_DISCARD_T_PTR_TO_BCM
                                        (discard));
        if (BCM_SUCCESS(rv)) {
            return rv;
        }
    }

    return BCM_E_NOT_FOUND;
}
