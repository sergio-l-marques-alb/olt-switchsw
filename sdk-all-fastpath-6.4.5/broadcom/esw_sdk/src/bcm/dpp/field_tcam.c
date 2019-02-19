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
 * Module: Field Processor APIs
 *
 * Purpose:
 *     'Field Processor' (FP) API for Dune Packet Processor devices
 *     TCAM suppot
 */

/*
 *  The functions in this module deal with TCAM feature on PPD devices. They
 *  are invoked by the main field module when certain things occur, such as
 *  calls that explicitly involve TCAM work.
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
#include <bcm/error.h>
#include <bcm/field.h>
#include <bcm/stack.h>

#include <bcm_int/petra_dispatch.h>
#include <bcm_int/dpp/field_int.h>
#include <bcm_int/dpp/error.h>
#include <bcm_int/dpp/alloc_mngr.h>
#include <bcm_int/dpp/mirror.h>
#include <bcm_int/dpp/cosq.h>
#include <bcm_int/dpp/policer.h>
#include <bcm_int/dpp/vlan.h>
#include <bcm_int/dpp/qos.h>
#include <bcm_int/dpp/l3.h>
#include <bcm_int/dpp/gport_mgmt.h>
#include <bcm_int/dpp/rx.h>
#include <bcm_int/common/multicast.h>
#ifdef BCM_WARM_BOOT_SUPPORT
#include <bcm_int/dpp/wb_db_field.h>
#endif /* def BCM_WARM_BOOT_SUPPORT */
#ifdef BCM_PETRAB_SUPPORT
#include <soc/dpp/Petra/PB_PP/pb_pp_sw_db.h>
#endif /* def BCM_PETRAB_SUPPORT */
#ifdef BCM_ARAD_SUPPORT
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_fp_key.h>
#include <soc/dpp/ARAD/arad_sw_db.h>
#endif /* BCM_ARAD_SUPPORT */
#include <soc/dpp/dpp_config_defs.h>
#include <soc/dpp/PPC/ppc_api_metering.h>
#include <soc/dpp/TMC/tmc_api_pmf_low_level_pgm.h>
/*
 *  For now, if _BCM_DPP_FIELD_WARM_BOOT_SUPPORT is FALSE, disable warm boot
 *  support here.
 */
#ifdef BCM_WARM_BOOT_SUPPORT
#if !_BCM_DPP_FIELD_WARM_BOOT_SUPPORT
#undef BCM_WARM_BOOT_SUPPORT
#endif  /* BCM_WARM_BOOT_SUPPORT */
#endif /* !_BCM_DPP_FIELD_WARM_BOOT_SUPPORT */

/* 
 * In Arad, the counting is done via the ACE-Pointer table. 
 * The final counter-id = ACE-Pointer (line id) 
 * For lines < 1K, both PP-Ports and Out-LIFs are changed. These lines are not used 
 * in current implementations, thus the Counter-ID must be superior to 1K 
 * The last line is invalid. 
 * The Driver reserves the last lines for redirection when not counting.
 */
#define _BCM_PETRA_CTR_BASE_VALUE_EGRESS_ARAD 1024
#define _BCM_PETRA_CTR_MAX_VALUE_EGRESS_ARAD  (4096 - 256 - 1)  /* table size minus static lines for Redirect */

int _bcm_dpp_field_tcam_entry_build_qualifiers_from_sw_state(_bcm_dpp_field_info_t *unitData, SOC_PPD_FP_ENTRY_INFO *entryInfo, _bcm_dpp_field_ent_idx_t entry, uint8 isExternalTcam, unsigned int entryMaxQuals, _bcm_dpp_field_stage_idx_t stage);
int _bcm_dpp_field_tcam_entry_remove_by_key(_bcm_dpp_field_info_t *unitData, uint8 isExternalTcam, _bcm_dpp_field_ent_idx_t entry);
int _bcm_dpp_field_tcam_entry_action_get_by_key(_bcm_dpp_field_info_t *unitData, uint8 isExternalTcam, _bcm_dpp_field_ent_idx_t entry,
                                                 bcm_field_action_t type,uint32 *param0,uint32 *param1);

/******************************************************************************
 *
 *  Local functions and data
 */


/*
 *  Function
 *     _bcm_dpp_field_tcam_entry_hardware_remove
 *  Purpose
 *     Remove a soc_petra PPD entry based upon the BCM field entry information
 *  Parameters
 *     (in) unitData = unit information
 *     (in) entry = entry ID
 *     (in) backingStore = TRUE to update backing store, FALSE otherwise
 *  Returns
 *     int (implied cast from bcm_error_t)
 *        BCM_E_NONE if successful
 *        BCM_E_* appropriately if not
 *  Notes
 *     Assumes entry validity checks have already been performed.
 *
 *     If swapping new entry in, this must be called after the new entry is
 *     installed but before the entry descriptor has been updated to reflect
 *     the new entry information.
 *
 *     Will complete removal process and return last error if there is one.
 */
STATIC int
_bcm_dpp_field_tcam_entry_hardware_remove(_bcm_dpp_field_info_t *unitData,
                                          uint8 isExternalTcam,
                                          _bcm_dpp_field_ent_idx_t entry,
                                          int backingStore)
{
    _bcm_dpp_field_group_t *groupData;
    uint32 ppdr;
    int result = BCM_E_NONE;
    int auxRes = BCM_E_NONE;
    uint32 *entryFlags;
    
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;
    
    groupData = &(unitData->groupD[_BCM_DPP_FIELD_TCAM_ENTRY_GROUP(isExternalTcam, entry)]);
    
    if (_BCM_DPP_FIELD_TCAM_ENTRY_FLAGS(isExternalTcam, entry) & _BCM_DPP_FIELD_ENTRY_IN_HW) 
    {
        LOG_DEBUG(BSL_LS_BCM_FP,
                  (BSL_META_U(unit,
                              "unit %d entry %u remvoving"
                              " hardware entry %u\n"),
                   unit,
                   _BCM_DPP_FIELD_TCAM_GLOBAL_ID(unit, entry, isExternalTcam),
                   _BCM_DPP_FIELD_TCAM_ENTRY_HW_HANDLE(isExternalTcam, entry)));
            ppdr = soc_ppd_fp_entry_remove(unitData->unitHandle,
                                           groupData->hwHandle,
                                           _BCM_DPP_FIELD_TCAM_ENTRY_HW_HANDLE(isExternalTcam, entry));
            auxRes = handle_sand_result(ppdr);
            if (BCM_E_NONE != auxRes) {
                result = auxRes;
                LOG_ERROR(BSL_LS_BCM_FP,
                          (BSL_META_U(unit,
                                      "unable to remove unit %d group %u entry %u"
                                      " (hw entry %u) from hardware: %d (%s)\n"),
                           unit,
                           _BCM_DPP_FIELD_TCAM_ENTRY_GROUP(isExternalTcam, entry),
                           _BCM_DPP_FIELD_TCAM_GLOBAL_ID(unit, entry, isExternalTcam),
                           _BCM_DPP_FIELD_TCAM_ENTRY_HW_HANDLE(isExternalTcam, entry),
                           result,
                           _SHR_ERRMSG(result)));
            }
     
        auxRes = shr_mres_free(unitData->hwResIds,
                               unitData->stageD[groupData->stage].entryRes,
                               1 /* count */,
                               _BCM_DPP_FIELD_TCAM_ENTRY_HW_HANDLE(isExternalTcam, entry));
        if (BCM_E_NONE != auxRes) {
            result = auxRes;
            LOG_ERROR(BSL_LS_BCM_FP,
                      (BSL_META_U(unit,
                                  "unit %d unable to return hardware entry"
                                  " handle %u to free list: %d (%s)\n"),
                       unit,
                       _BCM_DPP_FIELD_TCAM_ENTRY_HW_HANDLE(isExternalTcam, entry),
                       result,
                       _SHR_ERRMSG(result)));
        }

        if (_BCM_DPP_FIELD_TCAM_ENTRY_FLAGS(isExternalTcam, entry) & _BCM_DPP_FIELD_ENTRY_EGR_PRO) {
            auxRes = _bcm_dpp_am_template_trap_egress_free(unit,
                                                           unitData->entryTc[entry].egrTrapProfile);
        }
        if (BCM_E_NONE != auxRes) {
            result = auxRes;
            LOG_ERROR(BSL_LS_BCM_FP,
                      (BSL_META_U(unit,
                                  "unit %d unable to return hardware egress"
                                  " trap profile %d to free list: %d (%s)\n"),
                       unit,
                       unitData->entryTc[entry].egrTrapProfile,
                       result,
                       _SHR_ERRMSG(result)));
        }

        /* this entry is no longer in hardware */
        entryFlags = _BCM_DPP_FIELD_TCAM_ENTRY_FLAGS_PTR(isExternalTcam, entry);
        *entryFlags &= (~(_BCM_DPP_FIELD_ENTRY_IN_HW |
                                             _BCM_DPP_FIELD_ENTRY_EGR_PRO));

#ifdef BCM_WARM_BOOT_SUPPORT
        if (backingStore) {
            /* update backing store */
            _bcm_dpp_field_tcam_entry_wb_save(unitData, entry, isExternalTcam, NULL, NULL);
        }
#endif /* def BCM_WARM_BOOT_SUPPORT */
    } /* if (entryData->entryCmn.entryFlags & _BCM_DPP_FIELD_ENTRY_IN_HW) */
    BCMDNX_IF_ERR_EXIT(result);
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *     _bcm_dpp_field_tcam_entry_hardware_install
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
_bcm_dpp_field_tcam_entry_hardware_install(_bcm_dpp_field_info_t *unitData,
                                           uint8 isExternalTcam,
                                           _bcm_dpp_field_ent_idx_t entry)
{
    SOC_PPD_TRAP_EG_ACTION_PROFILE_INFO egTrapInfo;
    SOC_PPD_FP_ENTRY_INFO *entryInfo = NULL;
    _bcm_dpp_field_group_t *groupData;
    _bcm_dpp_field_stage_t *stageData;
    uint32 ppdr;
    uint32 newFlags;
    int egrTrapProfile = -1;
    int newHwHandle;
    SOC_SAND_SUCCESS_FAILURE success;
    unsigned int index;
    unsigned int offset;
    int result = BCM_E_NONE;
    int auxRes;
    uint32 *entryFlags;
    uint32 *hwPriority;
    uint32 *hwHandle;
     int      core=0; 

    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;
    
    groupData = &(unitData->groupD[_BCM_DPP_FIELD_TCAM_ENTRY_GROUP(isExternalTcam, entry)]);
    stageData = &(unitData->stageD[groupData->stage]);

    /* may need to deal with action profiles, so prepare for it */
    SOC_PPD_TRAP_EG_ACTION_PROFILE_INFO_clear(&egTrapInfo);

    /* keep old flags and handle around */
    newFlags = _BCM_DPP_FIELD_TCAM_ENTRY_FLAGS(isExternalTcam, entry);

    /* pick a new hardware entry handle */
    result = shr_mres_alloc(unitData->hwResIds,
                            unitData->stageD[groupData->stage].entryRes,
                            0 /* flags */,
                            1 /* count */,
                            &newHwHandle);
    if (BCM_E_NONE != result) {
        BCMDNX_IF_ERR_EXIT_MSG(result,
                            (_BSL_BCM_MSG_NO_UNIT("unit %d unable to allocate"
                                              " hardware handle for entry %u:"
                                              " %d (%s)"),
                             unit,
                             _BCM_DPP_FIELD_TCAM_GLOBAL_ID(unit, entry, isExternalTcam),
                             result,
                             _SHR_ERRMSG(result)));
    }

    BCMDNX_ALLOC(entryInfo, sizeof(SOC_PPD_FP_ENTRY_INFO), "_bcm_dpp_field_tcam_entry_hardware_install.entryInfo");
    if(entryInfo == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG_NO_UNIT("Memory allocation failure")));
    }
    /* build entry qualifier data from sw state */
    SOC_PPD_FP_ENTRY_INFO_clear(entryInfo);

    if (SOC_IS_ARAD(unit)) {
        entryInfo->priority = _BCM_DPP_FIELD_TCAM_ENTRY_HW_PRIORITY(isExternalTcam, entry);
    }
    else if (SOC_IS_PETRAB(unit)) {
        entryInfo->priority = _BCM_DPP_FIELD_TCAM_ENTRY_HW_PRIORITY(isExternalTcam, entry) ^ 1;
    }
    else {
        /* If Unit is neither Petra-B nor ARAD */
        BCM_ERR_EXIT_NO_MSG(BCM_E_NOT_FOUND);
    }

    result = _bcm_dpp_field_tcam_entry_build_qualifiers_from_sw_state(unitData, entryInfo, entry, isExternalTcam, stageData->devInfo->entryMaxQuals, groupData->stage);

    /* build entry action data from sw state */
    for (index = 0, offset = 0;
         index < SOC_PPD_FP_NOF_ACTIONS_PER_DB_MAX;
         index++) {
        if (_BCM_DPP_FIELD_PPD_ACTION_VALID(_BCM_DPP_FIELD_TCAM_ENTRY_ACTP(isExternalTcam, entry)[index].hwType)) {
            LOG_DEBUG(BSL_LS_BCM_FP,
                      (BSL_META_U(unit,
                                  "unit %d entry %u PPD action %d %s(%d)"
                                  " %08X %08X\n"),
                       unit,
                       _BCM_DPP_FIELD_TCAM_GLOBAL_ID(unit, entry, isExternalTcam),
                       index,
                       SOC_PPD_FP_ACTION_TYPE_to_string(_BCM_DPP_FIELD_TCAM_ENTRY_ACTP(isExternalTcam, entry)[index].hwType),
                       _BCM_DPP_FIELD_TCAM_ENTRY_ACTP(isExternalTcam, entry)[index].hwType,
                       _BCM_DPP_FIELD_TCAM_ENTRY_ACTP(isExternalTcam, entry)[index].hwFlags,
                       _BCM_DPP_FIELD_TCAM_ENTRY_ACTP(isExternalTcam, entry)[index].hwParam));
            switch (_BCM_DPP_FIELD_TCAM_ENTRY_ACTP(isExternalTcam, entry)[index].hwType) {
            case SOC_PPD_FP_ACTION_TYPE_EGR_TRAP:
                egTrapInfo.bitmap_mask |= _BCM_DPP_FIELD_TCAM_ENTRY_ACTP(isExternalTcam, entry)[index].hwFlags;
                if (_BCM_DPP_FIELD_TCAM_ENTRY_ACTP(isExternalTcam, entry)[index].hwFlags &
                    SOC_PPD_TRAP_ACTION_PROFILE_OVERWRITE_DEST) {
#ifdef BCM_ARAD_SUPPORT
                    if (SOC_IS_ARAD(unit) 
                        /* Avoid Drop action since its destination is not regular (discard) */
                        && (_BCM_DPP_FIELD_TCAM_ENTRY_ACTP(isExternalTcam, entry)[index].hwParam != SOC_PPD_TRAP_ACTION_PKT_DISCARD_ID)) {
                        int rv;
                        rv = soc_port_sw_db_tm_port_to_base_q_pair_get(
                                                    unitData->unitHandle,
													core,
                                                    _BCM_DPP_FIELD_TCAM_ENTRY_ACTP(isExternalTcam, entry)[index].hwParam,
                                                    &egTrapInfo.out_tm_port
                                                 );
                        if (BCM_FAILURE(rv)) { /* function returns valid port number or error ID */
                            BCMDNX_ERR_EXIT_MSG(_BCM_DPP_FIELD_TCAM_ENTRY_ACTP(isExternalTcam, entry)[index].hwParam,
                                             (_BSL_BCM_MSG_NO_UNIT("unable to map port %d (%08X) to base q pair"),
                                              _BCM_DPP_FIELD_TCAM_ENTRY_ACTP(isExternalTcam, entry)[index].hwParam,
                                              _BCM_DPP_FIELD_TCAM_ENTRY_ACTP(isExternalTcam, entry)[index].hwParam));
                        }
                        egTrapInfo.header_data.dsp_ptr = _BCM_DPP_FIELD_TCAM_ENTRY_ACTP(isExternalTcam, entry)[index].hwParam;
                    }
                    else 
#endif /* BCM_ARAD_SUPPORT */
                    {
                        egTrapInfo.out_tm_port = _BCM_DPP_FIELD_TCAM_ENTRY_ACTP(isExternalTcam, entry)[index].hwParam;
                    }
                }
                if (_BCM_DPP_FIELD_TCAM_ENTRY_ACTP(isExternalTcam, entry)[index].hwFlags &
                    SOC_PPD_TRAP_ACTION_PROFILE_OVERWRITE_DP) {
                    egTrapInfo.cos_info.dp = _BCM_DPP_FIELD_TCAM_ENTRY_ACTP(isExternalTcam, entry)[index].hwParam;
                }
                if (_BCM_DPP_FIELD_TCAM_ENTRY_ACTP(isExternalTcam, entry)[index].hwFlags &
                    SOC_PPD_TRAP_ACTION_PROFILE_OVERWRITE_CUD) {
#ifdef BCM_ARAD_SUPPORT
                    if (SOC_IS_ARAD(unit)) {
                        egTrapInfo.header_data.cud = _BCM_DPP_FIELD_TCAM_ENTRY_ACTP(isExternalTcam, entry)[index].hwParam;
                    }
                    else 
#endif /* BCM_ARAD_SUPPORT */
                    {
                        egTrapInfo.cud = _BCM_DPP_FIELD_TCAM_ENTRY_ACTP(isExternalTcam, entry)[index].hwParam;
                    }
                }
                if (_BCM_DPP_FIELD_TCAM_ENTRY_ACTP(isExternalTcam, entry)[index].hwFlags &
                    (~(SOC_PPD_TRAP_ACTION_PROFILE_OVERWRITE_DEST |
                       SOC_PPD_TRAP_ACTION_PROFILE_OVERWRITE_DP |
                       SOC_PPD_TRAP_ACTION_PROFILE_OVERWRITE_CUD))) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                                     (_BSL_BCM_MSG_NO_UNIT("unit %d entry %u"
                                                       " includes unknown"
                                                       " flags %08X on egress"
                                                       " trap action"),
                                      unit,
                                      _BCM_DPP_FIELD_TCAM_GLOBAL_ID(unit, entry, isExternalTcam),
                                      _BCM_DPP_FIELD_TCAM_ENTRY_ACTP(isExternalTcam, entry)[index].hwFlags &
                                      (~(SOC_PPD_TRAP_ACTION_PROFILE_OVERWRITE_DEST |
                                         SOC_PPD_TRAP_ACTION_PROFILE_OVERWRITE_DP |
                                         SOC_PPD_TRAP_ACTION_PROFILE_OVERWRITE_CUD))));
                }
                break;
            default:
                /* for most actions, the flags are ignored */
                entryInfo->actions[offset].type = _BCM_DPP_FIELD_TCAM_ENTRY_ACTP(isExternalTcam, entry)[index].hwType;
                entryInfo->actions[offset].val = _BCM_DPP_FIELD_TCAM_ENTRY_ACTP(isExternalTcam, entry)[index].hwParam;
                offset++;
            }
        } /* if (SOC_PPD_NOF_FP_ACTION_TYPES > entryData->tcActP[index].hwType) */
    } /* for (all actions) */
    if (offset > stageData->devInfo->entryMaxActs) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d stage %u group %u entry %u"
                                           " has more action slots in use"
                                           " %d than the stage supports %d"),
                          unit,
                          groupData->stage,
                          _BCM_DPP_FIELD_TCAM_ENTRY_GROUP(isExternalTcam, entry),
                          _BCM_DPP_FIELD_TCAM_GLOBAL_ID(unit, entry, isExternalTcam),
                          offset,
                          stageData->devInfo->entryMaxActs));
    }
    while (offset < SOC_PPD_FP_NOF_ACTIONS_PER_DB_MAX) {
        entryInfo->actions[offset].type = SOC_PPD_FP_ACTION_TYPE_INVALID;
        entryInfo->actions[offset].val = 0;
        offset++;
    }
    /* WARNING: from this point, assume new hardware entry owns resources */
    if (egTrapInfo.bitmap_mask) {
        /*
         *  At least one action uses an egress trap based feature; need to
         *  allocate a trap profile.  We do not 'exchange' it here because we
         *  do not want to affect the existing entry in case of error, so what
         *  we do is grab a new template here and then dispose of the old
         *  template later once the old entry has been removed from hardware.
         */
        result = _bcm_dpp_am_template_trap_egress_allocate(unit,
                                                           0 /* flags */,
                                                           &egTrapInfo,
                                                           &egrTrapProfile);
        if (BCM_E_NONE == result) {
            /* indicate egress trap profile is now in use */
            newFlags |= _BCM_DPP_FIELD_ENTRY_EGR_PRO;
            /* add egress trap action to the hardware entry */
            for (index = 0;
                 index < stageData->devInfo->entryMaxActs;
                 index++) {
                if (!_BCM_DPP_FIELD_PPD_ACTION_VALID(entryInfo->actions[index].type)) {
                    /* this slot is open; use it */
                    LOG_DEBUG(BSL_LS_BCM_FP,
                              (BSL_META_U(unit,
                                          "unit %d entry %u placing egress"
                                          " trap profile %d in slot %u\n"),
                               unit,
                               _BCM_DPP_FIELD_TCAM_GLOBAL_ID(unit, entry, isExternalTcam),
                               egrTrapProfile,
                               index));
                    entryInfo->actions[index].type = SOC_PPD_FP_ACTION_TYPE_EGR_TRAP;
#ifdef BCM_ARAD_SUPPORT
                    if (SOC_IS_ARAD(unit)) {
                        entryInfo->actions[index].val = (egrTrapProfile << 1) | 1;
                    } else
#endif /* def BCM_ARAD_SUPPORT */
#ifdef BCM_PETRAB_SUPPORT
                    if (SOC_IS_PETRAB(unit)) {
                        entryInfo->actions[index].val = egrTrapProfile;
                    } else
#endif /* def BCM_PETRAB_SUPPORT */
                    {
                        result = BCM_E_INTERNAL;
                        LOG_ERROR(BSL_LS_BCM_FP,
                                  (BSL_META_U(unit,
                                              "unit %d unable to determine"
                                              " device type for egress trap\n"),
                                   unit));
                    }
                    break;
                } /* the action in this slot is not yet used */
            } /* for (all possible hardware action slots) */
            if (stageData->devInfo->entryMaxActs <= index) {
                result = BCM_E_CONFIG;
                LOG_ERROR(BSL_LS_BCM_FP,
                          (BSL_META_U(unit,
                                      "unit %d entry %u has too many hardware"
                                      " actions to add egress trap action\n"),
                           unit,
                           _BCM_DPP_FIELD_TCAM_GLOBAL_ID(unit, entry, isExternalTcam)));
            }
        } else { /* if (BCM_E_NONE == result) */
            LOG_ERROR(BSL_LS_BCM_FP,
                      (BSL_META_U(unit,
                                  "unit %d entry %u unable to allocate egress"
                                  " trap template: %d (%s)\n"),
                       unit,
                       _BCM_DPP_FIELD_TCAM_GLOBAL_ID(unit, entry, isExternalTcam),
                       result,
                       _SHR_ERRMSG(result)));
        } /* if (BCM_E_NONE == result) */
    } else { /* if (egTrapInfo.bitmap_mask) */
        newFlags &= (~_BCM_DPP_FIELD_ENTRY_EGR_PRO);
    } /* if (egTrapInfo.bitmap_mask) */

    if (BCM_E_NONE == result) {
        /* add the entry to hardware */
        LOG_DEBUG(BSL_LS_BCM_FP,
                  (BSL_META_U(unit,
                              "unit %d entry %u inserting"
                              " hardware entry %u\n"),
                   unit,
                   _BCM_DPP_FIELD_TCAM_GLOBAL_ID(unit, entry, isExternalTcam),
                   newHwHandle));
        if (SOC_IS_ARAD(unit)) 
        {
            entryInfo->is_for_update = 0;
            entryInfo->is_invalid = 0;
            entryInfo->is_inserted_top = (_BCM_DPP_FIELD_TCAM_ENTRY_FLAGS(isExternalTcam, entry) & _BCM_DPP_FIELD_GROUP_INSTALL) ? 1 : 0;
        }
        ppdr = soc_ppd_fp_entry_add(unitData->unitHandle,
                                    groupData->hwHandle,
                                    newHwHandle,
                                    entryInfo,
                                    &success);
        auxRes = handle_sand_result(ppdr);
        if (BCM_E_NONE == auxRes) {
            result = translate_sand_success_failure(success);
        } else {
            result = auxRes;
        }
        if (BCM_E_NONE == result) {
            /* entry installed */
            
             result = _bcm_dpp_field_tcam_entry_hardware_remove(unitData,
                                                                isExternalTcam,
                                                                entry,
                                                                FALSE);

            if (SOC_IS_PETRAB(unit)) {
                hwPriority = _BCM_DPP_FIELD_TCAM_ENTRY_HW_PRIORITY_PTR(isExternalTcam, entry);
                /* update the entry's hardware priority */
                *hwPriority ^= 1;
            }
            /* keep the new hardware entry's handle */
            hwHandle = _BCM_DPP_FIELD_TCAM_ENTRY_HW_HANDLE_PTR(isExternalTcam, entry);
            *hwHandle = newHwHandle;
            if(!isExternalTcam) {
                /* keep the new hardware entry's egress trap profile */
                unitData->entryTc[entry].egrTrapProfile = egrTrapProfile;
            }
            /* some things may have affected the flags */
            entryFlags = _BCM_DPP_FIELD_TCAM_ENTRY_FLAGS_PTR(isExternalTcam, entry);
            *entryFlags = newFlags;
            /* entry is now in hardware */
            if (!(groupData->groupFlags & _BCM_DPP_FIELD_GROUP_HANDLE_ENTRIES_BY_KEY)) {
                *entryFlags |= _BCM_DPP_FIELD_ENTRY_IN_HW; 
            }
            /* entry is no longer changed since insert & no need to update */
            *entryFlags &= (~(_BCM_DPP_FIELD_ENTRY_CHANGED |
                                                 _BCM_DPP_FIELD_ENTRY_UPDATE));
            if (SOC_IS_ARAD(unit)) {
                /* entry is not new anymore */
                *entryFlags &= (~_BCM_DPP_FIELD_ENTRY_NEW);
                /* Clean group install flag */
                *entryFlags &= (~_BCM_DPP_FIELD_GROUP_INSTALL);
            }
#ifdef BCM_WARM_BOOT_SUPPORT
            /* update backing store */
            _bcm_dpp_field_tcam_entry_wb_save(unitData, entry, isExternalTcam, NULL, NULL);
#endif /* def BCM_WARM_BOOT_SUPPORT */
        } else { /* if (BCM_E_NONE == result) */
            LOG_ERROR(BSL_LS_BCM_FP,
                      (BSL_META_U(unit,
                                  "unable to add unit %d group %u entry %u"
                                  " to hardware: %d (%s)\n"),
                       unit,
                       _BCM_DPP_FIELD_TCAM_ENTRY_GROUP(isExternalTcam, entry),
                       _BCM_DPP_FIELD_TCAM_GLOBAL_ID(unit, entry, isExternalTcam),
                       result,
                       _SHR_ERRMSG(result)));
            /* clean up resources for failed new entry */
            auxRes = _bcm_dpp_am_template_trap_egress_free(unit,
                                                           egrTrapProfile);
        } /* if (BCM_E_NONE == result) */
    } /* if (BCM_E_NONE == result) */

    BCMDNX_IF_ERR_EXIT(result);
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
_bcm_dpp_field_tcam_entry_dump(_bcm_dpp_field_info_t *unitData,
                               uint8 isExternalTcam,
                               _bcm_dpp_field_ent_idx_t entry,
                               const char *prefix)
{
    bcm_field_aset_t actions;
    unsigned int index;
    unsigned int count;
    int result = BCM_E_NONE;
    int dumpedRanges;
#if _BCM_DPP_FIELD_DUMP_VERIFY_PPD
    uint32 soc_sandResult;
    uint8 okay;
    SOC_PPD_FP_ENTRY_INFO entInfoTc;
    SOC_PPD_FP_QUAL_VAL *hwQuals;
#endif /* _BCM_DPP_FIELD_DUMP_VERIFY_PPD */

    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;
    
    sal_memset(&(actions), 0x00, sizeof(actions));
#if _BCM_DPP_FIELD_DUMP_VERIFY_PPD
    if ((_BCM_DPP_FIELD_TCAM_ENTRY_FLAGS(isExternalTcam, entry) & (_BCM_DPP_FIELD_ENTRY_IN_HW |
                                           _BCM_DPP_FIELD_ENTRY_CHANGED |
                                           _BCM_DPP_FIELD_ENTRY_NEW)) ==
        (_BCM_DPP_FIELD_ENTRY_IN_HW)) {
        /* entry is in hardware and not changed since */
        SOC_PPD_FP_ENTRY_INFO_clear(&entInfoTc);
        soc_sandResult = soc_ppd_fp_entry_get(unitData->unitHandle,
                                              unitData->groupD[_BCM_DPP_FIELD_TCAM_ENTRY_GROUP(isExternalTcam, entry)].hwHandle,
                                              _BCM_DPP_FIELD_TCAM_ENTRY_HW_HANDLE(isExternalTcam, entry),
                                              &okay,
                                              &entInfoTc);
        result = handle_sand_result(soc_sandResult);
        BCMDNX_IF_ERR_EXIT_MSG(result,
                            (_BSL_BCM_MSG_NO_UNIT("unit %d unable to fetch entry %u"
                                              " hardware information (%u):"
                                              " %d (%s)"),
                             unit,
                             entry,
                             _BCM_DPP_FIELD_TCAM_ENTRY_HW_HANDLE(isExternalTcam, entry),
                             result,
                             _SHR_ERRMSG(result)));
        if (!okay) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d entry %u not okay trying"
                                               " to fetch entry from hardware"),
                              unit,
                              entry));
        }
        hwQuals = &(entInfoTc.qual_vals[0]);
    } else {
        /* entry is not in hardare or has changed */
        hwQuals = NULL;
    }
