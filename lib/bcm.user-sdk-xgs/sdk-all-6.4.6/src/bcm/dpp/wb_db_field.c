/*
 * $Id: wb_db_field.c,v 1.26 Broadcom SDK $
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
 *     Warm boot support
 */

#include <bcm/error.h>
#include <bcm/module.h>

#include <bcm_int/common/debug.h>
#include <bcm_int/dpp/error.h>
#include <bcm_int/dpp/field.h>
#include <bcm_int/dpp/field_int.h>
#include <bcm_int/dpp/wb_db_field.h>
#include <bcm_int/dpp/wb_db_cmn.h>
#include <soc/dpp/soc_sw_db.h>
#ifdef BCM_PETRAB_SUPPORT
#include <soc/dpp/Petra/PB_PP/pb_pp_sw_db.h>
#endif /* def BCM_PETRAB_SUPPORT */
#if defined(INCLUDE_KBP) && !defined(BCM_88030)
#include <soc/dpp/ARAD/arad_kbp.h>
#include <soc/dpp/dpp_wb_engine.h>
#include <shared/swstate/access/sw_state_access.h>
#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030) */
/*
 *  For now, if _BCM_DPP_FIELD_WARM_BOOT_SUPPORT is FALSE, disable warm boot
 *  support here.
 */
#if !_BCM_DPP_FIELD_WARM_BOOT_SUPPORT
#ifdef BCM_WARM_BOOT_SUPPORT
#undef BCM_WARM_BOOT_SUPPORT
#endif  /* BCM_WARM_BOOT_SUPPORT */
#endif /* !_BCM_DPP_FIELD_WARM_BOOT_SUPPORT */


#ifdef BCM_WARM_BOOT_SUPPORT

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_INIT

#include <shared/bsl.h>
#include <shared/swstate/access/sw_state_access.h>

/*
 *  Set _BCM_PPD_FIELD_WARM_BOOT_WRITE_TRACKING to TRUE if you want every warm
 *  boot write to emit a diagnostic message.  This can be helpful if there are
 *  problems, but there are places where it is just messy (in particular, a
 *  full warm boot sync on Arad will emit a couple hundred thousand lines of
 *  stuff assuming an 80 column display).  Setting it FALSE does not disable
 *  any error or exception diagnostic.
 */
#define _BCM_PPD_FIELD_WARM_BOOT_WRITE_TRACKING TRUE

/*
 *  Set _BCM_PPD_FIELD_WARM_BOOT_READ_TRACKING to TRUE if you want details
 *  emitted about the warm boot reads when recovering.  This can be helpful if
 *  there are problems, but there are places where normally it will just be
 *  messy (similar to the warm boot write tracking setting above).  Setting it
 *  false does not disable any error or exception diagnostic.
 */
#define _BCM_PPD_FIELD_WARM_BOOT_READ_TRACKING TRUE

uint8 wb_field_is_scheduled_sync[BCM_MAX_NUM_UNITS]; /*not autosync*/

/*
 *  Function
 *    _bcm_dpp_field_wb_bmp_put
 *  Purpose
 *    Write a SHR_BIT based bitmap to backing store
 *  Arguments
 *    (IN) unitData = unit information
 *    (IN) bitmap = pointer to bitmap to write
 *    (IN) currBits = bits in bitmap
 *    (OUT) ptr = where to write bits
 *    (IN) backBits = bits in backing store for this bitmap
 *    (IN) desc = description of this bitmap
 *    (IN) itemNum = item number for this bitmap
 *    (IN) itemType = description of the item type for this bitmap
 *  Results
 *    (none)
 *  Notes
 *    Assumes backing store bitmap was all zeroes before call
 */
STATIC void
_bcm_dpp_field_wb_bmp_put(_bcm_dpp_field_info_t *unitData,
                          SHR_BITDCL *bitmap,
                          unsigned int currBits,
                          uint32 *ptr,
                          unsigned int backBits,
                          const char *desc,
                          unsigned int itemNum,
                          const char *itemType)
{
    unsigned int index;
	int unit = unitData->unit;
	
    for (index = 0; index < currBits; index++) {
        if (index < backBits) {
            if (SHR_BITGET(bitmap, index)) {
                ptr[index >> 5] |= (1 << (index & 0x1F));
            }
        } else { /* if (index < backBits) */
            if (SHR_BITGET(bitmap, index)) {
                LOG_ERROR(BSL_LS_BCM_INIT,
                          (BSL_META_U(unit,
                                      "unit %d backing store supports"
                                      " %u %s, but %s %u has bit %u set\n"),
                           unit,
                           backBits,
                           desc,
                           itemType,
                           itemNum,
                           index));
                unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_FORMAT_FAIL;
            } /* if (SHR_BITGET(bitmap, index)) */
        } /* if (index < backBits) */
    } /* for (index = 0; index < currBits; index++) */
}

/*
 *  Function
 *    _bcm_dpp_field_wb_bmp_get
 *  Purpose
 *    Read a SHR_BIT based bitmap from backing store
 *  Arguments
 *    (IN) unitData = unit information
 *    (IN) ptr = where to get bits from backing store
 *    (IN) backBits = bits in backing store for this bitmap
 *    (OUT) bitmap = where to write the recovered bitmap
 *    (IN) currBits = bits in bitmap
 *    (IN) desc = description of this bitmap
 *    (IN) itemNum = item number for this bitmap
 *    (IN) itemType = description of the item type for this bitmap
 *  Results
 *    bcm_error_t (cast as int)
 *      BCM_E_NONE if successful
 *      BCM_E_* otherwise as appropriate
 *  Notes
 */
STATIC int
_bcm_dpp_field_wb_bmp_get(_bcm_dpp_field_info_t *unitData,
                          uint32 *ptr,
                          unsigned int backBits,
                          SHR_BITDCL *bitmap,
                          unsigned int currBits,
                          const char *desc,
                          unsigned int itemNum,
                          const char *itemType)
{
    unsigned int index;
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;
	
	unit = unitData->unit;

    /* recover the bits in backing store */
    for (index = 0; index < backBits; index++) {
        if (index < currBits) {
            if (ptr[index >> 5] & (1 << (index & 0x1F))) {
                SHR_BITSET(bitmap, index);
            } else {
                SHR_BITCLR(bitmap, index);
            }
        } else { /* if (index < currBits) */
            if (ptr[index >> 5] & (1 << (index & 0x1F))) {
                unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_RESTORE_FAIL;
                BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                                 (_BSL_BCM_MSG_NO_UNIT("unit %d currently supports"
                                                   " %u %s, but backing store"
                                                   " %s %u has bit %u set\n"),
                                  unit,
                                  currBits,
                                  desc,
                                  itemType,
                                  itemNum,
                                  index));
            } /* if (ptr[index >> 5] & (1 << (index & 0x1F))) */
        } /* if (index < currBits) */
    } /* for (index = 0; index < backBits; index++) */
    /* make sure any leftover bits are clear */
    while (index < currBits) {
        SHR_BITCLR(bitmap, index);
        index++;
    }
exit:
    BCMDNX_FUNC_RETURN;
}

void
_bcm_dpp_field_tcam_entry_wb_save(_bcm_dpp_field_info_t *unitData,
                                  _bcm_dpp_field_ent_idx_t entry,
                                  uint8 isExternalTcam,
                                  uint8 *data,
                                  unsigned int *size)
{
    void *wbData;
    _bcm_dpp_wb_1_0_field_entry_tcam_t *wbDataIntTcam = NULL;
    _bcm_dpp_wb_1_0_field_entry_ext_tcam_t *wbDataExtTcam = NULL;
    unsigned int index;
    unsigned int offset;
    unsigned int entrySize;
    int offsetEntry;
    int entryLimit;
    int entBias;
    uint32 *flags;
    uint32 *hwHandle;
    uint32 *group;
    int result;
	int unit = unitData->unit;

    offsetEntry = isExternalTcam ? unitData->wb.offsetExtTcamEntry : unitData->wb.offsetTcamEntry;
    entryLimit  = isExternalTcam ? unitData->wb.extTcamEntryLimit : unitData->wb.tcamEntryLimit;
    entBias = isExternalTcam ? _BCM_DPP_FIELD_ENT_BIAS(unit, ExternalTcam) : _BCM_DPP_FIELD_ENT_BIAS(unit, InternalTcam);
    if ((!size) && (0 == offsetEntry)) {
        LOG_ERROR(BSL_LS_BCM_INIT,
                  (BSL_META_U(unit,
                              "unit %d does not appear to support TCAM entry"
                              " data in backing store version %04X\n"),
                   unit,
                   unitData->wb.version));
        unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_FORMAT_FAIL;
        return;
    }
    if (entry >= entryLimit) {
        LOG_ERROR(BSL_LS_BCM_INIT,
                  (BSL_META_U(unit,
                              "unit %d backing store only supports TCAM"
                              " entries %d through %d\n. external TCAM: %d"),
                   unit,
                   entBias,
                   entBias + entryLimit - 1,
                   isExternalTcam));
        unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_FORMAT_FAIL;
        return;
    }
    switch (unitData->wb.version) {
    case _BCM_DPP_WB_FIELD_VERSION_1_0:
        entrySize = BCM_DPP_WB_FIELD_TCAM_ENTRY_SIZE(isExternalTcam);
        if (!isExternalTcam) {
            entrySize += (sizeof(wbDataIntTcam->tcamAction[0]) *
                      (unitData->wb.tcamBcmActionLimit - 1));
        }
        entrySize = _BCM_DPP_WB_FIELD_ALIGNED_SIZE(entrySize);
        if (size) {
            *size = entrySize;
        } else { /* if (size) */
            wbData = sal_alloc(entrySize, "tcam entry backing store temp");
            if (wbData) {
                sal_memset(wbData, 0x00, entrySize);
                if (_BCM_DPP_FIELD_TCAM_ENTRY_FLAGS(isExternalTcam, entry) &
                    _BCM_DPP_FIELD_ENTRY_IN_HW) {
                    if (isExternalTcam) {
                        wbDataExtTcam = wbData;
                    }
                    else {
                        wbDataIntTcam = wbData;
                    }
                    flags = BCM_DPP_WB_FIELD_TCAM_ENTRY_FLAGS_PTR(isExternalTcam);
                    *flags = _BCM_DPP_FIELD_TCAM_ENTRY_FLAGS(isExternalTcam, entry); 
                    hwHandle = BCM_DPP_WB_FIELD_TCAM_ENTRY_HW_HANDLE_PTR(isExternalTcam);
                    *hwHandle = _BCM_DPP_FIELD_TCAM_ENTRY_HW_HANDLE(isExternalTcam, entry);
                    group = BCM_DPP_WB_FIELD_TCAM_ENTRY_GROUP_PTR(isExternalTcam);
                    *group = _BCM_DPP_FIELD_TCAM_ENTRY_GROUP(isExternalTcam, entry);
                    if (!isExternalTcam) {
                        wbDataIntTcam->entryCmn.entryPriority = unitData->entryTc[entry].entryCmn.entryPriority;
                        for (index = 0, offset = 0;
                             index < _BCM_DPP_NOF_BCM_ACTIONS_PER_DB_MAX;
                             index++) {
                            if (bcmFieldActionCount >
                                unitData->entryTc[entry].tcActB[index].bcmType) {
                                if (unitData->wb.bcmFieldActionCount >
                                    unitData->entryTc[entry].tcActB[index].bcmType) {
                                    if (offset < unitData->wb.tcamBcmActionLimit) {
                                        wbDataIntTcam->tcamAction[offset].bcmType = unitData->entryTc[entry].tcActB[index].bcmType;
                                        wbDataIntTcam->tcamAction[offset].bcmParam0 = unitData->entryTc[entry].tcActB[index].bcmParam0;
                                        wbDataIntTcam->tcamAction[offset].bcmParam1 = unitData->entryTc[entry].tcActB[index].bcmParam1;
                                    } /* if (offset is valid in backing store) */
                                    offset++;
                                } else { /* if (action valid in backing store) */
                                    LOG_ERROR(BSL_LS_BCM_INIT,
                                              (BSL_META_U(unit,
                                                          "unit %d backing store"
                                                          " value of"
                                                          " bcmFieldActionCount"
                                                          " (%d) prohibits entry"
                                                          " %u action %s (%d) at"
                                                          " index %u\n"),
                                               unit,
                                               unitData->wb.bcmFieldActionCount,
                                               entry + _BCM_DPP_FIELD_ENT_BIAS(unit, InternalTcam),
                                               _bcm_dpp_field_action_name[unitData->entryTc[entry].tcActB[index].bcmType],
                                               unitData->entryTc[entry].tcActB[index].bcmType,
                                               index));
                                    unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_FORMAT_FAIL;
                                    wbDataIntTcam->tcamAction[index].bcmType = unitData->wb.bcmFieldActionCount;
                                } /* if (action valid in backing store) */
                            } /* if (action is valid now) */
                        } /* for (all actions in this entry) */
                        if (offset > unitData->wb.tcamBcmActionLimit) {
                            LOG_ERROR(BSL_LS_BCM_INIT,
                                      (BSL_META_U(unit,
                                                  "unit %d backing store only"
                                                  " stores %u actions for entry"
                                                  " %u, but entry %u has %u"
                                                  " actions\n"),
                                       unit,
                                       unitData->wb.tcamBcmActionLimit,
                                       entry + _BCM_DPP_FIELD_ENT_BIAS(unit, InternalTcam),
                                       entry + _BCM_DPP_FIELD_ENT_BIAS(unit, InternalTcam),
                                       offset));
                            unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_FORMAT_FAIL;
                        } /* if (offset is beyond backing store limit) */
                        while (offset < unitData->wb.tcamBcmActionLimit) {
                            wbDataIntTcam->tcamAction[offset].bcmType = unitData->wb.bcmFieldActionCount;
                            offset++;
                        } /* while (offset < unitData->wb_tcamBcmActionLimit) */
                    }
                } /* if (entry is in hardware) */
#if _BCM_PPD_FIELD_WARM_BOOT_WRITE_TRACKING
                LOG_DEBUG(BSL_LS_BCM_INIT,
                          (BSL_META_U(unit,
                                      "unit %d backing store write TCAM"
                                      " entry %u info at %u for %u bytes\n"),
                           unit,
                           entry,
                           unitData->wb.offsetTcamEntry + (entry * entrySize),
                           entrySize));
#endif /* _BCM_PPD_FIELD_WARM_BOOT_WRITE_TRACKING */
                if (data) {
                    sal_memcpy(&(data[offsetEntry + (entry * entrySize)]),
                               wbData,
                               entrySize);
                } else { /* if (data) */
                    sal_memcpy(&(unitData->wb.buffer[offsetEntry +
                                                     (entry * entrySize)]),
                               wbData,
                               entrySize);
					if (!(SOC_CONTROL(unit)->autosync) && !wb_field_is_scheduled_sync[unit]) {
					        /* mark dirty state. Currently no other processing done           */
					        /* note that there is no in module dirty bit optimization. use global dirty bit */
					        BCM_DPP_WB_DEV_FIELD_DIRTY_BIT_SET(unit);								        
					}
					else{
	                    result = soc_scache_commit_specific_data(unit,
	                                                             unitData->wb.handle,
	                                                             entrySize,
	                                                             (uint8*)wbData,
	                                                             offsetEntry +
	                                                             (entry * entrySize));
	                    if (SOC_E_NONE != result) {
	                        LOG_ERROR(BSL_LS_BCM_INIT,
                                          (BSL_META_U(unit,
                                                      "unit %d unable to update backing"
                                                      " store for entry %d: %d (%s)\n"),
                                           unit,
                                           entry + entBias,
                                           result,
                                           _SHR_ERRMSG(result)));
	                        unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_COMMIT_FAIL;
	                    }
	                }
                } /* if (data) */
                sal_free(wbData);
            } else { /* if (wbData) */
                LOG_ERROR(BSL_LS_BCM_INIT,
                          (BSL_META_U(unit,
                                      "unit %d unable to allocate temporary"
                                      " space to update backing store for"
                                      " field entry %u"),
                           unit,
                           entry + entBias));
                unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_COMMIT_FAIL;
            } /* if (wbData) */
        } /* if (size) */
        break;
    default:
        LOG_ERROR(BSL_LS_BCM_INIT,
                  (BSL_META_U(unit,
                              "unit %d unexpected warm boot version %04X\n"),
                   unit,
                   unitData->wb.version));
        unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_VERSION_FAIL;
        if (size) {
            *size = 0;
        }
    } /* switch (unitData->wb_version) */
}

/*
 *  Function
 *    _bcm_dpp_field_entry_qual_wb_load
 *  Purpose
 *    Load qualifier state into a BCM entry qualifier array, from a PPD
 *    qualifier array.
 *  Arguments
 *    IN unitData = pointer to the unit information
 *    IN entry = entry ID as would be presented at BCM layer
 *    IN bcmQuals = pointer to the BCM layer qualifiers buffer
 *    IN ppdQuals = pointer to the PPD layer qualifiers buffer
 *  Results
 *    bcm_error_t (cast as int)
 *      BCM_E_NONE if successful
 *      BCM_E_* otherwise as appropriate
 *  Notes
 */
STATIC int
_bcm_dpp_field_entry_qual_wb_load(_bcm_dpp_field_info_t *unitData,
                                  unsigned int entry,
                                  _bcm_dpp_field_qual_t *bcmQuals,
                                  SOC_PPC_FP_QUAL_VAL *ppdQuals)
{
    unsigned int index;
    unsigned int offset;
    unsigned int temp;
    int result;
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;

    /* copy qualifiers from hardware */
    for (index = 0, offset = 0;
         index < unitData->wb.ppdQualLimit;
         index++) {
        if ((_BCM_DPP_FIELD_PPD_QUAL_VALID(ppdQuals[index].type)) &&
            (ppdQuals[index].is_valid.arr[0] ||
             ppdQuals[index].is_valid.arr[1])) {
            /* this qualifier is valid and significant */
            if (SOC_PPD_FP_NOF_QUALS_PER_DB_MAX <= offset) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                                 (_BSL_BCM_MSG_NO_UNIT("unit %d entry %u tries to"
                                                   " claim %u qualifiers, but"
                                                   " only %u are supported"),
                                  unit,
                                  entry,
                                  offset,
                                  SOC_PPD_FP_NOF_QUALS_PER_DB_MAX));
            }
            bcmQuals[offset].hwType = ppdQuals[index].type;
            bcmQuals[offset].qualType = unitData->ppdQual[ppdQuals[index].type];
            result = BCM_E_NONE;
            if (bcmFieldQualifyCount == bcmQuals[offset].qualType) {
                /* not a standard qualifier; try mapping a programmable one */
                result = _bcm_petra_field_map_ppd_udf_to_bcm(unitData,
                                                             ppdQuals[index].type,
                                                             &temp);
            }
            if ((bcmQuals[offset].qualType > bcmFieldQualifyCount) ||
                (BCM_E_NONE != result)) {
                /* invalid or unknown qualifier */
                unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_RESTORE_FAIL;
                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                                 (_BSL_BCM_MSG_NO_UNIT("unit %d unable to find"
                                                   " a BCM qualifier to"
                                                   " reflect PPD qualifier"
                                                   " %s (%d)"),
                                  unit,
                                  SOC_PPD_FP_QUAL_TYPE_to_string(ppdQuals[index].type),
                                  ppdQuals[index].type));
            }
            COMPILER_64_SET(bcmQuals[offset].qualData,
                            ppdQuals[index].val.arr[1],
                            ppdQuals[index].val.arr[0]);
            COMPILER_64_SET(bcmQuals[offset].qualMask,
                            ppdQuals[index].is_valid.arr[1],
                            ppdQuals[index].is_valid.arr[0]);
            offset++;
        } /* if (hardware data indicates an active qualifier) */
    } /* for (all qualifiers in hardware) */
    /* fill in any unused qualifiers */
    while (offset < SOC_PPD_FP_NOF_QUALS_PER_DB_MAX) {
        bcmQuals[offset].qualType = bcmFieldQualifyCount;
        bcmQuals[offset].hwType = SOC_PPD_NOF_FP_QUAL_TYPES;
        COMPILER_64_ZERO(bcmQuals[offset].qualMask);
        COMPILER_64_ZERO(bcmQuals[offset].qualData);
        offset++;
    } /* while (offset < SOC_PPD_FP_NOF_QUALS_PER_DB_MAX) */
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *    _bcm_dpp_field_tcam_entry_wb_load
 *  Purpose
 *    Load a TCAM entry's information from the backing store
 *    buffer and hardware
 *  Arguments
 *    IN unitData = pointer to the unit information
 *  Results
 *    bcm_error_t (cast as int)
 *      BCM_E_NONE if successful
 *      BCM_E_* otherwise as appropriate
 *  Notes
 *    Cascaded groups may not have their pair available from the PPD, so this
 *    may not be restored; it needs to be scanned after all groups are loaded.
 */
