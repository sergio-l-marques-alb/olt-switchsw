/*
 * $Id: wb_db_vlan.c,v 1.9 Broadcom SDK $
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
 * Module: VLAN APIs
 *
 * Purpose:
 *     VLAN API for Dune Packet Processor devices
 *     Warm boot support
 */

#include <bcm/error.h>
#include <bcm/module.h>
#include <shared/bitop.h>

#include <bcm_int/common/debug.h>
#include <bcm_int/dpp/error.h>
#include <bcm_int/dpp/vlan.h>
#include <bcm_int/dpp/wb_db_cmn.h>
#include <bcm_int/dpp/wb_db_vlan.h>
#include <soc/dpp/soc_sw_db.h>
#include <soc/dpp/drv.h>

#ifdef BCM_WARM_BOOT_SUPPORT

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_BCM_INIT

#include <shared/bsl.h>

/*
 *  Set _BCM_DPP_VLAN_WARMBOOT_WRITE_TRACKING to TRUE if you want diagnostics
 *  displayed at VERB level every time the VLAN backing store is written.
 */
#define _BCM_DPP_VLAN_WARMBOOT_WRITE_TRACKING TRUE

/*
 *  Set _BCM_DPP_VLAN_WARMBOOT_READ_TRACKING to TRUE if you want diagnostics
 *  displayed at VERB level every time VLAN backing store is read.
 */
#define _BCM_DPP_VLAN_WARMBOOT_READ_TRACKING TRUE


uint8 wb_vlan_is_scheduled_sync[BCM_MAX_NUM_UNITS];
/*
 *  Function
 *    _bcm_dpp_vlan_wb_pbmp_put
 *  Purpose
 *    Write a PBMP to backing store
 *  Arguments
 *    (IN) unit = unit ID
 *    (IN) bitmap = pointer to bitmap to write
 *    (OUT) ptr = where to write bits
 *    (IN) desc = description of this bitmap
 *  Results
 *    (none)
 *  Notes
 *    Assumes backing store bitmap was all zeroes before call
 */
STATIC void
_bcm_dpp_vlan_wb_pbmp_put(int unit,
                          bcm_pbmp_t *bitmap,
                          uint32 *ptr,
                          const char *desc)
{
    unsigned int index;
    for (index = 0; index < _SHR_PBMP_PORT_MAX; index++) {
        if (index < _bcm_dpp_vlan_unit_state[unit]->wb.portsInPbmp) {
            if (BCM_PBMP_MEMBER((*bitmap), index)) {
                ptr[index >> 5] |= (1 << (index & 0x1F));
            }
        } else { /* if (index is within backing store PBMP) */
            if (BCM_PBMP_MEMBER((*bitmap), index)) {
                LOG_ERROR(BSL_LS_BCM_INIT,
                          (BSL_META_U(unit,
                                      "unit %d backing store supports"
                                      " %u %s, but it has bit %u set\n"),
                           unit,
                           _bcm_dpp_vlan_unit_state[unit]->wb.portsInPbmp,
                           desc,
                           index));
            } /* if (BCM_PBMP_MEMBER((*bitmap), index)) */
        } /* if (index is within backing store PBMP) */
    } /* for (index = 0; index < _SHR_PBMP_PORT_MAX; index++) */
}

/*
 *  Function
 *    _bcm_dpp_vlan_wb_pbmp_put
 *  Purpose
 *    Write a PBMP to backing store
 *  Arguments
 *    (IN) unit = unit ID
 *    (IN) ptr = where to read bits
 *    (OUT) bitmap = pointer to bitmap to read
 *    (IN) desc = description of this bitmap
 *  Results
 *    (none)
 *  Notes
 *    Assumes backing store bitmap was all zeroes before call
 */
STATIC int
_bcm_dpp_vlan_wb_pbmp_get(int unit,
                          uint32 *ptr,
                          bcm_pbmp_t *bitmap,
                          const char *desc)
{
    unsigned int index;
    BCMDNX_INIT_FUNC_DEFS;

    for (index = 0;
         index < _bcm_dpp_vlan_unit_state[unit]->wb.portsInPbmp;
         index++) {
        if (index < _SHR_PBMP_PORT_MAX) {
            if (ptr[index >> 5] & (1 << (index & 0x1F))) {
                BCM_PBMP_PORT_ADD(*bitmap, index);
            } else {
                BCM_PBMP_PORT_REMOVE(*bitmap, index);
            }
        } else { /* if (BCM_PBMP_MEMBER((*bitmap), index)) */
            if (ptr[index >> 5] & (1 << (index & 0x1F))) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                                 (_BSL_BCM_MSG("working code supports %u %s, but"
                                           " backing store has bit %u set"),
                                  _SHR_PBMP_PORT_MAX,
                                  desc,
                                  index));
            } /* if (ptr[index >> 5] |= (1 << (index & 0x1F))) */
        } /* if (index < _SHR_PBMP_PORT_MAX) */
    } /* for (index = 0; index < _SHR_PBMP_PORT_MAX; index++) */
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *    _bcm_dpp_vlan_wb_bmp_put
 *  Purpose
 *    Write a SHR_BIT based bitmap to backing store
 *  Arguments
 *    (IN) unit = unit ID
 *    (IN) bitmap = pointer to bitmap to write
 *    (IN) currBits = bits in bitmap
 *    (OUT) ptr = where to write bits
 *    (IN) backBits = bits in backing store for this bitmap
 *    (IN) desc = description of this bitmap
 *  Results
 *    (none)
 *  Notes
 *    Assumes backing store bitmap was all zeroes before call
 */
