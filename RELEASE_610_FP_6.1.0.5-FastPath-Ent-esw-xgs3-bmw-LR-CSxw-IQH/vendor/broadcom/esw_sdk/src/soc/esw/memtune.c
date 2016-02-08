/*
 * $Id: memtune.c,v 1.1 2011/04/18 17:11:04 mruas Exp $
 * $Copyright: Copyright 2009 Broadcom Corporation.
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
 * File:        memtune.c
 * Purpose:     External memory interface tuning routines.
 * Notes:       Framework for memory tuning sequences.
 */

#include <soc/defs.h>

#include <soc/cm.h>
#include <soc/debug.h>
#include <soc/error.h>
#include <soc/cmic.h>
#include <soc/mem.h>
#include <soc/memtune.h>
#include <soc/er_tcam.h>

#include <sal/appl/sal.h>
#include <sal/appl/config.h>
#include <shared/alloc.h>

#if defined(BCM_TRIUMPH_SUPPORT) || defined(BCM_EASYRIDER_SUPPORT)

#ifdef BCM_EASYRIDER_SUPPORT
#include <soc/easyrider.h>
#endif /* BCM_EASYRIDER_SUPPORT */
#ifdef BCM_TRIUMPH_SUPPORT
#include <soc/triumph.h>
#endif /* BCM_TRIUMPH_SUPPORT */

#ifdef BCM_EASYRIDER_SUPPORT
STATIC int
_soc_er_phase_sel_prog_hw_cb(soc_memtune_data_t *mt_data)
{
    soc_memtune_ctrl_t *mt_ctrl = mt_data->mt_ctrl;
    soc_er_memtune_data_t *er_mt_data = mt_ctrl->data;
    uint32 oval, rval, addr;

    if (mt_ctrl->phase_sel_cur != -1) {
        addr = soc_reg_addr(mt_ctrl->unit, er_mt_data->ddr_reg3, REG_PORT_ANY,
                            0);
        SOC_IF_ERROR_RETURN(soc_reg32_read(mt_ctrl->unit, addr, &rval));
        oval = rval;
        soc_reg_field_set(mt_ctrl->unit, er_mt_data->ddr_reg3, &rval,
                          PHASE_SEL0f, mt_ctrl->phase_sel_cur);
        soc_reg_field_set(mt_ctrl->unit, er_mt_data->ddr_reg3, &rval,
                          PHASE_SEL1f, mt_ctrl->phase_sel_cur);
        soc_reg_field_set(mt_ctrl->unit, er_mt_data->ddr_reg3, &rval,
                          OVRD_SM0_ENf, 1);
        soc_reg_field_set(mt_ctrl->unit, er_mt_data->ddr_reg3, &rval,
                          OVRD_SM1_ENf, 1);

        if (mt_data->interface != SOC_MEM_INTERFACE_QDR) {
            soc_reg_field_set(mt_ctrl->unit, er_mt_data->ddr_reg3, &rval,
                              PHASE_SEL2f, mt_ctrl->phase_sel_cur);
            soc_reg_field_set(mt_ctrl->unit, er_mt_data->ddr_reg3, &rval,
                              PHASE_SEL3f, mt_ctrl->phase_sel_cur);
            soc_reg_field_set(mt_ctrl->unit, er_mt_data->ddr_reg3, &rval,
                              OVRD_SM2_ENf, 1);
            soc_reg_field_set(mt_ctrl->unit, er_mt_data->ddr_reg3, &rval,
                              OVRD_SM3_ENf, 1);
        }
        if (rval != oval) {
            SOC_IF_ERROR_RETURN(soc_reg32_write(mt_ctrl->unit, addr, rval));
        }
    }

    return SOC_E_NONE;
}

STATIC int
_soc_er_ddr_prog_hw_cb(soc_memtune_data_t *mt_data)
{
    soc_memtune_ctrl_t *mt_ctrl = mt_data->mt_ctrl;
    soc_er_memtune_data_t *er_mt_data = mt_ctrl->data;
    uint32 oval, rval, addr, lat_bit1, lat_bit2;

    /* *** Weird translation *** */
    lat_bit1 = (mt_ctrl->ddr_latency_cur == 1) ? 1 : 0;
    lat_bit2 = (mt_ctrl->ddr_latency_cur == 0) ? 1 : 0;

    /* Latency settings */
    addr = soc_reg_addr(mt_ctrl->unit, er_mt_data->ddr_reg2, REG_PORT_ANY, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_read(mt_ctrl->unit, addr, &rval));
    oval = rval;
    if (mt_data->slice_mask & 0x1) {
        soc_reg_field_set(mt_ctrl->unit, er_mt_data->ddr_reg2, &rval,
                          SEL_EARLY2_0f, lat_bit2);
        soc_reg_field_set(mt_ctrl->unit, er_mt_data->ddr_reg2, &rval,
                          SEL_EARLY1_0f, lat_bit1);
    }
    if (mt_data->slice_mask & 0x2) {
        soc_reg_field_set(mt_ctrl->unit, er_mt_data->ddr_reg2, &rval,
                          SEL_EARLY2_1f, lat_bit2);
        soc_reg_field_set(mt_ctrl->unit, er_mt_data->ddr_reg2, &rval,
                          SEL_EARLY1_1f, lat_bit1);
    }
    if (mt_data->slice_mask & 0x4) {
        soc_reg_field_set(mt_ctrl->unit, er_mt_data->ddr_reg2, &rval,
                          SEL_EARLY2_2f, lat_bit2);
        soc_reg_field_set(mt_ctrl->unit, er_mt_data->ddr_reg2, &rval,
                          SEL_EARLY1_2f, lat_bit1);
    }
    if (mt_data->slice_mask & 0x8) {
        soc_reg_field_set(mt_ctrl->unit, er_mt_data->ddr_reg2, &rval,
                          SEL_EARLY2_3f, lat_bit2);
        soc_reg_field_set(mt_ctrl->unit, er_mt_data->ddr_reg2, &rval,
                          SEL_EARLY1_3f, lat_bit1);
    }
    if (rval != oval) {
        SOC_IF_ERROR_RETURN(soc_reg32_write(mt_ctrl->unit, addr, rval));
    }

    /* RX phase offset settings */
    if (mt_data->interface != SOC_MEM_INTERFACE_QDR) {
        addr = soc_reg_addr(mt_ctrl->unit, er_mt_data->ddr_reg3, REG_PORT_ANY,
                            0);
        SOC_IF_ERROR_RETURN(soc_reg32_read(mt_ctrl->unit, addr, &rval));
        oval = rval;
        if (mt_data->slice_mask & 0x1) {
            soc_reg_field_set(mt_ctrl->unit, er_mt_data->ddr_reg3, &rval,
                              DLL90_OFFSET_QKBf, mt_ctrl->rx_offset_cur);
        }
        if (rval != oval) {
            SOC_IF_ERROR_RETURN(soc_reg32_write(mt_ctrl->unit, addr, rval));
        }
    }

    addr = soc_reg_addr(mt_ctrl->unit, er_mt_data->ddr_reg1, REG_PORT_ANY, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_read(mt_ctrl->unit, addr, &rval));
    oval = rval;
    if (mt_data->interface == SOC_MEM_INTERFACE_QDR) {
        if (mt_data->slice_mask & 0x1) {
            soc_reg_field_set(mt_ctrl->unit, er_mt_data->ddr_reg1, &rval,
                              DLL90_OFFSET_QKf, mt_ctrl->rx_offset_cur);
            soc_reg_field_set(mt_ctrl->unit, er_mt_data->ddr_reg1, &rval,
                              DLL90_OFFSET_QKBf, mt_ctrl->rx_offset_cur);
        }
    } else {
        if (mt_data->slice_mask & 0x1) {
            soc_reg_field_set(mt_ctrl->unit, er_mt_data->ddr_reg1, &rval,
                              DLL90_OFFSET0_QKf, mt_ctrl->rx_offset_cur);
        }
        if (mt_data->slice_mask & 0x2) {
            soc_reg_field_set(mt_ctrl->unit, er_mt_data->ddr_reg1, &rval,
                              DLL90_OFFSET1f, mt_ctrl->rx_offset_cur);
        }
        if (mt_data->slice_mask & 0x4) {
            soc_reg_field_set(mt_ctrl->unit, er_mt_data->ddr_reg1, &rval,
                              DLL90_OFFSET2f, mt_ctrl->rx_offset_cur);
        }
        if (mt_data->slice_mask & 0x8) {
            soc_reg_field_set(mt_ctrl->unit, er_mt_data->ddr_reg1, &rval,
                              DLL90_OFFSET3f, mt_ctrl->rx_offset_cur);
        }
    }
    /* TX phase offset setting here */
    soc_reg_field_set(mt_ctrl->unit, er_mt_data->ddr_reg1, &rval,
                      DLL90_OFFSET_TXf, mt_ctrl->tx_offset_cur);
    if (rval != oval) {
        SOC_IF_ERROR_RETURN(soc_reg32_write(mt_ctrl->unit, addr, rval));
    }

    if (mt_ctrl->em_latency_cur != -1) {
        /* Program TMODE0 register */
        SOC_IF_ERROR_RETURN(READ_CSE_DTU_LTE_TMODE0r(mt_ctrl->unit, &rval));
        oval = rval;
        soc_reg_field_set(mt_ctrl->unit, CSE_DTU_LTE_TMODE0r, &rval,
                          EM_LATENCY7f,
                          (mt_ctrl->em_latency_cur == 7) ? 1 : 0);
        if (rval != oval) {
            SOC_IF_ERROR_RETURN(WRITE_CSE_DTU_LTE_TMODE0r(mt_ctrl->unit,
                                                          rval));
        }

        SOC_IF_ERROR_RETURN(READ_SEER_CONFIGr(mt_ctrl->unit, &rval));
        oval = rval;
        soc_reg_field_set(mt_ctrl->unit, SEER_CONFIGr, &rval,
                          CSE_SEL_EM_LATENCY7f,
                          (mt_ctrl->em_latency_cur == 7) ? 1 : 0);
        if (rval != oval) {
            SOC_IF_ERROR_RETURN(WRITE_SEER_CONFIGr(mt_ctrl->unit, rval));
        }
    }

    return SOC_E_NONE;
}

STATIC int
_soc_er_rldram_tune_test_cb(soc_memtune_data_t *mt_data)
{
    soc_memtune_ctrl_t *mt_ctrl = mt_data->mt_ctrl;
    soc_er_memtune_data_t *er_mt_data = mt_ctrl->data;
    soc_er_cd_chunk_t    chunk_data = NULL;
    soc_er_cd_slice_t    slice_data = NULL;
    soc_er_cd_columns_t  columns_data = NULL;
    int poll_count;
    uint32 rval, addr, slice, column, rand_val;
    int ix, rv;

    rv = soc_er_dual_dimensional_array_alloc(&chunk_data, 
                                             SOC_ER_CELLDATA_CHUNKS,
                                             SOC_MAX_MEM_WORDS);
    if (SOC_FAILURE(rv)) {
        goto done;
    }
    rv = soc_er_dual_dimensional_array_alloc(&slice_data, 
                                             SOC_ER_CELLDATA_SLICES,
                                             SOC_ER_CELLDATA_SLICE_WORDS);
    if (SOC_FAILURE(rv)) {
        goto done;
    }
    rv = soc_er_dual_dimensional_array_alloc(&columns_data, 
                                             SOC_ER_CELLDATA_SLICES,
                                             SOC_ER_CELLDATA_SLICE_COLUMNS);
    if (SOC_FAILURE(rv)) {
        goto done;
    }

    for (slice = 0; slice < SOC_ER_CELLDATA_SLICES; slice++) {
        for (column = 0; column < SOC_ER_CELLDATA_SLICE_COLUMNS;
             column++) {
            if ((slice < 8) || (slice > 50)) {
                rand_val = 100;
#ifndef NO_MEMTUNE
                rand_val = sal_rand();
#endif /* NO_MENTUNE */
                columns_data[slice][column] =
                    (rand_val & SOC_ER_CELLDATA_DIFF_COLUMN_MASK);
             } else {
                   columns_data[slice][column] =
                       ((slice + column) % 2) ?
                       SOC_ER_CELLDATA_DIFF_COLUMN_55 :
                       SOC_ER_CELLDATA_DIFF_COLUMN_AA;
             }
        }
    }

    columns_data[SOC_ER_CELLDATA_SLICES - 1][0] = 0;
    columns_data[SOC_ER_CELLDATA_SLICES - 1][1] = 0;
    columns_data[SOC_ER_CELLDATA_SLICES - 1][2] &=
        SOC_ER_CELLDATA_DIFF_S60_C2_MASK;

    soc_er_celldata_columns_to_slice(columns_data, slice_data);
    soc_er_celldata_slice_to_chunk(slice_data, chunk_data);
    rv = soc_er_celldata_chunk_write(mt_ctrl->unit, er_mt_data->write_mem,
                                     -1, chunk_data);
    if (SOC_FAILURE(rv)) {
        goto done;
    }

    for (ix = 0; ix < mt_data->test_count; ix++) {
        addr = soc_reg_addr(mt_ctrl->unit, er_mt_data->req_cmd, REG_PORT_ANY,
                            0);
        rv = soc_reg32_read(mt_ctrl->unit, addr, &rval);
        if (SOC_FAILURE(rv)) {
            goto done;
        }
        soc_reg_field_set(mt_ctrl->unit, er_mt_data->req_cmd, &rval, COMMANDf,
                          1);
        rv = soc_reg32_write(mt_ctrl->unit, addr, rval);
        if (SOC_FAILURE(rv)) {
            goto done;
        }

        addr = soc_reg_addr(mt_ctrl->unit, er_mt_data->bist_ctrl, REG_PORT_ANY,
                            0);
        poll_count = 0;
        while (poll_count < mt_ctrl->bist_poll_count) {
            sal_usleep(SOC_ER_RLDRAM_BIST_POLL_INTERVAL);
            rv = soc_reg32_read(mt_ctrl->unit, addr, &rval);
            if (SOC_FAILURE(rv)) {
                goto done;
            }
            if (soc_reg_field_get(mt_ctrl->unit, er_mt_data->bist_ctrl,
                                  rval, BIST_DONEf)) {
                break;
            }
            poll_count++;
        }

        if (poll_count == mt_ctrl->bist_poll_count) {
            return SOC_E_TIMEOUT;
        } else {
            if (!soc_reg_field_get(mt_ctrl->unit, er_mt_data->bist_ctrl,
                                   rval, BIST_PASSf)) {
                mt_ctrl->cur_fail_count++;
                if (mt_ctrl->cur_fail_count >= mt_data->max_fail_count) {
                    break;
                }
            }
        }
    }

done:
    if (NULL != chunk_data) {
        soc_er_dual_dimensional_array_free(chunk_data, SOC_ER_CELLDATA_CHUNKS);
    }
    if (NULL != slice_data) {
        soc_er_dual_dimensional_array_free(slice_data,SOC_ER_CELLDATA_SLICES); 
    }
    if (NULL != columns_data) {
        soc_er_dual_dimensional_array_free(columns_data,SOC_ER_CELLDATA_SLICES); 
    }
    return rv;
}

STATIC int
_soc_er_sram_tune_test_cb(soc_memtune_data_t *mt_data)
{
    soc_memtune_ctrl_t *mt_ctrl = mt_data->mt_ctrl;
    int ix;
    er_ext_sram_entry_t sram_param;

    sram_param.adr_mode = 2; /* INC2 */
    sram_param.latency = (mt_ctrl->em_latency_cur == 7) ? 1 : 0;
    sram_param.em_if_type = 0;
    sram_param.em_fall_rise = 0;

    for (ix = 0; ix < mt_data->test_count; ix++) {
        sram_param.data0[0] = SOC_ER_CELLDATA_DIFF_COLUMN_55; /* d0f0 */
        sram_param.data0[1] = SOC_ER_CELLDATA_DIFF_COLUMN_55; /* d0f1 */
        sram_param.data0[2] = SOC_ER_CELLDATA_DIFF_COLUMN_55; /* d0f2 */
        sram_param.data0[3] = SOC_ER_CELLDATA_DIFF_COLUMN_55; /* d0f3 */
        sram_param.data0[4] = 0; /* d0r0 */
        sram_param.data0[5] = 0; /* d0r1 */
        sram_param.data0[6] = 0; /* d0r2 */
        sram_param.data0[7] = 0; /* d0r3 */

        sram_param.data1[0] = SOC_ER_CELLDATA_DIFF_COLUMN_55; /* d1f0 */
        sram_param.data1[1] = SOC_ER_CELLDATA_DIFF_COLUMN_55; /* d1f1 */
        sram_param.data1[2] = SOC_ER_CELLDATA_DIFF_COLUMN_55; /* d1f2 */
        sram_param.data1[3] = SOC_ER_CELLDATA_DIFF_COLUMN_55; /* d1f3 */
        sram_param.data1[4] = 0; /* d1r0 */
        sram_param.data1[5] = 0; /* d1r1 */
        sram_param.data1[6] = 0; /* d1r2 */
        sram_param.data1[7] = 0; /* d1r3 */

        sram_param.wdoebr = 0xf;
        sram_param.wdoebf = 0x0;
        sram_param.wdmr = 0xf;
        sram_param.wdmf = 0x0;
        sram_param.rdmr = 0xf;
        sram_param.rdmf = 0x0;
        sram_param.addr0 = 1;
        sram_param.addr1 = 0xfffff;
        sram_param.test_mode = 3; /* WW-RR */
        sram_param.w2r_nops = 0x3;
        sram_param.r2w_nops = 0x3;

        SOC_IF_ERROR_RETURN
            (soc_easyrider_ext_sram_enable_set(mt_ctrl->unit,
                                               BCM_EXT_SRAM_HSE_MODE,
                                               TRUE, TRUE));

        SOC_IF_ERROR_RETURN
            (soc_easyrider_ext_sram_op(mt_ctrl->unit, BCM_EXT_SRAM_HSE_MODE,
                                       &sram_param));

        if (sram_param.err_cnt != 0) {
            mt_ctrl->cur_fail_count++;
            if (mt_ctrl->cur_fail_count >= mt_data->max_fail_count) {
                break;
            }
            continue;
        }

        sram_param.data0[0] = SOC_ER_CELLDATA_DIFF_COLUMN_AA; /* d0f0 */
        sram_param.data0[1] = SOC_ER_CELLDATA_DIFF_COLUMN_AA; /* d0f1 */
        sram_param.data0[2] = SOC_ER_CELLDATA_DIFF_COLUMN_AA; /* d0f2 */
        sram_param.data0[3] = SOC_ER_CELLDATA_DIFF_COLUMN_AA; /* d0f3 */
        sram_param.data0[4] = SOC_ER_CELLDATA_DIFF_COLUMN_55; /* d0r0 */
        sram_param.data0[5] = SOC_ER_CELLDATA_DIFF_COLUMN_55; /* d0r1 */
        sram_param.data0[6] = SOC_ER_CELLDATA_DIFF_COLUMN_55; /* d0r2 */
        sram_param.data0[7] = SOC_ER_CELLDATA_DIFF_COLUMN_55; /* d0r3 */

        sram_param.data1[0] = SOC_ER_CELLDATA_DIFF_COLUMN_AA; /* d1f0 */
        sram_param.data1[1] = SOC_ER_CELLDATA_DIFF_COLUMN_AA; /* d1f1 */
        sram_param.data1[2] = SOC_ER_CELLDATA_DIFF_COLUMN_AA; /* d1f2 */
        sram_param.data1[3] = SOC_ER_CELLDATA_DIFF_COLUMN_AA; /* d1f3 */
        sram_param.data1[4] = SOC_ER_CELLDATA_DIFF_COLUMN_55; /* d1r0 */
        sram_param.data1[5] = SOC_ER_CELLDATA_DIFF_COLUMN_55; /* d1r1 */
        sram_param.data1[6] = SOC_ER_CELLDATA_DIFF_COLUMN_55; /* d1r2 */
        sram_param.data1[7] = SOC_ER_CELLDATA_DIFF_COLUMN_55; /* d1r3 */

        sram_param.wdoebr = 0xf;
        sram_param.wdoebf = 0x0;
        sram_param.wdmr = 0xf;
        sram_param.wdmf = 0x0;
        sram_param.rdmr = 0x0;
        sram_param.rdmf = 0x0;
        sram_param.addr0 = 0;
        sram_param.addr1 = 0xffffe;
        sram_param.test_mode = 2; /* WR-WR */
        sram_param.w2r_nops = 0x0;
        sram_param.r2w_nops = 0x1;

        SOC_IF_ERROR_RETURN
            (soc_easyrider_ext_sram_enable_set(mt_ctrl->unit,
                                               BCM_EXT_SRAM_HSE_MODE,
                                               TRUE, TRUE));

        SOC_IF_ERROR_RETURN
            (soc_easyrider_ext_sram_op(mt_ctrl->unit, BCM_EXT_SRAM_HSE_MODE,
                                       &sram_param));

        if (sram_param.err_cnt != 0) {
            mt_ctrl->cur_fail_count++;
            if (mt_ctrl->cur_fail_count >= mt_data->max_fail_count) {
                break;
            }
            continue;
        }

        sram_param.data0[0] = SOC_ER_CELLDATA_DIFF_COLUMN_AA; /* d0f0 */
        sram_param.data0[1] = SOC_ER_CELLDATA_DIFF_COLUMN_AA; /* d0f1 */
        sram_param.data0[2] = SOC_ER_CELLDATA_DIFF_COLUMN_AA; /* d0f2 */
        sram_param.data0[3] = SOC_ER_CELLDATA_DIFF_COLUMN_AA; /* d0f3 */
        sram_param.data0[4] = SOC_ER_CELLDATA_DIFF_COLUMN_55; /* d0r0 */
        sram_param.data0[5] = SOC_ER_CELLDATA_DIFF_COLUMN_55; /* d0r1 */
        sram_param.data0[6] = SOC_ER_CELLDATA_DIFF_COLUMN_55; /* d0r2 */
        sram_param.data0[7] = SOC_ER_CELLDATA_DIFF_COLUMN_55; /* d0r3 */

        sram_param.data1[0] = 0x34455; /* d1f0 */
        sram_param.data1[1] = 0x15544; /* d1f1 */
        sram_param.data1[2] = 0x24433; /* d1f2 */
        sram_param.data1[3] = 0x055aa; /* d1f3 */
        sram_param.data1[4] = 0x0a5a5; /* d1r0 */
        sram_param.data1[5] = 0x1ffff; /* d1r1 */
        sram_param.data1[6] = 0x2a5a5; /* d1r2 */
        sram_param.data1[7] = 0x3ffff; /* d1r3 */

        sram_param.wdoebr = 0x0;
        sram_param.wdoebf = 0x0;
        sram_param.wdmr = 0x0;
        sram_param.wdmf = 0x0;
        sram_param.rdmr = 0x0;
        sram_param.rdmf = 0x0;
        sram_param.addr0 = 0;
        sram_param.addr1 = 0xffffe;
        sram_param.test_mode = 3; /* WW_RR */
        sram_param.w2r_nops = 0x3;
        sram_param.r2w_nops = 0x3;

        SOC_IF_ERROR_RETURN
            (soc_easyrider_ext_sram_enable_set(mt_ctrl->unit,
                                               BCM_EXT_SRAM_HSE_MODE,
                                               TRUE, TRUE));

        SOC_IF_ERROR_RETURN
            (soc_easyrider_ext_sram_op(mt_ctrl->unit, BCM_EXT_SRAM_HSE_MODE,
                                       &sram_param));

        if (sram_param.err_cnt != 0) {
            mt_ctrl->cur_fail_count++;
            if (mt_ctrl->cur_fail_count >= mt_data->max_fail_count) {
                break;
            }
        }
    }

    SOC_IF_ERROR_RETURN
        (soc_easyrider_ext_sram_enable_set(mt_ctrl->unit,
                                           BCM_EXT_SRAM_HSE_MODE,
                                           FALSE, TRUE));

    return SOC_E_NONE;
}

STATIC int
_soc_er_add_even_par(int byte)
{
    return ((0x699600 >> ((byte ^ (byte >> 4)) & 0x0f)) & 0x100) | byte;
}

