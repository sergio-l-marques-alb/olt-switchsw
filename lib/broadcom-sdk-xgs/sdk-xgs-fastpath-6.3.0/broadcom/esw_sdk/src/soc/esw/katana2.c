/*
 * $Id: katana2.c 1.83.2.2 Broadcom SDK $
 * $Copyright: Copyright 2012 Broadcom Corporation.
 * This program is the proprietary software of Broadcom Corporation
 * and/or its licensors, and may only be used, duplicated, modified
 * or distributed pursuant to the terms and conditions of a separate,
 * written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized
 * License, Broadcom grants no license (express or implied), right
 * to use, or waiver of any kind with respect to the Software, and
 * Broadcom expressly reserves all rights in and to the Software
 * and all intellectual property rights therein.  IF YOU HAVE
 * NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
 * IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
 * ALL USE OF THE SOFTWARE.  
 *  
 * Except as expressly set forth in the Authorized License,
 *  
 * 1.     This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use
 * all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of
 * Broadcom integrated circuit products.
 *  
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
 * PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
 * OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 * 
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 * BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
 * INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
 * ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
 * TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
 * THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR USD 1.00,
 * WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
 * ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
 *
 * File:        katana.c
 * Purpose:
 * Requires:
 */


#include <sal/core/boot.h>

#include <soc/katana2.h>
#include <soc/trident.h>
#include <soc/bradley.h>
#include <soc/drv.h>
#include <soc/error.h>
#include <soc/debug.h>
#include <soc/mem.h>
#include <soc/hash.h>
#include <soc/shmoo_ddr40.h>
#include <soc/phy/ddr40.h>
#include <soc/mspi.h>
#include <soc/phyreg.h>

#ifdef BCM_KATANA2_SUPPORT

typedef enum {
    _SOC_PARITY_TYPE_NONE,
    _SOC_PARITY_TYPE_GENERIC,
    _SOC_PARITY_TYPE_PARITY,
    _SOC_PARITY_TYPE_ECC,
    _SOC_PARITY_TYPE_HASH,
    _SOC_PARITY_TYPE_EDATABUF,
    _SOC_PARITY_TYPE_COUNTER,
    _SOC_PARITY_TYPE_MMU_THDO,
    _SOC_PARITY_TYPE_MMU_THDI,
    _SOC_PARITY_TYPE_AXP,
    _SOC_PARITY_TYPE_SER
} _soc_kt2_parity_info_type_t;


extern int _soc_ddr_shmoo_prepare_for_shmoo(int unit, int ci);
typedef struct _soc_katana2_ci_parama_s {
    uint32          speed;
    uint32          grade;
} _soc_katana2_ci_parama_t;

/* 56450 TDM sequence */

/* 0:CMIC Port  1-40:Physical Ports  41:Loopback Port */
#define KT2_MAX_LOGICAL_PORTS         42
#define KT2_MAX_PHYSICAL_PORTS        40

uint32 mxqspeeds[KT2_MAX_MXQBLOCKS][KT2_MAX_MXQPORTS_PER_BLOCK]={
              {10000,2500,0,1000},{10000,2500,0,1000},{10000,2500,0,1000},
              {10000,2500,0,1000},{10000,2500,0,1000},{10000,2500,0,1000},
              {13000,2500,0,2500},{13000,2500,0,2500},
              {21000,10000,0,10000},{21000,10000,0,10000}};
uint32 kt2_current_tdm[256]={0};
uint32 kt2_current_tdm_size=0;

int mxqblock_max_startaddr[KT2_MAX_MXQBLOCKS]={0};
int mxqblock_max_endaddr[KT2_MAX_MXQBLOCKS]={0};

static uint8  old_tdm_no=0;

/* --------*/
/* DEFAULT */
/* --------*/
#define KT2_DEFAULT_PORT_CONFIG 0

/* bcm5645x_config=0 */
/* Deprecated=0 24x GE + 16*2.5GE(HG-LIT) */
/* 24xGE(1..24) + 16*2.5GE(HG-LIT)(25..40) */
uint32 kt2_tdm_56450config_deprecated0[90]= {
       KT2_LPBK_PORT,	25,	     26,          27,          KT2_CMIC_PORT,
       28,	        35,	     38,          32,          1,
       29,	        36,	     39,          33,          5,
       30,	        37,	     40,          34,          9,
       31,	        13,	     17,          21,          2,
       6,	        10,	     14,          18,          22,
       KT2_LPBK_PORT,	25,	     26,          27,          3,
       28,	        35,	     38,          32,          7,
       29,	        36,	     39,          33,          11,
       30,	        37,	     40,          34,          KT2_CMIC_PORT,
       31,	        15,	     19,          23,          4,
       8,	        12,	     16,          20,          24,
       KT2_LPBK_PORT,	25,	     26,          27,          KT2_IDLE,
       28,	        35,	     38,          32,          KT2_IDLE,
       29,	        36,	     39,          33,          KT2_IDLE,
       30,	        37,	     40,          34,          KT2_IDLE,
       31,	        KT2_IDLE,    KT2_IDLE,    KT2_IDLE,    KT2_IDLE,
       KT2_IDLE,        KT2_IDLE,    KT2_IDLE,    KT2_IDLE,    KT2_IDLE,
};

/* 24xGE(1..24) + 16*2.5GE(HG-LIT)(25..40) */
uint32 kt2_tdm_56450speed_deprecated0[40]= {
       1000,1000,1000,1000,     /* 1 - 4   */
       1000,1000,1000,1000,     /* 5 - 8   */
       1000,1000,1000,1000,     /* 9 - 12  */
       1000,1000,1000,1000,     /* 13 - 16 */
       1000,1000,1000,1000,     /* 17 - 20 */
       1000,1000,1000,1000,     /* 21 - 24 */
       2500,2500,2500,2500,     /* 25 - 28 */
       2500,2500,2500,2500,     /* 29 - 32 */
       2500,2500,2500,2500,     /* 33 - 36 */
       2500,2500,2500,2500};    /* 37 - 40 */
soc_port_details_t kt2_port_details_deprecated0[]={
    {1,24,1,GE_PORT,1000},
    {25,40,1,HGL_PORT|STK_PORT,2500}, 
    {0,0,0,0} /* End */
};

/* bcm5645x_config=1 */

/* TDM-A1 Clock 178MHz */
/* 4xGE(1..4) 2xF-XAUI(5,9) 2xF-HG[13](25,26) 2x21G[G.INT](27,28) */
/*        subcycle   1 2 3 4 5 
      1
      ..
      18                       
*/
uint32 kt2_tdm_56450config_deprecated1[90]= {
       27,    26,               28,    5,    25,
       27,    KT2_LPBK_PORT,    28,    9,    26,
       27,    25,               28,    5,    KT2_CMIC_PORT,
       27,    26,               28,    9,    25,
       27,    1,                28,    5,    26,
       27,    25,               28,    9,    2,
       27,    26,               28,    5,    25,
       27,    KT2_LPBK_PORT,    28,    9,    26,
       27,    25,               28,    5,    3,
       27,    26,               28,    9,    25,
       27,    4,                28,    5,    26,
       27,    25,               28,    9,    KT2_CMIC_PORT,
       27,    26,               28,    5,    25,
       27,    KT2_LPBK_PORT,    28,    9,    26,
       27,    25,               28,    5,    KT2_IDLE,
       27,    26,               28,    9,    25,
       27,    KT2_IDLE,         28,    5,    26,
       27,    25,               28,    9,    KT2_IDLE,
};
/* 4xGE(1..4) 2xF-XAUI(5,9) 2xF-HG[13](25,26) 2x21G[G.INT](27,28) */
uint32 kt2_tdm_56450speed_deprecated1[40]= {
       1000 ,1000 ,1000 ,1000,     /* 1 - 4 */
       10000,0000 ,0000 ,0000,     /* 5 - 8 */
       10000,0000 ,0000 ,0000,     /* 9 - 12 */
       0000 ,0000 ,0000 ,0000,     /* 13 - 16 */
       0000 ,0000 ,0000 ,0000,     /* 17 - 20 */
       0000 ,0000 ,0000 ,0000,     /* 21 - 24 */
       13000,13000,21000,21000,    /* 25 - 28 */
       0,    0,    0,    0,        /* 29 - 32 */
       0,    0,    0,    0,        /* 33 - 36 */
       0,    0,    0,    0};       /* 37 - 40 */
/* Deprecated=1 4xGE(1..4) 2xF-XAUI(5,9) 2xF-HG[13](25,26) 2x21G[G.INT](27,28)*/
soc_port_details_t kt2_port_details_deprecated1[]={
    { 1, 4,1,GE_PORT,2500},
    { 5, 9,4,XE_PORT,10000},
    { 25, 26,1,HG_PORT|STK_PORT,13000},
    { 27, 28,1,HG_PORT|LPHY_PORT,21000},
    {0,0,0,0} /* End */
};

/* bcm5645x_config=2 */

/* TDM-A2 Clock 178MHz */
/* 16x2.5G(1..8,25..26,35..40) + 2xHG21(27..28) */
/*        subcycle   1 2 3 4 5 
      1
      ..
      18                       
*/
uint32 kt2_tdm_56450config_deprecated2[90]= {
       27,     KT2_LPBK_PORT,  28,     1,      5,
       27,     KT2_CMIC_PORT,  28,     25,     26,
       27,     2,              28,     6,      35,
       27,     38,             28,     3,      7,
       27,     36,             28,     39,     4,
       27,     8,              28,     37,     40,
       27,     KT2_LPBK_PORT,  28,     1,      5,
       27,     25,             28,     26,     2,
       27,     6,              28,     35,     38,
       27,     3,              28,     7,      36,
       27,     KT2_CMIC_PORT,  28,     39,     4,
       27,     8,              28,     37,     40,
       27,     KT2_LPBK_PORT,  28,     1,      5,
       27,     25,             28,     26,     2,
       27,     6,              28,     35,     38,
       27,     3,              28,     7,      36,
       27,     39,             28,     4,      8,
       27,     37,             28,     40,     KT2_IDLE,
};
/* 16x 2.5G + 2x HG21  */
uint32 kt2_tdm_56450speed_deprecated2[40]= {
       2500,2500,2500 ,2500 ,     /* 1 - 4 */
       2500,2500,2500 ,2500 ,     /* 5 - 8 */
       0000,0000,0000 ,0000 ,     /* 9 - 12 */
       0000,0000,0000 ,0000 ,     /* 13 - 16 */
       0000,0000,0000 ,0000 ,     /* 17 - 20 */
       0000,0000,0000 ,0000 ,     /* 21 - 24 */
       2500,2500,21000,21000,     /* 25 - 28 */
       0000,0000,0000 ,0000 ,     /* 29 - 32 */
       0000,0000,2500 ,2500 ,     /* 33 - 36 */
       2500,2500,2500 ,2500 };    /* 37 - 40 */
/* Deprecated=2 16x 2.5G + 2x HG21 */
soc_port_details_t kt2_port_details_deprecated2[]={
    { 1, 8,1,GE_PORT,2500},
    {25,26,1,GE_PORT,2500},
    {35,40,1,GE_PORT,2500},
    {27,28,1,HG_PORT|STK_PORT,21000},
    {0,0,0,0} /* End */
};

/* bcm5645x_config=3 */

/* TDM-A4 Clock 178MHz */
/* 24xGE(1..24) 8x2.5G(25..26,35..40) 2x21G[G.INT](27..28) */
/*        subcycle   1 2 3 4 5 
      1
      ..
      18                       
*/
uint32 kt2_tdm_56450config_deprecated3[90]= {
       27,     KT2_LPBK_PORT,  28,     25,     26,
       27,     KT2_CMIC_PORT,  28,     35,     38,
       27,     1,              28,     36,     39,
       27,     5,              28,     37,     40,
       27,     9,              28,     13,     17,
       27,     21,             28,     2,      6,
       27,     KT2_LPBK_PORT,  28,     25,     26,
       27,     10,             28,     35,      38,
       27,     14,             28,     36,     39,
       27,     18,             28,     37,     40,
       27,     KT2_CMIC_PORT,  28,     22,     3,
       27,     7,              28,     11,     15,
       27,     KT2_LPBK_PORT,  28,     25,     26,
       27,     19,             28,     35,     38,
       27,     23,             28,     36,     39,
       27,     4,              28,     37,     40,
       27,     8,              28,     12,     16,
       27,     20,             28,     24,     KT2_IDLE,
};
/* 24xGE(1..24) 8x2.5G(25..26,35..40) 2x21G[G.INT](27..28) */
uint32 kt2_tdm_56450speed_deprecated3[40]= {
       1000,1000,1000 ,1000 ,     /* 1 - 4   */
       1000,1000,1000 ,1000 ,     /* 5 - 8   */
       1000,1000,1000 ,1000 ,     /* 9 - 12  */
       1000,1000,1000 ,1000 ,     /* 13 - 16 */
       1000,1000,1000 ,1000 ,     /* 17 - 20 */
       1000,1000,1000 ,1000 ,     /* 21 - 24 */
       2500,2500,21000,21000,     /* 25 - 28 */
       0000,0000,0000 ,0000 ,     /* 29 - 32 */
       0000,0000,2500 ,2500 ,     /* 33 - 36 */
       2500,2500,2500 ,2500 };    /* 37 - 40 */
/* Deprecated=3  24x GE + 8x 2.5G + 2x 21G[G.INT] */
soc_port_details_t kt2_port_details_deprecated3[]={
    {1,24,1,GE_PORT,1000},
    {25,26,1,GE_PORT,2500},
    {27,28,1,HG_PORT|LPHY_PORT,21000},
    {35,40,1,GE_PORT,2500},
    {0,0,0,0} /* End */
};

/* bcm5645x_config=4 */

/* TDM-A5 Clock 178MHz */
/* 4x XFI + 1x HG21    */
/*        subcycle   1 2 3 4 5 
      1
      ..
      18                       
*/
uint32 kt2_tdm_56450config_deprecated4[90]= {
0 /* TBD */
};
uint32 kt2_tdm_56450speed_deprecated4[40]= {
0 /* TBD */
};
/* Deprecated=4  4x XFI + 1x HG21 */
soc_port_details_t kt2_port_details_deprecated4[]={
    {1,13,4,XE_PORT,10000},
    {28,28,1,HG_PORT|STK_PORT,21000},
    {0,0,0,0} /* End */
};

/* bcm5645x_config=5 */

/* Deprecated=5  24x GE + 2x F-HG[13] + 2x F-HG[21] */
uint32 kt2_tdm_56450config_deprecated5[90]= {
       27,     26,     28,     25,             KT2_LPBK_PORT,
       27,     1,      28,     KT2_CMIC_PORT,  26,
       27,     25,     28,     5,              9,
       27,     26,     28,     13,             25,
       27,     17,     28,     21,             26,
       27,     25,     28,     2,              6,
       27,     26,     28,     KT2_LPBK_PORT,  25,
       27,     10,     28,     14,             26,
       27,     25,     28,     18,             22,
       27,     26,     28,     3,              25,
       27,     7,      28,     KT2_CMIC_PORT,  26,
       27,     25,     28,     11,             15,
       27,     26,     28,     KT2_LPBK_PORT,  25,
       27,     19,     28,     23,             26,
       27,     25,     28,     4,              8,
       27,     26,     28,     12,             25,
       27,     16,     28,     20,             26,
       27,     25,     28,     24,             KT2_IDLE
};

/* 24xGE(1..24) 2xF-HG[13](25..26) 2xF-HG[21](27..28) */
uint32 kt2_tdm_56450speed_deprecated5[40]= {
       1000 ,1000 ,1000 ,1000,     /* 1 - 4 */
       1000 ,1000 ,1000 ,1000,     /* 5 - 8 */
       1000 ,1000 ,1000 ,1000,     /* 9 - 12 */
       1000 ,1000 ,1000 ,1000,     /* 13 - 16 */
       1000 ,1000 ,1000 ,1000,     /* 17 - 20 */
       1000 ,1000 ,1000 ,1000,     /* 21 - 24 */
       13000,13000,21000,21000,    /* 25 - 28 */
       0,    0,    0,    0,        /* 29 - 32 */
       0,    0,    0,    0,        /* 33 - 36 */
       0,    0,    0,    0};       /* 37 - 40 */
/* Deprecated=5  24x GE + 2x F-HG[13] + 2x F-HG[21] */
soc_port_details_t kt2_port_details_deprecated5[]={
    {1,24,1,GE_PORT,1000},
    {25,26,1,HG_PORT|STK_PORT,13000}, 
    {27,28,1,HG_PORT|STK_PORT,21000},
    {0,0,0,0} /* End */
};

const static uint32 kt2_tdm_56450AA_ref[88]= {
1,              5,      9,      13,     17,     21,25,26,27,28,KT2_LPBK_PORT,
2,              6,      10,     14,     18,     22,35,38,32,29,KT2_CMIC_PORT,
3,              7,      11,     15,     19,     23,36,39,33,30,KT2_IDLE,
4,              8,      12,     16,     20,     24,37,40,34,31,KT2_IDLE1,
KT2_IDLE1,      1,      5,      9,      13,     17,21,25,26,27,28,
KT2_LPBK_PORT,  2,      6,      10,     14,     18,22,35,38,32,29,
KT2_CMIC_PORT,  3,      7,      11,     15,     19,23,36,39,33,30,
KT2_IDLE,       4,      8,      12,     16,     20,24,37,40,34,31
};


const static uint32 kt2_tdm_56450A_ref[88]= {
1,              5,      9,      27,     28,     13,17,21,33,30,KT2_LPBK_PORT,
1,              5,      9,      27,     28,     13,17,21,33,30,KT2_CMIC_PORT,
1,              5,      9,      27,     28,     13,17,21,33,30,KT2_OLP_PORT,
1,              5,      9,      27,     28,     13,17,21,33,30,KT2_IDLE1,
KT2_IDLE1,      1,      5,      9,      27,     28,13,17,21,33,30,
KT2_LPBK_PORT,  1,      5,      9,      27,     28,13,17,21,33,30,
KT2_CMIC_PORT,  1,      5,      9,      27,     28,13,17,21,33,30,
KT2_OLP_PORT,   1,      5,      9,      27,     28,13,17,21,33,30
};
const static uint32 kt2_tdm_56450A1_ref[108]= { 
9,             25, 27,            28,            13, 26, 9,  27,       28,
KT2_LPBK_PORT, 25, KT2_CMIC_PORT, 27,            28, 26, 13, 9,        27,
28,            25, KT2_IDLE1,     KT2_IDLE1,     27, 26, 28, 13,       9,
27,            25, 28,            3,             13, 26, 27, 28,   KT2_OLP_PORT,
9,             25, 27,            28,            5,  26, 13, 27,       28,
KT2_LPBK_PORT, 25, 9,             27,            28, 26, 7,  13,       27,
28,            25, 1,             9,             27, 26, 28, KT2_IDLE, 13,
27,            25, 28,            KT2_CMIC_PORT, 9,  26, 27, 28,   KT2_OLP_PORT,
13,            25, 27,            28,            6,  26, 9,  27,        28,
KT2_LPBK_PORT, 25, 13,            27,            28, 26, 8,  9,         27,
28,            25, 2,             13,            27, 26, 28, KT2_IDLE,  9,
27,            25, 28,            4,             13, 26, 27, 28,   KT2_OLP_PORT
};
const static uint32 kt2_tdm_56450A2_ref[99]= {
KT2_IDLE1,     25, 9,            26, 21,            27, 17,            28,   13,
KT2_CMIC_PORT, 25, KT2_OLP_PORT, 26, 9,             27, 21,            28,   17,
1,             25, 13,           26, KT2_LPBK_PORT, 27, 9,             28,   21,
2,             25, 17,           26, 13,            27, KT2_CMIC_PORT, 28,   9,
3,             25, 21,           26, 17,            27, 13,            28,   8,
9,             25, KT2_OLP_PORT, 26, 21,            27, 17,            28,   13,
4,             25, 9,            26, KT2_LPBK_PORT, 27, 21,            28,   17,
13,            25, 5,            26, 9,             27, KT2_CMIC_PORT, 28,   21,
17,            25, 13,           26, 6,             27, 9,             28,   
                                                                  KT2_LPBK_PORT,
21,            25, 17,           26, 13,            27, KT2_OLP_PORT,  28,   9,
7,             25, 21,           26, 17,            27, 13,            28,   
                                                                   KT2_IDLE1
};

const static uint32 kt2_tdm_56450D_ref[20] = {
KT2_IDLE1,	1,	5,	27,	28,	
2,		6,	27,	28,	KT2_LPBK_PORT,
3,		7,	27,	28,	KT2_CMIC_PORT,
4,		8,	27,	28,	KT2_IDLE1,
};

/* bcm5645x_config=6 */

/* 8 x F.XAUI(1,,5,9,13,17,21,25,26)    2 x W10G = 2 x XFI (27,28) */
static uint32 kt2_tdm_56450A_1[88]={0}; /* Will be filled-up at run time */
static uint32 kt2_tdm_56450speed_1[40]= {
       10000,0000,0000,0000,     /* 1  - 4   */
       10000,0000,0000,0000,     /* 5  - 8   */
       10000,0000,0000,0000,     /* 9  - 12  */
       10000,0000,0000,0000,     /* 13 - 16  */
       10000,0000,0000,0000,     /* 17 - 20  */
       10000,0000,0000,0000,     /* 21 - 24  */
       10000,10000,10000,10000,  /* 25 - 28  */
       0000,0000,0000,0000,      /* 29 - 32  */
       0000,0000,0000,0000,      /* 33 - 36  */
       0000,0000,0000,0000};     /* 37 - 40  */

/* Config=1 8xF.XAUI  2xW10G */
/* 2 XFI */
soc_port_details_t kt2_port_details_config1[]={
    {1,25,4,XE_PORT,10000}, /* 7 F.XAUI */
    {26,28,1,XE_PORT,10000},/* 1 F.XAUI + 2 XFI */
    {0,0,0,0} /* End */
};  

/* bcm5645x_config=7 */

/* 7 x F.XAUI + 1 x XAUI(1,5,9,13,17,21,25,26)  2 x W10G = 2 x XFI (27,28) */
static uint32 kt2_tdm_56450A_2[88]={0}; /* Will be filled-up at run time */
static uint32 kt2_tdm_56450speed_2[40]= {
       10000,0000,0000,0000,     /* 1  - 4   */
       10000,0000,0000,0000,     /* 5  - 8   */
       10000,0000,0000,0000,     /* 9  - 12  */
       10000,0000,0000,0000,     /* 13 - 16  */
       10000,0000,0000,0000,     /* 17 - 20  */
       10000,0000,0000,0000,     /* 21 - 24  */
       10000,10000,10000,10000,  /* 25 - 28  */
       0000,0000,0000,0000,      /* 29 - 32  */
       0000,0000,0000,0000,      /* 33 - 36  */
       0000,0000,0000,2500};     /* 37 - 40  */
/* Config=2 7 x F.XAUI + 1 x XAUI + 2 x W10G */
/* 2XFI + OLP */
soc_port_details_t kt2_port_details_config2[]={
    {1,25,4,XE_PORT,10000}, /* 7 F.XAUI */
    {26,28,1,XE_PORT,10000},/* 1 XAUI Fixed + 2 XFI*/
    {40,40,1,OLP_PORT,2500},/* OLP */
    {0,0,0,0} /* End */
};

/* bcm5645x_config=8 */

/* 7 x F.XAUI(1,5,9,13,17,21,25)        1xW10G + 1xW20G (3 x XFI)(27,28,30) */
static uint32 kt2_tdm_56450A_3[88]={0}; /* Will be filled-up at run time */
static uint32 kt2_tdm_56450speed_3[40]= {
       10000,0000,0000,0000,     /* 1  - 4   */
       10000,0000,0000,0000,     /* 5  - 8   */
       10000,0000,0000,0000,     /* 9  - 12  */
       10000,0000,0000,0000,     /* 13 - 16  */
       10000,0000,0000,0000,     /* 17 - 20  */
       10000,0000,0000,0000,     /* 21 - 24  */
       10000,0000,10000,10000,   /* 25 - 28  */
       0000,10000,0000,0000,     /* 29 - 32  */
       0000,0000,0000,0000,      /* 33 - 36  */
       0000,0000,0000,2500};     /* 37 - 40  */
/* Config=3 7 x F.XAUI	1 x W10G + 1 x W20G */
/* 3XFI + OLP */
soc_port_details_t kt2_port_details_config3[]={
    {1,25,4,XE_PORT,10000}, /* 7 F.XAUI */
    {27,28,1,XE_PORT,10000},/* 2 XFI */
    {30,30,1,XE_PORT,10000},/* 1 XFI */
    {40,40,1,OLP_PORT,2500},/* OLP */
    {0,0,0,0} /* End */
};

/* bcm5645x_config=9 */

/* 6 x F.XAUI(1,5,9,13,17,21)   2 x W20G (4 x XFI) 27,33,28,30 */
static uint32 kt2_tdm_56450A_4[88]={0};
static uint32 kt2_tdm_56450speed_4[40]= {
       10000,0000,0000,0000,     /* 1  - 4   */
       10000,0000,0000,0000,     /* 5  - 8   */
       10000,0000,0000,0000,     /* 9  - 12  */
       10000,0000,0000,0000,     /* 13 - 16  */
       10000,0000,0000,0000,     /* 17 - 20  */
       10000,0000,0000,0000,     /* 21 - 24  */
       0000,0000,10000,10000,    /* 25 - 28  */
       0000,10000,0000,0000,     /* 29 - 32  */
       10000,0000,0000,0000,     /* 33 - 36  */
       0000,0000,0000,2500};     /* 37 - 40  */
/* Config=4 6 x F.XAUI	2 x W20G */
/* 4XFI + OLP */
soc_port_details_t kt2_port_details_config4[]={
    {1,21,4,XE_PORT,10000}, /* 6 F.XAUI */
    {27,28,1,XE_PORT,10000},/* 2 XFI */
    {30,33,3,XE_PORT,10000}, /* 2 XFI */
    {40,40,1,OLP_PORT,2500},/* OLP */
    {0,0,0,0} /* End */
};

/* bcm5645x_config=10 */

/* 4 x F.XAUI(1,5,9,13) 1 x W20G(27,33) + 1 x W40G (28,29,30,31)         */
/* MXQ8:27,33 MXQ9:28,30 */
/* ##MXQ7:26,39 Indirectly used */
/* ##MXQ6(25,36) Not used       */
static uint32 kt2_tdm_56450A_5[88]={0};
static unsigned int kt2_tdm_56450speed_5[40]= {
       10000,0000,0000,0000,     /* 1  - 4   */
       10000,0000,0000,0000,     /* 5  - 8   */
       10000,0000,0000,0000,     /* 9  - 12  */
       10000,0000,0000,0000,     /* 13 - 16  */
       0000,0000,0000,0000,      /* 17 - 20  */
       0000,0000,0000,0000,      /* 21 - 24  */
       0000,10000,10000,10000,   /* 25 - 28  */
       10000,10000,10000,0000,     /* 29 - 32  */
       10000,0000,0000,00000,    /* 33 - 36  */
       0000,0000,10000,2500};    /* 37 - 40  */
/* Config=5 4 x F.XAUI	1 x W20G + 1 x W40G */
/* 6XFI + OLP */
soc_port_details_t kt2_port_details_config5[]={
    {1,13,4,XE_PORT,10000}, /* 4 F.XAUI */
    {27,33,6,XE_PORT,10000}, /* 2 XFI */
    {28,31,1,XE_PORT,10000}, /* 4 XFI */
    {40,40,1,OLP_PORT,2500},/* OLP */
    {0,0,0,0} /* End */
};

/* bcm5645x_config=11 */

/* 2 x F.XAUI (1,5)     2 x W40G=8 XFI (25,36,26,39,27,33,28,30) */
/* MXQ6=25,36      MXQ7=26,39      MXQ8=27,33      MXQ9=28,30 */
static uint32 kt2_tdm_56450A_6[88]={0};
static uint32 kt2_tdm_56450speed_6[40]= {
       10000,0000,0000,0000,     /* 1  - 4   */
       10000,0000,0000,0000,     /* 5  - 8   */
       0000,0000,0000,0000,      /* 9  - 12  */
       0000,0000,0000,0000,      /* 13 - 16  */
       0000,0000,0000,0000,      /* 17 - 20  */
       0000,0000,0000,0000,      /* 21 - 24  */
       10000,10000,10000,10000,  /* 25 - 28  */
       0000,10000,0000,0000,     /* 29 - 32  */
       10000,0000,0000,10000,    /* 33 - 36  */
       0000,0000,10000,2500};    /* 37 - 40  */
/* Config=6 2 x F.XAUI	2 x W40G */
/* 8XFI + OLP */
/* ##MXQ7:26,39 Indirectly used */
/* ##MXQ6(25,36) Indirectly used       */
soc_port_details_t kt2_port_details_config6[]={
    {1,5,4,XE_PORT,10000},
    {27,27,1,XE_PORT,10000},
    {32,34,1,XE_PORT,10000},
    {28,31,1,XE_PORT,10000},
    {40,40,1,OLP_PORT,2500},/* OLP */
    {0,0,0,0} /* End */
};

/* bcm5645x_config=12 */

/* ATTN:Suspicious Configuration.Need to be double checked! */
/* 24GE + 1 F.XAUI      W40G + W20G (25,36,26,39,27,33,28,30)           */
/* MXQ6:25 MXQ8:27,33 MXQ9:28,30 ##MXQ7(26,39) Indirectly used*/

static uint32 kt2_tdm_56450A_7[88]={0};
static uint32 kt2_tdm_56450speed_7[40]= {
       1000,1000,1000,1000,     /* 1  - 4   */
       1000,1000,1000,1000,     /* 5  - 8   */
       1000,1000,1000,1000,     /* 9  - 12  */
       1000,1000,1000,1000,     /* 13 - 16  */
       1000,1000,1000,1000,     /* 17 - 20  */
       1000,1000,1000,1000,     /* 21 - 24  */
       10000,10000,10000,10000,  /* 25 - 28  */
       10000,10000,10000,0000,    /* 29 - 32  */
       10000,0000,0000,0000,   /* 33 - 36  */
       0000,0000,10000,2500};    /* 37 - 40  */
/* Config=7 24 x GE   1 x F.XAUI 1 x W40G  1 x W20G */
/* SP3 */
/* Assuming 6 XFI + OLP */
/* MXQ6:25,36 MXQ8:27,33 MXQ9:28,30 ##MXQ7(26,39) Not used*/
soc_port_details_t kt2_port_details_config7[]={
    {1,24,1,GE_PORT,1000},
    {25,25,1,XE_PORT,10000}, /* 1 F.XAUI */
    {27,33,6,XE_PORT,10000}, /* 1 XFI */
    {28,31,1,XE_PORT,10000}, /* 3 XFI */
    {40,40,1,OLP_PORT,2500},/* OLP */
    {0,0,0,0} /* End */
};

/* bcm5645x_config=13 */

/* 8x GE + 2x F-XAUI + 2x HG13 + 2x HG21 + OLP */
static uint32 kt2_tdm_56450A1_8[108]={0};
static uint32 kt2_tdm_56450speed_8[40]= {
       1000,1000,1000,1000,     /* 1  - 4   */
       1000,1000,1000,1000,     /* 5  - 8   */
       10000,0000,0000,0000,    /* 9  - 12  */
       10000,0000,0000,0000,    /* 13 - 16  */
       0000,0000,0000,0000,     /* 17 - 20  */
       0000,0000,0000,0000,     /* 21 - 24  */
       13000,13000,21000,21000, /* 25 - 28  */
       0000,0000,0000,0000,     /* 29 - 32  */
       0000,0000,0000,0000,     /* 33 - 36  */
       0000,0000,0000,2500};    /* 37 - 40  */
/* Config=8 8 x GE 2 x F.XAUI 1 x F.HG13  1 x X.HG13  2 x W21G */
soc_port_details_t kt2_port_details_config8[]={
    {1,8,1,GE_PORT,1000},
    {9,13,4,XE_PORT,10000}, /* 2 F.XAUI */
    {25,26,1,HG_PORT|STK_PORT,13000}, /* 26 Fixed XAUI */
    {27,28,1,HG_PORT|STK_PORT,21000},
    {40,40,1,OLP_PORT,2500},/* OLP */
    {0,0,0,0} /* End */
};

/* bcm5645x_config=14 */

/* 24x GE + 2x HG13 + 2x HG21 + OLP */
static uint32 kt2_tdm_56450A1_9[108]={0};
static uint32 kt2_tdm_56450speed_9[40]= {
       1000,1000,1000,1000,     /* 1  - 4   */
       1000,1000,1000,1000,     /* 5  - 8   */
       1000,1000,1000,1000,     /* 9  - 12  */
       1000,1000,1000,1000,     /* 13 - 16  */
       1000,1000,1000,1000,     /* 17 - 20  */
       1000,1000,1000,1000,     /* 21 - 24  */
       13000,13000,21000,21000, /* 25 - 28  */
       0000,0000,0000,0000,     /* 29 - 32  */
       0000,0000,0000,0000,     /* 33 - 36  */
       0000,0000,0000,2500};    /* 37 - 40  */
/* Config=9 24 x GE 1 x F.HG13  1 x X.HG13  2 x W21G */
soc_port_details_t kt2_port_details_config9[]={
    {1,24,1,GE_PORT,1000},
    {25,26,1,HG_PORT|STK_PORT,13000},  /* 26 Fixed XAUI */
    {27,28,1,HG_PORT|STK_PORT,21000},
    {40,40,1,OLP_PORT,2500},/* OLP */
    {0,0,0,0} /* End */
};


/* bcm5645x_config=15 */

/* 8 x 1GbE(1..8) + 4 x F.XAUI(9,13,17,21) + 
   1 x HG[13] + 1 x F.HG[13](25,26)   2 x W13G(27,28)(NO OLP) */
static uint32 kt2_tdm_56450A2_10[99]={0};
static uint32 kt2_tdm_56450speed_10[40]= {
       1000,1000,1000,1000,     /* 1  - 4   */
       1000,1000,1000,1000,     /* 5  - 8   */
       10000,0000,0000,0000,    /* 9  - 12  */
       10000,0000,0000,0000,    /* 13 - 16  */
       10000,0000,0000,0000,    /* 17 - 20  */
       10000,0000,0000,0000,    /* 21 - 24  */
       13000,13000,13000,13000, /* 25 - 28  */
       0000,0000,0000,0000,     /* 29 - 32  */
       0000,0000,0000,0000,     /* 33 - 36  */
       0000,0000,0000,0000};    /* 37 - 40  */
/* Config=10 8 x GE 4 x F.XAUI  2 x F.HG13  2 x W13G */
soc_port_details_t kt2_port_details_config10[]={
    {1,8,1,GE_PORT,1000},
    {9,21,4,XE_PORT,10000}, /* 4  F.XAUI */
    {25,28,1,HG_PORT|STK_PORT,13000},
    {0,0,0,0} /* End */
};

/* bcm5645x_config=16 */

/* 8 x 1GbE(1..8) + 4 x F.XAUI(9,13,17,21) + 1 x HG[13] + */
/* 1 x F.HG[13](25,26)   2 x W13G(27,28) */
static uint32 kt2_tdm_56450A2_11[99]={0};
static uint32 kt2_tdm_56450speed_11[40]= {
       1000,1000,1000,1000,     /* 1  - 4   */
       1000,1000,1000,1000,     /* 5  - 8   */
       10000,0000,0000,0000,    /* 9  - 12  */
       10000,0000,0000,0000,    /* 13 - 16  */
       10000,0000,0000,0000,    /* 17 - 20  */
       10000,0000,0000,0000,    /* 21 - 24  */
       13000,13000,13000,13000, /* 25 - 28  */
       0000,0000,0000,0000,     /* 29 - 32  */
       0000,0000,0000,0000,     /* 33 - 36  */
       0000,0000,0000,2500};    /* 37 - 40  */
/* Config=11 8 x 1GbE + 4 x F.XAUI + 1 x F.HG[13] + 1 x X.HG[13] 2 x W13G */
soc_port_details_t kt2_port_details_config11[]={
    {1,8,1,GE_PORT,1000},
    {9,21,4,XE_PORT,10000}, /* 4  F.XAUI */
    {25,28,1,HG_PORT|STK_PORT,13000},
    {40,40,1,OLP_PORT,2500},/* OLP */
    {0,0,0,0} /* End */
};

/* bcm5645x_config=17 */

/* 8 x 1GbE(1..8) + 4 x F.XAUI(9,13,17,21) + 1 x HG[13](25) */
/* 1x W13G(27) 1xW21G(28) */
static uint32 kt2_tdm_56450A2_12[99]={0};
static uint32 kt2_tdm_56450speed_12[40]= {
       1000,1000,1000,1000,     /* 1  - 4   */
       1000,1000,1000,1000,     /* 5  - 8   */
       10000,0000,0000,0000,    /* 9  - 12  */
       10000,0000,0000,0000,    /* 13 - 16  */
       10000,0000,0000,0000,    /* 17 - 20  */
       10000,0000,0000,0000,    /* 21 - 24  */
       13000,0000,13000,21000,  /* 25 - 28  */
       0000,0000,0000,0000,     /* 29 - 32  */
       0000,0000,0000,0000,     /* 33 - 36  */
       0000,0000,0000,2500};    /* 37 - 40  */
