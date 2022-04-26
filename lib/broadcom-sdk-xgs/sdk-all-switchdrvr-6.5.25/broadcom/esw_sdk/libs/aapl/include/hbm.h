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

#ifndef AVAGO_HBM_H_
#define AVAGO_HBM_H_

/** Doxygen File Header
 ** @file
 ** @brief   Functions and data structures for HBM access
 **/

#if AAPL_ENABLE_HBM

/* Default timeout for HBM operations */
EXT int avago_hbm_default_timeout;
EXT int avago_hbm_dwell_time;

/* HBM firmware parameters */
typedef enum
{
  AVAGO_HBM_INTERFACE_REQUEST               = 0,
  AVAGO_HBM_MAX_TIMEOUT                     = 1,
  AVAGO_HBM_TINIT1_CYCLES                   = 2,
  AVAGO_HBM_TINIT2_CYCLES                   = 3,
  AVAGO_HBM_TINIT3_CYCLES                   = 4,
  AVAGO_HBM_TINIT4_CYCLES                   = 5,
  AVAGO_HBM_TINIT5_CYCLES                   = 6,
  AVAGO_HBM_RW_LATENCY_OFFSET               = 7,
  AVAGO_HBM_LATENCY_ODD_N_EVEN              = 8,
  AVAGO_HBM_SAVE_RESTORE_CONFIG             = 9,
  AVAGO_HBM_MODE_REGISTER0                  = 10,
  AVAGO_HBM_MODE_REGISTER1                  = 11,
  AVAGO_HBM_MODE_REGISTER2                  = 12,
  AVAGO_HBM_MODE_REGISTER3                  = 13,
  AVAGO_HBM_MODE_REGISTER4                  = 14,
  AVAGO_HBM_MODE_REGISTER5                  = 15,
  AVAGO_HBM_MODE_REGISTER6                  = 16,
  AVAGO_HBM_MODE_REGISTER7                  = 17,
  AVAGO_HBM_MODE_REGISTER8                  = 18,
  AVAGO_HBM_PHY_CONFIG0                     = 19,
  AVAGO_HBM_PHY_CONFIG1                     = 20,
  AVAGO_HBM_PHY_CONFIG2                     = 21,
  AVAGO_HBM_PHY_CONFIG3                     = 22,
  AVAGO_HBM_PHY_CONFIG4                     = 23,
  AVAGO_HBM_PHY_CONFIG5                     = 24,
  AVAGO_HBM_PHY_CONFIG6                     = 25,
  AVAGO_HBM_LBP_DRV_IMP                     = 26,
  AVAGO_HBM_DELAY_CONFIG_DLL                = 27,
  AVAGO_HBM_IGNORE_PHYUPD_HANDSHAKE         = 28,
  AVAGO_HBM_TUPDMRS                         = 29,
  AVAGO_HBM_T_RDLAT_OFFSET                  = 30,
  AVAGO_HBM_MBIST_REPAIR_MODE               = 31,
  AVAGO_HBM_MBIST_PATTERN                   = 32,
  AVAGO_HBM_MIST_HARD_REPAIR_CYCLES         = 33,
  AVAGO_HBM_HARD_LANE_REPAIR_CYCLES         = 34,
  AVAGO_HBM_POWER_ON_LANE_REPAIR_MODE       = 35,
  AVAGO_HBM_MBIST_BANK_ADDRESS_END          = 36,
  AVAGO_HBM_MBIST_ROW_ADDRESS_END           = 37,
  AVAGO_HBM_MBIST_COLUMN_ADDRESS_END        = 38,
  AVAGO_HBM_FREQ                            = 39,
  AVAGO_HBM_DIV_MODE                        = 40,
  AVAGO_HBM_CKE_EXIT_STATE                  = 41,
  AVAGO_HBM_TEST_MODE_REGISTER0             = 42,
  AVAGO_HBM_TEST_MODE_REGISTER1             = 43,
  AVAGO_HBM_TEST_MODE_REGISTER2             = 44,
  AVAGO_HBM_TEST_MODE_REGISTER3             = 45,
  AVAGO_HBM_TEST_MODE_REGISTER4             = 46,
  AVAGO_HBM_TEST_MODE_REGISTER5             = 47,
  AVAGO_HBM_TEST_MODE_REGISTER6             = 48,
  AVAGO_HBM_TEST_MODE_REGISTER7             = 49,
  AVAGO_HBM_TEST_MODE_REGISTER8             = 50,
  AVAGO_HBM_CTC_RUN_CYCLES                  = 51,
  AVAGO_HBM_CTC_CHANNEL_IGNORE              = 52,
  AVAGO_HBM_CTC_INITIAL_ADDRESS_LO          = 53,
  AVAGO_HBM_CTC_INITIAL_ADDRESS_HI          = 54,
  AVAGO_HBM_CTC_MAX_ADDRESS_LO              = 55,
  AVAGO_HBM_CTC_MAX_ADDRESS_HI              = 56,
  AVAGO_HBM_TEST_T_RDLAT_OFFSET             = 57,
  AVAGO_HBM_MODE_REGISTER15                 = 58,
  AVAGO_HBM_TEST_MODE_REGISTER15            = 59,
  AVAGO_HBM_BYPASS_TESTMODE_RESET           = 60,
  AVAGO_HBM_DISABLE_ADDR_LANE_REPAIR        = 61,
  AVAGO_HBM_CTC_PATTERN_TYPE                = 62,
  AVAGO_HBM_BYPASS_REPAIR_ON_RESET          = 63,
  AVAGO_HBM_STACK_HEIGHT                    = 64,
  AVAGO_HBM_MANUFACTURER_ID                 = 65,
  AVAGO_HBM_DENSITY                         = 66,
  AVAGO_HBM_MANUALLY_CONFIGURE_ID           = 67,
  AVAGO_HBM_PARITY_LATENCY                  = 68,
  AVAGO_HBM_TEST_PARITY_LATENCY             = 69,
  AVAGO_HBM_DFI_T_RDDATA_EN                 = 70,
  AVAGO_HBM_MANUALLY_CONFIGURE_NWL          = 71,
  AVAGO_HBM_CTC_PSEUDO_CHANNEL              = 72,
  AVAGO_MMT_T_RDLAT_OFFSET                  = 87,
  AVAGO_HBM_SAVE_SOFT_LANE_REPAIRS          = 105,
  AVAGO_HBM_MMT_READ_REPEAT                 = 106,
  AVAGO_HBM_SPICO_FREQ                      = 110,
  AVAGO_HBM_RL_OVERRIDE                     = 116,
  AVAGO_HBM_DBI                             = 117,
  AVAGO_HBM_MMT_RDDATA_EN                   = 121,
  AVAGO_HBM_MMT_CONFIGURATION               = AVAGO_HBM_CTC_PATTERN_TYPE,

  AVAGO_HBM2E_INTERFACE_REQUEST             = 0,
  AVAGO_HBM2E_MAX_TIMEOUT                   = 1,
  AVAGO_HBM2E_CLK_F1_FREQ                   = 2,
  AVAGO_HBM2E_POLY_SELECT                   = 3,
  AVAGO_HBM2E_CLK_1500_DIVIDER              = 4,
  AVAGO_HBM2E_BYPASS_TESTMODE_RESET         = 5,
  AVAGO_HBM2E_BYPASS_REPAIR_ON_RESET        = 6,
  AVAGO_HBM2E_SAVE_SOFT_LANE_REPAIRS        = 7,
  AVAGO_HBM2E_TINIT1_CYCLES                 = 8,
  AVAGO_HBM2E_TINIT2_CYCLES                 = 9,
  AVAGO_HBM2E_TINIT3_CYCLES                 = 10,
  AVAGO_HBM2E_TINIT4_CYCLES                 = 11,
  AVAGO_HBM2E_TINIT5_CYCLES                 = 12,
  AVAGO_HBM2E_PHY_CONFIG_T_RDLAT_OFFSET     = 13,
  AVAGO_HBM2E_PHY_CONFIG_QC_DISABLE         = 14,
  AVAGO_HBM2E_PHY_CONFIG_QC_DELAY           = 15,
  AVAGO_HBM2E_PHY_CONFIG_QC_RD_OFFSET       = 16,
  AVAGO_HBM2E_PHY_CONFIG_QC_WR_OFFSET       = 17,
  AVAGO_HBM2E_PHY_CONFIG_QC_CK_OFFSET       = 18,
  AVAGO_HBM2E_PHY_CONFIG_DRV_IMP            = 19,
  AVAGO_HBM2E_PHY_CONFIG_DIFF_ENHANCE       = 20,
  AVAGO_HBM2E_PHY_CONFIG_BPC_CONFIG         = 21,
  AVAGO_HBM2E_PHY_CONFIG_PAR_LATENCY        = 22,
  AVAGO_HBM2E_PHY_CONFIG_PAR_ENABLE         = 23,
  AVAGO_HBM2E_PHY_CONFIG_DFI_FIFO_CONFIG    = 24,
  AVAGO_HBM2E_MR0                           = 25,
  AVAGO_HBM2E_MR1                           = 26,
  AVAGO_HBM2E_MR2                           = 27,
  AVAGO_HBM2E_MR3                           = 28,
  AVAGO_HBM2E_MR4                           = 29,
  AVAGO_HBM2E_MR5                           = 30,
  AVAGO_HBM2E_MR6                           = 31,
  AVAGO_HBM2E_MR7                           = 32,
  AVAGO_HBM2E_MR8                           = 33,
  AVAGO_HBM2E_MR15                          = 34,
  AVAGO_HBM2E_TEST_MR0                      = 35,
  AVAGO_HBM2E_TEST_MR1                      = 36,
  AVAGO_HBM2E_TEST_MR2                      = 37,
  AVAGO_HBM2E_TEST_MR3                      = 38,
  AVAGO_HBM2E_TEST_MR4                      = 39,
  AVAGO_HBM2E_TEST_MR5                      = 40,
  AVAGO_HBM2E_TEST_MR6                      = 41,
  AVAGO_HBM2E_TEST_MR7                      = 42,
  AVAGO_HBM2E_TEST_MR8                      = 43,
  AVAGO_HBM2E_TEST_MR15                     = 44,
  AVAGO_HBM2E_QC_AUTOSET                    = 45,
  AVAGO_HBM2E_QC_DLL_CONFIG                 = 46,
  AVAGO_HBM2E_QC_DLL_OFFSET                 = 47,
  AVAGO_HBM2E_DEVICE_ID_OVERRIDE            = 48,
  AVAGO_HBM2E_DEVICE_ID_MODEL_NUMBER        = 49,
  AVAGO_HBM2E_DEVICE_ID_STACK_HEIGHT        = 50,
  AVAGO_HBM2E_DEVICE_ID_DATE_CODE           = 51,
  AVAGO_HBM2E_DEVICE_ID_MANUFACTURER_ID     = 52,
  AVAGO_HBM2E_DEVICE_ID_DENSITY             = 53,
  AVAGO_HBM2E_BIST_CYCLE_COUNT              = 54,
  AVAGO_HBM2E_BIST_T_RDDATA_EN_OFFSET       = 55,
  AVAGO_HBM2E_BIST_RL                       = 56,
  AVAGO_HBM2E_BIST_WL                       = 57,
  AVAGO_HBM2E_BIST_SLB_MISR_TRIG_CONFIG     = 58,
  AVAGO_HBM2E_BIST_SLB_COMP_TRIG_CONFIG     = 59,
  AVAGO_HBM2E_BIST_RST_RDDATA_POINT_P0      = 60,
  AVAGO_HBM2E_LANE_REPAIR_MODE              = 61,
  AVAGO_HBM2E_LANE_BURN_WAIT_CYCLES         = 62,
  AVAGO_HBM2E_MBIST_BURN_WAIT_CYCLES        = 63,
  AVAGO_HBM2E_MBIST_REPAIR_MODE             = 64,
  AVAGO_HBM2E_MBIST_PATTERN                 = 65,
  AVAGO_HBM2E_TMRS                          = 66,
  AVAGO_HBM2E_MMT_CONFIGURATION             = 67,
  AVAGO_HBM2E_MMT_CONFIG_DWORD0             = 68,
  AVAGO_HBM2E_MMT_CONFIG_DWORD1             = 69,
  AVAGO_HBM2E_MMT_CONFIG_DWORD2             = 70,
  AVAGO_HBM2E_MMT_CONFIG_DWORD3             = 71,
  AVAGO_HBM2E_MMT_CONFIG_DWORD4             = 72,
  AVAGO_HBM2E_MMT_CONFIG_DWORD5             = 73,
  AVAGO_HBM2E_MMT_CONFIG_DWORD6             = 74,
  AVAGO_HBM2E_MMT_CONFIG_AWORD0             = 75,
  AVAGO_HBM2E_MMT_CONFIG_AWORD1             = 76,
  AVAGO_HBM2E_MMT_CONFIG_AWORD2             = 77,
  AVAGO_HBM2E_MMT_CONFIG_AWORD3             = 78,
  AVAGO_HBM2E_MMT_CONFIG_AWORD4             = 79,
  AVAGO_HBM2E_MMT_CONFIG_AWORD5             = 80,
  AVAGO_HBM2E_MMT_CONFIG_AWORD6             = 81,
  AVAGO_HBM2E_MMT_DWORD_MISR0               = 82,
  AVAGO_HBM2E_MMT_DWORD_MISR1               = 83,
  AVAGO_HBM2E_MMT_DWORD_MISR2               = 84,
  AVAGO_HBM2E_MMT_DWORD_MISR3               = 85,
  AVAGO_HBM2E_MMT_DWORD_MISR4               = 86,
  AVAGO_HBM2E_MMT_AWORD_MISR0               = 87,
  AVAGO_HBM2E_MMT_AWORD_MISR1               = 88,
  AVAGO_HBM2E_MMT_AWORD_MISR2               = 89,
  AVAGO_HBM2E_MMT_LFSR_COMPARE_STICKY0      = 90,
  AVAGO_HBM2E_MMT_MODE_REGISTER0            = 91,
  AVAGO_HBM2E_MMT_MODE_REGISTER1            = 92,
  AVAGO_HBM2E_MMT_MODE_REGISTER2            = 93,
  AVAGO_HBM2E_MMT_MODE_REGISTER3            = 94,
  AVAGO_HBM2E_MMT_MODE_REGISTER4            = 95,
  AVAGO_HBM2E_MMT_MODE_REGISTER5            = 96,
  AVAGO_HBM2E_MMT_MODE_REGISTER6            = 97,
  AVAGO_HBM2E_MMT_MODE_REGISTER7            = 98,
  AVAGO_HBM2E_MMT_MODE_REGISTER8            = 99,
  AVAGO_HBM2E_MMT_MODE_REGISTER15           = 100,
  AVAGO_HBM2E_MMT_CONTINUOUS_MODE           = 101,
  AVAGO_HBM2E_MMT_DISABLE_INIT              = 102,
  AVAGO_HBM2E_MMT_DISABLE_EXIT              = 103,
  AVAGO_HBM2E_MMT_TCR                       = 104,
  AVAGO_HBM2E_MMT_RL                        = 105,
  AVAGO_HBM2E_SPICO_FREQ                    = 106,
  AVAGO_HBM2E_MBIST_0                       = 107,
  AVAGO_HBM2E_MBIST_1                       = 108,
  AVAGO_HBM2E_MBIST_2                       = 109,
  AVAGO_HBM2E_MBIST_3                       = 110,
  AVAGO_HBM2E_MBIST_4                       = 111,
  AVAGO_HBM2E_MBIST_SIMULATION_FLAG         = 112,
  AVAGO_HBM2E_CKE_EXIT_STATE                = 113,
  AVAGO_HBM2E_TUPDMRS                       = 114,
  AVAGO_HBM2E_MODE1_LANE_REPAIR_LIMIT       = 115,
  AVAGO_HBM2E_TOTAL_LANE_REPAIR_LIMIT       = 116,
  AVAGO_HBM2E_MMT_LFSR_COMPARE_STICKY1      = 117,
  AVAGO_HBM2E_MMT_SUPPLY_CONDITIONING       = 118,
  AVAGO_HBM2E_MMT_T_RDLAT_OFFSET            = 119,
  AVAGO_HBM2E_MMT_RDDATA_EN                 = 120,
  AVAGO_HBM2E_MMT_DBI_ENABLE                = 121,
  AVAGO_HBM2E_DLL_SWEEP_CONFIG              = 122,
  AVAGO_HBM2E_MMT_INJ_ERROR                 = 123,
  AVAGO_HBM2E_MMT_CHANNEL_MASK              = 124,
  AVAGO_HBM2E_MMT_ROW_ADDR_RANGE            = 125,
  AVAGO_HBM2E_MMT_BURST_SPACE               = 126,
  AVAGO_HBM2E_MMT_SYNC_CHANNELS             = 127,
  AVAGO_HBM2E_QC_ADJUST                     = 128,
  AVAGO_HBM2E_MMT_READ_REPEAT               = 129,
  AVAGO_HBM2E_MMT_BURST_LENGTH              = 130,
  AVAGO_HBM2E_TMRS_CASE                     = 131,
  AVAGO_HBM2E_READ_ALL_TEMP_SENSORS         = 132,
  AVAGO_HBM2E_TEMP_SEL_WAIT_CYCLES          = 133,
  AVAGO_HBM2E_QC_RD_OFFSET_ADJUST           = 134,
  AVAGO_HBM2E_QC_WR_OFFSET_ADJUST           = 135,
  AVAGO_HBM2E_QC_CK_OFFSET_ADJUST           = 136
} Avago_hbm_parameter_t;

