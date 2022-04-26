/*
 * $Id: pll.h Exp $
 * $Copyright: (c) 2021 Broadcom.
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
 * This enum is used as an index to PLL1 init and set functions
 */
typedef enum
{
    DNX_PLL1_TYPE_INVALID = -1,
    DNX_PLL1_TYPE_FIRST = 0,
    /*
     * 1588
     */
    DNX_PLL1_TYPE_TS = DNX_PLL1_TYPE_FIRST,
    DNX_PLL1_TYPE_BS = 1,
    /*
     * NIF PLL
     */
    DNX_PLL1_TYPE_NIF = 2,
    /*
     * FAB PLL
     */
    DNX_PLL1_TYPE_FAB = 3,
    /*
     * FLEXE PLL
     */
    DNX_PLL1_TYPE_FLEXE = 4,
    /*
     * Number of PLL1 types
     */
    DNX_PLL1_TYPE_COUNT
} DNX_PLL1_TYPE;

/*
 * This enum is used as an index to PLL3 init and set functions
 */
typedef enum
{
    DNX_PLL3_TYPE_INVALID = -1,
    DNX_PLL3_TYPE_FIRST = 0,
    /*
     * NIF PLLs
     */
    DNX_PLL3_TYPE_NIF0 = DNX_PLL3_TYPE_FIRST,
    DNX_PLL3_TYPE_NIF1 = 1,
    /*
     * Fabric PLLs
     */
    DNX_PLL3_TYPE_MAS0 = 2,
    DNX_PLL3_TYPE_MAS1 = 3,
    /*
     * Number of PLL1 types
     */
    DNX_PLL3_TYPE_COUNT
} DNX_PLL3_TYPE;

/*
 * This enum is used as an index to PLL4 init and set functions
 */
typedef enum
{
    DNX_PLL4_TYPE_INVALID = -1,
    DNX_PLL4_TYPE_FIRST = 0,
    /*
     * 1588
     */
    DNX_PLL4_TYPE_TS = DNX_PLL4_TYPE_FIRST,
    DNX_PLL4_TYPE_BS = 1,
    /*
     * NIF PLL (there are 2 instances, but NIF1 is not real and should not be configured
     * so no need for enum value for both)
     */
    DNX_PLL4_TYPE_NIF = 2,
    /*
     * FAB PLL (there are 2 instances, but FAB1 is not real and should not be configured
     * so no need for enum value for both)
     */
    DNX_PLL4_TYPE_FAB = 3,
    /*
     * NIF TSC common clock
     */
    DNX_PLL4_TYPE_NIF_TSC = 4,
    /*
     * Framer core clock
     */
    DNX_PLL4_TYPE_FRAMER = 5,
    /*
     * Framer and SAR related clocks
     */
    DNX_PLL4_TYPE_SAR = 6,
    /*
     * Number of PLL4 types
     */
    DNX_PLL4_TYPE_COUNT
} DNX_PLL4_TYPE;

/*
 * This enum is used as an index to Serdes ref clocks
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

/**
 * \brief
 *   Get HW cmic timestamp.
 *
 * \param [in] unit - The unit number.
 * \param [in] time_stamp_index - The cmic time stamp index (zero or one).
 * \param [out] timestamp_ns - Pointer to the retrieved value.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e soc_dnx_pll_timestamp_get(
    int unit,
    int time_stamp_index,
    uint64 *timestamp_ns);

int soc_dnx_synce_pll_set(
    int unit,
    int synce_index,
    uint32 ndiv,
    uint32 mdiv,
    uint32 pdiv,
    uint32 fref,
    uint32 is_bypass);

int soc_dnx_synce_pll_power_enable_get(
    int unit,
    int synce_index,
    uint32 *enable);

int soc_dnx_synce_pll_power_enable_set(
    int unit,
    int synce_index,
    int enable);

int soc_dnx_pll_init(
    int unit);

shr_error_e soc_dnx_pll_3_div_get(
    int unit,
    DNX_PLL3_TYPE pll_type,
    uint32 *ndiv,
    uint32 *mdiv,
    uint32 *pdiv);

int soc_dnx_pll_1_init_all(
    int unit);

int soc_dnx_pll_3_init_all(
    int unit);

int soc_dnx_pll_4_init_all(
    int unit);

#endif /* !_SOC_DNX_PLL_INIT_H */