#endif /* _BCM_DPP_FIELD_DUMP_VERIFY_PPD */
    FIELD_PRINT(("%sEntry "
                 _BCM_DPP_FIELD_ENTRY_IDX_FORMAT
                 "\n",
                 prefix,
                 _BCM_DPP_FIELD_TCAM_GLOBAL_ID(unit, entry, isExternalTcam)));
    FIELD_PRINT(("%s  Flags       = %08X\n", prefix, _BCM_DPP_FIELD_TCAM_ENTRY_FLAGS(isExternalTcam, entry)));
#if _BCM_DPP_FIELD_DUMP_SYM_FLAGS
    FIELD_PRINT(("%s    %s %s %s %s %s %s %s\n",
                 prefix,
                 (_BCM_DPP_FIELD_TCAM_ENTRY_FLAGS(isExternalTcam, entry) & _BCM_DPP_FIELD_ENTRY_IN_USE)?"+USED":"-used",
                 (_BCM_DPP_FIELD_TCAM_ENTRY_FLAGS(isExternalTcam, entry) & _BCM_DPP_FIELD_ENTRY_IN_HW)?"+IN_HW":"-in_hw",
                 (_BCM_DPP_FIELD_TCAM_ENTRY_FLAGS(isExternalTcam, entry) & _BCM_DPP_FIELD_ENTRY_WANT_HW)?"+WANT_HW":"-want_hw",
                 (_BCM_DPP_FIELD_TCAM_ENTRY_FLAGS(isExternalTcam, entry) & _BCM_DPP_FIELD_ENTRY_UPDATE)?"+UPD":"-upd",
                 (_BCM_DPP_FIELD_TCAM_ENTRY_FLAGS(isExternalTcam, entry) & _BCM_DPP_FIELD_ENTRY_CHANGED)?"+CHG":"-chg",
                 (_BCM_DPP_FIELD_TCAM_ENTRY_FLAGS(isExternalTcam, entry) & _BCM_DPP_FIELD_ENTRY_NEW)?"+NEW":"-new",
                 (_BCM_DPP_FIELD_TCAM_ENTRY_FLAGS(isExternalTcam, entry) & _BCM_DPP_FIELD_ENTRY_STATS)?"+STA":"-sta"));
#endif /* _BCM_DPP_FIELD_DUMP_SYM_FLAGS */
    if (_BCM_DPP_FIELD_TCAM_ENTRY_FLAGS(isExternalTcam, entry) & _BCM_DPP_FIELD_ENTRY_IN_USE) {
        if ((_BCM_DPP_FIELD_ENTRY_IN_HW | _BCM_DPP_FIELD_ENTRY_CHANGED) ==
            (_BCM_DPP_FIELD_TCAM_ENTRY_FLAGS(isExternalTcam, entry) & (_BCM_DPP_FIELD_ENTRY_IN_HW |
                                 _BCM_DPP_FIELD_ENTRY_CHANGED))) {
            LOG_CLI((BSL_META_U(unit,
                                "%s    (entry changed since hw write)\n"), prefix));
        }
        LOG_CLI((BSL_META_U(unit,
                            "%s  Group       = "
                 _BCM_DPP_FIELD_GROUP_IDX_FORMAT
                            "\n"),
                 prefix,
                 _BCM_DPP_FIELD_TCAM_ENTRY_GROUP(isExternalTcam, entry)));
        LOG_CLI((BSL_META_U(unit,
                            "%s  Priority    = %8d\n"),
                 prefix,
                 _BCM_DPP_FIELD_TCAM_ENTRY_PRIORITY(isExternalTcam, entry)));
        LOG_CLI((BSL_META_U(unit,
                            "%s  Prev/Next   = "
                 _BCM_DPP_FIELD_ENTRY_IDX_FORMAT
                            " / "
                            _BCM_DPP_FIELD_ENTRY_IDX_FORMAT
                            "\n"),
                 prefix,
                 _BCM_DPP_FIELD_TCAM_GLOBAL_ID(unit, _BCM_DPP_FIELD_TCAM_ENTRY_PREV(isExternalTcam, entry), isExternalTcam),
                 _BCM_DPP_FIELD_TCAM_GLOBAL_ID(unit, _BCM_DPP_FIELD_TCAM_ENTRY_NEXT(isExternalTcam, entry), isExternalTcam)));
        if (_BCM_DPP_FIELD_TCAM_ENTRY_FLAGS(isExternalTcam, entry) & _BCM_DPP_FIELD_ENTRY_IN_HW) {
            LOG_CLI((BSL_META_U(unit,
                                "%s  HW entry ID = %08X\n"),
                     prefix,
                     _BCM_DPP_FIELD_TCAM_ENTRY_HW_HANDLE(isExternalTcam, entry)));
            LOG_CLI((BSL_META_U(unit,
                                "%s  HW priority = %08X\n"),
                     prefix,
                     _BCM_DPP_FIELD_TCAM_ENTRY_HW_PRIORITY(isExternalTcam, entry)));
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
                                                    unitData->groupD[_BCM_DPP_FIELD_TCAM_ENTRY_GROUP(isExternalTcam, entry)].stage,
                                                    _BCM_DPP_FIELD_TCAM_ENTRY_QUALS(isExternalTcam, entry),
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
#if _BCM_DPP_FIELD_DUMP_INTERMIX_PPD
        LOG_CLI((BSL_META_U(unit,
                            "%s  BCM actions:\n"), prefix));
#else /* _BCM_DPP_FIELD_DUMP_INTERMIX_PPD */
        LOG_CLI((BSL_META_U(unit,
                            "%s  Actions:\n"), prefix));
#endif /* _BCM_DPP_FIELD_DUMP_INTERMIX_PPD */
        count = 0;
        for (index = 0; index < _BCM_DPP_NOF_BCM_ACTIONS_PER_DB_MAX; index++) {
            if (bcmFieldActionCount > _BCM_DPP_FIELD_TCAM_ENTRY_ACTB(isExternalTcam, entry)[index].bcmType) {
                /* only show action if exists */
                if (!BCM_FIELD_ASET_TEST(actions, _BCM_DPP_FIELD_TCAM_ENTRY_ACTB(isExternalTcam, entry)[index].bcmType)) {
                    /* have not displayed this action yet */
                    switch (_BCM_DPP_FIELD_TCAM_ENTRY_ACTB(isExternalTcam, entry)[index].bcmType) {
                    /* add any actions that need special formatting */
                    default:
                        LOG_CLI((BSL_META_U(unit,
                                            "%s    %s (%d)\n"),
                                 prefix,
                                 _bcm_dpp_field_action_name[_BCM_DPP_FIELD_TCAM_ENTRY_ACTB(isExternalTcam, entry)[index].bcmType],
                                 _BCM_DPP_FIELD_TCAM_ENTRY_ACTB(isExternalTcam, entry)[index].bcmType));
                        LOG_CLI((BSL_META_U(unit,
                                            "%s      %08X,%08X\n"),
                                 prefix,
                                 _BCM_DPP_FIELD_TCAM_ENTRY_ACTB(isExternalTcam, entry)[index].bcmParam0,
                                 _BCM_DPP_FIELD_TCAM_ENTRY_ACTB(isExternalTcam, entry)[index].bcmParam1));
                        count++;
                        break;
                    } /* switch (entryData->qual[index].type) */
                } /* if (not encountered action yet) */
                BCM_FIELD_ASET_ADD(actions, _BCM_DPP_FIELD_TCAM_ENTRY_ACTB(isExternalTcam, entry)[index].bcmType);
            } /* if (action exists) */
        } /* for (index = 0; index < _BCM_DPP_NOF_BCM_ACTIONS_PER_DB_MAX; index++) */
        if (0 == count) {
            /* nothing in qualifiers */
            LOG_CLI((BSL_META_U(unit,
                                "%s    (none; a match will not affect traffic)\n"),
                     prefix));
        }
#if _BCM_DPP_FIELD_DUMP_INTERMIX_PPD
        LOG_CLI((BSL_META_U(unit,
                            "%s  PPD actions:\n"), prefix));
        count = 0;
        for (index = 0; index < SOC_PPD_FP_NOF_ACTIONS_PER_DB_MAX; index++) {
            if (_BCM_DPP_FIELD_PPD_ACTION_VALID(_BCM_DPP_FIELD_TCAM_ENTRY_ACTP(isExternalTcam, entry)[index].hwType)) {
                /* this action exists; display it */
                switch (_BCM_DPP_FIELD_TCAM_ENTRY_ACTP(isExternalTcam, entry)[index].hwType) {
                /* add any actions that need special formatting */
                case SOC_PPD_FP_ACTION_TYPE_EGR_TRAP:
                    LOG_CLI((BSL_META_U(unit,
                                        "%s    %s (%d)\n"),
                             prefix,
                             SOC_PPD_FP_ACTION_TYPE_to_string(_BCM_DPP_FIELD_TCAM_ENTRY_ACTP(isExternalTcam, entry)[index].hwType),
                             _BCM_DPP_FIELD_TCAM_ENTRY_ACTP(isExternalTcam, entry)[index].hwType));
                    LOG_CLI((BSL_META_U(unit,
                                        "%s      %08X: %08X\n"),
                             prefix,
                             _BCM_DPP_FIELD_TCAM_ENTRY_ACTP(isExternalTcam, entry)[index].hwFlags,
                             _BCM_DPP_FIELD_TCAM_ENTRY_ACTP(isExternalTcam, entry)[index].hwParam));
                    break;
                default:
                    LOG_CLI((BSL_META_U(unit,
                                        "%s    %s (%d)\n"),
                             prefix,
                             SOC_PPD_FP_ACTION_TYPE_to_string(_BCM_DPP_FIELD_TCAM_ENTRY_ACTP(isExternalTcam, entry)[index].hwType),
                             _BCM_DPP_FIELD_TCAM_ENTRY_ACTP(isExternalTcam, entry)[index].hwType));
                    LOG_CLI((BSL_META_U(unit,
                                        "%s      %08X\n"),
                             prefix,
                             _BCM_DPP_FIELD_TCAM_ENTRY_ACTP(isExternalTcam, entry)[index].hwParam));
                } /* swtich (entryData->tcActP[index].hwType) */
                count++;
            } /* if (action exists) */
        } /* for (index = 0; index < SOC_PPD_FP_NOF_ACTIONS_PER_DB_MAX; index++) */
        if (0 == count) {
            /* nothing in qualifiers */
            LOG_CLI((BSL_META_U(unit,
                                "%s    (none; a match will not affect traffic)\n"),
                     prefix));
        }
#endif /* _BCM_DPP_FIELD_DUMP_INTERMIX_PPD */
        BCM_EXIT;
    } else {/* if (entryData->flags & _BCM_DPP_FIELD_ENTRY_IN_USE) */
        LOG_CLI((BSL_META_U(unit,
                            "%s  (entry is not in use)\n"), prefix));
        BCM_ERR_EXIT_NO_MSG(BCM_E_NOT_FOUND);
    } /* if (entryData->flags & _BCM_DPP_FIELD_ENTRY_IN_USE) */
exit:
    BCMDNX_FUNC_RETURN;
}
#endif /* def BROADCOM_DEBUG */

int
_bcm_dpp_field_tcam_entry_install(_bcm_dpp_field_info_t *unitData,
                                  uint8 isExternalTcam,
                                  _bcm_dpp_field_ent_idx_t entry)
{
    int result = BCM_E_NONE;
    uint32 orig_flags;
    uint32 *entryFlags;
    _bcm_dpp_field_group_t *groupData;

    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;

    LOG_VERBOSE(BSL_LS_BCM_FP,
                (BSL_META("(%d,%d) enter\n"),
                 unit,
                 entry));

    if (_BCM_DPP_FIELD_TCAM_ENTRY_FLAGS(isExternalTcam, entry) & _BCM_DPP_FIELD_ENTRY_IN_USE) {        
        /* entry exists */
        orig_flags = _BCM_DPP_FIELD_TCAM_ENTRY_FLAGS(isExternalTcam, entry);
        entryFlags = _BCM_DPP_FIELD_TCAM_ENTRY_FLAGS_PTR(isExternalTcam, entry);
        *entryFlags |= (_BCM_DPP_FIELD_ENTRY_WANT_HW |
                                           _BCM_DPP_FIELD_ENTRY_UPDATE);
        if (SOC_IS_ARAD(unit)) {
            result = _bcm_dpp_field_tcam_entry_hardware_install(unitData, isExternalTcam, entry);
            if (result == BCM_E_NONE) {
                if (!(orig_flags & _BCM_DPP_FIELD_ENTRY_IN_HW)) {
                    /* if entry was not installed in HW, decrement uninstalled TCAM entries counter */
                    groupData = &(unitData->groupD[_BCM_DPP_FIELD_TCAM_ENTRY_GROUP(isExternalTcam, entry)]);

                     if (!(groupData->groupFlags & _BCM_DPP_FIELD_GROUP_HANDLE_ENTRIES_BY_KEY)) {
                         unitData->entryUninstalledTcCount--;
                     }
                }
                if (isExternalTcam  && !(SOC_DPP_CONFIG(unitData->unit)->pp.unbound_uninstalled_elk_entries_number)) {
                    /* clear external info for external TCAM */
                    _bcm_dpp_field_ext_info_entry_clear(&(unitData->extTcamInfo[unitData->entryExtTc[entry].ext_info_idx]));
                    soc_sand_occ_bm_occup_status_set(unitData->unit, unitData->extTcamBmp, unitData->entryExtTc[entry].ext_info_idx, FALSE);
                }
            }
        }
        else if (SOC_IS_PETRAB(unit)) {
            if (_BCM_DPP_FIELD_TCAM_ENTRY_FLAGS(isExternalTcam, entry) & _BCM_DPP_FIELD_ENTRY_NEW) {
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
                *entryFlags |= (_BCM_DPP_FIELD_ENTRY_WANT_HW |
                                                   _BCM_DPP_FIELD_ENTRY_UPDATE);
                result = _bcm_dpp_field_tcam_group_hw_refresh(unitData,
                                                              _BCM_DPP_FIELD_TCAM_ENTRY_GROUP(isExternalTcam, entry),
                                                              TRUE);
            } else { /* if (entryData->flags & _BCM_DPP_FIELD_ENTRY_NEW) */
                /* entry already has priority, so can just add to hardware */
                result = _bcm_dpp_field_tcam_entry_hardware_install(unitData,
                                                                    isExternalTcam,
                                                                    entry);
            } /* if (entryData->flags & _BCM_DPP_FIELD_ENTRY_NEW) */
        }
        else {
            /* If Unit is neither Petra-B nor ARAD */
            BCM_ERR_EXIT_NO_MSG(BCM_E_NOT_FOUND);
        }
    } else { /* if (entryData->flags & _BCM_DPP_FIELD_ENTRY_IN_USE) */
        LOG_ERROR(BSL_LS_BCM_FP,
                  (BSL_META_U(unit,
                              "unit %d entry %d is not in use\n"),
                   unit,
                   entry));
        result = BCM_E_NOT_FOUND;
    } /* if (entryData->flags & _BCM_DPP_FIELD_ENTRY_IN_USE) */

    LOG_VERBOSE(BSL_LS_BCM_FP,
                (BSL_META_U(unit,
                            "(%d,%d) return %d (%s)\n"),
                 unit,
                 entry,
                 result,
                 _SHR_ERRMSG(result)));
    BCMDNX_IF_ERR_EXIT(result);
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_field_tcam_entry_remove(_bcm_dpp_field_info_t *unitData,
                                 uint8 isExternalTcam,
                                 _bcm_dpp_field_ent_idx_t entry)
{
    int result = BCM_E_NONE;
    _bcm_dpp_field_group_t *groupData;
 
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;

    groupData = &(unitData->groupD[_BCM_DPP_FIELD_TCAM_ENTRY_GROUP(isExternalTcam, entry)]);

    if (groupData->groupFlags & _BCM_DPP_FIELD_GROUP_HANDLE_ENTRIES_BY_KEY) {
        result = _bcm_dpp_field_tcam_entry_remove_by_key(unitData, isExternalTcam, entry);
    }else {

        LOG_DEBUG(BSL_LS_BCM_FP,
                  (BSL_META_U(unit,
                              "(%d(%08X),%d) enter (%s,%s)\n"),
                   unit,
                   PTR_TO_INT(unitData),
                   entry,
                   (_BCM_DPP_FIELD_TCAM_ENTRY_FLAGS(isExternalTcam, entry) & _BCM_DPP_FIELD_ENTRY_IN_USE)?"+IN_USE":"-in_use",
                   (_BCM_DPP_FIELD_TCAM_ENTRY_FLAGS(isExternalTcam, entry) & _BCM_DPP_FIELD_ENTRY_IN_HW)?"+IN_HW":"-in_hw"));
        if (_BCM_DPP_FIELD_TCAM_ENTRY_FLAGS(isExternalTcam, entry) & _BCM_DPP_FIELD_ENTRY_IN_USE) {
            if ((_BCM_DPP_FIELD_TCAM_ENTRY_FLAGS(isExternalTcam, entry) & _BCM_DPP_FIELD_ENTRY_IN_HW)) {
                /* removing entry from HW. increment uninstalled TCAM entries counter */
                unitData->entryUninstalledTcCount++;
            }

            /* remove this entry from the PPD */
            result = _bcm_dpp_field_tcam_entry_hardware_remove(unitData, isExternalTcam, entry, TRUE);
        } else {
            LOG_ERROR(BSL_LS_BCM_FP,
                      (BSL_META_U(unit,
                                  "unit %d entry %d is not in use\n"),
                       unit,
                       entry));
            result = BCM_E_NOT_FOUND;
        }
        LOG_DEBUG(BSL_LS_BCM_FP,
                  (BSL_META_U(unit,
                              "(%d(%08X),%u) return %d (%s)\n"),
                   unit,
                   PTR_TO_INT(unitData),
                   entry,
                   result,
                   _SHR_ERRMSG(result)));
    }
    BCMDNX_IF_ERR_EXIT(result);
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_field_tcam_group_hw_refresh(_bcm_dpp_field_info_t *unitData,
                                     bcm_field_group_t group,
                                     int cond)
{
    _bcm_dpp_field_group_t *groupData;
    _bcm_dpp_field_ent_idx_t entry;
    unsigned int updated = 0;
    unsigned int copied = 0;
    unsigned int count = 0;
    int result;
    _bcm_dpp_field_entry_type_t entryType;    
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;

    groupData = &(unitData->groupD[group]);
    entryType = unitData->stageD[groupData->stage].modeBits[groupData->grpMode]->entryType;
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
    result = BCM_E_NONE;
    if (groupData->groupFlags & _BCM_DPP_FIELD_GROUP_PHASE) {
        /*
         *  Now odd phase, so priorities are all lower than the previous
         *  state (assuming there was one).  Update tail to head so that
         *  new versions of entries will 'stay' in proper order.
         */
        for (entry = groupData->entryTail;
             (entry < _BCM_DPP_FIELD_TCAM_ENTRY_LIMIT(entryType)) &&
             (BCM_E_NONE == result);
             entry = _BCM_DPP_FIELD_TCAM_ENTRY_PREV(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry), count++) {
            if ((!cond) || (_BCM_DPP_FIELD_TCAM_ENTRY_FLAGS(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry) & _BCM_DPP_FIELD_ENTRY_UPDATE)) {
                /* update this entry */
                if (_BCM_DPP_FIELD_TCAM_ENTRY_FLAGS(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry) & _BCM_DPP_FIELD_ENTRY_WANT_HW) {
                    /* entry wants to be in hardware */
                    if ((_BCM_DPP_FIELD_TCAM_ENTRY_FLAGS(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry) & _BCM_DPP_FIELD_ENTRY_CHANGED) ||
                        (0 == (_BCM_DPP_FIELD_TCAM_ENTRY_FLAGS(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry) & _BCM_DPP_FIELD_ENTRY_IN_HW))) {
                        /* entry has changed or is not yet in hardware, add it */
                        updated++;
                        result = _bcm_dpp_field_tcam_entry_hardware_install(unitData,
                                                                            _BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType),
                                                                            entry);
                    }
                } else { /* if (entryData->flags & _BCM_DPP_FIELD_ENTRY_WANT_HW) */
                    /* entry does not want to be in hardware */
                    if (_BCM_DPP_FIELD_TCAM_ENTRY_FLAGS(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry) & _BCM_DPP_FIELD_ENTRY_IN_HW) {
                        /* entry is in hardware, so take it out */
                        updated++;
                        result = _bcm_dpp_field_tcam_entry_remove(unitData,
                                                                  _BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType),
                                                                  entry);
                    }
                } /* if (entryData->flags & _BCM_DPP_FIELD_ENTRY_WANT_HW) */
            } else { /* if (update this entry) */
                /* no update to this entry... */
                if (_BCM_DPP_FIELD_TCAM_ENTRY_FLAGS(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry) & _BCM_DPP_FIELD_ENTRY_IN_HW) {
                    /* ...but need to shift it to new priority anyway */
                    copied++;
                    result = _bcm_dpp_field_tcam_entry_hardware_install(unitData,
                                                                        _BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType),
                                                                        entry);
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
             (entry < _BCM_DPP_FIELD_TCAM_ENTRY_LIMIT(entryType)) &&
             (BCM_E_NONE == result);
             entry = _BCM_DPP_FIELD_TCAM_ENTRY_NEXT(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry), count++) {
            if ((!cond) || (_BCM_DPP_FIELD_TCAM_ENTRY_FLAGS(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry) & _BCM_DPP_FIELD_ENTRY_UPDATE)) {
                if (_BCM_DPP_FIELD_TCAM_ENTRY_FLAGS(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry) & _BCM_DPP_FIELD_ENTRY_WANT_HW) {
                    /* entry wants to be in hardware */
                    if ((_BCM_DPP_FIELD_TCAM_ENTRY_FLAGS(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry) & _BCM_DPP_FIELD_ENTRY_CHANGED) ||
                        (0 == (_BCM_DPP_FIELD_TCAM_ENTRY_FLAGS(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry) & _BCM_DPP_FIELD_ENTRY_IN_HW))) {
                        /* entry has changed or is not yet in hardware, add it */
                        updated++;
                        result = _bcm_dpp_field_tcam_entry_hardware_install(unitData,
                                                                            _BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType),
                                                                            entry);
                    }
                } else { /* if (entryData->flags & _BCM_DPP_FIELD_ENTRY_WANT_HW) */
                    /* entry does not want to be in hardware */
                    if (_BCM_DPP_FIELD_TCAM_ENTRY_FLAGS(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry) & _BCM_DPP_FIELD_ENTRY_IN_HW) {
                        /* entry is in hardware, so take it out */
                        updated++;
                        result = _bcm_dpp_field_tcam_entry_remove(unitData,
                                                                  _BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType),
                                                                  entry);
                    }
                } /* if (entryData->flags & _BCM_DPP_FIELD_ENTRY_WANT_HW) */
            } else { /* if (update this entry) */
                /* no update to this entry... */
                if (_BCM_DPP_FIELD_TCAM_ENTRY_FLAGS(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry) & _BCM_DPP_FIELD_ENTRY_IN_HW) {
                    /* ...but need to shift it to new priority anyway */
                    copied++;
                    result = _bcm_dpp_field_tcam_entry_hardware_install(unitData,
                                                                        _BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType),
                                                                        entry);
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
    BCMDNX_IF_ERR_EXIT(result);
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *   Function
 *      _bcm_dpp_field_tcam_entry_action_update
 *   Purpose
 *      Take a set of proposed BCM layer actions for an entry, compute the
 *      appropriate PPD layer actions, update the BCM and PPD layer actions for
 *      the entry, and cleanup any resources freed due to removed actions or
 *      changes to the actions.
 *   Parameters
 *      (in) unitData = pointer to unit information
 *      (in) entry = entry ID
 *      (in) propActs = pointer to array of proposed actions
 *   Returns
 *      int (implied cast from bcm_error_t)
 *                    BCM_E_NONE if successful
 *                    BCM_E_* appropriately otherwise
 *   Notes
 *      This does not actually commit the entry to hardware, but other
 *      resources the entry references may be committed to hardware while
 *      building the PPD actions for the proposed action set, or may be
 *      released while cleaning up unused resources.
 *
 *      Assumes entry is valid.
 *
 *      It is possible that propActs will be edited to include implied actions
 *      if there are such based upon the actions already in propActs.
 */
STATIC int
_bcm_dpp_field_tcam_entry_action_update(_bcm_dpp_field_info_t *unitData,
                                        uint8 isExternalTcam,
                                        _bcm_dpp_field_ent_idx_t entry,
                                        _bcm_dpp_field_tc_b_act_t *propActs)
{
    _bcm_dpp_field_group_t *groupData;
    _bcm_dpp_field_stage_t *stageData;
    bcm_gport_t lookupGport[_BCM_DPP_FIELD_ACTION_CHAIN_MAX];
    bcm_gport_t lookupOutLif[_BCM_DPP_FIELD_ACTION_CHAIN_MAX];
    SHR_BITDCL actHit[_SHR_BITDCLSIZE(_BCM_DPP_NOF_BCM_ACTIONS_PER_DB_MAX)];
    _bcm_dpp_field_tc_p_act_t hwAct[SOC_PPD_FP_NOF_ACTIONS_PER_DB_MAX +
                                    _BCM_DPP_FIELD_ACTION_CHAIN_MAX];
    _bcm_dpp_field_tc_p_act_t *relActs;
    _bcm_dpp_field_tc_p_act_t *keepActs;
    SOC_PPD_FRWRD_DECISION_INFO fwdDecision;
    SOC_TMC_PKT_FRWRD_TYPE ppdForwardingType;
    SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT vlan_format;
#ifdef BCM_ARAD_SUPPORT
    SOC_SAND_SUCCESS_FAILURE success;
    SOC_PPD_FP_CONTROL_INDEX control_ndx;
    SOC_PPD_FP_CONTROL_INFO control_info;
#endif /* def BCM_ARAD_SUPPORT */
    uint64 dest_edata, mask_edata;
#ifdef BCM_ARAD_SUPPORT
    uint32 ppdr;
#endif /* def BCM_ARAD_SUPPORT */
    unsigned int bcmIndex;
    unsigned int ppdIndex;
    unsigned int actCount;
    unsigned int index;
    unsigned int proc;
    unsigned int cntr;
    int result;
    int local;
#ifdef BCM_ARAD_SUPPORT
    int meter;
    int group;
#endif /* def BCM_ARAD_SUPPORT */
    int fec;
    int inLif;
    int base_header;
    int ivec_id;
    int commit = FALSE;
    int mirrorId;
    bcm_module_t myModId;
    bcm_dpp_cosq_vsq_type_t vsqType;
    int vsq_core_id;
    uint8 vsq_is_ocb_only;
    uint8 is_ivec_to_set;
    uint8 is_vlan_format_valid;
    uint32 hwTrapCode = 0, ppdMask = 0;
    uint32 soc_sand_rv;
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;

    SOC_PPD_FRWRD_DECISION_INFO_clear(&fwdDecision);

    /* collect needed state information */
    groupData = &(unitData->groupD[_BCM_DPP_FIELD_TCAM_ENTRY_GROUP(isExternalTcam, entry)]);
    stageData = &(unitData->stageD[groupData->stage]);
    /* prepare workspace */
    sal_memset(&(actHit[0]),
               0x00,
               sizeof(actHit[0]) *
               _SHR_BITDCLSIZE(_BCM_DPP_NOF_BCM_ACTIONS_PER_DB_MAX));
    for (ppdIndex = 0;
         ppdIndex < (SOC_PPD_FP_NOF_ACTIONS_PER_DB_MAX +
                     _BCM_DPP_FIELD_ACTION_CHAIN_MAX);
         ppdIndex++) {
        hwAct[ppdIndex].hwType = SOC_PPD_FP_ACTION_TYPE_INVALID;
        hwAct[ppdIndex].hwFlags = 0;
        hwAct[ppdIndex].hwParam = 0;
    }
    /* need local module ID for certain operations */
    BCMDNX_IF_ERR_EXIT_MSG(bcm_petra_stk_my_modid_get(unit, &myModId),
                        (_BSL_BCM_MSG_NO_UNIT("unable to get unit %d module ID"),
                         unit));

    /* convert the BCM actions to PPD actions */
    for (bcmIndex = 0, ppdIndex = 0;
         bcmIndex < _BCM_DPP_NOF_BCM_ACTIONS_PER_DB_MAX;
         bcmIndex++) {
        if (SHR_BITGET(actHit, bcmIndex) ||
            (bcmFieldActionCount <= propActs[bcmIndex].bcmType)) {
            /* hit this one already or it is not valid/used; skip it */
            continue;
        }
        /* assume this BCM action is one PPD action */
        actCount = 1;
        /* prepare action workspace */
        for (index = 0; index < _BCM_DPP_FIELD_ACTION_CHAIN_MAX; index++) {
            lookupGport[index] = BCM_GPORT_INVALID;
            lookupOutLif[index] = BCM_GPORT_INVALID;
        }
        /* convert action according to hardware stage */
        if (SOC_PPD_FP_DATABASE_STAGE_INGRESS_PMF ==
            stageData->devInfo->hwStageId) {
            switch (propActs[bcmIndex].bcmType) {
            case bcmFieldActionCascadedKeyValueSet:
                /* param0 = cascaded key value */
                /* param1 = n/a */
                if (propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is ignored for %s"
                                         " action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                if (((~0) << unitData->cascadedKeyLen) &
                    propActs[bcmIndex].bcmParam0) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("param0 %d(%08X) too"
                                                       " big for unit %d"
                                                       " current cascaded"
                                                       " key length %d"),
                                      propActs[bcmIndex].bcmParam0,
                                      propActs[bcmIndex].bcmParam0,
                                      unit,
                                      unitData->cascadedKeyLen));
                }
                hwAct[ppdIndex].hwType = SOC_PPD_FP_ACTION_TYPE_CHANGE_KEY;
                hwAct[ppdIndex].hwParam = propActs[bcmIndex].bcmParam0;
                break;
            case bcmFieldActionTrap:
                /* param0 = trap GPORT (or trap ID) */
                /* param1 = n/a  (or trap strength if param0 is not GPORT) */
                hwAct[ppdIndex].hwType = SOC_PPD_FP_ACTION_TYPE_TRAP;
                if (BCM_GPORT_IS_SET(propActs[bcmIndex].bcmParam0)) {
                    if (!BCM_GPORT_IS_TRAP(propActs[bcmIndex].bcmParam0)) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                         (_BSL_BCM_MSG_NO_UNIT("%s action requires"
                                                           " TRAP type GPORT"
                                                           " as param0, GPORT"
                                                           " %08X is not"
                                                           " acceptable"),
                                          _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType],
                                          propActs[bcmIndex].bcmParam0));
                    }
                    /* take the provided GPORT as-is */
                    lookupGport[0] = propActs[bcmIndex].bcmParam0;
                    /* insert the trap qualifier in Arad */
                    if ((SOC_IS_ARAD(unit))
                        && (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "reduced_trap_action", 0) == 0)) {
                        hwAct[ppdIndex].hwParam = propActs[bcmIndex].bcmParam1 & 0xFFFF;   		    
                    }
                    else if (propActs[bcmIndex].bcmParam1) {
                        LOG_WARN(BSL_LS_BCM_FP,
                                 (BSL_META_U(unit,
                                             "param1(%d) is ignored for %s"
                                             " action if param0 is GPORT\n"),
                                  propActs[bcmIndex].bcmParam1,
                                  _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                    }

                } else if (SOC_IS_PETRAB(unit)) {
                    /* accept a raw trap code for testing */
                    
                    BCM_GPORT_TRAP_SET(lookupGport[0],
                                       propActs[bcmIndex].bcmParam0, /* ID */
                                       propActs[bcmIndex].bcmParam1, /* strength */
                                       0 /* snoop strength */);
                } else {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                                     (_BSL_BCM_MSG_NO_UNIT("unit %d does not recognize"
                                                       " unknown hardware"),
                                      unit));
                }
                break;
            case bcmFieldActionTrapReduced:
                /* param0 = trap GPORT (or trap ID) */
                /* param1 = n/a  (or trap strength if param0 is not GPORT) */
                hwAct[ppdIndex].hwType = SOC_PPD_FP_ACTION_TYPE_TRAP_REDUCED;
                if (BCM_GPORT_IS_SET(propActs[bcmIndex].bcmParam0)) {
                    if (!BCM_GPORT_IS_TRAP(propActs[bcmIndex].bcmParam0)) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                         (_BSL_BCM_MSG_NO_UNIT("%s action requires"
                                                           " TRAP type GPORT"
                                                           " as param0, GPORT"
                                                           " %08X is not"
                                                           " acceptable"),
                                          _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType],
                                          propActs[bcmIndex].bcmParam0));
                    }
                    /* take the provided GPORT as-is */
                    lookupGport[0] = propActs[bcmIndex].bcmParam0;
                    /* insert the trap qualifier in Arad */
                    if (propActs[bcmIndex].bcmParam1) {
                        LOG_WARN(BSL_LS_BCM_FP,
                                 (BSL_META_U(unit,
                                             "param1(%d) is ignored for %s"
                                             " action if param0 is GPORT\n"),
                                  propActs[bcmIndex].bcmParam1,
                                  _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                    }

                 
                } else {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                                     (_BSL_BCM_MSG_NO_UNIT("unit %d does not recognize"
                                                       " unknown hardware"),
                                      unit));
                }
                break;

            case bcmFieldActionISQ:
                /* param0 = ISQ */
                /* param1 = n/a */
                if (propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is ignored for %s"
                                         " action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                hwAct[ppdIndex].hwType = SOC_PPD_FP_ACTION_TYPE_IS;
                if (BCM_GPORT_IS_SET(propActs[bcmIndex].bcmParam0)) {
                    if (BCM_GPORT_IS_COSQ(propActs[bcmIndex].bcmParam0) &&
                        BCM_COSQ_GPORT_IS_ISQ(propActs[bcmIndex].bcmParam0)) {
                        hwAct[ppdIndex].hwParam = BCM_COSQ_GPORT_ISQ_QID_GET(propActs[bcmIndex].bcmParam0);
                    } else {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                         (_BSL_BCM_MSG_NO_UNIT("%s action rquires"
                                                           " COSQ_ISQ type"
                                                           " GPORT as param0,"
                                                           " GPORT %08X is"
                                                           " not acceptable"),
                                          _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType],
                                          propActs[bcmIndex].bcmParam0));
                    }
                } else {
                    /* accept ISQ ID raw */
                    
                    hwAct[ppdIndex].hwParam = propActs[bcmIndex].bcmParam0;
                }
                if (SOC_DPP_DEFS_GET(unit, nof_queues) < hwAct[ppdIndex].hwParam) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("ISQ ID of %d is not"
                                                       " valid on unit %d"),
                                      hwAct[ppdIndex].hwParam,
                                      unit));
                }
                break;
            case bcmFieldActionVSQ:
                /* param0 = VSQ */
                /* param1 = n/a */
                if (propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is ignored for %s"
                                         " action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                hwAct[ppdIndex].hwType = SOC_PPD_FP_ACTION_TYPE_VSQ_PTR;
                if (BCM_GPORT_IS_SET(propActs[bcmIndex].bcmParam0)) {
                    BCMDNX_IF_ERR_EXIT(_bcm_petra_cosq_gport_vsq_gport_get(unit,
                                                                           propActs[bcmIndex].bcmParam0,
                                                                           &vsq_core_id,
                                                                           &vsq_is_ocb_only,
                                                                           &vsqType,
                                                                           &local));
                    hwAct[ppdIndex].hwParam = local;
                } else {
                    /* accept VSQ ID raw */
                    
                    hwAct[ppdIndex].hwParam = propActs[bcmIndex].bcmParam0;
                }
                if (0xFF < hwAct[ppdIndex].hwParam) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("VSQ ID of %d is not"
                                                       " valid on unit %d"),
                                      hwAct[ppdIndex].hwParam,
                                      unit));
                }
                break;
            case bcmFieldActionPolicerLevel0:
            case bcmFieldActionPolicerLevel1:
                /* param0 = policer ID */
                /* param1 = n/a */
                if (propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is ignored for %s"
                                         " action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                if ((1 << (_DPP_POLICER_METER_GROUP_SHIFT(unit) + 1)) <= propActs[bcmIndex].bcmParam0) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("policer %d is not"
                                                       " valid on unit %d."
                                                       " Must be < %d"),
                                      propActs[bcmIndex].bcmParam0,
                                      unit,
                                      (1 << (_DPP_POLICER_METER_GROUP_SHIFT(unit) + 1))));
                }
                hwAct[ppdIndex].hwType = SOC_PPD_FP_ACTION_TYPE_METER;
