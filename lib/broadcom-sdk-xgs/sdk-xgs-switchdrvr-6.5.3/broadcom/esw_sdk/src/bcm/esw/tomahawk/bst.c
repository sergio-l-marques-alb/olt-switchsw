/*
 * $Id: $
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
 * BST - Buffer Statistics Tracking
 * Purpose: API to set and manage various BST resources for TH
 *
 */

#include <shared/bsl.h>
#include <soc/defs.h>
#include <sal/core/libc.h>
#include <shared/alloc.h>
#include <sal/core/spl.h>
#include <sal/core/boot.h>

#include <soc/drv.h>
#include <soc/error.h>
#include <soc/ll.h>
#include <soc/debug.h>
#include <soc/mem.h>
#include <soc/profile_mem.h>
#include <soc/tomahawk.h>

#include <bcm/switch.h>
#include <bcm/types.h>
#include <bcm/cosq.h>
#include <bcm_int/bst.h>
#include <bcm_int/esw/port.h>

#if defined(BCM_TOMAHAWK_SUPPORT)
extern int _bcm_bst_th_sync_hw_snapshot(int unit, bcm_bst_stat_id_t bid,
                                        int port, int index);
extern int _bcm_bst_th_hw_stat_clear(int unit,
                                     _bcm_bst_resource_info_t *resInfo,
                                     bcm_bst_stat_id_t bid, int port,
                                     int index);

STATIC int
_bcm_bst_th_byte_to_cell(int unit, uint32 bytes)
{
    return (bytes + (208 - 1))/208;
}

STATIC int
_bcm_bst_th_cell_to_byte(int unit, uint32 cells)
{
    return cells * 208;
}

STATIC int
_bcm_th_bst_control_set(int unit, bcm_switch_control_t type, int arg,
                        _bcm_bst_thread_oper_t operation)
{
    uint32 rval, rval_en, rval_action, i, fval, enable = 0;
    uint32 hdrm_en = 0;
    _bcm_bst_cmn_unit_info_t *bst_info;
    _bcm_bst_resource_info_t *pres = NULL;
    soc_reg_t stat_reg = INVALIDr;
    soc_field_t stat_field = INVALIDf;
    soc_field_t snap_en[] = {BST_HW_SNAPSHOT_EN_THDOf,
                             BST_HW_SNAPSHOT_EN_THDIf,
                             BST_HW_SNAPSHOT_EN_CFAPf
                            };
    soc_field_t snap_action[] = {ACTION_EN_THDOf,
                                 ACTION_EN_THDIf,
                                 ACTION_EN_CFAPf
                                };

    bst_info = _BCM_UNIT_BST_INFO(unit);

    if (!bst_info) {
        return BCM_E_INIT;
    }

    switch (type) {
    case bcmSwitchBstEnable:
        BCM_IF_ERROR_RETURN(READ_MMU_GCFG_BST_TRACKING_ENABLEr(unit, &rval));

        fval = arg ? 0xf : 0;

        soc_reg_field_set(unit, MMU_GCFG_BST_TRACKING_ENABLEr, &rval,
                          BST_TRACK_EN_THDOf, fval);
        soc_reg_field_set(unit, MMU_GCFG_BST_TRACKING_ENABLEr, &rval,
                          BST_TRACK_EN_THDIf, fval);
        soc_reg_field_set(unit, MMU_GCFG_BST_TRACKING_ENABLEr, &rval,
                          BST_TRACK_EN_CFAPf, fval);

        BCM_IF_ERROR_RETURN(WRITE_MMU_GCFG_BST_TRACKING_ENABLEr(unit, rval));

        /* Globally Enable/Disable Headroom Pool Monitoring for all
           headroom pools */
        hdrm_en = arg ? 0xf : 0;
        for (i = 0; i < NUM_LAYER(unit); i++) {
            BCM_IF_ERROR_RETURN
                (soc_reg32_set(unit, THDI_HDRM_POOL_CFGr, i, 0, hdrm_en));
        }

        if (operation == _bcmBstThreadOperExternal) {
            if (fval) {
                BCM_IF_ERROR_RETURN(_bcm_bst_sync_thread_start(unit));
            } else {
                BCM_IF_ERROR_RETURN(_bcm_bst_sync_thread_stop(unit));
            }
        } else {
            /* Set Enable Flag to True/False, to Run/Pause the Thread
             * respectively. Wont Kill the thread.
             */
            _BCM_BST_SYNC_THREAD_ENABLE_SET(unit, fval ? TRUE : FALSE);
        }

        break;
    case bcmSwitchBstTrackingMode:
        BCM_IF_ERROR_RETURN(READ_MMU_GCFG_MISCCONFIGr(unit, &rval));
        soc_reg_field_set(unit, MMU_GCFG_MISCCONFIGr, &rval, BST_TRACKING_MODEf, arg);
        BCM_IF_ERROR_RETURN(WRITE_MMU_GCFG_MISCCONFIGr(unit, rval));
        bst_info->track_mode = arg ? 1 : 0;
        /*
         * As Headroom Pool has two different stats resource
         * for tracking current usage count and peak usage count
         * we set up the respective resource accordingly based on
         * BST Tracking Mode
         */
        if (bst_info->track_mode) {
            /* Peak Mode */
            stat_reg = THDI_HDRM_POOL_PEAK_COUNT_HPr;
            stat_field = PEAK_BUFFER_COUNTf;
        } else {
            /* Current Mode */
            stat_reg = THDI_HDRM_POOL_COUNT_HPr;
            stat_field = TOTAL_BUFFER_COUNTf;
        }
        pres = _BCM_BST_RESOURCE(unit, bcmBstStatIdHeadroomPool);
        if (pres != NULL) {
            _BCM_BST_STAT_INFO(pres, pres->stat_mem[0], stat_reg,
                               stat_field);
        }
        break;
    case bcmSwitchBstSnapshotEnable:
        rval_en = 0;
        rval_action = 0;
        if (arg != 0) {
            BCM_IF_ERROR_RETURN(
                READ_MMU_GCFG_BST_HW_SNAPSHOT_ENr(unit, &rval_en));
            BCM_IF_ERROR_RETURN(
                READ_MMU_GCFG_BST_SNAPSHOT_ACTION_ENr(unit, &rval_action));

            for (i = _BCM_BST_SNAPSHOT_THDO; i < _BCM_BST_SNAPSHOT_COUNT; i++) {
                enable = 0;
                if (arg & (0x1 << i)) {
                    enable = 0xf;
                } else {
                    enable = 0;
                }

                soc_reg_field_set(unit, MMU_GCFG_BST_HW_SNAPSHOT_ENr, &rval_en,
                                  snap_en[i], enable);

                soc_reg_field_set(unit, MMU_GCFG_BST_SNAPSHOT_ACTION_ENr,
                                  &rval_action, snap_action[i], ((enable) ? 1 : 0));
            }
        }
        BCM_IF_ERROR_RETURN(
            WRITE_MMU_GCFG_BST_HW_SNAPSHOT_ENr(unit, rval_en));
        BCM_IF_ERROR_RETURN(
            WRITE_MMU_GCFG_BST_SNAPSHOT_ACTION_ENr(unit, rval_action));
        bst_info->snapshot_mode = arg;

        break;

    default:
        return BCM_E_UNAVAIL;
    }
    return BCM_E_NONE;
}

STATIC int
_bcm_th_bst_control_get(int unit, bcm_switch_control_t type, int *arg)
{
    uint32 rval;
    _bcm_bst_cmn_unit_info_t *bst_info;
    int hdrm_en, cfap_en;

    bst_info = _BCM_UNIT_BST_INFO(unit);

    if (!bst_info) {
        return BCM_E_INIT;
    }

    switch (type) {
    case bcmSwitchBstEnable:
        BCM_IF_ERROR_RETURN(READ_MMU_GCFG_BST_TRACKING_ENABLEr(unit, &rval));
        cfap_en = soc_reg_field_get(unit, MMU_GCFG_BST_TRACKING_ENABLEr, rval,
                                    BST_TRACK_EN_CFAPf);
        BCM_IF_ERROR_RETURN
            (soc_reg32_get(unit, THDI_HDRM_POOL_CFGr, 0, 0, &rval));
        hdrm_en = soc_reg_field_get(unit, THDI_HDRM_POOL_CFGr, rval,
                                    PEAK_COUNT_UPDATE_EN_HPf);

        if ((cfap_en == 0xf) &&
            (hdrm_en == 0xf)) {
            /* Only when both cfap and
               headroom tracking is enabled */
            *arg = 1;
        } else {
            *arg = 0;
        }
        break;
    case bcmSwitchBstTrackingMode:
        *arg = bst_info->track_mode;
        break;
    case bcmSwitchBstSnapshotEnable:
        *arg = bst_info->snapshot_mode;
        break;
    default:
        return BCM_E_UNAVAIL;
    }
    return BCM_E_NONE;
}

