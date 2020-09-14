/** \File: sand_signals.c
 * Signal Driver - init from XML DB, access and filtering
 */
/*
 * $Id: Exp $
 * $Copyright: (c) 2020 Broadcom.
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

#include <shared/utilex/utilex_rhlist.h>
#include <shared/utilex/utilex_str.h>
#include <shared/utilex/utilex_framework.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
#include <shared/shrextend/shrextend_debug.h>

#include <bcm/types.h>

#ifdef BCM_PETRA_SUPPORT
#include <soc/dpp/dpp_config_defs.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_diag.h>
#endif

#ifdef BCM_DNX_SUPPORT
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dbal/dbal_string_apis.h>
#include <soc/dnx/pp_stage.h>
#include <soc/dnx/dnx_visibility.h>
#include <soc/dnx/dbal/dbal_string_mgmt.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>
#endif

#include "sand_signals_internal.h"
#include <appl/diag/sand/diag_sand_utils.h>

#define BSL_LOG_MODULE BSL_LS_SOC_COMMON

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
                sal_strncat_s(value_n, digital_value_str, DSIG_MAX_SIZE_STR);
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

static shr_error_e
sand_signal_get_value(
    int unit,
    int core_id,
    int flags,
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
    if (!(flags & SIGNALS_MATCH_PEM)
        || (sal_strcasecmp(debug_signal->to, "PEM") && sal_strcasecmp(debug_signal->from, "PEM")))
    {
        sal_memcpy(value, debug_signal->adapter_value, sizeof(uint32) * uint_num);
    }
    else
#endif
    {
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
            uint32 reg_val[MAX_DEBUG_BUS_SIZE_WORDS] = {0};
            uint32 *source_value;
            int offset = debug_signal->address[0].lsb;
            switch(debug_signal->block_id)
            {
                case DBAL_ENUM_FVAL_PP_ASIC_BLOCK_TABLE:
                    /* Do nothing - no value required for this type of signals - will be fetched from DBAL */
                    SHR_EXIT();
                case DBAL_ENUM_FVAL_PP_ASIC_BLOCK_REGISTER:
#ifdef DNX_EMULATION
                    /*
                     * To speed up Signals read on Emulation, skip register signals
                     */
                    SHR_SET_CURRENT_ERR(_SHR_E_EMPTY);
                    break;
#else
                    source_value = reg_val;
                    /*
                     * Signal is associated with debug register field and source_value will point on it
                     */
                    SHR_SET_CURRENT_ERR(dnx_debug_reg_read(unit, core_id, debug_signal->hw, debug_signal->size,
                                                                                                            source_value));
                    break;
#endif
                case DBAL_ENUM_FVAL_PP_ASIC_BLOCK_TO_ENGINE:
                case DBAL_ENUM_FVAL_PP_ASIC_BLOCK_FROM_ENGINE:
                {
                    
                    int direction = (debug_signal->block_id == DBAL_ENUM_FVAL_PP_ASIC_BLOCK_TO_ENGINE) ? 0 : 1;
                    int size_copied = 0;
                    int range_idx;

                    for(range_idx = 0; range_idx < debug_signal->range_num; range_idx++)
                    {
                        int window_lsb = debug_signal->address[range_idx].lsb / DNX_PEMA_WINDOW_SIZE;
                        int window_msb = debug_signal->address[range_idx].msb / DNX_PEMA_WINDOW_SIZE;
                        int size = debug_signal->address[range_idx].msb - debug_signal->address[range_idx].lsb + 1;

                        offset = debug_signal->address[range_idx].lsb - window_lsb * DNX_PEMA_WINDOW_SIZE;
                        source_value = reg_val;
                        /*
                         * Due to the fact that we put inside value first relevant window - substract this window offset
                         */
                        do
                        {
                            SHR_IF_ERR_EXIT(dnx_debug_pema_read(unit, core_id, debug_signal->block_id, direction, debug_signal->address[0].low,
                                                                                                    window_lsb, source_value));
                            window_lsb++;
                            source_value += BITS2WORDS(DNX_PEMA_WINDOW_SIZE);
                        } while(window_lsb <= window_msb);

                        SHR_BITCOPY_RANGE(value, size_copied, reg_val, offset, size);
                        size_copied += size;
                    }
                    SHR_EXIT();
                }
                default:
                    /*
                     * Fetch cache pointer to relevant line
                     */
                    SHR_SET_CURRENT_ERR(dnx_debug_mem_read(unit, core_id, debug_signal->block_id, debug_signal->address[0].low,
                                                                                                 &source_value));
                    break;
            }
            if(SHR_FUNC_VAL_IS(_SHR_E_NONE))
            {
                /*
                 * Select requested bits
                 */
                if(offset + debug_signal->size > MAX_DEBUG_BUS_SIZE_BITS)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Signal:%s offset does not fit signal data size\n", debug_signal->attribute);
                }
                SHR_BITCOPY_RANGE(value, 0, source_value, offset, debug_signal->size);
            }
        }
#endif /* BCM_DNX_SUPPORT */
    }
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

shr_error_e
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
    else if(type == SAL_FIELD_TYPE_IP6)
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
    int core,
    device_t * device,
    int flags,
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
                        /*
                         * Check for further usage dynamic expansion for field
                         * In case of dynamic expansion take its resolution value for resolution of field value to string
                         */
                        sigstruct_t *sigstruct_exp4field;
                        sigstruct_exp4field = sand_signal_expand_get(unit, core, device, flags, sigstruct, from_n, to_n, block_n,
                                                                     signal_value, &sigstruct_field->expansion_m);

                        if (sigstruct_exp4field)
                        {
                            sal_strncpy(sigstruct_field->resolution, sigstruct_exp4field->resolution, RHNAME_MAX_SIZE-1);
                        }

                        res = sand_signal_param_get(unit, device, sigstruct_field->resolution, from_n, to_n, block_n,
                                                                                        *cur_field_value, field_str, 0);
                        if (res != _SHR_E_NONE)
                            sal_snprintf(field_str, RHNAME_MAX_SIZE - 1,"%u", *cur_field_value);
                    }
                    res = _SHR_E_NONE;
                }
                else
                {
                    res = sand_signal_field_get(unit, core, device, flags, sigstruct_field->expansion_m.name,
                                            from_n, to_n, block_n, cur_field_value, tokens[1], field_str, field_value);
                }
                break;
            }
        }
    }

    utilex_str_split_free(tokens, realtokens);

    return res;
}

#ifdef BCM_DNX_SUPPORT
#define CONTEXT_SIG_SIZE            6
#define CONTEXT_DBAL_FIELD_VT1      "VT1_CONTEXT_ID"
#define CONTEXT_DBAL_FIELD_VT2      "VT2_CONTEXT_ID"
#define CONTEXT_DBAL_FIELD_VT3      "VT3_CONTEXT_ID"
#define CONTEXT_DBAL_FIELD_VT4      "VT4_CONTEXT_ID"
#define CONTEXT_DBAL_FIELD_VT5      "VT5_CONTEXT_ID"
#define CONTEXT_DBAL_FIELD_FWD1     "FWD1_CONTEXT_ID"
#define CONTEXT_DBAL_FIELD_FWD2     "FWD2_CONTEXT_ID"

