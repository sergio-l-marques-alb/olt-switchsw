/** \File: sand_signals.c
 * Signal Driver - init from XML DB, access and filtering
 */
/*
 * $Id: Exp $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#if !defined(__KERNEL__)
#include <stdlib.h>
#endif

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
#include <shared/utilex/utilex_framework.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
#include <shared/shrextend/shrextend_debug.h>

#include <soc/sand/sand_signals.h>
#include <soc/sand/sand_aux_access.h>
#include <soc/sand/sand_pparse.h>

#include <bcm/types.h>

#ifdef BCM_PETRA_SUPPORT
#include <soc/dpp/dpp_config_defs.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_diag.h>
#endif

#ifdef BCM_DNX_SUPPORT
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/pp_stage.h>
#include <soc/dnx/dnx_visibility.h>
#include <soc/dnx/dbal/dbal_string_mgmt.h>
#include <soc/dnx/arr/arr.h>
#endif

#define BSL_LOG_MODULE BSL_LS_SOC_COMMON

static int
sand_signal_handle_get(
    int unit,
    char *block,
    char *from,
    char *to,
    char *name,
    debug_signal_t ** signal_p);

sigstruct_t *
sand_signal_struct_get(
    device_t*   device,
    char*       signal_n,
    char*       from_n,
    char*       to_n,
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
        if (!ISEMPTY(sigstruct->from_n) && !ISEMPTY(from_n) && sal_strcasecmp(sigstruct->from_n, from_n))
            continue;
        /*
          * If there is specific "to" stage name, only signal from this stage may be matched to it
          */
         if (!ISEMPTY(sigstruct->to_n) && !ISEMPTY(to_n) && sal_strcasecmp(sigstruct->to_n, to_n))
             continue;
        /*
         * If there is specific block name, only signal belonging to this block may be matched to it
         */

        if (!ISEMPTY(sigstruct->block_n) && !ISEMPTY(block_n) && sal_strcasecmp(sigstruct->block_n, block_n))
            continue;

        if (!sal_strcasecmp(RHNAME(sigstruct), signal_n))
        {
            RHUSER_ADD(sigstruct);
            return sigstruct;
        }
    }

    return NULL;
}

sigparam_t *
sand_signal_resolve_get(
    device_t * device,
    char*       signal_n,
    char*       from_n,
    char*       to_n,
    char*       block_n)
{
    sigparam_t *sigparam;

    if (device->param_list == NULL)
    {   /* Not initialized yet */
        LOG_WARN(BSL_LS_SOC_COMMON, (BSL_META("Signal Struct DB was not initialized\n")));
        return NULL;
    }

    RHITERATOR(sigparam, device->param_list)
    {
       /*
         * If there is specific "from" stage name, only signal from this stage may be matched to it
         */
        if (!ISEMPTY(sigparam->from_n) && !ISEMPTY(from_n) && sal_strcasecmp(sigparam->from_n, from_n))
            continue;
        /*
          * If there is specific "to" stage name, only signal from this stage may be matched to it
          */
         if (!ISEMPTY(sigparam->to_n) && !ISEMPTY(to_n) && sal_strcasecmp(sigparam->to_n, to_n))
             continue;
        /*
         * If there is specific block name, only signal belonging to this block may be matched to it
         */

        if (!ISEMPTY(sigparam->block_n) && !ISEMPTY(block_n) && sal_strcasecmp(sigparam->block_n, block_n))
            continue;

        if (!sal_strcasecmp(RHNAME(sigparam), signal_n))
        {
            RHUSER_ADD(sigparam);
            return sigparam;
        }
    }

    return NULL;
}

static int
sand_signal_param_get(
    int unit,
    device_t * device,
    char *signal_n,
    char *from_n,
    char *to_n,
    char *block_n,
    uint32 value,
    char *value_n,
    int show_value)
{
    int res = _SHR_E_NOT_FOUND;
    sigparam_t *cur_sigparam;
    sigparam_value_t *cur_sigparam_value;

    if ((cur_sigparam = sand_signal_resolve_get(device, signal_n, from_n, to_n, block_n)) == NULL)
    {
        SHR_EXIT();
    }

#ifdef BCM_DNX_SUPPORT
    /*
     * Use DBAL for signal resolution, SignalStructure.xml should contain mapping between signal DBAL name
     */
    if(SOC_IS_DNX(unit) && !ISEMPTY(cur_sigparam->dbal_n))
    {
        if((res = dnx_dbal_fields_string_form_hw_value_get(unit, cur_sigparam->dbal_n, value, value_n)) != _SHR_E_NONE)
        {
            LOG_WARN(BSL_LOG_MODULE, (BSL_META_U(unit, "DBAL resolution failed for %s=%d\n"), cur_sigparam->dbal_n, value));
        }
        else
        {
            if (show_value == 1)
            {
                char digital_value_str[RHNAME_MAX_SIZE];
                sal_snprintf(digital_value_str, RHNAME_MAX_SIZE, "(0x%x)", value);
                sal_strncat(value_n, digital_value_str, DSIG_MAX_SIZE_STR - sal_strlen(value_n));
            }
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
                sal_snprintf(value_n, DSIG_MAX_SIZE_STR - 1, "%s(0x%x)", RHNAME(cur_sigparam_value), value);
            else
                sal_snprintf(value_n, DSIG_MAX_SIZE_STR - 1, "%s", RHNAME(cur_sigparam_value));
            res = _SHR_E_NONE;
            break;
        }
    }

    if ((res != _SHR_E_NONE) && !ISEMPTY(cur_sigparam->default_str))
    {   /* Copy default param name if not found */
        sal_snprintf(value_n, DSIG_MAX_SIZE_STR - 1, "%s(%d)", cur_sigparam->default_str, value);
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

int
sand_signal_range_parse(
    char *bit_range,
    int *startbit_p,
    int *endbit_p,
    int order,
    int size)
{
    int startbit = -1, endbit = -1;
    if (strstr(bit_range, ":") == NULL)
    {
        sscanf(bit_range, "%d", &endbit);
        startbit = endbit;
    }
    else
        sscanf(bit_range, "%d:%d", &endbit, &startbit);

    if ((startbit >= DSIG_MAX_SIZE_BITS) || (endbit >= DSIG_MAX_SIZE_BITS) || (startbit < 0) || (endbit < 0))
    {
        return _SHR_E_INTERNAL;
    }

    if(order == PRINT_LITTLE_ENDIAN)
    {
        *startbit_p = startbit;
        *endbit_p = endbit;
    }
    else
    {
        *endbit_p   = size - endbit   - 1;
        *startbit_p = size - startbit - 1;
    }

    if(*startbit_p > *endbit_p)
    {
        return _SHR_E_INTERNAL;
    }
    return _SHR_E_NONE;
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

static shr_error_e
sand_signal_get_value(
    int unit,
    int core_id,
    debug_signal_t * debug_signal,
    uint32 * value)
{
    int uint_num;

    SHR_FUNC_INIT_VARS(unit);

    if ((uint_num = BITS2WORDS(debug_signal->size)) > DSIG_MAX_SIZE_UINT32)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Signal:%s size:%d is over the max:%d\n",
                                            debug_signal->attribute, debug_signal->size, DSIG_FIELD_MAX_SIZE_BITES);
    }
    /*
     * Zero all bytes before actual value set
     */
    sal_memset(value, 0, sizeof(uint32) * uint_num);
#ifdef ADAPTER_SERVER_MODE
    sal_memcpy(value, debug_signal->adapter_value, sizeof(uint32) * uint_num);
#else
#ifdef BCM_PETRA_SUPPORT

    if(SOC_IS_DPP(unit))
    {
        int j;
        ARAD_PP_DIAG_REG_FIELD debug_field;
        signal_address_t *address;
        uint32 range_val[ARAD_PP_DIAG_DBG_VAL_LEN];
        int range_size;
        int last_pos;

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
    }
#endif /* BCM_PETRA_SUPPORT */
#ifdef BCM_DNX_SUPPORT
    if(SOC_IS_DNX(unit))
    {
        uint32 *source_value;
        if(debug_signal->block_id == DBAL_ENUM_FVAL_PP_ASIC_BLOCK_TABLE)
        {
            /* Do nothing - no value required for this type of signals - will be fetched from DBAL */
            SHR_EXIT();
        }
        else if(debug_signal->block_id == DBAL_ENUM_FVAL_PP_ASIC_BLOCK_REGISTER)
        {
            uint32 reg_val[MAX_DEBUG_BUS_SIZE_WORDS] = {0};
            source_value = reg_val;
            /*
             * Signal is associated with debug register field and source_value will point on it
             */
            SHR_SET_CURRENT_ERR(dnx_debug_reg_read(unit, core_id, debug_signal->hw, debug_signal->size, reg_val));
        }
        else
        {
            /*
             * Fetch cache pointer to relevant line
             */
            SHR_SET_CURRENT_ERR(dnx_debug_mem_read(unit, core_id, debug_signal->block_id, debug_signal->address[0].low,
                                                                                         &source_value));
        }
        if(SHR_FUNC_VAL_IS(_SHR_E_NONE))
        {
            /*
             * Select requested bits
             */
            SHR_BITCOPY_RANGE(value, 0, source_value, debug_signal->address[0].lsb, debug_signal->size);
        }
    }
#endif /* BCM_DNX_SUPPORT */
#endif
exit:
    SHR_FUNC_EXIT;
}

shr_error_e static
sand_signal_mac_str_to_value(
        char *str_value,
        uint32 *value)
{
    int i_tok;
    char **tokens;
    uint8 mac_address[6];
    uint32 realtokens = 0, maxtokens = 7;

    SHR_FUNC_INIT_VARS(NO_UNIT);

    tokens = utilex_str_split(str_value, ":", maxtokens, &realtokens);
    if ((tokens == NULL) || (realtokens != 6))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "String:'%s' is not a legal mac address\n", str_value);
    }

    for(i_tok = 0; i_tok < realtokens; i_tok++)
    {
        if(sal_strlen(tokens[i_tok]) == 2)
        {
            mac_address[i_tok] = utilex_char_to_num(tokens[i_tok][0]) * 16 + utilex_char_to_num(tokens[i_tok][1]);
        }
        else if(sal_strlen(tokens[i_tok]) == 1)
        {
            mac_address[i_tok] = utilex_char_to_num(tokens[i_tok][0]);
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Token:'%s' from:'%s' is not a valid string for mac address\n", tokens[i_tok], str_value);
        }
    }

    SHR_IF_ERR_EXIT(utilex_pp_mac_address_struct_to_long(mac_address, value));

exit:
    utilex_str_split_free(tokens, realtokens);
    SHR_FUNC_EXIT;
}

