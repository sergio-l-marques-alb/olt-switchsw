/** \file bcm_int/dnx/dram/gddr6/gddr6.h
 *
 * This file contains GDDR6 main structure and routine declarations for the GDDR6 Dram operation.
 *
 */

/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef _BCMINT_DNX_DRAM_GDDR6_GDDR6_H_INCLUDED
/*
 * {
 */
#define _BCMINT_DNX_DRAM_GDDR6_GDDR6_H_INCLUDED

#ifdef DNX_DATA_INTERNAL
#undef DNX_DATA_INTERNAL
#endif
#include <soc/shmoo_g6phy16.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_dram_types.h>
#include <bcm/switch.h>

/*************** MACRA DEFINITION ********************/
#define DNX_GDDR6_ITER_ALL -1
#define DNX_GDDR6_CHANNEL_ITER_ALL -1

#define GDDR6_SWAP_SUFFIX_BUFFER_SIZE 256
#define GDDR6_SOC_PROPERTY_STR_BUFFER_SIZE 256
#define GDDR6_VAL_STR_BUFFER_SIZE 1024
#define GDDR6_STR_VAL_SIZE (32)

#define BURST_LENGTH 16
#define BURST_LENGTH_DIV_4 BURST_LENGTH/4
#define DNX_GDDR6_VAL_IS_IN_CLOCKS_BIT (31)

/** this enum describes the different possible actions when performing dram phy tuning */
typedef enum
{
    FIRST_DNX_GDDR6_PHY_TUNE_ACTION_E = 0,
    /** restore tuning parameters from soc property, not run tuning */
    DNX_GDDR6_RESTORE_TUNE_PARAMETERS = FIRST_DNX_GDDR6_PHY_TUNE_ACTION_E,
    /** run tuning to find correct tune parameters */
    DNX_GDDR6_RUN_TUNE,
    /** restore tune parameters from soc properties, if not found run tuning */
    DNX_GDDR6_RESTORE_TUNE_PARAMETERS_OR_RUN_TUNE,
    /** skip tune - relevant only during init */
    DNX_GDDR6_SKIP_TUNE,
    /** number of possible tune actions */
    NUM_DNX_GDDR6_PHY_TUNE_ACTION_E
} dnx_gddr6_phy_tune_action_e;

/** this enum describes the different possible Address Translation Matrixes (ATMs) */
typedef enum dnx_gddr6_tdu_matrix_option_e
{
        /** single Dram */
    DNX_GDDR6_SINGLE_DRAM_ATM = 0,
        /** 2 Dram */
    DNX_GDDR6_2_DRAMS_ATM = 1,
        /** number of ATMs */
    DNX_GDDR6_NOF_ATMS = 2
} dnx_gddr6_tdu_matrix_option_t;

typedef enum dnx_gddr6_bist_address_mode_e
{
    DNX_GDDR6_BIST_ADDRESS_MODE_INCREMENTAL = 0,
    DNX_GDDR6_BIST_ADDRESS_MODE_PRBS = 1,
} dnx_gddr6_bist_address_mode_t;

typedef enum dnx_gddr6_info_mode_e
{
    DNX_GDDR6_ID1 = 1,
    DNX_GDDR6_TEMP = 2,
    DNX_GDDR6_ID2 = 3,
} dnx_gddr6_info_mode_t;

typedef enum dnx_gddr6_bist_data_mode_e
{
    DNX_GDDR6_BIST_DATA_MODE_PATTERN = 0,
    DNX_GDDR6_BIST_DATA_MODE_PRBS = 1,
} dnx_gddr6_bist_data_mode_t;

typedef enum
{
    /*
     * Use both WCKs (one per data byte)
     */
    GDDR6_DRAM_WCK_PER_BYTE,
    /*
     *  Use single WCK (one per data word)
     */
    GDDR6_DRAM_WCK_PER_WORD,
} dnx_gddr6_wck_granularity;

/**
 * \brief
 *   DRAM protocol error counters
 */
typedef struct gddr6_dram_err_cnt_s
{
    /** Read CRC Bytes error count */
    uint32 read_crc_err_cnt[DNX_DATA_MAX_DRAM_GDDR6_BYTES_PER_CHANNEL][2];
    /** Write CRC Bytes error count */
    uint32 write_crc_err_cnt[DNX_DATA_MAX_DRAM_GDDR6_BYTES_PER_CHANNEL][2];
} dnx_gddr6_dram_err_cnt_t;

