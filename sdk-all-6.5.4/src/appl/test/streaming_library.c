/*
 * $Id: streaming_library.c,v 1.0 Broadcom SDK $
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
 * Streaming library for diagnostic (TR) tests.
 * This library contains functions use by streaming diagnostic tests.
 */

#ifndef _STREAMING_LIBRARY_H
#define _STREAMING_LIBRARY_H

#include <appl/diag/system.h>
#include <appl/diag/test.h>
#include <shared/alloc.h>
#include <shared/bsl.h>
#include <soc/cmicm.h>
#include <soc/mcm/cmicm.h>
#include "gen_pkt.h"
#include "streaming_library.h"

#if defined(BCM_ESW_SUPPORT)

static rate_calc_t *rate_calc_parray[SOC_MAX_NUM_DEVICES];

/*
 * Function:
 *      get_pipe
 * Purpose:
 *      Get pipe number for a given device port.
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *      port: Device port #
 *
 * Returns:
 *     Pipe number.
 */

int
get_pipe(int unit, int port)
{
    int pipe;

    for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
        if (SOC_PBMP_MEMBER(PBMP_PIPE(unit, pipe), port)) {
            break;
        }
    }
    return pipe;
}


/*
 * Function:
 *      num_cells
 * Purpose:
 *      Get the cell count for a given packet size.
 * Parameters:
 *      unit: StrataSwitch Unit #
 *      pkt_size: packet size in bytes
 *      port: Device port #
 *
 * Returns:
 *     Cell count
 */

uint32
num_cells(int unit, uint32 pkt_size, int port)
{
    uint32 num_cells = 1;
    uint32 i;

    if (IS_HG_PORT(unit, port)) {
        i = FIRST_CELL_HG2;
    }
    else {
        i = FIRST_CELL_ENET;
    }
    while (i < pkt_size) {
        num_cells++;
        i += CELL_SIZE;
    }
    return num_cells;
}


/*
 * Function:
 *      safe_pkt_size
 * Purpose:
 *      Calculate safe packet size based on oversub ratio. The assumption is that
 *      a front panel port will behave essentially as a line rate port and show no
 *      bandwidth degradation for packet sizes above the safe packet size.
 *      This is based on input from device microarchitects. Broadcom internal
 *      users of this test can contact microarchitecture for more info.
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *      ovs_ratio_x1000: Oversub ratio x1000
 *
 * Returns:
 *     Safe packet size.
 */

uint32
safe_pkt_size(int unit, int ovs_ratio_x1000)
{
    uint32 safe_size = 64;

    if (ovs_ratio_x1000 <= 1507) {
        safe_size =
            145 +
            ((((229000 - 144000) / (1507 - 1000)) * (ovs_ratio_x1000 -
                                                     1000)) / 1000);
    } else if ((ovs_ratio_x1000 > 1507) && (ovs_ratio_x1000 <= 1760)) {
        safe_size =
            353 +
            ((((416000 - 353000) / (1760 - 1508)) * (ovs_ratio_x1000 -
                                                     1508)) / 1000);
    } else if ((ovs_ratio_x1000 > 1760) && (ovs_ratio_x1000 <= 1912)) {
        safe_size =
            562 +
            ((((611000 - 562000) / (1912 - 1760)) * (ovs_ratio_x1000 -
                                                     1760)) / 1000);
    } else {
        safe_size = 770;
    }

    return safe_size;

}


/*
 * Function:
 *      lossless_flood_cnt
 * Purpose:
 *      Calculates number of packets that need to be sent to a port for a
 *      lossless swirl
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      pkt_size : Packet size in bytes
 *      port: Device port #
 * Returns:
 *     Number of packets needed for lossless flooding
 *
 */

uint32
lossless_flood_cnt(int unit, uint32 pkt_size, int port)
{
    uint32 flood_cnt = 0;
    uint32 total_cells = 0;
    rate_calc_t *rate_calc_p = rate_calc_parray[unit];

    if (pkt_size == 1) {
        while (total_cells < TARGET_CELL_COUNT) {
            total_cells += num_cells(unit,
                                     rate_calc_p->rand_pkt_sizes[port][flood_cnt],
                                     port);
            flood_cnt++;
        }
    } else {
        flood_cnt = TARGET_CELL_COUNT / (num_cells(unit, pkt_size, port));
    }

    if (flood_cnt < 3) {
        flood_cnt = 3;
    }
    return(flood_cnt);
}


/*
 * Function:
 *      validate_vlan
 * Purpose:
 *      Validates VLAN in VLAN table
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      vlan - VLAN ID
 *      enable : valid bit for the VLAN
 *
 * Returns:
 *     Nothing
 *
 */

void
validate_vlan(int unit, bcm_vlan_t vlan, int enable)
{
    vlan_tab_entry_t vlan_tab_entry;

    soc_mem_read(unit, VLAN_TABm, COPYNO_ALL, vlan, vlan_tab_entry.entry_data);
    soc_mem_field32_set(unit, VLAN_TABm, vlan_tab_entry.entry_data,
                        VALIDf, enable);
    soc_mem_write(unit, VLAN_TABm, COPYNO_ALL, vlan, vlan_tab_entry.entry_data);
}


/*
 * Function:
 *      dma_chan_check_done
 * Purpose:
 *      Check chain done or desc done bit in DMA status register.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      vchan - Virtual channel number
 *      type -  check desc done or chain done
 *      detected - whether desc done or chain done was detected
 *
 * Returns:
 *     SOC_E_XXXX
 *
 */

bcm_error_t
dma_chan_check_done(int unit, int vchan, soc_dma_poll_type_t type, int *detected)
{
    int rv = SOC_E_NONE;
    int cmc = vchan / N_DMA_CHAN;
    int chan = vchan % N_DMA_CHAN;

    switch (type) {
    case SOC_DMA_POLL_DESC_DONE:
        *detected = (soc_pci_read(unit, CMIC_CMCx_DMA_STAT_OFFSET(cmc))
                     & DS_CMCx_DMA_DESC_DONE(chan));
        break;
    case SOC_DMA_POLL_CHAIN_DONE:
        *detected = (soc_pci_read(unit, CMIC_CMCx_DMA_STAT_OFFSET(cmc))
                     & DS_CMCx_DMA_CHAIN_DONE(chan));
        break;
    default:
        break;
    }

    return rv;
}


