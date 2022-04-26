/*
 * $Id:$
 *
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * DNXC FABRIC PORTH
 */

#ifndef _SOC_DNXC_PORT_H_
#define _SOC_DNXC_PORT_H_

#if !defined(BCM_DNX_SUPPORT) && !defined(BCM_DNXF_SUPPORT)
#error "This file is for use by DNX (Jr2) and DNXF family only!"
#endif

#include <bcm/port.h>
#include <bcm/fabric.h>

#include <shared/port.h>
#include <soc/phyctrl.h>
#include <soc/dnxc/swstate/auto_generated/types/dnxc_types.h>
#include <shared/shrextend/shrextend_error.h>

#ifdef PORTMOD_SUPPORT
#include <soc/portmod/portmod.h>
#endif

/*************** for reading SoC properties ***************/
#define SOC_DNXC_PORT_LANE_CONFIG_DFE_INVALID                           (-1)
/* DFE as encoded by SoC property */
#define SOC_DNXC_PORT_LANE_CONFIG_DFE_OFF                               (0)
#define SOC_DNXC_PORT_LANE_CONFIG_DFE_ON                                (1)
#define SOC_DNXC_PORT_LANE_CONFIG_LP_DFE_ON                             (2)

/**********************************************************/

#define DNXC_PORT_INVALID_SPEED    (-2)

#define DNXC_PORT_PHY_SPEED_IS_PAM4(speed) \
    ((speed > 28125)? 1 : 0)

#define SOC_DNXC_PORT_MAX_CORE_ACCESS_PER_PORT  6

#define DNXC_PORT_FABRIC_LANES_PER_CORE     (8)

#define DNXC_PORT_TX_FIR_INVALID_MAIN_TAP   (0xffff)

#define DNXC_PORT_SERDES_LANE_CONFIG_INVALID_VAL (-1)

#define DNXC_PORT_STAT_GTIMER_NOF_CYCLES    (10000)

#define DNXC_PORT_FABRIC_FMAX_TX_PADDING_MIN_PORT_SPEED     (100000)

typedef struct dnxc_port_fabric_init_config_s
{
    soc_dnxc_lane_map_db_map_t lane2serdes[DNXC_PORT_FABRIC_LANES_PER_CORE];    /* lane-map configuration for the
                                                                                 * portmacro */
} dnxc_port_fabric_init_config_t;

typedef enum dnxc_port_init_stage_e
{
    dnxc_port_init_full = 0,
    dnxc_port_init_pass1,
    dnxc_port_init_pass2
} dnxc_port_init_stage_t;

typedef enum soc_dnxc_port_power_e
{
    soc_dnxc_port_power_off = 0,
    soc_dnxc_port_power_on = 1
} soc_dnxc_port_power_t;

typedef enum soc_dnxc_port_prbs_mode_e
{
    soc_dnxc_port_prbs_mode_phy = 0,
    soc_dnxc_port_prbs_mode_mac = 1,

    soc_dnxc_port_prbs_mode_count = 2
} soc_dnxc_port_prbs_mode_t;

typedef enum soc_dnxc_port_dfe_mode_e
{
    soc_dnxc_port_dfe_on = 0,   /* dfe is on and lp dfe is off */
    soc_dnxc_port_dfe_off,      /* dfe and lp dfe are both off */
    soc_dnxc_port_lp_dfe        /* dfe and lp dfe are both on */
} soc_dnxc_port_dfe_mode_t;

typedef enum soc_dnxc_port_channel_mode_e
{
    soc_dnxc_port_force_nr = 0,
    soc_dnxc_port_force_er
} soc_dnxc_port_channel_mode_t;

typedef struct soc_dnxc_port_phy_measure_s
{
    int valid;
    int one_clk_time_measured_int;
    int one_clk_time_measured_remainder;
    int serdes_freq_int;
    int serdes_freq_remainder;
    uint32 ref_clk_int;
    uint32 ref_clk_remainder;
    int lane;
} soc_dnxc_port_phy_measure_t;

