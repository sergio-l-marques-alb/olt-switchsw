/*
 * $Id: field_utils.c,v 1.98 Broadcom SDK $
 * $Copyright: Copyright 2015 Broadcom Corporation.
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
 *      Utilities functions for other field modules
 */

/*
 *  The functions in this module deal with programmable data qualifiers on PPD
 *  devices.  They are invoked by the main field module when certain things
 *  occur, such as calls that explicitly involve data qualifiers.
 */

/*
 *  Everything in this file assumes it is safely called behind a lock.
 */

/*
 *  Exported functions have their descriptions in the field_int.h file.
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_FP
#include <shared/bsl.h>
#include <shared/swstate/access/sw_state_access.h>
#include <bcm_int/common/debug.h>

#include <bcm/types.h>
#include <bcm/debug.h>
#include <bcm/error.h>
#include <bcm/field.h>
#include <bcm/stg.h>

#include <soc/drv.h>
#include <bcm_int/dpp/alloc_mngr.h>
#include <bcm_int/dpp/field_int.h>
#include <bcm_int/dpp/error.h>
#include <bcm_int/dpp/utils.h>
#ifdef BCM_WARM_BOOT_SUPPORT
#include <bcm_int/dpp/wb_db_field.h>
#endif /* def BCM_WARM_BOOT_SUPPORT */
#include <soc/dpp/PPD/ppd_api_fp.h>
#include <soc/dpp/PPD/ppd_api_port.h>
#ifdef BCM_PETRAB_SUPPORT
#include <soc/dpp/Petra/PB_PP/pb_pp_api_trap_mgmt.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_trap_mgmt.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_fp_key.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_fp_fem.h>
#endif /* def BCM_PETRAB_SUPPORT */
#ifdef BCM_ARAD_SUPPORT
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_trap_mgmt.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_fp.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_fp_key.h>
#include <soc/dpp/ARAD/arad_pmf_low_level_fem_tag.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_flp_init.h>
#endif /* def BCM_ARAD_SUPPORT */
#include <shared/bitop.h>

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
 *  Functions and data shared with other field modules
 */


CONST char* CONST _bcm_dpp_field_qual_name[bcmFieldQualifyCount] = BCM_FIELD_QUALIFY_STRINGS;
CONST char* CONST _bcm_dpp_field_action_name[bcmFieldActionCount] = BCM_FIELD_ACTION_STRINGS;
CONST char* CONST _bcm_dpp_field_group_mode_name[bcmFieldGroupModeCount] = BCM_FIELD_GROUP_MODE_STRINGS;
CONST char* CONST _bcm_dpp_field_stats_names[bcmFieldStatCount] = BCM_FIELD_STAT;
CONST char* CONST _bcm_dpp_field_control_names[bcmFieldControlCount] = BCM_FIELD_CONTROL_STRINGS;
CONST char* CONST _bcm_dpp_field_data_offset_base_names[bcmFieldDataOffsetBaseCount] = BCM_FIELD_DATA_OFFSET_BASE;
CONST char* CONST _bcm_dpp_field_header_format_names[bcmFieldHeaderFormatCount] = BCM_FIELD_HEADER_FORMAT;
CONST char* CONST _bcm_dpp_field_entry_type_names[_bcmDppFieldEntryTypeCount] = _BCM_DPP_FIELD_ENTRY_TYPE_STRINGS;
CONST char* CONST _bcm_dpp_field_stage_name[bcmFieldStageCount] = BCM_FIELD_STAGE_STRINGS;

int
_bcm_dpp_ppd_qual_bits(_bcm_dpp_field_info_t *unitData,
                       _bcm_dpp_field_stage_idx_t stage,
                       SOC_PPD_FP_QUAL_TYPE ppdQual,
                       unsigned int *exposed,
                       unsigned int *hardwareBestCase,
                       unsigned int *hardwareWorstCase)
{
    uint32 ppdExposed;
    uint32 ppdHardwareBestCase;
    uint32 ppdHardwareWorstCase;
    uint32 ppdr;
    uint32 found;
    int result;
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;

#ifdef BCM_PETRAB_SUPPORT
    if (SOC_IS_PETRAB(unit)){
        ppdr = soc_pb_pp_fp_qual_length_get(unitData->unitHandle,
                                            unitData->stageD[stage].devInfo->hwStageId,
                                            ppdQual,
                                            &found,
                                            &ppdHardwareBestCase,
                                            &ppdExposed);
        /* No difference in Petra-B */
        ppdHardwareWorstCase = ppdHardwareBestCase;
    } else
#endif /* def BCM_PETRAB_SUPPORT */
#ifdef BCM_ARAD_SUPPORT
    if (SOC_IS_ARAD(unit)){
        ppdr = arad_pp_fp_qual_length_get(unitData->unitHandle,
                                          unitData->stageD[stage].devInfo->hwStageId,
                                          ppdQual,
                                          &found,
                                          &ppdHardwareBestCase,
                                          &ppdHardwareWorstCase,
                                          &ppdExposed);
    } else
#endif /* def BCM_ARAD_SUPPORT */
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d hardware is not supported"),
                          unit));
    }
    result = handle_sand_result(ppdr);
    BCMDNX_IF_ERR_EXIT_MSG(result,
                        (_BSL_BCM_MSG_NO_UNIT("unit %d unable to resolve PPD stage"
                                          " %u qualifier type %d (%s) length:"
                                          " %d (%s)"),
                         unit,
                         stage,
                         ppdQual,
                         SOC_PPD_FP_QUAL_TYPE_to_string(ppdQual),
                         result,
                         _SHR_ERRMSG(result)));
    if (!found) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d unable to find stage %u"
                                           " PPD qualifier %d (%s)"),
                          unit,
                          stage,
                          ppdQual,
                          SOC_PPD_FP_QUAL_TYPE_to_string(ppdQual)));
    }
    if (exposed) {
        *exposed = ppdExposed;
    }
    if (hardwareBestCase) {
        *hardwareBestCase = ppdHardwareBestCase;
    }
    if (hardwareWorstCase) {
        *hardwareWorstCase = ppdHardwareWorstCase;
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_ppd_action_bits(_bcm_dpp_field_info_t *unitData,
                         _bcm_dpp_field_stage_idx_t stage,
                         SOC_PPD_FP_ACTION_TYPE ppdAct,
                         unsigned int *bits)
{
    uint32 ppdBits;
#ifdef BCM_PETRAB_SUPPORT
    uint32 ppdBitsFem;
#endif /* def BCM_PETRAB_SUPPORT */
#ifdef BCM_ARAD_SUPPORT
    uint32 ppdLsbEgress;
#endif /* def BCM_ARAD_SUPPORT */
    uint32 ppdr;
    int result;
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;

#ifdef BCM_PETRAB_SUPPORT
    if (SOC_IS_PETRAB(unit)){
        ppdr = soc_pb_pp_fp_action_type_max_size_get(unitData->unitHandle,
                                                     ppdAct,
                                                     &ppdBits,
                                                     &ppdBitsFem);
    } else
#endif /* def BCM_PETRAB_SUPPORT */
#ifdef BCM_ARAD_SUPPORT
    if (SOC_IS_ARAD(unit)){
        ppdr = arad_pmf_db_fes_action_size_get_unsafe(unitData->unitHandle,
                                                      ppdAct,
                                                      unitData->stageD[stage].devInfo->hwStageId,
                                                      &ppdBits,
                                                      &ppdLsbEgress);
    } else
#endif /* def BCM_ARAD_SUPPORT */
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d hardware is not supported"),
                          unit));
    }
    result = handle_sand_result(ppdr);
    BCMDNX_IF_ERR_EXIT_MSG(result,
                        (_BSL_BCM_MSG_NO_UNIT("unit %d unable to resolve PPD"
                                          " action type %d (%s) length:"
                                          " %d (%s)"),
                         unit,
                         ppdAct,
                         SOC_PPD_FP_ACTION_TYPE_to_string(ppdAct),
                         result,
                         _SHR_ERRMSG(result)));
    if (bits) {
        *bits = ppdBits;
    }
exit:
    BCMDNX_FUNC_RETURN;
}

#ifdef BROADCOM_DEBUG
/*
 *  Function
 *    _bcm_dpp_field_qset_dump
 *  Purpose
 *    Dump qualifier list from a qset
 *  Parameters
 *    (in) bcm_field_qset_t qset = the qset to dump
 *    (in) char *prefix = line prefix
 *  Returns
 *    nothing
 *  Notes
 *    No error checking or locking is done here.
 */
void
_bcm_dpp_field_qset_dump(const bcm_field_qset_t qset,
                         const char *prefix)
{
    bcm_field_qualify_t qualifier;
    unsigned int column = 0;
    unsigned int size;
    unsigned int temp;
	int unit = BSL_UNIT_UNKNOWN;

    /* for each qualifier potentially in the qset  */
    for (qualifier = 0; qualifier < bcmFieldQualifyCount; qualifier++) {
        /* if that qualifier actually is in the qset */
        if (BCM_FIELD_QSET_TEST(qset, qualifier)) {
            /* display the qualifier */
            if (0 == column) {
                /* just starting out */
                LOG_CLI((BSL_META_U(unit,
                                    "%s%s"),
                         prefix,
                         _bcm_dpp_field_qual_name[qualifier]));
                column = (sal_strlen(prefix) +
                          sal_strlen(_bcm_dpp_field_qual_name[qualifier]));
            } else if ((3 + column +
                       sal_strlen(_bcm_dpp_field_qual_name[qualifier])) >=
                       _BCM_DPP_FIELD_PAGE_WIDTH) {
                /* this would wrap */
                LOG_CLI((BSL_META_U(unit,
                                    ",\n%s%s"),
                         prefix,
                         _bcm_dpp_field_qual_name[qualifier]));
                column = (sal_strlen(prefix) +
                          sal_strlen(_bcm_dpp_field_qual_name[qualifier]));
            } else {
                /* this fits on the line */
                LOG_CLI((BSL_META_U(unit,
                                    ", %s"),
                         _bcm_dpp_field_qual_name[qualifier]));
                column += (2 +
                           sal_strlen(_bcm_dpp_field_qual_name[qualifier]));
            }
        } /* if (BCM_FIELD_QSET_TEST(thisGroup->qset, qualifier)) */
    } /* for (qualifier = 0; qualifier < bcmFieldQualifyCount; qualifier++) */
    for (qualifier = 0; qualifier < BCM_FIELD_USER_NUM_UDFS; qualifier++) {
        /* if that qualifier actually is in the qset */
        if (SHR_BITGET(qset.udf_map, qualifier)) {
            /* display the qualifier */
            for (temp = qualifier, size = 1;
                 temp >= 10;
                 temp /= 10, size++) {
                /* just size the number */
            }
            if (0 == column) {
                /* just starting out */
                LOG_CLI((BSL_META_U(unit,
                                    "%sDataField(%d)"), prefix, qualifier));
                column = sal_strlen(prefix) + 11 + size;
            } else if ((3 + column + 11 + size) >=
                       _BCM_DPP_FIELD_PAGE_WIDTH) {
                /* this would wrap */
                LOG_CLI((BSL_META_U(unit,
                                    ",\n%sDataField(%d)"), prefix, qualifier));
                column = sal_strlen(prefix) + 11 + size;
            } else {
                /* this fits on the line */
                LOG_CLI((BSL_META_U(unit,
                                    ", DataField(%d)"), qualifier));
                column += (13 + size);
            }
        } /* if (SHR_BITGET(qset.udf_map, qualifier)) */
    } /* for (all suported programmable qualifiers) */
    if (0 < column) {
        LOG_CLI((BSL_META_U(unit,
                            "\n")));
    } else {
        LOG_CLI((BSL_META_U(unit,
                            "%s(none)\n"), prefix));
    }
}
#endif /* def BROADCOM_DEBUG */

#ifdef BROADCOM_DEBUG
/*
 *  Function
 *    _bcm_dpp_field_aset_dump
 *  Purpose
 *    Dump action list from an aset
 *  Parameters
 *    (in) bcm_field_aset_t aset = the aset to dump
 *    (in) char *prefix = line prefix
 *  Returns
 *    nothing
 *  Notes
 *    No error checking or locking is done here.
 */
void
_bcm_dpp_field_aset_dump(const bcm_field_aset_t aset,
                         const char *prefix)
{
    bcm_field_action_t action;
    unsigned int column = 0;
    int unit;

    unit = BSL_UNIT_UNKNOWN;

    /* for each action potentially in the aset  */
    for (action = 0; action < bcmFieldActionCount; action++) {
        /* if that action actually is in the aset */
        if (BCM_FIELD_ASET_TEST(aset, action)) {
            /* display the qualifier */
            if (0 == column) {
                /* just starting out */
                LOG_CLI((BSL_META_U(unit,
                                    "%s%s"),
                         prefix,
                         _bcm_dpp_field_action_name[action]));
                column = (sal_strlen(prefix) +
                          sal_strlen(_bcm_dpp_field_action_name[action]));
            } else if ((3 + column +
                       sal_strlen(_bcm_dpp_field_action_name[action])) >=
                       _BCM_DPP_FIELD_PAGE_WIDTH) {
                /* this would wrap */
                LOG_CLI((BSL_META_U(unit,
                                    ",\n%s%s"),
                         prefix,
                         _bcm_dpp_field_action_name[action]));
                column = (sal_strlen(prefix) +
                          sal_strlen(_bcm_dpp_field_action_name[action]));
            } else {
                /* this fits on the line */
                LOG_CLI((BSL_META_U(unit,
                                    ", %s"),
                         _bcm_dpp_field_action_name[action]));
                column += (2 +
                           sal_strlen(_bcm_dpp_field_action_name[action]));
            }
        } /* if (BCM_FIELD_ASET_TEST(thisGroup->aset, action)) */
    } /* for (action = 0; action < bcmFieldActionCount; action++) */
    if (0 < column) {
        LOG_CLI((BSL_META_U(unit,
                            "\n")));
    } else {
        LOG_CLI((BSL_META_U(unit,
                            "%s(none)\n"), prefix));
    }
}
#endif /* def BROADCOM_DEBUG */

#ifdef BROADCOM_DEBUG
void
_bcm_dpp_field_hfset_dump(const bcm_field_header_format_set_t hfset,
                          const char *prefix)
{
    bcm_field_header_format_t hf;
    unsigned int column = 0;
    int unit;

    unit = BSL_UNIT_UNKNOWN;

    /* for each qualifier potentially in the qset  */
    for (hf = 0; hf < bcmFieldHeaderFormatCount; hf++) {
        /* if that qualifier actually is in the qset */
        if (BCM_FIELD_HEADER_FORMAT_SET_TEST(hfset, hf)) {
            /* display the qualifier */
            if (0 == column) {
                /* just starting out */
                LOG_CLI((BSL_META_U(unit,
                                    "%s%s"),
                         prefix,
                         _bcm_dpp_field_header_format_names[hf]));
                column = (sal_strlen(prefix) +
                          sal_strlen(_bcm_dpp_field_header_format_names[hf]));
            } else if ((3 + column +
                       sal_strlen(_bcm_dpp_field_header_format_names[hf])) >=
                       _BCM_DPP_FIELD_PAGE_WIDTH) {
                /* this would wrap */
                LOG_CLI((BSL_META_U(unit,
                                    ",\n%s%s"),
                         prefix,
                         _bcm_dpp_field_header_format_names[hf]));
                column = (sal_strlen(prefix) +
                          sal_strlen(_bcm_dpp_field_header_format_names[hf]));
            } else {
                /* this fits on the line */
                LOG_CLI((BSL_META_U(unit,
                                    ", %s"),
                         _bcm_dpp_field_header_format_names[hf]));
                column += (2 +
                           sal_strlen(_bcm_dpp_field_header_format_names[hf]));
            }
        } /* if (BCM_FIELD_HEADER_FORMAT_SET_TEST(thisGroup->aset, hf)) */
    } /* for (hf = 0; hf < bcmFieldHeaderFormatCount; hf++) */
    if (0 < column) {
        LOG_CLI((BSL_META_U(unit,
                            "\n")));
    } else {
        LOG_CLI((BSL_META_U(unit,
                            "%s(none)\n"), prefix));
    }
}
#endif /* def BROADCOM_DEBUG */

/* Conversion from PPD Trap id to HW trap id */
int
_bcm_dpp_field_trap_ppd_to_hw(int unit,
                              int trap_id,
                              uint32 *hwTrapCode)
{
    uint32 ppdr;
    int result;
    SOC_PPD_TRAP_CODE ppdTrapCode;

    BCMDNX_INIT_FUNC_DEFS ;

    *hwTrapCode = 0;
#ifdef BCM_PETRAB_SUPPORT
    if (SOC_IS_PETRAB(unit)) {
        SOC_PB_PP_TRAP_CODE_INTERNAL intTrapCode_pb;
        SOC_TMC_REG_FIELD *dc0, *dc1; /* scratch space */
        ppdTrapCode = trap_id;
        ppdr = soc_pb_pp_trap_mgmt_trap_code_to_internal(ppdTrapCode,
                                                     &intTrapCode_pb,
                                                     &dc0,
                                                     &dc1);
        *hwTrapCode = intTrapCode_pb;
    } else
#endif /* def BCM_PETRAB_SUPPORT */
#ifdef BCM_ARAD_SUPPORT
    if (SOC_IS_ARAD(unit)) {
        ARAD_PP_TRAP_CODE_INTERNAL intTrapCode_arad;
        ARAD_SOC_REG_FIELD dc0_arad, dc1_arad; /* scratch space */


        if (!_BCM_RX_EXPOSE_HW_ID(unit)) 
        {

                BCMDNX_IF_ERR_EXIT_MSG(_bcm_rx_ppd_trap_code_from_trap_id(unit, trap_id, &ppdTrapCode),
                         (_BSL_BCM_MSG_NO_UNIT("Unable to resolve PPD"
                                          " Trap id %d)"), trap_id));

                ppdr = arad_pp_trap_mgmt_trap_code_to_internal(unit,
                                                               ppdTrapCode,
                                                               &intTrapCode_arad,
                                                               &dc0_arad,
                                                               &dc1_arad);
        }
        else /* no need of converting to ppd first*/
        {
            ppdr = arad_pp_trap_mgmt_trap_code_to_internal(unit,
                                                         trap_id,
                                                         &intTrapCode_arad,
                                                         &dc0_arad,
                                                         &dc1_arad);
        }


        *hwTrapCode = intTrapCode_arad;
    } else
#endif /* BCM_ARAD_SUPPORT */
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d hardware is not supported"),
                          unit));
    }
    result = handle_sand_result(ppdr);
    BCMDNX_IF_ERR_EXIT_MSG(result,
                        (_BSL_BCM_MSG_NO_UNIT("unit %d unable to resolve PPD"
                                          " Trap code %d (%s):"
                                          " %d (%s)"),
                         unit,
                         ppdTrapCode,
                         SOC_PPD_TRAP_CODE_to_string(ppdTrapCode),
                         result,
                         _SHR_ERRMSG(result)));
exit:
    BCMDNX_FUNC_RETURN;
}

/* Conversion from PPD Trap id from HW trap id */
int
_bcm_dpp_field_trap_ppd_from_hw(int unit,
                                uint32 hwTrapCode,
                                SOC_PPD_TRAP_CODE *ppdTrapCode)
{
    uint32 ppdr;
    int result;

    BCMDNX_INIT_FUNC_DEFS ;

    *ppdTrapCode = SOC_PPD_NOF_TRAP_CODES;
#ifdef BCM_PETRAB_SUPPORT
    if (SOC_IS_PETRAB(unit)) {
        ppdr = soc_pb_pp_trap_cpu_trap_code_from_internal_unsafe(
                unit,
                hwTrapCode,
                ppdTrapCode);
    } else
#endif /* def BCM_PETRAB_SUPPORT */
#ifdef BCM_ARAD_SUPPORT
    if (SOC_IS_ARAD(unit)) {
        ppdr = arad_pp_trap_cpu_trap_code_from_internal_unsafe(
                unit,
                hwTrapCode,
                ppdTrapCode);
    } else
#endif /* BCM_ARAD_SUPPORT */
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d hardware is not supported"),
                          unit));
    }
    result = handle_sand_result(ppdr);
    BCMDNX_IF_ERR_EXIT_MSG(result,
                        (_BSL_BCM_MSG_NO_UNIT("unit %d unable to resolve PPD"
                                          " Trap code from HW %d:"
                                          " %d (%s)"),
                         unit,
                         hwTrapCode,
                         result,
                         _SHR_ERRMSG(result)));
exit:
    BCMDNX_FUNC_RETURN;
}