STATIC int
_soc_er_cyp_tcam_tune_test_cb(soc_memtune_data_t *mt_data)
{
    soc_memtune_ctrl_t *mt_ctrl = mt_data->mt_ctrl;
    int ix;
    er_ext_sram_entry_t sram_param;
    int temp_d0r00, temp_d0r01, temp_d0f00, temp_d0f01;
    int temp_d1r00, temp_d1r01, temp_d1f00, temp_d1f01;
    int d0r0, d0r1, d0f0, d0f1;
    int d1r0, d1r1, d1f0, d1f1;

    SOC_IF_ERROR_RETURN
        (soc_easyrider_ext_sram_enable_set(mt_ctrl->unit,
                                           BCM_EXT_SRAM_CSE_MODE,
                                           TRUE, TRUE));

    sram_param.adr_mode = 1; /* INC1 */
    sram_param.test_mode = 3; /* WW-RR */
    sram_param.latency = (mt_ctrl->em_latency_cur == 7) ? 1 : 0;

    sram_param.wdoebr = 0x2;
    sram_param.wdoebf = 0x2;
    sram_param.wdmr = 0x0;
    sram_param.wdmf = 0x0;
    sram_param.rdmr = 0x1;
    sram_param.rdmf = 0x1;

    sram_param.addr0 = 0x4040;
    sram_param.addr1 = 0x4043;

    sram_param.em_if_type = 1;
    sram_param.em_fall_rise = 0;
    sram_param.w2r_nops = 0x3;
    sram_param.r2w_nops = 0x3;

    for (ix = 0; ix < mt_data->test_count; ix++) {

        /* clean slate */
        temp_d0r00 = _soc_er_add_even_par(0);
        temp_d0r01 = _soc_er_add_even_par(0);
        temp_d0f00 = _soc_er_add_even_par(0);
        temp_d0f01 = _soc_er_add_even_par(0);

        temp_d1r00 = _soc_er_add_even_par(0);
        temp_d1r01 = _soc_er_add_even_par(0);
        temp_d1f00 = _soc_er_add_even_par(0);
        temp_d1f01 = _soc_er_add_even_par(0);

        d0r0 = d0r1 = (temp_d0r01 << 9) | temp_d0r00;
        d0f0 = d0f1 = (temp_d0f01 << 9) | temp_d0f00;
        d1r0 = d1r1 = (temp_d0r01 << 9) | temp_d0r00;
        d1f0 = d1f1 = (temp_d1f01 << 9) | temp_d1f00;

        sram_param.data0[0] = d0f0; /* d0f0 */
        sram_param.data0[1] = d0f1; /* d0f1 */
        sram_param.data0[2] = d0r0; /* d0r0 */
        sram_param.data0[3] = d0r1; /* d0r1 */

        sram_param.data1[0] = d1f0; /* d1f0 */
        sram_param.data1[1] = d1f1; /* d1f1 */
        sram_param.data1[2] = d1r0; /* d1r0 */
        sram_param.data1[3] = d1r1; /* d1r1 */

        SOC_IF_ERROR_RETURN
            (soc_easyrider_ext_sram_enable_set(mt_ctrl->unit,
                                               BCM_EXT_SRAM_CSE_MODE,
                                               TRUE, TRUE));
        SOC_IF_ERROR_RETURN
            (soc_easyrider_ext_sram_op(mt_ctrl->unit, BCM_EXT_SRAM_CSE_MODE,
                                       &sram_param));

        if (sram_param.err_cnt != 0) {
            mt_ctrl->cur_fail_count++;
            if (mt_ctrl->cur_fail_count >= mt_data->max_fail_count) {
                break;
            }
            continue;
        }

        /* now test the real data */
        temp_d0r00 = _soc_er_add_even_par(0xaa);
        temp_d0r01 = _soc_er_add_even_par(0x55);
        temp_d0f00 = _soc_er_add_even_par(0x00);
        temp_d0f01 = _soc_er_add_even_par(0xff);

        temp_d1r00 = _soc_er_add_even_par(0x55);
        temp_d1r01 = _soc_er_add_even_par(0xaa);
        temp_d1f00 = _soc_er_add_even_par(0xff);
        temp_d1f01 = _soc_er_add_even_par(0x00);

        d0r0 = d0r1 = (temp_d0r01 << 9) | temp_d0r00;
        d0f0 = d0f1 = (temp_d0f01 << 9) | temp_d0f00;
        d1r0 = d1r1 = (temp_d1r01 << 9) | temp_d1r00;
        d1f0 = d1f1 = (temp_d1f01 << 9) | temp_d1f00;

        sram_param.data0[0] = d0f0; /* d0f0 */
        sram_param.data0[1] = d0f1; /* d0f1 */
        sram_param.data0[2] = d0r0; /* d0r0 */
        sram_param.data0[3] = d0r1; /* d0r1 */

        sram_param.data1[0] = d1f0; /* d1f0 */
        sram_param.data1[1] = d1f1; /* d1f1 */
        sram_param.data1[2] = d1r0; /* d1r0 */
        sram_param.data1[3] = d1r1; /* d1r1 */

        SOC_IF_ERROR_RETURN
            (soc_easyrider_ext_sram_enable_set(mt_ctrl->unit,
                                               BCM_EXT_SRAM_CSE_MODE,
                                               TRUE, TRUE));
        SOC_IF_ERROR_RETURN
            (soc_easyrider_ext_sram_op(mt_ctrl->unit, BCM_EXT_SRAM_CSE_MODE,
                                       &sram_param));

        if (sram_param.err_cnt != 0) {
            mt_ctrl->cur_fail_count++;
            if (mt_ctrl->cur_fail_count >= mt_data->max_fail_count) {
                break;
            }
        }
    }

    SOC_IF_ERROR_RETURN
        (soc_easyrider_ext_sram_enable_set(mt_ctrl->unit,
                                           BCM_EXT_SRAM_CSE_MODE,
                                           FALSE, TRUE));

    return SOC_E_NONE;
}

STATIC int
_soc_er_nl_tcam_tune_test_cb(soc_memtune_data_t *mt_data)
{
    soc_memtune_ctrl_t *mt_ctrl = mt_data->mt_ctrl;
    int ix;

    for (ix = 0; ix < mt_data->test_count; ix++) {
        if (soc_er_tcam_type1_memtest(mt_ctrl->unit, 0x15555, 0x0aaaa) != 0) {
            mt_ctrl->cur_fail_count++;
            if (mt_ctrl->cur_fail_count >= mt_data->max_fail_count) {
                break;
            }
            continue;
        }
        if (soc_er_tcam_type1_memtest(mt_ctrl->unit, 0x00000, 0x1ffff) != 0) {
            mt_ctrl->cur_fail_count++;
            if (mt_ctrl->cur_fail_count >= mt_data->max_fail_count) {
                break;
            }
            continue;
        }
    }

    return SOC_E_NONE;
}

STATIC int
_soc_er_tb2_sram_tune_test_cb(soc_memtune_data_t *mt_data)
{
    soc_memtune_ctrl_t *mt_ctrl = mt_data->mt_ctrl;
    int ix;
    er_ext_sram_entry_t sram_param;

    SOC_IF_ERROR_RETURN
        (soc_easyrider_ext_sram_enable_set(mt_ctrl->unit,
                                           BCM_EXT_SRAM_CSE_MODE,
                                           TRUE, TRUE));

    sram_param.adr_mode = 2; /* INC2 */
    sram_param.latency = (mt_ctrl->em_latency_cur == 7) ? 1 : 0;

    sram_param.em_if_type = 0;
    sram_param.em_fall_rise = 0;

    for (ix = 0; ix < mt_data->test_count; ix++) {

        sram_param.data0[0] = SOC_ER_CELLDATA_DIFF_COLUMN_55; /* d0f0 */
        sram_param.data0[1] = SOC_ER_CELLDATA_DIFF_COLUMN_55; /* d0f1 */
        sram_param.data0[2] = 0; /* d0r0 */
        sram_param.data0[3] = 0; /* d0r1 */

        sram_param.data1[0] = SOC_ER_CELLDATA_DIFF_COLUMN_55; /* d1f0 */
        sram_param.data1[1] = SOC_ER_CELLDATA_DIFF_COLUMN_55; /* d1f1 */
        sram_param.data1[2] = 0; /* d1r0 */
        sram_param.data1[3] = 0; /* d1r1 */

        sram_param.addr0 = 1;
        sram_param.addr1 = 0xfffff;

        sram_param.wdoebr = 0x3;
        sram_param.wdoebf = 0x0;
        sram_param.wdmr = 0x3;
        sram_param.wdmf = 0x0;
        sram_param.rdmr = 0x3;
        sram_param.rdmf = 0x0;
        sram_param.w2r_nops = 0x3;
        sram_param.r2w_nops = 0x3;
        sram_param.test_mode = 3; /* WW-RR */

        SOC_IF_ERROR_RETURN
            (soc_easyrider_ext_sram_enable_set(mt_ctrl->unit,
                                               BCM_EXT_SRAM_CSE_MODE,
                                               TRUE, TRUE));
        SOC_IF_ERROR_RETURN
            (soc_easyrider_ext_sram_op(mt_ctrl->unit, BCM_EXT_SRAM_CSE_MODE,
                                       &sram_param));

        if (sram_param.err_cnt != 0) {
            mt_ctrl->cur_fail_count++;
            if (mt_ctrl->cur_fail_count >= mt_data->max_fail_count) {
                break;
            }
            continue;
        }

        sram_param.data0[0] = SOC_ER_CELLDATA_DIFF_COLUMN_AA; /* d0f0 */
        sram_param.data0[1] = SOC_ER_CELLDATA_DIFF_COLUMN_AA; /* d0f1 */
        sram_param.data0[2] = SOC_ER_CELLDATA_DIFF_COLUMN_55; /* d0r0 */
        sram_param.data0[3] = SOC_ER_CELLDATA_DIFF_COLUMN_55; /* d0r1 */

        sram_param.data1[0] = SOC_ER_CELLDATA_DIFF_COLUMN_AA; /* d1f0 */
        sram_param.data1[1] = SOC_ER_CELLDATA_DIFF_COLUMN_AA; /* d1f1 */
        sram_param.data1[2] = SOC_ER_CELLDATA_DIFF_COLUMN_55; /* d1r0 */
        sram_param.data1[3] = SOC_ER_CELLDATA_DIFF_COLUMN_55; /* d1r1 */

        sram_param.addr0 = 0;
        sram_param.addr1 = 0xffffe;

        sram_param.wdoebr = 0x3;
        sram_param.wdoebf = 0x0;
        sram_param.wdmr = 0x3;
        sram_param.wdmf = 0x0;
        sram_param.rdmr = 0x0;
        sram_param.rdmf = 0x0;
        sram_param.w2r_nops = 0x0;
        sram_param.r2w_nops = 0x1;
        sram_param.test_mode = 2; /* WR-WR */

        SOC_IF_ERROR_RETURN
            (soc_easyrider_ext_sram_enable_set(mt_ctrl->unit,
                                               BCM_EXT_SRAM_CSE_MODE,
                                               TRUE, TRUE));
        SOC_IF_ERROR_RETURN
            (soc_easyrider_ext_sram_op(mt_ctrl->unit, BCM_EXT_SRAM_CSE_MODE,
                                       &sram_param));

        if (sram_param.err_cnt != 0) {
            mt_ctrl->cur_fail_count++;
            if (mt_ctrl->cur_fail_count >= mt_data->max_fail_count) {
                break;
            }
            continue;
        }

        sram_param.data0[0] = 0; /* d0f0 */
        sram_param.data0[1] = 0; /* d0f1 */
        sram_param.data0[2] = 0; /* d0r0 */
        sram_param.data0[3] = 0; /* d0r1 */

        sram_param.data1[0] = 0; /* d1f0 */
        sram_param.data1[1] = 0; /* d1f1 */
        sram_param.data1[2] = 0; /* d1r0 */
        sram_param.data1[3] = 0; /* d1r1 */

        sram_param.addr0 = 0;
        sram_param.addr1 = 0xffffe;

        sram_param.wdoebr = 0x0;
        sram_param.wdoebf = 0x0;
        sram_param.wdmr = 0x0;
        sram_param.wdmf = 0x0;
        sram_param.rdmr = 0x0;
        sram_param.rdmf = 0x0;
        sram_param.w2r_nops = 0x3;
        sram_param.r2w_nops = 0x3;
        sram_param.test_mode = 0; /* WW */

        SOC_IF_ERROR_RETURN
            (soc_easyrider_ext_sram_enable_set(mt_ctrl->unit,
                                               BCM_EXT_SRAM_CSE_MODE,
                                               TRUE, TRUE));
        SOC_IF_ERROR_RETURN
            (soc_easyrider_ext_sram_op(mt_ctrl->unit, BCM_EXT_SRAM_CSE_MODE,
                                       &sram_param));

        if (sram_param.err_cnt != 0) {
            mt_ctrl->cur_fail_count++;
            if (mt_ctrl->cur_fail_count >= mt_data->max_fail_count) {
                break;
            }
            continue;
        }

        sram_param.data0[0] = SOC_ER_CELLDATA_DIFF_COLUMN_AA; /* d0f0 */
        sram_param.data0[1] = SOC_ER_CELLDATA_DIFF_COLUMN_AA; /* d0f1 */
        sram_param.data0[2] = SOC_ER_CELLDATA_DIFF_COLUMN_55; /* d0r0 */
        sram_param.data0[3] = SOC_ER_CELLDATA_DIFF_COLUMN_55; /* d0r1 */

        sram_param.data1[0] = 0x34455; /* d1f0 */
        sram_param.data1[1] = 0x15544; /* d1f1 */
        sram_param.data1[2] = 0x0A5A5; /* d1r0 */
        sram_param.data1[3] = 0x1FFFF; /* d1r1 */

        sram_param.addr0 = 0;
        sram_param.addr1 = 0xffffe;

        sram_param.wdoebr = 0x0;
        sram_param.wdoebf = 0x0;
        sram_param.wdmr = 0x0;
        sram_param.wdmf = 0x0;
        sram_param.rdmr = 0x0;
        sram_param.rdmf = 0x0;
        sram_param.w2r_nops = 0x3;
        sram_param.r2w_nops = 0x3;
        sram_param.test_mode = 3; /* WW-RR */

        SOC_IF_ERROR_RETURN
            (soc_easyrider_ext_sram_enable_set(mt_ctrl->unit,
                                               BCM_EXT_SRAM_CSE_MODE,
                                               TRUE, TRUE));
        SOC_IF_ERROR_RETURN
            (soc_easyrider_ext_sram_op(mt_ctrl->unit, BCM_EXT_SRAM_CSE_MODE,
                                       &sram_param));

        if (sram_param.err_cnt != 0) {
            mt_ctrl->cur_fail_count++;
            if (mt_ctrl->cur_fail_count >= mt_data->max_fail_count) {
                break;
            }
        }
    }

    SOC_IF_ERROR_RETURN
        (soc_easyrider_ext_sram_enable_set(mt_ctrl->unit,
                                           BCM_EXT_SRAM_CSE_MODE,
                                           FALSE, TRUE));

    return SOC_E_NONE;
}

STATIC int
_soc_er_tune_generate_config(soc_memtune_data_t *mt_data)
{
#ifndef NO_MEMTUNE
    soc_memtune_ctrl_t *mt_ctrl = mt_data->mt_ctrl;
    char config_str[64], val_str[10], *intf_str, *chn_str, *seer_str;
    uint32 ix, lat_bit1, lat_bit2, iter_count=4;

    /* *** Weird translation *** */
    lat_bit1 = (mt_ctrl->ddr_latency_cur == 1) ? 1 : 0;
    lat_bit2 = (mt_ctrl->ddr_latency_cur == 0) ? 1 : 0;

    switch (mt_data->interface) {
    case SOC_MEM_INTERFACE_RLDRAM_CH0:
        chn_str = "_ch0";
        intf_str = "mcu";
        seer_str = "";
        iter_count = 4;
        break;
    case SOC_MEM_INTERFACE_RLDRAM_CH1:
        chn_str = "_ch1";
        intf_str = "mcu";
        seer_str = "";
        iter_count = 4;
        break;
    case SOC_MEM_INTERFACE_SRAM:
        chn_str = "";
        intf_str = "ddr72";
        seer_str = "hse";
        iter_count = 4;
        break;
    case SOC_MEM_INTERFACE_QDR:
        chn_str = "";
        intf_str = "qdr36";
        seer_str = "cse";
        iter_count = 2;
        break;
    default:
        return SOC_E_PARAM;
    }

    sal_sprintf(val_str, "%d", lat_bit2);
    for (ix = 0; ix < iter_count; ix ++) {
        sal_sprintf(config_str, "%s_sel_early2_%d%s", intf_str, ix, chn_str);
        if (sal_config_set(config_str, val_str) < 0) {
            return SOC_E_MEMORY;
        }
    }

    sal_sprintf(val_str, "%d", lat_bit1);
    for (ix = 0; ix < iter_count; ix ++) {
        sal_sprintf(config_str, "%s_sel_early1_%d%s", intf_str, ix, chn_str);
        if (sal_config_set(config_str, val_str) < 0) {
            return SOC_E_MEMORY;
        }
    }

    sal_sprintf(val_str, "%d", mt_ctrl->rx_offset_cur);
    if (mt_data->interface == SOC_MEM_INTERFACE_QDR) {
        sal_sprintf(config_str, "%s_dll90_offset_qk%s", intf_str, chn_str);
        if (sal_config_set(config_str, val_str) < 0) {
            return SOC_E_MEMORY;
        }
    } else {
        sal_sprintf(config_str, "%s_dll90_offset0_qk%s", intf_str, chn_str);
        if (sal_config_set(config_str, val_str) < 0) {
            return SOC_E_MEMORY;
        }
        for (ix = 1; ix < 4; ix ++) {
            sal_sprintf(config_str, "%s_dll90_offset%d%s", intf_str, ix,
                        chn_str);
            if (sal_config_set(config_str, val_str) < 0) {
                return SOC_E_MEMORY;
            }
        }
    }
    sal_sprintf(config_str, "%s_dll90_offset_qkb%s", intf_str, chn_str);
    if (sal_config_set(config_str, val_str) < 0) {
        return SOC_E_MEMORY;
    }

    sal_sprintf(val_str, "%d", mt_ctrl->tx_offset_cur);
    sal_sprintf(config_str, "%s_dll90_offset_tx%s", intf_str, chn_str);
    if (sal_config_set(config_str, val_str) < 0) {
        return SOC_E_MEMORY;
    }

    if (mt_ctrl->em_latency_cur != -1) {
        sal_sprintf(val_str, "%d", (mt_ctrl->em_latency_cur == 7) ? 1 : 0);
        sal_sprintf(config_str, "seer_%s_em_latency7", seer_str);
        if (sal_config_set(config_str, val_str) < 0) {
            return SOC_E_MEMORY;
        }
    }

    if (mt_ctrl->phase_sel_cur != -1) {
        sal_sprintf(config_str, "%s_ovrd_sm_en%s", intf_str, chn_str);
        sal_sprintf(val_str, "%d", 1);
        if (sal_config_set(config_str, val_str) < 0) {
            return SOC_E_MEMORY;
        }

        sal_sprintf(config_str, "%s_phase_sel%s", intf_str, chn_str);
        sal_sprintf(val_str, "%d", mt_ctrl->phase_sel_cur);
        if (sal_config_set(config_str, val_str) < 0) {
            return SOC_E_MEMORY;
        }
    }
#endif  /* NO_MEMTUNE */

    return SOC_E_NONE;
}
#endif /* BCM_EASYRIDER_SUPPORT */

#ifdef BCM_TRIUMPH_SUPPORT
STATIC int
_soc_tr_phase_sel_prog_hw_cb(soc_memtune_data_t *mt_data)
{
    soc_memtune_ctrl_t *mt_ctrl = mt_data->mt_ctrl;
    soc_tr_memtune_data_t *tr_mt_data = mt_ctrl->data;
    uint32 oval, rval, addr;

    if (mt_ctrl->phase_sel_cur != -1) {
        addr = soc_reg_addr(mt_ctrl->unit, tr_mt_data->config_reg3,
                            REG_PORT_ANY, 0);
        SOC_IF_ERROR_RETURN(soc_reg32_read(mt_ctrl->unit, addr, &rval));
        oval = rval;
        soc_reg_field_set(mt_ctrl->unit, tr_mt_data->config_reg3, &rval,
                          PHASE_SELf, mt_ctrl->phase_sel_cur);
        soc_reg_field_set(mt_ctrl->unit, tr_mt_data->config_reg3, &rval,
                          OVRD_SM_ENf, 1);
        if (rval != oval) {
            SOC_IF_ERROR_RETURN(soc_reg32_write(mt_ctrl->unit, addr, rval));
        }
    }

    return SOC_E_NONE;
}

STATIC int
_soc_tr_ddr_prog_hw_cb(soc_memtune_data_t *mt_data)
{
    soc_memtune_ctrl_t *mt_ctrl = mt_data->mt_ctrl;
    soc_tr_memtune_data_t *tr_mt_data = mt_ctrl->data;
    uint32 oval, rval, addr, sel_early2, sel_early1;

    if (mt_ctrl->em_latency_cur != -1) {
        addr = soc_reg_addr(mt_ctrl->unit, tr_mt_data->tmode0, REG_PORT_ANY,
                            0);
        SOC_IF_ERROR_RETURN(soc_reg32_read(mt_ctrl->unit, addr, &rval));
        oval = rval;
        soc_reg_field_set(mt_ctrl->unit, tr_mt_data->tmode0, &rval,
                          EM_LATENCYf, mt_ctrl->em_latency_cur & 7);
        if (rval != oval) {
            SOC_IF_ERROR_RETURN(soc_reg32_write(mt_ctrl->unit, addr, rval));
        }
    }

    if (mt_ctrl->ddr_latency_cur != -1) {
        /*
         * DDR latency settings
         * if latency == 0, select result from 2 steps earlier
         * if latency == 1, select result from 1 step earlier
         * if latency == 2, select regular result
         */
        sel_early1 = 0;
        sel_early2 = 0;
        if (mt_ctrl->ddr_latency_cur == 0) {
            sel_early2 = 1;
        } else if (mt_ctrl->ddr_latency_cur == 1) {
            sel_early1 = 1;
        }

        addr = soc_reg_addr(mt_ctrl->unit, tr_mt_data->config_reg2,
                            REG_PORT_ANY, 0);
        SOC_IF_ERROR_RETURN(soc_reg32_read(mt_ctrl->unit, addr, &rval));
        oval = rval;
        if (mt_data->slice_mask & 0x1) {
            soc_reg_field_set(mt_ctrl->unit, tr_mt_data->config_reg2, &rval,
                              SEL_EARLY2_0f, sel_early2);
            soc_reg_field_set(mt_ctrl->unit, tr_mt_data->config_reg2, &rval,
                              SEL_EARLY1_0f, sel_early1);
        }
        if (mt_data->slice_mask & 0x2) {
            soc_reg_field_set(mt_ctrl->unit, tr_mt_data->config_reg2, &rval,
                              SEL_EARLY2_1f, sel_early2);
            soc_reg_field_set(mt_ctrl->unit, tr_mt_data->config_reg2, &rval,
                              SEL_EARLY1_1f, sel_early1);
        }
        if (rval != oval) {
            SOC_IF_ERROR_RETURN(soc_reg32_write(mt_ctrl->unit, addr, rval));
        }
    }

    addr = soc_reg_addr(mt_ctrl->unit, tr_mt_data->config_reg1, REG_PORT_ANY,
                        0);
    SOC_IF_ERROR_RETURN(soc_reg32_read(mt_ctrl->unit, addr, &rval));
    oval = rval;

    /* TX phase settings */
    if (mt_ctrl->tx_offset_cur != -1) {
        soc_reg_field_set(mt_ctrl->unit, tr_mt_data->config_reg1, &rval,
                          DLL90_OFFSET_TXf, mt_ctrl->tx_offset_cur & 0x0f);
        soc_reg_field_set(mt_ctrl->unit, tr_mt_data->config_reg1, &rval,
                          DLL90_OFFSET_TX4f, mt_ctrl->tx_offset_cur >> 4);
    }

    /* RX phase settings */
    if (mt_ctrl->rx_offset_cur != -1) {
        soc_reg_field_set(mt_ctrl->unit, tr_mt_data->config_reg1, &rval,
                          DLL90_OFFSET_QKf, mt_ctrl->rx_offset_cur & 0xf);
        soc_reg_field_set(mt_ctrl->unit, tr_mt_data->config_reg1, &rval,
                          DLL90_OFFSET_QKBf, mt_ctrl->rx_offset_cur & 0xf);
        soc_reg_field_set(mt_ctrl->unit, tr_mt_data->config_reg1, &rval,
                          DLL90_OFFSET_QK4f, mt_ctrl->rx_offset_cur >> 4);
        soc_reg_field_set(mt_ctrl->unit, tr_mt_data->config_reg1, &rval,
                          DLL90_OFFSET_QKB4f, mt_ctrl->rx_offset_cur >> 4);
    }

    if (rval != oval) {
        SOC_IF_ERROR_RETURN(soc_reg32_write(mt_ctrl->unit, addr, rval));
    }

    return SOC_E_NONE;
}

STATIC int
_soc_tr_ddr_nops_prog_hw_cb(soc_memtune_data_t *mt_data)
{
    soc_memtune_ctrl_t *mt_ctrl = mt_data->mt_ctrl;
    soc_tr_memtune_data_t *tr_mt_data = mt_ctrl->data;
    uint32 oval, rval, addr;

    addr = soc_reg_addr(mt_ctrl->unit, tr_mt_data->sram_ctl, REG_PORT_ANY, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_read(mt_ctrl->unit, addr, &rval));
    oval = rval;
    if (mt_ctrl->r2w_nops_cur != -1) {
        soc_reg_field_set(mt_ctrl->unit, tr_mt_data->sram_ctl, &rval,
                          NUM_R2W_NOPSf, mt_ctrl->r2w_nops_cur);
    }
    if (mt_ctrl->w2r_nops_cur != -1) {
        soc_reg_field_set(mt_ctrl->unit, tr_mt_data->sram_ctl, &rval,
                          NUM_W2R_NOPSf, mt_ctrl->w2r_nops_cur);
    }
    if (rval != oval) {
        SOC_IF_ERROR_RETURN(soc_reg32_write(mt_ctrl->unit, addr, rval));
    }

    return SOC_E_NONE;
}

