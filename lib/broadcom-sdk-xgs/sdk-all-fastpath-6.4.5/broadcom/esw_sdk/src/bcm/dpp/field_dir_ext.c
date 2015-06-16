/*
 * $Id: field_dir_ext.c,v 1.36 Broadcom SDK $
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
 * Module: Field Processor APIs
 *
 * Purpose:
 *     'Field Processor' (FP) API for Dune Packet Processor devices
 *     Direct extraction suppot
 */

/*
 *  The functions in this module deal with direct extraction feature on PPD
 *  devices. They are invoked by the main field module when certain things
 *  occur, such as calls that explicitly involve direct extraction work.
 */

/*
 *  Everything in this file assumes it is safely called behind a lock, except
 *  the last section, functions exposed at the BCM API layer.
 */

/*
 *  Exported functions have their descriptions in the field_int.h file.
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_FP

#include <shared/bsl.h>

#include "bcm_int/common/debug.h"

#include <bcm/types.h>
#include <bcm/debug.h>
#include <bcm/error.h>
#include <bcm/field.h>
#include <bcm_int/dpp/utils.h>
#include <bcm_int/dpp/field_int.h>
#include <bcm_int/dpp/error.h>
#include <bcm_int/dpp/switch.h>
#include <bcm_int/dpp/state.h>
#ifdef BCM_WARM_BOOT_SUPPORT
#include <soc/dcmn/dcmn_wb.h>
#include <bcm_int/dpp/wb_db_field.h>
#endif /* def BCM_WARM_BOOT_SUPPORT */

/*
 *  For now, if _BCM_DPP_FIELD_WARM_BOOT_SUPPORT is FALSE, disable warm boot
 *  support here.
 */
#ifdef BCM_WARM_BOOT_SUPPORT
#if !_BCM_DPP_FIELD_WARM_BOOT_SUPPORT
#undef BCM_WARM_BOOT_SUPPORT
#endif  /* BCM_WARM_BOOT_SUPPORT */
#endif /* !_BCM_DPP_FIELD_WARM_BOOT_SUPPORT */



/******************************************************************************
 *
 *  Local functions and data
 */

/*
 *  Function
 *    _bcm_dpp_field_dir_ext_action_add
 *  Purpose
 *    Add a direct extraction action to a direct extraction entry
 *  Arguments
 *    (in) unitData = unit information
 *    (in) entry = direct extraction entry ID
 *    (in) action = pointer to direct extraction action descriptionm
 *  Returns
 *    int (implied cast from bcm_error_t)
 *      BCM_E_NONE if successful
 *      BCM_E_* appropriately if not
 *  Notes
 */
STATIC int
_bcm_dpp_field_dir_ext_action_add(_bcm_dpp_field_info_t *unitData,
                                  _bcm_dpp_field_ent_idx_t entry,
                                  const bcm_field_extraction_action_t *action,
                                  int count,
                                  const bcm_field_extraction_field_t *extraction)
{
    _bcm_dpp_field_entry_dir_ext_t *entryData;
    _bcm_dpp_field_group_t *groupData;
    _bcm_dpp_field_stage_t *stageData;
    bcm_field_qualify_t qual;
    SOC_PPD_FP_DIR_EXTR_ACTION_VAL tempAct;
    unsigned int index;
    unsigned int offset;
    unsigned int bits;
    unsigned int actBits;
    unsigned int totalBits;
    uint32 mask;
    SOC_PPD_FP_ACTION_TYPE *ppdAct = NULL;
    SOC_PPD_FP_QUAL_TYPE *ppdQual = NULL;
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;

    entryData = &(unitData->entryDe[entry]);
    if (0 == (entryData->entryCmn.entryFlags & _BCM_DPP_FIELD_ENTRY_IN_USE)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d is not in use"),
                          unit,
                          entry + _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt)));
    }
    groupData = &(unitData->groupD[entryData->entryCmn.entryGroup]);
    stageData = &(unitData->stageD[groupData->stage]);
    if ((0 > action->action) || (bcmFieldActionCount <= action->action)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG_NO_UNIT("action type %d is not valid"),
                          action->action));
    }
    if (0 == count) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG_NO_UNIT("action must contain at least one"
                                           " extraction")));
    }
    if (!extraction) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG_NO_UNIT("obligatory IN pointer argument"
                                           " extractions must not be NULL")));
    }
    BCMDNX_IF_ERR_EXIT_MSG(_bcm_dpp_ppd_act_from_bcm_act(unitData,
                                                      groupData->stage,
                                                      action->action,
                                                      &ppdAct),
                        (_BSL_BCM_MSG_NO_UNIT("unit %d unable to find description"
                                          " for action %d (%s)"),
                         unit,
                         action->action,
                         _bcm_dpp_field_action_name[action->action]));
    if ((0 > ppdAct[0]) || 
        (!_BCM_DPP_FIELD_PPD_ACTION_VALID(ppdAct[0]))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d does not support action"
                                           " %d (%s) for stage %u types %08X"),
                          unit,
                          action->action,
                          _bcm_dpp_field_action_name[action->action],
                          groupData->stage,
                          groupData->groupTypes));
    }
    for (index = 1; index < unitData->devInfo->actChain; index++) {
        if (_BCM_DPP_FIELD_PPD_ACTION_VALID(ppdAct[index])) {
#if _BCM_DPP_ALLOW_DIR_EXT_COMPOUND_ACTIONS
            LOG_WARN(BSL_LS_BCM_FP,
                     (BSL_META_U(unit,
                                 "unit %d only uses the first action from a"
                                 " compound action in direct extraction\n"),
                      unit));
#else /* _BCM_DPP_ALLOW_DIR_EXT_COMPOUND_ACTIONS */
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d does not support"
                                               " compound actions for direct"
                                               " extraction features"),
                              unit));
#endif /* _BCM_DPP_ALLOW_DIR_EXT_COMPOUND_ACTIONS */
        }
    }
    BCMDNX_IF_ERR_EXIT_MSG(_bcm_dpp_ppd_action_bits(unitData,
                                                 groupData->stage,
                                                 ppdAct[0],
                                                 &actBits),
                        (_BSL_BCM_MSG_NO_UNIT("unit %d unable to get number of"
                                          " bits for action %d (%s) -> %d"),
                         unit,
                         action->action,
                         _bcm_dpp_field_action_name[action->action],
                         ppdAct[0]));
    /* Special case: over 20b, possible Action Value | 1b filter (using FES) */
    actBits += (actBits > 19)? 1: 0;

    if (groupData->groupFlags & _BCM_DPP_FIELD_GROUP_LARGE) {
        actBits++;
    }

    /* convert the action description to the PPD view, checking it as we go */
    sal_memset(&tempAct, 0x00, sizeof(tempAct));
    if (1 > count) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG_NO_UNIT("invalid number of extractions %d;"
                                           " must be >= 1"),
                          count));
    }
    if (SOC_PPD_FP_DIR_EXTR_MAX_NOF_FIELDS <= count) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d supports maximum %d"
                                           " extractions per action"),
                          unit,
                          SOC_PPD_FP_DIR_EXTR_MAX_NOF_FIELDS));
    }

    tempAct.type = ppdAct[0];
    for (index = 0, totalBits = 0;
         index < count;
         index++) {
        totalBits += extraction[index].bits;
        if ((1 > extraction[index].bits) ||
            (31 < extraction[index].bits)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d extraction bit lengths"
                                               " must be 1..31"),
                              unit));
        }
        tempAct.fld_ext[index].nof_bits = extraction[index].bits;
        mask = (1 << extraction[index].bits) - 1;
        if (extraction[index].flags &
            BCM_FIELD_EXTRACTION_FLAG_CONSTANT) {
            /* ensure nonzero constant */
           tempAct.fld_ext[index].cst_val = (extraction[index].value & mask) |
                                            (1 << extraction[index].bits);
           tempAct.fld_ext[index].type = SOC_PPD_NOF_FP_QUAL_TYPES;
           tempAct.fld_ext[index].fld_lsb = 0;
        } else { /* if (this extraction uses a constant value) */
            tempAct.fld_ext[index].cst_val = 0;
            qual = extraction[index].qualifier;
            if (extraction[index].flags &
                BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD) {
                /* data field, translate the index to PPD qualifier */
                if ((0 > qual) || (unitData->dqLimit <= qual)) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("data qualifier %d is"
                                                       " not valid; range is"
                                                       " 0..%d"),
                                      extraction[index].qualifier,
                                      unitData->dqLimit));
                }
                if (!SHR_BITGET(unitData->unitQset.udf_map,
                                extraction[index].qualifier)) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL,
                                     (_BSL_BCM_MSG_NO_UNIT("unit %d does not"
                                                       " support data qualifier"
                                                       " %d"),
                                      unit,
                                      extraction[index].qualifier));
                }
                if (!SHR_BITGET(groupData->qset.udf_map,
                                extraction[index].qualifier)) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("unit %d group %u qset"
                                                       " does not include data"
                                                       " qualifier %d"),
                                      unit,
                                      entryData->entryCmn.entryGroup,
                                      extraction[index].qualifier));
                }
                BCMDNX_IF_ERR_EXIT_MSG(_bcm_petra_field_map_bcm_udf_to_ppd(unitData,
                                                                        extraction[index].qualifier,
                                                                        &(tempAct.fld_ext[index].type)),
                                    (_BSL_BCM_MSG_NO_UNIT("unit %d unable to map"
                                                      " data qualifier %d to"
                                                      " PPD qualifier"),
                                     unit,
                                     extraction[index].qualifier));
            } else { /* if (this extraction uses data field qualifier) */
                /* not data field, use normal qualifier lookup */
                if ((0 > qual) || (bcmFieldQualifyCount <= qual)) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("qualifier %d is not"
                                                       " valid"),
                                      qual));
                }
                if (!BCM_FIELD_QSET_TEST(unitData->unitQset, qual)) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL,
                                     (_BSL_BCM_MSG_NO_UNIT("unit %d does not"
                                                       " support qualifier"
                                                       " %d (%s)"),
                                      unit,
                                      qual,
                                      _bcm_dpp_field_qual_name[qual]));
                }
                if (!BCM_FIELD_QSET_TEST(groupData->qset, qual)) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("unit %d group %u qset"
                                                       " does not include %d"
                                                       " (%s)"),
                                      unit,
                                      entryData->entryCmn.entryGroup,
                                      qual,
                                      _bcm_dpp_field_qual_name[qual]));
                }
                BCMDNX_IF_ERR_EXIT_MSG(_bcm_dpp_ppd_qual_from_bcm_qual(unitData,
                                                                    groupData->stage,
                                                                    groupData->groupTypes,
                                                                    qual,
                                                                    &ppdQual),
                                    (_BSL_BCM_MSG_NO_UNIT("unit %d unable to map"
                                                      " qualifier %d (%s) to"
                                                      " PPD qualifier"),
                                     unit,
                                     qual,
                                     _bcm_dpp_field_qual_name[qual]));
                if ((0 > ppdQual[0]) ||
                    (!_BCM_DPP_FIELD_PPD_QUAL_VALID(ppdQual[0]))) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("unit %d does not"
                                                       " support qualifier %d"
                                                       " (%s) for stage %u"
                                                       " types %08X"),
                                      unit,
                                      qual,
                                      _bcm_dpp_field_qual_name[qual],
                                      groupData->stage,
                                      groupData->groupTypes));
                }
                for (offset = 1;
                     offset < unitData->devInfo->qualChain;
                     offset++) {
                    if (_BCM_DPP_FIELD_PPD_QUAL_VALID(ppdQual[offset])) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                         (_BSL_BCM_MSG_NO_UNIT("unit %d does not"
                                                           " support compound"
                                                           " qualifiers for"
                                                           " direct extraction"),
                                          unit));
                    }
                }
                tempAct.fld_ext[index].type = ppdQual[0];
            } /* if (this extraction uses data field qualifier) */
            BCMDNX_IF_ERR_EXIT_MSG(_bcm_dpp_ppd_qual_bits(unitData,
                                                       groupData->stage,
                                                       tempAct.fld_ext[index].type,
                                                       NULL,
                                                       &bits,
                                                       NULL),
                                (_BSL_BCM_MSG_NO_UNIT("unit %d unable to get"
                                                  " number of bits for"
                                                  " PPD qualifier %d (%s)"),
                                 unit,
                                 tempAct.fld_ext[index].type,
                                 SOC_PPD_FP_QUAL_TYPE_to_string(tempAct.fld_ext[index].type)));
            if (0 > extraction[index].lsb) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                 (_BSL_BCM_MSG_NO_UNIT("LSb must be"
                                                   " nonnegative")));
            }
            if (bits < (extraction[index].lsb + extraction[index].bits)) {
                if (extraction[index].flags &
                    BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("LSb %d plus bits %d"
                                                       " must be less than or"
                                                       " equal to data qual"
                                                       " %d length %d"),
                                      extraction[index].lsb,
                                      extraction[index].bits,
                                      extraction[index].qualifier,
                                      bits));
                } else {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("LSb %d plus bits %d"
                                                       " must be less than or"
                                                       " equal to qualifier %d"
                                                       " (%s) length %d"),
                                      extraction[index].lsb,
                                      extraction[index].bits,
                                      extraction[index].qualifier,
                                      _bcm_dpp_field_qual_name[extraction[index].qualifier],
                                      bits));
                }
            }
            tempAct.fld_ext[index].fld_lsb = extraction[index].lsb;
        } /* if (this extraction uses a constant value) */
    } /* for (all extractions in this action) */
    if (totalBits > actBits) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG_NO_UNIT("action requires %d bits but total"
                                           " extraction came to %d bits"),
                          actBits,
                          totalBits));
    }
    if (totalBits < actBits) {
        if (index < SOC_PPC_FP_DIR_EXTR_MAX_NOF_FIELDS) {
            /* pad the extraction so it is full width */
            tempAct.fld_ext[index].cst_val = 1 << (actBits - totalBits);
            tempAct.fld_ext[index].type = SOC_PPD_NOF_FP_QUAL_TYPES;
            tempAct.fld_ext[index].fld_lsb = 0;
            tempAct.fld_ext[index].nof_bits = actBits - totalBits;
            index++;
            totalBits = actBits;
        } else { /* if (index < BCM_FIELD_ACTION_EXTRACTION_MAX) */
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                             (_BSL_BCM_MSG_NO_UNIT("action requires %d bits but"
                                               " total extraction came to %d"
                                               " bits and no extraciton left"
                                               " to pad appropriately"),
                              actBits,
                              totalBits));
        } /* if (index < BCM_FIELD_ACTION_EXTRACTION_MAX) */
    } /* if (totalBits < actBits) */
    tempAct.nof_fields = index;
    tempAct.base_val = action->bias & ((1 << actBits) - 1);
    for (/* carries state from previous loop */;
         index < SOC_PPC_FP_DIR_EXTR_MAX_NOF_FIELDS;
         index++) {
        tempAct.fld_ext[index].cst_val = 0;
        tempAct.fld_ext[index].nof_bits = 0;
        tempAct.fld_ext[index].fld_lsb = 0;
        tempAct.fld_ext[index].type = SOC_PPD_NOF_FP_QUAL_TYPES;
    }
    /* got through all that, now look for a place to add the action */
    for (index = 0, offset = stageData->devInfo->entryMaxActs;
         index < stageData->devInfo->entryMaxActs;
         index++) {
        if ((bcmFieldActionCount <= entryData->deAct[index].bcmType) &&
            (offset >= stageData->devInfo->entryMaxActs)) {
            /* first free one noticed so far */
            offset = index;
        }
        if (action->action == entryData->deAct[index].bcmType) {
            /* already have this action here */
            break;
        }
    }
    if (index < stageData->devInfo->entryMaxActs) {
#if _BCM_DPP_ALLOW_IMPLIED_ACTION_REPLACE
        /* overwrite the existing same action */
        offset = index;
#else  /* _BCM_DPP_ALLOW_IMPLIED_ACTION_REPLACE */
        BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d already has"
                                           " action %d (%s); remove it first"),
                          unit,
                          entry + _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt),
                          action->action,
                          _bcm_dpp_field_action_name[action->action]));
