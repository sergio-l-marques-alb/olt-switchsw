/** \file src/bcm/dnx/dram/hbmc/hbmc_dbal_access.h
 *
 * Internal DNX hbmc dbal access APIs to be used in hbmc
 * module
 *
 */

/*
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef _BCM_DNX_DRAM_HBMC_HBMCDBALACCESS_H_INCLUDED
/*
 * {
 */
#define _BCM_DNX_DRAM_HBMC_HBMCDBALACCESS_H_INCLUDED

/**
 * \brief This file is only used by DNX (JR2 family). Including it by
 * software that is not specific to DNX is an error.
 */
#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <shared/shrextend/shrextend_debug.h>
#include <soc/hbmc_shmoo.h>

/**
 * \brief - transform hbm_index and channel_in_hbm coordinates to a single sequential coordinate
 *
 * \param [in] unit - unit number
 * \param [in] hbm_index - hbm index
 * \param [in] channel_in_hbm - channel index in hbm
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
int dnx_hbmc_get_sequential_channel(
    int unit,
    int hbm_index,
    int channel_in_hbm);

/**
 * \brief - set phy register according to hbm index, channel and address
 *
 * \param [in] unit - unit number
 * \param [in] hbm_index - hbm index
 * \param [in] channel - channel in hbm
 * \param [in] address - address of phy register
 * \param [in] data - data to write
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_hbmc_dbal_access_phy_channel_register_set(
    int unit,
    int hbm_index,
    int channel,
    uint32 address,
    uint32 data);

/**
 * \brief - get phy register according to hbm index, channel and address
 *
 * \param [in] unit - unit number
 * \param [in] hbm_index - hbm index
 * \param [in] channel - channel in hbm
 * \param [in] address - address of phy register
 * \param [out] data - data read
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_hbmc_dbal_access_phy_channel_register_get(
    int unit,
    int hbm_index,
    int channel,
    uint32 address,
    uint32 *data);

/**
 * \brief - set phy register according to hbm index and address - midstack phy register
 *
 * \param [in] unit - unit number
 * \param [in] hbm_index - hbm index
 * \param [in] address - address of phy register
 * \param [in] data - data to write
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_hbmc_dbal_access_phy_midstack_register_set(
    int unit,
    int hbm_index,
    uint32 address,
    uint32 data);

/**
 * \brief - get phy register according to hbm index and address - midstack phy register
 *
 * \param [in] unit - unit number
 * \param [in] hbm_index - hbm index
 * \param [in] address - address of phy register
 * \param [out] data - data read
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_hbmc_dbal_access_phy_midstack_register_get(
    int unit,
    int hbm_index,
    uint32 address,
    uint32 *data);

/**
 * \brief - set bist configuration
 *
 * \param [in] unit - unit number
 * \param [in] hbm_index - hbm index
 * \param [in] channel - channel
 * \param [in] info - info to set
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_hbmc_dbal_access_bist_configuration_set(
    int unit,
    int hbm_index,
    int channel,
    const hbmc_shmoo_bist_info_t * info);

/**
 * \brief - get bist configuration
 *
 * \param [in] unit - unit number
 * \param [in] hbm_index - hbm index
 * \param [in] channel - channel
 * \param [out] info - info to get
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_hbmc_dbal_access_bist_configuration_get(
    int unit,
    int hbm_index,
    int channel,
    hbmc_shmoo_bist_info_t * info);

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
shr_error_e dnx_hbmc_dbal_access_bist_status_get(
    int unit,
    int hbm_index,
    int channel,
    uint32 *read_cmd_counter,
    uint32 *write_cmd_counter,
    uint32 *read_data_counter);

/**
 * \brief - start a bist run
 *
 * \param [in] unit - unit number
 * \param [in] hbm_index - hbm index
 * \param [in] channel - channel
 * \param [in] start - bist start
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_hbmc_dbal_access_bist_run(
    int unit,
    int hbm_index,
    int channel,
    uint32 start);

/**
 * \brief - poll for bist done indication
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
shr_error_e dnx_hbmc_dbal_access_bist_run_done_poll(
    int unit,
    int hbm_index,
    int channel);

/**
 * \brief - get bist error counters when running in write LFSR mode.
 *
 * \param [in] unit - unit number
 * \param [in] hbm_index - hbm index
 * \param [in] channel - channel
 * \param [out] error_counters - error counters
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_hbmc_dbal_access_bist_write_lfsr_error_counters_get(
    int unit,
    int hbm_index,
    int channel,
    hbmc_shmoo_bist_err_cnt_t * error_counters);

/**
 * \brief - get bist error counters
 *
 * \param [in] unit - unit number
 * \param [in] hbm_index - hbm index
 * \param [in] channel - channel
 * \param [out] error_counters - error counters
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_hbmc_dbal_access_bist_error_counters_get(
    int unit,
    int hbm_index,
    int channel,
    hbmc_shmoo_bist_err_cnt_t * error_counters);

/**
 * \brief - set per channel soft init
 *
 * \param [in] unit - unit number
 * \param [in] hbm_index - hbm index
 * \param [in] channel - channel
 * \param [out] in_soft_init - put channel in/out soft init (1/0)
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_hbmc_dbal_access_channel_soft_init_set(
    int unit,
    int hbm_index,
    int channel,
    uint32 in_soft_init);

/**
 * \brief - set per hbm index channels' soft reset
 *
 * \param [in] unit - unit number
 * \param [in] hbm_index - hbm index
 * \param [out] in_soft_reset - put channel in/out soft reset (1/0)
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_hbmc_dbal_access_channels_soft_reset_set(
    int unit,
    int hbm_index,
    uint32 in_soft_reset);

/**
 * \brief - set per hbm index channels' soft init
 *
 * \param [in] unit - unit number
 * \param [in] hbm_index - hbm index
 * \param [out] in_soft_init - put channel in/out soft init (1/0)
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_hbmc_dbal_access_channels_soft_init_set(
    int unit,
    int hbm_index,
    uint32 in_soft_init);

/**
 * \brief - get per channel soft init
 *
 * \param [in] unit - unit number
 * \param [in] hbm_index - hbm index
 * \param [in] channel - channel
 * \param [out] in_soft_init - get channel in/out soft init (1/0) state
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_hbmc_dbal_access_channel_soft_init_get(
    int unit,
    int hbm_index,
    int channel,
    uint32 *in_soft_init);

/**
 * \brief - set per hbm index HBMC soft reset
 *
 * \param [in] unit - unit number
 * \param [in] hbm_index - hbm index
 * \param [in] in_soft_reset - put HBMC block in/out soft reset (1/0)
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_hbmc_dbal_access_hbmc_soft_reset_set(
    int unit,
    int hbm_index,
    uint32 in_soft_reset);

/**
 * \brief - set per hbm index HBMC soft init
 *
 * \param [in] unit - unit number
 * \param [in] hbm_index - hbm index
 * \param [in] in_soft_init - put HBMC block in/out soft init (1/0)
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_hbmc_dbal_access_hbmc_soft_init_set(
    int unit,
    int hbm_index,
    uint32 in_soft_init);

/**
 * \brief - set per hbm index HRC soft reset
 *
 * \param [in] unit - unit number
 * \param [in] hbm_index - hbm index
 * \param [in] in_soft_reset - put HRC block in/out soft reset (1/0)
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_hbmc_dbal_access_hrc_soft_reset_set(
    int unit,
    int hbm_index,
    uint32 in_soft_reset);

/**
 * \brief - set per hbm index HRC soft init
 *
 * \param [in] unit - unit number
 * \param [in] hbm_index - hbm index
 * \param [in] in_soft_init - put HRC block in/out soft init (1/0)
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_hbmc_dbal_access_hrc_soft_init_set(
    int unit,
    int hbm_index,
    uint32 in_soft_init);

/**
 * \brief - set per hbm index TDU soft reset
 *
 * \param [in] unit - unit number
 * \param [in] hbm_index - hbm index
 * \param [in] in_soft_reset - put TDU blocks in/out soft reset (1/0)
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_hbmc_dbal_access_tdu_soft_reset_set(
    int unit,
    int hbm_index,
    uint32 in_soft_reset);

/**
 * \brief - set per hbm index TDU soft init
 *
 * \param [in] unit - unit number
 * \param [in] hbm_index - hbm index
 * \param [in] in_soft_init - put TDU blocks in/out soft init (1/0)
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_hbmc_dbal_access_tdu_soft_init_set(
    int unit,
    int hbm_index,
    uint32 in_soft_init);

/**
 * \brief - get enable refresh state
 *
 * \param [in] unit - unit number
 * \param [in] hbm_index - hbm index
 * \param [in] channel - channel
 * \param [out] enable_refresh - enable refresh state
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_hbmc_dbal_access_enable_refresh_get(
    int unit,
    int hbm_index,
    int channel,
    uint32 *enable_refresh);

/**
 * \brief - set enable refresh state
 *
 * \param [in] unit - unit number
 * \param [in] hbm_index - hbm index
 * \param [in] channel - channel
 * \param [in] enable_refresh - enable refresh state
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_hbmc_dbal_access_enable_refresh_set(
    int unit,
    int hbm_index,
    int channel,
    uint32 enable_refresh);

/**
 * \brief - set pll configuration
 *
 * \param [in] unit - unit number
 * \param [in] hbm_index - hbm index
 * \param [in] pll_info - pll info
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_hbmc_dbal_access_pll_config_set(
    int unit,
    int hbm_index,
    const hbmc_shmoo_pll_t * pll_info);

/**
 * \brief - get pll configuration
 *
 * \param [in] unit - unit number
 * \param [in] hbm_index - hbm index
 * \param [out] pll_info - pll info
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_hbmc_dbal_access_pll_config_get(
    int unit,
    int hbm_index,
    hbmc_shmoo_pll_t * pll_info);

/**
 * \brief - Configure PLL for APD PHY
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
shr_error_e dnx_hbmc_dbal_access_pll_configuration_for_apd_phy(
    int unit,
    int hbm_index);

/**
 * \brief - set pll control
 *
 * \param [in] unit - unit number
 * \param [in] hbm_index - hbm index
 * \param [in] reset - set in/out reset
 * \param [in] post_reset - set in/out post_reset
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_hbmc_dbal_access_pll_control_set(
    int unit,
    int hbm_index,
    uint32 reset,
    uint32 post_reset);

/**
 * \brief - poll for locked status in PLL corresponding to hbm_index
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
shr_error_e dnx_hbmc_dbal_access_pll_status_locked_poll(
    int unit,
    int hbm_index);

/**
 * \brief - take phy out of reset
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
shr_error_e dnx_hbmc_dbal_access_phy_control_out_of_reset_config(
    int unit,
    int hbm_index);

/**
 * \brief - configures all phy channels and midstack of given hbm index to be out of reset
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
shr_error_e dnx_hbmc_dbal_access_phy_channel_out_of_reset_config(
    int unit,
    int hbm_index);

/**
 * \brief - enable dynamic memory access to all tables under HBMC block for provided hbm index
 *
 * \param [in] unit - unit number
 * \param [in] hbm_index - hbm index
 * \param [in] enable - enable/disable 1/0
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_hbmc_dbal_access_hbmc_dynamic_memory_access_set(
    int unit,
    int hbm_index,
    int enable);

/**
 * \brief - enable dynamic memory access to all tables under HCC block for provided hbm index
 *
 * \param [in] unit - unit number
 * \param [in] hbm_index - hbm index
 * \param [in] enable - enable/disable 1/0
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_hbmc_dbal_access_hcc_dynamic_memory_access_set(
    int unit,
    int hbm_index,
    int enable);

/**
 * \brief - enable dynamic memory access to all tables under hbmc and hcc blocks for provided hbm index
 *
 * \param [in] unit - unit number
 * \param [in] hbm_index - hbm index
 * \param [in] enable - enable/disable 1/0
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_hbmc_dbal_access_dynamic_memory_access_set(
    int unit,
    int hbm_index,
    int enable);

/**
 * \brief - get state of HBM ECC and DBI read/write (enabled/disabled)
 *
 * \param [in] unit - unit number
 * \param [in] hbm_index - hbm index
 * \param [in] channel - channel index
 * \param [out] ecc - value of HBM ecc field
 * \param [out] dbi_read - value of HBM DBI read field
 * \param [out] dbi_write - value of HBM DBI write field
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_hbmc_dbal_access_hcc_configuration_ecc_dbi_get(
    int unit,
    int hbm_index,
    int channel,
    uint32 *ecc,
    uint32 *dbi_read,
    uint32 *dbi_write);

/**
 * \brief - set state of HBM ECC and DBI read/write (enabled/disabled)
 *
 * \param [in] unit - unit number
 * \param [in] hbm_index - hbm index
 * \param [in] channel - channel index
 * \param [in] ecc - enable/disable HBM ecc
 * \param [in] dbi_read - enable/disable HBM DBI read
 * \param [in] dbi_write - enable/disable HBM DBI write
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_hbmc_dbal_access_hcc_configuration_ecc_dbi_set(
    int unit,
    int hbm_index,
    int channel,
    uint32 ecc,
    uint32 dbi_read,
    uint32 dbi_write);

/**
 * \brief - configure HCC HBM tables
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
shr_error_e dnx_hbmc_dbal_access_hcc_hbm_config(
    int unit,
    int hbm_index);

/**
 * \brief - configure HCC Timing State Machines tables
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
shr_error_e dnx_hbmc_dbal_access_hcc_tsm_config(
    int unit,
    int hbm_index);

/**
 * \brief - configure HCC data source tables
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
shr_error_e dnx_hbmc_dbal_access_hcc_data_source_config(
    int unit,
    int hbm_index);

/**
 * \brief - Enable/disable HCC data path tables
 *
 * \param [in] unit - unit number
 * \param [in] hbm_index - hbm index
 * \param [in] enable - enable/disable data path
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_hbmc_dbal_access_hcc_data_path_enable_set(
    int unit,
    int hbm_index,
    uint32 enable);

/**
 * \brief - Enable/disable HCC request path tables
 *
 * \param [in] unit - unit number
 * \param [in] hbm_index - hbm index
 * \param [in] enable - enable/disable request path
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_hbmc_dbal_access_hcc_request_path_enable_set(
    int unit,
    int hbm_index,
    uint32 enable);

/**
 * \brief - Enable/disable HRC path tables
 *
 * \param [in] unit - unit number
 * \param [in] hbm_index - hbm index
 * \param [in] enable - enable/disable HRC path
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_hbmc_dbal_access_hrc_enable_set(
    int unit,
    int hbm_index,
    uint32 enable);

/**
 * \brief - enable/disable controller's ECC per channel
 *
 * \param [in] unit - unit number
 * \param [in] hbm_index - hbm index
 * \param [in] channel - channel
 * \param [in] enable - enable/disable ecc
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_hbmc_dbal_access_ecc_enable(
    int unit,
    int hbm_index,
    int channel,
    uint32 enable);

/**
 * \brief - mask/unmask CATTRIP indication
 *
 * \param [in] unit - unit number
 * \param [in] hbm_index - hbm index
 * \param [in] mask_cattrip_indication - use 1 to mask CATTRIP indication, use 0 to unmask
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_hbmc_dbal_access_cattrip_indication_mask_set(
    int unit,
    int hbm_index,
    uint32 mask_cattrip_indication);

/**
 * \brief - put hbmc in/out of reset
 *
 * \param [in] unit - unit number
 * \param [in] hbm_index - hbm index
 * \param [in] out_of_reset - out of reset/ in reset 1/0
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_hbmc_dbal_access_reset_control_set(
    int unit,
    int hbm_index,
    uint32 out_of_reset);

/**
 * \brief - clear cattrip interrupt
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
shr_error_e dnx_hbmc_dbal_access_cattrip_interrupt_clear(
    int unit,
    int hbm_index);

/**
 * \brief - set output enable
 *
 * \param [in] unit - unit number
 * \param [in] hbm_index - hbm index
 * \param [in] output_enable - output enable
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_hbmc_dbal_access_output_enable_set(
    int unit,
    int hbm_index,
    uint32 output_enable);

/**
 * \brief - set signals to channels clocks
 *
 * \param [in] unit - unit number
 * \param [in] hbm_index - hbm index
 * \param [in] ck_ui_0
 * \param [in] ck_ui_1
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_hbmc_dbal_access_clocks_set(
    int unit,
    int hbm_index,
    uint32 ck_ui_0,
    uint32 ck_ui_1);

/**
 * \brief - set output enable to channels clocks
 *
 * \param [in] unit - unit number
 * \param [in] hbm_index - hbm index
 * \param [in] ck_oen_ui_0
 * \param [in] ck_oen_ui_1
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_hbmc_dbal_access_clocks_output_enable_set(
    int unit,
    int hbm_index,
    uint32 ck_oen_ui_0,
    uint32 ck_oen_ui_1);

/**
 * \brief - set mode register
 *
 * \param [in] unit - unit number
 * \param [in] hbm_index - hbm index
 * \param [in] channel - channel
 * \param [in] mr_index - mode register index
 * \param [in] value - value to set
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_hbmc_dbal_access_mr_set(
    int unit,
    uint32 hbm_index,
    uint32 channel,
    uint32 mr_index,
    uint32 value);

/**
 * \brief - get mode register
 *
 * \param [in] unit - unit number
 * \param [in] hbm_index - hbm index
 * \param [in] channel - channel
 * \param [in] mr_index - mode register index
 * \param [out] value - value
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_hbmc_dbal_access_mr_get(
    int unit,
    uint32 hbm_index,
    uint32 channel,
    uint32 mr_index,
    uint32 *value);

/**
 * \brief - init mr with values - to the resolution of a single channel
 *
 * \param [in] unit - unit number
 * \param [in] hbm_index - hbm index
 * \param [in] channel_index - channel index
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_hbmc_dbal_access_mr_init_channel_set(
    int unit,
    int hbm_index,
    int channel_index);

/**
 * \brief - init mr with values - to the resolution of a single hbm (all its channels)
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
shr_error_e dnx_hbmc_dbal_access_mr_init_set(
    int unit,
    int hbm_index);

/**
 * \brief - set whether or not to update the HBM upon mode register changes
 *
 * \param [in] unit - unit number
 * \param [in] hbm_index - hbm index. use DNX_HBMC_ITER_ALL to update all hbms.
 * \param [in] channel - channel index. use DNX_HBMC_ITER_ALL to update all channels in the hbm.
 * \param [in] update - 1 - update hbm on mode register change or not, 0 - don't update.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_hbmc_dbal_access_update_hbm_on_mode_register_change(
    int unit,
    int hbm_index,
    int channel,
    int update);

/**
 * \brief - set cke and wdqs
 *
 * \param [in] unit - unit number
 * \param [in] hbm_index - hbm index
 * \param [in] cke_ui - cke_ui
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_hbmc_dbal_access_cke_set(
    int unit,
    int hbm_index,
    uint32 cke_ui);

/**
 * \brief - get data source id
 *
 * \param [in] unit - unit number
 * \param [in] core - core number
 * \param [out] data_source_id - data source id
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_hbmc_dbal_access_data_source_id_get(
    int unit,
    int core,
    uint32 *data_source_id);

/**
 * \brief - configure tdu
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
shr_error_e dnx_hbmc_dbal_access_tdu_configure(
    int unit);

/**
 * \brief - configure address translation matrix
 *
 * \param [in] unit - unit number
 * \param [in] atm - address translation matrix
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_hbmc_dbal_access_tdu_atm_configure(
    int unit,
    const uint32 *atm);

/**
 * \brief - read dram physical address
 *
 * \param [in] unit - unit number
 * \param [in] module - hbm module
 * \param [in] channel - channel in hbm
 * \param [in] bank - bank
 * \param [in] row - row
 * \param [in] column - column
 * \param [out] data - read data
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_hbmc_dbal_access_dram_cpu_access_get(
    int unit,
    uint32 module,
    uint32 channel,
    uint32 bank,
    uint32 row,
    uint32 column,
    uint32 *data);

/**
 * \brief - write dram physical address
 *
 * \param [in] unit - unit number
 * \param [in] module - hbm module
 * \param [in] channel - channel in hbm
 * \param [in] bank - bank
 * \param [in] row - row
 * \param [in] column - column
 * \param [out] pattern - pattern to write
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_hbmc_dbal_access_dram_cpu_access_set(
    int unit,
    uint32 module,
    uint32 channel,
    uint32 bank,
    uint32 row,
    uint32 column,
    uint32 *pattern);

#endif /* _BCM_DNX_DRAM_HBMC_HBMCDBALACCESS_H_INCLUDED */
