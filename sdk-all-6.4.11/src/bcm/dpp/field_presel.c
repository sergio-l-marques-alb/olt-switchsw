/*
 * $Id: field_presel.c,v 1.36 Broadcom SDK $
 * $Copyright: Copyright 2016 Broadcom Corporation.
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
 *     Preselector support
 */

/*
 *  The functions in this module deal with preselectors on PPD devices.  They
 *  are invoked by the main field module when certain things occur, such as
 *  calls that explicitly involve preselectors, or calls that are more general
 *  but using a preselector handle, and in some cases to deal with implied use
 *  of preselectors (such as a group that has no specified preselector set).
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

#include <soc/drv.h>
#include <bcm_int/dpp/field_int.h>
#include <bcm_int/dpp/error.h>
#include <bcm_int/dpp/utils.h>
#include <bcm_int/dpp/switch.h>
#include <soc/dpp/PPD/ppd_api_fp.h>
#include <soc/dpp/PPD/ppd_api_port.h>
#ifdef BCM_PETRAB_SUPPORT
#include <soc/dpp/Petra/PB_PP/pb_pp_sw_db.h>
#endif /* def BCM_PETRAB_SUPPORT */
#ifdef BCM_ARAD_SUPPORT
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_fp.h>
#endif /* BCM_ARAD_SUPPORT */
#include <shared/bitop.h>
#include <shared/swstate/access/sw_state_access.h>
#include <shared/swstate/sw_state_sync_db.h>

#include <soc/dcmn/dcmn_wb.h>

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

/******************************************************************************
 *
 *  Functions and data shared with other field modules
 */

int
_bcm_dpp_field_presel_info_get(bcm_dpp_field_info_OLD_t *unitData,
                               _bcm_dpp_field_presel_idx_t presel,
                               unsigned int qualLimit,
                               _bcm_dpp_field_stage_idx_t *stage,
                               _bcm_dpp_field_qual_t *bcmQuals)
{
    SOC_PPD_FP_PFG_INFO pfgInfo;
    uint32 sandResult;
    unsigned int index;
    unsigned int offset = 0;
    _bcm_dpp_field_presel_idx_t preselLimit;
    
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.preselLimit.get(unit, &preselLimit)) ;
    if (preselLimit <= presel) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d preselector %u invalid"),
                          unit,
                          presel));
    }
    SOC_PPD_FP_PFG_INFO_clear(&pfgInfo);
    sandResult = soc_ppd_fp_packet_format_group_get(unitData->unitHandle,
                                                    presel,
                                                    &pfgInfo);
    BCMDNX_IF_ERR_EXIT_MSG(handle_sand_result(sandResult),
                        (_BSL_BCM_MSG_NO_UNIT("unable to read unit %d presel %u"),
                         unit,
                         presel));
    /* ensure the buffer is clear first */
    if (bcmQuals) {
        for (index = 0; index < qualLimit; index++) {
            bcmQuals[index].hwType = BCM_FIELD_ENTRY_INVALID;
            bcmQuals[index].qualType = BCM_FIELD_ENTRY_INVALID;
            COMPILER_64_ZERO(bcmQuals[index].qualData);
            COMPILER_64_ZERO(bcmQuals[index].qualMask);
        }
    }
#ifdef BCM_PETRAB_SUPPORT
    if (SOC_IS_PETRAB(unit)) {
        /* PetraB PFG support is not qualifier based (legacy) */
        if (bcmQuals && (qualLimit > 0)) {
            /* caller provided qualifier buffer */
            bcmQuals[0].hwType = BCM_FIELD_ENTRY_INVALID;
            bcmQuals[0].qualType = bcmFieldQualifyHeaderFormatSet;
            COMPILER_64_SET(bcmQuals[0].qualData, 0, pfgInfo.hdr_format_bmp);
            COMPILER_64_SET(bcmQuals[0].qualMask, 0, ~0);
            
            offset = 1; 
        }
        /* PetraB only supports PFGs on ingress stage */
        pfgInfo.stage = SOC_PPD_FP_DATABASE_STAGE_INGRESS_PMF;
    } /* if (SOC_IS_PETRAB(unit)) */
#endif /* def BCM_PETRAB_SUPPORT */
#ifdef BCM_ARAD_SUPPORT
    if (SOC_IS_ARAD(unit)) {
        bcm_field_qualify_t ppdQual;
        /* Arad PFG support is qualifier based */
        if (bcmQuals) {
            if (pfgInfo.is_array_qualifier) {
                /* parse hardware state into working view */
                for (index = 0;
                     index < SOC_PPC_FP_NOF_QUALS_PER_PFG_MAX;
                     index++) {
                    if (_BCM_DPP_FIELD_PPD_QUAL_VALID(pfgInfo.qual_vals[index].type)) {
                        if (offset < qualLimit) {

                            bcmQuals[offset].hwType = pfgInfo.qual_vals[index].type;
                            COMPILER_64_SET(bcmQuals[offset].qualData, pfgInfo.qual_vals[index].val.arr[1],
                                                                         pfgInfo.qual_vals[index].val.arr[0]);
                            COMPILER_64_SET(bcmQuals[offset].qualMask, pfgInfo.qual_vals[index].is_valid.arr[1],
                                                                         pfgInfo.qual_vals[index].is_valid.arr[0]);
                            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.ppdQual.get(unit, pfgInfo.qual_vals[index].type, &ppdQual));
                            bcmQuals[offset].qualType = ppdQual;
                        } /* if (offset < qualLimit) */
                        offset++;
                    } /* if (PPD qualifier is valid) */
                } /* for (all possible qualifiers) */
            } /* if (pfgInfo.is_array_qualifier) */
        } /* if (bcmQuals) */
    } /* if (SOC_IS_ARAD(unit)) */
#endif /* def BCM_ARAD_SUPPORT */
    /* fill in the BCM layer stage index */
    if (stage) {
        for (index = 0; index < unitData->devInfo->stages; index++) {
            if (_BCM_DPP_FIELD_STAGE_DEV_INFO(unit, index).hwStageId == pfgInfo.stage) {
                *stage = index;
                break;
            }
        } /* for (index = 0; index < unitData->devInfo->stages; index++) */
    } /* if (stage) */
    if (bcmQuals) {
        if (offset >= qualLimit) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d presel %u has %u quals"
                                               " but was only able to collect %u"),
                              unit,
                              presel,
                              offset,
                              qualLimit));
        }
    } /* if (bcmQuals) */
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_field_presel_info_set(bcm_dpp_field_info_OLD_t *unitData,
                               _bcm_dpp_field_presel_idx_t presel,
                               unsigned int qualLimit,
                               _bcm_dpp_field_stage_idx_t stage,
                               const _bcm_dpp_field_qual_t *bcmQuals)
{
    SOC_PPD_FP_PFG_INFO *pfgInfo = NULL;
    uint32 sandResult;
    unsigned int index;
#ifdef BCM_ARAD_SUPPORT
    unsigned int offset;
#endif /* def BCM_ARAD_SUPPORT */
    SOC_SAND_SUCCESS_FAILURE success;
    _bcm_dpp_field_presel_idx_t preselLimit;

    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.preselLimit.get(unit, &preselLimit)) ;
    if (preselLimit <= presel) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d preselector %u invalid"),
                          unit,
                          presel));
    }
    BCMDNX_ALLOC(pfgInfo, sizeof(SOC_PPD_FP_PFG_INFO), "_bcm_dpp_field_presel_reset.pfgInfo");
    if (pfgInfo == NULL) {        
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG_NO_UNIT("failed to allocate memory")));
    }

    /* DPP requires existing PFG (if in place) be overwritten first */
    SOC_PPD_FP_PFG_INFO_clear(pfgInfo);
    pfgInfo->stage = SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF;
    success = SOC_SAND_SUCCESS;
    if (soc_property_get(unit, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE, FALSE) == 0) {
        sandResult = soc_ppd_fp_packet_format_group_set(unitData->unitHandle,
                                        presel,
                                        pfgInfo,
                                        &success);
        BCMDNX_IF_ERR_EXIT_MSG(handle_sand_result(sandResult),
                    (_BSL_BCM_MSG_NO_UNIT("unable to write unit %d"
                                      " presel %u)"),
                     unit,
                     presel));
        BCMDNX_IF_ERR_EXIT_MSG(translate_sand_success_failure(success),
                            (_BSL_BCM_MSG_NO_UNIT("write unit %d presel %u"
                                              " unsucessful"),
                             unit,
                             presel));
    }
    

    if (qualLimit) {
        /* caller provided qualifier info so intends more than just destroy */
        SOC_PPD_FP_PFG_INFO_clear(pfgInfo);
        if (unitData->devInfo->stages <= stage) {
            /* caller provided invalid stage; assume ingress */
            pfgInfo->stage = SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF;
        } else {
            /* use caller's provided stage ID */
            pfgInfo->stage = _BCM_DPP_FIELD_STAGE_DEV_INFO(unit, stage).hwStageId;
        } /* if (unitData->devInfo->stages <= stage) */
#ifdef BCM_PETRAB_SUPPORT
        if (SOC_IS_PETRAB(unit)) {
            /* PetraB uses legacy fields, but make it look like qualifiers */
            pfgInfo->pp_ports_bmp.arr[0] = 0xFFFFFFFF;
            pfgInfo->pp_ports_bmp.arr[1] = 0xFFFFFFFF;
            pfgInfo->vlan_tag_structure_bmp = 0xFFFFFFFF;
            for (index = 0; index < qualLimit; index++) {
                if (0 <= bcmQuals[index].qualType) {
                    switch (bcmQuals[index].qualType) {
                    case bcmFieldQualifyHeaderFormatSet:
                        pfgInfo->hdr_format_bmp = COMPILER_64_LO(bcmQuals[index].qualData);
                        break;
                    
                    case bcmFieldQualifyCount:
                        /* skip this value */
                        break;
                    default:
                    	if (bcmQuals[index].qualType == BCM_FIELD_ENTRY_INVALID)
                    		/* skip this value */
							break;
                        /* should not see this condition */
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                         (_BSL_BCM_MSG_NO_UNIT("unit %d presel %u"
                                                           " tries to specify "
                                                           " unsupported"
                                                           " qual %s (%d)"),
                                          unit,
                                          presel,
                                          (bcmQuals[index].qualType  != BCM_FIELD_ENTRY_INVALID)?_bcm_dpp_field_qual_name[bcmQuals[index].qualType]:"-",
                                          bcmQuals[index].qualType));
                    } /* switch (bcmQuals[index].qualType) */
                } else { /* if (0 <= bcmQuals[index].qualType) */
                    /* should not see this condition */
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("unit %d presel %u"
                                                       " tries to specify "
                                                       " unsupported"
                                                       " qual %d"),
                                      unit,
                                      presel,
                                      bcmQuals[index].qualType));
                } /* if (0 <= bcmQuals[index].qualType) */
            } /* for (index = 0; index < qualLimit; index++) */
        } /* if (SOC_IS_PETRAB(unit)) */