#endif /* _BCM_DPP_ALLOW_IMPLIED_ACTION_REPLACE */
    }
    /* set the action with the BCM action type and the constructed HW descr. */
    entryData->deAct[offset].bcmType = action->action;
    sal_memcpy(&(entryData->deAct[offset].hwParam),
               &tempAct,
               sizeof(entryData->deAct[offset].hwParam));
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *    _bcm_dpp_field_dir_ext_action_get
 *  Purpose
 *    Get a direct extraction action from a direct extraction entry
 *  Arguments
 *    (in) unitData = unit information
 *    (in) entry = direct extraction entry ID
 *    (in) action = pointer to direct extraction action descriptionm
 *  Returns
 *    int (implied cast from bcm_error_t)
 *      BCM_E_NONE if successful
 *      BCM_E_* appropriately if not
 *  Notes
 */
STATIC int
_bcm_dpp_field_dir_ext_action_get(_bcm_dpp_field_info_t *unitData,
                                  _bcm_dpp_field_ent_idx_t entry,
                                  bcm_field_extraction_action_t *action,
                                  int maxCount,
                                  bcm_field_extraction_field_t *extraction,
                                  int *actualCount)
{
    _bcm_dpp_field_entry_dir_ext_t *entryData;
    _bcm_dpp_field_stage_t *stageData;
    unsigned int index;
    unsigned int offset;
    SOC_PPD_FP_QUAL_TYPE qual;
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;

    entryData = &(unitData->entryDe[entry]);
    if ((0 > action->action) || (bcmFieldActionCount <= action->action)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG_NO_UNIT("action %d is not valid"),
                          action->action));
    }
    if (!BCM_FIELD_ASET_TEST(unitData->unitAset, action->action)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d does not support action"
                                           " %d (%s)"),
                          unit,
                          action->action,
                          _bcm_dpp_field_action_name[action->action]));
    }
    if (0 == (entryData->entryCmn.entryFlags & _BCM_DPP_FIELD_ENTRY_IN_USE)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d is not in use"),
                          unit,
                          entry + _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt)));
    }
    if (!BCM_FIELD_ASET_TEST(unitData->groupD[entryData->entryCmn.entryGroup].aset,
                             action->action)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d (group %d) aset"
                                           " does not include %d (%s)"),
                          unit,
                          entry + _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt),
                          entryData->entryCmn.entryGroup,
                          action->action,
                          _bcm_dpp_field_action_name[action->action]));
    }
    if ((0 < maxCount) && (!extraction)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG_NO_UNIT("if max count positive, extractions"
                                           " pointer must not be NULL")));
    }
    if (!actualCount) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG_NO_UNIT("obligatory OUT pointer argument"
                                           " actual count must not be NULL")));
    }
    stageData = &(unitData->stageD[unitData->groupD[entryData->entryCmn.entryGroup].stage]);
    for (index = 0; index < stageData->devInfo->entryMaxActs; index++) {
        if (action->action == entryData->deAct[index].bcmType) {
            break;
        }
    }
    if (index >= stageData->devInfo->entryMaxActs) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d does not have"
                                           " action %d (%s)"),
                          unit,
                          entry + _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt),
                          action->action,
                          _bcm_dpp_field_action_name[action->action]));
    }
    bcm_field_extraction_action_t_init(action);
    action->action = entryData->deAct[index].bcmType;
    action->bias = entryData->deAct[index].hwParam.base_val;
    if (0 < maxCount) {
        for (offset = 0;
             (offset < entryData->deAct[index].hwParam.nof_fields) &&
             (offset < maxCount);
             offset++) {
            extraction[offset].bits = entryData->deAct[index].hwParam.fld_ext[offset].nof_bits;
            extraction[offset].lsb = entryData->deAct[index].hwParam.fld_ext[offset].fld_lsb;
            extraction[offset].value = entryData->deAct[index].hwParam.fld_ext[offset].cst_val & ((1 << entryData->deAct[index].hwParam.fld_ext[offset].nof_bits) - 1);
            extraction[offset].flags = 0;
            if (!_BCM_DPP_FIELD_PPD_QUAL_VALID(entryData->deAct[index].hwParam.fld_ext[offset].type)) {
                extraction[offset].qualifier = bcmFieldQualifyCount;
                extraction[offset].flags |= BCM_FIELD_EXTRACTION_FLAG_CONSTANT;
            } else { /* if (constant value used at PPD layer) */
                qual = entryData->deAct[index].hwParam.fld_ext[offset].type;
                if (bcmFieldQualifyCount > unitData->ppdQual[qual]) {
                    extraction[offset].qualifier = unitData->ppdQual[qual];
                } else {
                    /* no PPD -> BCM translation; try data_qual */
                    BCMDNX_IF_ERR_EXIT_MSG(_bcm_petra_field_map_ppd_udf_to_bcm(unitData,
                                                                            qual,
                                                                            &(extraction[offset].qualifier)),
                                        (_BSL_BCM_MSG_NO_UNIT("unit %d unable to"
                                                          " get BCM qual"
                                                          " mapping for"
                                                          " hardware qual"
                                                          " %d (%s)"),
                                         unit,
                                         qual,
                                         SOC_PPD_FP_QUAL_TYPE_to_string(qual)));
                    extraction[offset].flags |= BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
                } /* if (PPD qual maps to BCM qual) */
            } /* if (constant value used at PPD layer) */
        }
        /* in this case, actualCount reflects actual number populated */
        *actualCount = offset;
    } else {
        /* in this case, actualCount reflects actual number needed */
        *actualCount = entryData->deAct[index].hwParam.nof_fields;
    }
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *     _bcm_dpp_field_dir_ext_entry_hardware_install
 *  Purpose
 *     Insert a Soc_petra PPD entry based upon the BCM field entry information
 *  Parameters
 *     (in) unitData = unit information
 *     (in) entry = entry ID
 *  Returns
 *     int (implied cast from bcm_error_t)
 *        BCM_E_NONE if successful
 *        BCM_E_* appropriately if not
 *  Notes
 *     Assumes entry validity checks have already been performed.
 *
 *     Will only work if the group is already in hardware.
 *
 *     Will destroy and recreate the hardware entry if it already exists.
 */