STATIC int
_bcm_dpp_field_tcam_entry_wb_load(_bcm_dpp_field_info_t *unitData,
                                  _bcm_dpp_field_ent_idx_t entry,
                                  uint8 isExternalTcam)
{
    SOC_PPD_FP_ENTRY_INFO entInfoTc;
    SOC_PPD_TRAP_EG_ACTION_PROFILE_INFO egrProfile;
    void *entryTemp = NULL;
    _bcm_dpp_field_group_t *groupData;
    _bcm_dpp_wb_1_0_field_entry_tcam_t *wbDataIntTcam = NULL;
    _bcm_dpp_wb_1_0_field_entry_ext_tcam_t *wbDataExtTcam = NULL;
    _bcm_dpp_field_entry_t *entryTempIntTcam = NULL;
    _bcm_dpp_field_entry_ext_t *entryTempExtTcam = NULL;
    _bcm_dpp_field_ent_idx_t current;
    _bcm_dpp_field_ent_idx_t *entryIdx; 
    unsigned int index;
    unsigned int offset;
    unsigned int entrySize;
    int offsetEntry;
    int entryLimit;
    int entBias;
    int result;
    int temp;
    uint32 flags;
    uint32 hwHandle;
    uint32 group;
    uint8 okay;
    uint32 ppdr;
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;

    offsetEntry = isExternalTcam ? unitData->wb.offsetExtTcamEntry : unitData->wb.offsetTcamEntry;
    entryLimit  = isExternalTcam ? unitData->wb.extTcamEntryLimit : unitData->wb.tcamEntryLimit;
    entBias = isExternalTcam ? _BCM_DPP_FIELD_ENT_BIAS(unit, ExternalTcam) : _BCM_DPP_FIELD_ENT_BIAS(unit, InternalTcam);

    if (entry >= entryLimit) {
        unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_FORMAT_FAIL;
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d tried to read backing"
                                           " store for TCAM entry"
                                           " %u but only TCAM"
                                           " entries %u..%u in backing store"),
                          unit,
                          entry + entBias,
                          entBias,
                          entryLimit + entBias));
    }
    if (entry > _BCM_DPP_FIELD_TCAM_ENTRY_LIMIT(isExternalTcam)) {
        unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_RESTORE_FAIL;
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d tried to read backing"
                                           " store for TCAM entry"
                                           " %u but only supports TCAM"
                                           " entries %u..%u"),
                          unit,
                          entry + entBias,
                          entBias,
                          _BCM_DPP_FIELD_TCAM_ENTRY_LIMIT(isExternalTcam) + entBias));
    }
    BCMDNX_ALLOC(entryTemp, _BCM_DPP_FIELD_TCAM_ENTRY_SIZE(isExternalTcam), "_bcm_dpp_field_tcam_entry_wb_load.entryTemp");
    if (entryTemp == NULL) {        
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG_NO_UNIT("failed to allocate memory")));
    }
    sal_memset(entryTemp, 0x00, _BCM_DPP_FIELD_TCAM_ENTRY_SIZE(isExternalTcam));
    switch (unitData->wb.version) {
    case _BCM_DPP_WB_FIELD_VERSION_1_0:
        entrySize = BCM_DPP_WB_FIELD_TCAM_ENTRY_SIZE(isExternalTcam);
        if (!isExternalTcam) {
            entrySize += (sizeof(wbDataIntTcam->tcamAction[0]) *
                      (unitData->wb.tcamBcmActionLimit - 1));
        }
        entrySize = _BCM_DPP_WB_FIELD_ALIGNED_SIZE(entrySize);
        if(isExternalTcam) {
            wbDataExtTcam = (void*)&(unitData->wb.buffer[offsetEntry + (entrySize * entry)]);
            entryTempExtTcam = entryTemp;
        }
        else {
            wbDataIntTcam = (void*)&(unitData->wb.buffer[offsetEntry + (entrySize * entry)]);
            entryTempIntTcam = entryTemp;;
        }
        if (0 == (BCM_DPP_WB_FIELD_TCAM_ENTRY_FLAGS(isExternalTcam) & _BCM_DPP_FIELD_ENTRY_IN_HW)) {
            /* nothing to recover here since not in hardware */
            BCM_EXIT;
        }
#if _BCM_PPD_FIELD_WARM_BOOT_READ_TRACKING
        LOG_DEBUG(BSL_LS_BCM_INIT,
                  (BSL_META_U(unit,
                              "unit %d recovering TCAM"
                              " entry %u information from warm boot buffer\n"),
                   unit,
                   entry + entBias));
#endif /* _BCM_PPD_FIELD_WARM_BOOT_READ_TRACKING */
        /* populate from backing store */
        flags = BCM_DPP_WB_FIELD_TCAM_ENTRY_FLAGS(isExternalTcam);
        hwHandle = BCM_DPP_WB_FIELD_TCAM_ENTRY_HW_HANDLE(isExternalTcam);
        group = BCM_DPP_WB_FIELD_TCAM_ENTRY_GROUP(isExternalTcam);
        if(isExternalTcam) {
            entryTempExtTcam->entryCmn.entryFlags = flags;
            entryTempExtTcam->entryCmn.hwHandle = hwHandle;
            entryTempExtTcam->entryCmn.entryGroup = group;
        }
        else {
            entryTempIntTcam->entryCmn.entryFlags = flags;
            entryTempIntTcam->entryCmn.hwHandle = hwHandle;
            entryTempIntTcam->entryCmn.entryGroup = group;
            entryTempIntTcam->entryCmn.entryPriority = wbDataIntTcam->entryCmn.entryPriority;
            entryTempIntTcam->egrTrapProfile = -1;
        }
        if (group >= unitData->wb.groupLimit) {
            unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_FORMAT_FAIL;
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d entry %u claims to be"
                                               " in group %u, but backing store"
                                               " only supports %u groups"),
                              unit,
                              entry + entBias,
                              _BCM_DPP_FIELD_TCAM_ENTRY_GROUP(isExternalTcam, entry),
                              unitData->wb.groupLimit));
        }
        if (group >= unitData->groupLimit) {
            unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_RESTORE_FAIL;
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d entry %u claims to be"
                                               " in group %u, but only"
                                               " supports %u groups"),
                              unit,
                              entry + entBias,
                              _BCM_DPP_FIELD_TCAM_ENTRY_GROUP(isExternalTcam, entry),
                              unitData->groupLimit));
        }
        groupData = &(unitData->groupD[group]);
        /* get state from PPD layer */
        SOC_PPD_FP_ENTRY_INFO_clear(&entInfoTc);
        ppdr = soc_ppd_fp_entry_get(unitData->unitHandle,
                                    groupData->hwHandle,
                                    (isExternalTcam ? (entryTempExtTcam->entryCmn.hwHandle) : (entryTempIntTcam->entryCmn.hwHandle)),
                                    &okay,
                                    &entInfoTc);
        result = handle_sand_result(ppdr);
        if (!okay) {
            result = BCM_E_FAIL;
        }
        if (BCM_E_NONE != result) {
            unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_RESTORE_FAIL;
            BCMDNX_ERR_EXIT_MSG(result,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d unable to read group"
                                               " %u entry %u (%u): %d (%s)"),
                              unit,
                              _BCM_DPP_FIELD_TCAM_ENTRY_GROUP(isExternalTcam, entry),
                              entry + entBias,
                              _BCM_DPP_FIELD_TCAM_ENTRY_HW_HANDLE(isExternalTcam, entry),
                              result,
                              _SHR_ERRMSG(result)));
        }
        /* mark this PPD layer ID as in use */
        temp = hwHandle;
        result = shr_mres_alloc(unitData->hwResIds,
                                unitData->stageD[groupData->stage].entryRes,
                                SHR_RES_ALLOC_WITH_ID,
                                1,
                                &temp);
        if (BCM_E_NONE != result) {
            unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_RESTORE_FAIL;
            BCMDNX_ERR_EXIT_MSG(result,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d stage %u unable to"
                                               " allocate TCAM"
                                               " hardware entry ID %u:"
                                               " %d (%s)"),
                              unit,
                              groupData->stage,
                              hwHandle,
                              result,
                              _SHR_ERRMSG(result)));
        }
        /* get other attributes from PPD entry */

        if (!isExternalTcam) {
            entryTempIntTcam->entryCmn.hwPriority = entInfoTc.priority;
            /* copy BCM actions from backing store */
            for (index = 0, offset = 0;
                 index < unitData->wb.tcamBcmActionLimit;
                 index++) {
                if (offset < _BCM_DPP_NOF_BCM_ACTIONS_PER_DB_MAX) {
                    if (wbDataIntTcam->tcamAction[index].bcmType < bcmFieldActionCount) {
                        /* this BCM action is valid */
                        entryTempIntTcam->tcActB[offset].bcmType = wbDataIntTcam->tcamAction[index].bcmType;
                        entryTempIntTcam->tcActB[offset].bcmParam0 = wbDataIntTcam->tcamAction[index].bcmParam0;
                        entryTempIntTcam->tcActB[offset].bcmParam1 = wbDataIntTcam->tcamAction[index].bcmParam1;
                        offset++;
                    }
                } else { /* if (offset < _BCM_DPP_NOF_BCM_ACTIONS_PER_DB_MAX) */
                    if (wbDataIntTcam->tcamAction[index].bcmType < bcmFieldActionCount) {
                        /* this BCM action is valid, but there are too many */
                        unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_RESTORE_FAIL;
                        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                                         (_BSL_BCM_MSG_NO_UNIT("unit %d entry %u"
                                                           " claims %u BCM actions"
                                                           " but only supports %u"
                                                           " BCM actions"),
                                          unit,
                                          entry + _BCM_DPP_FIELD_ENT_BIAS(unit, InternalTcam),
                                          offset,
                                          _BCM_DPP_NOF_BCM_ACTIONS_PER_DB_MAX));
                    }
                } /* if (offset < _BCM_DPP_NOF_BCM_ACTIONS_PER_DB_MAX) */
            } /* for (all BCM actions) */
            /* fill in remaining unused BCM actions */
            while (offset < _BCM_DPP_NOF_BCM_ACTIONS_PER_DB_MAX) {
                entryTempIntTcam->tcActB[offset].bcmType = bcmFieldActionCount;
                offset++;
            } /* while (offset < _BCM_DPP_NOF_BCM_ACTIONS_PER_DB_MAX) */
            /* copy PPD actions from hardware state */
            for (index = 0, offset = 0;
                 index < unitData->wb.ppdActionLimit;
                 index++) {
                if (offset < SOC_PPD_FP_NOF_ACTIONS_PER_DB_MAX) {
                    if (_BCM_DPP_FIELD_PPD_ACTION_VALID(entInfoTc.actions[index].type)) {
                        /* this PPD action is valid */
                        if (SOC_PPD_FP_ACTION_TYPE_EGR_TRAP == entInfoTc.actions[index].type) {
                            /* egress trap needs to be treated specially */
                            entryTempIntTcam->egrTrapProfile = entInfoTc.actions[index].val;
    #ifdef BCM_ARAD_SUPPORT
                            if (SOC_IS_ARAD(unit)) {
                                entryTempIntTcam->egrTrapProfile = entInfoTc.actions[index].val >> 1;
                                if (entryTempIntTcam->egrTrapProfile != 0 /* Profile 0 not valid */) {
                                    entryTempIntTcam->entryCmn.entryFlags |= _BCM_DPP_FIELD_ENTRY_EGR_PRO;
                                }
                            } else
    #endif /* def BCM_ARAD_SUPPORT */
    #ifdef BCM_PETRAB_SUPPORT
                            if (SOC_IS_PETRAB(unit)) {
                                entryTempIntTcam->entryCmn.entryFlags |= _BCM_DPP_FIELD_ENTRY_EGR_PRO;
                                entryTempIntTcam->egrTrapProfile = entInfoTc.actions[index].val;
                            } else
    #endif /* def BCM_PETRAB_SUPPORT */
                            {
                                unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_RESTORE_FAIL;
                                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                                                 (_BSL_BCM_MSG_NO_UNIT("unit %d unable"
                                                                   " to determine"
                                                                   " hardware type"
                                                                   " for egress"
                                                                   " trap action"),
                                                  unit));
                            }
                        } else { /* if (action is egress trap) */
                            /* not egress trap */
                            entryTempIntTcam->tcActP[offset].hwType = entInfoTc.actions[index].type;
                            entryTempIntTcam->tcActP[offset].hwParam = entInfoTc.actions[index].val;
                            offset++;
                        } /* if (action is egress trap) */
                    } /* if (action is valid) */
                } else { /* if (offset < SOC_PPD_FP_NOF_ACTIONS_PER_DB_MAX) */
                    if (_BCM_DPP_FIELD_PPD_ACTION_VALID(entInfoTc.actions[index].type)) {
                        /* this PPD action is valid but there are too many */
                        unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_RESTORE_FAIL;
                        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                                         (_BSL_BCM_MSG_NO_UNIT("unit %d entry %u"
                                                           " claims %u PPD actions"
                                                           " but only supports %u"
                                                           " PPD actions"),
                                          unit,
                                          entry + _BCM_DPP_FIELD_ENT_BIAS(unit, InternalTcam),
                                          offset,
                                          SOC_PPD_FP_NOF_ACTIONS_PER_DB_MAX));
                    }
                } /* if (offset < SOC_PPD_FP_NOF_ACTIONS_PER_DB_MAX) */
            } /* for (all PPD actions) */
            if (entryTempIntTcam->entryCmn.entryFlags & _BCM_DPP_FIELD_ENTRY_EGR_PRO) {
                /* entry uses egress profile action; need to parse the profile */
                ppdr = soc_ppd_trap_eg_profile_info_get(unitData->unitHandle,
                                                        entryTempIntTcam->egrTrapProfile,
                                                        &egrProfile);
                result = handle_sand_result(ppdr);
                if (BCM_E_NONE != result) {
                    unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_RESTORE_FAIL;
                    BCMDNX_ERR_EXIT_MSG(result,
                                     (_BSL_BCM_MSG_NO_UNIT("unit %d unable to read"
                                                       " egress profile %d:"
                                                       " %d (%s)"),
                                      unit,
                                      entryTempIntTcam->egrTrapProfile,
                                      result,
                                      _SHR_ERRMSG(result)));
                }
                if (egrProfile.bitmap_mask & SOC_PPD_TRAP_ACTION_PROFILE_OVERWRITE_DEST) {
                    if (offset < SOC_PPD_FP_NOF_ACTIONS_PER_DB_MAX) {
                        entryTempIntTcam->tcActP[offset].hwFlags = SOC_PPD_TRAP_ACTION_PROFILE_OVERWRITE_DEST;
                        entryTempIntTcam->tcActP[offset].hwType = SOC_PPD_FP_ACTION_TYPE_EGR_TRAP;
                        entryTempIntTcam->tcActP[offset].hwParam = egrProfile.out_tm_port;
                    }
                    offset++;
                }
                if (egrProfile.bitmap_mask & SOC_PPD_TRAP_ACTION_PROFILE_OVERWRITE_DP) {
                    if (offset < SOC_PPD_FP_NOF_ACTIONS_PER_DB_MAX) {
                        entryTempIntTcam->tcActP[offset].hwFlags = SOC_PPD_TRAP_ACTION_PROFILE_OVERWRITE_DP;
                        entryTempIntTcam->tcActP[offset].hwType = SOC_PPD_FP_ACTION_TYPE_EGR_TRAP;
                        entryTempIntTcam->tcActP[offset].hwParam = egrProfile.cos_info.dp;
                    }
                    offset++;
                }
                if (egrProfile.bitmap_mask & SOC_PPD_TRAP_ACTION_PROFILE_OVERWRITE_CUD) {
                    if (offset < SOC_PPD_FP_NOF_ACTIONS_PER_DB_MAX) {
                        entryTempIntTcam->tcActP[offset].hwFlags = SOC_PPD_TRAP_ACTION_PROFILE_OVERWRITE_CUD;
                        entryTempIntTcam->tcActP[offset].hwType = SOC_PPD_FP_ACTION_TYPE_EGR_TRAP;
    #ifdef BCM_ARAD_SUPPORT
                            if (SOC_IS_ARAD(unit)) {
                                entryTempIntTcam->tcActP[offset].hwParam = egrProfile.header_data.cud;
                            } else
    #endif
                            {
                                entryTempIntTcam->tcActP[offset].hwParam = egrProfile.cud;
                            }
                    }
                    offset++;
                }
                if (egrProfile.bitmap_mask &
                    (~(SOC_PPD_TRAP_ACTION_PROFILE_OVERWRITE_DEST |
                       SOC_PPD_TRAP_ACTION_PROFILE_OVERWRITE_DP |
                       SOC_PPD_TRAP_ACTION_PROFILE_OVERWRITE_CUD))) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                                     (_BSL_BCM_MSG_NO_UNIT("unit %d entry %u egress"
                                                       " trap %u includes unknown"
                                                       " flags %08X on egress"
                                                       " trap action"),
                                      unit,
                                      entry + _BCM_DPP_FIELD_ENT_BIAS(unit, InternalTcam),
                                      entryTempIntTcam->egrTrapProfile,
                                      egrProfile.bitmap_mask &
                                      (~(SOC_PPD_TRAP_ACTION_PROFILE_OVERWRITE_DEST |
                                         SOC_PPD_TRAP_ACTION_PROFILE_OVERWRITE_DP |
                                         SOC_PPD_TRAP_ACTION_PROFILE_OVERWRITE_CUD))));
                }
                if (offset > SOC_PPD_FP_NOF_ACTIONS_PER_DB_MAX) {
                    unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_RESTORE_FAIL;
                    BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                                     (_BSL_BCM_MSG_NO_UNIT("unit %d entry %u"
                                                       " claims %u PPD actions"
                                                       " but only supports %u"
                                                       " PPD actions"),
                                      unit,
                                      entry + _BCM_DPP_FIELD_ENT_BIAS(unit, InternalTcam),
                                      offset,
                                      SOC_PPD_FP_NOF_ACTIONS_PER_DB_MAX));
                }
            } /* if (entry uses egress profile) */
            /* fill in remaining unused PPD actions */
            while (offset < SOC_PPD_FP_NOF_ACTIONS_PER_DB_MAX) {
                entryTempIntTcam->tcActP[offset].hwType = SOC_PPD_NOF_FP_ACTION_TYPES;
                offset++;
            }
            /* copy qualifiers from hardware */
            BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_entry_qual_wb_load(unitData,
                                                                      entry + _BCM_DPP_FIELD_ENT_BIAS(unit, InternalTcam),
                                                                      &(entryTempIntTcam->entryCmn.entryQual[0]),
                                                                      &(entInfoTc.qual_vals[0])));
        }/* if (!isExternal) */
        break;
    default:
        unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_VERSION_FAIL;
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d unexpected warm boot"
                                           " version %04X\n"),
                          unit,
                          unitData->wb.version));
    } /* switch (unitData->wb_version) */
    /* commit the entry if we get to here */
    sal_memcpy(_BCM_DPP_FIELD_TCAM_ENTRY(isExternalTcam, entry), entryTemp, _BCM_DPP_FIELD_TCAM_ENTRY_SIZE(isExternalTcam));
    if (!isExternalTcam) {
        /* recover correct order from PPD layer priority (strict priority) */
        for (current = groupData->entryHead;
             (current < unitData->entryTcLimit) &&
             (unitData->entryTc[entry].entryCmn.hwPriority >
              unitData->entryTc[current].entryCmn.hwPriority);
             ) {
            /*
             *  ...as long as there are more entries, and the current entry
             *  priority value is greater than the new entry priority, examine the
             *  next entry in the group.
             */
            current = unitData->entryTc[current].entryCmn.entryNext;
        }
    }
    else {
        current = groupData->entryHead;
    }
    entryIdx = _BCM_DPP_FIELD_TCAM_ENTRY_NEXT_PTR(isExternalTcam, entry);
    *entryIdx = current;
    if (current < _BCM_DPP_FIELD_TCAM_ENTRY_LIMIT(isExternalTcam)) {
        entryIdx = _BCM_DPP_FIELD_TCAM_ENTRY_PREV_PTR(isExternalTcam, entry);
        *entryIdx = _BCM_DPP_FIELD_TCAM_ENTRY_PREV(isExternalTcam, current);
        if (_BCM_DPP_FIELD_TCAM_ENTRY_PREV(isExternalTcam, entry) < _BCM_DPP_FIELD_TCAM_ENTRY_LIMIT(isExternalTcam)) {
            entryIdx = _BCM_DPP_FIELD_TCAM_ENTRY_NEXT_PTR(isExternalTcam, _BCM_DPP_FIELD_TCAM_ENTRY_PREV(isExternalTcam, entry));
            *entryIdx = entry;
        }
        entryIdx = _BCM_DPP_FIELD_TCAM_ENTRY_PREV_PTR(isExternalTcam, current);
        *entryIdx = entry;
    } else {
        entryIdx = _BCM_DPP_FIELD_TCAM_ENTRY_PREV_PTR(isExternalTcam, entry);
        *entryIdx = groupData->entryTail;
        if (groupData->entryTail < _BCM_DPP_FIELD_TCAM_ENTRY_LIMIT(isExternalTcam)) {
            entryIdx = _BCM_DPP_FIELD_TCAM_ENTRY_NEXT_PTR(isExternalTcam, groupData->entryTail);
            *entryIdx = entry;
        }
        groupData->entryTail = entry;
    }
    if (current == groupData->entryHead) {
        groupData->entryHead = entry;
    }
    groupData->entryCount++;
    unitData->stageD[unitData->stageD[groupData->stage].entryTcSh1].entryCount++;
    unitData->stageD[unitData->stageD[groupData->stage].entryTcSh1].entryElems += unitData->stageD[groupData->stage].modeBits[groupData->grpMode]->entryCount;
    if (isExternalTcam) {
        unitData->entryExtTcCount++;
    }
    else {
        unitData->entryIntTcCount++;
    }
exit:
    BCM_FREE(entryTemp);
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *    _bcm_dpp_field_tcam_entry_all_wb_load
 *  Purpose
 *    Load the TCAM entry information from the backing store buffer and
 *    hardware
 *  Arguments
 *    IN unitData = pointer to the unit information
 *  Results
 *    bcm_error_t (cast as int)
 *      BCM_E_NONE if successful
 *      BCM_E_* otherwise as appropriate
 *  Notes
 */
STATIC int
_bcm_dpp_field_tcam_entry_all_wb_load(_bcm_dpp_field_info_t *unitData)
{
    _bcm_dpp_field_ent_idx_t entry;
    _bcm_dpp_field_ent_idx_t freeTail = unitData->entryTcLimit;
    _bcm_dpp_field_entry_t *entryDataIntTcam;
#if defined(INCLUDE_KBP) && !defined(BCM_88030)
    _bcm_dpp_field_entry_ext_t *entryDataExtTcam;
#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030) */
    int result;

    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;

    if (0 == unitData->wb.offsetTcamEntry) {
        unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_FORMAT_FAIL;
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d does not appear to support"
                                           " TCAM entry data in"
                                           " backing store version %04X\n"),
                          unit,
                          unitData->wb.version));
    }
#if defined(INCLUDE_KBP) && !defined(BCM_88030)
    if (SOC_DPP_IS_ELK_ENABLE(unit)) {
        if (0 == unitData->wb.offsetExtTcamEntry) {
            unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_FORMAT_FAIL;
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d does not appear to support"
                                               " external TCAM entry data in"
                                               " backing store version %04X\n"),
                              unit,
                              unitData->wb.version));
        }
    }
#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030) */
#if _BCM_PPD_FIELD_WARM_BOOT_READ_TRACKING
    LOG_DEBUG(BSL_LS_BCM_INIT,
              (BSL_META_U(unit,
                          "unit %d recovering all TCAM entry"
                          " information from warm boot buffer\n"),
               unit));
#endif /* _BCM_PPD_FIELD_WARM_BOOT_READ_TRACKING */
    /* clear direct extraction entry state */
    sal_memset(&(unitData->entryTc[0]),
               0x00,
               sizeof(unitData->entryTc[0]) * unitData->entryTcLimit);
    unitData->entryIntTcFree = unitData->entryTcLimit;
    unitData->entryIntTcCount = 0;
    /* traverse the groups, getting them back from hardware + backing store */
    for (entry = 0; entry < unitData->wb.tcamEntryLimit; entry++) {
        /* allow HW access, load needs to read memory in order to restore sw state, mem read uses write */
        SOC_DPP_ALLOW_WARMBOOT_WRITE((_bcm_dpp_field_tcam_entry_wb_load(unitData, entry, FALSE)), result);
        if (BCM_E_NONE != result) {
            unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_RESTORE_FAIL;
            BCMDNX_ERR_EXIT_MSG(result,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d unable to recover"
                                               " TCAM entry %u"
                                               " data from backing store:"
                                               " %d (%s)"),
                              unit,
                              entry + _BCM_DPP_FIELD_ENT_BIAS(unit, InternalTcam),
                              result,
                              _SHR_ERRMSG(result)));
        }
    } /* for (group = 0; group < unitData->wb.groupLimit; group++) */
    /* tidy up the unallocated direct extraction entries */
    for (entry = 0; entry < unitData->entryTcLimit; entry++) {
        entryDataIntTcam = &(unitData->entryTc[entry]);
        if (0 == (entryDataIntTcam->entryCmn.entryFlags &
                  _BCM_DPP_FIELD_ENTRY_IN_USE)) {
            entryDataIntTcam->entryCmn.entryNext = unitData->entryTcLimit;
            entryDataIntTcam->entryCmn.entryGroup = unitData->groupLimit;
            entryDataIntTcam->entryCmn.entryPrev = freeTail;
            if (freeTail < unitData->entryTcLimit) {
                unitData->entryTc[freeTail].entryCmn.entryNext = entry;
            } else { /* if (freeTail < unitData->groupLimit) */
                unitData->entryIntTcFree = entry;
            } /* if (freeTail < unitData->groupLimit) */
            freeTail = entry;
        } /* if (group is not in use) */
    } /* for (all groups this unit) */

#if defined(INCLUDE_KBP) && !defined(BCM_88030)
    if (SOC_DPP_IS_ELK_ENABLE(unit)) {
        /* clear direct extraction entry state */
        sal_memset(&(unitData->entryExtTc[0]),
                0x00,
                sizeof(unitData->entryExtTc[0]) * unitData->entryExtTcLimit);
        unitData->entryExtTcFree = unitData->entryExtTcLimit;
        unitData->entryExtTcCount = 0;
     /* traverse the groups, getting them back from hardware + backing store */
        for (entry = 0; entry < unitData->wb.extTcamEntryLimit; entry++) {
            result = _bcm_dpp_field_tcam_entry_wb_load(unitData, entry, TRUE);
            if (BCM_E_NONE != result) {
                     unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_RESTORE_FAIL;
                     BCMDNX_ERR_EXIT_MSG(result,
                                      (_BSL_BCM_MSG_NO_UNIT("unit %d unable to recover"
                                                        " external TCAM entry %u"
                                                        " data from backing store:"
                                                        " %d (%s)"),
                                       unit,
                                       entry + _BCM_DPP_FIELD_ENT_BIAS(unitData->unit, ExternalTcam),
                                       result,
                                       _SHR_ERRMSG(result)));
            }
        } /* for (group = 0; group < unitData->wb.groupLimit; group++) */
    
        /* tidy up the unallocated entries */
        for (entry = 0; entry < unitData->entryExtTcLimit; entry++) {
            entryDataExtTcam = &(unitData->entryExtTc[entry]);
            if (0 == (entryDataExtTcam->entryCmn.entryFlags &
                      _BCM_DPP_FIELD_ENTRY_IN_USE)) {
              entryDataExtTcam->entryCmn.entryNext = unitData->entryExtTcLimit;
                entryDataExtTcam->entryCmn.entryGroup = unitData->groupLimit;
                entryDataExtTcam->entryCmn.entryPrev = freeTail;
                if (freeTail < unitData->entryExtTcLimit) {
                    unitData->entryExtTc[freeTail].entryCmn.entryNext = entry;
                } else { /* if (freeTail < unitData->groupLimit) */
                    unitData->entryExtTcFree = entry;
                } /* if (freeTail < unitData->groupLimit) */
                freeTail = entry;
            } /* if (group is not in use) */
        } /* for (all groups this unit) */
    }
    
#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030) */
exit:
    BCMDNX_FUNC_RETURN;
}

void
_bcm_dpp_field_dir_ext_entry_wb_save(_bcm_dpp_field_info_t *unitData,
                                     _bcm_dpp_field_ent_idx_t entry,
                                     uint8 *data,
                                     unsigned int *size)
{
    _bcm_dpp_field_entry_dir_ext_t *entryData = &(unitData->entryDe[entry]);
    _bcm_dpp_wb_1_0_field_entry_dir_ext_t *wbData;
    unsigned int index;
    unsigned int entrySize;
    int result;
	int unit = unitData->unit;

    if ((!size) && (0 == unitData->wb.offsetDirExtEntry)) {
        LOG_ERROR(BSL_LS_BCM_INIT,
                  (BSL_META_U(unit,
                              "unit %d does not appear to support Direct"
                              " Extraction entry data in backing store"
                              " version %04X\n"),
                   unit,
                   unitData->wb.version));
        unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_FORMAT_FAIL;
        return;
    }
    if (entry >= unitData->wb.dirextEntryLimit) {
        LOG_ERROR(BSL_LS_BCM_INIT,
                  (BSL_META_U(unit,
                              "unit %d backing store only supports Direct"
                              " Extraciton entries %d through %d\n"),
                   unit,
                   _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt),
                   _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt) + unitData->wb.dirextEntryLimit - 1));
        unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_FORMAT_FAIL;
        return;
    }
    switch (unitData->wb.version) {
    case _BCM_DPP_WB_FIELD_VERSION_1_0:
        entrySize = (sizeof(*wbData) +
                     (sizeof(wbData->dirExtAction[0]) *
                      (unitData->wb.ppdActionLimit - 1)));
        entrySize = _BCM_DPP_WB_FIELD_ALIGNED_SIZE(entrySize);
        if (size) {
            *size = entrySize;
        } else { /* if (size) */
            wbData = sal_alloc(entrySize, "dir_ext entry backing store temp");
            if (wbData) {
                sal_memset(wbData, 0x00, entrySize);
                if (entryData->entryCmn.entryFlags &
                    _BCM_DPP_FIELD_ENTRY_IN_HW) {
                    wbData->entryCmn.entryFlags = entryData->entryCmn.entryFlags;
                    wbData->entryCmn.entryPriority = entryData->entryCmn.entryPriority;
                    wbData->entryCmn.hwHandle = entryData->entryCmn.hwHandle;
                    wbData->entryCmn.entryGroup = entryData->entryCmn.entryGroup;
                    for (index = 0;
                         index < SOC_PPD_FP_NOF_ACTIONS_PER_DB_MAX;
                         index++) {
                        if (bcmFieldActionCount >
                            entryData->deAct[index].bcmType) {
                            if (unitData->wb.bcmFieldActionCount >
                                entryData->deAct[index].bcmType) {
                                if (index < unitData->wb.ppdActionLimit) {
                                    wbData->dirExtAction[index].bcmType = entryData->deAct[index].bcmType;
                                } else { /* if (act idx valid in back store) */
                                    LOG_ERROR(BSL_LS_BCM_INIT,
                                              (BSL_META_U(unit,
                                                          "unit %d backing"
                                                          " store only stores"
                                                          " %u actions for"
                                                          " entry %u, but"
                                                          " entry %u has an"
                                                          " action at"
                                                          " index %u\n"),
                                               unit,
                                               unitData->wb.ppdActionLimit,
                                               entry + _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt),
                                               entry + _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt),
                                               index));
                                    unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_FORMAT_FAIL;
                                } /* if (act idx valid in back store) */
                            } else { /* if (action is valid in backing store) */
                                LOG_ERROR(BSL_LS_BCM_INIT,
                                          (BSL_META_U(unit,
                                                      "unit %d backing store"
                                                      " value of"
                                                      " bcmFieldActionCount"
                                                      " (%d) prohibits entry"
                                                      " %u action %s (%d) at"
                                                      " index %u\n"),
                                           unit,
                                           unitData->wb.bcmFieldActionCount,
                                           entry + _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt),
                                           _bcm_dpp_field_action_name[entryData->deAct[index].bcmType],
                                           entryData->deAct[index].bcmType,
                                           index));
                                unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_FORMAT_FAIL;
                                wbData->dirExtAction[index].bcmType = unitData->wb.bcmFieldActionCount;
                            } /* if (action is valid in backing store) */
                        } else { /* if (action is valid now) */
                            if (index < unitData->wb.ppdActionLimit) {
                                wbData->dirExtAction[index].bcmType = unitData->wb.bcmFieldActionCount;
                            }
                        } /* if (action is valid now) */
                    } /* for (all actions in this warm boot setup) */
                    while (index < unitData->wb.ppdActionLimit) {
                        wbData->dirExtAction[index].bcmType = unitData->wb.bcmFieldActionCount;
                        index++;
                    } /* while (offset < unitData->wb_tcamBcmActionLimit) */
                } /* if (entry is in hardware) */
