/** \file bcm_int/dnx/dram/hbmc/hbmc.h
 *
 * This file contains HBMC main structure and routine declarations for the HBM Dram operation.
 *
 */

/*
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef _BCMINT_DNX_DRAM_HBMC_HBMC_H_INCLUDED
/*
 * {
 */
#define _BCMINT_DNX_DRAM_HBMC_HBMC_H_INCLUDED

#include <shared/shrextend/shrextend_debug.h>

#ifdef DNX_DATA_INTERNAL
#undef DNX_DATA_INTERNAL
#endif
#include <soc/hbmc_shmoo.h>
#include <soc/dnx/dnx_hbmc.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_dram_types.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_dram.h>
#include <bcm/switch.h>

/*
 * DEFINEs
 * {
 */

#define DNX_HBMC_ITER_ALL -1
#define DNX_HBMC_STR_VAL_SIZE (32)
#define DNX_HBMC_HBM_SERIAL_NUMBER_SIZE 2

/**
 * \brief
 *   Hold Mode Registers values in order to save them before soft init and restore
 *   them after soft init is performed.
 */
typedef struct dnx_hbmc_mrs_values_s
{
    /** Mode Register value per dram,channel,mr */
    uint32
        mr_val[DNX_DATA_MAX_DRAM_GENERAL_INFO_MAX_NOF_DRAMS][DNX_DATA_MAX_DRAM_GENERAL_INFO_NOF_CHANNELS]
        [DNX_DATA_MAX_DRAM_GENERAL_INFO_NOF_MRS];
} dnx_hbmc_mrs_values_t;

/*
 * }
 */

/** this enum describes the different possible actions when performing dram phy tuning */
typedef enum
{
    FIRST_DNX_HBMC_PHY_TUNE_ACTION_E = 0,
    /** restore tune parameters from soc properties instead of finding them directly */
    DNX_HBMC_RESTORE_TUNE_PARAMETERS_FROM_SOC_PROPERTIES = FIRST_DNX_HBMC_PHY_TUNE_ACTION_E,
    /** restore tune parameters from otp instead of finding them directly */
    DNX_HBMC_RESTORE_TUNE_PARAMETERS_FROM_OTP,
    /** run tuning to find correct tune parameters */
    DNX_HBMC_RUN_TUNE,
    /** skip tune - relevant only during init */
    DNX_HBMC_SKIP_TUNE,
    /**
     * Restore tune parameters from soc properties, or, if not found, from otp. If
     * not found on OTP as well, find them directly (by tuning).
     */
    DNX_HBMC_RESTORE_TUNE_PARAMS_FROM_SOC_PROPS_OR_OTP_OR_TUNE,
    /** number of possible tune actions */
    NUM_DNX_HBMC_PHY_TUNE_ACTION_E
} dnx_hbmc_phy_tune_action_e;

/** this enum describes the different possible Address Translation Matrixes (ATMs) */
typedef enum dnx_hbmc_tdu_matrix_option_e
{
        /** single HBM */
    DNX_HBMC_SINGLE_HBM_ATM = 0,
        /** 2 HBM */
    DNX_HBMC_2_HBMS_ATM = 1,
        /** number of ATMs */
    DNX_HBMC_NOF_ATMS = 2
} dnx_hbmc_tdu_matrix_option_t;

/** this enum describes the different possible types of registers to be diagnosed differently */
typedef enum dnx_hbmc_diag_registers_type_e
{
    /** register type is histogram of 32 by 8 */
    DNX_HBMC_DIAG_REG_HISTOGRAM_32_8,
    /** number of register types */
    DNX_HBMC_NOF_DIAG_REG_TYPES
} dnx_hbmc_diag_registers_type_t;