STATIC int
_bcm_dpp_field_dir_ext_entry_hardware_install(_bcm_dpp_field_info_t *unitData,
                                              _bcm_dpp_field_ent_idx_t entry)
{
    SOC_PPD_FP_DIR_EXTR_ENTRY_INFO *entryInfo = NULL;
    _bcm_dpp_field_group_t *groupData;
    _bcm_dpp_field_entry_dir_ext_t *entryData;
    uint32 ppdr;
    uint32 oldFlags;
    uint32 oldHwHandle;
    int newHwHandle;
    SOC_SAND_SUCCESS_FAILURE success;
    unsigned int index;
    unsigned int offset;
    int result;
    int auxRes;

    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;
    
    entryData = &(unitData->entryDe[entry]);
    groupData = &(unitData->groupD[entryData->entryCmn.entryGroup]);

    /* keep old flags and handle around */
    oldHwHandle = entryData->entryCmn.hwHandle;
    oldFlags = entryData->entryCmn.entryFlags;

    /* pick a new hardware entry handle */
    BCMDNX_IF_ERR_EXIT_MSG(shr_mres_alloc(unitData->hwResIds,
                                       unitData->stageD[groupData->stage].entryDeRes,
                                       0 /* flags */,
                                       1 /* count */,
                                       &newHwHandle),
                        (_BSL_BCM_MSG_NO_UNIT("unit %d unable to allocate hardware"
                                          " handle for entry %d"),
                         unit,
                         entry + _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt)));

    /* build entry data from sw state */
    BCMDNX_ALLOC(entryInfo, sizeof(SOC_PPD_FP_DIR_EXTR_ENTRY_INFO), "_bcm_dpp_field_dir_ext_entry_hardware_install.entryInfo");
    if (entryInfo == NULL) {        
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG_NO_UNIT("failed to allocate memory")));
    }
    SOC_PPD_FP_DIR_EXTR_ENTRY_INFO_clear(entryInfo);
    entryInfo->priority = entryData->entryCmn.hwPriority;
    for (index = 0; index < SOC_PPD_FP_NOF_QUALS_PER_DB_MAX; index++) {
        if (_BCM_DPP_FIELD_PPD_QUAL_VALID(entryData->entryCmn.entryQual[index].hwType)) {
            LOG_DEBUG(BSL_LS_BCM_FP,
                      (BSL_META_U(unit,
                                  "unit %d entry %d qual %d(%s)->%d"
                                  " %08X%08X/%08X%08X\n"),
                       unit,
                       entry + _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt),
                       entryData->entryCmn.entryQual[index].qualType,
                       (bcmFieldQualifyCount>entryData->entryCmn.entryQual[index].qualType)?_bcm_dpp_field_qual_name[entryData->entryCmn.entryQual[index].qualType]:"-",
                       entryData->entryCmn.entryQual[index].hwType,
                       COMPILER_64_HI(entryData->entryCmn.entryQual[index].qualData),
                       COMPILER_64_LO(entryData->entryCmn.entryQual[index].qualData),
                       COMPILER_64_HI(entryData->entryCmn.entryQual[index].qualMask),
                       COMPILER_64_LO(entryData->entryCmn.entryQual[index].qualMask)));
            entryInfo->qual_vals[index].type = entryData->entryCmn.entryQual[index].hwType;
            /* SOC_SAND_U64 is a coerced 32b grain little-endian 64b number */
            entryInfo->qual_vals[index].val.arr[0] = COMPILER_64_LO(entryData->entryCmn.entryQual[index].qualData);
            entryInfo->qual_vals[index].val.arr[1] = COMPILER_64_HI(entryData->entryCmn.entryQual[index].qualData);
            entryInfo->qual_vals[index].is_valid.arr[0] = COMPILER_64_LO(entryData->entryCmn.entryQual[index].qualMask);
            entryInfo->qual_vals[index].is_valid.arr[1] = COMPILER_64_HI(entryData->entryCmn.entryQual[index].qualMask);
        } /* if (entryData->qual[index].hwType < bcmFieldQualifyCount) */
    } /* for (all qualifiers) */
    for (index = 0; index < SOC_PPD_FP_NOF_ACTIONS_PER_DB_MAX; index++) {
        if (_BCM_DPP_FIELD_PPD_ACTION_VALID(entryData->deAct[index].hwParam.type)) {
            LOG_DEBUG(BSL_LS_BCM_FP,
                      (BSL_META_U(unit,
                                  "unit %d entry %d action %d(%s)->%d: bias %08X, count %d\n"),
                       unit,
                       entry + _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt),
                       entryData->deAct[index].bcmType,
                       _bcm_dpp_field_action_name[entryData->deAct[index].bcmType],
                       entryData->deAct[index].hwParam.type,
                       entryData->deAct[index].hwParam.base_val,
                       entryData->deAct[index].hwParam.nof_fields));
            entryInfo->actions[index].base_val = entryData->deAct[index].hwParam.base_val;
            entryInfo->actions[index].nof_fields = entryData->deAct[index].hwParam.nof_fields;
            entryInfo->actions[index].type = entryData->deAct[index].hwParam.type;
            for (offset = 0;
                 offset < entryData->deAct[index].hwParam.nof_fields;
                 offset++) {
                if (entryData->deAct[index].hwParam.fld_ext[offset].cst_val) {
                    LOG_DEBUG(BSL_LS_BCM_FP,
                              (BSL_META_U(unit,
                                          "unit %d entry %d action %d(%s)->%d: constant %08X, low %d bits\n"),
                               unit,
                               entry + _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt),
                               entryData->deAct[index].bcmType,
                               _bcm_dpp_field_action_name[entryData->deAct[index].bcmType],
                               entryData->deAct[index].hwParam.type,
                               entryData->deAct[index].hwParam.fld_ext[offset].cst_val,
                               entryData->deAct[index].hwParam.fld_ext[offset].nof_bits));
                    entryInfo->actions[index].fld_ext[offset].cst_val = entryData->deAct[index].hwParam.fld_ext[offset].cst_val;
                    entryInfo->actions[index].fld_ext[offset].nof_bits = entryData->deAct[index].hwParam.fld_ext[offset].nof_bits;
                } else { /* if (extraction is using a constant) */
                    LOG_DEBUG(BSL_LS_BCM_FP,
                              (BSL_META_U(unit,
                                          "unit %d entry %u action %d(%s)->%d: qual %d(%s)->%d, bits %d..%d\n"),
                               unit,
                               entry + _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt),
                               entryData->deAct[index].bcmType,
                               _bcm_dpp_field_action_name[entryData->deAct[index].bcmType],
                               entryData->deAct[index].hwParam.type,
                               unitData->ppdQual[entryData->deAct[index].hwParam.fld_ext[offset].type],
                               (bcmFieldQualifyCount>unitData->ppdQual[entryData->deAct[index].hwParam.fld_ext[offset].type])?_bcm_dpp_field_qual_name[unitData->ppdQual[entryData->deAct[index].hwParam.fld_ext[offset].type]]:"-",
                              entryData->deAct[index].hwParam.fld_ext[offset].type,
                              entryData->deAct[index].hwParam.fld_ext[offset].fld_lsb + entryData->deAct[index].hwParam.fld_ext[offset].nof_bits - 1,
                               entryData->deAct[index].hwParam.fld_ext[offset].fld_lsb));
                    entryInfo->actions[index].fld_ext[offset].fld_lsb = entryData->deAct[index].hwParam.fld_ext[offset].fld_lsb;
                    entryInfo->actions[index].fld_ext[offset].nof_bits = entryData->deAct[index].hwParam.fld_ext[offset].nof_bits;
                    entryInfo->actions[index].fld_ext[offset].type = entryData->deAct[index].hwParam.fld_ext[offset].type;
                } /* if (extraction is using a constant) */
            } /* for (all fields in this action) */
        } /* if (SOC_PPD_NOF_FP_ACTION_TYPES > entryData->act[index].hwParam.type) */
    } /* for (all actions) */

    /* add the entry to hardware */
    LOG_DEBUG(BSL_LS_BCM_FP,
              (BSL_META_U(unit,
                          "add unit %d entry %d to hardware with handle %d\n"),
               unit,
               entry + _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt),
               newHwHandle));
    ppdr = soc_ppd_fp_direct_extraction_entry_add(unitData->unitHandle,
                                                  groupData->hwHandle,
                                                  newHwHandle,
                                                  entryInfo,
                                                  &success);
    result = handle_sand_result(ppdr);
    if (BCM_E_NONE == result) {
        result = translate_sand_success_failure(success);
    }
    if (BCM_E_NONE != result) {
        LOG_ERROR(BSL_LS_BCM_FP,
                  (BSL_META_U(unit,
                              "unit %d unable to add entry %d as hardware"
                              " entry %d: %d (%s)\n"),
                   unit,
                   entry + _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt),
                   newHwHandle,
                   result,
                   _SHR_ERRMSG(result)));
        auxRes = shr_mres_free(unitData->hwResIds,
                               unitData->stageD[groupData->stage].entryDeRes,
                               1 /* count */,
                               newHwHandle);
        if(auxRes != BCM_E_NONE) {
            LOG_ERROR(BSL_LS_BCM_FP,
                      (BSL_META_U(unit,
                                  "shr_mres_free Failed\n")));
        }
        BCM_ERR_EXIT_NO_MSG(result);
    }
    entryData->entryCmn.hwHandle = newHwHandle;
    /* entry is now in hardware */
    entryData->entryCmn.entryFlags |= _BCM_DPP_FIELD_ENTRY_IN_HW;
    /* entry is no longer changed since insert & no need to update */
    entryData->entryCmn.entryFlags &= (~(_BCM_DPP_FIELD_ENTRY_CHANGED |
                                  _BCM_DPP_FIELD_ENTRY_UPDATE));
#ifdef BCM_WARM_BOOT_SUPPORT
    /* update backing store */
    _bcm_dpp_field_dir_ext_entry_wb_save(unitData, entry, NULL, NULL);
#endif /* def BCM_WARM_BOOT_SUPPORT */

    /* remove from hardware if it is already installed */
    if (oldFlags & _BCM_DPP_FIELD_ENTRY_IN_HW) {
        LOG_DEBUG(BSL_LS_BCM_FP,
                  (BSL_META_U(unit,
                              "remove unit %d entry %d old copy from"
                              " hardware with handle %d\n"),
                   unit,
                   entry + _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt),
                   oldHwHandle));
        ppdr = soc_ppd_fp_direct_extraction_entry_remove(unitData->unitHandle,
                                                         groupData->hwHandle,
                                                         oldHwHandle);
        BCMDNX_IF_ERR_EXIT_MSG(handle_sand_result(ppdr),
                            (_BSL_BCM_MSG_NO_UNIT("unit %d unable to remove"
                                              " old entry %d from hardware"
                                              " as hardware entry %d"),
                             unit,
                             entry + _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt),
                             oldHwHandle));
        BCMDNX_IF_ERR_EXIT_MSG(shr_mres_free(unitData->hwResIds,
                                          unitData->stageD[groupData->stage].entryDeRes,
                                          1 /* count */,
                                          oldHwHandle),
                            (_BSL_BCM_MSG_NO_UNIT("unit %d unable to return"
                                              " hardware handle %d to"
                                              " free list"),
                             unit,
                             oldHwHandle));
    } /* if (oldFlags & _BCM_PETRA_FIELD_ENTRY_IN_HW) */
exit:
    BCM_FREE(entryInfo);
    BCMDNX_FUNC_RETURN;
}


/******************************************************************************
 *
 *  Functions and data shared with other field modules
 */

#ifdef BROADCOM_DEBUG
int
_bcm_dpp_field_dir_ext_entry_dump(_bcm_dpp_field_info_t *unitData,
                                  _bcm_dpp_field_ent_idx_t entry,
                                  const char *prefix)
{
    _bcm_dpp_field_entry_dir_ext_t *entryData = &(unitData->entryDe[entry]);
    bcm_field_aset_t actions;
    unsigned int index;
    unsigned int count;
    unsigned int bits;
    unsigned int offset;
    unsigned int dq;
    uint32 mask;
    int result = BCM_E_NONE;
    int dumpedRanges;
    char format[64];
#if _BCM_DPP_FIELD_DUMP_VERIFY_PPD
    uint32 soc_sandResult;
    uint8 okay;
    SOC_PPD_FP_DIR_EXTR_ENTRY_INFO *entInfoDe = NULL;
    SOC_PPD_FP_QUAL_VAL *hwQuals;
#endif /* _BCM_DPP_FIELD_DUMP_VERIFY_PPD */
 
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;
    
    sal_memset(&(actions), 0x00, sizeof(actions));
#if _BCM_DPP_FIELD_DUMP_VERIFY_PPD
    if ((entryData->entryCmn.entryFlags & (_BCM_DPP_FIELD_ENTRY_IN_HW |
                                           _BCM_DPP_FIELD_ENTRY_CHANGED |
                                           _BCM_DPP_FIELD_ENTRY_NEW)) ==
        (_BCM_DPP_FIELD_ENTRY_IN_HW)) {
        /* entry is in hardware and not changed since */
        BCMDNX_ALLOC(entInfoDe, sizeof(SOC_PPD_FP_DIR_EXTR_ENTRY_INFO), "_bcm_dpp_field_dir_ext_entry_dump.entInfoDe");
        if (entInfoDe == NULL) {        
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG_NO_UNIT("failed to allocate memory")));
        }
        SOC_PPD_FP_DIR_EXTR_ENTRY_INFO_clear(entInfoDe);
        soc_sandResult = soc_ppd_fp_direct_extraction_entry_get(unitData->unitHandle,
                                                                unitData->groupD[entryData->entryCmn.entryGroup].hwHandle,
                                                                entryData->entryCmn.hwHandle,
                                                                &okay,
                                                                entInfoDe);
        result = handle_sand_result(soc_sandResult);
        BCMDNX_IF_ERR_EXIT_MSG(result,
                            (_BSL_BCM_MSG_NO_UNIT("unit %d unable to fetch entry"
                                              " %u hardware information (%u):"
                                              " %d (%s)"),
                             unit,
                             entry,
                             entryData->entryCmn.hwHandle,
                             result,
                             _SHR_ERRMSG(result)));
        if (!okay) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d entry %u not okay"
                                               " trying to fetch entry from"
                                               " hardware"),
                              unit,
                              entry));
        }
        hwQuals = &(entInfoDe->qual_vals[0]);
    } else {
        /* entry is not in hardware or has changed */
        hwQuals = NULL;
    }
#endif /* _BCM_DPP_FIELD_DUMP_VERIFY_PPD */
    LOG_CLI((BSL_META_U(unit,
                        "%sEntry "
             _BCM_DPP_FIELD_ENTRY_IDX_FORMAT
                        "\n"),
             prefix,
             entry + _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt)));
    LOG_CLI((BSL_META_U(unit,
                        "%s  Flags       = %08X\n"), prefix, entryData->entryCmn.entryFlags));
#if _BCM_DPP_FIELD_DUMP_SYM_FLAGS
    LOG_CLI((BSL_META_U(unit,
                        "%s    %s %s %s %s %s %s %s\n"),
             prefix,
             (entryData->entryCmn.entryFlags & _BCM_DPP_FIELD_ENTRY_IN_USE)?"+USED":"-used",
             (entryData->entryCmn.entryFlags & _BCM_DPP_FIELD_ENTRY_IN_HW)?"+IN_HW":"-in_hw",
             (entryData->entryCmn.entryFlags & _BCM_DPP_FIELD_ENTRY_WANT_HW)?"+WANT_HW":"-want_hw",
             (entryData->entryCmn.entryFlags & _BCM_DPP_FIELD_ENTRY_UPDATE)?"+UPD":"-upd",
             (entryData->entryCmn.entryFlags & _BCM_DPP_FIELD_ENTRY_CHANGED)?"+CHG":"-chg",
             (entryData->entryCmn.entryFlags & _BCM_DPP_FIELD_ENTRY_NEW)?"+NEW":"-new",
             (entryData->entryCmn.entryFlags & _BCM_DPP_FIELD_ENTRY_STATS)?"+STA":"-sta"));