/* Set a hbm training parameter value */
EXT int avago_hbm_set_parameter(Aapl_t *aapl, uint spico_addr, Avago_hbm_parameter_t param, uint value);

/* Get a hbm training parameter value */
EXT int avago_hbm_get_parameter(Aapl_t *aapl, uint spico_addr, Avago_hbm_parameter_t param);


/* HBM firmware operations that can be run */
typedef enum
{
  AVAGO_HBM_OP_RESET                 = 0,
  AVAGO_HBM_OP_RESET_PHY             = 1,
  AVAGO_HBM_OP_RESET_HBM             = 2,
  AVAGO_HBM_OP_POWER_ON_FLOW         = 5,
  AVAGO_HBM_OP_CONNECTIVITY_CHECK    = 6,
  AVAGO_HBM_OP_BYPASS_TEST           = 7,
  AVAGO_HBM_OP_READ_DEVICE_ID        = 8,
  AVAGO_HBM_OP_AWORD_TEST            = 9,
  AVAGO_HBM_OP_AERR_TEST             = 10,
  AVAGO_HBM_OP_DWORD_TEST            = 11,
  AVAGO_HBM_OP_DERR_TEST             = 12,
  AVAGO_HBM_OP_LANE_REPAIR           = 13,
  AVAGO_HBM_OP_AWORD_ILB             = 14,
  AVAGO_HBM_OP_DWORD_ILB             = 15,
  AVAGO_HBM_OP_READ_TEMPERATURE      = 16,
  AVAGO_HBM_OP_BURN_HARD_REPAIRS     = 17,
  AVAGO_HBM_OP_COPY_HARD_REPAIRS     = 18,
  AVAGO_HBM_OP_MBIST                 = 19,
  AVAGO_HBM_OP_RUN_SAMSUNG_MBIST     = 19,
  AVAGO_HBM_OP_RUN_SKH_MBIST         = 20,
  AVAGO_HBM_OP_RESET_MODE_REGISTERS  = 21,
  AVAGO_HBM_OP_RESET_PHY_CONFIG      = 22,
  AVAGO_HBM_OP_AWORD_SLB             = 23,
  AVAGO_HBM_OP_DWORD_SLB             = 24,
  AVAGO_HBM_OP_SAMSUNG_CHIPPING_TEST = 25,
  AVAGO_HBM_OP_AERR_ILB              = 27,
  AVAGO_HBM_OP_AERR_SLB              = 28,
  AVAGO_HBM_OP_DERR_ILB              = 29,
  AVAGO_HBM_OP_DERR_SLB              = 30,
  AVAGO_HBM_OP_INITIALIZE_NWL_MCS    = 31,
  AVAGO_HBM_OP_RUN_CTCS              = 32,
  AVAGO_HBM_OP_VERIFY_LANE_ERRORS    = 33,
  AVAGO_HBM_OP_START_CTC             = 34,
  AVAGO_HBM_OP_STOP_CTC              = 35,
  AVAGO_HBM_OP_TMRS                  = 36,
  AVAGO_HBM_OP_RELEASE_CTC_CONTROL   = 37,
  AVAGO_HBM_OP_CATTRIP               = 38,
  AVAGO_HBM_OP_TOGGLE_NWL_CKE        = 39,
  AVAGO_HBM_OP_CTC_BANDWIDTH         = 40,
  AVAGO_HBM_OP_RUN_CELL_REPAIR       = 41,
  AVAGO_HBM_OP_FUSE_SCAN             = 42,
  AVAGO_HBM_OP_EXTEST_SLB            = 43,
  AVAGO_HBM_OP_DWORD_WRITE           = 53,
  AVAGO_HBM_OP_DWORD_READ            = 54,
  AVAGO_HBM_OP_RUN_MMT               = AVAGO_HBM_OP_RUN_CTCS,
  AVAGO_HBM_OP_START_MMT             = AVAGO_HBM_OP_START_CTC,
  AVAGO_HBM_OP_STOP_MMT              = AVAGO_HBM_OP_STOP_CTC,
  AVAGO_HBM2E_OP_RESET                        = 0,
  AVAGO_HBM2E_OP_RESET_FIFO_POINTERS          = 1,
  AVAGO_HBM2E_OP_READ_DEVICE_ID               = 2,
  AVAGO_HBM2E_OP_READ_TEMPERATURE             = 3,
  AVAGO_HBM2E_OP_READ_MRS                     = 4,
  AVAGO_HBM2E_OP_READ_PHY_CONFIG              = 5,
  AVAGO_HBM2E_OP_PROGRAM_MRS                  = 6,
  AVAGO_HBM2E_OP_PROGRAM_PHY_CONFIG_AND_QC    = 7,
  AVAGO_HBM2E_OP_PROGRAM_TMRS                 = 8,
  AVAGO_HBM2E_OP_LANE_REPAIR                  = 9,
  AVAGO_HBM2E_OP_READ_LANE_REPAIRS            = 10,
  AVAGO_HBM2E_OP_READ_NEW_LANE_REPAIRS        = 11,
  AVAGO_HBM2E_OP_READ_LANE_REPAIRS_BY_MODE    = 12,
  AVAGO_HBM2E_OP_BURN_LANE_REPAIRS            = 13,
  AVAGO_HBM2E_OP_MBIST                        = 14,
  AVAGO_HBM2E_OP_CELL_REPAIR                  = 16,
  AVAGO_HBM2E_OP_FUSE_SCAN                    = 17,
  AVAGO_HBM2E_OP_REAPPLY_CELL_REPAIRS         = 18,
  AVAGO_HBM2E_OP_TEST_PHY_BYPASS              = 19,
  AVAGO_HBM2E_OP_TEST_HBM_BYPASS              = 20,
  AVAGO_HBM2E_OP_TEST_CATTRIP                 = 21,
  AVAGO_HBM2E_OP_TEST_CHIPPING                = 22,
  AVAGO_HBM2E_OP_TEST_EXTEST                  = 23,
  AVAGO_HBM2E_OP_BIST_AERR                    = 24,
  AVAGO_HBM2E_OP_BIST_AERR_ILB                = 25,
  AVAGO_HBM2E_OP_BIST_AERR_SLB                = 26,
  AVAGO_HBM2E_OP_BIST_AWORD                   = 27,
  AVAGO_HBM2E_OP_BIST_AWORD_ILB               = 28,
  AVAGO_HBM2E_OP_BIST_AWORD_SLB               = 29,
  AVAGO_HBM2E_OP_BIST_DERR                    = 30,
  AVAGO_HBM2E_OP_BIST_DERR_ILB                = 31,
  AVAGO_HBM2E_OP_BIST_DERR_SLB                = 32,
  AVAGO_HBM2E_OP_BIST_DWORD_READ              = 33,
  AVAGO_HBM2E_OP_BIST_DWORD_WRITE             = 34,
  AVAGO_HBM2E_OP_BIST_DWORD_UPPER_ILB         = 35,
  AVAGO_HBM2E_OP_BIST_DWORD_UPPER_SLB         = 36,
  AVAGO_HBM2E_OP_BIST_DWORD_LOWER_ILB         = 37,
  AVAGO_HBM2E_OP_BIST_DWORD_LOWER_SLB         = 38,
  AVAGO_HBM2E_OP_BIST_AWORD_CUSTOM            = 39,
  AVAGO_HBM2E_OP_BIST_AWORD_ILB_CUSTOM        = 40,
  AVAGO_HBM2E_OP_BIST_AWORD_SLB_CUSTOM        = 41,
  AVAGO_HBM2E_OP_BIST_DWORD_READ_CUSTOM       = 42,
  AVAGO_HBM2E_OP_BIST_DWORD_WRITE_CUSTOM      = 43,
  AVAGO_HBM2E_OP_BIST_DWORD_UPPER_ILB_CUSTOM  = 44,
  AVAGO_HBM2E_OP_BIST_DWORD_UPPER_SLB_CUSTOM  = 45,
  AVAGO_HBM2E_OP_BIST_DWORD_LOWER_ILB_CUSTOM  = 46,
  AVAGO_HBM2E_OP_BIST_DWORD_LOWER_SLB_CUSTOM  = 47,
  AVAGO_HBM2E_OP_BIST_AERR_ILB_WAFER          = 48,
  AVAGO_HBM2E_OP_BIST_AERR_SLB_WAFER          = 49,
  AVAGO_HBM2E_OP_BIST_AWORD_ILB_WAFER         = 50,
  AVAGO_HBM2E_OP_BIST_AWORD_SLB_WAFER         = 51,
  AVAGO_HBM2E_OP_BIST_DERR_ILB_WAFER          = 52,
  AVAGO_HBM2E_OP_BIST_DERR_SLB_WAFER          = 53,
  AVAGO_HBM2E_OP_BIST_DWORD_UPPER_ILB_WAFER   = 54,
  AVAGO_HBM2E_OP_BIST_DWORD_UPPER_SLB_WAFER   = 55,
  AVAGO_HBM2E_OP_BIST_DWORD_LOWER_ILB_WAFER   = 56,
  AVAGO_HBM2E_OP_BIST_DWORD_LOWER_SLB_WAFER   = 57,
  AVAGO_HBM2E_OP_MMT                          = 58,
  AVAGO_HBM2E_OP_MMT_CUSTOM                   = 59,
  AVAGO_HBM2E_OP_MMT_CONFIGURE                = 60,
  AVAGO_HBM2E_OP_MMT_START                    = 61,
  AVAGO_HBM2E_OP_BIST_STOP_AND_CHECK          = 65,
  AVAGO_HBM2E_OP_FLOW_BIST                    = 66,
  AVAGO_HBM2E_OP_FLOW_BIST_WAFER              = 67,
  AVAGO_HBM2E_OP_FLOW_MMT                     = 68
} Avago_hbm_operation_t;

