/** \file dnxc_sw_state_verifications.h
 * This file is a container for verification utilities that may be 
 * used by multiple sw state modules.
 */
/*
 * $Id: $
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 */

#ifndef _DNXC_SW_STATE_VERIFICATIONS_H
/* { */
#define _DNXC_SW_STATE_VERIFICATIONS_H

/* 
 * Note! 
 * This include statement must be at the top of every sw state .c file 
 * It points to a set of in-code compilation flags that must be taken into 
 * account for every sw state componnent compilation 
 */
#include <soc/dnxc/swstate/sw_state_features.h>

/*
 * Include files
 * {
 */
#include <soc/types.h>
#include <soc/error.h>
#include <shared/bsl.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnxc/dnxc_verify.h>
#include <soc/dnxc/drv_dnxc_utils.h>
/*
 * }
 */

/* 
 * this macro is used for wrapping commands that should only 
 * compile when compiling with  DNX_SW_STATE_VERIFICATIONs
 */
#ifdef DNX_SW_STATE_VERIFICATIONS
#define DNXC_SW_STATE_VERIFICATIONS_ONLY(op)\
    if (DNXC_VERIFY_ALLOWED_GET(unit))\
    {\
        op;\
    }
#else
#define DNXC_SW_STATE_VERIFICATIONS_ONLY(op)
#endif

#ifdef DNX_SW_STATE_VERIFICATIONS

/*
 * This macro enables swstate "assert mode",
 * swstate verification error will fail with assertion.
 * When turned off errors are returned instead of assertions enforced.
 */
#define DNX_SW_STATE_VERIFICATIONS_ASSERT

#ifdef DNX_SW_STATE_VERIFICATIONS_ASSERT

#define DNXC_SW_STATE_IF_ERR_WITH_LOG_COMMON(_is_assert,_expr,_formatting_string,_param2,_param3,_param4)\
do {                                                                                                     \
    int _r_rv = (_expr);                                                                                 \
    if(SHR_FAILURE(_r_rv))                                                                               \
    {                                                                                                    \
        LOG_ERROR_EX(BSL_LOG_MODULE,                                                                     \
                     _formatting_string,                                                                 \
                     shrextend_errmsg_get(_r_rv) ,_param2,_param3,_param4);                              \
        if((_is_assert) && !dnxc_sw_state_err_assert_is_disabled(unit)) {                                \
            dnxc_sw_state_err_assert_fail_debug_hook(unit);                                              \
            assert(0);                                                                                   \
        } else {                                                                                         \
            _func_rv = _r_rv;                                                                            \
            SHR_EXIT() ;                                                                                 \
        }                                                                                                \
    }                                                                                                    \
} while(0)

#define DNXC_SW_STATE_IF_ERR_INTERNAL(_is_assert, _expr)\
    DNXC_SW_STATE_IF_ERR_WITH_LOG_COMMON(_is_assert,_expr," Error '%s' indicated ; %s%s%s\r\n",EMPTY,EMPTY,EMPTY)

#define DNXC_SW_STATE_IF_ERR_WITH_LOG_INTERNAL(_is_assert,_expr,_formatting_string,_param2,_param3,_param4)\
    DNXC_SW_STATE_IF_ERR_WITH_LOG_COMMON(_is_assert,_expr," Error: %s ; " _formatting_string,_param2,_param3,_param4)

#else /* DNX_SW_STATE_VERIFICATIONS_ASSERT */
#define DNXC_SW_STATE_IF_ERR_INTERNAL(_is_assert, _expr)\
    SHR_IF_ERR_EXIT(_expr)

#define DNXC_SW_STATE_IF_ERR_WITH_LOG_INTERNAL(_is_assert,_expr,_formatting_string,_param2,_param3,_param4)\
    SHR_IF_ERR_EXIT_WITH_LOG(_expr,_formatting_string,_param2,_param3,_param4)
#endif /* DNX_SW_STATE_VERIFICATIONS_ASSERT */

