/**
 * \file diag_sand_utils.c
 *
 * Misc. utilities for shell commands
 *
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#include <sal/core/regex.h>
#include <sal/appl/sal.h>

#include <shared/shrextend/shrextend_debug.h>
#include <appl/diag/system.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_utils.h>

#define BSL_LOG_MODULE BSL_LS_APPL_SHELL

/*
 * Utility routine to concatenate the first argument ("first"), with
 * the remaining arguments, with commas separating them.
 */
void
diag_sand_utils_collect_comma_args(
    args_t * a,
    char *valstr,
    char *first)
{
    char *s;

    sal_strncpy(valstr, first, ARGS_BUFFER);

    while ((s = ARG_GET(a)) != 0)
    {
        sal_strncat(valstr, ",", ARGS_BUFFER - sal_strlen(valstr) - 1);
        sal_strncat(valstr, s, ARGS_BUFFER - sal_strlen(valstr) - 1);
    }
}

cmd_result_t
diag_sand_error_get(
    shr_error_e shr_ret)
{
    cmd_result_t ret;
    switch (shr_ret)
    {
        case _SHR_E_NONE:
            ret = CMD_OK;
            break;
        case _SHR_E_PARAM:
            /*
             * Theoretically there should be ret = CMD_USAGE, but
             * For now Framework will handle wrong input problems on its own, just report failure
             */
            ret = CMD_FAIL;
            break;
        case _SHR_E_NOT_FOUND:
            ret = CMD_NFND;
            break;
        default:
            ret = CMD_FAIL;
            break;
    }
    return ret;
}

void
diag_sand_value_to_str(
    uint32 * value,
    int value_bit_size,
    char *value_str,
    int value_str_size)
{
    int i, j;
    int byte_size, long_size;
    int dest_byte_index, source_byte_index, real_byte_index;
    uint8 *source = (uint8 *) value;

    byte_size = BITS2BYTES(value_bit_size);
    /* Each byte require 2 bytes in output string - each nibble has its character */
    if (byte_size * 2 >= value_str_size)
    {
        cli_out("String length:%d cannot contain value size:%d\n", value_str_size, value_bit_size);
        return;
    }

    long_size = BYTES2WORDS(byte_size);
    sal_memset(value_str, 0, value_str_size);

    dest_byte_index = 0;
#ifdef BE_HOST
    for (i = 0; i < long_size; i++)
    {
        for (j = 0; j < 4; j++)
        {
            source_byte_index = 4 * (long_size - 1 - i) + j;
            real_byte_index = 4 * (long_size - 1 - i) + 3 - j;
            if (real_byte_index >= byte_size)
                continue;
            sal_snprintf(&value_str[2 * dest_byte_index], value_str_size, "%02x", source[source_byte_index]);
            dest_byte_index++;
        }
    }
#else
    for (i = 0; i < long_size; i++)
    {
        for (j = 0; j < 4; j++)
        {
            source_byte_index = 4 * (long_size - 1 - i) + 3 - j;
            real_byte_index = 4 * (long_size - 1 - i) + 3 - j;
            if (real_byte_index >= byte_size)
                continue;
            sal_snprintf(&value_str[2 * dest_byte_index], value_str_size, "%02x", source[source_byte_index]);
            dest_byte_index++;
        }
    }
#endif
}

int
diag_sand_value_same(
    uint32 * value_first,
    uint32 * value_second,
    int value_bit_size)
{
    int i;
    int byte_size, long_size;

    byte_size = BITS2BYTES(value_bit_size);
    long_size = BYTES2WORDS(byte_size);

    for (i = 0; i < long_size; i++)
    {
        if(value_first[i] != value_second[i])
            return FALSE;
    }
    return TRUE;
}

shr_error_e
diag_sand_compare_init(
    char *match_n,
    void **compare_handle_p)
{
    SHR_FUNC_INIT_VARS(NO_UNIT);

    if (ISEMPTY(match_n))
    {
        *compare_handle_p = NULL;
    }
    else
    {
#if !defined(__KERNEL__) && !defined(VXWORKS) && !defined(NO_REGEX)
        regex_t *regex = sal_alloc(sizeof(regex_t), "regex");
        /*
         * Compile regular expression 
         */
        if (regcomp(regex, match_n, REG_EXTENDED | REG_NOSUB))
        {
            SHR_CLI_EXIT(_SHR_E_PARAM, "Unable to compile:%s \n", match_n);
        }
        *compare_handle_p = (void *) regex;
#else
        *compare_handle_p = (void *) match_n;
        SHR_EXIT();
#endif
    }

exit:
    SHR_FUNC_EXIT;
}

int
diag_sand_compare(
    void *compare_handle,
    char *string)
{
    if (compare_handle == NULL) /* No comparison requested */
        return TRUE;
#if !defined(__KERNEL__) && !defined(VXWORKS) && !defined(NO_REGEX)
    if (!regexec((regex_t *) compare_handle, string, 0, NULL, 0))
    {
        return TRUE;
    }
#else
    if (sal_strcasestr(compare_handle, string) != NULL)
    {
        return TRUE;
    }
#endif
    else
        return FALSE;
}

