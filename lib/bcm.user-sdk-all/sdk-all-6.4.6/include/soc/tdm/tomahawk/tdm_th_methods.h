/*
 * $Id: tdm_th_methods.h$
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
 * TDM function prototypes for BCM56960
 */

#ifndef TDM_TH_PREPROCESSOR_PROTOTYPES_H
#define TDM_TH_PREPROCESSOR_PROTOTYPES_H

#ifdef _TDM_STANDALONE
	#include <tdm_th_soc.h>
#else
	#include <soc/tdm/tomahawk/tdm_th_soc.h>
#endif

/*
 * CHIP function prototypes 
 */

/* Functions managed by TDM chip executive */
LINKER_DECL int tdm_th_init( tdm_mod_t *_tdm );
LINKER_DECL int tdm_th_pmap_transcription( tdm_mod_t *_tdm );
LINKER_DECL int tdm_th_scheduler_wrap( tdm_mod_t *_tdm );
LINKER_DECL int tdm_th_vmap_alloc( tdm_mod_t *_tdm );
LINKER_DECL int tdm_th_parse_tdm_tbl( tdm_mod_t *_tdm );
LINKER_DECL int tdm_th_corereq( tdm_mod_t *_tdm );
LINKER_DECL int tdm_th_filter_chain( tdm_mod_t *_tdm );
LINKER_DECL int tdm_th_post( tdm_mod_t *_tdm );
LINKER_DECL int tdm_th_pick_vec( tdm_mod_t *_tdm );
LINKER_DECL int tdm_th_free( tdm_mod_t *_tdm );

/* Core executive polymorphic functions */
LINKER_DECL int tdm_th_which_tsc( tdm_mod_t *_tdm );
LINKER_DECL void tdm_th_parse_pipe( tdm_mod_t *_tdm );

/* Internalized functions */
LINKER_DECL int tdm_th_check_ethernet( tdm_mod_t *_tdm );

/* TDM.4 - Tomahawk chip API */
LINKER_DECL int tdm_th_legacy_which_tsc(unsigned char port, int **tsc);

/* TDM.4 - Tomahawk chip based filter methods */
LINKER_DECL int tdm_th_filter_check_pipe_sister_min( tdm_mod_t *_tdm );
LINKER_DECL int tdm_th_filter_check_port_sister_min( tdm_mod_t *_tdm, int port );
LINKER_DECL int tdm_th_filter_check_migrate_lr_slot(int idx_src, int idx_dst, int *tdm_tbl, int tdm_tbl_len, int **tsc);
LINKER_DECL int tdm_th_filter_sister_min( tdm_mod_t *_tdm );
LINKER_DECL int tdm_th_filter_migrate_lr_slot(int idx_src, int idx_dst, int *tdm_tbl, int tdm_tbl_len, int **tsc);
LINKER_DECL int tdm_th_filter_migrate_lr_slot_up(tdm_mod_t *_tdm);
LINKER_DECL int tdm_th_filter_migrate_lr_slot_dn(tdm_mod_t *_tdm);
LINKER_DECL int tdm_th_filter_dither(int *tdm_tbl, int lr_idx_limit, int accessories, int **tsc, int threshold, enum port_speed_e *speed);
LINKER_DECL int tdm_th_filter_fine_dither(int port, int *tdm_tbl, int lr_idx_limit, int accessories, int **tsc);
LINKER_DECL int tdm_th_filter_shift_lr_port(unsigned char port, int *tdm_tbl, int tdm_tbl_len, int dir);
LINKER_DECL int tdm_th_filter_migrate_os_slot(int idx_src, int idx_dst, int *tdm_tbl, int tdm_tbl_len, int **tsc, enum port_speed_e *speed);
LINKER_DECL int tdm_th_filter_smooth_idle_slice(int *tdm_tbl, int tdm_tbl_len, int **tsc, enum port_speed_e *speed);
LINKER_DECL int tdm_th_filter_smooth_os_slice(int *tdm_tbl, int tdm_tbl_len, int **tsc, enum port_speed_e *speed, int dir);
LINKER_DECL int tdm_th_filter_smooth_os_slice_fine(int *tdm_tbl, int tdm_tbl_len, int **tsc, enum port_speed_e *speed);
LINKER_DECL int tdm_th_filter_smooth_os_os_up(int *tdm_tbl, int tdm_tbl_len, int **tsc, enum port_speed_e *speed);
LINKER_DECL int tdm_th_filter_smooth_os_os_dn(int *tdm_tbl, int tdm_tbl_len, int **tsc, enum port_speed_e *speed);
LINKER_DECL int tdm_th_filter_smooth_ancl(int ancl_token, int *tdm_tbl, int tdm_tbl_len, int ancl_space_min);