#else /* DNX_SW_STATE_VERIFICATIONS */
#define DNXC_SW_STATE_IF_ERR_INTERNAL(_is_assert, _expr)
#define DNXC_SW_STATE_IF_ERR_WITH_LOG_INTERNAL(_is_assert,_expr,_formatting_string,_param2,_param3,_param4)
#endif /* DNX_SW_STATE_VERIFICATIONS */

#define DNXC_SW_STATE_IF_ERR_ASSERT(_expr)\
    DNXC_SW_STATE_IF_ERR_INTERNAL(TRUE, _expr)

#define DNXC_SW_STATE_IF_ERR_EXIT(_expr)\
    DNXC_SW_STATE_IF_ERR_INTERNAL(FALSE, _expr)

#define DNXC_SW_STATE_IF_ERR_ASSERT_WITH_LOG(_expr,_formatting_string,_param2,_param3,_param4)\
    DNXC_SW_STATE_IF_ERR_WITH_LOG_INTERNAL(TRUE, _expr,_formatting_string,_param2,_param3,_param4)

#define DNXC_SW_STATE_IF_ERR_EXIT_WITH_LOG(_expr,_formatting_string,_param2,_param3,_param4)\
    DNXC_SW_STATE_IF_ERR_WITH_LOG_INTERNAL(FALSE, _expr,_formatting_string,_param2,_param3,_param4)

/*
 * Enums
 * {
 */
