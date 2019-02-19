/*
 * $Id: dpp_signals_cint_data.c,v 1.19 Broadcom SDK $
 *
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
 * Hand-coded support for dpp_dsig_get routine and may be other
 *
 */

#if defined(INCLUDE_LIB_CINT) && defined(BCM_PETRA_SUPPORT)

#include <cint_config.h>
#include <cint_types.h>
#include <cint_porting.h>
#include <soc/defs.h>
#include <sal/core/libc.h>

#include "appl/diag/dcmn/dpp_debug_signals.h"

/* Macros section */

CINT_FWRAPPER_CREATE_RP4(int, int, 0, 0, dpp_dsig_get, int, int, unit, 0, 0,
        int, int, core, 0, 0, match_t*, match_t, match, 1, 0, signal_output_t*,
        signal_output_t, signal_output, 1, 0);

/* Functions and Macros mapping*/
static cint_function_t __cint_functions[] = {
CINT_FWRAPPER_ENTRY(dpp_dsig_get), { NULL } };

/* Structs section */
static void*
__cint_maddr__match_t(void* p, int mnum, cint_struct_type_t* parent) {
    void* rv;
    match_t *s = (match_t *) p;
    switch (mnum) {
    case 0:
        rv = &(s->block);
        break;
    case 1:
        rv = &(s->from);
        break;
    case 2:
        rv = &(s->to);
        break;
    case 3:
        rv = &(s->name);
        break;
    case 4:
        rv = &(s->flags);
        break;
    case 5:
        rv = &(s->output_order);
        break;
    default:
        rv = NULL;
        break;
    }
    return rv;
}

static cint_parameter_desc_t __cint_struct_members__match_t[] = { { "char",
        "block", 1, 0 }, { "char", "from", 1, 0 }, { "char", "to", 1, 0 }, {
        "char", "name", 1, 0 }, { "int", "flags", 0, 0 }, { "int",
        "output_order", 0, 0 }, { NULL } };

static void*
__cint_maddr__signal_address_t(void* p, int mnum, cint_struct_type_t* parent) {
    void* rv;
    signal_address_t *s = (signal_address_t *) p;
    switch (mnum) {
    case 0:
        rv = &(s->high);
        break;
    case 1:
        rv = &(s->low);
        break;
    case 2:
        rv = &(s->msb);
        break;
    case 3:
        rv = &(s->lsb);
        break;
    default:
        rv = NULL;
        break;
    }
    return rv;
}

static cint_parameter_desc_t __cint_struct_members__signal_address_t[] = { {
        "int", "high", 0, 0 }, { "int", "low", 0, 0 }, { "int", "msb", 0, 0 }, {
        "int", "lsb", 0, 0 }, { NULL } };

static void*
__cint_maddr__rhentry_t(void* p, int mnum, cint_struct_type_t* parent) {
    void* rv;
    rhentry_t *s = (rhentry_t *) p;
    switch (mnum) {
    case 0:
        rv = &(s->next);
        break;
    case 1:
        rv = &(s->prev);
        break;
    case 2:
        rv = &(s->id);
        break;
    case 3:
        rv = &(s->name);
        break;
    case 4:
        rv = &(s->users);
        break;
    case 5:
        rv = &(s->mode);
        break;
    default:
        rv = NULL;
        break;
    }
    return rv;
}

static cint_parameter_desc_t __cint_struct_members__rhentry_t[] = { {
        "rhentry_t", "next", 1, 0 }, { "rhentry_t", "prev", 1, 0 }, { "rhid",
        "id", 0, 0 }, { "char", "name", 0, RHNAME_MAX_SIZE }, { "int", "users",
        0, 0 }, { "int", "mode", 0, 0 }, { NULL } };

static void*
__cint_maddr__rhlist_t(void* p, int mnum, cint_struct_type_t* parent) {
    void* rv;
    rhlist_t *s = (rhlist_t *) p;
    switch (mnum) {
    case 0:
        rv = &(s->entry);
        break;
    case 1:
        rv = &(s->top);
        break;
    case 2:
        rv = &(s->tail);
        break;
    case 3:
        rv = &(s->num);
        break;
    case 4:
        rv = &(s->max_id);
        break;
    case 5:
        rv = &(s->entry_size);
        break;
    case 6:
        rv = &(s->sanity);
        break;
    default:
        rv = NULL;
        break;
    }
    return rv;
}

static cint_parameter_desc_t __cint_struct_members__rhlist_t[] = { {
        "rhentry_t", "entry", 0, 0 }, { "rhentry_t", "top", 1, 0 }, {
        "rhentry_t", "tail", 1, 0 }, { "int", "num", 0, 0 }, { "int", "max_id",
        0, 0 }, { "int", "entry_size", 0, 0 }, { "int", "sanity", 0, 0 },
        { NULL } };

