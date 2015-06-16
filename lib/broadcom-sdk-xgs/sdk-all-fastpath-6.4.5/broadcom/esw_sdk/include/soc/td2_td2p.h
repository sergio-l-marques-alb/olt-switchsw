/*
 * $Id:$
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
 * File:        td2_td2p.h
 * Purpose:
 * Requires:
 */
#ifndef _TD2_TD2P_H_
#define _TD2_TD2P_H_

#include <soc/mmu_config.h>

/* TDM algorithm */
#define _TD2P_NUM_PORT_MODULES 33
#define OVSB_TOKEN 250
#define IDL1_TOKEN 251
#define IDL2_TOKEN 252
#define NUM_EXT_PORTS 130
#define OS_GROUP_LEN 16
#define NUM_TSC 32

#define _TD2_OVS_TOKEN                  (NUM_EXT_PORTS+1)
#define _TD2_IDL_TOKEN                  (NUM_EXT_PORTS+2)

#define _TD2_PGW_TDM_LENGTH             32
#define _TD2_IARB_TDM_LENGTH            512
#define _TD2_TD2_CELLS_PER_OBM          1020
#define _MMU_OVS_GROUP_COUNT            4
#define _TD2_MMU_OVS_GROUP_COUNT        _MMU_OVS_GROUP_COUNT


#ifdef BCM_TRIDENT2PLUS_SUPPORT

extern int get_mmu_mode (int unit);
extern int soc_td2p_if_full_chip (int unit);

#define _TD2P_PGW_TDM_LENGTH            64
#define MAX_PGW_TDM_LENGTH              64

#define _TD2P_TD2_CELLS_PER_OBM         2044


#define _TD2P_MMU_OVS_GROUP_COUNT       8
#define MAX_MMU_OVS_GROUP_COUNT         8

#define GET_PGW_TDM_LENGTH(unit)\
    ((SOC_IS_TRIDENT2PLUS(unit)) ? _TD2P_PGW_TDM_LENGTH : _TD2_PGW_TDM_LENGTH )

#define GET_MMU_OVS_GCOUNT(unit)\
    ((SOC_IS_TRIDENT2PLUS(unit)) ? _TD2P_MMU_OVS_GROUP_COUNT : _TD2_MMU_OVS_GROUP_COUNT )

#define GET_NUM_CELLS_PER_OBM(unit)\
    ((SOC_IS_TRIDENT2PLUS(unit)) ? _TD2P_TD2_CELLS_PER_OBM : _TD2_TD2_CELLS_PER_OBM )

#define _TD2P_OVS_TOKEN                 OVSB_TOKEN
#define _TD2P_IDL_TOKEN                 IDL1_TOKEN

#define GET_OVS_TOKEN(unit)\
    ((SOC_IS_TRIDENT2PLUS(unit)) ? _TD2P_OVS_TOKEN : _TD2_OVS_TOKEN )
#define GET_IDL_TOKEN(unit)\
    ((SOC_IS_TRIDENT2PLUS(unit)) ? _TD2P_IDL_TOKEN : _TD2_IDL_TOKEN )

#else

#define GET_PGW_TDM_LENGTH(unit)         _TD2_PGW_TDM_LENGTH
#define GET_MMU_OVS_GCOUNT(unit)         _MMU_OVS_GROUP_COUNT
#define GET_NUM_CELLS_PER_OBM(unit)     _TD2_TD2_CELLS_PER_OBM

#define MAX_PGW_TDM_LENGTH              32
#define MAX_MMU_OVS_GROUP_COUNT         4

#define GET_OVS_TOKEN(unit)         _TD2_OVS_TOKEN
#define GET_IDL_TOKEN(unit)         _TD2_IDL_TOKEN

#endif

#define _PGW_TDM_OVS_SIZE               32

#define _PGW_MASTER_COUNT               4
#define _PGW_TDM_SLOTS_PER_REG          4
#define _MMU_TDM_LENGTH                 256
#define _MMU_OVS_GROUP_TDM_LENGTH       16
#define _IARB_TDM_LENGTH                512
#define _MMU_OVS_WT_GROUP_COUNT         4

#define SOC_TD2P_MAX_MMU_PORTS_PER_PIPE 52

