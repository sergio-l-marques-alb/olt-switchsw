/*
 * $Copyright: (c) 2016 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * File:    access_pack.c
 * Purpose: Miscellaneous routine for device db access
 */

#include <sal/appl/io.h>
#include <sal/core/libc.h>
#include <sal/appl/sal.h>
#include <soc/defs.h>
#include <soc/drv.h>

#include <shared/bitop.h>

#include <shared/dbx/dbx_xml.h>
#include <shared/dbx/dbx_file.h>
#include <shared/utilex/utilex_rhlist.h>
#include <shared/utilex/utilex_str.h>
#include <shared/shrextend/shrextend_debug.h>

#include <soc/sand/sand_signals.h>

#include <bcm/types.h>

#ifdef BCM_PETRA_SUPPORT
#include <soc/dpp/dpp_config_defs.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_diag.h>
#endif

#ifdef BCM_DNX_SUPPORT
#include <appl/diag/system.h>
#include <soc/dnx/dnx_data/dnx_data_device.h>
#include <soc/dnx/dbal/dbal.h>
#endif

#ifdef DL_ENABLE
#include <dlfcn.h>
#endif

#define BSL_LOG_MODULE BSL_LS_SOC_COMMON

static sigstruct_t *
sand_signal_struct_get(
    device_t*   device,
    char*       signal_n,
    char*       stage_n,
    char*       block_n)
{
    sigstruct_t *sigstruct;

    if (device->struct_list == NULL)
    {  /* Not initialized yet */
        LOG_WARN(BSL_LS_SOC_COMMON, (BSL_META("Signal Struct DB was not initialized\n")));
        return NULL;
    }

    RHITERATOR(sigstruct, device->struct_list)
    {
       /*
         * If there is specific "from" stage name, only signal from this stage may be matched to it
         */
        if (!ISEMPTY(sigstruct->from_n) && !ISEMPTY(stage_n) && sal_strcasecmp(sigstruct->from_n, stage_n))
            continue;
        /*
         * If there is specific block name, only signal belonging to this block may be matched to it
         */

        if (!ISEMPTY(sigstruct->block_n) && !ISEMPTY(block_n) && sal_strcasecmp(sigstruct->block_n, block_n))
            continue;

        if (!sal_strcasecmp(RHNAME(sigstruct), signal_n))
            return sigstruct;
    }

    return NULL;
}

static sigparam_t *
sand_signal_resolve_get(
    device_t * device,
    char *signal_n)
{
    if (device->param_list == NULL)
    {   /* Not initialized yet */
        LOG_WARN(BSL_LS_SOC_COMMON, (BSL_META("Signal Struct DB was not initialized\n")));
        return NULL;
    }

    return utilex_rhlist_entry_get_by_name(device->param_list, signal_n);
}

#ifdef CMODEL_SERVER_MODE
#include <soc/dnx/cmodel/cmodel_reg_access.h>

typedef struct {
    char*  start; /* Don't modify - keep to use for sal_free */
    char*  data;
    uint32 length;
    int    nof;
} cmodel_info_t;

#define CMODEL_INFO_GET_UINT32(mc_var, mc_info)                                                             \
                if((mc_info)->length >= sizeof(uint32))                                                     \
                {                                                                                           \
                    mc_var = bcm_ntohl(*((uint32 *)(mc_info)->data));                                       \
                    (mc_info)->length -= sizeof(uint32); (mc_info)->data += sizeof(uint32);                 \
                }                                                                                           \
                else                                                                                        \
                {                                                                                           \
                    SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Request 4(uint32) vs %u\n", (mc_info)->length);         \
                }

#define CMODEL_INFO_GET_MEM(mc_mem, mc_mem_length, mc_info)                                                 \
                if(mc_mem_length > DSIG_MAX_SIZE_STR)                                                       \
                {                                                                                           \
                    SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Signal size:%u is more than maximum supported:%u\n",    \
                                                            mc_mem_length, DSIG_MAX_SIZE_STR);              \
                }                                                                                           \
                if((mc_info)->length >= mc_mem_length)                                                      \
                {                                                                                           \
                    sal_memcpy(mc_mem, (mc_info)->data, mc_mem_length);                                     \
                    (mc_info)->length -= mc_mem_length; (mc_info)->data += mc_mem_length;                   \
                }                                                                                           \
                else                                                                                        \
                {                                                                                           \
                    SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Signal size:%u is more than buffer available:%u\n",     \
                                                                mc_mem_length, (mc_info)->length);          \
                }

static shr_error_e
sand_cmodel_get_signals(
        device_t * device,
        int unit,
        pp_block_t *cur_pp_block,
        match_t * match_p)
{
    cmodel_info_t *cmodel_info;
    int i_st;
    debug_signal_t *debug_signal;
    pp_stage_t *cur_pp_stage;
    uint32 name_length;

    SHR_FUNC_INIT_VARS(unit);

    if((cmodel_info  = utilex_alloc(sizeof(cmodel_info_t) * cur_pp_block->stage_num)) == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_MEMORY, "Memory allocation of:%zu failed for cmodel info\n",
                                                         sizeof(char *) * cur_pp_block->stage_num);
    }
    cur_pp_block->signal_num = 0;
    for(i_st = 0; i_st < cur_pp_block->stage_num; i_st++)
    {
        cur_pp_stage = &cur_pp_block->stages[i_st];
        /** Filter signal fetching if from condition was used */
        if (!ISEMPTY(match_p->from) && (sal_strcasestr(cur_pp_stage->name, match_p->from) == NULL))
            continue;
        SHR_IF_ERR_EXIT(cmodel_get_signal(unit, cur_pp_stage->id, &cmodel_info[i_st].length, &cmodel_info[i_st].start));
        if((cmodel_info[i_st].length == 0) || (cmodel_info[i_st].start == NULL))
        {
            LOG_WARN(BSL_LS_SOC_COMMON, (BSL_META("No signal info received from cmodel for stage:%s\n"), cur_pp_stage->name));
            continue;
        }
        cmodel_info[i_st].data = cmodel_info[i_st].start;
        /*
         * First 4 bytes of buffer represent number of signals
         */
        CMODEL_INFO_GET_UINT32(cmodel_info[i_st].nof, &cmodel_info[i_st]);
        cur_pp_block->signal_num += cmodel_info[i_st].nof;
    }

    if(cur_pp_block->debug_signals != NULL)
    {
        utilex_free(cur_pp_block->debug_signals);
        cur_pp_block->debug_signals = NULL;
    }

    if(cur_pp_block->signal_num == 0)
    {
        SHR_ERR_EXIT(_SHR_E_NONE, "No signals obtained from cmodel\n");
    }

    if((debug_signal = utilex_alloc(sizeof(debug_signal_t) * cur_pp_block->signal_num)) == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_MEMORY, "Memory allocation of:%zu failed for debug signals\n",
                                                        sizeof(debug_signal_t) * cur_pp_block->signal_num);
    }

    cur_pp_block->debug_signals = debug_signal;
    /*
     * Now traverse the buffer and extract stage names and id
     */
    for(i_st = 0; i_st < cur_pp_block->stage_num; i_st++)
    {
        int i_sig, org_length;
        char signal_str[DSIG_MAX_SIZE_STR + 1];
        cur_pp_stage = &cur_pp_block->stages[i_st];
        org_length = cmodel_info[i_st].length;
        for(i_sig = 0;  i_sig < cmodel_info[i_st].nof; i_sig++)
        {
            CMODEL_INFO_GET_UINT32(name_length, &cmodel_info[i_st]);
            CMODEL_INFO_GET_MEM(&debug_signal->attribute, name_length, &cmodel_info[i_st]);
            CMODEL_INFO_GET_UINT32(debug_signal->size, &cmodel_info[i_st]);
            sal_memset(signal_str, 0, DSIG_MAX_SIZE_STR + 1);
            CMODEL_INFO_GET_MEM(signal_str, (debug_signal->size + 3) / 4 + 2, &cmodel_info[i_st]);

            parse_long_integer(debug_signal->value, DSIG_MAX_SIZE_UINT32, signal_str);
            if(i_st != cur_pp_block->stage_num - 1) {
                sal_strcpy(debug_signal->to, cur_pp_block->stages[i_st + 1].name);
            }
            sal_strcpy(debug_signal->from,    cur_pp_stage->name);
            sal_strcpy(debug_signal->block_n, cur_pp_block->name);
            sal_sprintf(debug_signal->hw, "%s_%s", cur_pp_stage->name, debug_signal->attribute);
            /*
             * Now check if there is such name in SignalStructs
             */
            if ((sand_signal_struct_get(device, debug_signal->attribute, debug_signal->from, debug_signal->block_n)) != NULL)
            {   /* If there is match - attribute serves as expansion */
                strcpy(debug_signal->expansion, debug_signal->attribute);
            }
            if ((sand_signal_resolve_get(device, debug_signal->attribute)) != NULL)
            {   /* there is match - attribute serves as resolution param name */
                strcpy(debug_signal->resolution, debug_signal->attribute);
            }
            debug_signal++;
        }
        if(cmodel_info[i_st].length != 0)
        {
            LOG_WARN(BSL_LS_SOC_COMMON, (BSL_META("%d bytes left in %s buffer\n"), cmodel_info[i_st].length, cur_pp_stage->name));
        }
    }

exit:
    if(cmodel_info != NULL)
    {
        for(i_st = 0; i_st < cur_pp_block->stage_num; i_st++)
        {
            if(cmodel_info[i_st].start != NULL)
                sal_free(cmodel_info[i_st].start);
        }
        utilex_free(cmodel_info);
    }
    SHR_FUNC_EXIT;
}