#endif /* def BCM_PETRAB_SUPPORT */
#ifdef BCM_ARAD_SUPPORT
        if (SOC_IS_ARAD(unit)) {
            /* Arad uses qualifier array */
            if (qualLimit) {
                pfgInfo->is_array_qualifier = TRUE;
                if (soc_property_get(unit, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE, FALSE)) {
                    pfgInfo->is_for_hw_commit = FALSE;
                }
                for (index = 0, offset = 0; index < qualLimit; index++) {
                    if ((0 <= bcmQuals[index].qualType) &&
                        (bcmFieldQualifyCount > bcmQuals[index].qualType)) {
                        if (offset < SOC_PPC_FP_NOF_QUALS_PER_PFG_MAX) {
                            pfgInfo->qual_vals[offset].type = bcmQuals[index].hwType;
                            pfgInfo->qual_vals[offset].val.arr[0] = COMPILER_64_LO(bcmQuals[index].qualData);
                            pfgInfo->qual_vals[offset].val.arr[1] = COMPILER_64_HI(bcmQuals[index].qualData);
                            pfgInfo->qual_vals[offset].is_valid.arr[0] = COMPILER_64_LO(bcmQuals[index].qualMask);
                            pfgInfo->qual_vals[offset].is_valid.arr[1] = COMPILER_64_HI(bcmQuals[index].qualMask);
                            offset++;
                        } else { /* if (offset is valid) */
                            BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                                             (_BSL_BCM_MSG_NO_UNIT("unit %d presel"
                                                               " %u tries to"
                                                               " specify too"
                                                               " many quals"),
                                              unit,
                                              presel));
                        } /* if (offset is valid) */
                    } else if (BCM_FIELD_ENTRY_INVALID != bcmQuals[index].qualType) {
                        /* should not see this condition */
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                         (_BSL_BCM_MSG_NO_UNIT("unit %d presel %u"
                                                           " tries to specify "
                                                           " unsupported"
                                                           " qual %d"),
                                          unit,
                                          presel,
                                          bcmQuals[index].qualType));
                    }
                } /* for (index = 0, offset = 0; index < qualLimit; index++) */
            } /* if (caller provided any qualifiers) */
        } /* if (SOC_IS_ARAD(unit)) */
#endif /* def BCM_ARAD_SUPPORT */
        sandResult = soc_ppd_fp_packet_format_group_set(unitData->unitHandle,
                                                        presel,
                                                        pfgInfo,
                                                        &success);
        BCMDNX_IF_ERR_EXIT_MSG(handle_sand_result(sandResult),
                            (_BSL_BCM_MSG_NO_UNIT("unable to write unit %d"
                                              " presel %u)"),
                             unit,
                             presel));
        BCMDNX_IF_ERR_EXIT_MSG(translate_sand_success_failure(success),
                            (_BSL_BCM_MSG_NO_UNIT("write unit %d presel %u"
                                              " unsucessful"),
                             unit,
                             presel));
    } /* if (caller provided qualifier data) */
exit:
    BCM_FREE(pfgInfo);
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_field_presel_reset(int unit)
{
    SOC_PPD_FP_PFG_INFO *pfgInfo = NULL;
    uint32 limit = 0;
    uint32 index;
    uint32 sandResult;
    uint32 success;
    int result;
    int auxRes = BCM_E_NONE;
    uint32 handle = (unit);
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_ALLOC(pfgInfo, sizeof(SOC_PPD_FP_PFG_INFO), "_bcm_dpp_field_presel_reset.pfgInfo");
    if (pfgInfo == NULL) {        
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
    }

    SOC_PPD_FP_PFG_INFO_clear(pfgInfo);
    pfgInfo->stage = SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF;
#ifdef BCM_PETRAB_SUPPORT
    if (SOC_IS_PETRAB(unit)) {
        limit = _BCM_PETRA_FIELD_PRESEL_LIMIT(unit);
        /* Petra PPD might reserve last PFG for IPv4 use */
        if (soc_pb_pp_sw_db_ipv4_host_extend_enable_get(handle)) {
            limit--;
        }
    }
#endif /* def BCM_PETRAB_SUPPORT */
#ifdef BCM_ARAD_SUPPORT
    if (SOC_IS_ARAD(unit)) {
        limit = SOC_PPC_FP_NOF_PFGS_ARAD;
    }
#endif /* def BCM_ARAD_SUPPORT */
    for (index = 0; index < limit; index++) {
        sandResult = soc_ppd_fp_packet_format_group_set(handle,
                                                        index,
                                                        pfgInfo,
                                                        &success);
        result = handle_sand_result(sandResult);
        if (BCM_E_NONE == result) {
            result = translate_sand_success_failure(success);
        }
        if (BCM_E_NONE != result) {
            LOG_ERROR(BSL_LS_BCM_FP,
                      (BSL_META_U(unit,
                                  "unable to clear unit %d PFG %d:"
                                  " %d (%s)\n"),
                       unit,
                       index,
                       result,
                       _SHR_ERRMSG(result)));
            auxRes = result;
        }
    } /* for (index = 0; index < limit; index++) */

    BCMDNX_IF_ERR_EXIT(auxRes);
exit:
    BCM_FREE(pfgInfo);
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_field_presel_init(bcm_dpp_field_info_OLD_t *unitData)
{
#ifdef BCM_PETRAB_SUPPORT
    _bcm_dpp_field_stage_idx_t stage;
#endif /* def BCM_PETRAB_SUPPORT */
    _bcm_dpp_field_presel_idx_t presel;
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;

    /* verify initial setup */
    /*
     * was:
     *   unitData->preselLimit = unitData->devInfo->presels;
     */
    if (!SOC_WARM_BOOT(unit)) {
        /* Cold boot -  allocate sw-state resources */
        FIELD_ACCESS.preselLimit.set(unit, unitData->devInfo->presels);
        for (presel = 0; presel < unitData->devInfo->presels; presel++) {
            /*
             * Was:
             *   BCM_FIELD_PRESEL_REMOVE(unitData->preselInUse, presel);
             * Which was equivalent to:
             *   SHR_BITCLR(unitData->preselInUse.w, presel) ;
             */
            FIELD_ACCESS_PRESELINUSE_W.bit_clear(unit, presel) ;
            /*
             * Was:
             *   unitData->preselRefs[presel] = 0;
             */
            FIELD_ACCESS_PRESELREFS.set(unit, presel, 0) ;
        }
    }
#ifdef BCM_PETRAB_SUPPORT
    if (SOC_IS_PETRAB(unit)) {
        /* the PPD may reserve last preselector as IPv4 */
        presel = unitData->devInfo->presels - 1;
        if (soc_pb_pp_sw_db_ipv4_host_extend_enable_get(unitData->unitHandle)) {
            /* find the ingress stage */
            for (stage = 0; stage < unitData->devInfo->stages; stage++) {
                if (SOC_PPD_FP_DATABASE_STAGE_INGRESS_PMF ==
                    unitData->devInfo->stage[stage].hwStageId) {
                    break;
                }
            }
            if (unitData->devInfo->stages <= stage) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                                 (_BSL_BCM_MSG_NO_UNIT("unit %d has host extend"
                                                   " feature enabled but does"
                                                   " not appear to have an"
                                                   " ingress stage"),
                                  unit));
            }
            if (!SOC_WARM_BOOT(unit)) {
                /* Cold boot -  allocate sw-state resources */
                /*
                 * Was:
                 *   unitData->preselRefs[presel] = 1;
                 */
                BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_PRESELREFS.set(unit, presel, 1)) ;
                /*
                 * Was:
                 *   BCM_FIELD_PRESEL_ADD(unitData->preselInUse, presel);
                 * Which was equivalent to:
                 *   SHR_BITSET(unitData->preselInUse.w, presel) ;
                 */
                BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_PRESELINUSE_W.bit_set(unit, presel)) ;
            }
            FIELD_ACCESS.stageD.pfgsIPv4.w.bit_set(unit, stage, presel); /*            BCM_FIELD_PRESEL_ADD(unitData->stageD[stage].pfgsIPv4, presel);*/
        } /* if (IPv4 host extend is enabled) */
    } /* if (SOC_IS_PETRAB(unit)) */
#else /* def BCM_PETRAB_SUPPORT*/
    BCM_EXIT;