typedef struct
{
  uint global_error_code;
  uint channel_error_code[8];
  uint channel_operation_code[8];
} Avago_hbm_operation_results_t;

/* Launches an HBM operation on all PHY/HBM channels */
EXT int avago_hbm_launch_operation(Aapl_t *aapl, uint spico_addr, Avago_hbm_operation_t operation, Avago_hbm_operation_results_t *results, int max_timeout);
EXT int avago_hbm_launch_operation_nowait(Aapl_t *aapl, uint spico_addr, Avago_hbm_operation_t operation, int max_timeout);

/* Launches an HBM operation on a single PHY/HBM channel */
EXT int avago_hbm_launch_channel_operation(Aapl_t *aapl, uint spico_addr, Avago_hbm_operation_t operation, Avago_hbm_operation_results_t *results, uint channel, int max_timeout);
EXT int avago_hbm_launch_channel_operation_nowait(Aapl_t *aapl, uint spico_addr, Avago_hbm_operation_t operation, uint channel, int max_timeout);

EXT int avago_hbm_check_operation(Aapl_t *aapl, uint spico_addr, Avago_hbm_operation_t operation, Avago_hbm_operation_results_t *results, int max_timeout);
EXT int avago_hbm_check_channel_operation(Aapl_t *aapl, uint spico_addr, Avago_hbm_operation_t operation, Avago_hbm_operation_results_t *results, uint channel, int max_timeout);