#if _BCM_PPD_FIELD_WARM_BOOT_WRITE_TRACKING
                LOG_DEBUG(BSL_LS_BCM_INIT,
                          (BSL_META_U(unit,
                                      "unit %d backing store write Dir Ext"
                                      " entry %u info at %u for %u bytes\n"),
                           unit,
                           entry,
                           unitData->wb.offsetDirExtEntry + (entry * entrySize),
                           entrySize));
#endif /* _BCM_PPD_FIELD_WARM_BOOT_WRITE_TRACKING */
                if (data) {
                    sal_memcpy(&(data[unitData->wb.offsetDirExtEntry +
                                      (entry * entrySize)]),
                               wbData,
                               entrySize);
                } else { /* if (data) */
                    sal_memcpy(&(unitData->wb.buffer[unitData->wb.offsetDirExtEntry +
                                                     (entry * entrySize)]),
                               wbData,
                               entrySize);                               
					if (!(SOC_CONTROL(unit)->autosync) && !wb_field_is_scheduled_sync[unit]) {
					        /* mark dirty state. Currently no other processing done           */
					        /* note that there is no in module dirty bit optimization. use global dirty bit */
					        BCM_DPP_WB_DEV_FIELD_DIRTY_BIT_SET(unit);								        
					}
					else{
	                    result = soc_scache_commit_specific_data(unit,
	                                                             unitData->wb.handle,
	                                                             entrySize,
	                                                             (uint8*)wbData,
	                                                             unitData->wb.offsetDirExtEntry +
	                                                             (entry * entrySize));
	                    if (SOC_E_NONE != result) {
	                        LOG_ERROR(BSL_LS_BCM_INIT,
                                          (BSL_META_U(unit,
                                                      "unit %d unable to update backing"
                                                      " store for entry %d: %d (%s)\n"),
                                           unit,
                                           entry + _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt),
                                           result,
                                           _SHR_ERRMSG(result)));
	                        unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_COMMIT_FAIL;
	                    }
	                }
                } /* if (data) */
                sal_free(wbData);
            } else { /* if (wbData) */
                LOG_ERROR(BSL_LS_BCM_INIT,
                          (BSL_META_U(unit,
                                      "unit %d unable to allocate temporary"
                                      " space to update backing store for"
                                      " field entry %u"),
                           unit,
                           entry + _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt)));
                unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_COMMIT_FAIL;
            } /* if (wbData) */
        } /* if (size) */
        break;
    default:
        LOG_ERROR(BSL_LS_BCM_INIT,
                  (BSL_META_U(unit,
                              "unit %d unexpected warm boot version %04X\n"),
                   unit,
                   unitData->wb.version));
        unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_VERSION_FAIL;
        if (size) {
            *size = 0;
        }
    } /* switch (unitData->wb_version) */
}

/*
 *  Function
 *    _bcm_dpp_field_dir_ext_entry_wb_load
 *  Purpose
 *    Load a direct extraction entry's information from the backing store
 *    buffer and hardware
 *  Arguments
 *    IN unitData = pointer to the unit information
 *  Results
 *    bcm_error_t (cast as int)
 *      BCM_E_NONE if successful
 *      BCM_E_* otherwise as appropriate
 *  Notes
 *    Cascaded groups may not have their pair available from the PPD, so this
 *    may not be restored; it needs to be scanned after all groups are loaded.
 */
STATIC int
_bcm_dpp_field_dir_ext_entry_wb_load(_bcm_dpp_field_info_t *unitData,
                                     _bcm_dpp_field_ent_idx_t entry)
{
    SOC_PPD_FP_DIR_EXTR_ENTRY_INFO *entInfoDe = NULL;
    _bcm_dpp_field_entry_dir_ext_t *entryTemp = NULL;
    _bcm_dpp_field_entry_dir_ext_t *entryData;
    _bcm_dpp_field_group_t *groupData;
    _bcm_dpp_wb_1_0_field_entry_dir_ext_t *wbData;
    _bcm_dpp_field_ent_idx_t current;
    SOC_PPD_FP_ACTION_TYPE *ppdAct = NULL;
    unsigned int index;
    unsigned int offset;
    unsigned int adjust;
    unsigned int entrySize;
    int result;
    int temp;
    uint8 okay;
    uint32 ppdr;
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;

    if (entry >= unitData->wb.dirextEntryLimit) {
        unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_FORMAT_FAIL;
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d tried to read backing"
                                           " store for direct extraction entry"
                                           " %u but only direct extraction"
                                           " entries %u..%u in backing store"),
                          unit,
                          entry + _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt),
                          _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt),
                          unitData->wb.dirextEntryLimit + _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt)));
    }
    if (entry > unitData->entryDeLimit) {
        unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_RESTORE_FAIL;
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d tried to read backing"
                                           " store for direct extraction entry"
                                           " %u but only supports direct"
                                           " extraction entries %u..%u"),
                          unit,
                          entry + _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt),
                          _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt),
                          unitData->entryDeLimit + _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt)));
    }
    entryData = &(unitData->entryDe[entry]);

    BCMDNX_ALLOC(entryTemp, sizeof(_bcm_dpp_field_entry_dir_ext_t), "_bcm_dpp_field_dir_ext_entry_wb_load.entryTemp");
    if (entryTemp == NULL) {        
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG_NO_UNIT("failed to allocate memory")));
    }
    sal_memset(entryTemp, 0x00, sizeof(*entryTemp));
    switch (unitData->wb.version) {
    case _BCM_DPP_WB_FIELD_VERSION_1_0:
        entrySize = (sizeof(*wbData) +
                     (sizeof(wbData->dirExtAction[0]) *
                      (unitData->wb.ppdActionLimit - 1)));
        entrySize = _BCM_DPP_WB_FIELD_ALIGNED_SIZE(entrySize);
        wbData = (_bcm_dpp_wb_1_0_field_entry_dir_ext_t*)&(unitData->wb.buffer[unitData->wb.offsetDirExtEntry + (entrySize * entry)]);
        if (0 == (wbData->entryCmn.entryFlags & _BCM_DPP_FIELD_ENTRY_IN_HW)) {
            /* nothing to recover here since not in hardware */
            BCM_EXIT;
        }
#if _BCM_PPD_FIELD_WARM_BOOT_READ_TRACKING
        LOG_DEBUG(BSL_LS_BCM_INIT,
                  (BSL_META_U(unit,
                              "unit %d recovering direct extraction"
                              " entry %u information from warm boot buffer\n"),
                   unit,
                   entry + _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt)));
#endif /* _BCM_PPD_FIELD_WARM_BOOT_READ_TRACKING */
        /* populate from backing store */
        entryTemp->entryCmn.entryFlags = wbData->entryCmn.entryFlags;
        entryTemp->entryCmn.entryPriority = wbData->entryCmn.entryPriority;
        entryTemp->entryCmn.hwHandle = wbData->entryCmn.hwHandle;
        entryTemp->entryCmn.entryGroup = wbData->entryCmn.entryGroup;
        if (entryTemp->entryCmn.entryGroup >= unitData->wb.groupLimit) {
            unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_FORMAT_FAIL;
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d entry %u claims to be"
                                               " in group %u, but backing store"
                                               " only supports %u groups"),
                              unit,
                              entry + _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt),
                              entryTemp->entryCmn.entryGroup,
                              unitData->wb.groupLimit));
        }
        if (entryTemp->entryCmn.entryGroup >= unitData->groupLimit) {
            unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_RESTORE_FAIL;
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d entry %u claims to be"
                                               " in group %u, but only"
                                               " supports %u groups"),
                              unit,
                              entry + _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt),
                              entryTemp->entryCmn.entryGroup,
                              unitData->groupLimit));
        }
        groupData = &(unitData->groupD[entryTemp->entryCmn.entryGroup]);
        /* get state from PPD layer */
        BCMDNX_ALLOC(entInfoDe, sizeof(SOC_PPD_FP_DIR_EXTR_ENTRY_INFO), "_bcm_dpp_field_dir_ext_entry_wb_load.entInfoDe");
        if (entInfoDe == NULL) {        
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG_NO_UNIT("failed to allocate memory")));
        }
        ppdr = soc_ppd_fp_direct_extraction_entry_get(unitData->unitHandle,
                                                      groupData->hwHandle,
                                                      entryTemp->entryCmn.hwHandle,
                                                      &okay,
                                                      entInfoDe);
        result = handle_sand_result(ppdr);
        if (!okay) {
            result = BCM_E_FAIL;
        }
        if (BCM_E_NONE != result) {
            unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_RESTORE_FAIL;
            BCMDNX_ERR_EXIT_MSG(result,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d unable to read group"
                                               " %u entry %u (%u): %d (%s)"),
                              unit,
                              entryTemp->entryCmn.entryGroup,
                              entry + _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt),
                              entryTemp->entryCmn.hwHandle,
                              result,
                              _SHR_ERRMSG(result)));
        }
        /* mark this PPD layer ID as in use */
        temp = entryTemp->entryCmn.hwHandle;
        result = shr_mres_alloc(unitData->hwResIds,
                                unitData->stageD[groupData->stage].entryDeRes,
                                SHR_RES_ALLOC_WITH_ID,
                                1,
                                &temp);
        if (BCM_E_NONE != result) {
            unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_RESTORE_FAIL;
            BCMDNX_ERR_EXIT_MSG(result,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d stage %u unable to"
                                               " allocate direct extraction"
                                               " hardware entry ID %u"),
                              unit,
                              groupData->stage,
                              entryTemp->entryCmn.hwHandle));
        }
        /* copy actions from backing store and hardware */
        for (index = 0, offset = 0;
             index < unitData->wb.ppdActionLimit;
             index++) {
            if (offset < SOC_PPD_FP_NOF_ACTIONS_PER_DB_MAX) {
                if (wbData->dirExtAction[index].bcmType < bcmFieldActionCount) {
                    entryTemp->deAct[offset].bcmType = wbData->dirExtAction[index].bcmType;
                    /* map this action to PPD layer */
                    result = _bcm_dpp_ppd_act_from_bcm_act(unitData,
                                                           groupData->stage,
                                                           entryTemp->deAct[offset].bcmType,
                                                           &ppdAct);
                    if (BCM_E_NONE != result) {
                        unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_RESTORE_FAIL;
                        BCMDNX_ERR_EXIT_MSG(result,
                                         (_BSL_BCM_MSG_NO_UNIT("unit %d unable to"
                                                           " find description"
                                                           " for action %d"
                                                           " (%s)"),
                                          unit,
                                          entryTemp->deAct[offset].bcmType,
                                          _bcm_dpp_field_action_name[entryTemp->deAct[offset].bcmType]));
                    }
                    if ((0 > ppdAct[0]) ||
                        (!_BCM_DPP_FIELD_PPD_ACTION_VALID(ppdAct[0]))) {
                        unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_RESTORE_FAIL;
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                         (_BSL_BCM_MSG_NO_UNIT("unit %d does not"
                                                           " support action"
                                                           " %d (%s) for stage"
                                                           " %u types %08X"),
                                          unit,
                                          entryTemp->deAct[offset].bcmType,
                                          _bcm_dpp_field_action_name[entryTemp->deAct[offset].bcmType],
                                          groupData->stage,
                                          groupData->groupTypes));
                    }
                    /* find this action in the PPD data */
                    for (adjust = 0;
                         adjust < unitData->wb.ppdActionLimit;
                         adjust++) {
                        if (entInfoDe->actions[adjust].type == ppdAct[0]) {
                            /* found it; copy it to proper place */
                            sal_memcpy(&(entryTemp->deAct[offset].hwParam),
                                       &(entInfoDe->actions[adjust]),
                                       sizeof(entryTemp->deAct[offset].hwParam));
                            /* poison it so not found again */
                            entInfoDe->actions[adjust].type = SOC_PPD_NOF_FP_ACTION_TYPES;
                            break;
                        } /* if (entInfoDe->actions[adjust].type == ppdAct[0]) */
                    } /* for (all actions in PPD data) */
                    if (adjust >= unitData->wb.ppdActionLimit) {
                        unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_RESTORE_FAIL;
                        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                                         (_BSL_BCM_MSG_NO_UNIT("unit %d group %u"
                                                           " entry %u unable to"
                                                           " find PPD action"
                                                           " %s (%d) for BCM"
                                                           " action %s (%d)"
                                                           " at index %u"),
                                          unit,
                                          entryTemp->entryCmn.entryGroup,
                                          entry + _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt),
                                          SOC_PPD_FP_ACTION_TYPE_to_string(ppdAct[0]),
                                          ppdAct[0],
                                          _bcm_dpp_field_action_name[entryTemp->deAct[offset].bcmType],
                                          entryTemp->deAct[offset].bcmType,
                                          index));
                    } /* if (adjust >= unitData->wb.ppdActionLimit) */
                    offset++;
                } /* if (this action is valid in BCM data) */
            } else { /* if (offset < SOC_PPD_FP_NOF_ACTIONS_PER_DB_MAX) */
                if (wbData->dirExtAction[index].bcmType < bcmFieldActionCount) {
                    unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_RESTORE_FAIL;
                    BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                                     (_BSL_BCM_MSG_NO_UNIT("unit %d group %u entry"
                                                       " %u has action %s (%u)"
                                                       " at index %u but there"
                                                       " is no room for it"),
                                      unit,
                                      entryTemp->entryCmn.entryGroup,
                                      entry + _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt),
                                      _bcm_dpp_field_action_name[wbData->dirExtAction[index].bcmType],
                                      wbData->dirExtAction[index].bcmType,
                                      index));
                } /* if (this action is valid in BCM data) */
            } /* if (offset < SOC_PPD_FP_NOF_ACTIONS_PER_DB_MAX) */
        } /* for (all actions in backing store) */
        /* fill in actions not included in backing store */
        while (offset < SOC_PPD_FP_NOF_ACTIONS_PER_DB_MAX) {
            entryTemp->deAct[offset].bcmType = bcmFieldActionCount;
            entryTemp->deAct[offset].hwParam.type = SOC_PPD_NOF_FP_ACTION_TYPES;
            for (index = 0;
                 index < SOC_PPC_FP_DIR_EXTR_MAX_NOF_FIELDS;
                 index++) {
                entryTemp->deAct[offset].hwParam.fld_ext[index].type = SOC_PPD_NOF_FP_QUAL_TYPES;
            }
            offset++;
        } /* while (index < SOC_PPD_FP_NOF_ACTIONS_PER_DB_MAX) */
        /* copy qualifiers from hardware */
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_entry_qual_wb_load(unitData,
                                                                  entry + _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt),
                                                                  &(entryTemp->entryCmn.entryQual[0]),
                                                                  &(entInfoDe->qual_vals[0])));
        break;
    default:
        unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_VERSION_FAIL;
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d unexpected warm boot"
                                           " version %04X\n"),
                          unit,
                          unitData->wb.version));
    } /* switch (unitData->wb_version) */
    /* commit the entry if we get to here */
    sal_memcpy(entryData, entryTemp, sizeof(*entryData));
    for (current = groupData->entryHead;
         (current < unitData->entryTcLimit) &&
         (entryData->entryCmn.hwPriority >
          unitData->entryTc[current].entryCmn.hwPriority);
         ) {
        /*
         *  ...as long as there are more entries, and the current entry
         *  priority value is greater than the new entry priority, examine the
         *  next entry in the group.
         */
        current = unitData->entryTc[current].entryCmn.entryNext;
    }
    entryData->entryCmn.entryNext = current;
    if (current < unitData->entryDeLimit) {
        entryData->entryCmn.entryPrev = unitData->entryDe[current].entryCmn.entryPrev;
        if (entryData->entryCmn.entryPrev < unitData->entryDeLimit) {
            unitData->entryDe[entryData->entryCmn.entryPrev].entryCmn.entryNext = entry;
        }
        unitData->entryDe[current].entryCmn.entryPrev = entry;
    } else {
        entryData->entryCmn.entryPrev = groupData->entryTail;
        if (groupData->entryTail < unitData->entryDeLimit) {
            unitData->entryDe[groupData->entryTail].entryCmn.entryNext = entry;
        }
        groupData->entryTail = entry;
    }
    if (current == groupData->entryHead) {
        groupData->entryHead = entry;
    }
    groupData->entryCount++;
    unitData->stageD[unitData->stageD[groupData->stage].entryDeSh1].entryDeCount++;
    unitData->stageD[unitData->stageD[groupData->stage].entryDeSh1].entryDeElems += unitData->stageD[groupData->stage].modeBits[groupData->grpMode]->entryCount;
    unitData->entryDeCount++;
exit:
    BCM_FREE(entInfoDe);
    BCM_FREE(entryTemp);
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *    _bcm_dpp_field_dir_ext_entry_all_wb_load
 *  Purpose
 *    Load the direct extraction entry information from the backing store
 *    buffer and hardware
 *  Arguments
 *    IN unitData = pointer to the unit information
 *  Results
 *    bcm_error_t (cast as int)
 *      BCM_E_NONE if successful
 *      BCM_E_* otherwise as appropriate
 *  Notes
 */
STATIC int
_bcm_dpp_field_dir_ext_entry_all_wb_load(_bcm_dpp_field_info_t *unitData)
{
    _bcm_dpp_field_ent_idx_t entry;
    _bcm_dpp_field_ent_idx_t freeTail = unitData->entryDeLimit;
    _bcm_dpp_field_entry_dir_ext_t *entryData;
    int result;
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;
	
	unit = unitData->unit;

    if (0 == unitData->wb.offsetDirExtEntry) {
        unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_FORMAT_FAIL;
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d does not appear to support"
                                           " direct extraction entry data in"
                                           " backing store version %04X\n"),
                          unit,
                          unitData->wb.version));
    }
#if _BCM_PPD_FIELD_WARM_BOOT_READ_TRACKING
    LOG_DEBUG(BSL_LS_BCM_INIT,
              (BSL_META_U(unit,
                          "unit %d recovering all direct extraction entry"
                          " information from warm boot buffer\n"),
               unit));
#endif /* _BCM_PPD_FIELD_WARM_BOOT_READ_TRACKING */
    /* clear direct extraction entry state */
    sal_memset(&(unitData->entryDe[0]),
               0x00,
               sizeof(unitData->entryDe[0]) * unitData->entryDeLimit);
    unitData->entryDeFree = unitData->entryDeLimit;
    unitData->entryDeCount = 0;
    /* traverse the groups, getting them back from hardware + backing store */
    for (entry = 0; entry < unitData->wb.dirextEntryLimit; entry++) {
        result = _bcm_dpp_field_dir_ext_entry_wb_load(unitData, entry);
        if (BCM_E_NONE != result) {
            unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_RESTORE_FAIL;
            BCMDNX_ERR_EXIT_MSG(result,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d unable to recover"
                                               " direct extraction entry %u"
                                               " data from backing store:"
                                               " %d (%s)"),
                              unit,
                              entry + _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt),
                              result,
                              _SHR_ERRMSG(result)));
        }
    } /* for (group = 0; group < unitData->wb.groupLimit; group++) */
    /* tidy up the unallocated direct extraction entries */
    for (entry = 0; entry < unitData->entryDeLimit; entry++) {
        entryData = &(unitData->entryDe[entry]);
        if (0 == (entryData->entryCmn.entryFlags &
                  _BCM_DPP_FIELD_ENTRY_IN_USE)) {
            entryData->entryCmn.entryNext = unitData->entryDeLimit;
            entryData->entryCmn.entryGroup = unitData->groupLimit;
            entryData->entryCmn.entryPrev = freeTail;
            if (freeTail < unitData->entryDeLimit) {
                unitData->entryDe[freeTail].entryCmn.entryNext = entry;
            } else { /* if (freeTail < unitData->groupLimit) */
                unitData->entryDeFree = entry;
            } /* if (freeTail < unitData->groupLimit) */
            freeTail = entry;
        } /* if (group is not in use) */
    } /* for (all groups this unit) */
exit:
    BCMDNX_FUNC_RETURN;
}

void
_bcm_dpp_field_group_wb_save(_bcm_dpp_field_info_t *unitData,
                             _bcm_dpp_field_grp_idx_t group,
                             uint8 *data,
                             unsigned int *size)
{
    _bcm_dpp_field_group_t *groupData = &(unitData->groupD[group]);
    _bcm_dpp_wb_1_0_field_group_t *wbData;
    unsigned int groupSize;
    int result;
	int unit = unitData->unit;

    if ((!size) && (0 == unitData->wb.offsetGroup)) {
        LOG_ERROR(BSL_LS_BCM_INIT,
                  (BSL_META_U(unit,
                              "unit %d does not appear to support group"
                              " data in backing store version %04X\n"),
                   unit,
                   unitData->wb.version));
        unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_FORMAT_FAIL;
        return;
    }
    if (group >= unitData->wb.groupLimit) {
        LOG_ERROR(BSL_LS_BCM_INIT,
                  (BSL_META_U(unit,
                              "unit %d backing store only supports groups"
                              " %d through %d\n"),
                   unit,
                   0,
                   unitData->wb.groupLimit - 1));
        unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_FORMAT_FAIL;
        return;
    }
    switch (unitData->wb.version) {
    case _BCM_DPP_WB_FIELD_VERSION_1_0:
        groupSize = (sizeof(*wbData) +
                     (sizeof(uint32) *
                      (((unitData->wb.bcmFieldActionCount + 31) >> 5) - 1)));
        groupSize = _BCM_DPP_WB_FIELD_ALIGNED_SIZE(groupSize);
        if (size) {
            *size = groupSize;
        } else { /* if (size) */
            wbData = sal_alloc(groupSize, "group backing store temp");
            if (wbData) {
                sal_memset(wbData, 0x00, groupSize);
                if (groupData->groupFlags & _BCM_DPP_FIELD_GROUP_IN_HW) {
                    wbData->groupFlags = groupData->groupFlags;
                    wbData->grpMode = groupData->grpMode;
                    wbData->hwHandle = groupData->hwHandle;
                    wbData->predefKey = groupData->predefKey;
                    wbData->grpStage = unitData->devInfo->stage[groupData->stage].hwStageId;
                    _bcm_dpp_field_wb_bmp_put(unitData,
                                              &(groupData->oaset.w[0]),
                                              bcmFieldActionCount,
                                              &(wbData->oaset[0]),
                                              unitData->wb.bcmFieldActionCount,
                                              "actions in original action set",
                                              group,
                                              "group");
                } /* if (group is in hardware) */
#if _BCM_PPD_FIELD_WARM_BOOT_WRITE_TRACKING
                LOG_DEBUG(BSL_LS_BCM_INIT,
                          (BSL_META_U(unit,
                                      "unit %d backing store write group"
                                      " %u info at %u for %u bytes\n"),
                           unit,
                           group,
                           unitData->wb.offsetGroup + (group * groupSize),
                           groupSize));
#endif /* _BCM_PPD_FIELD_WARM_BOOT_WRITE_TRACKING */
                if (data) {
                    sal_memcpy(&(data[unitData->wb.offsetGroup +
                                      (group * groupSize)]),
                               wbData,
                               groupSize);
                } else { /* if (data) */
                    sal_memcpy(&(unitData->wb.buffer[unitData->wb.offsetGroup +
                                                     (group * groupSize)]),
                               wbData,
                               groupSize);
					if (!(SOC_CONTROL(unit)->autosync) && !wb_field_is_scheduled_sync[unit]) {
					        /* mark dirty state. Currently no other processing done           */
					        /* note that there is no in module dirty bit optimization. use global dirty bit */
					        BCM_DPP_WB_DEV_FIELD_DIRTY_BIT_SET(unit);								        
					}
					else{
	                    result = soc_scache_commit_specific_data(unit,
	                                                             unitData->wb.handle,
	                                                             groupSize,
	                                                             (uint8*)wbData,
	                                                             unitData->wb.offsetGroup +
	                                                             (group * groupSize));
	                    if (SOC_E_NONE != result) {
	                        LOG_ERROR(BSL_LS_BCM_INIT,
                                          (BSL_META_U(unit,
                                                      "unit %d unable to update backing"
                                                      " store for group %d: %d (%s)\n"),
                                           unit,
                                           group,
                                           result,
                                           _SHR_ERRMSG(result)));
	                        unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_COMMIT_FAIL;
	                    }
	                }
                } /* if (data) */
                sal_free(wbData);
            } else { /* if (wbData) */
                LOG_ERROR(BSL_LS_BCM_INIT,
                          (BSL_META_U(unit,
                                      "unit %d unable to allocate temporary"
                                      " space to update backing store for"
                                      " field group %u"),
                           unit,
                           group));
                unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_COMMIT_FAIL;
            } /* if (wbData) */
        } /* if (size) */
        break;
    default:
        LOG_ERROR(BSL_LS_BCM_INIT,
                  (BSL_META_U(unit,
                              "unit %d unexpected warm boot version %04X\n"),
                   unit,
                   unitData->wb.version));
        unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_VERSION_FAIL;
        if (size) {
            *size = 0;
        }
    } /* switch (unitData->wb_version) */
}

