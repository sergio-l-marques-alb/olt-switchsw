/* 
 * $Id: policer.c,v 1.48 Broadcom SDK $
 *
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
 * File:    tx.c
 * Purpose: Implementation of bcm_petra_tx* API for dune devices
 */


#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif



#define _ERR_MSG_MODULE_NAME BSL_BCM_POLICER

#include <shared/bsl.h>

#include <bcm/policer.h>
#include <soc/mem.h>

#include <bcm_int/control.h>
#include <bcm_int/dpp/utils.h>
#include <bcm_int/dpp/error.h>

#include <soc/dpp/PPD/ppd_api_metering.h>

#include <bcm_int/petra_dispatch.h>
#include <bcm_int/dpp/policer.h>

#include <bcm_int/dpp/alloc_mngr.h>
#include <bcm_int/dpp/rate.h>
#include <bcm_int/dpp/gport_mgmt.h>
#include <bcm_int/dpp/state.h>
#include <bcm_int/dpp/switch.h>

#include <shared/shr_resmgr.h>
#include <bcm/debug.h>
#include <bcm_int/common/debug.h>

#ifdef BCM_ARAD_SUPPORT
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_metering.h>
#endif /* BCM_ARAD_SUPPORT */

#include <shared/swstate/sw_state_access.h>
#include <shared/swstate/sw_state_sync_db.h>

/***************************************************************/

/*
 * Local defines
 */


#define DPP_POLICER_MSG(string)   _ERR_MSG_MODULE_NAME, unit, "%s[%d]: " string, __FILE__, __LINE__

#define DPP_POLICER_INIT_CHECK(unit) \
do { \
    BCM_DPP_UNIT_CHECK(unit); \
    if (!sw_state_sync_db[(unit)].dpp.policer_lock) { \
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNIT,  (_BSL_BCM_MSG("%s: Policers unitialized on unit:%d \n"), \
                           FUNCTION_NAME(), unit)); \
    } \
} while (0)

#define DPP_POLICER_UNIT_LOCK(unit) \
do { \
    if (sal_mutex_take(sw_state_sync_db[(unit)].dpp.policer_lock, sal_mutex_FOREVER)) { \
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("%s: sal_mutex_take failed for unit %d. \n"), \
                    FUNCTION_NAME(), unit)); \
    } \
    _lock_taken = 1;  \
} while (0)


#define DPP_POLICER_UNIT_UNLOCK(unit) \
do { \
    if(1 == _lock_taken) { \
        _lock_taken = 0;  \
        if (sal_mutex_give(sw_state_sync_db[(unit)].dpp.policer_lock)) { \
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("%s: sal_mutex_give failed for unit %d. \n"), \
                        FUNCTION_NAME(), unit)); \
        } \
    } \
} while (0)

#define DPP_POLICER_LEGAL_FLAGS_ARAD (  BCM_POLICER_WITH_ID                 | \
                                        /* Note: REPLACE is an internal */    \
                                        /* flag. */                           \
                                        BCM_POLICER_REPLACE                 | \
                                        BCM_POLICER_MACRO                   | \
                                        BCM_POLICER_COLOR_BLIND             | \
                                        BCM_POLICER_MAX_CKBITS_SEC_VALID    | \
                                        BCM_POLICER_MAX_PKBITS_SEC_VALID    | \
                                        BCM_POLICER_MODE_BYTES              | \
                                        BCM_POLICER_REPLACE_SHARED)

#ifdef BCM_88660_A0
#   define DPP_POLICER_LEGAL_FLAGS_ARAD_PLUS (DPP_POLICER_LEGAL_FLAGS_ARAD | BCM_POLICER_MODE_PACKETS)
#endif

/* implemented for PB:
 *  - assumption, meter groups: 0,1
 *  - meter encoding type: 0 
 */


/* convert defines*/
/* kbits to bytes, for burst BCM TO DPP */
#define _DPP_POLICER_KBITS_TO_BYTES(__bcm_rate) ((__bcm_rate == 0) ? 64 :((__bcm_rate)*125))
/* bytes to kbits , for burst DPP TO BCM */
#define _DPP_POLICER_BYTES_TO_KBITS(__bcm_rate) ((__bcm_rate)/125)

/* is high rate meter needed, according to rate/burst volume*/
#define _DPP_POLICER_IS_HIGH_RATE(__unit,__pol_cfg)    \
    ((__pol_cfg->ckbits_sec > SOC_DPP_CONFIG(unit)->meter.lr_max_rate) ||    \
    (__pol_cfg->pkbits_sec > SOC_DPP_CONFIG(unit)->meter.lr_max_rate) ||    \
    (_DPP_POLICER_KBITS_TO_BYTES((__pol_cfg)->ckbits_burst) > SOC_DPP_CONFIG(unit)->meter.lr_max_burst) || \
    (_DPP_POLICER_KBITS_TO_BYTES((__pol_cfg)->pkbits_burst) > SOC_DPP_CONFIG(unit)->meter.lr_max_burst))

/* this is only SW representation */
/* bit 12 tells if this is group 0 or 1*/

/* start index of High rate */
#define _DPP_POLICER_METER_HR_PROFILE_MIN(__unit) ((SOC_DPP_CONFIG(__unit)->meter.nof_meter_profiles - SOC_DPP_CONFIG(__unit)->meter.nof_high_rate_profiles))


/* 
 * format meter id 
 * 0-12: meter/policer id 
 * 13: group (for meter) 
 * 29: type 0:meter 1:ethernet-policer , 
 *     needed as same APIs and ID used for both meter types
 * Notes: 
 *    - meter type: has to be zero, as pointer by FP 
 */


#define _DPP_IS_ETH_POLICER_ID(__unit, __meter)  ((__meter) & _DPP_POLICER_METER_TYPE_MASK(__unit))
#define _DPP_ETH_POLICER_ID_GET(__unit, __meter) ((__meter) & _DPP_POLICER_METER_ID_MASK(__unit))
#define _DPP_ETH_POLICER_ID_SET(__unit, __meter) ((__meter) | _DPP_POLICER_METER_TYPE_MASK(__unit))

#define _DPP_ETH_METER_ID_GET(__unit, __meter) (__meter)

/* check if profile is high rate*/
#define _DPP_POLICER_PROFILE_IS_HR(_unit, __profile_id ) ((__profile_id) >= (SOC_DPP_CONFIG(unit)->meter.nof_meter_profiles - SOC_DPP_CONFIG(unit)->meter.nof_high_rate_profiles))


#define POLICER_ACCESS  sw_state_access[unit].dpp.bcm.policer


#define _DPP_POLICER_GROUP_MODES (14)

/* In packet mode the rates are in this resolution - i.e. a rate of (X * 64 will be X packets per sec). */
/* Since usual rates is in kbits, and since h/w rate is in bytes, the rates are later mutliplied by 128 internally, */
/* while we require multiplying only by 64 for packet mode. */
/* Therefore we divide by 128/64 = 2. */
#define _DPP_POLICER_PACKET_MODE_CORRECTION (2)

static int _dpp_policer_group_num_policers[_DPP_POLICER_GROUP_MODES] = {
    1,  /* bcmPolicerGroupModeSingle - all                              */
    3,  /* bcmPolicerGroupModeTrafficType - kn/uk uc, kn/uc mc, bc      */
    2,  /* bcmPolicerGroupModeDlfAll - <unknow-uc/mc, mc/uc/bc>         */
    -1, /* bcmPolicerGroupModeDlfIntPri - Unsupported                   */
    4,  /* bcmPolicerGroupModeTyped- uk-uc,k-uc, kn/uk mc, bc           */
    -1, /* bcmPolicerGroupModeTypedAll- uk-uc,k-uc, mc, bc, glbl        */
    -1, /* bcmPolicerGroupModeTypedIntPri 
           based on configured cos levels                               */
    -1, /* bcmPolicerGroupModeSingleWithControl-  all, glbl             */
    -1, /* bcmPolicerGroupModeTrafficTypeWithControl - uc, mc, bc, glbl */
    -1, /* bcmPolicerGroupModeDlfAllWithControl - Unsupported           */
    -1, /* bcmPolicerGroupModeDlfIntPriWithControl - <unknow-uc/mc, mc/uc/bc. glbl>                               */
    -1, /* bcmPolicerGroupModeTypedWithControl uk-uc,k-uc, mc, bc, glbl */
    -1, /* bcmPolicerGroupModeTypedAllWithControl                       */
    -1  /* bcmPolicerGroupModeTypedIntPriWithControl 
           based on configured cos levels                               */
};

/* policer utilities */