int
_bcm_dpp_field_HeaderFormat_bcm_to_ppd(bcm_field_header_format_t header_format,
                                       SOC_TMC_PARSER_PKT_HDR_STK_TYPE *hdr_stk_type,
                                       uint32 *ppdMask)
{
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    *ppdMask = 0x3F;
    switch (header_format) {
    case bcmFieldHeaderFormatL2:
        *hdr_stk_type = SOC_TMC_PARSER_PKT_HDR_STK_TYPE_ETH;
        break;
    case bcmFieldHeaderFormatIp4:
    case bcmFieldHeaderFormatIp4AnyL2L3:
        *hdr_stk_type = SOC_TMC_PARSER_PKT_HDR_STK_TYPE_IPV4_ETH;
        if (header_format == bcmFieldHeaderFormatIp4AnyL2L3) {
            *ppdMask = 0x7; /* Match on all the IPv4 packets */
        }
        break;
    case bcmFieldHeaderFormatIp6:
    case bcmFieldHeaderFormatIp6AnyL2L3:
        *hdr_stk_type = SOC_TMC_PARSER_PKT_HDR_STK_TYPE_IPV6_ETH;
        if (header_format == bcmFieldHeaderFormatIp6AnyL2L3) {
            *ppdMask = 0x7; /* Match on all the IPv6 packets */
        }
        break;
    case bcmFieldHeaderFormatMplsLabel1:
    case bcmFieldHeaderFormatMplsLabel1AnyL2L3:
        *hdr_stk_type = SOC_TMC_PARSER_PKT_HDR_STK_TYPE_MPLS1_ETH;
        if (header_format == bcmFieldHeaderFormatMplsLabel1AnyL2L3) {
            *ppdMask = 0x7; /* Match on all the MPLS1 packets */
        }
        break;
    case bcmFieldHeaderFormatMplsLabel2:
    case bcmFieldHeaderFormatMplsLabel2AnyL2L3:
        *hdr_stk_type = SOC_TMC_PARSER_PKT_HDR_STK_TYPE_MPLS2_ETH;
        if (header_format == bcmFieldHeaderFormatMplsLabel2AnyL2L3) {
            *ppdMask = 0x7; /* Match on all the MPLS2 packets */
        }
        break;
    case bcmFieldHeaderFormatMplsLabel3:
    case bcmFieldHeaderFormatMplsLabel3AnyL2L3:
        *hdr_stk_type = SOC_TMC_PARSER_PKT_HDR_STK_TYPE_MPLS3_ETH;
        if (header_format == bcmFieldHeaderFormatMplsLabel3AnyL2L3) {
            *ppdMask = 0x7; /* Match on all the MPLS2 packets */
        }
        break;
    case bcmFieldHeaderFormatEthEth:
        *hdr_stk_type = SOC_TMC_PARSER_PKT_HDR_STK_TYPE_ETH_ETH;
        break;
    case bcmFieldHeaderFormatTrill:
    case bcmFieldHeaderFormatEthTrillEth:
        *hdr_stk_type = SOC_TMC_PARSER_PKT_HDR_STK_TYPE_ETH_TRILL_ETH;
        break;
    case bcmFieldHeaderFormatIp4Ip4:
        *hdr_stk_type = SOC_TMC_PARSER_PKT_HDR_STK_TYPE_IPV4_IPV4_ETH;
        break;
    case bcmFieldHeaderFormatIp6Ip4:
        *hdr_stk_type = SOC_TMC_PARSER_PKT_HDR_STK_TYPE_IPV6_IPV4_ETH;
        break;
    case bcmFieldHeaderFormatIp4MplsLabel1:
        *hdr_stk_type = SOC_TMC_PARSER_PKT_HDR_STK_TYPE_IPV4_MPLS1_ETH;
        break;
    case bcmFieldHeaderFormatIp4MplsLabel2:
        *hdr_stk_type = SOC_TMC_PARSER_PKT_HDR_STK_TYPE_IPV4_MPLS2_ETH;
        break;
    case bcmFieldHeaderFormatIp4MplsLabel3:
        *hdr_stk_type = SOC_TMC_PARSER_PKT_HDR_STK_TYPE_IPV4_MPLS3_ETH;
        break;
    case bcmFieldHeaderFormatIp6MplsLabel1:
        *hdr_stk_type = SOC_TMC_PARSER_PKT_HDR_STK_TYPE_IPV6_MPLS1_ETH;
        break;
    case bcmFieldHeaderFormatIp6MplsLabel2:
        *hdr_stk_type = SOC_TMC_PARSER_PKT_HDR_STK_TYPE_IPV6_MPLS2_ETH;
        break;
    case bcmFieldHeaderFormatIp6MplsLabel3:
        *hdr_stk_type = SOC_TMC_PARSER_PKT_HDR_STK_TYPE_IPV6_MPLS3_ETH;
        break;
    case bcmFieldHeaderFormatEthMplsLabel1:
        *hdr_stk_type = SOC_TMC_PARSER_PKT_HDR_STK_TYPE_ETH_MPLS1_ETH;
        break;
    case bcmFieldHeaderFormatEthMplsLabel2:
        *hdr_stk_type = SOC_TMC_PARSER_PKT_HDR_STK_TYPE_ETH_MPLS2_ETH;
        break;
    case bcmFieldHeaderFormatEthMplsLabel3:
        *hdr_stk_type = SOC_TMC_PARSER_PKT_HDR_STK_TYPE_ETH_MPLS3_ETH;
        break;
    case bcmFieldHeaderFormatEthIp4Eth:
        *hdr_stk_type = SOC_TMC_PARSER_PKT_HDR_STK_TYPE_ETH_IPV4_ETH;
        break;
    case bcmFieldHeaderFormatFCoE:
        *hdr_stk_type = SOC_TMC_PARSER_PKT_HDR_STK_TYPE_FC_ENCAP_ETH;
        break;
    default:
        if (bcmFieldHeaderFormatCount > header_format) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                             (_BSL_BCM_MSG_NO_UNIT("header format %d not supported"),
                              header_format));
        } else {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                             (_BSL_BCM_MSG_NO_UNIT("header format %d not valid"),
                              header_format));
        }
    }
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_field_HeaderFormat_ppd_to_bcm(SOC_TMC_PARSER_PKT_HDR_STK_TYPE hdr_stk_type,
                                       uint32 ppdMask,
                                       bcm_field_header_format_t *header_format)
{
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;
    if (ppdMask == 0x7) {
        switch (hdr_stk_type) {
        case SOC_TMC_PARSER_PKT_HDR_STK_TYPE_IPV4_ETH:
            *header_format = bcmFieldHeaderFormatIp4AnyL2L3;
            break;
        case SOC_TMC_PARSER_PKT_HDR_STK_TYPE_IPV6_ETH:
            *header_format = bcmFieldHeaderFormatIp6AnyL2L3;
            break;
        case SOC_TMC_PARSER_PKT_HDR_STK_TYPE_MPLS1_ETH:
            *header_format = bcmFieldHeaderFormatMplsLabel1AnyL2L3;
            break;
        case SOC_TMC_PARSER_PKT_HDR_STK_TYPE_MPLS2_ETH:
            *header_format = bcmFieldHeaderFormatMplsLabel2AnyL2L3;
            break;
        case SOC_TMC_PARSER_PKT_HDR_STK_TYPE_MPLS3_ETH:
            *header_format = bcmFieldHeaderFormatMplsLabel3AnyL2L3;
            break;
        default:
            /*FIX: add bcm type */
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                             (_BSL_BCM_MSG_NO_UNIT("unexpected PPD header format %d"),
                              hdr_stk_type));
        }
    }
    else {
        switch (hdr_stk_type) {
        case SOC_TMC_PARSER_PKT_HDR_STK_TYPE_ETH:
            *header_format = bcmFieldHeaderFormatL2;
            break;
        case SOC_TMC_PARSER_PKT_HDR_STK_TYPE_IPV4_ETH:
            *header_format = bcmFieldHeaderFormatIp4;
            break;
        case SOC_TMC_PARSER_PKT_HDR_STK_TYPE_IPV6_ETH:
            *header_format = bcmFieldHeaderFormatIp6;
            break;
        case SOC_TMC_PARSER_PKT_HDR_STK_TYPE_MPLS1_ETH:
            *header_format = bcmFieldHeaderFormatMplsLabel1;
            break;
        case SOC_TMC_PARSER_PKT_HDR_STK_TYPE_MPLS2_ETH:
            *header_format = bcmFieldHeaderFormatMplsLabel2;
            break;
        case SOC_TMC_PARSER_PKT_HDR_STK_TYPE_MPLS3_ETH:
            *header_format = bcmFieldHeaderFormatMplsLabel3;
            break;
        case SOC_TMC_PARSER_PKT_HDR_STK_TYPE_ETH_ETH:
            *header_format = bcmFieldHeaderFormatEthEth;
            break;
        case SOC_TMC_PARSER_PKT_HDR_STK_TYPE_ETH_TRILL_ETH:
            *header_format = bcmFieldHeaderFormatEthTrillEth;
            break;
        case SOC_TMC_PARSER_PKT_HDR_STK_TYPE_IPV4_IPV4_ETH:
            *header_format = bcmFieldHeaderFormatIp4Ip4;
            break;
        case SOC_TMC_PARSER_PKT_HDR_STK_TYPE_IPV6_IPV4_ETH:
            *header_format = bcmFieldHeaderFormatIp6Ip4;
            break;
        case SOC_TMC_PARSER_PKT_HDR_STK_TYPE_IPV4_MPLS1_ETH:
            *header_format = bcmFieldHeaderFormatIp4MplsLabel1;
            break;
        case SOC_TMC_PARSER_PKT_HDR_STK_TYPE_IPV4_MPLS2_ETH:
            *header_format = bcmFieldHeaderFormatIp4MplsLabel2;
            break;
        case SOC_TMC_PARSER_PKT_HDR_STK_TYPE_IPV4_MPLS3_ETH:
            *header_format = bcmFieldHeaderFormatIp4MplsLabel3;
            break;
        case SOC_TMC_PARSER_PKT_HDR_STK_TYPE_IPV6_MPLS1_ETH:
            *header_format = bcmFieldHeaderFormatIp6MplsLabel1;
            break;
        case SOC_TMC_PARSER_PKT_HDR_STK_TYPE_IPV6_MPLS2_ETH:
            *header_format = bcmFieldHeaderFormatIp6MplsLabel2;
            break;
        case SOC_TMC_PARSER_PKT_HDR_STK_TYPE_IPV6_MPLS3_ETH:
            *header_format = bcmFieldHeaderFormatIp6MplsLabel3;
            break;
        case SOC_TMC_PARSER_PKT_HDR_STK_TYPE_ETH_MPLS1_ETH:
            *header_format = bcmFieldHeaderFormatEthMplsLabel1;
            break;
        case SOC_TMC_PARSER_PKT_HDR_STK_TYPE_ETH_MPLS2_ETH:
            *header_format = bcmFieldHeaderFormatEthMplsLabel2;
            break;
        case SOC_TMC_PARSER_PKT_HDR_STK_TYPE_ETH_MPLS3_ETH:
            *header_format = bcmFieldHeaderFormatEthMplsLabel3;
            break;
        case SOC_TMC_PARSER_PKT_HDR_STK_TYPE_ETH_IPV4_ETH:
            *header_format = bcmFieldHeaderFormatEthIp4Eth;
            break;
        case SOC_TMC_PARSER_PKT_HDR_STK_TYPE_FC_ENCAP_ETH:
            *header_format = bcmFieldHeaderFormatFCoE;
            break;
        default:
        case SOC_TMC_PARSER_PKT_HDR_STK_TYPE_FC_STD_ETH:
            /*FIX: add bcm type */
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                             (_BSL_BCM_MSG_NO_UNIT("unexpected PPD header format %d"),
                              hdr_stk_type));
        }
    }
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_field_HeaderFormatSet_bcm_to_ppd(const bcm_field_header_format_set_t *bcm_hfs,
                                          uint32 *soc_petra_hfs)
{
    bcm_field_header_format_set_t hfs;
    int index;
    uint32 phfs = 0;
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    sal_memcpy(&hfs, bcm_hfs, sizeof(hfs));
    if (BCM_FIELD_HEADER_FORMAT_SET_TEST(hfs, bcmFieldHeaderFormatL2)) {
        phfs |= SOC_TMC_FP_PKT_HDR_TYPE_ETH;
        BCM_FIELD_HEADER_FORMAT_SET_REMOVE(hfs, bcmFieldHeaderFormatL2);
    }
    if (BCM_FIELD_HEADER_FORMAT_SET_TEST(hfs, bcmFieldHeaderFormatEthEth)) {
        phfs |= SOC_TMC_FP_PKT_HDR_TYPE_ETH_ETH;
        BCM_FIELD_HEADER_FORMAT_SET_REMOVE(hfs, bcmFieldHeaderFormatEthEth);
    }
    if (BCM_FIELD_HEADER_FORMAT_SET_TEST(hfs, bcmFieldHeaderFormatIp4)) {
        phfs |= SOC_TMC_FP_PKT_HDR_TYPE_IPV4_ETH;
        BCM_FIELD_HEADER_FORMAT_SET_REMOVE(hfs, bcmFieldHeaderFormatIp4);
    }
    if (BCM_FIELD_HEADER_FORMAT_SET_TEST(hfs, bcmFieldHeaderFormatIp6)) {
        phfs |= SOC_TMC_FP_PKT_HDR_TYPE_IPV6_ETH;
        BCM_FIELD_HEADER_FORMAT_SET_REMOVE(hfs, bcmFieldHeaderFormatIp6);
    }
    if (BCM_FIELD_HEADER_FORMAT_SET_TEST(hfs, bcmFieldHeaderFormatMplsLabel1)) {
        phfs |= SOC_TMC_FP_PKT_HDR_TYPE_MPLS1_ETH;
        BCM_FIELD_HEADER_FORMAT_SET_REMOVE(hfs, bcmFieldHeaderFormatMplsLabel1);
    }
    if (BCM_FIELD_HEADER_FORMAT_SET_TEST(hfs, bcmFieldHeaderFormatMplsLabel2)) {
        phfs |= SOC_TMC_FP_PKT_HDR_TYPE_MPLS2_ETH;
        BCM_FIELD_HEADER_FORMAT_SET_REMOVE(hfs, bcmFieldHeaderFormatMplsLabel2);
    }
    if (BCM_FIELD_HEADER_FORMAT_SET_TEST(hfs, bcmFieldHeaderFormatMplsLabel3)) {
        phfs |= SOC_TMC_FP_PKT_HDR_TYPE_MPLS3_ETH;
        BCM_FIELD_HEADER_FORMAT_SET_REMOVE(hfs, bcmFieldHeaderFormatMplsLabel3);
    }
    if (BCM_FIELD_HEADER_FORMAT_SET_TEST(hfs, bcmFieldHeaderFormatEthMplsLabel1)) {
        phfs |= SOC_TMC_FP_PKT_HDR_TYPE_ETH_MPLS1_ETH;
        BCM_FIELD_HEADER_FORMAT_SET_REMOVE(hfs, bcmFieldHeaderFormatEthMplsLabel1);
    }
    if (BCM_FIELD_HEADER_FORMAT_SET_TEST(hfs, bcmFieldHeaderFormatEthMplsLabel2)) {
        phfs |= SOC_TMC_FP_PKT_HDR_TYPE_ETH_MPLS2_ETH;
        BCM_FIELD_HEADER_FORMAT_SET_REMOVE(hfs, bcmFieldHeaderFormatEthMplsLabel2);
    }
    if (BCM_FIELD_HEADER_FORMAT_SET_TEST(hfs, bcmFieldHeaderFormatEthMplsLabel3)) {
        phfs |= SOC_TMC_FP_PKT_HDR_TYPE_ETH_MPLS3_ETH;
        BCM_FIELD_HEADER_FORMAT_SET_REMOVE(hfs, bcmFieldHeaderFormatEthMplsLabel3);
    }
    if (BCM_FIELD_HEADER_FORMAT_SET_TEST(hfs, bcmFieldHeaderFormatIp4MplsLabel1)) {
        phfs |= SOC_TMC_FP_PKT_HDR_TYPE_IPV4_MPLS1_ETH;
        BCM_FIELD_HEADER_FORMAT_SET_REMOVE(hfs, bcmFieldHeaderFormatIp4MplsLabel1);
    }
    if (BCM_FIELD_HEADER_FORMAT_SET_TEST(hfs, bcmFieldHeaderFormatIp4MplsLabel2)) {
        phfs |= SOC_TMC_FP_PKT_HDR_TYPE_IPV4_MPLS2_ETH;
        BCM_FIELD_HEADER_FORMAT_SET_REMOVE(hfs, bcmFieldHeaderFormatIp4MplsLabel2);
    }
    if (BCM_FIELD_HEADER_FORMAT_SET_TEST(hfs, bcmFieldHeaderFormatIp4MplsLabel3)) {
        phfs |= SOC_TMC_FP_PKT_HDR_TYPE_IPV4_MPLS3_ETH;
        BCM_FIELD_HEADER_FORMAT_SET_REMOVE(hfs, bcmFieldHeaderFormatIp4MplsLabel3);
    }
    if (BCM_FIELD_HEADER_FORMAT_SET_TEST(hfs, bcmFieldHeaderFormatIp6MplsLabel1)) {
        phfs |= SOC_TMC_FP_PKT_HDR_TYPE_IPV6_MPLS1_ETH;
        BCM_FIELD_HEADER_FORMAT_SET_REMOVE(hfs, bcmFieldHeaderFormatIp6MplsLabel1);
    }
    if (BCM_FIELD_HEADER_FORMAT_SET_TEST(hfs, bcmFieldHeaderFormatIp6MplsLabel2)) {
        phfs |= SOC_TMC_FP_PKT_HDR_TYPE_IPV6_MPLS2_ETH;
        BCM_FIELD_HEADER_FORMAT_SET_REMOVE(hfs, bcmFieldHeaderFormatIp6MplsLabel2);
    }
    if (BCM_FIELD_HEADER_FORMAT_SET_TEST(hfs, bcmFieldHeaderFormatIp6MplsLabel3)) {
        phfs |= SOC_TMC_FP_PKT_HDR_TYPE_IPV6_MPLS3_ETH;
        BCM_FIELD_HEADER_FORMAT_SET_REMOVE(hfs, bcmFieldHeaderFormatIp6MplsLabel3);
    }
    if (BCM_FIELD_HEADER_FORMAT_SET_TEST(hfs, bcmFieldHeaderFormatIp4Ip4)) {
        phfs |= SOC_TMC_FP_PKT_HDR_TYPE_IPV4_IPV4_ETH;
        BCM_FIELD_HEADER_FORMAT_SET_REMOVE(hfs, bcmFieldHeaderFormatIp4Ip4);
    }
    if (BCM_FIELD_HEADER_FORMAT_SET_TEST(hfs, bcmFieldHeaderFormatIp6Ip4)) {
        phfs |= SOC_TMC_FP_PKT_HDR_TYPE_IPV6_IPV4_ETH;
        BCM_FIELD_HEADER_FORMAT_SET_REMOVE(hfs, bcmFieldHeaderFormatIp6Ip4);
    }
    if (BCM_FIELD_HEADER_FORMAT_SET_TEST(hfs, bcmFieldHeaderFormatEthTrillEth)) {
        phfs |= SOC_TMC_FP_PKT_HDR_TYPE_ETH_TRILL_ETH;
        BCM_FIELD_HEADER_FORMAT_SET_REMOVE(hfs, bcmFieldHeaderFormatEthTrillEth);
    }
    for (index = 0; index < bcmFieldHeaderFormatCount; index++) {
        if (BCM_FIELD_HEADER_FORMAT_SET_TEST(hfs, index)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                             (_BSL_BCM_MSG_NO_UNIT("unable to map field header"
                                               " format %s (%d) to a PPD"
                                               " header format"),
                              _bcm_dpp_field_header_format_names[index],
                              index));
        }
    }
    *soc_petra_hfs = phfs;
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_field_HeaderFormatSet_ppd_to_bcm(const uint32 soc_petra_hfs,
                                          bcm_field_header_format_set_t *bcm_hfs)
{
    bcm_field_header_format_set_t hfs;
    uint32 phfs = soc_petra_hfs;
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    BCM_FIELD_HEADER_FORMAT_SET_INIT(hfs);
    if (phfs & SOC_TMC_FP_PKT_HDR_TYPE_ETH) {
        BCM_FIELD_HEADER_FORMAT_SET_ADD(hfs, bcmFieldHeaderFormatL2);
        phfs &= (~SOC_TMC_FP_PKT_HDR_TYPE_ETH);
    }
    if (phfs & SOC_TMC_FP_PKT_HDR_TYPE_ETH_ETH) {
        BCM_FIELD_HEADER_FORMAT_SET_ADD(hfs, bcmFieldHeaderFormatEthEth);
        phfs &= (~SOC_TMC_FP_PKT_HDR_TYPE_ETH_ETH);
    }
    if (phfs & SOC_TMC_FP_PKT_HDR_TYPE_IPV4_ETH) {
        BCM_FIELD_HEADER_FORMAT_SET_ADD(hfs, bcmFieldHeaderFormatIp4);
        phfs &= (~SOC_TMC_FP_PKT_HDR_TYPE_IPV4_ETH);
    }
    if (phfs & SOC_TMC_FP_PKT_HDR_TYPE_IPV6_ETH) {
        BCM_FIELD_HEADER_FORMAT_SET_ADD(hfs, bcmFieldHeaderFormatIp6);
        phfs &= (~SOC_TMC_FP_PKT_HDR_TYPE_IPV6_ETH);
    }
    if (phfs & SOC_TMC_FP_PKT_HDR_TYPE_MPLS1_ETH) {
        BCM_FIELD_HEADER_FORMAT_SET_ADD(hfs, bcmFieldHeaderFormatMplsLabel1);
        phfs &= (~SOC_TMC_FP_PKT_HDR_TYPE_MPLS1_ETH);
    }
    if (phfs & SOC_TMC_FP_PKT_HDR_TYPE_MPLS2_ETH) {
        BCM_FIELD_HEADER_FORMAT_SET_ADD(hfs, bcmFieldHeaderFormatMplsLabel2);
        phfs &= (~SOC_TMC_FP_PKT_HDR_TYPE_MPLS2_ETH);
    }
    if (phfs & SOC_TMC_FP_PKT_HDR_TYPE_MPLS3_ETH) {
        BCM_FIELD_HEADER_FORMAT_SET_ADD(hfs, bcmFieldHeaderFormatMplsLabel3);
        phfs &= (~SOC_TMC_FP_PKT_HDR_TYPE_MPLS3_ETH);
    }
    if (phfs & SOC_TMC_FP_PKT_HDR_TYPE_ETH_MPLS1_ETH) {
        BCM_FIELD_HEADER_FORMAT_SET_ADD(hfs, bcmFieldHeaderFormatEthMplsLabel1);
        phfs &= (~SOC_TMC_FP_PKT_HDR_TYPE_ETH_MPLS1_ETH);
    }
    if (phfs & SOC_TMC_FP_PKT_HDR_TYPE_ETH_MPLS2_ETH) {
        BCM_FIELD_HEADER_FORMAT_SET_ADD(hfs, bcmFieldHeaderFormatEthMplsLabel2);
        phfs &= (~SOC_TMC_FP_PKT_HDR_TYPE_ETH_MPLS2_ETH);
    }
    if (phfs & SOC_TMC_FP_PKT_HDR_TYPE_ETH_MPLS3_ETH) {
        BCM_FIELD_HEADER_FORMAT_SET_ADD(hfs, bcmFieldHeaderFormatEthMplsLabel3);
        phfs &= (~SOC_TMC_FP_PKT_HDR_TYPE_ETH_MPLS3_ETH);
    }
    if (phfs & SOC_TMC_FP_PKT_HDR_TYPE_IPV4_MPLS1_ETH) {
        BCM_FIELD_HEADER_FORMAT_SET_ADD(hfs, bcmFieldHeaderFormatIp4MplsLabel1);
        phfs &= (~SOC_TMC_FP_PKT_HDR_TYPE_IPV4_MPLS1_ETH);
    }
    if (phfs & SOC_TMC_FP_PKT_HDR_TYPE_IPV4_MPLS2_ETH) {
        BCM_FIELD_HEADER_FORMAT_SET_ADD(hfs, bcmFieldHeaderFormatIp4MplsLabel2);
        phfs &= (~SOC_TMC_FP_PKT_HDR_TYPE_IPV4_MPLS2_ETH);
    }
    if (phfs & SOC_TMC_FP_PKT_HDR_TYPE_IPV4_MPLS3_ETH) {
        BCM_FIELD_HEADER_FORMAT_SET_ADD(hfs, bcmFieldHeaderFormatIp4MplsLabel3);
        phfs &= (~SOC_TMC_FP_PKT_HDR_TYPE_IPV4_MPLS3_ETH);
    }
    if (phfs & SOC_TMC_FP_PKT_HDR_TYPE_IPV6_MPLS1_ETH) {
        BCM_FIELD_HEADER_FORMAT_SET_ADD(hfs, bcmFieldHeaderFormatIp6MplsLabel1);
        phfs &= (~SOC_TMC_FP_PKT_HDR_TYPE_IPV6_MPLS1_ETH);
    }
    if (phfs & SOC_TMC_FP_PKT_HDR_TYPE_IPV6_MPLS2_ETH) {
        BCM_FIELD_HEADER_FORMAT_SET_ADD(hfs, bcmFieldHeaderFormatIp6MplsLabel2);
        phfs &= (~SOC_TMC_FP_PKT_HDR_TYPE_IPV6_MPLS2_ETH);
    }
    if (phfs & SOC_TMC_FP_PKT_HDR_TYPE_IPV6_MPLS3_ETH) {
        BCM_FIELD_HEADER_FORMAT_SET_ADD(hfs, bcmFieldHeaderFormatIp6MplsLabel3);
        phfs &= (~SOC_TMC_FP_PKT_HDR_TYPE_IPV6_MPLS3_ETH);
    }
    if (phfs & SOC_TMC_FP_PKT_HDR_TYPE_IPV4_IPV4_ETH) {
        BCM_FIELD_HEADER_FORMAT_SET_ADD(hfs, bcmFieldHeaderFormatIp4Ip4);
        phfs &= (~SOC_TMC_FP_PKT_HDR_TYPE_IPV4_IPV4_ETH);
    }
    if (phfs & SOC_TMC_FP_PKT_HDR_TYPE_IPV6_IPV4_ETH) {
        BCM_FIELD_HEADER_FORMAT_SET_ADD(hfs, bcmFieldHeaderFormatIp6Ip4);
        phfs &= (~SOC_TMC_FP_PKT_HDR_TYPE_IPV6_IPV4_ETH);
    }
    if (phfs & SOC_TMC_FP_PKT_HDR_TYPE_ETH_TRILL_ETH) {
        BCM_FIELD_HEADER_FORMAT_SET_ADD(hfs, bcmFieldHeaderFormatEthTrillEth);
        phfs &= (~SOC_TMC_FP_PKT_HDR_TYPE_ETH_TRILL_ETH);
    }
    if (phfs) {
        LOG_WARN(BSL_LS_BCM_FP,
                 (BSL_META_U(unit,
                             "bits %08X left over in PPD header bitmap"
                             " after converted supported formats to BCM\n"),
                  phfs));
    }
    sal_memcpy(bcm_hfs, &hfs, sizeof(*bcm_hfs));
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_field_HeaderFormatSet_ppd_member_to_HeaderFormat_ppd(const uint32 hfs,
                                                              SOC_TMC_PARSER_PKT_HDR_STK_TYPE *hf)
{
    int unit;

    unit = BSL_UNIT_UNKNOWN;

    if (SOC_TMC_FP_PKT_HDR_TYPE_ETH == hfs) {
        *hf = SOC_TMC_PARSER_PKT_HDR_STK_TYPE_ETH;
    } else if (SOC_TMC_FP_PKT_HDR_TYPE_ETH_ETH == hfs) {
        *hf = SOC_TMC_PARSER_PKT_HDR_STK_TYPE_ETH_ETH;
    } else if (SOC_TMC_FP_PKT_HDR_TYPE_IPV4_ETH == hfs) {
        *hf = SOC_TMC_PARSER_PKT_HDR_STK_TYPE_IPV4_ETH;
    } else if (SOC_TMC_FP_PKT_HDR_TYPE_IPV6_ETH == hfs) {
        *hf = SOC_TMC_PARSER_PKT_HDR_STK_TYPE_IPV6_ETH;
    } else if (SOC_TMC_FP_PKT_HDR_TYPE_MPLS1_ETH == hfs) {
        *hf = SOC_TMC_PARSER_PKT_HDR_STK_TYPE_MPLS1_ETH;
    } else if (SOC_TMC_FP_PKT_HDR_TYPE_MPLS2_ETH == hfs) {
        *hf = SOC_TMC_PARSER_PKT_HDR_STK_TYPE_MPLS2_ETH;
    } else if (SOC_TMC_FP_PKT_HDR_TYPE_MPLS3_ETH == hfs) {
        *hf = SOC_TMC_PARSER_PKT_HDR_STK_TYPE_MPLS3_ETH;
    } else if (SOC_TMC_FP_PKT_HDR_TYPE_ETH_MPLS1_ETH == hfs) {
        *hf = SOC_TMC_PARSER_PKT_HDR_STK_TYPE_ETH_MPLS1_ETH;
    } else if (SOC_TMC_FP_PKT_HDR_TYPE_ETH_MPLS2_ETH == hfs) {
        *hf = SOC_TMC_PARSER_PKT_HDR_STK_TYPE_ETH_MPLS2_ETH;
    } else if (SOC_TMC_FP_PKT_HDR_TYPE_ETH_MPLS3_ETH == hfs) {
        *hf = SOC_TMC_PARSER_PKT_HDR_STK_TYPE_ETH_MPLS3_ETH;
    } else if (SOC_TMC_FP_PKT_HDR_TYPE_IPV4_MPLS1_ETH == hfs) {
        *hf = SOC_TMC_PARSER_PKT_HDR_STK_TYPE_IPV4_MPLS1_ETH;
    } else if (SOC_TMC_FP_PKT_HDR_TYPE_IPV4_MPLS2_ETH == hfs) {
        *hf = SOC_TMC_PARSER_PKT_HDR_STK_TYPE_IPV4_MPLS2_ETH;
    } else if (SOC_TMC_FP_PKT_HDR_TYPE_IPV4_MPLS3_ETH == hfs) {
        *hf = SOC_TMC_PARSER_PKT_HDR_STK_TYPE_IPV4_MPLS3_ETH;
    } else if (SOC_TMC_FP_PKT_HDR_TYPE_IPV6_MPLS1_ETH == hfs) {
        *hf = SOC_TMC_PARSER_PKT_HDR_STK_TYPE_IPV6_MPLS1_ETH;
    } else if (SOC_TMC_FP_PKT_HDR_TYPE_IPV6_MPLS2_ETH == hfs) {
        *hf = SOC_TMC_PARSER_PKT_HDR_STK_TYPE_IPV6_MPLS2_ETH;
    } else if (SOC_TMC_FP_PKT_HDR_TYPE_IPV6_MPLS3_ETH == hfs) {
        *hf = SOC_TMC_PARSER_PKT_HDR_STK_TYPE_IPV6_MPLS3_ETH;
    } else if (SOC_TMC_FP_PKT_HDR_TYPE_IPV4_IPV4_ETH == hfs) {
        *hf = SOC_TMC_PARSER_PKT_HDR_STK_TYPE_IPV4_IPV4_ETH;
    } else if (SOC_TMC_FP_PKT_HDR_TYPE_IPV6_IPV4_ETH == hfs) {
        *hf = SOC_TMC_PARSER_PKT_HDR_STK_TYPE_IPV6_IPV4_ETH;
    } else if (SOC_TMC_FP_PKT_HDR_TYPE_ETH_TRILL_ETH == hfs) {
        *hf = SOC_TMC_PARSER_PKT_HDR_STK_TYPE_ETH_TRILL_ETH;
    } else {
        LOG_ERROR(BSL_LS_BCM_FP,
                  (BSL_META_U(unit,
                              "unable to translate headerFormatSet %08X to"
                              " header format\n"),
                   hfs));
        return BCM_E_PARAM;
    }
    
    return BCM_E_NONE;
}


int
_bcm_dpp_field_color_bcm_to_ppd(int bcmCol,
                                uint32 *ppdCol)
{
    switch (bcmCol) {
    case BCM_FIELD_COLOR_GREEN:
        *ppdCol = 0;
        break;
    case BCM_FIELD_COLOR_YELLOW:
        *ppdCol = 1;
        break;
    case BCM_FIELD_COLOR_RED:
        *ppdCol = 2;
        break;
    case BCM_FIELD_COLOR_BLACK:
        *ppdCol = 3;
        break;
    default:
        return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}


int
_bcm_dpp_field_color_ppd_to_bcm(uint32 ppdCol,
                                int *bcmCol)
{
    switch (ppdCol) {
    case 0:
        *bcmCol = BCM_FIELD_COLOR_GREEN;
        break;
    case 1:
        *bcmCol = BCM_FIELD_COLOR_YELLOW;
        break;
    case 2:
        *bcmCol = BCM_FIELD_COLOR_RED;
        break;
    case 3:
        *bcmCol = BCM_FIELD_COLOR_BLACK;
        break;
    default:
        return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}


int
_bcm_dpp_field_arp_opcode_bcm_to_ppd(bcm_field_ArpOpcode_t bcmArpOpcode,
                                uint32 *ppdArpOpcode)
{
    switch (bcmArpOpcode) {
    case bcmFieldArpOpcodeRequest:
        *ppdArpOpcode = 1;
        break;
    case bcmFieldArpOpcodeReply:
        *ppdArpOpcode = 2;
        break;
    default:
        return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}

int
_bcm_dpp_field_arp_opcode_ppd_to_bcm(bcm_field_ArpOpcode_t ppdArpOpcode,
                                bcm_field_ArpOpcode_t *bcmArpOpcode)
{
    switch (ppdArpOpcode) {
    case 1:
        *bcmArpOpcode = bcmFieldArpOpcodeRequest;
        break;
    case 2:
        *bcmArpOpcode = bcmFieldArpOpcodeReply;
        break;
    default:
        return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}


int
_bcm_dpp_field_stp_state_bcm_to_ppd(bcm_stg_stp_t bcmStp,
                                    SOC_PPC_PORT_STP_STATE_FLD_VAL *ppdStp)
{
    switch (bcmStp) {
    case BCM_STG_STP_BLOCK:
    case BCM_STG_STP_DISABLE:
    case BCM_STG_STP_LISTEN:
        *ppdStp = SOC_PPC_PORT_STP_STATE_FLD_VAL_BLOCK;
        break;
    case BCM_STG_STP_LEARN:
        *ppdStp = SOC_PPC_PORT_STP_STATE_FLD_VAL_LEARN;
        break;
    case BCM_STG_STP_FORWARD:
        *ppdStp = SOC_PPC_PORT_STP_STATE_FLD_VAL_FORWARD;
        break;
    default:
        return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}


int
_bcm_dpp_field_stp_state_ppd_to_bcm(SOC_PPC_PORT_STP_STATE_FLD_VAL ppdStp,
                                    bcm_stg_stp_t *bcmStp)
{
    switch (ppdStp) {
    case SOC_PPC_PORT_STP_STATE_FLD_VAL_BLOCK:
        *bcmStp = BCM_STG_STP_BLOCK;
        break;
    case SOC_PPC_PORT_STP_STATE_FLD_VAL_LEARN:
        *bcmStp = BCM_STG_STP_LEARN;
        break;
    case SOC_PPC_PORT_STP_STATE_FLD_VAL_FORWARD:
        *bcmStp = BCM_STG_STP_FORWARD;
        break;
    default:
        return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}


int
_bcm_dpp_field_forwarding_type_bcm_to_ppd(bcm_field_ForwardingType_t bcmForwardingType,
                                          SOC_TMC_PKT_FRWRD_TYPE *ppdForwardingType,
                                          uint32 *ppdMask)
{
    *ppdMask = 0xF;
    switch(bcmForwardingType) {
    case bcmFieldForwardingTypeL2:
        *ppdForwardingType = SOC_TMC_PKT_FRWRD_TYPE_BRIDGE;
        break;
    case bcmFieldForwardingTypeIp4Ucast:
        *ppdForwardingType = SOC_TMC_PKT_FRWRD_TYPE_IPV4_UC;
        break;
    case bcmFieldForwardingTypeIp4Mcast:
        *ppdForwardingType = SOC_TMC_PKT_FRWRD_TYPE_IPV4_MC;
        break;
    case bcmFieldForwardingTypeIp6Ucast:
        *ppdForwardingType = SOC_TMC_PKT_FRWRD_TYPE_IPV6_UC;
        break;
    case bcmFieldForwardingTypeIp6Mcast:
        *ppdForwardingType = SOC_TMC_PKT_FRWRD_TYPE_IPV6_MC;
        break;
    case bcmFieldForwardingTypeMpls:
    case bcmFieldForwardingTypeMplsLabel1:
    case bcmFieldForwardingTypeMplsLabel2:
    case bcmFieldForwardingTypeMplsLabel3:
        *ppdForwardingType = SOC_TMC_PKT_FRWRD_TYPE_MPLS;
        break;
    case bcmFieldForwardingTypeTrill:
        *ppdForwardingType = SOC_TMC_PKT_FRWRD_TYPE_TRILL;
        break;
    case bcmFieldForwardingTypeRxReason:
        *ppdForwardingType = SOC_TMC_PKT_FRWRD_TYPE_CPU_TRAP;
        break;
    case bcmFieldForwardingTypeFCoE:
        *ppdForwardingType = SOC_TMC_PKT_FRWRD_TYPE_CUSTOM1;
        break;
    case bcmFieldForwardingTypeTrafficManagement:
        *ppdForwardingType = SOC_TMC_PKT_FRWRD_TYPE_TM;
        break;
    case bcmFieldForwardingTypeSnoop:
        *ppdForwardingType = SOC_TMC_PKT_FRWRD_TYPE_SNOOP;
        break;
    case bcmFieldForwardingTypeAny:
        /* 
         * Special case: select maximum of Ethernet-based packets. 
         * In practice, all PP packets except FCoE 
         */
        *ppdForwardingType = 0;
        *ppdMask = 0x8; /* MSB unset */
        break;
    default:
        return BCM_E_PARAM;
    };
    return BCM_E_NONE;
}


int
_bcm_dpp_field_forwarding_type_ppd_to_bcm(SOC_TMC_PKT_FRWRD_TYPE ppdForwardingType,
                                          uint32 ppdMask,
                                          bcm_field_ForwardingType_t *bcmForwardingType)
{
    if (ppdMask == 0xF) {
        switch(ppdForwardingType) {
        case SOC_TMC_PKT_FRWRD_TYPE_BRIDGE:
            *bcmForwardingType = bcmFieldForwardingTypeL2;
            break;
        case SOC_TMC_PKT_FRWRD_TYPE_IPV4_UC:
            *bcmForwardingType = bcmFieldForwardingTypeIp4Ucast;
            break;
        case SOC_TMC_PKT_FRWRD_TYPE_IPV4_MC:
            *bcmForwardingType = bcmFieldForwardingTypeIp4Mcast;
            break;
        case SOC_TMC_PKT_FRWRD_TYPE_IPV6_UC:
            *bcmForwardingType = bcmFieldForwardingTypeIp6Ucast;
            break;
        case SOC_TMC_PKT_FRWRD_TYPE_IPV6_MC:
            *bcmForwardingType = bcmFieldForwardingTypeIp6Mcast;
            break;
        case SOC_TMC_PKT_FRWRD_TYPE_MPLS:
            *bcmForwardingType = bcmFieldForwardingTypeMpls;
            break;
        case SOC_TMC_PKT_FRWRD_TYPE_TRILL:
            *bcmForwardingType = bcmFieldForwardingTypeTrill;
            break;
        case SOC_TMC_PKT_FRWRD_TYPE_CPU_TRAP:
            *bcmForwardingType = bcmFieldForwardingTypeRxReason;
            break;
        case SOC_TMC_PKT_FRWRD_TYPE_CUSTOM1:
            *bcmForwardingType = bcmFieldForwardingTypeFCoE;
            break;
        case SOC_TMC_PKT_FRWRD_TYPE_TM:
            *bcmForwardingType = bcmFieldForwardingTypeTrafficManagement;
            break;
        case SOC_TMC_PKT_FRWRD_TYPE_SNOOP:
            *bcmForwardingType = bcmFieldForwardingTypeSnoop;
            break;
        default:
            return BCM_E_PARAM;
        };
    }
    else if (ppdMask == 0x8) {
        if (ppdForwardingType == 0) {
            *bcmForwardingType = bcmFieldForwardingTypeAny;
        }
        else {
            return BCM_E_PARAM;
        }
    }

    return BCM_E_NONE;
}


int
_bcm_dpp_field_app_type_bcm_to_ppd(int unit,
                                   bcm_field_AppType_t bcmAppType,
                                   uint32 *ppd_flp_program)
{
    BCMDNX_INIT_FUNC_DEFS ;

    *ppd_flp_program = 0;

#ifdef BCM_ARAD_SUPPORT
    if (SOC_IS_ARAD(unit)) {
        uint32
            sandResult,
            ppdAppType = 0;
        uint8  
            prog_index = 0;

        switch(bcmAppType) {
        case bcmFieldAppTypeL2:
            ppdAppType = ARAD_PP_FLP_KEY_PROGRAM_ETHERNET_ING_LEARN;
            break;
        case bcmFieldAppTypeTrafficManagement:
            ppdAppType = ARAD_PP_FLP_KEY_PROGRAM_TM;
            break;
        case bcmFieldAppTypeIp4Ucast:
            ppdAppType = ARAD_PP_FLP_KEY_PROGRAM_IPV4UC;
            break;
        case bcmFieldAppTypeIp4UcastRpf:
            ppdAppType = ARAD_PP_FLP_KEY_PROGRAM_IPV4UC_WITH_RPF;
            break;
        case bcmFieldAppTypeIp4McastRpf:
            ppdAppType = ARAD_PP_FLP_KEY_PROGRAM_IPV4MC_WITH_RPF;
            break;
        case bcmFieldAppTypeCompIp4McastRpf:
            ppdAppType = ARAD_PP_FLP_KEY_PROGRAM_IPV4COMPMC_WITH_RPF;
            break;
        case bcmFieldAppTypeIp6Ucast:
            ppdAppType = ARAD_PP_FLP_KEY_PROGRAM_IPV6UC;
            break;
        case bcmFieldAppTypeIp6Mcast:
            ppdAppType = ARAD_PP_FLP_KEY_PROGRAM_IPV6MC;
            break;
        case bcmFieldAppTypeTrillUcast:
            ppdAppType = ARAD_PP_FLP_KEY_PROGRAM_TRILL_UC;
            break;
        case bcmFieldAppTypeTrillMcast:
            ppdAppType = ARAD_PP_FLP_KEY_PROGRAM_TRILL_MC_ONE_TAG;
            break;
        case bcmFieldAppTypeFglTrillMcast:
            ppdAppType = ARAD_PP_FLP_KEY_PROGRAM_TRILL_MC_TWO_TAGS;
            break;
        case bcmFieldAppTypeMpls:
            ppdAppType = ARAD_PP_FLP_KEY_PROGRAM_LSR;
            break;
        case bcmFieldAppTypeMplsCoupling:
            ppdAppType = ARAD_PP_FLP_KEY_PROGRAM_COUPLING_LSR;
            break;
        case bcmFieldAppTypeIp4SrcBind:
            ppdAppType = ARAD_PP_FLP_KEY_PROGRAM_ETHERNET_TK_EPON_UNI_V4_STATIC;
            break;
        case bcmFieldAppTypeIp6SrcBind:
            ppdAppType = ARAD_PP_FLP_KEY_PROGRAM_ETHERNET_TK_EPON_UNI_V6_STATIC;
            break;
        case bcmFieldAppTypeIp4MacSrcBind:
            ppdAppType = ARAD_PP_FLP_KEY_PROGRAM_ETHERNET_TK_EPON_UNI_V4_DHCP;
            break;
        case bcmFieldAppTypeIp6MacSrcBind:
            ppdAppType = ARAD_PP_FLP_KEY_PROGRAM_ETHERNET_TK_EPON_UNI_V6_DHCP;
            break;
        case bcmFieldAppTypeMiM:
            ppdAppType = ARAD_PP_FLP_KEY_PROGRAM_ETHERNET_MAC_IN_MAC;
            break;
        case bcmFieldAppTypeL2VpnDirect:
            ppdAppType = ARAD_PP_FLP_KEY_PROGRAM_P2P;
            break;
        case bcmFieldAppTypeL2MiM:
            ppdAppType = ARAD_PP_FLP_KEY_PROGRAM_MAC_IN_MAC_AFTER_TERMINATIOM;
            break;
        case bcmFieldAppTypeL2TrillMcast:
            ppdAppType = ARAD_PP_FLP_KEY_PROGRAM_TRILL_AFTER_TERMINATION;
            break;
        case bcmFieldAppTypeFCoETransit:
            ppdAppType = ARAD_PP_FLP_KEY_PROGRAM_FC_TRANSIT;
            break;
        case bcmFieldAppTypeFCoE:
            ppdAppType = ARAD_PP_FLP_KEY_PROGRAM_FC;
            break;
        case bcmFieldAppTypeFCoEVft:
            ppdAppType = ARAD_PP_FLP_KEY_PROGRAM_FC_WITH_VFT;
            break;
        case bcmFieldAppTypeFCoERemote:
            ppdAppType = ARAD_PP_FLP_KEY_PROGRAM_FC_REMOTE;
            break;
        case bcmFieldAppTypeFCoEVftRemote:
            ppdAppType = ARAD_PP_FLP_KEY_PROGRAM_FC_WITH_VFT_REMOTE;
            break;
        case bcmFieldAppTypeIp6UcastRpf:
            if (SOC_DPP_CONFIG(unit)->pp.ipv6_with_rpf_2pass_exists) 
            {
                /* FLP Program FWD_PRCESSING_PROFILE of the IPv6-UC RPF 2PASS solution program */
                ppdAppType = ARAD_PP_FLP_KEY_PROGRAM_IPV6UC_WITH_RPF_2PASS;
            }
            else
            {
                    /* The standard IPv6-UC RPF FLP Program */
                    ppdAppType = ARAD_PP_FLP_KEY_PROGRAM_IPV6UC_WITH_RPF;
            }
            break;
        case bcmFieldAppTypeIp4McastBidir:
            ppdAppType = ARAD_PP_FLP_KEY_PROGRAM_BIDIR;
            break;
        case bcmFieldAppTypeVplsGreIp:
            ppdAppType = ARAD_PP_FLP_KEY_PROGRAM_VPLSOGRE;
            break;
		case bcmFieldAppTypeBfdIp4SingleHop:
			ppdAppType = ARAD_PP_FLP_KEY_PROGRAM_BFD_IPV4_SINGLE_HOP;
			break;
        default:
            return BCM_E_PARAM;
        };

        /* Translate the Application type to the FLP program */
        sandResult = arad_pp_flp_app_to_prog_index_get((unit), ppdAppType, &prog_index);
        BCMDNX_IF_ERR_EXIT(handle_sand_result(sandResult));
        *ppd_flp_program = prog_index;

    } /* if (SOC_IS_ARAD(unit))*/
#endif /* def BCM_ARAD_SUPPORT */

    goto exit;

exit:
    BCMDNX_FUNC_RETURN;
}


int
_bcm_dpp_field_app_type_ppd_to_bcm(int unit,
                                          uint32 ppd_flp_program,
                                          bcm_field_AppType_t *bcmAppType)
{
    BCMDNX_INIT_FUNC_DEFS ;

    *bcmAppType = bcmFieldAppTypeCount;
#ifdef BCM_ARAD_SUPPORT
    if (SOC_IS_ARAD(unit)) {
        uint8
            ppdAppType;

        /* Translate the FLP program to the application */
        BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.fec.flp_progs_mapping.get(unit, ppd_flp_program, &ppdAppType));

        switch(ppdAppType) {
        case ARAD_PP_FLP_KEY_PROGRAM_ETHERNET_ING_LEARN:
            *bcmAppType = bcmFieldAppTypeL2;
            break;
        case ARAD_PP_FLP_KEY_PROGRAM_TM:
            *bcmAppType = bcmFieldAppTypeTrafficManagement;
            break;
        case ARAD_PP_FLP_KEY_PROGRAM_IPV4UC:
            *bcmAppType = bcmFieldAppTypeIp4Ucast;
            break;
		case ARAD_PP_FLP_KEY_PROGRAM_BFD_IPV4_SINGLE_HOP:
			*bcmAppType = bcmFieldAppTypeBfdIp4SingleHop;
            break;
        case ARAD_PP_FLP_KEY_PROGRAM_IPV4UC_WITH_RPF:
            *bcmAppType = bcmFieldAppTypeIp4UcastRpf;
            break;
        case ARAD_PP_FLP_KEY_PROGRAM_IPV4MC_WITH_RPF:
            *bcmAppType = bcmFieldAppTypeIp4McastRpf;
            break;
        case ARAD_PP_FLP_KEY_PROGRAM_IPV4COMPMC_WITH_RPF:
            *bcmAppType = bcmFieldAppTypeCompIp4McastRpf;
            break;
        case ARAD_PP_FLP_KEY_PROGRAM_IPV6UC:
            *bcmAppType = bcmFieldAppTypeIp6Ucast;
            break;
        case ARAD_PP_FLP_KEY_PROGRAM_IPV6MC:
            *bcmAppType = bcmFieldAppTypeIp6Mcast;
            break;
        case ARAD_PP_FLP_KEY_PROGRAM_TRILL_UC:
        /* Same value case ARAD_PP_FLP_KEY_PROGRAM_ETHERNET_TK_EPON_UNI_V4_STATIC: */
            if (SOC_DPP_CONFIG(unit)->pp.pon_application_enable) {
                *bcmAppType = bcmFieldAppTypeIp4SrcBind;
            }
            else {
                *bcmAppType = bcmFieldAppTypeTrillUcast;
            }
            break;
        case ARAD_PP_FLP_KEY_PROGRAM_TRILL_MC_ONE_TAG:
        /* Same value: case ARAD_PP_FLP_KEY_PROGRAM_ETHERNET_TK_EPON_UNI_V6_STATIC: */
            if (SOC_DPP_CONFIG(unit)->pp.pon_application_enable) {
                *bcmAppType = bcmFieldAppTypeIp6SrcBind;
            }
            else {
                *bcmAppType = bcmFieldAppTypeTrillMcast;
            }
            break;
        case ARAD_PP_FLP_KEY_PROGRAM_LSR:
            *bcmAppType = bcmFieldAppTypeMpls;
            break;
        case ARAD_PP_FLP_KEY_PROGRAM_COUPLING_LSR:
            *bcmAppType = bcmFieldAppTypeMplsCoupling;
            break;
        case ARAD_PP_FLP_KEY_PROGRAM_ETHERNET_TK_EPON_UNI_V4_DHCP:
            *bcmAppType = bcmFieldAppTypeIp4MacSrcBind;
            break;
        case ARAD_PP_FLP_KEY_PROGRAM_ETHERNET_TK_EPON_UNI_V6_DHCP:
            *bcmAppType = bcmFieldAppTypeIp6MacSrcBind;
            break;
        case ARAD_PP_FLP_KEY_PROGRAM_ETHERNET_MAC_IN_MAC:
            *bcmAppType = bcmFieldAppTypeMiM;
            break;
        case ARAD_PP_FLP_KEY_PROGRAM_P2P:
            *bcmAppType = bcmFieldAppTypeL2VpnDirect;
            break;
        case ARAD_PP_FLP_KEY_PROGRAM_MAC_IN_MAC_AFTER_TERMINATIOM:
            /* Same value: case ARAD_PP_FLP_KEY_PROGRAM_TRILL_MC_TWO_TAGS */
            if (SOC_DPP_CONFIG(unit)->trill.mode) {
                *bcmAppType = bcmFieldAppTypeFglTrillMcast;
            }
            else {
                *bcmAppType = bcmFieldAppTypeL2MiM;
            }
            break;
            break;
        case ARAD_PP_FLP_KEY_PROGRAM_TRILL_AFTER_TERMINATION:
            /* Same value: case ARAD_PP_FLP_KEY_PROGRAM_FC_TRANSIT */
            if (SOC_DPP_CONFIG(unit)->trill.mode) {
                *bcmAppType = bcmFieldAppTypeL2TrillMcast;
            }
            else {
                *bcmAppType = bcmFieldAppTypeFCoETransit;
            }
            break;
        case ARAD_PP_FLP_KEY_PROGRAM_IPV6UC_WITH_RPF:
            *bcmAppType = bcmFieldAppTypeIp6UcastRpf;
            break;
        case ARAD_PP_FLP_KEY_PROGRAM_IPV6UC_WITH_RPF_2PASS:
            if (SOC_DPP_CONFIG(unit)->pp.ipv6_with_rpf_2pass_exists) 
            {
                *bcmAppType = bcmFieldAppTypeIp6UcastRpf;
            }
            break; /* If not 2pass exists and still FLP program is IPV6UC_WITH_RPF_2PASS, return def value set already */
        case ARAD_PP_FLP_KEY_PROGRAM_FC:
            *bcmAppType = bcmFieldAppTypeFCoE;
            break;
        case ARAD_PP_FLP_KEY_PROGRAM_FC_WITH_VFT:
            *bcmAppType = bcmFieldAppTypeFCoEVft;
            break;
        case ARAD_PP_FLP_KEY_PROGRAM_FC_REMOTE:
            *bcmAppType = bcmFieldAppTypeFCoERemote;
            break;
        case ARAD_PP_FLP_KEY_PROGRAM_FC_WITH_VFT_REMOTE:
            *bcmAppType = bcmFieldAppTypeFCoEVftRemote;
            break;
        case ARAD_PP_FLP_KEY_PROGRAM_VPLSOGRE:
            *bcmAppType = bcmFieldAppTypeVplsGreIp;
            break;
        case ARAD_PP_FLP_KEY_PROGRAM_BIDIR:
            *bcmAppType = bcmFieldAppTypeIp4McastBidir;
            break;
        default:
            return BCM_E_PARAM;
        };
    } /* if (SOC_IS_ARAD(unit))*/
#endif /* def BCM_ARAD_SUPPORT */

    goto exit;

exit:
    BCMDNX_FUNC_RETURN;
}

/* Which MPLS label in case it is MPLS */
int
_bcm_dpp_field_offset_ext_ppd_to_bcm(uint32 ppdOffsetExtData, uint32 ppdOffsetExtMask,
                                          bcm_field_ForwardingType_t *bcmForwardingType)
{
    if (ppdOffsetExtMask) {
        switch(ppdOffsetExtData) {
        case 0:
            *bcmForwardingType = bcmFieldForwardingTypeMplsLabel1;
            break;
        case 1:
            *bcmForwardingType = bcmFieldForwardingTypeMplsLabel2;
            break;
        case 2:
            *bcmForwardingType = bcmFieldForwardingTypeMplsLabel3;
            break;
        default:
            return BCM_E_PARAM;
        };
    }
    else {
        *bcmForwardingType = bcmFieldForwardingTypeMpls;
    }
    return BCM_E_NONE;
}

int
_bcm_dpp_field_offset_ext_bcm_to_ppd(bcm_field_ForwardingType_t bcmForwardingType,
                                        uint32 *ppdOffsetExtData, uint32 *ppdOffsetExtMask)
{
    switch(bcmForwardingType) {
    case bcmFieldForwardingTypeMplsLabel1:
        *ppdOffsetExtData = 0;
        *ppdOffsetExtMask = 0x3;
        break;
    case bcmFieldForwardingTypeMplsLabel2:
        *ppdOffsetExtData = 1;
        *ppdOffsetExtMask = 0x3;
        break;
    case bcmFieldForwardingTypeMplsLabel3:
        *ppdOffsetExtData = 2;
        *ppdOffsetExtMask = 0x3;
        break;
    default:
        *ppdOffsetExtData = 0;
        *ppdOffsetExtMask = 0;
    };
    return BCM_E_NONE;
}



int
_bcm_dpp_field_vlan_format_bcm_to_ppd(uint8 bcmVlanFormatBitmap,
                                      SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT *ppdVlanFormat)
{
    /* See the VLAN Tag handle summary to understand the mapping - or the get function below */
    if (bcmVlanFormatBitmap & BCM_FIELD_VLAN_FORMAT_OUTER_TAGGED) {
        if ((bcmVlanFormatBitmap & BCM_FIELD_VLAN_FORMAT_OUTER_TAGGED_VID_ZERO)
            || (bcmVlanFormatBitmap & BCM_FIELD_VLAN_FORMAT_INNER_TAGGED_VID_ZERO)) {
            /* Invalid bitmap */
            return BCM_E_PARAM;
        }
        else {
            if (bcmVlanFormatBitmap & BCM_FIELD_VLAN_FORMAT_INNER_TAGGED) {
                *ppdVlanFormat = SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_S_C_TAG;
            }
            else {
                *ppdVlanFormat = SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_S_TAG ;
            }
        }
    }
    else {
        /* No Outer-TAG */
        if (bcmVlanFormatBitmap & BCM_FIELD_VLAN_FORMAT_OUTER_TAGGED_VID_ZERO) {
            if (bcmVlanFormatBitmap & BCM_FIELD_VLAN_FORMAT_INNER_TAGGED_VID_ZERO) {
                /* Invalid bitmap */
                return BCM_E_PARAM;
            }
            else {
                if (bcmVlanFormatBitmap & BCM_FIELD_VLAN_FORMAT_INNER_TAGGED) {
                    *ppdVlanFormat = SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_PRIORITY_C_TAG;
                }
                else {
                    *ppdVlanFormat = SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_PRIORITY_TAG;
                }
            }
        }
        else {
            if (bcmVlanFormatBitmap) {
                /* Invalid bitmap */
                return BCM_E_PARAM;
            }
            else {
                *ppdVlanFormat = SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_NONE ;
            }
        }
    }
    return BCM_E_NONE;
}


int
_bcm_dpp_field_vlan_format_ppd_to_bcm(SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT ppdVlanFormat,
                                          uint8 *bcmVlanFormatBitmap)
{
    *bcmVlanFormatBitmap = 0;
    switch(ppdVlanFormat) {
    case SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_NONE:
        break;
    case SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_S_TAG:
        *bcmVlanFormatBitmap = BCM_FIELD_VLAN_FORMAT_OUTER_TAGGED;
        break;
    case SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_PRIORITY_TAG:
        *bcmVlanFormatBitmap = BCM_FIELD_VLAN_FORMAT_OUTER_TAGGED_VID_ZERO;
        break;
    case SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_PRIORITY_C_TAG:
        *bcmVlanFormatBitmap = BCM_FIELD_VLAN_FORMAT_OUTER_TAGGED_VID_ZERO
            | BCM_FIELD_VLAN_FORMAT_INNER_TAGGED;
        break;
    case SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_S_C_TAG:
        *bcmVlanFormatBitmap = BCM_FIELD_VLAN_FORMAT_OUTER_TAGGED
            | BCM_FIELD_VLAN_FORMAT_INNER_TAGGED;
        break;
    default:
        return BCM_E_PARAM;
    };
    return BCM_E_NONE;
}


int
_bcm_dpp_field_ip_type_bcm_to_ppd(bcm_field_IpType_t bcmIpType,
                                  uint8 is_egress,
                                    SOC_PPD_FP_PARSED_ETHERTYPE *ppdIpType)
{
    if (!is_egress) {
        switch (bcmIpType) {
        case bcmFieldIpTypeIpv4Any:
        case bcmFieldIpTypeIpv4NoOpts:
        case bcmFieldIpTypeIpv4WithOpts:
            *ppdIpType = SOC_PPD_FP_PARSED_ETHERTYPE_IPV4;
            break;
        case bcmFieldIpTypeIpv6:
            *ppdIpType = SOC_PPD_FP_PARSED_ETHERTYPE_IPV6;
            break;
        case bcmFieldIpTypeArp:
            *ppdIpType = SOC_PPD_FP_PARSED_ETHERTYPE_ARP;
            break;
        case bcmFieldIpTypeCfm:
            *ppdIpType = SOC_PPD_FP_PARSED_ETHERTYPE_CFM;
            break;
        case bcmFieldIpTypeTrill:
            *ppdIpType = SOC_PPD_FP_PARSED_ETHERTYPE_TRILL;
            break;
        case bcmFieldIpTypeMim:
            *ppdIpType = SOC_PPD_FP_PARSED_ETHERTYPE_MAC_IN_MAC;
            break;
        case bcmFieldIpTypeMplsUnicast:
            *ppdIpType = SOC_PPD_FP_PARSED_ETHERTYPE_MPLS;
            break;
        case bcmFieldIpTypeFCoE:
            *ppdIpType = SOC_PPD_FP_PARSED_ETHERTYPE_FC_E;
            break;
        case bcmFieldIpTypeMplsMulticast:
            *ppdIpType = (SOC_PPD_FP_PARSED_ETHERTYPE_USER_DEFINED_FIRST + 6);
            break;
        /* Special case for User-Header applications */
        case bcmFieldIpTypeCount:
            *ppdIpType = (SOC_PPD_FP_PARSED_ETHERTYPE_USER_DEFINED_FIRST + 4);
            break;
        case bcmFieldIpTypeAny:
            *ppdIpType = SOC_PPD_FP_PARSED_ETHERTYPE_NO_MATCH;
            break;
        default:
            return BCM_E_PARAM;
        }
    }
    else {
        switch (bcmIpType) {
        case bcmFieldIpTypeIpv4Any:
        case bcmFieldIpTypeIpv4NoOpts:
        case bcmFieldIpTypeIpv4WithOpts:
            *ppdIpType = SOC_PPD_FP_PARSED_ETHERTYPE_EGRESS_IPV4;
            break;
        case bcmFieldIpTypeIpv6:
            *ppdIpType = SOC_PPD_FP_PARSED_ETHERTYPE_EGRESS_IPV6;
            break;
        case bcmFieldIpTypeArp:
            *ppdIpType = SOC_PPD_FP_PARSED_ETHERTYPE_EGRESS_ARP;
            break;
        case bcmFieldIpTypeTrill:
            *ppdIpType = SOC_PPD_FP_PARSED_ETHERTYPE_EGRESS_TRILL;
            break;
        case bcmFieldIpTypeMim:
            *ppdIpType = SOC_PPD_FP_PARSED_ETHERTYPE_EGRESS_MAC_IN_MAC;
            break;
        case bcmFieldIpTypeCfm:
            *ppdIpType = SOC_PPD_FP_PARSED_ETHERTYPE_EGRESS_CFM;
            break;
        case bcmFieldIpTypeMplsUnicast:
            *ppdIpType = SOC_PPD_FP_PARSED_ETHERTYPE_EGRESS_MPLS;
            break;
        case bcmFieldIpTypeMplsMulticast:
            *ppdIpType = (SOC_PPD_FP_PARSED_ETHERTYPE_EGRESS_MPLS + 1);
            break;
        case bcmFieldIpTypeAny:
            *ppdIpType = SOC_PPD_FP_PARSED_ETHERTYPE_EGRESS_NO_MATCH;
            break;
        default:
            return BCM_E_PARAM;
        }
    }

    return BCM_E_NONE;
}


int
_bcm_dpp_field_ip_type_ppd_to_bcm(SOC_PPD_FP_PARSED_ETHERTYPE ppdIpType,
                                  uint8 is_egress,
                                    bcm_field_IpType_t *bcmIpType)
{
    if (!is_egress) {
        /* Special case for User-Header applications */
        if((uint32) ppdIpType == (uint32) SOC_PPD_FP_PARSED_ETHERTYPE_USER_DEFINED_FIRST + 4) {
            *bcmIpType = bcmFieldIpTypeCount;
        } else {
            switch (ppdIpType) {
            case SOC_PPD_FP_PARSED_ETHERTYPE_IPV4:
                *bcmIpType = bcmFieldIpTypeIpv4Any;
                break;
            case SOC_PPD_FP_PARSED_ETHERTYPE_IPV6:
                *bcmIpType = bcmFieldIpTypeIpv6;
                break;
            case SOC_PPD_FP_PARSED_ETHERTYPE_ARP:
                *bcmIpType = bcmFieldIpTypeArp;
                break;
            case SOC_PPD_FP_PARSED_ETHERTYPE_CFM:
                *bcmIpType = bcmFieldIpTypeCfm;
                break;
            case SOC_PPD_FP_PARSED_ETHERTYPE_TRILL:
                *bcmIpType = bcmFieldIpTypeTrill;
                break;
            case SOC_PPD_FP_PARSED_ETHERTYPE_MAC_IN_MAC:
                *bcmIpType = bcmFieldIpTypeMim;
                break;
            case SOC_PPD_FP_PARSED_ETHERTYPE_FC_E:
                *bcmIpType = bcmFieldIpTypeFCoE;
                break;
            case SOC_PPD_FP_PARSED_ETHERTYPE_MPLS:
                *bcmIpType = bcmFieldIpTypeMplsUnicast;
                break;
            case (SOC_PPD_FP_PARSED_ETHERTYPE_USER_DEFINED_FIRST + 6):
                *bcmIpType = bcmFieldIpTypeMplsMulticast;
                break;
            case SOC_PPD_FP_PARSED_ETHERTYPE_NO_MATCH:
                *bcmIpType = bcmFieldIpTypeAny;
                break;
            default:
                return BCM_E_PARAM;
            }
        }
    }
    else {
        switch ((SOC_PPD_FP_PARSED_ETHERTYPE_EGRESS)ppdIpType) {
        case SOC_PPD_FP_PARSED_ETHERTYPE_EGRESS_IPV4:
            *bcmIpType = bcmFieldIpTypeIpv4Any;
            break;
        case SOC_PPD_FP_PARSED_ETHERTYPE_EGRESS_IPV6:
            *bcmIpType = bcmFieldIpTypeIpv6;
            break;
        case SOC_PPD_FP_PARSED_ETHERTYPE_EGRESS_ARP:
            *bcmIpType = bcmFieldIpTypeArp;
            break;
        case SOC_PPD_FP_PARSED_ETHERTYPE_EGRESS_TRILL:
            *bcmIpType = bcmFieldIpTypeTrill;
            break;
        case SOC_PPD_FP_PARSED_ETHERTYPE_EGRESS_MAC_IN_MAC:
            *bcmIpType = bcmFieldIpTypeMim;
            break;
        case SOC_PPD_FP_PARSED_ETHERTYPE_EGRESS_CFM:
            *bcmIpType = bcmFieldIpTypeCfm;
            break;
        case SOC_PPD_FP_PARSED_ETHERTYPE_EGRESS_MPLS:
            *bcmIpType = bcmFieldIpTypeMplsUnicast;
            break;
        case (SOC_PPD_FP_PARSED_ETHERTYPE_EGRESS_MPLS + 1):
            *bcmIpType = bcmFieldIpTypeMplsMulticast;
            break;
        case SOC_PPD_FP_PARSED_ETHERTYPE_EGRESS_NO_MATCH:
            *bcmIpType = bcmFieldIpTypeAny;
            break;
        default:
            return BCM_E_PARAM;
        }
    }
    return BCM_E_NONE;
}


int
_bcm_dpp_field_l2_eth_format_bcm_to_ppd(bcm_field_L2Format_t bcmL2Format,
                                    SOC_PPD_FP_ETH_ENCAPSULATION *ppdL2Format)
{
    switch (bcmL2Format) {
    case bcmFieldL2FormatMiM:
    case bcmFieldL2FormatAny:
        *ppdL2Format = SOC_PPD_FP_ETH_ENCAPSULATION_UNDEF;
        break;
    case bcmFieldL2FormatEthII:
        *ppdL2Format = SOC_PPD_FP_ETH_ENCAPSULATION_ETH_II;
        break;
    case bcmFieldL2FormatSnap:
        *ppdL2Format = SOC_PPD_FP_ETH_ENCAPSULATION_LLC_SNAP;
        break;
    case bcmFieldL2FormatLlc:
        *ppdL2Format = SOC_PPD_FP_ETH_ENCAPSULATION_LLC;
        break;
    default:
        return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}


int
_bcm_dpp_field_l2_eth_format_ppd_to_bcm(SOC_PPD_FP_ETH_ENCAPSULATION ppdL2Format,
                                        bcm_field_L2Format_t *bcmL2Format)
{
    switch (ppdL2Format) {
    case SOC_PPD_FP_ETH_ENCAPSULATION_UNDEF:
        *bcmL2Format = bcmFieldL2FormatAny;
        break;
    case SOC_PPD_FP_ETH_ENCAPSULATION_ETH_II:
        *bcmL2Format = bcmFieldL2FormatEthII;
        break;
    case SOC_PPD_FP_ETH_ENCAPSULATION_LLC_SNAP:
        *bcmL2Format = bcmFieldL2FormatSnap;
        break;
    case SOC_PPD_FP_ETH_ENCAPSULATION_LLC:
        *bcmL2Format = bcmFieldL2FormatLlc;
        break;
    default:
        return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}


/* Translation during the ForwardingHeader Offset qualifier to know which offset */
int
_bcm_dpp_field_base_header_bcm_to_ppd(bcm_field_data_offset_base_t bcm_base_header,
                                      uint32 *ppd_base_header)
{
    switch (bcm_base_header) {
    case bcmFieldDataOffsetBasePacketStart:
        *ppd_base_header = 0;
        break;
    case bcmFieldDataOffsetBaseL2Header:
        *ppd_base_header = 1;
        break;
    case bcmFieldDataOffsetBaseFirstHeader:
        *ppd_base_header = 2;
        break;
    case bcmFieldDataOffsetBaseSecondHeader:
        *ppd_base_header = 3;
        break;
    case bcmFieldDataOffsetBaseThirdHeader:
        *ppd_base_header = 4;
        break;
    case bcmFieldDataOffsetBaseFourthHeader:
        *ppd_base_header = 5;
        break;
    default:
        return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}

int
_bcm_dpp_field_base_header_ppd_to_bcm(uint32 ppd_base_header,
                                      bcm_field_data_offset_base_t *bcm_base_header)
{
    switch (ppd_base_header) {
    case 0:
        *bcm_base_header = bcmFieldDataOffsetBasePacketStart;
        break;
    case 1:
        *bcm_base_header = bcmFieldDataOffsetBaseL2Header;
        break;
    case 2:
        *bcm_base_header = bcmFieldDataOffsetBaseFirstHeader;
        break;
    case 3:
        *bcm_base_header = bcmFieldDataOffsetBaseSecondHeader;
        break;
    case 4:
        *bcm_base_header = bcmFieldDataOffsetBaseThirdHeader;
        break;
    case 5:
        *bcm_base_header = bcmFieldDataOffsetBaseFourthHeader;
        break;
    default:
        return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}



int
_bcm_dpp_field_ip_next_protocol_bcm_to_ppd(bcm_field_IpProtocolCommon_t bcmIpNextProtocol,
                                    SOC_PPD_FP_PARSED_IP_NEXT_PROTOCOL *ppdIpNextProtocol)
{
    switch (bcmIpNextProtocol) {
    case bcmFieldIpProtocolCommonTcp:
        *ppdIpNextProtocol = SOC_PPD_PARSED_IP_NEXT_PROTOCOL_TCP;
        break;
    case bcmFieldIpProtocolCommonUdp:
        *ppdIpNextProtocol = SOC_PPD_PARSED_IP_NEXT_PROTOCOL_UDP;
        break;
    case bcmFieldIpProtocolCommonIgmp:
        *ppdIpNextProtocol = SOC_PPD_PARSED_IP_NEXT_PROTOCOL_IGMP;
        break;
    case bcmFieldIpProtocolCommonIcmp:
        *ppdIpNextProtocol = SOC_PPD_PARSED_IP_NEXT_PROTOCOL_ICMP;
        break;
    case bcmFieldIpProtocolCommonIp6Icmp:
        *ppdIpNextProtocol = SOC_PPD_PARSED_IP_NEXT_PROTOCOL_ICMP_V6;
        break;
    case bcmFieldIpProtocolCommonIpInIp:
        *ppdIpNextProtocol = SOC_PPD_PARSED_IP_NEXT_PROTOCOL_IPV4;
        break;
    case bcmFieldIpProtocolCommonIp6InIp:
        *ppdIpNextProtocol = SOC_PPD_PARSED_IP_NEXT_PROTOCOL_IPV6;
        break;
    case bcmFieldIpProtocolCommonMplsInIp:
        *ppdIpNextProtocol = SOC_PPD_PARSED_IP_NEXT_PROTOCOL_MPLS;
        break;
    case bcmFieldIpProtocolCommonUnknown:
        *ppdIpNextProtocol = SOC_PPD_PARSED_IP_NEXT_PROTOCOL_NO_MATCH;
        break;
    default:
        return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}


int
_bcm_dpp_field_ip_next_protocol_ppd_to_bcm(SOC_PPD_FP_PARSED_IP_NEXT_PROTOCOL ppdIpNextProtocol,
                                    bcm_field_IpProtocolCommon_t *bcmIpNextProtocol)
{
    switch (ppdIpNextProtocol) {
    case SOC_PPD_PARSED_IP_NEXT_PROTOCOL_TCP:
        *bcmIpNextProtocol = bcmFieldIpProtocolCommonTcp;
        break;
    case SOC_PPD_PARSED_IP_NEXT_PROTOCOL_UDP:
        *bcmIpNextProtocol = bcmFieldIpProtocolCommonUdp;
        break;
    case SOC_PPD_PARSED_IP_NEXT_PROTOCOL_IGMP:
        *bcmIpNextProtocol = bcmFieldIpProtocolCommonIgmp;
        break;
    case SOC_PPD_PARSED_IP_NEXT_PROTOCOL_ICMP:
        *bcmIpNextProtocol = bcmFieldIpProtocolCommonIcmp;
        break;
    case SOC_PPD_PARSED_IP_NEXT_PROTOCOL_ICMP_V6:
        *bcmIpNextProtocol = bcmFieldIpProtocolCommonIp6Icmp;
        break;
    case SOC_PPD_PARSED_IP_NEXT_PROTOCOL_IPV4:
        *bcmIpNextProtocol = bcmFieldIpProtocolCommonIpInIp;
        break;
    case SOC_PPD_PARSED_IP_NEXT_PROTOCOL_IPV6:
        *bcmIpNextProtocol = bcmFieldIpProtocolCommonIp6InIp;
        break;
    case SOC_PPD_PARSED_IP_NEXT_PROTOCOL_MPLS:
        *bcmIpNextProtocol = bcmFieldIpProtocolCommonMplsInIp;
        break;
    case SOC_PPD_PARSED_IP_NEXT_PROTOCOL_NO_MATCH:
        *bcmIpNextProtocol = bcmFieldIpProtocolCommonUnknown;
        break;
    default:
        return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}


int
_bcm_dpp_field_termination_type_bcm_to_ppd(bcm_field_TunnelType_t bcmTerminationType,
                                           SOC_PPD_PKT_TERM_TYPE *ppdTerminationType)
{
    switch (bcmTerminationType) {
    case bcmFieldTunnelTypeIp:
        *ppdTerminationType = SOC_PPD_PKT_TERM_TYPE_IPV4_ETH;
        break;
    case bcmFieldTunnelTypeIp6:
        *ppdTerminationType = SOC_PPD_PKT_TERM_TYPE_IPV6_ETH;
        break;
    case bcmFieldTunnelTypeMpls:
        *ppdTerminationType = SOC_PPD_PKT_TERM_TYPE_MPLS_ETH;
        break;
    case bcmFieldTunnelTypeMplsControlWord:
        *ppdTerminationType = SOC_PPD_PKT_TERM_TYPE_CW_MPLS_ETH;
        break;
    case bcmFieldTunnelTypeMplsLabel2:
        *ppdTerminationType = SOC_PPD_PKT_TERM_TYPE_MPLS2_ETH;
        break;
    case bcmFieldTunnelTypeMplsLabel2ControlWord:
        *ppdTerminationType = SOC_PPD_PKT_TERM_TYPE_CW_MPLS2_ETH;
        break;
    case bcmFieldTunnelTypeMplsLabel3:
        *ppdTerminationType = SOC_PPD_PKT_TERM_TYPE_MPLS3_ETH;
        break;
    case bcmFieldTunnelTypeMplsLabel3ControlWord:
        *ppdTerminationType = SOC_PPD_PKT_TERM_TYPE_CW_MPLS3_ETH;
        break;
    case bcmFieldTunnelTypeTrill:
        *ppdTerminationType = SOC_PPD_PKT_TERM_TYPE_TRILL;
        break;
    case bcmFieldTunnelTypeL2Gre:
    case bcmFieldTunnelTypeNone:
        *ppdTerminationType = SOC_PPD_PKT_TERM_TYPE_ETH;
        break;
    default:
        return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}


int
_bcm_dpp_field_termination_type_ppd_to_bcm(SOC_PPD_PKT_TERM_TYPE ppdTerminationType,
                                           bcm_field_TunnelType_t *bcmTerminationType)
{
    switch (ppdTerminationType) {
    case SOC_PPD_PKT_TERM_TYPE_IPV4_ETH:
        *bcmTerminationType = bcmFieldTunnelTypeIp;
        break;
    case SOC_PPD_PKT_TERM_TYPE_IPV6_ETH:
        *bcmTerminationType = bcmFieldTunnelTypeIp6;
        break;
    case SOC_PPD_PKT_TERM_TYPE_MPLS_ETH:
        *bcmTerminationType = bcmFieldTunnelTypeMpls;
        break;
    case SOC_PPD_PKT_TERM_TYPE_CW_MPLS_ETH:
        *bcmTerminationType = bcmFieldTunnelTypeMplsControlWord;
        break;
    case SOC_PPD_PKT_TERM_TYPE_MPLS2_ETH:
        *bcmTerminationType = bcmFieldTunnelTypeMplsLabel2;
        break;
    case SOC_PPD_PKT_TERM_TYPE_CW_MPLS2_ETH:
        *bcmTerminationType = bcmFieldTunnelTypeMplsLabel2ControlWord;
        break;
    case SOC_PPD_PKT_TERM_TYPE_MPLS3_ETH:
        *bcmTerminationType = bcmFieldTunnelTypeMplsLabel3;
        break;
    case SOC_PPD_PKT_TERM_TYPE_CW_MPLS3_ETH:
        *bcmTerminationType = bcmFieldTunnelTypeMplsLabel3ControlWord;
        break;
    case SOC_PPD_PKT_TERM_TYPE_TRILL:
        *bcmTerminationType = bcmFieldTunnelTypeTrill;
        break;
    case SOC_PPD_PKT_TERM_TYPE_ETH:
        *bcmTerminationType = bcmFieldTunnelTypeNone;
        break;
    default:
        return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}


int
_bcm_dpp_field_ip_frag_bcm_to_ppd(int unit, bcm_field_IpFrag_t bcmFragInfo,
                                    uint8 *ppdIpFragmented)
{

    if (SOC_IS_JERICHO(unit) && (soc_property_get(unit, spn_FIELD_IP_FIRST_FRAGMENT_PARSED, TRUE)) )
    {
        switch (bcmFragInfo) {
        case bcmFieldIpFragNonOrFirst:
            *ppdIpFragmented = 0;
            break;
        case bcmFieldIpFragNotFirst:
            *ppdIpFragmented = 1;
            break;
        default:
            return BCM_E_PARAM;
        }
    }
    else {
        switch (bcmFragInfo) {
        case bcmFieldIpFragNon:
            *ppdIpFragmented = 0;
            break;
        case bcmFieldIpFragAny:
            *ppdIpFragmented = 1;
            break;

        default:
            return BCM_E_PARAM;
        }
    }
    return BCM_E_NONE;
}

STATIC int
_bcm_dpp_field_presel_profile_type_convert(_bcm_dpp_field_info_t *unitData,
                                          _bcm_dpp_field_profile_type_t profile_type,
                                          SOC_PPC_FP_CONTROL_TYPE *control_ndx_type) 
{
    *control_ndx_type = SOC_PPC_NOF_FP_CONTROL_TYPES;
    switch (profile_type) {
    case _bcmDppFieldProfileTypeInPort:
        *control_ndx_type = SOC_PPC_FP_CONTROL_TYPE_IN_PORT_PROFILE;
        break;
    case _bcmDppFieldProfileTypeOutPort:
        *control_ndx_type = SOC_PPC_FP_CONTROL_TYPE_OUT_PORT_PROFILE;
        break;
    case _bcmDppFieldProfileTypeFlpProgramProfile:
        *control_ndx_type = SOC_PPC_FP_CONTROL_TYPE_FLP_PGM_PROFILE;
        break;
    case _bcmDppFieldProfileTypeInterfaceInPort:  
        *control_ndx_type = SOC_PPC_FP_CONTROL_TYPE_IN_TM_PORT_PROFILE;  
        break; 
    default:
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}


STATIC int
_bcm_dpp_field_presel_port_profile_hw_set(_bcm_dpp_field_info_t *unitData,
                                          int core_id,
                                          _bcm_dpp_field_profile_type_t profile_type,
                                          uint8 clear,
                                          uint32 profile,
                                          uint32 port_bitmap[SOC_PPC_FP_NOF_CONTROL_VALS]) 
{
    uint32
        success, 
        sandResult;
    SOC_PPD_FP_CONTROL_INDEX 
        control_ndx;
    SOC_PPD_FP_CONTROL_INFO 
        control_info;

    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;

    SOC_PPD_FP_CONTROL_INDEX_clear(&control_ndx);
    SOC_PPD_FP_CONTROL_INFO_clear(&control_info);

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_presel_profile_type_convert(unitData, profile_type, &(control_ndx.type)));
    control_ndx.val_ndx = profile;
    control_ndx.clear_val = clear? TRUE: FALSE;
    sal_memcpy(control_info.val, port_bitmap, sizeof(uint32) * SOC_PPC_FP_NOF_CONTROL_VALS);
    sandResult = soc_ppd_fp_control_set(unitData->unitHandle,
                                        core_id,
                                        &control_ndx,
                                        &control_info,
                                        &success);

    BCMDNX_IF_ERR_EXIT(handle_sand_result(sandResult));
    BCMDNX_IF_ERR_EXIT(translate_sand_success_failure(success));
    
exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_dpp_field_presel_port_profile_alloc_dealloc(_bcm_dpp_field_info_t *unitData,
                                                 _bcm_dpp_field_profile_type_t profile_type,
                                                 uint8 alloc,
                                                 int   nof_cores,
                                                 _bcm_dpp_core_ports_bitmap_t *core_ports_bitmap, 
                                                 uint64 *profile) /* if alloc=0 then IN else OUT */
{
    uint32 
        is_last,
        profile_idx,
        nof_profile_objects,
        empty_bitmap[SOC_PPC_FP_NOF_CONTROL_VALS];
    int core, get_nof_cores;
    _bcm_dpp_core_ports_bitmap_t  
        tmp_bitmap[MAX_NUM_OF_CORES];
    int eq, empty;

    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;

    sal_memset(empty_bitmap, 0, sizeof(empty_bitmap[0]) * SOC_PPC_FP_NOF_CONTROL_VALS);

    if(!alloc)
    {
        unitData->preselProfileRefs[profile_type][COMPILER_64_LO(*profile)]--;
        is_last = (unitData->preselProfileRefs[profile_type][COMPILER_64_LO(*profile)] == 0) ? TRUE :FALSE;
        if(is_last)
        {
            for (core = 0; core < SOC_DPP_DEFS_GET(unit, nof_cores); core++) {
                BCMDNX_IF_ERR_EXIT(
                   _bcm_dpp_field_presel_port_profile_hw_set(unitData, 
                                                          core,
                                                          profile_type, 
                                                          TRUE, /* clear */
                                                          COMPILER_64_LO(*profile), 
                                                          empty_bitmap));
            }

        }
    }
    else {
        /* 
         * In case of allocation, init the profile to 0 
         * Run over the possible profiles (1 to 7): 
         * - if not used, init the profile to it 
         * - if used, and equal, use this profile and stop the loop 
         * - if used and not equal, continue if no intersection, 
         * otherwise return error 
         */ 
        /* Init the profile to 0, to detect if no profile was found */
        COMPILER_64_ZERO(*profile);
        
        /* For each profile, make sure there is no intersection with port bitmap */
        for(profile_idx = 1; profile_idx < _BCM_DPP_PRESEL_NOF_PORT_PROFILES; profile_idx++) 
        {
            switch (profile_type) {
            case _bcmDppFieldProfileTypeInterfaceInPort:
            case _bcmDppFieldProfileTypeInPort:
            case _bcmDppFieldProfileTypeOutPort:
                nof_profile_objects = SOC_TMC_NOF_FAP_PORTS_ARAD;
                break;
            case _bcmDppFieldProfileTypeFlpProgramProfile:
                nof_profile_objects = SOC_DPP_DEFS_GET(unit, nof_flp_programs);
                break;
            default:
                return BCM_E_PARAM;
            }

            eq = TRUE;
            empty = TRUE;

            /* Check if there is an intersection between the two port bitmaps */
            sal_memset(tmp_bitmap, 0, sizeof(uint32) * SOC_PPC_FP_NOF_CONTROL_VALS);
            BCMDNX_IF_ERR_EXIT(
                _bcm_dpp_field_presel_port_profile_get(unit,
                                                       profile_type,
                                                       profile_idx,
                                                       &get_nof_cores,
                                                       tmp_bitmap));
            if (get_nof_cores != nof_cores) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                 (_BSL_BCM_MSG_NO_UNIT("Num of cores mismatch get=%d set=%d"),
                                  get_nof_cores,
                                  nof_cores));


            }
            /* Check on all cores : If equal on all cores use this profile, otherwise take an empty one.
               intersection on any core is considred error.*/
            for (core=0; core < nof_cores; core++) {

                if(0 != sal_memcmp(tmp_bitmap[core], empty_bitmap, sizeof(uint32) * SOC_PPC_FP_NOF_CONTROL_VALS))
                {
                    empty = FALSE;
                /* If port bitmaps are equal, then the same profile
                 * should be given to this PFG as well.
                 */
                    if(0 != sal_memcmp(core_ports_bitmap[core], tmp_bitmap[core], sizeof(uint32) * SOC_PPC_FP_NOF_CONTROL_VALS)) {
                        eq = FALSE;
                        /* Return error if intersection */
                        SHR_BITAND_RANGE(tmp_bitmap[core], core_ports_bitmap[core], 0, nof_profile_objects, tmp_bitmap[core]);
                        if (0 != sal_memcmp(tmp_bitmap[core], empty_bitmap, sizeof(uint32) * SOC_PPC_FP_NOF_CONTROL_VALS)) {
                            BCM_ERR_EXIT_NO_MSG(BCM_E_EXISTS);
                        }
                        break;
                    }
                }
            }


            if ( eq || empty ) {
                COMPILER_64_SET(*profile, 0, profile_idx);
            }

            if (eq) {
                break;
            }

        }

        if(COMPILER_64_IS_ZERO(*profile)) {
            BCM_ERR_EXIT_NO_MSG(BCM_E_EXISTS);
        }
        for (core=0; core < nof_cores; core++) {
            BCMDNX_IF_ERR_EXIT(
                _bcm_dpp_field_presel_port_profile_hw_set(unitData, 
                                                          core,
                                                          profile_type, 
                                                          FALSE, /* don't clear */
                                                          COMPILER_64_LO(*profile), 
                                                          core_ports_bitmap[core]));
        }
        unitData->preselProfileRefs[profile_type][COMPILER_64_LO(*profile)]++;


    }

#ifdef BCM_WARM_BOOT_SUPPORT
    _bcm_dpp_field_presel_all_wb_save(unitData, NULL, NULL);
#endif /* def BCM_WARM_BOOT_SUPPORT */

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_field_presel_port_profile_get(int unit,
                                       _bcm_dpp_field_profile_type_t profile_type,
                                       uint32 profile,
                                       int    *nof_cores,
                                       _bcm_dpp_core_ports_bitmap_t *core_port_bitmap)
{
    _DPP_FIELD_COMMON_LOCALS;
    
    uint32 
        sandResult;
    SOC_PPD_FP_CONTROL_INDEX 
        control_ndx;
    SOC_PPD_FP_CONTROL_INFO 
        control_info;
	int core;

    BCMDNX_INIT_FUNC_DEFS;

    _DPP_FIELD_UNIT_CHECK(unit, unitData);

    _DPP_FIELD_UNIT_LOCK(unitData);

    SOC_PPD_FP_CONTROL_INDEX_clear(&control_ndx);
    SOC_PPD_FP_CONTROL_INFO_clear(&control_info);

	*nof_cores = SOC_DPP_DEFS_GET(unit, nof_cores);
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_presel_profile_type_convert(unitData, profile_type, &(control_ndx.type)));
    control_ndx.val_ndx = profile;

    for (core = 0; core < *nof_cores; core++) {
        sal_memset(core_port_bitmap[core], 0, sizeof(_bcm_dpp_core_ports_bitmap_t));

        sandResult = soc_ppd_fp_control_get(unitData->unitHandle,
                                        core,
                                        &control_ndx,
                                        &control_info);

        result = handle_sand_result(sandResult);


        BCMDNX_IF_ERR_EXIT(result);

        sal_memcpy(core_port_bitmap[core], control_info.val, sizeof(control_info.val[0]) * SOC_PPC_FP_NOF_CONTROL_VALS);
    }

     _DPP_FIELD_UNIT_UNLOCK(unitData);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_field_presel_port_profile_set(int unit,
                                       _bcm_dpp_field_profile_type_t profile_type,
                                       int  nof_cores,
                                       _bcm_dpp_core_ports_bitmap_t *core_port_bitmap, 
                                       uint64 *profile)
{
    _DPP_FIELD_COMMON_LOCALS;
    BCMDNX_INIT_FUNC_DEFS;

    _DPP_FIELD_UNIT_CHECK(unit, unitData);

    _DPP_FIELD_UNIT_LOCK(unitData);

    result = _bcm_dpp_field_presel_port_profile_alloc_dealloc(unitData, 
                                                              profile_type,
                                                              TRUE, /* Alloc */
															  nof_cores,
                                                              core_port_bitmap, 
                                                              profile);
    _DPP_FIELD_UNIT_UNLOCK(unitData);
    BCMDNX_IF_ERR_EXIT(result);

exit:
    BCMDNX_FUNC_RETURN;
}
/*
 *  Function
 *     _bcm_dpp_clear_core_ports
 *  Purpose
 *     Clear core ports array.
 */
void _bcm_dpp_clear_core_ports(int                  nof_cores,
                               _bcm_dpp_core_ports_bitmap_t *core_port_bitmap)

{
    int core; 
	

    for (core = 0; core < nof_cores; core++) {
        sal_memset(core_port_bitmap[core], 0x00, sizeof(_bcm_dpp_core_ports_bitmap_t));
    }
}
/* 
 * Get the profile for a specific profile type for this entry. 
 * If the entry is preselector, all the profile types are relevant 
 * For other types of entries, only the FLP processing profile is relevant. 
 *  Get the profile value from the SW state, since the entry may not be in the HW.
 * The returned profile is 0 of not found or found 0. 
 */
int
_bcm_dpp_field_presel_port_entry_profile_get(_bcm_dpp_field_info_t *unitData,
                                             _bcm_dpp_field_profile_type_t profile_type,
                                             bcm_field_entry_t entry,
                                             uint64 *profile)
{
    _bcm_dpp_field_profile_type_t profile_type_lcl;
    uint32 
        qual_ndx,
        sandResult;
    _bcm_dpp_field_entry_common_t *common;
    _bcm_dpp_field_entry_common_ext_t *extCommon;
    _bcm_dpp_field_entry_type_t entryType;
    _bcm_dpp_field_qual_t *entryQual;

    SOC_PPD_FP_PFG_INFO pfgInfo;

    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;

    /* Init the profile to zero */
    COMPILER_64_SET(*profile, 0, 0);    
     
    /* Preselector case */
    if (_BCM_DPP_FIELD_ENTRY_IS_PRESEL(entry)) {
        entry &= (~BCM_FIELD_QUALIFY_PRESEL);
        sandResult = soc_ppd_fp_packet_format_group_get(unitData->unitHandle,
                                                        entry,
                                                        &pfgInfo);
        BCMDNX_IF_ERR_EXIT(handle_sand_result(sandResult));

        for(qual_ndx = 0; qual_ndx < SOC_PPC_FP_NOF_QUALS_PER_PFG_MAX; qual_ndx++)
        {
            switch (pfgInfo.qual_vals[qual_ndx].type) {
            case SOC_PPD_FP_QUAL_IRPP_IN_PORT_KEY_GEN_VAR_PS:
                profile_type_lcl = _bcmDppFieldProfileTypeInPort;
                break;
            case SOC_PPD_FP_QUAL_ERPP_OUT_PP_PORT_PMF_DATA_PS:
                profile_type_lcl = _bcmDppFieldProfileTypeOutPort;
                break;
            case SOC_PPD_FP_QUAL_FWD_PRCESSING_PROFILE:
                profile_type_lcl = _bcmDppFieldProfileTypeFlpProgramProfile;
                break;
            default:
                continue; /* continue to next index */
            }

            if (profile_type_lcl == profile_type) {
                COMPILER_64_SET(*profile, 0, pfgInfo.qual_vals[qual_ndx].val.arr[0]);     
                break;
            }
        }
    }
    else {
        if (profile_type == _bcmDppFieldProfileTypeFlpProgramProfile) {
            BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_entry_common_pointer(unitData,
                                                                        entry,
                                                                        &common,
                                                                        &extCommon,
                                                                        &entryType));
            if (entryType == _bcmDppFieldEntryTypeDirExt) {
                entryQual = (common)->entryQual;
            }
            else { /* internal/external TCAM */
                entryQual = _BCM_DPP_FIELD_TCAM_ENTRY_QUALS(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), 
                                                         _BCM_DPP_FIELD_TCAM_LOCAL_ID(unit, entry));
            }
            for (qual_ndx = 0; qual_ndx < SOC_PPD_FP_NOF_QUALS_PER_DB_MAX; qual_ndx++) {
                if (entryQual[qual_ndx].hwType == SOC_PPD_FP_QUAL_FWD_PRCESSING_PROFILE) {
                    COMPILER_64_SET(*profile, 0, COMPILER_64_LO(entryQual[qual_ndx].qualData));     
                    break;
                }
            }
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_field_presel_port_profile_clear_type(_bcm_dpp_field_info_t *unitData,
                                              _bcm_dpp_field_profile_type_t profile_type,
                                              bcm_field_entry_t entry)
{
    uint64 profile;

    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;

    /* Get the current profile */
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_presel_port_entry_profile_get(unitData, profile_type, entry, &profile));

    /* Dealloc this profile if not null */
    if (COMPILER_64_IS_ZERO(profile) == FALSE) {
        BCMDNX_IF_ERR_EXIT(
            _bcm_dpp_field_presel_port_profile_alloc_dealloc(unitData, 
                                                             profile_type, 
                                                             FALSE, /* Dealloc */
															 0,
                                                             NULL, /* For dealloc, should not be used */
                                                             &profile));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_field_presel_port_profile_clear_all(_bcm_dpp_field_info_t *unitData,
                                              bcm_field_entry_t entry)
{
    _bcm_dpp_field_profile_type_t profile_type;

    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;

    /* Clear all the profiles */
    for (profile_type = 0; profile_type < _bcmDppFieldProfileTypeCount; profile_type++) {
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_presel_port_profile_clear_type(unitData, profile_type, entry));
    }


exit:
    BCMDNX_FUNC_RETURN;
}


int
_bcm_dpp_field_ip_frag_ppd_to_bcm(int unit, uint8 ppdIpFragmented,
                                    bcm_field_IpFrag_t *bcmFragInfo)
{
    switch (ppdIpFragmented) {
    case 0:
        *bcmFragInfo = ( SOC_IS_JERICHO(unit) && (soc_property_get(unit, spn_FIELD_IP_FIRST_FRAGMENT_PARSED, TRUE)) ) ? bcmFieldIpFragNonOrFirst : bcmFieldIpFragNon;
        break;
    case 1:
        *bcmFragInfo = ( SOC_IS_JERICHO(unit) && (soc_property_get(unit, spn_FIELD_IP_FIRST_FRAGMENT_PARSED, TRUE)) ) ? bcmFieldIpFragNotFirst : bcmFieldIpFragAny;
        break;
    default:
        return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}

#ifdef BROADCOM_DEBUG
int
_bcm_dpp_field_entry_qual_dump(_bcm_dpp_field_info_t *unitData,
                               _bcm_dpp_field_stage_idx_t stage,
                               const _bcm_dpp_field_qual_t *qualData,
#if _BCM_DPP_FIELD_DUMP_VERIFY_PPD
                               const SOC_PPD_FP_QUAL_VAL *qualHwData,
#endif
                               unsigned int index,
                               int dumpRanges,
                               const char *prefix)
{
    bcm_mac_t macData;
    bcm_mac_t macMask;
    bcm_ip6_t ipv6Data;
    bcm_ip6_t ipv6Mask;
    bcm_field_header_format_t hdrFmt;
    uint64 data;
    uint64 mask;
    char format[32];
    unsigned int limit;
    unsigned int bits;
    unsigned int offset;
    int result;
    int retVal = 0;
	int unit = unitData->unit;

    if (SOC_PPD_NOF_FP_QUAL_TYPES == qualData[index].hwType) {
        /* nothing to display; don't do anything */
        return 0;
    }
    if ((bcmFieldQualifyCount > qualData[index].qualType) &&
        (unitData->qualMaps[qualData[index].qualType][_BCM_DPP_FIELD_QUALMAP_OFFSET_FLAGS] &
         _BCM_DPP_QUAL_FLAGS_RNG_VALID_BITS)) {
        /* it is a range type */
        if (dumpRanges) {
            result = _bcm_dpp_field_entry_range_dump(unitData,
                                                     qualData,
#if _BCM_DPP_FIELD_DUMP_VERIFY_PPD
                                                     qualHwData,
#endif
                                                     prefix);
            if (BCM_E_NONE == result) {
                retVal = 2; /* dumped the L4OPS range qualifier */
            } else {
                retVal = result; /* return the error that was encountered */
            }
        }
    } else {
        if (bcmFieldQualifyCount != qualData[index].qualType) {
            /* most stuff below uses this header */
#if _BCM_DPP_FIELD_DUMP_INTERMIX_PPD
            LOG_CLI((BSL_META_U(unit,
                                "%s    %s (%d) -> %s (%d)\n"),
                     prefix,
                     _bcm_dpp_field_qual_name[qualData[index].qualType],
                     qualData[index].qualType,
                     SOC_PPD_FP_QUAL_TYPE_to_string(qualData[index].hwType),
                     qualData[index].hwType));
#else /* _BCM_DPP_FIELD_DUMP_INTERMIX_PPD */
            LOG_CLI((BSL_META_U(unit,
                                "%s    %s (%d)\n"),
                     prefix,
                     _bcm_dpp_field_qual_name[qualData[index].qualType],
                     qualData[index].qualType));
#endif /* _BCM_DPP_FIELD_DUMP_INTERMIX_PPD */
        }
        /* build generic format based upon qualifier exposed size */
        result = _bcm_dpp_ppd_qual_bits(unitData,
                                        stage,
                                        qualData[index].hwType,
                                        &bits,
                                        NULL,
                                        NULL);
        if (BCM_E_NONE != result) {
            return result;
        }
        if (bits > 32) {
            sal_snprintf(&(format[0]),
                         sizeof(format) - 1,
                         "%%s      %%0%dX%%08X/%%0%dX%%08X\n",
                         (bits - 29) >> 2,
                         (bits - 29) >> 2);
        } else {
            sal_snprintf(&(format[0]),
                         sizeof(format) - 1,
                         "%%s      %%0%dX/%%0%dX\n",
                         (bits + 3) >> 2,
                         (bits + 3) >> 2);
        }
        switch (qualData[index].qualType) {
        case bcmFieldQualifyCount:
            if (_BCM_DPP_FIELD_PPD_QUAL_VALID(qualData[index].hwType)) {
                limit = 0;
                /* try mapping as (user-defined) data qualifier */
                result = _bcm_petra_field_map_ppd_udf_to_bcm(unitData,
                                                             qualData[index].hwType,
                                                             &offset);
                if (BCM_E_NONE == result) {
                    /* mapped PPD data field to BCM data field */
                    limit = 1;
                }
                switch (limit) {
                case 1:
                    /* programmable fields */
#if _BCM_DPP_FIELD_DUMP_INTERMIX_PPD
                    LOG_CLI((BSL_META_U(unit,
                                        "%s    DataField(%d) -> %s (%d)\n"),
                             prefix,
                             offset,
                             SOC_PPD_FP_QUAL_TYPE_to_string(qualData[index].hwType),
                             qualData[index].hwType));
#else /* _BCM_DPP_FIELD_DUMP_INTERMIX_PPD */
                    LOG_CLI((BSL_META_U(unit,
                                        "%s    DataField(%d)\n"),
                             prefix,
                             offset));
#endif /* _BCM_DPP_FIELD_DUMP_INTERMIX_PPD */
                    if (bits > 32) {
                        LOG_CLI((format,
                                 prefix,
                                 COMPILER_64_HI(qualData[index].qualData),
                                 COMPILER_64_LO(qualData[index].qualData),
                                 COMPILER_64_HI(qualData[index].qualMask),
                                 COMPILER_64_LO(qualData[index].qualMask)));
                    } else {
                            LOG_CLI((format,
                                     prefix,
                                     COMPILER_64_LO(qualData[index].qualData),
                                     COMPILER_64_LO(qualData[index].qualMask)));
                    }
                    retVal = 1; /* displayed a qualifier */
                default:
                    /* nothing to display here */
                    break;
                } /* switch (limit) */
            } /* if (hardware type is valid) */
            break;
        case bcmFieldQualifySrcMac:
        case bcmFieldQualifyDstMac:
            /* MAC address based qualifiers */
            data = qualData[index].qualData;
            mask = qualData[index].qualMask;
            for (offset = 0; offset < 6; offset++) {
                macData[5 - offset] = COMPILER_64_LO(data) & 0xFF;
                macMask[5 - offset] = COMPILER_64_LO(mask) & 0xFF;
                COMPILER_64_SHR(data, 8);
                COMPILER_64_SHR(mask, 8);
            }
            LOG_CLI((BSL_META_U(unit,
                                "%s      "
                     FIELD_MACA_FORMAT
                                "/"
                                FIELD_MACA_FORMAT
                                "\n"),
                     prefix,
                     FIELD_MACA_SHOW(macData),
                     FIELD_MACA_SHOW(macMask)));
            retVal = 1; /* displayed a qualifier */
            break;
        case bcmFieldQualifySrcIp6High:
        case bcmFieldQualifyDstIp6High:
            /* IPv6 (upper half) address based qualifiers */
            data = qualData[index].qualData;
            mask = qualData[index].qualMask;
            for (offset = 0; offset < 8; offset++) {
                ipv6Data[7 - offset] = COMPILER_64_LO(data) & 0xFF;
                ipv6Mask[7 - offset] = COMPILER_64_LO(mask) & 0xFF;
                ipv6Data[15 - offset] = 0;
                ipv6Mask[15 - offset] = 0;
                COMPILER_64_SHR(data, 8);
                COMPILER_64_SHR(mask, 8);
            }
            LOG_CLI((BSL_META_U(unit,
                                "%s      "
                     FIELD_IPV6A_FORMAT
                                "/\n%s      "
                                FIELD_IPV6A_FORMAT
                                "\n"),
                     prefix,
                     FIELD_IPV6A_SHOW(ipv6Data),
                     prefix,
                     FIELD_IPV6A_SHOW(ipv6Mask)));
            retVal = 1; /* displayed a qualifier */
            break;
        case bcmFieldQualifySrcIp6Low:
        case bcmFieldQualifyDstIp6Low:
            /* IPv6 (lower half) address based qualifiers */
            data = qualData[index].qualData;
            mask = qualData[index].qualMask;
            for (offset = 0; offset < 8; offset++) {
                ipv6Data[15 - offset] = COMPILER_64_LO(data) & 0xFF;
                ipv6Mask[15 - offset] = COMPILER_64_LO(mask) & 0xFF;
                ipv6Data[7 - offset] = 0;
                ipv6Mask[7 - offset] = 0;
                COMPILER_64_SHR(data, 8);
                COMPILER_64_SHR(mask, 8);
            }
            LOG_CLI((BSL_META_U(unit,
                                "%s      "
                     FIELD_IPV6A_FORMAT
                                "/\n%s      "
                                FIELD_IPV6A_FORMAT
                                "\n"),
                     prefix,
                     FIELD_IPV6A_SHOW(ipv6Data),
                     prefix,
                     FIELD_IPV6A_SHOW(ipv6Mask)));
            retVal = 1; /* displayed a qualifier */
            break;
        case bcmFieldQualifySrcIp:
        case bcmFieldQualifyDstIp:
        case bcmFieldQualifyInnerSrcIp:
        case bcmFieldQualifyInnerDstIp:
            /* IPv4 address based qualifiers */
            LOG_CLI((BSL_META_U(unit,
                                "%s      %08X/%08X (%u.%u.%u.%u/%u.%u.%u.%u)\n"),
                     prefix,
                     (uint32)(COMPILER_64_LO(qualData[index].qualData) & 0xFFFFFFFF),
                     (uint32)(COMPILER_64_LO(qualData[index].qualMask) & 0xFFFFFFFF),
                     (COMPILER_64_LO(qualData[index].qualData) >> 24) & 0xFF,
                     (COMPILER_64_LO(qualData[index].qualData) >> 16) & 0xFF,
                     (COMPILER_64_LO(qualData[index].qualData) >> 8) & 0xFF,
                     COMPILER_64_LO(qualData[index].qualData) & 0xFF,
                     (COMPILER_64_LO(qualData[index].qualMask) >> 24) & 0xFF,
                     (COMPILER_64_LO(qualData[index].qualMask) >> 16) & 0xFF,
                     (COMPILER_64_LO(qualData[index].qualMask) >> 8) & 0xFF,
                     COMPILER_64_LO(qualData[index].qualMask) & 0xFF));
            retVal = 1; /* displayed a qualifier */
            break;
        case bcmFieldQualifyHeaderFormat:
            result = _bcm_dpp_field_HeaderFormat_ppd_to_bcm(COMPILER_64_LO(qualData[index].qualData),
                                                            COMPILER_64_LO(qualData[index].qualMask) & 0x3F,
                                                            &hdrFmt);
            if (BCM_E_NONE == result) {
                LOG_CLI((BSL_META_U(unit,
                                    "%s      %s (%d)\n"),
                         prefix,
                         _bcm_dpp_field_header_format_names[hdrFmt],
                         hdrFmt));
            } else {
                LOG_CLI((BSL_META_U(unit,
                                    "%s      PPD PKT_HDR_STK_TYPE %d\n"),
                         prefix,
                         hdrFmt));
            }
            retVal = 1; /* displayed a qualifier */
            break;
        case bcmFieldQualifyColor:
            /*
             *  NOTE: does not translate back to BCM because there is no
             *  set of BCM strings and the PPD type is simple enough to
             *  parse out inline.
             */
            switch (COMPILER_64_LO(qualData[index].qualData) ) {
            case 0:
                LOG_CLI((BSL_META_U(unit,
                                    "%s      GREEN (%d)\n"),
                         prefix,
                         BCM_FIELD_COLOR_GREEN));
                break;
            case 1:
                LOG_CLI((BSL_META_U(unit,
                                    "%s      YELLOW (%d)\n"),
                         prefix,
                         BCM_FIELD_COLOR_YELLOW));
                break;
            case 2:
                LOG_CLI((BSL_META_U(unit,
                                    "%s      RED (%d)\n"),
                         prefix,
                         BCM_FIELD_COLOR_RED));
                break;
            case 3:
                LOG_CLI((BSL_META_U(unit,
                                    "%s      BLACK (%d)\n"),
                         prefix,
                         BCM_FIELD_COLOR_RED));
                break;
            default:
                LOG_CLI((BSL_META_U(unit,
                                    "%s      unknown PPD color %d\n"),
                         prefix,
                         COMPILER_64_LO(qualData[index].qualData)));
            }
            retVal = 1; /* displayed a qualifier */
            break;
        case bcmFieldQualifyIngressStpState:
            switch (COMPILER_64_LO(qualData[index].qualData)) {
            case SOC_PPD_PORT_STP_STATE_BLOCK:
                LOG_CLI((BSL_META_U(unit,
                                    "%s      BLOCK (%d)\n"),
                         prefix,
                         BCM_STG_STP_BLOCK));
                break;
            case SOC_PPD_PORT_STP_STATE_LEARN:
                LOG_CLI((BSL_META_U(unit,
                                    "%s      LEARN (%d)\n"),
                         prefix,
                         BCM_STG_STP_LEARN));
                break;
            case SOC_PPD_PORT_STP_STATE_FORWARD:
                LOG_CLI((BSL_META_U(unit,
                                    "%s      FORWARD (%d)\n"),
                         prefix,
                         BCM_STG_STP_FORWARD));
                break;
            default:
                LOG_CLI((BSL_META_U(unit,
                                    "%s      unknown PPD STP state %d\n"),
                         prefix,
                         COMPILER_64_LO(qualData[index].qualData)));
            }
            retVal = 1; /* displayed a qualifier */
            break;
        default:
            /* general case: display according to nybble count */
            if (bits > 32) {
                LOG_CLI((format,
                         prefix,
                         COMPILER_64_HI(qualData[index].qualData),
                         COMPILER_64_LO(qualData[index].qualData),
                         COMPILER_64_HI(qualData[index].qualMask),
                         COMPILER_64_LO(qualData[index].qualMask)));
            } else {
                LOG_CLI((format,
                         prefix,
                         COMPILER_64_LO(qualData[index].qualData),
                         COMPILER_64_LO(qualData[index].qualMask)));
            }
            retVal = 1; /* displayed a qualifier */
        } /* switch (entryData[index].qual[index].type) */
    }
#if _BCM_DPP_FIELD_DUMP_INTERMIX_PPD || _BCM_DPP_FIELD_DUMP_VERIFY_PPD
    if (1 == retVal) {
#if _BCM_DPP_FIELD_DUMP_VERIFY_PPD
        if (qualHwData) {
            for (offset = 0;
                 offset < SOC_PPD_FP_NOF_QUALS_PER_DB_MAX;
                 offset++) {
                if (qualHwData[offset].type == qualData[index].hwType) {
                    /* found the hardware qual matching this one */
                    break;
                }
            }
#if _BCM_DPP_FIELD_DUMP_INTERMIX_PPD
            LOG_CLI((BSL_META_U(unit,
                                "%s      %08X%08X/%08X%08X (expected)\n"),
                     prefix,
                     COMPILER_64_HI(qualData[index].qualData),
                     COMPILER_64_LO(qualData[index].qualData),
                     COMPILER_64_HI(qualData[index].qualMask),
                     COMPILER_64_LO(qualData[index].qualMask)));
#endif /* _BCM_DPP_FIELD_DUMP_INTERMIX_PPD */
            if (offset < SOC_PPD_FP_NOF_QUALS_PER_DB_MAX) {
#if _BCM_DPP_FIELD_DUMP_INTERMIX_PPD
                LOG_CLI((BSL_META_U(unit,
                                    "%s      %08X%08X/%08X%08X (actual)\n"),
                         prefix,
                         qualHwData[offset].val.arr[1],
                         qualHwData[offset].val.arr[0],
                         qualHwData[offset].is_valid.arr[1],
                         qualHwData[offset].is_valid.arr[0]));
#endif /* _BCM_DPP_FIELD_DUMP_INTERMIX_PPD */
                if ((COMPILER_64_HI(qualData[index].qualData) !=
                     qualHwData[offset].val.arr[1]) ||
                    (COMPILER_64_LO(qualData[index].qualData) !=
                     qualHwData[offset].val.arr[0]) ||
                    (COMPILER_64_HI(qualData[index].qualMask) !=
                     qualHwData[offset].is_valid.arr[1]) ||
                    (COMPILER_64_LO(qualData[index].qualMask) !=
                     qualHwData[offset].is_valid.arr[0])) {
                    LOG_CLI((BSL_META_U(unit,
                                        "%s      (BCM and PPD data mismatch)\n"),
                             prefix));
#if _BCM_DPP_FIELD_DUMP_VERIFY_ERROR
                    retVal = BCM_E_INTERNAL;
#endif /* _BCM_DPP_FIELD_DUMP_VERIFY_ERROR */
                }
            } else { /* if (offset < SOC_PPD_FP_NOF_QUALS_PER_DB_MAX) */
                LOG_CLI((BSL_META_U(unit,
                                    "%s      (unable to find hardware value)\n"),
                         prefix));
#if _BCM_DPP_FIELD_DUMP_VERIFY_ERROR
                retVal = BCM_E_INTERNAL;
#endif /* _BCM_DPP_FIELD_DUMP_VERIFY_ERROR */
            } /* if (offset < SOC_PPD_FP_NOF_QUALS_PER_DB_MAX) */
        } /* if (qualHwData) */
#else /* _BCM_DPP_FIELD_DUMP_VERIFY_PPD */
        LOG_CLI((BSL_META_U(unit,
                            "%s      %08X%08X/%08X%08X\n"),
                 prefix,
                 COMPILER_64_HI(qualData[index].qualData),
                 COMPILER_64_LO(qualData[index].qualData),
                 COMPILER_64_HI(qualData[index].qualMask),
                 COMPILER_64_LO(qualData[index].qualMask)));
#endif /* _BCM_DPP_FIELD_DUMP_VERIFY_PPD */
    }
#endif /* _BCM_DPP_FIELD_DUMP_INTERMIX_PPD || _BCM_DPP_FIELD_DUMP_VERIFY_PPD */
    return retVal;
}
#endif /* def BROADCOM_DEBUG */

#ifdef BROADCOM_DEBUG
int
_bcm_dpp_field_group_dump(_bcm_dpp_field_info_t *unitData,
                          _bcm_dpp_field_grp_idx_t group,
                          const char *prefix,
                          int entries)
{
    _bcm_dpp_field_group_t *groupData = &(unitData->groupD[group]);
    _bcm_dpp_field_ent_idx_t entry;
    unsigned int entryBias;
    unsigned int count;
    int result = BCM_E_NONE;
    char *newPrefix = NULL;
    _bcm_dpp_field_entry_type_t entryType;    
 
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;

    LOG_CLI((BSL_META_U(unit,
                        "%sGroup "
             _BCM_DPP_FIELD_GROUP_IDX_FORMAT
                        "\n"),
             prefix,
             group));
    LOG_CLI((BSL_META_U(unit,
                        "%s  Flags       = %08X\n"), prefix, groupData->groupFlags));
#if _BCM_DPP_FIELD_DUMP_SYM_FLAGS
    LOG_CLI((BSL_META_U(unit,
                        "%s    %s %s %s %s %s %s %s %s\n"),
             prefix,
             (groupData->groupFlags & _BCM_DPP_FIELD_GROUP_IN_USE)?"+USED":"-used",
             (groupData->groupFlags & _BCM_DPP_FIELD_GROUP_IN_HW)?"+IN_HW":"-in_hw",
             (groupData->groupFlags & _BCM_DPP_FIELD_GROUP_PHASE)?"PH1":"PH0",
             (groupData->groupFlags & _BCM_DPP_FIELD_GROUP_CHANGED)?"+CHG":"-chg",
             (groupData->groupFlags & _BCM_DPP_FIELD_GROUP_CHG_ENT)?"+CHGENT":"-chgent",
             (groupData->groupFlags & _BCM_DPP_FIELD_GROUP_ADD_ENT)?"+ADDENT":"-addent",
             (groupData->groupFlags & _BCM_DPP_FIELD_GROUP_INGRESS)?"IGR":(groupData->groupFlags & _BCM_DPP_FIELD_GROUP_EGRESS)?"EGR":"?d?",
             (groupData->groupFlags & _BCM_DPP_FIELD_GROUP_AUTO_SZ)?"+ASZ":"-asz"));
    LOG_CLI((BSL_META_U(unit,
                        "%s    %s %s %s %s %s %s %s %s %s\n"),
             prefix,
             (groupData->groupFlags & _BCM_DPP_FIELD_GROUP_L2)?"+L2":"-l2",
             (groupData->groupFlags & _BCM_DPP_FIELD_GROUP_IPV4)?"+IP4":"-ip4",
             (groupData->groupFlags & _BCM_DPP_FIELD_GROUP_IPV6)?"+IP6":"-ip6",
             (groupData->groupFlags & _BCM_DPP_FIELD_GROUP_MPLS)?"+MPLS":"-mpls",
             (groupData->groupFlags & _BCM_DPP_FIELD_GROUP_PRESEL)?"-ips":"+IPS",
             (groupData->groupFlags & _BCM_DPP_FIELD_GROUP_CASCADE)?"+CAS":"-cas",
             (groupData->groupFlags & _BCM_DPP_FIELD_GROUP_SMALL)?"+SMALL":"-small",
             (groupData->groupFlags & _BCM_DPP_FIELD_GROUP_LOOSE)?"+LOOSE":"-loose",
             (groupData->groupFlags & _BCM_DPP_FIELD_GROUP_SPARSE_PRIO)?"+SPARSE":"-sparse"));
#endif /* _BCM_PETRA_FIELD_DUMP_SYM_FLAGS */
    if (groupData->groupFlags & _BCM_DPP_FIELD_GROUP_IN_USE) {
        entryType = unitData->stageD[groupData->stage].modeBits[groupData->grpMode]->entryType;
        switch (entryType) {
        case _bcmDppFieldEntryTypeDirExt:
            entryBias = _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt);
            break;
        case _bcmDppFieldEntryTypeExternalTcam:
            entryBias = _BCM_DPP_FIELD_ENT_BIAS(unit, ExternalTcam);
            break;
        case _bcmDppFieldEntryTypeInternalTcam:
        default:
            /* default is TCAM */
            entryBias = _BCM_DPP_FIELD_ENT_BIAS(unit, InternalTcam);
        }
        LOG_CLI((BSL_META_U(unit,
                            "%s  Stage       = %8d (types %08X)\n"),
                 prefix,
                 groupData->stage,
                 groupData->groupTypes));
        if (groupData->groupFlags & _BCM_DPP_FIELD_GROUP_AUTO_SZ) {
            LOG_CLI((BSL_META_U(unit,
                                "%s  Mode        = Auto (%s)\n"),
                     prefix,
                     _bcm_dpp_field_group_mode_name[groupData->grpMode]));
        } else {
            LOG_CLI((BSL_META_U(unit,
                                "%s  Mode        = %s\n"),
                     prefix,
                     _bcm_dpp_field_group_mode_name[groupData->grpMode]));
        }
        LOG_CLI((BSL_META_U(unit,
                            "%s  Priority    = %8d\n"),
                 prefix,
                 groupData->priority));
        if (groupData->groupFlags & _BCM_DPP_FIELD_GROUP_IN_HW) {
            LOG_CLI((BSL_META_U(unit,
                                "%s  HW group ID = %08X\n"),
                     prefix,
                     groupData->hwHandle));
        }
        if (unitData->groupLimit > groupData->cascadePair) {
            LOG_CLI((BSL_META_U(unit,
                                "%s  CascadePair = "
                     _BCM_DPP_FIELD_GROUP_IDX_FORMAT
                                "\n"),
                     prefix,
                     groupData->cascadePair));
        }
        LOG_CLI((BSL_META_U(unit,
                            "%s  Prev/Next   = "
                 _BCM_DPP_FIELD_GROUP_IDX_FORMAT
                            " / "
                            _BCM_DPP_FIELD_GROUP_IDX_FORMAT
                            "\n"),
                 prefix,
                 groupData->groupPrev,
                 groupData->groupNext));
        LOG_CLI((BSL_META_U(unit,
                            "%s  Entry count = "
                 _BCM_DPP_FIELD_ENTRY_IDX_FORMAT
                            "\n"),
                 prefix,
                 groupData->entryCount));
        LOG_CLI((BSL_META_U(unit,
                            "%s  First/Last  = "
                 _BCM_DPP_FIELD_ENTRY_IDX_FORMAT
                            " / "
                            _BCM_DPP_FIELD_ENTRY_IDX_FORMAT
                            "\n"),
                 prefix,
                 groupData->entryHead + entryBias,
                 groupData->entryTail + entryBias));
        BCMDNX_ALLOC(newPrefix, sal_strlen(prefix) + 6,
                              "field group dump prefix");
        if (newPrefix) {
            sal_snprintf(newPrefix, sal_strlen(prefix) + 5, "%s    ", prefix);
        } else {
            result = BCM_E_MEMORY;
        }
        if (BCM_E_NONE == result) {
            if (SOC_PPD_NOF_FP_PREDEFINED_ACL_KEYS != groupData->predefKey) {
                LOG_CLI((BSL_META_U(unit,
                                    "%s  PredefKey   = %s\n"),
                         prefix,
                         SOC_PPD_FP_PREDEFINED_ACL_KEY_to_string(groupData->predefKey)));
            } else {
                LOG_CLI((BSL_META_U(unit,
                                    "%s  PredefKey   = (none)\n"), prefix));
            }
            LOG_CLI((BSL_META_U(unit,
                                "%s  Group QSET:\n"), prefix));
            _bcm_dpp_field_qset_dump(groupData->qset, newPrefix);
            LOG_CLI((BSL_META_U(unit,
                                "%s  Group ASET:\n"), prefix));
            _bcm_dpp_field_aset_dump(groupData->aset, newPrefix);
            LOG_CLI((BSL_META_U(unit,
                                "%s  Group preselectors (%s):\n"),
                     prefix,
                     (groupData->groupFlags & _BCM_DPP_FIELD_GROUP_PRESEL)?"configured":"implied"));
            LOG_CLI((BSL_META_U(unit,
                                "%s    "), prefix));
            for (entry = 0, count = 0;
                 entry < unitData->preselLimit;
                 entry++) {
                if (BCM_FIELD_PRESEL_TEST(groupData->preselSet, entry)) {
                    if (count) {
                        LOG_CLI((BSL_META_U(unit,
                                            ", %d"), entry));
                    } else {
                        LOG_CLI((BSL_META_U(unit,
                                            "%d"), entry));
                    }
                    count++;
                }
            } /* for (all possible preselector IDs) */
            if (!count) {
                LOG_CLI((BSL_META_U(unit,
                                    "(none)")));
            }
            LOG_CLI((BSL_META_U(unit,
                                "\n%s  Group preselectors (%s):\n"),
                     prefix,
                     "hardware"));
            LOG_CLI((BSL_META_U(unit,
                                "%s    "), prefix));
            for (entry = 0, count = 0;
                 entry < unitData->preselLimit;
                 entry++) {
                if (BCM_FIELD_PRESEL_TEST(groupData->preselHw, entry)) {
                    if (count) {
                        LOG_CLI((BSL_META_U(unit,
                                            ", %d"), entry));
                    } else {
                        LOG_CLI((BSL_META_U(unit,
                                            "%d"), entry));
                    }
                    count++;
                }
            } /* for (all possible preselector IDs) */
            if (!count) {
                LOG_CLI((BSL_META_U(unit,
                                    "(none)")));
            }
            LOG_CLI((BSL_META_U(unit,
                                "\n")));
        } /* if (BCM_E_NONE == result) */
        if ((BCM_E_NONE == result) && entries) {
            LOG_CLI((BSL_META_U(unit,
                                "%s  Entries:\n"), prefix));
            switch (entryType) {
            case _bcmDppFieldEntryTypeDirExt:
                for (entry = groupData->entryHead, count = 0;
                     (BCM_E_NONE == result) && (entry < unitData->entryDeLimit);
                     entry = unitData->entryDe[entry].entryCmn.entryNext) {
                    result = _bcm_dpp_field_dir_ext_entry_dump(unitData,
                                                               entry,
                                                               newPrefix);
                    count++;
                }
                break;
            default:
                /* default is TCAM */
                for (entry = groupData->entryHead, count = 0;
                     (BCM_E_NONE == result) && (entry < _BCM_DPP_FIELD_TCAM_ENTRY_LIMIT(entryType));
                     entry = _BCM_DPP_FIELD_TCAM_ENTRY_NEXT(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry)) {
                    result = _bcm_dpp_field_tcam_entry_dump(unitData,
                                                            _BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType),
                                                            entry,
                                                            newPrefix);
                    count++;
                }
            }
            if ((BCM_E_NONE == result) && (0 == count)) {
                LOG_CLI((BSL_META_U(unit,
                                    "%s(no entries in this group)\n"), newPrefix));
            }
        } /* if ((BCM_E_NONE == result) && entries) */
        if (BCM_E_NONE != result) {
            LOG_CLI((BSL_META_U(unit,
                                "%s  (error %d (%s) displaying group)\n"),
                     prefix,
                     result,
                     _SHR_ERRMSG(result)));
        }
    } else {
        LOG_CLI((BSL_META_U(unit,
                            "%s  (group is not in use)\n"), prefix));
        result = BCM_E_NOT_FOUND;
    }
    if (newPrefix) {
       BCM_FREE(newPrefix);
    }
    BCMDNX_IF_ERR_EXIT(result);
exit:
    BCMDNX_FUNC_RETURN;
}
#endif /* def BROADCOM_DEBUG */

#ifdef BROADCOM_DEBUG
/*
 *   Function
 *      _bcm_dpp_field_stage_dump
 *   Purpose
 *      Dump the state of a specific field stage
 *   Parameters
 *      (in) unitData = unit information
 *      (in) stage = stage to be dumped
 *      (in) prefix = string to prefix to each line
 *      (in) groups = TRUE to include groups, FALSE to exclude groups
 *      (in) entries = TRUE to include entries, FALSE to exclude entries
 *   Returns
 *      int (implied cast from bcm_error_t)
 *                    BCM_E_NONE if sccessful
 *                    BCM_E_* appropriately otherwise
 *   Notes
 *      If groups is FALSE, the value of entries does not matter: entries will
 *      not be displayed if groups are not displayed.
 */
STATIC int
_bcm_dpp_field_stage_dump(_bcm_dpp_field_info_t *unitData,
                          int stage,
                          const char *prefix,
                          int groups,
                          int entries)
{
    _bcm_dpp_field_stage_t *stageData;
    _bcm_dpp_field_grp_idx_t group;
    unsigned int count;
    char *newPrefix = NULL;
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;

    if ((0 > stage) || (stage > unitData->devInfo->stages)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d does not support stage %d"),
                          unit,
                          stage));
    }
    stageData = &(unitData->stageD[stage]);
    LOG_CLI((BSL_META_U(unit,
                        "%sStage "
             _BCM_DPP_FIELD_STAGE_IDX_FORMAT
                        " (%s) (%u)\n"),
             prefix,
             stage,
             stageData->devInfo->stageName,
             stageData->devInfo->hwStageId));
    BCMDNX_ALLOC(newPrefix,
              sal_strlen(prefix) + 6,
              "field stage dump prefix");
    if (newPrefix) {
        sal_snprintf(newPrefix, sal_strlen(prefix) + 5, "%s    ", prefix);
    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d unable to allocate %u"
                                           " bytes for prefix buffer"),
                          unit,
                          (uint32)sal_strlen(prefix) + 6));
    }
    LOG_CLI((BSL_META_U(unit,
                        "%s  Supported %s qualifiers:\n"),
             prefix,
             stageData->devInfo->stageName));
    _bcm_dpp_field_qset_dump(stageData->stgQset, newPrefix);
    LOG_CLI((BSL_META_U(unit,
                        "%s  Supported %s actions:\n"),
             prefix,
             stageData->devInfo->stageName));
    _bcm_dpp_field_aset_dump(stageData->stgAset, newPrefix);
    LOG_CLI((BSL_META_U(unit,
                        "%s  Supported %s group modes and bit widths:\n"),
             prefix,
             stageData->devInfo->stageName));
    for (group = 0, count = 0; group < bcmFieldGroupModeCount; group++) {
        if (stageData->modeBits[group] && (bcmFieldGroupModeAuto != group)) {
            LOG_CLI((BSL_META_U(unit,
                                "%s%-18s (%d): %3ub in, %3ub qual, %u elem/ent, %s\n"),
                     newPrefix,
                     _bcm_dpp_field_group_mode_name[group],
                     group,
                     stageData->modeBits[group]->length,
                     stageData->modeBits[group]->qualLength,
                     stageData->modeBits[group]->entryCount,
                     _bcm_dpp_field_entry_type_names[stageData->modeBits[group]->entryType]));
            count++;
        }
    }
    if (!count) {
        LOG_CLI((BSL_META_U(unit,
                            "%s(no supported group modes)\n"), newPrefix));
    }
    LOG_CLI((BSL_META_U(unit,
                        "%s  Qual in grp = %8d\n"),
             prefix,
             stageData->devInfo->entryMaxQuals));
    LOG_CLI((BSL_META_U(unit,
                        "%s  Act in grp  = %8d\n"),
             prefix,
             stageData->devInfo->entryMaxActs));
    if (stageData->devInfo->maxEntriesDe) {
        LOG_CLI((BSL_META_U(unit,
                            "%s  Qual DE lim = %8d\n"),
                 prefix,
                 stageData->devInfo->entryDeMaxQuals));
    }
    LOG_CLI((BSL_META_U(unit,
                        "%s  Groups      = "
             _BCM_DPP_FIELD_GROUP_IDX_FORMAT
                        " / "
                        _BCM_DPP_FIELD_GROUP_IDX_FORMAT
                        "\n"),
             prefix,
             unitData->stageD[stageData->groupSh1].groupCount,
             unitData->stageD[stageData->groupSh1].hwGroupLimit));
    LOG_CLI((BSL_META_U(unit,
                        "%s    first/last= "
             _BCM_DPP_FIELD_GROUP_IDX_FORMAT
                        " / "
                        _BCM_DPP_FIELD_GROUP_IDX_FORMAT
                        "\n"),
             prefix,
             stageData->groupHead,
             stageData->groupTail));
    LOG_CLI((BSL_META_U(unit,
                        "%s  Entries     = "
             _BCM_DPP_FIELD_ENTRY_IDX_FORMAT
                        " / "
                        _BCM_DPP_FIELD_ENTRY_IDX_FORMAT
                        "\n"),
             prefix,
             unitData->stageD[stageData->entryTcSh1].entryCount,
             unitData->stageD[stageData->entryTcSh1].hwEntryLimit));
    LOG_CLI((BSL_META_U(unit,
                        "%s    available = "
             _BCM_DPP_FIELD_ENTRY_IDX_FORMAT
                        "\n"),
             prefix,
             unitData->stageD[stageData->entryTcSh1].hwEntryLimit -
             unitData->stageD[stageData->entryTcSh1].entryElems));
    if (unitData->stageD[stageData->entryDeSh1].hwEntryDeLimit) {
        LOG_CLI((BSL_META_U(unit,
                            "%s  DirExt Ents = "
                 _BCM_DPP_FIELD_ENTRY_IDX_FORMAT
                            " / "
                            _BCM_DPP_FIELD_ENTRY_IDX_FORMAT
                            "\n"),
                 prefix,
                 unitData->stageD[stageData->entryDeSh1].entryDeCount,
                 unitData->stageD[stageData->entryDeSh1].hwEntryDeLimit));
    }
    if (groups) {
        LOG_CLI((BSL_META_U(unit,
                            "%s  Stage %s group information:\n"), prefix, stageData->devInfo->stageName));
        for (group = stageData->groupHead, count = 0;
             group < unitData->groupLimit;
             group = unitData->groupD[group].groupNext) {
            BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_group_dump(unitData,
                                                              group,
                                                              newPrefix,
                                                              entries));
            count++;
        }
        if (!count) {
            LOG_CLI((BSL_META_U(unit,
                                "%s(no groups in this stage)\n"), newPrefix));
        }
    } /* if (groups) */

exit:
    BCM_FREE(newPrefix);
    BCMDNX_FUNC_RETURN;
}
#endif /* def BROADCOM_DEBUG */

#ifdef BROADCOM_DEBUG
int
_bcm_dpp_field_unit_dump(_bcm_dpp_field_info_t *unitData,
                         const char *prefix,
                         int stages,
                         int groups,
                         int entries)
{
    _bcm_dpp_field_stage_idx_t stage;
    unsigned int count;
    int result = BCM_E_NONE;
    char *newPrefix = NULL;
#if _BCM_DPP_FIELD_DUMP_INCLUDE_PPD
    uint32 soc_sandResult;
    uint32 soc_sandIndex;
    uint32 soc_sandOffset;
    uint8 okay;
    SOC_PPD_FP_PFG_INFO *pfgInfo = NULL;
    SOC_PPD_FP_DATABASE_INFO dbInfo;
    SOC_PPD_FP_ENTRY_INFO *entInfoTc = NULL;
    SOC_PPD_FP_DIR_EXTR_ENTRY_INFO *entInfoDe = NULL;
    SOC_PPD_FP_CONTROL_INDEX control_ndx;
    SOC_PPD_FP_CONTROL_INFO control_info;
    unsigned int i;
    unsigned int j;
    unsigned int k;
    unsigned int l;
    const char *str;
#endif /* _BCM_PETRA_FIELD_DUMP_INCLUDE_PPD */
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;

    /* dump the BCM layer state */
    LOG_CLI((BSL_META_U(unit,
                        "%sBCM unit %d field support: %08X (%u bytes)\n"),
             prefix,
             unit,
             PTR_TO_INT(unitData),
             unitData->totalSize));
    LOG_CLI((BSL_META_U(unit,
                        "%s  Lock        = %08X\n"),
             prefix,
             PTR_TO_INT(unitData->unitLock)));
    LOG_CLI((BSL_META_U(unit,
                        "%s  Flags       = %08X\n"),
             prefix,
             unitData->unitFlags));
#if _BCM_DPP_FIELD_DUMP_SYM_FLAGS
#ifdef BCM_WARM_BOOT_SUPPORT
    LOG_CLI((BSL_META_U(unit,
                        "%s    %s %s %s %s %s\n"),
             prefix,
             (unitData->unitFlags & _BCM_DPP_FIELD_UNIT_WB_VERSION_FAIL)?"+WBVF":"-wbvf",
             (unitData->unitFlags & _BCM_DPP_FIELD_UNIT_WB_FORMAT_FAIL)?"+WBFF":"-wbff",
             (unitData->unitFlags & _BCM_DPP_FIELD_UNIT_WB_COMMIT_FAIL)?"+WBCF":"-wbcf",
             (unitData->unitFlags & _BCM_DPP_FIELD_UNIT_WB_RESTORE_FAIL)?"+WBRF":"-wbrf",
             (unitData->unitFlags & _BCM_DPP_FIELD_UNIT_STAT_CACHED)?"+STC":"-stc"));
#else /* def BCM_WARM_BOOT_SUPPORT */
    LOG_CLI((BSL_META_U(unit,
                        "%s    %s\n"),
             prefix,
             (unitData->unitFlags & _BCM_DPP_FIELD_UNIT_STAT_CACHED)?"+STC":"-stc"));
#endif /* def BCM_WARM_BOOT_SUPPORT */
#endif /* _BCM_PETRA_FIELD_DUMP_SYM_FLAGS */
    LOG_CLI((BSL_META_U(unit,
                        "%s  Groups      = "
             _BCM_DPP_FIELD_GROUP_IDX_FORMAT
                        " / "
                        _BCM_DPP_FIELD_GROUP_IDX_FORMAT
                        "\n"),
             prefix,
             unitData->groupCount,
             unitData->groupLimit));
    LOG_CLI((BSL_META_U(unit,
                        "%s    next free = "
             _BCM_DPP_FIELD_GROUP_IDX_FORMAT
                        "\n"),
             prefix,
             unitData->groupFree));
    LOG_CLI((BSL_META_U(unit,
                        "%s  Entries     = "
             _BCM_DPP_FIELD_ENTRY_IDX_FORMAT
                        " / "
                        _BCM_DPP_FIELD_ENTRY_IDX_FORMAT
                        "\n"),
             prefix,
             unitData->entryIntTcCount,
             unitData->entryTcLimit));
    LOG_CLI((BSL_META_U(unit,
                        "%s    next free = "
             _BCM_DPP_FIELD_ENTRY_IDX_FORMAT
                        "\n"),
             prefix,
             unitData->entryIntTcFree));
    if (unitData->entryDeLimit) {
        LOG_CLI((BSL_META_U(unit,
                            "%s  DirExt Ents = "
                 _BCM_DPP_FIELD_ENTRY_IDX_FORMAT
                            " / "
                            _BCM_DPP_FIELD_ENTRY_IDX_FORMAT
                            "\n"),
                 prefix,
                 unitData->entryDeCount,
                 unitData->entryDeLimit));
    }
    LOG_CLI((BSL_META_U(unit,
                        "%s  ExtTcam Ents = "
             _BCM_DPP_FIELD_ENTRY_IDX_FORMAT
                        " / "
                        _BCM_DPP_FIELD_ENTRY_IDX_FORMAT
                        "\n"),
             prefix,
             unitData->entryExtTcCount,
             unitData->entryExtTcLimit));
    LOG_CLI((BSL_META_U(unit,
                        "%s    next free = "
             _BCM_DPP_FIELD_ENTRY_IDX_FORMAT
                        "\n"),
             prefix,
             unitData->entryExtTcFree));
    LOG_CLI((BSL_META_U(unit,
                        "%s  Cascade Len = %8d bits\n"),
             prefix,
             unitData->cascadedKeyLen));
    LOG_CLI((BSL_META_U(unit,
                        "%s    casc grps = "
             _BCM_DPP_FIELD_GROUP_IDX_FORMAT
                        "\n"),
             prefix,
             unitData->groupCascaded));
    BCMDNX_ALLOC(newPrefix, sal_strlen(prefix) + 6,
                          "field unit dump prefix");
    if (newPrefix) {
        sal_snprintf(newPrefix, sal_strlen(prefix) + 5, "%s    ", prefix);
    } else {
        result = BCM_E_MEMORY;
    }
    if (BCM_E_NONE == result) {
        LOG_CLI((BSL_META_U(unit,
                            "%s  Preselectors:\n"), prefix));
        result = _bcm_dpp_field_presel_dump(unitData, newPrefix);
    }
    if (BCM_E_NONE == result) {
        LOG_CLI((BSL_META_U(unit,
                            "%s  Ranges:\n"), prefix));
        result = _bcm_dpp_field_range_dump(unitData, newPrefix);
    }
    if (BCM_E_NONE == result) {
        LOG_CLI((BSL_META_U(unit,
                            "%s  Data Field Qualifiers:\n"), prefix));
        result = _bcm_petra_field_data_qualifier_dump(unitData, newPrefix);
    }
    if ((BCM_E_NONE == result) && stages) {
        LOG_CLI((BSL_META_U(unit,
                            "%s  Stages:\n"), prefix));
        for (stage = 0, count = 0;
             (BCM_E_NONE == result) && (stage < unitData->devInfo->stages);
             stage++) {
            result = _bcm_dpp_field_stage_dump(unitData,
                                               stage,
                                               newPrefix,
                                               groups,
                                               entries);
            if (BCM_E_NONE == result) {
                count++;
            }
        }
        if ((BCM_E_NONE == result) && (0 == count)) {
            LOG_CLI((BSL_META_U(unit,
                                "%s(no groups in this unit)\n"), newPrefix));
        }
    } /* if ((BCM_E_NONE == result) && groups) */
#if _BCM_DPP_FIELD_DUMP_INCLUDE_PPD
    /* dump the PPD layer state */
    LOG_CLI((BSL_META_U(unit,
                        "%s  PPD state:\n"), prefix));
    LOG_CLI((BSL_META_U(unit,
                        "%s    PFG information:\n"), prefix));
    BCMDNX_ALLOC(pfgInfo, sizeof(SOC_PPD_FP_PFG_INFO), "_bcm_dpp_field_unit_dump.pfgInfo");
    if (pfgInfo == NULL) {        
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG_NO_UNIT("failed to allocate memory")));
    }
    for (soc_sandIndex = 0;
         soc_sandIndex < unitData->preselLimit;
         soc_sandIndex++) {
        SOC_PPD_FP_PFG_INFO_clear(pfgInfo);
        soc_sandResult = soc_ppd_fp_packet_format_group_get(unitData->unitHandle,
                                                            soc_sandIndex,
                                                            pfgInfo);
        result = handle_sand_result(soc_sandResult);
        if (BCM_E_NONE == result) {
#ifdef BCM_PETRAB_SUPPORT
            if (SOC_IS_PETRAB(unit)){
                LOG_CLI((BSL_META_U(unit,
                                    "%s      PFG %2d:\n"),
                         prefix,
                         soc_sandIndex));
                LOG_CLI((BSL_META_U(unit,
                                    "%s        hdr_bmp=%08X tag_bmp=%08X"
                         " port_bmp=%08X%08X\n"),
                         prefix,
                         pfgInfo->hdr_format_bmp,
                         pfgInfo->vlan_tag_structure_bmp,
                         pfgInfo->pp_ports_bmp.arr[0],
                         pfgInfo->pp_ports_bmp.arr[1]));
            } /* if (SOC_IS_PETRAB(unit))*/
#endif /* def BCM_PETRAB_SUPPORT */
#ifdef BCM_ARAD_SUPPORT
            if (SOC_IS_ARAD(unit)){
                if (pfgInfo->is_array_qualifier) {
                    /* Arad uses this to indicate PFG is present */
                    LOG_CLI((BSL_META_U(unit,
                                        "%s      PFG %2d: stage=%s (%d)\n"),
                             prefix,
                             soc_sandIndex,
                             SOC_PPC_FP_DATABASE_STAGE_to_string(pfgInfo->stage),
                             pfgInfo->stage));
                    for (i = 0, j = 0;
                         i < SOC_PPC_FP_NOF_QUALS_PER_PFG_MAX;
                         i++) {
                        if (_BCM_DPP_FIELD_PPD_QUAL_VALID(pfgInfo->qual_vals[i].type)) {
                            LOG_CLI((BSL_META_U(unit,
                                                "%s        %s (%d)\n"
                                     "%s          %08X%08X/%08X%08X\n"),
                                     prefix,
                                     SOC_PPD_FP_QUAL_TYPE_to_string(pfgInfo->qual_vals[i].type),
                                     pfgInfo->qual_vals[i].type,
                                     prefix,
                                     pfgInfo->qual_vals[i].val.arr[1],
                                     pfgInfo->qual_vals[i].val.arr[0],
                                     pfgInfo->qual_vals[i].is_valid.arr[1],
                                     pfgInfo->qual_vals[i].is_valid.arr[0]));
                            j++;
                        } /* if (qualifier type is valid) */
                    } /* for (all possible qualifiers) */
                    if (0 == j) {
                        LOG_CLI((BSL_META_U(unit,
                                            "%s              (none)\n"),
                                 prefix));
                    }
                } else { /* if (pfgInfo->is_array_qualifier) */
                    /* Arad uses this to indicate PFG not present */
                    LOG_CLI((BSL_META_U(unit,
                                        "%s      PFG %2d: (not in use)\n"),
                             prefix,
                             soc_sandIndex));
                } /* if (pfgInfo->is_array_qualifier) */
            } /* if (SOC_IS_ARAD(unit))*/
#endif /* def BCM_ARAD_SUPPORT */
        } else { /* if (BCM_E_NONE == result) */
            LOG_CLI((BSL_META_U(unit,
                                "%s      PFG %d: unable to read: %d (%s)\n"),
                     prefix,
                     soc_sandIndex,
                     result,
                     _SHR_ERRMSG(result)));
        } /* if (BCM_E_NONE == result) */
    } /* for (soc_sandIndex = 0; soc_sandIndex < 5; soc_sandIndex++) */
    LOG_CLI((BSL_META_U(unit,
                        "%s    L4OPS (TCP/UDP range) information:\n"), prefix));
    for (soc_sandIndex = 0;
         soc_sandIndex < _BCM_DPP_NOF_L4_PORT_RANGES;
         soc_sandIndex++) {
        SOC_PPD_FP_CONTROL_INDEX_clear(&control_ndx);
        SOC_PPD_FP_CONTROL_INFO_clear(&control_info);
        control_ndx.type = SOC_PPD_FP_CONTROL_TYPE_L4OPS_RANGE;
        control_ndx.val_ndx = soc_sandIndex;
        soc_sandResult = soc_ppd_fp_control_get(unitData->unitHandle,
                                                SOC_CORE_INVALID,
                                                &control_ndx,
                                                &control_info);
        result = handle_sand_result(soc_sandResult);
        if (BCM_E_NONE == result) {
            LOG_CLI((BSL_META_U(unit,
                                "%s      L4OPS %2d: Src = %5d through %5d, Dst"
                     " = %5d through %5d\n"),
                     prefix,
                     soc_sandIndex,
                     control_info.val[0],
                     control_info.val[1],
                     control_info.val[2],
                     control_info.val[3]));
        } else {
            LOG_CLI((BSL_META_U(unit,
                                "%s      L4OPS %2d: unable to read:"
                     " %08X -> %d (%s)\n"),
                     prefix,
                     soc_sandIndex,
                     soc_sandResult,
                     result,
                     _SHR_ERRMSG(result)));
        }
    } /* for (all L4OPS ranges) */
    LOG_CLI((BSL_META_U(unit,
                        "%s    Packet length range information:\n"), prefix));
    for (soc_sandIndex = 0;
         soc_sandIndex < _BCM_DPP_NOF_PKT_LEN_RANGES;
         soc_sandIndex++) {
        SOC_PPD_FP_CONTROL_INDEX_clear(&control_ndx);
        SOC_PPD_FP_CONTROL_INFO_clear(&control_info);
        control_ndx.type = SOC_PPD_FP_CONTROL_TYPE_PACKET_SIZE_RANGE;
        control_ndx.val_ndx = soc_sandIndex;
        soc_sandResult = soc_ppd_fp_control_get(unitData->unitHandle,
                                                SOC_CORE_INVALID,  /*Not used*/
                                                &control_ndx,
                                                &control_info);
        result = handle_sand_result(soc_sandResult);
        if (BCM_E_NONE == result) {
            LOG_CLI((BSL_META_U(unit,
                                "%s      Packet Length %1d: Min = %3d, Max = %3d\n"),
                     prefix,
                     soc_sandIndex,
                     control_info.val[0],
                     control_info.val[1]));
        } else {
            LOG_CLI((BSL_META_U(unit,
                                "%s      Packet Length %1d: unable to read:"
                     " %08X -> %d (%s)\n"),
                     prefix,
                     soc_sandIndex,
                     soc_sandResult,
                     result,
                     _SHR_ERRMSG(result)));
        }
    } /* for (all packet length ranges) */
    LOG_CLI((BSL_META_U(unit,
                        "%s    User defined fields information:\n"), prefix));
    for (soc_sandIndex = 0;
         soc_sandIndex < unitData->dqLimit;
         soc_sandIndex++) {
        SOC_PPD_FP_CONTROL_INDEX_clear(&control_ndx);
        SOC_PPD_FP_CONTROL_INFO_clear(&control_info);
        control_ndx.type = SOC_PPD_FP_CONTROL_TYPE_HDR_USER_DEF;
        control_ndx.val_ndx = soc_sandIndex;
        soc_sandResult = soc_ppd_fp_control_get(unitData->unitHandle,
                                                SOC_CORE_INVALID,  /*Not used*/
                                                &control_ndx,
                                                &control_info);
        result = handle_sand_result(soc_sandResult);
        if (BCM_E_NONE == result) {
            LOG_CLI((BSL_META_U(unit,
                                "%s      Data field %2d: Hdr = %1d, Ofs = %3d,"
                     " Len = %2d\n"),
                     prefix,
                     soc_sandIndex,
                     control_info.val[0],
                     control_info.val[1],
                     control_info.val[2]));
        } else {
            LOG_CLI((BSL_META_U(unit,
                                "%s      Data field %1d: unable to read: %08X ->"
                     " %d (%s)\n"),
                     prefix,
                     soc_sandIndex,
                     soc_sandResult,
                     result,
                     _SHR_ERRMSG(result)));
        }
    } /* for (all data fields) */
    SOC_PPD_FP_CONTROL_INDEX_clear(&control_ndx);
    SOC_PPD_FP_CONTROL_INFO_clear(&control_info);
    control_ndx.type = SOC_PPD_FP_CONTROL_TYPE_KEY_CHANGE_SIZE;
    control_ndx.val_ndx = 0;
    soc_sandResult = soc_ppd_fp_control_get(unitData->unitHandle,
                                            SOC_CORE_INVALID,  /*Not used*/
                                            &control_ndx,
                                            &control_info);
    result = handle_sand_result(soc_sandResult);
    if (BCM_E_NONE == result) {
        LOG_CLI((BSL_META_U(unit,
                            "%s    Cascade length = %d\n"),
                 prefix,
                 control_info.val[0]));
    } else {
        LOG_CLI((BSL_META_U(unit,
                            "%s    Cascade length = (error %08X retrieving)\n"),
                 prefix,
                 soc_sandResult));
    }
    LOG_CLI((BSL_META_U(unit,
                        "%s    Database information:\n"), prefix));
    for (soc_sandIndex = 0; soc_sandIndex < 128; soc_sandIndex++) {
        SOC_PPD_FP_DATABASE_INFO_clear(&dbInfo);
        soc_sandResult = soc_ppd_fp_database_get(unitData->unitHandle,
                                                 soc_sandIndex,
                                                 &dbInfo);
        result = handle_sand_result(soc_sandResult);
        if (BCM_E_NONE == result) {
            if (SOC_PPD_NOF_FP_DATABASE_TYPES > dbInfo.db_type) {
                /* the type is valid, so it's in use[?] */
                LOG_CLI((BSL_META_U(unit,
                                    "%s      DB %3d: type=%s(%d), strength=%d\n"),
                         prefix,
                         soc_sandIndex,
                         SOC_PPD_FP_DATABASE_TYPE_to_string(dbInfo.db_type),
                         dbInfo.db_type,
                         dbInfo.strength));
#ifdef BCM_PETRAB_SUPPORT
                if (SOC_IS_PETRAB(unit)){
                    LOG_CLI((BSL_META_U(unit,
                                        "%s        PFG set (PB) = %02X\n"),
                             prefix,
                             dbInfo.supported_pfgs));
                }
#endif /* def BCM_PETRAB_SUPPORT */
#ifdef BCM_ARAD_SUPPORT
                if (SOC_IS_ARAD(unit)){
                    LOG_CLI((BSL_META_U(unit,
                                        "%s        PFG set (A)  = %08X%08X\n"),
                             prefix,
                             dbInfo.supported_pfgs_arad[1],
                             dbInfo.supported_pfgs_arad[0]));
                }
#endif /* def BCM_ARAD_SUPPORT*/
                LOG_CLI((BSL_META_U(unit,
                                    "%s        Cascade Pair = %3d\n"),
                         prefix,
                         dbInfo.cascaded_coupled_db_id));
                LOG_CLI((BSL_META_U(unit,
                                    "%s        Qualifiers:\n"), prefix));
                for (i = 0, j = 0, k = 0;
                     i < SOC_PPD_FP_NOF_QUALS_PER_DB_MAX;
                     i++) {
                    if (_BCM_DPP_FIELD_PPD_QUAL_VALID(dbInfo.qual_types[i])) {
                        str = SOC_PPD_FP_QUAL_TYPE_to_string(dbInfo.qual_types[i]);
                        if (0 == k) {
                            LOG_CLI((BSL_META_U(unit,
                                                "%s          %s"), prefix, str));
                            k = sal_strlen(prefix) + sal_strlen(str) + 10;
                        } else if (k + 3 + sal_strlen(str) <
                                   _BCM_DPP_FIELD_PAGE_WIDTH) {
                            LOG_CLI((BSL_META_U(unit,
                                                ", %s"), str));
                            k += sal_strlen(str) + 2;
                        } else {
                            LOG_CLI((BSL_META_U(unit,
                                                ",\n%s          %s"), prefix, str));
                            k = sal_strlen(prefix) + sal_strlen(str) + 10;
                        }
                        j++;
                    }
                }
                if (0 == j) {
                    LOG_CLI((BSL_META_U(unit,
                                        "%s          (none)\n"), prefix));
                } else {
                    LOG_CLI((BSL_META_U(unit,
                                        "\n")));
                }
                LOG_CLI((BSL_META_U(unit,
                                    "%s        Actions:\n"), prefix));
                for (i = 0, j = 0, k = 0;
                     i < SOC_PPD_FP_NOF_ACTIONS_PER_DB_MAX;
                     i++) {
                    if (_BCM_DPP_FIELD_PPD_ACTION_VALID(dbInfo.action_types[i])) {
                        str = SOC_PPD_FP_ACTION_TYPE_to_string(dbInfo.action_types[i]);
                        if (0 == k) {
                            LOG_CLI((BSL_META_U(unit,
                                                "%s          %s"), prefix, str));
                            k = sal_strlen(prefix) + sal_strlen(str) + 10;
                        } else if (k + 3 + sal_strlen(str) <
                                   _BCM_DPP_FIELD_PAGE_WIDTH) {
                            LOG_CLI((BSL_META_U(unit,
                                                ", %s"), str));
                            k += sal_strlen(str) + 2;
                        } else {
                            LOG_CLI((BSL_META_U(unit,
                                                ",\n%s          %s"), prefix, str));
                            k = sal_strlen(prefix) + sal_strlen(str) + 10;
                        }
                        
                        j++;
                    }
                }
                if (0 == j) {
                    LOG_CLI((BSL_META_U(unit,
                                        "%s          (none)\n"), prefix));
                } else {
                    LOG_CLI((BSL_META_U(unit,
                                        "\n")));
                }
                if ((SOC_PPD_FP_DB_TYPE_TCAM == dbInfo.db_type) ||
                    (SOC_PPD_FP_DB_TYPE_DIRECT_TABLE == dbInfo.db_type) ||
                    (SOC_PPD_FP_DB_TYPE_FLP == dbInfo.db_type) ||
                    (SOC_PPD_FP_DB_TYPE_EGRESS == dbInfo.db_type)) {
                    LOG_CLI((BSL_META_U(unit,
                                        "%s        Entries:\n"), prefix));
                    BCM_FREE(entInfoTc);
                    BCMDNX_ALLOC(entInfoTc, sizeof(SOC_PPD_FP_ENTRY_INFO), "_bcm_dpp_field_unit_dump.entInfoTc");
                    if (entInfoTc == NULL) {        
                        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG_NO_UNIT("failed to allocate memory")));
                    }
                    for (soc_sandOffset = 0, l = 0;
                         soc_sandOffset < unitData->entryTcLimit;
                         soc_sandOffset++) {
                        LOG_CLI((BSL_META_U(unit,
                                            "entry %d\r"), soc_sandOffset));
                        SOC_PPD_FP_ENTRY_INFO_clear(entInfoTc);
                        soc_sandResult = soc_ppd_fp_entry_get(unitData->unitHandle,
                                                              soc_sandIndex,
                                                              soc_sandOffset,
                                                              &okay,
                                                              entInfoTc);
                        result = handle_sand_result(soc_sandResult);
                        if ((BCM_E_NONE == result) && okay) {
                            LOG_CLI((BSL_META_U(unit,
                                                "%s          Entry %5u: priority"
                                     " %u\n"),
                                     prefix,
                                     soc_sandOffset,
                                     entInfoTc->priority));
                            LOG_CLI((BSL_META_U(unit,
                                                "%s            Qualifiers:\n"),
                                     prefix));
                            for (i = 0, j = 0;
                                 i < SOC_PPD_FP_NOF_QUALS_PER_DB_MAX;
                                 i++) {
                                if (_BCM_DPP_FIELD_PPD_QUAL_VALID(entInfoTc->qual_vals[i].type)) {
                                    LOG_CLI((BSL_META_U(unit,
                                                        "%s              %s (%d)\n"
                                             "%s              "
                                                        "  %08X%08X/%08X%08X\n"),
                                             prefix,
                                             SOC_PPD_FP_QUAL_TYPE_to_string(entInfoTc->qual_vals[i].type),
                                             entInfoTc->qual_vals[i].type,
                                             prefix,
                                             entInfoTc->qual_vals[i].val.arr[1],
                                             entInfoTc->qual_vals[i].val.arr[0],
                                             entInfoTc->qual_vals[i].is_valid.arr[1],
                                             entInfoTc->qual_vals[i].is_valid.arr[0]));
                                    j++;
                                }
                            }
                            if (0 == j) {
                                LOG_CLI((BSL_META_U(unit,
                                                    "%s              (none)\n"),
                                         prefix));
                            }
                            LOG_CLI((BSL_META_U(unit,
                                                "%s            Actions:\n"), prefix));
                            for (i = 0, j = 0;
                                 i < SOC_PPD_FP_NOF_ACTIONS_PER_DB_MAX;
                                 i++) {
                                if (_BCM_DPP_FIELD_PPD_ACTION_VALID(entInfoTc->actions[i].type)) {
                                    LOG_CLI((BSL_META_U(unit,
                                                        "%s              %s (%d)\n"
                                             "%s              "
                                                        "  %08X\n"),
                                             prefix,
                                             SOC_PPD_FP_ACTION_TYPE_to_string(entInfoTc->actions[i].type),
                                             entInfoTc->actions[i].type,
                                             prefix,
                                             entInfoTc->actions[i].val));
                                    j++;
                                }
                            }
                            if (0 == j) {
                                LOG_CLI((BSL_META_U(unit,
                                                    "%s              (none)\n"),
                                         prefix));
                            }
                            l++;
                        } /* if ((BCM_E_NONE == result) && okay) */
                    } /* for (soc_sandOffset = 0; soc_sandOffset < 4K; soc_sandOffset++) */
                    if (0 == l) {
                        LOG_CLI((BSL_META_U(unit,
                                            "%s            (none)\n"),
                                 prefix));
                    }
                } else if (SOC_PPD_FP_DB_TYPE_DIRECT_EXTRACTION == dbInfo.db_type) {
                    LOG_CLI((BSL_META_U(unit,
                                        "%s        Entries:\n"), prefix));
                    BCM_FREE(entInfoDe);
                    BCMDNX_ALLOC(entInfoDe, sizeof(SOC_PPD_FP_DIR_EXTR_ENTRY_INFO), "_bcm_dpp_field_unit_dump.entInfoDe");
                    if (entInfoDe == NULL) {        
                        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG_NO_UNIT("failed to allocate memory")));
                    }
                    for (soc_sandOffset = 0, l = 0;
                         soc_sandOffset < unitData->entryDeLimit;
                         soc_sandOffset++) {
                        LOG_CLI((BSL_META_U(unit,
                                            "entry %d\r"), soc_sandOffset));
                        SOC_PPD_FP_DIR_EXTR_ENTRY_INFO_clear(entInfoDe);
                        soc_sandResult = soc_ppd_fp_direct_extraction_entry_get(unitData->unitHandle,
                                                                                soc_sandIndex,
                                                                                soc_sandOffset,
                                                                                &okay,
                                                                                entInfoDe);
                        result = handle_sand_result(soc_sandResult);
                        if ((BCM_E_NONE == result) && okay) {
                            LOG_CLI((BSL_META_U(unit,
                                                "%s          Entry %5u: priority"
                                     " %u\n"),
                                     prefix,
                                     soc_sandOffset,
                                     entInfoDe->priority));
                            LOG_CLI((BSL_META_U(unit,
                                                "%s            Qualifiers:\n"),
                                     prefix));
                            for (i = 0, j = 0;
                                 i < SOC_PPD_FP_NOF_QUALS_PER_DB_MAX;
                                 i++) {
                                if (_BCM_DPP_FIELD_PPD_QUAL_VALID(entInfoDe->qual_vals[i].type)) {
                                    LOG_CLI((BSL_META_U(unit,
                                                        "%s              %s (%d)\n"
                                             "%s              "
                                                        "  %08X%08X/%08X%08X\n"),
                                             prefix,
                                             SOC_PPD_FP_QUAL_TYPE_to_string(entInfoDe->qual_vals[i].type),
                                             entInfoDe->qual_vals[i].type,
                                             prefix,
                                             entInfoDe->qual_vals[i].val.arr[1],
                                             entInfoDe->qual_vals[i].val.arr[0],
                                             entInfoDe->qual_vals[i].is_valid.arr[1],
                                             entInfoDe->qual_vals[i].is_valid.arr[0]));
                                    j++;
                                }
                            } /* for (all qualifiers this entry) */
                            if (0 == j) {
                                LOG_CLI((BSL_META_U(unit,
                                                    "%s              (none)\n"),
                                         prefix));
                            }
                            LOG_CLI((BSL_META_U(unit,
                                                "%s            Actions:\n"), prefix));
                            for (i = 0, j = 0;
                                 i < SOC_PPD_FP_NOF_ACTIONS_PER_DB_MAX;
                                 i++) {
                                if (_BCM_DPP_FIELD_PPD_ACTION_VALID(entInfoDe->actions[i].type)) {
                                    LOG_CLI((BSL_META_U(unit,
                                                        "%s              %s (%d):"
                                             " %2d location%s\n"
                                                        "%s              "
                                                        "  bias=%08X\n"),
                                             prefix,
                                             SOC_PPD_FP_ACTION_TYPE_to_string(entInfoDe->actions[i].type),
                                             entInfoDe->actions[i].type,
                                             entInfoDe->actions[i].nof_fields,
                                             (1 == entInfoDe->actions[i].nof_fields)?"":"s",
                                             prefix,
                                             entInfoDe->actions[i].base_val));
                                    j++;
                                    for (k = 0;
                                         k < entInfoDe->actions[i].nof_fields;
                                         k++) {
                                        if ((entInfoDe->actions[i].fld_ext[k].cst_val) ||
                                            (SOC_PPD_NOF_FP_QUAL_TYPES == entInfoDe->actions[i].fld_ext[k].type)) {
                                            LOG_CLI((BSL_META_U(unit,
                                                                "%s                "
                                                     "const %08X, %d"
                                                                " LSbs\n"),
                                                     prefix,
                                                     entInfoDe->actions[i].fld_ext[k].cst_val,
                                                     entInfoDe->actions[i].fld_ext[k].nof_bits));
                                        } else {
                                            LOG_CLI((BSL_META_U(unit,
                                                                "%s                "
                                                     "%s (%d), bits"
                                                                " %d..%d\n"),
                                                     prefix,
                                                     SOC_PPD_FP_QUAL_TYPE_to_string(entInfoDe->actions[i].fld_ext[k].type),
                                                     entInfoDe->actions[i].fld_ext[k].type,
                                                     entInfoDe->actions[i].fld_ext[k].nof_bits + entInfoDe->actions[i].fld_ext[k].fld_lsb - 1,
                                                     entInfoDe->actions[i].fld_ext[k].fld_lsb));
                                        }
                                    } /* for (all extractions this action) */
                                } /* if (this action is valid) */
                            } /* for (all actions this entry) */
                            if (0 == j) {
                                LOG_CLI((BSL_META_U(unit,
                                                    "%s              (none)\n"),
                                         prefix));
                            }
                            l++;
                        } /* if ((BCM_E_NONE == result) && okay) */
                    } /* for (soc_sandOffset = 0; soc_sandOffset < 4K; soc_sandOffset++) */
                    if (0 == l) {
                        LOG_CLI((BSL_META_U(unit,
                                            "%s            (none)\n"),
                                 prefix));
                    }
                }
                
            } else { /* if (SOC_PPD_NOF_FP_DATABASE_TYPES > dbInfo.db_type) */
                if (SOC_PPD_NOF_FP_DATABASE_TYPES < dbInfo.db_type) {
                    LOG_CLI((BSL_META_U(unit,
                                        "%s      DB %3d: unexpected type %d\n"),
                             prefix,
                             soc_sandIndex,
                             dbInfo.db_type));
                }
            } /* if (SOC_PPD_NOF_FP_DATABASE_TYPES > dbInfo.db_type) */
        } else { /* if (BCM_E_NONE == result) */
            LOG_CLI((BSL_META_U(unit,
                                "%s      DB %3d: unable to read %d (%s)\n"),
                     prefix,
                     soc_sandIndex,
                     result,
                     _SHR_ERRMSG(result)));
        } /* if (BCM_E_NONE == result) */
    } /* for (soc_sandIndex = 0; soc_sandIndex < 128; soc_sandIndex++) */
#endif /* _BCM_PETRA_FIELD_DUMP_INCLUDE_PPD */
#ifdef BCM_WARM_BOOT_SUPPORT
    if (BCM_E_NONE == result) {
        _bcm_dpp_wb_field_state_dump(unitData, prefix);
    }
#endif /* def BCM_WARM_BOOT_SUPPORT */
#if _BCM_DPP_UNIT_DUMP_INCLUDE_RES_DUMP
    
    if (BCM_E_NONE == result) {
        result = shr_mres_dump(unitData->hwResIds);
    }
#endif /* _BCM_DPP_UNIT_DUMP_INCLUDE_RES_DUMP */

    if (BCM_E_NONE != result) {
        LOG_CLI((BSL_META_U(unit,
                            "%s  (error %d (%s) displaying unit)\n"),
                 prefix,
                 result,
                 _SHR_ERRMSG(result)));
    }

    BCMDNX_IF_ERR_EXIT(result);

exit:
    BCM_FREE(newPrefix);
    BCM_FREE(pfgInfo);
    BCM_FREE(entInfoTc);
    BCM_FREE(entInfoDe);
    BCMDNX_FUNC_RETURN;
}
#endif /* def BROADCOM_DEBUG */

signed int
_bcm_dpp_compare_entry_priority(int pri1,
                                int pri2)
{
    int unit;

    unit = BSL_UNIT_UNKNOWN;

    /* validate parameters */
    /* note that we don't allow *any* negative priorities on this platform */
    if (0 > pri1) {
        /* pri1 is not valid */
        LOG_ERROR(BSL_LS_BCM_FP,
                  (BSL_META_U(unit,
                              "pri1 (%d) invalid\n"), pri1));
        if (0 > pri2) {
            /* pri2 is not valid either, so equal   */
            LOG_ERROR(BSL_LS_BCM_FP,
                      (BSL_META_U(unit,
                                  "pri2 (%d) invalid\n"), pri1));
            return 0;
        }
        /* pri2 is valid, so it's greater */
        return -1;
    }
    if (0 > pri2) {
        /* pri2 is not valid, so pri1 is greater (since it's valid) */
        LOG_ERROR(BSL_LS_BCM_FP,
                  (BSL_META_U(unit,
                              "pri2 (%d) invalid\n"), pri1));
        return 1;
    }
    /*
     *  All other cases degenerated into this, with the change from the special
     *  values being negative to them being just really wide-flung positive
     *  values.
     */
    return pri1 - pri2;
}

int
_bcm_dpp_field_qset_subset(bcm_field_qset_t qset1,
                           bcm_field_qset_t qset2)
{
    unsigned int index;                 /* working index for loops */
    int result;                         /* result for caller */
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;
 
    LOG_DEBUG(BSL_LS_BCM_FP,
              (BSL_META_U(unit,
                          "(*,*)\n")));

    /* be optimistic */
    result = BCM_E_NONE;

    /* check all qualifiers */
    LOG_DEBUG(BSL_LS_BCM_FP,
              (BSL_META_U(unit,
                          "scanning qualifiers\n")));
    for (index = 0;
         (index < bcmFieldQualifyCount) && (BCM_E_NONE == result);
         index++) {
        if (BCM_FIELD_QSET_TEST(qset2, index)) {
            if (!BCM_FIELD_QSET_TEST(qset1, index)) {
                result = BCM_E_FAIL;
                LOG_ERROR(BSL_LS_BCM_FP,
                          (BSL_META_U(unit,
                                      "qualifier %d (%s) in qset 2"
                                      " but not in qset 1\n"),
                           index,
                           _bcm_dpp_field_qual_name[index]));
            }
        } /* if (BCM_FIELD_QSET_TEST(qset2, index)) */
    } /* for (all possible fixed qualifiers as long as no error) */
    for (index = 0;
         (index < BCM_FIELD_USER_NUM_UDFS) && (BCM_E_NONE == result);
         index++) {
        if (SHR_BITGET(qset2.udf_map, index)) {
            if (!SHR_BITGET(qset1.udf_map, index)) {
                result = BCM_E_FAIL;
                LOG_ERROR(BSL_LS_BCM_FP,
                          (BSL_META_U(unit,
                                      "data field qualifier %d in qset 2"
                                      " but not in qset 1\n"),
                           index));
            }
        } /* if (SHR_BITGET(qset2.udf_map, index)) */
    } /* for (all possible programmable qualifiers as long as no error) */

    /* then tell the caller the result */
    LOG_DEBUG(BSL_LS_BCM_FP,
              (BSL_META_U(unit,
                          "(*,*) = %d (%s)\n"),
               result,
               _SHR_ERRMSG(result)));
    BCMDNX_IF_ERR_EXIT(result);
exit:
    BCMDNX_FUNC_RETURN;
}

void
_bcm_dpp_field_qset_union(bcm_field_qset_t qset0,
                          bcm_field_qset_t qset1,
                          bcm_field_qset_t *qset2)
{
    bcm_field_qset_t_init(qset2);
    SHR_BITOR_RANGE(qset0.w,
                    qset1.w,
                    0,
                    bcmFieldQualifyCount,
                    qset2->w);
    SHR_BITOR_RANGE(qset0.udf_map,
                    qset1.udf_map,
                    0,
                    BCM_FIELD_USER_NUM_UDFS,
                    qset2->udf_map);
}

void
_bcm_dpp_field_qset_subset_count(bcm_field_qset_t qset0,
                                 bcm_field_qset_t qset1,
                                 unsigned int *qual0,
                                 unsigned int *qual1,
                                 unsigned int *qualBoth)
{
    unsigned int index;

    *qual0 = 0;
    *qual1 = 0;
    *qualBoth = 0;
    for (index = 0; index < bcmFieldQualifyCount; index++) {
        if (BCM_FIELD_QSET_TEST(qset0, index)) {
            (*qual0)++;
            if (BCM_FIELD_QSET_TEST(qset1, index)) {
                (*qualBoth)++;
            }
        }
        if (BCM_FIELD_QSET_TEST(qset1, index)) {
            (*qual1)++;
        }
    }
    for (index = 0; index < BCM_FIELD_USER_NUM_UDFS; index++) {
        if (SHR_BITGET(qset0.udf_map, index)) {
            (*qual0)++;
            if (SHR_BITGET(qset1.udf_map, index)) {
                (*qualBoth)++;
            }
        }
        if (SHR_BITGET(qset1.udf_map, index)) {
            (*qual1)++;
        }
    }
}

int
_bcm_dpp_field_aset_subset(bcm_field_aset_t aset1,
                           bcm_field_aset_t aset2)
{
    unsigned int index;                 /* working index for loops */
    int result;                         BCMDNX_INIT_FUNC_DEFS_NO_UNIT;
                                        /* result for caller */
    LOG_DEBUG(BSL_LS_BCM_FP,
              (BSL_META("(*,*)\n")));

    /* be optimistic */
    result = BCM_E_NONE;

    /* check all qualifiers */
    LOG_DEBUG(BSL_LS_BCM_FP,
              (BSL_META("scanning actions\n")));
    for (index = 0;
         (index < bcmFieldActionCount) && (BCM_E_NONE == result);
         index++) {
        if (BCM_FIELD_ASET_TEST(aset2,index)) {
            if (!BCM_FIELD_ASET_TEST(aset1,index)) {
                result = BCM_E_FAIL;
                LOG_ERROR(BSL_LS_BCM_FP,
                          (BSL_META("action %d (%s) in aset 2"
" but not in aset 1\n"),
                           index,
                           _bcm_dpp_field_action_name[index]));
            }
        } /* if (BCM_FIELD_ASET_TEST(qset2,index)) */
    } /* for (index = 0; index < bcmFieldActionCount; index++) */

    /* then tell the caller the result */
    LOG_DEBUG(BSL_LS_BCM_FP,
              (BSL_META("(*,*) = %d (%s)\n"),
               result,
               _SHR_ERRMSG(result)));
    BCMDNX_IF_ERR_EXIT(result);
exit:
    BCMDNX_FUNC_RETURN;
}

void
_bcm_dpp_field_aset_union(bcm_field_aset_t aset0,
                          bcm_field_aset_t aset1,
                          bcm_field_aset_t *aset2)
{
    bcm_field_aset_t_init(aset2);
    SHR_BITOR_RANGE(aset0.w, aset1.w, 0, bcmFieldActionCount, aset2->w);
}

int
_bcm_dpp_ppd_act_from_bcm_act(_bcm_dpp_field_info_t *unitData,
                              _bcm_dpp_field_stage_idx_t stage,
                              bcm_field_action_t bcmAct,
                              SOC_PPD_FP_ACTION_TYPE **ppdAct)
{
    unsigned int base;
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;

    if ((0 > bcmAct) || (bcmFieldActionCount <= bcmAct)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG_NO_UNIT("action %d is not valid"),
                          bcmAct));
    }
    if (!unitData->actMaps[bcmAct]) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d does not support action"
                                           " %d (%s)"),
                          unit,
                          bcmAct,
                          _bcm_dpp_field_action_name[bcmAct]));
    }
    /* compute the number of elements before that chain */
    base = stage * unitData->devInfo->actChain;
    /* offset for BCM action type */
    base += _BCM_DPP_FIELD_ACTMAP_OFFSET_MAPS;
    /* return the pointer to it to the caller */
    (*ppdAct) = (SOC_PPD_FP_ACTION_TYPE*)(&(unitData->actMaps[bcmAct][base]));
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_ppd_qual_from_bcm_qual(_bcm_dpp_field_info_t *unitData,
                                _bcm_dpp_field_stage_idx_t stage,
                                uint32 types,
                                bcm_field_qualify_t bcmQual,
                                SOC_PPD_FP_QUAL_TYPE **ppdQual)
{
    unsigned int base;
    _bcm_dpp_field_type_idx_t type;
    _bcm_dpp_field_type_idx_t ltype;
    _bcm_dpp_field_type_idx_t limit;
    uint32 typeLimit;
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;

    if ((0 > bcmQual) || (bcmFieldQualifyCount <= bcmQual)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG_NO_UNIT("qualifier %d is not valid"),
                          bcmQual));
    }
    typeLimit = (1 << unitData->devInfo->types) - 1;
    if (unitData->stageD[stage].devInfo->stageFlags &
        _BCM_DPP_FIELD_DEV_STAGE_USES_PRESELECTORS) {
        /* stage uses preselectors; consider them */
        limit = _BCM_DPP_FIELD_MAX_GROUP_TYPES + 1;
        typeLimit |= (1 << _BCM_DPP_FIELD_MAX_GROUP_TYPES);
    } else {
        /* stage does not use preselectors; do not consider them */
        limit = unitData->devInfo->types;
    }
    if (0 == (types & typeLimit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG_NO_UNIT("must specify at least one valid"
                                           " type in search bitmap (valid"
                                           " types include %08X)"),
                          typeLimit));
    }
    if (!unitData->qualMaps[bcmQual]) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d does not support qualifier"
                                           " %d (%s)"),
                          unit,
                          bcmQual,
                          _bcm_dpp_field_qual_name[bcmQual]));
    }
    type = limit;
    ltype = limit;
    while (type > 0) {
        type--;
        if ((types & (1 << type)) &&
            BCM_FIELD_QSET_TEST(unitData->stageD[stage].qsetType[type],
                                bcmQual)) {
            ltype = type;
            break;
        }
    }
    /*
     *  We pick the *last* (highest type number) match because we expect the
     *  types to get more specific as the number increases.
     *
     *  If there are divergences between overlapped types, we expect this to
     *  provide the 'most appropriate' version of the qualifier in the context.
     */
    if (ltype >= limit) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d stage %u types %08X does"
                                           " not include qualifier %d (%s)"),
                          unit,
                          stage,
                          types,
                          bcmQual,
                          _bcm_dpp_field_qual_name[bcmQual]));
    }
    /* get the qualifier chain that we need to use */
    base = unitData->stageD[stage].ltOffset[ltype];
    /* compute the number of elements before that chain */
    base *= unitData->devInfo->qualChain;
    /* offset for BCM qual type, width, shift distance */
    base += _BCM_DPP_FIELD_QUALMAP_OFFSET_MAPS;
    /* return the pointer to it to the caller */
    (*ppdQual) = (SOC_PPD_FP_QUAL_TYPE*)(&(unitData->qualMaps[bcmQual][base]));