/*
 *  Function
 *    _bcm_dpp_field_group_wb_load
 *  Purpose
 *    Load a group's information from the backing store buffer and hardware
 *  Arguments
 *    IN unitData = pointer to the unit information
 *  Results
 *    bcm_error_t (cast as int)
 *      BCM_E_NONE if successful
 *      BCM_E_* otherwise as appropriate
 *  Notes
 *    Cascaded groups may not have their pair available from the PPD, so this
 *    may not be restored; it needs to be scanned after all groups are loaded.
 */
STATIC int
_bcm_dpp_field_group_wb_load(_bcm_dpp_field_info_t *unitData,
                             _bcm_dpp_field_grp_idx_t group)
{
    SOC_PPD_FP_DATABASE_INFO dbInfo;
    _bcm_dpp_field_group_t groupTemp;
    _bcm_dpp_field_group_t *groupData;
    _bcm_dpp_wb_1_0_field_group_t *wbData;
    _bcm_dpp_field_stage_idx_t stage;
    _bcm_dpp_field_grp_idx_t current;
    unsigned int groupSize;
    unsigned int index;
    uint32 ppdr;
    int result;
    int temp;
#if defined(INCLUDE_KBP) && !defined(BCM_88030)
    uint32
        pmf_pgm_bmp_used,
        pmf_pgm_ndx_min,
        pmf_pgm_ndx_max,
        key_size_in_bytes,
        pmf_pgm_bmp_new_sw;
    ARAD_KBP_DB_HANDLES
        db_handles_info;
    SOC_SAND_SUCCESS_FAILURE success;
#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030) */
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;

    if (group >= unitData->wb.groupLimit) {
        unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_FORMAT_FAIL;
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d tried to read backing"
                                           " store for group %u but only"
                                           " %u groups in backing store"),
                          unit,
                          group,
                          unitData->wb.groupLimit));
    }
    if (group > unitData->groupLimit) {
        unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_RESTORE_FAIL;
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d tried to read backing"
                                           " store for group %u but only"
                                           " supports %u groups"),
                          unit,
                          group,
                          unitData->groupLimit));
    }
    groupData = &(unitData->groupD[group]);
    sal_memset(&groupTemp, 0x00, sizeof(groupTemp));
    switch (unitData->wb.version) {
    case _BCM_DPP_WB_FIELD_VERSION_1_0:
        groupSize = (sizeof(*wbData) +
                     (sizeof(uint32) *
                      (((unitData->wb.bcmFieldActionCount + 31) >> 5) - 1)));
        groupSize = _BCM_DPP_WB_FIELD_ALIGNED_SIZE(groupSize);
        groupData = &(unitData->groupD[group]);
        wbData = (_bcm_dpp_wb_1_0_field_group_t*)&(unitData->wb.buffer[unitData->wb.offsetGroup + (group * groupSize)]);
        if (0 == (wbData->groupFlags & _BCM_DPP_FIELD_GROUP_IN_USE)) {
            /* nothing to do here because this group is not in hardware */
            BCM_EXIT;
        }
#if _BCM_PPD_FIELD_WARM_BOOT_READ_TRACKING
        LOG_DEBUG(BSL_LS_BCM_INIT,
                  (BSL_META_U(unit,
                              "unit %d recovering group %u"
                              " information from warm boot buffer\n"),
                   unit,
                   group));
#endif /* _BCM_PPD_FIELD_WARM_BOOT_READ_TRACKING */
        SOC_PPD_FP_DATABASE_INFO_clear(&dbInfo);
        ppdr = soc_ppd_fp_database_get(unitData->unitHandle,
                                       wbData->hwHandle,
                                       &dbInfo);
        result = handle_sand_result(ppdr);
        if (BCM_E_NONE != result) {
            unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_RESTORE_FAIL;
            BCMDNX_ERR_EXIT_MSG(result,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d unable to get PPD"
                                               " information for group %u"
                                               " (%08X): %d (%s)\n"),
                              unit,
                              group,
                              wbData->hwHandle,
                              result,
                              _SHR_ERRMSG(result)));
        }
        for (stage = 0; stage < unitData->devInfo->stages; stage++) {
            if (unitData->devInfo->stage[stage].hwStageId ==
                wbData->grpStage) {
                /* found this group's stage */
                break;
            }
        } /* for (all supported stages on the unit) */
        if (stage >= unitData->devInfo->stages) {
            unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_RESTORE_FAIL;
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d group %d claims"
                                               " unsupported hardware"
                                               " stage %d"),
                              unit,
                              group,
                              wbData->grpStage));
        }
        /* populate basic infromation for the group */
        groupTemp.groupFlags = wbData->groupFlags;
        groupTemp.stage = stage;
        groupTemp.grpMode = wbData->grpMode;
        groupTemp.hwHandle = wbData->hwHandle;
        groupTemp.predefKey = wbData->predefKey;
        switch (groupTemp.grpMode) {
        case bcmFieldGroupModeDirectExtraction:
            groupTemp.entryHead = unitData->entryDeLimit;
            groupTemp.entryTail = unitData->entryDeLimit;
            break;
        /* every other mode assumes TCAM */
        default:
            if (stage == _BCM_DPP_FIELD_STAGE_INDEX_ING_EXTERNAL) {
                groupTemp.entryHead = unitData->entryExtTcLimit;
                groupTemp.entryTail = unitData->entryExtTcLimit;
            }
            else {
                groupTemp.entryHead = unitData->entryTcLimit;
                groupTemp.entryTail = unitData->entryTcLimit;
            }
        }
        groupTemp.priority = dbInfo.strength;
        /* cascading will be computed once all groups are loaded */
        groupTemp.cascadePair = unitData->groupLimit;
        /* mark this hardware group ID as in use */
        temp = groupTemp.hwHandle;
        result = shr_mres_alloc(unitData->hwResIds,
                                unitData->stageD[stage].groupRes,
                                SHR_RES_ALLOC_WITH_ID,
                                1,
                                &(temp));
        BCMDNX_IF_ERR_EXIT_MSG(result,
                            (_BSL_BCM_MSG_NO_UNIT("unable to mark unit %d stage"
                                              " %u group %u hardware ID %d"
                                              " used in resource %d: %d (%s)"),
                             unit,
                             stage,
                             group,
                             groupTemp.hwHandle,
                             unitData->stageD[stage].groupRes,
                             result,
                             _SHR_ERRMSG(result)));
        /* populate original BCM action set */
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_wb_bmp_get(unitData,
                                                          &(wbData->oaset[0]),
                                                          unitData->wb.bcmFieldActionCount,
                                                          &(groupTemp.oaset.w[0]),
                                                          bcmFieldActionCount,
                                                          "actions in original action set",
                                                          group,
                                                          "group"));
        /* populate group types bitmap */
        for (index = 0; index < unitData->devInfo->types; index++) {
            if ((unitData->stageD[groupTemp.stage].ltOffset[index] <
                 unitData->devInfo->mappings) &&
                (unitData->devInfo->stMapInfo[unitData->stageD[groupTemp.stage].ltOffset[index]].stmFlags ==
                 (unitData->devInfo->stMapInfo[unitData->stageD[groupTemp.stage].ltOffset[index]].stmFlags &
                  groupTemp.groupFlags)))  {
                /* this type is valid here, and this group should include it */
                groupTemp.groupTypes |= (1 << index);
            }
        } /* for (index = 0; index < unitData->devInfo->types; index++) */
        /* populate preselector set */
        BCM_FIELD_PRESEL_INIT(groupTemp.preselSet);
        for (index = 0; index < unitData->wb.preselLimit; index++) {
#ifdef BCM_ARAD_SUPPORT
            if (SOC_IS_ARAD(unit)) {
                if (SHR_BITGET(dbInfo.supported_pfgs_arad, index)) {
                    BCM_FIELD_PRESEL_ADD(groupTemp.preselSet, index);
                }
            } else
#endif /* def BCM_ARAD_SUPPORT */
#ifdef BCM_PETRAB_SUPPORT
            if (SOC_IS_PETRAB(unit)) {
                if (dbInfo.supported_pfgs & (1 << index)) {
                    BCM_FIELD_PRESEL_ADD(groupTemp.preselSet, index);
                }
            } else
#endif /* def BCM_PETRAB_SUPPORT */
            {
                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                                 (_BSL_BCM_MSG_NO_UNIT("unit %d unable"
                                                   " to retrieve"
                                                   " preselectors"
                                                   " from hardware"),
                                  unit));
            }
            /*
             *  Preselector references were loaded from backing store, so
             *  we do not try to manipulate that here.
             */
        } /* for (index = 0; index < unitData->wb.preselLimit; index++) */
        sal_memcpy(&(groupTemp.preselHw),
                   &(groupTemp.preselSet),
                   sizeof(groupTemp.preselHw));
        /* populate PPD layer qualifier set */
        for (index = 0; index < SOC_PPD_FP_NOF_QUALS_PER_DB_MAX; index++) {
            if (dbInfo.qual_types[index] <= SOC_PPD_NOF_FP_QUAL_TYPES) {
                if (_BCM_DPP_FIELD_PPD_QUAL_VALID(dbInfo.qual_types[index])) {
                    SHR_BITSET(groupTemp.pqset, dbInfo.qual_types[index]);
                } /* if (this qualifier is not ..._INVALID) */
            } else { /* if (this qualifier is a valid type) */
                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                                 (_BSL_BCM_MSG_NO_UNIT("unit %d group %u claims"
                                                   " unsupported PPD qualifier"
                                                   " %d"),
                                  unit,
                                  group,
                                  dbInfo.qual_types[index]));
            } /* if (this qualifier is a valid type) */
        } /* for (all possible PPD layer qualifier slots) */
        /* populate PPD layer action set */
        for (index = 0; index < SOC_PPD_FP_NOF_ACTIONS_PER_DB_MAX; index++) {
            if (dbInfo.action_types[index] <= SOC_PPD_NOF_FP_ACTION_TYPES) {
                if (_BCM_DPP_FIELD_PPD_ACTION_VALID(dbInfo.action_types[index])) {
                    SHR_BITSET(groupTemp.paset, dbInfo.action_types[index]);
                } /* if (this action is not ..._INVALID) */
            } else { /* if (this action is a valid type) */
                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                                 (_BSL_BCM_MSG_NO_UNIT("unit %d group %u claims"
                                                   " unsupported PPD action"
                                                   " %d"),
                                  unit,
                                  group,
                                  dbInfo.action_types[index]));
            } /* if (this action is a valid type) */
        } /* for (all possible PPD layer action slots) */
        /* populate BCM layer qualifier set */
        result = _bcm_dpp_field_group_qset_recover(unitData,
                                                   groupTemp.stage,
                                                   groupTemp.groupTypes,
                                                   &(groupTemp.pqset),
                                                   &(groupTemp.qset));
        BCMDNX_IF_ERR_EXIT_MSG(result,
                            (_BSL_BCM_MSG_NO_UNIT("unit %d group %u failed to"
                                              " reconstruct qset: %d (%s)"),
                             unit,
                             group,
                             result,
                             _SHR_ERRMSG(result)));
        /* populate BCM layer action set */
        result = _bcm_dpp_field_group_aset_recover(unitData,
                                                   groupTemp.stage,
                                                   &(groupTemp.paset),
                                                   &(groupTemp.aset));
        BCMDNX_IF_ERR_EXIT_MSG(result,
                            (_BSL_BCM_MSG_NO_UNIT("unit %d group %u failed to"
                                              " reconstruct aset: %d (%s)"),
                             unit,
                             group,
                             result,
                             _SHR_ERRMSG(result)));
#if defined(INCLUDE_KBP) && !defined(BCM_88030)
        if (stage == _BCM_DPP_FIELD_STAGE_INDEX_ING_EXTERNAL) {
            /* Configure ACL databases to KBP SW state */
            result = sw_state_access[unit].dpp.soc.arad.tm.kbp_info.db_info.get(unitData->unit,
                                           (ARAD_KBP_ACL_TABLE_ID_OFFSET + groupTemp.hwHandle),
                                           &db_handles_info);
            BCMDNX_IF_ERR_EXIT(result);
            key_size_in_bytes = SOC_SAND_DIV_ROUND_UP(db_handles_info.table_width, SOC_SAND_NOF_BITS_IN_CHAR);
            result = arad_pmf_prog_select_pmf_pgm_borders_get(
                      unitData->unit,
                      SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP,
                      FALSE, 
                      &pmf_pgm_ndx_min,
                      &pmf_pgm_ndx_max
                    );
            BCM_SAND_IF_ERR_EXIT(result);
            result = sw_state_access[unit].dpp.soc.arad.tm.pmf.db_info.progs.get(
                      unitData->unit,
                      SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP,
                      groupTemp.hwHandle,
                      0,
                      &pmf_pgm_bmp_used
                    );
            BCMDNX_IF_ERR_EXIT(result);
            result = arad_kbp_add_acl(
                    unitData->unit,
                    (ARAD_KBP_ACL_TABLE_ID_OFFSET + groupTemp.hwHandle), /* table_id */
                    (dbInfo.action_types[0] - SOC_PPC_FP_ACTION_TYPE_FLP_ACTION_0), /* search_id */
                    pmf_pgm_bmp_used,
                    pmf_pgm_ndx_min,
                    pmf_pgm_ndx_max,
                    key_size_in_bytes,
                    ARAD_PP_FP_KEY_ALLOC_CE_FLP_NO_SPLIT,
                    dbInfo.min_priority,
                    &pmf_pgm_bmp_new_sw,
                    &success
                  );
            BCM_SAND_IF_ERR_EXIT(result);
        }
#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030) */
        break;
    default:
        unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_VERSION_FAIL;
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d unexpected warm boot"
                                           " version %04X\n"),
                          unit,
                          unitData->wb.version));
    } /* switch (unitData->wb_version) */
    /* everything looks good; commit group */
    sal_memcpy(groupData, &groupTemp, sizeof(*groupData));
    /* search for where the group should be linked in the group list */
    for (current = unitData->stageD[stage].groupHead;
         (current < unitData->groupLimit) &&
         (unitData->groupD[current].priority < groupData->priority);
         current = unitData->groupD[current].groupNext) {
        /* traverse group list for stage until proper prio */
    }
    if (current < unitData->groupLimit) {
        /* still on the list; insert before current */
        groupData->groupNext = current;
        if (current == unitData->stageD[stage].groupHead) {
            groupData->groupPrev = unitData->groupLimit;
            unitData->stageD[stage].groupHead = group;
        } else {
            groupData->groupPrev = unitData->groupD[current].groupPrev;
            unitData->groupD[groupData->groupPrev].groupNext = group;
        }
        unitData->groupD[current].groupPrev = group;
    } else {
        /* fell off the list; add to tail */
        unitData->groupD[unitData->stageD[stage].groupTail].groupNext = group;
        groupData->groupPrev = unitData->stageD[stage].groupTail;
        groupData->groupNext = unitData->groupLimit;
        unitData->stageD[stage].groupTail = group;
        if (unitData->stageD[stage].groupHead >=
            unitData->groupLimit) {
            unitData->stageD[stage].groupHead = group;
        }
    }
    /* account for this group */
    unitData->stageD[unitData->stageD[groupData->stage].groupSh1].groupCount++;
    unitData->groupCount++;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *    _bcm_dpp_field_group_all_wb_load
 *  Purpose
 *    Load the group information from the backing store buffer and hardware
 *  Arguments
 *    IN unitData = pointer to the unit information
 *  Results
 *    bcm_error_t (cast as int)
 *      BCM_E_NONE if successful
 *      BCM_E_* otherwise as appropriate
 *  Notes
 */
STATIC int
_bcm_dpp_field_group_all_wb_load(_bcm_dpp_field_info_t *unitData)
{
    _bcm_dpp_field_group_t *groupData;
    _bcm_dpp_field_grp_idx_t group;
    _bcm_dpp_field_grp_idx_t freeTail = unitData->groupLimit;
    unsigned int index;
    int result;
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;

    if (0 == unitData->wb.offsetGroup) {
        unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_FORMAT_FAIL;
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d does not appear to support"
                                           " group data in backing store"
                                           " version %04X\n"),
                          unit,
                          unitData->wb.version));
    }
#if _BCM_PPD_FIELD_WARM_BOOT_READ_TRACKING
    LOG_DEBUG(BSL_LS_BCM_INIT,
              (BSL_META_U(unit,
                          "unit %d recovering all group"
                          " information from warm boot buffer\n"),
               unit));
#endif /* _BCM_PPD_FIELD_WARM_BOOT_READ_TRACKING */
    /* clear group state */
    sal_memset(&(unitData->groupD[0]),
               0x00,
               sizeof(unitData->groupD[0]) * unitData->groupLimit);
    unitData->groupFree = unitData->groupLimit;
    unitData->groupCount = 0;
    unitData->groupCascaded = 0;
    /* traverse the groups, getting them back from hardware + backing store */
    for (group = 0; group < unitData->wb.groupLimit; group++) {
        result = _bcm_dpp_field_group_wb_load(unitData, group);
        if (BCM_E_NONE != result) {
            unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_RESTORE_FAIL;
            BCMDNX_ERR_EXIT_MSG(result,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d unable to recover group"
                                               " %u data from backing store:"
                                               " %d (%s)"),
                              unit,
                              group,
                              result,
                              _SHR_ERRMSG(result)));
        }
    } /* for (group = 0; group < unitData->wb.groupLimit; group++) */
    /* tidy up the unallocated groups */
    for (group = 0; group < unitData->groupLimit; group++) {
        groupData = &(unitData->groupD[group]);
        if (0 == (groupData->groupFlags & _BCM_DPP_FIELD_GROUP_IN_USE)) {
            groupData->groupNext = unitData->groupLimit;
            groupData->groupPrev = freeTail;
            groupData->stage = unitData->devInfo->stages;
            if (freeTail < unitData->groupLimit) {
                unitData->groupD[freeTail].groupNext = group;
            } else { /* if (freeTail < unitData->groupLimit) */
                unitData->groupFree = group;
            } /* if (freeTail < unitData->groupLimit) */
            freeTail = group;
        } /* if (group is not in use) */
    } /* for (all groups this unit) */
    /* need to collect all of the cascading pairs now */
    for (group = 0; group < unitData->groupLimit; group++) {
        groupData = &(unitData->groupD[group]);
        if ((_BCM_DPP_FIELD_GROUP_IN_USE |
             _BCM_DPP_FIELD_GROUP_CASCADE) ==
            ((_BCM_DPP_FIELD_GROUP_IN_USE |
              _BCM_DPP_FIELD_GROUP_CASCADE) &
             groupData->groupFlags)) {
            /* group is in use and uses cascading resources */
            if (groupData->cascadePair < unitData->groupLimit) {
                /* already found this one's pair */
                continue;
            }
            /* account for this group */
            unitData->groupCascaded++;
            /* search for this group's pair */
            for (freeTail = group + 1;
                 freeTail < unitData->groupLimit;
                 freeTail++) {
                if ((_BCM_DPP_FIELD_GROUP_IN_USE |
                     _BCM_DPP_FIELD_GROUP_CASCADE) !=
                    ((_BCM_DPP_FIELD_GROUP_IN_USE |
                      _BCM_DPP_FIELD_GROUP_CASCADE) &
                     groupData->groupFlags)) {
                    /* this group is not in use or does not use cascading */
                    continue;
                }
                if (unitData->groupD[freeTail].cascadePair < unitData->groupLimit) {
                    /* this one's pair has already been found */
                    continue;
                }
                if (unitData->groupD[freeTail].stage != groupData->stage) {
                    /* this group is not in the same stage */
                    continue;
                }
                for (index = 0;
                     index < _SHR_BITDCLSIZE(unitData->preselLimit);
                     index++) {
                    if (unitData->groupD[freeTail].preselSet.w[index] !=
                        groupData->preselSet.w[index]) {
                        break;
                    }
                }
                if (index < _SHR_BITDCLSIZE(unitData->preselLimit)) {
                    /* preselector sets are not equal */
                    continue;
                }
                /* same stage and preselectors, so found the pair */
                groupData->cascadePair = freeTail;
                unitData->groupD[freeTail].cascadePair = group;
                /* account for the pair */
                unitData->groupCascaded++;
                /* done looking for this one's pair */
                break;
            } /* for (all groups after this one) */
            if (freeTail >= unitData->groupLimit) {
                unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_RESTORE_FAIL;
                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                                 (_BSL_BCM_MSG_NO_UNIT("unit %d group %u claims to"
                                                   " use cascade features but"
                                                   " has no apparent pair"),
                                  unit,
                                  group));
            }
        } /* if (this group is in use and uses cascading) */
    } /* for (group = 0; group < unitData->groupLimit; group++) */
exit:
    BCMDNX_FUNC_RETURN;
}

void
_bcm_dpp_field_stage_wb_save(_bcm_dpp_field_info_t *unitData,
                             _bcm_dpp_field_stage_idx_t stage,
                             uint8 *data,
                             unsigned int *size)
{
    unsigned int preselSetSize;
    unsigned int stageSize;
    _bcm_dpp_wb_1_0_field_stage_t *wbData;
    _bcm_dpp_field_stage_t *stageData;
    int result;
    int unit;

    unit = unitData->unit;
    if ((!size) && (0 == unitData->wb.offsetStage)) {
        LOG_ERROR(BSL_LS_BCM_INIT,
                  (BSL_META_U(unit,
                              "unit %d does not appear to support stage"
                              " data in backing store version %04X\n"),
                   unit,
                   unitData->wb.version));
        unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_FORMAT_FAIL;
        return;
    }
    if (stage >= unitData->wb.stageLimit) {
        LOG_ERROR(BSL_LS_BCM_INIT,
                  (BSL_META_U(unit,
                              "unit %d backing store only supports stages"
                              " %d through %d\n"),
                   unit,
                   0,
                   unitData->wb.stageLimit - 1));
        unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_FORMAT_FAIL;
        return;
    }
    switch (unitData->wb.version) {
    case _BCM_DPP_WB_FIELD_VERSION_1_0:
        preselSetSize = ((unitData->wb.preselLimit + 31) >> 5);
        stageSize = (sizeof(*wbData) +
                     (sizeof(uint32) * preselSetSize * 4));
        stageSize = _BCM_DPP_WB_FIELD_ALIGNED_SIZE(stageSize);
        if (size) {
            *size = stageSize;
        } else { /* if (size) */
            stageData = &(unitData->stageD[stage]);
            wbData = sal_alloc(stageSize, "stage backing store temp");
            if (wbData) {
                sal_memset(wbData, 0x00, stageSize);
                wbData->ipfgs[0] = stageData->devInfo->hwStageId;
                _bcm_dpp_field_wb_bmp_put(unitData,
                                          &(stageData->pfgsEther.w[0]),
                                          unitData->preselLimit,
                                          &(wbData->ipfgs[1]),
                                          unitData->wb.preselLimit,
                                          "preselectors",
                                          0,
                                          "implied Ethernet preselectors");
                _bcm_dpp_field_wb_bmp_put(unitData,
                                          &(stageData->pfgsIPv4.w[0]),
                                          unitData->preselLimit,
                                          &(wbData->ipfgs[1 + preselSetSize]),
                                          unitData->wb.preselLimit,
                                          "preselectors",
                                          0,
                                          "implied IPv4 Preselectors");
                _bcm_dpp_field_wb_bmp_put(unitData,
                                          &(stageData->pfgsIPv6.w[0]),
                                          unitData->preselLimit,
                                          &(wbData->ipfgs[1 + (preselSetSize * 2)]),
                                          unitData->wb.preselLimit,
                                          "preselectors",
                                          0,
                                          "implied IPv6 Preselectors");
                _bcm_dpp_field_wb_bmp_put(unitData,
                                          &(stageData->pfgsMPLS.w[0]),
                                          unitData->preselLimit,
                                          &(wbData->ipfgs[1 + (preselSetSize * 3)]),
                                          unitData->wb.preselLimit,
                                          "preselectors",
                                          0,
                                          "implied MPLS Preselectors");
#if _BCM_PPD_FIELD_WARM_BOOT_WRITE_TRACKING
                LOG_DEBUG(BSL_LS_BCM_INIT,
                          (BSL_META_U(unit,
                                      "unit %d backing store write stage %d"
                                      " info at %u for %u bytes\n"),
                           unit,
                           stage,
                           unitData->wb.offsetStage + (stageSize * stage),
                           stageSize));
#endif /* _BCM_PPD_FIELD_WARM_BOOT_WRITE_TRACKING */
                if (data) {
                    sal_memcpy(&(data[unitData->wb.offsetStage + (stageSize * stage)]),
                               wbData,
                               stageSize);
                } else { /* if (data) */
                    sal_memcpy(&(unitData->wb.buffer[unitData->wb.offsetStage + (stageSize * stage)]),
                               wbData,
                               stageSize);
					if (!(SOC_CONTROL(unit)->autosync) && !wb_field_is_scheduled_sync[unit]) {
					        /* mark dirty state. Currently no other processing done           */
					        /* note that there is no in module dirty bit optimization. use global dirty bit */
					        BCM_DPP_WB_DEV_FIELD_DIRTY_BIT_SET(unit);								        
					}
					else{
	                    result = soc_scache_commit_specific_data(unit,
	                                                             unitData->wb.handle,
	                                                             stageSize,
	                                                             (uint8*)wbData,
	                                                             unitData->wb.offsetStage + (stageSize * stage));
	                    if (SOC_E_NONE != result) {
	                        LOG_ERROR(BSL_LS_BCM_INIT,
                                          (BSL_META_U(unit,
                                                      "unit %d unable to update"
                                                      " backing store for stage %d"
                                                      " %d (%s)\n"),
                                           unit,
                                           stage,
                                           result,
                                           _SHR_ERRMSG(result)));
	                        unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_COMMIT_FAIL;
	                    }
	                }
                } /* if (data) */
                sal_free(wbData);
            } /* if (wbData) */
        } /* if (size) */
        break;
    default:
        LOG_ERROR(BSL_LS_BCM_INIT,
                  (BSL_META_U(unit,
                              "unit %d unexpected warm boot version %04X\n"),
                   unit,
                   unitData->wb.version));
        unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_VERSION_FAIL;
        if (size) {
            *size = 0;
        }
    } /* switch (unitData->wb_version) */
}

