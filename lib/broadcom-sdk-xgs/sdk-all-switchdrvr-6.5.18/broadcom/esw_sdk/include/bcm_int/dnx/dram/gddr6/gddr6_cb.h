/** \file bcm_int/dnx/dram/gddr6/gddr6_cb.h
 *
 * This file contains GDDR6 call backs routine declarations for the GDDR6 Dram tunning process.
 *
 */

/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef _BCMINT_DNX_DRAM_GDDR6_GDDR6_CB_H_INCLUDED
/*
 * {
 */
#define _BCMINT_DNX_DRAM_GDDR6_GDDR6_CB_H_INCLUDED

#include <soc/shmoo_g6phy16.h>
/**
 * \brief - modify MR register callback
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [in] channel_index - channel index, SHMOO_G6PHY16_MR_CMD_ALL_CHANNELS for all channels
 * \param [in] mr_index - MR register index
 * \param [in] data - value for modify
 * \param [in] mask - MR register mask value
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
shr_error_e dnx_gddr6_modify_mrs_cb(
    int unit,
    int dram_index,
    int channel_index,
    uint32 mr_index,
    uint32 data,
    uint32 mask);
/**
 * \brief - configure pll
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [in] pll_info - pll information
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
shr_error_e dnx_gddr6_pll_set_cb(
    int unit,
    int dram_index,
    const g6phy16_drc_pll_t * pll_info);

/**
 * \brief - PHY register read callback
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [in] addr - offset on register
 * \param [out] data - value for read
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
shr_error_e dnx_gddr6_phy_reg_read_cb(
    int unit,
    int dram_index,
    uint32 addr,
    uint32 *data);
/**
 * \brief - PHY register write callback
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [in] addr - offset on register
 * \param [in] data - value for write
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
shr_error_e dnx_gddr6_phy_reg_write_cb(
    int unit,
    int dram_index,
    uint32 addr,
    uint32 data);
/**
 * \brief - PHY register modify callback
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [in] addr - offset on register
 * \param [in] data - value for modify
 * \param [in] mask - mask value
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
shr_error_e dnx_gddr6_phy_reg_modify_cb(
    int unit,
    int dram_index,
    uint32 addr,
    uint32 data,
    uint32 mask);
/**
 * \brief - enable WR CRC callback
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [in] enable - enable WR CRC
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
shr_error_e dnx_gddr6_enable_wr_crc_cb(
    int unit,
    int dram_index,
    int enable);
/**
 * \brief - enable RD CRC callback
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [in] enable - enable RD CRC
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
shr_error_e dnx_gddr6_enable_rd_crc_cb(
    int unit,
    int dram_index,
    int enable);
/**
 * \brief - enable WR DBI callback
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [in] enable - enable WR DBI
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
shr_error_e dnx_gddr6_enable_wr_dbi_cb(
    int unit,
    int dram_index,
    int enable);
/**
 * \brief - enable RD DBI callback
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [in] enable - enable RD DBI
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
shr_error_e dnx_gddr6_enable_rd_dbi_cb(
    int unit,
    int dram_index,
    int enable);
/**
 * \brief - soft reset GDDRx controller without RAM callback
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
shr_error_e dnx_gddr6_soft_reset_controller_without_dram_cb(
    int unit,
    int dram_index);
/**
 * \brief - bist configuration set callback
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [in] info - bist info
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
shr_error_e dnx_gddr6_bist_conf_set_cb(
    int unit,
    int dram_index,
    const g6phy16_bist_info_t * info);
/**
 * \brief - bist run callback
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [in] info - bist info
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
shr_error_e dnx_gddr6_bist_run_cb(
    int unit,
    int dram_index,
    const g6phy16_bist_info_t * info);
/**
 * \brief - get bist error count callback
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [out] info - bist error number
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
shr_error_e dnx_gddr6_bist_err_cnt_cb(
    int unit,
    int dram_index,
    g6phy16_bist_err_cnt_t * info);

/**
 * \brief - dram init
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [in] phase - phase
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
shr_error_e dnx_gddr6_dram_init_cb(
    int unit,
    int dram_index,
    gddr6_dram_init_phase_t phase);

/**
 * \brief - get dram info access
 *
 * \param [in] unit - unit number
 * \param [out] shmoo_info - shmoo info
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
shr_error_e dnx_gddr6_dram_info_access_cb(
    int unit,
    g6phy16_shmoo_dram_info_t * shmoo_info);

/**
 * \brief - enable wck2ck training
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [in] enable - enable wck2ck training
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
shr_error_e dnx_gddr6_enable_wck2ck_training_cb(
    int unit,
    int dram_index,
    int enable);
/**
 * \brief - enable write leveling
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [in] command_parity_lattency - parity lattency
 * \param [in] use_continious_dqs - use_continious_dqs
 * \param [in] enable - enable
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
shr_error_e dnx_gddr6_enable_write_leveling_cb(
    int unit,
    int dram_index,
    uint32 command_parity_lattency,
    int use_continious_dqs,
    int enable);
/**
 * \brief - enable mpr
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [in] enable - enable mpr
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
shr_error_e dnx_gddr6_mpr_en_cb(
    int unit,
    int dram_index,
    int enable);
/**
 * \brief - load mpr
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [in] mpr_pattern - mpr pattern
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
shr_error_e dnx_gddr6_mpr_load_cb(
    int unit,
    int dram_index,
    uint8 *mpr_pattern);

/**
 * \brief - get dram vendor info
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [out] vendor_info - dram vendor info
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
shr_error_e dnx_gddr6_vendor_info_get_cb(
    int unit,
    int dram_index,
    g6phy16_vendor_info_t * vendor_info);

/**
 * \brief - dqs pulse generate
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [in] use_continious_dqs - use_continious_dqs
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
shr_error_e dnx_gddr6_dqs_pulse_gen_cb(
    int unit,
    int dram_index,
    int use_continious_dqs);

/**
 * \brief - dram controller training bist configuration
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [in] info - info
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
shr_error_e dnx_gddr6_training_bist_conf_set_cb(
    int unit,
    int dram_index,
    g6phy16_training_bist_info_t * info);

/**
 * \brief - training BIST error counter get
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [out] info - info
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
shr_error_e dnx_gddr6_training_bist_err_cnt_cb(
    int unit,
    int dram_index,
    g6phy16_training_bist_err_cnt_t * info);

/**
 * \brief - get dram DQ byte pair swap info
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [out] pairs_were_swapped - swapped
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
shr_error_e dnx_gddr6_shmoo_drc_dq_byte_pairs_swap_info_get_cb(
    int unit,
    int dram_index,
    int *pairs_were_swapped);

/**
 * \brief - provide a fixed clock (of 0) or a regular clock
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [in] force_dqs - if set, force a fixed clock of 0, else regular clock
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_gddr6_force_dqs_cb(
    int unit,
    int dram_index,
    uint32 force_dqs);

/**
 * \brief - enable refresh
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [in] enable - enable refresh
 * \param [in] new_trefi - value to update
 * \param [in] new_trefi_ab - value to update refresh AB
 * \param [out] curr_refi - current value before update
  * \param [out] curr_refi_ab - current value before update refresh AB
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
shr_error_e dnx_gddr6_enable_refresh_cb(
    int unit,
    int dram_index,
    int enable,
    uint32 new_trefi,
    uint32 new_trefi_ab,
    uint32 *curr_refi,
    uint32 *curr_refi_ab);

#endif