/* Config=12 8 x 1GbE + 4 x F.XAUI + 1 x F.HG[13] 1 x W13G 1 x W21G*/
/* 2xHG13 + 1xHG21 */
soc_port_details_t kt2_port_details_config12[]={
    {1,8,1,GE_PORT,1000},
    {9,21,4,XE_PORT,10000}, /* 4 F.XAUI */
    {25,27,2,HG_PORT|STK_PORT,13000},
    {28,28,1,HG_PORT|STK_PORT,21000},
    {40,40,1,OLP_PORT,2500},/* OLP */
    {0,0,0,0} /* End */
};

/* bcm5645x_config=18 */

/* 8 x 1GbE(1..8) + 4 x F.XAUI(9,13,17,21) + 2 x W21G(27,28) */
static uint32 kt2_tdm_56450A2_13[99]={0};
static uint32 kt2_tdm_56450speed_13[40]= {
       1000,1000,1000,1000,     /* 1  - 4   */
       1000,1000,1000,1000,     /* 5  - 8   */
       10000,0000,0000,0000,    /* 9  - 12  */
       10000,0000,0000,0000,    /* 13 - 16  */
       10000,0000,0000,0000,    /* 17 - 20  */
       10000,0000,0000,0000,    /* 21 - 24  */
       0000,0000,21000,21000,   /* 25 - 28  */
       0000,0000,0000,0000,     /* 29 - 32  */
       0000,0000,0000,0000,     /* 33 - 36  */
       0000,0000,0000,2500};    /* 37 - 40  */
/* Config=13 8 x 1GbE + 4 x F.XAUI + 2 x W21G*/
soc_port_details_t kt2_port_details_config13[]={
    {1,8,1,GE_PORT,1000},
    {9,21,4,XE_PORT,10000},
    {27,28,1,HG_PORT|STK_PORT,21000},
    {0,0,0,0} /* End */
};


/* bcm5645x_config=19 */


/* 8 x 1GbE(1..8) + 2 x F.XAUI(9,13) + 2xHG13(25,26) 2 x W20G(27,28) */
static uint32 kt2_tdm_56450A2_14[99]={0};
static uint32 kt2_tdm_56450speed_14[40]= {
       1000,1000,1000,1000,     /* 1  - 4   */
       1000,1000,1000,1000,     /* 5  - 8   */
       10000,0000,0000,0000,    /* 9  - 12  */
       10000,0000,0000,0000,    /* 13 - 16  */
       0000,0000,0000,0000,     /* 17 - 20  */
       0000,0000,0000,0000,     /* 21 - 24  */
       13000,13000,13000,13000, /* 25 - 28  */
       0000,0000,0000,0000,     /* 29 - 32  */
       0000,0000,0000,0000,     /* 33 - 36  */
       0000,0000,0000,2500};    /* 37 - 40  */
/* Config=14 8 x 1GbE + 2 x F.XAUI + 1 x F.HG[13] + 1 x X.HG[13]  2 x W20G */
/* TN2 */
/* Actually 4 XFI but treating as 2HG13 only!! */
soc_port_details_t kt2_port_details_config14[]={
    {1,8,1,GE_PORT,1000},
    {9,13,4,XE_PORT,10000}, /* 2 F.XAUI */
    {25,28,1,HG_PORT|STK_PORT,13000},
    {40,40,1,OLP_PORT,2500},/* OLP */
    {0,0,0,0} /* End */
};

/* bcm5645x_config=20 */

/* 16 x 1GbE(1..16) + 1 x F.XAUI(17) + 2xHG13(25,26) 2 x W21G(27,28) */
static uint32 kt2_tdm_56450A2_15[99]={0};
static uint32 kt2_tdm_56450speed_15[40]= {
       1000,1000,1000,1000,     /* 1 .- 4   */
       1000,1000,1000,1000,     /* 5  - 8   */
       1000,1000,1000,1000,     /* 9  - 12  */
       1000,1000,1000,1000,     /* 13 - 16  */
       10000,0000,0000,0000,    /* 17 - 20  */
       0000,0000,0000,0000,     /* 21 - 24  */
       13000,13000,13000,13000, /* 25 - 28  */
       0000,0000,0000,0000,     /* 29 - 32  */
       0000,0000,0000,0000,     /* 33 - 36  */
       0000,0000,0000,2500};    /* 37 - 40  */
/* Config=15 16 x 1GbE + 1 x F.XAUI + 1 x F.HG[13] + 1 x X.HG[13]  2 x W20G */
/* TN2 */
/* Assuming 4 XFI */
soc_port_details_t kt2_port_details_config15[]={
    {1,16,1,GE_PORT,1000},
    {17,17,1,XE_PORT,10000},
    {25,28,1,HG_PORT|STK_PORT,13000},
    {40,40,1,OLP_PORT,2500},/* OLP */
    {0,0,0,0} /* End */
};

/* QT Setup specific :: bcm5645x_config=21 */

/* 6 x F.XAUI(1,5,9,13,17,21)   2 x W20G 27,28 */
static uint32 kt2_tdm_56450A_16[88]={0};
static uint32 kt2_tdm_56450speed_16[40]= {
       10000,0000,0000,0000,     /* 1  - 4   */
       10000,0000,0000,0000,     /* 5  - 8   */
       10000,0000,0000,0000,     /* 9  - 12  */
       10000,0000,0000,0000,     /* 13 - 16  */
       10000,0000,0000,0000,     /* 17 - 20  */
       10000,0000,0000,0000,     /* 21 - 24  */
       0000,0000,10000,10000,    /* 25 - 28  */
       0000,0000,0000,0000,     /* 29 - 32  */
       0000,0000,0000,0000,     /* 33 - 36  */
       0000,0000,0000,2500};     /* 37 - 40  */
/* Config=4(bit different W20G) 6 x F.XAUI 2 x W20G(10g,10g) */
/* OLP */
soc_port_details_t kt2_port_details_config16[]={
    {1,21,4,XE_PORT,10000}, /* 6 F.XAUI */
    {27,28,1,XE_PORT,10000},/* 2 XFI */
    {40,40,1,OLP_PORT,2500},/* OLP */
    {0,0,0,0} /* End */
};

/* bcm5645x_config=22 */

/* 6 x F.XAUI(1,5,9,13,17,21)   2 x W20G (4 x XFI) 27,33,28,30 */
static uint32 kt2_tdm_56450A_17[88]={0};
static uint32 kt2_tdm_56450speed_17[40]= {
       10000,0000,0000,0000,     /* 1  - 4   */
       10000,0000,0000,0000,     /* 5  - 8   */
       10000,0000,0000,0000,     /* 9  - 12  */
       10000,0000,0000,0000,     /* 13 - 16  */
       10000,0000,0000,0000,     /* 17 - 20  */
       10000,0000,0000,0000,     /* 21 - 24  */
       0000,0000,10000,10000,    /* 25 - 28  */
       0000,10000,0000,0000,     /* 29 - 32  */
       10000,0000,0000,0000,     /* 33 - 36  */
       0000,0000,0000,2500};     /* 37 - 40  */
/* Config=4 6 x F.XAUI  2 x W20G */
/* 4XFI + OLP */
soc_port_details_t kt2_port_details_config17[]={
    {1,21,4,XE_PORT,10000}, /* 6 F.XAUI */
    {27,28,1,XE_PORT|LPHY_PORT,10000},/* 2 XFI */
    {30,33,3,XE_PORT|LPHY_PORT,10000}, /* 2 XFI */
    {40,40,1,OLP_PORT,2500},/* OLP */
    {0,0,0,0} /* End */
};

/* bcm5645x_config=23 */

/* 24x GE + 2x HG13 + 2x HG21 + OLP */
static uint32 kt2_tdm_56450A1_18[108]={0};
static uint32 kt2_tdm_56450speed_18[40]= {
       1000,1000,1000,1000,     /* 1  - 4   */
       1000,1000,1000,1000,     /* 5  - 8   */
       1000,1000,1000,1000,     /* 9  - 12  */
       1000,1000,1000,1000,     /* 13 - 16  */
       1000,1000,1000,1000,     /* 17 - 20  */
       1000,1000,1000,1000,     /* 21 - 24  */
       13000,13000,21000,21000, /* 25 - 28  */
       0000,0000,0000,0000,     /* 29 - 32  */
       0000,0000,0000,0000,     /* 33 - 36  */
       0000,0000,0000,2500};    /* 37 - 40  */
/* Config=9 24 x GE 1 x F.HG13  1 x X.HG13  2 x W21G */
soc_port_details_t kt2_port_details_config18[]={
    {1,24,1,GE_PORT,1000},
    {25,26,1,HG_PORT|STK_PORT,13000},  /* 26 Fixed XAUI */
    {27,28,1,HG_PORT|LPHY_PORT,21000},
    {40,40,1,OLP_PORT,2500},/* OLP */
    {0,0,0,0} /* End */
};

/* bcm5645x_config=24 */
/* All 40 ports with 2.5G */
static uint32 kt2_tdm_56450AA_19[88]={0};
static uint32 kt2_tdm_56450speed_19[40]= {
       2500,2500,2500,2500,     /* 1  - 4   */
       2500,2500,2500,2500,     /* 5  - 8   */
       2500,2500,2500,2500,     /* 9  - 12  */
       2500,2500,2500,2500,     /* 13 - 16  */
       2500,2500,2500,2500,     /* 17 - 20  */
       2500,2500,2500,2500,     /* 21 - 24  */
       2500,2500,2500,2500, /* 25 - 28  */
       2500,2500,2500,2500,     /* 29 - 32  */
       2500,2500,2500,2500,     /* 33 - 36  */
       2500,2500,2500,2500};    /* 37 - 40  */
soc_port_details_t kt2_port_details_config19[]={
    {1,24,1,GE_PORT,2500},
    {25,26,1,GE_PORT,2500},
    {27,34,1,GE_PORT|LPHY_PORT,2500},
    {35,40,1,GE_PORT,2500},
    {0,0,0,0} /* End */
};
/* bcm5645x_config=25 */
/* 8x1g(MXQ0,1) + 2x10g(WC0,WC1) */
static uint32 kt2_tdm_56450DA_20[32]={0};
static uint32 kt2_tdm_56450speed_20[40]= {
       1000,1000,1000,1000,    /* 1  - 4   */
       1000,1000,1000,1000,     /* 5  - 8   */
       0000,0000,0000,0000,     /* 9  - 12  */
       0000,0000,0000,0000,     /* 13 - 16  */
       0000,0000,0000,0000,     /* 17 - 20  */
       0000,0000,0000,0000,     /* 21 - 24  */
       0000,0000,10000,10000,    /* 25 - 28  */
       0000,0000,0000,0000,     /* 29 - 32  */
       0000,0000,0000,0000,     /* 33 - 36  */
       0000,0000,0000,0000};    /* 37 - 40  */
soc_port_details_t kt2_port_details_config20[]={
    {1, 8,1,GE_PORT,1000},
    {27,28,1,XE_PORT,10000},
    {0,0,0,0} /* End */
};


/* TDM-A Clock 178MHz */
/* 24xGE(1..24) 2xF-HG[13](25..26) 2xF-HG[21](27..28) */
/*        subcycle   1 2 3 4 5 
      1
      ..
      18                       
*/

static uint32 kt2_tdm_mxqblock_ports_used[KT2_MAX_MXQBLOCKS]={0};
static uint32 kt2_tdm_position_mxq_mask[90]={0};
static uint32 kt2_port_to_mxqblock[40]={0,0,0,0, /* 1-4   mxq0*/
                                        1,1,1,1, /* 5-8   mxq1*/
                                        2,2,2,2, /* 9-12  mxq2*/
                                        3,3,3,3, /* 13-16 mxq3*/
                                        4,4,4,4, /* 17-20 mxq4*/
                                        5,5,5,5, /* 21-24 mxq5*/
                                        6,7,8,9, /* 25-28 mxq6-9 */
                                        9,9,9,8, /* 29-32 mxq9,9,9-8 */
                                        8,8,6,6, /* 33-36 mxq8,8,6-6 */
                                        6,7,7,7  /* 37-40 mxq6,7,7,7 */
                                       };
kt2_tdm_pos_info_t kt2_tdm_pos_info[KT2_MAX_MXQBLOCKS]={{0}};
uint32 kt2_port_to_mxqblock_subports[40]={0,1,2,3, /* 1-4   mxq0*/
                                                 0,1,2,3, /* 5-8   mxq1*/
                                                 0,1,2,3, /* 9-12  mxq2*/
                                                 0,1,2,3, /* 13-16 mxq3*/
                                                 0,1,2,3, /* 17-20 mxq4*/
                                                 0,1,2,3, /* 21-24 mxq5*/
                                                 0,0,0,0, /* 25-28 mxq6-9 */
                                                 1,2,3,1, /* 29-32 mxq9,9,9-8 */
                                                 2,3,1,2, /* 33-36 mxq8,8,6-6 */
                                                 3,1,2,3  /* 37-40 mxq6,7,7,7 */
                                                };
uint8 kt2_tdm_update_flag=1;



uint32 kt2_tdm_0[78] = { 1,9,25,26,27,28,
                        17,2,25,26,27,28,
                        0,10,18,25,26,27,
                        28,3,11,25,26,27,
                        28,35,19,4,25,26,
                        27,28,12,20,25,26,
                        27,28,63,5,13,25,
                        26,27,28,21,6,25,
                        26,27,28,0,14,22,
                        25,26,27,28,7,15,
                        25,26,27,28,35,23,
                        8,25,26,27,28,16,
                        24,25,26,27,28,35
                      };


uint32 kt2_mxqblock_ports[KT2_MAX_MXQBLOCKS][KT2_MAX_MXQPORTS_PER_BLOCK]=
             {{1,2,3,4},
              {5,6,7,8},
              {9,10,11,12},
              {13,14,15,16},
              {17,18,19,20},
              {21,22,23,24},
              {25,35,36,37},
              {26,38,39,40},
              {27,32,33,34},
              {28,29,30,31}
             };
tdm_cycles_info_t kt2_tdm_cycles_info[KT2_MAX_TDM_FREQUENCY][KT2_MAX_SPEEDS]={
                  /*0:80MHz : 16Cycles*/
                  {{16,16,1},{16,16,1},{16,16,1},{4,3,4},
                   {3,2,6},{2,1,8},{2,1,8}},      

                  /*1:110MHz: 20Cycles*/
                  {{20,20,1},{20,20,1},{20,20,1},{6,4,4},
                   {4,3,5},{3,2,7},{3,2,7}},      

                  /*2:155MHz: 84Cycles*/
                  {{84,84,1},{42,41,2},{33,26,3},{9,8,10},
                   {7,6,12},{4,3,21},{4,3,21}},  

                  /*3:185MHz: 80Cycles*/
                  {{80,80,1},{50,39,2},{40,39 ,2 },{10,9,8},
                   {8,7,10},{5,4,16},{5,4,16}},  

                  /*4:185MHz: 90Cycles*/
                  {{90,90,1},{52,44,2},{39,29,3},{10,9,9}, 
                   {8,7,12},{5,4,18},{5,4,18}},  
                  /*5:205MHz: 108*/
                  {{108,108,1},{56,52,2},{44,34,3},{11,10,10},
                   {9,9,12},{5,4,21},{5,4,21}},  
                  /*6:205MHz: 88*/
                  {{88,88,1},{56,42,2},{44,42,2},{11,10,8},
                   {9,8,10},{5,4,18},{5,4,18}}, 
                  /*7:205MHz: 99*/
                  {{99,99,1},{56,48,2},{44,31,3},{11,10,9},
                   {9,8,11},{5,4,20},{5,4,20}},
                  /*8:120: 20: QT Specific..Won't be exercised actually*/
                  {{20,20,1},{20,20,1},{20,20,1},{6,4,4},
                   {6,4,4},{6,4,4},{6,4,4}}
                  };

tdm_cycles_info_t kt2_current_tdm_cycles_info[KT2_MAX_SPEEDS]={{0}};

typedef struct tdm_port_slots_info_s  {
        int32                        position;
        struct tdm_port_slots_info_s *prev;
        struct tdm_port_slots_info_s *next;
}tdm_port_slots_info_t;

tdm_port_slots_info_t kt2_tdm_port_slots_info[KT2_MAX_LOGICAL_PORTS];

static uint32 *bcm56450_tdm[]={kt2_tdm_56450config_deprecated0,
                            kt2_tdm_56450config_deprecated1,
                            kt2_tdm_56450config_deprecated2,
                            kt2_tdm_56450config_deprecated3,
                            kt2_tdm_56450config_deprecated4,
                            kt2_tdm_56450config_deprecated5,
                            kt2_tdm_56450A_1,kt2_tdm_56450A_2,
                            kt2_tdm_56450A_3,kt2_tdm_56450A_4,
                            kt2_tdm_56450A_5,kt2_tdm_56450A_6,
                            kt2_tdm_56450A_7,kt2_tdm_56450A1_8,
                            kt2_tdm_56450A1_9,kt2_tdm_56450A2_10,
                            kt2_tdm_56450A2_11,kt2_tdm_56450A2_12,
                            kt2_tdm_56450A2_13,kt2_tdm_56450A2_14,
                            kt2_tdm_56450A2_15,kt2_tdm_56450A_16,
                            kt2_tdm_56450A_17,kt2_tdm_56450A1_18,
                            kt2_tdm_56450AA_19,kt2_tdm_56450DA_20};
static uint32 *bcm56450_speed[]={kt2_tdm_56450speed_deprecated0,
                              kt2_tdm_56450speed_deprecated1,
                              kt2_tdm_56450speed_deprecated2,
                              kt2_tdm_56450speed_deprecated3,
                              kt2_tdm_56450speed_deprecated4,
                              kt2_tdm_56450speed_deprecated5,
                              kt2_tdm_56450speed_1,kt2_tdm_56450speed_2,
                              kt2_tdm_56450speed_3,kt2_tdm_56450speed_4,
                              kt2_tdm_56450speed_5,kt2_tdm_56450speed_6,
                              kt2_tdm_56450speed_7,kt2_tdm_56450speed_8,
                              kt2_tdm_56450speed_9,kt2_tdm_56450speed_10,
                              kt2_tdm_56450speed_11,kt2_tdm_56450speed_12,
                              kt2_tdm_56450speed_13,kt2_tdm_56450speed_14,
                              kt2_tdm_56450speed_15,kt2_tdm_56450speed_16,
                              kt2_tdm_56450speed_17,kt2_tdm_56450speed_18,
                              kt2_tdm_56450speed_19,kt2_tdm_56450speed_20};
bcm56450_tdm_info_t bcm56450_tdm_info[]={
                              {185,90,18,5}, {185,90,18,5}, {185,90,18,5}, /* Deprecated TDM's */
                              {185,90,18,5}, {185,90,18,5}, {185,90,18,5},

                              {205,88,8,11}, {205,88,8,11}, {205,88,8,11}, /* TDM_A */
			      {205,88,8,11}, {205,88,8,11}, {205,88,8,11},
                              {205,88,8,11}, 

                              {205,108,12,9}, {205,108,12,9}, /* TDM_A1 */
                              {205,99,11,9}, {205,99,11,9}, {205,99,11,9}, /* TDM_A2 */
                              {205,99,11,9}, {205,99,11,9}, {205,99,11,9},

                              {205,88,8,11}, /* Special Case for QT testing */
                              {205,88,8,11},{205,108,12,9},
                              {205,88,8,11},{120,20,4,5} }; 

#ifdef UNDER_TESTING
char tdm_config_string[80]={0};
/* 4xGE(1..4) 2xF-XAUI(5,9) 2xF-HG[13](25,26) 2x21G[G.INT](27,28) */
uint32 kt2_tdm_56450_test_speed0[40]= {
       1000 ,1000 ,1000 ,1000,     /* 1 - 4 */
       10000,0000 ,0000 ,0000,     /* 5 - 8 */
       10000,0000 ,0000 ,0000,     /* 9 - 12 */
       0000 ,0000 ,0000 ,0000,     /* 13 - 16 */
       0000 ,0000 ,0000 ,0000,     /* 17 - 20 */
       0000 ,0000 ,0000 ,0000,     /* 21 - 24 */
       13000,13000,21000,21000,    /* 25 - 28 */
       0,    0,    0,    0,        /* 29 - 32 */
       0,    0,    0,    0,        /* 33 - 36 */
       0,    0,    0,    0};       /* 37 - 40 */

/* 24xGE(1..24) 2xF-HG[13](25,26) 2x21G[G.INT](27,28) */
uint32 kt2_tdm_56450_test_speed1[40]= {
       1000 ,1000 ,1000 ,1000,     /* 1 - 4 */
       1000 ,1000 ,1000 ,1000,     /* 5 - 8 */
       1000 ,1000 ,1000 ,1000,     /* 9 - 12 */
       1000 ,1000 ,1000 ,1000,     /* 13 - 16 */
       1000 ,1000 ,1000 ,1000,     /* 17 - 20 */
       1000 ,1000 ,1000 ,1000,     /* 21 - 24 */
       13000,13000,21000,21000,    /* 25 - 28 */
       0,    0,    0,    0,        /* 29 - 32 */
       0,    0,    0,    0,        /* 33 - 36 */
       0,    0,    0,    0};       /* 37 - 40 */

/* 16x 2.5G(1..8 25,26,35..40) + 2x HG21(27..28)  */
uint32 kt2_tdm_56450_test_speed2[40]= {
       2500,2500,2500 ,2500 ,     /* 1 - 4 */
       2500,2500,2500 ,2500 ,     /* 5 - 8 */
       0000,0000,0000 ,0000 ,     /* 9 - 12 */
       0000,0000,0000 ,0000 ,     /* 13 - 16 */
       0000,0000,0000 ,0000 ,     /* 17 - 20 */
       0000,0000,0000 ,0000 ,     /* 21 - 24 */
       2500,2500,21000,21000,     /* 25 - 28 */
       0000,0000,0000 ,0000 ,     /* 29 - 32 */
       0000,0000,2500 ,2500 ,     /* 33 - 36 */
       2500,2500,2500 ,2500 };    /* 37 - 40 */

/* 24x GE(1..24) 8*2.5GE (25..26,35..40) + 2x HG21(27..28)  */
uint32 kt2_tdm_56450_test_speed3[40]= {
       1000,1000,1000 ,1000 ,     /* 1 - 4   */
       1000,1000,1000 ,1000 ,     /* 5 - 8   */
       1000,1000,1000 ,1000 ,     /* 9 - 12  */
       1000,1000,1000 ,1000 ,     /* 13 - 16 */
       1000,1000,1000 ,1000 ,     /* 17 - 20 */
       1000,1000,1000 ,1000 ,     /* 21 - 24 */
       2500,2500,21000,21000,     /* 25 - 28 */
       0000,0000,0000 ,0000 ,     /* 29 - 32 */
       0000,0000,2500 ,2500 ,     /* 33 - 36 */
       2500,2500,2500 ,2500 };    /* 37 - 40 */

/* 24x GE(1..24) 16*2.5GE (25..40) */
uint32 kt2_tdm_56450_test_speed4[40]= {
       1000,1000,1000,1000,     /* 1 - 4   */
       1000,1000,1000,1000,     /* 5 - 8   */
       1000,1000,1000,1000,     /* 9 - 12  */
       1000,1000,1000,1000,     /* 13 - 16 */
       1000,1000,1000,1000,     /* 17 - 20 */
       1000,1000,1000,1000,     /* 21 - 24 */
       2500,2500,2500,2500,     /* 25 - 28 */
       2500,2500,2500,2500,     /* 29 - 32 */
       2500,2500,2500,2500,     /* 33 - 36 */
       2500,2500,2500,2500};    /* 37 - 40 */

/* 12x GE(1..4,5..8,17..20) 12*2.5GE (5..8,13..16,21..24),4*10G(25..28) */
uint32 kt2_tdm_56450_test_speed5[40]= {
       1000,1000,1000,1000,     /* 1 - 4   */
       2500,2500,2500,2500,     /* 5 - 8   */
       1000,1000,1000,1000,     /* 9 - 12  */
       2500,2500,2500,2500,     /* 13 - 16 */
       1000,1000,1000,1000,     /* 17 - 20 */
       2500,2500,2500,2500,     /* 21 - 24 */
       10000,10000,10000,10000,     /* 25 - 28 */
       0000,0000,0000,0000,     /* 29 - 32 */
       0000,0000,0000,0000,     /* 33 - 36 */
       0000,0000,0000,0000};    /* 37 - 40 */

/* 8*10G(1,5,9,13,17,21,25,26) */
uint32 kt2_tdm_56450_test_speed6[40]= {
       10000,0000,0000,0000,     /* 1 - 4   */
       10000,0000,0000,0000,     /* 5 - 8   */
       10000,0000,0000,0000,     /* 9 - 12  */
       10000,0000,0000,0000,     /* 13 - 16 */
       10000,0000,0000,0000,     /* 17 - 20 */
       10000,0000,0000,0000,     /* 21 - 24 */
       10000,10000,0000,0000,     /* 25 - 28 */
       0000,0000,0000,0000,     /* 29 - 32 */
       0000,0000,0000,0000,     /* 33 - 36 */
       0000,0000,0000,0000};    /* 37 - 40 */

/* 24* 2G(1..24) 4*10G(25..28) */
uint32 kt2_tdm_56450_test_speed7[40]= {
       2000,2000,2000,2000,     /* 1 - 4   */
       2000,2000,2000,2000,     /* 5 - 8   */
       2000,2000,2000,2000,     /* 9 - 12  */
       2000,2000,2000,2000,     /* 13 - 16 */
       2000,2000,2000,2000,     /* 17 - 20 */
       2000,2000,2000,2000,     /* 21 - 24 */
       10000,10000,10000,10000,     /* 25 - 28 */
       0000,0000,0000,0000,     /* 29 - 32 */
       0000,0000,0000,0000,     /* 33 - 36 */
       0000,0000,0000,0000};    /* 37 - 40 */

/* 24*GE(1..24) 4*13G(25..28) */
uint32 kt2_tdm_56450_test_speed8[40]= {
       1000,1000,1000,1000,     /* 1 - 4   */
       1000,1000,1000,1000,     /* 5 - 8   */
       1000,1000,1000,1000,     /* 9 - 12  */
       1000,1000,1000,1000,     /* 13 - 16  */
       1000,1000,1000,1000,     /* 17 - 20  */
       1000,1000,1000,1000,     /* 21 - 24  */
       13000,13000,13000,13000,     /* 25- 28 */
       00,00,00,00,     /* 29 - 32  */
       00,00,00,00,     /* 33 - 36  */
       00,00,00,00};     /* 37 - 40  */

/* 18*GE(1..18) 6*2.5G(19..24) 4*13G(25..28) */
uint32 kt2_tdm_56450_test_speed9[40]= {
       1000,1000,1000,1000,     /* 1 - 4   */
       1000,1000,1000,1000,     /* 5 - 8   */
       1000,1000,1000,1000,     /* 9 - 12  */
       1000,1000,1000,1000,     /* 13 - 16  */
       1000,1000,2500,2500,     /* 17 - 20  */
       2500,2500,2500,2500,     /* 21 - 24  */
       13000,13000,13000,13000,     /* 25- 28 */
       00,00,00,00,     /* 29 - 32  */
       00,00,00,00,     /* 33 - 36  */
       00,00,00,00};     /* 37 - 40  */


/* 4xGE + 2x F-XAUI + 2x F-HG[13] + 2x F-HG[21] */
uint32 kt2_tdm_56450_test_speed10[40]= {
       1000 ,1000 ,1000 ,1000,     /* 1 - 4   */
       10000,0000 ,0000 ,0000,     /* 5 - 8   */
       10000,0000 ,0000 ,0000,     /* 9 - 12  */
       0000 ,0000 ,0000 ,0000,     /* 13 - 16  */
       0000 ,0000 ,0000 ,0000,     /* 17 - 20  */
       0000 ,0000 ,0000 ,0000,     /* 21 - 24  */
       13000,13000,21000,21000,     /* 25- 28 */
       00,00,00,00,     /* 29 - 32  */
       00,00,00,00,     /* 33 - 36  */
       00,00,00,00};     /* 37 - 40  */

/* 8xGE + 16x 2.5GE + 8x2.5G-HGL + 8x2.5G[G.INT] */
uint32 kt2_tdm_56450_test_speed11[40]= {
       1000 ,1000 ,2500 ,2500,     /* 1 - 4   */
       1000 ,1000 ,2500 ,2500,     /* 5 - 8   */
       1000 ,1000 ,2500 ,2500,     /* 9 - 12  */
       1000 ,1000 ,2500 ,2500,     /* 13 - 16  */
       2500 ,2500 ,2500 ,2500,     /* 17 - 20  */
       2500 ,2500 ,2500 ,2500,     /* 21 - 24  */
       2500 ,2500 ,2500 ,2500,     /* 25- 28 */
       2500 ,2500 ,2500 ,2500,     /* 29 - 32  */
       2500 ,2500 ,2500 ,2500,     /* 33 - 36  */
       2500 ,2500 ,2500 ,2500};     /* 37 - 40  */

/* 8xGE + 8x XFI / 8x F-XAUI / 4x F-XAUI + 4x XFI */
uint32 kt2_tdm_56450_test_speed12[40]= {
       1000 ,1000 ,1000 ,1000,     /* 1 - 4   */
       1000 ,1000 ,1000 ,1000,     /* 5 - 8   */
       10000 ,0000 ,0000 ,0000,     /* 9 - 12  */
       10000 ,0000 ,0000 ,0000,     /* 13- 16  */
       10000 ,0000 ,0000 ,0000,     /* 17 - 20  */
       10000 ,0000 ,0000 ,0000,     /* 21 - 24  */
       10000 ,10000 ,10000 ,10000,     /* 25 - 28 */
       0000 ,0000 ,0000 ,0000,     /* 29 - 32 */
       0000 ,0000 ,0000 ,0000,     /* 33 - 36  */
       0000 ,0000 ,0000 ,0000};     /* 37 - 40  */

/* 2*13G + 2*21G + 24*G */
uint32 kt2_tdm_56450_test_speed13[40]= {
       1000 ,1000 ,1000 ,1000,     /* 1 - 4   */
       1000 ,1000 ,1000 ,1000,     /* 5 - 8   */
       1000 ,1000 ,1000 ,0000,     /* 9 - 12  */
       1000 ,1000 ,1000 ,0000,     /* 13- 16  */
       1000 ,1000 ,1000 ,0000,     /* 17 - 20  */
       1000 ,1000 ,1000 ,0000,     /* 21 - 24  */
       13000 ,13000 ,21000 ,21000,     /* 25 - 28 */
       0000 ,0000 ,0000 ,0000,     /* 29 - 32 */
       0000 ,0000 ,0000 ,0000,     /* 33 - 36  */
       0000 ,0000 ,0000 ,0000};     /* 37 - 40  */

/* 24x GE + 4x HG[13]  */
uint32 kt2_tdm_56450_test_speed14[40]= {
       1000 ,1000 ,1000 ,1000,     /* 1 - 4   */
       1000 ,1000 ,1000 ,1000,     /* 5 - 8   */
       1000 ,1000 ,1000 ,1000,     /* 9 - 12  */
       1000 ,1000 ,1000 ,1000,     /* 13- 16  */
       1000 ,1000 ,1000 ,1000,     /* 17 - 20  */
       1000 ,1000 ,1000 ,1000,     /* 21 - 24  */
       13000 ,13000 ,13000 ,13000,     /* 25 - 28 */
       0000 ,0000 ,0000 ,0000,     /* 29 - 32 */
       0000 ,0000 ,0000 ,0000,     /* 33 - 36  */
       0000 ,0000 ,0000 ,0000};     /* 37 - 40  */

/* 24x GE + 4x XAUI / 16x 2.5G */
uint32 kt2_tdm_56450_test_speed15[40]= {
       1000 ,1000 ,1000 ,1000,     /* 1 - 4   */
       1000 ,1000 ,1000 ,1000,     /* 5 - 8   */
       1000 ,1000 ,1000 ,1000,     /* 9 - 12  */
       1000 ,1000 ,1000 ,1000,     /* 13- 16  */
       1000 ,1000 ,1000 ,1000,     /* 17 - 20  */
       1000 ,1000 ,1000 ,1000,     /* 21 - 24  */
       10000 ,10000 ,10000 ,10000,     /* 25 - 28 */
       0000 ,0000 ,0000 ,0000,     /* 29 - 32 */
       0000 ,0000 ,0000 ,0000,     /* 33 - 36  */
       0000 ,0000 ,0000 ,0000};     /* 37 - 40  */

/* 24x GE + 16x 2.5G */
uint32 kt2_tdm_56450_test_speed16[40]= {
       1000 ,1000 ,1000 ,1000,     /* 1 - 4   */
       1000 ,1000 ,1000 ,1000,     /* 5 - 8   */
       1000 ,1000 ,1000 ,1000,     /* 9 - 12  */
       1000 ,1000 ,1000 ,1000,     /* 13- 16  */
       1000 ,1000 ,1000 ,1000,     /* 17 - 20  */
       1000 ,1000 ,1000 ,1000,     /* 21 - 24  */
       2500 ,2500 ,2500 ,2500,     /* 25 - 28 */
       2500 ,2500 ,2500 ,2500,     /* 29 - 32 */
       2500 ,2500 ,2500 ,2500,     /* 33 - 36  */
       2500 ,2500 ,2500 ,2500};     /* 37 - 40  */

/* 8x GE + 2  * 10G */
uint32 kt2_tdm_56450_test_speed17[40]= {
       1000 ,1000 ,1000 ,1000,     /* 1 - 4   */
       1000 ,1000 ,1000 ,1000,     /* 5 - 8   */
       0000 ,0000 ,0000 ,0000,     /* 9 - 12  */
       0000 ,0000 ,0000 ,0000,     /* 13- 16  */
       0000 ,0000 ,0000 ,0000,     /* 17 - 20  */
       0000 ,0000 ,0000 ,0000,     /* 21 - 24  */
       10000 ,10000 ,0000 ,0000,     /* 25 - 28 */
       0000 ,0000 ,0000 ,0000,     /* 29 - 32 */
       0000 ,0000 ,0000 ,0000,     /* 33 - 36  */
       0000 ,0000 ,0000 ,0000};     /* 37 - 40  */

/* 4x 2.5GE + 1  * 13G 1 * 10G */
uint32 kt2_tdm_56450_test_speed18[40]= {
       2500 ,2500 ,2500 ,0000,     /* 1 - 4   */
       0000 ,0000 ,0000 ,0000,     /* 5 - 8   */
       0000 ,0000 ,0000 ,0000,     /* 9 - 12  */
       0000 ,0000 ,0000 ,0000,     /* 13- 16  */
       0000 ,0000 ,0000 ,0000,     /* 17 - 20  */
       0000 ,0000 ,0000 ,0000,     /* 21 - 24  */   
       13000 ,10000 ,0000 ,0000,     /* 25 - 28 */
       0000 ,0000 ,0000 ,0000,     /* 29 - 32 */
       0000 ,0000 ,0000 ,0000,     /* 33 - 36  */
       0000 ,0000 ,0000 ,0000};     /* 37 - 40  */

/* 4x 2.5GE + 4  * 1g */
uint32 kt2_tdm_56450_test_speed19[40]= {
       2500 ,0000 ,0000 ,0000,     /* 1 - 4   */
       2500 ,0000 ,0000 ,0000,     /* 5 - 8   */
       2500 ,0000 ,0000 ,0000,     /* 9 - 12  */
       2500 ,0000 ,0000 ,0000,     /* 13- 16  */
       1000 ,0000 ,0000 ,0000,     /* 17 - 20  */
       1000 ,0000 ,0000 ,0000,     /* 21 - 24  */
       1000 ,0000 ,0000 ,0000,     /* 25 - 28 */
       1000 ,0000 ,0000 ,0000,     /* 29 - 32 */
       0000 ,0000 ,0000 ,0000,     /* 33 - 36  */
       0000 ,0000 ,0000 ,0000};     /* 37 - 40  */










uint32 kt2_tdm_under_testing[90]= {KT2_IDLE};
uint32 kt2_tdm_under_testing_idle[180]= {KT2_IDLE};
#endif
static 
void soc_katana2_save_tdm_pos(int unit, uint8 new_tdm_size,uint32 *new_tdm);
void kt2_tdm_replace(uint32 *tdm, uint32 total_tdm_slots,
                     uint32  src, uint32 dst,uint32 one_time)
{
     uint32 index=0;
     for (index=0; index < total_tdm_slots ; index++ ) {
          if (tdm[index] == src) {
              tdm[index] = dst;
              if (one_time) {
                  break;
              }
          }
     }
}

