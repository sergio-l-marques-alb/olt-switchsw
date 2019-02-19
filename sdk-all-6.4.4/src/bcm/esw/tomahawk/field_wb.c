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


#ifdef BCM_WARM_BOOT_SUPPORT
#ifdef BCM_TOMAHAWK_SUPPORT
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
    "_bcmFieldInternalActionId",
    "_bcmFieldInternalActionParam",
    "_bcmFieldInternalActionHwIndex",
    "_bcmFieldInternalActionOldIndex",
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
    "_bcmFieldInternalEndStructEntrydetails",
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
    "_bcmFieldInternalControlStage",
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
    "_bcmFieldInternalStatnStat1",
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
    "_bcmfielditnernalDataControlDataQualHwBmp",
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
    { _bcmFieldInternalExtractor,           4,  0x0},
    { _bcmFieldInternalGroup,               0,  _FP_WB_TLV_NO_VALUE
                                                | _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalGroupId,             4,  0x0},
    { _bcmFieldInternalGroupPri,            4,  0x0},
    { _bcmFieldInternalGroupQset,           16, _FP_WB_TLV_NO_VALUE},
    { _bcmFieldInternalQsetW,               8,  _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalQsetUdfMap,          8,  _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalGroupFlags,          2,  0x0},
    { _bcmFieldInternalGroupPbmp,           4,  _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalGroupSlice,          4,  0x0},
    { _bcmFieldInternalGroupQual,           12, _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalQualQid,             4,  _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalQualOffset,          4,  _FP_WB_TLV_NO_VALUE
                                                | _FP_WB_TLV_LEN_ENCODED
                                                | _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalQualSize,            4,  0x0},
    { _bcmFieldInternalGroupStage,          4,  0x0},
    { _bcmFieldInternalGroupEntry,          4,  _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalGroupBlockCount,     2,  0x0},
    { _bcmFieldInternalGroupGroupStatus,    0,  _FP_WB_TLV_NO_VALUE},
    { _bcmFieldInternalGroupGroupAset,      4,  0x0},
    { _bcmFieldInternalGroupCounterBmp,     4,  0x0},
    { _bcmFieldInternalGroupInstance,       4,  0x0},
    { _bcmFieldInternalGroupExtCodes,       112,_FP_WB_TLV_NO_VALUE
                                                | _FP_WB_TLV_LEN_ENCODED
                                                | _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalGroupLtSlice,        4,  0x0},
    { _bcmFieldInternalGroupLtConfig,       4,  0x0},
    { _bcmFieldInternalGroupLtEntry,        4,  _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalGroupLtEntrySize,    2,  0x0},
    { _bcmFieldInternalGroupLtEntryStatus,  12, _FP_WB_TLV_NO_VALUE},
    { _bcmFieldInternalLtStatusEntriesFree, 4,  0x0},
    { _bcmFieldInternalLtStatusEntriesTotal,4,  0x0},
    { _bcmFieldInternalLtStatusEntriesCnt,  4,  0x0},
    { _bcmFieldInternalGroupQsetSize,       4,  0x0},
    { _bcmFieldInternalGroupHintId,         4,  0x0},
    { _bcmFieldInternalGroupMaxSize,        4,  0x0},
    { _bcmFieldInternalEndStructGroup,      4,  0x0},
    { _bcmFieldInternalQualOffsetField,     4,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalQualOffsetNumOffset, 1,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalQualOffsetOffsetArr, 128,_FP_WB_TLV_NO_TYPE
                                                | _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalQualOffsetWidth,     64, _FP_WB_TLV_NO_TYPE
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
    { _bcmFieldInternalEntryAction,         4,  _FP_WB_TLV_NO_VALUE
                                                | _FP_WB_TLV_LEN_ENCODED
                                                | _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalEntrySlice,          1,  0x0},
    { _bcmFieldInternalEntryGroup,          4,  0x0},
    { _bcmFieldInternalEntryStat,           8,  _FP_WB_TLV_NO_VALUE
                                                |  _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalEntryPolicer,        16, _FP_WB_TLV_NO_VALUE
                                                | _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalEntryIngMtp,         1,  0x0},
    { _bcmFieldInternalEntryEgrMtp,         1,  0x0},
    { _bcmFieldInternalEntryDvp,            1,  0x0},
    { _bcmFieldInternalEntryCopy,           1,  0x0},
    { _bcmFieldInternalEntryCopyTypePointer,0,  0x0},
    { _bcmFieldInternalPbmpData,            4,  0x0},
    { _bcmFieldInternalPbmpMask,            4,  0x0},
    { _bcmFieldInternalActionId,            4,  0x0},
    { _bcmFieldInternalActionParam,         4,  _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalActionHwIndex,       4,  0x0},
    { _bcmFieldInternalActionOldIndex,      4,  0x0},
    { _bcmFieldInternalActionFlags,         1,  0x0},
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
    { _bcmFieldInternalExtKeygenIndex,      1,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalLtEntrySlice,        4,  0x0},
    { _bcmFieldInternalLtEntryIndex,        4,  0x0},
    { _bcmFieldInternalGroupCount,          4,  0x0},
    { _bcmFieldInternalSliceCount,          4,  0x0},
    { _bcmFieldInternalLtSliceCount,        4,  0x0},
    { _bcmFieldInternalEndStructGroupQual,  4,  0x0},
    { _bcmFieldInternalEndStructQualOffset, 4,  0x0},
    { _bcmFieldInternalEndStructEntrydetails,4, _FP_WB_TLV_NO_VALUE},
    { _bcmFieldInternalEndStructEntry,      4,  0x0},
    { _bcmFieldInternalEntryPbmpData,       4,  0x0},
    { _bcmFieldInternalEntryPbmpMask,       4,  0x0},
    { _bcmFieldInternalEndStructAction,     4,  0x0},
    { _bcmFieldInternalGlobalEntryPolicer,  4,  0x0},
    { _bcmFieldInternalGlobalEntryPolicerPid,4, 0x0},
    { _bcmFieldInternalGlobalEntryPolicerFlags,4,0x0},
    { _bcmFieldInternalEndGroupExtractor,   4,  0x0},
    { _bcmFieldInternalControl,             4,  _FP_WB_TLV_NO_VALUE},
    { _bcmFieldInternalControlFlags,        1,  0x0},
    { _bcmFieldInternalControlStage,        4,  0x0},
    { _bcmFieldInternalControlGroups,       4,  _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalControlStages,       4,  0x0},
    { _bcmFieldInternalControlPolicerHash,  4,  _FP_WB_TLV_NO_VALUE
                                                | _FP_WB_TLV_LEN_ENCODED
                                                | _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalControlPolicerCount, 4,  0x0},
    { _bcmFieldInternalControlStathash,    4,   _FP_WB_TLV_NO_VALUE
                                                | _FP_WB_TLV_LEN_ENCODED
                                                | _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalControlStatCount,    4,  0x0},
    { _bcmFieldInternalControlHintBmp,      4,  _FP_WB_TLV_LEN_PRESENT},
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
    { _bcmFieldInternalStatHwIndex,         1,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalStatHwFlags,         4,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalStatnStat1,          0,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalStatnStat,           1,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalStatArr,             4,  _FP_WB_TLV_NO_TYPE
                                                | _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalStatHwStat,          4,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalStatHwMode,          1,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalStatHwEntryCount,    1,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalStatGid,             4,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalStatStageId,         4,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalStatStatValues,      8,  _FP_WB_TLV_NO_TYPE
                                                | _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalStatFlexMode,        4,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalEndStructStat,       4,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalHintHintid,          4,  0x0},
    { _bcmFieldInternalHintHints,           0,  _FP_WB_TLV_NO_VALUE
                                                | _FP_WB_TLV_LEN_ENCODED
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
    { _bcmFieldInternalControlEndStructUdf, 4,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalHintHintsHintType,   4,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalHintHintsQual,       4,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalHintHintsMaxValues,  4,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalHintHintsStartBit,   4,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalHintHintsEndBit,     4,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalHintHintsFlags,      4,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalHintHintsMaxGrpSize, 4,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalEndStructHintHints,  4,  0xc},
    { _bcmFieldInternalStageStageid,        4,  0x0},
    { _bcmFieldInternalStageFlags,          4,  0x0},
    { _bcmFieldInternalStageTcamSz,         4,  0x0},
    { _bcmFieldInternalStageTcamSlices,     4,  0x0},
    { _bcmFieldInternalStageNumInstances,   4,  0x0},
    { _bcmFieldInternalStageNumPipes,       4,  0x0},
    { _bcmFieldInternalStageRanges,         4,  _FP_WB_TLV_NO_VALUE
                                                | _FP_WB_TLV_LEN_ENCODED 
                                                | _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalStageRangeId,        4,  0x0},
    { _bcmFieldInternalStageNumMeterPools,  4,  0x0},
    { _bcmFieldInternalStageMeterPool,      4,  _FP_WB_TLV_NO_VALUE
                                                | _FP_WB_TLV_LEN_ENCODED
                                                | _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalStageNumCntrPools,   4,  _FP_WB_TLV_NO_VALUE
                                                | _FP_WB_TLV_LEN_ENCODED
                                                | _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalStageCntrPools,      4, _FP_WB_TLV_NO_VALUE
                                                | _FP_WB_TLV_LEN_ENCODED
                                                | _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalStageLtTcamSz,       4,  0x0},
    { _bcmFieldInternalStageNumLogicalTables,4, 0x0},
    { _bcmFieldInternalStageLtInfo,         4,  0x0},
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
    { _bcmFieldInternalMeterBmp,            4,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalCntrSliceId,         1,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalCntrSize,            2,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalCntrFreeCntrs,       2,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalCntrBmp,             4,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalLtConfigValid,       4,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalLtConfigLtId,        4,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalLtConfigLtPartPri,   4,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalLtConfigLtPartMap,   4,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalLtConfigLtActionPri, 4,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalLtConfigPri,         4,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalLtConfigFlags,       4,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalLtConfigEntry,       4,  _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalEndStructRanges,     4,  _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalEndStructMeter,      4,  _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalEndStructCntr,       4,  _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalEndStructLtConfig,   4,  _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalSlice,               0,  _FP_WB_TLV_NO_VALUE
                                                |  _FP_WB_TLV_LEN_PRESENT},
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
    { _bcmFieldInternalSlicePortPbmp,       4,  _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalSliceEntriesInfo,    4,  _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalSliceNextSlice,      4,  0x0},
    { _bcmFieldInternalSlicePrevSlice,      4,  0x0},
    { _bcmFieldInternalSliceFlags,          1,  0x0},
    { _bcmFieldInternalSliceGroupFlags,     1,  0x0},
    { _bcmFieldInternalSliceLtMap,          4,  0x0},
    { _bcmFieldInternalEndStructSlice,      4,  _FP_WB_TLV_NO_VALUE},
    { _bcmFieldInternalEndStructExtractor,  0,  _FP_WB_TLV_NO_VALUE},
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
    { _bcmFieldInternalDataControlUsageBmp,         4, _FP_WB_TLV_LEN_PRESENT},
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
    { _bcmFieldInternalDataControlEndStructDataQual,4, _FP_WB_TLV_NO_VALUE},
    { _bcmFieldInternalDataControlEthertypeStruct,  0, _FP_WB_TLV_NO_VALUE
                                                       | _FP_WB_TLV_LEN_ENCODED
                                                      | _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalDataControlEthertypeRefCount,4, _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalDataControlEthertypeL2,      2, _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalDataControlEthertypeVlanTag, 2, _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalDataControlEthertypePortEt,  2, _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalDataControlEthertypeRelOffset,4,_FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalDataControlProtStart,        0, _FP_WB_TLV_NO_VALUE
                                                       | _FP_WB_TLV_LEN_ENCODED
                                                      | _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalDataControlProtIp4RefCount,  4, _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalDataControlProtIp6RefCount,  4, _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalDataControlProtFlags,        4, _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalDataControlProtIp,           1, _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalDataControlProtL2,           2, _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalDataControlProtVlanTag,      2, _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalDataControlProtRelOffset,    4, _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalDataControlTcamStruct,       512,_FP_WB_TLV_NO_VALUE
                                                        | _FP_WB_TLV_LEN_ENCODED
                                                      | _FP_WB_TLV_LEN_PRESENT},
    { _bcmFieldInternalDataControlTcamRefCount,     0,   _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalDataControlTcamPriority,     0,   _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalDataControlElemSize,         4, 0x0},
    { _bcmFieldInternalDataControlNumElem,          4, 0x0},
    { _bcmFieldInternalEndStructDataControl,        4, 0x0},
    { _bcmFieldInternalControlUdfValid,             1, _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalControlUdfUseCount,          4, _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalControlUdfNum,               4, _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalControlUdfUserNum,           1, _FP_WB_TLV_NO_TYPE},
    { _bcmFieldInternalControlUdfDetails,           0, _FP_WB_TLV_NO_VALUE
                                                       | _FP_WB_TLV_LEN_ENCODED
                                                      | _FP_WB_TLV_LEN_PRESENT},

};

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
_tlv_print_array(int unit, void *value, int length, int chunk_sz) {
    int i         = 0;     /* local variable for loop */
    uint8 *val8   = 0;     /* variable to hold 8 bit values */
    uint16 *val16 = 0;     /* variable to hold 16 bit values */
    uint32 *val32 = 0;     /* variable to hold 32 bit values */
    long long unsigned int *val64 = 0;
                           /* variable to hold 64 bit values */

    switch (chunk_sz) {
        case 2:
             val16 = (uint16 *)value;
             for (i=0; i < length/chunk_sz; i++) {
                  LOG_DEBUG(BSL_LS_BCM_FP,
                   (BSL_META_U(unit, "TLV Value array: %x\n"), val16[i]));
             }
             break;
        case 4:
             val32 = (uint32 *)value;
              for (i=0; i < length/chunk_sz; i++) {
                  LOG_DEBUG(BSL_LS_BCM_FP,
                   (BSL_META_U(unit, "TLV Value array: %x\n"), val32[i]));
              }
              break;
        case 8:
             val64 = (long long unsigned int *)value;
              for (i=0; i < length/chunk_sz; i++) {
                  LOG_DEBUG(BSL_LS_BCM_FP,
                   (BSL_META_U(unit, "TLV Value array: %llx\n"), val64[i]));
              }
              break;
        default:
                val8 = (uint8 *)value;
                for (i=0; i < length; i++) {
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

    if (tlv == NULL || tlv->value == NULL) {
        return BCM_E_PARAM;
    }
    pos = *position;

    elem = tlv->type;
    if ((elem != _bcmFieldInternalType) &&
        !(static_type_map[elem].flags & _FP_WB_TLV_NO_TYPE))
    {   data = (tlv->basic_type <<  28);
        data |= elem;
        LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit, "TLV Write Enum: %s\n"),
                                             elem_name[elem]));
        sal_memcpy(&ptr[pos], &data, sizeof(uint32));
        pos += sizeof(uint32);
    } else {
        LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit, "writing FOR Enum: %s\n"),
                  elem_name[tlv->type]));
    }

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
    }

    if (static_type_map[elem].flags & _FP_WB_TLV_LEN_ENCODED) {
        write_size = (((tlv->length) & 0xffc00000) >> 22) *
                        sizeof(tlv->type);
    }

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

    if (tlv->type == -1) {
        sal_memcpy(&data, &ptr[pos], sizeof(uint32));
        pos += sizeof(uint32);

        tlv->type = data & 0x0fffffff;
        tlv->basic_type = (data & 0xf0000000) >> 28;
        LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit, "TLV Read Enum: %s\n"),
                                             elem_name[tlv->type]));
    } else {
        LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
                  "Reading FOR Enum: %s\n"),
                   elem_name[tlv->type]));
    }

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

    }
    if (static_type_map[tlv->type].flags
        & _FP_WB_TLV_LEN_ENCODED) {
         data = ((tlv->length) & 0xffc00000) >> 22;
         read_size = data * sizeof(uint32);
    }

    if (read_size > 0) {
        tlv->value = sal_alloc(read_size, "wb read values");
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
    _field_tlv_t *tlv_new;   /* TLV data structure instance */
    int length_arr;          /* length of array */

    if (*tlv != NULL) {
       sal_free(*tlv);
    }
    tlv_new = sal_alloc(sizeof(_field_tlv_t),"tlv alloc");

    if (basic_type == _bcmFieldInternalVariable) {
        tlv_new->value = sal_alloc(static_type_map[type].size,
                               "value alloc for tlv");
    } else if (basic_type == _bcmFieldInternalArray) {
        if (static_type_map[type].flags & _FP_WB_TLV_LEN_ENCODED) {
            length_arr = ((length) & 0xffc00000) >> 22;
        } else {
            length_arr = length;
        }
        tlv_new->value = sal_alloc((static_type_map[type].size * length_arr),
                            "value alloc for tlv");
    } else {
        /* unknown type */
        sal_free(tlv_new);
        tlv_new = NULL;
        return BCM_E_INTERNAL;
    }

    tlv_new->type = type;
    tlv_new->basic_type = basic_type;
    tlv_new->length = length;

    *tlv = tlv_new;
    return BCM_E_NONE;
}
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
_field_hash_sync(int unit, uint8 *ptr, uint32 *position) {

    _field_tlv_t *tlv       = NULL;       /* tlv data */
    _field_control_t        *fc;          /* Field control structure.       */
    _field_policer_t        *f_pl;        /* Internal policer descriptor.   */
    _field_stat_t           *f_st;        /* Internal Stat descriptor */
    _field_hints_t          *f_ht;        /* Internal Hints Descriptor */
    int idx                 = 0;          /* Iterator */
    int policer_count       = 0;          /* number of policers */
    int stat_count          = 0;          /* number of stats */
    int hints_count         = 0;          /* number of hints */
    uint32 endmarkerpolicer = _BCM_FIELD_POLICER_END_MARKER;
                                          /* End marker for policer */
    int sizepolicer         = *position;  /* size of policer */
    uint32 endmarkerstat    = _BCM_FIELD_STAT_END_MARKER;
                                          /* End marker for stat */
    int sizestat            = *position;  /* size of stat */
    uint32 endmarkerhint    = _BCM_FIELD_HINT_END_MARKER;
                                          /* End marker for hint */
    int sizehint            = *position;  /* size of hint */
    _field_hint_t           *hint;        /* temporary field for hint */
    int hintidx             = 0;          /* hint id variable */

    int type_policer[22] = {              /* types in _field_policer_t */
    (int)((_bcmFieldInternalVariable << 28)
           | _bcmFieldInternalPolicerPid),
    (int)((_bcmFieldInternalVariable << 28)
           | _bcmFieldInternalPolicerCfgFlags),
    (int)((_bcmFieldInternalVariable << 28)
           | _bcmFieldInternalPolicerCfgMode),
    (int)((_bcmFieldInternalVariable << 28)
           | _bcmFieldInternalPolicerCfgCkbitsSec),
    (int)((_bcmFieldInternalVariable << 28)
           | _bcmFieldInternalPolicerCfgMaxCkbitsSec),
    (int)((_bcmFieldInternalVariable << 28)
           | _bcmFieldInternalPolicerCfgCkbitsBurst),
    (int)((_bcmFieldInternalVariable << 28)
           | _bcmFieldInternalPolicerCfgPkbitsSec),
    (int)((_bcmFieldInternalVariable << 28)
           | _bcmFieldInternalPolicerCfgMaxPkbitsSec),
    (int)((_bcmFieldInternalVariable << 28)
           | _bcmFieldInternalPolicerCfgPkbitsBurst),
    (int)((_bcmFieldInternalVariable << 28)
           | _bcmFieldInternalPolicerCfgKbitsCurrent),
    (int)((_bcmFieldInternalVariable << 28)
           | _bcmFieldInternalPolicerCfgActionId),
    (int)((_bcmFieldInternalVariable << 28)
           | _bcmFieldInternalPolicerCfgSharingMode),
    (int)((_bcmFieldInternalVariable << 28)
           | _bcmFieldInternalPolicerCfgEntropyId),
    (int)((_bcmFieldInternalVariable << 28)
           | _bcmFieldInternalPolicerCfgPoolId),
    (int)((_bcmFieldInternalVariable << 28)
           | _bcmFieldInternalPolicerSwRefCount),
    (int)((_bcmFieldInternalVariable << 28)
           | _bcmFieldInternalPolicerHwRefCount),
    (int)((_bcmFieldInternalVariable << 28)
           | _bcmFieldInternalPolicerLevel),
    (int)((_bcmFieldInternalVariable << 28)
           | _bcmFieldInternalPolicerPoolIndex),
    (int)((_bcmFieldInternalVariable << 28)
           | _bcmFieldInternalPolicerHwIndex),
    (int)((_bcmFieldInternalVariable << 28)
           | _bcmFieldInternalPolicerHwFlags),
    (int)((_bcmFieldInternalVariable << 28)
           | _bcmFieldInternalPolicerStageId),
    (int)((_bcmFieldInternalVariable << 28)
           | _bcmFieldInternalEndStructPolicer)
    };

    int type_stat[17] = {                 /* types in _field_stat_t */
    (int)((_bcmFieldInternalVariable << 28)
           | _bcmFieldInternalStatsId),
    (int)((_bcmFieldInternalVariable << 28)
           | _bcmFieldInternalStatSwRefCount),
    (int)((_bcmFieldInternalVariable << 28)
           | _bcmFieldInternalStatHwRefCount),
    (int)((_bcmFieldInternalVariable << 28)
           | _bcmFieldInternalStatOffsetMode),
    (int)((_bcmFieldInternalVariable << 28)
           | _bcmFieldInternalStatPoolIndex),
    (int)((_bcmFieldInternalVariable << 28)
           | _bcmFieldInternalStatHwIndex),
    (int)((_bcmFieldInternalVariable << 28)
           | _bcmFieldInternalStatHwFlags),
    (int)((_bcmFieldInternalVariable << 28)
           | _bcmFieldInternalStatnStat),
    (int)((_bcmFieldInternalArray    << 28)
           | _bcmFieldInternalStatArr),
    (int)((_bcmFieldInternalVariable << 28)
           | _bcmFieldInternalStatHwStat),
    (int)((_bcmFieldInternalVariable << 28)
           | _bcmFieldInternalStatHwMode),
    (int)((_bcmFieldInternalVariable << 28)
           | _bcmFieldInternalStatHwEntryCount),
    (int)((_bcmFieldInternalVariable << 28)
           | _bcmFieldInternalStatGid),
    (int)((_bcmFieldInternalVariable << 28)
           | _bcmFieldInternalStatStageId),
    (int)((_bcmFieldInternalArray    << 28)
           | _bcmFieldInternalStatStatValues),
    (int)((_bcmFieldInternalVariable << 28)
           | _bcmFieldInternalStatFlexMode),
    (int)((_bcmFieldInternalVariable << 28)
           | _bcmFieldInternalEndStructStat)
    };
    int type_hint[8] =  {                 /* types in _field_hint_t */
    (int)((_bcmFieldInternalVariable << 28)
           | _bcmFieldInternalHintHintsHintType),
    (int)((_bcmFieldInternalVariable << 28)
           | _bcmFieldInternalHintHintsQual),
    (int)((_bcmFieldInternalVariable << 28)
           | _bcmFieldInternalHintHintsMaxValues),
    (int)((_bcmFieldInternalVariable << 28)
           | _bcmFieldInternalHintHintsStartBit),
    (int)((_bcmFieldInternalVariable << 28)
           | _bcmFieldInternalHintHintsEndBit),
    (int)((_bcmFieldInternalVariable << 28)
           | _bcmFieldInternalHintHintsFlags),
    (int)((_bcmFieldInternalVariable << 28)
           | _bcmFieldInternalHintHintsMaxGrpSize),
    (int) ((_bcmFieldInternalVariable << 28)
           | _bcmFieldInternalEndStructHintHints)
    };

    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));
    FP_LOCK(fc);

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

    /* Policer Hash */
    /* type addition */

    BCM_IF_ERROR_RETURN(_field_tlv_create(_bcmFieldInternalControlPolicerHash,
                                         _bcmFieldInternalArray,
                                         (policer_count | (22 << 22)), &tlv));
    tlv->value = &(type_policer);
    BCM_IF_ERROR_RETURN(tlv_write(unit,tlv,ptr,position));

    /* Iterate over all hash buckets. */
    for (idx = 0; idx < _FP_HASH_SZ(fc); idx++) {
        /* Iterate over entries in each bucket. */
        f_pl = fc->policer_hash[idx];
        while (NULL != f_pl) {
        if((f_pl->pid != -1)  && (f_pl->stage_id == _BCM_FIELD_STAGE_INGRESS)) {
            BCM_IF_ERROR_RETURN(
                 _field_tlv_create(_bcmFieldInternalPolicerPid,
                                   _bcmFieldInternalVariable,0, &tlv));
            tlv->value = &(f_pl->pid);
            BCM_IF_ERROR_RETURN(tlv_write(unit,tlv,ptr,position));
            BCM_IF_ERROR_RETURN(
                _field_tlv_create(_bcmFieldInternalPolicerCfgFlags,
                                  _bcmFieldInternalVariable,0, &tlv));
            tlv->value = &(f_pl->cfg.flags);
            BCM_IF_ERROR_RETURN(tlv_write(unit,tlv,ptr,position));
            BCM_IF_ERROR_RETURN(
                 _field_tlv_create(_bcmFieldInternalPolicerCfgMode,
                                   _bcmFieldInternalVariable,0, &tlv));
            tlv->value = &(f_pl->cfg.mode);
            BCM_IF_ERROR_RETURN(tlv_write(unit,tlv,ptr,position));
            BCM_IF_ERROR_RETURN(
                 _field_tlv_create(_bcmFieldInternalPolicerCfgCkbitsSec,
                                   _bcmFieldInternalVariable,0, &tlv));
            tlv->value = &(f_pl->cfg.ckbits_sec);
            BCM_IF_ERROR_RETURN(tlv_write(unit,tlv,ptr,position));
            BCM_IF_ERROR_RETURN(
                 _field_tlv_create(_bcmFieldInternalPolicerCfgMaxCkbitsSec,
                                   _bcmFieldInternalVariable,0, &tlv));
            tlv->value = &(f_pl->cfg.max_ckbits_sec);
            BCM_IF_ERROR_RETURN(tlv_write(unit,tlv,ptr,position));
            BCM_IF_ERROR_RETURN(
                 _field_tlv_create(_bcmFieldInternalPolicerCfgCkbitsBurst,
                                   _bcmFieldInternalVariable,0, &tlv));
            tlv->value = &(f_pl->cfg.ckbits_burst);
            BCM_IF_ERROR_RETURN(tlv_write(unit,tlv,ptr,position));
            BCM_IF_ERROR_RETURN(
                 _field_tlv_create(_bcmFieldInternalPolicerCfgPkbitsSec,
                                   _bcmFieldInternalVariable,0, &tlv));
            tlv->value = &(f_pl->cfg.pkbits_sec);
            BCM_IF_ERROR_RETURN(tlv_write(unit,tlv,ptr,position));
            BCM_IF_ERROR_RETURN(
                 _field_tlv_create(_bcmFieldInternalPolicerCfgMaxPkbitsSec,
                                   _bcmFieldInternalVariable,0, &tlv));
            tlv->value = &(f_pl->cfg.max_pkbits_sec);
            BCM_IF_ERROR_RETURN(tlv_write(unit,tlv,ptr,position));
            BCM_IF_ERROR_RETURN(
                 _field_tlv_create(_bcmFieldInternalPolicerCfgPkbitsBurst ,
                                   _bcmFieldInternalVariable,0, &tlv));
            tlv->value = &(f_pl->cfg.pkbits_burst);
            BCM_IF_ERROR_RETURN(tlv_write(unit,tlv,ptr,position));
            BCM_IF_ERROR_RETURN(
                 _field_tlv_create(_bcmFieldInternalPolicerCfgKbitsCurrent,
                                   _bcmFieldInternalVariable,0, &tlv));
            tlv->value = &(f_pl->cfg.kbits_current);
            BCM_IF_ERROR_RETURN(tlv_write(unit,tlv,ptr,position));
            BCM_IF_ERROR_RETURN(
                 _field_tlv_create(_bcmFieldInternalPolicerCfgActionId,
                                   _bcmFieldInternalVariable,0, &tlv));
            tlv->value = &(f_pl->cfg.action_id);
            BCM_IF_ERROR_RETURN(tlv_write(unit,tlv,ptr,position));
            BCM_IF_ERROR_RETURN(
                 _field_tlv_create(_bcmFieldInternalPolicerCfgSharingMode,
                                   _bcmFieldInternalVariable,0, &tlv));
            tlv->value = &(f_pl->cfg.sharing_mode);
            BCM_IF_ERROR_RETURN(tlv_write(unit,tlv,ptr,position));
            BCM_IF_ERROR_RETURN(
                _field_tlv_create(_bcmFieldInternalPolicerCfgEntropyId,
                                  _bcmFieldInternalVariable,0, &tlv));
            tlv->value = &(f_pl->cfg.entropy_id);
            BCM_IF_ERROR_RETURN(tlv_write(unit,tlv,ptr,position));
            BCM_IF_ERROR_RETURN(
                 _field_tlv_create(_bcmFieldInternalPolicerCfgPoolId,
                                   _bcmFieldInternalVariable,0, &tlv));
            tlv->value = &(f_pl->cfg.pool_id);
            BCM_IF_ERROR_RETURN(tlv_write(unit,tlv,ptr,position));
            BCM_IF_ERROR_RETURN(
                 _field_tlv_create(_bcmFieldInternalPolicerSwRefCount,
                                   _bcmFieldInternalVariable,0, &tlv));
            tlv->value = &(f_pl->sw_ref_count);
            BCM_IF_ERROR_RETURN(tlv_write(unit,tlv,ptr,position));
            BCM_IF_ERROR_RETURN(
                 _field_tlv_create(_bcmFieldInternalPolicerHwRefCount,
                                   _bcmFieldInternalVariable,0, &tlv));
            tlv->value = &(f_pl->hw_ref_count);
            BCM_IF_ERROR_RETURN(tlv_write(unit,tlv,ptr,position));
            BCM_IF_ERROR_RETURN(
                 _field_tlv_create(_bcmFieldInternalPolicerLevel,
                                   _bcmFieldInternalVariable,0, &tlv));
            tlv->value = &(f_pl->level);
            BCM_IF_ERROR_RETURN(tlv_write(unit,tlv,ptr,position));
            BCM_IF_ERROR_RETURN(
                _field_tlv_create(_bcmFieldInternalPolicerPoolIndex,
                                  _bcmFieldInternalVariable,0,&tlv));
            tlv->value = &(f_pl->pool_index);
            BCM_IF_ERROR_RETURN(tlv_write(unit,tlv,ptr,position));
            BCM_IF_ERROR_RETURN(
                 _field_tlv_create(_bcmFieldInternalPolicerHwIndex,
                                   _bcmFieldInternalVariable,0, &tlv));
            tlv->value = &(f_pl->hw_index);
            BCM_IF_ERROR_RETURN(tlv_write(unit,tlv,ptr,position));
            BCM_IF_ERROR_RETURN(
                 _field_tlv_create(_bcmFieldInternalPolicerHwFlags,
                                   _bcmFieldInternalVariable,0, &tlv));
            tlv->value = &(f_pl->hw_flags);
            BCM_IF_ERROR_RETURN(tlv_write(unit,tlv,ptr,position));
            BCM_IF_ERROR_RETURN(
                 _field_tlv_create(_bcmFieldInternalPolicerStageId,
                                   _bcmFieldInternalVariable,0, &tlv));
            tlv->value = &(f_pl->stage_id);
            BCM_IF_ERROR_RETURN(tlv_write(unit,tlv,ptr,position));

            sizepolicer = *position - sizepolicer;
            BCM_IF_ERROR_RETURN(
                 _field_tlv_create(_bcmFieldInternalEndStructPolicer,
                                   _bcmFieldInternalVariable,sizepolicer,
                                   &tlv));
            tlv->value = &endmarkerpolicer;
            BCM_IF_ERROR_RETURN(tlv_write(unit,tlv,ptr,position));

        }
            f_pl = f_pl->next;
        }
    }

    /* Stat Hash */
    sizestat = *position;

    /* types addition */
    BCM_IF_ERROR_RETURN(_field_tlv_create(_bcmFieldInternalControlStathash,
                                         _bcmFieldInternalVariable,(stat_count
                                          | (17 << 22)), &tlv));
    tlv->value = &(type_stat);
    BCM_IF_ERROR_RETURN(tlv_write(unit,tlv,ptr,position));

    /* Iterate over all hash buckets. */
    for (idx = 0; idx < _FP_HASH_SZ(fc); idx++) {
        /* Iterate over entries in each bucket. */
        f_st = fc->stat_hash[idx];
        while (NULL != f_st) {
        if((f_st->sid != -1)  && (f_st->stage_id == _BCM_FIELD_STAGE_INGRESS)) {

            BCM_IF_ERROR_RETURN(
                 _field_tlv_create(_bcmFieldInternalStatsId,
                                   _bcmFieldInternalVariable,0, &tlv));
            tlv->value = &(f_st->sid);
            BCM_IF_ERROR_RETURN(tlv_write(unit,tlv,ptr,position));
            BCM_IF_ERROR_RETURN(
                 _field_tlv_create(_bcmFieldInternalStatSwRefCount,
                                    _bcmFieldInternalVariable,0, &tlv));
            tlv->value = &(f_st->sw_ref_count);
            BCM_IF_ERROR_RETURN(tlv_write(unit,tlv,ptr,position));
            BCM_IF_ERROR_RETURN(
                 _field_tlv_create(_bcmFieldInternalStatHwRefCount,
                                    _bcmFieldInternalVariable,0, &tlv));
            tlv->value = &(f_st->hw_ref_count);
            BCM_IF_ERROR_RETURN(tlv_write(unit,tlv,ptr,position));
            BCM_IF_ERROR_RETURN(
                 _field_tlv_create(_bcmFieldInternalStatOffsetMode,
                                    _bcmFieldInternalVariable,0, &tlv));
            tlv->value = &(f_st->offset_mode);
            BCM_IF_ERROR_RETURN(tlv_write(unit,tlv,ptr,position));
            BCM_IF_ERROR_RETURN(
                 _field_tlv_create(_bcmFieldInternalStatPoolIndex,
                                    _bcmFieldInternalVariable,0, &tlv));
            tlv->value = &(f_st->pool_index);
            BCM_IF_ERROR_RETURN(tlv_write(unit,tlv,ptr,position));
            BCM_IF_ERROR_RETURN(
                 _field_tlv_create(_bcmFieldInternalStatHwIndex,
                                    _bcmFieldInternalVariable,0, &tlv));
            tlv->value = &(f_st->hw_index);
            BCM_IF_ERROR_RETURN(tlv_write(unit,tlv,ptr,position));
            BCM_IF_ERROR_RETURN(
                 _field_tlv_create(_bcmFieldInternalStatHwFlags,
                                    _bcmFieldInternalVariable,0, &tlv));
            tlv->value = &(f_st->hw_flags);
            BCM_IF_ERROR_RETURN(tlv_write(unit,tlv,ptr,position));
            BCM_IF_ERROR_RETURN(
                 _field_tlv_create(_bcmFieldInternalStatnStat,
                                    _bcmFieldInternalVariable,0, &tlv));
            tlv->value = &(f_st->nstat);
            BCM_IF_ERROR_RETURN(tlv_write(unit,tlv,ptr,position));
            BCM_IF_ERROR_RETURN(
                 _field_tlv_create(_bcmFieldInternalStatArr,
                                    _bcmFieldInternalArray,f_st->nstat,
                                    &tlv));
            tlv->value = (f_st->stat_arr);
            BCM_IF_ERROR_RETURN(tlv_write(unit,tlv,ptr,position));
            BCM_IF_ERROR_RETURN(
                 _field_tlv_create(_bcmFieldInternalStatHwStat,
                                    _bcmFieldInternalVariable,0, &tlv));
            tlv->value = &(f_st->hw_stat);
            BCM_IF_ERROR_RETURN(tlv_write(unit,tlv,ptr,position));
            BCM_IF_ERROR_RETURN(
                 _field_tlv_create(_bcmFieldInternalStatHwMode,
                                    _bcmFieldInternalVariable,0, &tlv));
            tlv->value = &(f_st->hw_mode);
            BCM_IF_ERROR_RETURN(tlv_write(unit,tlv,ptr,position));
            BCM_IF_ERROR_RETURN(
                 _field_tlv_create(_bcmFieldInternalStatHwEntryCount,
                                    _bcmFieldInternalVariable,0, &tlv));
            tlv->value = &(f_st->hw_entry_count);
            BCM_IF_ERROR_RETURN(tlv_write(unit,tlv,ptr,position));
            BCM_IF_ERROR_RETURN(
                 _field_tlv_create(_bcmFieldInternalStatGid,
                                    _bcmFieldInternalVariable,0, &tlv));
            tlv->value = &(f_st->gid);
            BCM_IF_ERROR_RETURN(tlv_write(unit,tlv,ptr,position));
            BCM_IF_ERROR_RETURN(
                 _field_tlv_create(_bcmFieldInternalStatStageId,
                                    _bcmFieldInternalVariable,0, &tlv));
            tlv->value = &(f_st->stage_id);
            BCM_IF_ERROR_RETURN(tlv_write(unit,tlv,ptr,position));
            BCM_IF_ERROR_RETURN(
                 _field_tlv_create(_bcmFieldInternalStatStatValues,
                                    _bcmFieldInternalArray,f_st->nstat,
                                    &tlv));
            tlv->value = (f_st->stat_values);
            BCM_IF_ERROR_RETURN(tlv_write(unit,tlv,ptr,position));
            BCM_IF_ERROR_RETURN(
                 _field_tlv_create(_bcmFieldInternalStatFlexMode,
                                    _bcmFieldInternalVariable,0, &tlv));
            tlv->value = &(f_st->flex_mode);
            BCM_IF_ERROR_RETURN(tlv_write(unit,tlv,ptr,position));

            sizestat = *position - sizestat;
            BCM_IF_ERROR_RETURN(
                 _field_tlv_create(_bcmFieldInternalEndStructStat,
                                    _bcmFieldInternalVariable,sizestat, &tlv));
            tlv->value = &endmarkerstat;
            BCM_IF_ERROR_RETURN(tlv_write(unit,tlv,ptr,position));

        }
            f_st = f_st->next;
        }
    }

    /* Hints Hash */

    sizehint = *position;
    BCM_IF_ERROR_RETURN(_field_tlv_create(_bcmFieldInternalControlHintHash,
                                         _bcmFieldInternalVariable, hints_count,
                                         &tlv));
    BCM_IF_ERROR_RETURN(tlv_write(unit,tlv,ptr,position));
    /* Iterate over all hash buckets. */
    for (idx = 0; idx < _FP_HASH_SZ(fc); idx++) {
        /* Iterate over entries in each bucket. */
        f_ht = fc->hints_hash[idx];
        while (NULL != f_ht) {
        if(f_ht->hintid != -1) {

           BCM_IF_ERROR_RETURN
                (_field_tlv_create(_bcmFieldInternalHintHintid,
                                   _bcmFieldInternalVariable,0, &tlv));
           tlv->value = &(f_ht->hintid);
           BCM_IF_ERROR_RETURN(tlv_write(unit,tlv,ptr,position));

           /* types addition */
           BCM_IF_ERROR_RETURN
                (_field_tlv_create(_bcmFieldInternalHintHints,
                                   _bcmFieldInternalArray,
                                   f_ht->hint_count | (8 << 22), &tlv));
           tlv->value = &(type_hint);
           BCM_IF_ERROR_RETURN(tlv_write(unit,tlv,ptr,position));
           hint = f_ht->hints;

           for (hintidx = 0; hintidx < f_ht->hint_count; hintidx++) {
                BCM_IF_ERROR_RETURN
                     (_field_tlv_create(_bcmFieldInternalHintHintsHintType,
                                        _bcmFieldInternalVariable,0,&tlv));
                tlv->value = &(hint->hint->hint_type);
                BCM_IF_ERROR_RETURN(tlv_write(unit,tlv,ptr,position));
                BCM_IF_ERROR_RETURN
                     (_field_tlv_create(_bcmFieldInternalHintHintsQual,
                                        _bcmFieldInternalVariable,0, &tlv));
                tlv->value = &(hint->hint->qual);
                BCM_IF_ERROR_RETURN(tlv_write(unit,tlv,ptr,position));
                BCM_IF_ERROR_RETURN
                     (_field_tlv_create(_bcmFieldInternalHintHintsMaxValues,
                                        _bcmFieldInternalVariable,0, &tlv));
                tlv->value = &(hint->hint->max_values);
                BCM_IF_ERROR_RETURN(tlv_write(unit,tlv,ptr,position));
                BCM_IF_ERROR_RETURN
                     (_field_tlv_create(_bcmFieldInternalHintHintsStartBit,
                                        _bcmFieldInternalVariable,0, &tlv));
                tlv->value = &(hint->hint->start_bit);
                BCM_IF_ERROR_RETURN(tlv_write(unit,tlv,ptr,position));
                BCM_IF_ERROR_RETURN
                     (_field_tlv_create(_bcmFieldInternalHintHintsEndBit,
                                        _bcmFieldInternalVariable,0, &tlv));
                tlv->value = &(hint->hint->end_bit);
                BCM_IF_ERROR_RETURN(tlv_write(unit,tlv,ptr,position));
                BCM_IF_ERROR_RETURN
                     (_field_tlv_create(_bcmFieldInternalHintHintsFlags,
                                        _bcmFieldInternalVariable,0, &tlv));
                tlv->value = &(hint->hint->flags);
                BCM_IF_ERROR_RETURN(tlv_write(unit,tlv,ptr,position));
                BCM_IF_ERROR_RETURN
                     (_field_tlv_create(_bcmFieldInternalHintHintsMaxGrpSize,
                                        _bcmFieldInternalVariable,0, &tlv));
                tlv->value = &(hint->hint->max_group_size);
                BCM_IF_ERROR_RETURN(tlv_write(unit,tlv,ptr,position));
                BCM_IF_ERROR_RETURN
                     (_field_tlv_create(_bcmFieldInternalEndStructHintHints,
                                        _bcmFieldInternalVariable, 0, &tlv));
                BCM_IF_ERROR_RETURN(tlv_write(unit,tlv,ptr,position));

                hint = hint->next;
           }

           BCM_IF_ERROR_RETURN
                (_field_tlv_create(_bcmFieldInternalHintGrpRefCount,
                                        _bcmFieldInternalVariable,0, &tlv));
           tlv->value = &(f_ht->grp_ref_count);
           BCM_IF_ERROR_RETURN(tlv_write(unit,tlv,ptr,position));
           BCM_IF_ERROR_RETURN
                (_field_tlv_create(_bcmFieldInternalHintCount,
                                        _bcmFieldInternalVariable,0, &tlv));
           tlv->value = &(f_ht->hint_count);
           BCM_IF_ERROR_RETURN(tlv_write(unit,tlv,ptr,position));

           sizehint = *position - sizehint;
           BCM_IF_ERROR_RETURN
                (_field_tlv_create(_bcmFieldInternalEndStructHint,
                                   _bcmFieldInternalVariable,sizehint, &tlv));
           tlv->value = &endmarkerhint;
           BCM_IF_ERROR_RETURN(tlv_write(unit,tlv,ptr,position));

        }
            f_ht = f_ht->next;
        }
    }


    FP_UNLOCK(fc);

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
_field_udf_sync(int unit, uint8 *ptr, uint32 *pos, _field_udf_t *udf) {
    _field_tlv_t *tlv   = NULL;
    int endmarker = 0xabcdefab;

    BCM_IF_ERROR_RETURN(_field_tlv_create(_bcmFieldInternalControlUdfValid,
                                   _bcmFieldInternalVariable, 0, &tlv));
    tlv->value = &(udf->valid);
    BCM_IF_ERROR_RETURN(tlv_write(unit,tlv,ptr,pos));

    BCM_IF_ERROR_RETURN(_field_tlv_create(_bcmFieldInternalControlUdfUseCount,
                                   _bcmFieldInternalVariable, 0, &tlv));
    tlv->value = &(udf->use_count);
    BCM_IF_ERROR_RETURN(tlv_write(unit,tlv,ptr,pos));

    BCM_IF_ERROR_RETURN(_field_tlv_create(_bcmFieldInternalControlUdfNum,
                                   _bcmFieldInternalVariable, 0, &tlv));
    tlv->value = &(udf->udf_num);
    BCM_IF_ERROR_RETURN(tlv_write(unit,tlv,ptr,pos));

    BCM_IF_ERROR_RETURN(_field_tlv_create(_bcmFieldInternalControlUdfUserNum,
                                   _bcmFieldInternalVariable, 0, &tlv));
    tlv->value = &(udf->user_num);
    BCM_IF_ERROR_RETURN(tlv_write(unit,tlv,ptr,pos));
    
    BCM_IF_ERROR_RETURN(_field_tlv_create(_bcmFieldInternalControlEndStructUdf,
                                        _bcmFieldInternalVariable, 0, &tlv));
    tlv->value = &endmarker;
    BCM_IF_ERROR_RETURN(tlv_write(unit,tlv,ptr,pos));


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
_field_control_sync(int unit, _field_control_t *fc) {

   _field_tlv_t *tlv   = NULL;      /* &tlv data */
    uint8 *scache_ptr  = fc->scache_ptr[_FIELD_SCACHE_PART_0];
                                    /* Scache Pointer */
    _field_group_t *fg;             /* Group information */
    int group_count    = 0;         /* Number of groups */
    int *gid ;                      /* Array of groupids */
    int hintbmp_length = 0;         /* length of hint bmp */
    uint32 endmarker   = _BCM_FIELD_CONTROL_END_MARKER;
                                    /* End marker for fc */
    int size           = fc->scache_pos;
                                    /* size for fc */
    int pos            = 0;         /* variable for loops */
    
    int type_udf[5] = { (int) ((_bcmFieldInternalVariable << 28) 
                               | _bcmFieldInternalControlUdfValid),
                        (int) ((_bcmFieldInternalVariable << 28)
                               | _bcmFieldInternalControlUdfUseCount),
                        (int) ((_bcmFieldInternalVariable << 28)
                               | _bcmFieldInternalControlUdfNum),
                        (int)  ((_bcmFieldInternalVariable << 28)
                               | _bcmFieldInternalControlUdfUserNum),
                        (int) ((_bcmFieldInternalVariable << 28)
                              | _bcmFieldInternalControlEndStructUdf),
                      };

    BCM_IF_ERROR_RETURN(_field_tlv_create(_bcmFieldInternalControl,
                                           _bcmFieldInternalVariable, 0, &tlv));
    BCM_IF_ERROR_RETURN(tlv_write(unit,tlv,scache_ptr,&(fc->scache_pos)));

    BCM_IF_ERROR_RETURN(_field_tlv_create(_bcmFieldInternalControlFlags,
                                           _bcmFieldInternalVariable, 0, &tlv));
    tlv->value = &(fc->flags);
    BCM_IF_ERROR_RETURN(tlv_write(unit,tlv,scache_ptr,&(fc->scache_pos)));

    fg = fc->groups;
    while (fg != NULL) {
    if (fg->stage_id == _BCM_FIELD_STAGE_INGRESS) {
        group_count ++ ;
    }
    fg = fg->next;
	}

    gid = sal_alloc((sizeof(uint32) * group_count), "alloc for groups");

    fg = fc->groups;
    while (fg != NULL) {
    if (fg->stage_id == _BCM_FIELD_STAGE_INGRESS) {
        gid[pos++] = fg->gid;
    }
    fg = fg->next;
	}

    BCM_IF_ERROR_RETURN(_field_tlv_create(_bcmFieldInternalControlUdfDetails,
                                          _bcmFieldInternalArray, 
                                          (BCM_FIELD_USER_NUM_UDFS | (5 << 22)),
                                           &tlv));
    tlv->value = &type_udf;
    BCM_IF_ERROR_RETURN(tlv_write(unit,tlv,scache_ptr,&(fc->scache_pos)));
    
    for (pos=0; pos < BCM_FIELD_USER_NUM_UDFS; pos++) {
        BCM_IF_ERROR_RETURN(_field_udf_sync(unit, scache_ptr, 
                                    &(fc->scache_pos), &(fc->udf[pos])));
    }


    BCM_IF_ERROR_RETURN(_field_tlv_create(_bcmFieldInternalControlGroups,
                                           _bcmFieldInternalArray,group_count,
                                           &tlv));
    tlv->value = &gid;
    BCM_IF_ERROR_RETURN(tlv_write(unit,tlv,scache_ptr,&(fc->scache_pos)));
    sal_free(gid);

    BCM_IF_ERROR_RETURN(_field_tlv_create(_bcmFieldInternalControlPolicerCount,
                                          _bcmFieldInternalVariable,0,&tlv));
    tlv->value = &(fc->policer_count);
    BCM_IF_ERROR_RETURN(tlv_write(unit,tlv,scache_ptr,&(fc->scache_pos)));

    BCM_IF_ERROR_RETURN(_field_tlv_create(_bcmFieldInternalControlStatCount,
                                          _bcmFieldInternalVariable,0,&tlv));
    tlv->value = &(fc->stat_count);
    BCM_IF_ERROR_RETURN(tlv_write(unit,tlv,scache_ptr,&(fc->scache_pos)));

    hintbmp_length = SHR_BITALLOCSIZE(_FP_HINT_ID_MAX);
    BCM_IF_ERROR_RETURN(_field_tlv_create(_bcmFieldInternalControlHintBmp,
                                          _bcmFieldInternalArray,
                                          hintbmp_length, &tlv));
    tlv->value = fc->hintid_bmp.w;
    BCM_IF_ERROR_RETURN(tlv_write(unit,tlv,scache_ptr,&(fc->scache_pos)));

    BCM_IF_ERROR_RETURN(
         _field_tlv_create(_bcmFieldInternalControlLastAllocatedLtId,
                           _bcmFieldInternalVariable,0,&tlv));
    tlv->value = &(fc->last_allocated_lt_eid);
    BCM_IF_ERROR_RETURN(tlv_write(unit,tlv,scache_ptr,&(fc->scache_pos)));

    BCM_IF_ERROR_RETURN(_field_hash_sync(unit,scache_ptr,&(fc->scache_pos)));

    size = fc->scache_pos - size;
    BCM_IF_ERROR_RETURN(_field_tlv_create(_bcmFieldInternalEndStructControl,
                                          _bcmFieldInternalVariable,size,&tlv));
    tlv->value = &endmarker;
    BCM_IF_ERROR_RETURN(tlv_write(unit,tlv,scache_ptr,&(fc->scache_pos)));
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
_field_stage_sync(int unit, _field_stage_t *stage_fc) {

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
_field_slice_sync(int unit, _field_slice_t *slice) {

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
_field_lt_slice_sync(int unit, _field_lt_slice_t *lt_slice) {

   return BCM_E_NONE;
}

/* Function:
 * _field_entry_sync
 *
 * Purpose:
 *    Sync _field_entry_t structure
 *
 * Paramaters:
 * unit          - (IN) BCM device number
 * slice         - (IN) Pointer to device slice structure
 *
 * Returns:
 *     BCM_E_XXX
 */

int
_field_entry_sync(int unit, _field_slice_t *slice) {

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
 * group         - (IN) Pointer to device group structure
 *
 * Returns:
 *     BCM_E_XXX
 */

int
_field_group_sync(int unit, _field_group_t *group) {

   return BCM_E_NONE;
}

/*
 * Function:
 * bcm_field_th_scache_sync
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
                           _field_control_t *fc,
                           _field_stage_t   *stage_fc
                           )
{
    uint32 group_count = 0;                 /* Number of groups */
    uint32 slice_id,pipe_id,lt_id;          /* Iterator varaiables */
    struct _field_slice_s *curr_slice;      /* Slice information */
    struct _field_lt_slice_s *curr_ltslice; /* LT slice information */
    _field_group_t      *fg;                /* Group information */
    _field_tlv_t *tlv = NULL;               /* TLV data */
    uint8 *scache_ptr = fc->scache_ptr[_FIELD_SCACHE_PART_0];
                                            /* Scache  Pointer */
    int  num_pipes = 0;                     /* Number of pipes */

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
    BCM_IF_ERROR_RETURN(_field_control_sync(unit,fc));

    /*_field_stage_t */
    LOG_DEBUG(BSL_LS_BCM_FP,(BSL_META_U(unit,"FP(unit %d):"
                                        "_bcm_field_th_ingress_scache_sync -"
                                        "Syncing _field_stage_t "
                                        "from pos = %d\r\n"),unit,
                                        fc->scache_pos));

    BCM_IF_ERROR_RETURN(_field_stage_sync(unit,stage_fc));

    /* _field_entry_t and _field_lt_entry_t */
    LOG_DEBUG(BSL_LS_BCM_FP,(BSL_META_U(unit,"FP(unit %d):"
                                        "_bcm_field_th_ingress_scache_sync -"
                                        "Syncing _field_entry_t,"
                                        "_field_lt_entry_t from pos = %d\r\n"),
                                        unit,fc->scache_pos));

    for(pipe_id = 0; pipe_id < num_pipes; pipe_id++) {
        curr_slice = stage_fc->slices[pipe_id];
        if(curr_slice != NULL) {
           for(slice_id= 0; slice_id < stage_fc->tcam_slices; slice_id ++) {
                BCM_IF_ERROR_RETURN(
                     _field_entry_sync(unit,
                                      (curr_slice + slice_id)));
           }
        }
    }

    /* _field_slice_t and _field_lt_slice_t */
    LOG_DEBUG(BSL_LS_BCM_FP,(BSL_META_U(unit,"FP(unit %d):"
                                        "_bcm_field_th_ingress_scache_sync -"
                                        "Syncing _field_slice_t,"
                                        "_field_lt_slice_t from pos = %d\r\n"),
                                        unit,fc->scache_pos));

    for(pipe_id = 0; pipe_id < num_pipes; pipe_id++) {
        curr_slice = stage_fc->slices[pipe_id];
        curr_ltslice = stage_fc->lt_slices[pipe_id];
        if (curr_slice != NULL) {
        for(slice_id= 0; slice_id < stage_fc->tcam_slices; slice_id ++) {
    	     BCM_IF_ERROR_RETURN(_field_slice_sync(unit,
                                                  (curr_slice + slice_id)));
        }
        } else {
        LOG_ERROR(BSL_LS_BCM_FP,(BSL_META_U(unit, "No slices to sync")));
        }
        if (curr_ltslice != NULL) {
            for (lt_id = 0; lt_id < stage_fc->num_logical_tables; lt_id++) {
                 BCM_IF_ERROR_RETURN(
                     _field_lt_slice_sync(unit,
                                         (curr_ltslice + lt_id)));
            }
        } else {
        LOG_ERROR(BSL_LS_BCM_FP,(BSL_META_U(unit, "No LT slices to sync")));
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
    group_count ++ ;
    fg = fg->next;
	}

    BCM_IF_ERROR_RETURN(_field_tlv_create(_bcmFieldInternalGroupCount,
                                          _bcmFieldInternalVariable,
                                          0,&tlv));
    tlv->value = &group_count;
    BCM_IF_ERROR_RETURN(tlv_write(unit,tlv,scache_ptr,&(fc->scache_pos)));

    /* Iterate over the groups linked-list */
    fg = fc->groups;
    while (fg != NULL) {
      _field_group_sync(unit,fg);
    fg = fg->next;
	}

    LOG_DEBUG(BSL_LS_BCM_FP,(BSL_META_U(unit,"FP(unit %d):"
                                        "_bcm_field_th_ingress_scache_sync -"
                                        "End of structures sync "
                                        "at pos = %d\r\n"),unit,
                                        fc->scache_pos));

    return BCM_E_NONE;
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
_field_wb_size_calc(uint32* total_size, _field_control_t  *fc)
{

    int entry_count = 512;              /* Max number of entries per group */
    int slice_count = 12;               /* Max number of slices */
    int basic_size_slice,basic_size_fc,basic_size_stage,basic_size_ltslice;
                                        /* Basic sizes of structures */
    int basic_size_group,basic_size_entry,basic_size_ltentry,basic_size_datafc;
                                        /* Basic sizes of structures */
    int size_slice,size_fc,size_stage,size_ltslice;
                                        /* Total sizes of structures */
    int size_group,size_entry,size_ltentry,size_datafc;
                                        /* Total sizes of structures */

    basic_size_slice = 43;
    basic_size_stage = 596;
    basic_size_ltslice = 32;
    basic_size_group = 94;
    basic_size_entry = 963;
    basic_size_ltentry = 949;
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

    size_entry = ((basic_size_entry + ((BCM_PBMP_PORT_MAX/8) * 2)
                  + (5 * _FP_POLICER_LEVEL_COUNT)) * 2 );

    size_ltentry = basic_size_ltentry;

    size_datafc = basic_size_datafc + (_FP_DATA_ETHERTYPE_MAX * 16)
                  + (_FP_DATA_IP_PROTOCOL_MAX * 21);

    size_fc = basic_size_fc + (10 * BCM_FIELD_USER_NUM_UDFS)
              + (_FP_MAX_NUM_LT * 4)
              + ( _FP_HASH_SZ(fc) * 7347);

    *total_size = size_fc + size_datafc + size_stage + (slice_count
                  * (size_slice + size_ltslice))
                  + (32 * size_group)
                  + (12 * 512 * 4 * (size_entry+size_ltentry));

    return BCM_E_NONE;

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

             BCM_IF_ERROR_RETURN(_field_wb_size_calc(&ifp_calculated_size,fc));

             /* value should be a multiple of 1024 */
             ifp_calculated_size = ((ifp_calculated_size / 1024) *1024);

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
                /* Expected size doesn't match retrieved size */
                return BCM_E_INTERNAL;
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
_field_policer_recover(int unit, _field_tlv_t *tlv) {

    int type_pos = 0;                     /* position of type */
    int i = 0;                            /* local variable for loops */
    int num_instances = 0;                /* number of instances */
    int num_types = 0;                    /* Number of types */
    _field_tlv_t tlv2;                    /* TLV structure */
    uint32 *type;                         /* Elem type */
    uint8 *scache_ptr;                    /* Scahce pointer */
    uint32 *position;                     /* Scache position */
    _field_policer_t    *f_pl = NULL;     /* Field policer structure */
    _field_control_t        *fc;          /* Field control structure.  */
    int rv = 0;

    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));
    FP_LOCK(fc);

    scache_ptr = fc->scache_ptr[_FIELD_SCACHE_PART_0];
    position = &(fc->scache_pos);
	tlv2.value = NULL;
	TLV_INIT(tlv2);
    type = (uint32 *)tlv->value;
    num_instances = (tlv->length) & 0x003fffff;
    num_types     = (tlv->length & 0xffc00000) >> 22;

    for (i=0; i < num_instances; i++) {

        _FP_XGS3_ALLOC(f_pl, sizeof (_field_policer_t), "Field policer");
        if (NULL == f_pl) {
           return (BCM_E_MEMORY);
        }
        type_pos = 0;

        while  ((tlv2.type != _bcmFieldInternalEndStructPolicer)
                 && (type_pos != num_types)) {
            TLV_INIT(tlv2);
            tlv2.type = (type[type_pos] & 0x0fffffff);
            tlv2.basic_type = (type[type_pos] & 0xf0000000) >> 28;
            rv = tlv_read(unit, &tlv2, scache_ptr, position);
            if (rv < 0) {
                goto cleanup;
            }

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
                     break;
                default :
                     rv = BCM_E_INTERNAL;
                     goto cleanup;
            }
            type_pos++;
        }

        TLV_INIT(tlv2);

        _FP_HASH_INSERT(fc->policer_hash, f_pl,
                 ((f_pl->pid) & _FP_HASH_INDEX_MASK(fc)));
    }

    FP_UNLOCK(fc);
    return BCM_E_NONE;

cleanup:
    FP_UNLOCK(fc);
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
_field_stat_recover(int unit, _field_tlv_t *tlv) {

    int type_pos = 0;                     /* position of type */
    int i = 0;                            /* local variable for loops */
    int num_instances = 0;                /* number of instances */
    int num_types = 0;                    /* Number of types */
    _field_tlv_t tlv2 ;             /* TLV structure */
    uint32 *type;                         /* Elem type */
    uint8 *scache_ptr;                    /* Scahce pointer */
    uint32 *position;                     /* Scache position */
    _field_stat_t       *f_st = NULL;     /* Field Stat Structure */
    _field_control_t    *fc;              /* Field control structure.   */
    int rv = 0;

    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));
    FP_LOCK(fc);
	tlv2.value = NULL;
    TLV_INIT(tlv2);
    scache_ptr = fc->scache_ptr[_FIELD_SCACHE_PART_0];
    position = &(fc->scache_pos);
    type = (uint32 *)tlv->value;
    num_instances = (tlv->length) & 0x003fffff;
    num_types     = (tlv->length & 0xffc00000) >> 22;
    for (i=0; i < num_instances; i++) {

        _FP_XGS3_ALLOC(f_st, sizeof (_field_stat_t), "Field stat entity");
        if (NULL == f_st) {
            return (BCM_E_MEMORY);
        }
        type_pos = 0;

        while  ((tlv2.type != _bcmFieldInternalEndStructStat)
                 && (type_pos != num_types)) {
            TLV_INIT(tlv2);
            tlv2.type = (type[type_pos] & 0x0fffffff);
            tlv2.basic_type = (type[type_pos] & 0xf0000000) >> 28;
            rv = tlv_read(unit, &tlv2, scache_ptr, position);
            if (rv  < 0) {
                goto cleanup;
            }
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
                    f_st->hw_mode = *(uint8 *)tlv2.value;
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
                    break;
                default:
                    rv = BCM_E_INTERNAL;
                    goto cleanup;
            }
            type_pos++;
        }
        TLV_INIT(tlv2);
        _FP_HASH_INSERT(fc->stat_hash, f_st,
                        (f_st->sid & _FP_HASH_INDEX_MASK(fc)));

    }

    FP_UNLOCK(fc);
    return BCM_E_NONE;
cleanup:
    FP_UNLOCK(fc);
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
                         _field_hints_t *f_ht) {
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
    int rv = 0;

    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));
    FP_LOCK(fc);
	tlv2.value = NULL;
    TLV_INIT(tlv2);
    scache_ptr = fc->scache_ptr[_FIELD_SCACHE_PART_0];
    position = &(fc->scache_pos);
    type = (uint32 *)tlv->value;
    num_instances = (tlv->length) & 0x003fffff;
    num_types     = (tlv->length & 0xffc00000) >> 22;

    for (i=0; i < num_instances; i++) {
         _FP_XGS3_ALLOC (hint_entry, sizeof (_field_hint_t),
                         "Field Hint Entry Structure");
         _FP_XGS3_ALLOC (hint_entry->hint, sizeof (bcm_field_hint_t),
                         "Field Hint Entry");
         hint_entry->next = NULL;
         type_pos = 0;

         while ((tlv2.type != _bcmFieldInternalEndStructHintHints)
                 && (type_pos != num_types)) {
             TLV_INIT(tlv2);
             tlv2.type = (type[type_pos] & 0x0fffffff);
             tlv2.basic_type = (type[type_pos] & 0xf0000000) >> 28;
             rv = tlv_read(unit, &tlv2, scache_ptr, position);
             if (rv < 0) {
                 goto cleanup;
             }
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
                     rv = BCM_E_INTERNAL;
                     goto cleanup;
            }
            type_pos++;
        }
        _FP_HINTS_LINKLIST_INSERT(&(f_ht->hints),hint_entry);
    }

    FP_UNLOCK(fc);
    return BCM_E_NONE;