shr_error_e
nasid_context_decode(
    int unit,
    int core,
    char *desc,
    uint32 *data,
    uint32 data_size_bits,
    rhlist_t ** parsed_info,
    char from_n[RHNAME_MAX_SIZE],
    char to_n[RHNAME_MAX_SIZE])
{
    /** the signal element that was added to the context rhlist due to the context parsing */
    signal_output_t *context_parse_sig = NULL;
    signal_output_t *nasid_sig = NULL;
    char context_parse_string[DSIG_MAX_SIZE_STR] = "";
    char context_value_string[DSIG_MAX_SIZE_STR] = "";
    char context_nasid_dbal_field[DSIG_MAX_SIZE_STR] = "";
    dnx_pp_stage_e current_stage_id;
    int pem_stage_id_reverse = 0; /** in case signal is from or to PEM, this is as if we look from a previous stage to current contexts */
    dnx_pp_stage_e chosen_context_stage_id;
    char stage_str[RHNAME_MAX_SIZE] = "";

    SHR_FUNC_INIT_VARS(unit);

    /*
     * In case from_n or to_n are NULL - skip this callback
     */
    if ((from_n == NULL) || (to_n == NULL))
    {
        SHR_EXIT();
    }

    /*
     * Sanity Context signal size given validation
     */
    if (data_size_bits != CONTEXT_SIG_SIZE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Need %db for Context signal parsing. %db given.",
                     CONTEXT_SIG_SIZE, data_size_bits);
    }

    /** Treat with PEM signal which stage to use */
    if (sal_strstr(from_n ,"PEM"))
    {
        sal_strncpy_s(stage_str, to_n, RHNAME_MAX_SIZE-1);
        pem_stage_id_reverse = -1;
    }
    else if (sal_strstr(to_n ,"PEM"))
    {
        sal_strncpy_s(stage_str, from_n, RHNAME_MAX_SIZE-1);
        pem_stage_id_reverse = -1;
    }
    else
    {
        sal_strncpy_s(stage_str, from_n, RHNAME_MAX_SIZE-1);
    }

    /** get current stage that the Context signal was called from  */
    SHR_IF_ERR_EXIT(dnx_pp_stage_string_to_id(unit, stage_str, &current_stage_id));
    current_stage_id += pem_stage_id_reverse;

    /*
     * Get the NASID signal and decide by its value from which stage's DBAL field to do context parsing
     */
    SHR_CLI_EXIT_IF_ERR(sand_signal_output_find(unit, core, SIGNALS_MATCH_NO_FETCH_ALL, "", stage_str, "", "Nasid",
                                               &nasid_sig), "no signal record for NASID in stage %s\n", stage_str);


    switch (nasid_sig->value[0])
    {
        case DBAL_ENUM_FVAL_NASID_NONE:
            /** No NASID - use current Context stage */
            chosen_context_stage_id = current_stage_id;
            break;
            /*
             * For other cases in case NASID ends in a lower Stage, proceed using current stage and choose max
             */
        case DBAL_ENUM_FVAL_NASID_VT1:
            chosen_context_stage_id = UTILEX_MAX(DNX_PP_STAGE_LLR, current_stage_id);
            break;
        case DBAL_ENUM_FVAL_NASID_VT2:
            chosen_context_stage_id = UTILEX_MAX(DNX_PP_STAGE_VTT1, current_stage_id);
            break;
        case DBAL_ENUM_FVAL_NASID_VT3:
            chosen_context_stage_id = UTILEX_MAX(DNX_PP_STAGE_VTT2, current_stage_id);
            break;
        case DBAL_ENUM_FVAL_NASID_VT4:
            chosen_context_stage_id = UTILEX_MAX(DNX_PP_STAGE_VTT3, current_stage_id);
            break;
        case DBAL_ENUM_FVAL_NASID_VT5:
            chosen_context_stage_id = UTILEX_MAX(DNX_PP_STAGE_VTT4, current_stage_id);
            break;
        case DBAL_ENUM_FVAL_NASID_FWD1:
            chosen_context_stage_id = UTILEX_MAX(DNX_PP_STAGE_VTT5, current_stage_id);
            break;
        case DBAL_ENUM_FVAL_NASID_FWD2:
            chosen_context_stage_id = UTILEX_MAX(DNX_PP_STAGE_IFWD1, current_stage_id);
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_FAIL, "Nasid stage:%d is not supported in Context-Nasid parsing\n", nasid_sig->value[0]);
            break;
    }

    /*
     * Trasnform chosen Context stage to its corresponding DBAL field to do parsing on Context value
     */
    switch (chosen_context_stage_id)
    {
        case DNX_PP_STAGE_LLR:
            sal_strncpy_s(context_nasid_dbal_field, CONTEXT_DBAL_FIELD_VT1, DSIG_MAX_SIZE_STR-1);
            break;
        case DNX_PP_STAGE_VTT1:
            sal_strncpy_s(context_nasid_dbal_field, CONTEXT_DBAL_FIELD_VT2, DSIG_MAX_SIZE_STR-1);
            break;
        case DNX_PP_STAGE_VTT2:
            sal_strncpy_s(context_nasid_dbal_field, CONTEXT_DBAL_FIELD_VT3, DSIG_MAX_SIZE_STR-1);
            break;
        case DNX_PP_STAGE_VTT3:
            sal_strncpy_s(context_nasid_dbal_field, CONTEXT_DBAL_FIELD_VT4, DSIG_MAX_SIZE_STR-1);
            break;
        case DNX_PP_STAGE_VTT4:
            sal_strncpy_s(context_nasid_dbal_field, CONTEXT_DBAL_FIELD_VT5, DSIG_MAX_SIZE_STR-1);
            break;
        case DNX_PP_STAGE_VTT5:
            sal_strncpy_s(context_nasid_dbal_field, CONTEXT_DBAL_FIELD_FWD1, DSIG_MAX_SIZE_STR-1);
            break;
        case DNX_PP_STAGE_IFWD1:
            sal_strncpy_s(context_nasid_dbal_field, CONTEXT_DBAL_FIELD_FWD2, DSIG_MAX_SIZE_STR-1);
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_FAIL, "current PP stage:%d is not supported in Context-Nasid parsing\n", chosen_context_stage_id);
            break;
    }

    /*
     * Activate DBAL parsing
     */
    SHR_IF_ERR_EXIT(dbal_decode(unit, SOC_CORE_DEFAULT, context_nasid_dbal_field, data, CONTEXT_SIG_SIZE, parsed_info, NULL, NULL));

    if (NULL == *parsed_info)
    {
        /*
         * Decoding is unsuccessful. Return an error.
         */
        SHR_ERR_EXIT(_SHR_E_FAIL, "Could not find decoding for dbal field: %s and data: %d\n", context_nasid_dbal_field, *data);
    }

    /*
     * Modify the Context print_value to include the Context DBAL field to the string of the parsing
     */
    context_parse_sig =  utilex_rhlist_entry_get_first(*parsed_info);
    sal_strncpy_s(context_parse_string, context_parse_sig->print_value, DSIG_MAX_SIZE_STR-1);
    sal_strncpy_s(context_parse_sig->print_value, context_nasid_dbal_field, DSIG_MAX_SIZE_STR-1);
    sal_strncat(context_parse_sig->print_value, "_", DSIG_MAX_SIZE_STR-1);
    sal_strncat(context_parse_sig->print_value, context_parse_string, DSIG_MAX_SIZE_STR-1);
    sal_strncat(context_parse_sig->print_value, "(0x", DSIG_MAX_SIZE_STR-1);
    sal_itoa(context_value_string, data[0], 16, 0, 1);
    sal_strncat(context_parse_sig->print_value, context_value_string, DSIG_MAX_SIZE_STR-1);
    sal_strncat(context_parse_sig->print_value, ")", DSIG_MAX_SIZE_STR-1);