static shr_error_e
sand_signal_ip6_str_to_value(char *str, uint32 *value)
{
    char *ts;
    int i, x;
    char str_buf[50];
    char *tmps = &str_buf[0];
    uint8 ip6_addr[UTILEX_PP_IPV6_ADDRESS_NOF_U8] = { 0 };

    SHR_FUNC_INIT_VARS(NO_UNIT);

    if (sal_strchr(str, ':'))
    {               /* colon notation */
        sal_strncpy(&tmps[2], str, 47);
        tmps[49] = '\0';
        for (i = 0; i < 8; i++)
        {
            tmps[0] = '0';
            tmps[1] = 'x';
            x = sal_ctoi(tmps, &ts);
            if ((x > 0xffff) || (x < 0))
            {
                SHR_EXIT_WITH_LOG(_SHR_E_PARAM, "IPv6 address:%s has invalid field:%s%s\n", str, tmps, EMPTY);
            }

            ip6_addr[2 * i] = (x >> 8) & 0xFF;
            ip6_addr[2 * i + 1] = x & 0xFF;
            if (*ts != ':')
            {   /* End of string */
                break;
            }
            tmps = ts - 1;
        }
        if (((i != 7) || (*ts != '\0')))
        {
            SHR_EXIT_WITH_LOG(_SHR_E_PARAM, "Invalid IPv6 address:%s%s%s\n", str, EMPTY, EMPTY);
        }
    }
    else
    {
        SHR_EXIT_WITH_LOG(_SHR_E_PARAM, "IPv6 address:%s has invalid field %s%s\n", str, EMPTY, EMPTY);
    }

    SHR_IF_ERR_EXIT(utilex_pp_ipv6_address_struct_to_long(ip6_addr, value));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
sand_signal_ip4_str_to_value(
        char *str_value,
        uint8 *ip4_address)
{
    int i_tok;
    char **tokens;
    uint32 byte_value;
    uint32 realtokens = 0, maxtokens = 5;

    SHR_FUNC_INIT_VARS(NO_UNIT);

    tokens = utilex_str_split(str_value, ".", maxtokens, &realtokens);
    if ((tokens == NULL) || (realtokens != 4))
    {
        SHR_EXIT_WITH_LOG(_SHR_E_PARAM, "String is not a legal IP  address:%s%s%s\n", str_value, EMPTY, EMPTY);
    }

    for(i_tok = 0; i_tok < realtokens; i_tok++)
    {
        SHR_IF_ERR_EXIT(utilex_str_stoul(tokens[i_tok], &byte_value));
        if(byte_value > 255)
        {
            SHR_EXIT_WITH_LOG(_SHR_E_PARAM, "IP address:%s has invalid field %s%s\n", str_value, EMPTY, EMPTY);
        }
#ifdef BE_HOST
        ip4_address[i_tok] = (uint8)byte_value;
#else
        ip4_address[3 - i_tok] = (uint8)byte_value;
#endif
    }

exit:
    utilex_str_split_free(tokens, realtokens);
    SHR_FUNC_EXIT;
}

static shr_error_e
sand_signal_str_to_value(
        int unit,
        sal_field_type_e type,
        char *str_value,
        int size,
        uint32 *value)
{
    SHR_FUNC_INIT_VARS(unit);

    switch(type)
    {
        case SAL_FIELD_TYPE_ARRAY32:
            SHR_SET_CURRENT_ERR(utilex_str_long_stoul(str_value, value, UTILEX_DIV_ROUND_UP(size, 32)));
            break;
        case SAL_FIELD_TYPE_INT32:
        case SAL_FIELD_TYPE_UINT32:
            SHR_SET_CURRENT_ERR(utilex_str_stoul(str_value, value));
            break;
        case SAL_FIELD_TYPE_MAC:
            SHR_IF_ERR_EXIT(sand_signal_mac_str_to_value(str_value, value));
            break;
        case SAL_FIELD_TYPE_IP4:
            SHR_IF_ERR_EXIT(sand_signal_ip4_str_to_value(str_value, (uint8 *)value));
            break;
        case SAL_FIELD_TYPE_IP6:
            SHR_IF_ERR_EXIT(sand_signal_ip6_str_to_value(str_value, value));
            break;
        case SAL_FIELD_TYPE_BOOL:
            break;
        case SAL_FIELD_TYPE_ENUM:
            break;
        case SAL_FIELD_TYPE_STR:
            SHR_EXIT_WITH_LOG(_SHR_E_PARAM, "Unsupported type for extraction:\"%s\"%s%s\n", sal_field_type_str(type), EMPTY, EMPTY);
            break;
        default:
            break;
    }
exit:
    SHR_FUNC_EXIT;
}

void
sand_signal_value_to_str(
    int unit,
    sal_field_type_e type,
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

    dest_byte_index = 0;
    if(type == SAL_FIELD_TYPE_MAC)
    {
        uint8 mac_addr[6];

        utilex_pp_mac_address_long_to_struct(org_source, mac_addr);

        sal_snprintf(dest, DSIG_MAX_SIZE_STR - 1, "%02X:%02X:%02X:%02X:%02X:%02X", mac_addr[0], mac_addr[1], mac_addr[2],
                                                                               mac_addr[3], mac_addr[4], mac_addr[5]);
    }
    else if(type == SAL_FIELD_TYPE_IP4)
    {
        uint32 ip_addr = *org_source;
        sal_snprintf(dest, DSIG_MAX_SIZE_STR - 1, "%d.%d.%d.%d", (ip_addr >> 24) & 0xff, (ip_addr >> 16) & 0xff,
                                                                 (ip_addr >> 8) & 0xff, ip_addr & 0xff);
    }
    if(type == SAL_FIELD_TYPE_IP6)
    {
        int iter, char_count;
        uint8 u8_val[UTILEX_PP_IPV6_ADDRESS_NOF_U8] = { 0 };

        utilex_pp_ipv6_address_long_to_struct(org_source, u8_val);

        for (iter = 0; iter < UTILEX_PP_IPV6_ADDRESS_NOF_U8;)
        {
            char_count = sal_snprintf(dest, DSIG_MAX_SIZE_STR - 1, "%02x", u8_val[iter++]);
            dest += char_count;
            char_count = sal_snprintf(dest, DSIG_MAX_SIZE_STR - 1, "%02x", u8_val[iter++]);
            dest += char_count;
            if(iter != UTILEX_PP_IPV6_ADDRESS_NOF_U8)
            {
                char_count = sal_snprintf(dest, DSIG_MAX_SIZE_STR, ":");
                dest += char_count;
            }
        }
    }
#ifdef BE_HOST
    else
    {
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
                    sal_snprintf(&dest[2 * dest_byte_index], 3, "%02x", source[source_byte_index]);
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
                    sal_snprintf(&dest[2 * dest_byte_index], 3, "%02x", source[source_byte_index]);
                    dest_byte_index++;
                }
            }
        }
    }
#else
    else
    {
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
                    sal_snprintf(&dest[2 * dest_byte_index], 3, "%02x", source[source_byte_index]);
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
                    sal_snprintf(&dest[2 * dest_byte_index], 3, "%02x", source[source_byte_index]);
                    dest_byte_index++;
                }
            }
        }
    }
#endif
}

static void
sand_signal_value_to_str_full(
    int unit,
    device_t * device,
    sal_field_type_e type,
    char *resolution_n,
    char *from_n,
    char *to_n,
    char *block_n,
    int flags,
    uint32 * org_source,
    char *dest,
    int bit_size,
    int byte_order)
{
    int value_show;

    if(flags & SIGNALS_MATCH_RESOLVE_ONLY)
    {
        value_show = FALSE;
    }
    else
    {
        value_show = TRUE;
    }
    sal_memset(dest, 0, DSIG_MAX_SIZE_STR);
    /*
     * If there is resolution and we want to do resolution and it was successful with non-empty string,
     * we may leave, otherwise continue to print the digital value
     */
    if (!ISEMPTY(resolution_n) && !(flags & SIGNALS_MATCH_NORESOLVE)
        && (sand_signal_param_get(unit, device, resolution_n, from_n, to_n, block_n, *org_source, dest, value_show) == _SHR_E_NONE)
            && (dest[0] != 0))
    {
        return;
    }

    sand_signal_value_to_str(unit, type, org_source, dest, bit_size, byte_order);
}

static void
sand_signal_description(
    match_t * match_p,
    char *desc_str)
{
    if (!ISEMPTY(match_p->block))
        sal_snprintf(desc_str, RHSTRING_MAX_SIZE - 1, "block:%s", match_p->block);
    if (!ISEMPTY(match_p->from))
        sal_snprintf(desc_str + sal_strlen(desc_str), RHSTRING_MAX_SIZE - 1 - sal_strlen(desc_str), " from:%s", match_p->from);
    if (!ISEMPTY(match_p->to))
        sal_snprintf(desc_str + sal_strlen(desc_str), RHSTRING_MAX_SIZE - 1 - sal_strlen(desc_str), " to:%s", match_p->to);
    if (!ISEMPTY(match_p->name))
        sal_snprintf(desc_str + sal_strlen(desc_str), RHSTRING_MAX_SIZE - 1 - sal_strlen(desc_str), " name:%s", match_p->name);
    return;
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
    char *from_n,
    char *to_n,
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

    if (!ISEMPTY(sigstruct_n) &&
                            (sigstruct = sand_signal_struct_get(device, sigstruct_n, from_n, to_n, block_n)) != NULL)
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
                    if(field_value != NULL)
                    {
                        sal_memcpy(field_value, cur_field_value, DSIG_MAX_SIZE_BYTES);
                    }
                    if (field_str != NULL)
                    {
                        res = sand_signal_param_get(unit, device, sigstruct_field->resolution, from_n, to_n, block_n,
                                                                                        *cur_field_value, field_str, 0);
                        if (res != _SHR_E_NONE)
                            sal_snprintf(field_str, RHNAME_MAX_SIZE - 1,"%u", *cur_field_value);
                    }
                    res = _SHR_E_NONE;
                }
                else
                {
                    res = sand_signal_field_get(unit, device, sigstruct_field->expansion_m.name,
                                            from_n, to_n, block_n, cur_field_value, tokens[1], field_str, field_value);
                }
                break;
            }
        }
    }

    utilex_str_split_free(tokens, realtokens);

    return res;
}

static shr_error_e
sand_signal_option_get(
    int unit,
    int core,
    device_t *device,
    char *sigstruct_n,
    char *from_n,
    char *to_n,
    char *block_n,
    uint32 *base_value,
    char *param_name,
    char *value_str)
{
    int res = _SHR_E_NONE;
    debug_signal_t *debug_signal = NULL;
    /*
     * Check if the option condition name is global signal
     */
    /*
     * Try to split the name and check the first token for
     */
    char **tokens;
    uint32 realtokens = 0;
    tokens = utilex_str_split(param_name, ".", 2, &realtokens);
    if (tokens == NULL)
    {
        goto exit;
    }

    if(sand_signal_handle_get(unit, block_n, from_n, NULL, tokens[0], &debug_signal) == 0)
    {
        /*
         * No such signal, check if it is internal field in the structure
         */
        res = sand_signal_field_get(unit, device, sigstruct_n, from_n, to_n, block_n,
                                                                            base_value, param_name, value_str, NULL);
    }
    else
    {
        uint32 cond_value[DSIG_MAX_SIZE_UINT32];
        /*
         * Debug Signal found, get it value
         */
        sand_signal_get_value(unit, core, debug_signal, cond_value);
        if(realtokens == 1)
        {
            sal_snprintf(value_str, RHNAME_MAX_SIZE - 1, "%d", cond_value[0]);
        }
        else
        {
            res =  sand_signal_field_get(unit, device, tokens[0], from_n, to_n, block_n,
                                                                               cond_value, tokens[1], value_str, NULL);
        }
    }

exit:
    utilex_str_split_free(tokens, realtokens);
    return res;
}

