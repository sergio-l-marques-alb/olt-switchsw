
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
#include <phymod/phymod_util.h>
#include <phymod/phymod_config.h>
#include <phymod/phymod_diagnostics.h>
#include <phymod/phymod_diagnostics_dispatch.h>
#include <phymod/chip/tsco_diagnostics.h>

#ifdef PHYMOD_TSCO_SUPPORT


int tsco_phy_fec_cl91_correctable_counter_get(const phymod_phy_access_t* phy, uint32_t* count)
{

    return PHYMOD_E_NONE;
}

int tsco_phy_fec_cl91_uncorrectable_counter_get(const phymod_phy_access_t* phy, uint32_t* count)
{

    return PHYMOD_E_NONE;

}

int tsco_phy_rsfec_symbol_error_counter_get(const phymod_phy_access_t* phy,
                                             int max_count,
                                             int* actual_count,
                                             uint32_t* error_count)
{

    return PHYMOD_E_NONE;
}

#endif /* PHYMOD_TSCO_SUPPORT */
