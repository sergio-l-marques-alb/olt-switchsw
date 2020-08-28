/** \file bcm_int/dnx/dram/hbmc/hbmc_cb.h
 *
 * This file contains HBMC call backs routine declarations for the HBM Dram tunning process.
 *
 */

/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef _BCMINT_DNX_DRAM_HBMC_HBMCCB_H_INCLUDED
/*
 * {
 */
#define _BCMINT_DNX_DRAM_HBMC_HBMCCB_H_INCLUDED

#include <soc/hbmc_shmoo.h>

/**
 * \brief - Modify mode registers call back
 *
 * \param [in] unit - unit number
 * \param [in] hbm_ndx - hbm index for which to change the mode register
 * \param [in] channel - channel in the hbm for which to change the mode register
 * \param [in] mr_index - mode register index
 * \param [in] data - data to modify
 * \param [in] mask - mask for data, bits that are set can change according to data
 *
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   modify Mode Register mr_index from HBMC instance hbm_ndx and channel channel.
 *   write data using mask to determine which bits to change. in HBM each MR has only 8b.
 *   function enforces this size and returns errors when exceeded. the configuration is
 *   symmetric between all channels and hbm indexes
 * \see
 *   * None
 */
shr_error_e dnx_hbmc_modify_mrs_cb(
    int unit,
    int hbm_ndx,
    int channel,
    int mr_index,
    uint32 data,
    uint32 mask);

/**
 * \brief - Set PLLs according to pll_info. fill other relevant fields with defaults
 *
 * \param [in] unit - unit number
 * \param [in] hbm_ndx - hbm index
 * \param [in] pll_info - pll info
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
shr_error_e dnx_hbmc_pll_set_cb(
    int unit,
    int hbm_ndx,
    const hbmc_shmoo_pll_t * pll_info);

/**
 * \brief - Read phy register. if (channel == -1) will read from midstack, else read from given channel
 *
 * \param [in] unit - unit number
 * \param [in] hbm_ndx - hbm index
 * \param [in] channel -  channel index in hbm - channel == -1 will read from midstack
 * \param [in] addr - address to read
 * \param [out] data - read data
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
shr_error_e dnx_hbmc_phy_reg_read_cb(
    int unit,
    int hbm_ndx,
    int channel,
    uint32 addr,
    uint32 *data);

/**
 * \brief - Write phy register. if (channel == -1) will write to midstack, else write to given channel
 *
 * \param [in] unit - unit number
 * \param [in] hbm_ndx - hbm index
 * \param [in] channel -  channel index in hbm - channel == -1 will write to midstack
 * \param [in] addr - address to write
 * \param [in] data - write data
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
shr_error_e dnx_hbmc_phy_reg_write_cb(
    int unit,
    int hbm_ndx,
    int channel,
    uint32 addr,
    uint32 data);

/**
 * \brief - modify phy register. if (channel == -1) will modify midstack, else modify given channel
 *
 * \param [in] unit - unit number
 * \param [in] hbm_ndx - hbm index
 * \param [in] channel -  channel index in hbm - channel == -1 will modify midstack
 * \param [in] addr - address to modify
 * \param [in] data - new data
 * \param [in] mask - mask of bits to change - set bits are changed.
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
shr_error_e dnx_hbmc_phy_reg_modify_cb(
    int unit,
    int hbm_ndx,
    int channel,
    uint32 addr,
    uint32 data,
    uint32 mask);

/** currently not supported until a specific demand from dram phy team */
/* { */
shr_error_e dnx_hbmc_enable_wr_parity_cb(
    int unit,
    int hbm_ndx,
    int channel,
    int enable);
shr_error_e dnx_hbmc_enable_rd_parity_cb(
    int unit,
    int hbm_ndx,
    int channel,
    int enable);
shr_error_e dnx_hbmc_enable_addr_parity_cb(
    int unit,
    int hbm_ndx,
    int channel,
    int enable);
shr_error_e dnx_hbmc_enable_wr_dbi_cb(
    int unit,
    int hbm_ndx,
    int channel,
    int enable);