void kt2_tdm_swap(uint32 *tdm, uint32 total_tdm_slots,
                  uint32  src, uint32 dst)
{
     uint32 index=0;
     uint32 temp_index=0;
     uint32 temp=0;
     for (index=0; index < total_tdm_slots ; index++ ) {
          if (tdm[index] == src) {
              temp_index=index;
          }
          if (tdm[index] == dst) {
              temp = tdm[temp_index];
              tdm[temp_index]=dst;
              tdm[index]=temp;
          }
     }
}




STATIC int
_soc_katana2_parity_enable_all(int unit, int enable)
{
    uint32 rval;

    /* MMU enables */
    SOC_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &rval));
    soc_reg_field_set(unit, MISCCONFIGr, &rval, PARITY_STAT_CLEARf, 1);
    SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, rval));

    soc_reg_field_set(unit, MISCCONFIGr, &rval, PARITY_CHK_ENf, 1);
    soc_reg_field_set(unit, MISCCONFIGr, &rval, PARITY_GEN_ENf, 1);
    soc_reg_field_set(unit, MISCCONFIGr, &rval, PARITY_STAT_CLEARf, 0);
    SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, rval));

    
    return SOC_E_NONE;
}

/* SER processing for TCAMs */
STATIC _soc_ser_parity_info_t _soc_kt2_ser_parity_info[] = {
    { EFP_TCAMm, _SOC_SER_PARITY_MODE_8BITS,
      CMIC_SER1_START_ADDR_0r, CMIC_SER1_END_ADDR_0r,
      CMIC_SER1_MEM_ADDR_0r, CMIC_SER1_PARITY_MODE_SEL_15_0r,
      RANGE_0_PARITY_BITSf, 1, 0, 0},
    { FP_TCAMm, _SOC_SER_PARITY_MODE_8BITS,
      CMIC_SER0_START_ADDR_0r, CMIC_SER0_END_ADDR_0r,
      CMIC_SER0_MEM_ADDR_0r, CMIC_SER0_PARITY_MODE_SEL_15_0r,
      RANGE_1_PARITY_BITSf, 0, 0, 0},
    { VFP_TCAMm, _SOC_SER_PARITY_MODE_8BITS,
      CMIC_SER0_START_ADDR_1r, CMIC_SER0_END_ADDR_1r,
      CMIC_SER0_MEM_ADDR_1r, CMIC_SER0_PARITY_MODE_SEL_15_0r,
      RANGE_2_PARITY_BITSf, 0, 0, 0},
    { FP_UDF_TCAMm, _SOC_SER_PARITY_MODE_4BITS,
      CMIC_SER0_START_ADDR_2r, CMIC_SER0_END_ADDR_2r,
      CMIC_SER0_MEM_ADDR_2r, CMIC_SER0_PARITY_MODE_SEL_15_0r,
      RANGE_3_PARITY_BITSf, 0, 0, 0},
    /* These memories already have a parity bit.
     * L3_TUNNELm,
     * VLAN_SUBNETm,
     * MY_STATION_TCAMm,
     * L2_USER_ENTRYm,
     * ESM_PKT_TYPE_ID,
     */
    { INVALIDm, _SOC_SER_PARITY_MODE_NUM},
};

#define SOC_KT2_SER_MEM_AVAILABLE (4096 * 32)  /* bits */

int
soc_kt2_ser_mem_clear(int unit, soc_mem_t mem)
{
    return soc_cmicm_ser_mem_clear(unit, _soc_kt2_ser_parity_info, mem);
    return 0;
}

void
soc_kt2_ser_fail(int unit)
{
    soc_process_cmicd_ser_parity_error(unit, _soc_kt2_ser_parity_info,
                                       _SOC_PARITY_TYPE_SER);
    return;
}

int
soc_katana2_pipe_mem_clear(int unit)
{
    uint32              rval;
    int                 pipe_init_usec;
    soc_timeout_t       to;

    /*
     * Reset the IPIPE and EPIPE block
     */
    rval = 0;
    SOC_IF_ERROR_RETURN(WRITE_ING_HW_RESET_CONTROL_1r(unit, rval));
    soc_reg_field_set(unit, ING_HW_RESET_CONTROL_2r, &rval, RESET_ALLf, 1);
    soc_reg_field_set(unit, ING_HW_RESET_CONTROL_2r, &rval, VALIDf, 1);
    /* Set count to # entries in largest IPIPE table (L2X) */
    soc_reg_field_set(unit, ING_HW_RESET_CONTROL_2r, &rval, COUNTf, 32768);
    SOC_IF_ERROR_RETURN(WRITE_ING_HW_RESET_CONTROL_2r(unit, rval));

    rval = 0;
    SOC_IF_ERROR_RETURN(WRITE_EGR_HW_RESET_CONTROL_0r(unit, rval));
    soc_reg_field_set(unit, EGR_HW_RESET_CONTROL_1r, &rval, RESET_ALLf, 1);
    soc_reg_field_set(unit, EGR_HW_RESET_CONTROL_1r, &rval, VALIDf, 1);
    /* Set count to # entries in largest EPIPE table (EGR_VLAN_XLATE) */
    soc_reg_field_set(unit, EGR_HW_RESET_CONTROL_1r, &rval, COUNTf, 16384);
    SOC_IF_ERROR_RETURN(WRITE_EGR_HW_RESET_CONTROL_1r(unit, rval));

    /* For simulation, set timeout to 10 sec.  Otherwise, timeout = 50 ms */
    if (SAL_BOOT_SIMULATION) {
        pipe_init_usec = 10000000;
    } else {
        pipe_init_usec = 50000;
    }
    soc_timeout_init(&to, pipe_init_usec, 0);

    /* Wait for IPIPE memory initialization done. */
    do {
        SOC_IF_ERROR_RETURN(READ_ING_HW_RESET_CONTROL_2r(unit, &rval));
        if (soc_reg_field_get(unit, ING_HW_RESET_CONTROL_2r, rval, DONEf)) {
            break;
        }
        if (soc_timeout_check(&to)) {
            soc_cm_debug(DK_WARN, "unit %d : ING_HW_RESET timeout\n", unit);
            break;
        }
    } while (TRUE);

    /* Wait for EPIPE memory initialization done. */
    do {
        SOC_IF_ERROR_RETURN(READ_EGR_HW_RESET_CONTROL_1r(unit, &rval));
        if (soc_reg_field_get(unit, EGR_HW_RESET_CONTROL_1r, rval, DONEf)) {
            break;
        }
        if (soc_timeout_check(&to)) {
            soc_cm_debug(DK_WARN, "unit %d : EGR_HW_RESET timeout\n", unit);
            break;
        }
    } while (TRUE);

    SOC_IF_ERROR_RETURN(READ_ING_HW_RESET_CONTROL_2r(unit, &rval));
    soc_reg_field_set(unit, ING_HW_RESET_CONTROL_2r, &rval, RESET_ALLf, 0);
    soc_reg_field_set(unit, ING_HW_RESET_CONTROL_2r, &rval, CMIC_REQ_ENABLEf, 1);
    SOC_IF_ERROR_RETURN(WRITE_ING_HW_RESET_CONTROL_2r(unit, rval));

    SOC_IF_ERROR_RETURN(READ_EGR_HW_RESET_CONTROL_1r(unit, &rval));
    soc_reg_field_set(unit, EGR_HW_RESET_CONTROL_1r, &rval, RESET_ALLf, 0);
    SOC_IF_ERROR_RETURN(WRITE_EGR_HW_RESET_CONTROL_1r(unit, rval));

    return SOC_E_NONE;
}

int soc_katana2_linkphy_mem_clear(unit)
{
    int index, max_index;
    soc_mem_t mem;

    mem = RXLP_INTERNAL_STREAM_MAP_PORT_0m;
    max_index = soc_mem_index_max(unit,mem);
    for (index = 0; index <= max_index; index++) {
        soc_mem_write(unit, mem, MEM_BLOCK_ALL, index,
                             soc_mem_entry_null(unit, mem));
    }
    mem = RXLP_INTERNAL_STREAM_MAP_PORT_1m;
    max_index = soc_mem_index_max(unit,mem);
    for (index = 0; index <= max_index; index++) {
        soc_mem_write(unit, mem, MEM_BLOCK_ALL, index,
                             soc_mem_entry_null(unit, mem));
    }
    mem = RXLP_INTERNAL_STREAM_MAP_PORT_2m;
    max_index = soc_mem_index_max(unit,mem);
    for (index = 0; index <= max_index; index++) {
        soc_mem_write(unit, mem, MEM_BLOCK_ALL, index,
                             soc_mem_entry_null(unit, mem));
    }
    mem = RXLP_INTERNAL_STREAM_MAP_PORT_3m;
    max_index = soc_mem_index_max(unit,mem);
    for (index = 0; index <= max_index; index++) {
        soc_mem_write(unit, mem, MEM_BLOCK_ALL, index,
                             soc_mem_entry_null(unit, mem));
    }

    mem = TXLP_PORT_STREAM_BITMAP_TABLEm;
    max_index = soc_mem_index_max(unit,mem);
    for (index = 0; index <= max_index; index++) {
        soc_mem_write(unit, mem, MEM_BLOCK_ALL, index,
            soc_mem_entry_null(unit, mem));
    }

    mem = TXLP_INT2EXT_STREAM_MAP_TABLEm;
    max_index = soc_mem_index_max(unit,mem);
    for (index = 0; index <= max_index; index++) {
        soc_mem_write(unit, mem, MEM_BLOCK_ALL, index,
                             soc_mem_entry_null(unit, mem));
    }

    mem = DEVICE_STREAM_ID_TO_PP_PORT_MAPm;
    max_index = soc_mem_index_max(unit,mem);
    for (index = 0; index <= max_index; index++) {
        soc_mem_write(unit, mem, MEM_BLOCK_ALL, index,
                             soc_mem_entry_null(unit, mem));
    }
    mem = PP_PORT_TO_PHYSICAL_PORT_MAPm;
    max_index = soc_mem_index_max(unit,mem);
    for (index = 0; index <= max_index; index++) {
        soc_mem_write(unit, mem, MEM_BLOCK_ALL, index,
                             soc_mem_entry_null(unit, mem));
    }

    return SOC_E_NONE;
}
soc_error_t kt2_tdm_verify(uint32 *tdm,
                           uint32 total_tdm_slots,
                           uint32 tdm_freq,    /* In MHHz */
                           uint32 *port_speed,
                           uint32 olp_port_flag,  
                           uint32 *offender_port)
{
    /* Following rules need to be met */
    /* Rule1 : Each Loopback port need 2.5G bandwidth */
    /* Rule2 : Each CMIC port need 2G bandwidth       */
    /* Rule3 : Loopback (LPBK) ports require min 3 cycle spacing among LPBK or 
               CMIC ports */ 
    /* Rule4 : Each subport in MXQPORT operates with 4 cycle TDM */
    /* Rule5 : Each 10G-XAUI ports should be able to operate in 4x2.5G with 4 
               slots allocated to each subport from the same MXQPORT */
    soc_error_t           rv=SOC_E_NONE;
    uint32                index=0;
    uint32                expected_tdm_cycles_min=0;
    uint32                expected_tdm_cycles_max=0;
    uint32                tdm_freq_index=0;
    uint32                speed_index=0;
    uint32                port=0;
    uint32                speed=0;
    uint32                worse_tdm_slot_spacing=0;
    uint32                prev_tdm_slot_spacing=0;
    uint32                next_tdm_slot_spacing=0;
    uint32                tdm_slot_spacing=0;
    uint32                min_tdm_cycles=0; /* To Meet BW */
    uint32                count=0; 
    uint32                skip_count=0; 
    uint32                mxqblock=0;
    uint32                outer_port=0;
    uint32                inner_port=0;
    uint32                spacing=0;
    uint32                loop=0;
    tdm_port_slots_info_t *head=NULL;
    tdm_port_slots_info_t *head1=NULL;
    tdm_port_slots_info_t *current=NULL;
    tdm_port_slots_info_t *current1=NULL;
    tdm_port_slots_info_t *prev = NULL;
    tdm_port_slots_info_t *temp=NULL;
    uint32                idle_flag=0;
    uint32                idle_rule=0;


   /* Possible TDM frequencies 
      80MHz(16Cycles) ,110MHz(20Cycles),155MHz(84Cycles),
      185MHz(80Cycles),185MHz(90Cycles) 
    */


   switch(tdm_freq) {
   case 80: tdm_freq_index=0;
            expected_tdm_cycles_min=16;
            expected_tdm_cycles_max=32;
            break;
   case 110: tdm_freq_index=1;
             expected_tdm_cycles_min=20;
             expected_tdm_cycles_max=40;
             break;
   case 155: tdm_freq_index=2;
             expected_tdm_cycles_min=84;
             expected_tdm_cycles_max=168;
             break;
   case 185: 
             expected_tdm_cycles_min=80;
             expected_tdm_cycles_max=180;
             if ((total_tdm_slots >= 80 ) && (total_tdm_slots <= 90)) {
                 tdm_freq_index=3; 
             } else {
                 tdm_freq_index=4; 
             }
             break;
   case 205:
             expected_tdm_cycles_min=88;
             expected_tdm_cycles_max=180;
             if (total_tdm_slots == 108 ) {
                 tdm_freq_index=5;
             }
             if (total_tdm_slots == 88 ) {
                 tdm_freq_index=6;
             }
             if (total_tdm_slots == 99 ) {
                 tdm_freq_index=7;
             }
             break;
   case 120: tdm_freq_index=8;
             expected_tdm_cycles_min=20;
             expected_tdm_cycles_max=20;
             break;
   default:  soc_cm_debug(DK_ERR,"Unsupported tdm frequency:%d\n",tdm_freq);
             return SOC_E_FAIL;
   }
   if (!((total_tdm_slots >= expected_tdm_cycles_min) && 
         (total_tdm_slots <= expected_tdm_cycles_max))) {
          soc_cm_debug(DK_ERR,"Unsupported tdm cycles:%d (min:%d max:%d\n",
                  total_tdm_slots,
                  expected_tdm_cycles_min,expected_tdm_cycles_max);
          return SOC_E_FAIL; 
   }

    idle_flag=0;
    idle_rule = 0;
    /* Initialize list */
    for (port=0;port < KT2_MAX_LOGICAL_PORTS ; port++) {
         kt2_tdm_port_slots_info[port].prev=&kt2_tdm_port_slots_info[port];
         kt2_tdm_port_slots_info[port].next=&kt2_tdm_port_slots_info[port];
         kt2_tdm_port_slots_info[port].position = -1;
    }
    for (index=0; index < total_tdm_slots; index++) {
         if ((tdm[index] == KT2_IDLE1) ||
             (tdm[index] == KT2_IDLE)) {
             if (idle_flag == 1) {
                 idle_rule = 1;
             }
             idle_flag=1;
             skip_count++;
             continue;
         }
         idle_flag=0;
         port = tdm[index];
         if (kt2_tdm_port_slots_info[port].position == -1) {
             /* First Entry */
             kt2_tdm_port_slots_info[port].position = index;
             skip_count++;
             continue;
         }
         head = current = &kt2_tdm_port_slots_info[port];
         while(current->next !=head) {
               current = current->next;
         } 
         current->next=sal_alloc(sizeof(tdm_port_slots_info_t),"TDM Pointer");
         count++;
         soc_cm_debug(DK_VERBOSE,"Count=%d SkipCount:%d  Port:%d  Addr:%p\n",count,skip_count,port,(void *)current->next);

         current->next->position=index;
         current->next->prev=current;
         current->next->next=head;
         head->prev=current->next;
   }
   if (idle_rule == 0) {
       /* Special case */
       /* soc_cm_debug(DK_VERBOSE,"Idle slot on edges \n"); */
       if (!(((tdm[0] == KT2_IDLE1) ||
             (tdm[0] == KT2_IDLE)) &&
            ((tdm[total_tdm_slots-1] == KT2_IDLE1) ||
             (tdm[total_tdm_slots-1] == KT2_IDLE)))) {
           soc_cm_debug(DK_VERBOSE,"Two consecutive idle not found \n");
           *offender_port = KT2_IDLE;
           return SOC_E_CONFIG;
       }
   }
   soc_cm_debug(DK_VERBOSE,"Final Count=%d SkipCount:%d\n",count,skip_count);
   soc_cm_debug(DK_VERBOSE,"Verifying rule  \n");
   soc_cm_debug(DK_VERBOSE,
                "Rule3: Loopback (LPBK) ports require min 3 cycle spacing "
                "among LPBK or CMIC ports \n");
   head = current = &kt2_tdm_port_slots_info[KT2_LPBK_PORT];
   do {
      prev = current->prev;
      if ( current->position != prev->position ) {
           prev_tdm_slot_spacing = 
                          (current->position - prev->position) > 0 ?
                          (current->position - prev->position)-1:
                          (total_tdm_slots - prev->position + 
                           current->position) - 1;
           next_tdm_slot_spacing = 
                          (current->next->position - current->position) >0 ?
                          (current->next->position - current->position)-1:
                          (total_tdm_slots - current->position +
                           current->next->position) - 1;
      if (prev_tdm_slot_spacing <= 3) {
          soc_cm_debug(DK_ERR,
                       "Loopback Ports need min 3 spacing but(prev)found"
                       "(current:%d , prev:%d)=%d\n",
                       current->position,prev->position,prev_tdm_slot_spacing);
          rv = SOC_E_FAIL;
          continue;
      } 
      if (next_tdm_slot_spacing <= 3) {
          soc_cm_debug(DK_ERR,
                       "Loopback Ports need min 3 spacing but(next)found"
                       "(current:%d , prev:%d)=%d\n",
                        current->position,prev->position,next_tdm_slot_spacing);
          rv = SOC_E_CONFIG;
          *offender_port = KT2_LPBK_PORT;
          continue;
      } 
      } else {
          prev_tdm_slot_spacing = next_tdm_slot_spacing = 0;
      }
      head1 = current1 = &kt2_tdm_port_slots_info[KT2_CMIC_PORT];
      do {
         tdm_slot_spacing = (current->position - current1->position) > 0 ?
                          (current->position - current1->position)-1:
                          (total_tdm_slots - current1->position + 
                           current->position) - 1  ;
         if (tdm_slot_spacing <= 3) {
             soc_cm_debug(DK_ERR,
                          "Loopback and CMIC ports need min 3 spacing but "
                          "found:%u" "(Loopack current:%d , CMIC Current:%d)\n",
                          tdm_slot_spacing,
                          current->position,current1->position);
             rv = SOC_E_CONFIG;
             *offender_port=KT2_LPBK_PORT;
             continue;
         }
         current1 = current1->next;
      }while ((current1 !=head1) && (rv == SOC_E_NONE));
      current = current->next;
   } while ((current !=head) && (rv == SOC_E_NONE));
   soc_cm_debug(DK_VERBOSE,"Verifying rules \n");
   soc_cm_debug(DK_VERBOSE,"Rule1 : Each Loopback port need 2.5G bandwidth \n");
   soc_cm_debug(DK_VERBOSE,"Rule2 : Each CMIC port need 2G bandwidth       \n");
   for (index=0;index < KT2_MAX_LOGICAL_PORTS && rv == SOC_E_NONE ;index++ ) {
        port = index ;
        if (kt2_tdm_port_slots_info[port].position == -1) {
            continue;
        }
        soc_cm_debug(DK_VERBOSE," %d ==>\n\n",port);
        switch(port) {
        case KT2_CMIC_PORT: soc_cm_debug(DK_VERBOSE,"CMIM \n");
                            speed =2000; 
                            break;
        case KT2_LPBK_PORT: soc_cm_debug(DK_VERBOSE,"LOOPBAKC \n");
                            speed =2500; 
                            break;
        default:            soc_cm_debug(DK_VERBOSE," Port:%d Speed %d ==>\n\n",
                                   port,port_speed[port-1]);
                            speed = port_speed[port-1];
                            break;
        }
        switch(speed) {
        case 1000 : speed_index=0;break;/* 1G   */ 
        case 2000 : speed_index=1;break;/* 2G   */ 
        case 2500 : speed_index=2;break;/* 2.5G */ 
        case 10000: speed_index=3;break;/* 10G  */ 
        case 13000: speed_index=4;break;/* 13G  */ 
        case 20000: speed_index=5;break;/* 20G  */ 
        case 21000: speed_index=6;break;/* 21G  */ 
        default   : soc_cm_debug(DK_ERR,"Unsupported speed:%d \n",speed);
                    return SOC_E_FAIL;
        }
        worse_tdm_slot_spacing=kt2_tdm_cycles_info[tdm_freq_index][speed_index].
                               worse_tdm_slot_spacing;
        min_tdm_cycles=kt2_tdm_cycles_info[tdm_freq_index][speed_index].
                       min_tdm_cycles;

        count=0;
        head = current = &kt2_tdm_port_slots_info[port];
        do {
           prev = current->prev;
           if (current->position != prev->position ) {
               prev_tdm_slot_spacing = 
                          (current->position - prev->position)> 0 ?
                          (current->position - prev->position)-1:
                          (total_tdm_slots - prev->position + 
                           current->position) - 1  ;
               next_tdm_slot_spacing = 
                          (current->next->position - current->position) > 0 ?
                          (current->next->position - current->position)-1:
                          (total_tdm_slots - current->position + 
                           current->next->position) - 1;
           } else {
               prev_tdm_slot_spacing = next_tdm_slot_spacing = 0;
           }
           soc_cm_debug(DK_VERBOSE,"Pos:%d Spacing (Prev %d - %d =  )%d"
                        "(Next %d - %d =  )%d\n", current->position,
                        current->position , prev->position ,
                        prev_tdm_slot_spacing, current->next->position , 
                        current->position , next_tdm_slot_spacing);
           if ((prev_tdm_slot_spacing > worse_tdm_slot_spacing) ||
               (next_tdm_slot_spacing > worse_tdm_slot_spacing)) {
                soc_cm_debug(DK_ERR,"WorseSpacing:%d NOK !!!! port:%d\n",
                       worse_tdm_slot_spacing,port);
                rv = SOC_E_CONFIG;
                *offender_port=port;
                continue;
           }
           count++;
           current = current->next;
        } while ((current !=head) && (rv == SOC_E_NONE)) ;
        soc_cm_debug(DK_VERBOSE,"TDM Cycles: %d \n",count);
        if (count < min_tdm_cycles) {
            soc_cm_debug(DK_ERR,"min_tdm_cycles:%dNOK  !!!! port:%d\n",
                         min_tdm_cycles,port);
            rv =  SOC_E_CONFIG;
            *offender_port=port;
            continue;
        }
        soc_cm_debug(DK_VERBOSE,"n== %d <==\n\n",port);
   }

   soc_cm_debug(DK_VERBOSE,"Verifying rule  \n");
   soc_cm_debug(DK_VERBOSE,
                "Rule4 : Each subport in MXQPORT operates with 4 cycle TDM \n");
   soc_cm_debug(DK_VERBOSE,"Verifying MXQports spacing concern\n");
   for(mxqblock=0;mxqblock<KT2_MAX_MXQBLOCKS && rv == SOC_E_NONE ;mxqblock++) {
       for(index=0;
           index<KT2_MAX_MXQPORTS_PER_BLOCK && rv == SOC_E_NONE;
           index++) {
           outer_port = kt2_mxqblock_ports[mxqblock][index];
           /* Ignore MXQSpacing issue for OLP Port */
           if ((outer_port == KT2_OLP_PORT) && (olp_port_flag)) {
                continue;
           }
           if (kt2_tdm_port_slots_info[outer_port].position == -1) {
               continue;
           }
           head = current = &kt2_tdm_port_slots_info[outer_port];
           do {
              for(loop=0;loop<KT2_MAX_MXQPORTS_PER_BLOCK; loop++) {
                  inner_port=kt2_mxqblock_ports[mxqblock][loop];
                  /* Ignore MXQSpacing issue for OLP Port */
                  if ((inner_port == KT2_OLP_PORT) && (olp_port_flag)) {
                       continue;
                  }
                  if ( inner_port == outer_port) {
                       continue;
                  }
                  if (kt2_tdm_port_slots_info[inner_port].position == -1) {
                      continue;
                  }
                  head1 = current1 = &kt2_tdm_port_slots_info[inner_port];
                  do {
                       spacing = (current->position - current1->position) > 0 ?
                                 (current->position - current1->position)-1:
                                 (total_tdm_slots - current1->position +
                                  current->position) - 1;
                       if (spacing < 3) {
                           soc_cm_debug(DK_ERR,
                                        "Port:%d MXQSpacing Issue Expected > 4 "
                                        "but observed %d\n",inner_port,spacing);
                           rv = SOC_E_CONFIG;
                           *offender_port=outer_port;
                           continue;
                       }
                       current1 = current1->next;
                  } while ((current1 !=head1) && (rv == SOC_E_NONE));
              }
              current = current->next;
         } while ((current !=head) && (rv == SOC_E_NONE));
      }
  }

  /* Free allocated resources */
  count=0;
  for (port=0;port < KT2_MAX_LOGICAL_PORTS ; port++) {
       head = current = &kt2_tdm_port_slots_info[port];
       do {
          temp = current->next;
          if (current != head) {
              count++;
              soc_cm_debug(DK_VERBOSE,"Freed count :%d\n",count);
              sal_free(current);
          }          
          current = temp;
       } while (current != head);
       kt2_tdm_port_slots_info[port].prev=NULL;
       kt2_tdm_port_slots_info[port].next=NULL;
       kt2_tdm_port_slots_info[port].position = -1;
  }
  sal_memcpy(&kt2_current_tdm_cycles_info,
             &kt2_tdm_cycles_info[tdm_freq_index][0],
             sizeof(tdm_cycles_info_t)*KT2_MAX_SPEEDS);
  return rv;
}
static soc_error_t 
kt2_tdm_position_mask(uint32 port, 
                      int32  pos, 
                      uint32 total_tdm_slots,
                      uint32 *tdm_position_mask)
{
    uint32 count=0;
    uint32 mxqblock=0;
    int32  pos1=0;
    int32  pos2=0;

    if (!((port >=  1) && (port <= KT2_MAX_PHYSICAL_PORTS))) {
         return SOC_E_PARAM;
    }
    mxqblock=kt2_port_to_mxqblock[port-1];
    for (count=0;count < 4; count++) {
         pos1 = (pos + count+1)%total_tdm_slots;
         tdm_position_mask[pos1] |= (1 << mxqblock);
         pos2 = (int32)(pos - (count+1));
         if (pos2 < 0) {
             pos2 = pos2 + total_tdm_slots;
         }
         tdm_position_mask[pos2] |= (1 << mxqblock);
    }
    return SOC_E_NONE;
}
void kt2_fill_pos(uint32 *tdm, 
                  uint32 total_tdm_slots,
                  int32  pos, 
                  uint32 port,
                  uint32 port_type,
                  uint32 min_tdm_cycles,
                  uint32 worse_tdm_slot_spacing,
                  uint32 optimal_tdm_slot_spacing)
{
    uint32 negative_flag=0;
    uint32 fill_count=0;
    uint32 optimal_pos=pos;
    uint32 no_optimal_flag=0;
    int32 mxqblock=0;
    static uint32 last_port_type;
    static uint32 previous_position[18]={0};

    if ((port != KT2_LPBK_PORT) && (port != KT2_CMIC_PORT)) {
         mxqblock=kt2_port_to_mxqblock[port-1];
    }
    if (pos == -1) {
        no_optimal_flag=1;
        pos = 0;
        if (last_port_type != port_type) { 
            for(fill_count=0;fill_count<18;fill_count++) {
                previous_position[fill_count]=0;
            }
        }
    }
    last_port_type=port_type;

    for(fill_count=0;fill_count<min_tdm_cycles;fill_count++) {
        if (no_optimal_flag == 1) {
            if ( previous_position[fill_count] != 0) {
                 pos = previous_position[fill_count]+1;
            }
        }
        while (1) {
               if (tdm[pos] == KT2_IDLE) {
                   if ((port == KT2_LPBK_PORT) || (port == KT2_CMIC_PORT)) {
                       break;
                   }
                   if ((kt2_tdm_position_mxq_mask[pos] & (1<<mxqblock))==0) {
                        break;
                   }
                   if (kt2_tdm_mxqblock_ports_used[kt2_port_to_mxqblock[port]] 
                       == 1) {
                        break;
                   }
               }
               if (no_optimal_flag == 0) {
                   if ((negative_flag == 0) && (fill_count != 0)) {
                        pos--;
                   } else {
                        pos++;
                        negative_flag=0;
                   }
               } else {
                   pos++;
               }
        }
        if (pos < optimal_pos) {
               negative_flag=1;
        }
        tdm[pos]=port;
        previous_position[fill_count]=pos;
        if (!((port == KT2_LPBK_PORT) || (port == KT2_CMIC_PORT))) {
               kt2_tdm_position_mask(port,
                                     pos,
                                     total_tdm_slots,
                                     kt2_tdm_position_mxq_mask);
        }
        pos = optimal_pos = pos + optimal_tdm_slot_spacing + 1;
        if (pos >= total_tdm_slots) {
            pos = total_tdm_slots -1;
            negative_flag = 0; /* forced too */
        }
    }
    if (no_optimal_flag == 0) {
            for(fill_count=0;fill_count<18;fill_count++) {
                previous_position[fill_count]=0;
            }
    }
}

