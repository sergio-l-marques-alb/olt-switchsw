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
 * MMU/Cosq soc routines
 *
 */

#include <sal/core/libc.h>

#include <soc/debug.h>
#include <soc/util.h>
#include <soc/mem.h>
#include <soc/trident2.h>
#include <soc/tomahawk.h>
#include <shared/bsl.h>
#if defined(BCM_TOMAHAWK_SUPPORT)

#define TH_MMU_PER_PIPE_PORT_OFFSET 64

int
soc_th_sched_weight_set(int unit, int port, int level,
                        int index, int weight, int mc)
{
    soc_info_t *si;
    soc_mem_t mem = INVALIDm;
    uint32 entry[SOC_MAX_MEM_WORDS];
    int phy_port, mmu_port, pipe;

    si = &SOC_INFO(unit);

    pipe = si->port_pipe[port];
    phy_port = si->port_l2p_mapping[port];
    mmu_port = si->port_p2m_mapping[phy_port];

    if (level == SOC_TH_NODE_LVL_L0) {
        mem = Q_SCHED_L0_WEIGHT_MEMm;
        index = ((mmu_port % TH_MMU_PER_PIPE_PORT_OFFSET)  * 10) + index
                + (mc ? SOC_TH_NUM_UC_QUEUES_PER_PIPE : 0);
    } else if (level == SOC_TH_NODE_LVL_L1) {
        mem = Q_SCHED_L1_WEIGHT_MEMm;

        if (IS_CPU_PORT(unit, port)) {
            index = 660 + index;
        } else if (IS_LB_PORT(unit, port)) {
            index = 650 + index;
        } else {
            index = ((mmu_port % TH_MMU_PER_PIPE_PORT_OFFSET)  * 10) + index
                    + (mc ? SOC_TH_NUM_UC_QUEUES_PER_PIPE : 0);
        }
    } else {
        return SOC_E_PARAM;
    }

    mem = SOC_MEM_UNIQUE_ACC(unit, mem)[pipe];

    SOC_IF_ERROR_RETURN
        (soc_mem_read(unit, mem, MEM_BLOCK_ALL, index, &entry));
    soc_mem_field32_set(unit, mem, &entry, WEIGHTf, weight);
    SOC_IF_ERROR_RETURN
        (soc_mem_write(unit, mem, MEM_BLOCK_ALL, index, &entry));

    return SOC_E_NONE;
}

int
soc_th_sched_weight_get(int unit, int port, int level,
                        int index, int *weight, int mc)
{
    soc_info_t *si;
    soc_mem_t mem = INVALIDm;
    uint32 entry[SOC_MAX_MEM_WORDS];
    int phy_port, mmu_port, pipe;

    si = &SOC_INFO(unit);

    pipe = si->port_pipe[port];
    phy_port = si->port_l2p_mapping[port];
    mmu_port = si->port_p2m_mapping[phy_port];

    if (level == SOC_TH_NODE_LVL_L0) {
        mem = Q_SCHED_L0_WEIGHT_MEMm;
        index = ((mmu_port % TH_MMU_PER_PIPE_PORT_OFFSET)  * 10) + index
                + (mc ? SOC_TH_NUM_UC_QUEUES_PER_PIPE : 0);
    } else if (level == SOC_TH_NODE_LVL_L1) {
        mem = Q_SCHED_L1_WEIGHT_MEMm;

        if (IS_CPU_PORT(unit, port)) {
            index = 660 + index;
        } else if (IS_LB_PORT(unit, port)) {
            index = 650 + index;
        } else {
            index = ((mmu_port % TH_MMU_PER_PIPE_PORT_OFFSET)  * 10) + index
                    + (mc ? SOC_TH_NUM_UC_QUEUES_PER_PIPE : 0);
        }
    } else {
        return SOC_E_PARAM;
    }

    mem = SOC_MEM_UNIQUE_ACC(unit, mem)[pipe];
    SOC_IF_ERROR_RETURN
        (soc_mem_read(unit, mem, MEM_BLOCK_ALL, index, &entry));
    *weight = soc_mem_field32_get(unit, mem, &entry, WEIGHTf);

    return SOC_E_NONE;
}