/*
 *  Function
 *    _bcm_dpp_field_stage_resource_purge
 *  Purpose
 *    Purge resources per stage
 *  Arguments
 *    IN unitData = pointer to the unit information
 *    IN resourceId = resource ID to purge
 *    IN stage = resource ID's stage
 *    IN name = resource ID's name
 *  Results
 *    bcm_error_t (cast as int)
 *      BCM_E_NONE if successful
 *      BCM_E_* otherwise as appropriate
 *  Notes
 */
STATIC int
_bcm_dpp_field_stage_resource_purge(_bcm_dpp_field_info_t *unitData,
                                    int resourceId,
                                    _bcm_dpp_field_stage_idx_t stage,
                                    const char *name)
{
    shr_res_pool_info_t poolInfo;
    int poolId;
    int first;
    int count;
    int result;
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;

    if (0 <= resourceId) {
        result = shr_mres_type_get(unitData->hwResIds,
                                   resourceId,
                                   &poolId,
                                   NULL,
                                   NULL);
        if (BCM_E_CONFIG == result) {
            /* resource is not configured; nothing to do */
            BCM_EXIT;
        }
        BCMDNX_IF_ERR_EXIT_MSG(result,
                            (_BSL_BCM_MSG_NO_UNIT("unable to fetch unit %d"
                                              " stage %s (%u) %s resource"
                                              " type %d information: %d (%s)"),
                             unit,
                             unitData->devInfo->stage[stage].stageName,
                             stage,
                             name,
                             resourceId,
                             result,
                             _SHR_ERRMSG(result)));
        BCMDNX_IF_ERR_EXIT_MSG(shr_mres_pool_info_get(unitData->hwResIds,
                                                   poolId,
                                                   &poolInfo),
                            (_BSL_BCM_MSG_NO_UNIT("unable to fetch unit %d"
                                              " stage %s (%u) %s resource"
                                              " pool %d state"),
                             unit,
                             unitData->devInfo->stage[stage].stageName,
                             stage,
                             name,
                             poolId));
        if (poolInfo.used) {
            BCMDNX_IF_ERR_EXIT_MSG(shr_mres_pool_get(unitData->hwResIds,
                                                  poolId,
                                                  NULL,
                                                  &first,
                                                  &count,
                                                  NULL,
                                                  NULL),
                                (_BSL_BCM_MSG_NO_UNIT("unable to fetch unit %d"
                                                  " stage %s (%u) %s resource"
                                                  " pool %d information"),
                                 unit,
                                 unitData->devInfo->stage[stage].stageName,
                                 stage,
                                 name,
                                 poolId));
            /*
             *  This tries to make sure there are no used elements in the
             *  appropriate resource pool by explicitly freing any that are
             *  in use.  It only occurs if there are some in use, but this
             *  should be called only during init, thence this is hopefully
             *  mere paranoia.
             */
            while (count > 0) {
                result = shr_mres_free(unitData->hwResIds,
                                       unitData->stageD[stage].groupRes,
                                       1,
                                       first);
                /* but we don't care about the result here */
                count--;
                first++;
            } /* while (count > 0) */
        } /* if (poolInfo.used) */
    } /* if (0 <= resourceId) */
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *    _bcm_dpp_field_stage_all_wb_load
 *  Purpose
 *    Load the stage information from the backing store buffer and hardware
 *  Arguments
 *    IN unitData = pointer to the unit information
 *  Results
 *    bcm_error_t (cast as int)
 *      BCM_E_NONE if successful
 *      BCM_E_* otherwise as appropriate
 *  Notes
 *    Nothing to restore from backing store, so just init state for stages.
 */
STATIC int
_bcm_dpp_field_stage_all_wb_load(_bcm_dpp_field_info_t *unitData)
{
    _bcm_dpp_field_stage_idx_t stage;
    _bcm_dpp_wb_1_0_field_stage_t *wbData;
    _bcm_dpp_field_stage_t *stageData;
    unsigned int preselSetSize;
    unsigned int stageSize;
    int qual_id, offset;
    bcm_field_data_qualifier_t qual;
#ifdef BCM_PETRAB_SUPPORT
    int found;
#endif /* def BCM_PETRAB_SUPPORT */
    int result = BCM_E_NONE;
    int adjust;
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;
#if _BCM_PPD_FIELD_WARM_BOOT_READ_TRACKING
    LOG_DEBUG(BSL_LS_BCM_INIT,
              (BSL_META_U(unit,
                          "unit %d recovering all stage"
                          " information from warm boot buffer\n"),
               unit));
#endif /* _BCM_PPD_FIELD_WARM_BOOT_READ_TRACKING */
    for (stage = 0; stage < unitData->devInfo->stages; stage++) {
        unitData->stageD[stage].entryCount = 0;
        unitData->stageD[stage].entryElems = 0;
        unitData->stageD[stage].entryDeCount = 0;
        unitData->stageD[stage].entryDeElems = 0;
        unitData->stageD[stage].groupCount = 0;
        unitData->stageD[stage].groupHead = unitData->groupLimit;
        unitData->stageD[stage].groupTail = unitData->groupLimit;
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_stage_resource_purge(unitData,
                                                                    unitData->stageD[stage].groupRes,
                                                                    stage,
                                                                    "group"));
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_stage_resource_purge(unitData,
                                                                    unitData->stageD[stage].entryRes,
                                                                    stage,
                                                                    "TCAM entry"));
        if (unitData->stageD[stage].hwEntryDeLimit > 0) { 
            BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_stage_resource_purge(unitData,
                                                                        unitData->stageD[stage].entryDeRes,
                                                                        stage,
                                                                        "DirExt entry"));
        }
    }
    switch (unitData->wb.version) {
    case _BCM_DPP_WB_FIELD_VERSION_1_0:
        preselSetSize = ((unitData->wb.preselLimit + 31) >> 5);
        stageSize = (sizeof(*wbData) +
                     (sizeof(uint32) * preselSetSize * 4));
        stageSize = _BCM_DPP_WB_FIELD_ALIGNED_SIZE(stageSize);
        for (stage = 0; stage < unitData->wb.stageLimit; stage++) {
            wbData = (_bcm_dpp_wb_1_0_field_stage_t*)(&(unitData->wb.buffer[unitData->wb.offsetStage + (stageSize * stage)]));
            for (adjust = 0; adjust < unitData->devInfo->stages; adjust++) {
                if (wbData->ipfgs[0] == unitData->stageD[adjust].devInfo->hwStageId) {
                    break;
                }
            }
            if (adjust < unitData->devInfo->stages) {
                stageData = &(unitData->stageD[adjust]);
                BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_wb_bmp_get(unitData,
                                                                  &(wbData->ipfgs[0]),
                                                                  unitData->wb.preselLimit,
                                                                  &(stageData->pfgsEther.w[0]),
                                                                  unitData->preselLimit,
                                                                  "preselectors",
                                                                  0,
                                                                  "implied Ethernet preselectors"));
                BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_wb_bmp_get(unitData,
                                                                  &(wbData->ipfgs[preselSetSize]),
                                                                  unitData->wb.preselLimit,
                                                                  &(stageData->pfgsIPv4.w[0]),
                                                                  unitData->preselLimit,
                                                                  "preselectors",
                                                                  0,
                                                                  "implied IPv4 Preselectors"));
                BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_wb_bmp_get(unitData,
                                                                  &(wbData->ipfgs[preselSetSize * 2]),
                                                                  unitData->wb.preselLimit,
                                                                  &(stageData->pfgsIPv6.w[0]),
                                                                  unitData->preselLimit,
                                                                  "preselectors",
                                                                  0,
                                                                  "implied IPv6 Preselectors"));
                BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_wb_bmp_get(unitData,
                                                                  &(wbData->ipfgs[preselSetSize * 3]),
                                                                  unitData->wb.preselLimit,
                                                                  &(stageData->pfgsMPLS.w[0]),
                                                                  unitData->preselLimit,
                                                                  "preselectors",
                                                                  0,
                                                                  "implied MPLS Preselectors"));
                /* restore data qualifiers bitmap for stage */
                for (qual_id = 0;
                     (qual_id < unitData->dqLimit) &&
                     (SHR_BITGET(unitData->dataFieldInUse, qual_id));
                     qual_id++) {
                        result = _bcm_petra_field_data_qualifier_get(unitData, qual_id, &qual);
                        if (BCM_E_NONE != result) {
                            /* invalid or unknown qualifier id */
                            unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_RESTORE_FAIL;
                            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                                                (_BSL_BCM_MSG_NO_UNIT("unit %d unable to get"
                                                                      " qualifier data"
                                                                      " qual_id = %d: %d (%s)"),
                                                 unit,
                                                 qual_id,
                                                 result,
                                                 _SHR_ERRMSG(result)));
                        }

                        if (qual.stage == stage) {
                            for (offset = 0; offset < unitData->devInfo->types; offset++) {
                                SHR_BITSET(unitData->stageD[stage].qsetType[offset].udf_map, qual_id);
                            }
                        }
                }
            } else {
                BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                                 (_BSL_BCM_MSG_NO_UNIT("unit %d stage %s (%d) in"
                                                   " backing store does not"
                                                   " exist in working config"),
                                  unit,
                                  SOC_PPC_FP_DATABASE_STAGE_to_string(wbData->ipfgs[0]),
                                  wbData->ipfgs[0]));
            }
        } /* for (stage = 0; stage < unitData->wb.stageLimit; stage++) */
	break;
    default:
        unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_VERSION_FAIL;
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d unexpected warm boot"
                                           " version %04X\n"),
                          unit,
                          unitData->wb.version));
    } /* switch (unitData->wb_version) */
#ifdef BCM_PETRAB_SUPPORT
    /* compensate for group use in IPv4 host extend (PetraB only) */
    if (SOC_IS_PETRAB(unit) &&
        (BCM_E_NONE == result) &&
        soc_pb_pp_sw_db_ipv4_host_extend_enable_get(unitData->unitHandle)) {
        for (stage = 0, found = FALSE;
             stage < unitData->devInfo->stages;
             stage++) {
            if (SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF ==
                unitData->devInfo->stage[stage].hwStageId) {
                found = TRUE;
                /* reserve the highest group ID in matching stages */
                adjust = unitData->stageD[stage].hwGroupLimit - 1;
                result = shr_mres_alloc(unitData->hwResIds,
                                        unitData->stageD[stage].groupRes,
                                        SHR_RES_ALLOC_WITH_ID,
                                        1 /* count */,
                                        &adjust);
                if (BCM_E_EXISTS == result) {
                    /* okay, must have already done this on earlier stage */
                    result = BCM_E_NONE;
                }
                if (BCM_E_NONE == result) {
                    /* reserve the lowest entry ID in matching stages */
                    adjust = 0;
                    result = shr_mres_alloc(unitData->hwResIds,
                                            unitData->stageD[stage].entryRes,
                                            SHR_RES_ALLOC_WITH_ID,
                                            1 /* count */,
                                            &adjust);
                    if (BCM_E_RESOURCE == result) {
                        /* okay, must have already done it on earlier stage */
                        result = BCM_E_NONE;
                    }
                }
                BCMDNX_IF_ERR_EXIT_MSG(result,
                                    (_BSL_BCM_MSG_NO_UNIT("unexpected error setting"
                                                      " resources for unit %d"
                                                      " IPv4 host extension"
                                                      " feature: %d (%s)"),
                                     unit,
                                     result,
                                     _SHR_ERRMSG(result)));
            } /* if (ingress type stage) */
        } /* for (all stages in this device as long as no error) */
        if (found) {
            LOG_WARN(BSL_LS_BCM_INIT,
                     (BSL_META_U(unit,
                                 "unit %d has IPv4 host extend enabled;"
                                 " this feature consumes field resources"
                                 " and is not tracked by the field APIs."
                                 "  It is possible that resources may be"
                                 " exhausted before detected by the field"
                                 " APIs.\n"),
                      unit));
        }
    } /* if (ipv4 host extend is enabled) */
#endif /* def BCM_PETRAB_SUPPORT */
exit:
    BCMDNX_FUNC_RETURN;
}

void
_bcm_dpp_field_presel_all_wb_save(_bcm_dpp_field_info_t *unitData,
                                  uint8 *data,
                                  unsigned int *size)
{
    _bcm_dpp_wb_1_0_field_presel_all_t *wbData;
    unsigned int preselSize;
    unsigned int index;
    _bcm_dpp_field_profile_type_t profile_type;
    int result;
    int unit;

    unit = unitData->unit;

    if ((!size) && (0 == unitData->wb.offsetPresel)) {
        LOG_ERROR(BSL_LS_BCM_INIT,
                  (BSL_META_U(unit,
                              "unit %d does not appear to support preselector"
                              " data in backing store version %04X\n"),
                   unit,
                   unitData->wb.version));
        unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_FORMAT_FAIL;
        return;
    }
    switch (unitData->wb.version) {
    case _BCM_DPP_WB_FIELD_VERSION_1_0:
        preselSize = (sizeof(*wbData) +
                      (sizeof(uint32) *
                       (unitData->wb.preselLimit - 1)));
        preselSize = _BCM_DPP_WB_FIELD_ALIGNED_SIZE(preselSize);
        if (size) {
            *size = preselSize;
        } else { /* if (size) */
            wbData = sal_alloc(preselSize, "preselectors backing store temp");
            if (wbData) {
                sal_memset(wbData, 0x00, preselSize);
                for (index = 0; index < _BCM_DPP_PRESEL_NOF_PORT_PROFILES; index++) {
                    for (profile_type = 0; profile_type < _bcmDppFieldProfileTypeCount; profile_type++) {
                        wbData->preselRefcounts[index + (profile_type * _BCM_DPP_PRESEL_NOF_PORT_PROFILES)] = 
                            unitData->preselProfileRefs[profile_type][index];
                    }
                }
                for (index = 0; index < unitData->preselLimit; index++) {
                    if (index < unitData->wb.preselLimit) {
                        if (BCM_FIELD_PRESEL_TEST(unitData->preselInUse,
                                                  index)) {
                            wbData->preselData[index] = unitData->preselRefs[index] + 1;
                        }
                    } else { /* if (index < unitData->wb_preselLimit) */
                        if (BCM_FIELD_PRESEL_TEST(unitData->preselInUse,
                                                  index)) {
                            LOG_ERROR(BSL_LS_BCM_INIT,
                                      (BSL_META_U(unit,
                                                  "unit %d preselector %u is"
                                                  " in use but not supported"
                                                  " by the backing store"
                                                  " (max %u)\n"),
                                       unit,
                                       index,
                                       unitData->wb.preselLimit));
                            unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_FORMAT_FAIL;
                        } /* if (this preselector is in use) */
                    } /* if (index < unitData->wb_preselLimit) */
                } /* for (all preselectors) */
#if _BCM_PPD_FIELD_WARM_BOOT_WRITE_TRACKING
                LOG_DEBUG(BSL_LS_BCM_INIT,
                          (BSL_META_U(unit,
                                      "unit %d backing store write presel"
                                      " info at %u for %u bytes\n"),
                           unit,
                           unitData->wb.offsetPresel,
                           preselSize));
#endif /* _BCM_PPD_FIELD_WARM_BOOT_WRITE_TRACKING */
                if (data) {
                    sal_memcpy(&(data[unitData->wb.offsetPresel]),
                               wbData,
                               preselSize);
                } else { /* if (data) */
                    sal_memcpy(&(unitData->wb.buffer[unitData->wb.offsetPresel]),
                               wbData,
                               preselSize);
					if (!(SOC_CONTROL(unit)->autosync) && !wb_field_is_scheduled_sync[unit]) {
					        /* mark dirty state. Currently no other processing done           */
					        /* note that there is no in module dirty bit optimization. use global dirty bit */
					        BCM_DPP_WB_DEV_FIELD_DIRTY_BIT_SET(unit);								        
					}
					else{
	                    result = soc_scache_commit_specific_data(unit,
	                                                             unitData->wb.handle,
	                                                             preselSize,
	                                                             (uint8*)wbData,
	                                                             unitData->wb.offsetPresel);
	                    if (SOC_E_NONE != result) {
	                        LOG_ERROR(BSL_LS_BCM_INIT,
                                          (BSL_META_U(unit,
                                                      "unit %d unable to update backing"
                                                      " store for preselectors: %d (%s)\n"),
                                           unit,
                                           result,
                                           _SHR_ERRMSG(result)));
	                        unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_COMMIT_FAIL;
	                    }
	                }
                } /* if (data) */
                sal_free(wbData);
            } else { /* if (wbData) */
                LOG_ERROR(BSL_LS_BCM_INIT,
                          (BSL_META_U(unit,
                                      "unit %d unable to allocate temporary"
                                      " space to update backing store for"
                                      " field preselectors"),
                           unit));
                unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_COMMIT_FAIL;
            } /* if (wbData) */
        } /* if (size) */
        break;
    default:
        LOG_ERROR(BSL_LS_BCM_INIT,
                  (BSL_META_U(unit,
                              "unit %d unexpected warm boot version %04X\n"),
                   unit,
                   unitData->wb.version));
        unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_VERSION_FAIL;
        if (size) {
            *size = 0;
        }
    } /* switch (unitData->wb_version) */
}

/*
 *  Function
 *    _bcm_dpp_field_presel_all_wb_load
 *  Purpose
 *    Load the preselector information from the backing store buffer
 *  Arguments
 *    IN unitData = pointer to the unit information
 *  Results
 *    bcm_error_t (cast as int)
 *      BCM_E_NONE if successful
 *      BCM_E_* otherwise as appropriate
 *  Notes
 */
STATIC int
_bcm_dpp_field_presel_all_wb_load(_bcm_dpp_field_info_t *unitData)
{
    _bcm_dpp_wb_1_0_field_presel_all_t *wbData;
    unsigned int index;
    _bcm_dpp_field_profile_type_t profile_type;
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;
    unit = unitData->unit;

    if (0 == unitData->wb.offsetPresel) {
        unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_FORMAT_FAIL;
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d does not appear to support"
                                           " group data in backing store"
                                           " version %04X\n"),
                          unit,
                          unitData->wb.version));
    }
#if _BCM_PPD_FIELD_WARM_BOOT_READ_TRACKING
    LOG_DEBUG(BSL_LS_BCM_INIT,
              (BSL_META_U(unit,
                          "unit %d recovering all preselector"
                          " information from warm boot buffer\n"),
               unit));
#endif /* _BCM_PPD_FIELD_WARM_BOOT_READ_TRACKING */
    switch (unitData->wb.version) {
    case _BCM_DPP_WB_FIELD_VERSION_1_0:
        wbData = (_bcm_dpp_wb_1_0_field_presel_all_t*)&(unitData->wb.buffer[unitData->wb.offsetPresel]);
        /* get reference counts &c */
        for (index = 0; index < unitData->wb.preselLimit; index++) {
            if (index < unitData->preselLimit) {
                if (wbData->preselData[index]) {
                    BCM_FIELD_PRESEL_ADD(unitData->preselInUse, index);
                    unitData->preselRefs[index] = wbData->preselData[index] - 1;
                } else {
                    BCM_FIELD_PRESEL_REMOVE(unitData->preselInUse, index);
                    unitData->preselRefs[index] = 0;
                }
            } else { /* if (index < unitData->preselLimit) */
                if (wbData->preselData[index]) {
                    unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_RESTORE_FAIL;
                    BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                                     (_BSL_BCM_MSG_NO_UNIT("unit %d backing store"
                                                       " claims preselector %u"
                                                       " is in use, but unit"
                                                       " %d only supports %u"
                                                       " preselectors\n"),
                                      unit,
                                      index,
                                      unit,
                                      unitData->preselLimit));
                }
            } /* if (index < unitData->preselLimit) */
        } /* for (all backing store preselectors) */
        while (index < unitData->preselLimit) {
            BCM_FIELD_PRESEL_REMOVE(unitData->preselInUse, index);
            unitData->preselRefs[index] = 0;
            index++;
        } /* while (index < unitData->preselLimit) */
        /* get implied preselectors for various formats */

        /* Get In/Out-Port reference counts */
        for (index = 0; index < _BCM_DPP_PRESEL_NOF_PORT_PROFILES; index++) {
            for (profile_type = 0; profile_type < _bcmDppFieldProfileTypeCount; profile_type++) {
                unitData->preselProfileRefs[profile_type][index] = 
                    wbData->preselRefcounts[index + (profile_type * _BCM_DPP_PRESEL_NOF_PORT_PROFILES)];
            }
        }
        break;
    default:
        unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_VERSION_FAIL;
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d unexpected warm boot"
                                           " version %04X\n"),
                          unit,
                          unitData->wb.version));
    } /* switch (unitData->wb_version) */
exit:
    BCMDNX_FUNC_RETURN;
}

void
_bcm_dpp_field_datafield_all_wb_save(_bcm_dpp_field_info_t *unitData,
                                     uint8 *data,
                                     unsigned int *size)
{
    _bcm_dpp_wb_1_0_field_datafield_all_t *wbData;
    unsigned int dataFieldSize;
    unsigned int index;
    int result;
    int unit;

    unit = unitData->unit;

    if ((!size) && (0 == unitData->wb.offsetDataField)) {
        LOG_ERROR(BSL_LS_BCM_INIT,
                  (BSL_META_U(unit,
                              "unit %d does not appear to support data field"
                              " data in backing store version %04X\n"),
                   unit,
                   unitData->wb.version));
        unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_FORMAT_FAIL;
        return;
    }
    switch (unitData->wb.version) {
    case _BCM_DPP_WB_FIELD_VERSION_1_0:
        dataFieldSize = (sizeof(*wbData) +
                         (sizeof(uint32) *
                          (unitData->wb.dataFieldLimit - 1)));
        dataFieldSize = _BCM_DPP_WB_FIELD_ALIGNED_SIZE(dataFieldSize);
        if (size) {
            *size = dataFieldSize;
        } else { /* if (size) */
            wbData = sal_alloc(dataFieldSize, "data fields backing store temp");
            if (wbData) {
                sal_memset(wbData, 0x00, dataFieldSize);
                for (index = 0; index < unitData->dqLimit; index++) {
                    if (index < unitData->wb.dataFieldLimit) {
                        if (SHR_BITGET(unitData->dataFieldInUse, index)) {
                            wbData->dataFieldRefs[index] = unitData->dataFieldRefs[index] + 1;
                            if (SHR_BITGET(unitData->dataFieldLenBit, index)) {
                                wbData->dataFieldRefs[index] |= 0x80000000;
                            }
                            if (SHR_BITGET(unitData->dataFieldOfsBit, index)) {
                                wbData->dataFieldRefs[index] |= 0x40000000;
                            }
                        } /* if (this data field is in use) */
                    } else { /* if (index < unitData->wb_dataFieldLimit) */
                        if (SHR_BITGET(unitData->dataFieldInUse, index)) {
                            LOG_ERROR(BSL_LS_BCM_INIT,
                                      (BSL_META_U(unit,
                                                  "unit %d data field %u is"
                                                  " in use but not supported"
                                                  " by the backing store"
                                                  " (max %u)\n"),
                                       unit,
                                       index,
                                       unitData->wb.dataFieldLimit));
                            unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_FORMAT_FAIL;
                        } /* if (this data field is in use) */
                    } /* if (index < unitData->wb_dataFieldLimit) */
                } /* for (all data fields) */
#if _BCM_PPD_FIELD_WARM_BOOT_WRITE_TRACKING
                LOG_DEBUG(BSL_LS_BCM_INIT,
                          (BSL_META_U(unit,
                                      "unit %d backing store write DataField"
                                      " info at %u for %u bytes\n"),
                           unit,
                           unitData->wb.offsetDataField,
                           dataFieldSize));
#endif /* _BCM_PPD_FIELD_WARM_BOOT_WRITE_TRACKING */
                if (data) {
                    sal_memcpy(&(data[unitData->wb.offsetDataField]),
                               wbData,
                               dataFieldSize);
                } else { /* if (data) */
                    sal_memcpy(&(unitData->wb.buffer[unitData->wb.offsetDataField]),
                               wbData,
                               dataFieldSize);
					if (!(SOC_CONTROL(unit)->autosync) && !wb_field_is_scheduled_sync[unit]) {
					        /* mark dirty state. Currently no other processing done           */
					        /* note that there is no in module dirty bit optimization. use global dirty bit */
					        BCM_DPP_WB_DEV_FIELD_DIRTY_BIT_SET(unit);								        
					}
					else{
	                    result = soc_scache_commit_specific_data(unit,
	                                                             unitData->wb.handle,
	                                                             dataFieldSize,
	                                                             (uint8*)wbData,
	                                                             unitData->wb.offsetDataField);
	                    if (SOC_E_NONE != result) {
	                        LOG_ERROR(BSL_LS_BCM_INIT,
                                          (BSL_META_U(unit,
                                                      "unit %d unable to update"
                                                      " backing store for data fields:"
                                                      " %d (%s)\n"),
                                           unit,
                                           result,
                                           _SHR_ERRMSG(result)));
	                        unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_COMMIT_FAIL;
	                    }
	                }
                } /* if (data) */
                sal_free(wbData);
            } else { /* if (wbData) */
                LOG_ERROR(BSL_LS_BCM_INIT,
                          (BSL_META_U(unit,
                                      "unit %d unable to allocate temporary"
                                      " space to update backing store for"
                                      " field data fields"),
                           unit));
                unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_COMMIT_FAIL;
            } /* if (wbData) */
        } /* if (size) */
        break;
    default:
        LOG_ERROR(BSL_LS_BCM_INIT,
                  (BSL_META_U(unit,
                              "unit %d unexpected warm boot version %04X\n"),
                   unit,
                   unitData->wb.version));
        unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_VERSION_FAIL;
        if (size) {
            *size = 0;
        }
    } /* switch (unitData->wb_version) */
}

