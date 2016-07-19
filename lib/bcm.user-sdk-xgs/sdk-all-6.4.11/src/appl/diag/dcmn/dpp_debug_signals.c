/*
 * $Id: dpp_debug_signals.c,v 1.00 Broadcom SDK $
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
 * File:    dpp_debug_signals.c
 * Purpose:    Routines for handling debug and internal signals
 */

#include <sal/appl/sal.h>
#include <shared/bitop.h>
#include <appl/diag/dcmn/diag_prt_print.h>
#include <appl/diag/dcmn/dpp_debug_signals.h>

#ifdef BCM_DPP_SUPPORT
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_diag.h>
#endif

pp_block_t *pp_blocks = NULL;
int block_num = 0;

sigstruct_t *sigstructs = NULL;
int sigstruct_num = 0;

sigparam_t *sigparams = NULL;
int sigparam_num = 0;

static sigstruct_t *dpp_sigstruct_get(char *signal_n)
{
    int i;
    sigstruct_t *cur_sigstruct;

    if (sigstructs == NULL) {
        /* Not initialized yet */
        cli_out("Signal Struct DB was not initialized\n");
        return 0;
    }

    for (i = 0; i < sigstruct_num; i++) {
        cur_sigstruct = &sigstructs[i];
        if (!sal_strcasecmp(cur_sigstruct->name, signal_n))
            return cur_sigstruct;
    }

    return NULL;
}

static int
dpp_stage_exists(char *stage_n)
{
    int i, k;
    pp_block_t *cur_pp_block;
    pp_stage_t *cur_pp_stage;

    for (i = 0; i < block_num; i++) {
        cur_pp_block = &pp_blocks[i];

        for (k = 0; k < cur_pp_block->stage_num; k++) {
            cur_pp_stage = &cur_pp_block->stages[k];

            if (!sal_strcasecmp(stage_n, cur_pp_stage->name))
                return 1;
        }
    }
    return 0;
}

static sigparam_t *dpp_sigresolve_get(char *signal_n)
{
    int i;
    sigparam_t *cur_sigparam;

    if (sigparams == NULL) {
        /* Not initialized yet */
        cli_out("Signal Struct DB was not initialized\n");
        return 0;
    }

    for (i = 0; i < sigparam_num; i++) {
        cur_sigparam = &sigparams[i];
        if (!sal_strcasecmp(cur_sigparam->name, signal_n))
            return cur_sigparam;
    }

    return NULL;
}

static void
dpp_dsig_get_value(int unit, int core_id, debug_signal_t *debug_signal, uint32 *value)
{
#ifdef BCM_DPP_SUPPORT
    int j;
    ARAD_PP_DIAG_REG_FIELD debug_field;
    signal_address_t *address;
    uint32 range_val[ARAD_PP_DIAG_DBG_VAL_LEN];
    int range_size;
    int last_pos;
    int uint_num;

    uint_num = ROUND_TO_N(debug_signal->size, 32);
    if (uint_num > MAX_DEBUG_SIGNAL_INT_SIZE) {
        cli_out("Bad signal_size:%d\n", debug_signal->size);
        uint_num = MAX_DEBUG_SIGNAL_INT_SIZE;
    }

    sal_memset(value, 0, sizeof(int) * uint_num);

    last_pos = 0;
    for (j = 0; j < debug_signal->range_num; j++) {
        address = &(debug_signal->address[j]);
        debug_field.base = (address->high << 16) + address->low;
        debug_field.lsb = address->lsb;
        debug_field.msb = address->msb;
        range_size = debug_field.msb + 1 - debug_field.lsb;
        arad_pp_diag_dbg_val_get_unsafe(unit, core_id, debug_signal->block_id, &debug_field, range_val);
        SHR_BITCOPY_RANGE(value, last_pos, range_val, 0, range_size);
        last_pos += range_size;
    }
#endif /* BCM_DPP_SUPPORT */
    return;
}

static void *dpp_get_top(char *descr, char *filename, char *topname, int flags)
{
    void *curTop = NULL;
    char filepath[RHFILE_MAX_SIZE];
    char dir[RHFILE_MAX_SIZE];
    int unit = 0;
    char *db_dir = NULL;

    if (ISEMPTY(filename)) {
        LOG_ERROR(BSL_LS_APPL_SHELL, (BSL_META_U(unit, "DB Resource not defined\n")));
        goto out;
    }

#if !defined(__KERNEL__)
    if ((db_dir = getenv("DPP_DB_PATH")) == NULL)
        db_dir = "./db";
#endif

    if (ISEMPTY(db_dir))
        db_dir = "./db";

    if(sal_strlen(db_dir) > (RHFILE_MAX_SIZE - 5)) {
        LOG_WARN(BSL_LS_APPL_SHELL, (BSL_META_U(unit, "PATH %s is too long\n"), db_dir));
        goto out;
    }

    if (dataio_dir_exists(db_dir) != SOC_SAND_OK) {
        LOG_VERBOSE(BSL_LS_APPL_SHELL, (BSL_META_U(unit, "Signal DB was not found in %s\n"), db_dir));
        goto out;
    }

    strcpy(dir, db_dir);
    if (!ISEMPTY(descr)) {
        strcat(dir, "/");
        if(sal_strlen(descr) > (RHFILE_MAX_SIZE - (sal_strlen(db_dir) + 5))) {
            LOG_WARN(BSL_LS_APPL_SHELL, (BSL_META_U(unit, "Directory %s is too long\n"), descr));
            goto out;
        }

        strcat(dir, descr);
    }

    if (dataio_dir_exists(dir) != SOC_SAND_OK) {
        LOG_WARN(BSL_LS_APPL_SHELL, (BSL_META_U(unit, "Device DB was not found in %s\n"), db_dir));
        goto out;
    }

    if (dataio_get_filepath(dir, filename, filepath) != SOC_SAND_OK) {
        goto out;
    }

    if ((curTop = xml_get_top(filepath, topname, flags)) == NULL) {
        LOG_WARN(BSL_LS_APPL_SHELL, (BSL_META_U(unit, "Cannot open %s\n"), filepath));
        goto out;
    }

out:
    return curTop;
}

static int
dpp_parse_address(char *full_address, signal_address_t *address)
{
    char **tokens;
    uint32 realtokens = 0;
    int i;

    if((tokens = dataio_split_mstr(full_address, "|", 5, &realtokens)) == NULL) {
        return 0;
    }

    for (i = 0; i < realtokens; i++) {
        sscanf(tokens[i], " {15'd%d,16'd%d} bits: [%d : %d]", &address->high,
                &address->low, &address->msb, &address->lsb);
        address++;
    }

    dataio_free_tokens(tokens, realtokens);

    return realtokens;
}

static int
dpp_parse_bit_range(char *bit_range, int *startbit_p, int *endbit_p)
{
    int startbit = -1, endbit = -1;
    if (strstr(bit_range, ":") == NULL) {
        sscanf(bit_range, "%d", &endbit);
        startbit = endbit;
    } else
        sscanf(bit_range, "%d:%d", &endbit, &startbit);

    if ((endbit < startbit) || (startbit >= 2048) || (endbit >= 2048)
            || (startbit < 0) || (endbit < 0))
        return _SHR_E_INTERNAL;

    if (startbit <= endbit) {
        *startbit_p = startbit;
        *endbit_p = endbit;
    } else {
        *startbit_p = endbit;
        *endbit_p = startbit;
    }

    return _SHR_E_NONE;
}