exit:
    SHR_FUNC_EXIT;
}
#endif


static shr_error_e
sand_signal_option_get(
    int unit,
    int core,
    device_t *device,
    int flags,
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

    if(sand_signal_handle_get(unit, 0, block_n, from_n, NULL, tokens[0], &debug_signal) != _SHR_E_NONE)
    {
        /*
         * No such signal, check if it is internal field in the structure
         */
        res = sand_signal_field_get(unit, core, device, flags, sigstruct_n, from_n, to_n, block_n,
                                                                            base_value, param_name, value_str, NULL);
    }
    else
    {
        uint32 cond_value[DSIG_MAX_SIZE_UINT32];
        /*
         * Debug Signal found, get it value
         */
        sand_signal_get_value(unit, core, flags, debug_signal, cond_value);
        if(realtokens == 1)
        {
            sal_snprintf(value_str, RHNAME_MAX_SIZE - 1, "%d", cond_value[0]);
        }
        else
        {
            res =  sand_signal_field_get(unit, core, device, flags, tokens[0], from_n, to_n, block_n,
                                                                               cond_value, tokens[1], value_str, NULL);
        }
    }

exit:
    utilex_str_split_free(tokens, realtokens);
    return res;
}

sigstruct_t *
sand_signal_expand_get(
    int     unit,
    int     core,
    device_t * device,
    int flags,
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
        if((!ISEMPTY(option->from) && sal_strcasecmp(option->from, from_n)) ||
           (!ISEMPTY(option->block) && sal_strcasecmp(option->block, block_n)))
        {
            option++;
            continue;
        }
        while ((n < DSIG_OPTION_PARAM_MAX_NUM) && !ISEMPTY(option->param[n].name))
        {
            char value_str[RHNAME_MAX_SIZE];
            if((sand_signal_option_get(unit, core, device, flags, RHNAME(sigstruct_in), from_n, to_n, block_n, value,
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

static shr_error_e
sand_signal_tree_match(
    int unit,
    rhlist_t * field_list,
    int flags,
    char *match_n,
    signal_output_t ** target_signal_output_p)
{
    signal_output_t *field_output;
    char *current_match_n;
    char **tokens = NULL;
    uint32 realtokens = 0, maxtokens = 2;

    SHR_FUNC_INIT_VARS(unit);

    SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);

    if ((tokens = utilex_str_split(match_n, ".", maxtokens, &realtokens)) == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Problem parsing match string:%s\n", match_n);
    }

    /* If there is no dot in match_n - match_n is considered as signal name */
    if(realtokens == 1)
    {
        current_match_n = match_n;
    }
    else if (realtokens == maxtokens)
    {
        current_match_n = tokens[0];
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Bad number:%d of tokens for:%s\n", realtokens, match_n);
    }

    {
        RHSAFE_ITERATOR(field_output, field_list)
        {
            int match_result;
            /**
             * We need down_match only if match_status is FALSE
             */
            char *down_match_n;
            if ((current_match_n == NULL)   /* No match string on input */
                || ((flags & SIGNALS_MATCH_EXACT) && !sal_strcasecmp(RHNAME(field_output), current_match_n))
                || (!(flags & SIGNALS_MATCH_EXACT) && (sal_strcasestr(RHNAME(field_output), current_match_n) != NULL)))
            {
                /*
                 * We have a match on this level - If there is no down match string - search is complete
                 */
                if(realtokens == 1)
                {
                    match_result = TRUE;
                }
                else
                {
                    match_result = FALSE;
                    down_match_n = tokens[1];
                }
            }
            else
            {
                match_result = FALSE;
                if(flags & SIGNALS_MATCH_STRICT)
                {
                    /**
                     * For strict search we failed, so we may free the field,
                     * down_match_n == NULL will prevent going deeper
                     */
                    down_match_n = NULL;
                }
                else
                {
                    /**
                     * We need to continue filter in the tree.
                     * signal_match_n was not found, so use match_n for down search
                     */
                    down_match_n = match_n;
                }
            }

            if(match_result == TRUE)
            {   /** No need to dig deeper, but do not leave yet - may be we need to filter other entries on the list */
                SHR_SET_CURRENT_ERR(_SHR_E_NONE);
                /**
                 * If user requested leaf pointer - remove it from the tree and keep under target_signal_output_p
                 * Do it once and we may leave the match after
                 * Pointer not NULL here - logical error, filling the pointer should trigger return to the top
                 */
                if((flags & SIGNALS_MATCH_LEAF) && (target_signal_output_p != NULL))
                {
                    if(*target_signal_output_p == NULL)
                    {
                        utilex_rhlist_entry_remove(field_list, field_output);
                        *target_signal_output_p = field_output;
                        SHR_EXIT();
                    }
                    else
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM, "output_signal pointer not empty for:%s\n", match_n);
                    }
                }
            }
            else
            {
                /*
                 * No match yet - there are still tokens to look for - go inside
                 * For strict search any missing level is unacceptable - no need to look deeper
                 */
                if((down_match_n != NULL) && (sand_signal_tree_match(unit, field_output->field_list,
                                                         flags, down_match_n, target_signal_output_p) == _SHR_E_NONE))
                {
                    SHR_SET_CURRENT_ERR(_SHR_E_NONE);
                    if((target_signal_output_p != NULL) && (*target_signal_output_p != NULL))
                    {
                        /*
                         * We are done, somebody downstairs found the matching leaf and returned its pointer
                         * No need to look further for more matching entries,
                         * Entire object will be freed by highest caller
                         */
                        SHR_EXIT();
                    }
                    /*
                     * Else if return is SUCCESS - just continue on the list
                     */
                }
                else
                {
                    /*
                     * No match on this branch - delete list from the field, field from parent list and
                     * continue with search
                     */
                    sand_signal_list_free(field_output->field_list);
                    utilex_rhlist_entry_del_free(field_list, field_output);
                }
            }
        }
    }
exit:
    utilex_str_split_free(tokens, realtokens);
    SHR_FUNC_EXIT;
}

static int
sand_signal_tree_build(
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
    int flags,
    sigstruct_t **sigstruct_p,
    rhlist_t **field_list_p)
{
    int size = 0;              /* By default no match - we yet need to find one, and it will be size */
    sigstruct_t *sigstruct = NULL;

   /*
     * Check if there is an expansion at all
     */
    if (ISEMPTY(sigstruct_n) ||
                            (sigstruct = sand_signal_struct_get(device, sigstruct_n, from_n, to_n, block_n)) == NULL)
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
        if(sigstruct->expansion_cb(unit, core, sigstruct->expansion_m.name, signal_value, size_in,
                                                                                        field_list_p, from_n, to_n) != _SHR_E_NONE)
        {
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META("Failure expanding on plugin for:%s\n"), RHNAME(sigstruct)));
            goto exit;
        }
        size = size_in;
    }
    else
    {
        sigstruct_t *sigstruct_exp, *sigstruct_exp4field;
        sigstruct_field_t *sigstruct_field;
        uint32 field_value[DSIG_MAX_SIZE_UINT32];
        char *field_dyn_name = NULL;
        signal_output_t *field_output;
        rhhandle_t temp = NULL;
        rhlist_t *field_list;

        /*
         * In case of dynamic expansion find the matching expansion and switch to it
         */
        if ((sigstruct_exp = sand_signal_expand_get(unit, core, device, flags, sigstruct, from_n, to_n, block_n,
                                                    signal_value, &sigstruct->expansion_m)) != NULL)
        {
            sigstruct = sigstruct_exp;
        }
        /*
         * Create the list
         */
        if ((field_list = utilex_rhlist_create("field_list", sizeof(signal_output_t), 0)) == NULL)
        {
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META("Failed to create field list for:%s\n"), RHNAME(sigstruct)));
            goto exit;
        }

        RHITERATOR(sigstruct_field, sigstruct->field_list)
        {
            int value_status = TRUE;
            /*
             * If there is a condition check that it is fulfilled. No need to check if value was not provided
             */
            if (!(flags & SIGNALS_MATCH_NOCOND) && !ISEMPTY(sigstruct_field->cond_attribute) && (signal_value != NULL))
            {
                sand_signal_field_get(unit, core, device, flags, RHNAME(sigstruct),
                                   from_n, to_n, block_n, signal_value, sigstruct_field->cond_attribute, NULL, field_value);
                if (0 == VALUE(field_value))
                {
                    /** From this place and below all fields values are irrelevant */
                    value_status = FALSE;
                }
            }
            /*
             * Check for further usage dynamic expansion for field
             * In case of dynamic expansion find the matching expansion and switch to it - in this case for field
             */
            sigstruct_exp4field = sand_signal_expand_get(unit, core, device, flags, sigstruct, from_n, to_n, block_n,
                                                         signal_value, &sigstruct_field->expansion_m);

            /** in case of layer type resolution - in case of expansion name of in layer_type 'Main_resolution' - remove name from appearing in print */
            if (sigstruct_exp4field)
                if (sal_strcasecmp(RHNAME(sigstruct_exp4field), "Main_resolution") == 0)
                    field_dyn_name = NULL;
                else
                    field_dyn_name = RHNAME(sigstruct_exp4field);
            else
                field_dyn_name = sigstruct_field->expansion_m.name;

            sal_memset(field_value, 0, DSIG_MAX_SIZE_BYTES);

            size = sigstruct->size;      /* Once we have match return will be non zero */

            /*
             * Now we can allocate output and fill it
             */
            if (utilex_rhlist_entry_add_tail(field_list, RHNAME(sigstruct_field),
                                                                            RHID_TO_BE_GENERATED, &temp) != _SHR_E_NONE)
            {
                /*
                 * No more place - return with what you have until now
                 */
                break;
            }

            field_output = temp;

            if (field_dyn_name)
            {
                sal_strncpy_s(field_output->expansion, field_dyn_name, sizeof(field_output->expansion) - 1);
            }

            if(signal_value != NULL)
            {
                sand_signal_field_get_value(signal_value, field_value, sigstruct_field->start_bit, sigstruct_field->size);
            }
            sal_memcpy(field_output->value, field_value, DSIG_MAX_SIZE_BYTES);
            field_output->size      = sigstruct_field->size;
            field_output->start_bit = sigstruct_field->start_bit;
            field_output->end_bit   = sigstruct_field->end_bit;

            if(value_status == TRUE)
            {
                sand_signal_value_to_str_full(unit, device, sigstruct_field->type, sigstruct_field->resolution,
                                              from_n, to_n, block_n, flags, field_value,
                                              field_output->print_value, sigstruct_field->size, byte_order);
                if (sigstruct_exp4field)
                {
                    /** if sigstruct_exp4field exists - use its string from value resolution for the field */
                    sand_signal_value_to_str_full(unit, device, sigstruct_field->type, sigstruct_exp4field->resolution,
                                                  from_n, to_n, block_n, flags, field_value,
                                                  field_output->print_value, sigstruct_exp4field->size, byte_order);

                    /*
                     * Check what to do with return status here
                     */
                    sand_signal_tree_build(unit, core, device, field_value, 0, RHNAME(sigstruct_exp4field),
                                           from_n, to_n, block_n, byte_order, flags, NULL, &field_output->field_list);
                }
            }
            else
            {
                sal_strncpy_s(field_output->print_value, "N/A", sizeof(field_output->print_value) - 1);
            }
        }
        *field_list_p = field_list;
    }