/*
 * Function:
 *      gen_random_l2_pkt
 * Purpose:
 *      Generate random L2 packet with seq ID
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      pkt_size : Packet size in bytes
 *      mac_da : Destination MAC Address
 *      mac_sa : Source MAC Address
 *      tpid : TPID
 *      vlan_id : VLAN
 *      seq_id : Sequence ID
 *
 * Returns:
 *     Nothing
 *
 */

void
gen_random_l2_pkt(uint8 *pkt_ptr, uint32 pkt_size,
                  uint8 mac_da[NUM_BYTES_MAC_ADDR],
                  uint8 mac_sa[NUM_BYTES_MAC_ADDR], uint16 tpid,
                  uint16 vlan_id, uint32 seq_id)
{
    int i, offset;
    uint32 crc;

    tgp_gen_random_l2_pkt(pkt_ptr, pkt_size, mac_da, mac_sa, tpid, vlan_id);

    /* Replace First 4 Payload Bytes with Sequence ID */
    offset = NUM_BYTES_L2_HDR;
    for (i = 3; i >= 0; i--) {
        pkt_ptr[offset++] = (seq_id >> (i * 8)) & 0x000000ff;
    }

    /* Replace Next 2  Payload Bytes with Packet Size */
    for (i = 1; i >= 0; i--) {
        pkt_ptr[offset++] = (pkt_size >> (i * 8)) & 0x00ff;
    }

    /* Recalculate CRC */
    tgp_populate_crc_table();
    crc = tgp_generate_calculate_crc(pkt_ptr, pkt_size);

    offset = pkt_size - NUM_BYTES_CRC;
    for (i = NUM_BYTES_CRC - 1; i >= 0 ; i--) {
        pkt_ptr[offset++] = (crc >> (i * 8)) & 0x000000ff;
    }
}


/*
 * Function:
 *      calc_oversub_ratio
 * Purpose:
 *      Calculate oversub ratio for each pipe based on port config
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *      pkt_size: packet size in bytes
 *
 * Returns:
 *    Nothing
 */

void
calc_oversub_ratio(int unit, uint32 pkt_size)
{
    int pipe, p;
    int pipe_bandwidth;
    int lr_bandwidth = 0;
    int ov_bandwidth = 0;
    int port_speed;
    uint16 dev_id;
    uint8 rev_id;
    soc_info_t *si = &SOC_INFO(unit);
    rate_calc_t *rate_calc_p = rate_calc_parray[unit];

    cli_out("\nCalculating oversub ratios");
    cli_out("\nOperating Freq = %0d", si->frequency);

    soc_cm_get_id(unit, &dev_id, &rev_id);

    if (dev_id == BCM56960_DEVICE_ID) {
        switch (si->frequency) {
        case TH_FREQ_850:
            pipe_bandwidth = TH_BW_850;
            break;
        case TH_FREQ_765:
            pipe_bandwidth = TH_BW_765;
            break;
        case TH_FREQ_672:
            pipe_bandwidth = TH_BW_672;
            break;
        case TH_FREQ_645:
            pipe_bandwidth = TH_BW_645;
            break;
        case TH_FREQ_545:
            pipe_bandwidth = TH_BW_545;
            break;
        default:
            pipe_bandwidth = TH_BW_850;
        }
    }
    else if (dev_id == BCM56860_DEVICE_ID || dev_id == BCM56850_DEVICE_ID) {
        switch (si->frequency) {
        case TD2P_FREQ_793:
            pipe_bandwidth = TD2P_BW_793;
            break;
        case TD2P_FREQ_635:
            pipe_bandwidth = TD2P_BW_635;
            break;
        case TD2P_FREQ_529:
            pipe_bandwidth = TD2P_BW_529;
            break;
        case TD2P_FREQ_421:
            pipe_bandwidth = TD2P_BW_421;
            break;
        default:
            pipe_bandwidth = TD2P_BW_793;
        }
    }
    else {
        pipe_bandwidth = TD2P_BW_793;
    }

    pipe_bandwidth = pipe_bandwidth - MISC_BW;

    cli_out("\nPer pipe BW = %0d", pipe_bandwidth);

    for (pipe = 0; pipe < si->num_pipe; pipe++) {
        lr_bandwidth = 0;
        ov_bandwidth = 0;
        PBMP_ITER(PBMP_PORT_ALL(unit), p) {
            if (p < SOC_MAX_NUM_PORTS) {
                bcm_port_speed_get(unit, p, &port_speed);
                if (SOC_PBMP_MEMBER(PBMP_PIPE(unit, pipe), p)) {
                    if (SOC_PBMP_MEMBER(PBMP_OVERSUB(unit), p)) {
                        ov_bandwidth += port_speed;
                    } else {
                        lr_bandwidth += port_speed;
                    }
                }
            }
        }

        rate_calc_p->ovs_ratio_x1000[pipe] =
            (ov_bandwidth) / ((pipe_bandwidth - lr_bandwidth) / 1000);

        if (pkt_size >
            safe_pkt_size(unit, rate_calc_p->ovs_ratio_x1000[pipe])) {
            rate_calc_p->ovs_ratio_x1000[pipe] = 1000;
        }

        if (rate_calc_p->ovs_ratio_x1000[pipe] < 1000) {
            rate_calc_p->ovs_ratio_x1000[pipe] = 1000;
        }

        cli_out
            ("\nFor pipe %0d, LR BW = %0d, OV BW = %0d, ovs_ratio(x1000) = %0d",
             pipe, lr_bandwidth, ov_bandwidth,
             rate_calc_p->ovs_ratio_x1000[pipe]);
    }
}


/*
 * Function:
 *      set_exp_rates
 * Purpose:
 *      Set expected rates array (rate_calc_p->exp_rate). This is based on port
 *      speed and oversub ratio (for oversubscribed ports).
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 */


