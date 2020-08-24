/** \file soc/dnx/synce/synce.h
 *
 * Functions for handling jr2 SyncE,
 * including NIF and Fabric SyncE.
 *
 */
/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef _SYNCE_H
#define _SYNCE_H

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (Jr2) family only!"
#endif

/*************
 * INCLUDES  *
 *************/
/* { */

/* } */

/*************
 * DEFINES   *
 *************/
/* { */

/* } */

/*************
 * MACROS    *
 *************/
/* { */

/* } */

/*************
 * TYPE DEFS *
 *************/
/* { */

typedef enum
{
    /**
     *  Synchronous Ethernet signal - differential (two signals
     *  per clock) recovered clock, two differential outputs
     */
    SOC_DNX_NIF_SYNCE_MODE_TWO_DIFF_CLK = 0,
    /**
     *  Synchronous Ethernet signal - recovered clock accompanied
     *  by a valid indication, two clk+valid outputs
     */
    SOC_DNX_NIF_SYNCE_MODE_TWO_CLK_AND_VALID = 1,
    /**
     *  Number of types in ARAD_NIF_SYNCE_MODE
     */
    SOC_DNX_NIF_SYNCE_NOF_MODES = 2
} SOC_DNX_PLL_NIF_SYNCE_MODE;

typedef enum
{
    /**
     *  SyncE reference clock PM divider 1
     */
    SOC_DNX_NIF_SYNCE_PM_DIV_1 = 0,
    /**
     *  SyncE reference clock PM divider 7
     */
    SOC_DNX_NIF_SYNCE_PM_DIV_7 = 1,
    /**
     *  SyncE reference clock PM divider 11
     */
    SOC_DNX_NIF_SYNCE_PM_DIV_11 = 2,

    SOC_DNX_NIF_SYNCE_NOF_PM_DIVIDERS = 3
} SOC_DNX_SYNCE_PM_DIVIDER;

/* } */

/*************
 * GLOBALS   *
 *************/
/* { */

/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */

int soc_dnx_synce_port_set(
    int unit,
    uint32 synce_index,
    soc_port_t port);

int soc_dnx_synce_port_get(
    int unit,
    uint32 synce_index,
    soc_port_t * port);

int soc_dnx_synce_divider_set(
    int unit,
    uint32 synce_index,
    int divider);

int soc_dnx_synce_divider_get(
    int unit,
    uint32 synce_index,
    int *divider);

int soc_dnx_nif_synce_enable_get(
    int unit,
    int core,
    int *enable);

int soc_dnx_nif_synce_enable_set(
    int unit,
    int core,
    int enable);

int soc_dnx_nif_synce_ref_clk_div_get(
    int unit,
    soc_port_t port,
    int *pm_synce_div,
    int *nif_synce_div);
/* } */

#endif /* _SYNCE_H */