STATIC int bcm_petra_meter_max_info(
   int unit,
   bcm_policer_config_t *pol_cfg) 
{
    BCMDNX_INIT_FUNC_DEFS;
    bcm_policer_config_t_init(pol_cfg);

    /* sw state allocate unique profile according to this infomration */
    pol_cfg->ckbits_sec = SOC_DPP_CONFIG(unit)->meter.max_rate;
    pol_cfg->pkbits_sec = SOC_DPP_CONFIG(unit)->meter.max_rate;
    if (SOC_IS_ARAD(unit)) {
        pol_cfg->max_pkbits_sec = SOC_DPP_CONFIG(unit)->meter.max_rate;
        pol_cfg->max_ckbits_sec = SOC_DPP_CONFIG(unit)->meter.max_rate;
    }
    pol_cfg->ckbits_burst = _DPP_POLICER_BYTES_TO_KBITS(SOC_DPP_CONFIG(unit)->meter.max_burst);
    pol_cfg->pkbits_burst = _DPP_POLICER_BYTES_TO_KBITS(SOC_DPP_CONFIG(unit)->meter.max_burst);
    pol_cfg->flags = 0;
    pol_cfg->mode = bcmPolicerModeTrTcmDs;

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int bcm_petra_policer_max_info(
   int unit,
   bcm_policer_config_t *pol_cfg
    )
{
    BCMDNX_INIT_FUNC_DEFS;
    bcm_policer_config_t_init(pol_cfg);
    pol_cfg->ckbits_sec = SOC_DPP_CONFIG(unit)->meter.max_rate;
    pol_cfg->pkbits_sec = 0;
    if (SOC_IS_ARAD(unit)) {
        pol_cfg->max_ckbits_sec = 0;
        pol_cfg->max_pkbits_sec = 0;
    }
    pol_cfg->ckbits_burst = _DPP_POLICER_BYTES_TO_KBITS(SOC_DPP_CONFIG(unit)->meter.max_burst);
    pol_cfg->pkbits_burst = 0;
    pol_cfg->flags = 0;
    pol_cfg->mode = bcmPolicerModeSrTcm;

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int bcm_petra_max_info_check(
   int unit,
   bcm_policer_config_t *pol_cfg,
        bcm_policer_config_t *max_pol_cfg)
{
    BCMDNX_INIT_FUNC_DEFS;

    /* sw state allocate unique profile according to this infomration */
    if (pol_cfg->ckbits_sec > max_pol_cfg->ckbits_sec ||
        pol_cfg->pkbits_sec > max_pol_cfg->pkbits_sec ||
        pol_cfg->ckbits_burst > max_pol_cfg->ckbits_burst ||
        pol_cfg->pkbits_burst > max_pol_cfg->pkbits_burst) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("one or more of the rates/bursts is above max")));
    }
    if (SOC_IS_ARAD(unit)) {
        if (pol_cfg->max_pkbits_sec != _DPP_POLICER_MAX_KBITS_SEC_UNLIMITED &&
            (pol_cfg->max_pkbits_sec > max_pol_cfg->max_pkbits_sec ||
             pol_cfg->max_pkbits_sec < pol_cfg->pkbits_sec)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("max_pkbits_sec is above max or < pkbits_sec")));
        }
        if (pol_cfg->mode == bcmPolicerModeCascade ||
            pol_cfg->mode == bcmPolicerModeCoupledCascade) {
            if (pol_cfg->max_ckbits_sec != _DPP_POLICER_MAX_KBITS_SEC_UNLIMITED &&
                (pol_cfg->max_ckbits_sec > max_pol_cfg->max_ckbits_sec ||
                 pol_cfg->max_ckbits_sec < pol_cfg->ckbits_sec)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("max_ckbits_sec is above max or < ckbits_sec")));
            }
        }
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function
 *      _bcm_petra_polcer_check_config
 * Purpose
 *      general check function for policer params
 * Parameters
 *      (in) unit       = unit number
 *      (in) pol_cfg       = policer configuration
 *      ((in)) is_eth_policer = is ethernet policer or meter
 * Returns
 *      bcm_error_t = BCM_E_NONE if no error
 *                    BCM_E_* appropriately if not 
 */
STATIC int
_bcm_petra_polcer_check_config(
   int unit,
   bcm_policer_config_t      *pol_cfg,
   int                        is_eth_policer,
        int                        policer_id)
{
    int rv = BCM_E_NONE;
    bcm_policer_config_t      max_pol_cfg;
    int range_mode;
    uint32 legal_flags;

    BCMDNX_INIT_FUNC_DEFS;

    legal_flags = DPP_POLICER_LEGAL_FLAGS_ARAD;

#ifdef BCM_88660_A0
    if (SOC_IS_ARADPLUS(unit)) {
        legal_flags = DPP_POLICER_LEGAL_FLAGS_ARAD_PLUS;
    }
#endif /* BCM_88660_A0 */

    if (SOC_IS_PETRAB(unit) && ((pol_cfg->flags & BCM_POLICER_REPLACE_SHARED) || (pol_cfg->entropy_id != 0))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (DPP_POLICER_MSG("entropy_id and BCM_POLICER_REPLACE_SHARED are not supported for this device\n")));
    }

    /* replace without ID! */
    if ((pol_cfg->flags & BCM_POLICER_REPLACE) && (!(pol_cfg->flags & BCM_POLICER_WITH_ID))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (DPP_POLICER_MSG("replace flag can not be present when with_id is not present \n")));
    }
    if (pol_cfg->kbits_current != 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (DPP_POLICER_MSG("current kbits has to be zero \n")));
    }

    if (pol_cfg->action_id != 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (DPP_POLICER_MSG("action id has to be zero \n")));
    }

    if (pol_cfg->sharing_mode != BCM_POLICER_GLOBAL_METER_SHARING_MODE_MIN) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (DPP_POLICER_MSG("in case of sharing only minimum is supported \n")));
    }


    if (SOC_IS_ARAD(unit)) {
        /* get meters range mode (64K/32K) */
        range_mode = soc_property_get(unit, spn_POLICER_INGRESS_COUNT, 64);
        if ((range_mode == 64) && (pol_cfg->flags & BCM_POLICER_MACRO)) {
            /* in 64K range mode, meters are not divided into 2 groups, so user cannot specify a group */
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (DPP_POLICER_MSG("BCM_POLICER_MACRO flag is not supported when policer_ingress_count is 64K\n")));
        }

        if ((pol_cfg->flags & BCM_POLICER_REPLACE_SHARED) && (pol_cfg->entropy_id == 0)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (DPP_POLICER_MSG("BCM_POLICER_REPLACE_SHARED is valid only with non-zero entropy_id\n")));
        }

        if (pol_cfg->entropy_id >= SOC_DPP_CONFIG(unit)->meter.nof_meter_profiles) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (DPP_POLICER_MSG("entropy_id is out of bounds (max %d).\n"), SOC_DPP_CONFIG(unit)->meter.nof_meter_profiles));
        }
    }

    /* verify parameters according to mode */
    if (!is_eth_policer) {
        switch (pol_cfg->mode) {
        case bcmPolicerModeSrTcm: 
            if (pol_cfg->pkbits_sec != 0) { /* eir has to be zero in single rate */
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (DPP_POLICER_MSG("for single rate pir has to be equal eir has to be 0 \n")));
            }
			if (pol_cfg->ckbits_sec !=0 && pol_cfg->ckbits_burst == 0) { /* if cir isn't zero, cbs can't be zero */
				BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (DPP_POLICER_MSG("Zero burst size isn't supported, 64 bytes is the minimal value \n")));
			}
			break; 
        case bcmPolicerModeTrTcmDs: /* two rate three colors */
		case bcmPolicerModeCoupledTrTcmDs: /* two rates three colors with coupling */
			if (pol_cfg->ckbits_sec !=0 && pol_cfg->ckbits_burst == 0) { /* if cir isn't zero, cbs can't be zero */
				BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (DPP_POLICER_MSG("Zero burst size isn't supported, 64 bytes is the minimal value \n")));
			}
            break;
        case  bcmPolicerModeCommitted: /* single rate single burst */
            if (pol_cfg->pkbits_sec != 0 || pol_cfg->pkbits_burst != 0) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (DPP_POLICER_MSG("for single rate pir/burst have to be equal to 0 \n")));
            }
            break;
#ifdef BCM_ARAD_SUPPORT
            /* modes that are supported only in ARAD */
        case bcmPolicerModeCascade: /* 4 meters work in hierarchical mode */
        case bcmPolicerModeCoupledCascade: /* 4 meters work in hierarchical mode with coupling */
            /* groups of 4 hierarchical meters */
            if (pol_cfg->flags & BCM_POLICER_WITH_ID) {
                /* meter has to be: meter % 4 = 0 */
                if ((policer_id % 4) & !(pol_cfg->flags & BCM_POLICER_REPLACE)) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_BADID, (_BSL_BCM_MSG("In cascade mode policer_id must be: policer_id %% 4 = 0")));
                }
            }
            break;
#endif /* BCM_ARAD_SUPPORT */
        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (DPP_POLICER_MSG("Invalid policer mode: %d \n"), pol_cfg->mode));
        }

        /* only these flags are supported for meters */
        if ((pol_cfg->flags & ~legal_flags) != 0) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (DPP_POLICER_MSG("illegal flags specified")));
        }

        rv = bcm_petra_meter_max_info(
           unit,
           &max_pol_cfg);
        BCMDNX_IF_ERR_EXIT(rv);
    } else {
        /* 
         * ethernet policer, has one bucket (commited), 
         *   in-profile: don't change color
         *   out-of-profile: red
         */

        if (pol_cfg->pkbits_burst != 0 || pol_cfg->pkbits_sec != 0) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("pkbits_burst and pkbits_sec must be zero"))); /* has to be zero */
        }
        if (pol_cfg->mode != bcmPolicerModeSrTcm) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("only bcmPolicerModeSrTcm mode is supported"))); /* only this mode is supported for ethernet policer */
        }

        /* only these flags are supported for ethernet policer */
        if (pol_cfg->flags & (~(BCM_POLICER_REPLACE | BCM_POLICER_WITH_ID | BCM_POLICER_MODE_BYTES | BCM_POLICER_MODE_PACKETS | BCM_POLICER_COLOR_BLIND))) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("one or more of the flags is not supported"))); /* not supported */
        }

        /* only these flags are supported for ethernet policer */
        if ((pol_cfg->flags & BCM_POLICER_MODE_PACKETS) && (pol_cfg->flags & BCM_POLICER_MODE_BYTES)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("flags BCM_POLICER_MODE_PACKETS and BCM_POLICER_MODE_BYTES cannot be both set"))); /* cannot be both */
        }

        rv = bcm_petra_policer_max_info(
           unit,
           &max_pol_cfg);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    rv = bcm_petra_max_info_check(unit, pol_cfg, &max_pol_cfg);
    BCMDNX_IF_ERR_EXIT(rv);

    

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function
 *      _bcm_dpp_policer_group_num_get
 * Purpose
 *      Get the number of policers in the group mode
 * Parameters
 *      (in) unit       = unit number
 *      (in) mode       = group mode
 *      (out) npolicers = number of policers
 * Returns
 *      bcm_error_t = BCM_E_NONE if no error
 *                    BCM_E_* appropriately if not 
 */
STATIC int
_bcm_dpp_policer_group_num_get(int unit, bcm_policer_group_mode_t mode,
                                  int *npolicers)
{
    int num;
    BCMDNX_INIT_FUNC_DEFS;

    if (mode >= _DPP_POLICER_GROUP_MODES) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("%s: not supported policer mode  \n"), FUNCTION_NAME()));
    }
    num = _dpp_policer_group_num_policers[mode];

    if (num < 0) {
        /* Unsupported mode specified */
        BCM_ERR_EXIT_NO_MSG(BCM_E_PARAM);
    } else {
        *npolicers = num;
    }
exit:
    BCMDNX_FUNC_RETURN;
}

#ifdef BCM_88660_A0

/** 
 *  Correct all rates in pol_cfg by the specified multiplier.
 *  
 *  @pol_cfg The configuration to correct.
 *  @multiplier The amount to multiply or divide by.
 *  @divide If TRUE then divide the rates by @multiplier.
 *          Otherwise multiply by it.
 *  
 */