#ifdef DNX_SW_STATE_VERIFICATIONS
typedef enum
{
    SW_STATE_FUNC_VERIFY,
    SW_STATE_FUNC_CREATE,
    SW_STATE_FUNC_IS_CREATED,
    SW_STATE_FUNC_DESTROY,
    SW_STATE_FUNC_GET_NEXT,
    SW_STATE_FUNC_GET_NEXT_IN_RANGE,
    SW_STATE_FUNC_STATUS_SET,
    SW_STATE_FUNC_IS_OCCUPIED,
    SW_STATE_FUNC_ALLOC_NEXT,
    SW_STATE_FUNC_CLEAR,
    SW_STATE_FUNC_PRINT,
    SW_STATE_FUNC_FIND,
    SW_STATE_FUNC_INSERT,
    SW_STATE_FUNC_DELETE,
    SW_STATE_FUNC_DELETE_ALL,
    SW_STATE_FUNC_INSERT_AT_INDEX,
    SW_STATE_FUNC_DELETE_BY_INDEX,
    SW_STATE_FUNC_GET_BY_INDEX,
    SW_STATE_FUNC_ITERATE,
    SW_STATE_FUNC_CREATE_EMPTY,
    SW_STATE_FUNC_NOF_ELEMENTS,
    SW_STATE_FUNC_NODE_KEY,
    SW_STATE_FUNC_NODE_VALUE,
    SW_STATE_FUNC_NODE_UPDATE,
    SW_STATE_FUNC_NEXT_NODE,
    SW_STATE_FUNC_PREVIOUS_NODE,
    SW_STATE_FUNC_ADD,
    SW_STATE_FUNC_ADD_FIRST,
    SW_STATE_FUNC_ADD_LAST,
    SW_STATE_FUNC_ADD_BEFORE,
    SW_STATE_FUNC_ADD_AFTER,
    SW_STATE_FUNC_ADD_AT_INDEX,
    SW_STATE_FUNC_ADD_AT_INDEX_MULTIPLE,
    SW_STATE_FUNC_REMOVE,
    SW_STATE_FUNC_REMOVE_BY_INDEX,
    SW_STATE_FUNC_REMOVE_BY_INDEX_MULTIPLE,
    SW_STATE_FUNC_REMOVE_NODE,
    SW_STATE_FUNC_GET_LAST,
    SW_STATE_FUNC_GET_FIRST,
    SW_STATE_FUNC_GET_ROOT,
    SW_STATE_FUNC_GET_PARENT,
    SW_STATE_FUNC_GET_LEFT_CHILD,
    SW_STATE_FUNC_GET_RIGHT_CHILD,
    SW_STATE_FUNC_ADD_ROOT,
    SW_STATE_FUNC_ADD_LEFT_CHILD,
    SW_STATE_FUNC_ADD_RIGHT_CHILD,
    SW_STATE_FUNC_SET_PARENT,
    SW_STATE_FUNC_SET_LEFT_CHILD,
    SW_STATE_FUNC_SET_RIGHT_CHILD,
    SW_STATE_FUNC_NODE_FREE,
    SW_STATE_FUNC_TAKE,
    SW_STATE_FUNC_GIVE,
    SW_STATE_FUNC_ALLOC,
    SW_STATE_FUNC_PTR_ALLOC,
    SW_STATE_FUNC_FREE,
    SW_STATE_FUNC_PTR_FREE,
    SW_STATE_FUNC_IS_ALLOCATED,
    SW_STATE_FUNC_SET,
    SW_STATE_FUNC_GET,
    SW_STATE_FUNC_INC,
    SW_STATE_FUNC_DEC,
    SW_STATE_FUNC_RANGE_READ,
    SW_STATE_FUNC_RANGE_WRITE,
    SW_STATE_FUNC_RANGE_FILL,
    SW_STATE_FUNC_ALLOC_BITMAP,
    SW_STATE_FUNC_BIT_SET,
    SW_STATE_FUNC_BIT_CLEAR,
    SW_STATE_FUNC_BIT_GET,
    SW_STATE_FUNC_BIT_RANGE_READ,
    SW_STATE_FUNC_BIT_RANGE_WRITE,
    SW_STATE_FUNC_BIT_RANGE_AND,
    SW_STATE_FUNC_BIT_RANGE_OR,
    SW_STATE_FUNC_BIT_RANGE_XOR,
    SW_STATE_FUNC_BIT_RANGE_REMOVE,
    SW_STATE_FUNC_BIT_RANGE_NEGATE,
    SW_STATE_FUNC_BIT_RANGE_CLEAR,
    SW_STATE_FUNC_BIT_RANGE_SET,
    SW_STATE_FUNC_BIT_RANGE_NULL,
    SW_STATE_FUNC_BIT_RANGE_TEST,
    SW_STATE_FUNC_BIT_RANGE_EQ,
    SW_STATE_FUNC_BIT_RANGE_COUNT,
    SW_STATE_FUNC_MEMREAD,
    SW_STATE_FUNC_MEMWRITE,
    SW_STATE_FUNC_MEMCMP,
    SW_STATE_FUNC_MEMSET,
    SW_STATE_FUNC_PBMP_NEQ,
    SW_STATE_FUNC_PBMP_EQ,
    SW_STATE_FUNC_PBMP_MEMBER,
    SW_STATE_FUNC_PBMP_NOT_NULL,
    SW_STATE_FUNC_PBMP_IS_NULL,
    SW_STATE_FUNC_PBMP_COUNT,
    SW_STATE_FUNC_PBMP_XOR,
    SW_STATE_FUNC_PBMP_REMOVE,
    SW_STATE_FUNC_PBMP_ASSIGN,
    SW_STATE_FUNC_PBMP_GET,
    SW_STATE_FUNC_PBMP_AND,
    SW_STATE_FUNC_PBMP_NEGATE,
    SW_STATE_FUNC_PBMP_OR,
    SW_STATE_FUNC_PBMP_CLEAR,
    SW_STATE_FUNC_PBMP_PORT_ADD,
    SW_STATE_FUNC_PBMP_PORT_FLIP,
    SW_STATE_FUNC_PBMP_PORT_REMOVE,
    SW_STATE_FUNC_PBMP_PORT_SET,
    SW_STATE_FUNC_PBMP_PORTS_RANGE_ADD,
    SW_STATE_FUNC_PBMP_FMT,
    SW_STATE_FUNC_DUMP,
    SW_STATE_FUNC_INIT,
    SW_STATE_FUNC_DEINIT,
    SW_STATE_FUNC_IS_INIT,
    SW_STATE_FUNC_GET_NAME,
    SW_STATE_FUNC_GET_CB,
    SW_STATE_FUNC_REGISTER_CB,
    SW_STATE_FUNC_UNREGISTER_CB,
    SW_STATE_FUNC_PIECE_ALLOC,
    SW_STATE_FUNC_PIECE_FREE,
    SW_STATE_FUNC_CHECK_ALL,
    SW_STATE_FUNC_CHECK_ALL_TAG,
    SW_STATE_FUNC_CHECK,
    SW_STATE_FUNC_TAG_GET,
    SW_STATE_FUNC_TAG_SET,
    SW_STATE_FUNC_NOF_FREE_ELEMENTS_GET,
    SW_STATE_FUNC_NOF_USED_ELEMENTS_IN_GRAIN_GET,
    SW_STATE_FUNC_ALLOCATE_SINGLE,
    SW_STATE_FUNC_FREE_SINGLE,
    SW_STATE_FUNC_ALLOCATE_SEVERAL,
    SW_STATE_FUNC_FREE_SEVERAL,
    SW_STATE_FUNC_DUMP_INFO_GET,
    SW_STATE_FUNC_CLEAR_ALL,
    SW_STATE_FUNC_BITMAP_ALLOC,
    SW_STATE_FUNC_BITMAP_FREE,
    SW_STATE_FUNC_EXCHANGE,
    SW_STATE_FUNC_REPLACE_DATA,
    SW_STATE_FUNC_PROFILE_DATA_GET,
    SW_STATE_FUNC_PROFILE_GET,
    SW_STATE_FUNC_FREE_ALL,
    SW_STATE_FUNC_CREATE_INFO_GET,
    SW_STATE_FUNC_STRINGNCAT,
    SW_STATE_FUNC_STRINGNCMP,
    SW_STATE_FUNC_STRINGNCPY,
    SW_STATE_FUNC_STRINGLEN,
    SW_STATE_FUNC_STRINGGET,
    SW_STATE_FUNC_NOF_ALLOCATED_ELEMENTS_IN_RANGE_GET,
    SW_STATE_FUNC_ADVANCED_ALGORITHM_INFO_SET,
    SW_STATE_FUNC_ADVANCED_ALGORITHM_INFO_GET,
    /*
     * must be last 
     */
    SW_STATE_FUNC_NOF
} dnxc_sw_state_function_types_e;
#endif /* DNX_SW_STATE_VERIFICATIONS */
/*
 * }
 */