#endif /* def BCM_PETRAB_SUPPORT */
exit:
    BCMDNX_FUNC_RETURN;
}

#ifdef BROADCOM_DEBUG
/*
 *   Function
 *      _bcm_dpp_field_presel_single_dump
 *   Purpose
 *      Dump the settings for a single preselector
 *   Parameters
 *      (in) bcm_dpp_field_info_OLD_t *unitData = pointer to unit data
 *      (in) const char *prefix = pointer to base prefix
 *      (in) const char *newPrefix = pointer to prefix plus indention
 *      (in) _bcm_dpp_field_presel_idx_t presel = which preselector to dump
 *   Returns
 *      int (implied cast from bcm_error_t)
 *                    BCM_E_NONE if sccessful
 *                    BCM_E_* appropriately otherwise
 *   Notes
 *     Since we do not cache preselectors, the 'verify' feature of the
 *     qualifier dump is not used here (it was just read from hardware, so what
 *     point is seen in comparing the value read to itself?).
 */
STATIC int
_bcm_dpp_field_presel_single_dump(bcm_dpp_field_info_OLD_t *unitData,
                                  const char *prefix,
                                  const char *newPrefix,
                                  _bcm_dpp_field_presel_idx_t presel)
{
    _bcm_dpp_field_qual_t preselQual[SOC_PPD_FP_NOF_QUALS_PER_DB_MAX];
    _bcm_dpp_field_stage_idx_t stage;
    unsigned int index;
    unsigned int count;
    int dumpedRanges;
    int result;
    int unit;
    unsigned int preselRefs_element ;

    unit = unitData->unit;

    result = FIELD_ACCESS_PRESELREFS.get(unit, presel, &preselRefs_element) ;
    if (BCM_E_NONE == result) {
        LOG_CLI((BSL_META_U(unit,
             "%sPreselector %d: refs = %d\n"),
             prefix,
             presel,
             preselRefs_element));
        result = _bcm_dpp_field_presel_info_get(unitData,
                                            presel,
                                            SOC_PPD_FP_NOF_QUALS_PER_DB_MAX,
                                            &stage,
                                            &(preselQual[0]));
        if (BCM_E_NONE == result) {
            LOG_CLI((BSL_META_U(unit,
                 "%s  Stage     = %s (%d)\n"),
                 prefix,
                 _BCM_DPP_FIELD_STAGE_DEV_INFO(unit, stage).stageName,
                 stage));
            /* dump qualifiers */
            for (index = 0, count = 0, dumpedRanges = FALSE, result = BCM_E_NONE;
                 (BCM_E_NONE == result) &&
                 (index < SOC_PPD_FP_NOF_QUALS_PER_DB_MAX);
                 index++) {
                result = _bcm_dpp_field_entry_qual_dump(unitData,
                                                    0 ,
                                                    &(preselQual[0]),
#if _BCM_DPP_FIELD_DUMP_VERIFY_PPD
                                                    NULL,
#endif /* _BCM_DPP_FIELD_DUMP_VERIFY_PPD */
                                                    index,
                                                    !dumpedRanges,
                                                    newPrefix);
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
                     "%s    (none; matches no traffic)\n"),
                     prefix));
            }
        } else { /* if (BCM_E_NONE == result) */
            LOG_CLI((BSL_META_U(unit,
                 "%s  (unable to access: %d (%s)\n"),
                 prefix,
                 result,
                 _SHR_ERRMSG(result)));
        } /* if (BCM_E_NONE == result) */
    } else { /* if (BCM_E_NONE == result) */
        LOG_CLI((BSL_META_U(unit,
             "%s  (unable to access: preselRefs[%d]: %d (%s))\n"),
             prefix,
             presel,
             result,
             _SHR_ERRMSG(result)));
    } /* if (BCM_E_NONE == result) */
    return result;
}

int
_bcm_dpp_field_presel_dump(bcm_dpp_field_info_OLD_t *unitData,
                           const char *prefix)
{
    bcm_field_presel_t index;
    int count;
    int result = BCM_E_NONE;
    char *newPrefix;
    int unit;
    _bcm_dpp_field_presel_idx_t preselLimit;
    uint8 bit_value ;

    unit = unitData->unit;
    result = FIELD_ACCESS.preselLimit.get(unit, &preselLimit) ;
    if (result != BCM_E_NONE) {
        goto exit ;
    }
    count = sal_strlen(prefix);
    newPrefix = sal_alloc(count + 6, "prefix workspace");
    if (!newPrefix) {
        LOG_CLI((BSL_META_U(unit,
                            "%sUnable to dump preselectors: could not allocate %d"
                 " bytes workspace\n"),
                 prefix,
                 count + 6));
        result = BCM_E_MEMORY;
        /*
         * No memory was allocated - No need to free.
         */
        goto exit ;
    }
    sal_snprintf(newPrefix,count + 5, "%s    ", prefix);
    for (index = 0, count = 0; index < preselLimit; index++) {
        /*
         * Was:
         *   if (BCM_FIELD_PRESEL_TEST(unitData->preselInUse, index))
         * Which was equivalent to:
         *   if (SHR_BITGET(unitData->preselInUse.w, index))
         */
        result = FIELD_ACCESS_PRESELINUSE_W.bit_get(unit, index, &bit_value) ;
        if (result != BCM_E_NONE) {
            break ;
        }
        if (bit_value) {
            count++;
            _bcm_dpp_field_presel_single_dump(unitData,
                                              prefix,
                                              newPrefix,
                                              index);
        } /* if (this preselector is in use) */
    } /* for (index = 0; index < preselLimit; index++) */
    if (!count) {
        LOG_CLI((BSL_META_U(unit,
                            "%s(no preselectors this unit)\n"), prefix));
    }
    sal_free(newPrefix);
exit:
    return result;
}
#endif /* def BROADCOM_DEBUG */

/*
 *   Function
 *      _bcm_dpp_field_presel_alloc
 *   Purpose
 *      Allocate a preselector
 *   Parameters
 *      (in) bcm_dpp_field_info_OLD_t *unitData = pointer to unit data
 *      (in) int with_id = TRUE if caller supplies ID, FALSE otherwise
 *      (in/out) bcm_field_presel_t *presel = pointer to the (place to put) ID
 *   Returns
 *      int (implied cast from bcm_error_t)
 *                    BCM_E_NONE if sccessful
 *                    BCM_E_* appropriately otherwise
 */
STATIC int
_bcm_dpp_field_presel_alloc(bcm_dpp_field_info_OLD_t *unitData,
                            _bcm_dpp_field_presel_alloc_flags_t flags,
                            bcm_field_presel_t *presel)
{
    bcm_field_presel_t index;
    _bcm_dpp_field_presel_idx_t preselLimit;
    uint8 bit_value ;

    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.preselLimit.get(unit, &preselLimit)) ;
    if (flags & _BCM_DPP_PRESEL_ALLOC_WITH_ID) {
        index = *presel;
        if ((0 > index) || (preselLimit <= index)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d does not support"
                                               " preselector ID %d"),
                              unit,
                              *presel));
        }
        /*
         * Was:
         *   if (BCM_FIELD_PRESEL_TEST(unitData->preselInUse, index))
         * Which was equivalent to:
         *   if (SHR_BITGET(unitData->preselInUse.w, index))
         */
        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_PRESELINUSE_W.bit_get(unit, index, &bit_value)) ;
        if (bit_value) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d preselector ID %d"
                                               " is already in use"),
                              unit,
                              *presel));
        }
    } else { /* if (flags & _BCM_PETRA_PRESEL_ALLOC_WITH_ID) */
        if (flags & _BCM_DPP_PRESEL_ALLOC_DOWN) {
            for (index = 0; index < preselLimit; index++) {
                /*
                 * Was:
                 *   if (!BCM_FIELD_PRESEL_TEST(unitData->preselInUse, (preselLimit - index) - 1))
                 * Which was equivalent to:
                 *   if (!SHR_BITGET(unitData->preselInUse.w, (preselLimit - index) - 1))
                 */
                BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_PRESELINUSE_W.bit_get(unit, ((preselLimit - index) - 1), &bit_value)) ;
                if (!bit_value) {
                    break;
                }
            }
        } else { /* if (flags & _BCM_DPP_PRESEL_ALLOC_DOWN) */
            for (index = 0; index < preselLimit; index++) {
                /*
                 * Was:
                 *   if (!BCM_FIELD_PRESEL_TEST(unitData->preselInUse, index))
                 * Which was equivalent to:
                 *   if (!SHR_BITGET(unitData->preselInUse.w, index))
                 */
                BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_PRESELINUSE_W.bit_get(unit, index, &bit_value)) ;
                if (!bit_value) {
                    break;
                }
            }
        } /* if (flags & _BCM_DPP_PRESEL_ALLOC_DOWN) */
        if (preselLimit <= index) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d has no preselectors"
                                               " available"),
                              unit));
        }
        if (flags & _BCM_DPP_PRESEL_ALLOC_DOWN) {
            index = (preselLimit - index) - 1;
        }
    } /* if (flags & _BCM_PETRA_PRESEL_ALLOC_WITH_ID) */
    /* set 'blank' state for the new preselector */
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_presel_info_set(unitData,
                                                           index,
                                                           0,
                                                           unitData->devInfo->stages,

                                                           NULL));
    if (!SOC_WARM_BOOT(unit)) {
        /* Cold boot -  allocate sw-state resources */
        /*
         * Was:
         *   BCM_FIELD_PRESEL_ADD(unitData->preselInUse, index)
         * Which was equivalent to:
         *   SHR_BITSET(unitData->preselInUse.w, index)
         */
        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_PRESELINUSE_W.bit_set(unit, index)) ;
    }
    *presel = index;
