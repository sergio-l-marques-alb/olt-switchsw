/*
 * $Id: $
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifndef _DNX_SW_STATE_DUMP_H
#define _DNX_SW_STATE_DUMP_H

#include <soc/dnxc/swstate/sw_state_features.h>
#include <soc/dnxc/swstate/sw_state_defs.h>
#include <soc/dnxc/swstate/types/sw_state_mutex.h>
#include <soc/dnxc/swstate/types/sw_state_sem.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)
#include <sal/core/sync.h>
#include <bcm/types.h>
#include <soc/types.h>

/* if the pointer sent to print is null, then skip this print */

#define DNX_SW_STATE_DUMP_PTR_NULL_CHECK(unit, ptr)\
        do{\
            /*coverity explanation: coverity has detected an out of bound read, due to i0 being incremented;\
            However, we have a check if i0 > the end condition of the loop. If the check catches that i0 has\
            reached the value of the end condition, we exit the function.\
            What coverity is catching is the last increment of i0 before the loop ends. */\
            /* coverity[overrun-local:FALSE] */\
            if((ptr) == NULL) {\
                return _SHR_E_NONE;\
            }\
        } while(0)

#define DNX_SW_STATE_STRIDE_DUMP_MAX_STRING_LEN 2048
#define DNX_SW_STATE_STRIDE_DUMP_FILE_NAME_LEN 512
#define DNX_SW_STATE_STRIDE_DUMP_TEMP_STR_LEN 50
#define DNX_SW_STATE_STRIDE_DUMP_HEX_LEN 4
#define DNX_SW_STATE_DUMP_STRIDE_THRESH 10

/* update the last printed value to be p, used by the stride mechanism */

#define DNX_SW_STATE_UPDATE_STRIDE(unit, p) \
        do { \
            if (sizeof(p)>sizeof(uint64)) dnx_sw_state_dump_end_of_stride(unit); \
            else { \
                uint64 value = COMPILER_64_INIT(0,0); \
                dnx_sw_state_copy_endian_independent(&value, &p, sizeof(value), sizeof(p), sizeof(p)); \
                dnx_sw_state_dump_update_stride(unit, value, (sizeof(p))); \
            } \
        } while(0)

/* print string without update the stride with the variable value */

#define DNX_SW_STATE_PRINT(unit, ...) \
        do { \
            char str[DNX_SW_STATE_STRIDE_DUMP_MAX_STRING_LEN] = {0}; \
            sal_sprintf(str, __VA_ARGS__); \
            if (dnx_sw_state_stride_dump[unit].is_stride == FALSE) {; \
                dnx_sw_state_dump_string(unit, str); \
            } else { \
                dnx_swstate_dump_update_last_string(unit, str); \
            } \
        } while (0)

/* print string and update the stride with the last printed value */

#define DNX_SW_STATE_PRINT_WITH_STRIDE_UPDATE(unit, p, ...) \
    do { \
        DNX_SW_STATE_UPDATE_STRIDE(unit, p); \
        if (dnx_sw_state_stride_dump[unit].is_stride == FALSE) { \
            DNX_SW_STATE_PRINT(unit, __VA_ARGS__); \
        } \
    } while (0)

#define STRING_TO_PRINT(short_str, full_str) dnx_sw_state_dump_mode_get(unit) == DNX_SW_STATE_DUMP_MODE_SHORT ? short_str : full_str

/* print string to STDOUT or to file chosen by "swstate config dump file_path=" */

#define DNX_SW_STATE_PRINT_MONITOR(unit, short_str, full_str, ...) \
    do { \
        if (sal_strcmp(dnx_sw_state_dump_directory_get(unit),"") == 0) { \
            char prefix[DNX_SW_STATE_STRIDE_DUMP_MAX_STRING_LEN] = {0}; \
            sal_sprintf(prefix, STRING_TO_PRINT(short_str, full_str), unit); \
            sal_sprintf(prefix + strlen(prefix), __VA_ARGS__); \
            DNX_SW_STATE_PRINT(unit, prefix); \
        } \
    } while (0)

/* print string to file */
/* used when printing to directory */

#define DNX_SW_STATE_PRINT_FILE(unit, ...) \
    do { \
        if (sal_strcmp(dnx_sw_state_dump_directory_get(unit),"") != 0) { \
            DNX_SW_STATE_PRINT(unit, __VA_ARGS__); \
       } \
    } while (0)