typedef soc_reg_t(*mmu_ovs_group_wt_regs_t)[_MMU_OVS_WT_GROUP_COUNT];


#ifdef BCM_TRIDENT2PLUS_SUPPORT

enum td2plus_evlxlt_hash_key_type_e {
    /* WARNING: values given match hardware register; do not modify */
    TD2PLUS_EVLXLT_HASH_KEY_TYPE_VLAN_XLATE = 0,
    TD2PLUS_EVLXLT_HASH_KEY_TYPE_VLAN_XLATE_DVP = 1,
    TD2PLUS_EVLXLT_HASH_KEY_TYPE_FCOE_XLATE = 2,
    TD2PLUS_EVLXLT_HASH_KEY_TYPE_ISID_XLATE = 3,
    TD2PLUS_EVLXLT_HASH_KEY_TYPE_ISID_DVP_XLATE = 4,
    TD2PLUS_EVLXLT_HASH_KEY_TYPE_L2GRE_VFI = 5,
    TD2PLUS_EVLXLT_HASH_KEY_TYPE_L2GRE_VFI_DVP = 6,
    TD2PLUS_EVLXLT_HASH_KEY_TYPE_FCOE_XLATE_DVP = 7,
    TD2PLUS_EVLXLT_HASH_KEY_TYPE_VXLAN_VFI = 8,
    TD2PLUS_EVLXLT_HASH_KEY_TYPE_VXLAN_VFI_DVP = 9,
    TD2PLUS_EVLXLT_HASH_KEY_TYPE_VFI_DVP_GROUP = 10,
    TD2PLUS_EVLXLT_HASH_KEY_TYPE_VRF_MAPPING = 11,
    TD2PLUS_EVLXLT_HASH_KEY_TYPE_VLAN_XLATE_VFI =12,
    TD2PLUS_EVLXLT_HASH_KEY_TYPE_COUNT
};

enum td2plus_dvp_evlxlt_hash_key_type_e {
    /* WARNING: values given match hardware register; do not modify */
    TD2PLUS_DVP_EVLXLT_HASH_KEY_TYPE_NOOP = 0,
    TD2PLUS_DVP_EVLXLT_HASH_KEY_TYPE_VFI = 1,
    TD2PLUS_DVP_EVLXLT_HASH_KEY_TYPE_DVP_VFI = 2,
    TD2PLUS_DVP_EVLXLT_HASH_KEY_TYPE_GID_VFI = 3,
    TD2PLUS_DVP_EVLXLT_HASH_KEY_TYPE_DVP_OL_VLAN = 4,
    TD2PLUS_DVP_EVLXLT_HASH_KEY_TYPE_PORT_UL_VLAN = 5,
    TD2PLUS_DVP_EVLXLT_HASH_KEY_TYPE_COUNT
};

extern mmu_ovs_group_wt_regs_t 
get_mmu_ovs_group_wt_regs (int unit, mmu_ovs_group_wt_regs_t old);

#endif


extern
int soc_td2p_set_obm_registers (int unit, soc_reg_t reg, int speed, int index, int obm_inst, 
        int lossless );
extern
int soc_td2p_obm_prob_drop_default_set (int unit, int xlp, int index,
                                        int obm_inst); 
extern
int soc_td2p_mmu_delay_insertion_set (int unit, int port, int speed); 
extern
int soc_td2p_mem_config(int unit, uint16 dev_id, uint8 rev_id);
extern
int soc_td2p_get_shared_bank_size(int unit, uint16 dev_id, uint8 rev_id);
extern uint32* soc_td2p_mmu_params_arr_get (uint16 dev_id, uint8 rev_id);
extern
void soc_cm_get_id_otp(int unit, uint16 *dev_id, uint8 *rev_id);

extern int
tdm_td2p_set_iarb_tdm_table(int core_bw, int is_x_ovs, int is_y_ovs, int mgm4x1,
int mgm4x2p5, int mgm1x10, int *iarb_tdm_wrap_ptr_x, int *iarb_tdm_wrap_ptr_y,
int *iarb_tdm_tbl_x, int *iarb_tdm_tbl_y);

extern int
soc_td2p_show_voltage(int unit);

extern int
soc_td2p_edb_buf_reset(int unit, soc_port_t port, int reset);

#endif /* _td2_td2p_h */