STATIC int
_bcm_bst_th_intr_enable_set(int unit, int enable)
{
    uint32 rval;
    if (!soc_property_get(unit, spn_POLLED_IRQ_MODE, 0)) {
        if (SOC_REG_IS_VALID(unit, MMU_XCFG_XPE_CPU_INT_ENr)) {
            BCM_IF_ERROR_RETURN(READ_MMU_XCFG_XPE_CPU_INT_ENr(unit, &rval));
            soc_reg_field_set(unit, MMU_XCFG_XPE_CPU_INT_ENr, &rval,
                              BST_THDO_INT_ENf, enable);
            soc_reg_field_set(unit, MMU_XCFG_XPE_CPU_INT_ENr, &rval,
                              BST_THDI_INT_ENf, enable);
            BCM_IF_ERROR_RETURN(WRITE_MMU_XCFG_XPE_CPU_INT_ENr(unit, rval));
        }

        if (SOC_REG_IS_VALID(unit, MMU_SCFG_SC_CPU_INT_ENr)) {
            BCM_IF_ERROR_RETURN(READ_MMU_SCFG_SC_CPU_INT_ENr(unit, &rval));
            soc_reg_field_set(unit, MMU_SCFG_SC_CPU_INT_ENr, &rval,
                              BST_CFAP_B_INT_ENf, enable);
            soc_reg_field_set(unit, MMU_SCFG_SC_CPU_INT_ENr, &rval,
                              BST_CFAP_A_INT_ENf, enable);
            BCM_IF_ERROR_RETURN(WRITE_MMU_SCFG_SC_CPU_INT_ENr(unit, rval));
        }
    }
    return BCM_E_NONE;
}

STATIC int
soc_th_set_hw_intr_cb(int unit)
{
    BCM_IF_ERROR_RETURN(soc_th_set_bst_callback(unit, &_bcm_bst_hw_event_cb));
    return BCM_E_NONE;
}

STATIC int
_bst_th_intr_status_reset(int unit)
{
    uint32 rval;

    BCM_IF_ERROR_RETURN(READ_MMU_XCFG_XPE_CPU_INT_CLEARr(unit, &rval));
    soc_reg_field_set(unit, MMU_XCFG_XPE_CPU_INT_CLEARr, &rval,
                      BST_THDO_INT_CLRf, 0);
    soc_reg_field_set(unit, MMU_XCFG_XPE_CPU_INT_CLEARr, &rval,
                      BST_THDI_INT_CLRf, 0);
    BCM_IF_ERROR_RETURN(WRITE_MMU_XCFG_XPE_CPU_INT_CLEARr(unit, rval));

    BCM_IF_ERROR_RETURN(READ_MMU_SCFG_SC_CPU_INT_CLEARr(unit, &rval));
    soc_reg_field_set(unit, MMU_SCFG_SC_CPU_INT_CLEARr, &rval,
                      BST_CFAP_A_INT_CLRf, 0);
    soc_reg_field_set(unit, MMU_SCFG_SC_CPU_INT_CLEARr, &rval,
                      BST_CFAP_B_INT_CLRf, 0);
    BCM_IF_ERROR_RETURN(WRITE_MMU_SCFG_SC_CPU_INT_CLEARr(unit, rval));

    return BCM_E_NONE;
}

STATIC int
_bcm_bst_th_intr_to_resources(int unit, uint32 *flags)
{
    uint32 flags_tr = 0;
    uint32 fval = 0;
    uint32 rval, rval_dev, rval32_type, rval32_status;
    uint64 rval_uc, rval_db;
    int index[_bcmResourceMaxCount] = {-1}, i;
    int res[_bcmResourceMaxCount] = {-1}, res_ct = 0;
    int bid_detect[_bcmResourceMaxCount] = {0};
    soc_field_t fld = INVALIDf;
    soc_reg_t reg = INVALIDr, reg_type = INVALIDr, reg_status = INVALIDr;
    int xpe = 0;

    _bcm_bst_th_intr_enable_set(unit, 0);
    _bst_th_intr_status_reset(unit);

    LOG_VERBOSE(BSL_LS_BCM_COSQ,
                (BSL_META_U(unit,
                            "BST interrupt handler: unit=%d flags=0x%x\n"),
                 unit, *flags));

    /* _bcmResourceDevice */
    BCM_IF_ERROR_RETURN(READ_CFAPBSTTHRSr(unit, &rval_dev));
    if (soc_reg_field_get(unit, CFAPBSTTHRSr, rval_dev, BST_THRESHOLDf)) {
        if (SOC_REG_IS_VALID(unit, MMU_SCFG_SC_CPU_INT_ENr)) {
            BCM_IF_ERROR_RETURN(READ_MMU_SCFG_SC_CPU_INT_STATr(unit, &rval));
            reg = MMU_SCFG_SC_CPU_INT_STATr;
            fld = BST_CFAP_A_INT_STATf;
            if (soc_reg_field_get(unit, reg, rval, fld)) {
                index[res_ct] = 0;
                res[res_ct++] = _bcmResourceDevice;
                soc_reg_field_set(unit, reg, &rval, fld, 0);
                BCM_IF_ERROR_RETURN(WRITE_MMU_SCFG_SC_CPU_INT_STATr(unit, rval));
            }
        }
    }

    /* THDI Status/Triggers */
    for (xpe = 0; xpe < NUM_XPE(unit); xpe++) {
        reg_type = SOC_REG_UNIQUE_ACC(unit, THDI_BST_TRIGGER_STATUS_TYPEr)[xpe];
        BCM_IF_ERROR_RETURN(soc_reg32_get(unit, reg_type, 0, 0, &rval32_type));
        reg_status = SOC_REG_UNIQUE_ACC(unit, THDI_BST_TRIGGER_STATUS_32r)[xpe];
        BCM_IF_ERROR_RETURN(soc_reg32_get(unit, reg_status, 0, 0, &rval32_status));

        /* _bcmResourceIngPool */
        fval = soc_reg_field_get(unit, THDI_BST_TRIGGER_STATUS_TYPEr, rval32_type,
                                 POOL_SHARED_TRIGGERf);
        if (fval && !bid_detect[_bcmResourceIngPool]) {
            fld = POOL_SHARED_TRIGGER_STATUSf;
            index[res_ct] = soc_reg_field_get(unit, reg_status, rval32_status, fld);
            res[res_ct++] = _bcmResourceIngPool;
            bid_detect[_bcmResourceIngPool] = 1;
        }

        /* _bcmResourcePortPool */
        fval = soc_reg_field_get(unit, THDI_BST_TRIGGER_STATUS_TYPEr, rval32_type,
                                 SP_SHARED_TRIGGERf);
        if (fval && !bid_detect[_bcmResourcePortPool]) {
            fld = SP_SHARED_TRIGGER_STATUSf;
            index[res_ct] = soc_reg_field_get(unit, reg_status, rval32_status, fld);
            res[res_ct++] = _bcmResourceIngPool;
            bid_detect[_bcmResourcePortPool] = 1;
        }

        /* _bcmResourcePriGroupShared */
        fval = soc_reg_field_get(unit, THDI_BST_TRIGGER_STATUS_TYPEr, rval32_type,
                                 PG_SHARED_TRIGGERf);
        if (fval && !bid_detect[_bcmResourcePriGroupShared]) {
            fld = PG_SHARED_TRIGGER_STATUSf;
            index[res_ct] = soc_reg_field_get(unit, reg_status, rval32_status, fld);
            res[res_ct++] = _bcmResourcePriGroupShared;
            bid_detect[_bcmResourcePriGroupShared] = 1;
        }

        /* _bcmResourcePriGroupHeadroom */
        fval = soc_reg_field_get(unit, THDI_BST_TRIGGER_STATUS_TYPEr, rval32_type,
                                 PG_HDRM_TRIGGERf);

        if (fval && !bid_detect[_bcmResourcePriGroupHeadroom]) {
            fld = PG_HDRM_TRIGGER_STATUSf;
            index[res_ct] = soc_reg_field_get(unit, reg_status, rval32_status, fld);
            res[res_ct++] = _bcmResourcePriGroupHeadroom;
            bid_detect[_bcmResourcePriGroupHeadroom] = 1;
        }

        rval32_type = 0;
        BCM_IF_ERROR_RETURN(soc_reg32_set(unit, reg_type, 0, 0, rval32_type));
        rval32_status = 0;
        BCM_IF_ERROR_RETURN(soc_reg32_set(unit, reg_status, 0, 0, rval32_status));
    }


    /* THDM DB Status/Triggers */
    for (xpe = 0; xpe < NUM_XPE(unit); xpe++) {
        BCM_IF_ERROR_RETURN
            (soc_reg_get(unit, MMU_THDM_DB_DEVICE_BST_STATr, xpe, 0, &rval_db));
        reg_status = MMU_THDM_DB_DEVICE_BST_STATr;

        /* _bcmResourceEgrPool */
        fval = soc_reg64_field32_get(unit, MMU_THDM_DB_DEVICE_BST_STATr, rval_db,
                                     MCUC_SP_BST_STAT_TRIGGEREDf);

        if (fval && !bid_detect[_bcmResourceEgrPool]) {
            fld = MCUC_SP_BST_STAT_IDf;
            index[res_ct] = soc_reg64_field32_get(unit, reg_status, rval_db, fld);
            res[res_ct++] = _bcmResourceEgrPool;
            bid_detect[_bcmResourceEgrPool] = 1;
        }

        /* _bcmResourceEgrMCastPool */
        if (fval && !bid_detect[_bcmResourceEgrMCastPool]) {
            fld = MC_SP_BST_STAT_IDf;
            index[res_ct] = soc_reg64_field32_get(unit, reg_status, rval_db, fld);
            res[res_ct++] = _bcmResourceEgrMCastPool;
            bid_detect[_bcmResourceEgrMCastPool] = 1;
        }

        COMPILER_64_ZERO(rval_db);
        BCM_IF_ERROR_RETURN
            (soc_reg_set(unit, MMU_THDM_DB_DEVICE_BST_STATr, xpe, 0, rval_db));
    }

    /* THDU Status/Triggers */
    for (xpe = 0; xpe < NUM_XPE(unit); xpe++) {
        BCM_IF_ERROR_RETURN
            (soc_reg_get(unit, THDU_BST_STATr, xpe, 0, &rval_uc));
        reg_status = THDU_BST_STATr;

        /* _bcmResourceUcast */
        fval = soc_reg64_field32_get(unit, MMU_THDM_DB_DEVICE_BST_STATr, rval_uc,
                                     BST_STAT_TRIGGERED_TYPEf);
        if (fval && !bid_detect[_bcmResourceUcast]) {
            fld = UC_Q_BST_STAT_IDf;
            index[res_ct] = soc_reg64_field32_get(unit, reg_status, rval_uc, fld);
            res[res_ct++] = _bcmResourceUcast;
            bid_detect[_bcmResourceUcast] = 1;
        }

        COMPILER_64_ZERO(rval_uc);
        BCM_IF_ERROR_RETURN
            (soc_reg_set(unit, THDU_BST_STATr, xpe, 0, rval_uc));
    }

    if (res_ct == 0) {
        _bcm_bst_th_intr_enable_set(unit, 1);
        /* No resource to fetch */
        return BCM_E_NONE;
    }

    for (i = 0; i < res_ct; i++) {
        BCM_IF_ERROR_RETURN(
            _bcm_bst_th_sync_hw_snapshot(unit, res[i], -1, index[i]));
    }

    *flags = flags_tr;
    _bcm_bst_th_intr_enable_set(unit, 1);
    return BCM_E_NONE;
}