cleanup:
    FP_UNLOCK(fc);
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
_field_hint_recover(int unit, _field_tlv_t *tlv) {
    int i = 0;                            /* local variable for loops */
    int num_instances = 0;                /* number of instances */
    _field_tlv_t tlv2;                    /* TLV structure */
    uint8 *scache_ptr;                    /* Scahce pointer */
    uint32 *position;                     /* Scache position */
    _field_hints_t    *f_ht = NULL;       /* field Hints structure */
    _field_control_t        *fc;          /* Field control structure.       */
    int rv = 0;

    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));
    FP_LOCK(fc);
	tlv2.value = NULL;
    TLV_INIT(tlv2);
    scache_ptr = fc->scache_ptr[_FIELD_SCACHE_PART_0];
    position = &(fc->scache_pos);
    num_instances = (int) tlv->length;

    for (i=0; i < num_instances; i++) {
    _FP_XGS3_ALLOC(f_ht, _FP_HASH_SZ(fc) * \
                   sizeof(_field_hints_t), "Hints hash");
    while  (tlv2.type != _bcmFieldInternalEndStructHint) {
            TLV_INIT(tlv2);
            rv = tlv_read(unit, &tlv2, scache_ptr, position);
            if (rv < 0) {
                goto cleanup;
            }
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
                    break;
                default:
                    rv = BCM_E_INTERNAL;
                    goto cleanup;

            }
        }
        TLV_INIT(tlv2);

        _FP_HASH_INSERT(fc->hints_hash, f_ht,
                       (f_ht->hintid & _FP_HASH_INDEX_MASK(fc)));

    }
    FP_UNLOCK(fc);
    return BCM_E_NONE;