static shr_error_e
sand_cmodel_get_stages(
        int unit,
        pp_block_t *cur_pp_block)
{
    cmodel_info_t stage_info_m;
    int i_ms, name_length;
    pp_stage_t *cur_pp_stage;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(cmodel_get_block_names(unit, (int *)&stage_info_m.length, &stage_info_m.start));
    if((stage_info_m.start == NULL) || (stage_info_m.length == 0))
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "CMODEL Returned NULL buffer\n");
    }
    stage_info_m.data = stage_info_m.start;
    /*
     * First 4 bytes of buffer represent number of ms
     */
    CMODEL_INFO_GET_UINT32(stage_info_m.nof, &stage_info_m);
    cur_pp_block->stages = utilex_alloc(sizeof(pp_stage_t) * stage_info_m.nof);
    /*
     * Now traverse the buffer and extract stage names and id
     */
    for(i_ms = 0; (i_ms < stage_info_m.nof) && (stage_info_m.length > sizeof(uint32) * 2); i_ms++)
    {
        if (cur_pp_block->stage_num == stage_info_m.nof)
            break;
        cur_pp_stage = &cur_pp_block->stages[cur_pp_block->stage_num++];
        CMODEL_INFO_GET_UINT32(cur_pp_stage->id,             &stage_info_m);
        CMODEL_INFO_GET_UINT32(name_length,                  &stage_info_m);
        CMODEL_INFO_GET_MEM(cur_pp_stage->name, name_length, &stage_info_m);
    }
    if(stage_info_m.length != 0)
    {
        LOG_WARN(BSL_LS_SOC_COMMON, (BSL_META("%d bytes left in stages buffer\n"), stage_info_m.length));
    }
exit:
    if(stage_info_m.start != NULL)
        sal_free(stage_info_m.start);
    SHR_FUNC_EXIT;
}

#endif /* CMODEL_SERVER_MODE */


static int
sand_signal_param_get(
    int unit,
    device_t * device,
    char *signal_n,
    uint32 value,
    char *value_n,
    int show_value)
{
    int res = _SHR_E_NOT_FOUND;
    sigparam_t *cur_sigparam;
    sigparam_value_t *cur_sigparam_value;

    if ((cur_sigparam = sand_signal_resolve_get(device, signal_n)) == NULL)
    {
        SHR_EXIT();
    }

#ifdef BCM_DNX_SUPPORT
    /*
     * Use DBAL for signal resolution, SignalStructure.xml should contaion mapping between signal DBAL name
     */
    if(SOC_IS_DNX(unit) && !ISEMPTY(cur_sigparam->dbal_n))
    {
        if((res = dbalc_fields_string_form_hw_value_get(unit, cur_sigparam->dbal_n, value, value_n)) != _SHR_E_NONE)
        {
            LOG_WARN(BSL_LOG_MODULE, (BSL_META_U(unit, "DBAL resolution failed for %s=%d\n"), cur_sigparam->dbal_n, value));
        }
        else
        {
            SHR_EXIT();
        }
    }
#endif
    /*
     * Verify that value is suitable for signal resolution size
     */
    if (value >= (1 << cur_sigparam->size))
    {
        cli_out("Value:%d excess signal:%s size:%d\n", value, signal_n, cur_sigparam->size);
        SHR_EXIT();
    }

    RHITERATOR(cur_sigparam_value, cur_sigparam->value_list)
    {
        if (value == cur_sigparam_value->value)
        {
            if (show_value == 1)
                sal_sprintf(value_n, "%s(0x%x)", RHNAME(cur_sigparam_value), value);
            else
                sal_sprintf(value_n, "%s", RHNAME(cur_sigparam_value));
            res = _SHR_E_NONE;
            break;
        }
    }

    if ((res != _SHR_E_NONE) && !ISEMPTY(cur_sigparam->default_str))
    {   /* Copy default param name if not found */
        sal_sprintf(value_n, "%s(%d)", cur_sigparam->default_str, value);
        res = _SHR_E_NONE;
    }

exit:
    return res;
}

int
sand_signal_address_parse(
    char *full_address,
    signal_address_t * address)
{
    char **tokens;
    uint32 realtokens = 0;
    int i_tok;

    if ((tokens = utilex_str_split(full_address, "|", DSIG_MAX_ADDRESS_RANGE_NUM, &realtokens)) == NULL)
    {
        return 0;
    }

    for (i_tok = 0; i_tok < realtokens; i_tok++)
    {
        sscanf(tokens[i_tok], " {15'd%d,16'd%d} bits: [%d : %d]", &address->high, &address->low, &address->msb,
               &address->lsb);
        address++;
    }

    utilex_str_split_free(tokens, realtokens);

    return realtokens;
}

static int
sand_signal_range_parse(
    char *bit_range,
    int *startbit_p,
    int *endbit_p)
{
    int startbit = -1, endbit = -1;
    if (strstr(bit_range, ":") == NULL)
    {
        sscanf(bit_range, "%d", &endbit);
        startbit = endbit;
    }
    else
        sscanf(bit_range, "%d:%d", &endbit, &startbit);

    if ((endbit < startbit) || (startbit >= DSIG_MAX_SIZE_BITS) || (endbit >= DSIG_MAX_SIZE_BITS) || (startbit < 0)
        || (endbit < 0))
        return _SHR_E_INTERNAL;

    if (startbit <= endbit)
    {
        *startbit_p = startbit;
        *endbit_p = endbit;
    }
    else
    {
        *startbit_p = endbit;
        *endbit_p = startbit;
    }

    return _SHR_E_NONE;
}

void
sand_signal_struct_expand_init(
    device_t * device)
{
    sigstruct_t *sigstruct;
    /*
     * Go through field and fill expansion were available
     */
    RHITERATOR(sigstruct, device->struct_list)
    {
        sigstruct_field_t *sigstruct_field;
        RHITERATOR(sigstruct_field, sigstruct->field_list)
        {
            if (!ISEMPTY(sigstruct_field->expansion_m.name))
            {
                /*
                 * If expansion is not empty and not dynamic, find the expansion, if failed zero the field
                 */
                if (sal_strcasecmp(sigstruct_field->expansion_m.name, "Dynamic"))
                    if (sand_signal_struct_get(device, sigstruct_field->expansion_m.name,
                                                        sigstruct->from_n, sigstruct->block_n) == NULL)
                    {
                        LOG_ERROR(BSL_LOG_MODULE, (BSL_META("No signal expansion:%s\n"), sigstruct_field->expansion_m.name));
                        SET_EMPTY(sigstruct_field->expansion_m.name);
                    }
            }
            else
            {
                /*
                 * When expansion is empty first check may be it can be expanded by name
                 */
                if (sand_signal_struct_get(device, RHNAME(sigstruct_field),
                                                    sigstruct->from_n, sigstruct->block_n) != NULL)
                {
                    strcpy(sigstruct_field->expansion_m.name, RHNAME(sigstruct_field));
                }
                /*
                 * Not found check resolution
                 */
                else if (!ISEMPTY(sigstruct_field->resolution))
                {
                    /*
                     * If resolution is not empty look if it exists
                     */
                    if (sand_signal_resolve_get(device, sigstruct_field->resolution) == NULL)
                    {
                        LOG_ERROR(BSL_LOG_MODULE, (BSL_META("No signal resolution:%s\n"), sigstruct_field->resolution));
                        SET_EMPTY(sigstruct_field->resolution);
                    }
                }
                else
                {
                    /*
                     * When resolution is empty check may be it can be expanded by name
                     */
                    if (sand_signal_resolve_get(device, RHNAME(sigstruct_field)) != NULL)
                        /*
                         * If there is match - attribute serves as resolution
                         */
                        strcpy(sigstruct_field->resolution, RHNAME(sigstruct_field));
                }
            }
        }
    }

    return;
}

void
sand_signal_list_free(
    rhlist_t * output_list)
{
    if(output_list)
    {
        signal_output_t *signal_output;

        RHITERATOR(signal_output, output_list)
        {
            sand_signal_list_free(signal_output->field_list);
        }
        utilex_rhlist_free_all(output_list);
    }
}

static void
sand_signal_get_value(
    int unit,
    int core_id,
    debug_signal_t * debug_signal,
    uint32 * value)
{
#ifdef CMODEL_SERVER_MODE
    sal_memcpy(value, debug_signal->value, DSIG_MAX_SIZE_BYTES);
#else
#ifdef BCM_PETRA_SUPPORT
    int j;
    ARAD_PP_DIAG_REG_FIELD debug_field;
    signal_address_t *address;
    uint32 range_val[ARAD_PP_DIAG_DBG_VAL_LEN];
    int range_size;
    int last_pos;
    int uint_num;

    uint_num = BITS2WORDS(debug_signal->size);
    if (uint_num > DSIG_MAX_SIZE_UINT32)
    {
        cli_out("Bad signal_size:%d\n", debug_signal->size);
        uint_num = DSIG_MAX_SIZE_UINT32;
    }

    sal_memset(value, 0, sizeof(int) * uint_num);

    last_pos = 0;
    for (j = 0; j < debug_signal->range_num; j++)
    {
        address = &(debug_signal->address[j]);
        debug_field.base = (address->high << 16) + address->low;
        debug_field.lsb = address->lsb;
        debug_field.msb = address->msb;
        range_size = debug_field.msb + 1 - debug_field.lsb;
        arad_pp_diag_dbg_val_get_unsafe(unit, core_id, debug_signal->block_id, &debug_field, range_val);
        SHR_BITCOPY_RANGE(value, last_pos, range_val, 0, range_size);
        last_pos += range_size;
    }
#endif /* BCM_PETRA_SUPPORT */
#endif
    return;
}

