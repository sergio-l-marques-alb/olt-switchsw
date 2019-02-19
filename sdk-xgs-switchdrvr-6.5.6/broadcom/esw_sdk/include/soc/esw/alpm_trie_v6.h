/* 
 * $Id:
 * $Copyright: Copyright 2016 Broadcom Corporation.
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
 * trie IPv6 data structure
 *
 */

#ifndef _ESW_TRIDENT2_ALPM_TRIE_V6_H_
#define _ESW_TRIDENT2_ALPM_TRIE_V6_H_

#ifdef ALPM_ENABLE
#ifndef ALPM_IPV6_128_SUPPORT
extern int _trie_v6_search(trie_node_t *trie,
			   unsigned int *key,
			   unsigned int length,
			   trie_node_t **payload,
			   unsigned int *result_key,
			   unsigned int *result_len,
               unsigned int dump,
               unsigned int find_pivot);

extern int _trie_v6_find_lpm(trie_node_t *trie,
			     unsigned int *key,
			     unsigned int length,
			     trie_node_t **payload,
			     trie_callback_f cb,
			     void *user_data,
			     unsigned int exclude_self);

extern int _trie_v6_find_bpm(trie_node_t *trie,
			     unsigned int *key,
			     unsigned int length,
			     int *bpm_length);

extern int _trie_v6_bpm_mask_get(trie_node_t *trie,
                unsigned int *key,
                unsigned int length,
                unsigned int *bpm_mask);

extern int _trie_v6_skip_node_free(trie_node_t *trie,
                            unsigned int *key,
                            unsigned int length);

extern int _trie_v6_skip_node_alloc(trie_node_t **node,
				    unsigned int *key,
				    /* bpm bit map if bpm management is required, passing null skips bpm management */
				    unsigned int *bpm, 
				    unsigned int msb, /* NOTE: valid msb position 1 based, 0 means skip0/0 node */
				    unsigned int skip_len,
				    trie_node_t *payload,
				    unsigned int count);

extern int _trie_v6_insert(trie_node_t *trie, 
			   unsigned int *key, 
			   /* bpm bit map if bpm management is required, passing null skips bpm management */
			   unsigned int *bpm, 
			   unsigned int length,
			   trie_node_t *payload, /* payload node */
               trie_node_t **child, /* child pointer if the child is modified */
               int child_count);

extern int _trie_v6_delete(trie_node_t *trie, 
			   unsigned int *key,
			   unsigned int length,
			   trie_node_t **payload,
			   trie_node_t **child);

extern int _trie_v6_split(trie_node_t  *trie,
			  unsigned int *pivot,
			  unsigned int *length,
			  unsigned int *split_count,
			  trie_node_t **split_node,
			  trie_node_t **child,
			  const unsigned int max_count,
			  const unsigned int max_split_len,
			  const int split_to_pair,
			  unsigned int *bpm,
			  trie_split_states_e_t *state,
			  int max_split_count);

extern int _trie_v6_merge(trie_node_t *parent_trie,
                          trie_node_t *child_trie,
                          unsigned int *child_pivot,
                          unsigned int length,
                          trie_node_t **new_parent);

extern int _trie_v6_split2(trie_node_t *trie,
                           unsigned int *key,
                           unsigned int key_len,
                           unsigned int *pivot,
                           unsigned int *pivot_len,
                           unsigned int *split_count,
                           trie_node_t **split_node,
                           trie_node_t **child,
                           trie_split2_states_e_t *state,
                           const int max_split_count,
                           const int exact_same);

extern int _trie_v6_propagate_prefix(trie_node_t *trie,
				     unsigned int *pfx,
				     unsigned int len,
				     unsigned int add, /* 0-del/1-add */
				     trie_propagate_cb_f cb,
				     trie_bpm_cb_info_t *cb_info);

extern int trie_v6_pivot_propagate_prefix(trie_node_t *pivot,
					  unsigned int pivot_len,
					  unsigned int *pfx,
					  unsigned int len,
					  unsigned int add, /* 0-del/1-add */
					  trie_propagate_cb_f cb,
					  trie_bpm_cb_info_t *cb_info);

extern int pvt_trie_v6_propagate_prefix(trie_node_t *pivot,
                    unsigned int pivot_len,
                    unsigned int *pfx,
                    unsigned int len,
                    trie_propagate_cb_f cb,
                    trie_bpm_cb_info_t *cb_info);

extern int tmu_trie_v6_split_ut(unsigned int seed);

extern int tmu_taps_trie_v6_ut(int id, unsigned int seed);

extern int tmu_taps_bpm_trie_v6_ut(int id, unsigned int seed);

#endif /* ALPM_IPV6_128_SUPPORT */
#endif /* ALPM_ENABLE */
#endif /* _ESW_TRIDENT2_ALPM_TRIE_V6_H_ */