static
void kt2_tdm_display(int unit,
                     uint32 *tdm,
                     uint32 total_tdm_slots,
                     uint32 row, uint32 col)
{
     uint32 inner_index=0;
     uint32 outer_index=0;

     soc_cm_debug(DK_VERBOSE,"\n");
     soc_cm_debug(DK_VERBOSE,"\t\t\t\t======TDM======\n");
     for (outer_index=0;outer_index < col ; outer_index ++ ) {
          soc_cm_debug(DK_VERBOSE,"\t%d",outer_index);
     }
     soc_cm_debug(DK_VERBOSE,"\n");
     for (outer_index=0;outer_index < col ; outer_index ++ ) {
          soc_cm_debug(DK_VERBOSE,"\t%d#",outer_index);
     }
     soc_cm_debug(DK_VERBOSE,"\n");

     for (outer_index=0; outer_index < total_tdm_slots ; outer_index +=col ) {
          soc_cm_debug(DK_VERBOSE,"%d==>\t",outer_index/col);
          for (inner_index=0;
               (inner_index < col) && 
               ((outer_index+inner_index) < total_tdm_slots) ; 
               inner_index++ ) {
               switch(tdm[outer_index+inner_index]) {
               case KT2_IDLE : soc_cm_debug(DK_VERBOSE,"IDLE\t"); break;
               case KT2_IDLE1 : soc_cm_debug(DK_VERBOSE,"IDLE1\t"); break;
               case KT2_LPBK_PORT : soc_cm_debug(DK_VERBOSE,"LPBK\t"); break;
               case KT2_CMIC_PORT : soc_cm_debug(DK_VERBOSE,"CMIC\t"); break;
               case KT2_OLP_PORT :  if (SOC_INFO(unit).olp_port) {
                                        soc_cm_debug(DK_VERBOSE,"OLP\t"); break;
                                    }
               default :            soc_cm_debug(DK_VERBOSE,"%d\t",
                                           tdm[outer_index+inner_index]);break;
               }
          }
          soc_cm_debug(DK_VERBOSE,"\n");
     }
}
soc_error_t kt2_tdm_generate(
            uint32 *port_speed,
            uint32 total_ports, 
            uint32 tdm_freq       , /* In MHHz */
            uint32 total_tdm_slots,
            uint32 *tdm, 
            char  *tdm_config_string)
{
    char temp_string[80];
    uint32 reserved_flag=0;
    uint32 optimal_pos=0;
    uint32 mxqblock;
    uint32 next_mxqblock;
    uint32 temp;
    uint32 total_bw=0;
    uint32 port=0;
    uint32 count=0;
    uint32 index=0;
    uint32 index1=0;

    int32  pos=0;

    uint32 kt2_1g_ports=0;
    uint32 kt2_1g_ports_array[40]={0};
    uint32 kt2_1g_ports_mxqflags[10]={0};
    /* int    kt2_1g_ports_pos[2]={0}; */
    uint32 kt2_2g_ports=0;
    uint32 kt2_2g_ports_array[40]={0};
    uint32 kt2_2g_ports_mxqflags[10]={0};

    uint32 kt2_2_5g_ports=0;
    uint32 kt2_2_5g_ports_array[40]={0};
    uint32 kt2_2_5g_ports_mxqflags[10]={0};

    uint32 kt2_10g_ports=0;
    uint32 kt2_10g_ports_array[40]={0};
    uint32 kt2_13g_ports=0;
    uint32 kt2_13g_ports_array[40]={0};
    /*
    uint32 kt2_20g_ports=0;
    uint32 kt2_20g_ports_array[40]={0};
    */
    uint32 kt2_21g_ports=0;
    uint32 kt2_21g_ports_array[40]={0};

    uint32 kt2_21g_reserved_position[2]={0,2};
    /* uint32 kt2_20g_reserved_position[2]={0,2}; */
    uint32 kt2_13g_reserved_position[2]={1,4};
    uint32 kt2_10g_reserved_position[2]={3,8};
    uint32 tdm_freq_index=0; 
    uint32 min_tdm_cycles=0; 
    uint32 lpbk_min_tdm_cycles=0; 
    uint32 cmic_min_tdm_cycles=0; 
    uint32 worse_tdm_slot_spacing=0; 
    uint32 optimal_tdm_slot_spacing=0; 
    /* uint32 min_tdm_slot_spacing=0; */
    uint32 speed_index=0; 
    uint32 speed_index_1g=0; 
    uint32 speed_index_2g=1; 
    uint32 speed_index_2_5g=2; 
    uint32 speed_index_10g=3; 
    uint32 speed_index_13g=4; 
    /* uint32 speed_index_20g=5;  */
    uint32 speed_index_21g=6; 
    uint32 total_slots=0;
    unsigned int row=0;
    unsigned int col=0;

    /* To avoid unused warning message with GCC 4.6.x */ 
    row = row;
    col = col; 

    for(index=0;index<10;index++) {
        kt2_tdm_mxqblock_ports_used[index]=0;
    }
    switch(tdm_freq) {
    case 80:  tdm_freq_index=0;
              row=4;
              col=4;
              break;
    case 110: tdm_freq_index=1;
              row=4;
              col=5;
              break;
    case 155: tdm_freq_index=2;
              row=12;
              col=7;
              break;
    case 185: if (total_tdm_slots == 80 ) {
                  row=16;
                  col=5;
                  tdm_freq_index=3; 
              } else {
                  tdm_freq_index=4; 
                  row=18;
                  col=5;
              }
              reserved_flag=1;
              break;
    default:  soc_cm_debug(DK_ERR,"Unsupported tdm frequency:%d \n",
                           tdm_freq);
              return SOC_E_FAIL;
    }

    for (port=0;port<total_ports;port++) {
         switch(port_speed[port]) {
         case 1000:  kt2_1g_ports_array[kt2_1g_ports]=port+1;
                     kt2_1g_ports++;
                     kt2_1g_ports_mxqflags[kt2_port_to_mxqblock[port]]=1;
                     total_slots += kt2_tdm_cycles_info[tdm_freq_index]
                                    [speed_index_1g].min_tdm_cycles;
                     break;
         case 2000:  kt2_2g_ports_array[kt2_2g_ports]=port+1;
                     kt2_2g_ports++;
                     kt2_2g_ports_mxqflags[kt2_port_to_mxqblock[port]]=1;
                     total_slots += kt2_tdm_cycles_info[tdm_freq_index]
                                    [speed_index_2g].min_tdm_cycles;
                     break;
         case 2500:  kt2_2_5g_ports_array[kt2_2_5g_ports]=port+1;
                     kt2_2_5g_ports++;
                     kt2_2_5g_ports_mxqflags[kt2_port_to_mxqblock[port]]=1;
                     total_slots += kt2_tdm_cycles_info[tdm_freq_index]
                                    [speed_index_2_5g].min_tdm_cycles;
                     break;
         case 10000: kt2_10g_ports_array[kt2_10g_ports]=port+1;kt2_10g_ports++;
                     total_slots += kt2_tdm_cycles_info[tdm_freq_index]
                                    [speed_index_10g].min_tdm_cycles;
                     break;
         case 13000: kt2_13g_ports_array[kt2_13g_ports]=port+1;kt2_13g_ports++;
                     total_slots += kt2_tdm_cycles_info[tdm_freq_index]
                                    [speed_index_13g].min_tdm_cycles;
                     break;
         case 20000:
         case 21000: kt2_21g_ports_array[kt2_21g_ports]=port+1;kt2_21g_ports++;
                     total_slots += kt2_tdm_cycles_info[tdm_freq_index]
                                    [speed_index_21g].min_tdm_cycles;
                     break;
         case 0    : continue;
         default   : soc_cm_debug(DK_ERR,
                                "Unsupported speed%d\n",port_speed[port]);
                     return SOC_E_FAIL;
         }     
         kt2_tdm_mxqblock_ports_used[kt2_port_to_mxqblock[port]]++;
    }
    /*
     * Multiply each base speed by 10 and then divide the total
     * bandwidth by 10 in order to avoid floating point (2.5G).
     */
    total_bw = (10 * kt2_1g_ports + 
                20 * kt2_2g_ports +
                25 * kt2_2_5g_ports +
                100 * kt2_10g_ports +
                130 * kt2_13g_ports +
                210 * kt2_21g_ports) / 10;
    if (total_bw > 92) {
        soc_cm_debug(DK_ERR,"Exceeded max bw:92 %d \n",total_bw);
        return SOC_E_FAIL;
    }
    speed_index=speed_index_2_5g;
    lpbk_min_tdm_cycles=kt2_tdm_cycles_info[tdm_freq_index][speed_index].
                        min_tdm_cycles;
    speed_index=speed_index_2g;
    cmic_min_tdm_cycles=kt2_tdm_cycles_info[tdm_freq_index][speed_index].
                        min_tdm_cycles;
    if (total_slots > (total_tdm_slots -
                       lpbk_min_tdm_cycles - cmic_min_tdm_cycles)) {
        soc_cm_debug(DK_ERR,"Exceeded max slots:%d %d \n",
                     (total_tdm_slots-
                      lpbk_min_tdm_cycles-cmic_min_tdm_cycles),total_slots);
        return SOC_E_FAIL;
    }

    sal_strcpy(tdm_config_string,"");
    if(kt2_21g_ports) {
       sal_sprintf(temp_string," %d*21HG ",kt2_21g_ports);
       sal_sprintf(tdm_config_string,"%s%s",tdm_config_string,temp_string);
    }
    if(kt2_13g_ports) {
       sal_sprintf(temp_string,"  %d*13HG ",kt2_13g_ports);
       sal_sprintf(tdm_config_string,"%s%s",tdm_config_string,temp_string);
    }
    if(kt2_10g_ports) {
       sal_sprintf(temp_string,"  %d*10HG ",kt2_10g_ports);
       sal_sprintf(tdm_config_string,"%s%s",tdm_config_string,temp_string);
    }
    if(kt2_2_5g_ports) {
       sal_sprintf(temp_string,"  %d*2.5G ",kt2_2_5g_ports);
       sal_sprintf(tdm_config_string,"%s%s",tdm_config_string,temp_string);
    }
    if(kt2_2g_ports) {
       sal_sprintf(temp_string,"  %d*2G ",kt2_2g_ports);
       sal_sprintf(tdm_config_string,"%s%s",tdm_config_string,temp_string);
    }
    if(kt2_1g_ports) {
       sal_sprintf(temp_string,"  %d*1G ",kt2_1g_ports);
       sal_sprintf(tdm_config_string,"%s%s",tdm_config_string,temp_string);
    }
    sal_sprintf(temp_string,"TotalBw=%d",total_bw);
    sal_sprintf(tdm_config_string,"%s%s",tdm_config_string,temp_string);
                              
    /* soc_cm_debug(DK_ERR,"Total BW:%d \n",total_bw); */
    for(pos=0;pos<total_tdm_slots;pos++) {
        tdm[pos]=KT2_IDLE;
        kt2_tdm_position_mxq_mask[pos]=0;
    }
    /* kt2_tdm_display(unit,tdm,total_tdm_slots,row,col);  */
    /* 1. First fill-up 10G ports */ 
    speed_index=speed_index_10g;
    min_tdm_cycles=kt2_tdm_cycles_info[tdm_freq_index][speed_index].
                   min_tdm_cycles;
    worse_tdm_slot_spacing=kt2_tdm_cycles_info[tdm_freq_index][speed_index].
                           worse_tdm_slot_spacing;
    optimal_tdm_slot_spacing=kt2_tdm_cycles_info[tdm_freq_index][speed_index].
                             optimal_tdm_slot_spacing;

    count=0;
    /* MAX 4 (2HG*2) 10G ports can be filled-up over here */
    for(index=kt2_21g_ports;index<2;index++) {
        for(index1=0;index1<2 && count<kt2_10g_ports;index1++,count++) {
            if (reserved_flag == 1) {
                /* 5=(9+1)/2 */
                pos = kt2_21g_reserved_position[index] + col*index1;
            } else {
                pos = optimal_pos= 0;
            }
            kt2_fill_pos(tdm, total_tdm_slots, 
                         pos,  kt2_10g_ports_array[index],10000,min_tdm_cycles,
                         worse_tdm_slot_spacing,optimal_tdm_slot_spacing);
            /* kt2_tdm_display(unit,tdm,total_tdm_slots,row,col);  */
        }
    }
    /* MAX 2 10G ports can be filled-up over here */
    for(index1=0;count<kt2_10g_ports;index1++,count++) {
        if (reserved_flag == 1) {
            if (index1 == 2) {
                break;
            }
            pos = kt2_10g_reserved_position[index1] ;
        } else {
            pos = optimal_pos = 0;
        }
        kt2_fill_pos(tdm, total_tdm_slots, pos,  
                     kt2_10g_ports_array[count],10000,min_tdm_cycles,
                     worse_tdm_slot_spacing,optimal_tdm_slot_spacing);
        /* kt2_tdm_display(unit,tdm,total_tdm_slots,row,col);  */
    }
    /* 10G ports can still be used(2 more) if 13G ports are not used !! */
    if (count < kt2_10g_ports) {
        if (kt2_13g_ports != 0) {
            soc_cm_debug(DK_ERR,"13G ports being used.."
                                "max 10G port can be 6 only\n");
            return SOC_E_FAIL;
        }
        for(index1=0; index1<2 && count<kt2_10g_ports; index1++,count++) {
             if (reserved_flag == 1) {
                 pos = kt2_13g_reserved_position[0] + col*index1;/*5=(9+1)/2*/
             } else {
                 pos = 0;
             }
             kt2_fill_pos(tdm, total_tdm_slots, pos,  
                          kt2_10g_ports_array[count],10000,min_tdm_cycles,
                          worse_tdm_slot_spacing,optimal_tdm_slot_spacing);
             /* kt2_tdm_display(unit,tdm,total_tdm_slots,row,col);  */
        }
    }
    speed_index=speed_index_21g;
    min_tdm_cycles=kt2_tdm_cycles_info[tdm_freq_index][speed_index].
                   min_tdm_cycles;
    worse_tdm_slot_spacing=kt2_tdm_cycles_info[tdm_freq_index][speed_index].
                           worse_tdm_slot_spacing;
    optimal_tdm_slot_spacing=kt2_tdm_cycles_info[tdm_freq_index][speed_index].
                             optimal_tdm_slot_spacing;

    /* 2. Fill-up 21,20G ports now */ 
    for(index=0;index<kt2_21g_ports;index++) {
        if (reserved_flag == 1) {
            pos = kt2_21g_reserved_position[index] ;
        } else {
            pos = 0;
        }
        kt2_fill_pos(tdm, total_tdm_slots, pos,  
                     kt2_21g_ports_array[index],21000,min_tdm_cycles,
                     worse_tdm_slot_spacing,optimal_tdm_slot_spacing);
        /* kt2_tdm_display(unit,tdm,total_tdm_slots,row,col);  */
    }
    speed_index=speed_index_13g;
    min_tdm_cycles=kt2_tdm_cycles_info[tdm_freq_index][speed_index].
                   min_tdm_cycles;
    worse_tdm_slot_spacing=kt2_tdm_cycles_info[tdm_freq_index][speed_index].
                           worse_tdm_slot_spacing;
    optimal_tdm_slot_spacing=kt2_tdm_cycles_info[tdm_freq_index][speed_index].
                             optimal_tdm_slot_spacing;

    /* 3. Fill-up 13G ports now */ 
    for(index=0;index<kt2_13g_ports;index++) {
        if (reserved_flag == 1) {
        switch(index) {
        case 0:
        case 1: pos = kt2_13g_reserved_position[index] ; break;
        case 2:
        case 3: if (tdm[kt2_21g_reserved_position[0]] == KT2_IDLE) {
                    pos = kt2_21g_reserved_position[0] ; break;
                }
                if (tdm[kt2_10g_reserved_position[0]] == KT2_IDLE) {
                    pos = kt2_10g_reserved_position[0] ; break;
                }
                soc_cm_debug(DK_ERR,
                             "21G  and 10G ports Slots are not free :%d \n",
                             index);
                return SOC_E_FAIL;
        }
        } else {
            pos = 0;
        }
        kt2_fill_pos(tdm, total_tdm_slots, pos,  
                     kt2_13g_ports_array[index],13000,min_tdm_cycles,
                     worse_tdm_slot_spacing,optimal_tdm_slot_spacing);
        /* kt2_tdm_display(unit,tdm,total_tdm_slots,row,col);  */
    }
    /* 3. Fill-up Loopback ports now */ 
    speed_index=speed_index_2_5g;
    min_tdm_cycles=kt2_tdm_cycles_info[tdm_freq_index][speed_index].
                   min_tdm_cycles;
    worse_tdm_slot_spacing=kt2_tdm_cycles_info[tdm_freq_index][speed_index].
                           worse_tdm_slot_spacing;
    optimal_tdm_slot_spacing=kt2_tdm_cycles_info[tdm_freq_index][speed_index].
                             optimal_tdm_slot_spacing;

    kt2_fill_pos(tdm, total_tdm_slots, -1,  KT2_LPBK_PORT,2500, min_tdm_cycles,
                 worse_tdm_slot_spacing,optimal_tdm_slot_spacing);
    /* 4. Fill-up CMIC ports now */ 
    speed_index=speed_index_2g;
    min_tdm_cycles=kt2_tdm_cycles_info[tdm_freq_index][speed_index].
                   min_tdm_cycles;
    worse_tdm_slot_spacing=kt2_tdm_cycles_info[tdm_freq_index][speed_index].
                           worse_tdm_slot_spacing;
    optimal_tdm_slot_spacing=kt2_tdm_cycles_info[tdm_freq_index][speed_index].
                             optimal_tdm_slot_spacing;
    /* min_tdm_slot_spacing=optimal_tdm_slot_spacing-(worse_tdm_slot_spacing);*/
    pos = 0;
    while (tdm[pos] != KT2_LPBK_PORT) {
           pos++;
    }
    pos = pos + 3 + 1; /* Minimum 3 spacing */
    while (tdm[pos] != KT2_IDLE) {
           pos++;
    }
    tdm[pos] = KT2_CMIC_PORT;
    /* kt2_tdm_display(unit,tdm,total_tdm_slots,row,col);  */
    kt2_fill_pos(tdm, total_tdm_slots, pos+optimal_tdm_slot_spacing+1,  
                 KT2_CMIC_PORT,2000, min_tdm_cycles-1,
                 worse_tdm_slot_spacing,optimal_tdm_slot_spacing);
    /* kt2_tdm_display(unit,tdm,total_tdm_slots,row,col);  */

    /* 5. Fill-up remaining 2.5G ports now */ 

    /* 5.1 First Re-arrange 2.5g ports */
    port = kt2_2_5g_ports_array[0];
    for (index=1;index < kt2_2_5g_ports;index++) {
         mxqblock = kt2_port_to_mxqblock[port-1];
         next_mxqblock=mxqblock;
         do {
             next_mxqblock=(next_mxqblock+1)%10;
             if (kt2_2_5g_ports_mxqflags[next_mxqblock] == 1) {
                 break;
             }
         }while (next_mxqblock != mxqblock);
         /* Swap entry with next mxqblock */
         index1=index;
         do {
             port = kt2_2_5g_ports_array[index1];
             if ( kt2_port_to_mxqblock[port-1] == next_mxqblock) {
                  break;
             }
             index1=(index1+1)%kt2_2_5g_ports;
         } while ( index1 != index);
         temp = kt2_2_5g_ports_array[index1];
         kt2_2_5g_ports_array[index1]= kt2_2_5g_ports_array[index];
         kt2_2_5g_ports_array[index]= temp;
    }

    /* 5.2 Now update tdm for 2.5g ports */
    speed_index=speed_index_2_5g;
    min_tdm_cycles=kt2_tdm_cycles_info[tdm_freq_index][speed_index].
                   min_tdm_cycles;
    worse_tdm_slot_spacing=kt2_tdm_cycles_info[tdm_freq_index][speed_index].
                           worse_tdm_slot_spacing;
    optimal_tdm_slot_spacing=kt2_tdm_cycles_info[tdm_freq_index][speed_index].
                             optimal_tdm_slot_spacing;

    for(index=0;index<kt2_2_5g_ports;index++) {
        port = kt2_2_5g_ports_array[index];
        kt2_fill_pos(tdm, total_tdm_slots, -1,  
                     port,2500, min_tdm_cycles,
                     worse_tdm_slot_spacing,optimal_tdm_slot_spacing);
        /* kt2_tdm_display(unit,tdm,total_tdm_slots,row,col);  */
    }

    /* 6. Fill-up 2G ports now */ 

    /* 6.1 Re-arrange 2g ports first */
    port = kt2_2g_ports_array[0];
    for (index=1;index < kt2_2g_ports;index++) {
         mxqblock = kt2_port_to_mxqblock[port-1];
         next_mxqblock=mxqblock;
         do {
             next_mxqblock=(next_mxqblock+1)%10;
             if (kt2_2g_ports_mxqflags[next_mxqblock] == 1) {
                 break;
             }
         }while (next_mxqblock != mxqblock);
         /* Swap entry with next mxqblock */
         index1=index;
         do {
             port = kt2_2g_ports_array[index1];
             if ( kt2_port_to_mxqblock[port-1] == next_mxqblock) {
                  break;
             }
             index1=(index1+1)%kt2_2g_ports;
         } while ( index1 != index);
         temp = kt2_2g_ports_array[index1];
         kt2_2g_ports_array[index1]= kt2_2g_ports_array[index];
         kt2_2g_ports_array[index]= temp;
    }

    /* 6.2 Now update tdm for 2g ports  */
    speed_index=speed_index_2g;
    min_tdm_cycles=kt2_tdm_cycles_info[tdm_freq_index][speed_index].
                   min_tdm_cycles;
    worse_tdm_slot_spacing=kt2_tdm_cycles_info[tdm_freq_index][speed_index].
                           worse_tdm_slot_spacing;
    optimal_tdm_slot_spacing=kt2_tdm_cycles_info[tdm_freq_index][speed_index].
                             optimal_tdm_slot_spacing;

    for(index=0;index<kt2_2g_ports;index++) {
        port = kt2_2g_ports_array[index];
        kt2_fill_pos(tdm, total_tdm_slots, -1,  
                     port,2000, min_tdm_cycles,
                     worse_tdm_slot_spacing,optimal_tdm_slot_spacing);
        /* kt2_tdm_display(unit,tdm,total_tdm_slots,row,col);  */
    }

    /* Re-arrange 1g ports */
    port = kt2_1g_ports_array[0];
    for (index=1;index < kt2_1g_ports;index++) {
         mxqblock = kt2_port_to_mxqblock[port-1];
         next_mxqblock=mxqblock;
         do {
             next_mxqblock=(next_mxqblock+1)%10;
             if (kt2_1g_ports_mxqflags[next_mxqblock] == 1) {
                 break;
             }
         }while (next_mxqblock != mxqblock);
         /* Swap entry with next mxqblock */
         index1=index;
         do {
             port = kt2_1g_ports_array[index1];
             if ( kt2_port_to_mxqblock[port-1] == next_mxqblock) {
                  break;
             }
             index1=(index1+1)%kt2_1g_ports;
         } while ( index1 != index);
         temp = kt2_1g_ports_array[index1];
         kt2_1g_ports_array[index1]= kt2_1g_ports_array[index];
         kt2_1g_ports_array[index]= temp;
    } 
    speed_index=speed_index_1g;
    min_tdm_cycles=kt2_tdm_cycles_info[tdm_freq_index][speed_index].
                   min_tdm_cycles;
    worse_tdm_slot_spacing=kt2_tdm_cycles_info[tdm_freq_index][speed_index].
                           worse_tdm_slot_spacing;
    optimal_tdm_slot_spacing=kt2_tdm_cycles_info[tdm_freq_index][speed_index].
                             optimal_tdm_slot_spacing;
    /* 6. Fill-up 1G ports now */ 
    for(index=0;index<kt2_1g_ports;index++) {
        port = kt2_1g_ports_array[index];
        kt2_fill_pos(tdm, total_tdm_slots, 0,  port,1000, min_tdm_cycles,
                     worse_tdm_slot_spacing,optimal_tdm_slot_spacing);
        /* kt2_tdm_display(unit,tdm,total_tdm_slots,row,col);  */
    }
    return SOC_E_NONE;
}



static uint32
 _soc_katana2_mmu_tdm_var(uint32 *arr, uint16 dev_id, int cfg_num, int pos)
{
    uint32 retval;

    retval = arr[pos];
    
    switch (dev_id) {
        case BCM56450_DEVICE_ID:
            if (cfg_num == 1) {
                switch (pos) {
                    case 4: case 17: case 30: case 43: case 56: case 69:
                        retval = 28;
                        break;
                    case 5: case 18: case 31: case 44: case 57: case 70:
                        retval = 29;
                        break;
                    case 10: case 23: case 36: case 49: case 62: case 75:
                        retval = 30;
                        break;
                    case 11: case 24: case 37: case 50: case 63: case 76:
                        retval = 31;
                        break;
                }
            }
            break;
        default:
            break;
    }

    return retval;
}

 STATIC soc_error_t
_soc_katana2_mmu_tdm_init(int unit)
{
    uint32 *arr;
    uint32 olp_port_flag=0;
    uint32 offender_port=0;
    soc_error_t retVal=SOC_E_NONE;
    uint32 port=0;
    uint32 outer_index=0;
    uint32 inner_index=0;
    uint32 tdm_index=0;
    uint32  row=18;
    uint32  col=5;


#ifdef UNDER_TESTING
    uint32 bcm56450_test_selection=0;
    uint32 *bcm56450_test_speed[]={kt2_tdm_56450_test_speed0,
                                   kt2_tdm_56450_test_speed1,
                                   kt2_tdm_56450_test_speed2,
                                   kt2_tdm_56450_test_speed3,
                                   kt2_tdm_56450_test_speed4,
                                   kt2_tdm_56450_test_speed5,
                                   kt2_tdm_56450_test_speed6,
                                   kt2_tdm_56450_test_speed7,
                                   kt2_tdm_56450_test_speed8,
                                   kt2_tdm_56450_test_speed9,
                                   kt2_tdm_56450_test_speed10,
                                   kt2_tdm_56450_test_speed11,
                                   kt2_tdm_56450_test_speed12,
                                   kt2_tdm_56450_test_speed13,
                                   kt2_tdm_56450_test_speed14,
                                   kt2_tdm_56450_test_speed15,
                                   kt2_tdm_56450_test_speed16,
                                   kt2_tdm_56450_test_speed17,
                                   kt2_tdm_56450_test_speed18,
                                   kt2_tdm_56450_test_speed19,
                                   };
    uint32  pos=0;
    uint32  index=0;
    uint32  freq=0;


#endif
    int i, tdm_size,tdm_freq, cfg_num;
    iarb_tdm_table_entry_t iarb_tdm;
    lls_port_tdm_entry_t lls_tdm;
    uint32 rval, arr_ele;
    uint16 dev_id;
    uint8 rev_id;
    uint32 port_enable_value=0;

    soc_cm_get_id(unit, &dev_id, &rev_id);
    cfg_num = soc_property_get(unit, spn_BCM5645X_CONFIG, 0);
    
    switch(dev_id) {
    case BCM56450_DEVICE_ID:
         if (cfg_num >= (sizeof(bcm56450_tdm)/sizeof(bcm56450_tdm[0]))) {
             return SOC_E_FAIL;
         } 
         tdm_size= bcm56450_tdm_info[cfg_num].tdm_size;
         tdm_freq= bcm56450_tdm_info[cfg_num].tdm_freq;
         row= bcm56450_tdm_info[cfg_num].row;
         col= bcm56450_tdm_info[cfg_num].col;
         arr = bcm56450_tdm[cfg_num];
         switch(cfg_num -5) { /* Will be later changed to switch(cfg_num) */
         case 1:/* kt2_tdm_56450A_1 */
                sal_memcpy(arr,
                           &kt2_tdm_56450A_ref[0],sizeof(kt2_tdm_56450A_ref));
                kt2_tdm_replace(arr,88,KT2_OLP_PORT,KT2_IDLE,0);
                kt2_tdm_replace(arr,tdm_size,33,25,0);
                kt2_tdm_replace(arr,tdm_size,30,26,0);
                olp_port_flag=0;
                break;
         case 2:/* kt2_tdm_56450A_2 */
                sal_memcpy(arr,
                           &kt2_tdm_56450A_ref[0], sizeof(kt2_tdm_56450A_ref));
                kt2_tdm_replace(arr,88,33,25,0);
                kt2_tdm_replace(arr,88,30,26,0);
                olp_port_flag=1;
                break;
         case 3:/* kt2_tdm_56450A_3 */
                sal_memcpy(arr,
                           &kt2_tdm_56450A_ref[0],sizeof(kt2_tdm_56450A_ref));
                kt2_tdm_replace(arr,88,33,25,0);
                olp_port_flag=1;
                break;
         case 4:/* kt2_tdm_56450A_4 */
                sal_memcpy(arr,
                           &kt2_tdm_56450A_ref[0], sizeof(kt2_tdm_56450A_ref));
                olp_port_flag=1;
                break;
         case 16:/* kt2_tdm_56450A_16 */
                sal_memcpy(arr,
                           &kt2_tdm_56450A_ref[0], sizeof(kt2_tdm_56450A_ref));
                kt2_tdm_replace(arr,88,30,KT2_IDLE,0);
                kt2_tdm_replace(arr,88,33,KT2_IDLE,0);
                olp_port_flag=1;
                break;
         case 17:/* kt2_tdm_56450A_17 */
                sal_memcpy(arr,
                           &kt2_tdm_56450A_ref[0], sizeof(kt2_tdm_56450A_ref));
                olp_port_flag=1;
                break;
         case 20:/* kt2_tdm_56450DA_20 */
                sal_memcpy(arr,
                           &kt2_tdm_56450D_ref[0], sizeof(kt2_tdm_56450D_ref));
                break;
         case 5:/* kt2_tdm_56450A_5 */
                sal_memcpy(arr,
                           &kt2_tdm_56450A_ref[0], sizeof(kt2_tdm_56450A_ref));
                kt2_tdm_swap(arr,88,9,17);
                kt2_tdm_replace(arr,88,17,26,0);
                kt2_tdm_replace(arr,88,21,39,0);
                olp_port_flag=1;
                break;
         case 6:/*  kt2_tdm_56450A_6 */
                sal_memcpy(arr,
                           &kt2_tdm_56450A_ref[0], sizeof(kt2_tdm_56450A_ref));
                kt2_tdm_replace(arr,88,28,36,0);
                kt2_tdm_replace(arr,88,27,28,0);
                kt2_tdm_replace(arr,88,9,27,0);
                kt2_tdm_replace(arr,88,33,25,0);
                kt2_tdm_replace(arr,88,30,26,0);
                kt2_tdm_replace(arr,88,13,39,0);
                kt2_tdm_replace(arr,88,17,33,0);
                kt2_tdm_replace(arr,88,21,30,0);
                olp_port_flag=1;
                break;
         case 7:/* kt2_tdm_56450A_7 */
                sal_memcpy(arr,
                           &kt2_tdm_56450A_ref[0], sizeof(kt2_tdm_56450A_ref));
                kt2_tdm_swap(arr,88,9,17);
                kt2_tdm_replace(arr,88,13,25,0);
                kt2_tdm_replace(arr,88,17,26,0);
                kt2_tdm_replace(arr,88,21,39,0);


                kt2_tdm_replace(arr,88,1,KT2_IDLE,0);
                kt2_tdm_replace(arr,88,5,KT2_IDLE,0);
                kt2_tdm_replace(arr,88,9,KT2_IDLE,0);
                tdm_index=0;
                for (outer_index=1;outer_index<=4;outer_index++) {
                     for (inner_index=0;inner_index < 6;inner_index++) {
                          port = outer_index + inner_index*4;
                          while(arr[tdm_index] != KT2_IDLE) {
                                tdm_index++;
                          }
                          arr[tdm_index]=port;
                     }
                }
                olp_port_flag=1;
                break;
         case 8:/* kt2_tdm_56450A1_8 */
                sal_memcpy(arr,
                           &kt2_tdm_56450A1_ref[0], 
                           sizeof(kt2_tdm_56450A1_ref));
                olp_port_flag=1;
                break;
         case 9:/* kt2_tdm_56450A1_9 */
                sal_memcpy(arr,
                           &kt2_tdm_56450A1_ref[0],
                           sizeof(kt2_tdm_56450A1_ref));
                kt2_tdm_replace(arr,108,9,KT2_IDLE,0);
                kt2_tdm_replace(arr,108,13,KT2_IDLE,0);
                for(port=1;port<=8;port++) {
                    kt2_tdm_replace(arr,108,port,KT2_IDLE,0);
                }
                tdm_index=0;
                for (outer_index=1;outer_index<=4;outer_index++) {
                     for (inner_index=0;inner_index < 6;inner_index++) {
                          port = outer_index + inner_index*4;/* 1,5,9,13,17,21
                                                                2,6,10,14,18,22,
                                                                3,7,11,15,19,23,
                                                                4,8,12,16,20,24
                                                              */
                          while(arr[tdm_index] != KT2_IDLE) {
                                 tdm_index++;
                          }
                          arr[tdm_index]=port;
                     }
                }
                olp_port_flag=1;
                break;
         case 18:/* kt2_tdm_56450A1_18 */
                sal_memcpy(arr,
                           &kt2_tdm_56450A1_ref[0], sizeof(kt2_tdm_56450A1_ref));
                kt2_tdm_replace(arr,108,9,KT2_IDLE,0);
                kt2_tdm_replace(arr,108,13,KT2_IDLE,0);
                for(port=1;port<=8;port++) {
                    kt2_tdm_replace(arr,108,port,KT2_IDLE,0);
                }
                tdm_index=0;
                for (outer_index=1;outer_index<=4;outer_index++) {
                     for (inner_index=0;inner_index < 6;inner_index++) {
                          port = outer_index + inner_index*4;/* 1,5,9,13,17,21
                                                                2,6,10,14,18,22,
                                                                3,7,11,15,19,23,
                                                                4,8,12,16,20,24
                                                              */
                          while(arr[tdm_index] != KT2_IDLE) {
                                 tdm_index++;
                          }
                          arr[tdm_index]=port;
                     }
                }
                olp_port_flag=1;
                break;
         case 10: /* kt2_tdm_56450A2_10 */
                  sal_memcpy(arr,
                             &kt2_tdm_56450A2_ref[0],
                             sizeof(kt2_tdm_56450A2_ref));
                  kt2_tdm_replace(arr,99,KT2_OLP_PORT,KT2_IDLE,0);
                  olp_port_flag=0;
                  break;
         case 11: /* kt2_tdm_56450A2_11 */
                  sal_memcpy(arr,
                            &kt2_tdm_56450A2_ref[0],
                            sizeof(kt2_tdm_56450A2_ref));
                  olp_port_flag=1;
                  break;
         case 12: /* kt2_tdm_56450A2_12 */
                  sal_memcpy(arr,
                             &kt2_tdm_56450A2_ref[0],
                             sizeof(kt2_tdm_56450A2_ref));
                  kt2_tdm_replace(arr,99,26,28,0);
                  olp_port_flag=1;
                  break;
         case 13: /* kt2_tdm_56450A2_13 */
                  sal_memcpy(arr,
                             &kt2_tdm_56450A2_ref[0],
                             sizeof(kt2_tdm_56450A2_ref));
                  kt2_tdm_replace(arr,99,26,28,0);
                  kt2_tdm_replace(arr,99,25,27,0);
                  olp_port_flag=1;
                  break;
         case 14: /* kt2_tdm_56450A2_14 */
                  sal_memcpy(arr,
                             &kt2_tdm_56450A2_ref[0],
                             sizeof(kt2_tdm_56450A2_ref));
                  kt2_tdm_replace(arr,99,21,KT2_IDLE,0);
                  kt2_tdm_replace(arr,99,17,KT2_IDLE,0);
                  olp_port_flag=1;
                  break;
         case 15: /* kt2_tdm_56450A2_15 */
                  memcpy(arr,
                         &kt2_tdm_56450A2_ref[0],
                         sizeof(kt2_tdm_56450A2_ref));
                  kt2_tdm_replace(arr,99,21,KT2_IDLE,0);
                  kt2_tdm_replace(arr,99,17,KT2_IDLE,0);
                  kt2_tdm_replace(arr,99,9,KT2_IDLE,0);
                  kt2_tdm_replace(arr,99,13,17,0);
                  for(port=1;port<=8;port++) {
                      kt2_tdm_replace(arr,99,port,KT2_IDLE,0);
                  }
                  tdm_index=0;
                  for (outer_index=1;outer_index<=4;outer_index++) {
                       for (inner_index=0;inner_index < 4;inner_index++) {
                            port = outer_index + inner_index*4;
                            while(arr[tdm_index] != KT2_IDLE) {
                                  tdm_index++;
                            }
                            arr[tdm_index]=port;
                       }
                  }
                  olp_port_flag=1;
                  break;
         case 19:/* kt2_tdm_56450AA_19 */
                sal_memcpy(arr,
                          &kt2_tdm_56450AA_ref[0], sizeof(kt2_tdm_56450AA_ref));
                olp_port_flag=0;
                break;
         default: olp_port_flag=0;
                  break;
         }
         soc_cm_print("Cfg=%d \n",cfg_num);
         kt2_tdm_display(unit,arr, tdm_size,  row,  col);
         /* speed = bcm56450_speed[cfg_num]; */
         retVal=kt2_tdm_verify(bcm56450_tdm[cfg_num],tdm_size,tdm_freq,
                               bcm56450_speed[cfg_num],
                               olp_port_flag,&offender_port);  
         if ((retVal == SOC_E_CONFIG) && (offender_port == KT2_IDLE)) {
              soc_cm_debug(DK_WARN,
                 "#Ignoring KT2:TDM IDLE Slot verification failure time-being\n"
                 "Issue has already been addressed but under testing\n");
              retVal=SOC_E_NONE;
         } 
         SOC_IF_ERROR_RETURN(retVal);
         /* ######################## */
         /* Just for testing purpose */
         /* ######################## */
#ifdef UNDER_TESTING
         for(bcm56450_test_selection=0;
             bcm56450_test_selection < sizeof(bcm56450_test_speed)/
                                       sizeof(bcm56450_test_speed[0]);
             bcm56450_test_selection++) {
             if ((bcm56450_test_selection >=0) &&
                 (bcm56450_test_selection <=10)) {
                 pos = 90; row = 18; col = 5;freq=185;
             }
             if ((bcm56450_test_selection >=11) &&
                 (bcm56450_test_selection <=13)) {
                 pos = 80; row = 16; col = 5;freq=185;
             }
             if ((bcm56450_test_selection >=14) &&
                 (bcm56450_test_selection <=16)) {
                 pos = 84; row = 12; col = 7;freq=155;
             }
             if ((bcm56450_test_selection >=17) &&
                 (bcm56450_test_selection <=18)) {
                 pos = 20; row = 4; col = 5;freq=110;
             }
             if ((bcm56450_test_selection >=19) &&
                 (bcm56450_test_selection <=19)) {
                 pos = 16; row = 4; col = 4;freq=80;
             }
             soc_cm_debug(DK_WARN,"\n###############\n");
             SOC_IF_ERROR_RETURN(kt2_tdm_generate(
                                 bcm56450_test_speed[bcm56450_test_selection],
                                 40,freq,pos,
                                 kt2_tdm_under_testing,tdm_config_string));
             retVal = kt2_tdm_verify(
                                 kt2_tdm_under_testing,pos,freq, 
                                 bcm56450_test_speed[bcm56450_test_selection],
                                 olp_port_flag,&offender_port);
             for(index=0;index<180;index++) {
                  kt2_tdm_under_testing_idle[index]=KT2_IDLE;
             }
             sal_memcpy(kt2_tdm_under_testing_idle,kt2_tdm_under_testing,
                        sizeof(uint32)*pos);
             if ((retVal == SOC_E_CONFIG) && (offender_port == KT2_IDLE)) {
                 do {
                    switch(offender_port) {
                    case KT2_IDLE:
                         if (kt2_tdm_under_testing_idle[pos-1] == KT2_IDLE) {
                             pos++;
                         } else {
                             pos+=2;
                         }
                         break;
                    default:kt2_tdm_under_testing_idle[pos-2]=offender_port;
                            pos++;
                            break; 
                    }
                    if (pos == 180) {
                        break;
                    }
                    retVal = kt2_tdm_verify(
                                 kt2_tdm_under_testing_idle,pos, freq, 
                                 bcm56450_test_speed[bcm56450_test_selection],
                                 olp_port_flag,&offender_port);
                  }while(retVal == SOC_E_CONFIG);
             } 
             SOC_IF_ERROR_RETURN(retVal);
             soc_cm_debug(DK_WARN,"Configuration:%d:(freq:%d Cycles:%d) %s",
                                  bcm56450_test_selection,freq,pos,
                                  tdm_config_string);
             kt2_tdm_display(unit,kt2_tdm_under_testing_idle,pos,row,col);
             soc_cm_debug(DK_WARN,"###############\n");
         } 
#endif
         break; 
    default:
        return SOC_E_FAIL;
    }

    /* Disable IARB TDM before programming... */
    SOC_IF_ERROR_RETURN(READ_IARB_TDM_CONTROLr(unit, &rval));
    soc_reg_field_set(unit, IARB_TDM_CONTROLr, &rval, DISABLEf, 1);
    soc_reg_field_set(unit, IARB_TDM_CONTROLr, &rval, TDM_WRAP_PTRf,
                      tdm_size -1);
    SOC_IF_ERROR_RETURN(WRITE_IARB_TDM_CONTROLr(unit, rval));

    for (i = 0; i < tdm_size; i++) {
        arr_ele = _soc_katana2_mmu_tdm_var(arr, dev_id, cfg_num, i);
        if (arr_ele <= 41) { /* Non Idle Slots */
            port_enable_value = 1;
        } else {
            port_enable_value = 0;
        }

        sal_memset(&iarb_tdm, 0, sizeof(iarb_tdm_table_entry_t));
        soc_IARB_TDM_TABLEm_field32_set(unit, &iarb_tdm, PORT_NUMf,
                                        arr_ele);
        SOC_IF_ERROR_RETURN(WRITE_IARB_TDM_TABLEm(unit, SOC_BLOCK_ALL, i, 
                                                  &iarb_tdm));

        if (0 == (i%2)) {
            /* Two entries per mem entry */
            sal_memset(&lls_tdm, 0, sizeof(lls_port_tdm_entry_t));
            soc_LLS_PORT_TDMm_field32_set(unit, &lls_tdm, PORT_ID_0f, arr_ele);
            soc_LLS_PORT_TDMm_field32_set(unit, &lls_tdm, 
                                          PORT_ID_0_ENABLEf, port_enable_value);
        } else {
            soc_LLS_PORT_TDMm_field32_set(unit, &lls_tdm, PORT_ID_1f, arr_ele);
            soc_LLS_PORT_TDMm_field32_set(unit, &lls_tdm, 
                                          PORT_ID_1_ENABLEf, port_enable_value);
            SOC_IF_ERROR_RETURN(WRITE_LLS_PORT_TDMm(unit, SOC_BLOCK_ALL, (i/2),
                                                      &lls_tdm));
            SOC_IF_ERROR_RETURN(WRITE_LLS_PORT_TDMm(unit, SOC_BLOCK_ALL, 128+(i/2),
                                                      &lls_tdm));
        }
    }
    SOC_IF_ERROR_RETURN(READ_IARB_TDM_CONTROLr(unit, &rval));
    soc_reg_field_set(unit, IARB_TDM_CONTROLr, &rval, DISABLEf, 0);
    soc_reg_field_set(unit, IARB_TDM_CONTROLr, &rval, TDM_WRAP_PTRf,
                      tdm_size -1);
    SOC_IF_ERROR_RETURN(WRITE_IARB_TDM_CONTROLr(unit, rval));

    
    rval = 0;
    soc_reg_field_set(unit, LLS_TDM_CAL_CFGr, &rval, END_Af, tdm_size - 1);
    soc_reg_field_set(unit, LLS_TDM_CAL_CFGr, &rval, END_Bf, tdm_size - 1);
    soc_reg_field_set(unit, LLS_TDM_CAL_CFGr, &rval, ENABLEf, 1);
    SOC_IF_ERROR_RETURN(WRITE_LLS_TDM_CAL_CFGr(unit, rval));
    soc_katana2_save_tdm_pos(unit, tdm_size,arr);
    mxqspeeds[6][0]= bcm56450_speed[cfg_num][25-1];
    mxqspeeds[7][0]= bcm56450_speed[cfg_num][26-1];
    mxqspeeds[8][0]= bcm56450_speed[cfg_num][27-1];
    mxqspeeds[9][0]= bcm56450_speed[cfg_num][28-1];

    return SOC_E_NONE;
}
soc_error_t 
soc_katana2_port_enable_set(int unit, soc_port_t port, int enable)
{
   soc_field_t port_enable_field[KT2_MAX_MXQPORTS_PER_BLOCK]=
                {PORT0f, PORT1f, PORT2f , PORT3f};
   uint32 rval ;
   uint8 mxqblock ;
   uint8 loop ;
   uint32 entry[SOC_MAX_MEM_WORDS];
   soc_info_t *si= &SOC_INFO(unit);
   uint32 lp_enable = 0, port_enable = 0;
   uint32 regval=0;

   SOC_IF_ERROR_RETURN(soc_katana2_get_port_mxqblock(unit,port,&mxqblock));
   if (!SOC_PBMP_MEMBER((PBMP_ALL(unit)), port)) {
       return SOC_E_PORT; 
   }
   for(loop=0;loop<KT2_MAX_MXQPORTS_PER_BLOCK; loop++) {
       if (kt2_mxqblock_ports[mxqblock][loop] == port) {
           break;
       } 
   }
   if (loop == KT2_MAX_MXQPORTS_PER_BLOCK) {
       return SOC_E_PORT; 
   }
   if (SOC_REG_PORT_VALID(unit, TXLP_PORT_ENABLEr, port)) {
       SOC_IF_ERROR_RETURN(READ_TXLP_PORT_ENABLEr(unit, port, &regval));
       port_enable = soc_reg_field_get(unit, TXLP_PORT_ENABLEr, 
                                       regval, PORT_ENABLEf);
       port_enable |= (1 << loop);
       soc_reg_field_set(unit, TXLP_PORT_ENABLEr, &regval, 
                         PORT_ENABLEf,port_enable);
       if (SOC_PBMP_MEMBER(si->linkphy_pbm, port)) {
           lp_enable = soc_reg_field_get(unit, TXLP_PORT_ENABLEr, 
                             regval, LP_ENABLEf);
           lp_enable |= (1 << loop);
           soc_reg_field_set(unit, TXLP_PORT_ENABLEr, &regval, 
                             LP_ENABLEf,lp_enable);
       }
       soc_cm_debug(DK_VERBOSE,"TXLP port_enable=%d lp_enable=%d\n",
                    port_enable,lp_enable);
       SOC_IF_ERROR_RETURN(WRITE_TXLP_PORT_ENABLEr(unit, port, regval));
   }
   if (SOC_REG_PORT_VALID(unit, RXLP_PORT_ENABLEr, port)) {
       SOC_IF_ERROR_RETURN(READ_RXLP_PORT_ENABLEr(unit, port, &regval));
       if (SOC_PBMP_MEMBER(si->linkphy_pbm, port)) {
           lp_enable = soc_reg_field_get(unit, RXLP_PORT_ENABLEr, 
                             regval, LP_ENABLEf);
           lp_enable |= (1 << loop);
           soc_reg_field_set(unit, RXLP_PORT_ENABLEr, &regval, 
                             LP_ENABLEf,lp_enable);
       }
       soc_cm_debug(DK_VERBOSE,"RXLP lp_enable=%d\n",lp_enable);
       SOC_IF_ERROR_RETURN(WRITE_RXLP_PORT_ENABLEr(unit, port, regval));
   }
   sal_memset(entry, 0, sizeof(egr_enable_entry_t));
   SOC_IF_ERROR_RETURN(READ_XPORT_PORT_ENABLEr(unit, port, &rval));
   if ((port == KT2_OLP_PORT) && (si->olp_port == 1) ) {
        
        loop = 0; /* Port 0 Field */
   }
   if (enable) {
       soc_reg_field_set(unit, XPORT_PORT_ENABLEr, &rval,
                         port_enable_field[loop], 1); /* Enable port */
       soc_mem_field32_set(unit, EGR_ENABLEm, entry, PRT_ENABLEf, 1);
       SOC_IF_ERROR_RETURN(WRITE_XPORT_PORT_ENABLEr(unit, port, rval));
        SOC_IF_ERROR_RETURN
            (WRITE_EGR_ENABLEm(unit, MEM_BLOCK_ALL, port, entry));

   } else {
       soc_reg_field_set(unit, XPORT_PORT_ENABLEr, &rval,
                         port_enable_field[loop], 0); /* Disable port */
       soc_mem_field32_set(unit, EGR_ENABLEm, entry, PRT_ENABLEf, 0);
        SOC_IF_ERROR_RETURN
            (WRITE_EGR_ENABLEm(unit, MEM_BLOCK_ALL, port, entry));
       SOC_IF_ERROR_RETURN(WRITE_XPORT_PORT_ENABLEr(unit, port, rval));
   }

   return SOC_E_NONE;
}
void soc_katana2_mxqblock_reset(int unit, uint8 mxqblock,int active_low)
{
    uint32 rval;
    soc_field_t hot_swap_reset_fld[]={
                TOP_MXQ0_HOTSWAP_RST_Lf,TOP_MXQ1_HOTSWAP_RST_Lf,
                TOP_MXQ2_HOTSWAP_RST_Lf,TOP_MXQ3_HOTSWAP_RST_Lf,
                TOP_MXQ4_HOTSWAP_RST_Lf,TOP_MXQ5_HOTSWAP_RST_Lf,
                TOP_MXQ6_HOTSWAP_RST_Lf,TOP_MXQ7_HOTSWAP_RST_Lf,
                TOP_MXQ8_HOTSWAP_RST_Lf,TOP_MXQ9_HOTSWAP_RST_Lf};

    READ_TOP_SOFT_RESET_REGr(unit, &rval);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, 
                      hot_swap_reset_fld[mxqblock],active_low);
    WRITE_TOP_SOFT_RESET_REGr(unit, rval);

}
soc_error_t 
soc_katana2_reconfigure_tdm(int unit,uint32 new_tdm_size,uint32 *new_tdm)
{
    uint8                new_tdm_no=0;
    soc_mem_t            tdm_table[]={IARB_TDM_TABLEm , IARB_TDM_TABLE_1m};
    soc_field_t          wrap_ptr[]={TDM_WRAP_PTRf , TDM_1_WRAP_PTRf};
    lls_port_tdm_entry_t lls_tdm[128]={{{0}}};
    iarb_tdm_table_entry_t iarb_tdm[256]={{{0}}};
    int                  cfg_num=0; 
    int                  iter=0; 

    uint8                index;
    uint32               rval;

    if (new_tdm_size > 108) {
        return SOC_E_PARAM;
    }
    /*
    if (sal_memcmp(&kt2_current_tdm[0],&new_tdm[0],
                   sizeof(kt2_current_tdm[0])) == 0) {
        return SOC_E_NONE;
    }
    */
    cfg_num = soc_property_get(unit, spn_BCM5645X_CONFIG, 0);

    SOC_IF_ERROR_RETURN(READ_IARB_TDM_CONTROLr(unit, &rval));
    old_tdm_no = soc_reg_field_get(unit, IARB_TDM_CONTROLr, rval, SELECT_TDMf);
    new_tdm_no = (old_tdm_no + 1)%2;

#if 0
    /* Disable IARB TDM before programming... */
    SOC_IF_ERROR_RETURN(READ_IARB_TDM_CONTROLr(unit, &rval));
    soc_reg_field_set(unit, IARB_TDM_CONTROLr, &rval, DISABLEf, 1);
    SOC_IF_ERROR_RETURN(WRITE_IARB_TDM_CONTROLr(unit, rval));
#endif

    for (index = 0; index < new_tdm_size; index++) {
        soc_IARB_TDM_TABLEm_field32_set(unit, &iarb_tdm[0], PORT_NUMf,
                                        new_tdm[index]);
    }
    soc_mem_write_range(unit, tdm_table[new_tdm_no], MEM_BLOCK_ANY, 0,
                        new_tdm_size, new_tdm);

    for (index = 0; index < new_tdm_size/2; index++) {
         soc_LLS_PORT_TDMm_field32_set(unit, &lls_tdm[index], 
                                       PORT_ID_0f, new_tdm[index*2]);
         if ( new_tdm[index*2] <= 41) {
              soc_LLS_PORT_TDMm_field32_set(unit, &lls_tdm[index], 
                                            PORT_ID_0_ENABLEf, 1);
         } else {
              soc_cm_debug(DK_VERBOSE,"IDLE SLOTs so PORT_ID_0_ENABLEf=0 for index=%d\n",
                           index);
              soc_LLS_PORT_TDMm_field32_set(unit, &lls_tdm[index], 
                                            PORT_ID_0_ENABLEf, 0);
         }
         soc_LLS_PORT_TDMm_field32_set(unit, &lls_tdm[index],     
                                       PORT_ID_1f, new_tdm[(index*2) + 1]);
         if ( new_tdm[(index*2)+1] <= 41) {
              soc_LLS_PORT_TDMm_field32_set(unit, &lls_tdm[index],   
                                            PORT_ID_1_ENABLEf, 1);
         } else {
              soc_cm_debug(DK_VERBOSE,
                           "IDLE SLOTs so PORT_ID_1_ENABLEf=0 for index=%d\n",
                           index);
              soc_LLS_PORT_TDMm_field32_set(unit, &lls_tdm[index],   
                                            PORT_ID_1_ENABLEf, 0);
         }
    }
    soc_mem_write_range(unit, LLS_PORT_TDMm, MEM_BLOCK_ANY,
                        new_tdm_no*128, (new_tdm_no*128) + new_tdm_size/2, &lls_tdm);


    READ_IARB_TDM_CONTROLr(unit, &rval);
    soc_reg_field_set(unit, IARB_TDM_CONTROLr, &rval, DISABLEf, 0);
    soc_reg_field_set(unit, IARB_TDM_CONTROLr, &rval, 
                      wrap_ptr[new_tdm_no],new_tdm_size -1);
    soc_reg_field_set(unit, IARB_TDM_CONTROLr, &rval, SELECT_TDMf, new_tdm_no);
    WRITE_IARB_TDM_CONTROLr(unit, rval);
    if (soc_cm_debug_check(DK_VERBOSE)) {
        kt2_tdm_display(unit,new_tdm,new_tdm_size,
                        bcm56450_tdm_info[cfg_num].row,
                        bcm56450_tdm_info[cfg_num].col);
    }
    SOC_IF_ERROR_RETURN(WRITE_LLS_TDM_CAL_CFG_SWITCHr(unit, 1));
    if ((SAL_BOOT_BCMSIM || SAL_BOOT_PLISIM)) {
        SOC_IF_ERROR_RETURN(READ_LLS_TDM_CAL_CFGr(unit, &rval));
        soc_reg_field_set(unit, LLS_TDM_CAL_CFGr, &rval,
                          CURRENT_CALENDARf,new_tdm_no);
        SOC_IF_ERROR_RETURN(WRITE_LLS_TDM_CAL_CFGr(unit, rval));
    }
    do {
        SOC_IF_ERROR_RETURN(READ_LLS_TDM_CAL_CFGr(unit, &rval));
        sal_udelay(100);
        if (soc_reg_field_get(unit, LLS_TDM_CAL_CFGr, rval, 
                              CURRENT_CALENDARf) == new_tdm_no) {
            break;
        }
    } while (iter++ < 100000);
    if (iter >= 100000) {
        soc_cm_debug(DK_ERR, "LLS Calendar switch failed !!\n");
        return SOC_E_INTERNAL;
    }
    old_tdm_no = new_tdm_no;
#if 1
    soc_katana2_save_tdm_pos(unit, new_tdm_size,new_tdm);
#endif

    return SOC_E_NONE;
}
static 
void soc_katana2_save_tdm_pos(int unit, uint8 new_tdm_size,uint32 *new_tdm)
{
    uint8 pos=0;
    uint8 port=0;
    uint8 mxqblock=0;
    uint8 total_slots=0;
    sal_memset(kt2_tdm_pos_info,0,sizeof(kt2_tdm_pos_info));
    for (pos=0;pos < new_tdm_size ; pos++) {
         port= new_tdm[pos];
         if ((port == KT2_IDLE) || (port == KT2_IDLE1) || 
             (port == KT2_LPBK_PORT) || (port == KT2_CMIC_PORT) /* ||
             (port == KT2_OLP_PORT) */) { /* QUICKTURN:ATTN */
              continue;
         }
         mxqblock = kt2_port_to_mxqblock[port-1];
         total_slots = kt2_tdm_pos_info[mxqblock].total_slots;
         kt2_tdm_pos_info[mxqblock].pos[total_slots]=pos;
         kt2_tdm_pos_info[mxqblock].total_slots++;
    }
    sal_memcpy(kt2_current_tdm,new_tdm,new_tdm_size*sizeof(new_tdm[0]));
    old_tdm_no=0;
    kt2_current_tdm_size=new_tdm_size;
}