static void
sand_value_to_str(
    int unit,
    device_t * device,
    char *resolution_n,
    uint32 * org_source,
    char *dest,
    int bit_size,
    int byte_order)
{
    int i, j;
    int byte_size, long_size;
    int dest_byte_index, source_byte_index, real_byte_index;
    uint8 *source = (uint8 *) org_source;

    byte_size = BITS2BYTES(bit_size);
    long_size = BYTES2WORDS(byte_size);
    sal_memset(dest, 0, DSIG_MAX_SIZE_STR);

    if (!ISEMPTY(resolution_n) && (sand_signal_param_get(unit, device, resolution_n, *org_source, dest, 1) == _SHR_E_NONE))
        return;

    dest_byte_index = 0;
#ifdef BE_HOST
    if (byte_order == PRINT_LITTLE_ENDIAN)
    {
        for (i = 0; i < long_size; i++)
        {
            for (j = 0; j < 4; j++)
            {
                source_byte_index = 4 * i + 3 - j;
                real_byte_index = 4 * i + j;
                if (real_byte_index >= byte_size)
                    continue;
                sal_sprintf(&dest[2 * dest_byte_index], "%02x", source[source_byte_index]);
                dest_byte_index++;
            }
        }
    }
    else
    {
        for (i = 0; i < long_size; i++)
        {
            for (j = 0; j < 4; j++)
            {
                source_byte_index = 4 * (long_size - 1 - i) + j;
                real_byte_index = 4 * (long_size - 1 - i) + 3 - j;
                if (real_byte_index >= byte_size)
                    continue;
                sal_sprintf(&dest[2 * dest_byte_index], "%02x", source[source_byte_index]);
                dest_byte_index++;
            }
        }
    }
#else
    if (byte_order == PRINT_LITTLE_ENDIAN)
    {
        for (i = 0; i < long_size; i++)
        {
            for (j = 0; j < 4; j++)
            {
                source_byte_index = 4 * i + j;
                real_byte_index = 4 * i + j;
                if (real_byte_index >= byte_size)
                    continue;
                sal_sprintf(&dest[2 * dest_byte_index], "%02x", source[source_byte_index]);
                dest_byte_index++;
            }
        }
    }
    else
    {
        for (i = 0; i < long_size; i++)
        {
            for (j = 0; j < 4; j++)
            {
                source_byte_index = 4 * (long_size - 1 - i) + 3 - j;
                real_byte_index = 4 * (long_size - 1 - i) + 3 - j;
                if (real_byte_index >= byte_size)
                    continue;
                sal_sprintf(&dest[2 * dest_byte_index], "%02x", source[source_byte_index]);
                dest_byte_index++;
            }
        }
    }
#endif
}

static void
dsig_print_description(
    match_t * match_p)
{
    cli_out("Unknown Signal ");
    if (!ISEMPTY(match_p->name))
        cli_out(":%s ", match_p->name);
    if (!ISEMPTY(match_p->block))
        cli_out("block:%s ", match_p->block);
    if (!ISEMPTY(match_p->from))
        cli_out("from:%s ", match_p->from);
    if (!ISEMPTY(match_p->to))
        cli_out("to:%s ", match_p->to);
    cli_out("\n");
}

/*
 * Copy field_size bits from "signal_value" with offset "field_offset" to field_value with size "field_size"
 */
static void
sand_signal_field_get_value(
    uint32 * signal_value,
    uint32 * field_value,
    int field_offset,
    int field_size)
{
    SHR_BITCOPY_RANGE(field_value, 0, signal_value, field_offset, field_size);
}

static int
sand_signal_field_get(
    int unit,
    device_t * device,
    char *sigstruct_n,
    char *stage_n,
    char *block_n,
    uint32 * signal_value,
    char *field_name,
    char *field_str,
    uint32 * field_value)
{
    sigstruct_t *sigstruct;
    sigstruct_field_t *sigstruct_field;
    int res = _SHR_E_NOT_FOUND;
    uint32 cur_field_value[DSIG_MAX_SIZE_UINT32];

    /*
     * Split by dot into struct and lower field, but only once, so maximum tokens = 2
     */
    char **tokens;
    uint32 realtokens = 0;

    if ((tokens = utilex_str_split(field_name, ".", 2, &realtokens)) == NULL)
    {
        return res;
    }

    if (!ISEMPTY(sigstruct_n) && (sigstruct = sand_signal_struct_get(device, sigstruct_n, stage_n, block_n)) != NULL)
    {
        RHITERATOR(sigstruct_field, sigstruct->field_list)
        {
            /*
             * We are looking for first token resolution
             */
            if (!sal_strcasecmp(tokens[0], RHNAME(sigstruct_field)))
            {
                sal_memset(cur_field_value, 0, DSIG_MAX_SIZE_BYTES);
                /*
                 * Obtain the value
                 */
                sand_signal_field_get_value(signal_value, cur_field_value, sigstruct_field->start_bit, sigstruct_field->size);
                /*
                 * If this is the last token - obtain string value
                 */
                if (realtokens == 1)
                {
                    sal_memcpy(field_value, cur_field_value, DSIG_MAX_SIZE_BYTES);
                    if (field_str != NULL)
                    {
                        res = sand_signal_param_get(unit, device, RHNAME(sigstruct_field), *cur_field_value, field_str, 0);
                        if (res != _SHR_E_NONE)
                            sal_sprintf(field_str, "%u", *cur_field_value);
                    }
                    res = _SHR_E_NONE;
                }
                else
                {
                    res = sand_signal_field_get(unit, device, sigstruct_field->expansion_m.name, stage_n, block_n, cur_field_value, tokens[1],
                                         field_str, field_value);
                }
                break;
            }
        }
    }

    utilex_str_split_free(tokens, realtokens);

    return res;
}

static sigstruct_t *
sand_signal_expand_get(
    int     unit,
    device_t * device,
    sigstruct_t *sigstruct_in,
    uint32 * value,
    expansion_t * expansion)
{
    int i, n;
    expansion_option_t *option;
    char field_str[RHNAME_MAX_SIZE];
    uint32 field_value[DSIG_MAX_SIZE_UINT32];
    int match_flag;
    sigstruct_t *sigstruct_out = NULL;
    uint32 rt_value, db_value;

    if (ISEMPTY(expansion->name))       /* No explicit expansion */
    {
        goto exit;
    }
    else if (sal_strcasecmp(expansion->name, "dynamic"))
    {
        /** static expansion */
        sigstruct_out = sand_signal_struct_get(device, expansion->name, sigstruct_in->from_n, sigstruct_in->block_n);
        goto exit;
    }

    /*
     * Dynamic expansion
     */
    option = expansion->options;
    for (i = 0; i < expansion->option_num; i++)
    {
        match_flag = TRUE;
        n = 0;
        while ((n < DSIG_OPTION_PARAM_MAX_NUM) && !ISEMPTY(option->param[n].name))
        {
            if (sand_signal_field_get(unit, device, RHNAME(sigstruct_in), sigstruct_in->from_n, sigstruct_in->block_n, value, option->param[n].name, field_str, field_value) !=
                _SHR_E_NONE)
            {
                match_flag = FALSE;
                break;
            }
            if (sal_strcasecmp(option->param[n].value, field_str))
            {
                if ((utilex_str_stoul(field_str, &rt_value) != _SHR_E_NONE)
                    || (utilex_str_stoul(option->param[n].value, &db_value) != _SHR_E_NONE) || (rt_value != db_value))
                {
                    match_flag = FALSE;
                    break;
                }
            }

            n++;
        }
        if (match_flag == TRUE)
        {       /* Found our expansion leave option loop */
            if((sigstruct_out = sand_signal_struct_get(device, option->name, sigstruct_in->from_n, sigstruct_in->block_n)) == NULL)
            {
                LOG_ERROR(BSL_LOG_MODULE, (BSL_META("Expansion:%s does not exist\n"), option->name));
                goto exit;
            }
            /*
             * In case expanded structure is smaller than original one, we need to trim the value from eiher LS or MS side
             * LS is easy no need to do anything, for MS we need to obtain new values
             */
            if(sigstruct_out->size > sigstruct_in->size)
            {
                LOG_ERROR(BSL_LOG_MODULE, (BSL_META("Expansion:%s cannot be larger then origin\n"), RHNAME(sigstruct_out)));
                goto exit;
            }
            if(option->trim_side == TRIM_MS)
            {
                uint32  temp_value[DSIG_MAX_SIZE_UINT32];
                sal_memset(temp_value, 0, BITS2WORDS(sigstruct_out->size) * sizeof(uint32));
                SHR_BITCOPY_RANGE(temp_value, 0, value, sigstruct_in->size - sigstruct_out->size, sigstruct_out->size);
                sal_memcpy(value, temp_value, BITS2WORDS(sigstruct_out->size) * sizeof(uint32));
            }
            break;
        }
        option++;
    }
exit:
    return sigstruct_out;
}

static void
sand_signal_match_list(
        rhlist_t *field_list,
        int flags,
        char *match_n)
{
    signal_output_t *field_output;

    RHSAFE_ITERATOR(field_output, field_list)
    {
        int valid = FALSE;
        /*
         * Either the
         */
        if ((match_n == NULL)   /* No match string on input */
            || ((flags & SIGNALS_MATCH_EXACT) && !sal_strcasecmp(RHNAME(field_output), match_n))
            || (!(flags & SIGNALS_MATCH_EXACT) && (sal_strcasestr(RHNAME(field_output), match_n) != NULL)))
        {
            valid = TRUE;
        }
        else
        {
            sand_signal_match_list(field_output->field_list, flags, match_n);
            if((field_output->field_list != NULL) && (RHLNUM(field_output->field_list) != 0))
                valid = TRUE;
        }
        if(valid == FALSE)
        {
            /*
             * No match on this field - delete list from the field, field from parent list and continue with search
             */
            sand_signal_list_free(field_output->field_list);
            utilex_rhlist_entry_del_free(field_list, field_output);
        }
    }

    return;
}