static sigstruct_t *
sand_signal_expand_get(
    int     unit,
    int     core,
    device_t * device,
    sigstruct_t *sigstruct_in,
    char *from_n,
    char *to_n,
    char *block_n,
    uint32 * value,
    expansion_t * expansion)
{
    int i, n;
    expansion_option_t *option;
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
        sigstruct_out = sand_signal_struct_get(device, expansion->name, from_n, to_n, block_n);
        goto exit;
    }

    /*
     * Dynamic expansion
     */
    if(value == NULL)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META("Value must be provided for dynamic expansion of:%s\n"), RHNAME(sigstruct_in)));
        goto exit;
    }
    option = expansion->options;
    for (i = 0; i < expansion->option_num; i++)
    {
        match_flag = TRUE;
        n = 0;
        while ((n < DSIG_OPTION_PARAM_MAX_NUM) && !ISEMPTY(option->param[n].name))
        {
            char value_str[RHNAME_MAX_SIZE];
            if((sand_signal_option_get(unit, core, device, RHNAME(sigstruct_in), from_n, to_n, block_n, value,
                                                                  option->param[n].name, value_str) != _SHR_E_NONE) ||
                (sal_strcasecmp(option->param[n].value, value_str) &&
                    ((utilex_str_stoul(value_str, &rt_value) != _SHR_E_NONE) ||
                        (utilex_str_stoul(option->param[n].value, &db_value) != _SHR_E_NONE) || (rt_value != db_value))))
            {
                 match_flag = FALSE;
                 break;
            }
            n++;
        }
        if (match_flag == TRUE)
        {       /* Found our expansion leave option loop */
            if((sigstruct_out = sand_signal_struct_get(device, option->name,
                                            sigstruct_in->from_n, sigstruct_in->to_n, sigstruct_in->block_n)) == NULL)
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
    int         core,
    device_t * device,
    uint32 * signal_value,
    int size_in,
    char *sigstruct_n,
    char *from_n,
    char *to_n,
    char *block_n,
    int byte_order,
    char *match_n,
    int in_flags,
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
    if (ISEMPTY(sigstruct_n) || (sigstruct = sand_signal_struct_get(device, sigstruct_n, from_n, to_n, block_n)) == NULL)
        goto exit;

    if(sigstruct->expansion_cb != NULL)
    {
        /*
         * Calling functions assign size 0 when there is no knowledge on it
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
        if(sigstruct->expansion_cb(unit, core, sigstruct->expansion_m.name, signal_value, size_in, &field_list) != _SHR_E_NONE)
        {
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META("Fail to expand on cb for:%s\n"), RHNAME(sigstruct)));
            goto exit;
        }
        /* Now check if we have a match just like we do below for regular XML parsing */
        if(field_list)
        {
            sand_signal_match_list(field_list, in_flags, match_n);

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
    if ((sigstruct_exp = sand_signal_expand_get(unit, core, device, sigstruct, from_n, to_n, block_n,
                                                                    signal_value, &sigstruct->expansion_m)) != NULL)
        sigstruct = sigstruct_exp;

    RHITERATOR(sigstruct_field, sigstruct->field_list)
    {
        int flags = in_flags;
        child_valid = 0;
        child_list = NULL;
        /*
         * If there is a condition check that it is fulfilled. No need to check if value was not provided
         */
        if (!(flags & SIGNALS_MATCH_NOCOND) && !ISEMPTY(sigstruct_field->cond_attribute) && (signal_value != NULL))
        {
            sand_signal_field_get(unit, device, RHNAME(sigstruct),
                               from_n, to_n, block_n, signal_value, sigstruct_field->cond_attribute, NULL, field_value);
            if (0 == VALUE(field_value))
            {
                /** From this place and below all fields values are irrelevant */
                flags |= SIGNALS_MATCH_EMPTY_VALUE;
            }
        }
        /*
         * Check for further usage dynamic expansion for field
         * In case of dynamic expansion find the matching expansion and switch to it - in this case for field
         */
        sigstruct_exp4field = sand_signal_expand_get(unit, core, device, sigstruct, from_n, to_n, block_n,
                                                     signal_value, &sigstruct_field->expansion_m);
        if (sigstruct_exp4field)
            field_dyn_name = RHNAME(sigstruct_exp4field);
        else
            field_dyn_name = sigstruct_field->expansion_m.name;

        sal_memset(field_value, 0, DSIG_MAX_SIZE_BYTES);
        if(signal_value != NULL)
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
            child_valid = sand_signal_expand(unit, core, device, field_value, 0, RHNAME(sigstruct_exp4field),
                                            from_n, to_n, block_n, byte_order, cur_match_n, flags, NULL, &child_list);
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

        sal_strncpy(RHNAME(field_output), RHNAME(sigstruct_field), RHNAME_MAX_SIZE - 1);

        sal_strncpy(field_output->expansion, field_dyn_name, RHNAME_MAX_SIZE - 1);

        sal_memcpy(field_output->value, field_value, DSIG_MAX_SIZE_BYTES);

        if(!(flags & SIGNALS_MATCH_EMPTY_VALUE))
        {
            sand_signal_value_to_str_full(unit, device, sigstruct_field->type, sigstruct_field->resolution,
                                          from_n, to_n, block_n, flags, field_value,
                                          field_output->print_value, sigstruct_field->size, byte_order);
        }
        else
        {
            sal_strncpy(field_output->print_value, "N/A", DSIG_MAX_SIZE_STR - 1);
        }
        field_output->field_list = child_list;
        field_output->size      = sigstruct_field->size;
        field_output->start_bit = sigstruct_field->start_bit;
        field_output->end_bit   = sigstruct_field->end_bit;
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

    if (ISEMPTY(expansion_n) || sand_signal_struct_get(device, expansion_n, NULL, NULL, NULL) == NULL)
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
    char *to_n,
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
    if((size = sand_signal_expand(unit, 0, device, value, size_in, expansion_n, from_n, to_n, block_n,
                        output_order, NULL, 0, &sigstruct, &field_list)) == 0)
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
     * Create print value from raw one if value was provided
     */
    if(value != NULL)
    {
        sand_signal_value_to_str_full(unit, device, SAL_FIELD_TYPE_NONE, expansion_n, from_n, to_n, block_n,
                                      0, value, signal_output->print_value, size, output_order);
        SHR_BITCOPY_RANGE(signal_output->value, 0, value, 0, size);
    }

    /*
     * Fill pointer to structure fields
     */
    signal_output->size = size;
    signal_output->field_list = field_list;
    sal_strncpy(signal_output->expansion, RHNAME(sigstruct), RHNAME_MAX_SIZE - 1);

exit:
    return rv;
}

static shr_error_e
sand_signal_field_params(
    int unit,
    device_t * device,
    int show_flag,
    char *field_n,
    char *from_n,
    char *to_n,
    char *block_n,
    int *offset_p,
    int *size_p,
    sigstruct_field_t **sigstruct_field_p)
{
    sigstruct_t *sigstruct;
    sigstruct_field_t *sigstruct_field;
    char **tokens;
    uint32 realtokens = 0;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Split by dot into structure name, lower field and what is left
     */

    if ((tokens = utilex_str_split(field_n, ".", 3, &realtokens)) == NULL)
    {
        SHR_EXIT_WITH_LOG(_SHR_E_MEMORY, "Failed to split field name %s%s%s\n", EMPTY, EMPTY, EMPTY);
    }

    if((sigstruct = sand_signal_struct_get(device, tokens[0], from_n, to_n, block_n)) == NULL)
    {
        SHR_EXIT_WITH_LOG(_SHR_E_NOT_FOUND, "Structure:\"%s\" not found %s%s\n", tokens[0], EMPTY, EMPTY);
    }

    if(realtokens == 1)
    {   /*
         * Means that input string was just structure name.
         * Offset is untouched, size will be the one of the entire structure
         */
        *size_p = sigstruct->size;
        SHR_EXIT();
    }

    RHITERATOR(sigstruct_field, sigstruct->field_list)
    {
       /*
        * We are looking for 2nd token resolution
        */
        if (!sal_strcasecmp(tokens[1], RHNAME(sigstruct_field)))
        {
            /*
             * Found - update offset from the beginning of root structure
             */
            if((sigstruct->order == PRINT_LITTLE_ENDIAN) || (show_flag == FALSE))
                *offset_p += sigstruct_field->start_bit;
            else
                *offset_p += (sigstruct->size - sigstruct_field->end_bit - 1);

            if (realtokens == 2)
            {  /* Last token - obtain size, default value pointer and return */
                *size_p = sigstruct_field->size;
                if(sigstruct_field_p != NULL)
                {
                    *sigstruct_field_p = sigstruct_field;
                }
            }
            else
            {  /* Move to the second token(after 1st point and repeat */
                char exp_field_n[RHNAME_MAX_SIZE];
                sal_snprintf(exp_field_n, RHNAME_MAX_SIZE - 1, "%s.%s", sigstruct_field->expansion_m.name, tokens[2]);
                SHR_IF_ERR_EXIT(sand_signal_field_params(unit, device, show_flag, exp_field_n,
                                                        from_n, to_n, block_n, offset_p, size_p, sigstruct_field_p));
            }
            break;
        }
    }

    if(sigstruct_field == NULL)
    {
        /*
         * Field does not exist
         */
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
    }
exit:
    utilex_str_split_free(tokens, realtokens);
    SHR_FUNC_EXIT;
}

shr_error_e
sand_signal_field_params_get(
    int unit,
    char *field_n,
    int *offset_p,
    int *size_p)
{
    device_t * device;
    /*
     * Assign NULL here, assuming that for current need there is no dependency on stage and block
     * Once it changes - it should be properly handled
     */
    char *block_n = NULL;
    char *from_n = NULL;
    char *to_n = NULL;

    SHR_FUNC_INIT_VARS(unit);

    if ((device = sand_signal_device_get(unit)) == NULL)
    {
        SHR_EXIT_WITH_LOG(_SHR_E_NOT_FOUND, "DB device not found %s%s%s\n", EMPTY, EMPTY, EMPTY);
    }

    *offset_p = 0;
    *size_p   = 0;

    /*
     * Do not print error message if field was not found, it may be part of work-flow
     * Caller should print message if it requested
     */
    SHR_SET_CURRENT_ERR(sand_signal_field_params(unit, device, TRUE, field_n,
                                                                        from_n, to_n, block_n, offset_p, size_p, NULL));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
sand_signal_compose(
    int unit,
    signal_output_t *signal_output,
    char *from_n,
    char *to_n,
    char *block_n,
    int byte_order)
{
    device_t *device;
    sigstruct_t *sigstruct;
    sigstruct_field_t *sigstruct_field;
    signal_output_t *field_output;

    SHR_FUNC_INIT_VARS(unit);

    if ((device = sand_signal_device_get(unit)) == NULL)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        SHR_EXIT();
    }

    if((sigstruct = sand_signal_struct_get(device, RHNAME(signal_output), from_n, to_n, block_n)) == NULL)
    {
        SHR_EXIT_WITH_LOG(_SHR_E_PARAM, "Structure:\"%s\" is not supported %s%s\n", RHNAME(signal_output), EMPTY, EMPTY);
    }

    signal_output->size = sigstruct->size;
    /*
     *  First - fill the structure with default field values
     */
    RHITERATOR(sigstruct_field, sigstruct->field_list)
    {
        SHR_BITCOPY_RANGE(signal_output->value, sigstruct_field->start_bit, sigstruct_field->default_value, 0,
                                                                            sigstruct_field->size);
    }
    /*
     * Now overwrite field from the input list
     */
    RHITERATOR(field_output, signal_output->field_list)
    {
        int size, offset = 0;
        sigstruct_field_t *sigstruct_field;

        SHR_SET_CURRENT_ERR(sand_signal_field_params(unit, device, FALSE, RHNAME(field_output), from_n, to_n, block_n,
                                                                                    &offset, &size, &sigstruct_field));
        if(!SHR_FUNC_VAL_IS(_SHR_E_NONE))
        {
            LOG_WARN(BSL_LS_SOC_COMMON, (BSL_META("Field:%s not found\n"), RHNAME(field_output)));
            SHR_EXIT();
        }
        /*
         * Extract the value from print_value if it is not empty,
         * otherwise just take value that was inside field_output
         */
        if(!ISEMPTY(field_output->print_value))
        {
            if(sand_signal_str_to_value(unit, sigstruct_field->type, field_output->print_value, size,
                                                                field_output->value) != _SHR_E_NONE)
            {
                LOG_WARN(BSL_LS_SOC_COMMON, (BSL_META("Error extracting value of '%s' from '%s'\n"),
                                                                RHNAME(sigstruct_field), field_output->print_value));
            }
        }

        SHR_BITCOPY_RANGE(signal_output->value, offset, field_output->value, 0, sigstruct_field->size);
    }

    sand_signal_value_to_str(unit, SAL_FIELD_TYPE_NONE, signal_output->value, signal_output->print_value,
                                                                  signal_output->size, byte_order);

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
sand_signal_single_get(
    device_t * device,
    int unit,
    int core,
    char *block_n,
    debug_signal_t *debug_signal,
    match_t * match_p,
    char *signal_match_n,
    char *field_match_n,
    rhlist_t * dsig_list)
{
    signal_output_t *signal_output = NULL;
    rhhandle_t temp = NULL;
    char *down_match_n;
    int top_valid, child_valid;
    uint32 value[DSIG_MAX_SIZE_UINT32];
    rhlist_t *field_list = NULL;
    char *output_signal_n;
    int flags = match_p->flags;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * We set status to not found and change it to success only if pass all the filters
     */
    SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);

    if ((flags & SIGNALS_MATCH_PERM) && (debug_signal->perm == 0))
        SHR_EXIT();

    if ((signal_match_n == NULL)       /* No match string on input */
        || ((flags & SIGNALS_MATCH_EXACT) &&   /* Match should be exact one */
            (!sal_strcasecmp(debug_signal->attribute, signal_match_n) || ((flags & SIGNALS_MATCH_HW) &&
             !sal_strcasecmp(debug_signal->hw, signal_match_n))))
        || (!(flags & SIGNALS_MATCH_EXACT) &&  /* Match may be any part of name */
           ((sal_strcasestr(debug_signal->attribute, signal_match_n) != NULL) || ((flags & SIGNALS_MATCH_HW) &&
             (sal_strcasestr(debug_signal->hw, signal_match_n) != NULL)))))
        top_valid = 1;
    else
        top_valid = 0;
    /*
     * If double flag set and no MATCH_DOUBLE skip this signal
     */
    if (!(flags & SIGNALS_MATCH_DOUBLE) && (debug_signal->double_flag == 1))
        SHR_EXIT();
    /*
     * If there is a condition check that it is fulfilled
     */
    if (!(flags & SIGNALS_MATCH_NOVALUE) && (!(flags & SIGNALS_MATCH_NOCOND) && debug_signal->cond_signal))
    {
        sand_signal_get_value(unit, core, debug_signal->cond_signal, value);
        if (VALUE(value) == 0)
        {
            flags |= SIGNALS_MATCH_EMPTY_VALUE;
        }
    }

    if ((top_valid == 1) && (field_match_n != NULL))
    {
        /*
         * Look into subfield for match, above match is not enough
         */
        top_valid = 0;
        down_match_n = field_match_n;
    }
    else
    {
        /** If signal found bring all its sub-fields, meaning using NULL match string */
        down_match_n = top_valid ? NULL : signal_match_n;
    }
    /*
     * Fill signal value
     */
    if (!(flags & SIGNALS_MATCH_NOVALUE))
    {
        if(sand_signal_get_value(unit, core, debug_signal, value) != _SHR_E_NONE)
        {
            flags |= SIGNALS_MATCH_EMPTY_VALUE;
        }
    }
    /*
     * Now start expansion
     */
    if ((flags & SIGNALS_MATCH_EXPAND) && !(flags & SIGNALS_MATCH_EMPTY_VALUE))
    {
        child_valid = sand_signal_expand(unit, core, device, value, debug_signal->size, debug_signal->expansion,
                                         debug_signal->from, debug_signal->to, block_n,
                                         match_p->output_order, down_match_n, flags, NULL, &field_list);
    }
    else
    {
        child_valid = 0;
        field_list = NULL;
    }

    if ((top_valid == 0) && (child_valid == 0)) /* No match in any place */
        SHR_EXIT();
    /*
     * We found it, keep output name as we have it from input
     */
    output_signal_n = debug_signal->attribute;
    /*
     * Now we can allocate output and fill it
     */
    SHR_SET_CURRENT_ERR(utilex_rhlist_entry_add_tail(dsig_list, output_signal_n, RHID_TO_BE_GENERATED,  &temp));
    if(!SHR_FUNC_VAL_IS(_SHR_E_NONE))
    {   /** free already allocated child list, that we have no way to preserve */
        sand_signal_list_free(field_list);
        SHR_EXIT();
    }
    signal_output = temp;
    /*
     * Copy debug signal into output_signal
     */
    signal_output->debug_signal = debug_signal;
    /*
     * Create print value from raw one
     */
    if(flags & SIGNALS_MATCH_EMPTY_VALUE)
    {
        sal_strncpy(signal_output->print_value, "N/A", DSIG_MAX_SIZE_STR - 1);
    }
    else if (!(flags & SIGNALS_MATCH_NOVALUE))
    {
        char *resolution_n;
        if(flags & SIGNALS_MATCH_EXPAND)
            resolution_n = debug_signal->resolution;
        else
            resolution_n = NULL;
        sand_signal_value_to_str_full(unit, device, SAL_FIELD_TYPE_NONE, resolution_n,
                                      debug_signal->from, debug_signal->to, block_n, flags, value,
                                      signal_output->print_value, debug_signal->size, match_p->output_order);
        sal_memcpy(signal_output->value, value, BITS2WORDS(debug_signal->size) * sizeof(uint32));
    }
    /*
     * Fill pointer to structure fields
     */
    signal_output->size = debug_signal->size;
    signal_output->core = core;
    sal_strncpy(signal_output->block_n, block_n, RHNAME_MAX_SIZE - 1);
    signal_output->field_list = field_list;
    signal_output->device = device;

    SHR_SET_CURRENT_ERR(_SHR_E_NONE);

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
sand_signal_filter_stage(
    debug_signal_t *debug_signal,
    pp_stage_t *stage_from,
    pp_stage_t *stage_to,
    match_t * match_p)
{
    char *from_n;
    char *to_n;

    if(match_p->flags & SIGNALS_MATCH_STAGE_RANGE)
    {
        if(((stage_from != NULL) && (stage_from->pp_id >= match_p->from_id) && (stage_from->pp_id < match_p->to_id)) ||
           ((stage_to != NULL)   && (stage_to->pp_id > match_p->from_id)    && (stage_to->pp_id <= match_p->to_id)) )
        {
            return _SHR_E_NONE;
        }
        else
        {
            return _SHR_E_NOT_FOUND;
        }
    }

    if(stage_from != NULL)
    {
        from_n = stage_from->name;
    }
    else if(debug_signal != NULL)
    {
        from_n = debug_signal->from;
    }
    else
    {
        from_n = "";
    }

    if(stage_to != NULL)
    {
        to_n = stage_to->name;
    }
    else if(debug_signal != NULL)
    {
        to_n   = debug_signal->to;
    }
    else
    {
        to_n = "";
    }

    if (!ISEMPTY(match_p->stage))
    {
        /** From == To means we want some stage either as from or to, so e one of conditions need to be true */
        if ((sal_strcasestr(from_n, match_p->stage) == NULL) && (sal_strcasestr(to_n, match_p->stage) == NULL))
            return _SHR_E_NOT_FOUND;
    }
    else
    {
        /** Here one of both conditions must be true to compare the attributes */
        if (!ISEMPTY(match_p->from) && (sal_strcasestr(from_n, match_p->from) == NULL))
            return _SHR_E_NOT_FOUND;
        if (!ISEMPTY(match_p->to)   && (sal_strcasestr(to_n, match_p->to)     == NULL))
            return _SHR_E_NOT_FOUND;
    }
    return _SHR_E_NONE;
}

shr_error_e
sand_signal_list_get(
    int unit,
    int core,
    match_t * match_p,
    rhlist_t * dsig_list)
{
    int i, j;
    char *match_n;
    char *field_match_n;
    device_t * device;
    pp_block_t *cur_pp_block;
    debug_signal_t *debug_signal;

    char **tokens = NULL;
    uint32 realtokens = 0, maxtokens = 2;

    SHR_FUNC_INIT_VARS(unit);

    if (((device = sand_signal_device_get(unit)) == NULL) || (device->pp_blocks == NULL))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Signal DB for:%s was not initialized\n", SOC_CHIP_STRING(unit));
    }

    match_n = match_p->name;

    tokens = utilex_str_split(match_n, ".", maxtokens, &realtokens);
    if (realtokens && (tokens == NULL))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Problem parsing match string:%s\n", match_n);
    }

    if (realtokens == maxtokens)
    {
        match_n = tokens[0];
    }

    if(tokens != NULL)
        field_match_n = tokens[1];
    else
        field_match_n = NULL;

    for (i = 0; i < device->block_num; i++)
    {
        cur_pp_block = &device->pp_blocks[i];

        if (!ISEMPTY(match_p->block) && sal_strcasecmp(cur_pp_block->name, match_p->block))
            continue;

#ifdef ADAPTER_SERVER_MODE
        if(!(match_p->flags & SIGNALS_MATCH_ASIC))
        {
            int i_st;
            for (i_st = 0; i_st < cur_pp_block->stage_num; i_st++)
            {
                pp_stage_t *cur_pp_stage = &cur_pp_block->stages[i_st];
                /*
                 * Ignore stage with no adapter association or without substitute
                 */
                if(ISEMPTY(cur_pp_stage->adapter_name) ||
                                ((cur_pp_stage->ms_id == DSIG_ILLEGAL_MS_ID) &&
                                !(cur_pp_stage->status & SIGNAL_STAGE_HAS_SUBSTITUTE)))
                    continue;
                /*
                 * Filter on stage
                 */
                if(sand_signal_filter_stage(NULL, cur_pp_stage, cur_pp_stage->stage_to, match_p) != _SHR_E_NONE)
                    continue;
                /*
                 * Fetch signals only if requested and if there is no cached data in cur_pp_stage->stage_debug_signals
                 */
                if(!(match_p->flags & SIGNALS_MATCH_NOVALUE) && (cur_pp_stage->stage_debug_signals == NULL))
                {
                    SHR_IF_ERR_EXIT(sand_adapter_get_stage_signals(device, unit, cur_pp_block->name, cur_pp_stage,
                                                                                                    match_p->flags));
                }

                for (j = 0; j < cur_pp_stage->stage_signal_num; j++)
                {
                    debug_signal = &cur_pp_stage->stage_debug_signals[j];

                    if ((sand_signal_single_get(device, unit, core, cur_pp_block->name, debug_signal, match_p, match_n,
                                                field_match_n, dsig_list) == _SHR_E_NONE) &&
                            (match_p->flags & SIGNALS_MATCH_ONCE))
                    {  /* No need to look anymore */
                        goto exit;
                    }
                }
            }  /* stage loop */
        }  /* Adapter context */
        else
#endif
        {
            for (j = 0; j < cur_pp_block->block_signal_num; j++)
            {
                debug_signal = &cur_pp_block->block_debug_signals[j];
                /*
                 * Filter on stage
                 */
                if(sand_signal_filter_stage(debug_signal, debug_signal->stage_from, debug_signal->stage_to, match_p)
                                                                                                        != _SHR_E_NONE)
                    continue;

                if ((sand_signal_single_get(device, unit, core, cur_pp_block->name, debug_signal, match_p, match_n, field_match_n,
                                            dsig_list) == _SHR_E_NONE) && (match_p->flags & SIGNALS_MATCH_ONCE))
                {  /* No need to look anymore */
                    goto exit;
                }
            }
        }
    }   /* Dynamic Data */

exit:
    utilex_str_split_free(tokens, realtokens);
    SHR_FUNC_EXIT;
}

static signal_output_t*
sand_signal_output_get(
    rhlist_t * field_list,
    char *full_name)
{
    signal_output_t *cur_signal_output = NULL;
    int status;
    char *name;

    char **tokens;
    uint32 realtokens = 0, maxtokens = 2;

    tokens = utilex_str_split(full_name, ".", maxtokens, &realtokens);
    if (realtokens && (tokens == NULL))
    {
        SHR_EXIT();
    }

    if (realtokens == maxtokens)
        name = tokens[0];
    else
        name = full_name;

    RHITERATOR(cur_signal_output, field_list)
    {
        status = sal_strcasecmp(RHNAME(cur_signal_output), name);
        /*
         * If there is no match try attribute instead of RHNAME
         */
        if((status != 0) && (cur_signal_output->debug_signal != NULL)
                                                            && (!ISEMPTY(cur_signal_output->debug_signal->attribute)))
        {
            status = sal_strcasecmp(cur_signal_output->debug_signal->attribute, name);
        }
        /*
         * If no more tokens and strings match we found our target
         */
        if ((realtokens == 1) && (status == 0))
        {
            SHR_EXIT();
        }
        else
        {
            char *next_name;
            signal_output_t *next_signal_output;
            /*
             * If we had 2 tokens and strings were match we need to move to new token
             */
            if ((realtokens == maxtokens) && (status == 0))
                next_name = tokens[1];
            else
                next_name = full_name;
            /*
             * If we have regular string or strings were not match keep original name
             */
            if ((next_signal_output = sand_signal_output_get(cur_signal_output->field_list, next_name)) != NULL)
            {
                cur_signal_output = next_signal_output;
                SHR_EXIT();
            }
        }
    }

exit:
    utilex_str_split_free(tokens, realtokens);
    return cur_signal_output;
}

static shr_error_e
sand_signal_is_supported(
    int unit,
    char *block_n,
    char *from_n,
    char *signal_n)
{
    unsupported_t *unsupported_signal;
    char **tokens = NULL;
    uint32 realtokens = 0;
    device_t*   device;

    SHR_FUNC_INIT_VARS(unit);

    if(ISEMPTY(signal_n) || ISEMPTY(block_n) || ISEMPTY(from_n))
    {
        /*
         * Deal only with full block, stage and attribute.
         * Otherwise just leave - we have not enough data to make the decision
         */
        SHR_EXIT();
    }

    if ((device = sand_signal_device_get(unit)) == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Signal DB for:%s was not initialized\n", SOC_CHIP_STRING(unit));
    }

    if (device->unsupported_list == NULL)
    {
        /*
         * No list, leave with no errors
         */
        SHR_EXIT();
    }
    /*
     * If signal name is actually field - split it and look for signal itself, field is not relevant
     */
    tokens = utilex_str_split(signal_n, ".", 2, &realtokens);
    if (tokens == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Problem parsing %s\n", signal_n);
    }
    /*
     * If there was a dot inside - take only first token
     */
    if(realtokens == 2)
    {
        signal_n = tokens[0];
    }

    RHITERATOR(unsupported_signal, device->unsupported_list)
    {
        /*
         * If there is specific block name, only signal belonging to this block may be matched to it
         */
        if (!ISEMPTY(unsupported_signal->block_n) && sal_strcasecmp(unsupported_signal->block_n, block_n))
            continue;
        /*
         * If there is specific "from" stage name, only signal from this stage may be matched to it
         */
        if (!ISEMPTY(unsupported_signal->from_n) && sal_strcasecmp(unsupported_signal->from_n, from_n))
            continue;
        /*
         * Unsupported may be per stage and/or block - not specific per signal
         */
        if (!ISEMPTY(RHNAME(unsupported_signal)) && !sal_strcasecmp(RHNAME(unsupported_signal), signal_n))
        {
            LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "Signal %s.%s.%s is not supported on this device\n"),
                                                                                            block_n, from_n, signal_n));
            SHR_SET_CURRENT_ERR(_SHR_E_UNAVAIL);
            SHR_EXIT();
       }
    }

exit:
    utilex_str_split_free(tokens, realtokens);
    SHR_FUNC_EXIT;
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
    rhlist_t *dsig_list = NULL;
    match_t match;
    char desc_str[RHSTRING_MAX_SIZE];

    SHR_FUNC_INIT_VARS(unit);

    if ((value == NULL) || (size == 0))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid signal output parameters\n");
    }

    SHR_SET_CURRENT_ERR(sand_signal_is_supported(unit, block, from, name));
    if (!SHR_FUNC_VAL_IS(_SHR_E_NONE))
    {
        SHR_EXIT();
    }

    memset(&match, 0, sizeof(match_t));
    match.block = block;
    match.from = from;
    match.to = to;
    match.name = name;
    match.flags = SIGNALS_MATCH_EXPAND | SIGNALS_MATCH_ONCE | SIGNALS_MATCH_EXACT | SIGNALS_MATCH_RETRY;

    /*
     * Create signal description string for log usage
     */
    sand_signal_description(&match, desc_str);

    if ((dsig_list = utilex_rhlist_create("signals", sizeof(signal_output_t), 0)) == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_MEMORY, "No memory for signal:'%s'\n", desc_str);
    }

    if ((sand_signal_list_get(unit, core, &match, dsig_list) != _SHR_E_NONE) || (RHLNUM(dsig_list) == 0))
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        SHR_EXIT();
    }
    else
    {
        signal_output_t *cur_signal_output;
        if ((cur_signal_output = sand_signal_output_get(dsig_list, name)) == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Fail to fetch signal:'%s' from output\n", desc_str);
        }
        else
        {
            if (size < BITS2WORDS(cur_signal_output->size))
            {
                SHR_EXIT_WITH_LOG(_SHR_E_RESOURCE, "Not enough memory for signal:%s %d vs %d\n", desc_str, size,
                                                                                             cur_signal_output->size);
            }
            else
            {
                sal_memcpy(value, cur_signal_output->value, BITS2WORDS(cur_signal_output->size) * 4);
            }
        }
    }

