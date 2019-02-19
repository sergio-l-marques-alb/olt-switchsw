/*
 * $Id: jer_stat.h,v 1.2 Broadcom SDK $
 *
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
 * JERICHO STAT H
 */
 
#ifndef _SOC_JER_MAC_STAT_H_
#define _SOC_JER_MAC_STAT_H_

#include <soc/error.h>
#include <soc/dcmn/error.h>
#include <bcm/stat.h>
#include <soc/dpp/ARAD/arad_stat.h>

typedef enum soc_jer_counters_e {
    soc_jer_counters_tx_control_cells_counter = 0,
    soc_jer_counters_tx_data_cell_counter = 1,
    soc_jer_counters_tx_data_byte_counter = 2,
    soc_jer_counters_rx_crc_errors_data_cells = 3, 
    soc_jer_counters_rx_crc_errors_control_cells_nonbypass = 4, 
    soc_jer_counters_rx_crc_errors_control_cells_bypass = 5,
    soc_jer_counters_rx_fec_correctable_error = 6,
    soc_jer_counters_rx_control_cells_counter = 7,
    soc_jer_counters_rx_data_cell_counter = 8,
    soc_jer_counters_rx_data_byte_counter = 9,
    soc_jer_counters_rx_dropped_retransmitted_control = 10,
    soc_jer_counters_dummy_val_1 = 11,
    soc_jer_counters_dummy_val_2 = 12,
    soc_jer_counters_tx_asyn_fifo_rate = 13,
    soc_jer_counters_rx_asyn_fifo_rate = 14,
    soc_jer_counters_rx_lfec_fec_uncorrrectable_errors = 15,
    soc_jer_counters_rx_llfc_primary_pipe = 16,
    soc_jer_counters_rx_llfc_second_pipe = 17,
    soc_jer_counters_rx_llfc_third_pipe = 18,
    soc_jer_counters_rx_kpcs_errors_counter = 19,
    soc_jer_counters_rx_kpcs_bypass_errors_counter = 20
}soc_jer_counters_t;

soc_error_t soc_jer_stat_nif_init(int unit);

soc_error_t soc_jer_fabric_stat_init(int unit);

soc_error_t soc_jer_mapping_stat_get(int unit,soc_port_t port, uint32 *counters,int *array_size ,bcm_stat_val_t type,int max_array_size);
soc_error_t soc_jer_stat_counter_length_get(int unit, int counter_id, int *length);

#endif /*_SOC_JER_MAC_STAT_H_*/