STATIC void
_bcm_dpp_vlan_wb_bmp_put(int unit,
                         SHR_BITDCL *bitmap,
                         unsigned int currBits,
                         uint32 *ptr,
                         unsigned int backBits,
                         const char *desc)
{
    unsigned int index;
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
                                      " %u %s, but it has bit %u set\n"),
                           unit,
                           backBits,
                           desc,
                           index));
            } /* if (SHR_BITGET(bitmap, index)) */
        } /* if (index < backBits) */
    } /* for (index = 0; index < currBits; index++) */
}

/*
 *  Function
 *    _bcm_dpp_vlan_wb_bmp_get
 *  Purpose
 *    Write a SHR_BIT based bitmap to backing store
 *  Arguments
 *    (IN) unit = unit ID
 *    (IN) ptr = where to read bits
 *    (IN) backBits = bits in backing store for this bitmap
 *    (OUT) bitmap = pointer to bitmap to read
 *    (IN) currBits = bits in bitmap
 *    (IN) desc = description of this bitmap
 *  Results
 *    (none)
 *  Notes
 *    Assumes backing store bitmap was all zeroes before call
 */
STATIC int
_bcm_dpp_vlan_wb_bmp_get(int unit,
                         uint32 *ptr,
                         unsigned int backBits,
                         SHR_BITDCL *bitmap,
                         unsigned int currBits,
                         const char *desc)
{
    unsigned int index;
    BCMDNX_INIT_FUNC_DEFS;

    for (index = 0; index < backBits; index++) {
        if (index < currBits) {
            if (ptr[index >> 5] & (1 << (index & 0x1F))) {
                SHR_BITSET(bitmap, index);
            } else {
                SHR_BITCLR(bitmap, index);
            }
        } else {
            if (ptr[index >> 5] & (1 << (index & 0x1F))) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                                 (_BSL_BCM_MSG("working code supports %u %s,"
                                           " but backing store has bit %u set"),
                                  currBits,
                                  desc,
                                  index));
            } /* if (ptr[index >> 5] & (1 << (index & 0x1F))) */
        } /* if (index < currBits) */
    } /* for (index = 0; index < backBits; index++) */
exit:
    BCMDNX_FUNC_RETURN;
}

void
_bcm_dpp_vlan_fid_ref_count_save(int unit,
                                 unsigned int index,
                                 uint8 *data,
                                 unsigned int *size)
{
    _bcm_dpp_wb_1_0_vlan_fid_ref_count_t *wbData = NULL;
    unsigned int wbSize;
    unsigned int wbOffset;
    int result;

    if (0 == _bcm_dpp_vlan_unit_state[unit]->wb.offsetFidRefs) {
        LOG_ERROR(BSL_LS_BCM_INIT,
                  (BSL_META_U(unit,
                              "unit %d FID reference is not supported in"
                              " backing store\n"),
                   unit));
        return;
    }
    if (index >= _bcm_dpp_vlan_unit_state[unit]->wb.sharedFids) {
        LOG_ERROR(BSL_LS_BCM_INIT,
                  (BSL_META_U(unit,
                              "unit %d FID reference %u is not supported"
                              " in backing store (max %u)\n"),
                   unit,
                   index,
                   _bcm_dpp_vlan_unit_state[unit]->wb.sharedFids));
        return;
    }
#if _BCM_DPP_VLAN_WARMBOOT_WRITE_TRACKING
    if (!size) {
        LOG_DEBUG(BSL_LS_BCM_INIT,
                  (BSL_META_U(unit,
                              "unit %d writing FID ref count %u to"
                              " backing store\n"),
                   unit,
                   index));
    }
#endif /* _BCM_DPP_VLAN_WARMBOOT_WRITE_TRACKING */
    switch (_bcm_dpp_vlan_unit_state[unit]->wb.version) {
    case _BCM_DPP_WB_VLAN_VERSION_1_0:
        wbSize = sizeof(*wbData);
        wbSize = _BCM_DPP_WB_VLAN_ALIGNED_SIZE(wbSize);
        if (size) {
            *size = wbSize;
        } else { /* if (size) */
            wbOffset = (_bcm_dpp_vlan_unit_state[unit]->wb.offsetFidRefs +
                      (wbSize * index));
            wbData = sal_alloc(wbSize,
                               "FID reference warm boot temp space");
            if (wbData) {
                sal_memset(wbData, 0x00, wbSize);
                wbData->fid = _bcm_dpp_vlan_unit_state[unit]->fid_ref_count[index].fid;
                wbData->refCount = _bcm_dpp_vlan_unit_state[unit]->fid_ref_count[index].ref_count;
                if (data) {
                    sal_memcpy(&(data[wbOffset]),
                               wbData,
                               wbSize);
                } else { /* if (data) */
                    sal_memcpy(&(_bcm_dpp_vlan_unit_state[unit]->wb.buffer[wbOffset]),
                               wbData,
                               wbSize);
                    if (!_bcm_dpp_switch_is_immediate_sync(unit) && !wb_vlan_is_scheduled_sync[unit]) {
                            /* mark dirty state. Currently no other processing done           */
                            /* note that there is no in module dirty bit optimization. use global dirty bit */
                            BCM_DPP_WB_DEV_VLAN_DIRTY_BIT_SET(unit);								        
                    }
                    else{
                         result = soc_scache_commit_specific_data(unit,
                                                                  _bcm_dpp_vlan_unit_state[unit]->wb.handle,
                                                                  wbSize,
                                                                  (uint8*)wbData,
                                                                  wbOffset);
                         if (SOC_E_NONE != result) {
                             LOG_ERROR(BSL_LS_BCM_INIT,
                                       (BSL_META_U(unit,
                                                   "unit %d failed to commit VLAN FID"
                                                   " reference %u to backing store"
                                                   " for %u bytes at %u: %d (%s)\n"),
                                        unit,
                                        index,
                                        wbSize,
                                        wbOffset,
                                        result,
                                        _SHR_ERRMSG(result)));
                         }
                    }
                } /* if (data) */
            } else { /* if (wbData) */
                LOG_ERROR(BSL_LS_BCM_INIT,
                          (BSL_META_U(unit,
                                      "unit %d failed to allocate %u bytes"
                                      " for FID reference warm boot temp space\n"),
                           unit,
                           (uint32)sizeof(*wbData)));
            } /* if (wbData) */
        } /* if (size) */
        break;
    default:
        LOG_ERROR(BSL_LS_BCM_INIT,
                  (BSL_META_U(unit,
                              "unit %d unexpected warm boot version %04X\n"),
                   unit,
                   _bcm_dpp_vlan_unit_state[unit]->wb.version));
        if (size) {
            *size = 0;
        }
    } /* switch (_bcm_dpp_vlan_unit_state[unit]->wb.version) */
    if (wbData) {
        sal_free(wbData);
    }
}