#ifdef BCM_WARM_BOOT_SUPPORT
#if (0)
/* { */
    /*
     * All 'save' and 'sync' of old sw state are dropped.
     */
    _bcm_dpp_field_presel_all_wb_save(unitData, NULL, NULL);
/* } */
#endif
#endif /* def BCM_WARM_BOOT_SUPPORT */
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *   Function
 *      _bcm_dpp_field_presel_free
 *   Purpose
 *      Free a preselector
 *   Parameters
 *      (in) bcm_dpp_field_info_OLD_t *unitData = pointer to unit data
 *      (in) bcm_field_presel_t presel = presel to be freed
 *   Returns
 *      int (implied cast from bcm_error_t)
 *                    BCM_E_NONE if sccessful
 *                    BCM_E_* appropriately otherwise
 */
STATIC int
_bcm_dpp_field_presel_free(bcm_dpp_field_info_OLD_t *unitData,
                           bcm_field_presel_t presel)
{
    _bcm_dpp_field_presel_idx_t preselLimit;
    uint8 bit_value ;
    unsigned int preselRefs_element ;

    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.preselLimit.get(unit, &preselLimit)) ;
    if ((0 > presel) || (preselLimit <= presel)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d does not support"
                                           " preselector ID %d"),
                          unit,
                          presel));
    }
    /*
     * Was:
     *   if (!BCM_FIELD_PRESEL_TEST(unitData->preselInUse, presel))
     * Which was equivalent to:
     *   if (!SHR_BITGET(unitData->preselInUse.w, presel))
     */
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_PRESELINUSE_W.bit_get(unit, presel, &bit_value)) ;
    if (!bit_value) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d preselector ID %d"
                                           " is not in use"),
                          unit,
                          presel));
    }
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_PRESELREFS.get(unit, presel, &preselRefs_element)) ;
    if (preselRefs_element) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d preselector id %d"
                                           " is still used by %d groups"),
                          unit,
                          presel,
                          preselRefs_element));
    }

    /* If port profiles are defined - clear them */
    BCMDNX_IF_ERR_EXIT(
        _bcm_dpp_field_presel_port_profile_clear_all(unitData,
                                                     (presel | BCM_FIELD_QUALIFY_PRESEL)));

    if ( soc_property_get(unit, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE, FALSE)) {
        
         BCMDNX_IF_ERR_EXIT( _bcm_dpp_field_presel_destroy(unitData, presel));
    }
    else
    {
        /* set 'blank' state for the discarded preselector */
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_presel_info_set(unitData,
                                                               presel,
                                                               0,
                                                               unitData->devInfo->stages,
                                                               NULL));
    }
    if (!SOC_WARM_BOOT(unit)) {
        /* Cold boot -  allocate sw-state resources */
        /*
         * Was:
         *   BCM_FIELD_PRESEL_REMOVE(unitData->preselInUse, presel);
         * Which was equivalent to:
         *   SHR_BITCLR(unitData->preselInUse.w, presel)
         */
        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_PRESELINUSE_W.bit_clear(unit, presel)) ;
    }
#ifdef BCM_WARM_BOOT_SUPPORT
#if (0)
/* { */
    /*
     * All 'save' and 'sync' of old sw state are dropped.
     */
    _bcm_dpp_field_presel_all_wb_save(unitData, NULL, NULL);
/* } */
#endif
#endif /* def BCM_WARM_BOOT_SUPPORT */
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_field_presel_qualify(bcm_dpp_field_info_OLD_t *unitData,
                              bcm_field_presel_t presel,
                              bcm_field_qualify_t qualifier,
                              unsigned int count,
                              const uint64 *data,
                              const uint64 *mask)
{
#ifdef BCM_PETRAB_SUPPORT
    SOC_PPD_FP_PFG_INFO *pfgInfo = NULL;
    SOC_SAND_SUCCESS_FAILURE success;
    uint32 sandResult;
    bcm_field_header_format_set_t hfs;
    bcm_field_header_format_t hf;
#endif /* def BCM_PETRAB_SUPPORT */
#ifdef BCM_ARAD_SUPPORT
    _bcm_dpp_field_qual_t *preselQuals = NULL;
    _bcm_dpp_field_stage_idx_t stage;
#endif /* def BCM_ARAD_SUPPORT */
    _bcm_dpp_field_presel_idx_t preselLimit;
    uint8 bit_value ;
    unsigned int preselRefs_element ;

    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.preselLimit.get(unit, &preselLimit)) ;
    if ((0 > presel) || (preselLimit <= presel)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d preselector %d not valid"),
                          unit,
                          presel));
    }
    /*
     * Was:
     *   if (!BCM_FIELD_PRESEL_TEST(unitData->preselInUse, presel))
     * Which was equivalent to:
     *   if (!SHR_BITGET(unitData->preselInUse.w, presel))
     */
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_PRESELINUSE_W.bit_get(unit, presel, &bit_value)) ;
    if (!bit_value) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d preselector %d not in use"),
                          unit,
                          presel));
    }
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_PRESELREFS.get(unit, presel, &preselRefs_element)) ;
    if (preselRefs_element) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d presleector %d is"
                                           " referenced by %d groups, so it"
                                           " can not be changed"),
                          unit,
                          presel,
                          preselRefs_element));
    }
    if ((0 > qualifier) || (bcmFieldQualifyCount <= qualifier)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG_NO_UNIT("qualifier type %d invalid"),
                          qualifier));
    }
    if (1 > count) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d requires at least one"
                                           " octbyte for qualifier data/mask"
                                           " arguments"),
                          unit));
    }
    /* general validation for certain arguments */
    switch (qualifier) {
    case bcmFieldQualifyStage:
        if (unitData->devInfo->stages <= COMPILER_64_LO(*data)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d stage %u invalid"),
                              unit,
                              COMPILER_64_LO(*data)));
        }
        if (0 == (_BCM_DPP_FIELD_STAGE_DEV_INFO(unit, COMPILER_64_LO(*data)).stageFlags &
                  _BCM_DPP_FIELD_DEV_STAGE_USES_PRESELECTORS)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d does not support"
                                               " preselectors on stage %s\n"),
                              unit,
                              _BCM_DPP_FIELD_STAGE_DEV_INFO(unit, COMPILER_64_LO(*data)).stageName));
        }
        break;
    default:
        /* no general checks */
        break;
    }
#ifdef BCM_PETRAB_SUPPORT
    if (SOC_IS_PETRAB(unit)) {
        /*
         *  PetraB does not use the qualifier array, and it can perform direct
         *  overwrite of a given preselector.  We will therefore do all of the
         *  updates for PetraB here.
         */
        pfgInfo = sal_alloc(sizeof(*pfgInfo), "PFG workspace");
        if (!pfgInfo) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d unable to allocate %u"
                                               " bytes for PFG workspace"),
                              unit,
                              (uint32)sizeof(*pfgInfo)));
        }
        sandResult = soc_ppd_fp_packet_format_group_get(unitData->unitHandle,
                                                        presel,
                                                        pfgInfo);
        BCMDNX_IF_ERR_EXIT_MSG(handle_sand_result(sandResult),
                            (_BSL_BCM_MSG_NO_UNIT("unable to read unit %d"
                                              " presel %u"),
                             unit,
                             presel));

        /* Always set the PP-Port: they may have be down previously */
        pfgInfo->pp_ports_bmp.arr[0] = 0xFFFFFFFF;
        pfgInfo->pp_ports_bmp.arr[1] = 0xFFFFFFFF;

        /* Process the possible qualifiers to what it recognizes */
        switch (qualifier) {
        case bcmFieldQualifyL2Format:
            /* special case of HeaderFormatSet */
            if (!COMPILER_64_IS_ZERO(*mask)) {
                if (!COMPILER_64_IS_ZERO(*data)) {
                    pfgInfo->hdr_format_bmp = (SOC_PPD_FP_PKT_HDR_TYPE_ETH |
                                               SOC_PPD_FP_PKT_HDR_TYPE_ETH_ETH);
                } else {
                    pfgInfo->hdr_format_bmp = 0;
                }
            }
            break;
        case bcmFieldQualifyIp4:
            /* special case of HeaderFormatSet */
            if (!COMPILER_64_IS_ZERO(*mask)) {
                if (!COMPILER_64_IS_ZERO(*data)) {
                    pfgInfo->hdr_format_bmp = (SOC_PPD_FP_PKT_HDR_TYPE_IPV4_ETH);
                } else {
                    pfgInfo->hdr_format_bmp = 0;
                }
            }
            break;
        case bcmFieldQualifyIp6:
            /* special case of HeaderFormatSet */
            if (!COMPILER_64_IS_ZERO(*mask)) {
                if (!COMPILER_64_IS_ZERO(*data)) {
                    pfgInfo->hdr_format_bmp = (SOC_PPD_FP_PKT_HDR_TYPE_IPV6_ETH);
                } else {
                    pfgInfo->hdr_format_bmp = 0;
                }
            }
            break;
        case bcmFieldQualifyMpls:
            /* special case of HeaderFormatSet */
            if (!COMPILER_64_IS_ZERO(*mask)) {
                if (!COMPILER_64_IS_ZERO(*data)) {
                    pfgInfo->hdr_format_bmp = (SOC_PPD_FP_PKT_HDR_TYPE_MPLS1_ETH |
                                               SOC_PPD_FP_PKT_HDR_TYPE_MPLS2_ETH |
                                               SOC_PPD_FP_PKT_HDR_TYPE_MPLS3_ETH);
                } else {
                    pfgInfo->hdr_format_bmp = 0;
                }
            }
            break;
        case bcmFieldQualifyStage:
            
            pfgInfo->stage = _BCM_DPP_FIELD_STAGE_DEV_INFO(unit, COMPILER_64_LO(*data)).hwStageId;
            break;
        case bcmFieldQualifyHeaderFormat:
            /* special case of HeaderFormatSet */
            
            BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_HeaderFormat_ppd_to_bcm(COMPILER_64_LO(*data),
                                                                           COMPILER_64_LO(*mask),
                                                                           &hf));
            BCM_FIELD_HEADER_FORMAT_SET_INIT(hfs);
            BCM_FIELD_HEADER_FORMAT_SET_ADD(hfs, hf);
            BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_HeaderFormatSet_bcm_to_ppd(&hfs,
                                                                              &(pfgInfo->hdr_format_bmp)));
            break;
        case bcmFieldQualifyHeaderFormatSet:
            
            pfgInfo->hdr_format_bmp = COMPILER_64_LO(*data);
            break;
        default:
            /* not a supported qualifier */
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d does not support"
                                               " qualifier %s (%d) for"
                                               " preselectors\n"),
                              unit,
                              _bcm_dpp_field_qual_name[qualifier],
                              qualifier));
        } /* switch (qualifier) */
        /* write the changed PFG back */
        sandResult = soc_ppd_fp_packet_format_group_set(unitData->unitHandle,
                                                        presel,
                                                        pfgInfo,
                                                        &success);
        BCMDNX_IF_ERR_EXIT_MSG(handle_sand_result(sandResult),
                            (_BSL_BCM_MSG_NO_UNIT("unable to write unit %d"
                                              " presel %u)"),
                             unit,
                             presel));
        BCMDNX_IF_ERR_EXIT_MSG(translate_sand_success_failure(success),
                            (_BSL_BCM_MSG_NO_UNIT("write unit %d presel %u"
                                              " unsucessful"),
                             unit,
                             presel));
    } /* if (SOC_IS_PETRAB(unit)) */
