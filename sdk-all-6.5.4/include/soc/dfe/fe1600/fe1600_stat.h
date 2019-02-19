/*
 * $Id: fe1600_stat.h,v 1.2 Broadcom SDK $
 *
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
 * FE1600 STAT H
 */
 
#ifndef _SOC_FE1600_STAT_H_
#define _SOC_FE1600_STAT_H_

#include <bcm/stat.h>
#include <soc/dfe/cmn/dfe_defs.h>
#include <soc/error.h>

typedef enum soc_fe1600_counters_e {
    soc_fe1600_counters_tx_control_cells_counter = 0,
    soc_fe1600_counters_tx_data_cell_counter = 1,
    soc_fe1600_counters_tx_data_byte_counter = 2,
    soc_fe1600_counters_rx_crc_errors_counter = 3, 
    soc_fe1600_counters_rx_lfec_fec_correctable_error = 4, 
    soc_fe1600_counters_rx_bec_crc_error = 4,
    soc_fe1600_counters_rx_8b_10b_disparity_errors = 4,
    soc_fe1600_counters_rx_control_cells_counter = 5,
    soc_fe1600_counters_rx_data_cell_counter = 6,
    soc_fe1600_counters_rx_data_byte_counter = 7,
    soc_fe1600_counters_rx_dropped_retransmitted_control = 8,
    soc_fe1600_counters_tx_bec_retransmit = 9,
    soc_fe1600_counters_rx_bec_retransmit = 10,
    soc_fe1600_counters_tx_asyn_fifo_rate_at_units_of_40_bits = 11,
    soc_fe1600_counters_rx_asyn_fifo_rate_at_units_of_40_bits = 12,
    soc_fe1600_counters_rx_lfec_fec_uncorrectable_erros = 13,
    soc_fe1600_counters_rx_bec_rx_fault = 13,
    soc_fe1600_counters_rx_8b_10b_code_errors = 13,
    soc_fe1600_nof_counters = 14
}soc_fe1600_counters_t;

soc_error_t soc_fe1600_stat_init(int unit);
soc_error_t soc_fe1600_stat_is_supported_type(int unit, bcm_port_t port, bcm_stat_val_t type);
soc_error_t soc_fe1600_stat_get(int unit,bcm_port_t port,uint64 *value,int *counters,int arr_size);
soc_error_t soc_fe1600_mapping_stat_get(int unit,bcm_port_t port, int *counters,int *array_size ,bcm_stat_val_t type,int max_array_size);

soc_error_t soc_fe1600_controlled_counter_set(int unit);
int soc_fe1600_controlled_counter_get(int unit, int counter_id, int port, uint64* val);
soc_error_t soc_fe1600_stat_counter_length_get(int unit, int counter_id, int *length);
#endif /*_SOC_FE1600_STAT_H_*/