#endif /* _BCM_PETRA_FIELD_DUMP_SYM_FLAGS */
    if (entryData->entryCmn.entryFlags & _BCM_DPP_FIELD_ENTRY_IN_USE) {
        if ((_BCM_DPP_FIELD_ENTRY_IN_HW | _BCM_DPP_FIELD_ENTRY_CHANGED) ==
            (entryData->entryCmn.entryFlags & (_BCM_DPP_FIELD_ENTRY_IN_HW |
                                 _BCM_DPP_FIELD_ENTRY_CHANGED))) {
            LOG_CLI((BSL_META_U(unit,
                                "%s    (entry changed since hw write)\n"), prefix));
        }
        LOG_CLI((BSL_META_U(unit,
                            "%s  Group       = "
                 _BCM_DPP_FIELD_GROUP_IDX_FORMAT
                            "\n"),
                 prefix,
                 entryData->entryCmn.entryGroup));
        LOG_CLI((BSL_META_U(unit,
                            "%s  Priority    = %8d\n"),
                 prefix,
                 entryData->entryCmn.entryPriority));
        LOG_CLI((BSL_META_U(unit,
                            "%s  Prev/Next   = "
                 _BCM_DPP_FIELD_ENTRY_IDX_FORMAT
                            " / "
                            _BCM_DPP_FIELD_ENTRY_IDX_FORMAT
                            "\n"),
                 prefix,
                 entryData->entryCmn.entryPrev + _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt),
                 entryData->entryCmn.entryNext + _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt)));
        if (entryData->entryCmn.entryFlags & _BCM_DPP_FIELD_ENTRY_IN_HW) {
            LOG_CLI((BSL_META_U(unit,
                                "%s  HW entry ID = %08X\n"),
                     prefix,
                     entryData->entryCmn.hwHandle));
            LOG_CLI((BSL_META_U(unit,
                                "%s  HW priority = %08X\n"),
                     prefix,
                     entryData->entryCmn.hwPriority));
        }
        LOG_CLI((BSL_META_U(unit,
                            "%s  Qualifiers:\n"), prefix));
        count = 0;
        dumpedRanges = FALSE;
        for (index = 0;
             (BCM_E_NONE == result) &&
             (index < SOC_PPD_FP_NOF_QUALS_PER_DB_MAX);
             index++) {
            result = _bcm_dpp_field_entry_qual_dump(unitData,
                                                    unitData->groupD[entryData->entryCmn.entryGroup].stage,
                                                    &(entryData->entryCmn.entryQual[0]),
#if _BCM_DPP_FIELD_DUMP_VERIFY_PPD
                                                    hwQuals,
#endif /* _BCM_DPP_FIELD_DUMP_VERIFY_PPD */
                                                    index,
                                                    !dumpedRanges,
                                                    prefix);
            if (1 == result) {
                count++;
                result = BCM_E_NONE;
            } else if (2 == result) {
                dumpedRanges = TRUE;
                count++;
                result = BCM_E_NONE;
            }
        } /* for (index = 0; index < SOC_PPD_FP_NOF_QUALS_PER_DB_MAX; index++) */
        if (0 == count) {
            /* nothing in qualifiers */
            LOG_CLI((BSL_META_U(unit,
                                "%s    (none; matches all applicable traffic)\n"),
                     prefix));
        }
        LOG_CLI((BSL_META_U(unit,
                            "%s  Actions:\n"), prefix));
        count = 0;
        for (index = 0; index < SOC_PPD_FP_NOF_ACTIONS_PER_DB_MAX; index++) {
            if (_BCM_DPP_FIELD_PPD_ACTION_VALID(entryData->deAct[index].hwParam.type)) {
                /* only show action if exists */
                if (!BCM_FIELD_ASET_TEST(actions, entryData->deAct[index].bcmType)) {
                    /* have not displayed this action yet */
                    result = _bcm_dpp_ppd_action_bits(unitData,
                                                      unitData->groupD[entryData->entryCmn.entryGroup].stage,
                                                      entryData->deAct[index].hwParam.type,
                                                      &bits);
                    if (BCM_E_NONE != result) {
                        LOG_ERROR(BSL_LS_BCM_FP,
                                  (BSL_META_U(unit,
                                              "unit %d unable to get bit count"
                                              " for action %d: %d (%s)\n"),
                                   unit,
                                   entryData->deAct[index].hwParam.type,
                                   result,
                                   _SHR_ERRMSG(result)));
                        continue;
                    }
                    LOG_CLI((BSL_META_U(unit,
                                        "%s    %s (%d) (%u bits, LSb first)\n"),
                             prefix,
                             _bcm_dpp_field_action_name[entryData->deAct[index].bcmType],
                             entryData->deAct[index].bcmType,
                             bits));
                    for (offset = 0;
                         offset < entryData->deAct[index].hwParam.nof_fields;
                         offset++) {
                        if (entryData->deAct[index].hwParam.fld_ext[offset].cst_val) {
                            sal_snprintf(format,
                                         sizeof(format),
                                         "%%s      Constant, %%u bits,"
                                         " value %%0%dX\n",
                                         (entryData->deAct[index].hwParam.fld_ext[offset].nof_bits + 3) >> 2);
                            mask = (1 << entryData->deAct[index].hwParam.fld_ext[offset].nof_bits) - 1;
                            LOG_CLI((format,
                                     prefix,
                                     entryData->deAct[index].hwParam.fld_ext[offset].nof_bits,
                                     entryData->deAct[index].hwParam.fld_ext[offset].cst_val & mask));
                        } else {
                            if (bcmFieldQualifyCount >
                                unitData->ppdQual[entryData->deAct[index].hwParam.fld_ext[offset].type]) {
                                /* standard qualifier */
                                LOG_CLI((BSL_META_U(unit,
                                                    "%s      %s (%d), bits %u..%u (%u bits)\n"),
                                         prefix,
                                         _bcm_dpp_field_qual_name[unitData->ppdQual[entryData->deAct[index].hwParam.fld_ext[offset].type]],
                                         unitData->ppdQual[entryData->deAct[index].hwParam.fld_ext[offset].type],
                                         entryData->deAct[index].hwParam.fld_ext[offset].nof_bits + entryData->deAct[index].hwParam.fld_ext[offset].fld_lsb - 1,
                                         entryData->deAct[index].hwParam.fld_ext[offset].fld_lsb,
                                         entryData->deAct[index].hwParam.fld_ext[offset].nof_bits));
                            } else {
                                /* programmable qualifier */
                                result = _bcm_petra_field_map_ppd_udf_to_bcm(unitData,
                                                                             entryData->deAct[index].hwParam.fld_ext[offset].type,
                                                                             &dq);
                                LOG_CLI((BSL_META_U(unit,
                                                    "%s      Data qualifier %d, bits %u..%u (%u bits)\n"),
                                         prefix,
                                         dq,
                                         entryData->deAct[index].hwParam.fld_ext[offset].nof_bits + entryData->deAct[index].hwParam.fld_ext[offset].fld_lsb - 1,
                                         entryData->deAct[index].hwParam.fld_ext[offset].fld_lsb,
                                         entryData->deAct[index].hwParam.fld_ext[offset].nof_bits));
                            }
                        }
                    } /* for (all fields in this action) */
                    mask = (1 << bits) - 1;
                    sal_snprintf(format,
                                 sizeof(format),
                                 "%%s      Overall bias = %%0%dX\n",
                                 (bits + 3) >> 2);
                    LOG_CLI((format,
                             prefix,
                             entryData->deAct[index].hwParam.base_val & mask));
                    count++;
                } /* if (not encountered this action yet) */
                BCM_FIELD_ASET_ADD(actions, entryData->deAct[index].bcmType);
            } /* if (action exists) */
        } /* for (index = 0; index < SOC_PPD_FP_NOF_ACTIONS_PER_DB_MAX; index++) */
        if (0 == count) {
            /* nothing in qualifiers */
            LOG_CLI((BSL_META_U(unit,
                                "%s    (none; a match will not affect traffic)\n"),
                     prefix));
        }
        BCM_EXIT;
    } else {/* if (entryData->flags & _BCM_PETRA_FIELD_ENTRY_IN_USE) */
        LOG_CLI((BSL_META_U(unit,
                            "%s  (entry is not in use)\n"), prefix));
        BCM_ERR_EXIT_NO_MSG(BCM_E_NOT_FOUND);
    } /* if (entryData->flags & _BCM_PETRA_FIELD_ENTRY_IN_USE) */
exit:
    BCM_FREE(entInfoDe);
    BCMDNX_FUNC_RETURN;
}
#endif /* def BROADCOM_DEBUG */

int
_bcm_dpp_field_dir_ext_entry_action_clear(_bcm_dpp_field_info_t *unitData,
                                          _bcm_dpp_field_ent_idx_t entry,
                                          bcm_field_action_t type)
{
    _bcm_dpp_field_entry_dir_ext_t *entryData;
    unsigned int index;
    unsigned int offset;
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;

    entryData = &(unitData->entryDe[entry]);
    if (0 == (entryData->entryCmn.entryFlags & _BCM_DPP_FIELD_ENTRY_IN_USE)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d is not in use"),
                          unit,
                          entry + _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt)));
    }
    if (!BCM_FIELD_ASET_TEST(unitData->groupD[entryData->entryCmn.entryGroup].aset, type)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d (group %d ASET"
                                           " does not include %d (%s)"),
                          unit,
                          entry + _BCM_DPP_FIELD_ENT_BIAS(unit,DirExt),
                          entryData->entryCmn.entryGroup,
                          type,
                          _bcm_dpp_field_action_name[type]));
    }
    for (index = 0;
         SOC_PPD_FP_NOF_ACTIONS_PER_DB_MAX > index;
         index++) {
        if (type == entryData->deAct[index].bcmType) {
            sal_memset(&(entryData->deAct[index]),
                       0x00,
                       sizeof(entryData->deAct[index]));
            entryData->deAct[index].bcmType = bcmFieldActionCount;
            entryData->deAct[index].hwParam.type = SOC_PPD_FP_ACTION_TYPE_INVALID;
            for (offset = 0; offset < SOC_PPD_FP_DIR_EXTR_MAX_NOF_FIELDS; offset++) {
                entryData->deAct[index].hwParam.fld_ext[offset].type = SOC_PPD_NOF_FP_QUAL_TYPES;
            }
            entryData->entryCmn.entryFlags |= _BCM_DPP_FIELD_ENTRY_CHANGED;
            unitData->groupD[entryData->entryCmn.entryGroup].groupFlags |= _BCM_DPP_FIELD_GROUP_CHG_ENT;
            break;
        }
    }
    if (SOC_PPD_FP_NOF_ACTIONS_PER_DB_MAX <= index) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d does not have"
                                           " action %d (%s)"),
                          unit,
                          entry + _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt),
                          type,
                          _bcm_dpp_field_action_name[type]));
    }
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_field_dir_ext_entry_clear_acts(_bcm_dpp_field_info_t *unitData,
                                        _bcm_dpp_field_ent_idx_t entry)
{
    _bcm_dpp_field_entry_dir_ext_t *entryData = &(unitData->entryDe[entry]);
    unsigned int index;
    unsigned int offset;
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;

    if (0 == (entryData->entryCmn.entryFlags & _BCM_DPP_FIELD_ENTRY_IN_USE)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d is not in use"),
                          unit,
                          entry + _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt)));
    }
    sal_memset(&(entryData->deAct[0]),
               0x00,
               sizeof(entryData->deAct[0]) * SOC_PPD_FP_NOF_ACTIONS_PER_DB_MAX);
    for (index = 0; index < SOC_PPD_FP_NOF_ACTIONS_PER_DB_MAX; index++) {
        entryData->deAct[index].bcmType = bcmFieldActionCount;
        entryData->deAct[index].hwParam.type = SOC_PPD_FP_ACTION_TYPE_INVALID;
        for (offset = 0; offset < SOC_PPD_FP_DIR_EXTR_MAX_NOF_FIELDS; offset++) {
            entryData->deAct[index].hwParam.fld_ext[offset].type = SOC_PPD_NOF_FP_QUAL_TYPES;
        }
    }
    entryData->entryCmn.entryFlags |= _BCM_DPP_FIELD_ENTRY_CHANGED;
    unitData->groupD[entryData->entryCmn.entryGroup].groupFlags |= _BCM_DPP_FIELD_GROUP_CHG_ENT;
exit:
    BCMDNX_FUNC_RETURN;
}

void
_bcm_dpp_field_dir_ext_entry_clear(_bcm_dpp_field_info_t *unitData,
                                   _bcm_dpp_field_ent_idx_t entry)
{
    _bcm_dpp_field_ent_idx_t next;
    _bcm_dpp_field_ent_idx_t prev;
    _bcm_dpp_field_entry_dir_ext_t *entryData = &(unitData->entryDe[entry]);
    unsigned int index;
    unsigned int offset;

    next = entryData->entryCmn.entryNext;
    prev = entryData->entryCmn.entryPrev;
    sal_memset(entryData, 0x00, sizeof(*entryData));
    entryData->entryCmn.entryNext = next;
    entryData->entryCmn.entryPrev = prev;
    entryData->entryCmn.entryPriority = BCM_FIELD_ENTRY_PRIO_DEFAULT;
    for (index = 0; index < SOC_PPD_FP_NOF_QUALS_PER_DB_MAX; index++) {
        entryData->entryCmn.entryQual[index].qualType = bcmFieldQualifyCount;
        entryData->entryCmn.entryQual[index].hwType = SOC_PPD_NOF_FP_QUAL_TYPES;
    }
    for (index = 0; index < SOC_PPD_FP_NOF_ACTIONS_PER_DB_MAX; index++) {
        entryData->deAct[index].bcmType = bcmFieldActionCount;
        entryData->deAct[index].hwParam.type = SOC_PPD_FP_ACTION_TYPE_INVALID;
        for (offset = 0; offset < SOC_PPD_FP_DIR_EXTR_MAX_NOF_FIELDS; offset++) {
            entryData->deAct[index].hwParam.fld_ext[offset].type = SOC_PPD_NOF_FP_QUAL_TYPES;
        }
    }
}

