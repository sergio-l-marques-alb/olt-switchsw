
/*
 *         
 * $Id: phymod.xml,v 1.1.2.5 Broadcom SDK $
 * 
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *         
 *     
 */

#include <phymod/phymod.h>
#include <phymod/phymod_util.h>
#include <phymod/phymod_config.h>
#include <phymod/phymod_diagnostics.h>
#include <phymod/phymod_diagnostics_dispatch.h>
#include <phymod/chip/tscbh.h>
#include <phymod/chip/tscbh_diagnostics.h>
#include "../../blackhawk/tier1/blackhawk_tsc_debug_functions.h"


#define PATTERN_MAX_LENGTH 240
#ifdef PHYMOD_BLACKHAWK_SUPPORT


/*phymod, internal enum mappings*/



int tscbh_phy_rx_slicer_position_set(const phymod_phy_access_t* phy, uint32_t flags, const phymod_slicer_position_t* position)
{
        
    
    /* Not supported */
    PHYMOD_DEBUG_ERROR(("tscbh_phy_rx_slicer_position_set function is NOT SUPPORTED!!\n"));

        
    return PHYMOD_E_NONE;
    
}

int tscbh_phy_rx_slicer_position_get(const phymod_phy_access_t* phy, uint32_t flags, phymod_slicer_position_t* position)
{
        
    
    /* Not supported */
    PHYMOD_DEBUG_ERROR(("tscbh_phy_rx_slicer_position_get function is NOT SUPPORTED!!\n"));

        
    return PHYMOD_E_NONE;
    
}


int tscbh_phy_rx_slicer_position_max_get(const phymod_phy_access_t* phy, uint32_t flags, const phymod_slicer_position_t* position_min, const phymod_slicer_position_t* position_max)
{
        
    
    /* Not supported */
    PHYMOD_DEBUG_ERROR(("tscbh_phy_rx_slicer_position_max_get function is NOT SUPPORTED!!\n"));

        
    return PHYMOD_E_NONE;
    
}


int tscbh_phy_prbs_config_set(const phymod_phy_access_t* phy, uint32_t flags , const phymod_prbs_t* prbs)
{
    return PHYMOD_E_NONE;
}

int tscbh_phy_prbs_config_get(const phymod_phy_access_t* phy, uint32_t flags , phymod_prbs_t* prbs)
{
    return PHYMOD_E_NONE;
}

int tscbh_phy_prbs_enable_set(const phymod_phy_access_t* phy, uint32_t flags , uint32_t enable)
{
    return PHYMOD_E_NONE;
}

int tscbh_phy_prbs_enable_get(const phymod_phy_access_t* phy, uint32_t flags , uint32_t* enable)
{
    return PHYMOD_E_NONE;
    
}


int tscbh_phy_prbs_status_get(const phymod_phy_access_t* phy, uint32_t flags, phymod_prbs_status_t* prbs_status)
{
    return PHYMOD_E_NONE;
    
}


int tscbh_phy_pattern_config_set(const phymod_phy_access_t* phy, const phymod_pattern_t* pattern)
{
        
    return PHYMOD_E_NONE;
    
}

int tscbh_phy_pattern_config_get(const phymod_phy_access_t* phy, phymod_pattern_t* pattern)
{
    return PHYMOD_E_NONE;
    
}

int tscbh_phy_pattern_enable_set(const phymod_phy_access_t* phy, uint32_t enable, const phymod_pattern_t* pattern)
{
    return PHYMOD_E_NONE;
    
}

int tscbh_phy_pattern_enable_get(const phymod_phy_access_t* phy, uint32_t* enable)
{
    return PHYMOD_E_NONE;
    
}


int tscbh_core_diagnostics_get(const phymod_core_access_t* core, phymod_core_diagnostics_t* diag)
{

    return PHYMOD_E_NONE;
}


int tscbh_phy_diagnostics_get(const phymod_phy_access_t* phy, phymod_phy_diagnostics_t* diag)
{

    return PHYMOD_E_NONE;
}

int tscbh_phy_pmd_info_dump(const phymod_phy_access_t* phy, char* type)
{
    return PHYMOD_E_NONE;
}



int tscbh_diagnostics_eye_margin_proj( const phymod_phy_access_t* phy, uint32_t flags,
                                        const phymod_phy_eyescan_options_t* eyescan_options) 
{
  return PHYMOD_E_NONE;
}

int tscbh_phy_eyescan_run(const phymod_phy_access_t* phy, 
                           uint32_t flags, 
                           phymod_eyescan_mode_t mode, 
                           const phymod_phy_eyescan_options_t* eyescan_options)
{
  return PHYMOD_E_NONE;
}


#endif /* PHYMOD_BLACKHAWK_SUPPORT */