static int
sand_signal_expand(
    int         unit,
    device_t * device,
    uint32 * signal_value,
    int size_in,
    char *sigstruct_n,
    char *stage_n,
    char *block_n,
    int byte_order,
    char *match_n,
    int flags,
    sigstruct_t **sigstruct_p,
    rhlist_t ** field_list_p)
{
    int size = 0;              /* By default no match - we yet need to find one, and it will be size */
    sigstruct_t *sigstruct = NULL, *sigstruct_exp, *sigstruct_exp4field;
    sigstruct_field_t *sigstruct_field;
    uint32 field_value[DSIG_MAX_SIZE_UINT32];
    char *field_dyn_name;
    signal_output_t *field_output;
    int top_valid, child_valid;
    char *cur_match_n;
    rhhandle_t temp = NULL;

    rhlist_t *field_list = NULL, *child_list = NULL;

    char **tokens;
    uint32 realtokens = 0, maxtokens = 2;

    tokens = utilex_str_split(match_n, ".", maxtokens, &realtokens);
    if (realtokens && (tokens == NULL))
    {
        goto exit;
    }

    if (realtokens == maxtokens)
    {
        match_n = tokens[0];
    }

    /*
     * Check if there is an expansion at all
     */
    if (ISEMPTY(sigstruct_n) || (sigstruct = sand_signal_struct_get(device, sigstruct_n, stage_n, block_n)) == NULL)
        goto exit;

    if(sigstruct->expansion_cb != NULL)
    {
        /*
         * Calling functions asign size 0 when there is no knowledge on it
         * And if there is no size in structure - error
         */
        if(size_in == 0)
        {
            if(sigstruct->size == 0)
            {
                LOG_ERROR(BSL_LOG_MODULE, (BSL_META("No size for:%s\n"), RHNAME(sigstruct)));
                goto exit;
            }
            size_in = sigstruct->size;
        }
        if(sigstruct->expansion_cb(unit, sigstruct->expansion_m.name, signal_value, size_in, &field_list) != _SHR_E_NONE)
        {
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META("Fail to expand on cb for:%s\n"), RHNAME(sigstruct)));
            goto exit;
        }
        /* Now check if we have a match just like we do below for regular XML parsing */
        if(field_list)
        {
            sand_signal_match_list(field_list, flags, match_n);

            if(RHLNUM(field_list) != 0)
                size = size_in;
            else
            {
                sand_signal_list_free(field_list);
                field_list = NULL;
            }
        }
        goto exit;
    }
    /*
     * In case of dynamic expansion find the matching expansion and switch to it
     */
    if ((sigstruct_exp = sand_signal_expand_get(unit, device, sigstruct, signal_value, &sigstruct->expansion_m)) != NULL)
        sigstruct = sigstruct_exp;

    RHITERATOR(sigstruct_field, sigstruct->field_list)
    {
        child_valid = 0;
        child_list = NULL;
        /*
         * If there is a condition check that it is fulfilled
         */
        if (!(flags & SIGNALS_MATCH_NOCOND) && !ISEMPTY(sigstruct_field->cond_attribute))
        {
            sand_signal_field_get(unit, device, RHNAME(sigstruct), stage_n, block_n, signal_value, sigstruct_field->cond_attribute, NULL, field_value);
            if (0 == VALUE(field_value))
                continue;
        }
        /*
         * Check for further usage dynamic expansion for field
         * In case of dynamic expansion find the matching expansion and switch to it - in this case for field
         */
        sigstruct_exp4field = sand_signal_expand_get(unit, device, sigstruct, signal_value, &sigstruct_field->expansion_m);
        if (sigstruct_exp4field)
            field_dyn_name = RHNAME(sigstruct_exp4field);
        else
            field_dyn_name = sigstruct_field->expansion_m.name;

        sal_memset(field_value, 0, DSIG_MAX_SIZE_BYTES);
        sand_signal_field_get_value(signal_value, field_value, sigstruct_field->start_bit, sigstruct_field->size);

        if ((match_n == NULL)   /* No match string on input */
            || ((flags & SIGNALS_MATCH_EXACT) && !sal_strcasecmp(RHNAME(sigstruct_field), match_n))
            || (!(flags & SIGNALS_MATCH_EXACT) && (sal_strcasestr(RHNAME(sigstruct_field), match_n) != NULL)))
            top_valid = 1;
        else
            top_valid = 0;

        if ((top_valid == 1) && (realtokens == maxtokens))
        {
            /*
             * Look into subfield for match, above match is not enough
             */
            top_valid = 0;
            cur_match_n = tokens[1];
        }
        else
        {
            cur_match_n = top_valid ? NULL : match_n;
        }

        if (sigstruct_exp4field)
        {
            child_valid = sand_signal_expand(unit, device, field_value, 0, RHNAME(sigstruct_exp4field), stage_n, block_n,
                                        byte_order, cur_match_n, flags, NULL, &child_list);
        }

        if ((top_valid == 0) && (child_valid == 0))
        {
            continue;
        }

        size = sigstruct->size;      /* Once we have match return will be non zero */

        /*
         * Allocate structure for the child
         */
        if (field_list == NULL)
        {
            if ((field_list = utilex_rhlist_create("field_list", sizeof(signal_output_t), 0)) == NULL)
                goto exit;
        }
        /*
         * Now we can allocate output and fill it
         */
        if (utilex_rhlist_entry_add_tail(field_list, NULL, RHID_TO_BE_GENERATED, &temp) != _SHR_E_NONE)
        {
            /*
             * No more place any more - return with what you have until now
             */
            break;
        }
        field_output = temp;

        strcpy(RHNAME(field_output), RHNAME(sigstruct_field));

        strcpy(field_output->expansion, field_dyn_name);

        memcpy(field_output->value, field_value, DSIG_MAX_SIZE_BYTES);

        sand_value_to_str(unit, device, sigstruct_field->resolution, field_value, field_output->print_value,
                           sigstruct_field->size, byte_order);
        field_output->field_list = child_list;
        field_output->size = sigstruct_field->size;
        if (flags & SIGNALS_MATCH_ONCE)
        {
            /*
             * No need to look anymore
             */
            break;
        }
    }

exit:
    if(sigstruct_p != NULL)
    {
        *sigstruct_p = sigstruct;
    }
    *field_list_p = field_list;
    utilex_str_split_free(tokens, realtokens);
    return size;
}

shr_error_e
sand_signal_parse_exists(
    int unit,
    char *expansion_n)
{
    shr_error_e rv = _SHR_E_NONE;

    device_t * device;

    if ((device = sand_signal_device_get(unit)) == NULL)
    {
        rv = _SHR_E_NOT_FOUND;
        goto exit;
    }

    if (ISEMPTY(expansion_n) || sand_signal_struct_get(device, expansion_n, NULL, NULL) == NULL)
    {
        rv = _SHR_E_NOT_FOUND;
        goto exit;
    }

exit:
    return rv;
}

shr_error_e
sand_signal_parse_get(
    int unit,
    char *expansion_n,
    char *block_n,
    char *from_n,
    int output_order,
    uint32 *value,
    int size_in,
    rhlist_t * dsig_list)
{
    shr_error_e rv = _SHR_E_NONE;

    device_t * device;
    rhhandle_t temp = NULL;
    signal_output_t *signal_output;
    rhlist_t *field_list = NULL;
    sigstruct_t *sigstruct;
    int size;

    if ((device = sand_signal_device_get(unit)) == NULL)
    {
        rv = _SHR_E_NOT_FOUND;
        goto exit;
    }

    /*
     * Generate list of fields per signal, size if actual expansion size
     */
    if((size = sand_signal_expand(unit, device, value, size_in, expansion_n, from_n, block_n, output_order, NULL, 0, &sigstruct, &field_list)) == 0)
    {
        sand_signal_list_free(field_list);
        rv = _SHR_E_NOT_FOUND;
        goto exit;
    }
    /*
     * Now we can allocate output and fill it
     */
    if((rv = utilex_rhlist_entry_add_tail(dsig_list, expansion_n, RHID_TO_BE_GENERATED,  &temp)) != _SHR_E_NONE)
    {   /** free already allocated child list, that we have no way to preserve */
        sand_signal_list_free(field_list);
        rv = _SHR_E_MEMORY;
        goto exit;
    }

    signal_output = temp;
    /*
     * Create print value from raw one
     */
    sand_value_to_str(unit, device, expansion_n, value, signal_output->print_value, size, output_order);
    /*
     * Fill pointer to structure fields
     */
    signal_output->size = size;
    signal_output->field_list = field_list;
    strcpy(signal_output->expansion, RHNAME(sigstruct));

exit:
    return rv;
}