typedef struct soc_dnxc_port_stat_gtimer_config_s
{
    uint32 enable;
    uint32 nof_periods;
} soc_dnxc_port_stat_gtimer_config_t;

typedef struct soc_dnxc_port_cable_swap_config_s
{
    uint32 enable;
    uint32 is_master;
} soc_dnxc_port_cable_swap_config_t;

#ifdef PORTMOD_SUPPORT
/* { */
typedef struct soc_dnxc_port_firmware_config_s
{
    int crc_check;
    int load_verify;
} soc_dnxc_port_firmware_config_t;

typedef int (
    *dnxc_core_address_get_f) (
    int unit,
    int core_index,
    uint16 *address);
shr_error_e soc_dnxc_fabric_single_pm_add(
    int unit,
    int core,
    int cores_num,
    int phy_offset,
    uint32 bypass_lanes_bitmap,
    int use_mutex,
    dnxc_core_address_get_f address_get_func);
shr_error_e soc_dnxc_fabric_pms_add(
    int unit,
    int cores_num,
    int phy_offset,
    uint32 bypass_lanes_bitmap,
    int use_mutex,
    dnxc_core_address_get_f address_get_func);
void soc_dnxc_fabric_pms_destroy(
    int unit,
    int cores_num);
shr_error_e soc_dnxc_external_phy_core_access_get(
    int unit,
    uint32 addr,
    phymod_access_t * access);
shr_error_e soc_dnxc_fabric_port_probe(
    int unit,
    int port,
    dnxc_port_init_stage_t init_stage,
    soc_dnxc_port_firmware_config_t * fw_config,
    dnxc_port_fabric_init_config_t * port_config);
shr_error_e soc_dnxc_port_cl72_set(
    int unit,
    soc_port_t port,
    int enable);
shr_error_e soc_dnxc_port_cl72_get(
    int unit,
    soc_port_t port,
    int *enable);
shr_error_e soc_dnxc_port_phy_control_set(
    int unit,
    soc_port_t port,
    int phyn,
    int lane,
    int is_sys_side,
    soc_phy_control_t type,
    uint32 value);
shr_error_e soc_dnxc_port_phy_control_get(
    int unit,
    soc_port_t port,
    int phyn,
    int lane,
    int is_sys_side,
    soc_phy_control_t type,
    uint32 *value);
shr_error_e soc_dnxc_port_resource_validate(
    int unit,
    const bcm_port_resource_t * resource);
shr_error_e soc_dnxc_port_resource_get(
    int unit,
    bcm_gport_t port,
    bcm_port_resource_t * resource);
shr_error_e soc_dnxc_port_fabric_resource_default_get(
    int unit,
    bcm_gport_t port,
    uint32 flags,
    bcm_port_resource_t * resource);
shr_error_e soc_dnxc_port_phy_tx_taps_default_get(
    int unit,
    bcm_port_t port,
    int is_pam4,
    bcm_port_phy_tx_t * tx);
shr_error_e soc_dnxc_port_lane_rate_get(
    int unit,
    bcm_port_t port,
    int *lane_rate);
shr_error_e soc_dnxc_port_phy_tx_set(
    int unit,
    bcm_port_t port,
    int phyn,
    int lane,
    int is_sys_side,
    bcm_port_phy_tx_t * tx);
shr_error_e soc_dnxc_port_phy_tx_get(
    int unit,
    bcm_port_t port,
    int phyn,
    int lane,
    int is_sys_side,
    bcm_port_phy_tx_t * tx);
int soc_dnxc_fabric_broadcast_firmware_loader(
    int unit,
    soc_pbmp_t * pbmp);

shr_error_e soc_dnxc_port_loopback_set(
    int unit,
    soc_port_t port,
    portmod_loopback_mode_t loopback);
shr_error_e soc_dnxc_port_loopback_get(
    int unit,
    soc_port_t port,
    portmod_loopback_mode_t * loopback);