static soc_error_t 
_soc_katana2_tdm_feasibility_check(int unit, soc_port_t port, int speed,
                                   uint8 *mxqblock, uint8 *sub_port,
                                   uint8 *min_tdm_cycles)
{
    uint8  mxqblock_local=0;
    uint8  sub_port_local=0;
    uint32 speed_index=0;
    soc_field_t wc_xfi_mode_sel_fld[]={WC0_8_XFI_MODE_SELf,WC1_8_XFI_MODE_SELf};
    uint32      wc_8_xfi_mode_sel_val = 0;
    uint32      top_misc_control_1_val = 0;
    if (kt2_tdm_update_flag == 0) {
        return SOC_E_NONE;
    }
    SOC_IF_ERROR_RETURN(soc_katana2_get_port_mxqblock(unit,port,
                                                      &mxqblock_local));
    sub_port_local = kt2_port_to_mxqblock_subports[port-1]; 
    switch(speed) {
    case 1000:  speed_index=0;
                break;
    case 2500:  speed_index=2;
                break;
    case 10000: speed_index=3;
                break;
    case 13000: speed_index=4;
                break;
    case 21000: speed_index=6;
                break;
    default:    soc_cm_print("Check:Invalid Speed:%d \n",speed);
                return SOC_E_CONFIG;
    }
    if (kt2_current_tdm_cycles_info[speed_index].min_tdm_cycles == 0) {
        soc_cm_print("Check:Invalid Minimum TDM Cycles=0 \n");
        return SOC_E_CONFIG;
    }
    if (kt2_tdm_pos_info[mxqblock_local].total_slots == 0) {
        if (((mxqblock_local == 8) || (mxqblock_local == 9)) &&
            ((sub_port_local == 1) || (sub_port_local == 3))) {
             /*Need To Check XFI Mode then */
             SOC_IF_ERROR_RETURN(READ_TOP_MISC_CONTROL_1r(
                                     unit,&top_misc_control_1_val));
             wc_8_xfi_mode_sel_val = soc_reg_field_get(
                                     unit, TOP_MISC_CONTROL_1r,
                                     top_misc_control_1_val,
                                     wc_xfi_mode_sel_fld[mxqblock_local-8]);
             if (wc_8_xfi_mode_sel_val == 0) {
                 soc_cm_print("Check:Total Slots in corresponding"
                              " MXQBlock=%d is zero \n", mxqblock_local);
                 return SOC_E_CONFIG;
             }
             mxqblock_local -= 2;
             sub_port_local  -= 1;
             if (kt2_tdm_pos_info[mxqblock_local].total_slots ==0) {
                 soc_cm_print("Check:Total Slots in corresponding"
                              " MXQBlock=%d is zero \n", mxqblock_local);
                 return SOC_E_CONFIG;
             }
        } else {
            soc_cm_print("Check: Total Slots in corresponding MXQBlock=%d"
                         " is zero \n", mxqblock_local);
            return SOC_E_CONFIG;
        }
    }
    if(mxqblock != NULL) {
       *mxqblock = mxqblock_local;
    }
    if(sub_port != NULL) {
       *sub_port = sub_port_local;
    }
    if(min_tdm_cycles != NULL) {
       *min_tdm_cycles = kt2_current_tdm_cycles_info[speed_index].
                         min_tdm_cycles; 
    }
    return SOC_E_NONE;
}
soc_error_t 
soc_katana2_tdm_feasibility_check(int unit, soc_port_t port, int speed)
{
    return _soc_katana2_tdm_feasibility_check(unit, port, speed,NULL,NULL,NULL);
}
soc_error_t 
soc_katana2_update_tdm(int unit, soc_port_t port, int speed)
{
    uint8  mxqblock=0;
    uint8  sub_port=0;
    uint8  min_tdm_cycles=0;
    uint8  available_tdm_slots=0;
    uint32 new_tdm[256];
    uint8  spacing=0;
    uint8  loop=0;
    uint8  pos=0;
    if (kt2_tdm_update_flag == 0) {
        return SOC_E_NONE;
    }
    SOC_IF_ERROR_RETURN(_soc_katana2_tdm_feasibility_check(
                        unit, port, speed, 
                        &mxqblock , &sub_port,&min_tdm_cycles));
    available_tdm_slots=kt2_tdm_pos_info[mxqblock].total_slots;
    sal_memcpy(&new_tdm[0],&kt2_current_tdm[0],
               kt2_current_tdm_size *sizeof(kt2_current_tdm[0]));
    spacing = available_tdm_slots / min_tdm_cycles;
    for (loop=0; loop < available_tdm_slots; loop++) {
         pos = kt2_tdm_pos_info[mxqblock].pos[loop];
         if (loop % spacing == 0) {
             new_tdm[pos]= port;
         } else {
             new_tdm[pos]= KT2_IDLE;
         }
    }
    soc_katana2_reconfigure_tdm(unit,kt2_current_tdm_size,new_tdm); 
    sal_memcpy(kt2_current_tdm,new_tdm,
               kt2_current_tdm_size*sizeof(new_tdm[0]));
    return SOC_E_NONE;
}
soc_error_t soc_katana2_get_port_mxqblock(
            int unit, soc_port_t port,uint8 *mxqblock)
{
   if (!((port >=1) && (port <= 40))) {
       return SOC_E_PARAM;
   }
   *mxqblock = kt2_port_to_mxqblock[port-1];
   return SOC_E_NONE; 
}
soc_error_t soc_katana2_get_core_port_mode(
            int unit,soc_port_t port,bcmMxqCorePortMode_t *mode)
{
   uint8      mxqblock = 0;
   uint8      loop =0;
   uint8      mxqport_used = 0;

   SOC_IF_ERROR_RETURN(soc_katana2_get_port_mxqblock(unit,port,&mxqblock));
   for(loop=0;loop<KT2_MAX_MXQPORTS_PER_BLOCK; loop++) {
       if (SOC_PBMP_MEMBER((PBMP_ALL(unit)), 
                            kt2_mxqblock_ports[mxqblock][loop])) {
           mxqport_used++;
       } 
   }
   switch(mxqport_used){
   case 0: return SOC_E_PARAM;
   case 1: *mode=bcmMxqCorePortModeSingle; /* 0=SINGLE */
           break; 
   case 2: *mode=bcmMxqCorePortModeDual;   /* 1=DUAL   */
           break; 
   case 3: 
   case 4: *mode=bcmMxqCorePortModeQuad;   /* 2=QUAD   */
           break;
   }
   return SOC_E_NONE; 
}
soc_error_t 
soc_katana2_get_phy_connection_mode(int unit,soc_port_t port,int mxqblock,
                                    bcmMxqConnection_t *connection)
{
   /* port is not being used..Might remove it later */

   soc_field_t wc_xfi_mode_sel_fld[]={WC0_8_XFI_MODE_SELf,WC1_8_XFI_MODE_SELf};
   uint32      wc_8_xfi_mode_sel_val = 0;
   uint32      top_misc_control_1_val = 0;


   if (mxqblock <= 5) {   /* MXQPORT is connected to Unicore */
       *connection=bcmMqxConnectionUniCore;
       return SOC_E_NONE; 
   } 
   if ((mxqblock >= 6) && (mxqblock <= 7)) {
       /*Need To Check XFI Mode then */
       SOC_IF_ERROR_RETURN(READ_TOP_MISC_CONTROL_1r(
                                             unit,&top_misc_control_1_val));
       wc_8_xfi_mode_sel_val = soc_reg_field_get(
                                       unit, TOP_MISC_CONTROL_1r,
                                       top_misc_control_1_val,
                                       wc_xfi_mode_sel_fld[mxqblock-6]);
       if (wc_8_xfi_mode_sel_val) {
           *connection=bcmMqxConnectionWarpCore;
       } else {     
           *connection=bcmMqxConnectionUniCore;
       }
       return SOC_E_NONE; 
   }
   if ((mxqblock >= 8) && (mxqblock <= 9)) {/*MXQPORT is connected to Warpcore*/
       *connection=bcmMqxConnectionWarpCore;
       return SOC_E_NONE; 
   } 
   return SOC_E_PARAM; 
}
soc_error_t soc_katana2_get_phy_port_mode(
            int unit, soc_port_t port,int speed, bcmMxqPhyPortMode_t *mode)
{
   int mxqblock=0;
   bcmMxqConnection_t connection;
   soc_info_t *si=&SOC_INFO(unit);

   if (!((port >=1) && (port <= 40))) {
       return SOC_E_PARAM;
   }
   if (!SOC_PBMP_MEMBER((PBMP_ALL(unit)), port)) {
       return SOC_E_PARAM;
   }
   si = &SOC_INFO(unit);
   /* If speed is less than 2.5G, irrespective of unicore,warpcore,phy_mode 
      will be zero */
   if (speed <= 2500) {
       *mode=bcmMxqPhyPortModeQuad;
       return SOC_E_NONE; 
   }
   mxqblock = kt2_port_to_mxqblock[port-1];
   SOC_IF_ERROR_RETURN(soc_katana2_get_phy_connection_mode(
                       unit,port,mxqblock,&connection));
   switch(connection) {
   case bcmMqxConnectionUniCore:
        if ((speed == 10000) || (speed==12000) || (speed==13000)) {
             *mode=bcmMxqPhyPortModeSingle;
             return SOC_E_NONE; 
        }
        break;
   case bcmMqxConnectionWarpCore:
        if (SOC_IS_KATANA2(unit) && SAL_BOOT_QUICKTURN) {
                if ((speed == 10000)) {
                     if (SOC_PBMP_MEMBER(si->linkphy_pbm, port)) { 
                         soc_cm_print("QUICKTURN:ATTN:LinkPhyPortDualMode");
                     } else {
                         soc_cm_print("QUICKTURN:ATTN:NonLinkPhyPortDualMode");
                     }
                     *mode=bcmMxqPhyPortModeDual;
                     return SOC_E_NONE; 
                }
            /* } */
        }
        if ((speed == 12000) || (speed==13000) || (speed==16000) ||
            (speed == 20000) || (speed==21000)) {
             *mode=bcmMxqPhyPortModeSingle;
             return SOC_E_NONE; 
        }
        if ((speed == 10000)) {
             *mode=bcmMxqPhyPortModeQuad;
             return SOC_E_NONE; 
        }
        break;
   }
   return SOC_E_CONFIG; 
}
void 
soc_katana2_pbmp_init(int unit, kt2_pbmp_t kt2_pbmp)
{
    int port;
    soc_port_details_t  *kt2_port_details[]={
                        /* Below 5 might be deprecated soon */
                        kt2_port_details_deprecated0,
                        kt2_port_details_deprecated1,
                        kt2_port_details_deprecated2,
                        kt2_port_details_deprecated3,
                        kt2_port_details_deprecated4,
                        kt2_port_details_deprecated5,
                        /* New Configuration */
                        kt2_port_details_config1,  kt2_port_details_config2,
                        kt2_port_details_config3,  kt2_port_details_config4,
                        kt2_port_details_config5,  kt2_port_details_config6,
                        kt2_port_details_config7,  kt2_port_details_config8,
                        kt2_port_details_config9,  kt2_port_details_config10,
                        kt2_port_details_config11, kt2_port_details_config12,
                        kt2_port_details_config13, kt2_port_details_config14,
                        kt2_port_details_config15, kt2_port_details_config16,
                        kt2_port_details_config17, kt2_port_details_config18,
                        kt2_port_details_config19, kt2_port_details_config20
                        };
    soc_port_details_t  *kt2_selected_port_details=NULL;
    uint32              kt2_port_details_index=0;
    uint32              loop_index=0;
    uint8               kt2_port_used_flags[40]={0};

    SOC_PBMP_CLEAR(*kt2_pbmp.pbmp_gport_stack);
    SOC_PBMP_CLEAR(*kt2_pbmp.pbmp_mxq);
    SOC_PBMP_CLEAR(*kt2_pbmp.pbmp_mxq1g);
    SOC_PBMP_CLEAR(*kt2_pbmp.pbmp_mxq2p5g);
    SOC_PBMP_CLEAR(*kt2_pbmp.pbmp_mxq10g);
    SOC_PBMP_CLEAR(*kt2_pbmp.pbmp_mxq13g);
    SOC_PBMP_CLEAR(*kt2_pbmp.pbmp_mxq21g);
    SOC_PBMP_CLEAR(*kt2_pbmp.pbmp_xport_xe);
    /* SOC_PBMP_CLEAR(*kt2_pbmp.pbmp_valid); */
    SOC_PBMP_CLEAR(*kt2_pbmp.pbmp_pp);
    SOC_PBMP_CLEAR(*kt2_pbmp.pbmp_linkphy);
    kt2_port_details_index=soc_property_get(unit, spn_BCM5645X_CONFIG,
                               KT2_DEFAULT_PORT_CONFIG);
    kt2_selected_port_details= kt2_port_details[kt2_port_details_index];
    loop_index=0;
    while(kt2_selected_port_details[loop_index].port_speed != 0) {
          soc_cm_debug(DK_VERBOSE,"start:%d end:%d incr:%d type:%d speed:%d\n",
                       kt2_selected_port_details[loop_index].start_port_no,
                       kt2_selected_port_details[loop_index].end_port_no,
                       kt2_selected_port_details[loop_index].port_incr,
                       kt2_selected_port_details[loop_index].port_type,
                       kt2_selected_port_details[loop_index].port_speed);
          for (port =  kt2_selected_port_details[loop_index].start_port_no;
               port <= kt2_selected_port_details[loop_index].end_port_no;
               port += kt2_selected_port_details[loop_index].port_incr) {
               if (kt2_selected_port_details[loop_index].port_type &
                     GE_PORT) {
                     switch(kt2_selected_port_details[loop_index].port_speed) {
                     case 1000: SOC_PBMP_PORT_ADD(*kt2_pbmp.pbmp_mxq1g, port); break;
                     case 2500: SOC_PBMP_PORT_ADD(*kt2_pbmp.pbmp_mxq2p5g, port); break;
                     default:break;/*error */
                     }
               }
               if (kt2_selected_port_details[loop_index].port_type & XE_PORT) {
                   SOC_PBMP_PORT_ADD(*kt2_pbmp.pbmp_mxq10g, port);
                   SOC_PBMP_PORT_ADD(*kt2_pbmp.pbmp_xport_xe, port);
               }
               if (kt2_selected_port_details[loop_index].port_type & HG_PORT) {
                   switch(kt2_selected_port_details[loop_index].port_speed) {
                   case 10000: SOC_PBMP_PORT_ADD(*kt2_pbmp.pbmp_mxq10g, port);break;
                   case 13000: SOC_PBMP_PORT_ADD(*kt2_pbmp.pbmp_mxq13g, port);break;
                   case 20000:
                   case 21000: SOC_PBMP_PORT_ADD(*kt2_pbmp.pbmp_mxq21g, port);break;
                   default:break;/*error */
                   }
               }
               if (kt2_selected_port_details[loop_index].port_type & HGL_PORT) {
                   SOC_PBMP_PORT_ADD(*kt2_pbmp.pbmp_mxq2p5g, port);
                   SOC_PBMP_PORT_ADD(*kt2_pbmp.pbmp_gport_stack, port);
               }
               if (kt2_selected_port_details[loop_index].port_type & STK_PORT) {
                   SOC_PBMP_PORT_ADD(*kt2_pbmp.pbmp_gport_stack, port);
               }
               /* Currently not taking action on CES,OLP */
               if (kt2_selected_port_details[loop_index].port_type &
                   LPHY_PORT) {
                   SOC_PBMP_PORT_ADD(*kt2_pbmp.pbmp_linkphy, port);
               }
               if (kt2_selected_port_details[loop_index].port_type & OLP_PORT) {
                   SOC_INFO(unit).olp_port = 1;
                   SOC_PBMP_PORT_ADD(*kt2_pbmp.pbmp_mxq2p5g, port); 
               }
               SOC_PBMP_PORT_ADD(*kt2_pbmp.pbmp_mxq, port);
               kt2_port_used_flags[port-1]=1;
          }
          loop_index++;
      }
      for (port = 1; port <= 40; port++) {
           if (kt2_port_used_flags[port-1] == 0) {
               SOC_PBMP_PORT_REMOVE(*kt2_pbmp.pbmp_valid, port);
           }
      }
      for (port = 0; port < 170; port++) {
           SOC_PBMP_PORT_ADD(*kt2_pbmp.pbmp_pp, port);
      }
}

void
soc_katana2_subport_init(int unit)
{
    soc_port_t port;
    soc_pbmp_t pbmp_subport, pbmp_linkphy;
    soc_info_t *si=&SOC_INFO(unit);

    SOC_PBMP_CLEAR(pbmp_subport);
    SOC_PBMP_CLEAR(pbmp_linkphy);
    SOC_PBMP_CLEAR(si->linkphy_pbm);
    SOC_PBMP_CLEAR(si->lp.bitmap);
    SOC_PBMP_CLEAR(si->subtag_pbm);

    pbmp_subport = soc_property_get_pbmp(unit, spn_PBMP_SUBPORT, 0);
    pbmp_linkphy = soc_property_get_pbmp(unit, spn_PBMP_LINKPHY, 0);

    SOC_PBMP_ASSIGN(si->subtag_pbm, pbmp_subport);

    if (SOC_PBMP_NOT_NULL(pbmp_subport)) {
        /* All members of pbmp_linkphy should also be
         *  member of pbmp_subport */
        if (SOC_PBMP_NOT_NULL(pbmp_linkphy)) {
            /* Check if pbmp_linkphy members are valid for
             * supporting LinkPHY.
             * For Katana2(BCM56450) port 27 to 34 only support LinkPHY */
            SOC_PBMP_ITER(pbmp_linkphy, port) {
                if (!SOC_REG_PORT_VALID(unit, RXLP_PORT_ENABLEr, port)) {
                    SOC_ERROR_PRINT((DK_ERR, "\nCONFIG ERROR\n"
                                "pbmp_linkphy member port %d is invalid for "
                                "LinkPHY support\n\n", port));
                    SOC_PBMP_CLEAR(si->linkphy_pbm);
                    SOC_PBMP_CLEAR(si->lp.bitmap);
                    SOC_PBMP_CLEAR(si->subtag_pbm);
                    break;
                }
                SOC_PBMP_PORT_ADD(si->linkphy_pbm, port);
                SOC_PBMP_PORT_ADD(si->lp.bitmap, port);
            }

            SOC_PBMP_ITER(pbmp_linkphy, port) {
                if (!SOC_PBMP_MEMBER(pbmp_subport, port)) {
                    SOC_ERROR_PRINT((DK_ERR, "\nCONFIG ERROR\n"
                        "pbmp_linkphy member port %d is not member of "
                        "pbmp_subport\n\n", port));
                    SOC_PBMP_CLEAR(si->linkphy_pbm);
                    SOC_PBMP_CLEAR(si->lp.bitmap);
                    SOC_PBMP_CLEAR(si->subtag_pbm);
                    break;
                }
                SOC_PBMP_PORT_REMOVE(si->subtag_pbm, port);
            }
        }
    } else {
        if (SOC_PBMP_NOT_NULL(pbmp_linkphy)) {
            SOC_ERROR_PRINT((DK_ERR, "\nCONFIG ERROR\n"
                "config variable pbmp_linkphy should be sub-set of "
                "config variable pbmp_subport\n\n"));
        }
    }
}

