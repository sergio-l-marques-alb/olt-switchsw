/*
 * $Id: cache_mem_test.h,v 1.0 2014/08/14 MiryH Exp $
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
 */

#ifndef _CACHE_MEM_TEST_H
#define _CACHE_MEM_TEST_H

#if defined(BCM_ESW_SUPPORT) || defined(BCM_SBX_SUPPORT) || defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT) || defined (BCM_ARAD_SUPPORT)

#include <appl/diag/shell.h>

int		do_cache_mem_test(int , args_t *a, void *);
int		do_cache_mem_test_init(int, args_t *, void **);
int		do_cache_mem_test_done(int, void *);



typedef enum cache_mem_test_write_value_pattern_e {
	cache_mem_test_write_value_pattern_all_ones = 0,
	cache_mem_test_write_value_pattern_all_zeroes,
	cache_mem_test_write_value_pattern_incremental,
	cache_mem_test_write_value_pattern_smart
}cache_mem_test_write_value_pattern_t;

typedef enum cache_mem_test_type_e {
	cache_mem_test_type_single = 0,
	cache_mem_test_type_all_mems
}cache_mem_test_type_t;

typedef enum cache_mem_test_write_type_e {
	cache_mem_test_write_type_dma = 0,
	cache_mem_test_write_type_schan
}cache_mem_test_write_type_t;

typedef enum cache_mem_test_partial_e {
	cache_mem_test_full = 0,
	cache_mem_test_write_only,
	cache_mem_test_read_only,
	cache_mem_test_cache_only
}cache_mem_test_partial_t;

typedef struct tr_do_cache_mem_test_e
{
	cache_mem_test_type_t				test_type;
	cache_mem_test_write_type_t			write_type;
	cache_mem_test_write_value_pattern_t write_value_pattern;
	cache_mem_test_partial_t			test_part;
    int                                 stat_mem_not_tested_cnt;
    int                                 stat_mem_succeed_cnt;
    int                                 stat_mem_fail_cnt;
    int                                 stat_mem_total_cnt;
	uint32								mem_id;
	cmd_result_t						result;
} tr_do_cache_mem_test_t;

#endif /*#(BCM_ESW_SUPPORT) || (BCM_SBX_SUPPORT) || (BCM_PETRA_SUPPORT) || (BCM_DFE_SUPPORT) ||  (BCM_ARAD_SUPPORT) */
#endif /*_CACHE_MEM_TEST_H*/