STATIC int
_soc_tr_tcam_prog_hw_cb(soc_memtune_data_t *mt_data)
{
    soc_memtune_ctrl_t *mt_ctrl = mt_data->mt_ctrl;
    soc_tr_memtune_data_t *tr_mt_data = mt_ctrl->data;
    uint32 oval, rval, addr;

    addr = soc_reg_addr(mt_ctrl->unit, tr_mt_data->config_reg1, REG_PORT_ANY,
                        0);
    SOC_IF_ERROR_RETURN(soc_reg32_read(mt_ctrl->unit, addr, &rval));
    oval = rval;
    if (mt_ctrl->tx_offset_cur != -1) {
        soc_reg_field_set(mt_ctrl->unit, tr_mt_data->config_reg1, &rval,
                          VCDL_TX_OFFSETf, mt_ctrl->tx_offset_cur & 0x1f);
        soc_reg_field_set(mt_ctrl->unit, tr_mt_data->config_reg1, &rval,
                          MSBMIDL_OFFSET_TXf, mt_ctrl->tx_offset_cur >> 5);
    }
    if (mt_ctrl->rx_offset_cur != -1) {
        soc_reg_field_set(mt_ctrl->unit, tr_mt_data->config_reg1, &rval,
                          VCDL_RX_OFFSETf, mt_ctrl->rx_offset_cur & 0x1f);
        soc_reg_field_set(mt_ctrl->unit, tr_mt_data->config_reg1, &rval,
                          MSBMIDL_OFFSET_RXf, mt_ctrl->rx_offset_cur >> 5);
    }
    if (rval != oval) {
        SOC_IF_ERROR_RETURN(soc_reg32_write(mt_ctrl->unit, addr, rval));
    }

    addr = soc_reg_addr(mt_ctrl->unit, tr_mt_data->config_reg3, REG_PORT_ANY,
                        0);
    SOC_IF_ERROR_RETURN(soc_reg32_read(mt_ctrl->unit, addr, &rval));
    oval = rval;
    if (mt_ctrl->fcd_dpeo_cur != -1) {
        /*
         * DPEO_0 is connected to DPEO pin of first TCAM
         * DPEO_1 is connected to DPEO pin of optional second TCAM
         */
        if (!mt_ctrl->dpeo_sel_cur) {
            soc_reg_field_set(mt_ctrl->unit, tr_mt_data->config_reg3, &rval,
                              FCD_DPEO_0f, mt_ctrl->fcd_dpeo_cur);
        } else {
            soc_reg_field_set(mt_ctrl->unit, tr_mt_data->config_reg3, &rval,
                              FCD_DPEO_1f, mt_ctrl->fcd_dpeo_cur);
        }
    }
    if (mt_ctrl->fcd_rbus_cur != -1) {
        /* use fcd_rbus + 1 for FCD_SMFL_0 and FCD_SMFL_1 */
        soc_reg_field_set(mt_ctrl->unit, tr_mt_data->config_reg3, &rval,
                          FCD_SMFL_0f, mt_ctrl->fcd_rbus_cur + 1);
        soc_reg_field_set(mt_ctrl->unit, tr_mt_data->config_reg3, &rval,
                          FCD_SMFL_1f, mt_ctrl->fcd_rbus_cur + 1);
        /* use same value for FCD_RBUS and FCD_RV */
        soc_reg_field_set(mt_ctrl->unit, tr_mt_data->config_reg3, &rval,
                          FCD_RBUSf, mt_ctrl->fcd_rbus_cur);
        soc_reg_field_set(mt_ctrl->unit, tr_mt_data->config_reg3, &rval,
                          FCD_RVf, mt_ctrl->fcd_rbus_cur);
    }
    if (rval != oval) {
        SOC_IF_ERROR_RETURN(soc_reg32_write(mt_ctrl->unit, addr, rval));
    }

    addr = soc_reg_addr(mt_ctrl->unit, tr_mt_data->etc_ctl, REG_PORT_ANY, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_read(mt_ctrl->unit, addr, &rval));
    oval = rval;
    if (mt_ctrl->rbus_sync_dly_cur != -1) {
        soc_reg_field_set(mt_ctrl->unit, tr_mt_data->etc_ctl, &rval,
                          RBUS_SYNC_DLYf, mt_ctrl->rbus_sync_dly_cur);
    }
    if (mt_ctrl->dpeo_sync_dly_cur != -1) {
        soc_reg_field_set(mt_ctrl->unit, tr_mt_data->etc_ctl, &rval,
                          DPEO_SYNC_DLYf, mt_ctrl->dpeo_sync_dly_cur);
    }
    if (rval != oval) {
        SOC_IF_ERROR_RETURN(soc_reg32_write(mt_ctrl->unit, addr, rval));
    }

    return SOC_E_NONE;
}

STATIC int
_soc_tr_sram_tune_test_cb(soc_memtune_data_t *mt_data)
{
    soc_memtune_ctrl_t *mt_ctrl = mt_data->mt_ctrl;
    tr_ext_sram_bist_t *sram_bist = mt_ctrl->sram_bist;
    int loop_mode, i;

    sram_bist->w2r_nops = mt_ctrl->w2r_nops_cur;
    sram_bist->r2w_nops = mt_ctrl->r2w_nops_cur;
    sram_bist->bg_tcam_loop_count = 1;

    for (i = 0; i < mt_data->test_count; i++) {
        for (loop_mode = 1; loop_mode < 4; loop_mode++) {
            if (loop_mode == 1) { /* RR */
                sram_bist->loop_mode = 0; /* WW */
                SOC_IF_ERROR_RETURN
                    (soc_triumph_ext_sram_enable_set(mt_ctrl->unit,
                                                     mt_data->sub_interface,
                                                     TRUE, TRUE));
                SOC_IF_ERROR_RETURN
                    (soc_triumph_ext_sram_bist_setup(mt_ctrl->unit,
                                                     mt_data->sub_interface,
                                                     sram_bist));
                SOC_IF_ERROR_RETURN
                    (soc_triumph_ext_sram_op(mt_ctrl->unit,
                                             mt_data->sub_interface,
                                             sram_bist, NULL));
            }
            sram_bist->loop_mode = loop_mode;
            SOC_IF_ERROR_RETURN
                (soc_triumph_ext_sram_enable_set(mt_ctrl->unit,
                                                 mt_data->sub_interface,
                                                 TRUE, TRUE));
            SOC_IF_ERROR_RETURN
                (soc_triumph_ext_sram_bist_setup(mt_ctrl->unit,
                                                 mt_data->sub_interface,
                                                 sram_bist));
            SOC_IF_ERROR_RETURN
                (soc_triumph_ext_sram_op(mt_ctrl->unit, mt_data->sub_interface,
                                         sram_bist, NULL));
            SOC_IF_ERROR_RETURN
                (soc_triumph_ext_sram_enable_set(mt_ctrl->unit,
                                                 mt_data->sub_interface,
                                                 FALSE, FALSE));
            if (sram_bist->err_cnt) {
                mt_ctrl->cur_fail_count++;
                if (mt_ctrl->cur_fail_count >= mt_data->max_fail_count) {
                    return SOC_E_NONE;
                }
                continue;
            }
        }
    }

    if (!mt_ctrl->cur_fail_count) {
        SOC_IF_ERROR_RETURN
            (soc_triumph_ext_sram_enable_set(mt_ctrl->unit,
                                             mt_data->sub_interface,
                                             TRUE, TRUE));
        sram_bist->loop_mode = 3; /* WW_RR */
        sram_bist->bg_tcam_loop_count =
            mt_data->bg_tcam_bist ? mt_data->bg_tcam_loop_count : 1;
        SOC_IF_ERROR_RETURN
            (soc_triumph_ext_sram_bist_setup(mt_ctrl->unit,
                                             mt_data->sub_interface,
                                             sram_bist));
        SOC_IF_ERROR_RETURN
            (soc_triumph_ext_sram_op(mt_ctrl->unit, mt_data->sub_interface,
                                     sram_bist, NULL));
        SOC_IF_ERROR_RETURN
            (soc_triumph_ext_sram_enable_set(mt_ctrl->unit,
                                             mt_data->sub_interface,
                                             FALSE, FALSE));
        if (sram_bist->err_cnt) {
            mt_ctrl->cur_fail_count++;
        }
    }

    return SOC_E_NONE;
}

STATIC int
_soc_tr_tcam_tune_test_cb(soc_memtune_data_t *mt_data)
{
    soc_memtune_ctrl_t *mt_ctrl = mt_data->mt_ctrl;
    soc_tr_memtune_data_t *tr_mt_data = mt_ctrl->data;
    uint32 rval;
    int rv, addr, i;
    uint32 exp_dpeo_rise, exp_dpeo_fall;
    uint32 pattern[4];
    uint32 data[8], data_miss[8];
    int index, exp_result0, exp_result1, result0, result1, valid;
    soc_mem_t mem;
    uint32 mask[4];

    addr = soc_reg_addr(mt_ctrl->unit, tr_mt_data->inst_status, REG_PORT_ANY,
                        0);

    switch (mt_data->sub_interface) {
    case 0: /* DBUS */
        /* Use the first few entries in the second block */
        pattern[0] = 0xffffffff;
        pattern[1] = 0x00000000;
        pattern[2] = 0x55555555;
        pattern[3] = 0xaaaaaaaa;
        mask[0] = mask[1] = mask[2] = mask[3] = 0;

        index = 16384;
        for (i = 0; i < 4; i ++) {
            data[0] = 0;
            data[1] = pattern[i] & 0xff;
            data[2] = data[3] = pattern[i];
            SOC_IF_ERROR_RETURN(soc_tcam_write_entry(mt_ctrl->unit,
                                                     TCAM_PARTITION_RAW,
                                                     index + i, mask, data));
        }

        for (i = 0; i < 4; i ++) {
            rv = soc_tcam_read_entry(mt_ctrl->unit, TCAM_PARTITION_RAW,
                                     index + i, mask, data, &valid);
            SOC_IF_ERROR_RETURN(soc_reg32_read(mt_ctrl->unit, addr, &rval));
            if (rv < 0 ||
                soc_reg_field_get(mt_ctrl->unit, tr_mt_data->inst_status, rval,
                                  SEQ_DPEO_ERRf) ||
                soc_reg_field_get(mt_ctrl->unit, tr_mt_data->inst_status, rval,
                                  DPEO_RISEf) ||
                soc_reg_field_get(mt_ctrl->unit, tr_mt_data->inst_status, rval,
                                  DPEO_FALLf) ||
                soc_reg_field_get(mt_ctrl->unit, tr_mt_data->inst_status, rval,
                                  DPRERR0f) ||
                soc_reg_field_get(mt_ctrl->unit, tr_mt_data->inst_status, rval,
                                  DPRERR1f) ||
                data[1] != (pattern[i] & 0xff) || data[2] != pattern[i] ||
                data[3] != pattern[i]) {
                mt_ctrl->cur_fail_count = mt_data->test_count + 1;
                break;
            }
        }
        break;

    case 1: /* DBUS_out (DPEO) */
        sal_memset(data, 0, sizeof(data));
        data[0] = data[4] = 0xabcd;
        SOC_IF_ERROR_RETURN
            (soc_tr_tcam_type1_memtest_dpeo(mt_ctrl->unit, 2, 0x3, data));
        SOC_IF_ERROR_RETURN(soc_reg32_read(mt_ctrl->unit, addr, &rval));
        /*
         * DPEO_0 is connected to DPEO pin of first TCAM
         * DPEO_1 is connected to DPEO pin of optional second TCAM
         */
        if (!mt_ctrl->dpeo_sel_cur) { /* DPEO_0 */
            exp_dpeo_rise = 1;
            exp_dpeo_fall = 1;
        } else { /* DPEO_1 */
            exp_dpeo_rise = 2;
            exp_dpeo_fall = 2;
        }
        if (!soc_reg_field_get(mt_ctrl->unit, tr_mt_data->inst_status, rval,
                               SEQ_DPEO_ERRf)||
            soc_reg_field_get(mt_ctrl->unit, tr_mt_data->inst_status, rval,
                              DPEO_RISEf) != exp_dpeo_rise ||
            soc_reg_field_get(mt_ctrl->unit, tr_mt_data->inst_status, rval,
                              DPEO_FALLf) != exp_dpeo_fall) {
            mt_ctrl->cur_fail_count = mt_data->test_count + 1;
        }
        break;

    case 2: /* RBUS */
        if (mt_data->mask[0] == 0xffffffff) {
            sal_memset(mask, 0, sizeof(mask));
            sal_memset(data, 0, sizeof(data));
            /* all '1's for lower 72-bit, all '0's for upper 72-bit */
            data[5] = 0xff;
            data[6] = data[7] = 0xffffffff;
        } else {
            sal_memcpy(mask, mt_data->mask, sizeof(mask));
            sal_memcpy(data, mt_data->data, sizeof(data));
        }
        sal_memcpy(data_miss, data, sizeof(data_miss));
        data_miss[7] ^= 1; /* use a bogus key to cause a miss */

        /* first do a search miss, ignore result index */
        rv = soc_tcam_search_entry(mt_ctrl->unit, TCAM_PARTITION_FWD_L2,
                                   TCAM_PARTITION_ACL_L2C, data_miss, &result0,
                                   &result1);
        if (SOC_FAILURE(rv) && rv != SOC_E_NOT_FOUND) {
            return rv;
        }
        /* then do a search hit, ignore result index, only check for status */
        rv = soc_tcam_search_entry(mt_ctrl->unit, TCAM_PARTITION_FWD_L2,
                                   TCAM_PARTITION_ACL_L2C, data, &result0,
                                   &result1);
        if (SOC_FAILURE(rv) && rv != SOC_E_NOT_FOUND) {
            return rv;
        }
        SOC_IF_ERROR_RETURN(soc_reg32_read(mt_ctrl->unit, addr, &rval));
        if (soc_reg_field_get(mt_ctrl->unit, tr_mt_data->inst_status, rval,
                              SEQ_DPEO_ERRf) ||
            soc_reg_field_get(mt_ctrl->unit, tr_mt_data->inst_status, rval,
                              RDACKf) ||
            soc_reg_field_get(mt_ctrl->unit, tr_mt_data->inst_status, rval,
                              DPEO_RISEf) ||
            soc_reg_field_get(mt_ctrl->unit, tr_mt_data->inst_status, rval,
                              DPEO_FALLf) ||
            soc_reg_field_get(mt_ctrl->unit, tr_mt_data->inst_status, rval,
                              DPRERR0f) ||
            soc_reg_field_get(mt_ctrl->unit, tr_mt_data->inst_status, rval,
                              DPRERR1f)) {
            mt_ctrl->cur_fail_count = mt_data->test_count + 1;
            break;
        }
        if (soc_reg_field_get(mt_ctrl->unit, tr_mt_data->inst_status, rval,
                              RBUS1_VALIDf)) {
            if (soc_reg_field_get(mt_ctrl->unit, tr_mt_data->inst_status, rval,
                                  RBUS0_VALIDf)) {
                break;
            } else {
                mt_ctrl->cur_fail_count = mt_data->test_count + 3;
                break;
            }
        } else {
            if (soc_reg_field_get(mt_ctrl->unit, tr_mt_data->inst_status, rval,
                                  RBUS0_VALIDf)) {
                mt_ctrl->cur_fail_count = mt_data->test_count + 1;
            } else {
                mt_ctrl->cur_fail_count = mt_data->test_count + 2;
            }
        }
        break;

    case 3: /* search test */
        index = 0x1555;
        if (mt_data->mask[0] == 0xffffffff) {
            sal_memset(mask, 0, sizeof(mask));
            sal_memset(data, 0, sizeof(data));
            /* all '1's for lower 72-bit, all '0's for upper 72-bit */
            data[5] = 0xff;
            data[6] = data[7] = 0xffffffff;
        } else {
            sal_memcpy(mask, mt_data->mask, sizeof(mask));
            sal_memcpy(data, mt_data->data, sizeof(data));
        }
        SOC_IF_ERROR_RETURN
            (soc_tcam_mem_index_to_raw_index(mt_ctrl->unit, EXT_L2_ENTRY_TCAMm,
                                             index, &mem, &exp_result0));
        SOC_IF_ERROR_RETURN
            (soc_tcam_mem_index_to_raw_index(mt_ctrl->unit,
                                             EXT_ACL144_TCAM_L2m,
                                             index, &mem, &exp_result1));

        for (i = 0; i < mt_data->test_count; i++) {
            rv = soc_tcam_search_entry(mt_ctrl->unit, TCAM_PARTITION_FWD_L2,
                                       TCAM_PARTITION_ACL_L2C,
                                       data, &result0, &result1);
            SOC_IF_ERROR_RETURN(soc_reg32_read(mt_ctrl->unit, addr, &rval));
            if (rv < 0 ||
                soc_reg_field_get(mt_ctrl->unit, tr_mt_data->inst_status, rval,
                                  SEQ_DPEO_ERRf) ||
                soc_reg_field_get(mt_ctrl->unit, tr_mt_data->inst_status, rval,
                                  DPEO_RISEf) ||
                soc_reg_field_get(mt_ctrl->unit, tr_mt_data->inst_status, rval,
                                  DPEO_FALLf) ||
                soc_reg_field_get(mt_ctrl->unit, tr_mt_data->inst_status, rval,
                                  DPRERR0f) ||
                soc_reg_field_get(mt_ctrl->unit, tr_mt_data->inst_status, rval,
                                  DPRERR1f) ||
                result0 != exp_result0 || result1 != exp_result1) {
                    mt_ctrl->cur_fail_count++;
                    continue;
            }

            rv = soc_triumph_tcam_search_bist(mt_ctrl->unit,
                                              TCAM_PARTITION_FWD_L2,
                                              TCAM_PARTITION_ACL_L2C,
                                              data, exp_result0, exp_result1,
                                              mt_data->tcam_loop_count);
            if (rv < 0) {
                mt_ctrl->cur_fail_count++;
            }
        }
        break;

    default:
        break;
    }

    return SOC_E_NONE;
}

STATIC int
_soc_tr_tune_generate_config(soc_memtune_data_t *mt_data)
{
#ifndef NO_MEMTUNE
    soc_memtune_ctrl_t *mt_ctrl = mt_data->mt_ctrl;
    soc_memtune_result_t *result;
    uint32 rval, val, pvt_val;
    char name_str[23], val_str[11];
    int use_midl;

    pvt_val = 0;
    if (mt_ctrl->odtres_val != -1) {
        pvt_val |= SOC_TR_MEMTUNE_OVRD_ODTRES_MASK <<
            SOC_TR_MEMTUNE_OVRD_ODTRES_SHIFT;
        pvt_val |= (mt_ctrl->odtres_val & SOC_TR_MEMTUNE_ODTRES_MASK) <<
            SOC_TR_MEMTUNE_ODTRES_SHIFT;
    }
    if (mt_ctrl->ndrive_val != -1 && mt_ctrl->pdrive_val != -1) {
        pvt_val |= SOC_TR_MEMTUNE_OVRD_DRIVER_MASK <<
            SOC_TR_MEMTUNE_OVRD_DRIVER_SHIFT;
        pvt_val |= (mt_ctrl->pdrive_val & SOC_TR_MEMTUNE_PDRIVE_MASK) <<
            SOC_TR_MEMTUNE_PDRIVE_SHIFT;
        pvt_val |= (mt_ctrl->ndrive_val & SOC_TR_MEMTUNE_NDRIVE_MASK) <<
            SOC_TR_MEMTUNE_NDRIVE_SHIFT;
    }
    if (mt_ctrl->slew_val != -1) {
        pvt_val |= SOC_TR_MEMTUNE_OVRD_SLEW_MASK <<
            SOC_TR_MEMTUNE_OVRD_SLEW_SHIFT;
        pvt_val |= (mt_ctrl->slew_val & SOC_TR_MEMTUNE_SLEW_MASK) <<
            SOC_TR_MEMTUNE_SLEW_SHIFT;
    }

    switch (mt_data->interface) {
    case SOC_MEM_INTERFACE_SRAM:
        val = 0;
        if (mt_ctrl->phase_sel_cur != -1) {
            val |= SOC_TR_MEMTUNE_OVRD_SM_MASK <<
                SOC_TR_MEMTUNE_OVRD_SM_SHIFT;
            val |= (mt_ctrl->phase_sel_cur & SOC_TR_MEMTUNE_PHASE_SEL_MASK) <<
                SOC_TR_MEMTUNE_PHASE_SEL_SHIFT;
        }
        sal_sprintf(val_str, "0x%08x",
                    SOC_TR_MEMTUNE_CONFIG_VALID_MASK |
                    val |
                    ((mt_ctrl->em_latency_cur &
                      SOC_TR_MEMTUNE_EM_LATENCY_MASK) <<
                     SOC_TR_MEMTUNE_EM_LATENCY_SHIFT) |
                    ((mt_ctrl->ddr_latency_cur &
                      SOC_TR_MEMTUNE_DDR_LATENCY_MASK) <<
                     SOC_TR_MEMTUNE_DDR_LATENCY_SHIFT) |
                    ((mt_ctrl->tx_offset_cur &
                      SOC_TR_MEMTUNE_DDR_TX_OFFSET_MASK) <<
                     SOC_TR_MEMTUNE_DDR_TX_OFFSET_SHIFT) |
                    ((mt_ctrl->rx_offset_cur &
                      SOC_TR_MEMTUNE_DDR_RX_OFFSET_MASK) <<
                     SOC_TR_MEMTUNE_DDR_RX_OFFSET_SHIFT) |
                    ((mt_ctrl->w2r_nops_cur & SOC_TR_MEMTUNE_W2R_NOPS_MASK) <<
                     SOC_TR_MEMTUNE_W2R_NOPS_SHIFT) |
                    ((mt_ctrl->r2w_nops_cur & SOC_TR_MEMTUNE_R2W_NOPS_MASK) <<
                     SOC_TR_MEMTUNE_R2W_NOPS_SHIFT) |
                    ((mt_data->freq & SOC_TR_MEMTUNE_DDR_FREQ_MASK) <<
                     SOC_TR_MEMTUNE_DDR_FREQ_SHIFT));
        sal_sprintf(name_str, "%s%d", spn_EXT_SRAM_TUNING,
                    mt_data->sub_interface);
        if (sal_config_set(name_str, val_str) < 0) {
            return SOC_E_MEMORY;
        }
        if (pvt_val) {
            sal_sprintf(val_str, "0x%08x",
                        SOC_TR_MEMTUNE_CONFIG_VALID_MASK | pvt_val);
            sal_sprintf(name_str, "%s%d", spn_EXT_SRAM_PVT,
                        mt_data->sub_interface);
            if (sal_config_set(name_str, val_str) < 0) {
                return SOC_E_MEMORY;
            }
        }
        if (mt_data->manual_settings) {
            break;
        }
        if (mt_ctrl->phase_sel_cur != -1) {
            result = mt_ctrl->ps_data[mt_ctrl->phase_sel_cur];
        } else {
            result = &mt_ctrl->result[0];
        }
        sal_sprintf(val_str, "0x%08x",
                    SOC_TR_MEMTUNE_CONFIG_VALID_MASK |
                    ((result->width & SOC_TR_MEMTUNE_STATS_WIDTH_MASK) <<
                     SOC_TR_MEMTUNE_STATS_WIDTH_SHIFT) |
                    ((result->height & SOC_TR_MEMTUNE_STATS_HEIGHT_MASK) <<
                     SOC_TR_MEMTUNE_STATS_HEIGHT_SHIFT) |
                    ((result->fail_count & SOC_TR_MEMTUNE_STATS_FAIL_MASK) <<
                     SOC_TR_MEMTUNE_STATS_FAIL_SHIFT));
        sal_sprintf(name_str, "%s%d", spn_EXT_SRAM_TUNING_STATS,
                    mt_data->sub_interface);
        if (sal_config_set(name_str, val_str) < 0) {
            return SOC_E_MEMORY;
        }
        break;
    case SOC_MEM_INTERFACE_TCAM:
        SOC_IF_ERROR_RETURN(READ_ETU_DDR72_CONFIG_REG1_ISr(mt_ctrl->unit,
                                                           &rval));
        use_midl =
            soc_reg_field_get(mt_ctrl->unit, ETU_DDR72_CONFIG_REG1_ISr, rval,
                              MIDL_TX_ENf) &&
            soc_reg_field_get(mt_ctrl->unit, ETU_DDR72_CONFIG_REG1_ISr, rval,
                              SEL_TX_CLKDLY_BLKf) ? 1 : 0;
        sal_sprintf(val_str, "0x%08x",
                    SOC_TR_MEMTUNE_CONFIG_VALID_MASK |
                    ((mt_ctrl->fcd_dpeo_cur & SOC_TR_MEMTUNE_FCD_DPEO_MASK) <<
                     SOC_TR_MEMTUNE_FCD_DPEO_SHIFT) |
                    ((mt_ctrl->dpeo_sync_dly_cur &
                      SOC_TR_MEMTUNE_DPEO_SYNC_DLY_MASK) <<
                     SOC_TR_MEMTUNE_DPEO_SYNC_DLY_SHIFT) |
                    ((mt_ctrl->fcd_rbus_cur & SOC_TR_MEMTUNE_FCD_RBUS_MASK) <<
                     SOC_TR_MEMTUNE_FCD_RBUS_SHIFT)|
                    ((mt_ctrl->rbus_sync_dly_cur &
                      SOC_TR_MEMTUNE_RBUS_SYNC_DLY_MASK) <<
                     SOC_TR_MEMTUNE_RBUS_SYNC_DLY_SHIFT));
        sal_sprintf(name_str, "%s0", spn_EXT_TCAM_TUNING);
        if (sal_config_set(name_str, val_str) < 0) {
            return SOC_E_MEMORY;
        }
        sal_sprintf(val_str, "0x%08x",
                    SOC_TR_MEMTUNE_CONFIG_VALID_MASK |
                    ((mt_ctrl->tx_offset_cur &
                      SOC_TR_MEMTUNE_TCAM_TX_OFFSET_MASK) <<
                     SOC_TR_MEMTUNE_TCAM_TX_OFFSET_SHIFT) |
                    ((mt_ctrl->rx_offset_cur &
                      SOC_TR_MEMTUNE_TCAM_RX_OFFSET_MASK) <<
                     SOC_TR_MEMTUNE_TCAM_RX_OFFSET_SHIFT) |
                    ((mt_data->freq &
                      SOC_TR_MEMTUNE_TCAM_FREQ_MASK) <<
                     SOC_TR_MEMTUNE_TCAM_FREQ_SHIFT) |
                    ((use_midl & SOC_TR_MEMTUNE_USE_MIDL_MASK) <<
                     SOC_TR_MEMTUNE_USE_MIDL_SHIFT));
        sal_sprintf(name_str, "%s1", spn_EXT_TCAM_TUNING);
        if (sal_config_set(name_str, val_str) < 0) {
            return SOC_E_MEMORY;
        }
        if (pvt_val) {
            sal_sprintf(val_str, "0x%08x",
                        SOC_TR_MEMTUNE_CONFIG_VALID_MASK | pvt_val);
            sal_sprintf(name_str, "%s", spn_EXT_TCAM_PVT);
            if (sal_config_set(name_str, val_str) < 0) {
                return SOC_E_MEMORY;
            }
        }
        if (mt_data->manual_settings) {
            break;
        }
        result = &mt_ctrl->result[0];
        sal_sprintf(val_str, "0x%08x",
                    SOC_TR_MEMTUNE_CONFIG_VALID_MASK |
                    ((result->width & SOC_TR_MEMTUNE_STATS_WIDTH_MASK) <<
                     SOC_TR_MEMTUNE_STATS_WIDTH_SHIFT) |
                    ((result->height & SOC_TR_MEMTUNE_STATS_HEIGHT_MASK) <<
                     SOC_TR_MEMTUNE_STATS_HEIGHT_SHIFT) |
                    ((result->fail_count & SOC_TR_MEMTUNE_STATS_FAIL_MASK) <<
                     SOC_TR_MEMTUNE_STATS_FAIL_SHIFT));
        sal_sprintf(name_str, "%s", spn_EXT_TCAM_TUNING_STATS);
        if (sal_config_set(name_str, val_str) < 0) {
            return SOC_E_MEMORY;
        }
        break;
    default:
        break;
    }
#endif /* NO_MEMTUNE */

    return SOC_E_NONE;
}
#endif /* BCM_TRIUMPH_SUPPORT */

