/*
 * $Id:$
 *
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * DNXC FABRIC PORTH
 */

#ifndef _SOC_DNXC_PORT_H_
#define _SOC_DNXC_PORT_H_

#if !defined(BCM_DNX_SUPPORT) && !defined(BCM_DNXF_SUPPORT)
#error "This file is for use by DNX (Jer2) and DNXF family only!" 
#endif

#include <bcm/port.h>

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

#define SOC_DNXC_PORT_LANE_CONFIG_MEDIA_TYPE_INVALID                    (-1)
/* MEDIA_TYPE as encoded by SoC property and by phymod */
#define SOC_DNXC_PORT_LANE_CONFIG_MEDIA_TYPE_BACKPLANE_COPPER_TRACE     (0) 
#define SOC_DNXC_PORT_LANE_CONFIG_MEDIA_TYPE_COPPER_CABLE               (1) 
#define SOC_DNXC_PORT_LANE_CONFIG_MEDIA_TYPE_OPTICS                     (2) 
/**********************************************************/

#define DNXC_PORT_INVALID_SPEED    (-2)

#define DNXC_PORT_PHY_SPEED_IS_PAM4(speed) \
    ((speed > 28125)? 1 : 0)

#define SOC_DNXC_PORT_MAX_CORE_ACCESS_PER_PORT  6

#define DNXC_PORT_FABRIC_LANES_PER_CORE     (8)

#define DNXC_PORT_TX_FIR_INVALID_MAIN_TAP   (0xffff)

#define DNXC_PORT_SERDES_LANE_CONFIG_INVALID_VAL (-1)

typedef struct dnxc_port_fabric_init_config_s {
    soc_dnxc_lane_map_db_map_t lane2serdes[DNXC_PORT_FABRIC_LANES_PER_CORE]; /*lane-map configuration for the portmacro*/
} dnxc_port_fabric_init_config_t;

typedef enum dnxc_port_init_stage_e {
    dnxc_port_init_full = 0,
    dnxc_port_init_pass1,
    dnxc_port_init_pass2
} dnxc_port_init_stage_t;

typedef enum soc_dnxc_port_pcs_e
{
    soc_dnxc_port_pcs_64_66_fec             = (int)_SHR_PORT_PCS_64B66BFEC,
    soc_dnxc_port_pcs_64_66                 = (int)_SHR_PORT_PCS_64B66B,
    soc_dnxc_port_pcs_64_66_rs_fec          = (int)_SHR_PORT_PCS_64B66B_RS_FEC,
    soc_dnxc_port_pcs_64_66_ll_rs_fec       = (int)_SHR_PORT_PCS_64B66B_LOW_LATENCY_RS_FEC,
    soc_dnxc_port_pcs_64_66_15t_rs_fec      = (int)_SHR_PORT_PCS_64B66B_15T_RS_FEC,
    soc_dnxc_port_pcs_64_66_15t_ll_rs_fec   = (int)_SHR_PORT_PCS_64B66B_15T_LOW_LATENCY_RS_FEC,
} soc_dnxc_port_pcs_t;


typedef enum soc_dnxc_port_power_e
{
    soc_dnxc_port_power_off       = 0,
    soc_dnxc_port_power_on        = 1
} soc_dnxc_port_power_t;

typedef enum soc_dnxc_port_prbs_mode_e {
    soc_dnxc_port_prbs_mode_phy = 0,
    soc_dnxc_port_prbs_mode_mac = 1,

    soc_dnxc_port_prbs_mode_count = 2
} soc_dnxc_port_prbs_mode_t;

typedef enum soc_dnxc_port_dfe_mode_e
{
    soc_dnxc_port_dfe_on = 0, /* dfe is on and lp dfe is off */
    soc_dnxc_port_dfe_off,    /* dfe and lp dfe are both  off */
    soc_dnxc_port_lp_dfe      /* dfe and lp dfe are both on */
} soc_dnxc_port_dfe_mode_t;

typedef enum soc_dnxc_port_channel_mode_e
{
    soc_dnxc_port_force_nr = 0,
    soc_dnxc_port_force_er
} soc_dnxc_port_channel_mode_t;

#define SOC_DNXC_PORT_CONTROL_FLAGS_RX_SERDES_IGNORE         (0x1)

#ifdef PORTMOD_SUPPORT

typedef int (*dnxc_core_address_get_f)(int unit, int core_index, uint16 *address);
shr_error_e soc_dnxc_external_phy_chain_info_get(int unit, int phy, int addresses_array_size, int *addresses, int *phys_in_chain);
shr_error_e soc_dnxc_fabric_single_pm_add(int unit, int core, int cores_num, int phy_offset, int use_mutex, dnxc_core_address_get_f address_get_func);
shr_error_e soc_dnxc_fabric_pms_add(int unit, int cores_num , int phy_offset, int use_mutex, dnxc_core_address_get_f address_get_func);
void soc_dnxc_fabric_pms_destroy(int unit, int cores_num);
shr_error_e soc_dnxc_external_phy_core_access_get(int unit, uint32 addr, phymod_access_t *access);
shr_error_e soc_dnxc_fabric_port_probe(int unit, int port, dnxc_port_init_stage_t init_stage, int fw_verify, dnxc_port_fabric_init_config_t* port_config);
shr_error_e soc_dnxc_port_cl72_set(int unit, soc_port_t port, int enable);
shr_error_e soc_dnxc_port_cl72_get(int unit, soc_port_t port, int *enable);
shr_error_e soc_dnxc_port_phy_control_set(int unit, soc_port_t port, int phyn, int lane, int is_sys_side, soc_phy_control_t type, uint32 value);
shr_error_e soc_dnxc_port_phy_control_get(int unit, soc_port_t port, int phyn, int lane, int is_sys_side, soc_phy_control_t type, uint32 *value);
shr_error_e soc_dnxc_port_resource_validate(int unit, const bcm_port_resource_t *resource);
shr_error_e soc_dnxc_port_resource_get(int unit, bcm_gport_t port, bcm_port_resource_t *resource);
shr_error_e soc_dnxc_port_fabric_resource_default_get(int unit, bcm_gport_t port, uint32 flags, bcm_port_resource_t *resource);
shr_error_e soc_dnxc_port_phy_tx_set(int unit, bcm_port_t port, int phyn, int lane, int is_sys_side, int is_init_sequence, bcm_port_phy_tx_t* tx);
shr_error_e soc_dnxc_port_phy_tx_get(int unit, bcm_port_t port, int phyn, int lane, int is_sys_side, bcm_port_phy_tx_t* tx);
int soc_dnxc_fabric_broadcast_firmware_loader(int unit, uint32 length, const uint8* data);