void
_soc_katana2_ci_init(int unit)
{
    uint32 rval;
    soc_cm_print("QUICKTURN:ATTN %s:%d \n",__FUNCTION__,  __LINE__);

WRITE_CI_CONFIG1r(unit, 0, 0xf0dcf0dc);
WRITE_CI_CONFIG1r(unit,.0,  0xf0dcf0dc);
WRITE_CI_CONFIG1r(unit,.1,  0xf0dcf0dc);
WRITE_CI_CONFIG1r(unit,.1,  0xf0dcf0dc);
WRITE_CI_CONFIG1r(unit,.2,  0xf0dcf0dc);
WRITE_CI_CONFIG1r(unit,.2,  0xf0dcf0dc);
WRITE_CI_CONFIG1r(unit,.3,  0xf0dcf0dc);
WRITE_CI_CONFIG1r(unit,.3,  0xf0dcf0dc);
WRITE_CI_CONFIG1r(unit,.4,  0xf0dcf0dc);
WRITE_CI_CONFIG1r(unit,.4,  0xf0dcf0dc);
WRITE_CI_CONFIG1r(unit,.5,  0xf0dcf0dc);
WRITE_CI_CONFIG1r(unit,.5,  0xf0dcf0dc);
WRITE_CI_RESETr(unit,.0,  0x00000004);
WRITE_CI_RESETr(unit,.0,  0x00000002);
WRITE_CI_RESETr(unit,.1,  0x00000004);
WRITE_CI_RESETr(unit,.1,  0x00000002);
WRITE_CI_RESETr(unit,.2,  0x00000004);
WRITE_CI_RESETr(unit,.2,  0x00000002);
WRITE_CI_RESETr(unit,.3,  0x00000004);
WRITE_CI_RESETr(unit,.3,  0x00000002);
WRITE_CI_RESETr(unit,.4,  0x00000004);
WRITE_CI_RESETr(unit,.4,  0x00000002);
WRITE_CI_RESETr(unit,.5,  0x00000004);
WRITE_CI_RESETr(unit,.5,  0x00000002);
WRITE_CI_CONFIG0r(unit,.0,  0x0324ab03);
WRITE_CI_CONFIG0r(unit,.0,  0x0324ab03);
WRITE_CI_CONFIG0r(unit,.1,  0x0324aa03);
WRITE_CI_CONFIG0r(unit,.1,  0x0324aa03);
WRITE_CI_CONFIG0r(unit,.2,  0x0324ab03);
WRITE_CI_CONFIG0r(unit,.2,  0x0324ab03);
WRITE_CI_CONFIG0r(unit,.3,  0x0324aa03);
WRITE_CI_CONFIG0r(unit,.3,  0x0324ab03);
WRITE_CI_CONFIG0r(unit,.4,  0x0324ab03);
WRITE_CI_CONFIG0r(unit,.4,  0x0324ab03);
WRITE_CI_CONFIG0r(unit,.5,  0x0324aa03);
WRITE_CI_CONFIG0r(unit,.5,  0x0324ab03);
WRITE_CI_CONFIG2r(unit,.0,  0x0d4860f0);
WRITE_CI_CONFIG2r(unit,.0,  0x0d4860f0);
WRITE_CI_CONFIG2r(unit,.1,  0x0c4860f0);
WRITE_CI_CONFIG2r(unit,.1,  0x0c4860f0);
WRITE_CI_CONFIG2r(unit,.2,  0x0d4860f0);
WRITE_CI_CONFIG2r(unit,.2,  0x0d4860f0);
WRITE_CI_CONFIG2r(unit,.3,  0x0c4860f0);
WRITE_CI_CONFIG2r(unit,.3,  0x0d4860f0);
WRITE_CI_CONFIG2r(unit,.4,  0x0d4860f0);
WRITE_CI_CONFIG2r(unit,.4,  0x0d4860f0);
WRITE_CI_CONFIG2r(unit,.5,  0x0c4860f0);
WRITE_CI_CONFIG2r(unit,.5,  0x0d4860f0);
WRITE_CI_CONFIG3r(unit,.0,  0x00003451);
WRITE_CI_CONFIG3r(unit,.0,  0x00003451);
WRITE_CI_CONFIG3r(unit,.1,  0x00003451);
WRITE_CI_CONFIG3r(unit,.1,  0x00003451);
WRITE_CI_CONFIG3r(unit,.2,  0x00003451);
WRITE_CI_CONFIG3r(unit,.2,  0x00003451);
WRITE_CI_CONFIG3r(unit,.3,  0x00003451);
WRITE_CI_CONFIG3r(unit,.3,  0x00003451);
WRITE_CI_CONFIG3r(unit,.4,  0x00003451);
WRITE_CI_CONFIG3r(unit,.4,  0x00003451);
WRITE_CI_CONFIG3r(unit,.5,  0x00003451);
WRITE_CI_CONFIG3r(unit,.5,  0x00003451);
WRITE_CI_CONFIG6r(unit,.0,  0x36000214);
WRITE_CI_CONFIG6r(unit,.0,  0x36000214);
WRITE_CI_CONFIG6r(unit,.1,  0x36000214);
WRITE_CI_CONFIG6r(unit,.1,  0x36000214);
WRITE_CI_CONFIG6r(unit,.2,  0x36000214);
WRITE_CI_CONFIG6r(unit,.2,  0x36000214);
WRITE_CI_CONFIG6r(unit,.3,  0x36000214);
WRITE_CI_CONFIG6r(unit,.3,  0x36000214);
WRITE_CI_CONFIG6r(unit,.4,  0x36000214);
WRITE_CI_CONFIG6r(unit,.4,  0x36000214);
WRITE_CI_CONFIG6r(unit,.5,  0x36000214);
WRITE_CI_CONFIG6r(unit,.5,  0x36000214);
WRITE_CI_PHY_STRAPS1r(unit,.0,  0x0004129b);
WRITE_CI_PHY_STRAPS1r(unit,.0,  0x0004129b);
WRITE_CI_PHY_STRAPS1r(unit,.1,  0x0004129b);
WRITE_CI_PHY_STRAPS1r(unit,.1,  0x0004129b);
WRITE_CI_PHY_STRAPS1r(unit,.2,  0x0004129b);
WRITE_CI_PHY_STRAPS1r(unit,.2,  0x0004129b);
WRITE_CI_PHY_STRAPS1r(unit,.3,  0x0004129b);
WRITE_CI_PHY_STRAPS1r(unit,.3,  0x0004129b);
WRITE_CI_PHY_STRAPS1r(unit,.4,  0x0004129b);
WRITE_CI_PHY_STRAPS1r(unit,.4,  0x0004129b);
WRITE_CI_PHY_STRAPS1r(unit,.5,  0x0004129b);
WRITE_CI_PHY_STRAPS1r(unit,.5,  0x0004129b);
WRITE_CI_RESETr(unit,.0,  0x00000002);
WRITE_CI_RESETr(unit,.0,  0x00000000);
WRITE_CI_RESETr(unit,.1,  0x00000002);
WRITE_CI_RESETr(unit,.1,  0x00000000);
WRITE_CI_RESETr(unit,.2,  0x00000002);
WRITE_CI_RESETr(unit,.2,  0x00000000);
WRITE_CI_RESETr(unit,.3,  0x00000002);
WRITE_CI_RESETr(unit,.3,  0x00000000);
WRITE_CI_RESETr(unit,.4,  0x00000002);
WRITE_CI_RESETr(unit,.4,  0x00000000);
WRITE_CI_RESETr(unit,.5,  0x00000002);
WRITE_CI_RESETr(unit,.5,  0x00000000);
WRITE_CI_DDR_MR0r(unit,.0,  0x00001b60);
WRITE_CI_DDR_MR0r(unit,.0,  0x00001b60);
WRITE_CI_DDR_MR0r(unit,.0,  0x00001b60);
WRITE_CI_DDR_MR1r(unit,.0,  0x00008010);
WRITE_CI_DDR_MR1r(unit,.0,  0x00008010);
WRITE_CI_DDR_MR1r(unit,.0,  0x00008010);
WRITE_CI_DDR_MR2r(unit,.0,  0x00010210);
WRITE_CI_DDR_MR2r(unit,.0,  0x00010210);
WRITE_CI_DDR_MR2r(unit,.0,  0x00010210);
WRITE_CI_DDR_MR0r(unit,.1,  0x00001b60);
WRITE_CI_DDR_MR0r(unit,.1,  0x00001b60);
WRITE_CI_DDR_MR0r(unit,.1,  0x00001b60);
WRITE_CI_DDR_MR1r(unit,.1,  0x00008010);
WRITE_CI_DDR_MR1r(unit,.1,  0x00008010);
WRITE_CI_DDR_MR1r(unit,.1,  0x00008010);
WRITE_CI_DDR_MR2r(unit,.0,  0x00010210);
WRITE_CI_DDR_MR2r(unit,.1,  0x00010210);
WRITE_CI_DDR_MR2r(unit,.1,  0x00010210);
WRITE_CI_DDR_MR0r(unit,.2,  0x00001b60);
WRITE_CI_DDR_MR0r(unit,.2,  0x00001b60);
WRITE_CI_DDR_MR0r(unit,.2,  0x00001b60);
WRITE_CI_DDR_MR1r(unit,.2,  0x00008010);
WRITE_CI_DDR_MR1r(unit,.2,  0x00008010);
WRITE_CI_DDR_MR1r(unit,.2,  0x00008010);
WRITE_CI_DDR_MR2r(unit,.0,  0x00010210);
WRITE_CI_DDR_MR2r(unit,.2,  0x00010210);
WRITE_CI_DDR_MR2r(unit,.2,  0x00010210);
WRITE_CI_DDR_MR0r(unit,.3,  0x00001b60);
WRITE_CI_DDR_MR0r(unit,.3,  0x00001b60);
WRITE_CI_DDR_MR0r(unit,.3,  0x00001b60);
WRITE_CI_DDR_MR1r(unit,.3,  0x00008010);
WRITE_CI_DDR_MR1r(unit,.3,  0x00008010);
WRITE_CI_DDR_MR1r(unit,.3,  0x00008010);
WRITE_CI_DDR_MR2r(unit,.0,  0x00010210);
WRITE_CI_DDR_MR2r(unit,.3,  0x00010210);
WRITE_CI_DDR_MR2r(unit,.3,  0x00010210);
WRITE_CI_DDR_MR0r(unit,.4,  0x00001b60);
WRITE_CI_DDR_MR0r(unit,.4,  0x00001b60);
WRITE_CI_DDR_MR0r(unit,.4,  0x00001b60);
WRITE_CI_DDR_MR1r(unit,.4,  0x00008010);
WRITE_CI_DDR_MR1r(unit,.4,  0x00008010);
WRITE_CI_DDR_MR1r(unit,.4,  0x00008010);
WRITE_CI_DDR_MR2r(unit,.0,  0x00010210);
WRITE_CI_DDR_MR2r(unit,.4,  0x00010210);
WRITE_CI_DDR_MR2r(unit,.4,  0x00010210);
WRITE_CI_DDR_MR0r(unit,.5,  0x00001b60);
WRITE_CI_DDR_MR0r(unit,.5,  0x00001b60);
WRITE_CI_DDR_MR0r(unit,.5,  0x00001b60);
WRITE_CI_DDR_MR1r(unit,.5,  0x00008010);
WRITE_CI_DDR_MR1r(unit,.5,  0x00008010);
WRITE_CI_DDR_MR1r(unit,.5,  0x00008010);
WRITE_CI_DDR_MR2r(unit,.0,  0x00010210);
WRITE_CI_DDR_MR2r(unit,.5,  0x00010210);
WRITE_CI_DDR_MR2r(unit,.5,  0x00010210);
WRITE_CI_RESETr(unit,.0,  0x00000000);
WRITE_CI_RESETr(unit,.0,  0x00000004);
WRITE_CI_RESETr(unit,.1,  0x00000000);
WRITE_CI_RESETr(unit,.1,  0x00000004);
WRITE_CI_RESETr(unit,.2,  0x00000000);
WRITE_CI_RESETr(unit,.2,  0x00000004);
WRITE_CI_RESETr(unit,.3,  0x00000000);
WRITE_CI_RESETr(unit,.3,  0x00000004);
WRITE_CI_RESETr(unit,.4,  0x00000000);
WRITE_CI_RESETr(unit,.4,  0x00000004);
WRITE_CI_RESETr(unit,.5,  0x00000000);
WRITE_CI_RESETr(unit,.5,  0x00000004);
sal_sleep( 2);
WRITE_CI_PHY_CONTROLr(unit,.0,  0x04a5360f);
WRITE_CI_PHY_CONTROLr(unit,.0,  0x04a5360f);
WRITE_CI_PHY_CONTROLr(unit,.1,  0x0425360f);
WRITE_CI_PHY_CONTROLr(unit,.1,  0x0425360f);
WRITE_CI_PHY_CONTROLr(unit,.2,  0x04a5360f);
WRITE_CI_PHY_CONTROLr(unit,.2,  0x04a5360f);
WRITE_CI_PHY_CONTROLr(unit,.3,  0x0425360f);
WRITE_CI_PHY_CONTROLr(unit,.3,  0x0425360f);
WRITE_CI_PHY_CONTROLr(unit,.4,  0x04a5360f);
WRITE_CI_PHY_CONTROLr(unit,.4,  0x04a5360f);
WRITE_CI_PHY_CONTROLr(unit,.5,  0x0425360f);
WRITE_CI_PHY_CONTROLr(unit,.5,  0x0425360f);
sal_sleep( 2);
WRITE_CI_PHY_CONTROLr(unit,.0,  0x04a5360f);
WRITE_CI_PHY_CONTROLr(unit,.0,  0x04a53603);
WRITE_CI_PHY_CONTROLr(unit,.1,  0x0425360f);
WRITE_CI_PHY_CONTROLr(unit,.1,  0x04253603);
WRITE_CI_PHY_CONTROLr(unit,.2,  0x04a5360f);
WRITE_CI_PHY_CONTROLr(unit,.2,  0x04a53603);
WRITE_CI_PHY_CONTROLr(unit,.3,  0x0425360f);
WRITE_CI_PHY_CONTROLr(unit,.3,  0x04253603);
WRITE_CI_PHY_CONTROLr(unit,.4,  0x04a5360f);
WRITE_CI_PHY_CONTROLr(unit,.4,  0x04a53603);
WRITE_CI_PHY_CONTROLr(unit,.5,  0x0425360f);
WRITE_CI_PHY_CONTROLr(unit,.5,  0x04253603);
sal_sleep( 2);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.0,  0x80000438);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.0,  0xe000001c);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.0,  0xa000001c);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x00001420);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x00001420);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x00001810);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.0,  0xa000001c);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.0,  0xc000001c);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.0,  0x8000001c);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x00001810);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.1,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x00300000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.3,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x00300000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.5,  0x00000000);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.0,  0x8000001c);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.0,  0xe0000014);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.0,  0xa0000014);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x00000012);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x00000012);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x00000002);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.0,  0xa0000014);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.0,  0xc0000014);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.0,  0x80000014);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x00000002);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.1,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x00300000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.3,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x00300000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.5,  0x00000000);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.0,  0x80000014);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.0,  0xe0000014);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.0,  0xa0000014);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x00000002);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x00000002);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x00000002);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.0,  0xa0000014);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.0,  0xc0000014);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.0,  0x80000014);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x00000002);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.1,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x00300000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.3,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x00300000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.5,  0x00000000);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.0,  0x80000014);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.0,  0xe0000014);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.0,  0xa0000014);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x00000002);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x00000002);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x00000002);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.0,  0xa0000014);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.0,  0xc0000014);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.0,  0x80000014);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x00000002);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.1,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x00300000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.3,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x00300000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.5,  0x00000000);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.2,  0x80000438);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.2,  0xe000001c);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.2,  0xa000001c);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x00001420);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x00001420);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x00001810);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.2,  0xa000001c);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.2,  0xc000001c);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.2,  0x8000001c);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x00000002);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.1,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x00001810);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.3,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x00300000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.5,  0x00000000);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.2,  0x8000001c);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.2,  0xe0000014);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.2,  0xa0000014);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x00000012);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x00000012);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x00000002);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.2,  0xa0000014);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.2,  0xc0000014);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.2,  0x80000014);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x00000002);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.1,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x00000002);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.3,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x00300000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.5,  0x00000000);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.2,  0x80000014);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.2,  0xe0000014);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.2,  0xa0000014);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x00000002);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x00000002);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x00000002);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.2,  0xa0000014);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.2,  0xc0000014);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.2,  0x80000014);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x00000002);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.1,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x00000002);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.3,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x00300000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.5,  0x00000000);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.2,  0x80000014);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.2,  0xe0000014);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.2,  0xa0000014);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x00000002);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x00000002);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x00000002);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.2,  0xa0000014);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.2,  0xc0000014);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.2,  0x80000014);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x00000002);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.1,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x00000002);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.3,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x00300000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.5,  0x00000000);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.4,  0x80000438);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.4,  0xe000001c);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.4,  0xa000001c);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x00001420);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x00001420);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x00001810);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.4,  0xa000001c);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.4,  0xc000001c);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.4,  0x8000001c);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x00000002);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.1,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x00000002);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.3,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x00001810);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.5,  0x00000000);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.4,  0x8000001c);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.4,  0xe0000014);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.4,  0xa0000014);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x00000012);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x00000012);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x00000002);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.4,  0xa0000014);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.4,  0xc0000014);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.4,  0x80000014);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x00000002);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.1,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x00000002);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.3,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x00000002);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.5,  0x00000000);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.4,  0x80000014);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.4,  0xe0000014);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.4,  0xa0000014);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x00000002);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x00000002);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x00000002);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.4,  0xa0000014);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.4,  0xc0000014);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.4,  0x80000014);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x00000002);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.1,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x00000002);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.3,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x00000002);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.5,  0x00000000);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.4,  0x80000014);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.4,  0xe0000014);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.4,  0xa0000014);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x00000002);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x00000002);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x00000002);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.4,  0xa0000014);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.4,  0xc0000014);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.4,  0x80000014);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x00000002);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.1,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x00000002);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.3,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x00000002);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.5,  0x00000000);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.0,  0x80000014);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.0,  0xe0000010);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.0,  0xa0000010);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x00a6e001);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.2,  0x80000014);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.2,  0xe0000010);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.2,  0xa0000010);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x00a6e001);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.4,  0x80000014);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.4,  0xe0000010);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.4,  0xa0000010);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x00a6e001);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x00a6e001);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x03f1ffff);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.0,  0xa0000010);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.0,  0xc0000064);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.0,  0x80000064);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x03f1ffff);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.1,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x00a6e001);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.3,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x00a6e001);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.5,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x03f1ffff);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x0001ffff);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.0,  0x80000064);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.0,  0xc0000068);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.0,  0x80000068);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x0001ffff);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.1,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x00a6e001);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.3,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x00a6e001);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.5,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x0001ffff);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x00000004);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.0,  0x80000068);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.0,  0xc000005c);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.0,  0x8000005c);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x00000004);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.1,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x00a6e001);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.3,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x00a6e001);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.5,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x00a6e001);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x03f1ffff);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.2,  0xa0000010);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.2,  0xc0000064);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.2,  0x80000064);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x00000004);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.1,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x03f1ffff);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.3,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x00a6e001);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.5,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x03f1ffff);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x0001ffff);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.2,  0x80000064);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.2,  0xc0000068);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.2,  0x80000068);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x00000004);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.1,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x0001ffff);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.3,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x00a6e001);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.5,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x0001ffff);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x00000004);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.2,  0x80000068);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.2,  0xc000005c);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.2,  0x8000005c);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x00000004);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.1,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x00000004);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.3,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x00a6e001);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.5,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x00a6e001);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x03f1ffff);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.4,  0xa0000010);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.4,  0xc0000064);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.4,  0x80000064);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x00000004);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.1,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x00000004);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.3,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x03f1ffff);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.5,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x03f1ffff);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x0001ffff);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.4,  0x80000064);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.4,  0xc0000068);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.4,  0x80000068);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x00000004);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.1,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x00000004);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.3,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x0001ffff);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.5,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x0001ffff);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x00000004);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.4,  0x80000068);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.4,  0xc000005c);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.4,  0x8000005c);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x00000004);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.1,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x00000004);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.3,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x00000004);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.5,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x00000004);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x00000000);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.0,  0x8000005c);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.0,  0xc000003c);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.0,  0x8000003c);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.1,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x00000004);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.3,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x00000004);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.5,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x00100000);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.0,  0x8000003c);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.0,  0xc000003c);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.0,  0x8000003c);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x00100000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.1,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x00000004);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.3,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x00000004);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.5,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x00000004);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x00000000);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.2,  0x8000005c);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.2,  0xc000003c);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.2,  0x8000003c);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x00100000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.1,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.3,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x00000004);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.5,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x00100000);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.2,  0x8000003c);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.2,  0xc000003c);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.2,  0x8000003c);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x00100000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.1,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x00100000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.3,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x00000004);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.5,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x00000004);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x00000000);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.4,  0x8000005c);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.4,  0xc000003c);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.4,  0x8000003c);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x00100000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.1,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x00100000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.3,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.5,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x00100000);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.4,  0x8000003c);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.4,  0xc000003c);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.4,  0x8000003c);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x00100000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.1,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x00100000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.3,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x00100000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.5,  0x00000000);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.0,  0x8000003c);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.0,  0xe000003c);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.0,  0xa000003c);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x0043f000);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.2,  0x8000003c);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.2,  0xe000003c);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.2,  0xa000003c);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x0043f000);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.4,  0x8000003c);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.4,  0xe000003c);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.4,  0xa000003c);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x0043f000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x0043f000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x0000000a);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.0,  0xa000003c);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.0,  0xc0000040);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.0,  0x80000040);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x0000000a);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.1,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x0043f000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.3,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x0043f000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.5,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x0000000a);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x0000000a);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.0,  0x80000040);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.0,  0xc00003a4);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.0,  0x800003a4);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x0000000a);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.1,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x0043f000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.3,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x0043f000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.5,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x0000000a);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x0000000a);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.0,  0x800003a4);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.0,  0xc00005a4);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.0,  0x800005a4);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x0000000a);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.1,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x0043f000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.3,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x0043f000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.5,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x0043f000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x0000000a);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.2,  0xa000003c);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.2,  0xc0000040);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.2,  0x80000040);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x0000000a);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.1,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x0000000a);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.3,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x0043f000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.5,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x0000000a);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x0000000a);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.2,  0x80000040);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.2,  0xc00003a4);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.2,  0x800003a4);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x0000000a);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.1,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x0000000a);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.3,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x0043f000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.5,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x0000000a);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x0000000a);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.2,  0x800003a4);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.2,  0xc00005a4);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.2,  0x800005a4);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x0000000a);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.1,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x0000000a);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.3,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x0043f000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.5,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x0043f000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x0000000a);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.4,  0xa000003c);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.4,  0xc0000040);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.4,  0x80000040);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x0000000a);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.1,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x0000000a);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.3,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x0000000a);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.5,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x0000000a);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x0000000a);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.4,  0x80000040);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.4,  0xc00003a4);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.4,  0x800003a4);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x0000000a);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.1,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x0000000a);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.3,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x0000000a);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.5,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x0000000a);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x0000000a);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.4,  0x800003a4);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.4,  0xc00005a4);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.4,  0x800005a4);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x0000000a);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.1,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x0000000a);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.3,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x0000000a);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.5,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x0000000a);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x00000003);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.0,  0x800005a4);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.0,  0xc0000360);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.0,  0x80000360);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x00000003);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.1,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x0000000a);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.3,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x0000000a);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.5,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x00000003);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x00000003);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.0,  0x80000360);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.0,  0xc0000560);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.0,  0x80000560);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x00000003);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.1,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x0000000a);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.3,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x0000000a);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.5,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x00000003);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x00000007);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.0,  0x80000560);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.0,  0xc0000364);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.0,  0x80000364);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x00000007);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.1,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x0000000a);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.3,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x0000000a);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.5,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x00000007);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x00000007);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.0,  0x80000364);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.0,  0xc0000564);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.0,  0x80000564);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x00000007);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.1,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x0000000a);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.3,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x0000000a);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.5,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x00000007);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x00000001);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.0,  0x80000564);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.0,  0xc00003ac);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.0,  0x800003ac);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x00000001);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.1,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x0000000a);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.3,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x0000000a);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.5,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x00000001);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x00000001);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.0,  0x800003ac);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.0,  0xc00005ac);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.0,  0x800005ac);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x00000001);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.1,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x0000000a);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.3,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x0000000a);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.5,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x0000000a);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x00000003);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.2,  0x800005a4);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.2,  0xc0000360);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.2,  0x80000360);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x00000001);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.1,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x00000003);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.3,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x0000000a);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.5,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x00000003);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x00000003);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.2,  0x80000360);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.2,  0xc0000560);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.2,  0x80000560);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x00000001);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.1,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x00000003);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.3,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x0000000a);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.5,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x00000003);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x00000007);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.2,  0x80000560);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.2,  0xc0000364);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.2,  0x80000364);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x00000001);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.1,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x00000007);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.3,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x0000000a);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.5,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x00000007);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x00000007);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.2,  0x80000364);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.2,  0xc0000564);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.2,  0x80000564);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x00000001);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.1,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x00000007);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.3,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x0000000a);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.5,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x00000007);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x00000001);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.2,  0x80000564);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.2,  0xc00003ac);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.2,  0x800003ac);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x00000001);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.1,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x00000001);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.3,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x0000000a);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.5,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x00000001);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x00000001);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.2,  0x800003ac);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.2,  0xc00005ac);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.2,  0x800005ac);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x00000001);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.1,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x00000001);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.3,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x0000000a);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.5,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x0000000a);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x00000003);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.4,  0x800005a4);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.4,  0xc0000360);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.4,  0x80000360);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x00000001);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.1,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x00000001);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.3,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x00000003);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.5,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x00000003);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x00000003);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.4,  0x80000360);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.4,  0xc0000560);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.4,  0x80000560);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x00000001);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.1,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x00000001);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.3,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x00000003);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.5,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x00000003);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x00000007);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.4,  0x80000560);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.4,  0xc0000364);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.4,  0x80000364);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x00000001);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.1,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x00000001);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.3,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x00000007);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.5,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x00000007);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x00000007);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.4,  0x80000364);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.4,  0xc0000564);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.4,  0x80000564);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x00000001);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.1,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x00000001);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.3,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x00000007);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.5,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x00000007);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x00000001);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.4,  0x80000564);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.4,  0xc00003ac);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.4,  0x800003ac);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x00000001);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.1,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x00000001);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.3,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x00000001);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.5,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x00000001);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x00000001);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.4,  0x800003ac);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.4,  0xc00005ac);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.4,  0x800005ac);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x00000001);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.1,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x00000001);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.3,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x00000001);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.5,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x00000001);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x00300000);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.0,  0x800005ac);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.0,  0xc0000238);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.0,  0x80000238);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x00300000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.1,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x00000001);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.3,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x00000001);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.5,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x00300000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x00300000);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.0,  0x80000238);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.0,  0xc0000438);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.0,  0x80000438);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x00300000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.1,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x00000001);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.3,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x00000001);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.5,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x00000001);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x00300000);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.2,  0x800005ac);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.2,  0xc0000238);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.2,  0x80000238);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x00300000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.1,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x00300000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.3,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x00000001);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.5,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x00300000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x00300000);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.2,  0x80000238);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.2,  0xc0000438);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.2,  0x80000438);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x00300000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.1,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x00300000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.3,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x00000001);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.5,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x00000001);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x00300000);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.4,  0x800005ac);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.4,  0xc0000238);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.4,  0x80000238);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x00300000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.1,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x00300000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.3,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x00300000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.5,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x00300000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x00300000);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.4,  0x80000238);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.4,  0xc0000438);
WRITE_CI_DDR_PHY_REG_CTRLr(unit,.4,  0x80000438);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.0,  0x00300000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.1,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.2,  0x00300000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.3,  0x00000000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.4,  0x00300000);
WRITE_CI_DDR_PHY_REG_DATAr(unit,.5,  0x00000000);
sal_sleep( 2);
WRITE_CI_PHY_CONTROLr(unit,.0,  0x05a53603);
WRITE_CI_PHY_CONTROLr(unit,.0,  0x05a5360f);
WRITE_CI_PHY_CONTROLr(unit,.1,  0x04253603);
WRITE_CI_PHY_CONTROLr(unit,.1,  0x0425360f);
WRITE_CI_PHY_CONTROLr(unit,.2,  0x05a53603);
WRITE_CI_PHY_CONTROLr(unit,.2,  0x05a5360f);
WRITE_CI_PHY_CONTROLr(unit,.3,  0x04253603);
WRITE_CI_PHY_CONTROLr(unit,.3,  0x0425360f);
WRITE_CI_PHY_CONTROLr(unit,.4,  0x05a53603);
WRITE_CI_PHY_CONTROLr(unit,.4,  0x05a5360f);
WRITE_CI_PHY_CONTROLr(unit,.5,  0x04253603);
WRITE_CI_PHY_CONTROLr(unit,.5,  0x0425360f);
sal_sleep( 2);
WRITE_CI_MRS_CMDr(unit,.0,  0x000000bc);
WRITE_CI_MRS_CMDr(unit,.0,  0x000000ce);
WRITE_CI_MRS_CMDr(unit,.1,  0x000000bc);
WRITE_CI_MRS_CMDr(unit,.1,  0x000000ce);
WRITE_CI_MRS_CMDr(unit,.2,  0x000000bc);
WRITE_CI_MRS_CMDr(unit,.2,  0x000000ce);
WRITE_CI_MRS_CMDr(unit,.3,  0x000000bc);
WRITE_CI_MRS_CMDr(unit,.3,  0x000000ce);
WRITE_CI_MRS_CMDr(unit,.4,  0x000000bc);
WRITE_CI_MRS_CMDr(unit,.4,  0x000000ce);
WRITE_CI_MRS_CMDr(unit,.5,  0x000000bc);
WRITE_CI_MRS_CMDr(unit,.5,  0x000000ce);
sal_sleep( 2);
WRITE_CI_MRS_CMDr(unit,.0,  0x0000008e);
WRITE_CI_MRS_CMDr(unit,.0,  0x000000cf);
WRITE_CI_MRS_CMDr(unit,.1,  0x0000008e);
WRITE_CI_MRS_CMDr(unit,.1,  0x000000cf);
WRITE_CI_MRS_CMDr(unit,.2,  0x0000008e);
WRITE_CI_MRS_CMDr(unit,.2,  0x000000cf);
WRITE_CI_MRS_CMDr(unit,.3,  0x0000008e);
WRITE_CI_MRS_CMDr(unit,.3,  0x000000cf);
WRITE_CI_MRS_CMDr(unit,.4,  0x0000008e);
WRITE_CI_MRS_CMDr(unit,.4,  0x000000cf);
WRITE_CI_MRS_CMDr(unit,.5,  0x0000008e);
WRITE_CI_MRS_CMDr(unit,.5,  0x000000cf);
sal_sleep( 2);
WRITE_CI_MRS_CMDr(unit,.0,  0x0000008f);
WRITE_CI_MRS_CMDr(unit,.0,  0x000000cd);
WRITE_CI_MRS_CMDr(unit,.1,  0x0000008f);
WRITE_CI_MRS_CMDr(unit,.1,  0x000000cd);
WRITE_CI_MRS_CMDr(unit,.2,  0x0000008f);
WRITE_CI_MRS_CMDr(unit,.2,  0x000000cd);
WRITE_CI_MRS_CMDr(unit,.3,  0x0000008f);
WRITE_CI_MRS_CMDr(unit,.3,  0x000000cd);
WRITE_CI_MRS_CMDr(unit,.4,  0x0000008f);
WRITE_CI_MRS_CMDr(unit,.4,  0x000000cd);
WRITE_CI_MRS_CMDr(unit,.5,  0x0000008f);
WRITE_CI_MRS_CMDr(unit,.5,  0x000000cd);
sal_sleep( 2);
WRITE_CI_MRS_CMDr(unit,.0,  0x0000008d);
WRITE_CI_MRS_CMDr(unit,.0,  0x000000cc);
WRITE_CI_MRS_CMDr(unit,.1,  0x0000008d);
WRITE_CI_MRS_CMDr(unit,.1,  0x000000cc);
WRITE_CI_MRS_CMDr(unit,.2,  0x0000008d);
WRITE_CI_MRS_CMDr(unit,.2,  0x000000cc);
WRITE_CI_MRS_CMDr(unit,.3,  0x0000008d);
WRITE_CI_MRS_CMDr(unit,.3,  0x000000cc);
WRITE_CI_MRS_CMDr(unit,.4,  0x0000008d);
WRITE_CI_MRS_CMDr(unit,.4,  0x000000cc);
WRITE_CI_MRS_CMDr(unit,.5,  0x0000008d);
WRITE_CI_MRS_CMDr(unit,.5,  0x000000cc);
sal_sleep( 2);
WRITE_CI_MRS_CMDr(unit,.0,  0x0000008c);
WRITE_CI_MRS_CMDr(unit,.0,  0x000000fc);
WRITE_CI_MRS_CMDr(unit,.1,  0x0000008c);
WRITE_CI_MRS_CMDr(unit,.1,  0x000000fc);
WRITE_CI_MRS_CMDr(unit,.2,  0x0000008c);
WRITE_CI_MRS_CMDr(unit,.2,  0x000000fc);
WRITE_CI_MRS_CMDr(unit,.3,  0x0000008c);
WRITE_CI_MRS_CMDr(unit,.3,  0x000000fc);
WRITE_CI_MRS_CMDr(unit,.4,  0x0000008c);
WRITE_CI_MRS_CMDr(unit,.4,  0x000000fc);
WRITE_CI_MRS_CMDr(unit,.5,  0x0000008c);
WRITE_CI_MRS_CMDr(unit,.5,  0x000000fc);
sal_sleep( 2);
WRITE_CI_CONFIG0r(unit,.3,  0x0324ab03);
WRITE_CI_CONFIG0r(unit,.3,  0x0324aa03);
WRITE_CI_CONFIG0r(unit,.5,  0x0324ab03);
WRITE_CI_CONFIG0r(unit,.5,  0x0324aa03);
WRITE_CI_CONFIG2r(unit,.3,  0x0d4860f0);
WRITE_CI_CONFIG2r(unit,.3,  0x0c4860f0);
WRITE_CI_CONFIG2r(unit,.5,  0x0d4860f0);
WRITE_CI_CONFIG2r(unit,.5,  0x0c4860f0);
sal_sleep( 2);
READ_CI_CONFIG0r(unit, 3, &rval);
soc_reg_field_set(unit, CI_CONFIG0r, &rval, TWLf, 0xa);
WRITE_CI_CONFIG0r(unit, 3, rval);
READ_CI_CONFIG0r(unit, 5, &rval);
soc_reg_field_set(unit, CI_CONFIG0r, &rval, TWLf, 0xa);
WRITE_CI_CONFIG0r(unit, 5, rval);
READ_CI_CONFIG2r(unit, 3, &rval);
soc_reg_field_set(unit, CI_CONFIG2r, &rval, TREAD_ENBf, 0xc);
WRITE_CI_CONFIG2r(unit, 3, rval);
READ_CI_CONFIG2r(unit, 3, &rval);
soc_reg_field_set(unit, CI_CONFIG2r, &rval, TREAD_ENBf, 0xc);
WRITE_CI_CONFIG2r(unit, 3, rval);
sal_sleep( 2);

}

