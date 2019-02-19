/*
 * $Id: tdm_td2p_decls_drv.h$
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
 * All Rights Reserved.$
 *
 * TDM function prototypes for BCM56860
 */

#ifndef TDM_TD2P_PREPROCESSOR_PROTOTYPES_H
#define TDM_TD2P_PREPROCESSOR_PROTOTYPES_H

#ifdef _TDM_STANDALONE
	#include <tdm_td2p_soc.h>
#else
	#include <soc/tdm/trident2p/tdm_td2p_soc.h>
#endif

/*
 * CHIP function prototypes 
 */

/* Functions managed by TDM chip executive */
LINKER_DECL int tdm_td2p_init( tdm_mod_t *_tdm );
LINKER_DECL int tdm_td2p_pmap_transcription( tdm_mod_t *_tdm );
LINKER_DECL int tdm_td2p_lls_wrapper( tdm_mod_t *_tdm );
LINKER_DECL int tdm_td2p_vbs_wrapper( tdm_mod_t *_tdm );
LINKER_DECL int tdm_td2p_extract_cal( tdm_mod_t *_tdm );
LINKER_DECL int tdm_td2p_corereq( tdm_mod_t *_tdm );
LINKER_DECL int tdm_td2p_filter_chain( tdm_mod_t *_tdm );
LINKER_DECL int tdm_td2p_post( tdm_mod_t *_tdm );
LINKER_DECL int tdm_td2p_free( tdm_mod_t *_tdm );

/* Core executive polymorphic functions */
LINKER_DECL int tdm_td2p_which_tsc( tdm_mod_t *_tdm_s );
LINKER_DECL int tdm_td2p_vmap_alloc( tdm_mod_t *_tdm );
LINKER_DECL int tdm_td2p_parse_mmu_tdm_tbl( tdm_mod_t *_tdm );

/* Internalized functions */
LINKER_DECL int tdm_td2p_check_ethernet( tdm_mod_t *_tdm_s );

/* TDM.4 - Trident2+ linked list API */
LINKER_DECL void tdm_td2p_ll_print(struct ll_node *llist);
LINKER_DECL void tdm_td2p_ll_deref(struct ll_node *llist, int *tdm[TD2P_LR_LLS_LEN], int lim);
LINKER_DECL void tdm_td2p_ll_append(struct ll_node *llist, unsigned char port_append, int *pointer);
LINKER_DECL void tdm_td2p_ll_insert(struct ll_node *llist, unsigned char port_insert, int idx);
LINKER_DECL int tdm_td2p_ll_delete(struct ll_node *llist, int idx);
LINKER_DECL int tdm_td2p_ll_get(struct ll_node *llist, int idx);
LINKER_DECL int tdm_td2p_ll_len(struct ll_node *llist);
LINKER_DECL void tdm_td2p_ll_strip(struct ll_node *llist, int *pool, int token);
LINKER_DECL int tdm_td2p_ll_count(struct ll_node *llist, int token);
LINKER_DECL void tdm_td2p_ll_weave(struct ll_node *llist, int wc_array[TD2P_NUM_PHY_PM][TD2P_NUM_PM_LNS], int token);
LINKER_DECL void tdm_td2p_ll_retrace(struct ll_node *llist, int wc_array[TD2P_NUM_PHY_PM][TD2P_NUM_PM_LNS]);
LINKER_DECL int tdm_td2p_ll_single_100(struct ll_node *llist);
LINKER_DECL int tdm_td2p_ll_free(struct ll_node *llist);

/* TDM.4 - Trident2+ chip API */
LINKER_DECL int tdm_td2p_lls_scheduler(struct ll_node *pgw_tdm, tdm_chip_legacy_t *td2p_chip, td2p_pgw_pntrs_t *pntrs_pkg, td2p_pgw_scheduler_vars_t *vars_pkg, int *pgw_tdm_tbl[TD2P_LR_LLS_LEN], int *ovs_tdm_tbl[TD2P_OS_LLS_GRP_LEN], int op_flags[2]);
LINKER_DECL void tdm_td2p_ovs_spacer(int (*wc)[TD2P_NUM_PHY_PM][TD2P_NUM_PM_LNS], int *ovs_tdm_tbl, int *ovs_spacing);
LINKER_DECL int tdm_td2p_legacy_which_tsc(unsigned char port, int **tsc);

/* TDM.4 - Trident2+ chip based prints and parses */
LINKER_DECL void tdm_td2p_print_tbl(int *cal, int len, const char* name, int id);
LINKER_DECL void tdm_td2p_print_tbl_ovs(int *cal, int *spc, int len, const char* name, int id);
LINKER_DECL void tdm_td2p_print_quad(enum port_speed_e *speed, enum port_state_e *state, int len, int idx_start, int idx_end);