typedef struct gddr6_bist_err_cnt_s
{
    uint32 cadt_err_cnt;
    uint32 cadt_err_bitmap[1];
    uint32 bist_data_err_cnt;
    uint32 bist_data_err_bitmap[1];
    uint32 bist_edc_err_cnt;
    uint32 bist_edc_err_bitmap[1];
    uint32 bist_dbi_err_cnt;
    uint32 bist_dbi_err_bitmap[1];
} dnx_gddr6_bist_err_cnt_t;

typedef struct gddr6_bist_status_cnt_s
{
    uint32 write_command_cnt[2];
    uint32 read_command_cnt[2];
    uint32 read_data_cnt[2];
    uint32 read_training_data_cnt[2];
    uint32 read_edc_cnt[2];
    uint32 wrtr_command_cnt[2];
    uint32 rdtr_command_cnt[2];
    uint32 ldff_command_cnt[2];
} dnx_gddr6_bist_status_cnt_t;

/**
 * \brief
 *   Hold data per channel
 */
typedef struct
{
    /** value per dram,channel */
    uint32 val[DNX_DATA_MAX_DRAM_GENERAL_INFO_MAX_NOF_DRAMS][DNX_DATA_MAX_DRAM_GENERAL_INFO_NOF_CHANNELS];
} dnx_gddr6_per_channel_info_t;

/**
 * \brief - verify dram index is valid
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 *
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_gddr6_index_verify(
    int unit,
    int dram_index);

/**
 * \brief - init sequence for gddr6 controller sw state
 *
 * \param [in] unit - unit number
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_gddr6_sw_state_init(
    int unit);

/**
 * \brief - init sequence for gddr6 controller
 *
 * \param [in] unit - unit number
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_gddr6_init(
    int unit);

/**
 * \brief - gddr6 dram pll configurations according to pll_info
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [in] pll_info - pll info to configure
 *
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_gddr6_configure_plls(
    int unit,
    int dram_index,
    const g6phy16_drc_pll_t * pll_info);

/**
 * \brief - gddr6 phy out of reset
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 *
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_gddr6_phy_out_of_reset(
    int unit,
    int dram_index);

/**
 * \brief - steps that has to be done during init before out of soft reset but after blocks reset
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 *
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_gddr6_phy_pll_init(
    int unit,
    int dram_index);

/**
 * \brief - get MR value
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [in] channel - channel in hbm
 * \param [in] mr_index - mode register index
 * \param [out] value - mode register value
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   configuration of MRs is symmetric, all drams, and all channels has the same value.
 * \see
 *   * None
 */
shr_error_e dnx_gddr6_mode_register_get(
    int unit,
    int dram_index,
    int channel,
    int mr_index,
    uint32 *value);
/**
 * \brief - Set MR value
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [in] channel - channel
 * \param [in] mr_index - mode register index
 * \param [out] value - mode register value
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   configuration of MRs is symmetric, all drams, and all channels has the same value.
 * \see
 *   * None
 */
shr_error_e dnx_gddr6_mode_register_set(
    int unit,
    int dram_index,
    int channel,
    int mr_index,
    uint32 value);
/**
 * \brief - perform soft init on channels
 *
 * \param [in] unit - unit number
 * \param [in] dram_ndx - dram index
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * soft init will NOT change configuration registers
 * \see
 *   * None
 */
shr_error_e dnx_gddr6_channels_soft_init(
    int unit,
    int dram_ndx);
/**
 * \brief - indirect access read from dram physical address
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram_index
 * \param [in] channel - channel index
 * \param [in] bank - bank index
 * \param [in] row - row index
 * \param [in] column - column index
 * \param [in] data - read data
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_gddr6_ind_access_physical_address_read(
    int unit,
    uint32 dram_index,
    uint32 channel,
    uint32 bank,
    uint32 row,
    uint32 column,
    uint32 *data);
/**
 * \brief - indirect access write to dram physical address
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram_index
 * \param [in] channel - channel index
 * \param [in] bank - bank index
 * \param [in] row - row index
 * \param [in] column - column index
 * \param [in] pattern - pattern to write, pattern size in bytes should be as defined in
 *                       dnx_data_dram.address_translation.physical_address_transaction_size
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_gddr6_ind_access_physical_address_write(
    int unit,
    uint32 dram_index,
    uint32 channel,
    uint32 bank,
    uint32 row,
    uint32 column,
    uint32 *pattern);
/**
 * \brief - indirect access read from logical dram buffer
 *
 * \param [in] unit - unit number
 * \param [in] buffer - logical buffer to read
 * \param [in] index - index in logical buffer to read, -1 means read all, else valid range is between 0 to
 *     dnx_data_dram.general_info.buffer_size/dnx_data_dram.address_translation.physical_address_transaction_size
 * \param [in] data - read data
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_gddr6_ind_access_logical_buffer_read(
    int unit,
    int buffer,
    int index,
    uint32 *data);

/**
 * \brief - indirect access write to logical dram buffer
 *
 * \param [in] unit - unit number
 * \param [in] buffer - logical buffer to write
 * \param [in] index - index in logical buffer to write, -1 means write all, else valid range is between 0 to
 *     dnx_data_dram.general_info.buffer_size/dnx_data_dram.address_translation.physical_address_transaction_size
 * \param [in] pattern - pattern to write, pattern size in bytes should be as defined in
 *                       dnx_data_dram.address_translation.physical_address_transaction_size
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_gddr6_ind_access_logical_buffer_write(
    int unit,
    int buffer,
    int index,
    uint32 *pattern);
/**
 * \brief
 *  GDDR6 access init function.
 *  Used to init GDDR6
 * \param [in] unit -
 *   The unit number.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */
