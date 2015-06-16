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
#include <bcm/error.h>
#include <bcm_int/esw/field.h>
#include <bcm_int/esw/tomahawk.h>
#include <soc/drv.h>
#include <soc/scache.h>
#include <bcm_int/esw/mirror.h>

#ifdef BCM_WARM_BOOT_SUPPORT
#ifdef BCM_TOMAHAWK_SUPPORT

_field_dynamic_map_t dynamic_map;

char *elem_name[] = {
    "_bcmFieldInternalType",
    "_bcmFieldInternalExtractor",
    "_bcmFieldInternalGroup",
    "_bcmFieldInternalGroupId",
    "_bcmFieldInternalGroupPri",
    "_bcmFieldInternalGroupQset",
    "_bcmFieldInternalQsetW",
    "_bcmFieldInternalQsetUdfMap",
    "_bcmFieldInternalGroupFlags",
    "_bcmFieldInternalGroupPbmp",
    "_bcmFieldInternalGroupSlice",
    "_bcmFieldInternalGroupQual",
    "_bcmFieldInternalQualQid",
    "_bcmFieldInternalQualOffset",
    "_bcmFieldInternalQualSize",
    "_bcmFieldInternalGroupStage",
    "_bcmFieldInternalGroupEntry",
    "_bcmFieldInternalGroupBlockCount",
    "_bcmFieldInternalGroupGroupStatus",
    "_bcmFieldInternalGroupGroupAset",
    "_bcmFieldInternalGroupCounterBmp",
    "_bcmFieldInternalGroupInstance",
    "_bcmFieldInternalGroupExtCodes",
    "_bcmFieldInternalGroupLtSlice",
    "_bcmFieldInternalGroupLtConfig",
    "_bcmFieldInternalGroupLtEntry",
    "_bcmFieldInternalGroupLtEntrySize",
    "_bcmFieldInternalGroupLtEntryStatus",
    "_bcmFieldInternalLtStatusEntriesFree",
    "_bcmFieldInternalLtStatusEntriesTotal",
    "_bcmFieldInternalLtStatusEntriesCnt",
    "_bcmFieldInternalGroupQsetSize",
    "_bcmFieldInternalGroupHintId",
    "_bcmFieldInternalGroupMaxSize",
    "_bcmFieldInternalEndStructGroup",
    "_bcmFieldInternalQualOffsetField",
    "_bcmFieldInternalQualOffsetNumOffset",
    "_bcmFieldInternalQualOffsetOffsetArr",
    "_bcmFieldInternalQualOffsetWidth",
    "_bcmFieldInternalQualOffsetSec",
    "_bcmFieldInternalQualOffsetBitPos",
    "_bcmFieldInternalQualOffsetQualWidth",
    "_bcmFieldInternalGroupPartCount",
    "_bcmFieldInternalEntryEid",
    "_bcmFieldInternalEntryPrio",
    "_bcmFieldInternalEntrySliceId",
    "_bcmFieldInternalEntryFlagsPart1",
    "_bcmFieldInternalEntryFlagsPart2",
    "_bcmFieldInternalEntryFlagsPart3",
    "_bcmFieldInternalEntryPbmp",
    "_bcmFieldInternalEntryAction",
    "_bcmFieldInternalEntrySlice",
    "_bcmFieldInternalEntryGroup",
    "_bcmFieldInternalEntryStat",
    "_bcmFieldInternalEntryPolicer",
    "_bcmFieldInternalEntryIngMtp",
    "_bcmFieldInternalEntryEgrMtp",
    "_bcmFieldInternalEntryDvp",
    "_bcmFieldInternalEntryCopy",
    "_bcmFieldInternalEntryCopyTypePointer",
    "_bcmFieldInternalPbmpData",
    "_bcmFieldInternalPbmpMask",
    "_bcmFieldInternalEntryActionsPbmp",
    "_bcmFieldInternalActionParam",
    "_bcmFieldInternalGroupClassAct",
    "_bcmFieldInternalActionHwIdx",
    "_bcmFieldInternalActionFlags",
    "_bcmFieldInternalStatFlags",
    "_bcmFieldInternalPolicerFlags",
    "_bcmFieldInternalExtl1e32Sel",
    "_bcmFieldInternalExtl1e16Sel",
    "_bcmFieldInternalExtl1e8Sel",
    "_bcmFieldInternalExtl1e4Sel",
    "_bcmFieldInternalExtl1e2Sel",
    "_bcmFieldInternalExtl2e16Sel",
    "_bcmFieldInternalExtl3e1Sel",
    "_bcmFieldInternalExtl3e2Sel",
    "_bcmFieldInternalExtl3e4Sel",
    "_bcmFieldInternalExtPmuxSel",
    "_bcmFieldInternalExtIntraSlice",
    "_bcmFieldInternalExtSecondary",
    "_bcmFieldInternalExtIpbmpRes",
    "_bcmFieldInternalExtNorml3l4",
    "_bcmFieldInternalExtNormmac",
    "_bcmFieldInternalExtAuxTagaSel",
    "_bcmFieldInternalExtAuxTagbSel",
    "_bcmFieldInternalExtAuxTagcSel",
    "_bcmFieldInternalExtAuxTagdSel",
    "_bcmFieldInternalExtTcpFnSel",
    "_bcmFieldInternalExtTosFnSel",
    "_bcmFieldInternalExtTtlFnSel",
    "_bcmFieldInternalExtClassIdaSel",
    "_bcmFieldInternalExtClassIdbSel",
    "_bcmFieldInternalExtClassIdcSel",
    "_bcmFieldInternalExtClassIddSel",
    "_bcmFieldInternalExtSrcContaSel",
    "_bcmFieldInternalExtSrcContbSel",
    "_bcmFieldInternalExtSrcDestCont0Sel",
    "_bcmFieldInternalExtSrcDestCont1Sel",
    "_bcmFieldInternalExtKeygenIndex",
    "_bcmFieldInternalLtEntrySlice",
    "_bcmFieldInternalLtEntryIndex",
    "_bcmFieldInternalGroupCount",
    "_bcmFieldInternalSliceCount",
    "_bcmFieldInternalLtSliceCount",
    "_bcmFieldInternalEndStructGroupQual",
    "_bcmFieldInternalEndStructQualOffset",
    "_bcmFieldInternalEndStructEntryDetails",
    "_bcmFieldInternalEndStructEntry",
    "_bcmFieldInternalEntryPbmpData",
    "_bcmFieldInternalEntryPbmpMask",
    "_bcmFieldInternalEndStructAction",
    "_bcmFieldInternalGlobalEntryPolicer",
    "_bcmFieldInternalGlobalEntryPolicerPid",
    "_bcmFieldInternalGlobalEntryPolicerFlags",
    "_bcmFieldInternalEndGroupExtractor",
    "_bcmFieldInternalControl",
    "_bcmFieldInternalControlFlags",
    "_bcmFieldInternalStage",
    "_bcmFieldInternalControlGroups",
    "_bcmFieldInternalControlStages",
    "_bcmFieldInternalControlPolicerHash",
    "_bcmFieldInternalControlPolicerCount",
    "_bcmFieldInternalControlStathash",
    "_bcmFieldInternalControlStatCount",
    "_bcmFieldInternalControlHintBmp",
    "_bcmFieldInternalControlHintHash",
    "_bcmFieldInternalControlLastAllocatedLtId",
    "_bcmFieldInternalEndStructControl",
    "_bcmFieldInternalPolicerPid",
    "_bcmFieldInternalPolicerSwRefCount",
    "_bcmFieldInternalPolicerHwRefCount",
    "_bcmFieldInternalPolicerLevel",
    "_bcmFieldInternalPolicerPoolIndex",
    "_bcmFieldInternalPolicerHwIndex",
    "_bcmFieldInternalPolicerHwFlags",
    "_bcmFieldInternalPolicerStageId",
    "_bcmFieldInternalEndStructPolicer",
    "_bcmFieldInternalStatsId",
    "_bcmFieldInternalStatSwRefCount",
    "_bcmFieldInternalStatHwRefCount",
    "_bcmFieldInternalStatOffsetMode",
    "_bcmFieldInternalStatPoolIndex",
    "_bcmFieldInternalStatHwIndex",
    "_bcmFieldInternalStatHwFlags",
    "_bcmFieldInternalEndStructStage",
    "_bcmFieldInternalStatnStat",
    "_bcmFieldInternalStatArr",
    "_bcmFieldInternalStatHwStat",
    "_bcmFieldInternalStatHwMode",
    "_bcmFieldInternalStatHwEntryCount",
    "_bcmFieldInternalStatGid",
    "_bcmFieldInternalStatStageId",
    "_bcmFieldInternalStatStatValues",
    "_bcmFieldInternalStatFlexMode",
    "_bcmFieldInternalEndStructStat",
    "_bcmFieldInternalHintHintid",
    "_bcmFieldInternalHintHints",
    "_bcmFieldInternalHintsHinttype",
    "_bcmFieldInternalHintsQual",
    "_bcmFieldInternalHintsMaxValues",
    "_bcmFieldInternalHintsStartbit",
    "_bcmFieldInternalHintsEndbit",
    "_bcmFieldInternalHintsFlags",
    "_bcmFieldInternalHintsMaxGrpSize",
    "_bcmFieldInternalEndStructHints",
    "_bcmFieldInternalHintGrpRefCount",
    "_bcmFieldInternalHintCount",
    "_bcmFieldInternalEndStructHint",
    "_bcmFieldInternalPolicerCfgFlags",
    "_bcmFieldInternalPolicerCfgMode",
    "_bcmFieldInternalPolicerCfgCkbitsSec",
    "_bcmFieldInternalPolicerCfgMaxCkbitsSec",
    "_bcmFieldInternalPolicerCfgCkbitsBurst",
    "_bcmFieldInternalPolicerCfgPkbitsSec",
    "_bcmFieldInternalPolicerCfgMaxPkbitsSec",
    "_bcmFieldInternalPolicerCfgPkbitsBurst ",
    "_bcmFieldInternalPolicerCfgKbitsCurrent",
    "_bcmFieldInternalPolicerCfgActionId",
    "_bcmFieldInternalPolicerCfgSharingMode",
    "_bcmFieldInternalPolicerCfgEntropyId",
    "_bcmFieldInternalPolicerCfgPoolId",
    "_bcmFieldInternalControlEndStructUdf",
    "_bcmFieldInternalHintHintsHintType",
    "_bcmFieldInternalHintHintsQual",
    "_bcmFieldInternalHintHintsMaxValues",
    "_bcmFieldInternalHintHintsStartBit",
    "_bcmFieldInternalHintHintsEndBit",
    "_bcmFieldInternalHintHintsFlags",
    "_bcmFieldInternalHintHintsMaxGrpSize",
    "_bcmFieldInternalEndStructHintHints",
    "_bcmFieldInternalStageStageid",
    "_bcmFieldInternalStageFlags",
    "_bcmFieldInternalStageTcamSz",
    "_bcmFieldInternalStageTcamSlices",
    "_bcmFieldInternalStageNumInstances",
    "_bcmFieldInternalStageNumPipes",
    "_bcmFieldInternalStageRanges",
    "_bcmFieldInternalStageRangeId",
    "_bcmFieldInternalStageNumMeterPools",
    "_bcmFieldInternalStageMeterPool",
    "_bcmFieldInternalStageNumCntrPools",
    "_bcmFieldInternalStageCntrPools",
    "_bcmFieldInternalStageLtTcamSz",
    "_bcmFieldInternalStageNumLogicalTables",
    "_bcmFieldInternalStageLtInfo",
    "_bcmFieldInternalStageExtLevels",
    "_bcmFieldInternalStageOperMode",
    "_bcmFieldInternalRangeFlags",
    "_bcmFieldInternalRangeRid",
    "_bcmFieldInternalRangeMin",
    "_bcmFieldInternalRangeMax",
    "_bcmFieldInternalRangeHwIndex",
    "_bcmFieldInternalRangeStyle",
    "_bcmFieldInternalMeterLevel",
    "_bcmFieldInternalMeterSliceId",
    "_bcmFieldInternalMeterSize",
    "_bcmFieldInternalMeterPoolSize",
    "_bcmFieldInternalMeterFreeMeters",
    "_bcmFieldInternalMeterNumMeterPairs",
    "_bcmFieldInternalMeterBmp",
    "_bcmFieldInternalCntrSliceId",
    "_bcmFieldInternalCntrSize",
    "_bcmFieldInternalCntrFreeCntrs",
    "_bcmFieldInternalCntrBmp",
    "_bcmFieldInternalLtConfigValid",
    "_bcmFieldInternalLtConfigLtId",
    "_bcmFieldInternalLtConfigLtPartPri",
    "_bcmFieldInternalLtConfigLtPartMap",
    "_bcmFieldInternalLtConfigLtActionPri",
    "_bcmFieldInternalLtConfigPri",
    "_bcmFieldInternalLtConfigFlags",
    "_bcmFieldInternalLtConfigEntry",
    "_bcmFieldInternalEndStructRanges",
    "_bcmFieldInternalEndStructMeter",
    "_bcmFieldInternalEndStructCntr",
    "_bcmFieldInternalEndStructLtConfig",
    "_bcmFieldInternalSlice",
    "_bcmFieldInternalSliceStartTcamIdx",
    "_bcmFieldInternalSliceNumber",
    "_bcmFieldInternalSliceEntryCount",
    "_bcmFieldInternalSliceFreeCount",
    "_bcmFieldInternalSliceCountersCount",
    "_bcmFieldInternalSliceMetersCount",
    "_bcmFieldInternalSliceInstalledEntriesCount",
    "_bcmFieldInternalSliceCounterBmp",
    "_bcmFieldInternalSliceMeterBmp",
    "_bcmFieldInternalSliceStageId",
    "_bcmFieldInternalSlicePortPbmp",
    "_bcmFieldInternalSliceEntriesInfo",
    "_bcmFieldInternalSliceNextSlice",
    "_bcmFieldInternalSlicePrevSlice",
    "_bcmFieldInternalSliceFlags",
    "_bcmFieldInternalSliceGroupFlags",
    "_bcmFieldInternalSliceLtMap",
    "_bcmFieldInternalEndStructSlice",
    "_bcmFieldInternalEndStructExtractor",
    "_bcmFieldInternalLtSliceSliceNum",
    "_bcmFieldInternalLtSliceStartTcamIdx",
    "_bcmFieldInternalLtSliceEntryCount",
    "_bcmFieldInternalLtSliceFreeCount",
    "_bcmFieldInternalLtSliceStageid",
    "_bcmFieldInternalLtSliceEntryinfo",
    "_bcmFieldInternalLtSliceNextSlice",
    "_bcmFieldInternalLtSlicePrevSlice",
    "_bcmFieldInternalLtSliceFlags",
    "_bcmFieldInternalLtSliceGroupFlags",
    "_bcmFieldInternalDataControlStart",
    "_bcmFieldInternalDataControlUsageBmp",
    "_bcmFieldInternalDataControlDataQualStruct",
    "_bcmFieldInternalDataControlDataQualQid",
    "_bcmFieldInternalDataControlDataQualUdfSpec",
    "_bcmFieldInternalDataControlDataQualOffsetBase",
    "_bcmFieldInternalDataControlDataQualOffset",
    "_bcmFieldInternalDataControlDataQualByteOffset",
    "_bcmFieldInternalDataControlDataQualHwBmp",
    "_bcmFieldInternalDataControlDataQualFlags",
    "_bcmFieldInternalDataControlDataQualElemCount",
    "_bcmFieldInternalDataControlDataQualLength",
    "_bcmFieldInternalDataControlEndStructDataQual",
    "_bcmFieldInternalDataControlEthertypeStruct",
    "_bcmFieldInternalDataControlEthertypeRefCount",
    "_bcmFieldInternalDataControlEthertypeL2",
    "_bcmFieldInternalDataControlEthertypeVlanTag",
    "_bcmFieldInternalDataControlEthertypePortEt",
    "_bcmFieldInternalDataControlEthertypeRelOffset",
    "_bcmFieldInternalDataControlProtStart",
    "_bcmFieldInternalDataControlProtIp4RefCount",
    "_bcmFieldInternalDataControlProtIp6RefCount",
    "_bcmFieldInternalDataControlProtFlags",
    "_bcmFieldInternalDataControlProtIp",
    "_bcmFieldInternalDataControlProtL2",
    "_bcmFieldInternalDataControlProtVlanTag",
    "_bcmFieldInternalDataControlProtRelOffset",
    "_bcmFieldInternalDataControlTcamStruct",
    "_bcmFieldInternalDataControlTcamRefCount",
    "_bcmFieldInternalDataControlTcamPriority",
    "_bcmFieldInternalDataControlElemSize",
    "_bcmFieldInternalDataControlNumElem",
    "_bcmFieldInternalEndStructDataControl",
    "_bcmFieldInternalControlUdfValid",
    "_bcmFieldInternalControlUdfUseCount",
    "_bcmFieldInternalControlUdfNum",
    "_bcmFieldInternalControlUdfUserNum",
    "_bcmFieldInternalControlUdfDetails",
    "_bcmFieldInternalDataControlEndStructEtype",
    "_bcmFieldInternalDataControlEndStructProt",
    "_bcmFieldInternalDataControlEndStructTcam",
    "_bcmFieldInternalEntryStatSid",
    "_bcmFieldInternalEntryStatExtendedSid",
    "_bcmFieldInternalEntryStatFlags",
    "_bcmFieldInternalEntryStatAction",
    "_bcmFieldInternalEndStructLtEntryDetails",
    "_bcmFieldInternalSliceLtPartitionPri",
    "_bcmFieldInternalEntryPolicerPid",
    "_bcmFieldInternalEntryPolicerFlags",
    "_bcmFieldInternalEndStructEntPolicer",
    "_bcmFieldInternalEndStructIFP",
    "_bcmfieldinternalElementCount",
};

/*
 *  Elements mapped to their attributes. Attributes cannot be modified.
 *  If there is a change in attributes for any of the elements, new
 *  element type _bcm_field_internal_element_t has to be created and
 *  then needs to be added here with updated attributes
 */
static const _field_type_map_t static_type_map[] = {
    { _bcmFieldInternalType,                4,  0x0},
    { _bcmFieldInternalExtractor,           4,  _FP_WB_TLV_LEN_ENCODED
                                                | _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalGroup,               0,  _FP_WB_TLV_NO_VALUE
                                                | _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalGroupId,             4,  0x0},
    { _bcmFieldInternalGroupPri,            4,  0x0},
    { _bcmFieldInternalGroupQset,           4, _FP_WB_TLV_NO_VALUE},
    { _bcmFieldInternalQsetW,               8,  _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalQsetUdfMap,          8,  _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalGroupFlags,          2,  0x0},
    { _bcmFieldInternalGroupPbmp,           1,  _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalGroupSlice,          4,  0x0},
    { _bcmFieldInternalGroupQual,           4, _FP_WB_TLV_NO_VALUE
                                                | _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalQualQid,             4,  _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalQualOffset,          4,  _FP_WB_TLV_LEN_ENCODED
                                                | _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalQualSize,            4,  0x0},
    { _bcmFieldInternalGroupStage,          4,  0x0},
    { _bcmFieldInternalGroupEntry,          4,  0x0},
    { _bcmFieldInternalGroupBlockCount,     2,  0x0},
    { _bcmFieldInternalGroupGroupStatus,    4,  _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalGroupGroupAset,      4,  _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalGroupCounterBmp,     4,  _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalGroupInstance,       4,  0x0},
    { _bcmFieldInternalGroupExtCodes,       4,  _FP_WB_TLV_NO_VALUE
                                                | _FP_WB_TLV_LEN_ENCODED
                                                | _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalGroupLtSlice,        4,  0x0},
    { _bcmFieldInternalGroupLtConfig,       4,  0x0},
    { _bcmFieldInternalGroupLtEntry,        4,  0x0},
    { _bcmFieldInternalGroupLtEntrySize,    2,  0x0},
    { _bcmFieldInternalGroupLtEntryStatus,  4,  _FP_WB_TLV_NO_VALUE},
    { _bcmFieldInternalLtStatusEntriesFree, 4,  0x0},
    { _bcmFieldInternalLtStatusEntriesTotal,4,  0x0},
    { _bcmFieldInternalLtStatusEntriesCnt,  4,  0x0},
    { _bcmFieldInternalGroupQsetSize,       4,  0x0},
    { _bcmFieldInternalGroupHintId,         4,  0x0},
    { _bcmFieldInternalGroupMaxSize,        4,  0x0},
    { _bcmFieldInternalEndStructGroup,      4,  0x0},
    { _bcmFieldInternalQualOffsetField,     4,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalQualOffsetNumOffset, 1,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalQualOffsetOffsetArr, 2,  _FP_WB_TLV_NO_TYPE
                                                | _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalQualOffsetWidth,     1,  _FP_WB_TLV_NO_TYPE
                                                | _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalQualOffsetSec,       1,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalQualOffsetBitPos,    1,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalQualOffsetQualWidth, 1,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalGroupPartCount,      4,  0x0},
    { _bcmFieldInternalEntryEid,            4,  0x0},
    { _bcmFieldInternalEntryPrio,           4,  0x0},
    { _bcmFieldInternalEntrySliceId,        4,  0x0},
    { _bcmFieldInternalEntryFlagsPart1,     4,  0x0},
    { _bcmFieldInternalEntryFlagsPart2,     4,  0x0},
    { _bcmFieldInternalEntryFlagsPart3,     4,  0x0},
    { _bcmFieldInternalEntryPbmp,           4,  0x0},
    { _bcmFieldInternalEntryAction,         4,  _FP_WB_TLV_LEN_ENCODED
                                                | _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalEntrySlice,          4,  0x0},
    { _bcmFieldInternalEntryGroup,          4,  0x0},
    { _bcmFieldInternalEntryStat,           4,  _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalEntryPolicer,        4,  _FP_WB_TLV_LEN_ENCODED
                                                | _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalEntryIngMtp,         1,  0x0},
    { _bcmFieldInternalEntryEgrMtp,         1,  0x0},
    { _bcmFieldInternalEntryDvp,            1,  0x0},
    { _bcmFieldInternalEntryCopy,           1,  _FP_WB_TLV_NO_VALUE},
    { _bcmFieldInternalEntryCopyTypePointer,0,  0x0},
    { _bcmFieldInternalPbmpData,            4,  0x0},
    { _bcmFieldInternalPbmpMask,            4,  0x0},
    { _bcmFieldInternalEntryActionsPbmp,    4,  _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalActionParam,         4,  _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalGroupClassAct,       1,  0x0},
    { _bcmFieldInternalActionHwIdx,         4,  _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalActionFlags,         1,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalStatFlags,           1,  0x0},
    { _bcmFieldInternalPolicerFlags,        1,  0x0},
    { _bcmFieldInternalExtl1e32Sel,         1,  _FP_WB_TLV_NO_TYPE
                                                | _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalExtl1e16Sel,         1,  _FP_WB_TLV_NO_TYPE
                                                | _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalExtl1e8Sel,          1,  _FP_WB_TLV_NO_TYPE
                                                | _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalExtl1e4Sel,          1,  _FP_WB_TLV_NO_TYPE
                                                | _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalExtl1e2Sel,          1,  _FP_WB_TLV_NO_TYPE
                                                | _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalExtl2e16Sel,         1,  _FP_WB_TLV_NO_TYPE
                                                | _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalExtl3e1Sel,          1,  _FP_WB_TLV_NO_TYPE
                                                | _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalExtl3e2Sel,          1,  _FP_WB_TLV_NO_TYPE
                                                | _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalExtl3e4Sel,          1,  _FP_WB_TLV_NO_TYPE
                                                | _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalExtPmuxSel,          1,  _FP_WB_TLV_NO_TYPE
                                                | _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalExtIntraSlice,       1,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalExtSecondary,        1,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalExtIpbmpRes,         1,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalExtNorml3l4,         1,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalExtNormmac,          1,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalExtAuxTagaSel,       1,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalExtAuxTagbSel,       1,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalExtAuxTagcSel,       1,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalExtAuxTagdSel,       1,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalExtTcpFnSel,         1,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalExtTosFnSel,         1,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalExtTtlFnSel,         1,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalExtClassIdaSel,      1,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalExtClassIdbSel,      1,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalExtClassIdcSel,      1,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalExtClassIddSel,      1,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalExtSrcContaSel,      1,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalExtSrcContbSel,      1,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalExtSrcDestCont0Sel,  1,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalExtSrcDestCont1Sel,  1,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalExtKeygenIndex,      4,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalLtEntrySlice,        4,  0x0},
    { _bcmFieldInternalLtEntryIndex,        4,  0x0},
    { _bcmFieldInternalGroupCount,          4,  0x0},
    { _bcmFieldInternalSliceCount,          4,  0x0},
    { _bcmFieldInternalLtSliceCount,        4,  0x0},
    { _bcmFieldInternalEndStructGroupQual,  4,  0x0},
    { _bcmFieldInternalEndStructQualOffset, 4,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalEndStructEntryDetails,4, _FP_WB_TLV_NO_VALUE},
    { _bcmFieldInternalEndStructEntry,      4,  0x0},
    { _bcmFieldInternalEntryPbmpData,       4,  0x0},
    { _bcmFieldInternalEntryPbmpMask,       4,  0x0},
    { _bcmFieldInternalEndStructAction,     4,  _FP_WB_TLV_NO_TYPE
                                                | _FP_WB_TLV_NO_VALUE},
    { _bcmFieldInternalGlobalEntryPolicer,  4,  0x0},
    { _bcmFieldInternalGlobalEntryPolicerPid,4, 0x0},
    { _bcmFieldInternalGlobalEntryPolicerFlags,1,0x0},
    { _bcmFieldInternalEndGroupExtractor,   4,  0x0},
    { _bcmFieldInternalControl,             4,  _FP_WB_TLV_NO_VALUE},
    { _bcmFieldInternalControlFlags,        1,  0x0},
    { _bcmFieldInternalStage,               4,  _FP_WB_TLV_NO_VALUE},
    { _bcmFieldInternalControlGroups,       4,  _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalControlStages,       4,  0x0},
    { _bcmFieldInternalControlPolicerHash,  4,  _FP_WB_TLV_LEN_ENCODED
                                                | _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalControlPolicerCount, 4,  0x0},
    { _bcmFieldInternalControlStathash,     4,  _FP_WB_TLV_LEN_ENCODED
                                                | _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalControlStatCount,    4,  0x0},
    { _bcmFieldInternalControlHintBmp,      1,  _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalControlHintHash,     4,  _FP_WB_TLV_NO_VALUE
                                                | _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalControlLastAllocatedLtId,4,0x0},
    { _bcmFieldInternalEndStructControl,    4,  0x0},
    { _bcmFieldInternalPolicerPid,          4,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalPolicerSwRefCount,   2,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalPolicerHwRefCount,   2,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalPolicerLevel,        1,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalPolicerPoolIndex,    1,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalPolicerHwIndex,      4,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalPolicerHwFlags,      4,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalPolicerStageId,      4,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalEndStructPolicer,    4,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalStatsId,             4,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalStatSwRefCount,      2,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalStatHwRefCount,      2,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalStatOffsetMode,      1,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalStatPoolIndex,       1,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalStatHwIndex,         4,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalStatHwFlags,         4,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalEndStructStage,      4,  0x0},
    { _bcmFieldInternalStatnStat,           1,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalStatArr,             4,  _FP_WB_TLV_NO_TYPE
                                                | _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalStatHwStat,          4,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalStatHwMode,          2,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalStatHwEntryCount,    1,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalStatGid,             4,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalStatStageId,         4,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalStatStatValues,      8,  _FP_WB_TLV_NO_TYPE
                                                | _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalStatFlexMode,        4,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalEndStructStat,       4,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalHintHintid,          4,  0x0},
    { _bcmFieldInternalHintHints,           0,  _FP_WB_TLV_LEN_ENCODED
                                                | _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalHintsHinttype,       4,  0x0},
    { _bcmFieldInternalHintsQual,           4,  0x0},
    { _bcmFieldInternalHintsMaxValues,      4,  0x0},
    { _bcmFieldInternalHintsStartbit,       4,  0x0},
    { _bcmFieldInternalHintsEndbit,         4,  0x0},
    { _bcmFieldInternalHintsFlags,          4,  0x0},
    { _bcmFieldInternalHintsMaxGrpSize,     4,  0x0},
    { _bcmFieldInternalEndStructHints,      4,  0x0},
    { _bcmFieldInternalHintGrpRefCount,     2,  0x0},
    { _bcmFieldInternalHintCount,           2,  0x0},
    { _bcmFieldInternalEndStructHint,       4,  0x0},
    { _bcmFieldInternalPolicerCfgFlags,     4,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalPolicerCfgMode,      4,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalPolicerCfgCkbitsSec, 4,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalPolicerCfgMaxCkbitsSec,4,_FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalPolicerCfgCkbitsBurst,4, _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalPolicerCfgPkbitsSec, 4,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalPolicerCfgMaxPkbitsSec,4,_FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalPolicerCfgPkbitsBurst,4,_FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalPolicerCfgKbitsCurrent,4,_FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalPolicerCfgActionId,  4,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalPolicerCfgSharingMode,4, _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalPolicerCfgEntropyId, 4,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalPolicerCfgPoolId,    4,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalControlEndStructUdf, 4,  _FP_WB_TLV_NO_TYPE
                                                | _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalHintHintsHintType,   4,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalHintHintsQual,       4,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalHintHintsMaxValues,  4,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalHintHintsStartBit,   4,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalHintHintsEndBit,     4,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalHintHintsFlags,      4,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalHintHintsMaxGrpSize, 4,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalEndStructHintHints,  4,  _FP_WB_TLV_NO_TYPE
                                                | _FP_WB_TLV_NO_VALUE},
    { _bcmFieldInternalStageStageid,        4,  0x0},
    { _bcmFieldInternalStageFlags,          4,  0x0},
    { _bcmFieldInternalStageTcamSz,         4,  0x0},
    { _bcmFieldInternalStageTcamSlices,     4,  0x0},
    { _bcmFieldInternalStageNumInstances,   4,  0x0},
    { _bcmFieldInternalStageNumPipes,       4,  0x0},
    { _bcmFieldInternalStageRanges,         4,  _FP_WB_TLV_LEN_ENCODED
                                                | _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalStageRangeId,        4,  0x0},
    { _bcmFieldInternalStageNumMeterPools,  4,  0x0},
    { _bcmFieldInternalStageMeterPool,      4,  _FP_WB_TLV_LEN_ENCODED
                                                | _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalStageNumCntrPools,   4,  0x0},
    { _bcmFieldInternalStageCntrPools,      4,  _FP_WB_TLV_LEN_ENCODED
                                                | _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalStageLtTcamSz,       4,  0x0},
    { _bcmFieldInternalStageNumLogicalTables,4, 0x0},
    { _bcmFieldInternalStageLtInfo,         4,  _FP_WB_TLV_LEN_ENCODED
                                                | _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalStageExtLevels,      4,  0x0},
    { _bcmFieldInternalStageOperMode,       4,  0x0},
    { _bcmFieldInternalRangeFlags,          4,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalRangeRid,            4,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalRangeMin,            4,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalRangeMax,            4,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalRangeHwIndex,        4,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalRangeStyle,          1,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalMeterLevel,          1,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalMeterSliceId,        4,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalMeterSize,           2,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalMeterPoolSize,       2,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalMeterFreeMeters,     2,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalMeterNumMeterPairs,  2,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalMeterBmp,            4,  _FP_WB_TLV_NO_TYPE
                                                | _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalCntrSliceId,         1,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalCntrSize,            2,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalCntrFreeCntrs,       2,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalCntrBmp,             4,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalLtConfigValid,       4,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalLtConfigLtId,        4,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalLtConfigLtPartPri,   4,  _FP_WB_TLV_NO_TYPE
                                                | _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalLtConfigLtPartMap,   4,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalLtConfigLtActionPri, 4,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalLtConfigPri,         4,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalLtConfigFlags,       4,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalLtConfigEntry,       4,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalEndStructRanges,     4,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalEndStructMeter,      4,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalEndStructCntr,       4,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalEndStructLtConfig,   4,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalSlice,               0,  _FP_WB_TLV_NO_VALUE},
    { _bcmFieldInternalSliceStartTcamIdx,   4,  0x0},
    { _bcmFieldInternalSliceNumber,         1,  0x0 },
    { _bcmFieldInternalSliceEntryCount,     4,  0x0},
    { _bcmFieldInternalSliceFreeCount,      4,  0x0},
    { _bcmFieldInternalSliceCountersCount,  4,  0x0},
    { _bcmFieldInternalSliceMetersCount,    4,  0x0},
    { _bcmFieldInternalSliceInstalledEntriesCount, 4, 0x0},
    { _bcmFieldInternalSliceCounterBmp,     4,  _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalSliceMeterBmp,       4,  _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalSliceStageId,        4,  0x0},
    { _bcmFieldInternalSlicePortPbmp,       1,  _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalSliceEntriesInfo,    4,  _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalSliceNextSlice,      4,  0x0},
    { _bcmFieldInternalSlicePrevSlice,      4,  0x0},
    { _bcmFieldInternalSliceFlags,          1,  0x0},
    { _bcmFieldInternalSliceGroupFlags,     1,  0x0},
    { _bcmFieldInternalSliceLtMap,          4,  0x0},
    { _bcmFieldInternalEndStructSlice,      4,  0x0},
    { _bcmFieldInternalEndStructExtractor,  4,  _FP_WB_TLV_NO_TYPE
                                                | _FP_WB_TLV_NO_VALUE},
    { _bcmFieldInternalLtSliceSliceNum,     1,  0x0},
    { _bcmFieldInternalLtSliceStartTcamIdx, 4,  0x0},
    { _bcmFieldInternalLtSliceEntryCount,   4,  0x0},
    { _bcmFieldInternalLtSliceFreeCount,    4,  0x0},
    { _bcmFieldInternalLtSliceStageid,      4,  0x0},
    { _bcmFieldInternalLtSliceEntryinfo,    4,  0x0},
    { _bcmFieldInternalLtSliceNextSlice,    4,  _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalLtSlicePrevSlice,    4,  _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalLtSliceFlags,        2,  0x0},
    { _bcmFieldInternalLtSliceGroupFlags,   2,  0x0},
    { _bcmFieldInternalDataControlStart,    0,  _FP_WB_TLV_NO_VALUE
                                                | _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalDataControlUsageBmp,         4, 0x0},
    { _bcmFieldInternalDataControlDataQualStruct,   0, _FP_WB_TLV_NO_VALUE
                                                      | _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalDataControlDataQualQid,      4, 0x0},
    { _bcmFieldInternalDataControlDataQualUdfSpec,  4, _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalDataControlDataQualOffsetBase,4,0x0},
    { _bcmFieldInternalDataControlDataQualOffset,   4, 0x0},
    { _bcmFieldInternalDataControlDataQualByteOffset,1,0x0},
    { _bcmFieldInternalDataControlDataQualHwBmp,    4, 0x0},
    { _bcmFieldInternalDataControlDataQualFlags,    4, 0x0},
    { _bcmFieldInternalDataControlDataQualElemCount,1, 0x0},
    { _bcmFieldInternalDataControlDataQualLength,   4, 0x0},
    { _bcmFieldInternalDataControlEndStructDataQual,4, _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalDataControlEthertypeStruct,  0, _FP_WB_TLV_LEN_ENCODED
                                                      | _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalDataControlEthertypeRefCount,4, _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalDataControlEthertypeL2,      2, _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalDataControlEthertypeVlanTag, 2, _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalDataControlEthertypePortEt,  2, _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalDataControlEthertypeRelOffset,4,_FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalDataControlProtStart,        0, _FP_WB_TLV_LEN_ENCODED
                                                      | _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalDataControlProtIp4RefCount,  4, _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalDataControlProtIp6RefCount,  4, _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalDataControlProtFlags,        4, _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalDataControlProtIp,           1, _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalDataControlProtL2,           2, _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalDataControlProtVlanTag,      2, _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalDataControlProtRelOffset,    4, _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalDataControlTcamStruct,       4, _FP_WB_TLV_LEN_ENCODED
                                                       |_FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalDataControlTcamRefCount,     1,   _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalDataControlTcamPriority,     1,   _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalDataControlElemSize,         4, 0x0},
    { _bcmFieldInternalDataControlNumElem,          4, 0x0},
    { _bcmFieldInternalEndStructDataControl,        4, 0x0},
    { _bcmFieldInternalControlUdfValid,             1, _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalControlUdfUseCount,          4, _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalControlUdfNum,               4, _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalControlUdfUserNum,           1, _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalControlUdfDetails,           0, _FP_WB_TLV_LEN_ENCODED
                                                      | _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalDataControlEndStructEtype,   4, _FP_WB_TLV_NO_TYPE
                                                      | _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalDataControlEndStructProt,    4, _FP_WB_TLV_NO_TYPE
                                                      | _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalDataControlEndStructTcam,    4, _FP_WB_TLV_NO_TYPE
                                                      | _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalEntryStatSid,        4, 0},
    { _bcmFieldInternalEntryStatExtendedSid,4, 0},
    { _bcmFieldInternalEntryStatFlags,      2, 0},
    { _bcmFieldInternalEntryStatAction,     4, 0},
    { _bcmFieldInternalEndStructLtEntryDetails, 4, _FP_WB_TLV_NO_VALUE},
    { _bcmFieldInternalSliceLtPartitionPri, 1, 0x0},
    { _bcmFieldInternalEntryPolicerPid,     4, _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalEntryPolicerFlags,   1, _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalEndStructEntPolicer, 0, _FP_WB_TLV_NO_TYPE
                                               | _FP_WB_TLV_NO_VALUE},
    {_bcmFieldInternalEndStructIFP,         0, 0x0},

};
#define BCM_IF_ERROR_CLEANUP(rv)            \
    if (rv < 0) {                           \
        goto cleanup;                       \
    }

#define _DYNAMIC_MAP_INSERT( _ptr_, _node_, _tail_)     \
    do {                                                \
        if (_tail_ == NULL) {                            \
            _ptr_ = _node_;                             \
            _tail_ = _node_;                            \
        } else {                                        \
            _tail_->next = _node_;                      \
            _tail_ = _tail_->next;                      \
        }                                               \
    } while(0);

#define _DYNAMIC_MAP_DELETE_ALL(_ptr_, _entry_type_) \
    do {                                             \
        _entry_type_ *node = NULL;                   \
        while (_ptr_ != NULL) {                      \
            node = _ptr_;                            \
            _ptr_ = _ptr_->next;                     \
            sal_free(node);                          \
            node = NULL;                             \
        }                                            \
    } while(0);                                      \

void dynamic_map_free(_field_dynamic_map_t dynamic_map) {
    _DYNAMIC_MAP_DELETE_ALL(dynamic_map.entry_map, _field_dynamic_map_entry_t);
    _DYNAMIC_MAP_DELETE_ALL(dynamic_map.lt_entry_map,
                            _field_dynamic_map_lt_entry_t);
}


/*
 * Function:
 *      _tlv_print_array
 * Purpose:
 *      print the array of values
 * Parameters:
 *      unit   - (IN) StrataSwitch unit #.
 *      value  - (IN) Pointer to the array of values
 *      length - (IN) length of the array.
 *   Returns:
 *      BCM_E_XXX
 */
int
_tlv_print_array(int unit, void *value, int length, int chunk_sz)
{
    int i         = 0;     /* local variable for loop */
    uint8 *val8   = 0;     /* variable to hold 8 bit values */
    uint16 *val16 = 0;     /* variable to hold 16 bit values */
    uint32 *val32 = 0;     /* variable to hold 32 bit values */
    long long unsigned int *val64 = 0;
                           /* variable to hold 64 bit values */

    switch (chunk_sz) {
        case 2:
             val16 = (uint16 *)value;
             for (i = 0; i < length/chunk_sz; i++) {
                  LOG_DEBUG(BSL_LS_BCM_FP,
                   (BSL_META_U(unit, "TLV Value array: %x\n"), val16[i]));
             }
             break;
        case 4:
             val32 = (uint32 *)value;
              for (i = 0; i < length/chunk_sz; i++) {
                  LOG_DEBUG(BSL_LS_BCM_FP,
                   (BSL_META_U(unit, "TLV Value array: %x\n"), val32[i]));
              }
              break;
        case 8:
             val64 = (long long unsigned int *)value;
              for (i = 0; i < length/chunk_sz; i++) {
                  LOG_DEBUG(BSL_LS_BCM_FP,
                   (BSL_META_U(unit, "TLV Value array: %llx\n"), val64[i]));
              }
              break;
        default:
                val8 = (uint8 *)value;
                for (i = 0; i < length; i++) {
                    LOG_DEBUG(BSL_LS_BCM_FP,
                       (BSL_META_U(unit, "TLV Value array: %02x\n"), val8[i]));
                }


    }

    return BCM_E_NONE;
}
/*
 * Function:
 *      tlv_write
 * Purpose:
 *      Element stored in scache =_internal_element_t | ((array/variable) << 28)
 *      if (_FP_WB_TLV_LEN_PRESENT) -> write length to scache
 *      if !(_FP_WB_TLV_NO_VALUE)
 *          -> if variable -> get size from static map and write
 *          -> if array -> length times (size of each component from static map)
 *      if length was encoded, types will be added later as variables with
 *      _bcmfieldinternaltype as element type where we write only a uint32(which
 *      is considered as value for the main type. If more types are added, it
 *      will be a array
 *      Will add an example here later about encoded.
 * Parameters:
 *      unit - (IN) StrataSwitch unit #.
 *      tlv  - (IN) input structure containing type to be written,
                    its length & value
 *      ptr  - (IN) pointer to scache; obtained from
                    field_control->scache_ptr[PART];
 *      pos  - (IN/OUT) Current offset from the base of scache part in use.
 *   Returns:
 *      BCM_E_XXX
 */

int
tlv_write(int unit, _field_tlv_t *tlv, uint8 *ptr, uint32 *position)
{
    uint32 pos, data               = 0;  /* local variables for
                                              * scache_pos and data
                                              */
    _bcm_field_internal_element_t elem;  /* local variable for elem type */
    int write_size = 0;                  /* size used for value field */

    if (tlv == NULL) {
        return BCM_E_PARAM;
    }
    pos = *position;

    elem = tlv->type;
    /* Write Type */
    if (!(static_type_map[elem].flags & _FP_WB_TLV_NO_TYPE))
    {   data = (tlv->basic_type <<  _FP_WB_TLV_BASIC_TYPE_SHIFT);
        data |= elem;
        LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit, "TLV Write Enum: %s\n"),
                                             elem_name[elem]));
        sal_memcpy(&ptr[pos], &data, sizeof(uint32));
        pos += sizeof(uint32);
    } else {
        LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit, "Writing For Enum: %s\n"),
                  elem_name[tlv->type]));
    }
    /* Write Length */
    if (static_type_map[elem].flags & _FP_WB_TLV_LEN_PRESENT) {
        LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit, "TLV Write Length: %x\n"),
                                             tlv->length));
        sal_memcpy(&ptr[pos], &(tlv->length), sizeof(uint32));
        pos += sizeof(uint32);
    }

    if ((tlv->value != NULL) &&
        !(static_type_map[elem].flags &
            _FP_WB_TLV_NO_VALUE)) {
            if (tlv->basic_type == _bcmFieldInternalVariable) {
                write_size = static_type_map[elem].size;
            } else if (tlv->basic_type == _bcmFieldInternalArray) {
                write_size = ((tlv->length) *
                              (static_type_map[elem].size));
            } else {
               return BCM_E_INTERNAL;
            }
            if (static_type_map[elem].flags & _FP_WB_TLV_LEN_ENCODED) {
                 write_size = (((tlv->length) & _FP_WB_TLV_TYPES_MASK) >>
                                 _FP_WB_TLV_TYPE_SHIFT) *
                                 sizeof(tlv->type);
             }
    }

    /* Write Value */
    if (write_size > 0) {
        BCM_IF_ERROR_RETURN(_tlv_print_array(unit, tlv->value, write_size,
                                    static_type_map[elem].size));
        sal_memcpy(&ptr[pos], tlv->value, write_size);
        pos += write_size;
    }

    *position = pos;

    return BCM_E_NONE;

}

/*
 * Function:
 *      tlv_read
 * Purpose:
 *      Read a chunck from scache. Chunk size depends on the type read/passed.
 *      Call function needs to free the memory allocated to tlv structure/value
 *      field in tlv structure.
 *      Function flow:
 *          Read type (if input type is not a valid one)
 *          Get corresponding flags, read length if it is present.
 *          If value is present, read the corresponding value/array of values
 *          If not, and length is encoded, read value array which contains the
 *          list of types that are encoded. Later tlv_read has to be called
 *          with corresponding type to get the value of each instance.
 * Parameters:
 *      unit - (IN) StrataSwitch unit #.
 *      tlv  - (OUT) structure containing type, length and value/value array
 *                   read(based on flags from static_type_map)
 *      ptr  - (IN) pointer to scache; obtained from
 *                  field_control->scache_ptr[PART];
 *      pos  - (IN/OUT) Current offset from the base of scache part in use.
 * Returns:
 *      BCM_E_XXXX
 */

int
tlv_read(int unit, _field_tlv_t *tlv, uint8 *ptr, uint32 *position)
{
    uint32 pos, data = 0;     /* local variables for scache_pos and data */
    int read_size    = 0;     /* size used for value field */

    if (tlv == NULL) {
        return BCM_E_PARAM;
    }
    pos = *position;

    /* Read Type */
    if (tlv->type == -1) {
        sal_memcpy(&data, &ptr[pos], sizeof(uint32));
        pos += sizeof(uint32);

        tlv->type = data & _FP_WB_TLV_ELEM_TYPE_MASK;
        tlv->basic_type = ((data & _FP_WB_TLV_BASIC_TYPE_MASK) >>
                           _FP_WB_TLV_BASIC_TYPE_SHIFT);
        LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit, "TLV Read Enum: %s\n"),
                                             elem_name[tlv->type]));
    } else {
        LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
                  "Reading For Enum: %s\n"),
                   elem_name[tlv->type]));
    }

    /* Read Length */
    if (static_type_map[tlv->type].flags
        & _FP_WB_TLV_LEN_PRESENT) {
        sal_memcpy(&(tlv->length), &ptr[pos], sizeof(uint32));
        pos += sizeof(uint32);
        LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit, "TLV READ Length: %x\n"),
                                             tlv->length));
    }


    if (!(static_type_map[tlv->type].flags
          & _FP_WB_TLV_NO_VALUE)) {
        if (tlv->basic_type == _bcmFieldInternalVariable) {
            read_size = static_type_map[tlv->type].size;
        } else if (tlv->basic_type == _bcmFieldInternalArray) {
            read_size = ((tlv->length) * static_type_map[tlv->type].size);
        } else {
            return BCM_E_INTERNAL;
        }
        if (static_type_map[tlv->type].flags
             & _FP_WB_TLV_LEN_ENCODED) {
              data = (((tlv->length) & _FP_WB_TLV_TYPES_MASK) >>
                       _FP_WB_TLV_TYPE_SHIFT);
              read_size = data * sizeof(uint32);
        }
    }

    /* Read Value */
    if (read_size > 0) {
        _FP_XGS3_ALLOC(tlv->value, read_size, "wb read values");
        sal_memcpy(tlv->value, &ptr[pos], read_size);
        pos += read_size;
        BCM_IF_ERROR_RETURN(
             _tlv_print_array(unit, tlv->value, read_size,
                              static_type_map[tlv->type].size));
    }
    if (read_size == 0) {
        tlv->value = NULL;
    }
    *position = pos;

    return BCM_E_NONE;
}


/* Function:
 * _field_tlv_create
 *
 * Purpose:
 *   Create memory for tlv and set basic fields
 *
 * Parameters:
 * type                    -(IN) element type
 * basic_type              -(IN) basic type of the element
 * length                  -(IN) length of element in case of arrays
 * tlv                     -(OUT) tlv output
 *
 *  Returns:
 *      BCM_E_XXXX
 */

int
_field_tlv_create(_bcm_field_internal_element_t type,
                  _bcm_field_internal_type_t basic_type,
                  uint32 length, _field_tlv_t **tlv)
{
    _field_tlv_t *tlv_new = NULL;   /* TLV data structure instance */

    if (*tlv != NULL) {
       sal_free(*tlv);
       *tlv = NULL;
    }
    _FP_XGS3_ALLOC(tlv_new,sizeof(_field_tlv_t),"tlv alloc");

    if (tlv_new == NULL) {
        return BCM_E_MEMORY;
    }
    tlv_new->type = type;
    tlv_new->basic_type = basic_type;
    tlv_new->length = length;

    *tlv = tlv_new;
    return BCM_E_NONE;
}

/* Basic function calls required in all Functions */

#define WB_FIELD_CONTROL_GET(fc, s_ptr, s_pos)                  \
    do {                                                        \
        BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));     \
        s_ptr = fc->scache_ptr[_FIELD_SCACHE_PART_0];           \
        s_pos = &(fc->scache_pos);                              \
    } while (0);

#define TLV_WRITE(unit, tlv, ptr, pos)     \
            BCM_IF_ERROR_RETURN(tlv_write(unit,tlv,ptr,pos));

#define TLV_CREATE(_type_, _inttype_, _size_, _value_) \
   BCM_IF_ERROR_RETURN(_field_tlv_create(_type_, _inttype_, _size_, _value_));


/* Function:
 * _field_hash_sync
 *
 * Purpose:
 *    Sync _field_hash_t structure
 *
 * Paramaters:
 * unit             - (IN) BCM device number
 * ptr              - (IN) pointer to scache; obtained from
 *                         field_control->scache_ptr[PART];
 * pos              - (IN/OUT) Current offset from the base
 *                             of scache part in use.
 *
 * Returns:
 *     BCM_E_XXX
 */

int
_field_hash_sync(int unit)
{

    _field_tlv_t *tlv       = NULL;       /* tlv data */
    _field_control_t        *fc;          /* Field control structure.       */
    _field_policer_t        *f_pl;        /* Internal policer descriptor.   */
    _field_stat_t           *f_st;        /* Internal Stat descriptor */
    _field_hints_t          *f_ht;        /* Internal Hints Descriptor */
    int idx                 = 0;          /* Iterator */
    int policer_count       = 0;          /* number of policers */
    int stat_count          = 0;          /* number of stats */
    int hints_count         = 0;          /* number of hints */
    uint32 endmarkerpolicer = _FIELD_WB_EM_POLICER;
                                          /* End marker for policer */
    uint32 endmarkerstat    = _FIELD_WB_EM_STAT;
                                          /* End marker for stat */
    uint32 endmarkerhint    = _FIELD_WB_EM_HINT;
                                          /* End marker for hint */
    _field_hint_t           *hint;        /* temporary field for hint */
    int hintidx             = 0;          /* hint id variable */
    int count = 0;

    int type_policer[22] = {              /* types in _field_policer_t */
    (int)((_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)
           | _bcmFieldInternalPolicerPid),
    (int)((_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)
           | _bcmFieldInternalPolicerCfgFlags),
    (int)((_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)
           | _bcmFieldInternalPolicerCfgMode),
    (int)((_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)
           | _bcmFieldInternalPolicerCfgCkbitsSec),
    (int)((_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)
           | _bcmFieldInternalPolicerCfgMaxCkbitsSec),
    (int)((_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)
           | _bcmFieldInternalPolicerCfgCkbitsBurst),
    (int)((_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)
           | _bcmFieldInternalPolicerCfgPkbitsSec),
    (int)((_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)
           | _bcmFieldInternalPolicerCfgMaxPkbitsSec),
    (int)((_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)
           | _bcmFieldInternalPolicerCfgPkbitsBurst),
    (int)((_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)
           | _bcmFieldInternalPolicerCfgKbitsCurrent),
    (int)((_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)
           | _bcmFieldInternalPolicerCfgActionId),
    (int)((_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)
           | _bcmFieldInternalPolicerCfgSharingMode),
    (int)((_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)
           | _bcmFieldInternalPolicerCfgEntropyId),
    (int)((_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)
           | _bcmFieldInternalPolicerCfgPoolId),
    (int)((_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)
           | _bcmFieldInternalPolicerSwRefCount),
    (int)((_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)
           | _bcmFieldInternalPolicerHwRefCount),
    (int)((_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)
           | _bcmFieldInternalPolicerLevel),
    (int)((_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)
           | _bcmFieldInternalPolicerPoolIndex),
    (int)((_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)
           | _bcmFieldInternalPolicerHwIndex),
    (int)((_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)
           | _bcmFieldInternalPolicerHwFlags),
    (int)((_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)
           | _bcmFieldInternalPolicerStageId),
    (int)((_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)
           | _bcmFieldInternalEndStructPolicer)
    };

    int type_stat[17] = {                 /* types in _field_stat_t */
    (int)((_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)
           | _bcmFieldInternalStatsId),
    (int)((_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)
           | _bcmFieldInternalStatSwRefCount),
    (int)((_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)
           | _bcmFieldInternalStatHwRefCount),
    (int)((_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)
           | _bcmFieldInternalStatOffsetMode),
    (int)((_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)
           | _bcmFieldInternalStatPoolIndex),
    (int)((_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)
           | _bcmFieldInternalStatHwIndex),
    (int)((_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)
           | _bcmFieldInternalStatHwFlags),
    (int)((_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)
           | _bcmFieldInternalStatnStat),
    (int)((_bcmFieldInternalArray    << _FP_WB_TLV_BASIC_TYPE_SHIFT)
           | _bcmFieldInternalStatArr),
    (int)((_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)
           | _bcmFieldInternalStatHwStat),
    (int)((_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)
           | _bcmFieldInternalStatHwMode),
    (int)((_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)
           | _bcmFieldInternalStatHwEntryCount),
    (int)((_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)
           | _bcmFieldInternalStatGid),
    (int)((_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)
           | _bcmFieldInternalStatStageId),
    (int)((_bcmFieldInternalArray    << _FP_WB_TLV_BASIC_TYPE_SHIFT)
           | _bcmFieldInternalStatStatValues),
    (int)((_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)
           | _bcmFieldInternalStatFlexMode),
    (int)((_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)
           | _bcmFieldInternalEndStructStat)
    };
    int type_hint[8] =  {                 /* types in _field_hint_t */
    (int)((_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)
           | _bcmFieldInternalHintHintsHintType),
    (int)((_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)
           | _bcmFieldInternalHintHintsQual),
    (int)((_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)
           | _bcmFieldInternalHintHintsMaxValues),
    (int)((_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)
           | _bcmFieldInternalHintHintsStartBit),
    (int)((_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)
           | _bcmFieldInternalHintHintsEndBit),
    (int)((_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)
           | _bcmFieldInternalHintHintsFlags),
    (int)((_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)
           | _bcmFieldInternalHintHintsMaxGrpSize),
    (int) ((_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)
           | _bcmFieldInternalEndStructHintHints)
    };
    uint8 *ptr;                          /* Pointer to scache_pos */
    uint32 *position;                    /* Scache position */

    WB_FIELD_CONTROL_GET(fc, ptr, position);


    /* Iterate over all hash buckets. */
    for (idx = 0; idx < _FP_HASH_SZ(fc); idx++) {
        /* Iterate over entries in each bucket. */
        f_pl = fc->policer_hash[idx];
        while (NULL != f_pl) {
        if((f_pl->pid != -1)  && (f_pl->stage_id == _BCM_FIELD_STAGE_INGRESS)) {
           policer_count ++;
        }
            f_pl = f_pl->next;
        }
    }

    /* Iterate over all hash buckets. */
    for (idx = 0; idx < _FP_HASH_SZ(fc); idx++) {
        /* Iterate over entries in each bucket. */
        f_st = fc->stat_hash[idx];
        while (NULL != f_st) {
        if((f_st->sid != -1)  && (f_st->stage_id == _BCM_FIELD_STAGE_INGRESS)) {
           stat_count ++;
        }
            f_st = f_st->next;
        }
    }
   /* Iterate over all hash buckets. */
    for (idx = 0; idx < _FP_HASH_SZ(fc); idx++) {
        /* Iterate over entries in each bucket. */
        f_ht = fc->hints_hash[idx];
        while (NULL != f_ht) {
        if(f_ht->hintid != -1) {
           hints_count ++;
        }
            f_ht = f_ht->next;
        }
    }

    /* _field_policer_t */

    /* In the encoded length below , first ten bits is value 22 [no of types],
     * next 22 bits is policer count in hash. Here number of types indicate
     * count of elements in the current datastructure.
     */

    TLV_CREATE(_bcmFieldInternalControlPolicerHash, _bcmFieldInternalArray,
               (policer_count | (22 << _FP_WB_TLV_TYPE_SHIFT)), &tlv);
    tlv->value = &(type_policer);
    TLV_WRITE(unit, tlv, ptr, position);


    /* Iterate over all hash buckets. */
    for (idx = 0; idx < _FP_HASH_SZ(fc); idx++) {
        /* Iterate over entries in each bucket. */
        f_pl = fc->policer_hash[idx];
        while (NULL != f_pl) {
        if((f_pl->pid != -1)  && (f_pl->stage_id == _BCM_FIELD_STAGE_INGRESS)) {
            TLV_CREATE(_bcmFieldInternalPolicerPid,
                                   _bcmFieldInternalVariable,0, &tlv);
            tlv->value = &(f_pl->pid);
            TLV_WRITE(unit, tlv, ptr, position);
            TLV_CREATE(_bcmFieldInternalPolicerCfgFlags,
                                  _bcmFieldInternalVariable,0, &tlv);
            tlv->value = &(f_pl->cfg.flags);
            TLV_WRITE(unit, tlv, ptr, position);
            TLV_CREATE(_bcmFieldInternalPolicerCfgMode,
                                 _bcmFieldInternalVariable,0, &tlv);
            tlv->value = &(f_pl->cfg.mode);
            TLV_WRITE(unit, tlv, ptr, position);
            TLV_CREATE(_bcmFieldInternalPolicerCfgCkbitsSec,
                                   _bcmFieldInternalVariable,0, &tlv);
            tlv->value = &(f_pl->cfg.ckbits_sec);
            TLV_WRITE(unit, tlv, ptr, position);
            TLV_CREATE(_bcmFieldInternalPolicerCfgMaxCkbitsSec,
                                   _bcmFieldInternalVariable,0, &tlv);
            tlv->value = &(f_pl->cfg.max_ckbits_sec);
            TLV_WRITE(unit, tlv, ptr, position);
            TLV_CREATE(_bcmFieldInternalPolicerCfgCkbitsBurst,
                                   _bcmFieldInternalVariable,0, &tlv);
            tlv->value = &(f_pl->cfg.ckbits_burst);
            TLV_WRITE(unit, tlv, ptr, position);
            TLV_CREATE(_bcmFieldInternalPolicerCfgPkbitsSec,
                         _bcmFieldInternalVariable,0, &tlv);
            tlv->value = &(f_pl->cfg.pkbits_sec);
            TLV_WRITE(unit, tlv, ptr, position);
            TLV_CREATE(_bcmFieldInternalPolicerCfgMaxPkbitsSec,
                                   _bcmFieldInternalVariable,0, &tlv);
            tlv->value = &(f_pl->cfg.max_pkbits_sec);
            TLV_WRITE(unit, tlv, ptr, position);
            TLV_CREATE(_bcmFieldInternalPolicerCfgPkbitsBurst ,
                                   _bcmFieldInternalVariable,0, &tlv);
            tlv->value = &(f_pl->cfg.pkbits_burst);
            TLV_WRITE(unit, tlv, ptr, position);
            TLV_CREATE(_bcmFieldInternalPolicerCfgKbitsCurrent,
                                   _bcmFieldInternalVariable,0, &tlv);
            tlv->value = &(f_pl->cfg.kbits_current);
            TLV_WRITE(unit, tlv, ptr, position);
            TLV_CREATE(_bcmFieldInternalPolicerCfgActionId,
                                   _bcmFieldInternalVariable,0, &tlv);
            tlv->value = &(f_pl->cfg.action_id);
            TLV_WRITE(unit, tlv, ptr, position);
            TLV_CREATE(_bcmFieldInternalPolicerCfgSharingMode,
                                   _bcmFieldInternalVariable,0, &tlv);
            tlv->value = &(f_pl->cfg.sharing_mode);
            TLV_WRITE(unit, tlv, ptr, position);
            TLV_CREATE(_bcmFieldInternalPolicerCfgEntropyId,
                                  _bcmFieldInternalVariable,0, &tlv);
            tlv->value = &(f_pl->cfg.entropy_id);
            TLV_WRITE(unit, tlv, ptr, position);
            TLV_CREATE(_bcmFieldInternalPolicerCfgPoolId,
                                   _bcmFieldInternalVariable,0, &tlv);
            tlv->value = &(f_pl->cfg.pool_id);
            TLV_WRITE(unit, tlv, ptr, position);
            TLV_CREATE(_bcmFieldInternalPolicerSwRefCount,
                                   _bcmFieldInternalVariable,0, &tlv);
            tlv->value = &(f_pl->sw_ref_count);
            TLV_WRITE(unit, tlv, ptr, position);
            TLV_CREATE(_bcmFieldInternalPolicerHwRefCount,
                                   _bcmFieldInternalVariable,0, &tlv);
            tlv->value = &(f_pl->hw_ref_count);
            TLV_WRITE(unit, tlv, ptr, position);
            TLV_CREATE(_bcmFieldInternalPolicerLevel,
                                   _bcmFieldInternalVariable,0, &tlv);
            tlv->value = &(f_pl->level);
            TLV_WRITE(unit, tlv, ptr, position);
            TLV_CREATE(_bcmFieldInternalPolicerPoolIndex,
                                  _bcmFieldInternalVariable,0,&tlv);
            tlv->value = &(f_pl->pool_index);
            TLV_WRITE(unit, tlv, ptr, position);
            TLV_CREATE(_bcmFieldInternalPolicerHwIndex,
                                   _bcmFieldInternalVariable,0, &tlv);
            tlv->value = &(f_pl->hw_index);
            TLV_WRITE(unit, tlv, ptr, position);
            TLV_CREATE(_bcmFieldInternalPolicerHwFlags,
                                   _bcmFieldInternalVariable,0, &tlv);
            tlv->value = &(f_pl->hw_flags);
            TLV_WRITE(unit, tlv, ptr, position);
            TLV_CREATE(_bcmFieldInternalPolicerStageId,
                                   _bcmFieldInternalVariable,0, &tlv);
            tlv->value = &(f_pl->stage_id);
            TLV_WRITE(unit, tlv, ptr, position);

            TLV_CREATE(_bcmFieldInternalEndStructPolicer,
                        _bcmFieldInternalVariable, 0, &tlv);
            tlv->value = &endmarkerpolicer;
            TLV_WRITE(unit, tlv, ptr, position);
            count ++;

        }
            f_pl = f_pl->next;
        }
    }

    count = 0;

    /* _field_stat_t */

    /* In the encoded length below , first ten bits is value 17 [no of types],
     * next 22 bits is stat count in hash. Here number of types indicate counts
     * of elements in the current datastructure.
     */

    TLV_CREATE(_bcmFieldInternalControlStathash, _bcmFieldInternalVariable,
                (stat_count | (17 << _FP_WB_TLV_TYPE_SHIFT)), &tlv);
    tlv->value = &(type_stat);
    TLV_WRITE(unit, tlv, ptr, position);

    /* Iterate over all hash buckets. */
    for (idx = 0; idx < _FP_HASH_SZ(fc); idx++) {
        /* Iterate over entries in each bucket. */
        f_st = fc->stat_hash[idx];
        while (NULL != f_st) {
        if((f_st->sid != -1)  && (f_st->stage_id == _BCM_FIELD_STAGE_INGRESS)) {

            TLV_CREATE(_bcmFieldInternalStatsId,
                                   _bcmFieldInternalVariable,0, &tlv);
            tlv->value = &(f_st->sid);
            TLV_WRITE(unit, tlv, ptr, position);
            TLV_CREATE(_bcmFieldInternalStatSwRefCount,
                                    _bcmFieldInternalVariable,0, &tlv);
            tlv->value = &(f_st->sw_ref_count);
            TLV_WRITE(unit, tlv, ptr, position);
            TLV_CREATE(_bcmFieldInternalStatHwRefCount,
                                    _bcmFieldInternalVariable,0, &tlv);
            tlv->value = &(f_st->hw_ref_count);
            TLV_WRITE(unit, tlv, ptr, position);
            TLV_CREATE(_bcmFieldInternalStatOffsetMode,
                                    _bcmFieldInternalVariable,0, &tlv);
            tlv->value = &(f_st->offset_mode);
            TLV_WRITE(unit, tlv, ptr, position);
            TLV_CREATE(_bcmFieldInternalStatPoolIndex,
                                    _bcmFieldInternalVariable,0, &tlv);
            tlv->value = &(f_st->pool_index);
            TLV_WRITE(unit, tlv, ptr, position);
            TLV_CREATE(_bcmFieldInternalStatHwIndex,
                                    _bcmFieldInternalVariable,0, &tlv);
            tlv->value = &(f_st->hw_index);
            TLV_WRITE(unit, tlv, ptr, position);
            TLV_CREATE(_bcmFieldInternalStatHwFlags,
                                    _bcmFieldInternalVariable,0, &tlv);
            tlv->value = &(f_st->hw_flags);
            TLV_WRITE(unit, tlv, ptr, position);
            TLV_CREATE(_bcmFieldInternalStatnStat,
                                    _bcmFieldInternalVariable,0, &tlv);
            tlv->value = &(f_st->nstat);
            TLV_WRITE(unit, tlv, ptr, position);
            TLV_CREATE(_bcmFieldInternalStatArr,
                                    _bcmFieldInternalArray,f_st->nstat,
                                    &tlv);
            tlv->value = (f_st->stat_arr);
            TLV_WRITE(unit, tlv, ptr, position);
            TLV_CREATE(_bcmFieldInternalStatHwStat,
                                    _bcmFieldInternalVariable,0, &tlv);
            tlv->value = &(f_st->hw_stat);
            TLV_WRITE(unit, tlv, ptr, position);
            TLV_CREATE(_bcmFieldInternalStatHwMode,
                                    _bcmFieldInternalVariable,0, &tlv);
            tlv->value = &(f_st->hw_mode);
            TLV_WRITE(unit, tlv, ptr, position);
            TLV_CREATE(_bcmFieldInternalStatHwEntryCount,
                                    _bcmFieldInternalVariable,0, &tlv);
            tlv->value = &(f_st->hw_entry_count);
            TLV_WRITE(unit, tlv, ptr, position);
            TLV_CREATE(_bcmFieldInternalStatGid,
                                    _bcmFieldInternalVariable,0, &tlv);
            tlv->value = &(f_st->gid);
            TLV_WRITE(unit, tlv, ptr, position);
            TLV_CREATE(_bcmFieldInternalStatStageId,
                                    _bcmFieldInternalVariable,0, &tlv);
            tlv->value = &(f_st->stage_id);
            TLV_WRITE(unit, tlv, ptr, position);
            TLV_CREATE(_bcmFieldInternalStatStatValues,
                                    _bcmFieldInternalArray,f_st->nstat,
                                    &tlv);
            tlv->value = (f_st->stat_values);
            TLV_WRITE(unit, tlv, ptr, position);
            TLV_CREATE(_bcmFieldInternalStatFlexMode,
                                    _bcmFieldInternalVariable,0, &tlv);
            tlv->value = &(f_st->flex_mode);
            TLV_WRITE(unit, tlv, ptr, position);

            TLV_CREATE(_bcmFieldInternalEndStructStat,
                               _bcmFieldInternalVariable, 0, &tlv);
            tlv->value = &endmarkerstat;
            TLV_WRITE(unit, tlv, ptr, position);
            count ++;
        }
            f_st = f_st->next;
        }
    }

    /* _field_hints_t */

    count = 0;
    TLV_CREATE(_bcmFieldInternalControlHintHash,
               _bcmFieldInternalVariable, hints_count, &tlv);
    TLV_WRITE(unit, tlv, ptr, position);
    /* Iterate over all hash buckets. */
    for (idx = 0; idx < _FP_HASH_SZ(fc); idx++) {
        /* Iterate over entries in each bucket. */
        f_ht = fc->hints_hash[idx];
        while (NULL != f_ht) {
        if(f_ht->hintid != -1) {
           TLV_CREATE(_bcmFieldInternalHintHintid,
                         _bcmFieldInternalVariable,0, &tlv);
           tlv->value = &(f_ht->hintid);
           TLV_WRITE(unit, tlv, ptr, position);

          /* _field_hint_t */
          /* In the encoded length below , first ten bits is value 8 [no of
           * types], next 22 bits is hints count in hash. Here number of types
           * indicate counts of elements in the current datastructure.
           */


           TLV_CREATE(_bcmFieldInternalHintHints, _bcmFieldInternalArray,
                      f_ht->hint_count
                      | (8 << _FP_WB_TLV_TYPE_SHIFT), &tlv);
           tlv->value = &(type_hint);
           TLV_WRITE(unit, tlv, ptr, position);
           hint = f_ht->hints;

           for (hintidx = 0; hintidx < f_ht->hint_count; hintidx++) {

                TLV_CREATE(_bcmFieldInternalHintHintsHintType,
                                        _bcmFieldInternalVariable,0,&tlv);
                tlv->value = &(hint->hint->hint_type);
                TLV_WRITE(unit, tlv, ptr, position);

                TLV_CREATE(_bcmFieldInternalHintHintsQual,
                                        _bcmFieldInternalVariable,0, &tlv);
                tlv->value = &(hint->hint->qual);
                TLV_WRITE(unit, tlv, ptr, position);

                TLV_CREATE(_bcmFieldInternalHintHintsMaxValues,
                                        _bcmFieldInternalVariable,0, &tlv);
                tlv->value = &(hint->hint->max_values);
                TLV_WRITE(unit, tlv, ptr, position);

                TLV_CREATE(_bcmFieldInternalHintHintsStartBit,
                                        _bcmFieldInternalVariable,0, &tlv);
                tlv->value = &(hint->hint->start_bit);
                TLV_WRITE(unit, tlv, ptr, position);

                TLV_CREATE(_bcmFieldInternalHintHintsEndBit,
                                _bcmFieldInternalVariable,0, &tlv);
                tlv->value = &(hint->hint->end_bit);
                TLV_WRITE(unit, tlv, ptr, position);

                TLV_CREATE(_bcmFieldInternalHintHintsFlags,
                            _bcmFieldInternalVariable,0, &tlv);
                tlv->value = &(hint->hint->flags);
                TLV_WRITE(unit, tlv, ptr, position);

                TLV_CREATE(_bcmFieldInternalHintHintsMaxGrpSize,
                           _bcmFieldInternalVariable,0, &tlv);
                tlv->value = &(hint->hint->max_group_size);
                TLV_WRITE(unit, tlv, ptr, position);

                TLV_CREATE(_bcmFieldInternalEndStructHintHints,
                           _bcmFieldInternalVariable, 0, &tlv);
                TLV_WRITE(unit, tlv, ptr, position);

                hint = hint->next;
           }


           TLV_CREATE(_bcmFieldInternalHintGrpRefCount,
                                    _bcmFieldInternalVariable,0, &tlv);
           tlv->value = &(f_ht->grp_ref_count);
           TLV_WRITE(unit, tlv, ptr, position);

           TLV_CREATE(_bcmFieldInternalHintCount,
                                        _bcmFieldInternalVariable,0, &tlv);
           tlv->value = &(f_ht->hint_count);
           TLV_WRITE(unit, tlv, ptr, position);

           TLV_CREATE(_bcmFieldInternalEndStructHint,
                                 _bcmFieldInternalVariable, 0, &tlv);
           tlv->value = &endmarkerhint;
           TLV_WRITE(unit, tlv, ptr, position);
           count ++;

        }
        f_ht = f_ht->next;
        }
    }

    sal_free(tlv);

    return BCM_E_NONE;
}
/* Function:
 * _field_udf_sync
 *
 * Purpose:
 *    Sync _field_udf_t structure
 *
 * Paramaters:
 * unit             - (IN) BCM device number
 * scache_ptr       - (IN) Pointer to device field control structure
 * ptr              -
 * udf              -
 * Returns:
 *     BCM_E_XXX
 */
int
_field_udf_sync(int unit, uint8 *ptr, uint32 *pos, _field_udf_t *udf)
{
    _field_tlv_t *tlv   = NULL;              /* TLV data structure */
    int endmarker       = _FIELD_WB_EM_UDF;  /* End marker for UDF */

    TLV_CREATE(_bcmFieldInternalControlUdfValid,
                                   _bcmFieldInternalVariable, 0, &tlv);
    tlv->value = &(udf->valid);
    TLV_WRITE(unit,tlv,ptr,pos);

    TLV_CREATE(_bcmFieldInternalControlUdfUseCount,
                                   _bcmFieldInternalVariable, 0, &tlv);
    tlv->value = &(udf->use_count);
    TLV_WRITE(unit,tlv,ptr,pos);

    TLV_CREATE(_bcmFieldInternalControlUdfNum,
                                   _bcmFieldInternalVariable, 0, &tlv);
    tlv->value = &(udf->udf_num);
    TLV_WRITE(unit,tlv,ptr,pos);

    TLV_CREATE(_bcmFieldInternalControlUdfUserNum,
                                   _bcmFieldInternalVariable, 0, &tlv);
    tlv->value = &(udf->user_num);
    TLV_WRITE(unit,tlv,ptr,pos);

    TLV_CREATE(_bcmFieldInternalControlEndStructUdf,
                                   _bcmFieldInternalVariable, 0, &tlv);
    tlv->value = &endmarker;
    TLV_WRITE(unit,tlv,ptr,pos);

    sal_free(tlv);
    return BCM_E_NONE;

}

/* Function:
 * _field_control_sync
 *
 * Purpose:
 *    Sync _field_control_t structure
 *
 * Paramaters:
 * unit             - (IN) BCM device number
 * fc               - (IN) Pointer to device field control structure
 *
 * Returns:
 *     BCM_E_XXX
 */

int
_field_control_sync(int unit)
{

    _field_tlv_t *tlv   = NULL;     /* Tlv data structure */
    uint8 *scache_ptr   = NULL;
                                    /* Scache Pointer */
    _field_group_t *fg;             /* Group information */
    int group_count    = 0;         /* Number of groups */
    int *gid           = NULL;      /* Array of groupids */
    int hintbmp_length = 0;         /* length of hint bmp */
    uint32 endmarker   = _FIELD_WB_EM_CONTROL;
                                    /* End marker for fc */
    int pos            = 0;         /* variable for loops */

    int type_udf[5] = {             /* types in _field_udf_t */
     (int) ((_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)
            | _bcmFieldInternalControlUdfValid),
     (int) ((_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)
            | _bcmFieldInternalControlUdfUseCount),
     (int) ((_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)
            | _bcmFieldInternalControlUdfNum),
     (int)  ((_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)
            | _bcmFieldInternalControlUdfUserNum),
     (int) ((_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)
           | _bcmFieldInternalControlEndStructUdf),
                      };
    uint32 *position;             /* Scache position */
    _field_control_t *fc;         /* field control Structure */

    WB_FIELD_CONTROL_GET(fc, scache_ptr, position);


    TLV_CREATE(_bcmFieldInternalControl,
               _bcmFieldInternalVariable, 0, &tlv);
    TLV_WRITE(unit, tlv, scache_ptr, position);

    TLV_CREATE(_bcmFieldInternalControlFlags,
                 _bcmFieldInternalVariable, 0, &tlv);
    tlv->value = &(fc->flags);
    TLV_WRITE(unit, tlv, scache_ptr, position);

    fg = fc->groups;
    while (fg != NULL) {
        if (fg->stage_id == _BCM_FIELD_STAGE_INGRESS) {
            group_count ++ ;
        }
        fg = fg->next;
	}

     _FP_XGS3_ALLOC(gid, (sizeof(uint32) * group_count), "alloc for groups");

    fg = fc->groups;
    while (fg != NULL) {
        if (fg->stage_id == _BCM_FIELD_STAGE_INGRESS) {
            gid[pos++] = fg->gid;
        }
        fg = fg->next;
	}

    /* In the encoded length below , first ten bits is value 5 [no of types],
     * next 22 bits is no of udfs. Here number of types indicate counts of
     * elements in the current datastructure.
     */

    TLV_CREATE(_bcmFieldInternalControlUdfDetails, _bcmFieldInternalArray,
               (BCM_FIELD_USER_NUM_UDFS | (5 << _FP_WB_TLV_TYPE_SHIFT)), &tlv);
    tlv->value = &type_udf;
    TLV_WRITE(unit, tlv, scache_ptr, position);

    for (pos = 0; pos < BCM_FIELD_USER_NUM_UDFS; pos++) {
        BCM_IF_ERROR_RETURN(_field_udf_sync(unit, scache_ptr,
                                    position, &(fc->udf[pos])));
    }


    TLV_CREATE(_bcmFieldInternalControlGroups,
               _bcmFieldInternalArray, group_count, &tlv);
    tlv->value = &gid;
    TLV_WRITE(unit, tlv, scache_ptr, position);
    sal_free(gid);

    TLV_CREATE(_bcmFieldInternalControlPolicerCount,
                                          _bcmFieldInternalVariable,0,&tlv);
    tlv->value = &(fc->policer_count);
    TLV_WRITE(unit, tlv, scache_ptr, position);

    TLV_CREATE(_bcmFieldInternalControlStatCount,
                                          _bcmFieldInternalVariable,0,&tlv);
    tlv->value = &(fc->stat_count);
    TLV_WRITE(unit, tlv, scache_ptr, position);

    hintbmp_length = SHR_BITALLOCSIZE(_FP_HINT_ID_MAX);
    TLV_CREATE(_bcmFieldInternalControlHintBmp,
                                          _bcmFieldInternalArray,
                                          hintbmp_length, &tlv);
    tlv->value = fc->hintid_bmp.w;
    TLV_WRITE(unit, tlv, scache_ptr, position);

    TLV_CREATE(_bcmFieldInternalControlLastAllocatedLtId,
                           _bcmFieldInternalVariable,0,&tlv);
    tlv->value = &(fc->last_allocated_lt_eid);
    TLV_WRITE(unit, tlv, scache_ptr, position);

    BCM_IF_ERROR_RETURN(_field_hash_sync(unit));

    TLV_CREATE(_bcmFieldInternalEndStructControl,
                             _bcmFieldInternalVariable, 0, &tlv);
    tlv->value = &endmarker;
    TLV_WRITE(unit, tlv, scache_ptr, position);

    sal_free(tlv);
    return BCM_E_NONE;


}
/* Function:
 * _field_datacontroldataqualifier_sync
 *
 * Purpose:
 *    Sync _field_data_qualifier_p structure
 *
 * Paramaters:
 * unit             - (IN) BCM device number
 * fdp              - (IN) Pointer to Field Data Qualifier structure
 *
 * Returns:
 *     BCM_E_XXX
 */

int
_field_datacontroldataqualifier_sync(int unit, _field_data_qualifier_p fdp)
{

    _field_tlv_t *tlv = NULL;              /* &tlv data */
    uint8 *ptr = NULL;                     /* Scache Pointer */
    _field_control_t *fc = NULL;           /* Field control structure. */
    uint32 *position = NULL;               /* Scache Position */
    int endmarker = _FIELD_WB_EM_DATAQUAL;
                                           /* End Marker */

    WB_FIELD_CONTROL_GET(fc, ptr, position);


    TLV_CREATE(_bcmFieldInternalDataControlDataQualQid,
                                _bcmFieldInternalVariable,0, &tlv);
    tlv->value = &(fdp->qid);
    TLV_WRITE(unit, tlv, ptr, position);


    TLV_CREATE(_bcmFieldInternalDataControlDataQualUdfSpec,
                                _bcmFieldInternalArray, 384, &tlv);
    tlv->value = &(fdp->spec);
    TLV_WRITE(unit, tlv, ptr, position);


    TLV_CREATE(_bcmFieldInternalDataControlDataQualOffsetBase,
                                _bcmFieldInternalVariable,0, &tlv);
    tlv->value = &(fdp->offset_base);
    TLV_WRITE(unit, tlv, ptr, position);


    TLV_CREATE(_bcmFieldInternalDataControlDataQualOffset,
                            _bcmFieldInternalVariable,0, &tlv);
    tlv->value = &(fdp->offset);
    TLV_WRITE(unit, tlv, ptr, position);

    TLV_CREATE(_bcmFieldInternalDataControlDataQualByteOffset,
                                _bcmFieldInternalVariable,0, &tlv);
    tlv->value = &(fdp->byte_offset);
    TLV_WRITE(unit, tlv, ptr, position);


    TLV_CREATE(_bcmFieldInternalDataControlDataQualHwBmp,
                                _bcmFieldInternalVariable,0, &tlv);
    tlv->value = &(fdp->hw_bmap);
    TLV_WRITE(unit, tlv, ptr, position);

    TLV_CREATE(_bcmFieldInternalDataControlDataQualFlags,
                            _bcmFieldInternalVariable,0, &tlv);
    tlv->value = &(fdp->flags);
    TLV_WRITE(unit, tlv, ptr, position);


    TLV_CREATE(_bcmFieldInternalDataControlDataQualElemCount,
                                _bcmFieldInternalVariable,0, &tlv);
    tlv->value = &(fdp->elem_count);
    TLV_WRITE(unit, tlv, ptr, position);


    TLV_CREATE(_bcmFieldInternalDataControlDataQualLength,
                                _bcmFieldInternalVariable,0, &tlv);
    tlv->value = &(fdp->length);
    TLV_WRITE(unit, tlv, ptr, position);

    TLV_CREATE(_bcmFieldInternalDataControlEndStructDataQual,
                                _bcmFieldInternalVariable, 0, &tlv);
    tlv->value = &(endmarker);
    TLV_WRITE(unit, tlv, ptr, position);

    sal_free(tlv);

    return BCM_E_NONE;
}

/* Function:
 * _field_extractor_sync
 *
 * Purpose:
 *    recover _field_slice_t structure
 *
 * Paramaters:
 * unit          - (IN) BCM device number
 * fc            - (IN) field control structure of the unit.
 * ext_sel       - (IN) address of start of array in slice structure.
 * count         - (IN) Number of extractor structures to be synced.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
_field_extractor_sync(int unit,
                      _field_ext_sel_t *ext_sel, int count)
{

    _field_tlv_t *tlv = NULL;  /* tlv structure */
    int i = 0;                 /* loop variable */
    int endmarker = _FIELD_WB_EM_EXTRACTOR;
                               /* End marker */
    uint8 *ptr;                /* pointer to scache_ptr (base) */
    uint32 *pos;               /* Pointer to scache_pos (offset) */
    _field_control_t *fc;      /* field Control structure */
    int type_ext_sel[32] = {   /* types in _field_ext_sel_t */
    (int) (_bcmFieldInternalExtl1e32Sel
           | (_bcmFieldInternalArray << _FP_WB_TLV_BASIC_TYPE_SHIFT)),
    (int) (_bcmFieldInternalExtl1e16Sel
           |(_bcmFieldInternalArray << _FP_WB_TLV_BASIC_TYPE_SHIFT)),
    (int) (_bcmFieldInternalExtl1e8Sel
           | (_bcmFieldInternalArray << _FP_WB_TLV_BASIC_TYPE_SHIFT)),
    (int) (_bcmFieldInternalExtl1e4Sel
           | (_bcmFieldInternalArray << _FP_WB_TLV_BASIC_TYPE_SHIFT)),
    (int) (_bcmFieldInternalExtl1e2Sel
           | (_bcmFieldInternalArray << _FP_WB_TLV_BASIC_TYPE_SHIFT)),
    (int) (_bcmFieldInternalExtl2e16Sel
           | (_bcmFieldInternalArray << _FP_WB_TLV_BASIC_TYPE_SHIFT)),
    (int) (_bcmFieldInternalExtl3e1Sel
           | (_bcmFieldInternalArray << _FP_WB_TLV_BASIC_TYPE_SHIFT)),
    (int) (_bcmFieldInternalExtl3e2Sel
           | (_bcmFieldInternalArray << _FP_WB_TLV_BASIC_TYPE_SHIFT)),
    (int) (_bcmFieldInternalExtl3e4Sel
           |(_bcmFieldInternalArray << _FP_WB_TLV_BASIC_TYPE_SHIFT)),
    (int) (_bcmFieldInternalExtPmuxSel
           | (_bcmFieldInternalArray << _FP_WB_TLV_BASIC_TYPE_SHIFT)),
    (int) (_bcmFieldInternalExtIntraSlice
           | (_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)),
    (int) (_bcmFieldInternalExtSecondary
           | (_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)),
    (int) (_bcmFieldInternalExtIpbmpRes
           | (_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)),
    (int) (_bcmFieldInternalExtNorml3l4
           | (_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)),
    (int) (_bcmFieldInternalExtNormmac
           | (_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)),
    (int) (_bcmFieldInternalExtAuxTagaSel
           | (_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)),
    (int) (_bcmFieldInternalExtAuxTagbSel
           | (_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)),
    (int) (_bcmFieldInternalExtAuxTagcSel
           | (_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)),
    (int) (_bcmFieldInternalExtAuxTagdSel
           | (_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)),
    (int) (_bcmFieldInternalExtTcpFnSel
           | (_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)),
    (int) (_bcmFieldInternalExtTosFnSel
           | (_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)),
    (int) (_bcmFieldInternalExtTtlFnSel
           | (_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)),
    (int) (_bcmFieldInternalExtClassIdaSel
           | (_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)),
    (int) (_bcmFieldInternalExtClassIdbSel
           | (_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)),
    (int) (_bcmFieldInternalExtClassIdcSel
           | (_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)),
    (int) (_bcmFieldInternalExtClassIddSel
           | (_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)),
    (int) (_bcmFieldInternalExtSrcContaSel
           | (_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)),
    (int) (_bcmFieldInternalExtSrcContbSel
           | (_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)),
    (int) (_bcmFieldInternalExtSrcDestCont0Sel
           | (_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)),
    (int) (_bcmFieldInternalExtSrcDestCont1Sel
           | (_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)),
    (int) (_bcmFieldInternalExtKeygenIndex
           | (_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)),
    (int) (_bcmFieldInternalEndStructExtractor
           | (_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)),
    };

    WB_FIELD_CONTROL_GET(fc, ptr, pos);


    /* In the encoded length below , first ten bits is value 32 [no of types],
     * next 22 bits is count of extractors. Here number of types indicate counts
     * of elements in the current datastructure.
     */

    TLV_CREATE(_bcmFieldInternalExtractor,
               _bcmFieldInternalArray, (count
               | (32 << _FP_WB_TLV_TYPE_SHIFT)), &tlv);
    tlv->value = &type_ext_sel[0];
    TLV_WRITE(unit,tlv,ptr,pos);

    for (i = 0; i < count; i++) {
        TLV_CREATE(_bcmFieldInternalExtl1e32Sel,
                                       _bcmFieldInternalArray, 4, &tlv);
        tlv->value = ext_sel->l1_e32_sel;
        TLV_WRITE(unit,tlv,ptr,pos);

        TLV_CREATE(_bcmFieldInternalExtl1e16Sel,
                                       _bcmFieldInternalArray, 7, &tlv);
        tlv->value = ext_sel->l1_e16_sel;
        TLV_WRITE(unit,tlv,ptr,pos);

        TLV_CREATE(_bcmFieldInternalExtl1e8Sel,
                                       _bcmFieldInternalArray, 7, &tlv);
        tlv->value = ext_sel->l1_e8_sel;
        TLV_WRITE(unit,tlv,ptr,pos);

        TLV_CREATE(_bcmFieldInternalExtl1e4Sel,
                                       _bcmFieldInternalArray, 8, &tlv);
        tlv->value = ext_sel->l1_e4_sel;
        TLV_WRITE(unit,tlv,ptr,pos);

        TLV_CREATE(_bcmFieldInternalExtl1e2Sel,
                                       _bcmFieldInternalArray, 8, &tlv);
        tlv->value = ext_sel->l1_e2_sel;
        TLV_WRITE(unit,tlv,ptr,pos);

        TLV_CREATE(_bcmFieldInternalExtl2e16Sel,
                                       _bcmFieldInternalArray, 10, &tlv);
        tlv->value = ext_sel->l2_e16_sel;
        TLV_WRITE(unit,tlv,ptr,pos);

        TLV_CREATE(_bcmFieldInternalExtl3e1Sel,
                                       _bcmFieldInternalArray, 2, &tlv);
        tlv->value = ext_sel->l3_e1_sel;
        TLV_WRITE(unit,tlv,ptr,pos);

        TLV_CREATE(_bcmFieldInternalExtl3e2Sel,
                                       _bcmFieldInternalArray, 5, &tlv);
        tlv->value = ext_sel->l3_e2_sel;
        TLV_WRITE(unit,tlv,ptr,pos);

        TLV_CREATE(_bcmFieldInternalExtl3e4Sel,
                                       _bcmFieldInternalArray, 21, &tlv);
        tlv->value = ext_sel->l3_e4_sel;
        TLV_WRITE(unit,tlv,ptr,pos);

        TLV_CREATE(_bcmFieldInternalExtPmuxSel,
                                       _bcmFieldInternalArray, 15, &tlv);
        tlv->value = ext_sel->pmux_sel;
        TLV_WRITE(unit,tlv,ptr,pos);

        TLV_CREATE(_bcmFieldInternalExtIntraSlice,
                                       _bcmFieldInternalVariable, 4, &tlv);
        tlv->value = &ext_sel->intraslice;
        TLV_WRITE(unit,tlv,ptr,pos);

        TLV_CREATE(_bcmFieldInternalExtSecondary,
                                       _bcmFieldInternalVariable, 4, &tlv);
        tlv->value = &ext_sel->secondary;
        TLV_WRITE(unit,tlv,ptr,pos);

        TLV_CREATE(_bcmFieldInternalExtIpbmpRes,
                                       _bcmFieldInternalVariable, 4, &tlv);
        tlv->value = &ext_sel->ipbm_present;
        TLV_WRITE(unit,tlv,ptr,pos);

        TLV_CREATE(_bcmFieldInternalExtNorml3l4,
                                       _bcmFieldInternalVariable, 4, &tlv);
        tlv->value = &ext_sel->normalize_l3_l4_sel;
        TLV_WRITE(unit,tlv,ptr,pos);

        TLV_CREATE(_bcmFieldInternalExtNormmac,
                                       _bcmFieldInternalVariable, 4, &tlv);
        tlv->value = &ext_sel->normalize_mac_sel;
        TLV_WRITE(unit,tlv,ptr,pos);

        TLV_CREATE(_bcmFieldInternalExtAuxTagaSel,
                                       _bcmFieldInternalVariable, 4, &tlv);
        tlv->value = &ext_sel->aux_tag_a_sel;
        TLV_WRITE(unit,tlv,ptr,pos);

        TLV_CREATE(_bcmFieldInternalExtAuxTagbSel,
                                       _bcmFieldInternalVariable, 4, &tlv);
        tlv->value = &ext_sel->aux_tag_b_sel;
        TLV_WRITE(unit,tlv,ptr,pos);


        TLV_CREATE(_bcmFieldInternalExtAuxTagcSel,
                                       _bcmFieldInternalVariable, 4, &tlv);
        tlv->value = &ext_sel->aux_tag_c_sel;
        TLV_WRITE(unit,tlv,ptr,pos);

        TLV_CREATE(_bcmFieldInternalExtAuxTagdSel,
                                       _bcmFieldInternalVariable, 4, &tlv);
        tlv->value = &ext_sel->aux_tag_d_sel;
        TLV_WRITE(unit,tlv,ptr,pos);

        TLV_CREATE(_bcmFieldInternalExtTcpFnSel,
                                       _bcmFieldInternalVariable, 4, &tlv);
        tlv->value = &ext_sel->tcp_fn_sel;
        TLV_WRITE(unit,tlv,ptr,pos);

            TLV_CREATE(_bcmFieldInternalExtTosFnSel,
                                       _bcmFieldInternalVariable, 4, &tlv);
        tlv->value = &ext_sel->tos_fn_sel;
        TLV_WRITE(unit,tlv,ptr,pos);

        TLV_CREATE(_bcmFieldInternalExtTtlFnSel,
                                       _bcmFieldInternalVariable, 4, &tlv);
        tlv->value = &ext_sel->ttl_fn_sel;
        TLV_WRITE(unit,tlv,ptr,pos);

        TLV_CREATE(_bcmFieldInternalExtClassIdaSel,
                                       _bcmFieldInternalVariable, 4, &tlv);
        tlv->value = &ext_sel->class_id_cont_a_sel;
        TLV_WRITE(unit,tlv,ptr,pos);

        TLV_CREATE(_bcmFieldInternalExtClassIdbSel,
                                       _bcmFieldInternalVariable, 4, &tlv);
        tlv->value = &ext_sel->class_id_cont_b_sel;
        TLV_WRITE(unit,tlv,ptr,pos);

        TLV_CREATE(_bcmFieldInternalExtClassIdcSel,
                                       _bcmFieldInternalVariable, 4, &tlv);
        tlv->value = &ext_sel->class_id_cont_c_sel;
        TLV_WRITE(unit,tlv,ptr,pos);

        TLV_CREATE(_bcmFieldInternalExtClassIddSel,
                                       _bcmFieldInternalVariable, 4, &tlv);
        tlv->value = &ext_sel->class_id_cont_d_sel;
        TLV_WRITE(unit,tlv,ptr,pos);

        TLV_CREATE(_bcmFieldInternalExtSrcContaSel,
                                       _bcmFieldInternalVariable, 4, &tlv);
        tlv->value = &ext_sel->src_cont_a_sel;
        TLV_WRITE(unit,tlv,ptr,pos);

        TLV_CREATE(_bcmFieldInternalExtSrcContbSel,
                                       _bcmFieldInternalVariable, 4, &tlv);
        tlv->value = &ext_sel->src_cont_b_sel;
        TLV_WRITE(unit,tlv,ptr,pos);

        TLV_CREATE(_bcmFieldInternalExtSrcDestCont0Sel,
                                       _bcmFieldInternalVariable, 4, &tlv);
        tlv->value = &ext_sel->src_dest_cont_0_sel;
        TLV_WRITE(unit,tlv,ptr,pos);

        TLV_CREATE(_bcmFieldInternalExtSrcDestCont1Sel,
                                       _bcmFieldInternalVariable, 4, &tlv);
        tlv->value = &ext_sel->src_dest_cont_1_sel;
        TLV_WRITE(unit,tlv,ptr,pos);


        TLV_CREATE(_bcmFieldInternalExtKeygenIndex,
                                      _bcmFieldInternalVariable, 4, &tlv);
        tlv->value = &ext_sel->keygen_index;
        TLV_WRITE(unit,tlv,ptr,pos);

        TLV_CREATE(_bcmFieldInternalEndStructExtractor,
                                      _bcmFieldInternalVariable, 4, &tlv);
        tlv->value = &endmarker;
        TLV_WRITE(unit,tlv,ptr,pos);


    }
    sal_free(tlv);
    return BCM_E_NONE;

}
/* Function:
 * _field_datacontrolethertype_sync
 *
 * Purpose:
 *    Sync _field_data_ethertype_t structure
 *
 * Paramaters:
 * unit             - (IN) BCM device number
 * fd               - (IN) Pointer to Field Data Control structure
 *
 * Returns:
 *     BCM_E_XXX
 */

int
_field_datacontrolethertype_sync(int unit, _field_data_control_t *fd)
{

    _field_tlv_t *tlv = NULL;              /* Tlv data structure */
    uint8 *ptr = NULL;                     /* Scache Pointer */
    _field_control_t *fc = NULL;           /* Field control structure. */
    uint32 *position = NULL;               /* Scache Position */
    int endmarker = _FIELD_WB_EM_DATAETYPE;
                                           /* End marker */
    int i = 0;                             /* Loop variable */
    int type_etype[6] = {                  /* types in _field_ethertype_t */
    (int)((_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)
           | _bcmFieldInternalDataControlEthertypeRefCount),
    (int)((_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)
           | _bcmFieldInternalDataControlEthertypeL2),
    (int)((_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)
           | _bcmFieldInternalDataControlEthertypeVlanTag),
    (int)((_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)
           | _bcmFieldInternalDataControlEthertypePortEt),
    (int)((_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)
           | _bcmFieldInternalDataControlEthertypeRelOffset),
    (int)((_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)
           | _bcmFieldInternalDataControlEndStructEtype)
    };

    WB_FIELD_CONTROL_GET(fc, ptr, position);

    /* In the encoded length below , first ten bits is value 6 [no of types],
     * next 22 bits is ethertype count. Here number of types indicate counts of
     * elements in the current datastructure.
     */

    TLV_CREATE(_bcmFieldInternalDataControlEthertypeStruct,
               _bcmFieldInternalArray, _FP_DATA_ETHERTYPE_MAX
               | (6 << _FP_WB_TLV_TYPE_SHIFT), &tlv);
    tlv->value = &(type_etype);
    TLV_WRITE(unit, tlv, ptr, position);


    for (i = 0; i < _FP_DATA_ETHERTYPE_MAX; i++) {

    TLV_CREATE(_bcmFieldInternalDataControlEthertypeRefCount,
                                _bcmFieldInternalVariable,0, &tlv);
    tlv->value = &(fd->etype[i].ref_count);
    TLV_WRITE(unit, tlv, ptr, position);


    TLV_CREATE(_bcmFieldInternalDataControlEthertypeL2,
                                _bcmFieldInternalVariable,0, &tlv);
    tlv->value = &(fd->etype[i].l2);
    TLV_WRITE(unit, tlv, ptr, position);

    TLV_CREATE(_bcmFieldInternalDataControlEthertypeVlanTag,
                     _bcmFieldInternalVariable,0, &tlv);
    tlv->value = &(fd->etype[i].vlan_tag);
    TLV_WRITE(unit, tlv, ptr, position);



    TLV_CREATE(_bcmFieldInternalDataControlEthertypePortEt,
                     _bcmFieldInternalVariable,0, &tlv);
    tlv->value = &(fd->etype[i].ethertype);
    TLV_WRITE(unit, tlv, ptr, position);


    TLV_CREATE(_bcmFieldInternalDataControlEthertypeRelOffset,
    	                 _bcmFieldInternalVariable,0, &tlv);
    tlv->value = &(fd->etype[i].relative_offset);
    TLV_WRITE(unit, tlv, ptr, position);

    TLV_CREATE(_bcmFieldInternalDataControlEndStructEtype,
                                _bcmFieldInternalVariable, 0, &tlv);
    tlv->value = &(endmarker);
    TLV_WRITE(unit, tlv, ptr, position);
    }

    sal_free(tlv);

    return BCM_E_NONE;

}

int
_field_datacontrolprot_sync(int unit, _field_data_control_t *fd)
{

    _field_tlv_t *tlv = NULL;              /* Tlv data structure */
    uint8 *ptr = NULL;                     /* Scache Pointer */
    _field_control_t *fc = NULL;           /* Field control structure. */
    uint32 *position = NULL;               /* Scache Position */
    int endmarker = _FIELD_WB_EM_DATAPROT; /* End marker */
    int i = 0;                             /* Loop local variable */
    int type_prot[8] = {                   /* types in _field_protocol_t */
    (int)((_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)
           | _bcmFieldInternalDataControlProtIp4RefCount),
    (int)((_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)
           | _bcmFieldInternalDataControlProtIp6RefCount),
    (int)((_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)
           | _bcmFieldInternalDataControlProtFlags),
    (int)((_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)
           | _bcmFieldInternalDataControlProtIp),
    (int)((_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)
           | _bcmFieldInternalDataControlProtL2),
    (int)((_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)
           | _bcmFieldInternalDataControlProtVlanTag),
    (int)((_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)
           | _bcmFieldInternalDataControlProtRelOffset),
    (int)((_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)
           | _bcmFieldInternalDataControlEndStructProt)
    };

    WB_FIELD_CONTROL_GET(fc, ptr, position);

    /* In the encoded length below , first ten bits is value 8 [no of types],
     * next 22 bits is protocol count. Here number of types indicate counts of
     * elements in the current datastructure.
     */

    TLV_CREATE(_bcmFieldInternalDataControlProtStart,
                                _bcmFieldInternalArray,
    		                    _FP_DATA_IP_PROTOCOL_MAX
                                | (8 << _FP_WB_TLV_TYPE_SHIFT), &tlv);
    tlv->value = &(type_prot);
    TLV_WRITE(unit, tlv, ptr, position);


    for (i = 0; i < _FP_DATA_IP_PROTOCOL_MAX; i++) {


    TLV_CREATE(_bcmFieldInternalDataControlProtIp4RefCount,
                                  _bcmFieldInternalVariable,0, &tlv);
    tlv->value = &(fd->ip[i].ip4_ref_count);
    TLV_WRITE(unit, tlv, ptr, position);


    TLV_CREATE(_bcmFieldInternalDataControlProtIp6RefCount,
                                  _bcmFieldInternalVariable,0, &tlv);
    tlv->value = &(fd->ip[i].ip6_ref_count);
    TLV_WRITE(unit, tlv, ptr, position);


    TLV_CREATE(_bcmFieldInternalDataControlProtFlags,
                                  _bcmFieldInternalVariable,0, &tlv);
    tlv->value = &(fd->ip[i].flags);
    TLV_WRITE(unit, tlv, ptr, position);


    TLV_CREATE(_bcmFieldInternalDataControlProtIp,
                                  _bcmFieldInternalVariable,0, &tlv);
    tlv->value = &(fd->ip[i].ip);
    TLV_WRITE(unit, tlv, ptr, position);


    TLV_CREATE(_bcmFieldInternalDataControlProtL2,
                                  _bcmFieldInternalVariable,0, &tlv);
    tlv->value = &(fd->ip[i].l2);
    TLV_WRITE(unit, tlv, ptr, position);


    TLV_CREATE(_bcmFieldInternalDataControlProtVlanTag,
                 _bcmFieldInternalVariable, 0,&tlv);
    tlv->value = &(fd->etype[i].vlan_tag);
    TLV_WRITE(unit, tlv, ptr, position);


    TLV_CREATE(_bcmFieldInternalDataControlProtRelOffset,
                                  _bcmFieldInternalVariable,0, &tlv);
    tlv->value = &(fd->etype[i].relative_offset);
    TLV_WRITE(unit, tlv, ptr, position);

    TLV_CREATE(_bcmFieldInternalDataControlEndStructProt,
                                _bcmFieldInternalVariable, 0, &tlv);
    tlv->value = &(endmarker);
    TLV_WRITE(unit, tlv, ptr, position);

    }
    sal_free(tlv);

    return BCM_E_NONE;

}

int
_field_datacontroltcamentry_sync(int unit, _field_data_control_t *fd)
{

    _field_tlv_t *tlv = NULL;              /* Tlv data structure*/
    uint8 *ptr = NULL;                     /* Scache Pointer */
    _field_control_t *fc = NULL;           /* Field control structure. */
    uint32 *position = NULL;               /* Scache Position */
    int endmarker = _FIELD_WB_EM_DATATCAM; /* End marker */
    int tcam_count = 0;                    /* Count of TCAM */
    _field_data_tcam_entry_t *tarr= NULL;  /* Tcam entry pointer */
    int i = 0;                             /* Loop local variable */
    int type_tcam[3] = {                 /* types in _field_data_tcam_entry_t */
    (int)((_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)
           | _bcmFieldInternalDataControlTcamRefCount),
    (int)((_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)
           | _bcmFieldInternalDataControlTcamPriority),
    (int)((_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)
           | _bcmFieldInternalDataControlEndStructTcam)
    };

    WB_FIELD_CONTROL_GET(fc, ptr, position);

    tarr = fd->tcam_entry_arr;
    tcam_count = soc_mem_index_count(unit, FP_UDF_TCAMm);

    /* In the encoded length below , first ten bits is value 3 [no of types],
     * next 22 bits is tcam count. Here number of types indicate counts of
     * elements in the current datastructure.
     */


    TLV_CREATE(_bcmFieldInternalDataControlTcamStruct,
               _bcmFieldInternalArray, tcam_count
               | (3 << _FP_WB_TLV_TYPE_SHIFT),
                                &tlv);
    tlv->value = &(type_tcam);
    TLV_WRITE(unit, tlv, ptr, position);


    for (i = 0; i < tcam_count; i++) {

    TLV_CREATE(_bcmFieldInternalDataControlTcamRefCount,
                                _bcmFieldInternalVariable,0, &tlv);
    tlv->value = &(tarr[i].ref_count);
    TLV_WRITE(unit, tlv, ptr, position);


    TLV_CREATE(_bcmFieldInternalDataControlTcamPriority,
                                _bcmFieldInternalVariable,0, &tlv);
    tlv->value = &(tarr[i].priority);
    TLV_WRITE(unit, tlv, ptr, position);


    TLV_CREATE(_bcmFieldInternalDataControlEndStructTcam,
                                _bcmFieldInternalVariable, 0, &tlv);
    tlv->value = &(endmarker);
    TLV_WRITE(unit, tlv, ptr, position);

    }

    sal_free(tlv);

    return BCM_E_NONE;

}


/* Function:
 * _field_data_control_sync
 *
 * Purpose:
 *    Sync _field_data_control_t structure
 *
 * Paramaters:
 * unit             - (IN) BCM device number
 * stage_fc         - (IN) Pointer to device stage structure
 *
 * Returns:
 *     BCM_E_XXX
 */

int
_field_data_control_sync(int unit, _field_stage_t *stage_fc)
{
    int qual_count = 0;                    /* Number of Qualifiers */
    _field_data_qualifier_p temp = NULL;   /* Field data qualifier */
    _field_data_control_t *fd = NULL;      /* Field data control structure */
    _field_tlv_t *tlv = NULL;              /* Tlv data */
    uint8 *ptr = NULL;                     /* Scache Pointer */
    _field_control_t *fc = NULL;           /* Field control structure. */
    uint32 *position = NULL;               /* Scache Position */
    int endmarker = _FIELD_WB_EM_DATACONTROL;
                                           /* End marker for data control */

    WB_FIELD_CONTROL_GET(fc, ptr, position);
    fd = stage_fc->data_ctrl;

    TLV_CREATE(_bcmFieldInternalDataControlStart,
                                _bcmFieldInternalVariable,0, &tlv);
    TLV_WRITE(unit, tlv, ptr, position);


    TLV_CREATE(_bcmFieldInternalDataControlUsageBmp,
                                _bcmFieldInternalVariable,0, &tlv);
    tlv->value = &(fd->usage_bmap);
    TLV_WRITE(unit, tlv, ptr, position);

    temp = fd->data_qual;
    while (temp != NULL)
    {
    qual_count++ ;
    temp=temp->next;
    }


    TLV_CREATE(_bcmFieldInternalDataControlDataQualStruct,
                                _bcmFieldInternalVariable, qual_count , &tlv);
    TLV_WRITE(unit, tlv, ptr, position);

    temp = fd->data_qual;
    while (temp != NULL)
    {
    BCM_IF_ERROR_RETURN(_field_datacontroldataqualifier_sync(unit,temp));
    temp=temp->next;
    }
    BCM_IF_ERROR_RETURN(_field_datacontrolethertype_sync(unit,fd));
    BCM_IF_ERROR_RETURN(_field_datacontrolprot_sync(unit,fd));
    BCM_IF_ERROR_RETURN(_field_datacontroltcamentry_sync(unit,fd));

    TLV_CREATE(_bcmFieldInternalDataControlElemSize,
                                _bcmFieldInternalVariable,0, &tlv);
    tlv->value = &(fd->elem_size);
    TLV_WRITE(unit, tlv, ptr, position);


    TLV_CREATE(_bcmFieldInternalDataControlNumElem,
                                _bcmFieldInternalVariable,0, &tlv);
    tlv->value = &(fd->num_elems);
    TLV_WRITE(unit, tlv, ptr, position);

    TLV_CREATE(_bcmFieldInternalEndStructDataControl,
                                _bcmFieldInternalVariable, 0, &tlv);
    tlv->value = &(endmarker);
    TLV_WRITE(unit, tlv, ptr, position);

    sal_free(tlv);

    return BCM_E_NONE;

}

/* Function:
 * _field_ltinfo_sync
 *
 * Purpose:
 *    Sync _field_lt_info_t structure
 *
 * Paramaters:
 * unit             - (IN) BCM device number
 * lt_info          - (IN) Pointer to device stage structure
 *
 * Returns:
 *     BCM_E_XXX
 */
int
_field_ltinfo_sync(int unit, _field_lt_config_t *lt_info)
{
    _field_tlv_t *tlv = NULL;              /* Tlv data */
    uint8 *ptr = NULL;                     /* Scache Pointer */
    _field_control_t *fc = NULL;           /* Field control structure. */
    uint32 *position = NULL;               /* Scache Position */
    uint32 endmarkerltconf = _FIELD_WB_EM_LTCONF;
                                           /* End Marker */

    WB_FIELD_CONTROL_GET(fc, ptr, position);


    TLV_CREATE(_bcmFieldInternalLtConfigValid,
                            _bcmFieldInternalVariable,0, &tlv);
    tlv->value = &(lt_info->valid);
    TLV_WRITE(unit, tlv, ptr, position);

    TLV_CREATE(_bcmFieldInternalLtConfigLtId,
                            _bcmFieldInternalVariable,0, &tlv);
    tlv->value = &(lt_info->lt_id);
    TLV_WRITE(unit, tlv, ptr, position);

    TLV_CREATE(_bcmFieldInternalLtConfigLtPartPri,
                            _bcmFieldInternalArray,_FP_MAX_LT_PARTS, &tlv);
    tlv->value = &(lt_info->lt_part_pri);
    TLV_WRITE(unit, tlv, ptr, position);

    TLV_CREATE(_bcmFieldInternalLtConfigLtPartMap,
                            _bcmFieldInternalVariable,0, &tlv);
    tlv->value = &(lt_info->lt_part_map);
    TLV_WRITE(unit, tlv, ptr, position);

    TLV_CREATE(_bcmFieldInternalLtConfigLtActionPri,
                            _bcmFieldInternalVariable,0, &tlv);
    tlv->value = &(lt_info->lt_action_pri);
    TLV_WRITE(unit, tlv, ptr, position);

    TLV_CREATE(_bcmFieldInternalLtConfigPri,
                            _bcmFieldInternalVariable,0, &tlv);
    tlv->value = &(lt_info->priority);
    TLV_WRITE(unit, tlv, ptr, position);

    TLV_CREATE(_bcmFieldInternalLtConfigFlags,
                            _bcmFieldInternalVariable,0, &tlv);
    tlv->value = &(lt_info->flags);
    TLV_WRITE(unit, tlv, ptr, position);

    TLV_CREATE(_bcmFieldInternalLtConfigEntry,
                            _bcmFieldInternalVariable,0, &tlv);
    tlv->value = &(lt_info->lt_entry);
    TLV_WRITE(unit,tlv, ptr, position);


    TLV_CREATE(_bcmFieldInternalEndStructLtConfig,
                            _bcmFieldInternalVariable,0, &tlv);
    tlv->value = &(endmarkerltconf);
    TLV_WRITE(unit, tlv, ptr, position);

    sal_free(tlv);

    return BCM_E_NONE;
}
/* Function:
 * _field_stage_sync
 *
 * Purpose:
 *    Sync _field_stage_t structure
 *
 * Paramaters:
 * unit             - (IN) BCM device number
 * stage_fc         - (IN) Pointer to device stage structure
 *
 * Returns:
 *     BCM_E_XXX
 */

int
_field_stage_sync(int unit, _field_stage_t *stage_fc)
{
   _field_tlv_t *tlv = NULL;        /* Tlv data */
    uint8 *ptr;                     /* Scache Pointer */
    uint32 endmarkerstage  = _FIELD_WB_EM_STAGE;
                                    /* endmarker for stage */
    uint32 endmarkerrange  = _FIELD_WB_EM_RANGE;
                                    /* endmarker for range */
    uint32 endmarkermeter  = _FIELD_WB_EM_METER;
                                    /* endmarker for meter */
    uint32 endmarkercntr   = _FIELD_WB_EM_CNTR;
                                    /* endmarker for cntr */
    int  num_pipes = 0;             /* Number of pipes */
    _field_control_t *fc;           /* Field control structure. */
    uint32 *position;               /* Scache Position */
    int range_count = 0;            /* Count of ranges */
    _field_range_t *fr;             /* field Range structure */
    int type_range[7] =  {          /* Types for _field_range_t structure */
       (int)((_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)
              |_bcmFieldInternalRangeFlags),
       (int)((_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)
              |_bcmFieldInternalRangeRid),
       (int)((_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)
              |_bcmFieldInternalRangeMin),
       (int)((_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)
              |_bcmFieldInternalRangeMax),
       (int)((_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)
              |_bcmFieldInternalRangeHwIndex),
       (int)((_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)
              |_bcmFieldInternalRangeStyle),
       (int)((_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)
              |_bcmFieldInternalEndStructRanges)
     };
    int type_meter[8] = {           /* Types for _field_meterpool_t structure */
       (int)((_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)
              | _bcmFieldInternalMeterLevel),
       (int)((_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)
              | _bcmFieldInternalMeterSliceId),
       (int)((_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)
              | _bcmFieldInternalMeterSize),
       (int)((_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)
              | _bcmFieldInternalMeterPoolSize),
       (int)((_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)
              | _bcmFieldInternalMeterFreeMeters),
       (int)((_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)
              | _bcmFieldInternalMeterNumMeterPairs),
       (int)((_bcmFieldInternalArray << _FP_WB_TLV_BASIC_TYPE_SHIFT)
              | _bcmFieldInternalMeterBmp),
       (int)((_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)
              | _bcmFieldInternalEndStructMeter)
    };
    int type_cntr[5] = {            /* Types for _field_cntrpool_t structure */
       (int)((_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)
              | _bcmFieldInternalCntrSliceId),
       (int)((_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)
              | _bcmFieldInternalCntrSize),
       (int)((_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)
              | _bcmFieldInternalCntrFreeCntrs),
       (int)((_bcmFieldInternalArray << _FP_WB_TLV_BASIC_TYPE_SHIFT)
              | _bcmFieldInternalCntrBmp),
       (int)((_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)
              | _bcmFieldInternalEndStructCntr)
   };

   int type_ltconf[9] = {          /* Types for _field_ltinfo_t structure */
       (int)((_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)
              | _bcmFieldInternalLtConfigValid),
       (int)((_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)
              | _bcmFieldInternalLtConfigLtId),
       (int)((_bcmFieldInternalArray << _FP_WB_TLV_BASIC_TYPE_SHIFT)
              | _bcmFieldInternalLtConfigLtPartPri),
       (int)((_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)
              | _bcmFieldInternalLtConfigLtPartMap),
       (int)((_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)
              | _bcmFieldInternalLtConfigLtActionPri),
       (int)((_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)
              | _bcmFieldInternalLtConfigPri),
       (int)((_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)
              | _bcmFieldInternalLtConfigFlags),
       (int)((_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)
              | _bcmFieldInternalLtConfigEntry),
       (int)((_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)
              | _bcmFieldInternalEndStructLtConfig)
    };
    int  i = 0,j = 0;               /* Local loop variable */

    if (stage_fc->oper_mode == bcmFieldGroupOperModeGlobal) {
        num_pipes = 1;
    } else if (stage_fc->oper_mode == bcmFieldGroupOperModePipeLocal) {
        num_pipes = (_FP_MAX_NUM_PIPES);
    }

    WB_FIELD_CONTROL_GET(fc, ptr, position);

    TLV_CREATE(_bcmFieldInternalStage,
                            _bcmFieldInternalVariable,0, &tlv);
    TLV_WRITE(unit, tlv, ptr, position);

    TLV_CREATE(_bcmFieldInternalStageOperMode,
                                        _bcmFieldInternalVariable,0, &tlv);
    tlv->value = &(stage_fc-> oper_mode);
    TLV_WRITE(unit, tlv, ptr, position);

    TLV_CREATE(_bcmFieldInternalStageStageid,
                            _bcmFieldInternalVariable,0, &tlv);
    tlv->value = &(stage_fc-> stage_id);
    TLV_WRITE(unit, tlv, ptr, position);

    TLV_CREATE(_bcmFieldInternalStageFlags,
                            _bcmFieldInternalVariable,0, &tlv);
    tlv->value = &(stage_fc->flags);
    TLV_WRITE(unit, tlv, ptr, position);

    TLV_CREATE(_bcmFieldInternalStageTcamSz,
                            _bcmFieldInternalVariable,0, &tlv);
    tlv->value = &(stage_fc->tcam_sz);
    TLV_WRITE(unit, tlv, ptr, position);

    TLV_CREATE(_bcmFieldInternalStageTcamSlices,
                            _bcmFieldInternalVariable,0,&tlv);
    tlv->value = &(stage_fc->tcam_slices);
    TLV_WRITE(unit, tlv, ptr, position);

    TLV_CREATE(_bcmFieldInternalStageNumInstances,
                            _bcmFieldInternalVariable,0, &tlv);
    tlv->value = &(stage_fc->num_instances);
    TLV_WRITE(unit, tlv, ptr, position);

    TLV_CREATE(_bcmFieldInternalStageNumPipes,
                            _bcmFieldInternalVariable,0, &tlv);
    tlv->value = &(stage_fc->num_pipes);
    TLV_WRITE(unit, tlv, ptr, position);

    TLV_CREATE(_bcmFieldInternalStageRangeId,
                            _bcmFieldInternalVariable,0,&tlv);
    tlv->value = &(stage_fc->range_id);
    TLV_WRITE(unit, tlv, ptr, position);

    TLV_CREATE(_bcmFieldInternalStageNumMeterPools,
                            _bcmFieldInternalVariable,0, &tlv);
    tlv->value = &(stage_fc->num_meter_pools);
    TLV_WRITE(unit, tlv, ptr, position);

    TLV_CREATE(_bcmFieldInternalStageNumCntrPools,
                            _bcmFieldInternalVariable,0,&tlv);
    tlv->value = &(stage_fc->num_cntr_pools);
    TLV_WRITE(unit, tlv, ptr, position);

    TLV_CREATE(_bcmFieldInternalStageLtTcamSz,
                            _bcmFieldInternalVariable,0, &tlv);
    tlv->value = &(stage_fc-> lt_tcam_sz);
    TLV_WRITE(unit, tlv, ptr, position);

    TLV_CREATE(_bcmFieldInternalStageNumLogicalTables,
                            _bcmFieldInternalVariable,0, &tlv);
    tlv->value = &(stage_fc-> num_logical_tables);
    TLV_WRITE(unit, tlv, ptr, position);

    TLV_CREATE(_bcmFieldInternalStageExtLevels,
                            _bcmFieldInternalVariable,0, &tlv);
    tlv->value = &(stage_fc-> num_ext_levels);
    TLV_WRITE(unit, tlv, ptr, position);

    /* _field_range_t */
    fr = stage_fc->ranges;
    while (fr != NULL) {
    range_count ++;
    fr = fr ->next;
    }

    /* In the encoded length below , first ten bits is value 7 [no of types],
     * next 22 bits is range count. Here number of types indicate counts of
     * elements in the current datastructure.
     */


    TLV_CREATE(_bcmFieldInternalStageRanges,
                           _bcmFieldInternalVariable,range_count
                           | (7 << _FP_WB_TLV_TYPE_SHIFT), &tlv);
    tlv->value = &(type_range);
    TLV_WRITE(unit, tlv, ptr, position);

    fr = stage_fc->ranges;
    while (fr != NULL) {

        TLV_CREATE(_bcmFieldInternalRangeFlags,
                                _bcmFieldInternalVariable,0, &tlv);
        tlv->value = &(fr->flags);
        TLV_WRITE(unit, tlv, ptr, position);

        TLV_CREATE(_bcmFieldInternalRangeRid,
                                _bcmFieldInternalVariable,0, &tlv);
        tlv->value = &(fr->rid);
        TLV_WRITE(unit, tlv, ptr, position);

        TLV_CREATE(_bcmFieldInternalRangeMin,
                                _bcmFieldInternalVariable,0, &tlv);
        tlv->value = &(fr->min);
        TLV_WRITE(unit, tlv, ptr, position);

        TLV_CREATE(_bcmFieldInternalRangeMax,
                                _bcmFieldInternalVariable,0, &tlv);
        tlv->value = &(fr->max);
        TLV_WRITE(unit, tlv, ptr, position);

        TLV_CREATE(_bcmFieldInternalRangeHwIndex,
                                _bcmFieldInternalVariable,0, &tlv);
        tlv->value = &(fr->hw_index);
        TLV_WRITE(unit, tlv, ptr, position);

        TLV_CREATE(_bcmFieldInternalRangeStyle,
                                _bcmFieldInternalVariable,0, &tlv);
        tlv->value = &(fr->style);
        TLV_WRITE(unit, tlv, ptr, position);

        TLV_CREATE(_bcmFieldInternalEndStructRanges,
                                _bcmFieldInternalVariable, 0, &tlv);
        tlv->value = &(endmarkerrange);
        TLV_WRITE(unit, tlv, ptr, position);

        fr = fr ->next;
    }

    /* _field_meter_pool_t */

    /* In the encoded length below , first ten bits is value 8 [no of types],
     * next 12 bits is num_meter_pools and next 10 bits is num_pipes.Here number
     * of types indicate counts of elements in the current datastructure.
     */

    TLV_CREATE(_bcmFieldInternalStageMeterPool, _bcmFieldInternalVariable,
           ((num_pipes) | (stage_fc->num_meter_pools << 10)
           | (8 << _FP_WB_TLV_TYPE_SHIFT)), &tlv);
    tlv->value = &(type_meter);
    TLV_WRITE(unit, tlv, ptr, position);

    for (i = 0; i < num_pipes; i++) {
    	for (j = 0; j < stage_fc->num_meter_pools; j++) {

            TLV_CREATE(_bcmFieldInternalMeterLevel,
                                    _bcmFieldInternalVariable,0, &tlv);
            tlv->value = &(stage_fc->meter_pool[i][j]->level);
            TLV_WRITE(unit, tlv, ptr, position);

            TLV_CREATE(_bcmFieldInternalMeterSliceId,
                                    _bcmFieldInternalVariable,0, &tlv);
            tlv->value = &(stage_fc->meter_pool[i][j]->slice_id);
            TLV_WRITE(unit, tlv, ptr, position);

            TLV_CREATE(_bcmFieldInternalMeterSize,
                                    _bcmFieldInternalVariable,0, &tlv);
            tlv->value = &(stage_fc->meter_pool[i][j]->size);
            TLV_WRITE(unit, tlv, ptr, position);

            TLV_CREATE(_bcmFieldInternalMeterPoolSize,
                                    _bcmFieldInternalVariable,0, &tlv);
            tlv->value = &( stage_fc->meter_pool[i][j]->pool_size);
            TLV_WRITE(unit, tlv, ptr, position);

            TLV_CREATE(_bcmFieldInternalMeterFreeMeters,
                                    _bcmFieldInternalVariable,0, &tlv);
            tlv->value = &(stage_fc->meter_pool[i][j]->free_meters);
            TLV_WRITE(unit, tlv, ptr, position);

            TLV_CREATE(_bcmFieldInternalMeterNumMeterPairs,
                                    _bcmFieldInternalVariable,0, &tlv);
            tlv->value = &(stage_fc->meter_pool[i][j]->num_meter_pairs);
            TLV_WRITE(unit, tlv, ptr, position);

            TLV_CREATE(_bcmFieldInternalMeterBmp, _bcmFieldInternalArray,
                       ((stage_fc->meter_pool[i][j]->size -32 +1) /32), &tlv);
            tlv->value = &(stage_fc->meter_pool[i][j]->meter_bmp);
            TLV_WRITE(unit, tlv, ptr, position);

            TLV_CREATE(_bcmFieldInternalEndStructMeter,
                                    _bcmFieldInternalVariable, 0, &tlv);
            tlv->value = &(endmarkermeter);
            TLV_WRITE(unit, tlv, ptr, position);

        }
    }

    /* _field_cntr_pool_t */

    /* In the encoded length below , first ten bits is value 5 [no of types],
     * next 22 bits is num_cntr_pools. Here number of types indicate counts of
     * elements in the current datastructure.
     */

    TLV_CREATE(_bcmFieldInternalStageCntrPools,
               _bcmFieldInternalVariable,
               (stage_fc->num_cntr_pools
               | (5 << _FP_WB_TLV_TYPE_SHIFT)), &tlv);
    tlv->value = &(type_cntr);
    TLV_WRITE(unit, tlv, ptr, position);

    for (i = 0; i < stage_fc->num_cntr_pools;i++) {

    TLV_CREATE(_bcmFieldInternalCntrSliceId,
                            _bcmFieldInternalVariable,0, &tlv);
    tlv->value = &(stage_fc->cntr_pool[i]->slice_id);
    TLV_WRITE(unit, tlv, ptr, position);

    TLV_CREATE(_bcmFieldInternalCntrSize,
                            _bcmFieldInternalVariable,0, &tlv);
    tlv->value = &(stage_fc->cntr_pool[i]->size);
    TLV_WRITE(unit, tlv, ptr, position);

    TLV_CREATE(_bcmFieldInternalCntrFreeCntrs,
                            _bcmFieldInternalVariable,0, &tlv);
    tlv->value = &(stage_fc->cntr_pool[i]->free_cntrs);
    TLV_WRITE(unit, tlv, ptr, position);

    TLV_CREATE(_bcmFieldInternalCntrBmp,_bcmFieldInternalArray,
                           (((stage_fc->cntr_pool[i]->size) -32 +1) /32),
                            &tlv);
    tlv->value = &(stage_fc->cntr_pool[i]->cntr_bmp);
    TLV_WRITE(unit, tlv, ptr, position);

    TLV_CREATE(_bcmFieldInternalEndStructCntr,
                            _bcmFieldInternalVariable, 0, &tlv);
    tlv->value = &(endmarkercntr);
    TLV_WRITE(unit, tlv, ptr, position);
    }

    /* _field_lt_info_t */
    /* In the encoded length below , first ten bits is value 9 [no of types],
     * next 12 bits is no of logical tables and next 10 bits is num_pipes.
     * Here number of types indicate counts of elements
     * in the current datastructure.
     */

    TLV_CREATE(_bcmFieldInternalStageLtInfo,
                            _bcmFieldInternalVariable,
                            ((num_pipes) | (_FP_MAX_NUM_LT << 10)
                            | (9 << _FP_WB_TLV_TYPE_SHIFT)), &tlv);
    tlv->value = &(type_ltconf);
    TLV_WRITE(unit, tlv, ptr, position);

    for (i = 0; i < num_pipes;i++) {
        for (j = 0; j < _FP_MAX_NUM_LT;j++) {
            _field_ltinfo_sync(unit, stage_fc->lt_info[i][j]);
        }
    }

    /* data_control_t sync */
    BCM_IF_ERROR_RETURN(_field_data_control_sync(unit, stage_fc));


    TLV_CREATE(_bcmFieldInternalEndStructStage,
                            _bcmFieldInternalVariable, 0, &tlv);
    tlv->value = &endmarkerstage;
    TLV_WRITE(unit, tlv, ptr, position);

    sal_free(tlv);

    return BCM_E_NONE;
}


/* Function:
 * _field_slice_sync
 *
 * Purpose:
 *    Sync _field_slice_t structure
 *
 * Paramaters:
 * unit          - (IN) BCM device number
 * slice         - (IN) Pointer to device slice structure
 *
 * Returns:
 *     BCM_E_XXX
 */

int
_field_slice_sync(int unit, _field_slice_t *slice)
{
    _field_tlv_t *tlv = NULL;    /* Tlv structure */
    _field_control_t *fc = NULL; /* Field control structure */
    uint8 *ptr;                  /* Pointer to scache_pos */
    uint32 *position;            /* Scache position */
    uint32 endmarker = _FIELD_WB_EM_SLICE;
                                 /* End marker */
    WB_FIELD_CONTROL_GET(fc, ptr, position);


    TLV_CREATE(_bcmFieldInternalSliceNumber,
                                 _bcmFieldInternalVariable, 0, &tlv);
    tlv->value = &(slice->slice_number);
    TLV_WRITE(unit, tlv, ptr, position);

    TLV_CREATE(_bcmFieldInternalSliceStartTcamIdx,
                                 _bcmFieldInternalVariable, 0, &tlv);
    tlv->value = &(slice->start_tcam_idx);
    TLV_WRITE(unit, tlv, ptr, position);

    TLV_CREATE(_bcmFieldInternalSliceEntryCount,
                                 _bcmFieldInternalVariable, 0, &tlv);
    tlv->value = &(slice->entry_count);
    TLV_WRITE(unit, tlv, ptr, position);

    TLV_CREATE(_bcmFieldInternalSliceFreeCount,
                                 _bcmFieldInternalVariable, 0, &tlv);
    tlv->value = &(slice->free_count);
    TLV_WRITE(unit, tlv, ptr, position);

    TLV_CREATE(_bcmFieldInternalSliceCountersCount,
                                 _bcmFieldInternalVariable, 0, &tlv);
    tlv->value = &(slice->counters_count);
    TLV_WRITE(unit, tlv, ptr, position);

    TLV_CREATE(_bcmFieldInternalSliceMetersCount,
                                 _bcmFieldInternalVariable, 0, &tlv);
    tlv->value = &(slice->meters_count);
    TLV_WRITE(unit, tlv, ptr, position);

    TLV_CREATE(_bcmFieldInternalSliceInstalledEntriesCount,
                                 _bcmFieldInternalVariable, 0, &tlv);
    tlv->value = &(slice->hw_ent_count);
    TLV_WRITE(unit, tlv, ptr, position);

    /* IFP uses global meter pools and counter pools, so no need to
     *  save below fields
     *
     * TLV_CREATE(_bcmFieldInternalSliceCounterBmp,
     *                            _bcmFieldInternalVariable, 0, &tlv);
     *tlv->value = &(slice->counter_bmp);
     *TLV_WRITE(unit, tlv, ptr, position);

     *TLV_CREATE(_bcmFieldInternalSliceMeterBmp,
     *                           _bcmFieldInternalVariable, 0, &tlv);
     *tlv->value = &(slice->meter_bmp);
     *TLV_WRITE(unit, tlv, ptr, position);
     */
    TLV_CREATE(_bcmFieldInternalSliceStageId,
                                 _bcmFieldInternalVariable, 0, &tlv);
    tlv->value = &(slice->stage_id);
    TLV_WRITE(unit, tlv, ptr, position);

    TLV_CREATE(_bcmFieldInternalSlicePortPbmp,
                                 _bcmFieldInternalArray, 32, &tlv);
    tlv->value = &(slice->pbmp);
    TLV_WRITE(unit, tlv, ptr, position);

    if (slice->next != NULL) {
        TLV_CREATE(_bcmFieldInternalSliceNextSlice,
                                 _bcmFieldInternalVariable, 0, &tlv);
        tlv->value = &(slice->next->slice_number);
       TLV_WRITE(unit, tlv, ptr, position);
    }
    if (slice->prev != NULL) {
        TLV_CREATE(_bcmFieldInternalSlicePrevSlice,
                                 _bcmFieldInternalVariable, 0, &tlv);
        tlv->value = &(slice->prev->slice_number);
       TLV_WRITE(unit, tlv, ptr, position);
    }
    TLV_CREATE(_bcmFieldInternalSliceFlags,
                                 _bcmFieldInternalVariable, 0, &tlv);
    tlv->value = &(slice->slice_flags);
    TLV_WRITE(unit, tlv, ptr, position);


    TLV_CREATE(_bcmFieldInternalSliceGroupFlags,
                                 _bcmFieldInternalVariable, 0, &tlv);
    tlv->value = &(slice->group_flags);
    TLV_WRITE(unit, tlv, ptr, position);

    TLV_CREATE(_bcmFieldInternalSliceLtMap,
                                 _bcmFieldInternalVariable, 0, &tlv);
    tlv->value = &(slice->lt_map);
    TLV_WRITE(unit, tlv, ptr, position);

    TLV_CREATE(_bcmFieldInternalSliceLtPartitionPri,
                                       _bcmFieldInternalVariable, 0, &tlv);
    tlv->value = &(slice->lt_partition_pri);
    TLV_WRITE(unit, tlv, ptr, position);

    BCM_IF_ERROR_RETURN(_field_extractor_sync(unit, slice->ext_sel,
                                                    _FP_MAX_NUM_LT));

    TLV_CREATE(_bcmFieldInternalEndStructSlice,
                                 _bcmFieldInternalVariable, 0, &tlv);
    tlv->value = &endmarker;
    TLV_WRITE(unit, tlv, ptr, position);

    sal_free(tlv);
    return BCM_E_NONE;
}


/* Function:
 * _field_lt_slice_sync
 *
 * Purpose:
 *    Sync _field_lt_slice_t structure
 *
 * Paramaters:
 * unit          - (IN) BCM device number
 * lt_slice      - (IN) Pointer to device lt_slice structure
 *
 * Returns:
 *     BCM_E_XXX
 */

int
_field_lt_slice_sync(int unit, _field_lt_slice_t *lt_slice)
{

    _field_tlv_t *tlv = NULL;    /* Tlv structure */
    _field_control_t *fc = NULL; /* field control structure */
    uint8 *ptr;                  /* pointer to scache_pos */
    uint32 *position;            /* Scache position */
    uint32 endmarker = _FIELD_WB_EM_LTSLICE;
                                 /* End marker */

    WB_FIELD_CONTROL_GET(fc, ptr, position);

    TLV_CREATE(_bcmFieldInternalLtSliceSliceNum,
                                 _bcmFieldInternalVariable, 0, &tlv);
    tlv->value = &(lt_slice->slice_number);
    TLV_WRITE(unit, tlv, ptr, position);

    TLV_CREATE(_bcmFieldInternalLtSliceStartTcamIdx,
                                 _bcmFieldInternalVariable, 0, &tlv);
    tlv->value = &(lt_slice->start_tcam_idx);
    TLV_WRITE(unit, tlv, ptr, position);

    TLV_CREATE(_bcmFieldInternalLtSliceEntryCount,
                                 _bcmFieldInternalVariable, 0, &tlv);
    tlv->value = &(lt_slice->entry_count);
    TLV_WRITE(unit, tlv, ptr, position);

    TLV_CREATE(_bcmFieldInternalLtSliceFreeCount,
                                 _bcmFieldInternalVariable, 0, &tlv);
    tlv->value = &(lt_slice->free_count);
    TLV_WRITE(unit, tlv, ptr, position);

    TLV_CREATE(_bcmFieldInternalLtSliceStageid,
                                 _bcmFieldInternalVariable, 0, &tlv);
    tlv->value = &(lt_slice->stage_id);
    TLV_WRITE(unit, tlv, ptr, position);

    if (lt_slice->next != NULL) {
        TLV_CREATE(_bcmFieldInternalLtSliceNextSlice,
                                 _bcmFieldInternalVariable, 0, &tlv);
        tlv->value = &(lt_slice->next->slice_number);
        TLV_WRITE(unit, tlv, ptr, position);
    }
    if (lt_slice->prev != NULL) {
        TLV_CREATE(_bcmFieldInternalLtSlicePrevSlice,
                                 _bcmFieldInternalVariable, 0, &tlv);
        tlv->value = &(lt_slice->prev->slice_number);
        TLV_WRITE(unit, tlv, ptr, position);
    }

    TLV_CREATE(_bcmFieldInternalLtSliceFlags,
                                 _bcmFieldInternalVariable, 0, &tlv);
    tlv->value = &(lt_slice->slice_flags);
    TLV_WRITE(unit, tlv, ptr, position);

    TLV_CREATE(_bcmFieldInternalLtSliceGroupFlags,
                                 _bcmFieldInternalVariable, 0, &tlv);
    tlv->value = &(lt_slice->group_flags);
    TLV_WRITE(unit, tlv, ptr, position);

    TLV_CREATE(_bcmFieldInternalEndStructSlice,
                                 _bcmFieldInternalVariable, 0, &tlv);
    tlv->value = &endmarker;
    TLV_WRITE(unit, tlv, ptr, position);

    sal_free(tlv);
    return BCM_E_NONE;

}

/* Function:
 * _field_actions_sync
 *
 * Purpose:
 *    Sync _field_action_t structure
 *    Mirror actions need special treatment as they are the only actions which
 *    can be added multiple times.
 *
 * Paramaters:
 * unit              - (IN) BCM device number
 * _field_control_t  - (IN) _field_control_t structure pointer
 * _field_action_t   - (IN) Pointer to action structure
 *
 * Returns:
 *     BCM_E_XXX
 */
int
_field_actions_sync(int unit, _field_action_t *act)
{

    _field_tlv_t *tlv = NULL;            /* pointer to tlv structure */
    _field_control_t *fc = NULL;         /* Pointer to field control structure*/
    uint8 *ptr;                          /* Pointer to scache_pos (offset) in
                                          * field control structure
                                          */
    uint32 *position;                    /* Scache position */
    _field_action_bmp_t action_bmp;      /* bit map of actions used for entry*/
    _field_action_t *f_act = NULL;       /* pointer to action structure */
    int size, param_id, action_id, mirror_id;
                                         /* loop variables */
    int hw_count = 0, count = 0;         /* count of params and hw index */
    int val_array[100] = { -1 };         /* Array to hold values of
                                          * params for some actions.Count is 100
                                          * since we cannot predict what
                                          * type of actions will be added
                                          */
    int hw_idx_array[100] = { -1 };      /* Array to hold hw_index in actions */
    int mirror_params_array[(BCM_MIRROR_MTP_COUNT * 2)] = { -1 };
                                         /* params of mirror actions */
    int mirror_hw_idx_array[BCM_MIRROR_MTP_COUNT] = { -1 };
                                         /* hw_index for mirror actions */
    _field_action_t *fa = NULL;          /* Pointer to actions structure */

    f_act = act;
    WB_FIELD_CONTROL_GET(fc, ptr, position);
    action_bmp.w = NULL;
    _FP_XGS3_ALLOC(action_bmp.w, SHR_BITALLOCSIZE(bcmFieldActionCount),
                    "Action BMP");

    while (act != NULL) {
        _FP_ACTION_BMP_ADD(action_bmp, act->action);
        act = act->next;
    }
    for (action_id = 0; action_id < bcmFieldActionCount; action_id++) {
        if (!_FP_ACTION_BMP_TEST(action_bmp, action_id)) {
                continue;
        }
        act = f_act;
        while (act->action != action_id) {
            act = act->next;
        }
        switch (act->action) {
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
            case bcmFieldActionEgressClassSelect:
            case bcmFieldActionHiGigClassSelect:
            case bcmFieldActionFabricQueue:
            case bcmFieldActionL3Switch:
            case bcmFieldActionRedirectMcast:
            case bcmFieldActionRedirectEgrNextHop:
            case bcmFieldActionRedirectIpmc:
            case bcmFieldActionRedirectCancel:
            case bcmFieldActionOuterVlanNew:
                 for (param_id = 0; param_id < 1; param_id++) {
                     val_array[count++]= act->param[param_id];
                 }
                 break;
            case bcmFieldActionDstMacNew:
            case bcmFieldActionSrcMacNew:
            case bcmFieldActionUnmodifiedPacketRedirectPort:
            case bcmFieldActionRedirect:
            case bcmFieldActionEtagNew:
            case bcmFieldActionVnTagNew:
            case bcmFieldActionRedirectTrunk:
                 for (param_id = 0; param_id < 2; param_id++) {
                     val_array[count++]= act->param[param_id];
                 }
                 break;
            case bcmFieldActionRedirectPbmp:
            case bcmFieldActionEgressMask:
            case bcmFieldActionEgressPortsAdd:
            case bcmFieldActionRedirectBcastPbmp:
                 for (param_id = 0; param_id < 4; param_id++) {
                      val_array[count++]= act->param[param_id];
                 }
                 break;
            case bcmFieldActionMirrorIngress:
            case bcmFieldActionMirrorEgress:
                 fa = act;
                 for (mirror_id = 0; mirror_id < (BCM_MIRROR_MTP_COUNT * 2);
                      mirror_id++) {
                      mirror_params_array[mirror_id] = -1;
                 }
                 mirror_id = 0;
                 /* For all remaining mirror actions add parameters now */
                 while (fa != NULL) {
                        if (fa ->action == action_id) {
                            for (param_id = 0; param_id < 2; param_id++) {
                                 mirror_params_array[mirror_id++] =
                                 fa->param[param_id];
                            }
                        }
                        fa = fa->next;
                 }
                 /* Mirror will have BCM_MIRROR_MTP_COUNT * 2 parameters */
                 for (mirror_id = 0;mirror_id < (BCM_MIRROR_MTP_COUNT * 2);
                      mirror_id++) {
                      val_array[count++] = mirror_params_array[mirror_id];
                 }
                 break;
            case bcmFieldActionRedirectVlan:
            case bcmFieldActionVnTagDelete:
            case bcmFieldActionEtagDelete:
            default:
                   break;
        }
        switch (act->action) {
            case bcmFieldActionDstMacNew:
            case bcmFieldActionSrcMacNew:
            case bcmFieldActionOuterVlanNew:
            case bcmFieldActionVnTagNew:
            case bcmFieldActionVnTagDelete:
            case bcmFieldActionEtagNew:
            case bcmFieldActionEtagDelete:
            case bcmFieldActionRedirectPbmp:
            case bcmFieldActionEgressMask:
            case bcmFieldActionEgressPortsAdd:
            case bcmFieldActionRedirectIpmc:
            case bcmFieldActionRedirectMcast:
            case bcmFieldActionRedirectBcastPbmp:
            case bcmFieldActionRedirectEgrNextHop:
            case bcmFieldActionL3Switch:
                 hw_idx_array[hw_count++] = act->hw_index;
                 break;
            case bcmFieldActionMirrorIngress:
            case bcmFieldActionMirrorEgress:
                 fa = act;
                 for (mirror_id = 0; mirror_id < BCM_MIRROR_MTP_COUNT;
                      mirror_id++) {
                      mirror_hw_idx_array[mirror_id]= -1;
                 }
                 mirror_id = 0;
                 /* For all remaining mirror actions add hw_idx now */
                 while (fa != NULL) {
                        if (fa ->action == action_id) {
                            mirror_hw_idx_array[mirror_id++]= fa->hw_index;
                        }
                        fa = fa->next;
                 }
                 /* Mirror will have BCM_MIRROR_MTP_COUNT hw indices */
                 for (mirror_id = 0; mirror_id < BCM_MIRROR_MTP_COUNT;
                     mirror_id++) {
                     hw_idx_array[hw_count++] = mirror_hw_idx_array[mirror_id];
                 }

            default:
                break;
        }


    }
    size = SHR_BITALLOCSIZE(bcmFieldActionCount);
    size = (size-1)/static_type_map[_bcmFieldInternalEntryActionsPbmp].size + 1;
    TLV_CREATE(_bcmFieldInternalEntryActionsPbmp, _bcmFieldInternalArray,
                size, &tlv);
    tlv->value = action_bmp.w;
    TLV_WRITE(unit, tlv, ptr, position);

    if (val_array[0] != -1) {
        TLV_CREATE(_bcmFieldInternalActionParam, _bcmFieldInternalArray,
                    count, &tlv);
        tlv->value = &val_array[0];
        TLV_WRITE(unit, tlv, ptr, position);
    }
    if (hw_idx_array[0] != -1) {
        TLV_CREATE(_bcmFieldInternalActionHwIdx, _bcmFieldInternalArray,
                                hw_count, &tlv);
        tlv->value = &hw_idx_array[0];
        TLV_WRITE(unit, tlv, ptr, position);
    }

    if (action_bmp.w !=NULL) {
        _FP_ACTION_BMP_FREE(action_bmp);
    }

    sal_free(tlv);
    return BCM_E_NONE;
}

/* Function:
 * _field_statistic_sync
 *
 * Purpose:
 *    sync _field_entry_stat_t structure
 *
 * Paramaters:
 * unit              - (IN) BCM device number
 * fc                - (IN) _field_control_t structure pointer
 * stat              - (IN) Pointer to _field_entry_stat_t structure.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
_field_statistic_sync(int unit, _field_control_t *fc,
                         _field_entry_stat_t stat) {

    _field_tlv_t *tlv = NULL; /* pointer to tlv structure */
    uint8 *ptr;               /* pointer to scache_pos (offset) */
    uint32 *position;         /* Scache position */

    WB_FIELD_CONTROL_GET(fc, ptr, position);

    TLV_CREATE(_bcmFieldInternalEntryStatSid,
               _bcmFieldInternalVariable, 0, &tlv);
    tlv->value = &(stat.sid);
    TLV_WRITE(unit, tlv, ptr, position);

    TLV_CREATE(_bcmFieldInternalEntryStatExtendedSid,
                                          _bcmFieldInternalVariable, 0, &tlv);
    tlv->value = &(stat.extended_sid);
    TLV_WRITE(unit, tlv, ptr, position);

    TLV_CREATE(_bcmFieldInternalEntryStatFlags,
                                           _bcmFieldInternalVariable, 0, &tlv);
    tlv->value = &(stat.flags);
    TLV_WRITE(unit, tlv, ptr, position);

    TLV_CREATE(_bcmFieldInternalEntryStatAction,
                                         _bcmFieldInternalVariable, 0, &tlv);
    tlv->value = &(stat.stat_action);
    TLV_WRITE(unit, tlv, ptr, position);

    sal_free(tlv);
    return BCM_E_NONE;

}
/* Function:
 * _field_lt_entry_sync
 *
 * Purpose:
 *    Sync _field_lt_entry_t structure
 *
 * Paramaters:
 * unit          - (IN) BCM device number
 * f_lt_ent      - (IN) _field_lt_entry_t structure.
 * parts_count   - (IN) Number of TCAM Parts.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
_field_lt_entry_info_sync(int unit, _field_lt_entry_t *f_lt_ent,
                          int parts_count)
{
    _field_tlv_t *tlv = NULL;        /* tlv data */
    int j = 0;                       /* j -> loop variable */
    uint32 end_marker = _FIELD_WB_EM_LTENTRY;
                                     /* end marker for the LT entry structure */
    uint8 *ptr;                      /* pointer to scache_pos(offset)      */
    _field_control_t *fc = NULL;     /* pointer to field_control structure */
    int temp;                        /* temporary variable */
    uint32 *position;                /* Scache position */

    int  flag_enums[3] = {  _bcmFieldInternalEntryFlagsPart1,
                            _bcmFieldInternalEntryFlagsPart2,
                            _bcmFieldInternalEntryFlagsPart3
                         };          /* If there are multiple parts in an
                                      * lt_entry, only flags differ
                                      * for each part. Save complete info about                                       * part1, and only flags info for
                                      * the remaining parts using FlagsPart2 and                                      * so on.
                                      */


    WB_FIELD_CONTROL_GET(fc, ptr, position);

    for (j = 0; j < parts_count; j++) {
        TLV_CREATE(flag_enums[j], _bcmFieldInternalVariable, 0, &tlv);
        tlv->value = &((f_lt_ent+j)->flags);
        TLV_WRITE(unit, tlv, ptr, position);
    }

    TLV_CREATE(_bcmFieldInternalEntryEid,
                                  _bcmFieldInternalVariable, 0, &tlv);
    tlv->value = &(f_lt_ent->eid);
    TLV_WRITE(unit, tlv, ptr, position);

    TLV_CREATE(_bcmFieldInternalEntrySliceId,
                                  _bcmFieldInternalVariable, 0, &tlv);
    tlv->value = &(f_lt_ent->index);
    TLV_WRITE(unit, tlv, ptr,position);

    TLV_CREATE( _bcmFieldInternalEntryPrio,
                                   _bcmFieldInternalVariable, 0, &tlv);
    tlv->value = &(f_lt_ent->prio);
    TLV_WRITE(unit, tlv, ptr, position);


    TLV_CREATE(_bcmFieldInternalEntryGroup,
                                  _bcmFieldInternalVariable, 0, &tlv);
    tlv->value = &(f_lt_ent->group->gid);
    TLV_WRITE(unit, tlv, ptr, position);


    TLV_CREATE(_bcmFieldInternalEntrySlice,
                                  _bcmFieldInternalVariable, 0, &tlv);
    temp  = ((int) (f_lt_ent->lt_fs->slice_number));
    tlv->value = &temp;
    TLV_WRITE(unit, tlv, ptr, position);

    /* Currently LT actions support only one action */
    if (f_lt_ent->actions != NULL) {
        TLV_CREATE(_bcmFieldInternalGroupClassAct,
                                            _bcmFieldInternalVariable, 0, &tlv);
        tlv->value = &(f_lt_ent->actions->param[0]);
        TLV_WRITE(unit, tlv, ptr, position);
    }

    TLV_CREATE(_bcmFieldInternalEndStructEntry,
                                  _bcmFieldInternalVariable, 0, &tlv);
    tlv->value = &end_marker;
    TLV_WRITE(unit, tlv, ptr, position);

    sal_free(tlv);
    return BCM_E_NONE;

}

/* Function:
 * _field_entry_info_sync
 *
 * Purpose:
 *    Sync _field_entry_t structure
 *
 * Paramaters:
 * unit          - (IN) BCM device number
 * f_ent         - (IN) _field_entry_t structure.
 * fc            - (IN) _field_control_t structure.
 * parts_count   - (IN) Number of TCAM Parts.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
_field_entry_info_sync(int unit, _field_entry_t *f_ent, int parts_count)
{
    _field_tlv_t *tlv = NULL;     /* tlv structure  */
    uint32 end_marker = _FIELD_WB_EM_ENTRY;
                                  /* end marker */
    uint8 *ptr;                   /* pointer to scache_structure */
    _field_control_t *fc = NULL;  /* pointer to field_control_t structure */
    int j = 0;                    /* j - loop variable */
    int temp;                     /* temporary variable */
     int  flag_enums[3] = { _bcmFieldInternalEntryFlagsPart1,
                            _bcmFieldInternalEntryFlagsPart2,
                            _bcmFieldInternalEntryFlagsPart3
                         };
                                 /* If there are multiple parts in an entry,
                                  * only flags differ.
                                  * save complete info about part1,
                                  * and only flags info for the
                                  * remaining parts using FlagsPart2
                                  * and soon.
                                  */
    int type_entrypolicer[3] = {
    (int) (_bcmFieldInternalEntryPolicerPid
           | (_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)),
    (int) (_bcmFieldInternalEntryPolicerFlags
           | (_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)),
    (int) (_bcmFieldInternalEndStructEntPolicer
           | (_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT))
                                };

                                 /* Holds types for all fields
                                  * in _field_entry_policer_t structure
                                  */
    uint32 *position;            /* Scache position */

    WB_FIELD_CONTROL_GET(fc, ptr, position);

    TLV_CREATE(_bcmFieldInternalEntryEid,
                                  _bcmFieldInternalVariable, 0, &tlv);
    tlv->value = &(f_ent->eid);
    TLV_WRITE(unit, tlv, ptr, position);

    TLV_CREATE( _bcmFieldInternalEntryPrio,
                                   _bcmFieldInternalVariable, 0, &tlv);
    tlv->value = &(f_ent->prio);
    TLV_WRITE(unit, tlv, ptr, position);


    TLV_CREATE(_bcmFieldInternalEntrySliceId,
                                  _bcmFieldInternalVariable, 0, &tlv);
    tlv->value = &(f_ent->slice_idx);
    TLV_WRITE(unit, tlv, ptr,position);

    for (j = 0; j < parts_count; j++) {
        TLV_CREATE(flag_enums[j], _bcmFieldInternalVariable, 0, &tlv);
        tlv->value = &((f_ent+j)->flags);
        TLV_WRITE(unit, tlv, ptr, position);
    }

    TLV_CREATE(_bcmFieldInternalEntryPbmpData,
                                  _bcmFieldInternalVariable, 0, &tlv);
    tlv->value = &(f_ent->pbmp.data);
    TLV_WRITE(unit,tlv, ptr, position);

    TLV_CREATE(_bcmFieldInternalEntryPbmpMask,
                                  _bcmFieldInternalVariable, 0, &tlv);
    tlv->value = &(f_ent->pbmp.mask);
    TLV_WRITE(unit,tlv, ptr, position);


    TLV_CREATE(_bcmFieldInternalEntryGroup,
                                  _bcmFieldInternalVariable, 0, &tlv);
    tlv->value = &(f_ent->group->gid);
    TLV_WRITE(unit, tlv, ptr, position);


    TLV_CREATE(_bcmFieldInternalEntrySlice,
                                  _bcmFieldInternalVariable, 0, &tlv);
    temp = ((int) (f_ent->fs->slice_number));
    tlv->value = &temp;
    TLV_WRITE(unit, tlv, ptr, position);

    if (f_ent->actions != NULL) {
        BCM_IF_ERROR_RETURN(_field_actions_sync(unit, f_ent->actions));
    }
    _field_statistic_sync(unit, fc, (f_ent->statistic));

    /* In the encoded length below , first ten bits is value 3 [no of types],
     * next 22 bits is policer level count. Here number of types indicate counts
     * of elements in the current datastructure.
     */

    TLV_CREATE(_bcmFieldInternalEntryPolicer,
               _bcmFieldInternalVariable, (_FP_POLICER_LEVEL_COUNT
               | (3 << _FP_WB_TLV_TYPE_SHIFT)), &tlv);

    tlv->value = &type_entrypolicer[0];
    TLV_WRITE(unit, tlv, ptr, position);

    for (j = 0; j < _FP_POLICER_LEVEL_COUNT; j++) {
        TLV_CREATE(_bcmFieldInternalEntryPolicerPid,
               _bcmFieldInternalVariable, 0, &tlv);
        tlv->value = &(f_ent->policer[j].pid);
        TLV_WRITE(unit, tlv, ptr, position);

        TLV_CREATE(_bcmFieldInternalEntryPolicerFlags,
                               _bcmFieldInternalVariable, 0, &tlv);
        tlv->value = &(f_ent->policer[j].flags);
        TLV_WRITE(unit, tlv, ptr, position);

        TLV_CREATE(_bcmFieldInternalEndStructEntPolicer,
                                     _bcmFieldInternalVariable, 0, &tlv);
        TLV_WRITE(unit, tlv, ptr, position);
    }

    TLV_CREATE(_bcmFieldInternalGlobalEntryPolicerPid,
                                          _bcmFieldInternalVariable, 0, &tlv);
    tlv->value = &(f_ent->global_meter_policer.pid);
    TLV_WRITE(unit, tlv, ptr, position);

    TLV_CREATE(_bcmFieldInternalGlobalEntryPolicerFlags,
                                          _bcmFieldInternalVariable, 0, &tlv);
    tlv->value = &(f_ent->global_meter_policer.flags);
    TLV_WRITE(unit, tlv, ptr, position);

    TLV_CREATE(_bcmFieldInternalEntryIngMtp,
                                  _bcmFieldInternalVariable, 0, &tlv);
    tlv->value = &(f_ent->ing_mtp_slot_map);
    TLV_WRITE(unit, tlv, ptr, position);

    TLV_CREATE(_bcmFieldInternalEntryEgrMtp,
                                  _bcmFieldInternalVariable, 0, &tlv);
    tlv->value = &(f_ent->egr_mtp_slot_map);
    TLV_WRITE(unit, tlv, ptr, position);

    TLV_CREATE(_bcmFieldInternalEntryDvp,
                                  _bcmFieldInternalVariable, 0, &tlv);
    tlv->value = &(f_ent->dvp_type);
    TLV_WRITE(unit, tlv, ptr, position);

    /* Disabling saveing entry copy for now to reduce scache_size
     *  if (f_ent->ent_copy != NULL) {
     */
    if (0) {
        TLV_CREATE(_bcmFieldInternalEntryCopy,
                                         _bcmFieldInternalVariable, 0, &tlv);
        TLV_WRITE(unit, tlv, ptr, position);

        _field_entry_info_sync(unit, f_ent->ent_copy, parts_count);
    }
    TLV_CREATE(_bcmFieldInternalEndStructEntry,
                                  _bcmFieldInternalVariable, 0, &tlv);
    tlv->value = &end_marker;
    TLV_WRITE(unit, tlv, ptr, position);

    sal_free(tlv);
    return BCM_E_NONE;

}
/* Function:
 * _field_entry_sync
 *
 * Purpose:
 *    call functions for _field_entry_sync and _field_lt_entry_sync.
 *
 * Paramaters:
 * unit          - (IN) BCM device number
 *
 * Returns:
 *     BCM_E_XXX
 */
int
_field_entry_sync(int unit)
{
    _field_tlv_t *tlv = NULL;        /* Pointer to tlv structure */
    _field_control_t *fc = NULL;     /* Pointer to field control structure. */
    _field_group_t *fg = NULL;       /* Pointer to traverse over groups */
    _field_entry_t *f_ent = NULL;    /* to traverse entries in each group */
    _field_lt_entry_t *f_lt_ent = NULL; /*to traverse over lt_entries */
    int parts_count = 0;             /* Number of parts for that group,
                                       interslice double wide can have two and
                                       interslice triple wide can have three */
    int  i = 0;                      /* local variable */
    uint8 *ptr;                      /* pointer to scache position(offset)  */
    uint32 *position;                /* Scache position */

    WB_FIELD_CONTROL_GET(fc, ptr, position);

    fg = fc->groups;
    while (fg != NULL) {
        if (fg->stage_id != _BCM_FIELD_STAGE_INGRESS) {
            fg = fg->next;
            continue;
        }
        BCM_IF_ERROR_RETURN(_bcm_field_entry_tcam_parts_count (unit,
                    fg->stage_id, fg->flags, &parts_count));

        TLV_CREATE(_bcmFieldInternalGroupPartCount,
                                      _bcmFieldInternalVariable, 0, &tlv);
        tlv->value = &parts_count;
        TLV_WRITE(unit, tlv, ptr, position);

        /* _field_entry_t */
        TLV_CREATE(_bcmFieldInternalGroupEntry,
                                      _bcmFieldInternalVariable, 0, &tlv);
        tlv->value = &(fg->group_status.entry_count);
        TLV_WRITE(unit, tlv, ptr, position);

        for (i = 0; i < fg->group_status.entry_count; i++) {
            if (fg->entry_arr[i] == NULL) {
                continue;
            }
            f_ent = fg->entry_arr[i];
            _field_entry_info_sync(unit, f_ent, parts_count);
        }

        /* _field_ltentry_t */
        TLV_CREATE(_bcmFieldInternalGroupLtEntry,
                                         _bcmFieldInternalVariable, 0, &tlv);
        tlv->value = &(fg->lt_grp_status.entry_count);
        TLV_WRITE(unit, tlv, ptr, position);


        for (i = 0; i < (fg->lt_grp_status.entry_count); i++) {
            if (fg->lt_entry_arr[i] == NULL) {
                continue;
            }
            f_lt_ent = fg->lt_entry_arr[i];
            _field_lt_entry_info_sync(unit, f_lt_ent, parts_count);
        }

        fg = fg->next;
    }

    TLV_CREATE(_bcmFieldInternalEndStructEntryDetails,
                            _bcmFieldInternalVariable, 0, &tlv);
    TLV_WRITE(unit, tlv, ptr, position);

    sal_free(tlv);
    return BCM_E_NONE;

}


/* Function:
 * _field_group_qualifier_sync
 *
 * Purpose:
 *    Sync _bcm_field_group_qual_t structure
 *
 * Paramaters:
 * unit          - (IN) BCM device number
 * qual_arr      - (IN) Pointer to field group qual structure
 *
 * Returns:
 *     BCM_E_XXX
 */

int
_field_group_qualifier_sync(int unit,
                            _bcm_field_group_qual_t *qual_arr)
{
    _field_tlv_t *tlv = NULL;       /* TLV data */
    uint8 *scache_ptr = NULL;       /* Pointer to Scache */
    _bcm_field_qual_offset_t *offset = NULL;
                                    /* Pointer to Qualifier offset */
    _field_control_t *fc;           /* Field control structure. */
    int  i =0;                      /* Loop local variable */
    uint32 endmarker = _FIELD_WB_EM_QUAL;
                                    /* End marker */
    uint32 endmarkeroffset = _FIELD_WB_EM_QUALOFFSET;
                                    /* End marker */
    uint32 *position;               /* pointer to scache position */
    int type_qual[8] =              /* types in _bcm_field_qual_offset_t */
    {
      (int)(_bcmFieldInternalQualOffsetField
            | (_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)),
     (int)(_bcmFieldInternalQualOffsetNumOffset
            | (_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)),
     (int)(_bcmFieldInternalQualOffsetOffsetArr
            | (_bcmFieldInternalArray << _FP_WB_TLV_BASIC_TYPE_SHIFT)),
     (int)(_bcmFieldInternalQualOffsetWidth
            | (_bcmFieldInternalArray << _FP_WB_TLV_BASIC_TYPE_SHIFT)),
     (int)(_bcmFieldInternalQualOffsetSec
            | (_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)),
     (int)(_bcmFieldInternalQualOffsetBitPos
            | (_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)),
     (int)(_bcmFieldInternalQualOffsetQualWidth
            | (_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT)),
     (int)(_bcmFieldInternalEndStructQualOffset
            | (_bcmFieldInternalVariable << _FP_WB_TLV_BASIC_TYPE_SHIFT))

    };

    WB_FIELD_CONTROL_GET(fc, scache_ptr, position);

    TLV_CREATE(_bcmFieldInternalQualQid, _bcmFieldInternalArray,
                qual_arr->size, &tlv);
    tlv->value = qual_arr->qid_arr;
    TLV_WRITE(unit, tlv, scache_ptr, position);

    TLV_CREATE(_bcmFieldInternalQualSize, _bcmFieldInternalVariable, 0, &tlv);
    tlv->value = &(qual_arr->size);
    TLV_WRITE(unit, tlv, scache_ptr, position);

    /* In the encoded length below , first ten bits is value 8 [no of types],
     * next 22 bits is qualifier array size . Here number of types indicate
     * counts of elements in the current datastructure.
     */

    TLV_CREATE(_bcmFieldInternalQualOffset, _bcmFieldInternalArray,
                    (qual_arr->size | (8 << _FP_WB_TLV_TYPE_SHIFT)), &tlv);
    tlv->value = &type_qual;
    TLV_WRITE(unit, tlv, scache_ptr, position);

    offset = qual_arr->offset_arr;
    for (i = 0; i < qual_arr->size; i++) {
        TLV_CREATE(_bcmFieldInternalQualOffsetField,
                   _bcmFieldInternalVariable, 0, &tlv);
        tlv->value = &offset->field;
        TLV_WRITE(unit, tlv, scache_ptr, position);

        TLV_CREATE(_bcmFieldInternalQualOffsetNumOffset,
                   _bcmFieldInternalVariable,0, &tlv);
        tlv->value = &offset->num_offsets;
        TLV_WRITE(unit, tlv, scache_ptr, position);

        TLV_CREATE(_bcmFieldInternalQualOffsetOffsetArr,
                    _bcmFieldInternalArray, _BCM_FIELD_QUAL_OFFSET_MAX, &tlv);
        tlv->value = &offset->offset;
        TLV_WRITE(unit, tlv, scache_ptr, position);

        TLV_CREATE(_bcmFieldInternalQualOffsetWidth,
                   _bcmFieldInternalArray, _BCM_FIELD_QUAL_OFFSET_MAX, &tlv);
        tlv->value = &offset->width;
        TLV_WRITE(unit, tlv, scache_ptr, position);

        TLV_CREATE(_bcmFieldInternalQualOffsetSec,
                   _bcmFieldInternalVariable, 0, &tlv);
        tlv->value = &offset->secondary;
        TLV_WRITE(unit, tlv, scache_ptr, position);

        TLV_CREATE(_bcmFieldInternalQualOffsetBitPos,
                   _bcmFieldInternalVariable, 0, &tlv);
        tlv->value = &offset->bit_pos;
        TLV_WRITE(unit, tlv, scache_ptr, position);

        TLV_CREATE(_bcmFieldInternalQualOffsetQualWidth,
                   _bcmFieldInternalVariable, 0, &tlv);
        tlv->value = &offset->qual_width;
        TLV_WRITE(unit, tlv, scache_ptr, position);


        TLV_CREATE(_bcmFieldInternalEndStructQualOffset,
                   _bcmFieldInternalVariable, 0, &tlv);
        tlv->value = &endmarkeroffset;
        TLV_WRITE(unit, tlv, scache_ptr, position);

        offset++;
    }

    TLV_CREATE(_bcmFieldInternalEndStructGroupQual,
               _bcmFieldInternalVariable, 0, &tlv);
    tlv->value = &endmarker;
    TLV_WRITE(unit, tlv, scache_ptr, position);

    return BCM_E_NONE;
}


/* Function:
 * _field_group_sync
 *
 * Purpose:
 *    Sync _field_group_t structure
 *
 * Paramaters:
 * unit          - (IN) BCM device number
 * fg         - (IN) Pointer to device group structure
 *
 * Returns:
 *     BCM_E_XXX
 */
int
_field_group_sync(int unit, _field_group_t *fg)
{
    _field_tlv_t *tlv = NULL;       /* TLV data */
    uint8 *scache_ptr = NULL;       /* Pointer to Scache */
    _field_control_t *fc;           /* Field control structure. */
    uint32 endmarker = _FIELD_WB_EM_GROUP; /* End marker */
    int parts_count = 0;            /* Number of entry parts. */
    int rv = 0;                     /* Return Variable */
    int partidx = 0;                /* Id to field entry part */
    int temp = 0;                   /* temporary variable */
    uint32 *position;               /* Pointer to scache pos */

    WB_FIELD_CONTROL_GET(fc, scache_ptr, position);

    TLV_CREATE(_bcmFieldInternalGroupId,
               _bcmFieldInternalVariable, 0, &tlv);
    tlv->value = &fg->gid;
    TLV_WRITE(unit, tlv, scache_ptr, position);

    TLV_CREATE(_bcmFieldInternalGroupPri,
               _bcmFieldInternalVariable, 0, &tlv);
    tlv->value = &fg->priority;
    TLV_WRITE(unit, tlv, scache_ptr, position);


    TLV_CREATE(_bcmFieldInternalQsetW, _bcmFieldInternalArray,
               _SHR_BITDCLSIZE(BCM_FIELD_QUALIFY_MAX), &tlv);
    tlv->value = &fg->qset.w;
    TLV_WRITE(unit, tlv, scache_ptr, position);

    TLV_CREATE(_bcmFieldInternalQsetUdfMap, _bcmFieldInternalArray,
               _SHR_BITDCLSIZE(BCM_FIELD_USER_NUM_UDFS), &tlv);
    tlv->value = &fg->qset.udf_map;
    TLV_WRITE(unit, tlv, scache_ptr, position);

    TLV_CREATE(_bcmFieldInternalGroupInstance,_bcmFieldInternalVariable,0,&tlv);
    tlv->value = &fg->instance;
    TLV_WRITE(unit, tlv, scache_ptr, position);

    TLV_CREATE(_bcmFieldInternalGroupSlice, _bcmFieldInternalVariable,0, &tlv);
    temp = (int) fg->slices->slice_number;
    tlv->value = &temp;
    TLV_WRITE(unit, tlv, scache_ptr, position);

    TLV_CREATE(_bcmFieldInternalGroupPbmp, _bcmFieldInternalArray,64, &tlv);
    tlv->value = &fg->pbmp;
    TLV_WRITE(unit, tlv, scache_ptr, position);

    /* Get number of entry parts we have to read. */
    rv = _bcm_field_entry_tcam_parts_count (unit, fg->stage_id,
                                            fg->flags, &parts_count);

    TLV_CREATE(_bcmFieldInternalGroupQual,
               _bcmFieldInternalArray, parts_count, &tlv);
    TLV_WRITE(unit, tlv, scache_ptr, position);

    for (partidx = 0; partidx < parts_count; partidx++) {
        BCM_IF_ERROR_RETURN( _field_group_qualifier_sync(unit,
                     &(fg->qual_arr[_FP_ENTRY_TYPE_DEFAULT][partidx])));
    }

    TLV_CREATE(_bcmFieldInternalGroupStage, _bcmFieldInternalVariable, 0, &tlv);
    tlv->value = &fg->stage_id;
    TLV_WRITE(unit, tlv, scache_ptr, position);

    TLV_CREATE(_bcmFieldInternalGroupBlockCount,
               _bcmFieldInternalVariable, 0, &tlv);
    tlv->value = &fg->ent_block_count;
    TLV_WRITE(unit, tlv, scache_ptr, position);

    TLV_CREATE(_bcmFieldInternalGroupGroupStatus, _bcmFieldInternalArray,
               (sizeof(bcm_field_group_status_t)/sizeof(int)), &tlv);
    tlv->value = &fg->group_status;
    TLV_WRITE(unit, tlv, scache_ptr, position);


    TLV_CREATE(_bcmFieldInternalGroupGroupAset, _bcmFieldInternalArray,
               _SHR_BITDCLSIZE(bcmFieldActionCount), &tlv);
    tlv->value = &fg->aset.w;
    TLV_WRITE(unit, tlv, scache_ptr, position);

    TLV_CREATE(_bcmFieldInternalGroupCounterBmp, _bcmFieldInternalArray,
                _FIELD_MAX_COUNTER_POOLS, &tlv);
    tlv->value = &fg->counter_pool_bmp.w;
    TLV_WRITE(unit, tlv, scache_ptr, position);


    TLV_CREATE(_bcmFieldInternalGroupLtSlice, _bcmFieldInternalVariable,0,&tlv);
    temp = (int) fg->lt_slices->slice_number;
    tlv->value = &temp;
    TLV_WRITE(unit, tlv, scache_ptr, position);


    BCM_IF_ERROR_RETURN(_field_extractor_sync(unit,
                        (fg->ext_codes), _FP_MAX_ENTRY_WIDTH));

    /* _field_ltinfo_t -> we save lt_id and link to the _field_lt_info_t
       structure recovered during stage reinit */

    TLV_CREATE(_bcmFieldInternalGroupLtConfig, _bcmFieldInternalVariable,
               0, &tlv);
    tlv->value = &fg->lt_info->lt_id;
    TLV_WRITE(unit, tlv, scache_ptr, position);

    TLV_CREATE(_bcmFieldInternalGroupLtEntrySize,
               _bcmFieldInternalVariable, 0, &tlv);
    tlv->value = &fg->lt_ent_blk_cnt;
    TLV_WRITE(unit, tlv, scache_ptr, position);


    TLV_CREATE(_bcmFieldInternalLtStatusEntriesCnt,
               _bcmFieldInternalVariable, 0, &tlv);
    tlv->value = &fg->lt_grp_status.entry_count;
    TLV_WRITE(unit, tlv, scache_ptr, position);

    TLV_CREATE(_bcmFieldInternalLtStatusEntriesFree,
               _bcmFieldInternalVariable, 0, &tlv);
    tlv->value = &fg->lt_grp_status.entries_free;
    TLV_WRITE(unit, tlv, scache_ptr, position);

    TLV_CREATE(_bcmFieldInternalLtStatusEntriesTotal,
               _bcmFieldInternalVariable, 0, &tlv);
    tlv->value = &fg->lt_grp_status.entries_total;
    TLV_WRITE(unit, tlv, scache_ptr, position);

    TLV_CREATE(_bcmFieldInternalGroupHintId,
               _bcmFieldInternalVariable, 0, &tlv);
    tlv->value = &fg->hintid;
    TLV_WRITE(unit, tlv, scache_ptr, position);

    TLV_CREATE(_bcmFieldInternalGroupMaxSize,
               _bcmFieldInternalVariable, 0, &tlv);
    tlv->value = &fg->max_group_size;
    TLV_WRITE(unit, tlv, scache_ptr, position);

    TLV_CREATE(_bcmFieldInternalEndStructGroup,
               _bcmFieldInternalVariable, 0, &tlv);
    tlv->value = &endmarker;
    TLV_WRITE(unit, tlv, scache_ptr, position);

    return rv;

}

/*
 * Function:
 * _bcm_field_th_ingress_scache_sync
 *
 * Purpose:
 *   Save field module software state to external cache.
 *
 * Parameters:
 * unit             - (IN) BCM device number
 * fc               - (IN) Pointer to device field control structure
 * stage_fc         - (IN) FP stage control info.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_th_ingress_scache_sync(int unit,
                                  _field_stage_t   *stage_fc)
{
    uint32 group_count = 0;                 /* Number of groups */
    uint32 slice_id,pipe_id,lt_id;          /* Iterator varaiables */
    struct _field_slice_s *curr_slice;      /* Slice information */
    struct _field_lt_slice_s *curr_ltslice; /* LT slice information */
    _field_group_t      *fg;                /* Group information */
    _field_tlv_t *tlv = NULL;               /* TLV data */
    uint8 *scache_ptr = NULL;               /* Scache  Pointer */
    int  num_pipes = 0;                     /* Number of pipes */
    int size = 0;                           /* size of scache for IFP */
    int rv = BCM_E_NONE;                    /* return value */
    _field_control_t *fc;                   /* Field Control Structure */
    uint32 *position;                       /* Pointer to scache pos */

    WB_FIELD_CONTROL_GET(fc, scache_ptr, position);
    size = fc->scache_pos;
    FP_LOCK(fc);

    if (stage_fc->oper_mode == bcmFieldGroupOperModeGlobal) {
        num_pipes = 1;
    } else if (stage_fc->oper_mode == bcmFieldGroupOperModePipeLocal) {
        num_pipes = (_FP_MAX_NUM_PIPES);
    }

    /* _field_control_t */
    LOG_DEBUG(BSL_LS_BCM_FP,(BSL_META_U(unit,"FP(unit %d):"
                                        "_bcm_field_th_ingress_scache_sync -"
                                        "Syncing _field_control_t "
                                        "from pos = %d\r\n"),unit,
                                        fc->scache_pos));
    rv = _field_control_sync(unit);
    BCM_IF_ERROR_CLEANUP(rv);

    /*_field_stage_t */
    LOG_DEBUG(BSL_LS_BCM_FP,(BSL_META_U(unit,"FP(unit %d):"
                                        "_bcm_field_th_ingress_scache_sync -"
                                        "Syncing _field_stage_t "
                                        "from pos = %d\r\n"),unit,
                                        fc->scache_pos));

    rv = _field_stage_sync(unit,stage_fc);
    BCM_IF_ERROR_CLEANUP(rv);

    TLV_CREATE(_bcmFieldInternalSlice,
               _bcmFieldInternalVariable, 0, &tlv);
    TLV_WRITE(unit, tlv, scache_ptr, position);

    /* _field_slice_t and _field_lt_slice_t */
    LOG_DEBUG(BSL_LS_BCM_FP,(BSL_META_U(unit,"FP(unit %d):"
                                        "_bcm_field_th_ingress_scache_sync -"
                                        "Syncing _field_slice_t,"
                                        "_field_lt_slice_t from pos = %d\r\n"),
                                        unit,fc->scache_pos));

    for (pipe_id = 0; pipe_id < num_pipes; pipe_id++) {
        curr_slice = stage_fc->slices[pipe_id];
        curr_ltslice = stage_fc->lt_slices[pipe_id];
        if (curr_slice != NULL) {
            for (slice_id= 0; slice_id < stage_fc->tcam_slices; slice_id ++) {
    	        rv = _field_slice_sync(unit, (curr_slice + slice_id));
                BCM_IF_ERROR_CLEANUP(rv);

            }
        } else {
            LOG_ERROR(BSL_LS_BCM_FP,(BSL_META_U(unit, "No slices to sync\n")));
        }
        if (curr_ltslice != NULL) {
            for (lt_id = 0; lt_id < stage_fc->tcam_slices; lt_id++) {
                rv = _field_lt_slice_sync(unit, (curr_ltslice + lt_id));
                BCM_IF_ERROR_CLEANUP(rv);
            }
        } else {
           LOG_ERROR(BSL_LS_BCM_FP,(BSL_META_U(unit,"No LT slices to sync\n")));
        }
    }

    /* _field_group_t */
    LOG_DEBUG(BSL_LS_BCM_FP,(BSL_META_U(unit,"FP(unit %d):"
                                        "_bcm_field_th_ingress_scache_sync -"
                                        "Syncing _field_group_t "
                                        "from pos = %d\r\n"),unit,
                                        fc->scache_pos));

    fg = fc->groups;
    while (fg != NULL) {
        if (fg->stage_id == _BCM_FIELD_STAGE_INGRESS) {
            group_count ++ ;
        }
        fg = fg->next;
	}

    TLV_CREATE(_bcmFieldInternalGroupCount, _bcmFieldInternalVariable, 0, &tlv);
    tlv->value = &group_count;
    TLV_WRITE(unit, tlv, scache_ptr, position);

    /* Iterate over the groups linked-list */
    fg = fc->groups;
    while (fg != NULL) {
        if (fg->stage_id == _BCM_FIELD_STAGE_INGRESS) {
            rv = _field_group_sync(unit,fg);
            BCM_IF_ERROR_CLEANUP(rv);
        }
        fg = fg->next;
	}

    /* _field_entry_t and _field_lt_entry_t */
    LOG_DEBUG(BSL_LS_BCM_FP,(BSL_META_U(unit,"FP(unit %d):"
                                        "_bcm_field_th_ingress_scache_sync -"
                                        "Syncing _field_entry_t,"
                                        "_field_lt_entry_t from pos = %d\r\n"),
                                        unit,fc->scache_pos));

    rv = _field_entry_sync(unit);
    BCM_IF_ERROR_CLEANUP(rv);

    LOG_DEBUG(BSL_LS_BCM_FP,(BSL_META_U(unit,"FP(unit %d):"
                                        "_bcm_field_th_ingress_scache_sync -"
                                        "End of structures sync "
                                        "at pos = %d\r\n"),unit,
                                        fc->scache_pos));

    TLV_CREATE(_bcmFieldInternalEndStructIFP,
               _bcmFieldInternalVariable, 0, &tlv);
    TLV_WRITE(unit, tlv, scache_ptr, position);


    size = (fc->scache_pos - size);
    LOG_VERBOSE(BSL_LS_BCM_FP,(BSL_META_U(unit, "Final scache_size = %d\n"),
                                          size));

cleanup:
    if (tlv != NULL) {
        sal_free(tlv);
        tlv = NULL;
    }
    FP_UNLOCK(fc);

    return rv;
}

/* Function:
 * _field_wb_size_calc
 *
 * Purpose:
 *   Calculate size required for IFP WB section
 *
 * Parameters:
 * total_size       - (OUT) Total size required for allocation
 * fc               - (IN) Pointer to device field control structure
 *
 * Returns:
 *      BCM_E_XXX
 */

int
_field_wb_size_calc(int unit, uint32* total_size, _field_control_t  *fc)
{

    int entry_count = 512;              /* Max number of entries per group */
    int slice_count = 12;               /* Max number of slices */
    int basic_size_slice,basic_size_fc,basic_size_stage,basic_size_ltslice = 0;
                                        /* Basic sizes of structures */
    int basic_size_group,basic_size_entry,basic_size_ltentry = 0;
    int basic_size_datafc = 0;
                                        /* Basic sizes of structures */
    int size_slice,size_fc,size_stage,size_ltslice = 0;
                                        /* Total sizes of structures */
    int size_group,size_entry,size_ltentry,size_datafc = 0;
                                        /* Total sizes of structures */
    int type_slice,type_fc,type_stage,type_ltslice = 0;
                                        /* Total size of types of structures */
    int type_group,type_entry,type_ltentry,type_datafc = 0;
                                        /* Total size of types of structures */

    int length_slice,length_fc,length_stage,length_ltslice = 0;
                                        /* Total size of length of structures */
    int length_group,length_entry,length_ltentry,length_datafc = 0;
                                        /* Total size of length of structures */
    int total_type = 0, total_length = 0;

    basic_size_slice = 43;
    basic_size_stage = 596;
    basic_size_ltslice = 32;
    basic_size_group = 94;
    basic_size_entry = 47 + 48 + 12 + (20 * 12);
                                       /* 20 actions needs software cache */
    basic_size_ltentry = 28;
    basic_size_datafc = 799756;
    basic_size_fc = 21;

    size_slice = basic_size_slice + (entry_count * (4 + (5/8)))
                 + (BCM_PBMP_PORT_MAX/8) + (111 * _FP_MAX_NUM_LT) ;

    size_stage = basic_size_stage + ( _FP_MAX_NUM_PIPES *_FIELD_MAX_METER_POOLS
                 * (13 + (entry_count* (2/8))))
                 + (_FIELD_MAX_CNTR_POOLS * (5 + (entry_count*(3/8))))
    			 + (_FP_MAX_NUM_PIPES * slice_count * 4)
                 + (_FP_MAX_NUM_PIPES *_FP_MAX_NUM_LT * 76);

    size_ltslice = basic_size_ltslice + (entry_count * 4);

    size_group = basic_size_group + ((BCM_FIELD_QUALIFY_MAX/8)
                 + (BCM_FIELD_USER_NUM_UDFS/8)) + (BCM_PBMP_PORT_MAX/8)
    			 + (_FP_MAX_ENTRY_TYPES * _FP_MAX_ENTRY_WIDTH * 10304)
                 + (entry_count * 4) + (bcmFieldActionCount/8)
    			 + (4 *_FIELD_MAX_COUNTER_POOLS) + (111 * _FP_MAX_ENTRY_WIDTH)
                 + (entry_count * 4) + (slice_count * 80) ;

    size_entry = (basic_size_entry + ((BCM_PBMP_PORT_MAX/8) * 2)
                  + (5 * _FP_POLICER_LEVEL_COUNT));

    size_ltentry = basic_size_ltentry;

    size_datafc = basic_size_datafc + (_FP_DATA_ETHERTYPE_MAX * 16)
                  + (_FP_DATA_IP_PROTOCOL_MAX * 21);

    size_fc = basic_size_fc + (10 * BCM_FIELD_USER_NUM_UDFS)
              + (_FP_MAX_NUM_LT * 4)
              + ((6144 * 97) + (8144 * 74)); /* policer and stats hash */

    /* Currently Tomahawk doesnt have any hints added.
     * Total size required for hints
     * as per max limit calculation is (1024 * (8 + (36 * 256)))
     * i.e 1024 hint structures and 256 hints per structure
     */

    LOG_VERBOSE(BSL_LS_BCM_FP,(BSL_META_U(unit, "slice size = %d\n"),
              (slice_count *size_slice)));
    LOG_VERBOSE(BSL_LS_BCM_FP,(BSL_META_U(unit, "lt slice size = %d\n"),
              (slice_count * size_ltslice)));
    LOG_VERBOSE(BSL_LS_BCM_FP,(BSL_META_U(unit, "stage size = %d\n"),size_stage));
    LOG_VERBOSE(BSL_LS_BCM_FP,(BSL_META_U(unit, "group size = %d\n"),
             (32 * 4 * size_group)));
    LOG_VERBOSE(BSL_LS_BCM_FP,(BSL_META_U(unit, "entry size = %d\n"),
             (12 * 512 * 4 * size_entry)));
    LOG_VERBOSE(BSL_LS_BCM_FP,(BSL_META_U(unit, "ltentry size = %d\n"),
             (12 * 32 * 4 * size_ltentry)));
    LOG_VERBOSE(BSL_LS_BCM_FP,(BSL_META_U(unit, "fc size = %d\n"),size_fc));
    LOG_VERBOSE(BSL_LS_BCM_FP,(BSL_META_U(unit, "datafc size = %d\n"),
              size_datafc));

    *total_size = size_fc + size_datafc + size_stage + (slice_count
                  * (size_slice + size_ltslice))
                  + (32 * 4 * size_group)
                  + (12 * 32 * 4 * size_ltentry)
                  + (12 * 512 * 4 * size_entry);

     LOG_VERBOSE(BSL_LS_BCM_FP,(BSL_META_U(unit, "total value = %d\n"),
               *total_size));


     type_fc = 64 + (13 * 256);
     type_datafc = 23 + (10 *512);
     type_stage = 48;
     type_slice = 50;
     type_ltslice = 10;
     type_group = 22 + 9 + (3 *12);
     type_ltentry = 7;
     type_entry = 25;
     total_type = type_fc + type_datafc + type_stage + (slice_count
                       * (type_slice + type_ltslice))
                       + (32 * 4 * type_group)
                       + (12 * 32 * 4 * type_ltentry)
                       + (12 * 512 * 4 * type_entry);

     total_type = total_type * 4;

     LOG_VERBOSE(BSL_LS_BCM_FP,(BSL_META_U(unit, "total type = %d\n"),
                                         total_type));


     length_slice = 32 *10;
     length_fc = length_datafc= length_stage = length_ltslice = length_entry= 0;
     length_ltentry = 2;
     length_group = 10;

     total_length = length_fc + length_datafc + length_stage + (slice_count
                       * (length_slice + length_ltslice))
                       + (32 * 4 * length_group)
                       + (12 * 32 * 4 * length_ltentry)
                       + (12 * 512 * 4 * length_entry);

     total_length = total_length * 4;
     LOG_VERBOSE(BSL_LS_BCM_FP,(BSL_META_U(unit, "total length = %d\n"),
               total_length));

      *total_size = *total_size + total_type + total_length;

     LOG_VERBOSE(BSL_LS_BCM_FP,(BSL_META_U(unit, "total value = %d\n"),
               *total_size));


    return BCM_E_NONE;

}

/* Function:
 * _bcm_field_scache_th_pointer_realloc
 *
 * Purpose:
 *   Initialize scache pointers for warmboot
 *
 * Parameters:
 * unit             - (IN) BCM device number
 * fc               - (IN) Pointer to device field control structure
 *
 * Returns:
 *      BCM_E_XXX
 */

int
_bcm_field_scache_th_pointer_realloc(int unit, _field_control_t  *fc)
{
    int               stable_size;        /* Total scache size */
    int               rv;                 /* Return variable */
    uint32            alloc_get;          /* Allocated size */
    soc_scache_handle_t handle;           /* scache handle */
    uint32              ifp_calculated_size;
                                          /* total scache size for IFP */

    if ((NULL != fc->scache_ptr[_FIELD_SCACHE_PART_0]) ||
        (NULL != fc->scache_ptr[_FIELD_SCACHE_PART_1])) {

        SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));
        SOC_SCACHE_HANDLE_SET(handle, unit, BCM_MODULE_FIELD,
                              _FIELD_SCACHE_PART_0);
        if ((stable_size > 0) && !SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit)) {

             BCM_IF_ERROR_RETURN(_field_wb_size_calc(unit,
                                                     &ifp_calculated_size,fc));

             /* value should be a multiple of 1024 */
             ifp_calculated_size = (((ifp_calculated_size + 1023) / 1024)
                                      * 1024);

             fc->scache_size[_FIELD_SCACHE_PART_0] = ((294 * 1024)
                                                      + (ifp_calculated_size)) ;

             fc->scache_size[_FIELD_SCACHE_PART_1] = 20 * 1024;

          /* Partition 0 size validation */

            /* Get the pointer for the Level 2 cache */
            rv = soc_scache_ptr_get(unit, handle,
                                    &(fc->scache_ptr[_FIELD_SCACHE_PART_0]),
                                    &alloc_get);
            if (BCM_FAILURE(rv)) {
                return rv;
            } else if (alloc_get != (fc->scache_size[_FIELD_SCACHE_PART_0]
                                     + SOC_WB_SCACHE_CONTROL_SIZE)) {
                /* Allocated size is less than expected size */
                   if (SOC_WARM_BOOT(unit)) {
                       SOC_IF_ERROR_RETURN
                        (soc_scache_realloc(unit, handle,
                        (fc->scache_size[_FIELD_SCACHE_PART_0] - (alloc_get
                                        - SOC_WB_SCACHE_CONTROL_SIZE))));
                       rv = soc_scache_ptr_get(unit, handle,
                                       &(fc->scache_ptr[_FIELD_SCACHE_PART_0]),
                                       &alloc_get);
                       if (BCM_FAILURE(rv)) {
                           return rv;
                       } else if (alloc_get !=
                                  (fc->scache_size[_FIELD_SCACHE_PART_0] +
                                   SOC_WB_SCACHE_CONTROL_SIZE)) {
                           /* Expected size doesn't match retrieved size */
                           return BCM_E_INTERNAL;
                       } else if (NULL == fc->scache_ptr[_FIELD_SCACHE_PART_0]) {
                           return BCM_E_MEMORY;
                       }
                   }
            } else if (NULL == fc->scache_ptr[_FIELD_SCACHE_PART_0]) {
                return BCM_E_MEMORY;
            }

        }
   }
   return  BCM_E_NONE;
}

/* Function:
 * _bcm_field_scache_th_pointer_init
 *
 * Purpose:
 *   Initialize scache pointers for warmboot
 *
 * Parameters:
 * unit             - (IN) BCM device number
 * fc               - (IN) Pointer to device field control structure
 *
 * Returns:
 *      BCM_E_XXX
 */

int
_bcm_field_scache_th_pointer_init(int unit, _field_control_t  *fc)
{
    int               stable_size;        /* Total scache size */
    int               rv;                 /* Return variable */
    uint32            alloc_get;          /* Allocated size */
    soc_scache_handle_t handle;           /* scache handle */
    uint32              ifp_calculated_size;
                                          /* total scache size for IFP */

    if ((NULL == fc->scache_ptr[_FIELD_SCACHE_PART_0]) ||
        (NULL == fc->scache_ptr[_FIELD_SCACHE_PART_1])) {

        SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));
        SOC_SCACHE_HANDLE_SET(handle, unit, BCM_MODULE_FIELD,
                              _FIELD_SCACHE_PART_0);
        if ((stable_size > 0) && !SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit)) {

             BCM_IF_ERROR_RETURN(_field_wb_size_calc(unit,
                                                     &ifp_calculated_size,fc));

             /* value should be a multiple of 1024 */
             ifp_calculated_size = (((ifp_calculated_size + 1023) / 1024)
                                      * 1024);

             fc->scache_size[_FIELD_SCACHE_PART_0] = ((294 * 1024)
                                                      + (ifp_calculated_size)) ;
             fc->scache_size[_FIELD_SCACHE_PART_1] = 20 * 1024;

            /* Partition 0 size validation */

            /* Get the pointer for the Level 2 cache */
            rv = soc_scache_ptr_get(unit, handle,
                                    &(fc->scache_ptr[_FIELD_SCACHE_PART_0]),
                                    &alloc_get);
            if (!SOC_WARM_BOOT(unit) && (SOC_E_NOT_FOUND == rv)) {
                /* Not yet allocated in Cold Boot */
                SOC_IF_ERROR_RETURN
                    (soc_scache_alloc(unit, handle,
                                      (fc->scache_size[_FIELD_SCACHE_PART_0]
                                      + SOC_WB_SCACHE_CONTROL_SIZE)));
                rv = soc_scache_ptr_get(unit, handle,
                                        &(fc->scache_ptr[_FIELD_SCACHE_PART_0]),
                                        &alloc_get);
            }

            if (BCM_FAILURE(rv)) {
                return rv;
            } else if (alloc_get != (fc->scache_size[_FIELD_SCACHE_PART_0]
                                     + SOC_WB_SCACHE_CONTROL_SIZE)) {
                if (!SOC_WARM_BOOT(unit)) {
                    /* Allocated size is greater than expected size */
                    return BCM_E_INTERNAL;
                }
            } else if (NULL == fc->scache_ptr[_FIELD_SCACHE_PART_0]) {
                return BCM_E_MEMORY;
            }

            /* Partition 1 size validation*/

            SOC_SCACHE_HANDLE_SET(handle, unit, BCM_MODULE_FIELD, 1);
            SOC_SCACHE_MODULE_MAX_PARTITIONS_SET(unit, BCM_MODULE_FIELD, 1);

            /* Get the pointer for the Level 2 cache */
            rv = soc_scache_ptr_get(unit, handle,
                                    &(fc->scache_ptr[_FIELD_SCACHE_PART_1]),
                                    &alloc_get);
            if (!SOC_WARM_BOOT(unit) && (SOC_E_NOT_FOUND == rv)) {
                /* Not yet allocated in Cold Boot */
                SOC_IF_ERROR_RETURN
                    (soc_scache_alloc(unit, handle,
                                      (fc->scache_size[_FIELD_SCACHE_PART_1]
                                      + SOC_WB_SCACHE_CONTROL_SIZE)));
                rv = soc_scache_ptr_get(unit, handle,
                                        &(fc->scache_ptr[_FIELD_SCACHE_PART_1]),
                                        &alloc_get);
            }

            if (BCM_FAILURE(rv)) {
                return rv;
            } else if (alloc_get != (fc->scache_size[_FIELD_SCACHE_PART_1]
                                    + SOC_WB_SCACHE_CONTROL_SIZE)) {
                /* Expected size doesn't match retrieved size */
                return BCM_E_INTERNAL;
            } else if (NULL == fc->scache_ptr[_FIELD_SCACHE_PART_1]) {
                return BCM_E_MEMORY;
            }

        }
   }
   return  BCM_E_NONE;
}

#define TLV_INIT(tlv)                    \
    do {                                 \
        tlv.type = -1;                   \
        tlv.length = 0;                  \
        if (NULL != tlv.value) {         \
            sal_free(tlv.value);         \
        }                                \
        tlv.value = NULL;                \
    }  while(0);

/* Function:
 * _field_policer_recover
 *
 * Purpose:
 *    recover _field_policer_t structure
 *
 * Paramaters:
 * unit             - (IN) BCM device number
 * tlv              - (IN) TLV structure for field
 * Returns:
 *     BCM_E_XXX
 */

int
_field_policer_recover(int unit, _field_tlv_t *tlv)
{

    int type_pos = 0;                     /* position of type */
    int i = 0;                            /* local variable for loops */
    int num_instances = 0;                /* number of instances */
    int num_types = 0;                    /* Number of types */
    _field_tlv_t tlv2;                    /* TLV structure */
    uint32 *type;                         /* Elem type */
    uint8 *scache_ptr;                    /* Scache pointer */
    uint32 *position;                     /* Scache position */
    _field_policer_t    *f_pl = NULL;     /* Field policer structure */
    _field_control_t        *fc;          /* Field control structure.  */
    int rv = 0;

    WB_FIELD_CONTROL_GET(fc, scache_ptr, position);
	tlv2.value = NULL;
	TLV_INIT(tlv2);
    type = (uint32 *)tlv->value;
    num_instances = (tlv->length) & _FP_WB_TLV_LENGTH_MASK;
    num_types     = ((tlv->length & _FP_WB_TLV_TYPES_MASK) >>
                    _FP_WB_TLV_TYPE_SHIFT);

    for (i = 0; i < num_instances; i++) {

        _FP_XGS3_ALLOC(f_pl, sizeof (_field_policer_t), "Field policer");
        if (NULL == f_pl) {
           return (BCM_E_MEMORY);
        }
        type_pos = 0;

        while  ((tlv2.type != _bcmFieldInternalEndStructPolicer)
                 && (type_pos != num_types)) {
            TLV_INIT(tlv2);
            tlv2.type = (type[type_pos] & _FP_WB_TLV_ELEM_TYPE_MASK);
            tlv2.basic_type = ((type[type_pos] & _FP_WB_TLV_BASIC_TYPE_MASK) >>
                              _FP_WB_TLV_BASIC_TYPE_SHIFT);
            rv = tlv_read(unit, &tlv2, scache_ptr, position);
            BCM_IF_ERROR_CLEANUP(rv);

            switch (tlv2.type) {

                case _bcmFieldInternalPolicerPid:
                     f_pl->pid = *(bcm_policer_t *)tlv2.value;
                     break;
                case _bcmFieldInternalPolicerCfgFlags:
                     f_pl->cfg.flags= *(uint32 *)tlv2.value;
                     break;

                case _bcmFieldInternalPolicerCfgMode:
                     f_pl->cfg.mode = *(bcm_policer_mode_t *)tlv2.value;
                     break;

                case _bcmFieldInternalPolicerCfgCkbitsSec:
                     f_pl->cfg.ckbits_sec = *(uint32 *)tlv2.value;
                     break;

                case _bcmFieldInternalPolicerCfgMaxCkbitsSec:
                     f_pl->cfg.max_ckbits_sec = *(uint32 *)tlv2.value;
                     break;


                case _bcmFieldInternalPolicerCfgCkbitsBurst:
                     f_pl->cfg.ckbits_burst = *(uint32 *)tlv2.value;
                     break;

                case _bcmFieldInternalPolicerCfgPkbitsSec:
                     f_pl->cfg.pkbits_sec = *(uint32 *)tlv2.value;
                     break;

                case _bcmFieldInternalPolicerCfgMaxPkbitsSec:
                     f_pl->cfg.max_pkbits_sec = *(uint32 *)tlv2.value;
                     break;

                case _bcmFieldInternalPolicerCfgPkbitsBurst :
                     f_pl->cfg.pkbits_burst = *(uint32 *)tlv2.value;
                     break;

                case _bcmFieldInternalPolicerCfgKbitsCurrent:
                     f_pl->cfg.kbits_current = *(uint32 *)tlv2.value;
                     break;

                case _bcmFieldInternalPolicerCfgActionId:
                     f_pl->cfg.action_id = *(uint32 *)tlv2.value;
                     break;


                case _bcmFieldInternalPolicerCfgSharingMode:
                     f_pl->cfg.action_id = *(uint32 *)tlv2.value;
                     break;

                case _bcmFieldInternalPolicerCfgEntropyId:
                     f_pl->cfg.entropy_id = *(uint32 *)tlv2.value;
                     break;

                case _bcmFieldInternalPolicerCfgPoolId:
                     f_pl->cfg.pool_id = *(bcm_policer_pool_t *)tlv2.value;
                     break;

                case _bcmFieldInternalPolicerSwRefCount:
                     f_pl->sw_ref_count = *(uint16 *)tlv2.value;
                     break;

                case _bcmFieldInternalPolicerHwRefCount:
                     f_pl->hw_ref_count = *(uint16 *)tlv2.value;
                     break;

                case _bcmFieldInternalPolicerLevel:
                     f_pl->level = *(uint8 *)tlv2.value;
                     break;

                case _bcmFieldInternalPolicerPoolIndex:
                     f_pl->pool_index = *(int8 *)tlv2.value;
                     break;

                case _bcmFieldInternalPolicerHwIndex:
                     f_pl->hw_index = *(int *)tlv2.value;
                     break;

                case _bcmFieldInternalPolicerHwFlags:
                     f_pl->hw_flags = *(uint32 *)tlv2.value;
                     break;

                case _bcmFieldInternalPolicerStageId:
                     f_pl->stage_id = *(_field_stage_id_t *)tlv2.value;
                     break;

                case _bcmFieldInternalEndStructPolicer:
                     if (*(uint32 *)tlv2.value != _FIELD_WB_EM_POLICER) {
                        LOG_ERROR(BSL_LS_BCM_FP,(BSL_META_U(unit,
                                       "END MARKER CHECK FAILED : POLICER\n")));
                        return BCM_E_INTERNAL;
                     }
                     break;
                default :
                     LOG_ERROR(BSL_LS_BCM_FP,(BSL_META_U(unit,
                               "unknown type detected : POLICER recovery\n")));
                     rv = BCM_E_INTERNAL;
                     goto cleanup;
            }
            type_pos++;
        }

        TLV_INIT(tlv2);

        _FP_HASH_INSERT(fc->policer_hash, f_pl,
                 ((f_pl->pid) & _FP_HASH_INDEX_MASK(fc)));
        f_pl = NULL;
    }

    return BCM_E_NONE;

cleanup:
    TLV_INIT(tlv2);
    if (f_pl != NULL) {
        sal_free(f_pl);
        f_pl = NULL;
    }
    return rv;


}

/* Function:
 * _field_stat_recover
 *
 * Purpose:
 *    recover _field_stat_t structure
 *
 * Paramaters:
 * unit             - (IN) BCM device number
 * fc               - (IN) Pointer to device fid control structure
 * tlv              - (IN) TLV structure for field
 * Returns:
 *     BCM_E_XXX
 */

int
_field_stat_recover(int unit, _field_tlv_t *tlv)
{

    int type_pos = 0;                     /* position of type */
    int i = 0;                            /* local variable for loops */
    int num_instances = 0;                /* number of instances */
    int num_types = 0;                    /* Number of types */
    _field_tlv_t tlv2 ;                   /* TLV structure */
    uint32 *type;                         /* Elem type */
    uint8 *scache_ptr;                    /* Scahce pointer */
    uint32 *position;                     /* Scache position */
    _field_stat_t       *f_st = NULL;     /* Field Stat Structure */
    _field_control_t    *fc;              /* Field control structure.   */
    int rv = 0;                           /* Return variable */

	tlv2.value = NULL;
    TLV_INIT(tlv2);
    WB_FIELD_CONTROL_GET(fc, scache_ptr, position);

    type = (uint32 *)tlv->value;
    num_instances = (tlv->length) & _FP_WB_TLV_LENGTH_MASK;
    num_types     = ((tlv->length & _FP_WB_TLV_TYPES_MASK) >>
                    _FP_WB_TLV_TYPE_SHIFT);
    for (i = 0; i < num_instances; i++) {

        _FP_XGS3_ALLOC(f_st, sizeof (_field_stat_t), "Field stat entity");
        if (NULL == f_st) {
            return (BCM_E_MEMORY);
        }
        type_pos = 0;

        while  ((tlv2.type != _bcmFieldInternalEndStructStat)
                 && (type_pos != num_types)) {
            TLV_INIT(tlv2);
            tlv2.type = (type[type_pos] & _FP_WB_TLV_ELEM_TYPE_MASK);
            tlv2.basic_type = ((type[type_pos] & _FP_WB_TLV_BASIC_TYPE_MASK) >>
                              _FP_WB_TLV_BASIC_TYPE_SHIFT);
            rv = tlv_read(unit, &tlv2, scache_ptr, position);
            BCM_IF_ERROR_CLEANUP(rv);

            switch (tlv2.type) {
                case _bcmFieldInternalStatsId:
                    f_st->sid = *(uint32 *)tlv2.value;
                    break;
                case _bcmFieldInternalStatSwRefCount:
                    f_st->sw_ref_count = *(uint16 *)tlv2.value;
                    break;
                case _bcmFieldInternalStatHwRefCount:
                    f_st->hw_ref_count = *(uint16 *)tlv2.value;
                    break;
                case _bcmFieldInternalStatOffsetMode:
                    f_st->offset_mode = *(int8 *)tlv2.value;
                    break;
                case _bcmFieldInternalStatPoolIndex:
                    f_st->pool_index = *(int8 *)tlv2.value;
                    break;
                case _bcmFieldInternalStatHwIndex :
                    f_st->hw_index = *(int *)tlv2.value;
                    break;
                case _bcmFieldInternalStatHwFlags:
                    f_st->hw_flags = *(uint32 *)tlv2.value;
                    break;
                case _bcmFieldInternalStatnStat:
                    f_st->nstat = *(uint8 *)tlv2.value;
                    break;
                case _bcmFieldInternalStatArr:
                    _FP_XGS3_ALLOC(f_st->stat_arr,
                                  (tlv2.length * sizeof (bcm_field_stat_t)),
                                   "Field stat array");
                    if (NULL == f_st->stat_arr) {
                        rv = (BCM_E_MEMORY);
                        goto cleanup;
                    }
                    sal_memcpy(f_st->stat_arr, tlv2.value,
                             ((static_type_map[tlv2.type].size) * tlv2.length));
                    break;
                case _bcmFieldInternalStatHwStat:
                    f_st->hw_stat = *(uint32 *)tlv2.value;
                    break;
                case _bcmFieldInternalStatHwMode:
                    f_st->hw_mode = *(uint16 *)tlv2.value;
                    break;
                case _bcmFieldInternalStatHwEntryCount:
                    f_st->hw_entry_count = *(uint8 *)tlv2.value;
                    break;
                case _bcmFieldInternalStatGid:
                    f_st->gid = *(bcm_field_group_t *)tlv2.value;
                    break;
                case _bcmFieldInternalStatStageId:
                    f_st->stage_id = *(_field_stage_id_t *)tlv2.value;
                    break;
                case _bcmFieldInternalStatStatValues:
                    _FP_XGS3_ALLOC(f_st->stat_values,
                                  (tlv2.length * sizeof (uint64)),
                                    "Field stat values array");
                     if (NULL == f_st->stat_values) {
                         rv = (BCM_E_MEMORY);
                         goto cleanup;
                     }
                    sal_memcpy(f_st->stat_values, tlv2.value,
                       ((static_type_map[tlv2.type].size) * tlv2.length));
                    break;
                case _bcmFieldInternalStatFlexMode:
                    f_st->flex_mode = *(uint32 *)tlv2.value;
                    break;
                case _bcmFieldInternalEndStructStat:
                    if (*(uint32 *)tlv2.value != _FIELD_WB_EM_STAT) {
                        LOG_ERROR(BSL_LS_BCM_FP,(BSL_META_U(unit,
                                        "END MARKER CHECK FAILED : STAT\n")));
                        return BCM_E_INTERNAL;
                    }
                    break;
                default:
                    LOG_ERROR(BSL_LS_BCM_FP,(BSL_META_U(unit,
                               "unknown type detected : STAT recovery\n")));
                    rv = BCM_E_INTERNAL;
                    goto cleanup;
            }
            type_pos++;
        }
        TLV_INIT(tlv2);
        _FP_HASH_INSERT(fc->stat_hash, f_st,
                        (f_st->sid & _FP_HASH_INDEX_MASK(fc)));
        f_st = NULL;

    }

    return BCM_E_NONE;
cleanup:
    TLV_INIT(tlv2);
    if (f_st != NULL) {
        if (f_st->stat_arr != NULL) {
            sal_free(f_st->stat_arr);
            f_st->stat_arr = NULL;
        }
        sal_free(f_st);
        f_st = NULL;
    }
    return rv;
}

/* Function:
 * _field_hint_count_array_recover
 *
 * Purpose:
 *    recover _bcm_field_hint_t structure
 *
 * Paramaters:
 * unit             - (IN) BCM device number
 * tlv              - (IN) TLV structure for field
 * f_ht             - (OUT) Field hints are stored
 * Returns:
 *     BCM_E_XXX
 */

int
_field_hint_count_array_recover(int unit, _field_tlv_t *tlv,
                         _field_hints_t *f_ht)
{
    int type_pos = 0;                     /* position of type */
    int i = 0;                            /* local variable for loops */
    int num_instances = 0;                /* number of instances */
    int num_types = 0;                    /* Number of types */
    _field_tlv_t tlv2;                    /* TLV structure */
    uint32 *type;                         /* Elem type */
    uint8 *scache_ptr;                    /* Scahce pointer */
    uint32 *position;                     /* Scache position */
    _field_hint_t     *hint_entry = NULL; /* Field hint structure */
    _field_control_t  *fc;                /* Field control structure.       */
    int rv = 0;                           /* Return variable */

	tlv2.value = NULL;
    TLV_INIT(tlv2);
    WB_FIELD_CONTROL_GET(fc, scache_ptr, position);

    type = (uint32 *)tlv->value;
    num_instances = (tlv->length) & _FP_WB_TLV_LENGTH_MASK;
    num_types     = ((tlv->length & _FP_WB_TLV_TYPES_MASK) >>
                    _FP_WB_TLV_TYPE_SHIFT);

    for (i = 0; i < num_instances; i++) {
         _FP_XGS3_ALLOC (hint_entry, sizeof (_field_hint_t),
                         "Field Hint Entry Structure");
         _FP_XGS3_ALLOC (hint_entry->hint, sizeof (bcm_field_hint_t),
                         "Field Hint Entry");
         hint_entry->next = NULL;
         type_pos = 0;

         while ((tlv2.type != _bcmFieldInternalEndStructHintHints)
                 && (type_pos != num_types)) {
             TLV_INIT(tlv2);
             tlv2.type = (type[type_pos] & _FP_WB_TLV_ELEM_TYPE_MASK);
             tlv2.basic_type =((type[type_pos] & _FP_WB_TLV_BASIC_TYPE_MASK) >>
                              _FP_WB_TLV_BASIC_TYPE_SHIFT);
             rv = tlv_read(unit, &tlv2, scache_ptr, position);
             BCM_IF_ERROR_CLEANUP(rv);
             switch (tlv2.type) {
                case _bcmFieldInternalHintHintsHintType:
                     hint_entry->hint->hint_type =
                                *(bcm_field_hint_type_t *)tlv2.value;
                     break;
                case _bcmFieldInternalHintHintsQual:
                     hint_entry->hint->qual =
                             *(bcm_field_qualify_t *)tlv2.value;
                     break;
                case _bcmFieldInternalHintHintsMaxValues:
                     hint_entry->hint->max_values = *(uint32 *)tlv2.value;
                     break;
                case _bcmFieldInternalHintHintsStartBit:
                     hint_entry->hint->start_bit = *(uint32 *)tlv2.value;
                     break;
                case _bcmFieldInternalHintHintsEndBit:
                     hint_entry->hint->end_bit = *(uint32 *)tlv2.value;
                     break;
                case _bcmFieldInternalHintHintsFlags:
                     hint_entry->hint->flags = *(uint32 *)tlv2.value;
                     break;
                case _bcmFieldInternalHintHintsMaxGrpSize:
                     hint_entry->hint->max_group_size = *(uint32 *)tlv2.value;
                     break;
                case _bcmFieldInternalEndStructHintHints:
                     break;
                default:
                     LOG_ERROR(BSL_LS_BCM_FP,(BSL_META_U(unit,
                   "unknown type detected : hint_count_array_recover\n")));
                     rv = BCM_E_INTERNAL;
                     goto cleanup;
            }
            type_pos++;
        }
        _FP_HINTS_LINKLIST_INSERT(&(f_ht->hints),hint_entry);
        hint_entry = NULL;

    }

    return BCM_E_NONE;
cleanup:
    TLV_INIT(tlv2);
    if (hint_entry->hint != NULL) {
        sal_free(hint_entry->hint);
        hint_entry->hint = NULL;
    }
    sal_free(hint_entry);
    hint_entry = NULL;
    return rv;

}

/* Function:
 * _field_hint_recover
 *
 * Purpose:
 *    recover _field_hint_t structure
 *
 * Paramaters:
 * unit             - (IN) BCM device number
 * tlv              - (IN) TLV structure for field
 * Returns:
 *     BCM_E_XXX
 */

int
_field_hint_recover(int unit, _field_tlv_t *tlv)
{
    int i = 0;                            /* local variable for loops */
    int num_instances = 0;                /* number of instances */
    _field_tlv_t tlv2;                    /* TLV structure */
    uint8 *scache_ptr;                    /* Scahce pointer */
    uint32 *position;                     /* Scache position */
    _field_hints_t   *f_ht = NULL;        /* field Hints structure */
    _field_control_t *fc;                 /* Field control structure */
    int rv = 0;                           /* Return variable */

	tlv2.value = NULL;
    TLV_INIT(tlv2);
    WB_FIELD_CONTROL_GET(fc, scache_ptr, position);
    num_instances = (int) tlv->length;

    for (i = 0; i < num_instances; i++) {
    _FP_XGS3_ALLOC(f_ht, _FP_HASH_SZ(fc) * \
                   sizeof(_field_hints_t), "Hints hash");
    while  (tlv2.type != _bcmFieldInternalEndStructHint) {
            TLV_INIT(tlv2);
            rv = tlv_read(unit, &tlv2, scache_ptr, position);
            BCM_IF_ERROR_CLEANUP(rv);
            switch (tlv2.type) {
                case _bcmFieldInternalHintHintid:
                    f_ht->hintid = *(bcm_field_hintid_t *)tlv2.value;
                    break;
                case _bcmFieldInternalHintHints:
                    _field_hint_count_array_recover(unit, &tlv2, f_ht);
                    break;
                case _bcmFieldInternalHintGrpRefCount:
                    f_ht->grp_ref_count = *(uint16 *)tlv2.value;
                    break;
                case _bcmFieldInternalHintCount:
                    f_ht->hint_count = *(uint16 *)tlv2.value;
                    break;
                case _bcmFieldInternalEndStructHint:
                    if (*(uint32 *)tlv2.value != _FIELD_WB_EM_HINT) {
                        LOG_ERROR(BSL_LS_BCM_FP,(BSL_META_U(unit,
                                        "END MARKER CHECK FAILED : HINT\n")));
                        return BCM_E_INTERNAL;
                    }
                    break;
                default:
                     LOG_ERROR(BSL_LS_BCM_FP,(BSL_META_U(unit,
                        "unknown type detected : _field_hint_recover\n")));
                    rv = BCM_E_INTERNAL;
                    goto cleanup;

            }
        }
        TLV_INIT(tlv2);

        _FP_HASH_INSERT(fc->hints_hash, f_ht,
                       (f_ht->hintid & _FP_HASH_INDEX_MASK(fc)));

    }
    return BCM_E_NONE;
cleanup:
    TLV_INIT(tlv2);
    if (f_ht != NULL) {
        sal_free(f_ht);
        f_ht = NULL;
    }
    return rv;

}
/* Function:
 * _field_udf_recover
 *
 * Purpose:
 *    recover _field_udf_t structure
 *
 * Paramaters:
 * unit             - (IN) BCM device number
 * fc               - (IN) Pointer to device field control structure
 *
 * Returns:
 *     BCM_E_XXX
 */
int
_field_udf_recover(int unit, uint8 *ptr, uint32 *pos, _field_udf_t *udf,
                    _field_tlv_t *tlv)
{
    int type_pos = 0;                     /* position of type */
    int i = 0;                            /* local variable for loops */
    int num_instances = 0;                /* number of instances */
    int num_types = 0;                    /* Number of types */
    _field_tlv_t tlv2;                    /* TLV structure */
    uint32 *type;                         /* Elem type */

    tlv2.value = NULL;
    TLV_INIT(tlv2);
    type = (uint32 *)tlv->value;
    num_instances = (tlv->length) & _FP_WB_TLV_LENGTH_MASK;
    num_types     = ((tlv->length & _FP_WB_TLV_TYPES_MASK) >>
                    _FP_WB_TLV_TYPE_SHIFT);

    for (i = 0; i < num_instances; i++) {
        type_pos = 0;

        while  ((tlv2.type != _bcmFieldInternalControlEndStructUdf)
                 && (type_pos != num_types)) {
            TLV_INIT(tlv2);
            tlv2.type = (type[type_pos] & _FP_WB_TLV_ELEM_TYPE_MASK);
            tlv2.basic_type = ((type[type_pos] & _FP_WB_TLV_BASIC_TYPE_MASK) >>
                              _FP_WB_TLV_BASIC_TYPE_SHIFT);
            BCM_IF_ERROR_RETURN(tlv_read(unit, &tlv2, ptr, pos));
            switch (tlv2.type) {
                case _bcmFieldInternalControlUdfValid:
                     udf->valid = *(uint8 *)tlv2.value;
                     break;
                case _bcmFieldInternalControlUdfUseCount:
                     udf->use_count = *(int *)tlv2.value;
                     break;
                case _bcmFieldInternalControlUdfNum:
                     udf->udf_num = *(bcm_field_qualify_t *)tlv2.value;
                     break;
                case _bcmFieldInternalControlUdfUserNum:
                     udf->user_num = *(uint8 *)tlv2.value;
                     break;
                case _bcmFieldInternalControlEndStructUdf:
                     TLV_INIT(tlv2);
                     break;
                default:
                     LOG_ERROR(BSL_LS_BCM_FP,(BSL_META_U(unit,
                      "unknown type detected : _field_udf_recover\n")));
                     TLV_INIT(tlv2);
                     return BCM_E_INTERNAL;
            }
            type_pos++;
        }
        udf++;
    }
    return BCM_E_NONE;

}

/* Function:
 * _field_control_recover
 *
 * Purpose:
 *    recover _field_control_t structure
 *
 * Paramaters:
 * unit             - (IN) BCM device number
 * fc               - (IN) Pointer to device field control structure
 *
 * Returns:
 *     BCM_E_XXX
 */

int
_field_control_recover(int unit, _field_control_t *fc)
{

    _field_tlv_t tlv;              /* Field TLV structure */
    uint8 *scache_ptr = fc->scache_ptr[_FIELD_SCACHE_PART_0];
                                   /* Pointer to Scache */
    uint32 *position = &(fc->scache_pos);
                                   /* Position in Scache */
	tlv.value = NULL;
    TLV_INIT(tlv);

    while (tlv.type != _bcmFieldInternalEndStructControl) {
        TLV_INIT(tlv);
        BCM_IF_ERROR_RETURN(tlv_read(unit, &tlv, scache_ptr, position));

        switch (tlv.type)
        {
            case _bcmFieldInternalControlFlags:
                 fc->flags = *(uint8 *)tlv.value;
                 break;
            case _bcmFieldInternalControlUdfDetails:
                  BCM_IF_ERROR_RETURN(_field_udf_recover(unit, scache_ptr,
                                           position, fc->udf, &tlv));
                  break;

            case _bcmFieldInternalControlGroups:
                 break;
            case _bcmFieldInternalControlStages:
                 break;
            case _bcmFieldInternalControlPolicerCount:
                 fc->policer_count = *(uint32 *)tlv.value;
                 break;
            case _bcmFieldInternalControlStatCount:
                 fc->stat_count = *(uint32 *)tlv.value;
                 break;
            case _bcmFieldInternalControlHintBmp:
                 sal_memcpy(fc->hintid_bmp.w, tlv.value,
                      ((static_type_map[tlv.type].size) * tlv.length));
                 break;
            case _bcmFieldInternalControlLastAllocatedLtId:
                 fc->last_allocated_lt_eid = *(uint32 *)tlv.value;
                 break;
            case _bcmFieldInternalControlPolicerHash:
                 BCM_IF_ERROR_RETURN(_field_policer_recover(unit, &tlv));
                 break;
            case _bcmFieldInternalControlStathash:
                 BCM_IF_ERROR_RETURN(_field_stat_recover(unit, &tlv));
                 break;
            case _bcmFieldInternalControlHintHash:
                 BCM_IF_ERROR_RETURN(_field_hint_recover(unit, &tlv));
                 break;
            case _bcmFieldInternalEndStructControl:
                 if (*(uint32 *)tlv.value != _FIELD_WB_EM_CONTROL) {
                        LOG_ERROR(BSL_LS_BCM_FP,(BSL_META_U(unit,
                                       "END MARKER CHECK FAILED : CONTROL\n")));
                     return BCM_E_INTERNAL;
                 }
                 break;
            default:
                 LOG_ERROR(BSL_LS_BCM_FP,(BSL_META_U(unit,
                      "unknown type detected : _field_control_recover\n")));
                 TLV_INIT(tlv);
                 return BCM_E_INTERNAL;

        }
    }
    TLV_INIT(tlv);
    return BCM_E_NONE;
}



/* Function:
 * _field_meterpool_recover
 *
 * Purpose:
 *    recover _field_meter_pool_t structure
 *
 * Paramaters:
 * unit             - (IN) BCM device number
 * tlv              - (IN) TLV structure for field
 * stage_fc         - (OUT) Pointer to field stage structure
 *
 * Returns:
 *     BCM_E_XXX
 */

int
_field_meterpool_recover(int unit, _field_tlv_t *tlv, _field_stage_t *stage_fc)
{
    int i = 0;                            /* local variable for loops */
    int j = 0;
    uint32 *type;                         /* Elem type */
    int num_types = 0;                    /* Number of types */
    _field_tlv_t tlv2;                    /* TLV structure */
    uint8 *scache_ptr;                    /* Scahce pointer */
    uint32 *position;                     /* Scache position */
    _field_control_t        *fc;          /* Field control structure.*/
    int type_pos = 0;                     /* position of type */
    int num_i_instances = 0;              /* num_instances in 1st dimension */
    int num_j_instances = 0;              /* num_instances in 2nd dimension */

    WB_FIELD_CONTROL_GET(fc, scache_ptr, position);
	tlv2.value = NULL;
    TLV_INIT(tlv2);
    type = (uint32 *)tlv->value;
    num_i_instances = (tlv->length) & 0x000003ff;
    num_j_instances = ((tlv->length) & 0x003ffc00) >> 10;
    num_types     = ((tlv->length & _FP_WB_TLV_TYPES_MASK) >>
                    _FP_WB_TLV_TYPE_SHIFT);

    for (i = 0; i < num_i_instances; i++) {
        for (j = 0; j < num_j_instances; j++) {
             type_pos = 0;

             while ((tlv2.type != _bcmFieldInternalEndStructMeter)
                     && (type_pos != num_types)) {
                TLV_INIT(tlv2);
                tlv2.type = (type[type_pos] & _FP_WB_TLV_ELEM_TYPE_MASK);
                tlv2.basic_type = ((type[type_pos] & _FP_WB_TLV_BASIC_TYPE_MASK) >>
                                  _FP_WB_TLV_BASIC_TYPE_SHIFT);
                BCM_IF_ERROR_RETURN(tlv_read(unit, &tlv2, scache_ptr, position));
                switch (tlv2.type) {
                    case _bcmFieldInternalMeterLevel:
                         stage_fc->meter_pool[i][j]->level = *(int8 *)tlv2.value;
                         break;
                    case _bcmFieldInternalMeterSliceId:
                         stage_fc->meter_pool[i][j]->slice_id = *(int *)tlv2.value;
                         break;
                    case _bcmFieldInternalMeterSize:
                         stage_fc->meter_pool[i][j]->size = *(uint16 *)tlv2.value;
                         break;
                    case _bcmFieldInternalMeterPoolSize:
                         stage_fc->meter_pool[i][j]->pool_size =
                                                            *(uint16 *)tlv2.value;
                         break;
                    case _bcmFieldInternalMeterFreeMeters:
                         stage_fc->meter_pool[i][j]->free_meters =
                                                    *(uint16 *)tlv2.value;
                         break;
                    case _bcmFieldInternalMeterNumMeterPairs:
                         stage_fc->meter_pool[i][j]->num_meter_pairs =
                                                            *(uint16 *)tlv2.value;
                         break;
                    case _bcmFieldInternalMeterBmp:
                         sal_memcpy(stage_fc->meter_pool[i][j]->meter_bmp.w,
                                 tlv2.value, ((tlv2.length) *
                                     (static_type_map[tlv2.type].size)));
                         break;
                    case _bcmFieldInternalEndStructMeter:
                         if (*(uint32 *)tlv2.value !=  _FIELD_WB_EM_METER) {
                            LOG_ERROR(BSL_LS_BCM_FP,(BSL_META_U(unit,
                                    "END MARKER CHECK FAILED : METER(stage)\n")));
                             return BCM_E_INTERNAL;
                         }
                         break;
                    default:
                         LOG_ERROR(BSL_LS_BCM_FP,(BSL_META_U(unit,
                            "unknown type detected : Meter recover\n")));
                        return BCM_E_INTERNAL;
                }
             type_pos++;

             }
        TLV_INIT(tlv2);
        }
    }

    return BCM_E_NONE;
}
/* Function:
 * _field_extractor_recover
 *
 * Purpose:
 *    recover _field_slice_t structure
 *
 * Paramaters:
 * unit          - (IN) BCM device number
 * ptr           - (IN) Pointer to scache in field control
 * pos           - (IN) Pointer to position in field control
 * tlv           - (IN) tlv structure containing encoded types and number.
 * ext_sel       - (IN) address of start of array in slice structure.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
_field_extractor_recover(int unit, uint8 *ptr, uint32 *pos,
                   _field_tlv_t *tlv,  _field_ext_sel_t *ext_sel)
{

    int type_pos = 0;                     /* position of type */
    int i = 0;                            /* local variable for loops */
    int num_instances = 0;                /* number of instances */
    int num_types = 0;                    /* Number of types */
    _field_tlv_t tlv2;                    /* TLV structure */
    uint32 *type;                         /* Elem type */

    tlv2.value = NULL;
    TLV_INIT(tlv2);
    type = (uint32 *)tlv->value;
    num_instances = (tlv->length) & _FP_WB_TLV_LENGTH_MASK;
    num_types     = ((tlv->length & _FP_WB_TLV_TYPES_MASK) >>
                    _FP_WB_TLV_TYPE_SHIFT);

    for (i = 0; i < num_instances; i++) {
        type_pos = 0;
        TLV_INIT(tlv2);
        while  ((tlv2.type != _bcmFieldInternalEndStructExtractor)
                 && (type_pos != num_types)) {
            TLV_INIT(tlv2);
            tlv2.type = (type[type_pos] & _FP_WB_TLV_ELEM_TYPE_MASK);
            tlv2.basic_type = ((type[type_pos] & _FP_WB_TLV_BASIC_TYPE_MASK) >>
                              _FP_WB_TLV_BASIC_TYPE_SHIFT);
            BCM_IF_ERROR_RETURN(tlv_read(unit, &tlv2, ptr, pos));
            switch (tlv2.type) {
                case _bcmFieldInternalExtl1e32Sel:
                     sal_memcpy(ext_sel->l1_e32_sel, tlv2.value, tlv2.length);
                     break;
                case _bcmFieldInternalExtl1e16Sel:
                     sal_memcpy(ext_sel->l1_e16_sel, tlv2.value, tlv2.length);
                     break;
                case _bcmFieldInternalExtl1e8Sel:
                     sal_memcpy(ext_sel->l1_e8_sel, tlv2.value, tlv2.length);
                     break;
                case _bcmFieldInternalExtl1e4Sel:
                     sal_memcpy(ext_sel->l1_e4_sel, tlv2.value, tlv2.length);
                     break;
                case _bcmFieldInternalExtl1e2Sel:
                     sal_memcpy(ext_sel->l1_e2_sel, tlv2.value, tlv2.length);
                     break;
                case _bcmFieldInternalExtl2e16Sel:
                     sal_memcpy(ext_sel->l2_e16_sel, tlv2.value, tlv2.length);
                     break;
                case _bcmFieldInternalExtl3e1Sel:
                     sal_memcpy(ext_sel->l3_e1_sel, tlv2.value, tlv2.length);
                     break;
                case _bcmFieldInternalExtl3e2Sel:
                     sal_memcpy(ext_sel->l3_e2_sel, tlv2.value, tlv2.length);
                     break;
                case _bcmFieldInternalExtl3e4Sel:
                     sal_memcpy(ext_sel->l3_e4_sel, tlv2.value, tlv2.length);
                     break;
                case _bcmFieldInternalExtPmuxSel:
                    sal_memcpy(ext_sel->pmux_sel, tlv2.value, tlv2.length);
                    break;
                case _bcmFieldInternalExtIntraSlice:
                     ext_sel->intraslice = *(int8 *)tlv2.value;
                     break;
                case _bcmFieldInternalExtSecondary:
                     ext_sel->secondary = *(int8 *)tlv2.value;
                     break;
                case _bcmFieldInternalExtIpbmpRes:
                     ext_sel->ipbm_present = *(int8 *)tlv2.value;
                     break;
                case _bcmFieldInternalExtNorml3l4:
                     ext_sel->normalize_l3_l4_sel= *(int8 *)tlv2.value;
                     break;
                case _bcmFieldInternalExtNormmac:
                     ext_sel->normalize_mac_sel = *(int8 *)tlv2.value;
                     break;
                case _bcmFieldInternalExtAuxTagaSel:
                     ext_sel->aux_tag_a_sel = *(int8 *)tlv2.value;
                     break;
                case _bcmFieldInternalExtAuxTagbSel:
                    ext_sel->aux_tag_b_sel = *(int8 *)tlv2.value;
                    break;

                case _bcmFieldInternalExtAuxTagcSel:
                    ext_sel->aux_tag_c_sel = *(int8 *)tlv2.value;
                    break;
                case _bcmFieldInternalExtAuxTagdSel:
                    ext_sel->aux_tag_d_sel = *(int8 *)tlv2.value;
                    break;
                case _bcmFieldInternalExtTcpFnSel:
                    ext_sel->tcp_fn_sel = *(int8 *)tlv2.value;
                    break;
                case _bcmFieldInternalExtTosFnSel:
                     ext_sel->tos_fn_sel = *(int8 *)tlv2.value;
                    break;
                case _bcmFieldInternalExtTtlFnSel:
                     ext_sel->ttl_fn_sel = *(int8 *)tlv2.value;
                    break;
                case _bcmFieldInternalExtClassIdaSel:
                     ext_sel->class_id_cont_a_sel = *(int8 *)tlv2.value;
                     break;
                case _bcmFieldInternalExtClassIdbSel:
                     ext_sel->class_id_cont_b_sel = *(int8 *)tlv2.value;
                     break;
                case _bcmFieldInternalExtClassIdcSel:
                     ext_sel->class_id_cont_c_sel = *(int8 *)tlv2.value;
                     break;
                case _bcmFieldInternalExtClassIddSel:
                     ext_sel->class_id_cont_d_sel = *(int8 *)tlv2.value;
                     break;
                case _bcmFieldInternalExtSrcContaSel:
                     ext_sel->src_cont_a_sel = *(int8 *)tlv2.value;
                     break;
                case _bcmFieldInternalExtSrcContbSel:
                     ext_sel->src_cont_b_sel = *(int8 *)tlv2.value;
                     break;
                case _bcmFieldInternalExtSrcDestCont0Sel:
                     ext_sel->src_dest_cont_0_sel = *(int8 *)tlv2.value;
                     break;
                case _bcmFieldInternalExtSrcDestCont1Sel:
                     ext_sel->src_dest_cont_1_sel = *(int8 *)tlv2.value;
                     break;
                case _bcmFieldInternalExtKeygenIndex:
                    ext_sel->keygen_index = *(uint32 *)tlv2.value;
                case _bcmFieldInternalEndStructExtractor:
                    break;
                default:
                     TLV_INIT(tlv2);
                     LOG_ERROR(BSL_LS_BCM_FP,(BSL_META_U(unit,
                        "unknown type detected : extractor_recover\n")));
                     return BCM_E_INTERNAL;
            }
            type_pos++;
        }
        ext_sel++;
    }
    return BCM_E_NONE;

}
#define SLICE_STRUCT(_stage_, _ptr_, _pipenum_, _sliceid_)      \
        (_stage_->_ptr_[_pipenum_][_sliceid_])                  \

/* Function:
 * _field_cntrpool_recover
 *
 * Purpose:
 *    recover _field_cntr_pool_t structure
 *
 * Paramaters:
 * unit             - (IN) BCM device number
 * tlv              - (IN) TLV structure for field
 * stage_fc         - (OUT) Pointer to field stage structure
 *
 * Returns:
 *     BCM_E_XXX
 */

int
_field_cntrpool_recover(int unit, _field_tlv_t *tlv, _field_stage_t *stage_fc)
{
    int i = 0;                            /* local variable for loops */
    uint32 *type;                         /* Elem type */
    int num_types = 0;                    /* Number of types */
    _field_tlv_t tlv2;                    /* TLV structure */
    uint8 *scache_ptr;                    /* Scahce pointer */
    uint32 *position;                     /* Scache position */
    _field_control_t        *fc;          /* Field control structure.*/
    int type_pos = 0;                     /* position of type */
    int num_i_instances = 0;              /* number of instances */
    _field_cntr_pool_t *f_cp = NULL;      /* Field counter pool structure */


    WB_FIELD_CONTROL_GET(fc, scache_ptr, position);
	tlv2.value = NULL;
    TLV_INIT(tlv2);
    type = (uint32 *)tlv->value;
    num_i_instances = (tlv->length) & _FP_WB_TLV_LENGTH_MASK;
    num_types     = ((tlv->length & _FP_WB_TLV_TYPES_MASK) >>
                    _FP_WB_TLV_TYPE_SHIFT);

    for (i = 0; i < num_i_instances; i++) {
         f_cp = stage_fc->cntr_pool[i];
         type_pos = 0;
         while ((tlv2.type != _bcmFieldInternalEndStructCntr)
                 && (type_pos != num_types)) {
            TLV_INIT(tlv2);
            tlv2.type = (type[type_pos] & _FP_WB_TLV_ELEM_TYPE_MASK);
            tlv2.basic_type = ((type[type_pos] & _FP_WB_TLV_BASIC_TYPE_MASK) >>
                              _FP_WB_TLV_BASIC_TYPE_SHIFT);
            BCM_IF_ERROR_RETURN(tlv_read(unit, &tlv2, scache_ptr, position));
            switch (tlv2.type) {
                case _bcmFieldInternalCntrSliceId:
                     f_cp->slice_id = *(int8 *)tlv2.value;
                     break;
                case _bcmFieldInternalCntrSize:
                     f_cp->size = *(uint16 *)tlv2.value;
                     break;
                case _bcmFieldInternalCntrFreeCntrs:
                     f_cp->free_cntrs = *(uint16 *)tlv2.value;
                     break;
                case _bcmFieldInternalCntrBmp:
                     sal_memcpy(f_cp->cntr_bmp.w, tlv2.value,
                               ((tlv2.length)
                                 * (static_type_map[tlv2.type].size)));

                     break;
                case _bcmFieldInternalEndStructCntr:
                     if (*(uint32 *)tlv2.value != _FIELD_WB_EM_CNTR) {
                        LOG_ERROR(BSL_LS_BCM_FP,(BSL_META_U(unit,
                                       "END MARKER CHECK FAILED : COUNTER\n")));
                         return BCM_E_INTERNAL;
                     }
                     break;
                default:
                     LOG_ERROR(BSL_LS_BCM_FP,(BSL_META_U(unit,
                        "unknown type detected : _field_cntrpool_recover\n")));
                    return BCM_E_INTERNAL;
            }
        type_pos++;

        }
    TLV_INIT(tlv2);
    }

    return BCM_E_NONE;
}

/* Function:
 * _field_groupqualoffset_recover
 *
 * Purpose:
 *    recover _bcm_field_qual_offset_t structure
 *
 * Paramaters:
 * unit             - (IN) BCM device number
 * tlv              - (IN) TLV structure for field
 * offset_arr       - (OUT) Pointer to qualifier offset array structure
 *
 * Returns:
 *     BCM_E_XXX
 */

int
_field_groupqualoffset_recover(int unit, _field_tlv_t *tlv,
                               _bcm_field_qual_offset_t *offset_arr)
{
    int i = 0;                            /* local variable for loops */
    int num_instances = 0;                /* number of instances */
    uint32 *type;                         /* Elem type */
    int num_types = 0;                    /* Number of types */
    _field_tlv_t tlv2;                    /* TLV structure */
    uint8 *scache_ptr;                    /* Scahce pointer */
    uint32 *position;                     /* Scache position */
    _field_control_t        *fc;          /* Field control structure.*/
    int type_pos = 0;                     /* Position of type */

    WB_FIELD_CONTROL_GET(fc, scache_ptr, position);
    tlv2.value = NULL;
    TLV_INIT(tlv2);
    type = (uint32 *)tlv->value;
    num_instances = (tlv->length) & _FP_WB_TLV_LENGTH_MASK;
    num_types     = ((tlv->length & _FP_WB_TLV_TYPES_MASK) >>
                    _FP_WB_TLV_TYPE_SHIFT);

    for (i = 0; i < num_instances; i++) {
      type_pos = 0;
      while ((tlv2.type != _bcmFieldInternalEndStructQualOffset)
                 && (type_pos != num_types)) {
            TLV_INIT(tlv2);
            tlv2.type = (type[type_pos] & _FP_WB_TLV_ELEM_TYPE_MASK);
            tlv2.basic_type = ((type[type_pos] & _FP_WB_TLV_BASIC_TYPE_MASK) >>
                              _FP_WB_TLV_BASIC_TYPE_SHIFT);
            BCM_IF_ERROR_RETURN (tlv_read(unit, &tlv2, scache_ptr, position));
            switch (tlv2.type) {
                 case _bcmFieldInternalQualOffsetField:
                      offset_arr[i].field = * (soc_field_t *)tlv2.value;
                      break;
                 case _bcmFieldInternalQualOffsetNumOffset:
                      offset_arr[i].num_offsets = * (uint8 *)tlv2.value;
                      break;
                 case _bcmFieldInternalQualOffsetOffsetArr:
                      sal_memcpy(offset_arr[i].offset, tlv2.value,
                      ((static_type_map[tlv2.type].size) * tlv2.length));
                      break;
                 case _bcmFieldInternalQualOffsetWidth:
                      sal_memcpy(offset_arr[i].width, tlv2.value,
                      ((static_type_map[tlv2.type].size) * tlv2.length));
                      break;
                 case _bcmFieldInternalQualOffsetSec:
                      offset_arr[i].secondary = * (uint8 *)tlv2.value;
                      break;
                 case _bcmFieldInternalQualOffsetBitPos:
                      offset_arr[i].bit_pos = * (uint8 *)tlv2.value;
                      break;
                 case _bcmFieldInternalQualOffsetQualWidth:
                      offset_arr[i].qual_width = * (uint8 *)tlv2.value;
                      break;
                 case _bcmFieldInternalEndStructQualOffset:
                      if (*(uint32 *)tlv2.value
                          != _FIELD_WB_EM_QUALOFFSET) {
                        LOG_ERROR(BSL_LS_BCM_FP,(BSL_META_U(unit,
                                  "END MARKER CHECK FAILED : "
                                  "QUALOFFSET\n")));
                          return BCM_E_INTERNAL;
                      }
                      break;
                 default:
                     LOG_ERROR(BSL_LS_BCM_FP,(BSL_META_U(unit,
                                "unknown type detected :"
                                " _groupqualoffset_recover\n")));
                     return BCM_E_INTERNAL;

            }
        type_pos++;

        }
        TLV_INIT(tlv2);

    }
    return BCM_E_NONE;

}


/* Function:
 * _field_groupqual_recover
 *
 * Purpose:
 *    recover _bcm_field_group_qual_t structure
 *
 * Paramaters:
 * unit             - (IN) BCM device number
 * tlv              - (IN) TLV structure for field
 * fg               - (OUT) Pointer to field group structure
 *
 * Returns:
 *     BCM_E_XXX
 */

int
_field_groupqual_recover(int unit, _field_tlv_t *tlv, _field_group_t *fg)
{
    int j = 0;
    _field_tlv_t tlv2;                    /* TLV structure */
    uint8 *scache_ptr;                    /* Scahce pointer */
    uint32 *position;                     /* Scache position */
    _field_control_t        *fc;          /* Field control structure.*/
    int num_instances = 0;                /* Number of instances */
    int rv = 0;                           /* Return Variable */
     uint8 entry_type;                    /* Index to tcam entries of
                                              multiple types            */
    WB_FIELD_CONTROL_GET(fc, scache_ptr, position);
    tlv2.value = NULL;
    TLV_INIT(tlv2);
    num_instances = (tlv->length);

    entry_type = _FP_ENTRY_TYPE_DEFAULT;

    for (j = 0; j < num_instances; j++) {

         while ((tlv2.type != _bcmFieldInternalEndStructGroupQual)) {
            TLV_INIT(tlv2);
            rv = tlv_read(unit, &tlv2, scache_ptr, position);
            BCM_IF_ERROR_CLEANUP(rv);

            switch (tlv2.type) {
                 case _bcmFieldInternalQualQid:
                      _FP_XGS3_ALLOC(fg->qual_arr[entry_type][j].qid_arr,
                                     ((static_type_map[tlv2.type].size)
                                     * tlv2.length) , "FP Qual Array");
                      if (fg->qual_arr[entry_type][j].qid_arr == NULL) {
                          return BCM_E_MEMORY;
                      }
                      sal_memcpy(fg->qual_arr[entry_type][j].qid_arr,
                                 tlv2.value,
                                 ((static_type_map[tlv2.type].size)
                                  * tlv2.length));
                      break;
                 case _bcmFieldInternalQualSize:
                      fg->qual_arr[entry_type][j].size = * (uint16*)tlv2.value;
                      break;
                 case _bcmFieldInternalQualOffset:
                       _FP_XGS3_ALLOC(fg->qual_arr[entry_type][j].offset_arr,
                                      (sizeof(_bcm_field_qual_offset_t)
                                        *(fg->qual_arr[entry_type][j].size)),
                                     "FP Qual Array");

                      if (fg->qual_arr[entry_type][j].offset_arr == NULL) {
                          return BCM_E_MEMORY;
                      }
                      BCM_IF_ERROR_RETURN(
                           _field_groupqualoffset_recover(unit,
                                   &tlv2,
                                   fg->qual_arr[entry_type][j].offset_arr));
                      break;
                 case _bcmFieldInternalEndStructGroupQual:
                      if (*(uint32 *)tlv2.value != _FIELD_WB_EM_QUAL) {
                        LOG_ERROR(BSL_LS_BCM_FP,(BSL_META_U(unit,
                                        "END MARKER CHECK FAILED : QUAL\n")));
                          return BCM_E_INTERNAL;
                      }
                      break;
                 default:
                     LOG_ERROR(BSL_LS_BCM_FP,(BSL_META_U(unit,
                        "unknown type detected : _groupqual_recover\n")));
                      rv = BCM_E_INTERNAL;
                      goto cleanup;
            }
        }
    TLV_INIT(tlv2);
    }

    return BCM_E_NONE;

cleanup:
    TLV_INIT(tlv2);
    if (fg->qual_arr[entry_type][j].qid_arr != NULL) {
        sal_free(fg->qual_arr[entry_type][j].qid_arr);
        fg->qual_arr[entry_type][j].qid_arr = NULL;
    }
    if (fg->qual_arr[entry_type][j].offset_arr != NULL) {
        sal_free(fg->qual_arr[entry_type][j].offset_arr);
        fg->qual_arr[entry_type][j].offset_arr = NULL;
    }

    return rv;

}

/* Function:
 * _field_ltinfo_recover
 *
 * Purpose:
 *    recover _field_lt_config_t structure
 *
 * Paramaters:
 * unit             - (IN) BCM device number
 * tlv              - (IN) TLV structure for field
 * stage_fc         - (OUT) Pointer to field stage structure
 *
 * Returns:
 *     BCM_E_XXX
 */

int
_field_ltinfo_recover(int unit, _field_tlv_t *tlv,
            _field_stage_t *stage_fc, _field_group_t *fg)
{
    int i = 0;                            /* local variable for loops */
    int j = 0;
    uint32 *type;                         /* Elem type */
    int num_types = 0;                    /* Number of types */
    _field_tlv_t tlv2;                    /* TLV structure */
    uint8 *scache_ptr;                    /* Scahce pointer */
    uint32 *position;                     /* Scache position */
    _field_control_t        *fc;          /* Field control structure.*/
    int type_pos = 0;                     /* position of type */
    int num_i_instances = 0;              /* num_instances in 1st dimension */
    int num_j_instances = 0;              /* num_instances in 2nd dimension */
    _field_lt_config_t *f_cfg = NULL;     /* Field config for LT */


	tlv2.value = NULL;
    TLV_INIT(tlv2);
    WB_FIELD_CONTROL_GET(fc, scache_ptr, position);
    type = (uint32 *)tlv->value;
    num_i_instances = (tlv->length) & 0x000003ff;
    num_j_instances = ((tlv->length) & 0x003ffc00) >> 10;
    num_types     = ((tlv->length & _FP_WB_TLV_TYPES_MASK) >>
                    _FP_WB_TLV_TYPE_SHIFT);

    if ((fg == NULL) && (stage_fc == NULL)) {
        return BCM_E_PARAM;
    }
    if (fg != NULL) {
        _FP_XGS3_ALLOC(f_cfg, sizeof(_field_lt_config_t), "group's lt config");
        fg->lt_info = f_cfg;
    }

    for (i = 0; i < num_i_instances; i++) {
    for (j = 0; j < num_j_instances; j++) {

    if (stage_fc != NULL) {
        f_cfg = stage_fc->lt_info[i][j];
    }
    type_pos = 0;

         while ((tlv2.type != _bcmFieldInternalEndStructLtConfig)
                 && (type_pos != num_types)) {
            TLV_INIT(tlv2);
            tlv2.type = (type[type_pos] & _FP_WB_TLV_ELEM_TYPE_MASK);
            tlv2.basic_type = ((type[type_pos] & _FP_WB_TLV_BASIC_TYPE_MASK) >>
                              _FP_WB_TLV_BASIC_TYPE_SHIFT);
            BCM_IF_ERROR_RETURN(tlv_read(unit, &tlv2, scache_ptr, position));
            switch (tlv2.type) {
                 case _bcmFieldInternalLtConfigValid:
                      f_cfg->valid = *(int *) tlv2.value;
                      break;
                 case _bcmFieldInternalLtConfigLtId:
                      f_cfg->lt_id = *(int *) tlv2.value;
                      break;
                 case _bcmFieldInternalLtConfigLtPartPri:
                      sal_memcpy(f_cfg->lt_part_pri, tlv2.value,
                                ((tlv2.length)
                                 * (static_type_map[tlv2.type].size)));
                      break;
                 case _bcmFieldInternalLtConfigLtPartMap:
                      f_cfg->lt_part_map = *(uint32 *)tlv2.value;
                      break;
                 case _bcmFieldInternalLtConfigLtActionPri:
                      f_cfg->lt_action_pri = *(int *) tlv2.value;
                      break;
                 case _bcmFieldInternalLtConfigPri:
                      f_cfg->priority = *(int *) tlv2.value;
                      break;
                 case _bcmFieldInternalLtConfigFlags:
                      f_cfg->flags = *(int *) tlv2.value;
                      break;
                 case _bcmFieldInternalLtConfigEntry:
                      f_cfg->lt_entry = *(bcm_field_entry_t *) tlv2.value;
                      break;
                 case _bcmFieldInternalEndStructLtConfig:
                      if (*(uint32 *)tlv2.value != _FIELD_WB_EM_LTCONF) {
                        LOG_ERROR(BSL_LS_BCM_FP,(BSL_META_U(unit,
                                        "END MARKER CHECK FAILED : LTCONF\n")));
                          return BCM_E_INTERNAL;
                      }
                      break;
                 default:
                     LOG_ERROR(BSL_LS_BCM_FP,(BSL_META_U(unit,
                        "unknown type detected : _lt_entry_info_recover\n")));
                    return BCM_E_INTERNAL;
            }
        type_pos++;

        }
    TLV_INIT(tlv2);
    }
    }

    return BCM_E_NONE;
}

/* Function:
 * _field_range_recover
 *
 * Purpose:
 *    recover _field_range_t structure
 *
 * Paramaters:
 * unit             - (IN) BCM device number
 * tlv              - (IN) TLV structure for field
 * stage_fc         - (OUT) Pointer to field stage structure
 *
 * Returns:
 *     BCM_E_XXX
 */

int
_field_range_recover(int unit, _field_tlv_t *tlv, _field_stage_t *stage_fc)
{
    int i = 0;                            /* local variable for loops */
    int num_instances = 0;                /* number of instances */
    uint32 *type;                         /* Elem type */
    int num_types = 0;                    /* Number of types */
    _field_tlv_t tlv2;                    /* TLV structure */
    uint8 *scache_ptr;                    /* Scahce pointer */
    uint32 *position;                     /* Scache position */
    _field_control_t        *fc;          /* Field control structure.*/
    _field_range_t *new_fr = NULL;        /* New field range structure */
    _field_range_t *prev_fr = NULL;       /* Prev field range structure */
    int type_pos = 0;                     /* Positon of type */
    int rv = 0;                           /* Return variable */

    WB_FIELD_CONTROL_GET(fc, scache_ptr, position);
	tlv2.value = NULL;
    TLV_INIT(tlv2);
    type = (uint32 *)tlv->value;
    num_instances = (tlv->length) & _FP_WB_TLV_LENGTH_MASK;
    num_types     = ((tlv->length & _FP_WB_TLV_TYPES_MASK) >>
                    _FP_WB_TLV_TYPE_SHIFT);

    for (i = 0; i < num_instances; i++) {
      _FP_XGS3_ALLOC(new_fr, sizeof(_field_range_t), "FP Range");
      if (NULL == new_fr) {
          return BCM_E_MEMORY;
      }
      type_pos = 0;
      while ((tlv2.type != _bcmFieldInternalEndStructRanges)
                 && (type_pos != num_types)) {
            TLV_INIT(tlv2);
            tlv2.type = (type[type_pos] & _FP_WB_TLV_ELEM_TYPE_MASK);
            tlv2.basic_type = ((type[type_pos] & _FP_WB_TLV_BASIC_TYPE_MASK) >>
                              _FP_WB_TLV_BASIC_TYPE_SHIFT);
            rv = tlv_read(unit, &tlv2, scache_ptr, position);
            BCM_IF_ERROR_CLEANUP(rv);
            switch (tlv2.type) {
                case _bcmFieldInternalRangeFlags:
                     new_fr->flags = *(uint32 *)tlv2.value;
                     break;
                case _bcmFieldInternalRangeRid:
                     new_fr->rid = *(bcm_field_range_t *)tlv2.value;
                     break;
                case _bcmFieldInternalRangeMin:
                     new_fr->min = *(bcm_l4_port_t *)tlv2.value;
                     break;
                case _bcmFieldInternalRangeMax:
                     new_fr->max = *(bcm_l4_port_t *)tlv2.value;
                     break;
                case _bcmFieldInternalRangeHwIndex:
                     new_fr->hw_index = *(int *)tlv2.value;
                     break;
                case _bcmFieldInternalRangeStyle:
                     new_fr->style = *(uint8 *)tlv2.value;
                     break;
                case _bcmFieldInternalEndStructRanges:
                     if (*(uint32 *)tlv2.value != _FIELD_WB_EM_RANGE) {
                        LOG_ERROR(BSL_LS_BCM_FP,(BSL_META_U(unit,
                                        "END MARKER CHECK FAILED : RANGE\n")));
                         return BCM_E_INTERNAL;
                     }
                     break;
                default:
                     LOG_ERROR(BSL_LS_BCM_FP,(BSL_META_U(unit,
                        "unknown type detected : _field_range_recover\n")));
                    rv = BCM_E_INTERNAL;
                    goto cleanup;

            }
        type_pos++;

        }
        new_fr->next = prev_fr;
        prev_fr = new_fr;
        new_fr = NULL;
        TLV_INIT(tlv2);

    }
    stage_fc->ranges = prev_fr;
    return BCM_E_NONE;

cleanup:
    TLV_INIT(tlv2);
    if (new_fr != NULL) {
        sal_free(new_fr);
        new_fr = NULL;
    }
    return rv;
}

/* Function:
 * _field_datacontroldataqualifier_recover
 *
 * Purpose:
 *    recover _field_data_qualifier_t structure
 *
 * Paramaters:
 * unit             - (IN) BCM device number
 * tlv              - (IN) TLV structure for field
 * fd               - (IN) Field data control structure
 * Returns:
 *     BCM_E_XXX
 */

int
_field_datacontroldataqualifier_recover(int unit, _field_tlv_t *tlv,
                                        _field_data_control_t *fd)
{
    int i = 0;                            /* local variable for loops */
    int num_instances = 0;                /* number of instances */
    _field_tlv_t tlv2;                    /* TLV structure */
    uint8 *scache_ptr;                    /* Scahce pointer */
    uint32 *position;                     /* Scache position */
    _field_control_t        *fc;          /* Field control structure.*/
    _field_data_qualifier_p new_f_dq = NULL;   /* New qualifier */
    _field_data_qualifier_p prev_f_dq = NULL;  /* Prev qualifier */
    int rv = 0;                           /* Return variable */

    WB_FIELD_CONTROL_GET(fc, scache_ptr, position);
	tlv2.value = NULL;
    TLV_INIT(tlv2);
    num_instances = (tlv->length);

    for (i = 0; i < num_instances; i++) {

      _FP_XGS3_ALLOC(new_f_dq, sizeof(_field_data_qualifier_t),
                   "FP data qualifier");
      if (NULL == new_f_dq) {
          return BCM_E_MEMORY;
      }

      _FP_XGS3_ALLOC(new_f_dq->spec, sizeof(bcm_field_udf_spec_t),
                   " FP data qualifier spec ");
      if (NULL == new_f_dq->spec) {
          sal_free(new_f_dq);
          new_f_dq = NULL;
          return BCM_E_MEMORY;
      }


      while (tlv2.type != _bcmFieldInternalDataControlEndStructDataQual) {
            TLV_INIT(tlv2);
            rv = tlv_read(unit, &tlv2, scache_ptr, position);
            if (rv < 0 ) {
                goto cleanup;
            }

            switch (tlv2.type) {
                case _bcmFieldInternalDataControlDataQualQid:
                     new_f_dq->qid = *(int *)tlv2.value;
                     break;
                case _bcmFieldInternalDataControlDataQualUdfSpec:
                     sal_memcpy(new_f_dq->spec->offset,tlv2.value,tlv2.length);
                     break;
                case _bcmFieldInternalDataControlDataQualOffsetBase:
                     new_f_dq->offset_base = * (bcm_field_data_offset_base_t *)
                                                tlv2.value;
                     break;
                case _bcmFieldInternalDataControlDataQualOffset:
                     new_f_dq->offset = * (int *)tlv2.value;
                     break;
                case _bcmFieldInternalDataControlDataQualByteOffset:
                     new_f_dq->byte_offset = * (uint8 *)tlv2.value;
                     break;
                case _bcmFieldInternalDataControlDataQualHwBmp:
                     new_f_dq->hw_bmap = * (uint32 *)tlv2.value;
                     break;
                case _bcmFieldInternalDataControlDataQualFlags:
                     new_f_dq->flags = * (uint32 *)tlv2.value;
                     break;
                case _bcmFieldInternalDataControlDataQualElemCount:
                     new_f_dq->elem_count = * (uint32 *)tlv2.value;
                     break;
                case _bcmFieldInternalDataControlDataQualLength:
                     new_f_dq->length = * (int *)tlv2.value;
                     break;
                case _bcmFieldInternalDataControlEndStructDataQual:
                     if (*(int *)tlv2.value != _FIELD_WB_EM_DATAQUAL) {
                        LOG_ERROR(BSL_LS_BCM_FP,(BSL_META_U(unit,
                                     "END MARKER CHECK FAILED : DATAQUAL\n")));
                         return BCM_E_INTERNAL;
                     }
                     break;
                default:
                     LOG_ERROR(BSL_LS_BCM_FP,(BSL_META_U(unit,
                        "unknown type detected : _dataqual_recover\n")));
                     rv = BCM_E_INTERNAL;
                     goto  cleanup;
            }
        }
        new_f_dq->next = prev_f_dq;
        prev_f_dq = new_f_dq;
        new_f_dq = NULL;
        TLV_INIT(tlv2);

    }
    fd->data_qual = new_f_dq;
    return BCM_E_NONE;

cleanup:
    TLV_INIT(tlv2);
    if (new_f_dq != NULL) {
        if (new_f_dq->spec != NULL) {
            sal_free(new_f_dq->spec);
            new_f_dq->spec = NULL;
        }
        sal_free(new_f_dq);
        new_f_dq = NULL;
    }
    return rv;

}

/* Function:
 * _field_datacontrolprot_recover
 *
 * Purpose:
 *    recover _field_data_protocol_t structure
 *
 * Paramaters:
 * unit             - (IN) BCM device number
 * tlv              - (IN) TLV structure for field
 * fd               - (IN) Field data control structure
 * Returns:
 *     BCM_E_XXX
 */

int
_field_datacontrolprot_recover(int unit, _field_tlv_t *tlv,
                               _field_data_control_t *fd)
{
    int i = 0;                            /* local variable for loops */
    int num_instances = 0;                /* number of instances */
    uint32 *type;                         /* Elem type */
    int num_types = 0;                    /* Number of types */
    _field_tlv_t tlv2;                    /* TLV structure */
    uint8 *scache_ptr;                    /* Scahce pointer */
    uint32 *position;                     /* Scache position */
    _field_control_t        *fc;          /* Field control structure.*/
    int type_pos = 0;                     /* Positon of type */

    WB_FIELD_CONTROL_GET(fc, scache_ptr, position);
	tlv2.value = NULL;
    TLV_INIT(tlv2);
    type = (uint32 *)tlv->value;
    num_instances = (tlv->length) & _FP_WB_TLV_LENGTH_MASK;
    num_types     = ((tlv->length & _FP_WB_TLV_TYPES_MASK) >>
                    _FP_WB_TLV_TYPE_SHIFT);

    for (i = 0; i < num_instances; i++) {
      type_pos = 0;
      while ((tlv2.type != _bcmFieldInternalDataControlEndStructProt)
                 && (type_pos != num_types)) {
            TLV_INIT(tlv2);
            tlv2.type = (type[type_pos] & _FP_WB_TLV_ELEM_TYPE_MASK);
            tlv2.basic_type = ((type[type_pos] & _FP_WB_TLV_BASIC_TYPE_MASK) >>
                              _FP_WB_TLV_BASIC_TYPE_SHIFT);
            BCM_IF_ERROR_RETURN(tlv_read(unit, &tlv2, scache_ptr, position));
            switch (tlv2.type) {
               case _bcmFieldInternalDataControlProtIp4RefCount:
                    fd->ip[i].ip4_ref_count = *(int *)tlv2.value;
                    break;
               case _bcmFieldInternalDataControlProtIp6RefCount:
                    fd->ip[i].ip6_ref_count = *(int *)tlv2.value;
                    break;
               case _bcmFieldInternalDataControlProtFlags:
                    fd->ip[i].flags = *(uint32 *)tlv2.value;
                    break;
               case _bcmFieldInternalDataControlProtIp:
                    fd->ip[i].ip = *(uint8 *)tlv2.value;
                    break;
               case _bcmFieldInternalDataControlProtL2:
                    fd->ip[i].l2 = *(uint16 *)tlv2.value;
                    break;
               case _bcmFieldInternalDataControlProtVlanTag:
                    fd->ip[i].vlan_tag = *(uint16 *)tlv2.value;
                    break;
               case _bcmFieldInternalDataControlProtRelOffset:
                    fd->ip[i].relative_offset = *(int *)tlv2.value;
                    break;
               case _bcmFieldInternalDataControlEndStructProt:
                    if (*(uint32 *)tlv2.value != _FIELD_WB_EM_DATAPROT) {
                        LOG_ERROR(BSL_LS_BCM_FP,(BSL_META_U(unit,
                                    "END MARKER CHECK FAILED : DATAPROT\n")));
                        return BCM_E_INTERNAL;
                    }
                    break;
               default:
                    LOG_ERROR(BSL_LS_BCM_FP,(BSL_META_U(unit,
                        "unknown type detected : _datacontrolprot_recover\n")));
                    return BCM_E_INTERNAL;

            }
        type_pos++;

        }
        TLV_INIT(tlv2);
    }
    return BCM_E_NONE;
}


/* Function:
 * _field_datacontrolethertype_recover
 *
 * Purpose:
 *    recover _field_data_ethertype_t structure
 *
 * Paramaters:
 * unit             - (IN) BCM device number
 * tlv              - (IN) TLV structure for field
 * fd               - (IN) Field data control structure
 * Returns:
 *     BCM_E_XXX
 */

int
_field_datacontrolethertype_recover(int unit, _field_tlv_t *tlv,
                                    _field_data_control_t *fd)
{
    int i = 0;                            /* local variable for loops */
    int num_instances = 0;                /* number of instances */
    uint32 *type;                         /* Elem type */
    int num_types = 0;                    /* Number of types */
    _field_tlv_t tlv2;                    /* TLV structure */
    uint8 *scache_ptr;                    /* Scahce pointer */
    uint32 *position;                     /* Scache position */
    _field_control_t        *fc;          /* Field control structure.*/
    int type_pos = 0;                     /* Positon of type */

    WB_FIELD_CONTROL_GET(fc, scache_ptr, position);
	tlv2.value = NULL;
    TLV_INIT(tlv2);
    type = (uint32 *)tlv->value;
    num_instances = (tlv->length) & _FP_WB_TLV_LENGTH_MASK;
    num_types     = ((tlv->length & _FP_WB_TLV_TYPES_MASK) >>
                    _FP_WB_TLV_TYPE_SHIFT);

    for (i = 0; i < num_instances; i++) {
      type_pos = 0;
      while ((tlv2.type != _bcmFieldInternalDataControlEndStructEtype)
                 && (type_pos != num_types)) {
            TLV_INIT(tlv2);
            tlv2.type = (type[type_pos] & _FP_WB_TLV_ELEM_TYPE_MASK);
            tlv2.basic_type = ((type[type_pos] & _FP_WB_TLV_BASIC_TYPE_MASK) >>
                              _FP_WB_TLV_BASIC_TYPE_SHIFT);
            BCM_IF_ERROR_RETURN(tlv_read(unit, &tlv2, scache_ptr, position));
            switch (tlv2.type) {
              case _bcmFieldInternalDataControlEthertypeRefCount:
                    fd->etype[i].ref_count = * (int *)tlv2.value;
                    break;

               case _bcmFieldInternalDataControlEthertypeL2:
                    fd->etype[i].l2 = * (uint16 *)tlv2.value;
                    break;

               case _bcmFieldInternalDataControlEthertypeVlanTag:
                    fd->etype[i].vlan_tag = * (uint16 *)tlv2.value;
                    break;

               case _bcmFieldInternalDataControlEthertypePortEt:
                    fd->etype[i].ethertype = * (bcm_port_ethertype_t *)
                                                tlv2.value;
                    break;

               case _bcmFieldInternalDataControlEthertypeRelOffset:
                    fd->etype[i].relative_offset = * (int *)tlv2.value;
                    break;

               case _bcmFieldInternalDataControlEndStructEtype:
                    if (*(uint32 *)tlv2.value != _FIELD_WB_EM_DATAETYPE) {
                        LOG_ERROR(BSL_LS_BCM_FP,(BSL_META_U(unit,
                                    "END MARKER CHECK FAILED : DATATYPE\n")));
                        return BCM_E_INTERNAL;
                    }
                    break;
               default:
                     LOG_ERROR(BSL_LS_BCM_FP,(BSL_META_U(unit,
                  "unknown type detected : _datacontrol_ethertype_recover\n")));
                    return BCM_E_INTERNAL;

            }
        type_pos++;

        }
        TLV_INIT(tlv2);
    }
    return BCM_E_NONE;
}

/* Function:
 * _field_datacontroltcamentry_recover
 *
 * Purpose:
 *    recover _field_data_tcam_entry_t structure
 *
 * Paramaters:
 * unit             - (IN) BCM device number
 * tlv              - (IN) TLV structure for field
 * fd               - (IN) Field data control structure
 * Returns:
 *     BCM_E_XXX
 */

int
_field_datacontroltcamentry_recover(int unit, _field_tlv_t *tlv,
                                        _field_data_control_t *fd)
{
    int i = 0;                            /* local variable for loops */
    int num_instances = 0;                /* number of instances */
    uint32 *type;                         /* Elem type */
    _field_tlv_t tlv2;                    /* TLV structure */
    uint8 *scache_ptr;                    /* Scahce pointer */
    uint32 *position;                     /* Scache position */
    _field_control_t        *fc;          /* Field control structure.*/
    _field_data_tcam_entry_t *fent = NULL; /* Field tcam entry */
    int type_pos = 0;                     /* Positon of type */
    int num_types = 0;                    /* Number of types */

    WB_FIELD_CONTROL_GET(fc, scache_ptr, position);
	tlv2.value = NULL;
    TLV_INIT(tlv2);
    type = (uint32 *)tlv->value;
    num_instances = (tlv->length) & _FP_WB_TLV_LENGTH_MASK;
    num_types     = ((tlv->length & _FP_WB_TLV_TYPES_MASK) >>
                    _FP_WB_TLV_TYPE_SHIFT);

    fent = fd->tcam_entry_arr;
    if (fent == NULL) {
        /* data control not initilaized */
        return BCM_E_INIT;
    }

    for (i = 0; i < num_instances; i++) {
      type_pos = 0;
      while ((tlv2.type != _bcmFieldInternalDataControlEndStructTcam)
               && (type_pos != num_types)) {
            TLV_INIT(tlv2);
            tlv2.type = (type[type_pos] & _FP_WB_TLV_ELEM_TYPE_MASK);
            tlv2.basic_type = ((type[type_pos] & _FP_WB_TLV_BASIC_TYPE_MASK) >>
                              _FP_WB_TLV_BASIC_TYPE_SHIFT);

            BCM_IF_ERROR_RETURN(tlv_read(unit, &tlv2, scache_ptr, position));
            switch (tlv2.type) {
               case _bcmFieldInternalDataControlTcamRefCount:
                    fent->ref_count = * (uint8 *)tlv2.value;
                    break;
               case _bcmFieldInternalDataControlTcamPriority:
                    fent->priority = * (uint8 *)tlv2.value;
                    break;
               case _bcmFieldInternalDataControlEndStructTcam:
                    if (*(uint32 *)tlv2.value != _FIELD_WB_EM_DATATCAM) {
                        LOG_ERROR(BSL_LS_BCM_FP,(BSL_META_U(unit,
                                    "END MARKER CHECK FAILED : DATATCAM\n")));
                        return BCM_E_INTERNAL;
                    }
                    break;
               default:
                     LOG_ERROR(BSL_LS_BCM_FP,(BSL_META_U(unit,
                   "unknown type detected : _datacontroltcamentry_recover\n")));
                     return BCM_E_INTERNAL;
            }
        type_pos++;
        }
        fent++;
        TLV_INIT(tlv2);
    }
    return BCM_E_NONE;
}


/* Function:
 * _field_data_control_recover
 *
 * Purpose:
 *    recover _field_data_control_t structure
 *
 * Paramaters:
 * unit             - (IN) BCM device number
 * tlv              - (IN) TLV structure for field
 * stage_fc         - (OUT) Pointer to field stage structure
 *
 * Returns:
 *     BCM_E_XXX
 */

int
_field_data_control_recover(int unit, _field_tlv_t *tlv,
                            _field_stage_t *stage_fc)
{
    _field_tlv_t tlv2;             /* Field TLV structure */
    uint8 *scache_ptr = NULL;        /* Pointer to Scache */
    uint32 *position = NULL;         /* Position in Scache */
    _field_control_t  *fc;           /* Field control structure.*/
    _field_data_control_t *fd = NULL;/* Field Data control structure */

    WB_FIELD_CONTROL_GET(fc, scache_ptr, position);
    tlv2.value = NULL;              /* Initialize value */
    TLV_INIT(tlv2);
    fd = stage_fc->data_ctrl;


    while (tlv2.type != _bcmFieldInternalEndStructDataControl) {
        TLV_INIT(tlv2);
        BCM_IF_ERROR_RETURN(tlv_read(unit, &tlv2, scache_ptr, position));

        switch (tlv2.type)
        {
             case _bcmFieldInternalDataControlUsageBmp:
                  fd->usage_bmap = * (uint32 *)tlv2.value;
                  break;
             case _bcmFieldInternalDataControlDataQualStruct:
                  BCM_IF_ERROR_RETURN(
                       _field_datacontroldataqualifier_recover(unit,
                                                               &tlv2,
                                                               fd));
                  break;
             case _bcmFieldInternalDataControlElemSize:
                  fd->elem_size = *(int *)tlv2.value;
                  break;
             case _bcmFieldInternalDataControlNumElem:
                  fd->num_elems = *(int *)tlv2.value;
                  break;
             case _bcmFieldInternalDataControlEthertypeStruct:
                  BCM_IF_ERROR_RETURN(_field_datacontrolethertype_recover(unit,
                                                                 &tlv2,
                                                                 fd));
                  break;
             case _bcmFieldInternalDataControlProtStart:
                  BCM_IF_ERROR_RETURN(_field_datacontrolprot_recover(unit,
                                                                 &tlv2,
                                                                 fd));
                  break;
             case _bcmFieldInternalDataControlTcamStruct:
                  BCM_IF_ERROR_RETURN(_field_datacontroltcamentry_recover(unit,
                                                                 &tlv2,
                                                                 fd));

                  break;
             case _bcmFieldInternalEndStructDataControl:
                  if (*(uint32 *)tlv2.value != _FIELD_WB_EM_DATACONTROL) {
                        LOG_ERROR(BSL_LS_BCM_FP,(BSL_META_U(unit,
                                  "END MARKER CHECK FAILED : DATACONTROL\n")));
                      return BCM_E_INTERNAL;
                  }
                  break;
             default:
                     LOG_ERROR(BSL_LS_BCM_FP,(BSL_META_U(unit,
                     "unknown type detected : _datacontrol_recover\n")));
                  return BCM_E_INTERNAL;
        }
    }
    TLV_INIT(tlv2);

    return BCM_E_NONE;
}


/* Function:
 * _field_stage_recover
 *
 * Purpose:
 *    recover _field_stage_t structure
 *
 * Paramaters:
 * unit             - (IN) BCM device number
 * stage_fc         - (IN) Pointer to device stage structure
 *
 * Returns:
 *     BCM_E_XXX
 */

int
_field_stage_recover(int unit, _field_stage_t **stage_fc)
{
    _field_tlv_t tlv;              /* Field TLV structure */
    uint8 *scache_ptr = NULL;      /* Pointer to Scache */
    uint32 *position = NULL;       /* Position in Scache */
    _field_control_t  *fc;         /* Field control structure.*/

    WB_FIELD_CONTROL_GET(fc, scache_ptr, position);
    tlv.value = NULL;              /* Initialize value */
    TLV_INIT(tlv);


    while (tlv.type != _bcmFieldInternalEndStructStage) {
        TLV_INIT(tlv);
        BCM_IF_ERROR_RETURN(tlv_read(unit, &tlv, scache_ptr, position));

        switch (tlv.type)
        {
            case _bcmFieldInternalStageStageid:
                 (*stage_fc)->stage_id = *(_field_stage_id_t *)tlv.value;
                 break;
            case _bcmFieldInternalStageFlags:
                 (*stage_fc)->flags = *(int *)tlv.value;
                 break;
            case _bcmFieldInternalStageTcamSz:
                 (*stage_fc)->tcam_sz = *(int *)tlv.value;
                 break;
            case _bcmFieldInternalStageTcamSlices:
                 (*stage_fc)->tcam_slices = *(int *)tlv.value;
                 break;
            case _bcmFieldInternalStageNumInstances:
                 (*stage_fc)->num_instances = *(int *)tlv.value;
                 break;
            case _bcmFieldInternalStageNumPipes:
                 (*stage_fc)->num_pipes = *(int *)tlv.value;
                 break;
            case _bcmFieldInternalStageRangeId:
                 (*stage_fc)->range_id = *(uint32 *)tlv.value;
                 break;
            case _bcmFieldInternalStageNumMeterPools:
                 (*stage_fc)->num_meter_pools = *(int *)tlv.value;
                 break;
            case _bcmFieldInternalStageNumCntrPools:
                 (*stage_fc)->num_cntr_pools = *(unsigned *)tlv.value;
                 break;
            case _bcmFieldInternalStageLtTcamSz:
                 (*stage_fc)->lt_tcam_sz = *(int *)tlv.value;
                 break;
            case _bcmFieldInternalStageNumLogicalTables:
                 (*stage_fc)->num_logical_tables = *(int *)tlv.value;
                 break;
            case _bcmFieldInternalStageExtLevels:
                 (*stage_fc)->num_ext_levels = *(int *)tlv.value;
                 break;
            case _bcmFieldInternalStageOperMode:
                 if ((*(bcm_field_group_oper_mode_t *)tlv.value)
                                        == bcmFieldGroupOperModePipeLocal) {
                     BCM_IF_ERROR_RETURN(_bcm_field_wb_group_oper_mode_set(unit,
                                         bcmFieldQualifyStageIngress,
                                         stage_fc,
                                         bcmFieldGroupOperModePipeLocal));
                 }
                 break;
            case _bcmFieldInternalStageRanges:
                 BCM_IF_ERROR_RETURN(_field_range_recover(unit,
                                                          &tlv, (*stage_fc)));
                 break;
            case _bcmFieldInternalStageMeterPool:
                 BCM_IF_ERROR_RETURN(_field_meterpool_recover(unit,
                                                              &tlv, (*stage_fc)));
                 break;
            case _bcmFieldInternalStageCntrPools:
                 BCM_IF_ERROR_RETURN(_field_cntrpool_recover(unit,
                                                             &tlv, (*stage_fc)));
                 break;
            case _bcmFieldInternalStageLtInfo:
                 BCM_IF_ERROR_RETURN(_field_ltinfo_recover(unit,
                                                      &tlv, (*stage_fc), NULL));
                 break;
            case _bcmFieldInternalDataControlStart:
                 BCM_IF_ERROR_RETURN(_field_data_control_recover(unit,
                                                                 &tlv,
                                                                 (*stage_fc)));
                 break;
            case _bcmFieldInternalEndStructStage:
                 if (*(uint32 *)tlv.value != _FIELD_WB_EM_STAGE) {
                        LOG_ERROR(BSL_LS_BCM_FP,(BSL_META_U(unit,
                                        "END MARKER CHECK FAILED : STAGE\n")));
                     return BCM_E_INTERNAL;
                 }
                 break;
            default:
                     LOG_ERROR(BSL_LS_BCM_FP,(BSL_META_U(unit,
                     "unknown type detected : _stage_recover\n")));
                 return BCM_E_INTERNAL;
        }
    }
    TLV_INIT(tlv);

    return BCM_E_NONE;
}

/* Function:
 * _field_slice_recover
 *
 * Purpose:
 *    recover _field_slice_t structure
 *
 * Paramaters:
 * unit          - (IN) BCM device number
 * pipe_id       - (IN) Pipe to which this slice belongs
 * slice         - (IN) Pointer to device slice structure
 *
 * Returns:
 *     BCM_E_XXX
 */

int
_field_slice_recover(int unit, int pipe_id, _field_slice_t *slice)
{
    _field_tlv_t tlv;                /* tlv structure */
    _field_control_t *fc = NULL;     /* Field control structure */
    _field_stage_t *stage_fc = NULL; /* Field stage structure */
    uint8 *scache_ptr;               /* Pointer to scache_ptr */
    uint8 temp_id;                   /* temp variable to store next
                                      * or prev slice id's
                                      */
    uint32 *position;                /* pointer to scache_pos */

    WB_FIELD_CONTROL_GET(fc, scache_ptr, position);
    BCM_IF_ERROR_RETURN(_field_stage_control_get(unit, _BCM_FIELD_STAGE_INGRESS,
                                                 &stage_fc));
    tlv.value = NULL;
    TLV_INIT(tlv);

    while (tlv.type != _bcmFieldInternalEndStructSlice) {
        TLV_INIT(tlv);
        BCM_IF_ERROR_RETURN(tlv_read(unit, &tlv, scache_ptr, position));

        switch (tlv.type) {

            case _bcmFieldInternalSliceNumber:
                  slice->slice_number = *(uint8 *)tlv.value;
                  break;
            case _bcmFieldInternalSliceStartTcamIdx:
                  slice->start_tcam_idx = *(int *)tlv.value;
                  break;
            case _bcmFieldInternalSliceEntryCount:
                  slice->entry_count = *(int *)tlv.value;
                  break;
            case _bcmFieldInternalSliceFreeCount:
                  slice->free_count = *(int *)tlv.value;
                  break;
            case _bcmFieldInternalSliceCountersCount:
                  slice->counters_count = *(int *)tlv.value;
                  break;
            case _bcmFieldInternalSliceMetersCount:
                  slice->meters_count = *(int *)tlv.value;
                  break;
            case _bcmFieldInternalSliceInstalledEntriesCount:
                  slice->hw_ent_count = *(int *)tlv.value;
                  break;
            case _bcmFieldInternalSliceCounterBmp:
                  sal_memcpy(&(slice->counter_bmp), tlv.value,
                              tlv.length);
                  break;
            case _bcmFieldInternalSliceMeterBmp:
                  sal_memcpy(&(slice->meter_bmp), tlv.value,
                          tlv.length);
                  break;
            case _bcmFieldInternalSliceStageId:
                  slice->stage_id = *(_field_stage_id_t *)tlv.value;
                  break;
            case _bcmFieldInternalSlicePortPbmp:
                  sal_memcpy(&(slice->pbmp), tlv.value,
                                (static_type_map[tlv.type].size * tlv.length));

                  break;
            case _bcmFieldInternalSliceNextSlice:
                  temp_id = *(uint8 *)tlv.value;
                  if (slice->slice_number > temp_id) {
                      slice->next =
                          &SLICE_STRUCT(stage_fc, slices, pipe_id, temp_id);
                      SLICE_STRUCT(stage_fc, slices, pipe_id, temp_id).prev
                                                                    = slice;
                  }
                  break;

            case _bcmFieldInternalSlicePrevSlice:
                  temp_id = *(uint8 *)tlv.value;
                  if (slice->slice_number > temp_id) {
                      slice->prev = &SLICE_STRUCT(stage_fc,slices,0,temp_id);
                      SLICE_STRUCT(stage_fc,slices,0,temp_id).next = slice;
                  }
                  break;

            case _bcmFieldInternalSliceFlags:
                  slice->slice_flags = *(uint16 *)tlv.value;
                  break;
            case _bcmFieldInternalSliceGroupFlags:
                  slice->group_flags = *(uint16 *)tlv.value;
                  break;
            case _bcmFieldInternalSliceLtMap:
                  slice->lt_map = *(uint32 *)tlv.value;
                  break;
            case _bcmFieldInternalSliceLtPartitionPri:
                  slice->lt_partition_pri = *(uint8 *)tlv.value;
                  break;
            case _bcmFieldInternalExtractor:
                 BCM_IF_ERROR_RETURN(_field_extractor_recover(unit, scache_ptr,
                                     position, &tlv, slice->ext_sel));
                 break;
            case _bcmFieldInternalEndStructSlice:
                 if (*(uint32 *)tlv.value != _FIELD_WB_EM_SLICE) {
                     LOG_ERROR(BSL_LS_BCM_FP,(BSL_META_U(unit,
                        "END MARKER CHECK FAILED : SLICE\n")));
                     return BCM_E_INTERNAL;
                 }
                 break;
            default:
                  TLV_INIT(tlv);
                     LOG_ERROR(BSL_LS_BCM_FP,(BSL_META_U(unit,
                     "unknown type detected : _slice_recover\n")));
                  return BCM_E_INTERNAL;
        }
    }
    return BCM_E_NONE;
}


/* Function:
 * _field_lt_slice_recover
 *
 * Purpose:
 *    recover _field_lt_slice_t structure
 *
 * Paramaters:
 * unit          - (IN) BCM device number
 * pipe_id       - (IN) Pipe to which this lt_slice belongs.
 * lt_slice      - (IN) Pointer to device lt_slice structure
 *
 * Returns:
 *     BCM_E_XXX
 */

int
_field_lt_slice_recover(int unit, int pipe_id, _field_lt_slice_t *lt_slice)
{
    _field_tlv_t tlv;                /* tlv structure */
    _field_control_t *fc = NULL;     /* field control structure */
    _field_stage_t *stage_fc = NULL; /* field stage structure */
    uint8 *scache_ptr;               /*pointer to scache_ptr */
    uint8 temp_id;                   /* temp var to store next
                                      * and prev slice id's
                                      */
    uint32 *position;                /* pointer to scache_pos */

    WB_FIELD_CONTROL_GET(fc, scache_ptr, position);
    BCM_IF_ERROR_RETURN(_field_stage_control_get(unit,
                        _BCM_FIELD_STAGE_INGRESS, &stage_fc));
    tlv.value = NULL;
    TLV_INIT(tlv);

    while (tlv.type != _bcmFieldInternalEndStructSlice) {
        TLV_INIT(tlv);
        BCM_IF_ERROR_RETURN(tlv_read(unit, &tlv, scache_ptr, position));

        switch (tlv.type) {

            case _bcmFieldInternalLtSliceSliceNum:
                  lt_slice->slice_number = *(uint8 *)tlv.value;
                  break;
            case _bcmFieldInternalLtSliceStartTcamIdx:
                  lt_slice->start_tcam_idx = *(int *)tlv.value;
                  break;
            case _bcmFieldInternalLtSliceEntryCount:
                  lt_slice->entry_count = *(int *)tlv.value;
                  break;
            case _bcmFieldInternalLtSliceFreeCount:
                  lt_slice->free_count = *(int *)tlv.value;
                  break;
            case _bcmFieldInternalLtSliceStageid:
                  lt_slice->stage_id = *(_field_stage_id_t *)tlv.value;
                  break;
            case _bcmFieldInternalLtSliceNextSlice:
                  temp_id = *(uint8 *)tlv.value;
                  if (lt_slice->slice_number > temp_id) {
                      lt_slice->next =
                          &SLICE_STRUCT(stage_fc, lt_slices, pipe_id, temp_id);
                      SLICE_STRUCT(stage_fc,lt_slices, pipe_id, temp_id).prev
                                                                    = lt_slice;
                  }
                  break;
            case _bcmFieldInternalLtSlicePrevSlice:
                  temp_id = *(uint8 *)tlv.value;
                  if (lt_slice->slice_number > temp_id) {
                      lt_slice->prev =
                          &SLICE_STRUCT(stage_fc, lt_slices, pipe_id, temp_id);
                      SLICE_STRUCT(stage_fc,lt_slices, pipe_id,temp_id).next
                                                                = lt_slice;
                  }
                  break;
            case _bcmFieldInternalLtSliceFlags:
                  lt_slice->slice_flags = *(uint16 *)tlv.value;
                  break;
            case _bcmFieldInternalLtSliceGroupFlags:
                  lt_slice->group_flags = *(uint16 *)tlv.value;
                  break;
            case _bcmFieldInternalEndStructSlice:
                  if (*(uint32 *)tlv.value != _FIELD_WB_EM_LTSLICE) {
                     LOG_ERROR(BSL_LS_BCM_FP,(BSL_META_U(unit,
                     "END MARKER CHECK FAILED : LT SLICE\n")));
                     return BCM_E_INTERNAL;
                  }
                  return BCM_E_NONE;
                  break;
            default:
                  TLV_INIT(tlv);
                  LOG_ERROR(BSL_LS_BCM_FP,(BSL_META_U(unit,
                  "unknown type detected : ltslice_recover\n")));
                  return BCM_E_INTERNAL;
        }
    }
    return BCM_E_NONE;

}
#define _FIELD_ENTRY_WB_ACTION_INSERT(_ptr_, action)    \
    do {                                                \
        if (action != NULL) {                           \
            action->next = _ptr_;                       \
            _ptr_ = action;                             \
        }                                               \
        action = NULL;                                  \
    } while (0);                                        \

/* Function:
 * _field_action_profile_set1_recover
 *
 * Purpose:
 *    recover profile set 1 actions
 *
 * Paramaters:
 * unit              - (IN) BCM device number
 * f_ent             - (IN) Field Entry Structure
 * e_buf             - (IN) FP Policy Entry data
 * act               - (OUT) Pointer to action structure
 *
 *
 * Returns:
 *     BCM_E_XXX
 */
int
_field_action_profile_set1_recover(int unit, _field_entry_t *f_ent,
                                   uint32* e_buf, _field_action_t *act)
{
    _bcm_field_action_offset_t a_offset; /* Action offsets */

    if (NULL == f_ent || NULL == act || NULL == e_buf) {
        LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META("Invalid Parameters\n")));
        return BCM_E_PARAM;
    }
    sal_memset(&a_offset, 0 , sizeof(a_offset));
    ACTION_GET(unit, f_ent, e_buf, act->action, 0, &a_offset);

    switch (act ->action) {

           case bcmFieldActionGpPrioIntCopy:
           case bcmFieldActionYpPrioIntCopy:
           case bcmFieldActionRpPrioIntCopy:
           case bcmFieldActionPrioIntCopy:
           case bcmFieldActionPrioIntTos:
           case bcmFieldActionGpPrioIntTos:
           case bcmFieldActionYpPrioIntTos:
           case bcmFieldActionRpPrioIntTos:
           case bcmFieldActionPrioIntCancel:
           case bcmFieldActionGpPrioIntCancel:
           case bcmFieldActionYpPrioIntCancel:
           case bcmFieldActionRpPrioIntCancel:
                /* No Params */
                break;

           case bcmFieldActionGpPrioIntNew:
           case bcmFieldActionYpPrioIntNew:
           case bcmFieldActionRpPrioIntNew:
           case bcmFieldActionGpDropPrecedence:
           case bcmFieldActionYpDropPrecedence:
           case bcmFieldActionRpDropPrecedence:
                act->param[0] = a_offset.value[0];
                break;

           case bcmFieldActionPrioIntNew:
                ACTION_GET(unit, f_ent, e_buf, bcmFieldActionGpPrioIntNew,
                           0, &a_offset);
                act->param[0] = a_offset.value[0];
                break;

           case bcmFieldActionDropPrecedence:
                ACTION_GET(unit, f_ent, e_buf, bcmFieldActionGpDropPrecedence,                             0, &a_offset);
                act->param[0] = a_offset.value[0];
                break;

           default:
                return BCM_E_INTERNAL;


    }

    return BCM_E_NONE;
}


/* Function:
 * _field_action_profile_set1_recover
 *
 * Purpose:
 *    recover profile set 2 actions
 *
 * Paramaters:
 * unit              - (IN) BCM device number
 * f_ent             - (IN) Field Entry Structure
 * e_buf             - (IN) FP Policy Entry data
 * act               - (OUT) Pointer to action structure
 *
 *
 * Returns:
 *     BCM_E_XXX
 */
int
_field_action_profile_set2_recover(int unit, _field_entry_t *f_ent,
                                   uint32* e_buf, _field_action_t *act)
{
    _bcm_field_action_offset_t a_offset;  /* Action offsets */

    if (NULL == f_ent || NULL == act || NULL == e_buf) {
        LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META("Invalid Parameters\n")));
        return BCM_E_PARAM;
    }
    sal_memset(&a_offset, 0 , sizeof(a_offset));
    ACTION_GET(unit, f_ent, e_buf, act->action, 0, &a_offset);

    switch (act ->action) {

           case bcmFieldActionGpEcnNew:
           case bcmFieldActionYpEcnNew:
           case bcmFieldActionRpEcnNew:
           case bcmFieldActionGpPrioPktNew:
           case bcmFieldActionYpPrioPktNew:
           case bcmFieldActionRpPrioPktNew:
           case bcmFieldActionRpDscpNew:
           case bcmFieldActionYpDscpNew:
           case bcmFieldActionGpDscpNew:
           case bcmFieldActionGpTosPrecedenceNew:
                act->param[0] = a_offset.value[0];
                break;

           case bcmFieldActionEcnNew:
                ACTION_GET(unit, f_ent, e_buf, bcmFieldActionGpEcnNew,
                           0, &a_offset);
                act->param[0] = a_offset.value[0];
                break;
           case bcmFieldActionPrioPktNew:
                ACTION_GET(unit, f_ent, e_buf, bcmFieldActionGpPrioPktNew,
                           0, &a_offset);
                act->param[0] = a_offset.value[0];
                break;
           case bcmFieldActionDscpNew:
                ACTION_GET(unit, f_ent, e_buf, bcmFieldActionGpDscpNew,
                           0, &a_offset);
                act->param[0] = a_offset.value[0];
                break;

           case bcmFieldActionGpPrioPktCopy:
           case bcmFieldActionGpPrioPktTos:
           case bcmFieldActionGpPrioPktCancel:
           case bcmFieldActionYpPrioPktCopy:
           case bcmFieldActionYpPrioPktTos:
           case bcmFieldActionYpPrioPktCancel:
           case bcmFieldActionRpPrioPktCopy:
           case bcmFieldActionRpPrioPktTos:
           case bcmFieldActionRpPrioPktCancel:
           case bcmFieldActionGpDscpCancel:
           case bcmFieldActionYpDscpCancel:
           case bcmFieldActionRpDscpCancel:
           case bcmFieldActionPrioPktCopy:
           case bcmFieldActionPrioPktTos:
           case bcmFieldActionDscpCancel:
           case bcmFieldActionPrioPktCancel:
           case bcmFieldActionGpTosPrecedenceCopy:
                /* No Params */
                break;
           default:
                return BCM_E_INTERNAL;

    }

    return BCM_E_NONE;

}


/* Function:
 * _field_action_l3swl2_recover
 *
 * Purpose:
 *    recover L3_SWITCH_L2 set actions
 *
 * Paramaters:
 * unit              - (IN) BCM device number
 * f_ent             - (IN) Field Entry Structure
 * e_buf             - (IN) FP Policy Entry data
 * act               - (OUT) Pointer to action structure
 *
 *
 * Returns:
 *     BCM_E_XXX
 */
int
_field_action_l3swl2_recover(int unit, _field_entry_t *f_ent,
                                   uint32* e_buf, _field_action_t *act)
{
    _bcm_field_action_offset_t a_offset; /* action offsets */

    if (NULL == f_ent || NULL == act || NULL == e_buf) {
        LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META("Invalid Parameters\n")));
        return BCM_E_PARAM;
    }
    sal_memset(&a_offset, 0 , sizeof(a_offset));
    ACTION_GET(unit, f_ent, e_buf, act->action, 0, &a_offset);

    switch (act ->action) {

         case bcmFieldActionNewClassId:
         case bcmFieldActionAddClassTag:
         case bcmFieldActionMultipathHash:
              act->param[0] = a_offset.value[0];
              break;
         case bcmFieldActionL3SwitchCancel:
              /* No Params */
              break;
         default:
              return BCM_E_INTERNAL;

    }

    return BCM_E_NONE;

}

/* Function:
 * _field_action_misc_recover
 *
 * Purpose:
 *    recover misc actions
 *
 * Paramaters:
 * unit              - (IN) BCM device number
 * f_ent             - (IN) Field Entry Structure
 * e_buf             - (IN) FP Policy Entry data
 * act               - (OUT) Pointer to action structure
 *
 *
 * Returns:
 *     BCM_E_XXX
 */
int
_field_action_misc_recover(int unit, _field_entry_t *f_ent,
                                   uint32* e_buf, _field_action_t *act)
{
    _bcm_field_action_offset_t a_offset;

    if (NULL == f_ent || NULL == act || NULL == e_buf) {
        LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META("Invalid Parameters\n")));
        return BCM_E_PARAM;
    }
    sal_memset(&a_offset, 0 , sizeof(a_offset));
    ACTION_GET(unit, f_ent, e_buf, act->action, 0, &a_offset);

    switch (act ->action) {
            case bcmFieldActionTrunkLoadBalanceCancel:
            case bcmFieldActionDynamicHgTrunkCancel:
            case bcmFieldActionEcmpLoadBalanceCancel:
            case bcmFieldActionNatCancel:
            case bcmFieldActionDoNotCheckUrpf:
            case bcmFieldActionDoNotChangeTtl:
            case bcmFieldActionMirrorOverride:
            case bcmFieldActionIngSampleEnable:
            case bcmFieldActionCopyToCpuCancel:
            case bcmFieldActionSwitchToCpuCancel:
            case bcmFieldActionSwitchToCpuReinstate:
            case bcmFieldActionDrop:
            case bcmFieldActionDropCancel:
            case bcmFieldActionGpCopyToCpuCancel:
            case bcmFieldActionGpSwitchToCpuCancel:
            case bcmFieldActionGpSwitchToCpuReinstate:
            case bcmFieldActionYpCopyToCpuCancel:
            case bcmFieldActionYpSwitchToCpuCancel:
            case bcmFieldActionYpSwitchToCpuReinstate:
            case bcmFieldActionRpCopyToCpuCancel:
            case bcmFieldActionRpSwitchToCpuCancel:
            case bcmFieldActionRpSwitchToCpuReinstate:
            case bcmFieldActionGpDrop:
            case bcmFieldActionGpDropCancel:
            case bcmFieldActionYpDrop:
            case bcmFieldActionYpDropCancel:
            case bcmFieldActionRpDrop:
            case bcmFieldActionRpDropCancel:
                 /* No params */
                 break;
            case bcmFieldActionColorIndependent:
            case bcmFieldActionServicePoolIdNew:
            case bcmFieldActionCosQCpuNew:
            case bcmFieldActionNat:
                 act->param[0] = a_offset.value[0];
                 break;
            default:
                 return BCM_E_INTERNAL;
    }
    return BCM_E_NONE;

}

/* Function:
 * _field_action_sofware_recover
 *
 * Purpose:
 *    recover cached actions
 *
 * Paramaters:
 * unit              - (IN) BCM device number
 * f_ent             - (IN) Field Entry Structure
 * e_buf             - (IN) FP Policy Entry data
 * act               - (OUT) Pointer to action structure
 *
 *
 * Returns:
 *     BCM_E_XXX
 */
int
_field_action_software_recover(int unit, _field_entry_t *f_ent,
                               _field_action_t *act, int *val_array, int *pos)
{
    int param_id = 0;                   /* Loop variable */
    int position = 0;                   /* pointer to position in the
                                         * array of values
                                         */
    _field_action_t *new_action = NULL; /* Used to hold multiple
                                         * actions structures in case
                                         * an action is repeated
                                         */
    _field_action_t *fa = act;          /* pointer to array of actions */

    position = *pos;
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
            case bcmFieldActionEgressClassSelect:
            case bcmFieldActionHiGigClassSelect:
            case bcmFieldActionFabricQueue:
            case bcmFieldActionL3Switch:
            case bcmFieldActionRedirectMcast:
            case bcmFieldActionRedirectEgrNextHop:
            case bcmFieldActionRedirectIpmc:
            case bcmFieldActionRedirectCancel:
            case bcmFieldActionOuterVlanNew:
               for (param_id = 0; param_id < 1; param_id++) {
                    act->param[param_id] = val_array[position++];
               }
               break;
            case bcmFieldActionDstMacNew:
            case bcmFieldActionSrcMacNew:
            case bcmFieldActionUnmodifiedPacketRedirectPort:
            case bcmFieldActionRedirect:
            case bcmFieldActionEtagNew:
            case bcmFieldActionVnTagNew:
            case bcmFieldActionRedirectTrunk:
                 for (param_id = 0; param_id < 2; param_id++) {
                      act->param[param_id] = val_array[position++];
                 }
                 break;
          case bcmFieldActionRedirectPbmp:
          case bcmFieldActionEgressMask:
          case bcmFieldActionEgressPortsAdd:
          case bcmFieldActionRedirectBcastPbmp:
               for (param_id = 0; param_id < 4; param_id++) {
                    act->param[param_id] = val_array[position++];
               }
               break;
            case bcmFieldActionMirrorIngress:
            case bcmFieldActionMirrorEgress:
                 fa = act;
                 /* fetch all mirror params, if param is != -1,
                  * action is valid, hence alloc new action
                  */
                 for (param_id = 0; param_id < BCM_MIRROR_MTP_COUNT;
                      param_id++) {
                      if ((val_array[position] != -1)
                           && (val_array[position + 1] != -1)) {
                           if (param_id != 0) {
                              new_action = NULL;
                              _FP_XGS3_ALLOC(new_action,
                                      sizeof(_field_action_t), "actions");
                              act->next = new_action;
                              act = act->next;
                           }
                           act->action = fa->action;
                           act->param[0] = val_array[position];
                           act->param[1] = val_array[position + 1];
                           act->hw_index = -1;
                           act->old_index = -1;
                           act->flags = _FP_ACTION_VALID ;

                      }
                      position = position + 2;
                 }
                 break;

            case bcmFieldActionRedirectVlan:
            case bcmFieldActionVnTagDelete:
            case bcmFieldActionEtagDelete:

      default:
             break;
    }
    *pos = position;
    return BCM_E_NONE;
}


/* Function:
 * _field_action_recover
 *
 * Purpose:
 *    recover _field_action_t structure
 *
 * Paramaters:
 * unit              - (IN) BCM device number
 * f_ent             - (IN) Field Entry Structure
 * act               - (OUT) Pointer to action structure
 * act_bmp           - (IN) Action Bitmap
 *
 *
 * Returns:
 *     BCM_E_XXX
 */

int
_field_actions_recover(int unit, _field_action_bmp_t act_bmp, uint32 *e_buf,
                       _field_entry_t *entry, int *val_array,
                       int *hw_idx_array)
{
    bcm_field_action_t action_id;         /* action id - used in loop */
    _field_action_t *curr_action = NULL;  /* current action */
    _field_action_t *fa = NULL;           /* pointer to actions array */
    int rv = 0, mirror_id = 0;            /* return value, loop variable */
    soc_profile_mem_t *redirect_profile;  /* pointer to redirect profile mem */
    int pos = 0;                          /* postion in the array of values
                                           * -val_array
                                           */
    int hw_count = 0;                     /* position in the array
                                           * of hw_indices
                                           */
    /* Validate input params , hw_index and val_array can be null in case there      * are actions that doesnt have params or hw_index
     */
    if (entry == NULL) {
         rv = BCM_E_INTERNAL;
         BCM_IF_ERROR_CLEANUP(rv);
    }

    if (act_bmp.w == NULL) {
        return BCM_E_NONE;
    }

    for (action_id = 0; action_id < bcmFieldActionCount; action_id++) {
        if (_FP_ACTION_BMP_TEST(act_bmp, action_id)) {
            _FP_XGS3_ALLOC(curr_action, sizeof(_field_action_t), "actions");
            curr_action->action = action_id;
            curr_action->hw_index = -1;
            curr_action->old_index = -1;
            curr_action->flags = _FP_ACTION_VALID;
            switch (action_id) {
                case bcmFieldActionGpPrioIntCopy:
                case bcmFieldActionGpPrioIntNew:
                case bcmFieldActionGpPrioIntTos:
                case bcmFieldActionGpPrioIntCancel:
                case bcmFieldActionYpPrioIntCopy:
                case bcmFieldActionYpPrioIntNew:
                case bcmFieldActionYpPrioIntTos:
                case bcmFieldActionYpPrioIntCancel:
                case bcmFieldActionRpPrioIntCopy:
                case bcmFieldActionRpPrioIntNew:
                case bcmFieldActionRpPrioIntTos:
                case bcmFieldActionRpPrioIntCancel:
                case bcmFieldActionGpDropPrecedence:
                case bcmFieldActionYpDropPrecedence:
                case bcmFieldActionRpDropPrecedence:
                case bcmFieldActionPrioIntCopy:
                case bcmFieldActionPrioIntNew:
                case bcmFieldActionPrioIntTos:
                case bcmFieldActionPrioIntCancel:
                case bcmFieldActionDropPrecedence:
                rv = _field_action_profile_set1_recover(unit, entry,
                                                   e_buf, curr_action);
                BCM_IF_ERROR_CLEANUP(rv);
                break;
                case bcmFieldActionGpEcnNew:
                case bcmFieldActionYpEcnNew:
                case bcmFieldActionRpEcnNew:
                case bcmFieldActionGpPrioPktCopy:
                case bcmFieldActionGpPrioPktNew:
                case bcmFieldActionGpPrioPktTos:
                case bcmFieldActionGpPrioPktCancel:
                case bcmFieldActionYpPrioPktCopy:
                case bcmFieldActionYpPrioPktNew:
                case bcmFieldActionYpPrioPktTos:
                case bcmFieldActionYpPrioPktCancel:
                case bcmFieldActionRpPrioPktCopy:
                case bcmFieldActionRpPrioPktNew:
                case bcmFieldActionRpPrioPktTos:
                case bcmFieldActionRpPrioPktCancel:
                case bcmFieldActionGpTosPrecedenceNew:
                case bcmFieldActionGpTosPrecedenceCopy:
                case bcmFieldActionGpDscpNew:
                case bcmFieldActionGpDscpCancel:
                case bcmFieldActionYpDscpNew:
                case bcmFieldActionYpDscpCancel:
                case bcmFieldActionRpDscpNew:
                case bcmFieldActionRpDscpCancel:
                case bcmFieldActionEcnNew:
                case bcmFieldActionPrioPktCopy:
                case bcmFieldActionPrioPktNew:
                case bcmFieldActionPrioPktTos:
                case bcmFieldActionPrioPktCancel:
                case bcmFieldActionDscpNew:
                case bcmFieldActionDscpCancel:
                rv = _field_action_profile_set2_recover(unit, entry,
                                                  e_buf, curr_action);
                BCM_IF_ERROR_CLEANUP(rv);
                break;
                case bcmFieldActionNewClassId:
                case bcmFieldActionAddClassTag:
                case bcmFieldActionMultipathHash:
                case bcmFieldActionL3SwitchCancel:
                rv = _field_action_l3swl2_recover(unit, entry,
                                            e_buf, curr_action);
                BCM_IF_ERROR_CLEANUP(rv);
                break;
                case bcmFieldActionTrunkLoadBalanceCancel:
                case bcmFieldActionDynamicHgTrunkCancel:
                case bcmFieldActionEcmpLoadBalanceCancel:
                case bcmFieldActionNatCancel:
                case bcmFieldActionDoNotCheckUrpf:
                case bcmFieldActionDoNotChangeTtl:
                case bcmFieldActionMirrorOverride:
                case bcmFieldActionIngSampleEnable:
                case bcmFieldActionCopyToCpuCancel:
                case bcmFieldActionSwitchToCpuCancel:
                case bcmFieldActionSwitchToCpuReinstate:
                case bcmFieldActionDrop:
                case bcmFieldActionDropCancel:
                case bcmFieldActionGpCopyToCpuCancel:
                case bcmFieldActionGpSwitchToCpuCancel:
                case bcmFieldActionGpSwitchToCpuReinstate:
                case bcmFieldActionYpCopyToCpuCancel:
                case bcmFieldActionYpSwitchToCpuCancel:
                case bcmFieldActionYpSwitchToCpuReinstate:
                case bcmFieldActionRpCopyToCpuCancel:
                case bcmFieldActionRpSwitchToCpuCancel:
                case bcmFieldActionRpSwitchToCpuReinstate:
                case bcmFieldActionGpDrop:
                case bcmFieldActionGpDropCancel:
                case bcmFieldActionYpDrop:
                case bcmFieldActionYpDropCancel:
                case bcmFieldActionRpDrop:
                case bcmFieldActionRpDropCancel:
                case bcmFieldActionNat:
                case bcmFieldActionColorIndependent:
                case bcmFieldActionServicePoolIdNew:
                case bcmFieldActionCosQCpuNew:
                rv = _field_action_misc_recover(unit, entry,
                                           e_buf, curr_action);
                BCM_IF_ERROR_CLEANUP(rv);
                break;
                case bcmFieldActionCopyToCpu:
                case bcmFieldActionTimeStampToCpu:
                case bcmFieldActionRpCopyToCpu:
                case bcmFieldActionRpTimeStampToCpu:
                case bcmFieldActionYpCopyToCpu:
                case bcmFieldActionYpTimeStampToCpu:
                case bcmFieldActionGpCopyToCpu:
                case bcmFieldActionGpTimeStampToCpu:
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
                case bcmFieldActionEgressClassSelect:
                case bcmFieldActionHiGigClassSelect:
                case bcmFieldActionFabricQueue:
#ifdef INCLUDE_L3
                case bcmFieldActionL3Switch:
                case bcmFieldActionNatEgressOverride:
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
                case bcmFieldActionMirrorIngress:
                case bcmFieldActionMirrorEgress:
                case bcmFieldActionDstMacNew:
                case bcmFieldActionSrcMacNew:
                case bcmFieldActionOuterVlanNew:
                case bcmFieldActionVnTagNew:
                case bcmFieldActionVnTagDelete:
                case bcmFieldActionEtagNew:
                case bcmFieldActionEtagDelete:
                rv = _field_action_software_recover(unit, entry,
                                           curr_action, val_array, &pos);
                BCM_IF_ERROR_CLEANUP(rv);
                break;
                default:
                     rv = BCM_E_INTERNAL;
                     goto cleanup;
             }

            switch (curr_action->action) {
                case bcmFieldActionDstMacNew:
                case bcmFieldActionSrcMacNew:
                case bcmFieldActionOuterVlanNew:
                case bcmFieldActionVnTagNew:
                case bcmFieldActionVnTagDelete:
                case bcmFieldActionEtagNew:
                case bcmFieldActionEtagDelete:
                case bcmFieldActionRedirectPbmp:
                case bcmFieldActionEgressMask:
                case bcmFieldActionEgressPortsAdd:
                case bcmFieldActionRedirectIpmc:
                case bcmFieldActionRedirectMcast:
                case bcmFieldActionRedirectBcastPbmp:
                case bcmFieldActionRedirectEgrNextHop:
                case bcmFieldActionL3Switch:
                     curr_action->hw_index = hw_idx_array[hw_count++];
                     break;
                case bcmFieldActionMirrorIngress:
                case bcmFieldActionMirrorEgress:
                     fa = curr_action;
                     for (mirror_id = 0; mirror_id < BCM_MIRROR_MTP_COUNT;
                          mirror_id++) {
                          if (hw_idx_array[hw_count] != -1) {
                              fa->hw_index = hw_idx_array[hw_count];
                              if (fa ->next != NULL) {
                                  fa = fa->next;
                              }
                           }
                           hw_count++;
                     }
                     break;
                 default :
                    break;
            }


            /* Update profile memeory for redirect action's reference count*/
            if ((curr_action->action == bcmFieldActionRedirectBcastPbmp) ||
                (curr_action->action == bcmFieldActionRedirectPbmp)) {
                 BCM_IF_ERROR_RETURN(_field_trx_redirect_profile_get(unit,
                                                           &redirect_profile));
                 SOC_PROFILE_MEM_REFERENCE(unit, redirect_profile,
                                            curr_action->hw_index, 1);
                 SOC_PROFILE_MEM_ENTRIES_PER_SET(unit, redirect_profile,
                                                 curr_action->hw_index, 1);
            }
            if ((curr_action->action == bcmFieldActionMirrorIngress) ||
                (curr_action->action == bcmFieldActionMirrorEgress)) {
                 fa ->next = entry->actions;
            } else {
                 curr_action->next = entry->actions;
            }
            entry->actions = curr_action;
            curr_action = NULL;

        }
    }

cleanup:
    sal_free(val_array);
    sal_free(hw_idx_array);
    if (NULL != curr_action) {
        sal_free(curr_action);
    }
    return rv;
}

#define _MAX_ENTRY_PARTS_COUNT 3

/* Function:
 * _field_lt_entry_info_recover
 *
 * Purpose:
 *    recover _field_lt_entry_t structure
 *
 * Paramaters:
 * unit          - (IN) BCM device number
 * f_lt_ent      - (IN/OUT) _field_lt_entry_t structure with memory allocated.
 * fc            - (IN) _field_control_t structure.
 * flags         - (IN/OUT) Array of uint32, to fill in the flags for each part.
 * Returns:
 *     BCM_E_XXX
 */
int
_field_lt_entry_info_recover(int unit, _field_lt_entry_t *f_lt_ent,
                             uint32 flags[], _field_group_t **new_fg)
{
    _field_tlv_t tlv;               /* tlv structure */
    uint8 *scache_ptr;              /* pointer to scache_ptr (base) */
    uint32 *position;               /* pointer to scache_pos (offset) */
    _field_control_t *fc = NULL;    /* pointer to field control structure */
    _field_stage_t *stage_fc = NULL;/* Field stage Structure */
    int temp;                       /* Temporary variable */
    int rv = 0;                     /* return Variable */
    _field_action_t *action = NULL; /* Lt entry action */

    WB_FIELD_CONTROL_GET(fc, scache_ptr, position);
    BCM_IF_ERROR_RETURN(_field_stage_control_get(unit,
                        _BCM_FIELD_STAGE_INGRESS, &stage_fc));
    tlv.value = NULL;
    TLV_INIT(tlv);

    while (tlv.type != _bcmFieldInternalEndStructEntry) {
        TLV_INIT(tlv);
        BCM_IF_ERROR_RETURN(tlv_read(unit, &tlv, scache_ptr, position));
        switch (tlv.type) {
            case _bcmFieldInternalEntryFlagsPart1:
                 f_lt_ent->flags = *(uint32 *)tlv.value;
                 break;

            case _bcmFieldInternalEntryFlagsPart2:
                 flags[1] = *(uint32 *)tlv.value;
                 break;

            case _bcmFieldInternalEntryFlagsPart3:
                 flags[2] = *(uint32 *)tlv.value;
                 break;


            case _bcmFieldInternalEntryEid:
                 f_lt_ent->eid = *(bcm_field_entry_t *)tlv.value;
                 break;


            case _bcmFieldInternalEntrySliceId:
                 f_lt_ent->index = *(uint32 *)tlv.value;
                 break;

            case _bcmFieldInternalEntryPrio:
                 f_lt_ent->prio = *(int *)tlv.value;
                 break;


            case _bcmFieldInternalEntrySlice:
                 temp = *(int *)tlv.value;
                 f_lt_ent->lt_fs =
                     &stage_fc->lt_slices[f_lt_ent->group->instance][temp];
                 break;

            case _bcmFieldInternalGroupClassAct:
                 _FP_XGS3_ALLOC(action, sizeof(_field_action_t), "actions");
                 action->action = bcmFieldActionGroupClassSet;
                 action->param[0] = *(uint32 *)tlv.value;
                 f_lt_ent->actions = action;
                 BCM_IF_ERROR_CLEANUP(rv);
                 break;

            case _bcmFieldInternalEntryGroup:
                 temp = *(int *)tlv.value;
                 f_lt_ent->group = (new_fg[temp]);
                 break;

            case _bcmFieldInternalEndStructEntry:
                 if (*(uint32 *)tlv.value != _FIELD_WB_EM_LTENTRY) {
                        LOG_ERROR(BSL_LS_BCM_FP,(BSL_META_U(unit,
                                      "END MARKER CHECK FAILED : LTENTRY\n")));
                     return BCM_E_INTERNAL;
                 }
                 break;

            default:
                TLV_INIT(tlv);
                LOG_ERROR(BSL_LS_BCM_FP,(BSL_META_U(unit,
                "unknown type detected : _lt_entry_recover\n")));
                return BCM_E_INTERNAL;
        }

    }

cleanup:
    TLV_INIT(tlv);
    return BCM_E_NONE;
}

/* Function:
 * _field_entry_policer_recover
 *
 * Purpose:
 *    recover field policers in  _field_entry_t structure
 *
 * Paramaters:
 * unit          - (IN) BCM device number
 * tlv           - (IN) tlv structure containing encoded types
 * f_ent         - (IN/OUT) _field_entry_t structure with memory allocated.
 * Returns:
 *     BCM_E_XXX
 */
int
_field_entry_policer_recover(int unit, _field_tlv_t *tlv, _field_entry_t *f_ent)
{

    int type_pos = 0;                     /* position of type */
    int i = 0;                            /* local variable for loops */
    int num_instances = 0;                /* number of instances */
    int num_types = 0;                    /* Number of types */
    _field_tlv_t tlv2;                    /* TLV structure */
    uint32 *type;                         /* Elem type */
    uint8 *scache_ptr;                    /* Scahce pointer */
    uint32 *position;                     /* Scache position */
    _field_control_t  *fc = NULL;         /* Field control structure.  */

    WB_FIELD_CONTROL_GET(fc, scache_ptr, position);
    tlv2.value = NULL;
    TLV_INIT(tlv2);
    type = (uint32 *)tlv->value;
    num_instances = (tlv->length) & _FP_WB_TLV_LENGTH_MASK;
    num_types     = ((tlv->length & _FP_WB_TLV_TYPES_MASK) >>
                    _FP_WB_TLV_TYPE_SHIFT);

    for (i = 0; i < num_instances; i++) {

        type_pos = 0;
        while  ((tlv2.type != _bcmFieldInternalEndStructEntPolicer)
                 && (type_pos != num_types)) {
            TLV_INIT(tlv2);
            tlv2.type = (type[type_pos] & _FP_WB_TLV_ELEM_TYPE_MASK);
            tlv2.basic_type = ((type[type_pos] & _FP_WB_TLV_BASIC_TYPE_MASK) >>
                              _FP_WB_TLV_BASIC_TYPE_SHIFT);
            BCM_IF_ERROR_RETURN(tlv_read(unit, &tlv2, scache_ptr, position));

            switch (tlv2.type) {

            case _bcmFieldInternalEntryPolicerPid:
                 f_ent->policer[i].pid = *(bcm_policer_t *)tlv2.value;
                 break;
            case _bcmFieldInternalEntryPolicerFlags:
                 f_ent->policer[i].flags = *(uint8 *)tlv2.value;
                 break;
            case _bcmFieldInternalEndStructEntPolicer:
                 TLV_INIT(tlv2);
                 break;
            default:
                 LOG_ERROR(BSL_LS_BCM_FP,(BSL_META_U(unit,
                     "unknown type detected : _entrypolicer_recover\n")));
                 return BCM_E_INTERNAL;
            }
            type_pos++;
        }
    }

    return BCM_E_NONE;
}

/* Function:
 * _field_entry_info_recover
 *
 * Purpose:
 *    recover _field_entry_t structure
 *
 * Paramaters:
 * unit          - (IN) BCM device number
 * f_ent         - (IN/OUT) _field_entry_t structure with memory allocated.
 * fc            - (IN) _field_control_t structure.
 * flags         - (IN/OUT) Array of uint32, to fill in the flags for each part.
 * Returns:
 *     BCM_E_XXX
 */

int
_field_entry_info_recover(int unit, _field_entry_t *f_ent,
                          uint32 flags[], int parts_count,
                          _field_group_t **new_fg,
                          _field_action_bmp_t *act_bmp, int **val_array,
                          int **hw_idx_array)

{
    _field_tlv_t tlv;                  /* tlv structure */
    uint8 *scache_ptr;                 /* pointer to scache_ptr (base) */
    uint32 *position;                  /* Pointer to scache_pos (offset) */
    /* uint32 flag_cpy[_MAX_ENTRY_PARTS_COUNT] = {0}; */
                                       /* array to flags of each entry part,
                                        * entries parts of the same
                                        * entry differ only in flags
                                        */
    _field_control_t *fc = NULL;       /* pointer to field control structure */
    _field_stage_t *stage_fc = NULL;   /* Field stage structure */
    int temp = 0;                      /* Temporary variable */
    int rv = 0;                        /* Return Variable */

    tlv.value = NULL;
    TLV_INIT(tlv);
    act_bmp->w = NULL;

    WB_FIELD_CONTROL_GET(fc, scache_ptr, position);
    BCM_IF_ERROR_RETURN(_field_stage_control_get(unit,
                        _BCM_FIELD_STAGE_INGRESS, &stage_fc));

    while (tlv.type != _bcmFieldInternalEndStructEntry) {
        TLV_INIT(tlv);
        rv = tlv_read(unit, &tlv, scache_ptr, position);
        BCM_IF_ERROR_CLEANUP(rv);
        switch (tlv.type)
        {

            case _bcmFieldInternalEntryEid:
                 f_ent->eid = *(bcm_field_entry_t *)tlv.value;
                 break;
            case _bcmFieldInternalEntryPrio:
                 f_ent->prio = *(int *)tlv.value;
                 break;

            case _bcmFieldInternalEntrySliceId:
                 f_ent->slice_idx = *(uint32 *)tlv.value;
                 break;

            case _bcmFieldInternalEntryFlagsPart1:
                 f_ent->flags = *(uint32 *)tlv.value;
                 break;

            case _bcmFieldInternalEntryFlagsPart2:
                 flags[1] = *(uint32 *)tlv.value;
                 break;

            case _bcmFieldInternalEntryFlagsPart3:
                 flags[2] = *(uint32 *)tlv.value;
                 break;

            case _bcmFieldInternalEntryPbmpData:
                 f_ent->pbmp.data = *(bcm_pbmp_t *)tlv.value;
                 break;

            case _bcmFieldInternalEntryPbmpMask:
                 f_ent->pbmp.mask = *(bcm_pbmp_t *)tlv.value;
                 break;

            case _bcmFieldInternalEntrySlice:
                 temp = *(int *)tlv.value;
                 f_ent->fs = &stage_fc->slices[f_ent->group->instance][temp];
                 break;

            case _bcmFieldInternalEntryGroup:
                 temp = *(int *)tlv.value;
                 f_ent->group = (new_fg[temp]);
                 break;

            case _bcmFieldInternalEntryActionsPbmp:
                 _FP_XGS3_ALLOC(act_bmp->w,
                                (tlv.length * static_type_map[tlv.type].size),
                                "action_bmp");
                 sal_memcpy(act_bmp->w, tlv.value,
                            (tlv.length * static_type_map[tlv.type].size));
                 break;

            case _bcmFieldInternalActionParam:
                 *val_array = (int *)tlv.value;
                 tlv.value = NULL;
                 break;

            case _bcmFieldInternalActionHwIdx:
                 *hw_idx_array = (int *)tlv.value;
                 tlv.value = NULL;
                 break;

            case _bcmFieldInternalEntryStatSid:
                 f_ent->statistic.sid = *(int *)tlv.value;
                 break;

            case _bcmFieldInternalEntryStatExtendedSid:
                 f_ent->statistic.extended_sid = *(int *)(tlv.value);
                 break;
            case _bcmFieldInternalEntryStatFlags:
                 f_ent->statistic.flags = *(uint16 *)tlv.value;
                 break;
            case _bcmFieldInternalEntryStatAction:
                 f_ent->statistic.stat_action =
                            *(bcm_field_stat_action_t *)tlv.value;
                 break;

            case _bcmFieldInternalEntryPolicer:
                  _field_entry_policer_recover(unit, &tlv, f_ent);
                 break;

            case _bcmFieldInternalGlobalEntryPolicerPid:
                 f_ent->global_meter_policer.pid = *(bcm_policer_t *) tlv.value;
                 break;

            case _bcmFieldInternalGlobalEntryPolicerFlags:
                 f_ent->global_meter_policer.flags = *(uint8 *)tlv.value;
                 break;

            case _bcmFieldInternalEntryIngMtp:
                 f_ent->ing_mtp_slot_map = *(uint8 *)tlv.value;
                 break;

            case _bcmFieldInternalEntryEgrMtp:
                 f_ent->egr_mtp_slot_map = *(uint8 *)tlv.value;
                 break;

            case _bcmFieldInternalEntryDvp:
                 f_ent->dvp_type = *(uint8 *)tlv.value;
                 break;
            case _bcmFieldInternalEntryCopy:
                 /*
                 _FP_XGS3_ALLOC(f_ent->ent_copy,
                 parts_count* sizeof(_field_entry_t), "field entry copy");
                 _field_entry_info_recover(unit, f_ent->ent_copy, flag_cpy,
                                           parts_count, new_fg, policy_mem,
                                           policy_buf);
                 if (parts_count > 1) {
                    for (j=1; j < parts_count; j++) {
                        sal_memcpy((f_ent->ent_copy)+j, (f_ent->ent_copy),
                                                      sizeof(_field_entry_t));
                        ((f_ent->ent_copy)+j)->flags = flag_cpy[j];
                    }
                 }
                 */
                 break;

            case _bcmFieldInternalEndStructEntry:
                 if (*(uint32 *)tlv.value != _FIELD_WB_EM_ENTRY) {
                        LOG_ERROR(BSL_LS_BCM_FP,(BSL_META_U(unit,
                                        "END MARKER CHECK FAILED : ENTRY\n")));
                     rv = BCM_E_INTERNAL;
                     goto cleanup;
                 }
                 break;

            default:
                 TLV_INIT(tlv);
                 LOG_ERROR(BSL_LS_BCM_FP,(BSL_META_U(unit,
                  "unknown type detected : _entryinfo_recover\n")));
                 rv =  BCM_E_INTERNAL;
                 goto cleanup;

        }
    }
    return BCM_E_NONE;
cleanup:
    TLV_INIT(tlv);
    if (f_ent->actions != NULL) {
        sal_free(f_ent->actions);
        f_ent->actions = NULL;
    }
    if (f_ent->ent_copy != NULL) {
       sal_free(f_ent->ent_copy);
       f_ent->ent_copy = NULL;
    }
    return rv;
}

/* Function:
 * _field_entry_recover
 *
 * Purpose:
 *    recover _field_entry_t and _field_lt_entry_t structures
 *    For each group,
 *       Get partscount and number of entries, call entry recover for each entry
 *       Using the same parts count, and get number of lt_entries, call
 *       lt_entry_recover for each lt_entry.
 *       Proceed to next group.
 *    when _bcmFieldInternalEndStructEntryDetails is reached, that means all
 *    entries in all groups are recovered.
 *
 * Paramaters:
 * unit          - (IN) BCM device number
 *
 * Returns:
 *     BCM_E_XXX
 */
int
_field_entry_recover(int unit, _field_group_t **new_fg)
{
    _field_tlv_t tlv;                   /* tlv structure */
    uint8 *scache_ptr;                  /* pointer to scache_pointer (base) */
    uint32 *position;                   /* pointer to scache_pos */
    uint32 flags[_MAX_ENTRY_PARTS_COUNT] = {0};
                                        /* Flags array to store flags
                                         * for each entry part
                                         */
    _field_control_t *fc = NULL;        /* pointer to field control structure */
    _field_entry_t *f_ent = NULL;       /* pointer to entry structure */
    _field_lt_entry_t *f_lt_ent = NULL; /* pointer to lt_entry structure */
    _field_dynamic_map_entry_t *dyf_ent= NULL;
                                        /* pointer to entry in dynamic map */
    _field_dynamic_map_entry_t *dyf_ent_tail = NULL;
                                        /* Tail of dynamic entry map LL */
    _field_dynamic_map_lt_entry_t *dyf_lt_ent= NULL;
                                        /* Dynamic  LT entry map structure */
    _field_dynamic_map_lt_entry_t *dyf_lt_ent_tail = NULL;
                                        /* Tail of Dynamic LT entry map LL */

    int i,j, parts_count = 0;           /* i,j - loop variables
                                         * parts_count ->
                                         * Number of parts in entry
                                         */
    int entry_count = 0, rv = 0;        /* entry_count -> number of entries
                                         * rv -> return value
                                         */
    _field_stage_t *stage_fc = NULL;    /* Field Stage structure */
    int pri_tcam_idx = -1;              /* Primary tcam index */
    int policy_tcam_idx = -1;           /* Policy memory index */
    int slice_number = 0;               /* Slice where entry belongs */
    int part_index =  0;                /* To loop through parts */
    soc_mem_t *fp_policy_mem;           /* IFP Policy memeory */
    char *fp_policy_buf[_FP_MAX_NUM_PIPES] =  {0};
                                        /* Buffer to read the FP_POLICY table */
    soc_mem_t *lt_policy_mem;           /* LT Policy memory */
    char *lt_policy_buf[_FP_MAX_NUM_PIPES] = {0};
                                        /* Buffer to read the FP_POLICY table */
    int index_min = -1, index_max = -1; /* Min and max index in Policy memory */
    int *val_array = NULL;              /* Array to hold params */
    int *hw_idx_array = NULL;           /* Array to hold hw_index */
    int pipe = 0, num_pipes = 0;        /*  Pipe variable and count of pipes */
    uint32 *e_buf = NULL;               /* Buffer holding corresponding entry */
    soc_mem_t  policy_tcam_mems[_FP_MAX_NUM_PIPES + 1] = {
                                                        IFP_POLICY_TABLE_PIPE0m,
                                                        IFP_POLICY_TABLE_PIPE1m,
                                                        IFP_POLICY_TABLE_PIPE2m,
                                                        IFP_POLICY_TABLE_PIPE3m,
                                                        IFP_POLICY_TABLEm
                                                      };
                                        /* IFP policy memory for every Pipe */
    soc_mem_t lt_policy_mems[_FP_MAX_NUM_PIPES + 1] =
                                 {
                                    IFP_LOGICAL_TABLE_SELECT_DATA_ONLY_PIPE0m,
                                    IFP_LOGICAL_TABLE_SELECT_DATA_ONLY_PIPE1m,
                                    IFP_LOGICAL_TABLE_SELECT_DATA_ONLY_PIPE2m,
                                    IFP_LOGICAL_TABLE_SELECT_DATA_ONLY_PIPE3m,
                                    IFP_LOGICAL_TABLE_SELECT_DATA_ONLYm
                                 };
                                        /* LT policy memory for every Pipe */
   _field_action_bmp_t act_bmp;         /* Action bitmap */

    BCM_IF_ERROR_RETURN(_field_stage_control_get(unit,
                         _BCM_FIELD_STAGE_INGRESS, &stage_fc));
    WB_FIELD_CONTROL_GET(fc, scache_ptr, position);
    tlv.value = NULL;
    TLV_INIT(tlv);

    if (stage_fc->oper_mode == bcmFieldGroupOperModeGlobal) {
        num_pipes = 1;
        fp_policy_mem = &policy_tcam_mems[_FP_MAX_NUM_PIPES];
        lt_policy_mem = &lt_policy_mems[_FP_MAX_NUM_PIPES];
    } else {
        num_pipes = 4;
        fp_policy_mem = &policy_tcam_mems[0];
        lt_policy_mem = &lt_policy_mems[0];
    }

    for (pipe = 0; pipe < num_pipes; pipe++) {

        fp_policy_buf[pipe] = soc_cm_salloc(unit,
                           SOC_MEM_TABLE_BYTES(unit, fp_policy_mem[pipe]),
                           "FP POLICY TABLE buffer");
        lt_policy_buf[pipe] = soc_cm_salloc(unit,
                           SOC_MEM_TABLE_BYTES(unit, lt_policy_mem[pipe]),
                           "FP POLICY TABLE buffer");
        if ((NULL == fp_policy_buf[pipe]) || (NULL == lt_policy_buf[pipe])) {
            rv = BCM_E_MEMORY;
            goto cleanup;
        }

        index_min = soc_mem_index_min(unit, fp_policy_mem[pipe]);
        index_max = soc_mem_index_max(unit, fp_policy_mem[pipe]);
        rv = soc_mem_read_range(unit, fp_policy_mem[pipe], MEM_BLOCK_ALL,
                                   index_min, index_max, fp_policy_buf[pipe]);
        BCM_IF_ERROR_CLEANUP(rv);

        index_min = soc_mem_index_min(unit, lt_policy_mem[pipe]);
        index_max = soc_mem_index_max(unit, lt_policy_mem[pipe]);
        rv = soc_mem_read_range(unit, lt_policy_mem[pipe], MEM_BLOCK_ALL,
                                 index_min, index_max, lt_policy_buf[pipe]);
        BCM_IF_ERROR_CLEANUP(rv);
    }

    while (tlv.type != _bcmFieldInternalEndStructEntryDetails) {
        TLV_INIT(tlv);
        rv = tlv_read(unit, &tlv, scache_ptr, position);
        BCM_IF_ERROR_CLEANUP(rv);
        switch (tlv.type) {
            case _bcmFieldInternalGroupPartCount:
                 parts_count = *(int *)tlv.value;
                 break;

            case _bcmFieldInternalGroupEntry:
                 entry_count = *(int *)tlv.value;
                 for (i = 0; i < entry_count; i++) {
                    _FP_XGS3_ALLOC(f_ent, parts_count* sizeof(_field_entry_t),
                                                                "field entry");
                    _FP_XGS3_ALLOC(dyf_ent, sizeof(_field_dynamic_map_entry_t),
                                    "temp ent");
                    sal_memset(flags, 0, sizeof(flags));

                    /* Recover entry */
                    rv = _field_entry_info_recover(unit, f_ent, flags,
                                                   parts_count, new_fg,
                                                   &act_bmp, &val_array,
                                                   &hw_idx_array);
                    BCM_IF_ERROR_CLEANUP(rv);
                    /* Get tcam index for entry */
                    pipe = f_ent->group->instance;
                    rv = _bcm_field_slice_offset_to_tcam_idx (unit, stage_fc,
                                                        pipe,
                                                        f_ent->fs->slice_number,
                                                        f_ent->slice_idx,
                                                        &pri_tcam_idx);
                    policy_tcam_idx = pri_tcam_idx;

                    if ((!(f_ent->group->flags
                         & _FP_GROUP_SPAN_SINGLE_SLICE)
                         || (f_ent->group->flags
                         & _FP_GROUP_INTRASLICE_DOUBLEWIDE))) {
                         policy_tcam_idx = pri_tcam_idx
                                     + (f_ent->fs->slice_number * 256);
                    }

                    BCM_IF_ERROR_CLEANUP(rv);
                    /* Get memory for tcam index  */
                    e_buf = soc_mem_table_idx_to_pointer(
                                         unit,
                                         fp_policy_mem[pipe],
                                         uint32 *,
                                         fp_policy_buf[pipe],
                                         policy_tcam_idx);

                    /* Actions recover for entry */
                    rv =  _field_actions_recover(unit, act_bmp, e_buf,
                                                 f_ent, val_array,
                                                 hw_idx_array);
                    BCM_IF_ERROR_CLEANUP(rv);

                    /* Recover remaining entry parts */
                    if (parts_count > 1) {
                        for (j=1; j < parts_count; j++) {
                            sal_memcpy(f_ent+j, f_ent, sizeof(_field_entry_t));
                            (f_ent+j)->flags = flags[j];
                            (f_ent+j)->actions = NULL;
                             /* Given primary entry tcam index calculate entry
                                                            part tcam index. */
                            rv = _bcm_field_entry_part_tcam_idx_get(unit, f_ent,
                                  pri_tcam_idx,  j, &part_index);
                            BCM_IF_ERROR_CLEANUP(rv);
                            rv = _bcm_field_th_tcam_idx_to_slice_offset(unit,
                                        stage_fc, f_ent, part_index,
                                                  &slice_number,
                                                  (int *)&f_ent[j].slice_idx);
                            BCM_IF_ERROR_CLEANUP(rv);
                            f_ent[j].fs =
                                    stage_fc->slices[f_ent->group->instance]
                                                            + slice_number;

                        }

                    }

                    /* Add to dynamic map */
                    dyf_ent->entry = f_ent;
                    dyf_ent->entry_id = f_ent->eid;
                    _DYNAMIC_MAP_INSERT(dynamic_map.entry_map, dyf_ent,
                                        dyf_ent_tail);
                    dyf_ent = NULL;
                    f_ent = NULL;
                 }
                 break;

            case _bcmFieldInternalGroupLtEntry:
                 entry_count = *(int *)tlv.value;
                 for (i = 0; i < entry_count; i++) {
                    _FP_XGS3_ALLOC(f_lt_ent,
                       parts_count*sizeof(_field_lt_entry_t), "field entry");
                    _FP_XGS3_ALLOC(dyf_lt_ent,
                        sizeof(_field_dynamic_map_entry_t), "temp ent");
                    sal_memset(flags, 0, sizeof(flags));

                    /* Recover entry */
                    rv =_field_lt_entry_info_recover(unit, f_lt_ent,
                                                     flags, new_fg);

                    /* Get tcam index for entry */
                    rv = _bcm_field_th_lt_slice_offset_to_tcam_index(unit,
                              stage_fc, f_lt_ent->group->instance,
                              f_lt_ent->lt_fs->slice_number,
                                  f_lt_ent->index, &pri_tcam_idx);
                    BCM_IF_ERROR_CLEANUP(rv);

                    /* Recover remaining entry parts */
                    if (parts_count > 1) {
                        for (j=1; j < parts_count; j++) {
                            sal_memcpy(f_lt_ent+j, f_lt_ent,
                                        sizeof(_field_lt_entry_t));
                            (f_lt_ent+j)->flags = flags[j];

                            rv = _bcm_field_th_lt_entry_part_tcam_idx_get(unit,
                                            f_lt_ent->group, pri_tcam_idx,
                                                          j, &part_index);
                            BCM_IF_ERROR_CLEANUP(rv);
                            rv = _bcm_field_th_lt_tcam_idx_to_slice_offset(unit,
                                       stage_fc, f_lt_ent->group->instance,
                                         part_index,
                                         &slice_number,(int *)&f_lt_ent->index);
                            BCM_IF_ERROR_CLEANUP(rv);
                            f_lt_ent->lt_fs =
                                 stage_fc->lt_slices[f_lt_ent->group->instance]
                                                    + slice_number;

                        }
                    }

                    /* Add to dynamic map */
                    dyf_lt_ent->lt_entry = f_lt_ent;
                    dyf_lt_ent->lt_entry_id = f_lt_ent->eid;
                    _DYNAMIC_MAP_INSERT(dynamic_map.lt_entry_map, dyf_lt_ent,
                                            dyf_lt_ent_tail);
                    dyf_lt_ent = NULL;
                    f_lt_ent = NULL;
                 }
                 break;

            case _bcmFieldInternalEndStructEntryDetails:
                 break;
            default:
                 rv = BCM_E_INTERNAL;
                  LOG_ERROR(BSL_LS_BCM_FP,(BSL_META_U(unit,
                 "unknown type detected : _entry_recover\n")));
                 goto cleanup;


        }
    }

    TLV_INIT(tlv);
    return BCM_E_NONE;

cleanup:
    TLV_INIT(tlv);
    if (f_lt_ent != NULL) {
        sal_free(f_lt_ent);
        f_lt_ent = NULL;
    }
    if (dyf_lt_ent != NULL) {
        sal_free(dyf_lt_ent);
        dyf_lt_ent = NULL;
    }
    if (f_ent != NULL) {
        sal_free(f_ent);
        f_ent = NULL;
    }
    if (dyf_ent != NULL) {
        sal_free(dyf_ent);
        dyf_ent = NULL;
    }
    return rv;

}


/* Function:
 * _field_group_recover
 *
 * Purpose:
 *    recover _field_group_t structure
 *
 * Paramaters:
 * unit          - (IN) BCM device number
 * fg            - (IN) Pointer to device group structure
 *
 * Returns:
 *     BCM_E_XXX
 */

int
_field_group_recover(int unit, _field_group_t *fg)
{
    _field_tlv_t tlv;                 /* Field TLV structure */
    uint32 *position = NULL;          /* Position in Scache */
    uint8 *scache_ptr = NULL;         /* Pointer to Scache */
    _field_control_t  *fc;            /* Field control structure.*/
    int mem_sz = 0;                   /* memeory to be allocated to entry_arr */
    _field_stage_t *stage_fc = NULL;  /* Field Stage structure */
    soc_profile_mem_t *keygen_profile;/* Key gen program profile memory */
    int part_count = 1;               /* Parts for Field entry in group */
    int i = 0;                        /* Local variable */

    BCM_IF_ERROR_RETURN(_field_stage_control_get(unit,
                            _BCM_FIELD_STAGE_INGRESS, &stage_fc));
    WB_FIELD_CONTROL_GET(fc, scache_ptr, position);
    tlv.value = NULL;
    TLV_INIT(tlv);


    while (tlv.type != _bcmFieldInternalEndStructGroup) {
        TLV_INIT(tlv);
        BCM_IF_ERROR_RETURN(tlv_read(unit, &tlv, scache_ptr, position));

        switch (tlv.type)
        {
            case _bcmFieldInternalGroupId:
                 fg->gid = *(bcm_field_group_t *)tlv.value;
                 break;
            case _bcmFieldInternalGroupPri:
                 fg->priority = *(int *)tlv.value;
                 break;
            case _bcmFieldInternalQsetW:
                 sal_memcpy(fg->qset.w, tlv.value,
                      ((static_type_map[tlv.type].size) * tlv.length));
                 break;
            case _bcmFieldInternalQsetUdfMap:
                 sal_memcpy(fg->qset.udf_map, tlv.value,
                      ((static_type_map[tlv.type].size) * tlv.length));
                 break;
            case _bcmFieldInternalGroupFlags:
                 fg->flags = *(uint16 *)tlv.value;
                 break;
            case _bcmFieldInternalGroupPbmp:
                 sal_memcpy(&fg->pbmp, tlv.value,
                             ((static_type_map[tlv.type].size) * tlv.length));
                 break;
            case _bcmFieldInternalGroupSlice:
                 fg->slices =
                     &(stage_fc->slices[fg->instance][*(int *)tlv.value]);
                 break;
            case _bcmFieldInternalGroupQual:
                 BCM_IF_ERROR_RETURN(_field_groupqual_recover(unit,
                                                                &tlv, fg));
                 break;
            case _bcmFieldInternalGroupStage:
                 fg->stage_id = * (_field_stage_id_t *)tlv.value;
                 break;
            case _bcmFieldInternalGroupEntry:
                 break;
            case _bcmFieldInternalGroupBlockCount:
                 fg->ent_block_count = *(uint16 *)tlv.value;
                 break;
            case _bcmFieldInternalGroupGroupStatus:
                 sal_memcpy(&(fg->group_status), tlv.value,
                      ((static_type_map[tlv.type].size) * tlv.length));
                 break;
            case _bcmFieldInternalGroupGroupAset:
                 sal_memcpy(&(fg->aset.w), tlv.value,
                      ((static_type_map[tlv.type].size) * tlv.length));
                 break;
            case _bcmFieldInternalGroupCounterBmp:
                 sal_memcpy(fg->counter_pool_bmp.w, tlv.value,
                      ((static_type_map[tlv.type].size) * tlv.length));
                 break;
            case _bcmFieldInternalGroupInstance:
                 fg->instance = *(int *)tlv.value;
                 break;
            case _bcmFieldInternalExtractor:
                 BCM_IF_ERROR_RETURN(_field_extractor_recover(unit,
                                    scache_ptr, position, &tlv, fg->ext_codes));
                 break;
            case _bcmFieldInternalGroupLtSlice:
                 fg->lt_slices =
                       &(stage_fc->lt_slices[fg->instance][*(int *)tlv.value]);
                 break;
            case _bcmFieldInternalGroupLtConfig:
                 fg->lt_info =
                     stage_fc->lt_info[fg->instance][*(int *)tlv.value];
                 break;
            case _bcmFieldInternalGroupLtEntry:
                 /* backlink  function will handle */
                 break;
            case _bcmFieldInternalGroupLtEntrySize:
                 fg->lt_ent_blk_cnt = *(uint16 *)tlv.value;
                 break;
            case _bcmFieldInternalLtStatusEntriesFree:
                 fg->lt_grp_status.entries_free = *(int *)tlv.value;
                 break;
            case _bcmFieldInternalLtStatusEntriesTotal:
                 fg->lt_grp_status.entries_total = *(int *)tlv.value;
                 break;
            case _bcmFieldInternalLtStatusEntriesCnt:
                 fg->lt_grp_status.entry_count = *(int *)tlv.value;
                 break;
            case _bcmFieldInternalGroupQsetSize:
                 fg->qset_size = *(int *)tlv.value;
                 break;
            case _bcmFieldInternalGroupHintId:
                 fg->hintid = *(bcm_field_hintid_t *)tlv.value;
                 break;
            case _bcmFieldInternalGroupMaxSize:
                 fg->max_group_size = *(uint32 *)tlv.value;
                 break;
            case _bcmFieldInternalEndStructGroup:
                 if (*(uint32 *)tlv.value != _FIELD_WB_EM_GROUP) {
                        LOG_ERROR(BSL_LS_BCM_FP,(BSL_META_U(unit,
                                        "END MARKER CHECK FAILED : GROUP\n")));
                     return BCM_E_INTERNAL;
                 }
                 break;

            default:
                 TLV_INIT(tlv);
                 LOG_ERROR(BSL_LS_BCM_FP,(BSL_META_U(unit,
                 "unknown type detected : _group_recover\n")));
                 return BCM_E_INTERNAL;

        }
    }

    fg->action_res_id = -1;

    /* Key gen program profile memory is set with correct reference count */
    keygen_profile = &stage_fc->keygen_profile[fg->instance].profile;

    if (fg->flags & _FP_GROUP_SPAN_TRIPLE_SLICE) {
        part_count = 3;
    } else if (fg->flags & _FP_GROUP_SPAN_DOUBLE_SLICE) {
        part_count = 2;
    } else {
        part_count = 1;
    }

    for (i = 0; i < part_count; i++) {
        SOC_PROFILE_MEM_REFERENCE(unit, keygen_profile,
                              fg->ext_codes[i].keygen_index, 1);
        SOC_PROFILE_MEM_ENTRIES_PER_SET(unit, keygen_profile,
                                          fg->ext_codes[i].keygen_index, 1);
    }

    /* Allocate memory for entries in group */
    mem_sz = _FP_GROUP_ENTRY_ARR_BLOCK * (fg->ent_block_count) *
                        sizeof(_field_entry_t *);
    _FP_XGS3_ALLOC(fg->entry_arr, mem_sz, "entry block for fp group");

    mem_sz = _FP_GROUP_ENTRY_ARR_BLOCK * (fg->lt_ent_blk_cnt + 1)
                    * sizeof(_field_lt_entry_t *);
    _FP_XGS3_ALLOC(fg->lt_entry_arr, mem_sz, "entry block for fp group");


    return BCM_E_NONE;
}

/* Function:
 * _field_backlink_data
 *
 * Purpose:
 *    backlink entries and lt entries from slice and group structures.
 *
 * Paramaters:
 * unit          - (IN) BCM device number
 * stage_fc      - (IN) Pointer to stage structure.
 * group_count   - (IN) Number of groups present
 *
 * Description:
 *  Entries are linked with their groups and slices at the time of
 *  entry_info_recover, and are stored temporarily in dynamic_map
 *  structure. In this function, these entries are picked up and are
 *  added into group->entry_arr and slice->entries.
 *
 * Returns:
 *     BCM_E_XXX
 */

int
_field_backlink_data(int unit, _field_stage_t *stage_fc, int max_grp_id)
{
    _field_dynamic_map_entry_t *dyf_ent= NULL;
                                  /* pointer to entry in dynamic map */
    _field_dynamic_map_lt_entry_t *dyf_lt_ent = NULL;
                                  /* pointer to LT entry in dynamic map */
    _field_group_t *fg = NULL;    /* Field group structure */
    _field_control_t *fc = NULL;  /* pointer to field control structure */
    _field_entry_t *f_ent = NULL; /* Field Entry structure */
    _field_lt_entry_t *f_lt_ent = NULL;
                                  /* Field LT entry structure */
    int *group_pos = NULL;        /* Group Position */
    int parts_count = 0, part = 0;/* Parts in Field entry */
    int rv = BCM_E_NONE;          /* Return  Variable */
    bcm_field_group_t current_gid = -1;
                                  /* Group id where entry belongs */

    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    dyf_ent = dynamic_map.entry_map;
    if (dyf_ent != NULL) {
        current_gid = dyf_ent->entry->group->gid;
    }
    fg = fc->groups;

    /* Group id's range from 0 to max_grp_id */
    _FP_XGS3_ALLOC(group_pos, (max_grp_id + 1) * sizeof(int),
                   "group count array");
    if (group_pos == NULL) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }

    while (dyf_ent != NULL)
    {
        /* Make sure that the current entry belongs to current group */
        if (fg->gid != current_gid) {
            fg = fc->groups;

            /* Find group where entry belongs */
            while (fg != NULL) {
                fg = fg->next;
                if (fg->gid == current_gid) {
                    break;
                }
            }
            if (fg == NULL) {
                 LOG_ERROR(BSL_LS_BCM_FP,(BSL_META_U(unit,
                                 "entry backlink failed\n")));
                rv = BCM_E_INTERNAL;
                goto cleanup;
            }
        }

        BCM_IF_ERROR_RETURN(_bcm_field_entry_tcam_parts_count (unit,
                            fg->stage_id, fg->flags, &parts_count));
        /* Link group to entry */
        fg->entry_arr[group_pos[current_gid]] = dyf_ent->entry;
        group_pos[current_gid]++;
        f_ent = dyf_ent->entry;
        for (part = 0; part < parts_count; part ++)
        {
            f_ent[part].fs->entries[f_ent[part].slice_idx] = f_ent + part;
        }

        dyf_ent = dyf_ent->next;
        if (dyf_ent != NULL) {
            current_gid = dyf_ent->entry->group->gid;
            parts_count = 0;
        }
    }

    dyf_lt_ent = dynamic_map.lt_entry_map;
    if (dyf_lt_ent != NULL) {
        current_gid = dyf_lt_ent->lt_entry->group->gid;
    }
    fg = fc->groups;
    sal_memset(group_pos, 0,  (max_grp_id  + 1) * sizeof(int));

    while (dyf_lt_ent != NULL) {

        /* Make sure that the current entry belongs to current group */
        if (fg->gid != current_gid) {
            fg = fc->groups;

            /* Find group where entry belongs */
            while (fg != NULL) {
                fg = fg->next;
                if (fg->gid == current_gid) {
                    break;
                }
            }
            if (fg == NULL) {
                 LOG_ERROR(BSL_LS_BCM_FP,(BSL_META_U(unit,
                                 "lt entry backlink failed\n")));
                rv = BCM_E_INTERNAL;
                goto cleanup;
            }
        }
        BCM_IF_ERROR_RETURN(_bcm_field_entry_tcam_parts_count (unit,
                            fg->stage_id, fg->flags, &parts_count));

        /* Link group to LT entry */
        fg->lt_entry_arr[group_pos[current_gid]] = dyf_lt_ent->lt_entry;
        group_pos[current_gid]++;
        f_lt_ent = dyf_lt_ent->lt_entry;
        for (part = 0; part < parts_count; part ++)
        {
             f_lt_ent[part].lt_fs->entries[f_lt_ent[part].index] = f_lt_ent;
        }

        dyf_lt_ent = dyf_lt_ent->next;
        if (dyf_lt_ent != NULL) {
            current_gid = dyf_lt_ent->lt_entry->group->gid;
            parts_count = 0;
        }
    }

cleanup:
    if (group_pos != NULL) {
        sal_free(group_pos);
        group_pos = NULL;
    }
    return rv;

}



/*
 * Function:
 *  _bcm_field_th_stage_ingress_reinit
 * Description:
 *  Service routine used to retain the software sate of
 *  IFP(Ingress Field Processor) configuration .
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
_bcm_field_th_stage_ingress_reinit(int unit,
                                   _field_control_t *fc,
                                   _field_stage_t   *stage_fc)
{
    int    rv = BCM_E_NONE;                 /* Operation return value  */
    uint8  *scache_ptr;                     /* Pointer to 1st scache part*/
    uint32 temp;                            /* Temporary variable   */
    _field_group_t **new_fg,*curr_fg, *prev_fg;
                                            /* For Group LL */
    uint32 slice_id,pipe_id,lt_id,group_id; /* Iterator varaiables */
    struct _field_slice_s *curr_slice;      /* Slice information */
    struct _field_lt_slice_s *curr_ltslice; /* LT slice information */
    _field_tlv_t tlv;                       /* TLV data */
    int  num_pipes = 0;                     /* Number of pipes */
    int group_count = 0, max_grp_id = 0;

	tlv.value = NULL;
    new_fg = NULL;
    prev_fg = curr_fg = NULL;

    FP_LOCK(fc);
    TLV_INIT(tlv);
    scache_ptr = fc->scache_ptr[_FIELD_SCACHE_PART_0];

    fc->scache_pos = 0;
    fc->scache_pos += SOC_WB_SCACHE_CONTROL_SIZE;

    while (tlv.type != _bcmFieldInternalEndStructIFP) {
        TLV_INIT(tlv);
        BCM_IF_ERROR_RETURN(tlv_read(unit, &tlv, scache_ptr,
                                     &(fc->scache_pos)));


    switch (tlv.type) {
        case _bcmFieldInternalControl:
             /* _field_control_t */
             LOG_DEBUG(BSL_LS_BCM_FP,(BSL_META_U(unit,"FP(unit %d):"
                                                 "_bcm_field_th_stage"
                                                 "_ingress_reinit"
                                                 " recovering _field_control_t "
                                                 " from pos = %d\r\n"),unit,
                                                 fc->scache_pos));
             rv = _field_control_recover(unit,fc);
             BCM_IF_ERROR_CLEANUP(rv);
             break;
        case _bcmFieldInternalStage:
             /*_field_stage_t */
             LOG_DEBUG(BSL_LS_BCM_FP,(BSL_META_U(unit,"FP(unit %d):"
                                                 "_bcm_field_th_stage"
                                                 "_ingress_reinit"
                                                 " recovering _field_stage_t "
                                                 " from pos = %d\r\n"),unit,
                                                 fc->scache_pos));
             rv = _field_stage_recover(unit,&stage_fc);
             BCM_IF_ERROR_CLEANUP(rv);
             if (stage_fc->oper_mode == bcmFieldGroupOperModeGlobal) {
                 num_pipes = 1;
             } else if (stage_fc->oper_mode == bcmFieldGroupOperModePipeLocal) {
                 num_pipes = (_FP_MAX_NUM_PIPES);
             }

             break;
        case _bcmFieldInternalSlice:
             /* _field_slice_t and _field_lt_slice_t */
             LOG_DEBUG(BSL_LS_BCM_FP,(BSL_META_U(unit,"FP(unit %d):"
                                                 "_bcm_field_th_stage"
                                                 "_ingress_reinit -"
                                                 "recovering _field_slice_t,"
                                                 "_field_lt_slice_t "
                                                 "from pos = %d\r\n"),
                                                 unit,fc->scache_pos));

             for (pipe_id = 0; pipe_id < num_pipes; pipe_id++) {
                 curr_slice = stage_fc->slices[pipe_id];
                 curr_ltslice = stage_fc->lt_slices[pipe_id];

                 if (curr_slice != NULL) {
                     for (slice_id= 0; slice_id < stage_fc->tcam_slices;
                         slice_id ++) {

                          rv = _field_slice_recover(unit, pipe_id,
                                                   (curr_slice + slice_id));
                          BCM_IF_ERROR_CLEANUP(rv);
                     }
                 } else {
                     LOG_ERROR(BSL_LS_BCM_FP,(BSL_META_U(unit,
                               "Memory not allocated for slice\n")));
                 }




                 if (curr_ltslice != NULL) {
                     for (lt_id = 0; lt_id < stage_fc->tcam_slices; lt_id++) {
                          rv = _field_lt_slice_recover(unit, pipe_id,
                                                        (curr_ltslice
                                                        + lt_id));
                          BCM_IF_ERROR_CLEANUP(rv);
                     }
                 }else {
                     LOG_ERROR(BSL_LS_BCM_FP,(BSL_META_U(unit,
                               "Memory not allocated for LT slice\n")));
                 }

             }
             break;
        case _bcmFieldInternalGroupCount:
             /* _field_group_t */
             LOG_DEBUG(BSL_LS_BCM_FP,(BSL_META_U(unit,"FP(unit %d):"
                                                 "_bcm_field_th_stage"
                                                 "_ingress_reinit -"
                                                 "recovering _field_group_t "
                                                 "from pos = %d\r\n"),unit,
                                                 fc->scache_pos));
             new_fg = NULL;
             prev_fg = NULL;
             group_count = *(int *)tlv.value;

             /* Iterate over the groups */
             for (group_id =0 ;group_id < group_count; group_id ++) {
                 curr_fg = NULL;
                 _FP_XGS3_ALLOC(curr_fg, sizeof(_field_group_t),
                                "current field grp");
                 rv = _field_group_recover(unit, curr_fg);
                 BCM_IF_ERROR_CLEANUP(rv);

                 if (prev_fg == NULL) {
                     prev_fg = curr_fg;
                     fc->groups = prev_fg;
                     max_grp_id = curr_fg->gid;
                 } else {
                     prev_fg->next = curr_fg;
                     prev_fg = curr_fg;
                     max_grp_id =
                         ((max_grp_id > (curr_fg->gid))
                          ? max_grp_id:(curr_fg->gid));
                 }
             }
             /* Group id's range from 0 to max_grp_id */
             _FP_XGS3_ALLOC(new_fg, (max_grp_id+1) *sizeof(_field_group_t *),
                            "group alloc");

             curr_fg = fc->groups;
             while (curr_fg != NULL) {
                 new_fg[(curr_fg->gid)] = curr_fg;
                 curr_fg = curr_fg->next;
             }
             /* _field_entry_t and _field_lt_entry_t */
             LOG_DEBUG(BSL_LS_BCM_FP,(BSL_META_U(unit,"FP(unit %d):"
                                      "_bcm_field_th_stage_ingress_reinit -"
                                      "recovering _field_entry_t,"
                                      "_field_lt_entry_t from pos = %d\r\n"),
                                                 unit,fc->scache_pos));

             rv = _field_entry_recover(unit, new_fg);
             BCM_IF_ERROR_CLEANUP(rv);
             break;
        case _bcmFieldInternalEndStructIFP:
        LOG_DEBUG(BSL_LS_BCM_FP,(BSL_META_U(unit,"FP(unit %d):"
                                 "_bcm_field_th_stage_ingress_reinit -"
                                 "End of structures recover "
                                 "at pos = %d\r\n"),unit, fc->scache_pos));
        break;
        default:
             rv = BCM_E_INTERNAL;
             goto cleanup;
        break;
        }
    }


    temp = 0;
    temp |= scache_ptr[fc->scache_pos];
    fc->scache_pos++;
    temp |= scache_ptr[fc->scache_pos] << 8;
    fc->scache_pos++;
    temp |= scache_ptr[fc->scache_pos] << 16;
    fc->scache_pos++;
    temp |= scache_ptr[fc->scache_pos] << 24;
    fc->scache_pos++;
    if (temp != _FIELD_IFP_DATA_END) {
        fc->l2warm = 0;
        rv = BCM_E_INTERNAL;
    }
    LOG_DEBUG(BSL_LS_BCM_FP,(BSL_META_U(unit,"FP(unit %d):"
                                        "_bcm_field_th_stage_ingress_reinit -"
                                        "End of IFP Section "
                                        "at pos = %d\r\n"),unit,
                                        fc->scache_pos));
    if ((dynamic_map.entry_map != NULL) ||
       (dynamic_map.lt_entry_map != NULL)) {
        rv =_field_backlink_data(unit, stage_fc, max_grp_id);
    }

cleanup:
    FP_UNLOCK(fc);
    if (new_fg != NULL) {
        sal_free(new_fg);
    }
    dynamic_map_free(dynamic_map);

    return rv;
 }


#endif /* BCM_TOMAHAWK_SUPPORT */
#endif /* BCM_WARM_BOOT_SUPPORT */