shr_error_e soc_dnxc_port_pre_enable_set(
    int unit,
    soc_port_t port,
    int enable);

shr_error_e soc_dnxc_port_post_enable_set(
    int unit,
    soc_port_t port,
    int enable);

shr_error_e soc_dnxc_port_enable_set(
    int unit,
    soc_port_t port,
    uint32 flags,
    int enable);
shr_error_e soc_dnxc_port_enable_get(
    int unit,
    soc_port_t port,
    uint32 flags,
    int *enable);

shr_error_e soc_dnxc_port_phy_reg_get(
    int unit,
    soc_port_t port,
    uint32 flags,
    uint32 phy_reg_addr,
    uint32 *phy_data);
shr_error_e soc_dnxc_port_phy_reg_set(
    int unit,
    soc_port_t port,
    uint32 flags,
    uint32 phy_reg_addr,
    uint32 phy_data);
shr_error_e soc_dnxc_port_phy_reg_modify(
    int unit,
    soc_port_t port,
    uint32 flags,
    uint32 phy_reg_addr,
    uint32 phy_data,
    uint32 phy_mask);
shr_error_e dnxc_soc_to_phymod_ref_clk(
    int unit,
    int ref_clk,
    phymod_ref_clk_t * phymod_ref_clk);
/**
 * \brief - dnx to portmod ref clk mapping
 *
 * \param [in] unit - chip unit id.
 * \param [in] ref_clk - dnx ref_clk
 * \param [out] phymod_ref_clk - portmod/phymod clk
 * \return
 *   shr_error_e
 *
 * \remarks
 *   * This internal method
 * \see
 *   * None
 */
shr_error_e dnx_to_phymod_ref_clk(
    int unit,
    int ref_clk,
    phymod_ref_clk_t * phymod_ref_clk);
/* port controls */
shr_error_e soc_dnxc_port_control_power_set(
    int unit,
    soc_port_t port,
    uint32 flags,
    soc_dnxc_port_power_t power);
shr_error_e soc_dnxc_port_control_power_get(
    int unit,
    soc_port_t port,
    soc_dnxc_port_power_t * power);
shr_error_e soc_dnxc_port_control_low_latency_llfc_set(
    int unit,
    soc_port_t port,
    int value);
shr_error_e soc_dnxc_port_control_low_latency_llfc_get(
    int unit,
    soc_port_t port,
    int *value);
shr_error_e soc_dnxc_port_control_fec_error_detect_set(
    int unit,
    soc_port_t port,
    int value);
shr_error_e soc_dnxc_port_control_fec_error_detect_get(
    int unit,
    soc_port_t port,
    int *value);
shr_error_e soc_dnxc_port_control_llfc_after_fec_enable_set(
    int unit,
    soc_port_t port,
    int value);
shr_error_e soc_dnxc_port_control_llfc_after_fec_enable_get(
    int unit,
    soc_port_t port,
    int *value);
shr_error_e soc_dnxc_port_extract_cig_from_llfc_enable_set(
    int unit,
    soc_port_t port,
    int value);
shr_error_e soc_dnxc_port_extract_cig_from_llfc_enable_get(
    int unit,
    soc_port_t port,
    int *value);
shr_error_e soc_dnxc_port_control_cells_fec_bypass_enable_set(
    int unit,
    soc_port_t port,
    int value);
shr_error_e soc_dnxc_port_control_cells_fec_bypass_enable_get(
    int unit,
    soc_port_t port,
    int *value);
shr_error_e soc_dnxc_port_rx_locked_get(
    int unit,
    soc_port_t port,
    uint32 *rx_locked);

/* PRBS */
shr_error_e soc_dnxc_port_prbs_tx_enable_set(
    int unit,
    soc_port_t port,
    soc_dnxc_port_prbs_mode_t mode,
    int value);
shr_error_e soc_dnxc_port_prbs_rx_enable_set(
    int unit,
    soc_port_t port,
    soc_dnxc_port_prbs_mode_t mode,
    int value);
