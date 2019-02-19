/*
 * $Id: tdm_th_soc.h$
 * $Copyright: Copyright 2015 Broadcom Corporation.
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
 * All Rights Reserved.$
 *
 * TDM soc header for BCM56960
 */

#ifndef TDM_BCM56960_PREPROCESSOR_SOC_DEFINES_H
#define TDM_BCM56960_PREPROCESSOR_SOC_DEFINES_H

#ifdef _TDM_STANDALONE
	#include <tdm_th_defines.h>
#else
	#include <soc/tdm/tomahawk/tdm_th_defines.h>
#endif

#define _SOC_TH_TDM_LR_VBS_LEN       256
#define _SOC_TH_TDM_OS_VBS_GRP_LEN   12
#define _SOC_TH_TDM_NUM_EXT_PORTS    136
#define _SOC_TH_TDM_NUM_PM_MOD       33

typedef struct _tdm_th_pipes_s {
	int idb_tdm_tbl_0[_SOC_TH_TDM_LR_VBS_LEN];
	int idb_tdm_ovs_0_a[_SOC_TH_TDM_OS_VBS_GRP_LEN];
	int idb_tdm_ovs_0_b[_SOC_TH_TDM_OS_VBS_GRP_LEN];
	int idb_tdm_ovs_0_c[_SOC_TH_TDM_OS_VBS_GRP_LEN];
	int idb_tdm_ovs_0_d[_SOC_TH_TDM_OS_VBS_GRP_LEN];
	int idb_tdm_ovs_0_e[_SOC_TH_TDM_OS_VBS_GRP_LEN];
	int idb_tdm_ovs_0_f[_SOC_TH_TDM_OS_VBS_GRP_LEN];
	int idb_tdm_ovs_0_g[_SOC_TH_TDM_OS_VBS_GRP_LEN];
	int idb_tdm_ovs_0_h[_SOC_TH_TDM_OS_VBS_GRP_LEN];
	int idb_tdm_tbl_1[_SOC_TH_TDM_LR_VBS_LEN];
	int idb_tdm_ovs_1_a[_SOC_TH_TDM_OS_VBS_GRP_LEN];
	int idb_tdm_ovs_1_b[_SOC_TH_TDM_OS_VBS_GRP_LEN];
	int idb_tdm_ovs_1_c[_SOC_TH_TDM_OS_VBS_GRP_LEN];
	int idb_tdm_ovs_1_d[_SOC_TH_TDM_OS_VBS_GRP_LEN];
	int idb_tdm_ovs_1_e[_SOC_TH_TDM_OS_VBS_GRP_LEN];
	int idb_tdm_ovs_1_f[_SOC_TH_TDM_OS_VBS_GRP_LEN];
	int idb_tdm_ovs_1_g[_SOC_TH_TDM_OS_VBS_GRP_LEN];
	int idb_tdm_ovs_1_h[_SOC_TH_TDM_OS_VBS_GRP_LEN];
	int idb_tdm_tbl_2[_SOC_TH_TDM_LR_VBS_LEN];
	int idb_tdm_ovs_2_a[_SOC_TH_TDM_OS_VBS_GRP_LEN];
	int idb_tdm_ovs_2_b[_SOC_TH_TDM_OS_VBS_GRP_LEN];
	int idb_tdm_ovs_2_c[_SOC_TH_TDM_OS_VBS_GRP_LEN];
	int idb_tdm_ovs_2_d[_SOC_TH_TDM_OS_VBS_GRP_LEN];
	int idb_tdm_ovs_2_e[_SOC_TH_TDM_OS_VBS_GRP_LEN];
	int idb_tdm_ovs_2_f[_SOC_TH_TDM_OS_VBS_GRP_LEN];
	int idb_tdm_ovs_2_g[_SOC_TH_TDM_OS_VBS_GRP_LEN];
	int idb_tdm_ovs_2_h[_SOC_TH_TDM_OS_VBS_GRP_LEN];
	int idb_tdm_tbl_3[_SOC_TH_TDM_LR_VBS_LEN];
	int idb_tdm_ovs_3_a[_SOC_TH_TDM_OS_VBS_GRP_LEN];
	int idb_tdm_ovs_3_b[_SOC_TH_TDM_OS_VBS_GRP_LEN];
	int idb_tdm_ovs_3_c[_SOC_TH_TDM_OS_VBS_GRP_LEN];
	int idb_tdm_ovs_3_d[_SOC_TH_TDM_OS_VBS_GRP_LEN];
	int idb_tdm_ovs_3_e[_SOC_TH_TDM_OS_VBS_GRP_LEN];
	int idb_tdm_ovs_3_f[_SOC_TH_TDM_OS_VBS_GRP_LEN];
	int idb_tdm_ovs_3_g[_SOC_TH_TDM_OS_VBS_GRP_LEN];
	int idb_tdm_ovs_3_h[_SOC_TH_TDM_OS_VBS_GRP_LEN];
	int mmu_tdm_tbl_0[_SOC_TH_TDM_LR_VBS_LEN];
	int mmu_tdm_ovs_0_a[_SOC_TH_TDM_OS_VBS_GRP_LEN];
	int mmu_tdm_ovs_0_b[_SOC_TH_TDM_OS_VBS_GRP_LEN];
	int mmu_tdm_ovs_0_c[_SOC_TH_TDM_OS_VBS_GRP_LEN];
	int mmu_tdm_ovs_0_d[_SOC_TH_TDM_OS_VBS_GRP_LEN];
	int mmu_tdm_ovs_0_e[_SOC_TH_TDM_OS_VBS_GRP_LEN];
	int mmu_tdm_ovs_0_f[_SOC_TH_TDM_OS_VBS_GRP_LEN];
	int mmu_tdm_ovs_0_g[_SOC_TH_TDM_OS_VBS_GRP_LEN];
	int mmu_tdm_ovs_0_h[_SOC_TH_TDM_OS_VBS_GRP_LEN];
	int mmu_tdm_tbl_1[_SOC_TH_TDM_LR_VBS_LEN];
	int mmu_tdm_ovs_1_a[_SOC_TH_TDM_OS_VBS_GRP_LEN];
	int mmu_tdm_ovs_1_b[_SOC_TH_TDM_OS_VBS_GRP_LEN];
	int mmu_tdm_ovs_1_c[_SOC_TH_TDM_OS_VBS_GRP_LEN];
	int mmu_tdm_ovs_1_d[_SOC_TH_TDM_OS_VBS_GRP_LEN];
	int mmu_tdm_ovs_1_e[_SOC_TH_TDM_OS_VBS_GRP_LEN];
	int mmu_tdm_ovs_1_f[_SOC_TH_TDM_OS_VBS_GRP_LEN];
	int mmu_tdm_ovs_1_g[_SOC_TH_TDM_OS_VBS_GRP_LEN];
	int mmu_tdm_ovs_1_h[_SOC_TH_TDM_OS_VBS_GRP_LEN];
	int mmu_tdm_tbl_2[_SOC_TH_TDM_LR_VBS_LEN];
	int mmu_tdm_ovs_2_a[_SOC_TH_TDM_OS_VBS_GRP_LEN];
	int mmu_tdm_ovs_2_b[_SOC_TH_TDM_OS_VBS_GRP_LEN];
	int mmu_tdm_ovs_2_c[_SOC_TH_TDM_OS_VBS_GRP_LEN];
	int mmu_tdm_ovs_2_d[_SOC_TH_TDM_OS_VBS_GRP_LEN];
	int mmu_tdm_ovs_2_e[_SOC_TH_TDM_OS_VBS_GRP_LEN];
	int mmu_tdm_ovs_2_f[_SOC_TH_TDM_OS_VBS_GRP_LEN];
	int mmu_tdm_ovs_2_g[_SOC_TH_TDM_OS_VBS_GRP_LEN];
	int mmu_tdm_ovs_2_h[_SOC_TH_TDM_OS_VBS_GRP_LEN];
	int mmu_tdm_tbl_3[_SOC_TH_TDM_LR_VBS_LEN];
	int mmu_tdm_ovs_3_a[_SOC_TH_TDM_OS_VBS_GRP_LEN];
	int mmu_tdm_ovs_3_b[_SOC_TH_TDM_OS_VBS_GRP_LEN];
	int mmu_tdm_ovs_3_c[_SOC_TH_TDM_OS_VBS_GRP_LEN];
	int mmu_tdm_ovs_3_d[_SOC_TH_TDM_OS_VBS_GRP_LEN];
	int mmu_tdm_ovs_3_e[_SOC_TH_TDM_OS_VBS_GRP_LEN];
	int mmu_tdm_ovs_3_f[_SOC_TH_TDM_OS_VBS_GRP_LEN];
	int mmu_tdm_ovs_3_g[_SOC_TH_TDM_OS_VBS_GRP_LEN];
	int mmu_tdm_ovs_3_h[_SOC_TH_TDM_OS_VBS_GRP_LEN];
} _tdm_th_pipes_t;

