/*
 * $Id: enduro_cosq.c,v 1.1 2011/04/18 17:11:02 mruas Exp $
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
 *
 */

#include <sal/core/libc.h>

#include <soc/defs.h>
#if defined(BCM_ENDURO_SUPPORT)
#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/profile_mem.h>
#include <soc/debug.h>

#include <bcm/error.h>
#include <bcm/cosq.h>

#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/strata.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/enduro.h>

#include <bcm_int/esw_dispatch.h>

#define EN_DRR_WEIGHT_MAX       0x7f
#define EN_WRR_WEIGHT_MAX       0x7f

#define EN_PACKET_FIELD_MAX     0x1fff
#define EN_CELL_FIELD_MAX       0x3fff

#define EN_PKT_REFRESH_MAX      0xfffff
#define EN_PKT_THD_MAX          0x7ff

/* COSQ where output of hierarchical COSQs scheduler is attached */
#define EN_SCHEDULER_COSQ       8

/* MMU cell size in bytes */
#define _BCM_EN_COSQ_CELLSIZE   128

/* Cache of COS_MAP Profile Table */
STATIC soc_profile_mem_t *_en_cos_map_profile[BCM_MAX_NUM_UNITS] = {NULL};

/* Number of COSQs configured for this device */
STATIC int _en_num_cosq[SOC_MAX_NUM_DEVICES];

/* Port Bitmap which includes ports capable of hierarchical queueing */
STATIC soc_pbmp_t _en_cosq_24q_ports[SOC_MAX_NUM_DEVICES];

/* Array to keep track of the number of hierarchical COSQs added per port */
STATIC uint8 *_en_num_port_cosq[SOC_MAX_NUM_DEVICES] = {NULL};

/* Forward declarations */
STATIC int _bcm_en_cosq_gport_delete(int unit, bcm_port_t port);
STATIC int _bcm_en_cosq_port_sched_set(int unit, soc_reg_t config_reg, 
                                       soc_reg_t weight_reg, bcm_port_t port,
                                       bcm_cos_queue_t start_cosq,
                                       int num_weights, const int weights[], 
                                       int mode);
STATIC int _bcm_en_cosq_discard_set(int unit, bcm_port_t port, uint32 flags,
                                    bcm_cos_queue_t cosq, soc_reg_t config_reg,
                                    soc_reg_t thresh_reg_green,
                                    soc_reg_t thresh_reg_yellow,
                                    soc_reg_t thresh_reg_red,
                                    soc_reg_t thresh_reg_non_tcp,
                                    uint32 min_thresh, uint32 max_thresh,
                                    int drop_probability, int gain);

/*
 * Convert the number of kbytes (1024 bytes) that can transmtted in one run
 * to weight encoding as per table above
 */
static int
_bcm_en_cos_drr_kbytes_to_weight(int kbytes)
{
    int weight;

    /* 1 weight-unit ~ 128 bytes */
    weight = 8 * kbytes;

    if (weight > EN_DRR_WEIGHT_MAX) {
        weight = EN_DRR_WEIGHT_MAX;
    }

    return weight;
}

/*
 * Convert the encoded weights to number of kbytes that can transmtted
 * in one run.
 */
static int
_bcm_en_cos_drr_weight_to_kbytes(int weight)
{
    assert(weight <= EN_DRR_WEIGHT_MAX);

    /* 1 weight-unit ~ 128 bytes */
    return (weight / 8);
}

/*
 * Function:
 *      bcm_cosq_init
 * Purpose:
 *      Initialize (clear) all COS schedule/mapping state.
 * Parameters:
 *      unit - StrataSwitch unit number.
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_en_cosq_init(int unit)
{
    STATIC int _en_max_cosq = -1;
    int num_cos, port_count = 44;

    if (_en_max_cosq < 0) {
        _en_max_cosq = NUM_COS(unit);
        NUM_COS(unit) = 8;
    }

    BCM_IF_ERROR_RETURN (bcm_en_cosq_detach(unit, 0));

    num_cos = soc_property_get(unit, spn_BCM_NUM_COS, BCM_COS_DEFAULT);

    if (num_cos < 1) {
        num_cos = 1;
    } else if (num_cos > 8) {
        num_cos = 8;
    }

    if (SOC_IS_TR_VL(unit)) {
        SOC_PBMP_CLEAR(_en_cosq_24q_ports[unit]);
        /* Keep track of ports that support 24 (16 + 8) queues */
        SOC_PBMP_PORT_ADD(_en_cosq_24q_ports[unit], 26);
        SOC_PBMP_PORT_ADD(_en_cosq_24q_ports[unit], 27);
        SOC_PBMP_PORT_ADD(_en_cosq_24q_ports[unit], 28);
        SOC_PBMP_PORT_ADD(_en_cosq_24q_ports[unit], 29);
        if (_en_num_port_cosq[unit] == NULL) {
            _en_num_port_cosq[unit] = sal_alloc(sizeof(uint8) * port_count,
                                         "_en_num_port_cosq");
        }
        sal_memset(_en_num_port_cosq[unit], 0, sizeof(uint8) * port_count);
    }

    return bcm_en_cosq_config_set(unit, num_cos);
}

/*
 * Function:
 *      bcm_cosq_detach
 * Purpose:
 *      Discard all COS schedule/mapping state.
 * Parameters:
 *      unit - StrataSwitch unit number.
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_en_cosq_detach(int unit, int software_state_only)
{
    bcm_port_t port;
    int weights[8];
    int cosq;

    if (SOC_IS_TR_VL(unit)) {
        /* Delete any gports that were added */
        if (_en_num_port_cosq[unit]) {
            BCM_PBMP_ITER(_en_cosq_24q_ports[unit], port) {
                if (_en_num_port_cosq[unit][port])
                {
                    if (!software_state_only)
                    {
                        BCM_IF_ERROR_RETURN
                            (_bcm_en_cosq_gport_delete(unit, port));
                    }
                }
            } 
            sal_free(_en_num_port_cosq[unit]);
            _en_num_port_cosq[unit] = NULL;
        }
    }

    for (cosq = 0; cosq < 8; cosq++) {
        weights[cosq] = 0;
    }

    if (!software_state_only)
    {
        PBMP_ALL_ITER(unit, port) {
            if (IS_CPU_PORT(unit, port)) {
                continue;
            }

            {
                /* Clear bandwidth settings on port */
                for (cosq = 0; cosq < 8; cosq++) {
                     BCM_IF_ERROR_RETURN
                        (bcm_en_cosq_port_bandwidth_set(unit, port, cosq, 0, 0, 0, 0));
                }
            }
            /* Clear scheduling settings on port */
            cosq = 8;
            BCM_IF_ERROR_RETURN
                (_bcm_en_cosq_port_sched_set(unit, ESCONFIGr, COSWEIGHTSr,
                                             port, 0, cosq, weights, 
                                             BCM_COSQ_WEIGHTED_ROUND_ROBIN));
            /* Clear discard settings on port */
     
            if (SOC_IS_TR_VL(unit)) {
                for (cosq = 0; cosq < 8; cosq++) {
                     BCM_IF_ERROR_RETURN
                        (_bcm_en_cosq_discard_set(unit, port, 
                                              BCM_COSQ_DISCARD_COLOR_ALL, cosq,
                                              WREDCONFIG_PACKETr,
                                              WREDPARAM_PACKETr,
                                              WREDPARAM_YELLOW_PACKETr,
                                              WREDPARAM_RED_PACKETr,
                                              WREDPARAM_NONTCP_PACKETr,
                                              EN_PACKET_FIELD_MAX, 
                                              EN_PACKET_FIELD_MAX, 100, 0));
                }
            }
        }

        {
            /* Disable WRED on all cosq level */
            BCM_IF_ERROR_RETURN (bcm_en_cosq_discard_set(unit, 0));
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_cosq_config_set
 * Purpose:
 *      Set the number of COS queues
 * Parameters:
 *      unit - Unit number.
 *      numq - number of COS queues (1-8).
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_en_cosq_config_set(int unit, int numq)
{
    int cos, prio, ratio, remain;
    uint32 index;
    soc_mem_t mem = PORT_COS_MAPm; 
    bcm_pbmp_t ports;
    bcm_port_t port;
    port_cos_map_entry_t cos_map_array[16];
    void *entries[16];

    /* Map the eight 802.1 priority levels to the active cosqs */
    if (numq > 8) {
        numq = 8;
    }

    sal_memset(cos_map_array, 0, 16 * sizeof(port_cos_map_entry_t));

    if (_en_cos_map_profile[unit] == NULL) {
        _en_cos_map_profile[unit] = sal_alloc(sizeof(soc_profile_mem_t),
                                          "COS_MAP Profile Mem");
        if (_en_cos_map_profile[unit] == NULL) {
            return BCM_E_MEMORY;
        }
        soc_profile_mem_t_init(_en_cos_map_profile[unit]);
    }

    /* Create profile table cache (or re-init if it already exists) */
    SOC_IF_ERROR_RETURN(soc_profile_mem_create(unit, &mem, 1,
                                               _en_cos_map_profile[unit]));

    ratio = 8 / numq;
    remain = 8 % numq;
    cos = 0;
    for (prio = 0; prio < 8; prio++) {
        soc_mem_field32_set(unit, PORT_COS_MAPm,
                            &cos_map_array[prio], COSf, cos);
        if (soc_mem_field_valid(unit,PORT_COS_MAPm,HG_COSf)) {
            soc_mem_field32_set(unit, PORT_COS_MAPm,
                                &cos_map_array[prio], HG_COSf, 0);
        }
        if ((prio + 1) == (((cos + 1) * ratio) +
                           ((remain < (numq - cos)) ? 0 :
                            (remain - (numq - cos) + 1)))) {
            cos++;
        }
        entries[prio] = (void *) &cos_map_array[prio];
    }

    /* Map remaining internal priority levels to highest priority cosq */
    cos = numq - 1;
    for (prio = 8; prio < 16; prio++) {
        soc_mem_field32_set(unit, PORT_COS_MAPm,
                            &cos_map_array[prio], COSf, cos);
        if (soc_mem_field_valid(unit,PORT_COS_MAPm,HG_COSf)) {
            soc_mem_field32_set(unit, PORT_COS_MAPm,
                            &cos_map_array[prio], HG_COSf, 0);
        }
        entries[prio] = (void *) &cos_map_array[prio];
    }

    /* Add a profile mem entry for each port */
    BCM_PBMP_ASSIGN(ports, PBMP_ALL(unit));
    PBMP_ITER(ports, port) {
        SOC_IF_ERROR_RETURN
            (soc_profile_mem_add(unit, _en_cos_map_profile[unit],
                                 (void *) &entries, 16, &index));
        SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, COS_MAP_SELr, port, 
                               SELECTf, (index / 16)));
#ifndef BCM_COSQ_HIGIG_MAP_DISABLE
        if (IS_HG_PORT(unit, port) || port == CMIC_PORT(unit)) {
            SOC_IF_ERROR_RETURN
                (soc_profile_mem_add(unit, _en_cos_map_profile[unit],
                                     (void *) &entries, 16, &index));
            SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, ICOS_MAP_SELr, port, 
                                   SELECTf, (index / 16)));
        }
#endif
    }