/**
 * \brief - hbm pll configurations according to pll_info
 *
 * \param [in] unit - unit number
 * \param [in] hbm_index - hbm index
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
shr_error_e dnx_hbmc_configure_plls(
    int unit,
    int hbm_index,
    const hbmc_shmoo_pll_t * pll_info);

/**
 * \brief - initialize the HBM PLL
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
shr_error_e dnx_hbmc_pll_init(
    int unit);

/**
 * \brief - initialize the HBM CPU controller and sets MRs
 *
 * \param [in] unit - unit number
 * \param [in] hbm_index - hbm index
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_hbmc_hbm_init(
    int unit,
    int hbm_index);

/**
 * \brief - get MR value
 *
 * \param [in] unit - unit number
 * \param [in] hbm_index - hbm index
 * \param [in] channel - channel in hbm
 * \param [in] mr_index - mode register index
 * \param [out] value - mode register value
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   configuration of MRs is symmetric, all hbms, and all channels has the same value.
 * \see
 *   * None
 */
shr_error_e dnx_hbmc_mode_register_get(
    int unit,
    int hbm_index,
    int channel,
    int mr_index,
    uint32 *value);

/**
 * \brief - Set MR value
 *
 * \param [in] unit - unit number
 * \param [in] hbm_index - hbm index
 * \param [in] channel - channel
 * \param [in] mr_index - mode register index
 * \param [out] value - mode register value
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   configuration of MRs is symmetric, all hbms, and all channels has the same value.
 * \see
 *   * None
 */
shr_error_e dnx_hbmc_mode_register_set(
    int unit,
    int hbm_index,
    int channel,
    int mr_index,
    uint32 value);

/**
 * \brief - run dram phy tune
 *
 * \param [in] unit - unit number
 * \param [in] hbm_index - hbm index, value of DNX_HBMC_ITER_ALL will tune all hbm indexes and all channels
 * \param [in] channel - channel index
 * \param [in] shmoo_type - shmoo type to run, SHMOO_HBM16_SHMOO_RSVP runs all types.
 * \param [in] flags - supported flags are SHMOO_HBM16_CTL_FLAGS_XXX
 * \param [in] action - possible actions are detailed in dnx_hbmc_phy_tune_action_e
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   configuration of MRs is symmetric, all hbms, and all channels has the same value.
 * \see
 *   * None
 */
shr_error_e dnx_hbmc_run_phy_tune(
    int unit,
    int hbm_index,
    int channel,
    int shmoo_type,
    uint32 flags,
    dnx_hbmc_phy_tune_action_e action);

