/******************************************************************************
 ******************************************************************************
 *  Revision      :   *
 *                                                                            *
 *  Description   :  Enum types used by Serdes API functions                  *
 *                                                                            *
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$                                                      *
 *  No portions of this material may be reproduced in any form without        *
 *  the written permission of:                                                *
 *      Broadcom Corporation                                                  *
 *      5300 California Avenue                                                *
 *      Irvine, CA  92617                                                     *
 *                                                                            *
 *  All information contained in this document is Broadcom Corporation        *
 *  company private proprietary, and trade secret.                            *
 *                                                                            *
 ******************************************************************************
 ******************************************************************************/

/** @file blackhawk_tsc_enum.h
 * Enum types used by Serdes API functions
 */

#ifndef BLACKHAWK_TSC_API_ENUM_H
#define BLACKHAWK_TSC_API_ENUM_H

#include "blackhawk_tsc_ipconfig.h"
#ifdef NON_SDK
#include <stdint.h>
#else
#include <phymod/phymod.h>
#endif


enum blackhawk_tsc_pll_refclk_enum {
    BLACKHAWK_TSC_PLL_REFCLK_UNKNOWN = 0, /* Refclk value to be determined by API. */
    BLACKHAWK_TSC_PLL_REFCLK_100MHZ         = 0x00100064UL, /* 100 MHz         */
    BLACKHAWK_TSC_PLL_REFCLK_106P25MHZ      = 0x004001A9UL, /* 106.25 MHz      */
    BLACKHAWK_TSC_PLL_REFCLK_122P88MHZ      = 0x01900C00UL, /* 122.88 MHz      */
    BLACKHAWK_TSC_PLL_REFCLK_125MHZ         = 0x0010007DUL, /* 125 MHz         */
    BLACKHAWK_TSC_PLL_REFCLK_145P947802MHZ  = 0x16C0CF85UL, /* 145.947802 MHz  */
    BLACKHAWK_TSC_PLL_REFCLK_155P52MHZ      = 0x01900F30UL, /* 155.52 MHz      */
    BLACKHAWK_TSC_PLL_REFCLK_156P25MHZ      = 0x00400271UL, /* 156.25 MHz      */
    BLACKHAWK_TSC_PLL_REFCLK_159P375MHZ     = 0x008004FBUL, /* 159.375 MHz     */
    BLACKHAWK_TSC_PLL_REFCLK_161P1328125MHZ = 0x08005091UL, /* 161.1328125 MHz */
    BLACKHAWK_TSC_PLL_REFCLK_166P67MHZ      = 0x0640411BUL, /* 166.67 MHz      */
    BLACKHAWK_TSC_PLL_REFCLK_166P7974882MHZ = 0x27D19F0AUL, /* 166.7974882 MHz */
    BLACKHAWK_TSC_PLL_REFCLK_167P38MHZ      = 0x06404162UL, /* 167.38 MHz      */
    BLACKHAWK_TSC_PLL_REFCLK_167P4107143MHZ = 0x0380249FUL, /* 167.4107143 MHz */
    BLACKHAWK_TSC_PLL_REFCLK_174P703125MHZ  = 0x04002BADUL, /* 174.703125 MHz  */
    BLACKHAWK_TSC_PLL_REFCLK_212P5MHZ       = 0x002001A9UL, /* 212.5 MHz       */
    BLACKHAWK_TSC_PLL_REFCLK_311p04         = 0x01901E60UL, /* 311.04 MHz      */
    BLACKHAWK_TSC_PLL_REFCLK_312P5MHZ       = 0x00200271UL, /* 312.5 MHz       */
    BLACKHAWK_TSC_PLL_REFCLK_425MHZ         = 0x001001A9UL  /* 425 MHz         */
    };