/*
 *  Function
 *    _bcm_dpp_field_datafield_all_wb_load
 *  Purpose
 *    Load the data field information from the backing store buffer
 *  Arguments
 *    IN unitData = pointer to the unit information
 *  Results
 *    bcm_error_t (cast as int)
 *      BCM_E_NONE if successful
 *      BCM_E_* otherwise as appropriate
 *  Notes
 */
STATIC int
_bcm_dpp_field_datafield_all_wb_load(_bcm_dpp_field_info_t *unitData)
{
    _bcm_dpp_wb_1_0_field_datafield_all_t *wbData;
    unsigned int index;
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;
    if (0 == unitData->wb.offsetDataField) {
        unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_FORMAT_FAIL;
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d does not appear to support"
                                           " data field data in backing store"
                                           " version %04X\n"),
                          unit,
                          unitData->wb.version));
    }
#if _BCM_PPD_FIELD_WARM_BOOT_READ_TRACKING
    LOG_DEBUG(BSL_LS_BCM_INIT,
              (BSL_META_U(unit,
                          "unit %d recovering all data field"
                          " information from warm boot buffer\n"),
               unit));
#endif /* _BCM_PPD_FIELD_WARM_BOOT_READ_TRACKING */
    switch (unitData->wb.version) {
    case _BCM_DPP_WB_FIELD_VERSION_1_0:
        wbData = (_bcm_dpp_wb_1_0_field_datafield_all_t*)&(unitData->wb.buffer[unitData->wb.offsetDataField]);
        for (index = 0; index < unitData->wb.dataFieldLimit; index++) {
            if (index < unitData->dqLimit) {
                if (wbData->dataFieldRefs[index] & 0x3FFFFFFF) {
                    if (wbData->dataFieldRefs[index] & 0x80000000) {
                        SHR_BITSET(unitData->dataFieldLenBit, index);
                    }
                    if (wbData->dataFieldRefs[index] & 0x40000000) {
                        SHR_BITSET(unitData->dataFieldOfsBit, index);
                    }
                    unitData->dataFieldRefs[index] = (wbData->dataFieldRefs[index] & 0x3FFFFFFF) - 1;
                    SHR_BITSET(unitData->dataFieldInUse, index);
                } else {
                    SHR_BITCLR(unitData->dataFieldInUse, index);
                    SHR_BITCLR(unitData->dataFieldLenBit, index);
                    SHR_BITCLR(unitData->dataFieldOfsBit, index);
                    unitData->dataFieldRefs[index] = 0;
                }
            } else { /* if (index < unitData->dqLimit) */
                if (wbData->dataFieldRefs[index] & 0x3FFFFFFF) {
                    unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_RESTORE_FAIL;
                    BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                                     (_BSL_BCM_MSG_NO_UNIT("unit %d backing store"
                                                       " has data field %u in"
                                                       " use but unit %d only"
                                                       " supports data fields"
                                                       " 0..%u\n"),
                                      unit,
                                      index,
                                      unit,
                                      unitData->dqLimit));
                }
            } /* if (index < unitData->dqLimit) */
        } /* for (index = 0; index < unitData->wb.dataFieldLimit; index++) */
        while (index < unitData->dqLimit) {
            SHR_BITCLR(unitData->dataFieldInUse, index);
            SHR_BITCLR(unitData->dataFieldLenBit, index);
            SHR_BITCLR(unitData->dataFieldOfsBit, index);
            unitData->dataFieldRefs[index] = 0;
        }
        break;
    default:
        unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_VERSION_FAIL;
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d unexpected warm boot"
                                           " version %04X\n"),
                          unit,
                          unitData->wb.version));
    } /* switch (unitData->wb_version) */
exit:
    BCMDNX_FUNC_RETURN;
}

void
_bcm_dpp_field_range_all_wb_save(_bcm_dpp_field_info_t *unitData,
                                 uint8 *data,
                                 unsigned int *size)
{
    _bcm_dpp_wb_1_0_field_range_all_t **wbData=NULL;
    unsigned int rangeSize1;
    unsigned int rangeSize;
    unsigned int index;
    unsigned int offset;
    int result;
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;

    if ((!size) && (0 == unitData->wb.offsetRange)) {
        LOG_ERROR(BSL_LS_BCM_INIT,
                  (BSL_META_U(unit,
                              "unit %d does not appear to support range"
                              " data in backing store version %04X\n"),
                   unit,
                   unitData->wb.version));
        unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_FORMAT_FAIL;
        return;
    }
    BCMDNX_ALLOC(wbData, unitData->wb.rangeTypes*sizeof(_bcm_dpp_wb_1_0_field_range_all_t *), "_bcm_dpp_field_range_all_wb_save:wbData ");
    if (wbData == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG_NO_UNIT("_bcm_dpp_field_range_all_wb_save  wbData allocation failed")));
    }

    switch (unitData->wb.version) {
    case _BCM_DPP_WB_FIELD_VERSION_1_0:
        rangeSize1 = (sizeof(*(wbData[0])) +
                      (sizeof(uint32) *
                       (((unitData->wb.rangeLimit + 31) >> 5) - 1)));
        rangeSize1 = _BCM_DPP_WB_FIELD_ALIGNED_SIZE(rangeSize1);
        rangeSize = rangeSize1 * unitData->wb.rangeTypes;
        rangeSize = _BCM_DPP_WB_FIELD_ALIGNED_SIZE(rangeSize);
        if (size) {
            *size = rangeSize;
        } else { /* if (size) */
            wbData[0] = sal_alloc(rangeSize, "range backing store temp");
            if (wbData[0]) {
                sal_memset(wbData[0], 0x00, rangeSize);
                for (index = 1; index < unitData->wb.rangeTypes; index++) {
                    wbData[index] = (_bcm_dpp_wb_1_0_field_range_all_t*)(((uint8*)(wbData[index - 1])) + rangeSize1);
                }
                for (offset = 0;
                     bcmFieldQualifyCount > unitData->devInfo->ranges[offset].qualifier;
                     offset++) {
                    /* just seek to end of list */
                }
                for (index = 0; index < offset; index++) {
                    if (index < unitData->wb.rangeTypes) {
                        wbData[index]->qualifyType = unitData->devInfo->ranges[index].qualifier;
                        wbData[index]->wb_rangeLimit = unitData->devInfo->ranges[index].count;
                        _bcm_dpp_field_wb_bmp_put(unitData,
                                                  &(unitData->rangeInUse[index][0]),
                                                  wbData[index]->wb_rangeLimit,
                                                  &(wbData[index]->rangeData[0]),
                                                  unitData->wb.rangeLimit,
                                                  "ranges",
                                                  wbData[index]->qualifyType,
#if defined(BROADCOM_DEBUG)
                                                  _bcm_dpp_field_qual_name[wbData[index]->qualifyType]
#else /* defined(BROADCOM_DEBUG) */
                                                  "-"
#endif /* defined(BROADCOM_DEBUG) */
                                                  );
                    } /* if (index < unitData->wb_rangeTypes) */
                } /* for (index = 0; index < offset; index++) */
                if (offset > unitData->wb.rangeTypes) {
                    LOG_ERROR(BSL_LS_BCM_INIT,
                              (BSL_META_U(unit,
                                          "unit %d supports %u range types"
                                          " but backing store supports only"
                                          " %u types\n"),
                               unit,
                               offset,
                               unitData->wb.rangeTypes));
                    unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_FORMAT_FAIL;
                }
#if _BCM_PPD_FIELD_WARM_BOOT_WRITE_TRACKING
                LOG_DEBUG(BSL_LS_BCM_INIT,
                          (BSL_META_U(unit,
                                      "unit %d backing store write range"
                                      " info at %u for %u bytes\n"),
                           unit,
                           unitData->wb.offsetRange,
                           rangeSize));
#endif /* _BCM_PPD_FIELD_WARM_BOOT_WRITE_TRACKING */
                if (data) {
                    sal_memcpy(&(data[unitData->wb.offsetRange]),
                               wbData[0],
                               rangeSize);
                } else { /* if (data) */
                    sal_memcpy(&(unitData->wb.buffer[unitData->wb.offsetRange]),
                               wbData[0],
                               rangeSize);
					if (!(SOC_CONTROL(unit)->autosync) && !wb_field_is_scheduled_sync[unit]) {
					        /* mark dirty state. Currently no other processing done           */
					        /* note that there is no in module dirty bit optimization. use global dirty bit */
					        BCM_DPP_WB_DEV_FIELD_DIRTY_BIT_SET(unit);								        
					}
					else{
	                    result = soc_scache_commit_specific_data(unit,
	                                                             unitData->wb.handle,
	                                                             rangeSize,
	                                                             (uint8*)wbData[0],
	                                                             unitData->wb.offsetRange);
	                    if (SOC_E_NONE != result) {
	                        LOG_ERROR(BSL_LS_BCM_INIT,
                                          (BSL_META_U(unit,
                                                      "unit %d unable to update backing"
                                                      " store for ranges: %d (%s)\n"),
                                           unit,
                                           result,
                                           _SHR_ERRMSG(result)));
	                        unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_COMMIT_FAIL;
	                    }
	                }
                } /* if (data) */
                sal_free(wbData[0]);
            } else { /* if (wbData) */
                LOG_ERROR(BSL_LS_BCM_INIT,
                          (BSL_META_U(unit,
                                      "unit %d unable to allocate temporary"
                                      " space to update backing store for"
                                      " field ranges"),
                           unit));
                unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_COMMIT_FAIL;
            } /* if (wbData) */
        } /* if (size) */
        break;
    default:
        LOG_ERROR(BSL_LS_BCM_INIT,
                  (BSL_META_U(unit,
                              "unit %d unexpected warm boot version %04X\n"),
                   unit,
                   unitData->wb.version));
        unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_VERSION_FAIL;
        if (size) {
            *size = 0;
        }
    } /* switch (unitData->wb_version) */
exit:
    BCM_FREE(wbData);
    BCMDNX_FUNC_RETURN_VOID_NO_UNIT;

}

/*
 *  Function
 *    _bcm_dpp_field_range_all_wb_load
 *  Purpose
 *    Load the range information from the backing store buffer
 *  Arguments
 *    IN unitData = pointer to the unit information
 *  Results
 *    bcm_error_t (cast as int)
 *      BCM_E_NONE if successful
 *      BCM_E_* otherwise as appropriate
 *  Notes
 */
STATIC int
_bcm_dpp_field_range_all_wb_load(_bcm_dpp_field_info_t *unitData)
{
    _bcm_dpp_wb_1_0_field_range_all_t **wbData =NULL;
    unsigned int rangeSize1;
    unsigned int index;
    unsigned int offset;
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;

    BCMDNX_ALLOC(wbData, unitData->wb.rangeTypes*sizeof(_bcm_dpp_wb_1_0_field_range_all_t *), "_bcm_dpp_field_range_all_wb_load:wbData ");
    if (wbData == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG_NO_UNIT("_bcm_dpp_field_range_all_wb_load  wbData allocation failed")));
    }

    if (0 == unitData->wb.offsetRange) {
        unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_FORMAT_FAIL;
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d does not appear to support"
                                           " range data in backing store"
                                           " version %04X\n"),
                          unit,
                          unitData->wb.version));
    }
#if _BCM_PPD_FIELD_WARM_BOOT_READ_TRACKING
    LOG_DEBUG(BSL_LS_BCM_INIT,
              (BSL_META_U(unit,
                          "unit %d recovering all range"
                          " information from warm boot buffer\n"),
               unit));
#endif /* _BCM_PPD_FIELD_WARM_BOOT_READ_TRACKING */
    switch (unitData->wb.version) {
    case _BCM_DPP_WB_FIELD_VERSION_1_0:
        /* make sure all range use is clear */
        for (index = 0; index < unitData->rangeQualTypes; index++) {
            sal_memset(unitData->rangeInUse[index],
                       0x00,
                       SHR_BITALLOCSIZE(_BCM_DPP_RANGE_MAX_COUNT));
        }
        /* figure out how big a range descriptor is in backing store */
        rangeSize1 = (sizeof(*(wbData[0])) +
                      (sizeof(uint32) *
                       (((unitData->wb.rangeLimit + 31) >> 5) - 1)));
        rangeSize1 = _BCM_DPP_WB_FIELD_ALIGNED_SIZE(rangeSize1);
        /* compute pointers to each range descriptor */
        wbData[0] = (_bcm_dpp_wb_1_0_field_range_all_t*)&(unitData->wb.buffer[unitData->wb.offsetRange]);
        for (index = 1; index < unitData->wb.rangeTypes; index++) {
            wbData[index] = (_bcm_dpp_wb_1_0_field_range_all_t*)(((uint8*)(wbData[index - 1])) + rangeSize1);
        }
        /* traverse the range descriptors in backing store */
        for (index = 0; index < unitData->wb.rangeTypes; index++) {
            /* find a corresponding range descriptor for hardware */
            for (offset = 0; offset < unitData->rangeQualTypes; offset++) {
                if (unitData->devInfo->ranges[offset].qualifier ==
                    wbData[index]->qualifyType) {
#if _BCM_PPD_FIELD_WARM_BOOT_READ_TRACKING
                    LOG_DEBUG(BSL_LS_BCM_INIT,
                              (BSL_META_U(unit,
                                          "unit %d recovering range type %u"
                                          " (%s, %d; %u ranges) from warm"
                                          " boot range %u (%u ranges)\n"),
                               unit,
                               offset,
                               _bcm_dpp_field_qual_name[unitData->devInfo->ranges[offset].qualifier],
                               unitData->devInfo->ranges[offset].qualifier,
                               unitData->devInfo->ranges[offset].count,
                               index,
                               wbData[index]->wb_rangeLimit));
#endif /* _BCM_PPD_FIELD_WARM_BOOT_READ_TRACKING */

#if defined(BROADCOM_DEBUG)

                     BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_wb_bmp_get(unitData,
                                                                      &(wbData[index]->rangeData[0]),
                                                                      wbData[index]->wb_rangeLimit,
                                                                      &(unitData->rangeInUse[offset][0]),
                                                                      unitData->devInfo->ranges[offset].count,
                                                                      "ranges",
                                                                       wbData[index]->qualifyType,
                                                                       _bcm_dpp_field_qual_name[wbData[index]->qualifyType]
));

#else /* defined(BROADCOM_DEBUG) */
                     BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_wb_bmp_get(unitData,

                                                                       &(wbData[index]->rangeData[0]),
                                                                       wbData[index]->wb_rangeLimit,
                                                                       &(unitData->rangeInUse[offset][0]),
                                                                       unitData->devInfo->ranges[offset].count,
                                                                       "ranges",
                                                                        wbData[index]->qualifyType,
                                                                        "-"
));

#endif /* defined(BROADCOM_DEBUG) */

                    break;
                } /* if (this type has same qual as backing store entry) */
            } /* for (all known range types) */
            if (offset >= unitData->rangeQualTypes) {
                unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_RESTORE_FAIL;
                BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                                 (_BSL_BCM_MSG_NO_UNIT("unit %d does not appear to"
                                                   " support ranges with"
                                                   " qualifier %s (%d), but"
                                                   " backing store has state"
                                                   " for such\n"),
                                  unit,
                                  (bcmFieldQualifyCount > wbData[index]->qualifyType)?_bcm_dpp_field_qual_name[wbData[index]->qualifyType]:"-",
                                  wbData[index]->qualifyType));
            }
        } /* for (all range types in backing store) */
        break;
    default:
        unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_VERSION_FAIL;
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d unexpected warm boot"
                                           " version %04X\n"),
                          unit,
                          unitData->wb.version));
    } /* switch (unitData->wb_version) */
exit:
    BCM_FREE(wbData);
    BCMDNX_FUNC_RETURN;
}

void
_bcm_dpp_field_unit_wb_save(_bcm_dpp_field_info_t *unitData,
                            uint8 *data,
                            unsigned int *size)
{
    _bcm_dpp_wb_1_0_field_unit_t *wbData;
    unsigned int unitSize;
    int result;
    int unit;

    unit = unitData->unit;

    if ((!size) && (0 == unitData->wb.offsetUnit)) {
        LOG_ERROR(BSL_LS_BCM_INIT,
                  (BSL_META_U(unit,
                              "unit %d does not appear to support unit"
                              " data in backing store version %04X\n"),
                   unit,
                   unitData->wb.version));
        unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_FORMAT_FAIL;
        return;
    }
    switch (unitData->wb.version) {
    case _BCM_DPP_WB_FIELD_VERSION_1_0:
        unitSize = sizeof(*wbData);
        unitSize = _BCM_DPP_WB_FIELD_ALIGNED_SIZE(unitSize);
        if (size) {
            *size = unitSize;
        } else { /* if (size) */
            wbData = sal_alloc(unitSize, "unit backing store temp");
            if (wbData) {
                sal_memset(wbData, 0x00, unitSize);
                /* keep all flags EXCEPT the warmboot alerts */
                wbData->unitFlags = (unitData->unitFlags &
                                     (~_BCM_DPP_FIELD_UNIT_WB_ANY_FAILURE));
#if _BCM_PPD_FIELD_WARM_BOOT_WRITE_TRACKING
                LOG_DEBUG(BSL_LS_BCM_INIT,
                          (BSL_META_U(unit,
                                      "unit %d backing store write unit"
                                      " info at %u for %u bytes\n"),
                           unit,
                           unitData->wb.offsetUnit,
                           unitSize));
#endif /* _BCM_PPD_FIELD_WARM_BOOT_WRITE_TRACKING */
                if (data) {
                    sal_memcpy(&(data[unitData->wb.offsetUnit]),
                               wbData,
                               unitSize);
                } else { /* if (data) */
                    sal_memcpy(&(unitData->wb.buffer[unitData->wb.offsetUnit]),
                               wbData,
                               unitSize);
					if (!(SOC_CONTROL(unitData->unit)->autosync) && !wb_field_is_scheduled_sync[unitData->unit]) {
					        /* mark dirty state. Currently no other processing done           */
					        /* note that there is no in module dirty bit optimization. use global dirty bit */
					        BCM_DPP_WB_DEV_FIELD_DIRTY_BIT_SET(unitData->unit);								        
					}
					else{
	                    result = soc_scache_commit_specific_data(unit,
	                                                             unitData->wb.handle,
	                                                             unitSize,
	                                                             (uint8*)wbData,
	                                                             unitData->wb.offsetUnit);
	                    if (SOC_E_NONE != result) {
	                        LOG_ERROR(BSL_LS_BCM_INIT,
                                          (BSL_META_U(unit,
                                                      "unit %d unable to update backing"
                                                      " store for data fields: %d (%s)\n"),
                                           unit,
                                           result,
                                           _SHR_ERRMSG(result)));
	                        unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_COMMIT_FAIL;
	                    }
	                }
                } /* if (data) */
                sal_free(wbData);
            } else { /* if (wbData) */
                LOG_ERROR(BSL_LS_BCM_INIT,
                          (BSL_META_U(unit,
                                      "unit %d unable to allocate temporary"
                                      " space to update backing store for"
                                      " field unit info"),
                           unitData->unit));
                unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_COMMIT_FAIL;
            } /* if (wbData) */
        } /* if (size) */
        break;
    default:
        LOG_ERROR(BSL_LS_BCM_INIT,
                  (BSL_META_U(unit,
                              "unit %d unexpected warm boot version %04X\n"),
                   unit,
                   unitData->wb.version));
        unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_VERSION_FAIL;
        if (size) {
            *size = 0;
        }
    } /* switch (unitData->wb_version) */
}

/*
 *  Function
 *    _bcm_dpp_field_unit_wb_load
 *  Purpose
 *    Load the unit's information from the backing store buffer
 *  Arguments
 *    IN unitData = pointer to the unit information
 *  Results
 *    bcm_error_t (cast as int)
 *      BCM_E_NONE if successful
 *      BCM_E_* otherwise as appropriate
 *  Notes
 *    This does not restore old flags concerning the warm boot error state.
 *    Instead, it leaves those flags as they currently are.
 */
STATIC int
_bcm_dpp_field_unit_wb_load(_bcm_dpp_field_info_t *unitData)
{
    _bcm_dpp_wb_1_0_field_unit_t *wbData;
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;

    if (0 == unitData->wb.offsetUnit) {
        unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_FORMAT_FAIL;
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d does not appear to support"
                                           " unit data in backing store"
                                           " version %04X\n"),
                          unit,
                          unitData->wb.version));
    }
#if _BCM_PPD_FIELD_WARM_BOOT_READ_TRACKING
    LOG_DEBUG(BSL_LS_BCM_INIT,
              (BSL_META_U(unit,
                          "unit %d recovering unit state"
                          " information from warm boot buffer\n"),
               unit));
#endif /* _BCM_PPD_FIELD_WARM_BOOT_READ_TRACKING */
    switch (unitData->wb.version) {
    case _BCM_DPP_WB_FIELD_VERSION_1_0:
        wbData = (_bcm_dpp_wb_1_0_field_unit_t*)&(unitData->wb.buffer[unitData->wb.offsetUnit]);
        unitData->unitFlags = ((unitData->unitFlags &
                                (_BCM_DPP_FIELD_UNIT_WB_ANY_FAILURE)) |
                               (wbData->unitFlags &
                                (~_BCM_DPP_FIELD_UNIT_WB_ANY_FAILURE)));
        break;
    default:
        unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_VERSION_FAIL;
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d unexpected warm boot"
                                           " version %04X\n"),
                          unit,
                          unitData->wb.version));
    } /* switch (unitData->wb_version) */
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *    _bcm_dpp_field_param_wb_save
 *  Purpose
 *    Init the parameters space in the warm boot buffer
 *  Arguments
 *    IN unitData = pointer to the unit information
 *    OUT data = where to put the data
 *    OUT size = pointer to where to put the size
 *  Results
 *    (none)
 *  Notes
 *    If size is not NULL, this will write the size of the parameters backing
 *    store record to the unsigned int pointed to by size.
 *
 *    If size is NULL, and data is NULL, this will write the information to the
 *    backing store.  If size is NULL and data is not NULL, this will treat the
 *    data as pointing to an image of the whole field backing store, and write
 *    the information at the same offset as writing to the backing store.
 *
 *    This always has something to write, since it applies to the whole unit,
 *    but there is not much to write, since subset writes are done for each
 *    invidivual part of the field APIs.
 *
 *    In general, since this only changes when setting up warm boot or when
 *    changing warm boot versions, this will only be called on those occasions
 *    or when performing a full warm boot sync.
 */