/* TDM.4 - Tomahawk chip based check/scan methods */
LINKER_DECL int tdm_th_check_same_port_dist_dn(int idx, int *tdm_tbl, int lim);
LINKER_DECL int tdm_th_check_same_port_dist_up(int idx, int *tdm_tbl, int lim);
LINKER_DECL int tdm_th_check_same_port_dist_dn_port(int port, int idx, int *tdm_tbl, int lim);
LINKER_DECL int tdm_th_check_same_port_dist_up_port(int port, int idx, int *tdm_tbl, int lim);
LINKER_DECL int tdm_th_check_fit_smooth(int *tdm_tbl, int port, int lr_idx_limit, int clump_thresh);
LINKER_DECL int tdm_th_slice_size_local(unsigned char idx, int *tdm, int lim);
LINKER_DECL int tdm_th_slice_prox_dn(int slot, int *tdm, int lim, int **tsc, enum port_speed_e *speed);
LINKER_DECL int tdm_th_slice_prox_up(int slot, int *tdm, int **tsc, enum port_speed_e *speed);
LINKER_DECL int tdm_th_slice_size(unsigned char port, int *tdm, int lim);
LINKER_DECL int tdm_th_slice_idx(unsigned char port, int *tdm, int lim);
LINKER_DECL int tdm_th_slice_prox_local(unsigned char idx, int *tdm, int lim, int **tsc);
LINKER_DECL int tdm_th_check_lls_flat_up(int idx, int *tdm_tbl, enum port_speed_e *speed);
LINKER_DECL int tdm_th_scan_lr_slot_num(int *tdm_tbl);
LINKER_DECL int tdm_th_scan_slice_min(unsigned char port, int *tdm, int lim, int *slice_start_idx, int pos);
LINKER_DECL int tdm_th_scan_slice_max(unsigned char port, int *tdm, int lim, int *slice_start_idx, int pos);
LINKER_DECL int tdm_th_scan_slice_size_local(unsigned char idx, int *tdm, int lim, int *slice_start_idx);
LINKER_DECL int tdm_th_scan_mix_slice_min(unsigned char port, int *tdm, int lim, int *slice_idx, int pos);
LINKER_DECL int tdm_th_scan_mix_slice_max(unsigned char port, int *tdm, int lim, int *slice_idx, int pos);
LINKER_DECL int tdm_th_scan_mix_slice_size_local(unsigned char idx, int *tdm, int lim, int *slice_start_idx);
LINKER_DECL int tdm_th_check_shift_cond_pattern(unsigned char port, int *tdm_tbl, int tdm_tbl_len, int **tsc, int dir);
LINKER_DECL int tdm_th_check_shift_cond_local_slice(unsigned char port, int *tdm_tbl, int tdm_tbl_len, int **tsc, int dir);
LINKER_DECL int tdm_th_check_slot_swap_cond(int idx, int *tdm_tbl, int tdm_tbl_len, int **tsc, enum port_speed_e *speed);
LINKER_DECL int tdm_th_scan_which_tsc( int port, tdm_mod_t *_tdm );
LINKER_DECL int tdm_th_check_pipe_ethernet( int port, tdm_mod_t *_tdm );

