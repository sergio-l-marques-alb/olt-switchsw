/** \file bcm_int/dnx/dram/gddr6/gddr6_func.h
 *
 * This file contains GDDR6 call backs routine declarations for the GDDR6 Dram tunning process.
 *
 */

/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef _BCMINT_DNX_DRAM_GDDR6_GDDR6_FUNC_H_INCLUDED
#define _BCMINT_DNX_DRAM_GDDR6_GDDR6_FUNC_H_INCLUDED

/*
 * {
 */
#include <soc/shmoo_g6phy16.h>
#include <bcm_int/dnx/dram/gddr6/gddr6.h>

/*
 *     macra definition
 */

/***************************************************/

typedef enum
{
    /*
     * costum pattern. Use the pattern as is.
     */
    GDDR6_DRAM_BIST_DATA_PATTERN_CUSTOM,
    /*
     *  The PRBS will be used to generate the pattern towards
     *  the DRAM.
     */
    GDDR6_DRAM_BIST_DATA_PATTERN_RANDOM_PRBS,
    /*
     *  Fill the data to write by 1010101... (Bits). The DATA_MODE
     *  may use this data pattern in different ways see
     *  SOC_TMC_DRAM_BIST_DATA_MODE. 
     */
    GDDR6_DRAM_BIST_DATA_PATTERN_DIFF,
    /*
     * Fill the data to write by 010101... (Bits). The DATA_MODE
     * may use this data pattern in different ways see
     * SOC_TMC_DRAM_BIST_DATA_MODE. 
     */
    GDDR6_DRAM_BIST_DATA_PATTERN_DIFF_REVERT,

    /*
     *  Fill the data to write by 11111111... (Bits). The DATA_MODE
     *  may use this data pattern in different ways see
     *  SOC_TMC_DRAM_BIST_DATA_MODE. Random mode ignores these
     *  values.
     */
    GDDR6_DRAM_BIST_DATA_PATTERN_ONE,
    /*
     *  Fill the data to write by 00000000... (Bits). The DATA_MODE
     *  may use this data pattern in different ways see
     *  SOC_TMC_DRAM_BIST_DATA_MODE. Random mode ignores these
     *  values.
     */
    GDDR6_DRAM_BIST_DATA_PATTERN_ZERO,
    /*
     * A different bit is selected from pattern0 - pattern7 in an incremental manner.
     * The selected bit is duplicated on all of the dram data bus.
     */
    GDDR6_DRAM_BIST_DATA_PATTERN_BIT_MODE,
    /*
     * data shift mode.
     * Every write/read command the data will be shifted 1 bit to the left in a cyclic manner.
     * The initial pattern is pattern0 which is duplicated 8 times.
     * This DATA_MODE create 8 consecutive dram transactions.
     */
    GDDR6_DRAM_BIST_DATA_PATTERN_SHIFT_MODE,
    /*
     * data address mode.
     * Every command data will be equal to the address it is written to.
     */
    GDDR6_DRAM_BIST_DATA_PATTERN_ADDR_MODE,
    /*
     * nof data pattern modes
     */
    GDDR6_DRAM_BIST_NOF_DATA_PATTERN_MODES
} dnx_gddr6_bist_pattern_mode;
typedef enum
{
    LDFF = 0,
    RDTR = 1,
    WRTR = 2,
    CADT = 3,
    VENDOR_ID = 4
} dnx_gddr6_training_bist_mode;

typedef struct ldff_pattern_s
{
    uint32 dq_pattern[SHMOO_G6PHY16_BIST_NOF_PATTERNS][4];
    uint32 dbi_pattern[SHMOO_G6PHY16_BIST_NOF_PATTERNS];
    uint32 edc_pattern[SHMOO_G6PHY16_BIST_NOF_PATTERNS];
} dnx_gddr6_ldff_pattern;

typedef struct training_bist_cmd_configuration_s
{
    uint64 commands;
    int write_weight;
    int read_weight;
    int fifo_depth;
    dnx_gddr6_training_bist_mode training_bist_mode;
} dnx_gddr6_training_bist_cmd_configuration;

typedef struct training_cadt_bist_configuration_s
{
    int commands;
    shmoo_g6phy16_cadt_mode_t cadt_mode;
    int cadt_to_cadt_prd;
    int cadt_to_rden_prd;
    int cadt_prbs_mode;
    int cadt_seed;
    int cadt_invert_caui2;
    uint32 cadt_pattern[SHMOO_G6PHY16_BIST_NOF_PATTERNS];
} dnx_gddr6_training_cadt_bist_configuration;

