/*
 * $Id: jer2_jer_stat.h,v 1.2 Broadcom SDK $
 *
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * JER2_JERICHO STAT H
 */

#ifndef _SOC_JER2_STAT_H_
#define _SOC_JER2_STAT_H_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (Jer2) family only!"
#endif

#include <soc/error.h>
#include <soc/dnxc/legacy/error.h>
#include <bcm/stat.h>

typedef enum soc_jer2_counters_e
{
    soc_jer2_counters_tx_control_cells_counter = 0,
    soc_jer2_counters_tx_data_cell_counter = 1,
    soc_jer2_counters_tx_data_byte_counter = 2,
    soc_jer2_counters_rx_crc_errors_data_cells = 3,
    soc_jer2_counters_rx_crc_errors_control_cells_nonbypass = 4,
    soc_jer2_counters_rx_crc_errors_control_cells_bypass = 5,
    soc_jer2_counters_rx_fec_correctable_error = 6,
    soc_jer2_counters_rx_control_cells_counter = 7,
    soc_jer2_counters_rx_data_cell_counter = 8,
    soc_jer2_counters_rx_data_byte_counter = 9,
    soc_jer2_counters_rx_dropped_retransmitted_control = 10,
    soc_jer2_counters_dummy_val_1 = 11,
    soc_jer2_counters_dummy_val_2 = 12,
    soc_jer2_counters_tx_asyn_fifo_rate = 13,
    soc_jer2_counters_rx_asyn_fifo_rate = 14,
    soc_jer2_counters_rx_lfec_fec_uncorrrectable_errors = 15,
    soc_jer2_counters_rx_llfc_primary_pipe = 16,
    soc_jer2_counters_rx_llfc_second_pipe = 17,
    soc_jer2_counters_rx_llfc_third_pipe = 18,
    soc_jer2_counters_rx_kpcs_errors_counter = 19,
    soc_jer2_counters_rx_kpcs_bypass_errors_counter = 20,
    soc_jer2_counters_rx_rs_fec_bit_error_counter = 21,
    soc_jer2_counters_rx_rs_fec_symbol_error_rate_counter = 22,
    soc_jer2_counters_ilkn_rx_pkt_counter = 23,
    soc_jer2_counters_ilkn_tx_pkt_counter = 24,
    soc_jer2_counters_ilkn_rx_byte_counter = 25,
    soc_jer2_counters_ilkn_tx_byte_counter = 26,
    soc_jer2_counters_ilkn_rx_err_pkt_counter = 27,
    soc_jer2_counters_ilkn_tx_err_pkt_counter = 28,
    soc_jer2_counters_nif_rx_fec_correctable_error = 29,
    soc_jer2_counters_nif_rx_fec_uncorrrectable_errors = 30,
    soc_jer2_counters_nif_rx_bip_error = 31,
    soc_jer2_counters_rx_eth_stats_drop_events_sch_low = 32,
    soc_jer2_counters_rx_eth_stats_drop_events_sch_high = 33,
    soc_jer2_counters_rx_eth_stats_drop_events_sch_tdm = 34,
    soc_jer2_counters_cdmib_r64 = 35,
    soc_jer2_counters_cdmib_r127 = 36,
    soc_jer2_counters_cdmib_r255 = 37,
    soc_jer2_counters_cdmib_r511 = 38,
    soc_jer2_counters_cdmib_r1023 = 39,
    soc_jer2_counters_cdmib_r1518 = 40,
    soc_jer2_counters_cdmib_rmgv = 41,
    soc_jer2_counters_cdmib_r2047 = 42,
    soc_jer2_counters_cdmib_r4095 = 43,
    soc_jer2_counters_cdmib_r9216 = 44,
    soc_jer2_counters_cdmib_r16383 = 45,
    soc_jer2_counters_cdmib_rbca = 46,
    soc_jer2_counters_cdmib_rprog0 = 47,
    soc_jer2_counters_cdmib_rprog1 = 48,
    soc_jer2_counters_cdmib_rprog2 = 49,
    soc_jer2_counters_cdmib_rprog3 = 50,
    soc_jer2_counters_cdmib_rpkt = 51,
    soc_jer2_counters_cdmib_rpok = 52,
    soc_jer2_counters_cdmib_ruca = 53,
    soc_jer2_counters_cdmib_reserved0 = 54,
    soc_jer2_counters_cdmib_rmca = 55,
    soc_jer2_counters_cdmib_rxpf = 56,
    soc_jer2_counters_cdmib_rxpp = 57,
    soc_jer2_counters_cdmib_rxcf = 58,
    soc_jer2_counters_cdmib_rfcs = 59,
    soc_jer2_counters_cdmib_rerpkt = 60,
    soc_jer2_counters_cdmib_rflr = 61,
    soc_jer2_counters_cdmib_rjbr = 62,
    soc_jer2_counters_cdmib_rmtue = 63,
    soc_jer2_counters_cdmib_rovr = 64,
    soc_jer2_counters_cdmib_rvln = 65,
    soc_jer2_counters_cdmib_rdvln = 66,
    soc_jer2_counters_cdmib_rxuo = 67,
    soc_jer2_counters_cdmib_rxuda = 68,
    soc_jer2_counters_cdmib_rxwsa = 69,
    soc_jer2_counters_cdmib_rprm = 70,
    soc_jer2_counters_cdmib_rpfc0 = 71,
    soc_jer2_counters_cdmib_rpfcoff0 = 72,
    soc_jer2_counters_cdmib_rpfc1 = 73,
    soc_jer2_counters_cdmib_rpfcoff1 = 74,
    soc_jer2_counters_cdmib_rpfc2 = 75,
    soc_jer2_counters_cdmib_rpfcoff2 = 76,
    soc_jer2_counters_cdmib_rpfc3 = 77,
    soc_jer2_counters_cdmib_rpfcoff3 = 78,
    soc_jer2_counters_cdmib_rpfc4 = 79,
    soc_jer2_counters_cdmib_rpfcoff4 = 80,
    soc_jer2_counters_cdmib_rpfc5 = 81,
    soc_jer2_counters_cdmib_rpfcoff5 = 82,
    soc_jer2_counters_cdmib_rpfc6 = 83,
    soc_jer2_counters_cdmib_rpfcoff6 = 84,
    soc_jer2_counters_cdmib_rpfc7 = 85,
    soc_jer2_counters_cdmib_rpfcoff7 = 86,
    soc_jer2_counters_cdmib_rund = 87,
    soc_jer2_counters_cdmib_rfrg = 88,
    soc_jer2_counters_cdmib_rrpkt = 89,
    soc_jer2_counters_cdmib_reserved1 = 90,
    soc_jer2_counters_cdmib_t64 = 91,
    soc_jer2_counters_cdmib_t127 = 92,
    soc_jer2_counters_cdmib_t255 = 93,
    soc_jer2_counters_cdmib_t511 = 94,
    soc_jer2_counters_cdmib_t1023 = 95,
    soc_jer2_counters_cdmib_t1518 = 96,
    soc_jer2_counters_cdmib_tmgv = 97,
    soc_jer2_counters_cdmib_t2047 = 98,
    soc_jer2_counters_cdmib_t4095 = 99,
    soc_jer2_counters_cdmib_t9216 = 100,
    soc_jer2_counters_cdmib_t16383 = 101,
    soc_jer2_counters_cdmib_tbca = 102,
    soc_jer2_counters_cdmib_tpfc0 = 103,
    soc_jer2_counters_cdmib_tpfcoff0 = 104,
    soc_jer2_counters_cdmib_tpfc1 = 105,
    soc_jer2_counters_cdmib_tpfcoff1 = 106,
    soc_jer2_counters_cdmib_tpfc2 = 107,
    soc_jer2_counters_cdmib_tpfcoff2 = 108,
    soc_jer2_counters_cdmib_tpfc3 = 109,
    soc_jer2_counters_cdmib_tpfcoff3 = 110,
    soc_jer2_counters_cdmib_tpfc4 = 111,
    soc_jer2_counters_cdmib_tpfcoff4 = 112,
    soc_jer2_counters_cdmib_tpfc5 = 113,
    soc_jer2_counters_cdmib_tpfcoff5 = 114,
    soc_jer2_counters_cdmib_tpfc6 = 115,
    soc_jer2_counters_cdmib_tpfcoff6 = 116,
    soc_jer2_counters_cdmib_tpfc7 = 117,
    soc_jer2_counters_cdmib_tpfcoff7 = 118,
    soc_jer2_counters_cdmib_tpkt = 119,
    soc_jer2_counters_cdmib_tpok = 120,
    soc_jer2_counters_cdmib_tuca = 121,
    soc_jer2_counters_cdmib_tufl = 122,
    soc_jer2_counters_cdmib_tmca = 123,
    soc_jer2_counters_cdmib_txpf = 124,
    soc_jer2_counters_cdmib_txpp = 125,
    soc_jer2_counters_cdmib_txcf = 126,
    soc_jer2_counters_cdmib_tfcs = 127,
    soc_jer2_counters_cdmib_terr = 128,
    soc_jer2_counters_cdmib_tovr = 129,
    soc_jer2_counters_cdmib_tjbr = 130,
    soc_jer2_counters_cdmib_trpkt = 131,
    soc_jer2_counters_cdmib_tfrg = 132,
    soc_jer2_counters_cdmib_tvln = 133,
    soc_jer2_counters_cdmib_tdvln = 134,
    soc_jer2_counters_cdmib_rbyt = 135,
    soc_jer2_counters_cdmib_rrbyt = 136,
    soc_jer2_counters_cdmib_tbyt = 137
} soc_jer2_counters_t;

shr_error_e soc_jer2_mapping_stat_get(
    int unit,
    soc_port_t port,
    uint32 *counters,
    int *array_size,
    uint32 *sub_counters,
    int *sub_array_size,
    bcm_stat_val_t type,
    int max_array_size);
shr_error_e soc_jer2_stat_counter_length_get(
    int unit,
    int counter_id,
    int *length);

int soc_jer2_ilkn_controlled_counter_clear_by_port(
    int unit,
    int port);

shr_error_e soc_jer2_stat_controlled_counter_enable_get(
    int unit,
    soc_port_t port,
    int index,
    int *enable,
    int *printable);

#endif /*_SOC_JER2_JER_MAC_STAT_H_*/