int
_bcm_bst_th_hw_stat_clear(int unit, _bcm_bst_resource_info_t *resInfo,
                    bcm_bst_stat_id_t bid, int port, int index)
{
    _bcm_bst_cmn_unit_info_t *bst_info;
    int rv, pipe;
    uint32 rval;
    soc_mem_t base_mem = INVALIDm, mem = INVALIDm;
    soc_reg_t reg;
    int min_hw_idx, max_hw_idx, idx;
    int pipe_factor = 1, pipe_offset = 0, inst, stats_per_inst;
    int num_entries, mem_wsz;
    void *pentry;
    char *dmabuf;
    int enable = 0;
    int sync_val = 0;

    if (bid < _BCM_BST_RESOURCE_MIN || bid >= _BCM_BST_RESOURCE_MAX) {
        return BCM_E_PARAM;
    }

    if ((bst_info = _BCM_UNIT_BST_INFO(unit)) == NULL) {
        return BCM_E_INIT;
    }

    if (bst_info->pre_sync) {
        bst_info->pre_sync(unit, bid, &sync_val);
    }

    /* sync all the stats */
    if (resInfo->flags & _BCM_BST_CMN_RES_F_PIPED) {
        pipe_factor = NUM_PIPE(unit);
    }
    if (index == -1) {
        min_hw_idx = resInfo->index_min;
        if (resInfo->num_instance > 1) {
            max_hw_idx = ((resInfo->index_max + 1) / pipe_factor /
                          resInfo->num_instance) - 1;
        } else {
            max_hw_idx = ((resInfo->index_max + 1) / pipe_factor) - 1;
        }
    } else {
        min_hw_idx = max_hw_idx = index;
        if (bid == bcmBstStatIdDevice) {
            min_hw_idx = resInfo->index_min;
            max_hw_idx = resInfo->index_max;
        }
    }

    for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
        if ((base_mem = resInfo->stat_mem[0]) != INVALIDm) {
            int xpe = 0;
            for (xpe = 0; xpe < NUM_XPE(unit); xpe++) {
                mem = SOC_MEM_UNIQUE_ACC_XPE_PIPE(unit, base_mem, xpe, pipe);
                if (mem == INVALIDm) {
                    continue;
                }

                if (index < 0) {
                    num_entries = soc_mem_index_count(unit, mem);
                    mem_wsz = sizeof(uint32) * soc_mem_entry_words(unit, mem);

                    dmabuf = soc_cm_salloc(unit, num_entries * mem_wsz, "bst dmabuf");
                    if (dmabuf == NULL) {
                        return BCM_E_MEMORY;
                    }

                    if (soc_mem_read_range(unit, mem, MEM_BLOCK_ALL,
                                            soc_mem_index_min(unit, mem),
                                            soc_mem_index_max(unit, mem),
                                            dmabuf)) {
                        soc_cm_sfree(unit, dmabuf);
                        return BCM_E_INTERNAL;
                    }

                    /* Clear stat counter if requested */
                    for (idx = min_hw_idx; idx <= max_hw_idx; idx++) {
                        pentry = soc_mem_table_idx_to_pointer(unit, mem, void*,
                                                              dmabuf, idx);
                        soc_mem_field32_set(unit, mem, pentry, resInfo->stat_field, 0);
                    }

                    if (soc_mem_write_range(unit, mem, MEM_BLOCK_ALL,
                                            soc_mem_index_min(unit, mem),
                                            soc_mem_index_max(unit, mem),
                                            dmabuf)) {
                        soc_cm_sfree(unit, dmabuf);
                        return BCM_E_INTERNAL;
                    }
                    soc_cm_sfree(unit, dmabuf);
                } else {
                    /* Clear a specific index */
                    uint32 entry[SOC_MAX_MEM_WORDS];
                    SOC_IF_ERROR_RETURN(
                        soc_mem_read(unit, mem, MEM_BLOCK_ALL, index, entry));
                    soc_mem_field32_set(unit, mem, entry,
                                        resInfo->stat_field, 0);
                    SOC_IF_ERROR_RETURN(
                        soc_mem_write(unit, mem, MEM_BLOCK_ALL, index, entry));
                }
            }
        } else if (resInfo->profile_r) {
            pipe_offset += max_hw_idx;
        } else if ((reg = resInfo->stat_reg[pipe]) != INVALIDr) {
            int stat_idx = 0;
            stats_per_inst = resInfo->num_stat_pp / resInfo->num_instance;

            /* Clear stat counters */
            if (bst_info->control_get) {
                bst_info->control_get(unit, bcmSwitchBstEnable, &enable);
            }
            if (enable && bst_info->control_set) {
                bst_info->control_set(unit, bcmSwitchBstEnable, 0, TRUE);
            }

            for (idx = min_hw_idx; idx <= max_hw_idx; idx++) {
                rval = 0;
                if (stats_per_inst) {
                    inst = idx / stats_per_inst;
                    stat_idx = idx % stats_per_inst;
                } else {
                    inst = REG_PORT_ANY;
                    stat_idx = idx;
                }
                if (bid != bcmBstStatIdIngPool) {
                    rv = soc_reg32_get(unit, reg, inst, stat_idx, &rval);
                    if (rv) {
                        return BCM_E_INTERNAL;
                    }
                    soc_reg_field_set(unit, reg, &rval, resInfo->stat_field, 0);
                    BCM_IF_ERROR_RETURN(soc_reg32_set(unit, reg, inst, stat_idx, rval));
                } else {
                    /* IngPool requires clearing using THD_MISC_CONTROL reg */
                    soc_field_t ing_sp_clr_fields[] = {
                        CLEAR_THDI_BST_SP_GLOBAL_SHARED_CNT0f,
                        CLEAR_THDI_BST_SP_GLOBAL_SHARED_CNT1f,
                        CLEAR_THDI_BST_SP_GLOBAL_SHARED_CNT2f,
                        CLEAR_THDI_BST_SP_GLOBAL_SHARED_CNT3f,
                        CLEAR_THDI_BST_SP_GLOBAL_SHARED_CNT4f
                    };
                    rval = 0;
                    /* 4 SP + 1 Public pool */
                    soc_reg_field_set(unit, THD_MISC_CONTROLr, &rval,
                                      ing_sp_clr_fields[stat_idx], 0);
                    BCM_IF_ERROR_RETURN
                        (soc_reg32_set(unit, THD_MISC_CONTROLr, inst, stat_idx, rval));

                    soc_reg_field_set(unit, THD_MISC_CONTROLr, &rval,
                                      ing_sp_clr_fields[stat_idx], 1);
                    BCM_IF_ERROR_RETURN
                        (soc_reg32_set(unit, THD_MISC_CONTROLr, inst, stat_idx, rval));

                }
            }

            if (enable && bst_info->control_set) {
                bst_info->control_set(unit, bcmSwitchBstEnable, enable, TRUE);
            }

        } /* if valid reg */
    } /* for each pipe */

    if (bst_info->post_sync) {
        bst_info->post_sync(unit, bid, sync_val);
    }

    return BCM_E_NONE;
}