/*
 *  Function
 *    _bcm_dpp_vlan_fid_ref_count_load_all
 *  Purpose
 *    Load all VSI LIF index table entries
 *  Arguments
 *    (IN) unit = unit ID
 *  Results
 *    (none)
 *  Notes
 *    Assumes working space is all zeroes before call
 */
STATIC int
_bcm_dpp_vlan_fid_ref_count_load_all(int unit)
{
    _bcm_dpp_wb_1_0_vlan_fid_ref_count_t *wbData;
    unsigned int wbSize;
    unsigned int wbOffset;
    unsigned int index;
    BCMDNX_INIT_FUNC_DEFS;

    if (0 == _bcm_dpp_vlan_unit_state[unit]->wb.offsetFidRefs) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                         (_BSL_BCM_MSG("FID reference is not supported in"
                            " backing store")));
    }
#if _BCM_DPP_VLAN_WARMBOOT_READ_TRACKING
    LOG_DEBUG(BSL_LS_BCM_INIT,
              (BSL_META_U(unit,
                          "unit %d reading FID ref counts from"
                          " backing store\n"),
               unit));
#endif /* _BCM_DPP_VLAN_WARMBOOT_WRITE_TRACKING */
    switch (_bcm_dpp_vlan_unit_state[unit]->wb.version) {
    case _BCM_DPP_WB_VLAN_VERSION_1_0:
        wbSize = sizeof(*wbData);
        wbSize = _BCM_DPP_WB_VLAN_ALIGNED_SIZE(wbSize);
        for (index = 0;
             index < _bcm_dpp_vlan_unit_state[unit]->wb.sharedFids;
             index++) {
            if (DPP_NOF_SHARED_FIDS <= index) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                                 (_BSL_BCM_MSG("VSI LIF index %u..%u in backing"
                                           " store not valid (max %u)"),
                                  index,
                                  0,
                                  BCM_DPP_NOF_BRIDGE_VLANS - 1));
            }
            wbOffset = (_bcm_dpp_vlan_unit_state[unit]->wb.offsetFidRefs +
                        (wbSize * index));
            wbData = (_bcm_dpp_wb_1_0_vlan_fid_ref_count_t*)&(_bcm_dpp_vlan_unit_state[unit]->wb.buffer[wbOffset]);
            _bcm_dpp_vlan_unit_state[unit]->fid_ref_count[index].fid = wbData->fid;
            _bcm_dpp_vlan_unit_state[unit]->fid_ref_count[index].ref_count = wbData->refCount;
        }
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                         (_BSL_BCM_MSG("unexpected warm boot version %04X"),
                          _bcm_dpp_vlan_unit_state[unit]->wb.version));
    } /* switch (_bcm_dpp_vlan_unit_state[unit]->wb.version) */
exit:
    BCMDNX_FUNC_RETURN;
}

