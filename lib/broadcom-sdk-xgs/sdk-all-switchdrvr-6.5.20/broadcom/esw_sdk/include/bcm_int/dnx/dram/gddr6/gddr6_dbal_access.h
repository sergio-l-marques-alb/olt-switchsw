/** \file include/bcm_int/dnx/dram/gddr6/gddr6_dbal_access.h
 *
 * Internal DNX gddr6 dbal access APIs to be used in gddr6
 * module
 *
 */

/*
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef _BCM_DNX_DRAM_GDDR6_GDDR6_DBAL_ACCESS_H_INCLUDED
/*
 * {
 */
#define _BCM_DNX_DRAM_GDDR6_GDDR6_DBAL_ACCESS_H_INCLUDED

/**
 * \brief This file is only used by DNX (JR2 family). Including it by
 * software that is not specific to DNX is an error.
 */
#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif

#include <shared/shrextend/shrextend_debug.h>
#include <soc/shmoo_g6phy16.h>

/**
 * \brief - transform dram_index and channel_in_dram coordinates to a single sequential coordinate
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [in] channel_in_dram - channel index in dram
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
int dnx_gddr6_get_sequential_channel(
    int unit,
    int dram_index,
    int channel_in_dram);

/**
 * \brief - set phy register according to dram index, channel and address
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
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
shr_error_e dnx_gddr6_dbal_access_phy_channel_register_set(
    int unit,
    int dram_index,
    uint32 address,
    uint32 data);

/**
 * \brief - get phy register according to dram index, channel and address
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
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
shr_error_e dnx_gddr6_dbal_access_phy_channel_register_get(
    int unit,
    int dram_index,
    uint32 address,
    uint32 *data);
/**
 * \brief - set TSM bank
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [in] disable - disable all tsm banks of the dram
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_gddr6_dbal_access_tsm_banks_disable_set(
    int unit,
    int dram_index,
    uint32 disable);

/**
 * \brief - start a bist run
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
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
shr_error_e dnx_gddr6_dbal_access_bist_run(
    int unit,
    int dram_index,
    int channel,
    uint32 start);
/**
 * \brief - start a training bist run
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
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
shr_error_e dnx_gddr6_dbal_access_training_bist_run(
    int unit,
    int dram_index,
    uint32 start);

/**
 * \brief - poll for bist done indication
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
shr_error_e dnx_gddr6_dbal_access_bist_run_done_poll(
    int unit,
    int dram_index,
    int channel);
/**
 * \brief - poll for training bist done indication
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
shr_error_e dnx_gddr6_dbal_access_training_bist_run_done_poll(
    int unit,
    int dram_index);

/**
 * \brief - set per channel soft init
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
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
shr_error_e dnx_gddr6_dbal_access_channel_soft_init_set(
    int unit,
    int dram_index,
    int channel,
    uint32 in_soft_init);

/**
 * \brief - set per dram index channels' soft reset
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
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
shr_error_e dnx_gddr6_dbal_access_channels_soft_reset_set(
    int unit,
    int dram_index,
    uint32 in_soft_reset);

/**
 * \brief - set per dram index channels' soft init
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
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
shr_error_e dnx_gddr6_dbal_access_channels_soft_init_set(
    int unit,
    int dram_index,
    uint32 in_soft_init);

/**
 * \brief - get enable refresh state
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [in] channel - channel
 * \param [out] enable_refresh - enable refresh state
 * \param [out] enable_refresh_ab - enable TSM all banks refresh state
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_gddr6_dbal_access_enable_refresh_get(
    int unit,
    int dram_index,
    int channel,
    uint32 *enable_refresh,
    uint32 *enable_refresh_ab);

/**
 * \brief - set enable refresh state
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [in] channel - channel
 * \param [in] enable_refresh - enable refresh state
 * \param [in] enable_refresh_ab - enable TSM refresh all bank refresh state
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_gddr6_dbal_access_enable_refresh_set(
    int unit,
    int dram_index,
    int channel,
    uint32 enable_refresh,
    uint32 enable_refresh_ab);
/**
 * \brief -pll reset
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [in] pll_in_reset - pll in reset state
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_gddr6_dbal_access_pll_reset(
    int unit,
    int dram_index,
    uint32 pll_in_reset);
/**
 * \brief -pll reset
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [in] pll_post_reset - pll post reset state
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_gddr6_dbal_access_pll_post_reset_set(
    int unit,
    int dram_index,
    uint32 pll_post_reset);

/**
 * \brief - set pll configuration
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
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
shr_error_e dnx_gddr6_dbal_access_pll_config_set(
    int unit,
    int dram_index,
    const g6phy16_drc_pll_t * pll_info);

/**
 * \brief - get pll configuration
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
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
shr_error_e dnx_gddr6_dbal_access_pll_config_get(
    int unit,
    int dram_index,
    const g6phy16_drc_pll_t * pll_info);
/**
 * \brief - pll DIV config set
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
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
shr_error_e dnx_gddr6_dbal_access_pll_div_config_set(
    int unit,
    int dram_index,
    const g6phy16_drc_pll_t * pll_info);
/**
 * \brief - pll init
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
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
shr_error_e dnx_gddr6_dbal_access_pll_init(
    int unit,
    int dram_index,
    const g6phy16_drc_pll_t * pll_info);
/**
 * \brief - poll for locked status in PLL corresponding to dram_index
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [in] pll_locked -   pll locked
 * \param [in] pll_stat_out - pll stat out
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_gddr6_dbal_access_pll_status_get(
    int unit,
    int dram_index,
    uint32 *pll_locked,
    uint32 *pll_stat_out);

/**
 * \brief - poll for locked status in PLL corresponding to dram_index
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
shr_error_e dnx_gddr6_dbal_access_pll_status_locked_poll(
    int unit,
    int dram_index);
/**
 * \brief - gddr6 phy iddq set
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [in] iddq - iddq value
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_gddr6_dbal_access_phy_iddq_set(
    int unit,
    int dram_index,
    uint32 iddq);
/**
 * \brief - gddr6 pll reference clock select
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [in] pll_clk_sel - pll clock
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_gddr6_dbal_access_pll_refclk_sel(
    int unit,
    int dram_index,
    uint32 pll_clk_sel);

/**
 * \brief - configures all phy channels of given dram index to be out of reset
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [in] out_of_reset - out of reset
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_gddr6_dbal_access_phy_out_of_reset_config(
    int unit,
    int dram_index,
    int out_of_reset);
/**
 * \brief - write fifo enable configure
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [in] write_fifo_enable - write fifo enable
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_gddr6_dbal_access_phy_write_fifo_enable_config(
    int unit,
    int dram_index,
    uint32 write_fifo_enable);
/**
 * \brief - gddr6 phy ilm mode configure
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [in] ilm_mode - ilm mode
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_gddr6_dbal_access_phy_ilm_mode_config(
    int unit,
    int dram_index,
    uint32 ilm_mode);

/**
 * \brief - enable dynamic memory access to all tables under dramc and hcc blocks for provided dram index
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
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
shr_error_e dnx_gddr6_dbal_access_dynamic_memory_access_set(
    int unit,
    int dram_index,
    int enable);

/**
 * \brief - configure gddr6 controller
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - gddr6 dram index
 * \param [in] ignore_vendor - ignore vendor info
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * the ignore vendor option is used when vendor info cannot be retrieved yet, but parts of the configurations are required to get the vendor info.
 * \see
 *   * None
 */