#ifdef BCM_PETRAB_SUPPORT
                if (SOC_IS_PETRAB(unit)) {
                    /* Simpler encoding in Soc_petra-B */
                    hwAct[ppdIndex].hwParam = propActs[bcmIndex].bcmParam0;
                } else
#endif /* def BCM_PETRAB_SUPPORT */
#ifdef BCM_ARAD_SUPPORT
                if (SOC_IS_ARAD(unit)) {
                    /* For Arad, different PPD action (different pointer) according to the Meter Processor */
                    group = _DPP_POLICER_ID_TO_GROUP(unit, propActs[bcmIndex].bcmParam0);
                    meter = _DPP_POLICER_ID_TO_METER(unit, propActs[bcmIndex].bcmParam0);
                    if (1 == group || ((propActs[bcmIndex].bcmParam0 == 0) && (propActs[bcmIndex].bcmType == bcmFieldActionPolicerLevel1)) ) {
                        hwAct[ppdIndex].hwType = SOC_PPD_FP_ACTION_TYPE_METER_B;
                    }
                    /* lsb for meter update, 0 means don't meter packet*/
                    hwAct[ppdIndex].hwParam = (meter << 1) +
                                              ((propActs[bcmIndex].bcmParam0 != 0)? 1 :0);
                } else
#endif /* def BCM_ARAD_SUPPORT */
                {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                                     (_BSL_BCM_MSG_NO_UNIT("unit %d does not know"
                                                       " how to convert"
                                                       " policer ID on"
                                                       " unknown hardware"),
                                      unit));
                }
                break;
            case bcmFieldActionUsePolicerResult:
                /* param0 = Policer location to change */
                /* param1 = n/a */
                if (propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is ignored for %s"
                                         " action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                if (propActs[bcmIndex].bcmParam0 &
                    (~(BCM_FIELD_USE_POLICER_RESULT_INGRESS |
                       BCM_FIELD_USE_POLICER_RESULT_EGRESS))) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("unit %d entry %u"
                                                       " param0 contains bits"
                                                       " %08X not supported"),
                                      unit,
                                      _BCM_DPP_FIELD_TCAM_GLOBAL_ID(unit, entry, isExternalTcam),
                                      propActs[bcmIndex].bcmParam1 &
                                      (~(BCM_FIELD_USE_POLICER_RESULT_INGRESS |
                                         BCM_FIELD_USE_POLICER_RESULT_EGRESS))));
                }
                local = SOC_PPC_MTR_RES_USE_GET_BY_TYPE((propActs[bcmIndex].bcmParam0 &
                                                         BCM_FIELD_USE_POLICER_RESULT_INGRESS),
                                                        (propActs[bcmIndex].bcmParam0 &
                                                         BCM_FIELD_USE_POLICER_RESULT_EGRESS));
#ifdef BCM_PETRAB_SUPPORT
                if (SOC_IS_PETRAB(unit)) {
                    hwAct[ppdIndex].hwType = SOC_PPD_FP_ACTION_TYPE_DP;
                    /* Bits 3:2 set to indicate the DP-Meter-Command must be changed */
                    hwAct[ppdIndex].hwParam = 0x8 + local;
                } else
#endif /* def BCM_PETRAB_SUPPORT */
#ifdef BCM_ARAD_SUPPORT
                if (SOC_IS_ARAD(unit)) {
                    hwAct[ppdIndex].hwType = SOC_PPD_FP_ACTION_TYPE_DP_METER_COMMAND;
                    hwAct[ppdIndex].hwParam = local;
                } else
#endif /* def BCM_ARAD_SUPPORT */
                {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                                     (_BSL_BCM_MSG_NO_UNIT("unit %d does not know"
                                                       " how to convert"
                                                       " policer DP update on"
                                                       " unknown hardware"),
                                      unit));
                }
                break;
            case bcmFieldActionStat:
            case bcmFieldActionStat1:
                /* param0 = stat ID */
                /* param1 = n/a */
                if (propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is ignored for %s action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                BCMDNX_IF_ERR_EXIT(_bcm_dpp_proc_cntr_from_stat(unitData,
                                                                     propActs[bcmIndex].bcmParam0,
                                                                     &proc,
                                                                     &cntr));
#ifdef BCM_PETRAB_SUPPORT
                if (SOC_IS_PETRAB(unit)) {
                    hwAct[ppdIndex].hwType = SOC_PPD_FP_ACTION_TYPE_COUNTER;
                    hwAct[ppdIndex].hwParam = (cntr << 1) | proc;
                } else
#endif /* def BCM_PETRAB_SUPPORT */
#ifdef BCM_ARAD_SUPPORT
                if(SOC_IS_ARAD(unit)) {
                    
                    hwAct[ppdIndex].hwType = (bcmFieldActionStat1 ==
                                              propActs[bcmIndex].bcmType)?
                                             SOC_PPD_FP_ACTION_TYPE_COUNTER_B:
                                             SOC_PPD_FP_ACTION_TYPE_COUNTER_A;
                     /* LSB always set: Update Counter, others: Counter-Id */
                    hwAct[ppdIndex].hwParam = (cntr << 1) | 1;
                } else
#endif /* BCM_ARAD_SUPPORT */
                {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                                     (_BSL_BCM_MSG_NO_UNIT("unit %d does not know"
                                                       " how to convert"
                                                       " stats info on"
                                                       " unknown hardware"),
                                      unit));
                }
                break;
#ifdef BCM_PETRAB_SUPPORT
            case bcmFieldActionCopyToCpu: /* Soc_petra-B only */
                /* param0 = n/a */
                /* param1 = n/a */
                if (propActs[bcmIndex].bcmParam0 ||
                    propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param0(%d) and param1(%d) are both"
                                         " ignored for %s action\n"),
                              propActs[bcmIndex].bcmParam0,
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                for (index = 0; index < BCM_PBMP_PORT_MAX; index++) {
                    if (BCM_PBMP_MEMBER(PBMP_CMIC(unit),index)) {
                        /* stop at the first CPU port */
                        break;
                    }
                }
                if (index < BCM_PBMP_PORT_MAX) {
                    /* found the first CPU port; mirror to it */
                    BCMDNX_IF_ERR_EXIT(bcm_petra_ingress_mirror_alloc(unit,
                                                                           myModId,
                                                                           myModId,
                                                                           index,
                                                                           &mirrorId));
                    hwAct[ppdIndex].hwType = SOC_PPD_FP_ACTION_TYPE_MIRROR;
                    hwAct[ppdIndex].hwParam = mirrorId;
                } else {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                                     (_BSL_BCM_MSG_NO_UNIT("unit %d does not seem"
                                                       " to have a CPU port\n"),
                                      unit));
                }
                break;
#endif /* def BCM_PETRAB_SUPPORT */
            case bcmFieldActionMirrorIngress:
                /* param0 = destination module (ignored if port is a GPORT) */
                /* param1 = destination port OR GPORT */
                if (BCM_GPORT_IS_SET(propActs[bcmIndex].bcmParam1) &&
                    BCM_GPORT_IS_MIRROR(propActs[bcmIndex].bcmParam1)) {
                    /*
                     *  Using GPORT mode and the GPORT is a mirror, so it was
                     *  probably created using the mirror dest APIs or similar,
                     *  and so should already have at least one reference. We
                     *  need to mark this as a reference to it so that when the
                     *  action is removed later it will not decrement to zero
                     *  and free the application-created mirror.
                     */
                    mirrorId = BCM_GPORT_MIRROR_GET(propActs[bcmIndex].bcmParam1);
#ifdef BCM_PETRAB_SUPPORT
                    /* Soc_petraB mirrors keep a reference count */
                    if (SOC_IS_PETRAB(unit)) {
                        BCMDNX_IF_ERR_EXIT(bcm_petra_ingress_mirror_reference(unit,
                                                                                   mirrorId));
                    }
#endif /* def BCM_PETRAB_SUPPORT */
                } else {
                    /*
                     *  Either the GPORT is not a MIRROR GPORT or the caller is
                     *  using the standard module,port form of the action.  In
                     *  either case, need to allocate a mirror for this to work
                     *  (the allocate function will reuse the same mirror if
                     *  there is already one going to this place).
                     */
#ifdef BCM_PETRAB_SUPPORT
                    if (SOC_IS_PETRAB(unit)) {
                        BCMDNX_IF_ERR_EXIT(bcm_petra_ingress_mirror_alloc(unit,
                                                                               myModId,
                                                                               propActs[bcmIndex].bcmParam0,
                                                                               propActs[bcmIndex].bcmParam1,
                                                                               &mirrorId));
                    } else
#endif /* def BCM_PETRAB_SUPPORT */
                    {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                                         (_BSL_BCM_MSG_NO_UNIT("unit %d does not"
                                                           " allow implicit"
                                                           " mirror alloc and"
                                                           " param1 %d(%08X)"
                                                           " is not a mirror"
                                                           " gport\n"),
                                          unit,
                                          propActs[bcmIndex].bcmParam1,
                                          propActs[bcmIndex].bcmParam1));
                    }
                }
                hwAct[ppdIndex].hwType = SOC_PPD_FP_ACTION_TYPE_MIRROR;
                hwAct[ppdIndex].hwParam = mirrorId;
                break;
            case bcmFieldActionSnoop:
                /* param0 = gport of type trap - the strength is inside */
                /* param1 = n/a */
                if (propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is ignored for %s"
                                         " action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                if (BCM_GPORT_IS_SET(propActs[bcmIndex].bcmParam0) &&
                    BCM_GPORT_IS_TRAP(propActs[bcmIndex].bcmParam0)) {
                    SOC_PPD_TRAP_CODE ppd_trap_code;

                    BCMDNX_IF_ERR_EXIT_MSG(_bcm_rx_ppd_trap_code_from_trap_id(unit, BCM_GPORT_TRAP_GET_ID(propActs[bcmIndex].bcmParam0),
                                                                           &ppd_trap_code),
                                        (_BSL_BCM_MSG_NO_UNIT("unable to map trap id %d to ppd trap"),
                                                          BCM_GPORT_TRAP_GET_ID(propActs[bcmIndex].bcmParam0)));
                    /* Snoop strength (2b) ; Snoop code (8b)*/
                    hwAct[ppdIndex].hwType = SOC_PPD_FP_ACTION_TYPE_SNP;
                    BCMDNX_IF_ERR_EXIT_MSG(_bcm_dpp_field_trap_ppd_to_hw(unit,
                                                                      ppd_trap_code,
                                                                      &hwAct[ppdIndex].hwParam),
                                        (_BSL_BCM_MSG_NO_UNIT("unable to get from"
                                                          " the HW Trap Id for"
                                                          " PPD trap %d(%s)"),
                                         ppd_trap_code, SOC_PPD_TRAP_CODE_to_string(ppd_trap_code)));
                   hwAct[ppdIndex].hwParam += (BCM_GPORT_TRAP_GET_SNOOP_STRENGTH(propActs[bcmIndex].bcmParam0) << 8);
                } else {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("%s action requires"
                                                       " TRAP type GPORT as"
                                                       " param0, param0 %08X"
                                                       " is not acceptable"),
                                      _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType],
                                      propActs[bcmIndex].bcmParam0));
                }
                break;
            case bcmFieldActionDrop:
                /* param0 = n/a */
                /* param1 = n/a */
                /* ingress drop: send to discard location */
                if (propActs[bcmIndex].bcmParam0 ||
                    propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param0(%d) and param1(%d) are both"
                                         " ignored for %s action\n"),
                              propActs[bcmIndex].bcmParam0,
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                hwAct[ppdIndex].hwType = SOC_PPD_FP_ACTION_TYPE_DEST;
                hwAct[ppdIndex].hwParam = _BCM_DPP_FIELD_DROP_DEST(unit);
                break;
            case bcmFieldActionRedirectMcast:
                /* param0 = L2 GPORT or multicast group ID */
                /* param1 = n/a */
            case bcmFieldActionRedirectIpmc:
                /* param0 = IPMC GPORT or multicast group ID */
                /* param1 = n/a */
                if (propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is ignored for %s"
                                         " action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                if (BCM_GPORT_IS_SET(propActs[bcmIndex].bcmParam0)) {
                    /* param0 is GPORT; handle as such */
                    lookupGport[0] = propActs[bcmIndex].bcmParam0;
                } else { /* if (BCM_GPORT_IS_SET(param0)) */
                    /* param0 is not GPORT; consider it a multicast group ID */
                    int multi_group_id = _BCM_MULTICAST_ID_GET(propActs[bcmIndex].bcmParam0);
                    if (16383 < multi_group_id) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                         (_BSL_BCM_MSG_NO_UNIT("non-GPORT %s"
                                                           " param0(%d) multi_group_id(%d)"
                                                           " is not valid"),
                                          _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType],
                                          propActs[bcmIndex].bcmParam0, multi_group_id));
                    }
                    
                    hwAct[ppdIndex].hwParam = propActs[bcmIndex].bcmParam0;
                } /* if (BCM_GPORT_IS_SET(param0)) */
                hwAct[ppdIndex].hwType = SOC_PPD_FP_ACTION_TYPE_DEST;
                break;
            case bcmFieldActionRedirectTrunk:
                /* param0 = aggregate GPORT or aggregate ID */
                /* param1 = n/a */
                if (propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is ignored for %s"
                                         " action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                if (BCM_GPORT_IS_SET(propActs[bcmIndex].bcmParam0)) {
                    /* param0 is GPORT; handle as such */
                    lookupGport[0] = propActs[bcmIndex].bcmParam0;
                } else { /* if (BCM_GPORT_IS_SET(param0)) */
                    /* param0 is not a GPORT; assume it is aggregate ID */
                    if (255 < propActs[bcmIndex].bcmParam0) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                         (_BSL_BCM_MSG_NO_UNIT("non-GPORT %s"
                                                           " param0(%d)"
                                                           " is not valid"),
                                          _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType],
                                          propActs[bcmIndex].bcmParam0));
                    }
                    
                    hwAct[ppdIndex].hwParam = (propActs[bcmIndex].bcmParam0 |
                                         (1 << 12));
                    
                } /* if (BCM_GPORT_IS_SET(param0)) */
                hwAct[ppdIndex].hwType = SOC_PPD_FP_ACTION_TYPE_DEST;
                break;
            case bcmFieldActionRedirect:
                /* param0 = module */
                /* param1 = GPORT or port number */
                if (BCM_GPORT_IS_SET(propActs[bcmIndex].bcmParam1)) {
                    /* param1 is GPORT; handle as such */
                    lookupGport[0] = propActs[bcmIndex].bcmParam1;
                } else { /* if (BCM_GPORT_IS_SET(param1)) */
                    /*
                     *  param1 is not a GPORT, so param1 must be physical port
                     *  number and param0 is module ID.
                     */
                    if ((_SHR_GPORT_PORT_MASK < propActs[bcmIndex].bcmParam1) ||
                        (_SHR_GPORT_MODID_MASK < propActs[bcmIndex].bcmParam0)) {
                        /* can't represent it as modport, so error */
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                         (_BSL_BCM_MSG_NO_UNIT("non-GPORT %s"
                                                           " param0(%d)"
                                                           " is not valid"
                                                           " modid (0..%d) or"
                                                           " param1(%d)"
                                                           " is not valid"
                                                           " port (0..%d)"),
                                          _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType],
                                          propActs[bcmIndex].bcmParam0,
                                          _SHR_GPORT_MODID_MASK,
                                          propActs[bcmIndex].bcmParam1,
                                          _SHR_GPORT_PORT_MASK));
                    }
                    BCM_GPORT_MODPORT_SET(lookupGport[0],
                                          propActs[bcmIndex].bcmParam0,
                                          propActs[bcmIndex].bcmParam1);
                    
                }
                hwAct[ppdIndex].hwType = SOC_PPD_FP_ACTION_TYPE_DEST;
                break;
            case bcmFieldActionL3Switch:
                /* param0 = FEC-ID */
                /* param1 = n/a */
                if (propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is ignored for %s"
                                         " action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                if (!_BCM_PETRA_L3_ITF_IS_FEC(propActs[bcmIndex].bcmParam0)) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("%s param0 (new FEC-Id)"
                                                       " %d, is not an egress"
                                                       " object that represents"
                                                       " a FEC"),
                                      _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType],
                                      propActs[bcmIndex].bcmParam0));

                }
                fec = _BCM_PETRA_L3_ITF_VAL_GET(propActs[bcmIndex].bcmParam0);
                if (0x7FFF < fec) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("%s param0 (new FEC-Id)"
                                                       " %d, but must be"
                                                       " (0..0x7FFF)"),
                                      _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType],
                                      propActs[bcmIndex].bcmParam0));
                }
                /* Get the Forward decision and the HW encoding */
                SOC_PPD_FRWRD_DECISION_FEC_SET(unitData->unitHandle,
                                               &fwdDecision,
                                               fec,
                                               soc_sand_rv);
                BCM_SAND_IF_ERR_EXIT_NO_UNIT(soc_sand_rv);