static void
dpp_buffer_to_str(char *resolution_n, uint32 *org_source, char *dest, int bit_size, int byte_order)
{
    int i, j;
    int byte_size, long_size;
    int dest_byte_index, source_byte_index, real_byte_index;
    uint8 *source = (uint8 *)org_source;

    byte_size = ROUND_TO_N(bit_size, 8);
    long_size = ROUND_TO_N(byte_size, 4);
    sal_memset(dest, 0, MAX_DEBUG_SIGNAL_SIZE * 2 + 8);

    if (!ISEMPTY(resolution_n) && (dpp_sigparam_get(resolution_n, *org_source, dest) == _SHR_E_NONE))
        return;

    dest_byte_index = 0;
    if (byte_order == PRINT_LITTLE_ENDIAN) {
        for (i = 0; i < long_size; i++) {
            for (j = 0; j < 4; j++) {
                source_byte_index = 4 * i + 3 - j;
                real_byte_index = 4 * i + j;
                if (real_byte_index >= byte_size)
                    continue;
                sprintf(&dest[2 * dest_byte_index], "%02x", source[source_byte_index]);
                dest_byte_index++;
            }
        }
    } else {
        for (i = 0; i < long_size; i++) {
            for (j = 0; j < 4; j++) {
                source_byte_index = 4 * (long_size - 1 - i) + j;
                real_byte_index = 4 * (long_size - 1 - i) + 3 - j;
                if (real_byte_index >= byte_size)
                    continue;
                sprintf(&dest[2 * dest_byte_index], "%02x", source[source_byte_index]);
                dest_byte_index++;
            }
        }
    }
}

int
dpp_qual_signal_get(char *stage_n, int offset, int size, int buffer, char *qual_name)
{
    int i, k, j;
    pp_block_t *cur_pp_block;
    pp_stage_t *cur_pp_stage;
    internal_signal_t *qual_signal;
    int res = _SHR_E_NOT_FOUND;

    if (pp_blocks == NULL) {
        /* Not initialized yet */
        cli_out("Blocks DB was not initialized\n");
        return res;
    }

    for (i = 0; i < block_num; i++) {
        cur_pp_block = &pp_blocks[i];
        for (k = 0; k < cur_pp_block->stage_num; k++) {
            cur_pp_stage = &cur_pp_block->stages[k];
            if (sal_strcasecmp(stage_n, cur_pp_stage->name))
                continue;
            qual_signal = cur_pp_stage->signals;
            for (j = 0; j < cur_pp_stage->number; j++, qual_signal++) {
                if (qual_signal->buffer != buffer)
                    continue;
                if (qual_signal->offset == offset) {
                    if (qual_signal->size != size)
                        sprintf(qual_name, "%s.Size-%db", qual_signal->name, size);
                    else
                        strcpy(qual_name, qual_signal->name);
                    return _SHR_E_NONE;
                } else if ((offset > qual_signal->offset)
                        && (offset < (qual_signal->offset + qual_signal->size))) {
                    sprintf(qual_name, "%s.Offset-%db.Size-%db",
                            qual_signal->name, qual_signal->offset - offset, size);
                    return _SHR_E_NONE;
                }
            }
        }
    }

    return res;
}

int dpp_qual_signal_print(match_t *match, int flags)
{
    int i, k, j;
    pp_block_t *cur_pp_block;
    pp_stage_t *cur_pp_stage;
    internal_signal_t *qual_signal;
    PRT_INIT_LOCAL

    if (pp_blocks == NULL) {
        /* Not initialized yet */
        cli_out("Blocks DB was not initialized\n");
        return _SHR_E_INIT;
    }

    PRT_ALLOC_LINE(LINE_EQUAL);

    /* Prepare header, pay attention to put header items and content in the same order */
    PRT_COPY("Stage");
    PRT_COPY("Signal");
    PRT_COPY("Offset");
    PRT_COPY("Size");
    if (flags & SIGNALS_PRINT_VERILOG) {
        PRT_COPY("Verilog");
    }

    PRT_SET_COLUMN_NUM;

    for (i = 0; i < block_num; i++) {
        cur_pp_block = &pp_blocks[i];

        for (k = 0; k < cur_pp_block->stage_num; k++) {
            cur_pp_stage = &cur_pp_block->stages[k];
            if (!ISEMPTY(match->to) && (sal_strcasestr(cur_pp_stage->name, match->to) == NULL))
                continue;

            qual_signal = cur_pp_stage->signals;
            for (j = 0; j < cur_pp_stage->number; j++, qual_signal++) {
                if (!ISEMPTY(match->name) && /* No match string on input  */
                    (((match->flags & SIGNALS_MATCH_EXACT) && sal_strcasecmp(qual_signal->name, match->name))
                    || (!(match->flags & SIGNALS_MATCH_EXACT)
                            && (sal_strcasestr(qual_signal->name, match->name) == NULL))))
                    continue;

                PRT_ALLOC_LINE(LINE_NONE);

                PRT_PRINT("%s.%s", cur_pp_block->name, cur_pp_stage->name);
                PRT_PRINT("%s", qual_signal->name);
                PRT_PRINT("%d", qual_signal->offset);
                PRT_PRINT("%d", qual_signal->size);
                if (flags & SIGNALS_PRINT_VERILOG) {
                    PRT_PRINT("%s", qual_signal->verilog);
                }
            }
        }
    }

    PRT_STAMP;
    return _SHR_E_NONE;
}

static int
dpp_qual_signals_init(char *dpp_device_descr, char *qual_signals_db, pp_stage_t *pp_stage)
{
    void *curTop, *cur;
    int res = _SHR_E_NONE;
    int cur_signal_num = 0;
    internal_signal_t *cur_internal_signal;

    if ((curTop = dpp_get_top(dpp_device_descr, qual_signals_db, "SignalInfo", 0)) == NULL) {
        res = _SHR_E_NOT_FOUND;
        goto exit;
    }

    RHDATA_GET_INT_STOP(curTop, "size0", pp_stage->buffer0_size);
    RHDATA_GET_INT_DEF(curTop, "size1", pp_stage->buffer1_size, 0);

    RHDATA_ITERATOR(cur, curTop, "signal")
    {
        cur_signal_num++;
    } /* Count all db objects */

    if (cur_signal_num == 0) {
        res = _SHR_E_NOT_FOUND;
        goto exit;
    }

    cur_internal_signal = pp_stage->signals = dataio_alloc(sizeof(internal_signal_t) * cur_signal_num);
    pp_stage->number = cur_signal_num;

    /* loop through entries */
    RHDATA_ITERATOR(cur, curTop, "signal") {
        xml_get_content_str(cur, "name",   cur_internal_signal->name, RHNAME_MAX_SIZE);
        xml_get_content_int(cur, "offset", &cur_internal_signal->offset);
        xml_get_content_int(cur, "size",   &cur_internal_signal->size);
        xml_get_content_int(cur, "buffer", &cur_internal_signal->buffer);
        xml_get_content_str(cur, "verilog",cur_internal_signal->verilog, RHVERILOG_MAX_SIZE);
        cur_internal_signal++;
    }

    xml_close_top(curTop);

    exit: return res;
}