STATIC void
_bcm_dpp_field_param_wb_save(_bcm_dpp_field_info_t *unitData,
                             uint8 *data,
                             unsigned int *size)
{
    _bcm_dpp_wb_1_0_field_param_t *wbData;
    unsigned int unitSize;
    int result;
    int unit;

    unit = unitData->unit;

    switch (unitData->wb.version) {
    case _BCM_DPP_WB_FIELD_VERSION_1_0:
        unitSize = sizeof(*wbData);
        unitSize = _BCM_DPP_WB_FIELD_ALIGNED_SIZE(unitSize);
        if (size) {
            *size = unitSize;
        } else { /* if (size) */
            wbData = sal_alloc(unitSize, "param backing store temp");
            if (wbData) {
                sal_memset(wbData, 0x00, unitSize);
                wbData->offsetUnit = unitData->wb.offsetUnit;
                wbData->offsetStage = unitData->wb.offsetStage;
                wbData->offsetGroup = unitData->wb.offsetGroup;
                wbData->offsetTcamEntry = unitData->wb.offsetTcamEntry;
                wbData->offsetExtTcamEntry = unitData->wb.offsetExtTcamEntry;
                wbData->offsetDirExtEntry = unitData->wb.offsetDirExtEntry;
                wbData->offsetPresel = unitData->wb.offsetPresel;
                wbData->offsetDataField = unitData->wb.offsetDataField;
                wbData->offsetRange = unitData->wb.offsetRange;
                wbData->bcmFieldQualifyCount = unitData->wb.bcmFieldQualifyCount;
                wbData->bcmFieldActionCount = unitData->wb.bcmFieldActionCount;
                wbData->tcamBcmActionLimit = unitData->wb.tcamBcmActionLimit;
                wbData->ppdActionLimit = unitData->wb.ppdActionLimit;
                wbData->ppdActionCount = unitData->wb.ppdActionCount;
                wbData->ppdQualLimit = unitData->wb.ppdQualLimit;
                wbData->ppdQualCount = unitData->wb.ppdQualCount;
                wbData->preselLimit = unitData->wb.preselLimit;
                wbData->dataFieldLimit = unitData->wb.dataFieldLimit;
                wbData->tcamEntryLimit = unitData->wb.tcamEntryLimit;
                wbData->extTcamEntryLimit = unitData->wb.extTcamEntryLimit;
                wbData->dirextEntryLimit = unitData->wb.dirextEntryLimit;
                wbData->groupLimit = unitData->wb.groupLimit;
                wbData->stageLimit = unitData->wb.stageLimit;
                wbData->rangeLimit = unitData->wb.rangeLimit;
                wbData->rangeTypes = unitData->wb.rangeTypes;
#if _BCM_PPD_FIELD_WARM_BOOT_WRITE_TRACKING
                LOG_DEBUG(BSL_LS_BCM_INIT,
                          (BSL_META_U(unit,
                                      "unit %d backing store write param"
                                      " info at %u for %u bytes\n"),
                           unit,
                           0,
                           unitSize));
#endif /* _BCM_PPD_FIELD_WARM_BOOT_WRITE_TRACKING */
                if (data) {
                    sal_memcpy(&(data[0]),
                               wbData,
                               unitSize);
                } else { /* if (data) */
                    sal_memcpy(&(unitData->wb.buffer[0]),
                               wbData,
                               unitSize);
					if (!(SOC_CONTROL(unit)->autosync) && !wb_field_is_scheduled_sync[unit]) {
					        /* mark dirty state. Currently no other processing done           */
					        /* note that there is no in module dirty bit optimization. use global dirty bit */
					        BCM_DPP_WB_DEV_FIELD_DIRTY_BIT_SET(unit);								        
					}
					else{
	                    result = soc_scache_commit_specific_data(unit,
	                                                             unitData->wb.handle,
	                                                             unitSize,
	                                                             (uint8*)wbData,
	                                                             0);
	                    if (SOC_E_NONE != result) {
	                        LOG_ERROR(BSL_LS_BCM_INIT,
                                          (BSL_META_U(unit,
                                                      "unit %d unable to update backing"
                                                      " store for data fields: %d (%s)\n"),
                                           unit,
                                           result,
                                           _SHR_ERRMSG(result)));
	                        unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_COMMIT_FAIL;
	                    }
                    }
                } /* if (data) */
                sal_free(wbData);
            } else { /* if (wbData) */
                LOG_ERROR(BSL_LS_BCM_INIT,
                          (BSL_META_U(unit,
                                      "unit %d unable to allocate temporary"
                                      " space to update backing store for"
                                      " field unit info"),
                           unit));
                unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_COMMIT_FAIL;
            } /* if (wbData) */
        } /* if (size) */
        break;
    default:
        LOG_ERROR(BSL_LS_BCM_INIT,
                  (BSL_META_U(unit,
                              "unit %d unexpected warm boot version %04X\n"),
                   unit,
                   unitData->wb.version));
        unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_VERSION_FAIL;
        if (size) {
            *size = 0;
        }
    } /* switch (unitData->wb_version) */
}

/*
 *  Function
 *    _bcm_dpp_field_all_wb_state_sync_int
 *  Purpose
 *    Sync the entire field state to the warm boot buffer
 *  Arguments
 *    IN unitData = pointer to the unit information
 *    OUT data = where to put the data
 *    OUT size = pointer to where to put the size
 *  Results
 *    (none)
 *  Notes
 *    If size is not NULL, this will write the size of the entire set of
 *    backing store records to the unsigned int pointed to by size.
 *
 *    If size is NULL, and data is NULL, this will write the information to the
 *    backing store.  If size is NULL and data is not NULL, this will treat the
 *    data as pointing to an image of the whole field backing store, and will
 *    fill it in completely.
 *
 *    This tries to minimise writes if it can -- if the caller does not provide
 *    a buffer, it tries to allocate its own buffer.  This is to reduce
 *    overhead in writing to, and wear and tear of, the backing store.  If it
 *    can not allocate a sufficiently large buffer (which is possible, since
 *    the buffer it will need is pretty big), it simply performs the sync as
 *    individual writes.
 */
STATIC void
_bcm_dpp_field_all_wb_state_sync_int(_bcm_dpp_field_info_t *unitData,
                                     uint8 *data,
                                     unsigned int *size)
{
    unsigned int index;
    uint8* localData;
    int result;
    int unit;

    unit = unitData->unit;

    switch (unitData->wb.version) {
    case _BCM_DPP_WB_FIELD_VERSION_1_0:
        if (size) {
            /* caller wants total size */
            *size = unitData->wb.totalSize;
        } else { /* if (size) */
            /* caller wants state sync */
            if (data) {
                /* caller provided a buffer; write to it */
                localData = data;
            } else {
                /*
                 *  Caller did not provide a buffer, so write to the backing
                 *  store, but we don't want so many individual writes, so
                 *  collect all of the writes and then update the whole thing.
                 */
                localData = unitData->wb.buffer;
            }
            _bcm_dpp_field_param_wb_save(unitData, localData, NULL);
            if (unitData->wb.offsetUnit) {
                _bcm_dpp_field_unit_wb_save(unitData, localData, NULL);
            }
            if (unitData->wb.offsetRange) {
                _bcm_dpp_field_range_all_wb_save(unitData, localData, NULL);
            }
            if (unitData->wb.offsetDataField) {
                _bcm_dpp_field_datafield_all_wb_save(unitData, localData, NULL);
            }
            if (unitData->wb.offsetPresel) {
                _bcm_dpp_field_presel_all_wb_save(unitData, localData, NULL);
            }
            if (unitData->wb.offsetStage) {
                for (index = 0; index < unitData->wb.stageLimit; index++) {
                    _bcm_dpp_field_stage_wb_save(unitData,
                                                 index,
                                                 localData,
                                                 NULL);
                }
            }
            if (unitData->wb.offsetGroup) {
                for (index = 0; index < unitData->wb.groupLimit; index++) {
                    _bcm_dpp_field_group_wb_save(unitData,
                                                 index,
                                                 localData,
                                                 NULL);
                }
            }
            if (unitData->wb.offsetTcamEntry) {
                for (index = 0; index < unitData->wb.tcamEntryLimit; index++) {
                    _bcm_dpp_field_tcam_entry_wb_save(unitData,
                                                      index,
                                                      FALSE,
                                                      localData,
                                                      NULL);
                }
            }
            if (unitData->wb.offsetExtTcamEntry) {
                for (index = 0; index < unitData->wb.extTcamEntryLimit; index++) {
                    _bcm_dpp_field_tcam_entry_wb_save(unitData,
                                                      index,
                                                      TRUE,
                                                      localData,
                                                      NULL);
                }
            }
            if (unitData->wb.offsetDirExtEntry) {
                for (index = 0; index < unitData->wb.dirextEntryLimit; index++) {
                    _bcm_dpp_field_dir_ext_entry_wb_save(unitData,
                                                         index,
                                                         localData,
                                                         NULL);
                }
            }
            if (!data) {
                /*
                 *  Caller did not provide a buffer, so we wrote to the backing
                 *  store instead, but did not commit yet.  Commit now...
                 */
				if (!(SOC_CONTROL(unit)->autosync) && !wb_field_is_scheduled_sync[unit]) {
				        /* mark dirty state. Currently no other processing done           */
				        /* note that there is no in module dirty bit optimization. use global dirty bit */
				        BCM_DPP_WB_DEV_FIELD_DIRTY_BIT_SET(unit);								        
				}
				else{
                    result = soc_scache_commit_specific_data(unit,
                                                         unitData->wb.handle,
                                                         unitData->wb.totalSize,
                                                         (uint8*)localData,
                                                         0 /* offset */);
	                if (SOC_E_NONE != result) {
	                    LOG_ERROR(BSL_LS_BCM_INIT,
                                      (BSL_META_U(unit,
                                                  "unit %d unable to perform backing"
                                                  " store sync: %d (%s)\n"),
                                       unit,
                                       result,
                                       _SHR_ERRMSG(result)));
	                    unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_COMMIT_FAIL;
	                }
	            }
            } /* if (!data) */
        } /* if (size) */
        break;
    default:
        LOG_ERROR(BSL_LS_BCM_INIT,
                  (BSL_META_U(unit,
                              "unit %d unexpected warm boot version %04X\n"),
                   unit,
                   unitData->wb.version));
        unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_VERSION_FAIL;
        if (size) {
            *size = 0;
        }
    } /* switch (unitData->wb_version) */
}

int
_bcm_dpp_field_all_wb_state_sync(int unit)
{
    _bcm_dpp_field_info_t *unitData;
    BCMDNX_INIT_FUNC_DEFS;

    _DPP_FIELD_UNIT_CHECK(unit, unitData);
    _DPP_FIELD_UNIT_LOCK(unitData);

    wb_field_is_scheduled_sync[unit] = 1;
    _bcm_dpp_field_all_wb_state_sync_int(unitData, NULL, NULL);
    wb_field_is_scheduled_sync[unit] = 0;

    _DPP_FIELD_UNIT_UNLOCK(unitData);
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *    _bcm_dpp_field_wb_layout_init
 *  Purpose
 *    Prepare warm boot layout information
 *  Arguments
 *    IN unitData = pointer to the unit information
 *    IN version = warm boot information version number
 *    IN handle = warm boot handle to be used
 *    OUT totalSize = where to put the total size
 *  Results
 *    bcm_error_t (cast as int)
 *      BCM_E_NONE if success
 *      BCM_E_* otherwise as appropriate
 *  Notes
 *    This prepares the internal warm boot state (not the actual warm boot
 *    backing store buffer) according to the specified version.  This must be
 *    called during cold boot before the backing store buffer is manipulated.
 */
STATIC void
_bcm_dpp_field_wb_layout_init(_bcm_dpp_field_info_t *unitData,
                              uint16 version,
                              soc_scache_handle_t handle,
                              unsigned int *totalSize)
{
    unsigned int offset = 0;
    unsigned int size;
    int unit;

    unit = unitData->unit;

    sal_memset(&(unitData->wb), 0x00, sizeof(unitData->wb));
    switch (version) {
    case _BCM_DPP_WB_FIELD_VERSION_1_0:
        /* set up simple parameters */
        unitData->wb.version = version;
        unitData->wb.handle = handle;
        unitData->wb.bcmFieldQualifyCount = bcmFieldQualifyCount;
        unitData->wb.bcmFieldActionCount = bcmFieldActionCount;
        unitData->wb.tcamBcmActionLimit = _BCM_DPP_NOF_BCM_ACTIONS_PER_DB_MAX;
        unitData->wb.ppdActionLimit = SOC_PPD_FP_NOF_ACTIONS_PER_DB_MAX;
        unitData->wb.ppdActionCount = SOC_PPD_NOF_FP_ACTION_TYPES;
        unitData->wb.ppdQualLimit = SOC_PPD_FP_NOF_QUALS_PER_DB_MAX;
        unitData->wb.ppdQualCount = SOC_PPD_NOF_FP_QUAL_TYPES;
        unitData->wb.preselLimit = unitData->preselLimit;
        unitData->wb.dataFieldLimit = unitData->dqLimit;
        unitData->wb.tcamEntryLimit = unitData->entryTcLimit;
        unitData->wb.extTcamEntryLimit = unitData->entryExtTcLimit;
        unitData->wb.dirextEntryLimit = unitData->entryDeLimit;
        unitData->wb.groupLimit = unitData->groupLimit;
        unitData->wb.stageLimit = unitData->devInfo->stages;
        unitData->wb.rangeLimit = _BCM_DPP_RANGE_MAX_COUNT;
        unitData->wb.rangeTypes = unitData->rangeQualTypes;
        /*
         *  Get sizes for each element and compute offsets.  Note that if an
         *  element has size zero in the format, it is marked with offset zero,
         *  which indicates that element (or all elements of that type) are not
         *  kept in backing store for this version.
         */
        _bcm_dpp_field_param_wb_save(unitData, NULL, &size);
        offset += size;
        _bcm_dpp_field_unit_wb_save(unitData, NULL, &size);
        if (size) {
            unitData->wb.offsetUnit = offset;
            offset += size;
        }
        _bcm_dpp_field_range_all_wb_save(unitData, NULL, &size);
        if (size) {
            unitData->wb.offsetRange = offset;
            offset += size;
        }
        _bcm_dpp_field_datafield_all_wb_save(unitData, NULL, &size);
        if (size) {
            unitData->wb.offsetDataField = offset;
            offset += size;
        }
        _bcm_dpp_field_presel_all_wb_save(unitData, NULL, &size);
        if (size) {
            unitData->wb.offsetPresel = offset;
            offset += size;
        }
        _bcm_dpp_field_stage_wb_save(unitData, 0, NULL, &size);
        if (size) {
            size *= unitData->wb.stageLimit;
            unitData->wb.offsetStage = offset;
            offset += size;
        }
        _bcm_dpp_field_group_wb_save(unitData, 0, NULL, &size);
        if (size) {
            size *= unitData->wb.groupLimit;
            unitData->wb.offsetGroup = offset;
            offset += size;
        }
        _bcm_dpp_field_tcam_entry_wb_save(unitData, 0, FALSE /* internal TCAM */, NULL, &size);
        if (size) {
            size *= unitData->wb.tcamEntryLimit;
            unitData->wb.offsetTcamEntry = offset;
            offset += size;
        }
        _bcm_dpp_field_tcam_entry_wb_save(unitData, 0, TRUE /* external TCAM */, NULL, &size);
        if (size) {
            size *= unitData->wb.extTcamEntryLimit;
            unitData->wb.offsetExtTcamEntry = offset;
            offset += size;
        }
        _bcm_dpp_field_dir_ext_entry_wb_save(unitData, 0, NULL, &size);
        if (size) {
            size *= unitData->wb.dirextEntryLimit;
            unitData->wb.offsetDirExtEntry = offset;
            offset += size;
        }
        *totalSize = offset;
        unitData->wb.totalSize = offset;
        break;
    default:
        LOG_ERROR(BSL_LS_BCM_INIT,
                  (BSL_META_U(unit,
                              "unit %d unexpected warm boot version %04X\n"),
                   unit,
                   unitData->wb.version));
        unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_VERSION_FAIL;
    } /* switch (version) */
}

/*
 *  Function
 *    _bcm_dpp_field_wb_layout_load
 *  Purpose
 *    Prepare warm boot layout information
 *  Arguments
 *    IN unitData = pointer to the unit information
 *    IN version = warm boot information version number
 *    IN handle = warm boot handle to be used
 *  Results
 *    bcm_error_t (cast as int)
 *      BCM_E_NONE if success
 *      BCM_E_* otherwise as appropriate
 *  Notes
 *    This prepares the internal warm boot information according to the
 *    existing warm boot data from the backing store.  It must be used during
 *    warm boot before the backing store is otherwise accessed.
 */
STATIC int
_bcm_dpp_field_wb_layout_load(_bcm_dpp_field_info_t *unitData,
                              uint16 version,
                              soc_scache_handle_t handle,
                              uint8 *buffer)
{
    _bcm_dpp_wb_1_0_field_param_t *params;
    unsigned int size;
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;

    sal_memset(&(unitData->wb), 0x00, sizeof(unitData->wb));
    params = (_bcm_dpp_wb_1_0_field_param_t*)buffer;
#if _BCM_PPD_FIELD_WARM_BOOT_READ_TRACKING
    LOG_DEBUG(BSL_LS_BCM_INIT,
              (BSL_META_U(unit,
                          "unit %d recovering warm boot layout information"
                          " from warm boot buffer\n"),
               unit));
#endif /* _BCM_PPD_FIELD_WARM_BOOT_READ_TRACKING */
    switch (version) {
    case _BCM_DPP_WB_FIELD_VERSION_1_0:
        unitData->wb.version = version;
        unitData->wb.handle = handle;
        unitData->wb.buffer = buffer;
        unitData->wb.bcmFieldQualifyCount = params->bcmFieldQualifyCount;
        unitData->wb.bcmFieldActionCount = params->bcmFieldActionCount;
        unitData->wb.tcamBcmActionLimit = params->tcamBcmActionLimit;
        unitData->wb.ppdActionLimit = params->ppdActionLimit;
        unitData->wb.ppdActionCount = params->ppdActionCount;
        unitData->wb.ppdQualLimit = params->ppdQualLimit;
        unitData->wb.ppdQualCount = params->ppdQualCount;
        unitData->wb.preselLimit = params->preselLimit;
        unitData->wb.dataFieldLimit = params->dataFieldLimit;
        unitData->wb.tcamEntryLimit = params->tcamEntryLimit;
        unitData->wb.extTcamEntryLimit = params->extTcamEntryLimit;
        unitData->wb.dirextEntryLimit = params->dirextEntryLimit;
        unitData->wb.groupLimit = params->groupLimit;
        unitData->wb.stageLimit = params->stageLimit;
        unitData->wb.rangeLimit = params->rangeLimit;
        unitData->wb.rangeTypes = params->rangeTypes;
        unitData->wb.offsetUnit = params->offsetUnit;
        unitData->wb.offsetStage = params->offsetStage;
        unitData->wb.offsetGroup = params->offsetGroup;
        unitData->wb.offsetTcamEntry = params->offsetTcamEntry;
        unitData->wb.offsetExtTcamEntry = params->offsetExtTcamEntry;
        unitData->wb.offsetDirExtEntry = params->offsetDirExtEntry;
        unitData->wb.offsetPresel = params->offsetPresel;
        unitData->wb.offsetDataField = params->offsetDataField;
        unitData->wb.offsetRange = params->offsetRange;
        /* Get sizes for each element and compute total */
        _bcm_dpp_field_param_wb_save(unitData, NULL, &size);
        unitData->wb.totalSize += size;
        _bcm_dpp_field_unit_wb_save(unitData, NULL, &size);
        unitData->wb.totalSize += size;
        _bcm_dpp_field_range_all_wb_save(unitData, NULL, &size);
        unitData->wb.totalSize += size;
        _bcm_dpp_field_datafield_all_wb_save(unitData, NULL, &size);
        unitData->wb.totalSize += size;
        _bcm_dpp_field_presel_all_wb_save(unitData, NULL, &size);
        unitData->wb.totalSize += size;
        _bcm_dpp_field_stage_wb_save(unitData, 0, NULL, &size);
        size *= unitData->wb.stageLimit;
        unitData->wb.totalSize += size;
        _bcm_dpp_field_group_wb_save(unitData, 0, NULL, &size);
        size *= unitData->wb.groupLimit;
        unitData->wb.totalSize += size;
        _bcm_dpp_field_tcam_entry_wb_save(unitData, 0, FALSE, NULL, &size);
        size *= unitData->wb.tcamEntryLimit;
        unitData->wb.totalSize += size;
        _bcm_dpp_field_tcam_entry_wb_save(unitData, 0, TRUE, NULL, &size);
        size *= unitData->wb.extTcamEntryLimit;
        unitData->wb.totalSize += size;
        _bcm_dpp_field_dir_ext_entry_wb_save(unitData, 0, NULL, &size);
        size *= unitData->wb.dirextEntryLimit;
        unitData->wb.totalSize += size;
        break;
    default:
        unitData->unitFlags |= _BCM_DPP_FIELD_UNIT_WB_VERSION_FAIL;
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d unexpected warm boot"
                                           " version %04X\n"),
                          unit,
                          unitData->wb.version));
    } /* switch (version) */
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_wb_field_state_init(_bcm_dpp_field_info_t *unitData)
{
    int result = BCM_E_NONE;
    int flags = SOC_DPP_SCACHE_DEFAULT;
    int exists;
    uint16 version = _BCM_DPP_WB_FIELD_VERSION_CURR;
    uint16 oldVersion = 0;
    unsigned int size;
    soc_scache_handle_t handle;
    uint8 *buffer;
    int unit;

    unit = unitData->unit;

    SOC_SCACHE_HANDLE_SET(handle, unit, BCM_MODULE_FIELD, 0);

    wb_field_is_scheduled_sync[unit] = 0;

    if (SOC_WARM_BOOT(unit)) {
        /* warm boot */
        size = 0;
        /* fetch the existing warm boot space */
        result = soc_dpp_scache_ptr_get(unit,
                                        handle,
                                        socDppScacheRetrieve,
                                        flags,
                                        &size,
                                        &buffer,
                                        version,
                                        &oldVersion,
                                        &exists);
        if (BCM_E_NONE == result) {
            LOG_DEBUG(BSL_LS_BCM_INIT,
                      (BSL_META_U(unit,
                                  "unit %d loading backing store state\n"),
                       unit));
            result = _bcm_dpp_field_wb_layout_load(unitData,
                                                   oldVersion,
                                                   handle,
                                                   buffer);
            if (BCM_E_NONE == result) {
                result = _bcm_dpp_field_unit_wb_load(unitData);
            }
            if (BCM_E_NONE == result) {
                result = _bcm_dpp_field_range_all_wb_load(unitData);
            }
            if (BCM_E_NONE == result) {
                result = _bcm_dpp_field_datafield_all_wb_load(unitData);
            }
            if (BCM_E_NONE == result) {
                result = _bcm_dpp_field_presel_all_wb_load(unitData);
            }
            if (BCM_E_NONE == result) {
                result = _bcm_dpp_field_stage_all_wb_load(unitData);
            }
            if (BCM_E_NONE == result) {
                result = _bcm_dpp_field_group_all_wb_load(unitData);
            }
            if (BCM_E_NONE == result) {
                result = _bcm_dpp_field_dir_ext_entry_all_wb_load(unitData);
            }
            if (BCM_E_NONE == result) {
                result = _bcm_dpp_field_tcam_entry_all_wb_load(unitData);
            }
            if (BCM_E_NONE == result) {
                if (version != oldVersion) {
                    /* set up layout for the preferred version */
                    _bcm_dpp_field_wb_layout_init(unitData,
                                                  version,
                                                  handle,
                                                  &size);
                    LOG_DEBUG(BSL_LS_BCM_INIT,
                              (BSL_META_U(unit,
                                          "unit %d reallocate %d bytes warm"
                                          " boot backing store space\n"),
                               unit,
                               size));
                    /* reallocate the warm boot space */
                    result = soc_dpp_scache_ptr_get(unit,
                                                    handle,
                                                    socDppScacheRealloc,
                                                    flags,
                                                    &size,
                                                    &buffer,
                                                    version,
                                                    &oldVersion,
                                                    &exists);
                    if (BCM_E_NONE != result) {
                        LOG_ERROR(BSL_LS_BCM_INIT,
                                  (BSL_META_U(unit,
                                              "unable to reallocate %d bytes warm"
                                              " boot space for unit %d field"
                                              " instance: %d (%s)\n"),
                                   size,
                                   unit,
                                   result,
                                   _SHR_ERRMSG(result)));
                    }
                } /* if (version != oldVersion) */
            } /* if (BCM_E_NONE == result) */
        } else { /* if (BCM_E_NONE == result) */
            LOG_ERROR(BSL_LS_BCM_INIT,
                      (BSL_META_U(unit,
                                  "unable to get current warm boot state for"
                                  " unit %d field instance: %d (%s)\n"),
                       unit,
                       result,
                       _SHR_ERRMSG(result)));
        } /* if (BCM_E_NONE == result) */
    } else {
        /* set up layout for the preferred version */
        _bcm_dpp_field_wb_layout_init(unitData, version, handle, &size);
        /* set up backing store space */
        LOG_DEBUG(BSL_LS_BCM_INIT,
                  (BSL_META_U(unit,
                              "unit %d allocate %d bytes warm boot backing"
                              " store space\n"),
                   unit,
                   size));
        result = soc_dpp_scache_ptr_get(unit,
                                        handle,
                                        socDppScacheCreate,
                                        flags,
                                        &size,
                                        &buffer,
                                        version,
                                        &oldVersion,
                                        &exists);
        oldVersion = 0; /* force full sync */
        if (BCM_E_NONE != result) {
            LOG_ERROR(BSL_LS_BCM_INIT,
                      (BSL_META_U(unit,
                                  "unable to allocate %d bytes warm boot space"
                                  " for unit %d field instance: %d (%s)\n"),
                       size,
                       unit,
                       result,
                       _SHR_ERRMSG(result)));
        }
    }
    if ((BCM_E_NONE == result) && (oldVersion != version)) {
        /* finish setting things up */
        unitData->wb.buffer = buffer;
        /* build an initial image in the buffer */
        _bcm_dpp_field_all_wb_state_sync_int(unitData, buffer, NULL);
        BCM_DPP_WB_DEV_DIRTY_BIT_SET(unit);
    }
    return result;
}