shr_error_e
sand_signal_list_get(
    device_t * device,
    int unit,
    int core,
    match_t * match_p,
    rhlist_t * dsig_list)
{
    shr_error_e rv = _SHR_E_NONE;
    int i, j;
    char *match_n;

    pp_block_t *cur_pp_block;
    debug_signal_t *debug_signals;
    signal_output_t *signal_output = NULL;
    rhhandle_t temp = NULL;

    int top_valid, child_valid;
    /*
     * Dynamic Data
     */
    uint32 value[DSIG_MAX_SIZE_UINT32];
    char *cur_match_n;
    rhlist_t *field_list = NULL;

    char **tokens = NULL;
    uint32 realtokens = 0, maxtokens = 2;

    if (device->pp_blocks == NULL)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META("Signal DB for:%s was not initialized\n"), SOC_CHIP_STRING(unit)));
        rv = _SHR_E_INTERNAL;
        goto exit;
    }
    /*
     * Zero the "value" just to be sure than in any case of problem it will be null string
     */
    memset(value, 0, sizeof(uint32) * DSIG_MAX_SIZE_UINT32);

    match_n = match_p->name;

    tokens = utilex_str_split(match_n, ".", maxtokens, &realtokens);
    if (realtokens && (tokens == NULL))
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META("Problem parsing match string\n")));
        rv = _SHR_E_INTERNAL;
        goto exit;
    }

    if (realtokens == maxtokens)
    {
        match_n = tokens[0];
    }

    for (i = 0; i < device->block_num; i++)
    {
        cur_pp_block = &device->pp_blocks[i];

        if (!ISEMPTY(match_p->block) && sal_strcasecmp(cur_pp_block->name, match_p->block))
            continue;

#ifdef CMODEL_SERVER_MODE
        if((rv = sand_cmodel_get_signals(device, unit, cur_pp_block, match_p)) != _SHR_E_NONE)
            goto exit;
#endif
        debug_signals = cur_pp_block->debug_signals;
        for (j = 0; j < cur_pp_block->signal_num; j++)
        {
            if ((match_p->flags & SIGNALS_MATCH_PERM) && (debug_signals[j].perm == 0))
                continue;

            if (!ISEMPTY(match_p->stage))
            {
                /** From == To means we want some stage either as from or to, so e one of conditions need to be true */
                if ((sal_strcasestr(debug_signals[j].from, match_p->stage) == NULL)
                    && (sal_strcasestr(debug_signals[j].to, match_p->stage) == NULL))
                    continue;
            }
            else
            {
                /** Here one of both conditions must be true to compare the attributes */
                if (!ISEMPTY(match_p->from) && (sal_strcasestr(debug_signals[j].from, match_p->from) == NULL))
                    continue;
                if (!ISEMPTY(match_p->to) && (sal_strcasestr(debug_signals[j].to, match_p->to) == NULL))
                    continue;
            }
            if ((match_n == NULL)       /* No match string on input */
                || ((match_p->flags & SIGNALS_MATCH_EXACT) &&   /* Match should be exact one */
                    (!sal_strcasecmp(debug_signals[j].attribute, match_n) || ((match_p->flags & SIGNALS_MATCH_HW) &&
                     !sal_strcasecmp(debug_signals[j].hw, match_n))))
                || (!(match_p->flags & SIGNALS_MATCH_EXACT) &&  /* Match may be any part of name */
                   ((sal_strcasestr(debug_signals[j].attribute, match_n) != NULL) || ((match_p->flags & SIGNALS_MATCH_HW) &&
                     (sal_strcasestr(debug_signals[j].hw, match_n) != NULL)))))
                top_valid = 1;
            else
                top_valid = 0;
            /*
             * If double flag set and no MATCH_DOUBLE skip this signal
             */
            if (!(match_p->flags & SIGNALS_MATCH_DOUBLE) && (debug_signals[j].double_flag == 1))
                continue;
            /*
             * If there is a condition check that it is fulfilled
             */
            if (!(match_p->flags & SIGNALS_MATCH_NOVALUE) &&
                (!(match_p->flags & SIGNALS_MATCH_NOCOND) && debug_signals[j].cond_signal))
            {
                sand_signal_get_value(unit, core, debug_signals[j].cond_signal, value);
                if (debug_signals[j].cond_value != VALUE(value))
                    continue;
            }

            if ((top_valid == 1) && (realtokens == maxtokens))
            {
                /*
                 * Look into subfield for match, above match is not enough
                 */
                top_valid = 0;
                cur_match_n = tokens[1];
            }
            else
            {
                /** If signal found bring all its sub-fields, meaning using NULL match string */
                cur_match_n = top_valid ? NULL : match_n;
            }
            /*
             * Step 1 - Fill signal value
             */
            if (!(match_p->flags & SIGNALS_MATCH_NOVALUE))
                sand_signal_get_value(unit, core, &debug_signals[j], value);
            /*
             * Step 2 - Now start expansion
             */
            if (match_p->flags & SIGNALS_MATCH_EXPAND)
            {
                child_valid = sand_signal_expand(unit, device, value, debug_signals[j].size, debug_signals[j].expansion, debug_signals[j].from,  debug_signals[j].block_n,
                        match_p->output_order, cur_match_n, match_p->flags, NULL, &field_list);
            }
            else
            {
                child_valid = 0;
                field_list = NULL;
            }

            if ((top_valid == 0) && (child_valid == 0)) /* No match in any place */
                continue;
            /*
             * Now we can allocate output and fill it
             */
            if((rv = utilex_rhlist_entry_add_tail(dsig_list, debug_signals[j].attribute, RHID_TO_BE_GENERATED,  &temp)) != _SHR_E_NONE)
            {   /** free already allocated child list, that we have no way to preserve */
                sand_signal_list_free(field_list);
                goto exit;
            }
            signal_output = temp;
            /*
             * Copy debug signal into output_signal
             */
            signal_output->debug_signal = &debug_signals[j];
            /*
             * Create print value from raw one
             */
            if (!(match_p->flags & SIGNALS_MATCH_NOVALUE))
            {
                char *resolution_n;
                if(match_p->flags & SIGNALS_MATCH_EXPAND)
                    resolution_n = debug_signals[j].resolution;
                else
                    resolution_n = NULL;
                sand_value_to_str(unit, device, resolution_n, value, signal_output->print_value,
                                                                        debug_signals[j].size, match_p->output_order);
                memcpy(signal_output->value, value, DSIG_MAX_SIZE_BYTES);
            }
            /*
             * Fill pointer to structure fields
             */
            signal_output->size = debug_signals[j].size;
            signal_output->core = core;
            signal_output->field_list = field_list;
            signal_output->device = device;
            if (match_p->flags & SIGNALS_MATCH_ONCE)
            {  /* No need to look anymore */
                goto exit;
            }
        }
    }   /* Dynamic Data */

exit:
    utilex_str_split_free(tokens, realtokens);
    return rv;
}

static int
sand_signal_value_read(
    rhlist_t * field_list,
    char *full_name,
    uint32 * value,
    int size)
{
    signal_output_t *cur_signal_output;
    int status;
    char *name;

    char **tokens;
    uint32 realtokens = 0, maxtokens = 2;

    tokens = utilex_str_split(full_name, ".", maxtokens, &realtokens);
    if (realtokens && (tokens == NULL))
    {
        goto exit;
    }

    if (realtokens == maxtokens)
        name = tokens[0];
    else
        name = full_name;

    RHITERATOR(cur_signal_output, field_list)
    {
        status = sal_strcasecmp(RHNAME(cur_signal_output), name);
        /*
         * If no more tokens and strings match we found our target
         */
        if ((realtokens == 1) && (status == 0))
        {
            if (size < BITS2WORDS(cur_signal_output->size))
            {
                cli_out("Not enough memory for signal:%s %d vs %d\n", name, size, cur_signal_output->size);
                return 0;
            }
            else
                memcpy(value, cur_signal_output->value, BITS2WORDS(cur_signal_output->size) * 4);
            return 1;
        }
        else
        {
            /*
             * If we had 2 tokens and strings were match we need to move to new token
             */
            if ((realtokens == maxtokens) && (status == 0))
                name = tokens[1];
            else
                name = full_name;

            /*
             * If we have regular string or strings were not match keep original name
             */
            if (sand_signal_value_read(cur_signal_output->field_list, name, value, size) == 1)
            {
                return 1;
            }
        }
    }

exit:
    utilex_str_split_free(tokens, realtokens);
    return 0;
}

int
dpp_dsig_read(
    int unit,
    int core,
    char *block,
    char *from,
    char *to,
    char *name,
    uint32 * value,
    int size)
{
    int res = _SHR_E_NONE;
    rhlist_t *dsig_list;
    match_t match;
    device_t *device;

    if ((value == NULL) || (size == 0))
    {
        res = _SHR_E_PARAM;
        goto exit;
    }

    if ((device = sand_signal_device_get(unit)) == NULL)
    {
      return res;
    }

    memset(&match, 0, sizeof(match_t));
    match.block = block;
    match.from = from;
    match.to = to;
    match.name = name;
    match.flags = SIGNALS_MATCH_EXPAND | SIGNALS_MATCH_ONCE | SIGNALS_MATCH_EXACT;

    if ((dsig_list = utilex_rhlist_create("signals", sizeof(signal_output_t), 0)) == NULL)
    {
        res = _SHR_E_MEMORY;
        goto exit;
    }

    if ((sand_signal_list_get(device, unit, core, &match, dsig_list) != _SHR_E_NONE) || (RHLNUM(dsig_list) == 0))
    {
        dsig_print_description(&match);
        res = _SHR_E_NOT_FOUND;
    }
    else
    {
        if (sand_signal_value_read(dsig_list, name, value, size) == 0)
        {
            res = _SHR_E_FAIL;
        }
    }

    sand_signal_list_free(dsig_list);

exit:
    return res;
}

int
sand_signal_handle_get(
    device_t * device,
    char *block,
    char *from,
    char *to,
    char *name,
    debug_signal_t ** signal_p)
{
    int signal_num = 0;
    rhlist_t *dsig_list;
    match_t match;
    signal_output_t *signal_output = NULL;
    int unit = 0;               /* Assuming there will be always unit 0 */
    int core = 0;               /* Assuming there will be always core 0 */

    if ((dsig_list = utilex_rhlist_create("signals", sizeof(signal_output_t), 0)) == NULL)
    {
        goto exit;
    }

    memset(&match, 0, sizeof(match_t));
    match.block = block;
    match.from = from;
    match.to = to;
    match.name = name;
    match.flags = SIGNALS_MATCH_EXACT | SIGNALS_MATCH_NOCOND | SIGNALS_MATCH_ONCE | SIGNALS_MATCH_NOVALUE;

    if (sand_signal_list_get(device, unit, core, &match, dsig_list) != _SHR_E_NONE)
    {
        cli_out("Signal:%s was not found\n", name);
    }
    else
    {
        if ((signal_output = utilex_rhlist_entry_get_first(dsig_list)) != NULL)
        {
            *signal_p = signal_output->debug_signal;
            signal_num = 1;
        }
    }

    sand_signal_list_free(dsig_list);

exit:
    return signal_num;
}

#if (defined(NO_FILEIO) || defined(STATIC_SIGNAL_TEST))
/*
 * {
 */