/*
 * COVERITY
 *
 * mask_edata is initialized inside _bcm_dpp_field_qualify_fwd_decision_to_hw_destination_convert.
 */
/* coverity[uninit_use_in_call] */
                COMPILER_64_SET(mask_edata, 0, (~0));

                BCMDNX_IF_ERR_EXIT_MSG(_bcm_dpp_field_qualify_fwd_decision_to_hw_destination_convert(unit,
                                                                                                  &fwdDecision,
                                                                                                  TRUE, /* is_for_destination */
                                                                                                  TRUE, /* is_for_action */
                                                                                                  &dest_edata,
                                                                                                  &mask_edata),
                                    (_BSL_BCM_MSG_NO_UNIT("unable to get from the"
                                                      " forward decision the"
                                                      " HW data, FEC %d"),
                                     propActs[bcmIndex].bcmParam0));

                hwAct[ppdIndex].hwParam = COMPILER_64_LO(dest_edata);
                hwAct[ppdIndex].hwType = SOC_PPD_FP_ACTION_TYPE_DEST;
                break;
            case bcmFieldActionDstRpfGportNew:
                /* param0 = GPORT */
                /* param1 = n/a */
                if (propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is ignored for %s"
                                         " action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                if (BCM_GPORT_IS_SET(propActs[bcmIndex].bcmParam0)) {
                    /* param0 is GPORT; handle as such */
                    lookupGport[0] = propActs[bcmIndex].bcmParam0;
                    hwAct[ppdIndex].hwType = SOC_PPD_FP_ACTION_TYPE_RPF_DESTINATION;
                    /* Destination is valid */
                    hwAct[ppdIndex + 1].hwType = SOC_PPD_FP_ACTION_TYPE_RPF_DESTINATION_VALID;
                    hwAct[ppdIndex + 1].hwParam = 0x1;
                    /* two hardware actions are used here */
                    actCount = 2;
                } else { /* if (BCM_GPORT_IS_SET(param0)) */
                    /*
                     *  param1 must be a GPORT, so param1 must be physical port
                     *  number and param0 is module ID.
                     */
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("%s param0(%d)"
                                                       " is not valid GPORT"),
                                      _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType],
                                      propActs[bcmIndex].bcmParam0));
                }
                break;
            case bcmFieldActionSrcGportNew:
                /* param0 = GPORT */
                /* param1 = n/a */
                if (propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is ignored for %s"
                                         " action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                if (BCM_GPORT_IS_SET(propActs[bcmIndex].bcmParam0)) {
                    lookupGport[0] = propActs[bcmIndex].bcmParam0;
                    hwAct[ppdIndex].hwType = SOC_PPD_FP_ACTION_TYPE_SRC_SYST_PORT;
                } else { /* if (BCM_GPORT_IS_SET(param0)) */
                    /*
                     *  param1 must be a GPORT, so param1 must be physical port
                     *  number and param0 is module ID.
                     */
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("%s param0(%d)"
                                                       " is not valid GPORT"),
                                      _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType],
                                      propActs[bcmIndex].bcmParam0));
                }
                break;
            case bcmFieldActionIngressGportSet:
                /* param0 = GPORT */
                /* param1 = n/a */
                if (propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is ignored for %s"
                                         " action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                if (BCM_GPORT_IS_SET(propActs[bcmIndex].bcmParam0)) {
                    /* Special case for In-LIF 0 to reset the In-LIF value */
                    if ((BCM_GPORT_IS_VLAN_PORT(propActs[bcmIndex].bcmParam0)) &&  (0 == BCM_GPORT_VLAN_PORT_ID_GET(propActs[bcmIndex].bcmParam0))) {
                        hwAct[ppdIndex].hwParam = 0;
                        hwAct[ppdIndex].hwType = SOC_PPD_FP_ACTION_TYPE_IN_LIF;
                    }
                    else {
                      /* param0 is GPORT; handle as such */
                      /* If the gport is of type LIF, change the InLIF, otherwise Destination */
                      result = _bcm_dpp_gport_to_global_lif(unit,
                                                     propActs[bcmIndex].bcmParam0,
                                                     &inLif,
                                                     NULL,
                                                     &fec,
                                                     &local);
                      if ((BCM_E_NONE != result) ||
                          (inLif == _BCM_GPORT_ENCAP_ID_LIF_INVALID)) {
                          /* Do not exit - just indicate it is not LIF */
                          LOG_WARN(BSL_LS_BCM_FP,
                                   (BSL_META_U(unit,
                                               "unable to compute inlif for"
                                               " unit %d GPORT %08X"
                                               " %d (%s)\n"),
                                    unit,
                                    propActs[bcmIndex].bcmParam0,
                                    result,
                                    _SHR_ERRMSG(result)));
                      }
                      /* Allow inLif 0 in Arad but not in Petra-B */
                      lookupOutLif[0] = propActs[bcmIndex].bcmParam0;
                      hwAct[ppdIndex].hwType = SOC_PPD_FP_ACTION_TYPE_IN_LIF;
                    }
                } else { /* if (BCM_GPORT_IS_SET(param0)) */
                    /*
                     *  param1 must be a GPORT, so param1 must be physical port
                     *  number and param0 is module ID.
                     */
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("%s param0(%d)"
                                                       " is not valid GPORT"),
                                      _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType],
                                      propActs[bcmIndex].bcmParam0));
                }
                break;
            case bcmFieldActionPrioIntNew:
                /* param0 = new priority (internal only?) */
                /* param1 (misdoc as N/A?) contains marking flags(???) */
            case bcmFieldActionVportTcNew:
                /* param0 = new traffic class value */
                /* param1 (misdoc as N/A?) contains marking flags(???) */
                if (propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is ignored for %s"
                                         " action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                if (7 < propActs[bcmIndex].bcmParam0) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("%s param0 (new"
                                                       " priority) %d, but"
                                                       " must be (0..7)"),
                                      _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType],
                                      propActs[bcmIndex].bcmParam0));
                }
                hwAct[ppdIndex].hwParam = (1 << 3) | (propActs[bcmIndex].bcmParam0 & 0x7);
                hwAct[ppdIndex].hwType = SOC_PPD_FP_ACTION_TYPE_TC;
                break;
            case bcmFieldActionIncomingMplsPortSet:
            case bcmFieldActionIncomingIpIfSet:
                /* param0 = interface-id */
                /* param1 = N/A */
                if (propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is ignored for %s"
                                         " action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                if (0xFFF < propActs[bcmIndex].bcmParam0) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("%s param0 (new"
                                                       " port) %d, but must"
                                                       " be (0..0xFFF)"),
                                      _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType],
                                      propActs[bcmIndex].bcmParam0));
                }
                hwAct[ppdIndex].hwParam = propActs[bcmIndex].bcmParam0 & 0xFFF;
                hwAct[ppdIndex].hwType = SOC_PPD_FP_ACTION_TYPE_IN_RIF;
                break;
            case bcmFieldActionSystemHeaderSet:
                /* param0 = System-Header-type */
                /* param1 = Header value */
                if (propActs[bcmIndex].bcmParam0 != bcmFieldSystemHeaderPphEei) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("%s param0 (system"
                                                       " header type) %d, but"
                                                       " must be"
                                                       " bcmFieldSystemHeaderPphEei"),
                                      _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType],
                                      propActs[bcmIndex].bcmParam0));
                }
                if (0xFFFFFF < propActs[bcmIndex].bcmParam1) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("%s param1 (new header)"
                                                       " %d, but must be"
                                                       " (0..0xFFFFFF)"),
                                      _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType],
                                      propActs[bcmIndex].bcmParam1));
                }
                hwAct[ppdIndex].hwParam = propActs[bcmIndex].bcmParam1 & 0xFFFFFF;
                hwAct[ppdIndex].hwType = SOC_PPD_FP_ACTION_TYPE_EEI;
                break;
            case bcmFieldActionLearnForwardingVlanIdNew:
                /* param0 = VLAN-ID (actually FID) */
                /* param1 = N/A */
                if (propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is ignored for %s"
                                         " action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                if (0x7FFF < propActs[bcmIndex].bcmParam0) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("%s param0 (new priority)"
                                                       " %d, but must be (0..0x7FFF)"),
                                      _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType],
                                      propActs[bcmIndex].bcmParam0));
                }
                hwAct[ppdIndex].hwParam = propActs[bcmIndex].bcmParam0 & 0x7FFF;
                hwAct[ppdIndex].hwType = SOC_PPD_FP_ACTION_TYPE_LEARN_FID;
                break;
            case bcmFieldActionLearnSrcPortNew:
                /* param0 = Source-Port */
                /* param1 = N/A */
                if (propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is ignored for %s"
                                         " action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                if (BCM_GPORT_IS_SET(propActs[bcmIndex].bcmParam0)) {
                    /* param0 is GPORT; handle as such */
                    lookupGport[0] = propActs[bcmIndex].bcmParam0;
                    hwAct[ppdIndex].hwType = SOC_PPD_FP_ACTION_TYPE_LEARN_DATA_0_TO_15;
                } else { /* if (BCM_GPORT_IS_SET(param0)) */
                    /*
                     *  param1 must be a GPORT, so param1 must be physical port
                     *  number and param0 is module ID.
                     */
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("%s param0(%d)"
                                                       " is not valid GPORT"),
                                      _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType],
                                      propActs[bcmIndex].bcmParam0));
                }
                break;
            case bcmFieldActionLearnInVPortNew:
                /* param0 = Source-VPort */
                /* param1 = N/A */
                if (propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is ignored for %s"
                                         " action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_gport_to_lif_convert(unit,
                                                                            propActs[bcmIndex].bcmParam0,
                                                                            TRUE,
                                                                            &(hwAct[ppdIndex].hwParam)));
                hwAct[ppdIndex].hwType = SOC_PPD_FP_ACTION_TYPE_LEARN_DATA_16_TO_39;
                break;
            case bcmFieldActionMacDaKnown:
                /* param0 = N/A */
                /* param1 = N/A */
                if (propActs[bcmIndex].bcmParam0 ||
                    propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param0(%d) and param1(%d) are both"
                                         " ignored for %s action\n"),
                              propActs[bcmIndex].bcmParam0,
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                /* Although the name, DLF is a Destination failure, so the packet is unknown */
                hwAct[ppdIndex].hwParam = 0x1;
                hwAct[ppdIndex].hwType = SOC_PPD_FP_ACTION_TYPE_UNKNOWN_ADDR;
                break;
            case bcmFieldActionDoNotLearn:
                /* param0 = 0 - do not learn, 1 - ingress learning, 2 - egress learning */
                /* param1 = N/A */
                if (propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is"
                                         " ignored for %s action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                /* Invert the meaning: 0 in HW means Do-not-learn */
                hwAct[ppdIndex].hwParam = (propActs[bcmIndex].bcmParam0 == 1)? 1: 0;
                hwAct[ppdIndex].hwType = SOC_PPD_FP_ACTION_TYPE_INGRESS_LEARN_ENABLE;
                hwAct[ppdIndex + 1].hwParam = (propActs[bcmIndex].bcmParam0 == 2)? 1: 0;
                hwAct[ppdIndex + 1].hwType = SOC_PPD_FP_ACTION_TYPE_EGRESS_LEARN_ENABLE;
                /* this took two actions */
                actCount = 2;
                break;
            case bcmFieldActionFilters:
                /* param0 = filter type */
                /* param1 = N/A */
                if (propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is ignored for %s"
                                         " action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                if (propActs[bcmIndex].bcmParam0 !=
                    BCM_FIELD_FILTER_SA_DISABLED) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("%s param0 (filter"
                                                       " type) %d, but must be"
                                                       " BCM_FIELD_FILTER_SA_DISABLED"),
                                      _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType],
                                      propActs[bcmIndex].bcmParam0));
                }
                hwAct[ppdIndex].hwParam = 0x1;
                hwAct[ppdIndex].hwType = SOC_PPD_FP_ACTION_TYPE_EXC_SRC;
                break;
            case bcmFieldActionOrientationSet:
                /* param0 = orientation type */
                /* param1 = N/A */
                if (propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is ignored for %s"
                                         " action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                if ((bcmFieldOrientationNetwork !=
                     propActs[bcmIndex].bcmParam0) &&
                    (bcmFieldOrientationAccess !=
                     propActs[bcmIndex].bcmParam0)) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("%s param0 (orientation)"
                                                       " %d, but must be one of"
                                                       " (bcmFieldOrientationNetwork,"
                                                       " bcmFieldOrientationAccess)"),
                                      _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType],
                                      propActs[bcmIndex].bcmParam0));
                }
                hwAct[ppdIndex].hwParam = (bcmFieldOrientationNetwork ==
                                           propActs[bcmIndex].bcmParam0);
                hwAct[ppdIndex].hwType = SOC_PPD_FP_ACTION_TYPE_ORIENTATION_IS_HUB;
                break;
            case bcmFieldActionDhcpPkt:
                /* param0 = BOOTP/DHCP packet indicator */
                /* param1 = N/A */
                if (propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is ignored for %s"
                                         " action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                hwAct[ppdIndex].hwParam = propActs[bcmIndex].bcmParam0?1:0;
                hwAct[ppdIndex].hwType = SOC_PPD_FP_ACTION_TYPE_PACKET_IS_BOOTP_DHCP;
                break;
            case bcmFieldActionCnmCancel:
                /* param0 = N/A */
                /* param1 = N/A */
                if (propActs[bcmIndex].bcmParam0 ||
                    propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param0(%d) and param1(%d) are both"
                                         " ignored for %s action\n"),
                              propActs[bcmIndex].bcmParam0,
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                /* Ignore-CP if done */
                hwAct[ppdIndex].hwParam = 0x1;
                hwAct[ppdIndex].hwType = SOC_PPD_FP_ACTION_TYPE_IGNORE_CP;
                break;
            case bcmFieldActionStartPacketStrip:
                /* param0 = Header-Base */
                /* 
                 * param1 = Number of bytes from the Header-Base 
                 * in a 2's complement format (up to 0x1F, positive values)  
                 */
                if (propActs[bcmIndex].bcmParam1 > 0x3F) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("%s param1 (Bytes to"
                                                       " strip) %d, but must"
                                                       " be <= 0x1F"),
                                      _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType],
                                      propActs[bcmIndex].bcmParam1));
                }
                /* Translate the base header */
                switch (propActs[bcmIndex].bcmParam0) {
                case bcmFieldStartToConfigurableStrip:
                    base_header = 0x0;
                    break;
                case bcmFieldStartToL2Strip:
                    base_header = 0x1;
                    break;
                case bcmFieldStartToFwdStrip:
                    base_header = 0x2;
                    break;
                case bcmFieldStartToFwdNextStrip:
                    base_header = 0x3;
                    break;
                default:
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("%s param0 (header base)"
                                                       " %d is not valid"),
                                      _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType],
                                      propActs[bcmIndex].bcmParam0));
                }
                hwAct[ppdIndex].hwParam = (propActs[bcmIndex].bcmParam1 |
                                           (base_header << 6));
                hwAct[ppdIndex].hwType = SOC_PPD_FP_ACTION_TYPE_BYTES_TO_REMOVE;
                break;
            case bcmFieldActionForwardingTypeNew:
                /* param0 = New Forwarding header type */
                /* param1 = N/A in general - if set, set the forwarding-offset */
                /* Translate the forwarding header */
                BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_forwarding_type_bcm_to_ppd(propActs[bcmIndex].bcmParam0,
                                                                                  &ppdForwardingType,
                                                                                  &ppdMask));
                
                switch(propActs[bcmIndex].bcmParam0) {
                case bcmFieldForwardingTypeL2:
                case bcmFieldForwardingTypeRxReason:
                case bcmFieldForwardingTypeTrafficManagement:
                case bcmFieldForwardingTypeSnoop:
                    local = 1;
                    break;
                case bcmFieldForwardingTypeIp4Ucast:
                case bcmFieldForwardingTypeIp4Mcast:
                case bcmFieldForwardingTypeIp6Ucast:
                case bcmFieldForwardingTypeIp6Mcast:
                case bcmFieldForwardingTypeMpls:
                case bcmFieldForwardingTypeTrill:
                    local = 2;
                    break;
                default:
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("%s param0 (Forwarding"
                                                       " Type) %d is not"
                                                       " supported"),
                                      _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType],
                                      propActs[bcmIndex].bcmParam0));
                };
                hwAct[ppdIndex].hwParam = ppdForwardingType;
                hwAct[ppdIndex].hwType = SOC_PPD_FP_ACTION_TYPE_FWD_CODE;
                /* Take fix (5:0), i.e. additional local-offset of 0, only base-header (8:6) */
                /* Set the base-header explicitly via param1 if unusual */
                local = (propActs[bcmIndex].bcmParam1)? propActs[bcmIndex].bcmParam1: local;
                hwAct[ppdIndex + 1].hwParam = 0 + (local << 6);
                hwAct[ppdIndex + 1].hwType = SOC_PPD_FP_ACTION_TYPE_FWD_OFFSET;
                /* two actions to do this */
                actCount = 2;
                break;
            case bcmFieldActionDropPrecedence:
                /* param0 = new DP (BCM_FIELD_COLOR_*) */
                /* param1 = n/a */
                if (propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is ignored for %s"
                                         " action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_color_bcm_to_ppd(propActs[bcmIndex].bcmParam0,
                                                                        &(hwAct[ppdIndex].hwParam)));
                /* but also need to tell it to always set the value */
                hwAct[ppdIndex].hwParam |= 0x0C;
                hwAct[ppdIndex].hwType = SOC_PPD_FP_ACTION_TYPE_DP;
                break;
            case bcmFieldActionMirrorEgressDisable:
                /* param0 = disable egress mirror flag */
                /* param1 = n/a */
                if (propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is ignored for %s"
                                         " action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                hwAct[ppdIndex].hwType = SOC_PPD_FP_ACTION_TYPE_MIR_DIS;
                hwAct[ppdIndex].hwParam = propActs[bcmIndex].bcmParam0?1:0;
                break;
            case bcmFieldActionVrfSet:
                /* param0 = new VRF ID */
                /* param1 = n/a */
                if (propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is ignored for %s"
                                         " action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                if (SOC_DPP_DEFS_GET(unit, nof_vrfs) < propActs[bcmIndex].bcmParam0) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("%s param0 (new VRF)"
                                                       " %d, but must be"
                                                       " (0..0xFFF)"),
                                      _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType],
                                      propActs[bcmIndex].bcmParam0));
                }
                hwAct[ppdIndex].hwType = SOC_PPD_FP_ACTION_TYPE_VRF;
                hwAct[ppdIndex].hwParam = propActs[bcmIndex].bcmParam0 & ( SOC_IS_JERICHO(unit) ? 0x1FFF: 0xFFF );
                break;
            case bcmFieldActionStatTag:
                /* param0 = new stat tag */
                /* param1 = n/a */
                if (propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is ignored for %s"
                                         " action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                if (0xFF < propActs[bcmIndex].bcmParam0) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("%s param0 (new stat"
                                                       " tag) %d, but must be"
                                                       " (0..0xFF)"),
                                      _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType],
                                      propActs[bcmIndex].bcmParam0));
                }
                hwAct[ppdIndex].hwType = SOC_PPD_FP_ACTION_TYPE_STAT;
                hwAct[ppdIndex].hwParam = propActs[bcmIndex].bcmParam0 & 0xFF;
                break;
            case bcmFieldActionMultipathHashAdditionalInfo:
                /* param0 = new MP hash additional info */
                /* param1 = n/a */
                if (propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is ignored for %s"
                                         " action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                if (0xFFFFF < propActs[bcmIndex].bcmParam0) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("%s param0 (new MP hash"
                                                       " additional info) %d,"
                                                       " but must be"
                                                       " (0..0xFFFFF)"),
                                      _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType],
                                      propActs[bcmIndex].bcmParam0));
                }
                hwAct[ppdIndex].hwType = SOC_PPD_FP_ACTION_TYPE_ECMP_LB;
                hwAct[ppdIndex].hwParam = (propActs[bcmIndex].bcmParam0 &
                                           0xFFFFF);
                break;
            case bcmFieldActionTrunkHashKeySet:
                /* param0 = new trunk hash key */
                /* param1 = n/a */
                if (propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is ignored for %s"
                                         " action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                if (0xFFFFF < propActs[bcmIndex].bcmParam0) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("%s param0 (new trunk"
                                                       " hash key) %d,"
                                                       " but must be"
                                                       " (0..0xFFFFF)"),
                                      _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType],
                                      propActs[bcmIndex].bcmParam0));
                }
                hwAct[ppdIndex].hwType = SOC_PPD_FP_ACTION_TYPE_LAG_LB;
                hwAct[ppdIndex].hwParam = (propActs[bcmIndex].bcmParam0 &
                                           0xFFFFF);
                break;
            case bcmFieldActionDscpNew:
                /* param0 = new DSCP value */
                /* param1 = n/a */
                if (propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is ignored for %s"
                                         " action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                if (0xFF < propActs[bcmIndex].bcmParam0) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("%s param0 (new DSCP"
                                                       " value) %d,"
                                                       " but must be"
                                                       " (0..0xFF)"),
                                      _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType],
                                      propActs[bcmIndex].bcmParam0));
                }
                hwAct[ppdIndex].hwType = SOC_PPD_FP_ACTION_TYPE_IN_DSCP_EXP;
                hwAct[ppdIndex].hwParam = propActs[bcmIndex].bcmParam0 & 0xFF;
                break;
            case bcmFieldActionTtlSet:
                /* param0 = new TTL value */
                /* param1 = n/a */
                if (propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is ignored for %s"
                                         " action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                if (0xFF < propActs[bcmIndex].bcmParam0) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("%s param0 (new TTL"
                                                       " value) %d,"
                                                       " but must be"
                                                       " (0..0xFF)"),
                                      _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType],
                                      propActs[bcmIndex].bcmParam0));
                }
                hwAct[ppdIndex].hwType = SOC_PPD_FP_ACTION_TYPE_IN_TTL;
                hwAct[ppdIndex].hwParam = propActs[bcmIndex].bcmParam0 & 0xFF;
                break;
            case bcmFieldActionVSwitchNew:
                /* param0 = new virtual switching instance */
                /* param1 = n/a */
                if (propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is ignored for %s"
                                         " action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                if (0xFFFF < propActs[bcmIndex].bcmParam0) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("%s param0 (new VSI)"
                                                       " %d, but must be"
                                                       " (0..0xFFFF)"),
                                      _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType],
                                      propActs[bcmIndex].bcmParam0));
                }
                hwAct[ppdIndex].hwType = SOC_PPD_FP_ACTION_TYPE_VSI;
                hwAct[ppdIndex].hwParam = propActs[bcmIndex].bcmParam0 & 0xFFFF;
                break;
            case bcmFieldActionVlanActionSetNew:
                /* If simple mode */
                /* param0 = new VLAN translation action Id */
                /* param1 = (optional) new BCM_FIELD_VLAN_FORMAT_ */
                /* If advanced mode */
                /* param0 = VLAN-Edit-Command */
                /* param1 = n/a */
                
                if (SOC_DPP_IS_VLAN_TRANSLATE_MODE_ADVANCED(unitData->unitHandle)) {
                    /* Advanced user */
                    if (propActs[bcmIndex].bcmParam1) {
                        LOG_WARN(BSL_LS_BCM_FP,
                                 (BSL_META_U(unit,
                                             "param1(%d) is ignored for %s"
                                             " action\n"),
                                  propActs[bcmIndex].bcmParam1,
                                  _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                    }
                    if (SOC_DPP_NOF_INGRESS_VLAN_EDIT_ACTION_IDS(unitData->unitHandle) <= propActs[bcmIndex].bcmParam0) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                         (_BSL_BCM_MSG_NO_UNIT("%s param0 (new VLAN-Edit-Command)"
                                                           " %d, but must be"
                                                           " (0..63)"),
                                          _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType],
                                          propActs[bcmIndex].bcmParam0));
                    }
                    hwAct[ppdIndex].hwType = SOC_PPD_FP_ACTION_TYPE_VLAN_EDIT_COMMAND;
                    hwAct[ppdIndex].hwParam = propActs[bcmIndex].bcmParam0;
                }
                else { /* Simple VLAN translation mode */
                    vlan_format = SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_NONE;
                    if (propActs[bcmIndex].bcmParam1) {
                        BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_vlan_format_bcm_to_ppd(propActs[bcmIndex].bcmParam1,
                                                                                      &vlan_format));
                    }
                    /* Get the IVEC-Id */
                    
                    _bcm_petra_vlan_edit_command_hw_get(unit,
                                                        propActs[bcmIndex].bcmParam0,
                                                        vlan_format,
                                                        &ivec_id,
                                                        &is_ivec_to_set,
                                                        &is_vlan_format_valid);
                    hwAct[ppdIndex].hwType = SOC_PPD_FP_ACTION_TYPE_VLAN_EDIT_COMMAND;
                    hwAct[ppdIndex].hwParam = ivec_id;
                }
                break;
            case bcmFieldActionOuterVlanPrioNew:
                /* param0 = new VLAN PCP */
                /* param1 = new VLAN DEI */
                hwAct[ppdIndex].hwType = SOC_PPD_FP_ACTION_TYPE_VLAN_EDIT_PCP_DEI;
                hwAct[ppdIndex].hwParam = (propActs[bcmIndex].bcmParam1 & 0x1) /* DEI */
                    | ((propActs[bcmIndex].bcmParam0 & 0x7) << 1); /* PCP  */
                break;
            case bcmFieldActionOuterVlanNew:
            case bcmFieldActionInnerVlanNew:
                /* param0 = new VLAN Id */
                /* param1 = n/a */
                if (propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is ignored for %s"
                                         " action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                if (0xFFF < propActs[bcmIndex].bcmParam0) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("%s param0 (new VLAN-Id)"
                                                       " %d, but must be"
                                                       " (0..0xFFF)"),
                                      _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType],
                                      propActs[bcmIndex].bcmParam0));
                }
                /* Change Outer/Inner VLAN-ID if the VLAN-Edit-Command maps it */
                hwAct[ppdIndex].hwType = (propActs[bcmIndex].bcmType == bcmFieldActionInnerVlanNew)?
                    SOC_PPD_FP_ACTION_TYPE_VLAN_EDIT_VID_2: SOC_PPD_FP_ACTION_TYPE_VLAN_EDIT_VID_1;
                hwAct[ppdIndex].hwParam = propActs[bcmIndex].bcmParam0 & 0xFFF;
                break;
            case bcmFieldActionClassDestSet:
            case bcmFieldActionClassSourceSet:

                if ((groupData->groupFlags & _BCM_DPP_FIELD_GROUP_HANDLE_ENTRIES_BY_KEY)) {
                    hwAct[ppdIndex].hwParam = propActs[bcmIndex].bcmParam0;
                    hwAct[ppdIndex].hwType = SOC_PPD_FP_ACTION_TYPE_USER_HEADER_1;

                    hwAct[ppdIndex + 1].hwParam = propActs[bcmIndex].bcmParam1;
                    hwAct[ppdIndex + 1].hwType = SOC_PPD_FP_ACTION_TYPE_USER_HEADER_2;
                    actCount = 2;
                } else {
                    /* param0 = User-Header value (max 31 bits due to FES limitation to extract 32b including valid bit) */
                    /* param1 = n/a */
                    if (propActs[bcmIndex].bcmParam1) {
                        LOG_WARN(BSL_LS_BCM_FP,
                                 (BSL_META_U(unit,
                                             "param1(%d) is ignored for %s"
                                             " action\n"),
                                  propActs[bcmIndex].bcmParam1,
                                  _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                    }
                    if (SHR_BITGET(&propActs[bcmIndex].bcmParam0, 31)) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                         (_BSL_BCM_MSG_NO_UNIT("%s param0 (new User-Header)"
                                                           " %d, but must be"
                                                           " (0..0x7FFF_FFFF)"),
                                          _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType],
                                          propActs[bcmIndex].bcmParam0));
                    }
                    /* Do not check the input size since the action size is complex to know (SOC property dependent) */
                    hwAct[ppdIndex].hwType = (propActs[bcmIndex].bcmType == bcmFieldActionClassDestSet)?
                        SOC_PPD_FP_ACTION_TYPE_USER_HEADER_1: SOC_PPD_FP_ACTION_TYPE_USER_HEADER_2;
                    hwAct[ppdIndex].hwParam = propActs[bcmIndex].bcmParam0 & 0x7FFFFFFF; /* Extract only the 31 LSBs*/
                }
                break;
            case bcmFieldActionEcnNew:
                /* param0 = new CI value */
                /* param1 = n/a */
                if (propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is ignored for %s"
                                         " action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                if (0x3 < propActs[bcmIndex].bcmParam0) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("%s param0 (new CI)"
                                                       " %d, but must be"
                                                       " (0..0xFFFF)"),
                                      _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType],
                                      propActs[bcmIndex].bcmParam0));
                }
                /* ECN-Capable in bit 0, CNI in bit 1 */
                hwAct[ppdIndex].hwType = SOC_PPD_FP_ACTION_TYPE_FWD_HDR_ENCAPSULATION;
                hwAct[ppdIndex].hwParam = ((propActs[bcmIndex].bcmParam0?0x1:0x0) |
                                           ((0x3 == propActs[bcmIndex].bcmParam0)?2:0));
                break;
            case bcmFieldActionVportNew:
                /* param0 = vport */
                /* param1 = n/a */
                if (propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is ignored for %s"
                                         " action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                lookupOutLif[0] = propActs[bcmIndex].bcmParam0;
                hwAct[ppdIndex].hwType = SOC_PPD_FP_ACTION_TYPE_OUTLIF;
                break;
            case bcmFieldActionIntPriorityAndDropPrecedence:
                /* param0 = new traffic class value */
                /* param1 = new DP (BCM_FIELD_COLOR_*) */
                switch (propActs[bcmIndex].bcmParam1) {
                case BCM_FIELD_COLOR_GREEN:
                    hwAct[ppdIndex].hwParam = 0xC; /* set DP const = green */
                    break;
                case BCM_FIELD_COLOR_YELLOW:
                    hwAct[ppdIndex].hwParam = 0xD; /* set DP const = yellow */
                    break;
                case BCM_FIELD_COLOR_RED:
                    hwAct[ppdIndex].hwParam = 0xE; /* set DP const = red */
                    break;
                case BCM_FIELD_COLOR_BLACK:
                    hwAct[ppdIndex].hwParam = 0xF; /* set DP const = black */
                    break;
                default:
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("param1 value %d not"
                                                       " supported on unit %d"
                                                       " for action %s"),
                                      propActs[bcmIndex].bcmParam1,
                                      unit,
                                      _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                hwAct[ppdIndex].hwType = SOC_PPD_FP_ACTION_TYPE_DP;
                if (7 < propActs[bcmIndex].bcmParam0) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("%s param0 (new"
                                                       " Priority) %d,"
                                                       " but must be"
                                                       " (0..7)"),
                                      _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType],
                                      propActs[bcmIndex].bcmParam0));
                }
                hwAct[ppdIndex + 1].hwParam = ((1 << 3) |
                                     (propActs[bcmIndex].bcmParam0 & 0x7));
                hwAct[ppdIndex + 1].hwType = SOC_PPD_FP_ACTION_TYPE_TC;
                /* this takes two actions */
                actCount = 2;
                break;
            case bcmFieldActionPphPresentSet:
                /* param0 = PPH type if present*/
                /* param1 = N/A */
                if (propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is ignored for %s"
                                         " action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                /* Check the PPH type is in range 0..3 */
                if (3 < propActs[bcmIndex].bcmParam0) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("%s param0 (pph type)"
                                                       " %d, but must be (0..3)"),
                                      _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType],
                                      propActs[bcmIndex].bcmParam0));
                }
                hwAct[ppdIndex].hwType = SOC_PPD_FP_ACTION_TYPE_PPH_TYPE;
                hwAct[ppdIndex].hwParam = propActs[bcmIndex].bcmParam0;
                break;

            case bcmFieldActionFabricHeaderSet:
                /* param0 = System header profile*/
                /* param1 = N/A */
                if (propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is ignored for %s"
                                         " action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                hwAct[ppdIndex].hwType = SOC_PPD_FP_ACTION_TYPE_SYSTEM_HEADER_PROFILE_ID;
                switch (propActs[bcmIndex].bcmParam0) {
                case bcmFieldFabricHeaderEthernet:
                    hwAct[ppdIndex].hwParam = SOC_TMC_PMF_PGM_HEADER_PROFILE_ETHERNET; /* Ethenet Profile Header*/
                    break;
                case bcmFieldFabricHeaderTrafficManagementUcast:
                    hwAct[ppdIndex].hwParam = SOC_TMC_PMF_PGM_HEADER_PROFILE_TM_UNICAST;  /* TM Unicast Profile Header*/
                    break;
                case bcmFieldFabricHeaderTrafficManagementMcast:
                    hwAct[ppdIndex].hwParam = SOC_TMC_PMF_PGM_HEADER_PROFILE_TM_MULTICAST;  /* TM UnicasProfile Header*/
                    break;
                case bcmFieldFabricHeaderTrafficManagementVPort:
                    hwAct[ppdIndex].hwParam = SOC_TMC_PMF_PGM_HEADER_PROFILE_TM_OUTLIF; /* TM Outlif Profile Header*/
                    break;
                case bcmFieldFabricHeaderStacking:
                    hwAct[ppdIndex].hwParam = SOC_TMC_PMF_PGM_HEADER_PROFILE_STACKING; /*Stacking Profile Header*/
                    break;
                case bcmFieldFabricHeaderEthernetDSP:
                    hwAct[ppdIndex].hwParam = SOC_TMC_PMF_PGM_HEADER_PROFILE_ETHERNET_DSP; /* DSP Extension Profile Header*/
                    break;
                case bcmFieldFabricHeaderEthernetLearn:
                    hwAct[ppdIndex].hwParam = SOC_TMC_PMF_PGM_HEADER_PROFILE_ETHERNET_LEARN; /* DSP Extension Profile Header*/
                    break;
                default:
                    hwAct[ppdIndex].hwParam = SOC_TMC_PMF_PGM_HEADER_PROFILE_ETHERNET; /* Ethenet Profile Header*/
                    break;
                }
                break;

            case bcmFieldActionOam: /*new action*/
                /* param0 = {OAM-Up-Mep, OAM-Sub-Type, OAM-offset, OAM-Stamp-Offset} */
                /* param1 = N/A */
                if (propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is ignored for %s"
                                         " action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                hwAct[ppdIndex].hwType = SOC_PPD_FP_ACTION_TYPE_OAM;
                hwAct[ppdIndex].hwParam = propActs[bcmIndex].bcmParam0;
                break;
            case bcmFieldActionStackingRouteNew: 
                /* param0 = value of the stacking route history bitmap*/
                /* param1 = N/A */
                if (propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is ignored for %s"
                                         " action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                hwAct[ppdIndex].hwType = SOC_PPD_FP_ACTION_TYPE_STACK_RT_HIST;
                hwAct[ppdIndex].hwParam = ( propActs[bcmIndex].bcmParam0 & 0xFFFF );
                break;
            default:
                /* should not get here */
                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                                 (_BSL_BCM_MSG_NO_UNIT("unexpected action type"
                                                   " %d(%s) on unit %d group"
                                                   " %u entry %u"),
                                  propActs[bcmIndex].bcmType,
                                  _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType],
                                  unit,
                                  _BCM_DPP_FIELD_TCAM_ENTRY_GROUP(isExternalTcam, entry),
                                  _BCM_DPP_FIELD_TCAM_GLOBAL_ID(unit, entry, isExternalTcam)));
            } /* switch (propActs[index].bcmType) */
            /* Handle LIF lookups if needed */
            for (index = 0; index < actCount; index++) {
                if (BCM_GPORT_INVALID != lookupOutLif[index]) {
                    BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_gport_to_lif_convert(unit,
                                                                           lookupOutLif[index],
                                                                           (((propActs[index].bcmType)==bcmFieldActionIngressGportSet ) ? TRUE : FALSE ),
                                                                           &(hwAct[ppdIndex + index].hwParam)));
                    LOG_DEBUG(BSL_LS_BCM_FP,
                              (BSL_META_U(unit,
                                          "GPORT %08X lookup resulted"
                                          " in outlif %08X\n"),
                               lookupOutLif[index],
                               hwAct[ppdIndex + index].hwParam));
                } /* if (lookupOutLif[index]) */
            } /* for (index = 0; index < actCount; index++) */
            /* Handle GPORT lookups if needed */
            for (index = 0; index < actCount; index++) {
                if (BCM_GPORT_INVALID != lookupGport[index]) {
                    result = _bcm_dpp_gport_to_fwd_decision(unit,
                                                            lookupGport[index],
                                                            &fwdDecision);
                    if (BCM_E_NONE != result) {
                        LOG_ERROR(BSL_LS_BCM_FP,
                                  (BSL_META_U(unit,
                                              "unable to compute forwarding"
                                              " decision for unit %d GPORT"
                                              " %08X: %d (%s)\n"),
                                   unit,
                                   lookupGport[index],
                                   result,
                                   _SHR_ERRMSG(result)));
                        BCMDNX_IF_ERR_EXIT(result);
                    }
                    /* Get the HW Destination from the Forward decision */
                    COMPILER_64_SET(mask_edata, 0, (~0));
                    BCMDNX_IF_ERR_EXIT_MSG(_bcm_dpp_field_qualify_fwd_decision_to_hw_destination_convert(unit,
                                                                                                      &fwdDecision,
                                                                                                      TRUE, /* is_for_destination */
                                                                                                         TRUE, /* is_for_action */
                                                                                                         &dest_edata,
                                                                                                         &mask_edata),
                                           (_BSL_BCM_MSG_NO_UNIT("unable to get from"
                                                                 " the forward"
                                                                 " decision the"
                                                                 " HW data")));
                    /* The GPORT lookup can override the action type! */
                    switch (fwdDecision.type) {
                    case SOC_PPD_FRWRD_DECISION_TYPE_TRAP:
                        /*
                         * The lookup-gport can come from Redirect also
                         * Allow Trap only for bcmFieldActionTrap
                         */
                        if ((SOC_PPD_FP_ACTION_TYPE_TRAP ==
                                 hwAct[ppdIndex + index].hwType) || (SOC_PPD_FP_ACTION_TYPE_TRAP_REDUCED ==
                                                                         hwAct[ppdIndex + index].hwType)) {
                            BCMDNX_IF_ERR_EXIT_MSG(_bcm_dpp_field_trap_ppd_to_hw(unit,
                                                                                 fwdDecision.additional_info.trap_info.action_profile.trap_code,
                                                                                 &hwTrapCode),
                                                   (_BSL_BCM_MSG_NO_UNIT("unable to"
                                                                         " get from"
                                                                         " the HW"
                                                                         " Trap Id"
                                                                         " for PPD"
                                                                         " trap Id %d"
                                                                         " (%s)"),
                                                    fwdDecision.additional_info.trap_info.action_profile.trap_code,
                                                    SOC_PPD_TRAP_CODE_to_string(fwdDecision.additional_info.trap_info.action_profile.trap_code)));
                            /* Set the strength in 10:8 */
                            if (SOC_IS_ARAD(unit)) {
                                /* insert the trap qualifier in Arad */
                                hwAct[ppdIndex + index].hwParam = (hwAct[ppdIndex + index].hwParam << 11)
                                    + (fwdDecision.additional_info.trap_info.action_profile.frwrd_action_strength << 8) + hwTrapCode;
                            } else {
                                hwAct[ppdIndex + index].hwParam = hwTrapCode;
                                hwAct[ppdIndex + index].hwParam |= (fwdDecision.additional_info.trap_info.action_profile.frwrd_action_strength << 8);
                            }
                        } else {
                            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                                (_BSL_BCM_MSG_NO_UNIT("%s action can"
                                                                      " not accept"
                                                                      " GPORT %08X"),
                                                 _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType],
                                                 lookupGport[index]));
                        }

                        break; 

                    case SOC_PPD_FRWRD_DECISION_TYPE_DROP:
                        if (SOC_PPD_FP_ACTION_TYPE_DEST !=
                            hwAct[ppdIndex + index].hwType) {
                            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                             (_BSL_BCM_MSG_NO_UNIT("%s action can"
                                                               " not accept"
                                                               " GPORT %08X"),
                                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType],
                                              lookupGport[index]));
                        }
                        hwAct[ppdIndex + index].hwParam = _BCM_DPP_FIELD_DROP_DEST(unit);
                        break;
                    case SOC_PPD_FRWRD_DECISION_TYPE_UC_FLOW:
                    case SOC_PPD_FRWRD_DECISION_TYPE_UC_LAG:
                    case SOC_PPD_FRWRD_DECISION_TYPE_UC_PORT:
                    case SOC_PPD_FRWRD_DECISION_TYPE_MC:
                    case SOC_PPD_FRWRD_DECISION_TYPE_FEC:
                        /* all of these look like 'DEST' action */
                        /* Special cases */
						hwAct[ppdIndex + index].hwParam = COMPILER_64_LO(dest_edata);
                        if ((hwAct[ppdIndex + index].hwType == SOC_PPD_FP_ACTION_TYPE_SRC_SYST_PORT)
                            || (hwAct[ppdIndex + index].hwType == SOC_PPD_FP_ACTION_TYPE_LEARN_DATA_0_TO_15)) {
                            /* Take only the 16 LSBs*/
                            hwAct[ppdIndex + index].hwParam = hwAct[ppdIndex + index].hwParam & 0xFFFF;
                        }
                        break;
					default:
						hwAct[ppdIndex + index].hwParam = COMPILER_64_LO(dest_edata);
                        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                                         (_BSL_BCM_MSG_NO_UNIT("GPORT %08X lookup"
                                                           " resulted in"
                                                           " unsupported"
                                                           " decision type %d"),
                                          lookupGport[index],
                                          fwdDecision.type));
                    }
                    LOG_DEBUG(BSL_LS_BCM_FP,
                              (BSL_META_U(unit,
                                          "GPORT %08X lookup resulted in"
                                          " decision type %d -> action %d"
                                          " param %08X\n"),
                               lookupGport[index],
                               fwdDecision.type,
                               hwAct[ppdIndex + index].hwType,
                               (int)(hwAct[ppdIndex + index].hwParam)));
                } /* if (lookupGport[index]) */
            } /* for (index = 0; index < actCount; index++) */

        } else if (SOC_PPD_FP_DATABASE_STAGE_EGRESS ==
                   stageData->devInfo->hwStageId) {
            switch (propActs[bcmIndex].bcmType) {
            case bcmFieldActionDrop:
                /* param0 = n/a */
                /* param1 = n/a */
                /* egress drop: send to a trap profile configured to drop frames */
                if (propActs[bcmIndex].bcmParam0 ||
                    propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param0(%d) and param1(%d) are both"
                                         " ignored for %s action\n"),
                              propActs[bcmIndex].bcmParam0,
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                hwAct[ppdIndex].hwType = SOC_PPD_FP_ACTION_TYPE_EGR_TRAP;
                hwAct[ppdIndex].hwFlags = SOC_PPD_TRAP_ACTION_PROFILE_OVERWRITE_DEST;
                hwAct[ppdIndex].hwParam = SOC_PPD_TRAP_ACTION_PKT_DISCARD_ID;
                break;
            case bcmFieldActionStat:
#ifdef BCM_ARAD_SUPPORT
                if (SOC_IS_ARAD(unit)) 
                {
                    uint32 tm_port;
                    uint32 pp_port;
                    uint32 stat_id, stat_id_mask = 0, 
                        ace_pointer_prge_var_nof_bits=10,
                        ace_pointer_prge_var_in_stat_id_lsb=(_BCM_PETRA_CTR_PROC_SHIFT - ace_pointer_prge_var_nof_bits);

                    /* param0 = Stat-ID */
                    /* param1 = GPORT or port number */

                    if (BCM_GPORT_IS_SET(propActs[bcmIndex].bcmParam1)) {
                        /* gport mode */
                        lookupGport[0] = propActs[bcmIndex].bcmParam1;
                    } else { /* (BCM_GPORT_IS_SET(propActs[bcmIndex].bcmParam1)) */
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                         (_BSL_BCM_MSG_NO_UNIT(" non-GPORT"
                                                           " param1(%d) is not"
                                                           " valid gport"),
                                          propActs[bcmIndex].bcmParam1));
                        
                    } /* (BCM_GPORT_IS_SET(propActs[bcmIndex].bcmParam1)) */

                    /* Convert gport to TM-Port and PP-Port */
                    BCMDNX_IF_ERR_EXIT_MSG(_bcm_dpp_field_gport_to_pp_port_tm_port_convert(unit,
                                                                                        lookupGport[0],
                                                                                        &pp_port,
                                                                                        &tm_port),
                                        (_BSL_BCM_MSG_NO_UNIT("unable to get the TM"
                                                          " or PP port with"
                                                          " gport %08X"),
                                         lookupGport[0]));

                    /* Check the Stat-ID is in range: stat-id 0 means do no count */
                    SHR_BITSET_RANGE(&stat_id_mask, ace_pointer_prge_var_in_stat_id_lsb, ace_pointer_prge_var_nof_bits);
                    stat_id = propActs[bcmIndex].bcmParam0 & (~stat_id_mask);
                    if (stat_id != 0) 
                    {
                        result = _bcm_dpp_proc_cntr_from_stat(unitData,
                                                              stat_id,
                                                              &proc,
                                                              &cntr);
                        if (BCM_E_NONE != result) {
                            BCMDNX_IF_ERR_EXIT(result);
                        }

                        if ((cntr < _BCM_PETRA_CTR_BASE_VALUE_EGRESS_ARAD) 
                            || (cntr >= _BCM_PETRA_CTR_MAX_VALUE_EGRESS_ARAD)) {
                            /* does not use the correct ACE-Pointer, so error */
                            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                             (_BSL_BCM_MSG_NO_UNIT(" Stat-ID param0(%d)"
                                                               " is invalid. It must"
                                                               " in range %d..%d"),
                                              stat_id,
                                              _BCM_PETRA_CTR_BASE_VALUE_EGRESS_ARAD,
                                              (_BCM_PETRA_CTR_MAX_VALUE_EGRESS_ARAD - 1)));

                        }
                    } else {
                        /* For invalid stat, write the PP-Port in the last lines */
                        cntr = _BCM_PETRA_CTR_MAX_VALUE_EGRESS_ARAD + pp_port;
                    }
                    /* Get an ACE-Port to modify the PP-Port */
                    SOC_PPD_FP_CONTROL_INDEX_clear(&control_ndx);
                    SOC_PPD_FP_CONTROL_INFO_clear(&control_info);
                    control_ndx.type = SOC_PPD_FP_CONTROL_TYPE_ACE_POINTER_PP_PORT;
                    control_ndx.val_ndx = cntr;
                    control_info.val[0] = pp_port;
                    SHR_BITCOPY_RANGE(&(control_info.val[1]), 0, &(propActs[bcmIndex].bcmParam0), 
                                       ace_pointer_prge_var_in_stat_id_lsb, ace_pointer_prge_var_nof_bits);
                    ppdr = soc_ppd_fp_control_set(unitData->unitHandle,
                                                  SOC_CORE_INVALID,
                                                  &control_ndx,
                                                  &control_info,
                                                  &success);
                    result = handle_sand_result(ppdr);
                    if (BCM_E_NONE == result) {
                        result = translate_sand_success_failure(success);
                    }
                    BCMDNX_IF_ERR_EXIT(result);
                    hwAct[ppdIndex].hwType = SOC_PPD_FP_ACTION_TYPE_ACE_POINTER;
                    /* In Arad, LSB indicates if valid */
                    hwAct[ppdIndex].hwParam = cntr;
                    /* takes a single action when not counting */
                    actCount = 1;
                    if (propActs[bcmIndex].bcmParam0 != 0) 
                    {
                        hwAct[ppdIndex + 1].hwType = SOC_PPD_FP_ACTION_TYPE_COUNTER_PROFILE;
                        hwAct[ppdIndex + 1].hwParam = 0x1; /* Counter-ID = ACE-Pointer */
                        /* takes two actions */
                        actCount = 2;
                    }
                } else