#endif /* def BCM_PETRAB_SUPPORT */
#ifdef BCM_ARAD_SUPPORT
    if (SOC_IS_ARAD(unit)) {
        switch (qualifier) {
            case bcmFieldQualifyStage:
            {
                /* stage is special and handled locally */
                
                preselQuals = sal_alloc(sizeof(*preselQuals) * SOC_PPD_FP_NOF_QUALS_PER_DB_MAX,
                                        "preselector workspace");
                if (!preselQuals) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY,
                                     (_BSL_BCM_MSG_NO_UNIT("unit %d unable to allocate"
                                                       " %u bytes for presel"
                                                       " workspace"),
                                      unit,
                                      (uint32)sizeof(*preselQuals) * SOC_PPD_FP_NOF_QUALS_PER_DB_MAX));
                }
                BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_presel_info_get(unitData,
                                                                       presel,
                                                                       SOC_PPD_FP_NOF_QUALS_PER_DB_MAX,
                                                                       &stage,
                                                                       preselQuals));
                stage = COMPILER_64_LO(*data);
                BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_presel_info_set(unitData,
                                                                       presel,
                                                                       SOC_PPD_FP_NOF_QUALS_PER_DB_MAX,
                                                                       stage,
                                                                       preselQuals));
                break;
            }
            default:
            {
                int32 qualMaps_handle ;

                /* general case goes to generic qualifier code */
                BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_QUALMAPS.get(unit, qualifier, &qualMaps_handle)) ;
                if (qualMaps_handle) {
                    /* this unit supports this qualifier */
                    BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_qualifier_set(unitData,
                                                                         presel | BCM_FIELD_QUALIFY_PRESEL,
                                                                         qualifier,
                                                                         count,
                                                                         data,
                                                                         mask));
                } else { /* if (qualMaps_handle) */
                    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL,
                                     (_BSL_BCM_MSG_NO_UNIT("unit %d does not support"
                                                       " qualifier %d (%s)\n"),
                                      unit,
                                      qualifier,
                                      _bcm_dpp_field_qual_name[qualifier]));
                } /* if (qualMaps_handle) */
            }
        } /* switch (qualifier) */
    } /* if (SOC_IS_ARAD(unit)) */
#endif /* def BCM_ARAD_SUPPORT */
exit:
#ifdef BCM_PETRAB_SUPPORT
    if (pfgInfo) {
        sal_free(pfgInfo);
    }
#endif /* def BCM_PETRAB_SUPPORT */
#ifdef BCM_ARAD_SUPPORT
    if (preselQuals) {
        sal_free(preselQuals);
    }
#endif /* def BCM_ARAD_SUPPORT */
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_field_presel_qualify_get(bcm_dpp_field_info_OLD_t *unitData,
                                  bcm_field_presel_t presel,
                                  bcm_field_qualify_t qualifier,
                                  unsigned int count,
                                  uint64 *data,
                                  uint64 *mask)
{
#ifdef BCM_PETRAB_SUPPORT
    SOC_PPD_FP_PFG_INFO *pfgInfo = NULL;
    uint32 sandResult;
    bcm_field_header_format_set_t hfs;
    bcm_field_header_format_t hf;
    unsigned int index;
    uint32 temp, ppdMask;
#endif /* def BCM_PETRAB_SUPPORT */
#ifdef BCM_ARAD_SUPPORT
    _bcm_dpp_field_stage_idx_t stage;
#endif /* def BCM_ARAD_SUPPORT */
    _bcm_dpp_field_presel_idx_t preselLimit;
    uint8 bit_value ;
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.preselLimit.get(unit, &preselLimit)) ;
    if ((0 > presel) || (preselLimit <= presel)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d preselector %d is invalid"),
                          unit,
                          presel));
    }
    /*
     * Was:
     *   if (!BCM_FIELD_PRESEL_TEST(unitData->preselInUse, presel))
     * Which was equivalent to:
     *   if (!SHR_BITGET(unitData->preselInUse.w, presel))
     */
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_PRESELINUSE_W.bit_get(unit, presel, &bit_value)) ;
    if (!bit_value) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d preselector %d not in use"),
                          unit,
                          presel));
    }
    if ((0 > qualifier) || (bcmFieldQualifyCount <= qualifier)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG_NO_UNIT("qualifier type %d invalid"),
                          qualifier));
    }
    if (1 > count) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d requires at least one"
                                           " octbyte for qualifier data/mask"
                                           " arguments"),
                          unit));
    }
#ifdef BCM_PETRAB_SUPPORT
    if (SOC_IS_PETRAB(unit)) {
        
        for (index = 0; index < count; index ++) {
            COMPILER_64_ZERO(data[index]);
            COMPILER_64_ZERO(mask[index]);
        }
        /*
         *  PetraB does not use the qualifier array, and it can perform direct
         *  oveerwrite of a given preselector.  We will therefore do all of the
         *  updates for PetraB here.
         */
        pfgInfo = sal_alloc(sizeof(*pfgInfo), "PFG workspace");
        if (!pfgInfo) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d unable to allocate %u"
                                               " bytes for PFG workspace"),
                              unit,
                              (uint32)sizeof(*pfgInfo)));
        }
        sandResult = soc_ppd_fp_packet_format_group_get(unitData->unitHandle,
                                                        presel,
                                                        pfgInfo);
        BCMDNX_IF_ERR_EXIT_MSG(handle_sand_result(sandResult),
                            (_BSL_BCM_MSG_NO_UNIT("unable to read unit %d"
                                              " presel %u"),
                             unit,
                             presel));
        /* processs the possible qualifiers to what it recognises */
        switch (qualifier) {
        case bcmFieldQualifyL2Format:
            if ((SOC_PPD_FP_PKT_HDR_TYPE_ETH |
                 SOC_PPD_FP_PKT_HDR_TYPE_ETH_ETH) ==
                pfgInfo->hdr_format_bmp) {
                COMPILER_64_SET(*data, 0, 1);
                COMPILER_64_SET(*mask, 0, 1);
            } else {
                COMPILER_64_SET(*data,0, 0);
                COMPILER_64_SET(*mask,0, 1);
            }
            break;
        case bcmFieldQualifyIp4:
            if ((SOC_PPD_FP_PKT_HDR_TYPE_IPV4_ETH) ==
                pfgInfo->hdr_format_bmp) {
                COMPILER_64_SET(*data, 0, 1);
                COMPILER_64_SET(*mask, 0, 1);
            } else {
                COMPILER_64_SET(*data, 0, 0);
                COMPILER_64_SET(*mask, 0, 1);
            }
            break;
        case bcmFieldQualifyIp6:
            if ((SOC_PPD_FP_PKT_HDR_TYPE_IPV6_ETH) ==
                pfgInfo->hdr_format_bmp) {
                COMPILER_64_SET(*data, 0, 1);
                COMPILER_64_SET(*mask, 0, 1);
            } else {
                COMPILER_64_SET(*data, 0, 0);
                COMPILER_64_SET(*mask, 0, 1);
            }
            break;
        case bcmFieldQualifyMpls:
            if ((SOC_PPD_FP_PKT_HDR_TYPE_MPLS1_ETH |
                 SOC_PPD_FP_PKT_HDR_TYPE_MPLS2_ETH |
                 SOC_PPD_FP_PKT_HDR_TYPE_MPLS3_ETH) ==
                pfgInfo->hdr_format_bmp) {
                COMPILER_64_SET(*data, 0, 1);
                COMPILER_64_SET(*mask, 0, 1);
            } else {
                COMPILER_64_SET(*data, 0, 0);
                COMPILER_64_SET(*mask, 0, 1);
            }
            break;
        case bcmFieldQualifyHeaderFormat:
            BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_HeaderFormatSet_ppd_to_bcm(pfgInfo->hdr_format_bmp,
                                                                              &hfs));
            for (hf = 0, index = 0; hf < bcmFieldHeaderFormatCount; hf++) {
                if (BCM_FIELD_HEADER_FORMAT_SET_TEST(hfs, hf)) {
                    temp = hf;
                    index++;
                }
            }
            if (0 == index) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                                 (_BSL_BCM_MSG_NO_UNIT("unit %d preselector %d has"
                                                   " no acceptable header format"),
                                  unit,
                                  presel));
            } else if (1 != index) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                                 (_BSL_BCM_MSG_NO_UNIT("unit %d preselector %d has"
                                                   " more than one header format;"
                                                   " get HeaderFormatSet instead"),
                                  unit,
                                  presel));
            }
            hf = temp;
            BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_HeaderFormat_bcm_to_ppd(hf,
                                                                           &temp,
                                                                           &ppdMask));
            COMPILER_64_SET(*data, 0, temp);
            COMPILER_64_SET(*mask, 0, ppdMask);
            break;
        case bcmFieldQualifyHeaderFormatSet:
             COMPILER_64_SET(*data, 0, pfgInfo->hdr_format_bmp);
             COMPILER_64_SET(*mask, ~0, ~0);
            break;
        case bcmFieldQualifyStage:
            for (index = 0; index < unitData->devInfo->stages; index++) {
                if (_BCM_DPP_FIELD_STAGE_DEV_INFO(unit, index).hwStageId ==
                    pfgInfo->stage) {
                    break;
                }
            }
            COMPILER_64_SET(*data, 0, index);
            COMPILER_64_SET(*mask, ~0, ~0);
            break;
        default:
            /* not a supported qualifier */
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d does not support"
                                               " qualifier %s (%d) for"
                                               " preselectors\n"),
                              unit,
                              _bcm_dpp_field_qual_name[qualifier],
                              qualifier));
        } /* switch (qualifier) */
    } /* if (SOC_IS_PETRAB(unit)) */