static int
dpp_debug_signals_get(char *block_n, char *from_n, char *to_n, char *signal_n, debug_signal_t **signal_p)
{
    int signal_num = 0;
    int i, j;
    pp_block_t *cur_pp_block;
    debug_signal_t *debug_signals;

    if (pp_blocks == NULL) {
        /* Not initialized yet */
        cli_out("Signal DB was not initialized\n");
        goto exit;
    }

    for (i = 0; i < block_num; i++) {
        cur_pp_block = &pp_blocks[i];
        if (!ISEMPTY(block_n) && sal_strcasecmp(cur_pp_block->name, block_n))
            continue;

        debug_signals = cur_pp_block->debug_signals;

        for (j = 0; j < cur_pp_block->signal_num; j++) {
            if (from_n == to_n) {
                /* From == To means we want some stage either as from or to, so e one of conditions need to be true */
                if (!ISEMPTY(from_n)
                        && (sal_strcasestr(debug_signals[j].from, from_n) == NULL)
                        && (sal_strcasestr(debug_signals[j].to, to_n) == NULL))
                    continue;
            } else {
                /* Here one of both conditions must be true to compare the attributes */
                if (!ISEMPTY(from_n) && (sal_strcasestr(debug_signals[j].from, from_n) == NULL))
                    continue;
                if (!ISEMPTY(to_n) && (sal_strcasestr(debug_signals[j].to, to_n) == NULL))
                    continue;
            }

            if (!ISEMPTY(signal_n)
                    && (sal_strcasestr(debug_signals[j].attribute, signal_n) == NULL)
                    && (sal_strcasestr(debug_signals[j].verilog, signal_n) == NULL))
                continue;

            /* We found debug_signal matching criteria - fill pointer and leave */
            signal_num = 1;
            *signal_p = &debug_signals[j];
            break;
        }
    }

exit:
    return signal_num;
}

static int dpp_dsig_init(char *dpp_device_descr)
{
    void *curTop, *curSubTop, *curBlock, *curStage, *cur;
    void *curSignalTop, *curSignal;
    int res = _SHR_E_NONE;
    debug_signal_t *debug_signal;
    int i, j;

    char full_address[RHSIGADDRESS_MAX_SIZE];
    char temp[RHNAME_MAX_SIZE];
    int signal_num;

    pp_block_t *cur_pp_block;
    pp_stage_t *cur_pp_stage;
    int size;

    if (pp_blocks != NULL)
        /* ALready initialized */
        return res;

    if ((curTop = dpp_get_top(dpp_device_descr, "PP", "top", 0)) == NULL) {
        res = _SHR_E_NOT_FOUND;
        goto exit;
    }

    if ((curSubTop = xml_get_first(curTop, "block-list")) == NULL) {
        res = _SHR_E_NOT_FOUND;
        goto exit;
    }

    pp_blocks = sal_alloc(sizeof(pp_block_t) * MAX_PP_BLOCK_NUM, "blocks");
    /* loop through entries */
    RHDATA_ITERATOR(curBlock, curSubTop, "block") {
        /* Verify that we are inside the limitations MAX_NUM */
        if(block_num == MAX_PP_BLOCK_NUM)
            break;
        cur_pp_block = &pp_blocks[block_num++];
        sal_memset(cur_pp_block, 0, sizeof(pp_block_t));
        RHDATA_GET_STR_CONT(curBlock, "name", cur_pp_block->name);
        RHDATA_GET_STR_DEF(curBlock, "debug-signals", cur_pp_block->debug_signals_n);

        cur_pp_block->stages = sal_alloc(sizeof(pp_stage_t) * MAX_PP_STAGE_NUM, "stages");
        /* loop through entries */
        RHDATA_ITERATOR(curStage, curBlock, "stage") {
            if(cur_pp_block->stage_num == MAX_PP_STAGE_NUM)
                break;
            cur_pp_stage = &cur_pp_block->stages[cur_pp_block->stage_num++];
            sal_memset(cur_pp_stage, 0, sizeof(pp_stage_t));
            RHDATA_GET_STR_CONT(curStage, "name", cur_pp_stage->name);
            RHDATA_GET_STR_DEF(curStage, "programmable", cur_pp_stage->programmable);
            RHDATA_GET_STR_DEF(curStage, "internal-signals", temp);
            if (!ISEMPTY(temp)) {
                dpp_qual_signals_init(dpp_device_descr, temp, cur_pp_stage);
            }
        }

        if (ISEMPTY(cur_pp_block->debug_signals_n)) /* No debug signals for this block */
            continue;

        signal_num = 0;
        if ((curSignalTop = dpp_get_top(dpp_device_descr, cur_pp_block->debug_signals_n, "SignalInfo", 0)) == NULL) {
            res = _SHR_E_NOT_FOUND;
            goto exit;
        }

        RHDATA_ITERATOR(curSignal, curSignalTop, "Signal")
        {
            signal_num++;
        } /* Count all db objects */

        if (signal_num == 0) {
            res = _SHR_E_NOT_FOUND;
            goto exit;
        }

        debug_signal = dataio_alloc(sizeof(debug_signal_t) * signal_num);

        cur_pp_block->debug_signals = debug_signal;
        cur_pp_block->signal_num = signal_num;

        /* loop through entries */
        RHDATA_ITERATOR(curSignal, curSignalTop, "Signal") {
            xml_get_content_str(curSignal, "Name", debug_signal->verilog, RHVERILOG_MAX_SIZE);
            xml_get_content_int(curSignal, "Size", &debug_signal->size);
            xml_get_content_int(curSignal, "BlockID", &debug_signal->block_id);
            xml_get_content_str(curSignal, "Changeable", temp, RHNAME_MAX_SIZE);
            xml_get_content_str(curSignal, "Attribute", debug_signal->attribute, RHNAME_MAX_SIZE);
            cur = xml_get_content_str(curSignal, "Condition", debug_signal->cond_attribute, RHNAME_MAX_SIZE);
            if ((cur != NULL) && !ISEMPTY(debug_signal->cond_attribute)) {
                RHDATA_GET_INT_DEF(cur, "Value", debug_signal->cond_value, 1);
            }

            debug_signal->changeable = (sal_strcasecmp(temp, "Yes") ? 0 : 1);

            xml_get_content_str(curSignal, "Expansion", debug_signal->expansion, RHNAME_MAX_SIZE);
            /* No explicit expansion - look for implicit one */
            if (!ISEMPTY(debug_signal->expansion)) {
                if ((dpp_sigstruct_get(debug_signal->expansion)) == NULL) {
                    cli_out("No signal expansion:%s\n", debug_signal->expansion);
                    SET_EMPTY(debug_signal->expansion);
                }
            } else {
                if ((dpp_sigstruct_get(debug_signal->attribute)) != NULL)
                    /* If there is match - attribute serves as expansion */
                    strcpy(debug_signal->expansion, debug_signal->attribute);
            }

            xml_get_content_str(curSignal, "Resolution", debug_signal->resolution, RHNAME_MAX_SIZE);
            if (!ISEMPTY(debug_signal->resolution)) {
                if ((dpp_sigresolve_get(debug_signal->resolution)) == NULL) {
                    cli_out("Signal resolution:%s does not exist\n", debug_signal->resolution);
                    SET_EMPTY(debug_signal->resolution);
                }
            } else {
                if ((dpp_sigresolve_get(debug_signal->attribute)) != NULL)
                    /* If there is match - attribute serves as resolution param name */
                    strcpy(debug_signal->resolution, debug_signal->attribute);
            }

            cur = xml_get_content_str(curSignal, "Double", temp, RHNAME_MAX_SIZE);
            if ((cur != NULL) && !ISEMPTY(temp))
                debug_signal->double_flag = 1;

            xml_get_content_str(curSignal, "From", debug_signal->from, RHNAME_MAX_SIZE);
            xml_get_content_str(curSignal, "To", debug_signal->to, RHNAME_MAX_SIZE);
            strcpy(debug_signal->block_n, cur_pp_block->name);
            xml_get_content_str(curSignal, "Address", full_address, RHSIGADDRESS_MAX_SIZE);
            debug_signal->range_num = dpp_parse_address(full_address, debug_signal->address);
            /* Verify consistency between size and sum of all bits in range */
            size = 0;
            for (i = 0; i < debug_signal->range_num; i++)
                size += (debug_signal->address[i].msb + 1 - debug_signal->address[i].lsb);

            if (size != debug_signal->size) {
                cli_out("Correcting size for:%s from:%d to %d\n",
                        debug_signal->attribute, debug_signal->size, size);
                debug_signal->size = size;
            }

            debug_signal++;
        }

        xml_close_top(curSignalTop);
    }

    for (i = 0; i < block_num; i++) {
        cur_pp_block = &pp_blocks[i];
        debug_signal = cur_pp_block->debug_signals;
        for (j = 0; j < cur_pp_block->signal_num; j++) {
            if (!ISEMPTY(debug_signal->cond_attribute)) {
                signal_num = dpp_debug_signals_get(debug_signal->block_n,
                        debug_signal->from, debug_signal->to,
                        debug_signal->cond_attribute, &debug_signal->cond_signal);
                if (signal_num == 0) {
                    cli_out("Condition Attribute:%s does not exist for:%s:%s -> %s\n",
                            debug_signal->cond_attribute, debug_signal->block_n,
                            debug_signal->from, debug_signal->to);
                }
            }
            debug_signal++;
        }
    }
    /* Now we need to extract condition attribute */

    xml_close_top(curTop);
exit:
    return res;
}