#ifdef BCM_COSQ_HIGIG_MAP_DISABLE
    /* identity mapping for higig ports */

    /* map prio0->cos0, prio1->cos1, ... , prio7->cos7 */
    for (prio = 0; prio < 8; prio++) {
        soc_mem_field32_set(unit, PORT_COS_MAPm,
                            &cos_map_array[prio], COSf, prio);
        if (soc_mem_field_valid(unit,PORT_COS_MAPm,HG_COSf)) {
            soc_mem_field32_set(unit, PORT_COS_MAPm,
                            &cos_map_array[prio], HG_COSf, 0);
        }
    }
    /* Map remaining internal priority levels to highest priority cosq */
    for (prio = 8; prio < 16; prio++) {
        soc_mem_field32_set(unit, PORT_COS_MAPm,
                            &cos_map_array[prio], COSf, 7);
        if (soc_mem_field_valid(unit,PORT_COS_MAPm,HG_COSf)) {
            soc_mem_field32_set(unit, PORT_COS_MAPm,
                            &cos_map_array[prio], HG_COSf, 0);
        }
    }
    PBMP_ITER(ports, port) {
        if (IS_HG_PORT(unit, port) || port == CMIC_PORT(unit)) {
            SOC_IF_ERROR_RETURN
                (soc_profile_mem_add(unit, _en_cos_map_profile[unit],
                                     (void *) &entries, 16, &index));
            soc_reg_field32_modify(unit, ICOS_MAP_SELr, port, 
                                   SELECTf, (index / 16));
        }
    }
#endif

    _en_num_cosq[unit] = numq;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_cosq_config_get
 * Purpose:
 *      Get the number of cos queues
 * Parameters:
 *      unit - StrataSwitch unit number.
 *      numq - (Output) number of cosq
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_en_cosq_config_get(int unit, int *numq)
{
    if (_en_num_cosq[unit] == 0) {
        return BCM_E_INIT;
    }

    if (numq != NULL) {
        *numq = _en_num_cosq[unit];
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_cosq_mapping_set
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
bcm_en_cosq_mapping_set(int unit, bcm_port_t port,
                        bcm_cos_t priority, bcm_cos_queue_t cosq)
{
    uint32 val, old_index, new_index;
    int i;
    port_cos_map_entry_t cos_map_array[16], *entry_p;
    void *entries[16];
    bcm_pbmp_t ports;

    if (priority < 0 || priority >= 8) {
        return (BCM_E_PARAM);
    }

    if (cosq < 0 || cosq >= 8) {
        return (BCM_E_PARAM);
    }

    if (port == -1) {	/* all ports */
        BCM_PBMP_ASSIGN(ports, PBMP_ALL(unit));
    } else if (SOC_PORT_VALID(unit, port) && IS_ALL_PORT(unit, port)) {
        BCM_PBMP_CLEAR(ports);
        BCM_PBMP_PORT_ADD(ports, port);
    } else {
        return BCM_E_PORT;
    }

    PBMP_ITER(ports, port) {
        SOC_IF_ERROR_RETURN(READ_COS_MAP_SELr(unit, port, &val));
        old_index = soc_reg_field_get(unit, COS_MAP_SELr, val, SELECTf);
        old_index *= 16;

        /* get current mapping profile values */
        for (i = 0; i < 16; i++) {
            entry_p = SOC_PROFILE_MEM_ENTRY(unit, _en_cos_map_profile[unit],
                                            port_cos_map_entry_t *, (old_index + i));
            memcpy(&cos_map_array[i], entry_p, sizeof(*entry_p));
            entries[i] = (void *) &cos_map_array[i];
        }
        soc_mem_field32_set(unit, PORT_COS_MAPm,
                            &cos_map_array[priority], COSf, cosq);
        SOC_IF_ERROR_RETURN
            (soc_profile_mem_add(unit, _en_cos_map_profile[unit],
                                 (void *) &entries, 16, &new_index));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, COS_MAP_SELr, port, 
                                    SELECTf, new_index / 16));
        SOC_IF_ERROR_RETURN
            (soc_profile_mem_delete(unit, _en_cos_map_profile[unit], old_index));

#ifndef BCM_COSQ_HIGIG_MAP_DISABLE
        if (IS_HG_PORT(unit, port) || port == CMIC_PORT(unit)) {
            SOC_IF_ERROR_RETURN
                (soc_profile_mem_add(unit, _en_cos_map_profile[unit],
                                     (void *) &entries, 16, &new_index));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, ICOS_MAP_SELr, port, 
                                        SELECTf, new_index / 16));
            SOC_IF_ERROR_RETURN
                (soc_profile_mem_delete(unit, _en_cos_map_profile[unit], old_index));
        }
#endif
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_cosq_mapping_get
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
bcm_en_cosq_mapping_get(int unit, bcm_port_t port,
                        bcm_cos_t priority, bcm_cos_queue_t *cosq)
{
    uint32 val;
    int index;
    port_cos_map_entry_t *entry_p;

    if (priority < 0 || priority >= 8) {
        return (BCM_E_PARAM);
    }

    if (port == -1) {
        port = REG_PORT_ANY;
    } else if (!SOC_PORT_VALID(unit, port) || !IS_ALL_PORT(unit, port)) {
        return BCM_E_PORT;
    }

    SOC_IF_ERROR_RETURN(READ_COS_MAP_SELr(unit, port, &val));
    index = soc_reg_field_get(unit, COS_MAP_SELr, val, SELECTf);
    index *= 16;

    entry_p = SOC_PROFILE_MEM_ENTRY(unit, _en_cos_map_profile[unit],
                                    port_cos_map_entry_t *, 
                                    (index + priority));
    *cosq = soc_mem_field32_get(unit, PORT_COS_MAPm, entry_p, COSf);
    return BCM_E_NONE;
}

int
bcm_en_cosq_port_bandwidth_set(int unit, bcm_port_t port,
                               bcm_cos_queue_t cosq,
                               uint32 kbits_sec_min,
                               uint32 kbits_sec_max,
                               uint32 kbits_sec_burst,
                               uint32 flags)
{
    uint32 regval;
    uint32 bucket_val = 0;
    uint64 regval_64;
    uint32 refresh_rate, bucketsize, granularity = 3, meter_flags = 0;
    int    refresh_bitsize, bucket_bitsize;

    /*
     * To set the new Bandwidth settings, the procedure adopted is
     * a. reset MAXBUCKETCONFIG, MINBUCKETCONFIG, MAXBUCKET,MINBUCKET
     * b. update MAXBUCKETCONFIG and MINBUCKETCONFIG with new values passed
     * c. if MISCCONFIG.METERING_CLK_EN not set before, enable it.
     */

    BCM_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &regval));
    if (soc_reg_field_get(unit, MISCCONFIGr, regval, ITU_MODE_SELf)) {
        meter_flags |= _BCM_XGS_METER_FLAG_NON_LINEAR;
    }
    meter_flags |= _BCM_XGS_METER_FLAG_GRANULARITY;

    /* Disable egress metering for this port */
    BCM_IF_ERROR_RETURN(READ_MAXBUCKETCONFIG_64r(unit, port, cosq, &regval_64));
    soc_reg64_field32_set(unit, MAXBUCKETCONFIG_64r, &regval_64, MAX_REFRESHf, 0);
    soc_reg64_field32_set(unit, MAXBUCKETCONFIG_64r, &regval_64, MAX_THDf, 0);
    BCM_IF_ERROR_RETURN(WRITE_MAXBUCKETCONFIG_64r(unit, port, cosq, regval_64));

    BCM_IF_ERROR_RETURN(READ_MINBUCKETCONFIG_64r(unit, port, cosq, &regval_64));
    soc_reg64_field32_set(unit, MINBUCKETCONFIG_64r, &regval_64, MIN_REFRESHf, 0);
    soc_reg64_field32_set(unit, MINBUCKETCONFIG_64r, &regval_64, MIN_THDf, 0);
    BCM_IF_ERROR_RETURN(WRITE_MINBUCKETCONFIG_64r(unit, port, cosq, regval_64));

    /*reset the MAXBUCKET register*/
    soc_reg_field_set(unit, MAXBUCKETr, &bucket_val, MAX_BUCKETf, 0);
    soc_reg_field_set(unit, MAXBUCKETr, &bucket_val, OUT_PROFILE_FLAGf, 0);
    BCM_IF_ERROR_RETURN(WRITE_MAXBUCKETr(unit, port, cosq, bucket_val));

    /*reset the MINBUCKET register value*/
    soc_reg_field_set(unit, MINBUCKETr, &bucket_val, MIN_BUCKETf, 0);
    soc_reg_field_set(unit, MINBUCKETr, &bucket_val, OUT_PROFILE_FLAGf, 0);
    BCM_IF_ERROR_RETURN(WRITE_MINBUCKETr(unit, port, cosq, bucket_val));

    refresh_bitsize =
        soc_reg_field_length(unit, MINBUCKETCONFIG_64r, MIN_REFRESHf);
    bucket_bitsize =
        soc_reg_field_length(unit, MINBUCKETCONFIG_64r, MIN_THDf);

    BCM_IF_ERROR_RETURN
        (_bcm_xgs_kbits_to_bucket_encoding(kbits_sec_min, kbits_sec_min,
                          meter_flags, refresh_bitsize, bucket_bitsize,
                          &refresh_rate, &bucketsize, &granularity));

    COMPILER_64_ZERO(regval_64);
    
    soc_reg64_field32_set(unit, MINBUCKETCONFIG_64r, &regval_64,
                          METER_GRANf, granularity);
    soc_reg64_field32_set(unit, MINBUCKETCONFIG_64r, &regval_64,
                          MIN_REFRESHf, refresh_rate);
    soc_reg64_field32_set(unit, MINBUCKETCONFIG_64r,
                          &regval_64, MIN_THDf, bucketsize);
    BCM_IF_ERROR_RETURN
        (WRITE_MINBUCKETCONFIG_64r(unit, port, cosq, regval_64));

    refresh_bitsize =
        soc_reg_field_length(unit, MAXBUCKETCONFIG_64r, MAX_REFRESHf);
    bucket_bitsize =
        soc_reg_field_length(unit, MAXBUCKETCONFIG_64r, MAX_THDf);

    BCM_IF_ERROR_RETURN
        (_bcm_xgs_kbits_to_bucket_encoding(kbits_sec_max, kbits_sec_burst,
                          meter_flags, refresh_bitsize, bucket_bitsize,
                          &refresh_rate, &bucketsize, &granularity));

    COMPILER_64_ZERO(regval_64);
    soc_reg64_field32_set(unit, MAXBUCKETCONFIG_64r, &regval_64,
                          METER_GRANf, granularity);
    soc_reg64_field32_set(unit, MAXBUCKETCONFIG_64r, &regval_64,
                          MAX_REFRESHf, refresh_rate);
    soc_reg64_field32_set(unit, MAXBUCKETCONFIG_64r, &regval_64, MAX_THDf,
                          bucketsize);
    BCM_IF_ERROR_RETURN(WRITE_MAXBUCKETCONFIG_64r(unit, port, cosq, regval_64));

    /* MISCCONFIG.METERING_CLK_EN is set by chip init */

    return BCM_E_NONE;
}