void
set_exp_rates(int unit)
{
    int p;
    int port_speed;
    uint64 ovs_ratio_x1000_64;
    rate_calc_t *rate_calc_p = rate_calc_parray[unit];

    cli_out("\nSetting expected rates");

    PBMP_ITER(PBMP_PORT_ALL(unit), p) {
        if (p < SOC_MAX_NUM_PORTS) {
            bcm_port_speed_get(unit, p, &port_speed);
            if (IS_HG_PORT(unit, p)) {
                switch (port_speed) {
                    case 120000: port_speed = 127000; break;
                    case 100000: port_speed = 106000; break;
                    case 40000:  port_speed = 42000;  break;
                    case 20000:  port_speed = 21000;  break;
                    case 10000:  port_speed = 11000;  break;
                    default: break;
                }
            }
            COMPILER_64_SET(rate_calc_p->exp_rate[p], 0, port_speed);
            if (SOC_PBMP_MEMBER(PBMP_OVERSUB(unit), p)) {
                COMPILER_64_UMUL_32(rate_calc_p->exp_rate[p], 1000000000);
                COMPILER_64_SET(ovs_ratio_x1000_64, 0,
                                rate_calc_p->ovs_ratio_x1000[get_pipe(unit, p)]);
                COMPILER_64_UDIV_64(rate_calc_p->exp_rate[p], ovs_ratio_x1000_64);
            } else {
                COMPILER_64_UMUL_32(rate_calc_p->exp_rate[p], 1000000);
            }
        }
    }
}


/*
 * Function:
 *      turn_off_cmic_mmu_bkp
 * Purpose:
 *      Turn off CMIC to MMU backpressure
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 */

void
turn_off_cmic_mmu_bkp(int unit)
{
    int cmc, ch;

    for (cmc = 0; cmc < SOC_CMCS_NUM(unit); cmc++) {
        for (ch = 0; ch < N_DMA_CHAN; ch++) {
            soc_pci_write(unit, CMIC_CMCx_CHy_RXBUF_THRESHOLD_CONFIG(cmc, ch),
                          0x20);
        }
    }
}


/*
 * Function:
 *      turn_off_fc
 * Purpose:
 *      Turn off flow control at the MAC, IDB and MMU.
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *      pbmp - Device port bitmap
 *
 * Returns:
 *     Nothing
 */

void
turn_off_fc(int unit, pbmp_t pbmp)
{
    int p;
    int obm_subp;
    int idx;
    uint64 idb_fc;

    soc_reg_t idb_fcc_regs[] = {
        IDB_OBM0_FLOW_CONTROL_CONFIG_PIPE0r,
        IDB_OBM1_FLOW_CONTROL_CONFIG_PIPE0r,
        IDB_OBM2_FLOW_CONTROL_CONFIG_PIPE0r,
        IDB_OBM3_FLOW_CONTROL_CONFIG_PIPE0r,
        IDB_OBM4_FLOW_CONTROL_CONFIG_PIPE0r,
        IDB_OBM5_FLOW_CONTROL_CONFIG_PIPE0r,
        IDB_OBM6_FLOW_CONTROL_CONFIG_PIPE0r,
        IDB_OBM7_FLOW_CONTROL_CONFIG_PIPE0r,
        IDB_OBM0_FLOW_CONTROL_CONFIG_PIPE1r,
        IDB_OBM1_FLOW_CONTROL_CONFIG_PIPE1r,
        IDB_OBM2_FLOW_CONTROL_CONFIG_PIPE1r,
        IDB_OBM3_FLOW_CONTROL_CONFIG_PIPE1r,
        IDB_OBM4_FLOW_CONTROL_CONFIG_PIPE1r,
        IDB_OBM5_FLOW_CONTROL_CONFIG_PIPE1r,
        IDB_OBM6_FLOW_CONTROL_CONFIG_PIPE1r,
        IDB_OBM7_FLOW_CONTROL_CONFIG_PIPE1r,
        IDB_OBM0_FLOW_CONTROL_CONFIG_PIPE2r,
        IDB_OBM1_FLOW_CONTROL_CONFIG_PIPE2r,
        IDB_OBM2_FLOW_CONTROL_CONFIG_PIPE2r,
        IDB_OBM3_FLOW_CONTROL_CONFIG_PIPE2r,
        IDB_OBM4_FLOW_CONTROL_CONFIG_PIPE2r,
        IDB_OBM5_FLOW_CONTROL_CONFIG_PIPE2r,
        IDB_OBM6_FLOW_CONTROL_CONFIG_PIPE2r,
        IDB_OBM7_FLOW_CONTROL_CONFIG_PIPE2r,
        IDB_OBM0_FLOW_CONTROL_CONFIG_PIPE3r,
        IDB_OBM1_FLOW_CONTROL_CONFIG_PIPE3r,
        IDB_OBM2_FLOW_CONTROL_CONFIG_PIPE3r,
        IDB_OBM3_FLOW_CONTROL_CONFIG_PIPE3r,
        IDB_OBM4_FLOW_CONTROL_CONFIG_PIPE3r,
        IDB_OBM5_FLOW_CONTROL_CONFIG_PIPE3r,
        IDB_OBM6_FLOW_CONTROL_CONFIG_PIPE3r,
        IDB_OBM7_FLOW_CONTROL_CONFIG_PIPE3r
    };

    soc_reg_t pgw_fcc_regs[] = {
        PGW_OBM_PORT0_FC_CONFIGr,
        PGW_OBM_PORT1_FC_CONFIGr,
        PGW_OBM_PORT2_FC_CONFIGr,
        PGW_OBM_PORT3_FC_CONFIGr,
        PGW_OBM_PORT4_FC_CONFIGr,
        PGW_OBM_PORT5_FC_CONFIGr,
        PGW_OBM_PORT6_FC_CONFIGr,
        PGW_OBM_PORT7_FC_CONFIGr,
        PGW_OBM_PORT8_FC_CONFIGr,
        PGW_OBM_PORT9_FC_CONFIGr,
        PGW_OBM_PORT10_FC_CONFIGr,
        PGW_OBM_PORT11_FC_CONFIGr,
        PGW_OBM_PORT12_FC_CONFIGr,
        PGW_OBM_PORT13_FC_CONFIGr,
        PGW_OBM_PORT14_FC_CONFIGr,
        PGW_OBM_PORT15_FC_CONFIGr
    };

    soc_field_t idb_flow_control_config_fields[] =
        { PORT_FC_ENf, LOSSLESS1_FC_ENf, LOSSLESS0_FC_ENf,
        LOSSLESS1_PRIORITY_PROFILEf, LOSSLESS0_PRIORITY_PROFILEf
    };
    uint32 idb_flow_control_config_values[] = { 0x0, 0x0, 0x0, 0xff, 0xff };

    cli_out("\nTurning off flow control at IDB/MMU");

    COMPILER_64_SET(idb_fc, 0x0, 0x0);

    PBMP_ITER(pbmp, p) {
        if (p < SOC_MAX_NUM_PORTS) {
            bcm_port_pause_set(unit, p, FALSE, FALSE);
            soc_reg_field32_modify(unit, THDI_INPUT_PORT_XON_ENABLESr, p,
                                   PORT_PAUSE_ENABLEf, 0x0);
        }
    }
    soc_reg_field32_modify(unit, THDI_INPUT_PORT_XON_ENABLESr, 0,
                           PORT_PAUSE_ENABLEf, 0x0);

    if (SOC_REG_IS_VALID(unit, IDB_OBM0_FLOW_CONTROL_CONFIG_PIPE0r)) {
        soc_reg_fields32_modify(unit, IDB_OBM0_FLOW_CONTROL_CONFIG_PIPE0r,
                                REG_PORT_ANY, 5, idb_flow_control_config_fields,
                                idb_flow_control_config_values);
        (void) soc_reg_get(unit, IDB_OBM0_FLOW_CONTROL_CONFIG_PIPE0r, 0, 0,
                           &idb_fc);
    }

    for (obm_subp = 0; obm_subp < NUM_SUBP_OBM; obm_subp++) {
        for(idx = 0; idx < (sizeof(idb_fcc_regs) / sizeof(soc_reg_t)); idx++) {
            if(SOC_REG_IS_VALID(unit, idb_fcc_regs[idx])) {
                (void) soc_reg_set(unit, idb_fcc_regs[idx], 0, obm_subp, idb_fc);
            }
        }
    }

    for (idx = 0; idx < (sizeof(pgw_fcc_regs) / sizeof(soc_reg_t)); idx++) {
        if (SOC_REG_IS_VALID(unit, pgw_fcc_regs[idx])) {
            soc_reg_field32_modify(unit, pgw_fcc_regs[idx], REG_PORT_ANY,
                               PORT_FC_ENABLEf, 0x0);
        }
    }
}