exit:
    if(dsig_list != NULL)
    {
        sand_signal_list_free(dsig_list);
    }
    SHR_FUNC_EXIT;
}

shr_error_e
sand_signal_info_get(
    int unit,
    int core,
    char *block,
    char *from,
    char *to,
    char *name,
    signal_output_t ** signal_output_p)
{
    rhlist_t *dsig_list = NULL;
    match_t match;
    char desc_str[RHSTRING_MAX_SIZE];

    SHR_FUNC_INIT_VARS(unit);

    SHR_SET_CURRENT_ERR(sand_signal_is_supported(unit, block, from, name));
    if (!SHR_FUNC_VAL_IS(_SHR_E_NONE))
    {
        SHR_EXIT();
    }

    memset(&match, 0, sizeof(match_t));
    match.block = block;
    match.from = from;
    match.to = to;
    match.name = name;
    match.flags = SIGNALS_MATCH_EXPAND | SIGNALS_MATCH_ONCE | SIGNALS_MATCH_EXACT | SIGNALS_MATCH_RETRY;

    /*
     * Create signal description string for log usage
     */
    sand_signal_description(&match, desc_str);

    if ((dsig_list = utilex_rhlist_create("signals", sizeof(signal_output_t), 0)) == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_MEMORY, "No memory for signal:'%s'\n", desc_str);
    }

    if ((sand_signal_list_get(unit, core, &match, dsig_list) != _SHR_E_NONE) || (RHLNUM(dsig_list) == 0))
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        SHR_EXIT();
    }
    else
    {
        signal_output_t * signal_output_internal;
        if ((signal_output_internal = sand_signal_output_get(dsig_list, name)) == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Fail to fetch signal:'%s' from output\n", desc_str);
        }
        else
        {
            *signal_output_p = signal_output_internal;
        }
    }