int
_bcm_bst_th_sync_hw_snapshot(int unit, bcm_bst_stat_id_t bid,
                             int port, int index)
{
    _bcm_bst_cmn_unit_info_t *bst_info;
    _bcm_bst_resource_info_t *resInfo;
    int num_entries, mem_wsz, sync_val = 0;
    int idx, idx_offset, thd_idx;
    int inst, xpe, xpe_offset, pipe, pipe_offset = 0;
    int stats_per_inst = 0; /* stats per mem instance(xpe) */
    void *pentry;
    char *dmabuf;
    uint32 rval, temp_val;
    soc_mem_t mem, base_mem;
    soc_reg_t reg;
    bcm_gport_t gport;
    bcm_cos_queue_t cosq;

    if ((bst_info = _BCM_UNIT_BST_INFO(unit)) == NULL) {
        return BCM_E_INIT;
    }

    if (bst_info->pre_sync) {
        bst_info->pre_sync(unit, bid, &sync_val);
    }

    /* sync all the stats */
    resInfo = _BCM_BST_RESOURCE(unit, bid);
    if (resInfo == NULL) {
        return BCM_E_PARAM;
    }

    base_mem = resInfo->stat_mem[0];
    reg = resInfo->stat_reg[0];

    if (reg != INVALIDr) {
        int stat_idx = 0;
        stats_per_inst = resInfo->num_stat_pp / resInfo->num_instance;
        pipe_offset = 0;

        for (idx = 0; idx < resInfo->num_stat_pp; idx++) {
            rval = 0;
            if (stats_per_inst) {
                inst = idx / stats_per_inst;
                stat_idx = idx % stats_per_inst;
            } else {
                inst = REG_PORT_ANY;
                stat_idx = idx;
                stats_per_inst = 1;
            }

            idx_offset = idx + pipe_offset;
            SOC_IF_ERROR_RETURN
                (soc_reg32_get(unit, reg, inst, stat_idx, &rval));
            temp_val = soc_reg_field_get(unit, reg, rval, resInfo->stat_field);
            if (bst_info->track_mode) {
                /* BST tracking is in Peak mode.
                 * Hence store the watermark value only.
                 */
                if (resInfo->p_stat[idx_offset] < temp_val) {
                    resInfo->p_stat[idx_offset] = temp_val;
                }
            } else {
                resInfo->p_stat[idx_offset] = temp_val;
            }

            /* Check if there exists threshold for a resource
             * continue if we don't have one */
            if (resInfo->p_threshold == NULL) {
                continue;
            }

            /* In TH, stat count and thd count need not be same as in previous
             * chips(check init). This is due to different AccType for Stat and
             * Threshold.
             * Stats - usually NON-DUP Acc Type
             * Threshold - usually DUP Acc Type
             * Hence threshold index might only exist for 1 instance and
             * applicable to all.
             */
            if (resInfo->flags & _BCM_BST_CMN_RES_F_RES_DEV) {
                /* For CFAP res, threshold exists per instance(xpe) */
                thd_idx = idx_offset;
            } else {
                thd_idx = idx_offset % stats_per_inst;
            }

            /* coverity[copy_paste_error : FALSE] */
            if (resInfo->p_threshold[thd_idx] &&
                (resInfo->p_stat[idx_offset] >=
                 (resInfo->p_threshold[thd_idx] * resInfo->threshold_gran)
                )) {
                /* Generate SOC EVENT */
                if (bst_info->handlers.reverse_resolve_index) {
                    /* reverse map the inedx to port/cos pair */
                    bst_info->handlers.reverse_resolve_index(unit, bid,
                                                             port, idx_offset,
                                                             &gport, &cosq);
                    soc_event_generate(unit, SOC_SWITCH_EVENT_MMU_BST_TRIGGER,
                                       bid, gport, cosq);

                }
            }
        }
    } else if (base_mem != INVALIDm) {
        int min_idx = 0, max_idx = 0;
        int stats_per_pipe = 0; /* stats per pipe per mem-instance(xpe) */

        /* Get the mem attributes(Max idx) from the first child,
         * which could be used for other instances.
         * Note: In TH, XPE 0/Pipe 0 instance valid for all memories(Ing/Egr).
         */
        mem = SOC_MEM_UNIQUE_ACC_XPE_PIPE(unit, base_mem, 0, 0);

        if (mem == INVALIDm) {
            return BCM_E_PARAM;
        }
        stats_per_inst = (resInfo->index_max + 1) / resInfo->num_instance;
        stats_per_pipe = stats_per_inst / NUM_PIPE(unit);

        num_entries = soc_mem_index_max(unit, mem) + 1;
        min_idx = soc_mem_index_min(unit, mem);
        max_idx = soc_mem_index_max(unit, mem);
        mem_wsz = sizeof(uint32) * soc_mem_entry_words(unit, mem);

        dmabuf = soc_cm_salloc(unit, num_entries * mem_wsz, "bst dmabuf");
        if (dmabuf == NULL) {
            return BCM_E_MEMORY;
        }

        for (xpe = 0; xpe < NUM_XPE(unit); xpe++) {
            xpe_offset = 0;
            gport = BCM_GPORT_INVALID;
            cosq = -1;

            if (xpe) {
                xpe_offset = stats_per_inst * xpe;
            }

            for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
                pipe_offset = 0;
                if (pipe) {
                    pipe_offset = stats_per_pipe * pipe;
                }

                mem = SOC_MEM_UNIQUE_ACC_XPE_PIPE(unit, base_mem, xpe, pipe);
                if (mem == INVALIDm) {
                    continue;
                }

                if (soc_mem_read_range(unit, mem, MEM_BLOCK_ALL,
                                        min_idx, max_idx, dmabuf)) {
                    soc_cm_sfree(unit, dmabuf);
                    return BCM_E_INTERNAL;
                }

                for (idx = 0; idx < max_idx; idx++) {
                    idx_offset = idx + pipe_offset + xpe_offset;
                    pentry = soc_mem_table_idx_to_pointer(unit, mem, void*,
                                                          dmabuf, idx);
                    temp_val = soc_mem_field32_get(unit, mem, pentry,
                                                   resInfo->stat_field);
                    if (bst_info->track_mode) {
                        /* BST tracking is in Peak mode.
                         * Hence store the watermark value only.
                         */
                        if (resInfo->p_stat[idx_offset] < temp_val) {
                            resInfo->p_stat[idx_offset] = temp_val;
                        }
                    } else {
                        resInfo->p_stat[idx_offset] = temp_val;
                    }

                    /* In TH, stat count and thd count need not be same as in
                     * previous chips(check init). This is due to different
                     * AccType for Stat and Threshold.
                     * Stats - usually NON-DUP Acc Type
                     * Threshold - usually DUP Acc Type
                     * Hence threshold index might only exist for 1 instance
                     * and applicable to all.
                     */
                    if (resInfo->flags & _BCM_BST_CMN_RES_F_RES_DEV) {
                        /* For CFAP res, threshold exists per instance(xpe) */
                        thd_idx = idx_offset;
                    } else {
                        /* For rest, considering thd of 1st instance(xpe) */
                        thd_idx = idx_offset % stats_per_inst;
                    }
                    if (resInfo->p_threshold[thd_idx] &&
                        (resInfo->p_stat[idx_offset] >=
                         (resInfo->p_threshold[thd_idx]) * resInfo->threshold_gran)) {
                        /* Generate SOC EVENT */
                        if (bst_info->handlers.reverse_resolve_index) {
                            /* reverse map the inedx to port/cos pair */
                            bst_info->handlers.reverse_resolve_index(unit, bid,
                                                                     port, idx_offset,
                                                                     &gport, &cosq);
                            soc_event_generate(unit, SOC_SWITCH_EVENT_MMU_BST_TRIGGER,
                                               bid, gport, cosq);

                        }
                    }
                }
            }
        }
        soc_cm_sfree(unit, dmabuf);
    }

    if (bst_info->post_sync) {
        bst_info->post_sync(unit, bid, sync_val);
    }

    return BCM_E_NONE;
}

/*
 * This function is used to enable and disable headroom
 * pool monitoring for given headroom pool index and pipe.
 */