shr_error_e soc_dnx_gddr6_access_init(
    int unit);

/**
 * \brief - init tdu
 *
 * \param [in] unit - unit number
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_gddr6_tdu_configure(
    int unit);

/**
 * \brief - set soft reset on all channels of gddr6 index provided
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [in] in_soft_reset - in/out of soft reset
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * soft reset will change configuration registers as well.
 * \see
 *   * None
 */
shr_error_e dnx_gddr6_channels_soft_reset_set(
    int unit,
    int dram_index,
    int in_soft_reset);

/**
 * \brief - set soft init on all channels of gddr6 index provided
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [in] in_soft_init - in/out of soft init
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * soft reset will change configuration registers as well.
 * \see
 *   * None
 */
shr_error_e dnx_gddr6_channels_soft_init_set(
    int unit,
    int dram_index,
    int in_soft_init);

/**
 * \brief - init phy registers
 *
 * \param [in] unit - unit number
 *
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_gddr6_phy_init(
    int unit);
/**
 * \brief - get refresh enable state, refresh_enable_bitmap=(refresh<<1+refresh_ab) for each channel, the channel 1
 *            -- high 2 bit, the channel 0 -- low 2 bit
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [out] refresh_enable_bitmap - bitmap per dram to indicate if refresh is enabled/disabled for that dram
 * \param [out] refresh_ab_enable_bitmap - bitmap per dram to indicate if refresh_AB is enabled/disabled for that dram
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_gddr6_dram_refresh_enable_bitmap_get(
    int unit,
    int dram_index,
    uint32 *refresh_enable_bitmap,
    uint32 *refresh_ab_enable_bitmap);
/**
 * \brief - set refresh enable bitmap refresh_enable_bitmap=(refresh<<1+refresh_ab) for each channel, the channel 1
 *            -- high 2 bit, the channel 0 -- low 2 bit
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [in] refresh_enable_bitmap - bitmap per dram to indicate if refresh is enabled/disabled for that dram
 * \param [in] refresh_ab_enable_bitmap - bitmap per dram to indicate if refresh_AB is enabled/disabled for that dram
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_gddr6_dram_refresh_enable_bitmap_set(
    int unit,
    int dram_index,
    uint32 refresh_enable_bitmap,
    uint32 refresh_ab_enable_bitmap);

/**
 * \brief - dram bist start action
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [in] channel - channel
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_gddr6_bist_start(
    int unit,
    int dram_index,
    int channel);

/**
 * \brief - dram bist stop action
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [in] channel - channel
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_gddr6_bist_stop(
    int unit,
    int dram_index,
    int channel);

/**
 * \brief - poll dram bist run done
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [in] channel - channel
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_gddr6_bist_run_done_poll(
    int unit,
    int dram_index,
    int channel);

/**
 * \brief - tune gddr6 dram phy
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [in] shmoo_type - shmoo type
 * \param [in] shmoo_flags - shmoo flags
 * \param [in] action - shmoo action, skip/tuning/restore from soc property
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_gddr6_run_phy_tune(
    int unit,
    int dram_index,
    int shmoo_type,
    uint32 shmoo_flags,
    dnx_gddr6_phy_tune_action_e action);

/**
 * \brief - determine according to BIST status and error counters if BIST run was successful, or if it had errors
 *
 * \param [in] unit - unit number
 * \param [in] bist_status_cnt - structure containing BIST status counters
 * \param [in] bist_err_cnt - structure containing BIST error counters
 * \param [out] bist_error_indicated - indication that a BIST error has occurred - meaning BIST run was not successful.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   *  when traffic redirection to OCB is disabled, dram_is_empty parameter is not relevant
 *      and will be set to TRUE
 * \see
 *   * None
 */