exit:
    if(dsig_list != NULL)
    {
        sand_signal_list_free(dsig_list);
    }
    SHR_FUNC_EXIT;
}

shr_error_e
sand_signal_verify(
    int unit,
    int core,
    char *block,
    char *from,
    char *to,
    char *name,
    uint32 *expected_value_a,
    int  size,
    char *expected_str,
    char *result_str)
{
    rhlist_t *dsig_list = NULL;
    match_t match;
    char desc_str[RHSTRING_MAX_SIZE];

    SHR_FUNC_INIT_VARS(unit);

    if(((expected_value_a == NULL) || (size == 0)) && (expected_str == NULL))
    {
        SHR_EXIT_WITH_LOG(_SHR_E_PARAM, "No valid output pointers %s%s%s\n", EMPTY, EMPTY, EMPTY);
    }

    SHR_SET_CURRENT_ERR(sand_signal_is_supported(unit, block, from, name));
    if (!SHR_FUNC_VAL_IS(_SHR_E_NONE))
    {
        SHR_EXIT();
    }

    memset(&match, 0, sizeof(match_t));
    match.block = block;
    match.from = from;
    match.to = to;
    match.name = name;
    match.flags = SIGNALS_MATCH_EXPAND | SIGNALS_MATCH_ONCE | SIGNALS_MATCH_EXACT | SIGNALS_MATCH_RESOLVE_ONLY;
    /*
     * Create signal description string for log usage
     */
    sand_signal_description(&match, desc_str);

    if ((dsig_list = utilex_rhlist_create("signals", sizeof(signal_output_t), 0)) == NULL)
    {
        SHR_EXIT_WITH_LOG(_SHR_E_MEMORY, "No memory for signal:%s%s%s\n", desc_str, EMPTY, EMPTY);
    }

    if ((sand_signal_list_get(unit, core, &match, dsig_list) != _SHR_E_NONE) || (RHLNUM(dsig_list) == 0))
    {
        SHR_EXIT_WITH_LOG(_SHR_E_NOT_FOUND, "Signal not found:%s%s%s\n", desc_str, EMPTY, EMPTY);
    }
    else
    {
        signal_output_t *cur_signal_output;
        if ((cur_signal_output = sand_signal_output_get(dsig_list, name)) == NULL)
        {
            SHR_EXIT_WITH_LOG(_SHR_E_FAIL, "Fail to fetch signal:%s from output %s%s\n", desc_str, EMPTY, EMPTY);
        }
        if(expected_value_a != NULL)
        {
            if (size < BITS2WORDS(cur_signal_output->size))
            {
                SHR_EXIT_WITH_LOG(_SHR_E_RESOURCE, "Not enough memory for signal:%s %d vs %d\n", desc_str, size,
                                                                                             cur_signal_output->size);
            }
            else
            {
                if(sal_memcmp(expected_value_a, cur_signal_output->value, BITS2WORDS(cur_signal_output->size) * 4) != 0)
                {
                    char print_value[DSIG_MAX_SIZE_STR];
                    sand_signal_value_to_str(unit, SAL_FIELD_TYPE_NONE, expected_value_a, print_value,
                                                                        cur_signal_output->size, PRINT_BIG_ENDIAN);
                    if(result_str == NULL)
                    {   /* Once place to return non-matched value was not provided, print an error */
                        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(NO_UNIT,
                                                            "For signal:%s result:%s does not match expected %s\n"),
                                                            desc_str, cur_signal_output->print_value, print_value));
                    }
                    else
                    {   /* Otherwise copy the result */
                        sal_strncpy(result_str, cur_signal_output->print_value, DSIG_MAX_SIZE_STR - 1);
                    }
                    SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
                    SHR_EXIT();
                }
            }
        }
        else if(expected_str != NULL)
        {
            /*
             * Expected value may be in string or integer form, so we need to compare both resolved and unresolved
             * string to expected value
             */
            uint32 expected_value = 0;
            shr_error_e expected_value_status = _SHR_E_UNAVAIL;
            /*
             * Assuming string represents uint32 value
             */
            if(cur_signal_output->size <= 32)
            {   /* No support here for signals more than uint32 */
                expected_value_status = sand_signal_str_to_value(unit, SAL_FIELD_TYPE_UINT32,
                                                            expected_str, cur_signal_output->size,  &expected_value);
            }
            if((sal_strcasecmp(expected_str, cur_signal_output->print_value) != 0) &&
                ((expected_value_status != _SHR_E_NONE ||
                (sal_memcmp(&expected_value, cur_signal_output->value, BITS2WORDS(cur_signal_output->size) * 4) != 0))))
            {
                if(result_str == NULL)
                {   /* Once place to return non-matched value was not provided, print an error  */
                    LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(NO_UNIT,
                                                        "For signal:%s result:%s does not match expected %s\n"),
                                                        desc_str, cur_signal_output->print_value, expected_str));
                }
                else
                {   /* Otherwise copy the result */
                    sal_strncpy(result_str, cur_signal_output->print_value, DSIG_MAX_SIZE_STR - 1);
                }
                SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
                SHR_EXIT();
            }
            else
            {
                if(result_str != NULL)
                {   /* In case of success copy print value for caller usage */
                    sal_strncpy(result_str, cur_signal_output->print_value, DSIG_MAX_SIZE_STR - 1);
                }
            }
        }
    }

exit:
    if(dsig_list != NULL)
    {
        sand_signal_list_free(dsig_list);
    }
    SHR_FUNC_EXIT;
}

static int
sand_signal_handle_get(
    int unit,
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

    if (sand_signal_list_get(unit, core, &match, dsig_list) != _SHR_E_NONE)
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

#if (!defined(NO_FILEIO) && !defined(STATIC_SIGNAL_TEST))
/*
 * }
 */

/* (!defined(NO_FILEIO) && !defined(STATIC_SIGNAL_TEST))
 * {
 */
/**
 * \brief Fetch pointer to the stage based either on PP or adapter name
 * \param [in] unit     - unit id
 * \param [in] pp_block - pointer to the block, stage should belong to
 * \param [in] stage_n  - array of commands under the root
 * \return
 *     \retval pointer to the stage if found one
 *     \retval NULL if stage was not found in the block
 */
static pp_stage_t *
sand_signal_stage_get(
    int unit,
    device_t *device,
    pp_block_t *pp_block,
    char *stage_n)
{
    pp_stage_t *pp_stage;
    int i_st;

#ifdef BCM_DNX_SUPPORT
    /*
     * Replace stage if it was KBR encoded, we are not interested in return status
     * It was either modified or not in both cases continue with what we have
     */
    if (SOC_IS_DNX(unit))
    {
        dnx_pp_stage_kbr_resolve(unit, stage_n, stage_n);
    }
#endif

    for(i_st = 0; i_st < pp_block->stage_num; i_st++)
    {
        pp_stage = &pp_block->stages[i_st];

        if(!sal_strcasecmp(stage_n, pp_stage->name))
            return pp_stage;
    }

    
/*    return utilex_rhlist_entry_get_by_name(device->phydb_list, stage_n); */
    return NULL;
}

/*
 * Read expansion information from XML for structure and fields
 * No verification is performed, will be done by sand_signal_expand_init
 */
static void
sand_signal_expand_read(
    xml_node cur,
    expansion_t * expansion)
{
    xml_node curOption;
    expansion_option_t *option;

    /*
     * Read expansion from XML and dynamic options if any, they will be verified later when all the structures will be
     * scanned from XML to memory
     */
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

        RHDATA_GET_STR_DEF_NULL(cur, "block", sigparam->block_n);
        RHDATA_GET_STR_DEF_NULL(cur, "from", sigparam->from_n);
        RHDATA_GET_STR_DEF_NULL(cur, "to", sigparam->to_n);
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

static void
sand_signal_field_type_set(
    xml_node cur,
    sigstruct_field_t *sigstruct_field)
{
    char name[RHNAME_MAX_SIZE];
    RHDATA_GET_STR_DEF_NULL(cur, "type", name);
    /*
     * If no type was assigned, assume uint32 for fields fitting into 32 or array for longer one
     */
    if(ISEMPTY(name))
    {
        if(sigstruct_field->size <= 32)
        {
            sigstruct_field->type = SAL_FIELD_TYPE_UINT32;
        }
        else
        {
            sigstruct_field->type = SAL_FIELD_TYPE_ARRAY32;
        }
    }
    else
    {
        if((sigstruct_field->type = sal_field_type_by_name(name)) == SAL_FIELD_TYPE_MAX)
        {
            LOG_WARN(BSL_LS_SOC_COMMON, (BSL_META("field:%s type:%s is unsupported\n"),RHNAME(sigstruct_field),  name));
        }
    }
}

static shr_error_e
sand_signal_expand_verify(
        device_t * device,
        char *name,
        char *from_n,
        char *to_n,
        char *block_n,
        expansion_t * expansion,
        char *resolution)
{
    /*
     * First check if there is expansion at all
     */
    if (ISEMPTY(expansion->name))
    {
        return _SHR_E_EMPTY;
    }
    /*
     * If the expansion is dynamic check all options
     */
    else if(!sal_strcasecmp(expansion->name, "Dynamic"))
    {
        int i_opt;
        expansion_option_t *option = expansion->options;
        for (i_opt = 0; i_opt < expansion->option_num; i_opt++)
        {
            if(sand_signal_struct_get(device, option->name, from_n, to_n, block_n) == NULL)
            {
                LOG_INFO(BSL_LOG_MODULE, (BSL_META("Expansion:%s does not exist\n"), option->name));
                return _SHR_E_NOT_FOUND;
            }
            option++;
        }
    }
    /*
     * Now we should check if specific expansion exists
     */
    else if(sand_signal_struct_get(device, expansion->name, from_n, to_n, block_n) == NULL)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META("No signal expansion:%s for %s\n"), expansion->name, name));
        SET_EMPTY(expansion->name);
        return _SHR_E_NOT_FOUND;
    }
    else if (!ISEMPTY(resolution) && (sand_signal_resolve_get(device, resolution, from_n, to_n, block_n) == NULL))
    {
        /*
         * If resolution is not empty look if it exists
         */
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META("No signal resolution:%s on %s\n"), resolution, name));
        SET_EMPTY(resolution);
        return _SHR_E_NOT_FOUND;
    }

    return _SHR_E_NONE;
}