exit:
    BCMDNX_FUNC_RETURN;
}

void
_bcm_dpp_field_group_priority_recalc(_bcm_dpp_field_info_t *unitData,
                                     _bcm_dpp_field_grp_idx_t group)
{
    _bcm_dpp_field_group_t *groupData;
    _bcm_dpp_field_ent_idx_t entry;
    unsigned int offset;
    unsigned int count;
    unsigned int phasePrio;
    _bcm_dpp_field_entry_type_t entryType;
	uint32 *entryFlags;    
	uint32 *hwPriority;
    int unit;
    
    unit = unitData->unit;    

    groupData = &(unitData->groupD[group]);
    entryType = unitData->stageD[groupData->stage].modeBits[groupData->grpMode]->entryType;
    /* toggle phase */
    groupData->groupFlags ^= _BCM_DPP_FIELD_GROUP_PHASE;
    /* compute base priority from phase */
    if (groupData->groupFlags & _BCM_DPP_FIELD_GROUP_PHASE) {
        phasePrio = 1 << 13;
    } else {
        phasePrio = 0;
    }
    LOG_DEBUG(BSL_LS_BCM_FP,
              (BSL_META_U(unit,
                          "unit %d group %d entering phase %d\n"),
               unit,
               group,
               (groupData->groupFlags & _BCM_DPP_FIELD_GROUP_PHASE)?1:0));
    /* set hardware priority for all existing entries in the group */
    switch (entryType) {
    case _bcmDppFieldEntryTypeDirExt:
        /* direct extraction */
        if (groupData->groupFlags & _BCM_DPP_FIELD_GROUP_PHASE) {
            /* phase 1, start at high/tail and work down/to head */
            for (entry = groupData->entryTail, offset = 7, count = 0;
                 entry < unitData->entryDeLimit;
                 entry = unitData->entryDe[entry].entryCmn.entryPrev, offset--, count++) {
                unitData->entryDe[entry].entryCmn.hwPriority = offset;
                /* entry is no longer 'new' (moved or added since priority update) */
                unitData->entryDe[entry].entryCmn.entryFlags &= (~_BCM_DPP_FIELD_ENTRY_NEW);
                /* entry has been changed since last time it was written to hardware */
                unitData->entryDe[entry].entryCmn.entryFlags |= _BCM_DPP_FIELD_ENTRY_CHANGED;
            }
        } else { /* if (groupData->flags & _BCM_DPP_FIELD_GROUP_PHASE) */
            /* phase 0, start at low/head and work up/to tail */
            for (entry = groupData->entryHead, offset = 0, count = 0;
                 entry < unitData->entryDeLimit;
                 entry = unitData->entryDe[entry].entryCmn.entryNext, offset++, count++) {
                unitData->entryDe[entry].entryCmn.hwPriority = offset;
                /* entry is no longer 'new' (moved or added since priority update) */
                unitData->entryDe[entry].entryCmn.entryFlags &= (~_BCM_DPP_FIELD_ENTRY_NEW);
                /* entry has been changed since last time it was written to hardware */
                unitData->entryDe[entry].entryCmn.entryFlags |= _BCM_DPP_FIELD_ENTRY_CHANGED;
            }
        } /* if (groupData->flags & _BCM_DPP_FIELD_GROUP_PHASE) */
        break;
    default:
        /* default is TCAM */
        for (entry = groupData->entryHead, offset = 0, count = 0;
             entry < _BCM_DPP_FIELD_TCAM_ENTRY_LIMIT(entryType);
             entry = _BCM_DPP_FIELD_TCAM_ENTRY_NEXT(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry), offset++, count++) {
            hwPriority = _BCM_DPP_FIELD_TCAM_ENTRY_HW_PRIORITY_PTR(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry);
            *hwPriority = ((_BCM_DPP_FIELD_TCAM_ENTRY_HW_PRIORITY(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry) & 1) |
                                                 (phasePrio + (offset << 1)));
            /* entry is no longer 'new' (moved or added since priority update) */
            entryFlags = _BCM_DPP_FIELD_TCAM_ENTRY_FLAGS_PTR(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry);
            *entryFlags &= (~_BCM_DPP_FIELD_ENTRY_NEW);
            /* entry has been changed since last time it was written to hardware */
            *entryFlags |= _BCM_DPP_FIELD_ENTRY_CHANGED;
        }
    }
    /* no entries have been added/moved since the hw priorities update now */
    groupData->groupFlags &= (~_BCM_DPP_FIELD_GROUP_ADD_ENT);
#ifdef BCM_WARM_BOOT_SUPPORT
    _bcm_dpp_field_group_wb_save(unitData, group, NULL, NULL);
#endif /* def BCM_WARM_BOOT_SUPPORT */
    LOG_DEBUG(BSL_LS_BCM_FP,
              (BSL_META_U(unit,
                          "unit %d group %d updated hardware priority on"
                          " %d entries\n"),
               unit,
               group,
               count));
}

