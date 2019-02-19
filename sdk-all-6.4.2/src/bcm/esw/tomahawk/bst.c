/*
 * $Id: $
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
 * TH counter module routines.
 *
 */

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
#include <bcm_int/bst.h>

#if defined(BCM_TOMAHAWK_SUPPORT)
extern int _bcm_bst_th_sync_hw_snapshot(int unit, bcm_bst_stat_id_t bid,
                                        int port, int index);

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
_bcm_th_bst_control_set(int unit, bcm_switch_control_t type, int arg)
{
    uint32 rval, rval_en, rval_action, i, fval, enable = 0;
    _bcm_bst_cmn_unit_info_t *bst_info;
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
        
        if (fval) {
            BCM_IF_ERROR_RETURN(_bcm_bst_sync_thread_start(unit));
        } else {
            BCM_IF_ERROR_RETURN(_bcm_bst_sync_thread_stop(unit));
        }

        break;
    case bcmSwitchBstTrackingMode:
        BCM_IF_ERROR_RETURN(READ_MMU_GCFG_MISCCONFIGr(unit, &rval));
        soc_reg_field_set(unit, MMU_GCFG_MISCCONFIGr, &rval, BST_TRACKING_MODEf, arg);
        BCM_IF_ERROR_RETURN(WRITE_MMU_GCFG_MISCCONFIGr(unit, rval));
        bst_info->track_mode = arg ? 1 : 0;
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
    bst_info = _BCM_UNIT_BST_INFO(unit);

    if (!bst_info) {
        return BCM_E_INIT;
    }

    switch (type) {
    case bcmSwitchBstEnable:
        BCM_IF_ERROR_RETURN(READ_MMU_GCFG_BST_TRACKING_ENABLEr(unit, &rval));
        *arg = soc_reg_field_get(unit, MMU_GCFG_BST_TRACKING_ENABLEr, rval, 
                                 BST_TRACK_EN_THDOf);
        *arg = *arg ? 1 : 0;
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
            soc_reg_field_set(unit, MMU_XCFG_XPE_CPU_INT_ENr, &rval,
                              BST_CFAP_INT_ENf, enable);
            BCM_IF_ERROR_RETURN(WRITE_MMU_XCFG_XPE_CPU_INT_ENr(unit, rval));
        }
    }
    return BCM_E_NONE;
}

