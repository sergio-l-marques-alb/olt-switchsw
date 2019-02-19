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

#if defined(DNX_SW_STATE_LOGGING)
#include <shared/bsl.h>
#include <soc/dnxc/swstate/sw_state_defs.h>
#include <soc/dnxc/swstate/dnx_sw_state_logging.h>

#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_SOCDNX_SWSTATEDNX

#define SW_STATE_LOG_MAX_STRING_LEN 1024
#define SW_STATE_LOG_HEX_LEN 4

/* print to str the n bytes of memory pointed by vp */
void
dnx_sw_state_print_mem_to_string(
    void const *vp,
    uint32 n,
    char **str)
{
    unsigned char const *p = vp;
    uint32 i;
    char *str_to_print = sal_alloc(SW_STATE_LOG_MAX_STRING_LEN, "swstate log");
    char str_tmp[SW_STATE_LOG_HEX_LEN] = { 0 };
    sal_memset(str_to_print, 0, SW_STATE_LOG_MAX_STRING_LEN);

    if (n < SW_STATE_LOG_MAX_STRING_LEN)
    {
        for (i = 0; i < n; i++)
        {
            sal_sprintf(str_tmp, "%02x", p[i]);
            sal_strcat(str_to_print, str_tmp);
        }
    }

    *str = str_to_print;
    return;
}