int
_bcm_dpp_field_dir_ext_entry_alloc(_bcm_dpp_field_info_t *unitData,
                                   _bcm_dpp_field_grp_idx_t group,
                                   int withId,
                                   _bcm_dpp_field_ent_idx_t *entry)
{
    _bcm_dpp_field_group_t *groupData = &(unitData->groupD[group]);
    _bcm_dpp_field_stage_t *stageData = &(unitData->stageD[groupData->stage]);
    _bcm_dpp_field_ent_idx_t next;
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;

    if (0 == (groupData->groupFlags & _BCM_DPP_FIELD_GROUP_IN_USE)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d group %d is not in use"),
                          unit,
                          group));
    }
    if (_bcmDppFieldEntryTypeDirExt !=
        stageData->modeBits[groupData->grpMode]->entryType) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d group %d is not a"
                                           " direct extraction group"),
                          unit,
                          group));
    }
    if ((unitData->stageD[stageData->entryDeSh1].entryDeElems +
         stageData->modeBits[groupData->grpMode]->entryCount) >=
        unitData->stageD[stageData->entryDeSh1].hwEntryDeLimit) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d stage %u would have too"
                                           " many direct extraction entries"),
                          unit,
                          groupData->stage));
    }
    if (groupData->entryCount > 7) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_FULL,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d group %d would have too"
                                           " many entries (max 8)"),
                          unit,
                          group));
    }
    if (withId) {
        next = (*entry);
    } else {
        next = unitData->entryDeFree;
    }
    LOG_DEBUG(BSL_LS_BCM_FP,
              (BSL_META_U(unit,
                          "unit %d chooses direct extraction entry %u%s\n"),
               unit,
               next + _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt),
               withId?" by request":""));
    if (next >= unitData->entryDeLimit) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d has no available direct"
                                           " extraction entries"),
                          unit));
    }
    if (unitData->entryDe[next].entryCmn.entryFlags & _BCM_DPP_FIELD_ENTRY_IN_USE) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d direct extraction entry %u"
                                           " is in use"),
                          unit,
                          next + _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt)));
    }
    *entry = next;
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_field_dir_ext_entry_add_to_group(_bcm_dpp_field_info_t *unitData,
                                          _bcm_dpp_field_grp_idx_t group,
                                          _bcm_dpp_field_ent_idx_t entry)
{
    _bcm_dpp_field_group_t *groupData;
    _bcm_dpp_field_entry_dir_ext_t *entryData;
    _bcm_dpp_field_stage_t *stageData;
    _bcm_dpp_field_ent_idx_t currEntry;
    int result = BCM_E_NONE;
 
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;
    entryData = &(unitData->entryDe[entry]);
    groupData = &(unitData->groupD[group]);
    stageData = &(unitData->stageD[groupData->stage]);
    if (0 == (groupData->groupFlags & _BCM_DPP_FIELD_GROUP_IN_USE)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d group %u not in use"),
                          unit,
                          group));
    }
    if (0 == (groupData->groupFlags & _BCM_DPP_FIELD_GROUP_IN_HW)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d group %u can not have"
                                           " entries created before setting"
                                           " group actions"),
                          unit,
                          group));
    }
    if ((unitData->stageD[stageData->entryDeSh1].entryDeElems +
         stageData->modeBits[groupData->grpMode]->entryCount) >
        unitData->stageD[stageData->entryDeSh1].hwEntryDeLimit) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE,
                         (_BSL_BCM_MSG_NO_UNIT("a new entry in unit %d group %u"
                                           " would exceed stage %u capacity"),
                          unit,
                          group,
                          groupData->stage));
    }
    if (entryData->entryCmn.entryFlags & _BCM_DPP_FIELD_ENTRY_IN_USE) {
        if (group != entryData->entryCmn.entryGroup) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d does not permit"
                                               " entries to migrate from"
                                               " one group to another"),
                              unit));
        }
        /* remove this entry from the group */
        if (entryData->entryCmn.entryPrev < unitData->entryDeLimit) {
            unitData->entryDe[entryData->entryCmn.entryPrev].entryCmn.entryNext = entryData->entryCmn.entryNext;
        } else {
            unitData->groupD[entryData->entryCmn.entryGroup].entryHead = entryData->entryCmn.entryNext;
        }
        if (entryData->entryCmn.entryNext < unitData->entryDeLimit) {
            unitData->entryDe[entryData->entryCmn.entryNext].entryCmn.entryPrev = entryData->entryCmn.entryPrev;
        } else {
            unitData->groupD[entryData->entryCmn.entryGroup].entryTail = entryData->entryCmn.entryPrev;
        }
    } else { /* if (entryData->flags & _BCM_PETRA_FIELD_ENTRY_IN_USE) */
        /* entry is not in use, so must be coming off the free list */
        if (entryData->entryCmn.entryPrev < unitData->entryDeLimit) {
            unitData->entryDe[entryData->entryCmn.entryPrev].entryCmn.entryNext = entryData->entryCmn.entryNext;
        } else {
            unitData->entryDeFree = entryData->entryCmn.entryNext;
        }
        if (entryData->entryCmn.entryNext < unitData->entryDeLimit) {
            unitData->entryDe[entryData->entryCmn.entryNext].entryCmn.entryPrev = entryData->entryCmn.entryPrev;
        }
        unitData->stageD[stageData->entryDeSh1].entryDeElems += stageData->modeBits[groupData->grpMode]->entryCount;
        unitData->stageD[stageData->entryDeSh1].entryDeCount++;
        groupData->entryCount++;
    } /* if (entryData->flags & _BCM_PETRA_FIELD_ENTRY_IN_USE) */
    /* scan for where this entry goes in the new group */
    for (currEntry = groupData->entryHead;
         (currEntry < unitData->entryDeLimit) &&
         (0 >= _bcm_dpp_compare_entry_priority(entryData->entryCmn.entryPriority,
                                               unitData->entryDe[currEntry].entryCmn.entryPriority));
         ) {
        /*
         *  ...as long as there are more entries, and the current entry
         *  priority is greater than or equal to the new entry priority,
         *  examine the next entry in the group.
         */
        currEntry = unitData->entryDe[currEntry].entryCmn.entryNext;
    }
    /* insert the new entry *before* currEntry */
    LOG_DEBUG(BSL_LS_BCM_FP,
              (BSL_META_U(unit,
                          "unit %d: inserting entry %d to group %u before"
                          " entry %d\n"),
               unit,
               entry + _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt),
               group,
               currEntry + _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt)));
    entryData->entryCmn.entryNext = currEntry;
    if (currEntry < unitData->entryDeLimit) {
        entryData->entryCmn.entryPrev = unitData->entryDe[currEntry].entryCmn.entryPrev;
        if (entryData->entryCmn.entryPrev < unitData->entryDeLimit) {
            unitData->entryDe[entryData->entryCmn.entryPrev].entryCmn.entryNext = entry;
        }
        unitData->entryDe[currEntry].entryCmn.entryPrev = entry;
    } else {
        entryData->entryCmn.entryPrev = groupData->entryTail;
        if (groupData->entryTail < unitData->entryDeLimit) {
            unitData->entryDe[groupData->entryTail].entryCmn.entryNext = entry;
        }
        groupData->entryTail = entry;
    }
    if (currEntry == groupData->entryHead) {
        groupData->entryHead = entry;
    }
    entryData->entryCmn.entryGroup = group;
    /* entry is in use, has been changed since hw commit, and is new/moved */
    entryData->entryCmn.entryFlags |= _BCM_DPP_FIELD_ENTRY_IN_USE |
                               _BCM_DPP_FIELD_ENTRY_CHANGED ;
    /* group contains changed and added/moved entry/entries */
    groupData->groupFlags |= _BCM_DPP_FIELD_GROUP_CHG_ENT |
                             _BCM_DPP_FIELD_GROUP_ADD_ENT;
#ifdef BCM_WARM_BOOT_SUPPORT
    _bcm_dpp_field_group_wb_save(unitData, group, NULL, NULL);
#endif /* def BCM_WARM_BOOT_SUPPORT */
    BCMDNX_IF_ERR_EXIT(result);
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_field_dir_ext_entry_remove(_bcm_dpp_field_info_t *unitData,
                                    _bcm_dpp_field_ent_idx_t entry)
{
    uint32 ppdr;
    _bcm_dpp_field_entry_dir_ext_t *entryData = &(unitData->entryDe[entry]);

    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;
    
    if (0 == (entryData->entryCmn.entryFlags & _BCM_DPP_FIELD_ENTRY_IN_USE)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d direct extraction entry %d"
                                           " is not in use"),
                          unit,
                          entry + _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt)));
    }
    if (0 == (entryData->entryCmn.entryFlags & _BCM_DPP_FIELD_ENTRY_IN_HW)) {
        /* not in hardware; nothing to do */
        BCM_EXIT;
    }
    LOG_DEBUG(BSL_LS_BCM_FP,
              (BSL_META_U(unit,
                          "remove unit %d entry %d old copy from"
                          " hardware with handle %d\n"),
               unit,
               entry + _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt),
               entryData->entryCmn.hwHandle));
    ppdr = soc_ppd_fp_direct_extraction_entry_remove(unitData->unitHandle,
                                                     unitData->groupD[entryData->entryCmn.entryGroup].hwHandle,
                                                     entryData->entryCmn.hwHandle);
    BCMDNX_IF_ERR_EXIT_MSG(handle_sand_result(ppdr),
                        (_BSL_BCM_MSG_NO_UNIT("unit %d unable to remove direct"
                                          " extraction entry %u from hardware"),
                         unit,
                         entry + _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt)));
    entryData->entryCmn.entryFlags &= (~_BCM_DPP_FIELD_ENTRY_IN_HW);
#ifdef BCM_WARM_BOOT_SUPPORT
    /* update backing store */
    _bcm_dpp_field_dir_ext_entry_wb_save(unitData, entry, NULL, NULL);