static void*
__cint_maddr__debug_signal_t(void* p, int mnum, cint_struct_type_t* parent) {
    void* rv;
    debug_signal_t *s = (debug_signal_t *) p;
    switch (mnum) {
    case 0:
        rv = &(s->size);
        break;
    case 1:
        rv = &(s->block_id);
        break;
    case 2:
        rv = &(s->address);
        break;
    case 3:
        rv = &(s->range_num);
        break;
    case 4:
        rv = &(s->changeable);
        break;
    case 5:
        rv = &(s->double_flag);
        break;
    case 6:
        rv = &(s->expansion);
        break;
    case 7:
        rv = &(s->resolution);
        break;
    case 8:
        rv = &(s->from);
        break;
    case 9:
        rv = &(s->to);
        break;
    case 10:
        rv = &(s->block_n);
        break;
    case 11:
        rv = &(s->attribute);
        break;
    case 12:
        rv = &(s->verilog);
        break;
    case 13:
        rv = &(s->cond_attribute);
        break;
    case 14:
        rv = &(s->cond_value);
        break;
    case 15:
        rv = &(s->cond_signal);
        break;
    default:
        rv = NULL;
        break;
    }
    return rv;
}

static cint_parameter_desc_t __cint_struct_members__debug_signal_t[] = { {
        "int", "size", 0, 0 }, { "int", "block_id", 0, 0 }, {
        "signal_address_t", "address", 0, MAX_ADDRESS_RANGE_NUM }, { "int",
        "range_num", 0, 0 }, { "int", "changeable", 0, 0 }, { "int",
        "double_flag", 0, 0 }, { "char", "expansion", 0, RHNAME_MAX_SIZE }, {
        "char", "resolution", 0, RHNAME_MAX_SIZE }, { "char", "from", 0,
        RHNAME_MAX_SIZE }, { "char", "to", 0, RHNAME_MAX_SIZE }, { "char",
        "block_n", 0, RHNAME_MAX_SIZE }, { "char", "attribute", 0,
        RHNAME_MAX_SIZE }, { "char", "verilog", 0, RHVERILOG_MAX_SIZE }, {
        "char", "cond_attribute", 0, RHNAME_MAX_SIZE }, { "int", "cond_value",
        0, 0 }, { "struct debug_signal_s", "cond_signal", 1, 0 }, { NULL } };

static void *
__cint_maddr__signal_output_t(void* p, int mnum, cint_struct_type_t* parent) {
    void* rv;
    signal_output_t *s = (signal_output_t *) p;
    switch (mnum) {
    case 0:
        rv = &(s->entry);
        break;
    case 1:
        rv = &(s->debug_signal);
        break;
    case 2:
        rv = &(s->size);
        break;
    case 3:
        rv = &(s->value);
        break;
    case 4:
        rv = &(s->print_value);
        break;
    case 5:
        rv = &(s->field_list);
        break;
    default:
        rv = NULL;
        break;
    }
    return rv;
}

static cint_parameter_desc_t __cint_struct_members__signal_output[] = { {
        "rhentry_t", "entry", 0, 0 },
        { "debug_signal_t", "debug_signal", 1, 0 }, { "int", "size", 0, 0 }, {
                "uint32", "value", 0, 0 }, { "char", "print_value", 1, 0 }, {
                "rhlist_t", "field_list", 0, 0 }, { NULL } };

static cint_struct_type_t __cint_structures[] = { { "match_t", sizeof(match_t),
        __cint_struct_members__match_t, __cint_maddr__match_t },
        { "signal_address_t", sizeof(signal_address_t),
                __cint_struct_members__signal_address_t,
                __cint_maddr__signal_address_t }, { "rhentry_t",
                sizeof(rhentry_t), __cint_struct_members__rhentry_t,
                __cint_maddr__rhentry_t }, { "rhlist_t", sizeof(rhlist_t),
                __cint_struct_members__rhlist_t, __cint_maddr__rhlist_t }, {
                "debug_signal_t", sizeof(debug_signal_t),
                __cint_struct_members__debug_signal_t,
                __cint_maddr__debug_signal_t }, { "signal_output_t",
                sizeof(signal_output_t), __cint_struct_members__signal_output,
                __cint_maddr__signal_output_t }, { NULL } };

/* Enums section */

static cint_enum_type_t __cint_enums[] = { { NULL } };

/* Defines section */
static cint_constants_t __cint_constants[] = { { NULL } };

/* Typedefs section */
static cint_parameter_desc_t __cint_typedefs[] = { { "unsigned int", "uint32",
        0, 0 }, { "int", "rhid", 0, 0 }, { NULL } };

/* Functions pointers section */
/*static cint_function_pointer_t __cint_function_pointers[0];*/

cint_data_t dpp_signals_cint_data = {
NULL, __cint_functions, __cint_structures, __cint_enums, __cint_typedefs,
        __cint_constants,
        NULL };

#else
int _dpp_signals_data_not_empty;
#endif /* defined(INCLUDE_LIB_CINT) & defined(BCM_PETRA_SUPPORT) */