void
_bcm_dpp_vlan_info_wb_save_all(int unit,
                               uint8 *data,
                               unsigned int *size)
{
    _bcm_dpp_wb_1_0_vlan_info_t *wbData = NULL;
    unsigned int wbSize;
    unsigned int wbOffset;
    unsigned int pbmpSize;
    unsigned int vlanSize;
    int result;

    if (0 == _bcm_dpp_vlan_unit_state[unit]->wb.offsetVlanInfo) {
        LOG_ERROR(BSL_LS_BCM_INIT,
                  (BSL_META_U(unit,
                              "unit %d VLAN info is not supported in"
                              " backing store\n"),
                   unit));
        return;
    }
#if _BCM_DPP_VLAN_WARMBOOT_WRITE_TRACKING
    if (!size) {
        LOG_DEBUG(BSL_LS_BCM_INIT,
                  (BSL_META_U(unit,
                              "unit %d writing VLAN info to"
                              " backing store\n"),
                   unit));
    }
#endif /* _BCM_DPP_VLAN_WARMBOOT_WRITE_TRACKING */
    switch (_bcm_dpp_vlan_unit_state[unit]->wb.version) {
    case _BCM_DPP_WB_VLAN_VERSION_1_0:
        pbmpSize = ((_bcm_dpp_vlan_unit_state[unit]->wb.portsInPbmp + 31) >> 5);
        vlanSize = ((_bcm_dpp_vlan_unit_state[unit]->wb.vlanCount + 31) >> 5);
        wbSize = ((sizeof(*wbData) - sizeof(uint32)) +
                  (((pbmpSize * 2) + (vlanSize)) * sizeof(uint32)));
        wbSize = _BCM_DPP_WB_VLAN_ALIGNED_SIZE(wbSize);
        if (size) {
            *size = wbSize;
        } else { /* if (size) */
            wbOffset = _bcm_dpp_vlan_unit_state[unit]->wb.offsetVlanInfo;
            wbData = sal_alloc(wbSize,
                               "VLAN information warm boot temp space");
            if (wbData) {
                sal_memset(wbData, 0x00, wbSize);
                wbData->count = _bcm_dpp_vlan_unit_state[unit]->vlan_info.count;
                wbData->defl = _bcm_dpp_vlan_unit_state[unit]->vlan_info.defl;
                if (_bcm_dpp_vlan_unit_state[unit]->vlan_info.init) {
                    wbData->flags |= 0x00000001;
                }
                _bcm_dpp_vlan_wb_pbmp_put(unit,
                                          &(_bcm_dpp_vlan_unit_state[unit]->vlan_info.defl_pbmp),
                                          &(wbData->bitmaps[0]),
                                          "default VLAN port bitmap");
                _bcm_dpp_vlan_wb_pbmp_put(unit,
                                          &(_bcm_dpp_vlan_unit_state[unit]->vlan_info.defl_ubmp),
                                          &(wbData->bitmaps[pbmpSize]),
                                          "default VLAN untagged port bitmap");
                _bcm_dpp_vlan_wb_bmp_put(unit,
                                         &(_bcm_dpp_vlan_unit_state[unit]->vlan_info.vlan_bmp[0]),
                                         BCM_VLAN_INVALID,
                                         &(wbData->bitmaps[pbmpSize * 2]),
                                         _bcm_dpp_vlan_unit_state[unit]->wb.vlanCount,
                                         "exiting VLANs bitmap");
                if (data) {
                    sal_memcpy(&(data[wbOffset]),
                               wbData,
                               wbSize);
                } else { /* if (data) */
                    sal_memcpy(&(_bcm_dpp_vlan_unit_state[unit]->wb.buffer[wbOffset]),
                               wbData,
                               wbSize);
                    if (!_bcm_dpp_switch_is_immediate_sync(unit) && !wb_vlan_is_scheduled_sync[unit]) {
                            /* mark dirty state. Currently no other processing done           */
                            /* note that there is no in module dirty bit optimization. use global dirty bit */
                            BCM_DPP_WB_DEV_VLAN_DIRTY_BIT_SET(unit);								        
                    }
                    else{
                         result = soc_scache_commit_specific_data(unit,
                                                                  _bcm_dpp_vlan_unit_state[unit]->wb.handle,
                                                                  wbSize,
                                                                  (uint8*)wbData,
                                                                  wbOffset);
                         if (SOC_E_NONE != result) {
                             LOG_ERROR(BSL_LS_BCM_INIT,
                                       (BSL_META_U(unit,
                                                   "unit %d failed to commit VLAN"
                                                   " state to backing store"
                                                   " for %u bytes at %u: %d (%s)\n"),
                                        unit,
                                        wbSize,
                                        wbOffset,
                                        result,
                                        _SHR_ERRMSG(result)));
                         }
                    }
                } /* if (data) */
            } else { /* if (wbData) */
                LOG_ERROR(BSL_LS_BCM_INIT,
                          (BSL_META_U(unit,
                                      "unit %d failed to allocate %u bytes"
                                      " for FID reference warm boot temp space\n"),
                           unit,
                           (uint32)sizeof(*wbData)));
            } /* if (wbData) */
        } /* if (size) */
        break;
    default:
        LOG_ERROR(BSL_LS_BCM_INIT,
                  (BSL_META_U(unit,
                              "unit %d unexpected warm boot version %04X\n"),
                   unit,
                   _bcm_dpp_vlan_unit_state[unit]->wb.version));
        if (size) {
            *size = 0;
        }
    } /* switch (_bcm_dpp_vlan_unit_state[unit]->wb.version) */
    if (wbData) {
        sal_free(wbData);
    }
}