shr_error_e dnx_gddr6_bist_errors_check(
    int unit,
    dnx_gddr6_bist_status_cnt_t * bist_status_cnt,
    dnx_gddr6_bist_err_cnt_t * bist_err_cnt,
    int *bist_error_indicated);

/**
 * \brief -
 *
 * \param [in] unit - unit number
 * \param [in] enable - enable traffic redirection to OCB
 * \param [out] dram_is_empty - indication that dram is now empty, relevant only for enabling redirection
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   *  when traffic redirection to OCB is disabled, dram_is_empty parameter is not relevant
 *      and will be set to TRUE
 * \see
 *   * None
 */
shr_error_e dnx_gddr6_redirect_traffic_to_ocb(
    int unit,
    uint32 enable,
    uint8 *dram_is_empty);

/**
 * \brief - get bist status
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [in] channel - channel
 * \param [out] bist_status_cnt - status counters
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_gddr6_bist_status_get(
    int unit,
    int dram_index,
    int channel,
    dnx_gddr6_bist_status_cnt_t * bist_status_cnt);

/**
 * \brief - enable dynamic calibration mechanism, differentiate between 2 state:
 *  set initial position
 *  don't set initial position
 *
 *  this is controlled by set_init_position parameter, should be used after initial position might have changed, meaning after DRAM tuning.
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [in] channel - channel index
 * \param [in] set_init_position - set/don't set initial position
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_gddr6_dynamic_calibration_enable(
    int unit,
    int dram_index,
    int channel,
    int set_init_position);

/**
 * \brief - disable dynamic calibration
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [in] channel - channel index
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_gddr6_dynamic_calibration_disable(
    int unit,
    int dram_index,
    int channel);

/**
 * \brief - check if dynamic calibration is enabled
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [in] channel - channel index
 * \param [in] is_enabled - indication if dynamic calibration is currently enabled
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_gddr6_dynamic_calibration_is_enabled(
    int unit,
    int dram_index,
    int channel,
    uint32 *is_enabled);

/**
 * \brief - Get dram temperature, this shouldn't run with traffic or Dram BIST.
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [out] sensor_data - read temperature
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_gddr6_dram_temp_get(
    int unit,
    int dram_index,
    bcm_switch_thermo_sensor_t * sensor_data);

/**
 * \brief - Get dram vendor_id, this shouldn't run with traffic or Dram BIST.
 *
 * \param [in] unit - unit number
 * \param [in] dram - dram index
 * \param [out] vendor_id - read vendor id (in raw format)
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_gddr6_dram_vendor_id_get(
    int unit,
    int dram,
    uint32 *vendor_id);

/**
 * \brief - decode vendor_id to vendor info.
 *
 * \param [in] unit - unit number
 * \param [in] vendor_id - vendor id (in raw format)
 * \param [out] vendor_info - decoded vendor info
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_gddr6_dram_vendor_id_to_vendor_info_decode(
    int unit,
    uint32 vendor_id,
    gddr6_dram_vendor_info_t * vendor_info);

/**
 * \brief - Get dram vendor info from sw-state.
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [out] vendor_info - vendor info
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_gddr6_vendor_info_from_sw_state_get(
    int unit,
    int dram_index,
    gddr6_dram_vendor_info_t * vendor_info);

/**
 * \brief - Set dram vendor info to sw-state.
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [in] vendor_info - vendor info
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_gddr6_vendor_info_from_sw_state_set(
    int unit,
    int dram_index,
    gddr6_dram_vendor_info_t * vendor_info);

/**
 * \brief - Register device specific CBs to common phy tuning mechanism
 *
 * \param [in] unit - unit number
 *
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
extern shr_error_e dnx_gddr6_call_backs_register(
    int unit);
/**
 * \brief - mark hbc blocks as not valid according to dram bitmap
 *
 * \param [in] unit - unit number
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_gddr6_dcc_blocks_enable_set(
    int unit);

/**
 * \brief
 *   Save current value of dynamic calibration 
 *   and disable dynamic calibration
 */
shr_error_e dnx_gddr6_dynamic_calibration_save_and_disable(
    int unit,
    dnx_gddr6_per_channel_info_t * dynamic_calibration_enable);

/**
 * \brief
 *   Save current value of dynamic calibration 
 *   and disable dynamic calibration
 */
shr_error_e dnx_gddr6_dynamic_calibration_restore(
    int unit,
    dnx_gddr6_per_channel_info_t * dynamic_calibration_enable);
#endif