/*
 * Function:
 *      set_port_property_arrays
 * Purpose:
 *      Set rand_pkt_sizes, port_oversub, and exp_rates arrays.
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *      pbmp - Device port bitmap
 *      pkt_size_param - Packet size in bytes
 *      max_num_cells_param - Maximum number of cells
 *
 * Returns:
 *     Nothing
 */

void
set_port_property_arrays(int unit, pbmp_t pbmp, int pkt_size_param,
                         int max_num_cells_param)
{
    int p, j;
    uint32 pkt_size;
    rate_calc_t *rate_calc_p = rate_calc_parray[unit];

    calc_oversub_ratio(unit, pkt_size_param);

    set_exp_rates(unit);

    PBMP_ITER(pbmp, p) {
        if (p < SOC_MAX_NUM_PORTS) {
            for (j = 0; j < TARGET_CELL_COUNT; j++) {
                do {
                    /* coverity[dont_call : FALSE] */
                    pkt_size = (sal_rand() % (MTU - MIN_PKT_SIZE + 1)) +
                               MIN_PKT_SIZE;
                } while (num_cells(unit, pkt_size, p) > max_num_cells_param);
                rate_calc_p->rand_pkt_sizes[p][j] = pkt_size;
            }
        }
    }
}


/*
 * Function:
 *      set_up_streams
 * Purpose:
 *      VLAN programming. Each port is put on an unique VLAN.
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *      pbmp - Device port bitmap
 *      vlan_bmp - bitmap of device ports that have vlan set up
 *
 * Returns:
 *     vlan_pbmp - bitmap of device ports that have vlan set up.
 */

pbmp_t
set_up_streams(int unit, pbmp_t pbmp, pbmp_t vlan_pbmp)
{
    int p;
    pbmp_t pbm, ubm;
    bcm_vlan_t vlan;

    BCM_PBMP_CLEAR(ubm);

    PBMP_ITER(pbmp, p) {
        if (p < SOC_MAX_NUM_PORTS) {
            vlan = VLAN + p;
            if (SOC_PBMP_MEMBER(vlan_pbmp, p)) {
                validate_vlan(unit, vlan, 1);
            } else {
                vlan = VLAN + p;
                BCM_PBMP_CLEAR(pbm);
                bcm_vlan_create(unit, vlan);
                BCM_PBMP_PORT_ADD(pbm, p);
                bcm_vlan_port_add(unit, vlan, pbm, ubm);
            }
        }
    }

    SOC_PBMP_OR(vlan_pbmp, pbmp);

    return vlan_pbmp;
}


/*
 * Function:
 *      soc_set_lpbk
 * Purpose:
 *      Enable and disable MAC/PHY loopback on selected ports
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *      loopback: loopback mode.
 *
 * Returns:
 *     SOC_E_XXXX
 */