/**
 * \brief - init sequence for HBMC with APD PHY
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
shr_error_e dnx_hbmc_init_with_apd_phy(
    int unit);

/**
 * \brief - init sequence for hbmc
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
shr_error_e dnx_hbmc_init(
    int unit);

/**
 * \brief - deinit sequence for hbmc
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
shr_error_e dnx_hbmc_deinit(
    int unit);

/**
 * \brief - values init for hbmc dram sw state. this function does not include
 * init of the sw state DB, only values init relevant for hbmc.
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
shr_error_e dnx_hbmc_sw_state_init(
    int unit);

/**
 * \brief - stops and destroy temp monitoring thread if it exists
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
shr_error_e dnx_hbmc_temp_monitor_deinit(
    int unit);

/**
 * \brief - create and start temp monitoring thread
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
shr_error_e dnx_hbmc_temp_monitor_init(
    int unit);

/**
 * \brief - check if temp monitoring thread is currently running.
 *
 * \param [in] unit - unit number
 * \param [out] is_active - indication if temp monitoring thread is running.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_hbmc_temp_monitor_is_active(
    int unit,
    int *is_active);

/**
 * \brief - resumes temp monitoring after it was paused.
 * if temp monitoring is already running, does nothing.
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
shr_error_e dnx_hbmc_temp_monitor_resume(
    int unit);

/**
 * \brief - stops temp monitoring thread, thread has to be active.
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
shr_error_e dnx_hbmc_temp_monitor_pause(
    int unit);

/**
 * \brief - get bist configuration
 *
 * \param [in] unit - unit number
 * \param [in] hbm_index - hbm index
 * \param [in] channel - channel
 * \param [out] info - container to save bist info
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_hbmc_bist_configuration_get(
    int unit,
    int hbm_index,
    int channel,
    hbmc_shmoo_bist_info_t * info);

/**
 * \brief - set bist configuration
 *
 * \param [in] unit - unit number
 * \param [in] hbm_index - hbm index
 * \param [in] channel - channel
 * \param [in] info - info according to which to configure the bist
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_hbmc_bist_configuration_set(
    int unit,
    int hbm_index,
    int channel,
    const hbmc_shmoo_bist_info_t * info);

/**
 * \brief - start bist run
 *
 * \param [in] unit - unit number
 * \param [in] hbm_index - hbm index
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
shr_error_e dnx_hbmc_bist_start(
    int unit,
    int hbm_index,
    int channel);

/**
 * \brief - stop bist run, usually used in infinite runs
 *
 * \param [in] unit - unit number
 * \param [in] hbm_index - hbm index
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
shr_error_e dnx_hbmc_bist_stop(
    int unit,
    int hbm_index,
    int channel);

/**
 * \brief - poll for bist run ending, provides indication that a bist run has ended (not used for infinite runs obviously...)
 *
 * \param [in] unit - unit number
 * \param [in] hbm_index - hbm index
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
shr_error_e dnx_hbmc_bist_run_done_poll(
    int unit,
    int hbm_index,
    int channel);

/**
 * \brief - perform soft init on channel
 *
 * \param [in] unit - unit number
 * \param [in] hbm_ndx - hbm index
 * \param [in] channel - channel
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * soft init will NOT change configuration registers
 * \see
 *   * None
 */
shr_error_e dnx_hbmc_channel_soft_init(
    int unit,
    int hbm_ndx,
    int channel);

/**
 * \brief - set soft init on all channels of hbm index provided
 *
 * \param [in] unit - unit number
 * \param [in] hbm_index - hbm index
 * \param [in] in_soft_init - in/out of soft init
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * soft init will NOT change configuration registers
 * \see
 *   * None
 */
shr_error_e dnx_hbmc_channels_soft_init_set(
    int unit,
    int hbm_index,
    int in_soft_init);

/**
 * \brief - set soft reset on all channels of hbm index provided
 *
 * \param [in] unit - unit number
 * \param [in] hbm_index - hbm index
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
shr_error_e dnx_hbmc_channels_soft_reset_set(
    int unit,
    int hbm_index,
    int in_soft_reset);

/**
 * \brief - set soft init on HBMC block of hbm index provided
 *
 * \param [in] unit - unit number
 * \param [in] hbm_index - hbm index
 * \param [in] in_soft_init - in/out of soft init
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * soft init will NOT change configuration registers
 * \see
 *   * None
 */
shr_error_e dnx_hbmc_hbmc_soft_init_set(
    int unit,
    int hbm_index,
    int in_soft_init);

/**
 * \brief - set soft reset on HBMC block of hbm index provided
 *
 * \param [in] unit - unit number
 * \param [in] hbm_index - hbm index
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
shr_error_e dnx_hbmc_hbmc_soft_reset_set(
    int unit,
    int hbm_index,
    int in_soft_reset);

/**
 * \brief - set soft init on HRC block of hbm index provided
 *
 * \param [in] unit - unit number
 * \param [in] hbm_index - hbm index
 * \param [in] in_soft_init - in/out of soft init
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * soft init will NOT change configuration registers
 * \see
 *   * None
 */
shr_error_e dnx_hbmc_hrc_soft_init_set(
    int unit,
    int hbm_index,
    int in_soft_init);