#define _TDM_LENGTH                    256
#define _OVS_GROUP_COUNT               6
#define _OVS_GROUP_TDM_LENGTH          12
#define NULL_TOKEN                     253

#define PORT_HIGIG2 999
#define PORT_ETHERNET 998

/* Value hardcoded in set_tdm.c, definition needs to be matched */
#define PORT_STATE_UNUSED              0
#define PORT_STATE_LINERATE            1
#define PORT_STATE_OVERSUB             2
#define PORT_STATE_CONTINUATION        3 /* part of other port */

#define TH_PIPES_PER_DEV 4
#define TH_PBLKS_PER_DEV 32

typedef struct _tdm_th_globals_s {
  enum port_speed_e speed[_SOC_TH_TDM_NUM_EXT_PORTS];
  int clk_freq;
  int port_rates_array[_SOC_TH_TDM_NUM_EXT_PORTS];
  int pm_encap_type[_SOC_TH_TDM_NUM_PM_MOD];
} _tdm_th_globals_t;

typedef struct _soc_tomahawk_tdm_s {
    int speed[TH_NUM_EXT_PORTS];
    int port_state[TH_NUM_EXT_PORTS]; /* 0-unused, 1-line rate, 2-oversub,
									   * 3-part of other port 
									   */
    /*
    int idb_tdm[TH_PIPES_PER_DEV][_TDM_LENGTH];
    int idb_ovs_tdm[TH_PIPES_PER_DEV][_OVS_GROUP_COUNT][_OVS_GROUP_TDM_LENGTH];
    int mmu_tdm[TH_PIPES_PER_DEV][_TDM_LENGTH];
    int mmu_ovs_tdm[TH_PIPES_PER_DEV][_OVS_GROUP_COUNT][_OVS_GROUP_TDM_LENGTH];
    */
    int *idb_tdm[TH_PIPES_PER_DEV];
    int *idb_ovs_tdm[TH_PIPES_PER_DEV][_OVS_GROUP_COUNT];
    int *mmu_tdm[TH_PIPES_PER_DEV];
    int *mmu_ovs_tdm[TH_PIPES_PER_DEV][_OVS_GROUP_COUNT];
    
	int pm_encap_type[TH_NUM_PM_MOD];
	
    _tdm_th_globals_t tdm_globals;
    _tdm_th_pipes_t tdm_pipe_tables;	

    /* Following fields are not arguments to the TDM code */
    int port_ratio[TH_PBLKS_PER_DEV];
    int ovs_ratio_x1000[TH_PIPES_PER_DEV];
} _soc_tomahawk_tdm_t;

#endif