bcm_error_t
soc_set_lpbk(int unit, pbmp_t pbmp, int loopback)
{
    int p;

    if (loopback == BCM_PORT_LOOPBACK_PHY_REMOTE) {
        cli_out("\nEnabling Remote PHY loopbacks");
    } else if (loopback == BCM_PORT_LOOPBACK_PHY) {
        cli_out("\nEnabling PHY loopbacks");
    } else if (loopback == BCM_PORT_LOOPBACK_MAC) {
        cli_out("\nEnabling MAC loopbacks");
    } else if (loopback == BCM_PORT_LOOPBACK_NONE) {
        cli_out("\nDisabling loopbacks");
    }
    PBMP_ITER(pbmp, p) {
        if (p < SOC_MAX_NUM_PORTS) {
            BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, p, loopback));
        }
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *      send_pkts
 * Purpose:
 *      Send packets to flood VLANs and create a swirl on each port.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      pbmp - Device port bitmap
 *      pkt_size_param - Packet size in bytes
 *      flood_cnt_param - Number of packets needed to saturate the port bandwidth
 *      pkt_seed - random seed to recreate the packet
 *
 * Returns:
 *     Nothing
 *
 */

void
send_pkts(int unit, pbmp_t pbmp, uint32 pkt_size_param, uint32 flood_cnt_param,
          uint32 pkt_seed)
{
    uint8 mac_da[] = MAC_DA;
    uint8 mac_sa[] = MAC_SA;
    uint8 *packet_store_ptr;
    uint32 pkt_size;
    int p, j;
    pbmp_t lp_pbm, empty_pbm0, empty_pbm1;
    dv_t *dv_tx;
    int channel_done;
    int flags = 0;
    uint32 pkt_count = 0;
    uint32 flood_cnt;
    uint32 use_random_packet_sizes = 0;
    rate_calc_t *rate_calc_p = rate_calc_parray[unit];

    soc_dma_init(unit);

    dv_tx = soc_dma_dv_alloc(unit, DV_TX, 3);

    SOC_PBMP_CLEAR(lp_pbm);
    SOC_PBMP_PORT_ADD(lp_pbm, 1);
    SOC_PBMP_CLEAR(empty_pbm0);
    SOC_PBMP_CLEAR(empty_pbm1);

    cli_out("\nSending packets ...");

    PBMP_ITER(pbmp, p) {
        if (p < SOC_MAX_NUM_PORTS) {
            if (pkt_size_param == 0) {
                if (IS_HG_PORT(unit, p)) {
                    pkt_size = HG2_WC_PKT_SIZE;
                } else {
                    pkt_size = ENET_WC_PKT_SIZE;
                }
            } else {
                pkt_size = pkt_size_param;
            }
    
            if (flood_cnt_param == 0) {
                flood_cnt = lossless_flood_cnt(unit, pkt_size, p);
            } else {
                flood_cnt = flood_cnt_param;
            }
    
            if (pkt_size == 1) {
                use_random_packet_sizes = 1;
            }
    
            for (j = 0; j < flood_cnt; j++) {
                if (use_random_packet_sizes == 1) {
                    pkt_size = rate_calc_p->rand_pkt_sizes[p][j];
                }
                packet_store_ptr =
                    sal_dma_alloc(pkt_size * sizeof(uint8), "packet");
    
                pkt_count++;
                channel_done = 0;
                soc_dma_abort_dv(unit, dv_tx);
                sal_srand(pkt_seed + p + j * 128);
                gen_random_l2_pkt(packet_store_ptr, pkt_size, mac_da, mac_sa,
                                  TPID, (VLAN + p), j);
                soc_dma_dv_reset(DV_TX, dv_tx);
                soc_dma_desc_add(dv_tx, (sal_vaddr_t) (packet_store_ptr),
                                 pkt_size, lp_pbm, empty_pbm0, empty_pbm1, flags,
                                 NULL);
                soc_dma_desc_end_packet(dv_tx);
                soc_dma_chan_config(unit, TX_CHAN, DV_TX, SOC_DMA_F_POLL);
                soc_dma_start(unit, TX_CHAN, dv_tx);
    
                while (channel_done == 0) {
                    dma_chan_check_done(unit, TX_CHAN, SOC_DMA_POLL_CHAIN_DONE,
                                        &channel_done);
                }
                sal_dma_free(packet_store_ptr);
            }
    
            LOG_INFO(BSL_LS_APPL_TESTS,
                     (BSL_META_U(unit, "\nDevice port %0d sent %0d packets"),
                      p, flood_cnt));
        }
    }
    cli_out("\n%0d packets sent", pkt_count);
    soc_dma_abort_dv(unit, dv_tx);
    soc_dma_dv_reset(DV_TX, dv_tx);
    soc_dma_dv_free(unit, dv_tx);
}


/*
 * Function:
 *      start_rate_measurement
 * Purpose:
 *      Record start time and start counts for rate measurement.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      pbmp - Device port bitmap
 *
 * Returns:
 *     Nothing
 *
 */

void
start_rate_measurement(int unit, pbmp_t pbmp)
{
    int p;
    uint64 rdata;
    rate_calc_t *rate_calc_p = rate_calc_parray[unit];

    PBMP_ITER(pbmp, p) {
        if  (p < SOC_MAX_NUM_PORTS) {
            rate_calc_p->stime[p] = sal_time_usecs();
            (void) soc_reg_get(unit, TPKTr, p, 0, &rdata);
            COMPILER_64_SET(rate_calc_p->tpkt_start[p], COMPILER_64_HI(rdata),
                            COMPILER_64_LO(rdata));
            (void) soc_reg_get(unit, TBYTr, p, 0, &rdata);
            COMPILER_64_SET(rate_calc_p->tbyt_start[p], COMPILER_64_HI(rdata),
                            COMPILER_64_LO(rdata));
        }
    }
}


/*
 * Function:
 *      set_up_ports
 * Purpose:
 *      Enable port bridging and HiGig lookup for HG2 ports
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      pbmp - Device port bitmap
 *
 * Returns:
 *     Nothing
 *
 */