STATIC void _bcm_dpp_policer_correct_config_rates(bcm_policer_config_t *pol_cfg, int multiplier, int divide)
{
    int i = 0;
    uint32 *rates[6];
    rates[0] = &pol_cfg->ckbits_burst;
    rates[1] = &pol_cfg->ckbits_sec;
    rates[2] = &pol_cfg->max_ckbits_sec;
    rates[3] = &pol_cfg->pkbits_burst;
    rates[4] = &pol_cfg->pkbits_sec;
    rates[5] = &pol_cfg->max_pkbits_sec;

    for (i = 0; i < sizeof(rates) / sizeof(rates[0]); i++) {
        if (divide) {
            *(rates[i]) /= multiplier;
        } else {
            *(rates[i]) *= multiplier;
        }
    }
}

#endif /* BCM_88660_A0 */

/* given API meter info, set
 *   - SW-state meter info
 *   - ppd meter info
 */
STATIC int bcm_petra_meter_to_inter_info(
   int unit,
   const bcm_policer_config_t      *pol_cfg_user,
   SOC_PPD_MTR_BW_PROFILE_INFO   *dpp_profile_info,
   bcm_dpp_am_meter_entry_t  *sw_state_info
   ) 
{
    /* Copy the config to a temp variable since it may change (times 64) if PACKET_MODE is set. */
    bcm_policer_config_t pol_cfg = *pol_cfg_user;

    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(dpp_profile_info);
    BCMDNX_NULL_CHECK(sw_state_info);

    /* Set the packet mode in case of Arad+ only and change the rates from */
    /* kbits per sec to packets (with the defined resolution) per sec */
#ifdef BCM_88660_A0
    if (SOC_IS_ARADPLUS(unit)) {
        if (pol_cfg.flags & BCM_POLICER_MODE_PACKETS) {
            /* Divide the values by _DPP_POLICER_PACKET_MODE_CORRECTION to fool bcm_petra_meter_to_inter_info into thinking */
            /* that the user has made the multiplication. */
            _bcm_dpp_policer_correct_config_rates(&pol_cfg, _DPP_POLICER_PACKET_MODE_CORRECTION, TRUE);

        }
    }
#endif /* BCM_88660_A0 */

    /* sw state allocate unique profile according to this infomration */
    sw_state_info->cir = pol_cfg.ckbits_sec;
    sw_state_info->eir = pol_cfg.pkbits_sec;
    sw_state_info->cbs = pol_cfg.ckbits_burst;
    sw_state_info->ebs = pol_cfg.pkbits_burst;
    /* store flags except SW flags*/
    sw_state_info->flags = pol_cfg.flags & (~(BCM_POLICER_WITH_ID | BCM_POLICER_REPLACE | BCM_POLICER_REPLACE_SHARED));
    sw_state_info->mode = pol_cfg.mode;
    sw_state_info->entropy_id = pol_cfg.entropy_id;

    /* DPP state, configuration of the device */
    SOC_PPD_MTR_BW_PROFILE_INFO_clear(dpp_profile_info);

    if (SOC_IS_ARAD(unit)) {
        if (pol_cfg.mode == bcmPolicerModeCascade || pol_cfg.mode == bcmPolicerModeCoupledCascade) {
            dpp_profile_info->is_sharing_enabled = TRUE;
        }
    }

    /* commited rate set */
    if (pol_cfg.ckbits_sec == 0) { /* zero, means disable credits, yet set to minimum cir */
        dpp_profile_info->cir = SOC_DPP_CONFIG(unit)->meter.meter_min_rate;
        dpp_profile_info->disable_cir = TRUE;
    } else {
        dpp_profile_info->cir = pol_cfg.ckbits_sec;
        dpp_profile_info->disable_cir = FALSE;
    }

	/* commited max rate, applicable only in hierarchical modes*/
	if (SOC_IS_ARAD(unit) && dpp_profile_info->is_sharing_enabled) {
		/* max_cir can exist also when disable_cir = TRUE since it will get extra credits from higher buckets */
		if (0 != pol_cfg.max_ckbits_sec) {
			dpp_profile_info->max_cir = pol_cfg.max_ckbits_sec;
		}
		else{
			dpp_profile_info->max_cir = SOC_DPP_CONFIG(unit)->meter.meter_min_rate; /*disabled, set to minimum*/
		}
	}

    /* coupling depends on policer mode */
    if (pol_cfg.mode == bcmPolicerModeSrTcm || pol_cfg.mode ==  bcmPolicerModeCoupledTrTcmDs) {
        dpp_profile_info->is_coupling_enabled = TRUE;
    } else {
        dpp_profile_info->is_coupling_enabled = FALSE;
    }
    dpp_profile_info->is_mtr_enabled = 1;

    /* excess/peak rate set */
    if (pol_cfg.pkbits_sec == 0) { /* zero, means disable credits, yet set to minimum cir */
        dpp_profile_info->eir = SOC_DPP_CONFIG(unit)->meter.meter_min_rate;
        dpp_profile_info->disable_eir = TRUE;
    } else {
        dpp_profile_info->disable_eir = FALSE;
        dpp_profile_info->eir = pol_cfg.pkbits_sec;
    }
    if (SOC_IS_ARAD(unit)) { /* set max_eir as well */
        if ((dpp_profile_info->disable_eir == FALSE) || (dpp_profile_info->is_coupling_enabled == TRUE)) {
            /* max_eir can exist also when disable_eir = TRUE, because if coupling is enabled the bucket gets extra credits from cir */
            dpp_profile_info->max_eir = pol_cfg.max_pkbits_sec;
        } else {
            dpp_profile_info->max_eir = dpp_profile_info->eir;
        }
    }

    dpp_profile_info->ebs = _DPP_POLICER_KBITS_TO_BYTES(pol_cfg.pkbits_burst); /* kbits to bytes*/
    dpp_profile_info->cbs = _DPP_POLICER_KBITS_TO_BYTES(pol_cfg.ckbits_burst); /* kbits to bytes*/
    dpp_profile_info->color_mode = (pol_cfg.flags & BCM_POLICER_COLOR_BLIND) ? SOC_PPD_MTR_COLOR_MODE_BLIND : SOC_PPD_MTR_COLOR_MODE_AWARE;

    if (SOC_IS_PETRAB(unit)) {
        /* always in fair mode */
        dpp_profile_info->is_fairness_enabled = TRUE;
    }

    /* fix for dpp access */
    if (dpp_profile_info->ebs < SOC_DPP_CONFIG(unit)->meter.min_burst) {
        dpp_profile_info->ebs = SOC_DPP_CONFIG(unit)->meter.min_burst;
    }

    if (dpp_profile_info->cbs < SOC_DPP_CONFIG(unit)->meter.min_burst) {
        dpp_profile_info->cbs = SOC_DPP_CONFIG(unit)->meter.min_burst;
    }

#ifdef BCM_88660_A0
    if (SOC_IS_ARADPLUS(unit)) {
      dpp_profile_info->is_packet_mode = (pol_cfg.flags & BCM_POLICER_MODE_PACKETS) ? TRUE : FALSE;
    }
#endif /* BCM_88660_A0 */

exit:
    BCMDNX_FUNC_RETURN;
}


/* given DPP meter info, set
 *   - API info
 */
STATIC int bcm_petra_meter_from_inter_info(
   int unit,
   SOC_PPD_MTR_METER_ID          meter_id,
   bcm_policer_config_t      *pol_cfg,
   SOC_PPD_MTR_BW_PROFILE_INFO   *dpp_profile_info
   ) 
{
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    soc_mem_t mem;
    int array_index, index;
    uint32 mem_val[2];
    uint32 global_sharing = 0;

    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(dpp_profile_info);
    BCMDNX_NULL_CHECK(pol_cfg);

    bcm_policer_config_t_init(pol_cfg);

    if (meter_id.group == 1) {
        pol_cfg->flags |= BCM_POLICER_MACRO;
    }

    /* commited rate set */
    if (dpp_profile_info->disable_cir == TRUE) { /* zero, means disable credits, yet set to minimum cir */
        pol_cfg->ckbits_sec = 0;
    } else {
        pol_cfg->ckbits_sec = dpp_profile_info->cir;
        if (SOC_IS_ARAD(unit) && dpp_profile_info->is_sharing_enabled) {
            pol_cfg->max_ckbits_sec = dpp_profile_info->max_cir;
        }
    }

    /* excess/peak rate set */
    if (dpp_profile_info->disable_eir == TRUE) { /* zero, means disable credits, yet set to minimum cir */
        pol_cfg->pkbits_sec = 0;
    } else {
        pol_cfg->pkbits_sec = dpp_profile_info->eir;
    }
    if (SOC_IS_ARAD(unit) && ((dpp_profile_info->disable_eir == FALSE) || (dpp_profile_info->is_coupling_enabled == TRUE))) {
        /* max_eir is valid either when eir is enabled or when coupling is set */
        pol_cfg->max_pkbits_sec = dpp_profile_info->max_eir;
    }

    pol_cfg->pkbits_burst = _DPP_POLICER_BYTES_TO_KBITS(dpp_profile_info->ebs); /* bytes to kbits */
    pol_cfg->ckbits_burst = _DPP_POLICER_BYTES_TO_KBITS(dpp_profile_info->cbs); /* bytes to kbits */
    if (dpp_profile_info->color_mode == SOC_PPD_MTR_COLOR_MODE_BLIND) {
        pol_cfg->flags |= BCM_POLICER_COLOR_BLIND;
    }

    if (SOC_IS_ARAD(unit)) {
        soc_sand_dev_id = (unit);

        /* get whether the 4 meters are in hierarchical mode */
        if (meter_id.group == 0) {
            mem = IDR_MCDA_PRFSELm;
        } else {
            mem = IDR_MCDB_PRFSELm;
        }
        if (meter_id.id < 16384) {
            array_index = 0;
            index = meter_id.id / 4;
        } else {
            array_index = 1;
            index = (meter_id.id - 16384) / 4;
        }

        /* get the entry */
        soc_sand_rv = soc_mem_array_read(soc_sand_dev_id, mem, array_index, MEM_BLOCK_ANY, index, mem_val);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        /* set the global_sharing */
        soc_mem_field_get(soc_sand_dev_id, mem, mem_val, GLOBAL_SHARINGf, &global_sharing);
    }

    /* coupling depends on policer mode */
    if (pol_cfg->pkbits_sec == 0 && dpp_profile_info->is_coupling_enabled == TRUE) {
        pol_cfg->mode = bcmPolicerModeSrTcm;
    } else if (pol_cfg->pkbits_sec == 0 && dpp_profile_info->ebs <= SOC_DPP_CONFIG(unit)->meter.min_burst) {
        pol_cfg->mode = bcmPolicerModeCommitted;
    } else if (dpp_profile_info->is_coupling_enabled == TRUE) {

        pol_cfg->mode = bcmPolicerModeCoupledTrTcmDs;
    } else {
        pol_cfg->mode = bcmPolicerModeTrTcmDs;
    }
    if (SOC_IS_ARAD(unit) && (dpp_profile_info->is_sharing_enabled == TRUE)) { /* cascade */
        if (global_sharing) {
            pol_cfg->mode = bcmPolicerModeCoupledCascade;
        } else {
            pol_cfg->mode = bcmPolicerModeCascade;
        }
    }

#ifdef BCM_88660_A0
    if (SOC_IS_ARADPLUS(unit)) {
        /* In packet mode the values are artifically divided by _DPP_POLICER_PACKET_MODE_CORRECTION and so must be scaled back to user values */
        if (dpp_profile_info->is_packet_mode) {
            _bcm_dpp_policer_correct_config_rates(pol_cfg, _DPP_POLICER_PACKET_MODE_CORRECTION, FALSE);
        }
    }
#endif /* BCM_88660_A0 */

exit:
    BCMDNX_FUNC_RETURN;
}



