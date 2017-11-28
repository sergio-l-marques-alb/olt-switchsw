
/*
 *         
 * $Id:$
 * 
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *         
 *
 */

#ifndef _PM8X50_SHARED_H_
#define _PM8X50_SHARED_H_

#define PM8X50_LANES_PER_CORE (8)
#define MAX_PORTS_PER_PM8X50 (8)

struct pm8x50_s{
    portmod_pbmp_t phys;
    int first_phy;
    phymod_ref_clk_t ref_clk;
    phymod_polarity_t polarity;
    phymod_lane_map_t lane_map;
    phymod_firmware_load_method_t fw_load_method;
    phymod_firmware_loader_f external_fw_loader;
    phymod_core_access_t int_core_access;
    int nof_phys[PM8X50_LANES_PER_CORE]; /* internal phys for each lane */
    uint8 core_num;
    portmod_mac_soft_reset_f portmod_mac_soft_reset;
    phymod_afe_pll_t afe_pll;
    int warmboot_skip_db_restore;
    uint8 tvco;
    uint8 ovco;
    int rescal;
};

#endif /*_PM8X50_SHARED_H_*/