#endif /* def BCM_PETRAB_SUPPORT */
#ifdef BCM_ARAD_SUPPORT
    if (SOC_IS_ARAD(unit)) {
        switch (qualifier) {
            case bcmFieldQualifyStage:
            {
                /* stage is special and handled locally */
                BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_presel_info_get(unitData,
                                                                       presel,
                                                                       0,
                                                                       &stage,
                                                                       NULL));
                COMPILER_64_SET(*data, 0, stage);
                COMPILER_64_SET(*mask, ~0, ~0);
                break;
            }
            default:
            {
                int32 qualMaps_handle ;

                BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_QUALMAPS.get(unit, qualifier, &qualMaps_handle)) ;
                if (qualMaps_handle) {
                    /* this unit supports this qualifier */
                    BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_qualifier_get(unitData,
                                                                         presel | BCM_FIELD_QUALIFY_PRESEL,
                                                                         qualifier,
                                                                         count,
                                                                         data,
                                                                         mask));
                } else { /* if (qualMaps_handle) */
                    /* this unit does not support this qualifier */
                    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL,
                                     (_BSL_BCM_MSG_NO_UNIT("unit %d does not support"
                                                       " qualifier %s (%d) for"
                                                       " preselectors\n"),
                                      unit,
                                      _bcm_dpp_field_qual_name[qualifier],
                                      qualifier));
                } /* if (qualMaps_handle) */
            }
        } /* switch (qualifier) */
    } /* if (SOC_IS_ARAD(unit)) */
#endif /* def BCM_ARAD_SUPPORT */
exit:
#ifdef BCM_PETRAB_SUPPORT
    if (pfgInfo) {
        sal_free(pfgInfo);
    }
#endif /* def BCM_PETRAB_SUPPORT */
    BCMDNX_FUNC_RETURN;
}

#ifdef BCM_PETRA_SUPPORT
/*
 *   Function
 *      _bcm_dpp_field_presel_implied_get
 *   Purpose
 *      Configure or retrieve an 'implied' presel
 *   Parameters
 *      (in) bcm_dpp_field_info_OLD_t *unitData = pointer to unit data to init
 *      (in) _bcm_dpp_field_implied_presel_t which = which one to set up
 *      (in) _bcm_dpp_field_stage_idx_t stage = stage for consideration
 *      (out) bcm_field_presel_set_t *preselSet = pointer to presel set
 *   Returns
 *      int (implied cast from bcm_error_t)
 *                    BCM_E_NONE if sccessful
 *                    BCM_E_* appropriately otherwise
 *   Notes
 *     Caller must initialise preselector set to empty before first call.  This
 *     function will add the preselectors to the set as appropriate.
 */
STATIC int
_bcm_dpp_field_presel_implied_get(bcm_dpp_field_info_OLD_t *unitData,
                                  _bcm_dpp_field_implied_presel_t which,
                                  _bcm_dpp_field_stage_idx_t stage,
                                  bcm_field_presel_set_t *preselSet)
{
    bcm_field_presel_set_t *baseSet;
    uint32 headerFormats;
    uint64 edata;
    uint64 emask;
    bcm_field_presel_t presel;
    unsigned int index;
    int result = BCM_E_INTERNAL;
    int auxRes;
    bcm_field_presel_set_t pfgs;
    _bcm_dpp_field_presel_idx_t preselLimit;
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.preselLimit.get(unit, &preselLimit)) ;
    switch (which) {
    case _BCM_DPP_IMPLIED_PRESEL_L2:
        FIELD_ACCESS.stageD.pfgsEther.get(unit, stage, &pfgs);
        headerFormats = (SOC_PPD_FP_PKT_HDR_TYPE_ETH |
                         SOC_PPD_FP_PKT_HDR_TYPE_ETH_ETH);
        break;
    case _BCM_DPP_IMPLIED_PRESEL_IPV4:
        FIELD_ACCESS.stageD.pfgsIPv4.get(unit, stage, &pfgs);
        headerFormats = (SOC_PPD_FP_PKT_HDR_TYPE_IPV4_ETH);
        break;
    case _BCM_DPP_IMPLIED_PRESEL_IPV6:
        FIELD_ACCESS.stageD.pfgsIPv6.get(unit, stage, &pfgs);
        headerFormats = (SOC_PPD_FP_PKT_HDR_TYPE_IPV6_ETH);
        break;
    case _BCM_DPP_IMPLIED_PRESEL_MPLS:
        FIELD_ACCESS.stageD.pfgsMPLS.get(unit, stage, &pfgs);
        headerFormats = (SOC_PPD_FP_PKT_HDR_TYPE_MPLS1_ETH |
                         SOC_PPD_FP_PKT_HDR_TYPE_MPLS2_ETH |
                         SOC_PPD_FP_PKT_HDR_TYPE_MPLS3_ETH);
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d does not support implied"
                                           " preselector type %d"),
                          unit,
                          which));
    } /* switch (which) */
    baseSet = &pfgs;

    result = BCM_E_NOT_FOUND;
    index = preselLimit;
    while (index > 0) {
        index--;
        if (BCM_FIELD_PRESEL_TEST(*baseSet, index)) {
            /* found one! */
            result = BCM_E_NONE;
            break;
        }
    } /* while (index > 0) */
    if (BCM_E_NONE != result) {
        /* did not find the appropriate preselector set; must build it */
        result = _bcm_dpp_field_presel_alloc(unitData,
                                             _BCM_DPP_PRESEL_ALLOC_DOWN,
                                             &presel);
        if (BCM_E_NONE != result) {
            LOG_ERROR(BSL_LS_BCM_FP,
                      (BSL_META_U(unit,
                                  "unable to allocate unit %d stage %u"
                                  " implied presel type %d: %d (%s)"),
                       unit,
                       stage,
                       which,
                       result,
                       _SHR_ERRMSG(result)));
        }
        if (BCM_E_NONE == result) {
            /* mark this preselector as used for this purpose */
            BCM_FIELD_PRESEL_ADD(*baseSet, presel);
            /* qualify the preselector for the requested stage */
            COMPILER_64_SET(edata, 0, stage);
            COMPILER_64_SET(emask, 0, ~0);
            result = _bcm_dpp_field_presel_qualify(unitData,
                                                   presel,
                                                   bcmFieldQualifyStage,
                                                   1,
                                                   &edata,
                                                   &emask);
            if (BCM_E_NONE != result) {
                LOG_ERROR(BSL_LS_BCM_FP,
                          (BSL_META_U(unit,
                                      "unable to add bcmFieldQualifyStage"
                                      " to unit %d stage %u implied presel"
                                      " type %d: %d (%s)"),
                           unit,
                           stage,
                           which,
                           result,
                           _SHR_ERRMSG(result)));
            }
        } /* if (BCM_E_NONE == result) */
        if (BCM_E_NONE == result) {
            /* qualify the preselector for the requested frame format set */
            COMPILER_64_SET(edata, 0, headerFormats);
            COMPILER_64_SET(emask, 0, ~0);
            result = _bcm_dpp_field_presel_qualify(unitData,
                                                   presel,
                                                   bcmFieldQualifyHeaderFormatSet,
                                                   1,
                                                   &edata,
                                                   &emask);
            if (BCM_E_NONE != result) {
                LOG_ERROR(BSL_LS_BCM_FP,
                          (BSL_META_U(unit,
                                      "unable to add"
                                      " bcmFieldQualifyHeaderFormatSet"
                                      " to unit %d stage %u implied presel"
                                      " type %d: %d (%s)"),
                           unit,
                           stage,
                           which,
                           result,
                           _SHR_ERRMSG(result)));
            }
        } /* if (BCM_E_NONE == result) */
        if (BCM_E_NONE != result) {
            /* something went wrong; back out what we have done */
            for (presel = 0; presel < preselLimit; presel++) {
                if (BCM_FIELD_PRESEL_TEST(*baseSet, index)) {
                    auxRes = _bcm_dpp_field_presel_free(unitData, presel);
                    if (BCM_E_NONE != auxRes) {
                        LOG_ERROR(BSL_LS_BCM_FP,
                                  (BSL_META_U(unit,
                                              "unable to free presel %d"
                                              " from unit %d stage %u"
                                              " implied set %d: %d (%s)\n"),
                                   presel,
                                   unit,
                                   stage,
                                   which,
                                   result,
                                   _SHR_ERRMSG(result)));
                    }
                } /* if (BCM_FIELD_PRESEL_TEST(*baseSet, index)) */
            } /* for (all possible PFGs) */
            /* make sure it is clear again */
            BCM_FIELD_PRESEL_INIT(*baseSet);
            /* no warm boot update because changes backed out */
        } /* if (BCM_E_NONE == result) */
#ifdef BCM_WARM_BOOT_SUPPORT
#if (0)
/* { */
    /*
     * All 'save' and 'sync' of old sw state are dropped.
     */
        if (BCM_E_NONE == result) {
            /* added appropriate preselector set to the stage, update WB */
            _bcm_dpp_field_presel_all_wb_save(unitData, NULL, NULL);
        }
/* } */
#endif
#endif /* def BCM_WARM_BOOT_SUPPORT */
    } /* if (BCM_E_NONE == result) */
    if (BCM_E_NONE == result) {
        /* we found (or built) the preselector set; add its members */
        for (presel = 0; presel < preselLimit; presel++) {
            if (BCM_FIELD_PRESEL_TEST(*baseSet, presel)) {
                BCM_FIELD_PRESEL_ADD(*preselSet,  presel);
            }
        } /* for (presel = 0; presel < unitData->preselLimit; presel++) */
    } /* if (BCM_E_NONE == result) */
    BCMDNX_IF_ERR_EXIT(result);