shr_error_e dnx_gddr6_dbal_access_dram_controller_config(
    int unit,
    uint32 dram_index,
    int ignore_vendor);
/**
 * \brief - gddr6 dram reset
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [in] out_of_reset - out of reset
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_gddr6_dbal_access_dram_reset_config(
    int unit,
    int dram_index,
    uint32 out_of_reset);

/**
 * \brief - set mode register
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
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
shr_error_e dnx_gddr6_dbal_access_mr_set(
    int unit,
    uint32 dram_index,
    uint32 mr_index,
    uint32 value);

/**
 * \brief - get mode register
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
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
shr_error_e dnx_gddr6_dbal_access_channel_mr_get(
    int unit,
    uint32 dram_index,
    uint32 channel,
    uint32 mr_index,
    uint32 *value);
/**
 * \brief - set mode register
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
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
shr_error_e dnx_gddr6_dbal_access_channel_mr_set(
    int unit,
    uint32 dram_index,
    uint32 channel,
    uint32 mr_index,
    uint32 value);

/**
 * \brief - cpu command
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [in] ca - command address
 * \param [in] cabi_n - command address bus inversion
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_gddr6_dbal_access_cpu_command_set(
    int unit,
    int dram_index,
    uint32 ca,
    uint32 cabi_n);

/**
 * \brief -  force constant cpu value set
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [in] ca - command address
 * \param [in] cabi_n - command address bus inversion
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_gddr6_dbal_force_constant_cpu_value_set(
    int unit,
    int dram_index,
    uint32 ca,
    uint32 cabi_n);
/**
 * \brief -  force constant cpu value set
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
shr_error_e dnx_gddr6_dbal_release_constant_cpu_set(
    int unit,
    int dram_index);

/**
 * \brief -  wck set
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [in] wck - wck
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_gddr6_dbal_access_wck_set(
    int unit,
    int dram_index,
    uint32 wck);
/**
 * \brief - set whether or not to update the GDDR6 upon mode register changes
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [in] update - 1 - update dram on mode register change or not, 0 - don't update.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_gddr6_dbal_access_update_dram_on_mode_register_change(
    int unit,
    int dram_index,
    int update);
/**
 * \brief - force update per mode reigster bitmap set
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [in] force - 1 - update dram on mode register change or not, 0 - don't update.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_gddr6_dbal_access_force_update_per_mode_register_bitmap_set(
    int unit,
    int dram_index,
    uint32 force);
/**
 * \brief -mask bitmap for update mode register
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [in] mask - 1 - mask mode register change or not, 0 - don't update.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_gddr6_dbal_access_mask_update_per_mode_register_bitmap_set(
    int unit,
    int dram_index,
    uint32 mask);

/**
 * \brief - set cke and wdqs
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
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
shr_error_e dnx_gddr6_dbal_access_cke_set(
    int unit,
    int dram_index,
    uint32 cke_ui);

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
shr_error_e dnx_gddr6_dbal_access_tdu_configure(
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
shr_error_e dnx_gddr6_dbal_access_tdu_atm_configure(
    int unit,
    const uint32 *atm);

/**
 * \brief - read dram physical address
 *
 * \param [in] unit - unit number
 * \param [in] module - dram module
 * \param [in] channel - channel in dram
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
shr_error_e dnx_gddr6_dbal_access_dram_cpu_access_get(
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
 * \param [in] module - dram module
 * \param [in] channel - channel in dram
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
shr_error_e dnx_gddr6_dbal_access_dram_cpu_access_set(
    int unit,
    uint32 module,
    uint32 channel,
    uint32 bank,
    uint32 row,
    uint32 column,
    uint32 *pattern);
/**
 * \brief - configure command address bitmap
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
shr_error_e dnx_gddr6_dbal_access_ca_bit_map_set(
    int unit,
    uint32 dram_index);
/**
 * \brief - configure DQ  bitmap
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
shr_error_e dnx_gddr6_dbal_access_dq_bit_map_set(
    int unit,
    uint32 dram_index);
/**
 * \brief - configure cadt  byte map
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
shr_error_e dnx_gddr6_dbal_access_cadt_byte_map_set(
    int unit,
    uint32 dram_index);
/**
 * \brief - enable WR CRC
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [in] enable - enable or not
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_gddr6_dbal_access_wr_crc_enable_set(
    int unit,
    uint32 dram_index,
    uint32 enable);
/**
 * \brief - enable RD CRC
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [in] enable - enable or not
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_gddr6_dbal_access_rd_crc_enable_set(
    int unit,
    uint32 dram_index,
    uint32 enable);
/**
 * \brief - enable WR DBI
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [in] enable - enable or not
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_gddr6_dbal_access_wr_dbi_enable_set(
    int unit,
    uint32 dram_index,
    uint32 enable);
/**
 * \brief - enable RD DBI
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [in] enable - enable or not
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_gddr6_dbal_access_rd_dbi_enable_set(
    int unit,
    uint32 dram_index,
    uint32 enable);

/**
 * \brief - get dram error count (CRC errors)
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [in] channel - channel
 * \param [out] dram_err_cnt - error count
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_gddr6_dbal_access_dram_err_cnt_get(
    int unit,
    uint32 dram_index,
    uint32 channel,
    dnx_gddr6_dram_err_cnt_t * dram_err_cnt);

/**
 * \brief - get bist error count
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [in] channel - channel
 * \param [out] bist_err_cnt - error count
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_gddr6_dbal_access_bist_err_cnt_get(
    int unit,
    uint32 dram_index,
    uint32 channel,
    dnx_gddr6_bist_err_cnt_t * bist_err_cnt);

/**
 * \brief - get bist status count
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [in] channel - channel
 * \param [out] bist_status_cnt - status count
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_gddr6_dbal_access_bist_status_cnt_get(
    int unit,
    uint32 dram_index,
    uint32 channel,
    dnx_gddr6_bist_status_cnt_t * bist_status_cnt);

/**
 * \brief - read fifo 'reset not' set
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [in] rstn - reset
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_gddr6_dbal_access_rd_fifo_rstn_set(
    int unit,
    uint32 dram_index,
    uint32 rstn);

/**
 * \brief - read fifo 'reset not' set per channel
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [in] channel - channel
 * \param [in] rstn - reset
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_gddr6_dbal_access_channel_rd_fifo_rstn_set(
    int unit,
    uint32 dram_index,
    int channel,
    uint32 rstn);

/**
 * \brief - clear pipelines interrupts
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
shr_error_e dnx_gddr6_dbal_access_pipelines_interrupts_clear(
    int unit,
    uint32 dram_index,
    uint32 channel);

/**
 * \brief - get pipelines interrupts
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - DRAM index
 * \param [in] channel - channel index
 * \param [out] is_underflow - a FIFO underflow has occurred
 * \param [out] is_overflow  - a FIFO overflow has occurred
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_gddr6_dbal_access_pipelines_interrupts_get(
    int unit,
    uint32 dram_index,
    uint32 channel,
    uint32 *is_underflow,
    uint32 *is_overflow);

/**
 * \brief - data training pattern set
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [in] ldff_pattern - pattern
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_gddr6_dbal_access_ldff_training_pattern_set(
    int unit,
    uint32 dram_index,
    dnx_gddr6_ldff_pattern * ldff_pattern);
/**
 * \brief - command address training pattern set
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [in] cadt_bist - pattern
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_gddr6_dbal_access_cadt_training_pattern_set(
    int unit,
    uint32 dram_index,
    dnx_gddr6_training_cadt_bist_configuration * cadt_bist);

/**
 * \brief - training bist configure
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [in] training_bist_conf - trainig configuration
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_gddr6_dbal_access_cmd_training_bist_configure_set(
    int unit,
    uint32 dram_index,
    dnx_gddr6_training_bist_cmd_configuration * training_bist_conf);

/**
 * \brief - Configure periodic temperatue readout parameters
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [in] temp_readout_config - periodic temperature readout parameters
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_gddr6_dbal_access_periodic_temp_readout_config(
    int unit,
    uint32 dram_index,
    dnx_gddr6_periodic_temp_readout_config_t * temp_readout_config);

/**
 * \brief - Enable/Disable periodic temperatue readout
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [in] channel - channel index
 * \param [in] enable - enable/disable periodic temperature readout
 *                      1 - enable
 *                      0 - disable
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_gddr6_dbal_access_periodic_temp_readout_enable(
    int unit,
    uint32 dram_index,
    uint32 channel,
    int enable);

/**
 * \brief - Read periodic temperatue readout status
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [in] channel - channel index
 * \param [out] temp_readout_status - periodic temperature readout status
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_gddr6_dbal_access_periodic_temp_readout_status_get(
    int unit,
    uint32 dram_index,
    uint32 channel,
    dnx_gddr6_periodic_temp_readout_status_t * temp_readout_status);

/**
 * \brief - configure dynamic calibration mechanism in DBAL, differentiate between 2 state:
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
shr_error_e dnx_gddr6_dbal_access_dynamic_calibration_config(
    int unit,
    uint32 dram_index,
    uint32 channel,
    int set_init_position);

/**
 * \brief - get indication if dynamic calibration is enabled
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [in] channel - channel index
 * \param [out] enable - indication if dynamic calibration is enabled or disabled
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_gddr6_dbal_access_dynamic_calibration_enable_get(
    int unit,
    uint32 dram_index,
    uint32 channel,
    uint32 *enable);

/**
 * \brief - set indication that dynamic calibration is enabled/disabled
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [in] channel - channel index
 * \param [in] enable - indication that dynamic calibration is enabled(1) or disabled(0)
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_gddr6_dbal_access_dynamic_calibration_enable_set(
    int unit,
    uint32 dram_index,
    uint32 channel,
    uint32 enable);

/**
 * \brief -  bist configure
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [in] channel - channel
 * \param [in] bist_conf -  configuration
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_gddr6_dbal_access_bist_configure_set(
    int unit,
    uint32 dram_index,
    uint32 channel,
    dnx_gddr6_bist_configuration_t * bist_conf);
/**
 * \brief - command address training mode set
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [in] cadt_mode -  mode
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_gddr6_dbal_access_training_cadt_mode_set(
    int unit,
    uint32 dram_index,
    uint32 cadt_mode);
/**
 * \brief - mr init set
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [in] mr_index -  index
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_gddr6_dbal_access_mr_init_set(
    int unit,
    int dram_index,
    int mr_index);

/**
 * \brief - set vendor id mode
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [in] mode -  mode
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */

shr_error_e dnx_gddr6_dbal_access_vendor_id_mode_set(
    int unit,
    uint32 dram_index,
    dnx_gddr6_info_mode_t mode);

/**
 * \brief - set vendor id mode
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [in] channel - channel index
 * \param [out] last_returned_data -  bist last returned data
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_gddr6_dbal_access_channel_bist_last_returned_get(
    int unit,
    uint32 dram_index,
    uint32 channel,
    dnx_gddr6_bist_last_returned_data_t * last_returned_data);
/**
 * \brief - get read dbi from channel
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [in] channel - channel index
 * \param [out] enable -  rd_dbi enable
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_gddr6_dbal_access_channel_rd_dbi_enable_get(
    int unit,
    uint32 dram_index,
    uint32 channel,
    uint32 *enable);
/**
 * \brief - set read dbi from channel
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [in] channel - channel index
 * \param [in] enable -  rd_dbi enable
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_gddr6_dbal_access_channel_rd_dbi_enable_set(
    int unit,
    uint32 dram_index,
    uint32 channel,
    uint32 enable);
/**
 * \brief - configure training bist mode
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [in] mode -  training bist mode
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_gddr6_dbal_access_training_bist_mode_set(
    int unit,
    uint32 dram_index,
    dnx_gddr6_training_bist_mode mode);

#endif /* _BCM_DNX_DRAM_GDDR6_GDDR6_DBAL_ACCESS_H_INCLUDED */