static int
sand_signal_struct_static_init(
    device_t * device,
    static_sigstruct_t *static_sigstructs,
    int static_sigstructs_num)
{
    int res = _SHR_E_NONE;
    static_sigstruct_t *static_sigstruct;
    static_field_t *static_field;
    sigstruct_t *sigstruct;
    sigstruct_field_t *sigstruct_field;
    rhhandle_t temp;

    if (device->struct_list != NULL)
        /*
         * ALready initialized
         */
        return res;

    if ((device->struct_list = utilex_rhlist_create("fields", sizeof(sigstruct_t), 0)) == NULL)
        goto exit;

    /*
     * loop through entries
     */
    for(static_sigstruct = static_sigstructs; static_sigstruct->name != NULL; static_sigstruct++)
    {
        if (utilex_rhlist_entry_add_tail(device->struct_list, static_sigstruct->name, RHID_TO_BE_GENERATED, &temp) != _SHR_E_NONE)
            break;

        sigstruct = temp;
        sigstruct->size = static_sigstruct->size;

        if ((sigstruct->field_list = utilex_rhlist_create("fields", sizeof(sigstruct_field_t), 0)) == NULL)
            goto exit;

        for(static_field = static_sigstruct->fields; static_field->name != NULL; static_field++)
        {
            if (utilex_rhlist_entry_add_tail(sigstruct->field_list, static_field->name, RHID_TO_BE_GENERATED, &temp) != _SHR_E_NONE)
                break;

            sigstruct_field = temp;
            if (sand_signal_range_parse(static_field->bitstr, &sigstruct_field->start_bit,
                                                                        &sigstruct_field->end_bit) != _SHR_E_NONE)
            {
                LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(NO_UNIT, "Field:%s.%s, has bad bites range:%s\n"),
                                                  RHNAME(sigstruct), RHNAME(sigstruct_field), static_field->bitstr));
                continue;
            }

            sigstruct_field->size = sigstruct_field->end_bit + 1 - sigstruct_field->start_bit;
            if (static_field->expansion)
            {
                if (sand_signal_struct_get(device, static_field->expansion, NULL, NULL) != NULL)
                    sal_strncpy(sigstruct_field->expansion_m.name, static_field->expansion, (RHNAME_MAX_SIZE - 1));
                else
                    LOG_WARN(BSL_LS_SOC_COMMON, (BSL_META_U(0, "No field expansion:%s\n"), static_field->expansion));
            }
            else
            {
                /*
                 * No explicit expansion - look for implicit one
                 */
                if ((sand_signal_struct_get(device, static_field->name, NULL, NULL)) != NULL)
                    /*
                     * If there is match - attribute serves as expansion
                     */
                    sal_strncpy(sigstruct_field->expansion_m.name, static_field->name, (RHNAME_MAX_SIZE - 1));
            }
        }
    }

exit:
    return res;
}

device_t *
sand_signal_static_init(
    int unit)
{
    device_t *device = NULL;
    static_device_t *static_device;
    static_block_t *static_block;
    static_signal_t *static_signal;
    pp_block_t *cur_pp_block;

    for(static_device = static_devices; static_device->name != NULL; static_device++)
    {
        if(!sal_strcasecmp(SOC_CHIP_STRING(unit), static_device->name))
            break;
    }

    /* Check if we have found the match */
    if(static_device->name == NULL)
    {
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "Signal Data for %s was not found\n"), SOC_CHIP_STRING(unit)));
        goto exit;
    }

    /* We have a match - now allocate device */
    if((device = utilex_alloc(sizeof(device_t))) == NULL)
        goto exit;

    /* Copy device name */
    sal_strncpy(RHNAME(device), static_device->name, (RHNAME_MAX_SIZE - 1));

    /* Initialize parsing structures for the device */
    if ((sand_signal_struct_static_init(device, static_device->sigstructs, static_device->sigstruct_num)) != _SHR_E_NONE)
        goto exit;

    /* Figure out how many blocks are in the device */
    for(static_block = static_device->blocks; static_block->name != NULL; static_block++)
        device->block_num++;

    if(device->block_num == 0)
    {
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "No blocks %s Data\n"), SOC_CHIP_STRING(unit)));
        goto exit;
    }

    /* Allocate space for blocks */
    if((cur_pp_block = device->pp_blocks = utilex_alloc(sizeof(pp_block_t) * device->block_num)) == NULL)
    {
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "No memory for %s Blocks Data\n"), SOC_CHIP_STRING(unit)));
        goto exit;
    }
    /*
     * loop through entries
     */
    for(static_block = static_device->blocks; static_block->name != NULL; static_block++)
    {
        debug_signal_t *debug_signal;

        sal_strncpy(cur_pp_block->name, static_block->name, (RHNAME_MAX_SIZE - 1));

        for(static_signal = static_block->signals; static_signal->name != NULL; static_signal++)
            cur_pp_block->signal_num++;
        /*
         * There are no debug signals for this block
         */
        if (cur_pp_block->signal_num == 0)
            continue;

        if((debug_signal = cur_pp_block->debug_signals = utilex_alloc(sizeof(debug_signal_t) * cur_pp_block->signal_num)) == NULL)
        {
            LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "No memory for %s Signal Data\n"), SOC_CHIP_STRING(unit)));
            goto exit;
        }

        /*
         * loop through entries
         */
        for(static_signal = static_block->signals; static_signal->name != NULL; static_signal++)
        {
            int size, i;
            sal_strncpy(debug_signal->attribute, static_signal->name, (RHNAME_MAX_SIZE - 1));
            if(static_signal->from != NULL)
                sal_strncpy(debug_signal->from, static_signal->from, (RHNAME_MAX_SIZE - 1));

            if(static_signal->to != NULL)
                sal_strncpy(debug_signal->to, static_signal->to, (RHNAME_MAX_SIZE - 1));

            debug_signal->size = static_signal->size;
            debug_signal->block_id = static_signal->block_id;
            sal_strncpy(debug_signal->block_n, cur_pp_block->name, (RHNAME_MAX_SIZE - 1));

            if(static_signal->addr_str != NULL)
                debug_signal->range_num = sand_signal_address_parse(static_signal->addr_str, debug_signal->address);
            else {
                LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(0, "Signal %s has no address\n"), debug_signal->attribute));
                continue;
            }

            if (static_signal->expansion)
            {
                if (sand_signal_struct_get(device, static_signal->expansion, debug_signal->from, debug_signal->block_n) != NULL)
                    sal_strncpy(debug_signal->expansion, static_signal->expansion, (RHNAME_MAX_SIZE - 1));
                else
                    LOG_WARN(BSL_LS_SOC_COMMON, (BSL_META_U(0, "No signal expansion:%s\n"), debug_signal->expansion));
            }
            else
            {
                /*
                 * No explicit expansion - look for implicit one
                 */
                if ((sand_signal_struct_get(device, debug_signal->attribute, debug_signal->from, debug_signal->block_n)) != NULL)
                    /*
                     * If there is match - attribute serves as expansion
                     */
                    sal_strcpy(debug_signal->expansion, debug_signal->attribute);
            }

            /*
             * Verify consistency between size and sum of all bits in range
             */
            size = 0;
            for (i = 0; i < debug_signal->range_num; i++)
                size += (debug_signal->address[i].msb + 1 - debug_signal->address[i].lsb);

            if (size != debug_signal->size)
            {
                LOG_WARN(BSL_LS_SOC_COMMON, (BSL_META_U(0, "Correcting size for:%s from:%d to %d\n"), debug_signal->attribute, debug_signal->size, size));
                debug_signal->size = size;
            }

            debug_signal++;
        }
        cur_pp_block++;
    }

exit:
    return device;
}
#else /* (defined(NO_FILEIO) || defined(STATIC_SIGNAL_TEST)) */
/*
 * }
 */

/* (!defined(NO_FILEIO) && !defined(STATIC_SIGNAL_TEST))
 * {
 */
static void
sand_signal_expand_init(
    xml_node cur,
    expansion_t * expansion)
{
    xml_node curOption;
    expansion_option_t *option;

    RHDATA_GET_STR_DEF_NULL(cur, "expansion", expansion->name);

    if (!sal_strcasecmp(expansion->name, "Dynamic"))
    {
        RHDATA_GET_NODE_NUM(expansion->option_num, cur, "option");
        option = expansion->options = utilex_alloc(expansion->option_num * sizeof(expansion_option_t));
        RHDATA_ITERATOR(curOption, cur, "option")
        {
            char trim_str[RHNAME_MAX_SIZE];
            RHDATA_GET_STR_CONT(curOption, "expansion", option->name);
            RHDATA_DELETE(curOption, "expansion");
            RHDATA_GET_STR_DEF_NULL(curOption, "trim", trim_str);
            RHDATA_DELETE(curOption, "trim");

            if(!sal_strcasecmp(trim_str, "ms"))
            {
                option->trim_side = TRIM_MS;
            }
            else if(!sal_strcasecmp(trim_str, "ls"))
            {
                option->trim_side = TRIM_LS;
            }
            else
            {
                option->trim_side = TRIM_NONE;
            }
            dbx_xml_property_get_all(curOption, option->param, "expansion", DSIG_OPTION_PARAM_MAX_NUM);
            option++;
        }
    }
    return;
}

static int
sand_signal_param_read(
    rhlist_t *param_list,
    xml_node nodeTop)
{
    xml_node curSubTop, cur, curSub;
    int res = _SHR_E_NONE;

    /*
     * Read all available parameters with their possible values
     */
    if ((curSubTop = dbx_xml_child_get_first(nodeTop, "signal-params")) == NULL)
    {
        res = _SHR_E_NOT_FOUND;
        goto exit;
    }

    RHDATA_ITERATOR(cur, curSubTop, "signal")
    {
        char name[RHNAME_MAX_SIZE];
        rhhandle_t temp;
        sigparam_t* sigparam;
        sigparam_value_t* sigparam_value;
        int value;

        RHDATA_GET_STR_CONT(cur, "name", name);

        if (utilex_rhlist_entry_add_tail(param_list, name, RHID_TO_BE_GENERATED, &temp) != _SHR_E_NONE)
            break;

        sigparam = temp;

        RHDATA_GET_STR_DEF_NULL(cur, "default", sigparam->default_str);
        RHDATA_GET_STR_DEF_NULL(cur, "dbal", sigparam->dbal_n);
        /*
         * If there is dbal property size if not must
         */
        if(ISEMPTY(sigparam->dbal_n))
        {
            RHDATA_GET_INT_CONT(cur, "size", sigparam->size);
        }
        else
        {
            RHDATA_GET_INT_DEF(cur, "size", sigparam->size, 0);
        }

        if ((sigparam->value_list = utilex_rhlist_create("values", sizeof(sigparam_value_t), 0)) == NULL)
            goto exit;

        RHDATA_ITERATOR(curSub, cur, "entry")
        {
            RHDATA_GET_STR_CONT(curSub, "name", name);
            RHDATA_GET_INT_CONT(curSub, "value", value);
            if (utilex_rhlist_entry_add_tail(sigparam->value_list, name, RHID_TO_BE_GENERATED, &temp) != _SHR_E_NONE)
                break;
            sigparam_value = temp;
            sigparam_value->value = value;
        }
    }

exit:
    return res;
}