/* TDM.4 - Trident2+ chip based checks and scans */
LINKER_DECL int tdm_td2p_check_same_port_dist_dn(int idx, int *tdm_tbl, int lim);
LINKER_DECL int tdm_td2p_check_same_port_dist_up(int idx, int *tdm_tbl, int lim);
LINKER_DECL int tdm_td2p_check_same_port_dist_dn_port(int port, int idx, int *tdm_tbl, int lim);
LINKER_DECL int tdm_td2p_check_same_port_dist_up_port(int port, int idx, int *tdm_tbl, int lim);
LINKER_DECL int tdm_td2p_check_fit_smooth(int *tdm_tbl, int port, int lr_idx_limit, int clump_thresh);
LINKER_DECL int tdm_td2p_slice_size_local(unsigned char idx, int *tdm, int lim);
LINKER_DECL int tdm_td2p_slice_prox_dn(int slot, int *tdm, int lim, int **tsc, enum port_speed_e *speed);
LINKER_DECL int tdm_td2p_slice_prox_up(int slot, int *tdm, int **tsc, enum port_speed_e *speed);
LINKER_DECL int tdm_td2p_slice_size(unsigned char port, int *tdm, int lim);
LINKER_DECL int tdm_td2p_slice_idx(unsigned char port, int *tdm, int lim);
LINKER_DECL int tdm_td2p_slice_prox_local(unsigned char idx, int *tdm, int lim, int **tsc);
LINKER_DECL int tdm_td2p_check_lls_flat_up(int idx, int *tdm_tbl, enum port_speed_e *speed);
LINKER_DECL int tdm_td2p_num_lr_slots(int *tdm_tbl);
LINKER_DECL int tdm_td2p_scan_slice_min(unsigned char port, int *tdm, int lim, int *slice_start_idx, int pos);
LINKER_DECL int tdm_td2p_scan_slice_max(unsigned char port, int *tdm, int lim, int *slice_start_idx, int pos);
LINKER_DECL int tdm_td2p_scan_slice_size_local(unsigned char idx, int *tdm, int lim, int *slice_start_idx);
LINKER_DECL int tdm_td2p_scan_mix_slice_min(unsigned char port, int *tdm, int lim, int *slice_idx, int pos);
LINKER_DECL int tdm_td2p_scan_mix_slice_max(unsigned char port, int *tdm, int lim, int *slice_idx, int pos);
LINKER_DECL int tdm_td2p_scan_mix_slice_size_local(unsigned char idx, int *tdm, int lim, int *slice_start_idx);
LINKER_DECL int tdm_td2p_check_shift_cond_pattern(unsigned char port, int *tdm_tbl, int tdm_tbl_len, int **tsc, int dir);
LINKER_DECL int tdm_td2p_check_shift_cond_local_slice(unsigned char port, int *tdm_tbl, int tdm_tbl_len, int **tsc, int dir);
LINKER_DECL int tdm_td2p_check_slot_swap_cond(int idx, int *tdm_tbl, int tdm_tbl_len, int **tsc, enum port_speed_e *speed);
LINKER_DECL int tdm_td2p_scan_which_tsc(int port, int tsc[TD2P_NUM_PHY_PM][TD2P_NUM_PM_LNS]);
LINKER_DECL int tdm_td2p_check_pipe_ethernet(int port, tdm_mod_t *_tdm);

/* TDM.4 - Trident2+ chip based filter methods */
LINKER_DECL int tdm_td2p_filter_dither(int *tdm_tbl, int lr_idx_limit, int accessories, int **tsc, int threshold, enum port_speed_e *speed);
LINKER_DECL int tdm_td2p_filter_fine_dither(int port, int *tdm_tbl, int lr_idx_limit, int accessories, int **tsc);
LINKER_DECL int tdm_td2p_filter_shift_lr_port(unsigned char port, int *tdm_tbl, int tdm_tbl_len, int dir);
LINKER_DECL int tdm_td2p_filter_migrate_os_slot(int idx_src, int idx_dst, int *tdm_tbl, int tdm_tbl_len, int **tsc, enum port_speed_e *speed);
LINKER_DECL int tdm_td2p_filter_smooth_idle_slice(int *tdm_tbl, int tdm_tbl_len, int **tsc, enum port_speed_e *speed);
LINKER_DECL int tdm_td2p_filter_smooth_os_slice(int *tdm_tbl, int tdm_tbl_len, int **tsc, enum port_speed_e *speed, int dir);
LINKER_DECL int tdm_td2p_filter_smooth_os_slice_fine(int *tdm_tbl, int tdm_tbl_len, int **tsc, enum port_speed_e *speed);
LINKER_DECL int tdm_td2p_filter_smooth_os_os_up(int *tdm_tbl, int tdm_tbl_len, int **tsc, enum port_speed_e *speed);
LINKER_DECL int tdm_td2p_filter_smooth_os_os_dn(int *tdm_tbl, int tdm_tbl_len, int **tsc, enum port_speed_e *speed);
LINKER_DECL int tdm_td2p_filter_smooth_ancl(int ancl_token, int *tdm_tbl, int tdm_tbl_len, int ancl_space_min);

/* TDM.4 - Trident2+ chip specific IARB calendars */
LINKER_DECL void tdm_td2p_init_iarb_tdm_ovs_table(int core_bw, int mgm4x1, int mgm4x2p5, int mgm1x10, int *iarb_tdm_wrap_ptr_ovs_x, int *iarb_tdm_wrap_ptr_ovs_y, int *iarb_tdm_tbl_ovs_x, int *iarb_tdm_tbl_ovs_y);
LINKER_DECL void tdm_td2p_init_iarb_tdm_lr_table(int core_bw, int mgm4x1, int mgm4x2p5, int mgm1x10, int *iarb_tdm_wrap_ptr_lr_x, int *iarb_tdm_wrap_ptr_lr_y, int *iarb_tdm_tbl_lr_x, int *iarb_tdm_tbl_lr_y);
LINKER_DECL int tdm_td2p_set_iarb_tdm_table(int core_bw, int is_x_ovs, int is_y_ovs, int mgm4x1, int mgm4x2p5, int mgm1x10, int *iarb_tdm_wrap_ptr_x, int *iarb_tdm_wrap_ptr_y, int *iarb_tdm_tbl_x, int *iarb_tdm_tbl_y);


#endif /* TDM_TD2P_PREPROCESSOR_PROTOTYPES_H */