int
_bcm_dpp_field_stage_type_qset_aset_get(int unit,
                                        _bcm_dpp_field_stage_idx_t stage,
                                        _bcm_dpp_field_type_idx_t type,
                                        bcm_field_qset_t *qset,
                                        bcm_field_aset_t *aset)
{
    _DPP_FIELD_COMMON_LOCALS;
    BCMDNX_INIT_FUNC_DEFS;

    _DPP_FIELD_UNIT_CHECK(unit, unitData);
    if ((!qset) || (!aset)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG("obligatory OUT args must not be NULL")));
    }
    _DPP_FIELD_UNIT_LOCK(unitData);

    if (stage > unitData->devInfo->stages) {
        result = BCM_E_NOT_FOUND;
        LOG_ERROR(BSL_LS_BCM_FP,
                  (BSL_META_U(unit,
                              "unit %d does not have stage %u\n"),
                   unit,
                   stage));
    }
    if (type > unitData->devInfo->types) {
        result = BCM_E_NOT_FOUND;
        LOG_ERROR(BSL_LS_BCM_FP,
                  (BSL_META_U(unit,
                              "unit %d does not have type %d\n"),
                   unit,
                   type));
    }
    if (BCM_E_NONE == result) {
        sal_memcpy(qset,
                   &(unitData->stageD[stage].qsetType[type]),
                   sizeof(*qset));
        sal_memcpy(aset,
                   &(unitData->stageD[stage].stgAset),
                   sizeof(*aset));
    }

    _DPP_FIELD_UNIT_UNLOCK(unitData);
    BCMDNX_IF_ERR_EXIT(result);
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_field_group_qset_recover(_bcm_dpp_field_info_t *unitData,
                                  _bcm_dpp_field_stage_idx_t stage,
                                  uint32 types,
                                  _bcm_dpp_field_qual_set_t *ppdQset,
                                  bcm_field_qset_t *bcmQset)
{
    unsigned int qualifier;
    unsigned int type;
    unsigned int chain;
    unsigned int base;
    unsigned int offset;
    unsigned int ppdQual;
    unsigned int bcmQual;
    int include;
    int result;
    int unit;

    unit = unitData->unit;

    BCM_FIELD_QSET_INIT(*bcmQset);
    /* scan for standard qualifiers */
    for (qualifier = 0; qualifier < unitData->qualMapCount; qualifier++) {
        base = qualifier * ((unitData->devInfo->mappings *
                             unitData->devInfo->qualChain) +
                            _BCM_DPP_FIELD_QUALMAP_OFFSET_MAPS);
        for (type = 0; type < unitData->devInfo->types; type++) {
            if (0 == (types & (1 << type))) {
                /* the group does not include this type */
                continue;
            }
            offset = (base +
                      (unitData->stageD[stage].ltOffset[type] *
                       unitData->devInfo->qualChain) +
                      _BCM_DPP_FIELD_QUALMAP_OFFSET_MAPS);
            /*
             *  Consider including this qualifier if it is valid here.  Also,
             *  consider including this qualifier if it is tagged as being
             *  only a flag qualifier for this particular type in this
             *  particular stage (in this case, it will be included unless
             *  there is another qualifier required that is neither implied nor
             *  included in the hardware qualifiers).
             */
            include = ((_BCM_DPP_FIELD_PPD_QUAL_VALID(unitData->devInfo->qualMap[offset])) ||
                       (SOC_PPD_NOF_FP_QUAL_TYPES + 1 ==
                        unitData->devInfo->qualMap[offset]));
            for (chain = 0;
                 include && (chain < unitData->devInfo->qualChain);
                 chain++) {
                ppdQual = unitData->devInfo->qualMap[offset + chain];
                if (_BCM_DPP_FIELD_PPD_QUAL_VALID(ppdQual)) {
                    /* this chain element is a valid type */
                    if (!SHR_BITGET(*ppdQset, ppdQual)) {
                        /* but this group does not have this qualifier */
                        include = FALSE;
                    } /* if (!SHR_BITGET(groupData->pqset, ppdQual)) */
                } /* if (this qualifier is valid) */
            } /* for (all qualifiers in this chain) */
            if (!include) {
                /* should not be included; try the next type */
                continue;
            }
            bcmQual = unitData->devInfo->qualMap[base + _BCM_DPP_FIELD_QUALMAP_OFFSET_QUAL];

            LOG_DEBUG(BSL_LS_BCM_FP,
                      (BSL_META_U(unit,
                                  "PPD qualifier chain at qual %u type %s"
                                  " (%d) implies %s (%d) should be in qset"
                                  " for unit %d stage %s (%u) types %08X\n"),
                       qualifier,
                       unitData->devInfo->typeNames[type],
                       type,
                       _bcm_dpp_field_qual_name[bcmQual],
                       bcmQual,
                       unit,
                       unitData->devInfo->stage[stage].stageName,
                       stage,
                       types));
            BCM_FIELD_QSET_ADD(*bcmQset, bcmQual);
        } /* for (type = 0; type < unitData->devInfo->types; type++) */
    } /* for (all qualifier maps supported by this unit) */
    /* scan for data qualifiers */
    for (ppdQual = 0; ppdQual < SOC_PPD_NOF_FP_QUAL_TYPES; ppdQual++) {
        if (SHR_BITGET(*ppdQset, ppdQual)) {
            /* this qualifier is included for this group */
            result = _bcm_petra_field_map_ppd_udf_to_bcm(unitData,
                                                         ppdQual,
                                                         &bcmQual);
            if (BCM_E_NONE == result) {
                /* this qualifier mapped to a data qualifier */
                LOG_DEBUG(BSL_LS_BCM_FP,
                          (BSL_META_U(unit,
                                      "PPD qualifier %s %d implies data"
                                      " qualifier %u should be in qset"
                                      " for unit %u stage %s (%u)"
                                      " (types %08X)\n"),
                           SOC_PPD_FP_QUAL_TYPE_to_string(ppdQual),
                           ppdQual,
                           bcmQual,
                           unit,
                           unitData->devInfo->stage[stage].stageName,
                           stage,
                           types));
                SHR_BITSET(bcmQset->udf_map, bcmQual);
            } /* if (BCM_E_NONE == result) */
        } /* if (SHR_BITGET(groupData->pqset, ppdQual)) */
    } /* for (all PPD layer qualifiers) */
    return BCM_E_NONE;
}