int
dpp_dsig_get(int unit, int core,  match_t *match, signal_output_t *signal_output)
{
    int res = _SHR_E_NONE;

    debug_signal_t *debug_signal;

    if((match == NULL) || (signal_output == NULL)) {
        res = _SHR_E_PARAM;
        goto exit;
    }

    sal_memset(signal_output, 0, sizeof(signal_output_t));
    if((dpp_debug_signals_get(match->block, match->from, match->to, match->name, &debug_signal)) == 0) {
        res = _SHR_E_NOT_FOUND;
        goto exit;
    }

    signal_output->debug_signal = debug_signal;
    signal_output->size = debug_signal->size;

    dpp_dsig_get_value(unit, core, debug_signal, signal_output->value);
    dpp_buffer_to_str(debug_signal->resolution, signal_output->value, signal_output->print_value,
                      debug_signal->size, match->output_order);

exit:
    return res;
}

void
dpp_debug_signals_check(void)
{
    int i, j;
    pp_block_t *cur_pp_block;
    debug_signal_t *debug_signals;

    if (pp_blocks == NULL) {
        /* Not initialized yet */
        cli_out("Signal DB was not initialized\n");
        return;
    }

    for (i = 0; i < block_num; i++) {
        cur_pp_block = &pp_blocks[i];
        debug_signals = cur_pp_block->debug_signals;

        for (j = 0; j < cur_pp_block->signal_num; j++) {
            if (!ISEMPTY(debug_signals[j].from) && !dpp_stage_exists(debug_signals[j].from))
                cli_out("Stage:%s on signal:%s block:%s does not exist\n",
                        debug_signals[j].from, debug_signals[j].attribute, cur_pp_block->name);
            if (!ISEMPTY(debug_signals[j].to) && !dpp_stage_exists(debug_signals[j].to))
                cli_out("Stage:%s on signal:%s block:%s does not exist\n",
                        debug_signals[j].to, debug_signals[j].attribute, cur_pp_block->name);
        }
    }
    return;
}

/* Copy field_size bits from "signal_value" with offset "field_offset" to field_value with size "field_size" */
static void
dpp_sigfield_get_value(uint32 *signal_value, uint32 *field_value, int field_offset, int field_size)
{
    SHR_BITCOPY_RANGE(field_value, 0, signal_value, field_offset, field_size);
}

static int
dpp_sigfield_get(char *sigstruct_n, uint32 *signal_value, char *field_name, char *field_str, uint32 *field_value)
{
    sigstruct_t *sigstruct;
    sigstruct_field_t *sigstruct_field;
    int i;
    int res = _SHR_E_NOT_FOUND;
    uint32 cur_field_value[MAX_DEBUG_SIGNAL_INT_SIZE];

    /* Split by dot into struct and lower field, but only once, so maximum tokens = 2 */
    char **tokens;
    uint32 realtokens = 0;

    if((tokens = dataio_split_mstr(field_name, ".", 2, &realtokens)) == NULL) {
        return res;
    }

    if (!ISEMPTY(sigstruct_n) && (sigstruct = dpp_sigstruct_get(sigstruct_n)) != NULL) {
        sigstruct_field = sigstruct->fields;
        for (i = 0; i < sigstruct->field_num; i++) {
            /* We are looking for first token resolution */
            if (!sal_strcasecmp(tokens[0], sigstruct_field->name)) {
                sal_memset(cur_field_value, 0, MAX_DEBUG_SIGNAL_SIZE);
                /* Obtain the value */
                dpp_sigfield_get_value(signal_value, cur_field_value, sigstruct_field->start_bit, sigstruct_field->size);
                /* If this is the last token - obtain string value */
                if (realtokens == 1) {
                    sal_memcpy(field_value, cur_field_value, MAX_DEBUG_SIGNAL_SIZE);
                    if (field_str != NULL) {
                        res = dpp_sigparam_get(sigstruct_field->name, *cur_field_value, field_str);
                        if (res != _SHR_E_NONE)
                            sprintf(field_str, "%u", *cur_field_value);
                    }
                    res = _SHR_E_NONE;
                } else {
                    res = dpp_sigfield_get(sigstruct_field->expansion_m.name, cur_field_value, tokens[1],
                                            field_str, field_value);
                }
                break;
            }
            sigstruct_field++;
        }
    }

    dataio_free_tokens(tokens, realtokens);

    return res;
}