cleanup:
    FP_UNLOCK(fc);
    TLV_INIT(tlv2);
    if (f_ht != NULL) {
        sal_free(f_ht);
        f_ht = NULL;
    }
    return rv;

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
_field_udf_recover(int unit, uint8 *ptr, uint32 *pos, _field_udf_t *udf,
                    _field_tlv_t *tlv) {
    _field_tlv_t tlv2;
    int type_pos = 0, num_instances = 0, num_types = 0, i = 0;
    uint32 *type;

    tlv2.value = NULL;
    TLV_INIT(tlv2);
    type = (uint32 *)tlv->value;
    num_instances = (tlv->length) & 0x003fffff;
    num_types     = (tlv->length & 0xffc00000) >> 22;

    for (i=0; i < num_instances; i++) {
        type_pos = 0;
        
        while  ((tlv2.type != _bcmFieldInternalControlEndStructUdf)
                 && (type_pos != num_types)) {
            TLV_INIT(tlv2);
            tlv2.type = (type[type_pos] & 0x0fffffff);
            tlv2.basic_type = (type[type_pos] & 0xf0000000) >> 28;
            BCM_IF_ERROR_RETURN(tlv_read(unit, &tlv2, ptr, pos));
            switch(tlv2.type) {
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
_field_control_recover(int unit, _field_control_t *fc) {

    _field_tlv_t tlv;              /* Field TLV structure */
    uint8 *scache_ptr = fc->scache_ptr[_FIELD_SCACHE_PART_0];
                                   /* Pointer to Scache */
    uint32 *position = &(fc->scache_pos);
                                   /* Position in Scache */
    FP_LOCK(fc);
	tlv.value = NULL;
    TLV_INIT(tlv);


    while (tlv.type != _bcmFieldInternalEndStructControl) {
        TLV_INIT(tlv);
        BCM_IF_ERROR_RETURN(tlv_read(unit, &tlv, scache_ptr, position));

        switch(tlv.type)
        {
            case  _bcmFieldInternalControl:
                  break;
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
                 break;
            default:
                 TLV_INIT(tlv);
                 return BCM_E_INTERNAL;

        }
    }
    TLV_INIT(tlv);

    FP_UNLOCK(fc);

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
_field_stage_recover(int unit, _field_stage_t *stage_fc) {

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
 * slice         - (IN) Pointer to device slice structure
 *
 * Returns:
 *     BCM_E_XXX
 */

int
_field_slice_recover(int unit, _field_slice_t *slice) {

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
 * lt_slice      - (IN) Pointer to device lt_slice structure
 *
 * Returns:
 *     BCM_E_XXX
 */

int
_field_lt_slice_recover(int unit, _field_lt_slice_t *lt_slice) {

   return BCM_E_NONE;
}

/* Function:
 * _field_entry_recover
 *
 * Purpose:
 *    recover _field_entry_t structure
 *
 * Paramaters:
 * unit          - (IN) BCM device number
 * slice         - (IN) Pointer to device slice structure
 *
 * Returns:
 *     BCM_E_XXX
 */

int
_field_entry_recover(int unit, _field_slice_t *slice) {

   return BCM_E_NONE;
}


/* Function:
 * _field_group_recover
 *
 * Purpose:
 *    recover _field_group_t structure
 *
 * Paramaters:
 * unit          - (IN) BCM device number
 * group         - (IN) Pointer to device group structure
 *
 * Returns:
 *     BCM_E_XXX
 */

int
_field_group_recover(int unit, _field_group_t *group) {

   return BCM_E_NONE;
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
    int    rv = BCM_E_NONE;                 /* Operation return value.     */
    uint8  *scache_ptr;                     /* Pointer to 1st scache part. */
    uint32 temp;                            /* Temporary variable.         */
    _field_group_t *new_fg,*curr_fg;        /* For Group LL */
    uint32 slice_id,pipe_id,lt_id,group_id; /* Iterator varaiables */
    struct _field_slice_s *curr_slice;      /* Slice information */
    struct _field_lt_slice_s *curr_ltslice; /* LT slice information */
    _field_tlv_t tlv;                      /* TLV data */
    int  num_pipes = 0;                     /* Number of pipes */

    if (stage_fc->oper_mode == bcmFieldGroupOperModeGlobal) {
        num_pipes = 1;
    } else if (stage_fc->oper_mode == bcmFieldGroupOperModePipeLocal) {
        num_pipes = (_FP_MAX_NUM_PIPES);
    }

	tlv.value = NULL;
    TLV_INIT(tlv);
    scache_ptr = fc->scache_ptr[_FIELD_SCACHE_PART_0];

    fc->scache_pos = 0;
    fc->scache_pos += SOC_WB_SCACHE_CONTROL_SIZE;

    /* _field_control_t */
    LOG_DEBUG(BSL_LS_BCM_FP,(BSL_META_U(unit,"FP(unit %d):"
                                        "_bcm_field_th_stage_ingress_reinit -"
                                        "recovering _field_control_t "
                                        "from pos = %d\r\n"),unit,
                                        fc->scache_pos));
    BCM_IF_ERROR_RETURN(_field_control_recover(unit,fc));

    /*_field_stage_t */
    LOG_DEBUG(BSL_LS_BCM_FP,(BSL_META_U(unit,"FP(unit %d):"
                                        "_bcm_field_th_stage_ingress_reinit -"
                                        "recovering _field_stage_t "
                                        "from pos = %d\r\n"),unit,
                                        fc->scache_pos));
    BCM_IF_ERROR_RETURN(_field_stage_recover(unit,stage_fc));

    /* _field_entry_t and _field_lt_entry_t */
    LOG_DEBUG(BSL_LS_BCM_FP,(BSL_META_U(unit,"FP(unit %d):"
                                        "_bcm_field_th_stage_ingress_reinit -"
                                        "recovering _field_entry_t,"
                                        "_field_lt_entry_t from pos = %d\r\n"),
                                        unit,fc->scache_pos));

    for(pipe_id = 0; pipe_id < num_pipes; pipe_id++) {
        curr_slice = stage_fc->slices[pipe_id];
        if (curr_slice !=NULL) {
            for(slice_id= 0; slice_id < stage_fc->tcam_slices; slice_id ++) {
    	         BCM_IF_ERROR_RETURN(
                      _field_entry_recover(unit,
                                           (curr_slice + slice_id)));
            }
        }
    }

    /* _field_slice_t and _field_lt_slice_t */
    LOG_DEBUG(BSL_LS_BCM_FP,(BSL_META_U(unit,"FP(unit %d):"
                                        "_bcm_field_th_stage_ingress_reinit -"
                                        "recovering _field_slice_t,"
                                        "_field_lt_slice_t from pos = %d\r\n"),
                                        unit,fc->scache_pos));
    for(pipe_id = 0; pipe_id < num_pipes; pipe_id++) {
        curr_slice = stage_fc->slices[pipe_id];
        curr_ltslice = stage_fc->lt_slices[pipe_id];
        if (curr_slice != NULL) {
            for(slice_id= 0; slice_id < stage_fc->tcam_slices; slice_id ++) {
                 BCM_IF_ERROR_RETURN(
                      _field_slice_recover(unit,
                                          (curr_slice + slice_id)));
            }
        } else {
            LOG_ERROR(BSL_LS_BCM_FP,(BSL_META_U(unit,
                                           "Memory not allocated for slice")));
        }

        if (curr_ltslice != NULL) {
            for (lt_id = 0; lt_id < stage_fc->num_logical_tables; lt_id++) {
                  BCM_IF_ERROR_RETURN(_field_lt_slice_recover(unit,
                                                             (curr_ltslice
                                                              + lt_id)));
            }
        }else {
            LOG_ERROR(BSL_LS_BCM_FP,(BSL_META_U(unit,
                                           "Memory not allocated"
                                           "for LT slice")));
        }

    }

    /* _field_group_t */
    LOG_DEBUG(BSL_LS_BCM_FP,(BSL_META_U(unit,"FP(unit %d):"
                                        "_bcm_field_th_stage_ingress_reinit -"
                                        "recovering _field_group_t "
                                        "from pos = %d\r\n"),unit,
                                        fc->scache_pos));
    new_fg = NULL;
    curr_fg = NULL;
    TLV_INIT(tlv);

    BCM_IF_ERROR_RETURN(tlv_read(unit,&tlv,scache_ptr,&(fc->scache_pos)));

    /* Iterate over the groups */
    for (group_id =0 ;group_id < tlv.length; group_id ++) {
         new_fg = sal_alloc(sizeof(_field_group_t), "group alloc");
         _field_group_recover(unit,new_fg);
         new_fg-> next = curr_fg;
         curr_fg = new_fg;
         sal_free(new_fg);
	}
    fc->groups = curr_fg;

    LOG_DEBUG(BSL_LS_BCM_FP,(BSL_META_U(unit,"FP(unit %d):"
                                        "_bcm_field_th_stage_ingress_reinit -"
                                        "End of structures recover "
                                        "at pos = %d\r\n"),unit,
                                        fc->scache_pos));

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

    return rv;
 }


#endif /* BCM_TOMAHAWK_SUPPORT */
#endif /* BCM_WARM_BOOT_SUPPORT */