void
_bcm_dpp_wb_field_state_dump(_bcm_dpp_field_info_t *unitData,
                             const char *prefix)
{
    unsigned int elemSize;
    unsigned int partSize;
    unsigned int index;
    unsigned int offset;
    unsigned int count;
    uint32 value;
    int unit;

    unit = unitData->unit;

    /*
     *  The implementation here is split into a bunch of switch statements
     *  instead of a single one because we may use the same format internally
     *  for some sections even if we change the version number to accommodate
     *  changes to other sections.
     */
    LOG_CLI((BSL_META_U(unit,
                        "%sWarm boot information:\n"), prefix));
    LOG_CLI((BSL_META_U(unit,
                        "%s  Attributes:\n"), prefix));
    LOG_CLI((BSL_META_U(unit,
                        "%s    Handle         = %08X\n"), prefix, unitData->wb.handle));
    LOG_CLI((BSL_META_U(unit,
                        "%s    Version        =     %04X\n"), prefix, unitData->wb.version));
    LOG_CLI((BSL_META_U(unit,
                        "%s    Base pointer   = %p\n"), prefix, (void*)unitData->wb.buffer));
    LOG_CLI((BSL_META_U(unit,
                        "%s    Total size     = %8d\n"), prefix, unitData->wb.totalSize));

    switch (unitData->wb.version) {
    case _BCM_DPP_WB_FIELD_VERSION_1_0:
        LOG_CLI((BSL_META_U(unit,
                            "%s  Offsets, sizes per element, count:\n"), prefix));
        _bcm_dpp_field_param_wb_save(unitData, NULL, &elemSize);
        LOG_CLI((BSL_META_U(unit,
                            "%s    Parameters     = %08X, %3u, %5u\n"), prefix, 0, elemSize, 1));
        _bcm_dpp_field_unit_wb_save(unitData, NULL, &elemSize);
        LOG_CLI((BSL_META_U(unit,
                            "%s    Unit           = %08X, %3u, %5u\n"), prefix, unitData->wb.offsetUnit, elemSize, 1));
        _bcm_dpp_field_range_all_wb_save(unitData, NULL, &elemSize);
        LOG_CLI((BSL_META_U(unit,
                            "%s    Range          = %08X, %3u, %5u\n"), prefix, unitData->wb.offsetRange, elemSize, 1));
        _bcm_dpp_field_datafield_all_wb_save(unitData, NULL,  &elemSize);
        LOG_CLI((BSL_META_U(unit,
                            "%s    Data Field     = %08X, %3u, %5u\n"), prefix, unitData->wb.offsetDataField, elemSize, 1));
        _bcm_dpp_field_presel_all_wb_save(unitData, NULL, &elemSize);
        LOG_CLI((BSL_META_U(unit,
                            "%s    Preselector    = %08X, %3u, %5u\n"), prefix, unitData->wb.offsetPresel, elemSize, 1));
        _bcm_dpp_field_stage_wb_save(unitData, 0, NULL, &elemSize);
        LOG_CLI((BSL_META_U(unit,
                            "%s    Stage          = %08X, %3u, %5u\n"), prefix, unitData->wb.offsetStage, elemSize, unitData->wb.stageLimit));
        _bcm_dpp_field_group_wb_save(unitData, 0, NULL, &elemSize);
        LOG_CLI((BSL_META_U(unit,
                            "%s    Group          = %08X, %3u, %5u\n"), prefix, unitData->wb.offsetGroup, elemSize, unitData->wb.groupLimit));
        _bcm_dpp_field_tcam_entry_wb_save(unitData, 0, FALSE, NULL, &elemSize);
        LOG_CLI((BSL_META_U(unit,
                            "%s    TCAM Entry     = %08X, %3u, %5u\n"), prefix, unitData->wb.offsetTcamEntry, elemSize, unitData->wb.tcamEntryLimit));
        _bcm_dpp_field_tcam_entry_wb_save(unitData, 0, TRUE, NULL, &elemSize);        
		LOG_CLI((BSL_META_U(unit,
                                    "%s    External TCAM Entry     = %08X, %3u, %5u\n"), prefix, unitData->wb.offsetExtTcamEntry, elemSize, unitData->wb.extTcamEntryLimit));
		_bcm_dpp_field_dir_ext_entry_wb_save(unitData, 0, NULL, &elemSize);
        LOG_CLI((BSL_META_U(unit,
                            "%s    Dir Ext Entry  = %08X, %3u, %5u\n"), prefix, unitData->wb.offsetDirExtEntry, elemSize, unitData->wb.dirextEntryLimit));
        LOG_CLI((BSL_META_U(unit,
                            "%s  Limits (in addition to above):\n"), prefix));
        LOG_CLI((BSL_META_U(unit,
                            "%s    QualifyCount   = %8u\n"), prefix, unitData->wb.bcmFieldQualifyCount));
        LOG_CLI((BSL_META_U(unit,
                            "%s    ActionCount    = %8u\n"), prefix, unitData->wb.bcmFieldActionCount));
        LOG_CLI((BSL_META_U(unit,
                            "%s    BCM action lim = %8u\n"), prefix, unitData->wb.tcamBcmActionLimit));
        LOG_CLI((BSL_META_U(unit,
                            "%s    PPD action lim = %8u\n"), prefix, unitData->wb.ppdActionLimit));
        LOG_CLI((BSL_META_U(unit,
                            "%s    PPD action cnt = %8u\n"), prefix, unitData->wb.ppdActionCount));
        LOG_CLI((BSL_META_U(unit,
                            "%s    PPD qual limit = %8u\n"), prefix, unitData->wb.ppdQualLimit));
        LOG_CLI((BSL_META_U(unit,
                            "%s    PPD qual count = %8u\n"), prefix, unitData->wb.ppdQualCount));
        LOG_CLI((BSL_META_U(unit,
                            "%s    Presel limit   = %8u\n"), prefix, unitData->wb.preselLimit));
        LOG_CLI((BSL_META_U(unit,
                            "%s    DataField lim  = %8u\n"), prefix, unitData->wb.dataFieldLimit));
        LOG_CLI((BSL_META_U(unit,
                            "%s    Range types    = %8u\n"), prefix, unitData->wb.rangeTypes));
        LOG_CLI((BSL_META_U(unit,
                            "%s    Range limit    = %8u\n"), prefix, unitData->wb.rangeLimit));
        break;
    default:
        LOG_CLI((BSL_META_U(unit,
                            "%s    (unsupported version)\n"), prefix));
    } /* switch (version) */

    LOG_CLI((BSL_META_U(unit,
                        "%s  Unit information:\n"), prefix));
    switch (unitData->wb.version) {
    case _BCM_DPP_WB_FIELD_VERSION_1_0:
        LOG_CLI((BSL_META_U(unit,
                            "%s    Unit flags     = %08X\n"),
                 prefix,
                 ((_bcm_dpp_wb_1_0_field_unit_t*)&(unitData->wb.buffer[unitData->wb.offsetUnit]))->unitFlags));
        break;
    default:
        LOG_CLI((BSL_META_U(unit,
                            "%s    (unsupported version)\n"), prefix));
    } /* switch (version) */

    LOG_CLI((BSL_META_U(unit,
                        "%s  Range information:\n"), prefix));
    switch (unitData->wb.version) {
    case _BCM_DPP_WB_FIELD_VERSION_1_0:
        elemSize = (sizeof(_bcm_dpp_wb_1_0_field_range_all_t) +
                    (sizeof(uint32) *
                     (((unitData->wb.rangeLimit + 31) >> 5) - 1)));
        for (index = 0; index < unitData->wb.rangeTypes; index++) {
            value = ((_bcm_dpp_wb_1_0_field_range_all_t*)(&(unitData->wb.buffer[unitData->wb.offsetRange + (elemSize * index)])))->qualifyType;
            LOG_CLI((BSL_META_U(unit,
                                "%s    Range type %u: %s (%d)\n"),
                     prefix,
                     index,
                     (bcmFieldQualifyCount > value)?_bcm_dpp_field_qual_name[value]:"-",
                     value));
            LOG_CLI((BSL_META_U(unit,
                                "%s      Ranges         = %8u\n"),
                     prefix,
                     ((_bcm_dpp_wb_1_0_field_range_all_t*)(&(unitData->wb.buffer[unitData->wb.offsetRange + (elemSize * index)])))->wb_rangeLimit));
            LOG_CLI((BSL_META_U(unit,
                                "%s      In-use bitmap  ="), prefix));
            offset = (unitData->wb.rangeLimit + 31) >> 5;
            while (offset > 0) {
                offset--;
                LOG_CLI((BSL_META_U(unit,
                                    " %08X"),
                         ((_bcm_dpp_wb_1_0_field_range_all_t*)(&(unitData->wb.buffer[unitData->wb.offsetRange + (elemSize * index)])))->rangeData[offset]));
            }
            LOG_CLI((BSL_META_U(unit,
                                "\n")));
        }
        break;
    default:
        LOG_CLI((BSL_META_U(unit,
                            "%s    (unsupported version)\n"), prefix));
    } /* switch (version) */

    LOG_CLI((BSL_META_U(unit,
                        "%s  Data field information:\n"), prefix));
    switch (unitData->wb.version) {
    case _BCM_DPP_WB_FIELD_VERSION_1_0:
        for (index = 0, count = 0;
             index < unitData->wb.dataFieldLimit;
             index++) {
            value = ((_bcm_dpp_wb_1_0_field_datafield_all_t*)&(unitData->wb.buffer[unitData->wb.offsetDataField]))->dataFieldRefs[index];
            if (value & 0x3FFFFFFF) {
                LOG_CLI((BSL_META_U(unit,
                                    "%s    Data field %3u: %s, %s, %5u refs\n"),
                         prefix,
                         index,
                         (value & 0x80000000)?"+LENBIT":"-lenbit",
                         (value & 0x40000000)?"+OFSBIT":"-ofsbit",
                         (value & 0x3FFFFFFF) - 1));
                count++;
            }
        }
        if (!count) {
            LOG_CLI((BSL_META_U(unit,
                                "%s    (none)\n"), prefix));
        }
        break;
    default:
        LOG_CLI((BSL_META_U(unit,
                            "%s    (unsupported version)\n"), prefix));
    } /* switch (version) */

    LOG_CLI((BSL_META_U(unit,
                        "%s  Preselector information:\n"), prefix));
    switch (unitData->wb.version) {
    case _BCM_DPP_WB_FIELD_VERSION_1_0:
        elemSize = (unitData->wb.preselLimit + 31) >> 5;
        for (index = 0; index < unitData->wb.preselLimit; index++) {
            value = ((_bcm_dpp_wb_1_0_field_presel_all_t*)&(unitData->wb.buffer[unitData->wb.offsetPresel]))->preselData[index];
            if (value) {
                LOG_CLI((BSL_META_U(unit,
                                    "%s    Preselector %3u  : %8u refs\n"),
                         prefix,
                         index,
                         value - 1));
            }
        } /* for (index = 0; index < unitData->wb.preselLimit; index++) */
        break;
    default:
        LOG_CLI((BSL_META_U(unit,
                            "%s    (unsupported version)\n"), prefix));
    } /* switch (version) */

    LOG_CLI((BSL_META_U(unit,
                        "%s  Stage information:\n"), prefix));
    switch (unitData->wb.version) {
    case _BCM_DPP_WB_FIELD_VERSION_1_0:
        partSize = (unitData->wb.preselLimit + 31) >> 5;
        elemSize = ((partSize * 4) + 1) * sizeof(uint32);
        for (index = 0, count = 0;
             index < unitData->wb.stageLimit;
             index++) {
            value = ((_bcm_dpp_wb_1_0_field_stage_t*)&(unitData->wb.buffer[unitData->wb.offsetStage + (elemSize * index)]))->ipfgs[0];
            LOG_CLI((BSL_META_U(unit,
                                "%s    Stage %d: %s (%d)\n"),
                     prefix,
                     index,
                     SOC_PPC_FP_DATABASE_STAGE_to_string(value),
                     value));
            LOG_CLI((BSL_META_U(unit,
                                "%s      L2 presel set    ="), prefix));
            offset = partSize;
            while (offset > 0) {
                offset--;
                LOG_CLI((BSL_META_U(unit,
                                    " %08X"),
                         ((_bcm_dpp_wb_1_0_field_stage_t*)&(unitData->wb.buffer[unitData->wb.offsetStage + (elemSize * index)]))->ipfgs[1 + offset]));
            }
            LOG_CLI((BSL_META_U(unit,
                                "\n%s      IPv4 presel set  ="), prefix));
            offset = partSize;
            while (offset > 0) {
                offset--;
                LOG_CLI((BSL_META_U(unit,
                                    " %08X"),
                         ((_bcm_dpp_wb_1_0_field_stage_t*)&(unitData->wb.buffer[unitData->wb.offsetStage + (elemSize * index)]))->ipfgs[1 + offset + partSize]));
            }
            LOG_CLI((BSL_META_U(unit,
                                "\n%s      IPv6 presel set  ="), prefix));
            offset = partSize;
            while (offset > 0) {
                offset--;
                LOG_CLI((BSL_META_U(unit,
                                    " %08X"),
                         ((_bcm_dpp_wb_1_0_field_stage_t*)&(unitData->wb.buffer[unitData->wb.offsetStage + (elemSize * index)]))->ipfgs[1 + offset + (partSize * 2)]));
            }
            LOG_CLI((BSL_META_U(unit,
                                "\n%s      MPLS presel set  ="), prefix));
            offset = partSize;
            while (offset > 0) {
                offset--;
                LOG_CLI((BSL_META_U(unit,
                                    " %08X"),
                         ((_bcm_dpp_wb_1_0_field_stage_t*)&(unitData->wb.buffer[unitData->wb.offsetStage + (elemSize * index)]))->ipfgs[1 + offset + (partSize * 3)]));
            }
            LOG_CLI((BSL_META_U(unit,
                                "\n")));
            count++;
        }
        if (!count) {
            LOG_CLI((BSL_META_U(unit,
                                "%s      (none)\n"), prefix));
        }
        break;
    default:
        LOG_CLI((BSL_META_U(unit,
                            "%s    (unsupported version)\n"), prefix));
    }

    LOG_CLI((BSL_META_U(unit,
                        "%s  Group information:\n"), prefix));
    switch (unitData->wb.version) {
    case _BCM_DPP_WB_FIELD_VERSION_1_0:
        elemSize = (sizeof(_bcm_dpp_wb_1_0_field_group_t) +
                    (sizeof(uint32) *
                     (((unitData->wb.bcmFieldActionCount + 31) >> 5) - 1)));
        for (index = 0, count = 0;
             index < unitData->wb.groupLimit;
             index++) {
            value = ((_bcm_dpp_wb_1_0_field_group_t*)&(unitData->wb.buffer[unitData->wb.offsetGroup + (elemSize * index)]))->groupFlags;
            if (value & _BCM_DPP_FIELD_GROUP_IN_USE) {
                LOG_CLI((BSL_META_U(unit,
                                    "%s    Group %d:\n"), prefix, index));
                LOG_CLI((BSL_META_U(unit,
                                    "%s      Flags     = %08X\n"),
                         prefix,
                         value));
                LOG_CLI((BSL_META_U(unit,
                                    "%s      HW handle = %08X\n"),
                         prefix,
                         ((_bcm_dpp_wb_1_0_field_group_t*)&(unitData->wb.buffer[unitData->wb.offsetGroup + (elemSize * index)]))->hwHandle));
                LOG_CLI((BSL_META_U(unit,
                                    "%s      PredefKey = %08X\n"),
                         prefix,
                         ((_bcm_dpp_wb_1_0_field_group_t*)&(unitData->wb.buffer[unitData->wb.offsetGroup + (elemSize * index)]))->predefKey));
                value = ((_bcm_dpp_wb_1_0_field_group_t*)&(unitData->wb.buffer[unitData->wb.offsetGroup + (elemSize * index)]))->grpStage;
                LOG_CLI((BSL_META_U(unit,
                                    "%s      Stage     = %s (%d)\n"),
                         prefix,
                         (unitData->devInfo->stages > value)?unitData->devInfo->stage[value].stageName:"-",
                         value));
                value = ((_bcm_dpp_wb_1_0_field_group_t*)&(unitData->wb.buffer[unitData->wb.offsetGroup + (elemSize * index)]))->grpMode;
                LOG_CLI((BSL_META_U(unit,
                                    "%s      Mode      = %s (%d)\n"),
                         prefix,
                         (bcmFieldGroupModeCount>value)?_bcm_dpp_field_group_mode_name[value]:"-",
                         value));
                LOG_CLI((BSL_META_U(unit,
                                    "%s      Orig ASET ="), prefix));
                offset = (unitData->wb.bcmFieldActionCount + 31) >> 5;
                while (offset > 0) {
                    offset--;
                    LOG_CLI((BSL_META_U(unit,
                                        " %08X"),
                             ((_bcm_dpp_wb_1_0_field_group_t*)&(unitData->wb.buffer[unitData->wb.offsetGroup + (elemSize * index)]))->oaset[offset]));
                }
                LOG_CLI((BSL_META_U(unit,
                                    "\n")));
                count++;
            } /* if (value & _BCM_DPP_FIELD_GROUP_IN_USE) */
        } /* for (all possible groups) */
        if (!count) {
            LOG_CLI((BSL_META_U(unit,
                                "%s    (none)\n"), prefix));
        }
        break;
    default:
        LOG_CLI((BSL_META_U(unit,
                            "%s    (unsupported version)\n"), prefix));
    } /* switch (version) */

    LOG_CLI((BSL_META_U(unit,
                        "%s  TCAM entry information:\n"), prefix));
    switch (unitData->wb.version) {
    case _BCM_DPP_WB_FIELD_VERSION_1_0:
        elemSize = (sizeof(_bcm_dpp_wb_1_0_field_entry_tcam_t) +
                    (sizeof(_bcm_dpp_wb_1_0_field_entry_tcam_action_t) *
                     (unitData->wb.tcamBcmActionLimit - 1)));
        for (index = 0, count = 0;
             index < unitData->wb.tcamEntryLimit;
             index++) {
            value = ((_bcm_dpp_wb_1_0_field_entry_tcam_t*)&(unitData->wb.buffer[unitData->wb.offsetTcamEntry + (elemSize * index)]))->entryCmn.entryFlags;
            if (value & _BCM_DPP_FIELD_ENTRY_IN_USE) {
                LOG_CLI((BSL_META_U(unit,
                                    "%s    TCAM entry %u:\n"),
                         prefix,
                         index + _BCM_DPP_FIELD_ENT_BIAS(unit, InternalTcam)));
                LOG_CLI((BSL_META_U(unit,
                                    "%s      Flags    = %08X\n"),
                         prefix,
                         value));
                LOG_CLI((BSL_META_U(unit,
                                    "%s      HWHandle = %08X\n"),
                         prefix,
                         ((_bcm_dpp_wb_1_0_field_entry_tcam_t*)&(unitData->wb.buffer[unitData->wb.offsetTcamEntry + (elemSize * index)]))->entryCmn.hwHandle));
                LOG_CLI((BSL_META_U(unit,
                                    "%s      Priority = %d\n"),
                         prefix,
                         ((_bcm_dpp_wb_1_0_field_entry_tcam_t*)&(unitData->wb.buffer[unitData->wb.offsetTcamEntry + (elemSize * index)]))->entryCmn.entryPriority));
                LOG_CLI((BSL_META_U(unit,
                                    "%s      Group    = %d\n"),
                         prefix,
                         ((_bcm_dpp_wb_1_0_field_entry_tcam_t*)&(unitData->wb.buffer[unitData->wb.offsetTcamEntry + (elemSize * index)]))->entryCmn.entryGroup));
                for (offset = 0;
                     offset < unitData->wb.tcamBcmActionLimit;
                     offset++) {
                    value = ((_bcm_dpp_wb_1_0_field_entry_tcam_t*)&(unitData->wb.buffer[unitData->wb.offsetTcamEntry + (elemSize * index)]))->tcamAction[offset].bcmType;
                    if (bcmFieldActionCount > value) {
                        LOG_CLI((BSL_META_U(unit,
                                            "%s      Action %2d      : %s (%d)\n"),
                                 prefix,
                                 offset,
                                 (bcmFieldActionCount > value)?_bcm_dpp_field_action_name[value]:"-",
                                 value));
                        LOG_CLI((BSL_META_U(unit,
                                            "%s                       %08X,%08X\n"),
                                 prefix,
                                 ((_bcm_dpp_wb_1_0_field_entry_tcam_t*)&(unitData->wb.buffer[unitData->wb.offsetTcamEntry + (elemSize * index)]))->tcamAction[offset].bcmParam0,
                                 ((_bcm_dpp_wb_1_0_field_entry_tcam_t*)&(unitData->wb.buffer[unitData->wb.offsetTcamEntry + (elemSize * index)]))->tcamAction[offset].bcmParam1));
                    }
                }
                count++;
            } /* if (value & _BCM_DPP_FIELD_ENTRY_IN_USE) */
        } /* for (all possible TCAM entries) */
        if (!count) {
            LOG_CLI((BSL_META_U(unit,
                                "%s    (none)\n"), prefix));
        }
        break;
    default:
        LOG_CLI((BSL_META_U(unit,
                            "%s    (unsupported version)\n"), prefix));
    } /* switch (version) */

    LOG_CLI((BSL_META_U(unit,
                        "%s  External TCAM entry information:\n"), prefix));
    switch (unitData->wb.version) {
    case _BCM_DPP_WB_FIELD_VERSION_1_0:
        elemSize = sizeof(_bcm_dpp_wb_1_0_field_entry_ext_tcam_t);
        for (index = 0, count = 0;
             index < unitData->wb.extTcamEntryLimit;
             index++) {
            value = ((_bcm_dpp_wb_1_0_field_entry_ext_tcam_t*)&(unitData->wb.buffer[unitData->wb.offsetExtTcamEntry + (elemSize * index)]))->entryCmn.entryFlags;
            if (value & _BCM_DPP_FIELD_ENTRY_IN_USE) {
                LOG_CLI((BSL_META_U(unit,
                                    "%s    External TCAM entry %u:\n"),
                         prefix,
                         index + _BCM_DPP_FIELD_ENT_BIAS(unit, ExternalTcam)));
                LOG_CLI((BSL_META_U(unit,
                                    "%s      Flags    = %08X\n"),
                         prefix,
                         value));
                LOG_CLI((BSL_META_U(unit,
                                    "%s      HWHandle = %08X\n"),
                         prefix,
                         ((_bcm_dpp_wb_1_0_field_entry_ext_tcam_t*)&(unitData->wb.buffer[unitData->wb.offsetExtTcamEntry + (elemSize * index)]))->entryCmn.hwHandle));
                LOG_CLI((BSL_META_U(unit,
                                    "%s      Group    = %d\n"),
                         prefix,
                         ((_bcm_dpp_wb_1_0_field_entry_ext_tcam_t*)&(unitData->wb.buffer[unitData->wb.offsetExtTcamEntry + (elemSize * index)]))->entryCmn.entryGroup));
                count++;
            } /* if (value & _BCM_DPP_FIELD_ENTRY_IN_USE) */
        } /* for (all possible external TCAM entries) */
        if (!count) {
            LOG_CLI((BSL_META_U(unit,
                                "%s    (none)\n"), prefix));
        }
        break;
    default:
        LOG_CLI((BSL_META_U(unit,
                            "%s    (unsupported version)\n"), prefix));
    } /* switch (version) */

    LOG_CLI((BSL_META_U(unit,
                        "%s  Dir Ext entry information:\n"), prefix));
    switch (unitData->wb.version) {
    case _BCM_DPP_WB_FIELD_VERSION_1_0:
        elemSize = (sizeof(_bcm_dpp_wb_1_0_field_entry_dir_ext_t) +
                    (sizeof(_bcm_dpp_wb_1_0_field_entry_dir_ext_action_t) *
                     (unitData->wb.ppdActionLimit - 1)));
        for (index = 0, count = 0;
             index < unitData->wb.dirextEntryLimit;
             index++) {
            value = ((_bcm_dpp_field_entry_dir_ext_t*)&(unitData->wb.buffer[unitData->wb.offsetDirExtEntry + (elemSize * index)]))->entryCmn.entryFlags;
            if (value & _BCM_DPP_FIELD_ENTRY_IN_USE) {
                LOG_CLI((BSL_META_U(unit,
                                    "%s    Dir Ext entry %u:\n"),
                         prefix,
                         index + _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt)));
                LOG_CLI((BSL_META_U(unit,
                                    "%s      Flags    = %08X\n"),
                         prefix,
                         value));
                LOG_CLI((BSL_META_U(unit,
                                    "%s      HWHandle = %08X\n"),
                         prefix,
                         ((_bcm_dpp_wb_1_0_field_entry_dir_ext_t*)&(unitData->wb.buffer[unitData->wb.offsetTcamEntry + (elemSize * index)]))->entryCmn.hwHandle));
                LOG_CLI((BSL_META_U(unit,
                                    "%s      Priority = %d\n"),
                         prefix,
                         ((_bcm_dpp_wb_1_0_field_entry_dir_ext_t*)&(unitData->wb.buffer[unitData->wb.offsetDirExtEntry + (elemSize * index)]))->entryCmn.entryPriority));
                LOG_CLI((BSL_META_U(unit,
                                    "%s      Group    = %d\n"),
                         prefix,
                         ((_bcm_dpp_wb_1_0_field_entry_dir_ext_t*)&(unitData->wb.buffer[unitData->wb.offsetTcamEntry + (elemSize * index)]))->entryCmn.entryGroup));
                for (offset = 0;
                     offset < unitData->wb.ppdActionLimit;
                     offset++) {
                    value = ((_bcm_dpp_wb_1_0_field_entry_dir_ext_t*)&(unitData->wb.buffer[unitData->wb.offsetTcamEntry + (elemSize * index)]))->dirExtAction[offset].bcmType;
                    if (bcmFieldActionCount > value) {
                        LOG_CLI((BSL_META_U(unit,
                                            "%s      Action %2d      : %s (%d)\n"),
                                 prefix,
                                 offset,
                                 (bcmFieldActionCount > value)?_bcm_dpp_field_action_name[value]:"-",
                                 value));
                    }
                }
                count++;
            } /* if (value & _BCM_DPP_FIELD_ENTRY_IN_USE) */
        } /* for (all possible Direct Extraction entries) */
        if (!count) {
            LOG_CLI((BSL_META_U(unit,
                                "%s    (none)\n"), prefix));
        }
        break;
    default:
        LOG_CLI((BSL_META_U(unit,
                            "%s    (unsupported version)\n"), prefix));
    } /* switch (version) */

}

#endif /* def BCM_WARM_BOOT_SUPPORT */

