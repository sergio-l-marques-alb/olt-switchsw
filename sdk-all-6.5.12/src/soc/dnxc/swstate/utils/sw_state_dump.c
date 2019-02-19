/*
 * $Id: $
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * INFO: this module implement a dump functionality for the SW State infrastructure layer,
 *
 */

/* 
 * Note! 
 * This include statement must be at the top of every sw state .c file 
 * It points to a set of in-code compilation flags that must be taken into 
 * account for every sw state componnent compilation 
 */
#include <soc/dnxc/swstate/sw_state_features.h>
/* ---------- */

#if defined(DNX_SW_STATE_DIAGNOSTIC)
#include <soc/dnxc/swstate/dnxc_sw_state_verifications.h>
#include <soc/dnxc/swstate/dnxc_sw_state_dispatcher.h>
#include <shared/bsl.h>
#include <soc/types.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <sal/core/libc.h>
#include <sal/appl/io.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdarg.h>

#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_SOCDNX_SWSTATEDNX

typedef long long unsigned int LLU;

dnx_sw_state_dump_t dnx_sw_state_stride_dump[BCM_MAX_NUM_UNITS];

char dnx_sw_state_stride_dump_file_name[BCM_MAX_NUM_UNITS][DNX_SW_STATE_STRIDE_DUMP_FILE_NAME_LEN];
char dnx_sw_state_stride_dump_last_file_name[BCM_MAX_NUM_UNITS][DNX_SW_STATE_STRIDE_DUMP_FILE_NAME_LEN];
char dnx_sw_state_stride_dump_directory_name[BCM_MAX_NUM_UNITS][DNX_SW_STATE_STRIDE_DUMP_FILE_NAME_LEN];
char dnx_sw_state_stride_dump_last_string[BCM_MAX_NUM_UNITS][DNX_SW_STATE_STRIDE_DUMP_MAX_STRING_LEN];

uint32 all_the_same_bitmap[BCM_MAX_NUM_UNITS];

uint8 fix_all_the_same_flag[BCM_MAX_NUM_UNITS];

FILE *dnx_sw_state_stride_dump_output_file[BCM_MAX_NUM_UNITS];

/* return in nof_elements the number of allocated elements pointed by ptr */
int
dnx_sw_state_get_nof_elements(
    int unit,
    uint32 module_id,
    void *ptr,
    uint32 size,
    int *from,
    int *nof_elements)
{
    int result = SOC_E_NONE;
    uint32 swstate_nof_elements = 0;
    uint32 swstate_element_size = 0;

    SOC_INIT_FUNC_DEFS;
    if (ptr == NULL)
    {
        *nof_elements = 0;
        *from = 0;
        dnx_sw_state_stride_dump[unit].skip_once++;
    }
    else
    {
        /* retrieve size and number of elements from the sw state. */
        result = dnxc_sw_state_dispatcher[unit][module_id].alloc_size(unit, module_id, (uint8 *)(ptr), &swstate_nof_elements, &swstate_element_size);

        if(SOC_E_NONE != result) {
            return result;
        }

        /* do a sanity check if the retrieved size is different from the size passed */
        if(swstate_element_size != size)
        {
            
            cli_out("Size passed differes from size retrieved from sw state \n");
            return SOC_E_INTERNAL;
        }

        *from = dnx_sw_state_dump_check_all_the_same(unit, ptr, size, *nof_elements) ? (*nof_elements) - 1 : 0;
    }
    SOC_EXIT;
exit:
    SOC_FUNC_RETURN;
}

/* print n bytes of memory pointed by vp */
void
dnx_sw_state_print_mem(
    int unit,
    void const *vp,
    uint32 n)
{
    unsigned char const *p = vp;
    uint32 i;
    char str_to_print[DNX_SW_STATE_STRIDE_DUMP_MAX_STRING_LEN] = { 0 };
    char str_tmp[DNX_SW_STATE_STRIDE_DUMP_HEX_LEN] = { 0 };

    sprintf(str_to_print, "%s", "0x");
    if (dnx_sw_state_is_little_endian())
    {
        for (i = 0; i < n; i++)
        {
            sprintf(str_tmp, "%02x", p[n - i - 1]);
            sal_strcat(str_to_print, str_tmp);
        }
    }
    else
    {
        for (i = 0; i < n; i++)
        {
            sprintf(str_tmp, "%02x", p[i]);
            sal_strcat(str_to_print, str_tmp);
        }
    }

    sal_strcat(str_to_print, "\n");
    dnx_sw_state_dump_string(unit, str_to_print);
    return;
}

/* update the stride counting. if the difference between the data in index i-1 to the data in index i equals to the difference
   between the data in index i to the data in index i+1, then the we increase same_diff_cnt.
   If same_diff_cnt reach threshold DNX_SW_STATE_DUMP_STRIDE_THRESH then we conclude a stride */