#define ABS(n) (((n) < 0) ? -(n) : (n))

STATIC void
_soc_memtune_get_txrx_area(soc_memtune_data_t *mt_data,
                           int tx_offset, int rx_offset,
                           int *width, int *height)
{
    soc_memtune_ctrl_t *mt_ctrl = mt_data->mt_ctrl;
    int best_area, best_shape, area, shape;
    int dt, dr, max_dt, max_dr, index, num_rx;

    num_rx = mt_ctrl->rx_offset_max - mt_ctrl->rx_offset_min + 1;
    index = (tx_offset - mt_ctrl->tx_offset_min) * num_rx +
            rx_offset - mt_ctrl->rx_offset_min;
    if (mt_ctrl->fail_array[index]) {
        *width = *height = 0;
        return;
    }

    max_dt = (mt_ctrl->tx_offset_max - mt_ctrl->tx_offset_min) / 2 + 1;
    for (dt = 1; dt < max_dt; dt++) {
        if (tx_offset - dt < mt_ctrl->tx_offset_min ||
            tx_offset + dt > mt_ctrl->tx_offset_max ||
            mt_ctrl->fail_array[index - dt * num_rx] ||
            mt_ctrl->fail_array[index + dt * num_rx]) {
            max_dt = dt;
            break;
        }
    }

    max_dr = (mt_ctrl->rx_offset_max - mt_ctrl->rx_offset_min) / 2 + 1;
    for (dr = 1; dr < max_dr; dr++) {
        if (rx_offset - dr < mt_ctrl->rx_offset_min ||
            rx_offset + dr > mt_ctrl->rx_offset_max ||
            mt_ctrl->fail_array[index - dr] ||
            mt_ctrl->fail_array[index + dr]) {
            max_dr = dr;
            break;
        }
    }

    area = max_dr * 2 - 1;
    shape = area - 1;
    best_area = area;
    best_shape = shape;
    *width = area;
    *height = 1;

    for (dt = 1; dt < max_dt; dt++) {
        for (dr = 1; dr < max_dr; dr++) {
            if (mt_ctrl->fail_array[index - dt * num_rx - dr] ||
                mt_ctrl->fail_array[index - dt * num_rx + dr] ||
                mt_ctrl->fail_array[index + dt * num_rx - dr] ||
                mt_ctrl->fail_array[index + dt * num_rx + dr]) {
                max_dr = dr;
                break;
            }
        }

        dr--;
        area = (dt * 2 + 1) * (dr * 2 + 1);
        shape = ABS(dt - dr) * 2;
        if (area > best_area ||
            (area == best_area && shape < best_shape)) {
            best_area = area;
            best_shape = shape;
            *width = dr * 2 + 1;
            *height = dt * 2 + 1;
        }
    }
}

STATIC int
_soc_memtune_txrx_analyze(soc_memtune_data_t *mt_data)
{
    soc_memtune_ctrl_t *mt_ctrl = mt_data->mt_ctrl;
    soc_memtune_result_t *result;
    int index, tx_offset, rx_offset;
    int area, shape, width, height;
    int best_area, best_shape, best_width, best_height, fail_count;

    /* Print failure matrix */
    if (mt_ctrl->flags & SOC_MEMTUNE_CTRL_FLAGS_SHOW_FAIL_MATRIX) {
        soc_cm_print("Memory tuning failure counts: unit %d interface %s:\n",
                     mt_ctrl->unit, mt_ctrl->intf_name);
        soc_cm_print("TX (vertical axis) range: %d - %d, "
                     "RX (horizontal axis) range: %d - %d\n",
                     mt_ctrl->tx_offset_min, mt_ctrl->tx_offset_max,
                     mt_ctrl->rx_offset_min, mt_ctrl->rx_offset_max);
        soc_cm_print("        |");
        for (index = mt_ctrl->rx_offset_min; index <= mt_ctrl->rx_offset_max;
             index++) {
            soc_cm_print("%4d\t", index);
        }
        soc_cm_print("\n--------+");
        for (rx_offset = mt_ctrl->rx_offset_min;
             rx_offset <= mt_ctrl->rx_offset_max; rx_offset++) {
            soc_cm_print("--------");
        }
        index = 0;
        for (tx_offset = mt_ctrl->tx_offset_min;
             tx_offset <= mt_ctrl->tx_offset_max; tx_offset++) {
            soc_cm_print("\n     %2d |", tx_offset);
            for (rx_offset = mt_ctrl->rx_offset_min;
                 rx_offset <= mt_ctrl->rx_offset_max; rx_offset++) {
                soc_cm_print("%4d\t", mt_ctrl->fail_array[index]);
                index++;
            }
        }
        soc_cm_print("\n");
    }

    if (mt_ctrl->flags & SOC_MEMTUNE_CTRL_FLAGS_SHOW_ANALYSIS) {
        soc_cm_print("Memory tuning analysis [box area, abs(width - height)]: "
                     "unit %d interface %s:\n",
                     mt_ctrl->unit, mt_ctrl->intf_name);
        soc_cm_print("        |");
        for (index = mt_ctrl->rx_offset_min; index <= mt_ctrl->rx_offset_max;
             index++) {
            soc_cm_print("%4d\t", index);
        }
        soc_cm_print("\n--------+");
        for (rx_offset = mt_ctrl->rx_offset_min;
             rx_offset <= mt_ctrl->rx_offset_max; rx_offset++) {
            soc_cm_print("--------");
        }
    }

    fail_count = 0;
    best_area = 0;
    best_shape = mt_ctrl->tx_offset_max + mt_ctrl->rx_offset_max;
    best_width = 0;
    best_height = 0;

    for (tx_offset = mt_ctrl->tx_offset_min;
         tx_offset <= mt_ctrl->tx_offset_max; tx_offset++) {
        if (mt_ctrl->flags & SOC_MEMTUNE_CTRL_FLAGS_SHOW_ANALYSIS) {
            soc_cm_print("\n     %2d |", tx_offset);
        }
        for (rx_offset = mt_ctrl->rx_offset_min;
             rx_offset <= mt_ctrl->rx_offset_max; rx_offset++) {
            _soc_memtune_get_txrx_area(mt_data, tx_offset, rx_offset,
                                       &width, &height);
            area = width * height;
            shape = ABS(width - height);
            if (area != 0) {
                if (mt_ctrl->flags & SOC_MEMTUNE_CTRL_FLAGS_SHOW_ANALYSIS) {
                    soc_cm_print("%3d,%1d\t", area, shape);
                }
                if (area > best_area ||
                    (area == best_area && shape < best_shape)) {
                    best_area = area;
                    best_shape = shape;
                    best_width = width;
                    best_height = height;
                    mt_ctrl->tx_offset_cur = tx_offset;
                    mt_ctrl->rx_offset_cur = rx_offset;
                }
            } else {
                fail_count++;
                if (mt_ctrl->flags & SOC_MEMTUNE_CTRL_FLAGS_SHOW_ANALYSIS) {
                    soc_cm_print("  *,*\t");
                }
            }
        }
    }
    if (mt_ctrl->flags & SOC_MEMTUNE_CTRL_FLAGS_SHOW_ANALYSIS) {
        soc_cm_print("\n");
    }

    result = &mt_ctrl->result[mt_ctrl->cur_result_count];

    result->fail_count = fail_count;
    if (best_area != 0) {
        /* save the stats and tx/rx settings for this result */
        result->width = best_width;
        result->height = best_height;
        result->tx_offset = mt_ctrl->tx_offset_cur;
        result->rx_offset = mt_ctrl->rx_offset_cur;
    }

    mt_ctrl->cur_result_count++;

    return SOC_E_NONE;
}

#define BACKSPACE               "\010"
STATIC int
_soc_memtune_phase_sel_compare(soc_memtune_data_t *mt_data)
{
    soc_memtune_ctrl_t *mt_ctrl = mt_data->mt_ctrl;
    soc_memtune_result_t *ps_data;
    int area[4], adj_area[4], shape, side;
    int best_area, best_shape, best_side, best_adj_area;
    int i, index;

    for (i = mt_ctrl->phase_sel_min; i <= mt_ctrl->phase_sel_max; i++) {
        area[i] = mt_ctrl->ps_data[i]->width * mt_ctrl->ps_data[i]->height;
    }
    for (i = mt_ctrl->phase_sel_min; i <= mt_ctrl->phase_sel_max; i++) {
        /* For each of the phase_sel values, get a sum of the
         * areas of the 2 neighboring phase_sel values. Later,
         * if 2 phase_sel values are found to have the same area/shape,
         * preference is given to the one with better neighbors.
         */
        adj_area[i] = area[i];
        index = i == mt_ctrl->phase_sel_min ? mt_ctrl->phase_sel_max : i - 1;
        if (mt_ctrl->ps_data[index]->ddr_latency ==
            mt_ctrl->ps_data[i]->ddr_latency &&
            mt_ctrl->ps_data[index]->em_latency ==
            mt_ctrl->ps_data[i]->em_latency) {
            adj_area[i] += area[index];
        }
        index = i == mt_ctrl->phase_sel_max ? mt_ctrl->phase_sel_min : i + 1;
        if (mt_ctrl->ps_data[index]->ddr_latency ==
            mt_ctrl->ps_data[i]->ddr_latency &&
            mt_ctrl->ps_data[index]->em_latency ==
            mt_ctrl->ps_data[i]->em_latency) {
            adj_area[i] += area[index];
        }
    }

    best_side = 0;
    best_adj_area = 0;
    best_area = 0;
    best_shape = 0;
    mt_ctrl->phase_sel_cur = 0;
    for (i = mt_ctrl->phase_sel_min; i <= mt_ctrl->phase_sel_max; i++) {
        ps_data = mt_ctrl->ps_data[i];
        if (ps_data->width > ps_data->height) {
            side = ps_data->height;
            shape = ps_data->width - ps_data->height;
        } else {
            side = ps_data->width;
            shape = ps_data->height - ps_data->width;
        }
        if (!area[i]) {
            continue;
        }
        if (side < best_side) {
            continue;
        } else if (side == best_side) {
            if (adj_area[i] < best_adj_area) {
                continue;
            } else if (adj_area[i] == best_adj_area) {
                if (area[i] < best_area) {
                    continue;
                } else if (area[i] == best_area) {
                    if (shape >= best_shape) {
                        continue;
                    }
                }
            }
        }

        best_side = side;
        best_adj_area = adj_area[i];
        best_area = area[i];
        best_shape = shape;
        mt_ctrl->phase_sel_cur = i;
    }

    return SOC_E_NONE;
}

STATIC int
_soc_memtune_ddr_single_test(soc_memtune_data_t *mt_data)
{
    soc_memtune_ctrl_t *mt_ctrl = mt_data->mt_ctrl;
    int rv;

    /* Lock in settings */
    rv = (*mt_ctrl->prog_hw1_fn)(mt_data);
    if (rv < 0) {
        SOC_ERROR_PRINT((DK_ERR, "unit %d %s prog hardware failure: %s\n",
                         mt_ctrl->unit, mt_ctrl->intf_name, soc_errmsg(rv)));
        return rv;
    }

    mt_ctrl->cur_fail_count = 0;

    rv = (*mt_ctrl->test_fn)(mt_data);

    if (rv < 0) {
        SOC_ERROR_PRINT((DK_ERR, "unit %d %s test failure: %s\n",
                         mt_ctrl->unit, mt_ctrl->intf_name, soc_errmsg(rv)));
        return rv;
    }

    if ((!(mt_ctrl->flags & SOC_MEMTUNE_CTRL_FLAGS_SHOW_PASS) &&
         mt_ctrl->cur_fail_count != mt_data->test_count) ||
        (!(mt_ctrl->flags & SOC_MEMTUNE_CTRL_FLAGS_SHOW_FAIL) &&
         mt_ctrl->cur_fail_count == mt_data->test_count)) {
        return SOC_E_NONE;
    }

    if (mt_ctrl->phase_sel_cur != -1) {
        soc_cm_print("Phase select=%d ", mt_ctrl->phase_sel_cur);
    }
    if (mt_ctrl->em_latency_cur != -1) {
        soc_cm_print("EM lat=%d ", mt_ctrl->em_latency_cur);
    }
    if (mt_ctrl->ddr_latency_cur != -1) {
        soc_cm_print("DDR lat=%d ", mt_ctrl->ddr_latency_cur);
    }
    if (mt_ctrl->tx_offset_cur != -1) {
        soc_cm_print("TX offset=%d ", mt_ctrl->tx_offset_cur);
    }
    if (mt_ctrl->rx_offset_cur != -1) {
        soc_cm_print("RX offset=%d ", mt_ctrl->rx_offset_cur);
    }
    if (mt_ctrl->flags & SOC_MEMTUNE_CTRL_FLAGS_SHOW_NOPS) {
        if (mt_ctrl->r2w_nops_cur != -1) {
            soc_cm_print("R2W nops=%d ", mt_ctrl->r2w_nops_cur);
        }
        if (mt_ctrl->w2r_nops_cur != -1) {
            soc_cm_print("W2R nops=%d ", mt_ctrl->w2r_nops_cur);
        }
    }
    soc_cm_print(": Fail count=%d\n", mt_ctrl->cur_fail_count);

    return SOC_E_NONE;
}

STATIC int
_soc_memtune_ddr_main(soc_memtune_data_t *mt_data)
{
    soc_memtune_ctrl_t *mt_ctrl = mt_data->mt_ctrl;
    soc_memtune_result_t *result;
    int rv, success, offset_count, old_result_count, index, i;
    char progress_symbol;

    if (mt_ctrl->flags & SOC_MEMTUNE_CTRL_FLAGS_SHOW_TEST_NAME) {
        soc_cm_print("Memory tuning test: unit %d interface %s:\n",
                     mt_ctrl->unit, mt_ctrl->intf_name);
    }

    if (mt_data->manual_settings) {
        mt_ctrl->phase_sel_cur = mt_data->man_phase_sel;
        mt_ctrl->em_latency_cur = mt_data->man_em_latency;
        mt_ctrl->ddr_latency_cur = mt_data->man_ddr_latency;
        mt_ctrl->tx_offset_cur = mt_data->man_tx_offset;
        mt_ctrl->rx_offset_cur = mt_data->man_rx_offset;
        mt_ctrl->w2r_nops_cur = mt_data->man_w2r_nops;
        mt_ctrl->r2w_nops_cur = mt_data->man_r2w_nops;
        mt_ctrl->cur_fail_count = 0;

        (*mt_ctrl->prog_hw2_fn)(mt_data); /* select phase */
        SOC_IF_ERROR_RETURN(_soc_memtune_ddr_single_test(mt_data));
        if (!mt_ctrl->cur_fail_count && mt_data->config) {
#ifdef BCM_TRIUMPH_SUPPORT
            if (soc_feature(mt_ctrl->unit, soc_feature_esm_support)) {
                SOC_IF_ERROR_RETURN(_soc_tr_tune_generate_config(mt_data));
            }
#endif /* BCM_TRIUMPH_SUPPORT */
        }
        return SOC_E_NONE;
    }

    offset_count = (mt_ctrl->tx_offset_max - mt_ctrl->tx_offset_min + 1) *
                   (mt_ctrl->rx_offset_max - mt_ctrl->rx_offset_min + 1);
    mt_ctrl->fail_array = sal_alloc(offset_count * sizeof(int),
                                    "memtune working data");
    if (mt_ctrl->fail_array == NULL) {
        return SOC_E_MEMORY;
    }
    sal_memset(mt_ctrl->fail_array, 0, offset_count * sizeof(int));

    mt_ctrl->w2r_nops_cur = mt_ctrl->w2r_nops_max;
    mt_ctrl->r2w_nops_cur = mt_ctrl->r2w_nops_max;
    mt_ctrl->cur_result_count = 0;

    for (mt_ctrl->phase_sel_cur = mt_ctrl->phase_sel_min;
         mt_ctrl->phase_sel_cur <= mt_ctrl->phase_sel_max;
         mt_ctrl->phase_sel_cur++) {
        if (mt_ctrl->phase_sel_cur != -1) {
            (*mt_ctrl->prog_hw2_fn)(mt_data);
        }

        old_result_count = mt_ctrl->cur_result_count;
        progress_symbol = '-';
        for (mt_ctrl->em_latency_cur = mt_ctrl->em_latency_min;
             mt_ctrl->em_latency_cur <= mt_ctrl->em_latency_max;
             mt_ctrl->em_latency_cur++) {
            for (mt_ctrl->ddr_latency_cur = mt_ctrl->ddr_latency_min;
                 mt_ctrl->ddr_latency_cur <= mt_ctrl->ddr_latency_max;
                 mt_ctrl->ddr_latency_cur++) {
                success = FALSE;
                for (mt_ctrl->tx_offset_cur = mt_ctrl->tx_offset_min;
                     mt_ctrl->tx_offset_cur <= mt_ctrl->tx_offset_max;
                     mt_ctrl->tx_offset_cur++) {
                    for (mt_ctrl->rx_offset_cur = mt_ctrl->rx_offset_min;
                         mt_ctrl->rx_offset_cur <= mt_ctrl->rx_offset_max;
                         mt_ctrl->rx_offset_cur++) {
                        if (mt_ctrl->flags &
                            SOC_MEMTUNE_CTRL_FLAGS_SHOW_PROGRESS) {
                            soc_cm_print("%c" BACKSPACE, progress_symbol);
                            progress_symbol ^= '-' ^ '|';
                        }

                        rv = _soc_memtune_ddr_single_test(mt_data);
                        if (rv < 0) { /* fail unexpected */
                            soc_cm_print(" " BACKSPACE);
                            sal_free(mt_ctrl->fail_array);
                            return rv;
                        }

                        index =
                            (mt_ctrl->tx_offset_cur - mt_ctrl->tx_offset_min) *
                            (mt_ctrl->rx_offset_max - mt_ctrl->rx_offset_min +
                             1) +
                            mt_ctrl->rx_offset_cur - mt_ctrl->rx_offset_min;
                        mt_ctrl->fail_array[index] =
                            mt_ctrl->cur_fail_count;
                        if (!mt_ctrl->cur_fail_count) {
                            success = TRUE;
                        }
                    } /* mt_ctrl->rx_offset_cur loop */
                } /* mt_ctrl->tx_offset_cur loop */
                if (success) {
                    if (mt_ctrl->flags &
                        SOC_MEMTUNE_CTRL_FLAGS_SHOW_PROGRESS) {
                        soc_cm_print(" " BACKSPACE);
                    }

                    /* Analyze and print result matrix */
                    result = &mt_ctrl->result[mt_ctrl->cur_result_count];
                    _soc_memtune_txrx_analyze(mt_data);
                    result->phase_sel = mt_ctrl->phase_sel_cur;
                    result->em_latency = mt_ctrl->em_latency_cur;
                    result->ddr_latency = mt_ctrl->ddr_latency_cur;
                    if (!mt_data->test_all_latency) {
                        goto loop_done;
                    }
                }
            } /* mt_ctrl->ddr_latency_cur loop */
        } /* mt_ctrl->em_latency_cur loop */

        /* If nothing is passed on any EM_LATENCY, DDR_LATENCY setting */
        if (mt_ctrl->cur_result_count == old_result_count) {
            result = &mt_ctrl->result[mt_ctrl->cur_result_count];
            result->phase_sel = mt_ctrl->phase_sel_cur;
            result->fail_count = offset_count;
            mt_ctrl->cur_result_count++;
        }

    loop_done:
        index = mt_ctrl->phase_sel_cur == -1 ? 0 : mt_ctrl->phase_sel_cur;
        for (i = old_result_count; i < mt_ctrl->cur_result_count; i++) {
            if (mt_ctrl->ps_data[index] == NULL ||
                (mt_ctrl->ps_data[index]->width *
                 mt_ctrl->ps_data[index]->height <
                 mt_ctrl->result[i].width * mt_ctrl->result[i].height)) {
               mt_ctrl->ps_data[index] = &mt_ctrl->result[i];
            }
        }
    }
    mt_ctrl->phase_sel_cur = mt_ctrl->phase_sel_min;

    if (mt_ctrl->phase_sel_cur != -1) {
        /* Select the best result from 4 different phase */
        _soc_memtune_phase_sel_compare(mt_data);
        result = mt_ctrl->ps_data[mt_ctrl->phase_sel_cur];

        /* Program phase setting from the best result */
        (*mt_ctrl->prog_hw2_fn)(mt_data);
    } else {
        result = mt_ctrl->ps_data[0];
    }

    if (result->fail_count == offset_count) {
        soc_cm_print("!!! SRAM tuning FAIL !!!\n");
    } else {
        /* Program latency and tx/rx setting from the best result */
        mt_ctrl->em_latency_cur = result->em_latency;
        mt_ctrl->ddr_latency_cur = result->ddr_latency;
        mt_ctrl->tx_offset_cur = result->tx_offset;
        mt_ctrl->rx_offset_cur = result->rx_offset;
        (*mt_ctrl->prog_hw1_fn)(mt_data);

        /* Tune number of nop between write-to-read and read-to-write */
        mt_ctrl->flags |= SOC_MEMTUNE_CTRL_FLAGS_SHOW_NOPS;
        success = FALSE;
        if (mt_ctrl->w2r_nops_max >= 0 && mt_ctrl->r2w_nops_max >= 0) {
            for (mt_ctrl->w2r_nops_cur = mt_ctrl->w2r_nops_min;
                 mt_ctrl->w2r_nops_cur <= mt_ctrl->w2r_nops_max;
                 mt_ctrl->w2r_nops_cur++) {
                for (mt_ctrl->r2w_nops_cur = mt_ctrl->r2w_nops_max;
                     mt_ctrl->r2w_nops_cur >= mt_ctrl->r2w_nops_min;
                     mt_ctrl->r2w_nops_cur--) {
                    _soc_memtune_ddr_single_test(mt_data);
                    if (!mt_ctrl->cur_fail_count) {
                        success = TRUE;
                        continue;
                    }
                    if (mt_ctrl->r2w_nops_cur < mt_ctrl->r2w_nops_max) {
                        mt_ctrl->r2w_nops_cur++;
                    }
                    break;
                }
                if (mt_ctrl->r2w_nops_cur < mt_ctrl->r2w_nops_min) {
                    mt_ctrl->r2w_nops_cur++;
                }
                if (success) {
                    break;
                }
            }
            if (success) {
                if (mt_ctrl->r2w_nops_cur < 1) {
                    mt_ctrl->r2w_nops_cur = 1; /* hardware interprets 0 as 4 */
                }
                if (*mt_ctrl->prog_hw3_fn != NULL) {
                    (*mt_ctrl->prog_hw3_fn)(mt_data);
                }
            } else { /* should not be here */
                mt_ctrl->r2w_nops_cur = mt_ctrl->r2w_nops_max;
                mt_ctrl->w2r_nops_cur = mt_ctrl->w2r_nops_max;
            }
        }
        mt_ctrl->flags &= ~SOC_MEMTUNE_CTRL_FLAGS_SHOW_NOPS;

        if (mt_ctrl->flags & SOC_MEMTUNE_CTRL_FLAGS_SHOW_SELECTION) {
            soc_cm_print("Memory tuning selection: ");
            if (mt_ctrl->phase_sel_cur != -1) {
                soc_cm_print("Phase sel=%d ", mt_ctrl->phase_sel_cur);
            }
            if (mt_ctrl->em_latency_cur != -1) {
                soc_cm_print("EM lat=%d ", mt_ctrl->em_latency_cur);
            }
            if (mt_ctrl->ddr_latency_cur != -1) {
                soc_cm_print("DDR lat=%d ", mt_ctrl->ddr_latency_cur);
            }
            if (mt_ctrl->tx_offset_cur != -1) {
                soc_cm_print("TX offset=%d ", mt_ctrl->tx_offset_cur);
            }
            if (mt_ctrl->rx_offset_cur != -1) {
                soc_cm_print("RX offset=%d ", mt_ctrl->rx_offset_cur);
            }
            if (mt_ctrl->w2r_nops_cur != -1) {
                soc_cm_print("W2R nops=%d ", mt_ctrl->w2r_nops_cur);
            }
            if (mt_ctrl->r2w_nops_cur != -1) {
                soc_cm_print("R2W nops=%d ", mt_ctrl->r2w_nops_cur);
            }
            soc_cm_print("\n");
        }
    }

    if (mt_ctrl->flags & SOC_MEMTUNE_CTRL_FLAGS_SHOW_SUMMARY) {
        if (mt_ctrl->flags & SOC_MEMTUNE_CTRL_FLAGS_SHOW_SUMMARY_HEADER) {
            soc_cm_print("\n Interface  , Area , Width , Height , PS ,"
                         "  TX ,  RX , Lat , Em , Pass , Fail");
            if (SOC_IS_TR_VL(mt_ctrl->unit)) {
                soc_cm_print(" , Freq , Tb , Sb , Dac\n");
            }
        }

        for (i = 0; i < mt_ctrl->cur_result_count; i++) {
            result = &mt_ctrl->result[i];

            soc_cm_print(" %10s , %4d ,  %3d  ,  %3d   ,",
                         mt_ctrl->intf_name, result->width * result->height,
                         result->width, result->height);
            if (mt_ctrl->phase_sel_cur != -1) {
                soc_cm_print("  %1d ,", result->phase_sel);
            } else {
                soc_cm_print(" ** ,");
            }
            if (result->fail_count != offset_count) {
                soc_cm_print("  %2d ,  %2d , %2d  ,",
                             result->tx_offset, result->rx_offset,
                             result->ddr_latency);
                if (result->em_latency != -1) {
                    soc_cm_print(" %2d ,", result->em_latency);
                } else {
                    soc_cm_print(" ** ,");
                }
            } else {
                soc_cm_print("  ** ,  ** , **  , ** ,");
            }
            soc_cm_print(" %4d , %4d",
                         offset_count - result->fail_count,
                         result->fail_count);
            if (SOC_IS_TR_VL(mt_ctrl->unit)) {
                soc_cm_print(" ,  %3d ,  %c ,  %c ,",
                             mt_data->freq,
                             mt_data->bg_tcam_bist ? 'Y' : 'N',
                             mt_data->bg_sram_bist ? 'Y' : 'N');
                if (mt_ctrl->dac_value != -1) {
                    soc_cm_print("  %2d", mt_ctrl->dac_value);
                } else {
                    soc_cm_print("  **");
                }
            }
            index = result->phase_sel == -1 ? 0 : result->phase_sel;
            if (result != mt_ctrl->ps_data[index]) {
                soc_cm_print(" (ignored)\n");
            } else if (result->fail_count != offset_count &&
                       mt_ctrl->phase_sel_cur != -1 &&
                       result == mt_ctrl->ps_data[mt_ctrl->phase_sel_cur]) {
                soc_cm_print(" <\n");
            } else {
                soc_cm_print("\n");
            }
        }
    }

    if (result->fail_count != offset_count) {
        if (mt_data->config) {
            rv = SOC_E_NONE;
#ifdef BCM_EASYRIDER_SUPPORT
            if (SOC_IS_EASYRIDER(mt_ctrl->unit)) {
                rv = _soc_er_tune_generate_config(mt_data);
            }
#endif
#ifdef BCM_TRIUMPH_SUPPORT
            if (soc_feature(mt_ctrl->unit, soc_feature_esm_support)) {
                rv = _soc_tr_tune_generate_config(mt_data);
            }
#endif
            if (rv < 0) {
                SOC_ERROR_PRINT((DK_ERR,
                                 "unit %d %s fail to generate config: %s\n",
                                 mt_ctrl->unit, mt_ctrl->intf_name,
                                 soc_errmsg(rv)));
            }
        }
    }

    sal_free(mt_ctrl->fail_array);
    return SOC_E_NONE;
}