enum blackhawk_tsc_pll_div_enum {
    BLACKHAWK_TSC_PLL_DIV_UNKNOWN = 0, /* Divide value to be determined by API. */
    BLACKHAWK_TSC_PLL_DIV_66         = (int)0x00000042, /* Divide by 66         */
    BLACKHAWK_TSC_PLL_DIV_67         = (int)0x00000043, /* Divide by 67         */
    BLACKHAWK_TSC_PLL_DIV_70         = (int)0x00000046, /* Divide by 70         */
    BLACKHAWK_TSC_PLL_DIV_72         = (int)0x00000048, /* Divide by 72         */
    BLACKHAWK_TSC_PLL_DIV_73P6       = (int)0x99998049, /* Divide by 73.6       */
    BLACKHAWK_TSC_PLL_DIV_79P2       = (int)0x3333004F, /* Divide by 79.2       */
    BLACKHAWK_TSC_PLL_DIV_80         = (int)0x00000050, /* Divide by 80         */
    BLACKHAWK_TSC_PLL_DIV_82P5       = (int)0x80000052, /* Divide by 82.5       */
    BLACKHAWK_TSC_PLL_DIV_84         = (int)0x00000054, /* Divide by 84         */
    BLACKHAWK_TSC_PLL_DIV_85         = (int)0x00000055, /* Divide by 85         */
    BLACKHAWK_TSC_PLL_DIV_87P5       = (int)0x80000057, /* Divide by 87.5       */
    BLACKHAWK_TSC_PLL_DIV_89P6       = (int)0x99998059, /* Divide by 89.6       */
    BLACKHAWK_TSC_PLL_DIV_90         = (int)0x0000005A, /* Divide by 90         */
    BLACKHAWK_TSC_PLL_DIV_96         = (int)0x00000060, /* Divide by 96         */
    BLACKHAWK_TSC_PLL_DIV_100        = (int)0x00000064, /* Divide by 100        */
    BLACKHAWK_TSC_PLL_DIV_120        = (int)0x00000078, /* Divide by 120        */
    BLACKHAWK_TSC_PLL_DIV_127P401984 = (int)0x66E8807F, /* Divide by 127.401984 */
    BLACKHAWK_TSC_PLL_DIV_128        = (int)0x00000080, /* Divide by 128        */
    BLACKHAWK_TSC_PLL_DIV_132        = (int)0x00000084, /* Divide by 132        */
    BLACKHAWK_TSC_PLL_DIV_140        = (int)0x0000008C, /* Divide by 140        */
    BLACKHAWK_TSC_PLL_DIV_144        = (int)0x00000090, /* Divide by 144        */
    BLACKHAWK_TSC_PLL_DIV_147P2      = (int)0x33330093, /* Divide by 147.2      */
    BLACKHAWK_TSC_PLL_DIV_158P4      = (int)0x6666809E, /* Divide by 158.4      */
    BLACKHAWK_TSC_PLL_DIV_160        = (int)0x000000A0, /* Divide by 160        */
    BLACKHAWK_TSC_PLL_DIV_165        = (int)0x000000A5, /* Divide by 165        */
    BLACKHAWK_TSC_PLL_DIV_168        = (int)0x000000A8, /* Divide by 168        */
    BLACKHAWK_TSC_PLL_DIV_170        = (int)0x000000AA, /* Divide by 170        */
    BLACKHAWK_TSC_PLL_DIV_175        = (int)0x000000AF, /* Divide by 175        */
    BLACKHAWK_TSC_PLL_DIV_180        = (int)0x000000B4, /* Divide by 180        */
    BLACKHAWK_TSC_PLL_DIV_184        = (int)0x000000B8, /* Divide by 184        */
    BLACKHAWK_TSC_PLL_DIV_192        = (int)0x000000C0, /* Divide by 192        */
    BLACKHAWK_TSC_PLL_DIV_198        = (int)0x000000C6, /* Divide by 198        */
    BLACKHAWK_TSC_PLL_DIV_200        = (int)0x000000C8, /* Divide by 200        */
    BLACKHAWK_TSC_PLL_DIV_224        = (int)0x000000E0, /* Divide by 224        */
    BLACKHAWK_TSC_PLL_DIV_240        = (int)0x000000F0, /* Divide by 240        */
    BLACKHAWK_TSC_PLL_DIV_264        = (int)0x00000108, /* Divide by 264        */
    BLACKHAWK_TSC_PLL_DIV_280        = (int)0x00000118, /* Divide by 280        */
    BLACKHAWK_TSC_PLL_DIV_330        = (int)0x0000014A, /* Divide by 330        */
    BLACKHAWK_TSC_PLL_DIV_350        = (int)0x0000015E  /* Divide by 350        */
};

/** TX AFE Settings Enum */
enum blackhawk_tsc_tx_afe_settings_enum {
    TX_AFE_TAP0,
    TX_AFE_TAP1,
    TX_AFE_TAP2,
    TX_AFE_TAP3,
    TX_AFE_TAP4,
    TX_AFE_TAP5,
    TX_AFE_TAP6,
    TX_AFE_TAP7,
    TX_AFE_TAP8,
    TX_AFE_TAP9,
    TX_AFE_TAP10,
    TX_AFE_TAP11
};

/** TXFIR Tap Enable Enum */
enum blackhawk_tsc_txfir_tap_enable_enum {
    NRZ_LP_3TAP  = 0,
    NRZ_6TAP     = 1,
    PAM4_LP_3TAP = 2,
    PAM4_6TAP    = 3
};

/** PLL Configuration Options Enum */
enum blackhawk_tsc_pll_option_enum {
    BLACKHAWK_TSC_PLL_OPTION_NONE,
    BLACKHAWK_TSC_PLL_OPTION_REFCLK_DOUBLER_EN,
    BLACKHAWK_TSC_PLL_OPTION_REFCLK_DIV2_EN,
    BLACKHAWK_TSC_PLL_OPTION_REFCLK_DIV4_EN
};

#endif
