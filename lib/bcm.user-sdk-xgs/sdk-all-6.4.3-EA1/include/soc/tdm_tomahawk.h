/*
 * $Id: tdm_tomahawk.h,v 1.1.10.25 Broadcom SDK $
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
 * File:        tdm_tomahawk.h
 * Purpose:
 * Requires:
 */


enum port_speed {SPEED_0=0, SPEED_10M=10, SPEED_20M=20, SPEED_25M=25, SPEED_33M=33, SPEED_40M=40, SPEED_50M=50, SPEED_100M=100, SPEED_100M_FX=101, SPEED_120M=120,SPEED_400M=400, SPEED_1G=1000, SPEED_1G_FX=1001, SPEED_1p2G=1200, SPEED_2G=2000, SPEED_2p5G=2500, SPEED_4G=4000, SPEED_5G=5000, SPEED_7p5G=7500, SPEED_10G=10000, SPEED_10G_DUAL=10001, SPEED_10G_XAUI=10002, SPEED_12G=12000, SPEED_12p5G=12500, SPEED_13G=13000, SPEED_15G=15000, SPEED_16G=16000, SPEED_20G=20000, SPEED_21G=21000, SPEED_21G_DUAL=21010, SPEED_24G=24000, SPEED_25G=25000, SPEED_27G=27000, SPEED_30G=30000, SPEED_40G=40000, SPEED_42G=40005, SPEED_42G_HG2=42000, SPEED_50G=50000, SPEED_53G=53000, SPEED_75G=75000, SPEED_82G=82000, SPEED_100G=100000, SPEED_106G=106000, SPEED_120G=120000, SPEED_126G=126000};

/* Types of ports based on traffic */
#define PORT_ETHERNET 998
#define PORT_HIGIG2 999

/* IDB: Idle slot guaranteed for refresh */
/* Oversub token */
#define OVSB_TOKEN 250
/* IDB: Idle slot for memreset, L2 management, sbus, other */
/* MMU: No explicit definition */
#define IDL1_TOKEN 251
/* MMU: Purge */
#define IDL2_TOKEN 252
/* IDB: NULL slot, no pick, no opportunistic */
/* MMU: NULL slot, no pick, no opportunistic */
#define NULL_TOKEN 253

/* Default number of PM in chip */
#define NUM_TSC 32

/* Number of physical ports plus CPU and loopback */
#define _TH_NUM_EXT_PORTS 136

/* Length of linerate speed groups */
#define _TH_LR_GROUP_LEN 256

/* Length of oversub speed groups */
#define _TH_OS_GROUP_LEN 12

/* Number of port modules */
#define _TH_NUM_PORT_MODULES NUM_TSC+1