/*
 *  Function
 *    _bcm_dpp_vlan_info_wb_load_all
 *  Purpose
 *    Load all of the vlan info structure
 *  Arguments
 *    (IN) unit = unit ID
 *  Results
 *    (none)
 *  Notes
 *    Assumes working space is all zeroes before call
 */
STATIC int
_bcm_dpp_vlan_info_wb_load_all(int unit)
{
    _bcm_dpp_wb_1_0_vlan_info_t *wbData;
    unsigned int wbSize;
    unsigned int pbmpSize;
    unsigned int vlanSize;
    BCMDNX_INIT_FUNC_DEFS;

    if (0 == _bcm_dpp_vlan_unit_state[unit]->wb.offsetVlanInfo) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                         (_BSL_BCM_MSG("VLAN info is not supported in"
                            " backing store")));
    }
#if _BCM_DPP_VLAN_WARMBOOT_READ_TRACKING
    LOG_DEBUG(BSL_LS_BCM_INIT,
              (BSL_META_U(unit,
                          "unit %d reading VLAN info from"
                          " backing store\n"),
               unit));
#endif /* _BCM_DPP_VLAN_WARMBOOT_WRITE_TRACKING */
    switch (_bcm_dpp_vlan_unit_state[unit]->wb.version) {
    case _BCM_DPP_WB_VLAN_VERSION_1_0:
        wbSize = sizeof(wbData);
        pbmpSize = ((_bcm_dpp_vlan_unit_state[unit]->wb.portsInPbmp + 31) >> 5);
        vlanSize = ((_bcm_dpp_vlan_unit_state[unit]->wb.vlanCount + 31) >> 5);
        wbSize = ((sizeof(*wbData) - sizeof(uint32)) +
                  (((pbmpSize * 2) + (vlanSize)) * sizeof(uint32)));
        wbSize = _BCM_DPP_WB_VLAN_ALIGNED_SIZE(wbSize);
        wbData = (_bcm_dpp_wb_1_0_vlan_info_t*)&(_bcm_dpp_vlan_unit_state[unit]->wb.buffer[_bcm_dpp_vlan_unit_state[unit]->wb.offsetVlanInfo]);
        _bcm_dpp_vlan_unit_state[unit]->vlan_info.count = wbData->count;
        _bcm_dpp_vlan_unit_state[unit]->vlan_info.defl = wbData->defl;
        if (wbData->flags & 0x00000001) {
            _bcm_dpp_vlan_unit_state[unit]->vlan_info.init = TRUE;
        } else {
            _bcm_dpp_vlan_unit_state[unit]->vlan_info.init = FALSE;
        }
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_vlan_wb_pbmp_get(unit,
                                                  &(wbData->bitmaps[0]),
                                                  &(_bcm_dpp_vlan_unit_state[unit]->vlan_info.defl_pbmp),
                                                  "default VLAN port bitmap"));
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_vlan_wb_pbmp_get(unit,
                                                  &(wbData->bitmaps[pbmpSize]),
                                                  &(_bcm_dpp_vlan_unit_state[unit]->vlan_info.defl_ubmp),
                                                  "default VLAN untagged port bitmap"));
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_vlan_wb_bmp_get(unit,
                                                 &(wbData->bitmaps[pbmpSize * 2]),
                                                 _bcm_dpp_vlan_unit_state[unit]->wb.vlanCount,
                                                 &(_bcm_dpp_vlan_unit_state[unit]->vlan_info.vlan_bmp[0]),
                                                 BCM_VLAN_INVALID,
                                                 "existing VLANs bitmap"));
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                         (_BSL_BCM_MSG("unexpected warm boot version %04X"),
                          _bcm_dpp_vlan_unit_state[unit]->wb.version));
    } /* switch (_bcm_dpp_vlan_unit_state[unit]->wb.version) */
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *    _bcm_dpp_vlan_param_wb_save
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
_bcm_dpp_vlan_param_wb_save(int unit,
                            uint8 *data,
                            unsigned int *size)
{
    _bcm_dpp_wb_1_0_vlan_param_t *wbData;
    unsigned int wbSize;
    int result;

#if _BCM_DPP_VLAN_WARMBOOT_WRITE_TRACKING
    if (!size) {
        LOG_DEBUG(BSL_LS_BCM_INIT,
                  (BSL_META_U(unit,
                              "unit %d writing VLAN parameters to"
                              " backing store\n"),
                   unit));
    }
#endif /* _BCM_DPP_VLAN_WARMBOOT_WRITE_TRACKING */
    switch (_bcm_dpp_vlan_unit_state[unit]->wb.version) {
    case _BCM_DPP_WB_VLAN_VERSION_1_0:
        wbSize = sizeof(*wbData);
        wbSize = _BCM_DPP_WB_VLAN_ALIGNED_SIZE(wbSize);
        if (size) {
            *size = wbSize;
        } else { /* if (size) */
            wbData = sal_alloc(wbSize, "param backing store temp");
            if (wbData) {
                sal_memset(wbData, 0x00, wbSize);
                wbData->offsetVlanInfo = _bcm_dpp_vlan_unit_state[unit]->wb.offsetVlanInfo;
                wbData->offsetFidRef = _bcm_dpp_vlan_unit_state[unit]->wb.offsetFidRefs;
                wbData->limitVlan = _bcm_dpp_vlan_unit_state[unit]->wb.vlanCount;
                wbData->limitSharedFid = _bcm_dpp_vlan_unit_state[unit]->wb.sharedFids;
                wbData->limitPortsInPbmp = _bcm_dpp_vlan_unit_state[unit]->wb.portsInPbmp;
                if (data) {
                    sal_memcpy(&(data[0]),
                               wbData,
                               wbSize);
                } else { /* if (data) */
                    sal_memcpy(&(_bcm_dpp_vlan_unit_state[unit]->wb.buffer[0]),
                               wbData,
                               wbSize);
                    if (!_bcm_dpp_switch_is_immediate_sync(unit) && !wb_vlan_is_scheduled_sync[unit]) {
                            /* mark dirty state. Currently no other processing done           */
                            /* note that there is no in module dirty bit optimization. use global dirty bit */
                            BCM_DPP_WB_DEV_VLAN_DIRTY_BIT_SET(unit);								        
                    }
                    else{
                         result = soc_scache_commit_specific_data(unit,
                                                                  _bcm_dpp_vlan_unit_state[unit]->wb.handle,
                                                                  wbSize,
                                                                  (uint8*)wbData,
                                                                  0);
                         if (SOC_E_NONE != result) {
                             if (SOC_E_NONE != result) {
                                 LOG_ERROR(BSL_LS_BCM_INIT,
                                           (BSL_META_U(unit,
                                                       "unit %d failed to commit"
                                                       " VLAN state to backing store"
                                                       " for %u bytes at %u:"
                                                       " %d (%s)\n"),
                                            unit,
                                            wbSize,
                                            0,
                                            result,
                                            _SHR_ERRMSG(result)));
                             }
                         }
                    }
                } /* if (data) */
                sal_free(wbData);
            } else { /* if (wbData) */
                LOG_ERROR(BSL_LS_BCM_INIT,
                          (BSL_META_U(unit,
                                      "unit %d unable to allocate temporary"
                                      " space to update backing store for"
                                      " VLAN unit info"),
                           unit));
            } /* if (wbData) */
        } /* if (size) */
        break;
    default:
        LOG_ERROR(BSL_LS_BCM_INIT,
                  (BSL_META_U(unit,
                              "unit %d unexpected warm boot version %04X\n"),
                   unit,
                   _bcm_dpp_vlan_unit_state[unit]->wb.version));
        if (size) {
            *size = 0;
        }
    } /* switch (unitData->wb_version) */
}

