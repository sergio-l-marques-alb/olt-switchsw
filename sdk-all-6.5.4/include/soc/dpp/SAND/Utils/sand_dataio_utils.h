/*
 * $Id: dataio_utils.h,v 1.00 Broadcom SDK $
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
 * File:    dataio_utils.h
 * Purpose:    Types and structures used when working with data IO
 */

#ifndef __SAND_DATAIO_UTILS_H
#define __SAND_DATAIO_UTILS_H

#ifdef __KERNEL__
#include <linux/string.h>
#else
#include <string.h>
#endif

#ifndef DUNE_BCM
#define SOC_SAND_OK        0
#define SOC_SAND_ERR    1

typedef enum {
    _SHR_E_NONE                 = 0,
    _SHR_E_INTERNAL             = -1,
    _SHR_E_MEMORY               = -2,
    _SHR_E_UNIT                 = -3,
    _SHR_E_PARAM                = -4,
    _SHR_E_EMPTY                = -5,
    _SHR_E_FULL                 = -6,
    _SHR_E_NOT_FOUND            = -7,
    _SHR_E_EXISTS               = -8,
    _SHR_E_TIMEOUT              = -9,
    _SHR_E_BUSY                 = -10,
    _SHR_E_FAIL                 = -11,
    _SHR_E_DISABLED             = -12,
    _SHR_E_BADID                = -13,
    _SHR_E_RESOURCE             = -14,
    _SHR_E_CONFIG               = -15,
    _SHR_E_UNAVAIL              = -16,
    _SHR_E_INIT                 = -17,
    _SHR_E_PORT                 = -18,

    _SHR_E_LIMIT                = -19           /* Must come last */
} _shr_error_t;

#define sal_memcpy memcpy
#define cli_out    printf
#else  /* DUNE_BCM */
#include <shared/error.h>
#include <shared/bsl.h>
#include <sal/core/alloc.h>
#include <sal/types.h>
#endif /* DUNE_BCM */

/* Misc defines */
#define  RHFILE_MAX_SIZE      128
#define  RHNAME_MAX_SIZE      64
#define  RHKEYWORD_MAX_SIZE   12

#define ISEMPTY(name_macro)     ((name_macro == NULL) || (name_macro[0] == 0))
#define SET_EMPTY(name_macro)   (name_macro[0] = 0)
#define VALUE(value_string)     (*((uint32 *)value_string))

char  **dataio_split_mstr(char *string, char *delim, uint32 maxtokens, uint32 *realtokens);
void    dataio_free_tokens(char **tokens, uint32 token_num);
void    dataio_swap_long(uint8 *buffer, int uint32_num);
int     dataio_pow(int n, int k);
void    dataio_replace_white_space(char *dest, char *source);
void    dataio_fillstr(char *str, int num, char ch);
void   *dataio_alloc(unsigned int size);

typedef enum {
    FILETYPE_NONE,        /* No .ext for in filename         */
    FILETYPE_UKNOWN,      /* There is an extension but not the one known */
    FILETYPE_AMB,         /* Multiple dots in filename      */
    FILETYPE_CSV,         /* Comma separated format         */
    FILETYPE_XML,         /* XML format                     */
    FILETYPE_VERILOG,     /* Verilog format                 */
    FILETYPE_MAX          /* Must come last                 */
} FILETYPE_E;

FILETYPE_E dataio_get_filetype(char *file_n);

uint32   dataio_dir_exists(char *path);
int      dataio_get_filepath(char *dir, char *name, char *filepath);
uint32   dataio_file_exists(char *filename);

uint32 dataio_stoul(char *str, uint32 *value_p);

#endif /* __SAND_DATAIO_UTILS_H */
