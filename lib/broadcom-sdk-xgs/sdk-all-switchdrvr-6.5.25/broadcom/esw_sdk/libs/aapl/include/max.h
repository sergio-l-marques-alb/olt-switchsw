/* AAPL CORE Revision: master
 *
 * Copyright (c) 2014-2021 Avago Technologies. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef AVAGO_MAX_H_
#define AVAGO_MAX_H_

/** Doxygen File Header
 ** @file
 ** @brief   Functions and data structures for MAX access
 **/

#if AAPL_ENABLE_MAX


/* Default timeout for max operations */
EXT int avago_max_default_timeout;
EXT int avago_max_dwell_time;

/* MAX firmware parameters */
typedef enum
{
  AVAGO_MAX_TIMEOUT                     = 0,
  AVAGO_MAX_FREQUENCY                   = 1,
  AVAGO_MAX_TX_PHY_CONFIG_LO            = 2,
  AVAGO_MAX_TX_PHY_CONFIG_HI            = 3,
  AVAGO_MAX_RX_PHY_CONFIG_LO            = 4,
  AVAGO_MAX_RX_PHY_CONFIG_HI            = 5,
  AVAGO_MAX_UNUSED_0                    = 6,
  AVAGO_MAX_UNUSED_1                    = 7,
  AVAGO_MAX_UNUSED_2                    = 8,
  AVAGO_MAX_UNUSED_3                    = 9,
  AVAGO_MAX_WAFER_MODE                  = 10,
  AVAGO_MAX_BIST_DIR                    = 11,
  AVAGO_MAX_BIST_MODE                   = 12,
  AVAGO_MAX_BIST_TOTAL_CYCLES_LO        = 13,
  AVAGO_MAX_BIST_TOTAL_CYCLES_HI        = 14,
  AVAGO_MAX_BIST_INJECT_ERROR           = 15,
  AVAGO_MAX_CONFIG_BIST_USER_SEL_DAT_LO = 16,
  AVAGO_MAX_CONFIG_BIST_USER_SEL_DAT_HI = 17,
  AVAGO_MAX_CONFIG_BIST_USER_SEL_ECC_LO = 18,
  AVAGO_MAX_CONFIG_BIST_USER_SEL_ECC_HI = 19,
  AVAGO_MAX_CONFIG_BIST_USER_INV_DAT_LO = 20,
  AVAGO_MAX_CONFIG_BIST_USER_INV_DAT_HI = 21,
  AVAGO_MAX_CONFIG_BIST_USER_INV_ECC_LO = 22,
  AVAGO_MAX_CONFIG_BIST_USER_INV_ECC_HI = 23,
  AVAGO_MAX_CONFIG_BIST_SEED            = 24,
  AVAGO_MAX_CONFIG_BIST_MODE            = 25,
  AVAGO_MAX_APPLY_REPAIRS               = 26,
  AVAGO_MAX_BIST_FREERUN                = 27,
  AVAGO_MAX_LANE_REPAIR_MODE            = 28,
  AVAGO_MAX_AUTOSET_QC                  = 29,
  AVAGO_MAX_DLL_CONFIG                  = 30,
  AVAGO_MAX_DLL_OFFSET                  = 31,
  AVAGO_MAX_CHECK_SLAVE_LANES           = 32,
  AVAGO_MAX_OVERRIDE_RESET_CHECK        = 33,
  AVAGO_MAX_REPAIR_OFFSET               = 34,
  AVAGO_MAX_QC_OFFSET                   = 35,
  AVAGO_MAX_QC_DELAY                    = 36,
  AVAGO_MAX_SAVE_REPAIRS                = 37,
  AVAGO_MAX_CONFIG_DBI_ENABLE           = 38,
  AVAGO_MAX_CONFIG_EXT_ECC_ENABLE       = 39,
  AVAGO_MAX_CONFIG_DRV_IMP              = 40,
  AVAGO_MAX_DIS_CLK_SYNC                = 41,
  AVAGO_MAX_BIST_DATA_PATTERN           = 42,
  AVAGO_MAX_BIST_IDLE_CYCLES            = 43,
  AVAGO_MAX_BIST_RUN_CYCLES             = 44,
  AVAGO_MAX_BIST_ERROR_MASK_DWORD_LO    = 45,
  AVAGO_MAX_BIST_ERROR_MASK_DWORD_HI    = 46,
  AVAGO_MAX_BIST_ERROR_MASK_ECC         = 47,
  AVAGO_MAX_DIFF_ENHANCE                = 48
} Avago_max_parameter_t;

/* Set a max training parameter value */
EXT int avago_max_set_parameter(Aapl_t *aapl, uint spico_addr, Avago_max_parameter_t param, uint value);

/* Get a max training parameter value */
EXT int avago_max_get_parameter(Aapl_t *aapl, uint spico_addr, Avago_max_parameter_t param);


