/*
 * $Id: tdm_th_methods.h$
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
LINKER_DECL int tdm_th_filter_chain( tdm_mod_t *_tdm );
LINKER_DECL int tdm_th_pick_vec( tdm_mod_t *_tdm );

/* Core executive polymorphic functions */
LINKER_DECL int tdm_th_which_tsc( tdm_mod_t *_tdm );
LINKER_DECL void tdm_th_parse_pipe( tdm_mod_t *_tdm );

/* Internalized functions */
LINKER_DECL int tdm_th_check_ethernet( tdm_mod_t *_tdm );

/* TDM.4 - Tomahawk chip API */
LINKER_DECL int tdm_th_legacy_which_tsc(unsigned char port, int **tsc);

/* TDM.4 - Tomahawk chip based filter methods */
LINKER_DECL int tdm_th_filter_vector_shift(int *tdm_tbl, int port, int dir);
LINKER_DECL void tdm_th_filter_local_slice_dn(int port, int *tdm_tbl, int **tsc, enum port_speed_e *speed);
LINKER_DECL void tdm_th_filter_local_slice_up(int port, int *tdm_tbl, int **tsc, enum port_speed_e *speed);
LINKER_DECL void tdm_th_filter_ancillary_smooth(int port, int *tdm_tbl, int lr_idx_limit, int accessories);
LINKER_DECL int tdm_th_filter_slice_dn(int port, int *tdm_tbl, int lr_idx_limit, int **tsc, enum port_speed_e *speed);
LINKER_DECL int tdm_th_filter_slice_up(int port, int *tdm_tbl, int lr_idx_limit, int **tsc, enum port_speed_e *speed);
LINKER_DECL void tdm_th_filter_dither(int *tdm_tbl, int lr_idx_limit, int accessories, int **tsc, int threshold, enum port_speed_e *speed);
LINKER_DECL int tdm_th_filter_fine_dither(int port, int *tdm_tbl, int lr_idx_limit, int accessories, int **tsc);
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
LINKER_DECL int tdm_th_num_lr_slots(int *tdm_tbl);


#endif /* TDM_TH_PREPROCESSOR_PROTOTYPES_H */