/* Reads the HBM channel errors code from firmware */
EXT int avago_hbm_get_operation_results(Aapl_t *aapl, uint spico_addr, Avago_hbm_operation_results_t *results);

/* Print the results of the operation */
EXT int avago_hbm_print_operation_results(Aapl_t *aapl, Avago_hbm_operation_results_t *results);

/* HBM Device ID */
typedef struct
{
  uint gen2_test;
  uint ecc;
  uint density;
  uint manufacturer_id;
  uint manufacturing_loc;
  uint manufacturing_year;
  uint manufacturing_week;
  unsigned long serial_number_31_0;
  uint  serial_number_35_32;
  uint addressing_mode;
  uint channel_available;
  uint hbm_stack_height;
  uint model_number;
  uint value2;
  uint value1;
  uint value0;
} Avago_hbm_device_id_t;

/* 1-D Eye Measurements */
typedef struct
{
  uint init_cfg_dll;
  uint init_dll_clk;
  uint init_dll_stb;
  uint init_obs_pvt;
  uint init_pvt_clk;
  uint init_pvt_stb;
  uint init_qc_rd_delay;
  uint init_qc_wr_delay;
  uint init_qc_ck_delay;
  uint freq;
  int left_qc_pvts[8];
  int right_qc_pvts[8];
  float setup_margin;
  float hold_margin;
} Avago_hbm_eye_t;