void
dnx_sw_state_dump_update_stride(
    int unit,
    uint64 value,
    uint32 size)
{

    uint32 last_idx = dnx_sw_state_stride_dump[unit].last_idx;
    uint32 curr_idx = dnx_sw_state_stride_dump[unit].curr_idx;
    uint32 *cnt_idx = &dnx_sw_state_stride_dump[unit].cnt_idx;
    uint64 *last_diff = dnx_sw_state_stride_dump[unit].last_diff;
    uint64 *last_val = dnx_sw_state_stride_dump[unit].last_val;
    uint32 *last_size = dnx_sw_state_stride_dump[unit].last_size;
    uint64 *same_diff_cnt = dnx_sw_state_stride_dump[unit].same_diff_cnt;
    uint8 *is_stride = &dnx_sw_state_stride_dump[unit].is_stride;

    /*
     * cnt_idx represents the number of reenterants of the same index. for example: a[1].x = 0 - cnt_idx = 0 a[1].y = 0 
     * - cnt_idx = 1 a[1].z = 0 - cnt_idx = 2 a[2].x = 0 - cnt_idx = 0 
     */

    /*
     * update the cnt_idx 
     */

    if (dnx_sw_state_stride_dump[unit].skip_once)
        return;
    if (last_idx != curr_idx)
        *cnt_idx = 0, dnx_sw_state_stride_dump[unit].nof_iterations++;
    else
        (*cnt_idx)++;

    if (last_size[0] == 0)
        dnx_sw_state_stride_dump[unit].nof_iterations = 0, *cnt_idx = 0;

    if (dnx_sw_state_stride_dump[unit].nof_iterations > 2)
    {
        if (value - last_val[*cnt_idx] == last_diff[*cnt_idx])
        {
            same_diff_cnt[*cnt_idx]++;
        }
        else
        {       /* broken stride */
            if (*is_stride)
                dnx_sw_state_dump_broken_stride(unit);
            sal_memset(same_diff_cnt, 0, DNX_SW_STATE_DUMP_STRIDE_MAX * sizeof(uint64));
        }
        *is_stride = (same_diff_cnt[0] > DNX_SW_STATE_DUMP_STRIDE_THRESH);
    }

    dnx_sw_state_stride_dump[unit].last_idx = curr_idx;
    last_diff[*cnt_idx] = value - last_val[*cnt_idx];
    last_val[*cnt_idx] = value;
    last_size[*cnt_idx] = size;
}

/* broken stride - a stride caused by break in the middle of a loop 
 * for example: data[0] = 0, data[1] = 1, ..., data[100] = 100, data[101] = 200, data [102] = 201
 *                                                                /
 *                                                       broken stride
 */
void
dnx_sw_state_dump_broken_stride(
    int unit)
{
    if (dnx_sw_state_stride_dump[unit].is_stride)
    {
        dnx_sw_state_dump_print_stride(unit, 1);
    }
    sal_memset(&dnx_sw_state_stride_dump[unit], 0, sizeof(dnx_sw_state_stride_dump[unit]));
}

/* end of stride - natural end of a loop
 * for example: data[0] = 0, data[1] = 1, ..., data[100] = 100,    new_data[0] = 0
 *                                                             /
 *                                                      end of stride (of data)
 */
void
dnx_sw_state_dump_end_of_stride(
    int unit)
{
    all_the_same_bitmap[unit] >>= 1;
    if (!dnx_sw_state_stride_dump[unit].skip_once)
    {
        if (dnx_sw_state_stride_dump[unit].is_stride)
        {
            dnx_sw_state_dump_print_stride(unit, 0);
        }
        sal_memset(&dnx_sw_state_stride_dump[unit], 0, sizeof(dnx_sw_state_stride_dump[unit]));
    }
    if (dnx_sw_state_stride_dump[unit].skip_once > 0)
        dnx_sw_state_stride_dump[unit].skip_once--;
}

/* check if all of nof_elements pointed by ptr have the same value */
uint8
dnx_sw_state_dump_check_all_the_same(
    int unit,
    void *ptr,
    int size,
    int nof_elements)
{
    int j;
    uint8 is_all_zero = TRUE;
    for (j = 0; j < nof_elements * size; j++)
    {
        if ((*(((uint8 *) ptr) + j)) != 0)
        {
            is_all_zero = FALSE;
            break;
        }
    }
    if (is_all_zero)
    {
        dnx_sw_state_stride_dump[unit].skip_once++;
        all_the_same_bitmap[unit]++;
        return TRUE;
    }
    for (j = 0; j < nof_elements - 1; j++)
    {
        if (0 != sal_memcmp((uint8 *) (ptr) + j * size, (uint8 *) (ptr) + (j + 1) * size, size))
            return FALSE;
    }
    all_the_same_bitmap[unit]++;
    return TRUE;
}