int
_bcm_en_cosq_port_packet_bandwidth_set(int unit, bcm_port_t port,
                                       bcm_cos_queue_t cosq,
                                       int pps, int burst)
{
    uint32 regval, max_config_addr, max_addr;
    soc_reg_t maxbucket_config_reg, maxbucket_reg;
    soc_field_t refresh_f, thd_sel_f, bucket_f;

    if (cosq < 0) {
        maxbucket_config_reg = CPUPORTMAXBUCKETCONFIG_64r;
        maxbucket_reg = CPUPORTMAXBUCKETr;
        refresh_f = MAX_REFRESHf;
        thd_sel_f = MAX_THD_SELf;
        bucket_f = MAX_BUCKETf;
        cosq = 0;
    } else {
        maxbucket_config_reg = CPUMAXBUCKETCONFIG_64r;
        maxbucket_reg = CPUMAXBUCKETr;
        refresh_f = MAX_REFRESHf;
        thd_sel_f = MAX_THDf;
        bucket_f = PKT_MAX_BUCKETf;
    }

    /*
     * To set the new Bandwidth settings, the procedure adopted is
     * a. reset MAXBUCKETCONFIG and MAXBUCKET
     * b. update MAXBUCKETCONFIG with new values passed
     * c. if MISCCONFIG.METERING_CLK_EN not set before, enable it.
     */

    /* Disable egress metering */
    max_config_addr = soc_reg_addr(unit, maxbucket_config_reg, port, cosq);
    BCM_IF_ERROR_RETURN(soc_reg32_read(unit, max_config_addr, &regval));
    soc_reg_field_set(unit, maxbucket_config_reg, &regval, refresh_f, 0);
    soc_reg_field_set(unit, maxbucket_config_reg, &regval, thd_sel_f, 0);
    BCM_IF_ERROR_RETURN(soc_reg32_write(unit, max_config_addr, regval));

    /*reset the MAXBUCKET register*/
    max_addr = soc_reg_addr(unit, maxbucket_reg, port, cosq);
    BCM_IF_ERROR_RETURN(soc_reg32_read(unit, max_addr, &regval));
    soc_reg_field_set(unit, maxbucket_reg, &regval, bucket_f, 0);
    soc_reg_field_set(unit, maxbucket_reg, &regval, OUT_PROFILE_FLAGf, 0);
    BCM_IF_ERROR_RETURN(soc_reg32_write(unit, max_addr, regval));

    /* Check packets-per second upper limit */
    if (pps > EN_PKT_REFRESH_MAX) {
       pps = EN_PKT_REFRESH_MAX;
    }

    /* Check burst upper limit */
    if (burst > EN_PKT_THD_MAX) {
       burst = EN_PKT_THD_MAX;
    }

    BCM_IF_ERROR_RETURN(soc_reg32_read(unit, max_config_addr, &regval));
    soc_reg_field_set(unit, maxbucket_config_reg, &regval, refresh_f, pps);
    soc_reg_field_set(unit, maxbucket_config_reg, &regval, thd_sel_f, burst);
    BCM_IF_ERROR_RETURN(soc_reg32_write(unit, max_config_addr, regval));

    /* MISCCONFIG.METERING_CLK_EN is set by chip init */

    return BCM_E_NONE;
}

int
_bcm_en_cosq_port_packet_bandwidth_get(int unit, bcm_port_t port,
                                       bcm_cos_queue_t cosq,
                                       int *pps, int *burst)
{
    uint32 regval, max_config_addr;
    soc_reg_t maxbucket_config_reg;
    soc_field_t refresh_f, thd_sel_f;

    if (cosq < 0) {
        maxbucket_config_reg = CPUPORTMAXBUCKETCONFIG_64r;
        refresh_f = MAX_REFRESHf;
        thd_sel_f = MAX_THD_SELf;
        cosq = 0;
    } else {
        maxbucket_config_reg = CPUMAXBUCKETCONFIG_64r;
        refresh_f = MAX_REFRESHf;
        thd_sel_f = MAX_THDf;
    }

    /* Disable egress metering */
    max_config_addr = soc_reg_addr(unit, maxbucket_config_reg, port, cosq);
    BCM_IF_ERROR_RETURN(soc_reg32_read(unit, max_config_addr, &regval));
    *pps = soc_reg_field_get(unit, maxbucket_config_reg, regval, refresh_f);
    *burst = soc_reg_field_get(unit, maxbucket_config_reg, regval, thd_sel_f);

    return BCM_E_NONE;
}

int
bcm_en_cosq_port_pps_set(int unit, bcm_port_t port,
                         bcm_cos_queue_t cosq, int pps)
{
    int temp_pps, burst;

    if (!IS_CPU_PORT(unit, port)) {
        return BCM_E_PORT;
    } else if (cosq >= NUM_CPU_COSQ(unit)) {
        return BCM_E_PARAM;
    }

    /* Get the current PPS and BURST settings */
    BCM_IF_ERROR_RETURN
        (_bcm_en_cosq_port_packet_bandwidth_get(unit, port, cosq,
                                                &temp_pps, &burst));

    /* Replace the current PPS setting, keep BURST the same */
    return _bcm_en_cosq_port_packet_bandwidth_set(unit, port, cosq,
                                                  pps, burst);
}

int
bcm_en_cosq_port_pps_get(int unit, bcm_port_t port,
                         bcm_cos_queue_t cosq, int *pps)
{
    int burst;

    if (!IS_CPU_PORT(unit, port)) {
        return BCM_E_PORT;
    } else if (cosq >= NUM_CPU_COSQ(unit)) {
        return BCM_E_PARAM;
    }

    return _bcm_en_cosq_port_packet_bandwidth_get(unit, port, cosq,
                                                  pps, &burst);
}

int
bcm_en_cosq_port_burst_set(int unit, bcm_port_t port,
                           bcm_cos_queue_t cosq, int burst)
{
    int pps, temp_burst;

    if (!IS_CPU_PORT(unit, port)) {
        return BCM_E_PORT;
    } else if (cosq >= NUM_CPU_COSQ(unit)) {
        return BCM_E_PARAM;
    }

    /* Get the current PPS and BURST settings */
    BCM_IF_ERROR_RETURN
        (_bcm_en_cosq_port_packet_bandwidth_get(unit, port, cosq,
                                                &pps, &temp_burst));

    /* Replace the current BURST setting, keep PPS the same */
    return _bcm_en_cosq_port_packet_bandwidth_set(unit, port, cosq,
                                                  pps, burst);
}

int
bcm_en_cosq_port_burst_get(int unit, bcm_port_t port,
                           bcm_cos_queue_t cosq, int *burst)
{
    int pps;

    if (!IS_CPU_PORT(unit, port)) {
        return BCM_E_PORT;
    } else if (cosq >= NUM_CPU_COSQ(unit)) {
        return BCM_E_PARAM;
    }

    return _bcm_en_cosq_port_packet_bandwidth_get(unit, port, cosq,
                                                  &pps, burst);
}

int
_bcm_en_cosq_port_resolve(int unit, bcm_gport_t gport,
                       bcm_module_t *modid, bcm_port_t *port,
                       bcm_trunk_t *trunk_id, int *id)
{
    bcm_module_t mod_in;
    bcm_port_t port_in;

    if (BCM_GPORT_IS_SCHEDULER(gport)) {
        mod_in = (BCM_GPORT_SCHEDULER_GET(gport) >> 8) & 0xff;
        port_in = BCM_GPORT_SCHEDULER_GET(gport) & 0xff;
        *id = BCM_GPORT_SCHEDULER_GET(gport);
        *trunk_id = -1;

        /* Since the modid/port in the scheduler GPORT is the
         * applications modid/port space, convert to local space.
         */
        BCM_IF_ERROR_RETURN
            (bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_SET,
                                    mod_in, port_in, modid, port));

        if (!_en_num_port_cosq[unit]) {
            return BCM_E_INIT;
        } else if (!SOC_PBMP_MEMBER(_en_cosq_24q_ports[unit], *port)) {
            return BCM_E_BADID;
        } else if (_en_num_port_cosq[unit][*port] == 0) {
            return BCM_E_NOT_FOUND;
        }
    } else {
        return BCM_E_BADID;
    }
    return BCM_E_NONE;
}

STATIC int
_bcm_en_cosq_resolve_mod_port(int unit, bcm_gport_t gport,
                              bcm_module_t *modid, bcm_port_t *port,
                              bcm_trunk_t *trunk_id, int *is_local)
{
    int gport_id;

    BCM_IF_ERROR_RETURN
        (_bcm_esw_gport_resolve(unit, gport, modid,
                                port, trunk_id, &gport_id));
    BCM_IF_ERROR_RETURN(
        _bcm_esw_modid_is_local(unit, *modid, is_local));

    return BCM_E_NONE;
}

int
bcm_en_cosq_port_bandwidth_get(int unit, bcm_port_t port,
                               bcm_cos_queue_t cosq,
                               uint32 *kbits_sec_min,
                               uint32 *kbits_sec_max,
                               uint32 *kbits_sec_burst,
                               uint32 *flags)
{
    uint32 regval;
    uint64 regval_64;
    uint32 refresh_rate = 0, bucketsize = 0,
        granularity = 3, meter_flags = 0;
    uint32 kbits_min_burst; /* Placeholder, since burst is unused for min */

    if (!kbits_sec_min || !kbits_sec_max || !kbits_sec_burst || !flags) {
        return (BCM_E_PARAM);
    }

    BCM_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &regval));
    if (soc_reg_field_get(unit, MISCCONFIGr, regval, ITU_MODE_SELf)) {
        meter_flags |= _BCM_XGS_METER_FLAG_NON_LINEAR;
    }
    meter_flags |= _BCM_XGS_METER_FLAG_GRANULARITY;

    BCM_IF_ERROR_RETURN(READ_MAXBUCKETCONFIG_64r(unit, port, cosq, &regval_64));
    granularity = soc_reg64_field32_get(unit, MAXBUCKETCONFIG_64r,
                                            regval_64, METER_GRANf);
    refresh_rate = soc_reg64_field32_get(unit, MAXBUCKETCONFIG_64r,
                                             regval_64, MAX_REFRESHf);
    bucketsize = soc_reg64_field32_get(unit, MAXBUCKETCONFIG_64r,
                                             regval_64, MAX_THDf);

    BCM_IF_ERROR_RETURN
        (_bcm_xgs_bucket_encoding_to_kbits(refresh_rate, bucketsize,
                                           granularity, meter_flags,
                                           kbits_sec_max, kbits_sec_burst));

    BCM_IF_ERROR_RETURN(READ_MINBUCKETCONFIG_64r(unit, port, cosq, &regval_64));

    granularity = soc_reg64_field32_get(unit, MINBUCKETCONFIG_64r,
                                            regval_64, METER_GRANf);
    refresh_rate = soc_reg64_field32_get(unit, MINBUCKETCONFIG_64r,
                                             regval_64, MIN_REFRESHf);

    BCM_IF_ERROR_RETURN
        (_bcm_xgs_bucket_encoding_to_kbits(refresh_rate, 0,
                                           granularity, meter_flags,
                                           kbits_sec_min, &kbits_min_burst));

    *flags = 0;

    return BCM_E_NONE;
}

STATIC int     
_bcm_en_cosq_discard_cap_enable_set(int unit, bcm_port_t port, 
                                    bcm_cos_queue_t cosq, soc_reg_t config_reg, 
                                    uint32 flags)
{
    uint32 val, addr;

    addr = soc_reg_addr(unit, config_reg, port, cosq);
    SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &val));

    if (flags & BCM_COSQ_DISCARD_CAP_AVERAGE) {
        soc_reg_field_set(unit, config_reg, &val, CAP_AVERAGEf, 1);
    } else {
        soc_reg_field_set(unit, config_reg, &val, CAP_AVERAGEf, 0);
    }
    if (flags & BCM_COSQ_DISCARD_ENABLE) {
        soc_reg_field_set(unit, config_reg, &val, ENABLEf, 1);
    } else {
        soc_reg_field_set(unit, config_reg, &val, ENABLEf, 0);
    }
    SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, val));
    return BCM_E_NONE;
}

STATIC int
_bcm_en_cosq_discard_cap_enable_get(int unit, bcm_port_t port, 
                                    bcm_cos_queue_t cosq, soc_reg_t config_reg, 
                                    uint32 *flags)
{
    uint32 val, addr;

    addr = soc_reg_addr(unit, config_reg, port, cosq);
    SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &val));

    if (soc_reg_field_get(unit, config_reg, val, CAP_AVERAGEf)) {
        *flags |= BCM_COSQ_DISCARD_CAP_AVERAGE;
    }
    if (soc_reg_field_get(unit, config_reg, val, ENABLEf)) {
        *flags |= BCM_COSQ_DISCARD_ENABLE;
    }
    return BCM_E_NONE;
}