static void
sand_signal_expand_init(
    device_t * device)
{
    sigstruct_t *sigstruct;
    /*
     * Go through all structures & their fields and fill expansion were available
     */
    RHITERATOR(sigstruct, device->struct_list)
    {
        sigstruct_field_t *sigstruct_field;
        /*
         * If plugin installed no need for further checks - responsibility of plugin
         */
        if(sigstruct->expansion_cb)
        {
            continue;
        }
        /*
         * First verify that structure expansion exists and is not dynamic,
         * if yes field are irrelevant even if they exist
         */
        if(sand_signal_expand_verify(device, RHNAME(sigstruct), sigstruct->from_n, sigstruct->to_n, sigstruct->block_n,
                                                         &sigstruct->expansion_m, sigstruct->resolution) == _SHR_E_NONE)
        {
            if((sigstruct->field_list != NULL) && (RHLNUM(sigstruct->field_list) != 0))
            {
                LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META("Structure:\"%s\" is expanded as \"%s\""
                                "Internal fields will be ignored\n"), RHNAME(sigstruct), sigstruct->expansion_m.name));
            }
            continue;
        }

        RHITERATOR(sigstruct_field, sigstruct->field_list)
        {
            /*
             * Do the same verification on field, what is different that we need to check if field name is expansion
             */
            if(sand_signal_expand_verify(device, RHNAME(sigstruct_field), sigstruct->from_n, sigstruct->to_n,
                    sigstruct->block_n, &sigstruct_field->expansion_m, sigstruct_field->resolution) == _SHR_E_NONE)
            {
                /*
                 * Expansion found and it is valid, go to next field
                 */
                continue;
            }
            /*
             * When expansion is empty(invalid) first check may be it can be expanded by name
             */
            if (sand_signal_struct_get(device, RHNAME(sigstruct_field),
                                                        sigstruct->from_n, sigstruct->to_n, sigstruct->block_n) != NULL)
            {
                sal_strncpy(sigstruct_field->expansion_m.name, RHNAME(sigstruct_field), RHNAME_MAX_SIZE - 1);
            }
            else if (sand_signal_resolve_get(device, RHNAME(sigstruct_field),
                                                        sigstruct->from_n, sigstruct->to_n, sigstruct->block_n) != NULL)
            {
                /*
                 * If there is match - attribute serves as resolution
                 */
                sal_strncpy(sigstruct_field->resolution, RHNAME(sigstruct_field), RHNAME_MAX_SIZE - 1);
            }
        }
    }

    return;
}

static int
sand_signal_struct_read(
    int unit,
    rhlist_t *struct_list,
    xml_node nodeTop)
{
    xml_node curSubTop, cur, curSub;
    int res = _SHR_E_NONE;
    device_t * device;

    if ((device = sand_signal_device_get(unit)) == NULL)
    {
        res = _SHR_E_NOT_FOUND;
        goto exit;
    }

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
        RHDATA_GET_STR_DEF_NULL(cur, "to", sigstruct->to_n);

        RHDATA_GET_STR_DEF_NULL(cur, "order", name);
        RHDATA_GET_STR_DEF_NULL(cur, "resolution", sigstruct->resolution);
        if(!sal_strcasecmp(name, "msb"))
            sigstruct->order = PRINT_BIG_ENDIAN;
        else
            sigstruct->order = PRINT_LITTLE_ENDIAN;

        RHDATA_GET_STR_DEF_NULL(cur, "plugin", name);
        if(!ISEMPTY(name))
        {
            /*
             * Record plugin name
             */
            sal_strncpy(sigstruct->plugin_n, name, RHKEYWORD_MAX_SIZE - 1);
            /*
             * We have read plugin name from XML, not we assume that callback name if <plugin>_decode
             */
            if(!sal_strcasecmp(name, "pparse"))
            {
                sigstruct->expansion_cb = pparse_decode;
            }
#ifdef BCM_DNX_SUPPORT
            /** dbal and dnx_arr are supported only for DNX */
            else  if (SOC_IS_DNX(unit))
            {
                if(!sal_strcasecmp(name, "dbal"))
                {
                    sigstruct->expansion_cb = dbal_decode;
                }
                else if(!sal_strcasecmp(name, "dnx_arr"))
                {
                    sigstruct->expansion_cb = dnx_arr_decode;
                }
            }
#endif /* BCM_DNX_SUPPORT */
            if(sigstruct->expansion_cb == NULL)
            {
                LOG_WARN(BSL_LS_SOC_COMMON, (BSL_META("Plugin:%s for struct:%s is not supported\n"),
                                                                                            name, RHNAME(sigstruct)));
            }
            else
            {   /*
                 * We succeeded to load plugin callback, get expansion name
                 * If there is none signal name itself will be used
                 * In this case even if there are fields inside the structure they are not relevant,
                 * Plugin takes precedence, field may stay for the situation without dynamic loading
                 */
                RHDATA_GET_STR_DEF_NULL(cur, "expansion", sigstruct->expansion_m.name);
                continue;
            }
        }

        sand_signal_expand_read(cur, &sigstruct->expansion_m);

        if ((sigstruct->field_list = utilex_rhlist_create("fields", sizeof(sigstruct_field_t), 0)) == NULL)
            goto exit;

        /*
         * Check if the structure is array
         */
        {
            int element_size, i_el;
            RHDATA_GET_INT_DEF(cur, "array", sigstruct->array_count, 0);
            RHDATA_GET_INT_DEF(cur, "start", sigstruct->array_start, 1);
            if(sigstruct->array_count != 0)
            {
                if((size % sigstruct->array_count) != 0)
                {
                    LOG_WARN(BSL_LS_SOC_COMMON, (BSL_META("Array struct %s size %d is not divisible by %d\n"),
                                                                      RHNAME(sigstruct), size, sigstruct->array_count));
                }
                element_size = size/sigstruct->array_count;
                for(i_el = 0; i_el < sigstruct->array_count; i_el++)
                {
                    char el_name[4];
                    sal_snprintf(el_name, RHNAME_MAX_SIZE - 1, "%d", i_el + sigstruct->array_start);

                    if (utilex_rhlist_entry_add_tail(sigstruct->field_list, el_name, RHID_TO_BE_GENERATED, &temp) != _SHR_E_NONE)
                        break;
                    sigstruct_field = temp;

                    if(sigstruct->order == PRINT_LITTLE_ENDIAN)
                    {
                        sigstruct_field->start_bit = element_size * i_el;
                    }
                    else
                    {
                        sigstruct_field->start_bit = size - element_size * (i_el + 1);
                    }
                    sigstruct_field->end_bit   = sigstruct_field->start_bit + element_size - 1;

                    sigstruct_field->size      = element_size;
                    sand_signal_field_type_set(cur, sigstruct_field);

                    sal_strncpy(sigstruct_field->expansion_m.name, sigstruct->expansion_m.name, RHNAME_MAX_SIZE - 1);
                    sigstruct_field->expansion_m.option_num = sigstruct->expansion_m.option_num;
                    /*
                     * For dynamic expansion if option is also indexed, add it to option name as it is for field name
                     */
                    if(sigstruct->expansion_m.options != NULL)
                    {
                        int i_opt;
                        expansion_option_t *option;
                        /*
                         * Expansion already extracted by parent, so just copy it
                         * Copy expansion name, number of options and set options after allocation the spaces for copy
                         */
                        option = sigstruct_field->expansion_m.options
                                  = utilex_alloc(sigstruct_field->expansion_m.option_num * sizeof(expansion_option_t));

                        sal_memcpy(sigstruct_field->expansion_m.options, sigstruct->expansion_m.options,
                                                 sigstruct_field->expansion_m.option_num * sizeof(expansion_option_t));

                        for(i_opt = 0; i_opt < sigstruct_field->expansion_m.option_num; i_opt++, option++)
                        {
                            int i_param;
                            attribute_param_t *attribute_param = option->param;
                            for(i_param = 0; i_param < DSIG_OPTION_PARAM_MAX_NUM; i_param++, attribute_param++)
                            {
                                sigstruct_t *param_sigstruct;
                                if(ISEMPTY(attribute_param->name))
                                    break;
                                /*
                                 * If attribute parameter is an arrayed structure name
                                 * assume that it should have the same index as field, otherwise it is plain parameter
                                 */
                                if(((param_sigstruct = sand_signal_struct_get(device, attribute_param->name,
                                                sigstruct->from_n, sigstruct->to_n, sigstruct->block_n)) == NULL) ||
                                                                                    (param_sigstruct->array_count == 0))
                                    continue;

                                sal_snprintf(attribute_param->name, RHNAME_MAX_SIZE - 1, "%s.%d", attribute_param->name,
                                                                                   i_el + param_sigstruct->array_start);
                            }
                        }
                    }
                    RHDATA_GET_STR_DEF_NULL(cur, "resolution", sigstruct_field->resolution);
                }
                /*
                 * Clean expansion from structure, for array only fields may be expanded
                 */
                if(sigstruct->expansion_m.options != NULL)
                {
                    sal_free(sigstruct->expansion_m.options);
                }
                sal_memset(&sigstruct->expansion_m, 0, sizeof(expansion_t));
            }
        }
        /*
         * loop through entries
         */
        RHDATA_ITERATOR(curSub, cur, "field")
        {
            char str_value[RHNAME_MAX_SIZE];

            RHDATA_GET_STR_CONT(curSub, "name", name);

            if (utilex_rhlist_entry_add_tail(sigstruct->field_list, name, RHID_TO_BE_GENERATED, &temp) != _SHR_E_NONE)
                break;

            sigstruct_field = temp;

            RHDATA_GET_STR_CONT(curSub, "bits", name);

            if (sand_signal_range_parse(name, &sigstruct_field->start_bit, &sigstruct_field->end_bit,
                                                                    sigstruct->order, sigstruct->size) !=  _SHR_E_NONE)
            {
                LOG_ERROR(BSL_LOG_MODULE, (BSL_META("Field:%s.%s, has bad bits range:%s\n"),
                                                                    RHNAME(sigstruct), RHNAME(sigstruct_field), name));
                continue;
            }

            sigstruct_field->size = sigstruct_field->end_bit + 1 - sigstruct_field->start_bit;
            if(sigstruct_field->size > DSIG_FIELD_MAX_SIZE_BITES)
            {
                LOG_ERROR(BSL_LOG_MODULE, (BSL_META("field:%s.%s size(%d) is more than maximum :%d\n"),
                           RHNAME(sigstruct), RHNAME(sigstruct_field), sigstruct_field->size, DSIG_FIELD_MAX_SIZE_BITES));
            }
            RHDATA_GET_STR_DEF_NULL(curSub, "condition", sigstruct_field->cond_attribute);
            RHDATA_GET_STR_DEF_NULL(curSub, "resolution", sigstruct_field->resolution);
            /*
             * Fetch field type name and convert it to enum
             */
            sand_signal_field_type_set(curSub, sigstruct_field);

            RHDATA_GET_STR_DEF_NULL(curSub, "default", str_value);
            /*
             * If default was not defined assume null value
             * Nothing to do - just keep initial state
             */
            if(!ISEMPTY(str_value))
            {
                if(sand_signal_str_to_value(unit, sigstruct_field->type, str_value, sigstruct_field->size,
                                                                    sigstruct_field->default_value) != _SHR_E_NONE)
                {
                    LOG_WARN(BSL_LS_SOC_COMMON, (BSL_META("Error extracting value of %s.%s from %s\n"),
                                                                 RHNAME(sigstruct), RHNAME(sigstruct_field),  str_value));
                }
            }
            sand_signal_expand_read(curSub, &sigstruct_field->expansion_m);
        }
    }

exit:
    return res;
}