STATIC int
_bcm_bst_th_intr_to_resources(int unit, uint32 *flags)
{
    uint32 flags_tr = 0;
    uint32 fval = 0;
    uint32 rval, rval_dev;
    uint64 rval_uc, rval_db;
    uint64 rval64;
    int index[_bcmResourceMaxCount] = {-1}, i;
    int res[_bcmResourceMaxCount] = {-1}, res_ct = 0;
    soc_field_t fld = INVALIDf;
    soc_reg_t reg = INVALIDr;

    _bcm_bst_th_intr_enable_set(unit, 0);

    BCM_IF_ERROR_RETURN(READ_BST_TRACKING_ENABLEr(unit, &rval));
    BCM_IF_ERROR_RETURN(READ_THDU_BST_STATr(unit, &rval_uc));
    BCM_IF_ERROR_RETURN(READ_MMU_THDM_DB_DEVICE_BST_STATr(unit, &rval_db));

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

    /* Ingress based Status/Triggers */
    reg = THDI_BST_TRIGGER_STATUSr;
    BCM_IF_ERROR_RETURN(READ_THDI_BST_TRIGGER_STATUS_TYPEr(unit, &rval));

    /* _bcmResourceIngPool */
    fval = soc_reg_field_get(unit, THDI_BST_TRIGGER_STATUS_TYPEr, 
                                     rval, POOL_SHARED_TRIGGERf);
    if (fval) {
        fld = POOL_SHARED_TRIGGER_STATUSf;
        index[res_ct] = soc_reg_field_get(unit, reg, rval, fld);
        res[res_ct++] = _bcmResourceIngPool;
        soc_reg_field_set(unit, THDI_BST_TRIGGER_STATUS_TYPEr, &rval,
                          POOL_SHARED_TRIGGERf, 0);
    }

    /* _bcmResourcePortPool */
    fval = soc_reg_field_get(unit, THDI_BST_TRIGGER_STATUS_TYPEr, rval,
                             SP_SHARED_TRIGGERf);
    if (fval) {
        fld = SP_SHARED_TRIGGER_STATUSf;
        index[res_ct] = soc_reg_field_get(unit, reg, rval, fld);
        res[res_ct++] = _bcmResourceIngPool;
        soc_reg_field_set(unit, THDI_BST_TRIGGER_STATUS_TYPEr, &rval,
                          SP_SHARED_TRIGGERf, 0);
    }

    /* _bcmResourcePriGroupShared */
    fval = soc_reg_field_get(unit, THDI_BST_TRIGGER_STATUS_TYPEr, rval,
                             PG_SHARED_TRIGGERf);
    if (fval) {
        fld = PG_SHARED_TRIGGER_STATUSf;
        index[res_ct] = soc_reg_field_get(unit, reg, rval, fld);
        res[res_ct++] = _bcmResourcePriGroupShared;
        soc_reg_field_set(unit, THDI_BST_TRIGGER_STATUS_TYPEr, &rval,
                          PG_SHARED_TRIGGERf, 0);
    }

    /* _bcmResourcePriGroupHeadroom */
    fval = soc_reg_field_get(unit, THDI_BST_TRIGGER_STATUS_TYPEr, rval,
                             PG_HDRM_TRIGGERf);

    if (fval) {
        fld = PG_HDRM_TRIGGER_STATUSf;
        index[res_ct] = soc_reg_field_get(unit, reg, rval, fld);
        res[res_ct++] = _bcmResourcePriGroupShared;
        soc_reg_field_set(unit, THDI_BST_TRIGGER_STATUS_TYPEr, &rval,
                          PG_SHARED_TRIGGERf, 0);
    }

    BCM_IF_ERROR_RETURN(WRITE_THDI_BST_TRIGGER_STATUS_TYPEr(unit, 0));

    /* Egress based Status/Triggers */
    /* _bcmResourceEgrPool */
    fval = soc_reg64_field32_get(unit, MMU_THDM_DB_DEVICE_BST_STATr, 
                                 rval_db, BST_STAT_TRIGGERED_TYPEf);
    if (fval) {
        reg = MMU_THDM_DB_DEVICE_BST_STATr;
        switch (fval) {
            case 0x1:
                res[res_ct] = _bcmResourceEgrPool;
                fld = MCUC_SP_BST_STAT_IDf;
                break;
            case 0x2:
                res[res_ct] = _bcmResourceEgrMCastPool;
                fld = MC_SP_BST_STAT_IDf;
                break;
            default:
                fld = INVALIDf;
        }
        if (fld != INVALIDf) {
            index[res_ct++] = soc_reg64_field32_get(unit, reg, rval_db, fld);
        }
    }

    /* _bcmResourceUcast */
    if ((fval = soc_reg64_field32_get(unit, THDU_BST_STATr,
                              rval_uc, BST_STAT_TRIGGERED_TYPEf))) {
        if (fval & 0x1) {
            res[res_ct++] = _bcmResourceUcast;
            fld = UC_Q_BST_STAT_IDf;
            index[res_ct] = soc_reg64_field32_get(unit, THDU_BST_STATr, rval_uc, fld);
        }
        COMPILER_64_ZERO(rval64);
        BCM_IF_ERROR_RETURN(WRITE_THDU_BST_STATr(unit, rval64));
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
        stats_per_inst = resInfo->num_stat_pp / resInfo->num_instance;
        pipe_offset = 0;

        for (idx = 0; idx < resInfo->num_stat_pp; idx++) {
            inst = idx / stats_per_inst;
            idx_offset = idx + pipe_offset;
            SOC_IF_ERROR_RETURN
                (soc_reg32_get(unit, reg, inst, idx, &rval));
            temp_val = soc_reg_field_get(unit, reg, rval, resInfo->stat_field);
            if (bst_info->track_mode) {
                if (resInfo->p_stat[idx_offset] > temp_val) {
                    resInfo->p_stat[idx_offset] = temp_val;
                }
            } else {
                resInfo->p_stat[idx_offset] = temp_val;
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
            
            /* coverity [copy_paste_error : FALSE] */
            if (resInfo->p_threshold[thd_idx] &&
                (resInfo->p_stat[idx_offset] >= 
                 (resInfo->p_threshold[thd_idx] * resInfo->threshold_gran)
                )) {
                /* Generate SOC EVENT */
                if (bst_info->handlers.reverse_resolve_index) {
                    /* reverse map the inedx to port/cos pair */
                    bst_info->handlers.reverse_resolve_index(unit, bid, 
                                                             port, index,
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
        stats_per_inst = resInfo->index_max / resInfo->num_instance;
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
                        if (resInfo->p_stat[idx_offset] > temp_val) {
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

int 
bcm_bst_th_init(int unit)
{
    _bcm_bst_cmn_unit_info_t *bst_info;
    _bcm_bst_resource_info_t *pres;
    int idx_count;
    /* stat count and thd count need not be same. 
     * stat_count - Acc Type of stat are Non Dup, hence stat is for all ACC type
     * thd_count - Acc Type of Thd are Dup(except CFAP), hence 1 copy.
     */
    int total_stat_count, total_thd_count;
    soc_mem_t stat_mem = INVALIDm, child_mem = INVALIDm;
    soc_reg_t stat_reg = INVALIDr;
    soc_field_t stat_field = INVALIDf;

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
    bst_info->pre_sync = NULL;
    bst_info->post_sync = NULL;

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
    total_thd_count = NUM_XPE(unit) * idx_count;
    _BCM_BST_THRESHOLD_INFO(pres, INVALIDm, CFAPBSTTHRSr, BST_THRESHOLDf, 1);
    _BCM_BST_RESOURCE_THRESHOLD_INIT(pres, total_thd_count, "bst cfap");
    /* Stat info count and Resource allocation */
    total_stat_count = total_thd_count; /* No per pipe copy */
    _BCM_BST_STAT_INFO(pres, stat_mem, stat_reg, stat_field);
    _BCM_BST_RESOURCE_STAT_INIT(pres, total_stat_count, "bst cfap");
    pres->index_min = 0;
    pres->index_max = total_stat_count - 1;
    /* DEVICE - Resources end */

    /* INGRESS - Resources begin */
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
    total_thd_count = idx_count; /* Acc type is Duplicate, hence 1 copy */
    _BCM_BST_THRESHOLD_INFO(pres, INVALIDm, 
            THDI_BST_SP_GLOBAL_SHARED_PROFILEr, SP_GLOBAL_SHARED_PROFILEf, 1);
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
    total_thd_count = NUM_PIPE(unit) * idx_count;
    _BCM_BST_THRESHOLD_INFO(pres, THDI_PORT_SP_CONFIGm, INVALIDr, 
                            SP_BST_PROFILE_SHAREDf, 1);
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
                                     _BCM_BST_CMN_RES_F_RES_ING);
    _BCM_BST_RESOURCE_PBMP_SET(pres, PBMP_ALL(unit));
    pres->num_instance = NUM_XPE(unit);
    stat_mem = THDI_PORT_PG_BSTm;
    stat_reg = INVALIDr;
    stat_field = PG_BST_STAT_SHAREDf;
    idx_count = NUM_PORT(unit) * 8;

    /* Threshold info count and Resource allocation */
    total_thd_count = NUM_PIPE(unit) * idx_count;
    _BCM_BST_THRESHOLD_INFO(pres, THDI_PORT_PG_CONFIGm, INVALIDr, 
                            PG_BST_PROFILE_SHAREDf, 1);
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
                                     _BCM_BST_CMN_RES_F_RES_ING);
    _BCM_BST_RESOURCE_PBMP_SET(pres, PBMP_ALL(unit));
    stat_mem = THDI_PORT_PG_BSTm;
    stat_reg = INVALIDr;
    stat_field = PG_BST_STAT_HDRMf;
    idx_count = NUM_PORT(unit) * 8;

    /* Threshold info count and Resource allocation */
    total_thd_count = NUM_PIPE(unit) * idx_count;
    _BCM_BST_THRESHOLD_INFO(pres, THDI_PORT_PG_CONFIGm, INVALIDr, 
                            PG_BST_PROFILE_HDRMf, 1);
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
    pres->threshold_adj = 0;
    stat_mem = INVALIDm;
    stat_reg = MMU_THDM_DB_POOL_MCUC_BST_STATr;
    stat_field = BST_STATf;

    /* Threshold info count and Resource allocation */
    total_thd_count = idx_count; /* No per pipe copy */
    _BCM_BST_THRESHOLD_INFO(pres, INVALIDm, 
                            MMU_THDM_DB_POOL_MCUC_BST_THRESHOLDr, BST_STATf, 1);
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
    pres->threshold_adj = 0;
    stat_mem = INVALIDm;
    stat_reg = MMU_THDM_DB_POOL_MC_BST_STATr;
    stat_field = BST_STATf;

    /* Threshold info count and Resource allocation */
    total_thd_count = idx_count; /* No per pipe copy */
    _BCM_BST_THRESHOLD_INFO(pres, INVALIDm, 
                            MMU_THDM_DB_POOL_MC_BST_THRESHOLDr, BST_STATf, 1);
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
    total_thd_count = NUM_PIPE(unit) * idx_count;
    _BCM_BST_THRESHOLD_INFO(pres, MMU_THDM_DB_QUEUE_CONFIGm, INVALIDr,
                            BST_THRESHOLD_PROFILEf, 1);
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
    total_thd_count = NUM_PIPE(unit) * idx_count;
    _BCM_BST_THRESHOLD_INFO(pres, MMU_THDU_CONFIG_QUEUEm, INVALIDr,
                            Q_WM_MAX_THRESHOLDf, 1);
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