int
_bcm_dpp_field_group_aset_recover(_bcm_dpp_field_info_t *unitData,
                                  _bcm_dpp_field_stage_idx_t stage,
                                  _bcm_dpp_field_action_set_t *ppdAset,
                                  bcm_field_aset_t *bcmAset)
{
    unsigned int action;
    unsigned int chain;
    unsigned int base;
    unsigned int offset;
    unsigned int ppdAct;
    int include;
    int unit;

    unit = unitData->unit;
    BCM_FIELD_ASET_INIT(*bcmAset);
    for (action = 0; action < unitData->actMapCount; action++) {
        base = action * ((unitData->devInfo->stages *
                          unitData->devInfo->actChain) +
                         _BCM_DPP_FIELD_ACTMAP_OFFSET_MAPS);
            offset = (base +
                  (stage * unitData->devInfo->actChain) +
                      _BCM_DPP_FIELD_ACTMAP_OFFSET_MAPS);
            /* assume the group might include this qualifier */
            include = _BCM_DPP_FIELD_PPD_ACTION_VALID(unitData->devInfo->actMap[offset]);
            for (chain = 0;
                 include && (chain < unitData->devInfo->actChain);
                 chain++) {
                ppdAct = unitData->devInfo->actMap[offset + chain];
                if (_BCM_DPP_FIELD_PPD_ACTION_VALID(ppdAct)) {
                    /* this chain element is a valid type */
                    if (!SHR_BITGET(*ppdAset, ppdAct)) {
                        /* but this group does not have this qualifier */
                        include = FALSE;
                    } /* if (!SHR_BITGET(groupData->pqset, ppdQual)) */
                } /* if (this qualifier is valid) */
            } /* for (all qualifiers in this chain) */
            if (!include) {
                /* should not be included; try the next type */
                continue;
            }

        LOG_VERBOSE(BSL_LS_BCM_FP,
                    (BSL_META_U(unit,
                                "PPD action chain at action %u"
                                " implies %s (%d) should be in aset"
                                " for unit %d stage %s (%u)\n"),
                     action,
                     _bcm_dpp_field_action_name[unitData->devInfo->actMap[base + _BCM_DPP_FIELD_ACTMAP_OFFSET_ACT]],
                     unitData->devInfo->actMap[base + _BCM_DPP_FIELD_ACTMAP_OFFSET_ACT],
                     unit,
                     unitData->devInfo->stage[stage].stageName,
                     stage));

            BCM_FIELD_ASET_ADD(*bcmAset,
                               unitData->devInfo->actMap[base + _BCM_DPP_FIELD_ACTMAP_OFFSET_ACT]);
    } /* for (all action maps supported by this unit) */
    return BCM_E_NONE;
}