static shr_error_e
sand_signal_struct_init(
    int unit,
    device_t * device)
{
    xml_node curTop, curSubTop, cur;
    char filename[RHSTRING_MAX_SIZE];

    SHR_FUNC_INIT_VARS(unit);

    if (device->struct_list != NULL)
    {
        /*
         * Already initialized
         */
        SHR_EXIT();
    }

    if ((device->param_list = utilex_rhlist_create("param_list", sizeof(sigparam_t), 0)) == NULL)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_MEMORY);
        SHR_EXIT();
    }

    if ((device->struct_list = utilex_rhlist_create("struct_list", sizeof(sigstruct_t), 0)) == NULL)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_MEMORY);
        SHR_EXIT();
    }

    /* All structure/param files will be listed in parsing-objects section of DNX-Devices */
    if ((curTop = dbx_file_get_xml_top(unit, "DNX-Devices.xml", "top", 0)) == NULL)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        SHR_EXIT();
    }
    if ((curSubTop = dbx_xml_child_get_first(curTop, "parsing-objects")) == NULL)
    {
        /*
         * No parsing objects - just leave
         */
        SHR_EXIT();
    }

    RHDATA_ITERATOR(cur, curSubTop, "include")
    {
        char type[RHNAME_MAX_SIZE];

        RHDATA_GET_STR_DEF_NULL(cur, "type", type);
        if(!sal_strcasecmp(type, "structs"))
        {
            xml_node curFile;
            int device_specific = 0;
            int flags = CONF_OPEN_ALTER_LOC | CONF_OPEN_NO_ERROR_REPORT;

            RHDATA_GET_INT_DEF(cur, "device_specific", device_specific, 0);
            if(device_specific == TRUE)
            {
                flags |= CONF_OPEN_PER_DEVICE;
            }

            RHDATA_GET_STR_CONT(cur, "file", filename);
            if ((curFile = dbx_file_get_xml_top(unit, filename, "top", flags)) == NULL)
                continue;

            sand_signal_param_read(device->param_list, curFile);
            sand_signal_struct_read(unit, device->struct_list, curFile);
            dbx_xml_top_close(curFile);
        }
    }

    dbx_xml_top_close(curTop);

    /*
     * Check if there is Unsupported DebugSignals, if not - not an error - continue as usual
     */
    if ((curTop = dbx_file_get_xml_top(unit, "UnsupportedDebugSignals.xml", "signals", CONF_OPEN_NO_ERROR_REPORT)) != NULL)
    {
        rhhandle_t temp = NULL;
        unsupported_t *unsupported_signal;
        int this_is_adapter;
        char *this_chip_n = (char *) soc_dev_name(unit);
#ifdef ADAPTER_SERVER_MODE
        this_is_adapter = 1;
#else
        this_is_adapter = 0;
#endif
        /*
         * Create list that will include all legitimate signals not supported for the device
         */
        if ((device->unsupported_list = utilex_rhlist_create("unsupported_list", sizeof(unsupported_t), 0)) == NULL)
        {
            SHR_SET_CURRENT_ERR(_SHR_E_MEMORY);
            SHR_EXIT();
        }
        /*
         * Read the list of unsupported signals. The difference will be in return status in signal get routines
         *   For not supported _SHR_E_UNAVAIL will be returned
         *   For not found     _SHR_E_NOT_FOUND
         */
        RHDATA_ITERATOR(cur, curTop, "signal")
        {
            xml_node curDevice;
            char chip_n[RHNAME_MAX_SIZE];
            int is_adapter;
            /*
             * Check if the unsupported device is a current one
             */
            RHDATA_ITERATOR(curDevice, cur, "device")
            {
                RHDATA_GET_STR_CONT(curDevice, "chip", chip_n);
                RHDATA_GET_INT_DEF(curDevice, "adapter", is_adapter, 0);
                if(!sal_strcasecmp(this_chip_n, chip_n) && (this_is_adapter == is_adapter))
                {
                    char signal_n[RHNAME_MAX_SIZE];
                    /*
                     * Add this element to the list
                     */
                    RHDATA_GET_STR_DEF_NULL(cur, "name", signal_n);
                    SHR_IF_ERR_EXIT(utilex_rhlist_entry_add_tail(device->unsupported_list, signal_n,
                                                                                         RHID_TO_BE_GENERATED, &temp));
                    unsupported_signal = (unsupported_t *)temp;
                    RHDATA_GET_STR_DEF_NULL(cur, "block", unsupported_signal->block_n);
                    RHDATA_GET_STR_DEF_NULL(cur, "from", unsupported_signal->from_n);
                    break;
                }
            }
        }

        dbx_xml_top_close(curTop);
    }

    sand_signal_expand_init(device);

exit:
    SHR_FUNC_EXIT;
}

static void
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
        utilex_free(sigstruct->expansion_m.options);
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

    if(device->unsupported_list)
        utilex_rhlist_free_all(device->unsupported_list);
    device->unsupported_list = NULL;

    return;
}
shr_error_e
sand_qual_signal_init(
    int unit,
    char *db_name,
    char *qual_signals_db,
    pp_stage_t * pp_stage)
{
    void *curTop, *cur;
    internal_signal_t *cur_internal_signal;
    SHR_FUNC_INIT_VARS(NO_UNIT);
    if ((curTop = dbx_file_get_xml_top(unit, qual_signals_db, "SignalInfo", CONF_OPEN_PER_DEVICE)) == NULL)
    {
        SHR_EXIT_WITH_LOG(_SHR_E_NOT_FOUND, "No section SignalInfo in:%s%s%s\n", db_name, EMPTY, EMPTY);
    }
    RHDATA_GET_INT_STOP(curTop, "size0", pp_stage->buffer0_size);
    RHDATA_GET_INT_DEF(curTop,  "size1", pp_stage->buffer1_size, 0);
    RHDATA_GET_NODE_NUM(pp_stage->number, curTop, "signal");
    if (pp_stage->number == 0)
    {
        SHR_EXIT_WITH_LOG(_SHR_E_NOT_FOUND, "No pp stages in:%s%s%s\n", db_name, EMPTY, EMPTY);
    }
    cur_internal_signal = pp_stage->signals = utilex_alloc(sizeof(internal_signal_t) * pp_stage->number);
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
    SHR_FUNC_EXIT;
}

/**
 * \brief Init list of memories(physical DB) per device
 * \param [in] unit     - unit id
 * \param [in] device   - pointer to device structure
 * \param [in] curTop   - handle to the top of PP.XML
 * \return
 *     \retval _SHR_E_NONE if all went smooth
 */
static shr_error_e
sand_signal_phydb_init(
        int unit,
        device_t * device,
        void *curTop)
{
    void *curSubTop, *curPhyDB;

    SHR_FUNC_INIT_VARS(unit);

    if ((curSubTop = dbx_xml_child_get_first(curTop, "phydb-list")) == NULL)
    {
        /*
         * No list - nothing happened
         */
        SHR_EXIT();
    }

    if ((device->phydb_list = utilex_rhlist_create("phydb_list", sizeof(pp_stage_t), 1)) == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_MEMORY, "No memory for db-list on:%s\n", soc_dev_name(unit));
    }
    /*
     * loop through entries
     */
    RHDATA_ITERATOR(curPhyDB, curSubTop, "phydb")
    {
        rhhandle_t rhhandle;
        char db_name[RHNAME_MAX_SIZE];
        /*
         * If there is no name - stage is not relevant for chip signal processing
         * Number of stages recorded is ones relevant
         */
        RHDATA_GET_STR_CONT(curPhyDB, "name", db_name);
        SHR_IF_ERR_EXIT(utilex_rhlist_entry_add_tail(device->phydb_list, db_name, RHID_TO_BE_GENERATED, &rhhandle));
    }

exit:
    SHR_FUNC_EXIT;
}

static pp_stage_t *
sand_signal_block_stage_find(
    pp_block_t *pp_block,
    char *name)
{
    int i_st;
    pp_stage_t *pp_stage = NULL;

    for (i_st = 0; i_st < pp_block->stage_num; i_st++)
    {
        pp_stage = &pp_block->stages[i_st];
        if(!sal_strcasecmp(pp_stage->name, name))
        {
            break;
        }
    }
    return pp_stage;
}

/*
 *
 */