typedef struct {
	int idb_tdm_tbl_0[_TH_LR_GROUP_LEN];
	int idb_tdm_ovs_0_a[_TH_OS_GROUP_LEN];
	int idb_tdm_ovs_0_b[_TH_OS_GROUP_LEN];
	int idb_tdm_ovs_0_c[_TH_OS_GROUP_LEN];
	int idb_tdm_ovs_0_d[_TH_OS_GROUP_LEN];
	int idb_tdm_ovs_0_e[_TH_OS_GROUP_LEN];
	int idb_tdm_ovs_0_f[_TH_OS_GROUP_LEN];
	int idb_tdm_ovs_0_g[_TH_OS_GROUP_LEN];
	int idb_tdm_ovs_0_h[_TH_OS_GROUP_LEN];
	int idb_tdm_tbl_1[_TH_LR_GROUP_LEN];
	int idb_tdm_ovs_1_a[_TH_OS_GROUP_LEN];
	int idb_tdm_ovs_1_b[_TH_OS_GROUP_LEN];
	int idb_tdm_ovs_1_c[_TH_OS_GROUP_LEN];
	int idb_tdm_ovs_1_d[_TH_OS_GROUP_LEN];
	int idb_tdm_ovs_1_e[_TH_OS_GROUP_LEN];
	int idb_tdm_ovs_1_f[_TH_OS_GROUP_LEN];
	int idb_tdm_ovs_1_g[_TH_OS_GROUP_LEN];
	int idb_tdm_ovs_1_h[_TH_OS_GROUP_LEN];
	int idb_tdm_tbl_2[_TH_LR_GROUP_LEN];
	int idb_tdm_ovs_2_a[_TH_OS_GROUP_LEN];
	int idb_tdm_ovs_2_b[_TH_OS_GROUP_LEN];
	int idb_tdm_ovs_2_c[_TH_OS_GROUP_LEN];
	int idb_tdm_ovs_2_d[_TH_OS_GROUP_LEN];
	int idb_tdm_ovs_2_e[_TH_OS_GROUP_LEN];
	int idb_tdm_ovs_2_f[_TH_OS_GROUP_LEN];
	int idb_tdm_ovs_2_g[_TH_OS_GROUP_LEN];
	int idb_tdm_ovs_2_h[_TH_OS_GROUP_LEN];
	int idb_tdm_tbl_3[_TH_LR_GROUP_LEN];
	int idb_tdm_ovs_3_a[_TH_OS_GROUP_LEN];
	int idb_tdm_ovs_3_b[_TH_OS_GROUP_LEN];
	int idb_tdm_ovs_3_c[_TH_OS_GROUP_LEN];
	int idb_tdm_ovs_3_d[_TH_OS_GROUP_LEN];
	int idb_tdm_ovs_3_e[_TH_OS_GROUP_LEN];
	int idb_tdm_ovs_3_f[_TH_OS_GROUP_LEN];
	int idb_tdm_ovs_3_g[_TH_OS_GROUP_LEN];
	int idb_tdm_ovs_3_h[_TH_OS_GROUP_LEN];
	int mmu_tdm_tbl_0[_TH_LR_GROUP_LEN];
	int mmu_tdm_ovs_0_a[_TH_OS_GROUP_LEN];
	int mmu_tdm_ovs_0_b[_TH_OS_GROUP_LEN];
	int mmu_tdm_ovs_0_c[_TH_OS_GROUP_LEN];
	int mmu_tdm_ovs_0_d[_TH_OS_GROUP_LEN];
	int mmu_tdm_ovs_0_e[_TH_OS_GROUP_LEN];
	int mmu_tdm_ovs_0_f[_TH_OS_GROUP_LEN];
	int mmu_tdm_ovs_0_g[_TH_OS_GROUP_LEN];
	int mmu_tdm_ovs_0_h[_TH_OS_GROUP_LEN];
	int mmu_tdm_tbl_1[_TH_LR_GROUP_LEN];
	int mmu_tdm_ovs_1_a[_TH_OS_GROUP_LEN];
	int mmu_tdm_ovs_1_b[_TH_OS_GROUP_LEN];
	int mmu_tdm_ovs_1_c[_TH_OS_GROUP_LEN];
	int mmu_tdm_ovs_1_d[_TH_OS_GROUP_LEN];
	int mmu_tdm_ovs_1_e[_TH_OS_GROUP_LEN];
	int mmu_tdm_ovs_1_f[_TH_OS_GROUP_LEN];
	int mmu_tdm_ovs_1_g[_TH_OS_GROUP_LEN];
	int mmu_tdm_ovs_1_h[_TH_OS_GROUP_LEN];
	int mmu_tdm_tbl_2[_TH_LR_GROUP_LEN];
	int mmu_tdm_ovs_2_a[_TH_OS_GROUP_LEN];
	int mmu_tdm_ovs_2_b[_TH_OS_GROUP_LEN];
	int mmu_tdm_ovs_2_c[_TH_OS_GROUP_LEN];
	int mmu_tdm_ovs_2_d[_TH_OS_GROUP_LEN];
	int mmu_tdm_ovs_2_e[_TH_OS_GROUP_LEN];
	int mmu_tdm_ovs_2_f[_TH_OS_GROUP_LEN];
	int mmu_tdm_ovs_2_g[_TH_OS_GROUP_LEN];
	int mmu_tdm_ovs_2_h[_TH_OS_GROUP_LEN];
	int mmu_tdm_tbl_3[_TH_LR_GROUP_LEN];
	int mmu_tdm_ovs_3_a[_TH_OS_GROUP_LEN];
	int mmu_tdm_ovs_3_b[_TH_OS_GROUP_LEN];
	int mmu_tdm_ovs_3_c[_TH_OS_GROUP_LEN];
	int mmu_tdm_ovs_3_d[_TH_OS_GROUP_LEN];
	int mmu_tdm_ovs_3_e[_TH_OS_GROUP_LEN];
	int mmu_tdm_ovs_3_f[_TH_OS_GROUP_LEN];
	int mmu_tdm_ovs_3_g[_TH_OS_GROUP_LEN];
	int mmu_tdm_ovs_3_h[_TH_OS_GROUP_LEN];
} th_tdm_pipes_t;


typedef struct {
  enum port_speed speed[_TH_NUM_EXT_PORTS];
  int clk_freq;
  int port_rates_array[_TH_NUM_EXT_PORTS];
  int pm_encap_type[_TH_NUM_PORT_MODULES];
} th_tdm_globals_t;

extern int TH_set_tdm_tbl(th_tdm_globals_t *tdm_globals, th_tdm_pipes_t *tdm_pipe_tables);