/*
 *  Function
 *    _bcm_dpp_field_qualifier_set_single_int
 *  Purpose
 *    Set a single qualifier on an entry
 *  Arguments
 *    (in) unitData = unit information
 *    (in) entry = entry ID
 *    (in) qual = pointer to qualifier array for the entry
 *    (in) qualLimit = max qualifier index applicable to this entry
 *    (in) hwType = PPD layer qualifier to add to the entry
 *    (in) data = data value for qualifier
 *    (in) mask = mask bits for qualifier
 *    (in) valid = valid bits for qualifier
 *  Returns
 *    bcm_error_t cast as int
 *      BCM_E_NONE if success
 *      BCM_E_* otherwise as appropriate
 *  Notes
 *    Caller must already hold the unit lock.
 *
 *    Warns if data or mask bits are outside of valid bits, or if data bits are
 *    outside of mask bits.  The data and mask are the values for the PPD
 *    layer, and are therefore not shifted or concatenated or sliced here.
 *
 *    Sets the BCM layer qualifier according to the reverse mapping.
 *
 *    Removes a qualifier from the list if its mask is all zeroes.  When a BCM
 *    qualifier is a proper subset of a PPD qualifier, it only affects those
 *    bits in the PPD qualifier, so if there are other valid bits at PPD, the
 *    qualifier will remain.
 *
 *    Caller must update entry flags, group flags, &c, as appropriate.
 */