exit:
    BCMDNX_FUNC_RETURN;
}
#endif /* def BCM_PETRA_SUPPORT */

#ifdef BCM_PETRA_SUPPORT
int
_bcm_dpp_field_presel_implied_teardown(bcm_dpp_field_info_OLD_t *unitData,
                                       _bcm_dpp_field_group_t *groupData)
{
    bcm_field_presel_t index;
    int result = BCM_E_NONE;
    int auxRes;
    _bcm_dpp_field_presel_idx_t preselLimit;
    unsigned int preselRefs_element ;

    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.preselLimit.get(unit, &preselLimit)) ;
    if (0 == (_BCM_DPP_FIELD_STAGE_DEV_INFO(unit, groupData->stage).stageFlags &
              _BCM_DPP_FIELD_DEV_STAGE_USES_PRESELECTORS)) {
        /* nothing to do for preselectors on this stage */
        BCM_EXIT;
    }
    /* remove preselectors no longer referenced from PPD */
    if (0 == (groupData->groupFlags & _BCM_DPP_FIELD_GROUP_PRESEL)) {
        /* this group uses implied preselectors */
        for (index = 0; index < preselLimit; index++) {
            if (BCM_FIELD_PRESEL_TEST(groupData->preselSet, index)) {
                BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_PRESELREFS.get(unit, index, &preselRefs_element)) ;
                if (0 == preselRefs_element) {
                    /* no more references; remove from PPD */
                    auxRes = _bcm_dpp_field_presel_free(unitData, index);
                    if (BCM_E_NONE == auxRes) {
                        /* mark this preselector not in use by implied sets */
                        FIELD_ACCESS.stageD.pfgsEther.w.bit_clear(unit, groupData->stage, index);
                        FIELD_ACCESS.stageD.pfgsIPv4.w.bit_clear(unit, groupData->stage, index);
                        FIELD_ACCESS.stageD.pfgsIPv6.w.bit_clear(unit, groupData->stage, index);
                        FIELD_ACCESS.stageD.pfgsMPLS.w.bit_clear(unit, groupData->stage, index);
                    } else { /* if (BCM_E_NONE == auxRes) */
                        result = auxRes;
                    } /* if (BCM_E_NONE == auxRes) */
                } /* if (this preselector has zero remaining references) */
            } /* if (BCM_FIELD_PRESEL_TEST(groupData->preselSet, index)) */
        } /* for (index = 0; index < preselLimit; index++) */
#ifdef BCM_WARM_BOOT_SUPPORT
#if (0)
/* { */
    /*
     * All 'save' and 'sync' of old sw state are dropped.
     */
    _bcm_dpp_field_presel_all_wb_save(unitData, NULL, NULL);
/* } */
#endif

#endif /* def BCM_WARM_BOOT_SUPPORT */
    } /* if (0 == (groupData->groupFlags & _BCM_DPP_FIELD_GROUP_PRESEL)) */
    BCMDNX_IF_ERR_EXIT(result);
exit:
    BCMDNX_FUNC_RETURN;
}
#endif /* def BCM_PETRA_SUPPORT */

#ifdef BCM_PETRA_SUPPORT
int
_bcm_dpp_field_presel_implied_setup(bcm_dpp_field_info_OLD_t *unitData,
                                    _bcm_dpp_field_group_t *groupData)
{
    bcm_field_presel_set_t pset;
    int auxRes;
    int result = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;

    if (0 == (_BCM_DPP_FIELD_STAGE_DEV_INFO(unit, groupData->stage).stageFlags &
              _BCM_DPP_FIELD_DEV_STAGE_USES_PRESELECTORS)) {
        /* nothing to do for preselectors on this stage */
        BCM_EXIT;
    }
    if (0 == (groupData->groupFlags & _BCM_DPP_FIELD_GROUP_PRESEL)) {
        BCM_FIELD_PRESEL_INIT(pset);
        if (groupData->groupFlags & _BCM_DPP_FIELD_GROUP_L2) {
            /* group wants L2 frames */
            result = _bcm_dpp_field_presel_implied_get(unitData,
                                                       _BCM_DPP_IMPLIED_PRESEL_L2,
                                                       groupData->stage,
                                                       &pset);
        }
        if ((BCM_E_NONE == result) &&
            (groupData->groupFlags & _BCM_DPP_FIELD_GROUP_IPV4)) {
            /* group wants IPv4 frames */
            result = _bcm_dpp_field_presel_implied_get(unitData,
                                                       _BCM_DPP_IMPLIED_PRESEL_IPV4,
                                                       groupData->stage,
                                                       &pset);
        }
        if ((BCM_E_NONE == result) &&
            (groupData->groupFlags & _BCM_DPP_FIELD_GROUP_IPV6)) {
            /* group wants IPv6 frames */
            result = _bcm_dpp_field_presel_implied_get(unitData,
                                                       _BCM_DPP_IMPLIED_PRESEL_IPV6,
                                                       groupData->stage,
                                                       &pset);
        }
        if ((BCM_E_NONE == result) &&
            (groupData->groupFlags & _BCM_DPP_FIELD_GROUP_MPLS)) {
            /* group wants MPLS frames */
            result = _bcm_dpp_field_presel_implied_get(unitData,
                                                       _BCM_DPP_IMPLIED_PRESEL_MPLS,
                                                       groupData->stage,
                                                       &pset);
        }
        /* copy the resulting preselector set to the group's description */
        sal_memcpy(&(groupData->preselSet),
                   &pset,
                   sizeof(groupData->preselSet));
    }
    if (BCM_E_NONE != result) {
        /* something went wrong; tear down any installed preselectors */
        auxRes = _bcm_dpp_field_presel_implied_teardown(unitData,
                                                        groupData);
        BCMDNX_IF_ERR_EXIT(auxRes);
    }
#ifdef BCM_WARM_BOOT_SUPPORT
#if (0)
/* { */
    /*
     * All 'save' and 'sync' of old sw state are dropped.
     */
    _bcm_dpp_field_presel_all_wb_save(unitData, NULL, NULL);
/* } */
#endif
#endif /* def BCM_WARM_BOOT_SUPPORT */
exit:
    BCMDNX_FUNC_RETURN;
}
#endif /* def BCM_PETRA_SUPPORT */

int
_bcm_dpp_field_group_presel_set(bcm_dpp_field_info_OLD_t *unitData,
                                _bcm_dpp_field_grp_idx_t group,
                                bcm_field_presel_set_t *presel_set)
{
    _bcm_dpp_field_group_t newGroupData;
    int result;
    uint32 groupFlags;
     BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;

    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.groupFlags.get(unit, group, &groupFlags));
    if (0 == (groupFlags & _BCM_DPP_FIELD_GROUP_IN_USE)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d group %u is not in use"),
                          unit,
                          group));
    }
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.get(unit, group, &newGroupData));
    /* assign preselector set to this group (replacing if needed) */
    if ((_BCM_DPP_FIELD_GROUP_IN_HW) ==
        (groupFlags &
         (_BCM_DPP_FIELD_GROUP_IN_HW | _BCM_DPP_FIELD_GROUP_PRESEL))) {
        /* group was in hardware but using 'implied' preselector set */
        LOG_WARN(BSL_LS_BCM_FP,
                 (BSL_META_U(unit,
                             "unit %d group %u was using implied presel set"
                             " because its presel set was not configured"
                             " before its action set was configured; please"
                             " configure the presel set for groups before"
                             " configuring their action set.\n"),
                  unit,
                  group));
    }
    newGroupData.groupFlags |= _BCM_DPP_FIELD_GROUP_PRESEL;
    sal_memcpy(&(newGroupData.preselSet),
               presel_set,
               sizeof(newGroupData.preselSet));
    if (groupFlags & _BCM_DPP_FIELD_GROUP_IN_HW) {
        result = _bcm_dpp_field_group_hardware_install(unitData,
                                                       group,
                                                       &newGroupData);
        BCMDNX_IF_ERR_EXIT_MSG(result,
                            (_BSL_BCM_MSG_NO_UNIT("unit %d group %u failed to"
                                              " update to hardware: %d (%s)"),
                             unit,
                             group,
                             result,
                             _SHR_ERRMSG(result)));
    }
    /* presel ref count updated by hardware install/remove */
    /* commit group information */
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.set(unit, group, &newGroupData));
#ifdef BCM_WARM_BOOT_SUPPORT
    /*_bcm_dpp_field_group_wb_save(unitData, group, NULL, NULL);*/