void
dnx_sw_state_log(
    dnx_sw_state_func_type_logging_e func_type,
    uint32 bsl_ls,
    char *func_name,
    void *value_addr,
    uint32 size,
    char *format,
    ...)
{

    va_list args;
    char str_to_print[SW_STATE_LOG_MAX_STRING_LEN] = { 0 };
    char new_format[SW_STATE_LOG_MAX_STRING_LEN] = { 0 };

    va_start(args, format);
    switch (func_type)
    {
        case DNX_SW_STATE_GET_LOGGING:
        case DNX_SW_STATE_SET_LOGGING:
        {
            char *str_value;
            dnx_sw_state_print_mem_to_string(value_addr, size, &str_value);
            sal_sprintf(new_format, "%s:\t%s = 0x%s\n", func_name, format, str_value);
            sal_free(str_value);
        }
            break;
        case DNX_SW_STATE_ALLOC_LOGGING:
        case DNX_SW_STATE_PTR_ALLOC_LOGGING:
            sal_sprintf(new_format, "%s:\t%s%s\n", func_name, format, " - %d instances");
            break;
        case DNX_SW_STATE_ALLOC_BITMAP_LOGGING:
            sal_sprintf(new_format, "%s:\t%s%s\n", func_name, format, " - %d bits");
            break;
        case DNX_SW_STATE_INIT_LOGGING:
        case DNX_SW_STATE_DEINIT_LOGGING:
        case DNX_SW_STATE_IS_INIT_LOGGING:
        case DNX_SW_STATE_IS_ALLOC_LOGGING:
        case DNX_SW_STATE_FREE_LOGGING:
        case DNX_SW_STATE_PTR_FREE_LOGGING:
        case DNX_SW_STATE_MUTEX_CREATE_LOGGING:
        case DNX_SW_STATE_MUTEX_DESTROY_LOGGING:
        case DNX_SW_STATE_MUTEX_TAKE_LOGGING:
        case DNX_SW_STATE_MUTEX_GIVE_LOGGING:
        case DNX_SW_STATE_PBMP_NEQ_LOGGING:
        case DNX_SW_STATE_PBMP_EQ_LOGGING:
        case DNX_SW_STATE_PBMP_MEMBER_LOGGING:
        case DNX_SW_STATE_PBMP_NOT_NULL_LOGGING:
        case DNX_SW_STATE_PBMP_IS_NULL_LOGGING:
        case DNX_SW_STATE_PBMP_COUNT_LOGGING:
        case DNX_SW_STATE_PBMP_XOR_LOGGING:
        case DNX_SW_STATE_PBMP_REMOVE_LOGGING:
        case DNX_SW_STATE_PBMP_ASSIGN_LOGGING:
        case DNX_SW_STATE_PBMP_GET_LOGGING:
        case DNX_SW_STATE_PBMP_AND_LOGGING:
        case DNX_SW_STATE_PBMP_NEGATE_LOGGING:
        case DNX_SW_STATE_PBMP_OR_LOGGING:
        case DNX_SW_STATE_PBMP_CLEAR_LOGGING:
        case DNX_SW_STATE_PBMP_PORT_ADD_LOGGING:
        case DNX_SW_STATE_PBMP_PORT_FLIP_LOGGING:
        case DNX_SW_STATE_PBMP_PORT_REMOVE_LOGGING:
        case DNX_SW_STATE_PBMP_PORT_SET_LOGGING:
        case DNX_SW_STATE_PBMP_PORTS_RANGE_ADD_LOGGING:
        case DNX_SW_STATE_PBMP_FMT_LOGGING:
        case DNX_SW_STATE_LL_CREATE_EMPTY_LOGGING:
        case DNX_SW_STATE_LL_DESTROY_LOGGING:
        case DNX_SW_STATE_LL_NOF_ELEMENTS_LOGGING:
        case DNX_SW_STATE_LL_NODE_KEY_LOGGING:
        case DNX_SW_STATE_LL_NODE_VALUE_LOGGING:
        case DNX_SW_STATE_LL_NODE_UPDATE_LOGGING:
        case DNX_SW_STATE_LL_NEXT_NODE_LOGGING:
        case DNX_SW_STATE_LL_PREVIOUS_NODE_LOGGING:
        case DNX_SW_STATE_LL_ADD_LOGGING:
        case DNX_SW_STATE_LL_ADD_FIRST_LOGGING:
        case DNX_SW_STATE_LL_ADD_LAST_LOGGING:
        case DNX_SW_STATE_LL_ADD_BEFORE_LOGGING:
        case DNX_SW_STATE_LL_ADD_AFTER_LOGGING:
        case DNX_SW_STATE_LL_REMOVE_NODE_LOGGING:
        case DNX_SW_STATE_LL_GET_LAST_LOGGING:
        case DNX_SW_STATE_LL_GET_FIRST_LOGGING:
        case DNX_SW_STATE_LL_PRINT_LOGGING:
        case DNX_SW_STATE_LL_FIND_LOGGING:
        case DNX_SW_STATE_OCC_BM_ALLOC_NEXT_LOGGING:
        case DNX_SW_STATE_OCC_BM_CLEAR_LOGGING:
        case DNX_SW_STATE_OCC_BM_CREATE_LOGGING:
        case DNX_SW_STATE_OCC_BM_DESTROY_LOGGING:
        case DNX_SW_STATE_OCC_BM_GET_NEXT_IN_RANGE_LOGGING:
        case DNX_SW_STATE_OCC_BM_GET_NEXT_LOGGING:
        case DNX_SW_STATE_OCC_BM_IS_OCCUPIED_LOGGING:
        case DNX_SW_STATE_OCC_BM_STATUS_SET_LOGGING:
        case DNX_SW_STATE_OCC_BM_PRINT_LOGGING:
        case DNX_SW_STATE_HTB_CREATE_LOGGING:
        case DNX_SW_STATE_HTB_DESTROY_LOGGING:
        case DNX_SW_STATE_HTB_INSERT_LOGGING:
        case DNX_SW_STATE_HTB_FIND_LOGGING:
        case DNX_SW_STATE_HTB_GET_NEXT_LOGGING:
        case DNX_SW_STATE_HTB_CLEAR_LOGGING:
        case DNX_SW_STATE_HTB_DELETE_LOGGING:
        case DNX_SW_STATE_HTB_INSERT_AT_INDEX_LOGGING:
        case DNX_SW_STATE_HTB_DELETE_BY_INDEX_LOGGING:
        case DNX_SW_STATE_HTB_GET_BY_INDEX_LOGGING:        
        case DNX_SW_STATE_HTB_PRINT_LOGGING:
        case DNX_SW_STATE_MULTI_SET_CREATE_LOGGING:
        case DNX_SW_STATE_MULTI_SET_DESTROY_LOGGING:
        case DNX_SW_STATE_MULTI_SET_FIND_LOGGING:
        case DNX_SW_STATE_MULTI_SET_GET_NEXT_LOGGING:
        case DNX_SW_STATE_MULTI_SET_CLEAR_LOGGING:
        case DNX_SW_STATE_MULTI_SET_ADD_LOGGING:
        case DNX_SW_STATE_MULTI_SET_ADD_AT_INDEX_LOGGING:
        case DNX_SW_STATE_MULTI_SET_ADD_AT_INDEX_MULTIPLE_LOGGING:
        case DNX_SW_STATE_MULTI_SET_GET_BY_INDEX_LOGGING:
        case DNX_SW_STATE_MULTI_SET_REMOVE_LOGGING:
        case DNX_SW_STATE_MULTI_SET_REMOVE_BY_INDEX_LOGGING:
        case DNX_SW_STATE_MULTI_SET_REMOVE_BY_INDEX_MULTIPLE_LOGGING:
        case DNX_SW_STATE_MULTI_SET_PRINT_LOGGING:
        case DNX_SW_STATE_DEFRAGMENTED_CHUNK_CREATE_LOGGING:
        case DNX_SW_STATE_DEFRAGMENTED_CHUNK_DESTROY_LOGGING:
        case DNX_SW_STATE_DEFRAGMENTED_CHUNK_PIECE_ALLOC_LOGGING:
        case DNX_SW_STATE_DEFRAGMENTED_CHUNK_PIECE_FREE_LOGGING:
        case DNX_SW_STATE_DEFRAGMENTED_CHUNK_PRINT_LOGGING:
        case DNX_SW_STATE_BIT_SET_LOGGING:
        case DNX_SW_STATE_BIT_CLEAR_LOGGING:
        case DNX_SW_STATE_BIT_GET_LOGGING:
        case DNX_SW_STATE_BIT_RANGE_READ_LOGGING:
        case DNX_SW_STATE_BIT_RANGE_WRITE_LOGGING:
        case DNX_SW_STATE_BIT_RANGE_AND_LOGGING:
        case DNX_SW_STATE_BIT_RANGE_OR_LOGGING:
        case DNX_SW_STATE_BIT_RANGE_XOR_LOGGING:
        case DNX_SW_STATE_BIT_RANGE_REMOVE_LOGGING:
        case DNX_SW_STATE_BIT_RANGE_NEGATE_LOGGING:
        case DNX_SW_STATE_BIT_RANGE_CLEAR_LOGGING:
        case DNX_SW_STATE_BIT_RANGE_SET_LOGGING:
        case DNX_SW_STATE_BIT_RANGE_NULL_LOGGING:
        case DNX_SW_STATE_BIT_RANGE_TEST_LOGGING:
        case DNX_SW_STATE_BIT_RANGE_EQ_LOGGING:
        case DNX_SW_STATE_BIT_RANGE_COUNT_LOGGING:
        case DNX_SW_STATE_CB_DB_GET_CB_LOGGING:
        case DNX_SW_STATE_CB_DB_REGISTER_CB_LOGGING:
        case DNX_SW_STATE_CB_DB_UNREGISTER_CB_LOGGING:
            sal_sprintf(new_format, "%s:\t%s\n", func_name, format);
            break;
        case DNX_SW_STATE_MEMREAD_LOGGING:
        case DNX_SW_STATE_MEMWRITE_LOGGING:
        case DNX_SW_STATE_MEMCMP_LOGGING:
        case DNX_SW_STATE_MEMSET_LOGGING:
        case DNX_SW_STATE_RANGE_READ_LOGGING:
        case DNX_SW_STATE_RANGE_WRITE_LOGGING:
        case DNX_SW_STATE_RANGE_FILL_LOGGING:
            sal_sprintf(new_format, "%s:\t%s%s\n", func_name, format, " - %d bytes");
            break;
    }
    sal_vsprintf(str_to_print, new_format, args);
    LOG_VERBOSE(bsl_ls, (BSL_META("%s"), str_to_print));
    va_end(args);

}

#else /* DNX_SW_STATE_LOGGING */
typedef int make_iso_compilers_happy;
#endif