/*
 * MACROs
 * {
 */
#ifdef DNX_SW_STATE_VERIFICATIONS
/****************************/
/* Jr2 sw state OOB macros */
/****************************/
#define DNX_SW_STATE_OOB_GET_TYPE_SIZE(ptr) sizeof(*(ptr))

#define DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(module_id, ptr, idx)\
    if (DNXC_VERIFY_ALLOWED_GET(unit))\
    {\
        DNXC_SW_STATE_IF_ERR_ASSERT(dnx_sw_state_oob_dynamic_array_check(unit, module_id, idx, (void *)ptr, DNX_SW_STATE_OOB_GET_TYPE_SIZE(ptr)));\
    }

#define DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(idx, size)\
    if (DNXC_VERIFY_ALLOWED_GET(unit))\
    {\
        DNXC_SW_STATE_IF_ERR_ASSERT(dnx_sw_state_oob_static_array_check(unit, (uint32) idx, (uint32) size));\
    }

#define DNX_SW_STATE_OOB_DYNAMIC_BUFFER_CHECK(module_id, ptr, idx)\
    if (DNXC_VERIFY_ALLOWED_GET(unit))\
    {\
        DNXC_SW_STATE_IF_ERR_ASSERT(dnx_sw_state_oob_dynamic_buffer_check(unit, module_id, (uint32) idx, (void *)ptr, DNX_SW_STATE_OOB_GET_TYPE_SIZE(ptr)));\
    }