/* print hexadecimally to the monitor */

#define DNX_SW_STATE_PRINT_OPAQUE_MONITOR(unit, p, short_str, full_str, ...) \
    do { \
        if (sal_strcmp(dnx_sw_state_dump_directory_get(unit),"") == 0) { \
            DNX_SW_STATE_UPDATE_STRIDE(unit, p); \
            DNX_SW_STATE_PRINT(unit, STRING_TO_PRINT(short_str, full_str), unit); \
            DNX_SW_STATE_PRINT(unit, __VA_ARGS__); \
            if (dnx_sw_state_stride_dump[unit].is_stride == FALSE) { \
                dnx_sw_state_print_mem(unit, (&p), sizeof(p)); \
            } \
            dnx_swstate_dump_update_last_string(unit, ""); \
        } \
    } while (0)

/* print hexadecimally to file */

#define DNX_SW_STATE_PRINT_OPAQUE_FILE(unit, p, ...) \
    do { \
        if (sal_strcmp(dnx_sw_state_dump_directory_get(unit),"") != 0) {\
            DNX_SW_STATE_UPDATE_STRIDE(unit, p); \
            DNX_SW_STATE_PRINT(unit, __VA_ARGS__); \
            if (dnx_sw_state_stride_dump[unit].is_stride == FALSE) { \
                dnx_sw_state_print_mem(unit, (&p), sizeof(p)); \
            } \
            dnx_swstate_dump_update_last_string(unit, ""); \
        } \
    } while (0)

#define DNX_SW_STATE_DUMP_UPDATE_CURRENT_IDX(unit, idx) dnx_sw_state_dump_update_current_idx(unit, idx)

#define DNX_SW_STAET_DUMP_END_OF_STRIDE(unit) dnx_sw_state_dump_end_of_stride(unit)

#define DNX_SW_STATE_DUMP_STRIDE_MAX 1000

extern FILE *dnx_sw_state_stride_dump_output_file[BCM_MAX_NUM_UNITS];

extern uint32 all_the_same_bitmap[BCM_MAX_NUM_UNITS];

extern uint8 fix_all_the_same_flag[BCM_MAX_NUM_UNITS];

typedef struct dnx_sw_state_dump_s
{
    uint32 last_idx;
    uint32 curr_idx;
    uint32 cnt_idx;
    uint32 nof_iterations;
    uint64 last_val[DNX_SW_STATE_DUMP_STRIDE_MAX];
    uint32 last_size[DNX_SW_STATE_DUMP_STRIDE_MAX];
    uint64 last_diff[DNX_SW_STATE_DUMP_STRIDE_MAX];
    uint64 same_diff_cnt[DNX_SW_STATE_DUMP_STRIDE_MAX];
    uint8 is_stride;
    uint8 enabled;
} dnx_sw_state_dump_t;

typedef struct dnx_sw_state_dump_filters_s
{
    /*
     * When this filter is different than 0, displays only the variable's subtree.
     */
    int nocontent;
    /*
     * Filter for the variable type.
     */
    char *typefilter;
    /*
     * Filter for the variable name.
     */
    char *namefilter;
} dnx_sw_state_dump_filters_t;

typedef enum
{
    DNX_SW_STATE_DUMP_MODE_SHORT,
    DNX_SW_STATE_DUMP_MODE_FULL
} dnx_sw_state_dump_mode_t;

/**
 * \brief
 *  Get number of allocated elements pointed by ptr
 * \param [in] unit    - Device Id
 * \param [in] module_id    - sw state module Id
 * \param [in] ptr - swstate pointer address
 * \param [in] flags
 * \return
 *   int - number of allocated elements
 * \remark
 *   * None
 * \see
 *   * None
 */

uint32 dnx_sw_state_get_nof_elements(
    int unit,
    uint32 module_id,
    void *ptr);

/**
 * \brief
 *  print n bytes of memory pointed by vp (hexadecimally)
 * \param [in] unit    - Device Id
 * \param [in] vp - address of content to be printed
 * \param [in] n - number of bytes to be printed
 * \return
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */

void dnx_sw_state_print_mem(
    int unit,
    void const *vp,
    uint32 n);

