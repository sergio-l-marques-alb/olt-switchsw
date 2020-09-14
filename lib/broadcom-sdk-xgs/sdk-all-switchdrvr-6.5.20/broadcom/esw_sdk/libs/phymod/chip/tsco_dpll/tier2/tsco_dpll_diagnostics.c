/*
 *
 * $Id: phymod.xml,v 1.1.2.5 Broadcom SDK $
 *
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 *
 */

#include <phymod/phymod.h>
#include <phymod/phymod_system.h>
#include <phymod/phymod_diagnostics.h>
#include <phymod/phymod_diagnostics_dispatch.h>

#ifdef PHYMOD_TSCO_DPLL_SUPPORT

int tsco_dpll_phy_prbs_config_set(const phymod_phy_access_t* phy, uint32_t flags , const phymod_prbs_t* prbs)
{

    return PHYMOD_E_NONE;

}

int tsco_dpll_phy_prbs_config_get(const phymod_phy_access_t* phy, uint32_t flags , phymod_prbs_t* prbs)
{

    return PHYMOD_E_NONE;

}


int tsco_dpll_phy_prbs_enable_set(const phymod_phy_access_t* phy, uint32_t flags , uint32_t enable)
{

    return PHYMOD_E_NONE;

}

int tsco_dpll_phy_prbs_enable_get(const phymod_phy_access_t* phy, uint32_t flags , uint32_t* enable)
{

    return PHYMOD_E_NONE;

}


int tsco_dpll_phy_prbs_status_get(const phymod_phy_access_t* phy, uint32_t flags, phymod_prbs_status_t* prbs_status)
{

    return PHYMOD_E_NONE;

}


int tsco_dpll_phy_pattern_config_set(const phymod_phy_access_t* phy, const phymod_pattern_t* pattern)
{

    return PHYMOD_E_NONE;

}

int tsco_dpll_phy_pattern_config_get(const phymod_phy_access_t* phy, phymod_pattern_t* pattern)
{

    return PHYMOD_E_NONE;

}


int tsco_dpll_phy_pattern_enable_set(const phymod_phy_access_t* phy, uint32_t enable, const phymod_pattern_t* pattern)
{

    return PHYMOD_E_NONE;

}

int tsco_dpll_phy_pattern_enable_get(const phymod_phy_access_t* phy, uint32_t* enable)
{

    return PHYMOD_E_NONE;
}


int tsco_dpll_phy_pmd_info_dump(const phymod_phy_access_t* phy, const char* type)
{

    return PHYMOD_E_NONE;
}


int tsco_dpll_phy_eyescan_run(const phymod_phy_access_t* phy, uint32_t flags, phymod_eyescan_mode_t mode, const phymod_phy_eyescan_options_t* eyescan_options)
{

    return PHYMOD_E_NONE;

}


int tsco_dpll_phy_fec_cl91_correctable_counter_get(const phymod_phy_access_t* phy, uint32_t* count)
{

    return PHYMOD_E_NONE;

}


int tsco_dpll_phy_fec_cl91_uncorrectable_counter_get(const phymod_phy_access_t* phy, uint32_t* count)
{

    return PHYMOD_E_NONE;

}


int tsco_dpll_phy_PAM4_tx_pattern_enable_set(const phymod_phy_access_t* phy, phymod_PAM4_tx_pattern_t pattern_type, uint32_t enable)
{


    return PHYMOD_E_NONE;

}

int tsco_dpll_phy_PAM4_tx_pattern_enable_get(const phymod_phy_access_t* phy, phymod_PAM4_tx_pattern_t pattern_type, uint32_t* enable)
{

    return PHYMOD_E_NONE;

}


int tsco_dpll_phy_ber_proj(const phymod_phy_access_t* phy, phymod_ber_proj_mode_t mode, const phymod_phy_ber_proj_options_t* options)
{        
    
    /* Place your code here */

        
    return PHYMOD_E_NONE;
    
}


int tsco_dpll_phy_fast_ber_proj_get(const phymod_phy_access_t* phy, uint32_t* ber_proj_data)
{

    return PHYMOD_E_NONE;

}


int tsco_dpll_phy_rsfec_symbol_error_counter_get(const phymod_phy_access_t* phy, int max_count, int* actual_count, uint32_t* error_count)
{

    return PHYMOD_E_NONE;

}


int tsco_dpll_phy_linkcat(const phymod_phy_access_t* phy, phymod_linkCAT_op_mode_t mode)
{

    return PHYMOD_E_NONE;

}


int tsco_dpll_phy_pmd_lane_diag_debug_level_set(const phymod_phy_access_t* phy, uint32_t level)
{

    return PHYMOD_E_NONE;

}

int tsco_dpll_phy_pmd_lane_diag_debug_level_get(const phymod_phy_access_t* phy, uint32_t* level)
{

    return PHYMOD_E_NONE;

}


#endif /* PHYMOD_TSCO_DPLL_SUPPORT */