#define DNX_SW_STATE_OOB_STATIC_BUFFER_CHECK(idx, size)\
    if (DNXC_VERIFY_ALLOWED_GET(unit))\
    {\
        DNXC_SW_STATE_IF_ERR_ASSERT(dnx_sw_state_oob_static_buffer_check(unit, (uint32) idx, (uint32) size));\
    }

#define DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(module_id, ptr, idx)\
    if (DNXC_VERIFY_ALLOWED_GET(unit))\
    {\
        DNXC_SW_STATE_IF_ERR_ASSERT(dnx_sw_state_oob_dynamic_bitmap_check(unit, module_id, idx, (void *)ptr));\
    }

#define DNX_SW_STATE_OOB_STATIC_BITMAP_CHECK(idx, size)\
    if (DNXC_VERIFY_ALLOWED_GET(unit))\
    {\
        DNXC_SW_STATE_IF_ERR_ASSERT(dnx_sw_state_oob_static_bitmap_check(unit, (uint32) idx, (uint32) size));\
    }

/************************/
/* GENERIC CHECK MACROS */
/************************/
#define DNX_SW_STATE_PTR_NULL_CHECK(unit, module_id, ptr)\
    if (DNXC_VERIFY_ALLOWED_GET(unit))\
    {\
        DNXC_SW_STATE_IF_ERR_ASSERT(dnxc_verifications_null_check(unit, module_id, (void*) ptr));\
    }

#define VERIFY_FUNCTION_CALL_ALLOWED(unit, module, func_type, flags)\
    if (DNXC_VERIFY_ALLOWED_GET(unit))\
    {\
        DNXC_SW_STATE_IF_ERR_ASSERT(dnxc_verifications_func_call_allowed(unit, module, func_type, flags));\
    }

#define VERIFY_MODULE_IS_INITIALIZED(unit, module_id, module_ptr)\
    if (DNXC_VERIFY_ALLOWED_GET(unit))\
    {\
        DNXC_SW_STATE_IF_ERR_ASSERT(dnxc_verifications_module_is_init(unit, module_id, module_ptr));\
    }

#define DNX_SW_STATE_FAMILY_CHECK(unit, module_id, family)\
    if (DNXC_VERIFY_ALLOWED_GET(unit))\
    {\
        DNXC_SW_STATE_IF_ERR_ASSERT(dnxc_verifications_family(unit, module_id, family));\
    }

#else /* DNX_SW_STATE_VERIFICATIONS */

/****************************/
/* Jr2 sw state OOB macros */
/****************************/
#define DNX_SW_STATE_OOB_GET_TYPE_SIZE(ptr)
#define DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(module_id, ptr, idx)
#define DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(idx, size)
#define DNX_SW_STATE_OOB_DYNAMIC_BUFFER_CHECK(module_id, ptr, idx)
#define DNX_SW_STATE_OOB_STATIC_BUFFER_CHECK(idx, size)
#define DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(module_id, ptr, idx)
#define DNX_SW_STATE_OOB_STATIC_BITMAP_CHECK(idx, size)

/*************************/
/* GENERIC CHECK MACROS */
/*************************/
#define DNX_SW_STATE_PTR_NULL_CHECK(unit, module_id, ptr)
#define VERIFY_FUNCTION_CALL_ALLOWED(unit, module, func_type, flags)
#define VERIFY_MODULE_IS_INITIALIZED(unit, module_id, module_ptr)
#define DNX_SW_STATE_FAMILY_CHECK(unit, module_id, family)

#endif /* DNX_SW_STATE_VERIFICATIONS */
/*
 * }
 */

/*
 * FUNCTIONs
 * {
 */

#ifdef DNX_SW_STATE_VERIFICATIONS
/**
 * \brief
 *  swstate err with assertion failure debug hook.
 * \param [in] unit    - Device Id
 * \return
 *   int - Indicate if function error should be used instead of assertino failure.
 * \remark
 *   * None
 * \see
 *   * None
 */
void dnxc_sw_state_err_assert_fail_debug_hook(
    int unit);

