/* $Id: ui_pure_defi_diag.h,v 1.2 Broadcom SDK $
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
*/
#ifndef UI_PURE_DEFI_DIAG_INCLUDED
/* { */
#define UI_PURE_DEFI_DIAG_INCLUDED
/*
 * Note:
 * the following definitions must range between PARAM_DIAG_START_RANGE_ID (9300)
 * and PARAM_DIAG_END_RANGE_ID (9400).
 * See ui_pure_defi.h
 */
#define PARAM_DIAG_HW_ID                   (PARAM_DIAG_START_RANGE_ID + 1)
#define PARAM_DIAG_HW_MEM_ID               (PARAM_DIAG_START_RANGE_ID + 2)
#define PARAM_DIAG_HW_MEM_BASE_ADDR_ID     (PARAM_DIAG_START_RANGE_ID + 3)
#define PARAM_DIAG_HW_MEM_SIZE_ID          (PARAM_DIAG_START_RANGE_ID + 4)
#define PARAM_DIAG_HW_MEM_REVERT_DATA_ID   (PARAM_DIAG_START_RANGE_ID + 5)
#define PARAM_DIAG_HW_MEM_REVERT_OFFSET_ID (PARAM_DIAG_START_RANGE_ID + 6)
#define PARAM_DIAG_HW_MEM_DATA_WALK_ID     (PARAM_DIAG_START_RANGE_ID + 7)
#define PARAM_DIAG_HW_MEM_OFFSET_WALK_ID   (PARAM_DIAG_START_RANGE_ID + 8)
#define PARAM_DIAG_HW_MEM_ALIGNMENT_ID     (PARAM_DIAG_START_RANGE_ID + 9)

#define PARAM_DIAG_HW_MEM_SET_SPACE_ID     (PARAM_DIAG_START_RANGE_ID + 10)
#define PARAM_DIAG_HW_MEM_SPACE_ZBT_ID     (PARAM_DIAG_START_RANGE_ID + 11)
#define PARAM_DIAG_HW_MEM_SPACE_DRAM_ID    (PARAM_DIAG_START_RANGE_ID + 12)
#define PARAM_DIAG_HW_MEM_SPACE_CPU_ID     (PARAM_DIAG_START_RANGE_ID + 13)

#define PARAM_DIAG_VERBOSE_ID              (PARAM_DIAG_START_RANGE_ID + 14)

#define PARAM_DIAG_HW_MEM_PATTERN_ID       (PARAM_DIAG_START_RANGE_ID + 15)

/* } */
#endif
