
/*
 *         
 * $Id:$
 * 
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *         
 *
 */

#ifndef _PM8X50_SHARED_H_
#define _PM8X50_SHARED_H_

#define PM8X50_LANES_PER_CORE (8)
#define MAX_PORTS_PER_PM8X50 (8)
#define PM8X50_MAX_NUM_PHYS  (1)

struct pm8x50_s{
    portmod_pbmp_t phys;
    int first_phy;
    phymod_ref_clk_t ref_clk;
    phymod_polarity_t polarity;
    phymod_lane_map_t lane_map;
    phymod_firmware_load_method_t fw_load_method;
    phymod_firmware_loader_f external_fw_loader;
    phymod_core_access_t int_core_access;
    uint8 core_num;
    portmod_mac_soft_reset_f portmod_mac_soft_reset;
    phymod_afe_pll_t afe_pll;
    int warmboot_skip_db_restore;
    uint8 tvco;
    uint8 ovco;
    int rescal;
    int is_master_pm;
    portmod_egress_buffer_reset_f portmod_egress_buffer_reset;
};

/* for a bypass port: convert speed to VCO rate */
int pm8x50_shared_pcs_bypassed_vco_get(int speed, portmod_vco_type_t* vco);

#endif /*_PM8X50_SHARED_H_*/