exit:
    if(sigstruct_p != NULL)
    {
        *sigstruct_p = sigstruct;
    }
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
    device_t * device;
    rhhandle_t temp = NULL;
    signal_output_t *signal_output;
    sigstruct_t *sigstruct;
    rhlist_t * field_list = NULL;
    int size;

    SHR_FUNC_INIT_VARS(unit);

    if ((device = sand_signal_device_get(unit)) == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_UNIT, "No signal device on unit:%d\n", unit);
    }

    /*
     * Generate list of fields per signal, size if actual expansion size
     */
    if((size = sand_signal_tree_build(unit, 0, device, value, size_in, expansion_n, from_n, to_n, block_n,
                                                                    output_order, 0, &sigstruct, &field_list)) == 0)
    {
        if(sal_strcmp(expansion_n, "Invalid") == 0)
        {
            SHR_EXIT();
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "");
        }
    }
    /*
     * Allocate output and fill it
     */
    SHR_IF_ERR_EXIT(utilex_rhlist_entry_add_tail(dsig_list, expansion_n, RHID_TO_BE_GENERATED, &temp));

    signal_output = temp;

    signal_output->size = size;
    signal_output->field_list = field_list;
    sal_strncpy_s(signal_output->expansion, RHNAME(sigstruct), sizeof(signal_output->expansion) - 1);
    /*
     * Create print value from raw one if value was provided
     */
    if(value != NULL)
    {
        sand_signal_value_to_str_full(unit, device, SAL_FIELD_TYPE_NONE, expansion_n, from_n, to_n, block_n,
                                      0, value, signal_output->print_value, size, output_order);
        SHR_BITCOPY_RANGE(signal_output->value, 0, value, 0, size);
    }