static int
sand_signal_struct_read(
    rhlist_t *struct_list,
    xml_node nodeTop)
{
    xml_node curSubTop, cur, curSub;
    int res = _SHR_E_NONE;

    if ((curSubTop = dbx_xml_child_get_first(nodeTop, "signal-structures")) == NULL)
    {
        res = _SHR_E_NOT_FOUND;
        goto exit;
    }

    /*
     * loop through entries
     */
    RHDATA_ITERATOR(cur, curSubTop, "structure")
    {
        char name[RHNAME_MAX_SIZE];
        rhhandle_t temp;
        sigstruct_t *sigstruct;
        sigstruct_field_t *sigstruct_field;
        int size;

        RHDATA_GET_STR_CONT(cur, "name", name);
        RHDATA_GET_INT_CONT(cur, "size", size);

        if (utilex_rhlist_entry_add_tail(struct_list, name, RHID_TO_BE_GENERATED, &temp) != _SHR_E_NONE)
            break;

        sigstruct = temp;

        sigstruct->size = size;
        RHDATA_GET_STR_DEF_NULL(cur, "block", sigstruct->block_n);
        RHDATA_GET_STR_DEF_NULL(cur, "from", sigstruct->from_n);
#ifdef DL_ENABLE
        RHDATA_GET_STR_DEF_NULL(cur, "plugin", name);
        if(!ISEMPTY(name))
        {
            char *error_str;
            /*
             * We have read plugin name from XML, not we assume that callback name if <plugin>_decode
             */
            sal_strcat(name, "_decode");
            /*
             * No we try and record callback address to structure, obviously there may be only one plugin oer struct
             * Below form is requested due to compiler problem with assigning void * to soemthing that is function pointer
             */
            *(void **) (&(sigstruct->expansion_cb)) = dlsym(RTLD_DEFAULT, name);
            /*
             * Verify that the attempt to load address of routine was succesful
             */
            if((error_str = dlerror()) != NULL)
            {
                LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META("Failed to load routine:%s for struct:%s with:%s\n"),
                                                                    name, RHNAME(sigstruct), error_str));
                sigstruct->expansion_cb = NULL;
            }
            else
            {   /*
                 * We succeeded to load plugin callback, get expansion name
                 * If there is none signal name itself will be used
                 */
                RHDATA_GET_STR_DEF_NULL(cur, "expansion", sigstruct->expansion_m.name);
                continue;
            }
        }
#endif

        sand_signal_expand_init(cur, &sigstruct->expansion_m);

        if ((sigstruct->field_list = utilex_rhlist_create("fields", sizeof(sigstruct_field_t), 0)) == NULL)
            goto exit;

        /*
         * loop through entries
         */
        RHDATA_ITERATOR(curSub, cur, "field")
        {
            RHDATA_GET_STR_CONT(curSub, "name", name);

            if (utilex_rhlist_entry_add_tail(sigstruct->field_list, name, RHID_TO_BE_GENERATED, &temp) != _SHR_E_NONE)
                break;

            sigstruct_field = temp;

            RHDATA_GET_STR_CONT(curSub, "bits", name);

            if (sand_signal_range_parse(name, &sigstruct_field->start_bit, &sigstruct_field->end_bit) !=  _SHR_E_NONE)
            {
                LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META("Field:%s.%s, has bad bits range:%s\n"),
                                                                    RHNAME(sigstruct), RHNAME(sigstruct_field), name));
                continue;
            }

            sigstruct_field->size = sigstruct_field->end_bit + 1 - sigstruct_field->start_bit;

            RHDATA_GET_STR_DEF_NULL(curSub, "condition", sigstruct_field->cond_attribute);
            RHDATA_GET_STR_DEF_NULL(curSub, "resolution", sigstruct_field->resolution);

            sand_signal_expand_init(curSub, &sigstruct_field->expansion_m);
        }
    }

exit:
    return res;
}

static int
sand_signal_struct_init(
    device_t * device)
{
    xml_node curTop, curSubTop, curInclude;

    int res = _SHR_E_NONE;

    char filename[RHSTRING_MAX_SIZE];

    if (device->struct_list != NULL)
        /*
         * ALready initialized
         */
        return res;

    /* First look into device specific expansion file */
    if ((curTop = dbx_file_get_xml_top(RHNAME(device), "PP.xml", "top", 0)) == NULL)
    {
        res = _SHR_E_NOT_FOUND;
        goto exit;
    }

    RHDATA_GET_STR_STOP(curTop, "expansion", filename);
    dbx_xml_top_close(curTop);

    if ((curTop = dbx_file_get_xml_top(RHNAME(device), filename, "top", 0)) == NULL)
    {
        res = _SHR_E_NOT_FOUND;
        goto exit;
    }

    if ((device->param_list = utilex_rhlist_create("param_list", sizeof(sigparam_t), 0)) == NULL)
        goto exit;
    sand_signal_param_read(device->param_list, curTop);

    if ((device->struct_list = utilex_rhlist_create("struct_list", sizeof(sigstruct_t), 0)) == NULL)
        goto exit;
    sand_signal_struct_read(device->struct_list, curTop);

    dbx_xml_top_close(curTop);

    /* Done with device specific - no check for universal structure files */
    if ((curTop = dbx_file_get_xml_top(NULL, "DNX-Devices.xml", "top", 0)) == NULL)
    {
        res = _SHR_E_NOT_FOUND;
        goto exit;
    }
    if ((curSubTop = dbx_xml_child_get_first(curTop, "parsing-objects")) == NULL)
    {
        goto exit;
    }

    RHDATA_ITERATOR(curInclude, curSubTop, "include")
    {
        char type[RHNAME_MAX_SIZE];
        RHDATA_GET_STR_DEF_NULL(curInclude, "type", type);
        if(!sal_strcasecmp(type, "structs"))
        {
            xml_node curFile;
            RHDATA_GET_STR_CONT(curInclude, "file", filename);
            if ((curFile = dbx_file_get_xml_top(NULL, filename, "top", 0)) == NULL)
                continue;

            sand_signal_param_read(device->param_list, curFile);
            sand_signal_struct_read(device->struct_list, curFile);
            dbx_xml_top_close(curFile);
        }
    }

    dbx_xml_top_close(curTop);

    sand_signal_struct_expand_init(device);

exit:
    return res;
}

static int
sand_signal_struct_deinit(
    device_t * device)
{
    sigstruct_t *sigstruct;
    sigparam_t* sigparam;

    RHITERATOR(sigstruct, device->struct_list)
    {
        sigstruct_field_t *sigstruct_field;
        RHITERATOR(sigstruct_field, sigstruct->field_list)
        {
            utilex_free(sigstruct_field->expansion_m.options);
        }
        if(sigstruct->field_list)
            utilex_rhlist_free_all(sigstruct->field_list);
    }
    utilex_rhlist_free_all(device->struct_list);
    device->struct_list = NULL;

    RHITERATOR(sigparam, device->param_list)
    {
        utilex_rhlist_free_all(sigparam->value_list);
    }
    utilex_rhlist_free_all(device->param_list);
    device->param_list = NULL;

    return _SHR_E_NONE;
}

shr_error_e
sand_qual_signal_init(
    char *db_name,
    char *qual_signals_db,
    pp_stage_t * pp_stage)
{
    shr_error_e res = _SHR_E_NONE;
    void *curTop, *cur;
    internal_signal_t *cur_internal_signal;

    if ((curTop = dbx_file_get_xml_top(db_name, qual_signals_db, "SignalInfo", 0)) == NULL)
    {
        res = _SHR_E_NOT_FOUND;
        goto exit;
    }

    RHDATA_GET_INT_STOP(curTop, "size0", pp_stage->buffer0_size);
    RHDATA_GET_INT_DEF(curTop,  "size1", pp_stage->buffer1_size, 0);
    RHDATA_GET_NODE_NUM(pp_stage->number, curTop, "signal")

    if (pp_stage->number == 0)
    {
        res = _SHR_E_NOT_FOUND;
        goto exit;
    }

    cur_internal_signal = pp_stage->signals = utilex_alloc(sizeof(internal_signal_t) * pp_stage->number);

    /*
     * loop through entries
     */
    RHDATA_ITERATOR(cur, curTop, "signal")
    {
        dbx_xml_child_get_content_str(cur, "name",    cur_internal_signal->name, RHNAME_MAX_SIZE);
        dbx_xml_child_get_content_int(cur, "offset",  &cur_internal_signal->offset);
        dbx_xml_child_get_content_int(cur, "size",    &cur_internal_signal->size);
        dbx_xml_child_get_content_int(cur, "buffer",  &cur_internal_signal->buffer);
        dbx_xml_child_get_content_str(cur, "verilog", cur_internal_signal->hw, RHSTRING_MAX_SIZE);
        cur_internal_signal++;
    }

    dbx_xml_top_close(curTop);

exit:
    return res;
}