shr_error_e soc_dnxc_port_loopback_set(int unit, soc_port_t port, portmod_loopback_mode_t loopback);
shr_error_e soc_dnxc_port_loopback_get(int unit, soc_port_t port, portmod_loopback_mode_t* loopback);

shr_error_e  soc_dnxc_port_enable_set(int unit, soc_port_t port, int enable);
shr_error_e  soc_dnxc_port_enable_get(int unit, soc_port_t port, int *enable);

shr_error_e soc_dnxc_port_phy_reg_get(int unit, soc_port_t port, uint32 flags, uint32 phy_reg_addr, uint32 *phy_data);
shr_error_e soc_dnxc_port_phy_reg_set(int unit, soc_port_t port, uint32 flags, uint32 phy_reg_addr, uint32 phy_data);
shr_error_e soc_dnxc_port_phy_reg_modify(int unit, soc_port_t port, uint32 flags, uint32 phy_reg_addr, uint32 phy_data, uint32 phy_mask);
shr_error_e dnxc_soc_to_phymod_ref_clk(int unit, int  ref_clk, phymod_ref_clk_t *phymod_ref_clk);

/* port controls */
shr_error_e soc_dnxc_port_control_pcs_set(int unit, soc_port_t port, soc_dnxc_port_pcs_t pcs);
shr_error_e soc_dnxc_port_control_pcs_get(int unit, soc_port_t port, soc_dnxc_port_pcs_t *pcs);
shr_error_e soc_dnxc_port_control_power_set(int unit, soc_port_t port, uint32 flags, soc_dnxc_port_power_t power);
shr_error_e soc_dnxc_port_control_power_get(int unit, soc_port_t port, soc_dnxc_port_power_t* power);
shr_error_e soc_dnxc_port_control_rx_enable_set(int unit, soc_port_t port, uint32 flags, int enable);
shr_error_e soc_dnxc_port_control_tx_enable_set(int unit, soc_port_t port, int enable);
shr_error_e soc_dnxc_port_control_rx_enable_get(int unit, soc_port_t port, int* enable);
shr_error_e soc_dnxc_port_control_tx_enable_get(int unit, soc_port_t port, int* enable);
shr_error_e soc_dnxc_port_control_low_latency_llfc_set(int unit, soc_port_t port, int value);
shr_error_e soc_dnxc_port_control_low_latency_llfc_get(int unit, soc_port_t port, int *value);
shr_error_e soc_dnxc_port_control_fec_error_detect_set(int unit, soc_port_t port, int value);
shr_error_e soc_dnxc_port_control_fec_error_detect_get(int unit, soc_port_t port, int *value);
shr_error_e soc_dnxc_port_control_llfc_after_fec_enable_set(int unit, soc_port_t port, int value);
shr_error_e soc_dnxc_port_control_llfc_after_fec_enable_get(int unit, soc_port_t port, int *value);
shr_error_e soc_dnxc_port_extract_cig_from_llfc_enable_set(int unit, soc_port_t port, int value);
shr_error_e soc_dnxc_port_extract_cig_from_llfc_enable_get(int unit, soc_port_t port, int *value);
shr_error_e soc_dnxc_port_rx_locked_get(int unit, soc_port_t port, uint32 *rx_locked);

/* PRBS */
shr_error_e soc_dnxc_port_prbs_tx_enable_set(int unit, soc_port_t port, soc_dnxc_port_prbs_mode_t mode, int value);
shr_error_e soc_dnxc_port_prbs_rx_enable_set(int unit, soc_port_t port, soc_dnxc_port_prbs_mode_t mode, int value);
shr_error_e soc_dnxc_port_prbs_tx_enable_get(int unit, soc_port_t port, soc_dnxc_port_prbs_mode_t mode,  int *value);
shr_error_e soc_dnxc_port_prbs_rx_enable_get(int unit, soc_port_t port, soc_dnxc_port_prbs_mode_t mode, int *value);
shr_error_e soc_dnxc_port_prbs_rx_status_get(int unit, soc_port_t port, soc_dnxc_port_prbs_mode_t mode, int* value);
shr_error_e soc_dnxc_port_prbs_tx_invert_data_set(int unit, soc_port_t port, soc_dnxc_port_prbs_mode_t mode, int invert);
shr_error_e soc_dnxc_port_prbs_tx_invert_data_get(int unit, soc_port_t port, soc_dnxc_port_prbs_mode_t mode, int *invert);
shr_error_e soc_dnxc_port_prbs_polynomial_set(int unit, soc_port_t port, soc_dnxc_port_prbs_mode_t mode, int value);
shr_error_e soc_dnxc_port_prbs_polynomial_get(int unit, soc_port_t port, soc_dnxc_port_prbs_mode_t mode, int *value);
#endif /*PORTMOD_SUPPORT*/

#endif /*_SOC_DNXC_PORT_H_*/