void
set_up_ports(int unit, pbmp_t pbmp)
{
    int i, p;
    lport_tab_entry_t lport_tab_entry;
    port_tab_entry_t port_tab_entry;
    soc_field_t ihg_lookup_fields[] =
        { HYBRID_MODE_ENABLEf, USE_MH_PKT_PRIf, USE_MH_VIDf, HG_LOOKUP_ENABLEf,
        REMOVE_MH_SRC_PORTf
    };
    uint32 ihg_lookup_values[] = { 0x0, 0x1, 0x1, 0x1, 0x0 };

    cli_out("\nEnabling HG_LOOKUP on HG ports");

    PBMP_ITER(pbmp, p) {
        if (p < SOC_MAX_NUM_PORTS && IS_HG_PORT(unit, p)) {
            soc_reg_fields32_modify(unit, IHG_LOOKUPr, p, 5,
                                    ihg_lookup_fields, ihg_lookup_values);
        }
    }

    cli_out("\nEnabling Port bridging");

    for (i = 0; i < soc_mem_index_max(unit, LPORT_TABm); i++) {
        soc_mem_read(unit, LPORT_TABm, COPYNO_ALL, i,
                     lport_tab_entry.entry_data);
        soc_mem_field32_set(unit, LPORT_TABm, lport_tab_entry.entry_data,
                            ALLOW_SRC_MODf, 0x1);
        soc_mem_field32_set(unit, LPORT_TABm, lport_tab_entry.entry_data,
                            PORT_BRIDGEf, 0x1);
        soc_mem_write(unit, LPORT_TABm, COPYNO_ALL, i,
                      lport_tab_entry.entry_data);
    }

    for (i = 0; i < soc_mem_index_max(unit, PORT_TABm); i++) {
        soc_mem_read(unit, PORT_TABm, COPYNO_ALL, i,
                     port_tab_entry.entry_data);
        soc_mem_field32_set(unit, PORT_TABm, port_tab_entry.entry_data,
                            PORT_BRIDGEf, 0x1);
        soc_mem_write(unit, PORT_TABm, COPYNO_ALL, i,
                          port_tab_entry.entry_data);
    }
}


/*
 * Function:
 *      check_mib_counters
 * Purpose:
 *      Checks MIB counters in MAC, IP, and EP
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      pbmp - Device port bitmap to be checked
 *
 * Returns:
 *     SOC_E_XXXX
 *
 */

bcm_error_t
check_mib_counters(int unit, pbmp_t pbmp)
{
    uint32 p, j;
    uint64 rdata;
    bcm_error_t rv = BCM_E_NONE;
    soc_reg_t good_counters[] = {
                                    RUCr,
                                    RPKTr,
                                    RUCAr,
                                    RBYTr,
                                    TPKTr,
                                    TUCAr,
                                    TBYTr
                                };
    soc_reg_t error_counters[] = {
                                    RFLRr,
                                    RFCSr,
                                    RJBRr,
                                    RMTUEr,
                                    RTRFUr,
                                    RERPKTr,
                                    RRPKTr,
                                    RUNDr,
                                    RFRGr,
                                    RRBYTr,
                                    TJBRr,
                                    TFCSr,
                                    TERRr,
                                    TFRGr,
                                    TRPKTr,
                                    TUFLr,
                                    TPCEr,
                                    RDISCr,
                                    RIPHE4r,
                                    RIPHE6r,
                                    RIPD4r,
                                    RIPD6r,
                                    RPORTDr
                                 };

    PBMP_ITER(pbmp, p) {
        if (p < SOC_MAX_NUM_PORTS) {
            for (j = 0; j < (sizeof(good_counters) / sizeof(soc_reg_t)); j++) {
                SOC_IF_ERROR_RETURN(soc_reg_get(unit, good_counters[j], p, 0,
                                                &rdata));
                if (COMPILER_64_IS_ZERO(rdata)) {
                    cli_out("\n*ERROR: Counter %s has a zero value for port %0d",
                            SOC_REG_NAME(unit, good_counters[j]), p);
                    rv = BCM_E_FAIL;
                }
            }
            for (j = 0; j < (sizeof(error_counters) / sizeof(soc_reg_t)); j++) {
                SOC_IF_ERROR_RETURN(soc_reg_get(unit, error_counters[j], p, 0,
                                                &rdata));
                if (!COMPILER_64_IS_ZERO(rdata)) {
                    cli_out("\n*ERROR: Error counter %s has a non zero value for port %0d",
                            SOC_REG_NAME(unit, error_counters[j]), p);
                    rv = BCM_E_FAIL;
                }
            }
        }
    }
    if (rv != BCM_E_NONE) {
        test_error(unit, "********* COUNTER CHECK FAILED *********\n");
    } else {
        cli_out("\n********* COUNTER CHECK PASSED *********");
    }
    return rv;
}


/*
 * Function:
 *      check_rates
 * Purpose:
 *      Check rates against expected rates.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      pbmp - Device port bitmap to be checked
 *      tolerance_lr - Percentage of rate tolerance for line rate ports
 *      tolerance_os - Percentage of rate tolerance for oversub ports
 *
 * Returns:
 *     SOC_E_XXXX
 *
 */