STATIC int
_bcm_th_bst_hdrm_monitor_set(int unit, int hdrm_idx,
                             int pipe, int enable)
{
    int idx, idx_min, idx_max;
    uint32 rval, hdrm_pool_cfg;

    if (pipe == -1) {
        /* Both Layer 0 and Layer 1 */
        idx_min = 0;
        idx_max = NUM_LAYER(unit);
    } else if ((pipe == 0) || (pipe == 3)) {
        /* Layer 0 */
        idx_min = 0;
        idx_max = 1;
    } else if ((pipe == 1) || (pipe == 2)) {
        /* Layer 1 */
        idx_min = 1;
        idx_max = 2;
    } else {
        return BCM_E_PARAM;
    }

    for (idx = idx_min; idx < idx_max; idx++) {
        BCM_IF_ERROR_RETURN
            (soc_reg32_get(unit, THDI_HDRM_POOL_CFGr, idx, 0, &rval));
        hdrm_pool_cfg = soc_reg_field_get(unit, THDI_HDRM_POOL_CFGr, rval,
                                          PEAK_COUNT_UPDATE_EN_HPf);
        if (enable == 0x1) {
            hdrm_pool_cfg = hdrm_pool_cfg | (0x1 << hdrm_idx);
        } else {
            hdrm_pool_cfg = hdrm_pool_cfg & (~(0x1 << hdrm_idx));
        }
        soc_reg_field_set(unit, THDI_HDRM_POOL_CFGr, &rval,
                          PEAK_COUNT_UPDATE_EN_HPf, hdrm_pool_cfg);
        BCM_IF_ERROR_RETURN
            (soc_reg32_set(unit, THDI_HDRM_POOL_CFGr, idx, 0, rval));
    }

    return BCM_E_NONE;
}

/*
 * This function is used to clear the hardware stat for
 * the headroom pool usage count for given [Port, Prio].
 */
STATIC int
_bcm_th_bst_hdrm_hw_stat_clear(int unit,
                               bcm_gport_t gport,
                               bcm_cos_t cosq)
{
    int i, j;
    uint32 rval, hdrm_pool_cfg[2];
    _bcm_bst_cmn_unit_info_t *bst_info;
    _bcm_bst_resource_info_t *resInfo;
    int enable = 0x0;
    uint32 xpe_map = 0;
    int start_hw_index = 0, end_hw_index = 0, pipe = -1;
    soc_info_t *si = NULL;
    int bcm_rv;

    resInfo = _BCM_BST_RESOURCE(unit, bcmBstStatIdHeadroomPool);
    if ((resInfo == NULL) || (!_BCM_BST_RESOURCE_VALID(resInfo))) {
        return BCM_E_PARAM;
    }

    if ((bst_info = _BCM_UNIT_BST_INFO(unit)) == NULL) {
        return BCM_E_INIT;
    }

    if (gport == -1) {
        if (cosq == BCM_COS_INVALID) {
            pipe = -1;
            /* 4 Headroom Pool Ids */
            start_hw_index = 0;
            end_hw_index = 3;
            xpe_map = 0xf;
        } else {
            return BCM_E_PARAM;
        }
    } else {
        if (bst_info->handlers.resolve_index == NULL) {
            return BCM_E_UNAVAIL;
        }

        bst_info->handlers.resolve_index(unit, gport, cosq,
                                         bcmBstStatIdHeadroomPool, &pipe,
                                         &start_hw_index, &end_hw_index,
                                         NULL, NULL, &bcm_rv);
        if (bcm_rv) {
            return bcm_rv;
        }
        si = &SOC_INFO(unit);
        xpe_map = si->ipipe_xpe_map[pipe];
    }

    for (j = 0; j < NUM_LAYER(unit); j++) {
        rval = 0x0;
        BCM_IF_ERROR_RETURN
            (soc_reg32_get(unit, THDI_HDRM_POOL_CFGr, j, 0, &rval));
        hdrm_pool_cfg[j] = soc_reg_field_get(unit, THDI_HDRM_POOL_CFGr, rval,
                                          PEAK_COUNT_UPDATE_EN_HPf);
    }

    for (i = start_hw_index; i <= end_hw_index; i++) {
        /* Disable hardware tracking the headroom pool */
        enable = 0x0;
        BCM_IF_ERROR_RETURN
            (_bcm_th_bst_hdrm_monitor_set(unit, i, pipe, enable));

        /* Reset Headroom Pool Peak Use-Count */
        for (j = 0; j < NUM_XPE(unit); j++) {
            if (xpe_map & (1 << j)) {
                BCM_IF_ERROR_RETURN
                    (soc_reg32_get(unit, resInfo->stat_reg[0],
                                   j, i, &rval));
                soc_reg_field_set(unit, resInfo->stat_reg[0], &rval,
                                  resInfo->stat_field, 0x0);

                BCM_IF_ERROR_RETURN
                    (soc_reg32_set(unit, THDI_HDRM_POOL_PEAK_COUNT_HPr,
                                   j, i, rval));
            }
        }

        /* Enable hardware tracking the headroom pool */
        enable = 0x1;
        BCM_IF_ERROR_RETURN
            (_bcm_th_bst_hdrm_monitor_set(unit, i, pipe, enable));
    }

    for (j = 0; j < NUM_LAYER(unit); j++) {
        rval = 0x0;
        soc_reg_field_set(unit, THDI_HDRM_POOL_CFGr, &rval,
                          PEAK_COUNT_UPDATE_EN_HPf, hdrm_pool_cfg[j]);
        BCM_IF_ERROR_RETURN
            (soc_reg32_set(unit, THDI_HDRM_POOL_CFGr, j, 0, rval));
    }

    return BCM_E_NONE;
}

/*
 * This function is used to clear the software stat for
 * the headroom pool usage count for given [Port, Prio].
 */