#endif /* def BCM_WARM_BOOT_SUPPORT */
    BCMDNX_IF_ERR_EXIT_MSG(shr_mres_free(unitData->hwResIds,
                                      unitData->stageD[unitData->groupD[entryData->entryCmn.entryGroup].stage].entryDeRes,
                                      1 /* count */,
                                      entryData->entryCmn.hwHandle),
                        (_BSL_BCM_MSG_NO_UNIT("unit %d unable to return direct"
                                          " extraction entry hardware ID %d"
                                          " to free list"),
                         unit,
                         entryData->entryCmn.hwHandle));
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_field_dir_ext_entry_destroy(_bcm_dpp_field_info_t *unitData,
                                     _bcm_dpp_field_ent_idx_t entry)
{
    _bcm_dpp_field_entry_dir_ext_t *entryData = &(unitData->entryDe[entry]);
    _bcm_dpp_field_group_t *groupData = &(unitData->groupD[entryData->entryCmn.entryGroup]);
    _bcm_dpp_field_stage_t *stageData = &(unitData->stageD[groupData->stage]);
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;

    if (0 == (entryData->entryCmn.entryFlags & _BCM_DPP_FIELD_ENTRY_IN_USE)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d not in use"),
                          unit,
                          entry + _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt)));
    }
    BCMDNX_IF_ERR_EXIT_MSG(_bcm_dpp_field_dir_ext_entry_remove(unitData, entry),
                        (_BSL_BCM_MSG_NO_UNIT("unable to remove unit %d entry %d"
                                          "from hardware"),
                         unit,
                         entry + _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt)));
    BCMDNX_IF_ERR_EXIT_MSG(_bcm_dpp_field_dir_ext_entry_clear_acts(unitData,
                                                                entry),
                        (_BSL_BCM_MSG_NO_UNIT("unable to remove all actions from"
                                          " unit %d entry %d"),
                         unit,
                         entry + _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt)));
    if (entryData->entryCmn.entryPrev < unitData->entryDeLimit) {
        unitData->entryDe[entryData->entryCmn.entryPrev].entryCmn.entryNext = entryData->entryCmn.entryNext;
    } else {
        groupData->entryHead = entryData->entryCmn.entryNext;
    }
    if (entryData->entryCmn.entryNext < unitData->entryDeLimit) {
        unitData->entryDe[entryData->entryCmn.entryNext].entryCmn.entryPrev = entryData->entryCmn.entryPrev;
    } else {
        groupData->entryTail = entryData->entryCmn.entryPrev;
    }
    entryData->entryCmn.entryNext = unitData->entryDeFree;
    entryData->entryCmn.entryPrev = unitData->entryDeLimit;
    entryData->entryCmn.entryFlags = 0;
    if (unitData->entryDeFree < unitData->entryDeLimit) {
        unitData->entryDe[unitData->entryDeFree].entryCmn.entryPrev = entry;
    }
    unitData->entryDeFree = entry;
    groupData->entryCount--;
    unitData->stageD[stageData->entryDeSh1].entryDeElems -= stageData->modeBits[groupData->grpMode]->entryCount;
    unitData->stageD[stageData->entryDeSh1].entryDeCount--;
    unitData->entryDeCount--;
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_field_dir_ext_group_hw_refresh(_bcm_dpp_field_info_t *unitData,
                                        bcm_field_group_t group,
                                        int cond)
{
    _bcm_dpp_field_group_t *groupData;
    _bcm_dpp_field_entry_dir_ext_t *entryData;
    _bcm_dpp_field_ent_idx_t entry;
    unsigned int updated = 0;
    unsigned int copied = 0;
    unsigned int count = 0;
 
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;
    
    groupData = &(unitData->groupD[group]);
    if (groupData->groupFlags & _BCM_DPP_FIELD_GROUP_ADD_ENT) {
        /* entries have been added or moved; update phase */
        LOG_DEBUG(BSL_LS_BCM_FP,
                  (BSL_META_U(unit,
                              "unit %d group %d has entries whose hardware"
                              " priority is not valid/current\n"),
                   unit,
                   group));
        _bcm_dpp_field_group_priority_recalc(unitData, group);
    }
    if (groupData->groupFlags & _BCM_DPP_FIELD_GROUP_PHASE) {
        /*
         *  Now odd phase, so priorities are all lower than the previous
         *  state (assuming there was one).  Update tail to head so that
         *  new versions of entries will 'stay' in proper order.
         */
        for (entry = groupData->entryTail;
             entry < unitData->entryDeLimit;
             entry = entryData->entryCmn.entryPrev, count++) {
            entryData = &(unitData->entryDe[entry]);
            if ((!cond) || (entryData->entryCmn.entryFlags & _BCM_DPP_FIELD_ENTRY_UPDATE)) {
                /* update this entry */
                if (entryData->entryCmn.entryFlags & _BCM_DPP_FIELD_ENTRY_WANT_HW) {
                    /* entry wants to be in hardware */
                    if ((entryData->entryCmn.entryFlags & _BCM_DPP_FIELD_ENTRY_CHANGED) ||
                        (0 == (entryData->entryCmn.entryFlags & _BCM_DPP_FIELD_ENTRY_IN_HW))) {
                        /* entry has changed or is not yet in hardware, add it */
                        updated++;
                        BCMDNX_IF_ERR_EXIT_MSG(_bcm_dpp_field_dir_ext_entry_hardware_install(unitData,
                                                                                          entry),
                                            (_BSL_BCM_MSG_NO_UNIT("unit %d unable"
                                                              " to insert"
                                                              " direct"
                                                              " extraction"
                                                              " entry %d into"
                                                              " hardware"),
                                             unit,
                                             entry +
                                             _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt)));
                    }
                } else { /* if (entryData->flags & _BCM_PETRA_FIELD_ENTRY_WANT_HW) */
                    /* entry does not want to be in hardware */
                    if (entryData->entryCmn.entryFlags & _BCM_DPP_FIELD_ENTRY_IN_HW) {
                        /* entry is in hardware, so take it out */
                        updated++;
                        BCMDNX_IF_ERR_EXIT_MSG(_bcm_dpp_field_dir_ext_entry_remove(unitData,
                                                                                entry),
                                            (_BSL_BCM_MSG_NO_UNIT("unit %d unable"
                                                              " to remove"
                                                              " direct"
                                                              " extraction"
                                                              " entry %d from"
                                                              " hardware"),
                                             unit,
                                             entry +
                                             _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt)));
                    }
                } /* if (entryData->flags & _BCM_PETRA_FIELD_ENTRY_WANT_HW) */
            } else { /* if (update this entry) */
                /* no update to this entry... */
                if (entryData->entryCmn.entryFlags & _BCM_DPP_FIELD_ENTRY_IN_HW) {
                    /* ...but need to shift it to new priority anyway */
                    copied++;
                    BCMDNX_IF_ERR_EXIT_MSG(_bcm_dpp_field_dir_ext_entry_hardware_install(unitData,
                                                                                      entry),
                                        (_BSL_BCM_MSG_NO_UNIT("unit %d unable"
                                                          " to insert"
                                                          " direct"
                                                          " extraction"
                                                          " entry %d into"
                                                          " hardware"),
                                         unit,
                                         entry +
                                         _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt)));
                }
            } /* if (update this entry) */
        } /* for (all entries in this group) */
    } else { /* if (groupData->flags & _BCM_DPP_FIELD_GROUP_PHASE) */
        /*
         *  Now even phase, so priorities are all higher than the previous
         *  state (assuming there was one).  Update head to tail so that
         *  new versions of entries will 'stay' in proper order.
         */
        for (entry = groupData->entryHead;
             entry < unitData->entryDeLimit;
             entry = entryData->entryCmn.entryNext, count++) {
            entryData = &(unitData->entryDe[entry]);
            if ((!cond) || (entryData->entryCmn.entryFlags & _BCM_DPP_FIELD_ENTRY_UPDATE)) {
                /* update this entry */
                if (entryData->entryCmn.entryFlags & _BCM_DPP_FIELD_ENTRY_WANT_HW) {
                    /* entry wants to be in hardware */
                    if ((entryData->entryCmn.entryFlags & _BCM_DPP_FIELD_ENTRY_CHANGED) ||
                        (0 == (entryData->entryCmn.entryFlags & _BCM_DPP_FIELD_ENTRY_IN_HW))) {
                        /* entry has changed or is not yet in hardware, add it */
                        updated++;
                        BCMDNX_IF_ERR_EXIT_MSG(_bcm_dpp_field_dir_ext_entry_hardware_install(unitData,
                                                                                          entry),
                                            (_BSL_BCM_MSG_NO_UNIT("unit %d unable"
                                                              " to insert"
                                                              " direct"
                                                              " extraction"
                                                              " entry %d into"
                                                              " hardware"),
                                             unit,
                                             entry +
                                             _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt)));
                    }
                } else { /* if (entryData->flags & _BCM_PETRA_FIELD_ENTRY_WANT_HW) */
                    /* entry does not want to be in hardware */
                    if (entryData->entryCmn.entryFlags & _BCM_DPP_FIELD_ENTRY_IN_HW) {
                        /* entry is in hardware, so take it out */
                        updated++;
                        BCMDNX_IF_ERR_EXIT_MSG(_bcm_dpp_field_dir_ext_entry_remove(unitData,
                                                                                entry),
                                            (_BSL_BCM_MSG_NO_UNIT("unit %d unable"
                                                              " to remove"
                                                              " direct"
                                                              " extraction"
                                                              " entry %d from"
                                                              " hardware"),
                                             unit,
                                             entry +
                                             _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt)));
                    }
                } /* if (entryData->flags & _BCM_PETRA_FIELD_ENTRY_WANT_HW) */
            } else { /* if (update this entry) */
                /* no update to this entry... */
                if (entryData->entryCmn.entryFlags & _BCM_DPP_FIELD_ENTRY_IN_HW) {
                    /* ...but need to shift it to new priority anyway */
                    copied++;
                    BCMDNX_IF_ERR_EXIT_MSG(_bcm_dpp_field_dir_ext_entry_hardware_install(unitData,
                                                                                      entry),
                                        (_BSL_BCM_MSG_NO_UNIT("unit %d unable"
                                                          " to insert"
                                                          " direct"
                                                          " extraction"
                                                          " entry %d into"
                                                          " hardware"),
                                         unit,
                                         entry +
                                         _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt)));
                }
            } /* if (update this entry) */
        } /* for (all entries in this group) */
    } /* if (groupData->flags & _BCM_DPP_FIELD_GROUP_PHASE) */
    LOG_DEBUG(BSL_LS_BCM_FP,
              (BSL_META_U(unit,
                          "unit %d group %d: %d entries, %d updated,"
                          " %d copied\n"),
               unit,
               group,
               count,
               updated,
               copied));
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_field_dir_ext_entry_install(_bcm_dpp_field_info_t *unitData,
                                     _bcm_dpp_field_ent_idx_t entry)
{
    _bcm_dpp_field_entry_dir_ext_t *entryData;
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;

    entryData = &(unitData->entryDe[entry]);
    if (0 == (entryData->entryCmn.entryFlags & _BCM_DPP_FIELD_ENTRY_IN_USE)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d not in use"),
                          unit,
                          entry + _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt)));
    }
        /* entry exists */
    entryData->entryCmn.entryFlags |= (_BCM_DPP_FIELD_ENTRY_WANT_HW |
                         _BCM_DPP_FIELD_ENTRY_UPDATE);
    if (entryData->entryCmn.entryFlags & _BCM_DPP_FIELD_ENTRY_NEW) {
        /*
         *  This entry is new or has been moved recently, so its
         *  hardware priority is either not assigned or not valid.  We
         *  need to reassign hardware priorities for the group so they
         *  include all of the entries now in the group, in their
         *  current positions.
         *
         *  After reassigning hardware priorities, though, we need to
         *  refresh the entire group in hardware, so first we set this
         *  entry as wanting hardware, recompute the priorities, and
         *  then finally update all of the entries that should be
         *  in hardware.
         *
         *  Since this must not, according to the API spec, affect the
         *  state of entries other than the specified one, there is a
         *  lot of additional work when there are many entries.
         */
        entryData->entryCmn.entryFlags |= (_BCM_DPP_FIELD_ENTRY_WANT_HW |
                             _BCM_DPP_FIELD_ENTRY_UPDATE);
        BCMDNX_IF_ERR_EXIT_MSG(_bcm_dpp_field_dir_ext_group_hw_refresh(unitData,
                                                                    entryData->entryCmn.entryGroup,
                                                                    TRUE),
                            (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d, group %u"
                                              " refresh failed"),
                             unit,
                             entry + _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt),
                             entryData->entryCmn.entryGroup));
    } else { /* if (entryData->flags & _BCM_PETRA_FIELD_ENTRY_NEW) */
        /* entry already has priority, so can just add to hardware */
        BCMDNX_IF_ERR_EXIT_MSG(_bcm_dpp_field_dir_ext_entry_hardware_install(unitData,
                                                                          entry),
                            (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d did not"
                                              " install to hardware"),
                             unit,
                             entry + _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt)));
    } /* if (entryData->flags & _BCM_PETRA_FIELD_ENTRY_NEW) */
exit:
    BCMDNX_FUNC_RETURN;
}

void
_bcm_dpp_field_dir_ext_entry_copy_id(_bcm_dpp_field_info_t *unitData,
                                     _bcm_dpp_field_ent_idx_t dest,
                                     _bcm_dpp_field_ent_idx_t source)
{
    _bcm_dpp_field_ent_idx_t next;
    _bcm_dpp_field_ent_idx_t prev;
    _bcm_dpp_field_entry_dir_ext_t *destData = &(unitData->entryDe[dest]);
    _bcm_dpp_field_entry_dir_ext_t *sourceData = &(unitData->entryDe[source]);

    /* preserve previous entry list position for now */
    next = destData->entryCmn.entryNext;
    prev = destData->entryCmn.entryPrev;
    /* copy the old entry to the new entry */
    sal_memcpy(destData, sourceData, sizeof(*destData));
    /* keep new entry in previous entry list for now */
    destData->entryCmn.entryNext = next;
    destData->entryCmn.entryPrev = prev;
    destData->entryCmn.entryPriority = BCM_FIELD_ENTRY_PRIO_DEFAULT;
    /* the copy is not yet in hardware, nor is it yet in use */
    destData->entryCmn.entryFlags &= (~(_BCM_DPP_FIELD_ENTRY_IN_USE |
                          _BCM_DPP_FIELD_ENTRY_IN_HW |
                          _BCM_DPP_FIELD_ENTRY_WANT_HW));
}

#if _BCM_DPP_FIELD_GET_VERIFY_PPD || _BCM_DPP_FIELD_GET_USE_PPD
int
_bcm_dpp_field_dir_ext_hardware_entry_check(_bcm_dpp_field_info_t *unitData,
                                            bcm_field_entry_t entry,
                                            SOC_PPD_FP_DIR_EXTR_ENTRY_INFO *entInfoDe)
{
    uint32 soc_sandResult;
    uint8 okay;
    unsigned int index;
    unsigned int offset;
    unsigned int j;
    unsigned int bits;
    uint32 mask;
    _bcm_dpp_field_group_t *groupData;
    _bcm_dpp_field_entry_dir_ext_t *entryData;
    _bcm_dpp_field_ent_idx_t ent;
    int result;
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;

    if ((_BCM_DPP_FIELD_ENT_BIAS(unit, DirExt) > entry) ||
        ((unitData->entryDeLimit + _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt)) <= entry)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d is not a"
                                           " direct extraction entry"),
                          unit,
                          entry));
    }
    ent = entry - _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt);
    entryData = &(unitData->entryDe[ent]);
    groupData = &(unitData->groupD[entryData->entryCmn.entryGroup]);

    SOC_PPD_FP_DIR_EXTR_ENTRY_INFO_clear(entInfoDe);
    soc_sandResult = soc_ppd_fp_direct_extraction_entry_get(unitData->unitHandle,
                                                            unitData->groupD[entryData->entryCmn.entryGroup].hwHandle,
                                                            entryData->entryCmn.hwHandle,
                                                            &okay,
                                                            entInfoDe);
    result = handle_sand_result(soc_sandResult);
    BCMDNX_IF_ERR_EXIT_MSG(result,
                        (_BSL_BCM_MSG_NO_UNIT("unit %d unable to fetch"
                                          " entry %u hardware"
                                          " information (%u):"
                                          " %d (%s)"),
                         unit,
                         entry,
                         entryData->entryCmn.hwHandle,
                         result,
                         _SHR_ERRMSG(result)));
    if (!okay) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d entry %u not okay"
                                           " trying to fetch entry"
                                           " from hardware"),
                          unit,
                          entry));
    }