shr_error_e
sand_signal_init(
    int        unit,
    device_t * device)
{
    shr_error_e rv = _SHR_E_NONE;
    void *curTop, *curSubTop, *curBlock;
    debug_signal_t *debug_signal;
    int i, j;

    pp_block_t *cur_pp_block;

    if (device == NULL)
    {
        rv = _SHR_E_NOT_FOUND;
        goto exit;
    }

    if (device->pp_blocks != NULL)
    { /* Already initialized, success */
        goto exit;
    }

    if((rv = sand_signal_struct_init(device)) != _SHR_E_NONE)
        goto exit;;

    if ((curTop = dbx_file_get_xml_top(RHNAME(device), "PP.xml", "top", 0)) == NULL)
    {
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META("No Pipeline Scheme found\n")));
        rv = _SHR_E_NOT_FOUND;
        goto exit;
    }

    if ((curSubTop = dbx_xml_child_get_first(curTop, "block-list")) == NULL)
    {
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META("No block-list in Pipeline Scheme\n")));
        rv = _SHR_E_INTERNAL;
        goto exit;
    }

    RHDATA_GET_NODE_NUM(device->block_num, curSubTop, "block");

    if((cur_pp_block = device->pp_blocks = utilex_alloc(sizeof(pp_block_t) * device->block_num)) == NULL)
    {
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "No memory for %s Blocks Data\n"), SOC_CHIP_STRING(unit)));
        rv = _SHR_E_MEMORY;
        goto exit;
    }
    /*
     * loop through entries
     */
    RHDATA_ITERATOR(curBlock, curSubTop, "block")
    {
        /*
         * Verify that we are inside the limitations MAX_NUM
         */
        RHDATA_GET_STR_CONT(curBlock, "name", cur_pp_block->name);
        RHDATA_GET_STR_DEF_NULL(curBlock, "debug-signals", cur_pp_block->debug_signals_n);

        /*
         * In case of c-model obtain the list of stages and add here
         */
#ifdef CMODEL_SERVER_MODE
        if((rv = sand_cmodel_get_stages(unit, cur_pp_block)) != _SHR_E_NONE)
            goto exit;
#else
        {
            void *curSignalTop, *curSignal, *cur, *curStage;
            int size;
            char full_address[DSIG_ADDRESS_MAX_SIZE];
            pp_stage_t *cur_pp_stage;
            char temp[RHNAME_MAX_SIZE];

            RHDATA_GET_NODE_NUM(cur_pp_block->stage_num, curBlock, "stage")
            if(cur_pp_block->stage_num == 0)
                continue;

            if((cur_pp_block->stages = utilex_alloc(sizeof(pp_stage_t) * cur_pp_block->stage_num)) == NULL)
            {
                LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META("No memory for stages\n")));
                rv = _SHR_E_MEMORY;
                goto exit;
            }
            cur_pp_stage = cur_pp_block->stages;
            /*
             * loop through entries
             */
            RHDATA_ITERATOR(curStage, curBlock, "stage")
            {
                RHDATA_GET_STR_CONT(curStage,     "name",             cur_pp_stage->name);
                RHDATA_GET_STR_DEF_NULL(curStage, "programmable",     cur_pp_stage->programmable);
                RHDATA_GET_STR_DEF_NULL(curStage, "internal-signals", temp);
                if (!ISEMPTY(temp))
                {
                    sand_qual_signal_init(RHNAME(device), temp, cur_pp_stage);
                }
                cur_pp_stage++;
            }

            if (ISEMPTY(cur_pp_block->debug_signals_n))     /* No debug signals for this block */
                continue;

            if ((curSignalTop =
                 dbx_file_get_xml_top(RHNAME(device), cur_pp_block->debug_signals_n, "SignalInfo", 0)) == NULL)
            {
                LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META("No Signal Info found in:%s\n"), cur_pp_block->debug_signals_n));
                rv = _SHR_E_NOT_FOUND;
                goto exit;
            }

            RHDATA_GET_NODE_NUM(cur_pp_block->signal_num, curSignalTop, "Signal");
            /*
             * It is like there are no debug signals for this block
             */
            if (cur_pp_block->signal_num == 0)
                continue;

            debug_signal = utilex_alloc(sizeof(debug_signal_t) * cur_pp_block->signal_num);

            cur_pp_block->debug_signals = debug_signal;

            /*
             * loop through entries
             */
            RHDATA_ITERATOR(curSignal, curSignalTop, "Signal")
            {
                dbx_xml_child_get_content_str(curSignal, "Name", debug_signal->hw, RHSTRING_MAX_SIZE);
                dbx_xml_child_get_content_str(curSignal, "From", debug_signal->from, RHNAME_MAX_SIZE);
                dbx_xml_child_get_content_str(curSignal, "To", debug_signal->to, RHNAME_MAX_SIZE);
                strcpy(debug_signal->block_n, cur_pp_block->name);
                dbx_xml_child_get_content_int(curSignal, "Size", &debug_signal->size);
                dbx_xml_child_get_content_int(curSignal, "BlockID", &debug_signal->block_id);
                dbx_xml_child_get_content_str(curSignal, "Changeable", temp, RHNAME_MAX_SIZE);
                dbx_xml_child_get_content_str(curSignal, "Attribute", debug_signal->attribute, RHNAME_MAX_SIZE);
                dbx_xml_child_get_content_int(curSignal, "Perm", &debug_signal->perm);
                cur = dbx_xml_child_get_content_str(curSignal, "Condition", debug_signal->cond_attribute, RHNAME_MAX_SIZE);
                if ((cur != NULL) && !ISEMPTY(debug_signal->cond_attribute))
                {
                    RHDATA_GET_INT_DEF(cur, "Value", debug_signal->cond_value, 1);
                }

                debug_signal->changeable = (sal_strcasecmp(temp, "Yes") ? 0 : 1);

                dbx_xml_child_get_content_str(curSignal, "Expansion", debug_signal->expansion, RHNAME_MAX_SIZE);
                /*
                 * No explicit expansion - look for implicit one
                 */
                if (!ISEMPTY(debug_signal->expansion))
                {
                    if ((sand_signal_struct_get(device, debug_signal->expansion, debug_signal->from, debug_signal->block_n)) == NULL)
                    {
                        LOG_INFO(BSL_LS_SOC_COMMON, (BSL_META("No signal expansion:%s\n"), debug_signal->expansion));
                        SET_EMPTY(debug_signal->expansion);
                    }
                }
                else
                {
                    if ((sand_signal_struct_get(device, debug_signal->attribute, debug_signal->from, debug_signal->block_n)) != NULL)
                        /*
                         * If there is match - attribute serves as expansion
                         */
                        strcpy(debug_signal->expansion, debug_signal->attribute);
                }

                dbx_xml_child_get_content_str(curSignal, "Resolution", debug_signal->resolution, RHNAME_MAX_SIZE);
                if (!ISEMPTY(debug_signal->resolution))
                {
                    if ((sand_signal_resolve_get(device, debug_signal->resolution)) == NULL)
                    {
                        LOG_INFO(BSL_LS_SOC_COMMON, (BSL_META("Signal resolution:%s does not exist\n"),  debug_signal->resolution));
                        SET_EMPTY(debug_signal->resolution);
                    }
                }
                else
                {
                    if ((sand_signal_resolve_get(device, debug_signal->attribute)) != NULL)
                        /*
                         * If there is match - attribute serves as resolution param name
                         */
                        strcpy(debug_signal->resolution, debug_signal->attribute);
                }

                cur = dbx_xml_child_get_content_str(curSignal, "Double", temp, RHNAME_MAX_SIZE);
                if ((cur != NULL) && !ISEMPTY(temp))
                    debug_signal->double_flag = 1;

                dbx_xml_child_get_content_str(curSignal, "Address", full_address, DSIG_ADDRESS_MAX_SIZE);
                debug_signal->range_num = sand_signal_address_parse(full_address, debug_signal->address);
                /*
                 * Verify consistency between size and sum of all bits in range
                 */
                size = 0;
                for (i = 0; i < debug_signal->range_num; i++)
                    size += (debug_signal->address[i].msb + 1 - debug_signal->address[i].lsb);

                if (size != debug_signal->size)
                {
                    cli_out("Correcting size for:%s from:%d to %d\n", debug_signal->attribute, debug_signal->size, size);
                    debug_signal->size = size;
                }

                debug_signal++;
            }

            dbx_xml_top_close(curSignalTop);
        }
#endif /* CMODEL_SERVER_MODE */
        cur_pp_block++;
    }
    /*
     * Now we need to extract condition attribute
     */
    for (i = 0; i < device->block_num; i++)
    {
        cur_pp_block = &device->pp_blocks[i];
        debug_signal = cur_pp_block->debug_signals;
        for (j = 0; j < cur_pp_block->signal_num; j++)
        {
            if (!ISEMPTY(debug_signal->cond_attribute))
            {
                if(sand_signal_handle_get(device, debug_signal->block_n,
                                             debug_signal->from, debug_signal->to,
                                             debug_signal->cond_attribute, &debug_signal->cond_signal) == 0)
                {
                    cli_out("Condition Attribute:%s does not exist for:%s:%s -> %s\n",
                            debug_signal->cond_attribute, debug_signal->block_n, debug_signal->from, debug_signal->to);
                }
            }
            debug_signal++;
        }
    }

    dbx_xml_top_close(curTop);
exit:
    return rv;
}

shr_error_e
sand_signal_deinit(
    int        unit,
    device_t * device)
{
    shr_error_e rv = _SHR_E_NONE;
    int i_ind;

    if (device == NULL)
    {
        rv = _SHR_E_NOT_FOUND;
        goto exit;
    }

    if (device->pp_blocks == NULL)
    { /* Not initialized, leave peacefully */
        goto exit;
    }

    if((rv = sand_signal_struct_deinit(device)) != _SHR_E_NONE)
        goto exit;;

    for(i_ind = 0; i_ind < device->block_num; i_ind++)
    {
        utilex_free(device->pp_blocks[i_ind].stages);
        utilex_free(device->pp_blocks[i_ind].debug_signals);
    }
    utilex_free(device->pp_blocks);

    device->pp_blocks = NULL;

exit:
    return rv;
}
/*
 * }
 */
#endif /*else of (!defined(NO_FILEIO) && !defined(STATIC_SIGNAL_TEST)) */

shr_error_e
sand_signal_reread(
    int unit,
    device_t *device)
{
    SHR_FUNC_INIT_VARS(unit);

#if !defined(NO_FILEIO) && !defined(STATIC_SIGNAL_TEST)
    /* DEInitialize Signals Data */
    SHR_IF_ERR_EXIT(sand_signal_deinit(unit, device));

    /* Initialize Access Objects */
    SHR_IF_ERR_EXIT(sand_signal_init(unit, device));
exit:
#endif /* (!defined(NO_FILEIO) && !defined(STATIC_SIGNAL_TEST)) */
    SHR_FUNC_EXIT;
}

