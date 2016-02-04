/*
 * $Id: $
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
 *
 * File: sw_state_def_init_indexes.h
 */
#ifndef _SHR_SW_STATE_DEFS_INIT_INDEXES_H
#define _SHR_SW_STATE_DEFS_INIT_INDEXES_H

#define INIT_INDEXES() 		uint8 is_param_allocated = 0; \
		int temp_size = 0; \
        uint32 allocated_size = 0; \
        uint8 is_allocated = 0; \
		int idx0 = 0; \
        int idx1 = 0; \
        int idx2 = 0; \
        int idx3 = 0; \
		uint32 *ptr0 = NULL; \
        uint32 *ptr1 = NULL; \
        uint32 *ptr2 = NULL; \
        uint32 *ptr3 = NULL; \
		int toIdx0 = 0; \
        int toIdx1 = 0; \
        int toIdx2 = 0; \
        int toIdx3 = 0; \
        is_param_allocated  = is_param_allocated + 0; \
        is_allocated = is_allocated + 0;\
        temp_size = temp_size + 0; \
		toIdx0 = toIdx0 + 0; \
        toIdx1 = toIdx1 + 0; \
        toIdx2 = toIdx2 + 0; \
        toIdx3 = toIdx3 + 0; \
        idx0 = idx0 + 0; \
        idx1 = idx1 + 0; \
        idx2 = idx2 + 0; \
        idx3 = idx3 + 0; \
		ptr0 = ptr0 + 0; \
        ptr1 = ptr1 + 0; \
        ptr2 = ptr2 + 0; \
        ptr3 = ptr3 + 0; \
        allocated_size = allocated_size + 0
#endif /* _SHR_SW_STATE_DEFS_INIT_INDEXES_H */