#endif /* def BCM_ARAD_SUPPORT */
                {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                                     (_BSL_BCM_MSG_NO_UNIT("unit %d does not know"
                                                       " how to convert"
                                                       " policer ID on"
                                                       " unknown hardware"),
                                      unit));
                }

                break;
#ifdef BCM_ARAD_SUPPORT
            case bcmFieldActionDropPrecedence:
                /* param0 = new DP (BCM_FIELD_COLOR_*) */
                /* param1 = n/a */
                if (propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is ignored for %s"
                                         " action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                /* Translate to HW Drop precedence */
                BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_color_bcm_to_ppd(propActs[bcmIndex].bcmParam0,
                                                                        &(hwAct[ppdIndex].hwParam)));
                hwAct[ppdIndex].hwType = SOC_PPD_FP_ACTION_TYPE_EGR_TRAP;
                hwAct[ppdIndex].hwFlags = SOC_PPD_TRAP_ACTION_PROFILE_OVERWRITE_DP;
                break;
#endif /* def BCM_ARAD_SUPPORT */
            case bcmFieldActionRedirect:
                /* param0 = module */
                /* param1 = GPORT or port number */
                /* egress redirect, only supports local port */
                if (BCM_GPORT_IS_SET(propActs[bcmIndex].bcmParam1)) {
                    /* gport mode */
                    lookupGport[0] = propActs[bcmIndex].bcmParam1;
                } else { /* (BCM_GPORT_IS_SET(propActs[bcmIndex].bcmParam1)) */
                    /* raw module,port mode */
                    if (myModId != propActs[bcmIndex].bcmParam0) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                         (_BSL_BCM_MSG_NO_UNIT("unit %d module %d"
                                                           " does not support"
                                                           " egress redirect"
                                                           " to other module"),
                                          unit,
                                          myModId));
                    }
                    if ((_SHR_GPORT_PORT_MASK <
                         propActs[bcmIndex].bcmParam1) ||
                        (_SHR_GPORT_MODID_MASK <
                         propActs[bcmIndex].bcmParam0)) {
                        /* can't represent it as modport, so error */
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                         (_BSL_BCM_MSG_NO_UNIT("non-GPORT"
                                                           " param0(%d) is not"
                                                           " valid modid"
                                                           " (0..%d) or"
                                                           " param1(%d) is not"
                                                           " valid port"
                                                           " (0..%d)"),
                                          propActs[bcmIndex].bcmParam0,
                                          _SHR_GPORT_MODID_MASK,
                                          propActs[bcmIndex].bcmParam1,
                                          _SHR_GPORT_PORT_MASK));
                    }
                    /* build modport from these arguments */
                    BCM_GPORT_MODPORT_SET(lookupGport[0],
                                          propActs[bcmIndex].bcmParam0,
                                          propActs[bcmIndex].bcmParam1);
                } /* (BCM_GPORT_IS_SET(propActs[bcmIndex].bcmParam1)) */
                hwAct[ppdIndex].hwType = SOC_PPD_FP_ACTION_TYPE_EGR_OFP;
#ifdef BCM_ARAD_SUPPORT
                if (SOC_IS_ARAD(unit)) {
                    /* Convert gport to TM-Port and PP-Port */
                    BCMDNX_IF_ERR_EXIT_MSG(_bcm_dpp_field_gport_to_pp_port_tm_port_convert(unit,
                                                                                        lookupGport[0],
                                                                                        &(hwAct[ppdIndex + 1].hwParam),
                                                                                        &(hwAct[ppdIndex].hwParam)),
                                        (_BSL_BCM_MSG_NO_UNIT("unable to get the TM"
                                                          " or PP port with"
                                                          " gport %08X"),
                                         lookupGport[0]));
                }
#endif /* def BCM_ARAD_SUPPORT */
                break;
#ifdef BCM_PETRAB_SUPPORT
            case bcmFieldActionVportNew:
                /* Only Soc_petra-B */
                /* param0 = vport */
                /* param1 = n/a */
                if (propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is ignored for %s"
                                         " action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                result = _bcm_dpp_field_gport_to_lif_convert(unit,
                                                             propActs[bcmIndex].bcmParam0,
                                                             FALSE,
                                                             &(hwAct[ppdIndex].hwParam));
                BCMDNX_IF_ERR_EXIT(result);
                LOG_DEBUG(BSL_LS_BCM_FP,
                          (BSL_META_U(unit,
                                      "GPORT %08X lookup resulted in outlif"
                                      " %08X\n"),
                           propActs[bcmIndex].bcmParam0,
                           hwAct[ppdIndex].hwParam));
                hwAct[ppdIndex].hwType = SOC_PPD_FP_ACTION_TYPE_EGR_OUTLIF;
                break;
#endif
            case bcmFieldActionRedirectVportPort:
                /* param0 = vport */
                /* param1 = port */

                if (BCM_GPORT_IS_SET(propActs[bcmIndex].bcmParam1)) {
                    /* gport mode */
                    lookupGport[0] = propActs[bcmIndex].bcmParam1;
                } else {
                    /* raw port mode */
                    /* build modport from local port */
                    if (_SHR_GPORT_PORT_MASK < propActs[bcmIndex].bcmParam1) {
                        /* can't represent it as modport, so error */
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                         (_BSL_BCM_MSG_NO_UNIT("non-GPORT param1"
                                                           " %d is not valid"
                                                           " port (0..%d)"),
                                          propActs[bcmIndex].bcmParam1,
                                          _SHR_GPORT_PORT_MASK));
                    }
                    BCM_GPORT_MODPORT_SET(lookupGport[0],
                                          myModId,
                                          propActs[bcmIndex].bcmParam1);
                }