static sigstruct_t *dpp_expansion_get(char *sigstruct_n, uint32 *value, expansion_t *expansion)
{
    int i, n;
    expansion_option_t *option;
    char field_str[RHNAME_MAX_SIZE];
    uint32 field_value[MAX_DEBUG_SIGNAL_INT_SIZE];
    int match_flag;
    sigstruct_t *sigstruct = NULL;
    uint32 rt_value, db_value;

    if (ISEMPTY(expansion->name)) /* No explicit expansion */{
        goto exit;
    } else if (sal_strcasecmp(expansion->name, "dynamic")) { /* static expansion */
        sigstruct = dpp_sigstruct_get(expansion->name);
        goto exit;
    }

    /* Dynamic expansion */
    option = expansion->options;
    for (i = 0; i < expansion->option_num; i++) {
        match_flag = 1;
        n = 0;
        while ((n < OPTION_PARAM_MAX_NUM) && !ISEMPTY(option->param[n].name)) {
            if (dpp_sigfield_get(sigstruct_n, value, option->param[n].name, field_str, field_value) != _SHR_E_NONE) {
                match_flag = 0;
                break;
            }
            if (sal_strcasecmp(option->param[n].value, field_str)) {
                if ((dataio_stoul(field_str, &rt_value) != _SHR_E_NONE)
                        || (dataio_stoul(option->param[n].value, &db_value)
                                != _SHR_E_NONE) || (rt_value != db_value)) {
                    match_flag = 0;
                    break;
                }
            }

            n++;
        }
        if (match_flag == 1) { /* Found our expansion leave option loop*/
            sigstruct = dpp_sigstruct_get(option->name);
            break;
        }
        option++;
    }
    exit: return sigstruct;
}

static int
dpp_dsig_expand(uint32 *signal_value, char *sigstruct_n, int byte_order,
                char *match_n, int flags, rhlist_t **field_list_p)
{
    int valid = 0; /* By default no match - we yet need to find one */
    sigstruct_t *sigstruct, *sigstruct_exp, *sigstruct_exp4field;
    sigstruct_field_t *sigstruct_field;
    int i;
    uint32 field_value[MAX_DEBUG_SIGNAL_INT_SIZE];
    char *field_dyn_name;
    signal_output_t *field_output;
    int top_valid, child_valid;
    char *cur_match_n;
    rhhandle_t temp = NULL;

    rhlist_t *field_list = NULL, *child_list = NULL;

    /* Check if there is an expansion at all */
    if (ISEMPTY(sigstruct_n) || (sigstruct = dpp_sigstruct_get(sigstruct_n)) == NULL)
        goto exit;

    /* In case of dynamic expansion find the matching expansion and switch to it */
    if ((sigstruct_exp = dpp_expansion_get(sigstruct_n, signal_value, &sigstruct->expansion_m)) != NULL)
        sigstruct = sigstruct_exp;

    for (i = 0, sigstruct_field = sigstruct->fields; i < sigstruct->field_num; i++, sigstruct_field++) {
        child_valid = 0;
        child_list = NULL;
        /* If there is a condition check that it is fulfilled */
        if (!(flags & SIGNALS_MATCH_NOCOND) && !ISEMPTY(sigstruct_field->cond_attribute)) {
            dpp_sigfield_get(sigstruct->name, signal_value, sigstruct_field->cond_attribute, NULL, field_value);
            if (0 == VALUE(field_value))
                continue;
        }

        /* Check for further usage dynamic expansion for field */
        /* In case of dynamic expansion find the matching expansion and switch to it - in this case for field */
        sigstruct_exp4field = dpp_expansion_get(sigstruct->name, signal_value, &sigstruct_field->expansion_m);
        if (sigstruct_exp4field)
            field_dyn_name = sigstruct_exp4field->name;
        else
            field_dyn_name = sigstruct_field->expansion_m.name;

        sal_memset(field_value, 0, MAX_DEBUG_SIGNAL_SIZE);
        dpp_sigfield_get_value(signal_value, field_value, sigstruct_field->start_bit, sigstruct_field->size);

        if ((match_n == NULL) /* No match string on input  */
                || ((flags & SIGNALS_MATCH_EXACT)  && !sal_strcasecmp(sigstruct_field->name, match_n))
                || (!(flags & SIGNALS_MATCH_EXACT) && (sal_strcasestr(sigstruct_field->name, match_n) != NULL)))
            top_valid = 1;
        else
            top_valid = 0;

        cur_match_n = top_valid ? NULL : match_n;

        if (sigstruct_exp4field) {
            child_valid = dpp_dsig_expand(field_value, sigstruct_exp4field->name, byte_order,
                                          cur_match_n, flags, &child_list);
        }

        if ((top_valid != 0) || (child_valid != 0)) {
            valid = 1; /* Once we have match return will be non zero */

            /* Allocate structure for the child */
            if(field_list == NULL) {
                if((field_list = rhlist_create("field_list", sizeof(signal_output_t), 0)) == NULL)
                    goto exit;
            }

            /* Now we can allocate output and fill it */
            if(rhlist_add_tail(field_list, NULL, RHID_TO_BE_GENERATED, &temp) == -SOC_SAND_ERR) {
                /* No more place any more - return with what you have until now */
                goto exit;
            }
            field_output = temp;

            if (!ISEMPTY(field_dyn_name) && sal_strcasecmp(sigstruct_field->name, field_dyn_name))
                sprintf(RHNAME(field_output), "%s(%s)", sigstruct_field->name, field_dyn_name);
            else
                strcpy(RHNAME(field_output), sigstruct_field->name);

            dpp_buffer_to_str(sigstruct_field->resolution, field_value, field_output->print_value,
                              sigstruct_field->size, byte_order);
            field_output->field_list = child_list;
            field_output->size       = sigstruct_field->size;
        }
    }

exit:
    *field_list_p = field_list;
    return valid;
}