shr_error_e soc_dnxc_port_prbs_tx_enable_get(
    int unit,
    soc_port_t port,
    soc_dnxc_port_prbs_mode_t mode,
    int *value);
shr_error_e soc_dnxc_port_prbs_rx_enable_get(
    int unit,
    soc_port_t port,
    soc_dnxc_port_prbs_mode_t mode,
    int *value);
shr_error_e soc_dnxc_port_prbs_rx_status_get(
    int unit,
    soc_port_t port,
    soc_dnxc_port_prbs_mode_t mode,
    int *value);
shr_error_e soc_dnxc_port_prbs_tx_invert_data_set(
    int unit,
    soc_port_t port,
    soc_dnxc_port_prbs_mode_t mode,
    int invert);
shr_error_e soc_dnxc_port_prbs_rx_invert_data_set(
    int unit,
    soc_port_t port,
    soc_dnxc_port_prbs_mode_t mode,
    int invert);
shr_error_e soc_dnxc_port_prbs_tx_invert_data_get(
    int unit,
    soc_port_t port,
    soc_dnxc_port_prbs_mode_t mode,
    int *invert);
shr_error_e soc_dnxc_port_prbs_rx_invert_data_get(
    int unit,
    soc_port_t port,
    soc_dnxc_port_prbs_mode_t mode,
    int *invert);
shr_error_e soc_dnxc_port_prbs_polynomial_set(
    int unit,
    soc_port_t port,
    soc_dnxc_port_prbs_mode_t mode,
    int value);
shr_error_e soc_dnxc_port_prbs_polynomial_get(
    int unit,
    soc_port_t port,
    soc_dnxc_port_prbs_mode_t mode,
    int *value);
/* } */
#endif /* PORTMOD_SUPPORT */

/**
 * \brief
 *   Set packets per tiks shaper.
 * \param [in] unit -
 *   The unit number.
 * \param [in] link -
 *   The fabric link number.
 * \param [in] burst -
 *   Burst of the shaper.
 * \param [in] nof_tiks -
 *   Rate in packets per tiks. A value of zero disables rate limiting (full speed).
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None.
 * \see
 *   * None.
 */
shr_error_e soc_dnxc_port_rate_egress_ppt_set(
    int unit,
    soc_port_t link,
    uint32 burst,
    uint32 nof_tiks);

/**
 * \brief
 *   Get packets per tiks shaper.
 * \param [in] unit -
 *   The unit number.
 * \param [in] link -
 *   The fabric link number.
 * \param [out] burst -
 *   Burst of the shaper.
 * \param [out] nof_tiks -
 *   Rate in packets per tiks. A value of zero denotes shaper is disabled.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None.
 * \see
 *   * None.
 */
shr_error_e soc_dnxc_port_rate_egress_ppt_get(
    int unit,
    soc_port_t link,
    uint32 *burst,
    uint32 *nof_tiks);

/**
 * \brief
 *   Get packets per tiks shaper.
 * \param [in] unit -
 *   The unit number.
 * \param [out] single_pll_restriction_disable -
 *   should single PPL restriction be disabled on this device
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None.
 * \see
 *   * None.
 */
shr_error_e soc_dnxc_single_pll_restriction_disable_get(
    int unit,
    int *single_pll_restriction_disable);

/**
 * \brief
 *   Perform the full ber projection test.
 *   Configures, collectes are returns the values.
 * \param [in] unit -
 *   The unit number.
 * \param [in] port -
 *   The port.
 * \param [in] lane -
 *   The lane.
 * \param [in] ber_proj -
 *   BER test parametes.
 * \param [in] max_errcnt -
 *   Maximumm number of lanes to test.
 * \param [out] errcnt_array -
 *   The result array.
 * \param [out] actual_errcnt -
 *   Number of valid elements in errcnt_array
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None.
 * \see
 *   * None.
 */
shr_error_e soc_dnxc_port_post_ber_proj_get(
    int unit,
    bcm_port_t port,
    int lane,
    const bcm_port_ber_proj_params_t * ber_proj,
    int max_errcnt,
    bcm_port_ber_proj_analyzer_errcnt_t * errcnt_array,
    int *actual_errcnt);