#ifdef BCM_ARAD_SUPPORT
                if (SOC_IS_ARAD(unit))
                {
                    uint32 tm_port;
                    uint32 pp_port;

                    result = _bcm_dpp_field_gport_to_lif_convert(unit,
                                                                 propActs[bcmIndex].bcmParam0,
                                                                 FALSE,
                                                                 &(hwAct[ppdIndex].hwParam));
                    BCMDNX_IF_ERR_EXIT(result);

                    LOG_DEBUG(BSL_LS_BCM_FP,
                              (BSL_META_U(unit,
                                          "GPORT %08X lookup resulted in outlif %08X\n"),
                               propActs[bcmIndex].bcmParam0,
                               hwAct[ppdIndex].hwParam));

                    BCMDNX_IF_ERR_EXIT_MSG(_bcm_dpp_field_gport_to_pp_port_tm_port_convert(unit,
                                                                                        lookupGport[0],
                                                                                        &pp_port,
                                                                                        &tm_port),
                                        (_BSL_BCM_MSG_NO_UNIT("unable to get the TM"
                                                          " or PP port with"
                                                          " gport %08X"),
                                         lookupGport[0]));

                    hwAct[ppdIndex + 1].hwParam = tm_port;

                    /* Use the action-profile action instead in Arad - no simple Out-LIF action */
                    hwAct[ppdIndex].hwType = SOC_PPD_FP_ACTION_TYPE_EGR_TRAP;
                    hwAct[ppdIndex].hwFlags = SOC_PPD_TRAP_ACTION_PROFILE_OVERWRITE_CUD;

                    hwAct[ppdIndex + 1].hwType = SOC_PPD_FP_ACTION_TYPE_EGR_TRAP;
                    hwAct[ppdIndex + 1].hwFlags = SOC_PPD_TRAP_ACTION_PROFILE_OVERWRITE_DEST;

                    actCount = 2;
                }
#endif /* def BCM_ARAD_SUPPORT */

#ifdef BCM_PETRAB_SUPPORT
                if (SOC_IS_PETRAB(unit))
                {
                    
                    BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_gport_to_lif_convert(unit,
                                                                                propActs[bcmIndex].bcmParam0,
                                                                                FALSE,
                                                                                &(hwAct[ppdIndex + 1].hwParam)));
                    LOG_DEBUG(BSL_LS_BCM_FP,
                              (BSL_META_U(unit,
                                          "GPORT %08X lookup resulted in outlif"
                                          " %08X\n"),
                               propActs[bcmIndex].bcmParam0,
                               hwAct[ppdIndex + 1].hwParam));
                    hwAct[ppdIndex].hwType = SOC_PPD_FP_ACTION_TYPE_EGR_OFP;
                    hwAct[ppdIndex + 1].hwType = SOC_PPD_FP_ACTION_TYPE_EGR_OUTLIF;
                    /* this takes two actions */
                    actCount = 2;
                }
#endif /* def BCM_PETRAB_SUPPORT */
                break;

#ifdef BCM_PETRAB_SUPPORT
            case bcmFieldActionIntPriorityAndDropPrecedence:
                /* param0 = new traffic class value */
                /* param0 = new DP (BCM_FIELD_COLOR_*) */
                if (7 < propActs[bcmIndex].bcmParam0) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("%s param0 (new"
                                                       " priority) %d, but"
                                                       " must be (0..7)"),
                                      _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType],
                                      propActs[bcmIndex].bcmParam0));
                }
                hwAct[ppdIndex].hwParam = propActs[bcmIndex].bcmParam0 & 0x7;
                
                switch (propActs[bcmIndex].bcmParam1) {
                case BCM_FIELD_COLOR_GREEN:
                    hwAct[ppdIndex].hwParam |= 0x0; /* set DP to indirect 0 = green */
                    break;
                case BCM_FIELD_COLOR_YELLOW:
                    hwAct[ppdIndex].hwParam |= 0x8; /* set DP to indirect 1 = yellow */
                    break;
                default:
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("param1 value %d is not"
                                                       " supported on unit %d"
                                                       " for action %s"),
                                      propActs[bcmIndex].bcmParam1,
                                      unit,
                                      _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                hwAct[ppdIndex].hwType = SOC_PPD_FP_ACTION_TYPE_EGR_TC_DP;
                break;
#endif /* def BCM_PETRAB_SUPPORT */
#ifdef BCM_ARAD_SUPPORT
            case bcmFieldActionPrioIntNew:
                /* param0 = new priority (internal only?) */
                /* param1 (misdoc as N/A?) contains marking flags(???) */
                if (propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is ignored for %s"
                                         " action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                if (7 < propActs[bcmIndex].bcmParam0) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("%s param0 (new priority)"
                                                       " %d, but must be (0..7)"),
                                      _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType],
                                      propActs[bcmIndex].bcmParam0));
                }
                hwAct[ppdIndex].hwParam = (1 /* Valid bit */ |
                                           ((propActs[bcmIndex].bcmParam0 &
                                             0x7) << 1)) /* TC << 1 */;
                hwAct[ppdIndex].hwType = SOC_PPD_FP_ACTION_TYPE_TC;
                break;
#endif /* def BCM_ARAD_SUPPORT */
#ifdef BCM_ARAD_SUPPORT
            case bcmFieldActionMirrorEgress:
                /* param0 = destination module (ignored if port is a GPORT) */
                /* param1 = destination port OR GPORT */
                if (BCM_GPORT_IS_SET(propActs[bcmIndex].bcmParam1) &&
                    BCM_GPORT_IS_MIRROR(propActs[bcmIndex].bcmParam1)) {
                    /*
                     *  Using GPORT mode and the GPORT is a mirror
                     */
                    mirrorId = BCM_GPORT_MIRROR_GET(propActs[bcmIndex].bcmParam1);
                } else {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                                     (_BSL_BCM_MSG_NO_UNIT("unit %d does not allow"
                                                       " implicit mirror"
                                                       " allocations.  Param1"
                                                       " %08X must be (but is"
                                                       " not) mirror gport\n"),
                                      unit,
                                      propActs[bcmIndex].bcmParam1));
                }
                hwAct[ppdIndex].hwType = SOC_PPD_FP_ACTION_TYPE_MIRROR;
                hwAct[ppdIndex].hwParam = mirrorId;
                break;
#endif /* def BCM_ARAD_SUPPORT */
            case bcmFieldActionQosMapIdNew:
                /* In Arad, indicated as useless.. */
                /* param0 = QoS Map Id of type egress */
                /* param1 = N/A */
                if (propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is ignored for %s"
                                         " action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                /* Check the QOS map profile is of type Egress */
                if (!BCM_INT_QOS_MAP_IS_EGRESS(propActs[bcmIndex].bcmParam0)) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param0(%d) must be type Egress for"
                                         " %s action\n"),
                              propActs[bcmIndex].bcmParam0,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                hwAct[ppdIndex].hwType = SOC_PPD_FP_ACTION_TYPE_COS_PROFILE;
                hwAct[ppdIndex].hwParam = BCM_QOS_MAP_PROFILE_GET(propActs[bcmIndex].bcmParam0) & 0xF; /* Encoded in 4 bits */
                break;
            default:
                /* should not get here */
                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                                 (_BSL_BCM_MSG_NO_UNIT("unexpected action type"
                                                   " %d(%s) on unit %d group"
                                                   " %u entry %u"),
                                  propActs[bcmIndex].bcmType,
                                  _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType],
                                  unit,
                                  _BCM_DPP_FIELD_TCAM_ENTRY_GROUP(isExternalTcam, entry),
                                  _BCM_DPP_FIELD_TCAM_GLOBAL_ID(unit, entry, isExternalTcam)));
            } /* switch (propActs[index].bcmType) */
            for (index = 0; index < actCount; index++) {
                
#ifdef BCM_ARAD_SUPPORT
                if (SOC_IS_ARAD(unit) &&
                    (hwAct[ppdIndex + index].hwType ==
                     SOC_PPD_FP_ACTION_TYPE_EGR_OFP)) {
                    int rv, core;
                    uint32 pp_port;
                    /* Map from local-port to PP-Port to Base-Q-Pair */
                    rv = soc_port_sw_db_local_to_pp_port_get(unit,
                                                          hwAct[ppdIndex + index].hwParam, &pp_port, &core);
                    if (BCM_FAILURE(rv)) { /* function returns valid port number or error ID */
                        BCMDNX_ERR_EXIT_MSG(hwAct[ppdIndex + index].hwParam,
                                         (_BSL_BCM_MSG_NO_UNIT("unable to map port"
                                                           "%d (%08X) to tm"
                                                           "port"),
                                          hwAct[ppdIndex + index].hwParam,
                                          hwAct[ppdIndex + index].hwParam));
                    }
                    rv = soc_port_sw_db_tm_port_to_base_q_pair_get(unitData->unitHandle, core, pp_port, &hwAct[ppdIndex + index].hwParam);
                    if (BCM_FAILURE(rv)) { /* function returns valid port number or error ID */
                        BCMDNX_ERR_EXIT_MSG(hwAct[ppdIndex + index].hwParam,
                                         (_BSL_BCM_MSG_NO_UNIT("unable to map port"
                                                           "%d (%08X) to base queue"
                                                           "pair"),
                                          hwAct[ppdIndex + index].hwParam,
                                          hwAct[ppdIndex + index].hwParam));
                    }
                }
#endif /* BCM_ARAD_SUPPORT */
            } /* for (index = 0; index < actCount; index++) */
        } else if (SOC_PPD_FP_DATABASE_STAGE_INGRESS_FLP ==
                   stageData->devInfo->hwStageId) {
            switch (propActs[bcmIndex].bcmType) {
            case bcmFieldActionExternalValue0Set:
            case bcmFieldActionExternalValue1Set:
            case bcmFieldActionExternalValue2Set:
            case bcmFieldActionExternalValue3Set:
                /* param0 = Value */
                /* param1 = n/a */
                if (propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is"
                                         " ignored for %s action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                switch (propActs[bcmIndex].bcmType) {
                case bcmFieldActionExternalValue0Set:
                    hwAct[ppdIndex].hwType = SOC_PPD_FP_ACTION_TYPE_FLP_ACTION_0;
                    break;
                case bcmFieldActionExternalValue1Set:
                    hwAct[ppdIndex].hwType = SOC_PPD_FP_ACTION_TYPE_FLP_ACTION_1;
                    break;
                case bcmFieldActionExternalValue2Set:
                    hwAct[ppdIndex].hwType = SOC_PPD_FP_ACTION_TYPE_FLP_ACTION_2;
                    break;
                default:
                case bcmFieldActionExternalValue3Set:
                    hwAct[ppdIndex].hwType = SOC_PPD_FP_ACTION_TYPE_FLP_ACTION_3;
                    break;
                }
                /* Limited to 32b */
                hwAct[ppdIndex].hwParam = propActs[bcmIndex].bcmParam0;
                break;
            default:
                /* should not get here */
                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                                 (_BSL_BCM_MSG_NO_UNIT("unexpected action type"
                                                   " %d(%s) on unit %d group"
                                                   " %u entry %u"),
                                  propActs[bcmIndex].bcmType,
                                  _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType],
                                  unit,
                                  _BCM_DPP_FIELD_TCAM_ENTRY_GROUP(isExternalTcam, entry),
                                  _BCM_DPP_FIELD_TCAM_GLOBAL_ID(unit, entry, isExternalTcam)));
            } /* switch (propActs[index].bcmType) */
        } else {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d entry %u group %u"
                                               " stage %u is unsupported"
                                               " hardware stage type %d"),
                              unit,
                              _BCM_DPP_FIELD_TCAM_GLOBAL_ID(unit, entry, isExternalTcam),
                              _BCM_DPP_FIELD_TCAM_ENTRY_GROUP(isExternalTcam, entry),
                              groupData->stage,
                              stageData->devInfo->hwStageId));
        }
        /* go to the next available PPD slot */
        if (ppdIndex + actCount <= SOC_PPD_FP_NOF_ACTIONS_PER_DB_MAX) {
            while (actCount) {
                LOG_DEBUG(BSL_LS_BCM_FP,
                          (BSL_META_U(unit,
                                      "unit %d entry %u group %u stage %u"
                                      " BCM action %u (%s(%d):%u(%08X),"
                                      "%u(%08X)) mapped to PPD action %u"
                                      " (%s(%d)%08X:%u(%08X))\n"),
                           unit,
                           _BCM_DPP_FIELD_TCAM_GLOBAL_ID(unit, entry, isExternalTcam),
                           _BCM_DPP_FIELD_TCAM_ENTRY_GROUP(isExternalTcam, entry),
                           groupData->stage,
                           bcmIndex,
                           _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType],
                           propActs[bcmIndex].bcmType,
                           propActs[bcmIndex].bcmParam0,
                           propActs[bcmIndex].bcmParam0,
                           propActs[bcmIndex].bcmParam1,
                           propActs[bcmIndex].bcmParam1,
                           ppdIndex,
                           SOC_PPD_FP_ACTION_TYPE_to_string(hwAct[ppdIndex].hwType),
                           hwAct[ppdIndex].hwType,
                           hwAct[ppdIndex].hwFlags,
                           hwAct[ppdIndex].hwParam,
                           hwAct[ppdIndex].hwParam));
                ppdIndex++;
                actCount--;
            }
        } else {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d entry %u has too many"
                                               " hardware actions (max %u)"),
                              unit,
                              _BCM_DPP_FIELD_TCAM_GLOBAL_ID(unit, entry, isExternalTcam),
                              SOC_PPD_FP_NOF_ACTIONS_PER_DB_MAX));
        }
        /* this BCM action has been considered */
        SHR_BITSET(actHit, bcmIndex);
    } /* for (all BCM layer action slots) */

    /* make sure there are no duplicate actions */
    for (ppdIndex = 0; ppdIndex < SOC_PPD_FP_NOF_ACTIONS_PER_DB_MAX; ppdIndex++) {
        for (index = ppdIndex + 1;
             index < SOC_PPD_FP_NOF_ACTIONS_PER_DB_MAX;
             index++) {
            if (!_BCM_DPP_FIELD_PPD_ACTION_VALID(hwAct[ppdIndex].hwType)) {
                /* not an action; skip this one */
                continue;
            }
            if (hwAct[ppdIndex].hwType != hwAct[index].hwType) {
                /* not the same action; skip this one */
                continue;
            }
            if ((SOC_PPD_FP_ACTION_TYPE_EGR_TRAP == hwAct[ppdIndex].hwType) &&
                (hwAct[ppdIndex].hwFlags != hwAct[index].hwFlags)) {
                /* not the same underlying action; skip this one */
                continue;
            }
            /* two hardware actions of the same type */
            if (hwAct[ppdIndex].hwParam != hwAct[index].hwParam) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                                 (_BSL_BCM_MSG_NO_UNIT("unit %d entry %u"
                                                   " hardware actions %u"
                                                   " and %u are both the"
                                                   " same action %s (%d) %08X"
                                                   " but their parameters"
                                                   " differ (%08X, %08X)"),
                                  unit,
                                  _BCM_DPP_FIELD_TCAM_GLOBAL_ID(unit, entry, isExternalTcam),
                                  ppdIndex,
                                  index,
                                  SOC_PPD_FP_ACTION_TYPE_to_string(hwAct[ppdIndex].hwType),
                                  hwAct[ppdIndex].hwType,
                                  hwAct[ppdIndex].hwFlags,
                                  hwAct[ppdIndex].hwParam,
                                  hwAct[index].hwParam));
            } else { /* if (hardware action params not equal) */
                /* but we don't want two of it even if duplicate */
                LOG_WARN(BSL_LS_BCM_FP,
                         (BSL_META_U(unit,
                                     "unit %d entry %u has two hardware"
                                     " actions %d and %d of type %s (%d)"
                                     " %08X with same parameter %08X\n"),
                          unit,
                          _BCM_DPP_FIELD_TCAM_GLOBAL_ID(unit, entry, isExternalTcam),
                          ppdIndex,
                          index,
                          SOC_PPD_FP_ACTION_TYPE_to_string(hwAct[ppdIndex].hwType),
                          hwAct[ppdIndex].hwType,
                          hwAct[ppdIndex].hwFlags,
                          hwAct[ppdIndex].hwParam));
                hwAct[index].hwType = SOC_PPD_FP_ACTION_TYPE_INVALID;
                hwAct[index].hwParam = ~0;
                hwAct[index].hwFlags = 0;
            } /* if (hardware action params not equal) */
        } /* for (all hardware actions after the current one) */
    } /* for (all hardware actions) */

    /* Everything went well to this point, commit the changes. */
    commit = TRUE;
exit:
    /* Must not abort function on error from here on */
    if (commit) {
        /* Will release any resources used by the old actions */
        relActs = _BCM_DPP_FIELD_TCAM_ENTRY_ACTP(isExternalTcam, entry);
        keepActs = &(hwAct[0]);
    } else { /* if (commit) */
        /* Will release any resources used by the new actions */
        relActs = &(hwAct[0]);
        keepActs = _BCM_DPP_FIELD_TCAM_ENTRY_ACTP(isExternalTcam, entry);
    } /* if (commit) */

    
    /*
     *  NOTE: errors that occur in this loop must not cause immediate exit; it
     *  might be reasonable to report them in some way, but everything here is
     *  freeing a resource so should not fail unless internal state is
     *  otherwise inconsistent.
     */
    for (ppdIndex = 0; ppdIndex < SOC_PPD_FP_NOF_ACTIONS_PER_DB_MAX; ppdIndex++) {
        /* Look for the same hardware action in the 'keep' set */
        for (index = 0; index < SOC_PPD_FP_NOF_ACTIONS_PER_DB_MAX; index++) {
            if (keepActs[index].hwType ==
                relActs[ppdIndex].hwType) {
                /* this is the same hardware action */
                break;
            }
        }
        switch (relActs[ppdIndex].hwType) {
#ifdef BCM_PETRAB_SUPPORT
        case SOC_PPD_FP_ACTION_TYPE_MIRROR:
            if (SOC_IS_PETRAB(unit)) {
                /*
                 *  Soc_petraB mirrors track reference counts, so even if we are
                 *  just keeping the same mirror, we need to release the
                 *  previous reference to it
                 */
                result = bcm_petra_ingress_mirror_free(unit,
                                                       relActs[ppdIndex].hwParam);
            }
#endif /* def BCM_PETRAB_SUPPORT */
        default:
            /* most actions do not need special resource work */
            break;
        } /* switch (relActs[ppdIndex].hwType) */
    } /* for (all actions in the released actions) */

    if (commit) {
        LOG_DEBUG(BSL_LS_BCM_FP,
                  (BSL_META_U(unit,
                              "unit %d entry %u BCM actions successfully"
                              " converted to PPD actions; committing the"
                              " changes to the entry\n"),
                   unit,
                   _BCM_DPP_FIELD_TCAM_GLOBAL_ID(unit, entry, isExternalTcam)));
        for (index = 0; index < _BCM_DPP_NOF_BCM_ACTIONS_PER_DB_MAX; index++) {
            _BCM_DPP_FIELD_TCAM_ENTRY_ACTB(isExternalTcam, entry)[index].bcmType = propActs[index].bcmType;
            _BCM_DPP_FIELD_TCAM_ENTRY_ACTB(isExternalTcam, entry)[index].bcmParam0 = propActs[index].bcmParam0;
            _BCM_DPP_FIELD_TCAM_ENTRY_ACTB(isExternalTcam, entry)[index].bcmParam1 = propActs[index].bcmParam1;
        }
        for (index = 0; index < SOC_PPD_FP_NOF_ACTIONS_PER_DB_MAX; index++) {
            _BCM_DPP_FIELD_TCAM_ENTRY_ACTP(isExternalTcam, entry)[index].hwType = hwAct[index].hwType;
            _BCM_DPP_FIELD_TCAM_ENTRY_ACTP(isExternalTcam, entry)[index].hwParam = hwAct[index].hwParam;
            _BCM_DPP_FIELD_TCAM_ENTRY_ACTP(isExternalTcam, entry)[index].hwFlags = hwAct[index].hwFlags;
        }
    } /* if (commit) */
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_field_tcam_entry_action_clear(_bcm_dpp_field_info_t *unitData,
                                       uint8 isExternalTcam,
                                       _bcm_dpp_field_ent_idx_t entry,
                                       bcm_field_action_t type)
{
    _bcm_dpp_field_tc_b_act_t tempActs[_BCM_DPP_NOF_BCM_ACTIONS_PER_DB_MAX];
    unsigned int index;
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;
    
    LOG_VERBOSE(BSL_LS_BCM_FP,
                (BSL_META_U(unit,
                            "(%d, %u, %d(%s))\n"),
                 unit,
                 _BCM_DPP_FIELD_TCAM_GLOBAL_ID(unit, entry, isExternalTcam),
                 type,
                 (type<bcmFieldActionCount)?_bcm_dpp_field_action_name[type]:"???"));
    if ((0 > type) || (bcmFieldActionCount <= type)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG_NO_UNIT("action type %d not valid"), type));
    }
    if (0 == (_BCM_DPP_FIELD_TCAM_ENTRY_FLAGS(isExternalTcam, entry) & _BCM_DPP_FIELD_ENTRY_IN_USE)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d entry %u not in use"),
                          unit,
                          _BCM_DPP_FIELD_TCAM_GLOBAL_ID(unit, entry, isExternalTcam)));
    }
    if (!BCM_FIELD_ASET_TEST(unitData->groupD[_BCM_DPP_FIELD_TCAM_ENTRY_GROUP(isExternalTcam, entry)].aset,
                             type)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d (group %d) ASET"
                                           " does not include %d(%s)"),
                          unit,
                          entry,
                          _BCM_DPP_FIELD_TCAM_ENTRY_GROUP(isExternalTcam, entry),
                          type,
                          _bcm_dpp_field_action_name[type]));
    }
    sal_memcpy(&(tempActs[0]),
               _BCM_DPP_FIELD_TCAM_ENTRY_ACTB(isExternalTcam, entry),
               sizeof(tempActs[0]) * _BCM_DPP_NOF_BCM_ACTIONS_PER_DB_MAX);
    for (index = 0;
         index < _BCM_DPP_NOF_BCM_ACTIONS_PER_DB_MAX;
         index++) {
        if (type == tempActs[index].bcmType) {
            LOG_DEBUG(BSL_LS_BCM_FP,
                      (BSL_META_U(unit,
                                  "unit %d entry %u action %d, %s(%d)"
                                  " %d(%08X) %d(%08X) weill be removed\n"),
                       unit,
                       _BCM_DPP_FIELD_TCAM_GLOBAL_ID(unit, entry, isExternalTcam),
                       index,
                       _bcm_dpp_field_action_name[tempActs[index].bcmType],
                       tempActs[index].bcmType,
                       tempActs[index].bcmParam0,
                       tempActs[index].bcmParam0,
                       tempActs[index].bcmParam1,
                       tempActs[index].bcmParam1));
            break;
        }
    }
    if (index < _BCM_DPP_NOF_BCM_ACTIONS_PER_DB_MAX) {
        tempActs[index].bcmType = bcmFieldActionCount;
        tempActs[index].bcmParam0 = 0;
        tempActs[index].bcmParam1 = 0;
    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d entry %u does not have"
                                           " action %s(%d)"),
                          unit,
                          _BCM_DPP_FIELD_TCAM_GLOBAL_ID(unit, entry, isExternalTcam),
                          _bcm_dpp_field_action_name[type],
                          type));
    }
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_tcam_entry_action_update(unitData,
                                                                    isExternalTcam,
                                                                    entry,
                                                                    &(tempActs[0])));
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_field_tcam_entry_clear_acts(_bcm_dpp_field_info_t *unitData,
                                     uint8 isExternalTcam,
                                     _bcm_dpp_field_ent_idx_t entry)
{
    _bcm_dpp_field_tc_b_act_t tempActs[_BCM_DPP_NOF_BCM_ACTIONS_PER_DB_MAX];
    unsigned int index;
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;

    LOG_VERBOSE(BSL_LS_BCM_FP,
                (BSL_META_U(unit,
                            "(%d, %u)\n"),
                 unit,
                 _BCM_DPP_FIELD_TCAM_GLOBAL_ID(unit, entry, isExternalTcam)));
    if (0 == (_BCM_DPP_FIELD_TCAM_ENTRY_FLAGS(isExternalTcam, entry) & _BCM_DPP_FIELD_ENTRY_IN_USE)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d entry %u not in use"),
                          unit,
                          _BCM_DPP_FIELD_TCAM_GLOBAL_ID(unit, entry, isExternalTcam)));
    }
    sal_memcpy(&(tempActs[0]),
               _BCM_DPP_FIELD_TCAM_ENTRY_ACTB(isExternalTcam, entry),
               sizeof(tempActs[0]) * _BCM_DPP_NOF_BCM_ACTIONS_PER_DB_MAX);
    for (index = 0;
         index < _BCM_DPP_NOF_BCM_ACTIONS_PER_DB_MAX;
         index++) {
        tempActs[index].bcmType = bcmFieldActionCount;
        tempActs[index].bcmParam0 = 0;
        tempActs[index].bcmParam1 = 0;
    }
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_tcam_entry_action_update(unitData,
                                                                    isExternalTcam,
                                                                    entry,
                                                                    &(tempActs[0])));
exit:
    BCMDNX_FUNC_RETURN;
}

void
_bcm_dpp_field_tcam_entry_clear(_bcm_dpp_field_info_t *unitData,
                                uint8 isExternalTcam,
                           _bcm_dpp_field_ent_idx_t entry)
{
    _bcm_dpp_field_ent_idx_t next;
    _bcm_dpp_field_ent_idx_t prev;
    _bcm_dpp_field_ent_idx_t *entryIdx;
    int *entryPriority;
    unsigned int index;
    uint32 ext_info_idx = 0;

    next = _BCM_DPP_FIELD_TCAM_ENTRY_NEXT(isExternalTcam, entry);
    prev = _BCM_DPP_FIELD_TCAM_ENTRY_PREV(isExternalTcam, entry);
    if (isExternalTcam) {
        ext_info_idx = unitData->entryExtTc[entry].ext_info_idx;
    }
    sal_memset((void *)_BCM_DPP_FIELD_TCAM_ENTRY(isExternalTcam, entry), 
               0x00, 
               isExternalTcam ? sizeof(_bcm_dpp_field_entry_ext_t) : sizeof(_bcm_dpp_field_entry_t));
    entryIdx = _BCM_DPP_FIELD_TCAM_ENTRY_NEXT_PTR(isExternalTcam, entry);
    *entryIdx = next;
    entryIdx = _BCM_DPP_FIELD_TCAM_ENTRY_PREV_PTR(isExternalTcam, entry);
    *entryIdx = prev;
    entryPriority = _BCM_DPP_FIELD_TCAM_ENTRY_PRIORITY_PTR(isExternalTcam, entry);
    *entryPriority = BCM_FIELD_ENTRY_PRIO_DEFAULT;
    if (isExternalTcam) {
        unitData->entryExtTc[entry].ext_info_idx = ext_info_idx;
    }

    for (index = 0; index < SOC_PPD_FP_NOF_QUALS_PER_DB_MAX; index++) {
        _BCM_DPP_FIELD_TCAM_ENTRY_QUALS(isExternalTcam, entry)[index].qualType = bcmFieldQualifyCount;
        _BCM_DPP_FIELD_TCAM_ENTRY_QUALS(isExternalTcam, entry)[index].hwType = SOC_PPD_NOF_FP_QUAL_TYPES;
    }
    for (index = 0; index < _BCM_DPP_NOF_BCM_ACTIONS_PER_DB_MAX; index++) {
        _BCM_DPP_FIELD_TCAM_ENTRY_ACTB(isExternalTcam, entry)[index].bcmType = bcmFieldActionCount;
    }
    for (index = 0; index < SOC_PPD_FP_NOF_ACTIONS_PER_DB_MAX; index++) {
        _BCM_DPP_FIELD_TCAM_ENTRY_ACTP(isExternalTcam, entry)[index].hwType = SOC_PPD_FP_ACTION_TYPE_INVALID;
    }
}