#ifdef BCM_TRIUMPH_SUPPORT
STATIC int
_soc_memtune_tcam_single_test(soc_memtune_data_t *mt_data)
{
    soc_tcam_info_t *tcam_info;
    soc_memtune_ctrl_t *mt_ctrl = mt_data->mt_ctrl;
    soc_tr_memtune_data_t *tr_mt_data = mt_ctrl->data;
    char *fail_name = NULL;
    int rv, index;
    uint32 addr, rval, mask[8], data[8];

    tcam_info = SOC_CONTROL(mt_ctrl->unit)->tcam_info;
    mt_ctrl->dpeo_sync_dly_cur = -1;
    mt_ctrl->fcd_dpeo_cur = -1;
    mt_ctrl->rbus_sync_dly_cur = -1;
    mt_ctrl->fcd_rbus_cur = -1;

    /* DBUS */
    mt_data->sub_interface = 0;
    rv = (*mt_ctrl->prog_hw1_fn)(mt_data);
    if (rv < 0) {
        SOC_ERROR_PRINT((DK_ERR, "unit %d %s prog hardware failure: %s\n",
                         mt_ctrl->unit, mt_ctrl->intf_name,
                         soc_errmsg(rv)));
        return rv;
    }
    mt_ctrl->cur_fail_count = 0;
    rv = (*mt_ctrl->test_fn)(mt_data);
    if (rv < 0) {
        SOC_ERROR_PRINT((DK_ERR, "unit %d %s dbus test failure: %s\n",
                         mt_ctrl->unit, mt_ctrl->intf_name, soc_errmsg(rv)));
        return rv;
    }
    if (mt_ctrl->cur_fail_count) {
        fail_name = "DBUS";
        goto done;
    }

    /* DBUS_out (DPEO) */
    addr = soc_reg_addr(mt_ctrl->unit, tr_mt_data->etc_ctl, REG_PORT_ANY, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_read(mt_ctrl->unit, addr, &rval));
    soc_reg_field_set(mt_ctrl->unit, tr_mt_data->etc_ctl, &rval,
                      OUT_DPR_ODD_FALLf, 3);
    soc_reg_field_set(mt_ctrl->unit, tr_mt_data->etc_ctl, &rval,
                      OUT_DPR_ODD_RISEf, 3);
    SOC_IF_ERROR_RETURN(soc_reg32_write(mt_ctrl->unit, addr, rval));

    mt_data->sub_interface = 1;
    for (mt_ctrl->dpeo_sync_dly_cur = mt_ctrl->dpeo_sync_dly_min;
         mt_ctrl->dpeo_sync_dly_cur <= mt_ctrl->dpeo_sync_dly_max;
         mt_ctrl->dpeo_sync_dly_cur++) {
        for (mt_ctrl->fcd_dpeo_cur = mt_ctrl->fcd_dpeo_min;
             mt_ctrl->fcd_dpeo_cur <= mt_ctrl->fcd_dpeo_max;
             mt_ctrl->fcd_dpeo_cur++) {
            rv = (*mt_ctrl->prog_hw1_fn)(mt_data);
            if (rv < 0) {
                SOC_ERROR_PRINT((DK_ERR, "unit %d %s prog hardware failure: "
                                 "%s\n",
                                 mt_ctrl->unit, mt_ctrl->intf_name,
                                 soc_errmsg(rv)));
                return rv;
            }
            mt_ctrl->cur_fail_count = 0;
            rv = (*mt_ctrl->test_fn)(mt_data);
            if (rv < 0) {
                SOC_ERROR_PRINT((DK_ERR, "unit %d %s dpeo test failure: %s\n",
                                 mt_ctrl->unit, mt_ctrl->intf_name,
                                 soc_errmsg(rv)));
                return rv;
            }
            if (!mt_ctrl->cur_fail_count) {
                break;
            }
        } /* mt_ctrl->fcd_dpeo_cur loop */
        if (!mt_ctrl->cur_fail_count) {
            break;
        }
    } /* mt_ctrl->dpeo_sync_dly_cur loop */
    addr = soc_reg_addr(mt_ctrl->unit, tr_mt_data->etc_ctl, REG_PORT_ANY, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_read(mt_ctrl->unit, addr, &rval));
    soc_reg_field_set(mt_ctrl->unit, tr_mt_data->etc_ctl, &rval,
                      OUT_DPR_ODD_FALLf, 0);
    soc_reg_field_set(mt_ctrl->unit, tr_mt_data->etc_ctl, &rval,
                      OUT_DPR_ODD_RISEf, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_write(mt_ctrl->unit, addr, rval));
    if (mt_ctrl->cur_fail_count) {
        fail_name = "DPEO";
        goto done;
    }

    /* setup for RBUS test */
    if (!tcam_info->num_tcams) {
        for (index = 0; index < TCAM_PARTITION_COUNT; index++) {
            tcam_info->partitions[index].num_entries = 0;
        }
        tcam_info->partitions[TCAM_PARTITION_FWD_L2].num_entries = 16384;
        tcam_info->partitions[TCAM_PARTITION_FWD_IP4].num_entries = 16384;
        tcam_info->partitions[TCAM_PARTITION_ACL_L2C].num_entries = 8192;
        soc_tcam_init(mt_ctrl->unit);
        sal_memset(mask, 0, sizeof(mask));
        sal_memset(data, 0, sizeof(data));
        for (index = 0; index < 16384; index++) {
            soc_tcam_write_entry(mt_ctrl->unit, TCAM_PARTITION_FWD_L2, index,
                                 mask, data);
        }
        for (index = 0; index < 8192; index++) {
            soc_tcam_write_entry(mt_ctrl->unit, TCAM_PARTITION_ACL_L2C, index,
                                 mask, data);
        }
        index = 0x1555;
        if (mt_data->mask[0] == 0xffffffff) {
            /* all '1's for lower 72-bit, all '0's for upper 72-bit */
            data[5] = 0xff;
            data[6] = data[7] = 0xffffffff;
        } else {
            sal_memcpy(mask, mt_data->mask, sizeof(mask));
            sal_memcpy(data, mt_data->data, sizeof(data));
        }
        soc_tcam_write_entry(mt_ctrl->unit, TCAM_PARTITION_FWD_L2, index,
                             &mask[4], &data[4]);
        soc_tcam_write_entry(mt_ctrl->unit, TCAM_PARTITION_ACL_L2C, index,
                             mask, data);
    }

    addr = soc_reg_addr(mt_ctrl->unit, tr_mt_data->etc_ctl, REG_PORT_ANY, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_read(mt_ctrl->unit, addr, &rval));
    soc_reg_field_set(mt_ctrl->unit, tr_mt_data->etc_ctl, &rval, IN_SMFL00f,
                      0);
    SOC_IF_ERROR_RETURN(soc_reg32_write(mt_ctrl->unit, addr, rval));

    mt_data->sub_interface = 2;
    for (mt_ctrl->rbus_sync_dly_cur = mt_ctrl->rbus_sync_dly_min;
         mt_ctrl->rbus_sync_dly_cur <= mt_ctrl->rbus_sync_dly_max;
         mt_ctrl->rbus_sync_dly_cur++) {
        for (mt_ctrl->fcd_rbus_cur = mt_ctrl->fcd_rbus_min;
             mt_ctrl->fcd_rbus_cur <= mt_ctrl->fcd_rbus_max;
             mt_ctrl->fcd_rbus_cur++) {
            rv = (*mt_ctrl->prog_hw1_fn)(mt_data);
            if (rv < 0) {
                SOC_ERROR_PRINT((DK_ERR, "unit %d %s prog hardware failure: "
                                 "%s\n",
                                 mt_ctrl->unit, mt_ctrl->intf_name,
                                 soc_errmsg(rv)));
                return rv;
            }
            mt_ctrl->cur_fail_count = 0;
            rv = (*mt_ctrl->test_fn)(mt_data);
            if (rv < 0) {
                SOC_ERROR_PRINT((DK_ERR, "unit %d %s rbus test failure: %s\n",
                                 mt_ctrl->unit, mt_ctrl->intf_name,
                                 soc_errmsg(rv)));
                return rv;
            }
            if (mt_ctrl->cur_fail_count == mt_data->test_count + 3) {
                continue;
            } else if (mt_ctrl->cur_fail_count == mt_data->test_count + 2) {
                break;
            }
            if (!mt_ctrl->cur_fail_count) {
                break;
            }
        }
        if (!mt_ctrl->cur_fail_count) {
            break;
        }
    }
    addr = soc_reg_addr(mt_ctrl->unit, tr_mt_data->etc_ctl, REG_PORT_ANY, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_read(mt_ctrl->unit, addr, &rval));
    soc_reg_field_set(mt_ctrl->unit, tr_mt_data->etc_ctl, &rval, IN_SMFL00f,
                      1);
    SOC_IF_ERROR_RETURN(soc_reg32_write(mt_ctrl->unit, addr, rval));
    if (mt_ctrl->cur_fail_count) {
        fail_name = "RBUS";
        goto done;
    }

    /* search test */
    mt_data->sub_interface = 3;
    mt_ctrl->cur_fail_count = 0;
    rv = (*mt_ctrl->test_fn)(mt_data);
    if (rv < 0) {
        SOC_ERROR_PRINT((DK_ERR, "unit %d %s search test failure: %s\n",
                         mt_ctrl->unit, mt_ctrl->intf_name, soc_errmsg(rv)));
        return rv;
    }

 done:
    if ((!(mt_ctrl->flags & SOC_MEMTUNE_CTRL_FLAGS_SHOW_PASS) &&
         mt_ctrl->cur_fail_count < mt_data->test_count) ||
        (!(mt_ctrl->flags & SOC_MEMTUNE_CTRL_FLAGS_SHOW_FAIL) &&
         mt_ctrl->cur_fail_count >= mt_data->test_count)) {
        return SOC_E_NONE;
    }

    if (mt_ctrl->tx_offset_cur != -1) {
        soc_cm_print("TX offset=%d ", mt_ctrl->tx_offset_cur);
    }
    if (mt_ctrl->rx_offset_cur != -1) {
        soc_cm_print("RX offset=%d ", mt_ctrl->rx_offset_cur);
    }
    if (mt_ctrl->dpeo_sync_dly_cur != -1) {
        soc_cm_print("DPEO_SYNC_DLY=%d ", mt_ctrl->dpeo_sync_dly_cur);
    }
    if (mt_ctrl->fcd_dpeo_cur != -1) {
        soc_cm_print("FCD_DPEO=%d ", mt_ctrl->fcd_dpeo_cur);
    }
    if (mt_ctrl->rbus_sync_dly_cur != -1) {
        soc_cm_print("RBUS_SYNC_DLY=%d ", mt_ctrl->rbus_sync_dly_cur);
    }
    if (mt_ctrl->fcd_rbus_cur != -1) {
        soc_cm_print("FCD_RBUS=%d ", mt_ctrl->fcd_rbus_cur);
    }
    if (fail_name) {
        soc_cm_print(": %s test FAIL\n", fail_name);
    } else {
        soc_cm_print(": Fail count=%d\n", mt_ctrl->cur_fail_count);
    }

    return SOC_E_NONE;
}

STATIC int
_soc_memtune_tcam_main(soc_memtune_data_t *mt_data)
{
    soc_tcam_info_t *tcam_info;
    soc_memtune_ctrl_t *mt_ctrl = mt_data->mt_ctrl;
    soc_memtune_result_t *result;
    int rv, offset_count, index;
    uint32 rval;

    tcam_info = SOC_CONTROL(mt_ctrl->unit)->tcam_info;

    if (mt_ctrl->flags & SOC_MEMTUNE_CTRL_FLAGS_SHOW_TEST_NAME) {
        soc_cm_print("TCAM tuning test: unit %d interface %s:\n",
                     mt_ctrl->unit, mt_ctrl->intf_name);
    }

    if (mt_data->manual_settings) {
        mt_ctrl->tx_offset_cur = mt_data->man_tx_offset;
        mt_ctrl->rx_offset_cur = mt_data->man_rx_offset;
        rv = _soc_memtune_tcam_single_test(mt_data);
        if (SOC_FAILURE(rv) || mt_ctrl->cur_fail_count) {
            soc_cm_print("!!! TCAM tuning FAIL !!!\n");
        } else if (mt_data->config) {
#ifdef BCM_TRIUMPH_SUPPORT
            if (soc_feature(mt_ctrl->unit, soc_feature_esm_support)) {
                SOC_IF_ERROR_RETURN(_soc_tr_tune_generate_config(mt_data));
            }
#endif /* BCM_TRIUMPH_SUPPORT */
        }
        return SOC_E_NONE;
    }

    offset_count = (mt_ctrl->tx_offset_max - mt_ctrl->tx_offset_min + 1) *
                   (mt_ctrl->rx_offset_max - mt_ctrl->rx_offset_min + 1);
    mt_ctrl->fail_array = sal_alloc(offset_count * sizeof(int),
                                    "memtune working data");
    if (mt_ctrl->fail_array == NULL) {
        return SOC_E_MEMORY;
    }
    sal_memset(mt_ctrl->fail_array, 0, offset_count * sizeof(int));

    for (mt_ctrl->tx_offset_cur = mt_ctrl->tx_offset_min;
         mt_ctrl->tx_offset_cur <= mt_ctrl->tx_offset_max;
         mt_ctrl->tx_offset_cur++) {
        for (mt_ctrl->rx_offset_cur = mt_ctrl->rx_offset_min;
             mt_ctrl->rx_offset_cur <= mt_ctrl->rx_offset_max;
             mt_ctrl->rx_offset_cur++) {
            rv = _soc_memtune_tcam_single_test(mt_data);
            index = (mt_ctrl->tx_offset_cur - mt_ctrl->tx_offset_min) *
                (mt_ctrl->rx_offset_max - mt_ctrl->rx_offset_min + 1) +
                mt_ctrl->rx_offset_cur - mt_ctrl->rx_offset_min;
            mt_ctrl->fail_array[index] =
                mt_ctrl->cur_fail_count > mt_data->test_count ?
                mt_data->test_count : mt_ctrl->cur_fail_count;
        } /* mt_ctrl->rx_offset_cur loop */
    } /* mt_ctrl->tx_offset_cur loop */

    /* Analyze and print result matrix */
    mt_ctrl->cur_result_count = 0;
    _soc_memtune_txrx_analyze(mt_data);
    result = &mt_ctrl->result[0];

    if (result->fail_count == offset_count) {
        soc_cm_print("!!! TCAM tuning FAIL !!!\n");
    } else {
        SOC_IF_ERROR_RETURN(READ_CMIC_SOFT_RESET_REGr(mt_ctrl->unit, &rval));
        soc_reg_field_set(mt_ctrl->unit, CMIC_SOFT_RESET_REGr, &rval,
                          CMIC_TCAM_RST_Lf, 0);
        SOC_IF_ERROR_RETURN(WRITE_CMIC_SOFT_RESET_REGr(mt_ctrl->unit, rval));
        sal_usleep(1000);
        soc_reg_field_set(mt_ctrl->unit, CMIC_SOFT_RESET_REGr, &rval,
                          EXT_TCAM_RSTf, 1);
        SOC_IF_ERROR_RETURN(WRITE_CMIC_SOFT_RESET_REGr(mt_ctrl->unit, rval));
        sal_usleep(100000);
        SOC_IF_ERROR_RETURN(READ_CMIC_SOFT_RESET_REGr(mt_ctrl->unit, &rval));
        soc_reg_field_set(mt_ctrl->unit, CMIC_SOFT_RESET_REGr, &rval,
                          EXT_TCAM_RSTf, 0);
        SOC_IF_ERROR_RETURN(WRITE_CMIC_SOFT_RESET_REGr(mt_ctrl->unit, rval));
        sal_usleep(1000);
        soc_reg_field_set(mt_ctrl->unit, CMIC_SOFT_RESET_REGr, &rval,
                          CMIC_TCAM_RST_Lf, 1);
        SOC_IF_ERROR_RETURN(WRITE_CMIC_SOFT_RESET_REGr(mt_ctrl->unit, rval));
        sal_usleep(100000);
        tcam_info->num_tcams = 0;

        rv = _soc_memtune_tcam_single_test(mt_data);
        if (SOC_FAILURE(rv) || mt_ctrl->cur_fail_count) {
            soc_cm_print("!!! TCAM tuning FAIL --- unstable result !!!\n");
            result->fail_count = offset_count;
        } else {
            if (mt_ctrl->flags & SOC_MEMTUNE_CTRL_FLAGS_SHOW_SELECTION) {
                soc_cm_print("TCAM tuning selection: ");
                if (mt_ctrl->tx_offset_cur != -1) {
                    soc_cm_print("TX offset=%d ", mt_ctrl->tx_offset_cur);
                }
                if (mt_ctrl->rx_offset_cur != -1) {
                    soc_cm_print("RX offset=%d ", mt_ctrl->rx_offset_cur);
                }
                if (mt_ctrl->dpeo_sync_dly_cur != -1) {
                    soc_cm_print("dpeo_sync_dly=%d ",
                                 mt_ctrl->dpeo_sync_dly_cur);
                }
                if (mt_ctrl->fcd_dpeo_cur != -1) {
                    soc_cm_print("fcd_dpeo=%d ", mt_ctrl->fcd_dpeo_cur);
                }
                if (mt_ctrl->rbus_sync_dly_cur != -1) {
                    soc_cm_print("rbus_sync_dly=%d ",
                                 mt_ctrl->rbus_sync_dly_cur);
                }
                if (mt_ctrl->fcd_rbus_cur != -1) {
                    soc_cm_print("fcd_rbus=%d ", mt_ctrl->fcd_rbus_cur);
                }
                soc_cm_print("\n");
            }
        }
    }

    if (mt_ctrl->flags & SOC_MEMTUNE_CTRL_FLAGS_SHOW_SUMMARY) {
        if (mt_ctrl->flags &
            SOC_MEMTUNE_CTRL_FLAGS_SHOW_SUMMARY_HEADER) {
            soc_cm_print("\n Interface  , Area , Width , Height ,"
                         "  TX ,  RX , Pass , Fail ,"
                         " Freq , Sb , Dac , Dist\n");
        }
        soc_cm_print(" %10s , %3d  ,  %3d  ,  %3d   ,",
                     mt_ctrl->intf_name, result->width * result->height,
                     result->width, result->height);
        soc_cm_print("  %2d ,  %2d ,",
                     result->tx_offset, result->rx_offset);
        soc_cm_print(" %4d , %4d ,  %3d ,  %c ,",
                     offset_count - result->fail_count, result->fail_count,
                     mt_data->freq,
                     mt_data->bg_sram_bist ? 'Y' : 'N');
        if (mt_ctrl->dac_value != -1) {
            soc_cm_print("  %2d ,", mt_ctrl->dac_value);
        } else {
            soc_cm_print("  ** ,");
        }
        soc_cm_print("  %2d", mt_ctrl->ptr_dist);
        soc_cm_print("\n");
    }

    if (result->fail_count != offset_count) {
        if (mt_data->config) {
            rv = _soc_tr_tune_generate_config(mt_data);
            if (rv < 0) {
                SOC_ERROR_PRINT((DK_ERR,
                                 "unit %d %s fail to generate config: %s\n",
                                 mt_ctrl->unit, mt_ctrl->intf_name,
                                 soc_errmsg(rv)));
            }
        }
    }

    if (mt_ctrl->fail_array) {
        sal_free(mt_ctrl->fail_array);
    }

    return SOC_E_NONE;
}
#endif /* BCM_TRIUMPH_SUPPORT */