/* Get the HBM device id and store the results in the device_id structure */
EXT int avago_hbm_read_device_id(Aapl_t *aapl, uint spico_addr, Avago_hbm_device_id_t *device_id);

/* Print the HBM device id  */
EXT int avago_hbm_print_device_id(Aapl_t *aapl, uint spico_addr);

/* HBM Mode Registers */
typedef struct
{
  uint mr0;
  uint mr1;
  uint mr2;
  uint mr3;
  uint mr4;
  uint mr5;
  uint mr6;
  uint mr7;
  uint mr8;
  uint mr9;
  uint mr10;
  uint mr11;
  uint mr12;
  uint mr13;
  uint mr14;
  uint mr15;
} Avago_hbm_mrs_t;

/* PHY CONFIG */
typedef struct
{
  bigint dword0;
  bigint dword1;
  bigint dword2;
  bigint dword3;
  unsigned long aword;
} Avago_hbm_phy_config_t;


/* Verify the HBM firmware / sbus ring */
EXT int avago_hbm_fw_check(Aapl_t *aapl, uint spico_addr);
EXT int avago_hbm_ring_check(Aapl_t *aapl, uint spico_addr);

/* Get the HBM MRS values and store the results in the mrs structure */
EXT int avago_hbm_read_mrs(Aapl_t *aapl, uint spico_addr, Avago_hbm_mrs_t *mrs, uint channel);