exit:
    SHR_FUNC_EXIT;
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
            LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META("Field:'%s' not found in '%s'\n"),
                                                                        RHNAME(field_output), RHNAME(signal_output)));
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

/*
 * Check if the user asked signal contains a translation for this devices,
 * if so, update the match_p to reflect this signal as called in this device
 */
static shr_error_e
sand_signal_update_signal_translation(
                                     int unit,
                                     match_t * match_p)
{
    translated_t *translated_signal;
    char **tokens = NULL;
    uint32 realtokens = 0;
    device_t*   device;

    char *signal_n;
    char *from_n;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Deal only with well defined specific user given signals - name + from
     */
    if((match_p == NULL) || ISEMPTY(match_p->name) || ISEMPTY(match_p->from))
    {
        SHR_EXIT();
    }

    signal_n = match_p->name;
    from_n = match_p->from;

    if ((device = sand_signal_device_get(unit)) == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Signal DB for:%s(%s) was not initialized\n",
                                     SOC_CHIP_NAME(SOC_CONTROL(unit)->chip_driver->type), SOC_CHIP_STRING(unit));
    }

    if (device->translated_list == NULL)
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

    RHITERATOR(translated_signal, device->translated_list)
    {

        /*
         * If there is specific "from" stage name, only signal from this stage may be matched to it
         */
        if (!ISEMPTY(translated_signal->from_n) && sal_strcasecmp(translated_signal->from_n, from_n))
            continue;
        /*
         * Found the signal that has a translation and matched user signal name and from
         */
        if (!ISEMPTY(RHNAME(translated_signal)) && !sal_strcasecmp(RHNAME(translated_signal), signal_n))
        {
            LOG_WARN(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "Signal %s.%s. is translated on this device to %s.%s. \n"),
                                                    from_n, signal_n, translated_signal->translate_from_n, translated_signal->translate_signal_n));

            /*
             * update the user match_p signal to the translated one and exit
             */
            match_p->name = translated_signal->translate_signal_n;
            match_p->block = translated_signal->translate_block_n;
            match_p->from = translated_signal->translate_from_n;
            match_p->to = translated_signal->translate_to_n;
            SHR_EXIT();
       }
    }

exit:
    utilex_str_split_free(tokens, realtokens);
    SHR_FUNC_EXIT;
}

static shr_error_e
sand_signal_is_restricted(
    int unit,
    char *block_n,
    char *to_n,
    char *signal_n)
{
    restricted_t *restricted_signal;
    char **tokens = NULL;
    uint32 realtokens = 0;
    device_t*   device;

    SHR_FUNC_INIT_VARS(unit);

    if(ISEMPTY(signal_n) || ISEMPTY(block_n) || ISEMPTY(to_n))
    {
        /*
         * Deal only with full block, stage and attribute.
         * Otherwise just leave - we have not enough data to make the decision
         */
        SHR_EXIT();
    }

    if ((device = sand_signal_device_get(unit)) == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Signal DB for:%s(%s) was not initialized\n",
                                     SOC_CHIP_NAME(SOC_CONTROL(unit)->chip_driver->type), SOC_CHIP_STRING(unit));
    }

    if (device->restricted_list == NULL)
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

    RHITERATOR(restricted_signal, device->restricted_list)
    {
        /*
         * If there is specific block name, only signal belonging to this block may be matched to it
         */
        if (!ISEMPTY(restricted_signal->block_n) && sal_strcasecmp(restricted_signal->block_n, block_n))
            continue;
        /*
         * If there is specific "to" stage name, only signal from this stage may be matched to it
         */
        if (!ISEMPTY(restricted_signal->to_n) && sal_strcasecmp(restricted_signal->to_n, to_n))
            continue;
        /*
         * Unsupported may be per stage and/or block - not specific per signal
         */
        if (!ISEMPTY(RHNAME(restricted_signal)) && !sal_strcasecmp(RHNAME(restricted_signal), signal_n))
        {
            LOG_WARN(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "Signal %s.%s.%s is restricted on this device\n"),
                                                                                            block_n, to_n, signal_n));
            SHR_SET_CURRENT_ERR(_SHR_E_UNAVAIL);
            SHR_EXIT();
       }
    }

exit:
    utilex_str_split_free(tokens, realtokens);
    SHR_FUNC_EXIT;
}

/**
 * \brief Check if debug_signal matches filter criteria after passing block and stage filet in sand_signal_get
 * \param [in] device       - pointer to device structure (per device type)
 * \param [in] unit         - unit id - correctness verified by calling routine
 * \param [in] core         - core id - correctness verified by calling routine
 * \param [in] block_n      - block name - provided as separate parameter
 *                                         because there is no block name inside debug_signal
 * \param [in] debug_signal - pointer to debug signal information
 * \param [in] param_p      - pointer to matching info - here name and flags are used
 * \param [out] signal_output_p - under this pointer the pointer to final signal output will be assigned
 *
 * \remark
 *
 * \see
 *  sand_signal_get
 */
