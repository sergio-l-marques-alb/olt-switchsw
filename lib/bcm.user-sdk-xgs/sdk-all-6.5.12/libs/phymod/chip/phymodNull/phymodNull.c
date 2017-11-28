/*
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#include <phymod/phymod.h>

int phymodNull_phy_eee_set(const phymod_phy_access_t* phy, uint32_t enable)
{
    return PHYMOD_E_UNAVAIL;
}

int phymodNull_phy_eee_get(const phymod_phy_access_t* phy, uint32_t* enable)
{
    return PHYMOD_E_UNAVAIL;
}