/**
 * \brief
 *   Initialize cable swap SW DB
 * \param [in] unit -
 *   The unit number.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None.
 * \see
 *   * None.
 */
shr_error_e soc_dnxc_port_cable_swap_info_set(
    int unit);

/*
 * \brief
 *   Set cable swap configuration.
 * \param [in] unit -
 *   The unit number.
 * \param [in] link  -
 *   link to set. Other links on the same PM must be disabled prior to calling this function
 *   when link == -1 sets all PMs
 * \param [in] type -
 *   type of cable swap configuration to configure
 * \param [in] arg -
 *   the  value to set
 * \remark
 *   Other links on the same PM must be disable prior to calling this function
 */
shr_error_e soc_dnxc_port_cable_swap_set(
    int unit,
    bcm_port_t link,
    bcm_fabric_link_control_t type,
    int arg);

shr_error_e soc_dnxc_port_cable_swap_set_verify(
    int unit,
    bcm_port_t link,
    bcm_fabric_link_control_t type,
    int arg);

/*
 * \brief
 *   Get PM for given link.
 * \param [in] unit -
 *   The unit number.
 * \param [in] link -
 *   link index
 *   when link == -1 return all PMs
 * \param [out] pm_start -
 *   First PM from range
 * \param [out] nof_pms -
 *   Number of PMs in range
 * \remark
 *   None
 */
shr_error_e soc_dnxc_port_link_pms_get(
    int unit,
    bcm_port_t link,
    int *pm_start,
    int *nof_pms);

shr_error_e soc_dnxc_port_cable_swap_get_verify(
    int unit,
    bcm_port_t link,
    bcm_fabric_link_control_t type,
    int *arg);

/*
 * \brief
 *   Get cable swap configuration.
 * \param [in] unit -
 *   The unit number.
 * \param [in] link -
 *   link to get. Other links on the same PM must be disabled prior to calling this function
 *   when link == -1 get values in PM0
 * \param [in] type -
 *   type of cable swap configuration to configure
 * \param [out] arg -
 *   the read value
 * \remark
 *   Other links on the same PM must be disable prior to calling this function
 */
shr_error_e soc_dnxc_port_cable_swap_get(
    int unit,
    bcm_port_t link,
    bcm_fabric_link_control_t type,
    int *arg);

/*
 * \brief - verify that the selected FEC control is valid for the current FEC
 *
 * \param [in] unit -     Unit number
 * \param [in] port -     Device or logical port number
 * \param [in] control_type - FEC control type
 *
 * \return
 *      shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e soc_dnxc_port_fabric_fec_control_verify(
    int unit,
    bcm_port_t port,
    bcm_port_control_t control_type);

/*
 * \brief - verify that the selected fabric FEC control is valid for the current FEC
 *
 * \param [in] unit -     Unit number
 * \param [in] fec_type - FEC type
 * \param [in] control_type - FEC control type
 * \param [out] is_combination_valid - Is the control valid for the selected FEC type
 * \paran [out] is_control_type_valid - Is the control type available for the current device
 * \return
 *      shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e soc_dnxc_port_fabric_fec_control_verify_params_get(
    int unit,
    bcm_port_phy_fec_t fec_type,
    bcm_port_control_t control_type,
    int *is_combination_valid,
    int *is_control_type_valid);

shr_error_e soc_dnxc_port_fabric_fmac_tx_padding_verify(
    int unit,
    bcm_port_t port,
    int pad_size,
    int is_set);

shr_error_e soc_dnxc_port_fabric_fmac_tx_padding_set(
    int unit,
    bcm_port_t port,
    int pad_size);

shr_error_e soc_dnxc_port_fabric_fmac_tx_padding_get(
    int unit,
    bcm_port_t port,
    int *pad_size);

#endif /*_SOC_DNXC_PORT_H_*/