static shr_error_e
sand_signal_single_get(
    device_t * device,
    int unit,
    int core,
    char *block_n,
    debug_signal_t *debug_signal,
    match_t * match_p,
    signal_output_t **signal_output_p)
{
    signal_output_t *signal_output = NULL;
    signal_output_t *target_signal_output = NULL;
    uint32 value[DSIG_MAX_SIZE_UINT32];
    int flags = match_p->flags;
    int match_result, value_status = TRUE;
    char *signal_match_n = NULL;
    char *down_match_n = NULL;
    rhlist_t *field_list = NULL;

    char **tokens = NULL;
    uint32 realtokens = 0, maxtokens = 2;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * This is required since this variable is accessed, below, in full (although
     * no use is made with that info). This makes Valgrind happier.
     */
    sal_memset(value,0,sizeof(value));
    /*
     * We set status to not found and change it to success only if pass all the filters
     */
    SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);

    if ((flags & SIGNALS_MATCH_PERM) && (debug_signal->perm == 0))
        SHR_EXIT();

    if(!ISEMPTY(match_p->name))
    {
        tokens = utilex_str_split(match_p->name, ".", maxtokens, &realtokens);
        if ((realtokens > maxtokens) || (tokens == NULL))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Problem parsing match string:%s\n", match_p->name);
        }

        /* If there is no dot in match_n - match_n is considered as signal name */
        if(realtokens == 1)
        {
            signal_match_n = match_p->name;
            down_match_n = NULL;
        }
        else if (realtokens == maxtokens)
        {
            signal_match_n = tokens[0];
            down_match_n = tokens[1];
        }
    }

    if ((signal_match_n == NULL)       /* No match string on input */
        || ((flags & SIGNALS_MATCH_EXACT) &&   /* Match should be exact one */
            (!sal_strcasecmp(debug_signal->attribute, signal_match_n) ||
             ((flags & SIGNALS_MATCH_HW) && !sal_strcasecmp(debug_signal->hw, signal_match_n))))
        || (!(flags & SIGNALS_MATCH_EXACT) &&  /* Match may be any part of name */
           ((sal_strcasestr(debug_signal->attribute, signal_match_n) != NULL) || ((flags & SIGNALS_MATCH_HW) &&
             (sal_strcasestr(debug_signal->hw, signal_match_n) != NULL)))))
    {
        /*
         * If there is no down match string - search is complete
         */
        if(down_match_n == NULL)
        {
            match_result = TRUE;
        }
        else
        {
            match_result = FALSE;
        }
    }
    else
    {
        /*
         * For strict search
         */
        if(flags & SIGNALS_MATCH_STRICT)
        {
            SHR_EXIT();
        }
        else
        {
            /*
             * We need to continue filter in the tree, build it, then cut non-matched branches.
             * signal_match_n was not found, so use original string from match_p for down search
             */
            match_result = FALSE;
            down_match_n = match_p->name;
        }
    }
    /*
     * If double flag set and no MATCH_DOUBLE skip this signal
     */
    if (!(flags & SIGNALS_MATCH_DOUBLE) && (debug_signal->double_flag == 1))
    {
        SHR_EXIT();
    }
    /*
     * If there is a condition check that it is fulfilled, if condition is 0 do not expand, this signal is NOT Available
     */
    if (!(flags & SIGNALS_MATCH_NOVALUE) && (!(flags & SIGNALS_MATCH_NOCOND) && debug_signal->cond_signal))
    {
        sand_signal_get_value(unit, core, flags, debug_signal->cond_signal, value);
        if (VALUE(value) == 0)
        {
            value_status = FALSE;
        }
    }
    /*
     * Fill signal value. On failure do not expand
     */
    if (!(flags & SIGNALS_MATCH_NOVALUE))
    {
        if(sand_signal_get_value(unit, core, flags, debug_signal, value) != _SHR_E_NONE)
        {
            value_status = FALSE;
        }
    }

    /*
     * Check if the signal is not in the N/A restricted list for specific devices, stages, etc due to a bug of some sort
     */
    if(sand_signal_is_restricted(unit, block_n, debug_signal->to ,match_p->name))
    {
        value_status = FALSE;
    }

    if ((flags & SIGNALS_MATCH_EXPAND) && (value_status == TRUE))
    {
        /*
         * If the build tree was not successful, further code will take case in case of no full match
         * Successful matching on the tree will bring match status to success
         */
        if(sand_signal_tree_build(unit, core, device, value, debug_signal->size, debug_signal->expansion,
                                          debug_signal->from, debug_signal->to, block_n,
                                          match_p->output_order, flags, NULL, &field_list) != 0)
        {
            /*
             * We have uncompleted match and tree - we need search the tree to complete the match
             */
            if((match_result == FALSE) &&
                    sand_signal_tree_match(unit, field_list, flags, down_match_n, &target_signal_output) == _SHR_E_NONE)
            {
                match_result = TRUE;
            }
        }
    }
    /*
     * If we had no match success for one reason or another - leave
     */
    if(match_result != TRUE)
    {
        sand_signal_list_free(field_list);
        SHR_EXIT();
    }

    if(target_signal_output != NULL)
    {
        /*
         * Free field_list because target_signal_output was already removed from it
         * when assigning in sand_signal_tree_match
         */
        sand_signal_list_free(field_list);
        signal_output = target_signal_output;
        /*
         * Size, value, print_value, field_list were filled by sand_signal_tree_build
         */
    }
    else
    {
        /*
         * Allocate signal_output
         */
        if((signal_output = sand_signal_output_alloc(debug_signal->attribute)) == NULL)
        {   /** free already allocated child list, that we have no way to preserve */
            SHR_ERR_EXIT(_SHR_E_MEMORY, "Signal Output allocation failed\n");
        }
        /*
         * Copy debug signal into output_signal
         */
        signal_output->field_list = field_list;
        /*
         * Create print value from raw one
         */
        if(value_status == FALSE)
        {
            sal_strncpy_s(signal_output->print_value, "N/A", sizeof(signal_output->print_value) - 1);
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
    }

    signal_output->debug_signal = debug_signal;
    signal_output->core = core;
    signal_output->block_n = block_n;
    signal_output->device = device;

    *signal_output_p = signal_output;
    SHR_SET_CURRENT_ERR(_SHR_E_NONE);

exit:
    utilex_str_split_free(tokens, realtokens);
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
        /** if both from and to are empty, there's nothing to compare to */
        if (ISEMPTY(from_n) && ISEMPTY(to_n))
        {
            return _SHR_E_NOT_FOUND;
        }

        /** Here one of both conditions must be true to compare the attributes */
        if (!ISEMPTY(match_p->from) && !ISEMPTY(from_n) && (sal_strcasestr(from_n, match_p->from) == NULL))
            return _SHR_E_NOT_FOUND;
        if (!ISEMPTY(match_p->to)   && !ISEMPTY(to_n)   && (sal_strcasestr(to_n, match_p->to)     == NULL))
            return _SHR_E_NOT_FOUND;

    }
    return _SHR_E_NONE;
}


shr_error_e
sand_signal_get(
    int unit,
    int core,
    match_t * match_p,
    rhlist_t * dsig_list,
    signal_output_t **signal_output_p)
{
    int i_bl, i_sig;
    device_t * device;
    pp_block_t *cur_pp_block;
    signal_output_t *signal_output = NULL;

    SHR_FUNC_INIT_VARS(unit);

    SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);

    if(dsig_list != NULL)
    {
        if(signal_output_p != NULL)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Both list and signal_ouput pointers are not NULL\n");
        }
    }
    else if(signal_output_p == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Both list and signal_ouput pointers are NULL\n");
    }

    if (((device = sand_signal_device_get(unit)) == NULL) || (device->pp_blocks == NULL))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Signal DB for:%s(%s) was not initialized\n",
                                     SOC_CHIP_NAME(SOC_CONTROL(unit)->chip_driver->type), SOC_CHIP_STRING(unit));
    }

    if (core == _SHR_CORE_ALL)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid Core %d\n", core);
    }

    /*
     * Check if the user asked signal, contains a translation to another signal on this device,
     * if yes, then update the user match_p asked signal to reflect the current specific device,
     * and proceed down
     */

    SHR_IF_ERR_EXIT(sand_signal_update_signal_translation(unit, match_p));