/**
 * \brief
 *  check if assertion failure is enabled instead of allow function to return error
 * \param [in] unit    - Device Id
 * \return
 *   int - Indicate if function error should be used instead of assertino failure.
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_sw_state_err_assert_is_disabled(
    int unit);

/**
 * \brief
 *  disable assertion failure, instead allow function to return error
 * \param [in] unit    - Device Id
 * \param [in] is_disabled    - Indicate if assertion is disabled or not
 * \return
 *   int - Error ID.
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_sw_state_err_assert_state_change(
    int unit,
    uint8 is_disabled);

/**
 * \brief
 *  denit state related to verification algorithms
 * \param [in] unit    - Device Id 
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_sw_state_verifications_deinit(
    int unit);

/**
 * \brief
 *  Init stuff that is needed for verification algorithms
 * \param [in] unit    - Device Id 
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_sw_state_verifications_init(
    int unit);

/**
 * \brief
 *  Perform some sanity checks to see that function is not being
 *  called when not allowed
 * \param [in] unit    - Device Id 
 * \param [in] module    - sw state module Id 
 * \param [in] func_type - enum representing the type of the 
 *        sw state function
 * \param [in] flags
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_verifications_func_call_allowed(
    int unit,
    uint32 module,
    dnxc_sw_state_function_types_e func_type,
    uint32 flags);

/**
 * \brief
 *  Perform some sanity checks to see that sw state module was
 *  initialized before accessing it
 * \param [in] unit    - Device Id 
 * \param [in] module_id    - Module Id 
 * \param [in] module_ptr    - a pointer to the root of the 
 *        swstate module
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_verifications_module_is_init(
    int unit,
    uint32 module_id,
    void *module_ptr);

/**
 * \brief
 *  Perform some sanity checks to see that sw state module was
 *  initialized before accessing it
 * \param [in] unit    - Device Id 
 * \param [in] module_id    - Module Id 
 * \param [in] ptr    - a pointer to the root of the 
 *        swstate module
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_verifications_null_check(
    int unit,
    uint32 module_id,
    void *ptr);

/**
 * \brief
 *  Perform some runtime checks , verify that device family
 *  is the same as compiled
 * \param [in] unit    - Device Id
 * \param [in] module_id  - Module Id
 * \param [in] soc_device    - soc device
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_verifications_family(
    int unit,
    uint32 module_id,
    soc_is_device_e soc_device);
/**
 * \brief
 *  Check if a static buffer index is out of bound
 * \param [in] unit    - Device Id 
 * \param [in] idx     - The index being checked
 * \param [in] arr_size    - The size of the arr
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnx_sw_state_oob_static_buffer_check(
    int unit,
    uint32 idx,
    uint32 arr_size);

/**
 * \brief
 *  Check if a static bitmap index is out of bound
 * \param [in] unit    - Device Id 
 * \param [in] idx     - The index being checked
 * \param [in] arr_size    - The size of the arr
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnx_sw_state_oob_static_bitmap_check(
    int unit,
    uint32 idx,
    uint32 arr_size);

/**
 * \brief
 *  Check if a static array index is out of bound
 * \param [in] unit    - Device Id 
 * \param [in] idx     - The index being checked
 * \param [in] arr_size    - The size of the arr
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnx_sw_state_oob_static_array_check(
    int unit,
    uint32 idx,
    uint32 arr_size);

/**
 * \brief
 *  Check if a dynamic array index is out of bound
 * \param [in] unit    - Device Id 
 * \param [in] module_id    - Module Id
 * \param [in] ptr     - Pointer to data allocated
 * \param [in] idx     - The index being checked
 * \param [in] ptr     - Pointer to array
 * \param [in] type_size     - size of array type
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnx_sw_state_oob_dynamic_array_check(
    int unit,
    uint32 module_id,
    int idx,
    void *ptr,
    uint32 type_size);

/**
 * \brief
 *  Check if dynamic buffer index is out of bound
 * \param [in] unit    - Device Id 
 * \param [in] module_id    - Module Id
 * \param [in] ptr     - Pointer to data allocated
 * \param [in] idx     - The index being checked
 * \param [in] ptr     - Pointer to buffer
 * \param [in] type_size     - size of buffer type
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnx_sw_state_oob_dynamic_buffer_check(
    int unit,
    uint32 module_id,
    uint32 idx,
    void *ptr,
    uint32 type_size);

/**
 * \brief
 *  Check if dynamic bitmap index is out of bound
 * \param [in] unit    - Device Id 
 * \param [in] module_id    - Module Id
 * \param [in] ptr     - Pointer to data allocated
 * \param [in] idx     - The index being checked
 * \param [in] ptr     - Pointer to bitmap
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnx_sw_state_oob_dynamic_bitmap_check(
    int unit,
    uint32 module_id,
    int idx,
    void *ptr);

/**
 * \brief
 *  Verifies the sw state alloc arguments.
 * \param [in] unit - Device Id
 * \param [in] module_id - Module id
 * \param [in] ptr_location - Pointer to the location that holds the pointer to the data that was allocated
 *
 * \param [in] nof_elements - Number of elements to allocate
 * \param [in] element_size - Size in bytes of the element type.
 * \param [in] flags - Allocation flags.
 * \param [in] dbg_string - Allocation dbg string.
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_sw_state_alloc_verify_common(
    int unit,
    uint32 module_id,
    uint8 **ptr_location,
    uint32 nof_elements,
    uint32 element_size,
    uint32 flags,
    char *dbg_string);

/**
 * \brief
 *  Verifies the free memory in the sw state arguments
 * \param [in] unit - Device Id
 * \param [in] module_id - Module id
 * \param [in] ptr_location - The pointer to the location that holds the pointer to the data to be freed.
 * \param [in] flags - Free flags.
 * \param [in] dbg_string - Allocation dbg string.
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_sw_state_free_verify_common(
    int unit,
    uint32 module_id,
    uint8 **ptr_location,
    uint32 flags,
    char *dbg_string);

/**
 * \brief
 *  Verifies the allocation size arguments.
 * \param [in] unit - Device Id
 * \param [in] module - Module Id
 * \param [in] location - Pointer to sw state data allocation
 * \param [in] nof_elements - Number of elements allocated
 * \param [in] element_size - Size in bytes of the element type
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */

int dnxc_sw_state_alloc_size_verify_common(
    int unit,
    uint32 module,
    uint8 *location,
    uint32 *nof_elements,
    uint32 *element_size);

/**
 * \brief
 *  Verifies Copy memory data to destination sw state location arguments.
 * \param [in] unit - Device Id
 * \param [in] module_id - Module Id
 * \param [in] size - The size of the of the data to be copied
 * \param [in] dest - Pointer to the destination in the sw state
 * \param [in] src -  Pointer to the source data
 * \param [in] flags - Memcpy flags.
 * \param [in] dbg_string - Debug strings.
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_sw_state_memcpy_verify_common(
    int unit,
    uint32 module_id,
    uint32 size,
    uint8 *dest,
    uint8 *src,
    uint32 flags,
    char *dbg_string);

/**
 * \brief
 *  Verifies memset sw state data arguments.
 * \param [in] unit - Device Id
 * \param [in] module_id - Module Id
 * \param [in] dest - Pointer to the destination in the sw state
 * \param [in] value - Byte pattern to be used
 * \param [in] size - Number of bytes to be set
 * \param [in] flags - Memset flags.
 * \param [in] dbg_string - Debug strings.
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_sw_state_memset_verify_common(
    int unit,
    uint32 module_id,
    uint8 *dest,
    uint32 value,
    uint32 size,
    uint32 flags,
    char *dbg_string);

#else
/** { */

/**
 * This is DUMMY code. It is only for compilers that do not accept empty files
 * and is never to be used.
 */
shr_error_e dnx_sw_state_verification_dummy_func_to_make_compiler_happy(
    int unit);

/** } */
#endif /* DNX_SW_STATE_VERIFICATIONS */

/*
 * }
 */

/* } */
#endif /* _DNXC_SW_STATE_VERIFICATIONS_H */