/**
 * \brief
 *  Update the stride counting.
 *  If the difference between the data in index i-1 to the data in index i equals to the difference
 *    between the data in index i to the data in index i+1, then the we increase same_diff_cnt.
 *  If same_diff_cnt reach threshold DNX_SW_STATE_DUMP_STRIDE_THRESH then we conclude a stride.
 * \param [in] unit  - Device Id
 * \param [in] value - last value sent to print
 * \param [in] size  - size of the value in bytes
 * \return
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */

void dnx_sw_state_dump_update_stride(
    int unit,
    uint64 value,
    uint32 size);

/**
 * \brief
 *  mark the end of a for loop.
 *  end of a for loop is always end of stride
 * \param [in] unit  - Device Id
 * \return
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */

void dnx_sw_state_dump_end_of_stride(
    int unit);

/**
 * \brief
 * broken stride - a stride caused by break in the middle of a loop
 * for example: data[0] = 0, data[1] = 1, ..., data[100] = 100, data[101] = 200, data [102] = 201
 *                                                                /
 *                                                       broken stride
 * \param [in] unit  - Device Id
 * \return
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */

void dnx_sw_state_dump_broken_stride(
    int unit);

/**
 * \brief
 * update the current index of the data being processed
 * for specific index there might be more the one prints
 * for example : x.y[0].z = 1
 *               x.y[0].w = 10
 *               x.y[1].z = 2
 *               x.y[1].w = 20
 *               x.y[2].z = 3
 *               x.y[2].w = 30
 * \param [in] unit  - Device Id
 * \param [in] idx - the current index
 * \return
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
void dnx_sw_state_dump_update_current_idx(
    int unit,
    int idx);

/**
 * \brief
 * print the number of indices in the last stride
 * \param [in] unit  - Device Id
 * \param [in] is_broken - specify if the stride ended at the end of look (not broken)
 *   or in the middle of a loop (broken)
 * \return
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
void dnx_sw_state_dump_print_stride(
    int unit,
    uint8 is_broken);

/**
 * \brief
 * set the file name, which the prints will go to (and open it for writings)
 * \param [in] unit  - Device Id
 * \param [in] filename - file name
 * \first_string_to_print - the first string to be printed in the file
 *     the first string is something like x.y[].z[].t
 *     and the following rows looks like [2][7] = 42
 * \return
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
void dnx_sw_state_dump_attach_file(
    int unit,
    char *filename,
    char *short_prefix,
    char *long_prefix,
    char *first_string_to_print);

/**
 * \brief
 * close the file (happens when we finish to write to this file)
 * \param [in] unit  - Device Id
 * \return
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
void dnx_sw_state_dump_detach_file(
    int unit);

/**
 * \brief
 * set the directory name with the file we print to will be located
 * \param [in] unit  - Device Id
 * \param [in] directory - the directory path
 * \return
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
void dnx_sw_state_dump_directory_set(
    int unit,
    char *directory);

/**
 * \brief
 * set file path which the dump output will go to
 * \param [in] unit  - Device Id
 * \param [in] file - the file path
 * \return
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
void dnx_sw_state_dump_file_set(
    int unit,
    char *file);

/**
 * \brief
 * close the file which set by dnx_sw_state_dump_file_set
 * \param [in] unit  - Device Id
 * \return
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
void dnx_sw_state_dump_file_unset(
    int unit);

/**
 * \brief
 * set the mode of the dump.
 * \param [in] unit  - Device Id
 * \param [in] mode - DNX_SW_STATE_DUMP_MODE_SHORT - print short prefix (only root's instance name)
 *                    DNX_SW_STATE_DUMP_MODE_FULL  - print full path (with roots_array)
 * \return
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
void dnx_sw_state_dump_mode_set(
    int unit,
    dnx_sw_state_dump_mode_t mode);

/**
 * \brief
 *  get the current directory path we print to
 * \param [in] unit  - Device Id
 * \return
 *   * directory path
 * \remark
 *   * None
 * \see
 *   * None
 */

char *dnx_sw_state_dump_directory_get(
    int unit);

/**
 * \brief
 *  get the mode of the dump
 * \param [in] unit  - Device Id
 * \return
 *   * mode
 * \remark
 *   * None
 * \see
 *   * None
 */

dnx_sw_state_dump_mode_t dnx_sw_state_dump_mode_get(
    int unit);