int
bcm_en_cosq_discard_set(int unit, uint32 flags)
{
    bcm_port_t port;
    bcm_cos_queue_t cosq;
    soc_reg_t config_reg = WREDCONFIG_PACKETr;

    PBMP_ALL_ITER(unit, port) {
        if (IS_CPU_PORT(unit, port)) {
            continue;
        }
        for (cosq = 0; cosq < 8; cosq++) {
            BCM_IF_ERROR_RETURN
                (_bcm_en_cosq_discard_cap_enable_set(unit, port, cosq,
                                                     config_reg, flags));
        }
        if (SOC_PBMP_MEMBER(_en_cosq_24q_ports[unit], port)) {
            for (cosq = 8; cosq < 24; cosq++) {
                BCM_IF_ERROR_RETURN
                    (_bcm_en_cosq_discard_cap_enable_set(unit, port, cosq,
                                                         config_reg, flags));
            }
        }
    }
    return BCM_E_NONE;
}

int
bcm_en_cosq_discard_get(int unit, uint32 *flags)
{
    int rv = BCM_E_NONE;
    bcm_port_t port;
    soc_reg_t config_reg = WREDCONFIG_PACKETr;

    PBMP_ALL_ITER(unit, port) {
        if (IS_CPU_PORT(unit, port)) {
            continue;
        }
        *flags = 0;
        rv = _bcm_en_cosq_discard_cap_enable_get(unit, port, 0,
                                                 config_reg, flags);
        break;
    }
    return rv;
}

/*      
 *  Convert HW drop probability to percent value
 */     
STATIC int       
_bcm_en_hw_drop_prob_to_percent[] = {
    0,     /* 0  */
    1,     /* 1  */
    2,     /* 2  */
    3,     /* 3  */
    4,     /* 4  */
    5,     /* 5  */
    6,     /* 6  */
    7,     /* 7  */
    8,     /* 8  */
    9,     /* 9  */
    10,    /* 10 */
    25,    /* 11 */
    50,    /* 12 */
    75,    /* 13 */
    100,   /* 14 */
    -1     /* 15 */
};

STATIC int
_bcm_en_percent_to_drop_prob(int percent) {
   int i;

   for (i=14; i > 0 ; i--) {
      if (percent >= _bcm_en_hw_drop_prob_to_percent[i]) {
          break;
      }
   }
   return i;
}

STATIC int
_bcm_en_drop_prob_to_percent(int drop_prob) {
   return (_bcm_en_hw_drop_prob_to_percent[drop_prob]);
}   

STATIC int 
_bcm_en_angle_to_packets_table[] =
{
    /*  0.. 5 */  16383, 5727, 2862, 1908, 1430, 1142,
    /*  6..11 */    951,  814,  711,  631,  567,  514,
    /* 12..17 */    470,  433,  401,  373,  348,  327,
    /* 18..23 */    307,  290,  274,  260,  247,  235,
    /* 24..29 */    224,  214,  205,  196,  188,  180,
    /* 30..35 */    173,  166,  160,  153,  148,  142,
    /* 36..41 */    137,  132,  127,  123,  119,  115,
    /* 42..47 */    111,  107,  103,  100,   96,   93,
    /* 48..53 */     90,   86,   83,   80,   78,   75,
    /* 54..59 */     72,   70,   67,   64,   62,   60,
    /* 60..65 */     57,   55,   53,   50,   48,   46,
    /* 66..71 */     44,   42,   40,   38,   36,   34,
    /* 72..77 */     32,   30,   28,   26,   24,   23,
    /* 78..83 */     21,   19,   17,   15,   14,   12,
    /* 84..89 */     10,    8,    6,    5,    3,    1,
    /* 90     */      0
};

/* 
 * Given a slope (angle in degrees) from 0 to 90, return
 * the number of packets in the range from 0% drop
 * probability to 100% drop probability.
 */
STATIC int
_bcm_en_angle_to_packets(int angle) {
    return (_bcm_en_angle_to_packets_table[angle]);
}

/*
 * Given a number of packets in the range from 0% drop probability
 * to 100% drop probability, return the slope (angle in degrees).
 */
STATIC int
_bcm_en_packets_to_angle(int packets) {
    int angle;

    for (angle = 90; angle >= 0 ; angle--) {
        if (packets <= _bcm_en_angle_to_packets_table[angle]) {
            break;
        }
    }
    return angle;
}

/*
 *  Convert exponent 0..15 to usec using:
 *   value = 1usec * 2 ^ exponent
 */
STATIC int
_bcm_en_hw_exponent_to_usec[] = {
    1,     /* 0 */
    2,     /* 1 */
    4,     /* 2 */
    8,     /* 3 */
    16,    /* 4 */
    32,    /* 5 */
    64,    /* 6 */
    128,   /* 7 */
    256,   /* 8 */
    512,   /* 9 */
    1024,  /* 10 */
    2048,  /* 11 */
    4096,  /* 12 */
    8192,  /* 13 */
    16384, /* 14 */
    32768, /* 15 */
};

STATIC int
_bcm_en_usec_to_exponent(int usec) {
   int i;

   for (i=15; i > 0 ; i--) {
      if (_bcm_en_hw_exponent_to_usec[i] <= usec) break;
   }
   return i;
}

STATIC int
_bcm_en_exponent_to_usec(int exp) {
   return (_bcm_en_hw_exponent_to_usec[exp]);
}

STATIC int     
_bcm_en_cosq_discard_set(int unit, bcm_port_t port, uint32 flags,
                         bcm_cos_queue_t cosq, soc_reg_t config_reg,
                         soc_reg_t thresh_reg_green,
                         soc_reg_t thresh_reg_yellow,
                         soc_reg_t thresh_reg_red,
                         soc_reg_t thresh_reg_non_tcp,
                         uint32 min_thresh, uint32 max_thresh,
                         int drop_probability, int gain)
{
    uint32 val, addr;

    addr = soc_reg_addr(unit, config_reg, port, cosq);
    SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &val));

    /* Program the weight */
    soc_reg_field_set(unit, config_reg, &val, WEIGHTf, gain);

    /* Program the drop probabilty */
    if ((flags & BCM_COSQ_DISCARD_COLOR_GREEN) ||
        ((flags & BCM_COSQ_DISCARD_COLOR_ALL) == BCM_COSQ_DISCARD_COLOR_ALL)) {
        soc_reg_field_set(unit, config_reg, &val, MAXDROPRATEf,
            _bcm_en_percent_to_drop_prob(drop_probability));
    }
    if ((flags & BCM_COSQ_DISCARD_COLOR_YELLOW) ||
        ((flags & BCM_COSQ_DISCARD_COLOR_ALL) == BCM_COSQ_DISCARD_COLOR_ALL)) {
        soc_reg_field_set(unit, config_reg, &val, YELLOW_MAXDROPRATEf,
            _bcm_en_percent_to_drop_prob(drop_probability));
    }
    if ((flags & BCM_COSQ_DISCARD_COLOR_RED) ||
        ((flags & BCM_COSQ_DISCARD_COLOR_ALL) == BCM_COSQ_DISCARD_COLOR_ALL)) {
        soc_reg_field_set(unit, config_reg, &val, RED_MAXDROPRATEf,
            _bcm_en_percent_to_drop_prob(drop_probability));
    }
    SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, val));

    /* Program the min and max thresholds */
    if ((flags & BCM_COSQ_DISCARD_COLOR_GREEN) ||
        ((flags & BCM_COSQ_DISCARD_COLOR_ALL) == BCM_COSQ_DISCARD_COLOR_ALL)) {
        addr = soc_reg_addr(unit, thresh_reg_green, port, cosq);
        SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &val));
        soc_reg_field_set(unit, thresh_reg_green, &val, DROPSTARTPOINTf,
                          min_thresh);
        soc_reg_field_set(unit, thresh_reg_green, &val, DROPENDPOINTf,
                          max_thresh);
        SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, val));
    }
    if ((flags & BCM_COSQ_DISCARD_COLOR_YELLOW) ||
        ((flags & BCM_COSQ_DISCARD_COLOR_ALL) == BCM_COSQ_DISCARD_COLOR_ALL)) {
        addr = soc_reg_addr(unit, thresh_reg_yellow, port, cosq);
        SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &val));
        soc_reg_field_set(unit, thresh_reg_yellow, &val,
                          YELLOW_DROPSTARTPOINTf, min_thresh);
        soc_reg_field_set(unit, thresh_reg_yellow, &val,
                          YELLOW_DROPENDPOINTf, max_thresh);
        SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, val));
    }
    if ((flags & BCM_COSQ_DISCARD_COLOR_RED) ||
        ((flags & BCM_COSQ_DISCARD_COLOR_ALL) == BCM_COSQ_DISCARD_COLOR_ALL)) {
        addr = soc_reg_addr(unit, thresh_reg_red, port, cosq);
        SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &val));
        soc_reg_field_set(unit, thresh_reg_red, &val,
                          RED_DROPSTARTPOINTf, min_thresh);
        soc_reg_field_set(unit, thresh_reg_red, &val,
                          RED_DROPENDPOINTf, max_thresh);
        SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, val));
    }

    return BCM_E_NONE;
}

STATIC int     
_bcm_en_cosq_discard_get(int unit, bcm_port_t port, uint32 flags,
                         bcm_cos_queue_t cosq, soc_reg_t config_reg,
                         soc_reg_t thresh_reg_green,
                         soc_reg_t thresh_reg_yellow,
                         soc_reg_t thresh_reg_red,
                         soc_reg_t thresh_reg_non_tcp,
                         uint32 *min_thresh, uint32 *max_thresh,
                         int *drop_probability, int *gain)
{
    uint32 val, addr, drop_rate;

    addr = soc_reg_addr(unit, config_reg, port, cosq);
    SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &val));

    /* Get the weight */
    *gain = soc_reg_field_get(unit, config_reg, val, WEIGHTf);

    /* Get the drop probabilty */
    if (flags & BCM_COSQ_DISCARD_COLOR_YELLOW) {
        drop_rate = soc_reg_field_get(unit, config_reg, val,
                                      YELLOW_MAXDROPRATEf);
    } else if (flags & BCM_COSQ_DISCARD_COLOR_RED) {
        drop_rate = soc_reg_field_get(unit, config_reg, val,
                                      RED_MAXDROPRATEf);
    } else {
        drop_rate = soc_reg_field_get(unit, config_reg, val,
                                      MAXDROPRATEf);
    }
    *drop_probability = _bcm_en_drop_prob_to_percent(drop_rate);

    /* Get the min and max thresholds */
    if (flags & BCM_COSQ_DISCARD_COLOR_YELLOW) {
        addr = soc_reg_addr(unit, thresh_reg_yellow, port, cosq);
        SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &val));
        *min_thresh = soc_reg_field_get(unit, thresh_reg_yellow, val,
                                        YELLOW_DROPSTARTPOINTf);
        *max_thresh = soc_reg_field_get(unit, thresh_reg_yellow, val,
                                        YELLOW_DROPENDPOINTf);
    } else if (flags & BCM_COSQ_DISCARD_COLOR_RED) {
        addr = soc_reg_addr(unit, thresh_reg_red, port, cosq);
        SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &val));
        *min_thresh = soc_reg_field_get(unit, thresh_reg_red, val,
                                        RED_DROPSTARTPOINTf);
        *max_thresh = soc_reg_field_get(unit, thresh_reg_red, val,
                                        RED_DROPENDPOINTf);
    } else {
        addr = soc_reg_addr(unit, thresh_reg_green, port, cosq);
        SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &val));
        *min_thresh = soc_reg_field_get(unit, thresh_reg_green, val, 
                                        DROPSTARTPOINTf);
        *max_thresh = soc_reg_field_get(unit, thresh_reg_green, val, 
                                        DROPENDPOINTf);
    }
    return BCM_E_NONE;
}

#define ABS(n) (((n) < 0) ? -(n) : (n))