/* update the current index of the data being proccessed */
void
dnx_sw_state_dump_update_current_idx(
    int unit,
    int idx)
{
    if (!dnx_sw_state_stride_dump[unit].skip_once)
        dnx_sw_state_stride_dump[unit].curr_idx = idx;
}

/* print the number of indices in the last stride */
void
dnx_sw_state_dump_print_stride(
    int unit, uint8 is_broken)
{
    char str_tmp[DNX_SW_STATE_STRIDE_DUMP_MAX_STRING_LEN] = { 0 };
    sprintf(str_tmp, "STRIDE X %llu\n",
            (LLU) (dnx_sw_state_stride_dump[unit].same_diff_cnt[0]) - DNX_SW_STATE_DUMP_STRIDE_THRESH);
    if (is_broken) sal_strcat(str_tmp, dnx_sw_state_stride_dump_last_string[unit]);
    dnx_sw_state_dump_string(unit, str_tmp);
}

void
dnx_sw_state_dump_directory_set(
    int unit,
    char *directory)
{
    
    sal_memset(dnx_sw_state_stride_dump_directory_name[unit], 0,
               sizeof(dnx_sw_state_stride_dump_directory_name[unit]) /
               sizeof(dnx_sw_state_stride_dump_directory_name[unit][0]));
    sal_strcpy(dnx_sw_state_stride_dump_directory_name[unit], directory);
}

char *
dnx_sw_state_dump_directory_get(
    int unit)
{
    return dnx_sw_state_stride_dump_directory_name[unit];
}

/* determine the file to print the data to
 * if not called, then STDOUT is the default
 */
void
dnx_sw_state_dump_attach_file(
    int unit,
    char *filename,
    char *first_string_to_print)
{
    uint8 print_first_line = 0;
    char shell_cmd[1024] = { 0 };
    if (dnx_sw_state_stride_dump_output_file[unit] != NULL && sal_strcmp(filename, dnx_sw_state_stride_dump_last_file_name[unit]) == 0)
        return;
    if (sal_strcmp(dnx_sw_state_stride_dump_directory_name[unit], "") == 0)
        return;
    sal_strcpy(dnx_sw_state_stride_dump_last_file_name[unit], filename);
    sal_memset(dnx_sw_state_stride_dump_file_name[unit], 0,
               sizeof(dnx_sw_state_stride_dump_file_name[unit]) / sizeof(dnx_sw_state_stride_dump_file_name[unit][0]));
    sal_strcpy(dnx_sw_state_stride_dump_file_name[unit], dnx_sw_state_stride_dump_directory_name[unit]);
    sal_strcat(dnx_sw_state_stride_dump_file_name[unit], "/");
    sal_strcat(dnx_sw_state_stride_dump_file_name[unit], filename);

    /*
     * if the file doesn't exist, create it and all the needed folders 
     */
    if (access(dnx_sw_state_stride_dump_file_name[unit], F_OK) == -1)
    {
        sal_strcpy(shell_cmd, "rm -rf ");
        sal_strcat(shell_cmd, dnx_sw_state_stride_dump_file_name[unit]);
        sal_strcat(shell_cmd, "; mkdir -p ");
        sal_strcat(shell_cmd, dnx_sw_state_stride_dump_file_name[unit]);
        sal_strcat(shell_cmd, "; rm -rf ");
        sal_strcat(shell_cmd, dnx_sw_state_stride_dump_file_name[unit]);
        system(shell_cmd);
        print_first_line = 1;
    }
    /*
     * detach the previous file before openning the new one 
     */
    dnx_sw_state_dump_detach_file(unit);
    /*
     * append 
     */
    if ((dnx_sw_state_stride_dump_output_file[unit] = sal_fopen(dnx_sw_state_stride_dump_file_name[unit], "a")) == 0)
    {
        cli_out("Error opening sw dump file %s\n", dnx_sw_state_stride_dump_file_name[unit]);
        return;
    }
    if (print_first_line == 1)
    {
        _DNX_SW_STATE_PRINT(unit, first_string_to_print, unit);
    }
}

/* restore the file resources associated with the sw state stride dump functionality */
void
dnx_sw_state_dump_detach_file(
    int unit)
{
    if (dnx_sw_state_stride_dump_output_file[unit] != NULL)
    {
        sal_fclose(dnx_sw_state_stride_dump_output_file[unit]);
        dnx_sw_state_stride_dump_output_file[unit] = NULL;
    }
}