#ifdef BCM_EASYRIDER_SUPPORT
STATIC int
_soc_er_mem_interface_tune(int unit, soc_memtune_data_t *mt_data)
{
    soc_memtune_ctrl_t  *mt_ctrl = mt_data->mt_ctrl;
    soc_er_memtune_data_t *er_mt_data;
    sop_memcfg_er_t     *mcer = NULL;
    soc_persist_t       *sop;

    er_mt_data = sal_alloc(sizeof(soc_er_memtune_data_t),
                           "memtune working data");
    if (!er_mt_data) {
        return SOC_E_MEMORY;
    }
    sal_memset(er_mt_data, 0, sizeof(soc_er_memtune_data_t));
    mt_ctrl->data = er_mt_data;

    sop = SOC_PERSIST(unit);
    if (sop) {
        mcer = &(sop->er_memcfg);
    }

    mt_ctrl->prog_hw1_fn = _soc_er_ddr_prog_hw_cb;
    mt_ctrl->prog_hw2_fn = _soc_er_phase_sel_prog_hw_cb;
    mt_ctrl->test_fn = _soc_er_rldram_tune_test_cb;

    mt_ctrl->phase_sel_min =
        mt_data->phase_sel_ovrd ? SOC_ER_PHASE_SEL_MIN : -1;
    mt_ctrl->phase_sel_max =
        mt_data->phase_sel_ovrd ? SOC_ER_PHASE_SEL_MAX : -1;
    mt_ctrl->em_latency_min = SOC_ER_EM_LATENCY_MIN;
    mt_ctrl->em_latency_max = SOC_ER_EM_LATENCY_MAX;
    mt_ctrl->ddr_latency_min = SOC_ER_RLDRAM_DDR_LATENCY_MIN;
    mt_ctrl->ddr_latency_max = SOC_ER_DDR_LATENCY_MAX;
    mt_ctrl->tx_offset_min = SOC_ER_RLDRAM_TX_OFFSET_MIN;
    mt_ctrl->tx_offset_max = SOC_ER_RLDRAM_TX_OFFSET_MAX;
    mt_ctrl->rx_offset_min = SOC_ER_RLDRAM_RX_OFFSET_MIN;
    mt_ctrl->rx_offset_max = SOC_ER_RLDRAM_RX_OFFSET_MAX;
    mt_ctrl->r2w_nops_max = -1;
    mt_ctrl->w2r_nops_max = -1;
    mt_ctrl->bist_poll_count = SOC_ER_RLDRAM_BIST_POLL_COUNT;

    switch (mt_data->interface) {
    case SOC_MEM_INTERFACE_RLDRAM_CH0:
        mt_ctrl->intf_name = "RLDRAM Ch0";
        mt_ctrl->em_latency_min = -1;
        er_mt_data->ddr_reg1 = MCU_CHN0_DDR_REG1r;
        er_mt_data->ddr_reg2 = MCU_CHN0_DDR_REG2r;
        er_mt_data->ddr_reg3 = MCU_CHN0_DDR_REG3r;
        er_mt_data->timing_reg = MCU_CHN0_TIMINGr;
        er_mt_data->req_cmd = MCU_CHN0_REQ_CMDr;
        er_mt_data->bist_ctrl = MCU_CHN0_BIST_CTRLr;
        er_mt_data->write_mem = C0_CELLm;
        er_mt_data->channel = 0;
        break;
    case SOC_MEM_INTERFACE_RLDRAM_CH1:
        mt_ctrl->intf_name = "RLDRAM Ch1";
        mt_ctrl->em_latency_min = -1;
        er_mt_data->ddr_reg1 = MCU_CHN1_DDR_REG1r;
        er_mt_data->ddr_reg2 = MCU_CHN1_DDR_REG2r;
        er_mt_data->ddr_reg3 = MCU_CHN1_DDR_REG3r;
        er_mt_data->timing_reg = MCU_CHN1_TIMINGr;
        er_mt_data->req_cmd = MCU_CHN1_REQ_CMDr;
        er_mt_data->bist_ctrl = MCU_CHN1_BIST_CTRLr;
        er_mt_data->write_mem = C1_CELLm;
        er_mt_data->channel = 1;
        break;
    case SOC_MEM_INTERFACE_SRAM:
        mt_ctrl->intf_name = "SRAM";
        mt_ctrl->test_fn = _soc_er_sram_tune_test_cb;
        mt_ctrl->ddr_latency_min = SOC_ER_SEER_DDR_LATENCY_MIN;
        er_mt_data->ddr_reg1 = DDR72_CONFIG_REG1_ISr;
        er_mt_data->ddr_reg2 = DDR72_CONFIG_REG2_ISr;
        er_mt_data->ddr_reg3 = DDR72_CONFIG_REG3_ISr;
        er_mt_data->timing_reg = INVALIDr;
        er_mt_data->req_cmd = INVALIDr;
        er_mt_data->bist_ctrl = INVALIDr;
        er_mt_data->write_mem = INVALIDm;
        er_mt_data->channel = -1;
        break;
    case SOC_MEM_INTERFACE_QDR:
        mt_ctrl->intf_name = "QDR";
        if (mcer) {
            if ((mcer->ext_table_cfg == 1) || (mcer->ext_table_cfg == 2)) {
                mt_ctrl->test_fn = _soc_er_tb2_sram_tune_test_cb;
            } else {
                if (mcer->tcam_select == ER_EXT_TCAM_TYPE1) {
                    mt_ctrl->test_fn = _soc_er_nl_tcam_tune_test_cb;
                } else {
                    mt_ctrl->test_fn = _soc_er_cyp_tcam_tune_test_cb;
                }
            }
        } else {
            /* Assume external SRAM is attached */
            mt_ctrl->test_fn = _soc_er_tb2_sram_tune_test_cb;
        }
        mt_ctrl->ddr_latency_min = SOC_ER_SEER_DDR_LATENCY_MIN;
        er_mt_data->ddr_reg1 = QDR36_CONFIG_REG1_ISr;
        er_mt_data->ddr_reg2 = QDR36_CONFIG_REG2_ISr;
        er_mt_data->ddr_reg3 = QDR36_CONFIG_REG3_ISr;
        er_mt_data->timing_reg = INVALIDr;
        er_mt_data->req_cmd = INVALIDr;
        er_mt_data->bist_ctrl = INVALIDr;
        er_mt_data->write_mem = INVALIDm;
        er_mt_data->channel = -1;
        break;
    default:
        return SOC_E_UNAVAIL;
    }

    return _soc_memtune_ddr_main(mt_data);
}
#endif /* BCM_EASYRIDER_SUPPORT */

#ifdef BCM_TRIUMPH_SUPPORT
STATIC int
_soc_tr_mem_interface_tune_setup(soc_memtune_data_t *mt_data)
{
    soc_memtune_ctrl_t  *mt_ctrl = mt_data->mt_ctrl;
    soc_tr_memtune_data_t *tr_mt_data = mt_ctrl->data;
    uint32 rval;

    switch (mt_data->interface) {
    case SOC_MEM_INTERFACE_SRAM:
        switch (mt_data->sub_interface) {
        case 0:
            mt_ctrl->intf_name = "SRAM0";
            tr_mt_data->config_reg1 = ES0_DDR36_CONFIG_REG1_ISr;
            tr_mt_data->config_reg2 = ES0_DDR36_CONFIG_REG2_ISr;
            tr_mt_data->config_reg3 = ES0_DDR36_CONFIG_REG3_ISr;
            tr_mt_data->status_reg2 = ES0_DDR36_STATUS_REG2_ISr;
            tr_mt_data->mode = ES0_DTU_MODEr;
            tr_mt_data->tmode0 = ES0_DTU_LTE_TMODE0r;
            tr_mt_data->tmode0_other_sram = ES1_DTU_LTE_TMODE0r;
            tr_mt_data->sram_ctl = ES0_SRAM_CTLr;
            break;
        case 1:
            mt_ctrl->intf_name = "SRAM1";
            tr_mt_data->config_reg1 = ES1_DDR36_CONFIG_REG1_ISr;
            tr_mt_data->config_reg2 = ES1_DDR36_CONFIG_REG2_ISr;
            tr_mt_data->config_reg3 = ES1_DDR36_CONFIG_REG3_ISr;
            tr_mt_data->status_reg2 = ES1_DDR36_STATUS_REG2_ISr;
            tr_mt_data->mode = ES1_DTU_MODEr;
            tr_mt_data->tmode0 = ES1_DTU_LTE_TMODE0r;
            tr_mt_data->tmode0_other_sram = ES0_DTU_LTE_TMODE0r;
            tr_mt_data->sram_ctl = ES1_SRAM_CTLr;
            break;
        default:
            return SOC_E_PARAM;
        }

        mt_ctrl->prog_hw1_fn = _soc_tr_ddr_prog_hw_cb;
        mt_ctrl->prog_hw2_fn = _soc_tr_phase_sel_prog_hw_cb;
        mt_ctrl->prog_hw3_fn = _soc_tr_ddr_nops_prog_hw_cb;
        mt_ctrl->test_fn = _soc_tr_sram_tune_test_cb;

        mt_ctrl->phase_sel_min =
            mt_data->phase_sel_ovrd ? SOC_TR_PHASE_SEL_MIN : -1;
        mt_ctrl->phase_sel_max =
            mt_data->phase_sel_ovrd ? SOC_TR_PHASE_SEL_MAX : -1;
        mt_ctrl->em_latency_min = SOC_TR_EM_LATENCY_MIN;
        mt_ctrl->em_latency_max = SOC_TR_EM_LATENCY_MAX;
        mt_ctrl->ddr_latency_min = SOC_TR_DDR_LATENCY_MIN;
        mt_ctrl->ddr_latency_max = SOC_TR_DDR_LATENCY_MAX;
        mt_ctrl->tx_offset_min = SOC_TR_SRAM_TX_OFFSET_MIN;
        mt_ctrl->tx_offset_max = SOC_TR_SRAM_TX_OFFSET_MAX;
        mt_ctrl->rx_offset_min = SOC_TR_SRAM_RX_OFFSET_MIN;
        mt_ctrl->rx_offset_max = SOC_TR_SRAM_RX_OFFSET_MAX;
        mt_ctrl->w2r_nops_min = SOC_TR_SRAM_W2R_NOPS_MIN;
        mt_ctrl->w2r_nops_max = SOC_TR_SRAM_W2R_NOPS_MAX;
        mt_ctrl->r2w_nops_min = SOC_TR_SRAM_R2W_NOPS_MIN;
        mt_ctrl->r2w_nops_max = SOC_TR_SRAM_R2W_NOPS_MAX;
        break;

    case SOC_MEM_INTERFACE_TCAM:
        mt_ctrl->intf_name = "TCAM";
        tr_mt_data->config_reg1 = ETU_DDR72_CONFIG_REG1_ISr;
        tr_mt_data->config_reg3 = ETU_DDR72_CONFIG_REG3_ISr;
        tr_mt_data->etc_ctl = ETC_CTLr;
        tr_mt_data->inst_status = ETU_ET_INST_STATUSr;

        mt_ctrl->prog_hw1_fn = _soc_tr_tcam_prog_hw_cb;
        mt_ctrl->test_fn = _soc_tr_tcam_tune_test_cb;

        SOC_IF_ERROR_RETURN
            (READ_ETU_DDR72_CONFIG_REG1_ISr(mt_ctrl->unit, &rval));
        /* software always select the same clock source for TX and RX */
        if (!mt_data->manual_settings) {
            mt_ctrl->tx_offset_min = SOC_TR_TCAM_TX_OFFSET_MIN;
            mt_ctrl->rx_offset_min = SOC_TR_TCAM_RX_OFFSET_MIN;
            if (soc_reg_field_get(mt_ctrl->unit, ETU_DDR72_CONFIG_REG1_ISr,
                                  rval, MIDL_TX_ENf) &&
                soc_reg_field_get(mt_ctrl->unit, ETU_DDR72_CONFIG_REG1_ISr,
                                  rval, SEL_TX_CLKDLY_BLKf)) {
                mt_ctrl->tx_offset_max = SOC_TR_TCAM_MIDL_TX_OFFSET_MAX;
                mt_ctrl->rx_offset_max = SOC_TR_TCAM_MIDL_RX_OFFSET_MAX;
            } else {
                mt_ctrl->tx_offset_max = SOC_TR_TCAM_VCDL_TX_OFFSET_MAX;
                mt_ctrl->rx_offset_max = SOC_TR_TCAM_VCDL_RX_OFFSET_MAX;
            }
            if (mt_data->man_tx_offset != -1) {
                mt_ctrl->tx_offset_min = mt_data->man_tx_offset;
                mt_ctrl->tx_offset_max = mt_data->man_tx_offset;
            }
            if (mt_data->man_rx_offset != -1) {
                mt_ctrl->rx_offset_min = mt_data->man_rx_offset;
                mt_ctrl->rx_offset_max = mt_data->man_rx_offset;
            }
        }

        mt_ctrl->dpeo_sel_cur = 0; /* do DPEO_0 only */
        mt_ctrl->dpeo_sync_dly_min = SOC_TR_DPEO_SYNC_DLY_MIN;
        mt_ctrl->dpeo_sync_dly_max = SOC_TR_DPEO_SYNC_DLY_MAX;
        mt_ctrl->fcd_dpeo_min = SOC_TR_FCD_DPEO_MIN;
        mt_ctrl->fcd_dpeo_max = SOC_TR_FCD_DPEO_MAX;

        mt_ctrl->rbus_sync_dly_min = SOC_TR_RBUS_SYNC_DLY_MIN;
        mt_ctrl->rbus_sync_dly_max = SOC_TR_RBUS_SYNC_DLY_MAX;
        mt_ctrl->fcd_rbus_min = SOC_TR_FCD_RBUS_MIN;
        mt_ctrl->fcd_rbus_max = SOC_TR_FCD_RBUS_MAX;
        break;

    default:
        return SOC_E_PARAM;
    }

    return SOC_E_NONE;
}

STATIC int
_soc_tr_mem_interface_tune(int unit, soc_memtune_data_t *mt_data)
{
    soc_tcam_info_t *tcam_info;
    soc_memtune_ctrl_t *mt_ctrl = mt_data->mt_ctrl;
    soc_tr_memtune_data_t *tr_mt_data;
    tr_ext_sram_bist_t *sram_bist, bg_sram_bist;
    soc_reg_t reg;
    uint32 addr, rval, sub_interface_mask;
    int rv, i;

    tcam_info = SOC_CONTROL(unit)->tcam_info;
    if (tcam_info == NULL) {
        return SOC_E_INIT;
    }

    tr_mt_data = sal_alloc(sizeof(soc_tr_memtune_data_t),
                           "memtune working data");
    if (!tr_mt_data) {
        return SOC_E_MEMORY;
    }
    sal_memset(tr_mt_data, 0, sizeof(soc_tr_memtune_data_t));
    mt_ctrl->data = tr_mt_data;

    sram_bist = sal_alloc(sizeof(tr_ext_sram_bist_t), "memtune working data");
    if (!sram_bist) {
        return SOC_E_MEMORY;
    }
    sal_memset(sram_bist, 0, sizeof(tr_ext_sram_bist_t));
    mt_ctrl->sram_bist = sram_bist;

    if (mt_data->bg_sram_bist) {
        sal_memset(&bg_sram_bist, 0, sizeof(bg_sram_bist));
        if (mt_data->bg_d0r_0 == 0xffffffff) {
            bg_sram_bist.d0r_0 = bg_sram_bist.d0r_1 = 0x3ffff;
            bg_sram_bist.d0f_0 = bg_sram_bist.d0f_1 = 0;
            bg_sram_bist.d1r_0 = bg_sram_bist.d1r_1 = 0x3ffff;
            bg_sram_bist.d1f_0 = bg_sram_bist.d1f_1 = 0;
        } else {
            bg_sram_bist.d0r_0 = mt_data->bg_d0r_0 & 0x3ffff;
            bg_sram_bist.d0r_1 = mt_data->bg_d0r_1 & 0x3ffff;
            bg_sram_bist.d0f_0 = mt_data->bg_d0f_0 & 0x3ffff;
            bg_sram_bist.d0f_1 = mt_data->bg_d0f_1 & 0x3ffff;
            bg_sram_bist.d1r_0 = mt_data->bg_d1r_0 & 0x3ffff;
            bg_sram_bist.d1r_1 = mt_data->bg_d1r_1 & 0x3ffff;
            bg_sram_bist.d1f_0 = mt_data->bg_d1f_0 & 0x3ffff;
            bg_sram_bist.d1f_1 = mt_data->bg_d1f_1 & 0x3ffff;
        }
        if (mt_data->bg_adr0 == -1) {
            bg_sram_bist.adr0 = 0x0;   /* starting address */
        } else {
            bg_sram_bist.adr0 = mt_data->bg_adr0 & 0x3ffffe;
        }
        if (mt_data->bg_adr1 == -1 || mt_data->bg_adr_mode == 3) {
            bg_sram_bist.adr1 = 0x10;  /* end address */
        } else {
            bg_sram_bist.adr1 = mt_data->bg_adr1 & 0x3ffffe;
        }
        if (mt_data->bg_loop_mode == -1) {
            bg_sram_bist.loop_mode = 3; /* WW_RR */
        } else {
            bg_sram_bist.loop_mode = mt_data->bg_loop_mode & 3;
        }
        if (mt_data->bg_adr_mode == -1) {
            bg_sram_bist.adr_mode = 2; /* from ADR0 to ADR1 w/ step of 2 */
        } else {
            bg_sram_bist.adr_mode = mt_data->bg_adr_mode & 3;
        }
        bg_sram_bist.em_latency = -1;
        bg_sram_bist.w2r_nops = -1;
        bg_sram_bist.r2w_nops = -1;
    }

    switch (mt_data->interface) {
    case SOC_MEM_INTERFACE_SRAM:
        if (mt_data->sub_interface > 1) {
            return SOC_E_PARAM;
        } else if (mt_data->sub_interface < 0) {
            sub_interface_mask = 0x3;
        } else {
            sub_interface_mask = 1 << mt_data->sub_interface;
        }

        /* (Optionally) program frequency */
        if (mt_data->freq != -1) {
            (void)soc_triumph_esm_init_set_sram_freq(unit, mt_data->freq);
        } else {
            mt_data->freq = tcam_info->sram_freq;
            soc_cm_print("Using SRAM frequency %d\n", tcam_info->sram_freq);
        }

        for (i = 0; i < 2; i++) {
            if (!(sub_interface_mask & (1 << i))) {
                continue;
            }
            mt_data->sub_interface = i;
            SOC_IF_ERROR_RETURN
                (_soc_tr_mem_interface_tune_setup(mt_data));

            /* Setup sram_bist control structure
             *          --   ---------   ---------   ---------   ---------   --
             *            \ /         \ /         \ /         \ /         \ /
             * DQ[35:18]   X   D0R_1   X   D0F_1  X    D1R_1   X   D1F_1   X
             *            / \         / \         / \         / \         / \
             *          --   ---------   ---------   ---------   ---------   --
             *
             *          --   ---------   ---------   ---------   ---------   --
             *            \ /         \ /         \ /         \ /         \ /
             * DQ[17:0]    X   D0R_0   X   D0F_0   X   D1R_0   X   D1F_0   X
             *            / \         / \         / \         / \         / \
             *          --   ---------   ---------   ---------   ---------   --
             */
            if (mt_data->d0r_0 == 0xffffffff) {
                sram_bist->d1r_0 = sram_bist->d0r_0 = 0xffffffff & 0x3ffff;
                sram_bist->d1r_1 = sram_bist->d0r_1 = 0x55555555 & 0x3ffff;
                sram_bist->d1f_0 = sram_bist->d0f_0 = 0x00000000 & 0x3ffff;
                sram_bist->d1f_1 = sram_bist->d0f_1 = 0xaaaaaaaa & 0x3ffff;
            } else {
                sram_bist->d0r_0 = mt_data->d0r_0 & 0x3ffff;
                sram_bist->d0r_1 = mt_data->d0r_1 & 0x3ffff;
                sram_bist->d0f_0 = mt_data->d0f_0 & 0x3ffff;
                sram_bist->d0f_1 = mt_data->d0f_1 & 0x3ffff;
                sram_bist->d1r_0 = mt_data->d1r_0 & 0x3ffff;
                sram_bist->d1r_1 = mt_data->d1r_1 & 0x3ffff;
                sram_bist->d1f_0 = mt_data->d1f_0 & 0x3ffff;
                sram_bist->d1f_1 = mt_data->d1f_1 & 0x3ffff;
            }
            if (mt_data->adr0 == -1) {
                sram_bist->adr0 = 0x0;   /* starting address */
            } else {
                sram_bist->adr0 = mt_data->adr0 & 0x3ffffe;
            }
            if (mt_data->adr1 == -1 || mt_data->adr_mode == 3) {
                sram_bist->adr1 = 0x10;  /* end address */
            } else {
                sram_bist->adr1 = mt_data->adr1 & 0x3ffffe;
            }
            if (mt_data->adr_mode == -1) {
                sram_bist->adr_mode = 2; /* from ADR0 to ADR1 w/ step of 2 */
            } else {
                sram_bist->adr_mode = mt_data->adr_mode & 3;
            }
            sram_bist->em_latency = -1;
            sram_bist->w2r_nops = -1;
            sram_bist->r2w_nops = -1;
            SOC_IF_ERROR_RETURN
                (soc_triumph_ext_sram_bist_setup(unit, mt_data->sub_interface,
                                                 sram_bist));

            /*
             * Following sram bist registers has been set properly,
             * don't touch them during test to save time:
             * D0R_0, D0R_1, D0F_0, D0F_1, D1R_0, D1R_1, D1F_0, D1F_1,
             * ADR0, ADR1, TMODE1
             */
            sram_bist->d0r_0 = 0xffffffff;
            sram_bist->d1r_0 = 0xffffffff;
            sram_bist->adr0 = -1;
            sram_bist->adr1 = -1;
            sram_bist->wdoebr = -1;

            /* Get BIASGEN DAC_VALUE */
            reg = tr_mt_data->config_reg3;
            addr = soc_reg_addr(unit, reg, REG_PORT_ANY, 0);
            SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &rval));
            mt_ctrl->dac_value = -1;
            if (soc_reg_field_get(unit, reg, rval, BIASGEN_DAC_ENf)) {
                mt_ctrl->dac_value = soc_reg_field_get(unit, reg, rval,
                                                       BIASGEN_DAC_CTRLf);
            }

            /* Optionally do PVT compensation */
            mt_ctrl->odtres_val = -1;
            mt_ctrl->pdrive_val = -1;
            mt_ctrl->ndrive_val = -1;
            mt_ctrl->slew_val = -1;
            if (mt_data->do_pvt_comp) {
                reg = tr_mt_data->config_reg2;
                SOC_IF_ERROR_RETURN
                    (soc_triumph_esm_init_pvt_comp(unit, reg,
                                                   tr_mt_data->status_reg2));
                addr = soc_reg_addr(unit, reg, REG_PORT_ANY, 0);
                SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &rval));
                if (soc_reg_field_get(unit, reg, rval, OVRD_EN_ODTRES_PVTf)) {
                    mt_ctrl->odtres_val = soc_reg_field_get(unit, reg, rval,
                                                            PVT_ODTRES_VALf);
                }
                if (soc_reg_field_get(unit, reg, rval, OVRD_EN_DRIVER_PVTf)) {
                    mt_ctrl->pdrive_val = soc_reg_field_get(unit, reg, rval,
                                                            PVT_PDRIVE_VALf);
                    mt_ctrl->ndrive_val = soc_reg_field_get(unit, reg, rval,
                                                            PVT_NDRIVE_VALf);
                }
                if (soc_reg_field_get(unit, reg, rval, OVRD_EN_SLEW_PVTf)) {
                    mt_ctrl->slew_val = soc_reg_field_get(unit, reg, rval,
                                                          PVT_SLEW_VALf);
                }
            }

            /* Optionally setup background TCAM BIST */
            if (mt_data->bg_tcam_bist) {
                soc_cm_print("Do TCAM BIST loop count %d\n",
                             mt_data->bg_tcam_loop_count);
                SOC_IF_ERROR_RETURN
                    (soc_tr_tcam_type1_memtest_dpeo(unit, 8,
                                                    mt_data->bg_tcam_oemap,
                                                    mt_data->bg_tcam_data));
                rval = 0;
                soc_reg_field_set(unit, ETU_LTE_BIST_CTLr, &rval,
                                  RD_EN_BIST_RSLTSf, 1);
                soc_reg_field_set(unit, ETU_LTE_BIST_CTLr, &rval, LOOP_COUNTf,
                                  mt_data->bg_tcam_loop_count);
                SOC_IF_ERROR_RETURN(WRITE_ETU_LTE_BIST_CTLr(unit, rval));
            }

            /* Optionally do background BIST on the other SRAM interface */
            if (mt_data->bg_sram_bist) {
                soc_cm_print("Do SRAM BIST on SRAM%d with loop mode %d\n",
                             mt_data->sub_interface ^ 1,
                             bg_sram_bist.loop_mode);
                soc_triumph_ext_sram_enable_set(unit,
                                                mt_data->sub_interface ^ 1,
                                                TRUE, TRUE);
                soc_triumph_ext_sram_bist_setup(unit,
                                                mt_data->sub_interface ^ 1,
                                                &bg_sram_bist);

                reg = tr_mt_data->tmode0_other_sram;
                addr = soc_reg_addr(unit, reg, REG_PORT_ANY, 0);
                rv = soc_reg32_read(unit, addr, &rval);
                if (SOC_SUCCESS(rv)) {
                    soc_reg_field_set(unit, reg, &rval, START_LAB_TESTf, 1);
                    soc_reg_field_set(unit, reg, &rval, FOREVERf, 1);
                    (void)soc_reg32_write(unit, addr, rval);
                }
            }

            rv = _soc_memtune_ddr_main(mt_data);

            if (mt_data->bg_sram_bist) {
                soc_reg_field_set(unit, reg, &rval, FOREVERf, 0);
                (void)soc_reg32_write(unit, addr, rval);
                soc_triumph_ext_sram_enable_set(unit,
                                                mt_data->sub_interface ^ 1,
                                                FALSE, TRUE);
            }

            if (mt_data->bg_tcam_bist) {
                rval = 0;
                soc_reg_field_set(unit, ETU_LTE_BIST_CTLr, &rval, LOOP_COUNTf,
                                  1);
                SOC_IF_ERROR_RETURN(WRITE_ETU_LTE_BIST_CTLr(unit, rval));
            }
        }
        break;

    case SOC_MEM_INTERFACE_TCAM:
        if (mt_data->sub_interface > 3) {
            return SOC_E_PARAM;
        }
        mt_data->sub_interface = 0;
        SOC_IF_ERROR_RETURN(_soc_tr_mem_interface_tune_setup(mt_data));

        SOC_IF_ERROR_RETURN(READ_CMIC_SOFT_RESET_REGr(unit, &rval));
        soc_reg_field_set(mt_ctrl->unit, CMIC_SOFT_RESET_REGr, &rval,
                          CMIC_TCAM_RST_Lf, 0);
        SOC_IF_ERROR_RETURN(WRITE_CMIC_SOFT_RESET_REGr(mt_ctrl->unit, rval));
        sal_usleep(1000);
        soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &rval, EXT_TCAM_RSTf, 1);
        SOC_IF_ERROR_RETURN(WRITE_CMIC_SOFT_RESET_REGr(unit, rval));

        if (mt_data->freq != -1) {
            (void)soc_triumph_esm_init_set_tcam_freq(mt_ctrl->unit,
                                                     mt_data->freq);
        } else {
            mt_data->freq = tcam_info->tcam_freq;
            soc_cm_print("Using TCAM frequency %d\n", tcam_info->tcam_freq);
        }

        sal_usleep(100000);
        SOC_IF_ERROR_RETURN(READ_CMIC_SOFT_RESET_REGr(unit, &rval));
        soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &rval, EXT_TCAM_RSTf, 0);
        SOC_IF_ERROR_RETURN(WRITE_CMIC_SOFT_RESET_REGr(unit, rval));
        sal_usleep(1000);
        soc_reg_field_set(mt_ctrl->unit, CMIC_SOFT_RESET_REGr, &rval,
                          CMIC_TCAM_RST_Lf, 1);
        SOC_IF_ERROR_RETURN(WRITE_CMIC_SOFT_RESET_REGr(mt_ctrl->unit, rval));
        sal_usleep(100000);
        tcam_info->num_tcams = 0;

        SOC_IF_ERROR_RETURN(READ_ETU_DDR72_CONFIG_REG2_ISr(unit, &rval));
        mt_ctrl->dac_value = -1;
        if (soc_reg_field_get(unit, ETU_DDR72_CONFIG_REG2_ISr, rval,
                              BIASGEN_DAC_ENf)) {
            mt_ctrl->dac_value =
                soc_reg_field_get(unit, ETU_DDR72_CONFIG_REG2_ISr, rval,
                                  BIASGEN_DAC_CTRLf);
        }

        SOC_IF_ERROR_RETURN(READ_ETU_DDR72_CONFIG_REG1_ISr(unit, &rval));
        mt_ctrl->ptr_dist =
            soc_reg_field_get(unit, ETU_DDR72_CONFIG_REG1_ISr, rval,
                              SEL_WRFIFO_PTR_CLKf);

        mt_ctrl->odtres_val = -1;
        mt_ctrl->pdrive_val = -1;
        mt_ctrl->ndrive_val = -1;
        mt_ctrl->slew_val = -1;
        if (mt_data->do_pvt_comp) {
            SOC_IF_ERROR_RETURN
                (soc_triumph_esm_init_pvt_comp(unit,
                                               ETU_DDR72_CONFIG_REG2_ISr,
                                               ETU_DDR72_STATUS_REG2_ISr));
            SOC_IF_ERROR_RETURN
                (READ_ETU_DDR72_CONFIG_REG2_ISr(unit, &rval));
            if (soc_reg_field_get(unit, ETU_DDR72_CONFIG_REG2_ISr, rval,
                                  OVRD_EN_ODTRES_PVTf)) {
                mt_ctrl->odtres_val =
                    soc_reg_field_get(unit, ETU_DDR72_CONFIG_REG2_ISr,
                                      rval, PVT_ODTRES_VALf);
            }
            if (soc_reg_field_get(unit, ETU_DDR72_CONFIG_REG2_ISr, rval,
                                  OVRD_EN_DRIVER_PVTf)) {
                mt_ctrl->pdrive_val =
                    soc_reg_field_get(unit, ETU_DDR72_CONFIG_REG2_ISr,
                                      rval, PVT_PDRIVE_VALf);
                mt_ctrl->ndrive_val =
                    soc_reg_field_get(unit, ETU_DDR72_CONFIG_REG2_ISr,
                                      rval, PVT_NDRIVE_VALf);
            }
            if (soc_reg_field_get(unit, ETU_DDR72_CONFIG_REG2_ISr, rval,
                                  OVRD_EN_SLEW_PVTf)) {
                mt_ctrl->slew_val =
                    soc_reg_field_get(unit, ETU_DDR72_CONFIG_REG2_ISr,
                                      rval, PVT_SLEW_VALf);
            }
        }

        if (mt_data->bg_sram_bist) {
            soc_cm_print("Do SRAM BIST on both SRAM with loop mode %d\n",
                         bg_sram_bist.loop_mode);
            soc_triumph_ext_sram_enable_set(unit, 0, TRUE, TRUE);
            soc_triumph_ext_sram_bist_setup(unit, 0, &bg_sram_bist);
            SOC_IF_ERROR_RETURN(READ_ES0_DTU_LTE_TMODE0r(unit, &rval));
            soc_reg_field_set(unit, ES0_DTU_LTE_TMODE0r, &rval,
                              START_LAB_TESTf, 1);
            soc_reg_field_set(unit, ES0_DTU_LTE_TMODE0r, &rval, FOREVERf, 1);
            SOC_IF_ERROR_RETURN(WRITE_ES0_DTU_LTE_TMODE0r(unit, rval));

            soc_triumph_ext_sram_enable_set(unit, 1, TRUE, TRUE);
            soc_triumph_ext_sram_bist_setup(unit, 1, &bg_sram_bist);
            SOC_IF_ERROR_RETURN(READ_ES1_DTU_LTE_TMODE0r(unit, &rval));
            soc_reg_field_set(unit, ES1_DTU_LTE_TMODE0r, &rval,
                              START_LAB_TESTf, 1);
            soc_reg_field_set(unit, ES1_DTU_LTE_TMODE0r, &rval, FOREVERf, 1);
            SOC_IF_ERROR_RETURN(WRITE_ES1_DTU_LTE_TMODE0r(unit, rval));
        }

        SOC_IF_ERROR_RETURN(_soc_memtune_tcam_main(mt_data));

        if (mt_data->bg_sram_bist) {
            SOC_IF_ERROR_RETURN(READ_ES0_DTU_LTE_TMODE0r(unit, &rval));
            soc_reg_field_set(unit, ES0_DTU_LTE_TMODE0r, &rval, FOREVERf, 0);
            SOC_IF_ERROR_RETURN(WRITE_ES0_DTU_LTE_TMODE0r(unit, rval));
            soc_triumph_ext_sram_enable_set(unit, 0, FALSE, TRUE);

            SOC_IF_ERROR_RETURN(READ_ES1_DTU_LTE_TMODE0r(unit, &rval));
            soc_reg_field_set(unit, ES1_DTU_LTE_TMODE0r, &rval, FOREVERf, 0);
            SOC_IF_ERROR_RETURN(WRITE_ES1_DTU_LTE_TMODE0r(unit, rval));
            soc_triumph_ext_sram_enable_set(unit, 1, FALSE, TRUE);
        }

        break;
    default:
        return SOC_E_UNAVAIL;
    }

    return SOC_E_NONE;
}
#endif /* BCM_TRIUMPH_SUPPORT */

