/*
 * $Id: diag_prt_print.h,v 1.00 Broadcom SDK $
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
 *
 * File:    diag_prt_print.h
 * Purpose: Types and structures used when working with data IO
 */

#ifndef __DIAG_PRT_PRINT_H
#define __DIAG_PRT_PRINT_H

#if !defined(__KERNEL__)
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#endif
#include <soc/dpp/SAND/Utils/sand_rhlist.h>
#include <soc/dpp/SAND/Utils/sand_dataio_utils.h>

typedef struct {
    uint32  width;
    char    format[RHKEYWORD_MAX_SIZE];
} column_t;

void diag_prt_print(rhlist_t *prt_list, int column_num);

/* Modes for list entries */
#define    LINE_NONE            0
#define    LINE_UNDERSCORE      1
#define    LINE_EQUAL           2

/* Facilities for pretty print */
#define  RHCOLUMN_MAX_SIZE    (RHNAME_MAX_SIZE + 1)

#define MAX_SHOW_COLUMNS            30

#define PRT_INIT_LOCAL              rhlist_t *prt_list = NULL;                                              \
                                    char *prt_line_offset = NULL;                                           \
                                    void *prt_line_start = NULL;                                            \
                                    int prt_column_num = 0;

#define PRT_COPY(mc_format)       { sal_strcpy(prt_line_offset, mc_format);                                 \
                                    prt_line_offset[RHCOLUMN_MAX_SIZE - 1] = 0;                             \
                                    prt_line_offset += RHCOLUMN_MAX_SIZE;                                   \
                                  }

#define PRT_PRINT(mc_format, ...) { snprintf(prt_line_offset, RHCOLUMN_MAX_SIZE, mc_format, ##__VA_ARGS__); \
                                    prt_line_offset[RHCOLUMN_MAX_SIZE - 1] = 0;                             \
                                    prt_line_offset += RHCOLUMN_MAX_SIZE;                                   \
                                  }

#define PRT_SKIP(mc_skip_num)       prt_line_offset += (mc_skip_num) * (RHCOLUMN_MAX_SIZE);
#define PRT_GET_CUR_COLUMN          (prt_line_offset - ((char *)prt_line_start + sizeof(rhentry_t)))/RHCOLUMN_MAX_SIZE;
#define PRT_SET_COLUMN_NUM          prt_column_num = PRT_GET_CUR_COLUMN

#define PRT_ALLOC_LINE(mc_mode)   { if(prt_list == NULL) {                                                                                  \
                                        prt_list = rhlist_create("prt_print", sizeof(rhentry_t) + RHCOLUMN_MAX_SIZE * MAX_SHOW_COLUMNS, 0); \
                                        if(prt_list == NULL)                                                                                \
                                            return -SOC_SAND_ERR;                                                                           \
                                    }                                                                                                       \
                                    if(rhlist_add_tail(prt_list, NULL, RHID_TO_BE_GENERATED, &prt_line_start) == -SOC_SAND_ERR) {          \
                                        rhlist_free_all(prt_list);                                                                          \
                                        return -SOC_SAND_ERR;                                                                               \
                                    }                                                                                                       \
                                    ((rhentry_t *)prt_line_start)->mode = mc_mode;                                                          \
                                    prt_line_offset = (char *)prt_line_start + sizeof(rhentry_t);                                           \
                                  }

#define PRT_FREE                    rhlist_free_all(prt_list);
#define PRT_STAMP                   diag_prt_print(prt_list, prt_column_num);

#endif /* __DIAG_PRT_PRINT_H */