/* Print the HBM MRS values */
EXT int avago_hbm_print_mrs(Aapl_t *aapl, uint spico_addr, uint channel);

/* Get the HBM PHY_CONFIG values and store the results in the phy_config structure */
EXT int avago_hbm_read_phy_config(Aapl_t *aapl, uint spico_addr, Avago_hbm_phy_config_t *phy_config, uint channel);

/* Print the PHY_CONFIG values */
EXT int avago_hbm_print_phy_config(Aapl_t *aapl, uint spico_addr, uint channel);

/* Print the HBM lane repairs */
EXT int avago_hbm_print_hard_lane_repairs(Aapl_t *aapl, uint apc_addr);
EXT int avago_hbm_print_lane_repairs(Aapl_t *aapl, uint spico_addr);

/* Get the HBM temperature */
EXT int avago_hbm_read_device_temp(Aapl_t *aapl, uint spico_addr);

/* Return the APC address on the sbus ring */
EXT int avago_hbm_get_apc_addr(Aapl_t *aapl, uint spico_addr, uint *apc_addr);

/* HBM diagnostics */
EXT int avago_hbm_parameter_diagnostics(Aapl_t *aapl, uint spico_addr);
EXT int avago_hbm_run_diagnostics(Aapl_t *aapl, uint spico_addr);
EXT int avago_hbm_run_ctc_diagnostics(Aapl_t *aapl, uint spico_addr, int do_reset, int do_init_nwl);
EXT int avago_hbm_run_mbist_diagnostics(Aapl_t *aapl, uint spico_addr);
EXT int avago_hbm_run_temp_diagnostics(Aapl_t *aapl, uint spico_addr, int count);
EXT int avago_hbm_ctc_start(Aapl_t *aapl, uint spico_addr, int pattern_type, int init);
EXT int avago_hbm_ctc_stop(Aapl_t *aapl, uint spico_addr);
EXT int avago_hbm_print_ctc_results(Aapl_t *aapl, uint apc_addr);
EXT BOOL avago_hbm_has_ctc(Aapl_t *aapl, uint apc_addr);
EXT int avago_hbm_run_mmt_diagnostics(Aapl_t *aapl, uint spico_addr);
EXT int avago_hbm_run_qc_diagnostics(Aapl_t *aapl, uint spico_addr);
EXT int avago_hbm_mmt_start(Aapl_t *aapl, uint spico_addr, int pattern_type);
EXT int avago_hbm_mmt_stop(Aapl_t *aapl, uint spico_addr);
EXT int avago_hbm_mmt_loop(Aapl_t *aapl, uint spico_addr, int count, int pattern_type);
EXT int avago_hbm_mmt_margin(Aapl_t *aapl, uint spico_addr, Avago_hbm_eye_t *hbm_eye);
EXT int avago_hbm_print_mmt_eye(Aapl_t *aapl, Avago_hbm_eye_t *hbm_eye);
EXT int avago_hbm_print_mmt_results(Aapl_t *aapl, uint apc_addr, uint verbose);
EXT int avago_hbm_print_spare_results(Aapl_t *aapl, uint spico_addr);
EXT int avago_hbm_read_lfsr_compare_sticky(Aapl_t *aapl, uint spico_addr, int hbm_not_phy, int result_group, const char * prefix);

EXT int avago_hbm_set_hbm_drv_str(Aapl_t *aapl, uint spico_addr, uint drvstr);
EXT int avago_hbm_set_phy_drv_str(Aapl_t *aapl, uint spico_addr, uint drvstr);
EXT int avago_hbm_set_phy_quarter_cycle(Aapl_t *aapl, uint spico_addr, uint qc_rd, uint qc_wr, uint qc_ck);

EXT int avago_hbm_cell_repair(Aapl_t *aapl, uint spico_addr, uint channel, uint pc, uint sid, uint bank, uint row, uint hard_repair);
EXT int avago_hbm_fuse_scan(Aapl_t *aapl, uint spico_addr, uint channel, uint pc, uint sid, uint bank, uint *result);

EXT int avago_hbm_soft_lane_repair(Aapl_t *aapl, uint spico_addr, uint channel, uint repair0, uint repair1, uint repair2);

/* TMRS */
EXT int avago_hbm_run_tmrs(Aapl_t *aapl, uint spico_addr, const char *tmrs_code, uint channel, uint safety);

EXT BOOL avago_hbm2e_check(Aapl_t *aapl, uint spico_addr);

#endif

#endif
