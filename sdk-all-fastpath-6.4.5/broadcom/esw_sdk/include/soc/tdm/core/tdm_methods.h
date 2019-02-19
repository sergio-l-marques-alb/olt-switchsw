/*
 * $Id: tdm_decls_drv.h$
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
 * TDM function prototypes for core scheduler
 */

#ifndef TDM_PREPROCESSOR_PROTOTYPES_H
#define TDM_PREPROCESSOR_PROTOTYPES_H
/*
 * CORE function prototypes 
 */

/* Class functions managed by TDM core executive */
LINKER_DECL int tdm_core_init( tdm_mod_t *_tdm );
LINKER_DECL int tdm_core_post( tdm_mod_t *_tdm );
LINKER_DECL int tdm_core_vbs_scheduler( tdm_mod_t *_tdm );
LINKER_DECL int tdm_core_vbs_scheduler_ovs( tdm_mod_t *_tdm );
LINKER_DECL int tdm_pick_vec( tdm_mod_t *_tdm );
LINKER_DECL int tdm_find_pm( tdm_mod_t *_tdm );
LINKER_DECL int tdm_core_acc_alloc( tdm_mod_t *_tdm );
LINKER_DECL int tdm_core_null( tdm_mod_t *_tdm );

/* Internal functions that can use class-heritable data */
LINKER_DECL void tdm_mem_transpose( tdm_mod_t *_tdm );
LINKER_DECL void tdm_sticky_transpose( tdm_mod_t *_tdm );
LINKER_DECL int tdm_type_chk( tdm_mod_t *_tdm );
LINKER_DECL void tdm_class_data( tdm_mod_t *_tdm );
LINKER_DECL void tdm_class_output( tdm_mod_t *_tdm );
LINKER_DECL void tdm_print_stat( tdm_mod_t *_tdm );
LINKER_DECL void tdm_vector_dump( tdm_mod_t *_tdm );
LINKER_DECL void tdm_print_vmap_vector( tdm_mod_t *_tdm );
LINKER_DECL void tdm_vector_zrow( tdm_mod_t *_tdm );
LINKER_DECL int tdm_find_fastest_triport( tdm_mod_t *_tdm );
LINKER_DECL int tdm_find_fastest_port( tdm_mod_t *_tdm );
LINKER_DECL int tdm_find_fastest_spd( tdm_mod_t *_tdm );
LINKER_DECL int tdm_map_find_y_indx( tdm_mod_t *_tdm );
LINKER_DECL void tdm_vector_rotate( tdm_mod_t *_tdm );
LINKER_DECL void tdm_vector_clear( tdm_mod_t *_tdm );
LINKER_DECL int tdm_fit_singular_col( tdm_mod_t *_tdm );
LINKER_DECL int tdm_fit_prox( tdm_mod_t *_tdm );
LINKER_DECL int tdm_count_nonsingular( tdm_mod_t *_tdm );
LINKER_DECL int tdm_fit_row_min( tdm_mod_t *_tdm );
LINKER_DECL int tdm_count_param_spd( tdm_mod_t *_tdm );
LINKER_DECL int tdm_nsin_row( tdm_mod_t *_tdm );
LINKER_DECL int tdm_core_filter_refactor( tdm_mod_t *_tdm );
LINKER_DECL int tdm_fill_ovs( tdm_mod_t *_tdm );

/* Linked list API partially merged with class method */
LINKER_DECL int tdm_ll_retrace(struct node *llist, tdm_mod_t *_tdm, int cadence_start_idx);
LINKER_DECL int tdm_ll_tsc_dist(struct node *llist, tdm_mod_t *_tdm, int idx);
LINKER_DECL void tdm_ll_strip(struct node *llist, tdm_mod_t *_tdm, int cadence_start_idx, int *pool, int *s_idx, int token);

/* Internal functions managed entirely by compiler */
LINKER_DECL void tdm_core_prealloc(unsigned char stack[TDM_AUX_SIZE], int buffer[TDM_AUX_SIZE], short *x, char *bool, int j);
LINKER_DECL int tdm_core_postalloc(unsigned char **vector_map, int freq, unsigned char spd, short *yy, short *y, int lr_idx_limit, unsigned char lr_stack[TDM_AUX_SIZE], int token, const char* speed, int num_ext_ports);
LINKER_DECL int tdm_slots(int port_speed);
LINKER_DECL int tdm_arr_exists(int element, int len, int arr[ARR_FIXED_LEN]);
LINKER_DECL int tdm_arr_append(int element, int len, int arr[ARR_FIXED_LEN], int num_ext_ports);
LINKER_DECL int tdm_empty_row(unsigned char **map, unsigned char y_idx, int num_ext_ports, int vec_map_wid);
LINKER_DECL void tdm_vector_rotate_step(unsigned char vector[], int size, int step);
LINKER_DECL int tdm_slice_size_2d(unsigned char **map, unsigned char y_idx, int lim, int num_ext_ports, int vec_map_wid);
LINKER_DECL int tdm_fit_singular_cnt(unsigned char **map, int node_y, int vec_map_wid, int num_ext_ports);
LINKER_DECL int tdm_map_cadence_count(unsigned char *vector, int idx, int vec_map_len);
LINKER_DECL int tdm_map_retrace_count(unsigned char **map, int x_idx, int y_idx, int vec_map_len, int vec_map_wid, int num_ext_ports);
LINKER_DECL int tdm_fill_ovs_simple(short *z, unsigned char ovs_buf[TDM_AUX_SIZE], int *bucket1, unsigned char *z11, int *bucket2, unsigned char *z22, int *bucket3, unsigned char *z33, int *bucket4, unsigned char *z44, int *bucket5, unsigned char *z55, int *bucket6, unsigned char *z66, int *bucket7, unsigned char *z77, int *bucket8, unsigned char *z88, int grp_len);

/* Linked list API managed entirely by compiler */
LINKER_DECL void tdm_ll_append(struct node *llist, unsigned char port_append);
LINKER_DECL void tdm_ll_insert(struct node *llist, unsigned char port_insert, int idx);
LINKER_DECL void tdm_ll_print(struct node *llist);
LINKER_DECL void tdm_ll_deref(struct node *llist, int *tdm, int lim);
LINKER_DECL int tdm_ll_delete(struct node *llist, int idx);
LINKER_DECL int tdm_ll_get(struct node *llist, int idx, int num_ext_ports);
LINKER_DECL int tdm_ll_dist(struct node *llist, int idx);
LINKER_DECL int tdm_ll_free(struct node *llist);

/* CMATH API */
LINKER_DECL int tdm_PQ(int f);
LINKER_DECL int tdm_sqrt(int input_signed);
LINKER_DECL int tdm_pow(int num, int pow);
LINKER_DECL int tdm_fac(int num_signed);
LINKER_DECL int tdm_abs(int num);


#endif /* TDM_PREPROCESSOR_PROTOTYPES_H */