int
soc_th_cosq_sched_mode_set(int unit, soc_port_t port, int level, int index,
                           soc_th_sched_mode_e mode, int weight, int mc)
{
    soc_reg_t reg;
    uint32 fval, rval, wrr_mask;
    soc_info_t *si = &SOC_INFO(unit);

    LOG_INFO(BSL_LS_SOC_COSQ,
             (BSL_META_U(unit,
                         "Port:%d L%s%d config : index=%d MODE=%d WT=%d\n"),
			 port, (level == 0) ? "r" : "", level - 1,
             index, mode, weight));

    /* selection between SP and WxRR is based on weight property. */
    SOC_IF_ERROR_RETURN(soc_th_sched_weight_set(unit, port,
                                                level, index, weight, mc));

    if (level == SOC_TH_NODE_LVL_L0) {
        if (IS_CPU_PORT(unit, port)) {
            reg = Q_SCHED_CPU_PORT_CONFIGr;
        } else {
            reg = Q_SCHED_PORT_CONFIGr;
        }
        index = 0;
    } else if (level == SOC_TH_NODE_LVL_L1) {
        if (IS_CPU_PORT(unit, port)) {
            int parent_index = 0;
            reg = Q_SCHED_CPU_L0_NODE_CONFIGr;
            SOC_IF_ERROR_RETURN
                (soc_th_cosq_cpu_parent_get(unit, index, SOC_TH_NODE_LVL_L1,
                                            &parent_index));
            index = parent_index;
        } else {
            reg = Q_SCHED_L0_NODE_CONFIGr;
            index %= si->port_num_cosq[port];
        }
    } else {
        return SOC_E_PARAM;
    }

    if (mode == SOC_TH_SCHED_MODE_WRR) {
        fval = 1;
    } else if (mode == SOC_TH_SCHED_MODE_WERR) {
        fval = 0;
    } else {
        return SOC_E_NONE;
    }

    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, port, 0, &rval));
    wrr_mask = soc_reg_field_get(unit, reg, rval, ENABLE_WRRf);
    wrr_mask &= ~(1 << index);
    wrr_mask |= (fval << index);
    soc_reg_field_set(unit, reg, &rval, ENABLE_WRRf, wrr_mask);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, port, 0, rval));

    return SOC_E_NONE;
}

int
soc_th_cosq_sched_mode_get(int unit, soc_port_t port, int level, int index,
                           soc_th_sched_mode_e *mode, int *weight, int mc)
{
    soc_reg_t reg;
    uint32 rval, wrr_mask;
    soc_info_t *si = &SOC_INFO(unit);

    /* selection between SP and WxRR is based on weight property. */
    SOC_IF_ERROR_RETURN(soc_th_sched_weight_get(unit, port,
                                                level, index, weight, mc));

    if (level == SOC_TH_NODE_LVL_L0) {
        if (IS_CPU_PORT(unit, port)) {
            reg = Q_SCHED_CPU_PORT_CONFIGr;
        } else {
            reg = Q_SCHED_PORT_CONFIGr;
        }
        index = 0;
    } else if (level == SOC_TH_NODE_LVL_L1) {
        if (IS_CPU_PORT(unit, port)) {
            int parent_index = 0;
            reg = Q_SCHED_CPU_L0_NODE_CONFIGr;
            SOC_IF_ERROR_RETURN
                (soc_th_cosq_cpu_parent_get(unit, index, SOC_TH_NODE_LVL_L1,
                                            &parent_index));
            index = parent_index;
        } else {
            reg = Q_SCHED_L0_NODE_CONFIGr;
            index %= si->port_num_cosq[port];
        }
    } else {
        return SOC_E_PARAM;
    }

    if (*weight == 0) {
        *mode = SOC_TH_SCHED_MODE_STRICT;
    } else {
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, port, 0, &rval));
        wrr_mask = soc_reg_field_get(unit, reg, rval, ENABLE_WRRf);
        if (wrr_mask & (1 << index)) {
            *mode = SOC_TH_SCHED_MODE_WRR;
        } else {
            *mode = SOC_TH_SCHED_MODE_WERR;
        }
    }

    return SOC_E_NONE;
}

