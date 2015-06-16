/*
 * $Id$
 * $Copyright: Copyright 2012 Broadcom Corporation.
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
 * File:        field.c
 * Purpose:     Field Processor module routines specific to BCM56960
 *
 */

#include <shared/bsl.h>

#include <soc/defs.h>
#if defined(BCM_TOMAHAWK_SUPPORT) && defined(BCM_FIELD_SUPPORT)
#include <soc/drv.h>
#include <soc/scache.h>
#include <soc/trident2.h>
#include <soc/triumph3.h>

#include <bcm/error.h>
#include <bcm/l3.h>
#include <bcm/field.h>
#include <bcm/tunnel.h>
#include <bcm/stat.h>
#include <shared/bsl.h>
#include <shared/et/bcmendian.h>

#include <bcm_int/common/multicast.h>
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/field.h>
#include <bcm_int/esw/l3.h>
#include <bcm_int/esw/nat.h>
#include <bcm_int/esw/policer.h>
#include <bcm_int/esw/trx.h>
#include <bcm_int/esw/triumph.h>
#include <bcm_int/esw/trident.h>
#include <bcm_int/esw/trident2.h>
#include <bcm_int/esw/tomahawk.h>
#include <bcm_int/esw/mirror.h>

#ifndef ALIGN32
#define ALIGN32(x)      (((x) + 31) & ~31)
#endif

#ifndef BIT
#define BIT(n)          (1U << (n))
#endif


#ifdef BCM_WARM_BOOT_SUPPORT
/* Version change .. Tomahawk IFP WB code */
#define BCM_WB_VERSION_1_9                SOC_SCACHE_VERSION(1,9)
#define BCM_WB_VERSION_1_8                SOC_SCACHE_VERSION(1,8)
#define BCM_WB_VERSION_1_3                SOC_SCACHE_VERSION(1,3)
#define BCM_WB_DEFAULT_VERSION            BCM_WB_VERSION_1_9
#endif

static char *action_name[bcmFieldActionCount] = BCM_FIELD_ACTION_STRINGS;

/* IFP meter pools information. */
#define _IFP_TH_NUM_METER_POOLS 8
#define _IFP_TH_NUM_METER_PAIRS_PER_POOL 128

/* IFP_TCAM modes */
#define _IFP_SLICE_MODE_NARROW  0    /* 80-bit Mode  */
#define _IFP_SLICE_MODE_WIDE    1    /* 160-bit Mode */
#define _FP_LT_ID_BASE 0
#define _FP_LT_ID_MAX (0x1F)

static soc_field_t _th_efp_slice_mode[4][2] =  {
          {SLICE_0_MODEf, SLICE_0_IPV6_KEY_MODEf},
          {SLICE_1_MODEf, SLICE_1_IPV6_KEY_MODEf},
          {SLICE_2_MODEf, SLICE_2_IPV6_KEY_MODEf},
          {SLICE_3_MODEf, SLICE_3_IPV6_KEY_MODEf}
       };
static soc_field_t classfldtbl[][7] = {
                       {SLICE_0_F1f, SLICE_0_F2f, SLICE_0_F3f, SLICE_0_F4f, 
                                     SLICE_0_F6f, SLICE_0_F7f, SLICE_0_F8f},
                       {SLICE_1_F1f, SLICE_1_F2f, SLICE_1_F3f, SLICE_1_F4f, 
                                     SLICE_1_F6f, SLICE_1_F7f, SLICE_1_F8f},
                       {SLICE_2_F1f, SLICE_2_F2f, SLICE_2_F3f, SLICE_2_F4f, 
                                     SLICE_2_F6f, SLICE_2_F7f, SLICE_2_F8f},
                       {SLICE_3_F1f, SLICE_3_F2f, SLICE_3_F3f, SLICE_3_F4f, 
                                     SLICE_3_F6f, SLICE_3_F7f, SLICE_3_F8f}
};

static soc_field_t dvpfldtbl[4] = 
                       { SLICE_0f, SLICE_1f, SLICE_2f, SLICE_3f };

static soc_field_t mdlfldtbl[4] = 
                       { SLICE_0f, SLICE_1f, SLICE_2f, SLICE_3f };

/* Function declarations */
STATIC int _field_th_stage_quals_ibus_map_init(int unit,
                                             _field_stage_t *stage_fc);

STATIC int _field_th_stage_extractors_init(int unit, _field_stage_t *stage_fc);

STATIC int _field_ser_oper_mode_init(int unit,
                                     int stage_id,
                                     bcm_field_group_oper_mode_t mode);


STATIC int
_field_th_tcam_policy_clear(int unit, _field_entry_t *f_ent, int tcam_idx);

/*
 * Function: _bcm_field_th_control_set
 *
 * Purpose:
 *     Set control status.
 *
 * Parameters:
 *     unit     - (IN) BCM device number.
 *     control  - (IN) Control element to set.
 *     state    - (IN) State (0/1).
 *
 * Returns:
 *     BCM_E_NONE    - Success
 *     BCM_E_INIT    - BCM unit not initialized
 *     BCM_E_PARAM   - Flag state not valid on device
 */
int
_bcm_field_th_control_set(int unit, _field_control_t *fc,
                          bcm_field_control_t control, uint32 state)
{
    int rv = BCM_E_NONE;  /* Operation return status.   */
    uint32              reg_values[2];    /* Register value buffer.     */
    soc_field_t         reg_fields[2];    /* Register fields to modify. */
    int                 field_count = 2;  /* Number of updated fields.  */

    reg_values[0] = reg_values[1] = (state == TRUE) ? 1: 0;

    if (NULL == fc) {
        return BCM_E_PARAM;
    }

    switch (control) {
        case bcmFieldControlRedirectIngressVlanCheck:
            reg_fields[0] = VLAN_CHECK_ENf;
            field_count = 1;
            rv = soc_reg_fields32_modify(unit, SW2_IFP_DST_ACTION_CONTROLr,
                                         REG_PORT_ANY, field_count,
                                         reg_fields, reg_values);
            break;
        case bcmFieldControlRedirectNonUcastTrunkResolve:
            reg_fields[0] = LAG_RES_ENf;
            reg_fields[1] = HGTRUNK_RES_ENf;
            field_count = 2;
            rv = soc_reg_fields32_modify(unit, SW2_IFP_DST_ACTION_CONTROLr,
                                         REG_PORT_ANY, field_count,
                                         reg_fields, reg_values);
            break;
        case bcmFieldControlRedirectNonUcastEtherTrunkResolve:
            reg_fields[0] = LAG_RES_ENf;
            field_count = 1;
            rv = soc_reg_fields32_modify(unit, SW2_IFP_DST_ACTION_CONTROLr,
                                         REG_PORT_ANY, field_count,
                                         reg_fields, reg_values);
            break;
        case bcmFieldControlRedirectNonUcastFabricTrunkResolve:
            reg_fields[0] = HGTRUNK_RES_ENf;
            field_count = 1;
            rv = soc_reg_fields32_modify(unit, SW2_IFP_DST_ACTION_CONTROLr,
                                         REG_PORT_ANY, field_count,
                                         reg_fields, reg_values);
            break;
        case bcmFieldControlRedirectExcludeSrcPort:
            reg_fields[0] = HG_SRC_REMOVAL_ENf;
            reg_fields[1] = SRC_REMOVAL_ENf;
            field_count = 2;
            rv = soc_reg_fields32_modify(unit, SW2_IFP_DST_ACTION_CONTROLr,
                                         REG_PORT_ANY, field_count,
                                         reg_fields, reg_values);
            break;
        case bcmFieldControlRedirectPortFloodBlock:
            reg_fields[0] = PORT_BLOCK_ENf;
            field_count = 1;
            rv = soc_reg_fields32_modify(unit, SW2_IFP_DST_ACTION_CONTROLr,
                                         REG_PORT_ANY, field_count,
                                         reg_fields, reg_values);
            break;
        case bcmFieldControlRedirectVlanFloodBlock:
            reg_fields[0] = VLAN_BLOCK_ENf;
            field_count = 1;
            rv = soc_reg_fields32_modify(unit, SW2_IFP_DST_ACTION_CONTROLr,
                                         REG_PORT_ANY, field_count,
                                         reg_fields, reg_values);
            break;
        case bcmFieldControlRedirectNextHopExcludeSrcPort:
            reg_fields[0] = SRC_REMOVAL_EN_FOR_REDIRECT_TO_NHIf;
            field_count = 1;
            rv = soc_reg_fields32_modify(unit, SW2_IFP_DST_ACTION_CONTROLr,
                                         REG_PORT_ANY, field_count,
                                         reg_fields, reg_values);
            break;
        default:
            rv = _bcm_field_control_set(unit, fc, control, state);
    }

    return rv;
}
/*
 * Function: _bcm_field_th_control_get
 *
 * Purpose:
 *     Get control status info.
 *
 * Parameters:
 *     unit     - (IN)  BCM device number.
 *     control  - (IN)  Control element to get.
 *     status   - (OUT) Status of field element.
 *
 * Returns:
 *     BCM_E_UNAVAIL - field control is not available
 *     BCM_E_PARAM   - *state pointing to NULL
 *     BCM_E_NONE    - Success
 */
int
_bcm_field_th_control_get(int unit, _field_control_t *fc,
                          bcm_field_control_t control, uint32 *state)
{

    uint32 reg_val;          /* Register value buffer. */
    int    rv = BCM_E_NONE;  /* Operation return status.   */

    if (NULL == state || NULL == fc) {
        return BCM_E_PARAM;
    }

    switch (control) {
        case bcmFieldControlRedirectIngressVlanCheck:
            rv = READ_SW2_IFP_DST_ACTION_CONTROLr(unit, &reg_val);
            if (BCM_SUCCESS(rv)) {
                *state = soc_reg_field_get(unit,
                                       SW2_IFP_DST_ACTION_CONTROLr,
                                       reg_val,
                                       VLAN_CHECK_ENf);
            }
            break;
        case bcmFieldControlRedirectNonUcastTrunkResolve:
            rv = READ_SW2_IFP_DST_ACTION_CONTROLr(unit, &reg_val);
            if (BCM_SUCCESS(rv)) {
                *state = soc_reg_field_get(unit,
                                       SW2_IFP_DST_ACTION_CONTROLr,
                                       reg_val,
                                       LAG_RES_ENf);
            }
            break;
        case bcmFieldControlRedirectNonUcastEtherTrunkResolve:
            rv = READ_SW2_IFP_DST_ACTION_CONTROLr(unit, &reg_val);
            if (BCM_SUCCESS(rv)) {
                *state = soc_reg_field_get(unit,
                                       SW2_IFP_DST_ACTION_CONTROLr,
                                       reg_val,
                                       LAG_RES_ENf);
            }
            break;
        case bcmFieldControlRedirectNonUcastFabricTrunkResolve:
            rv = READ_SW2_IFP_DST_ACTION_CONTROLr(unit, &reg_val);
            if (BCM_SUCCESS(rv)) {
                *state = soc_reg_field_get(unit,
                                       SW2_IFP_DST_ACTION_CONTROLr,
                                       reg_val,
                                       HGTRUNK_RES_ENf);
            }
            break;
        case bcmFieldControlRedirectExcludeSrcPort:
            rv = READ_SW2_IFP_DST_ACTION_CONTROLr(unit, &reg_val);
            if (BCM_SUCCESS(rv)) {
                *state = soc_reg_field_get(unit,
                                       SW2_IFP_DST_ACTION_CONTROLr,
                                       reg_val,
                                       SRC_REMOVAL_ENf);
            }
            break;
        case bcmFieldControlRedirectPortFloodBlock:
            rv = READ_SW2_IFP_DST_ACTION_CONTROLr(unit, &reg_val);
            if (BCM_SUCCESS(rv)) {
                *state = soc_reg_field_get(unit,
                                       SW2_IFP_DST_ACTION_CONTROLr,
                                       reg_val,
                                       PORT_BLOCK_ENf);
            }
            break;
        case bcmFieldControlRedirectVlanFloodBlock:
            rv = READ_SW2_IFP_DST_ACTION_CONTROLr(unit, &reg_val);
            if (BCM_SUCCESS(rv)) {
                *state = soc_reg_field_get(unit,
                                       SW2_IFP_DST_ACTION_CONTROLr,
                                       reg_val,
                                       VLAN_BLOCK_ENf);
            }
            break;
        case bcmFieldControlRedirectNextHopExcludeSrcPort:
            rv = READ_SW2_IFP_DST_ACTION_CONTROLr(unit, &reg_val);
            if (BCM_SUCCESS(rv)) {
                *state = soc_reg_field_get(unit,
                                       SW2_IFP_DST_ACTION_CONTROLr,
                                       reg_val,
                                       SRC_REMOVAL_EN_FOR_REDIRECT_TO_NHIf);
            }
            break;
        default:
            rv = _bcm_field_control_get(unit, fc, control, state);
    }

    return rv;

}
/*
 * Function:
 *    _field_th_instance_counters_deinit
 *
 * Purpose:
 *    Free instance counter usage bitmap resource.
 *
 * Parameters:
 *    unit     - (IN) BCM device number.
 *    stage_fc - (IN/OUT) Stage field control structure.
 *    inst     - (IN) Device stage instance information.
 *
 * Returns:
 *    BCM_E_PARAM  - Invalid instance number.
 *    BCM_E_NONE   - Success.
 */
STATIC int
_field_th_instance_counters_deinit(int unit, _field_stage_t *stage_fc, int inst)
{
    int slice; /* Slice iterator. */

    /* Input parameter check. */
    if (NULL == stage_fc) {
        return (BCM_E_NONE);
    }

    /* Validate instance value. */
    if (inst < 0 || inst >= stage_fc->num_instances) {
        return (BCM_E_PARAM);
    }

    /* Make sure slices array is initialized.*/
    if (NULL == stage_fc->slices[inst]) {
        return (BCM_E_NONE);
    }

    /* Destroy 64 bit counters. */
    _bcm_field_counter_collect_deinit(unit, stage_fc);

    if (_BCM_FIELD_STAGE_EGRESS == stage_fc->stage_id) {
        if (NULL != stage_fc->slices[inst][0].counter_bmp.w) {
            sal_free(stage_fc->slices[inst][0].counter_bmp.w);
        }

        for (slice = 0; slice < stage_fc->tcam_slices; slice++) {
            stage_fc->slices[inst][slice].counter_bmp.w = NULL;
        }
    }

    return (BCM_E_NONE);
}


struct cntr_hw_mode {
    uint32 hw_bmap;
    uint8  hw_entry_count;
    _field_stat_color_t color;
};

static const struct cntr_hw_mode th_ingress_cntr_hw_mode_tbl[] = {
    /* (R_COUNTER_OFS = 0, Y_COUNTER_OFS = 0, G_COUNTER_OFS = 1) */
    { BIT(bcmFieldStatGreenPackets) | BIT(bcmFieldStatGreenBytes),
      1, _bcmFieldStatColorGreen
    },
    /* (R_COUNTER_OFS = 0, Y_COUNTER_OFS = 1, G_COUNTER_OFS = 0) */
    { BIT(bcmFieldStatYellowPackets) | BIT(bcmFieldStatYellowBytes),
      1, _bcmFieldStatColorYellow
    },
    /* (R_COUNTER_OFS = 0, Y_COUNTER_OFS = 1, G_COUNTER_OFS = 1) */
    { BIT(bcmFieldStatNotRedPackets) | BIT(bcmFieldStatNotRedBytes),
      1, _bcmFieldStatColorNotRed
    },
    /* (R_COUNTER_OFS = 0, Y_COUNTER_OFS = 2, G_COUNTER_OFS = 1) */
    { BIT(bcmFieldStatYellowPackets) | BIT(bcmFieldStatYellowBytes)
      | BIT(bcmFieldStatGreenPackets) | BIT(bcmFieldStatGreenBytes)
      | BIT(bcmFieldStatNotRedPackets) | BIT(bcmFieldStatNotRedBytes),
      2, _bcmFieldStatColorGreenYellow
    },
    /* (R_COUNTER_OFS = 1, Y_COUNTER_OFS = 0, G_COUNTER_OFS = 0) */
    { BIT(bcmFieldStatRedPackets) | BIT(bcmFieldStatRedBytes),
      1, _bcmFieldStatColorRed
    },
    /* (R_COUNTER_OFS = 1, Y_COUNTER_OFS = 0, G_COUNTER_OFS = 1) */
    { BIT(bcmFieldStatNotYellowPackets) | BIT(bcmFieldStatNotYellowBytes),
      1, _bcmFieldStatColorNotYellow
    },
    /* (R_COUNTER_OFS = 1, Y_COUNTER_OFS = 0, G_COUNTER_OFS = 2) */
    { BIT(bcmFieldStatRedPackets) | BIT(bcmFieldStatRedBytes)
      | BIT(bcmFieldStatGreenPackets) | BIT(bcmFieldStatGreenBytes)
      | BIT(bcmFieldStatNotYellowPackets) | BIT(bcmFieldStatNotYellowBytes),
      2, _bcmFieldStatColorGreenRed
    },
    /* (R_COUNTER_OFS = 1, Y_COUNTER_OFS = 1, G_COUNTER_OFS = 0) */
    { BIT(bcmFieldStatNotGreenPackets) | BIT(bcmFieldStatNotGreenBytes),
      1, _bcmFieldStatColorNotGreen
    },
    /* (R_COUNTER_OFS = 1, Y_COUNTER_OFS = 1, G_COUNTER_OFS = 1) */
    { BIT(bcmFieldStatPackets) | BIT(bcmFieldStatBytes),
      1, _bcmFieldStatColorNoColor
    },
    /* (R_COUNTER_OFS = 1, Y_COUNTER_OFS = 1, G_COUNTER_OFS = 2) */
    { BIT(bcmFieldStatNotGreenPackets) | BIT(bcmFieldStatNotGreenBytes)
      | BIT(bcmFieldStatGreenPackets) | BIT(bcmFieldStatGreenBytes)
      | BIT(bcmFieldStatPackets) | BIT(bcmFieldStatBytes),
      2, _bcmFieldStatColorGreenNotGreen
    },
    /* (R_COUNTER_OFS = 1, Y_COUNTER_OFS = 2, G_COUNTER_OFS = 0) */
    { BIT(bcmFieldStatRedPackets) | BIT(bcmFieldStatRedBytes)
      | BIT(bcmFieldStatYellowPackets) | BIT(bcmFieldStatYellowBytes)
      | BIT(bcmFieldStatNotGreenPackets) | BIT(bcmFieldStatNotGreenBytes),
      2, _bcmFieldStatColorYellowRed
    },
    /* (R_COUNTER_OFS = 1, Y_COUNTER_OFS = 2, G_COUNTER_OFS = 1) */
    { BIT(bcmFieldStatNotYellowPackets) | BIT(bcmFieldStatNotYellowBytes)
      | BIT(bcmFieldStatYellowPackets) | BIT(bcmFieldStatYellowBytes)
      | BIT(bcmFieldStatPackets) | BIT(bcmFieldStatBytes),
      2, _bcmFieldStatColorYellowNotYellow
    },
    /* (R_COUNTER_OFS = 1, Y_COUNTER_OFS = 2, G_COUNTER_OFS = 2) */
    { BIT(bcmFieldStatRedPackets) | BIT(bcmFieldStatRedBytes)
      | BIT(bcmFieldStatNotRedPackets) | BIT(bcmFieldStatNotRedBytes)
      | BIT(bcmFieldStatPackets) | BIT(bcmFieldStatBytes),
      2, _bcmFieldStatColorRedNotRed
    },
    /* (R_COUNTER_OFS = 1, Y_COUNTER_OFS = 2, G_COUNTER_OFS = 3) */
    { BIT(bcmFieldStatRedPackets) | BIT(bcmFieldStatRedBytes)
      | BIT(bcmFieldStatYellowPackets) | BIT(bcmFieldStatYellowBytes)
      | BIT(bcmFieldStatGreenPackets) | BIT(bcmFieldStatGreenBytes)
      | BIT(bcmFieldStatNotRedPackets) | BIT(bcmFieldStatNotRedBytes)
      | BIT(bcmFieldStatNotYellowPackets) | BIT(bcmFieldStatNotYellowBytes)
      | BIT(bcmFieldStatNotGreenPackets) | BIT(bcmFieldStatNotGreenBytes)
      | BIT(bcmFieldStatPackets) | BIT(bcmFieldStatBytes),
      3, _bcmFieldStatColorGreenYellowRed
    }
};
/*
 * Function:
 *    _field_th_instance_meters_deinit
 *
 * Purpose:
 *    Free instance meters usage bitmap.
 *
 * Parameters:
 *    unit     - (IN) BCM device number.
 *    stage_fc - (IN/OUT) Stage field control structure.
 *    inst     - (IN) Device stage instance information.
 *
 * Returns:
 *    BCM_E_PARAM  - Invalid instance number.
 *    BCM_E_NONE   - Success.
 */
STATIC int
_field_th_instance_meters_deinit(int unit, _field_stage_t *stage_fc, int inst)
{
    int pool; /* Meter pool iterator. */
    _field_meter_pool_t *mp; /* Meter pool structure pointer. */
    int slice; /* Slice iterator. */
    _field_slice_t *fs; /* Slice configuration structure. */

    /* Input parameter check. */
    if (NULL == stage_fc) {
        return (BCM_E_NONE);
    }

    /* Validate instance value. */
    if (inst < 0 || inst >= stage_fc->num_instances) {
        return (BCM_E_PARAM);
    }

    /* Make sure slices array is initialized.*/
    if (NULL == stage_fc->slices[inst]) {
        return (BCM_E_NONE);
    }

    /* Free IFP meter pools belonging to instance. */
    if (stage_fc->flags & _FP_STAGE_GLOBAL_METER_POOLS) {
        for (pool = 0; pool < stage_fc->num_meter_pools; pool++) {
            /* Get the instance meter pool structure pointer. */
            mp = stage_fc->meter_pool[inst][pool];
            if (NULL != mp) {
                if (NULL != mp->meter_bmp.w) {
                    sal_free(mp->meter_bmp.w);
                }
                sal_free(mp);
                stage_fc->meter_pool[inst][pool] = NULL;
            }
        }
        return (BCM_E_NONE);
    }

    /* Free EFP meter usage bitmap resources belonging to an instance. */
    for (slice = 0; slice < stage_fc->tcam_slices; slice++) {
        fs = stage_fc->slices[inst] + slice;
        if (NULL != fs->meter_bmp.w) {
            sal_free(fs->meter_bmp.w);
        }
        fs->meter_bmp.w = NULL;
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *    _field_th_instance_entries_free
 *
 * Purpose:
 *    Free entries array for all slices in an instance.
 *
 * Parameters:
 *    unit     - (IN) BCM device number.
 *    stage_fc - (IN) Stage field control structure.
 *    inst - (IN) Device stage instance number information.
 *
 * Returns:
 *    BCM_E_PARAM - Device stage control information is NULL.
 *    BCM_E_NONE  - Success.
 */
STATIC int
_field_th_instance_entries_free(int unit,
                                _field_stage_t *stage_fc,
                                int inst)
{
    _field_slice_t *fs; /* Field Slice Pointer. */
    int slice; /* Slice iterator. */

    /* Input parameters check. */
    if (NULL == stage_fc) {
        return (BCM_E_PARAM);
    }

    /* Validate instance value. */
    if (inst < 0 || inst >= stage_fc->num_instances) {
        return (BCM_E_PARAM);
    }

    /* Deallocate instance entry pointers. */
    for (slice = 0; slice < stage_fc->tcam_slices; slice++) {
        fs = stage_fc->slices[inst] + slice;
        if (NULL != fs->entries) {
            sal_free(fs->entries);
            fs->entries = NULL;
        }
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *    _field_th_instance_prio_mgmt_deinit
 *
 * Purpose:
 *    Free memory used by priority management in an instance.
 *
 * Parameters:
 *    unit     - (IN) BCM device number.
 *    stage_fc - (IN) Stage field control structure.
 *    inst     - (IN) Device stage instance number information.
 *
 * Returns:
 *     BCM_E_PARAM - Stage control null or invalid instance.
 *     BCM_E_NONE - Success.
 */
STATIC int
_field_th_instance_prio_mgmt_deinit(int unit,
                                    _field_stage_t *stage_fc,
                                    int inst)
{
    int slice; /* Slice iterator. */
    _field_prio_mgmt_t *prio1, *prio2;

    /* Input parameters check. */
    if (NULL == stage_fc) {
        return (BCM_E_PARAM);
    }

    /* Validate instance value. */
    if (inst < 0 || inst >= stage_fc->num_instances) {
        return (BCM_E_PARAM);
    }

    if (_BCM_FIELD_STAGE_INGRESS == stage_fc->stage_id) {
        /* To be handled as part of IFP Group implementation. */
        return (BCM_E_NONE);
    }

    for (slice = 0; slice < stage_fc->tcam_slices; slice++) {
        prio1 = stage_fc->slices[inst][slice].prio_mgmt;
        while (prio1 != NULL) {
            prio2 = prio1->next;
            sal_free(prio1);
            prio1 = prio2;
        }
        stage_fc->slices[inst][slice].prio_mgmt = NULL;
    }

    return (BCM_E_NONE);
}
/*
 * Function:
 *     _bcm_field_th_data_qualifier_hw_alloc
 *
 * Purpose:
 *     Allocate hw resources for data qualifier installation
 *
 * Parameters:
 *     unit       - (IN) BCM device number. 
 *     stage_fc   - (IN) Stage field control.
 *     f_dq       - (IN) Field data qualifier descriptor.
 * Returns:
 *     BCM_E_XXX 
 */

int
_bcm_field_th_data_qualifier_hw_alloc(int unit,
                                      _field_stage_t *stage_fc,
                                      _field_data_qualifier_t *f_dq)
{
    int         idx;                            /* Data qualifier word iterator.            */
    int         elem_count;                     /* Number of elements allocated.            */
    int         temp_elem_count = 0;            /* Number of elements allocated.            */
    uint32      *usage_bmap;                    /* Offset usage bitmap.                     */
    SHR_BITDCL  bit16_chunks_free;              /* Bitmap of free 16 bit chunks             */
    SHR_BITDCL  bit32_chunks_free;              /* Bitmap of free 32 bit chunks             */
    SHR_BITDCL  bit32_chunks_free_partially;    /* Bitmap of 32 bit chunks free partially   */
    
    /* Input parameters check. */
    if ((NULL == f_dq) || (NULL == stage_fc)) {
        return (BCM_E_PARAM);
    }

     memset(&bit16_chunks_free, 0x0, sizeof(bit16_chunks_free));
     memset(&bit32_chunks_free, 0x0, sizeof(bit32_chunks_free));
     memset(&bit32_chunks_free_partially, 0x0, sizeof(bit32_chunks_free_partially));

    elem_count = 0;
    /* coverity[assign]: FALSE*/
    usage_bmap = &stage_fc->data_ctrl->usage_bmap;

    /* Loop through the usage_bmap and calculate the number of elements 
     * which are free to be allocated*/
    for (idx = 0; idx <= 15; idx++) {
        if (0 == ((*usage_bmap) & (1 << idx))) {
            elem_count++;
        }
    }
  
    /* Loop through the usage_bmap and identify which of the 16_bit(Chunks 0,1,8,9)      
     * chunks are free to be allocated and set the corresponding idx in 
     * bit16_chunks_free*/

    for (idx =0; idx <= 15; idx++) {
        if (idx == 0x0 || idx == 0x1 || idx == 0x8 || idx == 0x9) {
            if (SHR_BITGET(usage_bmap, idx) == 0) {
                /* coverity[ptr_arith] we check only from 0 to 15 idx and
                 * there are only 16 elements in UDF currently*/
                 SHR_BITSET(&bit16_chunks_free, idx);
            }
        }
    }

    /* Loop through the usage_bmap and identify which of the 32_bit chunks (Chunks        
     * other than 0,1,8,9) are free to be allocated. For a 32 bit chunk to be free fully
     * both elems in idx and idx+1 should be free. If either one of them is free, then 
     * it is taken as a partially free 32 bit chunk*/

    for (idx =0; idx < 15; idx += 2) {
        if (idx != 0x0 && idx != 0x1 && idx != 0x8 && idx != 0x9) {
            if ((SHR_BITGET(usage_bmap, idx) == 0) && 
                (SHR_BITGET(usage_bmap, idx + 1) == 0)) {
                /* coverity[ptr_arith] we check only from 0 to 15 idx and
                 * there are only 16 elements in UDF currently*/
                 SHR_BITSET(&bit32_chunks_free, idx); 
                /* coverity[ptr_arith] we check only from 0 to 15 idx and
                 * there are only 16 elements in UDF currently*/
                 SHR_BITSET(&bit32_chunks_free, (idx+1));
            } else if (SHR_BITGET(usage_bmap, idx) == 0) {
                /* coverity[ptr_arith] we check only from 0 to 15 idx and
                 * there are only 16 elements in UDF currently*/
                SHR_BITSET(&bit32_chunks_free_partially, idx);
            } else if (SHR_BITGET(usage_bmap, idx + 1) == 0) {
                /* coverity[ptr_arith] we check only from 0 to 15 idx and
                 * there are only 16 elements in UDF currently*/
                SHR_BITSET(&bit32_chunks_free_partially, (idx+1));
            }
        }
    }

    /* Check whether the requested elem count is available. Only if the count 
     * requested can be allocated, proceed. */
    if (elem_count >= f_dq->elem_count) {

        if (1 == f_dq->elem_count) {
            
            /*Only UDF2.4 through UDF2.7 chunks are specific to Flex Hash Rtag7. 
             *Hence only the data qualifiers - Data8 and Data9 can be allocated for
             *Flex Hash. If only one element is requested, the order 
             *of allocation is as follows: 
             * 1. If any 32 bit chunk is partially free, allocate it.
             * 2. If any 32 bit chunk is free as a whole, allocate it. */  

            if (f_dq->flags == BCM_FIELD_DATA_QUALIFIER_OFFSET_FLEX_HASH) { 
                for (idx = 12; idx <= 15; idx++) {
                    if (0 != ((bit32_chunks_free_partially) & (1 << idx))) {
                        f_dq->hw_bmap |= (1 << idx);
                        (*usage_bmap) |= (1 << idx);
                         return BCM_E_NONE;
                    }                    
                }

                for (idx = 12; idx <= 15; idx++) {
                    if (0 != ((bit32_chunks_free) & (1 << idx))) {
                        f_dq->hw_bmap |= (1 << idx);
                        (*usage_bmap) |= (1 << idx);
                        return BCM_E_NONE;
                    }
                }
                return BCM_E_RESOURCE;
            }

            /*If only one element is requested, then the order of allocation is as follows:
             * 1. If any 16_bit chunk is free, allocate it.
             * 2. Else If any 32 bit chunk is partially free, allocate it.
             * 3. Else If any 32 bit chunk is free as a whole, allocate it.  
             * (UDF2.4 thro UDF2.7 chunks are specific to RTAG7). Hence, in  all the above
             * cases, give first preference to chunks other than those used for RTAGS.
             */
       
            for (idx = 0; idx <= 15; idx++) {
                 if (0 != ((bit16_chunks_free) & (1 << idx))) {
                     f_dq->hw_bmap |= (1 << idx);
                     (*usage_bmap) |= (1 << idx);
                     return BCM_E_NONE;
                 }

            }

            for (idx = 0; idx <= 11; idx++) {
                 if (0 != ((bit32_chunks_free_partially) & (1 << idx))) {
                     f_dq->hw_bmap |= (1 << idx);
                     (*usage_bmap) |= (1 << idx);
                     return BCM_E_NONE;
                 }
            }

            for (idx = 0; idx <= 11; idx++) {
                 if (0 != ((bit32_chunks_free) & (1 << idx))) {
                     f_dq->hw_bmap |= (1 << idx);
                     (*usage_bmap) |= (1 << idx);
                     return BCM_E_NONE;
                 }
            }
            
            for (idx = 12; idx <= 15; idx++) {
                 if (0 != ((bit32_chunks_free_partially) & (1 << idx))) {
                     f_dq->hw_bmap |= (1 << idx);
                     (*usage_bmap) |= (1 << idx);
                     return BCM_E_NONE;
                 }
            }

            for (idx = 12; idx <= 15; idx++) {
                 if (0 != ((bit32_chunks_free) & (1 << idx))) {
                     f_dq->hw_bmap |= (1 << idx);
                     (*usage_bmap) |= (1 << idx);
                     return BCM_E_NONE;
                 }
            }

        } else {
            
            temp_elem_count = f_dq->elem_count;
            
            /*Only UDF2.4 through UDF2.7 chunks are specific to Flex Hash Rtag7. 
             *Hence only the data qualifiers - Data8 and Data9 can be allocated for
             *Flex Hash. */

            if (f_dq->flags == BCM_FIELD_DATA_QUALIFIER_OFFSET_FLEX_HASH) { 
            /*If more than one element is requested, then the order of allocation is as follows: 
             * Save elem_count requested in temp_elem_count and keep decreasing it as we allocate
             *1. Until temp_elem_count is not 1, allocate 32_bit chunks as much as possible
             *2. If more chunks are needed, allocate from 32_bit chunks partially free
             *3. If more chunks are needed, allocate partially from 32_bit chunks fully free
             */
                for (idx = 12; idx < 15; idx += 2) {
                    if (1 != temp_elem_count) {
                        if ((0 != ((bit32_chunks_free) & (1 << idx))) &&
                            (0 != ((bit32_chunks_free) & (1 << (idx + 1))))) {
                            f_dq->hw_bmap |= (1 << idx);
                            (*usage_bmap) |= (1 << idx);
                            f_dq->hw_bmap |= (1 << (idx + 1));
                            (*usage_bmap) |= (1 << (idx + 1));
                            temp_elem_count = temp_elem_count - 2;
                        }
                    }

                    if (0 == temp_elem_count) {
                        return BCM_E_NONE;
                    }
                }

                for (idx = 12; idx <= 15; idx++) {
                    if (0 != ((bit32_chunks_free_partially) & (1 << idx))) {
                        f_dq->hw_bmap |= (1 << idx);
                        (*usage_bmap) |= (1 << idx);
                        temp_elem_count = temp_elem_count - 1;
                    }

                    if (0 == temp_elem_count) {
                        return BCM_E_NONE;
                    }
                }
                
                for (idx = 12; idx <= 15; idx++) {
                    if (0 != ((bit32_chunks_free) & (1 << idx))) {
                        f_dq->hw_bmap |= (1 << idx);
                        (*usage_bmap) |= (1 << idx);
                        temp_elem_count = temp_elem_count - 1;
                    }

                    if (0 == temp_elem_count) {
                        return BCM_E_NONE;
                    }
                }
                return BCM_E_RESOURCE;
            }

            /*If more than one element is requested, then the order of allocation is as follows: 
             * Save elem_count requested in temp_elem_count and keep decreasing it as we allocate
             *1. Until temp_elem_count is not 1, allocate 32_bit chunks as much as possible
             *2. If more chunks are needed, allocate from 16_bit chunks             
             *3. If more chunks are needed, allocate from 32_bit chunks partially free
             *4. If more chunks are needed, allocate partially from 32_bit chunks fully free
             */
            
            for (idx = 0; idx < 11; idx += 2) {
                if (1 != temp_elem_count) {
                    if ((0 != ((bit32_chunks_free) & (1 << idx))) &&
                        (0 != ((bit32_chunks_free) & (1 << (idx + 1))))) {
                        f_dq->hw_bmap |= (1 << idx);
                        (*usage_bmap) |= (1 << idx);
                        f_dq->hw_bmap |= (1 << (idx + 1));
                        (*usage_bmap) |= (1 << (idx + 1));
                        temp_elem_count = temp_elem_count - 2;
                    }

                    if (0 == temp_elem_count) {
                        return BCM_E_NONE;
                    }
                }
            }

            for (idx = 0; idx <= 15; idx++) {
                if (0 != ((bit16_chunks_free) & (1 << idx))) {
                    f_dq->hw_bmap |= (1 << idx);
                    (*usage_bmap) |= (1 << idx);
                    temp_elem_count = temp_elem_count - 1;
                }

                if (0 == temp_elem_count) {
                    return BCM_E_NONE;
                }
            }

            for (idx = 0; idx <= 11; idx++) {
                if (0 != ((bit32_chunks_free_partially) & (1 << idx))) {
                    f_dq->hw_bmap |= (1 << idx);
                    (*usage_bmap) |= (1 << idx);
                    temp_elem_count = temp_elem_count - 1;
                }

                if (0 == temp_elem_count) {
                    return BCM_E_NONE;
                }
            }

            for (idx = 0; idx <= 11; idx++) {
                if (0 != ((bit32_chunks_free) & (1 << idx))) {
                    f_dq->hw_bmap |= (1 << idx);
                    (*usage_bmap) |= (1 << idx);
                    temp_elem_count = temp_elem_count - 1;
                }

                if (0 == temp_elem_count) {
                    return BCM_E_NONE;
                }
            }
           
            /*Give last preference to data qualifiers Data8 and Data9 (marking UDF2.4 
             *through UDF2.7) chunks as they are specific to RTAG7*/
            for (idx = 12; idx < 15; idx += 2) {
                if (1 != temp_elem_count) {
                    if ((0 != ((bit32_chunks_free) & (1 << idx))) &&
                        (0 != ((bit32_chunks_free) & (1 << (idx + 1))))) {
                        f_dq->hw_bmap |= (1 << idx);
                        (*usage_bmap) |= (1 << idx);
                        f_dq->hw_bmap |= (1 << (idx + 1));
                        (*usage_bmap) |= (1 << (idx + 1));
                        temp_elem_count = temp_elem_count - 2;
                    }

                    if (0 == temp_elem_count) {
                        return BCM_E_NONE;
                    }
                }
            }

            for (idx = 12; idx <= 15; idx++) { 
                if (0 != ((bit32_chunks_free_partially) & (1 << idx))) {
                    f_dq->hw_bmap |= (1 << idx);
                    (*usage_bmap) |= (1 << idx);
                    temp_elem_count = temp_elem_count - 1;
                }

                if (0 == temp_elem_count) {                    
                    return BCM_E_NONE;
                }
            }

            for (idx = 12; idx <= 15; idx++) {
                if (0 != ((bit32_chunks_free) & (1 << idx))) {
                    f_dq->hw_bmap |= (1 << idx);
                    (*usage_bmap) |= (1 << idx);
                    temp_elem_count = temp_elem_count - 1;
                }

                if (0 == temp_elem_count) {
                    return BCM_E_NONE;
                }
            }            
        }                   
    }

    return (BCM_E_RESOURCE);

}

/*
 * Function:
 *     _bcm_field_th_field_qset_data_qualifier_add
 *
 * Purpose: 
 *     Add field data qualifier to group qset.
 *
 * Parameters:
 *      unit          - (IN) bcm device.
 *      qset          - (IN/OUT) Group qualifier set.
 *      qualifier_id  - (IN) Data qualifier id.
 *
 * Returns:
 *      BCM_E_XXX
 */
int _bcm_field_th_field_qset_data_qualifier_add(int unit, 
                                                bcm_field_qset_t *qset, 
                                                int qual_id)
{
    _field_stage_t          *stage_fc;      /* Stage field control.       */
    _field_data_qualifier_t *f_dq;          /* Data qualifier descriptor. */
    _field_control_t        *fc;            /* Field control structure.   */
    int                     idx;            /* HW bitmap iteration index. */
    int                     rv;             /* Operation return status.   */
    int                     offset_2_3 = 0;  /* HW offset 0 and 1 usage status. */
    int                     offset_4_5 = 0;  /* HW offset 2 and 3 usage status. */
    int                     offset_6_7 = 0;  /* HW offset 4 and 5 usage status. */
    int                     offset_a_b = 0;  /* HW offset 8 and 9 usage status. */
    int                     offset_c_d = 0;  /* HW offset a and b usage status. */
    int                     offset_e_f = 0;  /* HW offset c and d usage status. */

    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));
    FP_LOCK(fc);

    /* Get stage field control structure. */
    rv = _field_stage_control_get(unit, _BCM_FIELD_STAGE_INGRESS, &stage_fc); 
    if (BCM_FAILURE(rv)) {
        FP_UNLOCK(fc);
        return (rv);
    }

    /*Get data qualifier info. */
    rv = _bcm_field_data_qualifier_get(unit, stage_fc, qual_id,  &f_dq);
    if (BCM_FAILURE(rv)) {
        FP_UNLOCK(fc);
        return (rv);
    }


    /*--------------------------------------------------------------------------------------*
     * Each chunk or chunk pair corresponds to the data qualifier as marked below:
     *
     * 15 -- 14   13 -- 12    11 -- 10    9    8     7 -- 6    5 -- 4   3 -- 2   1     0
     *{________}  {_______}   {_______} {___}{___}  {______}  {______}  {_____} {___}{___}
     *   Data9      Data8       Data7   Data6 Data5   Data4    Data3     Data2  Data1 Data0
     *  (32bit)    (32bit)    (32bit)  (16bit)(16bit) (32bit) (32bit)   (32bit)(16bit)(16bit)
     * {_________________________________________}   {___________________________________} 
     *                     ||                                          ||
     *                    UDF_2                                       UDF_1
     *--------------------------------------------------------------------------------------*/

    /* Loop through the hw_map and set the correspoding bit in qset's udf_map*/
   
    for (idx = 0; idx <= 15; idx++) {
        if (f_dq->hw_bmap & (1 << idx)) {
            SHR_BITSET(qset->udf_map, idx);
        }
    }
    
    /* Add the qualifiers corresponding to the bit in qset->udf_map. Like, 
     * 16_bit qualifiers are _bcmFieldQualifyData0, _bcmFieldQualifyData1, 
     *_bcmFieldQualifyData5,_bcmFieldQualifyData6 and map 0,1,8,9 respectively*/

    if (SHR_BITGET(qset->udf_map, 0x0)) { 
        BCM_FIELD_QSET_ADD_INTERNAL(*qset, _bcmFieldQualifyData0);
    }
    if (SHR_BITGET(qset->udf_map, 0x1)) {
        BCM_FIELD_QSET_ADD_INTERNAL(*qset, _bcmFieldQualifyData1);
    }
    if (SHR_BITGET(qset->udf_map, 0x8)) {
        BCM_FIELD_QSET_ADD_INTERNAL(*qset, _bcmFieldQualifyData5);
    }
    if (SHR_BITGET(qset->udf_map, 0x9)) {
        BCM_FIELD_QSET_ADD_INTERNAL(*qset, _bcmFieldQualifyData6);
    }
   
    /* 32_bit qualifiers are over a range of udf_map and hence 
     * get HW offsets usage status. */

    SHR_BITTEST_RANGE(qset->udf_map, 0x2, 2, offset_2_3);
    SHR_BITTEST_RANGE(qset->udf_map, 0x4, 2, offset_4_5);
    SHR_BITTEST_RANGE(qset->udf_map, 0x6, 2, offset_6_7);
    SHR_BITTEST_RANGE(qset->udf_map, 0xa, 2, offset_a_b);
    SHR_BITTEST_RANGE(qset->udf_map, 0xc, 2, offset_c_d);
    SHR_BITTEST_RANGE(qset->udf_map, 0xe, 2, offset_e_f);
   
    if (offset_2_3) {
        BCM_FIELD_QSET_ADD_INTERNAL(*qset, _bcmFieldQualifyData2);
    }
    if (offset_4_5) {
        BCM_FIELD_QSET_ADD_INTERNAL(*qset, _bcmFieldQualifyData3);
    }
    if (offset_6_7) {
        BCM_FIELD_QSET_ADD_INTERNAL(*qset, _bcmFieldQualifyData4);
    }
    if (offset_a_b) {
        BCM_FIELD_QSET_ADD_INTERNAL(*qset, _bcmFieldQualifyData7);
    }
    if (offset_c_d) {
        BCM_FIELD_QSET_ADD_INTERNAL(*qset, _bcmFieldQualifyData8);
    }
    if (offset_e_f) {
        BCM_FIELD_QSET_ADD_INTERNAL(*qset, _bcmFieldQualifyData9);
    }
    FP_UNLOCK(fc);
    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_field_th_field_qset_data_qualifier_delete
 * Purpose:
 *      Delete field data qualifier from group qset.
 * Parameters:
 *      unit          - (IN) bcm device.
 *      qset          - (IN/OUT) Group qualifier set.
 *      qualifier_id  - (IN) Data qualifier id.
 * Returns:
 *      BCM_E_XXX
 */
int _bcm_field_th_field_qset_data_qualifier_delete (int unit, bcm_field_qset_t *qset,
                                                int qual_id)
{
    _field_stage_t          *stage_fc;  /* Stage field control.       */
    _field_data_qualifier_t *f_dq;      /* Data qualifier descriptor. */
    _field_control_t        *fc;        /* Field control structure.   */
    int                     idx;        /* HW bitmap iteration index. */
    int                     rv;         /* Operation return status.   */
    int                     offset_2_3 = 0;  /* HW offset 0 and 1 usage status. */
    int                     offset_4_5 = 0;  /* HW offset 2 and 3 usage status. */
    int                     offset_6_7 = 0;  /* HW offset 4 and 5 usage status. */
    int                     offset_a_b = 0;  /* HW offset 8 and 9 usage status. */
    int                     offset_c_d = 0;  /* HW offset a and b usage status. */
    int                     offset_e_f = 0;  /* HW offset c and d usage status. */
    
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));
    FP_LOCK(fc);

    /* Get stage field control structure. */
    rv = _field_stage_control_get(unit, _BCM_FIELD_STAGE_INGRESS, &stage_fc); 
    if (BCM_FAILURE(rv)) {
        FP_UNLOCK(fc);
        return (rv);
    }

    /*Get data qualifier info. */
    rv = _bcm_field_data_qualifier_get(unit, stage_fc, qual_id,  &f_dq);
    if (BCM_FAILURE(rv)) {
        FP_UNLOCK(fc);
        return (rv);
    }

    /* Loop through the hw_map and clear the correspoding bit in qset's udf_map*/
    for (idx = 0; idx <= 15; idx++)
    {
        if (f_dq->hw_bmap & (1 << idx)) {
            SHR_BITCLR(qset->udf_map, idx);
        }
    }

    if (!SHR_BITGET(qset->udf_map, 0x0)) {
        BCM_FIELD_QSET_REMOVE_INTERNAL(*qset, _bcmFieldQualifyData0);
    }

    if (!SHR_BITGET(qset->udf_map, 0x1)) {
        BCM_FIELD_QSET_REMOVE_INTERNAL(*qset, _bcmFieldQualifyData1);
    }

    if (!SHR_BITGET(qset->udf_map, 0x8)) {
        BCM_FIELD_QSET_REMOVE_INTERNAL(*qset, _bcmFieldQualifyData5);
    }

    if (!SHR_BITGET(qset->udf_map, 0x9)) {
        BCM_FIELD_QSET_REMOVE_INTERNAL(*qset, _bcmFieldQualifyData6);
    }

    /* Get HW offsets usage status. */
    SHR_BITTEST_RANGE(qset->udf_map, 0x2, 2, offset_2_3);
    SHR_BITTEST_RANGE(qset->udf_map, 0x4, 2, offset_4_5);
    SHR_BITTEST_RANGE(qset->udf_map, 0x6, 2, offset_6_7);
    SHR_BITTEST_RANGE(qset->udf_map, 0xa, 2, offset_a_b);
    SHR_BITTEST_RANGE(qset->udf_map, 0xc, 2, offset_c_d);
    SHR_BITTEST_RANGE(qset->udf_map, 0xe, 2, offset_e_f);
   
    if (!offset_2_3) {
        BCM_FIELD_QSET_REMOVE_INTERNAL(*qset, _bcmFieldQualifyData2);
    }
    if (!offset_4_5) {
        BCM_FIELD_QSET_REMOVE_INTERNAL(*qset, _bcmFieldQualifyData3);
    }
    if (!offset_6_7) {
        BCM_FIELD_QSET_REMOVE_INTERNAL(*qset, _bcmFieldQualifyData4);
    }
    if (!offset_a_b) {
        BCM_FIELD_QSET_REMOVE_INTERNAL(*qset, _bcmFieldQualifyData7);
    }
    if (!offset_c_d) {
        BCM_FIELD_QSET_REMOVE_INTERNAL(*qset, _bcmFieldQualifyData8);
    }
    if (!offset_e_f) {
        BCM_FIELD_QSET_REMOVE_INTERNAL(*qset, _bcmFieldQualifyData9);
    }
    
    FP_UNLOCK(fc);
    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_field_th_field_qset_data_qualifier_get
 * Purpose:
 *      Fetch the data qualifier that maps the index
 * Parameters:
 *      idx           - (IN) Index of the element
 *      qset          - (IN) Group qualifier set.
 *      qualifier_id  - (OUT) Data qualifier id.
 * Returns:
 *      BCM_E_XXX
 */
int _bcm_field_th_field_qset_data_qualifier_get(int idx, 
        bcm_field_qset_t qset,
        bcm_field_qualify_t *qid) 
{
    if (idx < 0 || idx > 15) {
        return (BCM_E_PARAM);
    }
    
    if (NULL == qid) {
        return (BCM_E_PARAM);
    }

    /* Return the qualifier id, if the qualifier_id is set in the Qset
     * and its corresponding index value is the same mapped with it*/
    if ((idx == 0) && 
            BCM_FIELD_QSET_TEST_INTERNAL(qset,
                _bcmFieldQualifyData0)) {
        *qid = _bcmFieldQualifyData0;
    } else if ((idx == 1) && 
            BCM_FIELD_QSET_TEST_INTERNAL(qset,
                _bcmFieldQualifyData1)) {
        *qid     = _bcmFieldQualifyData1;
    } else if (((idx == 2) || (idx == 3)) && 
            BCM_FIELD_QSET_TEST_INTERNAL(qset,
                _bcmFieldQualifyData2)) {
        *qid = _bcmFieldQualifyData2;
    } else if (((idx == 4) || (idx == 5)) && 
            BCM_FIELD_QSET_TEST_INTERNAL(qset,
                _bcmFieldQualifyData3)) {
        *qid = _bcmFieldQualifyData3;
    } else if (((idx == 6) || (idx == 7)) && 
            BCM_FIELD_QSET_TEST_INTERNAL(qset,
                _bcmFieldQualifyData4)) {
        *qid = _bcmFieldQualifyData4;
    } else if ((idx == 8) && 
            BCM_FIELD_QSET_TEST_INTERNAL(qset,
                _bcmFieldQualifyData5)) {
        *qid = _bcmFieldQualifyData5;
    } else if ((idx == 9) && 
            BCM_FIELD_QSET_TEST_INTERNAL(qset,
                _bcmFieldQualifyData6)) {
        *qid = _bcmFieldQualifyData6;
    } else if (((idx == 0xa) || (idx == 0xb)) && 
            BCM_FIELD_QSET_TEST_INTERNAL(qset,
                _bcmFieldQualifyData7)) {
        *qid = _bcmFieldQualifyData7;
    } else if (((idx == 0xc) || (idx == 0xd)) && 
            BCM_FIELD_QSET_TEST_INTERNAL(qset,
                _bcmFieldQualifyData8)) {
        *qid = _bcmFieldQualifyData8;
    } else if (((idx == 0xe) || (idx == 0xf)) &&
            BCM_FIELD_QSET_TEST_INTERNAL(qset,
                _bcmFieldQualifyData9)) {
        *qid = _bcmFieldQualifyData9;
    } else {
        return (BCM_E_INTERNAL);
    }

    return BCM_E_NONE;
}
/*
 * Function: _bcm_field_th_field_qualify_data_elem
 *
 * Purpose:
 *
 * Parameters:
 *     unit          - (IN) BCM device number.
 *     eid           - (IN) Entry ID.
 *     f_dq          - (IN) Data qualifier descriptor.
 *     qual_elem_idx - (IN) Data qualifier element index.
 *     data          - (IN) Data bytes for the indicated data qualifier.
 *                Network byte ordered. 
 *     mask     - Mask bytes for the indicated data qualifier.
 *                Network byte ordered.
 * Returns:
 *     BCM_E_XXX   
 */
int
_bcm_field_th_field_qualify_data_elem(int unit, bcm_field_entry_t eid,
                         _field_data_qualifier_t *f_dq,
                         uint8 qual_elem_idx,
                         uint32 data, uint32 mask)
{
    _field_stage_t           *stage_fc;         /* Stage field control.       */
    _field_entry_t           *f_ent = NULL;     /* Field entry structure.     */ 
    int                      idx;               /* HW bitmap iteration index. */
    int                      rv;                /* Operation return status.   */
    int                      elem_idx = 0;      /* Qualifier Element Index.   */
    bcm_field_qualify_t      qid = 0;           /* Qualifier id.              */
    uint32                   q_data = 0;        /* Qualifier data             */
    uint32                   q_mask = 0;        /* Qualifier mask             */
    int                      bit32_chunk = 0;   /* Chunk is 32bit or 16bit    */

    /* Input parameters check. */
    if (NULL == f_dq) {
        return (BCM_E_PARAM);
    }

    BCM_IF_ERROR_RETURN
        (_field_stage_control_get(unit, _BCM_FIELD_STAGE_INGRESS, &stage_fc));

    /* Find a proper element to insert the data. */
    for (idx = 0; idx <= 15; idx ++) {
        if (f_dq->hw_bmap & (1 << idx)) {
            if (elem_idx == qual_elem_idx) {
                break;
            } else {
                elem_idx++;
            }
        }
    }
    
    if (idx != 0x0 && idx != 0x1 && idx != 0x8 && idx != 0x9)
    {
        bit32_chunk = 1;
    }
    
    /* Get entry info. */
    BCM_IF_ERROR_RETURN
        (_field_entry_get(unit, eid, _FP_ENTRY_PRIMARY, &f_ent));
        
    BCM_IF_ERROR_RETURN(_bcm_field_th_field_qset_data_qualifier_get(idx, f_ent->group->qset,&qid));

    if(bit32_chunk) {
        /*Since a 32 bit chunk can be allocated even partially, 
          do not overwrite existing data in 32bit chunk.*/
        rv = _bcm_field_entry_qualifier_uint32_get(unit, f_ent->eid, qid, &q_data, &q_mask);

        if ((rv != BCM_E_NOT_FOUND) && (BCM_FAILURE(rv))) {
            return rv;
        }

        if (rv == BCM_E_NONE) { 
            if((idx % 2) == 0) {
                /* Idx is even => Lower 16 bits need to be written.Hence,mask the upper
                 *16 bits of the q_data fetched from tcam and add the data*/
                q_data &= ~((1 << 16) - 1);
                q_mask &= ~((1 << 16) - 1);

                data |= q_data;
                mask |= q_mask;                
            } else {
                /* Idx is odd => Upper 16 bits need to be written.Hence,mask the lower
                 *16 bits of the q_data fetched from tcam and add the data*/
                q_data &= ((1 << 16) - 1);
                q_mask &= ((1 << 16) - 1);

                data = q_data << 16 | data;
                mask = q_mask << 16 | mask;
            }
        }
    }

    /* If device supports per-pipe FP configuration, */
    if (soc_feature(unit, soc_feature_field_multi_pipe_support) &&
            (f_ent->group->stage_id == _BCM_FIELD_STAGE_INGRESS)) {
        /* coverity[callee_ptr_arith : FALSE] */
        return (_bcm_field_th_qualify_set(unit, eid, qid,
                    &data, &mask));
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_field_th_field_qualify_data_get
 * Purpose:
 *      Get data/mask in the search engine for entry field data qualifier.
 * Parameters:
 *      unit          - (IN) bcm device.
 *      eid           - (IN) Entry id. 
 *      qual_id       - (IN) Data qualifier id.
 *      length_max    - (IN) Length of data & mask arrays.
 *      data          - (OUT) Match data.
 *      mask          - (OUT) Match mask.
 *      length        - (OUT) Length of data & mask arrays.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_field_th_field_qualify_data_get(int unit, bcm_field_entry_t eid, int qual_id,
                               uint16 length_max,  uint8 *data, uint8 *mask, 
                               uint16 *length)
{
    _field_stage_t          *stage_fc;                      /* Stage field control.       */
    _field_data_qualifier_t *f_dq;                          /* Data qualifier descriptor. */
    _field_control_t        *fc;                            /* Field control structure.   */
    _field_entry_t          *f_ent = NULL;                  /* Field entry structure.     */
    uint32                  data1;                          /* 32 bit qualifier data.     */
    uint32                  mask1;                          /* 32 bit qualifier mask.     */
    uint16                  data2;                          /* 16 bit qualifier data.     */
    uint16                  mask2;                          /* 16 bit qualifier mask.     */
    uint8                   *p_data1 = (uint8 *) &data1;    /* 32bit qualifier data pointer */
    uint8                   *p_mask1 = (uint8 *) &mask1;    /* 32bit qualifier mask pointer */
    uint8                   *p_data2 = (uint8 *) &data2;    /* 16bit qualifier data pointer */
    uint8                   *p_mask2 = (uint8 *) &mask2;    /* 16bit qualifier mask pointer */
    int                     bit32_chunk = 0;                /* Chunk is 32bit or 16bit    */
    bcm_field_qualify_t     qid = 0;                        /* Qualifier id.              */
    int                     idx;                            /* HW bitmap iteration index. */
    int                     rv;                             /* Operation return status.   */
    int                     shift = 0;
    int                     copy_len;
    int                     len = 0;
    #ifdef LE_HOST
    uint32 data_swap, mask_swap;
    #endif /* LE_HOST */

    /* Input parameters check. */
    if ((NULL == data) || (NULL == mask) ||
        (NULL == length) || (length_max == 0)) {
        return (BCM_E_PARAM);
    }
   
    memset(&data1, 0, sizeof(data1));
    memset(&mask1, 0, sizeof(mask1));
   
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    FP_LOCK(fc);

    /* Get stage field control structure. */
    rv = _field_stage_control_get(unit, _BCM_FIELD_STAGE_INGRESS, &stage_fc);
    if (BCM_FAILURE(rv)) {
        FP_UNLOCK(fc);
        return (rv);
    }

    /* Get data qualifier info. */
    rv = _bcm_field_data_qualifier_get(unit, stage_fc, qual_id,  &f_dq);
    if (BCM_FAILURE(rv)) {
        FP_UNLOCK(fc);
        return (rv);
    }

    /*
     * If provided entry data length is greater than data length used
     * to create qualifier then return error
     */

    if (length_max > f_dq->length) {
        FP_UNLOCK(fc);
        return (BCM_E_PARAM);
    }

    rv = _bcm_field_entry_get_by_id(unit, eid, &f_ent);
    if (BCM_FAILURE(rv)) {
        FP_UNLOCK(fc);
        return (rv);
    }

    for (idx = 0; idx <= 15; idx++) { 
        if (!(f_dq->hw_bmap & (1 << idx))) {
            continue;
        }
    
        if (idx != 0x0 && idx != 0x1 && idx != 0x8 && idx != 0x9) {
            bit32_chunk = 1;
        } else {
            bit32_chunk = 0;
        }

        /*Get the qualifier id matching the idx from the Group's qset*/
        BCM_IF_ERROR_RETURN(_bcm_field_th_field_qset_data_qualifier_get(idx, f_ent->group->qset,&qid));

        /*Fetch the data from the Tcam.*/
        if(bit32_chunk) {
            rv = _bcm_field_entry_qualifier_uint32_get(unit, eid, qid,
                    &data1, &mask1);
        } else {
            rv = _bcm_field_entry_qualifier_uint16_get(unit, eid, qid,
                    &data2, &mask2);
        }
#ifdef LE_HOST
        data_swap = BCMSWAP32(data1);
        data1 = data_swap;
        mask_swap = BCMSWAP32(mask1);
        mask1 = mask_swap;        
        data_swap = BCMSWAP16(data2);
        data2 = data_swap;
        mask_swap = BCMSWAP16(mask2);
        mask2 = mask_swap;        
#endif /* LE_HOST */

        if (len < f_dq->length) {
            if(bit32_chunk) {
                /* Idx is even => Lower 16 bits need to be copied. Hence,
                 * start from offset 0. Idx is odd => Upper 16 bits need to be
                 * copied, hence start from offset 2 of the 32bit chunk's data.*/
                shift = (idx % 2 == 0) ? 0 : 2 ;
                copy_len = ((f_dq->length - len) == 1)? 1 : 2;
                sal_memcpy((data + len), p_data1 + shift, copy_len);
                sal_memcpy((mask + len), p_mask1 + shift, copy_len);
                len+= copy_len;
            } else { 
                copy_len = ((f_dq->length - len) == 1)? 1 : 2;
                sal_memcpy((data + len), p_data2, copy_len);
                sal_memcpy((mask + len), p_mask2, copy_len);
                len+= copy_len;
            }
        }
    }

    *length = f_dq->length;

    FP_UNLOCK(fc);
    return BCM_E_NONE;
}
/*
 * Function:
 *    _field_th_data_qualifier_destroy_all
 *
 * Purpose:
 *    Destroy all data/offset  based qualifiers.
 *
 * Parameters:
 *    unit - (IN) BCM device number
 *
 * Returns:
 *    BCM_E_INIT    - Stage is not initialized.
 *    BCM_E_UNAVAIL - function not supported for this stage.
 *    BCM_E_NONE    - Success.
 */
STATIC int
_field_th_data_qualifier_destroy_all(int unit)
{
    _field_data_qualifier_t *f_dq; /* Internal data qualifier descriptor. */
    _field_stage_t *stage_fc; /* Stage field control. */

    BCM_IF_ERROR_RETURN(_field_stage_control_get(unit,
        _BCM_FIELD_STAGE_INGRESS, &stage_fc));

    while (NULL != stage_fc->data_ctrl->data_qual) {
        f_dq = stage_fc->data_ctrl->data_qual;
        BCM_IF_ERROR_RETURN(_bcm_field_data_qualifier_destroy(unit, f_dq->qid));
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *    _field_th_stage_data_ctrl_deinit
 *
 * Purpose:
 *    De-allocate data qualifiers control structure.
 *
 * Parameters:
 *    unit      - (IN) BCM device number
 *    stage_fc  - (IN/OUT) Stage for which data qualifier need to be freed.
 *
 * Returns:
 *     BCM_E_PARAM  - NULL Stage control data structure pointer.
 *     BCM_E_NONE   - Success
 */
STATIC int
_field_th_stage_data_ctrl_deinit(int unit, _field_stage_t *stage_fc)
{
    _field_data_control_t *data_ctrl; /* Data qualifiers control structure
                                         pointer. */

    /* Input parameters check. */
    if (NULL == stage_fc) {
        return (BCM_E_PARAM);
    }

    /* Free the data qualifiers control structure only for IFP. */
    if (_BCM_FIELD_STAGE_INGRESS != stage_fc->stage_id) {
        return (BCM_E_NONE);
    }

    /* If data qualifiers control structure is already freed, return SUCCESS. */
    data_ctrl = stage_fc->data_ctrl;
    if (NULL == data_ctrl) {
        return (BCM_E_NONE);
    }

    /* Delete all data qualifiers. */
    _field_th_data_qualifier_destroy_all(unit);

    /* Free control structures. */
    if (NULL != data_ctrl->tcam_entry_arr) {
        sal_free(data_ctrl->tcam_entry_arr);
    }

    /* Free data qualifiers control structure memory. */
    if (NULL != data_ctrl) {
        sal_free(data_ctrl);
    }

    /* Initialize stage data qualifier control structure pointer to NULL. */
    stage_fc->data_ctrl = NULL;
    return (BCM_E_NONE);
}

/*
 * Function:
 *    _field_th_stage_extractors_deinit
 *
 * Purpose:
 *    Deinitialize field stage extractors information.
 *
 * Parameters:
 *    unit     - (IN) BCM device number
 *    stage_fc - (IN) stage control structure.
 *
 * Returns:
 *    BCM_E_PARAM - Device Stage field control structure pointer is NULL.
 *    BCM_E_NONE  - Success
 */
STATIC int
_field_th_stage_extractors_deinit(int unit, _field_stage_t *stage_fc)
{
    _field_ext_conf_mode_t emode;       /* Extractor mode.              */
    _field_ext_cfg_t *ext_cfg;          /* Extractor configuration.     */
    _field_ext_sections_t *section;     /* Extractor section pointer.   */
    _field_ext_info_t *ext_info;        /* Extractor information.       */
    int level;                          /* Level iterator.              */
    int sec;                            /* Sections iterator.           */

    /* Input parameters check. */
    if (NULL == stage_fc) {
        return (BCM_E_PARAM);
    }

    /* Do nothing for Non-IFP stages. */
    if (_BCM_FIELD_STAGE_INGRESS != stage_fc->stage_id) {
        return (BCM_E_NONE);
    }

    /* Do nothing if it's already freed. */
    if (NULL == stage_fc->ext_cfg_arr) {
        return (BCM_E_NONE);
    }

    for (emode = 0; emode < _FieldExtConfModeCount; emode++) {

        /* Get extractor config array pointer. */
        ext_info = stage_fc->ext_cfg_arr[emode];
        if (NULL == ext_info) {
            continue;
        }

        /* Free extractor configuration memory allocated for all levels. */
        for (level = 0; level < _FP_EXT_LEVEL_COUNT; level++) {
            ext_cfg = stage_fc->ext_cfg_arr[emode]->ext_cfg[level];
            if (NULL != ext_cfg) {
                sal_free(ext_cfg);
            }
        }

        /* Free extractor sections memory for all sections. */
        for (sec = 0; sec < _FieldKeygenExtSelCount; sec++) {
            section = stage_fc->ext_cfg_arr[emode]->sections[sec];
            if (NULL != section) {
                sal_free(section);
            }
        }

        /* Free extractor sections pointers memory. */
        sal_free(stage_fc->ext_cfg_arr[emode]->sections);

        /* Free extractors configuration array. */
        if (NULL != ext_info) {
            sal_free(ext_info);
        }

        /* Mark extractor configuration as unused. */
        stage_fc->ext_cfg_arr[emode] = NULL;
    }

    /* Deallocate extractor configuration array pointers memory. */
    sal_free(stage_fc->ext_cfg_arr);

    /* Mark extractor configuration array as unused. */
    stage_fc->ext_cfg_arr = NULL;

    return (BCM_E_NONE);
}

/*
 * Function:
 *    _field_th_stage_quals_ibus_map_deinit
 *
 * Purpose:
 *    Deinitialize field stage input bus qualifiers list.
 *
 * Parameters:
 *    unit     - (IN) BCM device number
 *    stage_fc - (IN) stage control structure.
 *
 * Returns:
 *    BCM_E_PARAM - Device Stage field control structure pointer is NULL.
 *    BCM_E_NONE  - Success
 */
STATIC int
_field_th_stage_quals_ibus_map_deinit(int unit, _field_stage_t *stage_fc)
{
    _field_qual_sec_info_t *qual_sec;      /* Qualifier section info pointer. */
    _field_qual_sec_info_t *temp_qual_sec; /* Qualifier section info pointer. */
    int idx;                               /* Iterator index.                 */

    /* Input parameters check. */
    if (NULL == stage_fc) {
        return (BCM_E_PARAM);
    }

    /* Do nothing for Non-IFP stages. */
    if (_BCM_FIELD_STAGE_INGRESS != stage_fc->stage_id) {
        return (BCM_E_NONE);
    }

    /* Do nothing if already freed. */
    if (NULL == stage_fc->input_bus.qual_list) {
        return (BCM_E_NONE);
    }

    /* Deallocate qualifier sections memory. */
    for (idx = 0; idx < _bcmFieldQualifyCount; idx++) {
        qual_sec = stage_fc->input_bus.qual_list[idx];
        while(NULL != qual_sec) {
            temp_qual_sec = qual_sec;
            qual_sec = qual_sec->next;
            sal_free(temp_qual_sec);
        }
    }

    /* Deallocate qualifiers sections pointers memory. */
    sal_free(stage_fc->input_bus.qual_list);

    /* Mark qualifier list as unused. */
    stage_fc->input_bus.qual_list = NULL;

    return (BCM_E_NONE);
}
/*
 * Function:
 *    _field_th_stage_actions_deinit
 *
 * Purpose:
 *    Deinitialize field stage actions configuration array.
 *
 * Parameters:
 *    unit     - (IN) BCM device number
 *    stage_fc - (IN) stage control structure.
 *
 * Returns:
 *    BCM_E_PARAM - Device Stage field control structure pointer is NULL.
 *    BCM_E_NONE  - Success
 */
STATIC int
_field_th_stage_actions_deinit(int unit, _field_stage_t *stage_fc)
{
    int idx;                 /* Index to loop through  all field actions */
    _bcm_field_action_offset_t *offset;       /* Offset Information.     */

    /* Input parameters check. */
    if (NULL == stage_fc) {
        return (BCM_E_PARAM);
    }

    /* Do nothing for Non-IFP stages. */
    if (_BCM_FIELD_STAGE_INGRESS != stage_fc->stage_id) {
        return (BCM_E_NONE);
    }


    if (NULL != stage_fc->f_action_arr) {

        for (idx = 0; idx < bcmFieldActionCount; idx++) {

            if (NULL != stage_fc->f_action_arr[idx]) {

                if (NULL != stage_fc->f_action_arr[idx]->offset) {
                    do {
                        offset = stage_fc->f_action_arr[idx]->offset;
                        stage_fc->f_action_arr[idx]->offset =
                                   stage_fc->f_action_arr[idx]->offset->next;
                        /* Deallocate different offset information for same action. */
                        sal_free(offset);
                    } while(stage_fc->f_action_arr[idx]->offset);
                }
                /* Deallocate action configuration */
                sal_free(stage_fc->f_action_arr[idx]);
            }
        }

        /* Deallocate action configuration pointer array */
        sal_free(stage_fc->f_action_arr);
    }

    return BCM_E_NONE;
}
/*
 * Function:
 *    _field_th_stage_delete
 *
 * Purpose:
 *    Deinitialize a field stage.
 *
 * Parameters:
 *    unit     - (IN) BCM device number
 *    fc       - (IN/OUT) Field control info of device.
 *    stage_fc - (IN) stage control structure.
 *
 * Returns:
 *    BCM_E_PARAM - Device field control information is NULL.
 *    BCM_E_NONE  - Success
 */
STATIC int
_field_th_stage_delete(int unit, _field_control_t *fc, _field_stage_t *stage_fc)
{
    _field_stage_t *stage_iter; /* Device stages iterator. */
    int inst; /* Stage instance iterator. */

    /* Input parameters check. */
    if (NULL == fc) {
        return (BCM_E_PARAM);
    }

    /* Stage already destroyed. */
    if (NULL == stage_fc) {
        return (BCM_E_NONE);
    }

    /* Destroy data qualifiers control structure. */
    _field_th_stage_data_ctrl_deinit(unit, stage_fc);

    /* Destroy qualifier tables and qsets. */
    _bcm_field_stage_fpf_destroy(unit, stage_fc);

    /* Free memory allocated for extractors configuration table. */
    _field_th_stage_extractors_deinit(unit, stage_fc);

    /* Free memory allocated for qualifiers input bus mapping table. */
    _field_th_stage_quals_ibus_map_deinit(unit, stage_fc);

    /* Free memory allocated for actions configurations. */
    _field_th_stage_actions_deinit(unit, stage_fc);

    /* Free stage per-instance information. */
    for (inst = 0; inst < stage_fc->num_instances; inst++) {

        /*
         * Check if slices were initialized for current instance in the
         * stage.
         */
        if (NULL != stage_fc->slices[inst]) {

            /* Deallocate counters usage bitmap. */
            _field_th_instance_counters_deinit(unit, stage_fc, inst);

            /* Deallocate instance meters usage bitmap. */
            _field_th_instance_meters_deinit(unit, stage_fc, inst);

            /* Deallocate instance entry pointers. */
            _field_th_instance_entries_free(unit, stage_fc, inst);

            /* Deallocate instance priority management structures. */
            _field_th_instance_prio_mgmt_deinit(unit, stage_fc, inst);

            sal_free(stage_fc->slices[inst]);
            stage_fc->slices[inst] = NULL;
        }

        if (_BCM_FIELD_STAGE_INGRESS == stage_fc->stage_id) {
            /* Destroy keygen program profiles for valid instances. */
            soc_profile_mem_destroy(unit,
                &stage_fc->keygen_profile[inst].profile);
        }
    }

    if (_BCM_FIELD_STAGE_INGRESS == stage_fc->stage_id) {
       /* Deallocate Preselector resources. */
       _bcm_field_th_preselector_deinit(unit, fc, stage_fc);
    } 

    /* Destroy all valid range checkers. */
    while (stage_fc->ranges != NULL) {
        bcm_esw_field_range_destroy(unit, stage_fc->ranges->rid);
    }

    /* Destroy global redirection action profile. */
    soc_profile_mem_destroy(unit, &stage_fc->redirect_profile);

    /* Destroy class status info struct */
    if (stage_fc->stage_id == _BCM_FIELD_STAGE_CLASS) {
        _bcm_field_th_class_stage_status_deinit(unit, stage_fc);
    }

    /* Remove stage from stages linked list. */
    stage_iter = fc->stages;
    while (NULL != stage_iter) {
        if (stage_iter == stage_fc) {
            fc->stages = stage_fc->next;
            break;
        } else if (stage_iter->next == stage_fc) {
            stage_iter->next = stage_fc->next;
            break;
        }
        stage_iter = stage_iter->next;
    }

    /* Free stage info. */
    sal_free(stage_fc);
    stage_fc = NULL;

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_th_instance_slices_init
 *
 * Purpose:
 *    Allocate memory for slices and initialize slice specific flags &
 *    parameters for a specific instance.
 *
 * Parameters:
 *    unit      - (IN) BCM device number.
 *    stage_fc  - (IN/OUT) Stage field control structure.
 *    inst      - (IN) Stage instance information.
 *
 * Returns:
 *     BCM_E_PARAM  - NULL stage control pointer.
 *     BCM_E_MEMORY - Allocation failure.
 *     BCM_E_NONE   - Success.
 */
STATIC int
_field_th_instance_slices_init(int unit, _field_stage_t *stage_fc, int inst)
{
    struct _field_slice_s *fs; /* Slice info. */
    int slice; /* Slice iterator. */
    int mem_sz; /* Memory allocation buffer size. */

    /* Input parameters check. */
    if (NULL == stage_fc) {
        return (BCM_E_PARAM);
    }

    /* Validate instance value. */
    if (inst < 0 || inst >= stage_fc->num_instances) {
        return (BCM_E_PARAM);
    }

    /* Allocate slices info. */
    mem_sz = stage_fc->tcam_slices * sizeof(_field_slice_t);
    fs = sal_alloc(mem_sz, "stage slices info");
    if (NULL == fs) {
        return (BCM_E_MEMORY);
    }

    /* Initialze stage slice data structure. */
    sal_memset(fs, 0, mem_sz);

    /* Initialize stage instance slices pointer. */
    stage_fc->slices[inst] = fs;

    /* Initialize stage slices info. */
    for (slice = 0; slice < stage_fc->tcam_slices; slice++) {
        fs[slice].slice_number = slice;
        fs[slice].stage_id = stage_fc->stage_id;
        fs[slice].next = NULL;
        fs[slice].prev = NULL;

        /* Set legacy slice selector settings to don't care. */
        fs[slice].doublewide_key_select = _FP_SELCODE_DONT_CARE;
        fs[slice].src_class_sel = _FP_SELCODE_DONT_CARE;
        fs[slice].dst_class_sel = _FP_SELCODE_DONT_CARE;
        fs[slice].intf_class_sel = _FP_SELCODE_DONT_CARE;
        fs[slice].loopback_type_sel = _FP_SELCODE_DONT_CARE;
        fs[slice].ingress_entity_sel = _FP_SELCODE_DONT_CARE;
        fs[slice].src_entity_sel = _FP_SELCODE_DONT_CARE;
        fs[slice].dst_fwd_entity_sel = _FP_SELCODE_DONT_CARE;
        fs[slice].fwd_field_sel = _FP_SELCODE_DONT_CARE;

        /* Both IFP and VFP support Intra Slice Double Wide mode. */
        if (_BCM_FIELD_STAGE_EGRESS != stage_fc->stage_id) {
            fs[slice].slice_flags |= _BCM_FIELD_SLICE_INTRASLICE_CAPABLE;
        }
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *    _field_th_slices_init
 *
 * Purpose:
 *    Allocate memory for slices per-instance in a stage.
 *
 * Parameters:
 *    unit - (IN) BCM device number.
 *    stage_fc - (IN/OUT) Stage field control structure.
 *    fc - (IN/OUT) Field control structure.
 *
 * Returns:
 *     BCM_E_PARAM  - NULL stage control pointer or field control.
 *     BCM_E_MEMORY - Allocation failure.
 *     BCM_E_NONE   - Success.
 */
STATIC int
_field_th_slices_init(int unit, _field_stage_t *stage_fc, _field_control_t *fc)
{
    int inst; /* Stage instance iterator. */
    int rv; /* Operation return value. */

    /* Input parameters check. */
    if (NULL == fc || NULL == stage_fc) {
        return (BCM_E_PARAM);
    }

    /* For all valid instances, initialize slices information. */
    for (inst = 0; inst < stage_fc->num_instances; inst++) {
        rv = _field_th_instance_slices_init(unit, stage_fc, inst);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                "FP(unit %d) Error: _field_th_instance_slices_init=%d\n"),
                 unit, rv));
            return (rv);
        }
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *    _field_th_stage_entries_free
 *
 * Purpose:
 *    Free entries array for all stage slices for all instances.
 *
 * Parameters:
 *     unit     - (IN) BCM device number.
 *     stage_fc - (IN/OUT) Stage field control.
 *
 * Returns:
 *     BCM_E_PARAM  - NULL stage control pointer
 *     BCM_E_NONE   - Success.
 */
STATIC int
_field_th_stage_entries_free(int unit, _field_stage_t *stage_fc)
{
    _field_slice_t *fs; /* Field slice pointer. */
    int slice; /* Slice iterator. */
    int inst; /* Instance iterator. */

    /* Input prameters check. */
    if (NULL == stage_fc) {
        return (BCM_E_PARAM);
    }

    /* Deallocate instance entry pointers. */
    for (inst = 0; inst < stage_fc->num_instances; inst++) {
        for (slice = 0; slice < stage_fc->tcam_slices; slice++) {
            fs = stage_fc->slices[inst] + slice;
            if (NULL != fs->entries) {
                sal_free(fs->entries);
                fs->entries = NULL;
            }
        }
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_th_instance_entries_init
 * Purpose:
 *     Initialize Logical Table slice entries for a StageIngress instance.
 * Parameters:
 *     unit     - (IN) BCM device number.
 *     stage_fc - (IN) stage control structure pointer.
 *     inst     - (IN) Field Pipe Instance for which Logical tables must be
 *                     deinitialized.
 * Returns:
 *     BCM_E_PARAM - Device stage field control information is NULL
 *                   or Invalid instance specified.
 *     BCM_E_NONE  - Success
 */
STATIC int
_field_th_instance_entries_init(int unit, _field_stage_t *stage_fc, int inst)
{
    int slice; /* Slice iterator. */
    int tcam_idx; /* Slice first entry tcam index tracker. */
    int mem_sz; /* Allocation size. */
    _field_slice_t *fs; /* Slice config structure. */

    /* Input parameters check. */
    if (NULL == stage_fc) {
        return (BCM_E_PARAM);
    }

    /* Check if instance is in valid range. */
    if (inst < 0 || inst >= stage_fc->num_instances) {
        return (BCM_E_PARAM);
    }

    /* Make sure slices array is initialized. */
    if (NULL == stage_fc->slices[inst]) {
        return (BCM_E_INTERNAL);
    }

    /* Iterate over all stage's slices and allocate entries for each slice. */
    for (slice = 0, tcam_idx = 0; slice < stage_fc->tcam_slices; slice++) {
        /* Get Pipe slice control structure pointer. */
        fs = stage_fc->slices[inst] + slice;

        /*  Initialize entries per-slice count. */
        fs->entry_count = stage_fc->tcam_sz / stage_fc->tcam_slices;

        /* Calculate size for total entry pointers. */
        mem_sz = fs->entry_count * sizeof(void *);

        /* Allocate slice entries pointers array. */
        _FP_XGS3_ALLOC(fs->entries, mem_sz, "array of entry pointers.");
        if (NULL == fs->entries) {
            /* Allocation failed free all previously allocated entries. */
            _field_th_stage_entries_free(unit, stage_fc);
            return (BCM_E_MEMORY);
        }

        fs->start_tcam_idx = tcam_idx;
        tcam_idx += fs->entry_count;
        fs->free_count = fs->entry_count;
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *    _field_th_entries_init
 *
 * Purpose:
 *    Allocate the memory for entries in all the slices in all instances for a
 *    stage.
 *
 * Parameters:
 *    unit - (IN) BCM device number.
 *    stage_fc - (IN/OUT) Stage field control.
 *
 * Returns:
 *    BCM_E_PARAM   - NULL stage control pointer
 *    BCM_E_MEMORY  - Allocation failure
 *    BCM_E_NONE    - Success
 */
STATIC int
_field_th_entries_init(int unit, _field_stage_t *stage_fc)
{
    int inst; /* Instance iterator. */

    /* Input parameters check. */
    if (NULL == stage_fc) {
        return (BCM_E_PARAM);
    }

    /* Iterate over all instances. */
    for (inst = 0; inst < stage_fc->num_instances; inst++) {
        BCM_IF_ERROR_RETURN(_field_th_instance_entries_init(unit, stage_fc,
            inst));
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *    _field_th_meters_deinit
 *
 * Purpose:
 *    Deinitialize merter usage bitmap for all instances in a stage.
 *
 * Parameters:
 *    unit - (IN) BCM device number.
 *    stage_fc - (IN/OUT) Stage field control.
 *
 * Returns:
 *    BCM_E_PARAM  - NULL stage control pointer.
 *    BCM_E_NONE   - Success.
 */
STATIC int
_field_th_meters_deinit(int unit, _field_stage_t *stage_fc)
{
    int inst; /* Instance iterator. */

    /* Input parameters check. */
    if (NULL == stage_fc) {
        return (BCM_E_PARAM);
    }

    /* Free instance meter resources. */
    for (inst = 0; inst < stage_fc->num_instances; inst++) {
        _field_th_instance_meters_deinit(unit, stage_fc, inst);
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *   _field_th_meter_pools_init
 *
 * Purpose:
 *    Initialize field meter pools usage bitmap for all instances in a stage.
 *
 * Parameters:
 *     unit - (IN) BCM device number
 *     stage_fc - (IN/OUT) Stage field control structure.
 *
 * Returns:
 *    BCM_E_PARAM   - NULL stage control pointer.
 *    BCM_E_MEMORY  - Allocation failure.
 *    BCM_E_NONE    - Success.
 */
STATIC int
_field_th_meter_pools_init(int unit, _field_stage_t *stage_fc)
{
    int inst; /* Instance iterator. */
    int pool_idx; /* Meter pools iterator. */
    int size; /* Number of meters in a pool. */
    _field_meter_pool_t *mp; /* Meter pool pointer. */

    /* Input parameters check. */
    if (NULL == stage_fc) {
        return (BCM_E_PARAM);
    }

    /* Initialize number of meter pools. */
    stage_fc->num_meter_pools = _IFP_TH_NUM_METER_POOLS;

    /* Initialize number of meter per pool.*/
    size = SHR_BITALLOCSIZE(_IFP_TH_NUM_METER_PAIRS_PER_POOL << 1);

    /* Initialize meter pools in all instances. */
    for (inst = 0; inst < stage_fc->num_instances; inst++) {
        for (pool_idx = 0; pool_idx < stage_fc->num_meter_pools; pool_idx++) {
            /* Allocate Pipe meter pool memory. */
            stage_fc->meter_pool[inst][pool_idx] =
                sal_alloc(sizeof(_field_meter_pool_t), "fp_meter_pool");
            if (NULL == stage_fc->meter_pool[inst][pool_idx]) {
                _field_th_meters_deinit(unit, stage_fc);
                return (BCM_E_MEMORY);
            }

            /* Initialize stage instance meter pool info. */
            mp = stage_fc->meter_pool[inst][pool_idx];
            mp->slice_id = -1;
            mp->level = -1;
            mp->pool_size = (_IFP_TH_NUM_METER_PAIRS_PER_POOL * 2);
            mp->size = (_IFP_TH_NUM_METER_PAIRS_PER_POOL * 2);
            mp->free_meters = (_IFP_TH_NUM_METER_PAIRS_PER_POOL * 2);
            mp->num_meter_pairs = (_IFP_TH_NUM_METER_PAIRS_PER_POOL);

            /* Allocate instance Per-Pool meter usage bitmap memory. */
            mp->meter_bmp.w = sal_alloc(size, "fp_meters_map");
            if (NULL == mp->meter_bmp.w) {
                _field_th_meters_deinit(unit, stage_fc);
                return (BCM_E_MEMORY);
            }
            /* Clear pool meter usage bitmap. */
            sal_memset(mp->meter_bmp.w, 0, size);
        }
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *    _field_th_slice_meters_init
 *
 * Purpose:
 *    Initialize field slice meters usage bitmap for all instances in a stage.
 *
 * Parameters:
 *    unit - (IN) BCM device number
 *    stage_fc - (IN/OUT) Stage field control structure.
 *
 * Returns:
 *    BCM_E_PARAM   - NULL stage control pointer.
 *    BCM_E_MEMORY  - Allocation failure.
 *    BCM_E_NONE    - Success.
 */
STATIC int
_field_th_slice_meters_init(int unit, _field_stage_t *stage_fc)
{
    _field_slice_t *fs; /* Slice config structure. */
    int inst; /* Instance iterator. */
    int size; /* Allocation size. */
    int slice; /* Slice iterator. */

    /* Input parameters check. */
    if (NULL == stage_fc) {
        return (BCM_E_PARAM);
    }

    /* Iterate over all instances and slices and initialize meter information.*/
    for (inst = 0; inst < stage_fc->num_instances; inst++) {
        for (slice = 0; slice <stage_fc->tcam_slices; slice++) {

            /* Get the slice pointer. */
            fs = stage_fc->slices[inst] + slice;

            size = SHR_BITALLOCSIZE(_BCM_FIELD_SLICE_SIZE(stage_fc, inst, 0));
            /* Allocate meter usage bitmap. */
            fs->meter_bmp.w = sal_alloc(size, "fp_meters_map");
            if (NULL == fs->meter_bmp.w) {
                _field_th_meters_deinit(unit, stage_fc);
                return (BCM_E_MEMORY);
            }

            /* Set slice meter count and clear usage bitmap. */
            fs->meters_count = _BCM_FIELD_SLICE_SIZE(stage_fc, inst, 0);
            sal_memset(fs->meter_bmp.w, 0, size);
        }
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *   _field_th_meters_init
 *
 * Purpose:
 *    Initialize field meter usage bitmap for the sage.
 *
 * Parameters:
 *    unit - (IN) BCM device number
 *    stage_fc - (IN/OUT) Stage field control structure.
 *
 * Returns:
 *    BCM_E_PARAM   - NULL stage control pointer.
 *    BCM_E_MEMORY  - Allocation failure.
 *    BCM_E_NONE    - Success.
 */
STATIC int
_field_th_meters_init(int unit, _field_stage_t *stage_fc)
{
    /* Input parameters check. */
    if (NULL == stage_fc) {
        return (BCM_E_PARAM);
    }

    /* Lookup stage has no dedicated meter support. */
    if (_BCM_FIELD_STAGE_LOOKUP == stage_fc->stage_id) {
        return (BCM_E_NONE);
    }

    /* Initialize Global meter pools information for IFP. */
    if (stage_fc->flags & _FP_STAGE_GLOBAL_METER_POOLS) {
        return (_field_th_meter_pools_init(unit, stage_fc));
    }

    /* Initialize slice meters for EFP. */
    return (_field_th_slice_meters_init(unit, stage_fc));
}

/*
 * Function:
 *   _field_th_stage_data_ctrl_init
 *
 * Purpose:
 *    Allocate and initialize data qualifiers control structure.
 *
 * Parameters:
 *    unit - (IN) BCM device number.
 *    stage_fc - (IN/OUT) Stage field control structure.
 *
 * Returns:
 *    BCM_E_PARAM   - NULL stage control pointer.
 *    BCM_E_MEMORY  - Allocation failure.
 *    BCM_E_NONE    - Success.
 */
STATIC int
_field_th_stage_data_ctrl_init(int unit, _field_stage_t *stage_fc)
{
    int mem_sz; /* Memory size. */
    _field_data_control_t *data_ctrl = NULL;

    /* Input prameters check. */
    if (NULL == stage_fc) {
        return (BCM_E_PARAM);
    }

    /* Allocate data control structure. */
    mem_sz = sizeof(_field_data_control_t);
    _FP_XGS3_ALLOC(data_ctrl, mem_sz, "Data qualification control");
    if (NULL == data_ctrl) {
        return (BCM_E_MEMORY);
    }

    /* Allocate data TCAM entry control structure. */
    mem_sz = sizeof(_field_data_tcam_entry_t) *
                soc_mem_index_count(unit, FP_UDF_TCAMm);
    _FP_XGS3_ALLOC(data_ctrl->tcam_entry_arr, mem_sz, "UDF tcam entry array");
    if (NULL == data_ctrl->tcam_entry_arr) {
        sal_free(data_ctrl);
        return (BCM_E_MEMORY);
    }

    /* Initialize UDF offset geometry. */
    data_ctrl->elem_size = 2;
    data_ctrl->num_elems = 8;

    /* Initialize stage data control structure. */
    stage_fc->data_ctrl = data_ctrl;
    return (BCM_E_NONE);
}

/*
 * Function:
 *    _field_th_instance_slice_counters_alloc
 *
 * Purpose:
 *    Allocate counters usage bitmap for a slice in an instance.
 *
 * Parameters:
 *     unit - (IN) BCM device number.
 *     fs - (IN/OUT) Slice control structure.
 *
 * Returns:
 *    BCM_E_PARAM   - NULL slice structure pointer.
 *    BCM_E_MEMORY  - Allocation failure.
 *    BCM_E_NONE    - Success.
 */
STATIC int
_field_th_instance_slice_counters_alloc(int unit, _field_slice_t *fs)
{
    int size; /* Allocation size. */

    /* Input parameters check. */
    if (NULL == fs) {
        return (BCM_E_PARAM);
    }

    /* Determine the counter bit array size. */
    size = SHR_BITALLOCSIZE(fs->counters_count);

    /* Allocate counter usage map memory. */
    fs->counter_bmp.w = sal_alloc(size, "fp_counter_map");
    if (NULL == fs->counter_bmp.w) {
        return (BCM_E_MEMORY);
    }
    sal_memset(fs->counter_bmp.w, 0, size);
    return (BCM_E_NONE);
}

/*
 * Function:
 *    _field_th_counters_deinit
 *
 * Purpose:
 *    Deinitialize slice counter usage bitmap for all instances in a stage.
 *
 * Parameters:
 *    unit - (IN) BCM device number
 *    stage_fc - (IN/OUT) Stage field control structure.
 *
 * Returns:
 *     BCM_E_NONE - Success
 */
STATIC int
_field_th_counters_deinit(int unit, _field_stage_t *stage_fc)
{
    int slice; /* Slice iterator. */
    int inst; /* Instance iterator. */

    /* Input parameters check. */
    if (NULL == stage_fc) {
        return (BCM_E_NONE);
    }

    /*
     * VFP and IFP use Flex counter pools, skip per-slice
     * counter bitmap allocation.
     */
    if (_BCM_FIELD_STAGE_LOOKUP == stage_fc->stage_id
        || _BCM_FIELD_STAGE_INGRESS == stage_fc->stage_id) {
        return (BCM_E_NONE);
    }

    /* Counters usage bitmap is maintained only for slice_0 in EFP. */
    for (inst = 0; inst < stage_fc->num_instances; inst++) {
        /* Free counter usage bitmap in slice_0 for each instance. */
        if (NULL != stage_fc->slices[inst][0].counter_bmp.w) {
            sal_free(stage_fc->slices[inst][0].counter_bmp.w);
        }

        /* Initialize counter usage bitmap pointer value to NULL. */
        for (slice = 0; slice < stage_fc->tcam_slices; slice++) {
            stage_fc->slices[inst][slice].counter_bmp.w = NULL;
        }
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *    _field_th_counters_init
 *
 * Purpose:
 *    Initialize slice counter usage bitmap for all slices in a stage.
 *
 * Parameters:
 *     unit - (IN) BCM device number.
 *     fc - (IN) Field control structure.
 *     stage_fc - (IN/OUT) Stage field control structure.
 *
 * Returns:
 *    BCM_E_PARAM - Null field control structure or stage control parameter.
 *    BCM_E_INTERNAL - Null stage slice control structure.
 *    BCM_E_MEMORY - Allocation failure.
 *    BCM_E_NONE - Success.
 */
STATIC int
_field_th_counters_init(int unit,
                        _field_control_t *fc,
                        _field_stage_t *stage_fc)
{
    _field_slice_t *fs; /* Slice config structure. */
    int slice; /* Slice iterator. */
    int inst; /* Instance iterator. */
    int rv; /* Operation return value. */
    int cntr_share = FALSE; /* Share counter with slice_0 */

    /* Input parameters check. */
    if (NULL == stage_fc || NULL == fc) {
        return (BCM_E_PARAM);
    }

    for (inst = 0; inst < stage_fc->num_instances; inst++) {
        for (slice = 0; slice < stage_fc->tcam_slices; slice++) {

            /* Get the slice pointer. */
            fs = stage_fc->slices[inst] + slice;

            /*
             * VFP and IFP use flex counter resources hence slice
             * counters count is zero.
             */
            if (_BCM_FIELD_STAGE_LOOKUP == stage_fc->stage_id
                || _BCM_FIELD_STAGE_INGRESS == stage_fc->stage_id) {
                fs->counters_count = 0;
                continue;
            }

            /* Initialize counters for EFP stage. */
            fs->counters_count = soc_mem_index_count(unit, EFP_COUNTER_TABLEm);

            /*
             * All EFP slices share the same global counter pool.
             * For slices > 0, counter sharing is TRUE.
             */
            if (slice) {
                cntr_share = TRUE;
            }

            /* Share counters with slice zero (Global counter pool). */
            if (cntr_share) {
                stage_fc->slices[inst][slice].counter_bmp.w =
                    stage_fc->slices[inst][0].counter_bmp.w;
            } else {
                /* Allocate new set of counters for the slice. */
                rv = _field_th_instance_slice_counters_alloc(unit, fs);
                if (BCM_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                        "FP(unit %d) Error: "
                        "_field_th_instance_slice_counters_alloc=%d\n"),
                         unit, rv));
                    _field_th_counters_deinit(unit, stage_fc);
                    return (rv);
                }
            }
        }
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *    _field_th_keygen_profiles_init
 * Purpose:
 *   Initialize keygen program profiles based on group operational mode.
 * Parameters:
 *     unit - (IN) BCM device number.
 *     stage_fc - (IN/OUT) Stage field control structure.
 *
 * Returns:
 *    BCM_E_PARAM - Null field control structure or stage control parameter.
 *    BCM_E_INTERNAL - Null stage slice control structure.
 *    BCM_E_MEMORY - Allocation failure.
 *    BCM_E_NONE - Success.
 */
STATIC int
_field_th_keygen_profiles_init(int unit, _field_stage_t *stage_fc)
{
    soc_mem_t mem[2];               /* SOC memory names.                  */
    int entry_words[2];             /* Entry size in words.               */
    int inst;                       /* Instance iterator.                 */
    int rv;                         /* Operation return status.           */
    static const soc_mem_t keygen_mem[][_FP_MAX_NUM_PIPES] = /* Profiles. */
        {
            {
                /* Keygen Profile1. */
                IFP_KEY_GEN_PROGRAM_PROFILE_PIPE0m,
                IFP_KEY_GEN_PROGRAM_PROFILE_PIPE1m,
                IFP_KEY_GEN_PROGRAM_PROFILE_PIPE2m,
                IFP_KEY_GEN_PROGRAM_PROFILE_PIPE3m
            },
            {
                /* Keygen Profile2. */
                IFP_KEY_GEN_PROGRAM_PROFILE2_PIPE0m,
                IFP_KEY_GEN_PROGRAM_PROFILE2_PIPE1m,
                IFP_KEY_GEN_PROGRAM_PROFILE2_PIPE2m,
                IFP_KEY_GEN_PROGRAM_PROFILE2_PIPE3m
            }
        };

    /* Input parameter check. */
    if (NULL == stage_fc) {
        return (BCM_E_PARAM);
    }

    switch (stage_fc->oper_mode) {
        /* Field Groups operational in Global mode. */
        case bcmFieldGroupOperModeGlobal:
            /* Initialize profile1 memory name. */
            mem[0] = IFP_KEY_GEN_PROGRAM_PROFILEm;

            /* Initialize profile2 memory name. */
            mem[1] = IFP_KEY_GEN_PROGRAM_PROFILE2m;

            /* Determine keygen profile entry size in number of words. */
            entry_words[0] = soc_mem_entry_words(unit, mem[0]);

            /* Determine post mux profile entry size in number of words. */
            entry_words[1] = soc_mem_entry_words(unit, mem[1]);

            /* Create keygen program profile table. */
            rv = soc_profile_mem_create(unit, mem, entry_words, 2,
                    &stage_fc->keygen_profile[_FP_DEF_INST].profile);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                    "FP(unit %d) Error: Keygen profile creation failed."
                    "=%d\n"), unit, rv));
                return (rv);
            }
            break;

        /* Field Groups operational in Per-Pipe mode. */
        case bcmFieldGroupOperModePipeLocal:
            /* Determine keygen profiles entry size in number of words. */
            entry_words[0] = soc_mem_entry_words(unit,
                                IFP_KEY_GEN_PROGRAM_PROFILE_PIPE0m);
            entry_words[1] = soc_mem_entry_words(unit,
                                IFP_KEY_GEN_PROGRAM_PROFILE2_PIPE0m);

            for (inst = 0; inst < stage_fc->num_instances; inst++) {
                /* Initialize keygen program profile1 memory name. */
                mem[0] = keygen_mem[0][inst];

                /* Initialize keygen program profile2 memory name. */
                mem[1] = keygen_mem[1][inst];

                /* Create keygen program profile table. */
                rv = soc_profile_mem_create(unit, mem, entry_words, 2,
                            &stage_fc->keygen_profile[inst].profile);
                if (BCM_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                        "FP(unit %d) Error: KeyGen profile creation failed."
                        "=%d\n"), unit, rv));
                    return (rv);
                }
            }
            break;
        default:
            /*
             * Invalid Group Operational mode, should never hit this condition.
             */
            return (BCM_E_INTERNAL);
    }

    return (BCM_E_NONE);
}

/*
 *  Function:
 *     _field_th_stage_core_init
 * Purpose:
 *    Initialize all data structures related to a Field Pipeline stage.
 * Parameters:
 *     unit         - (IN) BCM device number.
 *     fc           - (IN/OUT) Field control structure.
 *     stage_fc     - (IN/OUT) Stage field control structure.
 *
 * Returns:
 *    BCM_E_PARAM - Null field control structure or stage control parameter.
 *    BCM_E_INTERNAL - Invalid CAP stage detected.
 *    BCM_E_MEMORY - Allocation failure.
 *    BCM_E_NONE - Success.
 */
STATIC int
_field_th_stage_core_init(int unit,
                          _field_control_t *fc,
                          _field_stage_t *stage_fc)
{
    int rv = BCM_E_INTERNAL; /* Operation return value. */
    int entry_words; /* Redirection profile entry size. */
    soc_mem_t mem; /* SOC Memory name variable. */
    int map_id; /* Virtual Group entry iterator. */
    int idx; /* Virtual priority iterator. */

    /* Input prameters check. */
    if ((NULL == fc) || (NULL == stage_fc)) {
        return (BCM_E_PARAM);
    }

    /* Initialize stage slices information. */
    rv = _field_th_slices_init(unit, stage_fc, fc);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
            "FP(unit %d) Error: _field_th_slices_init=%d\n"), unit, rv));
        _field_th_stage_delete(unit, fc, stage_fc);
        return (rv);
    }

    /* Initialize stage slices entries information. */
    rv = _field_th_entries_init(unit, stage_fc);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
            "FP(unit %d) Error: _field_th_entries_init=%d\n"), unit, rv));
        _field_th_stage_delete(unit, fc, stage_fc);
        return (rv);
    }

    /* Initialize Meters. */
    rv = _field_th_meters_init(unit, stage_fc);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
            "FP(unit %d) Error: _field_th_meters_init=%d\n"), unit, rv));
        _field_th_stage_delete(unit, fc, stage_fc);
        return (rv);
    }

    /* Initialize counters usage bitmap. */
    rv = _field_th_counters_init(unit, fc, stage_fc);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
            "FP(unit %d) Error: _field_th_counters_init=%d\n"), unit, rv));
        _field_th_stage_delete(unit, fc, stage_fc);
        return (rv);
    }

    /* Action profile; will be setup in chip-specific init */
    soc_profile_mem_t_init(&stage_fc->ext_act_profile);

    /* Initialize global Redirect action memory profile. */
    soc_profile_mem_t_init(&stage_fc->redirect_profile);

    /* Redirection action profile table initialization. */
    if (_BCM_FIELD_STAGE_INGRESS == stage_fc->stage_id) {

        /* Assign redirection profile memory name "mem" variable. */
        mem = IFP_REDIRECTION_PROFILEm;

        /* Determine redirection profile entry size in number of words. */
        entry_words = sizeof(ifp_redirection_profile_entry_t) / sizeof(uint32);

        /* Create the redirection profile table. */
        rv = soc_profile_mem_create(unit, &mem, &entry_words, 1,
                &stage_fc->redirect_profile);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                "FP(unit %d) Error: Redirection profile creation failed.=%d\n"),
                 unit, rv));
            _field_th_stage_delete(unit, fc, stage_fc);
            return (rv);
        }

        /* Initialize regular keygen and post mux keygen program profiles. */
        rv = _field_th_keygen_profiles_init(unit, stage_fc);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                "FP(unit %d) Error: _field_th_keygen_profiles_init=%d\n"),
                 unit, rv));
            _field_th_stage_delete(unit, fc, stage_fc);
            return (rv);
        }

        /* Initialize range checker seed ID value. */
        stage_fc->range_id = 1;

        /* Initialize stage data control information. */
        rv = _field_th_stage_data_ctrl_init(unit, stage_fc);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                "FP(unit %d) Error: _field_th_stage_data_ctrl_init=%d\n"),
                 unit, rv));
            _field_th_stage_delete(unit, fc, stage_fc);
            return (rv);
        }
    }

    switch (stage_fc->stage_id) {

        case _BCM_FIELD_STAGE_LOOKUP:
        /* passthru */
        /* coverity[MISSING_BREAK: FALSE] */
        case _BCM_FIELD_STAGE_EGRESS:
            /* Initialize stage select codes table. */
            rv = _bcm_field_stage_fpf_init(unit, stage_fc);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                    "FP(unit %d) Error: _bcm_field_stage_fpf_init=%d\n"),
                     unit, rv));
                _field_th_stage_delete(unit, fc, stage_fc);
                return (rv);
            };

            /*
             * Initialize the FP_SLICE_MAP for VFP and EFP.
             *     Physical_slice and virtual_group are H/W initial values
             */
            for (map_id = 0; map_id < _FP_VMAP_CNT; map_id++) {
                for (idx = 0; idx < _FP_VMAP_SIZE; idx++) {
                    stage_fc->vmap[map_id][idx].valid = FALSE;
                    stage_fc->vmap[map_id][idx].vmap_key = idx;
                    stage_fc->vmap[map_id][idx].virtual_group = idx;
                    stage_fc->vmap[map_id][idx].flags = 0;
                }
            }
            break;

        case _BCM_FIELD_STAGE_INGRESS:

            /* Initialize stage legacy select codes table. */
            rv = _bcm_field_stage_fpf_init(unit, stage_fc);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                    "FP(unit %d) Error: _bcm_field_stage_fpf_init=%d\n"),
                     unit, rv));
                _field_th_stage_delete(unit, fc, stage_fc);
                return (rv);
            };

            /* Initialize stage Preselector information. */
            rv = _bcm_field_th_stage_preselector_init(unit, fc, stage_fc);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                    "FP(unit %d) Error: _bcm_field_th_stage_preselector_init=%d"
                    "\n"), unit, rv));
                _field_th_stage_delete(unit, fc, stage_fc);
                return (rv);
            }

            /*
             * Initialize stage's extractor configuration for various slice
             * modes.
             */
            rv = _field_th_stage_extractors_init(unit, stage_fc);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                    "FP(unit %d) Error: _field_th_stage_extractors_init=%d\n"),
                    unit, rv));
                _field_th_stage_delete(unit, fc, stage_fc);
                return (rv);
            }

            /* Initialize IFP qualifers to HW input bus fields & offsets. */
            rv = _field_th_stage_quals_ibus_map_init(unit, stage_fc);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                    "FP(unit %d) Error: _field_th_stage_extractors_init=%d\n"),
                    unit, rv));
                _field_th_stage_delete(unit, fc, stage_fc);
                return (rv);
            }
            break;
        default:
            /* Should never hit this condition. */
            return (BCM_E_INTERNAL);
    }

    FP_LOCK(fc);

    /* Add stage to Field Control structure. */
    stage_fc->next = fc->stages;
    fc->stages = stage_fc;

    FP_UNLOCK(fc);

    return (rv);
}

/*
 * Function:
 *    _field_th_stage_add
 *
 * Purpose:
 *    Initialize all data structures related to a field stage.
 *
 * Parameters:
 *     unit - (IN) BCM device number.
 *     fc - (IN/OUT) Field control structure.
 *     stage_id - (IN) Field processor stage ID.
 *
 * Returns:
 *    BCM_E_PARAM - Null field control structure.
 *    BCM_E_INTERNAL - Null stage slice control structure.
 *    BCM_E_MEMORY - Allocation failure.
 *    BCM_E_NONE - Success.
 */
STATIC int
_field_th_stage_add(int unit,
                    _field_control_t *fc,
                    _field_stage_id_t stage_id,
                    bcm_field_group_oper_mode_t mode)
{
    int rv = BCM_E_INTERNAL; /* Operation return value. */
    _field_stage_t *stage_fc; /* Stage information. */

    /* Input parameter check. */
    if (NULL == fc) {
        return (BCM_E_PARAM);
    }

    /* Allocate field stage structure instance memory. */
    stage_fc = sal_alloc(sizeof(_field_stage_t), "FP stage info.");
    if (NULL == stage_fc) {
        LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
            "FP(unit %d) Error: Allocation failure for stage info\n"), unit));
        return (BCM_E_MEMORY);
    }

    /* Clear contents of allocated buffer. */
    sal_memset(stage_fc, 0, sizeof(_field_stage_t));

    /* Initialize FP stage. */
    stage_fc->stage_id = stage_id;

    /* By default all CAP stages come up in Global mode (Legacy TD2). */
    if (bcmFieldGroupOperModeCount == mode) {
        if (stage_id == _BCM_FIELD_STAGE_INGRESS) {
           /* Initialize the SER during init for the given default mode. */
           BCM_IF_ERROR_RETURN
               (_field_ser_oper_mode_init(unit, stage_id,
                                          bcmFieldGroupOperModeGlobal));
        }
        stage_fc->oper_mode = bcmFieldGroupOperModeGlobal;
    } else {
        /* Default Group oper mode modified using Group oper mode API. */
        stage_fc->oper_mode = mode;
    }

    /*
     * Initialize number of FP data structure instances to be supported
     * per-stage.
     */
    stage_fc->num_pipes = _FP_MAX_NUM_PIPES;

    if (bcmFieldGroupOperModePipeLocal == stage_fc->oper_mode) {
        /*
         * Initialize number of FP instances to number of pipes supported in
         * hardware.
         */
        stage_fc->num_instances = stage_fc->num_pipes;
    } else {
        /* Initialize to legacy TD2 mode - Global mode (single instance). */
        stage_fc->num_instances = 1;
    }

    switch (stage_fc->stage_id) {
        case _BCM_FIELD_STAGE_LOOKUP:
            /* Flags */
            stage_fc->flags |= (_FP_STAGE_SLICE_ENABLE
                                | _FP_STAGE_AUTO_EXPANSION
                                | _FP_STAGE_GLOBAL_COUNTERS);
            /* Slice geometry */
            stage_fc->tcam_sz     = soc_mem_index_count(unit, VFP_TCAMm);
            stage_fc->tcam_slices = 4;
            break;

        case _BCM_FIELD_STAGE_INGRESS:
            /* Flags */
            stage_fc->flags |= (_FP_STAGE_SLICE_ENABLE
                                | _FP_STAGE_AUTO_EXPANSION
                                | _FP_STAGE_GLOBAL_METER_POOLS
                                | _FP_STAGE_GLOBAL_COUNTERS
                                | _FP_STAGE_MULTI_PIPE_COUNTERS);

            /*
             * Initialize Slice geometry.
             *     Number of entries in TCAM remains the same for both duplicate
             *     and unique views. Use duplicate view to calculate TCAM size.
             *     Initialize to maximum entries supported per-slice (80bit
             *     mode).
             */
            stage_fc->tcam_sz = soc_mem_index_count(unit, IFP_TCAMm);

            /* Number of hardware slices is equal to maximum number of LT parts. */
            stage_fc->tcam_slices = _FP_MAX_LT_PARTS;

            /* Initialize number of logical tables supported by device. */
            stage_fc->num_logical_tables = _FP_MAX_NUM_LT;

            /*
             * Initialize number of valid entries in Logical Table selection
             * TCAM.
             */
            stage_fc->lt_tcam_sz = stage_fc->tcam_slices
                                    * stage_fc->num_logical_tables;
            /* Initialize number of extractor levels */
            stage_fc->num_ext_levels = 4;
            break;

        case _BCM_FIELD_STAGE_EGRESS:
            /* Flags */
            stage_fc->flags |= (_FP_STAGE_SLICE_ENABLE
                                | _FP_STAGE_GLOBAL_COUNTERS
                                | _FP_STAGE_SEPARATE_PACKET_BYTE_COUNTERS
                                | _FP_STAGE_AUTO_EXPANSION
                                | _FP_STAGE_MULTI_PIPE_COUNTERS);
            /* Slice geometry */
            stage_fc->tcam_sz = soc_mem_index_count(unit, EFP_TCAMm);
            stage_fc->tcam_slices = 4;
            break;
        case _BCM_FIELD_STAGE_CLASS:
            rv = _bcm_field_th_class_stage_add(unit, fc, stage_fc);
            if (BCM_FAILURE(rv)) {
                sal_free(stage_fc);
            }
            return (rv);
        default:
            if (NULL != stage_fc) {
                sal_free(stage_fc);
            }
            return (rv);
    }

    /*
     * Initialize Field Stage attributes. On error condition, memory location
     * pointed by "stage_fc" pointer is freed inside this routine in error
     * return section. So, no need to free again in the error return section.
     * No need to do stage core init for stage class as it is a pseudo stage.
     */
    rv = _field_th_stage_core_init(unit, fc, stage_fc);
    if (BCM_FAILURE(rv)) {
        if (NULL != stage_fc) {
            sal_free(stage_fc);
        }
        LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
            "FP(unit %d) Error: _field_th_stage_core_init=%d\n"), unit, rv));
    }

    return (rv);
}

/*
 * Function:
 *    _bcm_field_th_stages_delete
 *
 * Purpose:
 *    Deinitialize a field stage.
 *
 * Parameters:
 *    unit - (IN) BCM device number.
 *    fc - (IN/OUT)Field control info for device.
 *
 * Returns:
 *    BCM_E_PARAM - Null field control structure.
 *    BCM_E_NONE - Success.
 */
int
_bcm_field_th_stages_delete(int unit, _field_control_t *fc)
{
    _field_stage_t *stage_fc; /* Stage field control structure. */

    /* Input parameters check. */
    if (NULL == fc) {
        return (BCM_E_PARAM);
    }

    if (NULL != fc->fc_lock) {
        FP_LOCK(fc);
    }

    /* Free stages slices structures. */
    while (NULL != fc->stages) {
        stage_fc = fc->stages;
        /* Free stage resources and stage itself. */
        _field_th_stage_delete(unit, fc, stage_fc);
    }

    if (NULL != fc->fc_lock) {
        FP_UNLOCK(fc);
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *    _bcm_field_th_stages_add
 *
 * Purpose:
 *    Initialize all Field Processor stages of the device.
 *
 * Parameters:
 *    unit - (IN) BCM device number
 *    fc - (IN/OUT) Field control structure.
 *
 * Returns:
 *    BCM_E_PARAM - Null field control structure.
 *    BCM_E_NONE - Success.
 */
int
_bcm_field_th_stages_add(int unit, _field_control_t *fc)
{
    int rv; /* Operation return value. */

    /* Input parameters check. */
    if (NULL == fc) {
        return (BCM_E_PARAM);
    }

    /* Initialize CAP stages. */
    if (soc_feature(unit, soc_feature_field_multi_stage)
        && (SOC_SWITCH_BYPASS_MODE_L3_ONLY != SOC_SWITCH_BYPASS_MODE(unit))) {
        /* Add VFP Stage. */
        rv = _field_th_stage_add(unit, fc, _BCM_FIELD_STAGE_LOOKUP,
                bcmFieldGroupOperModeCount);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
            "FP(unit %d) Error: _field_th_stage_add(Lookup)=%d\n"), unit, rv));
            _bcm_field_th_stages_delete(unit, fc);
            return (rv);
        }

        /* Add EFP Stage. */
        rv = _field_th_stage_add(unit, fc, _BCM_FIELD_STAGE_EGRESS,
                bcmFieldGroupOperModeCount);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
            "FP(unit %d) Error: _field_th_stage_add(Egress)=%d\n"), unit, rv));
            _bcm_field_th_stages_delete(unit, fc);
            return (rv);
        }
    }

    /* Add IFP Stage. */
    rv = _field_th_stage_add(unit, fc, _BCM_FIELD_STAGE_INGRESS,
                                    bcmFieldGroupOperModeCount);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
            "FP(unit %d) Error: _field_th_stage_add(Ingress)=%d\n"), unit, rv));
        /*
         * Delete all previous stages initialized on this device in case of
         * error.
         */
        _bcm_field_th_stages_delete(unit, fc);
        return (rv);
    }

    /* Add Class Stage. */
    rv = _field_th_stage_add(unit, fc, _BCM_FIELD_STAGE_CLASS,
                                        bcmFieldGroupOperModeCount);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                        "FP(unit %d) Error: _field_th_stage_add(Class)=%d\n"), unit, rv));
        /*
         * Delete all previous stages initialized on this device in case of
         * error.
         */
        _bcm_field_th_stages_delete(unit, fc);
        return (rv);
    }

    /* Initialize device LT entry allocation ID variable. */
    fc->last_allocated_lt_eid = 0;

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_field_action_insert
 * Purpose:
 *     Add action to stage actions list.
 * Parameters:
 *     unit     - (IN) BCM device number.
 *     stage_fc - (IN) Stage field control strucutre.
 *     action   - (IN) Field Action.
 *     ptr      - (IN) FP action info.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_action_insert(int unit, _field_stage_t *stage_fc,
                         int action, _bcm_field_action_offset_t *ptr)
{
    int flag_first;                                /* action allocation flag. */
    int alloc_size;                                /* Memory allocation size. */
    _bcm_field_action_conf_t *f_action;            /* Action configuration.   */
    _bcm_field_action_offset_t *offset_info;       /* Offset Information.     */
    _bcm_field_action_offset_t *new_offset_info;   /* New Offset Information. */

    /* Input parameters check. */
    if ((NULL == ptr) || (NULL == stage_fc) ||
        (action < 0) || (action > bcmFieldActionCount)) {
        return BCM_E_PARAM;
    }

    if ((_BCM_FIELD_STAGE_INGRESS == stage_fc->stage_id) &&
        (ptr->flags & _BCM_FIELD_ACTION_NO_IFP_SUPPORT)) {
        return BCM_E_NONE;
    }
    if ((_BCM_FIELD_STAGE_EXACTMATCH == stage_fc->stage_id) &&
        (ptr->flags & _BCM_FIELD_ACTION_NO_EM_SUPPORT)) {
         return BCM_E_NONE;
    }

    f_action = NULL;
    offset_info = NULL;
    new_offset_info = NULL;

    f_action = stage_fc->f_action_arr[action];
    /* Allocate Action descriptor. */
    flag_first = 0;
    if (NULL == f_action) {
        alloc_size = sizeof(_bcm_field_action_conf_t);
        _FP_XGS3_ALLOC(f_action, alloc_size, "FP Action info");
        if (NULL == f_action) {
            return (BCM_E_MEMORY);
        }
        f_action->action = action;
        flag_first = 1;
    } else {
        offset_info = f_action->offset;
        while (NULL != offset_info) {
            if (offset_info->flags & ptr->flags) {
                return BCM_E_EXISTS;
            }
            offset_info = offset_info->next;
        }
    }

    alloc_size = sizeof(_bcm_field_action_offset_t);
    _FP_XGS3_ALLOC(new_offset_info, alloc_size, "FP Action offset info");
    if (NULL == new_offset_info) {
        if (flag_first) {
            if (NULL != f_action) {
                sal_free(f_action);
            }
        }
        return (BCM_E_MEMORY);
    }

    /* Copy new configuration to configuration array. */
    new_offset_info->flags  |= ptr->flags;
    new_offset_info->offset[0] = ptr->offset[0];
    new_offset_info->width[0]  = ptr->width[0];
    new_offset_info->value[0]  = ptr->value[0];
    new_offset_info->offset[1] = ptr->offset[1];
    new_offset_info->width[1]  = ptr->width[1];
    new_offset_info->value[1]  = ptr->value[1];
    new_offset_info->offset[2] = ptr->offset[2];
    new_offset_info->width[2]  = ptr->width[2];
    new_offset_info->value[2]  = ptr->value[2];
    new_offset_info->offset[3] = ptr->offset[3];
    new_offset_info->width[3]  = ptr->width[3];
    new_offset_info->value[3]  = ptr->value[3];
    new_offset_info->next    = f_action->offset;
    f_action->offset = new_offset_info;

    /* Install Action configuration into stage actions array. */
    stage_fc->f_action_arr[action] = f_action;

    return (BCM_E_NONE);
}
/*
 * Function:
 *     _field_th_ingress_actions_init
 * Purpose:
 *     Initialize device stage lookup qaualifiers
 *     select codes & offsets
 * Parameters:
 *     unit       - (IN) BCM device number.
 *     stage_fc   - (IN) Field Processor stage control structure.
 * Returns:
 *     BCM_E_NONE
 */
int
_field_th_common_actions_init(int unit, _field_stage_t *stage_fc)
{
    _FP_ACTION_DECL
    uint32 offset = 0;                  /* General variable to carry offset. */
    uint32 profile_one_offset = 0;      /* Offset of PROFILE_ONE_SET. */
    uint32 profile_two_offset = 0;      /* Offset of PROFILE_TWO_SET. */
    uint32 redirect_offset = 0;         /* Offset of REDIRECT_SET. */
    uint32 l3swl2change_offset = 0;     /* Offset of L3SW_CHANGE_L2_SET. */
    uint32 nat_offset = 0;              /* Offset of NAT_SET. */
    uint32 mirror_offset = 0;           /* Offset of MIRROR_SET. */
    uint32 lb_offset = 0;               /* Offset of LB_CONTROL_SET. */
    uint32 nat_override_offset = 0;     /* Offset of NAT_OVERRIDE_SET. */
    uint32 copytocpu_offset = 0;        /* Offset of COPY_TO_CPU_SET. */
    uint32 cutthrough_offset = 0;       /* Offset of CUT_THRU_OVERRIDE_SET. */
    uint32 urpf_offset = 0;             /* Offset of URPF_OVERRIDE_SET. */
    uint32 ttl_offset = 0;              /* Offset of TTL_SET. */
    uint32 cpucos_offset = 0;           /* Offset of CHANGE_CPU_COS_SET. */
    uint32 drop_offset = 0;             /* Offset of DROP_SET. */
    uint32 mirror_override_offset = 0;  /* Offset of MIRROR_OVERRIDE_SET. */
    uint32 sflow_offset = 0;            /* Offset of SFLOW_SET. */
    uint32 debug_offset = 0;            /* Offset of INSTRUMENTATION_TRIGGERS_ENABLE. */

    /* Input parameters check. */
    if (NULL == stage_fc) {
        return (BCM_E_PARAM);
    }
    if (stage_fc->stage_id == _BCM_FIELD_STAGE_INGRESS) {
        profile_one_offset     = 0;  /* Start of the IFP_POLICY_TABLE */
        mirror_offset          = 56; /* IFP_PROFILE_SET_1(56b) */
        lb_offset              = 68; /* PROFILE_SET_1(56b) + MIRROR_SET(12b) */
        nat_override_offset    = 71; /* PROFILE_SET_1(56b) + MIRROR_SET(12b) +
                                      *  LB_CONTROLS_SET(3b)
                                      */
        copytocpu_offset       = 72; /* PROFILE_SET_1(56b) + MIRROR_SET(12b) +
                                      *  LB_CONTROLS_SET(3b) + NAT_OVERRIDE_SET(1b)
                                      */
        cutthrough_offset      = 89; /* PROFILE_SET_1(56b) + MIRROR_SET(12b) +
                                      * LB_CONTROLS_SET(3b) + NAT_OVERRIDE_SET(1b) +
                                      *  COPY_TO_CPU_SET(17b)
                                      */
        urpf_offset            = 90; /* PROFILE_SET_1(56b) + MIRROR_SET(12b) +
                                      * LB_CONTROLS_SET(3b) + NAT_OVERRIDE_SET(1b) +
                                      * COPY_TO_CPU_SET(17b) +
                                      * CUT_THRU_OVERRIDE_SET(1b)
                                      */
        ttl_offset             = 91; /* PROFILE_SET_1(56b) + MIRROR_SET(12b) +
                                      * LB_CONTROLS_SET(3b) + NAT_OVERRIDE_SET(1b) +
                                      * COPY_TO_CPU_SET(17b) +
                                      * CUT_THRU_OVERRIDE_SET(1b) +
                                      * URPF_OVERRIDE_SET(1b)
                                      */
        profile_two_offset    = 101; /* PROFILE_SET_1(56b) + MIRROR_SET(12b) +
                                      * LB_CONTROLS_SET(3b) + NAT_OVERRIDE_SET(1b) +
                                      * COPY_TO_CPU_SET(17b) +
                                      * CUT_THRU_OVERRIDE_SET(1b) +
                                      * URPF_OVERRIDE_SET(1b) + IFP_TTL_SET(1b) +
                                      * RESERVED_0(1b) + ECCP_0(8b)
                                      */
        l3swl2change_offset   = 153; /* PROFILE_SET_1(56b) + MIRROR_SET(12b) +
                                      * LB_CONTROLS_SET(3b) + NAT_OVERRIDE_SET(1b) +
                                      * COPY_TO_CPU_SET(17b) +
                                      * CUT_THRU_OVERRIDE_SET(1b) +
                                      * URPF_OVERRIDE_SET(1b) + IFP_TTL_SET(1b) +
                                      * RESERVED_0(1b) + ECCP_0(8b) +
                                      * PROFILE_SET_2(52b)
                                      */
        cpucos_offset         = 176; /* PROFILE_SET_1(56b) + MIRROR_SET(12b) +
                                      * LB_CONTROLS_SET(3b) + NAT_OVERRIDE_SET(1b) +
                                      * COPY_TO_CPU_SET(17b) +
                                      * CUT_THRU_OVERRIDE_SET(1b) +
                                      * URPF_OVERRIDE_SET(1b) + IFP_TTL_SET(1b) +
                                      * RESERVED_0(1b) + ECCP_0(8b) +
                                      * PROFILE_SET_2(52b) + L3SW_CHANGE_L2_SET(33b)
                                      */
        drop_offset           = 184; /* PROFILE_SET_1(56b) + MIRROR_SET(12b) +
                                      * LB_CONTROLS_SET(3b) + NAT_OVERRIDE_SET(1b) +
                                      * COPY_TO_CPU_SET(17b) +
                                      * CUT_THRU_OVERRIDE_SET(1b) +
                                      * URPF_OVERRIDE_SET(1b) + IFP_TTL_SET(1b) +
                                      * RESERVED_0(1b) + ECCP_0(8b) +
                                      * PROFILE_SET_2(52b) + L3SW_CHANGE_L2_SET(33b)
                                      * + CHANGE_CPU_COS_SET(8b)
                                      */
        mirror_override_offset = 190; /* PROFILE_SET_1(56b) + MIRROR_SET(12b) +
                                       * LB_CONTROLS_SET(3b) +
                                       * NAT_OVERRIDE_SET(1b) +
                                       * COPY_TO_CPU_SET(17b) +
                                       * CUT_THRU_OVERRIDE_SET(1b) +
                                       * URPF_OVERRIDE_SET(1b) + IFP_TTL_SET(1b) +
                                       * RESERVED_0(1b) + ECCP_0(8b) +
                                       * PROFILE_SET_2(52b) +
                                       * L3SW_CHANGE_L2_SET(33b) +
                                       * CHANGE_CPU_COS_SET(8b) + DROP_SET(6b) +
                                       */
        sflow_offset           = 192; /* PROFILE_SET_1(56b) + MIRROR_SET(12b) +
                                       * LB_CONTROLS_SET(3b) +
                                       * NAT_OVERRIDE_SET(1b) +
                                       * COPY_TO_CPU_SET(17b) +
                                       * CUT_THRU_OVERRIDE_SET(1b) +
                                       * URPF_OVERRIDE_SET(1b) + IFP_TTL_SET(1b) +
                                       * RESERVED_0(1b) + ECCP_0(8b) +
                                       * PROFILE_SET_2(52b) +
                                       * L3SW_CHANGE_L2_SET(33b) +
                                       * CHANGE_CPU_COS_SET(8b) + DROP_SET(6b) +
                                       * MIRROR_OVERRIDE_SET(1b) +
                                       * GREEN_TO_PID_SET(1b)
                                       */
        debug_offset           = 193; /* PROFILE_SET_1(56b) + MIRROR_SET(12b) +
                                       * LB_CONTROLS_SET(3b) +
                                       * NAT_OVERRIDE_SET(1b) +
                                       * COPY_TO_CPU_SET(17b) +
                                       * CUT_THRU_OVERRIDE_SET(1b) +
                                       * URPF_OVERRIDE_SET(1b) + IFP_TTL_SET(1b) +
                                       * RESERVED_0(1b) + ECCP_0(8b) +
                                       * PROFILE_SET_2(52b) +
                                       * L3SW_CHANGE_L2_SET(33b) +
                                       * CHANGE_CPU_COS_SET(8b) + DROP_SET(6b) +
                                       * MIRROR_OVERRIDE_SET(1b) +
                                       * GREEN_TO_PID_SET(1b) + SFLOW_SET(1b)
                                       */
        redirect_offset        = 202; /* PROFILE_SET_1(56b) + MIRROR_SET(12b) +
                                       * LB_CONTROLS_SET(3b) +
                                       * NAT_OVERRIDE_SET(1b) +
                                       * COPY_TO_CPU_SET(17b) +
                                       * CUT_THRU_OVERRIDE_SET(1b) +
                                       * URPF_OVERRIDE_SET(1b) + IFP_TTL_SET(1b) +
                                       * RESERVED_0(1b) + ECCP_0(8b) +
                                       * PROFILE_SET_2(52b) +
                                       * L3SW_CHANGE_L2_SET(33b) +
                                       * CHANGE_CPU_COS_SET(8b) + DROP_SET(6b) +
                                       * MIRROR_OVERRIDE_SET(1b) +
                                       * GREEN_TO_PID_SET(1b) + SFLOW_SET(1b) +
                                       * INSTRUMENTATION_TRIGGERS_ENABLE(1b) +
                                       * ECCP_1(8b)
                                       */
        nat_offset             = 283; /* PROFILE_SET_1(56b) + MIRROR_SET(12b) +
                                       * LB_CONTROLS_SET(3b) +
                                       * NAT_OVERRIDE_SET(1b) +
                                       * COPY_TO_CPU_SET(17b) +
                                       * CUT_THRU_OVERRIDE_SET(1b) +
                                       * URPF_OVERRIDE_SET(1b) + IFP_TTL_SET(1b) +
                                       * RESERVED_0(1b) + ECCP_0(8b) +
                                       * PROFILE_SET_2(52b) +
                                       * L3SW_CHANGE_L2_SET(33b) +
                                       * CHANGE_CPU_COS_SET(8b) + DROP_SET(6b) +
                                       * MIRROR_OVERRIDE_SET(1b) +
                                       * GREEN_TO_PID_SET(1b) + SFLOW_SET(1b) +
                                       * INSTRUMENTATION_TRIGGERS_ENABLE(1b) +
                                       * ECCP_1(8b) + REDIRECT_SET(38b) +
                                       * COUNTER_SET(25b) + METER_SET(18b)
                                       */
    } else if (stage_fc->stage_id == _BCM_FIELD_STAGE_EXACTMATCH) {
        profile_two_offset=56; 
    }

    /* IFP_PROFILE_SET_1(56b) */
    offset = profile_one_offset;

    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionRpDropPrecedence,
                                                         0, offset + 0, 2, -1);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionYpDropPrecedence,
                                                         0, offset + 2, 2, -1);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionGpDropPrecedence,
                                                         0, offset + 4, 2, -1);

    _FP_ACTION_ADD_TWO(unit, stage_fc, bcmFieldActionRpCosQNew,
                                               0, offset + 6, 8, -1, 30, 4, 1);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionRpPrioIntCopy,
                                                         0, offset + 30, 4, 4);
    _FP_ACTION_ADD_TWO(unit, stage_fc, bcmFieldActionRpPrioIntNew,
                                               0, offset + 6, 8, -1, 30, 4, 5);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionRpPrioIntTos,
                                                         0, offset + 30, 4, 6);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionRpPrioIntCancel,
                                                         0, offset + 30, 4, 7);
    _FP_ACTION_ADD_TWO(unit, stage_fc, bcmFieldActionRpUcastCosQNew,
                                               0, offset + 6, 4, -1, 30, 4, 8);
    _FP_ACTION_ADD_TWO(unit, stage_fc, bcmFieldActionRpMcastCosQNew,
                                              0, offset + 10, 4, -1, 30, 4, 9);

    _FP_ACTION_ADD_TWO(unit, stage_fc, bcmFieldActionYpCosQNew,
                                              0, offset + 14, 8, -1, 34, 4, 1);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionYpPrioIntCopy,
                                                         0, offset + 34, 4, 4);
    _FP_ACTION_ADD_TWO(unit, stage_fc, bcmFieldActionYpPrioIntNew,
                                              0, offset + 14, 8, -1, 34, 4, 5);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionYpPrioIntTos,
                                                         0, offset + 34, 4, 6);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionYpPrioIntCancel,
                                                         0, offset + 34, 4, 7);
    _FP_ACTION_ADD_TWO(unit, stage_fc, bcmFieldActionYpUcastCosQNew,
                                              0, offset + 14, 4, -1, 34, 4, 8);
    _FP_ACTION_ADD_TWO(unit, stage_fc, bcmFieldActionYpMcastCosQNew,
                                              0, offset + 18, 4, -1, 34, 4, 9);

    _FP_ACTION_ADD_TWO(unit, stage_fc, bcmFieldActionGpCosQNew,
                                              0, offset + 22, 8, -1, 38, 4, 1);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionGpPrioIntCopy,
                                                         0, offset + 38, 4, 4);
    _FP_ACTION_ADD_TWO(unit, stage_fc, bcmFieldActionGpPrioIntNew,
                                              0, offset + 22, 8, -1, 38, 4, 5);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionGpPrioIntTos,
                                                         0, offset + 38, 4, 6);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionGpPrioIntCancel,
                                                         0, offset + 38, 4, 7);
    _FP_ACTION_ADD_TWO(unit, stage_fc, bcmFieldActionGpUcastCosQNew,
                                              0, offset + 22, 4, -1, 38, 4, 8);
    _FP_ACTION_ADD_TWO(unit, stage_fc, bcmFieldActionGpMcastCosQNew,
                                              0, offset + 26, 4, -1, 38, 4, 9);

    /* IFP_MIRROR_SET(12b) */
    offset = mirror_offset;

    _FP_ACTION_ADD_TWO(unit, stage_fc, bcmFieldActionMirrorIngress,
                                          0, offset + 8, 4, 0, offset , 8, 0);

    _FP_ACTION_ADD_TWO(unit, stage_fc, bcmFieldActionMirrorEgress,
                                          0, offset + 8, 4, 0, offset , 8, 0);

    /* IFP_LB_CONTROLS_SET(3b) */
    offset = lb_offset;

    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionDynamicHgTrunkCancel,
                                                           0, offset + 0, 1, 1);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionTrunkLoadBalanceCancel,
                                                           0, offset + 1, 1, 1);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionEcmpLoadBalanceCancel,
                                                           0, offset + 2, 1, 1);

    /* IFP_NAT_OVERRIDE_SET(1b) */
    offset = nat_override_offset;

    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionNatCancel,
                                                           0, offset + 0, 1, 1);
    /* IFP_COPY_TO_CPU_SET(17b) */
    offset = copytocpu_offset;

    _FP_ACTION_ADD_TWO(unit, stage_fc, bcmFieldActionRpCopyToCpu,
                                        0, offset + 9, 8, -1, offset + 0, 3, 1);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionRpCopyToCpuCancel,
                                                           0, offset + 0, 3, 2);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionRpSwitchToCpuCancel,
                                                           0, offset + 0, 3, 3);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionRpSwitchToCpuReinstate,
                                                           0, offset + 0, 3, 4);
    _FP_ACTION_ADD_TWO(unit, stage_fc, bcmFieldActionRpTimeStampToCpu,
                                        0, offset + 9, 8, -1, offset + 0, 3, 5);

    _FP_ACTION_ADD_TWO(unit, stage_fc, bcmFieldActionYpCopyToCpu,
                                        0, offset + 9, 8, -1, offset + 3, 3, 1);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionYpCopyToCpuCancel,
                                                           0, offset + 3, 3, 2);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionYpSwitchToCpuCancel,
                                                           0, offset + 3, 3, 3);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionYpSwitchToCpuReinstate,
                                                           0, offset + 3, 3, 4);
    _FP_ACTION_ADD_TWO(unit, stage_fc, bcmFieldActionYpTimeStampToCpu,
                                        0, offset + 9, 8, -1, offset + 3, 3, 5);

    _FP_ACTION_ADD_TWO(unit, stage_fc, bcmFieldActionGpCopyToCpu,
                                        0, offset + 9, 8, -1, offset + 6, 3, 1);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionGpCopyToCpuCancel,
                                                           0, offset + 6, 3, 2);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionGpSwitchToCpuCancel,
                                                           0, offset + 6, 3, 3);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionGpSwitchToCpuReinstate,
                                                           0, offset + 6, 3, 4);
    _FP_ACTION_ADD_TWO(unit, stage_fc, bcmFieldActionGpTimeStampToCpu,
                                        0, offset + 9, 8, -1, offset + 6, 3, 5);

    /* IFP_CUT_THRU_OVERRIDE_SET(1b) */
    offset = cutthrough_offset;

    /* IFP_URPF_OVERRIDE_SET(1b) */
    offset = urpf_offset;

    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionDoNotCheckUrpf,
                                                           0, offset + 0, 1, 1);

    /* IFP_TTL_SET(1b) */
    offset = ttl_offset;

    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionDoNotChangeTtl,
                                                           0, offset + 0, 1, 1);

    /* IFP_PROFILE_SET_2(52b) */
    offset = profile_two_offset;

    _FP_ACTION_ADD_TWO(unit, stage_fc, bcmFieldActionRpEcnNew,
                                        0, offset + 0, 2, -1, offset + 6, 1, 1);
    _FP_ACTION_ADD_TWO(unit, stage_fc, bcmFieldActionYpEcnNew,
                                        0, offset + 2, 2, -1, offset + 7, 1, 1);
    _FP_ACTION_ADD_TWO(unit, stage_fc, bcmFieldActionGpEcnNew,
                                        0, offset + 4, 2, -1, offset + 8, 1, 1);

    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionRpPrioPktCopy,
                                                          0, offset + 18, 3, 4);
    _FP_ACTION_ADD_TWO(unit, stage_fc, bcmFieldActionRpPrioPktNew,
                                       0, offset + 9, 3, -1, offset + 18, 3, 5);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionRpPrioPktTos,
                                                          0, offset + 18, 3, 6);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionRpPrioPktCancel,
                                                          0, offset + 18, 3, 7);

    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionYpPrioPktCopy,
                                                          0, offset + 21, 3, 4);
    _FP_ACTION_ADD_TWO(unit, stage_fc, bcmFieldActionYpPrioPktNew,
                                      0, offset + 12, 3, -1, offset + 21, 3, 5);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionYpPrioPktTos,
                                                          0, offset + 21, 3, 6);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionYpPrioPktCancel,
                                                          0, offset + 21, 3, 7);

    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionGpPrioPktCopy,
                                                          0, offset + 24, 3, 4);
    _FP_ACTION_ADD_TWO(unit, stage_fc, bcmFieldActionGpPrioPktNew,
                                      0, offset + 15, 3, -1, offset + 24, 3, 5);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionGpPrioPktTos,
                                                          0, offset + 24, 3, 6);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionGpPrioPktCancel,
                                                          0, offset + 24, 3, 7);

    _FP_ACTION_ADD_TWO(unit, stage_fc, bcmFieldActionRpDscpNew,
                                      0, offset + 40, 6, -1, offset + 27, 2, 1);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionRpDscpCancel,
                                                          0, offset + 27, 2, 2);

    _FP_ACTION_ADD_TWO(unit, stage_fc, bcmFieldActionYpDscpNew,
                                      0, offset + 34, 6, -1, offset + 29, 2, 1);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionYpDscpCancel,
                                                          0, offset + 29, 2, 2);

    _FP_ACTION_ADD_TWO(unit, stage_fc, bcmFieldActionGpDscpNew,
                                      0, offset + 46, 6, -1, offset + 31, 3, 3);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionGpDscpCancel,
                                                          0, offset + 31, 3, 4);

    _FP_ACTION_ADD_TWO(unit, stage_fc, bcmFieldActionGpTosPrecedenceNew,
                                      0, offset + 46, 6, -1, offset + 31, 3, 1);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionGpTosPrecedenceCopy,
                                                          0, offset + 31, 3, 2);

    /* IFP_L3SW_CHANGE_L2_SET(33b) */
    offset = l3swl2change_offset;

    _FP_ACTION_ADD_TWO(unit, stage_fc, bcmFieldActionDstMacNew,
                                      0, offset + 0, 15, -1, offset + 19, 4, 1);
    _FP_ACTION_ADD_TWO(unit, stage_fc, bcmFieldActionSrcMacNew,
                                      0, offset + 0, 15, -1, offset + 19, 4, 1);
    _FP_ACTION_ADD_TWO(unit, stage_fc, bcmFieldActionOuterVlanNew,
                                      0, offset + 0, 15, -1, offset + 19, 4, 1);
    _FP_ACTION_ADD_TWO(unit, stage_fc, bcmFieldActionVnTagNew,
                                      0, offset + 0, 15, -1, offset + 19, 4, 1);
    _FP_ACTION_ADD_TWO(unit, stage_fc, bcmFieldActionVnTagDelete,
                                      0, offset + 0, 15, -1, offset + 19, 4, 1);
    _FP_ACTION_ADD_TWO(unit, stage_fc, bcmFieldActionEtagNew,
                                      0, offset + 0, 15, -1, offset + 19, 4, 1);
    _FP_ACTION_ADD_TWO(unit, stage_fc, bcmFieldActionEtagDelete,
                                      0, offset + 0, 15, -1, offset + 19, 4, 1);
    _FP_ACTION_ADD_THREE(unit, stage_fc, bcmFieldActionFabricQueue, 0,
                     offset + 0, 2, -1,  offset + 2, 16, -1, offset + 19, 4, 3);
    _FP_ACTION_ADD_TWO(unit, stage_fc, bcmFieldActionAddClassTag,
                                      0, offset + 0, 16, -1, offset + 19, 4, 4);
    _FP_ACTION_ADD_THREE(unit, stage_fc, bcmFieldActionL3Switch,
                       _BCM_FIELD_ACTION_L3SWITCH_NEXT_HOP,  offset + 0, 15, -1,
                                          offset + 17, 1, 0, offset + 19, 4, 6);
    _FP_ACTION_ADD_THREE(unit, stage_fc, bcmFieldActionL3Switch,
                            _BCM_FIELD_ACTION_L3SWITCH_ECMP, offset + 0, 11, -1,
                                          offset + 17, 1, 1, offset + 19, 4, 6);
    _FP_ACTION_ADD_TWO(unit, stage_fc, bcmFieldActionMultipathHash,
                                      0, offset + 11, 3, -1, offset + 19, 4, 6);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionL3SwitchCancel,
                                                          0, offset + 19, 4, 7);
    _FP_ACTION_ADD_TWO(unit, stage_fc, bcmFieldActionEgressClassSelect,
                                       0, offset + 0, 4, -1, offset + 19, 4, 8);
    _FP_ACTION_ADD_TWO(unit, stage_fc, bcmFieldActionHiGigClassSelect,
                                       0, offset + 4, 3, -1, offset + 19, 4, 8);
    _FP_ACTION_ADD_TWO(unit, stage_fc, bcmFieldActionNewClassId,
                                       0, offset + 7, 9, -1, offset + 19, 4, 8);

    /* IFP_CHANGE_CPU_COS_SET(8b) */
    offset = cpucos_offset;

    _FP_ACTION_ADD_TWO(unit, stage_fc, bcmFieldActionCosQCpuNew,
                                        0, offset + 0, 6, -1, offset + 6, 2, 1);
    _FP_ACTION_ADD_THREE(unit, stage_fc, bcmFieldActionServicePoolIdNew, 0,
                                                              offset + 2, 2, -1,
                                                              offset + 5, 1, 1,
                                                              offset + 6, 2, 2);

    /* IFP_DROP_SET(6b) */
    offset = drop_offset;

    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionRpDrop,
                                                           0, offset + 0, 2, 1);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionRpDropCancel,
                                                           0, offset + 0, 2, 2);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionYpDrop,
                                                           0, offset + 2, 2, 1);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionYpDropCancel,
                                                           0, offset + 2, 2, 2);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionGpDrop,
                                                           0, offset + 4, 2, 1);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionGpDropCancel,
                                                           0, offset + 4, 2, 2);

    /* IFP_MIRROR_OVERRIDE_SET(1b) */
    offset = mirror_override_offset;

    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionMirrorOverride,
                                                           0, offset + 0, 1, 1);

    /* IFP_SFLOW_SET(1b) */
    offset = sflow_offset;

    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionIngSampleEnable,
                                                           0, offset + 0, 1, 1);

    /* IFP_INSTRUMENTATION_TRIGGERS_ENABLE_SET(1b)*/
    offset = debug_offset;

    /* IFP_REDIRECT_SET(38b) */
    offset = redirect_offset;

    _FP_ACTION_ADD_FOUR(unit, stage_fc, bcmFieldActionRedirect,
                                                _BCM_FIELD_ACTION_REDIRECT_DGLP,
                                         offset + 6, 16, -1, offset + 22 , 1, 0,
                                            offset + 3, 3, 0, offset + 0, 3, 1);
    _FP_ACTION_ADD_FOUR(unit, stage_fc, bcmFieldActionRedirect,
                                                 _BCM_FIELD_ACTION_REDIRECT_DVP,
                                          offset + 6, 14, -1, offset + 22, 1, 0,
                                            offset + 3, 3, 0, offset + 0, 3, 1);
    _FP_ACTION_ADD_FOUR(unit, stage_fc, bcmFieldActionRedirect,
                                               _BCM_FIELD_ACTION_REDIRECT_TRUNK,
                                          offset + 6, 16, -1, offset + 22, 1, 1,
                                            offset + 3, 3, 0, offset + 0, 3, 1);
    _FP_ACTION_ADD_FOUR(unit, stage_fc, bcmFieldActionRedirectTrunk, 0,
                                          offset + 6, 16, -1, offset + 22, 1, 1,
                                            offset + 3, 3, 0, offset + 0, 3, 1);
    _FP_ACTION_ADD_FOUR(unit, stage_fc,
                                     bcmFieldActionUnmodifiedPacketRedirectPort,
                                                _BCM_FIELD_ACTION_REDIRECT_DGLP,
                                          offset + 6, 17, -1, offset + 18, 1, 1,
                                            offset + 3, 3, 1, offset + 0, 3, 1);
    _FP_ACTION_ADD_THREE(unit, stage_fc, bcmFieldActionRedirectEgrNextHop,
                                            _BCM_FIELD_ACTION_REDIRECT_NEXT_HOP,
                        offset + 6, 16, -1, offset + 3, 3, 2, offset + 0, 3, 1);
    _FP_ACTION_ADD_THREE(unit, stage_fc, bcmFieldActionRedirectEgrNextHop,
                                                _BCM_FIELD_ACTION_REDIRECT_ECMP,
                        offset + 6, 16, -1, offset + 3, 3, 3, offset + 0, 3, 1);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionRedirectCancel,
                                                           0, offset + 0, 3, 2);
    _FP_ACTION_ADD_THREE(unit, stage_fc, bcmFieldActionRedirectPbmp, 0,
                       offset + 6, 10, -1, offset + 23, 2, 0, offset + 0, 3, 3);
    _FP_ACTION_ADD_THREE(unit, stage_fc, bcmFieldActionRedirectVlan, 0,
                        offset + 17, 1, 0, offset + 23, 2, 1, offset + 0, 3, 3);
    _FP_ACTION_ADD_FOUR(unit, stage_fc, bcmFieldActionRedirectBcastPbmp, 0,
                                          offset + 6, 10, -1, offset + 17, 1, 1,
                                           offset + 23, 2, 1, offset + 0, 3, 3);
    _FP_ACTION_ADD_THREE(unit, stage_fc, bcmFieldActionRedirectMcast, 0,
                       offset + 6, 14, -1, offset + 23, 2, 2, offset + 0, 3, 3);
    _FP_ACTION_ADD_THREE(unit, stage_fc, bcmFieldActionRedirectIpmc, 0,
                       offset + 6, 14, -1, offset + 23, 2, 3, offset + 0, 3, 3);
    _FP_ACTION_ADD_TWO(unit, stage_fc, bcmFieldActionEgressMask, 0,
                                          offset + 6, 10, -1, offset + 0, 3, 4);
    _FP_ACTION_ADD_TWO(unit, stage_fc, bcmFieldActionEgressPortsAdd, 0,
                                          offset + 6, 10, -1, offset + 0, 3, 5);


    /* IFP_NAT_SET(12b) */
    offset = nat_offset;

    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionNat,
                                                        0, offset + 11, 1, -1);
    _FP_ACTION_ADD_TWO(unit, stage_fc, bcmFieldActionNatEgressOverride,
                                    0, offset + 0, 10, -1, offset + 10, 1, -1);

    /* All actions which have corresponding Colored(Gp/Yp/Rp) Actions needs to be
     * initialized with some default configuration(offset = 0, width = 0, value = 0).
     */
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionCosQNew, 0, 0, 0, 0);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionPrioIntCopy, 0, 0, 0, 0);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionPrioIntNew, 0, 0, 0, 0);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionPrioIntTos, 0, 0, 0, 0);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionPrioIntCancel, 0, 0, 0, 0);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionUcastCosQNew, 0, 0, 0, 0);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionMcastCosQNew, 0, 0, 0, 0);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionDropPrecedence, 0, 0, 0, 0);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionColorIndependent, 0, 0, 0, 0);    
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionEcnNew, 0, 0, 0, 0);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionPrioPktCopy, 0, 0, 0, 0);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionPrioPktNew, 0, 0, 0, 0);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionPrioPktTos, 0, 0, 0, 0);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionPrioPktCancel, 0, 0, 0, 0);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionDscpNew, 0, 0, 0, 0);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionDscpCancel, 0, 0, 0, 0);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionCopyToCpu, 0, 0, 0, 0);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionCopyToCpuCancel, 0, 0, 0, 0);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionSwitchToCpuCancel, 0, 0, 0, 0);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionSwitchToCpuReinstate, 0, 0, 0, 0);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionTimeStampToCpu, 0, 0, 0, 0);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionTimeStampToCpuCancel, 0, 0, 0, 0);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionGpTimeStampToCpuCancel, 0, 0, 0, 0);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionYpTimeStampToCpuCancel, 0, 0, 0, 0);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionRpTimeStampToCpuCancel, 0, 0, 0, 0);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionDrop, 0, 0, 0, 0);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionDropCancel, 0, 0, 0, 0);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionPrioPktAndIntCopy, 0, 0, 0, 0);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionPrioPktAndIntNew, 0, 0, 0, 0);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionPrioPktAndIntTos, 0, 0, 0, 0);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionPrioPktAndIntCancel, 0, 0, 0, 0);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionRpPrioPktAndIntCopy, 0, 0, 0, 0);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionRpPrioPktAndIntNew, 0, 0, 0, 0);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionRpPrioPktAndIntTos, 0, 0, 0, 0);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionRpPrioPktAndIntCancel, 0, 0, 0, 0);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionGpPrioPktAndIntCopy, 0, 0, 0, 0);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionGpPrioPktAndIntNew, 0, 0, 0, 0);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionGpPrioPktAndIntTos, 0, 0, 0, 0);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionGpPrioPktAndIntCancel, 0, 0, 0, 0);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionYpPrioPktAndIntCopy, 0, 0, 0, 0);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionYpPrioPktAndIntNew, 0, 0, 0, 0);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionYpPrioPktAndIntTos, 0, 0, 0, 0);
    _FP_ACTION_ADD_ONE(unit, stage_fc, bcmFieldActionYpPrioPktAndIntCancel, 0, 0, 0, 0);

    return BCM_E_NONE;
}
/*
 * Function:
 *     _field_th_actionss_init
 * Purpose:
 *     Initialize device actions offsets, widths and encodings.
 * Parameters:
 *     unit       - (IN) BCM device number.
 *     stage_fc   - (IN) Field Processor stage control structure.
 *
 * Returns:
 *     BCM_E_NONE
 * Notes:
 *
 */
STATIC int
_field_th_actions_init(int unit, _field_stage_t *stage_fc)
{
    /* Allocated stage qualifiers configuration array. */
    _FP_XGS3_ALLOC(stage_fc->f_action_arr,
                   (bcmFieldActionCount * sizeof(_bcm_field_action_conf_t *)),
                   "Field Actions");
    if (NULL == stage_fc->f_action_arr) {
        LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META("Memory Resource Error\n")));
        return (BCM_E_MEMORY);
    }

    switch (stage_fc->stage_id) {
        case _BCM_FIELD_STAGE_INGRESS:
            return (_field_th_common_actions_init(unit, stage_fc));
        case _BCM_FIELD_STAGE_EXACTMATCH:
            return (_field_th_common_actions_init(unit, stage_fc));
        default:
            ;
    }

    sal_free(stage_fc->f_action_arr);
    return (BCM_E_NONE);
}
/*
 * Function:
 *     _field_th_lookup_qualifiers_init
 * Purpose:
 *     Initialize device stage lookup qaualifiers
 *     select codes & offsets
 * Parameters:
 *     unit       - (IN) BCM device number.
 *     stage_fc   - (IN) Field Processor stage control structure.
 * Returns:
 *     BCM_E_NONE
 */
STATIC int
_field_th_lookup_qualifiers_init(int unit, _field_stage_t *stage_fc)
{
    const unsigned f1_offset = 164, f2_offset = 36, f3_offset = 0;
    _FP_QUAL_DECL;

    /* Input parameters check. */
    if (NULL == stage_fc) {
        return (BCM_E_PARAM);
    }

    /* Allocated stage qualifiers configuration array. */
    _FP_XGS3_ALLOC(stage_fc->f_qual_arr,
        (_bcmFieldQualifyCount * sizeof(_bcm_field_qual_info_t *)),
         "Field qualifiers");
    if (NULL == stage_fc->f_qual_arr) {
        return (BCM_E_MEMORY);
    }

    /* Enable the overlay of Sender Ethernet Address onto MACSA
     * on ARP/RARP packets.
     */
    BCM_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, ING_CONFIG_64r, REG_PORT_ANY,
                                ARP_VALIDATION_ENf, 1));

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyStageLookup,
                 _bcmFieldSliceSelDisable, 0, 0, 0);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp4,
                 _bcmFieldSliceSelDisable, 0, 0, 0);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp6,
                 _bcmFieldSliceSelDisable, 0, 0, 0);

    /* FPF1 */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInnerIpProtocolCommon,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceIpHeaderSelect, 1, f1_offset, 3);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocolCommon,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceIpHeaderSelect, 0, f1_offset, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTunnelTerminated,
                 _bcmFieldSliceSelFpf1, 0, f1_offset + 3, 1);
    /* 1-bit IpInfo is used to qualify on BCM_FIELD_IP_CHECKSUM_OK */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpInfo,
                 _bcmFieldSliceSelFpf1, 0, f1_offset + 4, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerTpid,
                 _bcmFieldSliceSelFpf1, 0, f1_offset + 5, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterTpid,
                 _bcmFieldSliceSelFpf1, 0, f1_offset + 7, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2Format,
                 _bcmFieldSliceSelFpf1, 0, f1_offset + 9, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigProxy,
                 _bcmFieldSliceSelFpf1, 0, f1_offset + 11, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                 _bcmFieldSliceSelFpf1, 0, f1_offset + 12, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf1, 0, f1_offset + 13, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf1, 0, f1_offset + 13, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf1, 0, f1_offset + 25, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf1, 0, f1_offset + 26, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf1, 0, f1_offset + 29, 2);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerIpType,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceIpHeaderSelect, 1, f1_offset + 31,
                               5);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceIpHeaderSelect, 0, f1_offset + 31,
                               5);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcPort,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf1, 0,
                     _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityGlp,
                     0,
                     f1_offset + 36, 8, /* Port value in SGLP */
                     f1_offset + 44, 8, /* Module value in SGLP */
                     f1_offset + 52, 1, /* Trunk bit in SGLP (should be 0) */
                     0
                     );

    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcTrunk,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf1, 0,
                     _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityGlp,
                     0,
                     f1_offset + 36, 16, /* trunk id field of SGLP */
                     f1_offset + 52, 1,  /* trunk bit of SGLP */
                     0, 0,
                     0
                     );

    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcMplsGport,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf1, 0,
                     _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityMplsGport,
                     0,
                     f1_offset + 36, 17, /* S_FIELD */
                     f1_offset + 56, 1,  /* SVP_VALID */
                     0, 0,
                     0
                     );

    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcMimGport,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf1, 0,
                     _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityMimGport,
                     0,
                     f1_offset + 36, 17, /* S_FIELD */
                     f1_offset + 56, 1,  /* SVP_VALID */
                     0, 0,
                     0
                     );

     _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcVxlanGport,
                      _bcmFieldDevSelDisable, 0,
                      _bcmFieldSliceSelFpf1, 0,
                      _bcmFieldSliceSrcEntitySelect,
                      _bcmFieldFwdEntityVxlanGport,
                      0,
                      f1_offset + 36, 17, /* S_FIELD */
                      f1_offset + 56, 1,  /* SVP_VALID */
                      0, 0,
                      0
                      );

    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcModPortGport,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf1, 0,
                     _bcmFieldSliceSrcEntitySelect,
                     _bcmFieldFwdEntityModPortGport,
                     0,
                     f1_offset + 36, 16, /* mod + port field of unresolved SGLP */
                     0, 0,
                     0, 0,
                     0
                     );

    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcModuleGport,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf1, 0,
                     _bcmFieldSliceSrcEntitySelect,
                     _bcmFieldFwdEntityModPortGport,
                     0,
                     f1_offset + 36 + 8, 8, /* mod field of unresolved SGLP */
                     0, 0,
                     0, 0,
                     0
                     );

    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyInPort,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf1, 0,
                     _bcmFieldSliceSrcEntitySelect,
                     _bcmFieldFwdEntityPortGroupNum,
                     0,
                     f1_offset + 36, 8, /* ingress port field */
                     0, 0,
                     0, 0,
                     0
                     );

    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf1, 0,
                     _bcmFieldSliceSrcEntitySelect,
                     _bcmFieldFwdEntityPortGroupNum,
                     0,
                     f1_offset + 36 + 8, 8, /* ingress port group field */
                     0, 0,
                     0, 0,
                     0
                     );

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4Ports,
                 _bcmFieldSliceSelFpf1, 0, f1_offset + 56, 1);


    /* F2_0 */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerTtl,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 1, f2_offset, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 0, f2_offset, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                 _bcmFieldSliceSelFpf2, 0, f2_offset + 8, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerIpFrag,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 1, f2_offset + 14,
                               2);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 0, f2_offset + 14,
                               2);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerTos,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 1, f2_offset + 16,
                               8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 0, f2_offset + 16,
                               8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerL4DstPort,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 1, f2_offset + 24,
                               16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 0, f2_offset + 24,
                               16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerL4SrcPort,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 1, f2_offset + 40,
                               16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 0, f2_offset + 40,
                               16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerIpProtocol,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 1, f2_offset + 56,
                               8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 0, f2_offset + 56,
                               8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerDstIp,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 1, f2_offset + 64,
                               32);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 0, f2_offset + 64,
                               32);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerSrcIp,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 1, f2_offset + 96,
                               32);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 0, f2_offset + 96,
                               32);

    /* F2_1 */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerSrcIp6,
                               _bcmFieldSliceSelFpf2, 1,
                               _bcmFieldSliceIpHeaderSelect, 1, f2_offset, 128);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcIp6,
                               _bcmFieldSliceSelFpf2, 1,
                               _bcmFieldSliceIpHeaderSelect, 0, f2_offset, 128);

    /* F2_2 */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerDstIp6,
                               _bcmFieldSliceSelFpf2, 2,
                               _bcmFieldSliceIpHeaderSelect, 1, f2_offset, 128);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6,
                               _bcmFieldSliceSelFpf2, 2,
                               _bcmFieldSliceIpHeaderSelect, 0, f2_offset, 128);

    /* F2_3 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf2, 3, f2_offset + 16, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMac,
                 _bcmFieldSliceSelFpf2, 3, f2_offset + 32, 48);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstMac,
                 _bcmFieldSliceSelFpf2, 3, f2_offset + 80, 48);

    /* F2_4 */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerTos,
                               _bcmFieldSliceSelFpf2, 4,
                               _bcmFieldSliceIpHeaderSelect, 1, f2_offset, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                               _bcmFieldSliceSelFpf2, 4,
                               _bcmFieldSliceIpHeaderSelect, 0, f2_offset, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerIpProtocol,
                               _bcmFieldSliceSelFpf2, 4,
                               _bcmFieldSliceIpHeaderSelect, 1, f2_offset + 8,
                               8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                               _bcmFieldSliceSelFpf2, 4,
                               _bcmFieldSliceIpHeaderSelect, 0, f2_offset + 8,
                               8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerDstIp,
                               _bcmFieldSliceSelFpf2, 4,
                               _bcmFieldSliceIpHeaderSelect, 1, f2_offset + 16,
                               32);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                               _bcmFieldSliceSelFpf2, 4,
                               _bcmFieldSliceIpHeaderSelect, 0, f2_offset + 16,
                               32);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerSrcIp,
                               _bcmFieldSliceSelFpf2, 4,
                               _bcmFieldSliceIpHeaderSelect, 1, f2_offset + 48,
                               32);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                               _bcmFieldSliceSelFpf2, 4,
                               _bcmFieldSliceIpHeaderSelect, 0, f2_offset + 48,
                               32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMac,
                 _bcmFieldSliceSelFpf2, 4, f2_offset + 80, 48);

    /* F2_5 */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerTos,
                               _bcmFieldSliceSelFpf2, 5,
                               _bcmFieldSliceIpHeaderSelect, 1, f2_offset, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                               _bcmFieldSliceSelFpf2, 5,
                               _bcmFieldSliceIpHeaderSelect, 0, f2_offset, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerIpProtocol,
                               _bcmFieldSliceSelFpf2, 5,
                               _bcmFieldSliceIpHeaderSelect, 1, f2_offset + 8,
                               8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                               _bcmFieldSliceSelFpf2, 5,
                               _bcmFieldSliceIpHeaderSelect, 0, f2_offset + 8,
                               8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerDstIp,
                               _bcmFieldSliceSelFpf2, 5,
                               _bcmFieldSliceIpHeaderSelect, 1, f2_offset + 16,
                               32);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                               _bcmFieldSliceSelFpf2, 5,
                               _bcmFieldSliceIpHeaderSelect, 0, f2_offset + 16,
                               32);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerSrcIp,
                               _bcmFieldSliceSelFpf2, 5,
                               _bcmFieldSliceIpHeaderSelect, 1, f2_offset + 48,
                               32);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                               _bcmFieldSliceSelFpf2, 5,
                               _bcmFieldSliceIpHeaderSelect, 0, f2_offset + 48,
                               32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstMac,
                 _bcmFieldSliceSelFpf2, 5, f2_offset + 80, 48);

    /* F2_6 */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerSrcIp6High,
                               _bcmFieldSliceSelFpf2, 6,
                               _bcmFieldSliceIpHeaderSelect, 1, f2_offset, 64);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcIp6High,
                               _bcmFieldSliceSelFpf2, 6,
                               _bcmFieldSliceIpHeaderSelect, 0, f2_offset, 64);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerDstIp6High,
                               _bcmFieldSliceSelFpf2, 6,
                               _bcmFieldSliceIpHeaderSelect, 1, f2_offset + 64,
                               64);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6High,
                               _bcmFieldSliceSelFpf2, 6,
                               _bcmFieldSliceIpHeaderSelect, 0, f2_offset + 64,
                               64);

    /* F2_7 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVnTag,
                 _bcmFieldSliceSelFpf2, 7, f2_offset + 32, 33);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySnap,
                 _bcmFieldSliceSelFpf2, 7, f2_offset + 64, 40);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyLlc,
                 _bcmFieldSliceSelFpf2, 7, f2_offset + 104, 24);

    /* F2_8 */
    _FP_QUAL_ADD(unit, stage_fc, _bcmFieldQualifyData0,
                 _bcmFieldSliceSelFpf2, 8, f2_offset, 128);

    /* F2_9 */
    _FP_QUAL_ADD(unit, stage_fc, _bcmFieldQualifyData1,
                 _bcmFieldSliceSelFpf2, 9, f2_offset, 128);

    /* F2_10 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanInner,
                 _bcmFieldSliceSelFpf2, 10, f2_offset + 0, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanOuter,
                 _bcmFieldSliceSelFpf2, 10, f2_offset + 3, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFcoeSOF,
                 _bcmFieldSliceSelFpf2, 10, f2_offset + 6, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanDFCtl,
                 _bcmFieldSliceSelFpf2, 10, f2_offset + 14, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanCSCtl,
                 _bcmFieldSliceSelFpf2, 10, f2_offset + 22, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanFCtl,
                 _bcmFieldSliceSelFpf2, 10, f2_offset + 30, 24);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanType,
                 _bcmFieldSliceSelFpf2, 10, f2_offset + 54, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanDstId,
                 _bcmFieldSliceSelFpf2, 10, f2_offset + 62, 24);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanSrcId,
                 _bcmFieldSliceSelFpf2, 10, f2_offset + 86, 24);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanRCtl,
                 _bcmFieldSliceSelFpf2, 10, f2_offset + 110, 8);

    /* F3_0 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlan,
                 _bcmFieldSliceSelFpf3, 0, f3_offset + 0, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf3, 0, f3_offset + 0, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
                 _bcmFieldSliceSelFpf3, 0, f3_offset + 12, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
                 _bcmFieldSliceSelFpf3, 0, f3_offset + 13, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf3, 0, f3_offset + 16, 16);

    /* F3_1 */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerL4DstPort,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceIpHeaderSelect, 1, f3_offset + 0,
                               16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceIpHeaderSelect, 0, f3_offset + 0,
                               16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerL4SrcPort,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceIpHeaderSelect, 1, f3_offset + 16,
                               16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceIpHeaderSelect, 0, f3_offset + 16,
                               16);

    /* F3_2 */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerTos,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceIpHeaderSelect, 1, f3_offset + 0,
                               8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceIpHeaderSelect, 0, f3_offset + 0,
                               8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerIpProtocol,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceIpHeaderSelect, 1, f3_offset + 8,
                               8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceIpHeaderSelect, 0, f3_offset + 8,
                               8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyExtensionHeaderSubCode,
                 _bcmFieldSliceSelFpf3, 2, f3_offset + 16, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyExtensionHeaderType,
                 _bcmFieldSliceSelFpf3, 2, f3_offset + 24, 8);


    /* F3_3 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL3,
                 _bcmFieldSliceSelFpf3, 3, f3_offset + 0, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlan,
                 _bcmFieldSliceSelFpf3, 3, f3_offset + 8, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf3, 3, f3_offset + 8, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
                 _bcmFieldSliceSelFpf3, 3, f3_offset + 20, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
                 _bcmFieldSliceSelFpf3, 3, f3_offset + 21, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcModuleGport,
                 _bcmFieldSliceSelFpf3, 3, f3_offset + 24, 8);

    /* F3_4 ? */

    /* F3_5 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanVFTHopCount,
                 _bcmFieldSliceSelFpf3, 5, f3_offset, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanVFTVsanId,
                 _bcmFieldSliceSelFpf3, 5, f3_offset + 8, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanVFTVsanPri,
                 _bcmFieldSliceSelFpf3, 5, f3_offset + 20, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanVFTVersion,
                 _bcmFieldSliceSelFpf3, 5, f3_offset + 23, 2);

    /* DWF3 */
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInnerTos,
                            _bcmFieldSliceSelFpf3, 0,
                            _bcmFieldSliceIpHeaderSelect, 1, f3_offset + 0, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTos,
                            _bcmFieldSliceSelFpf3, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, f3_offset + 0, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInnerTtl,
                            _bcmFieldSliceSelFpf3, 0,
                            _bcmFieldSliceIpHeaderSelect, 1, f3_offset + 8, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                            _bcmFieldSliceSelFpf3, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, f3_offset + 8, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                            _bcmFieldSliceSelFpf3, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, f3_offset + 16,
                            6);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInnerIpFrag,
                            _bcmFieldSliceSelFpf3, 0,
                            _bcmFieldSliceIpHeaderSelect, 1, f3_offset + 22,
                            2);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                            _bcmFieldSliceSelFpf3, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, f3_offset + 22,
                            2);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInnerIpProtocol,
                            _bcmFieldSliceSelFpf3, 0,
                            _bcmFieldSliceIpHeaderSelect, 1, f3_offset + 24,
                            8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                            _bcmFieldSliceSelFpf3, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, f3_offset + 24,
                            8);

    /* DWF2_0 */
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInnerTtl,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 1, f2_offset + 0,
                            8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, f2_offset + 0,
                            8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, f2_offset + 8,
                            6);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInnerIpFrag,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 1, f2_offset + 14,
                            2);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, f2_offset + 14,
                            2);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInnerTos,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 1, f2_offset + 16,
                            8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTos,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, f2_offset + 16,
                            8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInnerL4DstPort,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 1, f2_offset + 24,
                            16);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, f2_offset + 24,
                            16);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInnerL4SrcPort,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 1, f2_offset + 40,
                            16);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, f2_offset + 40,
                            16);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInnerIpProtocol,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 1, f2_offset + 56,
                            8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, f2_offset + 56,
                            8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInnerDstIp,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 1, f2_offset + 64,
                            32);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, f2_offset + 64,
                            32);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInnerSrcIp,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 1, f2_offset + 96,
                            32);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, f2_offset + 96,
                            32);

    /* DWF2_1 */
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInnerSrcIp6,
                            _bcmFieldSliceSelFpf2, 1,
                            _bcmFieldSliceIpHeaderSelect, 1, f2_offset,
                            128);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifySrcIp6,
                            _bcmFieldSliceSelFpf2, 1,
                            _bcmFieldSliceIpHeaderSelect, 0, f2_offset,
                            128);

    /* DWF2_2 */
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, _bcmFieldQualifyData1,
                            _bcmFieldSliceSelFpf2, 2,
                            _bcmFieldSliceSelDisable, 0,
                            f2_offset, 128);

    /* DWF1 */
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc,
                            bcmFieldQualifyExtensionHeaderSubCode,
                            _bcmFieldSliceSelFpf1, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, f1_offset + 0,
                            8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyExtensionHeaderType,
                            _bcmFieldSliceSelFpf1, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, f1_offset + 8,
                            8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL3,
                            _bcmFieldSliceSelFpf1, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, f1_offset + 16,
                            8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                            _bcmFieldSliceSelFpf1, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, f1_offset + 24,
                            16);

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_th_egress_qualifiers_init
 * Purpose:
 *     Initialize device stage egress qaualifiers
 *     select codes & offsets
 * Parameters:
 *     unit       - (IN) BCM device number.
 *     stage_fc   - (IN) Field Processor stage control structure.
 *
 * Returns:
 *     BCM_E_NONE
 */
STATIC int
_field_th_egress_qualifiers_init(int unit, _field_stage_t *stage_fc)
{
    _FP_QUAL_DECL;
    _key_fld_ = KEYf;

    /* Input parameters check. */
    if (NULL == stage_fc) {
        return (BCM_E_PARAM);
    }

    /* Allocated stage qualifiers configuration array. */
    _FP_XGS3_ALLOC(stage_fc->f_qual_arr,
        (_bcmFieldQualifyCount * sizeof(_bcm_field_qual_info_t *)),
         "Field qualifiers");
    if (NULL == stage_fc->f_qual_arr) {
        return (BCM_E_MEMORY);
    }

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyStageEgress,
                 _bcmFieldSliceSelDisable, 0, 0, 0);

    /* EFP_KEY1(IPv4 Key) Qualifiers */

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4Ports,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 0, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIntPriority,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 1, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyColor,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 5, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 7, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 9, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 17, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 33, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIcmpTypeCode,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 33, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 49, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 57, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 65, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 97, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 129, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigProxy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 137, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 138, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 139, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 151, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 159, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMirrorCopy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 160, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 161, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 161, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 173, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 174, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 177, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 179, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 180, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOutPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 188, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 196, 5);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               201, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               201, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               201, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               201, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               201, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               201, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               201, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               201, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClass,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 1,
                               201, 12
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyEgressClassL3Interface,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 2,
                               201, 12
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassTrill,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 3,
                               201, 12
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassL2Gre,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 3,
                               201, 12
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIngressClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               201, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               201, 13
                               );
   _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassVPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               201, 13
                               );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 214, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 214, 13);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVpn,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 214, 13);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 228, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIntCongestionNotification,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 230, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 232, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp4,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 0, 0);

    /* EFP_KEY2 (IPv6 Singlewide Key) Qualifiers */

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4Ports,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 0, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 1, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcIp6,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceIp6AddrSelect,
                               _BCM_FIELD_EGRESS_SLICE_V6_KEY_MODE_SIP6,
                               9, 128);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceIp6AddrSelect,
                               _BCM_FIELD_EGRESS_SLICE_V6_KEY_MODE_DIP6,
                               9, 128);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcIp6High,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceIp6AddrSelect,
                               _BCM_FIELD_EGRESS_SLICE_V6_KEY_MODE_SIP_DIP_64,
                               9, 64);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6High,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceIp6AddrSelect,
                               _BCM_FIELD_EGRESS_SLICE_V6_KEY_MODE_SIP_DIP_64,
                               73, 64);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 137, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigProxy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 145, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 146, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 147, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 159, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 167, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMirrorCopy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 168, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 169, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 169, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 181, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 182, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 185, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 187, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 188, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 0,
                               196, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 0,
                               196, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 0,
                               196, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 0,
                               196, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 0,
                               196, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 0,
                               196, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 0,
                               196, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 0,
                               196, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassTrill,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 4,
                               196, 12
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassL2Gre,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 4,
                               196, 12
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClass,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 1,
                               196, 12
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyEgressClassL3Interface,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 2,
                               196, 12
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyOutPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 3,
                               196, 8
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIngressClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 0,
                               196, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 0,
                               196, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassVPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 0,
                               196, 13
                               );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 209, 5);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 214, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 214, 13);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVpn,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 214, 13);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 228, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIntCongestionNotification,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 230, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 232, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp6,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 0, 0);

    /* EFP_KEY3 (IPv6 Double Wide Key) Qualifiers */

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4Ports,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 0, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIntPriority,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 1, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyColor,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 5, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 7, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 9, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 17, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 33, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIcmpTypeCode,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 33, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 49, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 57, 128);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyExtensionHeaderSubCode,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 185, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyExtensionHeaderType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 193, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 201, 5);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClass,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3,
                               _bcmFieldSliceSelEgrClassF3, 0,
                               206, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyEgressClassL3Interface,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3,
                               _bcmFieldSliceSelEgrClassF3, 1,
                               206, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassTrill,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3,
                               _bcmFieldSliceSelEgrClassF3, 3,
                               206, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassL2Gre,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3,
                               _bcmFieldSliceSelEgrClassF3, 3,
                               206, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIntCongestionNotification,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 218, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 220, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp6,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 0, 0);

    /* EFP_KEY4(L2 Key) Qualifiers */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4Ports,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 0, 1);
    /* EG_NEXT_HOP_CLASS_ID */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClass,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrDvpKey4, 1,
                               2, 12);
    /* EG_L3_INTF_CLASS_ID */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyEgressClassL3Interface,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrDvpKey4, 2,
                               2, 12);
    /* EG_DVP_CLASS_ID */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassTrill,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrDvpKey4, 3,
                               2, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassWlan,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrDvpKey4, 3,
                               2, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassL2Gre,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrDvpKey4, 3,
                               2, 12);

    /* DVP_VALID + DVP + SPARE_DVP */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMplsGport,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrDvpKey4, 0,
                               1, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMimGport,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrDvpKey4, 0,
                               1, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstVxlanGport,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrDvpKey4, 0,
                               1, 17);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIntPriority,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 18, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyColor,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 22, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2Format,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 24, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 26, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMac,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 42, 48);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstMac,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 90, 48);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanTranslationHit,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 138, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigProxy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 139, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 140, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlan,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 141, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 141, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 142, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 145, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 157, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 165, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMirrorCopy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 166, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 167, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 167, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 179, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 180, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 183, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 185, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 186, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 0,
                               194, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 0,
                               194, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 0,
                               194, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 0,
                               194, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 0,
                               194, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 0,
                               194, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 0,
                               194, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 0,
                               194, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyOutPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 1,
                               194, 8);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyCpuQueue,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                     _bcmFieldSliceSelEgrClassF4, 2,
                     0,
                     194, 6, /* CpuCos - 6 bits */
                     0, 0,
                     0, 0,
                     0
                     );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIngressClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 0,
                               194, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 0,
                               194, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassVPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 0,
                               194, 12
                               );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 207, 5);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 212, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 212, 13);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVpn,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 212, 13);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 226, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIntCongestionNotification,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 228, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 230, 1);

    /*EFP_KEY6 (HiGiG Key)  Qualifiers*/
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFabricQueueTag,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 0, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigProxy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 160, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 161, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 162, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 170, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOutPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 178, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassF6, 0,
                               186, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassF6, 0,
                               186, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassF6, 0,
                               186, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassF6, 0,
                               186, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassF6, 0,
                               186, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassF6, 0,
                               186, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassF6, 0,
                               186, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassF6, 0,
                               186, 13
                               );

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassF6, 0,
                               186, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassVPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassF6, 0,
                               186, 13
                               );

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClass,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassF3, 0,
                               186, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyEgressClassL3Interface,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassF6, 1,
                               186, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassTrill,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassF6, 2,
                               186, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassL2Gre,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassF6, 2,
                               186, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIntCongestionNotification,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 199, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 201, 1);

    /* EFP_KEY7(Loopback Key) qualifiers */

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigProxy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7, 128, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7, 129, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7, 130, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7, 138, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOutPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7, 146, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIntPriority,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 154, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyColor,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 158, 2);

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassF7, 0,
                               160, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassF7, 0,
                               160, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassF7, 0,
                               160, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassF7, 0,
                               160, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassF7, 0,
                               160, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassF7, 0,
                               160, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassF7, 0,
                               160, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassF7, 0,
                               160, 13
                               );

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassF7, 0,
                               160, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassVPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassF7, 0,
                               160, 13
                               );

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClass,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassF7, 0,
                               160, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyEgressClassL3Interface,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassF7, 1,
                               160, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassTrill,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassF7, 2,
                               160, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassL2Gre,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassF7, 2,
                               160, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIntCongestionNotification,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7, 173, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7, 175, 1);

    /* EFP_KEY8(Bytes After L2 Key) Qualifiers */

    /* DVP_VALID + DVP + SPARE_DVP */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMplsGport,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrDvpKey4, 0,
                               0, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMimGport,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrDvpKey4, 0,
                               0, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstVxlanGport,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrDvpKey4, 0,
                               0, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClass,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassF3, 0,
                               1, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyEgressClassL3Interface,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassF3, 1,
                               1, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassTrill,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassF3, 2,
                               1, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassL2Gre,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassF3, 2,
                               1, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIntPriority,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8, 17, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyColor,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8, 21, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8, 23, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigProxy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8, 39, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8, 40, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8, 41, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMirrorCopy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8, 49, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8, 50, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8, 51, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyOutPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassF8, 1,
                               203, 8);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyCpuQueue,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                     _bcmFieldSliceSelEgrClassF8, 2,
                     0,
                     203, 6, /* CpuCos - 6 bits */
                     0, 0,
                     0, 0,
                     0
                     );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassF8, 0,
                               203, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassF8, 0,
                               203, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassF8, 0,
                               203, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassF8, 0,
                               203, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassF8, 0,
                               203, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassF8, 0,
                               203, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassF8, 0,
                               203, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassF8, 0,
                               203, 13
                               );

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassF8, 0,
                               203, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassVPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassF8, 0,
                               203, 13
                               );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8, 216, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8, 216, 14);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVpn,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8, 216, 14);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8, 230, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIntCongestionNotification,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8, 232, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8, 234, 1);



    return (BCM_E_NONE);
}

 /*
  * Function:
  *     _bcm_field_th_egress_mode_set
  *
  * Purpose:
  *     Helper function to _bcm_field_th_mode_install that sets the mode of a
  *     slice in a register value that is to be used for FP_SLICE_CONFIGr.
  *
  * Parameters:
  *     unit       - (IN) BCM device number.
  *     slice_numb - (IN) Slice number to set mode for.
  *     fg         - (IN) Installed group structure.
  *     flags      - (IN) New group/slice mode.
  *
  * Returns:
  *     BCM_E_XXX
  */
int
_bcm_field_th_egress_mode_set(int unit, uint8 slice_numb,
                               _field_group_t *fg, uint8 flags)
{
    uint32 mode_val[2];

    /* Input parameters check. */
    if ((NULL == fg) || (slice_numb >= COUNTOF(_th_efp_slice_mode))) {
        return (BCM_E_PARAM);
    }

    mode_val[1]  = _BCM_FIELD_EGRESS_SLICE_V6_KEY_MODE_SIP6;

    if (flags & _FP_GROUP_SPAN_DOUBLE_SLICE) {

        /* DstIP6 + SrcIp6 or IPv4 + L2 Double wide key. */
        if (((_BCM_FIELD_EFP_KEY2 == fg->sel_codes[0].fpf3) &&
            (_BCM_FIELD_EFP_KEY3 == fg->sel_codes[1].fpf3))) {
            mode_val[0] = _BCM_FIELD_EGRESS_SLICE_MODE_DOUBLE;
            if (_FP_SELCODE_DONT_CARE != fg->sel_codes[0].ip6_addr_sel) {
                mode_val[1] =fg->sel_codes[0].ip6_addr_sel;
            }
        }

        if (_BCM_FIELD_EFP_KEY4 == fg->sel_codes[1].fpf3) {
            if ((_BCM_FIELD_EFP_KEY1 == fg->sel_codes[0].fpf3) &&
                (BCM_FIELD_QSET_TEST(fg->qset, bcmFieldQualifyIp4))) {
                 mode_val[0] = _BCM_FIELD_EGRESS_SLICE_MODE_DOUBLE;
            } else if (_BCM_FIELD_EFP_KEY2 == fg->sel_codes[0].fpf3) {
                 mode_val[0] = _BCM_FIELD_EGRESS_SLICE_MODE_DOUBLE_L3_V6;
                 if (_FP_SELCODE_DONT_CARE != fg->sel_codes[0].ip6_addr_sel) {
                     mode_val[1] =fg->sel_codes[0].ip6_addr_sel;
                 }
            } else if (_BCM_FIELD_EFP_KEY8 == fg->sel_codes[0].fpf3) {
                 mode_val[0] = _BCM_FIELD_EGRESS_SLICE_MODE_DOUBLE;
            } else {
                 mode_val[0] = _BCM_FIELD_EGRESS_SLICE_MODE_DOUBLE_L3_ANY;
            }
        }

        BCM_IF_ERROR_RETURN
            (soc_reg_fields32_modify(unit, EFP_SLICE_CONTROLr, REG_PORT_ANY, 2,
                                    _th_efp_slice_mode[slice_numb], mode_val));
    } else {
        if (_BCM_FIELD_EFP_KEY4 == fg->sel_codes[0].fpf3) {
            /* L2 - Slice mode. */
            mode_val[0] = _BCM_FIELD_EGRESS_SLICE_MODE_SINGLE_L2;

        } else  if ((_BCM_FIELD_EFP_KEY1 == fg->sel_codes[0].fpf3)  &&
                    BCM_FIELD_QSET_TEST(fg->qset, bcmFieldQualifyIp4)) {
            /* L3 - IPv4 single wide key. */
            mode_val[0] = _BCM_FIELD_EGRESS_SLICE_MODE_SINGLE_L3;

        } else if ((_BCM_FIELD_EFP_KEY2 == fg->sel_codes[0].fpf3)) {
            /* L3 - IPv6 single wide key. */
            mode_val[0] = _BCM_FIELD_EGRESS_SLICE_MODE_SINGLE_L3;
            if (_FP_SELCODE_DONT_CARE != fg->sel_codes[0].ip6_addr_sel) {
                mode_val[1] =fg->sel_codes[0].ip6_addr_sel;
            }
        } else {
            /* L3 common key. */
            mode_val[0] = _BCM_FIELD_EGRESS_SLICE_MODE_SINGLE_L3_ANY;
        }

        BCM_IF_ERROR_RETURN
            (soc_reg_fields32_modify(unit, EFP_SLICE_CONTROLr, REG_PORT_ANY, 2,
                                    _th_efp_slice_mode[slice_numb], mode_val));

    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_th_mode_set
 *
 * Purpose:
 *    Auxiliary routine used to set group pairing mode.
 * Parameters:
 *     unit       - (IN) BCM device number.
 *     slice_numb - (IN) Slice number to set mode for.
 *     fg         - (IN) Installed group structure.
 *     flags      - (IN) New group/slice mode.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_th_mode_set(int unit, uint8 slice_numb,
                       _field_group_t *fg, uint8 flags)
{
    int rv;     /* Operation return status. */

    /* Input parameter check. */
    if (NULL == fg) {
        return (BCM_E_PARAM);
    }

    switch (fg->stage_id) {
      case _BCM_FIELD_STAGE_INGRESS:
          rv = BCM_E_NONE; /* Mode and select codes programmed together. */
          break;
      case _BCM_FIELD_STAGE_LOOKUP:
          rv  = _bcm_field_td2_lookup_mode_set(unit, slice_numb, fg, flags);
          break;
      case _BCM_FIELD_STAGE_EGRESS:
          rv = _bcm_field_th_egress_mode_set(unit, slice_numb, fg, flags);
          break;
      default:
          rv = BCM_E_PARAM;
    }
    return (rv);
}
/*
 * Function:
 *     _field_th_egress_selcodes_install
 *
 * Purpose:
 *     Writes the field egress secondary select codes.
 *
 * Parameters:
 *     unit          - (IN) BCM device number
 *     fg            - (IN) Installed group structure.
 *     slice_numb    - (IN) Slice number to set mode for.
 *     selcode_index - (IN) Index to group slecodes.
 *
 *
 * Returns:
 *     BCM_E_INTERNAL - On read/write errors
 *     BCM_E_NONE     - Success
 *
 * Note:
 *     Unit lock should be held by calling function.
 */

STATIC int
_field_th_egress_selcodes_install(int            unit,
                                   _field_group_t *fg,
                                   uint8          slice_num,
                                   int            selcode_idx
                                   )
{
    uint8 idx; /* Index to EFP keys. */
    _field_sel_t *sel;

    if (NULL == fg) {
        return BCM_E_PARAM;
    }

    /* Max number of slices on Tomahawk for EFP is 4(slice 0 - slice 3) */
    if (slice_num >= 4)  {
        return BCM_E_INTERNAL;
    }

    sel = &fg->sel_codes[selcode_idx];
    if (sel == NULL) {
       return BCM_E_INTERNAL;
    }

    idx = 0;    
    /* Class Id Selector codes for different EFP keys (KEY1 - KEY8) */
    if (sel->egr_class_f1_sel != _FP_SELCODE_DONT_CARE) {
        BCM_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                                                   EFP_CLASSID_SELECTORr,
                                                   REG_PORT_ANY,
                                                   classfldtbl[slice_num][idx],
                                                   sel->egr_class_f1_sel
                                                   ));
    }

    idx++;
    if (sel->egr_class_f2_sel != _FP_SELCODE_DONT_CARE) {
        BCM_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                                                   EFP_CLASSID_SELECTORr,
                                                   REG_PORT_ANY,
                                                   classfldtbl[slice_num][idx],
                                                   sel->egr_class_f2_sel
                                                   ));
    }

    idx++;
    if (sel->egr_class_f3_sel != _FP_SELCODE_DONT_CARE) {
        BCM_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                                                   EFP_CLASSID_SELECTORr,
                                                   REG_PORT_ANY,
                                                   classfldtbl[slice_num][idx],
                                                   sel->egr_class_f3_sel
                                                   ));
    }

    idx++;
    if (sel->egr_class_f4_sel != _FP_SELCODE_DONT_CARE) {
        BCM_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                                                   EFP_CLASSID_SELECTORr,
                                                   REG_PORT_ANY,
                                                   classfldtbl[slice_num][idx],
                                                   sel->egr_class_f4_sel
                                                   ));
    }

    idx++;
    if (sel->egr_class_f6_sel != _FP_SELCODE_DONT_CARE) {
        BCM_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                                                   EFP_CLASSID_SELECTORr,
                                                   REG_PORT_ANY,
                                                   classfldtbl[slice_num][idx],
                                                   sel->egr_class_f6_sel
                                                   ));
    }

    idx++;
    if (sel->egr_class_f7_sel != _FP_SELCODE_DONT_CARE) {
        BCM_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                                                   EFP_CLASSID_SELECTORr,
                                                   REG_PORT_ANY,
                                                   classfldtbl[slice_num][idx],
                                                   sel->egr_class_f7_sel
                                                   ));
    }

    idx++;
    if (sel->egr_class_f8_sel != _FP_SELCODE_DONT_CARE) {
        BCM_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                                                   EFP_CLASSID_SELECTORr,
                                                   REG_PORT_ANY,
                                                   classfldtbl[slice_num][idx],
                                                   sel->egr_class_f8_sel
                                                   ));
    }

    /* SELECTOR CODES for EFP_KEY4_DVP_SELECTOR register */
    if (sel->egr_key4_dvp_sel != _FP_SELCODE_DONT_CARE) {
        BCM_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                                                   EFP_KEY4_DVP_SELECTORr,
                                                   REG_PORT_ANY,
                                                   dvpfldtbl[slice_num],
                                                   sel->egr_key4_dvp_sel
                                                  ));
    }
    /* SELECTOR CODES for EFP_KEY8_DVP_SELECTOR register */
    if (sel->egr_key8_dvp_sel != _FP_SELCODE_DONT_CARE) {
        BCM_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                                                   EFP_KEY8_DVP_SELECTORr,
                                                   REG_PORT_ANY,
                                                   dvpfldtbl[slice_num],
                                                   sel->egr_key8_dvp_sel
                                                  ));
    }

    /* SELECTOR CODES for EFP_KEY4_MDL_SELECTOR register */
    if (sel->egr_key4_mdl_sel != _FP_SELCODE_DONT_CARE) {
        BCM_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                                                   EFP_KEY4_MDL_SELECTORr,
                                                   REG_PORT_ANY,
                                                   mdlfldtbl[slice_num],
                                                   sel->egr_key4_mdl_sel
                                                  ));
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_field_th_selcodes_install
 *
 * Purpose:
 *     Writes the field select codes (ie. FPFx).
 *
 * Parameters:
 *     unit  - BCM device number
 *     fs    - slice that needs its select codes written
 *
 * Returns:
 *     BCM_E_INTERNAL - On read/write errors
 *     BCM_E_NONE     - Success
 *
 * Note:
 *     Unit lock should be held by calling function.
 */
int
_bcm_field_th_selcodes_install(int unit, _field_group_t *fg,
                                uint8 slice_numb, bcm_pbmp_t pbmp,
                                int selcode_index)
{
    int rv;    /* Operation return status. */

    /* Input parameters check. */
    if (NULL == fg) {
        return (BCM_E_PARAM);
    }

    /* Set slice mode. Single/Double/Triple, Intraslice */
    rv = _bcm_field_th_mode_set(unit, slice_numb, fg, fg->flags);
    BCM_IF_ERROR_RETURN(rv);
    switch (fg->stage_id) {
      case _BCM_FIELD_STAGE_LOOKUP:
          rv = _bcm_field_td2_lookup_selcodes_install(unit, fg, slice_numb,
                                                      selcode_index);
          break;
      case _BCM_FIELD_STAGE_EGRESS:
            rv = _field_th_egress_selcodes_install(unit, fg, slice_numb,
                                                         selcode_index);
            break;
      case _BCM_FIELD_STAGE_EXTERNAL:
      case _BCM_FIELD_STAGE_INGRESS:
          rv = (BCM_E_NONE);
          break;
      default:
          rv = (BCM_E_PARAM);
    }
    return (rv);
}

/*
 * Function:
 *     _bcm_field_action_conf_get
 * Purpose:
 *     Get configuration structure of a field action in the given stage.
 * Parameters:
 *     unit     - (IN)  BCM device number.
 *     stage_fc - (IN)  Stage field control strucutre.
 *     action   - (IN)  Field Action.
 *     ptr      - (OUT) FP action info.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_action_config_get(int unit, _field_stage_t *stage_fc,
                             int action, _bcm_field_action_conf_t **ptr)
{

    /* Input parameters check. */
    if (NULL == stage_fc || NULL == ptr ||
        (action < 0) || (action > bcmFieldActionCount)) {
        LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META("Invalid Parameters\n")));
        return BCM_E_PARAM;
    }

    if (_BCM_FIELD_STAGE_INGRESS != stage_fc->stage_id) {
        return BCM_E_INTERNAL;
    }

    *ptr = stage_fc->f_action_arr[action];
    if (NULL == *ptr) {
        return BCM_E_UNAVAIL;
    }

    return BCM_E_NONE;
}
/*
 * Function:
 *     _bcm_field_action_offset_get
 * Purpose:
 *     Get offset info of a field action for the given stage and flags.
 * Parameters:
 *     unit     - (IN)  BCM device number.
 *     stage_fc - (IN)  Stage field control strucutre.
 *     action   - (IN)  Field Action.
 *     ptr      - (OUT) FP action offset info.
 *     flags    - (IN)  Field Actions Flags of type _BCM_FIELD_ACTION_XXX.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_action_offset_get(int unit, _field_stage_t *stage_fc,
                                      bcm_field_action_t action,
                                _bcm_field_action_offset_t *ptr,
                                                   uint32 flags)
{
    uint32 found; /* Indicates action offset existance.*/
    _bcm_field_action_conf_t   *aconf;  /* Action config structure. */
    _bcm_field_action_offset_t *offset; /* Field Action Offset Iformation. */

    /* Input parameters check. */
    if (NULL == stage_fc || NULL == ptr ||
        (action < 0) || (action > bcmFieldActionCount)) {
        LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META("Invalid Parameters\n")));
        return BCM_E_PARAM;
    }

    if (_BCM_FIELD_STAGE_INGRESS != stage_fc->stage_id) {
        return BCM_E_INTERNAL;
    }

    aconf = stage_fc->f_action_arr[action];
    if (NULL == aconf) {
        return BCM_E_UNAVAIL;
    }

    found = 0;
    offset = aconf->offset;
    do {
        if (flags == (offset->flags & flags)) {
            sal_memset(ptr, 0, sizeof(_bcm_field_action_offset_t));
            sal_memcpy(ptr, offset, sizeof(_bcm_field_action_offset_t));
            found = 1;
            break;
        }
        offset = offset->next;
    } while (NULL != offset);

    if (!found) {
        LOG_VERBOSE(BSL_LS_BCM_FP,(BSL_META_U(unit,
                "Action offset info for flags %d is not available.\n"), flags));
        return BCM_E_UNAVAIL;
    }

    return BCM_E_NONE;
}

STATIC int
_field_th_ingress_action_support_check(int unit, unsigned stage,
                                       bcm_field_action_t action)
{
    int rv;  /* Operation return status. */
    _field_stage_t *stage_fc; /* Field Stage Structure. */
    _bcm_field_action_conf_t *aconf; /* Field Action config structure. */

    rv = _field_stage_control_get(unit, stage, &stage_fc);
    BCM_IF_ERROR_RETURN(rv);

    return _bcm_field_action_config_get(unit, stage_fc, action, &aconf);
}
/*
 * Function:
 *     _field_th_stage_action_support_check
 * Purpose:
 *     Check if action is supported by tomahawk for the given stage.
 * Parameters:
 *     unit     - (IN) BCM device number.
 *     stage    - (IN) Field processor stage.
 *     action   - (IN) Action to check(bcmFieldActionXXX)
 *     result   - (OUT)
 *               TRUE   - action is supported by device
 *               FALSE  - action is NOT supported by device
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_stage_action_support_check(int unit,
                                     unsigned stage,
                                     bcm_field_action_t action,
                                     int *result)
{

    if (NULL == result) {
        return BCM_E_PARAM;
    }

    if (_BCM_FIELD_STAGE_INGRESS == stage) {

        if (BCM_SUCCESS(_field_th_ingress_action_support_check(unit, stage, action))) {
            *result = TRUE;
        } else {
            *result = FALSE;
        }

        return BCM_E_NONE;
    }

    switch (action) {
        case bcmFieldActionHiGigDstModuleGportNew:
        case bcmFieldActionHiGigDstPortGportNew:
        case bcmFieldActionHiGigDstGportNew:
        case bcmFieldActionGpHiGigDropPrecedenceNew:
        case bcmFieldActionYpHiGigDropPrecedenceNew:
        case bcmFieldActionRpHiGigDropPrecedenceNew:
        case bcmFieldActionHiGigDropPrecedenceNew:
        case bcmFieldActionGpHiGigIntPriNew:
        case bcmFieldActionYpHiGigIntPriNew:
        case bcmFieldActionRpHiGigIntPriNew:
        case bcmFieldActionHiGigIntPriNew:
        case bcmFieldActionLoopbackSrcModuleGportNew:
        case bcmFieldActionLoopbackSrcPortGportNew:
        case bcmFieldActionLoopbackSrcGportNew:
        case bcmFieldActionLoopbackCpuMasqueradePktProfileNew:
        case bcmFieldActionLoopbackPacketProcessingPortNew:
        case bcmFieldActionLoopBackTypeNew:
            *result = (stage == _BCM_FIELD_STAGE_EGRESS);
            return (BCM_E_NONE);
        default:
        ;
    }

    return (_bcm_field_td2_stage_action_support_check(unit, stage,
                                                      action, result));
}

/*
 * Function:
 *     _bcm_field_th_stage_action_support_check
 * Purpose:
 *     Check if action is supported by tomahawk for the given stage.
 * Parameters:
 *     unit     - (IN) BCM device number.
 *     stage    - (IN) Field processor stage.
 *     action   - (IN) Action to check(bcmFieldActionXXX)
 *     result   - (OUT)
 *               TRUE   - action is supported by device
 *               FALSE  - action is NOT supported by device
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_th_stage_action_support_check(int unit,
                                         unsigned stage,
                                         bcm_field_action_t action,
                                         int *result)
{
    return _field_th_stage_action_support_check(unit, stage, action, result);
}

/*
 * Function:
 *     _bcm_field_th_action_support_check
 * Purpose:
 *     Check if action is supported by tomahawk for the given stage.
 * Parameters:
 *     unit     - (IN) BCM device number.
 *     f_ent    - (IN) Field entry structure to get policy info from.
 *     action   - (IN) Action to check(bcmFieldActionXXX)
 *     result   - (OUT)
 *               TRUE   - action is supported by device
 *               FALSE  - action is NOT supported by device
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_th_action_support_check(int unit,
                                   _field_entry_t *f_ent,
                                   bcm_field_action_t action,
                                   int *result)
{
    return _bcm_field_th_stage_action_support_check(unit,
                                                    f_ent->group->stage_id,
                                                    action, result);
}

/*
 * Function:
 *     _bcm_field_th_mirror_resolve
 * Purpose:
 *     Resolve mirroring destination & enable mirroring for rule matching
 *     packets.
 * Parameters:
 *     unit     - (IN) BCM device number
 *     f_ent    - (IN) Entry structure to get policy info from
 *     fa       - (IN) field action
 *     params   - (OUT) Field Action params structure.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_th_mirror_resolve(int unit,
                             _field_entry_t *f_ent,
                             _field_action_t *fa,
                             _field_action_params_t *params)
{
    int rv;             /* Operation return status.       */
    int mtp_index;      /* Index to IM_MIRROR_CONTROL.    */
    int mtp_type_index; /* Bit position in MIRROR field.  */
    int flexibleMirror; /* Flag to indicate flexible mirroring status. */
    int directedMirror; /* Flag to indicate directed mirroring status. */

    /* Input parameters check. */
    if (NULL == f_ent || NULL == fa || NULL == params) {
        return (BCM_E_PARAM);
    }

    mtp_index = fa->hw_index;
    mtp_type_index = mtp_index;

    /* shifting the index back since flexible slot shift was done
     * prior to this
     */
    rv = bcm_esw_switch_control_get(unit,bcmSwitchFlexibleMirrorDestinations,
                                                            &flexibleMirror);
    BCM_IF_ERROR_RETURN(rv);

    rv = bcm_esw_switch_control_get(unit,bcmSwitchDirectedMirroring,
                                                   &directedMirror);
    BCM_IF_ERROR_RETURN(rv);

    if (soc_feature(unit, soc_feature_mirror_flexible)
        && flexibleMirror && directedMirror) {
        mtp_index = (fa->hw_index & BCM_MIRROR_MTP_FLEX_SLOT_MASK);
        mtp_type_index = ((fa->hw_index >> BCM_MIRROR_MTP_FLEX_SLOT_SHIFT)
                           & BCM_MIRROR_MTP_FLEX_SLOT_MASK);
    }

    /* For Tomahawk Ingress and Egress Mirror together supports 4 MTP's. */
    if (mtp_type_index >= 4) {
        fa->hw_index = _FP_INVALID_INDEX;
        return (BCM_E_INTERNAL);
    }


    params->mtp_index = mtp_index;
    params->mtp_type_index = mtp_type_index;

    LOG_VERBOSE(BSL_LS_BCM_FP,(BSL_META_U(unit, "Resolved Parameters: ")));
    LOG_VERBOSE(BSL_LS_BCM_FP,(BSL_META_U(unit,
                                  "MTP Index = %d, "), params->mtp_index));
    LOG_VERBOSE(BSL_LS_BCM_FP,(BSL_META_U(unit,
                        "MTP Type Index = %d, "), params->mtp_type_index));

    return (BCM_E_NONE);
}
/*
 * Function:
 *     _bcm_field_th_cosq_resolve
 * Purpose:
 *     Resolve field action parameters for cosq actions.
 * Parameters:
 *     unit     - (IN) BCM device number
 *     f_ent    - (IN) Field entry structure.
 *     fa       - (IN) Field action structure.
 *     params   - (OUT) Field Action params structure.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_th_cosq_resolve(int unit, _field_entry_t *f_ent,
                                       _field_action_t *fa,
                            _field_action_params_t *params)
{
    int    rv;         /* Operation return status.   */
    int    cosq_new;   /* New Cosq Value.            */
    uint32 cosq;       /* Cosq Value passed by User. */

    if (NULL == f_ent || NULL == fa || NULL == params) {
        LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META("Invalid Parameters\n")));
        return BCM_E_PARAM;
    }

    cosq = fa->param[0];
    cosq_new = -1;
    switch (fa->action) {
        case bcmFieldActionCosQNew:
        case bcmFieldActionGpCosQNew:
        case bcmFieldActionYpCosQNew:
        case bcmFieldActionRpCosQNew:
        case bcmFieldActionUcastCosQNew:
        case bcmFieldActionGpUcastCosQNew:
        case bcmFieldActionYpUcastCosQNew:
        case bcmFieldActionRpUcastCosQNew:
            if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(cosq)) {
                rv = _bcm_th_cosq_index_resolve(unit, cosq, 0,
                        _BCM_TH_COSQ_INDEX_STYLE_UCAST_QUEUE, NULL,
                        &cosq_new, NULL);
                BCM_IF_ERROR_RETURN(rv);
            } else {
                cosq_new = cosq;
            }
            break;
        case bcmFieldActionMcastCosQNew:
        case bcmFieldActionGpMcastCosQNew:
        case bcmFieldActionYpMcastCosQNew:
        case bcmFieldActionRpMcastCosQNew:
            if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(cosq)) {
                rv = _bcm_th_cosq_index_resolve(unit, cosq, 0,
                         _BCM_TH_COSQ_INDEX_STYLE_MCAST_QUEUE, NULL,
                         &cosq_new, NULL);
                BCM_IF_ERROR_RETURN(rv);
            } else {
                cosq_new = cosq;
            }
            break;
        default:
            LOG_VERBOSE(BSL_LS_BCM_FP,(BSL_META_U(unit,
                        "%s can't be resolved.\n"), action_name[fa->action]));
            return BCM_E_UNAVAIL;
    }

    if (-1 == cosq_new) {
        return BCM_E_PARAM;
    }

    params->cosq_new = cosq_new;

    LOG_VERBOSE(BSL_LS_BCM_FP,(BSL_META_U(unit,
                "Resolved Parameters:\n cosq_new = %d \n"), params->cosq_new));
    return BCM_E_NONE;
}
/*
 * Function:
 *     _bcm_field_th_fabric_tag_resolve
 * Purpose:
 *     Resolve field action parameters for action bcmFieldActionFabricQueue.
 * Parameters:
 *     unit     - (IN) BCM device number
 *     f_ent    - (IN) Field entry structure.
 *     fa       - (IN) Field action structure.
 *     params   - (OUT) Field Action params structure.
 * Returns:
 *     BCM_E_XXX
 */
int _bcm_field_th_fabric_tag_resolve(int unit, _field_entry_t *f_ent,
                                                 _field_action_t *fa,
                                      _field_action_params_t *params)
{
    uint8 fabric_tag_type; /* Fabric Tag Type. */
    uint32 in_flags;       /* Incoming Fabric Queue Flags. */
    uint32 fabric_tag;     /* Fabric Tag Value. */

    if (NULL == f_ent || NULL == fa || NULL == params) {
        LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META("Invalid Parameters\n")));
        return BCM_E_PARAM;
    }

    in_flags = fa->param[0];

    if ((in_flags & BCM_FABRIC_QUEUE_QOS_BASE)  &&
        (in_flags & BCM_FABRIC_QUEUE_DEST_OFFSET)) {
        return (BCM_E_PARAM);
    }

    if (in_flags &
         ~(BCM_FABRIC_QUEUE_CUSTOMER |
           BCM_FABRIC_QUEUE_QOS_BASE |
           BCM_FABRIC_QUEUE_DEST_OFFSET |
           0xffff)) {
        return (BCM_E_PARAM);
    }

    /* Tag type resolution. */
    if (in_flags & BCM_FABRIC_QUEUE_QOS_BASE) {
        fabric_tag_type = 0x3;  /* Offset to base queue number from the
                                   QUEUE_MAP Table. Index into QUEUE_MAP
                                   Table is {DST_MODID, DST_PID} */
    } else if (in_flags & BCM_FABRIC_QUEUE_DEST_OFFSET) {
        fabric_tag_type = 0x2;  /* Index into QUEUE_MAP Table used
                                   for lookup.*/
    } else {
        fabric_tag_type = 0x1;  /* Explicit queue number. */
    }

    fabric_tag = in_flags & 0xffff;

    params->fabric_tag_type = fabric_tag_type;
    params->fabric_tag = fabric_tag;

    LOG_VERBOSE(BSL_LS_BCM_FP,(BSL_META_U(unit, "Resolved Parameters:\n")));
    LOG_VERBOSE(BSL_LS_BCM_FP,(BSL_META_U(unit,
                               "fabric_tag_type = %d \n fabric_tag = %d\n"),
                               params->fabric_tag_type, params->fabric_tag));
    return BCM_E_NONE;
}
/*
 * Function:
 *     _bcm_field_th_i2e_class_resolve
 * Purpose:
 *     Resolve field action parameters for action
 *     bcmFieldActionEgressClassSelect.
 * Parameters:
 *     unit     - (IN) BCM device number
 *     f_ent    - (IN) Field entry structure.
 *     fa       - (IN) Field action structure.
 *     params   - (OUT) Field Action params structure.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_th_i2e_class_resolve(int unit, _field_entry_t *f_ent,
                                            _field_action_t *fa,
                                 _field_action_params_t *params)
{
    uint32 i2e_cl_sel; /* Ingress to Egress Class Selector. */

    if (NULL == f_ent || NULL == fa || NULL == params) {
        LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META("Invalid Parameters\n")));
        return BCM_E_PARAM;
    }

    switch (fa->param[0]) {
        case BCM_FIELD_EGRESS_CLASS_SELECT_PORT:
            i2e_cl_sel = 0x1;
            break;
        case BCM_FIELD_EGRESS_CLASS_SELECT_SVP:
            i2e_cl_sel = 0x2;
            break;
        case BCM_FIELD_EGRESS_CLASS_SELECT_L3_IIF:
            i2e_cl_sel = 0x3;
            break;
        case BCM_FIELD_EGRESS_CLASS_SELECT_FIELD_SRC:
            i2e_cl_sel = 0x4; /* VFP hi */
            break;
        case BCM_FIELD_EGRESS_CLASS_SELECT_FIELD_DST:
            i2e_cl_sel = 0x5; /* VFP lo */
            break;
        case BCM_FIELD_EGRESS_CLASS_SELECT_L2_SRC:
            i2e_cl_sel = 0x6;
            break;
        case BCM_FIELD_EGRESS_CLASS_SELECT_L2_DST:
            i2e_cl_sel = 0x7;
            break;
        case BCM_FIELD_EGRESS_CLASS_SELECT_L3_SRC:
            i2e_cl_sel = 0x8;
            break;
        case BCM_FIELD_EGRESS_CLASS_SELECT_L3_DST:
            i2e_cl_sel = 0x9;
            break;
        case BCM_FIELD_EGRESS_CLASS_SELECT_VLAN:
            i2e_cl_sel = 0xa;
            break;
        case BCM_FIELD_EGRESS_CLASS_SELECT_VRF:
            i2e_cl_sel = 0xb;
            break;
        case BCM_FIELD_EGRESS_CLASS_SELECT_NEW:
            i2e_cl_sel = 0xf;
            break;
        default:
            LOG_VERBOSE(BSL_LS_BCM_FP,(BSL_META_U(unit,
                        "Invalid i2e class selector: %d.\n"), fa->param[0]));
            return (BCM_E_PARAM);
    }

    params->i2e_cl_sel = i2e_cl_sel;
    LOG_VERBOSE(BSL_LS_BCM_FP,(BSL_META_U(unit,
             "Resolved Parameters:\n i2e_cl_sel = %d \n"), params->i2e_cl_sel));
    return BCM_E_NONE;
}
/*
 * Function:
 *     _bcm_field_th_i2e_hg_class_resolve
 * Purpose:
 *     Resolve field action parameters for action bcmFieldActionHiGigClassSelect
 * Parameters:
 *     unit     - (IN) BCM device number
 *     f_ent    - (IN) Field entry structure.
 *     fa       - (IN) Field action structure.
 *     params   - (OUT) Field Action params structure.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_th_i2e_hg_class_resolve(int unit, _field_entry_t *f_ent,
                                               _field_action_t *fa,
                                    _field_action_params_t *params)
{
    uint32 i2e_hg_cl_sel; /* Ingress to Egress HiGiG Class selector. */

    if (NULL == f_ent || NULL == fa || NULL == params) {
        LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META("Invalid Parameters\n")));
        return BCM_E_PARAM;
    }

    switch (fa->param[0]) {
        case BCM_FIELD_HIGIG_CLASS_SELECT_EGRESS:
            i2e_hg_cl_sel = 1;
            break;
        case BCM_FIELD_HIGIG_CLASS_SELECT_PORT:
            i2e_hg_cl_sel = 4;
            break;
        default:
            /* Invalid parameter should have been caught earlier */
            LOG_VERBOSE(BSL_LS_BCM_FP,(BSL_META_U(unit,
                     "Invalid i2e higig class selector: %d.\n"), fa->param[0]));
            return (BCM_E_PARAM);
    }

    params->i2e_hg_cl_sel = i2e_hg_cl_sel;
    LOG_VERBOSE(BSL_LS_BCM_FP,(BSL_META_U(unit, "Resolved Parameters:\n")));
    LOG_VERBOSE(BSL_LS_BCM_FP,(BSL_META_U(unit,
                              "i2e_hg_cl_sel = %d \n"), params->i2e_hg_cl_sel));
    return BCM_E_NONE;
}
#ifdef INCLUDE_L3
/*
 * Function:
 *     _bcm_field_th_nat_resolve
 * Purpose:
 *     Resolve field action parameters for actions in IFP_NAT_SET.
 * Parameters:
 *     unit     - (IN) BCM device number
 *     f_ent    - (IN) Field entry structure.
 *     fa       - (IN) Field action structure.
 *     params   - (OUT) Field Action params structure.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_th_nat_resolve(int unit, _field_entry_t *f_ent,
                                      _field_action_t *fa,
                           _field_action_params_t *params)
{
    int32 hw_idx;
    int32 hw_half;

    if (NULL == f_ent || NULL == fa || NULL == params) {
        LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META("Invalid Parameters\n")));
        return BCM_E_PARAM;
    }

    hw_idx = hw_half = 0;
    BCM_L3_NAT_EGRESS_HW_IDX_GET(fa->param[0], hw_idx, hw_half);
    if ((hw_idx < 0) || (hw_idx > soc_mem_index_max(unit,
                                  EGR_NAT_PACKET_EDIT_INFOm))) {
       LOG_VERBOSE(BSL_LS_BCM_FP,(BSL_META_U(unit, "Invalid Parameters.\n")));
       return BCM_E_PARAM;
    }

    params->hw_idx = hw_idx;
    params->hw_half = hw_half;

    LOG_VERBOSE(BSL_LS_BCM_FP,(BSL_META_U(unit, "Resolved Parameters:\n")));
    LOG_VERBOSE(BSL_LS_BCM_FP,(BSL_META_U(unit,
                                          "hw_idx = %d \n hw_half = %d \n"),
                                           params->hw_idx, params->hw_half));
    return BCM_E_NONE;

}
#endif
/*
 * Function:
 *     _bcm_field_th_redirect_resolve
 * Purpose:
 *     Resolve field action parameters for actions in IFP_REDIRECT_SET.
 * Parameters:
 *     unit     - (IN) BCM device number
 *     f_ent    - (IN) Field entry structure.
 *     fa       - (IN) Field action structure.
 *     params   - (OUT) Field Action params structure.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_th_redirect_resolve(int unit, _field_entry_t *f_ent,
                                           _field_action_t *fa,
                                _field_action_params_t *params)
{
    int    nh_ecmp_id;   /* Next Hop or ECMP Id. */
    uint8  is_dvp;       /* Redirect to DVP. */
    uint8  is_trunk;     /* Redirect to Trunk. */
    uint8  ucast_redirect_control; /* Unicast Redirection Control. */
    uint8  mcast_redirect_control; /* Multicast Redirection Control. */
    uint32 flags;                  /* Field Action Flags. */
    uint32 redir_field;            /* Dest object to redirect the packet. */

    if (NULL == fa || NULL == f_ent || NULL == params) {
        LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META("Invalid Parameters\n")));
        return (BCM_E_PARAM);
    }

    nh_ecmp_id = -1;
    is_dvp = 0;
    is_trunk = 0;
    ucast_redirect_control = 0;
    mcast_redirect_control = 0;
    flags = 0;
    redir_field = 0;

    switch (fa->action) {
        case bcmFieldActionRedirect: /* param0 = modid, param1 = port*/
            ucast_redirect_control = 0x6;
            if (BCM_GPORT_IS_MPLS_PORT(fa->param[1])) {
                redir_field = BCM_GPORT_MPLS_PORT_ID_GET((int)fa->param[1]);
                is_dvp = 1;
            } else if (BCM_GPORT_IS_MIM_PORT(fa->param[1])) {
                redir_field = BCM_GPORT_MIM_PORT_ID_GET((int)fa->param[1]);
                is_dvp = 1;
            } else if (BCM_GPORT_IS_VXLAN_PORT(fa->param[1])) {
                redir_field = BCM_GPORT_VXLAN_PORT_ID_GET((int)fa->param[1]);
                is_dvp = 1;
            } else if (BCM_GPORT_IS_TRILL_PORT(fa->param[1])) {
                redir_field = BCM_GPORT_TRILL_PORT_ID_GET((int)fa->param[1]);
                is_dvp = 1;
            } else if (BCM_GPORT_IS_L2GRE_PORT(fa->param[1])) {
                redir_field = BCM_GPORT_L2GRE_PORT_ID_GET((int)fa->param[1]);
                is_dvp = 1;
            } else if (BCM_GPORT_IS_NIV_PORT(fa->param[1])) {
                redir_field = BCM_GPORT_NIV_PORT_ID_GET((int)fa->param[1]);
                is_dvp = 1;
            } else {
                redir_field = ((fa->param[0] & 0xff) << 8);
                redir_field |= (fa->param[1] & 0xff);
                ucast_redirect_control = 0x0;
                is_trunk = 0;
            }
            break;
        case bcmFieldActionRedirectTrunk: /* param0 = trunk ID */
            redir_field |= fa->param[0];
            ucast_redirect_control = 0;
            is_trunk = 1;
            break;
        case bcmFieldActionRedirectCancel:
            break;
        case bcmFieldActionRedirectPbmp:
            redir_field = fa->hw_index;
            mcast_redirect_control = 0x0;
            break;
        case bcmFieldActionEgressMask:
            redir_field = fa->hw_index;
            break;
        case bcmFieldActionEgressPortsAdd:
            redir_field = fa->hw_index;
            break;
#ifdef INCLUDE_L3
        case bcmFieldActionRedirectIpmc:
            if (soc_feature(unit, soc_feature_field_action_redirect_ipmc)) {
                if (_BCM_MULTICAST_IS_SET(fa->param[0])) {
                    if ((0 == _BCM_MULTICAST_IS_L3(fa->param[0])) &&
                        (0 == _BCM_MULTICAST_IS_WLAN(fa->param[0]))) {
                        return (BCM_E_PARAM);
                    }
                    redir_field = _BCM_MULTICAST_ID_GET(fa->param[0]);
                } else {
                    redir_field = fa->param[0];
                }
            } else {
                redir_field = fa->hw_index;
            }
            mcast_redirect_control = 0x3;
            break;
        case bcmFieldActionRedirectEgrNextHop:
            /* Get next hop info from Egress Object ID param */
            BCM_IF_ERROR_RETURN(_bcm_field_policy_set_l3_nh_resolve(unit,
                                      fa->param[0], &flags, &nh_ecmp_id));

            if (flags & BCM_L3_MULTIPATH) {
                /* Param0 - ECMP next hop */
                if (0 == soc_feature(unit,
                                   soc_feature_field_action_redirect_ecmp)) {
                    return (BCM_E_PARAM);
                }
                ucast_redirect_control = 0x3;
            } else {
                ucast_redirect_control = 0x2;
            }
            break;
#endif /* INCLUDE_L3 */
        case bcmFieldActionRedirectMcast:
            if (soc_feature(unit, soc_feature_field_action_redirect_ipmc)) {
                if (_BCM_MULTICAST_IS_SET(fa->param[0])) {
                    if (0 == _BCM_MULTICAST_IS_L2(fa->param[0])) {
                        return (BCM_E_PARAM);
                    }
                    redir_field = _BCM_MULTICAST_ID_GET(fa->param[0]);
                } else {
                    redir_field = fa->param[0];
                }
            } else {
                redir_field = fa->hw_index;
            }
            mcast_redirect_control = 0x2;
            break;
        case bcmFieldActionRedirectVlan:
            mcast_redirect_control = 0x1;
            break;
        case bcmFieldActionRedirectBcastPbmp:
            redir_field = fa->hw_index;
            mcast_redirect_control = 0x1;
            break;
        case bcmFieldActionUnmodifiedPacketRedirectPort:
            if (BCM_GPORT_IS_MODPORT(fa->param[0])) {
                redir_field = ((BCM_GPORT_MODPORT_MODID_GET(
                                                    fa->param[0]) & 0xff) << 7);
                redir_field |= (BCM_GPORT_MODPORT_PORT_GET(
                                                          fa->param[0]) & 0xff);
                is_trunk = 0;
            } else if (BCM_GPORT_IS_TRUNK(fa->param[0])) {
                redir_field = 0x1 << 17;
                redir_field |= BCM_GPORT_TRUNK_GET(fa->param[0]);
                is_trunk = 1;
            } else {
                redir_field = ((fa->param[0] & 0xff) << 7);
                redir_field |= (fa->param[1] & 0xff);
                is_trunk = 0;
            }
            ucast_redirect_control = 0x1;
            break;
        default:
            LOG_VERBOSE(BSL_LS_BCM_FP,(BSL_META_U(unit,
                        "%s can't be resolved.\n"), action_name[fa->action]));
            return (BCM_E_UNAVAIL);
    }

    params->flags |= flags;
    params->is_dvp = is_dvp;
    params->is_trunk = is_trunk;
    params->nh_ecmp_id = nh_ecmp_id;
    params->redirect_value = redir_field;
    params->ucast_redirect_control = ucast_redirect_control;
    params->mcast_redirect_control = mcast_redirect_control;

    LOG_VERBOSE(BSL_LS_BCM_FP,(BSL_META_U(unit, "Resolved Parameters:\n")));
    LOG_VERBOSE(BSL_LS_BCM_FP,(BSL_META_U(unit,
                                         "is_dvp = %d \n"), params->is_dvp));
    LOG_VERBOSE(BSL_LS_BCM_FP,(BSL_META_U(unit,
                                      "is_trunk = %d \n"), params->is_trunk));
    LOG_VERBOSE(BSL_LS_BCM_FP,(BSL_META_U(unit,
                                  "nh_ecmp_id = %d \n"), params->nh_ecmp_id));
    LOG_VERBOSE(BSL_LS_BCM_FP,(BSL_META_U(unit,
                          "redirect_value = %d \n"), params->redirect_value));
    LOG_VERBOSE(BSL_LS_BCM_FP,(BSL_META_U(unit,
                                            "ucast_redirect_control = %d \n"),
                                             params->ucast_redirect_control));
    LOG_VERBOSE(BSL_LS_BCM_FP,(BSL_META_U(unit,
                                            "mcast_redirect_control = %d \n"),
                                             params->mcast_redirect_control));

    return (BCM_E_NONE);
}
/*
 * Function:
 *     _bcm_field_th_l3swl2change_resolve
 * Purpose:
 *     Resolve field action parameters for actions in IFP_L3SW_L2_CHANGE_SET.
 * Parameters:
 *     unit     - (IN) BCM device number
 *     f_ent    - (IN) Field entry structure.
 *     fa       - (IN) Field action structure.
 *     params   - (OUT) Field Action params structure.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_th_l3swl2change_resolve(int unit,
                                   _field_entry_t *f_ent,
                                   _field_action_t *fa,
                                   _field_action_params_t *params)
{
    int rv;         /* Operation return status. */
    int nh_ecmp_id; /* Next Hop or ECMP Id. */
    uint32 flags;   /* Multippath Flags. */

    if (NULL == fa || NULL == f_ent || NULL == params) {
        LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META("Invalid Parameters\n")));
        return (BCM_E_PARAM);
    }

    rv = BCM_E_NONE;
    switch (fa->action)  {
#ifdef INCLUDE_L3
        case bcmFieldActionL3Switch:
            rv = _bcm_field_policy_set_l3_nh_resolve(unit, fa->param[0],
                                                   &flags, &nh_ecmp_id);
            BCM_IF_ERROR_RETURN(rv);
            break;
#endif
        default:
            LOG_VERBOSE(BSL_LS_BCM_FP,(BSL_META_U(unit,
                        "%s can't be resolved.\n"), action_name[fa->action]));
            return BCM_E_UNAVAIL;
    }

    params->flags |= flags;
    params->nh_ecmp_id = nh_ecmp_id;

    LOG_VERBOSE(BSL_LS_BCM_FP,(BSL_META_U(unit, "Resolved Parameters:\n")));
    LOG_VERBOSE(BSL_LS_BCM_FP,(BSL_META_U(unit,
                                          "flags = %d \n  nh_ecmp_id = %d \n"),
                                           params->flags, params->nh_ecmp_id));
    return rv;
}
/*
 * Function:
 *     _bcm_field_th_action_resolve
 * Purpose:
 *     Resolve field action parameters.
 * Parameters:
 *     unit     - (IN) BCM device number
 *     f_ent    - (IN) Field entry structure.
 *     fa       - (IN) Field action structure.
 *     params   - (OUT) Field Action params structure.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_action_resolve(int unit, _field_entry_t *f_ent,
                          _field_action_t *fa, _field_action_params_t *params)
{
    if (NULL == f_ent || NULL == fa || NULL == params) {
        LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META("Invalid Parameters\n")));
        return BCM_E_PARAM;
    }

    switch (fa->action) {
        /* Actions from PROFILE_SET_1 */
        case bcmFieldActionCosQNew:
        case bcmFieldActionGpCosQNew:
        case bcmFieldActionYpCosQNew:
        case bcmFieldActionRpCosQNew:
        case bcmFieldActionUcastCosQNew:
        case bcmFieldActionGpUcastCosQNew:
        case bcmFieldActionYpUcastCosQNew:
        case bcmFieldActionRpUcastCosQNew:
        case bcmFieldActionMcastCosQNew:
        case bcmFieldActionGpMcastCosQNew:
        case bcmFieldActionYpMcastCosQNew:
        case bcmFieldActionRpMcastCosQNew:
            return _bcm_field_th_cosq_resolve(unit, f_ent, fa, params);
        case bcmFieldActionEgressClassSelect:
            return _bcm_field_th_i2e_class_resolve(unit, f_ent, fa, params);
        case bcmFieldActionHiGigClassSelect:
            return _bcm_field_th_i2e_hg_class_resolve(unit, f_ent, fa, params);
        case bcmFieldActionFabricQueue:
            return _bcm_field_th_fabric_tag_resolve(unit, f_ent, fa, params);
#ifdef INCLUDE_L3
        case bcmFieldActionL3Switch:
            return _bcm_field_th_l3swl2change_resolve(unit, f_ent, fa, params);
        case bcmFieldActionNatEgressOverride:
            return _bcm_field_th_nat_resolve(unit, f_ent, fa, params);
#endif
        case bcmFieldActionRedirect:
        case bcmFieldActionRedirectTrunk:
        case bcmFieldActionUnmodifiedPacketRedirectPort:
        case bcmFieldActionRedirectMcast:
#ifdef INCLUDE_L3
        case bcmFieldActionRedirectEgrNextHop:
        case bcmFieldActionRedirectIpmc:
#endif
        case bcmFieldActionRedirectCancel:
        case bcmFieldActionRedirectPbmp:
        case bcmFieldActionRedirectVlan:
        case bcmFieldActionRedirectBcastPbmp:
        case bcmFieldActionEgressMask:
        case bcmFieldActionEgressPortsAdd:
            return _bcm_field_th_redirect_resolve(unit, f_ent, fa, params);
        case bcmFieldActionMirrorIngress:
        case bcmFieldActionMirrorEgress:
            return _bcm_field_th_mirror_resolve(unit, f_ent, fa, params);
        default:
            LOG_VERBOSE(BSL_LS_BCM_FP,(BSL_META_U(unit,
                        "%s can't be resolved.\n"), action_name[fa->action]));
            return BCM_E_UNAVAIL;
   }

   return BCM_E_NONE;
}
/*
 * Function:
 *     _bcm_field_th_ingress_action_params_check
 * Purpose:
 *     Check field action parameters.
 * Parameters:
 *     unit     - (IN) BCM device number
 *     f_ent    - (IN) Field entry structure.
 *     fa       - (IN) Field action structure.
 *     action   - (IN) Field Action of type bcmFieldActionXXX.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_th_ingress_action_params_check(int unit,
                                         _field_entry_t *f_ent,
                                         _field_action_t *fa)
{
    int rv; /* Operation return status. */
    _field_stage_t *stage_fc; /* Field Stage Structure. */
    _field_action_params_t params; /* Field Params Structutre. */
    _bcm_field_action_offset_t a_offset; /* Field Offset Structure. */
    bcm_field_action_t action;  /* Field Action of Type bcmFieldActionXXX. */


    if (NULL == f_ent ||
        NULL == fa) {
        LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META("Invalid Parameters\n")));
        return BCM_E_PARAM;
    }

    rv = _field_stage_control_get(unit, f_ent->group->stage_id, &stage_fc);
    BCM_IF_ERROR_RETURN(rv);

    sal_memset(&params , 0, sizeof(params));

    action = fa->action;
    switch (action) {
        /* Actions from PROFILE_SET_1 */
        case bcmFieldActionCosQNew:
        case bcmFieldActionGpCosQNew:
        case bcmFieldActionYpCosQNew:
        case bcmFieldActionRpCosQNew:
            action = bcmFieldActionGpCosQNew;
            ACTION_OFFSET_GET(unit, stage_fc, action, &a_offset, 0);
            ACTION_RESOLVE(unit, f_ent, fa, &params);
            ACTION_CHECK(a_offset.width[0], params.cosq_new);
            break;
        case bcmFieldActionUcastCosQNew:
        case bcmFieldActionGpUcastCosQNew:
        case bcmFieldActionYpUcastCosQNew:
        case bcmFieldActionRpUcastCosQNew:
            action = bcmFieldActionGpUcastCosQNew;
            ACTION_OFFSET_GET(unit, stage_fc, action, &a_offset, 0);
            ACTION_RESOLVE(unit, f_ent, fa, &params);
            ACTION_CHECK(a_offset.width[0], params.cosq_new);
            break;
        case bcmFieldActionMcastCosQNew:
        case bcmFieldActionGpMcastCosQNew:
        case bcmFieldActionYpMcastCosQNew:
        case bcmFieldActionRpMcastCosQNew:
            action = bcmFieldActionGpMcastCosQNew;
            ACTION_OFFSET_GET(unit, stage_fc, action, &a_offset, 0);
            ACTION_RESOLVE(unit, f_ent, fa, &params);
            ACTION_CHECK(a_offset.width[0], params.cosq_new << 4);
            break;
        case bcmFieldActionPrioIntNew:
        case bcmFieldActionGpPrioIntNew:
        case bcmFieldActionYpPrioIntNew:
        case bcmFieldActionRpPrioIntNew:
        case bcmFieldActionPrioPktAndIntNew:
        case bcmFieldActionRpPrioPktAndIntNew:
        case bcmFieldActionGpPrioPktAndIntNew:
        case bcmFieldActionYpPrioPktAndIntNew:
            action = bcmFieldActionGpPrioIntNew;
            ACTION_OFFSET_GET(unit, stage_fc, action, &a_offset, 0);
            ACTION_CHECK(a_offset.width[0], fa->param[0]);
            break;
        case bcmFieldActionDropPrecedence:
        case bcmFieldActionGpDropPrecedence:
        case bcmFieldActionYpDropPrecedence:
        case bcmFieldActionRpDropPrecedence:
            action = bcmFieldActionGpDropPrecedence;
            ACTION_OFFSET_GET(unit, stage_fc, action, &a_offset, 0);
            ACTION_CHECK(a_offset.width[0], fa->param[0]);
            break;
        case bcmFieldActionPrioIntCopy:
        case bcmFieldActionPrioIntTos:
        case bcmFieldActionPrioIntCancel:
        case bcmFieldActionGpPrioIntCopy:
        case bcmFieldActionGpPrioIntTos:
        case bcmFieldActionGpPrioIntCancel:
        case bcmFieldActionYpPrioIntCopy:
        case bcmFieldActionYpPrioIntTos:
        case bcmFieldActionYpPrioIntCancel:
        case bcmFieldActionRpPrioIntCopy:
        case bcmFieldActionRpPrioIntTos:
        case bcmFieldActionRpPrioIntCancel:
            break;
        /* Actions from PROFILE_SET_2 */
        case bcmFieldActionEcnNew:
        case bcmFieldActionGpEcnNew:
        case bcmFieldActionYpEcnNew:
        case bcmFieldActionRpEcnNew:
            action = bcmFieldActionGpEcnNew;
            ACTION_OFFSET_GET(unit, stage_fc, action, &a_offset, 0);
            ACTION_CHECK(a_offset.width[0], fa->param[0]);
            break;
        case bcmFieldActionPrioPktNew:
        case bcmFieldActionGpPrioPktNew:
        case bcmFieldActionYpPrioPktNew:
        case bcmFieldActionRpPrioPktNew:
            action = bcmFieldActionGpPrioPktNew;
            ACTION_OFFSET_GET(unit, stage_fc, action, &a_offset, 0);
            ACTION_CHECK(a_offset.width[0], fa->param[0]);
            break;
        case bcmFieldActionGpTosPrecedenceNew:
            action = bcmFieldActionGpTosPrecedenceNew;
            ACTION_OFFSET_GET(unit, stage_fc, action, &a_offset, 0);
            ACTION_CHECK(a_offset.width[0], fa->param[0]);
            break;
        case bcmFieldActionDscpNew:
        case bcmFieldActionGpDscpNew:
        case bcmFieldActionYpDscpNew:
        case bcmFieldActionRpDscpNew:
            action = bcmFieldActionGpDscpNew;
            ACTION_OFFSET_GET(unit, stage_fc, action, &a_offset, 0);
            ACTION_CHECK(a_offset.width[0], fa->param[0]);
            break;
        case bcmFieldActionPrioPktCopy:
        case bcmFieldActionGpPrioPktCopy:
        case bcmFieldActionYpPrioPktCopy:
        case bcmFieldActionRpPrioPktCopy:
        case bcmFieldActionPrioPktTos:
        case bcmFieldActionGpPrioPktTos:
        case bcmFieldActionYpPrioPktTos:
        case bcmFieldActionRpPrioPktTos:
        case bcmFieldActionPrioPktCancel:
        case bcmFieldActionGpPrioPktCancel:
        case bcmFieldActionYpPrioPktCancel:
        case bcmFieldActionRpPrioPktCancel:
        case bcmFieldActionDscpCancel:
        case bcmFieldActionGpDscpCancel:
        case bcmFieldActionYpDscpCancel:
        case bcmFieldActionRpDscpCancel:
        case bcmFieldActionGpTosPrecedenceCopy:
        case bcmFieldActionPrioPktAndIntCopy:
        case bcmFieldActionPrioPktAndIntTos:
        case bcmFieldActionPrioPktAndIntCancel:
        case bcmFieldActionRpPrioPktAndIntCopy:
        case bcmFieldActionRpPrioPktAndIntTos:
        case bcmFieldActionRpPrioPktAndIntCancel:
        case bcmFieldActionGpPrioPktAndIntCopy:
        case bcmFieldActionGpPrioPktAndIntTos:
        case bcmFieldActionGpPrioPktAndIntCancel:
        case bcmFieldActionYpPrioPktAndIntCopy:
        case bcmFieldActionYpPrioPktAndIntTos:
        case bcmFieldActionYpPrioPktAndIntCancel:
            break;
        /* Actions from L3SW_CHANGE_L2_SWT */
        case bcmFieldActionEgressClassSelect:
            ACTION_OFFSET_GET(unit, stage_fc, action, &a_offset, 0);
            ACTION_RESOLVE(unit, f_ent, fa, &params);
            ACTION_CHECK(a_offset.width[0], params.i2e_cl_sel);
            break;
        case bcmFieldActionHiGigClassSelect:
            ACTION_OFFSET_GET(unit, stage_fc, action, &a_offset, 0);
            ACTION_RESOLVE(unit, f_ent, fa, &params);
            ACTION_CHECK(a_offset.width[0], params.i2e_hg_cl_sel);
            break;
        case bcmFieldActionNewClassId:
        case bcmFieldActionMultipathHash:
        case bcmFieldActionAddClassTag:
            ACTION_OFFSET_GET(unit, stage_fc, action, &a_offset, 0);
            ACTION_CHECK(a_offset.width[0], fa->param[0]);
            break;
        case bcmFieldActionFabricQueue:
            ACTION_OFFSET_GET(unit, stage_fc, action, &a_offset, 0);
            ACTION_RESOLVE(unit, f_ent, fa, &params);
            ACTION_CHECK(a_offset.width[0], params.fabric_tag_type);
            ACTION_CHECK(a_offset.width[1], params.fabric_tag);
            break;
#ifdef INCLUDE_L3
        case bcmFieldActionL3Switch:
            ACTION_RESOLVE(unit, f_ent, fa, &params);
            if (params.flags & BCM_L3_MULTIPATH) {
                ACTION_OFFSET_GET(unit, stage_fc, action, &a_offset,
                                 _BCM_FIELD_ACTION_L3SWITCH_ECMP);
                  ACTION_CHECK(a_offset.width[0], params.nh_ecmp_id);
            } else {
                ACTION_OFFSET_GET(unit, stage_fc, action, &a_offset,
                             _BCM_FIELD_ACTION_L3SWITCH_NEXT_HOP);
                  ACTION_CHECK(a_offset.width[0], params.nh_ecmp_id);
            }
            break;
#endif
        case bcmFieldActionDstMacNew:
        case bcmFieldActionSrcMacNew:
        case bcmFieldActionOuterVlanNew:
        case bcmFieldActionVnTagNew:
        case bcmFieldActionVnTagDelete:
        case bcmFieldActionEtagNew:
        case bcmFieldActionEtagDelete:
        case bcmFieldActionL3SwitchCancel:
            break;
        /* Actions from IFP_LB_CONTROLS_SET */
        case bcmFieldActionDynamicHgTrunkCancel:
        case bcmFieldActionTrunkLoadBalanceCancel:
        case bcmFieldActionEcmpLoadBalanceCancel:
            break;
        /* Actions from IFP_NAT_OVERRIDE_SET */
        case bcmFieldActionNatCancel:
            break;
        /* Actions from IFP_COPY_TO_CPU_SET */
        case bcmFieldActionCopyToCpu:
        case bcmFieldActionGpCopyToCpu:
        case bcmFieldActionYpCopyToCpu:
        case bcmFieldActionRpCopyToCpu:
            if (0 != fa->param[0]) {
                action = bcmFieldActionGpCopyToCpu;
                ACTION_OFFSET_GET(unit, stage_fc, action, &a_offset, 0);
                ACTION_CHECK(a_offset.width[0], fa->param[1]);
            }
            break;
        case bcmFieldActionTimeStampToCpu:
        case bcmFieldActionGpTimeStampToCpu:
        case bcmFieldActionYpTimeStampToCpu:
        case bcmFieldActionRpTimeStampToCpu:
            if (0 != fa->param[0]) {
                action = bcmFieldActionGpTimeStampToCpu;
                ACTION_OFFSET_GET(unit, stage_fc, action, &a_offset, 0);
                ACTION_CHECK(a_offset.width[0], fa->param[1]);
            }
            break;
        case bcmFieldActionCopyToCpuCancel:
        case bcmFieldActionGpCopyToCpuCancel:
        case bcmFieldActionYpCopyToCpuCancel:
        case bcmFieldActionRpCopyToCpuCancel:
        case bcmFieldActionSwitchToCpuCancel:
        case bcmFieldActionGpSwitchToCpuCancel:
        case bcmFieldActionYpSwitchToCpuCancel:
        case bcmFieldActionRpSwitchToCpuCancel:
        case bcmFieldActionSwitchToCpuReinstate:
        case bcmFieldActionGpSwitchToCpuReinstate:
        case bcmFieldActionYpSwitchToCpuReinstate:
        case bcmFieldActionRpSwitchToCpuReinstate:
        case bcmFieldActionTimeStampToCpuCancel:
        case bcmFieldActionGpTimeStampToCpuCancel:
        case bcmFieldActionYpTimeStampToCpuCancel:
        case bcmFieldActionRpTimeStampToCpuCancel:

            break;
        /* Actions from IFP_XXX_OVERRIDE_SET */
        case bcmFieldActionDoNotCheckUrpf:
        case bcmFieldActionDoNotChangeTtl:
        case bcmFieldActionMirrorOverride:
            break;
        /* Actions from IFP_CHANGE_CPU_COS_SET */
        case bcmFieldActionCosQCpuNew:
        case bcmFieldActionServicePoolIdNew:
            ACTION_OFFSET_GET(unit, stage_fc, action, &a_offset, 0);
            ACTION_CHECK(a_offset.width[0], fa->param[0]);
            break;
        /* Actions from IFP_DROP_SET */
        case bcmFieldActionDrop:
        case bcmFieldActionGpDrop:
        case bcmFieldActionYpDrop:
        case bcmFieldActionRpDrop:
        case bcmFieldActionDropCancel:
        case bcmFieldActionGpDropCancel:
        case bcmFieldActionYpDropCancel:
        case bcmFieldActionRpDropCancel:
            break;
        /* Actions from IFP_SFLOW_SET */
        case bcmFieldActionIngSampleEnable:
            break;
        /* Actions from IFP_NAT_SET */
#ifdef INCLUDE_L3
        case bcmFieldActionNatEgressOverride:
            ACTION_OFFSET_GET(unit, stage_fc, action, &a_offset, 0);
            ACTION_RESOLVE(unit, f_ent, fa, &params);
            ACTION_CHECK(a_offset.width[0], params.hw_idx);
            ACTION_CHECK(a_offset.width[1], params.hw_half);
            break;
#endif
        case bcmFieldActionNat:
            break;
        /* Actions from REDIRECT_SET */
        case bcmFieldActionRedirectTrunk:
            ACTION_OFFSET_GET(unit, stage_fc, action, &a_offset, 0);
            ACTION_RESOLVE(unit, f_ent, fa, &params);
            ACTION_CHECK(a_offset.width[0], params.redirect_value);
            break;
        case bcmFieldActionRedirect:
            rv = _bcm_field_action_dest_check(unit, fa);
            BCM_IF_ERROR_RETURN(rv);
            ACTION_RESOLVE(unit, f_ent, fa, &params);
            if (params.is_dvp) {
                ACTION_OFFSET_GET(unit, stage_fc, action, &a_offset,
                                  _BCM_FIELD_ACTION_REDIRECT_DVP);
            } else if (params.is_trunk) {
                ACTION_OFFSET_GET(unit, stage_fc, action, &a_offset,
                                 _BCM_FIELD_ACTION_REDIRECT_TRUNK);
            } else {
                ACTION_OFFSET_GET(unit, stage_fc, action, &a_offset,
                                 _BCM_FIELD_ACTION_REDIRECT_DGLP);
            }
            ACTION_CHECK(a_offset.width[0], params.redirect_value);
            break;
        case bcmFieldActionUnmodifiedPacketRedirectPort:
            ACTION_OFFSET_GET(unit, stage_fc, action, &a_offset,
                             _BCM_FIELD_ACTION_REDIRECT_DGLP);
            ACTION_RESOLVE(unit, f_ent, fa, &params);
            ACTION_CHECK(a_offset.width[0], params.redirect_value);
            break;
#ifdef INCLUDE_L3
        case bcmFieldActionRedirectEgrNextHop:
            ACTION_RESOLVE(unit, f_ent, fa, &params);
            if (params.flags & BCM_L3_MULTIPATH) {
                ACTION_OFFSET_GET(unit, stage_fc, fa->action, &a_offset,
                                      _BCM_FIELD_ACTION_REDIRECT_ECMP);
            } else {
                ACTION_OFFSET_GET(unit, stage_fc, fa->action, &a_offset,
                                  _BCM_FIELD_ACTION_REDIRECT_NEXT_HOP);
            }
            ACTION_CHECK(a_offset.width[0], params.nh_ecmp_id);
            break;
#endif
        case bcmFieldActionRedirectCancel:
        case bcmFieldActionRedirectPbmp:
        case bcmFieldActionRedirectVlan:
        case bcmFieldActionRedirectBcastPbmp:
        case bcmFieldActionRedirectIpmc:
        case bcmFieldActionRedirectMcast:
        case bcmFieldActionEgressMask:
        case bcmFieldActionEgressPortsAdd:
        case bcmFieldActionColorIndependent:
            break;
        case bcmFieldActionMirrorIngress:
        case bcmFieldActionMirrorEgress:
            rv = _bcm_field_action_dest_check(unit, fa);
            BCM_IF_ERROR_RETURN(rv);
            break;
        default:
            LOG_VERBOSE(BSL_LS_BCM_FP,(BSL_META_U(unit,
                "Unsupported Action %s.\n"), action_name[fa->action]));
            return BCM_E_UNAVAIL;
   }
   return BCM_E_NONE;
}
/*
 * Function:
 *     _bcm_field_th_action_params_check
 * Purpose:
 *     Check field action parameters.
 * Parameters:
 *     unit     - BCM device number
 *     f_ent    - Field entry structure.
 *     fa       - field action
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_th_action_params_check(int unit,
                              _field_entry_t  *f_ent,
                              _field_action_t *fa)
{
    uint8    mod_id;     /* Module Id.              */
    uint8    port_id;    /* Port Id.                */
    soc_mem_t mem;       /* Policy table memory id. */
    soc_mem_t tcam_mem;  /* Tcam memory id.         */

    if (NULL == f_ent ||
        NULL == fa) {
        LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META("Invalid Parameters\n")));
        return BCM_E_PARAM;
    }

    if (_BCM_FIELD_STAGE_EXTERNAL == f_ent->group->stage_id) {
        return BCM_E_INTERNAL;
    }

    BCM_IF_ERROR_RETURN
         (_bcm_field_th_tcam_policy_mem_get(unit, f_ent, &tcam_mem, &mem));

    switch (f_ent->group->stage_id) {
        case _BCM_FIELD_STAGE_EGRESS:
            switch (fa->action) {
                case bcmFieldActionHiGigDstModuleGportNew:
                    if (BCM_GPORT_IS_MODPORT(fa->param[0])) {
                        mod_id = BCM_GPORT_MODPORT_MODID_GET(fa->param[0]);
                    } else {
                        return BCM_E_PARAM;
                    }
                    PolicyCheck(unit, mem, HG_DST_MODID_NEWf, mod_id);
                    return (BCM_E_NONE);
                case bcmFieldActionHiGigDstPortGportNew:
                    if (BCM_GPORT_IS_MODPORT(fa->param[0])) {
                        port_id = BCM_GPORT_MODPORT_PORT_GET(fa->param[0]);
                    } else {
                        return BCM_E_PARAM;
                    }
                    PolicyCheck(unit, mem, HG_DST_PORT_NEWf, port_id);
                    return (BCM_E_NONE);
                case bcmFieldActionHiGigDstGportNew:
                    if (BCM_GPORT_IS_MODPORT(fa->param[0])) {
                        mod_id = BCM_GPORT_MODPORT_MODID_GET(fa->param[0]);
                        port_id = BCM_GPORT_MODPORT_PORT_GET(fa->param[0]);
                    } else {
                        return BCM_E_PARAM;
                    }
                    PolicyCheck(unit, mem, HG_DST_MODID_NEWf, mod_id);
                    PolicyCheck(unit, mem, HG_DST_PORT_NEWf, port_id);
                    return (BCM_E_NONE);
                case bcmFieldActionGpHiGigDropPrecedenceNew:
                    PolicyCheck(unit, mem, G_HG_DP_NEWf, fa->param[0]);
                    return (BCM_E_NONE);
                case bcmFieldActionYpHiGigDropPrecedenceNew:
                    PolicyCheck(unit, mem, Y_HG_DP_NEWf, fa->param[0]);
                    return (BCM_E_NONE);
                case bcmFieldActionRpHiGigDropPrecedenceNew:
                    PolicyCheck(unit, mem, R_HG_DP_NEWf, fa->param[0]);
                    return (BCM_E_NONE);
                case bcmFieldActionHiGigDropPrecedenceNew:
                    PolicyCheck(unit, mem, G_HG_DP_NEWf, fa->param[0]);
                    PolicyCheck(unit, mem, Y_HG_DP_NEWf, fa->param[0]);
                    PolicyCheck(unit, mem, R_HG_DP_NEWf, fa->param[0]);
                    return (BCM_E_NONE);
                case bcmFieldActionGpHiGigIntPriNew:
                    PolicyCheck(unit, mem, G_HG_INT_PRI_NEWf , fa->param[0]);
                    return (BCM_E_NONE);
                case bcmFieldActionYpHiGigIntPriNew:
                    PolicyCheck(unit, mem, Y_HG_INT_PRI_NEWf , fa->param[0]);
                    return (BCM_E_NONE);
                case bcmFieldActionRpHiGigIntPriNew:
                    PolicyCheck(unit, mem, R_HG_INT_PRI_NEWf , fa->param[0]);
                    return (BCM_E_NONE);
                case bcmFieldActionHiGigIntPriNew:
                    PolicyCheck(unit, mem, G_HG_INT_PRI_NEWf , fa->param[0]);
                    PolicyCheck(unit, mem, Y_HG_INT_PRI_NEWf , fa->param[0]);
                    PolicyCheck(unit, mem, R_HG_INT_PRI_NEWf , fa->param[0]);
                    return (BCM_E_NONE);
                case bcmFieldActionLoopbackSrcModuleGportNew:
                    if (BCM_GPORT_IS_MODPORT(fa->param[0])) {
                        mod_id = BCM_GPORT_MODPORT_MODID_GET(fa->param[0]);
                    } else {
                        return BCM_E_PARAM;
                    }
                    PolicyCheck(unit, mem, LB_SRC_MODID_NEWf, mod_id);
                    return (BCM_E_NONE);
                case bcmFieldActionLoopbackSrcPortGportNew:
                    if (BCM_GPORT_IS_MODPORT(fa->param[0])) {
                        port_id = BCM_GPORT_MODPORT_PORT_GET(fa->param[0]);
                    } else {
                        return BCM_E_PARAM;
                    }
                    PolicyCheck(unit, mem, LB_SRC_PORT_NEWf, port_id);
                    return (BCM_E_NONE);
                case bcmFieldActionLoopbackSrcGportNew:
                    if (BCM_GPORT_IS_MODPORT(fa->param[0])) {
                        mod_id = BCM_GPORT_MODPORT_MODID_GET(fa->param[0]);
                        port_id = BCM_GPORT_MODPORT_PORT_GET(fa->param[0]);
                    } else {
                        return BCM_E_PARAM;
                    }
                    PolicyCheck(unit, mem, LB_SRC_MODID_NEWf, mod_id);
                    PolicyCheck(unit, mem, LB_SRC_PORT_NEWf, port_id);
                    return (BCM_E_NONE);
                case bcmFieldActionLoopbackCpuMasqueradePktProfileNew:
                    PolicyCheck(unit, mem,
                                LB_PACKET_PROFILE_NEWf,
                                fa->param[0]);
                    return (BCM_E_NONE);
                case bcmFieldActionLoopbackPacketProcessingPortNew:
                    PolicyCheck(unit, mem, LB_PP_PORT_NEWf , fa->param[0]);
                    return (BCM_E_NONE);
                case bcmFieldActionLoopBackTypeNew:
                    PolicyCheck(unit, mem, LB_TYPE_NEWf , fa->param[0]);
                    return (BCM_E_NONE);
                default:
                    ;
            }
            break;
        case _BCM_FIELD_STAGE_INGRESS:
            return _bcm_field_th_ingress_action_params_check(unit, f_ent, fa);
        default:
            ;
    }

    return _bcm_field_td2_action_params_check(unit, f_ent, fa);
}
/*
 * Function:
 *     _bcm_field_th_val_set
 * Purpose:
 *     Write the width size of data from p_data to p_fn_data at offset.
 * Parameters:
 *     p_fn_data      - (OUT) Field Policy table entry
 *     p_data         - (IN) Data to be written into p_fn_data
 *     offset         - (IN) Offset at which data has to be copied to p_fn_data
 *     width          - (IN) Size of the data to be written into p_fn_data
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_th_val_set(uint32 *p_fn_data, uint32 *p_data,
                            uint32 offset, uint32 width)
{
    uint32 u32_mask;
    int idx, wp, bp, len;

    if (NULL == p_fn_data || NULL == p_data) {
        LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META("Invalid Parameters\n")));
        return BCM_E_PARAM;
    }

    wp = offset / 32;
    bp = offset & (32 - 1);
    idx = 0;

    for (len = width; len > 0; len -= 32) {
        if (bp) {
            if (len < 32) {
                u32_mask = (1 << len) - 1;
                if ((p_data[idx] & ~u32_mask) != 0) {
                    return (BCM_E_PARAM);
                }
            } else {
                u32_mask = 0xffffffff;
            }

            p_fn_data[wp] &= ~(u32_mask << bp);
            p_fn_data[wp++] |= p_data[idx] << bp;
            p_fn_data[wp] &= ~(u32_mask >> (32 - bp));
            p_fn_data[wp] |= p_data[idx] >> (32 - bp) & ((1 << bp) - 1);

        } else {
            if (len < 32) {
                u32_mask = (1 << len) - 1;
                if ((p_data[idx] & ~u32_mask) != 0) {
                    return (BCM_E_PARAM);
                }
                p_fn_data[wp] &= ~u32_mask;
                p_fn_data[wp++] |= p_data[idx];
            } else {
                p_fn_data[wp++] = p_data[idx];
            }
        }

        idx++;
    }

    return BCM_E_NONE;
}
/*
 * Function:
 *     _bcm_field_th_val_get
 * Purpose:
 *     Write the width size of data to p_data from p_fn_data at offset.
 * Parameters:
 *     p_fn_data      - (IN)  Field Policy table entry
 *     p_data         - (OUT) Data to be from p_fn_data
 *     offset         - (IN)  Offset at which data has to be
 *                            copied from p_fn_data
 *     width          - (IN)  Size of the data to be written into p_data
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_th_val_get(uint32 *p_fn_data, uint32 *p_data,
                            uint32 offset, uint32 width)
{
    int                 idx, wp, bp, len;

    /* Input parameters check. */
    if (NULL == p_fn_data || NULL == p_data) {
        LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META("Invalid Parameters\n")));
        return (BCM_E_PARAM);
    }

    wp = offset / 32;
    bp = offset & (32 - 1);
    idx = 0;

    for (len = width; len > 0; len -= 32) {
        if (bp) {
            p_data[idx] = (p_fn_data[wp++] >> bp) & ((1 << (32 - bp)) - 1);
            p_data[idx] |= p_fn_data[wp] << (32 - bp);
        } else {
            p_data[idx] = p_fn_data[wp++];
        }

        if (len < 32) {
            p_data[idx] &= ((1 << len) - 1);
        }

        idx++;
    }

    return (BCM_E_NONE);
}
/*
 * Function:
 *     _bcm_field_action_val_set
 * Purpose:
 *     Get the actions to be written
 * Parameters:
 *     unit     - BCM device number
 *     mem      - Policy table memory
 *     fa       - field action
 *     buf      - (OUT) Field Policy table entry
 * Returns:
 *     BCM_E_NONE
 *     BCM_E_PARAM - Action parameter out-of-range or unrecognized action.
 */
int
_bcm_field_action_val_set(int unit, _field_entry_t *f_ent, uint32 *entbuf,
                                     _bcm_field_action_offset_t *a_offset)
{
    int rv;
    uint32 idx;
    uint32 offset;
    uint32 width;
    uint32 value;

    if (NULL == entbuf || NULL == f_ent || NULL == a_offset) {
        LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META("Invalid Parameters\n")));
        return (BCM_E_PARAM);
    }

    for (idx = 0; idx < _FP_ACTION_PARAM_SZ; idx++) {
        if (a_offset->width[idx] > 0 ) {
            offset = a_offset->offset[idx];
            width  = a_offset->width[idx];
            value  = a_offset->value[idx];
            /*
             * COVERITY
             *
             * This flow takes care of the  Out-of-bounds access issue
             * for data and mask.
             */
            /* coverity[callee_ptr_arith : FALSE] */
            rv = _bcm_field_th_val_set(entbuf, &value, offset, width);
            BCM_IF_ERROR_RETURN(rv);
        }
    }

    return BCM_E_NONE;
}
/*
 * Function:
 *     _bcm_field_action_val_get
 * Purpose:
 *     Get the actions written into buffer
 * Parameters:
 *     unit     - (IN) BCM device number
 *     f_ent    - (IN) Field Entry
 *     buf      - (IN) Field Policy table entry
 *     action   - (IN) Field Action of type bcmFieldActionXXX
 *     flags    - (IN) flags to inform offset configuration
 *     a_offset - (OUT) Action offset information
 * Returns:
 *     BCM_E_NONE
 *     BCM_E_PARAM - Action parameter out-of-range or unrecognized action.
 */
int
_bcm_field_action_val_get(int unit,
                          _field_entry_t *f_ent,
                          uint32 *entbuf,
                          bcm_field_action_t action,
                          uint32 flags,
                          _bcm_field_action_offset_t *a_offset)
{
    int rv;
    uint32 idx;
    uint32 value;
    _field_stage_t *stage_fc;

    if (NULL == entbuf || NULL == a_offset || NULL == f_ent) {
        LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META("Invalid Parameters\n")));
        return (BCM_E_PARAM);
    }

    rv = _field_stage_control_get(unit, f_ent->group->stage_id, &stage_fc);
    BCM_IF_ERROR_RETURN(rv);

    rv = _bcm_field_action_offset_get(unit, stage_fc, action, a_offset, flags);
    BCM_IF_ERROR_RETURN(rv);


    for (idx = 0; idx < _FP_ACTION_PARAM_SZ; idx++) {
        if (a_offset->width[idx] > 0 ) {
            value = 0;
            rv = _bcm_field_th_val_get(entbuf, &value,
                                       a_offset->offset[idx],
                                       a_offset->width[idx]);
            BCM_IF_ERROR_RETURN(rv);
            a_offset->value[idx]  = value;
        }
    }

    return BCM_E_NONE;
}
/*
 * Function:
 *     _bcm_field_th_profile1_cd_action_set
 * Purpose:
 *     Set the color dependent actions of IFP_PROFILE_ONE_SET
 *     in entbuf which is to be written into POLICY table.
 * Parameters:
 *     unit     - BCM device number
 *     action   - Field Action of type bcmFieldActionXXX
 *     f_ent    - entry structure to get policy info from
 *     fa       - field action
 *     entbuf   - (OUT) Field Policy table entry
 * Returns:
 *     BCM_E_NONE
 *     BCM_E_PARAM - Action parameter out-of-range or unrecognized action.
 * Notes:
 *     This is a simple read/modify/write pattern.
 *     FP unit lock should be held by calling function.
 */
int
_bcm_field_th_profile1_cd_action_set(int unit,
                                     bcm_field_action_t action,
                                     _field_entry_t *f_ent,
                                     _field_action_t *fa, uint32 *entbuf)

{
    int rv;
    int ucosq = BCM_COS_INVALID;            /* Current unicast queue value */
    int mcosq = BCM_COS_INVALID;            /* Current mcast queue value */
    uint8 mcast_mode_set = 0;               /* Multicast action is set */
    uint8 ucast_mode_set = 0;               /* Unicast action is set */
    _field_stage_t *stage_fc;
    _field_action_t *f_ent_act;
    _field_action_params_t params;
    _bcm_field_action_offset_t a_offset;

    if (NULL == f_ent || NULL == fa || NULL == entbuf) {
        LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META("Invalid Parameters\n")));
        return BCM_E_PARAM;
    }

    rv = _field_stage_control_get(unit, f_ent->group->stage_id, &stage_fc);
    BCM_IF_ERROR_RETURN(rv);

    rv = _bcm_field_action_offset_get(unit, stage_fc, action, &a_offset, 0);
    BCM_IF_ERROR_RETURN(rv);

    sal_memset(&params, 0, sizeof(params));

    switch (action) {
        case bcmFieldActionGpCosQNew:
        case bcmFieldActionYpCosQNew:
        case bcmFieldActionRpCosQNew:
            ACTION_RESOLVE(unit, f_ent, fa, &params);
            a_offset.value[0] = params.cosq_new;
            ACTION_SET(unit, f_ent, entbuf, &a_offset);
            break;
        case bcmFieldActionGpPrioIntCopy:
        case bcmFieldActionYpPrioIntCopy:
        case bcmFieldActionRpPrioIntCopy:
        case bcmFieldActionGpPrioIntTos:
        case bcmFieldActionYpPrioIntTos:
        case bcmFieldActionRpPrioIntTos:
        case bcmFieldActionGpPrioIntCancel:
        case bcmFieldActionYpPrioIntCancel:
        case bcmFieldActionRpPrioIntCancel:
            ACTION_SET(unit, f_ent, entbuf, &a_offset);
            break;
        case bcmFieldActionGpPrioIntNew:
        case bcmFieldActionYpPrioIntNew:
        case bcmFieldActionRpPrioIntNew:
            a_offset.value[0] = fa->param[0];
            ACTION_SET(unit, f_ent, entbuf, &a_offset);
            break;
        case bcmFieldActionGpUcastCosQNew:
            ACTION_RESOLVE(unit, f_ent, fa, &params);
            ucosq = params.cosq_new;
            mcast_mode_set = 0;
            /* Check if corresponding multicast action is set for this entry */
            for (f_ent_act = f_ent->actions;
                 f_ent_act != NULL;
                 f_ent_act = f_ent_act->next) {
                if (bcmFieldActionGpMcastCosQNew == f_ent_act->action ||
                    bcmFieldActionMcastCosQNew == f_ent_act->action) {
                    sal_memset(&params, 0, sizeof(params));
                    /* Get current Mcast queue value */
                    mcast_mode_set = 1;
                    ACTION_RESOLVE(unit, f_ent, f_ent_act, &params);
                    mcosq = params.cosq_new;
                    break;
                }
            }
            if (0 == mcast_mode_set) {
                a_offset.value[0] = ucosq;
                ACTION_SET(unit, f_ent, entbuf, &a_offset);
            } else {
                /* Set Ucast and Mcast queue modes */
                a_offset.value[0] = ucosq;
                ACTION_SET(unit, f_ent, entbuf, &a_offset);

                action = bcmFieldActionGpMcastCosQNew;
                ACTION_OFFSET_GET(unit, stage_fc, action, &a_offset, 0);

                a_offset.value[0] = mcosq;
                ACTION_SET(unit, f_ent, entbuf, &a_offset);
            }
            break;
        case bcmFieldActionYpUcastCosQNew:
            ACTION_RESOLVE(unit, f_ent, fa, &params);
            ucosq = params.cosq_new;
            mcast_mode_set = 0;
            /* Check if corresponding multicast action is set for this entry */
            for (f_ent_act = f_ent->actions;
                 f_ent_act != NULL;
                 f_ent_act = f_ent_act->next) {
                if (bcmFieldActionYpMcastCosQNew == f_ent_act->action ||
                    bcmFieldActionMcastCosQNew == f_ent_act->action) {
                    sal_memset(&params, 0, sizeof(params));
                    /* Get current Mcast queue value */
                    mcast_mode_set = 1;
                    ACTION_RESOLVE(unit, f_ent, f_ent_act, &params);
                    mcosq = params.cosq_new;
                    break;
                }
            }
            if (0 == mcast_mode_set) {
                a_offset.value[0] = ucosq;
                ACTION_SET(unit, f_ent, entbuf, &a_offset);
            } else {
                /* Set Ucast and Mcast queue modes */
                a_offset.value[0] = ucosq;
                ACTION_SET(unit, f_ent, entbuf, &a_offset);

                action = bcmFieldActionYpMcastCosQNew;
                ACTION_OFFSET_GET(unit, stage_fc, action, &a_offset, 0);

                a_offset.value[0] = mcosq;
                ACTION_SET(unit, f_ent, entbuf, &a_offset);
            }
            break;
        case bcmFieldActionRpUcastCosQNew:
            ACTION_RESOLVE(unit, f_ent, fa, &params);
            ucosq = params.cosq_new;
            mcast_mode_set = 0;
            /* Check if corresponding multicast action is set for this entry */
            for (f_ent_act = f_ent->actions;
                 f_ent_act != NULL;
                 f_ent_act = f_ent_act->next) {
                if (bcmFieldActionRpMcastCosQNew == f_ent_act->action ||
                    bcmFieldActionMcastCosQNew == f_ent_act->action) {
                    sal_memset(&params, 0, sizeof(params));
                    /* Get current Mcast queue value */
                    mcast_mode_set = 1;
                    ACTION_RESOLVE(unit, f_ent, f_ent_act, &params);
                    mcosq = params.cosq_new;
                    break;
                }
            }
            if (0 == mcast_mode_set) {
                a_offset.value[0] = ucosq;
                ACTION_SET(unit, f_ent, entbuf, &a_offset);
            } else {
                /* Set Ucast and Mcast queue modes */
                a_offset.value[0] = ucosq;
                ACTION_SET(unit, f_ent, entbuf, &a_offset);

                action = bcmFieldActionRpMcastCosQNew;
                ACTION_OFFSET_GET(unit, stage_fc, action, &a_offset, 0);

                a_offset.value[0] = mcosq;
                ACTION_SET(unit, f_ent, entbuf, &a_offset);
            }
            break;
        case bcmFieldActionGpMcastCosQNew:
            ACTION_RESOLVE(unit, f_ent, fa, &params);
            mcosq = params.cosq_new;
            ucast_mode_set = 0;
            /* Check if corresponding multicast action is set for this entry */
            for (f_ent_act = f_ent->actions;
                 f_ent_act != NULL;
                 f_ent_act = f_ent_act->next) {
                if (bcmFieldActionGpUcastCosQNew == f_ent_act->action ||
                    bcmFieldActionUcastCosQNew == f_ent_act->action) {
                    sal_memset(&params, 0, sizeof(params));
                    /* Get current Mcast queue value */
                    ucast_mode_set = 1;
                    ACTION_RESOLVE(unit, f_ent, f_ent_act, &params);
                    mcosq = params.cosq_new;
                    break;
                }
            }
            if (0 == ucast_mode_set) {
                a_offset.value[0] = mcosq;
                ACTION_SET(unit, f_ent, entbuf, &a_offset);
            } else {
                /* Set Ucast and Mcast queue modes */
                a_offset.value[0] = mcosq;
                ACTION_SET(unit, f_ent, entbuf, &a_offset);

                action = bcmFieldActionGpUcastCosQNew;
                ACTION_OFFSET_GET(unit, stage_fc, action, &a_offset, 0);

                a_offset.value[0] = ucosq;
                ACTION_SET(unit, f_ent, entbuf, &a_offset);
            }
            break;
        case bcmFieldActionYpMcastCosQNew:
            ACTION_RESOLVE(unit, f_ent, fa, &params);
            mcosq = params.cosq_new;
            ucast_mode_set = 0;
            /* Check if corresponding multicast action is set for this entry */
            for (f_ent_act = f_ent->actions;
                 f_ent_act != NULL;
                 f_ent_act = f_ent_act->next) {
                if (bcmFieldActionYpUcastCosQNew == f_ent_act->action ||
                    bcmFieldActionUcastCosQNew == f_ent_act->action) {
                    sal_memset(&params, 0, sizeof(params));
                    /* Get current Mcast queue value */
                    ucast_mode_set = 1;
                    ACTION_RESOLVE(unit, f_ent, f_ent_act, &params);
                    mcosq = params.cosq_new;
                    break;
                }
            }

            if (0 == ucast_mode_set) {
                a_offset.value[0] = mcosq;
                ACTION_SET(unit, f_ent, entbuf, &a_offset);
            } else {
                /* Set Ucast and Mcast queue modes */
                a_offset.value[0] = mcosq;
                ACTION_SET(unit, f_ent, entbuf, &a_offset);

                action = bcmFieldActionYpUcastCosQNew;
                ACTION_OFFSET_GET(unit, stage_fc, action, &a_offset, 0);

                a_offset.value[0] = ucosq;
                ACTION_SET(unit, f_ent, entbuf, &a_offset);
            }

            break;
        case bcmFieldActionRpMcastCosQNew:
            ACTION_RESOLVE(unit, f_ent, fa, &params);
            mcosq = params.cosq_new;
            ucast_mode_set = 0;
            /* Check if corresponding multicast action is set for this entry */
            for (f_ent_act = f_ent->actions;
                 f_ent_act != NULL;
                 f_ent_act = f_ent_act->next) {
                if (bcmFieldActionRpUcastCosQNew == f_ent_act->action ||
                    bcmFieldActionUcastCosQNew == f_ent_act->action) {
                    sal_memset(&params, 0, sizeof(params));
                    /* Get current Mcast queue value */
                    ucast_mode_set = 1;
                    ACTION_RESOLVE(unit, f_ent, f_ent_act, &params);
                    mcosq = params.cosq_new;
                    break;
                }
            }

            if (0 == ucast_mode_set) {
                a_offset.value[0] = mcosq;
                ACTION_SET(unit, f_ent, entbuf, &a_offset);
            } else {
                /* Set Ucast and Mcast queue modes */
                a_offset.value[0] = mcosq;
                ACTION_SET(unit, f_ent, entbuf, &a_offset);

                action = bcmFieldActionRpUcastCosQNew;
                ACTION_OFFSET_GET(unit, stage_fc, action, &a_offset, 0);

                a_offset.value[0] = ucosq;
                ACTION_SET(unit, f_ent, entbuf, &a_offset);
            }
            break;
        case bcmFieldActionGpDropPrecedence:
        case bcmFieldActionYpDropPrecedence:
        case bcmFieldActionRpDropPrecedence:
            a_offset.value[0] = fa->param[0];
            ACTION_SET(unit, f_ent, entbuf, &a_offset);
            break;
        default:
            LOG_VERBOSE(BSL_LS_BCM_FP,(BSL_META_U(unit,
                        "%s is not found.\n"), action_name[action]));
            return BCM_E_UNAVAIL;
    }

    return BCM_E_NONE;
}
/*
 * Function:
 *     _bcm_field_th_profile1_action_set
 * Purpose:
 *     Set the color independent actions of IFP_PROFILE_ONE_SET
 *     in entbuf which is to be written into POLICY table.
 * Parameters:
 *     unit     - BCM device number
 *     action   - Field Action of type bcmFieldActionXXX
 *     f_ent    - entry structure to get policy info from
 *     fa       - field action
 *     entbuf   - (OUT) Field Policy table entry
 * Returns:
 *     BCM_E_NONE
 *     BCM_E_PARAM - Action parameter out-of-range or unrecognized action.
 * Notes:
 *     This is a simple read/modify/write pattern.
 *     FP unit lock should be held by calling function.
 */
int
_bcm_field_th_profile1_action_set(int unit, _field_entry_t *f_ent,
                                  _field_action_t *fa, uint32 *entbuf)

{
    int rv;

    if (NULL == f_ent || NULL == fa || NULL == entbuf) {
        LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META("Invalid Parameters\n")));
        return BCM_E_PARAM;
    }

    switch (fa->action) {
        case bcmFieldActionCosQNew:
            rv = _bcm_field_th_profile1_cd_action_set(unit,
                                                      bcmFieldActionGpCosQNew,
                                                      f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);

            rv = _bcm_field_th_profile1_cd_action_set(unit,
                                                      bcmFieldActionYpCosQNew,
                                                      f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);

            rv = _bcm_field_th_profile1_cd_action_set(unit,
                                                      bcmFieldActionRpCosQNew,
                                                      f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);
            break;
        case bcmFieldActionPrioIntCopy:
        case bcmFieldActionPrioPktAndIntCopy:
            rv = _bcm_field_th_profile1_cd_action_set(unit,
                                                    bcmFieldActionGpPrioIntCopy,
                                                    f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);

            rv = _bcm_field_th_profile1_cd_action_set(unit,
                                                    bcmFieldActionYpPrioIntCopy,
                                                    f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);

            rv = _bcm_field_th_profile1_cd_action_set(unit,
                                                    bcmFieldActionRpPrioIntCopy,
                                                    f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);
            break;
        case bcmFieldActionPrioIntNew:
        case bcmFieldActionPrioPktAndIntNew:
            rv = _bcm_field_th_profile1_cd_action_set(unit,
                                                     bcmFieldActionGpPrioIntNew,
                                                     f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);

            rv = _bcm_field_th_profile1_cd_action_set(unit,
                                                     bcmFieldActionYpPrioIntNew,
                                                     f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);

            rv = _bcm_field_th_profile1_cd_action_set(unit,
                                                     bcmFieldActionRpPrioIntNew,
                                                     f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);
            break;
        case bcmFieldActionPrioIntTos:
        case bcmFieldActionPrioPktAndIntTos:
            rv = _bcm_field_th_profile1_cd_action_set(unit,
                                                     bcmFieldActionGpPrioIntTos,
                                                     f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);

            rv = _bcm_field_th_profile1_cd_action_set(unit,
                                                     bcmFieldActionYpPrioIntTos,
                                                     f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);

            rv = _bcm_field_th_profile1_cd_action_set(unit,
                                                     bcmFieldActionRpPrioIntTos,
                                                     f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);
            break;
        case bcmFieldActionPrioIntCancel:
        case bcmFieldActionPrioPktAndIntCancel:
            rv = _bcm_field_th_profile1_cd_action_set(unit,
                                                  bcmFieldActionGpPrioIntCancel,
                                                  f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);

            rv = _bcm_field_th_profile1_cd_action_set(unit,
                                                  bcmFieldActionYpPrioIntCancel,
                                                  f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);

            rv = _bcm_field_th_profile1_cd_action_set(unit,
                                                  bcmFieldActionRpPrioIntCancel,
                                                  f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);
            break;
        case bcmFieldActionUcastCosQNew:
            rv = _bcm_field_th_profile1_cd_action_set(unit,
                                                   bcmFieldActionGpUcastCosQNew,
                                                   f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);

            rv = _bcm_field_th_profile1_cd_action_set(unit,
                                                   bcmFieldActionYpUcastCosQNew,
                                                   f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);

            rv = _bcm_field_th_profile1_cd_action_set(unit,
                                                   bcmFieldActionRpUcastCosQNew,
                                                   f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);
            break;
        case bcmFieldActionMcastCosQNew:
            rv = _bcm_field_th_profile1_cd_action_set(unit,
                                                   bcmFieldActionGpMcastCosQNew,
                                                   f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);

            rv = _bcm_field_th_profile1_cd_action_set(unit,
                                                   bcmFieldActionYpMcastCosQNew,
                                                   f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);

            rv = _bcm_field_th_profile1_cd_action_set(unit,
                                                   bcmFieldActionRpMcastCosQNew,
                                                   f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);
            break;
        case bcmFieldActionDropPrecedence:
            rv = _bcm_field_th_profile1_cd_action_set(unit,
                                                 bcmFieldActionGpDropPrecedence,
                                                 f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);

            rv = _bcm_field_th_profile1_cd_action_set(unit,
                                                 bcmFieldActionYpDropPrecedence,
                                                 f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);

            rv = _bcm_field_th_profile1_cd_action_set(unit,
                                                 bcmFieldActionRpDropPrecedence,
                                                 f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);
            break;
        default:
            return _bcm_field_th_profile1_cd_action_set(unit, fa->action,
                                                        f_ent, fa, entbuf);
    }

    return BCM_E_NONE;
}
/*
 * Function:
 *     _bcm_field_th_profile2_cd_action_set
 * Purpose:
 *     Set the color dependent actions of IFP_PROFILE_TWO_SET
 *     in entbuf which is to be written into POLICY table.
 * Parameters:
 *     unit     - BCM device number
 *     action   - Field Action of type bcmFieldActionXXX
 *     f_ent    - entry structure to get policy info from
 *     fa       - field action
 *     entbuf   - (OUT) Field Policy table entry
 * Returns:
 *     BCM_E_NONE
 *     BCM_E_PARAM - Action parameter out-of-range or unrecognized action.
 * Notes:
 *     This is a simple read/modify/write pattern.
 *     FP unit lock should be held by calling function.
 */
int
_bcm_field_th_profile2_cd_action_set(int unit,
                                     bcm_field_action_t action,
                                     _field_entry_t *f_ent,
                                     _field_action_t *fa, uint32 *entbuf)

{
    int rv;
    _field_stage_t *stage_fc;
    _bcm_field_action_offset_t a_offset;


    if (NULL == f_ent || NULL == fa || NULL == entbuf) {
        LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META("Invalid Parameters\n")));
        return BCM_E_PARAM;
    }

    rv = _field_stage_control_get(unit, f_ent->group->stage_id, &stage_fc);
    BCM_IF_ERROR_RETURN(rv);

    rv = _bcm_field_action_offset_get(unit, stage_fc, action, &a_offset, 0);
    BCM_IF_ERROR_RETURN(rv);

    switch (action) {
        case bcmFieldActionGpPrioPktNew:
        case bcmFieldActionYpPrioPktNew:
        case bcmFieldActionRpPrioPktNew:
        case bcmFieldActionGpTosPrecedenceNew:
        case bcmFieldActionGpDscpNew:
        case bcmFieldActionYpDscpNew:
        case bcmFieldActionRpDscpNew:
        case bcmFieldActionGpEcnNew:
        case bcmFieldActionYpEcnNew:
        case bcmFieldActionRpEcnNew:
            a_offset.value[0] = fa->param[0];
            ACTION_SET(unit, f_ent, entbuf, &a_offset);
            break;
        case bcmFieldActionGpPrioPktCopy:
        case bcmFieldActionGpPrioPktTos:
        case bcmFieldActionGpPrioPktCancel:
        case bcmFieldActionGpTosPrecedenceCopy:
        case bcmFieldActionGpDscpCancel:
        case bcmFieldActionYpPrioPktCopy:
        case bcmFieldActionYpPrioPktTos:
        case bcmFieldActionYpPrioPktCancel:
        case bcmFieldActionYpDscpCancel:
        case bcmFieldActionRpPrioPktCopy:
        case bcmFieldActionRpPrioPktTos:
        case bcmFieldActionRpPrioPktCancel:
        case bcmFieldActionRpDscpCancel:
            ACTION_SET(unit, f_ent, entbuf, &a_offset);
            break;
        default:
            LOG_VERBOSE(BSL_LS_BCM_FP,(BSL_META_U(unit,
                        "%s is not found.\n"), action_name[action]));
            return BCM_E_UNAVAIL;
    }

    return BCM_E_NONE;
}
/*
 * Function:
 *     _bcm_field_th_profile2_action_set
 * Purpose:
 *     Set the color independent actions of IFP_PROFILE_TWO_SET
 *     in entbuf which is to be written into POLICY table.
 * Parameters:
 *     unit     - BCM device number
 *     action   - Field Action of type bcmFieldActionXXX
 *     f_ent    - entry structure to get policy info from
 *     fa       - field action
 *     entbuf   - (OUT) Field Policy table entry
 * Returns:
 *     BCM_E_NONE
 *     BCM_E_PARAM - Action parameter out-of-range or unrecognized action.
 * Notes:
 *     This is a simple read/modify/write pattern.
 *     FP unit lock should be held by calling function.
 */
int
_bcm_field_th_profile2_action_set(int unit, _field_entry_t *f_ent,
                                  _field_action_t *fa, uint32 *entbuf)

{
    int rv;

    if (NULL == f_ent || NULL == fa || NULL == entbuf) {
        LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META("Invalid Parameters\n")));
        return BCM_E_PARAM;
    }

    switch (fa->action) {
        case bcmFieldActionEcnNew:
            rv = _bcm_field_th_profile2_cd_action_set(unit,
                                                      bcmFieldActionGpEcnNew,
                                                      f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);

            rv = _bcm_field_th_profile2_cd_action_set(unit,
                                                      bcmFieldActionYpEcnNew,
                                                      f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);

            rv = _bcm_field_th_profile2_cd_action_set(unit,
                                                      bcmFieldActionRpEcnNew,
                                                      f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);
            break;
        case bcmFieldActionPrioPktNew:
        case bcmFieldActionPrioPktAndIntNew:
            rv = _bcm_field_th_profile2_cd_action_set(unit,
                                                      bcmFieldActionGpPrioPktNew,
                                                      f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);

            rv = _bcm_field_th_profile2_cd_action_set(unit,
                                                      bcmFieldActionYpPrioPktNew,
                                                      f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);

            rv = _bcm_field_th_profile2_cd_action_set(unit,
                                                      bcmFieldActionRpPrioPktNew,
                                                      f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);
            break;
        case bcmFieldActionDscpNew:
            rv = _bcm_field_th_profile2_cd_action_set(unit,
                                                      bcmFieldActionGpDscpNew,
                                                      f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);

            rv = _bcm_field_th_profile2_cd_action_set(unit,
                                                      bcmFieldActionYpDscpNew,
                                                      f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);

            rv = _bcm_field_th_profile2_cd_action_set(unit,
                                                      bcmFieldActionRpDscpNew,
                                                      f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);
            break;
        case bcmFieldActionDscpCancel:
            rv = _bcm_field_th_profile2_cd_action_set(unit,
                                                      bcmFieldActionGpDscpCancel,
                                                      f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);

            rv = _bcm_field_th_profile2_cd_action_set(unit,
                                                      bcmFieldActionYpDscpCancel,
                                                      f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);

            rv = _bcm_field_th_profile2_cd_action_set(unit,
                                                      bcmFieldActionRpDscpCancel,
                                                      f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);
            break;
        case bcmFieldActionPrioPktCopy:
        case bcmFieldActionPrioPktAndIntCopy:
            rv = _bcm_field_th_profile2_cd_action_set(unit,
                                                      bcmFieldActionGpPrioPktCopy,
                                                      f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);

            rv = _bcm_field_th_profile2_cd_action_set(unit,
                                                      bcmFieldActionYpPrioPktCopy,
                                                      f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);

            rv = _bcm_field_th_profile2_cd_action_set(unit,
                                                      bcmFieldActionRpPrioPktCopy,
                                                      f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);
            break;
        case bcmFieldActionPrioPktTos:
        case bcmFieldActionPrioPktAndIntTos:
            rv = _bcm_field_th_profile2_cd_action_set(unit,
                                                      bcmFieldActionGpPrioPktTos,
                                                      f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);

            rv = _bcm_field_th_profile2_cd_action_set(unit,
                                                      bcmFieldActionYpPrioPktTos,
                                                      f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);

            rv = _bcm_field_th_profile2_cd_action_set(unit,
                                                      bcmFieldActionRpPrioPktTos,
                                                      f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);
            break;
        case bcmFieldActionPrioPktCancel:
        case bcmFieldActionPrioPktAndIntCancel:
            rv = _bcm_field_th_profile2_cd_action_set(unit,
                                                      bcmFieldActionGpPrioPktCancel,
                                                      f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);

            rv = _bcm_field_th_profile2_cd_action_set(unit,
                                                      bcmFieldActionYpPrioPktCancel,
                                                      f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);

            rv = _bcm_field_th_profile2_cd_action_set(unit,
                                                      bcmFieldActionRpPrioPktCancel,
                                                      f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);
            break;
        default:
            return _bcm_field_th_profile2_cd_action_set(unit, fa->action,
                                                        f_ent, fa, entbuf);
    }

    return BCM_E_NONE;
}
/*
 * Function:
 *     _bcm_field_th_redirect_action_set
 * Purpose:
 *     Set the actions of IFP_REDIRECT_SET in entbuf
 *     which is to be written into POLICY table.
 * Parameters:
 *     unit     - BCM device number
 *     action   - Field Action of type bcmFieldActionXXX
 *     f_ent    - entry structure to get policy info from
 *     fa       - field action
 *     entbuf   - (OUT) Field Policy table entry
 * Returns:
 *     BCM_E_NONE
 *     BCM_E_PARAM - Action parameter out-of-range or unrecognized action.
 * Notes:
 *     This is a simple read/modify/write pattern.
 *     FP unit lock should be held by calling function.
 */
int
_bcm_field_th_redirect_action_set(int unit,
                                  _field_entry_t *f_ent,
                                  _field_action_t *fa, uint32 *entbuf)

{
    int rv;
    _field_stage_t *stage_fc;
    _field_action_params_t params;
    _bcm_field_action_offset_t a_offset;


    if (NULL == f_ent || NULL == fa || NULL == entbuf) {
        LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META("Invalid Parameters\n")));
        return BCM_E_PARAM;
    }

    rv = _field_stage_control_get(unit, f_ent->group->stage_id, &stage_fc);
    BCM_IF_ERROR_RETURN(rv);

    sal_memset(&params, 0, sizeof(params));

    switch(fa->action) {
        case bcmFieldActionRedirectTrunk:
            ACTION_OFFSET_GET(unit, stage_fc, fa->action, &a_offset, 0);
            ACTION_RESOLVE(unit, f_ent, fa, &params);
            a_offset.value[0] = params.redirect_value;
            ACTION_SET(unit, f_ent, entbuf, &a_offset);
            break;
        case bcmFieldActionRedirect:
        case bcmFieldActionUnmodifiedPacketRedirectPort:
            ACTION_RESOLVE(unit, f_ent, fa, &params);
            if (params.is_dvp) {
                ACTION_OFFSET_GET(unit, stage_fc, fa->action, &a_offset,
                                      _BCM_FIELD_ACTION_REDIRECT_DVP);
            } else if (params.is_trunk) {
                ACTION_OFFSET_GET(unit, stage_fc, fa->action, &a_offset,
                                      _BCM_FIELD_ACTION_REDIRECT_TRUNK);
            } else {
                ACTION_OFFSET_GET(unit, stage_fc, fa->action, &a_offset,
                                       _BCM_FIELD_ACTION_REDIRECT_DGLP);
            }
            a_offset.value[0] = params.redirect_value;
            ACTION_SET(unit, f_ent, entbuf, &a_offset);
            break;
#ifdef INCLUDE_L3
        case bcmFieldActionRedirectEgrNextHop:
            ACTION_RESOLVE(unit, f_ent, fa, &params);
            if (params.flags & BCM_L3_MULTIPATH) {
                ACTION_OFFSET_GET(unit, stage_fc, fa->action, &a_offset,
                                      _BCM_FIELD_ACTION_REDIRECT_ECMP);
            } else {
                ACTION_OFFSET_GET(unit, stage_fc, fa->action, &a_offset,
                                      _BCM_FIELD_ACTION_REDIRECT_NEXT_HOP);
            }
            a_offset.value[0] = params.nh_ecmp_id;
            ACTION_SET(unit, f_ent, entbuf, &a_offset);
            break;
#endif
        case bcmFieldActionRedirectCancel:
            ACTION_OFFSET_GET(unit, stage_fc, fa->action, &a_offset, 0);
            ACTION_SET(unit, f_ent, entbuf, &a_offset);
            break;
        case bcmFieldActionRedirectPbmp:
        case bcmFieldActionRedirectVlan:
        case bcmFieldActionRedirectBcastPbmp:
        case bcmFieldActionRedirectMcast:
#ifdef INCLUDE_L3
        case bcmFieldActionRedirectIpmc:
#endif
            ACTION_OFFSET_GET(unit, stage_fc, fa->action, &a_offset, 0);
            ACTION_RESOLVE(unit, f_ent, fa, &params);
            a_offset.value[0] = params.redirect_value;
            a_offset.value[1] = params.mcast_redirect_control;
            ACTION_SET(unit, f_ent, entbuf, &a_offset);
            break;
        case bcmFieldActionEgressMask:
            ACTION_OFFSET_GET(unit, stage_fc, fa->action, &a_offset, 0);
            ACTION_RESOLVE(unit, f_ent, fa, &params);
            a_offset.value[0] = params.redirect_value;
            ACTION_SET(unit, f_ent, entbuf, &a_offset);
            break;
        case bcmFieldActionEgressPortsAdd:
            ACTION_OFFSET_GET(unit, stage_fc, fa->action, &a_offset, 0);
            ACTION_RESOLVE(unit, f_ent, fa, &params);
            a_offset.value[0] = params.redirect_value;
            ACTION_SET(unit, f_ent, entbuf, &a_offset);
            break;
        default:
            LOG_VERBOSE(BSL_LS_BCM_FP,(BSL_META_U(unit,
                        "%s is not found.\n"), action_name[fa->action]));
            return BCM_E_UNAVAIL;
    }

    f_ent->flags |= _FP_ENTRY_COLOR_INDEPENDENT;
    return BCM_E_NONE;
}
/*
 * Function:
 *     _bcm_field_th_l3swl2change_action_set
 * Purpose:
 *     Set the actions of IFP_REDIRECT_SET in entbuf
 *     which is to be written into POLICY table.
 * Parameters:
 *     unit     - BCM device number
 *     action   - Field Action of type bcmFieldActionXXX
 *     f_ent    - entry structure to get policy info from
 *     fa       - field action
 *     entbuf   - (OUT) Field Policy table entry
 * Returns:
 *     BCM_E_NONE
 *     BCM_E_PARAM - Action parameter out-of-range or unrecognized action.
 * Notes:
 *     This is a simple read/modify/write pattern.
 *     FP unit lock should be held by calling function.
 */
int
_bcm_field_th_l3swl2change_action_set(int unit,
                                      _field_entry_t *f_ent,
                                      _field_action_t *fa, uint32 *entbuf)
{
    int rv;
    _field_stage_t *stage_fc;
    _field_action_params_t params;
    _bcm_field_action_offset_t a_offset;

    if (NULL == f_ent || NULL == fa || NULL == entbuf) {
        LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META("Invalid Parameters\n")));
        return BCM_E_PARAM;
    }

    rv = _field_stage_control_get(unit, f_ent->group->stage_id, &stage_fc);
    BCM_IF_ERROR_RETURN(rv);

    sal_memset(&params, 0, sizeof(params));

    switch (fa->action) {
        case bcmFieldActionEgressClassSelect:
            ACTION_OFFSET_GET(unit, stage_fc, fa->action, &a_offset, 0);
            ACTION_RESOLVE(unit, f_ent, fa, &params);
            a_offset.value[0] = params.i2e_cl_sel;
            ACTION_SET(unit, f_ent, entbuf, &a_offset);
            break;
        case bcmFieldActionHiGigClassSelect:
            ACTION_OFFSET_GET(unit, stage_fc, fa->action, &a_offset, 0);
            ACTION_RESOLVE(unit, f_ent, fa, &params);
            a_offset.value[0] = params.i2e_hg_cl_sel;
            ACTION_SET(unit, f_ent, entbuf, &a_offset);
            break;
        case bcmFieldActionNewClassId:
        case bcmFieldActionAddClassTag:
        case bcmFieldActionMultipathHash:
            ACTION_OFFSET_GET(unit, stage_fc, fa->action, &a_offset, 0);
            a_offset.value[0] = fa->param[0];
            ACTION_SET(unit, f_ent, entbuf, &a_offset);
            break;
        case bcmFieldActionDstMacNew:
        case bcmFieldActionSrcMacNew:
        case bcmFieldActionOuterVlanNew:
        case bcmFieldActionVnTagNew:
        case bcmFieldActionVnTagDelete:
        case bcmFieldActionEtagNew:
        case bcmFieldActionEtagDelete:
            ACTION_OFFSET_GET(unit, stage_fc, fa->action, &a_offset, 0);
            a_offset.value[0] = fa->hw_index;
            ACTION_SET(unit, f_ent, entbuf, &a_offset);
            break;
        case bcmFieldActionFabricQueue:
            ACTION_OFFSET_GET(unit, stage_fc, fa->action, &a_offset, 0);
            ACTION_RESOLVE(unit, f_ent, fa, &params);
            a_offset.value[0] = params.fabric_tag_type;
            a_offset.value[1] = params.fabric_tag;
            ACTION_SET(unit, f_ent, entbuf, &a_offset);
            break;
#ifdef INCLUDE_L3
        case bcmFieldActionL3Switch:
            ACTION_RESOLVE(unit, f_ent, fa, &params);
            if (params.flags & BCM_L3_MULTIPATH) {
                ACTION_OFFSET_GET(unit, stage_fc, fa->action, &a_offset,
                                     _BCM_FIELD_ACTION_L3SWITCH_ECMP);
                a_offset.value[0]  = params.nh_ecmp_id;
                ACTION_SET(unit, f_ent, entbuf, &a_offset);
            } else {
                ACTION_OFFSET_GET(unit, stage_fc, fa->action, &a_offset,
                                 _BCM_FIELD_ACTION_L3SWITCH_NEXT_HOP);
                a_offset.value[0] = params.nh_ecmp_id;
                ACTION_SET(unit, f_ent, entbuf, &a_offset);
            }
            break;
#endif
        case bcmFieldActionL3SwitchCancel:
            ACTION_OFFSET_GET(unit, stage_fc, fa->action, &a_offset, 0);
            ACTION_SET(unit, f_ent, entbuf, &a_offset);
            break;
        default:
            LOG_VERBOSE(BSL_LS_BCM_FP,(BSL_META_U(unit,
                        "%s is not found.\n"), action_name[fa->action]));
            return BCM_E_UNAVAIL;
    }

    f_ent->flags |= _FP_ENTRY_COLOR_INDEPENDENT;
    return BCM_E_NONE;
}
/*
 * Function:
 *     _bcm_field_th_misc_cd_action_set
 * Purpose:
 *     Set the miscellaneous color dependent actions in entbuf which is to be
 *     written into POLICY table.
 * Parameters:
 *     unit     - BCM device number
 *     action   - Field Action of type bcmFieldActionXXX
 *     f_ent    - entry structure to get policy info from
 *     fa       - field action
 *     entbuf   - (OUT) Field Policy table entry
 * Returns:
 *     BCM_E_NONE
 *     BCM_E_PARAM - Action parameter out-of-range or unrecognized action.
 * Notes:
 *     This is a simple read/modify/write pattern.
 *     FP unit lock should be held by calling function.
 */
int
_bcm_field_th_misc_cd_action_set(int unit,
                                 bcm_field_action_t action,
                                 _field_entry_t *f_ent,
                                 _field_action_t *fa, uint32 *entbuf)
{
    int rv;
    _field_stage_t *stage_fc;
    _field_action_params_t params;
    _bcm_field_action_offset_t a_offset;

    if (NULL == f_ent || NULL == fa || NULL == entbuf) {
        LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META("Invalid Parameters\n")));
        return BCM_E_PARAM;
    }

    rv = _field_stage_control_get(unit, f_ent->group->stage_id, &stage_fc);
    BCM_IF_ERROR_RETURN(rv);

    rv = _bcm_field_action_offset_get(unit, stage_fc, action, &a_offset, 0);
    BCM_IF_ERROR_RETURN(rv);

    sal_memset(&params, 0, sizeof(params));

    switch (action) {
        case bcmFieldActionGpSwitchToCpuReinstate:
        case bcmFieldActionYpSwitchToCpuReinstate:
        case bcmFieldActionRpSwitchToCpuReinstate:
        case bcmFieldActionGpDrop:
        case bcmFieldActionYpDrop:
        case bcmFieldActionRpDrop:
        case bcmFieldActionGpDropCancel:
        case bcmFieldActionYpDropCancel:
        case bcmFieldActionRpDropCancel:
            ACTION_SET(unit, f_ent, entbuf, &a_offset);
            break;
        case bcmFieldActionGpTimeStampToCpu:
        case bcmFieldActionYpTimeStampToCpu:
        case bcmFieldActionRpTimeStampToCpu:
        case bcmFieldActionGpCopyToCpu:
        case bcmFieldActionYpCopyToCpu:
        case bcmFieldActionRpCopyToCpu:
            if (fa->param[0]) {
                a_offset.value[0] = fa->param[1];
            } else {
                a_offset.value[0] = 0;
            }
            ACTION_SET(unit, f_ent, entbuf, &a_offset);
            break;
        case bcmFieldActionGpCopyToCpuCancel:
            sal_memset(&a_offset, 0 , sizeof(a_offset));
            action = bcmFieldActionGpSwitchToCpuCancel;
            ACTION_GET(unit, f_ent, entbuf, action, 0, &a_offset);
            if (a_offset.value[0] == 0x3) {
                a_offset.value[0] = 0x6;
            } else {
                a_offset.value[0] = 0x2;
            }
            ACTION_SET(unit, f_ent, entbuf, &a_offset);
            break;
        case bcmFieldActionYpCopyToCpuCancel:
            sal_memset(&a_offset, 0 , sizeof(a_offset));
            action = bcmFieldActionYpSwitchToCpuCancel;
            ACTION_GET(unit, f_ent, entbuf, action, 0, &a_offset);
            if (a_offset.value[0] == 0x3) {
                a_offset.value[0] = 0x6;
            } else {
                a_offset.value[0] = 0x2;
            }
            ACTION_SET(unit, f_ent, entbuf, &a_offset);
            break;
        case bcmFieldActionRpCopyToCpuCancel:
            sal_memset(&a_offset, 0 , sizeof(a_offset));
            action = bcmFieldActionRpSwitchToCpuCancel;
            ACTION_GET(unit, f_ent, entbuf, action, 0, &a_offset);
            if (a_offset.value[0] == 0x3) {
                a_offset.value[0] = 0x6;
            } else {
                a_offset.value[0] = 0x2;
            }
            ACTION_SET(unit, f_ent, entbuf, &a_offset);
            break;
        case bcmFieldActionGpSwitchToCpuCancel:
            sal_memset(&a_offset, 0 , sizeof(a_offset));
            action = bcmFieldActionGpCopyToCpuCancel;
            ACTION_GET(unit, f_ent, entbuf, action, 0, &a_offset);
            if (a_offset.value[0] == 0x2) {
                a_offset.value[0] = 0x6;
            } else {
                a_offset.value[0] = 0x3;
            }
            ACTION_SET(unit, f_ent, entbuf, &a_offset);
            break;
        case bcmFieldActionYpSwitchToCpuCancel:
            sal_memset(&a_offset, 0 , sizeof(a_offset));
            action = bcmFieldActionYpCopyToCpuCancel;
            ACTION_GET(unit, f_ent, entbuf, action, 0, &a_offset);
            if (a_offset.value[0] == 0x2) {
                a_offset.value[0] = 0x6;
            } else {
                a_offset.value[0] = 0x3;
            }
            ACTION_SET(unit, f_ent, entbuf, &a_offset);
            break;
        case bcmFieldActionRpSwitchToCpuCancel:
            sal_memset(&a_offset, 0 , sizeof(a_offset));
            action = bcmFieldActionRpCopyToCpuCancel;
            ACTION_GET(unit, f_ent, entbuf, action, 0, &a_offset);
            if (a_offset.value[0] == 0x2) {
                a_offset.value[0] = 0x6;
            } else {
                a_offset.value[0] = 0x3;
            }
            ACTION_SET(unit, f_ent, entbuf, &a_offset);
            break;
        case bcmFieldActionRpPrioPktAndIntCopy:
            rv = _bcm_field_th_profile1_cd_action_set(unit,
                                                    bcmFieldActionRpPrioIntCopy,
                                                    f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);
            rv = _bcm_field_th_profile2_cd_action_set(unit,
                                                    bcmFieldActionRpPrioPktCopy,
                                                    f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);
            break;
        case bcmFieldActionRpPrioPktAndIntNew:
            rv = _bcm_field_th_profile1_cd_action_set(unit,
                                                    bcmFieldActionRpPrioIntNew,
                                                    f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);
            rv = _bcm_field_th_profile2_cd_action_set(unit,
                                                    bcmFieldActionRpPrioPktNew,
                                                    f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);
            break;
        case bcmFieldActionRpPrioPktAndIntTos:
            rv = _bcm_field_th_profile1_cd_action_set(unit,
                                                    bcmFieldActionRpPrioIntTos,
                                                    f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);
            rv = _bcm_field_th_profile2_cd_action_set(unit,
                                                    bcmFieldActionRpPrioPktTos,
                                                    f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);
            break;
        case bcmFieldActionRpPrioPktAndIntCancel:
            rv = _bcm_field_th_profile1_cd_action_set(unit,
                                                    bcmFieldActionRpPrioIntCancel,
                                                    f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);
            rv = _bcm_field_th_profile2_cd_action_set(unit,
                                                    bcmFieldActionRpPrioPktCancel,
                                                    f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);
            break;
        case bcmFieldActionGpPrioPktAndIntCopy:
            rv = _bcm_field_th_profile1_cd_action_set(unit,
                                                    bcmFieldActionGpPrioIntCopy,
                                                    f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);
            rv = _bcm_field_th_profile2_cd_action_set(unit,
                                                    bcmFieldActionGpPrioPktCopy,
                                                    f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);
            break;
        case bcmFieldActionGpPrioPktAndIntNew:
            rv = _bcm_field_th_profile1_cd_action_set(unit,
                                                    bcmFieldActionGpPrioIntNew,
                                                    f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);
            rv = _bcm_field_th_profile2_cd_action_set(unit,
                                                    bcmFieldActionGpPrioPktNew,
                                                    f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);
            break;
        case bcmFieldActionGpPrioPktAndIntTos:
            rv = _bcm_field_th_profile1_cd_action_set(unit,
                                                    bcmFieldActionGpPrioIntTos,
                                                    f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);
            rv = _bcm_field_th_profile2_cd_action_set(unit,
                                                    bcmFieldActionGpPrioPktTos,
                                                    f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);
            break;
        case bcmFieldActionGpPrioPktAndIntCancel:
            rv = _bcm_field_th_profile1_cd_action_set(unit,
                                                    bcmFieldActionGpPrioIntCancel,
                                                    f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);
            rv = _bcm_field_th_profile2_cd_action_set(unit,
                                                    bcmFieldActionGpPrioPktCancel,
                                                    f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);
            break;
        case bcmFieldActionYpPrioPktAndIntCopy:
            rv = _bcm_field_th_profile1_cd_action_set(unit,
                                                    bcmFieldActionYpPrioIntCopy,
                                                    f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);
            rv = _bcm_field_th_profile2_cd_action_set(unit,
                                                    bcmFieldActionYpPrioPktCopy,
                                                    f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);
            break;
        case bcmFieldActionYpPrioPktAndIntNew:
            rv = _bcm_field_th_profile1_cd_action_set(unit,
                                                    bcmFieldActionYpPrioIntNew,
                                                    f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);
            rv = _bcm_field_th_profile2_cd_action_set(unit,
                                                    bcmFieldActionYpPrioPktNew,
                                                    f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);
            break;
        case bcmFieldActionYpPrioPktAndIntTos:
            rv = _bcm_field_th_profile1_cd_action_set(unit,
                                                    bcmFieldActionYpPrioIntTos,
                                                    f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);
            rv = _bcm_field_th_profile2_cd_action_set(unit,
                                                    bcmFieldActionYpPrioPktTos,
                                                    f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);
            break;
        case bcmFieldActionYpPrioPktAndIntCancel:
            rv = _bcm_field_th_profile1_cd_action_set(unit,
                                                    bcmFieldActionYpPrioIntCancel,
                                                    f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);
            rv = _bcm_field_th_profile2_cd_action_set(unit,
                                                    bcmFieldActionYpPrioPktCancel,
                                                    f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);
            break;

        default:
            LOG_VERBOSE(BSL_LS_BCM_FP,(BSL_META_U(unit,
                        "%s is not found.\n"), action_name[action]));
            return BCM_E_UNAVAIL;
    }

    return BCM_E_NONE;

}

/*
 * Function:
 *     _bcm_field_th_misc_action_set
 * Purpose:
 *     Set the miscellaneous actions in entbuf which is to be
 *     written into POLICY table.
 * Parameters:
 *     unit     - BCM device number
 *     action   - Field Action of type bcmFieldActionXXX
 *     f_ent    - entry structure to get policy info from
 *     fa       - field action
 *     entbuf   - (OUT) Field Policy table entry
 * Returns:
 *     BCM_E_NONE
 *     BCM_E_PARAM - Action parameter out-of-range or unrecognized action.
 * Notes:
 *     This is a simple read/modify/write pattern.
 *     FP unit lock should be held by calling function.
 */
int
_bcm_field_th_misc_action_set(int unit,
                              _field_entry_t *f_ent,
                              _field_action_t *fa, uint32 *entbuf)
{
    int rv;
    _field_stage_t *stage_fc;
    _field_action_params_t params;
    _bcm_field_action_offset_t a_offset;


    if (NULL == f_ent || NULL == fa || NULL == entbuf) {
        LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META("Invalid Parameters\n")));
        return BCM_E_PARAM;
    }

    rv = _field_stage_control_get(unit, f_ent->group->stage_id, &stage_fc);
    BCM_IF_ERROR_RETURN(rv);

    rv = _bcm_field_action_offset_get(unit, stage_fc, fa->action, &a_offset, 0);
    BCM_IF_ERROR_RETURN(rv);

    sal_memset(&params, 0, sizeof(params));

    switch (fa->action) {
        case bcmFieldActionTrunkLoadBalanceCancel:
        case bcmFieldActionDynamicHgTrunkCancel:
        case bcmFieldActionEcmpLoadBalanceCancel:
        case bcmFieldActionNatCancel:
        case bcmFieldActionDoNotCheckUrpf:
        case bcmFieldActionDoNotChangeTtl:
        case bcmFieldActionMirrorOverride:
        case bcmFieldActionIngSampleEnable:
        case bcmFieldActionNat:
            ACTION_SET(unit, f_ent, entbuf, &a_offset);
            break;
#ifdef INCLUDE_L3
        case bcmFieldActionNatEgressOverride:
            ACTION_RESOLVE(unit, f_ent, fa, &params);
            a_offset.value[0] = params.hw_idx;
            a_offset.value[1] = params.hw_half;
            ACTION_SET(unit, f_ent, entbuf, &a_offset);
            break;
#endif
        case bcmFieldActionCosQCpuNew:
        case bcmFieldActionServicePoolIdNew:
            a_offset.value[0] = fa->param[0];
            ACTION_SET(unit, f_ent, entbuf, &a_offset);
            break;
        case bcmFieldActionMirrorIngress:
        case bcmFieldActionMirrorEgress:
            sal_memset(&a_offset, 0 , sizeof(a_offset));
            ACTION_GET(unit, f_ent, entbuf, fa->action, 0, &a_offset);
            ACTION_RESOLVE(unit, f_ent, fa, &params);
            a_offset.value[0] |= (0x1 << params.mtp_type_index);
            a_offset.value[1] |= (0x3 & params.mtp_index) << (2 * params.mtp_type_index);
            ACTION_SET(unit, f_ent, entbuf, &a_offset);
            break;
        case bcmFieldActionDrop:
            rv = _bcm_field_th_misc_cd_action_set(unit,
                                                  bcmFieldActionGpDrop,
                                                  f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);

            rv = _bcm_field_th_misc_cd_action_set(unit,
                                                  bcmFieldActionYpDrop,
                                                  f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);

            rv = _bcm_field_th_misc_cd_action_set(unit,
                                                  bcmFieldActionRpDrop,
                                                  f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);
            break;
        case bcmFieldActionDropCancel:
            rv = _bcm_field_th_misc_cd_action_set(unit,
                                                  bcmFieldActionGpDropCancel,
                                                  f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);

            rv = _bcm_field_th_misc_cd_action_set(unit,
                                                  bcmFieldActionYpDropCancel,
                                                  f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);

            rv = _bcm_field_th_misc_cd_action_set(unit,
                                                  bcmFieldActionRpDropCancel,
                                                  f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);
            break;
        case bcmFieldActionCopyToCpu:
            rv = _bcm_field_th_misc_cd_action_set(unit,
                                                  bcmFieldActionGpCopyToCpu,
                                                  f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);

            rv = _bcm_field_th_misc_cd_action_set(unit,
                                                  bcmFieldActionYpCopyToCpu,
                                                  f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);

            rv = _bcm_field_th_misc_cd_action_set(unit,
                                                  bcmFieldActionRpCopyToCpu,
                                                  f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);
            break;
        case bcmFieldActionCopyToCpuCancel:
            rv = _bcm_field_th_misc_cd_action_set(unit,
                                                bcmFieldActionGpCopyToCpuCancel,
                                                f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);

            rv = _bcm_field_th_misc_cd_action_set(unit,
                                                bcmFieldActionYpCopyToCpuCancel,
                                                f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);

            rv = _bcm_field_th_misc_cd_action_set(unit,
                                                bcmFieldActionRpCopyToCpuCancel,
                                                f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);
            break;
        case bcmFieldActionSwitchToCpuCancel:
            rv = _bcm_field_th_misc_cd_action_set(unit,
                                              bcmFieldActionGpSwitchToCpuCancel,
                                              f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);

            rv = _bcm_field_th_misc_cd_action_set(unit,
                                              bcmFieldActionYpSwitchToCpuCancel,
                                              f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);

            rv = _bcm_field_th_misc_cd_action_set(unit,
                                              bcmFieldActionRpSwitchToCpuCancel,
                                              f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);
            break;
        case bcmFieldActionSwitchToCpuReinstate:
            rv = _bcm_field_th_misc_cd_action_set(unit,
                                           bcmFieldActionGpSwitchToCpuReinstate,
                                           f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);

            rv = _bcm_field_th_misc_cd_action_set(unit,
                                           bcmFieldActionYpSwitchToCpuReinstate,
                                           f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);

            rv = _bcm_field_th_misc_cd_action_set(unit,
                                           bcmFieldActionRpSwitchToCpuReinstate,
                                           f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);
            break;
        case bcmFieldActionTimeStampToCpu:
            rv = _bcm_field_th_misc_cd_action_set(unit,
                                              bcmFieldActionGpTimeStampToCpu,
                                              f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);

            rv = _bcm_field_th_misc_cd_action_set(unit,
                                              bcmFieldActionYpTimeStampToCpu,
                                              f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);

            rv = _bcm_field_th_misc_cd_action_set(unit,
                                              bcmFieldActionRpTimeStampToCpu,
                                              f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);

	    rv = _bcm_field_th_misc_cd_action_set(unit,
                                                  bcmFieldActionGpDrop,
                                                  f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);

            rv = _bcm_field_th_misc_cd_action_set(unit,
                                                  bcmFieldActionYpDrop,
                                                  f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);

            rv = _bcm_field_th_misc_cd_action_set(unit,
                                                  bcmFieldActionRpDrop,
                                                  f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);
            break;
        case bcmFieldActionGpTimeStampToCpu:
            rv = _bcm_field_th_misc_cd_action_set(unit,
                                              bcmFieldActionGpTimeStampToCpu,
                                              f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);

	    rv = _bcm_field_th_misc_cd_action_set(unit,
                                                  bcmFieldActionGpDrop,
                                                  f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);
            break;
        case bcmFieldActionYpTimeStampToCpu:
            rv = _bcm_field_th_misc_cd_action_set(unit,
                                              bcmFieldActionYpTimeStampToCpu,
                                              f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);

            rv = _bcm_field_th_misc_cd_action_set(unit,
                                                  bcmFieldActionYpDrop,
                                                  f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);
            break;
        case bcmFieldActionRpTimeStampToCpu:
            rv = _bcm_field_th_misc_cd_action_set(unit,
                                              bcmFieldActionRpTimeStampToCpu,
                                              f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);

            rv = _bcm_field_th_misc_cd_action_set(unit,
                                                  bcmFieldActionRpDrop,
                                                  f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);
            break;
        case bcmFieldActionTimeStampToCpuCancel:
            rv = _bcm_field_th_misc_cd_action_set(unit,
                                                bcmFieldActionGpCopyToCpuCancel,
                                                f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);

            rv = _bcm_field_th_misc_cd_action_set(unit,
                                                bcmFieldActionYpCopyToCpuCancel,
                                                f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);

            rv = _bcm_field_th_misc_cd_action_set(unit,
                                                bcmFieldActionRpCopyToCpuCancel,
                                                f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);

	    rv = _bcm_field_th_misc_cd_action_set(unit,
                                                  bcmFieldActionGpDropCancel,
                                                  f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);

            rv = _bcm_field_th_misc_cd_action_set(unit,
                                                  bcmFieldActionYpDropCancel,
                                                  f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);

            rv = _bcm_field_th_misc_cd_action_set(unit,
                                                  bcmFieldActionRpDropCancel,
                                                  f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);
            break;
	case bcmFieldActionGpTimeStampToCpuCancel:
            rv = _bcm_field_th_misc_cd_action_set(unit,
                                                bcmFieldActionGpCopyToCpuCancel,
                                                f_ent, fa, entbuf);
	    BCM_IF_ERROR_RETURN(rv);

	    rv = _bcm_field_th_misc_cd_action_set(unit,
                                                  bcmFieldActionGpDropCancel,
                                                  f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);
            break;
	case bcmFieldActionYpTimeStampToCpuCancel:
            rv = _bcm_field_th_misc_cd_action_set(unit,
                                                bcmFieldActionYpCopyToCpuCancel,
                                                f_ent, fa, entbuf);
	    BCM_IF_ERROR_RETURN(rv);

	    rv = _bcm_field_th_misc_cd_action_set(unit,
                                                  bcmFieldActionYpDropCancel,
                                                  f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);
            break;
	case bcmFieldActionRpTimeStampToCpuCancel:
            rv = _bcm_field_th_misc_cd_action_set(unit,
                                                bcmFieldActionRpCopyToCpuCancel,
                                                f_ent, fa, entbuf);
	    BCM_IF_ERROR_RETURN(rv);

	    rv = _bcm_field_th_misc_cd_action_set(unit,
                                                  bcmFieldActionRpDropCancel,
                                                  f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);
            break;
        case bcmFieldActionPrioPktAndIntCopy:
        case bcmFieldActionPrioPktAndIntNew:
        case bcmFieldActionPrioPktAndIntTos:
        case bcmFieldActionPrioPktAndIntCancel:
            rv = _bcm_field_th_profile1_action_set(unit, 
                                               f_ent, fa, entbuf);

            BCM_IF_ERROR_RETURN(rv);
       
            rv = _bcm_field_th_profile2_action_set(unit,
                                               f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);
            break;
        default:
            return _bcm_field_th_misc_cd_action_set(unit, fa->action,
                                                    f_ent, fa, entbuf);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_field_th_action_set
 * Purpose:
 *     Set the actions in entbuf which is to be written into POLICY table.
 * Parameters:
 *     unit     - BCM device number
 *     mem      - Policy table memory
 *     f_ent    - entry structure to get policy info from
 *     tcam_idx - index into TCAM
 *     fa       - field action
 *     buf      - (OUT) Field Policy table entry
 * Returns:
 *     BCM_E_NONE
 *     BCM_E_PARAM - Action parameter out-of-range or unrecognized action.
 * Notes:
 *     This is a simple read/modify/write pattern.
 *     FP unit lock should be held by calling function.
 */
int
_bcm_field_th_action_set(int unit, soc_mem_t mem, _field_entry_t *f_ent,
                          int tcam_idx, _field_action_t *fa, uint32 *entbuf)
{
    int rv;

    if (NULL == f_ent || NULL == fa || NULL == entbuf) {
        LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META("Invalid Parameters\n")));
        return (BCM_E_PARAM);
    }

    switch (fa->action) {
        case bcmFieldActionCosQNew:
        case bcmFieldActionGpCosQNew:
        case bcmFieldActionYpCosQNew:
        case bcmFieldActionRpCosQNew:
        case bcmFieldActionUcastCosQNew:
        case bcmFieldActionGpUcastCosQNew:
        case bcmFieldActionYpUcastCosQNew:
        case bcmFieldActionRpUcastCosQNew:
        case bcmFieldActionMcastCosQNew:
        case bcmFieldActionGpMcastCosQNew:
        case bcmFieldActionYpMcastCosQNew:
        case bcmFieldActionRpMcastCosQNew:
        case bcmFieldActionPrioIntNew:
        case bcmFieldActionGpPrioIntNew:
        case bcmFieldActionYpPrioIntNew:
        case bcmFieldActionRpPrioIntNew:
        case bcmFieldActionDropPrecedence:
        case bcmFieldActionGpDropPrecedence:
        case bcmFieldActionYpDropPrecedence:
        case bcmFieldActionRpDropPrecedence:
        case bcmFieldActionPrioIntCopy:
        case bcmFieldActionPrioIntTos:
        case bcmFieldActionPrioIntCancel:
        case bcmFieldActionGpPrioIntCopy:
        case bcmFieldActionGpPrioIntTos:
        case bcmFieldActionGpPrioIntCancel:
        case bcmFieldActionYpPrioIntCopy:
        case bcmFieldActionYpPrioIntTos:
        case bcmFieldActionYpPrioIntCancel:
        case bcmFieldActionRpPrioIntCopy:
        case bcmFieldActionRpPrioIntTos:
        case bcmFieldActionRpPrioIntCancel:
            rv = _bcm_field_th_profile1_action_set(unit, f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);
            break;
        case bcmFieldActionEcnNew:
        case bcmFieldActionGpEcnNew:
        case bcmFieldActionYpEcnNew:
        case bcmFieldActionRpEcnNew:
        case bcmFieldActionPrioPktNew:
        case bcmFieldActionGpPrioPktNew:
        case bcmFieldActionYpPrioPktNew:
        case bcmFieldActionRpPrioPktNew:
        case bcmFieldActionGpTosPrecedenceNew:
        case bcmFieldActionDscpNew:
        case bcmFieldActionGpDscpNew:
        case bcmFieldActionYpDscpNew:
        case bcmFieldActionRpDscpNew:
        case bcmFieldActionPrioPktCopy:
        case bcmFieldActionGpPrioPktCopy:
        case bcmFieldActionYpPrioPktCopy:
        case bcmFieldActionRpPrioPktCopy:
        case bcmFieldActionPrioPktTos:
        case bcmFieldActionGpPrioPktTos:
        case bcmFieldActionYpPrioPktTos:
        case bcmFieldActionRpPrioPktTos:
        case bcmFieldActionPrioPktCancel:
        case bcmFieldActionGpPrioPktCancel:
        case bcmFieldActionYpPrioPktCancel:
        case bcmFieldActionRpPrioPktCancel:
        case bcmFieldActionDscpCancel:
        case bcmFieldActionGpDscpCancel:
        case bcmFieldActionYpDscpCancel:
        case bcmFieldActionRpDscpCancel:
        case bcmFieldActionGpTosPrecedenceCopy:
            rv = _bcm_field_th_profile2_action_set(unit, f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);
            break;
        case bcmFieldActionRedirect:
        case bcmFieldActionRedirectTrunk:
        case bcmFieldActionUnmodifiedPacketRedirectPort:
        case bcmFieldActionRedirectMcast:
        case bcmFieldActionRedirectEgrNextHop:
        case bcmFieldActionRedirectCancel:
        case bcmFieldActionRedirectPbmp:
        case bcmFieldActionRedirectVlan:
        case bcmFieldActionRedirectBcastPbmp:
        case bcmFieldActionRedirectIpmc:
        case bcmFieldActionEgressMask:
        case bcmFieldActionEgressPortsAdd:
            rv = _bcm_field_th_redirect_action_set(unit, f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);
            break;
        case bcmFieldActionEgressClassSelect:
        case bcmFieldActionHiGigClassSelect:
        case bcmFieldActionNewClassId:
        case bcmFieldActionMultipathHash:
        case bcmFieldActionAddClassTag:
        case bcmFieldActionFabricQueue:
        case bcmFieldActionL3Switch:
        case bcmFieldActionDstMacNew:
        case bcmFieldActionSrcMacNew:
        case bcmFieldActionOuterVlanNew:
        case bcmFieldActionVnTagNew:
        case bcmFieldActionVnTagDelete:
        case bcmFieldActionEtagNew:
        case bcmFieldActionEtagDelete:
        case bcmFieldActionL3SwitchCancel:
            rv = _bcm_field_th_l3swl2change_action_set(unit, f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);
            break;
        default:
            rv = _bcm_field_th_misc_action_set(unit, f_ent, fa, entbuf);
            BCM_IF_ERROR_RETURN(rv);
    }

    fa->flags &= ~_FP_ACTION_DIRTY; /* Mark action as installed. */

    return (BCM_E_NONE);
}

/*
 * Function:
 *    _field_qual_sec_info_t_init
 * Purpose:
 *    Initializes qualifier section information structure.
 * Parameters:
 *    ptr - (IN/OUT) Pointer to qualifier section information structure.
 * Returns:
 *    None
 */
STATIC void
_field_qual_sec_info_t_init(_field_qual_sec_info_t *ptr)
{
    int sel_idx; /* Iterator index. */

    /* Input parameter check. */
    if (NULL == ptr) {
        return;
    }

    /* Clear qualifier configuration structure. */
    sal_memset(ptr, 0, sizeof(_field_qual_sec_info_t));

    /* Initialize selectors to disabled. */
    for (sel_idx = 0; sel_idx < _FP_QUAL_MAX_CHUNKS; sel_idx++) {
        ptr->e_params[sel_idx].section = _FieldKeygenExtSelDisable;
    }

    /* Initialize control selector to disabled. */
    ptr->ctrl_sel = _FieldExtCtrlSelDisable;

    return;
}

/*
 * Function:
 *     _bcm_field_th_qual_sec_info_insert
 * Purpose:
 *     Insert a qualifier section information element to the list.
 * Parameters:
 *     unit     - (IN) BCM device number.
 *     stage_fc - (IN) Stage field control structure pointer.
 *     ptr      - (IN) Field qualifier section information structure pointer.
 * Returns:
 *    BCM_E_PARAM   - Invalid parameter.
 *    BCM_E_MEMORY  - Memory allocation failure.
 *    BCM_E_NONE    - Success.
 */
int
_bcm_field_th_qual_sec_info_insert(int unit, _field_stage_t *stage_fc,
                                   _field_qual_sec_info_t *ptr)
{
    _field_qual_sec_info_t *qual = NULL; /* Field qualifier section info. */
    bcm_field_qualify_t qid;

    /* Input parameters check. */
    if ((NULL == stage_fc) || (NULL == ptr)) {
        return (BCM_E_PARAM);
    }

    /* Get the qualifier identifier. */
    qid = ptr->qid;

    /* Get the qualifier information pointier. */
    qual = stage_fc->input_bus.qual_list[qid];
    if (NULL == qual) {
        _FP_XGS3_ALLOC(qual, sizeof(_field_qual_sec_info_t),
            "IFP qual section info");
        if (NULL == qual) {
            return (BCM_E_MEMORY);
        }

        /* Initialize qualifier section information structure. */
        _field_qual_sec_info_t_init(qual);
    
        /* Add new qualifier section information to the list. */
        stage_fc->input_bus.qual_list[qid] = qual;

    } else {

        /* Go to the end of qualifier section info's. */
        while(NULL != qual->next) {
           qual = qual->next;
        }

        _FP_XGS3_ALLOC(qual->next, sizeof(_field_qual_sec_info_t),
            "IFP qual section info");
        if (NULL == qual->next) {
            return (BCM_E_MEMORY);
        }
   
        qual = qual->next;
    }

    /* Copy qualifier section information to input bus list. */
    sal_memcpy(qual, ptr, sizeof(_field_qual_sec_info_t));

    /* Update input bus width and number of fields information. */
    stage_fc->input_bus.size += ptr->size;
    stage_fc->input_bus.num_fields++;

    LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
        "FP(unit %d) Verb: IBUS Qid=%d BusWidth=%d Num_Fields=%d.\n"),
        unit, qual->qid, stage_fc->input_bus.size,
        stage_fc->input_bus.num_fields));

    return (BCM_E_NONE);
}


/*
 * Function:
 *    _field_th_stage_quals_ibus_map_init
 * Purpose:
 *    Initialize Field Ingress Stage Qualifiers to their extractors mapping.
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    stage_fc   - (IN/OUT) Field Processor stage control structure.
 *
 * Returns:
 *    BCM_E_PARAM - Null field stage control structure.
 *    BCM_E_MEMORY - Allocation failure.
 *    BCM_E_INTERNAL - Invalid CAP Stage ID.
 *    BCM_E_NONE - Success.
 */
STATIC int
_field_th_stage_quals_ibus_map_init(int unit, _field_stage_t *stage_fc)
{
    _FP_QUAL_SEC_INFO_DECL;
    /* L0 BUS extractor sections offset:
     *    - 32bit extractor = 0
     *    - 16bit extractor = 608
     *    - 8bit extractor  = 1120
     *    - 4bit extractor  = 1280
     *    - 2bit extractor  = 1400
     */

    /* Input parameters check. */
    if (NULL == stage_fc) {
        return (BCM_E_PARAM);
    }

    /* Allocate stage qualifiers extraction mapping array. */
    _FP_XGS3_ALLOC(stage_fc->input_bus.qual_list,
        (_bcmFieldQualifyCount * sizeof(_field_qual_sec_info_t *)),
         "IFP qualifiers");
    if (NULL == stage_fc->input_bus.qual_list) {
        return (BCM_E_MEMORY);
    }

    /* Post MUX qualifiers initialization. */
    /* Not Intializing the SGPP,SGLP and SVP's as post mux qualifiers
	as these can be part of premuxer's and can use SRC_Container_A/B selectors*/
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyInPorts,
        _FieldKeygenExtSelDisable, 0, 36, 0);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyNatDstRealmId,
        _FieldKeygenExtSelDisable, 0, 2, 157);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyNatNeeded,
        _FieldKeygenExtSelDisable, 0, 1, 158);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyDrop,
        _FieldKeygenExtSelDisable, 0, 1, 159);

    /*Initailizing the offset values to 0 for _FieldExtCtrlSrcDestCont1Sel
	and _FieldExtCtrlSrcDestCont0Sel. Appropirate offsets will be updated during
	group creation depending on 80bit mode or 160bit mode*/

	/*SRC_DST_CONTAINER_0_SEL*/
	/*DGLP*/
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
        _FieldExtCtrlSrcDestCont0Sel, 4, _FieldKeygenExtSelDisable, 0,
        _FieldKeygenExtSelDisable, 0, 16,  1, 0, 0);
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstPort, _FieldExtCtrlSrcDestCont0Sel, 4,
        _FieldKeygenExtSelDisable, 0, _FieldKeygenExtSelDisable, 0, 16,  1, 0,
        0);
	/*DVP*/
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstMimGport, _FieldExtCtrlSrcDestCont0Sel, 5,
        _FieldKeygenExtSelDisable, 0, _FieldKeygenExtSelDisable, 0, 16,  1, 0,
        0);
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstMimGports, _FieldExtCtrlSrcDestCont0Sel, 5,
        _FieldKeygenExtSelDisable, 0, _FieldKeygenExtSelDisable, 0, 16,  1, 0,
        0);
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstNivGport, _FieldExtCtrlSrcDestCont0Sel, 5,
        _FieldKeygenExtSelDisable, 0, _FieldKeygenExtSelDisable, 0, 16,  1, 0,
        0);
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstNivGports, _FieldExtCtrlSrcDestCont0Sel, 5,
        _FieldKeygenExtSelDisable, 0, _FieldKeygenExtSelDisable, 0, 16,  1, 0,
        0);
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstVxlanGport, _FieldExtCtrlSrcDestCont0Sel, 5,
        _FieldKeygenExtSelDisable, 0, _FieldKeygenExtSelDisable, 0, 16,  1, 0,
        0);
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstVxlanGports, _FieldExtCtrlSrcDestCont0Sel, 5,
        _FieldKeygenExtSelDisable, 0, _FieldKeygenExtSelDisable, 0, 16,  1, 0,
        0);
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstVlanGport, _FieldExtCtrlSrcDestCont0Sel, 5,
        _FieldKeygenExtSelDisable, 0, _FieldKeygenExtSelDisable, 0, 16,  1, 0,
        0);
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstVlanGports, _FieldExtCtrlSrcDestCont0Sel, 5,
        _FieldKeygenExtSelDisable, 0, _FieldKeygenExtSelDisable, 0, 16,  1, 0,
        0);
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstMplsGport, _FieldExtCtrlSrcDestCont0Sel, 5,
        _FieldKeygenExtSelDisable, 0, _FieldKeygenExtSelDisable, 0, 16,  1, 0,
        0);
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstMplsGports, _FieldExtCtrlSrcDestCont0Sel, 5,
        _FieldKeygenExtSelDisable, 0, _FieldKeygenExtSelDisable, 0, 16,  1, 0,
        0);
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstWlanGport, _FieldExtCtrlSrcDestCont0Sel, 5,
        _FieldKeygenExtSelDisable, 0, _FieldKeygenExtSelDisable, 0, 16,  1, 0,
        0);
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstWlanGports, _FieldExtCtrlSrcDestCont0Sel, 5,
        _FieldKeygenExtSelDisable, 0, _FieldKeygenExtSelDisable, 0, 16,  1, 0,
        0);
	/*ECMP 1*/
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstMultipath, _FieldExtCtrlSrcDestCont0Sel, 7,
        _FieldKeygenExtSelDisable, 0, _FieldKeygenExtSelDisable, 0, 16,  1, 0,
        0);
	/*NEXT_HOP*/
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstL3EgressNextHops, _FieldExtCtrlSrcDestCont0Sel, 9,
        _FieldKeygenExtSelDisable, 0, _FieldKeygenExtSelDisable, 0, 16,  1, 0,
        0);
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstL3Egress, _FieldExtCtrlSrcDestCont0Sel, 9,
        _FieldKeygenExtSelDisable, 0, _FieldKeygenExtSelDisable, 0, 16,  1, 0,
        0);
	/*IPMC_GROUP :L3MC Group ID*/
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstMulticastGroup, _FieldExtCtrlSrcDestCont0Sel, 10,
        _FieldKeygenExtSelDisable, 0, _FieldKeygenExtSelDisable, 0, 16,  1, 0,
        0);
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstMulticastGroups, _FieldExtCtrlSrcDestCont0Sel, 10,
        _FieldKeygenExtSelDisable, 0, _FieldKeygenExtSelDisable, 0, 16,  1, 0,
        0);
	/*SRC_DST_CONTAINER_1_SEL*/
	/*DGLP*/
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
        _FieldExtCtrlSrcDestCont1Sel, 4, _FieldKeygenExtSelDisable, 0,
        _FieldKeygenExtSelDisable, 0, 16,  1, 0, 0);
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstPort, _FieldExtCtrlSrcDestCont1Sel, 4,
        _FieldKeygenExtSelDisable, 0, _FieldKeygenExtSelDisable, 0, 16,  1, 0,
        0);
	/*DVP*/
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstMimGport, _FieldExtCtrlSrcDestCont1Sel, 5,
        _FieldKeygenExtSelDisable, 0, _FieldKeygenExtSelDisable, 0, 16,  1, 0,
        0);
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstMimGports, _FieldExtCtrlSrcDestCont1Sel, 5,
        _FieldKeygenExtSelDisable, 0, _FieldKeygenExtSelDisable, 0, 16,  1, 0,
        0);
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstNivGport, _FieldExtCtrlSrcDestCont1Sel, 5,
        _FieldKeygenExtSelDisable, 0, _FieldKeygenExtSelDisable, 0, 16,  1, 0,
        0);
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstNivGports, _FieldExtCtrlSrcDestCont1Sel, 5,
        _FieldKeygenExtSelDisable, 0, _FieldKeygenExtSelDisable, 0, 16,  1, 0,
        0);
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstVxlanGport, _FieldExtCtrlSrcDestCont1Sel, 5,
        _FieldKeygenExtSelDisable, 0, _FieldKeygenExtSelDisable, 0, 16,  1, 0,
        0);
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstVxlanGports, _FieldExtCtrlSrcDestCont1Sel, 5,
        _FieldKeygenExtSelDisable, 0, _FieldKeygenExtSelDisable, 0, 16,  1, 0,
        0);
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstVlanGport, _FieldExtCtrlSrcDestCont1Sel, 5,
        _FieldKeygenExtSelDisable, 0, _FieldKeygenExtSelDisable, 0, 16,  1, 0,
        0);
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstVlanGports, _FieldExtCtrlSrcDestCont1Sel, 5,
        _FieldKeygenExtSelDisable, 0, _FieldKeygenExtSelDisable, 0, 16,  1, 0,
        0);
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstMplsGport, _FieldExtCtrlSrcDestCont1Sel, 5,
        _FieldKeygenExtSelDisable, 0, _FieldKeygenExtSelDisable, 0, 16,  1, 0,
        0);
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstMplsGports, _FieldExtCtrlSrcDestCont1Sel, 5,
        _FieldKeygenExtSelDisable, 0, _FieldKeygenExtSelDisable, 0, 16,  1, 0,
        0);
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstWlanGport, _FieldExtCtrlSrcDestCont1Sel, 5,
        _FieldKeygenExtSelDisable, 0, _FieldKeygenExtSelDisable, 0, 16,  1, 0,
        0);
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstWlanGports, _FieldExtCtrlSrcDestCont1Sel, 5,
        _FieldKeygenExtSelDisable, 0, _FieldKeygenExtSelDisable, 0, 16,  1, 0,
        0);
	/*ECMP 1*/
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstMultipath, _FieldExtCtrlSrcDestCont1Sel, 7,
        _FieldKeygenExtSelDisable, 0, _FieldKeygenExtSelDisable, 0, 16,  1, 0,
        0);
	/*IPMC_GROUP :L3MC Group ID*/
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstL3EgressNextHops, _FieldExtCtrlSrcDestCont1Sel, 9,
        _FieldKeygenExtSelDisable, 0, _FieldKeygenExtSelDisable, 0, 16,  1, 0,
        0);
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstL3Egress, _FieldExtCtrlSrcDestCont1Sel, 9,
        _FieldKeygenExtSelDisable, 0, _FieldKeygenExtSelDisable, 0, 16,  1, 0,
        0);
	/*IPMC_GROUP :L3MC Group ID*/
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstMulticastGroup, _FieldExtCtrlSrcDestCont1Sel, 10,
        _FieldKeygenExtSelDisable, 0, _FieldKeygenExtSelDisable, 0, 16,  1, 0,
        0);
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstMulticastGroups, _FieldExtCtrlSrcDestCont1Sel, 10,
        _FieldKeygenExtSelDisable, 0, _FieldKeygenExtSelDisable, 0, 16,  1, 0,
        0);

    /*
     *  L2 qualifiers.
     */
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyISid,
        _FieldKeygenExtSelL1E32, 4, 32, 96);
    _FP_QUAL_TWO_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyDstMac,
        _FieldKeygenExtSelL1E32, 3, _FieldKeygenExtSelL1E16, 31, 32, 16, 96,
        496);
    _FP_QUAL_TWO_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifySrcMac,
        _FieldKeygenExtSelL1E32, 2, _FieldKeygenExtSelL1E16, 30, 32, 16, 64,
        480);
    _FP_QUAL_TWO_SEC_INFO_ADD(unit, stage_fc,
        bcmFieldQualifyL2PayloadFirstEightBytes,
        _FieldKeygenExtSelL1E32, 17, _FieldKeygenExtSelL1E32, 18, 32, 32, 544,
        576);

    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
        _FieldKeygenExtSelL1E16, 25, 16, 400);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
        _FieldKeygenExtSelL1E16, 25, 16, 400);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
        _FieldKeygenExtSelL1E16, 25, 16, 400);
    _FP_QUAL_TWO_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
        _FieldKeygenExtSelL1E8, 4, _FieldKeygenExtSelL1E4, 5, 8, 4, 32,
        20);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyInnerVlan,
        _FieldKeygenExtSelL1E16, 26, 16, 416);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
        _FieldKeygenExtSelL1E16, 26, 16, 416);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
        _FieldKeygenExtSelL1E16, 26, 16, 416);
    _FP_QUAL_TWO_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
        _FieldKeygenExtSelL1E8, 3, _FieldKeygenExtSelL1E4, 4, 8, 4, 24,
        16);

    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
        _FieldKeygenExtSelL1E16, 29, 16, 464);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyMacAddrsNormalized,
        _FieldKeygenExtSelL1E2, 15, 2, 30);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyIpAddrsNormalized,
        _FieldKeygenExtSelL1E2, 15, 2, 30);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyL2Format,
        _FieldKeygenExtSelL1E2, 21, 2, 42);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
        _FieldKeygenExtSelL1E2, 22, 2, 44);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyTranslatedVlanFormat,
        _FieldKeygenExtSelL1E2, 23, 2, 46);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyOuterTpid,
        _FieldKeygenExtSelL1E2, 24, 2, 48);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyInnerTpid,
        _FieldKeygenExtSelL1E2, 25, 2, 50);

    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyVlanTranslationHit,
        _FieldKeygenExtSelL1E4, 10, 4, 40);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanValid,
        _FieldKeygenExtSelL1E4, 10, 4, 40);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyIngressStpState,
        _FieldKeygenExtSelL1E4, 11, 4, 44);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyL2SrcHit,
        _FieldKeygenExtSelL1E4, 11, 4, 46);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyL2SrcStatic,
        _FieldKeygenExtSelL1E4, 11, 4, 47);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyL2DestHit,
        _FieldKeygenExtSelL1E4, 12, 4, 48);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyL2CacheHit,
        _FieldKeygenExtSelL1E4, 12, 4, 48);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyL3SrcHostHit,
        _FieldKeygenExtSelL1E4, 12, 4, 48);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyIpmcStarGroupHit,
        _FieldKeygenExtSelL1E4, 12, 4, 48);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyL3DestHostHit,
        _FieldKeygenExtSelL1E4, 12, 4, 48);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyL3DestRouteHit,
        _FieldKeygenExtSelL1E4, 13, 4, 52);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyL2StationMove,
        _FieldKeygenExtSelL1E4, 13, 4, 52);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyDosAttack,
        _FieldKeygenExtSelL1E4, 13, 4, 52);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyIpTunnelHit,
        _FieldKeygenExtSelL1E4, 13, 4, 52);

    /* MPLS. */
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc,
        bcmFieldQualifyMplsForwardingLabelAction, _FieldKeygenExtSelL1E4,
        26, 4, 108);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyMplsForwardingLabel, _FieldExtCtrlAuxTagASel, 3,
        _FieldKeygenExtSelL1E32, 1, 32, 32);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyMplsForwardingLabelId, _FieldExtCtrlAuxTagASel, 3,
        _FieldKeygenExtSelL1E32, 1, 32, 32);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyMplsForwardingLabelTtl, _FieldExtCtrlAuxTagASel, 3,
        _FieldKeygenExtSelL1E16, 4, 16, 64);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyMplsForwardingLabelExp, _FieldExtCtrlAuxTagASel, 3,
        _FieldKeygenExtSelL1E16, 5, 16, 80);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyMplsControlWord, _FieldExtCtrlAuxTagASel, 4,
        _FieldKeygenExtSelL1E32, 1, 32, 32);
    /*
     *  L3 IPv6 qualifiers.
     */
    _FP_QUAL_FOUR_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifySrcIp6,
        _FieldKeygenExtSelL1E32, 11, _FieldKeygenExtSelL1E32, 12,
        _FieldKeygenExtSelL1E32, 13, _FieldKeygenExtSelL1E32, 14, 32, 32, 32,
        32, 352, 384, 416, 448);
    _FP_QUAL_TWO_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifySrcIp6Low,
        _FieldKeygenExtSelL1E32, 11, _FieldKeygenExtSelL1E32, 12, 32, 32, 352,
        384);
    _FP_QUAL_TWO_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifySrcIp6High,
        _FieldKeygenExtSelL1E32, 13, _FieldKeygenExtSelL1E32, 14, 32, 32, 416,
        448);
    _FP_QUAL_FOUR_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyDstIp6,
        _FieldKeygenExtSelL1E32, 15, _FieldKeygenExtSelL1E32, 16,
        _FieldKeygenExtSelL1E32, 17, _FieldKeygenExtSelL1E32, 18, 32, 32, 32,
        32, 480, 512, 544, 576);
    _FP_QUAL_TWO_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyDstIp6Low,
        _FieldKeygenExtSelL1E32, 15, _FieldKeygenExtSelL1E32, 16, 32, 32, 480,
        512);
    _FP_QUAL_TWO_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyDstIp6High,
        _FieldKeygenExtSelL1E32, 17, _FieldKeygenExtSelL1E32, 18, 32, 32, 544,
        576);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyExtensionHeader2Type,
        _FieldKeygenExtSelL1E8, 15, 8, 120);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyExtensionHeaderType,
        _FieldKeygenExtSelL1E8, 16, 8, 128);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyExtensionHeaderSubCode,
        _FieldKeygenExtSelL1E8, 17, 8, 136);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyIp6FlowLabel, _FieldExtCtrlAuxTagASel, 9,
        _FieldKeygenExtSelL1E32, 1, 32, 32);
    /*
     *  L3 IPv4 qualifiers.
     */
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
        _FieldKeygenExtSelL1E32, 11, 32, 352);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
        _FieldKeygenExtSelL1E32, 15, 32, 480);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
            bcmFieldQualifyTos, _FieldExtCtrlTosFnSel, 2,
            _FieldKeygenExtSelL1E8, 12, 8, 96);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
            bcmFieldQualifyTtl, _FieldExtCtrlTtlFnSel, 2,
            _FieldKeygenExtSelL1E8, 13, 8, 104);
    /* bcmFieldQualifyInnerIpProtocol == bcmFieldQualifyInnerIp6NextHeader. */
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyInnerIpProtocol,
        _FieldKeygenExtSelL1E8, 16, 8, 120);
    /* bcmFieldQualifyIpProtocol == bcmFieldQualifyIp6NextHeader. */
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
            _FieldKeygenExtSelL1E8, 16, 8, 128);
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
            _FieldExtCtrlTcpFnSel, 2,
            _FieldKeygenExtSelL1E4, 2,
            _FieldKeygenExtSelL1E4, 3,
            4, 4, 8, 12);
    _FP_QUAL_TWO_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyIpType,
        _FieldKeygenExtSelL1E4, 1, _FieldKeygenExtSelL1E2, 4, 4, 2, 4,
        8);
    /* Bitpos_33 maps to this qualifier in LO bus. */
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyRepCopy,
        _FieldKeygenExtSelL1E2, 5, 2, 10);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyIcmpError,
        _FieldKeygenExtSelL1E2, 5, 2, 10);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyDstIpLocal,
        _FieldKeygenExtSelL1E2, 16, 2, 32);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyIpFlags,
        _FieldKeygenExtSelL1E2, 18, 2, 36);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
        _FieldKeygenExtSelL1E2, 19, 2, 38);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyColor,
        _FieldKeygenExtSelL1E2, 26, 2, 52);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc,
        bcmFieldQualifyGenericAssociatedChannelLabelValid,
        _FieldKeygenExtSelL1E2, 27, 2, 54);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc,
        bcmFieldQualifyRouterAlertLabelValid,
        _FieldKeygenExtSelL1E2, 27, 2, 54);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc,
        bcmFieldQualifyRouterAlertLabelValid,
        _FieldKeygenExtSelL1E2, 27, 2, 55);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyMplsControlWordValid,
        _FieldKeygenExtSelL1E2, 28, 2, 56);

    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyIpInfo,
        _FieldKeygenExtSelL1E2, 29, 2, 58);

    /* L4 qualifiers. */
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
        _FieldKeygenExtSelL1E16, 27, 16, 432);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
        _FieldKeygenExtSelL1E16, 28, 16, 448);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyIcmpTypeCode,
        _FieldKeygenExtSelL1E16, 28, 16, 448);

    /*
     *  FCoE qualifiers.
     */
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyFibreChanVFTHopCount,
        _FieldKeygenExtSelL1E8, 13, 8, 104);
    /* FCoE Data Field Control Flags. */
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyFibreChanDFCtl,
        _FieldKeygenExtSelL1E8, 16, 8, 128);

    /* Switch - Port and Interface qualifiers. */
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyInPort,
        _FieldKeygenExtSelL1E8, 0, 8, 0);
    _FP_QUAL_THREE_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyVpn,
        _FieldKeygenExtSelL1E8, 1, _FieldKeygenExtSelL1E4, 0,
        _FieldKeygenExtSelL1E2, 1, 8, 4, 2, 8, 0, 2);
    _FP_QUAL_TWO_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyVrf,
        _FieldKeygenExtSelL1E8, 2, _FieldKeygenExtSelL1E4, 29, 8, 4, 16,
        116);

    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyInterfaceClassPort, _FieldExtCtrlClassIdContASel, 0,
        _FieldKeygenExtSelL1E16, 15, 16, 240);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyInterfaceClassL3, _FieldExtCtrlClassIdContASel, 1,
        _FieldKeygenExtSelL1E16, 15, 16, 240);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyInterfaceClassVPort, _FieldExtCtrlClassIdContASel, 2,
        _FieldKeygenExtSelL1E16, 15, 16, 240);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyInterfaceClassL2, _FieldExtCtrlClassIdContASel, 3,
        _FieldKeygenExtSelL1E16, 15, 16, 240);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstClassField, _FieldExtCtrlClassIdContASel, 4,
        _FieldKeygenExtSelL1E16, 15, 16, 240);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifySrcClassField, _FieldExtCtrlClassIdContASel, 5,
        _FieldKeygenExtSelL1E16, 15, 16, 240);

    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyInterfaceClassPort, _FieldExtCtrlClassIdContBSel, 0,
        _FieldKeygenExtSelL1E16, 14, 16, 224);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyInterfaceClassL3, _FieldExtCtrlClassIdContBSel, 1,
        _FieldKeygenExtSelL1E16, 14, 16, 224);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyInterfaceClassVPort, _FieldExtCtrlClassIdContBSel, 2,
        _FieldKeygenExtSelL1E16, 14, 16, 224);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyInterfaceClassL2, _FieldExtCtrlClassIdContBSel, 3,
        _FieldKeygenExtSelL1E16, 14, 16, 224);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstClassField, _FieldExtCtrlClassIdContBSel, 4,
        _FieldKeygenExtSelL1E16, 14, 16, 224);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifySrcClassField, _FieldExtCtrlClassIdContBSel, 5,
        _FieldKeygenExtSelL1E16, 14, 16, 224);
	/*Container_A_SEL*/
	/*SGPP +*/
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifySrcModPortGport,
        _FieldExtCtrlSrcContASel, 0, _FieldKeygenExtSelL1E16, 10,
        _FieldKeygenExtSelL1E2, 3, 16,  2, 160, 6);

    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifySrcModPortGports,
        _FieldExtCtrlSrcContASel, 0, _FieldKeygenExtSelL1E16, 10,
        _FieldKeygenExtSelL1E2, 3, 16,  2, 160, 6);

    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifySrcPort,
        _FieldExtCtrlSrcContASel, 0, _FieldKeygenExtSelL1E16, 10,
        _FieldKeygenExtSelL1E2, 3, 16,  2, 160, 6);

    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifySrcModuleGport,
        _FieldExtCtrlSrcContASel, 0, _FieldKeygenExtSelL1E16, 10,
        _FieldKeygenExtSelL1E2, 3, 16,  2, 160, 6);

    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifySrcTrunkMemberGport,
        _FieldExtCtrlSrcContASel, 0, _FieldKeygenExtSelL1E16, 10,
        _FieldKeygenExtSelL1E2, 3, 16,  2, 160, 6);
	/*SGPP -*/
	/*SGLP +*/
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
	bcmFieldQualifySrcTrunk,
        _FieldExtCtrlSrcContASel, 1, _FieldKeygenExtSelL1E16, 10,
        _FieldKeygenExtSelL1E2, 3, 16,  2, 160, 6);
	/*SGLP -*/
	/* SVP */
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifySrcMplsGport,
	_FieldExtCtrlSrcContASel, 2, _FieldKeygenExtSelL1E16, 10,
        _FieldKeygenExtSelL1E2, 3, 16,  2, 160, 6);

    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifySrcMplsGports,
	_FieldExtCtrlSrcContASel, 2, _FieldKeygenExtSelL1E16, 10,
        _FieldKeygenExtSelL1E2, 3, 16,  2, 160, 6);

    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifySrcMimGport,
	_FieldExtCtrlSrcContASel, 2, _FieldKeygenExtSelL1E16, 10,
        _FieldKeygenExtSelL1E2, 3, 16,  2, 160, 6);

    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifySrcMimGports,
	_FieldExtCtrlSrcContASel, 2, _FieldKeygenExtSelL1E16, 10,
        _FieldKeygenExtSelL1E2, 3, 16,  2, 160, 6);

    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifySrcWlanGport,
	_FieldExtCtrlSrcContASel, 2, _FieldKeygenExtSelL1E16, 10,
        _FieldKeygenExtSelL1E2, 3, 16,  2, 160, 6);

    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifySrcWlanGports,
	_FieldExtCtrlSrcContASel, 2, _FieldKeygenExtSelL1E16, 10,
        _FieldKeygenExtSelL1E2, 3, 16,  2, 160, 6);

    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifySrcVlanGport,
	_FieldExtCtrlSrcContASel, 2, _FieldKeygenExtSelL1E16, 10,
        _FieldKeygenExtSelL1E2, 3, 16,  2, 160, 6);

    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifySrcVlanGports,
	_FieldExtCtrlSrcContASel, 2, _FieldKeygenExtSelL1E16, 10,
        _FieldKeygenExtSelL1E2, 3, 16,  2, 160, 6);

    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifySrcVxlanGport,
	_FieldExtCtrlSrcContASel, 2, _FieldKeygenExtSelL1E16, 10,
        _FieldKeygenExtSelL1E2, 3, 16,  2, 160, 6);

    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifySrcVxlanGports,
	_FieldExtCtrlSrcContASel, 2, _FieldKeygenExtSelL1E16, 10,
        _FieldKeygenExtSelL1E2, 3, 16,  2, 160, 6);

    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifySrcNivGport,
	_FieldExtCtrlSrcContASel, 2, _FieldKeygenExtSelL1E16, 10,
        _FieldKeygenExtSelL1E2, 3, 16,  2, 160, 6);

    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifySrcNivGports,
	_FieldExtCtrlSrcContASel, 2, _FieldKeygenExtSelL1E16, 10,
        _FieldKeygenExtSelL1E2, 3, 16,  2, 160, 6);
	/*SVP - */
	/*Container_B_SEL*/
	/*SGPP +*/
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifySrcModPortGport,
        _FieldExtCtrlSrcContBSel, 0, _FieldKeygenExtSelL1E16, 11,
        _FieldKeygenExtSelL1E2, 3, 16,  2, 176, 6);

    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifySrcModPortGports,
        _FieldExtCtrlSrcContBSel, 0, _FieldKeygenExtSelL1E16, 11,
        _FieldKeygenExtSelL1E2, 3, 16,  2, 176, 6);

    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifySrcPort,
        _FieldExtCtrlSrcContBSel, 0, _FieldKeygenExtSelL1E16, 11,
        _FieldKeygenExtSelL1E2, 3, 16,  2, 176, 6);

    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifySrcModuleGport,
        _FieldExtCtrlSrcContBSel, 0, _FieldKeygenExtSelL1E16, 11,
        _FieldKeygenExtSelL1E2, 3, 16,  2, 176, 6);

    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifySrcTrunkMemberGport,
        _FieldExtCtrlSrcContBSel, 0, _FieldKeygenExtSelL1E16, 11,
        _FieldKeygenExtSelL1E2, 3, 16,  2, 176, 6);

	/*SGPP -*/
	/*SGLP +*/

    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
	bcmFieldQualifySrcTrunk,
        _FieldExtCtrlSrcContBSel, 1, _FieldKeygenExtSelL1E16, 11,
        _FieldKeygenExtSelL1E2, 3, 16,  2, 176, 6);

	/*SGLP -*/
	/* SVP */
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifySrcMplsGport,
	_FieldExtCtrlSrcContBSel, 2, _FieldKeygenExtSelL1E16, 11,
        _FieldKeygenExtSelL1E2, 3, 16,  2, 176, 6);

    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifySrcMplsGports,
	_FieldExtCtrlSrcContBSel, 2, _FieldKeygenExtSelL1E16, 11,
        _FieldKeygenExtSelL1E2, 3, 16,  2, 176, 6);

    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifySrcMimGport,
	_FieldExtCtrlSrcContBSel, 2, _FieldKeygenExtSelL1E16, 11,
        _FieldKeygenExtSelL1E2, 3, 16,  2, 176, 6);

    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifySrcMimGports,
	_FieldExtCtrlSrcContBSel, 2, _FieldKeygenExtSelL1E16, 11,
        _FieldKeygenExtSelL1E2, 3, 16,  2, 176, 6);

    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifySrcWlanGport,
	_FieldExtCtrlSrcContBSel, 2, _FieldKeygenExtSelL1E16, 11,
        _FieldKeygenExtSelL1E2, 3, 16,  2, 176, 6);

    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifySrcWlanGports,
	_FieldExtCtrlSrcContBSel, 2, _FieldKeygenExtSelL1E16, 11,
        _FieldKeygenExtSelL1E2, 3, 16,  2, 176, 6);

    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifySrcVlanGport,
	_FieldExtCtrlSrcContBSel, 2, _FieldKeygenExtSelL1E16, 11,
        _FieldKeygenExtSelL1E2, 3, 16,  2, 176, 6);

    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifySrcVlanGports,
	_FieldExtCtrlSrcContBSel, 2, _FieldKeygenExtSelL1E16, 11,
        _FieldKeygenExtSelL1E2, 3, 16,  2, 176, 6);

    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifySrcVxlanGport,
	_FieldExtCtrlSrcContBSel, 2, _FieldKeygenExtSelL1E16, 11,
        _FieldKeygenExtSelL1E2, 3, 16,  2, 176, 6);

    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifySrcVxlanGports,
	_FieldExtCtrlSrcContBSel, 2, _FieldKeygenExtSelL1E16, 11,
        _FieldKeygenExtSelL1E2, 3, 16,  2, 176, 6);

    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifySrcNivGport,
	_FieldExtCtrlSrcContBSel, 2, _FieldKeygenExtSelL1E16, 11,
        _FieldKeygenExtSelL1E2, 3, 16,  2, 176, 6);

    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifySrcNivGports,
	_FieldExtCtrlSrcContBSel, 2, _FieldKeygenExtSelL1E16, 11,
        _FieldKeygenExtSelL1E2, 3, 16,  2, 176, 6);
	/*SVP - */

    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifySrcClassL2, _FieldExtCtrlClassIdContCSel, 0,
        _FieldKeygenExtSelL1E16, 13, 16, 208);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstClassL2, _FieldExtCtrlClassIdContCSel, 1,
        _FieldKeygenExtSelL1E16, 13, 16, 208);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifySrcClassL3, _FieldExtCtrlClassIdContCSel, 2,
        _FieldKeygenExtSelL1E16, 13, 16, 208);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstClassL3, _FieldExtCtrlClassIdContCSel, 3,
        _FieldKeygenExtSelL1E16, 13, 16, 208);

    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifySrcClassL2, _FieldExtCtrlClassIdContDSel, 0,
        _FieldKeygenExtSelL1E16, 12, 16, 192);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstClassL2, _FieldExtCtrlClassIdContDSel, 1,
        _FieldKeygenExtSelL1E16, 12, 16, 192);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifySrcClassL3, _FieldExtCtrlClassIdContDSel, 2,
        _FieldKeygenExtSelL1E16, 12, 16, 192);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyDstClassL3, _FieldExtCtrlClassIdContDSel, 3,
        _FieldKeygenExtSelL1E16, 12, 16, 192);

    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifySrcVirtualPortValid,
        _FieldKeygenExtSelL1E2, 10, 2, 20);

    /* Meta data qualifiers. */
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanId,
        _FieldKeygenExtSelL1E16, 17, 16, 272);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyL3Ingress,
        _FieldKeygenExtSelL1E16, 18, 16, 288);
    _FP_QUAL_TWO_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyRangeCheck,
        _FieldKeygenExtSelL1E16, 23, _FieldKeygenExtSelL1E16, 24, 16, 16, 368,
        384);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyPacketRes,
        _FieldKeygenExtSelL1E8, 11, 8, 88);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyTunnelType,
        _FieldKeygenExtSelL1E8, 10, 8, 80);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyLoopbackType,
        _FieldKeygenExtSelL1E8, 10, 8, 80);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyMplsLabel1Hit,
        _FieldKeygenExtSelL1E4, 14, 4, 56);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyTrillEgressRbridgeHit,
        _FieldKeygenExtSelL1E4, 14, 4, 56);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyL2GreSrcIpHit,
        _FieldKeygenExtSelL1E4, 14, 4, 56);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyMimSrcGportHit,
        _FieldKeygenExtSelL1E4, 14, 4, 56);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyMplsLabel2Hit,
        _FieldKeygenExtSelL1E4, 14, 4, 56);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyTrillIngressRbridgeHit,
        _FieldKeygenExtSelL1E4, 14, 4, 56);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyL2GreVfiHit,
        _FieldKeygenExtSelL1E4, 14, 4, 56);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyMimVfiHit,
        _FieldKeygenExtSelL1E4, 14, 4, 56);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyMplsTerminated,
        _FieldKeygenExtSelL1E4, 14, 4, 56);

    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyForwardingType,
        _FieldKeygenExtSelL1E4, 15, 4, 60);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyIntPriority,
        _FieldKeygenExtSelL1E4, 27, 4, 108);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyMHOpcode,
        _FieldKeygenExtSelL1E4, 28, 4, 112);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyHiGigProxy,
        _FieldKeygenExtSelL1E2, 7, 2, 14);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyL4Ports,
        _FieldKeygenExtSelL1E2, 8, 2, 16);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyMirrorCopy,
        _FieldKeygenExtSelL1E2, 20, 2, 40);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
        _FieldKeygenExtSelL1E2, 20, 2, 40);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyNatSrcRealmId,
        _FieldKeygenExtSelL1E2, 0, 2, 0);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc,
        bcmFieldQualifyIntCongestionNotification,
        _FieldKeygenExtSelL1E2, 9, 2, 18);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyMyStationHit,
        _FieldKeygenExtSelL1E2, 17, 2, 34);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
        _FieldKeygenExtSelL1E2, 17, 2, 34);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyRtag7AHashUpper, _FieldExtCtrlAuxTagDSel, 3,
        _FieldKeygenExtSelL1E16, 0, 16, 0);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyRtag7AHashLower, _FieldExtCtrlAuxTagDSel, 4,
        _FieldKeygenExtSelL1E16, 0, 16, 0);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyRtag7BHashUpper, _FieldExtCtrlAuxTagDSel, 5,
        _FieldKeygenExtSelL1E16, 0, 16, 0);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyRtag7BHashLower, _FieldExtCtrlAuxTagDSel, 6,
        _FieldKeygenExtSelL1E16, 0, 16, 0);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyRtag7AHashUpper, _FieldExtCtrlAuxTagCSel, 3,
        _FieldKeygenExtSelL1E16, 1, 16, 16);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyRtag7AHashLower, _FieldExtCtrlAuxTagCSel, 4,
        _FieldKeygenExtSelL1E16, 1, 16, 16);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyRtag7BHashUpper, _FieldExtCtrlAuxTagCSel, 5,
        _FieldKeygenExtSelL1E16, 1, 16, 16);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyRtag7BHashLower, _FieldExtCtrlAuxTagCSel, 6,
        _FieldKeygenExtSelL1E16, 1, 16, 16);

    /* VXLAN qualifiers. */
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyVxlanNetworkId,
        _FieldKeygenExtSelL1E32, 4, 32, 128);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyVxlanFlags,
        _FieldKeygenExtSelL1E32, 4, 32, 128);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyVnTag, _FieldExtCtrlAuxTagASel, 0,
        _FieldKeygenExtSelL1E32, 0, 32, 32);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyCnTag, _FieldExtCtrlAuxTagASel, 1,
        _FieldKeygenExtSelL1E32, 1, 32, 32);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
        bcmFieldQualifyFabricQueueTag, _FieldExtCtrlAuxTagASel, 2,
        _FieldKeygenExtSelL1E32, 1, 32, 32);

    /* Class Qualifiers */
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyFcoeRxID,
            _FieldKeygenExtSelL1E16, 27, 16, 432);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyFcoeOxID,
            _FieldKeygenExtSelL1E16, 28, 16, 448);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyL4DstPortClass,
            _FieldKeygenExtSelL1E4, 16, 4, 64);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyFcoeRxIDClass,
            _FieldKeygenExtSelL1E4, 16, 4, 64);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPortClass,
            _FieldKeygenExtSelL1E4, 17, 4, 68);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyFcoeOxIDClass,
            _FieldKeygenExtSelL1E4, 17, 4, 68);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyEtherTypeClass,
            _FieldKeygenExtSelL1E4, 18, 4, 72);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyIpProtocolClass,
            _FieldKeygenExtSelL1E4, 19, 4, 76);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifySrcIpClassMsbNibble,
            _FieldKeygenExtSelL1E4, 20, 4, 80);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifySrcIp6ClassMsbNibble,
            _FieldKeygenExtSelL1E4, 20, 4, 80);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyFibreChanSrcIdClassMsbNibble,
            _FieldKeygenExtSelL1E4, 20, 4, 80);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyDstIpClassMsbNibble,
            _FieldKeygenExtSelL1E4, 21, 4, 84);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyDstIp6ClassMsbNibble,
            _FieldKeygenExtSelL1E4, 21, 4, 84);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyFibreChanDstIdClassMsbNibble,
            _FieldKeygenExtSelL1E4, 21, 4, 84);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyDstIpClassLower,
            _FieldKeygenExtSelL1E16, 19, 16, 304);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyDstIp6ClassLower,
            _FieldKeygenExtSelL1E16, 19, 16, 304);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyFibreChanDstIdClassLower,
            _FieldKeygenExtSelL1E16, 19, 16, 304);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyDstIpClassUpper,
            _FieldKeygenExtSelL1E16, 20, 16, 320);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyDstIp6ClassUpper,
            _FieldKeygenExtSelL1E16, 20, 16, 320);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyFibreChanDstIdClassUpper,
            _FieldKeygenExtSelL1E16, 20, 16, 320);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifySrcIpClassLower,
            _FieldKeygenExtSelL1E16, 21, 16, 336);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifySrcIp6ClassLower,
            _FieldKeygenExtSelL1E16, 21, 16, 336);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyFibreChanSrcIdClassLower,
            _FieldKeygenExtSelL1E16, 21, 16, 336);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifySrcIpClassUpper,
            _FieldKeygenExtSelL1E16, 22, 16, 352);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifySrcIp6ClassUpper,
            _FieldKeygenExtSelL1E16, 22, 16, 352);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyFibreChanSrcIdClassUpper,
            _FieldKeygenExtSelL1E16, 22, 16, 352);
    _FP_QUAL_THREE_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifySrcIpClass,
            _FieldKeygenExtSelL1E16, 21, _FieldKeygenExtSelL1E16, 22,
            _FieldKeygenExtSelL1E4, 20, 16, 16, 4, 336, 352, 80);
    _FP_QUAL_THREE_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifySrcIp6Class,
            _FieldKeygenExtSelL1E16, 21, _FieldKeygenExtSelL1E16, 22,
            _FieldKeygenExtSelL1E4, 20, 16, 16, 4, 336, 352, 80);
    _FP_QUAL_THREE_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyFibreChanSrcIdClass,
            _FieldKeygenExtSelL1E16, 21, _FieldKeygenExtSelL1E16, 22,
            _FieldKeygenExtSelL1E4, 20, 16, 16, 4, 336, 352, 80);
    _FP_QUAL_THREE_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyDstIpClass,
            _FieldKeygenExtSelL1E16, 19, _FieldKeygenExtSelL1E16, 20,
            _FieldKeygenExtSelL1E4, 21, 16, 16, 4, 304, 320, 84);
    _FP_QUAL_THREE_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyDstIp6Class,
            _FieldKeygenExtSelL1E16, 19, _FieldKeygenExtSelL1E16, 20,
            _FieldKeygenExtSelL1E4, 21, 16, 16, 4, 304, 320, 84);
    _FP_QUAL_THREE_SEC_INFO_ADD(unit, stage_fc, bcmFieldQualifyFibreChanDstIdClass,
            _FieldKeygenExtSelL1E16, 19, _FieldKeygenExtSelL1E16, 20,
            _FieldKeygenExtSelL1E4, 21, 16, 16, 4, 304, 320, 84);
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc, bcmFieldQualifyTcpClassZero,
            _FieldExtCtrlTcpFnSel, 0,
            _FieldKeygenExtSelL1E4, 2,
            _FieldKeygenExtSelL1E4, 3,
            4, 4, 8, 12);
    _FP_QUAL_TWO_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc, bcmFieldQualifyTcpClassOne,
            _FieldExtCtrlTcpFnSel, 1,
            _FieldKeygenExtSelL1E4, 2,
            _FieldKeygenExtSelL1E4, 3,
            4, 4, 8, 12);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
            bcmFieldQualifyTosClassZero, _FieldExtCtrlTosFnSel, 0,
            _FieldKeygenExtSelL1E8, 12, 8, 96);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
            bcmFieldQualifyTosClassOne, _FieldExtCtrlTosFnSel, 1,
            _FieldKeygenExtSelL1E8, 12, 8, 96);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
            bcmFieldQualifyTtlClassZero, _FieldExtCtrlTtlFnSel, 0,
            _FieldKeygenExtSelL1E8, 13, 8, 104);
    _FP_QUAL_SEC_INFO_CTRL_SEL_ADD(unit, stage_fc,
            bcmFieldQualifyTtlClassOne, _FieldExtCtrlTtlFnSel, 1,
            _FieldKeygenExtSelL1E8, 13, 8, 104);

    /* UDF1 and UDF2 qualifiers. */
    
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, _bcmFieldQualifyData0,
        _FieldKeygenExtSelL1E16, 9, 16, 144);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, _bcmFieldQualifyData1,
        _FieldKeygenExtSelL1E16, 8, 16, 128);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, _bcmFieldQualifyData2,
        _FieldKeygenExtSelL1E32, 10, 32, 320);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, _bcmFieldQualifyData3,
        _FieldKeygenExtSelL1E32, 9, 32, 288);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, _bcmFieldQualifyData4,
        _FieldKeygenExtSelL1E32, 8, 32, 256);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, _bcmFieldQualifyData5,
        _FieldKeygenExtSelL1E16, 7, 16, 112);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, _bcmFieldQualifyData6,
        _FieldKeygenExtSelL1E16, 6, 16, 96);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, _bcmFieldQualifyData7,
        _FieldKeygenExtSelL1E32, 7, 32, 224);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, _bcmFieldQualifyData8,
        _FieldKeygenExtSelL1E32, 6, 32, 192);
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, _bcmFieldQualifyData9,
        _FieldKeygenExtSelL1E32, 5, 32, 160);


    /* Preselector Logical Table ID Qualifier. */
    _FP_QUAL_SEC_INFO_ADD(unit, stage_fc, _bcmFieldQualifyPreLogicalTableId,
        _FieldKeygenExtSelL1E8, 5, 8, 40);
    return (BCM_E_NONE);
}

/*
 * Function:
 *    _field_th_ingress_qualifiers_init
 *
 * Purpose:
 *    Initialize Field Ingress Stage Qualifiers extractors and offsets.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    stage_fc   - (IN/OUT) Field Processor stage control structure.
 *
 * Returns:
 *    BCM_E_PARAM       - Null field stage control structure.
 *    BCM_E_MEMORY      - Allocation failure.
 *    BCM_E_INTERNAL    - Invalid CAP Stage ID.
 *    BCM_E_NONE        - Success.
 */
STATIC int
_field_th_ingress_qualifiers_init(int unit, _field_stage_t *stage_fc)
{
    _FP_QUAL_DECL;

    /* Allocated stage qualifiers configuration array. */
    _FP_XGS3_ALLOC(stage_fc->f_qual_arr,
        (_bcmFieldQualifyCount * sizeof(_bcm_field_qual_info_t *)),
        "IFP qualifiers");
    if (stage_fc->f_qual_arr == 0) {
        return (BCM_E_MEMORY);
    }

    /* Pseudo Qualifiers. */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyStage,
        _bcmFieldSliceSelDisable, 0, 0, 0);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyStageIngress,
        _bcmFieldSliceSelDisable, 0, 0, 0);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp4,
        _bcmFieldSliceSelDisable, 0, 0, 0);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp6,
        _bcmFieldSliceSelDisable, 0, 0, 0);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyNormalizeIpAddrs,
        _bcmFieldSliceSelDisable, 0, 0, 0);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyNormalizeMacAddrs,
        _bcmFieldSliceSelDisable, 0, 0, 0);

    /* Post muxed qualifiers. */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPorts,
        _bcmFieldSliceSelDisable, 0, 0, 36);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyNatDstRealmId,
        _bcmFieldSliceSelDisable, 0, 156, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyNatNeeded,
        _bcmFieldSliceSelDisable, 0, 158, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
        _bcmFieldSliceSelDisable, 0, 159, 1);

    /*Initailizing the offset values for below postmux qualifiers.
       Appropirate offsets will be updated during
       group creation depending on 80bit mode or 160bit mode*/
	/*DGLP*/
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldDevSelDisable, 0,
                     0,
                     0, 16,
                     0, 1,
                     0, 0,
                     0);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldDevSelDisable, 0,
                     0,
                     0, 16,
                     0, 1,
                     0, 0,
                     0);
	/*DVP*/
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstMplsGport,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldDevSelDisable, 0,
                     0,
                     0, 16,
                     0, 1,
                     0, 0,
                     0);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstMplsGports,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldDevSelDisable, 0,
                     0,
                     0, 16,
                     0, 1,
                     0, 0,
                     0);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstMimGport,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldDevSelDisable, 0,
                     0,
                     0, 16,
                     0, 1,
                     0, 0,
                     0);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstMimGports,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldDevSelDisable, 0,
                     0,
                     0, 16,
                     0, 1,
                     0, 0,
                     0);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstWlanGport,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldDevSelDisable, 0,
                     0,
                     0, 16,
                     0, 1,
                     0, 0,
                     0);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstWlanGports,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldDevSelDisable, 0,
                     0,
                     0, 16,
                     0, 1,
                     0, 0,
                     0);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstVlanGport,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldDevSelDisable, 0,
                     0,
                     0, 16,
                     0, 1,
                     0, 0,
                     0);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstVlanGports,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldDevSelDisable, 0,
                     0,
                     0, 16,
                     0, 1,
                     0, 0,
                     0);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstVxlanGport,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldDevSelDisable, 0,
                     0,
                     0, 16,
                     0, 1,
                     0, 0,
                     0);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstVxlanGports,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldDevSelDisable, 0,
                     0,
                     0, 16,
                     0, 1,
                     0, 0,
                     0);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstNivGport,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldDevSelDisable, 0,
                     0,
                     0, 16,
                     0, 1,
                     0, 0,
                     0);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstNivGports,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldDevSelDisable, 0,
                     0,
                     0, 16,
                     0, 1,
                     0, 0,
                     0);
	/*DVP - */
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstL3EgressNextHops,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldDevSelDisable, 0,
                     0,
                     0, 16,
                     0, 1,
                     0, 0,
                     0);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstMultipath,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldDevSelDisable, 0,
                     0,
                     0, 16,
                     0, 1,
                     0, 0,
                     0);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstL3Egress,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldDevSelDisable, 0,
                     0,
                     0, 16,
                     0, 1,
                     0, 0,
                     0);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstMulticastGroup,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldDevSelDisable, 0,
                     0,
                     0, 16,
                     0, 1,
                     0, 0,
                     0);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstMulticastGroups,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldDevSelDisable, 0,
                     0,
                     0, 16,
                     0, 1,
                     0, 0,
                     0);

    /* L2 qualifiers - 48 bits section. */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstMac,
        _bcmFieldSliceSelDisable, 0, 0, 48);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMac,
        _bcmFieldSliceSelDisable, 0, 0, 48);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2PayloadFirstEightBytes,
        _bcmFieldSliceSelDisable, 0, 0, 64);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyISid,
        _bcmFieldSliceSelDisable, 0, 0, 24);

    /* L2 qualifiers - 16 bits section. */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
        _bcmFieldSliceSelDisable, 0, 0, 16);
    _FP_QUAL_MULTI_INFO_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
        _bcmFieldSliceSelDisable, 0, 0, 16, 1, 12);
    _FP_QUAL_MULTI_INFO_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
        _bcmFieldSliceSelDisable, 0, 0, 16, 3, 13);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
        _bcmFieldSliceSelDisable, 0, 0, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlan,
        _bcmFieldSliceSelDisable, 0, 0, 16);
    _FP_QUAL_MULTI_INFO_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
        _bcmFieldSliceSelDisable, 0, 0, 16, 1, 12);
    _FP_QUAL_MULTI_INFO_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
        _bcmFieldSliceSelDisable, 0, 0, 16, 3, 13);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
        _bcmFieldSliceSelDisable, 0, 0, 12);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
        _bcmFieldSliceSelDisable, 0, 0, 16);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMacAddrsNormalized,
        _bcmFieldSliceSelDisable, 0, 0, 1);
    _FP_QUAL_MULTI_INFO_ADD(unit, stage_fc, bcmFieldQualifyIpAddrsNormalized,
        _bcmFieldSliceSelDisable, 0, 1, 2, 1, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2Format,
        _bcmFieldSliceSelDisable, 0, 0, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
        _bcmFieldSliceSelDisable, 0, 0, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTranslatedVlanFormat,
        _bcmFieldSliceSelDisable, 0, 0, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterTpid,
        _bcmFieldSliceSelDisable, 0, 0, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerTpid,
        _bcmFieldSliceSelDisable, 0, 0, 2);

    _FP_QUAL_MULTI_INFO_ADD(unit, stage_fc, bcmFieldQualifyVlanTranslationHit,
        _bcmFieldSliceSelDisable, 0, 0, 4, 3, 0);
    _FP_QUAL_MULTI_INFO_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanValid,
        _bcmFieldSliceSelDisable, 0, 3, 4, 1, 3);
    _FP_QUAL_MULTI_INFO_ADD(unit, stage_fc, bcmFieldQualifyIngressStpState,
        _bcmFieldSliceSelDisable, 0, 0, 4, 2, 0);
    _FP_QUAL_MULTI_INFO_ADD(unit, stage_fc, bcmFieldQualifyL2SrcHit,
        _bcmFieldSliceSelDisable, 0, 0, 4, 1, 2);
    _FP_QUAL_MULTI_INFO_ADD(unit, stage_fc, bcmFieldQualifyL2SrcStatic,
        _bcmFieldSliceSelDisable, 0, 0, 4, 1, 3);
    _FP_QUAL_MULTI_INFO_ADD(unit, stage_fc, bcmFieldQualifyL2DestHit,
        _bcmFieldSliceSelDisable, 0, 0, 4, 1, 0);
    _FP_QUAL_MULTI_INFO_ADD(unit, stage_fc, bcmFieldQualifyL2CacheHit,
        _bcmFieldSliceSelDisable, 0, 1, 4, 1, 1);
    _FP_QUAL_MULTI_INFO_ADD(unit, stage_fc, bcmFieldQualifyL3SrcHostHit,
        _bcmFieldSliceSelDisable, 0, 2, 4, 1, 2);
    _FP_QUAL_MULTI_INFO_ADD(unit, stage_fc, bcmFieldQualifyIpmcStarGroupHit,
        _bcmFieldSliceSelDisable, 0, 2, 4, 1, 2);
    _FP_QUAL_MULTI_INFO_ADD(unit, stage_fc, bcmFieldQualifyL3DestHostHit,
        _bcmFieldSliceSelDisable, 0, 3, 4, 1, 3);
    _FP_QUAL_MULTI_INFO_ADD(unit, stage_fc, bcmFieldQualifyL3DestRouteHit,
        _bcmFieldSliceSelDisable, 0, 0, 4, 1, 0);
    _FP_QUAL_MULTI_INFO_ADD(unit, stage_fc, bcmFieldQualifyL2StationMove,
        _bcmFieldSliceSelDisable, 0, 1, 4, 1, 1);
    _FP_QUAL_MULTI_INFO_ADD(unit, stage_fc, bcmFieldQualifyDosAttack,
        _bcmFieldSliceSelDisable, 0, 2, 4, 1, 2);
    _FP_QUAL_MULTI_INFO_ADD(unit, stage_fc, bcmFieldQualifyIpTunnelHit,
        _bcmFieldSliceSelDisable, 0, 3, 4, 1, 3);

    /* MPLS */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsForwardingLabelAction,
        _bcmFieldSliceSelDisable, 0, 0, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsForwardingLabel,
        _bcmFieldSliceSelDisable, 0, 0, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsForwardingLabelId,
        _bcmFieldSliceSelDisable, 0, 12, 21);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsForwardingLabelTtl,
        _bcmFieldSliceSelDisable, 0, 0, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsForwardingLabelExp,
        _bcmFieldSliceSelDisable, 0, 0, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsControlWord,
        _bcmFieldSliceSelDisable, 0, 0, 32);

    /* L3 IPv6 qualifiers - 128 bits section. */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp6,
        _bcmFieldSliceSelDisable, 0, 0, 128);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6,
        _bcmFieldSliceSelDisable, 0, 0, 128);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp6High,
        _bcmFieldSliceSelDisable, 0, 0, 64);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6High,
        _bcmFieldSliceSelDisable, 0, 0, 64);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyExtensionHeader2Type,
        _bcmFieldSliceSelDisable, 0, 0, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyExtensionHeaderType,
        _bcmFieldSliceSelDisable, 0, 0, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyExtensionHeaderSubCode,
        _bcmFieldSliceSelDisable, 0, 0, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp6FlowLabel,
        _bcmFieldSliceSelDisable, 0, 0, 20);

    /* L3 IPv4 qualifiers - 32 bits section. */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
        _bcmFieldSliceSelDisable, 0, 0, 32);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
        _bcmFieldSliceSelDisable, 0, 0, 32);

    /* L3 IPv4 qualifiers - 16 bits section. */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
        _bcmFieldSliceSelDisable, 0, 0, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
        _bcmFieldSliceSelDisable, 0, 0, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIcmpTypeCode,
        _bcmFieldSliceSelDisable, 0, 0, 16);

    /* L3 IPv4 qualifiers - 8 bits section. */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerIpProtocol,
        _bcmFieldSliceSelDisable, 0, 0, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
        _bcmFieldSliceSelDisable, 0, 0, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
        _bcmFieldSliceSelDisable, 0, 0, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTos,
        _bcmFieldSliceSelDisable, 0, 0, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
        _bcmFieldSliceSelDisable, 0, 0, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
        _bcmFieldSliceSelDisable, 0, 0, 5);
    /* L3 qualifiers - 1 bit section. */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyRepCopy,
        _bcmFieldSliceSelDisable, 0, 1, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIcmpError,
        _bcmFieldSliceSelDisable, 0, 0, 1);
    _FP_QUAL_MULTI_INFO_ADD(unit, stage_fc, bcmFieldQualifyDstIpLocal,
        _bcmFieldSliceSelDisable, 0, 0, 2, 1, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpFlags,
        _bcmFieldSliceSelDisable, 0, 0, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
        _bcmFieldSliceSelDisable, 0, 0, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyColor,
        _bcmFieldSliceSelDisable, 0, 0, 2);
    _FP_QUAL_ADD(unit, stage_fc,
        bcmFieldQualifyGenericAssociatedChannelLabelValid,
        _bcmFieldSliceSelDisable, 0, 0, 1);
    _FP_QUAL_ADD(unit, stage_fc,
        bcmFieldQualifyRouterAlertLabelValid,
        _bcmFieldSliceSelDisable, 0, 1, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsControlWordValid,
        _bcmFieldSliceSelDisable, 0, 0, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpInfo,
        _bcmFieldSliceSelDisable, 0, 0, 1);

    /* Switch - Port and Interface qualifiers. */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPort,
        _bcmFieldSliceSelDisable, 0, 0, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVpn,
        _bcmFieldSliceSelDisable, 0, 0, 14);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
        _bcmFieldSliceSelDisable, 0, 0, 11);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
        _bcmFieldSliceSelDisable, 0, 0, 10);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
        _bcmFieldSliceSelDisable, 0, 0, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL3,
        _bcmFieldSliceSelDisable, 0, 0, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassVPort,
        _bcmFieldSliceSelDisable, 0, 0, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL2,
        _bcmFieldSliceSelDisable, 0, 0, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
        _bcmFieldSliceSelDisable, 0, 0, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
        _bcmFieldSliceSelDisable, 0, 0, 16);
	/*SGPP*/
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcPort,
        _bcmFieldSliceSelDisable, 0, 0, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcModPortGport,
        _bcmFieldSliceSelDisable, 0, 0, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcModPortGports,
        _bcmFieldSliceSelDisable, 0, 0, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcModuleGport,
        _bcmFieldSliceSelDisable, 0, 0, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcTrunkMemberGport,
        _bcmFieldSliceSelDisable, 0, 0, 16);
	/*SGLP*/
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcTrunk,
        _bcmFieldSliceSelDisable, 0, 0, 16);
	/*SVP*/
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMplsGport,
        _bcmFieldSliceSelDisable, 0, 0, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMplsGports,
        _bcmFieldSliceSelDisable, 0, 0, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMimGport,
        _bcmFieldSliceSelDisable, 0, 0, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMimGports,
        _bcmFieldSliceSelDisable, 0, 0, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcWlanGport,
        _bcmFieldSliceSelDisable, 0, 0, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcWlanGports,
        _bcmFieldSliceSelDisable, 0, 0, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcVlanGport,
        _bcmFieldSliceSelDisable, 0, 0, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcVlanGports,
        _bcmFieldSliceSelDisable, 0, 0, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcVxlanGport,
        _bcmFieldSliceSelDisable, 0, 0, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcVxlanGports,
        _bcmFieldSliceSelDisable, 0, 0, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcNivGport,
        _bcmFieldSliceSelDisable, 0, 0, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcNivGports,
        _bcmFieldSliceSelDisable, 0, 0, 16);
	/*SVP - */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
        _bcmFieldSliceSelDisable, 0, 0, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
        _bcmFieldSliceSelDisable, 0, 0, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
        _bcmFieldSliceSelDisable, 0, 0, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
        _bcmFieldSliceSelDisable, 0, 0, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
        _bcmFieldSliceSelDisable, 0, 0, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL2,
        _bcmFieldSliceSelDisable, 0, 0, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL3,
        _bcmFieldSliceSelDisable, 0, 0, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcVirtualPortValid,
        _bcmFieldSliceSelDisable, 0, 0, 1);

    /* Meta data qualifiers. */
    /* L3_IIF table size is 8192. */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanId,
        _bcmFieldSliceSelDisable, 0, 0, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Ingress,
        _bcmFieldSliceSelDisable, 0, 0, 13);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyRangeCheck,
        _bcmFieldSliceSelDisable, 0, 0, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyPacketRes,
        _bcmFieldSliceSelDisable, 0, 0, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTunnelType,
        _bcmFieldSliceSelDisable, 0, 0, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyLoopbackType,
        _bcmFieldSliceSelDisable, 0, 0, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsLabel1Hit,
        _bcmFieldSliceSelDisable, 0, 0, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTrillEgressRbridgeHit,
        _bcmFieldSliceSelDisable, 0, 0, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2GreSrcIpHit,
        _bcmFieldSliceSelDisable, 0, 0, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMimSrcGportHit,
        _bcmFieldSliceSelDisable, 0, 0, 1);
    _FP_QUAL_MULTI_INFO_ADD(unit, stage_fc, bcmFieldQualifyMplsLabel2Hit,
        _bcmFieldSliceSelDisable, 0, 1, 4, 1, 1);
    _FP_QUAL_MULTI_INFO_ADD(unit, stage_fc, bcmFieldQualifyTrillIngressRbridgeHit,
        _bcmFieldSliceSelDisable, 0, 1, 4, 1, 1);
    _FP_QUAL_MULTI_INFO_ADD(unit, stage_fc, bcmFieldQualifyL2GreVfiHit,
        _bcmFieldSliceSelDisable, 0, 1, 4, 1, 1);
    _FP_QUAL_MULTI_INFO_ADD(unit, stage_fc, bcmFieldQualifyMimVfiHit,
        _bcmFieldSliceSelDisable, 0, 1, 4, 1, 1);
    _FP_QUAL_MULTI_INFO_ADD(unit, stage_fc, bcmFieldQualifyMplsTerminated,
        _bcmFieldSliceSelDisable, 0, 2, 4, 1, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingType,
        _bcmFieldSliceSelDisable, 0, 0, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIntPriority,
        _bcmFieldSliceSelDisable, 0, 0, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMHOpcode,
        _bcmFieldSliceSelDisable, 0, 0, 4);
    _FP_QUAL_MULTI_INFO_ADD(unit, stage_fc, bcmFieldQualifyHiGigProxy,
        _bcmFieldSliceSelDisable, 0, 1, 2, 1, 1);
    _FP_QUAL_MULTI_INFO_ADD(unit, stage_fc, bcmFieldQualifyL4Ports,
        _bcmFieldSliceSelDisable, 0, 1, 2, 1, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMirrorCopy,
        _bcmFieldSliceSelDisable, 0, 0, 1);
    _FP_QUAL_MULTI_INFO_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
        _bcmFieldSliceSelDisable, 0, 0, 2, 1, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyNatSrcRealmId,
        _bcmFieldSliceSelDisable, 0, 0, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIntCongestionNotification,
        _bcmFieldSliceSelDisable, 0, 0, 2);
    _FP_QUAL_MULTI_INFO_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
        _bcmFieldSliceSelDisable, 0, 1, 2, 1, 0);
    _FP_QUAL_MULTI_INFO_ADD(unit, stage_fc, bcmFieldQualifyMyStationHit,
        _bcmFieldSliceSelDisable, 0, 1, 2, 1, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyRtag7AHashUpper,
        _bcmFieldSliceSelDisable, 0, 0, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyRtag7AHashLower,
        _bcmFieldSliceSelDisable, 0, 0, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyRtag7BHashUpper,
        _bcmFieldSliceSelDisable, 0, 0, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyRtag7BHashLower,
        _bcmFieldSliceSelDisable, 0, 0, 16);

    /* VXLAN qualifiers. */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVxlanNetworkId,
        _bcmFieldSliceSelDisable, 0, 0, 24);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVxlanFlags,
        _bcmFieldSliceSelDisable, 0, 24, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVnTag,
        _bcmFieldSliceSelDisable, 0, 0, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyCnTag,
        _bcmFieldSliceSelDisable, 0, 0, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFabricQueueTag,
        _bcmFieldSliceSelDisable, 0, 0, 32);

    /* Class Qualifiers */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFcoeRxID,
            _bcmFieldSliceSelDisable, 0, 0, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFcoeOxID,
            _bcmFieldSliceSelDisable, 0, 0, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPortClass,
            _bcmFieldSliceSelDisable, 0, 0, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFcoeRxIDClass,
            _bcmFieldSliceSelDisable, 0, 0, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPortClass,
            _bcmFieldSliceSelDisable, 0, 0, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFcoeOxIDClass,
            _bcmFieldSliceSelDisable, 0, 0, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherTypeClass,
            _bcmFieldSliceSelDisable, 0, 0, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocolClass,
            _bcmFieldSliceSelDisable, 0, 0, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIpClassMsbNibble,
            _bcmFieldSliceSelDisable, 0, 0, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp6ClassMsbNibble,
            _bcmFieldSliceSelDisable, 0, 0, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanSrcIdClassMsbNibble,
            _bcmFieldSliceSelDisable, 0, 0, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIpClassMsbNibble,
            _bcmFieldSliceSelDisable, 0, 0, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6ClassMsbNibble,
            _bcmFieldSliceSelDisable, 0, 0, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanDstIdClassMsbNibble,
            _bcmFieldSliceSelDisable, 0, 0, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIpClassLower,
            _bcmFieldSliceSelDisable, 0, 0, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6ClassLower,
            _bcmFieldSliceSelDisable, 0, 0, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanDstIdClassLower,
            _bcmFieldSliceSelDisable, 0, 0, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIpClassUpper,
            _bcmFieldSliceSelDisable, 0, 0, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6ClassUpper,
            _bcmFieldSliceSelDisable, 0, 0, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanDstIdClassUpper,
            _bcmFieldSliceSelDisable, 0, 0, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIpClassLower,
            _bcmFieldSliceSelDisable, 0, 0, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp6ClassLower,
            _bcmFieldSliceSelDisable, 0, 0, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanSrcIdClassLower,
            _bcmFieldSliceSelDisable, 0, 0, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIpClassUpper,
            _bcmFieldSliceSelDisable, 0, 0, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp6ClassUpper,
            _bcmFieldSliceSelDisable, 0, 0, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanSrcIdClassUpper,
            _bcmFieldSliceSelDisable, 0, 0, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIpClass,
            _bcmFieldSliceSelDisable, 0, 0, 36);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp6Class,
            _bcmFieldSliceSelDisable, 0, 0, 36);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanSrcIdClass,
            _bcmFieldSliceSelDisable, 0, 0, 36);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIpClass,
            _bcmFieldSliceSelDisable, 0, 0, 36);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6Class,
            _bcmFieldSliceSelDisable, 0, 0, 36);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanDstIdClass,
            _bcmFieldSliceSelDisable, 0, 0, 36);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTcpClassZero,
            _bcmFieldSliceSelDisable, 0, 0, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTosClassZero,
            _bcmFieldSliceSelDisable, 0, 0, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTtlClassZero,
            _bcmFieldSliceSelDisable, 0, 0, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTcpClassOne,
            _bcmFieldSliceSelDisable, 0, 0, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTosClassOne,
            _bcmFieldSliceSelDisable, 0, 0, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTtlClassOne,
            _bcmFieldSliceSelDisable, 0, 0, 8);



    /* UDF1 and UDF2 qualifiers. */

    _FP_QUAL_ADD(unit, stage_fc, _bcmFieldQualifyData0,
        _bcmFieldSliceSelDisable, 0, 0, 16);
    _FP_QUAL_ADD(unit, stage_fc, _bcmFieldQualifyData1,
        _bcmFieldSliceSelDisable, 0, 0, 16);
    _FP_QUAL_ADD(unit, stage_fc, _bcmFieldQualifyData2,
        _bcmFieldSliceSelDisable, 0, 0, 32);
    _FP_QUAL_ADD(unit, stage_fc, _bcmFieldQualifyData3,
        _bcmFieldSliceSelDisable, 0, 0, 32);
    _FP_QUAL_ADD(unit, stage_fc, _bcmFieldQualifyData4,
        _bcmFieldSliceSelDisable, 0, 0, 32);
    _FP_QUAL_ADD(unit, stage_fc, _bcmFieldQualifyData5,
        _bcmFieldSliceSelDisable, 0, 0, 16);
    _FP_QUAL_ADD(unit, stage_fc, _bcmFieldQualifyData6,
        _bcmFieldSliceSelDisable, 0, 0, 16);
    _FP_QUAL_ADD(unit, stage_fc, _bcmFieldQualifyData7,
        _bcmFieldSliceSelDisable, 0, 0, 32);
    _FP_QUAL_ADD(unit, stage_fc, _bcmFieldQualifyData8,
        _bcmFieldSliceSelDisable, 0, 0, 32);
    _FP_QUAL_ADD(unit, stage_fc, _bcmFieldQualifyData9,
        _bcmFieldSliceSelDisable, 0, 0, 32);

    /* Preselector Logical Table ID Qualifier */
    _FP_QUAL_ADD(unit, stage_fc, _bcmFieldQualifyPreLogicalTableId,
        _bcmFieldSliceSelDisable, 0, 0, 5);
    return (BCM_E_NONE);
}

/*
 * Function:
 *    _field_th_qualifiers_init
 * Purpose:
 *    Initialize device qaualifiers select codes & offsets
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    stage_fc   - (IN/OUT) Field Processor stage control structure.
 *
 * Returns:
 *    BCM_E_PARAM - Null field stage control structure.
 *    BCM_E_MEMORY - Allocation failure.
 *    BCM_E_INTERNAL - Invalid CAP Stage ID.
 *    BCM_E_NONE - Success.
 */
STATIC int
_field_th_qualifiers_init(int unit, _field_stage_t *stage_fc)
{
    /* Input parameters check. */
    if (NULL == stage_fc) {
        return (BCM_E_PARAM);
    }

    /* Initialize the stage's qualifier information. */
    switch (stage_fc->stage_id) {
        case _BCM_FIELD_STAGE_INGRESS:
            return (_field_th_ingress_qualifiers_init(unit, stage_fc));
        case _BCM_FIELD_STAGE_LOOKUP:
            return (_field_th_lookup_qualifiers_init(unit, stage_fc));
        case _BCM_FIELD_STAGE_EGRESS:
            return (_field_th_egress_qualifiers_init(unit, stage_fc));
        case _BCM_FIELD_STAGE_CLASS:
            return BCM_E_NONE;
        default:
            break;
    }

    return (BCM_E_INTERNAL);
}

/*
 * Function:
 *     _bcm_field_th_egress_key_match_type_set
 *
 * Purpose:
 *     Set key match type based on entry group.
 *     NOTE: For double wide entries key type must be the same for
 *           both parts of the entry.
 * Parameters:
 *     unit   - (IN) BCM device number
 *     f_ent  - (IN) Slice number to enable
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_th_egress_key_match_type_set (int unit, _field_entry_t *f_ent)
{
    _field_group_t    *fg;          /* Field group entry belongs to. */
    uint32            data;         /* Key match type.               */
    uint32            default_data; /* default key match type.       */
    uint32            mask;         /* Key match type mask.          */
    int               rv;           /* Operation return status.      */

    /* Key match type offset.        */
    _bcm_field_qual_offset_t q_offset;
    {
       sal_memset(&q_offset, 0, sizeof(q_offset));
       q_offset.field = KEYf;
       q_offset.num_offsets = 1;
       q_offset.offset[0] = 236;
       q_offset.width[0] = 4;
    }

    /* Input parameters check. */
    if (NULL == f_ent) {
        LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META("Invalid Parameters\n")));
        return (BCM_E_PARAM);
    }
    if (NULL == (fg = f_ent->group)) {
        LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META("Invalid Parameters\n")));
        return (BCM_E_PARAM);
    }

    if (fg->flags & _FP_GROUP_SPAN_SINGLE_SLICE) {
        switch (fg->sel_codes[0].fpf3) {
            case _BCM_FIELD_EFP_KEY1:
                data = KEY_TYPE_IPv4_SINGLE;
                default_data = KEY_TYPE_L2_SINGLE;
                break;
            case _BCM_FIELD_EFP_KEY2:
                data = KEY_TYPE_IPv6_SINGLE;
                default_data = KEY_TYPE_L2_SINGLE;
                break;
            case _BCM_FIELD_EFP_KEY4:
                data = KEY_TYPE_L2_SINGLE;
                default_data = KEY_TYPE_L2_SINGLE;
                break;
            default:
                return (BCM_E_INTERNAL);
        }
    } else {
        switch (fg->sel_codes[1].fpf3) {
            case _BCM_FIELD_EFP_KEY3:
                data = KEY_TYPE_IPv6_DOUBLE;
                default_data = KEY_TYPE_L2_DOUBLE;
                break;
            case _BCM_FIELD_EFP_KEY4:
                if (_BCM_FIELD_EFP_KEY1 == fg->sel_codes[0].fpf3) {
                    if (BCM_FIELD_QSET_TEST(fg->qset, bcmFieldQualifyIp4)){
                        default_data = KEY_TYPE_L2_DOUBLE;
                    } else {
                        default_data = KEY_TYPE_IPv4_L2_L3_DOUBLE;
                    }
                    data = KEY_TYPE_IPv4_L2_L3_DOUBLE;
                } else if (_BCM_FIELD_EFP_KEY2 == fg->sel_codes[0].fpf3) {
                    data = KEY_TYPE_IPv4_IPv6_DOUBLE;
                    default_data = KEY_TYPE_L2_DOUBLE;
                } else if (_BCM_FIELD_EFP_KEY6 == fg->sel_codes[0].fpf3) {
                    data = KEY_TYPE_HIGIG_DOUBLE;
                    default_data = KEY_TYPE_IPv4_L2_L3_DOUBLE;
                } else if (_BCM_FIELD_EFP_KEY7 == fg->sel_codes[0].fpf3) {
                    data = KEY_TYPE_LOOPBACK_DOUBLE;
                    default_data = KEY_TYPE_IPv4_L2_L3_DOUBLE;
                } else if (_BCM_FIELD_EFP_KEY8 == fg->sel_codes[0].fpf3) {
                    data = KEY_TYPE_L2_DOUBLE;
                    default_data = KEY_TYPE_L2_DOUBLE;
                }
                break;
            default:
                return (BCM_E_INTERNAL);
        }
    }

    mask = 0xf;

   /*
    * COVERITY
    *
    *This flow takes care of the  Out-of-bounds access issue
    * for data and mask.
    */
    /* coverity[callee_ptr_arith : FALSE] */
    rv = _bcm_field_qual_value_set_by_entry_type(unit, &q_offset, f_ent,
                                                 &data, &mask,
                                                 _FP_ENTRY_TYPE_0);
    BCM_IF_ERROR_RETURN(rv);

   /*
    * COVERITY
    *
    * This flow takes care of the  Out-of-bounds access issue
    * for data and mask.
    */

    /* coverity[callee_ptr_arith : FALSE] */

    rv = _bcm_field_qual_value_set_by_entry_type(unit, &q_offset, f_ent,
                                                 &default_data, &mask,
                                                 _FP_ENTRY_TYPE_1);
    BCM_IF_ERROR_RETURN(rv);

    return rv;
}

/*
 * Function:
 *     _bcm_field_th_egress_selcode_get
 * Purpose:
 *     Finds a select encodings that will satisfy the
 *     requested qualifier set (Qset).
 * Parameters:
 *     unit      - (IN) BCM unit number.
 *     stage_fc  - (IN) Stage Field control structure.
 *     qset_req  - (IN) Client qualifier set.
 *     fg        - (IN/OUT)Select code information filled into the group.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_th_egress_selcode_get(int unit, _field_stage_t *stage_fc,
                               bcm_field_qset_t *qset_req,
                               _field_group_t *fg)
{
    int             rv;          /* Operation return status. */
    bcm_field_qset_t new_qset;

    /* Input parameters check. */
    if ((NULL == fg) || (NULL == qset_req) || (NULL == stage_fc)) {
        LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META("Invalid Parameters\n")));
        return (BCM_E_PARAM);
    }

    if  (fg->flags & _FP_GROUP_SPAN_SINGLE_SLICE) {
        /* Attempt _BCM_FIELD_EFP_KEY4  (L2 key). */
        rv = _bcm_field_egress_key_attempt(unit, stage_fc, qset_req,
                                           _BCM_FIELD_EFP_KEY4, 0, fg);
        if (BCM_SUCCESS(rv)) {

            rv = _bcm_field_trx_qset_get(qset_req, stage_fc,
                                         _BCM_FIELD_EFP_KEY4, -1, &new_qset);
            BCM_IF_ERROR_RETURN(rv);

            /* In EFP, for a given qset we can have two selector code
               combination. So attempt _BCM_FIELD_EFP_KEY4 (Any Packet type) */

            rv = _bcm_field_trx_egress_default_selcode_get(unit, stage_fc,
                                                           &new_qset,
                                                           _BCM_FIELD_EFP_KEY4,
                                                           0, fg);
            if (BCM_SUCCESS(rv) || (BCM_FAILURE(rv) &&
                                    (rv != BCM_E_RESOURCE))) {
                fg->sel_codes[0].fpf3= _BCM_FIELD_EFP_KEY4;
                return rv;
            }
        } else if(BCM_FAILURE(rv) && (rv != BCM_E_RESOURCE)) {
            return rv;
        }

        /* Attempt _BCM_FIELD_EFP_KEY1  (IPv4 key). */
        rv = _bcm_field_egress_key_attempt(unit, stage_fc, qset_req,
                                           _BCM_FIELD_EFP_KEY1, 0, fg);

        if (BCM_SUCCESS(rv)) {

            rv = _bcm_field_trx_qset_get(qset_req, stage_fc,
                                         _BCM_FIELD_EFP_KEY4, -1, &new_qset);
            BCM_IF_ERROR_RETURN(rv);


            /* In EFP, for a given qset we can have two selector code
               combination. So attempt _BCM_FIELD_EFP_KEY4 (Any Packet type) */

            rv = _bcm_field_trx_egress_default_selcode_get(unit, stage_fc,
                                                           &new_qset,
                                                           _BCM_FIELD_EFP_KEY4,
                                                           0, fg);
            if (BCM_SUCCESS(rv) || (BCM_FAILURE(rv) &&
                                    (rv != BCM_E_RESOURCE))) {
                fg->sel_codes[0].fpf3= _BCM_FIELD_EFP_KEY1;
                return rv;
            }
        } else if(BCM_FAILURE(rv) && (rv != BCM_E_RESOURCE)) {
            return rv;
        }

        /* Attempt _BCM_FIELD_EFP_KEY2  (IPv6 key). */
        rv = _bcm_field_egress_key_attempt(unit, stage_fc, qset_req,
                                           _BCM_FIELD_EFP_KEY2, 0, fg);
        if (BCM_SUCCESS(rv)) {

            rv = _bcm_field_trx_qset_get(qset_req, stage_fc,
                                         _BCM_FIELD_EFP_KEY4, -1, &new_qset);
            BCM_IF_ERROR_RETURN(rv);


            /* In EFP, for a given qset we can have two selector code
               combination. So attempt _BCM_FIELD_EFP_KEY4 (Any Packet type) */


            rv = _bcm_field_trx_egress_default_selcode_get(unit, stage_fc,
                                                           &new_qset,
                                                           _BCM_FIELD_EFP_KEY4,
                                                           0, fg);
            if (BCM_SUCCESS(rv) || (BCM_FAILURE(rv) &&
                                    (rv != BCM_E_RESOURCE))) {
                fg->sel_codes[0].fpf3= _BCM_FIELD_EFP_KEY2;
            return rv;
        }
        } else if(BCM_FAILURE(rv) && (rv != BCM_E_RESOURCE)) {
            return rv;
        }
    } else {

        if (BCM_FIELD_QSET_TEST(fg->qset, bcmFieldQualifyIp4)) {
            /* L3 + L2 double wide key for IPv4 packets */
            rv = _bcm_field_egress_key_attempt(unit, stage_fc, qset_req,
                                               _BCM_FIELD_EFP_KEY1,
                                               _BCM_FIELD_EFP_KEY4, fg);
            if (BCM_SUCCESS(rv)) {

                rv = _bcm_field_trx_qset_get(qset_req, stage_fc,
                                             _BCM_FIELD_EFP_KEY8,
                                             _BCM_FIELD_EFP_KEY4,
                                             &new_qset);
                BCM_IF_ERROR_RETURN(rv);


                /* In EFP, for a given qset we can have two selector code
                   combination. So attempt _BCM_FIELD_EFP_KEY8 and
                   _BCM_FIELD_EFP_KEY4 (Any Packet type) */


                rv = _bcm_field_trx_egress_default_selcode_get(unit, stage_fc,
                                                               &new_qset,
                                                           _BCM_FIELD_EFP_KEY8,
                                                           _BCM_FIELD_EFP_KEY4,
                                                              fg);
                if (BCM_SUCCESS(rv) || (BCM_FAILURE(rv) &&
                                   (rv != BCM_E_RESOURCE))) {
                    fg->sel_codes[0].fpf3= _BCM_FIELD_EFP_KEY1;
                    fg->sel_codes[1].fpf3= _BCM_FIELD_EFP_KEY4;
                    return rv;
                }
            } else if(BCM_FAILURE(rv) && (rv != BCM_E_RESOURCE)) {
                return rv;
            }
        }

        /* L2 + L3 double wide predefined key. */
        rv = _bcm_field_egress_key_attempt(unit, stage_fc, qset_req,
                                           _BCM_FIELD_EFP_KEY1,
                                           _BCM_FIELD_EFP_KEY4, fg);
        if (BCM_SUCCESS(rv)) {

            rv = _bcm_field_trx_qset_get(qset_req, stage_fc,
                                         _BCM_FIELD_EFP_KEY1,
                                         _BCM_FIELD_EFP_KEY4,
                                         &new_qset);
            BCM_IF_ERROR_RETURN(rv);


            /* In EFP, for a given qset we can have two selector code
               combination. So attempt _BCM_FIELD_EFP_KEY1 and
               _BCM_FIELD_EFP_KEY4 (Any Packet type) */


            rv = _bcm_field_trx_egress_default_selcode_get(unit, stage_fc,
                                                           &new_qset,
                                                           _BCM_FIELD_EFP_KEY1,
                                                           _BCM_FIELD_EFP_KEY4,
                                                           fg);
            if (BCM_SUCCESS(rv) || (BCM_FAILURE(rv) &&
                                    (rv != BCM_E_RESOURCE))) {
                fg->sel_codes[0].fpf3= _BCM_FIELD_EFP_KEY1;
                fg->sel_codes[1].fpf3= _BCM_FIELD_EFP_KEY4;
                return rv;
            }
        } else if(BCM_FAILURE(rv) && (rv != BCM_E_RESOURCE)) {
            return rv;
        }

        /* HiGiG Doublewide key */
        rv = _bcm_field_egress_key_attempt(unit, stage_fc, qset_req,
                                           _BCM_FIELD_EFP_KEY6,
                                           _BCM_FIELD_EFP_KEY4, fg);
        if (BCM_SUCCESS(rv)) {

            rv = _bcm_field_trx_qset_get(qset_req, stage_fc,
                                         _BCM_FIELD_EFP_KEY1,
                                         _BCM_FIELD_EFP_KEY4,
                                         &new_qset);
            BCM_IF_ERROR_RETURN(rv);


            /* In EFP, for a given qset we can have two selector code
               combination. So attempt _BCM_FIELD_EFP_KEY1 and
               _BCM_FIELD_EFP_KEY4 (Any Packet type) */


            rv = _bcm_field_trx_egress_default_selcode_get(unit, stage_fc,
                                                           &new_qset,
                                                           _BCM_FIELD_EFP_KEY1,
                                                           _BCM_FIELD_EFP_KEY4,
                                                           fg);
            if (BCM_SUCCESS(rv) || (BCM_FAILURE(rv) &&
                                    (rv != BCM_E_RESOURCE))) {
                fg->sel_codes[0].fpf3= _BCM_FIELD_EFP_KEY6;
                fg->sel_codes[1].fpf3= _BCM_FIELD_EFP_KEY4;
                return rv;
            }
        } else if(BCM_FAILURE(rv) && (rv != BCM_E_RESOURCE)) {
            return rv;
        }

        /* Loopback double wide key */

        rv = _bcm_field_egress_key_attempt(unit, stage_fc, qset_req,
                                           _BCM_FIELD_EFP_KEY7,
                                           _BCM_FIELD_EFP_KEY4, fg);
        if (BCM_SUCCESS(rv)) {

            rv = _bcm_field_trx_qset_get(qset_req, stage_fc,
                                         _BCM_FIELD_EFP_KEY1,
                                         _BCM_FIELD_EFP_KEY4,
                                         &new_qset);
            BCM_IF_ERROR_RETURN(rv);


            /* In EFP, for a given qset we can have two selector code
               combination. So attempt _BCM_FIELD_EFP_KEY1 and
               _BCM_FIELD_EFP_KEY4 (Any Packet type) */


            rv = _bcm_field_trx_egress_default_selcode_get(unit, stage_fc,
                                                           &new_qset,
                                                           _BCM_FIELD_EFP_KEY1,
                                                           _BCM_FIELD_EFP_KEY4,
                                                           fg);
            if (BCM_SUCCESS(rv) || (BCM_FAILURE(rv) &&
                                    (rv != BCM_E_RESOURCE))) {
                fg->sel_codes[0].fpf3= _BCM_FIELD_EFP_KEY7;
                fg->sel_codes[1].fpf3= _BCM_FIELD_EFP_KEY4;
                return rv;
            }
        } else if(BCM_FAILURE(rv) && (rv != BCM_E_RESOURCE)) {
            return rv;
        }

        /* L3 + L2 double wide key for IPv4 packets */
        rv = _bcm_field_egress_key_attempt(unit, stage_fc, qset_req,
                                           _BCM_FIELD_EFP_KEY1,
                                           _BCM_FIELD_EFP_KEY4, fg);
        if (BCM_SUCCESS(rv)) {

            rv = _bcm_field_trx_qset_get(qset_req, stage_fc,
                                         _BCM_FIELD_EFP_KEY8,
                                         _BCM_FIELD_EFP_KEY4,
                                         &new_qset);
            BCM_IF_ERROR_RETURN(rv);


            /* In EFP, for a given qset we can have two selector code
               combination. So attempt _BCM_FIELD_EFP_KEY8 and
               _BCM_FIELD_EFP_KEY4 (Any Packet type) */


            rv = _bcm_field_trx_egress_default_selcode_get(unit, stage_fc,
                                                           &new_qset,
                                                           _BCM_FIELD_EFP_KEY8,
                                                           _BCM_FIELD_EFP_KEY4,
                                                           fg);
            if (BCM_SUCCESS(rv) || (BCM_FAILURE(rv) &&
                                    (rv != BCM_E_RESOURCE))) {
                fg->sel_codes[0].fpf3= _BCM_FIELD_EFP_KEY1;
                fg->sel_codes[1].fpf3= _BCM_FIELD_EFP_KEY4;
                return rv;
            }
        } else if(BCM_FAILURE(rv) && (rv != BCM_E_RESOURCE)) {
            return rv;
        }

        /* L3 + L2 double wide key for IPv6 packets */
        rv = _bcm_field_egress_key_attempt(unit, stage_fc, qset_req,
                                           _BCM_FIELD_EFP_KEY2,
                                           _BCM_FIELD_EFP_KEY4, fg);
        if (BCM_SUCCESS(rv)) {

            rv = _bcm_field_trx_qset_get(qset_req, stage_fc,
                                         _BCM_FIELD_EFP_KEY8,
                                         _BCM_FIELD_EFP_KEY4,
                                         &new_qset);
            BCM_IF_ERROR_RETURN(rv);


            /* In EFP, for a given qset we can have two selector code
               combination. So attempt _BCM_FIELD_EFP_KEY8 and
               _BCM_FIELD_EFP_KEY4 (Any Packet type) */


            rv = _bcm_field_trx_egress_default_selcode_get(unit, stage_fc,
                                                           &new_qset,
                                                           _BCM_FIELD_EFP_KEY8,
                                                           _BCM_FIELD_EFP_KEY4,
                                                           fg);
            if (BCM_SUCCESS(rv) || (BCM_FAILURE(rv) &&
                                    (rv != BCM_E_RESOURCE))) {
                fg->sel_codes[0].fpf3= _BCM_FIELD_EFP_KEY2;
                fg->sel_codes[1].fpf3= _BCM_FIELD_EFP_KEY4;
                return rv;
            }
        } else if(BCM_FAILURE(rv) && (rv != BCM_E_RESOURCE)) {
            return rv;
        }

        /* L3 key for IPv6 packets */
        rv = _bcm_field_egress_key_attempt(unit, stage_fc, qset_req,
                                           _BCM_FIELD_EFP_KEY2,
                                           _BCM_FIELD_EFP_KEY3, fg);
        if (BCM_SUCCESS(rv)) {

            rv = _bcm_field_trx_qset_get(qset_req, stage_fc,
                                         _BCM_FIELD_EFP_KEY8,
                                         _BCM_FIELD_EFP_KEY4,
                                         &new_qset);
            BCM_IF_ERROR_RETURN(rv);


            /* In EFP, for a given qset we can have two selector code
               combination. So attempt _BCM_FIELD_EFP_KEY8 and
               _BCM_FIELD_EFP_KEY4 (Any Packet type) */


            rv = _bcm_field_trx_egress_default_selcode_get(unit, stage_fc,
                                                           &new_qset,
                                                           _BCM_FIELD_EFP_KEY8,
                                                           _BCM_FIELD_EFP_KEY4,
                                                           fg);
            if (BCM_SUCCESS(rv) || (BCM_FAILURE(rv) &&
                                    (rv != BCM_E_RESOURCE))) {
                fg->sel_codes[0].fpf3= _BCM_FIELD_EFP_KEY2;
                fg->sel_codes[1].fpf3= _BCM_FIELD_EFP_KEY3;
                return rv;
            }
        } else if(BCM_FAILURE(rv) && (rv != BCM_E_RESOURCE)) {
            return rv;
        }

        /* Bytes after L2 Double key for non IP packets */
        rv = _bcm_field_egress_key_attempt(unit, stage_fc, qset_req,
                                           _BCM_FIELD_EFP_KEY8,
                                           _BCM_FIELD_EFP_KEY4, fg);
        if (BCM_SUCCESS(rv)) {

            rv = _bcm_field_trx_qset_get(qset_req, stage_fc,
                                         _BCM_FIELD_EFP_KEY8,
                                         _BCM_FIELD_EFP_KEY4,
                                         &new_qset);
            BCM_IF_ERROR_RETURN(rv);


            /* In EFP, for a given qset we can have two selector code
               combination. So attempt _BCM_FIELD_EFP_KEY8 and
               _BCM_FIELD_EFP_KEY4 (Any Packet type) */


            rv = _bcm_field_trx_egress_default_selcode_get(unit, stage_fc,
                                                           &new_qset,
                                                           _BCM_FIELD_EFP_KEY8,
                                                           _BCM_FIELD_EFP_KEY4,
                                                           fg);
            if (BCM_SUCCESS(rv) || (BCM_FAILURE(rv) &&
                                    (rv != BCM_E_RESOURCE))) {
                fg->sel_codes[0].fpf3= _BCM_FIELD_EFP_KEY8;
                fg->sel_codes[1].fpf3= _BCM_FIELD_EFP_KEY4;
                return rv;
            }
        } else if(BCM_FAILURE(rv) && (rv != BCM_E_RESOURCE)) {
            return rv;
        }
    }

    return BCM_E_RESOURCE;
}

/*
 * Function:
 *     _bcm_field_th_selcode_get
 * Purpose:
 *     Finds a 4-tuple of select encodings that will satisfy the
 *     requested qualifier set (Qset).
 * Parameters:
 *     unit      - (IN) BCM unit number.
 *     stage_fc  - (IN) Stage Field control structure.
 *     qset_req  - (IN) Client qualifier set.
 *     fg        - (IN/OUT)Select code information filled into the group.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_th_selcode_get(int unit, _field_stage_t *stage_fc,
                          bcm_field_qset_t *qset_req,
                          _field_group_t *fg)
{
    int  rv;                     /* Operation return status. */

    /* Input parameters check. */
    if ((NULL == fg) || (NULL == qset_req) || (NULL == stage_fc)) {
        LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META("Invalid Parameters\n")));
        return (BCM_E_PARAM);
    }

    /* Egress qualifiers are selected based on Key. */
    switch (stage_fc->stage_id) {
      case _BCM_FIELD_STAGE_EGRESS:
          rv =  _bcm_field_th_egress_selcode_get(unit, stage_fc, qset_req, fg);
          break;
      default:
          rv =  _bcm_field_selcode_get(unit, stage_fc, qset_req, fg);
    }
    return (rv);
}

/*
 * Function:
 *    _field_th_group_install
 *
 * Purpose:
 *    Auxiliary routine used to install field group.
 * Parameters:
 *     unit       - (IN) BCM device number.
 *     fg         - (IN) Installed group structure.
 *
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_group_install(int unit, _field_group_t *fg)
{
    _field_slice_t *fs;        /* Slice pointer.           */
    uint8  slice_number;       /* Slices iterator.         */
    int    parts_count;        /* Number of entry parts.   */
    int    idx;                /* Iteration index.         */

    if (NULL == fg) {
        LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META("Invalid Parameters\n")));
        return (BCM_E_PARAM);
    }

    /* Get number of entry parts. */
    BCM_IF_ERROR_RETURN(_bcm_field_entry_tcam_parts_count(unit,
                                                          fg->stage_id,
                                                          fg->flags,
                                                          &parts_count
                                                          )
                        );

    for (idx = 0; idx < parts_count; ++idx) {
        BCM_IF_ERROR_RETURN(_bcm_field_tcam_part_to_slice_number(unit, idx,
                                                                 fg,
                                                                 &slice_number
                                                                 )
                            );
        fs = fg->slices + slice_number;

        BCM_IF_ERROR_RETURN(_bcm_field_th_selcodes_install(unit,
                                                           fg,
                                                           fs->slice_number,
                                                           fg->pbmp,
                                                           idx)
                            );
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_field_th_group_install
 *
 * Purpose:
 *    Auxiliary routine used to install field group.
 * Parameters:
 *     unit       - (IN) BCM device number.
 *     fg         - (IN) Installed group structure.
 *
 * Returns:
 *     BCM_E_XXX
 */

int
_bcm_field_th_group_install(int unit, _field_group_t *fg)
{
  return _field_th_group_install(unit, fg);
}
           /* START OF COUNTER ROUTINES */


/*
 * Function:
 *     _bcm_field_th_flex_stat_index_get
 * Purpose:
 *      Get the indexes and flags needed to compose requested statistic
 *      which uses flex counter pools.
 *
 * Parameters:
 *   unit          - (IN)  BCM device number.
 *   f_st          - (IN)  Field statistics entity.
 *   stat          - (IN)  Counter type.
 *   idx1          - (OUT) Primary counter index.
 *   idx2          - (OUT) Secondary counter index.
 *   idx3          - (OUT) Ternary Counter Index.
 *   out_flags     - (OUT) Counter flags.
 * Returns:
 *    BCM_E_XXX
 */
int
_bcm_field_th_flex_stat_index_get(int unit,
                                  _field_stat_t *f_st,
                                  bcm_field_stat_t stat,
                                  int *idx1, int *idx2,
                                  int *idx3, uint32 *out_flags)
{
    int     fp_stat_mode_max; /* Max stat modes.                 */
    uint8   idx;              /* HW counter modes iterator.      */
    uint32  flags = 0;        /* _FP_STAT_XXX flags.             */
    uint32  req_bmap;         /* Requested statistics bitmap.    */
    uint32  hw_bmap;          /* HW supported statistics bitmap. */
    _field_stat_color_t  color; /* Stat Color. */

    if (NULL == f_st ||
        NULL == idx1 ||
        NULL == idx2 ||
        NULL == idx3 ||
        NULL == out_flags) {
        return BCM_E_PARAM;
    }

    /* Get application requested bitmap. */
    BCM_IF_ERROR_RETURN
        (_bcm_field_stat_array_to_bmap(unit, f_st, &req_bmap));

    /* If bitmap is zero - no mode selection is required. */
    if (0 == req_bmap) {
        return BCM_E_INTERNAL;
    }

    *idx1 = *idx2 = *idx3 = _FP_INVALID_INDEX;

    fp_stat_mode_max = COUNTOF(th_ingress_cntr_hw_mode_tbl);
    for (idx = 0; idx < fp_stat_mode_max; idx++) {
        hw_bmap = th_ingress_cntr_hw_mode_tbl[idx].hw_bmap;
        color = th_ingress_cntr_hw_mode_tbl[idx].color;
        if (0 == ((req_bmap | hw_bmap) & ~(hw_bmap))) {
            /*
             * For maximizing utilization of hardware counters, this should
             * select the hw_mode both supports the requested stats
             * AND minimizes the number of required hardware counters.
             */
             break;
        }
    }

    if  (idx >= fp_stat_mode_max) {
        return (BCM_E_INTERNAL);
    }

    switch (stat) {
        case bcmFieldStatBytes:
            flags |= _FP_STAT_BYTES;
            /* Fall through */
        case bcmFieldStatPackets:
            if (_bcmFieldStatColorNoColor == color) {
                *idx1 = f_st->hw_index;
            } else if ((_bcmFieldStatColorGreenNotGreen == color) ||
                     (_bcmFieldStatColorYellowNotYellow == color) ||
                     (_bcmFieldStatColorRedNotRed == color)) {
                *idx1 = f_st->hw_index;
                *idx2 = f_st->hw_index + 1;
            } else if (_bcmFieldStatColorGreenYellowRed == color) {
                *idx1 = f_st->hw_index;
                *idx2 = f_st->hw_index + 1;
                *idx3 = f_st->hw_index + 2;
            } else {
                return BCM_E_INTERNAL;
            }
            break;
        case bcmFieldStatGreenBytes:
            flags |= _FP_STAT_BYTES;
            /* Fall through */
        case bcmFieldStatGreenPackets:
            if ((_bcmFieldStatColorGreen == color) ||
                (_bcmFieldStatColorGreenYellow == color) ||
                (_bcmFieldStatColorGreenRed == color) ||
                (_bcmFieldStatColorGreenNotGreen == color) ||
                (_bcmFieldStatColorGreenYellowRed == color)) {
                *idx1 = f_st->hw_index;
            } else {
                return BCM_E_INTERNAL;
            }
            break;
        case bcmFieldStatYellowBytes:
            flags |= _FP_STAT_BYTES;
            /* Fall through */
        case bcmFieldStatYellowPackets:
            if ((_bcmFieldStatColorYellow == color) ||
                (_bcmFieldStatColorYellowRed == color) ||
                (_bcmFieldStatColorYellowNotYellow == color)) {
                *idx1 = f_st->hw_index;
            } else if ((_bcmFieldStatColorGreenYellow == color) ||
                       (_bcmFieldStatColorGreenYellowRed == color)) {
                *idx1 = f_st->hw_index + 1;
            } else {
                return BCM_E_INTERNAL;
            }
            break;
        case bcmFieldStatRedBytes:
            flags |= _FP_STAT_BYTES;
            /* Fall through */
        case bcmFieldStatRedPackets:
            if ((_bcmFieldStatColorRed == color) ||
                (_bcmFieldStatColorRedNotRed == color)) {
                *idx1 = f_st->hw_index;
            } else if ((_bcmFieldStatColorGreenRed == color) ||
                       (_bcmFieldStatColorYellowRed == color)) {
                *idx1 = f_st->hw_index + 1;
            } else if (_bcmFieldStatColorGreenYellowRed == color) {
                *idx1 = f_st->hw_index + 2;
            } else {
                return BCM_E_INTERNAL;
            }
            break;
        case bcmFieldStatNotGreenBytes:
            flags |= _FP_STAT_BYTES;
            /* Fall through */
        case bcmFieldStatNotGreenPackets:
            if (_bcmFieldStatColorNotGreen == color) {
                *idx1 = f_st->hw_index;
            } else if (_bcmFieldStatColorGreenNotGreen == color) {
                *idx1 = f_st->hw_index + 1;
            } else if (_bcmFieldStatColorYellowRed == color) {
                *idx1 = f_st->hw_index;
                *idx2 = f_st->hw_index + 1;
            } else if (_bcmFieldStatColorGreenYellowRed == color) {
                *idx1 = f_st->hw_index + 1;
                *idx2 = f_st->hw_index + 2;
            } else {
                return BCM_E_INTERNAL;
            }
            break;
        case bcmFieldStatNotYellowBytes:
            flags |= _FP_STAT_BYTES;
            /* Fall through */
        case bcmFieldStatNotYellowPackets:
            if (_bcmFieldStatColorNotYellow == color) {
                *idx1 = f_st->hw_index;
            } else if (_bcmFieldStatColorYellowNotYellow == color) {
                *idx1 = f_st->hw_index + 1;
            } else if (_bcmFieldStatColorGreenRed == color) {
                *idx1 = f_st->hw_index;
                *idx2 = f_st->hw_index + 1;
            } else if (_bcmFieldStatColorGreenYellowRed == color) {
                *idx1 = f_st->hw_index;
                *idx2 = f_st->hw_index + 2;
            } else {
                return BCM_E_INTERNAL;
            }
            break;
        case bcmFieldStatNotRedBytes:
            flags |= _FP_STAT_BYTES;
            /* Fall through */
        case bcmFieldStatNotRedPackets:
            if (_bcmFieldStatColorNotRed == color) {
                *idx1 = f_st->hw_index;
            } else if (_bcmFieldStatColorRedNotRed == color) {
                *idx1 = f_st->hw_index + 1;
            } else if ((_bcmFieldStatColorGreenYellow == color) ||
                       (_bcmFieldStatColorGreenYellowRed == color)) {
                *idx1 = f_st->hw_index;
                *idx2 = f_st->hw_index + 1;
            } else {
                return BCM_E_INTERNAL;
            }
            break;
        default:
            return BCM_E_INTERNAL;
    }

    flags |= _FP_STAT_ADD;
    *out_flags = flags;
    return BCM_E_NONE;
}
/*
 * Function:
 *     _bcm_field_th_stat_index_get
 * Purpose:
 *      Get the indexes and flags needed to compose requested statistic.
 *
 * Parameters:
 *   unit          - (IN)  BCM device number.
 *   f_st          - (IN)  Field statistics entity.
 *   stat          - (IN)  Counter type.
 *   idx1          - (OUT)  Primary counter index.
 *   idx2          - (OUT)  Secondary counter index.
 *   out_flags     - (OUT)  Counter flags.
 * Returns:
 *    BCM_E_XXX
 */
int
_bcm_field_th_stat_index_get(int unit,
                             _field_stat_t *f_st,
                             bcm_field_stat_t stat,
                             int *idx1, int *idx2,
                             int *idx3, uint32 *out_flags)
{
    int rv;

    if (NULL == f_st ||
        NULL == idx1 ||
        NULL == idx2 ||
        NULL == idx3 ||
        NULL == out_flags) {
        return BCM_E_PARAM;
    }

    if (_BCM_FIELD_STAGE_INGRESS == f_st->stage_id) {
        rv = _bcm_field_th_flex_stat_index_get(unit, f_st, stat, idx1,
                                               idx2, idx3, out_flags);
    } else {
        rv = _bcm_field_trx_stat_index_get(unit, f_st, stat, idx1,
                                           idx2, idx3, out_flags);
    }

    return rv;
}
/*
 * Function:
 *    _field_th_flex_mode_get
 * Purpose:
 *    Get flex stat hardware counter mode information
 * Parameters:
 *     unit  - (IN) BCM device number
 *     f_st  - (IN) Pointer to field stat structure
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_flex_mode_get(int unit,
                         _field_stat_t *f_st)
{
    int     fp_stat_mode_max; /* Max stat modes.  */
    uint8   idx;            /* HW counter modes iterator.      */
    uint8   hw_entry_count; /* Number of counter table entries.*/
    uint32  req_bmap;       /* Requested statistics bitmap.    */
    uint32  hw_bmap;        /* HW supported statistics bitmap. */

    /* Input parameter check. */
    if (NULL == f_st) {
        return (BCM_E_PARAM);
    }

    /* Get application requested bitmap. */
    BCM_IF_ERROR_RETURN
        (_bcm_field_stat_array_to_bmap(unit, f_st, &req_bmap));

    /* If bitmap is zero - no mode selection is required. */
    if (0 == req_bmap) {
        f_st->hw_mode = 0;
        f_st->hw_stat = 0;
        f_st->hw_flags |= _FP_STAT_FLEX_CNTR;
        f_st->flex_mode = 0;
        f_st->hw_entry_count = 0;
        return (BCM_E_NONE);
    }

    fp_stat_mode_max = COUNTOF(th_ingress_cntr_hw_mode_tbl);
    for (idx = 0; idx < fp_stat_mode_max; idx++) {
        hw_bmap = th_ingress_cntr_hw_mode_tbl[idx].hw_bmap;
        hw_entry_count = th_ingress_cntr_hw_mode_tbl[idx].hw_entry_count;
        if (0 == ((req_bmap | hw_bmap) & ~(hw_bmap))) {
            /*
             * For maximizing utilization of hardware counters, this should
             * select the hw_mode both supports the requested stats
             * AND minimizes the number of required hardware counters.
             */
             break;
        }
    }

    if  (idx >= fp_stat_mode_max) {
        return (BCM_E_RESOURCE);
    }

     /* Initialize stat structure with flex stat parameter info. */
     f_st->hw_stat   = req_bmap;
     f_st->hw_flags |= _FP_STAT_FLEX_CNTR;
     f_st->flex_mode = 0;
     f_st->hw_entry_count = hw_entry_count;

     return (BCM_E_NONE);
}
/*
 * Function:
 *    _bcm_field_th_stat_hw_mode_get
 *
 * Description:
 *      Get hw counter mode which will satisfy requested statistics set.
 * Parameters:
 *      unit     - (IN) BCM device number.
 *      f_st     - (IN) Field statistics entity.
 *      stage_id - (IN) Field stage id.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_field_th_stat_hw_mode_get(int unit, _field_stat_t *f_st,
                                  _field_stage_id_t stage_id)
{
    /* Input parameters check. */
    if (NULL == f_st) {
        return (BCM_E_PARAM);
    }

    /*
     * Tomahawk supports advanced flex counters
     * for both IFP then get flex stat mode for this Stat.
     */
    if (soc_feature(unit, soc_feature_advanced_flex_counter)
        && (_BCM_FIELD_STAGE_INGRESS == stage_id)) {
        return (_field_th_flex_mode_get(unit, f_st));
    }

    /* EFP still uses dedicated counter pools which can be used by only EFP.
     * Tomahawk VFP uses advance flex counters but no change when compred to
     * Trident2.
     */
    return _bcm_field_stat_hw_mode_get(unit, f_st, stage_id);
}

/*
 * Function:
 *    _bcm_field_th_group_counter_pbm_clear
 *
 * Description:
 *      Clear group counter pool bitmap based on used entries in
 *      in the pool.
 * Parameters:
 *      unit     - (IN) BCM device number.
 *      f_ent    - (IN) Field entry structure.
 *      pool_id  - (IN) Counter pool id.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_field_th_group_counter_pbm_clear(int unit,
                                      _field_entry_t *f_ent,
                                      uint32 pool_id)
{
    int rv; /* Operational Status. */
    uint32 num_pools; /* Counter pool count. */
    uint32 actual_num_pools; /* Actual Counter Pooll Count. */
    bcm_stat_flex_direction_t direction; /* Flex Stat Module object to indicate
                                          * Ingres/Egress Counter pools.
                                          */
    /* Flex Stat Object to carry counter pool info. */
    bcm_stat_flex_pool_stat_info_t pool_info[_FIELD_MAX_COUNTER_POOLS];

    /* Though Tomahawk supports 20 flex counter pools , as of now only 16 pools
     * are initialized and operating. Remain 4 pools are unused. num_pools has
     * to be set to 20 once the remaining 4 pools are supported.
     */
    num_pools = _FIELD_MAX_COUNTER_POOLS;
    direction = bcmStatFlexDirectionIngress;
    rv = _bcm_esw_stat_flex_pool_info_multi_get(unit,
                                                direction,
                                                num_pools,
                                                pool_info,
                                                &actual_num_pools);
    BCM_IF_ERROR_RETURN(rv);

    /* Used entries in any pool is zero means flex stat module disassociated
     * that pool from Field module and can be assigned to any other module.
     * So field group should also disassociate the pool from its in use counter
     * pool bitmap.
     */
    if (0 == pool_info[pool_id].used_entries) {

        /* If the pool is in use for the group, disassocite it now. */
        if (_FP_COUNTER_POOL_BMP_TEST(
                            f_ent->group->counter_pool_bmp, pool_id)) {
            _FP_COUNTER_POOL_BMP_REMOVE(
                            f_ent->group->counter_pool_bmp, pool_id);
        }
    }

    return BCM_E_NONE;
}
/*
 * Function:
 *     _bcm_field_th_flex_counter_free
 *
 * Purpose:
 *     Free flex counter resources used by field entry/entries.
 * Parameters:
 *     unit     - (IN) BCM device number.
 *     f_ent    - (IN) Entry array policer belongs to.
 *     f_st     - (IN) Statistics entity.
 *     mem      - (IN) Policy Table name.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_th_flex_counter_free(int unit,
                                _field_entry_t *f_ent,
                                _field_stat_t *f_st, soc_mem_t mem)
{
    int              rv; /* Operational status. */
    int              entry_idx; /* Entry index in a slice. */
    _field_control_t *fc; /* Field Control Structure. */
    _field_stage_t   *stage_fc; /* Field Stage structure.*/

    if (NULL == f_ent || NULL == f_st || INVALIDm == mem) {
        return BCM_E_PARAM;
    }

    /* Get pipeline stage information. */
    BCM_IF_ERROR_RETURN
        (_field_stage_control_get(unit,
                                  f_ent->group->stage_id,
                                  &stage_fc
                                  )
        );

    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));


    /* Detach flex stat only if it has been created. */
    if (f_st->flex_mode != 0) {
        /*
         * Get entry index at which flex STAT information
         * has been programmed in policy table.
         */
        BCM_IF_ERROR_RETURN
            (_bcm_field_slice_offset_to_tcam_idx(unit,
                                                 stage_fc,
                                                 _FP_DEF_INST,
                                                 f_ent->fs->slice_number,
                                                 f_ent->slice_idx,
                                                 &entry_idx
                                                 ));

        /* (Intra Slice) Double Wide/ Triple wide modes, IFP_TCAM_WIDE have 256 entries
         * but IFP_POLICY_TABLE will have 512 entries.
         */
        if ((!(f_ent->group->flags & _FP_GROUP_SPAN_SINGLE_SLICE)
            || (f_ent->group->flags & _FP_GROUP_INTRASLICE_DOUBLEWIDE))) {
            entry_idx = entry_idx + (f_ent->fs->slice_number * 256);  
        }


        /* Disassociate flex counters from the IFP policy table entry
         * they are attached to. Once detached, the flex counters are
         * no longer updated even if matching traffic hits the corresponding
         * IFP policy table entry.
         */
        rv = _bcm_esw_stat_flex_detach_ingress_table_counters
                (unit,
                 mem,
                 entry_idx
                 );
        /*
         * Flex STATs module initialized before field module, ignore
         * error during bcm_field_detach().
         */
        if (BCM_FAILURE(rv) && (TRUE == fc->init)) {
            return (rv);
        }

        /* Release counter resources and perform associated cleanup in flex stat
         * module. This function must be called, once the associated statistics
         * collection function is no longer needed, and the reserved counter resources
         * can be freed for use by other applications. Once this function is called,
         * the counters associated with the given stat counter id are no longer
         * available for any further stat collection.*/

        rv = bcm_esw_stat_group_destroy(unit, f_st->flex_mode);

        /*
         * Flex STATs module initialized before field module, ignore
         * error during bcm_field_detach(). 
         * Ignore:BCM_E_INTERNAL (as multiple stat entries use same SingleMode
         */
        if (rv == BCM_E_INTERNAL) { /* Doesn't look appropriate error code but*/
            rv = BCM_E_NONE;
        }
        if (BCM_FAILURE(rv) && (TRUE == fc->init)) {
            return (rv);
        }
        /* Check and if required disassociate counter pools from the group. */
        rv = _bcm_field_th_group_counter_pbm_clear(unit, f_ent,
                                                   f_st->pool_index);
        BCM_IF_ERROR_RETURN(rv);

        /* Reset the field stat attributes. */
        f_st->hw_index   = _FP_INVALID_INDEX;
        f_st->pool_index = _FP_INVALID_INDEX;
        f_st->hw_entry_count = 0;
        f_st->flex_mode = 0;

    }

    return (BCM_E_NONE);
}
/*
 * Function:
 *     _bcm_field_th_ingress_stat_hw_free
 *
 * Purpose:
 *     Deallocate hw counter from an entry.
 *
 * Parameters:
 *     unit      - (IN) BCM device number.
 *     f_ent     - (IN) Entry statistics belongs to.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_th_ingress_stat_hw_free (int unit, _field_entry_t *f_ent)
{
    int                  rv;         /* Operation return status.          */
    int                  idx;        /* Statistics iteration index.       */
    int                  entry_idx;  /* Entry index at policy table       */
    uint64               value;      /* 64 bit zero to reset hw value.    */
    soc_mem_t            tcam_mem;   /* Tcam Table Name.                  */
    soc_mem_t            policy_mem; /* Policy Table Name.                */
    _field_stat_t        *f_st;      /* Statistics entity descriptor.     */
    _field_stage_t       *stage_fc;  /* pipeline stage                    */
    _field_control_t     *fc;        /* Field Control Structure.          */
    _field_entry_stat_t  *f_ent_st;  /* Field entry statistics structure. */

    /* Input parameters check. */
    if (NULL == f_ent) {
        return (BCM_E_PARAM);
    }

    f_ent_st = &f_ent->statistic;
    /* Skip uninstalled statistics entity. */
    if (0 == (f_ent_st->flags & _FP_ENTRY_STAT_INSTALLED)) {
        return (BCM_E_NONE);
    }

    /* Get pipeline stage information. */
    BCM_IF_ERROR_RETURN
        (_field_stage_control_get(unit,
                                  f_ent->group->stage_id,
                                  &stage_fc
                                  )
        );

    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    /* Read stat entity configuration. */
    BCM_IF_ERROR_RETURN(_bcm_field_stat_get(unit, f_ent_st->sid, &f_st));

    BCM_IF_ERROR_RETURN(_bcm_field_th_tcam_policy_mem_get(unit, f_ent, 
                                                      &tcam_mem, &policy_mem));
    /* Decrement hw reference count. */
    if (f_st->hw_ref_count > 0) {
        f_st->hw_ref_count--;
        if (f_st->flex_mode != 0 && f_st->hw_ref_count > 0) {
            /*
             * Get entry index at which flex STAT information
             * has been programmed in policy table.
             */
            BCM_IF_ERROR_RETURN
                (_bcm_field_slice_offset_to_tcam_idx(unit,
                                                     stage_fc,
                                                     _FP_DEF_INST,
                                                     f_ent->fs->slice_number,
                                                     f_ent->slice_idx,
                                                     &entry_idx
                                                     )
                );

           /* (Intra Slice) Double Wide/ Triple wide modes, IFP_TCAM_WIDE have 256 entries
            * but IFP_POLICY_TABLE will have 512 entries.
            */
           if ((!(f_ent->group->flags & _FP_GROUP_SPAN_SINGLE_SLICE)
               || (f_ent->group->flags & _FP_GROUP_INTRASLICE_DOUBLEWIDE))) {
               entry_idx = entry_idx + (f_ent->fs->slice_number * 256);
           }

           /* Disassociate flex counters from the IFP policy table entry
            * they are attached to. Once detached, the flex counters are
            * no longer updated even if matching traffic hits the corresponding
            * IFP policy table entry.
            */
            rv = _bcm_esw_stat_flex_detach_ingress_table_counters
                    (unit,
                     policy_mem,
                     entry_idx
                     );
            /*
             * Flex STAT module detached before field module, ignore
             * error during bcm_field_detach().
             */
            if (BCM_FAILURE(rv) && (TRUE == fc->init)) {
                return (rv);
            }
        }
    }

    /* Statistics is not used by any other entry. */
    if (f_st->hw_ref_count == 0) {
        COMPILER_64_ZERO(value);
        /* Read & Reset  individual statistics. */
        for (idx = 0; idx < f_st->nstat; idx++) {
            rv = _field_stat_value_get(unit, 0, f_st, f_st->stat_arr[idx],
                                       f_st->stat_values + idx);
            if (BCM_FAILURE(rv)) {
                /* During system restart flex stat module
                 * might be reinitialized before fp reinit. Hence
                 * calls to the flex stat module will result in
                 * BCM_E_NOT_FOUND * error code.
                 */
                if (0 == ((_BCM_FIELD_STAGE_INGRESS == f_st->stage_id)
                    && (BCM_E_NOT_FOUND == rv))) {
                    return rv;
                }
            } else {
                rv = _field_stat_value_set(unit, f_st,
                                           f_st->stat_arr[idx], value);
                BCM_IF_ERROR_RETURN(rv);
            }
        }

        /* Mark hw resource as unused in sw. */
        rv = _bcm_field_th_flex_counter_free(unit, f_ent, f_st, policy_mem);
        BCM_IF_ERROR_RETURN(rv);
    }

    /*
     * If qualifiers have not been modified for this entry,
     * set Action only dirty flag.
     */
    if (0 == (f_ent->flags & _FP_ENTRY_DIRTY)) {
        f_ent->flags |= _FP_ENTRY_ACTION_ONLY_DIRTY;
    }

    f_ent_st->flags &= ~_FP_ENTRY_STAT_INSTALLED;

    /* Mark entry for reinstall. */
    f_ent->flags |= _FP_ENTRY_DIRTY;
    return (BCM_E_NONE);
}
/*
 * Function:
 *     _bcm_field_th_stat_hw_free
 *
 * Purpose:
 *     Deallocate hw counter from an entry.
 *
 * Parameters:
 *     unit      - (IN) BCM device number.
 *     f_ent     - (IN) Entry statistics belongs to.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_th_stat_hw_free(int unit, _field_entry_t *f_ent)
{
    int rv; /* Operational Status. */

    if (NULL == f_ent) {
        return BCM_E_PARAM;
    }

    if (_BCM_FIELD_STAGE_INGRESS == f_ent->fs->stage_id) {
        rv = _bcm_field_th_ingress_stat_hw_free(unit, f_ent);
    } else {
        rv = _bcm_field_stat_hw_free(unit, f_ent);
    }

    return rv;
}
/*
 * Function:
 *     _bcm_field_stat_array_get
 *
 * Purpose:
 *     Get field stat types array from flex stat attribute array.
 *
 * Parameters:
 *     unit        - (IN) BCM device number.
 *     nattr       - (IN) Number of attributes.
 *     stat_attr   - (IN) Flex stat attribute array.
 *     nstat       - (OUT) Number of field stat types.    
 *     stata_arr   - (OUT) Field stat array.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_stat_array_get(int unit,
                          uint32 nattr,
                          bcm_stat_group_mode_attr_selector_t *stat_attr,
                          int *nstat, bcm_field_stat_t *stat_arr)
{
     int idx = 0; /* Index Variable. */
     int packets = -1;  /* Al coloreed packets. */
     int bytes = -1;    /* All colored bytes. */
     int g_packets = -1; /* Green Packets. */
     int y_packets = -1; /* Yellow Packets. */
     int r_packets = -1; /* Red Packets. */    
     int g_bytes = -1; /* Green Bytes. */
     int y_bytes = -1; /* Yellow Bytes. */
     int r_bytes = -1; /* Red Bytes. */
     int ng_packets = -1; /* Not Green Packets. */
     int ny_packets = -1; /* Not Yellow Packets. */
     int nr_packets = -1; /* Not Red Packets. */    
     int ng_bytes = -1; /* Not Green Bytes. */
     int ny_bytes = -1; /* Not Yellow Bytes. */
     int nr_bytes = -1; /* Not Red Bytes. */
     int g_offset = -1; /* Green counter offset. */
     int y_offset = -1; /* Yellow Counter Offset. */
     int r_offset = -1; /* Red Counter Offset. */
     int color = 0; /* Bitmap of G, Y and R colors. */
     int g_present = 0; /* Green Packets Present. */
     int y_present = 0; /* Yellow Packets Present. */
     int r_present = 0; /* Red Packets Present. */

     /* Parameters Check. */

     /* Max number of attributes for any flex stat Id should not excced 3, as
      * the max counters attached to an ifp entry doesnot exceed 3. 
      */
     if (nattr <= 0 || nattr > 3) {
         return BCM_E_PARAM;
     }

     if (NULL == nstat || NULL == stat_attr || NULL == stat_arr) {
         return BCM_E_PARAM;
     }

     *nstat = 0;

     /* Get the flex stat Colors and flex stat counters offsets.  */ 
     for (idx = 0; idx < nattr; idx++) {
         if (NULL != (stat_attr + idx)) {
             if (bcmColorGreen == (stat_attr + idx)->attr_value) {
                 g_present = 1;
                 g_offset = (stat_attr + idx)->counter_offset;
             } else if (bcmColorYellow == (stat_attr + idx)->attr_value) {
                 y_present = 1;
                 y_offset = (stat_attr + idx)->counter_offset;
             } else if (bcmColorRed == (stat_attr + idx)->attr_value) {
                 r_present = 1;
                 r_offset = (stat_attr + idx)->counter_offset;
             }
         }
     }

     color = (r_present << 2) | (y_present << 1) | g_present;

     switch (color) {
         case 1:
             /* If flex Stat has bcmColorGreen, then include Green Packets and 
              * Green Bytes into field stat types. 
              */
             if (-1 != g_offset) {
                 g_packets = 1;
                 g_bytes = 1;
                 *nstat = 2;
             } else {
                 return BCM_E_INTERNAL;
             }
             break;
         case 2:
             /* If flex Stat has bcmColorYellow, then include Yellow Packets and
              * Yellow Bytes into field stat types. 
              */
             if (-1 != y_offset) {
                 y_packets = 1;
                 y_bytes = 1;
                 *nstat = 2;
             } else {
                 return BCM_E_INTERNAL;
             }
             break;
         case 3:
             /* flex stat has both bcmColorGreen and bcmColorYellow. */
             if (-1 != g_offset && -1 != y_offset) {
                 /* If flex stat has both bcmColorGreen and bcmColorYellow, 
                  * and corrsponding counter offsets are same include 
                  * NotRedPackets and NotRedBytes into field stat types. 
                  */
                 if (g_offset == y_offset) {
                     nr_packets = 1;
                     nr_bytes = 1;
                     *nstat = 2;
                 } else {
                     /* If flex stat has both bcmColorGreen and bcmColorYellow, 
                      * and corrsponding counter offsets are not same include 
                      * GreenPackets, GreenBytes, YellowPackets and YellowBytes 
                      * into field stat types. 
                      */
                     g_packets = 1;
                     g_bytes = 1;
                     y_packets = 1;
                     y_bytes = 1;
                     *nstat = 4;
                 }
             } else {
                 return BCM_E_INTERNAL;
             }
             break;
         case 4:
             /* If flex Stat has bcmColorYellow, then include Yellow Packets and
              * Yellow Bytes into field stat types. 
              */
             if (-1 != r_offset) {
                 r_packets = 1;
                 r_bytes = 1;
                 *nstat = 2;
             } else {
                 return BCM_E_INTERNAL;
             }
             break;
         case 5:
             /* flex stat has both bcmColorGreen and bcmColorRed. */
             if (-1 != g_offset && -1 != r_offset) {
                 if (g_offset == y_offset) {
                     /* If flex stat has both bcmColorGreen and bcmColorRed, 
                      * and corrsponding counter offsets are same include 
                      * NotYellowPackets and NotYellowBytes into field stat 
                      * types. 
                      */
                     ny_packets = 1;
                     ny_bytes = 1;
                     *nstat = 2;
                 } else {
                     /* If flex stat has both bcmColorGreen and bcmColorRed, 
                      * and corrsponding counter offsets are not same include 
                      * GreenPackets, GreenBytes, RedPackets and RedBytes 
                      * into field stat types. 
                      */
                     g_packets = 1;
                     g_bytes = 1;
                     r_packets = 1;
                     r_bytes = 1;
                     *nstat = 4;
                 }
             } else {
                 return BCM_E_INTERNAL;
             }
             break;
         case 6:
             /* flex stat has both bcmColorYellow and bcmColorRed. */
             if (-1 != r_offset && -1 != y_offset) {
                 if (r_offset == y_offset) {
                     /* If flex stat has both bcmColorYellow and bcmColorRed, 
                      * and corrsponding counter offsets are same include 
                      * NotGreenPackets and NotGreenBytes into field stat 
                      * types. 
                      */
                     ng_packets = 1;
                     ng_bytes = 1;
                     *nstat = 2;
                 } else {
                     /* If flex stat has both bcmColorGreen and bcmColorRed, 
                      * and corrsponding counter offsets are not same include 
                      * YellowPackets, YellowBytes, RedPackets and RedBytes 
                      * into field stat types. 
                      */
                     r_packets = 1;
                     r_bytes = 1;
                     y_packets = 1;
                     y_bytes = 1;
                     *nstat = 4;
                 }
             } else {
                 return BCM_E_INTERNAL;
             }
             break;
         case 7:
             /* flex stat has ColorGreen , ColorYellow and ColorRed. */
             if (-1 != g_offset && -1 != y_offset && -1 != r_offset) {
                 if (g_offset == y_offset && g_offset == r_offset) {
                     /* Flex stat has bcmColorGreen ,bcmColorYellow and 
                      * bcmColorRed, and corrsponding counter offsets are same 
                      * include Packets and Bytes into field stat 
                      * types. 
                      */
                     packets = 1;
                     bytes = 1;
                     *nstat = 2;
                 } else if (g_offset == y_offset && g_offset != r_offset) {
                     /* Flex stat has bcmColorGreen ,bcmColorYellow and 
                      * bcmColorRed. Green and Yellow counter offsets are same 
                      * but have different red counter offset then include 
                      * NotRedPackets, NotRedBytes, RedPackets and RedBytes into
                      * field stat types. 
                      */
                     nr_packets = 1;
                     nr_bytes = 1;
                     r_packets = 1;
                     r_bytes = 1;
                     *nstat = 4;
                 } else if (g_offset != y_offset && g_offset == r_offset) {
                     /* Flex stat has bcmColorGreen ,bcmColorYellow and 
                      * bcmColorRed. Green and Red counter offsets are same 
                      * but have different Yellow counter offset then include 
                      * NotYellowPackets, NotYellowBytes, YellowPackets and 
                      * YellowBytes into field stat types. 
                      */
                     ny_packets = 1;
                     ny_bytes = 1;
                     y_packets = 1;
                     y_bytes = 1;
                     *nstat = 4;
                 } else if(r_offset == y_offset && g_offset != r_offset) {
                     /* Flex stat has bcmColorGreen ,bcmColorYellow and 
                      * bcmColorRed. Yellow and Red counter offsets are same 
                      * but have different Green counter offset then include 
                      * NotGreenPackets, NotGreenBytes, GreenPackets and 
                      * GreenBytes into field stat types. 
                      */
                     ng_packets = 1;
                     ng_bytes = 1;
                     g_packets = 1;
                     g_bytes = 1;
                     *nstat = 4;
                 } else if (r_offset != y_offset && g_offset != r_offset && 
                            y_offset != g_offset) {
                     /* Flex stat has bcmColorGreen ,bcmColorYellow and 
                      * bcmColorRed and corrsponding counter offsets are 
                      * different then include GreenPackets, GreenBytes, 
                      * YellowPackets, YellowBytes , NotGreenPackets, 
                      * NotGreenBytes, NotYellowPackets, NotYellowBYtes, 
                      * NoteRedPackets, NotRedBytes, Packets and Bytes into 
                      * field stat types. 
                      */
                     g_packets = 1;
                     g_bytes = 1;                
                     y_packets = 1;
                     y_bytes = 1;
                     r_packets = 1;
                     r_bytes = 1;
                     ng_packets = 1;
                     ng_bytes = 1;                
                     ny_packets = 1;
                     ny_bytes = 1;
                     nr_packets = 1;
                     nr_bytes = 1;
                     packets = 1;
                     bytes = 1;
                     *nstat = 14;
                 }
             } else {
                 return BCM_E_INTERNAL;
             }
             break;
         default:
             return BCM_E_INTERNAL;
     }

     /* Number field stat types cannot exceed 14 */
     if (*nstat > 14) {
         return BCM_E_INTERNAL;
     }

     /* Update the field stat types belongs given flex stat attributes. */
     idx = 0;
     if (-1 != g_packets) {
         *(stat_arr+ idx) = bcmFieldStatGreenPackets;
         idx++;
     }

     if (-1 != y_packets) {
         *(stat_arr+ idx) = bcmFieldStatYellowPackets;
         idx++;
     }

     if (-1 != r_packets) {
         *(stat_arr+ idx) = bcmFieldStatRedPackets;
         idx++;
     }

     if (-1 != g_bytes) {
         *(stat_arr+ idx) = bcmFieldStatGreenBytes;
         idx++;
     }

     if (-1 != y_bytes) {
         *(stat_arr+ idx) = bcmFieldStatYellowBytes;
         idx++;
     }

     if (-1 != r_bytes) {
         *(stat_arr+ idx) = bcmFieldStatRedBytes;
         idx++;
     }

     if (-1 != ng_packets) {
         *(stat_arr+ idx) = bcmFieldStatNotGreenPackets;
         idx++;
     }

     if (-1 != ny_packets) {
         *(stat_arr+ idx) = bcmFieldStatNotYellowPackets;
         idx++;
     }

     if (-1 != nr_packets) {
         *(stat_arr+ idx) = bcmFieldStatNotRedPackets;
         idx++;
     }

     if (-1 != ng_bytes) {
         *(stat_arr+ idx) = bcmFieldStatNotGreenBytes;
         idx++;
     }

     if (-1 != ny_bytes) {
         *(stat_arr+ idx) = bcmFieldStatNotYellowBytes;
         idx++;
     }

     if (-1 != nr_bytes) {
         *(stat_arr+ idx) = bcmFieldStatNotRedBytes;
         idx++;
     }

     if (-1 != packets) {
         *(stat_arr+ idx) = bcmFieldStatPackets;
         idx++;
     }

     if (-1 != bytes) {
         *(stat_arr+ idx) = bcmFieldStatBytes;
         idx++;
     }

     return BCM_E_NONE;
}
/*
 * Function:
 *     _bcm_field_th_stat_attach
 *
 * Purpose:
 *     Attach flex counters created using flex stat API's to fie;ld group.
 *
 * Parameters:
 *     unit        - (IN) BCM device number.
 *     field_group - (IN) Field Group Id.
 *     flex_sid    - (IN) Flex stat Id created using flex stat API's.
 *     stat_id     - (OUT) Field stat id mapped to flex_sid.    
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_th_stat_attach(int unit,
                          bcm_field_group_t field_group,
                          uint32 flex_sid, uint32 *stat_id)
{
    int rv; /* Operational Status. */
    int nstat;  /* Number of stats attached to stat id. */
    int fp_stat_mode_max;  /* Max stat modes. */
    uint8  idx; /* Index variable to carry loop count. */
    uint32 pool_num;          /* Flex Stat Hw Pool No. */
    uint32 base_index;        /* Flex Stat counter base index. */
    uint32 num_hw_cntrs;      /* Number of counters allocated.   */
    uint32 req_bmap;          /* Requested statistics bitmap.    */
    uint32 hw_bmap;           /* HW supported statistics bitmap. */
    uint32 nattr;             /* Number of flex stat attribute types. */
    _field_control_t *fc;     /* Field Control OPerationational structure. */
    _field_stat_t *f_st;      /* Field statistics descriptor.   */
    bcm_field_stat_t stat_arr[14]; /* Allowable stat types to attach. */
    bcm_stat_group_mode_t   stat_mode;   /* Stat type bcmStatGroupModeXXX. */
    bcm_stat_object_t       stat_obj;    /* Stat object type. */
    bcm_stat_flex_mode_t    offset_mode; /* Counter mode.     */
    bcm_stat_flex_direction_t direction; /* Ingress/Egress Flex Stats. */
    bcm_stat_group_mode_attr_selector_t stat_attr[3]; /* Flex stat attrs. */


    /* Null parameter Check */
    if (NULL == stat_id) {
        return BCM_E_PARAM;
    }

    /* Get flex counter hardware details stored in flex stat ID.*/
    _bcm_esw_stat_get_counter_id_info(unit, 
                                      flex_sid,
                                      &stat_mode,
                                      &stat_obj,
                                      &offset_mode,
                                      &pool_num,
                                      &base_index);

    /* Validate the stat object is in range. */
    BCM_IF_ERROR_RETURN(_bcm_esw_stat_validate_object(unit,
                                                      stat_obj, &direction));
    /* Validate the stat mode. */
    BCM_IF_ERROR_RETURN(_bcm_esw_stat_validate_group(unit, stat_mode));

    /* Validate stat object. */
    if (bcmStatObjectIngFieldStageIngress != stat_obj) {
        LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
              "Retrieved object need to be bcmStatObjectFieldStageIngress\n")));
        return BCM_E_PARAM;
    }

    /* Get flex stat attributes array for the given flex stat Id. */
    rv = _bcm_flex_ctr_field_stat_types_get(unit, flex_sid,
                                            stat_attr, &nattr);

    BCM_IF_ERROR_RETURN(rv);

    sal_memset(stat_arr, 0, sizeof(stat_arr));

    /* Get the field stat types from flex stat attributes array. */
    rv = _bcm_field_stat_array_get(unit, nattr, stat_attr, &nstat, stat_arr);
    BCM_IF_ERROR_RETURN(rv);

    /* Get the field control operational structure. */
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));
    FP_LOCK(fc);

    /* Create field stat corrsponding to flext stat id. */
    rv = bcm_esw_field_stat_create(unit, field_group, nstat,
                                          stat_arr, (int *)stat_id);

    if (BCM_FAILURE(rv)) {
        FP_UNLOCK(fc);
        return (rv);
    }

    /* Get the newly created stat operational structure. */
    rv = _bcm_field_stat_get(unit, *stat_id, &f_st);
    if(BCM_FAILURE(rv)) {
       FP_UNLOCK(fc);
       bcm_esw_field_stat_destroy(unit,*stat_id);
       return rv;
    }

    /* Get application requested bitmap. */
    BCM_IF_ERROR_RETURN
        (_bcm_field_stat_array_to_bmap(unit, f_st, &req_bmap));

    fp_stat_mode_max = COUNTOF(th_ingress_cntr_hw_mode_tbl);

    for (idx = 0; idx < fp_stat_mode_max; idx++) {

         hw_bmap = th_ingress_cntr_hw_mode_tbl[idx].hw_bmap;
         num_hw_cntrs = th_ingress_cntr_hw_mode_tbl[idx].hw_entry_count;

         if (0 == ((req_bmap | hw_bmap) & ~(hw_bmap))) {
             /*
              * For maximizing utilization of hardware counters, this should
              * select the hw_mode both supports the requested stats
              * AND minimizes the number of required hardware counters.
              */
              break;
         }
    }

    /* Update the flex mode with flex stat id so that during entry installs 
     * h/w resources will not be allocated agagin. Also update the other 
     * Stat attributes.
     */
    f_st->flex_mode = flex_sid;
    f_st->pool_index = pool_num;
    f_st->hw_index = base_index;
    f_st->hw_mode = offset_mode;
    f_st->hw_entry_count = num_hw_cntrs;

    FP_UNLOCK(fc);

    return BCM_E_NONE;
}
/*
 * Function:
 *     _bcm_field_group_flex_ctr_info_get
 * Purpose:
 *     To get the miscellaenous information about field group.
 * Parameters:
 *     unit      - (IN) BCM device number.
 *     group     - (IN) Field groups Id.
 *     instance  - (OUT) Pipe to which Field groupd belongs to.
 *     counter_pool_bmp - (OUT) Bitmap of flex Counter pools in which 
 *                              counters are allocated for field group. 
 * Returns:
 *     BCM_E_XXX
 */
int 
_bcm_field_group_flex_ctr_info_get(int unit, 
                               bcm_field_group_t group, int *instance, 
                               _field_counter_pool_bmp_t *counter_pool_bmp)
{
    int rv;                           /* Operation Status.             */
    _field_group_t *fg = NULL;        /* Group Operational Structure.  */
    bcm_field_group_oper_mode_t mode; /* Stage Group operational mode. */
    
    if (NULL == instance || NULL == counter_pool_bmp) {
        return BCM_E_PARAM;
    }
   
    /* Get the group operational structure. */ 
    BCM_IF_ERROR_RETURN(_field_group_get(unit, group, &fg));

    /* Get the field stage's group operational mode. */
    rv = _bcm_field_th_group_oper_mode_get(unit, 
                                           bcmFieldQualifyStageIngress, &mode);
    BCM_IF_ERROR_RETURN(rv);

    if (bcmFieldGroupOperModePipeLocal == mode) {
        /* Group instance holds pipe to which this group belongs to. */
        *instance = fg->instance;    
    } else if (bcmFieldGroupOperModeGlobal == mode) {
        /* In global mode group instance is set to _FP_DEF_INST(= 0), So to 
         * dis ambiguate from pipe 0 in PipeLocal mode, instance is given back
         * as -1.
         */
        *instance = -1;
    } else {
        /* No other mode are supported. */
        return BCM_E_INTERNAL;
    }

    /* Copy counter pool bitmap from group operational structure. */
    sal_memcpy(counter_pool_bmp, &fg->counter_pool_bmp, 
                    sizeof(_field_counter_pool_bmp_t)); 

    return BCM_E_NONE;
}
/*
 * Function:
 *     _bcm_field_group_flex_ctr_pool_bmp_update
 * Purpose:
 *     Update the counter pool bitmap of field group.
 * Parameters:
 *     unit      - (IN) BCM device number.
 *     group     - (IN) Field groups Id.
 *     pool_number - (IN) Flex counter pool number. 
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_group_flex_ctr_pool_bmp_update(int unit, 
                                         bcm_field_group_t group, 
                                         int pool_number)
{
    _field_group_t *fg = NULL; /* Group Operational Structure.  */

    /* Get the group operational structure. */
    BCM_IF_ERROR_RETURN(_field_group_get(unit, group, &fg));

    /*Update the group's flex counter pool bitmap. */ 
    _FP_COUNTER_POOL_BMP_ADD(fg->counter_pool_bmp, pool_number);

    return BCM_E_NONE;
}
/*
 * Function:
 *     _bcm_field_th_ingress_stat_hw_alloc
 *
 * Purpose:
 *     Allocate flex stat hardware resource
 *
 * Parameters:
 *     unit      - (IN) BCM device number.
 *     f_ent     - (IN) Entry array policer belongs to.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_th_ingress_stat_hw_alloc(int unit,
                                    _field_entry_t *f_ent)
{
    int    rv;                /* Return Operational Status.      */
    int    fp_stat_mode_max;  /* Max stat modes.                 */
    uint8  idx;               /* HW counter modes iterator.      */
    uint32 flex_sid;          /* Flex Stat ID.                   */
    uint32 num_hw_cntrs;      /* Number of counters allocated.   */
    uint32 pool_num;          /* Flex Stat Hw Pool No.           */
    uint32 base_index;        /* Flex Stat counter base index.   */
    uint32 num_selectors;     /* NUmber of flex attr slectors.   */
    uint32 req_bmap;          /* Requested statistics bitmap.    */
    uint32 hw_bmap;           /* HW supported statistics bitmap. */
    uint32 mode_id;           /* Flex Stat Mode. */
    uint32 num_counters;      /* Number of flex counters required. */
    _field_stat_color_t     color;        /* Flex Stat Mode in s/w.         */
    _field_entry_stat_t     *f_ent_st;    /* Field entry stat collector.    */
    _field_stat_t           *f_st;        /* Field statistics descriptor.   */
    bcm_stat_flex_mode_t    offset_mode;  /* Counter mode.                  */
    bcm_stat_group_mode_t   stat_mode;    /* Stat type bcmStatGroupModeXXX. */
    bcm_stat_object_t       stat_obj;     /* Stat object type.              */
    bcm_stat_group_mode_id_config_t stat_config; /* Flex stat configuration. */
    bcm_stat_group_mode_attr_selector_t attr_sel[6]; /* Flext stat Attribute
                                                      * selector structure. */

    /* Input parameters check. */
    if (NULL == f_ent) {
        return (BCM_E_PARAM);
    }

    /* Get field entry STAT structure. */
    f_ent_st = &f_ent->statistic;

    /* Read STAT configuration. */
    rv = (_bcm_field_stat_get(unit, f_ent_st->sid, &f_st));
    if (rv == BCM_E_NOT_FOUND) {
       /* No Stats attached */
       return BCM_E_NONE;
    } else if (BCM_FAILURE(rv)) {
       return rv;
    }

    /* If counter is disabled skip allocation. */
    if (0 == f_st->hw_stat) {
        LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
                               "FP(unit %d): Stat is not enabled.\n "), unit));
        return (BCM_E_INTERNAL);
    }

    /* Get application requested bitmap. */
    BCM_IF_ERROR_RETURN
        (_bcm_field_stat_array_to_bmap(unit, f_st, &req_bmap));

    /* If bitmap is zero - no mode selection is required. */
    if (0 == req_bmap) {
        LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
                                  "FP(unit %d): No Valid stats.\n "), unit));
        return BCM_E_INTERNAL;
    }


    if (_FP_INVALID_INDEX == f_st->hw_index) {

        fp_stat_mode_max = COUNTOF(th_ingress_cntr_hw_mode_tbl);

        for (idx = 0; idx < fp_stat_mode_max; idx++) {

            hw_bmap = th_ingress_cntr_hw_mode_tbl[idx].hw_bmap;
            num_hw_cntrs = th_ingress_cntr_hw_mode_tbl[idx].hw_entry_count;
            color = th_ingress_cntr_hw_mode_tbl[idx].color;

            if (0 == ((req_bmap | hw_bmap) & ~(hw_bmap))) {
                /*
                 * For maximizing utilization of hardware counters, this should
                 * select the hw_mode both supports the requested stats
                 * AND minimizes the number of required hardware counters.
                 */
                break;
            }
        }

        if (idx >= fp_stat_mode_max) {
            return (BCM_E_INTERNAL);
        }

        for (idx = 0; idx < 6; idx++) {
            /* Initialize attribute selectors to 0 */
            bcm_stat_group_mode_attr_selector_t_init(&attr_sel[idx]);
        }

        switch (color) {
            case _bcmFieldStatColorGreen:
                attr_sel[0].attr = bcmStatGroupModeAttrFieldIngressColor;
                attr_sel[0].attr_value = bcmColorGreen;
                attr_sel[0].counter_offset = 0;
                num_selectors = 1;
                num_counters = 1;
                break;
            case _bcmFieldStatColorYellow:
                attr_sel[0].attr = bcmStatGroupModeAttrFieldIngressColor;
                attr_sel[0].attr_value = bcmColorYellow;
                attr_sel[0].counter_offset = 0;
                num_selectors = 1;
                num_counters = 1;
                break;
            case _bcmFieldStatColorRed:
                attr_sel[0].attr = bcmStatGroupModeAttrFieldIngressColor;
                attr_sel[0].attr_value = bcmColorRed;
                attr_sel[0].counter_offset = 0;
                num_selectors = 1;
                num_counters = 1;
                break;
            case _bcmFieldStatColorNotGreen:
                attr_sel[0].attr = bcmStatGroupModeAttrFieldIngressColor;
                attr_sel[0].attr_value = bcmColorYellow;
                attr_sel[0].counter_offset = 0;
                attr_sel[1].attr = bcmStatGroupModeAttrFieldIngressColor;
                attr_sel[1].attr_value = bcmColorRed;
                attr_sel[1].counter_offset = 0;
                num_selectors = 2;
                num_counters = 1;
                break;
            case _bcmFieldStatColorNotYellow:
                attr_sel[0].attr = bcmStatGroupModeAttrFieldIngressColor;
                attr_sel[0].attr_value = bcmColorGreen;
                attr_sel[0].counter_offset = 0;
                attr_sel[1].attr = bcmStatGroupModeAttrFieldIngressColor;
                attr_sel[1].attr_value = bcmColorRed;
                attr_sel[1].counter_offset = 0;
                num_selectors = 2;
                num_counters = 1;
                break;
            case _bcmFieldStatColorNotRed:
                attr_sel[0].attr = bcmStatGroupModeAttrFieldIngressColor;
                attr_sel[0].attr_value = bcmColorGreen;
                attr_sel[0].counter_offset = 0;
                attr_sel[1].attr = bcmStatGroupModeAttrFieldIngressColor;
                attr_sel[1].attr_value = bcmColorYellow;
                attr_sel[1].counter_offset = 0;
                num_selectors = 2;
                num_counters = 1;
                break;
            case _bcmFieldStatColorGreenYellow:
                attr_sel[0].attr = bcmStatGroupModeAttrFieldIngressColor;
                attr_sel[0].attr_value = bcmColorGreen;
                attr_sel[0].counter_offset = 0;
                attr_sel[1].attr = bcmStatGroupModeAttrFieldIngressColor;
                attr_sel[1].attr_value = bcmColorYellow;
                attr_sel[1].counter_offset = 1;
                num_selectors = 2;
                num_counters = 2;
                break;
            case _bcmFieldStatColorGreenRed:
                attr_sel[0].attr = bcmStatGroupModeAttrFieldIngressColor;
                attr_sel[0].attr_value = bcmColorGreen;
                attr_sel[0].counter_offset = 0;
                attr_sel[1].attr = bcmStatGroupModeAttrFieldIngressColor;
                attr_sel[1].attr_value = bcmColorRed;
                attr_sel[1].counter_offset = 1;
                num_selectors = 2;
                num_counters = 2;
                break;
            case _bcmFieldStatColorYellowRed:
                attr_sel[0].attr = bcmStatGroupModeAttrFieldIngressColor;
                attr_sel[0].attr_value = bcmColorYellow;
                attr_sel[0].counter_offset = 0;
                attr_sel[1].attr = bcmStatGroupModeAttrFieldIngressColor;
                attr_sel[1].attr_value = bcmColorRed;
                attr_sel[1].counter_offset = 1;
                num_selectors = 2;
                num_counters = 2;
                break;
            case _bcmFieldStatColorGreenNotGreen:
                attr_sel[0].attr = bcmStatGroupModeAttrFieldIngressColor;
                attr_sel[0].attr_value = bcmColorGreen;
                attr_sel[0].counter_offset = 0;
                attr_sel[1].attr = bcmStatGroupModeAttrFieldIngressColor;
                attr_sel[1].attr_value = bcmColorYellow;
                attr_sel[1].counter_offset = 1;
                attr_sel[2].attr = bcmStatGroupModeAttrFieldIngressColor;
                attr_sel[2].attr_value = bcmColorRed;
                attr_sel[2].counter_offset = 1;
                num_selectors = 3;
                num_counters = 2;
                break;
            case _bcmFieldStatColorYellowNotYellow:
                attr_sel[0].attr = bcmStatGroupModeAttrFieldIngressColor;
                attr_sel[0].attr_value = bcmColorYellow;
                attr_sel[0].counter_offset = 0;
                attr_sel[1].attr = bcmStatGroupModeAttrFieldIngressColor;
                attr_sel[1].attr_value = bcmColorGreen;
                attr_sel[1].counter_offset = 1;
                attr_sel[2].attr = bcmStatGroupModeAttrFieldIngressColor;
                attr_sel[2].attr_value = bcmColorRed;
                attr_sel[2].counter_offset = 1;
                num_selectors = 3;
                num_counters = 2;
                break;
            case _bcmFieldStatColorRedNotRed:
                attr_sel[0].attr = bcmStatGroupModeAttrFieldIngressColor;
                attr_sel[0].attr_value = bcmColorRed;
                attr_sel[0].counter_offset = 0;
                attr_sel[1].attr = bcmStatGroupModeAttrFieldIngressColor;
                attr_sel[1].attr_value = bcmColorGreen;
                attr_sel[1].counter_offset = 1;
                attr_sel[2].attr = bcmStatGroupModeAttrFieldIngressColor;
                attr_sel[2].attr_value = bcmColorYellow;
                attr_sel[2].counter_offset = 1;
                num_selectors = 3;
                num_counters = 2;
                break;
            case _bcmFieldStatColorNoColor:
                attr_sel[0].attr = bcmStatGroupModeAttrFieldIngressColor;
                attr_sel[0].attr_value = bcmColorGreen;
                attr_sel[0].counter_offset = 0;
                attr_sel[1].attr = bcmStatGroupModeAttrFieldIngressColor;
                attr_sel[1].attr_value = bcmColorYellow;
                attr_sel[1].counter_offset = 0;
                attr_sel[2].attr = bcmStatGroupModeAttrFieldIngressColor;
                attr_sel[2].attr_value = bcmColorRed;
                attr_sel[2].counter_offset = 0;
                num_selectors = 3;
                num_counters = 1;
                break;
            case _bcmFieldStatColorGreenYellowRed:
                attr_sel[0].attr = bcmStatGroupModeAttrFieldIngressColor;
                attr_sel[0].attr_value = bcmColorGreen;
                attr_sel[0].counter_offset = 0;
                attr_sel[1].attr = bcmStatGroupModeAttrFieldIngressColor;
                attr_sel[1].attr_value = bcmColorYellow;
                attr_sel[1].counter_offset = 1;
                attr_sel[2].attr = bcmStatGroupModeAttrFieldIngressColor;
                attr_sel[2].attr_value = bcmColorRed;
                attr_sel[2].counter_offset = 2;
                num_selectors = 3;
                num_counters = 3;
                break;
             default:
                 return BCM_E_INTERNAL;
        }

        /* Create custom counter update mode */
        bcm_stat_group_mode_id_config_t_init(&stat_config);
        stat_config.flags = BCM_STAT_GROUP_MODE_INGRESS;
        stat_config.total_counters = num_counters;
        stat_config.hint.type = bcmStatGroupAllocHintIngressFieldGroup;
        stat_config.hint.value = f_ent->group->gid; /* group id returned by 
                                                     * field_group_create()*/
        rv = bcm_esw_stat_group_mode_id_config_create(unit, 0, &stat_config,
                                                      num_selectors, attr_sel,
                                                      &mode_id);
        BCM_IF_ERROR_RETURN(rv);
 
        /* Create custom stat group */
        rv = bcm_esw_stat_custom_group_create(unit, mode_id, 
                                              bcmStatObjectIngFieldStageIngress,
                                              &flex_sid, &num_hw_cntrs);
        BCM_IF_ERROR_RETURN(rv);

        /* Get flex counter hardware details stored in flex stat ID.*/
        _bcm_esw_stat_get_counter_id_info(unit, flex_sid,
                                          &stat_mode,
                                          &stat_obj,
                                          &offset_mode,
                                          &pool_num,
                                          &base_index);

        /* If the pool is not set to in use for the group, set it now. */
        if (!_FP_COUNTER_POOL_BMP_TEST(
                            f_ent->group->counter_pool_bmp, pool_num)) {
            _FP_COUNTER_POOL_BMP_ADD(f_ent->group->counter_pool_bmp, pool_num);
        }

        /* Store flex stat hardware details in stat data structure. */
        f_st->flex_mode = flex_sid;
        f_st->pool_index = pool_num;
        f_st->hw_index = base_index;
        f_st->hw_mode = offset_mode;
        f_st->hw_entry_count = num_hw_cntrs;
    }

    return (BCM_E_NONE);
}
/*
 * Function:
 *     _bcm_field_th_stat_hw_alloc
 *
 * Purpose:
 *     Allocate Stat hardware resource
 *
 * Parameters:
 *     unit      - (IN) BCM device number.
 *     f_ent     - (IN) Entry array stat belongs to.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_th_stat_hw_alloc(int unit,
                            _field_entry_t *f_ent)
{
    int rv; /* Operational Status. */

    if (NULL == f_ent) {
        return BCM_E_PARAM;
    }

    if (_BCM_FIELD_STAGE_INGRESS == f_ent->fs->stage_id) {
        rv = _bcm_field_th_ingress_stat_hw_alloc(unit, f_ent);
    } else {
        rv = _bcm_field_stat_hw_alloc(unit, f_ent);
    }

    return rv;
}
/*
 * Function:
 *     _field_trx_flex_stat_action_set
 * Purpose:
 *     Install flex counter update action into policy table.
 * Parameters:
 *     unit         - (IN) BCM device number
 *     f_ent        - (IN) entry structure to get policy info from
 *     mem          - (IN) Policy table memory
 *     tcam_idx     - (IN) Common index of various tables
 *     buf          - (OUT) Field Policy table entry
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_th_flex_stat_action_set(int unit,
                                   _field_entry_t *f_ent,
                                   soc_mem_t mem,
                                   int tcam_idx,
                                   uint32 *buf)
{
    int rv; /* Return Operational Status. */
    int idx; /* Stat array index. */
    _field_stat_t *f_st; /* Field statistics descriptor. */


    /* Increment statistics hw reference count. */
    if ((f_ent->statistic.flags & _FP_ENTRY_STAT_VALID)
        && !(f_ent->statistic.flags & _FP_ENTRY_STAT_INSTALLED)) {
        /* Get statistics entity description structure. */
        rv = _bcm_field_stat_get(unit, f_ent->statistic.sid, &f_st);
        BCM_IF_ERROR_RETURN(rv);

        if (_FP_INVALID_INDEX != f_st->hw_index) {
            /*
             * For STATs that are shared by entries, hardware counters
             * are not allocated again. But reference count is incremented
             * for these counters.
             */
            f_st->hw_ref_count++;

            /* Once flex counters have been allocated for the IFP, these counters then
             * need to be attached to the appropriate IFP policy table entry. Once flex
             * counters have been attached by associating them with the appropriate IFP
             * policy table entry, counters are now ready to be updated by corresponding
             * packets.
             */
            rv = _bcm_esw_stat_flex_attach_ingress_table_counters1(unit, mem,
                                                     tcam_idx, f_st->hw_mode,
                                                              f_st->hw_index,
                                                      f_st->pool_index, buf);
            BCM_IF_ERROR_RETURN(rv);

            /* Mark entry as installed. */
            f_ent->statistic.flags |=  _FP_ENTRY_STAT_INSTALLED;

            /*
             * Write individual statistics previous value, first time
             * entry is installed in hardware.
             */
            if (1 == f_st->hw_ref_count) {
                for (idx = 0; idx < f_st->nstat; idx++) {
                    rv = _field_stat_value_set(unit, f_st, f_st->stat_arr[idx],
                                                       f_st->stat_values[idx]);
                    BCM_IF_ERROR_RETURN(rv);
                }
            }
        }
    } else {
        /* setting COUNTER_SETf to 0 will set FLEX_CTR_BASE_COUNTER_IDX,
         * FLEX_CTR_OFFSET_MODE, FLEX_CTR_POOL_NUMBER, G_COUNT, Y_COUNT and
         * R_COUNT fields to 0.
         */
        PolicySet(unit, mem, buf, COUNTER_SETf, 0);
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_field_th_stat_action_set
 * Purpose:
 *     Install counter update action into policy table.
 * Parameters:
 *     unit         - (IN) BCM device number
 *     f_ent        - (IN) entry structure to get policy info from
 *     mem          - (IN) Policy table memory
 *     tcam_idx     - (IN) Common index of various tables
 *     buf          - (OUT) Field Policy table entry
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_th_stat_action_set(int unit, _field_entry_t *f_ent,
                               soc_mem_t mem, int tcam_idx,
                               uint32 *buf)
{
    int rv = BCM_E_NONE;  /* Operation return status.     */

    if (NULL == f_ent || NULL == buf) {
        return (BCM_E_PARAM);
    }

    if ((_BCM_FIELD_STAGE_INGRESS == f_ent->group->stage_id)) {
        /* Tomahawk IFP uses Flex counters. */
        rv = _bcm_field_th_flex_stat_action_set(unit, f_ent, mem,
                                                  tcam_idx, buf);
    } else {
        /* There is no difference in EFP and VFP counters
         * when compared to Trident2.
         */
        rv = _bcm_field_trx_stat_action_set(unit, f_ent, mem, tcam_idx, buf);
    }

    return rv;
}
/*
 * Function:
 *     _bcm_field_th_multi_pipe_counter_mem_get
 *
 * Purpose:
 *     To get the pipe specific counter memory names for a given
 *     Field Processor stage(VFP/IFP/EFP).
 * Parameters:
 *     unit         - (IN)  BCM device number
 *     stage_fc     - (IN)  Field Processor stage control structure.
 *     counter_mem  - (OUT) pointer to array of counter memories.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_th_multi_pipe_counter_mem_get(int unit,
                                         _field_stage_t *stage_fc,
                                         soc_mem_t *counter_mem)
{
    int              pipe;          /* XGS pipeline to operate on.        */
    int              max_pipes;     /* Maximum XGS pipelines in the stage */

    /* Input parameters check. */
    if ((NULL == stage_fc) ||
        (NULL == counter_mem)) {
        return BCM_E_PARAM;
    }

    max_pipes = stage_fc->num_pipes;
    for (pipe = 0; pipe < max_pipes; pipe++) {
        counter_mem[pipe] = INVALIDm;
    }

    switch(stage_fc->stage_id) {
        case _BCM_FIELD_STAGE_EGRESS:
            counter_mem[0] = EFP_COUNTER_TABLE_PIPE0m;
            counter_mem[1] = EFP_COUNTER_TABLE_PIPE1m;
            counter_mem[2] = EFP_COUNTER_TABLE_PIPE2m;
            counter_mem[3] = EFP_COUNTER_TABLE_PIPE3m;
            break;
        case _BCM_FIELD_STAGE_LOOKUP:
            break;
        default:
            return BCM_E_INTERNAL;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _field_th_counter_write
 * Purpose:
 *     Write accumulated sw & hw counters
 * Parameters:
 *   unit          - (IN) BCM device number
 *   mem           - (IN) FieldProcessor(FP) Counter Memory name
 *   buf           - (IN) Memory table entry for FP counter
 *   cntrs32_buf   - (IN) s/w packet Counter value
 *   cntrs64_buf   - (IN) s/w byte Counter value
 *   packet_count  - (IN) Packet counter value
 *   byte_count    - (IN) Byte counter value
 * Returns:
 *    BCM_E_XXX
 */

STATIC int
_field_th_counter_write(int unit, soc_mem_t mem, int idx, uint32 *buf,
                            _field_counter32_collect_t *cntrs32_buf,
                            _field_counter64_collect_t *cntrs64_buf,
                            uint64 *packet_count, uint64 *byte_count)
{
    uint32  hw_val[2];     /* Parsed field counter value.*/
    uint32  len;           /* field length */

    if (NULL == buf) {
        return BCM_E_PARAM;
    }

    /* Set byte count in the memory */
    if (NULL != byte_count) {
        /* For Tomahawk field_length = 34 */
        len = soc_mem_field_length(unit, mem, BYTE_COUNTERf);

        if (NULL == cntrs64_buf) {
            return BCM_E_PARAM;
        }

        COMPILER_64_ZERO(cntrs64_buf->accumulated_counter);
        COMPILER_64_OR(cntrs64_buf->accumulated_counter, (*byte_count));
        COMPILER_64_SET(cntrs64_buf->last_hw_value,
                        COMPILER_64_HI(*byte_count) & ((1 << (len - 32)) - 1),
                        COMPILER_64_LO(*byte_count));
        hw_val[0] = COMPILER_64_LO(cntrs64_buf->last_hw_value);
        hw_val[1] = COMPILER_64_HI(cntrs64_buf->last_hw_value);
        soc_mem_field_set(unit, mem, buf, BYTE_COUNTERf, hw_val);
    }

    /* Set packet count in the memory */
    if (NULL != packet_count) {
        /* For Tomahawk field_length = 26 */
        len = soc_mem_field_length(unit, mem, PACKET_COUNTERf);

        if (NULL == cntrs32_buf) {
            return BCM_E_PARAM;
        }

        COMPILER_64_ZERO(cntrs32_buf->accumulated_counter);
        COMPILER_64_OR(cntrs32_buf->accumulated_counter,
                       (*packet_count));
        cntrs32_buf->last_hw_value =
            (COMPILER_64_LO(*packet_count) & ((1 << len) - 1));
        hw_val[0] = cntrs32_buf->last_hw_value;
        hw_val[1] = 0;
        soc_mem_field_set(unit, mem, buf, PACKET_COUNTERf, hw_val);
    }

    return soc_mem_write(unit, mem, MEM_BLOCK_ALL, idx, buf);
}

/*
 * Function:
 *    _bcm_field_th_pipe_counter_set
 *
 * Description:
 *   Service routine used to set 64-bit software accumulated counters
 *   of a field processor counter for a given pipe
 *   (Ingress/Lookup/Egress/External).
 * Parameters:
 *   unit         - (IN) BCM device number.
 *   stage_fc     - (IN) Stage field control structure.
 *   counter_mam  - (IN) FieldProcessor(FP) counter name.
 *   mem_buf      - (IN) Memory table entry for FP counter
 *   idx          - (IN) Counter hw index.
 *   pipe         - (IN) XGS pipeline to operate on.
 *   packet_count - (IN) Packet Counter value
 *   byte_count   - (IN) Byte Counter value
 * Returns:
 *    BCM_E_XXX
 */
int
_bcm_field_th_pipe_counter_set(int unit,
                            _field_stage_t *stage_fc,
                            soc_mem_t counter_mem,
                            uint32 *mem_buf,
                            int idx, int pipe,
                            uint64 *packet_count,
                            uint64 *byte_count)
{
    _field_counter32_collect_t *cntrs32_buf;  /* Sw packet counter value    */
    _field_counter64_collect_t *cntrs64_buf;  /* Sw byte counter value      */

    /* Input parameters check. */
    if ((NULL == stage_fc) ||
        (NULL == mem_buf)) {
        return BCM_E_PARAM;
    }

    if (INVALIDm != counter_mem) {
        if (NULL != mem_buf) {
            cntrs64_buf = &stage_fc->_field_64_counters[pipe][idx];
            cntrs32_buf = &stage_fc->_field_32_counters[pipe][idx];
            BCM_IF_ERROR_RETURN
                (_field_th_counter_write(unit, counter_mem,
                                             idx, mem_buf,
                                             cntrs32_buf, cntrs64_buf,
                                             packet_count, byte_count));
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *    _field_th_multi_pipe_sw_counter_set
 *
 * Description:
 *   Service routine used to set 64-bit software accumulated counters.
 * Parameters:
 *   unit            - (IN) BCM device number.
 *   stage_fc        - (IN) Stage field control structure.
 *   stat_group_pipe - (IN) XGS Pipline associated to the field group.
 *   idx             - (IN) Counter hw index.
 *   packet_count    - (IN) Packet Counter value
 *   byte_count      - (IN) Byte Counter value
 * Returns:
 *    BCM_E_XXX
 *
 */
STATIC int
_field_th_multi_pipe_sw_counter_set(int unit, _field_stage_t *stage_fc,
                                 int stat_group_pipe, int idx,
                                 uint64 *packet_count,
                                 uint64 *byte_count)
{
    int              rv;            /* Operation return status.           */
    int              pipe;          /* XGS pipeline to operate on.        */
    int              max_pipes;     /* Maximum XGS pipelines in the stage */
    uint32           *hw_buffer;    /* memory HW buffer.                  */
    uint32           *hw_buf;       /* memory HW buffer.                  */
    _field_control_t *fc;           /* Field control structure.           */
    soc_mem_t        counter_mem[_FP_MAX_NUM_PIPES]; /* FieldProcessor
                                                           (IFP/EFP/VFP)
                                                           counter memories */

    hw_buffer = NULL;

    /* Get field control structure. */
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    /* Get counter memory. */
    rv = _bcm_field_th_multi_pipe_counter_mem_get(unit, stage_fc,
                                                     counter_mem);
    BCM_IF_ERROR_RETURN(rv);

    /* HW index sanity check. */
    if (idx < soc_mem_index_min(unit, counter_mem[0]) ||
        idx > soc_mem_index_max(unit, counter_mem[0])) {
        return (BCM_E_INTERNAL);
    }

    max_pipes = stage_fc->num_pipes;
    if (_BCM_FIELD_GLOBAL_GROUP != stat_group_pipe) {
        for (pipe = 0; pipe < max_pipes; pipe++ ) {
             if (pipe != stat_group_pipe) {
                 counter_mem[pipe] = INVALIDm;
             }
        }
    }

    /* Allocate buffer for counter values in HW.  */
    _FP_XGS3_ALLOC(hw_buffer,
                   _FP_MAX_NUM_PIPES * WORDS2BYTES(SOC_MAX_MEM_FIELD_WORDS),
                   "Counter buffers");
    if (NULL == hw_buffer) {
        return (BCM_E_MEMORY);
    }

    for (pipe = 0; pipe < max_pipes; pipe++) {
        if (INVALIDm != counter_mem[pipe]) {
            hw_buf = hw_buffer + (pipe * SOC_MAX_MEM_FIELD_WORDS);
            rv = soc_mem_read(unit,
                              counter_mem[pipe],
                              MEM_BLOCK_ANY,
                              idx, hw_buf);
            if (BCM_FAILURE(rv)) {
                sal_free(hw_buffer);
                return rv;
            }

            rv = _bcm_field_th_pipe_counter_set(unit, stage_fc,
                                             counter_mem[pipe],
                                             hw_buf, idx, pipe,
                                             packet_count, byte_count);
            if (BCM_FAILURE(rv)) {
                sal_free(hw_buffer);
                return rv;
            }

            /* Write 0 to other pipeline counter value. */
            if (NULL != packet_count) {
                COMPILER_64_ZERO(*packet_count);
            }
            if (NULL != byte_count) {
                COMPILER_64_ZERO(*byte_count);
            }

        }
    }

    sal_free(hw_buffer);

    return rv;
}

/*
 * Function:
 *     _bcm_field_th_counter_read
 * Purpose:
 *     Update accumulated sw counter and return current counter value.
 * Parameters:
 *   unit          - (IN)  BCM device number
 *   stage_fc      - (IN)  Stage field control
 *   mamacc_byte   - (IN)  Memory access cache for byte counters
 *   memacc_pkt    - (IN)  Memory access cache for packet counters
 *   buf           - (IN)  Memory table entry for FP counter
 *   cntrs32_buf   - (IN)  s/w packet Counter value
 *   cntrs64_buf   - (IN)  s/w byte Counter value
 *   packet_count  - (OUT) Packet counter value
 *   byte_count    - (OUT) Byte counter value
 * Returns:
 *    BCM_E_XXX
 */
int
_bcm_field_th_counter_read(int unit, _field_stage_t *stage_fc,
                        soc_memacc_t *memacc_byte,
                        soc_memacc_t *memacc_pkt, uint32 *buf,
                        _field_counter32_collect_t *cntrs32_buf,
                        _field_counter64_collect_t *cntrs64_buf,
                        uint64 *packet_count, uint64 *byte_count)
{
    uint32  hw_val[2];     /* Parsed field counter value.*/

    if (NULL == buf ||
        NULL == memacc_byte ||
        NULL == memacc_pkt ||
        NULL == cntrs32_buf ||
        NULL == cntrs64_buf) {
        return BCM_E_PARAM;
    }

    /* Byte counter. */
    hw_val[0] = hw_val[1] = 0;
    soc_memacc_field_get(memacc_byte, buf, hw_val);

    _bcm_field_34bit_counter_update(unit, hw_val, cntrs64_buf);
    if (NULL != byte_count) {
        COMPILER_64_ADD_64(*byte_count, cntrs64_buf->accumulated_counter);
    }
    /* Packet counter. */
    hw_val[0] = hw_val[1] = 0;
    soc_memacc_field_get(memacc_pkt, buf, hw_val);
    _bcm_field_26bit_counter_update(unit, hw_val, cntrs32_buf);
    if (NULL != packet_count) {
        COMPILER_64_ADD_64(*packet_count, cntrs32_buf->accumulated_counter);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_field_th_pipe_counter_get
 * Purpose:
 *     Update accumulated sw counter and return current counter value.
 * Parameters:
 *   unit          - (IN)  BCM device number
 *   stage_fc      - (IN)  Stage field control
 *   counter_mem   - (IN)  Memory name for FP counter
 *   mem_buf       - (IN)  Memory table entry for FP counter
 *   idx           - (IN)  Counter index
 *   pipe          - (IN)  XGS pipeline to operate on
 *   packet_count  - (OUT) Packet counter value
 *   byte_count    - (OUT) Byte counter value
 * Returns:
 *    BCM_E_XXX
 */
int
_bcm_field_th_pipe_counter_get(int unit,
                            _field_stage_t *stage_fc,
                            soc_mem_t counter_mem,
                            uint32 *mem_buf,
                            int idx,
                            int pipe,
                            uint64 *packet_count,
                            uint64 *byte_count)
{
    _field_counter32_collect_t *cntrs32_buf;  /* Sw packet counter value    */
    _field_counter64_collect_t *cntrs64_buf;  /* Sw byte counter value      */
    soc_memacc_t *memacc_pkt, *memacc_byte;   /* Memory access cache.   */

    /* Input parameters check. */
    if (NULL == stage_fc) {
        return BCM_E_PARAM;
    }

    cntrs64_buf = &stage_fc->_field_64_counters[pipe][idx];
    cntrs32_buf = &stage_fc->_field_32_counters[pipe][idx];
    memacc_byte = &(stage_fc->_field_memacc_counters[(pipe * 2)]);
    memacc_pkt  = &(stage_fc->_field_memacc_counters[((pipe * 2) + 1)]);
    BCM_IF_ERROR_RETURN(_bcm_field_th_counter_read(unit, stage_fc,
                                                   memacc_byte,
                                                   memacc_pkt,
                                                   mem_buf,
                                                   cntrs32_buf, cntrs64_buf,
                                                   packet_count, byte_count));

    return BCM_E_NONE;
}

/*
 * Function:
 *    _field_th_multi_pipe_sw_counter_get
 *
 * Description:
 *       Service routine used to retrieve 64-bit software accumulated counters.
 * Parameters:
 *   unit         - (IN) BCM device number.
 *   stage_fc     - (IN) Stage field control structure.
 *   stat_group_pipe - (IN) XGS Pipline associated to the field group.
 *   idx          - (IN) Counter hw index.
 *   packet_count - (OUT) Packet Counter value
 *   byte_count   - (OUT) Byte Counter value
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
_field_th_multi_pipe_sw_counter_get(int unit,
                                 _field_stage_t *stage_fc,
                                 int stat_group_pipe,
                                 int idx,
                                 uint64 *packet_count,
                                 uint64 *byte_count)
{
    int                       rv;              /* Operation return status.    */
    int                       pipe;            /* XGS pipeline to operate on. */
    int                       max_pipes;       /* Maximum pipeline per stage. */
    uint32                    *hw_buf;         /*  memory HW  buffer.         */
    uint32                    *hw_buffer;      /*  memory HW  buffer.         */
    _field_control_t          *fc;             /* Field control structure.    */
    _field_counter32_collect_t *cntrs32_buf;   /* Sw packet counter value     */
    _field_counter64_collect_t *cntrs64_buf;   /* Sw byte counter value       */
    soc_mem_t counter_mem[_FP_MAX_NUM_PIPES];  /* FieldProcessor
                                                     (IFP/EFP/VFP)
                                                     counter memories */


    hw_buf = hw_buffer = NULL;

    /* Get field control structure. */
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    /* Get counter memory. */
    rv = _bcm_field_th_multi_pipe_counter_mem_get(unit, stage_fc,
                                                    counter_mem);
    BCM_IF_ERROR_RETURN(rv);

    /* HW index sanity check. */
    if (idx < soc_mem_index_min(unit, counter_mem[0]) ||
        idx > soc_mem_index_max(unit, counter_mem[0])) {
        return (BCM_E_INTERNAL);
    }

    max_pipes = stage_fc->num_pipes;
    if (_BCM_FIELD_GLOBAL_GROUP != stat_group_pipe) {
        for (pipe = 0; pipe < max_pipes; pipe++ ) {
             if (pipe != stat_group_pipe) {
                 counter_mem[pipe] = INVALIDm;
             }
        }
    }

    if (0 == (fc->flags & _FP_STAT_SYNC_ENABLE)) {
        for (pipe = 0; pipe < max_pipes; pipe++) {
            if (INVALIDm != counter_mem[pipe]) {
                cntrs64_buf = &stage_fc->_field_64_counters[pipe][idx];
                COMPILER_64_ADD_64(*byte_count,
                                   cntrs64_buf->accumulated_counter);
                cntrs32_buf = &stage_fc->_field_32_counters[pipe][idx];
                COMPILER_64_ADD_64(*packet_count,
                                    cntrs32_buf->accumulated_counter);
            }
        }
    } else {
        /* Allocate buffer for counter values in HW.  */
        _FP_XGS3_ALLOC(hw_buffer,
                     max_pipes * WORDS2BYTES(SOC_MAX_MEM_FIELD_WORDS),
                     "Counter buffers");
        if (NULL == hw_buffer) {
            return (BCM_E_MEMORY);
        }

        for (pipe = 0; pipe < max_pipes; pipe++) {
            if (INVALIDm != counter_mem[pipe]) {
                hw_buf = hw_buffer + (pipe * SOC_MAX_MEM_FIELD_WORDS);
                rv = soc_mem_read(unit,
                                  counter_mem[pipe],
                                  MEM_BLOCK_ANY,
                                  idx, hw_buf);
                if (BCM_FAILURE(rv)) {
                    sal_free(hw_buffer);
                    return rv;
                }
                rv = _bcm_field_th_pipe_counter_get(unit, stage_fc,
                                                 counter_mem[pipe],
                                                 hw_buf, idx, pipe,
                                                 packet_count, byte_count);
                if (BCM_FAILURE(rv)) {
                    sal_free(hw_buffer);
                    return rv;
                }

            }
        }

        sal_free(hw_buffer);

    }

    return rv;
}

/*
 * Function:
 *      _bcm_th_field_stat_value_set
 *
 * Description:
 *      Set 64 bit counter value for specific statistic type.
 * Parameters:
 *      unit      - (IN) BCM device number.
 *      f_st      - (IN) Statistics entity descriptor.
 *      stat      - (IN) Collected statistics descriptor.
 *      value     - (IN) Collected counters value.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_th_field_stat_value_set(int unit,
                             _field_stat_t *f_st,
                             bcm_field_stat_t stat,
                             uint64 value)
{
    int                 rv = BCM_E_NONE; /* Operation return status.          */
    int                 idx1;            /* First counter index to read.      */
    int                 idx2;            /* Second counter index to read.     */
    int                 idx3;            /* Third counter index to read.      */
    int                 len;             /* length of the h/w field.          */
    uint32              flags;           /* _FP_STAT_XXX.                     */
    uint64              hw_value;        /* h/w counter value to be written.  */
    uint64              reset_val;       /* Value to be set on reset          */
    _field_control_t    *fc;             /* Field control structure.          */
    bcm_stat_value_t    stat_val;        /* Stat Values structure to SET.     */
    soc_reg_t           reg;             /* Variable to hold SOC register.    */
    soc_ctr_control_info_t ctrl_info;    /* Centralized counter information.  */

    /* Input parameters check. */
    if (NULL == f_st) {
        return (BCM_E_PARAM);
    }

    /* Check that stat was requested during statistics entity creation. */
    for (idx1 = 0; idx1 < f_st->nstat; idx1++) {
        if (stat == f_st->stat_arr[idx1]) {
            break;
        }
    }
    if (idx1 == f_st->nstat) {
        return (BCM_E_PARAM);
    }

    /* Get field control structure. */
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    /* Get hw indexes and flags needed to compose requested statistic.*/
    rv = fc->functions.fp_stat_index_get(unit, f_st, stat, &idx1,
                                         &idx2, &idx3, &flags);
    BCM_IF_ERROR_RETURN(rv);

    if (_FP_INVALID_INDEX == idx1) {
        return (BCM_E_INTERNAL);
    }

    if (_BCM_FIELD_STAGE_EGRESS == f_st->stage_id) {
        if (flags & _FP_STAT_BYTES) {
            reg = SOC_COUNTER_NON_DMA_EFP_BYTE;
            /* For Tomahawk BYTE_COUNTERf field_length = 34*/
            len = soc_mem_field_length(unit, EFP_COUNTER_TABLEm, BYTE_COUNTERf);
        } else {
            reg = SOC_COUNTER_NON_DMA_EFP_PKT;
            /* For Tomahawk field_length = 26 */
            len = soc_mem_field_length(unit, EFP_COUNTER_TABLEm,
                                                PACKET_COUNTERf);
        }

        if (len < 32) {
            COMPILER_64_SET(hw_value, 0,
                            COMPILER_64_LO(value) & ((1 << len) - 1));
        } else {
            COMPILER_64_SET(hw_value,
                            COMPILER_64_HI(value) & ((1 << (len - 32)) - 1),
                                                     COMPILER_64_LO(value));
        }

        
        ctrl_info.instance = -1;
        ctrl_info.instance_type = SOC_CTR_INSTANCE_TYPE_PIPE;
        COMPILER_64_ZERO(reset_val);

        rv = soc_counter_generic_set(unit, reg,
                                     ctrl_info, 0,
                                     idx1, reset_val);
        BCM_IF_ERROR_RETURN(rv);
        /* Reset accumulated counter value at secondary index. */
        if (_FP_INVALID_INDEX != idx2) {
            rv = soc_counter_generic_set(unit, reg,
                                         ctrl_info, 0,
                                         idx2, reset_val);
            BCM_IF_ERROR_RETURN(rv);
        }

        if (_FP_INVALID_INDEX != idx3) {
            rv = soc_counter_generic_set(unit, reg,
                                         ctrl_info, 0,
                                         idx3, reset_val);
            BCM_IF_ERROR_RETURN(rv);
        }

        /* Set accumulated counter value at primary index. */
        ctrl_info.instance = 0;
        rv = soc_counter_generic_set(unit, reg,
                                     ctrl_info, 0,
                                     idx1, hw_value);
        BCM_IF_ERROR_RETURN(rv);
    } else if (_BCM_FIELD_STAGE_INGRESS == f_st->stage_id) {

        if (flags & _FP_STAT_BYTES) {
            COMPILER_64_SET(stat_val.bytes,
                            COMPILER_64_HI(value),
                            COMPILER_64_LO(value));
        } else {
            COMPILER_64_TO_32_LO(stat_val.packets, value);
        }

        /* Set accumulated counter value at primary index. */
        /* coverity[uninit_use_in_call : FALSE] */
        rv = _bcm_esw_stat_counter_raw_set(unit,
                                           f_st->flex_mode,
                                           (flags & _FP_STAT_BYTES) ? 1 : 0,
                                           idx1 - f_st->hw_index,
                                           &stat_val);
        BCM_IF_ERROR_RETURN(rv);

        if (_FP_INVALID_INDEX != idx2) {
            COMPILER_64_ZERO(stat_val.bytes);
            stat_val.packets = 0;
            /* Reset accumulated counter value at secondary index. */
            /* coverity[uninit_use_in_call : FALSE] */
             rv = _bcm_esw_stat_counter_raw_set(unit,
                                               f_st->flex_mode,
                                               (flags & _FP_STAT_BYTES) ? 1 : 0,
                                               idx2 - f_st->hw_index,
                                               &stat_val);
             BCM_IF_ERROR_RETURN(rv);
        }

        if (_FP_INVALID_INDEX != idx3) {
            COMPILER_64_ZERO(stat_val.bytes);
            stat_val.packets = 0;
            /* Reset accumulated counter value at ternary index. */
            /* coverity[uninit_use_in_call : FALSE] */
             rv = _bcm_esw_stat_counter_raw_set(unit,
                                               f_st->flex_mode,
                                               (flags & _FP_STAT_BYTES) ? 1 : 0,
                                               idx3 - f_st->hw_index,
                                               &stat_val);
             BCM_IF_ERROR_RETURN(rv);
        }
    } else {
        return BCM_E_INTERNAL;
     }

     return (rv);
}

/*
 * Function:
 *      _bcm_field_th_multi_pipe_stat_value_set
 *
 * Description:
 *      Set 64 bit counter value for specific statistic type.
 * Parameters:
 *      unit      - (IN) BCM device number.
 *      f_st      - (IN) Statistics entity descriptor.
 *      stat      - (IN) Collected statistics descriptor.
 *      value     - (IN) Collected counters value.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_field_th_multi_pipe_stat_value_set(int unit,
                                 _field_stat_t *f_st,
                                 bcm_field_stat_t stat,
                                 uint64 value)
{
    int                 rv;             /* Operation return status.          */
    int                 idx1;           /* First counter index to read.      */
    int                 idx2;           /* Second counter index to read.     */
    int                 idx3;           /* Third counter index to read.      */
    int                 stat_group_pipe; /* XGS Pipline associated
                                            to the field group.              */
    uint32              flags;          /* _FP_STAT_XXX.                     */
    _field_stage_t      *stage_fc;      /* Stage field control.              */
    _field_control_t    *fc;            /* Field control structure.          */
    uint64              *byte_count;    /* Pointer to hold byte count value  */
    uint64              *packet_count;  /* Pointer to hold pkt counter value */

    /* Input parameters check. */
    if (NULL == f_st) {
        return (BCM_E_PARAM);
    }

    /* Get field control. */
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    /* Stage field control structure. */
    rv = _field_stage_control_get(unit, f_st->stage_id, &stage_fc);
    BCM_IF_ERROR_RETURN(rv);

    /* Check that stat was requested during statistics entity creation. */
    for (idx1 = 0; idx1 < f_st->nstat; idx1++) {
        if (stat == f_st->stat_arr[idx1]) {
            break;
        }
    }
    if (idx1 == f_st->nstat) {
        return (BCM_E_PARAM);
    }

    /* Get hw indexes and flags needed to compose requested statistic.*/
    rv = fc->functions.fp_stat_index_get(unit, f_st, stat, &idx1,
                                         &idx2, &idx3, &flags);
    BCM_IF_ERROR_RETURN(rv);

    stat_group_pipe = _BCM_FIELD_GLOBAL_GROUP;

    /* Set accumulated counter value at primary index. */
    if (_FP_INVALID_INDEX == idx1) {
        return (BCM_E_INTERNAL);
    }

    if (flags & _FP_STAT_BYTES) {
        packet_count = NULL;
        byte_count = &value;
    } else {
        byte_count = NULL;
        packet_count = &value;
    }

    rv = _field_th_multi_pipe_sw_counter_set(unit, stage_fc,
                                             stat_group_pipe,
                                             idx1, packet_count,
                                             byte_count);
    BCM_IF_ERROR_RETURN(rv);

    COMPILER_64_ZERO(value);

    /* Reset accumulated counter value at secondary index. */
    if (_FP_INVALID_INDEX != idx2) {
        rv = _field_th_multi_pipe_sw_counter_set(unit, stage_fc,
                                                 stat_group_pipe,
                                                 idx2, packet_count,
                                                 byte_count);
        BCM_IF_ERROR_RETURN(rv);
    }

    if (_FP_INVALID_INDEX != idx3) {
        rv = _field_th_multi_pipe_sw_counter_set(unit, stage_fc,
                                                 stat_group_pipe,
                                                 idx3, packet_count,
                                                 byte_count);
        BCM_IF_ERROR_RETURN(rv);
    }

    return (rv);
}

/*
 * Function:
 *      _bcm_th_field_stat_value_get
 *
 * Description:
 *      Get 64 bit counter value for specific statistic type.
 * Parameters:
 *      unit      - (IN) BCM device number.
 *      f_st      - (IN) Statistics entity descriptor.
 *      stat      - (IN) Collected statistics descriptor.
 *      value     - (OUT) Collected counters value.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_th_field_stat_value_get(int unit, int sync_mode,
                             _field_stat_t *f_st,
                             bcm_field_stat_t stat,
                             uint64 *value)
{

    int                 idx1;     /* First counter index to read.      */
    int                 idx2;     /* Second counter index to read.     */
    int                 idx3;     /* Third counter index to read.      */
    int                 rv;       /* Operation return status.          */
    uint32              flags;    /* _FP_STAT_XXX.                     */
    uint64              count_1;  /* Primary index packet count.       */
    uint64              count_2;  /* Primary index byte count.         */
    uint64              count_3;  /* Third counterindex byte count.    */
    _field_control_t    *fc;      /* Field control structure.          */
    soc_ctr_control_info_t ctrl_info;
    soc_reg_t reg;
    bcm_stat_value_t stat_val;

    if (NULL == f_st || NULL == value) {
        return BCM_E_PARAM;
    }

    /* Initialization. */
    COMPILER_64_ZERO(count_1);
    COMPILER_64_ZERO(count_2);
    COMPILER_64_ZERO(count_3);
    idx1 = idx2 = idx3 = _FP_INVALID_INDEX;
    flags = 0;

    /* Check that stat was requested during statistics entity creation. */
    for (idx1 = 0; idx1 < f_st->nstat; idx1++) {
        if (stat == f_st->stat_arr[idx1]) {
            break;
        }
    }
    if (idx1 == f_st->nstat) {
        return (BCM_E_PARAM);
    }

    /* Get field control structure. */
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    /* Get hw indexes and flags needed to compose requested statistic.*/
    rv = fc->functions.fp_stat_index_get(unit, f_st, stat, &idx1,
                                         &idx2, &idx3, &flags);
    BCM_IF_ERROR_RETURN(rv);

    if (_BCM_FIELD_STAGE_EGRESS == f_st->stage_id) {
        ctrl_info.instance = -1;
        ctrl_info.instance_type = SOC_CTR_INSTANCE_TYPE_PIPE;

        if (flags & _FP_STAT_BYTES) {
            reg = SOC_COUNTER_NON_DMA_EFP_BYTE;
        } else {
            reg = SOC_COUNTER_NON_DMA_EFP_PKT;
        }

        /* Get accumulated counter value at primary index. */
        if (_FP_INVALID_INDEX != idx1) {
            rv = soc_counter_generic_get (unit, reg,
                                          ctrl_info, 0,
                                          idx1, &count_1);
            BCM_IF_ERROR_RETURN(rv);
        }

        /* Get accumulated counter value at secondary index. */
        if (_FP_INVALID_INDEX != idx2) {
            rv = soc_counter_generic_get (unit, reg,
                                          ctrl_info, 0,
                                          idx2, &count_2);
            BCM_IF_ERROR_RETURN(rv);
        }

        /* Get accumulated counter value at secondary index. */
        if (_FP_INVALID_INDEX != idx3) {
            rv = soc_counter_generic_get (unit, reg,
                                          ctrl_info, 0,
                                          idx3, &count_3);
            BCM_IF_ERROR_RETURN(rv);
        }

    } else if ((_BCM_FIELD_STAGE_INGRESS == f_st->stage_id) &&
               (f_st->hw_flags & _FP_STAT_FLEX_CNTR) &&
               (soc_feature(unit, soc_feature_advanced_flex_counter))) {

        /* Get accumulated counter value at primary index. */
        if (_FP_INVALID_INDEX != idx1) {
            rv = _bcm_esw_stat_counter_raw_get(unit, sync_mode, f_st->flex_mode,
                                               (flags & _FP_STAT_BYTES) ? 1 : 0,
                                                          idx1 - f_st->hw_index,
                                                                     &stat_val);
            /*
             * Flex STAT module is initalized before field module during reset.
             * Attempt to read flex stats by field module would result in
             * error. Ignore these errors.
             */
            if (BCM_FAILURE(rv)) {
                if (TRUE == fc->init) {
                    return (rv);
                } else {
                    return (BCM_E_NOT_FOUND);
                }
            } else {
                /* Store retrieved stat value in return variable. */
                if (flags & _FP_STAT_BYTES) {
                    COMPILER_64_OR(count_1, stat_val.bytes);
                } else {
                    COMPILER_64_ADD_32(count_1, stat_val.packets);
                }
            }
        }

        /* Get accumulated counter value at secondary index. */
        if (_FP_INVALID_INDEX != idx2) {

            rv = _bcm_esw_stat_counter_raw_get(unit, sync_mode, f_st->flex_mode,
                                               (flags & _FP_STAT_BYTES) ? 1 : 0,
                                                          idx2 - f_st->hw_index,
                                                                     &stat_val);
            /*
             * Flex STAT module is initalized before field module during reset.
             * Attempt to read flex stats by field module would result in
             * error. Ignore these errors.
             */
            if (BCM_FAILURE(rv)) {
                if (TRUE == fc->init) {
                    return (rv);
                } else {
                    return (BCM_E_NOT_FOUND);
                }
            } else {
                /* Store retrieved stat value in return variable. */
                if (flags & _FP_STAT_BYTES) {
                    COMPILER_64_OR(count_2, stat_val.bytes);
                } else {
                    COMPILER_64_ADD_32(count_2, stat_val.packets);
                }
            }

        }

        /* Get accumulated counter value at secondary index. */
        if (_FP_INVALID_INDEX != idx3) {

            rv = _bcm_esw_stat_counter_raw_get(unit, sync_mode, f_st->flex_mode,
                                               (flags & _FP_STAT_BYTES) ? 1 : 0,
                                                          idx3 - f_st->hw_index,
                                                                     &stat_val);
            /*
             * Flex STAT module is initalized before field module during reset.
             * Attempt to read flex stats by field module would result in
             * error. Ignore these errors.
             */
            if (BCM_FAILURE(rv)) {
                if (TRUE == fc->init) {
                    return (rv);
                } else {
                    return (BCM_E_NOT_FOUND);
                }
            } else {
                /* Store retrieved stat value in return variable. */
                if (flags & _FP_STAT_BYTES) {
                    COMPILER_64_OR(count_3, stat_val.bytes);
                } else {
                    COMPILER_64_ADD_32(count_3, stat_val.packets);
                }
            }
        }

    } else {
        return BCM_E_INTERNAL;
    }

    /* Get final counter value. */
    COMPILER_64_ZERO(*value);
    if (flags & _FP_STAT_ADD) {
        COMPILER_64_ADD_64(count_1, count_2);
        COMPILER_64_ADD_64(count_1, count_3);
    } else if (flags & _FP_STAT_SUBSTRACT) {
        COMPILER_64_SUB_64(count_1, count_2);
    }
    COMPILER_64_OR(*value, count_1);
    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_field_th_multi_pipe_stat_value_get
 *
 * Description:
 *      Get 64 bit counter value for specific statistic type.
 * Parameters:
 *      unit      - (IN) BCM device number.
 *      f_st      - (IN) Statistics entity descriptor.
 *      stat      - (IN) Collected statistics descriptor.
 *      value     - (OUT) Collected counters value.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_field_th_multi_pipe_stat_value_get(int unit,
                                 _field_stat_t *f_st,
                                 bcm_field_stat_t stat,
                                 uint64 *value)
{
    int                 idx1;            /* First counter index to read.      */
    int                 idx2;            /* Second counter index to read.     */
    int                 idx3;            /* Third counter index to read.      */
    int                 rv;              /* Operation return status.          */
    int                 stat_group_pipe; /* XGS Pipline associated
                                            to the field group.               */
    uint32              flags;           /* _FP_STAT_XXX.                     */
    uint64              packet_count_1;  /* Primary index packet count.       */
    uint64              byte_count_1;    /* Primary index byte count.         */
    uint64              packet_count_2;  /* Secondary index packet count.     */
    uint64              byte_count_2;    /* Secondary index byte count.       */
    uint64              packet_count_3;  /* Third counter index packet count. */
    uint64              byte_count_3;    /* Third counterindex byte count.    */
    _field_stage_t      *stage_fc;       /* Stage field control.              */
    _field_control_t    *fc;             /* Field control structure.          */

    /* Initialization. */
    COMPILER_64_ZERO(packet_count_1);
    COMPILER_64_ZERO(packet_count_2);
    COMPILER_64_ZERO(packet_count_3);
    COMPILER_64_ZERO(byte_count_1);
    COMPILER_64_ZERO(byte_count_2);
    COMPILER_64_ZERO(byte_count_3);
    idx1 = idx2 = idx3 = _FP_INVALID_INDEX;
    flags = 0;

    /* Get field control. */
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    /* Stage field control structure. */
    rv = _field_stage_control_get(unit, f_st->stage_id, &stage_fc);
    BCM_IF_ERROR_RETURN(rv);

    /* Check that stat was requested during statistics entity creation. */
    for (idx1 = 0; idx1 < f_st->nstat; idx1++) {
        if (stat == f_st->stat_arr[idx1]) {
            break;
        }
    }
    if (idx1 == f_st->nstat) {
        return (BCM_E_PARAM);
    }

    /* Get hw indexes and flags needed to compose requested statistic.*/
    rv = fc->functions.fp_stat_index_get(unit, f_st, stat, &idx1,
                                         &idx2, &idx3, &flags);
    BCM_IF_ERROR_RETURN(rv);

    stat_group_pipe = _BCM_FIELD_GLOBAL_GROUP;

    /* Get accumulated counter value at primary index. */
    if (_FP_INVALID_INDEX != idx1) {
        rv = _field_th_multi_pipe_sw_counter_get(unit, stage_fc,
                                              stat_group_pipe, idx1,
                                              &packet_count_1,
                                              &byte_count_1);
        BCM_IF_ERROR_RETURN(rv);
    }

    /* Get accumulated counter value at secondary index. */
    if (_FP_INVALID_INDEX != idx2) {
        rv = _field_th_multi_pipe_sw_counter_get(unit, stage_fc,
                                              stat_group_pipe, idx2,
                                              &packet_count_2,
                                              &byte_count_2);
        BCM_IF_ERROR_RETURN(rv);
    }

    /* Get accumulated counter value at secondary index. */
    if (_FP_INVALID_INDEX != idx3) {
        rv = _field_th_multi_pipe_sw_counter_get(unit, stage_fc,
                                              stat_group_pipe, idx3,
                                              &packet_count_3,
                                              &byte_count_3);
        BCM_IF_ERROR_RETURN(rv);
    }

    /* Get final counter value. */
    COMPILER_64_ZERO(*value);
    if (flags & _FP_STAT_BYTES) {
        if (flags & _FP_STAT_ADD) {
            COMPILER_64_ADD_64(byte_count_1, byte_count_2);
            COMPILER_64_ADD_64(byte_count_1, byte_count_3);
        } else if (flags & _FP_STAT_SUBSTRACT) {
            COMPILER_64_SUB_64(byte_count_1, byte_count_2);
        }
        COMPILER_64_OR(*value, byte_count_1);
    } else {
        if (flags & _FP_STAT_ADD) {
            COMPILER_64_ADD_64(packet_count_1, packet_count_2);
            COMPILER_64_ADD_64(packet_count_1, packet_count_3);
        } else if (flags & _FP_STAT_SUBSTRACT) {
            COMPILER_64_SUB_64(packet_count_1, packet_count_2);
        }
        COMPILER_64_OR(*value, packet_count_1);
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_th_counter_multi_pipe_memacc_alloc
 * Purpose:
 *     Initialize software cached memory access info for field counters
 *
 * Parameters:
 *     unit         - (IN) BCM device number.
 *     mem          - (IN) Counter memory.
 *     descr        - (IN) Counter descriptor.
 *     ptr          - (OUT) Allocated pointer.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_counter_multi_pipe_memacc_alloc(int unit, soc_memacc_t **memacc_ptr)
{
    int mem_size; /* Total memory size of memory access info of
                     all different counter in all pipes */

    /* Input parameters check. */
    if (NULL == memacc_ptr) {
        return (BCM_E_PARAM);
    }

    mem_size = _FIELD_COUNTER_MEMACC_PIPE_NUM * sizeof(soc_memacc_t);
    *memacc_ptr = sal_alloc(mem_size, "FP counter memory access cache");
    if (NULL == *memacc_ptr) {
        return (BCM_E_MEMORY);
    }
    sal_memset(*memacc_ptr, 0, mem_size);
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_field_th_counter_multi_pipe_collect_init
 * Purpose:
 *     Initialize software field 64bit counters.
 *
 * Parameters:
 *     unit         - (IN) BCM device number.
 *     fc           - (IN) Field control structure.
 *     stage_fc     - (IN/OUT) Stage field control structure.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_th_counter_multi_pipe_collect_init(int unit,
                                       _field_control_t *fc,
                                       _field_stage_t *stage_fc)
{
    int              rv = BCM_E_NONE; /* Operation return status.            */
    int              pipe;            /* XGS pipeline to operate on.         */
    int              max_pipes;       /* Maximum XGS pipelines in the stage. */
    soc_memacc_t     *memacc_list;    /* Memory access cache list.           */
    soc_mem_t        counter_mem[_FP_MAX_NUM_PIPES]; /* FieldProcessor
                                                           (IFP/EFP/VFP)
                                                           counter memories  */
    /* Input parameters check. */
    if (NULL == stage_fc) {
        return (BCM_E_PARAM);
    }

    rv = _bcm_field_th_multi_pipe_counter_mem_get(unit, stage_fc,
                                                  counter_mem);
    BCM_IF_ERROR_RETURN(rv);


    max_pipes = stage_fc->num_pipes;

    /* Check if stage has counters. */
    if (INVALIDm == counter_mem[0]) {
        return (BCM_E_NONE);
    }

    /* Allocate space for cached counter memory access info */
    rv = _field_th_counter_multi_pipe_memacc_alloc(unit,
                                   &(stage_fc->_field_memacc_counters));
    if (BCM_FAILURE(rv)) {
        return (rv);
    }
    memacc_list = stage_fc->_field_memacc_counters;

    for (pipe = 0; pipe < max_pipes; pipe++) {

        rv = _bcm_field_counter32_collect_alloc(unit, counter_mem[pipe],
                                               "FP pipeline counters",
                                           &stage_fc->_field_32_counters[pipe]);
        if (BCM_FAILURE(rv)) {
            _bcm_field_counter_collect_deinit(unit, stage_fc);
            return (rv);
        }

        rv = _bcm_field_counter64_collect_alloc(unit, counter_mem[pipe],
                                            "FP pipeline byte counters",
                                           &stage_fc->_field_64_counters[pipe]);
        if (BCM_FAILURE(rv)) {
            _bcm_field_counter_collect_deinit(unit, stage_fc);
            return (rv);
        }

        rv = soc_memacc_init(unit, counter_mem[pipe], BYTE_COUNTERf,
                                 &(memacc_list[(pipe * 2)]));
        if(BCM_FAILURE(rv)) {
            _bcm_field_counter_collect_deinit(unit, stage_fc);
            return (rv);
        }

        rv = soc_memacc_init(unit, counter_mem[pipe], PACKET_COUNTERf,
                            &(memacc_list[((pipe * 2) + 1)]));

        if(BCM_FAILURE(rv)) {
            _bcm_field_counter_collect_deinit(unit, stage_fc);
            return (rv);
        }
    }
    return (rv);
}

/*
 * Function:
 *  _bcm_field_th_multi_pipe_sw_counter_update
 * Description:
 *  Service routine used to update 64-bit software accumulated counters.
 *  based on hw memory block.
 * Parameters:
 *   unit     - (IN) BCM device number.
 *   stage_fc - (IN/OUT) Stage field control structure.
 *   mem      - (IN) Counters memory.
 *   idx_min  - (IN) First counter index in the buffer.
 *   idx_max  - (IN) Last counter index in the buffer.
 *   buf      - (IN) Counters buffer.
 *   flags    - (IN) Device counters flags.
 * Returns:
 *  BCM_E_XXX
 */
int
_bcm_field_th_multi_pipe_sw_counter_update(int unit, _field_stage_t *stage_fc,
                                        soc_mem_t mem, int idx_min, int idx_max,
                                        char *buf, int pipe)
{
    int                    rv = BCM_E_NONE; /* Operartion return status.      */
    int                    idx;             /* Iteration index.               */
    int                    buf_offset;      /* Offset into counters buffer.   */
    uint32                 *entry_p;        /* HW entry pointer.              */
    _field_control_t       *fc;             /* Field control structure.       */

    if ((NULL == stage_fc) || (NULL == buf)) {
        return BCM_E_PARAM;
    }

    /* Get field control structure. */
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));
    for (idx = idx_min, buf_offset = 0; idx <= idx_max; idx++, buf_offset++) {
        /* Read counter value from the first memory. */
        entry_p = soc_mem_table_idx_to_pointer(unit, mem, uint32 *,
                                               buf, buf_offset);

        /* Process accumulated counter value. */
        rv = _bcm_field_th_pipe_counter_get(unit, stage_fc, mem, entry_p,
                                            idx, pipe, NULL, NULL);
        if (BCM_FAILURE(rv)) {
            break;
        }
    }
    return rv;
}

/*
 * Function:
 *    _field_th_multi_pipe_mem_counters_read
 * Description:
 *    Service routine used to coolect 64-bit software accumulated counters.
 *    for a specific stage.
 * Parameters:
 *    unit     - (IN) BCM device number.
 *    mem      - (IN) Counters memory.
 *    pipe     - (IN) XGS pipeline to operate on.
 *    stage_fc - (IN/OUT) Stage field control structure.
 * Returns:
 *  BCM_E_XXX
 */
STATIC int
_field_th_multi_pipe_mem_counters_read(int unit,
                                    soc_mem_t mem,
                                    int pipe,
                                    _field_stage_t *stage_fc)
{
    int       rv = BCM_E_NONE;       /* Operation return value.   */
    int       idx;                   /* Table iteration index.    */
    int       buf_end_idx;           /* Buffer end index.         */
    int       alloc_size;            /* DMA buffer size.          */
    int       max_idx;               /* Counter memory index max. */
    int       min_idx;               /* Counter memory index min. */
    char      *buf;                  /* Buffer to read the table. */


    /* Input parameters check. */
    if (NULL == stage_fc) {
        return (BCM_E_PARAM);
    }

    /* Make sure memory is valid */
    if (INVALIDm == mem) {
        return BCM_E_INTERNAL;
    }

    /* Get table boundaries. */
    min_idx = soc_mem_index_min(unit, mem);
    max_idx = soc_mem_index_max(unit, mem);

    /* Allocate memory buffer. */
    alloc_size = (_FP_64_COUNTER_BUCKET * sizeof(fp_counter_table_entry_t));
    buf = soc_cm_salloc(unit, alloc_size, "fp_64_bit_counter");
    if (buf == NULL) {
        return (BCM_E_MEMORY);
    }

    /* Dma part of the table & update software counters. */
    soc_mem_lock(unit, mem);
    for (idx = min_idx; idx <= max_idx; idx += _FP_64_COUNTER_BUCKET) {
        buf_end_idx = MIN(max_idx, (idx + _FP_64_COUNTER_BUCKET - 1));
        /* Dma range of entries in counter table. */
        rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY, idx,
                                    buf_end_idx, buf);
        if (BCM_FAILURE(rv)) {
            break;
        }

        rv = _bcm_field_th_multi_pipe_sw_counter_update(unit, stage_fc,
                                                     mem, idx,
                                                     buf_end_idx,
                                                     buf, pipe);
        if (BCM_FAILURE(rv)) {
            break;
        }
    }
    soc_mem_unlock(unit, mem);
    soc_cm_sfree(unit, buf);
    return (rv);
}

/*
 * Function:
 *  _bcm_field_th_stage_multi_pipe_counters_collect
 * Description:
 *  Service routine used to coolect  64-bit software accumulated counters.
 *  for a specific stage.
 * Parameters:
 *   unit     - (IN) BCM device number.
 *   fc       - (IN) Field control structure.
 *   stage_fc - (IN/OUT) Stage field control structure.
 * Returns:
 *  BCM_E_XXX
 * Notes:
 *  None.
 */
int
_bcm_field_th_stage_multi_pipe_counters_collect (int unit, _field_control_t *fc,
                               _field_stage_t *stage_fc)
{
    int              rv = BCM_E_NONE; /* Operation return value.            */
    int              pipe;            /* XGS pipeline to operate on.        */
    int              max_pipes;       /* Maximum XGS pipelines in the stage */
    soc_mem_t        counter_mem[_FP_MAX_NUM_PIPES]; /* FieldProcessor
                                                           (IFP/EFP/VFP)
                                                           counter memories */
    /* Input parameters check. */
    if (NULL == stage_fc || NULL == fc) {
        return (BCM_E_PARAM);
    }

    max_pipes = stage_fc->num_pipes;
    for (pipe = 0; pipe < max_pipes; pipe++ ) {
        if (NULL == stage_fc->_field_32_counters[pipe] ||
            NULL == stage_fc->_field_64_counters[pipe])
            return BCM_E_UNAVAIL;
    }

    /* Get counters memory for the stage. */
    rv = _bcm_field_th_multi_pipe_counter_mem_get(unit, stage_fc,
                                                     counter_mem);
    BCM_IF_ERROR_RETURN(rv);

    for (pipe = 0; pipe < max_pipes; pipe++ ) {
        if (INVALIDm != counter_mem[pipe]) {
        BCM_IF_ERROR_RETURN
            (_field_th_multi_pipe_mem_counters_read(unit, counter_mem[pipe],
                                                 pipe, stage_fc));
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_th_field_counter_config_set
 * Description:
 *     To set the SOC configuration for EFP counters.
 * Parameters:
 *     unit     - (IN) BCM device number.
 * Returns:
 *     BCM_E_XXX
 * Notes:
 *     Field module must perform 2 steps to start the counter module to
 *     start performing the DMA.
 *     1. Enable the DMA for the respective counter (Both PKT/BYTE)
 *     2. Setup a DMA profile.
 */

int
_bcm_th_field_counter_config_set(int unit)
{
    int rv;
    int count;
    soc_ctr_control_info_t ctrl_info;
    soc_ctr_ctrl_config_t  ctrl_config;

    ctrl_info.instance_type = SOC_CTR_INSTANCE_TYPE_PIPE;
    ctrl_info.instance = -1;
    ctrl_config.config_type = SOC_CTR_CTRL_CONFIG_DMA_ENABLE;
    ctrl_config.config_val = 1; /* Enable DMA */
    count = 1;
    rv = soc_counter_config_multi_set(0, SOC_COUNTER_NON_DMA_EFP_PKT,
                                      count, &ctrl_info, &ctrl_config);
    BCM_IF_ERROR_RETURN(rv);

    rv = soc_counter_config_multi_set(0, SOC_COUNTER_NON_DMA_EFP_BYTE,
                                  count, &ctrl_info, &ctrl_config);

    BCM_IF_ERROR_RETURN(rv);

    ctrl_config.config_type = SOC_CTR_CTRL_CONFIG_DMA_RATE_PROFILE_ALL;
    ctrl_config.config_val = 1; /* Enable ALL profiles for EFP_PKT */
    rv = soc_counter_config_multi_set (0, SOC_COUNTER_NON_DMA_EFP_PKT,
                                       count, &ctrl_info, &ctrl_config);

    BCM_IF_ERROR_RETURN(rv);

    rv = soc_counter_config_multi_set(0, SOC_COUNTER_NON_DMA_EFP_BYTE,
                                  count, &ctrl_info, &ctrl_config);
    return rv;
}

              /*  END OF COUNTER ROUTINES */

              /*  START OF POLICER ROUTINES */
/*
 * Function:
 *     _bcm_field_th_policer_mem_get
 * Purpose:
 *     Get policer table name.
 * Parameters:
 *     unit         - (IN)  BCM device number
 *     stage_fc     - (IN)  Field Processor stage control structure.
 *     counter_mem  - (OUT) pointer to array of counter memories.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_th_policer_mem_get(int unit,
                              _field_stage_t *stage_fc,
                              int instance,
                              soc_mem_t *policer_mem)
{
    static const soc_mem_t ifp_policer_mem[] = /* Per-Pipe IFP meter table view. */
        {
            IFP_METER_TABLE_PIPE0m,
            IFP_METER_TABLE_PIPE1m,
            IFP_METER_TABLE_PIPE2m,
            IFP_METER_TABLE_PIPE3m

        };

    static const soc_mem_t efp_policer_mem[] = /* Per-Pipe EFP meter table view. */
        {
            EFP_METER_TABLE_PIPE0m,
            EFP_METER_TABLE_PIPE1m,
            EFP_METER_TABLE_PIPE2m,
            EFP_METER_TABLE_PIPE3m

        };

    if (NULL == policer_mem || NULL == stage_fc) {
        return (BCM_E_PARAM);
    }

    *policer_mem = INVALIDm;

    /* Resolve meter table name. */
    switch (stage_fc->stage_id) {
      case _BCM_FIELD_STAGE_INGRESS:
          switch (stage_fc->oper_mode) {
              case bcmFieldGroupOperModeGlobal:
                  *policer_mem = IFP_METER_TABLEm;
                  break;
              case bcmFieldGroupOperModePipeLocal:
                  *policer_mem = ifp_policer_mem[instance];
                  break;
              default:
                  return (BCM_E_INTERNAL);
          }
          break;
      case _BCM_FIELD_STAGE_EGRESS:
          switch (stage_fc->oper_mode) {
              case bcmFieldGroupOperModeGlobal:
                  *policer_mem = EFP_METER_TABLEm;
                  break;
              case bcmFieldGroupOperModePipeLocal:
                  *policer_mem = efp_policer_mem[instance];
                  break;
              default:
                  return (BCM_E_INTERNAL);
          }
          break;
      default:
          return (BCM_E_INTERNAL);
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_field_th_policer_install
 * Purpose:
 *     Install a meter pair into the hardware tables.
 * Parameters:
 *     unit   - (IN) BCM device number.
 *     f_ent  - (IN) Field entry.
 *     f_pl   - (IN) Field policer descriptor.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_th_policer_install(int unit, _field_entry_t *f_ent,
                               _field_policer_t *f_pl)
{
    int       refresh_bitsize;          /* Number of bits for the
                                           refresh rate field.                */
    int       bucket_max_bitsize;       /* Number of bits for the
                                           bucket max field.                  */
    int       rv;                       /* Operation return status.           */
    int       multi_factor;             /* Multiple Factor. */
    uint32    bucketsize_peak = 0;      /* Bucket size.                       */
    uint32    refresh_rate_peak = 0;    /* Policer refresh rate.              */
    uint32    granularity_peak = 0;     /* Policer granularity.               */
    uint32    bucketsize_commit = 0;    /* Bucket size.                       */
    uint32    refresh_rate_commit = 0;  /* Policer refresh rate.              */
    uint32    granularity_commit = 0;   /* Policer granularity.               */
    uint32    flags;                    /* Policer flags.                     */
    soc_mem_t policer_mem;              /* Meter table name.                  */
    _field_stage_t *stage_fc;           /* Stage Field control structure.     */

    /* Input parameters check. */
    if ((NULL == f_ent) || (NULL == f_pl)) {
        return (BCM_E_PARAM);
    }

    if (NULL == f_ent->group || NULL == f_ent->fs) {
        return (BCM_E_INTERNAL);
    }

    if (0 == (f_pl->hw_flags & _FP_POLICER_DIRTY)) {
        return (BCM_E_NONE);
    }


    rv = _field_stage_control_get (unit, f_ent->fs->stage_id, &stage_fc);
    BCM_IF_ERROR_RETURN(rv);

    /* Resolve meter table name. */
    rv = _bcm_field_th_policer_mem_get(unit, stage_fc, f_ent->group->instance, &policer_mem);
    BCM_IF_ERROR_RETURN(rv);



    refresh_bitsize = soc_mem_field_length(unit, policer_mem, REFRESHCOUNTf);
    bucket_max_bitsize = soc_mem_field_length(unit, policer_mem, BUCKETSIZEf);

    /* lookup bucket size from tables */
    flags = _BCM_XGS_METER_FLAG_GRANULARITY | _BCM_XGS_METER_FLAG_FP_POLICER;

    /* Set packet mode flags setting */
    if (f_pl->cfg.flags & BCM_POLICER_MODE_PACKETS) {
        flags |= _BCM_XGS_METER_FLAG_PACKET_MODE;
    } else {
        flags &= ~_BCM_XGS_METER_FLAG_PACKET_MODE;
    }

    /* In Tomahawk EFP metering, Refresh tick duration has changed from 
     * 7.8125us to 15.625us ,but the RTL does not double the REFRESHCOUNT 
     * while putting the tokens into the bucket which is causing 50% lesser 
     * traffic rate than expected.So the work around is ehe value programmed 
     * from software in EFP meters needs to be doubled.
     */

    multi_factor = 1;
    if (_BCM_FIELD_STAGE_EGRESS == stage_fc->stage_id) {
        multi_factor = 2;
    }

    if (f_pl->cfg.mode  != bcmPolicerModeSrTcm) {
        if (f_pl->hw_flags & _FP_POLICER_COMMITTED_DIRTY) {
            /* Calculate policer bucket size/refresh_rate/granularity. */
            rv = _bcm_xgs_kbits_to_bucket_encoding(f_pl->cfg.ckbits_sec,
                                                   f_pl->cfg.ckbits_burst,
                                                   flags, refresh_bitsize,
                                                   bucket_max_bitsize, 
                                                   &refresh_rate_commit,
                                                   &bucketsize_commit,
                                                   &granularity_commit);

            if ((INVALIDm != policer_mem) && BCM_SUCCESS(rv)) {
                /* Programm policer parameters into hw. */
                rv =  _bcm_field_trx_policer_hw_update(unit, f_ent, f_pl,
                                                  BCM_FIELD_METER_COMMITTED,
                                                  bucketsize_commit,
                                                  (refresh_rate_commit * 
                                                   multi_factor),
                                                  granularity_commit,
                                                  policer_mem);
                BCM_IF_ERROR_RETURN(rv);

            }
            f_pl->hw_flags &= ~_FP_POLICER_COMMITTED_DIRTY;
        }

        if (f_pl->hw_flags & _FP_POLICER_PEAK_DIRTY) {

            if (_FP_POLICER_EXCESS_HW_METER(f_pl)) {
                /*
                 * Rates are always set in committed variables,
                 * for flow meters.
                 */
                f_pl->cfg.pkbits_sec = f_pl->cfg.ckbits_sec;
                f_pl->cfg.pkbits_burst = f_pl->cfg.ckbits_burst;
            }

            /* Calculate policer bucket size/refresh_rate/granularity. */
            rv = _bcm_xgs_kbits_to_bucket_encoding(f_pl->cfg.pkbits_sec,
                                                   f_pl->cfg.pkbits_burst,
                                                   flags,
                                                   refresh_bitsize,
                                                   bucket_max_bitsize,
                                                   &refresh_rate_peak,
                                                   &bucketsize_peak,
                                                   &granularity_peak);

            if ((INVALIDm != policer_mem) && BCM_SUCCESS(rv)) {
                /* Programm policer parameters into hw. */
                rv =  _bcm_field_trx_policer_hw_update(unit, f_ent, f_pl,
                                                       BCM_FIELD_METER_PEAK,
                                                       bucketsize_peak,
                                                       (refresh_rate_peak *
                                                        multi_factor),
                                                       granularity_peak,
                                                       policer_mem);
                BCM_IF_ERROR_RETURN(rv);

            }

            f_pl->hw_flags &= ~_FP_POLICER_PEAK_DIRTY;

            if (_FP_POLICER_EXCESS_HW_METER(f_pl)) {
                /* Reset peak meter rates. */
                f_pl->cfg.pkbits_sec = 0;
                f_pl->cfg.pkbits_burst = 0;
            }
        }
    } else {
        if (f_pl->hw_flags & _FP_POLICER_COMMITTED_DIRTY) {
            /* Calculate policer bucket size/refresh_rate/granularity. */
            rv = _bcm_xgs_kbits_to_bucket_encoding(f_pl->cfg.ckbits_sec,
                                                   f_pl->cfg.ckbits_burst,
                                                   flags, refresh_bitsize,
                                                   bucket_max_bitsize,
                                                   &refresh_rate_commit,
                                                   &bucketsize_commit,
                                                   &granularity_commit);
            BCM_IF_ERROR_RETURN(rv);
        }

        if (f_pl->hw_flags & _FP_POLICER_PEAK_DIRTY) {
            /* Calculate policer bucket size/refresh_rate/granularity. */
            rv = _bcm_xgs_kbits_to_bucket_encoding(f_pl->cfg.pkbits_sec,
                                                   f_pl->cfg.pkbits_burst,
                                                   flags,
                                                   refresh_bitsize,
                                                   bucket_max_bitsize,
                                                   &refresh_rate_peak,
                                                   &bucketsize_peak,
                                                   &granularity_peak);
            BCM_IF_ERROR_RETURN(rv);
        }

        if (granularity_commit != granularity_peak) {
            if (granularity_commit < granularity_peak) {
                rv = _bcm_xgs_kbits_to_dual_bucket_encoding(
                                                         f_pl->cfg.ckbits_sec,
                                                         f_pl->cfg.ckbits_burst,
                                                         flags,
                                                         refresh_bitsize,
                                                         bucket_max_bitsize,
                                                         granularity_peak,
                                                         &refresh_rate_commit,
                                                         &bucketsize_commit,
                                                         &granularity_commit);
            } else if (granularity_commit > granularity_peak) {
                rv = _bcm_xgs_kbits_to_dual_bucket_encoding(
                                                         f_pl->cfg.pkbits_sec,
                                                         f_pl->cfg.pkbits_burst,
                                                         flags,
                                                         refresh_bitsize,
                                                         bucket_max_bitsize,
                                                         granularity_commit,
                                                         &refresh_rate_peak,
                                                         &bucketsize_peak,
                                                         &granularity_peak);
            }
            BCM_IF_ERROR_RETURN(rv);
        }


        if ((INVALIDm != policer_mem) && BCM_SUCCESS(rv)) {
            /* Programm policer parameters into hw. */
            rv =  _bcm_field_trx_policer_hw_update(unit, f_ent, f_pl,
                                                  BCM_FIELD_METER_COMMITTED,
                                                  bucketsize_commit,
                                                  (refresh_rate_commit *
                                                   multi_factor),
                                                  granularity_commit,
                                                  policer_mem);
            BCM_IF_ERROR_RETURN(rv);
        }

        f_pl->hw_flags &= ~_FP_POLICER_COMMITTED_DIRTY;

        if ((INVALIDm != policer_mem) && BCM_SUCCESS(rv)) {
            /* Programm policer parameters into hw. */
            rv =  _bcm_field_trx_policer_hw_update(unit, f_ent, f_pl,
                                                 BCM_FIELD_METER_PEAK,
                                                 bucketsize_peak,
                                                 (refresh_rate_peak * 
                                                  multi_factor),
                                                 granularity_peak,
                                                 policer_mem);
            BCM_IF_ERROR_RETURN(rv);
        }

        f_pl->hw_flags &= ~_FP_POLICER_PEAK_DIRTY;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_field_th_policer_meter_hw_free
 *
 * Purpose:
 *     Clear the meter entries from the h/w associated with the policer
 *
 * Parameters:
 *     unit      - (IN) BCM device number.
 *     level     - (IN) Policer level.
 *     f_ent     - (IN) Entry policer belongs to.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_th_policer_meter_hw_free(int unit, _field_entry_t *f_ent,
                                 _field_entry_policer_t *f_ent_pl)
{
    int                rv = BCM_E_NONE; /* Return value                  */
    int                idx;             /* Meter hw index                */
    soc_mem_t          policer_mem;     /* Meter table identifier. */
    _field_meter_pool_t *mp; /* Meter pool pointer. */
    _field_slice_t *fs; /* Field slice pointer. */
    _field_stage_t    *stage_fc;        /* Stage field control structure */
    _field_policer_t  *f_pl;            /* Policer descriptor            */

    /* Get field stage control structure. */
    BCM_IF_ERROR_RETURN
        (_field_stage_control_get(unit, f_ent->group->stage_id,
                                  &stage_fc));

    /* Resolve meter table name. */
    rv = _bcm_field_th_policer_mem_get(unit, stage_fc, f_ent->group->instance, &policer_mem);
    BCM_IF_ERROR_RETURN(rv);

    /* Read policer configuration.*/
    BCM_IF_ERROR_RETURN
        (_bcm_field_policer_get(unit, f_ent_pl->pid, &f_pl));

    /* Must be a valid meter index. */
    if (f_pl->hw_index == _FP_INVALID_INDEX) {
        return (BCM_E_INTERNAL);
    }

    if (stage_fc->flags & _FP_STAGE_GLOBAL_METER_POOLS) {
        mp = stage_fc->meter_pool[f_ent->group->instance][f_pl->pool_index];
        /* Get slice policer installed in. */
        idx =  (f_pl->pool_index * mp->pool_size) + (2 * f_pl->hw_index);

    } else {
        fs = stage_fc->slices[f_ent->group->instance] + f_pl->pool_index;
        idx = fs->start_tcam_idx +  (2 * f_pl->hw_index);
    }

    if (idx < soc_mem_index_min(unit, policer_mem) ||
        idx > soc_mem_index_max(unit, policer_mem)) {
        return (BCM_E_INTERNAL);
    }

    if (!_FP_POLICER_IS_FLOW_MODE(f_pl)) {
        rv = soc_mem_write(unit, policer_mem, MEM_BLOCK_ALL,
                idx, soc_mem_entry_null(unit, policer_mem));
        BCM_IF_ERROR_RETURN(rv);

        idx++;
        rv = soc_mem_write(unit, policer_mem, MEM_BLOCK_ALL,
                idx, soc_mem_entry_null(unit, policer_mem));
    } else {
        if (_FP_POLICER_COMMITTED_HW_METER(f_pl)) {
            idx ++;
        }

        rv = soc_mem_write(unit, policer_mem, MEM_BLOCK_ALL,
                idx, soc_mem_entry_null(unit, policer_mem));
    }

    return (BCM_E_NONE);
}
         /* END OF POLICER ROUTINES */


/*
 * Function:
 *    _field_th_hw_clear
 *
 * Purpose:
 *    Clear hardware memories for the requested stage.
 *
 * Parameters:
 *    unit - (IN) BCM device number.
 *    stage_fc - (IN) Field Processor stage control structure.
 *
 * Returns:
 *    BCM_E_INTERNAL - Invalid CAP stage.
 *    BCM_E_NONE - Success.
 */
STATIC int
_field_th_hw_clear(int unit, _field_stage_t *stage_fc)
{
    /* Input parameters check. */
    if (NULL == stage_fc) {
        return (BCM_E_NONE);
    }

    /*
     * Note: DO NOT CLEAR HW tables when in warm boot mode.
     * If HW tables are cleared, there is no configuration available in hardware
     * to perform Level2 or Level1 WB recovery
     */
    if (SOC_WARM_BOOT(unit)) {
        return (BCM_E_NONE);
    }

    switch (stage_fc->stage_id) {
        case _BCM_FIELD_STAGE_INGRESS:
            BCM_IF_ERROR_RETURN(soc_mem_clear(unit, FP_UDF_TCAMm, COPYNO_ALL,
                TRUE));
            BCM_IF_ERROR_RETURN(soc_mem_clear(unit, FP_UDF_OFFSETm, COPYNO_ALL,
                TRUE));
            BCM_IF_ERROR_RETURN(soc_mem_clear(unit, IFP_RANGE_CHECKm,
                COPYNO_ALL, TRUE));
            BCM_IF_ERROR_RETURN(soc_mem_clear(unit, IFP_TCAM_WIDEm, COPYNO_ALL,
                TRUE));
            BCM_IF_ERROR_RETURN(soc_mem_clear(unit, IFP_POLICY_TABLEm,
                COPYNO_ALL, TRUE));
            BCM_IF_ERROR_RETURN(soc_mem_clear(unit, IFP_METER_TABLEm,
                COPYNO_ALL, TRUE));
            BCM_IF_ERROR_RETURN(soc_mem_clear(unit, IFP_LOGICAL_TABLE_SELECTm,
                COPYNO_ALL, TRUE));
            BCM_IF_ERROR_RETURN(soc_mem_clear(unit,
                IFP_KEY_GEN_PROGRAM_PROFILEm, COPYNO_ALL, TRUE));
            BCM_IF_ERROR_RETURN(soc_mem_clear(unit,
                IFP_KEY_GEN_PROGRAM_PROFILE2m, COPYNO_ALL, TRUE));
            BCM_IF_ERROR_RETURN(soc_mem_clear(unit,
                IFP_LOGICAL_TABLE_ACTION_PRIORITYm, COPYNO_ALL, TRUE));
            break;
        case _BCM_FIELD_STAGE_LOOKUP:
            BCM_IF_ERROR_RETURN(soc_mem_clear(unit, VFP_TCAMm, COPYNO_ALL,
                TRUE));
            BCM_IF_ERROR_RETURN(soc_mem_clear(unit, VFP_POLICY_TABLEm,
                COPYNO_ALL, TRUE));
            /*
             * Group destroy done as part of Field Deinit "bcm_esw_field_detach"
             * clears following registers:
             *     VFP_KEY_CONTROL_1
             *     VFP_KEY_CONTROL_2
             *     VFP_SLICE_MAP
             */
            break;
        case _BCM_FIELD_STAGE_EGRESS:
            BCM_IF_ERROR_RETURN(soc_mem_clear(unit, EFP_TCAMm, COPYNO_ALL,
                TRUE));
            BCM_IF_ERROR_RETURN(soc_mem_clear(unit, EFP_POLICY_TABLEm,
                COPYNO_ALL, TRUE));
            BCM_IF_ERROR_RETURN(soc_mem_clear(unit, EFP_METER_TABLEm,
                COPYNO_ALL, TRUE));
            BCM_IF_ERROR_RETURN(soc_mem_clear(unit, EFP_COUNTER_TABLEm,
                COPYNO_ALL, TRUE));
            break;
        case _BCM_FIELD_STAGE_CLASS:
            BCM_IF_ERROR_RETURN(soc_mem_clear(unit, TCP_FNm, COPYNO_ALL,
                        TRUE));
            BCM_IF_ERROR_RETURN(soc_mem_clear(unit, TOS_FNm, COPYNO_ALL,
                        TRUE));
            BCM_IF_ERROR_RETURN(soc_mem_clear(unit, TCP_FNm, COPYNO_ALL,
                        TRUE));
            BCM_IF_ERROR_RETURN(soc_mem_clear(unit, SRC_COMPRESSIONm, COPYNO_ALL,
                        TRUE));
            BCM_IF_ERROR_RETURN(soc_mem_clear(unit, DST_COMPRESSIONm, COPYNO_ALL,
                        TRUE));
            BCM_IF_ERROR_RETURN(soc_mem_clear(unit, IP_PROTO_MAPm, COPYNO_ALL,
                        TRUE));
            break;
        default:
            /* Should never hit this default condition. */
            return (BCM_E_INTERNAL);

    } /* end switch */

    return (BCM_E_NONE);
}

/*
 * Function:
 *    _field_th_pipes_hw_clear
 *
 * Purpose:
 *    Clear hardware memories for a requested stage on all pipes.
 *
 * Parameters:
 *    unit - (IN) BCM device number
 *    stage_fc - (IN) Field Processor stage control structure.
 *
 * Returns:
 *    BCM_E_INTERNAL - Invalid CAP stage.
 *    BCM_E_NONE - Success.
 */
STATIC int
_field_th_pipes_hw_clear(int unit, _field_stage_t *stage_fc)
{
    int pipe; /* Pipe iterator. */

    soc_mem_t rcheck[_FP_MAX_NUM_PIPES] = /* IFP range checker memory names. */
        {
            IFP_RANGE_CHECK_PIPE0m,
            IFP_RANGE_CHECK_PIPE1m,
            IFP_RANGE_CHECK_PIPE2m,
            IFP_RANGE_CHECK_PIPE3m
        };
    soc_mem_t fp_tcam[_FP_MAX_NUM_PIPES] = /* IFP TCAM memory names. */
        {
            IFP_TCAM_WIDE_PIPE0m,
            IFP_TCAM_WIDE_PIPE1m,
            IFP_TCAM_WIDE_PIPE2m,
            IFP_TCAM_WIDE_PIPE3m
        };
    soc_mem_t fp_policy[_FP_MAX_NUM_PIPES] = /* FP POLICY memory names. */
        {
            IFP_POLICY_TABLE_PIPE0m,
            IFP_POLICY_TABLE_PIPE1m,
            IFP_POLICY_TABLE_PIPE2m,
            IFP_POLICY_TABLE_PIPE3m
        };
    soc_mem_t lt_tcam[_FP_MAX_NUM_PIPES] = /* LT selection TCAM memory names. */
        {
            IFP_LOGICAL_TABLE_SELECT_PIPE0m,
            IFP_LOGICAL_TABLE_SELECT_PIPE1m,
            IFP_LOGICAL_TABLE_SELECT_PIPE2m,
            IFP_LOGICAL_TABLE_SELECT_PIPE3m
        };
    soc_mem_t fp_meter[_FP_MAX_NUM_PIPES] = /* IFP meter memory names. */
        {
            IFP_METER_TABLE_PIPE0m,
            IFP_METER_TABLE_PIPE1m,
            IFP_METER_TABLE_PIPE2m,
            IFP_METER_TABLE_PIPE3m
        };
    soc_mem_t kgen_prof1[_FP_MAX_NUM_PIPES] = /* Keygen program memory names. */
        {
            IFP_KEY_GEN_PROGRAM_PROFILE_PIPE0m,
            IFP_KEY_GEN_PROGRAM_PROFILE_PIPE1m,
            IFP_KEY_GEN_PROGRAM_PROFILE_PIPE2m,
            IFP_KEY_GEN_PROGRAM_PROFILE_PIPE3m
        };
    soc_mem_t kgen_prof2[_FP_MAX_NUM_PIPES] = /* Keygen program memory names. */
        {
            IFP_KEY_GEN_PROGRAM_PROFILE2_PIPE0m,
            IFP_KEY_GEN_PROGRAM_PROFILE2_PIPE1m,
            IFP_KEY_GEN_PROGRAM_PROFILE2_PIPE2m,
            IFP_KEY_GEN_PROGRAM_PROFILE2_PIPE3m
        };
    soc_mem_t action_pri[_FP_MAX_NUM_PIPES] = /* LT priority memory names. */
        {
            IFP_LOGICAL_TABLE_ACTION_PRIORITY_PIPE0m,
            IFP_LOGICAL_TABLE_ACTION_PRIORITY_PIPE1m,
            IFP_LOGICAL_TABLE_ACTION_PRIORITY_PIPE2m,
            IFP_LOGICAL_TABLE_ACTION_PRIORITY_PIPE3m
        };
    soc_mem_t vfp_tcam[_FP_MAX_NUM_PIPES] = /* VFP TCAM memory names. */
        {
            VFP_TCAM_PIPE0m,
            VFP_TCAM_PIPE1m,
            VFP_TCAM_PIPE2m,
            VFP_TCAM_PIPE3m
        };
    soc_mem_t vfp_policy[_FP_MAX_NUM_PIPES] = /* VFP policy memory names. */
        {
            VFP_POLICY_TABLE_PIPE0m,
            VFP_POLICY_TABLE_PIPE1m,
            VFP_POLICY_TABLE_PIPE2m,
            VFP_POLICY_TABLE_PIPE3m
        };
    soc_mem_t efp_tcam[_FP_MAX_NUM_PIPES] = /* EFP TCAM memory names. */
        {
            EFP_TCAM_PIPE0m,
            EFP_TCAM_PIPE1m,
            EFP_TCAM_PIPE2m,
            EFP_TCAM_PIPE3m
        };
    soc_mem_t efp_policy[_FP_MAX_NUM_PIPES] = /* EFP policy memory names. */
        {
            EFP_POLICY_TABLE_PIPE0m,
            EFP_POLICY_TABLE_PIPE1m,
            EFP_POLICY_TABLE_PIPE2m,
            EFP_POLICY_TABLE_PIPE3m
        };
    soc_mem_t efp_meter[_FP_MAX_NUM_PIPES] = /* EFP meter memory names. */
        {
            EFP_METER_TABLE_PIPE0m,
            EFP_METER_TABLE_PIPE1m,
            EFP_METER_TABLE_PIPE2m,
            EFP_METER_TABLE_PIPE3m
        };
    soc_mem_t efp_cntr[_FP_MAX_NUM_PIPES] = /* EFP counter memory names. */
        {
            EFP_COUNTER_TABLE_PIPE0m,
            EFP_COUNTER_TABLE_PIPE1m,
            EFP_COUNTER_TABLE_PIPE2m,
            EFP_COUNTER_TABLE_PIPE3m
        };
    soc_mem_t ttl_fn[_FP_MAX_NUM_PIPES] = /* Class TTL memory names. */
    {
        TTL_FN_PIPE0m,
        TTL_FN_PIPE1m,
        TTL_FN_PIPE2m,
        TTL_FN_PIPE3m
    };
    soc_mem_t tos_fn[_FP_MAX_NUM_PIPES] = /* Class TOS memory names. */
    {
        TOS_FN_PIPE0m,
        TOS_FN_PIPE1m,
        TOS_FN_PIPE2m,
        TOS_FN_PIPE3m
    };
    soc_mem_t tcp_fn[_FP_MAX_NUM_PIPES] = /* Class TCP memory names. */
    {
        TCP_FN_PIPE0m,
        TCP_FN_PIPE1m,
        TCP_FN_PIPE2m,
        TCP_FN_PIPE3m
    };
    soc_mem_t src_compress[_FP_MAX_NUM_PIPES] = /* Class Src Compression memory names. */
    {
        SRC_COMPRESSION_PIPE0m,
        SRC_COMPRESSION_PIPE1m,
        SRC_COMPRESSION_PIPE2m,
        SRC_COMPRESSION_PIPE3m
    };
    soc_mem_t dst_compress[_FP_MAX_NUM_PIPES] = /* Class Dst Compression memory names. */
    {
        DST_COMPRESSION_PIPE0m,
        DST_COMPRESSION_PIPE1m,
        DST_COMPRESSION_PIPE2m,
        DST_COMPRESSION_PIPE3m
    };
    soc_mem_t ip_proto_map[_FP_MAX_NUM_PIPES] = /* IP Protocol Map Memory names. */
    {
        IP_PROTO_MAP_PIPE0m,
        IP_PROTO_MAP_PIPE1m,
        IP_PROTO_MAP_PIPE2m,
        IP_PROTO_MAP_PIPE3m
    };

    /* Input parameters check. */
    if (NULL == stage_fc) {
        return (BCM_E_NONE);
    }

    /*
     * Note: DO NOT CLEAR HW tables when in warm boot mode.
     * If HW tables are cleared, there is no configuration available in hardware
     * to perform Level2 or Level1 WB recovery
     */
    if (SOC_WARM_BOOT(unit)) {
        return (BCM_E_NONE);
    }

    switch (stage_fc->stage_id) {
        case _BCM_FIELD_STAGE_INGRESS:
            for (pipe = 0; pipe < stage_fc->num_pipes; pipe++) {
                BCM_IF_ERROR_RETURN(soc_mem_clear(unit, rcheck[pipe],
                    COPYNO_ALL, TRUE));
                BCM_IF_ERROR_RETURN(soc_mem_clear(unit, fp_tcam[pipe],
                    COPYNO_ALL, TRUE));
                BCM_IF_ERROR_RETURN(soc_mem_clear(unit, fp_policy[pipe],
                    COPYNO_ALL, TRUE));
                BCM_IF_ERROR_RETURN(soc_mem_clear(unit, fp_meter[pipe],
                    COPYNO_ALL, TRUE));
                BCM_IF_ERROR_RETURN(soc_mem_clear(unit, lt_tcam[pipe],
                    COPYNO_ALL, TRUE));
                BCM_IF_ERROR_RETURN(soc_mem_clear(unit, kgen_prof1[pipe],
                    COPYNO_ALL, TRUE));
                BCM_IF_ERROR_RETURN(soc_mem_clear(unit, kgen_prof2[pipe],
                    COPYNO_ALL, TRUE));
                BCM_IF_ERROR_RETURN(soc_mem_clear(unit, action_pri[pipe],
                    COPYNO_ALL, TRUE));
            }
            /*
             * Currently Field module doesn't support per-pipe mode for UDF.
             * Hence, accessing the memory globally.
             */
            BCM_IF_ERROR_RETURN(soc_mem_clear(unit, FP_UDF_TCAMm, COPYNO_ALL,
                TRUE));
            BCM_IF_ERROR_RETURN(soc_mem_clear(unit, FP_UDF_OFFSETm, COPYNO_ALL,
                TRUE));
            break;
        case _BCM_FIELD_STAGE_LOOKUP:
            for (pipe = 0; pipe < stage_fc->num_pipes; pipe++) {
                BCM_IF_ERROR_RETURN(soc_mem_clear(unit, vfp_tcam[pipe],
                    COPYNO_ALL, TRUE));
                BCM_IF_ERROR_RETURN(soc_mem_clear(unit, vfp_policy[pipe],
                    COPYNO_ALL, TRUE));
            }
            break;
        case _BCM_FIELD_STAGE_EGRESS:
            for (pipe = 0; pipe < stage_fc->num_pipes; pipe++) {
                BCM_IF_ERROR_RETURN(soc_mem_clear(unit, efp_tcam[pipe],
                    COPYNO_ALL, TRUE));
                BCM_IF_ERROR_RETURN(soc_mem_clear(unit, efp_policy[pipe],
                    COPYNO_ALL, TRUE));
                BCM_IF_ERROR_RETURN(soc_mem_clear(unit, efp_meter[pipe],
                    COPYNO_ALL, TRUE));
                BCM_IF_ERROR_RETURN(soc_mem_clear(unit, efp_cntr[pipe],
                    COPYNO_ALL, TRUE));
            }
            break;
        case _BCM_FIELD_STAGE_CLASS:
            for (pipe = 0; pipe < stage_fc->num_pipes; pipe++) {
                BCM_IF_ERROR_RETURN(soc_mem_clear(unit, ttl_fn[pipe],
                            COPYNO_ALL, TRUE));
                BCM_IF_ERROR_RETURN(soc_mem_clear(unit, tos_fn[pipe],
                            COPYNO_ALL, TRUE));
                BCM_IF_ERROR_RETURN(soc_mem_clear(unit, tcp_fn[pipe],
                            COPYNO_ALL, TRUE));
                BCM_IF_ERROR_RETURN(soc_mem_clear(unit, src_compress[pipe],
                            COPYNO_ALL, TRUE));
                BCM_IF_ERROR_RETURN(soc_mem_clear(unit, dst_compress[pipe],
                            COPYNO_ALL, TRUE));
                BCM_IF_ERROR_RETURN(soc_mem_clear(unit, ip_proto_map[pipe],
                            COPYNO_ALL, TRUE));
            }
            break;
        default:
            /* Should never hit this default condition. */
            return (BCM_E_INTERNAL);

    } /* end switch */

    return (BCM_E_NONE);
}

/*
 * Function:
 *    _field_th_pipes_meter_refresh_enable
 *
 * Purpose:
 *    Enable meters refresh cycle on all pipes.
 *
 * Parameters:
 *    unit - (IN) BCM device number.
 *    fc - (IN) Field control structure.
 *    state  - (IN) On/Off boolean.
 *
 * Returns:
 *    BCM_E_PARAM - Null field control structure or invalid value for "state".
 *    BCM_E_INTERNAL - Null field stage control structure.
 *    BCM_E_NONE - Success.
 */
STATIC int
_field_th_pipes_meter_refresh_enable(int unit,
                                     _field_control_t *fc,
                                     uint32 state)
{
    int pipe; /* Pipe iterator. */
    _field_stage_t *stage_fc; /* Pointer to Stage control structure. */
    soc_reg_t aux_arb_ctrl[_FP_MAX_NUM_PIPES] = /* FP IP auxilary arbiter
                                                 * control register. */
        {
            AUX_ARB_CONTROL_PIPE0r,
            AUX_ARB_CONTROL_PIPE1r,
            AUX_ARB_CONTROL_PIPE2r,
            AUX_ARB_CONTROL_PIPE3r
        };

      /* Input parameters check. */
    if (NULL == fc || ((TRUE != state) && (FALSE != state))) {
        return (BCM_E_PARAM);
    }

    /* Get the stage control handle. */
    stage_fc = fc->stages;

    /* Ensure stage data structure is initialized. */
    if (NULL == stage_fc) {
        return (BCM_E_INTERNAL);
    }

    /* Enable FP meter refresh in each pipe */
    for (pipe = 0; pipe < stage_fc->num_pipes; pipe++) {
        BCM_IF_ERROR_RETURN(soc_reg_field32_modify(unit, aux_arb_ctrl[pipe],
            REG_PORT_ANY, FP_REFRESH_ENABLEf, state));
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *    _field_th_detach
 *
 * Purpose:
 *    Clears field module hardware tables based on Group operational mode.
 *    Also disables FP meter refresh and FP lookup control on ports.
 *
 * Parameters:
 *    unit - (IN) BCM device number
 *    fc - (IN) Field control structure.
 *
 * Returns:
 *    BCM_E_PARAM - Null field control structure.
 *    BCM_E_INTERNAL - Invalid CAP stage ID.
 *    BCM_E_NONE - Success.
 */
STATIC int
_field_th_detach(int unit, _field_control_t *fc)
{
    _field_stage_t *stage_fc; /* Stage control structure pointer. */

    /* Input parameters check. */
    if (NULL == fc) {
        return (BCM_E_PARAM);
    }

    /* Get the stage control handle. */
    stage_fc = fc->stages;

    /*
     * Initialize FP hardware tables based on the device Group Oper mode
     * settings.
     */
    switch (stage_fc->oper_mode) {
        case bcmFieldGroupOperModeGlobal:
            /* Legacy global mode, use DUPLICATE view to clear FP HW tables. */
            while (stage_fc) {

                /* Clear hardware table */
                BCM_IF_ERROR_RETURN(_field_th_hw_clear(unit, stage_fc));

                /* Goto next stage */
                stage_fc = stage_fc->next;
            }

            /* Disable FP meter refresh on all pipes. */
            BCM_IF_ERROR_RETURN(_field_meter_refresh_enable_set(unit, fc,
                FALSE));
            break;

        case bcmFieldGroupOperModePipeLocal:
            while (stage_fc) {
                /* Clear hardware table */
                BCM_IF_ERROR_RETURN(_field_th_pipes_hw_clear(unit, stage_fc));

                /* Goto next stage */
                stage_fc = stage_fc->next;
            }

            /* Enable FP meter refresh on all pipes. */
            BCM_IF_ERROR_RETURN(_field_th_pipes_meter_refresh_enable(unit, fc,
                FALSE));
            break;
        default:
            break;
    }

    /* Enable filter processor */
    BCM_IF_ERROR_RETURN(_field_port_filter_enable_set(unit, fc, FALSE));

    /* Murthy: Check and disable any counter ejection support here. */

    return (BCM_E_NONE);
}
/*
 * Function:
 *     _bcm_field_th_egress_slice_clear
 *
 * Purpose:
 *     Reset slice configuraton on group deletion event.
 *  
 * Parameters:
 *     unit       - (IN) BCM device number.
 *     slice_numb - (IN) Slice number to set mode for.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_th_egress_slice_clear(int unit, uint8 slice_numb)
{
    int rv;  /* Operation return status. */
    uint32 mode_val[2] =  {0, 0}; /* Mode values for slice control. */
    uint32 class_mode_val[7] = {0, 0, 0, 0, 0, 0, 0}; /* Mode values for 
                                                       * class select register
                                                       */
    /* Input parameters check. */
    if (slice_numb >= COUNTOF(_th_efp_slice_mode)) {
        return (BCM_E_PARAM);
    }

    rv = soc_reg_fields32_modify(unit, EFP_SLICE_CONTROLr, REG_PORT_ANY,
                           2, _th_efp_slice_mode[slice_numb], mode_val);
    BCM_IF_ERROR_RETURN(rv); 

    rv = soc_reg_fields32_modify(unit, EFP_CLASSID_SELECTORr, REG_PORT_ANY,
                               7, classfldtbl[slice_numb], class_mode_val);
    BCM_IF_ERROR_RETURN(rv);

    rv = soc_reg_field32_modify(unit, EFP_KEY4_DVP_SELECTORr, REG_PORT_ANY,
                                                 dvpfldtbl[slice_numb], 0);
    BCM_IF_ERROR_RETURN(rv);

    rv = soc_reg_field32_modify(unit, EFP_KEY4_MDL_SELECTORr, REG_PORT_ANY,
                                                 mdlfldtbl[slice_numb], 0);
    BCM_IF_ERROR_RETURN(rv);

    rv = soc_reg_field32_modify(unit, EFP_KEY8_DVP_SELECTORr, REG_PORT_ANY,
                                                 mdlfldtbl[slice_numb], 0);
    return (rv);
}

/*
 * Function:
 *     _field_th_slice_clear
 *
 * Purpose:
 *     Clear slice configuration on group removal
 *
 * Parameters:
 *     unit  - BCM device number
 *     fg    - Field group slice belongs to
 *     fs    - Field slice structure.
 *
 * Returns:
 *     BCM_E_XXX
 *
 */
STATIC int
_field_th_slice_clear(int unit, _field_group_t *fg, _field_slice_t *fs)
{
    int rv;

    switch (fs->stage_id) {
      case _BCM_FIELD_STAGE_INGRESS:
          rv = _bcm_field_th_ingress_slice_clear(unit, fg, fs);
          break;
      case _BCM_FIELD_STAGE_LOOKUP:
          rv = _bcm_field_td2_lookup_slice_clear(unit, fs->slice_number);
          break;
      case _BCM_FIELD_STAGE_EGRESS:
          rv = _bcm_field_th_egress_slice_clear(unit, fs->slice_number);
          break;
      default:
          rv = BCM_E_INTERNAL;
    }
    return (rv);
}

/*
 * Function:
 *     _bcm_field_th_slice_clear
 *
 * Purpose:
 *     Clear slice configuration on group removal
 *
 * Parameters:
 *     unit  - BCM device number
 *     fg    - Field group slice belongs to
 *     fs    - Field slice structure.
 *
 * Returns:
 *     BCM_E_XXX
 *
 */
int
_bcm_field_th_slice_clear(int unit, _field_group_t *fg, _field_slice_t *fs)
{
    return _field_th_slice_clear(unit, fg, fs);
}
/*
 * Function:
 *    _field_th_functions_init
 *
 * Purpose:
 *    Set up functions pointers
 *
 * Parameters:
 *    functions - (IN/OUT) Pointer to device specific field module utility
 *                         routines.
 *
 * Returns:
 *     Nothing
 */
void
_field_th_functions_init(_field_funct_t *functions)
{
    functions->fp_detach               = _field_th_detach;
    functions->fp_group_install        = _bcm_field_th_group_install;
    functions->fp_selcodes_install     = _bcm_field_th_selcodes_install;
    functions->fp_slice_clear          = _bcm_field_th_slice_clear;
    functions->fp_entry_remove         = _bcm_field_th_entry_remove;
    functions->fp_entry_move           = _bcm_field_th_entry_move;
    functions->fp_entry_enable         = _bcm_field_th_entry_enable_set;
    functions->fp_selcode_get          = _bcm_field_th_selcode_get;
    functions->fp_selcode_to_qset      = _bcm_field_selcode_to_qset;
    functions->fp_qual_list_get        = _bcm_field_qual_lists_get;
    functions->fp_tcam_policy_clear    = NULL;
    functions->fp_tcam_policy_install  = _bcm_field_th_entry_install;
    functions->fp_tcam_policy_reinstall = _bcm_field_th_entry_reinstall;
    functions->fp_policer_install      = _bcm_field_th_policer_install;
    functions->fp_write_slice_map      = _bcm_field_td2_write_slice_map;
    functions->fp_qualify_ip_type      = _bcm_field_td2_qualify_ip_type;
    functions->fp_qualify_ip_type_get  = _bcm_field_td2_qualify_ip_type_get;
    functions->fp_action_support_check = _bcm_field_th_action_support_check;
    functions->fp_action_conflict_check = _bcm_field_td2_action_conflict_check;
    functions->fp_counter_get          = _bcm_field_td_counter_get;
    functions->fp_counter_set          = _bcm_field_td_counter_set;
    functions->fp_stat_value_get       = _bcm_th_field_stat_value_get;
    functions->fp_stat_value_set       = _bcm_th_field_stat_value_set;
    functions->fp_stat_index_get       = _bcm_field_th_stat_index_get;
    functions->fp_action_params_check  = _bcm_field_th_action_params_check;
    functions->fp_action_depends_check = _bcm_field_trx_action_depends_check;
    functions->fp_egress_key_match_type_set
                                    = _bcm_field_th_egress_key_match_type_set;
    functions->fp_external_entry_install  = NULL;
    functions->fp_external_entry_reinstall  = NULL;
    functions->fp_external_entry_remove   = NULL;
    functions->fp_external_entry_prio_set = NULL;
    functions->fp_data_qualifier_ethertype_add
                          = _bcm_field_trx2_data_qualifier_ethertype_add;
    functions->fp_data_qualifier_ethertype_delete
                          = _bcm_field_trx2_data_qualifier_ethertype_delete;
    functions->fp_data_qualifier_ip_protocol_add
                          = _bcm_field_trx2_data_qualifier_ip_protocol_add;
    functions->fp_data_qualifier_ip_protocol_delete
                          = _bcm_field_trx2_data_qualifier_ip_protocol_delete;
    functions->fp_data_qualifier_packet_format_add
                          = _bcm_field_trx2_data_qualifier_packet_format_add;
    functions->fp_data_qualifier_packet_format_delete
                          = _bcm_field_trx2_data_qualifier_packet_format_delete;
    functions->fp_control_set = _bcm_field_th_control_set;
    functions->fp_control_get = _bcm_field_th_control_get;
    functions->fp_stat_hw_mode_get = _bcm_field_th_stat_hw_mode_get;
    functions->fp_stat_hw_alloc    = _bcm_field_th_stat_hw_alloc;
    functions->fp_stat_hw_free     = _bcm_field_th_stat_hw_free;
    functions->fp_group_add        = _bcm_field_th_group_add;
    functions->fp_qualify_svp      = _bcm_field_th_qualify_svp;
    functions->fp_qualify_dvp      = _bcm_field_th_qualify_dvp;
    functions->fp_qualify_trunk    = _bcm_field_th_qualify_trunk;
    functions->fp_qualify_trunk_get = _bcm_field_th_qualify_trunk_get;
    functions->fp_qualify_inports = _bcm_field_th_qualify_InPorts;
    functions->fp_entry_stat_extended_attach = NULL;
    functions->fp_entry_stat_extended_get = NULL;
    functions->fp_entry_stat_detach = _bcm_field_entry_stat_detach;
    functions->fp_class_size_get = _bcm_field_th_qual_class_size_get;
}

/*
 * Function:
 *     _field_th_ext_config_list_add
 * Purpose:
 *     Add extractor configuration to stage extractor configuration list.
 * Parameters:
 *     unit         - (IN) BCM device number.
 *     stage_fc     - (IN) Stage field control structure pointer.
 *     emode        - (IN) Extractor mode.
 *     level        - (IN) Extractor hierarchy level.
 *     new_ext_cfg  - (IN) New extractor configuration to be added the list.
 * Returns:
 *    BCM_E_PARAM   - Invalid parameter.
 *    BCM_E_MEMORY  - Memory allocation failure.
 *    BCM_E_NONE    - Success.
 */
STATIC int
_field_th_ext_config_list_add(int unit, _field_stage_t *stage_fc,
                              _field_ext_conf_mode_t emode, int level,
                              _field_ext_cfg_t *new_ext_cfg)
{

    _field_ext_info_t *ext_info = NULL;   /* Field extractor info.          */
    _field_ext_cfg_t *ext_cfg = NULL;     /* Field extractor configuration. */
    int alloc_sz;                         /* Memory allocation size.        */

    /* Input parameters check. */
    if ((NULL == stage_fc) || (NULL == new_ext_cfg)
         || ((emode < _FieldExtConfMode160Bits)
              || (emode > _FieldExtConfMode480Bits))) {
        return (BCM_E_PARAM);
    }

    /* Get stage's extractor array poiter for given mode . */
    ext_info = stage_fc->ext_cfg_arr[emode];

    if (NULL == ext_info) {
        /* Allocate extractor descriptor. */
        alloc_sz = sizeof(_field_ext_info_t);
        _FP_XGS3_ALLOC(ext_info, alloc_sz, "FP extractor info");
        if (NULL == ext_info) {
            LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                "FP(unit %d) Error: Allocation failure for extractor info.\n"),
                 unit));
            return (BCM_E_MEMORY);
        }
        ext_info->mode = emode;
    }

    /*
     * Re-allocate configuration array to hold the new extractor
     * configuration.
     */
    alloc_sz = (ext_info->conf_size[level] + 1) * sizeof(_field_ext_cfg_t);
    _FP_XGS3_ALLOC(ext_cfg, alloc_sz, "FP extractor config");
    if (NULL == ext_cfg) {
        LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
            "FP(unit %d) Error: Allocation failure for extractor config.\n"),
             unit));
        if (NULL != ext_info) {
            sal_free(ext_info);
        }
        return (BCM_E_MEMORY);
    }

    /* Copy previous configuration to new extractor array. */
    if (NULL != ext_info->ext_cfg[level]) {
        alloc_sz -= sizeof(_field_ext_cfg_t);
        sal_memcpy(ext_cfg, ext_info->ext_cfg[level], alloc_sz);
        sal_free(ext_info->ext_cfg[level]);
    }

    /* Set configuration array to new array. */
    ext_info->ext_cfg[level] = ext_cfg;

    /* Copy new configuration to configuration array. */
    ext_info->ext_cfg[level][ext_info->conf_size[level]] = *new_ext_cfg;

    /* Increment number of extractors for this mode. */
    ext_info->conf_size[level]++;

    /* Install extractor configuration to extractors configuration array. */
    stage_fc->ext_cfg_arr[emode] = ext_info;

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_th_ext_section_add
 * Purpose:
 *     Add extractor section to stage extractor configuration list.
 * Parameters:
 *     unit         - (IN) BCM device number.
 *     stage_fc     - (IN) Stage field control structure pointer.
 *     emode        - (IN) Extractor mode.
 *     level        - (IN) Extractor hierarchy level.
 *     new_ext_cfg  - (IN) New extractor configuration to be added the list.
 * Returns:
 *     BCM_E_PARAM   - Invalid parameter.
 *     BCM_E_MEMORY  - Memory allocation failure.
 *     BCM_E_NONE    - Success.
 */
STATIC int
_field_th_ext_section_add(int unit, _field_stage_t *stage_fc,
                          _field_ext_conf_mode_t emode, int level,
                          _field_ext_cfg_t *new_ext_cfg)
{
    _field_ext_info_t *ext_info = NULL; /* Field extractor info.          */
    int alloc_sz;                       /* Memory allocation size.        */
    _field_ext_sections_t *ext_sec;     /* Extractor section pointer.     */
    _field_keygen_ext_sel_t in_sec_idx; /* Input section index.           */

    /* Input parameters check. */
    if ((NULL == stage_fc) || (NULL == new_ext_cfg)
         || ((emode < _FieldExtConfMode160Bits)
              || (emode > _FieldExtConfMode480Bits))) {
        return (BCM_E_PARAM);
    }

    /* Get stage's extractor array poiter for given mode . */
    ext_info = stage_fc->ext_cfg_arr[emode];

    if (NULL == ext_info->sections) {

        /* Calculate the allocation size. */
        alloc_sz = _FieldKeygenExtSelCount * sizeof(_field_ext_sections_t *);

        /* Allocate memory for stage extractor configuration modes. */
        _FP_XGS3_ALLOC(stage_fc->ext_cfg_arr[emode]->sections,
            alloc_sz, "Field Ext sections");
        if (NULL == stage_fc->ext_cfg_arr[emode]->sections) {

            LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                "FP(unit %d) Error: Allocation failure for extractor "
                "sections.\n"), unit));

            return (BCM_E_MEMORY);
        }
    }

    /* Get the new input section name. */
    in_sec_idx = new_ext_cfg->in_sec;

    /* Get extractor section pointer. */
    ext_sec = stage_fc->ext_cfg_arr[emode]->sections[in_sec_idx];

    /* Calcuate and update maximum output of a section for given extractors. */
    if (NULL == ext_sec) {
        alloc_sz = sizeof(_field_ext_sections_t);
        _FP_XGS3_ALLOC(ext_sec, alloc_sz, "FP extractor section");
        if (NULL == ext_sec) {
            LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                "FP(unit %d) Error: Allocation failure for extractor "
                "section.\n"), unit));
            return (BCM_E_MEMORY);
        }

        /* Initialize section name. */
        ext_sec->sec = new_ext_cfg->in_sec;

        /* Initialize extractor drain bits. */
        ext_sec->drain_bits = new_ext_cfg->gran;

        /* Increment number of sections. */
        ext_info->num_sec++;
        /* Initialize section pointer to point to new section. */
        stage_fc->ext_cfg_arr[emode]->sections[in_sec_idx] = ext_sec;

    } else {
        LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
            "FP(unit %d) Verb: B4-Extractor Sec=%d Gran=%d drain_bits=%d\n"),
            unit, ext_sec->sec, new_ext_cfg->gran, ext_sec->drain_bits));

        /* Update drain bit capacity. */
        ext_sec->drain_bits += new_ext_cfg->gran;

        LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
            "FP(unit %d) Verb: Aft-Extractor Sec=%d Gran=%d drain_bits=%d\n"),
            unit, ext_sec->sec, new_ext_cfg->gran, ext_sec->drain_bits));
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *    _bcm_field_th_ext_config_insert
 * Purpose:
 *    Add extractor configuration to configuration list.
 * Parameters:
 *     unit         - (IN) BCM device number.
 *     stage_fc     - (IN) Stage field control structure pointer.
 *     emode        - (IN) Extractor mode.
 *     level        - (IN) Extractor hierarchy level.
 *     new_ext_cfg  - (IN) New extractor configuration to be added the list.
 * Returns:
 *     BCM_E_PARAM   - Invalid parameter.
 *     BCM_E_MEMORY  - Memory allocation failure.
 *     BCM_E_NONE    - Success.
 */
int
_bcm_field_th_ext_config_insert(int unit, _field_stage_t *stage_fc,
                                _field_ext_conf_mode_t emode,
                                int level, _field_ext_cfg_t *new_ext_cfg)
{
    /* Input parameters check. */
    if ((NULL == stage_fc) || (NULL == new_ext_cfg)
         || ((emode < _FieldExtConfMode160Bits)
              || (emode > _FieldExtConfMode480Bits))) {
        return (BCM_E_PARAM);
    }

    /* Skip Level 4 extractor as this is the final output section. */
    if (_FieldKeygenExtSelL4 != new_ext_cfg->in_sec
        && (_FieldKeygenExtSelL4A != new_ext_cfg->in_sec)
        && (_FieldKeygenExtSelL4B != new_ext_cfg->in_sec)
        && (_FieldKeygenExtSelL4C != new_ext_cfg->in_sec)) {
        BCM_IF_ERROR_RETURN(_field_th_ext_config_list_add(unit, stage_fc, emode,
            level, new_ext_cfg));
    }

    /* Add extractor section information. */
    BCM_IF_ERROR_RETURN(_field_th_ext_section_add(unit, stage_fc, emode, level,
        new_ext_cfg));

    return (BCM_E_NONE);
}

/*
 * Function:
 *    _field_th_stage_extractors_init
 * Purpose:
 *    Initialize extractor section hierarcy structure for the IFP stage.
 * Parameters:
 *    unit     - (IN) BCM device number.
 *    stage_fc - (IN) Stage field control structure pointer.
 * Returns:
 *    BCM_E_PARAM   - Invalid parameter.
 *    BCM_E_MEMORY  - Memory allocation failure.
 *    BCM_E_NONE    - Success.
 */
STATIC int
_field_th_stage_extractors_init(int unit, _field_stage_t *stage_fc)
{
    _FP_EXT_CONFIG_DECL;             /* Extractors config declaration.      */
    int idx;                         /* Index iterator.                     */
    int level = 0;                   /* Extractor hierarchy level.          */
    int gran = 0;                    /* Extractor granularity.              */
    int ext_num = 0;                 /* Extractor number.                   */
    _field_keygen_ext_sel_t sec = 0; /* Keygen selector encoding.           */
    _field_ext_conf_mode_t emode;    /* Extractor configuration mode.       */
    int num_pass_thru_bits = 104;    /* Extractor passthru bits count.      */
    int conf = 0;                    /* Configuration mode iterator.        */
    _field_ext_cfg_t cfg;            /* Extractor configuration structure.  */
    int part;                        /* Entry part number.                  */

    /* Input parameters check. */
    if (NULL == stage_fc) {
        return (BCM_E_PARAM);
    }

    /* Skip this routine for non-IFP stages. */
    if (_BCM_FIELD_STAGE_INGRESS != stage_fc->stage_id) {
        return (BCM_E_NONE);
    }

    /* Allocate memory for stage extractor configuration modes. */
    _FP_XGS3_ALLOC(stage_fc->ext_cfg_arr,
                   (_FieldExtConfModeCount * sizeof(_field_ext_info_t *)),
                    "Field Extractors");
    if (NULL == stage_fc->ext_cfg_arr) {
        return (BCM_E_MEMORY);
    }

    /* Initialize Level_1 32-bit extractor section. */
    level = 1;
    gran = 32;
    ext_num = 0;
    part = 0;
    emode = _FieldExtConfMode160Bits;
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, part, level, gran, ext_num,
        _FieldKeygenExtSelL1E32, _FieldKeygenExtSelL2E16, 0, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, part, level, gran, (ext_num + 1),
        _FieldKeygenExtSelL1E32, _FieldKeygenExtSelL2E16, 32, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, part, level, gran, (ext_num + 2),
        _FieldKeygenExtSelL1E32, _FieldKeygenExtSelL2E16, 64, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, part, level, gran, (ext_num + 3),
        _FieldKeygenExtSelL1E32, _FieldKeygenExtSelL2E16, 96, 0, 0, 0);

    /* Initialize Level_1 16-bit extractors section. */
    level = 1;
    gran = 16;
    ext_num = 0;
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, part, level, gran, ext_num,
        _FieldKeygenExtSelL1E16, _FieldKeygenExtSelL2E16, 128, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, part, level, gran, (ext_num + 1),
        _FieldKeygenExtSelL1E16, _FieldKeygenExtSelL2E16, 144, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, part, level, gran, (ext_num + 2),
        _FieldKeygenExtSelL1E16, _FieldKeygenExtSelL2E16, 160, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, part, level, gran, (ext_num + 3),
        _FieldKeygenExtSelL1E16, _FieldKeygenExtSelL2E16, 176, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, part, level, gran, (ext_num + 4),
        _FieldKeygenExtSelL1E16, _FieldKeygenExtSelL2E16, 192, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, part, level, gran, (ext_num + 5),
        _FieldKeygenExtSelL1E16, _FieldKeygenExtSelL2E16, 208, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, part, level, gran, (ext_num + 6),
        _FieldKeygenExtSelL1E16, _FieldKeygenExtSelL2E16, 224, 0, 0, 0);

    /* Initialize Level_1 8-bit extractors section. */
    level = 1;
    gran = 8;
    ext_num = 0;
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, part, level, gran, ext_num,
        _FieldKeygenExtSelL1E8, _FieldKeygenExtSelL2E4, 0, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, part, level, gran, (ext_num + 1),
        _FieldKeygenExtSelL1E8, _FieldKeygenExtSelL2E4, 8, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, part, level, gran, (ext_num + 2),
        _FieldKeygenExtSelL1E8, _FieldKeygenExtSelL2E4, 16, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, part, level, gran, (ext_num + 3),
        _FieldKeygenExtSelL1E8, _FieldKeygenExtSelL2E4, 24, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, part, level, gran, (ext_num + 4),
        _FieldKeygenExtSelL1E8, _FieldKeygenExtSelL2E4, 32, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, part, level, gran, (ext_num + 5),
        _FieldKeygenExtSelL1E8, _FieldKeygenExtSelL2E4, 40, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, part, level, gran, (ext_num + 6),
        _FieldKeygenExtSelL1E8, _FieldKeygenExtSelL2E4, 48, 0, 0, 0);

    /* Initialize Level_1 4-bit extractors section. */
    level = 1;
    gran = 4;
    ext_num = 0;
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, part, level, gran, ext_num,
        _FieldKeygenExtSelL1E4, _FieldKeygenExtSelL2E4, 56, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, part, level, gran, (ext_num + 1),
        _FieldKeygenExtSelL1E4, _FieldKeygenExtSelL2E4, 60, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, part, level, gran, (ext_num + 2),
        _FieldKeygenExtSelL1E4, _FieldKeygenExtSelL2E4, 64, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, part, level, gran, (ext_num + 3),
        _FieldKeygenExtSelL1E4, _FieldKeygenExtSelL2E4, 68, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, part, level, gran, (ext_num + 4),
        _FieldKeygenExtSelL1E4, _FieldKeygenExtSelL2E4, 72, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, part, level, gran, (ext_num + 5),
        _FieldKeygenExtSelL1E4, _FieldKeygenExtSelL2E4, 76, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, part, level, gran, (ext_num + 6),
        _FieldKeygenExtSelL1E4, _FieldKeygenExtSelL2E4, 80, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, part, level, gran, (ext_num + 7),
        _FieldKeygenExtSelL1E4, _FieldKeygenExtSelL2E4, 84, 0, 0, 0);

    /* Initialize Level1 2-bit extractors section. */
    level = 1;
    gran = 2;
    ext_num = 0;
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, part, level, gran, ext_num,
        _FieldKeygenExtSelL1E2, _FieldKeygenExtSelL2E4, 88, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, part, level, gran, (ext_num + 1),
        _FieldKeygenExtSelL1E2, _FieldKeygenExtSelL2E4, 90, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, part, level, gran, (ext_num + 2),
        _FieldKeygenExtSelL1E2, _FieldKeygenExtSelL2E4, 92, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, part, level, gran, (ext_num + 3),
        _FieldKeygenExtSelL1E2, _FieldKeygenExtSelL2E4, 94, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, part, level, gran, (ext_num + 4),
        _FieldKeygenExtSelL1E2, _FieldKeygenExtSelL2E4, 96, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, part, level, gran, (ext_num + 5),
        _FieldKeygenExtSelL1E2, _FieldKeygenExtSelL2E4, 98, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, part, level, gran, (ext_num + 6),
        _FieldKeygenExtSelL1E2, _FieldKeygenExtSelL2E4, 100, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, part, level, gran, (ext_num + 7),
        _FieldKeygenExtSelL1E2, _FieldKeygenExtSelL2E4, 102, 0, 0, 0);

    /* Initialize Level_2 16-bit extractors section. */
    level = 2;
    gran = 16;
    ext_num = 0;
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, part, level, gran, ext_num,
        _FieldKeygenExtSelL2E16, _FieldKeygenExtSelL3E16, 0, 0, 0,
        (_FP_EXT_ATTR_NOT_WITH_IPBM | _FP_EXT_ATTR_NOT_IN_EIGHTY_BIT));
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, part, level, gran, (ext_num + 1),
        _FieldKeygenExtSelL2E16, _FieldKeygenExtSelL3E16, 16, 0, 0,
        (_FP_EXT_ATTR_NOT_WITH_IPBM | _FP_EXT_ATTR_NOT_IN_EIGHTY_BIT));
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, part, level, gran, (ext_num + 2),
        _FieldKeygenExtSelL2E16, _FieldKeygenExtSelL3E16, 32, 0, 0,
        (_FP_EXT_ATTR_NOT_IN_EIGHTY_BIT | _FP_EXT_ATTR_NOT_WITH_SRC_DST_CONT_0));
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, part, level, gran, (ext_num + 3),
        _FieldKeygenExtSelL2E16, _FieldKeygenExtSelL3E16, 48, 0, 0,
        (_FP_EXT_ATTR_NOT_IN_EIGHTY_BIT | _FP_EXT_ATTR_NOT_WITH_SRC_DST_CONT_1));

    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, part, level, gran, (ext_num + 4),
        _FieldKeygenExtSelL2E16, _FieldKeygenExtSelL3E4, 0, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, part, level, gran, (ext_num + 5),
        _FieldKeygenExtSelL2E16, _FieldKeygenExtSelL3E4, 16, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, part, level, gran, (ext_num + 6),
        _FieldKeygenExtSelL2E16, _FieldKeygenExtSelL3E4, 32, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, part, level, gran, (ext_num + 7),
        _FieldKeygenExtSelL2E16, _FieldKeygenExtSelL3E4, 48, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, part, level, gran, (ext_num + 8),
        _FieldKeygenExtSelL2E16, _FieldKeygenExtSelL3E4, 64, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, part, level, gran, (ext_num + 9),
        _FieldKeygenExtSelL2E16, _FieldKeygenExtSelL3E4, 80, 0, 0, 0);

    /* Initialize Level_2 1-bit extractors section. */
    level = 2;
    gran = 1;
    ext_num = 0;
    for (idx = 0; idx < num_pass_thru_bits; idx++) {
        _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, part, level, gran,
            (ext_num + idx), _FieldKeygenExtSelL2E4, _FieldKeygenExtSelL3E4,
            96, 0, 0, _FP_EXT_ATTR_PASS_THRU);
    }

    /* Initialize two Level_3 16-bit extractors section. */
    level = 3;
    gran = 16;
    ext_num = 0;
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, part, level, gran, ext_num,
        _FieldKeygenExtSelL3E16, _FieldKeygenExtSelL4, 0, 0, 0,
        (_FP_EXT_ATTR_NOT_WITH_IPBM | _FP_EXT_ATTR_NOT_IN_EIGHTY_BIT));
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, part, level, gran, (ext_num + 1),
        _FieldKeygenExtSelL3E16, _FieldKeygenExtSelL4, 20, 0, 0,
        (_FP_EXT_ATTR_NOT_WITH_IPBM | _FP_EXT_ATTR_NOT_IN_EIGHTY_BIT));

    /* Initialize four Level_3 4-bit extractors section. */
    level = 3;
    gran = 4;
    ext_num = 0;
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, part, level, gran, ext_num,
        _FieldKeygenExtSelL3E4, _FieldKeygenExtSelL4, 16, 0, 0,
        (_FP_EXT_ATTR_NOT_WITH_IPBM | _FP_EXT_ATTR_NOT_IN_EIGHTY_BIT));
    for (idx = 1; idx < 4; idx++) {
        _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, part, level, gran,
            (ext_num + idx), _FieldKeygenExtSelL3E4, _FieldKeygenExtSelL4,
            36 + (gran * (idx - 1)) , 0, 0, _FP_EXT_ATTR_NOT_IN_EIGHTY_BIT);
    }

    /* Initialize Level_3 16-bit extractors section. */
    level = 3;
    gran = 16;
    ext_num = 2;
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, part, level, gran, ext_num,
        _FieldKeygenExtSelL3E16, _FieldKeygenExtSelL4, 48, 0, 0,
        (_FP_EXT_ATTR_NOT_IN_EIGHTY_BIT | _FP_EXT_ATTR_NOT_WITH_SRC_DST_CONT_0));
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, part, level, gran, (ext_num + 1),
        _FieldKeygenExtSelL3E16, _FieldKeygenExtSelL4, 64, 0, 0,
        _FP_EXT_ATTR_NOT_IN_EIGHTY_BIT | _FP_EXT_ATTR_NOT_WITH_SRC_DST_CONT_1);

    /* Initialize seventeen Level_3 4-bit extractors section. */
    level = 3;
    gran = 4;
    ext_num = 4;
    for (idx = 0; idx < 2; idx++) {
        _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, part, level, gran,
            (ext_num + idx), _FieldKeygenExtSelL3E4, _FieldKeygenExtSelL4,
            80 + (idx * gran), 0, 0, 0);
    }
    ext_num = 4;
    for (idx = 2; idx < 6; idx++) {
        _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, part, level, gran,
            (ext_num + idx), _FieldKeygenExtSelL3E4, _FieldKeygenExtSelL4,
            80 + (idx * gran), 0, 0, _FP_EXT_ATTR_NOT_WITH_SRC_DST_CONT_0_80);
    }
    ext_num = 4;
    for (idx = 6; idx < 10; idx++) {
        _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, part, level, gran,
            (ext_num + idx), _FieldKeygenExtSelL3E4, _FieldKeygenExtSelL4,
            80 + (idx * gran), 0, 0, _FP_EXT_ATTR_NOT_WITH_SRC_DST_CONT_1_80);
    }
    ext_num = 4;
    for (idx = 10; idx < 17; idx++) {
        _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, part, level, gran,
            (ext_num + idx), _FieldKeygenExtSelL3E4, _FieldKeygenExtSelL4,
            80 + (idx * gran), 0, 0, 0);
    }

    /* Initialize 5 Level_3 2-bit extractors section. */
    level = 3;
    gran = 2;
    for (ext_num = 0; ext_num < 2; ext_num++) {
        _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, part, level, gran, ext_num,
            _FieldKeygenExtSelL3E4, _FieldKeygenExtSelL4,
            148 + (ext_num * gran), 0, 0, 0);
    }

    /* Overlay with _FP_POST_MUX_SRC_DST_CONT_0/1. */
    ext_num = 0;
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, part, level, gran, ext_num + 2,
        _FieldKeygenExtSelL3E4, _FieldKeygenExtSelL4, 152, 0, 0,
        (_FP_EXT_ATTR_NOT_WITH_SRC_DST_CONT_0 |
         _FP_EXT_ATTR_NOT_WITH_SRC_DST_CONT_1));

    ext_num = 0;
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, part, level, gran, ext_num + 3,
        _FieldKeygenExtSelL3E4, _FieldKeygenExtSelL4, 154, 0, 0, 0);

    /* Overlay with NAT_DST_REALM_ID. */
    ext_num = 0;
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, part, level, gran, ext_num + 4,
        _FieldKeygenExtSelL3E4, _FieldKeygenExtSelL4, 156, 0, 0,
        _FP_EXT_ATTR_NOT_WITH_NAT_DST_REALM_ID);

    /* Initialize 2 Level_3 1-bit extractors section. */
    level = 3;
    gran = 1;
    ext_num = 0;
    /* Overlay with NAT_NEEDED. */
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, part, level, gran, ext_num,
        _FieldKeygenExtSelL3E4, _FieldKeygenExtSelL4, 158, 0, 0,
        _FP_EXT_ATTR_NOT_WITH_NAT_NEEDED);

    /* Overlay with DROP. */
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, part, level, gran, ext_num + 1,
        _FieldKeygenExtSelL3E4, _FieldKeygenExtSelL4, 159, 0, 0,
        _FP_EXT_ATTR_NOT_WITH_DROP);

    /* Initialize Level_4 extractor section. */
    level = 4;
    gran = 160;
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, part, level, gran, 0,
        _FieldKeygenExtSelL4, _FieldKeygenExtSelDisable, 0, 0, 0, 0);

    /* Temporary debug log messages. */
    LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
        "FP(unit %d) Verb: Extractor slice mode=%s.\n"), unit,
         (_FieldExtConfMode160Bits == stage_fc->ext_cfg_arr[emode]->mode)
         ? "160b" : "Invalid"));

    for (idx = 0; idx < _FP_EXT_LEVEL_COUNT; idx++) {

        LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
            "FP(unit %d) Verb: Extractor Level=%d conf_sz=%d.\n"),
             unit, idx, stage_fc->ext_cfg_arr[emode]->conf_size[idx]));

        for (conf = 0; conf < stage_fc->ext_cfg_arr[emode]->conf_size[idx];
             conf++) {
            cfg = stage_fc->ext_cfg_arr[emode]->ext_cfg[idx][conf];

            LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                "FP(unit %d) Verb: Ext_id=0x%x Gran=%d Ext_num=%d IN=%d OUT=%d"
                " Flags=%d In_use=%x.\n"),
                unit, cfg.ext_id, cfg.gran, cfg.ext_num, cfg.in_sec,
                cfg.out_sec, cfg.flags, cfg.in_use));
        }

        LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                "FP(unit %d) Verb: Extractor Num Sections=%d\n"),
                 unit, stage_fc->ext_cfg_arr[emode]->num_sec));

        for (sec = _FieldKeygenExtSelDisable; sec < _FieldKeygenExtSelCount;
             sec++) {
            /* Skip invalid sections. */
            if (NULL == stage_fc->ext_cfg_arr[emode]->sections[sec]) {
                continue;
            }
            LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                "FP(unit %d) Verb: Extractor section name=%d fill_bits=%d"
                " drain_bits=%d.\n"),
                 unit,
                 stage_fc->ext_cfg_arr[emode]->sections[sec]->sec,
                 stage_fc->ext_cfg_arr[emode]->sections[sec]->fill_bits,
                 stage_fc->ext_cfg_arr[emode]->sections[sec]->drain_bits
                 ));
        }
    }

    /* 320 bits mode - Initialize Level_1 32-bit extractor section. */
    level = 1;
    gran = 32;
    ext_num = 0;
    emode = _FieldExtConfMode320Bits;

    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num,
        _FieldKeygenExtSelL1E32, _FieldKeygenExtSelL2AE16, 0, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num + 1,
        _FieldKeygenExtSelL1E32, _FieldKeygenExtSelL2AE16, 32, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num,
        _FieldKeygenExtSelL1E32, _FieldKeygenExtSelL2BE16, 0, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num + 1,
        _FieldKeygenExtSelL1E32, _FieldKeygenExtSelL2BE16, 32, 0, 0, 0);

    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num + 2,
        _FieldKeygenExtSelL1E32, _FieldKeygenExtSelL2AE16, 64, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num + 3,
        _FieldKeygenExtSelL1E32, _FieldKeygenExtSelL2AE16, 96, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num + 2,
        _FieldKeygenExtSelL1E32, _FieldKeygenExtSelL2BE16, 64, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num + 3,
        _FieldKeygenExtSelL1E32, _FieldKeygenExtSelL2BE16, 96, 0, 0, 0);

    /* Initialize Level_1 16-bit extractor section. */
    level = 1;
    gran = 16;
    ext_num = 0;
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num,
        _FieldKeygenExtSelL1E16, _FieldKeygenExtSelL2BE16, 128, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num + 1,
        _FieldKeygenExtSelL1E16, _FieldKeygenExtSelL2BE16, 144, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num,
        _FieldKeygenExtSelL1E16, _FieldKeygenExtSelL2AE16, 128, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num + 1,
        _FieldKeygenExtSelL1E16, _FieldKeygenExtSelL2AE16, 144, 0, 0, 0);

    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num + 2,
        _FieldKeygenExtSelL1E16, _FieldKeygenExtSelL2BE16, 160, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num + 3,
        _FieldKeygenExtSelL1E16, _FieldKeygenExtSelL2BE16, 176, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num + 2,
        _FieldKeygenExtSelL1E16, _FieldKeygenExtSelL2AE16, 160, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num + 3,
        _FieldKeygenExtSelL1E16, _FieldKeygenExtSelL2AE16, 176, 0, 0, 0);

    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num + 4,
        _FieldKeygenExtSelL1E16, _FieldKeygenExtSelL2BE16, 192, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num + 5,
        _FieldKeygenExtSelL1E16, _FieldKeygenExtSelL2BE16, 208, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num + 4,
        _FieldKeygenExtSelL1E16, _FieldKeygenExtSelL2AE16, 192, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num + 5,
        _FieldKeygenExtSelL1E16, _FieldKeygenExtSelL2AE16, 208, 0, 0, 0);

    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num + 6,
        _FieldKeygenExtSelL1E16, _FieldKeygenExtSelL2BE16, 224, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num + 6,
        _FieldKeygenExtSelL1E16, _FieldKeygenExtSelL2AE16, 224, 0, 0, 0);

    /* Initialize Level_1 8-bit extractor section. */
    level = 1;
    gran = 8;
    ext_num = 0;
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num,
        _FieldKeygenExtSelL1E8, _FieldKeygenExtSelL2BE4, 0, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num + 1,
        _FieldKeygenExtSelL1E8, _FieldKeygenExtSelL2BE4, 8, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num,
        _FieldKeygenExtSelL1E8, _FieldKeygenExtSelL2AE4, 0, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num + 1,
        _FieldKeygenExtSelL1E8, _FieldKeygenExtSelL2AE4, 8, 0, 0, 0);

    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num + 2,
        _FieldKeygenExtSelL1E8, _FieldKeygenExtSelL2BE4, 16, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num + 3,
        _FieldKeygenExtSelL1E8, _FieldKeygenExtSelL2BE4, 24, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num + 2,
        _FieldKeygenExtSelL1E8, _FieldKeygenExtSelL2AE4, 16, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num + 3,
        _FieldKeygenExtSelL1E8, _FieldKeygenExtSelL2AE4, 24, 0, 0, 0);

    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num + 4,
        _FieldKeygenExtSelL1E8, _FieldKeygenExtSelL2BE4, 32, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num + 5,
        _FieldKeygenExtSelL1E8, _FieldKeygenExtSelL2BE4, 40, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num + 4,
        _FieldKeygenExtSelL1E8, _FieldKeygenExtSelL2AE4, 32, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num + 5,
        _FieldKeygenExtSelL1E8, _FieldKeygenExtSelL2AE4, 40, 0, 0, 0);

    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num + 6,
        _FieldKeygenExtSelL1E8, _FieldKeygenExtSelL2BE4, 48, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num + 6,
        _FieldKeygenExtSelL1E8, _FieldKeygenExtSelL2AE4, 48, 0, 0, 0);

    /* Initialize Level_1 4-bit extractor section. */
    level = 1;
    gran = 4;
    ext_num = 0;
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num,
        _FieldKeygenExtSelL1E4, _FieldKeygenExtSelL2BE4, 56, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num + 1,
        _FieldKeygenExtSelL1E4, _FieldKeygenExtSelL2BE4, 60, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num,
        _FieldKeygenExtSelL1E4, _FieldKeygenExtSelL2AE4, 56, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num + 1,
        _FieldKeygenExtSelL1E4, _FieldKeygenExtSelL2AE4, 60, 0, 0, 0);

    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num + 2,
        _FieldKeygenExtSelL1E4, _FieldKeygenExtSelL2BE4, 64, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num + 3,
        _FieldKeygenExtSelL1E4, _FieldKeygenExtSelL2BE4, 68, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num + 2,
        _FieldKeygenExtSelL1E4, _FieldKeygenExtSelL2AE4, 64, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num + 3,
        _FieldKeygenExtSelL1E4, _FieldKeygenExtSelL2AE4, 68, 0, 0, 0);

    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num + 4,
        _FieldKeygenExtSelL1E4, _FieldKeygenExtSelL2BE4, 72, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num + 5,
        _FieldKeygenExtSelL1E4, _FieldKeygenExtSelL2BE4, 76, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num + 4,
        _FieldKeygenExtSelL1E4, _FieldKeygenExtSelL2AE4, 72, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num + 5,
        _FieldKeygenExtSelL1E4, _FieldKeygenExtSelL2AE4, 76, 0, 0, 0);

    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num + 6,
        _FieldKeygenExtSelL1E4, _FieldKeygenExtSelL2BE4, 80, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num + 7,
        _FieldKeygenExtSelL1E4, _FieldKeygenExtSelL2BE4, 84, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num + 6,
        _FieldKeygenExtSelL1E4, _FieldKeygenExtSelL2AE4, 80, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num + 7,
        _FieldKeygenExtSelL1E4, _FieldKeygenExtSelL2AE4, 84, 0, 0, 0);

    /* Initialize Level_1 2-bit extractor section. */
    level = 1;
    gran = 2;
    ext_num = 0;
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num,
        _FieldKeygenExtSelL1E2, _FieldKeygenExtSelL2BE4, 88, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num + 1,
        _FieldKeygenExtSelL1E2, _FieldKeygenExtSelL2BE4, 90, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num,
        _FieldKeygenExtSelL1E2, _FieldKeygenExtSelL2AE4, 88, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num + 1,
        _FieldKeygenExtSelL1E2, _FieldKeygenExtSelL2AE4, 90, 0, 0, 0);

    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num + 2,
        _FieldKeygenExtSelL1E2, _FieldKeygenExtSelL2BE4, 92, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num + 3,
        _FieldKeygenExtSelL1E2, _FieldKeygenExtSelL2BE4, 94, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num + 2,
        _FieldKeygenExtSelL1E2, _FieldKeygenExtSelL2AE4, 92, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num + 3,
        _FieldKeygenExtSelL1E2, _FieldKeygenExtSelL2AE4, 94, 0, 0, 0);

    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num + 4,
        _FieldKeygenExtSelL1E2, _FieldKeygenExtSelL2BE4, 96, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num + 5,
        _FieldKeygenExtSelL1E2, _FieldKeygenExtSelL2BE4, 98, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num + 4,
        _FieldKeygenExtSelL1E2, _FieldKeygenExtSelL2AE4, 96, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num + 5,
        _FieldKeygenExtSelL1E2, _FieldKeygenExtSelL2AE4, 98, 0, 0, 0);

    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num + 6,
        _FieldKeygenExtSelL1E2, _FieldKeygenExtSelL2BE4, 100, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num + 7,
        _FieldKeygenExtSelL1E2, _FieldKeygenExtSelL2BE4, 102, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num + 6,
        _FieldKeygenExtSelL1E2, _FieldKeygenExtSelL2AE4, 100, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num + 7,
        _FieldKeygenExtSelL1E2, _FieldKeygenExtSelL2AE4, 102, 0, 0, 0);

    /* Initialize Level_2 16-bit extractors section. */
    level = 2;
    gran = 16;
    ext_num = 0;
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num,
        _FieldKeygenExtSelL2AE16, _FieldKeygenExtSelL3AE16, 0, 0, 0,
        (_FP_EXT_ATTR_NOT_WITH_IPBM));
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num + 1,
        _FieldKeygenExtSelL2AE16, _FieldKeygenExtSelL3AE16, 16, 0, 0,
        (_FP_EXT_ATTR_NOT_WITH_IPBM));

    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num + 2,
        _FieldKeygenExtSelL2AE16, _FieldKeygenExtSelL3AE16, 32, 0, 0,
	_FP_EXT_ATTR_NOT_WITH_SRC_DST_CONT_0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num + 3,
        _FieldKeygenExtSelL2AE16, _FieldKeygenExtSelL3AE16, 48, 0, 0,
	_FP_EXT_ATTR_NOT_WITH_SRC_DST_CONT_1);

    for (ext_num = 4; ext_num < 10; ext_num++) {
        _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num,
            _FieldKeygenExtSelL2AE16, _FieldKeygenExtSelL3AE4,
            ((ext_num - 4) * gran), 0, 0, 0);
    }

    level = 2;
    gran = 16;
    ext_num = 0;
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num,
        _FieldKeygenExtSelL2BE16, _FieldKeygenExtSelL3BE16, 0, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num + 1,
        _FieldKeygenExtSelL2BE16, _FieldKeygenExtSelL3BE16, 16, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num + 2,
        _FieldKeygenExtSelL2BE16, _FieldKeygenExtSelL3BE16, 32, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num + 3,
        _FieldKeygenExtSelL2BE16, _FieldKeygenExtSelL3BE16, 48, 0, 0, 0);

    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num + 4,
            _FieldKeygenExtSelL2BE16, _FieldKeygenExtSelL3BE4, 0, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num + 5,
            _FieldKeygenExtSelL2BE16, _FieldKeygenExtSelL3BE4, 16, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num + 6,
            _FieldKeygenExtSelL2BE16, _FieldKeygenExtSelL3BE4, 32, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num + 7,
            _FieldKeygenExtSelL2BE16, _FieldKeygenExtSelL3BE4, 48, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num + 8,
            _FieldKeygenExtSelL2BE16, _FieldKeygenExtSelL3BE4, 64, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num + 9,
            _FieldKeygenExtSelL2BE16, _FieldKeygenExtSelL3BE4, 80, 0, 0, 0);

    /* Initialize Level_2 1-bit extractors section. */
    level = 2;
    gran = 1;
    ext_num = 0;
    for (idx = 0; idx < num_pass_thru_bits; idx++) {
        _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran,
            (ext_num + idx), _FieldKeygenExtSelL2AE4, _FieldKeygenExtSelL3AE4,
            96, 0, 0, (_FP_EXT_ATTR_PASS_THRU));
    }

    /* Initialize Level_2 1-bit extractors section. */
    level = 2;
    gran = 1;
    ext_num = 0;
    for (idx = 0; idx < num_pass_thru_bits; idx++) {
        _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran,
            (ext_num + idx), _FieldKeygenExtSelL2BE4, _FieldKeygenExtSelL3BE4,
            96, 0, 0, (_FP_EXT_ATTR_PASS_THRU));
    }

    /* Initialize Level_3 16-bit extractors section - SLICE_A. */
    level = 3;
    gran = 16;
    ext_num = 0;
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num,
        _FieldKeygenExtSelL3AE16, _FieldKeygenExtSelL4A, 0, 0, 0,
        (_FP_EXT_ATTR_NOT_WITH_IPBM));
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num + 1,
        _FieldKeygenExtSelL3AE16, _FieldKeygenExtSelL4A, 20, 0, 0,
        (_FP_EXT_ATTR_NOT_WITH_IPBM));

    /* Overlay with _FP_POST_MUX_SRC_DST_CONT_0/1. */
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num + 2,
        _FieldKeygenExtSelL3AE16, _FieldKeygenExtSelL4A, 48, 0, 0,
        _FP_EXT_ATTR_NOT_WITH_SRC_DST_CONT_0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num + 3,
        _FieldKeygenExtSelL3AE16, _FieldKeygenExtSelL4A, 64, 0, 0,
	_FP_EXT_ATTR_NOT_WITH_SRC_DST_CONT_1);

    /* Initialize Level_3 4-bit extractors section. */
    level = 3;
    gran = 4;
    ext_num = 0;
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num,
        _FieldKeygenExtSelL3AE4, _FieldKeygenExtSelL4A, 16, 0, 0,
        (_FP_EXT_ATTR_NOT_WITH_IPBM));

    for (idx = 1; idx < 4; idx++) {
        _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran,
            (ext_num + idx), _FieldKeygenExtSelL3AE4, _FieldKeygenExtSelL4A,
            36 + (gran * (idx - 1)), 0, 0, 0);
    }

    level = 3;
    gran = 4;
    ext_num = 4;
    for (idx = 0; idx < 17; idx++) {
        _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran,
            (ext_num + idx), _FieldKeygenExtSelL3AE4, _FieldKeygenExtSelL4A,
            80 + (idx * gran), 0, 0, 0);
    }

    /* Initialize Level_3 2-bit extractors section. */
    level = 3;
    gran = 2;
    for (ext_num = 0; ext_num < 2; ext_num++) {
        _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num,
            _FieldKeygenExtSelL3AE4, _FieldKeygenExtSelL4A,
            148 + (ext_num * gran), 0, 0, 0);
    }

    /* Overlay with _FP_POST_MUX_SRC_DST_CONT_0/1. */
    ext_num = 0;
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num + 2,
        _FieldKeygenExtSelL3AE4, _FieldKeygenExtSelL4A, 152, 0, 0,
        (_FP_EXT_ATTR_NOT_WITH_SRC_DST_CONT_0 |
         _FP_EXT_ATTR_NOT_WITH_SRC_DST_CONT_1));

    ext_num = 0;
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num + 3,
        _FieldKeygenExtSelL3AE4, _FieldKeygenExtSelL4A, 154, 0, 0, 0);

    /* Overlay with NAT_DST_REALM_ID. */
    ext_num = 0;
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num + 4,
        _FieldKeygenExtSelL3AE4, _FieldKeygenExtSelL4A, 156, 0, 0,
        _FP_EXT_ATTR_NOT_WITH_NAT_DST_REALM_ID);

    /* Initialize 2 Level_3 1-bit extractors section. */
    level = 3;
    gran = 1;
    ext_num = 0;
    /* Overlay with NAT_NEEDED. */
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num,
        _FieldKeygenExtSelL3AE4, _FieldKeygenExtSelL4A, 158, 0, 0,
        _FP_EXT_ATTR_NOT_WITH_NAT_NEEDED);
    /* Overlay with DROP. */
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num + 1,
        _FieldKeygenExtSelL3AE4, _FieldKeygenExtSelL4A, 159, 0, 0,
        _FP_EXT_ATTR_NOT_WITH_DROP);

    /* Initialize Level_3 16-bit extractors section - SLICE_B. */
    level = 3;
    gran = 16;
    ext_num = 0;
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num,
        _FieldKeygenExtSelL3BE16, _FieldKeygenExtSelL4B, 0, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num + 1,
        _FieldKeygenExtSelL3BE16, _FieldKeygenExtSelL4B, 20, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num + 2,
        _FieldKeygenExtSelL3BE16, _FieldKeygenExtSelL4B, 48, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num + 3,
        _FieldKeygenExtSelL3BE16, _FieldKeygenExtSelL4B, 64, 0, 0, 0);

    /* Initialize Level_3 4-bit extractors section. */
    level = 3;
    gran = 4;
    ext_num = 0;
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num,
        _FieldKeygenExtSelL3BE4, _FieldKeygenExtSelL4B, 16, 0, 0, 0);

    for (idx = 1; idx < 4; idx++) {
        _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran,
            (ext_num + idx), _FieldKeygenExtSelL3BE4, _FieldKeygenExtSelL4B,
            36 + (gran * (idx - 1)), 0, 0, 0);
    }

    level = 3;
    gran = 4;
    ext_num = 4;
    for (idx = 0; idx < 17; idx++) {
        _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran,
            (ext_num + idx), _FieldKeygenExtSelL3BE4, _FieldKeygenExtSelL4B,
            80 + (idx * gran), 0, 0, 0);
    }

    /* Initialize Level_3 2-bit extractors section. */
    level = 3;
    gran = 2;
    for (ext_num = 0; ext_num < 5; ext_num++) {
        _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num,
            _FieldKeygenExtSelL3BE4, _FieldKeygenExtSelL4B,
            148 + (ext_num * gran), 0, 0, 0);
    }

    /* Initialize 2 Level_3 1-bit extractors section. */
    level = 3;
    gran = 1;
    for (ext_num = 0; ext_num < 2; ext_num++) {
        _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num,
            _FieldKeygenExtSelL3BE4, _FieldKeygenExtSelL4B,
            158 + (ext_num * gran), 0, 0, 0);
    }

    /* Initialize Level_4 extractor section. */
    level = 4;
    gran = 160;
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, 0,
        _FieldKeygenExtSelL4, _FieldKeygenExtSelDisable, 0, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, 0,
        _FieldKeygenExtSelL4A, _FieldKeygenExtSelDisable, 0, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, 0,
        _FieldKeygenExtSelL4B, _FieldKeygenExtSelDisable, 0, 0, 0, 0);

    /* Temporary debug log messages. */
    LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
        "FP(unit %d) Verb: Extractor slice mode=%s.\n"), unit,
         (_FieldExtConfMode320Bits == stage_fc->ext_cfg_arr[emode]->mode)
         ? "320" : "Invalid"));

    for (idx = 0; idx < _FP_EXT_LEVEL_COUNT; idx++) {

        LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
            "FP(unit %d) Verb: Extractor Level=%d conf_sz=%d.\n"),
             unit, idx, stage_fc->ext_cfg_arr[emode]->conf_size[idx]));

        for (conf = 0; conf < stage_fc->ext_cfg_arr[emode]->conf_size[idx];
             conf++) {
            cfg = stage_fc->ext_cfg_arr[emode]->ext_cfg[idx][conf];

            LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                "FP(unit %d) Verb: Ext_id=0x%x Gran=%d Ext_num=%d IN=%d OUT=%d"
                " Flags=%d In_use=%x.\n"),
                unit, cfg.ext_id, cfg.gran, cfg.ext_num, cfg.in_sec,
                cfg.out_sec, cfg.flags, cfg.in_use));
        }

        LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                "FP(unit %d) Verb: Extractor Num Sections=%d\n"),
                 unit, stage_fc->ext_cfg_arr[emode]->num_sec));

        for (sec = _FieldKeygenExtSelDisable; sec < _FieldKeygenExtSelCount;
             sec++) {
            /* Skip invalid sections. */
            if (NULL == stage_fc->ext_cfg_arr[emode]->sections[sec]) {
                continue;
            }
            LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                "FP(unit %d) Verb: Extractor section name=%d fill_bits=%d"
                " drain_bits=%d.\n"),
                 unit,
                 stage_fc->ext_cfg_arr[emode]->sections[sec]->sec,
                 stage_fc->ext_cfg_arr[emode]->sections[sec]->fill_bits,
                 stage_fc->ext_cfg_arr[emode]->sections[sec]->drain_bits
                 ));
        }
    }

    /* 480 bit mode - Initialize Level_1 32-bit extractor section. */
    level = 1;
    gran = 32;
    ext_num = 0;
    emode = _FieldExtConfMode480Bits;

    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num,
        _FieldKeygenExtSelL1E32, _FieldKeygenExtSelL2AE16, 0, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num + 1,
        _FieldKeygenExtSelL1E32, _FieldKeygenExtSelL2AE16, 32, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num,
        _FieldKeygenExtSelL1E32, _FieldKeygenExtSelL2BE16, 0, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num + 1,
        _FieldKeygenExtSelL1E32, _FieldKeygenExtSelL2BE16, 32, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 2, level, gran, ext_num,
        _FieldKeygenExtSelL1E32, _FieldKeygenExtSelL2CE16, 0, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 2, level, gran, ext_num + 1,
        _FieldKeygenExtSelL1E32, _FieldKeygenExtSelL2CE16, 32, 0, 0, 0);

    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num + 2,
        _FieldKeygenExtSelL1E32, _FieldKeygenExtSelL2AE16, 64, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num + 3,
        _FieldKeygenExtSelL1E32, _FieldKeygenExtSelL2AE16, 96, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num + 2,
        _FieldKeygenExtSelL1E32, _FieldKeygenExtSelL2BE16, 64, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num + 3,
        _FieldKeygenExtSelL1E32, _FieldKeygenExtSelL2BE16, 96, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 2, level, gran, ext_num + 2,
        _FieldKeygenExtSelL1E32, _FieldKeygenExtSelL2CE16, 64, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 2, level, gran, ext_num + 3,
        _FieldKeygenExtSelL1E32, _FieldKeygenExtSelL2CE16, 96, 0, 0, 0);

    /* Initialize Level_1 16-bit extractor section. */
    level = 1;
    gran = 16;
    ext_num = 0;
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num,
        _FieldKeygenExtSelL1E16, _FieldKeygenExtSelL2BE16, 128, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num + 1,
        _FieldKeygenExtSelL1E16, _FieldKeygenExtSelL2BE16, 144, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 2, level, gran, ext_num,
        _FieldKeygenExtSelL1E16, _FieldKeygenExtSelL2CE16, 128, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 2, level, gran, ext_num + 1,
        _FieldKeygenExtSelL1E16, _FieldKeygenExtSelL2CE16, 144, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num,
        _FieldKeygenExtSelL1E16, _FieldKeygenExtSelL2AE16, 128, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num + 1,
        _FieldKeygenExtSelL1E16, _FieldKeygenExtSelL2AE16, 144, 0, 0, 0);

    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num + 2,
        _FieldKeygenExtSelL1E16, _FieldKeygenExtSelL2BE16, 160, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num + 3,
        _FieldKeygenExtSelL1E16, _FieldKeygenExtSelL2BE16, 176, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 2, level, gran, ext_num + 2,
        _FieldKeygenExtSelL1E16, _FieldKeygenExtSelL2CE16, 160, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 2, level, gran, ext_num + 3,
        _FieldKeygenExtSelL1E16, _FieldKeygenExtSelL2CE16, 176, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num + 2,
        _FieldKeygenExtSelL1E16, _FieldKeygenExtSelL2AE16, 160, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num + 3,
        _FieldKeygenExtSelL1E16, _FieldKeygenExtSelL2AE16, 176, 0, 0, 0);

    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num + 4,
        _FieldKeygenExtSelL1E16, _FieldKeygenExtSelL2BE16, 192, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num + 5,
        _FieldKeygenExtSelL1E16, _FieldKeygenExtSelL2BE16, 208, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 2, level, gran, ext_num + 4,
        _FieldKeygenExtSelL1E16, _FieldKeygenExtSelL2CE16, 192, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 2, level, gran, ext_num + 5,
        _FieldKeygenExtSelL1E16, _FieldKeygenExtSelL2CE16, 208, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num + 4,
        _FieldKeygenExtSelL1E16, _FieldKeygenExtSelL2AE16, 192, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num + 5,
        _FieldKeygenExtSelL1E16, _FieldKeygenExtSelL2AE16, 208, 0, 0, 0);

    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num + 6,
        _FieldKeygenExtSelL1E16, _FieldKeygenExtSelL2BE16, 224, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 2, level, gran, ext_num + 6,
        _FieldKeygenExtSelL1E16, _FieldKeygenExtSelL2CE16, 224, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num + 6,
        _FieldKeygenExtSelL1E16, _FieldKeygenExtSelL2AE16, 224, 0, 0, 0);

    /* Initialize Level_1 8-bit extractor section. */
    level = 1;
    gran = 8;
    ext_num = 0;
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num,
        _FieldKeygenExtSelL1E8, _FieldKeygenExtSelL2BE4, 0, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num + 1,
        _FieldKeygenExtSelL1E8, _FieldKeygenExtSelL2BE4, 8, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 2, level, gran, ext_num,
        _FieldKeygenExtSelL1E8, _FieldKeygenExtSelL2CE4, 0, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 2, level, gran, ext_num + 1,
        _FieldKeygenExtSelL1E8, _FieldKeygenExtSelL2CE4, 8, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num,
        _FieldKeygenExtSelL1E8, _FieldKeygenExtSelL2AE4, 0, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num + 1,
        _FieldKeygenExtSelL1E8, _FieldKeygenExtSelL2AE4, 8, 0, 0, 0);

    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num + 2,
        _FieldKeygenExtSelL1E8, _FieldKeygenExtSelL2BE4, 16, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num + 3,
        _FieldKeygenExtSelL1E8, _FieldKeygenExtSelL2BE4, 24, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 2, level, gran, ext_num + 2,
        _FieldKeygenExtSelL1E8, _FieldKeygenExtSelL2CE4, 16, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 2, level, gran, ext_num + 3,
        _FieldKeygenExtSelL1E8, _FieldKeygenExtSelL2CE4, 24, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num + 2,
        _FieldKeygenExtSelL1E8, _FieldKeygenExtSelL2AE4, 16, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num + 3,
        _FieldKeygenExtSelL1E8, _FieldKeygenExtSelL2AE4, 24, 0, 0, 0);

    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num + 4,
        _FieldKeygenExtSelL1E8, _FieldKeygenExtSelL2BE4, 32, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num + 5,
        _FieldKeygenExtSelL1E8, _FieldKeygenExtSelL2BE4, 40, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 2, level, gran, ext_num + 4,
        _FieldKeygenExtSelL1E8, _FieldKeygenExtSelL2CE4, 32, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 2, level, gran, ext_num + 5,
        _FieldKeygenExtSelL1E8, _FieldKeygenExtSelL2CE4, 40, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num + 4,
        _FieldKeygenExtSelL1E8, _FieldKeygenExtSelL2AE4, 32, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num + 5,
        _FieldKeygenExtSelL1E8, _FieldKeygenExtSelL2AE4, 40, 0, 0, 0);

    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num + 6,
        _FieldKeygenExtSelL1E8, _FieldKeygenExtSelL2BE4, 48, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 2, level, gran, ext_num + 6,
        _FieldKeygenExtSelL1E8, _FieldKeygenExtSelL2CE4, 48, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num + 6,
        _FieldKeygenExtSelL1E8, _FieldKeygenExtSelL2AE4, 48, 0, 0, 0);

    /* Initialize Level_1 4-bit extractor section. */
    level = 1;
    gran = 4;
    ext_num = 0;
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num,
        _FieldKeygenExtSelL1E4, _FieldKeygenExtSelL2BE4, 56, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num + 1,
        _FieldKeygenExtSelL1E4, _FieldKeygenExtSelL2BE4, 60, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 2, level, gran, ext_num,
        _FieldKeygenExtSelL1E4, _FieldKeygenExtSelL2CE4, 56, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 2, level, gran, ext_num + 1,
        _FieldKeygenExtSelL1E4, _FieldKeygenExtSelL2CE4, 60, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num,
        _FieldKeygenExtSelL1E4, _FieldKeygenExtSelL2AE4, 56, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num + 1,
        _FieldKeygenExtSelL1E4, _FieldKeygenExtSelL2AE4, 60, 0, 0, 0);

    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num + 2,
        _FieldKeygenExtSelL1E4, _FieldKeygenExtSelL2BE4, 64, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num + 3,
        _FieldKeygenExtSelL1E4, _FieldKeygenExtSelL2BE4, 68, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 2, level, gran, ext_num + 2,
        _FieldKeygenExtSelL1E4, _FieldKeygenExtSelL2CE4, 64, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 2, level, gran, ext_num + 3,
        _FieldKeygenExtSelL1E4, _FieldKeygenExtSelL2CE4, 68, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num + 2,
        _FieldKeygenExtSelL1E4, _FieldKeygenExtSelL2AE4, 64, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num + 3,
        _FieldKeygenExtSelL1E4, _FieldKeygenExtSelL2AE4, 68, 0, 0, 0);

    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num + 4,
        _FieldKeygenExtSelL1E4, _FieldKeygenExtSelL2BE4, 72, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num + 5,
        _FieldKeygenExtSelL1E4, _FieldKeygenExtSelL2BE4, 76, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 2, level, gran, ext_num + 4,
        _FieldKeygenExtSelL1E4, _FieldKeygenExtSelL2CE4, 72, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 2, level, gran, ext_num + 5,
        _FieldKeygenExtSelL1E4, _FieldKeygenExtSelL2CE4, 76, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num + 4,
        _FieldKeygenExtSelL1E4, _FieldKeygenExtSelL2AE4, 72, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num + 5,
        _FieldKeygenExtSelL1E4, _FieldKeygenExtSelL2AE4, 76, 0, 0, 0);

    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num + 6,
        _FieldKeygenExtSelL1E4, _FieldKeygenExtSelL2BE4, 80, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num + 7,
        _FieldKeygenExtSelL1E4, _FieldKeygenExtSelL2BE4, 84, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 2, level, gran, ext_num + 6,
        _FieldKeygenExtSelL1E4, _FieldKeygenExtSelL2CE4, 80, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 2, level, gran, ext_num + 7,
        _FieldKeygenExtSelL1E4, _FieldKeygenExtSelL2CE4, 84, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num + 6,
        _FieldKeygenExtSelL1E4, _FieldKeygenExtSelL2AE4, 80, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num + 7,
        _FieldKeygenExtSelL1E4, _FieldKeygenExtSelL2AE4, 84, 0, 0, 0);

    /* Initialize Level_1 2-bit extractor section. */
    level = 1;
    gran = 2;
    ext_num = 0;
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 2, level, gran, ext_num,
        _FieldKeygenExtSelL1E2, _FieldKeygenExtSelL2CE4, 88, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 2, level, gran, ext_num + 1,
        _FieldKeygenExtSelL1E2, _FieldKeygenExtSelL2CE4, 90, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num,
        _FieldKeygenExtSelL1E2, _FieldKeygenExtSelL2BE4, 88, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num + 1,
        _FieldKeygenExtSelL1E2, _FieldKeygenExtSelL2BE4, 90, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num,
        _FieldKeygenExtSelL1E2, _FieldKeygenExtSelL2AE4, 88, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num + 1,
        _FieldKeygenExtSelL1E2, _FieldKeygenExtSelL2AE4, 90, 0, 0, 0);

    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 2, level, gran, ext_num + 2,
        _FieldKeygenExtSelL1E2, _FieldKeygenExtSelL2CE4, 92, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 2, level, gran, ext_num + 3,
        _FieldKeygenExtSelL1E2, _FieldKeygenExtSelL2CE4, 94, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num + 2,
        _FieldKeygenExtSelL1E2, _FieldKeygenExtSelL2BE4, 92, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num + 3,
        _FieldKeygenExtSelL1E2, _FieldKeygenExtSelL2BE4, 94, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num + 2,
        _FieldKeygenExtSelL1E2, _FieldKeygenExtSelL2AE4, 92, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num + 3,
        _FieldKeygenExtSelL1E2, _FieldKeygenExtSelL2AE4, 94, 0, 0, 0);

    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 2, level, gran, ext_num + 4,
        _FieldKeygenExtSelL1E2, _FieldKeygenExtSelL2CE4, 96, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 2, level, gran, ext_num + 5,
        _FieldKeygenExtSelL1E2, _FieldKeygenExtSelL2CE4, 98, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num + 4,
        _FieldKeygenExtSelL1E2, _FieldKeygenExtSelL2BE4, 96, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num + 5,
        _FieldKeygenExtSelL1E2, _FieldKeygenExtSelL2BE4, 98, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num + 4,
        _FieldKeygenExtSelL1E2, _FieldKeygenExtSelL2AE4, 96, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num + 5,
        _FieldKeygenExtSelL1E2, _FieldKeygenExtSelL2AE4, 98, 0, 0, 0);

    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 2, level, gran, ext_num + 6,
        _FieldKeygenExtSelL1E2, _FieldKeygenExtSelL2CE4, 100, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 2, level, gran, ext_num + 7,
        _FieldKeygenExtSelL1E2, _FieldKeygenExtSelL2CE4, 102, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num + 6,
        _FieldKeygenExtSelL1E2, _FieldKeygenExtSelL2BE4, 100, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num + 7,
        _FieldKeygenExtSelL1E2, _FieldKeygenExtSelL2BE4, 102, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num + 6,
        _FieldKeygenExtSelL1E2, _FieldKeygenExtSelL2AE4, 100, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num + 7,
        _FieldKeygenExtSelL1E2, _FieldKeygenExtSelL2AE4, 102, 0, 0, 0);

    /* Initialize Level_2 16-bit extractors section - Slice_A. */
    level = 2;
    gran = 16;
    ext_num = 0;
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num,
        _FieldKeygenExtSelL2AE16, _FieldKeygenExtSelL3AE16, 0, 0, 0,
        (_FP_EXT_ATTR_NOT_WITH_IPBM));
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num + 1,
        _FieldKeygenExtSelL2AE16, _FieldKeygenExtSelL3AE16, 16, 0, 0,
        (_FP_EXT_ATTR_NOT_WITH_IPBM));
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num + 2,
        _FieldKeygenExtSelL2AE16, _FieldKeygenExtSelL3AE16, 32, 0, 0,
	_FP_EXT_ATTR_NOT_WITH_SRC_DST_CONT_0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode,  0, level, gran, ext_num + 3,
        _FieldKeygenExtSelL2AE16, _FieldKeygenExtSelL3AE16, 48, 0, 0,
	_FP_EXT_ATTR_NOT_WITH_SRC_DST_CONT_1);
    for (ext_num = 4; ext_num < 10; ext_num++) {
        _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num,
            _FieldKeygenExtSelL2AE16, _FieldKeygenExtSelL3AE4,
            ((ext_num - 4) * gran), 0, 0, 0);
    }

    /* Initialize Level_2 1-bit extractors section. */
    level = 2;
    gran = 1;
    ext_num = 0;
    for (idx = 0; idx < num_pass_thru_bits; idx++) {
        _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran,
            (ext_num + idx), _FieldKeygenExtSelL2AE4, _FieldKeygenExtSelL3AE4,
            96, 0, 0, (_FP_EXT_ATTR_PASS_THRU));
    }

    /* Initialize Level_2 16-bit extractors section - Slice_B. */
    level = 2;
    gran = 16;
    ext_num = 0;
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num,
        _FieldKeygenExtSelL2BE16, _FieldKeygenExtSelL3BE16, 0, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num + 1,
        _FieldKeygenExtSelL2BE16, _FieldKeygenExtSelL3BE16, 16, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num + 2,
        _FieldKeygenExtSelL2BE16, _FieldKeygenExtSelL3BE16, 32, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num + 3,
        _FieldKeygenExtSelL2BE16, _FieldKeygenExtSelL3BE16, 48, 0, 0, 0);

    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num + 4,
        _FieldKeygenExtSelL2BE16, _FieldKeygenExtSelL3BE4, 0, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num + 5,
        _FieldKeygenExtSelL2BE16, _FieldKeygenExtSelL3BE4, 16, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num + 6,
        _FieldKeygenExtSelL2BE16, _FieldKeygenExtSelL3BE4, 32, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num + 7,
        _FieldKeygenExtSelL2BE16, _FieldKeygenExtSelL3BE4, 48, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num + 8,
        _FieldKeygenExtSelL2BE16, _FieldKeygenExtSelL3BE4, 64, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num + 9,
        _FieldKeygenExtSelL2BE16, _FieldKeygenExtSelL3BE4, 80, 0, 0, 0);

    /* Initialize Level_2 1-bit extractors section. */
    level = 2;
    gran = 1;
    ext_num = 0;
    for (idx = 0; idx < num_pass_thru_bits; idx++) {
        _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran,
            (ext_num + idx), _FieldKeygenExtSelL2BE4, _FieldKeygenExtSelL3BE4,
            96, 0, 0, (_FP_EXT_ATTR_PASS_THRU));
    }

    /* Initialize Level_2 16-bit extractors section - Slice_C. */
    level = 2;
    gran = 16;
    ext_num = 0;
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 2, level, gran, ext_num,
        _FieldKeygenExtSelL2CE16, _FieldKeygenExtSelL3CE16, 0, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 2, level, gran, ext_num + 1,
        _FieldKeygenExtSelL2CE16, _FieldKeygenExtSelL3CE16, 16, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 2, level, gran, ext_num + 2,
        _FieldKeygenExtSelL2CE16, _FieldKeygenExtSelL3CE16, 32, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 2, level, gran, ext_num + 3,
        _FieldKeygenExtSelL2CE16, _FieldKeygenExtSelL3CE16, 48, 0, 0, 0);

    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 2, level, gran, ext_num + 4,
        _FieldKeygenExtSelL2CE16, _FieldKeygenExtSelL3CE4, 0, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 2, level, gran, ext_num + 5,
        _FieldKeygenExtSelL2CE16, _FieldKeygenExtSelL3CE4, 16, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 2, level, gran, ext_num + 6,
        _FieldKeygenExtSelL2CE16, _FieldKeygenExtSelL3CE4, 32, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 2, level, gran, ext_num + 7,
        _FieldKeygenExtSelL2CE16, _FieldKeygenExtSelL3CE4, 48, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 2, level, gran, ext_num + 8,
        _FieldKeygenExtSelL2CE16, _FieldKeygenExtSelL3CE4, 64, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 2, level, gran, ext_num + 9,
        _FieldKeygenExtSelL2CE16, _FieldKeygenExtSelL3CE4, 80, 0, 0, 0);

    /* Initialize Level_2 1-bit extractors section. */
    level = 2;
    gran = 1;
    ext_num = 0;
    for (idx = 0; idx < num_pass_thru_bits; idx++) {
        _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 2, level, gran,
            (ext_num + idx), _FieldKeygenExtSelL2CE4, _FieldKeygenExtSelL3CE4,
            96, 0, 0, (_FP_EXT_ATTR_PASS_THRU));
    }

    /* Initialize Level_3 16-bit extractors section - SLICE_A. */
    level = 3;
    gran = 16;
    ext_num = 0;
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num,
        _FieldKeygenExtSelL3AE16, _FieldKeygenExtSelL4A, 0, 0, 0,
        (_FP_EXT_ATTR_NOT_WITH_IPBM));
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num + 1,
        _FieldKeygenExtSelL3AE16, _FieldKeygenExtSelL4A, 20, 0, 0,
        (_FP_EXT_ATTR_NOT_WITH_IPBM));
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num + 2,
        _FieldKeygenExtSelL3AE16, _FieldKeygenExtSelL4A, 48, 0, 0,
        _FP_EXT_ATTR_NOT_WITH_SRC_DST_CONT_0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num + 3,
        _FieldKeygenExtSelL3AE16, _FieldKeygenExtSelL4A, 64, 0, 0,
	_FP_EXT_ATTR_NOT_WITH_SRC_DST_CONT_1);

    /* Initialize Level_3 4-bit extractors section. */
    level = 3;
    gran = 4;
    ext_num = 0;
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num,
        _FieldKeygenExtSelL3AE4, _FieldKeygenExtSelL4A, 16, 0, 0,
        (_FP_EXT_ATTR_NOT_WITH_IPBM));
    for (idx = 1; idx < 4; idx++) {
        _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran,
            (ext_num + idx), _FieldKeygenExtSelL3AE4, _FieldKeygenExtSelL4A,
            36 + (gran * (idx - 1)), 0, 0, 0);
    }

    level = 3;
    gran = 4;
    ext_num = 4;
    for (idx = 0; idx < 17; idx++) {
        _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran,
            (ext_num + idx), _FieldKeygenExtSelL3AE4, _FieldKeygenExtSelL4A,
            80 + (idx * gran), 0, 0, 0);
    }
    /* Initialize Level_3 2-bit extractors section. */
    level = 3;
    gran = 2;
    for (ext_num = 0; ext_num < 2; ext_num++) {
        _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num,
            _FieldKeygenExtSelL3AE4, _FieldKeygenExtSelL4A,
            148 + (ext_num * gran), 0, 0, 0);
    }
    
    /* Overlay with _FP_POST_MUX_SRC_DST_CONT_0/1. */
    ext_num = 0;
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num + 2,
        _FieldKeygenExtSelL3AE4, _FieldKeygenExtSelL4A, 152, 0, 0,
        (_FP_EXT_ATTR_NOT_WITH_SRC_DST_CONT_0 |
         _FP_EXT_ATTR_NOT_WITH_SRC_DST_CONT_1));
    ext_num = 0;
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num + 3,
        _FieldKeygenExtSelL3AE4, _FieldKeygenExtSelL4A, 154, 0, 0, 0);

    /* Overlay with NAT_DST_REALM_ID. */
    ext_num = 0;
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num + 4,
        _FieldKeygenExtSelL3AE4, _FieldKeygenExtSelL4A, 156, 0, 0,
        _FP_EXT_ATTR_NOT_WITH_NAT_DST_REALM_ID);

    /* Initialize Level_3 1-bit extractors section. */
    level = 3;
    gran = 1;
    ext_num = 0;
    /* Overlay with NAT_NEEDED. */
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num,
        _FieldKeygenExtSelL3AE4, _FieldKeygenExtSelL4A, 158, 0, 0,
        _FP_EXT_ATTR_NOT_WITH_NAT_NEEDED);
    /* Overlay with DROP. */
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, ext_num + 1,
        _FieldKeygenExtSelL3AE4, _FieldKeygenExtSelL4A, 159, 0, 0,
        _FP_EXT_ATTR_NOT_WITH_DROP);

    /* Initialize Level_3 16-bit extractors section - SLICE_B. */
    level = 3;
    gran = 16;
    ext_num = 0;
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num,
        _FieldKeygenExtSelL3BE16, _FieldKeygenExtSelL4B, 0, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num + 1,
        _FieldKeygenExtSelL3BE16, _FieldKeygenExtSelL4B, 20, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num + 2,
        _FieldKeygenExtSelL3BE16, _FieldKeygenExtSelL4B, 48, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num + 3,
        _FieldKeygenExtSelL3BE16, _FieldKeygenExtSelL4B, 64, 0, 0, 0);

    /* Initialize Level_3 4-bit extractors section. */
    level = 3;
    gran = 4;
    ext_num = 0;
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num,
        _FieldKeygenExtSelL3BE4, _FieldKeygenExtSelL4B, 16, 0, 0, 0);
    for (idx = 1; idx < 4; idx++) {
        _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran,
            (ext_num + idx), _FieldKeygenExtSelL3BE4, _FieldKeygenExtSelL4B,
            36 + (gran * (idx - 1)), 0, 0, 0);
    }
    level = 3;
    gran = 4;
    ext_num = 4;
    for (idx = 0; idx < 17; idx++) {
        _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran,
            (ext_num + idx), _FieldKeygenExtSelL3BE4, _FieldKeygenExtSelL4B,
            80 + (idx * gran), 0, 0, 0);
    }
    /* Initialize Level_3 2-bit extractors section. */
    level = 3;
    gran = 2;
    for (ext_num = 0; ext_num < 5; ext_num++) {
        _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num,
            _FieldKeygenExtSelL3BE4, _FieldKeygenExtSelL4B,
            148 + (gran * ext_num), 0, 0, 0);
    }

    /* Initialize Level_3 1-bit extractors section. */
    level = 3;
    gran = 1;
    for (ext_num = 0; ext_num < 2; ext_num++) {
        _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, ext_num,
            _FieldKeygenExtSelL3BE4, _FieldKeygenExtSelL4B,
            158 + (gran * ext_num), 0, 0, 0);
    }

    /* Initialize Level_3 16-bit extractors section - SLICE_C. */
    level = 3;
    gran = 16;
    ext_num = 0;
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 2, level, gran, ext_num,
        _FieldKeygenExtSelL3CE16, _FieldKeygenExtSelL4C, 0, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 2, level, gran, ext_num + 1,
        _FieldKeygenExtSelL3CE16, _FieldKeygenExtSelL4C, 20, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 2, level, gran, ext_num + 2,
        _FieldKeygenExtSelL3CE16, _FieldKeygenExtSelL4C, 48, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 2, level, gran, ext_num + 3,
        _FieldKeygenExtSelL3CE16, _FieldKeygenExtSelL4C, 64, 0, 0, 0);

    /* Initialize Level_3 4-bit extractors section. */
    level = 3;
    gran = 4;
    ext_num = 0;
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 2, level, gran, ext_num,
        _FieldKeygenExtSelL3CE4, _FieldKeygenExtSelL4C, 16, 0, 0, 0);
    for (idx = 1; idx < 4; idx++) {
        _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 2, level, gran,
            (ext_num + idx), _FieldKeygenExtSelL3CE4, _FieldKeygenExtSelL4C,
            36 + (gran * (idx - 1)), 0, 0, 0);
    }

    level = 3;
    gran = 4;
    ext_num = 4;
    for (idx = 0; idx < 17; idx++) {
        _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 2, level, gran,
            (ext_num + idx), _FieldKeygenExtSelL3CE4, _FieldKeygenExtSelL4C,
            80 + (idx * gran), 0, 0, 0);
    }

    /* Initialize Level_3 2-bit extractors section. */
    level = 3;
    gran = 2;
    for (ext_num = 0; ext_num < 5; ext_num++) {
        _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 2, level, gran, ext_num,
            _FieldKeygenExtSelL3CE4, _FieldKeygenExtSelL4C,
            148 + (gran * ext_num), 0, 0, 0);
    }

    /* Initialize Level_3 1-bit extractors section. */
    level = 3;
    gran = 1;
    for (ext_num = 0; ext_num < 2; ext_num++) {
        _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 2, level, gran, ext_num,
            _FieldKeygenExtSelL3CE4, _FieldKeygenExtSelL4C,
            158 + (gran * ext_num), 0, 0, 0);
    }

    /* Initialize Level_4 extractor section. */
    level = 4;
    gran = 160;
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, 0,
        _FieldKeygenExtSelL4, _FieldKeygenExtSelDisable, 0, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 0, level, gran, 0,
        _FieldKeygenExtSelL4A, _FieldKeygenExtSelDisable, 0, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 1, level, gran, 0,
        _FieldKeygenExtSelL4B, _FieldKeygenExtSelDisable, 0, 0, 0, 0);
    _FP_EXT_CONFIG_ADD(unit, stage_fc, emode, 2, level, gran, 0,
        _FieldKeygenExtSelL4C, _FieldKeygenExtSelDisable, 0, 0, 0, 0);

    /* Temporary debug log messages. */
    LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
        "FP(unit %d) Verb: Extractor slice mode=%s.\n"), unit,
         (_FieldExtConfMode480Bits == stage_fc->ext_cfg_arr[emode]->mode)
         ? "480" : "Invalid"));

    for (idx = 0; idx < _FP_EXT_LEVEL_COUNT; idx++) {

        LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
            "FP(unit %d) Verb: Extractor Level=%d conf_sz=%d.\n"),
             unit, idx, stage_fc->ext_cfg_arr[emode]->conf_size[idx]));

        for (conf = 0; conf < stage_fc->ext_cfg_arr[emode]->conf_size[idx];
             conf++) {
            cfg = stage_fc->ext_cfg_arr[emode]->ext_cfg[idx][conf];

            LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                "FP(unit %d) Verb: Ext_id=0x%x Gran=%d Ext_num=%d IN=%d OUT=%d"
                " Flags=%d In_use=%x.\n"),
                unit, cfg.ext_id, cfg.gran, cfg.ext_num, cfg.in_sec,
                cfg.out_sec, cfg.flags, cfg.in_use));
        }

        LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                "FP(unit %d) Verb: Extractor Num Sections=%d\n"),
                 unit, stage_fc->ext_cfg_arr[emode]->num_sec));

        for (sec = _FieldKeygenExtSelDisable; sec < _FieldKeygenExtSelCount;
             sec++) {
            /* Skip invalid sections. */
            if (NULL == stage_fc->ext_cfg_arr[emode]->sections[sec]) {
                continue;
            }
            LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                "FP(unit %d) Verb: Extractor section name=%d fill_bits=%d"
                " drain_bits=%d.\n"),
                 unit,
                 stage_fc->ext_cfg_arr[emode]->sections[sec]->sec,
                 stage_fc->ext_cfg_arr[emode]->sections[sec]->fill_bits,
                 stage_fc->ext_cfg_arr[emode]->sections[sec]->drain_bits
                 ));
        }
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *    _bcm_field_th_init
 * Purpose:
 *    Perform initializations that are specific to BCM56960. This
 *    includes initializing the FP field select bit offset tables for FPF[1-3]
 *    for every stage.
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    fc         - (IN) Field Processor control structure.
 *
 * Returns:
 *    BCM_E_PARAM - Null field control structure or stage control structure.
 *    BCM_E_INTERNAL - Invalid CAP stage ID.
 *    BCM_E_NONE - Success.
 */
int
_bcm_field_th_init(int unit, _field_control_t *fc)
{
    _field_stage_t *stage_fc; /* Pointer to Stage control structure. */

    /* Input parameters check. */
    if (NULL == fc || NULL == fc->stages) {
        return (BCM_E_PARAM);
    }

    /* Get the stage control handle. */
    stage_fc = fc->stages;

    /*
     * Initialize FP hardware tables based on the device Group Oper mode
     * settings.
     */
    switch (stage_fc->oper_mode) {
        case bcmFieldGroupOperModeGlobal:
            /* Legacy global mode, use DUPLICATE view to clear FP HW tables. */
            while (stage_fc) {
                if (!SAL_BOOT_BCMSIM && !SAL_BOOT_QUICKTURN && !SAL_BOOT_XGSSIM) {
                    /* Clear hardware table */
                    BCM_IF_ERROR_RETURN(_field_th_hw_clear(unit,
                        stage_fc));
                }

                /* Initialize qualifiers info. */
                BCM_IF_ERROR_RETURN(_field_th_qualifiers_init(unit, stage_fc));


                /* Initialize stage actions information. */
                BCM_IF_ERROR_RETURN(_field_th_actions_init(unit, stage_fc));

                /* Goto next stage */
                stage_fc = stage_fc->next;
            }

            if (0 == SOC_WARM_BOOT(unit)) {
                /* Enable FP meter refresh on all pipes. */
                BCM_IF_ERROR_RETURN(_field_meter_refresh_enable_set(unit, fc,
                    TRUE));
            }
            break;

        case bcmFieldGroupOperModePipeLocal:
            while (stage_fc) {
                if (!SAL_BOOT_BCMSIM && !SAL_BOOT_QUICKTURN && !SAL_BOOT_XGSSIM) {
                    /* Clear hardware table */
                    BCM_IF_ERROR_RETURN(_field_th_pipes_hw_clear(unit,
                        stage_fc));
                }

                /* Initialize qualifiers info. */
                BCM_IF_ERROR_RETURN(_field_th_qualifiers_init(unit, stage_fc));

                /* Initialize stage actions information. */
                BCM_IF_ERROR_RETURN(_field_th_actions_init(unit, stage_fc));

                /* Goto next stage */
                stage_fc = stage_fc->next;
            }
            if (0 == SOC_WARM_BOOT(unit)) {
                /* Enable FP meter refresh on all pipes. */
                BCM_IF_ERROR_RETURN(_field_th_pipes_meter_refresh_enable(unit,
                                                                     fc, TRUE));
            }
            break;

        default:
            break;
    }

    if (0 == SOC_WARM_BOOT(unit)) {
        /* Enable filter processor */
        BCM_IF_ERROR_RETURN(_field_port_filter_enable_set(unit, fc, TRUE));

    }

    /* Initialize the function pointers */
    _field_th_functions_init(&fc->functions);

    /* Register with Counter ejection */
    _bcm_th_field_counter_config_set(unit);

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_ingress_entry_tcam_wide_install
 * Purpose:
 *     Set the per-pipe PBMP in the Ingress TCAM
 * Parameters:
 *     unit         - (IN) BCM device number
 *     f_ent        - (IN) entry structure to get policy info from
 *     tcam_idx     - (IN) Index in the TCAM memory.
 *     _f_pbmp      - (IN) Reference to field PBMP.
 *     num_pipe     - (IN) Number of Pipes the _f_pbmp has.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_ingress_entry_tcam_wide_install(int unit, _field_entry_t *f_ent,
                                       int tcam_idx, _field_pbmp_t *_f_pbmp,
                                       int num_pipe)
{
    int            inst;                       /* Pipe Instance          */
    int            valid = 0;                  /* Valid bit              */
    _field_tcam_t  *tcam;
    uint32         entry_buf[SOC_MAX_MEM_FIELD_WORDS] = {0};
                                     /* Buffer to fill Policy & TCAM entry.*/
    soc_mem_t      ifp_tcam_wide[_FP_MAX_NUM_PIPES] =
                                     /* IFP TCAM memory names. */
                   {
                     IFP_TCAM_WIDE_PIPE0m,
                     IFP_TCAM_WIDE_PIPE1m,
                     IFP_TCAM_WIDE_PIPE2m,
                     IFP_TCAM_WIDE_PIPE3m
                   };

    if (f_ent == NULL || _f_pbmp == NULL) {
       return BCM_E_PARAM;
    }

    /* Update the valid bit based on the mode and group flags */
    valid = (f_ent->group->flags & _FP_GROUP_LOOKUP_ENABLED) ? 3 : 0;

    tcam = &f_ent->tcam;
    for (inst = 0; inst < num_pipe; inst++) {
      if (BCM_PBMP_IS_NULL(_f_pbmp[inst].data)) {
         /* Need not to set the entry in the Pipe */
         continue;
      }
      sal_memset(entry_buf, 0x0, sizeof(entry_buf));
      soc_mem_field_set(unit, ifp_tcam_wide[inst], entry_buf, KEYf, tcam->key);
      soc_mem_field_set(unit, ifp_tcam_wide[inst], entry_buf, MASKf, tcam->mask);
      soc_mem_field_set(unit, ifp_tcam_wide[inst], entry_buf,
                        IPBMf, (uint32 *)&_f_pbmp[inst].data);
      soc_mem_field_width_fit_set(unit, ifp_tcam_wide[inst], entry_buf,
                        IPBM_MASKf, (uint32 *)&_f_pbmp[inst].mask);
      soc_mem_field32_set(unit, ifp_tcam_wide[inst], entry_buf, VALIDf, valid);
      BCM_IF_ERROR_RETURN(soc_mem_write(unit,
                                        ifp_tcam_wide[inst],
                                        MEM_BLOCK_ALL,
                                        tcam_idx,
                                        entry_buf));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _field_ingress_pipe_pbmp_get
 * Purpose:
 *     Retrieve the per-pipe PBMP from the given pbmp
 * Parameters:
 *     unit         - (IN)  BCM device number
 *     _in_f_pbmp   - (IN)  Input PBMP, contains all pipes pbmp.
 *     pbm_len      - (IN)  length of per-pipe PBMP field.
 *     num_pipe     - (IN)  Number of Pipes the _out_f_pbmp has.
 *     _out_f_pbmp  - (OUT) Reference to per-pipe PBMP.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_ingress_pipe_pbmp_get(int               unit,
                             _field_pbmp_t     *_in_f_pbmp,
                             int               pbm_len,
                             int               num_pipe,
                             _field_pbmp_t     *_out_f_pbmp)
{
   int      inst;       /* Pipe Instance */
   int      port;       /* Input Port */
   int      pipe_port;  /* Pipe Port */
   int      pipe_ports_max;

   if (_in_f_pbmp == NULL || _out_f_pbmp == NULL) {
      return BCM_E_PARAM;
   }

   
#if 0
   /* Validate the input ports */
   BCM_IF_ERROR_RETURN(bcm_esw_port_config_get(unit, &port_config));
#endif
   pipe_ports_max = SOC_MAX_NUM_PORTS/SOC_MAX_NUM_PIPES;

   /* Update the Output Data Pipe Pbmp based on the input pbmp */
   BCM_PBMP_ITER(_in_f_pbmp->data, port) {
       SOC_IF_ERROR_RETURN
          (soc_port_pipe_get(unit, port, &inst));
       if (inst < num_pipe) {
          pipe_port = port - (inst * pipe_ports_max);
          if (pipe_port > pbm_len) {
             LOG_ERROR(BSL_LS_BCM_FP, (BSL_META(
               "Entry Data Pipe Port[%d] going beyond PBMP field length:[%d].\n\r"),
                pipe_port, pbm_len));
             return BCM_E_PARAM;
          }
          BCM_PBMP_PORT_ADD(_out_f_pbmp[inst].data, pipe_port);
       } else {
          LOG_ERROR(BSL_LS_BCM_FP, (BSL_META(
           "Retrieved Port instance:%d is out of Output Data pbmp range.\r"),
            inst));
           return BCM_E_INTERNAL;
       }
   }

   /* Update the Output Mask Pipe Pbmp based on the input pbmp */
   BCM_PBMP_ITER(_in_f_pbmp->mask, port) {
       SOC_IF_ERROR_RETURN
          (soc_port_pipe_get(unit, port, &inst));
       if (inst < num_pipe) {
          pipe_port = port - (inst * pipe_ports_max);
          if (pipe_port > pbm_len) {
             LOG_ERROR(BSL_LS_BCM_FP, (BSL_META(
               "Entry Mask Pipe Port[%d] going beyond PBMP field length:[%d].\n\r"),
                pipe_port, pbm_len));
             return BCM_E_PARAM;
          }
          BCM_PBMP_PORT_ADD(_out_f_pbmp[inst].mask, pipe_port);
       } else {
          LOG_ERROR(BSL_LS_BCM_FP, (BSL_META(
           "Retrieved Port instance:%d is out of Output Mask pbmp range.\r"),
            inst));
           return BCM_E_INTERNAL;
       }
   }

   return BCM_E_NONE;
}

/*
 * Function:
 *     _field_th_ingress_entry_qual_tcam_install
 * Purpose:
 *     Install the entry qualifiers in the Ingress TCAM.
 * Parameters:
 *     unit         - (IN) BCM device number
 *     f_ent        - (IN) entry structure to get policy info from
 *     tcam_mem     - (IN) TCAM memory index.
 *     tcam_idx     - (IN) Index in the TCAM memory.
 * Returns:
 *     BCM_E_XXX
 */

STATIC int
_field_th_ingress_entry_qual_tcam_install(int unit, _field_entry_t *f_ent,
                                          soc_mem_t tcam_mem, int tcam_idx)
{
    uint32         entry_buf[SOC_MAX_MEM_FIELD_WORDS] = {0};
                              /* Buffer to fill Policy & TCAM entry.*/
    _field_stage_t *stage_fc; /* Reference to Stage control structure. */
    _field_tcam_t  *tcam;
    _field_group_t *fg;
    int            rv;
    uint32         valid;
    uint8          mode;

    /* Input parameters check. */
    if (NULL == f_ent) {
        return (BCM_E_PARAM);
    }

    tcam = &f_ent->tcam;
    fg = f_ent->group;
    if (tcam == NULL || fg == NULL) {
       return BCM_E_INTERNAL;
    }

    /* Determine the slice mode based on group flags. */
    mode = _BCM_FIELD_IS_SLICE_MODE_PBMP_SUPPORTED(fg->flags) ?
            _IFP_SLICE_MODE_WIDE : _IFP_SLICE_MODE_NARROW;

    BCM_IF_ERROR_RETURN
       (_field_stage_control_get(unit, _BCM_FIELD_STAGE_INGRESS, &stage_fc));

    /*
     * When the TCAM Mode is Wide on a global view,
     * Install the entry on the Pipes to support the PBMP.
     */
    if (BCM_FIELD_QSET_TEST(f_ent->group->qset, bcmFieldQualifyInPorts) &&
        (mode == _IFP_SLICE_MODE_WIDE) &&
        (stage_fc->oper_mode == bcmFieldGroupOperModeGlobal)) {
          int             pbm_len;                  /* Length of the field */
          _field_pbmp_t   _f_pipe_pbmp[_FP_MAX_NUM_PIPES];

          pbm_len = soc_mem_field_length(unit, IFP_TCAM_WIDE_PIPE0m, IPBMf);
          if (pbm_len == 0) {
              LOG_ERROR(BSL_LS_BCM_FP, (BSL_META("Invalid length,"
                    " Failed to write PBMP on IFP_TCAM Pipes.\n\r")));
              return BCM_E_INTERNAL;
          }

          sal_memset(&_f_pipe_pbmp, 0x0,
                     sizeof(_field_pbmp_t)*_FP_MAX_NUM_PIPES);
          rv = _field_ingress_pipe_pbmp_get(unit, &f_ent->pbmp, pbm_len,
                                            _FP_MAX_NUM_PIPES, _f_pipe_pbmp);
          if (BCM_FAILURE(rv)) {
             LOG_ERROR(BSL_LS_BCM_FP, (BSL_META("Failed to retrieve Pipe PBMP,"
                    " Failed to write PBMP on IFP_TCAM Pipes.\n\r")));
             return BCM_E_INTERNAL;
          }

          rv = _field_ingress_entry_tcam_wide_install(unit, f_ent, tcam_idx,
                                                      _f_pipe_pbmp,
                                                      _FP_MAX_NUM_PIPES);
          if (BCM_FAILURE(rv)) {
             LOG_ERROR(BSL_LS_BCM_FP, (BSL_META(
                      " Failed to write PBMP on IFP_TCAM Pipes.\n\r")));
             return BCM_E_INTERNAL;
          }

          return (BCM_E_NONE);
    }

    BCM_IF_ERROR_RETURN(soc_mem_read(unit,
                                     tcam_mem,
                                     MEM_BLOCK_ANY,
                                     tcam_idx,
                                     entry_buf));
    soc_mem_field_set(unit, tcam_mem, entry_buf, KEYf, tcam->key);
    soc_mem_field_set(unit, tcam_mem, entry_buf, MASKf, tcam->mask);

#if 0
    if ((BCM_FIELD_QSET_TEST(f_ent->group->qset, bcmFieldQualifyInPorts)) &&
        (mode == _IFP_SLICE_MODE_WIDE)) {
        soc_mem_field_set(unit, tcam_mem, entry_buf,
                          IPBMf, (uint32 *)&(f_ent->pbmp.data));
        soc_mem_field_width_fit_set(unit, tcam_mem, entry_buf,
                           IPBM_MASKf, (uint32 *)&(f_ent->pbmp.mask));
    }
#endif
    /* Update the valid bit based on the mode and group flags */
    valid = (fg->flags & _FP_GROUP_LOOKUP_ENABLED) ?
              ((mode == _IFP_SLICE_MODE_WIDE) ? 3 : 1) : 0;
    soc_mem_field32_set(unit, tcam_mem, entry_buf, VALIDf, valid);

    BCM_IF_ERROR_RETURN(soc_mem_write(unit,
                                      tcam_mem,
                                      MEM_BLOCK_ALL,
                                      tcam_idx,
                                      entry_buf));
   return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_field_th_tcam_policy_mem_get
 *
 * Purpose:
 *     Get tcam & policy memories for a specific chip pipeline stage.
 *
 * Parameters:
 *     unit       -  (IN)  BCM device number.
 *     f_ent     -   (IN)  Field entry.
 *     tcam_mem   -  (OUT) TCAM Memory id.
 *     policy_mem -  (OUT) Policy memory id.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_th_tcam_policy_mem_get(int unit, 
                                  _field_entry_t *f_ent,
                                  soc_mem_t *tcam_mem,
                                  soc_mem_t *policy_mem)
{
    int   mode;     /* IFP_TCAM mode */
    int   instance; /* Pipe Instance */
    soc_mem_t fp_tcam_wide[_FP_MAX_NUM_PIPES] = /* IFP TCAM memory names. */
        {
            IFP_TCAM_WIDE_PIPE0m,
            IFP_TCAM_WIDE_PIPE1m,
            IFP_TCAM_WIDE_PIPE2m,
            IFP_TCAM_WIDE_PIPE3m
        };
    soc_mem_t fp_tcam_narrow[_FP_MAX_NUM_PIPES] = /* IFP TCAM memory names. */
        {
            IFP_TCAM_PIPE0m,
            IFP_TCAM_PIPE1m,
            IFP_TCAM_PIPE2m,
            IFP_TCAM_PIPE3m
        };
    soc_mem_t fp_policy_table[_FP_MAX_NUM_PIPES] = /* IFP POLICY table
                                                      memory names. */
        {
            IFP_POLICY_TABLE_PIPE0m,
            IFP_POLICY_TABLE_PIPE1m,
            IFP_POLICY_TABLE_PIPE2m,
            IFP_POLICY_TABLE_PIPE3m,
        };
    _field_stage_t *stage_fc; /* Reference to Stage control structure. */

    /* Input parameters check. */
    if ((NULL == f_ent) || (NULL == tcam_mem) || (NULL == policy_mem)) {
        return (BCM_E_PARAM);
    }

    if (NULL == f_ent->group) {
        return (BCM_E_INTERNAL);
    }

    BCM_IF_ERROR_RETURN
      (_field_stage_control_get (unit, f_ent->group->stage_id, &stage_fc));

    instance = f_ent->group->instance;

    switch (f_ent->group->stage_id) {
       case _BCM_FIELD_STAGE_INGRESS:

            mode = _BCM_FIELD_IS_SLICE_MODE_PBMP_SUPPORTED(f_ent->group->flags)?
                   _IFP_SLICE_MODE_WIDE : _IFP_SLICE_MODE_NARROW;
            if (stage_fc->oper_mode == bcmFieldGroupOperModeGlobal) {
               *tcam_mem = (mode == _IFP_SLICE_MODE_WIDE) ?
                            IFP_TCAM_WIDEm: IFP_TCAMm;
               *policy_mem = IFP_POLICY_TABLEm;
            } else {
               *tcam_mem = (mode == _IFP_SLICE_MODE_WIDE) ?
                            fp_tcam_wide[instance]:fp_tcam_narrow[instance];
               *policy_mem = fp_policy_table[instance];
            }
            break;

       case _BCM_FIELD_STAGE_LOOKUP:
            *tcam_mem = VFP_TCAMm;
            *policy_mem = VFP_POLICY_TABLEm;
            break;

       case _BCM_FIELD_STAGE_EGRESS:
            *tcam_mem = EFP_TCAMm;
            *policy_mem = EFP_POLICY_TABLEm;
            break;

       default:
            *tcam_mem = *policy_mem = INVALIDm;
            return BCM_E_PARAM;
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_th_default_policer_set
 *
 * Purpose:
 *     Get metering portion of Policy Table.
 *
 * Parameters:
 *     unit      - (IN)BCM device number.
 *     stage_fc  - (IN)Stage control structure.
 *     level     - (IN)Policer level.
 *     f_ent     - (IN)Software entry structure to get tcam info from.
 *     buf       - (IN/OUT)Hardware policy entry
 *
 * Returns:
 *     BCM_E_NONE  - Success
 *
 */

STATIC int
_field_th_default_policer_set(int unit, _field_stage_t *stage_fc,
                               int level, _field_entry_t *f_ent, uint32 *buf)
{
    _bcm_field_action_offset_t  meter_info_offset; /* Meter Set for basic Meter info. */

    /* Input parameter check. */
    if ((NULL == stage_fc) || (NULL == buf))  {
        return (BCM_E_PARAM);
    }

      sal_memset(&meter_info_offset, 0x0, sizeof(meter_info_offset));

      /* METER_PAIR_MODE */
       meter_info_offset.offset[1] = 266;
       meter_info_offset.width[1]  = 3;
       meter_info_offset.value[1]  = 0;

       /* METER_PAIR_MODE_MODIFIER */
       meter_info_offset.offset[2] = 265;
       meter_info_offset.width[2]  = 1;
       meter_info_offset.value[2]  = 1;

       ACTION_SET(unit, f_ent, buf, &meter_info_offset);

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_th_ingress_policer_action_set
 *
 * Purpose:
 *     Get metering portion of Policy Table.
 *
 * Parameters:
 *     unit      - (IN)BCM device number.
 *     f_ent     - (IN)Software entry structure to get tcam info from.
 *     buf       - (IN/OUT)Hardware policy entry
 *
 * Returns:
 *     BCM_E_NONE  - Success
 *
 */
int
_field_th_ingress_policer_action_set(int unit, _field_entry_t *f_ent,
                                     uint32 *buf)
{
    _field_entry_policer_t      *f_ent_pl;  /* Field entry policer descriptor.*/
    _field_stage_t              *stage_fc;  /* Stage field control structure. */
    _field_policer_t            *f_pl;      /* Field policer descriptor.      */
    int                         idx;        /* Policers levels iterator.      */
    uint32                      meter_pair_mode = BCM_FIELD_METER_MODE_DEFAULT;
                                            /* Hw meter usage bits.      */
    int                         meter_pair_idx;  /* Meter pair index.         */
    int                         rv;              /* Operation return status.  */
    _bcm_field_action_offset_t  meter_offset;    /* Meter Set for ODD_EVEN    */
    _bcm_field_action_offset_t  meter_info_offset;/* Meter Set for basic info */
    int                         instance;    /*  Current Pipe instance        */

    /* Input parameter check. */
    if ((NULL == f_ent) || (NULL == buf))  {
        return (BCM_E_PARAM);
    }
    if (NULL == f_ent->group) {
        return (BCM_E_PARAM);
    }

    /* Get stage control structure. */
    rv = _field_stage_control_get(unit, f_ent->group->stage_id, &stage_fc);
    BCM_IF_ERROR_RETURN(rv);

    /* Get the current pipe instance from group */
    instance = f_ent->group->instance;

    for (idx = 0; idx < _FP_POLICER_LEVEL_COUNT; idx++) {
        f_ent_pl = f_ent->policer + idx;

        if ((idx > 0)) {
            /* Trident device does not support Level1 policers */
            break;
        }

    if (0 == (f_ent_pl->flags & _FP_POLICER_INSTALLED)) {
            /* Install preserve the color policer. */
            rv = _field_th_default_policer_set(unit, stage_fc, idx,
                                                f_ent, buf);
            BCM_IF_ERROR_RETURN(rv);
            continue;
        }

        /* Get policer config. */
        rv = _bcm_field_policer_get(unit, f_ent_pl->pid, &f_pl);
        BCM_IF_ERROR_RETURN(rv);

        if (0 == f_pl->level) {
            /* Get hw encoding for meter mode. */
            rv = _bcm_field_meter_pair_mode_get(unit, f_pl, &meter_pair_mode);
            BCM_IF_ERROR_RETURN(rv);
            /* If level 0 policer is Modified trTcm ->
             * meter sharing mode is dual.
             */
        }


           /* Pair index is (Pool number * Pairs in Pool + Pair number) */
            meter_pair_idx
                = (f_pl->pool_index
                    * (stage_fc->meter_pool[instance][f_pl->pool_index]->num_meter_pairs))
                    + (f_pl->hw_index);

                 sal_memset(&meter_info_offset, 0x0, sizeof(meter_info_offset));
                 sal_memset(&meter_offset, 0x0, sizeof(meter_offset));

                 /* METER_PAIR_INDEX */
                 meter_info_offset.offset[0] = 269;
                 meter_info_offset.width[0]  = 10;
                 meter_info_offset.value[0]  = meter_pair_idx;

                 /* METER_PAIR_MODE */
                 meter_info_offset.offset[1] = 266;
                 meter_info_offset.width[1]  = 3;
                 meter_info_offset.value[1]  = meter_pair_mode;

                 /*
                 * Flow mode is the only one that cares about the test and
                 * update bits.
                 */
                if (_FP_POLICER_EXCESS_HW_METER(f_pl)) {
                    /* Excess meter - even index. */

                    /* METER_TEST_ODD */
                    meter_offset.offset[0] = 282;
                    meter_offset.width[0]  = 1;
                    meter_offset.value[0]  = 0;

                     /* METER_TEST_EVEN */
                    meter_offset.offset[1] = 280;
                    meter_offset.width[1]  = 1;
                    meter_offset.value[1]  = 1;

                    /* METER_UPDATE_ODD */
                    meter_offset.offset[2] = 281;
                    meter_offset.width[2]  = 1;
                    meter_offset.value[2]  = 0;

                    /* METER_UPDATE_EVEN */
                    meter_offset.offset[3] = 279;
                    meter_offset.width[3]  = 1;
                    meter_offset.value[3]  = 1;

                } else if (_FP_POLICER_COMMITTED_HW_METER(f_pl)) {
                    /* Committed meter - odd index. */

                    /* METER_TEST_ODD */
                    meter_offset.offset[0] = 282;
                    meter_offset.width[0]  = 1;
                    meter_offset.value[0]  = 1;

                     /* METER_TEST_EVEN */
                    meter_offset.offset[1] = 280;
                    meter_offset.width[1]  = 1;
                    meter_offset.value[1]  = 0;

                    /* METER_UPDATE_ODD */
                    meter_offset.offset[2] = 281;
                    meter_offset.width[2]  = 1;
                    meter_offset.value[2]  = 1;

                    /* METER_UPDATE_EVEN */
                    meter_offset.offset[3] = 279;
                    meter_offset.width[3]  = 1;
                    meter_offset.value[3]  = 0;

                }

                if ((f_pl->cfg.mode == bcmPolicerModePassThrough) ||
                    (f_pl->cfg.mode == bcmPolicerModeSrTcmModified)) {

                    /* METER_PAIR_MODE_MODIFIER */
                    meter_info_offset.offset[2] = 265;
                    meter_info_offset.width[2]  = 1;
                    meter_info_offset.value[2]  = 1;
                }

                ACTION_SET(unit, f_ent, buf, &meter_offset);
                ACTION_SET(unit, f_ent, buf, &meter_info_offset);
    }

   return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_field_th_policer_action_set
 *
 * Purpose:
 *     Get metering portion of Policy Table.
 *
 * Parameters:
 *     unit      - (IN)BCM device number.
 *     f_ent     - (IN)Software entry structure to get tcam info from.
 *     mem       - (IN)Policy table memory.
 *     buf       - (IN/OUT)Hardware policy entry
 *
 * Returns:
 *     BCM_E_NONE  - Success
 *
 */
int
_bcm_field_th_policer_action_set(int unit, _field_entry_t *f_ent,
                                  soc_mem_t mem, uint32 *buf)
{
    int rv;

    if (f_ent->group->stage_id == _BCM_FIELD_STAGE_INGRESS) {
        rv = _field_th_ingress_policer_action_set(unit, f_ent, buf);
    } else {
        rv = _bcm_field_trx_policer_action_set(unit, f_ent, mem, buf);
    }
    return rv;
}

/*
 * Function:
 *     _field_th_entry_policy_mem_install
 * Purpose:
 *     Install the entry actions into the policy table.
 * Parameters:
 *     unit         - (IN) BCM device number
 *     f_ent        - (IN) entry structure to get policy info from
 *     policy_mem   - (IN) Policy table memory
 *     tcam_idx     - (IN) Common index of various tables
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_entry_policy_mem_install(int unit, _field_entry_t *f_ent,
                                   soc_mem_t policy_mem, int tcam_idx)
{
    uint32            e_buf[SOC_MAX_MEM_FIELD_WORDS] = {0};
                                       /* Buffer to fill Policy entry.        */
    _field_action_t   *fa = NULL;      /* Reference to field action structure */
    int               rv;              /* Operation return status.            */
    int               policy_idx;      /* Policy index in the Policy table.   */

    /* Input parameters check. */
    if (NULL == f_ent) {
        return (BCM_E_PARAM);
    }


    /* (Intra Slice) Double Wide/ Triple wide modes, IFP_TCAM_WIDE have 256 entries
     * but IFP_POLICY_TABLE will have 512 entries.
     */
    if ((!(f_ent->group->flags & _FP_GROUP_SPAN_SINGLE_SLICE)
         || (f_ent->group->flags & _FP_GROUP_INTRASLICE_DOUBLEWIDE))) {
        policy_idx = tcam_idx + (f_ent->fs->slice_number * 256);
    } else {
       policy_idx = tcam_idx;
    }


    /* Extract the policy info from the entry structure. */
    for (fa = f_ent->actions; fa != NULL; fa = fa->next) {
        /* Skip the invalid entries */
        if (_FP_ACTION_VALID & fa->flags) {
            rv = _bcm_field_th_action_set(unit, policy_mem, f_ent,
                                          policy_idx, fa, e_buf);
            if (BCM_FAILURE(rv)) {
               LOG_ERROR(BSL_LS_BCM_FP, (BSL_META("TH Action set failed.\n")));
               return rv;
            }
        }
    }

    /*
     * Handle color dependence/independence
     * (1 bit field in GREEN_TO_PID_SET)
     */
    {
       _bcm_field_action_offset_t  a_offset;
       sal_memset(&a_offset, 0x0, sizeof(a_offset));
       a_offset.offset[0] = 191;
       a_offset.width[0]  = 1;
       a_offset.value[0]  = ((f_ent->flags & _FP_ENTRY_COLOR_INDEPENDENT)
                              ? 1 : 0);
       ACTION_SET(unit, f_ent, e_buf, &a_offset);
    }

    /* Extract meter related policy fields */
    BCM_IF_ERROR_RETURN
        (_bcm_field_th_policer_action_set(unit, f_ent, policy_mem, e_buf));

    /* Extract counter related policy fields */
    BCM_IF_ERROR_RETURN
        (_bcm_field_th_stat_action_set(unit, f_ent, policy_mem,
                                        policy_idx, e_buf));

    /* Write the POLICY Table */
    rv = soc_mem_write(unit, policy_mem, MEM_BLOCK_ALL, policy_idx, e_buf);
    return (rv);
}

/*
 * Function:
 *     _field_th_entry_qual_tcam_install
 * Purpose:
 *     Install the entry qualifiers in the TCAM.
 * Parameters:
 *     unit         - (IN) BCM device number
 *     f_ent        - (IN) entry structure to get policy info from
 *     tcam_mem     - (IN) TCAM memory index.
 *     tcam_idx     - (IN) Index in the TCAM memory.
 * Returns:
 *     BCM_E_XXX
 */

STATIC int
_field_th_entry_qual_tcam_install(int unit, _field_entry_t *f_ent,
                                  soc_mem_t tcam_mem, int tcam_idx)
{
    /* Input parameters check. */
    if (NULL == f_ent) {
        return (BCM_E_PARAM);
    }

    switch (f_ent->group->stage_id) {
        case _BCM_FIELD_STAGE_INGRESS:
            return
              _field_th_ingress_entry_qual_tcam_install(unit, f_ent,
                                                        tcam_mem, tcam_idx);

        case _BCM_FIELD_STAGE_EGRESS:
        case _BCM_FIELD_STAGE_LOOKUP:
         /* Need to update per-pipe support for other stages.*/
        default:
            return (BCM_E_INTERNAL);
    }

    return (BCM_E_NONE);
}


/*
 * Function:
 *     _bcm_field_th_entry_install
 * Purpose:
 *     Write entry into the device hardware TCAM memory.
 * Parameters:
 *     unit     -   (IN) BCM Device Number
 *     f_ent    -   (IN) Physical entry structure to be installed
 *     tcam_idx -   (IN) common index of various tables
 * Returns:
 *     BCM_E_XXX        On TCAM read/write errors
 *     BCM_E_NONE
 * Note:
 *     Unit lock should be held by calling function.
 */
int
_bcm_field_th_entry_install(int unit, _field_entry_t *f_ent, int tcam_idx)
{
    int              rv = BCM_E_INTERNAL; /* Return Value            */
    soc_mem_t        tcam_mem;            /* TCAM memory id.         */
    soc_mem_t        policy_mem;          /* Policy table memory id. */

    /* Input parameters check. */
    if (NULL == f_ent) {
        return (BCM_E_PARAM);
    }

    if (NULL == f_ent->fs) {
        return (BCM_E_PARAM);
    }

    /*
     * Invoke common Install function for the stages other than Ingress.
     * Need to re-visit for per-pipe support for other stages.
     */
    if (f_ent->group->stage_id != _BCM_FIELD_STAGE_INGRESS) {
       return _bcm_field_tr_entry_install(unit, f_ent, tcam_idx);
    }

    rv = _bcm_field_th_tcam_policy_mem_get(unit, f_ent,
                                       &tcam_mem, &policy_mem);
    BCM_IF_ERROR_RETURN(rv);

    if ((tcam_idx < soc_mem_index_min(unit, tcam_mem)) ||
        (tcam_idx > soc_mem_index_max(unit, tcam_mem))) {
        return (BCM_E_PARAM);
    }

    /* Write actions into Policy Table */
    rv = _field_th_entry_policy_mem_install(unit, f_ent, policy_mem, tcam_idx);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_FP, (BSL_META(
                 "Failed to Install Policy Table for Entry[%d]"
                 " tcam_idx:[%d].\n\r"),
                 f_ent->eid, tcam_idx));
        return (rv);
    }

    /* Write qualifiers into TCAM */
    rv = (_field_th_entry_qual_tcam_install(unit, f_ent, tcam_mem, tcam_idx));

    /* Enable the Slice if it is the first Entry */
    if (BCM_SUCCESS(rv)) {
       if ((f_ent->fs->slice_flags & _BCM_FIELD_SLICE_HW_ENABLE) == 0)  {
          if (BCM_FAILURE(_bcm_field_th_slice_enable_set(unit,
                                                f_ent->group, f_ent->fs, 1))) {
             LOG_ERROR(BSL_LS_BCM_FP, (BSL_META(
                      "Failed to Enable Field Slice[%d] for Entry[%d].\n\r"),
                      f_ent->fs->slice_number, f_ent->eid));
          }
          f_ent->fs->slice_flags |= _BCM_FIELD_SLICE_HW_ENABLE;
       }
       /* Increment the HW Slice entry count */
       f_ent->fs->hw_ent_count++;
    } else {
       LOG_ERROR(BSL_LS_BCM_FP, (BSL_META(
                 "Failed to Install the TCAM Entry[%d] tcam_idx:[%d].\n\r"),
                 f_ent->eid, tcam_idx));
    }

    LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
        "==> %s(): Entry[%d] Installed - mem:%d tcam_idx:%d slice:%d\n\r"),
         __func__, f_ent->eid, tcam_mem, tcam_idx, f_ent->fs->slice_number));

    return rv;
}

/*
 * Function:
 *     _bcm_field_th_entry_reinstall
 * Purpose:
 *     Write entry into the chip's memory.
 * Parameters:
 *     unit     -   BCM Unit
 *     f_ent    -   Physical entry structure to be installed
 *     tcam_idx - common index of various tables
 * Returns:
 *     BCM_E_XXX        On TCAM read/write errors
 *     BCM_E_NONE
 * Note:
 *     Unit lock should be held by calling function.
 */
int
_bcm_field_th_entry_reinstall(int unit, _field_entry_t *f_ent, int tcam_idx)
{
    int              rv = BCM_E_INTERNAL; /* Return Value */
    soc_mem_t        tcam_mem;          /* TCAM memory id.                    */
    soc_mem_t        policy_mem;        /* Policy table memory id .           */

    /* Input parameters check. */
    if (NULL == f_ent) {
       return (BCM_E_PARAM);
    }

    if (NULL == f_ent->fs) {
       return (BCM_E_PARAM);
    }

    /*
     * Invoke common Install function for the stages other than Ingress.
     * Need to re-visit for per-pipe support for other stages.
     */
    if (f_ent->group->stage_id != _BCM_FIELD_STAGE_INGRESS) {
       return _bcm_field_tr_entry_reinstall(unit, f_ent, tcam_idx);
    }

    rv = _bcm_field_th_tcam_policy_mem_get(unit, f_ent,
                                       &tcam_mem, &policy_mem);
    BCM_IF_ERROR_RETURN(rv);

    if ((tcam_idx < soc_mem_index_min(unit, tcam_mem)) ||
        (tcam_idx > soc_mem_index_max(unit, tcam_mem))) {
        return (BCM_E_PARAM);
    }

    /* Write actions into Policy Table */
    rv = _field_th_entry_policy_mem_install(unit, f_ent, policy_mem, tcam_idx);
    return rv;
}

/*
 * Function:
 *     _field_th_ingress_qual_tcam_key_mask_get
 * Purpose:
 *     Allocate & read from hw ingress tcam key/mask image.
 * Parameters:
 *     unit  - (IN)     BCM device number.
 *     f_ent - (IN/OUT) Field entry.
 *     tcam  - (IN)     Reference to Entry TCAM structure.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_ingress_qual_tcam_key_mask_get(int unit,
                                          _field_entry_t *f_ent,
                                          _field_tcam_t *tcam)
{
    int             rv = BCM_E_INTERNAL; /* Return Value */
    uint32          tcam_key_words;      /* tcam key length */
    soc_mem_t       tcam_mem;            /* TCAM Memory name */
    soc_mem_t       policy_mem;          /* Policy Memory name */


    /* Validate input parameters */
    if (NULL == f_ent || NULL == tcam) {
       return BCM_E_PARAM;
    }

    /* Validate the group */
    if (NULL == f_ent->fs || NULL == f_ent->group) {
       return BCM_E_INTERNAL;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_field_th_tcam_policy_mem_get(unit, f_ent,
                                       &tcam_mem, &policy_mem));

    tcam_key_words = BITS2WORDS(soc_mem_field_length(unit,
                                                     tcam_mem,
                                                     KEYf));
    /* Retrieve the key size */
    tcam->key_size = WORDS2BYTES(tcam_key_words);

    /* Allocate Memory for TCAM Key and Mask */
    _FP_XGS3_ALLOC(tcam->key, tcam->key_size, "IFP_TCAM Key Alloc.");
    _FP_XGS3_ALLOC(tcam->mask, tcam->key_size, "IFP_TCAM Mask Alloc.");
    if (tcam->key == NULL || tcam->mask == NULL) {
        rv = BCM_E_MEMORY;
        goto error;
    }

    if (f_ent->flags & _FP_ENTRY_INSTALLED) {
        int    tcam_idx;
        uint32 tcam_entry[SOC_MAX_MEM_FIELD_WORDS] = {0};

        rv = _bcm_field_entry_tcam_idx_get(unit, f_ent, &tcam_idx);
        if (BCM_FAILURE(rv)) {
           goto error;
        }

        rv = soc_mem_read(unit,
                          tcam_mem,
                          MEM_BLOCK_ANY,
                          tcam_idx,
                          tcam_entry);
        if (BCM_FAILURE(rv)) {
            goto error;
        }
        soc_mem_field_get(unit, tcam_mem, tcam_entry, KEYf, tcam->key);
        soc_mem_field_get(unit, tcam_mem, tcam_entry, MASKf, tcam->mask);
    }

    return (BCM_E_NONE);

 error:
    if (tcam->key) {
        sal_free(tcam->key);
        tcam->key = NULL;
    }
    if (tcam->mask) {
        sal_free(tcam->mask);
        tcam->mask = NULL;
    }

    return rv;
}

/*
 * Function:
 *     _bcm_field_qual_tcam_key_mask_get
 * Purpose:
 *     Allocate & read from hw tcam key/mask image.
 * Parameters:
 *     unit  - (IN)     BCM device number.
 *     f_ent - (IN/OUT) Field entry.
 *     tcam  - (IN)     Reference to Entry TCAM structure.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_th_qual_tcam_key_mask_get(int unit,
                                     _field_entry_t *f_ent,
                                     _field_tcam_t  *tcam)
{
    int rv = BCM_E_NONE;  /* Return value */

    /* Validate input parameters */
    if (NULL == f_ent || NULL == tcam) {
       return BCM_E_PARAM;
    }

    switch (f_ent->group->stage_id) {
       case _BCM_FIELD_STAGE_INGRESS:
          rv = _field_th_ingress_qual_tcam_key_mask_get(unit, f_ent, tcam);
          break;
       case _BCM_FIELD_STAGE_LOOKUP:
       case _BCM_FIELD_STAGE_EGRESS:
          rv = _field_qual_tcam_key_mask_get(unit, f_ent, tcam, 0);
          break;
       default:
          rv = BCM_E_INTERNAL;
          break;
    }

    return rv;
}

/*
 * Function:
 *     _field_th_tcam_policy_clear
 *
 * Purpose:
 *     Writes a null entry into the TCAM plus Policy table.
 *
 * Parameters:
 *     unit      -   (IN) BCM device number.
 *     f_ent     -   (IN) Field entry.
 *     tcam_idx  -   (IN) Entry tcam index.
 *
 * Returns:
 *     BCM_E_XXX
 *
 * Note:
 *     Unit lock should be held by calling function.
 */
STATIC int
_field_th_tcam_policy_clear(int unit, _field_entry_t *f_ent, int tcam_idx)
{
    soc_mem_t      tcam_mem;            /* TCAM memory id.                */
    soc_mem_t      policy_mem;          /* Policy table memory id .       */

    _field_stage_t *stage_fc;           /* Stage field control structure. */
    int              num_tcams = 0;     /* Number of TCAMs                    */
    int              ct;
    soc_mem_t        tcam_mems[_FP_MAX_NUM_PIPES] =
                                        /* IFP TCAM memory names. */
                   {
                     IFP_TCAM_WIDE_PIPE0m,
                     IFP_TCAM_WIDE_PIPE1m,
                     IFP_TCAM_WIDE_PIPE2m,
                     IFP_TCAM_WIDE_PIPE3m
                   };
    int policy_idx;                     /* Policy index in the Policy table.   */ 

    /* Validate input parameters */
    if (NULL == f_ent) {
       return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_field_th_tcam_policy_mem_get(unit, f_ent, &tcam_mem, &policy_mem));

    /* Get field stage control . */
    BCM_IF_ERROR_RETURN
        (_field_stage_control_get(unit, f_ent->group->stage_id, &stage_fc));

    /*
     * When the TCAM Mode is Wide on a global view,
     * Clear entries from the Pipes.
     */
    if ((BCM_FIELD_QSET_TEST(f_ent->group->qset, bcmFieldQualifyInPorts)) &&
        (tcam_mem == IFP_TCAM_WIDEm) &&
        (stage_fc->oper_mode == bcmFieldGroupOperModeGlobal)) {
       num_tcams = 4;
    } else {
       num_tcams = 1;
       tcam_mems[0] = tcam_mem;
    }

    for (ct = 0; ct < num_tcams; ct++) {
       tcam_mem = tcam_mems[ct];

       /* Memory index sanity check. */
       if (tcam_idx > soc_mem_index_max(unit, tcam_mem)) {
          return (BCM_E_PARAM);
       }

       /* Write the TCAM & Policy Tables */
       SOC_IF_ERROR_RETURN
           (soc_mem_write(unit, tcam_mem, MEM_BLOCK_ALL, tcam_idx,
                          soc_mem_entry_null(unit, tcam_mem)));

       if (ct == 0) {
        /* (Intra Slice) Double Wide/ Triple wide modes, IFP_TCAM_WIDE have 256 entries
         * but IFP_POLICY_TABLE will have 512 entries.
         */
        if ((!(f_ent->group->flags & _FP_GROUP_SPAN_SINGLE_SLICE)
             || (f_ent->group->flags & _FP_GROUP_INTRASLICE_DOUBLEWIDE))) {
            policy_idx = tcam_idx + (f_ent->fs->slice_number * 256);
        } else {
            policy_idx = tcam_idx;
        }
          SOC_IF_ERROR_RETURN
             (soc_mem_write(unit, policy_mem, MEM_BLOCK_ALL, policy_idx,
                            soc_mem_entry_null(unit, policy_mem)));
       }
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_field_th_entry_move
 * Purpose:
 *     Copy an entry from one TCAM index to another. It copies the values in
 *     hardware from the old index to the new index.
 *     IT IS ASSUMED THAT THE NEW INDEX IS EMPTY (VALIDf=00) IN HARDWARE.
 *     The old Hardware index is cleared at the end.
 * Parameters:
 *     unit           - (IN) BCM device number.
 *     f_ent          - (IN) Entry to move
 *     parts_count    - (IN) Field entry parts count.
 *     tcam_idx_old   - (IN) Source entry tcam index.
 *     tcam_idx_new   - (IN) Destination entry tcam index.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_th_entry_move(int unit, _field_entry_t *f_ent, int parts_count,
                         int *tcam_idx_old, int *tcam_idx_new)
{
    uint32  entry_buf[SOC_MAX_MEM_FIELD_WORDS];
                                /* Buffer to zero TCAM & Policy entry.*/
    uint32  policy_buf[_FP_MAX_ENTRY_WIDTH][SOC_MAX_MEM_FIELD_WORDS];
                                /* For policies */
    soc_mem_t tcam_mem;                 /* TCAM memory id.                    */
    soc_mem_t policy_mem;               /* Policy table memory id .           */
    int tcam_idx_max;                   /* TCAM memory max index.             */
    int tcam_idx_min;                   /* TCAM memory min index.             */
    _field_stage_t *stage_fc;           /* Stage field control structure.     */
    _field_stage_id_t stage_id;         /* Field pipeline stage id.           */
    int idx;                            /* Iteration index.                   */
    _field_group_t   *fg;               /* Field group structure.             */
    int              rv;                /* Operation return status.           */
    int              num_tcams = 0;     /* Number of TCAMs                    */
    int              ct;
    int              policy_idx;
    soc_mem_t        tcam_mems[_FP_MAX_NUM_PIPES] =
                                     /* IFP TCAM memory names. */
                   {
                     IFP_TCAM_WIDE_PIPE0m,
                     IFP_TCAM_WIDE_PIPE1m,
                     IFP_TCAM_WIDE_PIPE2m,
                     IFP_TCAM_WIDE_PIPE3m
                   };

    /* Input parameters check. */
    if ((NULL == f_ent) || (NULL == tcam_idx_old) || (NULL == tcam_idx_new)) {
       return (BCM_E_PARAM);
    }

    /*
     * Invoke common function for the stages other than Ingress
     * Need to re-visit for per-pipe support for other stages.
     */
    if (f_ent->group->stage_id != _BCM_FIELD_STAGE_INGRESS) {
       return _bcm_field_td2_entry_move(unit, f_ent, parts_count,
                                            tcam_idx_old, tcam_idx_new);
    }

    fg = f_ent->group;
    if (fg == NULL) {
       return BCM_E_INTERNAL;
    }

    /* Get field stage control . */
    stage_id = fg->stage_id;
    rv = _field_stage_control_get(unit, stage_id, &stage_fc);
    BCM_IF_ERROR_RETURN(rv);

    /* Get entry tcam and actions. */
    BCM_IF_ERROR_RETURN
         (_bcm_field_th_tcam_policy_mem_get(unit, f_ent,
                                       &tcam_mem, &policy_mem));

    /*
     * When the TCAM Mode is Wide on a global view,
     * Move entries from the Pipes.
     */
    if ((BCM_FIELD_QSET_TEST(fg->qset, bcmFieldQualifyInPorts)) &&
        (tcam_mem == IFP_TCAM_WIDEm) &&
        (stage_fc->oper_mode == bcmFieldGroupOperModeGlobal)) {
       num_tcams = 4;
    } else {
       num_tcams = 1;
       tcam_mems[0] = tcam_mem;
    }

    for (ct = 0; ct < num_tcams; ct++) {
       tcam_mem = tcam_mems[ct];

       tcam_idx_max = soc_mem_index_max(unit, tcam_mem);
       tcam_idx_min = soc_mem_index_min(unit, tcam_mem);

       for (idx = 0; idx < parts_count; idx++) {
           /* Index sanity check. */
           if ((tcam_idx_old[idx] < tcam_idx_min) ||
               (tcam_idx_old[idx] > tcam_idx_max) ||
               (tcam_idx_new[idx] < tcam_idx_min) ||
               (tcam_idx_new[idx] > tcam_idx_max)) {
               LOG_DEBUG(BSL_LS_BCM_FP,
                         (BSL_META_U(unit,
                          "Invalid index range for _field_td2_entry_move\n"
                     "from %d to %d"), tcam_idx_old[idx], tcam_idx_new[idx]));
                return (BCM_E_PARAM);
           }
       }
      /*
       * Write entry to the destination
       * ORDER is important
       */
       for (idx = parts_count - 1; idx >= 0; idx--) {
           if (ct == 0) {

              /* (Intra Slice) Double Wide/ Triple wide modes, IFP_TCAM_WIDE have 256 entries
               * but IFP_POLICY_TABLE will have 512 entries.
               */
              if ((!(f_ent->group->flags & _FP_GROUP_SPAN_SINGLE_SLICE)
                  || (f_ent->group->flags & _FP_GROUP_INTRASLICE_DOUBLEWIDE))) {
                  policy_idx = tcam_idx_old[idx] + ((f_ent + idx)->fs->slice_number * 256);
              } else {
                  policy_idx = tcam_idx_old[idx];
              }

              /* Read policy entry from current tcam index. */
              rv = soc_mem_read(unit, policy_mem, MEM_BLOCK_ANY,
                                   policy_idx, policy_buf[idx]);

              BCM_IF_ERROR_RETURN(rv);

             /* (Intra Slice) Double Wide/ Triple wide modes, IFP_TCAM_WIDE have 256 entries
              *  but IFP_POLICY_TABLE will have 512 entries.
              */
              if ((!(f_ent->group->flags & _FP_GROUP_SPAN_SINGLE_SLICE)
                  || (f_ent->group->flags & _FP_GROUP_INTRASLICE_DOUBLEWIDE))) {
                  policy_idx = tcam_idx_new[idx] + ((f_ent + idx)->fs->slice_number * 256);
              } else {
                  policy_idx = tcam_idx_new[idx];
              }

              /* Write duplicate  policy entry to new tcam index. */
              rv = soc_mem_write(unit, policy_mem, MEM_BLOCK_ALL,
                                tcam_idx_new[idx], policy_buf[idx]);
              BCM_IF_ERROR_RETURN(rv);
           }

           /* Read tcam entry from current tcam index. */
           rv = soc_mem_read(unit, tcam_mem, MEM_BLOCK_ANY,
                             tcam_idx_old[idx], entry_buf);
           BCM_IF_ERROR_RETURN(rv);

           /* Write duplicate  tcam entry to new tcam index. */
           rv = soc_mem_write(unit, tcam_mem, MEM_BLOCK_ALL,
                              tcam_idx_new[idx], entry_buf);
           BCM_IF_ERROR_RETURN(rv);
       }
    }

    /*
     * Clear old location
     * ORDER is important
     */
     for (idx = 0; idx < parts_count; idx++) {
         rv = _field_th_tcam_policy_clear(unit, f_ent,
                                         tcam_idx_old[idx]);
         BCM_IF_ERROR_RETURN(rv);
     }

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_field_th_entry_remove
 * Purpose:
 *     Remove a previously installed physical entry.
 * Parameters:
 *     unit   - BCM device number
 *     f_ent  - Physical entry data
 * Returns:
 *     BCM_E_XXX
 *     BCM_E_NONE
 * Notes:
 *     FP unit lock should be held by calling function.
 */
int
_bcm_field_th_entry_remove(int unit, _field_entry_t *f_ent, int tcam_idx)
{
    int rv; /* Return value */

    if (NULL == f_ent) {
        return (BCM_E_PARAM);
    }

    if ((NULL == f_ent->fs) || (NULL == f_ent->group)) {
        return (BCM_E_INTERNAL);
    }

    /*
     * Invoke common function for the stages other than Ingress.
     * Need to re-visit for per-pipe support for other stages.
     */
    if (f_ent->group->stage_id != _BCM_FIELD_STAGE_INGRESS) {
       return _bcm_field_fb_entry_remove(unit, f_ent, tcam_idx);
    }

    rv = _field_th_tcam_policy_clear(unit, f_ent, tcam_idx);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_FP, (BSL_META(
                  "Failed to Remove the TCAM entry for entry[%d] tcam_idx[%d]."
                  "\n\r"),
                  f_ent->eid, tcam_idx));
        return rv;
    }

    f_ent->flags |= _FP_ENTRY_DIRTY; /* Mark entry as not installed */
    /* Decrement the HW Slice entry count */
    f_ent->fs->hw_ent_count--;
    /* Disable the Slice, if it the last entry */
    if (f_ent->fs->hw_ent_count == 0) {
       if (BCM_FAILURE
           (_bcm_field_th_slice_enable_set(unit, f_ent->group, f_ent->fs, 0))) {
             LOG_WARN(BSL_LS_BCM_FP, (BSL_META(
                      "Failed to Disable Field Slice[%d] for Entry[%d].\n\r"),
                      f_ent->fs->slice_number, f_ent->eid));
       }
       f_ent->fs->slice_flags &= ~_BCM_FIELD_SLICE_HW_ENABLE;
    }
    LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
        "==> %s(): Entry[%d] Un-installed - tcam_idx:%d slice:%d\n\r"), __func__,
         f_ent->eid, tcam_idx, f_ent->fs->slice_number));
    return (BCM_E_NONE);
}

/*
 * Function: _field_th_ingress_entry_enable_set
 *
 * Purpose:
 *     Enable/Disable an entry from the ingress field hw tables.
 *
 * Parameters:
 *     unit        - (IN) BCM device number
 *     f_ent       - (IN) Reference to Entry structure
 *     enable_flag - (IN) Flag to enable or disable
 *
 * Returns:
 *     BCM_E_XXX
 *
 * Notes:
 *     This does not destroy the entry, nor deallocate any related resources;
 *     it only enables/disables a rule from hardware table using VALIDf of the
 *     corresponding hardware entry. To deallocate the memory used by the entry
 *     call bcm_field_entry_destroy.
 */
int
_field_th_ingress_entry_enable_set(int unit, _field_entry_t *f_ent,
                                   int enable_flag)
{
    int                 rv;          /* Operation return status. */
    int                 tcam_idx;
    int                 valid = 0;
    soc_mem_t           tcam_mem = INVALIDm;
    soc_mem_t           policy_mem = INVALIDm;
    uint32              tcam_entry[SOC_MAX_MEM_FIELD_WORDS] = {0};

    if (NULL == f_ent) {
        return (BCM_E_PARAM);
    }

    if ((NULL == f_ent->fs) || (NULL == f_ent->group)) {
        return (BCM_E_INTERNAL);
    }

    /* Get tcam indexes for installed entry. */
    rv = _bcm_field_entry_tcam_idx_get(unit, f_ent,
                                       &tcam_idx);
    if (BCM_FAILURE(rv)) {
        return (rv);
    }

    rv = _bcm_field_th_tcam_policy_mem_get(unit, f_ent,
                                       &tcam_mem, &policy_mem);
    BCM_IF_ERROR_RETURN(rv);

    rv = soc_mem_read(unit, tcam_mem, MEM_BLOCK_ANY,
                      tcam_idx, tcam_entry);
    if (BCM_FAILURE(rv)) {
        return (rv);
    }

    if (enable_flag) {
       int  mode;

       mode = _BCM_FIELD_IS_SLICE_MODE_PBMP_SUPPORTED(f_ent->group->flags) ?
                            _IFP_SLICE_MODE_WIDE : _IFP_SLICE_MODE_NARROW;
       /* Update the valid bit based on the mode */
       valid = (mode == _IFP_SLICE_MODE_WIDE) ? 3 : 1;
    }

    soc_mem_field32_set(unit, tcam_mem, tcam_entry, VALIDf, valid);
    BCM_IF_ERROR_RETURN(soc_mem_write(unit,
                                      tcam_mem,
                                      MEM_BLOCK_ALL,
                                      tcam_idx,
                                      tcam_entry));

    if (enable_flag) {
       f_ent->flags |= _FP_ENTRY_ENABLED;
    }
    else {
       f_ent->flags &= ~_FP_ENTRY_ENABLED;
    }

    return BCM_E_NONE;
}

/*
 * Function: _bcm_field_th_entry_enable_set
 *
 * Purpose:
 *     Enable/Disable an entry from the hardware tables.
 *
 * Parameters:
 *     unit        - (IN) BCM device number
 *     f_ent       - (IN) Reference to Entry structure
 *     enable_flag - (IN) Flag to enable or disable
 *
 * Returns:
 *     BCM_E_XXX
 *
 * Notes:
 *     This does not destroy the entry, nor deallocate any related resources;
 *     it only enables/disables a rule from hardware table using VALIDf of the
 *     corresponding hardware entry. To deallocate the memory used by the entry
 *     call bcm_field_entry_destroy.
 */
int
_bcm_field_th_entry_enable_set(int unit, _field_entry_t *f_ent, int enable_flag)
{
    if (NULL == f_ent) {
        return (BCM_E_PARAM);
    }

    if ((NULL == f_ent->fs) || (NULL == f_ent->group)) {
        return (BCM_E_INTERNAL);
    }

    switch (f_ent->group->stage_id) {
       case _BCM_FIELD_STAGE_INGRESS:
           return _field_th_ingress_entry_enable_set(unit, f_ent, enable_flag);
           break;

       default:
          return BCM_E_INTERNAL;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _field_ser_oper_mode_init
 * Purpose:
 *      Initialize the SER for the given operational mode
 *      per-CAP stage.
 * Parameters:
 *      unit    - (IN) BCM Device number.
 *      stage   - (IN) Field Stage Pipeline value.
 *      mode    - (IN) Field Group Operational Mode enum value.
 * Returns:
 *      BCM_E_NONE   - Operation successful.
 *      BCM_E_PARAM  - Invalid operational mode or Field Stage.
 *      BCM_E_XXX    - For others.
 *
 * Notes:
 *   1. Valid "stage" input parameter values supported by
 *      this API are:
 *              "bcmFieldQualifyStageLookup"
 * 	        "bcmFieldQualifyStageIngress"
 *              "bcmFieldQualifyStageEgress"
 *              "bcmFieldQualifyStageExactMatch"
 *              "bcmFieldQualifyStageClass"
 *              "bcmFieldQualifyStageClassExactMatch".
 *   2. Irrespective of the given mode, the IFP_TCAM_WIDE is
 *      always be initialized to Unique Mode.
 */
STATIC int
_field_ser_oper_mode_init(
    int unit,
    int stage_id,
    bcm_field_group_oper_mode_t mode)
{
    int               idx;          /* Temp Index variable. */
    int               ser_mode;     /* SER mode */
    int               rv;           /* Return Value */
    int8              num_tcam = 0, /* Number of tcams */
                      ct;
    soc_mem_t         tcam_mem[12] = {INVALIDm};  /* Tcam memory id */
    int               set_mode[12] = {0};

   /* Validate input parameters */
   if ((mode != bcmFieldGroupOperModeGlobal) &&
       (mode != bcmFieldGroupOperModePipeLocal)) {
      return BCM_E_PARAM;
   }

   /* Update the SER mode */
   ser_mode = (mode == bcmFieldGroupOperModeGlobal) ?
               _SOC_SER_MEM_MODE_GLOBAL:_SOC_SER_MEM_MODE_PIPE_UNIQUE;

   /* Update stage_id and tcam memory for the given stage */
   switch (stage_id) {
     case _BCM_FIELD_STAGE_LOOKUP:
          tcam_mem[0] = VFP_TCAMm;
          tcam_mem[1] = FP_UDF_TCAMm;
          num_tcam = 2;
          break;
     case _BCM_FIELD_STAGE_INGRESS:
          tcam_mem[0] = FP_UDF_TCAMm;
          tcam_mem[1] = IFP_LOGICAL_TABLE_SELECTm;
          tcam_mem[2] = IFP_LOGICAL_TABLE_SELECT_TCAM_ONLYm;
          tcam_mem[3] = IFP_TCAMm;
          tcam_mem[4] = IFP_TCAM_WIDEm;
          num_tcam = 5;
          break;
     case _BCM_FIELD_STAGE_EGRESS:
          tcam_mem[0] = EFP_TCAMm;
          num_tcam = 1;
          break;
#if 0  
     case _BCM_FIELD_STAGE_EXACTMATCH:
          tcam_mem[0] = EXACT_MATCH_LOGICAL_TABLE_SELECTm;
          tcam_mem[1] = EXACT_MATCH_LOGICAL_TABLE_SELECT_TCAM_ONLYm;
          num_tcam = 2;
          break;
#endif /* 0 */
     case _BCM_FIELD_STAGE_CLASS:
          tcam_mem[0] = SRC_COMPRESSIONm;
          tcam_mem[1] = SRC_COMPRESSION_TCAM_ONLYm;
          tcam_mem[2] = DST_COMPRESSIONm;
          tcam_mem[3] = DST_COMPRESSION_TCAM_ONLYm;
          tcam_mem[4] = TTL_FNm;
          tcam_mem[5] = TOS_FNm;
          tcam_mem[6] = TCP_FNm;
          tcam_mem[7] = IP_PROTO_MAPm;
          num_tcam = 8;
          break;
     default:
          return BCM_E_PARAM;
   }

   for (idx = 0; idx < num_tcam; idx++) {
       set_mode[idx] = ser_mode;
   }

   for (ct = 0; num_tcam > 0; ct++,num_tcam--) {
      rv = soc_generic_ser_mem_update(unit, tcam_mem[ct], set_mode[ct]);
      if (SOC_FAILURE(rv)) {
         LOG_ERROR(BSL_LS_BCM_FP,
                   (BSL_META_U(unit,
                    "SER memory update failed[%d] to set"
                    " '%s' mode for tcam[%d].\n"),
                    rv,
                   (set_mode[ct] == _SOC_SER_MEM_MODE_GLOBAL)?"Global":"Unique",
                   tcam_mem[ct]));
      }
   }

   return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_field_wb_group_oper_mode_set
 * Purpose:
 *      Configure Field Processor Group operational mode
 *      per-CAP stage.
 * Parameters:
 *      unit    - (IN) BCM Device number.
 *      stage   - (IN) Field Stage Pipeline Qualifier enum value.
 *      mode    - (IN) Field Group Operational Mode enum value.
 * Returns:
 *      BCM_E_NONE   - Operation successful.
 *      BCM_E_PARAM  - Invalid operational mode or Field Stage.
 *      BCM_E_INIT   - BCM unit not initialized.
 *      BCM_E_BUSY   - Field stage has valid group/entry configured.
 *
 * Notes:
 *      Valid "stage" input parameter values supported by
 *      this API are:
 *              "bcmFieldQualifyStageLookup"
 * 	        "bcmFieldQualifyStageIngress"
 *              "bcmFieldQualifyStageEgress"
 *              "bcmFieldQualifyStageExactMatch".
 */

int
_bcm_field_wb_group_oper_mode_set(
    int unit,
    bcm_field_qualify_t stage,
    _field_stage_t     **stage_fc,
    bcm_field_group_oper_mode_t mode)
{
   _field_control_t   *fc;          /* Field control structure. */
   _field_stage_id_t  stage_id;     /* Pipeline stage id */
   _field_group_t     *fg;          /* Field group structure */
    int               rv;           /* Return Value */

   /* Validate input parameters */
   if ((mode != bcmFieldGroupOperModeGlobal) &&
       (mode != bcmFieldGroupOperModePipeLocal)) {
      return BCM_E_PARAM;
   }

   /* Update stage_id and tcam memory for the given stage */
   switch (stage) {
     case bcmFieldQualifyStageLookup:
          stage_id = _BCM_FIELD_STAGE_LOOKUP;
          break;
     case bcmFieldQualifyStageIngress:
          stage_id = _BCM_FIELD_STAGE_INGRESS;
          break;
     case bcmFieldQualifyStageEgress:
          stage_id = _BCM_FIELD_STAGE_EGRESS;
          break;
#if 0  
     case bcmFieldQualifyStageExactMatch:
          stage_id = _BCM_FIELD_STAGE_EXACTMATCH;
          break;
#endif /* 0 */
     case bcmFieldQualifyStageClass:
     case bcmFieldQualifyStageClassExactMatch:
          stage_id = _BCM_FIELD_STAGE_CLASS;
          break;
     default:
          return BCM_E_PARAM;
   }

   /* Verify whether the mode is already set */
   if ((*stage_fc)->oper_mode == mode) {
      return BCM_E_NONE;
   }

   /* Field control structure */
   BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

   /* Verify whether stage related group/entry is configured */
   fg = fc->groups;
   while (NULL != fg) {
      if (fg->stage_id == stage_id) {
         /* can't modify the stage as a group already exists */
         return BCM_E_BUSY;
      }
      fg = fg->next;
   }

   /* To reinitialize the stage, first delete the stage. */
   rv = _field_th_stage_delete(unit, fc, *stage_fc);
   if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
            "FP(unit %d) Error: Stage=%d Delete failed = %d.\n"),
            unit, (*stage_fc)->stage_id, rv));
        return (rv);
   }

   /*
    * Add the stage back. Stage structure is allocated in this function hence
    * pass the stage ID as input parameter to this function.
    * The oper_mode is updated in this function.
    */
   rv = _field_th_stage_add(unit, fc, stage_id, mode);
   if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
            "FP(unit %d) Error: Stage=%d Add failed = %d.\n"),
                                         unit, stage_id, rv));
        return (rv);
   }

   /* SER initialization */
   BCM_IF_ERROR_RETURN(_field_ser_oper_mode_init(unit, stage_id, mode));

   BCM_IF_ERROR_RETURN
      (_field_stage_control_get(unit, stage_id, stage_fc));

   /* Initialize qualifiers info. */
   BCM_IF_ERROR_RETURN(_field_th_qualifiers_init(unit, *stage_fc));

   /* Initialize stage actions information. */
   BCM_IF_ERROR_RETURN(_field_th_actions_init(unit, *stage_fc));

   /* Initialize supported qset for each stage. */
   BCM_IF_ERROR_RETURN(_bcm_field_stages_supported_qset_init(unit, fc));

   return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_field_th_group_oper_mode_set
 * Purpose:
 *      Configure Field Processor Group operational mode
 *      per-CAP stage.
 * Parameters:
 *      unit    - (IN) BCM Device number.
 *      stage   - (IN) Field Stage Pipeline Qualifier enum value.
 *      mode    - (IN) Field Group Operational Mode enum value.
 * Returns:
 *      BCM_E_NONE   - Operation successful.
 *      BCM_E_PARAM  - Invalid operational mode or Field Stage.
 *      BCM_E_INIT   - BCM unit not initialized.
 *      BCM_E_BUSY   - Field stage has valid group/entry configured.
 *
 * Notes:
 *      Valid "stage" input parameter values supported by
 *      this API are:
 *              "bcmFieldQualifyStageLookup"
 * 	        "bcmFieldQualifyStageIngress"
 *              "bcmFieldQualifyStageEgress"
 *              "bcmFieldQualifyStageExactMatch".
 */
int
_bcm_field_th_group_oper_mode_set(
    int unit,
    bcm_field_qualify_t stage,
    bcm_field_group_oper_mode_t mode)
{
   _field_stage_t     *stage_fc;    /* Stage field control structure. */
   _field_control_t   *fc;          /* Field control structure. */
   _field_stage_id_t  stage_id;     /* Pipeline stage id */
   _field_group_t     *fg;          /* Field group structure */
    int               rv;           /* Return Value */

   /* Validate input parameters */
   if ((mode != bcmFieldGroupOperModeGlobal) &&
       (mode != bcmFieldGroupOperModePipeLocal)) {
      return BCM_E_PARAM;
   }

   /* Update stage_id and tcam memory for the given stage */
   switch (stage) {
     case bcmFieldQualifyStageLookup:
          stage_id = _BCM_FIELD_STAGE_LOOKUP;
          break;
     case bcmFieldQualifyStageIngress:
          stage_id = _BCM_FIELD_STAGE_INGRESS;
          break;
     case bcmFieldQualifyStageEgress:
          stage_id = _BCM_FIELD_STAGE_EGRESS;
          break;
#if 0  
     case bcmFieldQualifyStageExactMatch:
          stage_id = _BCM_FIELD_STAGE_EXACTMATCH;
          break;
#endif /* 0 */
     case bcmFieldQualifyStageClass:
     case bcmFieldQualifyStageClassExactMatch:
          stage_id = _BCM_FIELD_STAGE_CLASS;
          break;
     default:
          return BCM_E_PARAM;
   }

   /*
    * Retrieve Field stage control structure,
    * It also return BCM_E_INIT if the field is not initialized.
    */
   BCM_IF_ERROR_RETURN
      (_field_stage_control_get(unit, stage_id, &stage_fc));

   /* Verify whether the mode is already set */
   if (stage_fc->oper_mode == mode) {
      return BCM_E_NONE;
   }

   /* Field control structure */
   BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

   /* Verify whether stage related group/entry is configured */
   fg = fc->groups;
   while (NULL != fg) {
      if (fg->stage_id == stage_id) {
         /* can't modify the stage as a group already exists */
         return BCM_E_BUSY;
      }
      fg = fg->next;
   }

   switch (stage_fc->oper_mode) {
        case bcmFieldGroupOperModeGlobal:
            /* Clear hardware table */
            BCM_IF_ERROR_RETURN(_field_th_hw_clear(unit, stage_fc));
            break;
        case bcmFieldGroupOperModePipeLocal:
            /* Clear hardware table */
            BCM_IF_ERROR_RETURN(_field_th_pipes_hw_clear(unit, stage_fc));
            break;
        default:
            return BCM_E_INTERNAL;
   }

   /* To reinitialize the stage, first delete the stage. */
   rv = _field_th_stage_delete(unit, fc, stage_fc);
   if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
            "FP(unit %d) Error: Stage=%d Delete failed = %d.\n"),
            unit, stage_fc->stage_id, rv));
        return (rv);
   }

   /*
    * Add the stage back. Stage structure is allocated in this function hence
    * pass the stage ID as input parameter to this function.
    * The oper_mode is updated in this function.
    */
   rv = _field_th_stage_add(unit, fc, stage_id, mode);
   if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
            "FP(unit %d) Error: Stage=%d Add failed = %d.\n"),
                                         unit, stage_id, rv));
        return (rv);
   }

   /* SER initialization */
   BCM_IF_ERROR_RETURN(_field_ser_oper_mode_init(unit, stage_id, mode));

   BCM_IF_ERROR_RETURN
      (_field_stage_control_get(unit, stage_id, &stage_fc));

   switch (stage_fc->oper_mode) {
        case bcmFieldGroupOperModeGlobal:
            /* Clear hardware table */
            BCM_IF_ERROR_RETURN(_field_th_hw_clear(unit, stage_fc));
            break;
        case bcmFieldGroupOperModePipeLocal:
            /* Clear hardware table */
            BCM_IF_ERROR_RETURN(_field_th_pipes_hw_clear(unit, stage_fc));
            break;
        default:
            return BCM_E_INTERNAL;
   }


   /* Initialize qualifiers info. */
   BCM_IF_ERROR_RETURN(_field_th_qualifiers_init(unit, stage_fc));

   /* Initialize stage actions information. */
   BCM_IF_ERROR_RETURN(_field_th_actions_init(unit, stage_fc));

   /* Initialize supported qset for each stage. */
   BCM_IF_ERROR_RETURN(_bcm_field_stages_supported_qset_init(unit, fc));

   return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_field_th_group_oper_mode_get
 * Purpose:
 *      Retrieve Field Processor Group operational mode
 *      per-CAP stage.
 * Parameters:
 *      unit    - (IN)  BCM Device number.
 *      stage   - (IN)  Field Stage Pipeline Qualifier enum value.
 *      mode    - (OUT) Reference to Field Group Operational
 *                      Mode enum value.
 * Returns:
 *      BCM_E_NONE   - Operation successful.
 *      BCM_E_PARAM  - Invalid parameter.
 *      BCM_E_INIT   - BCM unit not initialized.
 *
 * Notes:
 *      Valid "stage" input parameter values supported by
 *      this API are:
 *              "bcmFieldQualifyStageLookup"
 * 	        "bcmFieldQualifyStageIngress"
 *              "bcmFieldQualifyStageEgress"
 *              "bcmFieldQualifyStageExactMatch".
 */

int
_bcm_field_th_group_oper_mode_get(
    int unit,
    bcm_field_qualify_t stage,
    bcm_field_group_oper_mode_t *mode)
{
   _field_stage_t     *stage_fc; /* Stage field control structure. */
   _field_stage_id_t  stage_id;  /* Pipeline stage id */

   /* Validate input parameters */
   if (mode == NULL) {
      return BCM_E_PARAM;
   }

   switch (stage) {
     case bcmFieldQualifyStageLookup:
          stage_id = _BCM_FIELD_STAGE_LOOKUP;
          break;
     case bcmFieldQualifyStageIngress:
          stage_id = _BCM_FIELD_STAGE_INGRESS;
          break;
     case bcmFieldQualifyStageEgress:
          stage_id = _BCM_FIELD_STAGE_EGRESS;
          break;
     case bcmFieldQualifyStageClass:
     case bcmFieldQualifyStageClassExactMatch:
          stage_id = _BCM_FIELD_STAGE_CLASS;
          break;
#if 0  
     case bcmFieldQualifyStageExactMatch:
          stage_id = _BCM_FIELD_STAGE_EXACTMATCH;
          break;
#endif /* 0 */
     default:
          return BCM_E_PARAM;
          break;
   }

   /*
    * Retrieve Field stage control structure,
    * It also return BCM_E_INIT if the field is not initialized.
    */
   BCM_IF_ERROR_RETURN
      (_field_stage_control_get (unit, stage_id, &stage_fc));

   /* update the mode */
   *mode = stage_fc->oper_mode;
   return BCM_E_NONE;
}

/*
 * Function:
 *     _field_th_ingress_slice_enable_set
 *
 * Purpose:
 *     Enables or disables packet lookup on a hardware slice
 *     and configures the slice mode.
 *
 * Parameters:
 *     unit     - (IN) BCM device number.
 *     fg       - (IN) Reference to Group Structure.
 *     fs       - (IN) Reference to Slice Structure.
 *     enable   - (IN) enable/disable state of slice.
 *
 * Returns:
 *     BCM_E_NONE      - Success
 *     BCM_E_INTERNAL  - SOC read/write error
 *
 * Note:
 *     Calling function is responsible for checking feature availability.
 */

STATIC int
_field_th_ingress_slice_enable_set(int            unit,
                                   _field_group_t *fg,
                                   uint8          slice_idx,
                                   uint8          enable)
{
    uint32              regval_old, regval_new;
    uint8               mode;
    _field_stage_t     *stage_fc;
    soc_reg_t           reg;
    soc_reg_t           ifp_config_pipe[] = {IFP_CONFIG_PIPE0r,
                                             IFP_CONFIG_PIPE1r,
                                             IFP_CONFIG_PIPE2r,
                                             IFP_CONFIG_PIPE3r};

    BCM_IF_ERROR_RETURN(_field_stage_control_get(unit,
                        _BCM_FIELD_STAGE_INGRESS, &stage_fc));

    if (stage_fc->oper_mode == bcmFieldGroupOperModeGlobal) {
       reg = IFP_CONFIGr;
    } else {
       reg = ifp_config_pipe[fg->instance];
    }

    BCM_IF_ERROR_RETURN
       (soc_reg32_get(unit, reg, REG_PORT_ANY, slice_idx, &regval_old));
    regval_new = regval_old;

    enable = (enable == 0) ? 0 : 1;
    soc_reg_field_set(unit, reg, &regval_new, IFP_SLICE_ENABLEf, enable);
    soc_reg_field_set(unit, reg, &regval_new, IFP_SLICE_LOOKUP_ENABLEf, enable);

    /* Determine the slice mode based on group flags. */
    mode = _BCM_FIELD_IS_SLICE_MODE_PBMP_SUPPORTED(fg->flags) ?
           _IFP_SLICE_MODE_WIDE : _IFP_SLICE_MODE_NARROW;

    soc_reg_field_set(unit, reg, &regval_new, IFP_SLICE_MODEf, mode);

    if (regval_new != regval_old) {
       BCM_IF_ERROR_RETURN
          (soc_reg32_set(unit, reg, REG_PORT_ANY, slice_idx, regval_new));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_field_th_slice_enable_set
 *
 * Purpose:
 *     Enables or disables packet lookup on a hardware slice.
 *
 * Parameters:
 *     unit     - (IN) BCM device number
 *     fg       - (IN) Reference to Group Structure.
 *     fs       - (IN) Reference to Slice Structure.
 *     enable   - (IN) enable/disable state of slice.
 *
 * Returns:
 *     BCM_E_NONE      - SUCCESS
 *     BCM_E_INTERNAL  - SOC read/write error
 *
 * Note:
 *     Calling function is responsible for checking feature availability.
 */
int
_bcm_field_th_slice_enable_set(int                unit,
                               _field_group_t     *fg,
                               _field_slice_t     *fs,
                                uint8             enable)
{
    if (NULL == fg || NULL == fs) {
        return (BCM_E_PARAM);
    }

    switch (fg->stage_id) {
        case _BCM_FIELD_STAGE_INGRESS:
             return
                (_field_th_ingress_slice_enable_set(unit, fg,
                                                    fs->slice_number, enable));
             break;
        case _BCM_FIELD_STAGE_LOOKUP:
        case _BCM_FIELD_STAGE_EGRESS:
             return (_bcm_field_fb_slice_enable_set(unit, fg,
                                                    fs->slice_number, enable));
             break;
        default:
             LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META(
                         "Invalid stage_id[%d].\n\r"),
                         fg->stage_id));
             break;
    }

    return BCM_E_INTERNAL;
}

/*
 * Function:
 *     _bcm_field_th_qualify_class
 * Purpose:
 *     Qualifies field entry classifier id data and mask value
 * Parameters:
 *     unit  - (IN) BCM device number
 *     entry - (IN) Field entry identifier
 *     data  - (IN) Classifier ID lookup key value
 *     mask  - (IN) Classifier ID lookup mask value
 * Returns:
 *     BCM_E_XXXX
 */
int
_bcm_field_th_qualify_class(int unit,
                            bcm_field_entry_t entry,
                            bcm_field_qualify_t qual,
                            uint32 *data,
                            uint32 *mask)
{
    _field_entry_t *f_ent;
    uint32 cl_width, cl_max;
    uint32 ifp_cl_type; /* IFP Class Type propagted to EFP. */

    /* Get field entry part that contains the qualifier. */
    BCM_IF_ERROR_RETURN(_bcm_field_entry_qual_get(unit, entry, qual, &f_ent));

    switch (f_ent->group->stage_id) {
        case _BCM_FIELD_STAGE_INGRESS:
            switch (qual) {
                /* Class IDs from L2 Entry Table */
                case bcmFieldQualifySrcClassL2:
                case bcmFieldQualifyDstClassL2:
                    cl_width = 6;
                    break;
                /* Class fields from VFP_CLASS_ID_HI */
                case bcmFieldQualifySrcClassField:
                /* Class fields from VFP_CLASS_ID_L0 */
                case bcmFieldQualifyDstClassField:
                    cl_width = 10;
                    break;
                /* Class IDs from L3 Entry table (6) */
                case bcmFieldQualifySrcClassL3:
                case bcmFieldQualifyDstClassL3:
                    cl_width = 6;
                    break;
                /* Class ID from VLAN table (12) */
                case bcmFieldQualifyInterfaceClassL2:
                /* Class ID from L3_IIF (12) */
                case bcmFieldQualifyInterfaceClassL3:
                /* Class ID from Port Table (12) */
                case bcmFieldQualifyInterfaceClassPort:
                /* Class ID from SVP (12) */
                case bcmFieldQualifyInterfaceClassVPort:
                    cl_width = 12;
                    break;
                default:
                    return (BCM_E_INTERNAL);
            }

            cl_max = 1 << cl_width;

            if ((*data >= cl_max) ||
                (*mask != BCM_FIELD_EXACT_MATCH_MASK && *mask >= cl_max)) {
                return (BCM_E_PARAM);
            }
            break;

        case _BCM_FIELD_STAGE_LOOKUP:
             return
                (_bcm_field_td2_qualify_class(unit, entry, qual,
                                              data, mask));
        case _BCM_FIELD_STAGE_EGRESS:
            cl_width = 9;
            /* Need to set IFP_CLASS_TYPE in TCAM (upper 4 bits) */
            switch (qual) {
                case bcmFieldQualifySrcClassL2:
                    ifp_cl_type = 6;
                    break;
                case bcmFieldQualifySrcClassL3:
                    ifp_cl_type = 8;
                    break;
                case bcmFieldQualifySrcClassField:
                    ifp_cl_type = 4;
                    break;
                case bcmFieldQualifyDstClassL2:
                    ifp_cl_type = 7;
                    break;
                case bcmFieldQualifyDstClassL3:
                    ifp_cl_type = 9;
                    break;
                case bcmFieldQualifyDstClassField:
                    ifp_cl_type = 5;
                    break;
                case bcmFieldQualifyInterfaceClassL2:
                    ifp_cl_type = 10;
                    break;
                case bcmFieldQualifyInterfaceClassL3:
                    ifp_cl_type = 3;
                    break;
                case bcmFieldQualifyIngressClassField:
                    ifp_cl_type = 15;
                    break;
                case bcmFieldQualifyIngressInterfaceClassPort:
                    ifp_cl_type = 1;
                    break;
                case bcmFieldQualifyIngressInterfaceClassVPort:
                    ifp_cl_type = 2;
                    break;
                case bcmFieldQualifyEgressClass:
                case bcmFieldQualifyEgressClassL3Interface:
                case bcmFieldQualifyEgressClassTrill:
                case bcmFieldQualifyEgressClassL2Gre:
                    cl_width = 12;
                    ifp_cl_type = 0;
                    break;
                default:
                    return (BCM_E_INTERNAL);
            }

            cl_max   = 1 << cl_width;
            
            if (*data >= cl_max || (*mask != BCM_FIELD_EXACT_MATCH_MASK && *mask >= cl_max)) {
                return (BCM_E_PARAM);
            }

            if (ifp_cl_type) {
                *data |= ifp_cl_type << cl_width;
                *mask |= 0xf << cl_width;
            }
            break;
        default:
            return (BCM_E_INTERNAL);
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_field_th_qualify_class_get
 * Purpose:
 *     Retrieve field entry classifier id data and mask values
 * Parameters: 
 *     unit  - (IN) BCM device number
 *     entry - (IN) Field entry identifier
 *     qual  - (IN) Field entry qualifier enumeration
 *     data  - (IN/OUT) Classifier ID lookup key value
 *     mask  - (IN/OUT) Classifier ID lookup mask value
 * Returns:
 *     BCM_E_XXXX
 */
int
_bcm_field_th_qualify_class_get(int unit,
                                 bcm_field_entry_t entry,
                                 bcm_field_qualify_t qual,
                                 uint32 *data,
                                 uint32 *mask
                                 )
{
    _field_entry_t *f_ent;
    const uint32 m = (1 << 9) - 1;

    /* Get field entry part that contains the qualifier. */
    BCM_IF_ERROR_RETURN(_bcm_field_entry_qual_get(unit, entry, qual, &f_ent));

    if (f_ent->group->stage_id == _BCM_FIELD_STAGE_EGRESS) {
        switch (qual) {
            case bcmFieldQualifySrcClassL2:
            case bcmFieldQualifySrcClassL3:
            case bcmFieldQualifySrcClassField:
            case bcmFieldQualifyDstClassL2:
            case bcmFieldQualifyDstClassL3:
            case bcmFieldQualifyDstClassField:
            case bcmFieldQualifyInterfaceClassL2:
            case bcmFieldQualifyInterfaceClassL3:
            case bcmFieldQualifyIngressClassField:
            case bcmFieldQualifyIngressInterfaceClassPort:
            case bcmFieldQualifyIngressInterfaceClassVPort:
                /* Mask off IFP_CLASSID_TYPE */
                *data &= m;
                *mask &= m;
                break;
            default:
                break;
        }
    }

    return (BCM_E_NONE);
}

/*
 * Function: _bcm_field_th_ForwardingType_set
 *
 * Purpose:
 *     Add qualification data to an entry for IP Type.
 *
 * Parameters:
 *     unit     - (IN) BCM unit number.
 *     stage_id - (IN) Stage ID. 
 *     entry    - (IN) Entry ID.
 *     type     - (IN) Forwarding type.
 *     data     - (OUT) Data.
 *     mask     - (OUT) Mask.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_th_forwardingType_set(int unit, _field_stage_id_t stage_id,
                                 bcm_field_entry_t entry,
                                 bcm_field_ForwardingType_t type,
                                 uint32 *data, uint32 *mask)
{

    if (NULL == data || NULL == mask) {
       return BCM_E_PARAM;
    }

    switch (stage_id) {
       case _BCM_FIELD_STAGE_INGRESS:
           switch (type) {
            case bcmFieldForwardingTypeAny:
                *data = 0;
                *mask = 0;
                break;
            case bcmFieldForwardingTypeL2:
                *data = 0;
                *mask = 0xE;
                break;
            case bcmFieldForwardingTypeL2Independent:
                *data = 0;
                *mask = 0xF;
                break;
            case bcmFieldForwardingTypeL2Shared:
                *data = 1;
                *mask = 0xF;
                break;
            case bcmFieldForwardingTypeL2Vpn:
                *data = 2;
                *mask = 0xF;
                break;
            case bcmFieldForwardingTypeL2VpnDirect:
                *data = 3;
                *mask = 0xF;
                break;
            case bcmFieldForwardingTypeL3Direct:
                *data = 4;
                *mask = 0xF;
                break;
            case bcmFieldForwardingTypeL3:
                *data = 5;
                *mask = 0xF;
                break;
            case bcmFieldForwardingTypeMpls:
                *data = 7;
                *mask = 0xF;
                break;
            case bcmFieldForwardingTypeTrill:
                *data = 8;
                *mask = 0xF;
                break;
            default:
               return (BCM_E_PARAM);
          }
          break;
      default:
          return BCM_E_INTERNAL;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_field_th_forwardingType_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyForwardingType
 *                     qualifier from the field entry.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      stage_id - (IN) Stage ID. 
 *      entry    - (IN) BCM field entry id.
 *      type     - (OUT) Qualifier match forwarding type.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
_bcm_field_th_forwardingType_get(int unit, _field_stage_id_t stage_id,
                                 bcm_field_entry_t entry,
                                 uint32 data,  uint32 mask,
                                 bcm_field_ForwardingType_t *type)
{
    if (NULL == type) {
       return BCM_E_PARAM;
    }

    switch (stage_id) {
       case _BCM_FIELD_STAGE_INGRESS:
          if ((0 == data) && (mask == 0)) {
              *type = bcmFieldForwardingTypeAny;
          } else if ((0 == data) && (mask == 0xE)) {
              *type = bcmFieldForwardingTypeL2;
          } else if ((0 == data) && (mask == 0xF)) {
              *type = bcmFieldForwardingTypeL2Independent;
          } else if ((1 == data) && (mask == 0xF)) {
              *type = bcmFieldForwardingTypeL2Shared;
          } else if ((2 == data) && (mask == 0xF)) {
              *type =  bcmFieldForwardingTypeL2Vpn;
          } else if ((3 == data) && (mask == 0xF)) {
              *type = bcmFieldForwardingTypeL2VpnDirect;
          } else if ((4 == data) && (mask == 0xF)) {
              *type = bcmFieldForwardingTypeL3Direct;
          } else if ((5 == data) && (mask == 0xF)) {
              *type = bcmFieldForwardingTypeL3;
          } else if ((7 == data) && (mask == 0xF)) {
              *type = bcmFieldForwardingTypeMpls;
          } else if ((8 == data) && (mask == 0xF)) {
              *type = bcmFieldForwardingTypeTrill;
          } else {
              return (BCM_E_INTERNAL);
          }
           break;
       default:
           return BCM_E_INTERNAL;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_field_th_qualify_svp
 * Purpose:
 *      Add source virtual port field qualification to a field entry.
 * Parameters:
 *      unit  - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      qual  - (IN) SVP Qualifier type
 *      data  - (IN) SVP data
 *      mask  - (IN) SVP Mask
 *      svp_valid - (IN) Indication to SVP Valid field
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
_bcm_field_th_qualify_svp(int unit, bcm_field_entry_t entry,
                          bcm_field_qualify_t qual,
                          uint32 data, uint32 mask,
                          int svp_valid)
{
    int                        rv;        /* Operational Status */
     _field_entry_t            *f_ent;    /* Field entry structure. */
     _bcm_field_qual_offset_t  *q_offset; /* Field qualifier offset info. */

     /* Get the field entry operational structure. */
     BCM_IF_ERROR_RETURN(_bcm_field_entry_qual_get(unit, entry,
                                                   qual, &f_ent));

     /* Get qualifier offsets in the tcam. */
     rv = _field_qual_offset_get(unit, f_ent, qual, &q_offset);
     BCM_IF_ERROR_RETURN(rv);

    if (mask == (uint32)BCM_FIELD_EXACT_MATCH_MASK) {
        uint32 multi_width;

        _BCM_FIELD_QUAL_MULTI_OFFSET_WIDTH(q_offset, multi_width);

        /* Exact match requested, make maximum mask value */
        mask =  (1 << multi_width) - 1;
    }

    /* coverity[address_of : FALSE] */
    /* coverity[callee_ptr_arith : FALSE] */
    rv = _bcm_field_th_qualify_set(unit, entry, qual, &data, &mask);
    BCM_IF_ERROR_RETURN(rv);

    f_ent->flags |= _FP_ENTRY_DIRTY;
    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_field_th_qualify_dvp
 * Purpose:
 *      Add destination virtual port field qualification to a field entry.
 * Parameters:
 *      unit  - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      qual  - (IN) SVP Qualifier type
 *      data  - (IN) SVP data
 *      mask  - (IN) SVP Mask
 *      dvp_valid - (IN) Indication to DVP Valid field
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
_bcm_field_th_qualify_dvp(int unit, 
                          bcm_field_entry_t entry,
                          bcm_field_qualify_t qual,
                          uint32 data, uint32 mask, 
                          int dvp_valid)
{
    int                       rv;        /* Operational Status */
     _field_entry_t            *f_ent;   /* Field entry structure. */
     _bcm_field_qual_offset_t  *q_offset; /* Field qualifier offset info. */


     /* Get the field entry operational structure. */
     BCM_IF_ERROR_RETURN(_bcm_field_entry_qual_get(unit, entry,
                                                   qual, &f_ent));

     /* Get qualifier offsets in the tcam. */
     rv = _field_qual_offset_get(unit, f_ent, qual, &q_offset);
     BCM_IF_ERROR_RETURN(rv);

    if (mask == (uint32)BCM_FIELD_EXACT_MATCH_MASK) {
        uint32 multi_width;

        _BCM_FIELD_QUAL_MULTI_OFFSET_WIDTH(q_offset, multi_width);
        /* Exact match requested, make maximum mask value */
        mask =  (1 << multi_width) - 1;
    }

    /* coverity[address_of : FALSE] */
    /* coverity[callee_ptr_arith : FALSE] */
    rv = _bcm_field_th_qualify_set(unit, entry, qual, &data, &mask);
    BCM_IF_ERROR_RETURN(rv);

    f_ent->flags |= _FP_ENTRY_DIRTY;
    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_field_th_qualify_trunk
 * Purpose:
 *      Match on Source or Destination Trunk in Field Processor.
 * Parameters:
 *      unit  - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      qual  - (IN) Trunk Qualifier type
 *      data  - (IN) Trunk data
 *      mask  - (IN) Trunk Mask
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
_bcm_field_th_qualify_trunk(int unit, 
                            bcm_field_entry_t entry,
                            bcm_field_qualify_t qual,
                            bcm_trunk_t data, bcm_trunk_t mask)
{
    int              rv;          /* Operational Status. */
    uint32           hw_data = 0; /* HW buffer data. */
    uint32           hw_mask = 0; /* HW buffer mask. */
     _field_entry_t  *f_ent;      /* Field entry structure. */


    TRUNK_CHK_TGID_EXTENDED(unit, data);

    rv = _field_entry_get(unit, entry, _FP_ENTRY_PRIMARY, &f_ent);
    BCM_IF_ERROR_RETURN(rv);

    if (_BCM_FIELD_STAGE_INGRESS == f_ent->group->stage_id) {

        /* set the trunk bit */
        hw_data = data | (0x1 << SOC_TRUNK_BIT_POS(unit));
        hw_mask = mask | (0x1 << SOC_TRUNK_BIT_POS(unit));

        /* coverity[address_of : FALSE] */
        /* coverity[callee_ptr_arith : FALSE] */
        rv = _bcm_field_th_qualify_set(unit, entry, qual, &hw_data, &hw_mask);
        BCM_IF_ERROR_RETURN(rv);

        f_ent->flags |= _FP_ENTRY_DIRTY;
    } else {
        return _bcm_field_qualify_trunk(unit, entry, qual, data, mask);
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_field_th_qualify_trunk_get
 * Purpose:
 *      Get Source or Destination Trunk qualified in Field Processor Key.
 * Parameters:
 *      unit  - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      qual  - (IN) Trunk Qualifier type
 *      data  - (OUT) Trunk data
 *      mask  - (OUT) Trunk Mask
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */

int
_bcm_field_th_qualify_trunk_get(int unit,
                                bcm_field_entry_t entry,
                                bcm_field_qualify_t qual,
                                bcm_trunk_t *data, bcm_trunk_t *mask)
{
    int       rv;          /* Operational Status. */
    uint32    hw_data = 0; /* HW buffer data. */
    uint32    hw_mask = 0; /* HW buffer mask. */
    _field_entry_t *f_ent; /* Field entry structure. */


    /* Input parameters check. */
    if ((NULL == data) || (NULL == mask))  {
        return (BCM_E_PARAM);
    }

    if ((qual != bcmFieldQualifySrcTrunk) && (qual != bcmFieldQualifyDstTrunk)) {
        return (BCM_E_INTERNAL);
    }

    rv = _field_entry_get(unit, entry, _FP_ENTRY_PRIMARY, &f_ent);
    BCM_IF_ERROR_RETURN(rv);

    if (_BCM_FIELD_STAGE_INGRESS == f_ent->group->stage_id) {

        /* Read qualifier match value and mask. */
        rv = _bcm_field_entry_qualifier_uint32_get(unit, entry, qual,
                                                   &hw_data, &hw_mask);

        /* set the trunk bit */
        *data = hw_data & ~(0x1 << SOC_TRUNK_BIT_POS(unit));
        *mask = hw_mask & ~(0x1 << SOC_TRUNK_BIT_POS(unit));

    } else {
        return _bcm_field_qualify_trunk_get(unit, entry, qual, data, mask);
    }

    return (BCM_E_NONE);
}
/*
 * Function:
 *      _bcm_field_th_qualify_InPorts
 * Purpose:
 *      Match on Source or Destination Trunk in Field Processor.
 * Parameters:
 *      unit  - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      qual  - (IN) IPBM Qualifier type
 *      data  - (IN) IPBM data
 *      mask  - (IN) IPBM Mask
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
_bcm_field_th_qualify_InPorts(int unit, 
                              bcm_field_entry_t entry,
                              bcm_field_qualify_t qual,
                              bcm_pbmp_t data, 
                              bcm_pbmp_t mask,
                              bcm_pbmp_t valid_pbmp)
{
    int            rv;         /* Operational Status.         */
    _field_entry_t *f_ent;     /* Field entry structure.      */
    bcm_pbmp_t     temp_pbm;   /* Holds valid pbm supported.  */
    bcm_port_t     port;       /* Holds physical port number. */

    if (bcmFieldQualifyInPorts != qual) {
        return BCM_E_PARAM;
    }


    BCM_PBMP_ASSIGN(temp_pbm, valid_pbmp);
    BCM_PBMP_AND(temp_pbm, data);
    if (BCM_PBMP_NEQ(temp_pbm, data)) {
       return BCM_E_PARAM;
    }

    /*
     * Check and clear invalid ports in the mask. 
     */
    BCM_PBMP_AND(valid_pbmp, mask);
    BCM_PBMP_ASSIGN(mask, valid_pbmp);

    rv = _field_entry_get(unit, entry, _FP_ENTRY_PRIMARY, &f_ent);
    BCM_IF_ERROR_RETURN(rv);
 

    BCM_PBMP_CLEAR(f_ent->pbmp.data);
    BCM_PBMP_CLEAR(f_ent->pbmp.mask);
 
    switch (f_ent->group->instance) {
        case _BCM_FIELD_PIPE0_GROUP:
        case _BCM_FIELD_PIPE1_GROUP:
        case _BCM_FIELD_PIPE2_GROUP:
        case _BCM_FIELD_PIPE3_GROUP:
            BCM_PBMP_ITER(data, port)  {
                 BCM_PBMP_PORT_ADD(f_ent->pbmp.data, port - 
                                   (34 * f_ent->group->instance));
            }
            BCM_PBMP_ITER(mask, port)  {
                 BCM_PBMP_PORT_ADD(f_ent->pbmp.mask, port -
                                   (34 * f_ent->group->instance));
            }
            break;
        default:
            return BCM_E_INTERNAL;
    } 

    /* coverity[address_of : FALSE] */
    /* coverity[callee_ptr_arith : FALSE] */
    rv = _bcm_field_th_qualify_set(unit, entry, qual, 
                                   (uint32 *)&f_ent->pbmp.data, 
                                   (uint32 *)&f_ent->pbmp.mask);
    BCM_IF_ERROR_RETURN(rv);

    f_ent->flags |= _FP_ENTRY_DIRTY;

    return BCM_E_NONE;
}
/*
 * Function:
 *     _bcm_field_th_qual_part_offset_get
 * Purpose:
 *     Retrieve qualifier offsets
 * Parameters:
 *     unit       - (IN) BCM device number.
 *     f_ent      - (IN) Field entry structure.
 *     entry_part - (IN) Entry part
 *     qid        - (IN) Qualifier id.
 *     offset     - (OUT) Qualifier offset and width.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_th_qual_part_offset_get(int unit, _field_entry_t *f_ent, 
                                   int entry_part, int qid, 
                                   _bcm_field_qual_offset_t *offset)
{
    _bcm_field_group_qual_t *q_arr;     /* Qualifiers array.          */
    _field_group_t          *fg;        /* Field group structure.     */
    uint8                   entry_type; /* Index to tcam entries
                                           of multiple types      */
    int                     idx;        /* Entry qualifiers iterator. */

    LOG_DEBUG(BSL_LS_BCM_FP,
              (BSL_META_U(unit,
               "_field_qual_offset_get\n")));

    /* Input parameters check. */
    if ((NULL == offset) || (NULL == f_ent)) {
        return (BCM_E_PARAM);
    }

    /* Validate Entry Part */
    if (entry_part > _FP_MAX_ENTRY_WIDTH) {
        return (BCM_E_PARAM);
    }

    /* Get entry group. */
    fg = f_ent->group;

    if (0 == BCM_FIELD_QSET_TEST(fg->qset, qid)) {
        /* Only qualifiers that were explicitly requested may be used */
        LOG_ERROR(BSL_LS_BCM_FP,
                  (BSL_META_U(unit,
                  "FP(unit %d) Error: qual=%s not in group=%d Qset\n"),
                   unit, _field_qual_name(qid), f_ent->group->gid));
        return (BCM_E_PARAM);
    }

    entry_type = _FP_ENTRY_TYPE_DEFAULT;
    /* Get qualifier information. */
    q_arr = &(fg->qual_arr[entry_type][entry_part]);
    for (idx = 0; idx < q_arr->size; idx++) {
        if (qid == q_arr->qid_arr[idx]) {
            sal_memcpy(offset, q_arr->offset_arr + idx,
                          sizeof(_bcm_field_qual_offset_t));
            break;
        }
    }

    if (idx == q_arr->size) {
        offset = NULL;
        return (BCM_E_NOT_FOUND);
    }

    /*
     *  For Single wide mode, MSB 80-bits of 160-bits are valid
     *  in the final key, hence shift the offset by 80-bit.
     */
    if ((fg->flags & _FP_GROUP_SPAN_SINGLE_SLICE) &&
        !(fg->flags & _FP_GROUP_INTRASLICE_DOUBLEWIDE)) {
         int qual_part;

        for (qual_part = 0; qual_part < (offset)->num_offsets; ++qual_part) {
            if ((offset)->width[qual_part] > 0) {
                (offset)->offset[qual_part] -= 80;
            }
        }
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_field_th_qual_complete_value_get
 * Purpose:
 *     Retrieve qualifier's actual data and mask.
 * Parameters:
 *     unit        - (IN) BCM device number.
 *     f_ent       - (IN) Field entry structure.
 *     qid         - (IN) Qualifier id.
 *     q_data      - (OUT) Qualifier data array.
 *     q_mask      - (OUT) Qualifier mask array.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_field_th_qual_complete_value_get(int unit, _field_entry_t *f_ent,
                                      int qid,
                                      _bcm_field_qual_data_t q_data,
                                      _bcm_field_qual_data_t q_mask)
{
    int                       rv = BCM_E_INTERNAL;
    int                       width = 0;
    int                       start_offset = 0;
    int                       part_ct = 0;  
    int                       entry_parts = 0;  
    _bcm_field_qual_offset_t  q_offset;
    _bcm_field_qual_data_t    local_data;
    _bcm_field_qual_data_t    local_mask;
   
    _FP_QUAL_DATA_CLEAR(q_data);
    _FP_QUAL_DATA_CLEAR(q_mask);
 
    if (NULL == f_ent) {
       return BCM_E_PARAM;
    }

    /* Retrieve group entry parts */
    rv = _bcm_field_th_entry_tcam_parts_count(unit, f_ent->group->flags,
                                              &entry_parts);
    if (BCM_FAILURE(rv)) {
        return (rv);
    }

    for (part_ct = 0; part_ct < entry_parts; part_ct++) {
        /* Initialize q_offset to NULL */
        sal_memset(&q_offset, 0x0, sizeof(q_offset));

        /* Retrieve the Qualifier offsets for the particular entry part */  
        rv = _bcm_field_th_qual_part_offset_get(unit, f_ent, part_ct,
                                               qid, &q_offset);
        if (rv == BCM_E_NOT_FOUND) {
           continue;
        } else if (BCM_FAILURE(rv)) {
           return rv;
        }

       _FP_QUAL_DATA_CLEAR(local_data);
       _FP_QUAL_DATA_CLEAR(local_mask);

       /* Combined width of a qualifier for a given part */
       _BCM_FIELD_QUAL_MULTI_OFFSET_WIDTH(&q_offset, width);
       if (width > 0) {
          /* Read qualifier match value and mask. */
          rv = _bcm_field_qual_value_get(unit, &q_offset, f_ent + part_ct,
                                         local_data, local_mask);
          BCM_IF_ERROR_RETURN(rv);
       } else {
          continue;
       }

       /* Adjust Data and Mask based on the part offsets. */
       rv = _bcm_field_qual_partial_data_set(local_data, start_offset,
                                             width, q_data);
       BCM_IF_ERROR_RETURN(rv);
       rv = _bcm_field_qual_partial_data_set(local_mask, start_offset,
                                             width, q_mask);
       BCM_IF_ERROR_RETURN(rv);
       /* Increment the offset for the next part */
       start_offset += width;
    }

   /* 
    * Verify whether qualifier offset is present in the qual Array.
    */
    if (start_offset == 0 && width == 0) {
        LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
              "Qualifier[%s] Offset not found for Entry:%d\n\r"),
               _field_qual_name(qid), f_ent->eid));
        return BCM_E_NOT_FOUND; 
    }

    return BCM_E_NONE; 
}

/*
 * Function:
 *      _bcm_field_th_qualify_set
 *
 * Purpose:
 *      Utility routine for qualify APIs taking 32 bits or less
 *
 * Parameters:
 *      unit - BCM device number
 *      entry - Entry ID
 *      qual  - Qualifier field
 *      data  - Matching data
 *      mask  - Bit mask for data
 *
 * Returns:
 *     BCM_E_NONE     - BCM device number
 *     BCM_E_PARAM    - Qualifier not in group's Qset
 */

int
_bcm_field_th_qualify_set(int unit, bcm_field_entry_t entry,
                          int qual, uint32 *data, uint32 *mask)
{
    int                      rv;
    _bcm_field_qual_offset_t q_offset;
    _field_entry_t           *f_ent;
    _bcm_field_qual_data_t   out_data1;
    _bcm_field_qual_data_t   out_mask1;
    _bcm_field_qual_data_t   out_data2;
    _bcm_field_qual_data_t   out_mask2;
    uint32                   start_offset = 0;
    int                      parts_cnt;
    int                      miss_count;
    int                      part_idx;
    int                      idx;
    _bcm_field_qual_data_t   null_data;
    int                      bit_pos = 0;
    int                      qual_width = 0;
    uint16                   chunk_size[_BCM_FIELD_QUAL_OFFSET_MAX] = {0};
    uint16                   chunk_offset[_BCM_FIELD_QUAL_OFFSET_MAX] = {0};
    _field_stage_t           *stage_fc;
    _bcm_field_qual_data_t   q_data;
    _bcm_field_qual_data_t   q_mask;

    if (NULL == data || NULL == mask) {
        return BCM_E_PARAM;
    }

    /* Retrieve the field entry. */
    rv = _field_entry_get(unit, entry, _FP_ENTRY_PRIMARY, &f_ent);
    BCM_IF_ERROR_RETURN(rv);

    /* Currently this function handles qualifiers related to IFP */
    if (f_ent->group->stage_id != _BCM_FIELD_STAGE_INGRESS) {
        return BCM_E_INTERNAL;
    }

    /* Retrieve group entry parts */
    rv = _bcm_field_th_entry_tcam_parts_count(unit, f_ent->group->flags,
                                              &parts_cnt);
    if (BCM_FAILURE(rv)) {
        return (rv);
    }

    rv = _field_stage_control_get(unit, f_ent->group->stage_id, &stage_fc);
    BCM_IF_ERROR_RETURN(rv);


    switch (qual) {
        case bcmFieldQualifyL2SrcStatic:
        case bcmFieldQualifyL2SrcHit:
        case bcmFieldQualifyDstIpLocal:
        case bcmFieldQualifyHiGig:
        case bcmFieldQualifyForwardingVlanValid:
        case bcmFieldQualifyL2CacheHit:
        case bcmFieldQualifyL3SrcHostHit:
        case bcmFieldQualifyIpmcStarGroupHit:
        case bcmFieldQualifyL3DestHostHit:
        case bcmFieldQualifyL2StationMove:
        case bcmFieldQualifyDosAttack:
        case bcmFieldQualifyIpTunnelHit:
        case bcmFieldQualifyMyStationHit:
        case bcmFieldQualifyIpAddrsNormalized:
        case bcmFieldQualifyHiGigProxy:
        case bcmFieldQualifyL4Ports:
        case bcmFieldQualifyMplsLabel2Hit:
        case bcmFieldQualifyTrillIngressRbridgeHit:
        case bcmFieldQualifyL2GreVfiHit:
        case bcmFieldQualifyMimVfiHit:
        case bcmFieldQualifyMplsTerminated:
        case bcmFieldQualifyInnerVlanPri:
        case bcmFieldQualifyInnerVlanCfi:
        case bcmFieldQualifyOuterVlanPri:
        case bcmFieldQualifyOuterVlanCfi:

            /*
             * Retrieve the Original qualifier width and Bit position.
             */  
            if ((stage_fc->f_qual_arr[qual] != NULL) &&
                (stage_fc->f_qual_arr[qual]->conf_arr != NULL)) {
                bit_pos = stage_fc->f_qual_arr[qual]->conf_arr->offset.bit_pos;
                qual_width = stage_fc->f_qual_arr[qual]->conf_arr->offset.qual_width;
            } else {
                return BCM_E_INTERNAL;
            }

            /*
             * Retrieve the complete value on the given extractor and Add
             * the existing values to the current data before Set.
             * This case is applicable for the cases, where more than one qualifier
             * is placed on the same extractor.
             */   

            _FP_QUAL_DATA_CLEAR(q_data);
            _FP_QUAL_DATA_CLEAR(q_mask);
            /* 
             * Validate the Qualifier Data and modify the data as per the
             * bit position and  original width.
             */
            if (data[0] & ~((1 << qual_width) - 1)) {
                return BCM_E_PARAM;   
            } 

            data[0] <<= bit_pos; 
            mask[0] = (mask[0] & ((1 << qual_width) - 1)) << bit_pos;

            rv = _bcm_field_th_qual_complete_value_get(unit, f_ent,
                                             qual, q_data, q_mask);
            if ((rv != BCM_E_NOT_FOUND) && (BCM_FAILURE(rv))) {
                return rv;
            }

            if (rv == BCM_E_NONE) {
                q_data[0] &= ~(((1 << qual_width) - 1) << bit_pos);
                q_mask[0] &= ~(((1 << qual_width) - 1) << bit_pos);

                data[0] |= q_data[0];
                mask[0] |= q_mask[0];
            }
            break;
        default:
            break;   
    }

    /* Initialize q_offset to NULL */
    sal_memset(&q_offset, 0x0, sizeof(q_offset));
    _FP_QUAL_DATA_CLEAR(null_data);
    miss_count = 0;

    /* Update each chunk size of the qualifier. */
    for (part_idx = 0; part_idx < parts_cnt; ++part_idx) {
        /* Retrieve the Qualifier offsets for the particular entry part */
        rv = _bcm_field_th_qual_part_offset_get(unit, f_ent + part_idx,
                                                part_idx, qual, &q_offset);

        if (rv == BCM_E_NOT_FOUND) {
            continue;
        } else if (BCM_FAILURE(rv)) {
           return rv;
        }

        for (idx = 0; idx < _BCM_FIELD_QUAL_OFFSET_MAX; idx++) {
             if (q_offset.width[idx]) {
                 chunk_size[idx] = q_offset.width[idx];
             }
        } 
    }

    /* Update each chunk offset of the qualifier. offsets in q_offset
     * are local to the partition but chunk_offset array will have offsets
     * across the partitions.
     */
    for (idx = 0; idx < _BCM_FIELD_QUAL_OFFSET_MAX; idx++) {

        if (idx == 0) {
            chunk_offset[idx] = 0;
            continue;
        }

        if (chunk_size[idx]) {
           chunk_offset[idx] = chunk_offset[idx-1] + chunk_size[idx - 1];
        } else {
           break;
        }
    }

    for (part_idx = 0; part_idx < parts_cnt; ++part_idx) {
        /* Retrieve the Qualifier offsets for the particular entry part */  
        rv = _bcm_field_th_qual_part_offset_get(unit, f_ent + part_idx, 
                                                part_idx, qual, &q_offset);
        if (rv == BCM_E_NOT_FOUND) {
            miss_count += 1;
            continue;
        } else if (BCM_FAILURE(rv)) {
           return rv;
        }

        _FP_QUAL_DATA_CLEAR(out_data2);
        _FP_QUAL_DATA_CLEAR(out_mask2);

        start_offset = 0;

        /* Extract relevent data for this partion from user passed data. */
        for (idx = 0; idx < _BCM_FIELD_QUAL_OFFSET_MAX; idx++) {
      
            if (0 == q_offset.width[idx]) {
                continue;
            }

            _FP_QUAL_DATA_CLEAR(out_data1);
            _FP_QUAL_DATA_CLEAR(out_mask1);

             
            rv = _bcm_field_qual_partial_data_get(data, chunk_offset[idx],
                                              q_offset.width[idx], out_data1);
            BCM_IF_ERROR_RETURN(rv);

            rv = _bcm_field_qual_partial_data_set(out_data1, start_offset, 
                                              q_offset.width[idx], out_data2);
            BCM_IF_ERROR_RETURN(rv);

            rv = _bcm_field_qual_partial_data_get(mask, chunk_offset[idx],
                                              q_offset.width[idx], out_mask1);
            BCM_IF_ERROR_RETURN(rv);

            rv = _bcm_field_qual_partial_data_set(out_mask1, start_offset, 
                                              q_offset.width[idx], out_mask2);
            BCM_IF_ERROR_RETURN(rv);

            start_offset += q_offset.width[idx];

        }

        /* Program data/mask pair to tcam buffer. */
        /* coverity[callee_ptr_arith : FALSE] */
        rv = _bcm_field_qual_value_set(unit, &q_offset, f_ent + part_idx,
                                       out_data2, out_mask2);
        BCM_IF_ERROR_RETURN(rv);

    }

    /* 
     * Verify whether qualifier offset is present in the qual Array.
     */
    if (miss_count == parts_cnt) {
        LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
              "Qualifier[%s] Offset not found for Entry:%d\n\r"),
               _field_qual_name(qual), entry));

        return BCM_E_NOT_FOUND;
    }

    f_ent->flags |= _FP_ENTRY_DIRTY;
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_field_th_entry_qualifier_key_get
 *
 * Purpose:
 *     Get qualifier match value and mask.
 *
 * Parameters:
 *     unit      - BCM device number
 *     entry     - Entry id.
 *     qual_id   - qualifier id.
 *     q_data    - Qualifier data array.
 *     q_mask    - Qualifier mask array.
 * Returns:
 *     Nothing.
 */
int
_bcm_field_th_entry_qualifier_key_get(int unit,
                                      bcm_field_entry_t entry,
                                      int qual_id,
                                      _bcm_field_qual_data_t q_data,
                                      _bcm_field_qual_data_t q_mask)
{
    int                       entry_parts; /* Tcam part number.             */
    _bcm_field_qual_offset_t  q_offset;    /* Qualifier offset in the tcam. */
    _field_entry_t            *f_ent;      /* Field entry structure.        */
    int                       rv;          /* Operation return status.      */
    uint32                    width = 0;   /* Offset width                  */
    uint32                    start_offset = 0;
    int                       part_ct;
    _bcm_field_qual_data_t    local_data;
    _bcm_field_qual_data_t    local_mask;
    uint8                     bit_pos;
    _field_stage_t           *stage_fc;

    /* Get entry part that contains requested qualifier. */
    rv = _field_entry_get(unit, entry, _FP_ENTRY_PRIMARY, &f_ent);
    BCM_IF_ERROR_RETURN(rv);

    if (f_ent->group->stage_id != _BCM_FIELD_STAGE_INGRESS) {
       return BCM_E_INTERNAL;
    }

    /* Get number of parts in field entry based on group flags. */
    rv = _bcm_field_th_entry_tcam_parts_count (unit, f_ent->group->flags,
                                               &entry_parts);
    BCM_IF_ERROR_RETURN(rv);

    for (part_ct = 0; part_ct < entry_parts; part_ct++) {
        /* Initialize q_offset to NULL */
        sal_memset(&q_offset, 0x0, sizeof(q_offset));

        /* Retrieve the Qualifier offsets for the particular entry part */  
        rv = _bcm_field_th_qual_part_offset_get(unit, f_ent, part_ct,
                                               qual_id, &q_offset);
        if (rv == BCM_E_NOT_FOUND) {
           continue;
        } else if (BCM_FAILURE(rv)) {
           return rv;
        }

       /* Combined width of a qualifier for a given part */
       _BCM_FIELD_QUAL_MULTI_OFFSET_WIDTH(&q_offset, width);
       if (width > 0) {
          /* Read qualifier match value and mask. */
          rv = _bcm_field_qual_value_get(unit, &q_offset, f_ent + part_ct,
                                         local_data, local_mask);
          BCM_IF_ERROR_RETURN(rv);
       } else {
          continue;
       }

       /* Adjust Data and Mask based on the part offsets. */
       rv = _bcm_field_qual_partial_data_set(local_data, start_offset,
                                             width, q_data);
       BCM_IF_ERROR_RETURN(rv);
       rv = _bcm_field_qual_partial_data_set(local_mask, start_offset,
                                             width, q_mask);
       BCM_IF_ERROR_RETURN(rv);
       /* Increment the offset for the next part */
       start_offset += width;
    }

   /* 
    * Verify whether qualifier offset is present in the qual Array.
    */
    if (start_offset == 0 && width == 0) {
        LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
              "Qualifier[%s] Offset not found for Entry:%d\n\r"),
               _field_qual_name(qual_id), entry));
        return BCM_E_NOT_FOUND; 
    }

    switch (qual_id) {
        case bcmFieldQualifyL2SrcStatic:
        case bcmFieldQualifyL2SrcHit:
        case bcmFieldQualifyDstIpLocal:
        case bcmFieldQualifyHiGig:
        case bcmFieldQualifyForwardingVlanValid:
        case bcmFieldQualifyL2CacheHit:
        case bcmFieldQualifyL3SrcHostHit:
        case bcmFieldQualifyIpmcStarGroupHit:
        case bcmFieldQualifyL3DestHostHit:
        case bcmFieldQualifyL2StationMove:
        case bcmFieldQualifyDosAttack:
        case bcmFieldQualifyIpTunnelHit:
        case bcmFieldQualifyMyStationHit:
        case bcmFieldQualifyIpAddrsNormalized:
        case bcmFieldQualifyHiGigProxy:
        case bcmFieldQualifyL4Ports:
        case bcmFieldQualifyMplsLabel2Hit:
        case bcmFieldQualifyTrillIngressRbridgeHit:
        case bcmFieldQualifyL2GreVfiHit:
        case bcmFieldQualifyMimVfiHit:
        case bcmFieldQualifyMplsTerminated:
        case bcmFieldQualifyInnerVlanPri:
        case bcmFieldQualifyOuterVlanPri:
        case bcmFieldQualifyInnerVlanCfi:
        case bcmFieldQualifyOuterVlanCfi:
            rv = _field_stage_control_get(unit, f_ent->group->stage_id, &stage_fc);
            BCM_IF_ERROR_RETURN(rv);
            /* Adjust the Data and Mask as per API input */
            if (stage_fc->f_qual_arr[qual_id]->conf_arr != NULL) {
                bit_pos = stage_fc->f_qual_arr[qual_id]->conf_arr->offset.bit_pos;
                q_data[0] >>= bit_pos;
                q_mask[0] >>= bit_pos;
            }
            break;
        default:
            break;
    }    

    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_field_th_qualifier_delete
 * Purpose:
 *      Remove match criteria from a field processor entry.
 * Parameters:
 *      unit    - (IN) Unit number.
 *      entry   - (IN) BCM field entry id.
 *      qual_id - (IN) BCM field qualifier id.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *   The assumption is that all qualifiers are less then 128 bit.
 */
int
_bcm_field_th_qualifier_delete(int unit, bcm_field_entry_t entry,
                               bcm_field_qualify_t qual_id)
{
    _bcm_field_qual_data_t    q_data;  /* Qualifier match data.     */
    _bcm_field_qual_data_t    q_mask;  /* Qualifier match mask.     */
    int                       rv;      /* Operation return status.  */

    sal_memset(q_data, 0, sizeof(_bcm_field_qual_data_t));
    sal_memset(q_mask, 0, sizeof(_bcm_field_qual_data_t));

    if (_BCM_FIELD_IS_PRESEL_ENTRY(entry) == TRUE) {
       rv = _bcm_field_presel_qualify_set(unit, entry, qual_id,
                                          q_data, q_mask);
    } else {
       rv = _bcm_field_th_qualify_set(unit, entry, qual_id,
                                      q_data, q_mask);
    }

    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
              "Qualifier[%s] delete failed for Entry:%d (0x%x)\n\r"),
               _field_qual_name(qual_id), entry, entry));
    }
    return (rv);
}

/*
 * Function:
 *     _bcm_field_th_lt_tcam_data_mem_get
 *
 * Purpose:
 *     Retrieve Preselector tcam & data memories for a specific pipeline stage.
 *
 * Parameters:
 *     unit       -  (IN)  BCM device number.
 *     stage_fc   -  (IN)  Reference to Field Stage Control structure.
 *     instance   -  (IN)  Pipe Instance.
 *     mem_type   -  (IN)  Type of Memory (_BCM_FIELD_MEM_TYPE_xx).
 *     view_type  -  (IN)  Memory View type (_BCM_FIELD_MEM_VIEW_TYPE_xx).
 *     tcam_mem   -  (OUT) Reference to TCAM Memory id.
 *     data_mem   -  (OUT) Reference to DATA Memory id.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_th_lt_tcam_policy_mem_get(int unit, _field_stage_t *stage_fc,
                                     int instance, int mem_type, int view_type,
                                     soc_mem_t *tcam_mem, soc_mem_t *data_mem)
{
    soc_mem_t ifp_lt_tcam[_FP_MAX_NUM_PIPES+1] = /* IFP LT TCAM memory names. */
        {
            IFP_LOGICAL_TABLE_SELECT_TCAM_ONLY_PIPE0m,
            IFP_LOGICAL_TABLE_SELECT_TCAM_ONLY_PIPE1m,
            IFP_LOGICAL_TABLE_SELECT_TCAM_ONLY_PIPE2m,
            IFP_LOGICAL_TABLE_SELECT_TCAM_ONLY_PIPE3m,
            IFP_LOGICAL_TABLE_SELECT_TCAM_ONLYm
        };

    soc_mem_t ifp_lt_data[_FP_MAX_NUM_PIPES+1] = /* IFP LT Data memory names. */
        {
            IFP_LOGICAL_TABLE_SELECT_DATA_ONLY_PIPE0m,
            IFP_LOGICAL_TABLE_SELECT_DATA_ONLY_PIPE1m,
            IFP_LOGICAL_TABLE_SELECT_DATA_ONLY_PIPE2m,
            IFP_LOGICAL_TABLE_SELECT_DATA_ONLY_PIPE3m,
            IFP_LOGICAL_TABLE_SELECT_DATA_ONLYm
        };

    soc_mem_t ifp_lt_tcam_data[_FP_MAX_NUM_PIPES+1] = /* IFP LT TCAM-Data memory names. */
        {
            IFP_LOGICAL_TABLE_SELECT_PIPE0m,
            IFP_LOGICAL_TABLE_SELECT_PIPE1m,
            IFP_LOGICAL_TABLE_SELECT_PIPE2m,
            IFP_LOGICAL_TABLE_SELECT_PIPE3m,
            IFP_LOGICAL_TABLE_SELECTm
        };
    soc_mem_t em_lt_tcam[_FP_MAX_NUM_PIPES+1] = /* EM LT TCAM memory names. */
        {
            EXACT_MATCH_LOGICAL_TABLE_SELECT_TCAM_ONLY_PIPE0m,
            EXACT_MATCH_LOGICAL_TABLE_SELECT_TCAM_ONLY_PIPE1m,
            EXACT_MATCH_LOGICAL_TABLE_SELECT_TCAM_ONLY_PIPE2m,
            EXACT_MATCH_LOGICAL_TABLE_SELECT_TCAM_ONLY_PIPE3m,
            EXACT_MATCH_LOGICAL_TABLE_SELECT_TCAM_ONLYm
        };

    soc_mem_t em_lt_data[_FP_MAX_NUM_PIPES+1] = /* EM LT Data memory names. */
        {
            EXACT_MATCH_LOGICAL_TABLE_SELECT_DATA_ONLY_PIPE0m,
            EXACT_MATCH_LOGICAL_TABLE_SELECT_DATA_ONLY_PIPE1m,
            EXACT_MATCH_LOGICAL_TABLE_SELECT_DATA_ONLY_PIPE2m,
            EXACT_MATCH_LOGICAL_TABLE_SELECT_DATA_ONLY_PIPE3m,
            EXACT_MATCH_LOGICAL_TABLE_SELECT_DATA_ONLYm
        };

    soc_mem_t em_lt_tcam_data[_FP_MAX_NUM_PIPES+1] = /* EM LT TCAM-Data memory names. */
        {
            EXACT_MATCH_LOGICAL_TABLE_SELECT_PIPE0m,
            EXACT_MATCH_LOGICAL_TABLE_SELECT_PIPE1m,
            EXACT_MATCH_LOGICAL_TABLE_SELECT_PIPE2m,
            EXACT_MATCH_LOGICAL_TABLE_SELECT_PIPE3m,
            EXACT_MATCH_LOGICAL_TABLE_SELECTm
        };

    soc_mem_t *tcam_ptr, *data_ptr, *comb_ptr;

    /* Input parameters check. */
    if (NULL == stage_fc) {
        return (BCM_E_PARAM);
    }

    if ((((view_type & _BCM_FIELD_MEM_VIEW_TYPE_TCAM) ||
          (view_type & _BCM_FIELD_MEM_VIEW_TYPE_TCAM_DATA_COMB)) &&
         (tcam_mem == NULL)) ||
        ((view_type & _BCM_FIELD_MEM_VIEW_TYPE_DATA) &&
         (data_mem == NULL))) {
        return (BCM_E_PARAM);
    }

    /* Validate instance */
    if ((instance < 0) || (instance >= _FP_MAX_NUM_PIPES)) {
        return (BCM_E_PARAM);
    }

    switch (mem_type) {
       case _BCM_FIELD_MEM_TYPE_IFP_LT:
            if (stage_fc->stage_id != _BCM_FIELD_STAGE_INGRESS) {
               return BCM_E_PARAM;
            }
            tcam_ptr = ifp_lt_tcam;
            data_ptr = ifp_lt_data;
            comb_ptr = ifp_lt_tcam_data;
            break;

       case _BCM_FIELD_MEM_TYPE_EM_LT:
            if (stage_fc->stage_id != _BCM_FIELD_STAGE_EXACTMATCH) {
               return BCM_E_PARAM;
            }
            tcam_ptr = em_lt_tcam;
            data_ptr = em_lt_data;
            comb_ptr = em_lt_tcam_data;
            break;

       default:
            return BCM_E_PARAM;
    }

    if (stage_fc->oper_mode == bcmFieldGroupOperModeGlobal) {
       instance = _FP_MAX_NUM_PIPES;         
    }

    switch (view_type) {
       case _BCM_FIELD_MEM_VIEW_TYPE_TCAM:
           *tcam_mem = tcam_ptr[instance];
           break;
       case _BCM_FIELD_MEM_VIEW_TYPE_DATA:
           *data_mem = data_ptr[instance];
           break;
       case (_BCM_FIELD_MEM_VIEW_TYPE_TCAM |
             _BCM_FIELD_MEM_VIEW_TYPE_DATA):
           *tcam_mem = tcam_ptr[instance];
           *data_mem = data_ptr[instance];
           break;
       case _BCM_FIELD_MEM_VIEW_TYPE_TCAM_DATA_COMB:
           *tcam_mem = comb_ptr[instance];
           break;
       default:
           return (BCM_E_PARAM);
    } 
           
    return (BCM_E_NONE);
}

#if defined (BROADCOM_DEBUG)
/*
 * Function:
 *     _bcm_field_th_entry_phys_dump
 *
 * Purpose:
 *     Show contents of a physical entry structure
 *
 * Parameters:
 *     unit       - BCM device number
 *     f_ent      - Physical entry to dump
 * Returns:
 *     Nothing.
 */
int
_bcm_field_th_entry_phys_dump(int unit, _field_entry_t *f_ent)
{
    int                       idx;
    int                       qual_idx;
    uint32                    width;
    _field_group_t            *fg;
    _bcm_field_qual_data_t    q_data;
    _bcm_field_qual_data_t    q_mask;
    _bcm_field_qual_offset_t  q_offset;
    int                       rv, i;
    int                       entry_parts = 0;
    int                       part_ct;
    _bcm_field_qual_data_t    local_data;
    _bcm_field_qual_data_t    local_mask;
    uint32                    data = 0;
    uint32                    mask = 0;
    int                       start_offset = 0;
    /* Input validation check */
    if (f_ent == NULL) {
       return BCM_E_PARAM;
    }
    /* Get number of parts in field entry based on group flags. */
    rv = _bcm_field_th_entry_tcam_parts_count (unit, f_ent->group->flags,
                                               &entry_parts);
    BCM_IF_ERROR_RETURN(rv);

    LOG_CLI((BSL_META_U(unit,
            "         slice=%d, slice_idx=%#x, part =%d prio=%#x, flags=%#x, "),
             f_ent->fs->slice_number, f_ent->slice_idx, entry_parts,
             f_ent->prio, f_ent->flags));
    if (!(f_ent->flags & _FP_ENTRY_INSTALLED)) {
        LOG_CLI((BSL_META_U(unit,
                            "Not installed\n")));
    } else {
        LOG_CLI((BSL_META_U(unit,
                            "Installed, ")));

        if (!(f_ent->flags & _FP_ENTRY_ENABLED)) {
            LOG_CLI((BSL_META_U(unit,
                                "Disabled")));
        }
        else {
            LOG_CLI((BSL_META_U(unit,
                                "Enabled")));
        }
        if (f_ent->flags & _FP_ENTRY_DIRTY) {
            LOG_CLI((BSL_META_U(unit,
                                ", Dirty")));
        }
        else {
             LOG_CLI((BSL_META_U(unit,
                                 "\n")));
        }
    }

    rv = _bcm_field_qual_tcam_key_mask_get(unit, f_ent);
    if (BCM_FAILURE(rv)) {
        LOG_CLI((BSL_META_U(unit,
                            "\nUnit (%d) Entry (%d) tcam key read failure.\n"),
                 unit, f_ent->eid));
        return BCM_E_INTERNAL;
    }

    LOG_CLI((BSL_META_U(unit,
                        "              tcam: color_indep=%d, "),
             f_ent->flags & _FP_ENTRY_COLOR_INDEPENDENT));



    fg = f_ent->group;

    for (qual_idx = 0; qual_idx < _bcmFieldQualifyCount; qual_idx++) {

        if (0 == BCM_FIELD_QSET_TEST(fg->qset, qual_idx)) {
           continue;
        }

        _FP_QUAL_DATA_CLEAR(q_data);
        _FP_QUAL_DATA_CLEAR(q_mask);
        start_offset = 0;
        width = 0;
        LOG_CLI((BSL_META_U(unit,
                                "\n %s "),
                     _field_qual_name(qual_idx)));
        for (part_ct = 0; part_ct < entry_parts; part_ct++) {
            /* Initialize q_offset to NULL */
            sal_memset(&q_offset, 0x0, sizeof(q_offset));

            _FP_QUAL_DATA_CLEAR(local_data);
            _FP_QUAL_DATA_CLEAR(local_mask);
            _FP_QUAL_DATA_CLEAR(q_data);
            _FP_QUAL_DATA_CLEAR(q_mask);
            /* Retrieve the Qualifier offsets for the particular entry part */
            rv = _bcm_field_th_qual_part_offset_get(unit, f_ent, part_ct,
                                                    qual_idx, &q_offset);
            if (rv == BCM_E_NOT_FOUND) {
               continue;
            } else if (BCM_FAILURE(rv)) {
               return rv;
            }
            /* Combined width of a qualifier for a given part */
            _BCM_FIELD_QUAL_MULTI_OFFSET_WIDTH(&q_offset, width);
            if (width > 0) {
                /* Read qualifier match value and mask. */
                rv = _bcm_field_qual_value_get(unit, &q_offset, f_ent + part_ct,
                                               local_data, local_mask);
                BCM_IF_ERROR_RETURN(rv);
            } else {
                continue;
            }

            /* Adjust Data and Mask based on the part offsets. */
            rv = _bcm_field_qual_partial_data_set(local_data, start_offset,
                                                  width, q_data);
            BCM_IF_ERROR_RETURN(rv);
            rv = _bcm_field_qual_partial_data_set(local_mask, start_offset,
                                                  width, q_mask);
            BCM_IF_ERROR_RETURN(rv);
            /* Increment the offset for the next part */
            start_offset += width;

            for (i = 0; i < q_offset.num_offsets; i++) {
               if (0 != q_offset.width[i]) {
                 int delta = 0;
                 if ((fg->flags & _FP_GROUP_SPAN_SINGLE_SLICE) &&
                     !(fg->flags & _FP_GROUP_INTRASLICE_DOUBLEWIDE)) {
                    delta = 80;
                 }
                  LOG_CLI((BSL_META_U(unit,
                                "\n    Part:%d Offset%d: %2d Width%d: %3d "),
                                part_ct, i, q_offset.offset[i]+delta, i,
                                q_offset.width[i]));
               }
            }

            LOG_CLI((BSL_META_U(unit,
                                "\n    DATA=0x")));
            for (idx = 3; idx >= 0; idx--) {
                data = q_data[idx];
                mask = q_mask[idx];
                if (data == 0 && mask == 0) {
                  continue;
                }
               /* Check if HW packet_res must be remmaped for API use */
               if (bcmFieldQualifyPacketRes == qual_idx) {
                  _bcm_field_td2_qualify_PacketRes_get(unit,
                                      f_ent->eid, &data, &mask);
                  q_mask[idx] = mask;
               }
               LOG_CLI((BSL_META_U(unit,
                                   "%08x "), data));
            }

            LOG_CLI((BSL_META_U(unit,
                                "\n    MASK=0x")));
            for (idx = 3; idx >= 0; idx--) {
                data = q_data[idx];
                mask = q_mask[idx];
                if (data == 0 && mask == 0) {
                  continue;
                }
                LOG_CLI((BSL_META_U(unit,
                                    "%08x "), q_mask[idx]));
            }
        }
    }

    LOG_CLI((BSL_META_U(unit,
                        "\n")));
    return BCM_E_NONE;
}
#endif /* (BROADCOM_DEBUG) */

#ifdef BCM_WARM_BOOT_SUPPORT

              /*  START OF WARMBOOT ROUTINES */
/*
 * Function:
 *     _bcm_field_th_scache_sync
 *
 * Purpose:
 *     Save field module software state to external cache.
 *
 * Parameters:
 *     unit             - (IN) BCM device number
 *     fc               - (IN) Pointer to device field control structure
 *     stage_fc         - (IN) FP stage control info.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_th_scache_sync(int              unit,
                           _field_control_t *fc,
                           _field_stage_t   *stage_fc
                           )
{
    int                   idx;             /* Index to field entry.          */
    int                   rv = BCM_E_NONE; /* Operation return value.        */
    int                   efp_slice_mode;  /* Slice mode of EFP.             */
    int                   paired = 0;      /* TCAM paired status of a slice. */
    int                   ratio = 0;       /* TCAM entry ratio.              */
    int                   slice_idx;       /* Index of a slice in (V/I/E)FP  */
    uint8                 *buf;            /* Pointer to 1st scache part.    */
    uint8                 *buf1;           /* Pointer to 2nd scache part.    */
    uint32                start_char;      /* Start character for (V/I/E)FP  */
    uint32                end_char;        /* End caharacter for (V/I/E)FP.  */
    uint32                val;             /* Carry Register contents.       */
    uint64                rval;            /* Carry register contents.       */
    _field_slice_t        *fs;             /* Slice pointer.                 */
    _field_group_t        *fg;             /* Filed group pointer.           */
    _field_data_control_t *data_ctrl;      /* UDF control data pointer.      */
    _field_entry_t        *f_ent;          /* Pointer to field entry.        */
    soc_field_t           fld;             /* field in any memory/register.  */

    soc_field_t _efp_slice_mode[] = {SLICE_0_MODEf, SLICE_1_MODEf,
                                     SLICE_2_MODEf, SLICE_3_MODEf};

    buf = fc->scache_ptr[_FIELD_SCACHE_PART_0];
    buf1 = fc->scache_ptr[_FIELD_SCACHE_PART_1];

    switch (stage_fc->stage_id) {
        case _BCM_FIELD_STAGE_INGRESS:
            start_char = _FIELD_IFP_DATA_START;
            end_char = _FIELD_IFP_DATA_END;
            break;
        case _BCM_FIELD_STAGE_EGRESS:
            start_char = _FIELD_EFP_DATA_START;
            end_char = _FIELD_EFP_DATA_END;
            break;
        case _BCM_FIELD_STAGE_LOOKUP:
            start_char = _FIELD_VFP_DATA_START;
            end_char = _FIELD_VFP_DATA_END;
            break;
        case _BCM_FIELD_STAGE_EXTERNAL:
            /*
            if (_field_tr2_ext_scache_sync_chk(unit, fc, stage_fc)) {
                return (_field_tr2_ext_scache_sync(unit, fc, stage_fc));
            } */
            start_char = _FIELD_EXTFP_DATA_START;
            end_char   = _FIELD_EXTFP_DATA_END;
            break;
        default:
            return BCM_E_PARAM;
    }

    LOG_DEBUG(BSL_LS_BCM_FP,
              (BSL_META_U(unit,
                          "FP(unit %d): _bcm_field_th_scache_sync() - "
                          "Synching scache for FP stage %d...\n"),
               unit, stage_fc->stage_id));

   if (stage_fc->stage_id != _BCM_FIELD_STAGE_INGRESS) {

    _field_scache_stage_hdr_save(fc, start_char);

   }

    if (stage_fc->stage_id == _BCM_FIELD_STAGE_INGRESS) {

       BCM_IF_ERROR_RETURN(_bcm_field_th_ingress_scache_sync(unit,
                                                             stage_fc));
   
        LOG_DEBUG(BSL_LS_BCM_FP,
                  (BSL_META_U(unit,
                              "FP(unit %d): _bcm_field_th_scache_sync() -"
                              " Writing end of section @ byte %d.\n"),
                   unit, fc->scache_pos));

        /* Mark the end of the IFP section */
        buf[fc->scache_pos] = end_char & 0xFF;
        fc->scache_pos++;
        buf[fc->scache_pos] = (end_char >> 8) & 0xFF;
        fc->scache_pos++;
        buf[fc->scache_pos] = (end_char >> 16) & 0xFF;
        fc->scache_pos++;
        buf[fc->scache_pos] = (end_char >> 24) & 0xFF;
        fc->scache_pos++;
        fc->scache_usage = fc->scache_pos; /* Usage in bytes */

        return BCM_E_NONE;
    }

    /* Save data qualifiers */
    if ((data_ctrl = stage_fc->data_ctrl) != 0) {
        if (soc_mem_is_valid(unit, FP_UDF_TCAMm)) {
            /* Device has UDF TCAM =>
               Save internal information regarding TCAM entry usage
            */

            _field_data_tcam_entry_t *p;
            unsigned                 n;

            for (p = data_ctrl->tcam_entry_arr,
                     n = soc_mem_index_count(unit, FP_UDF_TCAMm);
                 n;
                 --n, ++p
                 ) {
                buf[fc->scache_pos] = p->ref_count;
                fc->scache_pos++;
            }
        }

        _field_scache_sync_data_quals_write(fc, data_ctrl);
    }

    _field_slice_map_write(unit, fc, stage_fc);

    for (slice_idx = 0; slice_idx < stage_fc->tcam_slices; slice_idx++) {
        LOG_DEBUG(BSL_LS_BCM_FP,
                  (BSL_META_U(unit,
                              "FP(unit %d): _bcm_field_th_scache_sync() - "
                              "Checking slice %d...\n"),
                   unit, slice_idx));
        /* Skip slices without groups */
        fs = stage_fc->slices[_FP_DEF_INST] + slice_idx;
        fg = fc->groups;
        while (fg != NULL) {
            if (fg->stage_id != stage_fc->stage_id) {
                fg = fg->next;
                continue; /* Not in this stage */
            }
            if (fg->slices[0].slice_number == slice_idx) {
                break;
            }
            fg = fg->next;
        }

        if (fg == NULL) {
            continue; /* No group found */
        }

        /* Also skip expanded slices */
        if (stage_fc->slices[_FP_DEF_INST][slice_idx].prev != NULL) {
            continue;
        }

        /* Ignore secondary slice in paired mode */
        switch (stage_fc->stage_id) {
            case _BCM_FIELD_STAGE_EGRESS:
                BCM_IF_ERROR_RETURN(READ_EFP_SLICE_CONTROLr(unit, &val));
                efp_slice_mode = soc_reg_field_get(unit,
                                     EFP_SLICE_CONTROLr,
                                     val,
                                     _efp_slice_mode[slice_idx]);
                if ((efp_slice_mode == _BCM_FIELD_EGRESS_SLICE_MODE_DOUBLE)
                    || (efp_slice_mode
                    == _BCM_FIELD_EGRESS_SLICE_MODE_DOUBLE_L3_ANY)
                    || (efp_slice_mode
                    == _BCM_FIELD_EGRESS_SLICE_MODE_DOUBLE_L3_V6)) {
                    paired = 1;
                }
                break;
            case _BCM_FIELD_STAGE_LOOKUP:
                BCM_IF_ERROR_RETURN(READ_VFP_KEY_CONTROL_1r(unit, &rval));
                fld = _bcm_field_trx_slice_pairing_field[slice_idx / 2];
                paired = soc_reg64_field32_get(unit,
                            VFP_KEY_CONTROL_1r,
                            rval,
                            fld);
                break;
            default:
                return BCM_E_PARAM;
                break;
        }
        if (paired && (slice_idx % 2)) {
            continue;
        }
        BCM_IF_ERROR_RETURN
            (_field_tr2_group_entry_write(unit,
                slice_idx,
                fs,
                fc,
                stage_fc));
    }

    /* Now sync the expanded slices */
    for (slice_idx = 0; slice_idx < stage_fc->tcam_slices; slice_idx++) {
        fs = stage_fc->slices[_FP_DEF_INST] + slice_idx;
        /* Skip empty slices */
        if (fs->entry_count == fs->free_count) {
            LOG_DEBUG(BSL_LS_BCM_FP,
                      (BSL_META_U(unit,
                                  "FP(unit %d): _bcm_field_th_scache_sync() - "
                                  "Slice is empty.\n"),
                       unit));
            continue;
        }

        /* Skip master slices */
        if (stage_fc->slices[_FP_DEF_INST][slice_idx].prev == NULL) {
            continue;
        }

        /*
         * Skip expanded slices with no entries installed in Hw
         * to match recovery logic.
         */
        if (fs->group_flags & _FP_GROUP_INTRASLICE_DOUBLEWIDE) {
            ratio = 2;
        } else {
            ratio = 1;
        }

        for (idx = 0; idx < fs->entry_count / ratio; idx++) {
            /* Find EID that matches this HW index */
            f_ent = fs->entries[idx];
            if (f_ent == NULL) {
                continue;
            }
            if (!(f_ent->flags & _FP_ENTRY_INSTALLED)) {
                continue;
            }
            break;
        }

        if (idx == (fs->entry_count / ratio)) {
            continue;
        }

        /* Ignore secondary slice in paired mode */
        switch (stage_fc->stage_id) {
            case _BCM_FIELD_STAGE_EGRESS:
                BCM_IF_ERROR_RETURN(READ_EFP_SLICE_CONTROLr(unit, &val));
                efp_slice_mode = soc_reg_field_get(unit,
                                    EFP_SLICE_CONTROLr,
                                    val,
                                    _efp_slice_mode[slice_idx]);
                if ((efp_slice_mode == _BCM_FIELD_EGRESS_SLICE_MODE_DOUBLE)
                    || (efp_slice_mode
                    == _BCM_FIELD_EGRESS_SLICE_MODE_DOUBLE_L3_ANY)
                    || (efp_slice_mode
                    == _BCM_FIELD_EGRESS_SLICE_MODE_DOUBLE_L3_V6)) {
                    paired = 1;
                }
                break;
            case _BCM_FIELD_STAGE_LOOKUP:
                BCM_IF_ERROR_RETURN(READ_VFP_KEY_CONTROL_1r(unit, &rval));
                fld = _bcm_field_trx_slice_pairing_field[slice_idx / 2];
                paired = soc_reg64_field32_get(unit,
                            VFP_KEY_CONTROL_1r,
                            rval,
                            fld);
                break;
            default:
                return BCM_E_PARAM;
                break;
        }
        if (paired && (slice_idx % 2)) {
            continue;
        }
        BCM_IF_ERROR_RETURN
            (_field_tr2_group_entry_write(unit, slice_idx, fs, fc, stage_fc));
    }

    LOG_DEBUG(BSL_LS_BCM_FP,
              (BSL_META_U(unit,
                          "FP(unit %d): _bcm_field_th_scache_sync() -"
                          " Writing end of section @ byte %d.\n"),
               unit, fc->scache_pos));

    /* Mark the end of the section */
    buf[fc->scache_pos] = end_char & 0xFF;
    fc->scache_pos++;
    buf[fc->scache_pos] = (end_char >> 8) & 0xFF;
    fc->scache_pos++;
    buf[fc->scache_pos] = (end_char >> 16) & 0xFF;
    fc->scache_pos++;
    buf[fc->scache_pos] = (end_char >> 24) & 0xFF;
    fc->scache_pos++;
    fc->scache_usage = fc->scache_pos; /* Usage in bytes */

    if(NULL != buf1) {
        /* Mark the end of the section */
        buf1[fc->scache_pos1] = end_char & 0xFF;
        fc->scache_pos1++;
        buf1[fc->scache_pos1] = (end_char >> 8) & 0xFF;
        fc->scache_pos1++;
        buf1[fc->scache_pos1] = (end_char >> 16) & 0xFF;
        fc->scache_pos1++;
        buf1[fc->scache_pos1] = (end_char >> 24) & 0xFF;
        fc->scache_pos1++;
    }

    return rv;
}
/*
 * Function:
 *     _bcm_field_th_group_construct_quals_add
 *
 * Purpose:
 *     Recover all qualifiers for the given group, based on its selector codes
 *
 * Parameters:
 *     unit             - (IN) BCM device number
 *     fc               - (IN) Device field control structure pointer
 *     stage_fc         - (IN) FP stage control info
 *     fg               - (IN/OUT) Select code information filled into the group
 *     entry_type       - (IN) EFP entry type
 *
 * Returns:
 *     BCM_E_NONE
 */
int
_bcm_field_th_group_construct_quals_add(int unit,
                                         _field_control_t *fc,
                                         _field_stage_t *stage_fc,
                                         _field_group_t *fg,
                                         uint8 entry_type)
{
    int rv;        /* Operation return value.       */
    int parts_cnt; /* Parts count in a field group. */
    int part_idx;  /* Part index in a field group.  */

    if ((!fc) || (!stage_fc) || (!fg)) {
        return(BCM_E_INTERNAL);
    }

    rv = _bcm_field_entry_tcam_parts_count(unit, fg->stage_id,
                                           fg->flags, &parts_cnt);
    if (BCM_FAILURE(rv)) {
        return (rv);
    }

    for (part_idx = 0; part_idx < parts_cnt; ++part_idx) {
        _bcm_field_group_qual_t *grp_qual = &fg->qual_arr[entry_type][part_idx];
        unsigned qid;

        for (qid = 0; qid < _bcmFieldQualifyCount; ++qid) {
            _bcm_field_qual_info_t *f_qual_arr = stage_fc->f_qual_arr[qid];
            unsigned               j;
            uint8                  diff_cnt;

            if (f_qual_arr == NULL) {
                continue; /* Qualifier does not exist in this stage */
            }

            if (fc->l2warm && !BCM_FIELD_QSET_TEST(fg->qset, qid)) {
                continue; /* Qualifier not present in the group */
            }

            /* Add all of the stage's qualifiers that match the recovered
               selector codes.  Qualifiers that appear more than once
               (because more than one configuration of a qualifier matches
               the recovered selector codes) will be cleaned up later.
            */

            for (j = 0; j < f_qual_arr->conf_sz; j++) {
                if (_field_selector_diff(unit,
                                         fg->sel_codes,
                                         part_idx,
                                         &f_qual_arr->conf_arr[j].selector,
                                         &diff_cnt
                                         )
                    != BCM_E_NONE
                    || diff_cnt != 0
                    ) {
                    continue;
                }

                if (!fc->l2warm)  {
                    BCM_FIELD_QSET_ADD(fg->qset, qid);
                }

                _field_trx_group_qual_add(grp_qual,
                                          qid,
                                          &f_qual_arr->conf_arr[j].offset
                                          );

                _field_qset_udf_bmap_reinit(unit,
                                            stage_fc,
                                            &fg->qset,
                                            qid
                                            );
            }
        }
    }

    return (BCM_E_NONE);
}
/*
 * Function:
 *  _bcm_field_th_stage_egress_reinit
 * Description:
 *  Service routine used to retain the software sate of
 *  EFP(Egress Field Processor) configuration .
 * Parameters:
 *   unit     - (IN) BCM device number.
 *   fc       - (IN) Field control structure.
 *   stage_fc - (IN/OUT) Stage field control structure.
 * Returns:
 *  BCM_E_XXX
 * Notes:
 *  None.
 */
int
_bcm_field_th_stage_egress_reinit(int unit, _field_control_t *fc,
                               _field_stage_t *stage_fc)
{
    char *efp_policy_buf = NULL;  /* Buffer to read the EFP_POLICY table      */
    int  idx;                     /* Index to field entry.                    */
    int  idx1;
    int slice_idx;                /* Slice index in (V/I/E)FP.                */
    int vslice_idx;               /* Slice index in a virtual group.          */
    int index_min;                /* Min index of a memory.                   */
    int index_max;                /* Max index of a field memory.             */
    int rv = BCM_E_NONE;          /* Operation status.                        */
    int mem_sz;                   /* Size of the memory.                      */
    int slice_ent_cnt;            /* Entry count in a slice.                  */
    int parts_count = 1;          /* Parts count in a field group.            */
    int i;                        /* Used to loop through parts i
                                     in a field group.                        */
    int pri_tcam_idx;             /* Priority of an entry in TCAM.            */
    int part_index;               /* Index to part(s) in a field group.       */
    int slice_number;             /* (V/I/E) slice number.                    */
    int phys_tcam_idx;            /* Index to physical TCAM in (V/I/E)FP.     */
    int prio;                     /* Priority of current field entry.         */
    int prev_prio;                /* Priority of previous field entry.        */
    int expanded[4];              /* Expansion status of slices in (V/I/E)FP. */
    int slice_master_idx[4];      /* Expansion status of slices in (V/I/E)FP. */
    int priority;                 /* Priority of a field entry.               */
    int multigroup;               /* Field group's multigroup status.         */
    int max;                      /*Temporary variable.                       */
    int master_slice;             /* Master slice number for expanded group.  */
    uint8 *buf = NULL;            /* Pointer to 1st scache part.              */
    uint8 *buf1 = NULL;           /* Pointer to 2nd scache part.              */
    uint8 old_physical_slice;     /* Physical slice number.                   */
    uint8 slice_num;              /* Physical slice number.                   */
    uint16 default_key_1;         /* Default selcode of a field group in EFP. */
    uint16 default_key_2;         /* Default selcode of a field group in EFP. */
    uint32 *efp_tcam_buf = NULL;  /* Buffer to read the EFP_TCAM table.       */
    uint32 rval;                  /* Holds conetets of register.              */
    uint32 efp_slice_mode;        /* Mode of a slice in EFP.                  */
    uint32 efp_slice_ipv6_mode;   /* IPv6 mode of a slice in EFP.             */
    uint32 temp;                  /* Temporary varibale.                      */
    uint32 key_match_type[2];     /* Key match types of field group in EFP.   */
    uint32 entry_type = 0;        /* Type of entry of filed group in EFP.     */
    uint32 entry_flags;           /* Flags set for a entry in field group.    */
    _field_slice_t *fs = NULL;    /* Slice pointer.                           */
    _field_group_t *fg = NULL;    /* Filed group pointer.                     */
    _field_entry_t *f_ent = NULL; /* Field entry pointer.                     */
    bcm_field_group_t gid;        /* Field group ID.                          */
    bcm_field_entry_t eid;        /* Field entry ID.                          */
    bcm_field_stat_t sid;         /* Field stat ID.                           */
    bcm_policer_t pid;            /* Field policer ID.                        */
    bcm_field_qset_t qset;        /* QSET of field group.                     */
    bcm_pbmp_t all_pbmp;          /* Port Bit Map.                            */
    int group_flags = 0;
    uint8 dvp_type;
    int action_res_id;
    uint16 version = 0;
    _field_entry_wb_t *f_ent_wb = NULL;
    _field_slice_t *fs_temp = NULL;    /* Slice pointer.                           */
    efp_tcam_entry_t *efp_tcam_entry = NULL;           /* Buffer to hold
                                                          contents of entry in
                                                          EFP_TCAM entry. */
    efp_policy_table_entry_t *efp_policy_entry = NULL; /* Buffer to hold
                                                          contetnts of entry in
                                                          EFP_POLICY_TABLE. */


    soc_field_t efp_en_flds[4] = {SLICE_ENABLE_SLICE_0f, SLICE_ENABLE_SLICE_1f,
                                  SLICE_ENABLE_SLICE_2f, SLICE_ENABLE_SLICE_3f};

    soc_field_t efp_lk_en_flds[4] =
                      {LOOKUP_ENABLE_SLICE_0f, LOOKUP_ENABLE_SLICE_1f,
                       LOOKUP_ENABLE_SLICE_2f, LOOKUP_ENABLE_SLICE_3f};

    buf = fc->scache_ptr[_FIELD_SCACHE_PART_0];
    buf1 = fc->scache_ptr[_FIELD_SCACHE_PART_1];

    fc->scache_pos1 = 0;
    fc->scache_pos1 += SOC_WB_SCACHE_CONTROL_SIZE;

    sal_memset(expanded, 0, 4 * sizeof(int));
    sal_memset(slice_master_idx, 0, 4 * sizeof(int));

    if (fc->l2warm) {
        rv = _field_scache_stage_hdr_chk(fc, _FIELD_EFP_DATA_START);
        if (BCM_FAILURE(rv)) {
            return (rv);
        }
        sal_memcpy(&version, fc->scache_ptr[_FIELD_SCACHE_PART_0], sizeof(uint16));
    }

    /* DMA various tables */
    efp_tcam_buf = soc_cm_salloc(unit, sizeof(efp_tcam_entry_t) *
                                 soc_mem_index_count(unit, EFP_TCAMm),
                                 "EFP TCAM buffer");
    if (NULL == efp_tcam_buf) {
        return BCM_E_MEMORY;
    }

    sal_memset(efp_tcam_buf, 0, sizeof(efp_tcam_entry_t) *
               soc_mem_index_count(unit, EFP_TCAMm));
    index_min = soc_mem_index_min(unit, EFP_TCAMm);
    index_max = soc_mem_index_max(unit, EFP_TCAMm);
    fs = stage_fc->slices[_FP_DEF_INST];
    if (stage_fc->flags & _FP_STAGE_HALF_SLICE) {
        slice_ent_cnt = fs->entry_count * 2;
        /* DMA in chunks */
        for (slice_idx = 0; slice_idx < stage_fc->tcam_slices; slice_idx++) {
            fs = stage_fc->slices[_FP_DEF_INST] + slice_idx;
            if ((rv = soc_mem_read_range(unit, EFP_TCAMm, MEM_BLOCK_ALL,
                                         slice_idx * slice_ent_cnt,
                                         slice_idx * slice_ent_cnt +
                                             fs->entry_count - 1,
                                         efp_tcam_buf + slice_idx *
                                             slice_ent_cnt *
                                  soc_mem_entry_words(unit, EFP_TCAMm))) < 0 ) {
                goto cleanup;
            }
        }
    } else {
        slice_ent_cnt = fs->entry_count;
        if ((rv = soc_mem_read_range(unit, EFP_TCAMm, MEM_BLOCK_ALL,
                                     index_min, index_max,
                                     efp_tcam_buf)) < 0 ) {
            goto cleanup;
        }
    }
    efp_policy_buf = soc_cm_salloc(unit, SOC_MEM_TABLE_BYTES
                                  (unit, EFP_POLICY_TABLEm),
                                  "EFP POLICY TABLE buffer");
    if (NULL == efp_policy_buf) {
        return BCM_E_MEMORY;
    }
    index_min = soc_mem_index_min(unit, EFP_POLICY_TABLEm);
    index_max = soc_mem_index_max(unit, EFP_POLICY_TABLEm);
    if ((rv = soc_mem_read_range(unit, EFP_POLICY_TABLEm, MEM_BLOCK_ALL,
                                 index_min, index_max,
                                 efp_policy_buf)) < 0 ) {
        goto cleanup;
    }

    /* Get slice expansion status and virtual map */
    if ((rv = _field_slice_expanded_status_get(unit, fc, stage_fc, expanded, slice_master_idx)) < 0) {
        goto cleanup;
    }

    /* Iterate over the slices */
    if ((rv = READ_EFP_SLICE_CONTROLr(unit, &rval)) < 0) {
        goto cleanup;
    }

    for (slice_idx = 0; slice_idx < stage_fc->tcam_slices; slice_idx++) {
        /* Ignore disabled slice */
        if ((soc_reg_field_get(unit, EFP_SLICE_CONTROLr, rval,
                               efp_en_flds[slice_idx]) == 0) ||
            (soc_reg_field_get(unit, EFP_SLICE_CONTROLr, rval,
                               efp_lk_en_flds[slice_idx]) == 0)) {
            continue;
        }

        efp_slice_mode = soc_reg_field_get(unit, EFP_SLICE_CONTROLr, rval,
                                           _th_efp_slice_mode[slice_idx][0]);
        efp_slice_ipv6_mode = soc_reg_field_get(unit, EFP_SLICE_CONTROLr, rval,
                                           _th_efp_slice_mode[slice_idx][1]);

        /* Skip if slice has no valid groups and entries */
        fs = stage_fc->slices[_FP_DEF_INST] + slice_idx;
        key_match_type[0] = ~0;
        key_match_type[1] = ~0;

        /* Get KEY_MATCH_TYPE from the valid entries of EFP_TCAM */
        for (idx = 0; idx < slice_ent_cnt; idx++) {
            efp_tcam_entry = soc_mem_table_idx_to_pointer(unit, EFP_TCAMm,
                                                          efp_tcam_entry_t *,
                                 efp_tcam_buf, idx + slice_ent_cnt * slice_idx);

            if (soc_EFP_TCAMm_field32_get(unit, efp_tcam_entry, VALIDf) != 0) {

                /* In Tomahawk, EFP_TCAM entry KEY field
                 *  width is  (240 + 2b VALID)
                 */

                if (key_match_type[0] == ~0) {
                    _field_extract((uint32 *)efp_tcam_entry, 236 + 2, 4,
                                   &key_match_type[0]);
                    continue;
                }

                if (key_match_type[1] == ~0) {
                    _field_extract((uint32 *)efp_tcam_entry, 236 + 2, 4,
                                   &key_match_type[1]);
                    if (key_match_type[0] == key_match_type[1]) {
                        key_match_type[1] = ~0;
                    } else {
                        /* break once possible two key match types are
                         * extracted from the valid entries
                         */
                        break;
                    }
                }
            }
        }

        if (key_match_type[0] == ~0 && !fc->l2warm) {
            continue;
        }

        /* Skip second part of slice pair */
        if (((efp_slice_mode == _BCM_FIELD_EGRESS_SLICE_MODE_DOUBLE) ||
             (efp_slice_mode == _BCM_FIELD_EGRESS_SLICE_MODE_DOUBLE_L3_ANY) ||
             (efp_slice_mode == _BCM_FIELD_EGRESS_SLICE_MODE_DOUBLE_L3_V6)) &&
            (slice_idx % 2)) {
            continue;
        }

        /* Don't need to read selectors for expanded slice */
        if (expanded[slice_idx]) {
            continue;
        }

        fg = NULL;
        if (efp_slice_mode <= 5) { /* Valid values */
            /* If Level 2, retrieve the GIDs in this slice */
            if (fc->l2warm) {
                rv = _field_trx_scache_slice_group_recover(unit,
                                                           fc,
                                                           slice_idx,
                                                           NULL,
                                                           stage_fc,
                                                           0);
                if (BCM_FAILURE(rv) && (rv != BCM_E_NOT_FOUND)) {
                    fc->l2warm = 0;
                    goto cleanup;
                }
                if (rv == BCM_E_NOT_FOUND) {
                    rv = BCM_E_NONE;
                    continue;
                }
            }

            rv = _field_tr2_group_construct_alloc(unit, &fg);
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }

            action_res_id = BCM_FIELD_GROUP_ACTION_RES_ID_DEFAULT;

            group_flags = 0;
            if (fc->l2warm) {
                /* Get stored group ID and QSET for Level 2 */
                rv = _field_group_info_retrieve(unit,
                                                -1,
                                                &gid,
                                                &priority,
                                                &action_res_id,
                                                &group_flags,
                                                &qset,
                                                fc
                                                );
                sal_memcpy(&fg->qset, &qset, sizeof(bcm_field_qset_t));
            } else {
                if ((rv = _bcm_field_group_id_generate(unit, &gid)) == BCM_E_NONE) {
                    unsigned vmap, vslice;

                    for (priority = -1, vmap = 0;
                         priority == -1 && vmap < _FP_VMAP_CNT; ++vmap) {
                        for (vslice = 0; vslice < COUNTOF(stage_fc->vmap[0]);
                             ++vslice) {
                            if (stage_fc->vmap[vmap][vslice].vmap_key ==
                                                                  slice_idx) {
                                priority =
                                          stage_fc->vmap[vmap][vslice].priority;
                                break;
                            }
                        }
                    }

                    if (priority == -1) {
                        rv = BCM_E_INTERNAL;
                    }
                }
            }
            if (BCM_FAILURE(rv)) {
                sal_free(fg);
                goto cleanup;
            }

            fg->gid      = gid;
            fg->priority = slice_idx;
            fg->action_res_id = action_res_id;
            fg->stage_id = stage_fc->stage_id;

            switch (efp_slice_mode) {

                case _BCM_FIELD_EGRESS_SLICE_MODE_SINGLE_L2:
                    fg->flags |= _FP_GROUP_SPAN_SINGLE_SLICE;
                    fg->sel_codes[0].fpf3 = _BCM_FIELD_EFP_KEY4;
                    parts_count = 1;
                    rv = _bcm_field_th_group_construct_quals_add(unit, fc,
                                                                 stage_fc, fg,
                                                              _FP_ENTRY_TYPE_0);
                    if (BCM_FAILURE(rv)) {
                        sal_free(fg);
                        goto cleanup;
                    }

                    rv = _bcm_field_th_group_construct_quals_add(unit, fc,
                                                                 stage_fc, fg,
                                                              _FP_ENTRY_TYPE_1);
                    if (BCM_FAILURE(rv)) {
                        sal_free(fg);
                        goto cleanup;
                    }
                    break;
                case _BCM_FIELD_EGRESS_SLICE_MODE_SINGLE_L3:

                    fg->flags |= _FP_GROUP_SPAN_SINGLE_SLICE;
                    parts_count = 1;

                    if (efp_slice_ipv6_mode) {
                        fg->sel_codes[0].fpf3 = _BCM_FIELD_EFP_KEY2;
                        default_key_1 = _BCM_FIELD_EFP_KEY2;
                    } else {
                        if ((key_match_type[0] == KEY_TYPE_IPv6_SINGLE) ||
                            (key_match_type[1] == KEY_TYPE_IPv6_SINGLE)) {
                            fg->sel_codes[0].fpf3 = _BCM_FIELD_EFP_KEY2;
                            default_key_1 = _BCM_FIELD_EFP_KEY2;
                        } else {
                            fg->sel_codes[0].fpf3 = _BCM_FIELD_EFP_KEY1;
                            default_key_1 = _BCM_FIELD_EFP_KEY2;
                        }
                    }

                    rv = _bcm_field_th_group_construct_quals_add(unit, fc,
                                                             stage_fc, fg,
                                                             _FP_ENTRY_TYPE_0);
                    if (BCM_FAILURE(rv)) {
                        sal_free(fg);
                        goto cleanup;
                    }

                    fg->sel_codes[0].fpf3 = _BCM_FIELD_EFP_KEY4;
                    rv = _bcm_field_th_group_construct_quals_add(unit, fc,
                                                             stage_fc, fg,
                                                             _FP_ENTRY_TYPE_1);
                    if (BCM_FAILURE(rv)) {
                        sal_free(fg);
                        goto cleanup;
                    }

                    fg->sel_codes[0].fpf3 = default_key_1;

                    break;
                case _BCM_FIELD_EGRESS_SLICE_MODE_SINGLE_L3_ANY:

                    fg->flags |= _FP_GROUP_SPAN_SINGLE_SLICE;
                    parts_count = 1;

                    fg->sel_codes[0].fpf3 = _BCM_FIELD_EFP_KEY1;
                    rv = _bcm_field_th_group_construct_quals_add(unit, fc,
                                                             stage_fc, fg,
                                                             _FP_ENTRY_TYPE_0);
                    if (BCM_FAILURE(rv)) {
                        sal_free(fg);
                        goto cleanup;
                    }

                    fg->sel_codes[0].fpf3 = _BCM_FIELD_EFP_KEY4;
                    rv = _bcm_field_th_group_construct_quals_add(unit, fc,
                                                             stage_fc, fg,
                                                             _FP_ENTRY_TYPE_1);
                    if (BCM_FAILURE(rv)) {
                        sal_free(fg);
                        goto cleanup;
                    }

                    fg->sel_codes[0].fpf3 = _BCM_FIELD_EFP_KEY1;
                    break;
                case _BCM_FIELD_EGRESS_SLICE_MODE_DOUBLE:

                    fg->flags |= _FP_GROUP_SPAN_DOUBLE_SLICE;
                    parts_count = 2;

                    if (efp_slice_ipv6_mode) {
                        fg->sel_codes[0].fpf3 = _BCM_FIELD_EFP_KEY2;
                        fg->sel_codes[1].fpf3 = _BCM_FIELD_EFP_KEY3;
                        default_key_1 = _BCM_FIELD_EFP_KEY2;
                        default_key_2 = _BCM_FIELD_EFP_KEY3;
                    } else {

                        if ((key_match_type[0] == KEY_TYPE_IPv6_DOUBLE) ||
                            (key_match_type[1] == KEY_TYPE_IPv6_DOUBLE)) {
                            fg->sel_codes[0].fpf3 = _BCM_FIELD_EFP_KEY2;
                            fg->sel_codes[1].fpf3 = _BCM_FIELD_EFP_KEY3;
                            default_key_1 = _BCM_FIELD_EFP_KEY2;
                            default_key_2 = _BCM_FIELD_EFP_KEY3;
                        } else if ((key_match_type[0] ==
                                    KEY_TYPE_IPv4_L2_L3_DOUBLE) ||
                                   (key_match_type[0] ==
                                    KEY_TYPE_IPv4_L2_L3_DOUBLE) ) {
                            fg->sel_codes[0].fpf3 = _BCM_FIELD_EFP_KEY1;
                            fg->sel_codes[1].fpf3 = _BCM_FIELD_EFP_KEY4;
                            default_key_1 = _BCM_FIELD_EFP_KEY1;
                            default_key_2 = _BCM_FIELD_EFP_KEY4;
                        } else {
                            fg->sel_codes[0].fpf3 = _BCM_FIELD_EFP_KEY8;
                            fg->sel_codes[1].fpf3 = _BCM_FIELD_EFP_KEY4;
                            default_key_1 = _BCM_FIELD_EFP_KEY8;
                            default_key_2 = _BCM_FIELD_EFP_KEY4;
                        }
                    }

                    rv = _bcm_field_th_group_construct_quals_add(unit, fc,
                                                             stage_fc, fg,
                                                             _FP_ENTRY_TYPE_0);
                    if (BCM_FAILURE(rv)) {
                        sal_free(fg);
                        goto cleanup;
                    }

                    fg->sel_codes[0].fpf3 = _BCM_FIELD_EFP_KEY8;
                    fg->sel_codes[1].fpf3 = _BCM_FIELD_EFP_KEY4;

                    rv = _bcm_field_th_group_construct_quals_add(unit, fc,
                                                             stage_fc, fg,
                                                             _FP_ENTRY_TYPE_1);
                    if (BCM_FAILURE(rv)) {
                        sal_free(fg);
                        goto cleanup;
                    }

                    fg->sel_codes[0].fpf3 = default_key_1;
                    fg->sel_codes[1].fpf3 = default_key_2;
                    break;

                case _BCM_FIELD_EGRESS_SLICE_MODE_DOUBLE_L3_ANY:

                    fg->flags |= _FP_GROUP_SPAN_DOUBLE_SLICE;
                    parts_count = 2;

                    if ((key_match_type[0] == KEY_TYPE_HIGIG_DOUBLE) ||
                        (key_match_type[1] == KEY_TYPE_HIGIG_DOUBLE)) {
                        fg->sel_codes[0].fpf3 = _BCM_FIELD_EFP_KEY6;
                        fg->sel_codes[1].fpf3 = _BCM_FIELD_EFP_KEY4;
                        default_key_1 = _BCM_FIELD_EFP_KEY6;
                        default_key_2 = _BCM_FIELD_EFP_KEY4;
                    } else if ((key_match_type[0] ==
                                KEY_TYPE_LOOPBACK_DOUBLE) ||
                               (key_match_type[0] ==
                                KEY_TYPE_LOOPBACK_DOUBLE) ) {
                        fg->sel_codes[0].fpf3 = _BCM_FIELD_EFP_KEY7;
                        fg->sel_codes[1].fpf3 = _BCM_FIELD_EFP_KEY4;
                        default_key_1 = _BCM_FIELD_EFP_KEY7;
                        default_key_2 = _BCM_FIELD_EFP_KEY4;
                    } else {
                        fg->sel_codes[0].fpf3 = _BCM_FIELD_EFP_KEY1;
                        fg->sel_codes[1].fpf3 = _BCM_FIELD_EFP_KEY4;
                        default_key_1 = _BCM_FIELD_EFP_KEY1;
                        default_key_2 = _BCM_FIELD_EFP_KEY4;
                    }

                    rv = _bcm_field_th_group_construct_quals_add(unit, fc,
                                                             stage_fc, fg,
                                                             _FP_ENTRY_TYPE_0);
                    if (BCM_FAILURE(rv)) {
                        sal_free(fg);
                        goto cleanup;
                    }

                    fg->sel_codes[0].fpf3 = _BCM_FIELD_EFP_KEY1;
                    fg->sel_codes[1].fpf3 = _BCM_FIELD_EFP_KEY4;

                    rv = _bcm_field_th_group_construct_quals_add(unit, fc,
                                                             stage_fc, fg,
                                                             _FP_ENTRY_TYPE_1);
                    if (BCM_FAILURE(rv)) {
                        sal_free(fg);
                        goto cleanup;
                    }

                    fg->sel_codes[0].fpf3 = default_key_1;
                    fg->sel_codes[1].fpf3 = default_key_2;
                    break;

                case _BCM_FIELD_EGRESS_SLICE_MODE_DOUBLE_L3_V6:

                    fg->flags |= _FP_GROUP_SPAN_DOUBLE_SLICE;
                    parts_count = 2;

                    fg->sel_codes[0].fpf3 = _BCM_FIELD_EFP_KEY2;
                    fg->sel_codes[1].fpf3 = _BCM_FIELD_EFP_KEY4;


                    rv = _bcm_field_th_group_construct_quals_add(unit, fc,
                                                             stage_fc, fg,
                                                             _FP_ENTRY_TYPE_0);
                    if (BCM_FAILURE(rv)) {
                        sal_free(fg);
                        goto cleanup;
                    }

                    fg->sel_codes[0].fpf3 = _BCM_FIELD_EFP_KEY8;
                    fg->sel_codes[1].fpf3 = _BCM_FIELD_EFP_KEY4;

                    rv = _bcm_field_th_group_construct_quals_add(unit, fc,
                                                             stage_fc, fg,
                                                             _FP_ENTRY_TYPE_1);
                    if (BCM_FAILURE(rv)) {
                        sal_free(fg);
                        goto cleanup;
                    }

                    fg->sel_codes[0].fpf3 = _BCM_FIELD_EFP_KEY2;
                    fg->sel_codes[1].fpf3 = _BCM_FIELD_EFP_KEY4;

                    break;
                default:
                    rv = BCM_E_INTERNAL;
                    sal_free(fg);
                    goto cleanup;
            }

            /* Associate slice(s) to group */
            fg->slices = stage_fc->slices[_FP_DEF_INST] + slice_idx;
            SOC_PBMP_CLEAR(all_pbmp);
            SOC_PBMP_ASSIGN(all_pbmp, PBMP_PORT_ALL(unit));
            SOC_PBMP_OR(all_pbmp, PBMP_CMIC(unit));
            SOC_PBMP_ASSIGN(fg->pbmp, all_pbmp);
            BCM_PBMP_OR(fs->pbmp, fg->pbmp);

            /* Initialize group default ASET list. */
            rv = _field_group_default_aset_set(unit, fg);
            if (BCM_FAILURE(rv)) {
                sal_free(fg);
                goto cleanup;
            }

            fg->flags |= _FP_GROUP_LOOKUP_ENABLED;
            fg->next = fc->groups;
            fc->groups = fg;

            /* Now go over the entries in this slice */
            prev_prio = -1;
            for (idx = 0; idx < slice_ent_cnt; idx++) {
                efp_tcam_entry = soc_mem_table_idx_to_pointer
                                     (unit, EFP_TCAMm, efp_tcam_entry_t *,
                                      efp_tcam_buf, idx +
                                      slice_ent_cnt * slice_idx);
                if (soc_EFP_TCAMm_field32_get(unit, efp_tcam_entry,
                                              VALIDf) == 0) {
                    continue;
                }

                _field_extract((uint32 *)efp_tcam_entry, 236 + 2, 4,
                               &entry_type);

                /* Allocate memory for this entry */
                mem_sz = parts_count * sizeof (_field_entry_t);
                _FP_XGS3_ALLOC(f_ent, mem_sz, "field entry");
                if (f_ent == NULL) {
                    rv = BCM_E_MEMORY;
                    goto cleanup;
                }
                for (idx1 = 0; idx1 < _FP_POLICER_LEVEL_COUNT; idx1++) {
                     f_ent->policer[idx1].pid = _FP_INVALID_INDEX;
                }
                sid = pid = -1;
                if (fc->l2warm) {
                    rv = _field_trx_entry_info_retrieve(unit,
                                                        &eid,
                                                        &prio,
                                                        fc,
                                                        0,
                                                        &prev_prio,
                                                        &sid,
                                                        &pid,
                                                        stage_fc,
                                                        &dvp_type,
                                                        f_ent_wb
                                                        );
                    
                    if (BCM_FAILURE(rv)) {
                        sal_free(f_ent);
                        goto cleanup;
                    }
                } else {
                    _bcm_field_last_alloc_eid_incr();
                }

                switch(efp_slice_mode) {
                    case _BCM_FIELD_EGRESS_SLICE_MODE_SINGLE_L2:
                        f_ent->efp_key_match_type = 1;
                        break;
                    case _BCM_FIELD_EGRESS_SLICE_MODE_SINGLE_L3:
                        if (fg->sel_codes[0].fpf3 == _BCM_FIELD_EFP_KEY1) {
                            if (KEY_TYPE_IPv4_SINGLE == entry_type) {
                                f_ent->efp_key_match_type = 0;
                            } else {
                                f_ent->efp_key_match_type = 1;
                            }
                        } else {
                            if (KEY_TYPE_IPv6_SINGLE == entry_type) {
                                f_ent->efp_key_match_type = 0;
                            } else {
                                f_ent->efp_key_match_type = 1;
                            }
                        }
                        break;
                    case _BCM_FIELD_EGRESS_SLICE_MODE_SINGLE_L3_ANY:
                        if (KEY_TYPE_IPv4_SINGLE == entry_type) {
                            f_ent->efp_key_match_type = 0;
                        } else {
                            f_ent->efp_key_match_type = 1;
                        }
                        break;
                    case _BCM_FIELD_EGRESS_SLICE_MODE_DOUBLE:
                        if ((fg->sel_codes[0].fpf3 == _BCM_FIELD_EFP_KEY1) &&
                            (fg->sel_codes[1].fpf3 == _BCM_FIELD_EFP_KEY4)) {
                            if (KEY_TYPE_IPv4_L2_L3_DOUBLE == entry_type) {
                                f_ent->efp_key_match_type = 0;
                            } else {
                                f_ent->efp_key_match_type = 1;
                            }
                        } else if ((fg->sel_codes[0].fpf3 ==
                                    _BCM_FIELD_EFP_KEY2) &&
                                   (fg->sel_codes[1].fpf3 ==
                                    _BCM_FIELD_EFP_KEY3)) {
                            if (KEY_TYPE_IPv6_DOUBLE == entry_type) {
                                f_ent->efp_key_match_type = 0;
                            } else {
                                f_ent->efp_key_match_type = 1;
                            }
                        } else {
                            f_ent->efp_key_match_type = 1;
                        }
                        break;
                    case _BCM_FIELD_EGRESS_SLICE_MODE_DOUBLE_L3_ANY:
                        if ((fg->sel_codes[0].fpf3 == _BCM_FIELD_EFP_KEY6) &&
                            (fg->sel_codes[1].fpf3 == _BCM_FIELD_EFP_KEY4)) {
                            if (KEY_TYPE_HIGIG_DOUBLE == entry_type) {
                                f_ent->efp_key_match_type = 0;
                            } else {
                                f_ent->efp_key_match_type = 1;
                            }
                        } else if ((fg->sel_codes[0].fpf3 ==
                                    _BCM_FIELD_EFP_KEY7) &&
                                   (fg->sel_codes[1].fpf3 ==
                                    _BCM_FIELD_EFP_KEY4)) {
                            if (KEY_TYPE_LOOPBACK_DOUBLE == entry_type) {
                                f_ent->efp_key_match_type = 0;
                            } else {
                                f_ent->efp_key_match_type = 1;
                            }
                        } else {
                            f_ent->efp_key_match_type = 1;
                        }
                        break;
                    case _BCM_FIELD_EGRESS_SLICE_MODE_DOUBLE_L3_V6:
                        if (KEY_TYPE_IPv4_IPv6_DOUBLE == entry_type) {
                            f_ent->efp_key_match_type = 0;
                        } else {
                            f_ent->efp_key_match_type = 1;
                        }
                        break;
                    default:
                        rv = BCM_E_INTERNAL;
                        sal_free(f_ent);
                        goto cleanup;
                }

                pri_tcam_idx = idx + slice_ent_cnt * slice_idx;
                for (i = 0; i < parts_count; i++) {
                    if (fc->l2warm) {
                        f_ent[i].eid = eid;
                        /* Set retrieved dvp_type */
                        f_ent[i].dvp_type = dvp_type;
                    } else {
                        f_ent[i].eid = _bcm_field_last_alloc_eid_get();
                    }
                    f_ent[i].group = fg;
                    if (fc->flags & _FP_COLOR_INDEPENDENT) {
                        f_ent[i].flags |= _FP_ENTRY_COLOR_INDEPENDENT;
                    }
                    rv = _bcm_field_tcam_part_to_entry_flags(unit, i, fg,
                                                             &f_ent[i].flags);
                    if (BCM_FAILURE(rv)) {
                        sal_free(f_ent);
                        goto cleanup;
                    }
                    rv = _bcm_field_entry_part_tcam_idx_get(unit, f_ent,
                                                            pri_tcam_idx,
                                                            i, &part_index);
                    if (BCM_FAILURE(rv)) {
                        sal_free(f_ent);
                        goto cleanup;
                    }
                    rv = _bcm_field_tcam_idx_to_slice_offset(unit, stage_fc,
                                                             _FP_DEF_INST,
                                                             part_index,
                                                             &slice_number,
                                                (int *)&f_ent[i].slice_idx);
                    if (BCM_FAILURE(rv)) {
                        sal_free(f_ent);
                        goto cleanup;
                    }
                    f_ent[i].fs = stage_fc->slices[_FP_DEF_INST] + slice_number;
                    if (0 == (f_ent[i].flags & _FP_ENTRY_SECOND_HALF)) {
                        /* Decrement slice free entry count for primary
                           entries. */
                        f_ent[i].fs->free_count--;
                    }
                    /* Assign entry to a slice */
                    f_ent[i].fs->entries[f_ent[i].slice_idx] = f_ent + i;
                    f_ent[i].flags |= _FP_ENTRY_INSTALLED;

                    if (soc_EFP_TCAMm_field32_get(unit, efp_tcam_entry, VALIDf) == 3) {
                        f_ent[i].flags |= _FP_ENTRY_ENABLED;
                    }

                    /* Get the actions associated with this entry part */
                    efp_policy_entry = soc_mem_table_idx_to_pointer
                                               (unit, EFP_POLICY_TABLEm,
                                                efp_policy_table_entry_t *,
                                                efp_policy_buf, part_index);
                    rv = _field_tr2_actions_recover(unit,
                                                    EFP_POLICY_TABLEm,
                                                    (uint32 *) efp_policy_entry,
                                                    f_ent,
                                                    i,
                                                    sid,
                                                    pid,
                                                    f_ent_wb);
                }
                /* Add to the group */
                rv = _field_group_entry_add(unit, fg, f_ent);
                if (BCM_FAILURE(rv)) {
                    sal_free(f_ent);
                    goto cleanup;
                }
                f_ent = NULL;
            }
        }

        /* Free up the temporary slice group info */
        if (fc->l2warm) {
            _field_scache_slice_group_free(unit,
                                           fc,
                                           slice_idx
                                           );
        }
    }

    /* Now go over the expanded slices */
    for (slice_idx = 0; slice_idx < stage_fc->tcam_slices; slice_idx++) {
        if (!expanded[slice_idx]) {
            continue;
        }
        /* Ignore secondary slice in paired mode */
        efp_slice_mode = soc_reg_field_get(unit, EFP_SLICE_CONTROLr, rval,
                                           _th_efp_slice_mode[slice_idx][0]);
        if (((efp_slice_mode == _BCM_FIELD_EGRESS_SLICE_MODE_DOUBLE) ||
             (efp_slice_mode == _BCM_FIELD_EGRESS_SLICE_MODE_DOUBLE_L3_ANY) ||
             (efp_slice_mode == _BCM_FIELD_EGRESS_SLICE_MODE_DOUBLE_L3_V6)) &&
            (slice_idx % 2)) {
            continue;
        }
        /* Skip if slice has no valid entries */
        fs = stage_fc->slices[_FP_DEF_INST] + slice_idx;
        slice_ent_cnt = fs->entry_count;
        for (idx = 0; idx < slice_ent_cnt; idx++) {
            if (_bcm_field_slice_offset_to_tcam_idx(unit, stage_fc,
                  _FP_DEF_INST, slice_idx, idx, &phys_tcam_idx) != BCM_E_NONE) {
                rv = BCM_E_INTERNAL;
                goto cleanup;
            }
            efp_tcam_entry = soc_mem_table_idx_to_pointer(unit,
                             EFP_TCAMm, efp_tcam_entry_t *,
                             efp_tcam_buf, phys_tcam_idx);
            if (soc_EFP_TCAMm_field32_get(unit, efp_tcam_entry, VALIDf) != 0) {
                break;
            }
        }
        if (idx == slice_ent_cnt) {
            continue;
        }
        /* If Level 2, retrieve the GIDs in this slice */
        if (fc->l2warm) {
            rv = _field_scache_slice_group_recover(unit,
                     fc, slice_idx, &multigroup);
            if (BCM_FAILURE(rv) && (rv != BCM_E_NOT_FOUND)) {
                fc->l2warm = 0;
                goto cleanup;
            }
            if (rv == BCM_E_NOT_FOUND) {
                rv = BCM_E_NONE;
                continue;
            }
        }
        /* Now find the master slice for this virtual group */
        vslice_idx = _field_physical_to_virtual(unit, slice_idx, stage_fc);
        if (vslice_idx < 0) {
            rv = BCM_E_INTERNAL;

            goto cleanup;
        }

        if (fc->l2warm
                && (version >=BCM_WB_VERSION_1_8)) {
            master_slice = slice_master_idx[slice_idx];
        } else {
        max = -1;
        for (i = 0; i < stage_fc->tcam_slices; i++) {
            if ((stage_fc->vmap[0][vslice_idx].virtual_group ==
                stage_fc->vmap[0][i].virtual_group) && (i != vslice_idx)) {
                if (i > max) {
                    max = i;
                }
            }
        }
        if (max < 0) {
            rv = BCM_E_INTERNAL;

            goto cleanup;
        }

        master_slice = stage_fc->vmap[0][max].vmap_key;
        }
        /* See which group is in this slice - can be only one */
        fg = fc->groups;
        while (fg != NULL) {
            /* Check if group is in this slice */
            fs = &fg->slices[0];
            if ((fg->stage_id == stage_fc->stage_id)
                    && (fs->slice_number == master_slice)) {
                break;
            }
            fg = fg->next;
        }
        if (fg == NULL) {
            rv = BCM_E_INTERNAL;

            goto cleanup;
        }

        old_physical_slice = fs->slice_number;

        /* Set up the new physical slice parameters in Software */
        for(part_index = parts_count - 1; part_index >= 0; part_index--) {
            /* Get entry flags. */
            rv = _bcm_field_tcam_part_to_entry_flags(unit, part_index, fg,
                    &entry_flags);
            BCM_IF_ERROR_RETURN(rv);

            /* Get slice id for entry part */
            rv = _bcm_field_tcam_part_to_slice_number(unit, part_index,
                                                      fg, &slice_num);
            BCM_IF_ERROR_RETURN(rv);

            /* Get slice pointer. */
            fs = stage_fc->slices[_FP_DEF_INST] + slice_idx + slice_num;

            if (0 == (entry_flags & _FP_ENTRY_SECOND_HALF)) {
                /*
                 * Set per slice configuration &  number of free entries in the
                 * slice.
                 */
                fs->free_count = fs->entry_count;
                if (fg->flags & _FP_GROUP_INTRASLICE_DOUBLEWIDE) {
                    fs->free_count >>= 1;
                }
                /* Set group flags in in slice.*/
                fs->group_flags = fg->flags & _FP_GROUP_STATUS_MASK;

                /* Add slice to slices linked list . */
                fs_temp = stage_fc->slices[_FP_DEF_INST] + old_physical_slice + slice_num;
                /* To handle more than one auto expanded slice in a group */
                while (fs_temp->next != NULL) {
                    fs_temp = fs_temp->next;
                }
                fs_temp->next = fs;
                fs->prev = fs_temp;
            }
        }

        fs = stage_fc->slices[_FP_DEF_INST] + slice_idx;
        fs->group_flags = fg->flags & _FP_GROUP_STATUS_MASK;

        prev_prio = -1;
        SOC_PBMP_CLEAR(all_pbmp);
        SOC_PBMP_ASSIGN(all_pbmp, PBMP_PORT_ALL(unit));
        SOC_PBMP_OR(all_pbmp, PBMP_CMIC(unit));
        SOC_PBMP_ASSIGN(fg->pbmp, all_pbmp);
        BCM_PBMP_OR(fs->pbmp, fg->pbmp);
        for (idx = 0; idx < slice_ent_cnt; idx++) {
            if (_bcm_field_slice_offset_to_tcam_idx(unit, stage_fc,
                  _FP_DEF_INST, slice_idx, idx, &phys_tcam_idx) != BCM_E_NONE) {
                rv = BCM_E_INTERNAL;
                goto cleanup;
            }
            efp_tcam_entry = soc_mem_table_idx_to_pointer(unit,
                             EFP_TCAMm, efp_tcam_entry_t *,
                             efp_tcam_buf, phys_tcam_idx);
            if (soc_EFP_TCAMm_field32_get(unit, efp_tcam_entry, VALIDf) == 0) {
                continue;
            }

            /* Allocate memory for the entry */
            rv = _bcm_field_entry_tcam_parts_count(unit, fg->stage_id,
                                                   fg->flags, &parts_count);
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }
            mem_sz = parts_count * sizeof (_field_entry_t);
            _FP_XGS3_ALLOC(f_ent, mem_sz, "field entry");
            if (f_ent == NULL) {
                rv = BCM_E_MEMORY;
                goto cleanup;
            }

            for (idx1 = 0; idx1 < _FP_POLICER_LEVEL_COUNT; idx1++) {
                 f_ent->policer[idx1].pid = _FP_INVALID_INDEX;
            }

            sid = pid = -1;
            if (fc->l2warm) {
                rv = _field_trx_entry_info_retrieve(unit,
                         &eid, &prio, fc, multigroup, &prev_prio, &sid, &pid,
                         stage_fc, &dvp_type,f_ent_wb);
                
                if (BCM_FAILURE(rv)) {
                    sal_free(f_ent);
                    goto cleanup;
                }
            } else {
                _bcm_field_last_alloc_eid_incr();
            }
            pri_tcam_idx = phys_tcam_idx;
            for (i = 0; i < parts_count; i++) {
                if (fc->l2warm) {
                    /* Use retrieved EID */
                    f_ent[i].eid = eid;
                    /* Set retrieved dvp_type */
                    f_ent[i].dvp_type = dvp_type;
                } else {
                    f_ent[i].eid = _bcm_field_last_alloc_eid_get();
                }
                f_ent[i].group = fg;

                if (fc->flags & _FP_COLOR_INDEPENDENT) {
                    f_ent[i].flags |= _FP_ENTRY_COLOR_INDEPENDENT;
                }
                rv = _bcm_field_tcam_part_to_entry_flags(unit, i, fg,
                                                         &f_ent[i].flags);
                if (BCM_FAILURE(rv)) {
                    sal_free(f_ent);
                    goto cleanup;
                }
                rv = _bcm_field_entry_part_tcam_idx_get(unit,
                         f_ent, pri_tcam_idx, i, &part_index);
                if (BCM_FAILURE(rv)) {
                    sal_free(f_ent);
                    goto cleanup;
                }
                rv = _bcm_field_tcam_idx_to_slice_offset(unit,
                         stage_fc, _FP_DEF_INST, part_index, &slice_number,
                         (int *)&f_ent[i].slice_idx);
                if (BCM_FAILURE(rv)) {
                    sal_free(f_ent);
                    goto cleanup;
                }
                f_ent[i].fs = stage_fc->slices[_FP_DEF_INST] + slice_number;
                if (0 == (f_ent[i].flags & _FP_ENTRY_SECOND_HALF)) {
                    /* Decrement slice free entry count for primary
                       entries. */
                    f_ent[i].fs->free_count--;
                }
                /* Assign entry to a slice */
                f_ent[i].fs->entries[f_ent[i].slice_idx] = f_ent + i;
                f_ent[i].flags |= _FP_ENTRY_INSTALLED;

                if (soc_EFP_TCAMm_field32_get(unit, efp_tcam_entry, VALIDf) == 3) {
                    f_ent[i].flags |= _FP_ENTRY_ENABLED;
                }

                /* Get the actions associated with this part of the entry */
                efp_policy_entry = soc_mem_table_idx_to_pointer(
                    unit, EFP_POLICY_TABLEm, efp_policy_table_entry_t *,
                    efp_policy_buf, part_index);
                rv = _field_tr2_actions_recover(unit,
                    EFP_POLICY_TABLEm, (uint32 *) efp_policy_entry,
                    f_ent, i, sid, pid, f_ent_wb);
            }
            /* Add to the group */
            if (fc->l2warm) {
                f_ent->prio = prio;
            } else {
                f_ent->prio = (vslice_idx << 10) | (slice_ent_cnt - idx);
            }
            rv = _field_group_entry_add(unit, fg, f_ent);
            if (BCM_FAILURE(rv)) {
                sal_free(f_ent);
                goto cleanup;
            }
            f_ent = NULL;
        }
        /* Free up the temporary slice group info */
        if (fc->l2warm) {
            _field_scache_slice_group_free(unit, fc, slice_idx);
        }
    }

    if (fc->l2warm) {
        temp = 0;
        temp |= buf[fc->scache_pos];
        fc->scache_pos++;
        temp |= buf[fc->scache_pos] << 8;
        fc->scache_pos++;
        temp |= buf[fc->scache_pos] << 16;
        fc->scache_pos++;
        temp |= buf[fc->scache_pos] << 24;
        fc->scache_pos++;
        if (temp != _FIELD_EFP_DATA_END) {
            fc->l2warm = 0;
            rv = BCM_E_INTERNAL;
        }

        if (NULL != buf1) {
            temp = 0;
            temp |= buf1[fc->scache_pos1];
            fc->scache_pos1++;
            temp |= buf1[fc->scache_pos1] << 8;
            fc->scache_pos1++;
            temp |= buf1[fc->scache_pos1] << 16;
            fc->scache_pos1++;
            temp |= buf1[fc->scache_pos1] << 24;
            fc->scache_pos1++;
            if (temp != _FIELD_EFP_DATA_END) {
                fc->l2warm = 0;
                rv = BCM_E_INTERNAL;
            }
        }

    }

    if (BCM_SUCCESS(rv)) {
        _field_group_slice_vmap_recovery(unit, fc, stage_fc);
    }

    _field_tr2_stage_reinit_all_groups_cleanup(unit, fc,
                                               _BCM_FIELD_STAGE_EGRESS,
                                               NULL);
cleanup:
    if (efp_tcam_buf) {
        soc_cm_sfree(unit, efp_tcam_buf);
    }
    if (efp_policy_buf) {
        soc_cm_sfree(unit, efp_policy_buf);
    }
    return rv;
}

               /*  END OF WARMBOOT ROUTINES */
#endif  /* BCM_WARM_BOOT_SUPPORT */

#else /* BCM_TOMAHAWK_SUPPORT && BCM_FIELD_SUPPORT */
int _th_field_not_empty;
#endif  /* BCM_TOMAHAWK_SUPPORT && BCM_FIELD_SUPPORT */