shr_error_e dnx_hbmc_enable_rd_dbi_cb(
    int unit,
    int hbm_ndx,
    int channel,
    int enable);

/**
 * \brief - do soft init for given channel on controller side
 *
 * \param [in] unit - unit number
 * \param [in] hbm_ndx - hbm index
 * \param [in] channel -  channel index in hbm
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
shr_error_e dnx_hbmc_soft_reset_controller_without_dram_cb(
    int unit,
    int hbm_ndx,
    int channel);

/* } */

/**
 * \brief - configure bist run according to info
 *
 * \param [in] unit - unit number
 * \param [in] flags - supported flags
 * \param [in] hbm_ndx - hbm index
 * \param [in] channel -  channel index in hbm
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
shr_error_e dnx_hbmc_bist_conf_set_cb(
    int unit,
    int flags,
    int hbm_ndx,
    int channel,
    const hbmc_shmoo_bist_info_t * info);

/**
 * \brief - get bist error counters
 *
 * \param [in] unit - unit number
 * \param [in] flags - supported flags
 * \param [in] hbm_ndx - hbm index
 * \param [in] channel -  channel index in hbm
 * \param [in] error_counters - bist error counter
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
shr_error_e dnx_hbmc_bist_err_cnt_cb(
    int unit,
    int flags,
    int hbm_ndx,
    int channel,
    hbmc_shmoo_bist_err_cnt_t * error_counters);

/**
 * \brief - enable/disable refresh. in HBM we have only Opportunistic refresh
 *
 * \param [in] unit - unit number
 * \param [in] hbm_ndx - hbm index
 * \param [in] channel -  channel index in hbm
 * \param [in] enable - 1 - enable, 0 - disable
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
shr_error_e dnx_hbmc_enable_refresh_cb(
    int unit,
    int hbm_ndx,
    int channel,
    int enable);

/**
 * \brief - HBM initialization sequence with stable power.
 *
 * \param [in] unit - unit number
 * \param [in] hbm_ndx - hbm index
 * \param [in] phase - phase to init - not used in dnx implementation
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
shr_error_e dnx_hbmc_dram_init_cb(
    int unit,
    int hbm_ndx,
    int phase);


/**
 * \brief - get dram info relevant for tuning
 *
 * \param [in] unit - unit number
 * \param [in] shmoo_info - dram info relevant for tuning
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
shr_error_e dnx_hbmc_dram_info_access_cb(
    int unit,
    hbmc_shmoo_dram_info_t * shmoo_info);

/**
 * \brief - Get HBM model part type
 *
 * \param [in] unit - unit number
 * \param [in] hbm_index - HBM index
 * \param [out] model_part - HBM model part type for provided HBM index
 *
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * This function can be called only after the HBM SW state init part is done.
 * \see
 *   * None
 */
shr_error_e dnx_hbmc_model_part_get_cb(
    int unit,
    int hbm_index,
    hbmc_shmoo_hbm_model_part_t * model_part);

/**
 * \brief - CB function to get bist run status
 *
 * \param [in] unit - unit number
 * \param [in] hbm_ndx - hbm index
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
shr_error_e dnx_hbmc_bist_status_get_cb(
    int unit,
    int hbm_ndx,
    int channel,
    uint32 *read_cmd_counter,
    uint32 *write_cmd_counter,
    uint32 *read_data_counter);

/**
 * \brief - CB function to check for channel dwords alignment
 *
 * \param [in] unit - unit number
 * \param [in] hbm_ndx - hbm index
 * \param [in] channel - channel
 * \param [out] is_aligned - indication if dwords are aligned
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * This CB was introduced originally to provide a WA.
 *   in future architectures the problem was resolved and rendered it irrelevant.
 * \see
 *   * None
 */
shr_error_e dnx_hbmc_phy_channel_dwords_alignment_check_cb(
    int unit,
    int hbm_ndx,
    int channel,
    int *is_aligned);

#endif /* _BCMINT_DNX_DRAM_HBMC_HBMCCB_H_INCLUDED */