STATIC int
_bcm_dpp_field_qualifier_set_single_int(_bcm_dpp_field_info_t *unitData,
                                        bcm_field_entry_t entry,
                                        _bcm_dpp_field_qual_t *qual,
                                        unsigned int qualLimit,
                                        SOC_PPD_FP_QUAL_TYPE hwType,
                                        const uint64 data,
                                        const uint64 mask,
                                        const uint64 valid)
{
    uint64 edata;
    uint64 emask;
    uint64 ivalid;
    unsigned int index;
    unsigned int free;
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;

    LOG_DEBUG(BSL_LS_BCM_FP,
              (BSL_META_U(unit,
                          "(*(%d),%d,%d;%s(%d;%s),"
                          "%08X%08X,%08X%08X,%08X%08X)\n"),
               unit,
               entry,
               hwType,
               SOC_PPD_FP_QUAL_TYPE_to_string(hwType),
               unitData->ppdQual[hwType],
               (bcmFieldQualifyCount>unitData->ppdQual[hwType])?_bcm_dpp_field_qual_name[unitData->ppdQual[hwType]]:"-",
               COMPILER_64_HI(data),
               COMPILER_64_LO(data),
               COMPILER_64_HI(mask),
               COMPILER_64_LO(mask),
               COMPILER_64_HI(valid),
               COMPILER_64_LO(valid)));
    edata = data;
    emask = mask;
    ivalid = valid;
    COMPILER_64_NOT(ivalid);
    COMPILER_64_AND(edata, ivalid);
    if (!COMPILER_64_IS_ZERO(edata)) {
        LOG_WARN(BSL_LS_BCM_FP,
                 (BSL_META_U(unit,
                             "unit %d entry %d qualifier %d;%s (%d;%s)"
                             " data %08X%08X has bits not marked"
                             " as valid %08X%08X\n"),
                  unit,
                  entry,
                  hwType,
                  SOC_PPD_FP_QUAL_TYPE_to_string(hwType),
                  unitData->ppdQual[hwType],
                  (bcmFieldQualifyCount>unitData->ppdQual[hwType])?_bcm_dpp_field_qual_name[unitData->ppdQual[hwType]]:"-",
                  COMPILER_64_HI(data),
                  COMPILER_64_LO(data),
                  COMPILER_64_HI(valid),
                  COMPILER_64_LO(valid)));
    }
    COMPILER_64_AND(emask, ivalid);
    if (!COMPILER_64_IS_ZERO(emask)) {
        LOG_WARN(BSL_LS_BCM_FP,
                 (BSL_META_U(unit,
                             "unit %d entry %d qualifier %d;%s (%d;%s)"
                             " mask %08X%08X has bits not marked"
                             " as valid %08X%08X\n"),
                  unit,
                  entry,
                  hwType,
                  SOC_PPD_FP_QUAL_TYPE_to_string(hwType),
                  unitData->ppdQual[hwType],
                  (bcmFieldQualifyCount>unitData->ppdQual[hwType])?_bcm_dpp_field_qual_name[unitData->ppdQual[hwType]]:"-",
                  COMPILER_64_HI(mask),
                  COMPILER_64_LO(mask),
                  COMPILER_64_HI(valid),
                  COMPILER_64_LO(valid)));
    }
    edata = data;
    emask = mask;
    COMPILER_64_NOT(emask);
    COMPILER_64_AND(edata, emask);
    if (!COMPILER_64_IS_ZERO(edata)) {
        LOG_ERROR(BSL_LS_BCM_FP,
                  (BSL_META_U(unit,
                              "unit %d entry %d qualifier %d;%s (%d;%s)"
                              " data %08X%08X has bits not included"
                              " in mask %08X%08X; these bits will"
                              " be ignored in matching\n"),
                   unit,
                   entry,
                   hwType,
                   SOC_PPD_FP_QUAL_TYPE_to_string(hwType),
                   unitData->ppdQual[hwType],
                   (bcmFieldQualifyCount>unitData->ppdQual[hwType])?_bcm_dpp_field_qual_name[unitData->ppdQual[hwType]]:"-",
                   COMPILER_64_HI(data),
                   COMPILER_64_LO(data),
                   COMPILER_64_HI(mask),
                   COMPILER_64_LO(mask)));
    }

    /* find either the old value or a free slot */
    for (index = 0, free = qualLimit;
         (index < qualLimit) && (qual[index].hwType != hwType);
         index++) {
        /* look for free quals until scanned all or hit requested qual */
        if ((!_BCM_DPP_FIELD_PPD_QUAL_VALID(qual[index].hwType)) &&
            (qualLimit <= free)) {
            /* first free one seen; keep it */
            free = index;
        }
    } /* for (all possible qualifier slots this stage) */
    edata = data;
    emask = mask;
    COMPILER_64_AND(edata, valid);
    COMPILER_64_AND(emask, valid);
    COMPILER_64_AND(edata, mask);
    if (index < qualLimit) {
        /* found existing instance, update it */
        /*
         *  Note that some qualifiers are subsets of others, and we consider
         *  these subsets to be just a masked part of the superset. This means
         *  that we need to allow multiple subsets of a superset, thence we
         *  must only overwrite the subset at hand whenever updating a subset
         *  of an existing qualifier.
         */
        COMPILER_64_AND(qual[index].qualData, ivalid);
        COMPILER_64_AND(qual[index].qualMask, ivalid);
        COMPILER_64_OR(qual[index].qualData, edata);
        COMPILER_64_OR(qual[index].qualMask, emask);
    } else if (free < qualLimit) {
        /* did not find existing instance, use the free slot */
        /*
         *  No need to worry particularly about being a subset of an
         *  existing qualifier if there is no existing qualifier.
         */
        qual[free].hwType = hwType;
        qual[free].qualType = unitData->ppdQual[hwType];
        qual[free].qualData = edata;
        qual[free].qualMask = emask;
        index = free;
    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d does not have hw"
                                           " qualifier %d (%s) but already has"
                                           " as many hw qualifiers as it is"
                                           " allowed to have %d"),
                          unit,
                          entry,
                          hwType,
                          SOC_PPD_FP_QUAL_TYPE_to_string(hwType),
                          qualLimit));
    }
    /* Remove the zeroes qualifiers except for the direct table entries */
    if (COMPILER_64_IS_ZERO(qual[index].qualMask) 
          && _BCM_DPP_FIELD_ENT_IS_DIR_EXT(unit, entry)) {
        /* the significant bits are all zeroes, so remove it */
        LOG_DEBUG(BSL_LS_BCM_FP,
                  (BSL_META_U(unit,
                              "unit %d entry %d qualifier %d (%s) (%d (%s))"
                              " removed since it has no significant bits\n"),
                   unit,
                   entry,
                   unitData->ppdQual[hwType],
                   (bcmFieldQualifyCount>unitData->ppdQual[hwType])?_bcm_dpp_field_qual_name[unitData->ppdQual[hwType]]:"-",
                   hwType,
                   SOC_PPD_FP_QUAL_TYPE_to_string(hwType)));
        COMPILER_64_ZERO(qual[index].qualData);
        COMPILER_64_ZERO(qual[index].qualMask);
        qual[index].hwType = SOC_PPD_NOF_FP_QUAL_TYPES;
        qual[index].qualType = bcmFieldQualifyCount;
    } else {
        LOG_DEBUG(BSL_LS_BCM_FP,
                  (BSL_META_U(unit,
                              "unit %d entry %d qualifier %d (%s) (%d (%s))"
                              " now %08X%08X/%08X%08X\n"),
                   unit,
                   entry,
                   unitData->ppdQual[hwType],
                   (bcmFieldQualifyCount>unitData->ppdQual[hwType])?_bcm_dpp_field_qual_name[unitData->ppdQual[hwType]]:"-",
                   hwType,
                   SOC_PPD_FP_QUAL_TYPE_to_string(hwType),
                   COMPILER_64_HI(qual[index].qualData),
                   COMPILER_64_LO(qual[index].qualData),
                   COMPILER_64_HI(qual[index].qualMask),
                   COMPILER_64_LO(qual[index].qualMask)));
    }
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *    _bcm_dpp_field_qualifier_set_prep
 *  Purpose
 *    Prepare for manupulating the qualifiers for some kind of entry/presel
 *  Arguments
 *    (in) unitData = pointer to unit data
 *    (in) entry = BCM layer entry ID
 *    (out) qualData = where to put pointer to qual data array
 *    (out) qualLimit = where to put max quals in qual data array
 *    (out) stage = where to put the applicable stage ID
 *    (out) bqset = where to put a pointer to the applicable BCM QSET
 *    (out) pqset = where to put a pointer to the applicable PPD QSET
 *    (out) groupTypes = where to put the group types bitmap
 *  Returns
 *    int (implied cast from bcm_error_t)
 *      BCM_E_NONE if the proposed qset is acceptable
 *      BCM_E_* appropriately if not
 */

STATIC int
_bcm_dpp_field_qualifier_set_prep(_bcm_dpp_field_info_t *unitData,
                                  bcm_field_entry_t entry,
                                  _bcm_dpp_field_qual_t **qualData,
                                  unsigned int *qualLimit,
                                  _bcm_dpp_field_stage_idx_t *stage,
                                  bcm_field_qset_t **bqset,
                                  _bcm_dpp_field_qual_set_t **pqset,
                                  uint32 *groupTypes)
{
    _bcm_dpp_field_entry_common_t *common;
    _bcm_dpp_field_entry_common_ext_t *extCommon;
    _bcm_dpp_field_entry_type_t entryType;
    _bcm_dpp_field_group_t *groupData;
    uint32 entryFlags;
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;

    if (_BCM_DPP_FIELD_ENTRY_IS_ENTRY(entry)) {
        /* 'normal' entries are cached and so we use the cache */
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_entry_common_pointer(unitData,
                                                                    entry,
                                                                    &common,
                                                                    &extCommon,
                                                                    &entryType));
        switch (entryType) {
        case _bcmDppFieldEntryTypeInternalTcam:
        case _bcmDppFieldEntryTypeExternalTcam:
            groupData = &(unitData->groupD[_BCM_DPP_FIELD_TCAM_ENTRY_GROUP(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), 
                                                         _BCM_DPP_FIELD_TCAM_LOCAL_ID(unit, entry))]);
            *qualLimit = unitData->stageD[groupData->stage].devInfo->entryMaxQuals;
            entryFlags = _BCM_DPP_FIELD_TCAM_ENTRY_FLAGS(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), 
                                                         _BCM_DPP_FIELD_TCAM_LOCAL_ID(unit, entry));
            *qualData = _BCM_DPP_FIELD_TCAM_ENTRY_QUALS(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), 
                                                         _BCM_DPP_FIELD_TCAM_LOCAL_ID(unit, entry));

            break;
        case _bcmDppFieldEntryTypeDirExt:
            groupData = &(unitData->groupD[(common)->entryGroup]);
            *qualLimit = unitData->stageD[groupData->stage].devInfo->entryDeMaxQuals;
            entryFlags = (common)->entryFlags;
            *qualData = &((common)->entryQual[0]);
            break;
        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d unknown"
                                               " type %d"),
                              unit,
                              entry,
                              entryType));
        } /* switch (entryType) */
        if (0 == (entryFlags & _BCM_DPP_FIELD_ENTRY_IN_USE)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d not in use"),
                              unit,
                              entry));
        } /* if (0 == (common->entryFlags & _BCM_DPP_FIELD_ENTRY_IN_USE)) */
        *stage = groupData->stage;
        if (bqset) {
            *bqset = &(groupData->qset);
        }
        if (pqset) {
            *pqset = &(groupData->pqset);
        }
        if (groupTypes) {
            *groupTypes = groupData->groupTypes;
        }
    } else if (_BCM_DPP_FIELD_ENTRY_IS_PRESEL(entry)) {
        /* preselectors are not cached, so need to fake it for them */
        *qualData = NULL;
        *qualLimit = SOC_PPD_FP_NOF_QUALS_PER_DB_MAX;
        entry = _BCM_DPP_FIELD_PRESEL_FROM_ENTRY(entry);
        if ((0 > entry) || (unitData->preselLimit <= entry)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d presel %d invalid"),
                              unit,
                              entry));
        }
        if (!BCM_FIELD_PRESEL_TEST(unitData->preselInUse, entry)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d presel %d not in use"),
                              unit,
                              entry));
        }
        *qualData = sal_alloc(sizeof(**qualData) * (*qualLimit),
                              "preselector 'set' working space");
        if (!(*qualData)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d unable to allocate %u"
                                               " bytes presel workspace"),
                              unit,
                              (uint32)sizeof(**qualData) * (*qualLimit)));
        }
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_presel_info_get(unitData,
                                                               entry,
                                                               *qualLimit,
                                                               stage,
                                                               *qualData));
        if (bqset) {
            *bqset = &(unitData->stageD[*stage].qsetType[_BCM_DPP_FIELD_MAX_GROUP_TYPES]);
        }
        if (pqset) {
            *pqset = &(unitData->stageD[*stage].ppqset);
        }
        if (groupTypes) {
            *groupTypes = 1 << _BCM_DPP_FIELD_MAX_GROUP_TYPES;
        }
    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d is not valid"),
                          unit,
                          entry));
    }
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *    _bcm_dpp_field_qualifier_set_cleanup
 *  Purpose
 *    Perform any work that needs to be done after updating the qualifiers on
 *    an entry or preselector or whatever else
 *  Arguments
 *    (in) unitData = pointer to unit data
 *    (in) entry = BCM layer entry ID
 *    (in) qualData = new qualifier data array
 *    (in) changed = TRUE if something was changed
 *  Returns
 *    nothing
 */
STATIC int
_bcm_dpp_field_qualifier_set_cleanup(_bcm_dpp_field_info_t *unitData,
                                     bcm_field_entry_t entry,
                                     _bcm_dpp_field_qual_t *qualData,
                                     _bcm_dpp_field_stage_idx_t stage,
                                     int changed)
{
    _bcm_dpp_field_entry_common_t *common;
    _bcm_dpp_field_entry_common_ext_t *extCommon;
    _bcm_dpp_field_entry_type_t entryType;
    int result = BCM_E_NONE;
    int unit;

    unit = unitData->unit;

    if (_BCM_DPP_FIELD_ENTRY_IS_ENTRY(entry)) {
        if (changed) {
            /* update entry and group flags */
            result = _bcm_dpp_field_entry_common_pointer(unitData,
                                                         entry,
                                                         &common,
                                                         &extCommon,
                                                         &entryType);
            /* that call should NOT fail at this point... */
            if (BCM_E_NONE == result) {
                if (entryType == _bcmDppFieldEntryTypeExternalTcam) {
                    /* this entry has changed */
                    extCommon->entryFlags |= _BCM_DPP_FIELD_ENTRY_CHANGED;
                    /* the group contains changed entry/entries */
                    unitData->groupD[extCommon->entryGroup].groupFlags |= _BCM_DPP_FIELD_GROUP_CHG_ENT;
                }
                else { /* internal TCAM / direct extraction */
                    /* this entry has changed */
                    common->entryFlags |= _BCM_DPP_FIELD_ENTRY_CHANGED;
                    /* the group contains changed entry/entries */
                    unitData->groupD[common->entryGroup].groupFlags |= _BCM_DPP_FIELD_GROUP_CHG_ENT;
                }
            }
        } /* if (changed) */
        /* 'normal' entries are manipulated in cache, so no other cleanup */
    } else if (_BCM_DPP_FIELD_ENTRY_IS_PRESEL(entry)) {
        /* presel entries are not cached, so clean up from faking it */
        entry = _BCM_DPP_FIELD_PRESEL_FROM_ENTRY(entry);
        if (qualData) {
            if (changed) {
                result = _bcm_dpp_field_presel_info_set(unitData,
                                                        entry,
                                                        SOC_PPD_FP_NOF_QUALS_PER_DB_MAX,
                                                        stage,
                                                        qualData);
                if (BCM_E_NONE != result) {
                    LOG_ERROR(BSL_LS_BCM_FP,
                              (BSL_META_U(unit,
                                          "unit %d preselector %d write after"
                                          " changes failed: %d (%s)\n"),
                               unit,
                               entry,
                               result,
                               _SHR_ERRMSG(result)));
                }
            } /* if (changed) */
            sal_free(qualData);
        }
    }
    /* final else omitted because it should not matter -- prep errored out */
    return result;
}

int
_bcm_dpp_field_qualifier_set_single(_bcm_dpp_field_info_t *unitData,
                                    bcm_field_entry_t entry,
                                    SOC_PPD_FP_QUAL_TYPE hwType,
                                    const uint64 data,
                                    const uint64 mask,
                                    const uint64 valid)
{
    _bcm_dpp_field_qual_t *qualData;
    _bcm_dpp_field_qual_set_t *pqset;
    _bcm_dpp_field_stage_idx_t stage;
    unsigned int qualLimit;
    int changed = FALSE;
    int result;
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;
	
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_qualifier_set_prep(unitData,
                                                              entry,
                                                              &qualData,
                                                              &qualLimit,
                                                              &stage,
                                                              NULL,
                                                              &pqset,
                                                              NULL));
    if (!SHR_BITGET(*pqset, hwType)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d hardware"
                                           " qualifier set does"
                                           " not include %d (%s)"),
                          unit,
                          entry,
                          hwType,
                          SOC_PPD_FP_QUAL_TYPE_to_string(hwType)));
    } /* if (!SHR_BITGET(*pqset, hwType)) */
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_qualifier_set_single_int(unitData,
                                                                    entry,
                                                                    qualData,
                                                                    qualLimit,
                                                                    hwType,
                                                                    data,
                                                                    mask,
                                                                    valid));
    /* if we get to here, the entry was changed */
    changed = TRUE;
exit:
    result = _bcm_dpp_field_qualifier_set_cleanup(unitData,
                                                  entry,
                                                  qualData,
                                                  stage,
                                                  changed);
    if (BCM_E_NONE != result) {
        _rv = result;
    }
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *     _bcm_dpp_field_qualifier_set_int
 *  Purpose
 *     Set a qualifier to an entry
 *  Parameters
 *     (in) unitData = unit information
 *     (in) entry = entry ID
 *     (in) stage = stage in which the entry exists
 *     (in/out) qualData = pointer to the entry qualifier data
 *     (in) qualLimit = max qualifiers for this type of entry
 *     (in) bcmQual = BCM qualifier type
 *     (in) ppdQual = pointer to appropriate PPD qualifier chain
 *     (in) count = number of elements in qualifier data/mask arrays
 *     (in) data = pointer to array of qualifier data
 *     (in) mask = pointer to array of qualifier mask
 *     (out) changed = poitner to where to put the 'changed' indicator
 *  Returns
 *     int (implied cast from bcm_error_t)
 *        BCM_E_NONE if successful
 *        BCM_E_* appropriately if not
 *  Notes
 *     If the qualifier mask and data are zero, and the qualifier exists, will
 *     remove the qualifier from the entry.
 *
 *     If the qualifier mask or data is nonzero, and the qualifier exists, this
 *     will update the mask and data values for the qualifier.  If the
 *     qualifier mask is nonzero and the qualifier does not exist, this will
 *     add the qualifier.
 *
 *     Does not return error at bits being set in data that are not set in
 *     mask, but it does emit a warning.  It does return BCM_E_PARAM at bits
 *     being in mask or data that are not valid for the qualifier.
 *
 *     Caller must already hold the unit lock.
 *
 *     This manipulates BCM layer qualifiers, so will shift left if needed
 *     (padding with zeroes) to align BCM LSb to appropriate bit of PPD, plus
 *     it supports complex BCM qualifiers (those made of more than one PPD
 *     qualifier)  by slicing the bits into appropriate parts for use as PPD
 *     layer qualifiers.
 *
 *     Bits added to the LSb end to move a qualifier left before applying it
 *     are not considered significant, but all bits from the least significant
 *     bit of the argument and going leftward are significant, even if they are
 *     not included (so if setting a qualifier that requires array of two and
 *     only one is provided, it is as if providing data=0 and mask=0 for the
 *     missing upper element of the array).
 *
 *     The array is little-endian with 64b grains: least significant octbyte is
 *     array[0], then next more significant is array[1], and so on.  Within
 *     array elements, the data are in machine-native order.
 */
STATIC int
_bcm_dpp_field_qualifier_set_int(_bcm_dpp_field_info_t *unitData,
                                 bcm_field_entry_t entry,
                                 _bcm_dpp_field_stage_idx_t stage,
                                 _bcm_dpp_field_qual_t *qualData,
                                 unsigned int qualLimit,
                                 bcm_field_qualify_t bcmQual,
                                 const SOC_PPD_FP_QUAL_TYPE *ppdQual,
                                 int count,
                                 const uint64 *data,
                                 const uint64 *mask,
                                 int *changed)
{
    uint64 edata0, edata1;
    uint64 emask0, emask1;
    uint64 evalid0, evalid1;
    uint64 etemp;
    unsigned int *len = NULL;
    int index;
    int length;
    int bias;
    int dist;
    int shift;
    int offset;
    int result;
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit; 

    result = BCM_E_NONE;
    *changed = FALSE;
    len = sal_alloc(sizeof(unsigned int)*unitData->devInfo->qualChain, "_bcm_dpp_field_qualifier_set_int");
    if (len == NULL) {
         BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY,
                          (_BSL_BCM_MSG_NO_UNIT("Alocation failed")));

    }
    /* collect the lengths for all of the PPD qualifiers in the chain */
    for (index = 0, length = 0;
         (BCM_E_NONE == result) &&
         (index < unitData->devInfo->qualChain);
         index++) {
        if (_BCM_DPP_FIELD_PPD_QUAL_VALID(ppdQual[index])) {
            result = _bcm_dpp_ppd_qual_bits(unitData,
                                            stage,
                                            ppdQual[index],
                                            &(len[index]),
                                            NULL,
                                            NULL);
            length += len[index];
        }
    }
    BCMDNX_IF_ERR_EXIT_MSG(result,
                        (_BSL_BCM_MSG_NO_UNIT("was unable to query qualifier %d"
                                          " (%s) chain %d (PPD qual %d (%s))"
                                          " length for unit %d stage %u:"
                                          " %d (%s)"),
                         bcmQual,
                         _bcm_dpp_field_qual_name[bcmQual],
                         index,
                         ppdQual[index],
                         SOC_PPD_FP_QUAL_TYPE_to_string(ppdQual[index]),
                         unit,
                         stage,
                         result,
                         _SHR_ERRMSG(result)));
    /*
     *  If that worked, we have the PPD claimed length for each of the
     *  qualifiers in the chain that will be used for this BCM layer qualifier.
     *  We also have the total PPD qualifier chain length.
     *
     *  We want to verify the settings, though, and allow emulated and shifted
     *  and other features, to map the qualifiers in a more versatile manner.
     *  To this end, the table also specifies a length.  We will use that
     *  length to verify the arguments, unless it says to not do so or it says
     *  to specifically use the PPD layer length.
     *
     *  The intent is that the BCM layer will explicitly provide a length for
     *  almost all qualifiers.  This comes from the idea that we want the
     *  versatile mapping and some sanity checking as we go.
     *
     *  However, there are cases where we do not want to bother validating the
     *  arguments here.  Mostly these would be cases where the function
     *  immediately (or nearly so) up the call stack has already performed this
     *  validation, such as the case for a translated qualifier, where a
     *  qualifier is mapped from BCM layer constant values to PPD layer
     *  constant values (look for bcmFieldQualifyColor as an example).  This is
     *  achieved by setting zero as the BCM layer length in the table.
     *
     *  Another exception to using the BCM layer bit count is when that count
     *  will be inconsistent between stages, or can be adjusted by some kind of
     *  configuration setting.  An example of this is the chained key feature
     *  that allows data to be passed from one field group to another.  In this
     *  case, the validation is to be done using the PPD layer qualifier
     *  lengths instead of the constant from the table.  This is achieved by
     *  setting -1 as the BCM layer length in the table.
     */
    if (0 == unitData->qualMaps[bcmQual][_BCM_DPP_FIELD_QUALMAP_OFFSET_LENGTH]) {
        /* caller must ensure values fit within bit width limits */
        length = 0;
    } else if (0 < unitData->qualMaps[bcmQual][_BCM_DPP_FIELD_QUALMAP_OFFSET_LENGTH]) {
        /* use BCM layer width */
        length = unitData->qualMaps[bcmQual][_BCM_DPP_FIELD_QUALMAP_OFFSET_LENGTH];
    } /* else just keep the computed length above */
    shift = unitData->qualMaps[bcmQual][_BCM_DPP_FIELD_QUALMAP_OFFSET_SHIFT];
    LOG_DEBUG(BSL_LS_BCM_FP,
              (BSL_META_U(unit,
                          "unit %d entry %d qual %s (%d):"
                          " length %d bits, shift %d\n"),
               unit,
               entry,
               _bcm_dpp_field_qual_name[bcmQual],
               bcmQual,
               length,
               shift));
    /*
     *  Before starting on the commit of the new/updated qualifier, we need to
     *  make sure the data and mask are within the constraints of the API.
     *
     *  At this point, all of the data/mask bits are pushed far right, so we
     *  only need to count bits until we get to the end of the qualifier as the
     *  API expects, and then make sure the last octbyte does not contain any
     *  unexpected bits.
     *
     *  We only check this is the arguments should have come from the
     *  applicatin; for translated or otherwise special types, this is skipped.
     *
     *  In these cases, the function that provides the translated values must
     *  verify its inbound arguments and provide sensible data here.  We assume
     *  here that all of the provided bits matter, but will quietly discard
     *  excess bits (those not consumed by the hardware qualifiers) later.
     */
    if (length) {
        /* arguments directly from application; verify their sizes */
        if (shift > 0) {
            /* argument will be shifted right; bits below LSb are invalid */
            offset = shift;
        } else {
            /* argument will be shifted left or unshifted; LSb is bit zero */
            offset = 0;
        }
        for (index = 0, dist = 0; index < count; index++, dist += 64) {
            /* assume all bits are valid */
            COMPILER_64_SET(etemp, 0, 0);
            if ((dist + 63) < offset) {
                /* this octbyte is below the LSb */
                COMPILER_64_SET(etemp, ~0, ~0);
            } else if (dist < offset) {
                /* this octbyte contains bits below the LSb */
                COMPILER_64_SET(evalid0, 0, 1);
                COMPILER_64_SHL(evalid0, offset - dist);
                COMPILER_64_SUB_32(evalid0, 1);
                COMPILER_64_OR(etemp, evalid0);
            }
            if (dist >= (offset + length)) {
                /* this octbyte is above the MSb */
                COMPILER_64_SET(etemp, ~0, ~0);
            } else if ((dist + 64) > (offset + length)) {
                /* this octbyte contains bits above the MSb */
                COMPILER_64_SET(evalid0, ~0, ~0);
                COMPILER_64_SHL(evalid0, (offset + length) - dist);
                COMPILER_64_OR(etemp, evalid0);
            }
            /* at this point, etemp contains invalid bits for this octbyte */
            edata0 = data[index];
            emask0 = mask[index];
            COMPILER_64_AND(edata0, etemp);
            COMPILER_64_AND(emask0, etemp);
            if (!COMPILER_64_IS_ZERO(edata0)) {
                LOG_ERROR(BSL_LS_BCM_FP,
                          (BSL_META_U(unit,
                                      "max bits for qualifier %d (%s) is %d"
                                      " for unit %d entry %d; provided data"
                                      " value %d %08X%08X has extra bit(s)"
                                      " %08X%08X set\n"),
                           bcmQual,
                           _bcm_dpp_field_qual_name[bcmQual],
                           length,
                           unit,
                           entry,
                           index,
                           COMPILER_64_HI(data[index]),
                           COMPILER_64_LO(data[index]),
                           COMPILER_64_HI(edata0),
                           COMPILER_64_LO(edata0)));
            }
            if (!COMPILER_64_IS_ZERO(emask0)) {
                LOG_ERROR(BSL_LS_BCM_FP,
                          (BSL_META_U(unit,
                                      "max bits for qualifier %d (%s) is %d"
                                      " for unit %d entry %d; provided mask"
                                      " value %d %08X%08X has extra bit(s)"
                                      " %08X%08X set\n"),
                           bcmQual,
                           _bcm_dpp_field_qual_name[bcmQual],
                           length,
                           unit,
                           entry,
                           index,
                           COMPILER_64_HI(mask[index]),
                           COMPILER_64_LO(mask[index]),
                           COMPILER_64_HI(emask0),
                           COMPILER_64_LO(emask0)));
            }
            if (!COMPILER_64_IS_ZERO(edata0) || !COMPILER_64_IS_ZERO(emask0)) {
                BCM_ERR_EXIT_NO_MSG(BCM_E_PARAM);
            }
        } /* for (index = 0, dist = 0; index < count; index++, dist += 64) */
    } else { /* if (length) */
        /* translated arguments; assume all bits matter for now */
        length = count * 64;
    } /* if (length) */
    /*
     *  The arguments did not contain bogus bits; can keep going...
     */
    for (index = 0, offset = -1, dist = -64; /* qual #0, source octbyte #-1 */
         (index < unitData->devInfo->qualChain) &&
         (BCM_E_NONE == result);
         /* increment inline */) {
        if (shift > (dist + 63)) {
            /* look at next source octbyte */
            dist += 64;
            offset++;
            continue;
        }
        if (!_BCM_DPP_FIELD_PPD_QUAL_VALID(ppdQual[index])) {
            break;
        }
        if (0 > offset) {
            COMPILER_64_ZERO(edata0);
            COMPILER_64_ZERO(emask0);
            COMPILER_64_ZERO(evalid0);
        } else if (offset < count) {
            edata0 = data[offset];
            emask0 = mask[offset];
            COMPILER_64_SET(evalid0, ~0, ~0);
        } else {
            COMPILER_64_ZERO(edata0);
            COMPILER_64_ZERO(emask0);
            COMPILER_64_SET(evalid0, ~0, ~0);
        }
        if (offset >= (count - 1)) {
            COMPILER_64_ZERO(edata1);
            COMPILER_64_ZERO(emask1);
            COMPILER_64_SET(evalid1, ~0, ~0);
        } else {
            edata1 = data[offset + 1];
            emask1 = mask[offset + 1];
            COMPILER_64_SET(evalid1, ~0, ~0);
        }
        bias = shift - dist;
        if (bias) {
            /* will need mask value since SHR is ASR not LSR */
            COMPILER_64_SET(etemp, ~0, ~0);
            COMPILER_64_SHL(etemp, 64 - bias);
            COMPILER_64_NOT(etemp);
            /* shift current octbyte right by the needed distance */
            COMPILER_64_SHR(edata0, bias);
            COMPILER_64_SHR(emask0, bias);
            COMPILER_64_SHR(evalid0, bias);
            /* mask off dragged 'sign' bit (despite being unsigned value) */
            COMPILER_64_AND(edata0, etemp);
            COMPILER_64_AND(emask0, etemp);
            COMPILER_64_AND(evalid0, etemp);
            /* shift next octbyte left by (64 - the needed distance) */
            COMPILER_64_SHL(edata1, 64 - bias);
            COMPILER_64_SHL(emask1, 64 - bias);
            COMPILER_64_SHL(evalid1, 64 - bias);
            /* merge the two octbytes */
            COMPILER_64_OR(edata0, edata1);
            COMPILER_64_OR(emask0, emask1);
            COMPILER_64_OR(evalid0, evalid1);
        } else {
            COMPILER_64_ZERO(etemp);
        }
        if (length < (64 - bias)) {
            /* remaining valid bits fit in this octbyte; mask off unwanted */
            COMPILER_64_NOT(etemp);
            COMPILER_64_SHL(etemp, length);
            COMPILER_64_NOT(etemp);
            COMPILER_64_AND(edata0, etemp);
            COMPILER_64_AND(emask0, etemp);
            COMPILER_64_AND(evalid0, etemp);
        }
        /* now adjust the base values to lie within this qualifier */
        if (64 > len[index]) {
            COMPILER_64_SET(etemp, ~0, ~0);
            COMPILER_64_SHL(etemp, len[index]);
            COMPILER_64_NOT(etemp);
            COMPILER_64_AND(edata0, etemp);
            COMPILER_64_AND(emask0, etemp);
            COMPILER_64_AND(evalid0, etemp);
        }
        result = _bcm_dpp_field_qualifier_set_single_int(unitData,
                                                         entry,
                                                         qualData,
                                                         qualLimit,
                                                         ppdQual[index],
                                                         edata0,
                                                         emask0,
                                                         evalid0);
        if (BCM_E_NONE == result) {
            /* note that the entry has been changed */
            *changed = TRUE;
        } else {
            /* stop on errors */
            _rv = result;
            break;
        }
        /* remove this qualifier's bits from what we have to do */
        shift += len[index];
        length -= len[index];
        /* if LSb now in next octbyte, work from that point */
        if (((dist + len[index]) >> 6) != (dist >> 6)) {
            offset++;
            dist += 64;
        }
        /* move to next qualifier in chain */
        index++;
    } /* for (all qualifiers in chain as long as no error) */
    if (unitData->qualMaps[bcmQual][_BCM_DPP_FIELD_QUALMAP_OFFSET_LENGTH]) {
        /* only check this if non-translated arguments */
        if (length > 0) {
            LOG_WARN(BSL_LS_BCM_FP,
                     (BSL_META_U(unit,
                                 "still have %d bits left over!\n"),
                      length));
        }
    }
exit:
    if (len) {
        sal_free(len);
    }
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_field_qualifier_set(_bcm_dpp_field_info_t *unitData,
                             bcm_field_entry_t entry,
                             bcm_field_qualify_t bcmQual,
                             int count,
                             const uint64 *data,
                             const uint64 *mask)
{
    _bcm_dpp_field_qual_t *qualData;
    bcm_field_qset_t *bqset;
    SOC_PPD_FP_QUAL_TYPE *ppdQual;
    _bcm_dpp_field_stage_idx_t stage;
    unsigned int qualLimit;
    uint32 groupTypes;
    int changed = FALSE;
    int result;
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;
	
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_qualifier_set_prep(unitData,
                                                              entry,
                                                              &qualData,
                                                              &qualLimit,
                                                              &stage,
                                                              &bqset,
                                                              NULL,
                                                              &groupTypes));
    if (bcmFieldQualifyCount > bcmQual) {
        /* standard BCM qualifier */
        if (!BCM_FIELD_QSET_TEST(unitData->unitQset, bcmQual)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d does not support"
                                               " qualifier %d (%s)"),
                              unit,
                              bcmQual,
                              _bcm_dpp_field_qual_name[bcmQual]));
        }
        if (!BCM_FIELD_QSET_TEST(*bqset, bcmQual)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d QSET does not"
                                               " include %d (%s)"),
                              unit,
                              entry,
                              bcmQual,
                              _bcm_dpp_field_qual_name[bcmQual]));
        } /* if (!BCM_FIELD_QSET_TEST(gropData->qset, type)) */
    } else { /* if (bcmFieldQualifyCount > type) */
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG_NO_UNIT("invalid BCM qualifier type %d"),
                          bcmQual));
    } /* if (bcmFieldQualifyCount > type) */
    /* get the qualifier mapping */
    result = _bcm_dpp_ppd_qual_from_bcm_qual(unitData,
                                             stage,
                                             groupTypes,
                                             bcmQual,
                                             &ppdQual);
    BCMDNX_IF_ERR_EXIT_MSG(result,
                        (_BSL_BCM_MSG_NO_UNIT("was unable to map qualifier %d (%s)"
                                          " to PPD for unit %d entry %d (stage"
                                          " %u types %08X: %d (%s)"),
                         bcmQual,
                         _bcm_dpp_field_qual_name[bcmQual],
                         unit,
                         entry,
                         stage,
                         groupTypes,
                         result,
                         _SHR_ERRMSG(result)));
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_qualifier_set_int(unitData,
                                                             entry,
                                                             stage,
                                                             qualData,
                                                             qualLimit,
                                                             bcmQual,
                                                             ppdQual,
                                                             count,
                                                             data,
                                                             mask,
                                                             &changed));

exit:
    result = _bcm_dpp_field_qualifier_set_cleanup(unitData,
                                                  entry,
                                                  qualData,
                                                  stage,
                                                  changed);
    if (BCM_E_NONE != result) {
        _rv = result;
    }
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_field_entry_qualifier_general_set_int(_bcm_dpp_field_info_t *unitData,
                                               bcm_field_entry_t entry,
                                               bcm_field_qualify_t bcmQual,
                                               int count,
                                               const uint64 *data,
                                               const uint64 *mask)
{
    bcm_field_presel_t presel;
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;
    
    if ((0 > bcmQual) || (bcmFieldQualifyCount <= bcmQual)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG_NO_UNIT("qualifier type %d not valid"),
                          bcmQual));
    }
    if (_BCM_DPP_FIELD_ENTRY_IS_PRESEL(entry)) {
        /*
         *  There are special cases for preselectors (bcmFieldQualifyStage;
         *  bcmFieldQualifyHeaderFormatSet on PetraB, and maybe others), so
         *  give the preselector code a chance to filter the request.  If the
         *  device supports the qualifier as a 'special' case, it will be
         *  handled by _bcm_petra_field_presel_qualify, but if it should be a
         *  'general' case, _bcm_petra_field_presel_qualify will in turn call
         *  _bcm_dpp_field_qualifier_set to do the actual work.
         */
        presel = _BCM_DPP_FIELD_PRESEL_FROM_ENTRY(entry);
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_presel_qualify(unitData,
                                                              presel,
                                                              bcmQual,
                                                              count,
                                                              data,
                                                              mask));
    } else {
        if (unitData->qualMaps[bcmQual]) {
            /* this unit supports this qualifier */
            BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_qualifier_set(unitData,
                                                                 entry,
                                                                 bcmQual,
                                                                 count,
                                                                 data,
                                                                 mask));
        } else { /* if (unitData->qualMaps[bcmQual]) */
            /* this unit does not support this qualifier */
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d does not support"
                                               "qualifier %d (%s)"),
                              unit,
                              bcmQual,
                              _bcm_dpp_field_qual_name[bcmQual]));
        } /* if (unitData->qualMaps[bcmQual]) */
    }
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *    _bcm_dpp_field_qualifier_get_single_int
 *  Purpose
 *    Get a single qualifier on an entry
 *  Arguments
 *    (in) unitData = unit information
 *    (in) entry = entry ID
 *    (in) qual = pointer to qualifier array for this entry
 *    (in) qualLimit = max qualifier index applicable to this entry
 *    (in) hwType = PPD layer qualifier to add to the entry
 *    (in) qualHwData = pointer to hardware qualifier data
 *    (out) data = where to put data value
 *    (out) mask = where to put mask value
 *  Returns
 *    bcm_error_t cast as int
 *      BCM_E_NONE if success
 *      BCM_E_* otherwise as appropriate
 *  Notes
 *    Caller must already hold the unit lock.
 *
 *    Assumes entry exists in a group.
 *
 *    Caller can choose what bits are desired after return.
 */