int
_bcm_dpp_field_tcam_entry_action_set(_bcm_dpp_field_info_t *unitData,
                                     uint8 isExternalTcam,
                                     _bcm_dpp_field_ent_idx_t entry,
                                     bcm_field_action_t type,
                                     uint32 param0,
                                     uint32 param1)
{
    _bcm_dpp_field_tc_b_act_t tempActs[_BCM_DPP_NOF_BCM_ACTIONS_PER_DB_MAX];
    unsigned int index;
    unsigned int avail;
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;

    LOG_VERBOSE(BSL_LS_BCM_FP,
                (BSL_META_U(unit,
                            "(%d, %u, %d(%s), %d(%08X), %d(%08X)\n"),
                 unit,
                 _BCM_DPP_FIELD_TCAM_GLOBAL_ID(unit, entry, isExternalTcam),
                 type,
                 (type<bcmFieldActionCount)?_bcm_dpp_field_action_name[type]:"???",
                 param0,
                 param0,
                 param1,
                 param1));
    if ((0 > type) || (bcmFieldActionCount <= type)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG_NO_UNIT("action type %d not valid"), type));
    }
    if (0 == (_BCM_DPP_FIELD_TCAM_ENTRY_FLAGS(isExternalTcam, entry) & _BCM_DPP_FIELD_ENTRY_IN_USE)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d entry %u not in use"),
                          unit,
                          _BCM_DPP_FIELD_TCAM_GLOBAL_ID(unit, entry, isExternalTcam)));
    }
    if (!BCM_FIELD_ASET_TEST(unitData->groupD[_BCM_DPP_FIELD_TCAM_ENTRY_GROUP(isExternalTcam, entry)].aset,
                             type)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d (group %d) ASET"
                                           " does not include %d(%s)"),
                          unit,
                          entry,
                          _BCM_DPP_FIELD_TCAM_ENTRY_GROUP(isExternalTcam, entry),
                          type,
                          _bcm_dpp_field_action_name[type]));
    }
    sal_memcpy(&(tempActs[0]),
               _BCM_DPP_FIELD_TCAM_ENTRY_ACTB(isExternalTcam, entry),
               sizeof(tempActs[0]) * _BCM_DPP_NOF_BCM_ACTIONS_PER_DB_MAX);
    for (index = 0, avail = _BCM_DPP_NOF_BCM_ACTIONS_PER_DB_MAX;
         index < _BCM_DPP_NOF_BCM_ACTIONS_PER_DB_MAX;
         index++) {
        if (type == tempActs[index].bcmType) {
#if _BCM_DPP_ALLOW_IMPLIED_ACTION_REPLACE
            LOG_DEBUG(BSL_LS_BCM_FP,
                      (BSL_META_U(unit,
                                  "unit %d entry %u action %d, %s(%d)"
                                  " %d(%08X) %d(%08X) weill be replaced\n"),
                       unit,
                       _BCM_DPP_FIELD_TCAM_GLOBAL_ID(unit, entry, isExternalTcam),
                       index,
                       _bcm_dpp_field_action_name[tempActs[index].bcmType],
                       tempActs[index].bcmType,
                       tempActs[index].bcmParam0,
                       tempActs[index].bcmParam0,
                       tempActs[index].bcmParam1,
                       tempActs[index].bcmParam1));
            break;
#else /* _BCM_DPP_ALLOW_IMPLIED_ACTION_REPLACE */
            BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d already has"
                                               " action %d (%s)"),
                              unit,
                              entry,
                              type,
                              _bcm_dpp_field_action_name[type]));
#endif /* _BCM_DPP_ALLOW_IMPLIED_ACTION_REPLACE */
        } else if (bcmFieldActionCount == tempActs[index].bcmType) {
            if (_BCM_DPP_NOF_BCM_ACTIONS_PER_DB_MAX <= avail) {
                LOG_DEBUG(BSL_LS_BCM_FP,
                          (BSL_META_U(unit,
                                      "unit %d entry %d has action slot %d"
                                      " available\n"),
                           unit,
                           _BCM_DPP_FIELD_TCAM_GLOBAL_ID(unit, entry, isExternalTcam),
                           index));
                avail = index;
            }
        }
    } /* for (all valid BCM action slots) */
#if _BCM_DPP_ALLOW_IMPLIED_ACTION_REPLACE
    if (index < _BCM_DPP_NOF_BCM_ACTIONS_PER_DB_MAX) {
        /* use existing copy of this action instead of adding a new one */
        avail = index;
    }
#endif /* _BCM_DPP_ALLOW_IMPLIED_ACTION_REPLACE */
    if (avail < _BCM_DPP_NOF_BCM_ACTIONS_PER_DB_MAX) {
        tempActs[avail].bcmType = type;
        tempActs[avail].bcmParam0 = param0;
        tempActs[avail].bcmParam1 = param1;
    } else { /* if (avail < _BCM_DPP_NOF_BCM_ACTIONS_PER_DB_MAX) */
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d entry %u has too many BCM"
                                           " layer actions (%u) to add more"),
                          unit,
                          _BCM_DPP_FIELD_TCAM_GLOBAL_ID(unit, entry, isExternalTcam),
                          avail));
    } /* if (avail < _BCM_DPP_NOF_BCM_ACTIONS_PER_DB_MAX) */
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_tcam_entry_action_update(unitData,
                                                                    isExternalTcam,
                                                                    entry,
                                                                    &(tempActs[0])));
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_field_tcam_entry_destroy(_bcm_dpp_field_info_t *unitData,
                                  uint8 isExternalTcam,
                                  _bcm_dpp_field_ent_idx_t entry)
{
    int result = BCM_E_NONE;
    _bcm_dpp_field_group_t *groupData = &(unitData->groupD[_BCM_DPP_FIELD_TCAM_ENTRY_GROUP(isExternalTcam, entry)]);
    _bcm_dpp_field_stage_t *stageData = &(unitData->stageD[groupData->stage]);
    _bcm_dpp_field_entry_type_t entryType;   
    _bcm_dpp_field_ent_idx_t *entryIdx; 
    uint32 *entryFlags;
    uint8 isExtInfoUsed = 0;
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;

    LOG_DEBUG(BSL_LS_BCM_FP,
              (BSL_META_U(unit,
                          "(%d(%08X),%d) enter (%s,%s)\n"),
               unit,
               PTR_TO_INT(unitData),
               entry,
               (_BCM_DPP_FIELD_TCAM_ENTRY_FLAGS(isExternalTcam, entry) & _BCM_DPP_FIELD_ENTRY_IN_USE)?"+IN_USE":"-in_use",
               (_BCM_DPP_FIELD_TCAM_ENTRY_FLAGS(isExternalTcam, entry) & _BCM_DPP_FIELD_ENTRY_IN_HW)?"+IN_HW":"-in_hw"));
    if (_BCM_DPP_FIELD_TCAM_ENTRY_FLAGS(isExternalTcam, entry) & _BCM_DPP_FIELD_ENTRY_IN_USE) {
        entryType = unitData->stageD[groupData->stage].modeBits[groupData->grpMode]->entryType;
        result = _bcm_dpp_field_tcam_entry_remove(unitData, isExternalTcam, entry);

        if (BCM_E_NONE == result) {
            /* clean up actions */
            result = _bcm_dpp_field_tcam_entry_clear_acts(unitData, isExternalTcam, entry);

            if (isExternalTcam) {
                soc_sand_occ_bm_is_occupied(unitData->unit, unitData->extTcamBmp, unitData->entryExtTc[entry].ext_info_idx, &isExtInfoUsed);
                if (isExtInfoUsed) {
                    _bcm_dpp_field_ext_info_entry_clear(&(unitData->extTcamInfo[unitData->entryExtTc[entry].ext_info_idx]));
                    soc_sand_occ_bm_occup_status_set(unitData->unit, unitData->extTcamBmp, unitData->entryExtTc[entry].ext_info_idx, FALSE);
                }
            }
            /* decrement uninstalled TCAM entries counter */
            unitData->entryUninstalledTcCount--;
        }

        if (BCM_E_NONE == result) {
            /* delink from the group's entry list */
            if (_BCM_DPP_FIELD_TCAM_ENTRY_PREV(isExternalTcam, entry) < _BCM_DPP_FIELD_TCAM_ENTRY_LIMIT(entryType)) {
                entryIdx = _BCM_DPP_FIELD_TCAM_ENTRY_NEXT_PTR(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType),
                                                              _BCM_DPP_FIELD_TCAM_ENTRY_PREV(isExternalTcam, entry));
                *entryIdx = _BCM_DPP_FIELD_TCAM_ENTRY_NEXT(isExternalTcam, entry);
            } else {
                groupData->entryHead = _BCM_DPP_FIELD_TCAM_ENTRY_NEXT(isExternalTcam, entry);
            }
            if (_BCM_DPP_FIELD_TCAM_ENTRY_NEXT(isExternalTcam, entry) < _BCM_DPP_FIELD_TCAM_ENTRY_LIMIT(entryType)) {
                entryIdx = _BCM_DPP_FIELD_TCAM_ENTRY_PREV_PTR(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType),
                                                              _BCM_DPP_FIELD_TCAM_ENTRY_NEXT(isExternalTcam, entry));
                *entryIdx = _BCM_DPP_FIELD_TCAM_ENTRY_PREV(isExternalTcam, entry);
            } else {
                groupData->entryTail = _BCM_DPP_FIELD_TCAM_ENTRY_PREV(isExternalTcam, entry);
            }
            entryIdx = _BCM_DPP_FIELD_TCAM_ENTRY_NEXT_PTR(isExternalTcam, entry);
            *entryIdx = _BCM_DPP_FIELD_TCAM_ENTRY_TC_FREE(entryType);
            entryIdx = _BCM_DPP_FIELD_TCAM_ENTRY_PREV_PTR(isExternalTcam, entry);
            *entryIdx = _BCM_DPP_FIELD_TCAM_ENTRY_LIMIT(entryType);
            entryFlags = _BCM_DPP_FIELD_TCAM_ENTRY_FLAGS_PTR(isExternalTcam, entry);
            *entryFlags = 0;
            if (_BCM_DPP_FIELD_TCAM_ENTRY_TC_FREE(entryType) < _BCM_DPP_FIELD_TCAM_ENTRY_LIMIT(entryType)) {
                entryIdx = _BCM_DPP_FIELD_TCAM_ENTRY_PREV_PTR(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), _BCM_DPP_FIELD_TCAM_ENTRY_TC_FREE(entryType));
                *entryIdx = entry;
            }

            _BCM_DPP_FIELD_TCAM_ENTRY_TC_FREE_OPERATION(entryType, = entry;)
            groupData->entryCount--;
            unitData->stageD[stageData->entryTcSh1].entryElems -= stageData->modeBits[groupData->grpMode]->entryCount;
            unitData->stageD[stageData->entryTcSh1].entryCount--;
            _BCM_DPP_FIELD_TCAM_ENTRY_TC_COUNT_OPERATION(entryType, --;)
        } /* if (BCM_E_NONE == result) */
    } else { /* if (entryData->flags & _BCM_DPP_FIELD_ENTRY_IN_USE) */
        LOG_ERROR(BSL_LS_BCM_FP,
                  (BSL_META_U(unit,
                              "unit %d entry %d is not in use\n"),
                   unit,
                   entry));
        result = BCM_E_NOT_FOUND;
    } /* if (entryData->flags & _BCM_DPP_FIELD_ENTRY_IN_USE) */
    LOG_DEBUG(BSL_LS_BCM_FP,
              (BSL_META_U(unit,
                          "(%d(%08X),%d) return %d (%s)\n"),
               unit,
               PTR_TO_INT(unitData),
               entry,
               result,
               _SHR_ERRMSG(result)));

    BCMDNX_IF_ERR_EXIT(result);
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_field_tcam_entry_add_to_group(_bcm_dpp_field_info_t *unitData,
                                       _bcm_dpp_field_grp_idx_t group,
                                       _bcm_dpp_field_ent_idx_t entry)
{
    _bcm_dpp_field_group_t *groupData;
    _bcm_dpp_field_stage_t *stageData;
    _bcm_dpp_field_ent_idx_t currEntry;
    _bcm_dpp_field_entry_type_t entryType;   
    _bcm_dpp_field_ent_idx_t *entryIdx; 
    _bcm_dpp_field_grp_idx_t *entryGroup;
    uint32 *entryFlags;
    uint32 *hwPriority;
    int max_entry_priorities;
    int insertEntry = FALSE;
    int result = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;
    
    unit = unitData->unit;
    
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
    if ((unitData->stageD[stageData->entryTcSh1].entryElems +
         stageData->modeBits[groupData->grpMode]->entryCount) >
        unitData->stageD[stageData->entryTcSh1].hwEntryLimit) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE,
                         (_BSL_BCM_MSG_NO_UNIT("a new entry in unit %d group %u"
                                           " would exceed stage %u capacity"),
                          unit,
                          group,
                          groupData->stage));
    }
    entryType = unitData->stageD[groupData->stage].modeBits[groupData->grpMode]->entryType;
    if (_BCM_DPP_FIELD_TCAM_ENTRY_FLAGS(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry)
        & _BCM_DPP_FIELD_ENTRY_IN_USE) {
        if (group != _BCM_DPP_FIELD_TCAM_ENTRY_GROUP(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d does not permit"
                                               " entries to migrate from"
                                               " one group to another"),
                              unit));
        }
        else if(groupData->grpMode == bcmFieldGroupModeDirect) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d direct-lookup : entry already in use"),
                              unit));
        }
        if (_BCM_DPP_FIELD_ENT_TYPE_IS_INTTCAM(entryType)) {
            /* remove this entry from the group */
            if (_BCM_DPP_FIELD_TCAM_ENTRY_PREV(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry) < _BCM_DPP_FIELD_TCAM_ENTRY_LIMIT(entryType)) {
                entryIdx = _BCM_DPP_FIELD_TCAM_ENTRY_NEXT_PTR(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), 
                                                              _BCM_DPP_FIELD_TCAM_ENTRY_PREV(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry));
                *entryIdx = _BCM_DPP_FIELD_TCAM_ENTRY_NEXT(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry);
            } else {
                unitData->groupD[_BCM_DPP_FIELD_TCAM_ENTRY_GROUP(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry)].entryHead = 
                    _BCM_DPP_FIELD_TCAM_ENTRY_NEXT(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry);
            }
            if (_BCM_DPP_FIELD_TCAM_ENTRY_NEXT(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry) < _BCM_DPP_FIELD_TCAM_ENTRY_LIMIT(entryType)) {
                entryIdx = _BCM_DPP_FIELD_TCAM_ENTRY_PREV_PTR(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), 
                                                              _BCM_DPP_FIELD_TCAM_ENTRY_NEXT(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry));
                *entryIdx = _BCM_DPP_FIELD_TCAM_ENTRY_PREV(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry);
            } else {
                unitData->groupD[_BCM_DPP_FIELD_TCAM_ENTRY_GROUP(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry)].entryTail = 
                    _BCM_DPP_FIELD_TCAM_ENTRY_PREV(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry);
            }
            insertEntry = TRUE;
        } /* if (_BCM_DPP_FIELD_ENT_TYPE_IS_INTTCAM(entryType)) */
    } else { /* if (entryData->flags & _BCM_DPP_FIELD_ENTRY_IN_USE) */
        /* entry is not in use, so must be coming off the free list */
        if (_BCM_DPP_FIELD_TCAM_ENTRY_PREV(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry) < _BCM_DPP_FIELD_TCAM_ENTRY_LIMIT(entryType)) {  
            entryIdx = _BCM_DPP_FIELD_TCAM_ENTRY_NEXT_PTR(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), 
                                                          _BCM_DPP_FIELD_TCAM_ENTRY_PREV(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry));
            *entryIdx = _BCM_DPP_FIELD_TCAM_ENTRY_NEXT(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry);
        } else {
            _BCM_DPP_FIELD_TCAM_ENTRY_TC_FREE_OPERATION(entryType, = _BCM_DPP_FIELD_TCAM_ENTRY_NEXT(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry);)
        }
        if (_BCM_DPP_FIELD_TCAM_ENTRY_NEXT(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry) < _BCM_DPP_FIELD_TCAM_ENTRY_LIMIT(entryType)) {
            entryIdx = _BCM_DPP_FIELD_TCAM_ENTRY_PREV_PTR(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), 
                                                          _BCM_DPP_FIELD_TCAM_ENTRY_NEXT(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry));
            *entryIdx = _BCM_DPP_FIELD_TCAM_ENTRY_PREV(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry);
        }
        _BCM_DPP_FIELD_TCAM_ENTRY_TC_COUNT_OPERATION(entryType, ++;)
        unitData->stageD[stageData->entryTcSh1].entryElems += stageData->modeBits[groupData->grpMode]->entryCount;
        unitData->stageD[stageData->entryTcSh1].entryCount++;
        groupData->entryCount++;
        unitData->entryUninstalledTcCount++;
        insertEntry = TRUE;
    } /* if (entryData->flags & _BCM_DPP_FIELD_ENTRY_IN_USE) */

    if (insertEntry) {
        if (_BCM_DPP_FIELD_ENT_TYPE_IS_INTTCAM(entryType)) {
            /* scan for where this entry goes in the new group */
            for (currEntry = groupData->entryHead;
                 (currEntry < unitData->entryTcLimit) &&
                 (0 >= _bcm_dpp_compare_entry_priority(unitData->entryTc[entry].entryCmn.entryPriority,
                                                       unitData->entryTc[currEntry].entryCmn.entryPriority));
                 ) {
                /*
                 *  ...as long as there are more entries, and the current entry
                 *  priority is greater than or equal to the new entry priority,
                 *  examine the next entry in the group.
                 */
                currEntry = _BCM_DPP_FIELD_TCAM_ENTRY_NEXT(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), currEntry);
            }
        } else {
            /* External TCAM. Entry should be added to the head of the list */
            currEntry = groupData->entryHead;
        }
        /* insert the new entry *before* currEntry */
        LOG_DEBUG(BSL_LS_BCM_FP,
                  (BSL_META_U(unit,
                              "unit %d: inserting entry %u to group %u before"
                              " entry %u\n"),
                   unit,
                   entry,
                   group,
                   currEntry));
        entryIdx = _BCM_DPP_FIELD_TCAM_ENTRY_NEXT_PTR(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry);
        *entryIdx = currEntry;
        if (currEntry < _BCM_DPP_FIELD_TCAM_ENTRY_LIMIT(entryType)) {
            entryIdx = _BCM_DPP_FIELD_TCAM_ENTRY_PREV_PTR(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry);
            *entryIdx = _BCM_DPP_FIELD_TCAM_ENTRY_PREV(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), currEntry);
            if (_BCM_DPP_FIELD_TCAM_ENTRY_PREV(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry) < _BCM_DPP_FIELD_TCAM_ENTRY_LIMIT(entryType)) {
                entryIdx = _BCM_DPP_FIELD_TCAM_ENTRY_NEXT_PTR(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), 
                                                              _BCM_DPP_FIELD_TCAM_ENTRY_PREV(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry));
                *entryIdx = entry;
            }
            entryIdx = _BCM_DPP_FIELD_TCAM_ENTRY_PREV_PTR(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), currEntry);
            *entryIdx = entry;
        } else {
            entryIdx = _BCM_DPP_FIELD_TCAM_ENTRY_PREV_PTR(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry);
            *entryIdx = groupData->entryTail;
            if (groupData->entryTail < _BCM_DPP_FIELD_TCAM_ENTRY_LIMIT(entryType)) {
                entryIdx = _BCM_DPP_FIELD_TCAM_ENTRY_NEXT_PTR(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), groupData->entryTail);
                *entryIdx = entry;
            }
            groupData->entryTail = entry;
        }
        if (currEntry == groupData->entryHead) {
            groupData->entryHead = entry;
        }

        entryGroup = _BCM_DPP_FIELD_TCAM_ENTRY_GROUP_PTR(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry);
        *entryGroup = group;
    }

    /* entry is in use, has been changed since hw commit, and is new/moved */
    entryFlags = _BCM_DPP_FIELD_TCAM_ENTRY_FLAGS_PTR(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry);
    *entryFlags |= _BCM_DPP_FIELD_ENTRY_IN_USE |
                   _BCM_DPP_FIELD_ENTRY_CHANGED |
                   _BCM_DPP_FIELD_ENTRY_NEW;
    /* group contains changed and added/moved entry/entries */
    groupData->groupFlags |= _BCM_DPP_FIELD_GROUP_CHG_ENT |
                             _BCM_DPP_FIELD_GROUP_ADD_ENT;

    if (SOC_IS_ARAD(unit)) {
        max_entry_priorities = groupData->maxEntryPriorities;
        hwPriority = _BCM_DPP_FIELD_TCAM_ENTRY_HW_PRIORITY_PTR(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry);
        if (_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType) && (max_entry_priorities > 0)) {
            *hwPriority = max_entry_priorities - _BCM_DPP_FIELD_TCAM_ENTRY_PRIORITY(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry);
        }
        else {
            *hwPriority = BCM_FIELD_ENTRY_BCM_PRIO_TO_DPP_PRIO(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType),
                                                               _BCM_DPP_FIELD_TCAM_ENTRY_PRIORITY(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry));
        }
    }

#ifdef BCM_WARM_BOOT_SUPPORT
    _bcm_dpp_field_group_wb_save(unitData, group, NULL, NULL);
#endif /* def BCM_WARM_BOOT_SUPPORT */

    BCMDNX_IF_ERR_EXIT(result);
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_field_tcam_entry_action_get(_bcm_dpp_field_info_t *unitData,
                                     uint8 isExternalTcam,
                                     _bcm_dpp_field_ent_idx_t entry,
                                     bcm_field_action_t type,
                                     uint32 *param0,
                                     uint32 *param1)
{
    unsigned int index;
    SOC_PPD_FP_ENTRY_INFO entInfo;
    SOC_PPD_FP_ACTION_TYPE ppdType;

    uint8 found;
    uint32 soc_sand_rv;
    int result;
    _bcm_dpp_field_group_t *groupData;

    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;
    
    if ((0 > type) || (bcmFieldActionCount <= type)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG_NO_UNIT("action type %d not valid"), type));
    }

    groupData = &(unitData->groupD[_BCM_DPP_FIELD_TCAM_ENTRY_GROUP(isExternalTcam, entry)]);
    if (groupData->groupFlags & _BCM_DPP_FIELD_GROUP_HANDLE_ENTRIES_BY_KEY) {

            result = _bcm_dpp_field_tcam_entry_action_get_by_key(unitData,isExternalTcam, entry, type, param0, param1);           
    } else {

        if (_BCM_DPP_FIELD_TCAM_ENTRY_FLAGS(isExternalTcam, entry) & _BCM_DPP_FIELD_ENTRY_IN_USE) {
            if ((isExternalTcam) && (_BCM_DPP_FIELD_TCAM_ENTRY_FLAGS(isExternalTcam, entry) & _BCM_DPP_FIELD_ENTRY_IN_HW)) {
                /* get action from SOC layer. It's not kept in SW state */
                switch (type) {
                    case bcmFieldActionExternalValue0Set:
                        ppdType = SOC_PPD_FP_ACTION_TYPE_FLP_ACTION_0;
                        break;
                    case bcmFieldActionExternalValue1Set:
                        ppdType = SOC_PPD_FP_ACTION_TYPE_FLP_ACTION_1;
                        break;
                    case bcmFieldActionExternalValue2Set:
                        ppdType = SOC_PPD_FP_ACTION_TYPE_FLP_ACTION_2;
                        break;
                    case bcmFieldActionExternalValue3Set:
                        ppdType = SOC_PPD_FP_ACTION_TYPE_FLP_ACTION_3;
                        break;
                    default:
                        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                                         (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d action type %d (%s) illegal for external TCAM"),
                                          unit,
                                          entry,
                                          type,
                                          _bcm_dpp_field_action_name[type]));

                }
                SOC_PPD_FP_ENTRY_INFO_clear(&entInfo);
                soc_sand_rv = soc_ppd_fp_entry_get(unitData->unitHandle,
                                                   unitData->groupD[_BCM_DPP_FIELD_TCAM_ENTRY_GROUP(isExternalTcam, entry)].hwHandle,
                                                   _BCM_DPP_FIELD_TCAM_ENTRY_HW_HANDLE(isExternalTcam, entry),
                                                   &found,
                                                   &entInfo);
                result = handle_sand_result(soc_sand_rv);
                if (BCM_E_NONE == result) {
                    if (found) {
                        index = 0;
                        while ((index < SOC_PPD_FP_NOF_ACTIONS_PER_DB_MAX) &&
                               (entInfo.actions[index].type != ppdType)) {
                            index++;
                        }
                        if (index < SOC_PPD_FP_NOF_ACTIONS_PER_DB_MAX) {
                            *param0 = entInfo.actions[index].val;
                            *param1 = 0;
                        }
                        else {
                            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                                             (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d does not have"
                                                               " action %d (%s)"),
                                              unit,
                                              entry,
                                              type,
                                              _bcm_dpp_field_action_name[type]));
                        }
                    }
                    else {
                        result = BCM_E_NOT_FOUND;
                        LOG_ERROR(BSL_LS_BCM_FP,
                                  (BSL_META_U(unit,
                                              "unit %d entry %d not valid\n"), unit, entry));
                    }
                }
            }
            else {
                index = 0;
                while ((index < _BCM_DPP_NOF_BCM_ACTIONS_PER_DB_MAX) &&
                       (_BCM_DPP_FIELD_TCAM_ENTRY_ACTB(isExternalTcam, entry)[index].bcmType != type)) {
                    index++;
                }
                /*
                 *  index = offset of the requested type if it existed, or
                 *  _BCM_DPP_NOF_BCM_ACTIONS_PER_DB_MAX
                 */
                if (index < _BCM_DPP_NOF_BCM_ACTIONS_PER_DB_MAX) {
                    /* found requested type, get its information */
                    *param0 = _BCM_DPP_FIELD_TCAM_ENTRY_ACTB(isExternalTcam, entry)[index].bcmParam0;
                    *param1 = _BCM_DPP_FIELD_TCAM_ENTRY_ACTB(isExternalTcam, entry)[index].bcmParam1;
                } else {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                                     (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d does not have"
                                                       " action %d (%s)"),
                                      unit,
                                      entry,
                                      type,
                                      _bcm_dpp_field_action_name[type]));
                }
            }
        } else {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d not in use"),
                              unit,
                              entry));
        }
    }
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