/**
 * \brief
 *   check if all the elements pointed by ptr have the same value
 * \param [in] unit  - Device Id
 * \param [in] ptr  - address of the array to be checked
 * \param [in] size - size of element in the array
 * \param [in] nof_elements - number of elements in the array
 * \param [out] all_the_same - "0-" if all the elements are the same (used by the dump functions)
 * \return
 *   * TRUE - if all the elements have the same value
 *   * FALSE - else
 * \remark
 *   * None
 * \see
 *   * None
 */

uint8 dnx_sw_state_dump_check_all_the_same(
    int unit,
    void *ptr,
    int size,
    int nof_elements,
    char **all_the_same);

/**
 * \brief
 *   print string to the monitor/file (take stride into account
 *     if we are in the middle of a string the string will not be printed)
 * \param [in] unit  - Device Id
 * \param [in] str - the string to be printed
 * \return
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
void dnx_sw_state_dump_string(
    int unit,
    char *str);

/**
 * \brief
 *   Copy numbers with keeping in mind endianness - used by the stride mechanism
 * \param [in] destination
 * \param [in] source
 * \param [in] dst_size - destination size
 * \param [in] dst_size - source size
 * \param [in] size - size to be copied (the assumption is that size <= dst_size and size <= src_size)
 * \return
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
void *dnx_sw_state_copy_endian_independent(
    void *destination,
    const void *source,
    size_t dst_size,
    size_t src_size,
    size_t size);

/**
 * \brief
 *   check if the operating system uses little endianness
 * \return
 *   * TRUE - if little endian
 *   * FALSE - if big endian
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnx_sw_state_is_little_endian(
    void);

/**
 * \brief
 *   update last string to be printed when stride is broken
 *   with the stride mechanism, when the stride is broken the last string is no printed
 *   because we print the variable name before we print the value
 *   for example, when we have x[0] = 0, x[1] = 1, x[2] = 2 , ... , x[100] = 100, x[101] = 200
 *       when "x[101]" is being sent to print, we are still in stride mode, and when we get
 *       the number 200, the stride is detected and we have to print the stride and after it
 *       we have to print "x[101] = 200". For that reason we have to remember the string x[101].
 * \param [in] unit - Device Id
 * \param [in] str - the string to be remembered
 * \return
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
void dnx_swstate_dump_update_last_string(
    int unit,
    char *str);

/**
 * \brief
 *   Checks if the string compile with the regex.
 * \param [in] match_string - regular expression string
 * \param [in] string - string to check
 * \return
 *   * TRUE - if string compile with the regex
 *   * FALSE - if string does not compile with the regex
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnx_sw_state_compare(
    char *match_string,
    char *string);

/* examples of user defined printers */

void dnx_sw_state_print_uint8(
    int unit,
    uint8 *var);
void dnx_sw_state_print_uint16(
    int unit,
    uint16 *var);
void dnx_sw_state_print_int16(
    int unit,
    int16 *var);
void dnx_sw_state_print_uint32(
    int unit,
    uint32 *var);
void dnx_sw_state_print_uint64(
    int unit,
    uint64 *var);
void dnx_sw_state_print_int(
    int unit,
    int *var);
void dnx_sw_state_print_char(
    int unit,
    char *var);
void dnx_sw_state_print_mutex(
    int unit,
    sw_state_mutex_t * mutex);
void dnx_sw_state_print_sem(
    int unit,
    sw_state_sem_t * sem);
void dnx_sw_state_print_shr_bitdcl(
    int unit,
    SHR_BITDCL * bitmap);

extern dnx_sw_state_dump_t dnx_sw_state_stride_dump[BCM_MAX_NUM_UNITS];

#else
#define DNX_SW_STATE_PRINT(unit, ...) cli_out(__VA_ARGS__)
#define DNX_SW_STATE_PRINT_WITH_STRIDE_UPDATE(unit, p, ...) DNX_SW_STATE_PRINT(unit, __VA_ARGS__)
#define DNX_SW_STATE_DUMP_UPDATE_CURRENT_IDX(unit, idx)
#define DNX_SW_STAET_DUMP_END_OF_STRIDE(unit)
#endif /* DNX_SW_STATE_DIAGNOSTIC */
#endif /* _SHR_DNX_SW_STATE_DUMP_H */