/*
 *  Function
 *    _bcm_dpp_vlan_all_wb_state_sync_int
 *  Purpose
 *    Sync the entire VLAN state to the warm boot buffer
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
void
_bcm_dpp_vlan_all_wb_state_sync_int(int unit,
                                    uint8 *data,
                                    unsigned int *size)
{
    uint8 *wbData;
    unsigned int index;
    int result;

#if _BCM_DPP_VLAN_WARMBOOT_WRITE_TRACKING
    if (!size) {
        LOG_DEBUG(BSL_LS_BCM_INIT,
                  (BSL_META_U(unit,
                              "unit %d writing full VLAN state to"
                              " backing store\n"),
                   unit));
    }
#endif /* _BCM_DPP_VLAN_WARMBOOT_WRITE_TRACKING */
    switch (_bcm_dpp_vlan_unit_state[unit]->wb.version) {
    case _BCM_DPP_WB_VLAN_VERSION_1_0:
        /* we need total size either way */
        if (size) {
            /* caller wants total size */
            *size = _bcm_dpp_vlan_unit_state[unit]->wb.totalSize;
        } else { /* if (size) */
            /* caller wants state sync */
            if (data) {
                /* caller provided a buffer; write to it */
                wbData = data;
            } else {
                /*
                 *  Caller did not provide a buffer, so write to the backing
                 *  store, but we don't want so many individual writes, so
                 *  collect all of the writes and then update the whole thing.
                 */
                wbData = _bcm_dpp_vlan_unit_state[unit]->wb.buffer;
            }
            _bcm_dpp_vlan_param_wb_save(unit, wbData, NULL);
            _bcm_dpp_vlan_info_wb_save_all(unit, wbData, NULL);
            for (index = 0;
                 index < _bcm_dpp_vlan_unit_state[unit]->wb.sharedFids;
                 index++) {
                _bcm_dpp_vlan_fid_ref_count_save(unit, index, wbData, NULL);
            }
            if (!data) {
                /*
                 *  Caller did not provide a buffer, so we wrote to the backing
                 *  store instead, but did not commit yet.  Commit now...
                 */
                 if (!_bcm_dpp_switch_is_immediate_sync(unit) && !wb_vlan_is_scheduled_sync[unit]) {
                         /* mark dirty state. Currently no other processing done           */
                         /* note that there is no in module dirty bit optimization. use global dirty bit */
                         BCM_DPP_WB_DEV_VLAN_DIRTY_BIT_SET(unit);								        
                 }
                 else{
                      result = soc_scache_commit_specific_data(unit,
                                                               _bcm_dpp_vlan_unit_state[unit]->wb.handle,
                                                               _bcm_dpp_vlan_unit_state[unit]->wb.totalSize,
                                                               (uint8*)wbData,
                                                               0 /* offset */);
                      if (SOC_E_NONE != result) {
                          LOG_ERROR(BSL_LS_BCM_INIT,
                                    (BSL_META_U(unit,
                                                "unit %d unable to perform backing"
                                                " store sync: %d (%s)\n"),
                                     unit,
                                     result,
                                     _SHR_ERRMSG(result)));
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
                   _bcm_dpp_vlan_unit_state[unit]->wb.version));
        if (size) {
            *size = 0;
        }
    } /* switch (_bcm_dpp_vlan_unit_state[unit]->wb.version) */
}

int
_bcm_dpp_vlan_all_wb_state_sync(int unit)
{
#ifdef SCACHE_CRC_CHECK
    int rv;
#endif
    BCMDNX_INIT_FUNC_DEFS;

    
    wb_vlan_is_scheduled_sync[unit] = 1;
    _bcm_dpp_vlan_all_wb_state_sync_int(unit, NULL, NULL);
    wb_vlan_is_scheduled_sync[unit] = 0;
    

#ifdef SCACHE_CRC_CHECK
    /* VLAN WB code currently use commit_specific_data, which invalidates the CRC.
       Current solution is to append the CRC here again */
    rv = soc_scache_hsbuf_crc32_append(unit, _bcm_dpp_vlan_unit_state[unit]->wb.handle);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
#endif
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *    _bcm_dpp_vlan_wb_layout_init
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
STATIC int
_bcm_dpp_vlan_wb_layout_init(int unit,
                             uint16 version,
                             soc_scache_handle_t handle,
                             unsigned int *totalSize)
{
    unsigned int wbSize;
    unsigned int wbOffset;
    BCMDNX_INIT_FUNC_DEFS;

#if _BCM_DPP_VLAN_WARMBOOT_WRITE_TRACKING
    LOG_DEBUG(BSL_LS_BCM_INIT,
              (BSL_META_U(unit,
                          "unit %d preparing VLAN backing store parameters\n"),
               unit));
#endif /* _BCM_DPP_VLAN_WARMBOOT_WRITE_TRACKING */
    sal_memset(&(_bcm_dpp_vlan_unit_state[unit]->wb),
               0x00,
               sizeof(_bcm_dpp_vlan_unit_state[unit]->wb));
    wbOffset = 0;
    switch (version) {
    case _BCM_DPP_WB_VLAN_VERSION_1_0:
        /* basic parameters */
        _bcm_dpp_vlan_unit_state[unit]->wb.handle = handle;
        _bcm_dpp_vlan_unit_state[unit]->wb.version = version;
        _bcm_dpp_vlan_unit_state[unit]->wb.vlanCount = BCM_VLAN_INVALID;
        _bcm_dpp_vlan_unit_state[unit]->wb.sharedFids = DPP_NOF_SHARED_FIDS;
        _bcm_dpp_vlan_unit_state[unit]->wb.portsInPbmp = _SHR_PBMP_PORT_MAX;
        /* compute offsets */
        _bcm_dpp_vlan_param_wb_save(unit, NULL, &wbSize);
        wbOffset += wbSize;
        _bcm_dpp_vlan_unit_state[unit]->wb.offsetVlanInfo = wbOffset;
        _bcm_dpp_vlan_info_wb_save_all(unit, NULL, &wbSize);
        wbOffset += wbSize;
        _bcm_dpp_vlan_unit_state[unit]->wb.offsetFidRefs = wbOffset;
        _bcm_dpp_vlan_fid_ref_count_save(unit, 0, NULL, &wbSize);
        wbOffset += (_bcm_dpp_vlan_unit_state[unit]->wb.sharedFids * wbSize);
        _bcm_dpp_vlan_unit_state[unit]->wb.totalSize = wbOffset;
        *totalSize = wbOffset;
        break;
    default:
        *totalSize = 0;
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                         (_BSL_BCM_MSG("unexpected warm boot version %04X\n"),
                          version));
    } /* switch (version) */
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *    _bcm_dpp_vlan_wb_layout_load
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
_bcm_dpp_vlan_wb_layout_load(int unit,
                             uint16 version,
                             soc_scache_handle_t handle,
                             uint8 *buffer)
{
    _bcm_dpp_wb_1_0_vlan_param_t *wbData;
    unsigned int wbSize;
    BCMDNX_INIT_FUNC_DEFS;

#if _BCM_DPP_VLAN_WARMBOOT_WRITE_TRACKING
    LOG_DEBUG(BSL_LS_BCM_INIT,
              (BSL_META_U(unit,
                          "unit %d loading VLAN backing store parameters\n"),
               unit));
#endif /* _BCM_DPP_VLAN_WARMBOOT_WRITE_TRACKING */
    sal_memset(&(_bcm_dpp_vlan_unit_state[unit]->wb),
               0x00,
               sizeof(_bcm_dpp_vlan_unit_state[unit]->wb));
    switch (version) {
    case _BCM_DPP_WB_VLAN_VERSION_1_0:
        wbData = (_bcm_dpp_wb_1_0_vlan_param_t*)&(buffer[0]);
        _bcm_dpp_vlan_unit_state[unit]->wb.buffer = buffer;
        _bcm_dpp_vlan_unit_state[unit]->wb.version = version;
        _bcm_dpp_vlan_unit_state[unit]->wb.handle = handle;
        _bcm_dpp_vlan_unit_state[unit]->wb.offsetVlanInfo = wbData->offsetVlanInfo;
        _bcm_dpp_vlan_unit_state[unit]->wb.offsetFidRefs = wbData->offsetFidRef;
        _bcm_dpp_vlan_unit_state[unit]->wb.vlanCount = wbData->limitVlan;
        _bcm_dpp_vlan_unit_state[unit]->wb.sharedFids = wbData->limitSharedFid;
        _bcm_dpp_vlan_unit_state[unit]->wb.portsInPbmp = wbData->limitPortsInPbmp;
        /* compute offsets */
        _bcm_dpp_vlan_unit_state[unit]->wb.totalSize = 0;
        _bcm_dpp_vlan_param_wb_save(unit, NULL, &wbSize);
        _bcm_dpp_vlan_unit_state[unit]->wb.totalSize += wbSize;
        _bcm_dpp_vlan_info_wb_save_all(unit, NULL, &wbSize);
        _bcm_dpp_vlan_unit_state[unit]->wb.totalSize += wbSize;
        _bcm_dpp_vlan_fid_ref_count_save(unit, 0, NULL, &wbSize);
        _bcm_dpp_vlan_unit_state[unit]->wb.totalSize += (_bcm_dpp_vlan_unit_state[unit]->wb.sharedFids * wbSize);
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                         (_BSL_BCM_MSG("unexpected warm boot version %04X\n"),
                          version));
    } /* switch (version) */
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_wb_vlan_state_init(int unit)
{
    int result = BCM_E_NONE;
    int flags = SOC_DPP_SCACHE_DEFAULT;
    int exists;
    uint16 version = _BCM_DPP_WB_VLAN_VERSION_CURR;
    uint16 oldVersion = 0;
    unsigned int size;
    soc_scache_handle_t handle;
    uint8 *buffer;
    BCMDNX_INIT_FUNC_DEFS;

    wb_vlan_is_scheduled_sync[unit] = 0;

    SOC_SCACHE_HANDLE_SET(handle, unit, BCM_MODULE_VLAN, 0);

    if (SOC_WARM_BOOT(unit)) {
        /* warm boot */
        size = 0;
        /* fetch the existing warm boot space */
        result = soc_dpp_scache_ptr_get(unit,
                                        handle,
                                        socDppScacheRetreive,
                                        flags,
                                        &size,
                                        &buffer,
                                        version,
                                        &oldVersion,
                                        &exists);
        if (BCM_E_NONE == result) {
            LOG_DEBUG(BSL_LS_BCM_INIT,
                      (BSL_META_U(unit,
                                  "unit %d loading VLAN backing store state\n"),
                       unit));
            BCMDNX_IF_ERR_EXIT(_bcm_dpp_vlan_wb_layout_load(unit,
                                                         version,
                                                         handle,
                                                         buffer));
            BCMDNX_IF_ERR_EXIT(_bcm_dpp_vlan_info_wb_load_all(unit));
            BCMDNX_IF_ERR_EXIT(_bcm_dpp_vlan_fid_ref_count_load_all(unit));
            if (BCM_E_NONE == result) {
                if (version != oldVersion) {
                    /* set up layout for the preferred version */
                    _bcm_dpp_vlan_wb_layout_init(unit,
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
                                              "unable to reallocate %d bytes"
                                              " warm boot space for unit %d"
                                              " VLAN instance: %d (%s)\n"),
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
                                  " unit %d VLAN instance: %d (%s)\n"),
                       unit,
                       result,
                       _SHR_ERRMSG(result)));
        } /* if (BCM_E_NONE == result) */
    } else {
        /* set up layout for the preferred version */
        _bcm_dpp_vlan_wb_layout_init(unit, version, handle, &size);
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
        _bcm_dpp_vlan_unit_state[unit]->wb.buffer = buffer;
        /* build an initial image in the buffer */
        _bcm_dpp_vlan_all_wb_state_sync_int(unit, buffer, NULL);
        BCM_DPP_WB_DEV_DIRTY_BIT_SET(unit);
    }
exit:
    BCMDNX_FUNC_RETURN;
}



#endif /* def BCM_WARM_BOOT_SUPPORT */

