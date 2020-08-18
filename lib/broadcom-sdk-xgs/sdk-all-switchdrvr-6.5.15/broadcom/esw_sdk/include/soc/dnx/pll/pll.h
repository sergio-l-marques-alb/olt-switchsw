/*
 * $Id: jer2_pll_init.h Exp $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifndef _SOC_DNX_PLL_INIT_H
#define _SOC_DNX_PLL_INIT_H

/*************
 * INCLUDES  *
 *************/

/*************
 * DEFINES   *
 *************/

/*************
 * TYPE DEFS *
 *************/

/*
 * This enum is used as an index to PLL init and set functions
 */
typedef enum
{
    /*
     * NIF
     */
    DNX_PLL_TYPE_NIF_0 = 0,
    DNX_PLL_TYPE_NIF_1 = 1,
    /*
     * Fabric
     */
    DNX_PLL_TYPE_FABRIC_0 = 2,
    DNX_PLL_TYPE_FABRIC_1 = 3,
    /*
     * 1588
     */
    DNX_PLL_TYPE_TS = 4,
    DNX_PLL_TYPE_BS = 5,
    /*
     * Number of Jericho 2 PLL types
     */
    DNX_NOF_PLL_TYPES = 8
} DNX_PLL_TYPE;

/*
 * This enum is used as an index to PLL ref clocks
 */
typedef enum
{
    /*
     * PLL bypassed(input ref clock == output ref clock)
     */
    DNX_SERDES_REF_CLOCK_BYPASS = -2,
    /*
     * Disable pll
     */
    DNX_SERDES_REF_CLOCK_DISABLE = -1,
    /*
     * 125 MHZ
     */
    DNX_SERDES_REF_CLOCK_125 = 0,
    /*
     * 156.25 Mhz
     */
    DNX_SERDES_REF_CLOCK_156_25 = 1,
    /*
     * 312.5 MHZ
     */
    DNX_SERDES_REF_CLOCK_312_5 = 2,
    /*
     *  Number of ref clocks in DNX_SERDES_REF_CLOCK_TYPE
     */
    DNX_SERDES_NOF_REF_CLOCKS = 3
} DNX_SERDES_REF_CLOCK_TYPE;

/*************
 * FUNCTIONS *
 *************/

int soc_dnx_synce_pll_set(
    int unit,
    int synce_index,
    uint32 ndiv,
    uint32 mdiv,
    uint32 pdiv,
    uint32 fref,
    uint32 is_bypass);

int soc_dnx_pll_init(
    int unit);

int soc_dnx_pll_deinit(
    int unit);

shr_error_e soc_dnx_pll_3_div_get(
    int unit,
    DNX_PLL_TYPE pll_type,
    uint32 *ndiv,
    uint32 *mdiv,
    uint32 *pdiv);
#endif /* !_SOC_DNX_PLL_INIT_H */