void
diag_sand_compare_close(
    void *compare_handle)
{
    if (compare_handle == NULL) /* No comparison was requested */
        return;
#if !defined(__KERNEL__) && !defined(VXWORKS) && !defined(NO_REGEX)
    regfree(compare_handle);
    sal_free(compare_handle);
#endif
    return;
}

shr_error_e
diag_sand_reg_blocks_get(
    int unit,
    soc_reg_info_t * reginfo,
    char *block_str,
    char *match_n)
{
    shr_error_e rv;
    int i_bl;

    if (!ISEMPTY(match_n))
        rv = _SHR_E_NOT_FOUND;
    else
        rv = _SHR_E_NONE;

    sal_memset(block_str, 0, PRT_COLUMN_WIDTH_BIG);
    for (i_bl = 0; SOC_BLOCK_INFO(unit, i_bl).type >= 0; i_bl++)
    {
        if (!SOC_BLOCK_IS_TYPE(unit, i_bl, reginfo->block))
            continue;
        if (sal_strlen(block_str) != 0)
            sal_strcat(block_str, ",");

        sal_sprintf(block_str + sal_strlen(block_str), "%s", SOC_BLOCK_NAME(unit, i_bl));

        if (!SOC_INFO(unit).block_valid[i_bl])
            sal_strcat(block_str, "(dis)");
        if (!ISEMPTY(match_n) && sal_strcasestr(SOC_BLOCK_NAME(unit, i_bl), match_n))
            rv = _SHR_E_NONE;
    }
    return rv;
}

shr_error_e
diag_sand_mem_blocks_get(
    int unit,
    soc_mem_t mem,
    char *block_str,
    char *match_n)
{
    shr_error_e rv;
    unsigned int i_bl;

    if (!ISEMPTY(match_n))
        rv = _SHR_E_NOT_FOUND;
    else
        rv = _SHR_E_NONE;

    sal_memset(block_str, 0, PRT_COLUMN_WIDTH_BIG);
    SOC_MEM_BLOCK_ITER(unit, mem, i_bl)
    {
        if (sal_strlen(block_str) != 0)
            sal_strcat(block_str, ",");

        sprintf(block_str + sal_strlen(block_str), "%s", SOC_BLOCK_NAME(unit, i_bl));

        if (!SOC_INFO(unit).block_valid[i_bl])
            sal_strcat(block_str, "(dis)");
        if (!ISEMPTY(match_n) && sal_strcasestr(SOC_BLOCK_NAME(unit, i_bl), match_n))
            rv = _SHR_E_NONE;
    }

    return rv;
}

void
diag_sand_get_name_and_index(
    int unit,
    char *block_n,
    uint32 address,
    /** coverity[param_set_but_not_used:FALSE] */
    char *name,
    int *index)
{
    soc_reg_t reg;
    soc_mem_t mem;

    soc_reg_info_t *reginfo;
    char block_str[PRT_COLUMN_WIDTH_BIG];
    uint8 acc_type;
    uint32 reg_addr;

    acc_type = TRUE;
    for (reg = 0; reg < NUM_SOC_REG; reg++)
    {
        if (!SOC_REG_IS_VALID(unit, reg))
        continue;

        reginfo = &SOC_REG_INFO(unit, reg);

        reg_addr = SOC_REG_BASE(unit, reg);

        if(address != SH_SAND_MAX_UINT32)
        {
            int max_adr_offset = 0;
            if(reginfo->flags & SOC_REG_FLAG_ARRAY)
            {
                max_adr_offset = (reginfo->numels - 1) * SOC_REG_ELEM_SKIP(unit, reg);
            }
            if((address < reg_addr) || (address > reg_addr + max_adr_offset))
                continue;
        }

        /*
         * Filter on blocks
         */

        if (diag_sand_reg_blocks_get(unit, reginfo, block_str, block_n) != _SHR_E_NONE)
            continue;
        name = SOC_REG_NAME(unit, reg);
        *index = address - reg_addr;
    }

    for (mem = 0; mem < NUM_SOC_MEM; mem++)
    {

        if (!SOC_MEM_IS_VALID(unit, mem))
            continue;

        /*
         * Fill blocks
         */
        if (diag_sand_mem_blocks_get(unit, mem, block_str, block_n) != _SHR_E_NONE)
            continue;

        if(address != SH_SAND_MAX_UINT32)
        {
            uint32 start, end;
            int array_index = 0;
            soc_mem_array_info_t *maip;

            start = soc_mem_addr_get(unit, mem, 0, SOC_MEM_BLOCK_ANY(unit, mem), 0, &acc_type);

            if((maip = SOC_MEM_ARRAY_INFOP(unit, mem)) != NULL)
                array_index = maip->numels - 1;

            end = soc_mem_addr_get(unit, mem, array_index, SOC_MEM_BLOCK_ANY(unit, mem), soc_mem_index_max(unit, mem), &acc_type);
            if((address < start) || (address > end))
                continue;
            name = SOC_MEM_NAME(unit, mem);
            *index = address - start;
        }
    }

}