/* 
 * Given ppd info, allocate bw profile and map meter to it.
 *  
 * change_profile - Should the data of the profile be set ? 
 *  
 * if *new_profile is not 0 then:
 * 1) a. If the profile is not allocated and change_profile then allocate and set data.
 *    b. If the profile is not allocated and !change_profile then return BCM_E_NOT_FOUND.
 *    c. If allocated and change_profile then change the profile data.
 * 2) Change the profile of the meter to *new_profile.
 * 
 * If *new_profile != 0 and the old profile was user managed (entropy_id == 0) then
 * BCM_E_PARAM is returned. 
 * This is to protect the user from accidentally reusing auto managed profiles for 
 * user managed profiles. 
 *  
 */
STATIC int bcm_petra_meter_profile_alloc(
   int                            unit,
   int                            group,
   int                            meter,
   bcm_dpp_am_meter_entry_t      *sw_state_info,
   SOC_PPD_MTR_BW_PROFILE_INFO   *dpp_profile_info,
   int                            is_high_rate,
   int                           *new_profile,
   int                           *old_profile,
   int                            change_profile) 
{
    int old_is_high_rate = 0;
    SOC_PPD_MTR_METER_ID meter_id;
    SOC_SAND_SUCCESS_FAILURE failure_indication;
    int rv = BCM_E_NONE;
    SOC_PPD_MTR_BW_PROFILE_INFO exact_dpp_profile_info;
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    int is_last = 0;
    int is_allocated = 0;
    uint32 tmp_old_profile;

    BCMDNX_INIT_FUNC_DEFS;

    /* change_profile only valid with new_profile */
    if (change_profile && (*new_profile == 0)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (DPP_POLICER_MSG("BUG: change_profile is only valid in combination with new_profile\n")));
    }
    
    BCMDNX_NULL_CHECK(sw_state_info);

    soc_sand_dev_id = (unit);

    SOC_PPD_MTR_METER_ID_clear(&meter_id);
    meter_id.group = group;
    meter_id.id = meter;

    /* check if old setting was to high rate */
    soc_sand_rv = soc_ppd_mtr_meter_ins_to_bw_profile_map_get(
       soc_sand_dev_id,
       &meter_id,
       &tmp_old_profile
       );
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    *old_profile = tmp_old_profile;

    if (SOC_IS_PETRAB(unit)) {
        old_is_high_rate = _DPP_POLICER_PROFILE_IS_HR(unit, *old_profile);
    }

    /* if use same high/low rate use exchange */
    /* This is always true for arad */
    if (old_is_high_rate == is_high_rate) {
        
        /* Check that the profile we need to set isn't being used for auto management. */
        if (*new_profile != 0) {
            bcm_dpp_am_meter_entry_t old_sw_state_info;
            rv = _bcm_dpp_am_template_meter_data_get(unit, *new_profile, group, is_high_rate, &old_sw_state_info);
            if (rv != BCM_E_NOT_FOUND) {
                BCMDNX_IF_ERR_EXIT(rv);

                if (old_sw_state_info.entropy_id == 0) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (DPP_POLICER_MSG("Profile %d can not be used since it is already in use by auto management. \n"),
                                                                   *new_profile));
                }
            }

        }
        rv = _bcm_dpp_am_template_meter_exchange(unit, *old_profile, group, is_high_rate, sw_state_info, &is_last, new_profile, change_profile, &is_allocated);
        BCMDNX_IF_ERR_EXIT(rv);
    } else {
        /* if not, alloc in new rate */
        rv = _bcm_dpp_am_template_meter_alloc(unit, 0, group, is_high_rate, sw_state_info, new_profile, &is_allocated);
        BCMDNX_IF_ERR_EXIT(rv);
        /* remove old rate */
        rv = _bcm_dpp_am_template_meter_free(unit, *old_profile, group, old_is_high_rate, &is_last);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    /* update profile:
       porfile 0 cannot be allocated/deallocated (it exists from the start)
       no need to add profile 0 is was already done in init */
    if ((is_allocated == 1) && (*new_profile != 0)) {
        soc_sand_rv = soc_ppd_mtr_bw_profile_add(
           soc_sand_dev_id,
           group,
           *new_profile,
           dpp_profile_info,
           &exact_dpp_profile_info,
           &failure_indication);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        SOC_SAND_IF_FAIL_RETURN(failure_indication);
    }

    if ((is_allocated == 1) || (*new_profile == 0) /* porfile 0 cannot be allocated/deallocated (it exists from the start) */
        || (old_is_high_rate == is_high_rate)/* if associate a new meter-idx with existing profile, is_allocated will return 0 
       but we still need to associate the new meter-id with the exiting profile. */) {

        /* map meter to new profile */
        soc_sand_rv = soc_ppd_mtr_meter_ins_to_bw_profile_map_set(
           soc_sand_dev_id,
           &meter_id,
           *new_profile
           );
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

#ifdef BCM_ARAD_SUPPORT
STATIC void _bcm_petra_meter_profile_min_max_rates_get(
   SOC_PPD_MTR_BW_PROFILE_INFO   dpp_profile_info,
   uint32  *min_rate,
   uint32  *max_rate) 
{
    if (dpp_profile_info.max_cir != ARAD_PP_BW_PROFILE_IR_MAX_UNLIMITED) {
        *max_rate = dpp_profile_info.max_cir;
    } else {
        *max_rate = dpp_profile_info.cir;
    }
    if ((dpp_profile_info.max_eir != ARAD_PP_BW_PROFILE_IR_MAX_UNLIMITED) && (*max_rate < dpp_profile_info.max_eir)) {
        *max_rate = dpp_profile_info.max_eir;
    } else if (*max_rate < dpp_profile_info.eir) {
        *max_rate = dpp_profile_info.eir;
    }

    if (!(dpp_profile_info.disable_cir)) {
        *min_rate = dpp_profile_info.cir;
    }
    if (!(dpp_profile_info.disable_eir) && (*min_rate > dpp_profile_info.eir)) {
        *min_rate = dpp_profile_info.eir;
    } else if (*min_rate > dpp_profile_info.max_eir) {
        *min_rate = dpp_profile_info.max_eir;
    }
}

/* 
 * get the min/max rates of the 3 other meters
 * since changing the rate of one of the meters, affects all 4 meters (rev-exp is shared)
 * the function will recalculate the rates of the 3 other meters
 */
STATIC int _bcm_petra_hierarchical_meters_max_min_rates_get_and_update(
   int unit,
   int meter_id, /* one of the 4 hierarchical_meters. min/max rates will be of the 3 other meters */
   int group,
   SOC_PPD_MTR_BW_PROFILE_INFO   *dpp_profile_info) 
{
    SOC_PPD_MTR_BW_PROFILE_INFO   dpp_other_profile_info, dpp_other_exact_profile_info;
    int i;
    uint32 profile;
    SOC_PPD_MTR_METER_ID other_meter;
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    uint32 min_rate = 0, max_rate, /* of the current meter */
       other_min_rate = 0, other_max_rate; /* of another meter */
    SOC_SAND_SUCCESS_FAILURE failure_indication;

    BCMDNX_INIT_FUNC_DEFS;

    soc_sand_dev_id = (unit);

    SOC_PPD_MTR_METER_ID_clear(&other_meter);

    /* calc min and max rate of meter_id */
    _bcm_petra_meter_profile_min_max_rates_get(
       *dpp_profile_info,
       &min_rate,
       &max_rate);

    dpp_profile_info->hierarchical_max_rate = max_rate;
    dpp_profile_info->hierarchical_min_rate = min_rate;

    other_meter.group = group;
    for (i = 0; i < 4; i++) {
        other_meter.id = meter_id - (meter_id % 4) + i;
        if (other_meter.id != meter_id) {

            /* get the meters profile */
            soc_sand_rv = soc_ppd_mtr_meter_ins_to_bw_profile_map_get(
               soc_sand_dev_id,
               &other_meter,
               &profile
               );
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            /* get the profile data */
            soc_sand_rv = soc_ppd_mtr_bw_profile_get(
               soc_sand_dev_id,
               group,
               profile,
               &dpp_other_profile_info
               );
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            /* calc min and max rate for profile */
            _bcm_petra_meter_profile_min_max_rates_get(
               dpp_other_profile_info,
               &other_min_rate,
               &other_max_rate);

            if (dpp_profile_info->hierarchical_max_rate < other_max_rate) {
                dpp_profile_info->hierarchical_max_rate = other_max_rate;
            }
            if (dpp_profile_info->hierarchical_min_rate > other_min_rate) {
                dpp_profile_info->hierarchical_min_rate = other_min_rate;
            }

            /* check if new meter configuration affects other meter */
            if ((max_rate > other_max_rate) || (min_rate < other_min_rate)) {

                /* recalculate other meter */
                if (max_rate > other_max_rate) {
                    dpp_profile_info->hierarchical_max_rate = other_max_rate;
                }
                if (min_rate < other_min_rate) {
                    dpp_profile_info->hierarchical_min_rate = other_min_rate;
                }
                soc_sand_rv = soc_ppd_mtr_bw_profile_add(
                   soc_sand_dev_id,
                   group,
                   profile,
                   &dpp_other_profile_info,
                   &dpp_other_exact_profile_info,
                   &failure_indication
                   );
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                SOC_SAND_IF_FAIL_EXIT(failure_indication);
            }
        }
    }
exit:
    BCMDNX_FUNC_RETURN;
}
#endif /* BCM_ARAD_SUPPORT */

/*
 * configure MRPS meter (vs ethernet policer)
 */
STATIC int _bcm_petra_meter_set(
   int unit,
   bcm_policer_config_t *pol_cfg,
   bcm_policer_t *policer_id) 
{
    int rv = BCM_E_NONE;
    int am_flags = 0;
    int act_meter_group;
    int meter = 0;
    int meter_id;
    int group = 0;
    int nof_meters = 1, meter_id_incr;
    SOC_PPD_MTR_BW_PROFILE_INFO   dpp_profile_info;
    bcm_dpp_am_meter_entry_t  sw_state_info;

    /* For PB only */
    int is_high_rate = 0;

    int old_profile;

    /* We expect a non-zero value here only for Arad. */
    int new_profile_initial = pol_cfg->entropy_id;
    int new_profile;
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    soc_mem_t mem;
    int array_index, index;
    uint32 mem_val[2];
    uint32 global_sharing = 0;
    int range_mode;

    /* We expect a non-zero value here only for Arad. */
    int change_profile = (pol_cfg->flags & BCM_POLICER_REPLACE_SHARED) ? TRUE : FALSE;

    BCMDNX_INIT_FUNC_DEFS;

    /* allocate policer ID */
    if (pol_cfg->flags & BCM_POLICER_WITH_ID) {
        am_flags |= SHR_RES_ALLOC_WITH_ID;

        /* get the meter (0-(8K-1) in PB, 0-(32K-1) in ARAD) and the group (0/1) from policer_id */
        meter_id = *policer_id;
        group = _DPP_POLICER_ID_TO_GROUP(unit, meter_id);
        meter = _DPP_POLICER_ID_TO_METER(unit, meter_id);
    }

    if (!(pol_cfg->flags & BCM_POLICER_REPLACE)) {
        /* for both devices there's another way of getting the group */
        if (pol_cfg->flags & BCM_POLICER_MACRO) {
            group = 1;
        }
        /* otherwise group = 0 (in 32K range mode) */
    }

    if (!(pol_cfg->flags & BCM_POLICER_WITH_ID)) {
        /* get meters range mode (64K/32K) */
        range_mode = soc_property_get(unit, spn_POLICER_INGRESS_COUNT, 64);
        if (range_mode == 64) {
            /* in 64K range mode, meters are not divided into 2 groups, so group has no meaning */
            group = BCM_DPP_AM_METER_GROUP_ANY;
        }
    }

    if (pol_cfg->mode == bcmPolicerModeCascade || pol_cfg->mode == bcmPolicerModeCoupledCascade) {
        nof_meters = 4; /* allocate 4 meters, first one % 4 = 0 */
        if (pol_cfg->mode == bcmPolicerModeCoupledCascade) {
            global_sharing = 1; /* 4 meters are in hierarchical mode */
        }
    }

    /* conver API parameters to internal hw/sw structures */
    rv = bcm_petra_meter_to_inter_info(
       unit,
       pol_cfg,
       &dpp_profile_info,
       &sw_state_info);
    BCMDNX_IF_ERR_EXIT(rv);

    if (pol_cfg->flags & BCM_POLICER_REPLACE) {

        if (nof_meters == 4) {
            rv = bcm_dpp_am_meter_is_alloced(
               unit,
               group,
               nof_meters,
               meter - (meter % 4));
        } else {
            rv = bcm_dpp_am_meter_is_alloced(
               unit,
               group,
               nof_meters,
               meter);
        }
        if (rv == BCM_E_NOT_FOUND) {
            LOG_ERROR(BSL_LS_BCM_POLICER,
                      (BSL_META_U(unit,
                                 " replace/destroy nonexist policer %d \n"),
                       *policer_id));
            BCMDNX_IF_ERR_EXIT(rv);
        }
        act_meter_group = group;

#ifdef BCM_ARAD_SUPPORT
        if (global_sharing) { /* 4 meters are in hierarchical mode and one of them is being replaced */

            /* get 3 other members to determine the highest and lowest rates among the 4 meters
               a change to one of the 4 meters requires a change of all 4, because rev-exp is shared */
            soc_sand_rv = _bcm_petra_hierarchical_meters_max_min_rates_get_and_update(
               unit,
               meter,
               group,
               &dpp_profile_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
#endif /* BCM_ARAD_SUPPORT */
    } else { /* allocate or mark as allocated */
        rv = bcm_dpp_am_meter_alloc(
           unit,
           am_flags,
           group,
           nof_meters,
           &act_meter_group,
           &meter);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    /* at this point meter id is allocated */
    *policer_id = _DPP_POLICER_ID_FROM_METER_GROUP(unit, meter, act_meter_group);

    if (SOC_IS_PETRAB(unit)) {
        is_high_rate = _DPP_POLICER_IS_HIGH_RATE(unit, pol_cfg);
    }

    /* set profile id to store this information */
    if (pol_cfg->flags & BCM_POLICER_REPLACE) {
        /* in case of replace, only chage the rates of one of the 4 meters group */
        nof_meters = 1;
    }
    for (meter_id_incr = 0; meter_id_incr < nof_meters; ++meter_id_incr) {
        new_profile = new_profile_initial;
        
        rv = bcm_petra_meter_profile_alloc(unit, act_meter_group, meter + meter_id_incr, &sw_state_info, &dpp_profile_info, is_high_rate,
                                           &new_profile, &old_profile, change_profile);
        if (rv == BCM_E_NOT_FOUND) {
            /* This means that the old profile did not exist and change_profile != 1, which means there is no way */
            /* to know how to allocate a new profile. */
            LOG_WARN(BSL_LS_BCM_POLICER,
                     (BSL_META_U(unit,
                                 "Cannot set the profile of meter %d to profile %d - profile is not allocated.\n"
                                 "HINT: To allocate it use BCM_POLICER_REPLACE_SHARED with the new configuration for the profile.\n"), 
                      meter, *policer_id));
            
        }

        
        /* Is this an error or not? Remember rv might be overridden. */
        if (rv != BCM_E_NONE) { /* if some reason fail to allocate profile, release policer if needed */
            LOG_ERROR(BSL_LS_BCM_POLICER,
                      (BSL_META_U(unit,
                                  " fail to allocate profile for policer %d \n"),
                       *policer_id));
            if (!(pol_cfg->flags & BCM_POLICER_REPLACE)) {
                bcm_dpp_am_meter_dealloc(
                   unit,
                   0,
                   act_meter_group,
                   nof_meters,
                   meter);
            }
        }
    }

    if (SOC_IS_ARAD(unit)) {

        soc_sand_dev_id = (unit);

        /* update in table whether the 4 meters are in hierarchical mode */
        if (act_meter_group == 0) {
            mem = IDR_MCDA_PRFSELm;
        } else {
            mem = IDR_MCDB_PRFSELm;
        }
        if (meter < 16384) {
            array_index = 0;
            index = meter / 4;
        } else {
            array_index = 1;
            index = (meter - 16384) / 4;
        }

        /* get the entry */
        soc_sand_rv = soc_mem_array_read(soc_sand_dev_id, mem, array_index, MEM_BLOCK_ANY, index, mem_val);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        /* set the global_sharing */
        soc_mem_field_set(soc_sand_dev_id, mem, mem_val, GLOBAL_SHARINGf, &global_sharing);

        soc_sand_rv = soc_mem_array_write(soc_sand_dev_id, mem, array_index, MEM_BLOCK_ANY, index, mem_val);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

/*
Used in init
*/
int
bcm_petra_policer_detach(int unit) 
{
    int result = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);
    /* get rid of allocated structure */
    BCMDNX_IF_ERR_EXIT(POLICER_ACCESS.free(unit));

    BCMDNX_IF_ERR_EXIT(result);

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_policer_init(int unit) 
{
    int rv = BCM_E_NONE;
    SOC_PPD_MTR_BW_PROFILE_INFO   dpp_profile_info;
    SOC_PPD_MTR_BW_PROFILE_INFO   exact_dpp_profile_info;
    SOC_SAND_SUCCESS_FAILURE failure_indication;
    SOC_PPD_MTR_METER_ID meter_id;
    bcm_dpp_am_meter_entry_t  sw_state_info;
    int group_indx;
    int meter;
    SOC_PPD_MTR_GLBL_INFO
       mtr_glbl_info;
    SOC_PPD_MTR_GROUP_INFO
       mtr_group_info;
    uint32 soc_sand_rv;
    unsigned int soc_sand_dev_id;
    uint32 bw_profile = 0;
    int high_rate = 0;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_DPP_SWITCH_API_START(unit);

    if (SOC_IS_JERICHO(unit)) {
        goto exit;
    }
    
    sw_state_sync_db[(unit)].dpp.policer_lock = sal_mutex_create("_dpp_meter_unit_lock");
    if (!sw_state_sync_db[(unit)].dpp.policer_lock) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE, (_BSL_BCM_MSG("%s: fail to allocate lock \n"), FUNCTION_NAME()));
    }

    if (SOC_WARM_BOOT(unit)) {
        BCM_EXIT;
    }

    /* create new unit state information */
    BCMDNX_IF_ERR_EXIT(POLICER_ACCESS.alloc(unit));


    /* take lock */
    DPP_POLICER_INIT_CHECK(unit);
    DPP_POLICER_UNIT_LOCK(unit);

    SOC_PPD_MTR_BW_PROFILE_INFO_clear(&dpp_profile_info);
    SOC_PPD_MTR_METER_ID_clear(&meter_id);
    SOC_PPD_MTR_GLBL_INFO_clear(&mtr_glbl_info);
    SOC_PPD_MTR_GROUP_INFO_clear(&mtr_group_info);

    soc_sand_dev_id = (unit);

    if (!SOC_WARM_BOOT(unit)) {
        /* enable policing */
        soc_sand_rv = soc_ppd_mtr_eth_policer_enable_set(soc_sand_dev_id, TRUE);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        if (SOC_IS_PETRAB(unit)) {
            mtr_glbl_info.update_all_copies = 1;
            mtr_group_info.is_hr_enabled = 1;
            mtr_group_info.max_packet_size = 16 * 1024 - 1;

            soc_sand_rv = soc_ppd_mtr_glbl_info_set(soc_sand_dev_id, &mtr_glbl_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }

        /* allocate profile zero, from high rate with maximum bndwidth */
        /* map all meters to max rate */
        dpp_profile_info.color_mode = SOC_PPD_MTR_COLOR_MODE_AWARE;
        dpp_profile_info.disable_cir = FALSE;
        dpp_profile_info.disable_eir = FALSE;
        dpp_profile_info.ebs = SOC_DPP_CONFIG(unit)->meter.max_burst;
        dpp_profile_info.eir = SOC_DPP_CONFIG(unit)->meter.max_rate;
        dpp_profile_info.cbs = SOC_DPP_CONFIG(unit)->meter.max_burst;
        dpp_profile_info.cir = SOC_DPP_CONFIG(unit)->meter.max_rate;
        if (SOC_IS_PETRAB(unit)) {
            dpp_profile_info.is_fairness_enabled = TRUE;
        } else { /* ARAD */
            dpp_profile_info.max_eir = SOC_DPP_CONFIG(unit)->meter.max_rate;
            dpp_profile_info.is_fairness_enabled = FALSE;
            dpp_profile_info.is_sharing_enabled = FALSE; 
        }
        dpp_profile_info.is_coupling_enabled = TRUE;

        /* sw state allocate unique profile according to this infomration */
        sw_state_info.cir = dpp_profile_info.cir;
        sw_state_info.eir = dpp_profile_info.eir;
        sw_state_info.cbs = _DPP_POLICER_BYTES_TO_KBITS(dpp_profile_info.cbs);
        sw_state_info.ebs = _DPP_POLICER_BYTES_TO_KBITS(dpp_profile_info.ebs);
        sw_state_info.flags = 0;
        sw_state_info.mode = bcmPolicerModeTrTcmDs;
        sw_state_info.entropy_id = 0;

        for (group_indx = 0; group_indx <= 1; ++group_indx) {

            if (SOC_IS_PETRAB(unit)) {
                soc_sand_rv = soc_ppd_mtr_meters_group_info_set(
                   soc_sand_dev_id,
                   group_indx,
                   &mtr_group_info);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            }

            /* set first profile in high rate, for full rate, 'as no disable' */
            if (SOC_IS_PETRAB(unit)) {
                bw_profile = _DPP_POLICER_METER_HR_PROFILE_MIN(unit);
                high_rate = 1;
            } else { /* ARAD */
                /* there are no high-rate profiles, so profile 0 is configured */
                bw_profile = 0;
                high_rate = 0;
            }
            soc_sand_rv = soc_ppd_mtr_bw_profile_add(
               soc_sand_dev_id,
               group_indx,
               bw_profile,
               &dpp_profile_info,
               &exact_dpp_profile_info,
               &failure_indication
               );
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            SOC_SAND_IF_FAIL_EXIT(failure_indication);
            meter_id.group = group_indx;

            if (SOC_IS_PETRAB(unit)) {
              /* map all meters to use this profile */
              for (meter = 0; meter < SOC_DPP_CONFIG(unit)->meter.nof_meter_a; ++meter) {
                  meter_id.id = meter;
                  soc_sand_rv = soc_ppd_mtr_meter_ins_to_bw_profile_map_set(
                     soc_sand_dev_id,
                     &meter_id,
                     bw_profile
                     );
                  BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
              }
            }

        }
    }

    rv = _bcm_dpp_am_template_meter_init(unit, bw_profile, high_rate, &sw_state_info);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = bcm_petra_rate_init(unit);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    if (unit >= 0 && unit < BCM_MAX_NUM_UNITS) {
        DPP_POLICER_UNIT_UNLOCK(unit);
    }
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

/*
 * use to create "meter" to create ethernet policer use  group_create
 */
int
bcm_petra_policer_create(int unit, bcm_policer_config_t *pol_cfg,
                         bcm_policer_t *policer_id) 
{
    int rv = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;
    _BCM_DPP_SWITCH_API_START(unit);

    /* verify parameters */
    DPP_POLICER_INIT_CHECK(unit);

    DPP_POLICER_UNIT_LOCK(unit);

    rv = _bcm_petra_polcer_check_config(unit, pol_cfg, 0, *policer_id);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = _bcm_petra_meter_set(unit, pol_cfg, policer_id);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    if (unit >= 0 && unit < BCM_MAX_NUM_UNITS) {
        DPP_POLICER_UNIT_UNLOCK(unit);
    }
    BCMDNX_FUNC_RETURN;
}


int bcm_petra_policer_group_create(
   int unit,
   bcm_policer_group_mode_t mode,
   bcm_policer_t *policer_id,
   int *npolicers) 
{
    int rv = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;

    DPP_POLICER_INIT_CHECK(unit);

    /* how many policers needed for this mode */
    rv = _bcm_dpp_policer_group_num_get(unit, mode, npolicers);
    BCMDNX_IF_ERR_EXIT(rv);

    DPP_POLICER_UNIT_LOCK(unit);

    rv = bcm_dpp_am_policer_alloc(unit, 0, *npolicers, policer_id);
    BCMDNX_IF_ERR_EXIT(rv);

    BCMDNX_IF_ERR_EXIT(POLICER_ACCESS.policer_group.mode.set(unit, *policer_id, mode));

    *policer_id = _DPP_ETH_POLICER_ID_SET(unit, *policer_id);

exit:
    if (unit >= 0 && unit < BCM_MAX_NUM_UNITS) {
        DPP_POLICER_UNIT_UNLOCK(unit);
    }
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}



/* get policer-id from opposit group */
int bcm_petra_policer_envelop_create(
   int unit,
   uint32 flag,
   bcm_policer_t macro_flow_policer_id,
   bcm_policer_t *policer_id) 
{

    int act_meter_group;
    int meter;
    int group;
    int rv = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;

    _BCM_DPP_SWITCH_API_START(unit);
    DPP_POLICER_INIT_CHECK(unit);

    group = _DPP_POLICER_ID_TO_GROUP(unit, macro_flow_policer_id);
    meter = _DPP_POLICER_ID_TO_METER(unit, macro_flow_policer_id);

    /* check parameters */
    if (_DPP_IS_ETH_POLICER_ID(unit, macro_flow_policer_id)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (DPP_POLICER_MSG("envelop is not supported for ethernet policer \n")));
    }

    /* flags is ingnored, as micro/macro determined according to meter usage */

    /* take semaphore */
    DPP_POLICER_UNIT_LOCK(unit);

    /* check if policer is allocated */
    rv = bcm_dpp_am_meter_is_alloced(
       unit,
       group,
       1,
       meter);
    if (rv == BCM_E_NOT_FOUND) {
        LOG_ERROR(BSL_LS_BCM_POLICER,
                  (BSL_META_U(unit, 
                              " replace/destroy nonexist policer %d \n"),
                   macro_flow_policer_id));
            BCMDNX_IF_ERR_EXIT(rv);
    }

    /* alloc from inverse group */
    rv = bcm_dpp_am_meter_alloc(
       unit,
       0,
       1 - group, /* assuming group: 0,1*/
       1,
       &act_meter_group,
       &meter);
    BCMDNX_IF_ERR_EXIT(rv);

    *policer_id = _DPP_POLICER_ID_FROM_METER_GROUP(unit, meter, act_meter_group);


exit:
    if (unit >= 0 && unit < BCM_MAX_NUM_UNITS) {
        DPP_POLICER_UNIT_UNLOCK(unit);
    }
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

/* used for both, policer/meter according to policer-id*/
int
bcm_petra_policer_set(int unit, bcm_policer_t policer_id, bcm_policer_config_t *pol_cfg) 
{
    int policer_indx;
    SOC_PPD_MTR_BW_PROFILE_INFO policer_info;
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    int rv = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;

    _BCM_DPP_SWITCH_API_START(unit);
    /* check parameters */
    DPP_POLICER_INIT_CHECK(unit);

    BCMDNX_NULL_CHECK(pol_cfg);

    policer_indx = _DPP_ETH_POLICER_ID_GET(unit, policer_id);

    /* if not ethenet policer then call policer_create with_id*/
    if (!_DPP_IS_ETH_POLICER_ID(unit, policer_id)) {
        pol_cfg->flags |= (BCM_POLICER_WITH_ID | BCM_POLICER_REPLACE);
        BCMDNX_IF_ERR_EXIT(bcm_petra_policer_create(unit, pol_cfg, &policer_id));
        BCM_EXIT;
    }

    soc_sand_dev_id = (unit);
    if (policer_indx > _DPP_POLICER_MAX_POLICER_ID || policer_indx < _DPP_POLICER_MIN_POLICER_ID) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("policer_id is invalid")));
    }
    /* common check */
    rv = _bcm_petra_polcer_check_config(unit, pol_cfg, 1, policer_id);
    BCMDNX_IF_ERR_EXIT(rv);

    /* take lock */
    DPP_POLICER_UNIT_LOCK(unit);

    rv = bcm_dpp_am_policer_is_alloc(unit, 0, policer_indx);
    if (rv != BCM_E_EXISTS) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (DPP_POLICER_MSG("policer is not allocated %d \n"), policer_id));
    }
    /* reset rv */
    rv = BCM_E_NONE;

    SOC_PPD_MTR_BW_PROFILE_INFO_clear(&policer_info);
    policer_info.cir = pol_cfg->ckbits_sec;
    policer_info.cbs = _DPP_POLICER_KBITS_TO_BYTES(pol_cfg->ckbits_burst);
    policer_info.is_packet_mode = pol_cfg->flags & BCM_POLICER_MODE_PACKETS ? TRUE : FALSE;

    if (SOC_IS_ARADPLUS(unit)) { 
      policer_info.color_mode = pol_cfg->flags & BCM_POLICER_COLOR_BLIND ? SOC_PPC_MTR_COLOR_MODE_BLIND : SOC_PPC_MTR_COLOR_MODE_AWARE;
    } 
    soc_sand_rv = soc_ppd_mtr_eth_policer_glbl_profile_set(soc_sand_dev_id, policer_indx, &policer_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
exit:
    if (unit >= 0 && unit < BCM_MAX_NUM_UNITS) {
        DPP_POLICER_UNIT_UNLOCK(unit);
    }
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}



STATIC int
bcm_petra_eth_policer_destroy(int unit, bcm_policer_t policer_id) {

    bcm_policer_config_t pol_cfg;
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    DPP_POLICER_INIT_CHECK(unit);

    if (!_DPP_IS_ETH_POLICER_ID(unit, policer_id)) {
      BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (DPP_POLICER_MSG("0x%x is not a policer id\n"), policer_id));
    }

    policer_id = _DPP_ETH_POLICER_ID_GET(unit, policer_id);

    if (policer_id > _DPP_POLICER_MAX_POLICER_ID || policer_id < _DPP_POLICER_MIN_POLICER_ID) {
      BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (DPP_POLICER_MSG("incorrect policer id: %d \n"), policer_id));
    }

    /* update policer to maximum rate */
    rv = bcm_petra_meter_max_info(
       unit,
       &pol_cfg);
    BCMDNX_IF_ERR_EXIT(rv);

    /* These params are irrelevant for global ethernet policer. */
    pol_cfg.pkbits_sec = 0;
    pol_cfg.pkbits_burst = 0;
    pol_cfg.max_ckbits_sec = 0;
    pol_cfg.max_pkbits_sec = 0;
    pol_cfg.mode = bcmPolicerModeSrTcm;

    pol_cfg.flags = BCM_POLICER_REPLACE | BCM_POLICER_WITH_ID;

    DPP_POLICER_UNIT_LOCK(unit);

    rv = bcm_petra_policer_set(unit, _DPP_ETH_POLICER_ID_SET(unit, policer_id), &pol_cfg);
    BCMDNX_IF_ERR_EXIT(rv);
    /* MARK AS UNUSED */
    rv = bcm_dpp_am_policer_dealloc(unit, 0, 1, policer_id);
    BCMDNX_IF_ERR_EXIT(rv);


exit:
    if (unit >= 0 && unit < BCM_MAX_NUM_UNITS) {
        DPP_POLICER_UNIT_UNLOCK(unit);
    }
    BCMDNX_FUNC_RETURN;
}

STATIC int
bcm_petra_meter_destroy(int unit, bcm_policer_t policer_id) 
{
    bcm_policer_config_t pol_cfg;
    int rv;
    int nof_meters = 1;
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

    /* get the mode to see how many meter_ids to dealloc */
    rv = bcm_petra_policer_get(unit, policer_id, &pol_cfg);
    BCMDNX_IF_ERR_EXIT(rv);

    if (pol_cfg.mode == bcmPolicerModeCascade || pol_cfg.mode == bcmPolicerModeCoupledCascade) {
        nof_meters = 4; /* 4 meters, first one % 4 = 0 */
    }

    /* update policer to maximum rate */
    rv = bcm_petra_meter_max_info(
       unit,
       &pol_cfg);
    BCMDNX_IF_ERR_EXIT(rv);
    pol_cfg.flags  =  BCM_POLICER_REPLACE | BCM_POLICER_WITH_ID;

    DPP_POLICER_UNIT_LOCK(unit);

    rv = _bcm_petra_meter_set(
       unit,
       &pol_cfg,
       &policer_id);
    BCMDNX_IF_ERR_EXIT(rv);

    /* mark meter as not used */
    rv = bcm_dpp_am_meter_dealloc(
       unit,
       0,
       _DPP_POLICER_ID_TO_GROUP(unit, policer_id),
       nof_meters,
       _DPP_POLICER_ID_TO_METER(unit, policer_id));
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    if (unit >= 0 && unit < BCM_MAX_NUM_UNITS) {
        DPP_POLICER_UNIT_UNLOCK(unit);
    }
    BCMDNX_FUNC_RETURN;
}



int
bcm_petra_policer_destroy(int unit, bcm_policer_t policer_id) 
{
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_DPP_SWITCH_API_START(unit);
    /* lock is taken in called functions */
    if (_DPP_IS_ETH_POLICER_ID(unit, policer_id)) {
        BCMDNX_IF_ERR_EXIT(bcm_petra_eth_policer_destroy(unit, _DPP_ETH_POLICER_ID_GET(unit, policer_id)));
    } else {
        BCMDNX_IF_ERR_EXIT(bcm_petra_meter_destroy(unit, _DPP_ETH_METER_ID_GET(unit, policer_id)));
    }
exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}


/* for eth policer id */
STATIC int
bcm_petra_eth_policer_get(int unit, bcm_policer_t policer_id,
                          bcm_policer_config_t *pol_cfg) 
{
    int policer_indx;
    SOC_PPD_MTR_BW_PROFILE_INFO policer_info;
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    int rv = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;

    /* check parameters */

    bcm_policer_config_t_init(pol_cfg);

    soc_sand_dev_id = (unit);

    BCM_DPP_UNIT_CHECK(unit);

    policer_indx = _DPP_ETH_POLICER_ID_GET(unit, policer_id);

    if (policer_indx > _DPP_POLICER_MAX_POLICER_ID || policer_indx < _DPP_POLICER_MIN_POLICER_ID) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (DPP_POLICER_MSG("policer not exist \n")));
    }

    DPP_POLICER_UNIT_LOCK(unit);

    rv = bcm_dpp_am_policer_is_alloc(unit, 0, policer_indx);
    if (rv != BCM_E_EXISTS) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (DPP_POLICER_MSG("policer is not allocated %d \n"), policer_id));
    }
    /* reset rv */
    rv = BCM_E_NONE;

    soc_sand_rv = soc_ppd_mtr_eth_policer_glbl_profile_get(soc_sand_dev_id, policer_indx, &policer_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    if (policer_info.disable_cir) {
        pol_cfg->ckbits_sec = 0;
    } else {
        pol_cfg->ckbits_sec = policer_info.cir;
    }
    pol_cfg->ckbits_burst = _DPP_POLICER_BYTES_TO_KBITS(policer_info.cbs);

    pol_cfg->mode = bcmPolicerModeSrTcm;
    if (policer_info.is_packet_mode) {
        pol_cfg->flags |= BCM_POLICER_MODE_PACKETS;
    }
    if (policer_info.color_mode == SOC_PPC_MTR_COLOR_MODE_BLIND) {
        pol_cfg->flags |= BCM_POLICER_COLOR_BLIND;
        /* This can only be set in Arad+. */
        BCMDNX_VERIFY(SOC_IS_ARADPLUS(unit));
    }
    
exit:
    if (unit >= 0 && unit < BCM_MAX_NUM_UNITS) {
        DPP_POLICER_UNIT_UNLOCK(unit);
    }
    BCMDNX_FUNC_RETURN;
}