#if _BCM_DPP_FIELD_GET_VERIFY_PPD
    if (entryData->entryCmn.hwPriority != entInfoDe->priority) {
#if !_BCM_DPP_FIELD_GET_VERIFY_RETURN_ERROR
        LOG_WARN(BSL_LS_BCM_FP,
                 (BSL_META_U(unit,
                             "unit %d entry %d hardware priority %d does"
                             " not match PPD layer priority %d\n"),
                  unit,
                  entry,
                  entryData->entryCmn.hwPriority,
                  entInfoDe->priority));
#else /* !_BCM_DPP_FIELD_GET_VERIFY_RETURN_ERROR */
        BCMDNX_ERR_EXIT_MSG(_BCM_DPP_FIELD_GET_VERIFY_ERROR,
                         (_BSL_BCM_MSG_NO_UNIT(("unit %d entry %d hardware priority"
                                            " %d does not match PPD layer"
                                            " priority %d\n"),
                                           unit,
                                           entry,
                                           entryData->entryCmn.hwPriority,
                                           entInfoDe->priority));
#endif /* !_BCM_DPP_FIELD_GET_VERIFY_RETURN_ERROR */
    }
    for (index = 0;
         index < unitData->stageD[groupData->stage].devInfo->entryMaxQuals;
         index++) {
        if (_BCM_DPP_FIELD_PPD_QUAL_VALID(entryData->entryCmn.entryQual[index].hwType)) {
            /* qualifier is in use */
            for (offset = 0;
                 (offset < SOC_PPD_FP_NOF_QUALS_PER_DB_MAX) &&
                 (entInfoDe->qual_vals[offset].type !=
                  entryData->entryCmn.entryQual[index].hwType);
                 offset++) {
                /* iterate until found matching qualifier */
            }
            if (offset < SOC_PPD_FP_NOF_QUALS_PER_DB_MAX) {
                /* found matching qualifier; compare it */
                if ((COMPILER_64_HI(entryData->entryCmn.entryQual[index].qualData) !=
                     entInfoDe->qual_vals[offset].val.arr[1]) ||
                    (COMPILER_64_LO(entryData->entryCmn.entryQual[index].qualData) !=
                     entInfoDe->qual_vals[offset].val.arr[0]) ||
                    (COMPILER_64_HI(entryData->entryCmn.entryQual[index].qualMask) !=
                     entInfoDe->qual_vals[offset].is_valid.arr[1]) ||
                    (COMPILER_64_LO(entryData->entryCmn.entryQual[index].qualMask) !=
                     entInfoDe->qual_vals[offset].is_valid.arr[0])) {
#if !_BCM_DPP_FIELD_GET_VERIFY_RETURN_ERROR
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "unit %d entry %d qualifier %d %s"
                                         " (%d) %08X%08X/%08X%08X -> %s (%d)"
                                         " does not match PPD qualifier %d %s"
                                         " (%d) %08X%08X/%08X%08X\n"),
                              unit,
                              entry,
                              index,
                              (entryData->entryCmn.entryQual[index].qualType < bcmFieldQualifyCount)?_bcm_dpp_field_qual_name[entryData->entryCmn.entryQual[index].qualType]:"-",
                              entryData->entryCmn.entryQual[index].qualType,
                              COMPILER_64_HI(entryData->entryCmn.entryQual[index].qualData),
                              COMPILER_64_LO(entryData->entryCmn.entryQual[index].qualData),
                              COMPILER_64_HI(entryData->entryCmn.entryQual[index].qualMask),
                              COMPILER_64_LO(entryData->entryCmn.entryQual[index].qualMask),
                              SOC_PPD_FP_QUAL_TYPE_to_string(entryData->entryCmn.entryQual[index].hwType),
                              entryData->entryCmn.entryQual[index].hwType,
                              offset,
                              SOC_PPD_FP_QUAL_TYPE_to_string(entInfoDe->qual_vals[offset].type),
                              entInfoDe->qual_vals[offset].type,
                              entInfoDe->qual_vals[offset].val.arr[1],
                              entInfoDe->qual_vals[offset].val.arr[0],
                              entInfoDe->qual_vals[offset].is_valid.arr[1],
                              entInfoDe->qual_vals[offset].is_valid.arr[0]));
#else /* !_BCM_DPP_FIELD_GET_VERIFY_RETURN_ERROR */
                    BCMDNX_ERR_EXIT_MSG(_BCM_DPP_FIELD_GET_VERIFY_ERROR,
                                     (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d"
                                                       " qualifier %d %s (%d)"
                                                       " %08X%08X/%08X%08X ->"
                                                       " %s (%d) does not match"
                                                       " PPD qualifier %d %s"
                                                       " (%d):"
                                                       " %08X%08X/%08X%08X"),
                                      unit,
                                      entry,
                                      index,
                                      (entryData->entryCmn.entryQual[index].qualType < bcmFieldQualifyCount)?_bcm_dpp_field_qual_name[entryData->entryCmn.entryQual[index].qualType]:"-",
                                      entryData->entryCmn.entryQual[index].qualType,
                                      COMPILER_64_HI(entryData->entryCmn.entryQual[index].qualData),
                                      COMPILER_64_LO(entryData->entryCmn.entryQual[index].qualData),
                                      COMPILER_64_HI(entryData->entryCmn.entryQual[index].qualMask),
                                      COMPILER_64_LO(entryData->entryCmn.entryQual[index].qualMask),
                                      SOC_PPD_FP_QUAL_TYPE_to_string(entryData->entryCmn.entryQual[index].hwType),
                                      entryData->entryCmn.entryQual[index].hwType,
                                      offset,
                                      SOC_PPD_FP_QUAL_TYPE_to_string(entInfoDe->qual_vals[offset].type),
                                      entInfoDe->qual_vals[offset].type,
                                      entInfoDe->qual_vals[offset].val.arr[1],
                                      entInfoDe->qual_vals[offset].val.arr[0],
                                      entInfoDe->qual_vals[offset].is_valid.arr[1],
                                      entInfoDe->qual_vals[offset].is_valid.arr[0]));
#endif /* !_BCM_DPP_FIELD_GET_VERIFY_RETURN_ERROR */
                } /* if (qualifier data or mask differs) */
            } else { /* if (offset < SOC_PPD_FP_NOF_QUALS_PER_DB_MAX) */
#if !_BCM_DPP_FIELD_GET_VERIFY_RETURN_ERROR
                LOG_WARN(BSL_LS_BCM_FP,
                         (BSL_META_U(unit,
                                     "unit %d entry %d qualifier %d %s (%d)"
                                     " %08X%08X/%08X%08X -> %s (%d) does"
                                     " not have matching PPD qualifier\n"),
                          unit,
                          entry,
                          index,
                          (entryData->entryCmn.entryQual[index].qualType < bcmFieldQualifyCount)?_bcm_dpp_field_qual_name[entryData->entryCmn.entryQual[index].qualType]:"-",
                          entryData->entryCmn.entryQual[index].qualType,
                          COMPILER_64_HI(entryData->entryCmn.entryQual[index].qualData),
                          COMPILER_64_LO(entryData->entryCmn.entryQual[index].qualData),
                          COMPILER_64_HI(entryData->entryCmn.entryQual[index].qualMask),
                          COMPILER_64_LO(entryData->entryCmn.entryQual[index].qualMask),
                          SOC_PPD_FP_QUAL_TYPE_to_string(entryData->entryCmn.entryQual[index].hwType),
                          entryData->entryCmn.entryQual[index].hwType));
#else /* !_BCM_DPP_FIELD_GET_VERIFY_RETURN_ERROR */
                BCMDNX_ERR_EXIT_MSG(_BCM_DPP_FIELD_GET_VERIFY_ERROR,
                                 (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d qualifier"
                                                   " %d %s (%d) %08X%08X"
                                                   "/%08X%08X -> %s (%d) does"
                                                   " not have matching PPD"
                                                   " qualifier"),
                                  unit,
                                  entry,
                                  index,
                                  (entryData->entryCmn.entryQual[index].qualType < bcmFieldQualifyCount)?_bcm_dpp_field_qual_name[entryData->entryCmn.entryQual[index].qualType]:"-",
                                  entryData->entryCmn.entryQual[index].qualType,
                                  COMPILER_64_HI(entryData->entryCmn.entryQual[index].qualData),
                                  COMPILER_64_LO(entryData->entryCmn.entryQual[index].qualData),
                                  COMPILER_64_HI(entryData->entryCmn.entryQual[index].qualMask),
                                  COMPILER_64_LO(entryData->entryCmn.entryQual[index].qualMask),
                                  SOC_PPD_FP_QUAL_TYPE_to_string(entryData->entryCmn.entryQual[index].hwType),
                                  entryData->entryCmn.entryQual[index].hwType));
#endif /* !_BCM_DPP_FIELD_GET_VERIFY_RETURN_ERROR */
            } /* if (offset < SOC_PPD_FP_NOF_QUALS_PER_DB_MAX) */
        } /* if (qualifier is in use) */
    } /* for (all possible qualifier slots on this entry) */
    for (index = 0;
         index < unitData->stageD[groupData->stage].devInfo->entryMaxActs;
         index++) {
        if (_BCM_DPP_FIELD_PPD_ACTION_VALID(entryData->deAct[index].hwParam.type)) {
            /* action is in use */
            for (offset = 0;
                 (offset < SOC_PPD_FP_NOF_ACTIONS_PER_DB_MAX) &&
                 (entInfoDe->actions[index].type !=
                  entryData->deAct[index].hwParam.type);
                 offset++) {
                /* iterate until found matching action */
            }
            if (offset < SOC_PPD_FP_NOF_ACTIONS_PER_DB_MAX) {
                BCMDNX_IF_ERR_EXIT(_bcm_dpp_ppd_action_bits(unitData,
                                                                 groupData->stage,
                                                                 entryData->deAct[index].hwParam.type,
                                                                 &bits));
                mask = (1 << bits) - 1;
                if (entryData->deAct[index].hwParam.nof_fields !=
                    entInfoDe->actions[offset].nof_fields) {
#if !_BCM_DPP_FIELD_GET_VERIFY_RETURN_ERROR
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "unit %d entry %d action %d %s (%d)"
                                         " -> %d %s (%d) number of fields %d"
                                         " differs from hardware value %d\n"),
                              unit,
                              entry,
                              index,
                              (entryData->deAct[index].bcmType<bcmFieldActionCount)?_bcm_dpp_field_action_name[entryData->deAct[index].bcmType]:"-",
                              entryData->deAct[index].bcmType,
                              offset,
                              SOC_PPD_FP_ACTION_TYPE_to_string(entInfoDe->actions[offset].type),
                              entInfoDe->actions[offset].type,
                              entryData->deAct[index].hwParam.nof_fields,
                              entInfoDe->actions[offset].nof_fields));
#else /* !_BCM_DPP_FIELD_GET_VERIFY_RETURN_ERROR */
                    BCMDNX_ERR_EXIT_MSG(_BCM_DPP_FIELD_GET_VERIFY_ERROR,
                                     (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d action"
                                                       " %d %s (%d) -> %d %s"
                                                       " (%d) number of fields"
                                                       " %d differs from"
                                                       " hardware value %d"),
                                      unit,
                                      entry,
                                      index,
                                      (entryData->deAct[index].bcmType<bcmFieldActionCount)?_bcm_dpp_field_action_name[entryData->deAct[index].bcmType]:"-",
                                      entryData->deAct[index].bcmType,
                                      offset,
                                      SOC_PPD_FP_ACTION_TYPE_to_string(entInfoDe->actions[offset].type),
                                      entInfoDe->actions[offset].type,
                                      entryData->deAct[index].hwParam.nof_fields,
                                      entInfoDe->actions[offset].nof_fields));
#endif /* !_BCM_DPP_FIELD_GET_VERIFY_RETURN_ERROR */
                }
                if ((entryData->deAct[index].hwParam.base_val & mask) !=
                    (entInfoDe->actions[offset].base_val & mask)) {
#if !_BCM_DPP_FIELD_GET_VERIFY_RETURN_ERROR
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "unit %d entry %d action %d %s (%d)"
                                         " -> %d %s (%d) bias %08X differs"
                                         " from hardware value %08X\n"),
                              unit,
                              entry,
                              index,
                              (entryData->deAct[index].bcmType<bcmFieldActionCount)?_bcm_dpp_field_action_name[entryData->deAct[index].bcmType]:"-",
                              entryData->deAct[index].bcmType,
                              offset,
                              SOC_PPD_FP_ACTION_TYPE_to_string(entInfoDe->actions[offset].type),
                              entInfoDe->actions[offset].type,
                              entryData->deAct[index].hwParam.base_val,
                              entInfoDe->actions[offset].base_val));
#else /* !_BCM_DPP_FIELD_GET_VERIFY_RETURN_ERROR */
                    BCMDNX_ERR_EXIT_MSG(_BCM_DPP_FIELD_GET_VERIFY_ERROR,
                                     (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d action"
                                                       " %d %s (%d) -> %d %s"
                                                       " (%d) bias %08X differs"
                                                       " from hardware value"
                                                       " %08X"),
                                      unit,
                                      entry,
                                      index,
                                      (entryData->deAct[index].bcmType<bcmFieldActionCount)?_bcm_dpp_field_action_name[entryData->deAct[index].bcmType]:"-",
                                      entryData->deAct[index].bcmType,
                                      offset,
                                      SOC_PPD_FP_ACTION_TYPE_to_string(entInfoDe->actions[offset].type),
                                      entInfoDe->actions[offset].type,
                                      entryData->deAct[index].hwParam.base_val,
                                      entInfoDe->actions[offset].base_val));
