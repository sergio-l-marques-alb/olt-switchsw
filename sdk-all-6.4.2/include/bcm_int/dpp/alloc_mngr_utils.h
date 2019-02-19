/*
 * $Id: alloc_mngr_utils.h,v 1.45 Broadcom SDK $
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
 * File:        alloc_mngr_utils.h
 * Purpose:     Utils for resource allocation. Especially concerning multi core devices.
 *
 */

#ifndef  INCLUDE_ALLOC_MNGR_UTILS_H
#define  INCLUDE_ALLOC_MNGR_UTILS_H


#define BCM_DPP_AM_DEFAULT_NOF_POOLS    1
#define BCM_DPP_AM_DEFAULT_CORE_ID      0
#define BCM_DPP_AM_INVALID_CORE_ID      (-1)


/*
 * GENERAL CORE HANDLING UTILS - START
 * 
 */

int 
bcm_dpp_am_resource_to_nof_pools(int unit, int res_type, uint8 *nof_pools);

int 
bcm_dpp_am_template_to_nof_pools(int unit, int res_type, uint8 *nof_pools);

int
bcm_dpp_am_resource_base_pool_id_set(int unit, int resource_id, int base_pool_id);

int
bcm_dpp_am_template_base_pool_id_set(int unit, int template_id, int pool_id);

int
bcm_dpp_am_resource_id_to_pool_id_get(int unit, int core_id, int resource_id, int *pool_id);

int
bcm_dpp_am_template_id_to_pool_id_get(int unit, int core_id, int template_id, int *pool_id);

int 
bcm_dpp_am_pool_id_to_resource_id_get(int unit, int pool_id, int *res_id, int *core_id);

int 
bcm_dpp_am_pool_id_to_template_id_get(int unit, int pool_id, int *template_id, int *core_id);


/*
 * GENERAL CORE HANDLING UTILS - END
 * 
 */


/*
 * RESOURCE MANAGER ENCAPSULATION - START
 * 
 */

int 
dpp_am_res_free(int unit, int core_id, int res_id, int count, int elem);

int 
dpp_am_res_free_and_status(int unit, int core_id, int res_id, int count, int elem, uint32 *flags);

int 
dpp_am_res_alloc(int unit, int core_id, int res_id, uint32 flags, int count, int *elem);

int 
dpp_am_res_alloc_align(int unit, int core_id, int res_id, uint32 flags, int align, int offset, int count, int *elem);

int
dpp_am_res_alloc_align_tag(int unit, int core_id, int res_id, uint32 flags, int align, int offset, const void *tag, int count, int *elem);

int
dpp_am_res_check(int unit, int core_id, int res_id, int count, int elem);

/* Return the amount of used elements in res_id. */
int
dpp_am_res_used_count(int unit, int core_id, int res_id, int *used_count);

int
dpp_am_res_check_all(int unit, int core_id, int res_id, int count, int elem);

int
dpp_am_res_set_pool(int unit, int core_id, int res_id, shr_res_allocator_t manager, 
                                 int low_id, int count, const void *extras, const char *name);

int
dpp_am_res_unset_pool(int unit, int core_id, int res_id);

/*
 * RESOURCE MANAGER ENCAPSULATION - END
 * 
 */


/*
 * TEMPLATE MANAGER ENCAPSULATION - START
 * 
 */


int 
dpp_am_template_allocate(int unit, int core_id, int template_id, uint32 flags, const void *data, int *is_allocated, int *template);

int 
dpp_am_template_allocate_group(int unit, int core_id, int template_id, uint32 flags, const void *data, int nof_additions, int *is_allocated, int *template);

int
dpp_am_template_exchange(int unit, int core_id, int template_id, uint32 flags, const void *data, int old_template,
                                      int *is_last, int *template, int *is_allocated);

int
dpp_am_template_exchange_test(int unit, int core_id, int template_id, uint32 flags, const void *data, int old_template,
                                      int *is_last, int *template, int *is_allocated);

int
dpp_am_template_free(int unit, int core_id, int template_id, int template, int *is_last);

int 
dpp_am_template_clear(int unit, int core_id, int template_id);

int
dpp_am_template_data_get(int unit, int core_id, int template_id, int template, void *data);

int
dpp_am_template_template_get(int unit, int core_id, int template_id, const void *data, int *template);

int
dpp_am_template_ref_count_get(int unit, int core_id, int template_id, int template, uint32 *ref_count);


/*
 * TEMPLATE MANAGER ENCAPSULATION - END
 * 
 */

#endif /*INCLUDE_ALLOC_MNGR_UTILS_H*/