static shr_error_e
sand_signal_get_stages(
        int unit,
        pp_block_t *pp_block,
        void *curBlock)
{
    void *curStage;
    int db_stage_num;

    SHR_FUNC_INIT_VARS(unit);

    RHDATA_GET_NODE_NUM(db_stage_num, curBlock, "stage")
    if(db_stage_num == 0)
    {
        /*
         * No stages - nothing to we can do - continue to the next block
         */
        SHR_EXIT();
    }

    if((pp_block->stages = utilex_alloc(sizeof(pp_stage_t) * db_stage_num)) == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_MEMORY, "No memory for stages\n");
    }
    /*
     * loop through entries
     */
    pp_block->stage_num = 0;
    RHDATA_ITERATOR(curStage, curBlock, "stage")
    {
        char tmp[RHNAME_MAX_SIZE];
        pp_stage_t *pp_stage = &pp_block->stages[pp_block->stage_num];
        /*
         * MS_ID is legitimate number, so we need to initialize it to some illegal one
         */
        pp_stage->ms_id = DSIG_ILLEGAL_MS_ID;
        /*
         * If there is no name - stage is not relevant for chip signal processing
         * Number of stages recorded is ones relevant
         */
        RHDATA_GET_STR_DEF_NULL(curStage, "name", pp_stage->name);
#ifdef BCM_DNX_SUPPORT
        if (SOC_IS_DNX(unit)) {
            if(!ISEMPTY(pp_stage->name) && dnx_pp_stage_string_to_id(unit, pp_stage->name, &pp_stage->pp_id) != 0)
            {
                LOG_INFO(BSL_LS_SOC_COMMON, (BSL_META("Stage:%s is not registered in dnx data\n"), pp_stage->name));
            }
            RHDATA_GET_STR_DEF_NULL(curStage, "adapter", pp_stage->adapter_name);
            if(ISEMPTY(pp_stage->name))
            {
#ifdef ADAPTER_SERVER_MODE
                if(!ISEMPTY(pp_stage->adapter_name))
                {
                    sal_strncpy(pp_stage->name, pp_stage->adapter_name, RHNAME_MAX_SIZE - 1);
                }
                else
#endif
                {  
                    LOG_INFO(BSL_LS_SOC_COMMON, (BSL_META("Stage has no name, ignore\n")));
                    continue;
                }
            }
        }
#endif
        RHDATA_GET_STR_DEF_NULL(curStage, "status", tmp);
        if(!ISEMPTY(tmp))
        {
            if(!sal_strcasecmp(tmp, "global") || !sal_strcasecmp(tmp, "register"))
            {
                pp_stage->status |= SIGNAL_STAGE_PORT_VIS_IGNORE;
            }
            else if(!sal_strcasecmp(tmp, "previous"))
            {
                pp_stage->status |= SIGNAL_STAGE_PREVIOUS;
            }
        }
        pp_block->stage_num++;
    }
    /*
     * Now match substituted stages
     */
    RHDATA_ITERATOR(curStage, curBlock, "stage")
    {
        char stage_n[RHNAME_MAX_SIZE];
        pp_stage_t *pp_stage, *is_subst_pp_stage;

        RHDATA_GET_STR_DEF_NULL(curStage, "substitute", stage_n);
        if(ISEMPTY(stage_n))
            continue;
            /*
             * Substitute exists means that this stage is not represented explicitly in ADAPTER,
             * but rather its signals are provided through substitute stage
             * So look for substitute and save the pointer to this peer inside
             * It should be always in already learned stages above
             */

        if((is_subst_pp_stage = sand_signal_block_stage_find(pp_block, stage_n)) == NULL)
        {
            LOG_INFO(BSL_LS_SOC_COMMON, (BSL_META("Non existing substitute stage:%s\n"), stage_n));
            continue;
        }

        RHDATA_GET_STR_DEF_NULL(curStage, "name", stage_n);
        if((pp_stage = sand_signal_block_stage_find(pp_block, stage_n)) == NULL)
        {
            LOG_INFO(BSL_LS_SOC_COMMON, (BSL_META("Stage to be substituted by:%s has no name\n"),
                                                                                    is_subst_pp_stage->name));
            continue;
        }
        /** Fill adapter name with regular PP name of substitute stage */
        sal_strncpy(pp_stage->adapter_name, pp_stage->name, RHNAME_MAX_SIZE - 1);

        is_subst_pp_stage->status |= SIGNAL_STAGE_IS_SUBSTITUTE;
        is_subst_pp_stage->peer_stage = pp_stage;
        pp_stage->status |= SIGNAL_STAGE_HAS_SUBSTITUTE;
        pp_stage->peer_stage = is_subst_pp_stage;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
sand_signal_init(
    int        unit,
    device_t * device)
{
    void *curTop, *curSubTop, *curBlock;
    debug_signal_t *debug_signal;
    int i, i_block = 0;

    pp_block_t *cur_pp_block;

    SHR_FUNC_INIT_VARS(unit);

    if (device == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "No device for:%s(%d)\n", soc_dev_name(unit), unit);
    }

    if (device->pp_blocks != NULL)
    { /* Already initialized, success */
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(sand_signal_struct_init(unit, device));

    if ((curTop = dbx_file_get_xml_top(unit, "PP.xml", "top", CONF_OPEN_ALTER_LOC | CONF_OPEN_PER_DEVICE)) == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "No PP Scheme found for:%s\n", soc_dev_name(unit));
    }

    SHR_IF_ERR_EXIT(sand_signal_phydb_init(unit, device, curTop));

    if ((curSubTop = dbx_xml_child_get_first(curTop, "block-list")) == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "No block-list in PP Scheme for:%s\n", soc_dev_name(unit));
    }

    RHDATA_GET_NODE_NUM(device->block_num, curSubTop, "block");
    if(device->block_num == 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "No blocks in PP Scheme for:%s\n", soc_dev_name(unit));
    }

    if((device->pp_blocks = utilex_alloc(sizeof(pp_block_t) * device->block_num)) == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_MEMORY, "No memory for %s Blocks Data\n", soc_dev_name(unit));
    }
    /*
     * loop through entries
     */
    RHDATA_ITERATOR(curBlock, curSubTop, "block")
    {
        /*
         * Verify that we are inside the limitations MAX_NUM
         */
        cur_pp_block = &device->pp_blocks[i_block++];
        RHDATA_GET_STR_CONT(curBlock, "name", cur_pp_block->name);
        RHDATA_GET_STR_DEF_NULL(curBlock, "debug-signals", cur_pp_block->debug_signals_n);

        /*
         * Mechanism of building stage list is different for adapter and chip
         * In adapter we take the list from server, check presence of adapter name in PP.XML and assign stages to
         * block accordingly. Signals will be assigned per stage
         * In chip we just read the list, signals will be assigned per block
         */
        SHR_IF_ERR_EXIT(sand_signal_get_stages(unit, cur_pp_block, curBlock));
        {
            void *curSignalTop, *curSignal, *cur;
            int size =0;
            char full_address[DSIG_ADDRESS_MAX_SIZE];
            char temp[RHNAME_MAX_SIZE];

            if(cur_pp_block->stage_num == 0)
                continue;

            if (ISEMPTY(cur_pp_block->debug_signals_n))     /* No debug signals for this block */
                continue;

            if ((curSignalTop = dbx_file_get_xml_top(unit, cur_pp_block->debug_signals_n, "SignalInfo",
                                                                  CONF_OPEN_ALTER_LOC | CONF_OPEN_PER_DEVICE)) == NULL)
            {
                SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "No Signal Info found in:%s\n", cur_pp_block->debug_signals_n);
            }

            RHDATA_GET_NODE_NUM(cur_pp_block->block_signal_num, curSignalTop, "Signal");
            /*
             * No debug signals for this block
             */
            if (cur_pp_block->block_signal_num == 0)
                continue;

            if((debug_signal = utilex_alloc(sizeof(debug_signal_t) * cur_pp_block->block_signal_num)) == NULL)
            {
                SHR_ERR_EXIT(_SHR_E_MEMORY, "No memory for debug signals\n");
            }

            cur_pp_block->block_debug_signals = debug_signal;

            /*
             * loop through entries
             */
            RHDATA_ITERATOR(curSignal, curSignalTop, "Signal")
            {
                dbx_xml_child_get_content_str(curSignal, "Name", debug_signal->hw, RHSTRING_MAX_SIZE);
                dbx_xml_child_get_content_str(curSignal, "From", debug_signal->from, RHNAME_MAX_SIZE);
                dbx_xml_child_get_content_str(curSignal, "To", debug_signal->to, RHNAME_MAX_SIZE);
                dbx_xml_child_get_content_int(curSignal, "Size", &debug_signal->size);
                dbx_xml_child_get_content_int(curSignal, "BlockID", &debug_signal->block_id);
                dbx_xml_child_get_content_str(curSignal, "Attribute", debug_signal->attribute, RHNAME_MAX_SIZE);
#ifdef ADAPTER_SERVER_MODE
                dbx_xml_child_get_content_str(curSignal, "Adapter", debug_signal->adapter_name, RHNAME_MAX_SIZE);
#endif
                debug_signal->stage_from = sand_signal_stage_get(unit, device, cur_pp_block, debug_signal->from);
                if(ISEMPTY(debug_signal->from))
                {
                    LOG_INFO(BSL_LS_SOC_COMMON, (BSL_META("Block:%s Signal:%s - 'From Stage' is empty\n"),
                                                                        cur_pp_block->name, debug_signal->attribute));
                }
                else if(debug_signal->stage_from == NULL)
                {
                    LOG_INFO(BSL_LS_SOC_COMMON, (BSL_META("Block:%s Signal:%s - From Stage:%s is not in PP Scheme\n"),
                                                      cur_pp_block->name, debug_signal->attribute, debug_signal->from));
                }
                debug_signal->stage_to = sand_signal_stage_get(unit, device, cur_pp_block, debug_signal->to);
                if(ISEMPTY(debug_signal->to))
                {
                    LOG_INFO(BSL_LS_SOC_COMMON, (BSL_META("Block:%s Signal:%s - 'To Stage' is empty\n"),
                                                                        cur_pp_block->name, debug_signal->attribute));
                }
                else if(debug_signal->stage_to == NULL)
                {
                    LOG_INFO(BSL_LS_SOC_COMMON, (BSL_META("Block:%s Signal:%s - To Stage:%s is not in PP Scheme\n"),
                                                      cur_pp_block->name, debug_signal->attribute, debug_signal->to));
                }

                dbx_xml_child_get_content_int(curSignal, "Perm", &debug_signal->perm);
                cur = dbx_xml_child_get_content_str(curSignal, "Condition", debug_signal->cond_attribute, RHNAME_MAX_SIZE);
                if ((cur != NULL) && !ISEMPTY(debug_signal->cond_attribute))
                {
                    RHDATA_GET_INT_DEF(cur, "Value", debug_signal->cond_value, 1);
                }

                dbx_xml_child_get_content_str(curSignal, "Expansion", debug_signal->expansion, RHNAME_MAX_SIZE);
                /*
                 * No explicit expansion - look for implicit one
                 */
                if (!ISEMPTY(debug_signal->expansion))
                {
                    if ((sand_signal_struct_get(device, debug_signal->expansion,
                                                    debug_signal->from, debug_signal->to, cur_pp_block->name)) == NULL)
                    {
                        LOG_INFO(BSL_LS_SOC_COMMON, (BSL_META("No signal expansion:%s from:%s block:%s\n"),
                                                  debug_signal->expansion, debug_signal->from, cur_pp_block->name));
                        SET_EMPTY(debug_signal->expansion);
                    }
                }
                else
                {
                    if ((sand_signal_struct_get(device, debug_signal->attribute,
                                                     debug_signal->from, debug_signal->to, cur_pp_block->name)) != NULL)
                        /*
                         * If there is match - attribute serves as expansion
                         */
                        sal_strncpy(debug_signal->expansion, debug_signal->attribute, RHNAME_MAX_SIZE - 1);
                }

                dbx_xml_child_get_content_str(curSignal, "Resolution", debug_signal->resolution, RHNAME_MAX_SIZE);
                if (!ISEMPTY(debug_signal->resolution))
                {
                    if ((sand_signal_resolve_get(device, debug_signal->resolution,
                                                    debug_signal->from, debug_signal->to, cur_pp_block->name)) == NULL)
                    {
                        LOG_INFO(BSL_LS_SOC_COMMON, (BSL_META("Signal resolution:%s does not exist\n"),  debug_signal->resolution));
                        SET_EMPTY(debug_signal->resolution);
                    }
                }
                else
                {
                    if ((sand_signal_resolve_get(device, debug_signal->attribute,
                                                    debug_signal->from, debug_signal->to, cur_pp_block->name)) != NULL)
                        /*
                         * If there is match - attribute serves as resolution param name
                         */
                        sal_strncpy(debug_signal->resolution, debug_signal->attribute, RHNAME_MAX_SIZE - 1);
                }

                cur = dbx_xml_child_get_content_str(curSignal, "Double", temp, RHNAME_MAX_SIZE);
                if ((cur != NULL) && !ISEMPTY(temp))
                    debug_signal->double_flag = 1;

                dbx_xml_child_get_content_str(curSignal, "Address", full_address, DSIG_ADDRESS_MAX_SIZE);
                if(SOC_IS_DPP(unit))
                {
                    debug_signal->range_num = sand_signal_address_parse(full_address, debug_signal->address);
                    /*
                     * Verify consistency between size and sum of all bits in range
                     */
                    size = 0;
                    for (i = 0; i < debug_signal->range_num; i++)
                        size += (debug_signal->address[i].msb + 1 - debug_signal->address[i].lsb);
                }
                else if(SOC_IS_DNX(unit))
                {
                    /*
                     * address[0] will serve for DNX devices, low will represent bus
                     */
                    sscanf(full_address, "{bus:%d} bits: [%d : %d]", &debug_signal->address[0].low,
                                                                      &debug_signal->address[0].msb,
                                                                      &debug_signal->address[0].lsb);
                    size = debug_signal->address[0].msb + 1 - debug_signal->address[0].lsb;
                    debug_signal->range_num = 1;
                }
                if (size != debug_signal->size)
                {
                    cli_out("Correcting size for:%s from:%d to %d\n", debug_signal->attribute, debug_signal->size, size);
                    debug_signal->size = size;
                }

                debug_signal++;
            }

            dbx_xml_top_close(curSignalTop);
        }
    }
#ifdef ADAPTER_SERVER_MODE
    /*
     * Update adapter stage ms_id
     */
    SHR_IF_ERR_EXIT(sand_adapter_update_stages(unit, device));
#else
    /*
     * Now we need to extract condition attribute - not in ADAPTER - there are no signals yet
     * We can consider to fetch them in run-time
     */
    for (i = 0; i < device->block_num; i++)
    {
        int j;
        cur_pp_block = &device->pp_blocks[i];
        debug_signal = cur_pp_block->block_debug_signals;
        for (j = 0; j < cur_pp_block->block_signal_num; j++)
        {
            if (!ISEMPTY(debug_signal->cond_attribute))
            {
                if(sand_signal_handle_get(unit, cur_pp_block->name,
                                             debug_signal->from, debug_signal->to,
                                             debug_signal->cond_attribute, &debug_signal->cond_signal) == 0)
                {
                    cli_out("Condition Attribute:%s does not exist for:%s:%s -> %s\n",
                            debug_signal->cond_attribute, cur_pp_block->name, debug_signal->from, debug_signal->to);
                }
            }
            debug_signal++;
        }
    }
#endif

    dbx_xml_top_close(curTop);
exit:
    SHR_FUNC_EXIT;
}

void
sand_signal_deinit(
    int        unit,
    device_t * device)
{
    int i_ind, i_st;

    if (device == NULL)
    {
        goto exit;
    }

    if (device->pp_blocks == NULL)
    { /* Not initialized, leave peacefully */
        goto exit;
    }

    sand_signal_struct_deinit(device);

    for(i_ind = 0; i_ind < device->block_num; i_ind++)
    {
        if(device->pp_blocks != NULL)
        {
            for(i_st = 0; i_st < device->pp_blocks[i_ind].stage_num; i_st++)
            {
                if(device->pp_blocks[i_ind].stages != NULL)
                {
                    utilex_free(device->pp_blocks[i_ind].stages[i_st].stage_debug_signals);
                }
            }
            utilex_free(device->pp_blocks[i_ind].stages);
            utilex_free(device->pp_blocks[i_ind].block_debug_signals);
        }
    }
    utilex_free(device->pp_blocks);
    device->block_num = 0;

    if(device->phydb_list != NULL)
    {
        utilex_rhlist_free_all(device->phydb_list);
    }
    device->phydb_list = NULL;
    device->pp_blocks = NULL;

exit:
    return;
}

/*
 * }
 */
#endif /* else of (!defined(NO_FILEIO) && !defined(STATIC_SIGNAL_TEST)) */

shr_error_e
sand_signal_reread(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

#if !defined(NO_FILEIO) && !defined(STATIC_SIGNAL_TEST)
    {
        device_t *device;
        if ((device = sand_signal_device_get(unit)) == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "");
        }
        /* DEInitialize Signals Data */
        sand_signal_deinit(unit, device);

        /* Initialize Access Objects */
        SHR_IF_ERR_EXIT(sand_signal_init(unit, device));
    }
exit:
#endif /* (!defined(NO_FILEIO) && !defined(STATIC_SIGNAL_TEST)) */
    SHR_FUNC_EXIT;
}