#endif /* def BCM_WARM_BOOT_SUPPORT */

exit:
    BCMDNX_FUNC_RETURN;
}

/******************************************************************************
 *
 *  Functions and data exported to API users
 */

int
bcm_petra_field_presel_create(int unit,
                              bcm_field_presel_t *presel_id)
{
    _DPP_FIELD_COMMON_LOCALS;
    BCMDNX_INIT_FUNC_DEFS;

    _DPP_FIELD_UNIT_CHECK(unit, unitData);
    if (!presel_id) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG("obligatory OUT argument must not be NULL")));
    }
    _DPP_FIELD_UNIT_LOCK(unitData);
    result = _bcm_dpp_field_presel_alloc(unitData, 0 /* flags */, presel_id);
    _DPP_FIELD_UNIT_UNLOCK(unitData);
    BCMDNX_IF_ERR_EXIT(result);
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_field_presel_create_id(int unit,
                                 bcm_field_presel_t presel_id)
{
    _DPP_FIELD_COMMON_LOCALS;
    BCMDNX_INIT_FUNC_DEFS;

    _DPP_FIELD_UNIT_CHECK(unit, unitData);
    _DPP_FIELD_UNIT_LOCK(unitData);
    result = _bcm_dpp_field_presel_alloc(unitData,
                                         _BCM_DPP_PRESEL_ALLOC_WITH_ID,
                                         &presel_id);
    _DPP_FIELD_UNIT_UNLOCK(unitData);
    BCMDNX_IF_ERR_EXIT(result);
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_field_presel_destroy(int unit,
                               bcm_field_presel_t presel_id)
{
    _DPP_FIELD_COMMON_LOCALS;
    BCMDNX_INIT_FUNC_DEFS;

    _DPP_FIELD_UNIT_CHECK(unit, unitData);
    _DPP_FIELD_UNIT_LOCK(unitData);
    result = _bcm_dpp_field_presel_free(unitData, presel_id);
    _DPP_FIELD_UNIT_UNLOCK(unitData);
    BCMDNX_IF_ERR_EXIT(result);
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_field_group_presel_set(int unit,
                                 bcm_field_group_t group,
                                 bcm_field_presel_set_t *presel_set)
{
    _DPP_FIELD_COMMON_LOCALS;
   _bcm_dpp_field_grp_idx_t groupLimit;

    BCMDNX_INIT_FUNC_DEFS;
   FIELD_ACCESS.groupLimit.get(unit, &groupLimit);

    _DPP_FIELD_UNIT_CHECK(unit, unitData);
    if ((0 > group) || (groupLimit <= group)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                         (_BSL_BCM_MSG("group %d is not valid"), group));
    }
    if (!presel_set) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG("obligatory IN pointer must not be NULL")));
    }
    _DPP_FIELD_UNIT_LOCK(unitData);

    result = _bcm_dpp_field_group_presel_set(unitData, group, presel_set);

    _DPP_FIELD_UNIT_UNLOCK(unitData);
    BCMDNX_IF_ERR_EXIT(result);
exit:
#if (defined(BCM_DPP_SUPPORT) && defined(BCM_WARM_BOOT_SUPPORT))
    _DCMN_BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE(unit);
#endif /* (defined(BCM_DPP_SUPPORT) && defined(BCM_WARM_BOOT_SUPPORT)) */
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_field_group_presel_get(int unit,
                                 bcm_field_group_t group,
                                 bcm_field_presel_set_t *presel_set)
{
    _DPP_FIELD_COMMON_LOCALS;
    _bcm_dpp_field_grp_idx_t groupLimit;
    uint32 groupFlags;
    int dpp_field_unit_lock_was_taken = 0;

    BCMDNX_INIT_FUNC_DEFS;
    FIELD_ACCESS.groupLimit.get(unit, &groupLimit);
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.groupFlags.get(unit, group, &groupFlags));

    _DPP_FIELD_UNIT_CHECK(unit, unitData);
    if ((0 > group) || (groupLimit <= group)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                         (_BSL_BCM_MSG("group %d is not valid"), group));
    }
    if (!presel_set) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG("obligatory OUT argument must not be NULL")));
    }

    _DPP_FIELD_UNIT_LOCK(unitData);
    dpp_field_unit_lock_was_taken = 1;
    if (groupFlags & _BCM_DPP_FIELD_GROUP_IN_USE) {
        if (0 == (groupFlags & _BCM_DPP_FIELD_GROUP_PRESEL)) {
            LOG_WARN(BSL_LS_BCM_FP,
                     (BSL_META_U(unit,
                                 "unit %d group %d reading preselector set"
                                 " from a group where it was not set\n"),
                      unit,
                      group));
        }

        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.preselSet.get(unit, group, presel_set));
    } else {
        LOG_ERROR(BSL_LS_BCM_FP,
                  (BSL_META_U(unit,
                              "unit %d group %d is not in use\n"),
                   unit,
                   group));
        result = BCM_E_NOT_FOUND;
    }

    BCMDNX_IF_ERR_EXIT(result);
exit:
    if (dpp_field_unit_lock_was_taken) {
        _DPP_FIELD_UNIT_UNLOCK(unitData);
    }
#if (defined(BCM_DPP_SUPPORT) && defined(BCM_WARM_BOOT_SUPPORT))
    _DCMN_BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE(unit);
#endif /* (defined(BCM_DPP_SUPPORT) && defined(BCM_WARM_BOOT_SUPPORT)) */
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_field_presel_install(bcm_dpp_field_info_OLD_t *unitData,
                               _bcm_dpp_field_presel_idx_t presel)
{
    SOC_PPD_FP_PFG_INFO *pfgInfo = NULL;
    uint32 sandResult;
    uint32 success;
    _bcm_dpp_field_stage_idx_t stage;
    int unit=0;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_ALLOC(pfgInfo, sizeof(SOC_PPD_FP_PFG_INFO), "_bcm_dpp_field_presel_reset.pfgInfo");
    if (pfgInfo == NULL) {        
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
    }

    SOC_PPD_FP_PFG_INFO_clear(pfgInfo);

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_presel_info_get(unitData,
                                                      presel,
                                                      0,
                                                      &stage,
                                                      NULL));

    pfgInfo->stage = stage;
    pfgInfo->is_for_hw_commit = TRUE;
    pfgInfo->is_array_qualifier = TRUE;

    sandResult = soc_ppd_fp_packet_format_group_set(unitData->unitHandle,
                                                    presel,
                                                    pfgInfo,
                                                    &success);
    BCMDNX_IF_ERR_EXIT_MSG(handle_sand_result(sandResult),
                        (_BSL_BCM_MSG_NO_UNIT("unable to write unit %d"
                                          " presel %u)"),
                         unitData->unit,
                         presel));
    BCMDNX_IF_ERR_EXIT_MSG(translate_sand_success_failure(success),
                        (_BSL_BCM_MSG_NO_UNIT("write unit %d presel %u"
                                          " unsucessful"),
                         unitData->unit,
                         presel));
exit:
    BCM_FREE(pfgInfo);
    BCMDNX_FUNC_RETURN;

}

int
_bcm_dpp_field_presel_destroy(bcm_dpp_field_info_OLD_t *unitData,
                               _bcm_dpp_field_presel_idx_t presel)
{
    SOC_PPD_FP_PFG_INFO *pfgInfo = NULL;
    uint32 sandResult;
    uint32 success;
    _bcm_dpp_field_stage_idx_t stage;
    int unit=0;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_ALLOC(pfgInfo, sizeof(SOC_PPD_FP_PFG_INFO), "_bcm_dpp_field_presel_reset.pfgInfo");
    if (pfgInfo == NULL) {        
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
    }

    SOC_PPD_FP_PFG_INFO_clear(pfgInfo);

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_presel_info_get(unitData,
                                                      presel,
                                                      0,
                                                      &stage,
                                                      NULL));

    pfgInfo->stage = stage;
    pfgInfo->is_for_hw_commit = TRUE;
    pfgInfo->is_array_qualifier = FALSE;

    sandResult = soc_ppd_fp_packet_format_group_set(unitData->unitHandle,
                                                    presel,
                                                    pfgInfo,
                                                    &success);
    BCMDNX_IF_ERR_EXIT_MSG(handle_sand_result(sandResult),
                        (_BSL_BCM_MSG_NO_UNIT("unable to write unit %d"
                                          " presel %u)"),
                         unitData->unit,
                         presel));
    BCMDNX_IF_ERR_EXIT_MSG(translate_sand_success_failure(success),
                        (_BSL_BCM_MSG_NO_UNIT("write unit %d presel %u"
                                          " unsucessful"),
                         unitData->unit,
                         presel));
exit:
    BCM_FREE(pfgInfo);
    BCMDNX_FUNC_RETURN;

}