/*
 * no_exist_print - If 1 then print error if the meter does not exist.
 */
STATIC int
_bcm_petra_meter_get_with_opt(int unit, bcm_policer_t policer_id,
                     bcm_policer_config_t *pol_cfg, int no_exist_print) 
{
    uint32 bw_profile;
    SOC_PPD_MTR_METER_ID meter_id;
    SOC_PPD_MTR_BW_PROFILE_INFO   dpp_profile_info;
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    int rv = BCM_E_NONE;
    uint32 tmp_flags = 0;

    BCMDNX_INIT_FUNC_DEFS;

    SOC_PPD_MTR_METER_ID_clear(&meter_id);
    meter_id.group = _DPP_POLICER_ID_TO_GROUP(unit, policer_id);
    meter_id.id = _DPP_POLICER_ID_TO_METER(unit, policer_id);

    DPP_POLICER_UNIT_LOCK(unit);

    /* check if meter is exist */
    rv = bcm_dpp_am_meter_is_alloced(
       unit,
       meter_id.group,
       1,
       meter_id.id);
    if (rv != BCM_E_EXISTS) { /* meter is not exist */
        if (!no_exist_print) {
            BCM_ERR_EXIT_NO_MSG(rv);
        } else {
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }

    soc_sand_dev_id = (unit);

    /* get profile of meter */
    soc_sand_rv = soc_ppd_mtr_meter_ins_to_bw_profile_map_get(
       soc_sand_dev_id,
       &meter_id,
       &bw_profile
       );
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    /* get profile data */
    soc_sand_rv = soc_ppd_mtr_bw_profile_get(
       soc_sand_dev_id,
       meter_id.group,
       bw_profile,
       &dpp_profile_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    /* IMPORTANT NOTE: pol_cfg is OUT and we use it as INOUT. */
    /* bcm_petra_meter_from_inter_info inits the config, so we must record the flags. */
    if (SOC_IS_ARAD(unit)) {
        tmp_flags = pol_cfg->flags;
    }

    /* map profile data to bcm */
    rv = bcm_petra_meter_from_inter_info(
       unit,
       meter_id,
       pol_cfg,
       &dpp_profile_info);
    BCMDNX_IF_ERR_EXIT(rv);

    if (SOC_IS_ARAD(unit) && (tmp_flags & BCM_POLICER_REPLACE_SHARED)) {
        pol_cfg->entropy_id = bw_profile;
    }

exit:
    DPP_POLICER_UNIT_UNLOCK(unit);
    BCMDNX_FUNC_RETURN;
}

/* Default wrapper _bcm_petra_meter_get_with_opt */
STATIC int 
_bcm_petra_meter_get(int unit, bcm_policer_t policer_id,
                     bcm_policer_config_t *pol_cfg) 
{
    return _bcm_petra_meter_get_with_opt(unit, policer_id, pol_cfg, TRUE /* no_exist_print */);
}

/* for meter and policer according to policer-id */
int
bcm_petra_policer_get(int unit, bcm_policer_t policer_id,
                      bcm_policer_config_t *pol_cfg) 
{
    int rv = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;

    DPP_POLICER_INIT_CHECK(unit);

    /* if ethenet policer */
    if (_DPP_IS_ETH_POLICER_ID(unit, policer_id)) {
        rv = bcm_petra_eth_policer_get(unit, _DPP_ETH_POLICER_ID_GET(unit, policer_id), pol_cfg);
        BCMDNX_IF_ERR_EXIT(rv);
    } else {
        /* if meter */
        rv = _bcm_petra_meter_get(unit, _DPP_ETH_METER_ID_GET(unit, policer_id), pol_cfg);
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_policer_traverse(int unit, bcm_policer_traverse_cb traverse_callback,
                           void *cookie) 
{
    int policer_id;
    bcm_policer_config_t pol_cfg;
    int group_indx;
    int meter;
    int rv = BCM_E_NONE;
    int policer_indx;

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(traverse_callback);
    BCM_DPP_UNIT_CHECK(unit);

    /* over meters */
    for (group_indx = 0; group_indx <= 1; ++group_indx) {
        /* map all meters to use this profile */
        for (meter = 0; meter < SOC_DPP_CONFIG(unit)->meter.nof_meter_a; ++meter) {

            policer_id = _DPP_POLICER_ID_FROM_METER_GROUP(unit, meter, group_indx);

            rv = _bcm_petra_meter_get_with_opt(unit, policer_id, &pol_cfg, FALSE /* no_exist_print */);
            if (rv != BCM_E_NONE) {
                continue;
            }
            rv = (*traverse_callback)(unit, policer_id, &pol_cfg, cookie);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }

    for (policer_indx = _DPP_POLICER_MIN_POLICER_ID; policer_indx < _DPP_POLICER_NOF_POLICERS; ++policer_indx) {

        policer_id = _DPP_ETH_POLICER_ID_SET(unit, policer_indx);

        rv = bcm_petra_eth_policer_get(unit, policer_id, &pol_cfg);
        if (rv != BCM_E_NONE) {
            continue;
        }
        rv = (*traverse_callback)(unit, policer_id, &pol_cfg, cookie);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}



STATIC int bcm_petra_policer_destroy_cb(
   int unit,
   bcm_policer_t policer_id,
   bcm_policer_config_t *info,
   void *user_data) {

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(bcm_petra_policer_destroy(unit, policer_id));
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_policer_destroy_all_by_callback(int unit) 
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(bcm_petra_policer_traverse(unit, bcm_petra_policer_destroy_cb, 0));
exit:
    BCMDNX_FUNC_RETURN;
}


int
bcm_petra_policer_destroy_all(int unit) 
{
    int policer_id;
    int group_indx;
    int meter;
    int rv = BCM_E_NONE;
    int policer_indx;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_DPP_SWITCH_API_START(unit);

    /* over meters */
    for (group_indx = 0; group_indx <= 1; ++group_indx) {
        /* map all meters to use this profile */
        for (meter = 0; meter < SOC_DPP_CONFIG(unit)->meter.nof_meter_a; ++meter) {

            rv = bcm_dpp_am_meter_is_alloced(
               unit,
               group_indx,
               1,
               meter);
            if (rv != BCM_E_EXISTS) { /* meter is not exist */
                continue;
            }

            policer_id = _DPP_POLICER_ID_FROM_METER_GROUP(unit, meter, group_indx);

            rv = bcm_petra_meter_destroy(unit, policer_id);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }

    /* destroy all policers aloocated by the user
       policer_id=0 is default so it is not destroyed */
    for (policer_indx = _DPP_POLICER_MIN_POLICER_ID; policer_indx < _DPP_POLICER_NOF_POLICERS; ++policer_indx) {

        rv = bcm_dpp_am_policer_is_alloc(unit, 0, policer_indx);
        if (rv != BCM_E_EXISTS) {
            continue;
        }

        policer_id = _DPP_ETH_POLICER_ID_SET(unit, policer_indx);

        rv = bcm_petra_eth_policer_destroy(unit, policer_id);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    BCM_EXIT;
exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}


int bcm_petra_policer_port_set(
   int unit,
   bcm_port_t port,
   bcm_policer_t policer_id) 
{
    int policer_indx;
    int num_to_alloc = 0;
    bcm_policer_group_mode_t mode;
    int  type_offset[SOC_PPD_NOF_MTR_ETH_TYPES] = { 0 };
    int port_i;
    int plc_indx;
    int soc_sand_rv = 0;
    SOC_PPD_MTR_ETH_TYPE pkt_type;
    int rv = BCM_E_NONE;
    _bcm_dpp_gport_info_t gport_info;

    BCMDNX_INIT_FUNC_DEFS;
    _BCM_DPP_SWITCH_API_START(unit);

    DPP_POLICER_INIT_CHECK(unit);

    policer_indx = _DPP_ETH_POLICER_ID_GET(unit, policer_id);

    /* verify parameters */

    /* policer id means no policing */
    if (policer_id != 0) {
        /* can be only ethernet policer */
        if (!_DPP_IS_ETH_POLICER_ID(unit, policer_id)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (DPP_POLICER_MSG("this is not ethernet policer %d \n"), policer_id));
        }

        DPP_POLICER_UNIT_LOCK(unit);

        /* is this policer allocated */
        rv = bcm_dpp_am_policer_is_alloc(unit, 0, policer_indx);
        if (rv != BCM_E_EXISTS) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (DPP_POLICER_MSG("policer is not allocated %d \n"), policer_id));
        }
        /* reset rv */
        rv = BCM_E_NONE;


        /* policer to mode */
        BCMDNX_IF_ERR_EXIT(POLICER_ACCESS.policer_group.mode.get(unit, policer_indx, &mode));

        rv = _bcm_dpp_policer_group_num_get(unit, mode, &num_to_alloc);
        BCMDNX_IF_ERR_EXIT(rv);

        /* check that all policers in group is allocated */
        for (plc_indx = policer_indx + 1; plc_indx < policer_indx + num_to_alloc; ++plc_indx) {
            rv = bcm_dpp_am_policer_is_alloc(unit, 0, plc_indx);
            if (rv != BCM_E_EXISTS) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (DPP_POLICER_MSG("policer is not allocated %d \n"), _DPP_ETH_POLICER_ID_SET(unit, plc_indx)));
            }
            /* reset rv */
            rv = BCM_E_NONE;
        }

        /* map traffic type to policer offset */
        switch (mode) {
        case bcmPolicerGroupModeSingle: /* all types to base policer */
            break;
        case bcmPolicerGroupModeTrafficType:
            type_offset[SOC_PPD_MTR_ETH_TYPE_UNKNOW_UC] = 0;
            type_offset[SOC_PPD_MTR_ETH_TYPE_KNOW_UC] = 0;
            type_offset[SOC_PPD_MTR_ETH_TYPE_UNKNOW_MC] = 1;
            type_offset[SOC_PPD_MTR_ETH_TYPE_KNOW_MC] = 1;
            type_offset[SOC_PPD_MTR_ETH_TYPE_BC] = 2;
            break;
        case bcmPolicerGroupModeDlfAll:
            type_offset[SOC_PPD_MTR_ETH_TYPE_UNKNOW_UC] = 0;
            type_offset[SOC_PPD_MTR_ETH_TYPE_UNKNOW_MC] = 0;
            type_offset[SOC_PPD_MTR_ETH_TYPE_KNOW_UC] = 1;
            type_offset[SOC_PPD_MTR_ETH_TYPE_KNOW_MC] = 1;
            type_offset[SOC_PPD_MTR_ETH_TYPE_BC] = 1;
            break;
        case bcmPolicerGroupModeTyped:
            type_offset[SOC_PPD_MTR_ETH_TYPE_UNKNOW_UC] = 0;
            type_offset[SOC_PPD_MTR_ETH_TYPE_KNOW_UC] = 1;
            type_offset[SOC_PPD_MTR_ETH_TYPE_UNKNOW_MC] = 2;
            type_offset[SOC_PPD_MTR_ETH_TYPE_KNOW_MC] = 2;
            type_offset[SOC_PPD_MTR_ETH_TYPE_BC] = 3;
            break;
        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (DPP_POLICER_MSG("policer is not allocated %d \n"), _DPP_ETH_POLICER_ID_SET(unit, plc_indx)));
        }
    }

    rv = _bcm_dpp_gport_to_phy_port(unit, port, 0, &gport_info); 
    BCMDNX_IF_ERR_EXIT(rv);

    BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_i) {
        for (pkt_type = 0; pkt_type < SOC_PPD_NOF_MTR_ETH_TYPES; ++pkt_type) {

            soc_sand_rv = soc_ppd_mtr_eth_policer_glbl_profile_map_set(
               unit,
               port_i,
               pkt_type,
               policer_indx + type_offset[pkt_type]
               );
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
    }

exit:
    if (unit >= 0 && unit < BCM_MAX_NUM_UNITS) {
        DPP_POLICER_UNIT_UNLOCK(unit);
    }
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_policer_port_get(
   int unit,
   bcm_port_t port,
   bcm_policer_t *policer_id) 
{
    int policer_indx;
    int port_i;
    int soc_sand_rv = 0;
    int min_policer = _DPP_POLICER_NOF_POLICERS;
    uint32 policer_profile;
    SOC_PPD_MTR_ETH_TYPE pkt_type;
    int rv = BCM_E_NONE;
    _bcm_dpp_gport_info_t gport_info;

    BCMDNX_INIT_FUNC_DEFS;

    DPP_POLICER_INIT_CHECK(unit);

    rv = _bcm_dpp_gport_to_phy_port(unit, port, 0, &gport_info); 
    BCMDNX_IF_ERR_EXIT(rv);

    DPP_POLICER_UNIT_LOCK(unit);

    /* get mapping from port to policer, get minumum policer in the group */
    BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_i) {

        for (pkt_type = 0; pkt_type < SOC_PPD_NOF_MTR_ETH_TYPES; ++pkt_type) {

            soc_sand_rv = soc_ppd_mtr_eth_policer_glbl_profile_map_get(
               unit,
               port_i,
               pkt_type,
               &policer_profile
               );
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            if (policer_profile < min_policer) {
                min_policer = policer_profile;
            }
        }
        break; /* get info from first port */
    }

    /* this is the policer of the port group */
    policer_indx = min_policer;

    /* is this policer allocated, 0 is default */
    if (policer_indx != 0) {
        rv = bcm_dpp_am_policer_is_alloc(unit, 0, policer_indx);
        if (rv != BCM_E_EXISTS) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (DPP_POLICER_MSG("policer is not allocated %d \n"), _DPP_ETH_POLICER_ID_SET(unit, policer_indx)));
        }
        /* reset rv */
        rv = BCM_E_NONE;
        *policer_id = _DPP_ETH_POLICER_ID_SET(unit, policer_indx);
    } else {
        *policer_id = 0;
    }


exit:
    if (unit >= 0 && unit < BCM_MAX_NUM_UNITS) {
        DPP_POLICER_UNIT_UNLOCK(unit);
    }
    BCMDNX_FUNC_RETURN;
}