#ifdef BCM_DNX_SUPPORT
    if(!(match_p->flags & SIGNALS_MATCH_NO_FETCH_ALL) && SOC_IS_DNX(unit))
    {
#ifndef ADAPTER_SERVER_MODE
        SHR_IF_ERR_EXIT(dnx_visibility_selective_fetch_all(unit, core));
#endif
        /* Reset PEM caching */
        if(match_p->flags & SIGNALS_MATCH_PEM)
        {
            SHR_IF_ERR_EXIT(dnx_visibility_engine_clear(unit, core));
        }
    }
#endif
    for (i_bl = 0; i_bl < device->block_num; i_bl++)
    {
        cur_pp_block = &device->pp_blocks[i_bl];

        if (!ISEMPTY(match_p->block) && sal_strcasecmp(cur_pp_block->name, match_p->block))
        {
            continue;
        }

#ifdef ADAPTER_SERVER_MODE
        if(!(match_p->flags & SIGNALS_MATCH_ASIC) && !(match_p->flags & SIGNALS_MATCH_PEM))
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

                for (i_sig = 0; i_sig < cur_pp_stage->stage_signal_num; i_sig++)
                {
                    debug_signal_t *debug_signal = &cur_pp_stage->stage_debug_signals[i_sig];
                    if ((sand_signal_single_get(device, unit, core, cur_pp_block->name, debug_signal, match_p,
                                                                                      &signal_output) == _SHR_E_NONE))
                    {
                        if(dsig_list == NULL)
                        {
                            /**
                             * User wants to see single output No need to look anymore
                             * signal_output_p checked at the beginning
                             */
                            *signal_output_p = signal_output;
                            SHR_SET_CURRENT_ERR(_SHR_E_NONE);
                            SHR_EXIT();
                        }
                        else
                        {
                            if(utilex_rhlist_entry_append(dsig_list, signal_output) != _SHR_E_NONE)
                            {
                                /*
                                 * Internal error - return with what you have until now
                                 */
                                SHR_SET_CURRENT_ERR(_SHR_E_INTERNAL);
                                break;
                            }
                            SHR_SET_CURRENT_ERR(_SHR_E_NONE);
                            if(match_p->flags & SIGNALS_MATCH_ONCE)
                            {
                                /* No need to look anymore */
                                SHR_EXIT();
                            }
                        }
                    }
                }
            }  /* stage loop */
        }  /* Adapter context */
        else
#endif
        {
            for (i_sig = 0; i_sig < cur_pp_block->block_signal_num; i_sig++)
            {
                debug_signal_t *debug_signal = &cur_pp_block->block_debug_signals[i_sig];

#if defined(BCM_DNX_SUPPORT) && !defined(ADAPTER_SERVER_MODE)
                /*
                 * Skip register signals
                 */
                if((match_p->flags & SIGNALS_MATCH_NO_REGS) &&
                                                    (debug_signal->block_id >= DBAL_ENUM_FVAL_PP_ASIC_BLOCK_REGISTER))
                {
                    continue;
                }
                /*
                 * Skip pem signals if it was not specifically stated to include them
                 */
                if(!(match_p->flags & SIGNALS_MATCH_PEM) &&
                    ((debug_signal->block_id == DBAL_ENUM_FVAL_PP_ASIC_BLOCK_TO_ENGINE) ||
                     (debug_signal->block_id == DBAL_ENUM_FVAL_PP_ASIC_BLOCK_FROM_ENGINE)))
                {
                    continue;
                }
#endif
                /*
                 * Filter on stage
                 */
                if(sand_signal_filter_stage(debug_signal, debug_signal->stage_from, debug_signal->stage_to, match_p)
                                                                                                        != _SHR_E_NONE)
                    continue;

                if ((sand_signal_single_get(device, unit, core, cur_pp_block->name, debug_signal, match_p,
                                                                                    &signal_output) == _SHR_E_NONE))
                {  /* No need to look anymore */
                    if(dsig_list == NULL)
                    {
                        /**
                         * User wants to see single output No need to look anymore
                         * signal_output_p checked at the beginning
                         */
                        *signal_output_p = signal_output;
                        SHR_SET_CURRENT_ERR(_SHR_E_NONE);
                        SHR_EXIT();
                    }
                    else
                    {
                        if(utilex_rhlist_entry_append(dsig_list, signal_output))
                        {
                            /*
                             * Internal error - return with what you have until now
                             */
                            SHR_SET_CURRENT_ERR(_SHR_E_INTERNAL);
                            break;
                        }
                        SHR_SET_CURRENT_ERR(_SHR_E_NONE);
                        if(match_p->flags & SIGNALS_MATCH_ONCE)
                        {
                            /* No need to look anymore */
                            SHR_EXIT();
                        }
                    }
                }
            }
        }
    }   /* Dynamic Data */

exit:
    SHR_FUNC_EXIT;
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
        SHR_ERR_EXIT(_SHR_E_PARAM, "Signal DB for:%s(%s) was not initialized\n",
                                     SOC_CHIP_NAME(SOC_CONTROL(unit)->chip_driver->type), SOC_CHIP_STRING(unit));
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
            LOG_WARN(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "Signal %s.%s.%s is not supported on this device\n"),
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
    signal_output_t *signal_output = NULL;
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
    match.flags = SIGNALS_MATCH_EXPAND | SIGNALS_MATCH_ONCE | SIGNALS_MATCH_EXACT | SIGNALS_MATCH_RETRY
                                                                                                | SIGNALS_MATCH_LEAF;
    /*
     * Create signal description string for log usage
     */
    sand_signal_description(&match, desc_str);

    if (sand_signal_get(unit, core, &match, NULL, &signal_output) != _SHR_E_NONE)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        SHR_EXIT();
    }

    if (size < BITS2WORDS(signal_output->size))
    {
        SHR_EXIT_WITH_LOG(_SHR_E_RESOURCE, "Not enough memory for signal:%s %d(Words) vs %d(Bits)\n", desc_str, size ,
                                                                                                 signal_output->size);
    }

    sal_memcpy(value, signal_output->value, BITS2WORDS(signal_output->size) * 4);

exit:
    sand_signal_output_free(signal_output);
    SHR_FUNC_EXIT;
}