int
dpp_dsig_get_list(int unit, int core, match_t *match_p, rhlist_t *dsig_list)
{
    int i, j;
    char *match_n;
    int res = _SHR_E_NONE;

    pp_block_t *cur_pp_block;
    debug_signal_t *debug_signals;
    signal_output_t *signal_output = NULL;
    rhhandle_t temp = NULL;

    int top_valid, child_valid;
    /* Dynamic Data */
    uint32 value[MAX_DEBUG_SIGNAL_INT_SIZE];
    char *cur_match_n;
    rhlist_t *field_list = NULL;

    if (pp_blocks == NULL) {
        /* Not initialized yet */
        cli_out("Signal DB was not initialized\n");
        res = _SHR_E_INIT;
        goto exit;
    }

    for (i = 0; i < block_num; i++) {
        cur_pp_block = &pp_blocks[i];

        if (!ISEMPTY(match_p->block) && sal_strcasecmp(cur_pp_block->name, match_p->block))
            continue;

        debug_signals = cur_pp_block->debug_signals;
        for (j = 0; j < cur_pp_block->signal_num; j++) {
            if (match_p->from == match_p->to) {
                /* From == To means we want some stage either as from or to, so e one of conditions need to be true */
                if (!ISEMPTY(match_p->from)
                        && (sal_strcasestr(debug_signals[j].from, match_p->from) == NULL)
                        && (sal_strcasestr(debug_signals[j].to, match_p->to) == NULL))
                    continue;
            } else {
                /* Here one of both conditions must be true to compare the attributes */
                if (!ISEMPTY(match_p->from)
                        && (sal_strcasestr(debug_signals[j].from, match_p->from) == NULL))
                    continue;
                if (!ISEMPTY(match_p->to)
                        && (sal_strcasestr(debug_signals[j].to, match_p->to) == NULL))
                    continue;
            }
            match_n = match_p->name;

            if ((match_n == NULL)                                           /* No match string on input  */
                || ((match_p->flags & SIGNALS_MATCH_EXACT) &&               /* Match should be exact one */
                    (!sal_strcasecmp(debug_signals[j].attribute, match_n) ||
                     !sal_strcasecmp(debug_signals[j].verilog, match_n)))
                || (!(match_p->flags & SIGNALS_MATCH_EXACT) &&              /* Match should be any part of name */
                     ((sal_strcasestr(debug_signals[j].attribute, match_n) != NULL) ||
                      (sal_strcasestr(debug_signals[j].verilog, match_n) != NULL))))
                top_valid = 1;
            else
                top_valid = 0;

            /* If double flag not set avoid this */
            if (!(match_p->flags & SIGNALS_MATCH_DOUBLE) && (debug_signals[j].double_flag == 1))
                continue;

            /* If there is a condition check that it is fulfilled */
            if (!(match_p->flags & SIGNALS_MATCH_NOCOND) && debug_signals[j].cond_signal) {
                dpp_dsig_get_value(unit, core, debug_signals[j].cond_signal, value);
                if (debug_signals[j].cond_value != VALUE(value))
                    continue;
            }

            cur_match_n = top_valid ? NULL : match_n;

            /* Step 1 - Fill signal value */
            dpp_dsig_get_value(unit, core, &debug_signals[j], value);

            /* Step 2 - Now start expansion */
            child_valid = dpp_dsig_expand(value, debug_signals[j].expansion, match_p->output_order,
                                          cur_match_n, match_p->flags, &field_list);

            if ((top_valid == 0) && (child_valid == 0)) /* No match in any place */
                continue;

            /* Now we can allocate output and fill it */
            if(rhlist_add_tail(dsig_list, NULL, RHID_TO_BE_GENERATED, &temp) == -SOC_SAND_ERR) {
                /* No more place any more - return with what you have until now */
                res = _SHR_E_MEMORY;
                /* free already allocated child list, that we have no way to preserve */
                dpp_dsig_free(field_list);
                goto exit;
            }
            signal_output = temp;

            /* Copy debug signal into output_signal */
            signal_output->debug_signal = &debug_signals[j];
            /* Create print value from raw one */
            dpp_buffer_to_str(debug_signals[j].resolution, value, signal_output->print_value,
                              debug_signals[j].size, match_p->output_order);
            memcpy(signal_output->value, value, MAX_DEBUG_SIGNAL_SIZE);
            /* Fill pointer to structure fields */
            signal_output->size = debug_signals[j].size;
            signal_output->field_list = field_list;
        }
    }    /* Dynamic Data */

exit:
    return res;
}

static int
dpp_dsig_expand_print(rhlist_t *field_list, int depth, rhlist_t *prt_list)
{
    char tmp[MAX_DEBUG_SIGNAL_SIZE];
    char *prt_line_offset = NULL;
    void *prt_line_start = NULL;
    signal_output_t *field_output;

    RHITERATOR(field_output, field_list) {
        PRT_ALLOC_LINE(LINE_NONE);
        PRT_SKIP(3);

        dataio_fillstr(tmp, depth * 3, ' ');
        PRT_PRINT("%s%s", tmp, RHNAME(field_output));
        PRT_PRINT("%d", field_output->size);
        PRT_PRINT("%s", field_output->print_value);

        dpp_dsig_expand_print(field_output->field_list, depth + 1, prt_list);
    }

    return _SHR_E_NONE;
}

int
dpp_dsig_print(rhlist_t *dsig_list, int flags)
{
    int j;
    int skip_num;
    char *cur_print_value;
    int cur_size;
    debug_signal_t *debug_signal;
    signal_output_t *signal_output;
    PRT_INIT_LOCAL

    if (dsig_list == NULL) {
        cli_out("No signals found\n");
        return _SHR_E_INIT;
    }

    PRT_ALLOC_LINE(LINE_EQUAL);

    /* Prepare header, pay attention to put header items and content in the same order */
    PRT_COPY("Block");
    PRT_COPY("From");
    PRT_COPY("To");
    PRT_COPY("Attribute");
    PRT_COPY("Size");
    if (flags & SIGNALS_PRINT_VALUE) {
        PRT_COPY("Value");
    }

    skip_num = PRT_GET_CUR_COLUMN;

    if (flags & SIGNALS_PRINT_VERILOG) {
        PRT_COPY("Verilog");
        PRT_COPY("High");
        PRT_COPY("Low");
        PRT_COPY("MSB");
        PRT_COPY("LSB");
    }

    PRT_SET_COLUMN_NUM;
    RHITERATOR(signal_output, dsig_list) {
        PRT_ALLOC_LINE(LINE_NONE);
        debug_signal = signal_output->debug_signal;

        PRT_PRINT("%s", debug_signal->block_n);
        PRT_PRINT("%s", debug_signal->from);
        PRT_PRINT("%s", debug_signal->to);
        if (!ISEMPTY(debug_signal->expansion)
                && sal_strcasecmp(debug_signal->attribute, debug_signal->expansion)) {
            PRT_PRINT("%s(%s)", debug_signal->attribute, debug_signal->expansion);
        } else {
            PRT_PRINT("%s", debug_signal->attribute);
        }
        PRT_PRINT("%d", debug_signal->size);

        if (flags & SIGNALS_PRINT_VALUE) {
            PRT_PRINT("%s", signal_output->print_value);
            if (flags & SIGNALS_PRINT_FULL) {
                cur_print_value = signal_output->print_value;
                cur_size = debug_signal->size;
                while (cur_size > RHCHUNK_MAX_SIZE) {
                    PRT_ALLOC_LINE(LINE_NONE);

                    PRT_SKIP(skip_num - 1);
                    cur_size -= RHCHUNK_MAX_SIZE;
                    cur_print_value += RHNAME_MAX_SIZE;
                    PRT_PRINT("%s", cur_print_value);
                }
            }
        }

        if (flags & SIGNALS_PRINT_VERILOG) {
            PRT_PRINT("%s", debug_signal->verilog);
            for (j = 0; j < debug_signal->range_num; j++) {
                if (j != 0) {
                    PRT_ALLOC_LINE(LINE_NONE);
                    PRT_SKIP(skip_num + 1);
                }
                PRT_PRINT("%d", debug_signal->address[j].high);
                PRT_PRINT("%d", debug_signal->address[j].low);
                PRT_PRINT("%d", debug_signal->address[j].msb);
                PRT_PRINT("%d", debug_signal->address[j].lsb);
            }
        }

        if(flags & SIGNALS_PRINT_EXPAND) {
            if(dpp_dsig_expand_print(signal_output->field_list, 1, prt_list) != _SHR_E_NONE) {
                return _SHR_E_MEMORY;
            }
        }
    }

    PRT_STAMP;
    return _SHR_E_NONE;
}

void
dpp_dsig_free(rhlist_t *output_list)
{
    signal_output_t *signal_output;
    RHITERATOR(signal_output, output_list) {
        if(signal_output->field_list) {
            dpp_dsig_free(signal_output->field_list);
        }
    }
    rhlist_free_all(output_list);
}