int _soc_katana2_misc_init(int unit)
{
    soc_info_t *si;
    uint32 rval;
    uint32 xport_port_enable_reg=0;
#if 0
    uint32 mxq6_7_rval=0;
#endif
    uint32 entry[SOC_MAX_MEM_WORDS], ing_entry[SOC_MAX_MEM_WORDS];
    txlp_port_addr_map_table_entry_t txlp_port_addr_map_table_entry={{0}};
    uint32                start_addr=0;
    uint32                end_addr=0;
    uint8                 mxqblock_port = 0;
    soc_pbmp_t pbmp;
    int port, blk_port, blk;
    int count;
    uint64 reg64;
    uint16 dev_id;
    uint8 rev_id;
    int i;
    soc_field_t fields[3];
    uint32 values[3];
    int rv;
    bcmMxqCorePortMode_t core_mode = 0;
    bcmMxqPhyPortMode_t  phy_mode;
    int                  any_phy_port=0; 
    uint8                loop=0;             
    int                  cfg_num=0; 
    uint8                mxqblock=0;
    uint8                port_count=0;
    uint8                xfi_mode=0;
    uint8                wc_10g_21g_sel=0;
    uint32               port_speed=0;
    uint32               any_port_speed=0;

    soc_field_t port_enable_field[KT2_MAX_MXQPORTS_PER_BLOCK]=
                {PORT0f, PORT1f, PORT2f , PORT3f};
   soc_field_t wc_xfi_mode_sel_fld[]={WC0_8_XFI_MODE_SELf,WC1_8_XFI_MODE_SELf};
   uint32      top_misc_control_1_val = 0;

    soc_cm_get_id(unit, &dev_id, &rev_id);

    si = &SOC_INFO(unit);

    if (!SOC_IS_RELOADING(unit) && !SOC_WARM_BOOT(unit)) {
        /* Clear IPIPE/EIPIE Memories */
        SOC_IF_ERROR_RETURN(soc_katana2_pipe_mem_clear(unit));
        SOC_IF_ERROR_RETURN(soc_katana2_linkphy_mem_clear(unit));
    }

    /* Some registers are implemented in memory, need to clear them in order
     * to have correct parity value */
    PBMP_PP_ALL_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(WRITE_EGR_VLAN_CONTROL_1r(unit, port, 0));
        SOC_IF_ERROR_RETURN(WRITE_EGR_IPMC_CFG2r(unit, port, 0));
    }

    if (SOC_IS_KATANA2(unit) && SAL_BOOT_QUICKTURN) {
        if (soc_property_get(unit, spn_DDR3_AUTO_TUNE, FALSE)) {
            soc_cm_print("QUICKTURN:ATTN %s:%d \n",__FUNCTION__,  __LINE__);
            _soc_katana2_ci_init(unit);
        }
    }

    /* Reset MXQPORT MIB counter (registers implemented in memory) */
    SOC_BLOCK_ITER(unit, blk, SOC_BLK_MXQPORT) {
        blk_port = SOC_BLOCK_PORT(unit, blk);
        if (blk_port < 0) {
            continue;
        }
        rval = 0;
        soc_reg_field_set(unit, XPORT_MIB_RESETr, &rval, CLR_CNTf, 0xf); /* All Ports */
        SOC_IF_ERROR_RETURN(WRITE_XPORT_MIB_RESETr(unit, blk_port, rval));
        SOC_IF_ERROR_RETURN(WRITE_XPORT_MIB_RESETr(unit, blk_port, 0));
    }

    if (soc_property_get(unit, spn_PARITY_ENABLE, FALSE)) {
        _soc_katana2_parity_enable_all(unit, TRUE);
    }

    sal_memset(entry, 0, sizeof(cpu_pbm_entry_t));
    soc_mem_pbmp_field_set(unit, CPU_PBMm, entry, BITMAPf, &PBMP_CMIC(unit));
    SOC_IF_ERROR_RETURN
        (soc_mem_write(unit, CPU_PBMm, MEM_BLOCK_ALL, 0, entry));

    sal_memset(entry, 0, sizeof(cpu_pbm_2_entry_t));
    soc_mem_pbmp_field_set(unit, CPU_PBM_2m, entry, BITMAPf, &PBMP_CMIC(unit));
    SOC_IF_ERROR_RETURN
        (soc_mem_write(unit, CPU_PBM_2m, MEM_BLOCK_ALL, 0, entry));

    SOC_PBMP_CLEAR(pbmp);
    PBMP_ALL_ITER(unit, port) {
        if (IS_LB_PORT(unit, port) ||
            (si->port_group[port] >= 2 && si->port_group[port] <= 3)) {
            SOC_PBMP_PORT_ADD(pbmp, port);
        }
    }

    sal_memset(entry, 0, sizeof(egr_physical_port_entry_t));
    sal_memset(ing_entry, 0, sizeof(ing_physical_port_table_entry_t));

    soc_mem_field32_set(unit, EGR_PHYSICAL_PORTm, entry, PORT_TYPEf, 1);
    soc_mem_field32_set(unit, ING_PHYSICAL_PORT_TABLEm,
            ing_entry, PORT_TYPEf, 1);
    /* Higig ports */
    PBMP_HG_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(soc_reg_field32_modify(
            unit, XPORT_CONFIGr, port, HIGIG_MODEf, 1));        
        SOC_IF_ERROR_RETURN(soc_mem_write(unit, EGR_PHYSICAL_PORTm,
            MEM_BLOCK_ALL, port, entry));
        SOC_IF_ERROR_RETURN(soc_mem_write(unit, ING_PHYSICAL_PORT_TABLEm, 
            MEM_BLOCK_ALL, port, ing_entry));
    }
  
    /* CMIC higig index */
    SOC_IF_ERROR_RETURN(soc_mem_write(
        unit, EGR_PHYSICAL_PORTm, MEM_BLOCK_ALL, si->cpu_hg_index, entry));
    SOC_IF_ERROR_RETURN(soc_mem_write(
        unit, ING_PHYSICAL_PORT_TABLEm, MEM_BLOCK_ALL,
        si->cpu_hg_index, ing_entry));

    /* Loopback port */
    soc_mem_field32_set(unit, EGR_PHYSICAL_PORTm, entry, PORT_TYPEf, 2);
    soc_mem_field32_set(unit, ING_PHYSICAL_PORT_TABLEm, ing_entry,
        PORT_TYPEf, 2);
    SOC_IF_ERROR_RETURN(soc_mem_write(
        unit, EGR_PHYSICAL_PORTm, MEM_BLOCK_ALL, LB_PORT(unit), entry));
    SOC_IF_ERROR_RETURN(soc_mem_write(
        unit, ING_PHYSICAL_PORT_TABLEm,
        MEM_BLOCK_ALL, LB_PORT(unit), ing_entry));
    /* ingress physical port to pp_port mapping */
    SOC_IF_ERROR_RETURN(soc_mem_field32_modify(
        unit, ING_PHYSICAL_PORT_TABLEm,
        LB_PORT(unit), PP_PORTf, LB_PORT(unit)));

    /* pp_port to egress physical port mapping */
    SOC_IF_ERROR_RETURN(soc_mem_field32_modify(
        unit, PP_PORT_TO_PHYSICAL_PORT_MAPm,
        LB_PORT(unit), DESTINATIONf,LB_PORT(unit)));

    /* All quad ports in default config */
    
    cfg_num = soc_property_get(unit, spn_BCM5645X_CONFIG, 0);
    SOC_BLOCK_ITER(unit, blk, SOC_BLK_MXQPORT) {
        blk_port = SOC_BLOCK_PORT(unit, blk);
        if (blk_port < 0) {
            continue;
        }
        SOC_IF_ERROR_RETURN(soc_katana2_get_port_mxqblock(
                            unit,blk_port,&mxqblock));
        port_count=0; 
        xfi_mode = 0;
        if ((mxqblock == 8) || (mxqblock == 9)) {
             for (loop=0;loop<KT2_MAX_MXQPORTS_PER_BLOCK; loop++) {
                  if (SOC_PBMP_MEMBER((PBMP_ALL(unit)), 
                      kt2_mxqblock_ports[mxqblock][loop])) {
                      if (bcm56450_speed[cfg_num]
                             [kt2_mxqblock_ports[mxqblock][loop]-1] == 10000) {
                          port_count++;
                      }
                  }  
             }
             if (port_count == KT2_MAX_MXQPORTS_PER_BLOCK) {
                /*Need To Set XFI Mode then */
                xfi_mode = 1;
             }
             SOC_IF_ERROR_RETURN(READ_TOP_MISC_CONTROL_1r(
                                 unit,&top_misc_control_1_val));
             soc_reg_field_set(unit, TOP_MISC_CONTROL_1r, 
                               &top_misc_control_1_val,
                               wc_xfi_mode_sel_fld[mxqblock-8],xfi_mode);
             SOC_IF_ERROR_RETURN(WRITE_TOP_MISC_CONTROL_1r(
                                 unit,top_misc_control_1_val));
        }
        xport_port_enable_reg = 0;
        any_phy_port=0; 
        any_port_speed=0; 
        for (loop=0;loop<KT2_MAX_MXQPORTS_PER_BLOCK; loop++) {
             if (SOC_PBMP_MEMBER((PBMP_ALL(unit)), 
                            kt2_mxqblock_ports[mxqblock][loop])) {
                 if ((kt2_mxqblock_ports[mxqblock][loop] == KT2_OLP_PORT) &&
                     (si->olp_port == 1) ) {
                     
                     soc_reg_field_set(unit, XPORT_PORT_ENABLEr, 
                                   &xport_port_enable_reg,
                                   port_enable_field[0], 1); /*Enable port */
                 } else { 
                     soc_reg_field_set(unit, XPORT_PORT_ENABLEr, 
                                   &xport_port_enable_reg,
                                   port_enable_field[loop], 1); /*Enable port */
                 }
                 if (any_phy_port == 0) {
                     any_phy_port= kt2_mxqblock_ports[mxqblock][loop];
                     any_port_speed= bcm56450_speed[cfg_num]
                                         [kt2_mxqblock_ports[mxqblock][loop]-1];
                 }
             } else {
                 soc_reg_field_set(unit, XPORT_PORT_ENABLEr, 
                                   &xport_port_enable_reg,
                                   port_enable_field[loop], 0);/*Disable port */
             }
        }

        rval = 0;
        SOC_IF_ERROR_RETURN(soc_katana2_get_phy_port_mode(
                            unit, any_phy_port,
                            bcm56450_speed[cfg_num][any_phy_port-1], 
                            &phy_mode));
        soc_reg_field_set(unit, XPORT_MODE_REGr, &rval,
                            PHY_PORT_MODEf, phy_mode);

        SOC_IF_ERROR_RETURN(soc_katana2_get_core_port_mode(
                            unit,blk_port,&core_mode));
        soc_reg_field_set(unit, XPORT_MODE_REGr, &rval,
                            CORE_PORT_MODEf, core_mode);
        wc_10g_21g_sel = 0;
        if ((mxqblock>=6) && (mxqblock<=9)) {
            for (loop=0;loop<KT2_MAX_MXQPORTS_PER_BLOCK; loop++) {
                 port_speed= bcm56450_speed[cfg_num]
                                      [kt2_mxqblock_ports[mxqblock][loop]-1];
                 if ((mxqblock==8) || (mxqblock==9)) {
                      if ((SAL_BOOT_QUICKTURN) && 
                          (core_mode == bcmMxqCorePortModeSingle) &&
                          (SOC_PBMP_MEMBER(si->linkphy_pbm, 
                               kt2_mxqblock_ports[mxqblock][loop]))) {
                          soc_cm_print("QUICKTURN: ATTN : 10G_21G_SEL=0 \n");
                          wc_10g_21g_sel = 0;
                          break;
                      }
                      if ((port_speed == 10000) || (port_speed == 21000)) {
                           wc_10g_21g_sel = 1;
                           break;
                      }
                 }
                 if ((mxqblock==6) || (mxqblock==7)) {
                      if ((port_speed == 10000) && (xfi_mode ==1)) {
                           wc_10g_21g_sel = 1;
                           break;
                      }
                 }
            }
#if 0
            if (xfi_mode) { /* so obvious 10g speed */
                mxq6_7_rval=0;
                soc_reg_field_set(unit, XPORT_MODE_REGr, &mxq6_7_rval,
                                  WC_10G_21G_SELf, wc_10g_21g_sel);
                SOC_IF_ERROR_RETURN(WRITE_XPORT_MODE_REGr(unit, 
                             kt2_mxqblock_ports[mxqblock-2][0], mxq6_7_rval));
            }
#endif
            soc_reg_field_set(unit, XPORT_MODE_REGr, &rval,
                               WC_10G_21G_SELf, wc_10g_21g_sel);

        }
        if ((any_port_speed != 0 ) && 
            (any_port_speed < 10000)) {
             soc_reg_field_set(unit, XPORT_MODE_REGr, &rval,
                               PORT_GMII_MII_ENABLEf, 1);
        } else {
             soc_reg_field_set(unit, XPORT_MODE_REGr, &rval,
                               PORT_GMII_MII_ENABLEf, 0);
        } 
        SOC_IF_ERROR_RETURN(WRITE_XPORT_MODE_REGr(unit, blk_port, rval));
        SOC_IF_ERROR_RETURN(WRITE_XPORT_PORT_ENABLEr(unit, blk_port, 
                                                     xport_port_enable_reg));
    }

    count = 32;

    /* Setup main TDM control */
    SOC_IF_ERROR_RETURN(READ_IARB_TDM_CONTROLr(unit, &rval));
    /* BCMSIM handles Single Cell packets */
    if (SAL_BOOT_BCMSIM) {
        soc_reg_field_set(unit, IARB_TDM_CONTROLr, &rval, DISABLEf, 1);
    } else {
        soc_reg_field_set(unit, IARB_TDM_CONTROLr, &rval, DISABLEf, 0);
    }
    soc_reg_field_set(unit, IARB_TDM_CONTROLr, &rval, TDM_WRAP_PTRf, count);
    SOC_IF_ERROR_RETURN(WRITE_IARB_TDM_CONTROLr(unit, rval));

    SOC_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &rval));
    soc_reg_field_set(unit, MISCCONFIGr, &rval, METERING_CLK_ENf, 1);
    SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, rval));

    if (0) { 
        rval = 0;
        soc_reg_field_set(unit, ING_BYPASS_CTRLr, &rval, IFP_BYPASS_ENABLEf,
                          1);
        SOC_IF_ERROR_RETURN(WRITE_ING_BYPASS_CTRLr(unit, rval));

        rval = 0;
        soc_reg_field_set(unit, EGR_BYPASS_CTRLr, &rval, EFP_BYPASSf, 1);
        SOC_IF_ERROR_RETURN(WRITE_EGR_BYPASS_CTRLr(unit, rval));
    }

    /* Enable dual hash on L2, L3 and MPLS_ENTRY tables */
    fields[0] = ENABLEf;
    values[0] = 1;
    fields[1] = HASH_SELECTf;
    values[1] = FB_HASH_CRC32_LOWER;
    fields[2] = INSERT_LEAST_FULL_HALFf;
    values[2] = 1;
    SOC_IF_ERROR_RETURN
        (soc_reg_fields32_modify(unit, L2_AUX_HASH_CONTROLr, REG_PORT_ANY, 3,
                                 fields, values));
    SOC_IF_ERROR_RETURN
        (soc_reg_fields32_modify(unit, L3_AUX_HASH_CONTROLr, REG_PORT_ANY, 3,
                                 fields, values));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, MPLS_ENTRY_HASH_CONTROLr, REG_PORT_ANY,
                                INSERT_LEAST_FULL_HALFf, 1));

    /*
     * IPMC init 
     */
    SOC_IF_ERROR_RETURN(READ_RQE_GLOBAL_CONFIGr(unit, &rval));
    soc_reg_field_set(unit,RQE_GLOBAL_CONFIGr, &rval, PARITY_GEN_ENf,
                      1);
    soc_reg_field_set(unit,RQE_GLOBAL_CONFIGr, &rval, PARITY_CHK_ENf,
                      1);
    SOC_IF_ERROR_RETURN(WRITE_RQE_GLOBAL_CONFIGr(unit, rval));

    /*
     * Egress Enable
     */
    sal_memset(entry, 0, sizeof(egr_enable_entry_t));
    soc_mem_field32_set(unit, EGR_ENABLEm, entry, PRT_ENABLEf, 0);
    PBMP_ALL_ITER(unit, port) {
        rv = WRITE_EGR_ENABLEm(unit, MEM_BLOCK_ALL, port, entry);
        /* ****************************************************************** */
        
        /* ****************************************************************** */
        if (SOC_FAILURE(rv)) {
            if (SOC_IS_KATANA2(unit) && (rv == _SHR_E_PARAM)) {
                continue;
            }
            return rv;
        }
    }

    sal_memset(entry, 0, sizeof(epc_link_bmap_entry_t));
    soc_mem_pbmp_field_set(unit, EPC_LINK_BMAPm, entry, PORT_BITMAPf,
                           &PBMP_CMIC(unit));
    SOC_IF_ERROR_RETURN(WRITE_EPC_LINK_BMAPm(unit, MEM_BLOCK_ALL, 0, entry));

    SOC_IF_ERROR_RETURN(READ_ING_CONFIG_64r(unit, &reg64));
    soc_reg64_field32_set(unit, ING_CONFIG_64r, &reg64,
                          L3SRC_HIT_ENABLEf, 1);
    soc_reg64_field32_set(unit, ING_CONFIG_64r, &reg64,
                          L2DST_HIT_ENABLEf, 1);
    soc_reg64_field32_set(unit, ING_CONFIG_64r, &reg64,
                          APPLY_EGR_MASK_ON_L2f, 1);
    soc_reg64_field32_set(unit, ING_CONFIG_64r, &reg64,
                          APPLY_EGR_MASK_ON_L3f, 1);
    soc_reg64_field32_set(unit, ING_CONFIG_64r, &reg64,
                          ARP_RARP_TO_FPf, 0x3); /* enable both ARP & RARP */
    soc_reg64_field32_set(unit, ING_CONFIG_64r, &reg64,
                          ARP_VALIDATION_ENf, 1);
    soc_reg64_field32_set(unit, ING_CONFIG_64r, &reg64,
                          IGNORE_HG_HDR_LAG_FAILOVERf, 1);
    SOC_IF_ERROR_RETURN(WRITE_ING_CONFIG_64r(unit, reg64));


    /* The HW defaults for EGR_VLAN_CONTROL_1.VT_MISS_UNTAG == 1, which
     * causes the outer tag to be removed from packets that don't have
     * a hit in the egress vlan tranlation table. Set to 0 to disable this.
     */
    rval = 0;
    soc_reg_field_set(unit, EGR_VLAN_CONTROL_1r, &rval, VT_MISS_UNTAGf, 0);

    /* Enable pri/cfi remarking on egress ports. */
    soc_reg_field_set(unit, EGR_VLAN_CONTROL_1r, &rval, REMARK_OUTER_DOT1Pf,
                      1);
    PBMP_ALL_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(WRITE_EGR_VLAN_CONTROL_1r(unit, port, rval));
    }

    SOC_PBMP_ASSIGN(pbmp, PBMP_ALL(unit));
    SOC_PBMP_REMOVE(pbmp, PBMP_LB(unit));
    SOC_IF_ERROR_RETURN(soc_mem_read(unit, ING_EN_EFILTER_BITMAPm,
                                     MEM_BLOCK_ANY, 0, &entry));
    soc_mem_pbmp_field_set(unit, ING_EN_EFILTER_BITMAPm, &entry, BITMAPf,
                           &pbmp);
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, ING_EN_EFILTER_BITMAPm,
                                      MEM_BLOCK_ANY, 0, &entry));

    SOC_IF_ERROR_RETURN(READ_CMIC_TXBUF_CONFIGr(unit, &rval));
    soc_reg_field_set(unit, CMIC_TXBUF_CONFIGr, &rval,
                            FIRST_SERVE_BUFFERS_WITH_EOP_CELLSf, 0);
    SOC_IF_ERROR_RETURN(WRITE_CMIC_TXBUF_CONFIGr(unit, rval));

    if (soc_mspi_init(unit) != SOC_E_NONE) {
        soc_cm_debug(DK_WARN, "unit %d : MSPI Init Failed\n", unit);
    }

    if (soc_feature(unit, soc_feature_ces)) {
	/*
	 * Add CES TDM ports
	 */
	for (i = 0;i < 16;i++) {
	    int port = 39 + i;
	    SOC_PBMP_PORT_ADD(si->tdm_pbm, port);
	    si->tdm.port[si->tdm.num++] = port;
	    if (si->tdm.min > port || si->tdm.min < 0) {
		si->tdm.min = port;
	    }
	    if (si->tdm.max < port) {
		si->tdm.max = port;
	    }
	    SOC_PBMP_PORT_ADD(si->tdm.bitmap, port);
	}
    }
    /* TXLP_PORT_ADDR_MAP_TABLE */
    for (mxqblock=8;mxqblock<=9;mxqblock++) {
         for (loop=0,start_addr=0;loop<KT2_MAX_MXQPORTS_PER_BLOCK;loop++) {
             sal_memset(&txlp_port_addr_map_table_entry,0,
                        sizeof(txlp_port_addr_map_table_entry_t));
             mxqblock_port=kt2_mxqblock_ports[mxqblock][loop];
             if (SOC_PBMP_MEMBER((PBMP_ALL(unit)), mxqblock_port)) {
                 if (!SOC_PBMP_MEMBER(si->linkphy_pbm, mxqblock_port)) {
                     if (si->port_speed_max[mxqblock_port] <= 2500) {
                         end_addr = start_addr + 5; /* 6 Cells */
                     } else if (si->port_speed_max[mxqblock_port] <= 10000) {
                         end_addr = start_addr + 11; /* 12 Cells */
                     } else if (si->port_speed_max[mxqblock_port] <= 13000) {
                         end_addr = start_addr + 15; /* 16 Cells */
                     } else if (si->port_speed_max[mxqblock_port] <= 21000) {
                         end_addr = start_addr + 19; /* 20 Cells */
                     }
                     soc_TXLP_PORT_ADDR_MAP_TABLEm_field_set(unit,
                         &txlp_port_addr_map_table_entry,START_ADDRf,&start_addr);
                     soc_TXLP_PORT_ADDR_MAP_TABLEm_field_set(unit,
                         &txlp_port_addr_map_table_entry,END_ADDRf,&end_addr);
                     start_addr = end_addr+1;
                 }
             } 
             SOC_IF_ERROR_RETURN(WRITE_TXLP_PORT_ADDR_MAP_TABLEm(
                                 unit,SOC_INFO(unit).txlp_block[mxqblock-8],
                                 loop, &txlp_port_addr_map_table_entry));
        }
    }

    return SOC_E_NONE;
}

#define KA2_MMU_EFIFO_DEPTH             10
#define KA2_MMU_EFIFO_XMIT_THERESHOLD   4

/* Just splitted _soc_katana2_mmu_init() for flex IO feature */
/* Configure Egress Fifo */
/* Port BW Ctrl */
STATIC int
_soc_katana2_mmu_init1_port(int unit,int port,uint32 *nxtaddr)
{
    uint32     rval=0;
    soc_info_t *si= &SOC_INFO(unit);
    int depth = 0;
    int thereshold = 0;
    int mxqblock=0;

    if (SOC_PBMP_MEMBER((PBMP_ALL(unit)), port)) {
        if (!(IS_CPU_PORT(unit,port) || IS_LB_PORT(unit,port))) {
            mxqblock = kt2_port_to_mxqblock[port-1];
            /* Configure Egress Fifo */
            if(mxqblock_max_startaddr[mxqblock] == 0) {
               mxqblock_max_startaddr[mxqblock]= *nxtaddr;
            }
        }
        rval = 0;
        soc_reg_field_set(unit, DEQ_EFIFO_CFGr, &rval, 
                          EGRESS_FIFO_START_ADDRESSf, *nxtaddr);
        soc_cm_debug(DK_VERBOSE,"port=%d nxtaddr=%d \n",port,*nxtaddr);
        if (si->port_speed_max[port] <= 1000) {
            depth = 10;
            thereshold = (IS_EXT_MEM_PORT(unit, port)) ? 8 : 0;
        } else if (si->port_speed_max[port] < 10000) {
            depth = 16;
            thereshold = (IS_EXT_MEM_PORT(unit, port)) ? 15 : 0;
        } else if (si->port_speed_max[port] < 20000) {
            depth = 53;
            thereshold = (IS_EXT_MEM_PORT(unit, port)) ? 49 : 0;
        } else { /* > 20G */
            depth = 90;
            thereshold = (IS_EXT_MEM_PORT(unit, port)) ? 85 : 0;
        }
        soc_reg_field_set(unit, DEQ_EFIFO_CFGr, &rval, 
                          EGRESS_FIFO_XMIT_THRESHOLDf, thereshold);
        soc_reg_field_set(unit, DEQ_EFIFO_CFGr, &rval, 
                          EGRESS_FIFO_DEPTHf, depth);
        if (SOC_PBMP_MEMBER(si->linkphy_pbm, port)) {
            soc_reg_field_set(unit, DEQ_EFIFO_CFGr, &rval, 
                              EGRESS_FIFO_LINK_PHYf, 1);
        }
        *nxtaddr += depth;
        SOC_IF_ERROR_RETURN(WRITE_DEQ_EFIFO_CFGr(unit, port, rval));

        /* Port BW Ctrl */
        SOC_IF_ERROR_RETURN(READ_TOQ_PORT_BW_CTRLr(unit, port, &rval));
        if (IS_CPU_PORT(unit,port) || IS_LB_PORT(unit,port)) {
            soc_reg_field_set(unit, TOQ_PORT_BW_CTRLr, &rval, 
                              PORT_BWf, 50);
            soc_reg_field_set(unit, TOQ_PORT_BW_CTRLr, &rval, 
                              START_THRESHOLDf, 127);
        } else if (si->port_speed_max[port] >= 10000) {
            soc_reg_field_set(unit, TOQ_PORT_BW_CTRLr, &rval, 
                              PORT_BWf, 500);
            soc_reg_field_set(unit, TOQ_PORT_BW_CTRLr, &rval, 
                              START_THRESHOLDf, 34);
        } else if (si->port_speed_max[port] == 2500) {
            soc_reg_field_set(unit, TOQ_PORT_BW_CTRLr, &rval, 
                              PORT_BWf, 125);
            soc_reg_field_set(unit, TOQ_PORT_BW_CTRLr, &rval, 
                              START_THRESHOLDf, 19);
        } else {
            soc_reg_field_set(unit, TOQ_PORT_BW_CTRLr, &rval, 
                              PORT_BWf, 50);
            soc_reg_field_set(unit, TOQ_PORT_BW_CTRLr, &rval, 
                              START_THRESHOLDf, 7);
        }
        SOC_IF_ERROR_RETURN(WRITE_TOQ_PORT_BW_CTRLr(unit, port, rval));
        if (!(IS_CPU_PORT(unit,port) || IS_LB_PORT(unit,port))) {
            mxqblock_max_endaddr[mxqblock]=(*nxtaddr)-1;
        }
    } else {
        SOC_IF_ERROR_RETURN(WRITE_DEQ_EFIFO_CFGr(unit, port, 0));
        SOC_IF_ERROR_RETURN(WRITE_TOQ_PORT_BW_CTRLr(unit, port, 0));
    }
    return SOC_E_NONE;
}
STATIC int
_soc_katana2_mmu_init1(int unit) 
{
    uint32 nxtaddr=0;
    int    port=0;
    int    mxqblock=0;
    int    subport=0;
     /* cpu port */
     _soc_katana2_mmu_init1_port(unit,0,&nxtaddr);
    for (mxqblock=0;mxqblock<KT2_MAX_MXQBLOCKS ; mxqblock++) {
         for (subport=0;subport<KT2_MAX_MXQPORTS_PER_BLOCK; subport++) {
              port = kt2_mxqblock_ports[mxqblock][subport];
              _soc_katana2_mmu_init1_port(unit,port,&nxtaddr);
         }
    }
     /* lb port */
    _soc_katana2_mmu_init1_port(unit,41,&nxtaddr);
    for (mxqblock=0;mxqblock<KT2_MAX_MXQBLOCKS ; mxqblock++) {
         soc_cm_debug(DK_VERBOSE,"start=%d end=%d \n",
                      mxqblock_max_startaddr[mxqblock],
                      mxqblock_max_endaddr[mxqblock]);
    }
    return SOC_E_NONE;
}
/* Just splitted _soc_katana2_mmu_init() for flex IO feature */
/* LLS Queue Configuration */
STATIC int
_soc_katana2_mmu_init2(int unit)
{
    int                     port=0;
    int                     i=0;
    int                     rv=0;
    lls_port_config_entry_t lls_port_cfg={{0}};
    lls_l0_parent_entry_t   l0_parent={{0}};
    lls_l0_config_entry_t   l0_config={{0}};
    lls_l1_parent_entry_t   l1_parent={{0}};
    lls_l1_config_entry_t   l1_config={{0}};
    lls_l2_parent_entry_t   l2_parent={{0}};

    /* LLS Queue Configuration */
    for (port=0;port<42;port++) {
         sal_memset(&lls_port_cfg, 0, sizeof(lls_port_config_entry_t));
         sal_memset(&l0_parent, 0, sizeof(lls_l0_parent_entry_t));
         sal_memset(&l1_parent, 0, sizeof(lls_l1_parent_entry_t));
         sal_memset(&l1_config, 0, sizeof(lls_l1_config_entry_t));
         sal_memset(&l2_parent, 0, sizeof(lls_l2_parent_entry_t));
         if (SOC_PBMP_MEMBER((PBMP_ALL(unit)), port)) {
             soc_mem_field32_set(unit, LLS_PORT_CONFIGm, 
                                 &lls_port_cfg, L0_LOCK_ON_PACKETf, 1);
             soc_mem_field32_set(unit, LLS_PORT_CONFIGm, 
                                 &lls_port_cfg, L1_LOCK_ON_PACKETf, 1);
             soc_mem_field32_set(unit, LLS_PORT_CONFIGm, 
                                 &lls_port_cfg, L2_LOCK_ON_PACKETf, 1);
             soc_mem_field32_set(unit, LLS_PORT_CONFIGm, 
                                 &lls_port_cfg, P_NUM_SPRIf, 1);
             soc_mem_field32_set(unit, LLS_PORT_CONFIGm, 
                                 &lls_port_cfg, P_START_SPRIf, (port*4));
             soc_mem_field32_set(unit, LLS_L0_PARENTm, 
                                 &l0_parent, C_PARENTf, (port));
             SOC_IF_ERROR_RETURN(WRITE_LLS_L0_PARENTm(unit, MEM_BLOCK_ALL, 
                                 (port*4), &l0_parent));
             sal_memset(&l0_config, 0, sizeof(lls_l0_config_entry_t));
             soc_mem_field32_set(unit, LLS_L0_CONFIGm, 
                                 &l0_config, P_NUM_SPRIf, 1);
             soc_mem_field32_set(unit, LLS_L0_CONFIGm, 
                                 &l0_config, P_START_SPRIf, (port * 16));
             soc_mem_field32_set(unit, LLS_L1_PARENTm, 
                                 &l1_parent, C_PARENTf, (port * 4));
             soc_mem_field32_set(unit, LLS_L1_CONFIGm, 
                                 &l1_config, P_NUM_SPRIf, 8);
#ifndef _KATANA2_DEBUG
        /* Changed in the latest QT Regfile. MMU Implementor to revisit / reconfirm */
             soc_mem_field32_set(unit, LLS_L1_CONFIGm, 
                                 &l1_config, P_START_UC_SPRIf,
                                 IS_CPU_PORT(unit, port) ? 0 : (port*8)+40);
             soc_mem_field32_set(unit, LLS_L1_CONFIGm, 
                                 &l1_config, P_START_MC_SPRIf,
                                 IS_CPU_PORT(unit, port) ? 0 : (port*8)+40);
#else
             soc_mem_field32_set(unit, LLS_L1_CONFIGm, 
                                 &l1_config, P_START_SPRIf,
                                 IS_CPU_PORT(unit, port) ? 0 : (port*8)+40);
#endif
             soc_mem_field32_set(unit, LLS_L2_PARENTm, 
                                 &l2_parent, C_PARENTf, (port*16));
         } 
         rv = WRITE_LLS_PORT_CONFIGm(unit, MEM_BLOCK_ALL, port, &lls_port_cfg);
         /******************************************************************* */
         
         /******************************************************************* */
         if (SOC_FAILURE(rv)) {
             if (SOC_IS_KATANA2(unit) && (rv == _SHR_E_PARAM)) {
                 continue;
             }
             return rv;
         }
         SOC_IF_ERROR_RETURN(WRITE_LLS_L0_CONFIGm(unit, MEM_BLOCK_ALL, 
                             (port * 4), &l0_config));
         SOC_IF_ERROR_RETURN(WRITE_LLS_L1_PARENTm(unit, MEM_BLOCK_ALL, 
                             (port * 16), &l1_parent));
         SOC_IF_ERROR_RETURN(WRITE_LLS_L1_CONFIGm(unit, MEM_BLOCK_ALL, 
                             (port * 16), &l1_config));
         for(i=0; i<8; i++) {
             SOC_IF_ERROR_RETURN(WRITE_LLS_L2_PARENTm(unit, MEM_BLOCK_ALL,
                    IS_CPU_PORT(unit, port) ? i : ((port*8)+40+i), &l2_parent));
         }
    }
    return SOC_E_NONE;
}
/* Just splitted _soc_katana2_mmu_init() for flex IO feature */
STATIC int
_soc_katana2_mmu_init3(int unit)
{
    int    port=0;
    uint32 rval=0;
    for (port=0;port<42;port++) {
         rval = 0;
         if (SOC_PBMP_MEMBER((PBMP_ALL(unit)), port)) {
             soc_reg_field_set(unit, PORT_MAX_PKT_SIZEr, &rval, 
                               PORT_MAX_PKT_SIZEf,49);
         }
         SOC_IF_ERROR_RETURN(WRITE_PORT_MAX_PKT_SIZEr(unit, port, rval));
         SOC_IF_ERROR_RETURN(WRITE_THDIEXT_PORT_MAX_PKT_SIZEr(unit, port,
                                                              rval));
         SOC_IF_ERROR_RETURN(WRITE_THDIEMA_PORT_MAX_PKT_SIZEr(unit, port, 
                                                              rval));
         SOC_IF_ERROR_RETURN(WRITE_THDIRQE_PORT_MAX_PKT_SIZEr(unit, port, 
                                                              rval));
         SOC_IF_ERROR_RETURN(WRITE_THDIQEN_PORT_MAX_PKT_SIZEr(unit, port, 
                                                              rval));

         rval = 0;
         SOC_IF_ERROR_RETURN(WRITE_THDIEXT_PORT_PRI_XON_ENABLEr(unit, port, 
                                                                rval));
         SOC_IF_ERROR_RETURN(WRITE_THDIEXT_PORT_PRI_XON_ENABLEr(unit, port, 
                                                                rval));
         SOC_IF_ERROR_RETURN(WRITE_THDIEMA_PORT_PRI_XON_ENABLEr(unit, port, 
                                                                rval));
         SOC_IF_ERROR_RETURN(WRITE_THDIRQE_PORT_PRI_XON_ENABLEr(unit, port, 
                                                                rval));
         SOC_IF_ERROR_RETURN(WRITE_THDIQEN_PORT_PRI_XON_ENABLEr(unit, port, 
                                                                rval));
    }

    /* MXQ Ports */
    for (port=0;port<42;port++) {
         rval = 0;
         if (SOC_PBMP_MEMBER((PBMP_MXQ_ALL(unit)), port)) {
             rval = 0xffffff; /* PRIx_GRP = 0x7 */
         }
         SOC_IF_ERROR_RETURN(WRITE_PORT_PRI_GRP0r(unit, port, rval));
         SOC_IF_ERROR_RETURN(WRITE_PORT_PRI_GRP1r(unit, port, rval));
         SOC_IF_ERROR_RETURN(WRITE_THDIEXT_PORT_PRI_GRP0r(unit, port, rval));
         SOC_IF_ERROR_RETURN(WRITE_THDIEXT_PORT_PRI_GRP1r(unit, port, rval));
         SOC_IF_ERROR_RETURN(WRITE_THDIEMA_PORT_PRI_GRP0r(unit, port, rval));
         SOC_IF_ERROR_RETURN(WRITE_THDIEMA_PORT_PRI_GRP1r(unit, port, rval));
         SOC_IF_ERROR_RETURN(WRITE_THDIRQE_PORT_PRI_GRP0r(unit, port, rval));
         SOC_IF_ERROR_RETURN(WRITE_THDIRQE_PORT_PRI_GRP1r(unit, port, rval));
         SOC_IF_ERROR_RETURN(WRITE_THDIQEN_PORT_PRI_GRP0r(unit, port, rval));
         SOC_IF_ERROR_RETURN(WRITE_THDIQEN_PORT_PRI_GRP1r(unit, port, rval));
    }
    return SOC_E_NONE;
}
int _soc_katana2_mmu_reconfigure(int unit)
{
  SOC_IF_ERROR_RETURN(_soc_katana2_mmu_init1(unit));
  SOC_IF_ERROR_RETURN(_soc_katana2_mmu_init2(unit));
  SOC_IF_ERROR_RETURN(_soc_katana2_mmu_init3(unit));
  return SOC_E_NONE;
}