int
bcm_en_cosq_discard_port_set(int unit, bcm_port_t port,
                             bcm_cos_queue_t cosq,
                             uint32 color,
                             int drop_start,
                             int drop_slope,
                             int average_time)
{
    bcm_module_t modid;
    bcm_port_t local_port;
    bcm_trunk_t trunk_id;
    int i, gain, is_local, cosq_start, num_cosq;
    uint32 min_thresh = 0, max_thresh = 0;

    if ((drop_slope < 0) || (drop_slope > 90)) {
        return BCM_E_PARAM;
    }
    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN
            (_bcm_en_cosq_resolve_mod_port(unit, port, &modid,
                                           &local_port, &trunk_id, &is_local));

        if (is_local) {
            if (!_en_num_port_cosq[unit]) {
                return BCM_E_INIT;
            }
            if (BCM_GPORT_IS_SCHEDULER(port)) {
                if (_en_num_port_cosq[unit][local_port] == 0) {
                    return BCM_E_NOT_FOUND;
                } else if (cosq >= _en_num_port_cosq[unit][local_port]) {
                    return BCM_E_PARAM;
                } else if (cosq < 0) {
                    cosq_start = 8;
                    num_cosq = _en_num_port_cosq[unit][local_port];
                } else {
                    cosq_start = cosq + 8;
                    num_cosq = 1;
                }
            } else {
                if (cosq >= _en_num_cosq[unit]) {
                    return BCM_E_PARAM;
                } else if (cosq < 0) {
                    cosq_start = 0;
                    num_cosq = 8;
                } else {
                    cosq_start = cosq;
                    num_cosq = 1;
                }
            }
        } else {
            return BCM_E_PORT;
        }
    } else {
        local_port = port;
        if (cosq >= _en_num_cosq[unit]) {
            return BCM_E_PARAM;
        } else if (cosq < 0) {
            cosq_start = 0;
            num_cosq = 8;
        } else {
            cosq_start = cosq;
            num_cosq = 1;
        }
    }

    /* Get the exponent (gain) used in the average queue size calculataion */
    gain = _bcm_en_usec_to_exponent(average_time);

    /* Since the queue size is dynamically adjusted based on 
     * buffer availability, there is no fixed queue size. Therefore
     * the "drop_start" parameter as a percent of the queue size
     * cannot be used to determine the threshold values. Instead
     * a negative value is used for "drop_start" to indicate
     * number of packets for the min threshold.
     */
    if (drop_start > 0) {
        return BCM_E_PARAM;
    } else {
        min_thresh = ABS(drop_start);
        if (min_thresh > EN_PACKET_FIELD_MAX) {
            min_thresh = EN_PACKET_FIELD_MAX;
        }
    }
    
    /* Calculate the max threshold. For a given slope (angle in 
     * degrees), determine how many packets are in the range from
     * 0% drop probability to 100% drop probability. Add that
     * number to the min_treshold to the the max_threshold.
     */
    max_thresh = min_thresh + _bcm_en_angle_to_packets(drop_slope);
    if (max_thresh > EN_PACKET_FIELD_MAX) {
        max_thresh = EN_PACKET_FIELD_MAX;
    }

    for (i = cosq_start; i < (cosq_start + num_cosq); i++) { 
        BCM_IF_ERROR_RETURN
            (_bcm_en_cosq_discard_set(unit, local_port, color, i,
                                      WREDCONFIG_PACKETr,
                                      WREDPARAM_PACKETr,
                                      WREDPARAM_YELLOW_PACKETr,
                                      WREDPARAM_RED_PACKETr,
                                      WREDPARAM_NONTCP_PACKETr,
                                      min_thresh, max_thresh, 100, gain));
    }
    return BCM_E_NONE;
}

int
bcm_en_cosq_discard_port_get(int unit, bcm_port_t port,
                             bcm_cos_queue_t cosq,
                             uint32 color,
                             int *drop_start,
                             int *drop_slope,
                             int *average_time)
{
    bcm_module_t modid;
    bcm_port_t local_port;
    bcm_trunk_t trunk_id;
    int gain, is_local, drop_prob;
    uint32 min_thresh = 0, max_thresh = 0;

    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN
            (_bcm_en_cosq_resolve_mod_port(unit, port, &modid,
                                           &local_port, &trunk_id, &is_local));

        if (is_local) {
            if (!_en_num_port_cosq[unit]) {
                return BCM_E_INIT;
            }
            if (BCM_GPORT_IS_SCHEDULER(port)) {
                if (_en_num_port_cosq[unit][local_port] == 0) {
                    return BCM_E_NOT_FOUND;
                } else if (cosq >= _en_num_port_cosq[unit][local_port]) {
                    return BCM_E_PARAM;
                } else if (cosq < 0) {
                    cosq = 8;
                } else {
                    cosq = cosq + 8;
                }
            } else {
                if (cosq >= _en_num_cosq[unit]) {
                    return BCM_E_PARAM;
                } else if (cosq < 0) {
                    cosq = 0;
                }
            }
        } else {
            return BCM_E_PORT;
        }
    } else {
        local_port = port;
        if (cosq >= _en_num_cosq[unit]) {
            return BCM_E_PARAM;
        } else if (cosq < 0) {
            cosq = 0;
        }
    }

    BCM_IF_ERROR_RETURN
        (_bcm_en_cosq_discard_get(unit, local_port, color, cosq,
                                  WREDCONFIG_PACKETr,
                                  WREDPARAM_PACKETr,
                                  WREDPARAM_YELLOW_PACKETr,
                                  WREDPARAM_RED_PACKETr,
                                  WREDPARAM_NONTCP_PACKETr,
                                  &min_thresh, &max_thresh, &drop_prob, &gain));

    *average_time = _bcm_en_exponent_to_usec(gain);

    /* Since the queue size is dynamically adjusted based on 
     * buffer availability, there is no fixed queue size. Therefore
     * the "drop_start" parameter as a percent of the queue size
     * cannot be used to determine the threshold values. Instead
     * a negative value is used for "drop_start" to indicate
     * number of packets for the min threshold.
     */
    *drop_start = 0 - min_thresh;
    
    /* Calculate the slope using the min and max threshold.
     * The angle is calculated knowing drop probability at min
     * threshold is 0% and drop probability at max threshold is 100%.
     */
    *drop_slope = _bcm_en_packets_to_angle(max_thresh - min_thresh);

    return BCM_E_NONE;
}

STATIC int     
_bcm_en_cosq_port_sched_set(int unit, soc_reg_t config_reg, 
                            soc_reg_t weight_reg, bcm_port_t port,
                            bcm_cos_queue_t start_cosq,
                            int num_weights, const int weights[], 
                            int mode)
{
    int t, i, cosq;
    uint32 wrr;
    int mbits = 0;
    int enc_weights[16];
    uint32 addr;
    
    switch (mode) {
    case BCM_COSQ_STRICT:
        mbits = 0;
        break;
    case BCM_COSQ_ROUND_ROBIN:
        mbits = 1;
        break;
    case BCM_COSQ_WEIGHTED_ROUND_ROBIN:
        mbits = 2;
        /*
         * All weight values must fit within 7 bits.
         * If weight is 0, this queue is run in strict mode,
         * others run in WRR mode.
         */
        t = 0;
        for(i = 0; i < num_weights; i++) {
            t |= weights[i];
        }
        if ((t & ~0x7f) != 0) {
            return BCM_E_PARAM;
        }
        for(i = 0; i < num_weights; i++) {
            enc_weights[i] = weights[i];
        }
        break;
    case BCM_COSQ_DEFICIT_ROUND_ROBIN:
        mbits = 3;
        for(i = 0; i < num_weights; i++) {
            enc_weights[i] = _bcm_en_cos_drr_kbytes_to_weight(weights[i]);
        }
        break;
    case BCM_COSQ_BOUNDED_DELAY:        /* not supported in xgs */
    default:
        return BCM_E_PARAM;
    }

    /* Program the scheduling mode */
    SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, config_reg, port, 
                           SCHEDULING_SELECTf, mbits));

    if ((mode == BCM_COSQ_WEIGHTED_ROUND_ROBIN) ||
        (mode == BCM_COSQ_DEFICIT_ROUND_ROBIN)) {
        /*
         * Weighted Fair Queueing scheduling among vaild COSs
         */
        for(cosq = start_cosq, i = 0; i < num_weights; cosq++, i++) {
            addr = soc_reg_addr(unit, weight_reg, port, cosq);
            SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &wrr));
            soc_reg_field_set(unit, weight_reg, &wrr,
                              COSWEIGHTSf, enc_weights[i]);
            SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, wrr));
        }
    }
    return BCM_E_NONE;
}