/*
 * Function to Set Parent(L0) for a given L1 queue of the CPU port
 * Note: Only child @ L1 level is supported.
 * Because only L1 level queues can be attached to different parent (L0.0-L0.9)
 * of the CPU port.
 */
int
soc_th_cosq_cpu_parent_set(int unit, int child_index, int child_level,
                           int parent_index)
{
    soc_reg_t reg = Q_SCHED_CPU_L1_MC_QUEUE_L0_MAPPINGr;
    uint32 rval = 0;

    if (child_level != SOC_TH_NODE_LVL_L1) {
        /* Only child at L1 are allowed to move */
        return SOC_E_PARAM;
    }
    /* Set the L1 queue's parent to the given L0 node */
    SOC_IF_ERROR_RETURN
        (soc_reg32_get(unit, reg, 0, child_index, &rval));
    soc_reg_field_set(unit, reg, &rval, SELECTf, parent_index);
    SOC_IF_ERROR_RETURN
        (soc_reg32_set(unit, reg, 0, child_index, rval));

    return SOC_E_NONE;
}

/*
 * Function to Get Parent(L0) for a given L1 queue of the CPU port
 * Note: Only child @ L1 level is supported.
 * Because only L1 level queues can be attached to different parent (L0.0-L0.9)
 * of the CPU port.
 */
int
soc_th_cosq_cpu_parent_get(int unit, int child_index, int child_level,
                           int *parent_index)
{
    soc_reg_t reg = Q_SCHED_CPU_L1_MC_QUEUE_L0_MAPPINGr;
    uint32 rval = 0;

    if (child_level != SOC_TH_NODE_LVL_L1) {
        /* Only child at L1 are allowed to move */
        return SOC_E_PARAM;
    }
    /* Set the L1 queue's parent to the given L0 node */
    SOC_IF_ERROR_RETURN
        (soc_reg32_get(unit, reg, 0, child_index, &rval));
    *parent_index = soc_reg_field_get(unit, reg, rval, SELECTf);

    return SOC_E_NONE;
}

int soc_th_mmu_get_shared_size(int unit, int *thdi_shared,
                               int *thdo_db_shared, int *thdo_qe_shared)
{
    uint32 rval = 0;
    /* Read shared size from Service pool 0 */
    /* THDI */
    SOC_IF_ERROR_RETURN
        (READ_THDI_BUFFER_CELL_LIMIT_SPr(unit, 0, &rval));
    *thdi_shared = soc_reg_field_get(unit, THDI_BUFFER_CELL_LIMIT_SPr,
                                     rval, LIMITf);
    /* THDO - DB */
    rval = 0;
    SOC_IF_ERROR_RETURN
        (READ_MMU_THDM_DB_POOL_SHARED_LIMITr(unit, 0, &rval));
    *thdo_db_shared = soc_reg_field_get(unit, MMU_THDM_DB_POOL_SHARED_LIMITr,
                                        rval, SHARED_LIMITf);

    /* THDO - QE */
    rval = 0;
    SOC_IF_ERROR_RETURN
        (READ_MMU_THDM_MCQE_POOL_SHARED_LIMITr(unit, 0, &rval));
    *thdo_qe_shared = soc_reg_field_get(unit, MMU_THDM_MCQE_POOL_SHARED_LIMITr,
                                        rval, SHARED_LIMITf);
    LOG_VERBOSE(BSL_LS_SOC_MMU,
                (BSL_META_U(unit,
                            "MMU Shared cell allocation: ING: %d, "
                            "EGR: DB %d, QE %d (unit of cells)\n"),
                            *thdi_shared, *thdo_db_shared, *thdo_qe_shared));
    return SOC_E_NONE;
}

int soc_th_mmu_config_res_limits_update(int unit, int res, int ing_shd,
                                        int egr_db_shd, int egr_qe_shd,
                                        int post_update)
{
    return soc_th_mmu_config_shared_buf_recalc(unit, res, ing_shd,
                                               egr_db_shd, egr_qe_shd,
                                               post_update);
}

#endif /* BCM_TOMAHAWK_SUPPORT */