/* print the data to the monitor or to file, determined by the dnx_sw_state_dump_attach_file */
void
dnx_sw_state_dump_string(
    int unit,
    char *str)
{

    if (dnx_sw_state_stride_dump_output_file[unit])
    {
        sal_fprintf(dnx_sw_state_stride_dump_output_file[unit], str);
    }
    else
    {
        cli_out(str);
    }
}

/* Copy numbers with keeping in mind endianness */
void *
dnx_sw_state_copy_endian_independent(
    void *destination,
    const void *source,
    size_t dst_size,
    size_t src_size,
    size_t size)
{
    if (dnx_sw_state_is_little_endian())
    {
        sal_memcpy(destination, source, size);
    }
    else
    {
        sal_memcpy((uint8 *) destination + dst_size - size, (uint8 *) source + src_size - size, size);
    }
    return destination;
}

/* check if the arch is little endian */
int
dnx_sw_state_is_little_endian(
    void)
{
    int i = 1;
    char *p = (char *) &i;

    if (p[0] == 1)
        return 1;
    else
        return 0;
}

void dnx_swstate_dump_update_last_string(int unit, char *str) {
    sal_strcpy(dnx_sw_state_stride_dump_last_string[unit], str);
}

void dnx_sw_state_dump_middle_concat(char *str, char *str_to_concat, char after_char, uint8 from_the_end) {
    char new_str[DNX_SW_STATE_STRIDE_DUMP_MAX_STRING_LEN] = {0};
    /* find the position to concatenate str_to_concat */
    int len = sal_strlen(str);
    int i;
    for (i = len - 1; i >= 0; i--) {
        if (str[i] == after_char) {
            from_the_end--;
        }
        if (from_the_end == 0) break;
    }
    /* assert that there are from_the_end characters that equal to after_char in str */   
    assert(from_the_end == 0);
    sal_strncpy(new_str, str, i + 1);
    sal_strcat(new_str, str_to_concat);
    sal_strcat(new_str, str + i + 1);
    sal_strcpy(str, new_str);   
}


void dnx_sw_state_dump_fix_all_the_same(int unit, char *str) {
    uint32 bitmap = all_the_same_bitmap[unit];
    uint8 i = 1;
    while (bitmap != 0) {
        /* if all the same */
        if (bitmap % 2 == 1) {
            /* concatenate "0-" after the i'th '[' from the end of str */
            dnx_sw_state_dump_middle_concat(str, "0-", '[', i);
        }
        i++;
        bitmap >>= 1;
    }
}

void
dnx_sw_state_print_uint8(
    int unit,
    uint8* var)
{
    DNX_SW_STATE_PRINT(unit, *var, "%hhu\n", *var);
}

void
dnx_sw_state_print_uint16(
    int unit,
    uint16* var)
{
    DNX_SW_STATE_PRINT(unit, *var, "%hu\n", *var);
}

void
dnx_sw_state_print_uint32(
    int unit,
    uint32* var)
{
    DNX_SW_STATE_PRINT(unit, *var, "%u\n", *var);
}

void
dnx_sw_state_print_uint64(
    int unit,
    uint64* var)
{
    DNX_SW_STATE_PRINT(unit, *var, "%llu\n", (long long unsigned int)*var);
}

void
dnx_sw_state_print_int(
    int unit,
    int* var)
{
    DNX_SW_STATE_PRINT(unit, *var, "%d\n", *var);
}

void
dnx_sw_state_print_char(
    int unit,
    char* var)
{
    DNX_SW_STATE_PRINT(unit, *var, "%c\n", *var);
}

void
dnx_sw_state_print_shr_bitdcl(
    int unit,
    SHR_BITDCL* bitmap)
{
    int i;
    char tmp_str[100] = { 0 };
    char tmp_str2[3] = { 0 };
    SHR_BITDCL bitmap_tmp = *bitmap;
    uint32 max_pow = 1 << (sizeof(bitmap_tmp) * 8 - 1);
    for (i = 0; i < sizeof(SHR_BITDCL) * 8; ++i)
    {
        /*
         * print last bit and shift left. 
         */
        sal_sprintf(tmp_str2, "%u ", !!(bitmap_tmp & max_pow));
        sal_strcat(tmp_str, tmp_str2);
        bitmap_tmp = bitmap_tmp << 1;
    }
    sal_strcat(tmp_str, "\n");
    DNX_SW_STATE_PRINT(unit, bitmap_tmp, tmp_str);
}

void
dnx_sw_state_print_mutex(
    int unit,
    sal_mutex_t* mutex)
{
    DNX_SW_STATE_PRINT(unit, *mutex, "mutex\n");
}

#else /* DNX_SW_STATE_DIAGNOSTIC */
typedef int make_iso_compilers_happy;
#endif