/* TDM.4 - Trident2+ linked list API */
LINKER_DECL void tdm_th_ll_print(struct th_ll_node *llist);
LINKER_DECL void tdm_th_ll_deref(struct th_ll_node *llist, int *tdm[TH_LR_VBS_LEN], int lim);
LINKER_DECL void tdm_th_ll_append(struct th_ll_node *llist, unsigned char port_append, int *pointer);
LINKER_DECL void tdm_th_ll_insert(struct th_ll_node *llist, unsigned char port_insert, int idx);
LINKER_DECL int tdm_th_ll_delete(struct th_ll_node *llist, int idx);
LINKER_DECL int tdm_th_ll_get(struct th_ll_node *llist, int idx);
LINKER_DECL int tdm_th_ll_len(struct th_ll_node *llist);
LINKER_DECL void tdm_th_ll_strip(struct th_ll_node *llist, int *pool, int token);
LINKER_DECL int tdm_th_ll_count(struct th_ll_node *llist, int token);
LINKER_DECL void tdm_th_ll_weave(struct th_ll_node *llist, int token, tdm_mod_t *_tdm);
LINKER_DECL void tdm_th_ll_retrace(struct th_ll_node *llist, tdm_mod_t *_tdm);
LINKER_DECL int tdm_th_ll_single_100(struct th_ll_node *llist);
LINKER_DECL int tdm_th_ll_free(struct th_ll_node *llist);

/* TDM.4 - Tomahawk chip based checker */
LINKER_DECL void tdm_th_chk_print_config(enum port_speed_e speed[TH_NUM_EXT_PORTS], enum port_state_e state[TH_NUM_EXT_PORTS], int traffic[TH_NUM_PHY_PM+1]);
LINKER_DECL int tdm_th_chk_tdm_shim_which_tsc(int port, int tsc[TH_NUM_PHY_PM][TH_NUM_PM_LNS]);
LINKER_DECL int tdm_th_chk_tdm_shim_check_ethernet(int port, enum port_speed_e speed[TH_NUM_EXT_PORTS], int tsc[TH_NUM_PHY_PM][TH_NUM_PM_LNS], int traffic[TH_NUM_PHY_PM+1]);
LINKER_DECL void tdm_th_chk_tdm_lr_jitter(int *tdm, int *tracker, int freq, int len, enum port_speed_e speed[TH_NUM_EXT_PORTS], int id, int fail[7]);
LINKER_DECL int tdm_th_chk_tdm_os_jitter(int *tdm, int lim, int num_os, int id);
LINKER_DECL int tdm_th_chk_tdm_sub_lr(int port, int freq, int lim, int *tdm, enum port_speed_e speed[TH_NUM_EXT_PORTS], enum port_state_e state[TH_NUM_EXT_PORTS], int tsc[TH_NUM_PHY_PM][TH_NUM_PM_LNS], int traffic[TH_NUM_PHY_PM+1], int id);
LINKER_DECL int tdm_th_chk_tdm_sub_os(int port, enum port_speed_e speed[TH_NUM_EXT_PORTS], int os_spds[4][8], int *bucket0, int *bucket1, int *bucket2, int *bucket3, int *bucket4, int *bucket5, int *bucket6, int *bucket7, int id);
LINKER_DECL int tdm_th_chk_tdm_min(int *tdm, int len, int tsc[TH_NUM_PHY_PM][TH_NUM_PM_LNS], int id);
LINKER_DECL int tdm_th_chk_tdm_tsc(int port, enum port_speed_e speed[TH_NUM_EXT_PORTS], enum port_state_e state[TH_NUM_EXT_PORTS], int tsc[TH_NUM_PHY_PM][TH_NUM_PM_LNS]);
LINKER_DECL int tdm_th_chk_tdm_struct(int *idb_tdm, int *mmu_tdm, int i_len, int m_len, int pipe, enum port_speed_e speed[TH_NUM_EXT_PORTS], int freq, int traffic[TH_NUM_PHY_PM+1], int tsc[TH_NUM_PHY_PM][TH_NUM_PM_LNS]);
LINKER_DECL void tdm_th_chk_tdm( tdm_mod_t *_tdm);

/* Print */
LINKER_DECL void tdm_th_print_tbl(tdm_mod_t *_tdm);
LINKER_DECL void tdm_th_print_config(tdm_mod_t *_tdm);

#endif /* TDM_TH_PREPROCESSOR_PROTOTYPES_H */