STATIC int
_bcm_en_cosq_port_sched_get(int unit, soc_reg_t config_reg, 
                            soc_reg_t weight_reg, bcm_port_t port,
                            bcm_cos_queue_t start_cosq,
                            int num_weights, int weights[], int *mode)
{
    uint32 addr, escfg, wrr;
    int mbits, i, cosq;

    mbits = -1;
    addr = soc_reg_addr(unit, config_reg, port, 0);
    SOC_IF_ERROR_RETURN
        (soc_reg32_read(unit, addr, &escfg));
    mbits = soc_reg_field_get(unit, config_reg, escfg, SCHEDULING_SELECTf);
    
    switch (mbits) {
    case 0:
        *mode = BCM_COSQ_STRICT;
        break;
    case 1:
        *mode = BCM_COSQ_ROUND_ROBIN;
        break;
    case 2:
        *mode = BCM_COSQ_WEIGHTED_ROUND_ROBIN;
        break;
    case 3:
        *mode = BCM_COSQ_DEFICIT_ROUND_ROBIN;
        break;
    default:
        return BCM_E_INTERNAL;
    }

    if ((mbits == 2) || (mbits == 3)) {
        wrr = 0;
        for(cosq = start_cosq, i = 0; i < num_weights; cosq++, i++) {
            addr = soc_reg_addr(unit, weight_reg, port, cosq);
            SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &wrr));
            weights[i] = soc_reg_field_get(unit, weight_reg, wrr,
                                           COSWEIGHTSf);
        }
    
        if (mbits == 3) {
            int i;
            for(i = 0; i < num_weights; i++) {
                weights[i] = _bcm_en_cos_drr_weight_to_kbytes(weights[i]);
            }
        }
    }   
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_cosq_port_sched_set
 * Purpose:
 *      Set up class-of-service policy and corresponding weights and delay
 * Parameters:
 *      unit - StrataSwitch unit number.
 *      pbm - port bitmap
 *      mode - Scheduling mode, one of BCM_COSQ_xxx
 *      weights - Weights for each COS queue
 *                Only for BCM_COSQ_WEIGHTED_FAIR_ROUND_ROBIN mode.
 *                For DRR Weight is specified in Kbytes
 *      delay - This parameter is not used 
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_en_cosq_port_sched_set(int unit, bcm_pbmp_t pbm,
                           int mode, const int weights[], int delay)
{
    bcm_port_t port;

    PBMP_ITER(pbm, port) {
        BCM_IF_ERROR_RETURN
            (_bcm_en_cosq_port_sched_set(unit, ESCONFIGr, COSWEIGHTSr,
                                         port, 0, 8, weights, mode));
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_cosq_port_sched_get
 * Purpose:
 *      Retrieve class-of-service policy and corresponding weights and delay
 * Parameters:
 *      unit     - StrataSwitch unit number.
 *      pbm      - port bitmap
 *      mode     - (output) Scheduling mode, one of BCM_COSQ_xxx
 *      weights  - (output) Weights for each COS queue
 *                          Only for BCM_COSQ_WEIGHTED_ROUND_ROBIN and
 *                          BCM_COSQ_DEFICIT_ROUND_ROBIN mode.
 *                 For DRR Weight is specified in Kbytes
 *      delay    - This parameter is not used
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      Actually just returns data for the first port in the bitmap
 */

int
bcm_en_cosq_port_sched_get(int unit, bcm_pbmp_t pbm,
                           int *mode, int weights[], int *delay)
{
    bcm_port_t port;

    PBMP_ITER(pbm, port) {
        BCM_IF_ERROR_RETURN
            (_bcm_en_cosq_port_sched_get(unit, ESCONFIGr, COSWEIGHTSr,
                                         port, 0, 8, weights, mode));
        break;
    }
    return BCM_E_NONE;
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
bcm_en_cosq_gport_add(int unit, bcm_gport_t port, int numq,
                      uint32 flags, bcm_gport_t *gport)
{
    bcm_module_t modid, mod_out;
    bcm_port_t local_port, port_out;
    bcm_trunk_t trunk_id;
    int gport_id, is_local;

    if ((numq <= 0) || (numq > 16) ||
        (!BCM_GPORT_IS_LOCAL(port) && !BCM_GPORT_IS_MODPORT(port))) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_en_cosq_resolve_mod_port(unit, port, &modid,
                                       &local_port, &trunk_id, &is_local));

    /* Verify that the specified port is for my modid and that 
     * it is capable of hierarchical queueing.
     */
    if (is_local) {
        if (!_en_num_port_cosq[unit]) {
            return BCM_E_INIT;
        } else if (!SOC_PBMP_MEMBER(_en_cosq_24q_ports[unit], local_port)) {
            return BCM_E_PORT;
        } else if (_en_num_port_cosq[unit][local_port]) {
            return BCM_E_EXISTS;
        }
        /* Call delete routine to init HW settings */
        BCM_IF_ERROR_RETURN (_bcm_en_cosq_gport_delete(unit, local_port));

        _en_num_port_cosq[unit][local_port] = numq;

        /* The GPORT ID for SCHEDULER is the modid/port from application
         * space. This allows the GPORT ID to be used on remote module
         * API calls. Convert from local space to application space.
         */
        BCM_IF_ERROR_RETURN
            (bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                                    modid, local_port, &mod_out, &port_out));
        gport_id = (mod_out << 8) | port_out;
        BCM_GPORT_SCHEDULER_SET(*gport, gport_id);
    } else {
        return BCM_E_PORT;
    }
    return BCM_E_NONE;
}
 
STATIC int
_bcm_en_cosq_gport_delete(int unit, bcm_port_t port)
{
    int cosq, weights[16];
    uint32 val;

    /* Detach from physical port */
    BCM_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, COS_MODEr, port, SELECTf, 0x0));
    BCM_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, ING_COS_MODEr, port, SELECTf, 0x0));

    /* Clear bandwidth of all Stage1 COSQs */
    for (cosq = 8; cosq < 24; cosq++) {
        BCM_IF_ERROR_RETURN
            (bcm_en_cosq_port_bandwidth_set(unit, port, cosq, 0, 0, 0, 0));
    }

    /* Clear bandwidth of S1 output queue */
    cosq = 24;
    BCM_IF_ERROR_RETURN
        (bcm_en_cosq_port_bandwidth_set(unit, port, cosq, 0, 0, 0, 0));

    /* Clear weights of all Stage1 COSQs */
    for (cosq = 0; cosq < 16; cosq++) {
        weights[cosq] = 0;
    }
    BCM_IF_ERROR_RETURN
        (_bcm_en_cosq_port_sched_set(unit, S1V_CONFIGr, S1V_COSWEIGHTSr,
                                     port, 0, 16,
                                     weights, BCM_COSQ_WEIGHTED_ROUND_ROBIN));

    /* Clear weight for S1 output queue */
    cosq = 8;
    BCM_IF_ERROR_RETURN(READ_COSWEIGHTSr(unit, port, cosq, &val));
    soc_reg_field_set(unit, COSWEIGHTSr, &val, COSWEIGHTSf, 0);
    BCM_IF_ERROR_RETURN(WRITE_COSWEIGHTSr(unit, port, cosq, val));

    /* Clear discard settings for all Stage1 COSQs */
    for (cosq = 8; cosq < 24; cosq++) {
        BCM_IF_ERROR_RETURN
            (_bcm_en_cosq_discard_set(unit, port, 
                                      BCM_COSQ_DISCARD_COLOR_ALL, cosq,
                                      WREDCONFIG_PACKETr,
                                      WREDPARAM_PACKETr,
                                      WREDPARAM_YELLOW_PACKETr,
                                      WREDPARAM_RED_PACKETr,
                                      WREDPARAM_NONTCP_PACKETr,
                                      EN_PACKET_FIELD_MAX, 
                                      EN_PACKET_FIELD_MAX, 100, 0));
    }

    _en_num_port_cosq[unit][port] = 0;

    return BCM_E_NONE;
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
bcm_en_cosq_gport_delete(int unit, bcm_gport_t gport)
{
    bcm_module_t modid;
    bcm_port_t local_port;
    bcm_trunk_t trunk_id;
    int is_local;

    if (!BCM_GPORT_IS_SCHEDULER(gport)) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_en_cosq_resolve_mod_port(unit, gport, &modid,
                                       &local_port, &trunk_id, &is_local));
    if (is_local) {
        if (!_en_num_port_cosq[unit]) {
            return BCM_E_INIT;
        } else if (_en_num_port_cosq[unit][local_port] == 0) {
            return BCM_E_NOT_FOUND;
        }
        BCM_IF_ERROR_RETURN (_bcm_en_cosq_gport_delete(unit, local_port));
    } else {
        return BCM_E_PORT;
    }
    return BCM_E_NONE;
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
bcm_en_cosq_gport_traverse(int unit, bcm_cosq_gport_traverse_cb cb,
                           void *user_data)
{
    bcm_module_t my_modid, mod_out;
    bcm_port_t port, port_out;
    bcm_gport_t gport, sched_gport;
    int gport_id;
    uint32 flags = BCM_COSQ_GPORT_SCHEDULER;

    if (_en_num_port_cosq[unit]) {
        BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &my_modid));
        BCM_PBMP_ITER(_en_cosq_24q_ports[unit], port) {
            if (_en_num_port_cosq[unit][port]) {

                /* Construct physical port GPORT ID */
                BCM_IF_ERROR_RETURN
                    (bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                                            my_modid, port, &mod_out, &port_out));
                BCM_GPORT_MODPORT_SET(gport, mod_out, port_out);

                /* Construct scheduler GPORT ID */
                gport_id = (mod_out << 8) | port_out;
                BCM_GPORT_SCHEDULER_SET(sched_gport, gport_id);

                /* Call application call-back */
                cb(unit, gport, _en_num_port_cosq[unit][port],
                   flags, sched_gport, user_data);
            }
        } 
    } else {
        return BCM_E_INIT;
    }
    return BCM_E_NONE;
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
bcm_en_cosq_gport_bandwidth_set(int unit, bcm_gport_t gport,
                                bcm_cos_queue_t cosq, uint32 kbits_sec_min,
                                uint32 kbits_sec_max, uint32 flags)
{
    bcm_module_t modid;
    bcm_port_t local_port;
    bcm_trunk_t trunk_id;
    int i, is_local;
    bcm_cos_queue_t cosq_start = 0, cosq_end = 0;

    BCM_IF_ERROR_RETURN
        (_bcm_en_cosq_resolve_mod_port(unit, gport, &modid,
                                       &local_port, &trunk_id, &is_local));

    if (is_local) {
        if (!_en_num_port_cosq[unit]) {
            return BCM_E_INIT;
        }

        if (BCM_GPORT_IS_SCHEDULER(gport)) {
            if (_en_num_port_cosq[unit][local_port] == 0) {
                return BCM_E_NOT_FOUND;
            } else if (cosq < 0) {
                cosq_start = 8;
                cosq_end = 23;
            } else if (cosq >= _en_num_port_cosq[unit][local_port]) {
                return BCM_E_PARAM;
            } else {
                cosq_start = cosq_end = (cosq + 8);
            }
        } else if (cosq == EN_SCHEDULER_COSQ) {
            if (!SOC_PBMP_MEMBER(_en_cosq_24q_ports[unit], local_port)) {
                return BCM_E_PORT;
            }
            /* Output of S1 scheduler is configured using register offset 24 */
            cosq_start = cosq_end = 24;
        } else {
            if (cosq >= _en_num_cosq[unit]) {
                return BCM_E_PARAM;
            } else if (cosq < 0) {
                cosq_start = 0;
                cosq_end = 7;
            } else {
                cosq_start = cosq_end = cosq;
            }
        }
        for (i = cosq_start; i <= cosq_end; i++) {
            BCM_IF_ERROR_RETURN
                (bcm_en_cosq_port_bandwidth_set(unit, local_port, i,
                                                kbits_sec_min,
                                                kbits_sec_max,
                                                kbits_sec_max,
                                                flags));
        }
    } else {
        return BCM_E_PORT;
    }
    return BCM_E_NONE;
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
bcm_en_cosq_gport_bandwidth_get(int unit, bcm_gport_t gport,
                                bcm_cos_queue_t cosq, uint32 *kbits_sec_min,
                                uint32 *kbits_sec_max, uint32 *flags)
{
    bcm_module_t modid;
    bcm_port_t local_port;
    bcm_trunk_t trunk_id;
    int is_local;
    uint32 kbits_max_burst; /* Placeholder */

    *kbits_sec_min = *kbits_sec_max = *flags = 0;

    BCM_IF_ERROR_RETURN
        (_bcm_en_cosq_resolve_mod_port(unit, gport, &modid,
                                       &local_port, &trunk_id, &is_local));
    
    if (is_local) {
        if (!_en_num_port_cosq[unit]) {
            return BCM_E_INIT;
        }
        if (BCM_GPORT_IS_SCHEDULER(gport)) {
            if (_en_num_port_cosq[unit][local_port] == 0) {
                return BCM_E_NOT_FOUND;
            } else if (cosq >= _en_num_port_cosq[unit][local_port]) {
                return BCM_E_PARAM;
            } else if (cosq < 0) {
                cosq = 8;
            } else {
                cosq += 8;
            }
        } else if (cosq == EN_SCHEDULER_COSQ) {
            if (!SOC_PBMP_MEMBER(_en_cosq_24q_ports[unit], local_port)) {
                return BCM_E_PORT;
            }
            /* Output of S1 scheduler is configured using register offset 24 */
            cosq = 24;
        } else {
            if (cosq >= _en_num_cosq[unit]) {
                return BCM_E_PARAM;
            } else if (cosq < 0) {
                cosq = 0;
            }
        }
        BCM_IF_ERROR_RETURN
            (bcm_en_cosq_port_bandwidth_get(unit, local_port, cosq,
                                            kbits_sec_min, kbits_sec_max,
                                            &kbits_max_burst, flags));
    } else {
        return BCM_E_PORT;
    }
    return BCM_E_NONE;
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
 *      weight - (IN) Weight for the specified COS queue(s)
 *               Unused if mode is BCM_COSQ_STRICT.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_en_cosq_gport_sched_set(int unit, bcm_gport_t gport,
                            bcm_cos_queue_t cosq, int mode, int weight)
{
    bcm_module_t modid;
    bcm_port_t local_port;
    bcm_trunk_t trunk_id;
    int i, is_local;
    bcm_cos_queue_t cosq_start = 0;
    int num_weights = 1, weights[16];
    soc_reg_t config_reg = ESCONFIGr;
    soc_reg_t weight_reg = COSWEIGHTSr;

    BCM_IF_ERROR_RETURN
        (_bcm_en_cosq_resolve_mod_port(unit, gport, &modid,
                                       &local_port, &trunk_id, &is_local));

    if (is_local) {
        if (!_en_num_port_cosq[unit]) {
            return BCM_E_INIT;
        }
        if (BCM_GPORT_IS_SCHEDULER(gport)) {
            if (_en_num_port_cosq[unit][local_port] == 0) {
                return BCM_E_NOT_FOUND;
            } else if (cosq >= _en_num_port_cosq[unit][local_port]) {
                return BCM_E_PARAM;
            } else if (cosq < 0) {
                cosq_start = 0;
                num_weights = 16;
                for (i = 0; i < num_weights; i++) {
                    if (i < _en_num_port_cosq[unit][local_port]) {
                        weights[i] = weight;
                    } else {
                        weights[i] = 0;
                    }
                }
            } else {
                cosq_start = cosq;
                num_weights = 1;
                weights[0] = weight;
            }
            config_reg = S1V_CONFIGr;
            weight_reg = S1V_COSWEIGHTSr;
        } else if (cosq == EN_SCHEDULER_COSQ) {
            if (!SOC_PBMP_MEMBER(_en_cosq_24q_ports[unit], local_port)) {
                return BCM_E_PORT;
            }
            /* Weight for output of S1 scheduler is configured using offset 8 
             * of COSWEIGHTS register.
             */
            cosq_start = 8;
            num_weights = 1;
            weights[0] = weight;
        } else {
            if (cosq >= _en_num_cosq[unit]) {
                return BCM_E_PARAM;
            } else if (cosq < 0) {
                cosq_start = 0;
                num_weights = 8;
                for (i = 0; i < num_weights; i++) {
                    if (i < _en_num_cosq[unit]) {
                        weights[i] = weight;
                    } else {
                        weights[i] = 0;
                    }
                }
            } else {
                cosq_start = cosq;
                num_weights = 1;
                weights[0] = weight;
            }
        }

        BCM_IF_ERROR_RETURN
            (_bcm_en_cosq_port_sched_set(unit, config_reg, weight_reg, 
                                         local_port, cosq_start, num_weights,
                                         weights, mode));
    } else {
        return BCM_E_PORT;
    }
    return BCM_E_NONE;
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
 *      weight - (OUT) Weight for the specified COS queue(s)
 *               Unused if mode is BCM_COSQ_STRICT.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_en_cosq_gport_sched_get(int unit, bcm_gport_t gport, 
                            bcm_cos_queue_t cosq, int *mode, int *weight)
{   
    bcm_module_t modid;
    bcm_port_t local_port;
    bcm_trunk_t trunk_id;
    int is_local;
    soc_reg_t config_reg = ESCONFIGr;
    soc_reg_t weight_reg = COSWEIGHTSr;

    *mode = *weight = 0;

    BCM_IF_ERROR_RETURN
        (_bcm_en_cosq_resolve_mod_port(unit, gport, &modid,
                                       &local_port, &trunk_id, &is_local));
    if (is_local) {
        if (!_en_num_port_cosq[unit]) {
            return BCM_E_INIT;
        }
        if (BCM_GPORT_IS_SCHEDULER(gport)) {
            if (_en_num_port_cosq[unit][local_port] == 0) {
                return BCM_E_NOT_FOUND;
            } else if (cosq >= _en_num_port_cosq[unit][local_port]) {
                return BCM_E_PARAM;
            } else if (cosq < 0) {
                cosq = 0;
            }
            config_reg = S1V_CONFIGr;
            weight_reg = S1V_COSWEIGHTSr;
        } else if (cosq == EN_SCHEDULER_COSQ) {
            if (!SOC_PBMP_MEMBER(_en_cosq_24q_ports[unit], local_port)) {
                return BCM_E_PORT;
            }
            /* Weight for output of S1 scheduler is configured using offset 8 
             * of COSWEIGHTS register.
             */
            cosq = 8;
        } else {
            if (cosq >= _en_num_cosq[unit]) {
                return BCM_E_PARAM;
            } else if (cosq < 0) {
                cosq = 0;
            }
        }
        BCM_IF_ERROR_RETURN
            (_bcm_en_cosq_port_sched_get(unit, config_reg, weight_reg, 
                                         local_port, cosq, 1,
                                         weight, mode));
    } else {
        return BCM_E_PORT;
    }
    return BCM_E_NONE;
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
bcm_en_cosq_gport_discard_set(int unit, bcm_gport_t port, bcm_cos_queue_t cosq,
                              bcm_cosq_gport_discard_t *discard)
{
    bcm_module_t modid;
    bcm_port_t local_port;
    bcm_trunk_t trunk_id;
    int is_local;
    uint32 min_thresh, max_thresh;
    int cell_size, cell_field_max;

    if ((discard == NULL) || 
        (discard->gain < 0) || (discard->gain > 15) ||
        (discard->drop_probability < 0) || (discard->drop_probability > 100)) {
        return BCM_E_PARAM;
    }

    {
        cell_size      = _BCM_EN_COSQ_CELLSIZE;
        cell_field_max = EN_CELL_FIELD_MAX;
    }

    min_thresh = discard->min_thresh;
    max_thresh = discard->max_thresh;
    if (discard->flags & BCM_COSQ_DISCARD_BYTES) {
        /* Convert bytes to cells */
        min_thresh += (cell_size - 1);
        min_thresh /= cell_size;

        max_thresh += (cell_size - 1);
        max_thresh /= cell_size;

        if ((min_thresh > cell_field_max) || 
            (max_thresh > cell_field_max)) {
            return BCM_E_PARAM;
        }
    } else { /* BCM_COSQ_DISCARD_PACKETS */
        if ((min_thresh > EN_PACKET_FIELD_MAX) || 
            (max_thresh > EN_PACKET_FIELD_MAX)) {
            return BCM_E_PARAM;
        }
    }

    if (port != BCM_GPORT_INVALID) {
        BCM_IF_ERROR_RETURN
            (_bcm_en_cosq_resolve_mod_port(unit, port, &modid,
                                           &local_port, &trunk_id, &is_local));
        if (!is_local) {
            return BCM_E_PORT;
        }

        if (cosq < 0) {
            /* per-port discard settings */
            if (discard->flags & BCM_COSQ_DISCARD_BYTES) {
                BCM_IF_ERROR_RETURN
                    (_bcm_en_cosq_discard_set(unit, local_port,
                                              discard->flags, 0,
                                              PORT_WREDCONFIG_CELLr,
                                              PORT_WREDPARAM_CELLr,
                                              PORT_WREDPARAM_YELLOW_CELLr,
                                              PORT_WREDPARAM_RED_CELLr,
                                              PORT_WREDPARAM_NONTCP_CELLr,
                                              min_thresh, max_thresh, 
                                              discard->drop_probability,
                                              discard->gain));
                BCM_IF_ERROR_RETURN
                    (_bcm_en_cosq_discard_cap_enable_set(unit, local_port, 0,
                                                         PORT_WREDCONFIG_CELLr, 
                                                         discard->flags));

            } else { /* BCM_COSQ_DISCARD_PACKETS */
                BCM_IF_ERROR_RETURN
                    (_bcm_en_cosq_discard_set(unit, local_port,
                                              discard->flags, 0,
                                              PORT_WREDCONFIG_PACKETr,
                                              PORT_WREDPARAM_PACKETr,
                                              PORT_WREDPARAM_YELLOW_PACKETr,
                                              PORT_WREDPARAM_RED_PACKETr,
                                              PORT_WREDPARAM_NONTCP_PACKETr,
                                              min_thresh, max_thresh, 
                                              discard->drop_probability,
                                              discard->gain));
                BCM_IF_ERROR_RETURN
                    (_bcm_en_cosq_discard_cap_enable_set(unit, local_port, 0,
                                                         PORT_WREDCONFIG_PACKETr, 
                                                         discard->flags));
            }
        } else {
            /* per-port/per-cos discard settings */
            if (BCM_GPORT_IS_SCHEDULER(port)) {
                if (_en_num_port_cosq[unit][local_port] == 0) {
                    return BCM_E_NOT_FOUND;
                } else if (cosq >= _en_num_port_cosq[unit][local_port]) {
                    return BCM_E_PARAM;
                } else {
                    cosq = cosq + 8;
                }
            } else if (cosq >= _en_num_cosq[unit]) {
                return BCM_E_PARAM;
            }

            if (discard->flags & BCM_COSQ_DISCARD_BYTES) {
                BCM_IF_ERROR_RETURN
                    (_bcm_en_cosq_discard_set(unit, local_port,
                                              discard->flags, cosq,
                                              WREDCONFIG_CELLr,
                                              WREDPARAM_CELLr,
                                              WREDPARAM_YELLOW_CELLr,
                                              WREDPARAM_RED_CELLr,
                                              WREDPARAM_NONTCP_CELLr,
                                              min_thresh, max_thresh, 
                                              discard->drop_probability,
                                              discard->gain));
                BCM_IF_ERROR_RETURN
                    (_bcm_en_cosq_discard_cap_enable_set(unit, local_port, cosq,
                                                         WREDCONFIG_CELLr, 
                                                         discard->flags));
            } else { /* BCM_COSQ_DISCARD_PACKETS */
                BCM_IF_ERROR_RETURN
                    (_bcm_en_cosq_discard_set(unit, local_port,
                                              discard->flags, cosq,
                                              WREDCONFIG_PACKETr,
                                              WREDPARAM_PACKETr,
                                              WREDPARAM_YELLOW_PACKETr,
                                              WREDPARAM_RED_PACKETr,
                                              WREDPARAM_NONTCP_PACKETr,
                                              min_thresh, max_thresh, 
                                              discard->drop_probability,
                                              discard->gain));
                BCM_IF_ERROR_RETURN
                    (_bcm_en_cosq_discard_cap_enable_set(unit, local_port, cosq,
                                                         WREDCONFIG_PACKETr, 
                                                         discard->flags));
            }
        }
    } else {
        /* chip-wide discard settings */
        if (discard->flags & BCM_COSQ_DISCARD_BYTES) {
            BCM_IF_ERROR_RETURN
                (_bcm_en_cosq_discard_set(unit, REG_PORT_ANY, discard->flags,
                                          0, GLOBAL_WREDCONFIG_CELLr,
                                          GLOBAL_WREDPARAM_CELLr,
                                          GLOBAL_WREDPARAM_YELLOW_CELLr,
                                          GLOBAL_WREDPARAM_RED_CELLr,
                                          GLOBAL_WREDPARAM_NONTCP_CELLr,
                                          min_thresh, max_thresh, 
                                          discard->drop_probability,
                                          discard->gain));
            BCM_IF_ERROR_RETURN
                (_bcm_en_cosq_discard_cap_enable_set(unit, REG_PORT_ANY, 0,
                                                     GLOBAL_WREDCONFIG_CELLr, 
                                                     discard->flags));
        } else { /* BCM_COSQ_DISCARD_PACKETS */
            BCM_IF_ERROR_RETURN
                (_bcm_en_cosq_discard_set(unit, REG_PORT_ANY, discard->flags,
                                          0, GLOBAL_WREDCONFIG_PACKETr,
                                          GLOBAL_WREDPARAM_PACKETr,
                                          GLOBAL_WREDPARAM_YELLOW_PACKETr,
                                          GLOBAL_WREDPARAM_RED_PACKETr,
                                          GLOBAL_WREDPARAM_NONTCP_PACKETr,
                                          min_thresh, max_thresh, 
                                          discard->drop_probability,
                                          discard->gain));
            BCM_IF_ERROR_RETURN
                (_bcm_en_cosq_discard_cap_enable_set(unit, REG_PORT_ANY, 0,
                                                     GLOBAL_WREDCONFIG_PACKETr, 
                                                     discard->flags));
        }
    }
    return BCM_E_NONE;
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
bcm_en_cosq_gport_discard_get(int unit, bcm_gport_t port, bcm_cos_queue_t cosq,
                              bcm_cosq_gport_discard_t *discard)
{
    bcm_module_t modid;
    bcm_port_t local_port;
    bcm_trunk_t trunk_id;
    int is_local;
    uint32 min_thresh, max_thresh;

    if (discard == NULL) {
        return BCM_E_PARAM;
    }

    if (port != BCM_GPORT_INVALID) {
        BCM_IF_ERROR_RETURN
            (_bcm_en_cosq_resolve_mod_port(unit, port, &modid,
                                           &local_port, &trunk_id, &is_local));
        if (!is_local) {
            return BCM_E_PORT;
        }

        if (cosq < 0) {
            /* per-port discard settings */
            if (discard->flags & BCM_COSQ_DISCARD_BYTES) {
                BCM_IF_ERROR_RETURN
                    (_bcm_en_cosq_discard_get(unit, local_port,
                                              discard->flags, 0,
                                              PORT_WREDCONFIG_CELLr,
                                              PORT_WREDPARAM_CELLr,
                                              PORT_WREDPARAM_YELLOW_CELLr,
                                              PORT_WREDPARAM_RED_CELLr,
                                              PORT_WREDPARAM_NONTCP_CELLr,
                                              &min_thresh, &max_thresh, 
                                              &discard->drop_probability,
                                              &discard->gain));
                BCM_IF_ERROR_RETURN
                    (_bcm_en_cosq_discard_cap_enable_get(unit, local_port, 0,
                                                         PORT_WREDCONFIG_CELLr, 
                                                         &discard->flags));
            } else { /* BCM_COSQ_DISCARD_PACKETS */
                BCM_IF_ERROR_RETURN
                    (_bcm_en_cosq_discard_get(unit, local_port,
                                              discard->flags, 0,
                                              PORT_WREDCONFIG_PACKETr,
                                              PORT_WREDPARAM_PACKETr,
                                              PORT_WREDPARAM_YELLOW_PACKETr,
                                              PORT_WREDPARAM_RED_PACKETr,
                                              PORT_WREDPARAM_NONTCP_PACKETr,
                                              &min_thresh, &max_thresh, 
                                              &discard->drop_probability,
                                              &discard->gain));
                BCM_IF_ERROR_RETURN
                    (_bcm_en_cosq_discard_cap_enable_get(unit, local_port, 0,
                                                         PORT_WREDCONFIG_PACKETr, 
                                                         &discard->flags));
            }
        } else {
            /* per-port/per-cos discard settings */
            if (SOC_IS_TR_VL(unit)) {
                if (BCM_GPORT_IS_SCHEDULER(port)) {
                    if (_en_num_port_cosq[unit][local_port] == 0) {
                        return BCM_E_NOT_FOUND;
                    } else if (cosq >= _en_num_port_cosq[unit][local_port]) {
                        return BCM_E_PARAM;
                    } else {
                        cosq = cosq + 8;
                    }
                } else if (cosq >= _en_num_cosq[unit]) {
                    return BCM_E_PARAM;
                }
            }

            if (discard->flags & BCM_COSQ_DISCARD_BYTES) {
                BCM_IF_ERROR_RETURN
                    (_bcm_en_cosq_discard_get(unit, local_port,
                                              discard->flags, cosq,
                                              WREDCONFIG_CELLr,
                                              WREDPARAM_CELLr,
                                              WREDPARAM_YELLOW_CELLr,
                                              WREDPARAM_RED_CELLr,
                                              WREDPARAM_NONTCP_CELLr,
                                              &min_thresh, &max_thresh, 
                                              &discard->drop_probability,
                                              &discard->gain));
                BCM_IF_ERROR_RETURN
                    (_bcm_en_cosq_discard_cap_enable_get(unit, local_port, cosq,
                                                         WREDCONFIG_CELLr, 
                                                         &discard->flags));
            } else { /* BCM_COSQ_DISCARD_PACKETS */
                BCM_IF_ERROR_RETURN
                    (_bcm_en_cosq_discard_get(unit, local_port,
                                              discard->flags, cosq,
                                              WREDCONFIG_PACKETr,
                                              WREDPARAM_PACKETr,
                                              WREDPARAM_YELLOW_PACKETr,
                                              WREDPARAM_RED_PACKETr,
                                              WREDPARAM_NONTCP_PACKETr,
                                              &min_thresh, &max_thresh, 
                                              &discard->drop_probability,
                                              &discard->gain));
                BCM_IF_ERROR_RETURN
                    (_bcm_en_cosq_discard_cap_enable_get(unit, local_port, cosq,
                                                         WREDCONFIG_PACKETr, 
                                                         &discard->flags));
            }
        }
    } else {
        /* chip-wide discard settings */
        if (discard->flags & BCM_COSQ_DISCARD_BYTES) {
            BCM_IF_ERROR_RETURN
                (_bcm_en_cosq_discard_get(unit, REG_PORT_ANY, discard->flags,
                                          0, GLOBAL_WREDCONFIG_CELLr,
                                          GLOBAL_WREDPARAM_CELLr,
                                          GLOBAL_WREDPARAM_YELLOW_CELLr,
                                          GLOBAL_WREDPARAM_RED_CELLr,
                                          GLOBAL_WREDPARAM_NONTCP_CELLr,
                                          &min_thresh, &max_thresh, 
                                          &discard->drop_probability,
                                          &discard->gain));
            BCM_IF_ERROR_RETURN
                (_bcm_en_cosq_discard_cap_enable_get(unit, REG_PORT_ANY, 0,
                                                     GLOBAL_WREDCONFIG_CELLr, 
                                                     &discard->flags));
        } else { /* BCM_COSQ_DISCARD_PACKETS */
            BCM_IF_ERROR_RETURN
                (_bcm_en_cosq_discard_get(unit, REG_PORT_ANY, discard->flags,
                                          0, GLOBAL_WREDCONFIG_PACKETr,
                                          GLOBAL_WREDPARAM_PACKETr,
                                          GLOBAL_WREDPARAM_YELLOW_PACKETr,
                                          GLOBAL_WREDPARAM_RED_PACKETr,
                                          GLOBAL_WREDPARAM_NONTCP_PACKETr,
                                          &min_thresh, &max_thresh, 
                                          &discard->drop_probability,
                                          &discard->gain));
            BCM_IF_ERROR_RETURN
                (_bcm_en_cosq_discard_cap_enable_get(unit, REG_PORT_ANY, 0,
                                                     GLOBAL_WREDCONFIG_PACKETr, 
                                                     &discard->flags));
        }
    }

    if (discard->flags & BCM_COSQ_DISCARD_BYTES) {
        /* Convert cells to bytes */
        min_thresh *= _BCM_EN_COSQ_CELLSIZE;
        max_thresh *= _BCM_EN_COSQ_CELLSIZE;
    }
    discard->min_thresh = min_thresh;
    discard->max_thresh = max_thresh;

    return BCM_E_NONE;
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
 */
int
bcm_en_cosq_gport_attach(int unit, bcm_gport_t sched_gport, 
                         bcm_gport_t input_gport, bcm_cos_queue_t cosq)
{
    bcm_module_t sched_modid, input_modid;
    bcm_port_t sched_port, input_port;
    bcm_trunk_t trunk_id;
    int is_local;

    if (!BCM_GPORT_IS_SCHEDULER(sched_gport)) {
        return BCM_E_PARAM;
    } else if (!_en_num_port_cosq[unit]) {
        return BCM_E_INIT;
    } else if (cosq < 0) {
        cosq = EN_SCHEDULER_COSQ;
    } else if (cosq != EN_SCHEDULER_COSQ) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_en_cosq_resolve_mod_port(unit, sched_gport, &sched_modid,
                                       &sched_port, &trunk_id, &is_local));

    if (is_local) {
        BCM_IF_ERROR_RETURN
            (_bcm_en_cosq_resolve_mod_port(unit, input_gport, &input_modid,
                                           &input_port, &trunk_id, &is_local));

        if (_en_num_port_cosq[unit][sched_port] == 0) {
            /* GPORT has not been added. */
            return BCM_E_NOT_FOUND;
        } else if ((sched_modid != input_modid) || (sched_port != input_port)) {
            /* Be sure the GPORT matches to the physical port */
            return BCM_E_PARAM;
        }

        BCM_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, ING_COS_MODEr, input_port, 
                                    SELECTf, 0x3)); /* VLAN_COS */
        BCM_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, COS_MODEr, input_port, 
                                    SELECTf, 0x3)); /* VLAN_COS */
    } else {
        return BCM_E_PORT;
    }
    return BCM_E_NONE;
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
bcm_en_cosq_gport_detach(int unit, bcm_gport_t sched_gport,
                         bcm_gport_t input_gport, bcm_cos_queue_t cosq)
{
    bcm_module_t sched_modid, input_modid;
    bcm_port_t sched_port, input_port;
    bcm_trunk_t trunk_id;
    int is_local;

    if (!BCM_GPORT_IS_SCHEDULER(sched_gport)) {
        return BCM_E_PARAM;
    } else if (!_en_num_port_cosq[unit]) {
        return BCM_E_INIT;
    } else if ((cosq > 0) && (cosq != EN_SCHEDULER_COSQ)) {
        return BCM_E_NOT_FOUND;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_en_cosq_resolve_mod_port(unit, sched_gport, &sched_modid,
                                       &sched_port, &trunk_id, &is_local));
    if (is_local) {
       BCM_IF_ERROR_RETURN
           (_bcm_en_cosq_resolve_mod_port(unit, input_gport, &input_modid,
                                          &input_port, &trunk_id, &is_local));
        if (_en_num_port_cosq[unit][sched_port] == 0) {
            /* GPORT has not been added. */
            return BCM_E_NOT_FOUND;
        } else if ((sched_modid != input_modid) || (sched_port != input_port)) {
            /* Be sure the GPORT matches to the physical port */
            return BCM_E_PARAM;
        }
    
        BCM_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, COS_MODEr, input_port, SELECTf, 0x0));
        BCM_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, ING_COS_MODEr, input_port, SELECTf, 0x0));
    } else {
        return BCM_E_PORT;
    }
    return BCM_E_NONE;
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
bcm_en_cosq_gport_attach_get(int unit, bcm_gport_t sched_gport,
                             bcm_gport_t *input_gport, bcm_cos_queue_t *cosq)
{
    bcm_module_t sched_modid, input_modid;
    bcm_port_t sched_port, input_port;
    bcm_trunk_t trunk_id;
    int is_local;
    uint32 val;

    if (!BCM_GPORT_IS_SCHEDULER(sched_gport) ||
        !cosq || !input_gport) {
        return BCM_E_PARAM;
    } else if (!_en_num_port_cosq[unit]) {
        return BCM_E_INIT;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_en_cosq_resolve_mod_port(unit, sched_gport, &sched_modid,
                                       &sched_port, &trunk_id, &is_local));
    if (is_local) {
        if (_en_num_port_cosq[unit][sched_port] == 0) {
            /* GPORT has not been added. */
            return BCM_E_NOT_FOUND;
        }

        BCM_IF_ERROR_RETURN(READ_COS_MODEr(unit, sched_port, &val));
        if (soc_reg_field_get(unit, COS_MODEr, val, SELECTf) == 3) {
            /* GPORT has been attached */
            *cosq = EN_SCHEDULER_COSQ;

            BCM_IF_ERROR_RETURN
                (bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                                        sched_modid, sched_port,
                                        &input_modid, &input_port));
            BCM_GPORT_MODPORT_SET(*input_gport, input_modid, input_port);
        } else {
            /* GPORT has not been attached */
            return BCM_E_NOT_FOUND;
        }
    } else {
        return BCM_E_PORT;
    }
    return BCM_E_NONE;
}

#else /* BCM_ENDURO_SUPPORT */
int _en_cosq_not_empty;
#endif  /* BCM_ENDURO_SUPPORT */