bcm_error_t
check_rates(int unit, pbmp_t pbmp, uint32 tolerance_lr, uint32 tolerance_os)
{
    int p;
    soc_info_t *si;
    sal_usecs_t etime[SOC_MAX_NUM_PORTS];
    uint64 tpkt_end[SOC_MAX_NUM_PORTS];
    uint64 tbyt_end[SOC_MAX_NUM_PORTS];
    uint64 rdata;
    uint32 ipg, preamble;
    uint64 tbyt_delta, tpkt_delta;
    sal_usecs_t dtime;
    uint64 rate_calc_int_64;
    uint64 rate[SOC_MAX_NUM_PORTS];
    uint64 min_rate;
    uint64 max_rate;
    uint64 exp_rate;
    uint32 tolerance;
    uint64 margin_of_error;
    uint64 hundred_64, meg;
    char exp_rate_int_str[32], avg_rate_int_str[32];
    rate_calc_t *rate_calc_p = rate_calc_parray[unit];
    bcm_error_t rv = BCM_E_NONE;

    si = &SOC_INFO(unit);
    COMPILER_64_SET(hundred_64, 0, 100);
    COMPILER_64_SET(meg, 0, 1000000);

    PBMP_ITER(pbmp, p) {
        if (p < SOC_MAX_NUM_PORTS) {
            etime[p] = sal_time_usecs();
            SOC_IF_ERROR_RETURN(soc_reg_get(unit, TPKTr, p, 0, &rdata));
            COMPILER_64_SET(tpkt_end[p], COMPILER_64_HI(rdata),
                                         COMPILER_64_LO(rdata));
            SOC_IF_ERROR_RETURN(soc_reg_get(unit, TBYTr, p, 0, &rdata));
            COMPILER_64_SET(tbyt_end[p], COMPILER_64_HI(rdata),
                                         COMPILER_64_LO(rdata));
        }
    }

    PBMP_ITER(pbmp, p) {
        if (p < SOC_MAX_NUM_PORTS) {
            if (IS_HG_PORT(unit, p)) {
                ipg = HG2_IPG;
                preamble = 0;
            } else {
                ipg = ENET_IPG;
                preamble = ENET_PREAMBLE;
            }
            COMPILER_64_DELTA(tbyt_delta, rate_calc_p->tbyt_start[p], tbyt_end[p]);
            COMPILER_64_DELTA(tpkt_delta, rate_calc_p->tpkt_start[p], tpkt_end[p]);
            COMPILER_64_UMUL_32(tpkt_delta, (ipg + preamble));
            COMPILER_64_ADD_64(tbyt_delta, tpkt_delta);
            COMPILER_64_UMUL_32(tbyt_delta, 8);
            COMPILER_64_SET(rate[p], COMPILER_64_HI(tbyt_delta),
                                     COMPILER_64_LO(tbyt_delta));
            dtime = SAL_USECS_SUB(etime[p], rate_calc_p->stime[p]);
            COMPILER_64_SET(rate_calc_int_64, 0, dtime);
        COMPILER_64_UDIV_64(rate[p], rate_calc_int_64);
        }
    }

    PBMP_ITER(pbmp, p) {
        if (p < SOC_MAX_NUM_PORTS &&
            !(COMPILER_64_IS_ZERO(rate_calc_p->exp_rate[p]))) {
            if (SOC_PBMP_MEMBER(PBMP_OVERSUB(unit), p)) {
                tolerance = tolerance_os;
            } else {
                tolerance = tolerance_lr;
            }

#define RATE_EXP    (rate_calc_p->exp_rate[p])
#define RATE_EXP_HI (COMPILER_64_HI(RATE_EXP))
#define RATE_EXP_LO (COMPILER_64_LO(RATE_EXP))

            /* margin_of_error */
            COMPILER_64_SET(margin_of_error, RATE_EXP_HI, RATE_EXP_LO);
            COMPILER_64_UMUL_32(margin_of_error, tolerance);
            COMPILER_64_UDIV_64(margin_of_error, hundred_64);
            /* min_rate */
            COMPILER_64_DELTA(min_rate, margin_of_error, RATE_EXP);
            COMPILER_64_UDIV_64(min_rate, meg);
            /* max_rate */
            COMPILER_64_SET(max_rate, RATE_EXP_HI, RATE_EXP_LO);
            COMPILER_64_ADD_64(max_rate, margin_of_error);
            COMPILER_64_UDIV_64(max_rate, meg);

            cli_out("\n");
            if ((COMPILER_64_LT(rate[p], min_rate)) ||
                (COMPILER_64_GT(rate[p], max_rate))) {
                cli_out("*ERROR: ");
                rv = BCM_E_FAIL;
            }

            COMPILER_64_SET(exp_rate, RATE_EXP_HI, RATE_EXP_LO);
            COMPILER_64_UDIV_64(exp_rate, meg);
            format_uint64_decimal(exp_rate_int_str,
                                  exp_rate, 0);
            format_uint64_decimal(avg_rate_int_str, rate[p], 0);
            cli_out("Dev Port %3d, Phy Port %3d, ", p, si->port_l2p_mapping[p]);
            cli_out("Exp Rate %6s Mbps, Avg Rate %6s Mbps",
                     exp_rate_int_str, avg_rate_int_str);
        }
    }
    if (rv != BCM_E_NONE) {
        test_error(unit, "********** RATE CHECK FAILED ***********\n");
    } else {
        cli_out("\n********** RATE CHECK PASSED ***********");
    }
    return rv;
}


/*
 * Function:
 *      check_packet_integrity
 * Purpose:
 *      Redirect all packets back to CPU and check packet integrity
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      pbmp - Device port bitmap to be checked
 *      pkt_size_param - Packet size in bytes
 *      flood_cnt_param - Number of packets needed to saturate the port bandwidth
 *      pkt_seed - random seed to recreate the packet
 *
 * Returns:
 *     SOC_E_XXXX
 *
 */