/* HBM firmware operations that can be run */
typedef enum
{
  AVAGO_MAX_OP_RESET                     = 0,
  AVAGO_MAX_OP_READ_RX_PHY_CONFIG        = 3,
  AVAGO_MAX_OP_READ_TX_PHY_CONFIG        = 4,
  AVAGO_MAX_OP_READ_LANE_REPAIRS         = 5,
  AVAGO_MAX_OP_PROGRAM_PHY_CONFIG_AND_QC = 8,
  AVAGO_MAX_OP_RUN_LANE_REPAIR           = 12,
  AVAGO_MAX_OP_REPORT_NEW_LANE_REPAIRS   = 13,
  AVAGO_MAX_OP_BURN_REPAIRS              = 14,
  AVAGO_MAX_OP_READ_EFUSE_REPAIRS        = 15,
  AVAGO_MAX_OP_RUN_BYPASS_TEST           = 19,
  AVAGO_MAX_OP_BIST_MMT                  = 23,
  AVAGO_MAX_OP_BIST_ILB_TX               = 24,
  AVAGO_MAX_OP_BIST_SLB_TX               = 25,
  AVAGO_MAX_OP_BIST_ILB_RX               = 26,
  AVAGO_MAX_OP_BIST_SLB_RX               = 27,
  AVAGO_MAX_OP_BIST_CUSTOM               = 28,
  AVAGO_MAX_OP_BIST_STOP_N_CHECK         = 29,
  AVAGO_MAX_OP_BIST_START_FREERUN        = 30
} Avago_max_operation_t;

typedef struct
{
  uint operation_status;
  uint global_error_code;
  uint channel_error_code[16];
  uint channel_operation_code[16];
} Avago_max_operation_results_t;



EXT int avago_max_get_interface_from_addr(Aapl_t *aapl, uint sbus_addr);
EXT int avago_max_get_addr_from_interface(Aapl_t *aapl, uint sbus_addr, uint max_interface);
EXT int avago_max_launch_operation( Aapl_t *aapl, uint spico_addr, Avago_max_operation_t operation, Avago_max_operation_results_t *results, int max_timeout);
EXT int avago_max_launch_operation_nowait( Aapl_t *aapl, uint spico_addr, Avago_max_operation_t operation, int max_timeout);
EXT int avago_max_launch_channel_operation( Aapl_t *aapl, uint spico_addr, Avago_max_operation_t operation, Avago_max_operation_results_t *results, uint channel, int max_timeout);
EXT int avago_max_launch_channel_operation_nowait( Aapl_t *aapl, uint spico_addr, Avago_max_operation_t operation, uint channel, int max_timeout);
EXT int avago_max_check_channel_operation( Aapl_t *aapl, uint spico_addr, Avago_max_operation_t operation, Avago_max_operation_results_t *results, uint channel, int max_timeout);
EXT int avago_max_check_operation( Aapl_t *aapl, uint spico_addr, Avago_max_operation_t operation, Avago_max_operation_results_t *results, int max_timeout);
EXT int avago_max_get_operation_results( Aapl_t *aapl, uint spico_addr, Avago_max_operation_results_t *results);
EXT int avago_max_print_operation_results(Aapl_t *aapl, Avago_max_operation_results_t *results, uint verbose);
EXT int avago_max_launch_channel_operation_base(Aapl_t *aapl, uint sbus_addr, Avago_max_operation_t operation, Avago_max_operation_results_t *results, uint channel, int max_timeout, int wait_for_complete);
EXT int avago_max_lane_diagnostics(Aapl_t *aapl, uint sbus_addr);
EXT int avago_max_bist_diagnostics(Aapl_t *aapl, uint sbus_addr);
EXT int avago_max_print_lane_repairs(Aapl_t *aapl, uint sbus_addr);
EXT const char *avago_max_master_slave_str(Aapl_t *aapl, uint sbus_addr);
EXT int avago_max_parameter_diagnostics(Aapl_t *aapl, uint sbus_addr);

EXT int avago_max_bist_bidir_diagnostics(
    Aapl_t *aapl,           /**< [in] Pointer to Aapl_t structure. */
    Avago_addr_t *addr,     /**< [in] SBus slice (broadcast) address. */
    Avago_addr_t *start,    /**< [out] Start of hardware address range. */
    Avago_addr_t *stop,     /**< [out] End of hardware address range. */
    Avago_addr_t *first,    /**< [out] First hardware address. */
    uint flags);             /**< [in] Flags to control iteration. */

EXT BOOL avago_max_is_master(Aapl_t *aapl, uint sbus_addr);

EXT int avago_max_print_spare_results(Aapl_t *aapl, uint sbus_addr, uint skip_zero_results);
EXT unsigned long avago_max_read_tx_phy_config(Aapl_t *aapl, uint sbus_addr, uint channel);
EXT int avago_max_print_phy_config(Aapl_t *aapl, uint sbus_addr, uint channel, uint tx_rx);
EXT unsigned long avago_max_read_rx_phy_config(Aapl_t *aapl, uint sbus_addr, uint channel);
EXT int avago_max_print_rx_bist_results(Aapl_t *aapl, uint sbus_addr, uint channel, uint remote_rx);

#endif

#if AAPL_ENABLE_CHIPLET
EXT BOOL avago_chiplet_bringup(Aapl_t *aapl, int chip, int flags, uint refclk, bigint phy_freq, int words, const int *max_fw);
#endif

#endif
