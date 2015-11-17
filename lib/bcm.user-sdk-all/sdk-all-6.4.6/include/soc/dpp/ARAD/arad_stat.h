/*
 * $Id: arad_stat.h,v 1.2 Broadcom SDK $
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
 * FE1600 STAT H
 */
 
#ifndef _SOC_ARAD_MAC_STAT_H_
#define _SOC_ARAD_MAC_STAT_H_

#include <bcm/stat.h>
#include <soc/error.h>
#include <soc/dpp/ARAD/arad_api_nif.h>

extern soc_controlled_counter_t soc_arad_controlled_counter[];

#define SOC_ARAD_MAC_COUNTER_FIRST                                  ARAD_NIF_NOF_COUNTER_TYPES
#define SOC_ARAD_MAC_COUNTERS_TX_CONTROL_CELLS_COUNTER              (SOC_ARAD_MAC_COUNTER_FIRST + 0)
#define SOC_ARAD_MAC_COUNTERS_TX_DATA_CELL_COUNTER                  (SOC_ARAD_MAC_COUNTER_FIRST + 1)
#define SOC_ARAD_MAC_COUNTERS_TX_DATA_BYTE_COUNTER                  (SOC_ARAD_MAC_COUNTER_FIRST + 2)
#define SOC_ARAD_MAC_COUNTERS_RX_CRC_ERRORS_COUNTER                 (SOC_ARAD_MAC_COUNTER_FIRST + 3) 
#define SOC_ARAD_MAC_COUNTERS_RX_LFEC_FEC_CORRECTABLE_ERROR         (SOC_ARAD_MAC_COUNTER_FIRST + 4) 
#define SOC_ARAD_MAC_COUNTERS_RX_BEC_CRC_ERROR                      (SOC_ARAD_MAC_COUNTER_FIRST + 4)
#define SOC_ARAD_MAC_COUNTERS_RX_8B_10B_DISPARITY_ERRORS            (SOC_ARAD_MAC_COUNTER_FIRST + 4)
#define SOC_ARAD_MAC_COUNTERS_RX_CONTROL_CELLS_COUNTER              (SOC_ARAD_MAC_COUNTER_FIRST + 5)
#define SOC_ARAD_MAC_COUNTERS_RX_DATA_CELL_COUNTER                  (SOC_ARAD_MAC_COUNTER_FIRST + 6)
#define SOC_ARAD_MAC_COUNTERS_RX_DATA_BYTE_COUNTER                  (SOC_ARAD_MAC_COUNTER_FIRST + 7)
#define SOC_ARAD_MAC_COUNTERS_RX_DROPPED_RETRANSMITTED_CONTROL      (SOC_ARAD_MAC_COUNTER_FIRST + 8)
#define SOC_ARAD_MAC_COUNTERS_TX_BEC_RETRANSMIT                     (SOC_ARAD_MAC_COUNTER_FIRST + 9)
#define SOC_ARAD_MAC_COUNTERS_RX_BEC_RETRANSMIT                     (SOC_ARAD_MAC_COUNTER_FIRST + 10)
#define SOC_ARAD_MAC_COUNTERS_TX_ASYN_FIFO_RATE_AT_UNITS_OF_40_BITS (SOC_ARAD_MAC_COUNTER_FIRST + 11)
#define SOC_ARAD_MAC_COUNTERS_RX_ASYN_FIFO_RATE_AT_UNITS_OF_40_BITS (SOC_ARAD_MAC_COUNTER_FIRST + 12)
#define SOC_ARAD_MAC_COUNTERS_RX_LFEC_FEC_UNCORRECTABLE_ERRORS      (SOC_ARAD_MAC_COUNTER_FIRST + 13)
#define SOC_ARAD_MAC_COUNTERS_RX_BEC_RX_FAULT                       (SOC_ARAD_MAC_COUNTER_FIRST + 13)
#define SOC_ARAD_MAC_COUNTERS_RX_8B_10B_CODE_ERRORS                 (SOC_ARAD_MAC_COUNTER_FIRST + 13)
#define SOC_ARAD_MAC_COUNTERS_RX_LLFC_PRIMARY                       (SOC_ARAD_MAC_COUNTER_FIRST + 14)
#define SOC_ARAD_MAC_COUNTERS_RX_LLFC_SECONDARY                     (SOC_ARAD_MAC_COUNTER_FIRST + 15)

#define SOC_ARAD_NOF_MAC_COUNTERS                                   16
#define SOC_ARAD_NOF_COUNTERS                                       (ARAD_NIF_NOF_COUNTER_TYPES+SOC_ARAD_NOF_MAC_COUNTERS)

/*
 * ILKN counters mode.
 */
typedef enum soc_arad_stat_ilkn_counters_mode_e {
    soc_arad_stat_ilkn_counters_mode_physical,
    soc_arad_stat_ilkn_counters_mode_packets_per_channel
} soc_arad_stat_ilkn_counters_mode_t;

#define SOC_ARAD_NIF_ILKN_COUNTER_PER_CHANNEL_CANNEL_SUPPORTED_MAX  (112)

#define SOC_ARAD_STAT_COUNTER_MODE_PACKETS_PER_CHANNEL(unit, port)                                                                  \
                (SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), port) &&                                                                        \
                SOC_DPP_CONFIG(unit)->arad->init.ports.ilkn_counters_mode == soc_arad_stat_ilkn_counters_mode_packets_per_channel)
#define SOC_ARAD_STAT_COUNTER_MODE_PHISYCAL(unit, port)                                                                             \
                (!SOC_ARAD_STAT_COUNTER_MODE_PACKETS_PER_CHANNEL(unit, port))

soc_error_t soc_arad_fabric_stat_init(int unit);
soc_error_t soc_arad_stat_nif_init(int unit);
soc_error_t soc_arad_mapping_stat_get(int unit, soc_port_t port, uint32 *cnt_type, int *num_cntrs_p, bcm_stat_val_t type, int num_cntrs_in);
soc_error_t soc_arad_stat_clear_on_read_set(int unit, int enable);
soc_error_t soc_arad_stat_controlled_counter_enable_get(int unit, soc_port_t port, int index, int *enable);
int soc_arad_mac_controlled_counter_get(int unit, int counter_id, int port, uint64* val);



#endif /*_SOC_ARAD_MAC_STAT_H_*/