bcm_error_t
check_packet_integrity(int unit, pbmp_t pbmp, uint32 pkt_size_param,
                       uint32 flood_cnt_param, uint32 pkt_seed)
{
    bcm_error_t rv = BCM_E_NONE;
    uint8 mac_da[] = MAC_DA;
    uint8 mac_sa[] = MAC_SA;
    uint8 *ref_pkt_ptr;
    uint8 *rx_pkt_ptr;
    uint32 pkt_size;
    int p, j, k;
    pbmp_t lp_pbm, empty_pbm0, empty_pbm1;
    dv_t *dv_rx;
    int channel_done;
    int flags = 0;
    uint32 exp_pkt_count = 0;
    uint32 pkt_count = 0;
    uint32 pkt_count_port = 0;
    uint32 flood_cnt;
    uint32 timeout;
    uint32 pkt_match = 1;
    uint32 cnt_match = 1;
    uint32 print_mismatch_pkt = 1;
    uint32 seq_id;

    dv_rx = soc_dma_dv_alloc(unit, DV_RX, 3);

    SOC_PBMP_CLEAR(lp_pbm);
    SOC_PBMP_PORT_ADD(lp_pbm, 1);
    SOC_PBMP_CLEAR(empty_pbm0);
    SOC_PBMP_CLEAR(empty_pbm1);

    cli_out("\nRouting all packets back to CPU to check pkt integrity ...");

    soc_pci_write(unit, CMIC_CMCx_CHy_COS_CTRL_RX_0_OFFSET(0, RX_CHAN),
                  0xffffffff);
    soc_pci_write(unit, CMIC_CMCx_CHy_COS_CTRL_RX_1_OFFSET(0, RX_CHAN),
                  0xffffffff);

    soc_reg_field32_modify(unit, CPU_CONTROL_0r, 0, UVLAN_TOCPUf, 0x1);

    ref_pkt_ptr = sal_dma_alloc(MTU * sizeof(uint8), "ref_packet");
    rx_pkt_ptr = sal_dma_alloc(MTU * sizeof(uint8), "rx_pkt");
    if (ref_pkt_ptr == NULL || rx_pkt_ptr == NULL) {
        if (ref_pkt_ptr) {
            sal_dma_free(ref_pkt_ptr);
        }
        if (rx_pkt_ptr) {
            sal_dma_free(rx_pkt_ptr);
        }
        test_error(unit, "Failed to allocate DMA memory for packet integrity check\n");
        return BCM_E_FAIL;
    }
    
    PBMP_ITER(pbmp, p) {
        if (p < SOC_MAX_NUM_PORTS) {
            if (pkt_size_param == 0) {
                if (IS_HG_PORT(unit, p)) {
                    pkt_size = HG2_WC_PKT_SIZE;
                } else {
                    pkt_size = ENET_WC_PKT_SIZE;
                }
            } else {
                pkt_size = pkt_size_param;
            }
    
            if (flood_cnt_param == 0) {
                flood_cnt = lossless_flood_cnt(unit, pkt_size, p);
            } else {
                flood_cnt = flood_cnt_param;
            }
            exp_pkt_count += flood_cnt;
    
            for (j = 0; j < flood_cnt; j++) {
                for (k = 0; k < MTU; k++) {
                    rx_pkt_ptr[k] = 0x00;
                }
    
                channel_done = 0;
                soc_dma_abort_dv(unit, dv_rx);
    
                soc_dma_dv_reset(DV_RX, dv_rx);
                soc_dma_desc_add(dv_rx, (sal_vaddr_t) (rx_pkt_ptr), MTU,
                                 lp_pbm, empty_pbm0, empty_pbm1, flags, NULL);
                soc_dma_desc_end_packet(dv_rx);
                soc_dma_chan_config(unit, RX_CHAN, DV_RX, SOC_DMA_F_POLL);
    
                soc_dma_start(unit, RX_CHAN, dv_rx);
                if (j == 0) {
                    validate_vlan(unit, VLAN + p, 0);
                }
    
                if (SAL_BOOT_RTLSIM ||
                    soc_property_get(unit, spn_EMULATION_REGS, 0)) {
                    timeout = RXDMA_TIMEOUT * 10;
                } else {
                    timeout = RXDMA_TIMEOUT;
                }
                while (channel_done == 0 && timeout > 0) {
                    dma_chan_check_done(unit, RX_CHAN, SOC_DMA_POLL_CHAIN_DONE,
                                        &channel_done);
                    timeout--;
                }
                if (timeout == 0) {
                    cli_out("\n*ERROR: Timed out waiting for pkt from port %0d",
                            p);
                    break;
                }
    
                pkt_size = 0x00000000;
                pkt_size |= (rx_pkt_ptr[NUM_BYTES_L2_HDR + 4] << 8);
                pkt_size |= (rx_pkt_ptr[NUM_BYTES_L2_HDR + 5]);
    
                if (timeout > 0 && pkt_size > 0) {
                    pkt_count++;
                    pkt_count_port++;
                    seq_id = 0x00000000;
    
                    seq_id |= (rx_pkt_ptr[NUM_BYTES_L2_HDR] << 24);
                    seq_id |= (rx_pkt_ptr[NUM_BYTES_L2_HDR + 1] << 16);
                    seq_id |= (rx_pkt_ptr[NUM_BYTES_L2_HDR + 2] << 8);
                    seq_id |= (rx_pkt_ptr[NUM_BYTES_L2_HDR + 3]);
    
                    sal_srand(pkt_seed + p + seq_id * 128);
                    gen_random_l2_pkt(ref_pkt_ptr, pkt_size, mac_da, mac_sa,
                                      TPID, (VLAN + p), seq_id);
    
                    for (k = 0; k < (pkt_size - NUM_BYTES_CRC); k++) {
                        if (rx_pkt_ptr[k] != ref_pkt_ptr[k]) {
                            pkt_match = 0;
                            if (print_mismatch_pkt) {
                                cli_out("\n*ERROR: Device Port %0d has packet corruption",
                                        p);
                                cli_out("\nExpected Packet:");
                                tgp_print_pkt(ref_pkt_ptr, pkt_size);
                                cli_out("\nReceived Packet:");
                                tgp_print_pkt(rx_pkt_ptr, pkt_size);
                                print_mismatch_pkt = 0;
                            }
                            break;
                        }
                    }
                }
            }
    
            if (pkt_count_port < flood_cnt) {
                cli_out("\n*ERROR: Device Port %0d expected %0d pkts, received %0d packets",
                        p, flood_cnt, pkt_count_port);
                cnt_match = 0;
            } else {
                LOG_INFO(BSL_LS_APPL_TESTS,
                         (BSL_META_U(unit, "\nDevice port %0d received %0d packets"),
                          p, pkt_count_port));
            }
            pkt_count_port = 0;
            print_mismatch_pkt = 1;
        }
    }

    cli_out("\n%0d packets received", pkt_count);
    soc_dma_abort_dv(unit, dv_rx);
    soc_dma_dv_reset(DV_RX, dv_rx);
    soc_dma_dv_free(unit, dv_rx);

    if ((pkt_count == exp_pkt_count) && (pkt_match == 1) && (cnt_match == 1)) {
        cli_out("\n**** PACKET INTEGRITY CHECK PASSED *****");
    } else {
        test_error(unit, "**** PACKET INTEGRITY CHECK FAILED *****\n");
        rv = BCM_E_FAIL;
    }

    
    sal_dma_free(rx_pkt_ptr);
    sal_dma_free(ref_pkt_ptr);
    return rv;
}

#endif /* BCM_ESW_SUPPORT */
#endif /* _STREAMING_LIBRARY_H */