#endif /* !_BCM_DPP_FIELD_GET_VERIFY_RETURN_ERROR */
                }
                for (j = 0;
                     j < SOC_PPC_FP_DIR_EXTR_MAX_NOF_FIELDS;
                     j++) {
                    if (entryData->deAct[index].hwParam.fld_ext[j].type !=
                        entInfoDe->actions[offset].fld_ext[j].type) {
#if !_BCM_DPP_FIELD_GET_VERIFY_RETURN_ERROR
                        LOG_WARN(BSL_LS_BCM_FP,
                                 (BSL_META_U(unit,
                                             "unit %d entry %d action %d %s"
                                             " (%d) -> %d %s (%d) extraction"
                                             " %d type %d differs from"
                                             " hardware value %d\n"),
                                  unit,
                                  entry,
                                  index,
                                  (entryData->deAct[index].bcmType<bcmFieldActionCount)?_bcm_dpp_field_action_name[entryData->deAct[index].bcmType]:"-",
                                  entryData->deAct[index].bcmType,
                                  offset,
                                  SOC_PPD_FP_ACTION_TYPE_to_string(entInfoDe->actions[offset].type),
                                  entInfoDe->actions[offset].type,
                                  j,
                                  entryData->deAct[index].hwParam.fld_ext[j].type,
                                  entInfoDe->actions[offset].fld_ext[j].type));
#else /* !_BCM_DPP_FIELD_GET_VERIFY_RETURN_ERROR */
                        BCMDNX_ERR_EXIT_MSG(_BCM_DPP_FIELD_GET_VERIFY_ERROR,
                                         (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d"
                                                           " action %d %s (%d)"
                                                           " -> %d %s (%d)"
                                                           " extraction %d"
                                                           " type %d differs"
                                                           " from hardware"
                                                           " value %d"),
                                          unit,
                                          entry,
                                          index,
                                          (entryData->deAct[index].bcmType<bcmFieldActionCount)?_bcm_dpp_field_action_name[entryData->deAct[index].bcmType]:"-",
                                          entryData->deAct[index].bcmType,
                                          offset,
                                          SOC_PPD_FP_ACTION_TYPE_to_string(entInfoDe->actions[offset].type),
                                          entInfoDe->actions[offset].type,
                                          j,
                                          entryData->deAct[index].hwParam.fld_ext[j].type,
                                          entInfoDe->actions[offset].fld_ext[j].type));
#endif /* !_BCM_DPP_FIELD_GET_VERIFY_RETURN_ERROR */
                    }
                    if (entryData->deAct[index].hwParam.fld_ext[j].nof_bits !=
                        entInfoDe->actions[offset].fld_ext[j].nof_bits) {
#if !_BCM_DPP_FIELD_GET_VERIFY_RETURN_ERROR
                        LOG_WARN(BSL_LS_BCM_FP,
                                 (BSL_META_U(unit,
                                             "unit %d entry %d action %d %s"
                                             " (%d) -> %d %s (%d) extraction"
                                             " %d number of bits %d differs"
                                             " from hardware value %d\n"),
                                  unit,
                                  entry,
                                  index,
                                  (entryData->deAct[index].bcmType<bcmFieldActionCount)?_bcm_dpp_field_action_name[entryData->deAct[index].bcmType]:"-",
                                  entryData->deAct[index].bcmType,
                                  offset,
                                  SOC_PPD_FP_ACTION_TYPE_to_string(entInfoDe->actions[offset].type),
                                  entInfoDe->actions[offset].type,
                                  j,
                                  entryData->deAct[index].hwParam.fld_ext[j].nof_bits,
                                  entInfoDe->actions[offset].fld_ext[j].nof_bits));
#else /* !_BCM_DPP_FIELD_GET_VERIFY_RETURN_ERROR */
                        BCMDNX_ERR_EXIT_MSG(_BCM_DPP_FIELD_GET_VERIFY_ERROR,
                                         (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d"
                                                           " action %d %s (%d)"
                                                           " -> %d %s (%d)"
                                                           " extraction %d"
                                                           " number of bits %d"
                                                           " differs from"
                                                           " hardware"
                                                           " value %d"),
                                          unit,
                                          entry,
                                          index,
                                          (entryData->deAct[index].bcmType<bcmFieldActionCount)?_bcm_dpp_field_action_name[entryData->deAct[index].bcmType]:"-",
                                          entryData->deAct[index].bcmType,
                                          offset,
                                          SOC_PPD_FP_ACTION_TYPE_to_string(entInfoDe->actions[offset].type),
                                          entInfoDe->actions[offset].type,
                                          j,
                                          entryData->deAct[index].hwParam.fld_ext[j].nof_bits,
                                          entInfoDe->actions[offset].fld_ext[j].nof_bits));
#endif /* !_BCM_DPP_FIELD_GET_VERIFY_RETURN_ERROR */
                    }
                    mask = (1 << entryData->deAct[index].hwParam.fld_ext[j].nof_bits) - 1;
                    if ((entryData->deAct[index].hwParam.fld_ext[j].cst_val &
                         mask) !=
                        (entInfoDe->actions[offset].fld_ext[j].cst_val &
                         mask)) {
#if !_BCM_DPP_FIELD_GET_VERIFY_RETURN_ERROR
                        LOG_WARN(BSL_LS_BCM_FP,
                                 (BSL_META_U(unit,
                                             "unit %d entry %d action %d %s"
                                             " (%d) -> %d %s (%d) extraction"
                                             " %d constant %08X differs"
                                             " from hardware value %08X\n"),
                                  unit,
                                  entry,
                                  index,
                                  (entryData->deAct[index].bcmType<bcmFieldActionCount)?_bcm_dpp_field_action_name[entryData->deAct[index].bcmType]:"-",
                                  entryData->deAct[index].bcmType,
                                  offset,
                                  SOC_PPD_FP_ACTION_TYPE_to_string(entInfoDe->actions[offset].type),
                                  entInfoDe->actions[offset].type,
                                  j,
                                  entryData->deAct[index].hwParam.fld_ext[j].cst_val,
                                  entInfoDe->actions[offset].fld_ext[j].cst_val));
#else /* !_BCM_DPP_FIELD_GET_VERIFY_RETURN_ERROR */
                        BCMDNX_ERR_EXIT_MSG(_BCM_DPP_FIELD_GET_VERIFY_ERROR,
                                         (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d"
                                                           " action %d %s (%d)"
                                                           " -> %d %s (%d)"
                                                           " extraction %d"
                                                           " constant %08X"
                                                           " differs from"
                                                           " hardware"
                                                           " value %08X"),
                                          unit,
                                          entry,
                                          index,
                                          (entryData->deAct[index].bcmType<bcmFieldActionCount)?_bcm_dpp_field_action_name[entryData->deAct[index].bcmType]:"-",
                                          entryData->deAct[index].bcmType,
                                          offset,
                                          SOC_PPD_FP_ACTION_TYPE_to_string(entInfoDe->actions[offset].type),
                                          entInfoDe->actions[offset].type,
                                          j,
                                          entryData->deAct[index].hwParam.fld_ext[j].cst_val,
                                          entInfoDe->actions[offset].fld_ext[j].cst_val));
#endif /* !_BCM_DPP_FIELD_GET_VERIFY_RETURN_ERROR */
                    }
                    if (entryData->deAct[index].hwParam.fld_ext[j].fld_lsb !=
                        entInfoDe->actions[offset].fld_ext[j].fld_lsb) {
#if !_BCM_DPP_FIELD_GET_VERIFY_RETURN_ERROR
                        LOG_WARN(BSL_LS_BCM_FP,
                                 (BSL_META_U(unit,
                                             "unit %d entry %d action %d %s"
                                             " (%d) -> %d %s (%d) extraction"
                                             " %d least significant bit %d"
                                             " differs from hardware value"
                                             " %d\n"),
                                  unit,
                                  entry,
                                  index,
                                  (entryData->deAct[index].bcmType<bcmFieldActionCount)?_bcm_dpp_field_action_name[entryData->deAct[index].bcmType]:"-",
                                  entryData->deAct[index].bcmType,
                                  offset,
                                  SOC_PPD_FP_ACTION_TYPE_to_string(entInfoDe->actions[offset].type),
                                  entInfoDe->actions[offset].type,
                                  j,
                                  entryData->deAct[index].hwParam.fld_ext[j].fld_lsb,
                                  entInfoDe->actions[offset].fld_ext[j].fld_lsb));
#else /* !_BCM_DPP_FIELD_GET_VERIFY_RETURN_ERROR */
                        BCMDNX_ERR_EXIT_MSG(_BCM_DPP_FIELD_GET_VERIFY_ERROR,
                                         (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d"
                                                           " action %d %s (%d)"
                                                           " -> %d %s (%d)"
                                                           " extraction %d"
                                                           " least significant"
                                                           " bit %d differs"
                                                           " from hardware"
                                                           " value %d"),
                                          unit,
                                          entry,
                                          index,
                                          (entryData->deAct[index].bcmType<bcmFieldActionCount)?_bcm_dpp_field_action_name[entryData->deAct[index].bcmType]:"-",
                                          entryData->deAct[index].bcmType,
                                          offset,
                                          SOC_PPD_FP_ACTION_TYPE_to_string(entInfoDe->actions[offset].type),
                                          entInfoDe->actions[offset].type,
                                          j,
                                          entryData->deAct[index].hwParam.fld_ext[j].fld_lsb,
                                          entInfoDe->actions[offset].fld_ext[j].fld_lsb));
#endif /* !_BCM_DPP_FIELD_GET_VERIFY_RETURN_ERROR */
                    }
                } /* for (all possible extractions in this action) */
            } else { /* if (offset < SOC_PPD_FP_NOF_ACTIONS_PER_DB_MAX) */
#if !_BCM_DPP_FIELD_GET_VERIFY_RETURN_ERROR
                LOG_WARN(BSL_LS_BCM_FP,
                         (BSL_META_U(unit,
                                     "unit %d entry %d action %d %s (%d) ->"
                                     " %s (%d) does not have matching PPD"
                                     " action\n"),
                          unit,
                          entry,
                          index,
                          (entryData->deAct[index].bcmType < bcmFieldActionCount)?_bcm_dpp_field_action_name[entryData->deAct[index].bcmType]:"-",
                          entryData->deAct[index].bcmType,
                          SOC_PPD_FP_ACTION_TYPE_to_string(entryData->deAct[index].hwParam.type),
                          entryData->deAct[index].hwParam.type));
#else /* !_BCM_DPP_FIELD_GET_VERIFY_RETURN_ERROR */
                BCMDNX_ERR_EXIT_MSG(_BCM_DPP_FIELD_GET_VERIFY_ERROR,
                                 (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d action %d"
                                                   " %s (%d) -> %s (%d) does"
                                                   " not have matching PPD"
                                                   " action\n"),
                                  unit,
                                  entry,
                                  index,
                                  (entryData->deAct[index].bcmType < bcmFieldActionCount)?_bcm_dpp_field_action_name[entryData->deAct[index].bcmType]:"-",
                                  entryData->deAct[index].bcmType,
                                  SOC_PPD_FP_ACTION_TYPE_to_string(entryData->deAct[index].hwParam.type),
                                  entryData->deAct[index].hwParam.type));
#endif /* !_BCM_DPP_FIELD_GET_VERIFY_RETURN_ERROR */
            } /* if (offset < SOC_PPD_FP_NOF_ACTIONS_PER_DB_MAX) */
        } /* if (action is in use) */
    } /* for (all possible action slots on this entry) */
#endif /* _BCM_DPP_FIELD_GET_VERIFY_PPD */

exit:
    BCMDNX_FUNC_RETURN;
}
#endif /* _BCM_DPP_FIELD_GET_VERIFY_PPD || _BCM_DPP_FIELD_GET_USE_PPD */


/******************************************************************************
 *
 *  Functions and data exported to API users
 */

int
bcm_petra_field_direct_extraction_action_add(int unit,
                                             bcm_field_entry_t entry,
                                             bcm_field_extraction_action_t action,
                                             int count,
                                             bcm_field_extraction_field_t *extractions)
{
    _DPP_FIELD_COMMON_LOCALS;
    _bcm_dpp_field_ent_idx_t ent;
    BCMDNX_INIT_FUNC_DEFS;

#if (defined(BCM_DPP_SUPPORT) && defined(BCM_WARM_BOOT_SUPPORT))
    _DCMN_BCM_WARM_BOOT_API_TEST_OVERRIDE_WB_TEST_MODE(unit);
#endif /* (defined(BCM_DPP_SUPPORT) && defined(BCM_WARM_BOOT_SUPPORT)) */

    _BCM_DPP_SWITCH_API_START(unit);
    _DPP_FIELD_UNIT_CHECK(unit, unitData);
    _DPP_FIELD_UNIT_LOCK(unitData);

    if ((_BCM_DPP_FIELD_ENT_BIAS(unit, DirExt) <= entry) &&
        ((unitData->entryDeLimit + _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt)) > entry))  {
        /* entry is from the direct extraction range */
        ent = entry - _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt);
        result = _bcm_dpp_field_dir_ext_action_add(unitData,
                                                   ent,
                                                   &action,
                                                   count,
                                                   extractions);
    } else {
        result = BCM_E_NOT_FOUND;
        LOG_ERROR(BSL_LS_BCM_FP,
                  (BSL_META_U(unit,
                              "unit %d entry %d is not valid"
                              " for this function\n"),
                   unit,
                   entry));
    }

    _DPP_FIELD_UNIT_UNLOCK(unitData);
    BCMDNX_IF_ERR_EXIT(result);
exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_field_direct_extraction_action_get(int unit,
                                             bcm_field_entry_t entry,
                                             bcm_field_extraction_action_t *action,
                                             int maxCount,
                                             bcm_field_extraction_field_t *extractions,
                                             int *actualCount)
{
    _DPP_FIELD_COMMON_LOCALS;
    _bcm_dpp_field_ent_idx_t ent;
    BCMDNX_INIT_FUNC_DEFS;
#if (defined(BCM_DPP_SUPPORT) && defined(BCM_WARM_BOOT_SUPPORT))
    _DCMN_BCM_WARM_BOOT_API_TEST_RETRACT_OVERRIDEN_WB_TEST_MODE(unit);
#endif /* (defined(BCM_DPP_SUPPORT) && defined(BCM_WARM_BOOT_SUPPORT)) */

    _DPP_FIELD_UNIT_CHECK(unit, unitData);
    _DPP_FIELD_UNIT_LOCK(unitData);

    if ((_BCM_DPP_FIELD_ENT_BIAS(unit, DirExt) <= entry) &&
        ((unitData->entryDeLimit + _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt)) > entry))  {
        /* entry is from the direct extraction range */
        ent = entry - _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt);
        result = _bcm_dpp_field_dir_ext_action_get(unitData,
                                                   ent,
                                                   action,
                                                   maxCount,
                                                   extractions,
                                                   actualCount);
    } else { /* if (entry is valid direct extraction entry) */
        result = BCM_E_NOT_FOUND;
        LOG_ERROR(BSL_LS_BCM_FP,
                  (BSL_META_U(unit,
                              "unit %d entry %d is not valid"
                              " for this function\n"),
                   unit,
                   entry));
    } /* if (entry is valid direct extraction entry) */

    _DPP_FIELD_UNIT_UNLOCK(unitData);
    BCMDNX_IF_ERR_EXIT(result);
exit:
    BCMDNX_FUNC_RETURN;
}