shr_error_e
sand_signal_output_find(
    int unit,
    int core,
    int extra_flags,
    char *block,
    char *from,
    char *to,
    char *name,
    signal_output_t ** signal_output_p)
{
    match_t match;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(signal_output_p, _SHR_E_PARAM, "signal_output_p");
    /*
     * Not NULL signal_output will be freed, assuming caller have a lot of signals to fetch and using the same pointer
     * to do so. After it to be sure that if the result is not SUCCESS - NULL will be returned in signal_output_p
     */
    sand_signal_output_free(*signal_output_p);
    *signal_output_p = NULL;
    /*
     * Setup match data
     */
    sal_memset(&match, 0, sizeof(match_t));
    match.block = block;
    match.from = from;
    match.to = to;
    match.name = name;
    match.flags = extra_flags | SIGNALS_MATCH_EXPAND | SIGNALS_MATCH_STRICT |
            SIGNALS_MATCH_EXACT | SIGNALS_MATCH_RETRY | SIGNALS_MATCH_ONCE | SIGNALS_MATCH_LEAF;
    /*
     * Find required signal
     */
    SHR_IF_ERR_EXIT_NO_MSG(sand_signal_get(unit, core, &match, NULL, signal_output_p));

exit:
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
    char *result_str,
    int flags)
{
    signal_output_t *signal_output = NULL;
    match_t match;
    char desc_str[RHSTRING_MAX_SIZE];

    SHR_FUNC_INIT_VARS(unit);

    if(((expected_value_a == NULL) || (size == 0)) && (expected_str == NULL))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "No valid output pointers\n");
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
    match.flags = SIGNALS_MATCH_EXPAND | SIGNALS_MATCH_ONCE | SIGNALS_MATCH_EXACT | SIGNALS_MATCH_RETRY
                                                                                                | SIGNALS_MATCH_LEAF | flags;
    /*
     * Create signal description string for log usage
     */
    sand_signal_description(&match, desc_str);

    if (sand_signal_get(unit, core, &match, NULL, &signal_output) != _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Signal:%s not found\n", desc_str);
    }
    if (expected_value_a != NULL)
    {
        if (size < BITS2WORDS(signal_output->size))
        {
            SHR_ERR_EXIT(_SHR_E_RESOURCE, "Not enough memory for signal:%s %d vs %d\n", desc_str, size,
                                                                                              signal_output->size);
        }
        if (sal_memcmp(expected_value_a, signal_output->value, WORDS2BYTES(BITS2WORDS(signal_output->size))) != 0)
        {
            char print_value[DSIG_MAX_SIZE_STR];
            sand_signal_value_to_str(unit, SAL_FIELD_TYPE_NONE, expected_value_a, print_value,
                                                                signal_output->size, PRINT_BIG_ENDIAN);
            if(result_str == NULL)
            {   /* Once place to return non-matched value was not provided, print an error */
                LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(NO_UNIT,
                                                    "For signal:%s result:%s does not match expected %s\n"),
                                                    desc_str, signal_output->print_value, print_value));
            }
            else
            {
                /*
                 * Otherwise copy the result.
                 * Do not copy more than the size of the output container.
                 * Make sure to set an ending NULL on output string.
                 */
                sal_strncpy_s(result_str, signal_output->print_value, WORDS2BYTES(size));
            }
            SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
            SHR_EXIT();
        }
    }
    else if (expected_str != NULL)
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
        if(signal_output->size <= 32)
        {   /* No support here for signals more than uint32 */
            expected_value_status = sand_signal_str_to_value(unit, SAL_FIELD_TYPE_UINT32,
                                                        expected_str, signal_output->size,  &expected_value);
        }
        /*
         * Print value may contain number in brackets after resolved name. Make it go - replacing by 0
         */
        utilex_str_replace(signal_output->print_value, '(', 0);
        if ((sal_strcasecmp(expected_str, signal_output->print_value) != 0) &&
            ((expected_value_status != _SHR_E_NONE ||
            (sal_memcmp(&expected_value, signal_output->value, WORDS2BYTES(BITS2WORDS(signal_output->size))) != 0) )))
        {
            if (result_str == NULL)
            {   /* Once place to return non-matched value was not provided, print an error  */
                LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(NO_UNIT,
                                                    "For signal:%s result:%s does not match expected %s\n"),
                                                    desc_str, signal_output->print_value, expected_str));
            }
            else
            {
                /*
                 * Otherwise copy the result.
                 * Do not copy more than the size of the output container.
                 * Make sure to set an ending NULL on output string.
                 */
                sal_strncpy_s(result_str, signal_output->print_value, WORDS2BYTES(size));
                /*
                 */
                result_str[WORDS2BYTES(size) - 1] = 0;
            }
            SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
            SHR_EXIT();
        }
        else
        {
            if (result_str != NULL)
            {
                /*
                 * In case of success copy print value for caller usage
                 * Do not copy more than the size of the output container.
                 * Make sure to set an ending NULL on output string.
                 */
                sal_strncpy_s(result_str, signal_output->print_value, WORDS2BYTES(size));
            }
        }
    }

exit:
    sand_signal_output_free(signal_output);
    SHR_FUNC_EXIT;
}

/*
 * Details see in sand_signals_internal.h
 */
shr_error_e
sand_signal_handle_get(
    int unit,
    int extra_flags,
    char *block,
    char *from,
    char *to,
    char *name,
    debug_signal_t ** signal_p)
{
    match_t match;
    signal_output_t *signal_output = NULL;
    int core = 0;               /* Assuming there will be always core 0 */

    SHR_FUNC_INIT_VARS(unit);

    memset(&match, 0, sizeof(match_t));
    match.block = block;
    match.from = from;
    match.to = to;
    match.name = name;
    match.flags = extra_flags | SIGNALS_MATCH_NO_FETCH_ALL | SIGNALS_MATCH_EXACT | SIGNALS_MATCH_NOCOND | SIGNALS_MATCH_ONCE | SIGNALS_MATCH_NOVALUE;

    SHR_IF_ERR_EXIT_NO_MSG(sand_signal_get(unit, core, &match, NULL, &signal_output));

    *signal_p = signal_output->debug_signal;

exit:
    sand_signal_output_free(signal_output);
    SHR_FUNC_EXIT;
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

signal_output_t *
sand_signal_output_alloc(char * output_signal_n)
{
    signal_output_t *signal_output = sal_alloc(sizeof(signal_output_t), "Signal Output");
    if(signal_output != NULL)
    {
        sal_memset(signal_output, 0, sizeof(signal_output_t));
        /*
         * Mark that the object is not yet on any list
         */
        signal_output->entry.id = -1;
        /*
         * Fill entry name to be used when adding to the list for sanitation
         */
        sal_strncpy_s(signal_output->entry.name, output_signal_n, sizeof(signal_output->entry.name) - 1);
    }
    return signal_output;
}

void
sand_signal_output_free(
    signal_output_t *signal_output)
{
    if(signal_output)
    {
        sand_signal_list_free(signal_output->field_list);
        sal_free(signal_output);
    }
}