int
soc_mem_interface_tune(int unit, soc_memtune_data_t *mt_data)
{
    soc_memtune_ctrl_t *mt_ctrl;
    int rv = SOC_E_NONE;

    mt_ctrl = sal_alloc(sizeof(soc_memtune_ctrl_t), "memtune working data");
    if (!mt_ctrl) {
        return SOC_E_MEMORY;
    }
    sal_memset(mt_ctrl, 0, sizeof(soc_memtune_ctrl_t));
    mt_ctrl->unit = unit;
    mt_data->mt_ctrl = mt_ctrl;

    if (mt_data->verbose) {
        mt_ctrl->flags |= SOC_MEMTUNE_CTRL_FLAGS_SHOW_TEST_NAME |
            SOC_MEMTUNE_CTRL_FLAGS_SHOW_SUMMARY |
            SOC_MEMTUNE_CTRL_FLAGS_SHOW_SUMMARY_HEADER |
            SOC_MEMTUNE_CTRL_FLAGS_SHOW_FAIL_MATRIX |
            SOC_MEMTUNE_CTRL_FLAGS_SHOW_ANALYSIS |
            SOC_MEMTUNE_CTRL_FLAGS_SHOW_SELECTION |
            SOC_MEMTUNE_CTRL_FLAGS_SHOW_PASS;
        if (!mt_data->suppress_fail) {
            mt_ctrl->flags |= SOC_MEMTUNE_CTRL_FLAGS_SHOW_FAIL;
        }
    }
    if (mt_data->summary) {
        mt_ctrl->flags |= SOC_MEMTUNE_CTRL_FLAGS_SHOW_SUMMARY;
    }
    if (mt_data->summary_header) {
        mt_ctrl->flags |= SOC_MEMTUNE_CTRL_FLAGS_SHOW_SUMMARY_HEADER;
    }
    if (mt_data->show_matrix) {
        mt_ctrl->flags |= SOC_MEMTUNE_CTRL_FLAGS_SHOW_FAIL_MATRIX |
            SOC_MEMTUNE_CTRL_FLAGS_SHOW_ANALYSIS |
            SOC_MEMTUNE_CTRL_FLAGS_SHOW_SELECTION;
    }
    if (mt_data->show_progress) {
        mt_ctrl->flags |= SOC_MEMTUNE_CTRL_FLAGS_SHOW_PROGRESS;
    }

#ifndef NO_MEMTUNE
    /* Prime cell data for testing */
    sal_srand(mt_data->rseed);
#endif /* NO_MEMTUNE */

#ifdef BCM_EASYRIDER_SUPPORT
    if (SOC_IS_EASYRIDER(unit)) {
        rv =_soc_er_mem_interface_tune(unit, mt_data);
    }
#endif /* BCM_EASYRIDER_SUPPORT */
#ifdef BCM_TRIUMPH_SUPPORT
    if (SOC_IS_TRIUMPH(unit) || SOC_IS_TRIUMPH2(unit)) {
        rv = _soc_tr_mem_interface_tune(unit, mt_data);
    }
#endif /* BCM_TRIUMPH_SUPPORT */

    if (mt_ctrl->data) {
        sal_free(mt_ctrl->data);
    }
    if (mt_ctrl->sram_bist) {
        sal_free(mt_ctrl->sram_bist);
    }
    sal_free(mt_ctrl);

    return rv;
}

#ifdef BCM_TRIUMPH_SUPPORT
STATIC int
_soc_tr_memtune_lvl2_tcam_bist_setup(soc_memtune_data_t *mt_data)
{
    soc_memtune_ctrl_t *mt_ctrl = mt_data->mt_ctrl;
    uint32 oe_map, data[32], rval;
    int i;

#define SETUP_72BIT_DATA(name,index,d0_msb,d1,d2_lsb) \
    name[index * 4] = d2_lsb; \
    name[index * 4 + 1] = d1; \
    name[index * 4 + 2] = d0_msb;
    switch (mt_data->tcam_data_choice) {
    case 0:
        oe_map = 0xff;
        SETUP_72BIT_DATA(data, 0, 0xa5, 0xa5a5a5a5, 0xa5a5a5a5);
        SETUP_72BIT_DATA(data, 1, 0x5a, 0x5a5a5a5a, 0x5a5a5a5a);
        SETUP_72BIT_DATA(data, 2, 0x00, 0x00000000, 0x00000000);
        SETUP_72BIT_DATA(data, 3, 0x22, 0x22222222, 0x22222222);
        SETUP_72BIT_DATA(data, 4, 0xff, 0xffffffff, 0xffffffff);
        SETUP_72BIT_DATA(data, 5, 0x44, 0x44444444, 0x44444444);
        SETUP_72BIT_DATA(data, 6, 0x01, 0x23456789, 0x01234567);
        SETUP_72BIT_DATA(data, 7, 0x76, 0x54321098, 0x76543210);
        break;
    case 1:
        oe_map = 0xff;
        SETUP_72BIT_DATA(data, 0, 0x00, 0x00000000, 0x00000000);
        SETUP_72BIT_DATA(data, 1, 0xff, 0xffffffff, 0xffffffff);
        SETUP_72BIT_DATA(data, 2, 0x00, 0x00000000, 0x00000000);
        SETUP_72BIT_DATA(data, 3, 0xff, 0xffffffff, 0xffffffff);
        SETUP_72BIT_DATA(data, 4, 0x00, 0x00000000, 0x00000000);
        SETUP_72BIT_DATA(data, 5, 0xff, 0xffffffff, 0xffffffff);
        SETUP_72BIT_DATA(data, 6, 0x00, 0x00000000, 0x00000000);
        SETUP_72BIT_DATA(data, 7, 0xff, 0xffffffff, 0xffffffff);
        break;
    case 2:
        oe_map = 0xff;
        SETUP_72BIT_DATA(data, 0, 0xa5, 0xa5a5a5a5, 0xa5a5a5a5);
        SETUP_72BIT_DATA(data, 1, 0x5a, 0x5a5a5a5a, 0x5a5a5a5a);
        SETUP_72BIT_DATA(data, 2, 0xa5, 0xa5a5a5a5, 0xa5a5a5a5);
        SETUP_72BIT_DATA(data, 3, 0x5a, 0x5a5a5a5a, 0x5a5a5a5a);
        SETUP_72BIT_DATA(data, 4, 0xa5, 0xa5a5a5a5, 0xa5a5a5a5);
        SETUP_72BIT_DATA(data, 5, 0x5a, 0x5a5a5a5a, 0x5a5a5a5a);
        SETUP_72BIT_DATA(data, 6, 0xa5, 0xa5a5a5a5, 0xa5a5a5a5);
        SETUP_72BIT_DATA(data, 7, 0x5a, 0x5a5a5a5a, 0x5a5a5a5a);
        break;
    case 3:
        oe_map = 0xff;
        SETUP_72BIT_DATA(data, 0, 0x00, 0x00000000, 0x00000000);
        SETUP_72BIT_DATA(data, 1, 0x00, 0x00000000, 0x00000000);
        SETUP_72BIT_DATA(data, 2, 0x00, 0x00000000, 0x00000000);
        SETUP_72BIT_DATA(data, 3, 0x00, 0x00000000, 0x00000000);
        SETUP_72BIT_DATA(data, 4, 0x00, 0x00000000, 0x00000000);
        SETUP_72BIT_DATA(data, 5, 0x00, 0x00000000, 0x00000000);
        SETUP_72BIT_DATA(data, 6, 0x00, 0x00000000, 0x00000000);
        SETUP_72BIT_DATA(data, 7, 0x00, 0x00000000, 0x00000000);
        break;
    case 4:
        oe_map = 0xff;
        SETUP_72BIT_DATA(data, 0, 0xaa, 0xaaaaaaaa, 0xaaaaaaaa);
        SETUP_72BIT_DATA(data, 1, 0x55, 0x55555555, 0x55555555);
        SETUP_72BIT_DATA(data, 2, 0xaa, 0xaaaaaaaa, 0xaaaaaaaa);
        SETUP_72BIT_DATA(data, 3, 0x55, 0x55555555, 0x55555555);
        SETUP_72BIT_DATA(data, 4, 0xaa, 0xaaaaaaaa, 0xaaaaaaaa);
        SETUP_72BIT_DATA(data, 5, 0x55, 0x55555555, 0x55555555);
        SETUP_72BIT_DATA(data, 6, 0xaa, 0xaaaaaaaa, 0xaaaaaaaa);
        SETUP_72BIT_DATA(data, 7, 0x55, 0x55555555, 0x55555555);
        break;
    case 5:
        oe_map = 0xff;
        SETUP_72BIT_DATA(data, 0, 0xff, 0xffffffff, 0xffffffff);
        SETUP_72BIT_DATA(data, 1, 0xff, 0xffffffff, 0xffffffff);
        SETUP_72BIT_DATA(data, 2, 0xff, 0xffffffff, 0xffffffff);
        SETUP_72BIT_DATA(data, 3, 0xff, 0xffffffff, 0xffffffff);
        SETUP_72BIT_DATA(data, 4, 0xff, 0xffffffff, 0xffffffff);
        SETUP_72BIT_DATA(data, 5, 0xff, 0xffffffff, 0xffffffff);
        SETUP_72BIT_DATA(data, 6, 0xff, 0xffffffff, 0xffffffff);
        SETUP_72BIT_DATA(data, 7, 0xff, 0xffffffff, 0xffffffff);
        break;
    case 6:
        oe_map = 0x55;
        SETUP_72BIT_DATA(data, 0, 0xff, 0xffffffff, 0xffffffff);
        SETUP_72BIT_DATA(data, 2, 0xff, 0xffffffff, 0xffffffff);
        SETUP_72BIT_DATA(data, 4, 0xff, 0xffffffff, 0xffffffff);
        SETUP_72BIT_DATA(data, 6, 0xff, 0xffffffff, 0xffffffff);
        break;
    case 7:
        oe_map = 0x55;
        SETUP_72BIT_DATA(data, 0, 0x00, 0x00000000, 0x00000000);
        SETUP_72BIT_DATA(data, 2, 0x00, 0x00000000, 0x00000000);
        SETUP_72BIT_DATA(data, 4, 0x00, 0x00000000, 0x00000000);
        SETUP_72BIT_DATA(data, 6, 0x00, 0x00000000, 0x00000000);
        break;
    case 8:
        oe_map = 0xff;
        SETUP_72BIT_DATA(data, 0, 0x00, 0x0000000f, 0xffffffff);
        SETUP_72BIT_DATA(data, 1, 0x00, 0x00000000, 0x00000000);
        SETUP_72BIT_DATA(data, 2, 0x00, 0x0000000f, 0xffffffff);
        SETUP_72BIT_DATA(data, 3, 0x00, 0x00000000, 0x00000000);
        SETUP_72BIT_DATA(data, 4, 0x00, 0x0000000f, 0xffffffff);
        SETUP_72BIT_DATA(data, 5, 0x00, 0x00000000, 0x00000000);
        SETUP_72BIT_DATA(data, 6, 0x00, 0x0000000f, 0xffffffff);
        SETUP_72BIT_DATA(data, 7, 0x00, 0x00000000, 0x00000000);
        break;
    case 81:
        oe_map = 0xff;
        SETUP_72BIT_DATA(data, 0, 0xff, 0xfffffff0, 0x00000000);
        SETUP_72BIT_DATA(data, 1, 0x00, 0x00000000, 0x00000000);
        SETUP_72BIT_DATA(data, 2, 0xff, 0xfffffff0, 0x00000000);
        SETUP_72BIT_DATA(data, 3, 0x00, 0x00000000, 0x00000000);
        SETUP_72BIT_DATA(data, 4, 0xff, 0xfffffff0, 0x00000000);
        SETUP_72BIT_DATA(data, 5, 0x00, 0x00000000, 0x00000000);
        SETUP_72BIT_DATA(data, 6, 0xff, 0xfffffff0, 0x00000000);
        SETUP_72BIT_DATA(data, 7, 0x00, 0x00000000, 0x00000000);
        break;
    case 200:
        oe_map = 0xff;
        SETUP_72BIT_DATA(data, 0, 0x00, 0x00000000, 0x00000000);
        SETUP_72BIT_DATA(data, 1, 0x6d, 0xb6df6db6, 0xdb6fd6db);
        SETUP_72BIT_DATA(data, 2, 0x00, 0x00000000, 0x00000000);
        SETUP_72BIT_DATA(data, 3, 0x6d, 0xb6df6db6, 0xdb6fd6db);
        SETUP_72BIT_DATA(data, 4, 0x00, 0x00000000, 0x00000000);
        SETUP_72BIT_DATA(data, 5, 0x6d, 0xb6df6db6, 0xdb6fd6db);
        SETUP_72BIT_DATA(data, 6, 0x00, 0x00000000, 0x00000000);
        SETUP_72BIT_DATA(data, 7, 0x6d, 0xb6df6db6, 0xdb6fd6db);
        break;
    case 2000:
        oe_map = 0;
        break;
    default:
        soc_cm_print("Unknown TcamDataChoice %d\n", mt_data->tcam_data_choice);
        return SOC_E_PARAM;
    }
#undef SETUP_72BIT_DATA

    if (mt_ctrl->flags & SOC_MEMTUNE_CTRL_FLAGS_SHOW_SETTING) {
        soc_cm_print("TCAM setting:\n");
        for (i = 0; i < 8; i++) {
            if (oe_map & (1 << i)) {
                soc_cm_print("  K%d: %02x-%08x-%08x OE: ENABLE\n",
                             i, data[i * 4 + 2], data[i * 4 + 1], data[i * 4]);
            } else {
                soc_cm_print("  K%d: zz-zzzzzzzz-zzzzzzzz OE: DISABLE\n", i);
            }
        }
    }
    SOC_IF_ERROR_RETURN
        (soc_tr_tcam_type1_memtest_dpeo(mt_ctrl->unit, 8, oe_map, data));

    rval = 0;
    soc_reg_field_set(mt_ctrl->unit, ETU_LTE_BIST_CTLr, &rval,
                      RD_EN_BIST_RSLTSf, 1);
    soc_reg_field_set(mt_ctrl->unit, ETU_LTE_BIST_CTLr, &rval, LOOP_COUNTf,
                      mt_data->tcam_loop_count);
    SOC_IF_ERROR_RETURN(WRITE_ETU_LTE_BIST_CTLr(mt_ctrl->unit, rval));

    return SOC_E_NONE;
}

