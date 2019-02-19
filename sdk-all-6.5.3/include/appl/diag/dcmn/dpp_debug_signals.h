/*
 * $Id: dpp_debug_signals.h,v 1.00 Broadcom SDK $
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
 * File:    dpp_debug_signals.h
 * Purpose: Interface to debug_signal database
 */

#ifndef __DPP_DEBUG_SIGNALS_H
#define __DPP_DEBUG_SIGNALS_H

#include <shared/libxml/xml_api.h>
#include <soc/dpp/SAND/Utils/sand_rhlist.h>
#include <soc/dpp/SAND/Utils/sand_dataio_utils.h>

#define PRINT_LITTLE_ENDIAN 0
#define PRINT_BIG_ENDIAN    1

#define MAX_ADDRESS_RANGE_NUM   5
#define MAX_DEBUG_SIGNAL_SIZE   128
#define MAX_DEBUG_SIGNAL_INT_SIZE   MAX_DEBUG_SIGNAL_SIZE/4
#define MAX_PP_BLOCK_NUM        4
#define MAX_PP_STAGE_NUM        20
#define OPTION_PARAM_MAX_NUM    5

#define  RHVERILOG_MAX_SIZE   128
#define  RHCHUNK_MAX_SIZE     RHNAME_MAX_SIZE * 4 /* Each 4 bits are mapped to 1 character in print */
#define  RHSIGADDRESS_MAX_SIZE  200

#define ROUND_TO_N(mc_size, mc_num) ((mc_size % mc_num) ? (mc_size / mc_num + 1) : (mc_size / mc_num))

typedef struct {
    int high;
    int low;
    int msb;
    int lsb;
} signal_address_t;

typedef struct debug_signal_s {
    /* Static Data */
    int size;
    int block_id;
    signal_address_t address[MAX_ADDRESS_RANGE_NUM];
    int range_num;
    int changeable;
    int double_flag;
    char expansion[RHNAME_MAX_SIZE];
    char resolution[RHNAME_MAX_SIZE];
    char from[RHNAME_MAX_SIZE];
    char to[RHNAME_MAX_SIZE];
    char block_n[RHNAME_MAX_SIZE];
    char attribute[RHNAME_MAX_SIZE];
    char verilog[RHVERILOG_MAX_SIZE];
    char cond_attribute[RHNAME_MAX_SIZE];
    int  cond_value;
    struct debug_signal_s *cond_signal;
} debug_signal_t;

typedef struct {
    rhentry_t      entry;
    debug_signal_t *debug_signal;
    int            size;
    uint32         value[MAX_DEBUG_SIGNAL_INT_SIZE];
    char           print_value[MAX_DEBUG_SIGNAL_SIZE * 2 + 8];
    rhlist_t       *field_list;
} signal_output_t;

typedef struct {
    char *block;
    char *from;
    char *to;
    char *name;
    int flags;
    int output_order;
} match_t;

typedef struct {
    int offset;
    int size;
    int buffer;
    char name[RHNAME_MAX_SIZE];
    char verilog[RHVERILOG_MAX_SIZE];
} internal_signal_t;

typedef struct {
    char               name[RHNAME_MAX_SIZE];
    char               programmable[RHKEYWORD_MAX_SIZE];
    internal_signal_t *signals;
    int                number;
    int                buffer0_size;
    int                buffer1_size;
} pp_stage_t;

typedef struct {
    char               name[RHNAME_MAX_SIZE];
    int                stage_num;
    pp_stage_t        *stages;
    char               debug_signals_n[RHNAME_MAX_SIZE];
    debug_signal_t    *debug_signals;
    int                signal_num;
} pp_block_t;

#define SIGNALS_PRINT_VERILOG   0x1
#define SIGNALS_PRINT_VALUE     0x2
#define SIGNALS_PRINT_EXPAND    0x4
#define SIGNALS_PRINT_FULL      0x8

#define SIGNALS_MATCH_EXACT     0x1
#define SIGNALS_MATCH_NOCOND    0x2
#define SIGNALS_MATCH_DOUBLE    0x4

#define QUAL_BUFFER_LSB      0x0
#define QUAL_BUFFER_MSB      0x1

#define EXPANSION_STATIC    0
#define EXPANSION_DYNAMIC   1

/* Sigstruct section */
typedef struct {
    char              name[RHNAME_MAX_SIZE];
    attribute_param_t param[OPTION_PARAM_MAX_NUM];
} expansion_option_t;

typedef struct {
    char    name[RHNAME_MAX_SIZE];
    expansion_option_t *options;
    int     option_num;
} expansion_t;

typedef struct {
    char    name[RHNAME_MAX_SIZE];
    int     start_bit;
    int     end_bit;
    int 	size;
    char resolution[RHNAME_MAX_SIZE];
    char cond_attribute[RHNAME_MAX_SIZE];
    int  cond_value;
    expansion_t expansion_m;
} sigstruct_field_t;

typedef struct {
    char               name[RHNAME_MAX_SIZE];
    int                size;
    sigstruct_field_t *fields;
    int                field_num;
    expansion_t        expansion_m;
} sigstruct_t;

typedef struct {
    char    name[RHNAME_MAX_SIZE];
    int     value;
} sigparam_value_t;

typedef struct {
    char                name[RHNAME_MAX_SIZE];
    int                 size;
    char                default_str[RHNAME_MAX_SIZE];
    sigparam_value_t   *values;
    int                 value_num;
} sigparam_t;

int     dpp_signals_init(char *dpp_device_descr);

void    dpp_debug_signals_check(void);

/* New approach to signal output allowing field matching */
int     dpp_qual_signal_get(char *stage_n, int offset, int size, int buffer, char *qual_name);
int     dpp_qual_signal_print(match_t *match, int flags);

int     dpp_stage_print(void);
int     dpp_stage_size(char *stage_n, int buffer);

int     dpp_dsig_get(int unit, int core,  match_t *match, signal_output_t *signal_output);
int     dpp_dsig_get_list(int unit, int core, match_t *match, rhlist_t *dsig_list);
int     dpp_dsig_print(rhlist_t *dsig_list, int flags);
void    dpp_dsig_free(rhlist_t *output_list);

int     dpp_sigstruct_print(char *match_n);
int     dpp_sigparam_print(char *match_n);
int     dpp_sigparam_get(char *signal_n, uint32 value, char *value_n);
#endif /* END of __DPP_DEBUG_SIGNALS_H */