void
_bcm_dpp_field_tcam_entry_copy_id(_bcm_dpp_field_info_t *unitData,
                                  uint8 isExternalTcam,
                                  _bcm_dpp_field_ent_idx_t dest,
                                  _bcm_dpp_field_ent_idx_t source)
{
    _bcm_dpp_field_ent_idx_t next;
    _bcm_dpp_field_ent_idx_t prev;
    _bcm_dpp_field_ent_idx_t *entry_Idx;
    uint32 *entryFlags;
    int *priority;


    /* preserve previous entry list position for now */
    next = _BCM_DPP_FIELD_TCAM_ENTRY_NEXT(isExternalTcam, dest);
    prev = _BCM_DPP_FIELD_TCAM_ENTRY_PREV(isExternalTcam, dest);
    /* copy the old entry to the new entry */
    sal_memcpy(_BCM_DPP_FIELD_TCAM_ENTRY(isExternalTcam, dest),
               _BCM_DPP_FIELD_TCAM_ENTRY(isExternalTcam, source),
                isExternalTcam ? sizeof(_bcm_dpp_field_entry_ext_t) : sizeof(_bcm_dpp_field_entry_t));
    /* keep new entry in previous entry list for now */
    entry_Idx = _BCM_DPP_FIELD_TCAM_ENTRY_NEXT_PTR(isExternalTcam, dest);
    *entry_Idx = next;
    entry_Idx = _BCM_DPP_FIELD_TCAM_ENTRY_PREV_PTR(isExternalTcam, dest);
    *entry_Idx = prev;
    priority = _BCM_DPP_FIELD_TCAM_ENTRY_PRIORITY_PTR(isExternalTcam, dest);
    *priority = BCM_FIELD_ENTRY_PRIO_DEFAULT;
    /* the copy is not yet in hardware, nor is it yet in use */
    entryFlags = _BCM_DPP_FIELD_TCAM_ENTRY_FLAGS_PTR(isExternalTcam, dest);
    *entryFlags &= (~(_BCM_DPP_FIELD_ENTRY_IN_USE |
                                 _BCM_DPP_FIELD_ENTRY_IN_HW |
                                 _BCM_DPP_FIELD_ENTRY_WANT_HW));
}




int
    _bcm_dpp_field_tcam_entry_action_get_by_key(_bcm_dpp_field_info_t *unitData, uint8 isExternalTcam, _bcm_dpp_field_ent_idx_t entry,
                                                 bcm_field_action_t type,uint32 *param0,uint32 *param1)
{
    int result;
    SOC_PPD_FP_ENTRY_INFO entryInfo;
    _bcm_dpp_field_group_t *groupData;
    _bcm_dpp_field_stage_t *stageData;
    uint8 found;
    uint32 soc_sand_rv;

    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;
    
    groupData = &(unitData->groupD[_BCM_DPP_FIELD_TCAM_ENTRY_GROUP(isExternalTcam, entry)]);
    stageData = &(unitData->stageD[groupData->stage]);

    result = _bcm_dpp_field_tcam_entry_build_qualifiers_from_sw_state(unitData, &entryInfo, entry, isExternalTcam, stageData->devInfo->entryMaxQuals, groupData->stage);
    if (BCM_E_NONE != result) {        
        LOG_ERROR(BSL_LS_BCM_FP,(BSL_META_U(unit, "error in _bcm_dpp_field_tcam_entry_build_qualifiers_from_sw_state\n")));
    }

    BCMDNX_IF_ERR_EXIT(result);

    soc_sand_rv = soc_ppd_fp_entry_get(unitData->unitHandle,
                                                   unitData->groupD[_BCM_DPP_FIELD_TCAM_ENTRY_GROUP(isExternalTcam, entry)].hwHandle,
                                                   _BCM_DPP_FIELD_TCAM_ENTRY_HW_HANDLE(isExternalTcam, entry),
                                                   &found,
                                                   &entryInfo);
    result = handle_sand_result(soc_sand_rv);
    if (BCM_E_NONE == result) {
        if (found) {
            (*param0) = entryInfo.actions[0].val; 
            /*index = 0;
            while ((index < SOC_PPD_FP_NOF_ACTIONS_PER_DB_MAX) &&
                   (entryInfo.actions[index].type != ppdType)) {
                index++;
            }
            if (index < SOC_PPD_FP_NOF_ACTIONS_PER_DB_MAX) {
                *param0 = entryInfo.actions[index].val;
                *param1 = 0;
            }
            else {
                BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,(_BSL_BCM_MSG_NO_UNIT("unit %d entry %d does not have"
                                                   " action %d (%s)"),unit,entry,type,_bcm_dpp_field_action_name[type]));
            }*/
        }
        else {
            result = BCM_E_NOT_FOUND;
            LOG_ERROR(BSL_LS_BCM_FP,
                      (BSL_META_U(unit,
                                  "unit %d entry %d not valid\n"), unit, entry));
        }
    }

    BCMDNX_IF_ERR_EXIT(result);    

exit:
    BCMDNX_FUNC_RETURN;
}


int
    _bcm_dpp_field_tcam_entry_remove_by_key(_bcm_dpp_field_info_t *unitData, uint8 isExternalTcam, _bcm_dpp_field_ent_idx_t entry)
{
    int result;
    SOC_PPD_FP_ENTRY_INFO entryInfo;
    _bcm_dpp_field_group_t *groupData;
    _bcm_dpp_field_stage_t *stageData;
    uint32 ppdr;

    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;
    
    groupData = &(unitData->groupD[_BCM_DPP_FIELD_TCAM_ENTRY_GROUP(isExternalTcam, entry)]);
    stageData = &(unitData->stageD[groupData->stage]);

    result = _bcm_dpp_field_tcam_entry_build_qualifiers_from_sw_state(unitData, &entryInfo, entry, isExternalTcam, stageData->devInfo->entryMaxQuals, groupData->stage);
    if (BCM_E_NONE != result) {        
        LOG_ERROR(BSL_LS_BCM_FP,(BSL_META_U(unit, "error in _bcm_dpp_field_tcam_entry_build_qualifiers_from_sw_state\n")));
    }

    BCMDNX_IF_ERR_EXIT(result);

    ppdr = soc_ppd_fp_entry_remove_by_key(unitData->unitHandle, groupData->hwHandle, &entryInfo);

    result = handle_sand_result(ppdr);
    if (BCM_E_NONE != result) {        
        LOG_ERROR(BSL_LS_BCM_FP,(BSL_META_U(unit, "unable to remove unit %d group %u entry %u"
                              " (hw entry %u) from hardware: %d (%s)\n"),
                   unit,
                   _BCM_DPP_FIELD_TCAM_ENTRY_GROUP(isExternalTcam, entry),
                   _BCM_DPP_FIELD_TCAM_GLOBAL_ID(unit, entry, isExternalTcam),
                   _BCM_DPP_FIELD_TCAM_ENTRY_HW_HANDLE(isExternalTcam, entry),
                   result,_SHR_ERRMSG(result)));
    }

    BCMDNX_IF_ERR_EXIT(result);    

exit:
    BCMDNX_FUNC_RETURN;
}


int
    _bcm_dpp_field_tcam_entry_build_qualifiers_from_sw_state(_bcm_dpp_field_info_t *unitData, SOC_PPD_FP_ENTRY_INFO *entryInfo, _bcm_dpp_field_ent_idx_t entry, uint8 isExternalTcam, unsigned int entryMaxQuals, _bcm_dpp_field_stage_idx_t stage)
{
    int index, offset;

    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    for (index = 0, offset = 0; index < SOC_PPD_FP_NOF_QUALS_PER_DB_MAX; index++) {
        if (_BCM_DPP_FIELD_PPD_QUAL_VALID(_BCM_DPP_FIELD_TCAM_ENTRY_QUALS(isExternalTcam, entry)[index].hwType)) {
            LOG_DEBUG(BSL_LS_BCM_FP,(BSL_META_U(unit,"unit %d entry %u qual %d(%s)->%d(%s)"" %08X%08X/%08X%08X\n"),
                       unit,
                       _BCM_DPP_FIELD_TCAM_GLOBAL_ID(unit, entry, isExternalTcam),
                       _BCM_DPP_FIELD_TCAM_ENTRY_QUALS(isExternalTcam, entry)[index].qualType,
                       (bcmFieldQualifyCount>_BCM_DPP_FIELD_TCAM_ENTRY_QUALS(isExternalTcam, entry)[index].qualType)?_bcm_dpp_field_qual_name[_BCM_DPP_FIELD_TCAM_ENTRY_QUALS(isExternalTcam, entry)[index].qualType]:"-",
                       _BCM_DPP_FIELD_TCAM_ENTRY_QUALS(isExternalTcam, entry)[index].hwType,
                       SOC_PPD_FP_QUAL_TYPE_to_string(_BCM_DPP_FIELD_TCAM_ENTRY_QUALS(isExternalTcam, entry)[index].hwType),
                       COMPILER_64_HI(_BCM_DPP_FIELD_TCAM_ENTRY_QUALS(isExternalTcam, entry)[index].qualData),
                       COMPILER_64_LO(_BCM_DPP_FIELD_TCAM_ENTRY_QUALS(isExternalTcam, entry)[index].qualData),
                       COMPILER_64_HI(_BCM_DPP_FIELD_TCAM_ENTRY_QUALS(isExternalTcam, entry)[index].qualMask),
                       COMPILER_64_LO(_BCM_DPP_FIELD_TCAM_ENTRY_QUALS(isExternalTcam, entry)[index].qualMask)));

            entryInfo->qual_vals[offset].type = _BCM_DPP_FIELD_TCAM_ENTRY_QUALS(isExternalTcam, entry)[index].hwType;
            /* SOC_SAND_U64 is a coerced 32b grain little-endian 64b number */
            entryInfo->qual_vals[offset].val.arr[0] = COMPILER_64_LO(_BCM_DPP_FIELD_TCAM_ENTRY_QUALS(isExternalTcam, entry)[index].qualData);
            entryInfo->qual_vals[offset].val.arr[1] = COMPILER_64_HI(_BCM_DPP_FIELD_TCAM_ENTRY_QUALS(isExternalTcam, entry)[index].qualData);
            entryInfo->qual_vals[offset].is_valid.arr[0] = COMPILER_64_LO(_BCM_DPP_FIELD_TCAM_ENTRY_QUALS(isExternalTcam, entry)[index].qualMask);
            entryInfo->qual_vals[offset].is_valid.arr[1] = COMPILER_64_HI(_BCM_DPP_FIELD_TCAM_ENTRY_QUALS(isExternalTcam, entry)[index].qualMask);
            offset++;
        } /* if (entryData->qual[index].hwType < bcmFieldQualifyCount) */
    } /* for (all qualifiers) */
    if (offset > entryMaxQuals) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,(_BSL_BCM_MSG_NO_UNIT("unit %d stage %u group %u entry %u"
                                           " has more qualifier slots in use"
                                           " %d than the stage supports %d"),
                          unit,
                          stage,
                          _BCM_DPP_FIELD_TCAM_ENTRY_GROUP(isExternalTcam, entry),
                          _BCM_DPP_FIELD_TCAM_GLOBAL_ID(unit, entry, isExternalTcam),
                          offset,
                          entryMaxQuals));
    }

    while (offset < SOC_PPD_FP_NOF_QUALS_PER_DB_MAX) {
        entryInfo->qual_vals[offset].type = SOC_PPD_NOF_FP_QUAL_TYPES;
        entryInfo->qual_vals[offset].is_valid.arr[0] = 0;
        entryInfo->qual_vals[offset].is_valid.arr[1] = 0;
        entryInfo->qual_vals[offset].val.arr[0] = 0;
        entryInfo->qual_vals[offset].val.arr[1] = 0;
        offset++;
    }

exit:
    BCMDNX_FUNC_RETURN;
}


#if _BCM_DPP_FIELD_GET_VERIFY_PPD || _BCM_DPP_FIELD_GET_USE_PPD
int
_bcm_dpp_field_tcam_hardware_entry_check(_bcm_dpp_field_info_t *unitData,
                                         bcm_field_entry_t entry,
                                         SOC_PPD_FP_ENTRY_INFO *entInfoTc)
{
    uint32 soc_sandResult;
    uint8 okay;
    unsigned int index;
    unsigned int offset;
    _bcm_dpp_field_group_t *groupData;
    _bcm_dpp_field_ent_idx_t ent;
    int result;
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;

    if (!(_BCM_DPP_FIELD_ENT_IS_TCAM(unit, entry))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d is not a"
                                           " TCAM entry"),
                          unit,
                          entry));
    }
    ent = _BCM_DPP_FIELD_TCAM_LOCAL_ID(unit, entry);
    groupData = &(unitData->groupD[_BCM_DPP_FIELD_TCAM_ENTRY_GROUP(_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent)]);

    SOC_PPD_FP_ENTRY_INFO_clear(entInfoTc);
    soc_sandResult = soc_ppd_fp_entry_get(unitData->unitHandle,
                                          unitData->groupD[_BCM_DPP_FIELD_TCAM_ENTRY_GROUP(_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent)].hwHandle,
                                          _BCM_DPP_FIELD_TCAM_ENTRY_HW_HANDLE(_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent),
                                          &okay,
                                          entInfoTc);
    result = handle_sand_result(soc_sandResult);
    BCMDNX_IF_ERR_EXIT_MSG(result,
                        (_BSL_BCM_MSG_NO_UNIT("unit %d unable to fetch"
                                          " entry %u hardware"
                                          " information (%u):"
                                          " %d (%s)"),
                         unit,
                         entry,
                         _BCM_DPP_FIELD_TCAM_ENTRY_HW_HANDLE(_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent),
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
    if (_BCM_DPP_FIELD_TCAM_ENTRY_HW_PRIORITY(_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent) != entInfoTc->priority) {
#if !_BCM_DPP_FIELD_GET_VERIFY_RETURN_ERROR
        LOG_WARN(BSL_LS_BCM_FP,
                 (BSL_META_U(unit,
                             "unit %d entry %d hardware priority %d does"
                             " not match PPD layer priority %d\n"),
                  unit,
                  entry,
                  _BCM_DPP_FIELD_TCAM_ENTRY_HW_PRIORITY(_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent),
                  entInfoTc->priority));
#else /* !_BCM_DPP_FIELD_GET_VERIFY_RETURN_ERROR */
        BCMDNX_ERR_EXIT_MSG(_BCM_DPP_FIELD_GET_VERIFY_ERROR,
                         (_BSL_BCM_MSG_NO_UNIT(("unit %d entry %d hardware priority"
                                            " %d does not match PPD layer"
                                            " priority %d\n"),
                                           unit,
                                           entry,
                                           _BCM_DPP_FIELD_TCAM_ENTRY_HW_PRIORITY(_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent),
                                           entInfoTc->priority)));
#endif /* !_BCM_DPP_FIELD_GET_VERIFY_RETURN_ERROR */
    }
    for (index = 0;
         index < unitData->stageD[groupData->stage].devInfo->entryMaxQuals;
         index++) {
        if (_BCM_DPP_FIELD_PPD_QUAL_VALID(_BCM_DPP_FIELD_TCAM_ENTRY_QUALS(_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent)[index].hwType)) {
            /* qualifier is in use */
            for (offset = 0;
                 (offset < SOC_PPD_FP_NOF_QUALS_PER_DB_MAX) &&
                 (entInfoTc->qual_vals[offset].type !=
                  _BCM_DPP_FIELD_TCAM_ENTRY_QUALS(_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent)[index].hwType);
                 offset++) {
                /* iterate until found matching qualifier */
            }
            if (offset < SOC_PPD_FP_NOF_QUALS_PER_DB_MAX) {
                /* found matching qualifier; compare it */
                if ((COMPILER_64_HI(_BCM_DPP_FIELD_TCAM_ENTRY_QUALS(_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent)[index].qualData) !=
                     entInfoTc->qual_vals[offset].val.arr[1]) ||
                    (COMPILER_64_LO(_BCM_DPP_FIELD_TCAM_ENTRY_QUALS(_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent)[index].qualData) !=
                     entInfoTc->qual_vals[offset].val.arr[0]) ||
                    (COMPILER_64_HI(_BCM_DPP_FIELD_TCAM_ENTRY_QUALS(_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent)[index].qualMask) !=
                     entInfoTc->qual_vals[offset].is_valid.arr[1]) ||
                    (COMPILER_64_LO(_BCM_DPP_FIELD_TCAM_ENTRY_QUALS(_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent)[index].qualMask) !=
                     entInfoTc->qual_vals[offset].is_valid.arr[0])) {
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
                              (_BCM_DPP_FIELD_TCAM_ENTRY_QUALS(_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent)[index].qualType < bcmFieldQualifyCount)?_bcm_dpp_field_qual_name[_BCM_DPP_FIELD_TCAM_ENTRY_QUALS(_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent)[index].qualType]:"-",
                              _BCM_DPP_FIELD_TCAM_ENTRY_QUALS(_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent)[index].qualType,
                              COMPILER_64_HI(_BCM_DPP_FIELD_TCAM_ENTRY_QUALS(_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent)[index].qualData),
                              COMPILER_64_LO(_BCM_DPP_FIELD_TCAM_ENTRY_QUALS(_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent)[index].qualData),
                              COMPILER_64_HI(_BCM_DPP_FIELD_TCAM_ENTRY_QUALS(_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent)[index].qualMask),
                              COMPILER_64_LO(_BCM_DPP_FIELD_TCAM_ENTRY_QUALS(_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent)[index].qualMask),
                              SOC_PPD_FP_QUAL_TYPE_to_string(_BCM_DPP_FIELD_TCAM_ENTRY_QUALS(_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent)[index].hwType),
                              _BCM_DPP_FIELD_TCAM_ENTRY_QUALS(_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent)[index].hwType,
                              offset,
                              SOC_PPD_FP_QUAL_TYPE_to_string(entInfoTc->qual_vals[offset].type),
                              entInfoTc->qual_vals[offset].type,
                              entInfoTc->qual_vals[offset].val.arr[1],
                              entInfoTc->qual_vals[offset].val.arr[0],
                              entInfoTc->qual_vals[offset].is_valid.arr[1],
                              entInfoTc->qual_vals[offset].is_valid.arr[0]));
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
                                      (_BCM_DPP_FIELD_TCAM_ENTRY_QUALS(_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent)[index].qualType < bcmFieldQualifyCount)?_bcm_dpp_field_qual_name[_BCM_DPP_FIELD_TCAM_ENTRY_QUALS(_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent)[index].qualType]:"-",
                                      _BCM_DPP_FIELD_TCAM_ENTRY_QUALS(_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent)[index].qualType,
                                      COMPILER_64_HI(_BCM_DPP_FIELD_TCAM_ENTRY_QUALS(_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent)[index].qualData),
                                      COMPILER_64_LO(_BCM_DPP_FIELD_TCAM_ENTRY_QUALS(_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent)[index].qualData),
                                      COMPILER_64_HI(_BCM_DPP_FIELD_TCAM_ENTRY_QUALS(_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit,entry), ent)[index].qualMask),
                                      COMPILER_64_LO(_BCM_DPP_FIELD_TCAM_ENTRY_QUALS(_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent)[index].qualMask),
                                      SOC_PPD_FP_QUAL_TYPE_to_string(_BCM_DPP_FIELD_TCAM_ENTRY_QUALS(_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent)[index].hwType),
                                      _BCM_DPP_FIELD_TCAM_ENTRY_QUALS(_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent)[index].hwType,
                                      offset,
                                      SOC_PPD_FP_QUAL_TYPE_to_string(entInfoTc->qual_vals[offset].type),
                                      entInfoTc->qual_vals[offset].type,
                                      entInfoTc->qual_vals[offset].val.arr[1],
                                      entInfoTc->qual_vals[offset].val.arr[0],
                                      entInfoTc->qual_vals[offset].is_valid.arr[1],
                                      entInfoTc->qual_vals[offset].is_valid.arr[0]));
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
                          (_BCM_DPP_FIELD_TCAM_ENTRY_QUALS(_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent)[index].qualType < bcmFieldQualifyCount)?_bcm_dpp_field_qual_name[_BCM_DPP_FIELD_TCAM_ENTRY_QUALS(_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent)[index].qualType]:"-",
                          _BCM_DPP_FIELD_TCAM_ENTRY_QUALS(_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent)[index].qualType,
                          COMPILER_64_HI(_BCM_DPP_FIELD_TCAM_ENTRY_QUALS(_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent)[index].qualData),
                          COMPILER_64_LO(_BCM_DPP_FIELD_TCAM_ENTRY_QUALS(_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent)[index].qualData),
                          COMPILER_64_HI(_BCM_DPP_FIELD_TCAM_ENTRY_QUALS(_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent)[index].qualMask),
                          COMPILER_64_LO(_BCM_DPP_FIELD_TCAM_ENTRY_QUALS(_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent)[index].qualMask),
                          SOC_PPD_FP_QUAL_TYPE_to_string(_BCM_DPP_FIELD_TCAM_ENTRY_QUALS(_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent)[index].hwType),
                          _BCM_DPP_FIELD_TCAM_ENTRY_QUALS(_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent)[index].hwType));
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
                                  (_BCM_DPP_FIELD_TCAM_ENTRY_QUALS(_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent)[index].qualType < bcmFieldQualifyCount)?_bcm_dpp_field_qual_name[_BCM_DPP_FIELD_TCAM_ENTRY_QUALS(_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent)[index].qualType]:"-",
                                  _BCM_DPP_FIELD_TCAM_ENTRY_QUALS(_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent)[index].qualType,
                                  COMPILER_64_HI(_BCM_DPP_FIELD_TCAM_ENTRY_QUALS(_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent)[index].qualData),
                                  COMPILER_64_LO(_BCM_DPP_FIELD_TCAM_ENTRY_QUALS(_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent)[index].qualData),
                                  COMPILER_64_HI(_BCM_DPP_FIELD_TCAM_ENTRY_QUALS(_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent)[index].qualMask),
                                  COMPILER_64_LO(_BCM_DPP_FIELD_TCAM_ENTRY_QUALS(_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent)[index].qualMask),
                                  SOC_PPD_FP_QUAL_TYPE_to_string(_BCM_DPP_FIELD_TCAM_ENTRY_QUALS(_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent)[index].hwType),
                                  _BCM_DPP_FIELD_TCAM_ENTRY_QUALS(_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent)[index].hwType));
#endif /* !_BCM_DPP_FIELD_GET_VERIFY_RETURN_ERROR */
            } /* if (offset < SOC_PPD_FP_NOF_QUALS_PER_DB_MAX) */
        } /* if (qualifier is in use) */
    } /* for (all possible qualifier slots on this entry) */
    for (index = 0;
         index < unitData->stageD[groupData->stage].devInfo->entryMaxActs;
         index++) {
        if (_BCM_DPP_FIELD_PPD_ACTION_VALID(_BCM_DPP_FIELD_TCAM_ENTRY_ACTP(_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent)[index].hwType)) {
            /* action is in use */
            /* 
             * COVERITY 
             */
            /* use of the macro SOC_DPP_DEFS_MAX can lead to unneeded comperations */
            /* coverity[same_on_both_sides] */
            for (offset = 0;
                 (offset < SOC_PPD_FP_NOF_ACTIONS_PER_DB_MAX) &&
                 (entInfoTc->actions[offset].type !=
                  _BCM_DPP_FIELD_TCAM_ENTRY_ACTP(_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent)[index].hwType);
                 offset++) {
                /* iterate until found matching action */
            }
            if (offset < SOC_PPD_FP_NOF_ACTIONS_PER_DB_MAX) {
                if (_BCM_DPP_FIELD_TCAM_ENTRY_ACTP(_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent)[index].hwParam !=
                    entInfoTc->actions[offset].val) {
#if !_BCM_DPP_FIELD_GET_VERIFY_RETURN_ERROR
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "unit %d entry %d PPD action %d"
                                         " %s(%d) %08X does not match"
                                         " hardware action %d %s(%d) %08Xn"),
                              unit,
                              entry,
                              index,
                              SOC_PPD_FP_ACTION_TYPE_to_string(_BCM_DPP_FIELD_TCAM_ENTRY_ACTP(_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent)[index].hwType),
                              _BCM_DPP_FIELD_TCAM_ENTRY_ACTP(_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent)[index].hwType,
                              _BCM_DPP_FIELD_TCAM_ENTRY_ACTP(_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent)[index].hwParam,
                              offset,
                              SOC_PPD_FP_ACTION_TYPE_to_string(entInfoTc->actions[offset].type),
                              entInfoTc->actions[offset].type,
                              entInfoTc->actions[offset].val));
#else /* !_BCM_DPP_FIELD_GET_VERIFY_RETURN_ERROR */
                    BCMDNX_ERR_EXIT_MSG(_BCM_DPP_FIELD_GET_VERIFY_ERROR,
                                     (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d PPD"
                                                       " action %d %s(%d) %08X"
                                                       " does not match"
                                                       " hardware action %d"
                                                       " %s(%d) %08Xn"),
                                      unit,
                                      entry,
                                      index,
                                      SOC_PPD_FP_ACTION_TYPE_to_string(_BCM_DPP_FIELD_TCAM_ENTRY_ACTP(_BCM_DPP_FIELD_ENT_IS_EXTTCAM((unit, entry), ent)[index].hwType),
                                      _BCM_DPP_FIELD_TCAM_ENTRY_ACTP(_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent)[index].hwType,
                                      _BCM_DPP_FIELD_TCAM_ENTRY_ACTP(_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent)[index].hwParam,
                                      offset,
                                      SOC_PPD_FP_ACTION_TYPE_to_string(entInfoTc->actions[offset].type),
                                      entInfoTc->actions[offset].type,
                                      entInfoTc->actions[offset].val));
#endif /* !_BCM_DPP_FIELD_GET_VERIFY_RETURN_ERROR */
                } /* if (action mismatches) */
            } else { /* if (offset < SOC_PPD_FP_NOF_ACTIONS_PER_DB_MAX) */
#if !_BCM_DPP_FIELD_GET_VERIFY_RETURN_ERROR
                LOG_WARN(BSL_LS_BCM_FP,
                         (BSL_META_U(unit,
                                     "unit %d entry %d PPD action %d %s(%d)"
                                     " %08X has no matching PPD action\n"),
                          unit,
                          entry,
                          index,
                          SOC_PPD_FP_ACTION_TYPE_to_string(_BCM_DPP_FIELD_TCAM_ENTRY_ACTP(_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent)[index].hwType),
                          _BCM_DPP_FIELD_TCAM_ENTRY_ACTP(_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent)[index].hwType,
                          _BCM_DPP_FIELD_TCAM_ENTRY_ACTP(_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent)[index].hwParam));
#else /* !_BCM_DPP_FIELD_GET_VERIFY_RETURN_ERROR */
                BCMDNX_ERR_EXIT_MSG(_BCM_DPP_FIELD_GET_VERIFY_ERROR,
                                 (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d PPD"
                                                   " action %d %s(%d) %08X"
                                                   " has no matching PPD"
                                                   " action"),
                                  unit,
                                  entry,
                                  index,
                                  SOC_PPD_FP_ACTION_TYPE_to_string(_BCM_DPP_FIELD_TCAM_ENTRY_ACTP(_BCM_DPP_FIELD_ENT_IS_EXTTCAM((unit, entry), ent)[index].hwType),
                                  _BCM_DPP_FIELD_TCAM_ENTRY_ACTP(_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent)[index].hwType,
                                  _BCM_DPP_FIELD_TCAM_ENTRY_ACTP(_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent)[index].hwParam);
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