STATIC int
_soc_tr_memtune_lvl2_sram_bist_setup(soc_memtune_data_t *mt_data)
{
    soc_memtune_ctrl_t *mt_ctrl = mt_data->mt_ctrl;
    tr_ext_sram_bist_t sram_bist;
    soc_reg_t reg;
    int addr;
    uint32 rval;

    sal_memset(&sram_bist, 0, sizeof(sram_bist));

#define SPLIT_36BIT_DATA(name,d0_msb,d1_lsb) \
    name## _0 = (d1_lsb) & 0x3ffff; \
    name## _1 = ((d0_msb & 0xf) << 14) | ((d1_lsb & 0xfffc0000) >> 18);
    switch (mt_data->sram_data_choice) {
    case 0:
        SPLIT_36BIT_DATA(sram_bist.d0r, 0x5, 0x55555555);
        SPLIT_36BIT_DATA(sram_bist.d0f, 0xa, 0xaaaaaaaa);
        SPLIT_36BIT_DATA(sram_bist.d1r, 0x5, 0x55555555);
        SPLIT_36BIT_DATA(sram_bist.d1f, 0xa, 0xaaaaaaaa);
        break;
    case 1:
        SPLIT_36BIT_DATA(sram_bist.d0r, 0xf, 0xffffffff);
        SPLIT_36BIT_DATA(sram_bist.d0f, 0x0, 0x00000000);
        SPLIT_36BIT_DATA(sram_bist.d1r, 0xf, 0xffffffff);
        SPLIT_36BIT_DATA(sram_bist.d1f, 0x0, 0x00000000);
        break;
    case 2:
        SPLIT_36BIT_DATA(sram_bist.d0r, 0xf, 0xffffffff);
        SPLIT_36BIT_DATA(sram_bist.d0f, 0xf, 0xffffffff);
        SPLIT_36BIT_DATA(sram_bist.d1r, 0x0, 0x00000000);
        SPLIT_36BIT_DATA(sram_bist.d1f, 0x0, 0x00000000);
        break;
    case 3:
        SPLIT_36BIT_DATA(sram_bist.d0r, 0x0, 0x00000000);
        SPLIT_36BIT_DATA(sram_bist.d0f, 0x0, 0x00000000);
        SPLIT_36BIT_DATA(sram_bist.d1r, 0xf, 0xffffffff);
        SPLIT_36BIT_DATA(sram_bist.d1f, 0xf, 0xffffffff);
        break;
    case 4:
        SPLIT_36BIT_DATA(sram_bist.d0r, 0x0, 0x00000000);
        SPLIT_36BIT_DATA(sram_bist.d0f, 0x0, 0x00000000);
        SPLIT_36BIT_DATA(sram_bist.d1r, 0x0, 0x00000000);
        SPLIT_36BIT_DATA(sram_bist.d1f, 0x0, 0x00000000);
        break;
    case 5:
        SPLIT_36BIT_DATA(sram_bist.d0r, 0xf, 0xffffffff);
        SPLIT_36BIT_DATA(sram_bist.d0f, 0xf, 0xffffffff);
        SPLIT_36BIT_DATA(sram_bist.d1r, 0xf, 0xffffffff);
        SPLIT_36BIT_DATA(sram_bist.d1f, 0xf, 0xffffffff);
        break;
    case 6:
        SPLIT_36BIT_DATA(sram_bist.d0r, 0xf, 0xfffc1020);
        SPLIT_36BIT_DATA(sram_bist.d0f, 0x0, 0x00000000);
        SPLIT_36BIT_DATA(sram_bist.d1r, 0xf, 0xfffc1020);
        SPLIT_36BIT_DATA(sram_bist.d1f, 0x0, 0x00000000);
        break;
    default:
        soc_cm_print("Unknown SramDataChoice %d\n", mt_data->sram_data_choice);
        return SOC_E_PARAM;
    }
#undef SPLIT_36BIT_DATA

    switch (mt_data->alt_adr) {
    case 0: /* ADR0:0 ADR1:78, INC2 */
        sram_bist.adr1 = 78;
        sram_bist.adr_mode = 2;
    case 1: /* ADR0:0 ADR1:0x3ffffe, ALT_A0A1 */
        sram_bist.adr1 = 0x3ffffe;
        break;
    case 2: /* ADR0:0 ADR1:2, ALT_A0A1 */
        sram_bist.adr1 = 2;
        break;
    case 3: /* ADR0:0x3ffffe ADR1:0, ALT_A0A1 */
        sram_bist.adr0 = 0x3ffffe;
        break;
    default:
        soc_cm_print("Unknown AltAdr %d\n", mt_data->alt_adr);
        return SOC_E_PARAM;
    }

    sram_bist.em_latency = -1;
    sram_bist.bg_tcam_loop_count = mt_data->tcam_loop_count;

    if (mt_ctrl->flags & SOC_MEMTUNE_CTRL_FLAGS_SHOW_SETTING) {
        soc_cm_print("SRAM setting:\n");
        soc_cm_print("  D0R:%05x-%05x D0F:%05x-%05x D1R:%05x-%05x "
                     "D1F:%05x-%05x\n",
                     sram_bist.d0r_1, sram_bist.d0r_0,
                     sram_bist.d0f_1, sram_bist.d0f_0,
                     sram_bist.d1r_1, sram_bist.d1r_0,
                     sram_bist.d1f_1, sram_bist.d1f_0);
        soc_cm_print("  ADR0:%05x ADR1:%05x ADR_MODE:%d\n",
                     sram_bist.adr0, sram_bist.adr1, sram_bist.adr_mode);
    }

    if (mt_data->loop_mode[0] != 4) {
        reg = ES0_SRAM_CTLr;
        addr = soc_reg_addr(mt_ctrl->unit, reg, REG_PORT_ANY, 0);
        SOC_IF_ERROR_RETURN(soc_reg32_read(mt_ctrl->unit, addr, &rval));
        sram_bist.w2r_nops =
            soc_reg_field_get(mt_ctrl->unit, reg, rval, NUM_W2R_NOPSf);
        sram_bist.r2w_nops =
            soc_reg_field_get(mt_ctrl->unit, reg, rval, NUM_R2W_NOPSf);

        if (mt_ctrl->flags & SOC_MEMTUNE_CTRL_FLAGS_SHOW_SETTING) {
            soc_cm_print("  (SRAM0) W2R_NOPS:%d R2W_NOPS:%d LOOP_MODE: %d\n",
                         sram_bist.w2r_nops, sram_bist.r2w_nops,
                         mt_data->loop_mode[0]);
        }
        SOC_IF_ERROR_RETURN
            (soc_triumph_ext_sram_bist_setup(mt_ctrl->unit, 0, &sram_bist));
    }

    if (mt_data->loop_mode[1] != 4) {
        reg = ES1_SRAM_CTLr;
        addr = soc_reg_addr(mt_ctrl->unit, reg, REG_PORT_ANY, 0);
        SOC_IF_ERROR_RETURN(soc_reg32_read(mt_ctrl->unit, addr, &rval));
        sram_bist.w2r_nops =
            soc_reg_field_get(mt_ctrl->unit, reg, rval, NUM_W2R_NOPSf);
        sram_bist.r2w_nops =
            soc_reg_field_get(mt_ctrl->unit, reg, rval, NUM_R2W_NOPSf);

        if (mt_ctrl->flags & SOC_MEMTUNE_CTRL_FLAGS_SHOW_SETTING) {
            soc_cm_print("  (SRAM1) W2R_NOPS:%d R2W_NOPS:%d LOOP_MODE: %d\n",
                         sram_bist.w2r_nops, sram_bist.r2w_nops,
                         mt_data->loop_mode[1]);
        }
        SOC_IF_ERROR_RETURN
            (soc_triumph_ext_sram_bist_setup(mt_ctrl->unit, 1, &sram_bist));
    }

    return SOC_E_NONE;
}

STATIC int
_soc_tr_memtune_lvl2_test(soc_memtune_data_t *mt_data, int *fail_count)
{
    soc_memtune_ctrl_t *mt_ctrl = mt_data->mt_ctrl;
    tr_ext_sram_bist_t sram_bist[2], *sram_bist_ptr, *opt_sram_bist_ptr;
    int i, count, start, end, done;

    if (mt_data->loop_mode[0] == 4) {
        start = end = 1;
        sram_bist_ptr = &sram_bist[1];
        opt_sram_bist_ptr = NULL;
    } else if (mt_data->loop_mode[1] == 4) {
        start = end = 0;
        sram_bist_ptr = &sram_bist[0];
        opt_sram_bist_ptr = NULL;
    } else {
        start = 0;
        end = 1;
        sram_bist_ptr = &sram_bist[0];
        opt_sram_bist_ptr = &sram_bist[1];
    }

    for (i = start; i <= end; i++) {
        sal_memset(&sram_bist[i], 0, sizeof(tr_ext_sram_bist_t));

        /*
         * Following sram bist registers has been set properly,
         * don't touch them during test to save time:
         * D0R_0, D0R_1, D0F_0, D0F_1, D1R_0, D1R_1, D1F_0, D1F_1,
         * ADR0, ADR1, TMODE1
         */
        sram_bist[i].d0r_0 = 0xffffffff;
        sram_bist[i].d1r_0 = 0xffffffff;
        sram_bist[i].adr0 = -1;
        sram_bist[i].adr1 = -1;
        sram_bist[i].wdoebr = -1;
        sram_bist[i].em_latency = -1;
        sram_bist[i].r2w_nops = -1;
        sram_bist[i].w2r_nops = -1;

        sram_bist[i].bg_tcam_loop_count = mt_data->tcam_loop_count;
    }

    for (count = 0; count < mt_data->test_count; count++) {
        for (i = start; i <= end; i++) {
            if (mt_data->loop_mode[i] == 1) /* RR */ {
                sram_bist[i].loop_mode = 0; /* WW */
                SOC_IF_ERROR_RETURN
                    (soc_triumph_ext_sram_enable_set(mt_ctrl->unit, i, TRUE,
                                                     TRUE));
                SOC_IF_ERROR_RETURN
                    (soc_triumph_ext_sram_bist_setup(mt_ctrl->unit, i,
                                                     &sram_bist[i]));
                SOC_IF_ERROR_RETURN
                    (soc_triumph_ext_sram_op(mt_ctrl->unit, i, &sram_bist[i],
                                             NULL));
            }
            sram_bist[i].loop_mode = mt_data->loop_mode[i];
            SOC_IF_ERROR_RETURN
                (soc_triumph_ext_sram_enable_set(mt_ctrl->unit, i, TRUE,
                                                 TRUE));
            SOC_IF_ERROR_RETURN
                (soc_triumph_ext_sram_bist_setup(mt_ctrl->unit, i,
                                                 &sram_bist[i]));
        }

        SOC_IF_ERROR_RETURN
            (soc_triumph_ext_sram_op(mt_ctrl->unit, start, sram_bist_ptr,
                                     opt_sram_bist_ptr));

        done = FALSE;
        for (i = start; i <= end; i++) {
            SOC_IF_ERROR_RETURN
                (soc_triumph_ext_sram_enable_set(mt_ctrl->unit, i, FALSE,
                                                 FALSE));
            if (sram_bist[i].err_cnt) {
                fail_count[i]++;
                if (fail_count[i] >= mt_data->max_fail_count) {
                    done = TRUE;
                }
            }
        }
        if (done) {
            break;
        }
    }

    return SOC_E_NONE;
}

STATIC int
_soc_tr_mem_interface_tune_lvl2(soc_memtune_data_t *mt_data)
{
    soc_tcam_info_t *tcam_info;
    soc_memtune_ctrl_t *mt_ctrl = mt_data->mt_ctrl;
    soc_tr_memtune_data_t *tr_mt_data;
    soc_memtune_result_t *result;
    int offset_count[2], index[2];
    int tx_offset[2], tx_offset_cur[2], tx_offset_min[2], tx_offset_max[2];
    int rx_offset[2], rx_offset_cur[2], rx_offset_min[2], rx_offset_max[2];
    int fail_count[2], total_fail_count[2];
    static int *fail_array[2] = { NULL, NULL };
    static int delta = 0;
    static const soc_reg_t reg[2] = { ES0_DDR36_CONFIG_REG1_ISr,
                                      ES1_DDR36_CONFIG_REG1_ISr };
    int addr, dt, dr, i, start, end, skip;
    uint32 rval;

    if (mt_data->loop_mode[0] < 0 || mt_data->loop_mode[0] > 4) {
        soc_cm_print("Unknown LoopMode %d\n", mt_data->loop_mode[0]);
        return SOC_E_PARAM;
    }

    if (mt_data->loop_mode[1] < 0 || mt_data->loop_mode[1] > 4) {
        soc_cm_print("Unknown LoopMode %d\n", mt_data->loop_mode[1]);
        return SOC_E_PARAM;
    }

    start = 0;
    end = 1;
    if (mt_data->loop_mode[0] == 4) {
        if (mt_data->loop_mode[1] == 4) {
            return SOC_E_NONE; /* do nothing */
        } else {
            start++;
        }
    } else if (mt_data->loop_mode[1] == 4) {
        end--;
    }

    tcam_info = SOC_CONTROL(mt_ctrl->unit)->tcam_info;
    if (tcam_info == NULL) {
        return SOC_E_INIT;
    }

    tr_mt_data = sal_alloc(sizeof(soc_tr_memtune_data_t),
                           "memtune working data");
    if (!tr_mt_data) {
        return SOC_E_MEMORY;
    }
    sal_memset(tr_mt_data, 0, sizeof(soc_tr_memtune_data_t));
    mt_ctrl->data = tr_mt_data;

    mt_data->interface = SOC_MEM_INTERFACE_SRAM;
    mt_data->sub_interface = 0; /* any legal value to allow setup to pass */
    _soc_tr_mem_interface_tune_setup(mt_data);
    mt_ctrl->phase_sel_cur = -1;
    mt_ctrl->em_latency_cur = -1;
    mt_ctrl->ddr_latency_cur = -1;

    if (mt_data->delta != -1) {
        delta = mt_data->delta;
    }

    for (i = start; i <= end; i++) {
        if (mt_data->tx_offset[i] != -1 || mt_data->rx_offset[i] != -1) {
            if (mt_data->tx_offset[i] < mt_ctrl->tx_offset_min ||
                mt_data->tx_offset[i] > mt_ctrl->tx_offset_max ||
                mt_data->rx_offset[i] < mt_ctrl->rx_offset_min ||
                mt_data->rx_offset[i] > mt_ctrl->rx_offset_max) {
                soc_cm_print("Invalid TX/RX value %d/%d\n",
                             mt_data->tx_offset[i], mt_data->rx_offset[i]);
                return SOC_E_PARAM;
            }
            tx_offset[i] = mt_data->tx_offset[i];
            rx_offset[i] = mt_data->rx_offset[i];
        } else {
            /* Get current TX/RX offset setting as center of sweep test */
            addr = soc_reg_addr(mt_ctrl->unit, reg[i], REG_PORT_ANY, 0);
            SOC_IF_ERROR_RETURN(soc_reg32_read(mt_ctrl->unit, addr, &rval));
            tx_offset[i] =
                soc_reg_field_get(mt_ctrl->unit, reg[i], rval,
                                  DLL90_OFFSET_TXf) |
                (soc_reg_field_get(mt_ctrl->unit, reg[i], rval,
                                   DLL90_OFFSET_TX4f) << 4);
            rx_offset[i] =
                soc_reg_field_get(mt_ctrl->unit, reg[i], rval,
                                  DLL90_OFFSET_QKf) |
                (soc_reg_field_get(mt_ctrl->unit, reg[i], rval,
                                   DLL90_OFFSET_QK4f) << 4);
        }

        tx_offset_min[i] = mt_ctrl->tx_offset_min;
        if (tx_offset[i] - delta >= tx_offset_min[i]) {
            tx_offset_min[i] = tx_offset[i] - delta;
        }
        tx_offset_max[i] = mt_ctrl->tx_offset_max;
        if (tx_offset[i] + delta <= tx_offset_max[i]) {
            tx_offset_max[i] = tx_offset[i] + delta;
        }
        rx_offset_min[i] = mt_ctrl->rx_offset_min;
        if (rx_offset[i] - delta >= rx_offset_min[i]) {
            rx_offset_min[i] = rx_offset[i] - delta;
        }
        rx_offset_max[i] = mt_ctrl->rx_offset_max;
        if (rx_offset[i] + delta <= rx_offset_max[i]) {
            rx_offset_max[i] = rx_offset[i] + delta;
        }
        offset_count[i] = (tx_offset_max[i] - tx_offset_min[i] + 1) *
            (rx_offset_max[i] - rx_offset_min[i] + 1);

        if (mt_data->delta != -1) {
            if (fail_array[i] != NULL) {
                sal_free(fail_array[i]);
                fail_array[i] = NULL;
            }
        }
        if (fail_array[i] == NULL) {
            fail_array[i] = sal_alloc(offset_count[i] * sizeof(int),
                                      "memtune working data");
            if (fail_array[i] == NULL) {
                return SOC_E_MEMORY;
            }
            sal_memset(fail_array[i], 0, offset_count[i] * sizeof(int));
        }
        total_fail_count[i] = 0;
    }

    /* Setup TCAM BIST */
    SOC_IF_ERROR_RETURN(_soc_tr_memtune_lvl2_tcam_bist_setup(mt_data));

    /* Setup SRAM BIST */
    SOC_IF_ERROR_RETURN(_soc_tr_memtune_lvl2_sram_bist_setup(mt_data));

    for (dt = -delta; dt <= delta; dt++) {
        for (dr = -delta; dr <= delta; dr++) {
            skip = TRUE;
            for (i = start; i <= end; i++) {
                if (tx_offset[i] + dt < tx_offset_min[i]) {
                    tx_offset_cur[i] = tx_offset_min[i];
                } else if (tx_offset[i] + dt > tx_offset_max[i]) {
                    tx_offset_cur[i] = tx_offset_max[i];
                } else {
                    tx_offset_cur[i] = tx_offset[i] + dt;
                }
                if (rx_offset[i] + dr < rx_offset_min[i]) {
                    rx_offset_cur[i] = rx_offset_min[i];
                } else if (rx_offset[i] + dr > rx_offset_max[i]) {
                    rx_offset_cur[i] = rx_offset_max[i];
                } else {
                    rx_offset_cur[i] = rx_offset[i] + dr;
                }
                index[i] = (tx_offset_cur[i] - tx_offset_min[i]) *
                    (rx_offset_max[i] - rx_offset_min[i] + 1) +
                    rx_offset_cur[i] - rx_offset_min[i];
                if (tx_offset[i] + dt == tx_offset_cur[i] &&
                    rx_offset[i] + dr == rx_offset_cur[i]) {
                    if (fail_array[i][index[i]] < mt_data->max_fail_count) {
                        skip = FALSE;
                    }
                }
                fail_count[i] = mt_data->max_fail_count;
            }

            if (!skip) {
                for (i = start; i <= end; i++) {
                    mt_ctrl->tx_offset_cur = tx_offset_cur[i];
                    mt_ctrl->rx_offset_cur = rx_offset_cur[i];
                    tr_mt_data->config_reg1 = reg[i];
                    (*mt_ctrl->prog_hw1_fn)(mt_data);
                    fail_count[i] = 0;
                }

                /* Do test */
                _soc_tr_memtune_lvl2_test(mt_data, fail_count);

                if (mt_ctrl->flags & SOC_MEMTUNE_CTRL_FLAGS_SHOW_FAIL ||
                    mt_ctrl->flags & SOC_MEMTUNE_CTRL_FLAGS_SHOW_PASS) {
                    for (i = start; i <= end; i++) {
                        soc_cm_print("SRAM%d TX=%d RX=%d: Fail count=%d ",
                                     i, tx_offset[i] + dt, rx_offset[i] + dr,
                                     fail_count[i]);
                    }
                    soc_cm_print("\n");
                }
            }

            for (i = start; i <= end; i++) {
                if (index[i] != -1) {
                    fail_array[i][index[i]] += fail_count[i];
                    if (fail_array[i][index[i]] > mt_data->max_fail_count) {
                        fail_array[i][index[i]] = mt_data->max_fail_count;
                    }
                    total_fail_count[i] += fail_count[i];
                }
            }
        }
    }

    /* Clear TCAM BIST setting */
    rval = 0;
    soc_reg_field_set(mt_ctrl->unit, ETU_LTE_BIST_CTLr, &rval, LOOP_COUNTf, 1);
    (void)WRITE_ETU_LTE_BIST_CTLr(mt_ctrl->unit, rval);

    for (i = start; i <= end; i++) {
        mt_ctrl->intf_name = i == 0 ? "SRAM0" : "SRAM1";
        if (!total_fail_count[i]) {
            if (mt_ctrl->flags & SOC_MEMTUNE_CTRL_FLAGS_SHOW_SUMMARY) {
                soc_cm_print("%s: TX=%d RX=%d Delta=%d all passes\n",
                             mt_ctrl->intf_name, tx_offset[i], rx_offset[i],
                             delta);
            }
        } else {
            /* Analyze and print result matrix */
            mt_ctrl->tx_offset_min = tx_offset_min[i];
            mt_ctrl->tx_offset_max = tx_offset_max[i];
            mt_ctrl->rx_offset_min = rx_offset_min[i];
            mt_ctrl->rx_offset_max = rx_offset_max[i];
            mt_ctrl->fail_array = fail_array[i];
            mt_ctrl->cur_result_count = 0;
            result = &mt_ctrl->result[mt_ctrl->cur_result_count];
            _soc_memtune_txrx_analyze(mt_data);

            if (result->fail_count == offset_count[i]) {
                soc_cm_print("!!! %s Tuning FAIL !!!\n", mt_ctrl->intf_name);
            } else {
                if (mt_ctrl->flags & SOC_MEMTUNE_CTRL_FLAGS_SHOW_SUMMARY) {
                    soc_cm_print("%s OldTX=%d OldRX=%d NewTX=%d NewRX=%d "
                                 "Area=%d Width=%d Height=%d\n",
                                 mt_ctrl->intf_name,
                                 tx_offset[i], rx_offset[i],
                                 mt_ctrl->tx_offset_cur,
                                 mt_ctrl->rx_offset_cur,
                                 result->width * result->height,
                                 result->width, result->height);
                }
#ifndef NO_MEMTUNE
                if (mt_data->config) {
                    char name_str[24], val_str[11];
                    sal_sprintf(name_str, "%s%d", spn_EXT_SRAM_TUNING, i);
                    rval = soc_property_get(mt_ctrl->unit, name_str, 0);
                    rval &= ~(SOC_TR_MEMTUNE_DDR_TX_OFFSET_MASK <<
                              SOC_TR_MEMTUNE_DDR_TX_OFFSET_SHIFT);
                    rval |= ((mt_ctrl->tx_offset_cur &
                              SOC_TR_MEMTUNE_DDR_TX_OFFSET_MASK) <<
                             SOC_TR_MEMTUNE_DDR_TX_OFFSET_SHIFT);
                    rval &= ~(SOC_TR_MEMTUNE_DDR_RX_OFFSET_MASK <<
                              SOC_TR_MEMTUNE_DDR_RX_OFFSET_SHIFT);
                    rval |= ((mt_ctrl->rx_offset_cur &
                              SOC_TR_MEMTUNE_DDR_RX_OFFSET_MASK) <<
                             SOC_TR_MEMTUNE_DDR_RX_OFFSET_SHIFT);
                    sal_sprintf(val_str, "0x%08x", rval);
                    if (sal_config_set(name_str, val_str) < 0) {
                        return SOC_E_MEMORY;
                    }
                    sal_sprintf(name_str, "%s%d", spn_EXT_SRAM_TUNING2_STATS,
                                i);
                    rval = SOC_TR_MEMTUNE_CONFIG_VALID_MASK |
                        ((result->width & SOC_TR_MEMTUNE_STATS_WIDTH_MASK) <<
                         SOC_TR_MEMTUNE_STATS_WIDTH_SHIFT) |
                        ((result->height & SOC_TR_MEMTUNE_STATS_HEIGHT_MASK) <<
                         SOC_TR_MEMTUNE_STATS_HEIGHT_SHIFT) |
                        ((result->fail_count &
                          SOC_TR_MEMTUNE_STATS_FAIL_MASK) <<
                         SOC_TR_MEMTUNE_STATS_FAIL_SHIFT);
                    sal_sprintf(val_str, "0x%08x", rval);
                    if (sal_config_set(name_str, val_str) < 0) {
                        return SOC_E_MEMORY;
                    }
                }
#endif /* NO_MEMTUNE */
            }
        }
        mt_ctrl->tx_offset_cur = tx_offset[i];
        mt_ctrl->rx_offset_cur = rx_offset[i];
        tr_mt_data->config_reg1 = reg[i];
        (*mt_ctrl->prog_hw1_fn)(mt_data);
    }

    return SOC_E_NONE;
}
#endif /* BCM_TRIUMPH_SUPPORT */

int
soc_mem_interface_tune_lvl2(int unit, soc_memtune_data_t *mt_data)
{
    soc_memtune_ctrl_t *mt_ctrl;
    int rv = SOC_E_NONE;

    mt_ctrl = sal_alloc(sizeof(soc_memtune_ctrl_t), "memtune working data");
    if (!mt_ctrl) {
        return SOC_E_MEMORY;
    }
    sal_memset(mt_ctrl, 0, sizeof(soc_memtune_ctrl_t));
    mt_ctrl->unit = unit;
    mt_data->mt_ctrl = mt_ctrl;

    if (mt_data->verbose) {
        mt_ctrl->flags |= SOC_MEMTUNE_CTRL_FLAGS_SHOW_TEST_NAME |
            SOC_MEMTUNE_CTRL_FLAGS_SHOW_SUMMARY |
            SOC_MEMTUNE_CTRL_FLAGS_SHOW_FAIL_MATRIX |
            SOC_MEMTUNE_CTRL_FLAGS_SHOW_ANALYSIS |
            SOC_MEMTUNE_CTRL_FLAGS_SHOW_FAIL |
            SOC_MEMTUNE_CTRL_FLAGS_SHOW_PASS |
            SOC_MEMTUNE_CTRL_FLAGS_SHOW_SETTING;
    }
    if (mt_data->summary) {
        mt_ctrl->flags |= SOC_MEMTUNE_CTRL_FLAGS_SHOW_SUMMARY;
    }
    if (mt_data->show_setting) {
        mt_ctrl->flags |= SOC_MEMTUNE_CTRL_FLAGS_SHOW_SETTING;
    }
    if (mt_data->show_matrix) {
        mt_ctrl->flags |= SOC_MEMTUNE_CTRL_FLAGS_SHOW_FAIL_MATRIX |
            SOC_MEMTUNE_CTRL_FLAGS_SHOW_ANALYSIS;
    }

#ifdef BCM_TRIUMPH_SUPPORT
    if (SOC_IS_TRIUMPH(unit) || SOC_IS_TRIUMPH2(unit)) {
        rv = _soc_tr_mem_interface_tune_lvl2(mt_data);
    }
#endif /* BCM_TRIUMPH_SUPPORT */

    if (mt_ctrl->data) {
        sal_free(mt_ctrl->data);
    }
    sal_free(mt_ctrl);

    return rv;
}
#endif /* BCM_EASYRIDER_SUPPORT || BCM_TRIUMPH_SUPPORT */