static void
dpp_expansion_init(void *cur, expansion_t *expansion)
{
    void *curOption;
    expansion_option_t *option;

    RHDATA_GET_STR_DEF(cur, "expansion", expansion->name);

    if (!sal_strcasecmp(expansion->name, "Dynamic")) {
        RHDATA_GET_NODE_NUM(expansion->option_num, cur, "option");
        option = expansion->options = dataio_alloc(expansion->option_num * sizeof(expansion_option_t));
        RHDATA_ITERATOR(curOption, cur, "option") {
            RHDATA_GET_STR_CONT(curOption, "expansion", option->name);
            xml_get_attributes(curOption, option->param, "expansion", OPTION_PARAM_MAX_NUM);
            option++;
        }
    }
    return;
}

static void
dpp_sigstruct_init_expansion(void)
{
    int i, j;
    sigstruct_t *cur_sigstruct;
    sigstruct_field_t *cur_sigstruct_field;

    if (sigstructs == NULL) {
        /* Not initialized yet */
        cli_out("Signal Struct DB was not initialized\n");
        return;
    }

    /* Go through field and fill expansion were available */
    for (i = 0; i < sigstruct_num; i++) {
        cur_sigstruct = &sigstructs[i];
        for (j = 0; j < cur_sigstruct->field_num; j++) {
            cur_sigstruct_field = &(cur_sigstruct->fields[j]);
            if (!ISEMPTY(cur_sigstruct_field->expansion_m.name)) {
                /* If expansion is not empty and not dynamic, find the expansion, if failed zero the field */
                if (sal_strcasecmp(cur_sigstruct_field->expansion_m.name, "Dynamic"))
                    if (dpp_sigstruct_get(cur_sigstruct_field->expansion_m.name) == NULL) {
                        cli_out("No signal expansion:%s\n", cur_sigstruct_field->expansion_m.name);
                        SET_EMPTY(cur_sigstruct_field->expansion_m.name);
                    }
            } else {
                /* When expansion is empty first check may be it can be expanded by name */
                if (dpp_sigstruct_get(cur_sigstruct_field->name) != NULL)
                    strcpy(cur_sigstruct_field->expansion_m.name, cur_sigstruct_field->name);
                /* Not found check resolution */
                else if (!ISEMPTY(cur_sigstruct_field->resolution)) {
                    /* If resolution is not empty look if it exists */
                    if (dpp_sigresolve_get(cur_sigstruct_field->resolution) == NULL) {
                        cli_out("No signal resolution:%s\n", cur_sigstruct_field->resolution);
                        SET_EMPTY(cur_sigstruct_field->resolution);
                    }
                } else {
                    /* When resolution is empty check may be it can be expanded by name */
                    if (dpp_sigresolve_get(cur_sigstruct_field->name) != NULL)
                        /* If there is match - attribute serves as resolution */
                        strcpy(cur_sigstruct_field->resolution, cur_sigstruct_field->name);
                }
            }
        }
    }

    return;
}

static int
dpp_sigstruct_init(char *dpp_device_descr)
{
    void *curTop, *curSubTop, *cur, *curSub;
    int res = _SHR_E_NONE;
    sigstruct_t *cur_sigstruct;
    sigstruct_field_t *cur_sigstruct_field;
    sigparam_t *cur_sigparam;
    sigparam_value_t *cur_sigparam_value;

    char temp[RHNAME_MAX_SIZE];

    if (sigstructs != NULL)
        /* ALready initialized */
        return res;

    if ((curTop = dpp_get_top(dpp_device_descr, "PP", "top", 0)) == NULL) {
        res = _SHR_E_NOT_FOUND;
        goto exit;
    }

    RHDATA_GET_STR_STOP(curTop, "expansion", temp);
    xml_close_top(curTop);

    if ((curTop = dpp_get_top(dpp_device_descr, temp, "top", 0)) == NULL) {
        res = _SHR_E_NOT_FOUND;
        goto exit;
    }

    /* Read all available parameters with their possible values */
    if ((curSubTop = xml_get_first(curTop, "signal-params")) == NULL) {
        res = _SHR_E_NOT_FOUND;
        goto exit;
    }

    RHDATA_GET_NODE_NUM(sigparam_num, curSubTop, "signal");
    cur_sigparam = sigparams = dataio_alloc(sigparam_num * sizeof(sigparam_t));

    RHDATA_ITERATOR(cur, curSubTop, "signal") {
        RHDATA_GET_STR_CONT(cur, "name", cur_sigparam->name);
        RHDATA_GET_INT_CONT(cur, "size", cur_sigparam->size);
        RHDATA_GET_STR_DEF(cur, "default", cur_sigparam->default_str);

        RHDATA_GET_NODE_NUM(cur_sigparam->value_num, cur, "entry");
        cur_sigparam_value = cur_sigparam->values =
            dataio_alloc(cur_sigparam->value_num * sizeof(sigparam_value_t));

        RHDATA_ITERATOR(curSub, cur, "entry") {
            RHDATA_GET_STR_CONT(curSub, "name", cur_sigparam_value->name);
            RHDATA_GET_INT_CONT(curSub, "value", cur_sigparam_value->value);
            cur_sigparam_value++;
        }

        cur_sigparam++;
    }

    if ((curSubTop = xml_get_first(curTop, "signal-structures")) == NULL) {
        res = _SHR_E_NOT_FOUND;
        goto exit;
    }

    RHDATA_GET_NODE_NUM(sigstruct_num, curSubTop, "structure");
    cur_sigstruct = sigstructs = dataio_alloc(sigstruct_num * sizeof(sigstruct_t));

    /* loop through entries */
    RHDATA_ITERATOR(cur, curSubTop, "structure") {
        RHDATA_GET_STR_CONT(cur, "name", cur_sigstruct->name);
        RHDATA_GET_INT_CONT(cur, "size", cur_sigstruct->size);
        dpp_expansion_init(cur, &cur_sigstruct->expansion_m);

        RHDATA_GET_NODE_NUM(cur_sigstruct->field_num, cur, "field");
        cur_sigstruct_field = cur_sigstruct->fields
            = dataio_alloc(cur_sigstruct->field_num * sizeof(sigstruct_field_t));

        /* loop through entries */
        RHDATA_ITERATOR(curSub, cur, "field") {
            RHDATA_GET_STR_CONT(curSub, "name", cur_sigstruct_field->name);
            RHDATA_GET_STR_CONT(curSub, "bits", temp);
            RHDATA_GET_STR_DEF(curSub, "condition", cur_sigstruct_field->cond_attribute);

            if (dpp_parse_bit_range(temp, &cur_sigstruct_field->start_bit,
                    &cur_sigstruct_field->end_bit) != _SHR_E_NONE) {
                cli_out("Field:%s.%s, has bad bites range:%s\n",
                        cur_sigstruct->name, cur_sigstruct_field->name, temp);
                continue;
            }

            cur_sigstruct_field->size = cur_sigstruct_field->end_bit + 1 - cur_sigstruct_field->start_bit;

            RHDATA_GET_STR_DEF(curSub, "resolution", cur_sigstruct_field->resolution);

            dpp_expansion_init(curSub, &cur_sigstruct_field->expansion_m);

            cur_sigstruct_field++;
        }
        cur_sigstruct++;
    }

    dpp_sigstruct_init_expansion();

    xml_close_top(curTop);
    exit: return res;
}