STATIC int
_soc_katana2_mmu_init(int unit)
{
    uint32 rval;
    uint64 r64val, rval64;
    int                 mmu_init_usec, i;
    soc_timeout_t       to;
    uint32 entry[SOC_MAX_MEM_WORDS];
    mmu_aging_lmt_int_entry_t age_entry;
    int count;
    uint16 dev_id;
    uint8 rev_id;

    soc_cm_get_id(unit, &dev_id, &rev_id);

    /* Init Link List Scheduler */
    rval = 0;
    soc_reg_field_set(unit, LLS_SOFT_RESETr, &rval, SOFT_RESETf, 0);
    SOC_IF_ERROR_RETURN(WRITE_LLS_SOFT_RESETr(unit, rval));

    rval = 0;
    soc_reg_field_set(unit, LLS_INITr, &rval, INITf, 1);
    SOC_IF_ERROR_RETURN(WRITE_LLS_INITr(unit, rval));

    /* Wait for LLS init done. */
    if (SAL_BOOT_SIMULATION) {
        mmu_init_usec = 10000000; /* Simulation  .. 10sec */
    } else {
        mmu_init_usec = 50000;
    }
    soc_timeout_init(&to, mmu_init_usec, 0);
    do {
        SOC_IF_ERROR_RETURN(READ_LLS_INITr(unit, &rval));
        if (soc_reg_field_get(unit, LLS_INITr, rval, INIT_DONEf)) {
            break;
        }
        if (soc_timeout_check(&to)) {
            soc_cm_debug(DK_WARN, "unit %d : LLS INIT timeout\n", unit);
            break;
        }
    } while (TRUE);

    /* Setup TDM for MMU Arb & LLS */
    SOC_IF_ERROR_RETURN(_soc_katana2_mmu_tdm_init(unit));

    for(i=0; i<16; i++) {
        SOC_IF_ERROR_RETURN(READ_TOQ_EXT_MEM_BW_MAP_TABLEr(unit, i, &rval));
        
        soc_reg_field_set(unit, TOQ_EXT_MEM_BW_MAP_TABLEr, &rval,
                                            GBL_GUARENTEE_BW_LIMITf, 1450);
        soc_reg_field_set(unit, TOQ_EXT_MEM_BW_MAP_TABLEr, &rval,
                                            WR_PHASEf, 120);
        soc_reg_field_set(unit, TOQ_EXT_MEM_BW_MAP_TABLEr, &rval,
                                            RD_PHASEf, 120);
        SOC_IF_ERROR_RETURN(
            WRITE_TOQ_EXT_MEM_BW_MAP_TABLEr(unit,i,rval));
    }

    SOC_IF_ERROR_RETURN(_soc_katana2_mmu_init1(unit));

    rval = 0;
    soc_reg_field_set(unit, DEQ_EFIFO_CFG_COMPLETEr, &rval, EGRESS_FIFO_CONFIGURATION_COMPLETEf, 1);
    SOC_IF_ERROR_RETURN(WRITE_DEQ_EFIFO_CFG_COMPLETEr(unit, rval));

    /* Enable LLS */
    rval = 0;
    soc_reg_field_set(unit, LLS_CONFIG0r, &rval, DEQUEUE_ENABLEf, 1);
    soc_reg_field_set(unit, LLS_CONFIG0r, &rval, ENQUEUE_ENABLEf, 1);
    soc_reg_field_set(unit, LLS_CONFIG0r, &rval, FC_ENABLEf, 1);
    soc_reg_field_set(unit, LLS_CONFIG0r, &rval, MIN_ENABLEf, 1);
    soc_reg_field_set(unit, LLS_CONFIG0r, &rval, PORT_SCHEDULER_ENABLEf, 1);
    soc_reg_field_set(unit, LLS_CONFIG0r, &rval, SHAPER_ENABLEf, 1);
    if (SOC_REG_FIELD_VALID(unit, LLS_CONFIG0r, SPRI_VECT_MODE_ENABLEf)) {
        if (soc_feature(unit, soc_feature_vector_based_spri)) {
            soc_reg_field_set(unit, LLS_CONFIG0r, &rval,
                              SPRI_VECT_MODE_ENABLEf, 1);
        }
    }
    SOC_IF_ERROR_RETURN(WRITE_LLS_CONFIG0r(unit, rval));

    /* Enable shaper background refresh */
    rval = 0;
    soc_reg_field_set(unit, LLS_MAX_REFRESH_ENABLEr, &rval, L0_MAX_REFRESH_ENABLEf, 1);
    soc_reg_field_set(unit, LLS_MAX_REFRESH_ENABLEr, &rval, L1_MAX_REFRESH_ENABLEf, 1);
    soc_reg_field_set(unit, LLS_MAX_REFRESH_ENABLEr, &rval, L2_MAX_REFRESH_ENABLEf, 1);
    soc_reg_field_set(unit, LLS_MAX_REFRESH_ENABLEr, &rval, PORT_MAX_REFRESH_ENABLEf, 1);
    soc_reg_field_set(unit, LLS_MAX_REFRESH_ENABLEr, &rval, S0_MAX_REFRESH_ENABLEf, 1);
    soc_reg_field_set(unit, LLS_MAX_REFRESH_ENABLEr, &rval, S1_MAX_REFRESH_ENABLEf, 1);
    SOC_IF_ERROR_RETURN(WRITE_LLS_MAX_REFRESH_ENABLEr(unit, rval));

    rval = 0;
    soc_reg_field_set(unit, LLS_MIN_REFRESH_ENABLEr, &rval, L0_MIN_REFRESH_ENABLEf, 1);
    soc_reg_field_set(unit, LLS_MIN_REFRESH_ENABLEr, &rval, L1_MIN_REFRESH_ENABLEf, 1);
    soc_reg_field_set(unit, LLS_MIN_REFRESH_ENABLEr, &rval, L2_MIN_REFRESH_ENABLEf, 1);
    SOC_IF_ERROR_RETURN(WRITE_LLS_MIN_REFRESH_ENABLEr(unit, rval));

    /* SOC_IF_ERROR_RETURN(_soc_katana2_mmu_init2(unit)); */

    /* RQE configuration */
    rval = 0;
    soc_reg_field_set(unit, RQE_SCHEDULER_CONFIGr, &rval,
                      L0_MCM_MODEf, 1);
    soc_reg_field_set(unit, RQE_SCHEDULER_CONFIGr, &rval,
                      L0_CC_MODEf, 1);
    soc_reg_field_set(unit, RQE_SCHEDULER_CONFIGr, &rval,
                      L0_UCM_MODEf, 1);
    soc_reg_field_set(unit, RQE_SCHEDULER_CONFIGr, &rval,
                      L1_MODEf, 1);
    SOC_IF_ERROR_RETURN(WRITE_RQE_SCHEDULER_CONFIGr(unit, rval));

    rval = 0;
    soc_reg_field_set(unit, RQE_SCHEDULER_WEIGHT_L0_QUEUEr, &rval,
                      WRR_WEIGHTf, 1);
    for(i=0; i<SOC_REG_NUMELS(unit, RQE_SCHEDULER_WEIGHT_L0_QUEUEr); i++) {
        SOC_IF_ERROR_RETURN(WRITE_RQE_SCHEDULER_WEIGHT_L0_QUEUEr(unit, i, 
                                                                 rval));
    }

    rval = 0;
    soc_reg_field_set(unit, RQE_SCHEDULER_WEIGHT_L1_QUEUEr, &rval,
                      WRR_WEIGHTf, 1);
    for(i=0; i<SOC_REG_NUMELS(unit, RQE_SCHEDULER_WEIGHT_L1_QUEUEr); i++) {
        SOC_IF_ERROR_RETURN(WRITE_RQE_SCHEDULER_WEIGHT_L1_QUEUEr(unit, i, 
                                                                 rval));
    }

    /* Enable all ports */
    COMPILER_64_ZERO(r64val);
    COMPILER_64_SET(rval64, 0xf, 0xffffffff); /*Bits 0..25 */
    soc_reg64_field_set(unit, INPUT_PORT_RX_ENABLE_64r, &r64val,
                          INPUT_PORT_RX_ENABLEf, rval64);
    SOC_IF_ERROR_RETURN(WRITE_INPUT_PORT_RX_ENABLE_64r(unit, r64val));
    SOC_IF_ERROR_RETURN(WRITE_THDIEMA_INPUT_PORT_RX_ENABLE_64r(unit, r64val));
    SOC_IF_ERROR_RETURN(WRITE_THDIEXT_INPUT_PORT_RX_ENABLE_64r(unit, r64val));
    SOC_IF_ERROR_RETURN(WRITE_THDIQEN_INPUT_PORT_RX_ENABLE_64r(unit, r64val));
    SOC_IF_ERROR_RETURN(WRITE_THDIRQE_INPUT_PORT_RX_ENABLE_64r(unit, r64val));

    SOC_IF_ERROR_RETURN(WRITE_THDI_BYPASSr(unit, 0));
    SOC_IF_ERROR_RETURN(WRITE_THDIQEN_THDI_BYPASSr(unit, 0));
    SOC_IF_ERROR_RETURN(WRITE_THDIRQE_THDI_BYPASSr(unit, 0));
    SOC_IF_ERROR_RETURN(WRITE_THDIEXT_THDI_BYPASSr(unit, 0));
    SOC_IF_ERROR_RETURN(WRITE_THDIEMA_THDI_BYPASSr(unit, 0));
    SOC_IF_ERROR_RETURN(WRITE_THDO_BYPASSr(unit, 0));

    if (soc_feature(unit, soc_feature_ddr3)) {
        /* Configure EMC */
        SOC_IF_ERROR_RETURN(READ_EMC_CFGr(unit, &rval));
        soc_reg_field_set(unit, EMC_CFGr, &rval, NUM_CISf, 3);
        soc_reg_field_set(unit, EMC_CFGr, &rval, DRAM_SIZEf, 1); 
        SOC_IF_ERROR_RETURN(WRITE_EMC_CFGr(unit, rval));

        SOC_IF_ERROR_RETURN(READ_EMC_FREE_POOL_SIZESr(unit, &rval));
        soc_reg_field_set(unit, EMC_FREE_POOL_SIZESr, &rval, WTFP_SIZEf, 240);
        soc_reg_field_set(unit, EMC_FREE_POOL_SIZESr, &rval, RSFP_SIZEf, 128);
        SOC_IF_ERROR_RETURN(WRITE_EMC_FREE_POOL_SIZESr(unit, rval));

        SOC_IF_ERROR_RETURN(READ_EMC_IWRB_SIZEr(unit, &rval));
        soc_reg_field_set(unit, EMC_IWRB_SIZEr, &rval, IWRB_SIZEf, 2);
        SOC_IF_ERROR_RETURN(WRITE_EMC_IWRB_SIZEr(unit, rval));

        SOC_IF_ERROR_RETURN(READ_EMC_IRRB_THRESHOLDSr(unit, &rval));
        soc_reg_field_set(unit, EMC_IRRB_THRESHOLDSr, &rval, ALL_Q_XOFF_THRESHOLDf, 112);
        soc_reg_field_set(unit, EMC_IRRB_THRESHOLDSr, &rval, ALL_UNDERRUN_Q_XON_THRESHOLDf, 112);
        soc_reg_field_set(unit, EMC_IRRB_THRESHOLDSr, &rval, ALL_Q_XON_THRESHOLDf, 78);
        SOC_IF_ERROR_RETURN(WRITE_EMC_IRRB_THRESHOLDSr(unit, rval));
    }

    /* WRED Configuration */
    if ((dev_id != BCM56450_DEVICE_ID)) { /* And all other 90MHz variants */
        SOC_IF_ERROR_RETURN(READ_WRED_MISCCONFIGr(unit, &rval));
        soc_reg_field_set(unit, WRED_MISCCONFIGr, &rval, BASE_UPDATE_INTERVALf, 7);
        SOC_IF_ERROR_RETURN(WRITE_WRED_MISCCONFIGr(unit, rval));

        SOC_IF_ERROR_RETURN(READ_WRED_PARITY_ERROR_MASKr(unit, &rval));
        soc_reg_field_set(unit, WRED_PARITY_ERROR_MASKr, &rval, UPDATE_INTRPT_MASKf, 0);
        SOC_IF_ERROR_RETURN(WRITE_WRED_PARITY_ERROR_MASKr(unit, rval));

        SOC_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &rval));
        soc_reg_field_set(unit, MISCCONFIGr, &rval, REFRESH_ENf, 1);
        SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, rval));
    }
    SOC_IF_ERROR_RETURN(_soc_katana2_mmu_init3(unit));

    rval = 0xffffff; /* PRIx_GRP = 0x7 */
    SOC_IF_ERROR_RETURN(WRITE_MMU_ENQ_PROFILE_0_PRI_GRP0r(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_MMU_ENQ_PROFILE_0_PRI_GRP1r(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_MMU_ENQ_PROFILE_1_PRI_GRP0r(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_MMU_ENQ_PROFILE_1_PRI_GRP1r(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_MMU_ENQ_PROFILE_2_PRI_GRP0r(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_MMU_ENQ_PROFILE_2_PRI_GRP1r(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_MMU_ENQ_PROFILE_3_PRI_GRP0r(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_MMU_ENQ_PROFILE_3_PRI_GRP1r(unit, rval));

    /* Input port shared space */
/*    rval = 0;
    soc_reg_field_set(unit, USE_SP_SHAREDr, &rval, ENABLEf, 1);
    SOC_IF_ERROR_RETURN(WRITE_USE_SP_SHAREDr(unit, rval));
*/
    rval = 0;
    SOC_IF_ERROR_RETURN(WRITE_THDIEXT_BUFFER_CELL_LIMIT_SPr(unit,0,rval));
    SOC_IF_ERROR_RETURN(WRITE_THDIEMA_BUFFER_CELL_LIMIT_SPr(unit,0,rval));
    soc_reg_field_set(unit, BUFFER_CELL_LIMIT_SPr, &rval, LIMITf, 7475);
    SOC_IF_ERROR_RETURN(WRITE_BUFFER_CELL_LIMIT_SPr(unit,0,rval));
    soc_reg_field_set(unit, BUFFER_CELL_LIMIT_SPr, &rval, LIMITf, 5658);
    SOC_IF_ERROR_RETURN(WRITE_THDIRQE_BUFFER_CELL_LIMIT_SPr(unit,0,rval));
    soc_reg_field_set(unit, BUFFER_CELL_LIMIT_SPr, &rval, LIMITf, 193078);
    SOC_IF_ERROR_RETURN(WRITE_THDIQEN_BUFFER_CELL_LIMIT_SPr(unit,0,rval));

    SOC_IF_ERROR_RETURN(WRITE_BUFFER_CELL_LIMIT_SP_SHAREDr(unit,0));
    SOC_IF_ERROR_RETURN(WRITE_THDIEXT_BUFFER_CELL_LIMIT_SP_SHAREDr(unit,0));
    SOC_IF_ERROR_RETURN(WRITE_THDIEMA_BUFFER_CELL_LIMIT_SP_SHAREDr(unit,0));
    SOC_IF_ERROR_RETURN(WRITE_THDIRQE_BUFFER_CELL_LIMIT_SP_SHAREDr(unit,0));
    SOC_IF_ERROR_RETURN(WRITE_THDIQEN_BUFFER_CELL_LIMIT_SP_SHAREDr(unit,0));

    rval = 0;
    SOC_IF_ERROR_RETURN(WRITE_THDIEXT_CELL_RESET_LIMIT_OFFSET_SPr(unit, 0, rval));
    SOC_IF_ERROR_RETURN(WRITE_THDIEMA_CELL_RESET_LIMIT_OFFSET_SPr(unit, 0, rval));
    soc_reg_field_set(unit, CELL_RESET_LIMIT_OFFSET_SPr, &rval, OFFSETf, 3);
    SOC_IF_ERROR_RETURN(WRITE_THDIRQE_CELL_RESET_LIMIT_OFFSET_SPr(unit, 0, rval));
    soc_reg_field_set(unit, CELL_RESET_LIMIT_OFFSET_SPr, &rval, OFFSETf, 63);
    SOC_IF_ERROR_RETURN(WRITE_CELL_RESET_LIMIT_OFFSET_SPr(unit, 0, rval));
    soc_reg_field_set(unit, CELL_RESET_LIMIT_OFFSET_SPr, &rval, OFFSETf, 81);
    SOC_IF_ERROR_RETURN(WRITE_THDIQEN_CELL_RESET_LIMIT_OFFSET_SPr(unit, 0, rval));

    /* Input port per-device global headroom */
    rval = 0;
    SOC_IF_ERROR_RETURN(WRITE_THDIEXT_GLOBAL_HDRM_LIMITr(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_THDIEMA_GLOBAL_HDRM_LIMITr(unit, rval));
    soc_reg_field_set(unit, GLOBAL_HDRM_LIMITr, &rval, GLOBAL_HDRM_LIMITf, 108);
    SOC_IF_ERROR_RETURN(WRITE_GLOBAL_HDRM_LIMITr(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_THDIRQE_GLOBAL_HDRM_LIMITr(unit, rval));
    soc_reg_field_set(unit, GLOBAL_HDRM_LIMITr, &rval, GLOBAL_HDRM_LIMITf, 2916);
    SOC_IF_ERROR_RETURN(WRITE_THDIQEN_GLOBAL_HDRM_LIMITr(unit, rval));

    SOC_IF_ERROR_RETURN(READ_THDO_MISCCONFIGr(unit, &rval));
    soc_reg_field_set(unit, THDO_MISCCONFIGr, &rval, STAT_CLEARf, 0);
    soc_reg_field_set(unit, THDO_MISCCONFIGr, &rval, PARITY_CHK_ENf, 1);
    soc_reg_field_set(unit, THDO_MISCCONFIGr, &rval, PARITY_GEN_ENf, 1);
    SOC_IF_ERROR_RETURN(WRITE_THDO_MISCCONFIGr(unit, rval));

    SOC_IF_ERROR_RETURN(READ_OP_THR_CONFIGr(unit, &rval));
    soc_reg_field_set(unit, OP_THR_CONFIGr, &rval, EARLY_E2E_SELECTf, 0);
    SOC_IF_ERROR_RETURN(WRITE_OP_THR_CONFIGr(unit, rval));

    SOC_IF_ERROR_RETURN(READ_OP_BUFFER_SHARED_LIMIT_CELLE_POOL0r(unit, &rval));
    soc_reg_field_set(unit, OP_BUFFER_SHARED_LIMIT_CELLE_POOL0r, &rval,
                           OP_BUFFER_SHARED_LIMIT_CELLEf, 0);
    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_SHARED_LIMIT_CELLE_POOL0r(unit, rval));

    SOC_IF_ERROR_RETURN(READ_OP_BUFFER_SHARED_LIMIT_QENTRYr(unit, &rval));
    soc_reg_field_set(unit, OP_BUFFER_SHARED_LIMIT_QENTRYr, &rval,
                           OP_BUFFER_SHARED_LIMIT_QENTRYf, 261712);
    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_SHARED_LIMIT_QENTRYr(unit, rval));

    SOC_IF_ERROR_RETURN(READ_OP_BUFFER_SHARED_LIMIT_THDORQEQr(unit, &rval));
    soc_reg_field_set(unit, OP_BUFFER_SHARED_LIMIT_THDORQEQr, &rval,
                         OP_BUFFER_SHARED_LIMITf, 8191);
    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_SHARED_LIMIT_THDORQEQr(unit, rval));

    SOC_IF_ERROR_RETURN(READ_OP_BUFFER_SHARED_LIMIT_THDOEMAr(unit, &rval));
    soc_reg_field_set(unit, OP_BUFFER_SHARED_LIMIT_THDOEMAr, &rval,
                       OP_BUFFER_SHARED_LIMITf, 0);
    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_SHARED_LIMIT_THDOEMAr(unit, rval));

    SOC_IF_ERROR_RETURN(READ_OP_BUFFER_SHARED_LIMIT_RESUME_CELLE_POOL0r(unit, &rval));
    soc_reg_field_set(unit, OP_BUFFER_SHARED_LIMIT_RESUME_CELLE_POOL0r, &rval,
                         OP_BUFFER_SHARED_LIMIT_RESUME_CELLEf, 0);
    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_SHARED_LIMIT_RESUME_CELLE_POOL0r(unit, rval));

    SOC_IF_ERROR_RETURN(READ_OP_BUFFER_SHARED_LIMIT_RESUME_QENTRYr(unit, &rval));
    soc_reg_field_set(unit, OP_BUFFER_SHARED_LIMIT_RESUME_QENTRYr, &rval,
                         OP_BUFFER_SHARED_LIMIT_RESUME_QENTRYf, 261523);
    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_SHARED_LIMIT_RESUME_QENTRYr(unit, rval));

    SOC_IF_ERROR_RETURN(READ_OP_BUFFER_SHARED_LIMIT_RESUME_THDORQEQr(unit, &rval));
    soc_reg_field_set(unit, OP_BUFFER_SHARED_LIMIT_RESUME_THDORQEQr, &rval,
                       OP_BUFFER_SHARED_LIMIT_RESUMEf, 8185);
    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_SHARED_LIMIT_RESUME_THDORQEQr(unit, rval));

    SOC_IF_ERROR_RETURN(READ_OP_BUFFER_SHARED_LIMIT_RESUME_THDOEMAr(unit, &rval));
    soc_reg_field_set(unit, OP_BUFFER_SHARED_LIMIT_RESUME_THDOEMAr, &rval,
                     OP_BUFFER_SHARED_LIMIT_RESUMEf, 0);
    SOC_IF_ERROR_RETURN(WRITE_OP_BUFFER_SHARED_LIMIT_RESUME_THDOEMAr(unit, rval));

    for(i=0; i<SOC_REG_NUMELS(unit, OP_QUEUE_CONFIG_THDORQEIr); i++) {
        SOC_IF_ERROR_RETURN(READ_OP_QUEUE_CONFIG_THDORQEIr(unit, i, &rval));
        soc_reg_field_set(unit, OP_QUEUE_CONFIG_THDORQEIr, &rval, Q_LIMIT_ENABLEf, 0);
        soc_reg_field_set(unit, OP_QUEUE_CONFIG_THDORQEIr, &rval, Q_LIMIT_DYNAMICf, 0);
        soc_reg_field_set(unit, OP_QUEUE_CONFIG_THDORQEIr, &rval, Q_SHARED_LIMITf, 7079);
        SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_CONFIG_THDORQEIr(unit, i, rval));
    }
    for(i=0; i<SOC_REG_NUMELS(unit, OP_QUEUE_CONFIG_THDORQEEr); i++) {
        SOC_IF_ERROR_RETURN(READ_OP_QUEUE_CONFIG_THDORQEEr(unit, i, &rval));
        soc_reg_field_set(unit, OP_QUEUE_CONFIG_THDORQEEr, &rval, Q_LIMIT_ENABLEf, 0);
        soc_reg_field_set(unit, OP_QUEUE_CONFIG_THDORQEEr, &rval, Q_LIMIT_DYNAMICf, 0);
        soc_reg_field_set(unit, OP_QUEUE_CONFIG_THDORQEEr, &rval, Q_SHARED_LIMITf, 0);
        SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_CONFIG_THDORQEEr(unit, i, rval));
    }
    for(i=0; i<SOC_REG_NUMELS(unit, OP_QUEUE_CONFIG_THDORQEQr); i++) {
        SOC_IF_ERROR_RETURN(READ_OP_QUEUE_CONFIG_THDORQEQr(unit, i, &rval));
        soc_reg_field_set(unit, OP_QUEUE_CONFIG_THDORQEQr, &rval, Q_LIMIT_ENABLEf, 0);
        soc_reg_field_set(unit, OP_QUEUE_CONFIG_THDORQEQr, &rval, Q_LIMIT_DYNAMICf, 0);
        soc_reg_field_set(unit, OP_QUEUE_CONFIG_THDORQEQr, &rval, Q_SHARED_LIMITf, 8191);
        SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_CONFIG_THDORQEQr(unit, i, rval));
    }
    for(i=0; i<SOC_REG_NUMELS(unit, OP_QUEUE_CONFIG_THDOEMAr); i++) {
        SOC_IF_ERROR_RETURN(READ_OP_QUEUE_CONFIG_THDOEMAr(unit, i, &rval));
        soc_reg_field_set(unit, OP_QUEUE_CONFIG_THDOEMAr, &rval, Q_LIMIT_ENABLEf, 0);
        soc_reg_field_set(unit, OP_QUEUE_CONFIG_THDOEMAr, &rval, Q_LIMIT_DYNAMICf, 0);
        soc_reg_field_set(unit, OP_QUEUE_CONFIG_THDOEMAr, &rval, Q_SHARED_LIMITf, 0);
        SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_CONFIG_THDOEMAr(unit, i, rval));
    }

    for(i=0; i<SOC_REG_NUMELS(unit, OP_QUEUE_CONFIG1_THDORQEIr); i++) {
        SOC_IF_ERROR_RETURN(READ_OP_QUEUE_CONFIG1_THDORQEIr(unit, i, &rval));
        soc_reg_field_set(unit, OP_QUEUE_CONFIG1_THDORQEIr, &rval, Q_COLOR_ENABLEf, 0);
        soc_reg_field_set(unit, OP_QUEUE_CONFIG1_THDORQEIr, &rval, Q_COLOR_DYNAMICf, 0);
        soc_reg_field_set(unit, OP_QUEUE_CONFIG1_THDORQEIr, &rval, Q_MINf, 0);
        SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_CONFIG1_THDORQEIr(unit, i, rval));
    }
    for(i=0; i<SOC_REG_NUMELS(unit, OP_QUEUE_CONFIG1_THDORQEEr); i++) {
        SOC_IF_ERROR_RETURN(READ_OP_QUEUE_CONFIG1_THDORQEEr(unit, i, &rval));
        soc_reg_field_set(unit, OP_QUEUE_CONFIG1_THDORQEEr, &rval, Q_COLOR_ENABLEf, 0);
        soc_reg_field_set(unit, OP_QUEUE_CONFIG1_THDORQEEr, &rval, Q_COLOR_DYNAMICf, 0);
        soc_reg_field_set(unit, OP_QUEUE_CONFIG1_THDORQEEr, &rval, Q_MINf, 0);
        SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_CONFIG1_THDORQEEr(unit, i, rval));
    }
    for(i=0; i<SOC_REG_NUMELS(unit, OP_QUEUE_CONFIG1_THDORQEQr); i++) {
        SOC_IF_ERROR_RETURN(READ_OP_QUEUE_CONFIG1_THDORQEQr(unit, i, &rval));
        soc_reg_field_set(unit, OP_QUEUE_CONFIG1_THDORQEQr, &rval, Q_COLOR_ENABLEf, 0);
        soc_reg_field_set(unit, OP_QUEUE_CONFIG1_THDORQEQr, &rval, Q_COLOR_DYNAMICf, 0);
        soc_reg_field_set(unit, OP_QUEUE_CONFIG1_THDORQEQr, &rval, Q_MINf, 0);
        SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_CONFIG1_THDORQEQr(unit, i, rval));
    }
    for(i=0; i<SOC_REG_NUMELS(unit, OP_QUEUE_CONFIG1_THDOEMAr); i++) {
        SOC_IF_ERROR_RETURN(READ_OP_QUEUE_CONFIG1_THDOEMAr(unit, i, &rval));
        soc_reg_field_set(unit, OP_QUEUE_CONFIG1_THDOEMAr, &rval, Q_COLOR_ENABLEf, 0);
        soc_reg_field_set(unit, OP_QUEUE_CONFIG1_THDOEMAr, &rval, Q_COLOR_DYNAMICf, 0);
        soc_reg_field_set(unit, OP_QUEUE_CONFIG1_THDOEMAr, &rval, Q_MINf, 0);
        SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_CONFIG1_THDOEMAr(unit, i, rval));
    }

    for(i=0; i<SOC_REG_NUMELS(unit, OP_QUEUE_RESET_OFFSET_THDORQEIr); i++) {
        SOC_IF_ERROR_RETURN(READ_OP_QUEUE_RESET_OFFSET_THDORQEIr(unit, i, &rval));
        soc_reg_field_set(unit, OP_QUEUE_RESET_OFFSET_THDORQEIr, &rval, Q_RESET_OFFSETf, 2);
        SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_RESET_OFFSET_THDORQEIr(unit, i, rval));
    }
    for(i=0; i<SOC_REG_NUMELS(unit, OP_QUEUE_RESET_OFFSET_THDORQEEr); i++) {
        SOC_IF_ERROR_RETURN(READ_OP_QUEUE_RESET_OFFSET_THDORQEEr(unit, i, &rval));
        soc_reg_field_set(unit, OP_QUEUE_RESET_OFFSET_THDORQEEr, &rval, Q_RESET_OFFSETf, 0);
        SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_RESET_OFFSET_THDORQEEr(unit, i, rval));
    }
    for(i=0; i<SOC_REG_NUMELS(unit, OP_QUEUE_RESET_OFFSET_THDORQEQr); i++) {
        SOC_IF_ERROR_RETURN(READ_OP_QUEUE_RESET_OFFSET_THDORQEQr(unit, i, &rval));
        soc_reg_field_set(unit, OP_QUEUE_RESET_OFFSET_THDORQEQr, &rval, Q_RESET_OFFSETf, 1);
        SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_RESET_OFFSET_THDORQEQr(unit, i, rval));
    }
    for(i=0; i<SOC_REG_NUMELS(unit, OP_QUEUE_RESET_OFFSET_THDOEMAr); i++) {
        SOC_IF_ERROR_RETURN(READ_OP_QUEUE_RESET_OFFSET_THDOEMAr(unit, i, &rval));
        soc_reg_field_set(unit, OP_QUEUE_RESET_OFFSET_THDOEMAr, &rval, Q_RESET_OFFSETf, 0);
        SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_RESET_OFFSET_THDOEMAr(unit, i, rval));
    }
    for(i=0; i<400; i++) { /* 8 x 41 ports  + 72 CPU */
        SOC_IF_ERROR_RETURN(READ_MMU_THDO_QCONFIG_CELLm(unit,MEM_BLOCK_ANY,i,&entry));
        soc_mem_field32_set(unit, MMU_THDO_QCONFIG_CELLm, &entry,
                                    Q_SHARED_LIMIT_CELLf, 10490);
        soc_mem_field32_set(unit, MMU_THDO_QCONFIG_CELLm, &entry,
                                    Q_MIN_CELLf, 0);
        soc_mem_field32_set(unit, MMU_THDO_QCONFIG_CELLm, &entry,
                                    Q_LIMIT_ENABLE_CELLf, 0);
        soc_mem_field32_set(unit, MMU_THDO_QCONFIG_CELLm, &entry,
                                    Q_LIMIT_DYNAMIC_CELLf, 0);
        soc_mem_field32_set(unit, MMU_THDO_QCONFIG_CELLm, &entry,
                                    Q_COLOR_ENABLE_CELLf, 0);
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDO_QCONFIG_CELLm(unit,MEM_BLOCK_ANY,i,&entry));

        SOC_IF_ERROR_RETURN(READ_MMU_THDO_QOFFSET_CELLm(unit,MEM_BLOCK_ANY,i,&entry));
        soc_mem_field32_set(unit, MMU_THDO_QOFFSET_CELLm, &entry,
                                    RESET_OFFSET_CELLf, 2);
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDO_QOFFSET_CELLm(unit,MEM_BLOCK_ANY,i,&entry));

        SOC_IF_ERROR_RETURN(READ_MMU_THDO_QCONFIG_QENTRYm(unit,MEM_BLOCK_ANY,i,&entry));
        soc_mem_field32_set(unit, MMU_THDO_QCONFIG_QENTRYm, &entry,
                                    Q_SHARED_LIMIT_QENTRYf, 261712);
        soc_mem_field32_set(unit, MMU_THDO_QCONFIG_QENTRYm, &entry,
                                    Q_MIN_QENTRYf, 0);
        soc_mem_field32_set(unit, MMU_THDO_QCONFIG_QENTRYm, &entry,
                                    Q_LIMIT_ENABLE_QENTRYf, 0);
        soc_mem_field32_set(unit, MMU_THDO_QCONFIG_QENTRYm, &entry,
                                    Q_LIMIT_DYNAMIC_QENTRYf, 0);
        soc_mem_field32_set(unit, MMU_THDO_QCONFIG_QENTRYm, &entry,
                                    Q_COLOR_ENABLE_QENTRYf, 0);
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDO_QCONFIG_QENTRYm(unit,MEM_BLOCK_ANY,i,&entry));

        SOC_IF_ERROR_RETURN(READ_MMU_THDO_QOFFSET_QENTRYm(unit,MEM_BLOCK_ANY,i,&entry));
        soc_mem_field32_set(unit, MMU_THDO_QOFFSET_QENTRYm, &entry,
                                    RESET_OFFSET_QENTRYf, 1);
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDO_QOFFSET_QENTRYm(unit,MEM_BLOCK_ANY,i,&entry));

        SOC_IF_ERROR_RETURN(READ_MMU_THDO_OPNCONFIG_CELLm(unit,MEM_BLOCK_ANY,i,&entry));
        soc_mem_field32_set(unit, MMU_THDO_OPNCONFIG_CELLm, &entry,
                                    OPN_SHARED_LIMIT_CELLf, 10490);
        soc_mem_field32_set(unit, MMU_THDO_OPNCONFIG_CELLm, &entry,
                                    OPN_SHARED_RESET_VALUE_CELLf, 10472);
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDO_OPNCONFIG_CELLm(unit,MEM_BLOCK_ANY,i,&entry));

        SOC_IF_ERROR_RETURN(READ_MMU_THDO_OPNCONFIG_QENTRYm(unit,MEM_BLOCK_ANY,i,&entry));
        soc_mem_field32_set(unit, MMU_THDO_OPNCONFIG_QENTRYm, &entry,
                                    OPN_SHARED_LIMIT_QENTRYf, 261712);
        soc_mem_field32_set(unit, MMU_THDO_OPNCONFIG_QENTRYm, &entry,
                                    OPN_SHARED_RESET_VALUE_QENTRYf, 261710);
        soc_mem_field32_set(unit, MMU_THDO_OPNCONFIG_QENTRYm, &entry,
                                    PORT_LIMIT_ENABLE_QENTRYf, 0);
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDO_OPNCONFIG_QENTRYm(unit,MEM_BLOCK_ANY,i,&entry));
    }

    /* Initialize MMU internal/external aging limit memory */
    count = soc_mem_index_count(unit, MMU_AGING_LMT_INTm); 
    sal_memset(&age_entry, 0, sizeof(mmu_aging_lmt_int_entry_t));
    for (i=0; i < count; i++) {
        SOC_IF_ERROR_RETURN(WRITE_MMU_AGING_LMT_INTm(unit,
                            MEM_BLOCK_ANY, i, &age_entry));
    }

    count = soc_mem_index_count(unit, MMU_AGING_LMT_EXTm); 
    for (i=0; i < count; i++) {
        SOC_IF_ERROR_RETURN(WRITE_MMU_AGING_LMT_EXTm(unit,
                            MEM_BLOCK_ANY, i, &age_entry));
    }

    return SOC_E_NONE;
}

STATIC int
_soc_katana2_age_timer_get(int unit, int *age_seconds, int *enabled)
{
    uint32 rval;

    SOC_IF_ERROR_RETURN(READ_L2_AGE_TIMERr(unit, &rval));
    *enabled = soc_reg_field_get(unit, L2_AGE_TIMERr, rval, AGE_ENAf);
    *age_seconds = soc_reg_field_get(unit, L2_AGE_TIMERr, rval, AGE_VALf);

    return SOC_E_NONE;
}

STATIC int
_soc_katana2_age_timer_max_get(int unit, int *max_seconds)
{
    *max_seconds =
        soc_reg_field_get(unit, L2_AGE_TIMERr, 0xffffffff, AGE_VALf);

    return SOC_E_NONE;
}

STATIC int
_soc_katana2_age_timer_set(int unit, int age_seconds, int enable)
{
    uint32 rval;

    rval = 0;
    soc_reg_field_set(unit, L2_AGE_TIMERr, &rval, AGE_ENAf, enable);
    soc_reg_field_set(unit, L2_AGE_TIMERr, &rval, AGE_VALf, age_seconds);
    SOC_IF_ERROR_RETURN(WRITE_L2_AGE_TIMERr(unit, rval));

    return SOC_E_NONE;
}

int kt2_linkphy_port[2][4] = {
    {27, 32, 33, 34}, /* {T,R}XLP0 */
    {28, 29, 30, 31}  /* {T,R}XLP1 */
};

int 
soc_kt2_linkphy_port_reg_blk_idx_get(
    int unit, int port, int blktype, int *block, int *index)
{
    int i,j;
    int blk = -1;
    int idx = -1;

    for (i=0; i<2; i++) {
        for(j=0; j<4; j++) {
            if(port == kt2_linkphy_port[i][j]) {
                    blk = i;
                    idx = j;
                    break;
            }
        }
    }

    if (blktype == SOC_BLK_TXLP) {
        if (blk == 0) {
            blk = TXLP0_BLOCK(unit);
        } else if (blk == 1) {
            blk = TXLP1_BLOCK(unit);
        }
    } else if (blktype == SOC_BLK_RXLP) {
        if (blk == 0) {
            blk = RXLP0_BLOCK(unit);
        } else if (blk == 1) {
            blk = RXLP1_BLOCK(unit);
        }
    } else {
        return SOC_E_PARAM;
    }

    if (block != NULL) {
        *block = blk;
    }
    if (index != NULL) {
        *index = idx;
    }
    if ((blk == -1) || (idx == -1)) {
        return SOC_E_NOT_FOUND;
    }
    return SOC_E_NONE;
}

int
soc_kt2_linkphy_port_blk_idx_get(
    int unit, int port, int *block, int *index)
{
    int i,j;
    int blk = -1;
    int idx = -1;

    for (i=0; i<2; i++) {
        for(j=0; j<4; j++) {
            if(port == kt2_linkphy_port[i][j]) {
                    blk = i;
                    idx = j;
                    break;
            }
        }
    }

    if (block != NULL) {
        *block = blk;
    }
    if (index != NULL) {
        *index = idx;
    }
    if ((blk == -1) || (idx == -1)) {
        return SOC_E_NOT_FOUND;
    }
    return SOC_E_NONE;
}

int soc_kt2_linkphy_get_portid(int unit, int block, int index) {
    if (index > 3) {
        return -1;
    }
    if ((block == TXLP0_BLOCK(unit)) || (block == RXLP0_BLOCK(unit))) {
        return kt2_linkphy_port[0][index];
    } else if ((block == TXLP1_BLOCK(unit)) || (block == RXLP1_BLOCK(unit))) {
            return kt2_linkphy_port[1][index];
    }
    return -1;
}


/*
 * cpu port (mmu port 0): 48 queues (0-47)
 * loopback port (mmu port 35): 8 queues (0-7)
 * hg ports : 24 queues
 * other ports : 8 queues, vlan levels : 64                                    
 */

int
soc_katana2_num_cosq_init(int unit)
{
    soc_info_t *si;
    int port, prev_port, vlan_queue_enable = 0;
    int vlan_port = 0;
    
    si = &SOC_INFO(unit);
    prev_port = 0;

    for (port=0;port < 42;port++) {
        si->port_num_ext_cosq[port] = 0;
        si->port_num_cosq[port] = 0;
        /* Supports max 8 ports for vlan queues */
        vlan_queue_enable =
                   soc_property_port_get(unit, port, spn_VLAN_QUEUE_ENABLE, 0);

        if (vlan_queue_enable != 0) {
            if (vlan_port < 8 && !IS_CPU_PORT(unit, port)) {
                vlan_port++;
            } else {
                vlan_queue_enable = 0;  
            }
        }

        if (vlan_queue_enable != 0) {
            si->port_num_ext_cosq[port] = soc_property_get(unit,
                                            spn_VLAN_QUEUE_LEVELS_MAX, 56);
            if (si->port_num_ext_cosq[port] > 56) {
                si->port_num_ext_cosq[port] = 56;
            }
        }
        if (IS_CPU_PORT(unit, port)) {
            si->port_cosq_base[port] = 0;
            si->port_uc_cosq_base[port] = 0;
            si->port_num_uc_cosq[port] = 72;
            si->port_num_ext_cosq[port] = 0;
        } else {
            si->port_cosq_base[port] = si->port_cosq_base[prev_port] +
                                       si->port_num_uc_cosq[prev_port];
            si->port_uc_cosq_base[port] = si->port_cosq_base[port];
            if (IS_HG_PORT(unit, port)) {
                si->port_num_uc_cosq[port] = ((si->port_num_ext_cosq[port] == 0) ? \
                                     24 : ((si->port_num_ext_cosq[port] + 24) > 64) ? \
                                     64 : (si->port_num_ext_cosq[port] + 24));
            } else {
                si->port_num_uc_cosq[port] = ((si->port_num_ext_cosq[port] == 0) ?
                                     8 : ((si->port_num_ext_cosq[port] + 8) > 64) ?
                                     64 : (si->port_num_ext_cosq[port] + 8));
            }
        }
        prev_port = port;
        if (soc_cm_debug_check(DK_VERBOSE)) {
            soc_cm_print("\nport num %d cosq_base %d num_cosq %d",
                   port, si->port_cosq_base[port], si->port_num_uc_cosq[port]);
        }
    }
    return SOC_E_NONE;
}

soc_blk_ctr_reg_desc_t _soc_kt2_blk_ctr_reg_desc[] =
{
    {
        SOC_BLK_RXLP,
        {
            {RXLP_UNEXPECTED_ETHERTYPEr, 1, 1, 0},
            {RXLP_UNKNOWN_STREAM_IDr, 1, 1, 0},
            {RXLP_DFC_FRAMESr, 1, 1, 0},
            {RXLP_DFC_FRAME_UNEXPECTED_MACDAr, 1, 1, 0},
            {RXLP_DFC_FRAME_UNEXPECTED_MACSAr, 1, 1, 0},
            {RXLP_DFC_HEADER_OPCODE_ERRORr, 1, 1, 0},
            {RXLP_DFC_HEADER_TIME_ERRORr, 1, 1, 0},
            {RXLP_DFC_LENGTH_ERRORr, 1, 1, 0},
            {INVALIDr, 0, 0}
        }
    }
};

void
soc_kt2_blk_counter_config(int unit)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    /* TimeBeing Skipping it due to some NACK issue */
    if (SAL_BOOT_QUICKTURN) {
        soc_cm_print("QUICKTURN:ATTN: Skipping RXLP Counter(NACK issue)\n");
        soc->blk_ctr_desc_count = 0;
    } else {
        soc->blk_ctr_desc_count = COUNTOF(_soc_kt2_blk_ctr_reg_desc);
    }
    soc->blk_ctr_desc = _soc_kt2_blk_ctr_reg_desc;
}

/*
 * Katana chip driver functions.
 */
soc_functions_t soc_katana2_drv_funs = {
    _soc_katana2_misc_init,
    _soc_katana2_mmu_init,
    _soc_katana2_age_timer_get,
    _soc_katana2_age_timer_max_get,
    _soc_katana2_age_timer_set,
};

/* KT2 OAM */
void
soc_kt2_oam_handler_register(int unit, soc_kt2_oam_handler_t handler)
{
#if 0
    kt2_oam_handler[unit] = handler;
    soc_intr_enable(unit, IRQ_MEM_FAIL);
#endif
}

#endif /* BCM_KATANA_SUPPORT */