typedef struct training_bist_configuration_s
{
    dnx_gddr6_bist_pattern_mode dq_pattern_mode;
    dnx_gddr6_bist_pattern_mode dbi_pattern_mode;
    dnx_gddr6_bist_pattern_mode edc_pattern_mode;
    dnx_gddr6_ldff_pattern pattern;
    dnx_gddr6_training_bist_cmd_configuration cmd_cfg;
    dnx_gddr6_training_cadt_bist_configuration cadt_cfg;
} dnx_gddr6_training_bist_configuration;

typedef struct bist_configuration_s
{
    uint64 nof_commands;
    int write_weight;
    int read_weight;
    int same_row;

    dnx_gddr6_bist_address_mode_t bank_mode;
    int bank_start;
    int bank_end;
    int bank_seed;
    dnx_gddr6_bist_address_mode_t column_mode;
    int column_start;
    int column_end;
    int column_seed;
    dnx_gddr6_bist_address_mode_t row_mode;
    int row_start;
    int row_end;
    int row_seed;

    dnx_gddr6_bist_data_mode_t data_mode;
    uint32 dbi_mode;

    uint32 data_seed[8];
    uint32 dbi_seed;

    uint32 bist_dq_pattern[8][8];
    uint32 bist_dbi_pattern[8];

    uint32 bist_timer_us;
} dnx_gddr6_bist_configuration_t;

typedef struct last_returned_data
{
    uint32 last_returned_dq[8];
    uint32 last_returned_dbi;
    uint32 last_returned_edc;
} dnx_gddr6_bist_last_returned_data_t;

/**
 * \brief - initialize the GDDR6 controller timing and swap info based on board
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - controller that is connected to dram index
 * \param [in] ignore_vendor - ignore vendor when init controller
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * the ignore vendor option is used when vendor info cannot be retrieved yet, but parts of the configurations are required to get the vendor info.
 * \see
 *   * None
 */
shr_error_e dnx_gddr6_controller_init(
    int unit,
    int dram_index,
    int ignore_vendor);

/**
 * \brief - This function is responsible for gddr6 init sequence. initialize Dram side (not controller side) and sets MRs
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [in] phase - the dram init phase to do. phases are described in relevant enum (gddr6_dram_init_phase_t)
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_gddr6_dram_init(
    int unit,
    int dram_index,
    gddr6_dram_init_phase_t phase);
/**
 * \brief - get the GDDR6 dram shmoo info
 *
 * \param [in] unit - unit number
 * \param [out] shmoo_info - gddr6 dram shmoo info
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_gddr6_shmoo_dram_info_get(
    int unit,
    g6phy16_shmoo_dram_info_t * shmoo_info);
/**
 * \brief - start wck2ck training
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_gddr6_wck2clk_training_start(
    int unit,
    int dram_index);

/**
 * \brief - get bist error count
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [out] bist_error_count - bist error counter
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_gddr6_bist_error_counter_get(
    int unit,
    int dram_index,
    dnx_gddr6_bist_err_cnt_t * bist_error_count);
/**
 * \brief - configure data bist
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [in] channel - channel
 * \param [in] bist_cfg - bist configuration
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_gddr6_data_bist_configure(
    int unit,
    int dram_index,
    int channel,
    dnx_gddr6_bist_configuration_t * bist_cfg);

/**
 * \brief - get dram vendor info
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [in] mode - vendor info mode
 * \param [out] gddr6_vendor - vendor info
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_gddr6_vendor_info_get(
    int unit,
    int dram_index,
    uint32 mode,
    uint32 *gddr6_vendor);

shr_error_e dnx_gddr6_load_precharge_all_command(
    int unit,
    int dram_index,
    uint32 cabi_enabled);

shr_error_e dnx_gddr6_load_activate_command(
    int unit,
    int dram_index,
    uint32 bank,
    uint32 cabi_enabled);

/**
 * \brief - get bist error counter
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [in] channel - channel
 * \param [out] bist_err_cnt - bist error counter
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_gddr6_bist_err_cnt_get(
    int unit,
    int dram_index,
    int channel,
    dnx_gddr6_bist_err_cnt_t * bist_err_cnt);

#endif