/**
 * \brief - set soft reset on HRC block of hbm index provided
 *
 * \param [in] unit - unit number
 * \param [in] hbm_index - hbm index
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
shr_error_e dnx_hbmc_hrc_soft_reset_set(
    int unit,
    int hbm_index,
    int in_soft_reset);

/**
 * \brief - set soft init on TDU block of hbm index provided
 *
 * \param [in] unit - unit number
 * \param [in] hbm_index - hbm index
 * \param [in] in_soft_init - in/out of soft init
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * soft init will NOT change configuration registers
 * \see
 *   * None
 */
shr_error_e dnx_hbmc_tdu_soft_init_set(
    int unit,
    int hbm_index,
    int in_soft_init);

/**
 * \brief - set soft reset on TDU block of hbm index provided
 *
 * \param [in] unit - unit number
 * \param [in] hbm_index - hbm index
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
shr_error_e dnx_hbmc_tdu_soft_reset_set(
    int unit,
    int hbm_index,
    int in_soft_reset);

/**
 * \brief - Save Mode Registers values.
 *   Due to HW overlook soft init also clears the MRs in the controller side,
 *   so after soft init need to write them back.
 *   We need to write those MRs only to the controller, and not to the HBM,
 *   since they are already updated there.
 *
 * \param [in] unit - unit number
 * \param [out] mr_values_p - save MRs values to this struct
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_hbmc_soft_init_mrs_save(
    int unit,
    dnx_hbmc_mrs_values_t * mr_values_p);

/**
 * \brief - Restore Mode Registers values.
 *   Due to HW overlook soft init also clears the MRs in the controller side,
 *   so after soft init need to write them back.
 *   We need to write those MRs only to the controller, and not to the HBM,
 *   since they are already updated there.
 *
 * \param [in] unit - unit number
 * \param [in] mr_values_p - restore MRs values from this struct
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_hbmc_soft_init_mrs_restore(
    int unit,
    dnx_hbmc_mrs_values_t * mr_values_p);

/**
 * \brief - indirect access read from dram physical address
 *
 * \param [in] unit - unit number
 * \param [in] hbm_index - hbm_index
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
shr_error_e dnx_hbmc_ind_access_physical_address_read(
    int unit,
    uint32 hbm_index,
    uint32 channel,
    uint32 bank,
    uint32 row,
    uint32 column,
    uint32 *data);

/**
 * \brief - indirect access write to dram physical address
 *
 * \param [in] unit - unit number
 * \param [in] hbm_index - hbm_index
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
shr_error_e dnx_hbmc_ind_access_physical_address_write(
    int unit,
    uint32 hbm_index,
    uint32 channel,
    uint32 bank,
    uint32 row,
    uint32 column,
    uint32 *pattern);

/**
 * \brief - indirect access read from logical dram buffer
 *
 * \param [in] unit - unit number
 * \param [in] core - read logical buffer that is associated to this core
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
shr_error_e dnx_hbmc_ind_access_logical_buffer_read(
    int unit,
    int core,
    int buffer,
    int index,
    uint32 *data);

/**
 * \brief - indirect access write to logical dram buffer
 *
 * \param [in] unit - unit number
 * \param [in] core - write logical buffer that is associated to this core
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
shr_error_e dnx_hbmc_ind_access_logical_buffer_write(
    int unit,
    int core,
    int buffer,
    int index,
    uint32 *pattern);

/**
 * \brief -
 *
 * \param [in] unit - unit number
 * \param [in] enable - enable traffic redirection to OCB
 * \param [out] hbm_is_empty - indication that hbm is now empty, relevant only for enabling redirection
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   *  when traffic redirection to OCB is disabled, hbm_is_empty parameter is not relevant
 *      and will be set to TRUE
 * \see
 *   * None
 */
shr_error_e dnx_hbmc_redirect_traffic_to_ocb(
    int unit,
    uint32 enable,
    uint8 *hbm_is_empty);

