/*----------------------------------------------------------------------
 * $Id: tbhmod_flexe.h,
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * $Copyright: (c) 2014 Broadcom Corporation All Rights Reserved.$
 *  Broadcom Corporation
 *  Proprietary and Confidential information
 *  All rights reserved
 *  This source file is the property of Broadcom Corporation, and
 *  may not be copied or distributed in any isomorphic form without the
 *  prior written consent of Broadcom Corporation.
 *----------------------------------------------------------------------
 *  Description: define enumerators
 *----------------------------------------------------------------------*/
/*
 * $Id: $
 * $Copyright:
 * All Rights Reserved.$
 */

#ifndef _tbhmod_flexe_H_
#define _tbhmod_flexe_H_

#include <phymod/phymod.h>
#include <phymod/phymod_debug.h>

/* UI macros definition. */
#define TBHMOD_FLEXE_UI_20G_NRZ_OSX1 0x0C698000
#define TBHMOD_FLEXE_UI_20G_NRZ_OSX2 0x18D30000
#define TBHMOD_FLEXE_UI_25G_PAM4 0x04F70000
#define TBHMOD_FLEXE_UI_25G_NRZ_OSX1 0x09EE0000
#define TBHMOD_FLEXE_UI_25G_NRZ_OSX2P5 0x18D30000
#define TBHMOD_FLEXE_UI_26G_PAM4 0x04D19E00
#define TBHMOD_FLEXE_UI_26G_NRZ 0x09A33C00

/* FCLK period macros definition. */
#define TBHMOD_FLEXE_FCLK_PERIOD_20G_DIV8 0x634c
#define TBHMOD_FLEXE_FCLK_PERIOD_20G_DIV6 0x4a79
#define TBHMOD_FLEXE_FCLK_PERIOD_25G 0x4f70
#define TBHMOD_FLEXE_FCLK_PERIOD_26G 0x4d19

/* PMD datapath latency. */
#define TBHMOD_FLEXE_PMD_TX_DP_LATENCY_NRZ 144
#define TBHMOD_FLEXE_PMD_TX_DP_LATENCY_PAM4 62

extern int tbhmod_flexe_pmd_ln_dp_reset_override_enable(PHYMOD_ST* pc, int enable);
extern int tbhmod_flexe_pcs_reset_sw_war(const PHYMOD_ST* pc);
extern int tbhmod_flexe_pcs_set_1588_ui(PHYMOD_ST* pc, uint32_t vco, uint32_t vco1, int os_mode, int clk4sync_div, int pam4);
extern int tbhmod_flexe_1588_pmd_latency(PHYMOD_ST* pc, int an_en, uint32_t vco, int os_mode, int pam4);

#endif  /*  _tbhmod_flexe_H_ */