int dpp_sigstruct_print(char *match_n)
{
    int i, k;
    sigstruct_t *cur_sigstruct;
    sigstruct_field_t *cur_sigstruct_field;
    PRT_INIT_LOCAL

    if (sigstructs == NULL) {
        /* Not initialized yet */
        cli_out("Signal Struct DB was not initialized\n");
        return _SHR_E_INIT;
    }

    PRT_ALLOC_LINE(LINE_EQUAL);

    /* Prepare header, pay attention to put header items and content in the same order */
    PRT_COPY("Signal");
    PRT_COPY("Size");
    PRT_COPY("Field");
    PRT_COPY("Start");
    PRT_COPY("Size");
    PRT_COPY("Condition");

    PRT_SET_COLUMN_NUM;
    for (i = 0; i < sigstruct_num; i++) {
        cur_sigstruct = &sigstructs[i];
        if (!ISEMPTY(match_n) && (sal_strcasestr(cur_sigstruct->name, match_n) == NULL))
            continue;

        PRT_ALLOC_LINE(LINE_NONE);

        PRT_PRINT("%s", cur_sigstruct->name);
        PRT_PRINT("%d", cur_sigstruct->size);

        for (k = 0; k < cur_sigstruct->field_num; k++) {
            cur_sigstruct_field = &cur_sigstruct->fields[k];
            if (k != 0) {
                PRT_ALLOC_LINE(LINE_NONE);
                PRT_SKIP(2);
            }

            PRT_PRINT("%s", cur_sigstruct_field->name);
            PRT_PRINT("%d", cur_sigstruct_field->start_bit);
            PRT_PRINT("%d", cur_sigstruct_field->size);
            PRT_PRINT("%s", cur_sigstruct_field->cond_attribute);
        }
    }

    PRT_STAMP;
    return _SHR_E_NONE;
}

int dpp_sigparam_get(char *signal_n, uint32 value, char *value_n)
{
    int res = _SHR_E_NOT_FOUND;
    int i, k;
    sigparam_t *cur_sigparam;
    sigparam_value_t *cur_sigparam_value;

    if (sigstructs == NULL) {
        /* Not initialized yet */
        cli_out("Signal Struct DB was not initialized\n");
        return res;
    }

    for (i = 0; i < sigparam_num; i++) {
        cur_sigparam = &sigparams[i];
        if (sal_strcasecmp(cur_sigparam->name, signal_n))
            continue;

        /* Verify that value is suitable for signal resolution size */
        if (value >= (1 << cur_sigparam->size)) {
            cli_out("Value:%d excess signal:%s size:%d\n", value, signal_n,
                    cur_sigparam->size);
            break;
        }

        for (k = 0; k < cur_sigparam->value_num; k++) {
            cur_sigparam_value = &cur_sigparam->values[k];
            if (value == cur_sigparam_value->value) {
                sprintf(value_n, "%s(%d)", cur_sigparam_value->name, value);
                res = _SHR_E_NONE;
                break;
            }
        }

        if ((res != _SHR_E_NONE) && !ISEMPTY(cur_sigparam->default_str))/* Copy default param name if not found */{
            sprintf(value_n, "%s(%d)", cur_sigparam->default_str, value);
            res = _SHR_E_NONE;
        }

        break;
    }

    return res;
}

int dpp_sigparam_print(char *match_n)
{
    int i, k;
    sigparam_t *cur_sigparam;
    sigparam_value_t *cur_sigparam_value;
    PRT_INIT_LOCAL

    if (sigstructs == NULL) {
        /* Not initialized yet */
        cli_out("Signal Struct DB was not initialized\n");
        return _SHR_E_INIT;
    }

    PRT_ALLOC_LINE(LINE_EQUAL);

    /* Prepare header, pay attention to put header items and content in the same order */
    PRT_COPY("Signal");
    PRT_COPY("Size");
    PRT_COPY("Value Name");
    PRT_COPY("Value");

    PRT_SET_COLUMN_NUM;

    for (i = 0; i < sigparam_num; i++) {
        cur_sigparam = &sigparams[i];
        if (!ISEMPTY(match_n) && (sal_strcasestr(cur_sigparam->name, match_n) == NULL))
            continue;

        PRT_ALLOC_LINE(LINE_NONE);

        PRT_PRINT("%s", cur_sigparam->name);
        PRT_PRINT("%d", cur_sigparam->size);

        for (k = 0; k < cur_sigparam->value_num; k++) {
            cur_sigparam_value = &cur_sigparam->values[k];
            if (k != 0) {
                PRT_ALLOC_LINE(LINE_NONE);
                PRT_SKIP(2);
            }

            PRT_PRINT("%s", cur_sigparam_value->name);
            PRT_PRINT("%d", cur_sigparam_value->value);
        }
    }

    PRT_STAMP;
    return _SHR_E_NONE;
}

int
dpp_stage_size(char *stage_n, int buffer)
{
    int i, k;
    pp_block_t *cur_pp_block;
    pp_stage_t *cur_pp_stage;

    for (i = 0; i < block_num; i++) {
        cur_pp_block = &pp_blocks[i];

        for (k = 0; k < cur_pp_block->stage_num; k++) {
            cur_pp_stage = &cur_pp_block->stages[k];
            if (!sal_strcasecmp(stage_n, cur_pp_stage->name)) {
                if (buffer == QUAL_BUFFER_LSB)
                    return cur_pp_stage->buffer0_size;
                else if (buffer == QUAL_BUFFER_MSB)
                    return cur_pp_stage->buffer1_size;
            }
        }
    }

    cli_out("Failed to find stage:%s buffer:%d\n", stage_n, buffer);
    return -1;
}

int dpp_stage_print(void)
{
    int i, k;
    pp_block_t *cur_pp_block;
    pp_stage_t *cur_pp_stage;
    PRT_INIT_LOCAL

    if (pp_blocks == NULL) {
        /* Not initialized yet */
        cli_out("Blocks DB was not initialized\n");
        return _SHR_E_INIT;
    }

    PRT_ALLOC_LINE(LINE_EQUAL);

    /* Prepare header, pay attention to put header items and content in the same order */
    PRT_COPY("Block");
    PRT_COPY("Stage");
    PRT_COPY("Programmable");

    PRT_SET_COLUMN_NUM;

    for (i = 0; i < block_num; i++) {
        cur_pp_block = &pp_blocks[i];

        PRT_ALLOC_LINE(LINE_NONE);
        PRT_PRINT("%s", cur_pp_block->name);

        for (k = 0; k < cur_pp_block->stage_num; k++) {
            cur_pp_stage = &cur_pp_block->stages[k];
            if (k != 0) {
                PRT_ALLOC_LINE(LINE_NONE);
                PRT_SKIP(1);
            }

            PRT_PRINT("%s", cur_pp_stage->name);
            PRT_PRINT("%s", cur_pp_stage->programmable);
        }
    }

    PRT_STAMP;
    return _SHR_E_NONE;
}

int dpp_signals_init(char *global_descr)
{
    int res = _SHR_E_NONE;

    if (dpp_sigstruct_init(global_descr) != _SHR_E_NONE) {
        res = _SHR_E_NOT_FOUND;
        goto exit;
    }

    if (dpp_dsig_init(global_descr) != _SHR_E_NONE) {
        res = _SHR_E_NOT_FOUND;
        goto exit;
    }

    exit: return res;
}