/**
 * \brief -
 *
 * \param [in] unit - unit number
 * \param [out] allow_traffic_to_dram - returned indication if traffic is allowed into dram
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_hbmc_is_traffic_allowed_into_dram(
    int unit,
    uint8 *allow_traffic_to_dram);

/**
 * \brief - get bist run status
 *
 * \param [in] unit - unit number
 * \param [in] hbm_index - hbm index
 * \param [in] channel - channel
 * \param [out] read_cmd_counter - number of read commands executed
 * \param [out] write_cmd_counter - number of write commands executed
 * \param [out] read_data_counter - number of data read
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_hbmc_bist_status_get(
    int unit,
    int hbm_index,
    int channel,
    uint32 *read_cmd_counter,
    uint32 *write_cmd_counter,
    uint32 *read_data_counter);

/**
 * \brief - Implementation of API bcm_switch_dram_power_down_cb_register
 *
 * \param [in] unit - unit number
 * \param [in] flags - NONE
 * \param [in] callback - pointer to callback function
 * \param [in] userdata - pointer to the user data. NULL if not exist.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_hbmc_power_down_cb_register(
    int unit,
    uint32 flags,
    bcm_switch_dram_power_down_callback_t callback,
    void *userdata);

/**
 * \brief - implementation of API bcm_switch_dram_power_down_cb_unregister
 *
 * \param [in] unit - unit number
 * \param [in] callback - pointer to callback function
 * \param [in] userdata - pointer to the user data. NULL if not exist.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_hbmc_power_down_cb_unregister(
    int unit,
    bcm_switch_dram_power_down_callback_t callback,
    void *userdata);

/**
 * \brief - dram power protection. WHen HBM temperature exceed the treshold, need to power down the hbm.
 * This function handle the power down procedure
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
shr_error_e dnx_hbmc_power_down(
    int unit);

/**
 * \brief - HBM re-init after power up, which was made after power down. (part of temperature monitor routine).
 *
 * \param [in] unit - unit number
 * \param [in] flags - currently not is use
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_hbmc_reinit(
    int unit,
    uint32 flags);

/**
 * \brief - get HBM temperature
 *
 * \param [in] unit - unit number
 * \param [in] interface_id - hbm_index or (-1) for All.
 * \param [out] value - temperature value.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_hbmc_temp_get(
    int unit,
    int interface_id,
    int *value);

/**
 * \brief - set HBM vendor info to SW-State
 *
 * \param [in] unit - unit number
 * \param [in] hbm_index - hbm_index
 * \param [in] vendor_info - hbm vendor info to update to SW-State
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_hbmc_vendor_info_to_sw_state_set(
    int unit,
    int hbm_index,
    hbm_dram_vendor_info_hbm_t * vendor_info);

/**
 * \brief - get HBM vendor info from SW-State
 *
 * \param [in] unit - unit number
 * \param [in] hbm_index - hbm_index
 * \param [out] vendor_info - retrieved hbm vendor info from SW-State
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_hbmc_vendor_info_from_sw_state_get(
    int unit,
    int hbm_index,
    hbm_dram_vendor_info_hbm_t * vendor_info);

/**
 * \brief - decode device ID read into meaningful fields according to JEDEC spec
 *
 * \param [in] unit - unit number
 * \param [in] device_id - Raw read of device data
 * \param [out] vendor_info - parsed vendor info.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_hbmc_device_id_to_vendor_info_decode(
    int unit,
    soc_dnx_hbm_device_id_t * device_id,
    hbm_dram_vendor_info_hbm_t * vendor_info);

/**
 * \brief - run read LFSR loopback BIST
 *
 * \param [in] unit - unit number
 * \param [in] hbm_index - hbm index
 * \param [in] channel - hbm channel.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_hbmc_read_lfsr_loopback_bist_run(
    int unit,
    uint32 hbm_index,
    uint32 channel);
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
shr_error_e dnx_hbmc_hbc_blocks_enable_set(
    int unit);

/*
 * }
 */
#endif /* _BCMINT_DNX_DRAM_HBMC_HBMC_H_INCLUDED */
