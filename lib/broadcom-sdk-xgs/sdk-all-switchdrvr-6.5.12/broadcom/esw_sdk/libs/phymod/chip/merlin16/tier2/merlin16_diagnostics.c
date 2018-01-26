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
#include <phymod/phymod_diagnostics.h>
#include <phymod/phymod_diagnostics_dispatch.h>
#include "merlin16/tier1/merlin16_diag.h"
#include "merlin16/tier1/merlin16_config.h"


#ifdef PHYMOD_MERLIN16_SUPPORT


STATIC int merlin16_diagnostics_eyescan_run_uc(const phymod_phy_access_t* phy, uint32_t flags)
{
    if(PHYMOD_EYESCAN_F_ENABLE_GET(flags)) {
        return merlin16_display_eye_scan(&phy->access);
    }

    return PHYMOD_E_NONE;
}

int merlin16_phy_eyescan_run(const phymod_phy_access_t* phy, uint32_t flags, phymod_eyescan_mode_t mode, const phymod_phy_eyescan_options_t* eyescan_options)
{        

    switch(mode) {
        case phymodEyescanModeFast:
             return merlin16_diagnostics_eyescan_run_uc(phy, flags);
        default:
            PHYMOD_RETURN_WITH_ERR(PHYMOD_E_PARAM, (_PHYMOD_MSG("unsupported eyescan mode %u"), mode));
    }
        
    return PHYMOD_E_NONE;
    
}

#endif /* PHYMOD_MERLIN16_SUPPORT */