STATIC int
_bcm_dpp_field_qualifier_get_single_int(_bcm_dpp_field_info_t *unitData,
                                        bcm_field_entry_t entry,
                                        const _bcm_dpp_field_qual_t *qual,
#if _BCM_DPP_FIELD_GET_USE_PPD
                                        const SOC_PPD_FP_QUAL_VAL *qualHwData,
#endif /* _BCM_DPP_FIELD_GET_USE_PPD */
                                        unsigned int qualLimit,
                                        SOC_PPD_FP_QUAL_TYPE hwType,
                                        uint64 *data,
                                        uint64 *mask)
{
    unsigned int index;
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;

#if _BCM_DPP_FIELD_GET_USE_PPD
    if (qualHwData) {
        /* using the hardware view */
        for (index = 0;
             (index < qualLimit) && (qualHwData[index].type != hwType);
             index++) {
            /* look until scanned all or hit requested qual */
        }
        if (index < qualLimit) {
            COMPILER_64_SET(*data, qualHwData[index].val.arr[1],
                                     qualHwData[index].val.arr[0]);
            COMPILER_64_SET(*mask, qualHwData[index].is_valid.arr[1],
                                     qualHwData[index].is_valid.arr[0]);
        }
    } else { /* if (qualHwData) */
        /* using the cached state */
#endif /* _BCM_DPP_FIELD_GET_USE_PPD */
        for (index = 0;
             (index < qualLimit) && (qual[index].hwType != hwType);
             index++) {
            /* look until scanned all or hit requested qual */
        } /* for (all possible qualifier slots this stage) */
        if (index < qualLimit) {
            *data = qual[index].qualData;
            *mask = qual[index].qualMask;
        }
#if _BCM_DPP_FIELD_GET_USE_PPD
    } /* if (qualHwData) */
#endif /* _BCM_DPP_FIELD_GET_USE_PPD */
    if (index >= qualLimit) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d does not have"
                                           " hw qualifier %d (%s)"),
                          unit,
                          entry,
                          hwType,
                          SOC_PPD_FP_QUAL_TYPE_to_string(hwType)));
    }
exit:
    LOG_DEBUG(BSL_LS_BCM_FP,
              (BSL_META_U(unit,
                          "(%d,%d,%d;%s(%d;%s),&(%08X%08X),&(%08X%08X)):"
                          " %d (%s)\n"),
               unit,
               entry,
               hwType,
               SOC_PPD_FP_QUAL_TYPE_to_string(hwType),
               unitData->ppdQual[hwType],
               (bcmFieldQualifyCount>unitData->ppdQual[hwType])?_bcm_dpp_field_qual_name[unitData->ppdQual[hwType]]:"-",
               COMPILER_64_HI(*data),
               COMPILER_64_LO(*data),
               COMPILER_64_HI(*mask),
               COMPILER_64_LO(*mask),
               _rv,
               _SHR_ERRMSG(_rv)));
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *    _bcm_dpp_field_qualifier_get_prep
 *  Purpose
 *    Get (and check) hardware values for qualifiers on an entry
 *  Arguments
 *    (in) unitData = unit information
 *    (in) entry = entry ID
 *    (out) qualData = where to put pointer to qual data array
 *    (out) qualLimit = where to put max quals in qual data array
 *    (out) stage = where to put the applicable stage ID
 *    (out) bqset = where to put a pointer to the applicable BCM QSET
 *    (out) pqset = where to put a pointer to the applicable PPD QSET
 *    (out) groupTypes = where to put the group types bitmap
 *    (out) qualHwData = where to put hardware qualifier data
 *    (out) useData = where to put boolean indicating whether to use hw quals
 *  Returns
 *    bcm_error_t cast as int
 *      BCM_E_NONE if success
 *      BCM_E_* otherwise as appropriate
 *  Notes
 *    Assumes entry is valid, lock is held.
 *
 *    The value at useData is TRUE if and only if the entry is valid, in
 *    hardware, and has not been updated since it was committed to hardware.
 */

STATIC int
_bcm_dpp_field_qualifier_get_prep(_bcm_dpp_field_info_t *unitData,
                                  bcm_field_entry_t entry,
                                  _bcm_dpp_field_qual_t **qualData,
                                  unsigned int *qualLimit,
                                  _bcm_dpp_field_stage_idx_t *stage,
                                  bcm_field_qset_t **bqset,
                                  _bcm_dpp_field_qual_set_t **pqset,
                                  uint32 *groupTypes
#if _BCM_DPP_FIELD_GET_VERIFY_PPD || _BCM_DPP_FIELD_GET_USE_PPD
                                  ,SOC_PPD_FP_QUAL_VAL *qualHwData,
                                  int *useData
#endif /* _BCM_DPP_FIELD_GET_VERIFY_PPD || _BCM_DPP_FIELD_GET_USE_PPD */
                                  )
{
#if _BCM_DPP_FIELD_GET_VERIFY_PPD || _BCM_DPP_FIELD_GET_USE_PPD
    SOC_PPD_FP_ENTRY_INFO *entInfoTc = NULL;
    SOC_PPD_FP_DIR_EXTR_ENTRY_INFO *entInfoDe = NULL;
#endif /* _BCM_DPP_FIELD_GET_VERIFY_PPD || _BCM_DPP_FIELD_GET_USE_PPD */
    _bcm_dpp_field_entry_common_t *common;
    _bcm_dpp_field_entry_common_ext_t *extCommon;
    _bcm_dpp_field_group_t *groupData;
    _bcm_dpp_field_entry_type_t entryType;
    uint32 entryFlags;
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;

    if (_BCM_DPP_FIELD_ENTRY_IS_ENTRY(entry)) {
        /* 'normal' entries are cached and so we use the cache */
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_entry_common_pointer(unitData,
                                                                    entry,
                                                                    &common,
                                                                    &extCommon,
                                                                    &entryType));
        switch (entryType) {
        case _bcmDppFieldEntryTypeInternalTcam:
        case _bcmDppFieldEntryTypeExternalTcam:
            groupData = &(unitData->groupD[_BCM_DPP_FIELD_TCAM_ENTRY_GROUP(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), 
                                                         _BCM_DPP_FIELD_TCAM_LOCAL_ID(unit, entry))]);
            entryFlags = _BCM_DPP_FIELD_TCAM_ENTRY_FLAGS(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), 
                                                         _BCM_DPP_FIELD_TCAM_LOCAL_ID(unit, entry));
            *qualLimit = unitData->stageD[groupData->stage].devInfo->entryMaxQuals;
            *qualData = _BCM_DPP_FIELD_TCAM_ENTRY_QUALS(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), 
                                                         _BCM_DPP_FIELD_TCAM_LOCAL_ID(unit, entry));
#if _BCM_DPP_FIELD_GET_VERIFY_PPD || _BCM_DPP_FIELD_GET_USE_PPD
            entInfoTc = sal_alloc(sizeof(*entInfoTc), "TCAM entry temp");
#endif /* _BCM_DPP_FIELD_GET_VERIFY_PPD || _BCM_DPP_FIELD_GET_USE_PPD */
            break;
        case _bcmDppFieldEntryTypeDirExt:
            groupData = &(unitData->groupD[common->entryGroup]);
            entryFlags = common->entryFlags;
            *qualLimit = unitData->stageD[groupData->stage].devInfo->entryDeMaxQuals;
            *qualData = &(common->entryQual[0]);
#if _BCM_DPP_FIELD_GET_VERIFY_PPD || _BCM_DPP_FIELD_GET_USE_PPD
            entInfoDe = sal_alloc(sizeof(*entInfoDe), "Dir Ext entry temp");
#endif /* _BCM_DPP_FIELD_GET_VERIFY_PPD || _BCM_DPP_FIELD_GET_USE_PPD */
            break;
        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d unknown"
                                               " type %d"),
                              unit,
                              entry,
                              entryType));
        } /* switch (entryType) */
        if (0 == (entryFlags & _BCM_DPP_FIELD_ENTRY_IN_USE)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d not in use"),
                              unit,
                              entry));
        } /* if (0 == (common->entryFlags & _BCM_DPP_FIELD_ENTRY_IN_USE)) */
        if (stage) {
            *stage = groupData->stage;
        }
        if (bqset) {
            *bqset = &(groupData->qset);
        }
        if (pqset) {
            *pqset = &(groupData->pqset);
        }
        if (groupTypes) {
            *groupTypes = groupData->groupTypes;
        }
#if _BCM_DPP_FIELD_GET_VERIFY_PPD || _BCM_DPP_FIELD_GET_USE_PPD
        *useData = FALSE;
        if ((entryFlags & (_BCM_DPP_FIELD_ENTRY_IN_HW |
                                   _BCM_DPP_FIELD_ENTRY_CHANGED |
                                   _BCM_DPP_FIELD_ENTRY_NEW)) ==
             (_BCM_DPP_FIELD_ENTRY_IN_HW)) {
            if (entInfoTc) {
                BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_tcam_hardware_entry_check(unitData,
                                                                                 entry,
                                                                                 entInfoTc));
                sal_memcpy(qualHwData,
                           &(entInfoTc->qual_vals[0]),
                           sizeof(*qualHwData) * SOC_PPD_FP_NOF_QUALS_PER_DB_MAX);
                *useData = TRUE;
            }
            if (entInfoDe) {
                BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_dir_ext_hardware_entry_check(unitData,
                                                                                    entry,
                                                                                    entInfoDe));
                sal_memcpy(qualHwData,
                           &(entInfoDe->qual_vals[0]),
                           sizeof(*qualHwData) * SOC_PPD_FP_NOF_QUALS_PER_DB_MAX);
                *useData = TRUE;
            }
        } /* if (entry is in hardware, and not changed, and not new) */
#endif /* _BCM_DPP_FIELD_GET_VERIFY_PPD || _BCM_DPP_FIELD_GET_USE_PPD */
    } else if (_BCM_DPP_FIELD_ENTRY_IS_PRESEL(entry)) {
#if _BCM_DPP_FIELD_GET_VERIFY_PPD || _BCM_DPP_FIELD_GET_USE_PPD
        *useData = FALSE;
#endif /* _BCM_DPP_FIELD_GET_VERIFY_PPD || _BCM_DPP_FIELD_GET_USE_PPD */
        /* preselectors are not cached, so need to fake it for them */
        *qualData = NULL;
        *qualLimit = SOC_PPD_FP_NOF_QUALS_PER_DB_MAX;
        entry = _BCM_DPP_FIELD_PRESEL_FROM_ENTRY(entry);
        if ((0 > entry) || (unitData->preselLimit <= entry)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d presel %d invalid"),
                              unit,
                              entry));
        }
        if (!BCM_FIELD_PRESEL_TEST(unitData->preselInUse, entry)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d presel %d not in use"),
                              unit,
                              entry));
        }
        *qualData = sal_alloc(sizeof(**qualData) * SOC_PPD_FP_NOF_QUALS_PER_DB_MAX,
                              "preselector 'set' working space");
        if (!(*qualData)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d unable to allocate %u"
                                               " bytes presel workspace"),
                              unit,
                              (uint32)sizeof(*qualData) * SOC_PPD_FP_NOF_QUALS_PER_DB_MAX));
        }
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_presel_info_get(unitData,
                                                               entry,
                                                               *qualLimit,
                                                               stage,
                                                               *qualData));
        if (bqset) {
            *bqset = &(unitData->stageD[*stage].qsetType[_BCM_DPP_FIELD_MAX_GROUP_TYPES]);
        }
        if (pqset) {
            *pqset = &(unitData->stageD[*stage].ppqset);
        }
        if (groupTypes) {
            *groupTypes = 1 << _BCM_DPP_FIELD_MAX_GROUP_TYPES;
        }
    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d is not valid"),
                          unit,
                          entry));
    }
exit:
#if _BCM_DPP_FIELD_GET_VERIFY_PPD || _BCM_DPP_FIELD_GET_USE_PPD
    if (entInfoTc) {
        sal_free(entInfoTc);
    }
    if (entInfoDe) {
        sal_free(entInfoDe);
    }
#endif /* _BCM_DPP_FIELD_GET_VERIFY_PPD || _BCM_DPP_FIELD_GET_USE_PPD */
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *    _bcm_dpp_field_qualifier_get_cleanup
 *  Purpose
 *    Perform any work that needs to be done after reading the qualifiers on an
 *    entry or preselector or whatever else
 *  Arguments
 *    (in) unitData = pointer to unit data
 *    (in) entry = BCM layer entry ID
 *    (in) qualData = new qualifier data array
 *  Returns
 *    nothing
 */
STATIC void
_bcm_dpp_field_qualifier_get_cleanup(_bcm_dpp_field_info_t *unitData,
                                     bcm_field_entry_t entry,
                                     _bcm_dpp_field_qual_t *qualData)
{
    if (_BCM_DPP_FIELD_ENTRY_IS_ENTRY(entry)) {
        /* 'normal' entries are manipulated in cache, so no other cleanup */
    } else if (_BCM_DPP_FIELD_ENTRY_IS_PRESEL(entry)) {
        /* presel entries are not cached, so clean up from faking it */
        entry = _BCM_DPP_FIELD_PRESEL_FROM_ENTRY(entry);
        if (qualData) {
            sal_free(qualData);
        }
    }
    /* final else omitted because it should not matter -- prep errored out */
}

int
_bcm_dpp_field_qualifier_get_single(_bcm_dpp_field_info_t *unitData,
                                    bcm_field_entry_t entry,
                                    SOC_PPD_FP_QUAL_TYPE hwType,
                                    uint64 *data,
                                    uint64 *mask)
{
    _bcm_dpp_field_qual_t *qualData;
    _bcm_dpp_field_qual_set_t *pqset;
    unsigned int qualLimit;
#if _BCM_DPP_FIELD_GET_VERIFY_PPD || _BCM_DPP_FIELD_GET_USE_PPD
    SOC_PPD_FP_QUAL_VAL qualHwData[SOC_PPD_FP_NOF_QUALS_PER_DB_MAX];
    int useData;
#endif /* _BCM_DPP_FIELD_GET_VERIFY_PPD || _BCM_DPP_FIELD_GET_USE_PPD */
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;

#if _BCM_DPP_FIELD_GET_VERIFY_PPD || _BCM_DPP_FIELD_GET_USE_PPD
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_qualifier_get_prep(unitData,
                                                              entry,
                                                              &qualData,
                                                              &qualLimit,
                                                              NULL,
                                                              NULL,
                                                              &pqset,
                                                              NULL,
                                                              &(qualHwData[0]),
                                                              &useData));
#else /* _BCM_DPP_FIELD_GET_VERIFY_PPD || _BCM_DPP_FIELD_GET_USE_PPD */
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_qualifier_get_prep(unitData,
                                                              entry,
                                                              &qualData,
                                                              &qualLimit,
                                                              NULL,
                                                              NULL,
                                                              &pqset,
                                                              NULL));
#endif /* _BCM_DPP_FIELD_GET_VERIFY_PPD || _BCM_DPP_FIELD_GET_USE_PPD */
    if (!SHR_BITGET(*pqset, hwType)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d hardware"
                                           " qualifier set does"
                                           " not include %d (%s)"),
                          unit,
                          entry,
                          hwType,
                          SOC_PPD_FP_QUAL_TYPE_to_string(hwType)));
    } /* if (!SHR_BITGET(*pqset, hwType)) */
#if _BCM_DPP_FIELD_GET_VERIFY_PPD || _BCM_DPP_FIELD_GET_USE_PPD
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_qualifier_get_single_int(unitData,
                                                                    entry,
                                                                    qualData,
                                                                    useData?(&(qualHwData[0])):NULL,
                                                                    qualLimit,
                                                                    hwType,
                                                                    data,
                                                                    mask));
#else /* _BCM_DPP_FIELD_GET_VERIFY_PPD || _BCM_DPP_FIELD_GET_USE_PPD */
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_qualifier_get_single_int(unitData,
                                                                    entry,
                                                                    qualData,
                                                                    qualLimit,
                                                                    hwType,
                                                                    data,
                                                                    mask));
#endif /* _BCM_DPP_FIELD_GET_VERIFY_PPD || _BCM_DPP_FIELD_GET_USE_PPD */
exit:
    _bcm_dpp_field_qualifier_get_cleanup(unitData,
                                         entry,
                                         qualData);
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *     _bcm_dpp_field_qualifier_get_int
 *  Purpose
 *     Get a qualifier from an entry
 *  Parameters
 *     (in) unitData = unit information
 *     (in) entry = entry ID
 *     (in) stage = stage in which the entry exists
 *     (in) qualData = pointer to the entry qualifier data
 *     (in) qualLimit = max qualifiers for this type of entry
 *     (in) bcmQual = BCM qualifier type
 *     (in) ppdQual = pointer to appropriate PPD qualifier chain
 *     (in) count = number of elements in qualifier data/mask arrays
 *     (in) data = pointer to array of qualifier data
 *     (in) mask = pointer to array of qualifier mask
 *  Returns
 *     int (implied cast from bcm_error_t)
 *        BCM_E_NONE if successful
 *        BCM_E_* appropriately if not
 *  Notes
 *     Caller must already hold the unit lock.
 *
 *     This manipulates BCM layer qualifiers, so will shift left if needed
 *     (padding with zeroes) to align BCM LSb to appropriate bit of PPD, plus
 *     it supports complex BCM qualifiers (those made of more than one PPD
 *     qualifier)  by slicing the bits into appropriate parts for use as PPD
 *     layer qualifiers.
 *
 *     Bits added to the LSb end to move a qualifier left before applying it
 *     are not considered significant, but all bits from the least significant
 *     bit of the argument and going leftward are significant, even if they are
 *     not included (so if setting a qualifier that requires array of two and
 *     only one is provided, it is as if providing data=0 and mask=0 for the
 *     missing upper element of the array).
 *
 *     The array is little-endian with 64b grains: least significant octbyte is
 *     array[0], then next more significant is array[1], and so on.  Within
 *     array elements, the data are in machine-native order.
 */
STATIC int
_bcm_dpp_field_qualifier_get_int(_bcm_dpp_field_info_t *unitData,
                                 bcm_field_entry_t entry,
                                 _bcm_dpp_field_stage_idx_t stage,
                                 _bcm_dpp_field_qual_t *qualData,
#if _BCM_DPP_FIELD_GET_VERIFY_PPD || _BCM_DPP_FIELD_GET_USE_PPD
                                 SOC_PPD_FP_QUAL_VAL *qualHwData,
#endif /* _BCM_DPP_FIELD_GET_VERIFY_PPD || _BCM_DPP_FIELD_GET_USE_PPD */
                                 unsigned int qualLimit,
                                 bcm_field_qualify_t bcmQual,
                                 const SOC_PPD_FP_QUAL_TYPE *ppdQual,
                                 int count,
                                 uint64 *data,
                                 uint64 *mask)
{
    uint64 edata0, edata1;
    uint64 emask0, emask1;
    uint64 etemp;
    unsigned int *len = NULL;
    unsigned int index;
    unsigned int offset;
    signed int length;
    signed int shift;
    signed int over;
    int result = BCM_E_NONE;
    int found;

    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;
    COMPILER_64_ZERO(edata0);
    COMPILER_64_ZERO(emask0);

    len = sal_alloc(sizeof(unsigned int)*unitData->devInfo->qualChain, "_bcm_dpp_field_qualifier_get_int");
    if (len == NULL) {
         BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY,
                          (_BSL_BCM_MSG_NO_UNIT("Alocation failed")));

    }

    
    for (index = 0; index < count; index ++) {
        COMPILER_64_ZERO(data[index]);
        COMPILER_64_ZERO(mask[index]);
    }
    /* collect the lengths for all of the PPD qualifiers in the chain */
    for (index = 0, length = 0;
         (BCM_E_NONE == result) &&
         (index < unitData->devInfo->qualChain);
         index++) {
        if (_BCM_DPP_FIELD_PPD_QUAL_VALID(ppdQual[index])) {
            result = _bcm_dpp_ppd_qual_bits(unitData,
                                            stage,
                                            ppdQual[index],
                                            &(len[index]),
                                            NULL,
                                            NULL);
            length += len[index];
        }
    }
    BCMDNX_IF_ERR_EXIT_MSG(result,
                        (_BSL_BCM_MSG_NO_UNIT("was unable to query qualifier %d"
                                          " (%s) chain %d (PPD qual %d (%s))"
                                          " length for unit %d stage %u:"
                                          " %d (%s)"),
                         bcmQual,
                         _bcm_dpp_field_qual_name[bcmQual],
                         index,
                         ppdQual[index],
                          SOC_PPD_FP_QUAL_TYPE_to_string(ppdQual[index]),
                         unit,
                         stage,
                         result,
                         _SHR_ERRMSG(result)));
    /*
     *  If that worked, we have the PPD claimed length for each of the
     *  qualifiers in the chain that will be used for this BCM layer qualifier.
     *  We also have the total PPD qualifier chain length.
     *
     *  We want to verify the settings, though, and allow emulated and shifted
     *  and other features, to map the qualifiers in a more versatile manner.
     *  To this end, the table also specifies a length.  We will use that
     *  length to verify the arguments, unless it says to not do so or it says
     *  to specifically use the PPD layer length.
     *
     *  The intent is that the BCM layer will explicitly provide a length for
     *  almost all qualifiers.  This comes from the idea that we want the
     *  versatile mapping and some sanity checking as we go.
     *
     *  However, there are cases where we do not want to bother validating the
     *  arguments here.  Mostly these would be cases where the function
     *  immediately (or nearly so) up the call stack has already performed this
     *  validation, such as the case for a translated qualifier, where a
     *  qualifier is mapped from BCM layer constant values to PPD layer
     *  constant values (look for bcmFieldQualifyColor as an example).  This is
     *  achieved by setting zero as the BCM layer length in the table.
     *
     *  Another exception to using the BCM layer bit count is when that count
     *  will be inconsistent between stages, or can be adjusted by some kind of
     *  configuration setting.  An example of this is the chained key feature
     *  that allows data to be passed from one field group to another.  In this
     *  case, the validation is to be done using the PPD layer qualifier
     *  lengths instead of the constant from the table.  This is achieved by
     *  setting -1 as the BCM layer length in the table.
     */
    if (0 == unitData->qualMaps[bcmQual][_BCM_DPP_FIELD_QUALMAP_OFFSET_LENGTH]) {
        /* assume worst case length */
        length = count * 64;
    } else if (0 < unitData->qualMaps[bcmQual][_BCM_DPP_FIELD_QUALMAP_OFFSET_LENGTH]) {
        /* use BCM layer width */
        length = unitData->qualMaps[bcmQual][_BCM_DPP_FIELD_QUALMAP_OFFSET_LENGTH];
    } /* else (-1) just keep the computed length above */
    shift = unitData->qualMaps[bcmQual][_BCM_DPP_FIELD_QUALMAP_OFFSET_SHIFT];
    LOG_DEBUG(BSL_LS_BCM_FP,
              (BSL_META_U(unit,
                          "unit %d entry %d qual %s (%d):"
                          " length %d bits, shift %d\n"),
               unit,
               entry,
               _bcm_dpp_field_qual_name[bcmQual],
               bcmQual,
               length,
               shift));
    for (index = 0, offset = 0, found = 0;
         (index < unitData->devInfo->qualChain) &&
         (offset < count) &&
         (BCM_E_NONE == result);
         index++) {
        if (63 < shift) {
            /* skip to next octbyte of buffer (initial condition only) */
            COMPILER_64_ZERO(data[offset]);
            COMPILER_64_ZERO(mask[offset]);
            shift -= 64;
            length -= 64;
            offset++;
            continue;
        }
        if (_BCM_DPP_FIELD_PPD_QUAL_VALID(ppdQual[index])) {
            result = _bcm_dpp_field_qualifier_get_single_int(unitData,
                                                             entry,
                                                             qualData,
#if _BCM_DPP_FIELD_GET_VERIFY_PPD || _BCM_DPP_FIELD_GET_USE_PPD
                                                             qualHwData,
#endif /* _BCM_DPP_FIELD_GET_VERIFY_PPD || _BCM_DPP_FIELD_GET_USE_PPD */
                                                             qualLimit,
                                                             ppdQual[index],
                                                             &edata0,
                                                             &emask0);
            if (BCM_E_NOT_FOUND == result) {
                /* if not found, assume the qualifier is zero data, zero mask */
                COMPILER_64_ZERO(edata0);
                COMPILER_64_ZERO(emask0);
                result = BCM_E_NONE;
            } else if (BCM_E_NONE != result) {
                /* stop on any other error */
                break;
            } else {
                /* no error */
                found++;
            }
        } else {
            COMPILER_64_ZERO(edata0);
            COMPILER_64_ZERO(emask0);
            len[index] = 64;
        }
        edata1 = edata0;
        emask1 = emask0;
        if (0 > shift) {
            /* shifting right (initial condition only) */
            COMPILER_64_SHR(edata1, -shift);
            COMPILER_64_SHR(emask1, -shift);
            COMPILER_64_SET(etemp, ~0, ~0);
            /* may need to mask value since SHR is ASR not LSR */
            COMPILER_64_SHL(etemp, len[index]);
            COMPILER_64_NOT(etemp);
            COMPILER_64_AND(edata1, etemp);
            COMPILER_64_AND(emask1, etemp);
            /* set initial data and mask values */
            data[offset] = edata1;
            mask[offset] = emask1;
            /* adjust shift distance for next qualifier */
            shift += len[index];
        } else if (0 == shift) {
            /* first qualifier this octbyte of the buffer */
            data[offset] = edata1;
            mask[offset] = emask1;
            /* adjust shift distance for next qualifier */
            shift = len[index];
        } else /* (0 < shift) */ {
            /* shifting left */
            COMPILER_64_SHL(edata1, shift);
            COMPILER_64_SHL(emask1, shift);
            /* add these bits to the data and mask */
            COMPILER_64_OR(data[offset], edata1);
            COMPILER_64_OR(mask[offset], emask1);
            shift += len[index];
            if ((64 < shift) && ((offset + 1) < count)) {
                /* there were bits left over */
                over = shift & 0x3F;
                COMPILER_64_SHR(edata0, len[index] - over);
                COMPILER_64_SHR(emask0, len[index] - over);
                /* may need to mask value since SHR is ASR not LSR */
                COMPILER_64_SET(etemp, ~0, ~0);
                COMPILER_64_SHL(etemp, over);
                COMPILER_64_NOT(etemp);
                COMPILER_64_AND(edata0, etemp);
                COMPILER_64_AND(emask0, etemp);
                /* store remaining bits of this qualifier in next buffer */
                data[offset] = edata0;
                mask[offset] = emask0;
            }
        }
        if ((length < 64) && (offset < count)) {
            /* end of qualifier occurs here; throw out extra bits */
            COMPILER_64_SET(etemp, ~0, ~0);
            COMPILER_64_SHL(etemp, shift);
            COMPILER_64_NOT(etemp);
            COMPILER_64_AND(data[offset], etemp);
            COMPILER_64_AND(mask[offset], etemp);
            length = 0;
        }
        if (63 < shift) {
            /* need to move to next octbyte */
            shift -= 64;
            length -= 64;
            offset++;
        }
    } /* for (all qualifiers in chain as long as buffer and no erorr) */
    if (unitData->qualMaps[bcmQual][_BCM_DPP_FIELD_QUALMAP_OFFSET_LENGTH]) {
        if (length > 0) {
            LOG_WARN(BSL_LS_BCM_FP,
                     (BSL_META_U(unit,
                                 "still have %d bits left over!\n"), length));
        }
        if (offset < (count - 1)) {
            LOG_WARN(BSL_LS_BCM_FP,
                     (BSL_META_U(unit,
                                 "still have %d buffer elements left over!\n"),
                      (count - 1) - offset));
            for (offset++; offset < count; offset++) {
                COMPILER_64_ZERO(data[offset]);
                COMPILER_64_ZERO(mask[offset]);
            } /* for (offset++; offset < count; offset++) */
        } /* if (offset < (count - 1)) */
    }
    /*
     *  We discarded BCM_E_NOT_FOUND above because it is possible that for a
     *  compound qualifier, there will only be one part present.  However, the
     *  semantics of the call require BCM_E_NOT_FOUND if a qualifier is not
     *  present, so we need to emulate it now (despite having clobbered the
     *  caller's buffer!).  We shall do so by asserting BCM_E_NOT_FOUND in the
     *  case where *none* of the parts of a qualifier was present, so if any
     *  part of a compound qualifier (or the single part of a simple qualifier)
     *  is present, it is considered found.
     */
    if (!found) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d does not have"
                                           " qualifier %d (%s)"),
                          unit,
                          entry,
                          bcmQual,
                          _bcm_dpp_field_qual_name[bcmQual]));
    }
exit:
    if ((len != NULL)) {
        sal_free(len);
    }

    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_field_qualifier_get(_bcm_dpp_field_info_t *unitData,
                             bcm_field_entry_t entry,
                             bcm_field_qualify_t bcmQual,
                             int count,
                             uint64 *data,
                             uint64 *mask)
{
    _bcm_dpp_field_qual_t *qualData;
    bcm_field_qset_t *bqset;
    SOC_PPD_FP_QUAL_TYPE *ppdQual;
    _bcm_dpp_field_stage_idx_t stage;
    unsigned int qualLimit;
    uint32 groupTypes;
    int result;
#if _BCM_DPP_FIELD_GET_VERIFY_PPD || _BCM_DPP_FIELD_GET_USE_PPD
    SOC_PPD_FP_QUAL_VAL qualHwData[SOC_PPD_FP_NOF_QUALS_PER_DB_MAX];
    int useData;
#endif /* _BCM_DPP_FIELD_GET_VERIFY_PPD || _BCM_DPP_FIELD_GET_USE_PPD */
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;

#if _BCM_DPP_FIELD_GET_VERIFY_PPD || _BCM_DPP_FIELD_GET_USE_PPD
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_qualifier_get_prep(unitData,
                                                              entry,
                                                              &qualData,
                                                              &qualLimit,
                                                              &stage,
                                                              &bqset,
                                                              NULL,
                                                              &groupTypes,
                                                              &(qualHwData[0]),
                                                              &useData));
#else /* _BCM_DPP_FIELD_GET_VERIFY_PPD || _BCM_DPP_FIELD_GET_USE_PPD */
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_qualifier_get_prep(unitData,
                                                              entry,
                                                              &qualData,
                                                              &qualLimit,
                                                              &stage,
                                                              &bqset,
                                                              NULL,
                                                              &groupTypes));
#endif /* _BCM_DPP_FIELD_GET_VERIFY_PPD || _BCM_DPP_FIELD_GET_USE_PPD */
    if (bcmFieldQualifyCount > bcmQual) {
        /* standard BCM qualifier */
        if (!BCM_FIELD_QSET_TEST(unitData->unitQset, bcmQual)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d does not support"
                                               " qualifier %d (%s)"),
                              unit,
                              bcmQual,
                              _bcm_dpp_field_qual_name[bcmQual]));
        }
        if (!BCM_FIELD_QSET_TEST(*bqset, bcmQual)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d QSET does not"
                                               " include %d (%s)"),
                              unit,
                              entry,
                              bcmQual,
                              _bcm_dpp_field_qual_name[bcmQual]));
        } /* if (!BCM_FIELD_QSET_TEST(gropData->qset, type)) */
    } else { /* if (bcmFieldQualifyCount > type) */
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG_NO_UNIT("invalid BCM qualifier type %d"),
                          bcmQual));
    } /* if (bcmFieldQualifyCount > type) */
    /* get the qualifier mapping */
    result = _bcm_dpp_ppd_qual_from_bcm_qual(unitData,
                                             stage,
                                             groupTypes,
                                             bcmQual,
                                             &ppdQual);
    BCMDNX_IF_ERR_EXIT_MSG(result,
                        (_BSL_BCM_MSG_NO_UNIT("was unable to map qualifier %d (%s)"
                                          " to PPD for unit %d entry %d (stage"
                                          " %u types %08X: %d (%s)"),
                         bcmQual,
                         _bcm_dpp_field_qual_name[bcmQual],
                         unit,
                         entry,
                         stage,
                         groupTypes,
                         result,
                         _SHR_ERRMSG(result)));
#if _BCM_DPP_FIELD_GET_VERIFY_PPD || _BCM_DPP_FIELD_GET_USE_PPD
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_qualifier_get_int(unitData,
                                                             entry,
                                                             stage,
                                                             qualData,
                                                             useData?(&qualHwData[0]):NULL,
                                                             qualLimit,
                                                             bcmQual,
                                                             ppdQual,
                                                             count,
                                                             data,
                                                             mask));
#else /* _BCM_DPP_FIELD_GET_VERIFY_PPD || _BCM_DPP_FIELD_GET_USE_PPD */
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_qualifier_get_int(unitData,
                                                             entry,
                                                             stage,
                                                             qualData,
                                                             qualLimit,
                                                             bcmQual,
                                                             ppdQual,
                                                             count,
                                                             data,
                                                             mask));
#endif /* _BCM_DPP_FIELD_GET_VERIFY_PPD || _BCM_DPP_FIELD_GET_USE_PPD */
exit:
    _bcm_dpp_field_qualifier_get_cleanup(unitData,
                                         entry,
                                         qualData);
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_field_entry_qualifier_general_get_int(_bcm_dpp_field_info_t *unitData,
                                               bcm_field_entry_t entry,
                                               bcm_field_qualify_t bcmQual,
                                               unsigned int count,
                                               uint64 *data,
                                               uint64 *mask)
{
    bcm_field_presel_t presel;
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;

    if ((0 > bcmQual) || (bcmFieldQualifyCount <= bcmQual)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG_NO_UNIT("qualifier type %d not valid"),
                          bcmQual));
    }
    if (_BCM_DPP_FIELD_ENTRY_IS_PRESEL(entry)) {
        /*
         *  There are special cases for preselectors (bcmFieldQualifyStage;
         *  bcmFieldQualifyHeaderFormatSet on PetraB, and maybe others), so
         *  give the preselector code a chance to filter the request.  If the
         *  device supports the qualifier as a 'special' case, it will be
         *  handled by _bcm_petra_field_presel_qualify, but if it should be a
         *  'general' case, _bcm_petra_field_presel_qualify will in turn call
         *  _bcm_dpp_field_qualifier_set to do the actual work.
         */
        presel = _BCM_DPP_FIELD_PRESEL_FROM_ENTRY(entry);
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_presel_qualify_get(unitData,
                                                                  presel,
                                                                  bcmQual,
                                                                  count,
                                                                  data,
                                                                  mask));
    } else {
        if (unitData->qualMaps[bcmQual]) {
            /* this unit supports this qualifier */
            BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_qualifier_get(unitData,
                                                                 entry,
                                                                 bcmQual,
                                                                 count,
                                                                 data,
                                                                 mask));
        } else { /* if (unitData->qualMaps[bcmQual]) */
            /* this unit does not support this qualifier */
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d does not support"
                                               "qualifier %d (%s)"),
                              unit,
                              bcmQual,
                              _bcm_dpp_field_qual_name[bcmQual]));
        } /* if (unitData->qualMaps[bcmQual]) */
    }
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_ppd_stage_from_bcm_stage(_bcm_dpp_field_info_t *unitData,
                                  bcm_field_stage_t bcmStage,
                                  SOC_PPD_FP_DATABASE_STAGE *hwStageId)
{
    int index;
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT

    unit = unitData->unit;

    /* default stage */
    *hwStageId = SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF; 

    for (index = 0; index < unitData->devInfo->stages; index++) {
        if (unitData->stageD[index].devInfo->bcmStage == bcmStage) {
            *hwStageId = unitData->stageD[index].devInfo-> hwStageId;
            break;
        }
    }

    if (index == unitData->devInfo->stages) {
    	BCM_ERR_EXIT_NO_MSG(BCM_E_PARAM);

/*
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG_NO_UNIT("bcm stage %d is not valid. return default stage."),
                          bcmStage));
*/
    }
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_bcm_stage_from_ppd_stage(_bcm_dpp_field_info_t *unitData,
                                  SOC_PPD_FP_DATABASE_STAGE hwStageId,
                                  bcm_field_stage_t *bcmStage)
{
    int index;
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT

    unit = unitData->unit;

    /* default stage */
    *bcmStage = bcmFieldStageIngress; 

    for (index = 0; index < unitData->devInfo->stages; index++) {
        if (unitData->stageD[index].devInfo->hwStageId == hwStageId) {
            *bcmStage = unitData->stageD[index].devInfo->bcmStage;
                break;
        }
    }

    if (index == unitData->devInfo->stages) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG_NO_UNIT("ppd stage %d is not valid. return default stage."),
                          hwStageId));
    }
exit:
    BCMDNX_FUNC_RETURN;
}