STATIC int
_bcm_th_bst_hdrm_sw_stat_clear(int unit,
                               bcm_gport_t gport,
                               bcm_cos_t cosq)
{
    int i = 0, j = 0;
    _bcm_bst_resource_info_t *resInfo;
    _bcm_bst_cmn_unit_info_t *bst_info;
    uint32 xpe_map = 0;
    int start_hw_index = 0, end_hw_index = 0, pipe = -1;
    soc_info_t *si = NULL;
    int bcm_rv;

    if ((bst_info = _BCM_UNIT_BST_INFO(unit)) == NULL) {
        return BCM_E_INIT;
    }

    resInfo = _BCM_BST_RESOURCE(unit, bcmBstStatIdHeadroomPool);
    if ((resInfo == NULL) || (!_BCM_BST_RESOURCE_VALID(resInfo))) {
        return BCM_E_PARAM;
    }

    if (gport == -1) {
        if (cosq == BCM_COS_INVALID) {
            /* 4 Headroom Pool Ids */
            start_hw_index = 0;
            end_hw_index = 3;
            xpe_map = 0xf;
        } else {
            return BCM_E_PARAM;
        }
    } else {
        if (bst_info->handlers.resolve_index == NULL) {
            return BCM_E_UNAVAIL;
        }

        bst_info->handlers.resolve_index(unit, gport, cosq,
                                         bcmBstStatIdHeadroomPool, &pipe,
                                         &start_hw_index, &end_hw_index,
                                         NULL, NULL, &bcm_rv);
        if (bcm_rv) {
            return bcm_rv;
        }
        si = &SOC_INFO(unit);
        xpe_map = si->ipipe_xpe_map[pipe];
    }

    for (i = start_hw_index; i <= end_hw_index; i++) {
        for (j = 0; j < NUM_XPE(unit); j++) {
            if (xpe_map & (1 << j)) {
                resInfo->p_stat[i + j * NUM_XPE(unit)] = 0x0;
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * This function is used to clear the software/hardware
 * stat for the headroom pool usage count for given [Port, Prio].
 */
int
_bcm_th_bst_hdrm_stat_clear(int unit,
                            bcm_gport_t gport,
                            bcm_cos_queue_t cosq)
{
    _bcm_bst_cmn_unit_info_t *bst_info;
    _bcm_bst_resource_info_t *resInfo;
    int bcm_rv = BCM_E_NONE;

    if ((bst_info = _BCM_UNIT_BST_INFO(unit)) == NULL) {
        return BCM_E_INIT;
    }

    /* sync all the stats */
    resInfo = _BCM_BST_RESOURCE(unit, bcmBstStatIdHeadroomPool);
    if ((resInfo == NULL) || (!_BCM_BST_RESOURCE_VALID(resInfo))) {
        return BCM_E_PARAM;
    }

    sal_mutex_take(bst_info->bst_reslock, sal_mutex_FOREVER);
    bcm_rv = _bcm_th_bst_hdrm_sw_stat_clear(unit, gport, cosq);
    sal_mutex_give(bst_info->bst_reslock);
    if (BCM_FAILURE(bcm_rv)) {
        return bcm_rv;
    }

    if (bst_info->track_mode) {
        /* BST tracking is in peak mode */
        BCM_IF_ERROR_RETURN
            (_bcm_th_bst_hdrm_hw_stat_clear(unit, gport, cosq));
    }

    return BCM_E_NONE;
}

/*
 * This function is used to get the BST stat for
 * the headroom pool usage count from software cache.
 */
STATIC int
_bcm_th_cosq_bst_hdrm_stat_sw_get(int unit,
                                  bcm_gport_t gport,
                                  bcm_cos_t cosq,
                                  uint32 *max_hdrm_stat)
{
    _bcm_bst_resource_info_t *resInfo;
    _bcm_bst_cmn_unit_info_t *bst_info;
    uint32 temp_stat = 0;
    int i = 0, j = 0;
    int start_hw_index = 0, end_hw_index = 0, pipe = -1;
    uint32 xpe_map = 0;
    soc_info_t *si = NULL;
    int bcm_rv;

    resInfo = _BCM_BST_RESOURCE(unit, bcmBstStatIdHeadroomPool);
    if ((resInfo == NULL) || (!_BCM_BST_RESOURCE_VALID(resInfo))) {
        return BCM_E_PARAM;
    }

    if ((bst_info = _BCM_UNIT_BST_INFO(unit)) == NULL) {
        return BCM_E_INIT;
    }

    if (gport == -1) {
        if (cosq == BCM_COS_INVALID) {
            xpe_map = 0xf;
            start_hw_index = 0;
            end_hw_index = 3;
        } else {
            return BCM_E_PARAM;
        }
    } else {
        if (bst_info->handlers.resolve_index == NULL) {
            return BCM_E_UNAVAIL;
        }

        bst_info->handlers.resolve_index(unit, gport, cosq,
                                         bcmBstStatIdHeadroomPool, &pipe,
                                         &start_hw_index, &end_hw_index,
                                         NULL, NULL, &bcm_rv);
        if (bcm_rv) {
            return bcm_rv;
        }
        si = &SOC_INFO(unit);
        xpe_map = si->ipipe_xpe_map[pipe];
    }

    for (i = start_hw_index; i <= end_hw_index; i++) {
        for (j = 0; j < NUM_XPE(unit); j++) {
            if (xpe_map & (1 << j)) {
                temp_stat = resInfo->p_stat[i + (j * NUM_XPE(unit))];
                if (temp_stat > *max_hdrm_stat) {
                    *max_hdrm_stat = temp_stat;
                }
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * This function is used to get the BST stat for
 * the headroom pool usage count for given [Port, Prio].
 * Note: Regardless of tracking mode this function
 * returns max value of headroom pools.
 */
int
_bcm_th_cosq_bst_hdrm_stat_get(int unit,
                               bcm_gport_t gport,
                               bcm_cos_queue_t cosq,
                               uint32 options,
                               uint64 *pvalue)
{
    uint32 max_hdrm_stat = 0;

    if (pvalue == NULL) {
        return BCM_E_PARAM;
    }

    COMPILER_64_ZERO(*pvalue);

    BCM_IF_ERROR_RETURN
        (_bcm_th_cosq_bst_hdrm_stat_sw_get(unit, gport, cosq,
                                           &max_hdrm_stat));

    /* Cell Size = 208 bytes */
    COMPILER_64_ADD_32(*pvalue,
                       (max_hdrm_stat * _TH_MMU_BYTES_PER_CELL));

    if (options & BCM_COSQ_STAT_CLEAR) {
        BCM_IF_ERROR_RETURN
            (_bcm_th_bst_hdrm_stat_clear(unit, gport, cosq));
    }
    return BCM_E_NONE;
}

/* Function to return the corresponding XPE instance for a given gport.
 * Direction(Ingress/Egress) is derived from bid type
 */
int _bst_th_port_to_mmu_inst_map_get(int unit, bcm_bst_stat_id_t bid,
                                     bcm_gport_t gport, uint32 *mmu_inst_map)
{
    soc_info_t *si = &SOC_INFO(unit);
    _bcm_bst_resource_info_t *resInfo;
    bcm_port_t local_port;
    bcm_module_t module;
    bcm_trunk_t trunk;
    int id, pipe;

    resInfo = _BCM_BST_RESOURCE(unit, bid);
    if ((resInfo == NULL) || (!_BCM_BST_RESOURCE_VALID(resInfo))) {
        return BCM_E_PARAM;
    }

    if (!BCM_GPORT_IS_SET(gport)) {
        if (!SOC_PORT_VALID(unit, gport)) {
            return BCM_E_PORT;
        }
        local_port = gport;
    } else {
        BCM_IF_ERROR_RETURN
            (_bcm_esw_gport_resolve(unit, gport, &module,
                                    &local_port, &trunk, &id));
    }

    SOC_IF_ERROR_RETURN(soc_port_pipe_get(unit, local_port, &pipe));

    if ((bid == bcmBstStatIdEgrPool) || (bid == bcmBstStatIdEgrMCastPool) ||
        (bid == bcmBstStatIdUcast) || (bid == bcmBstStatIdMcast)) {
        /* For Egress Res,
         * Pipe 0,1 has XPE 0,2
         * Pipe 2,3 has XPE 1,3
         */
        *mmu_inst_map = si->epipe_xpe_map[pipe];
    } else if ((bid == bcmBstStatIdIngPool) || (bid == bcmBstStatIdPortPool) ||
               (bid == bcmBstStatIdPriGroupShared) ||
               (bid == bcmBstStatIdPriGroupHeadroom)) {
        /* For Ingress Res,
         * Pipe 0,3 has XPE 0,1
         * Pipe 1,2 has XPE 2,3
         */
        *mmu_inst_map = si->ipipe_xpe_map[pipe];
    } else {
        /* For Global res, return all instance */
        *mmu_inst_map = (1 << NUM_XPE(unit)) - 1;
    }

    return BCM_E_NONE;
}

int
bcm_bst_th_init(int unit)
{
    _bcm_bst_cmn_unit_info_t *bst_info;
    _bcm_bst_resource_info_t *pres;
    int idx_count, i;
    /* stat count and thd count need not be same.
     * stat_count - Acc Type of stat are Non Dup, hence stat is for all ACC type
     * thd_count - Acc Type of Thd are Dup(except CFAP), hence 1 copy.
     */
    int total_stat_count, total_thd_count;
    soc_mem_t stat_mem = INVALIDm, child_mem = INVALIDm, thd_mem = INVALIDm;
    soc_reg_t stat_reg = INVALIDr;
    soc_field_t stat_field = INVALIDf, thd_field = INVALIDf;
    int layer = 0, granularity = 1;
    uint32 rval;

    bst_info = _BCM_UNIT_BST_INFO(unit);
    if (!bst_info) {
        return BCM_E_MEMORY;
    }

    bst_info->max_bytes = 0x500000;
    bst_info->to_cell = _bcm_bst_th_byte_to_cell;
    bst_info->to_byte = _bcm_bst_th_cell_to_byte;
    bst_info->control_set = _bcm_th_bst_control_set;
    bst_info->control_get = _bcm_th_bst_control_get;
    bst_info->intr_to_resources = _bcm_bst_th_intr_to_resources;
    bst_info->hw_stat_snapshot = _bcm_bst_th_sync_hw_snapshot;
    bst_info->hw_stat_clear = _bcm_bst_th_hw_stat_clear;
    bst_info->intr_enable_set = _bcm_bst_th_intr_enable_set;
    bst_info->pre_sync = NULL;
    bst_info->post_sync = NULL;
    bst_info->hw_intr_cb = soc_th_set_hw_intr_cb;
    bst_info->port_to_mmu_inst_map = _bst_th_port_to_mmu_inst_map_get;

    /* DEVICE - Resources begin */
    /* bcmBstStatIdDevice */
    pres = _BCM_BST_RESOURCE(unit, bcmBstStatIdDevice);
    _BCM_BST_STAT_THRESHOLD_INFO_INIT(pres);
    _BCM_BST_RESOURCE_FLAG_SET(pres, _BCM_BST_CMN_RES_F_RES_DEV);
    pres->num_instance = NUM_XPE(unit);
    idx_count = 1;
    stat_mem = INVALIDm;
    stat_reg = CFAPBSTSTATr;
    stat_field = BST_STATf;

    /* Threshold info count and Resource allocation */
    granularity = 1;
    total_thd_count = NUM_XPE(unit) * idx_count;
    _BCM_BST_THRESHOLD_INFO(pres, INVALIDm, CFAPBSTTHRSr, BST_THRESHOLDf,
                            granularity);
    _BCM_BST_RESOURCE_THRESHOLD_INIT(pres, total_thd_count, "bst cfap");
    /* Stat info count and Resource allocation */
    total_stat_count = total_thd_count; /* No per pipe copy */
    _BCM_BST_STAT_INFO(pres, stat_mem, stat_reg, stat_field);
    _BCM_BST_RESOURCE_STAT_INIT(pres, total_stat_count, "bst cfap");
    pres->index_min = 0;
    pres->index_max = total_stat_count - 1;
    /* DEVICE - Resources end */

    /* INGRESS - Resources begin */
    /* bcmBstStatIdHeadroomPool */
    pres = _BCM_BST_RESOURCE(unit, bcmBstStatIdHeadroomPool);
    _BCM_BST_STAT_THRESHOLD_INFO_INIT(pres);
    _BCM_BST_RESOURCE_FLAG_SET(pres, _BCM_BST_CMN_RES_F_RES_ING);
    pres->num_instance = NUM_XPE(unit);
    /* There are 4 HDRM POOL per XPE */
    idx_count = _TH_MMU_NUM_POOL;
    stat_mem = INVALIDm;
    BCM_IF_ERROR_RETURN(READ_MMU_GCFG_MISCCONFIGr(unit, &rval));
    bst_info->track_mode = soc_reg_field_get(unit, MMU_GCFG_MISCCONFIGr,
                                             rval, BST_TRACKING_MODEf);
    /*
     * As Headroom Pool has two different stats resource
     * for tracking current usage count and peak usage count
     * we set up the respective resource accordingly based on
     * BST Tracking Mode
     */
    if (bst_info->track_mode) {
        /* Peak Mode */
        stat_reg = THDI_HDRM_POOL_PEAK_COUNT_HPr;
        stat_field = PEAK_BUFFER_COUNTf;
    } else {
        /* Current Mode */
        stat_reg = THDI_HDRM_POOL_COUNT_HPr;
        stat_field = TOTAL_BUFFER_COUNTf;
    }

    /* Disabling the headroom Pool monitoring during init */
    for (layer = 0; layer < NUM_LAYER(unit); layer++) {
        BCM_IF_ERROR_RETURN
            (soc_reg32_set(unit, THDI_HDRM_POOL_CFGr, layer, 0, 0x0));
    }

    /* Stat info count and Resource allocation */
    total_stat_count = NUM_XPE(unit) * idx_count; /* No per pipe copy */
    _BCM_BST_STAT_INFO(pres, stat_mem, stat_reg, stat_field);
    _BCM_BST_RESOURCE_STAT_INIT(pres, total_stat_count, "bst ing headroom pool");
    pres->index_min = 0;
    pres->index_max = total_stat_count - 1;


    /* bcmBstStatIdIngPool */
    pres = _BCM_BST_RESOURCE(unit, bcmBstStatIdIngPool);
    _BCM_BST_STAT_THRESHOLD_INFO_INIT(pres);
    _BCM_BST_RESOURCE_FLAG_SET(pres, _BCM_BST_CMN_RES_F_RES_ING);
    pres->num_instance = NUM_XPE(unit);
    idx_count = 4;
    stat_mem = INVALIDm;
    stat_reg = THDI_BST_SP_GLOBAL_SHARED_CNTr;
    stat_field = SP_GLOBAL_SHARED_CNTf;

    /* Threshold info count and Resource allocation */
    granularity = 1;
    total_thd_count = idx_count; /* Acc type is Duplicate, hence 1 copy */
    _BCM_BST_THRESHOLD_INFO(pres, INVALIDm,
                            THDI_BST_SP_GLOBAL_SHARED_PROFILEr,
                            SP_GLOBAL_SHARED_PROFILEf, granularity);
    _BCM_BST_RESOURCE_THRESHOLD_INIT(pres, total_thd_count, "bst ing pool");
    /* Stat info count and Resource allocation */
    total_stat_count = NUM_XPE(unit) * total_thd_count; /* No per pipe copy */
    _BCM_BST_STAT_INFO(pres, stat_mem, stat_reg, stat_field);
    _BCM_BST_RESOURCE_STAT_INIT(pres, total_stat_count, "bst ing pool");
    pres->index_min = 0;
    pres->index_max = total_stat_count - 1;

    /* bcmBstStatIdPortPool */
    pres = _BCM_BST_RESOURCE(unit, bcmBstStatIdPortPool);
    _BCM_BST_STAT_THRESHOLD_INFO_INIT(pres);
    _BCM_BST_RESOURCE_FLAG_SET(pres, _BCM_BST_CMN_RES_F_PROFILEID |
                                     _BCM_BST_CMN_RES_F_PIPED |
                                     _BCM_BST_CMN_RES_F_RES_ING);
    _BCM_BST_RESOURCE_PBMP_SET(pres, PBMP_ALL(unit));
    pres->num_instance = NUM_XPE(unit);
    stat_mem = THDI_PORT_SP_BSTm;
    stat_reg = INVALIDr;
    stat_field = SP_BST_STAT_PORT_COUNTf;

    /* Get the mem attributes(Max idx) from the first child,
     * which could be used for other instances.
     * Note: In TH, XPE 0/Pipe 0 instance valid for all memories(Ing/Egr).
     */
    child_mem = SOC_MEM_UNIQUE_ACC_XPE_PIPE(unit, stat_mem, 0, 0);
    idx_count = soc_mem_index_max(unit, child_mem) + 1;

    /* Threshold info count and Resource allocation */
    granularity = 1;
    total_thd_count = NUM_PIPE(unit) * idx_count;
    thd_mem = THDI_PORT_SP_CONFIGm;
    thd_field = SP_BST_PROFILE_SHAREDf;
    for (i = 0; i < NUM_PIPE(unit); i++) {
        _BCM_BST_THRESHOLD_INFO_N(pres, SOC_MEM_UNIQUE_ACC(unit, thd_mem)[i],
                                  INVALIDr, thd_field, granularity, i);
    }
    _BCM_BST_PROFILE_INFO(pres, INVALIDm, THDI_BST_SP_SHARED_PROFILEr,
                          BST_SP_SHARED_THRESHf);
    _BCM_BST_RESOURCE_THRESHOLD_INIT(pres, total_thd_count, "bst port pool");
    /* Stat info count and Resource allocation */
    total_stat_count = NUM_XPE(unit) * total_thd_count;
    _BCM_BST_STAT_INFO(pres, stat_mem, stat_reg, stat_field);
    _BCM_BST_RESOURCE_STAT_INIT(pres, total_stat_count, "bst port pool");
    pres->index_min = 0;
    pres->index_max = total_stat_count - 1;

    /* bcmBstStatIdPriGroupShared */
    pres = _BCM_BST_RESOURCE(unit, bcmBstStatIdPriGroupShared);
    _BCM_BST_STAT_THRESHOLD_INFO_INIT(pres);
    _BCM_BST_RESOURCE_FLAG_SET(pres, _BCM_BST_CMN_RES_F_PROFILEID |
                                     _BCM_BST_CMN_RES_F_PIPED |
                                     _BCM_BST_CMN_RES_F_RES_ING);
    _BCM_BST_RESOURCE_PBMP_SET(pres, PBMP_ALL(unit));
    pres->num_instance = NUM_XPE(unit);
    stat_mem = THDI_PORT_PG_BSTm;
    stat_reg = INVALIDr;
    stat_field = PG_BST_STAT_SHAREDf;
    idx_count = NUM_PORT(unit) * 8;
    child_mem = SOC_MEM_UNIQUE_ACC_XPE_PIPE(unit, stat_mem, 0, 0);
    idx_count = soc_mem_index_max(unit, child_mem) + 1;

    /* Threshold info count and Resource allocation */
    granularity = 1;
    total_thd_count = idx_count * NUM_PIPE(unit);
    thd_mem = THDI_PORT_PG_CONFIGm;
    thd_field = PG_BST_PROFILE_SHAREDf;
    for (i = 0; i < NUM_PIPE(unit); i++) {
        _BCM_BST_THRESHOLD_INFO_N(pres, SOC_MEM_UNIQUE_ACC(unit, thd_mem)[i],
                                  INVALIDr, thd_field, granularity, i);
    }
    _BCM_BST_PROFILE_INFO(pres, INVALIDm,
                          THDI_BST_PG_SHARED_PROFILEr, BST_PG_SHARED_THRESHf);
    _BCM_BST_RESOURCE_THRESHOLD_INIT(pres, total_thd_count, "bst pg");
    /* Stat info count and Resource allocation */
    /* Stat res - THDI_PORT_PG_BST : (Num Ports (32/pipe) * 4 pipes) * 8(PGs) */
    total_stat_count = NUM_XPE(unit) * total_thd_count;
    _BCM_BST_STAT_INFO(pres, stat_mem, stat_reg, stat_field);
    _BCM_BST_RESOURCE_STAT_INIT(pres, total_stat_count, "bst pg");
    pres->index_min = 0;
    pres->index_max = total_stat_count - 1;

    /* bcmBstStatIdPriGroupHeadroom */
    pres = _BCM_BST_RESOURCE(unit, bcmBstStatIdPriGroupHeadroom);
    _BCM_BST_STAT_THRESHOLD_INFO_INIT(pres);
    pres->num_instance = NUM_XPE(unit);
    _BCM_BST_RESOURCE_FLAG_SET(pres, _BCM_BST_CMN_RES_F_PROFILEID |
                                     _BCM_BST_CMN_RES_F_PIPED |
                                     _BCM_BST_CMN_RES_F_RES_ING);
    _BCM_BST_RESOURCE_PBMP_SET(pres, PBMP_ALL(unit));
    stat_mem = THDI_PORT_PG_BSTm;
    stat_reg = INVALIDr;
    stat_field = PG_BST_STAT_HDRMf;
    idx_count = NUM_PORT(unit) * 8;
    child_mem = SOC_MEM_UNIQUE_ACC_XPE_PIPE(unit, stat_mem, 0, 0);
    idx_count = soc_mem_index_max(unit, child_mem) + 1;

    /* Threshold info count and Resource allocation */
    granularity = 1;
    total_thd_count = idx_count * NUM_PIPE(unit);
    thd_mem = THDI_PORT_PG_CONFIGm;
    thd_field = PG_BST_PROFILE_HDRMf;
    for (i = 0; i < NUM_PIPE(unit); i++) {
        _BCM_BST_THRESHOLD_INFO_N(pres, SOC_MEM_UNIQUE_ACC(unit, thd_mem)[i],
                                  INVALIDr, thd_field, granularity, i);
    }
    _BCM_BST_PROFILE_INFO(pres, INVALIDm, THDI_BST_PG_HDRM_PROFILEr,
                          BST_PG_HDRM_THRESHf);
    _BCM_BST_RESOURCE_THRESHOLD_INIT(pres, total_thd_count, "bst pg hdrm");
    /* Stat info count and Resource allocation */
    /* Stat res - THDI_PORT_PG_BST : (Num Ports (32/pipe) * 4 pipes) * 8(PGs) */
    total_stat_count = NUM_XPE(unit) * total_thd_count;
    _BCM_BST_STAT_INFO(pres, stat_mem, stat_reg, stat_field);
    _BCM_BST_RESOURCE_STAT_INIT(pres, total_stat_count, "bst pg hdrm");
    pres->index_min = 0;
    pres->index_max = total_stat_count - 1;
    /* INGRESS - Resources end */

    /* EGRESS - Resources begin */
    /* bcmBstStatIdEgrPool */
    pres = _BCM_BST_RESOURCE(unit, bcmBstStatIdEgrPool);
    _BCM_BST_STAT_THRESHOLD_INFO_INIT(pres);
    _BCM_BST_RESOURCE_FLAG_SET(pres, _BCM_BST_CMN_RES_F_RES_EGR);
    pres->num_instance = NUM_XPE(unit);
    idx_count = 4;
    pres->threshold_adj = -1;
    stat_mem = INVALIDm;
    stat_reg = MMU_THDM_DB_POOL_MCUC_BST_STATr;
    stat_field = BST_STATf;

    /* Threshold info count and Resource allocation */
    granularity = 1;
    total_thd_count = idx_count; /* No per pipe copy */
    _BCM_BST_THRESHOLD_INFO(pres, INVALIDm,
                            MMU_THDM_DB_POOL_MCUC_BST_THRESHOLDr, BST_STATf,
                            granularity);
    _BCM_BST_RESOURCE_THRESHOLD_INIT(pres, total_thd_count, "bst egrpool");
    /* Stat info count and Resource allocation */
    total_stat_count = NUM_XPE(unit) * total_thd_count; /* No per pipe copy */
    _BCM_BST_STAT_INFO(pres, stat_mem, stat_reg, stat_field);
    _BCM_BST_RESOURCE_STAT_INIT(pres, total_stat_count, "bst egrpool");
    pres->index_min = 0;
    pres->index_max = total_stat_count - 1;

    /* bcmBstStatIdEgrMCastPool */
    pres = _BCM_BST_RESOURCE(unit, bcmBstStatIdEgrMCastPool);
    _BCM_BST_STAT_THRESHOLD_INFO_INIT(pres);
    _BCM_BST_RESOURCE_FLAG_SET(pres, _BCM_BST_CMN_RES_F_RES_EGR);
    pres->num_instance = NUM_XPE(unit);
    idx_count = 4;
    pres->threshold_adj = -1;
    stat_mem = INVALIDm;
    stat_reg = MMU_THDM_DB_POOL_MC_BST_STATr;
    stat_field = BST_STATf;

    /* Threshold info count and Resource allocation */
    granularity = 1;
    total_thd_count = idx_count; /* No per pipe copy */
    _BCM_BST_THRESHOLD_INFO(pres, INVALIDm,
                            MMU_THDM_DB_POOL_MC_BST_THRESHOLDr, BST_STATf,
                            granularity);
    _BCM_BST_RESOURCE_THRESHOLD_INIT(pres, total_thd_count, "bst egr mcpool");
    /* Stat info count and Resource allocation */
    total_stat_count = NUM_XPE(unit) * total_thd_count; /* No per pipe copy */
    _BCM_BST_STAT_INFO(pres, stat_mem, stat_reg, stat_field);
    _BCM_BST_RESOURCE_STAT_INIT(pres, total_stat_count, "bst egr mcpool");
    pres->index_min = 0;
    pres->index_max = total_stat_count - 1;

    /* bcmBstStatIdMcast */
    pres = _BCM_BST_RESOURCE(unit, bcmBstStatIdMcast);
    _BCM_BST_STAT_THRESHOLD_INFO_INIT(pres);
    _BCM_BST_RESOURCE_FLAG_SET(pres, _BCM_BST_CMN_RES_F_PROFILEID |
                                     _BCM_BST_CMN_RES_F_PIPED |
                                     _BCM_BST_CMN_RES_F_RES_EGR);
    _BCM_BST_RESOURCE_PBMP_SET(pres, PBMP_CMIC(unit));
    pres->num_instance = NUM_XPE(unit);
    stat_mem = MMU_THDM_DB_QUEUE_BSTm;
    stat_reg = INVALIDr;
    stat_field = Q_COUNTf;

    /* Get the mem attributes(Max idx) from the first child,
     * which could be used for other instances.
     * Note: In TH, XPE 0/Pipe 0 instance valid for all memories(Ing/Egr).
     */
    child_mem = SOC_MEM_UNIQUE_ACC_XPE_PIPE(unit, stat_mem, 0, 0);
    idx_count = soc_mem_index_max(unit, child_mem) + 1;

    /* Threshold info count and Resource allocation */
    granularity = 1;
    total_thd_count = NUM_PIPE(unit) * idx_count;
    thd_mem = MMU_THDM_DB_QUEUE_CONFIGm;
    thd_field = BST_THRESHOLD_PROFILEf;
    for (i = 0; i < NUM_PIPE(unit); i++) {
        _BCM_BST_THRESHOLD_INFO_N(pres, SOC_MEM_UNIQUE_ACC(unit, thd_mem)[i],
                                  INVALIDr, thd_field, granularity, i);
    }
    _BCM_BST_PROFILE_INFO(pres, INVALIDm,
                   MMU_THDM_DB_QUEUE_MC_BST_THRESHOLD_PROFILEr, BST_STATf);
    _BCM_BST_RESOURCE_THRESHOLD_INIT(pres, total_thd_count, "bst mcast");
    /* Stat info count and Resource allocation */
    total_stat_count = NUM_XPE(unit) * total_thd_count;
    _BCM_BST_STAT_INFO(pres, stat_mem, stat_reg, stat_field);
    _BCM_BST_RESOURCE_STAT_INIT(pres, total_stat_count, "bst mcast");
    pres->index_min = 0;
    pres->index_max = total_stat_count - 1;

    /* bcmBstStatIdUcast */
    pres = _BCM_BST_RESOURCE(unit, bcmBstStatIdUcast);
    _BCM_BST_STAT_THRESHOLD_INFO_INIT(pres);
    _BCM_BST_RESOURCE_FLAG_SET(pres, _BCM_BST_CMN_RES_F_PROFILEID |
                                     _BCM_BST_CMN_RES_F_PIPED |
                                     _BCM_BST_CMN_RES_F_RES_EGR);
    _BCM_BST_RESOURCE_PBMP_SET(pres, PBMP_PORT_ALL(unit));
    pres->num_instance = NUM_XPE(unit);
    stat_mem = MMU_THDU_BST_QUEUEm;
    stat_reg = INVALIDr;
    stat_field = Q_COUNTf;

    /* Get the mem attributes(Max idx) from the first child,
     * which could be used for other instances.
     * Note: In TH, XPE 0/Pipe 0 instance valid for all memories(Ing/Egr).
     */
    child_mem = SOC_MEM_UNIQUE_ACC_XPE_PIPE(unit, stat_mem, 0, 0);
    idx_count = soc_mem_index_max(unit, child_mem) + 1;

    /* Threshold info count and Resource allocation */
    granularity = 8; /* UC Queue Thd is 8 cell granularity */
    total_thd_count = NUM_PIPE(unit) * idx_count;
    thd_mem = MMU_THDU_CONFIG_QUEUEm;
    thd_field = Q_WM_MAX_THRESHOLDf;
    for (i = 0; i < NUM_PIPE(unit); i++) {
        _BCM_BST_THRESHOLD_INFO_N(pres, SOC_MEM_UNIQUE_ACC(unit, thd_mem)[i],
                                  INVALIDr, thd_field, granularity, i);
    }
    _BCM_BST_PROFILE_INFO(pres, INVALIDm, OP_UC_QUEUE_BST_THRESHOLDr, BST_STATf);
    _BCM_BST_RESOURCE_THRESHOLD_INIT(pres, total_thd_count, "bst ucast");
    /* Stat info count and Resource allocation */
    total_stat_count = NUM_XPE(unit) * total_thd_count;
    _BCM_BST_STAT_INFO(pres, stat_mem, stat_reg, stat_field);
    _BCM_BST_RESOURCE_STAT_INIT(pres, total_stat_count, "bst ucast");
    pres->index_min = 0;
    pres->index_max = total_stat_count - 1;
    /* EGRESS - Resources end */

    return BCM_E_NONE;
}
#endif /* BCM_TOMAHAWK_SUPPORT */

