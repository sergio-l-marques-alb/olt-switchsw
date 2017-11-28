/** \file dnxc_sw_state_verifications.h
 * This file is a container for verification utilities that may be 
 * used by multiple sw state modules.
 */
/*
 * $Id: $
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 */

#ifndef _DNXC_SW_STATE_VERIFICATIONS_H
/* { */
#define _DNXC_SW_STATE_VERIFICATIONS_H

/*
 * Include files
 * {
 */
#include <soc/types.h>
#include <soc/error.h>
#include <shared/bsl.h>
#include <shared/shrextend/shrextend_debug.h>
/*
 * }
 */

/* 
 * this macro is used for wrapping commands that should only 
 * compile when compiling with  DNX_SW_STATE_VERIFICATIONs
 */
#ifdef DNX_SW_STATE_VERIFICATIONS
#define DNX_SW_STATE_VERIFICATIONS_ONLY(op) op
#else
#define DNX_SW_STATE_VERIFICATIONS_ONLY(op)
#endif

/*
 * Enums
 * {
 */
#ifdef DNX_SW_STATE_VERIFICATIONS
typedef enum
{
    SW_STATE_FUNC_VERIFY,
    SW_STATE_FUNC_CREATE,
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
    SW_STATE_FUNC_NOF,
    SW_STATE_FUNC_PIECE_ALLOC,
    SW_STATE_FUNC_PIECE_FREE
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
/* Jer2 sw state OOB macros */
/****************************/
#define DNX_SW_STATE_OOB_GET_TYPE_SIZE(ptr) sizeof(*(ptr))

#define DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(module_id, ptr, idx)\
    do{\
        if(dnx_sw_state_oob_dynamic_array_check(unit, module_id, idx, (void *)ptr, DNX_SW_STATE_OOB_GET_TYPE_SIZE(ptr)) != SOC_E_NONE)\
        {\
            LOG_ERROR(BSL_LS_SHARED_SWSTATE, (BSL_META_U(unit, "sw state ERROR: value %u is outside of DYNAMIC ARRAY limits.\n"), (uint32)(idx)));\
            return _SHR_E_INTERNAL;\
        }\
    } while(0)

#define DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(idx, size)\
    do{\
        if((idx >= (size)) || (idx < 0))\
        {\
            LOG_ERROR(BSL_LS_SHARED_SWSTATE, (BSL_META_U(unit, "sw state ERROR: value %u is outside of STATIC ARRAY limit %u.\n"), (uint32)(idx), (uint32)(size)));\
            return _SHR_E_INTERNAL;\
        }\
    } while(0)

#define DNX_SW_STATE_OOB_DYNAMIC_BUFFER_CHECK(module_id, ptr, idx)\
    do{\
        if(dnx_sw_state_oob_dynamic_buffer_check(unit, module_id, idx, (void *)ptr, DNX_SW_STATE_OOB_GET_TYPE_SIZE(ptr)) != SOC_E_NONE)\
        {\
            LOG_ERROR(BSL_LS_SHARED_SWSTATE, (BSL_META_U(unit, "sw state ERROR: value %u is outside of DYNAMIC BUFFER limits.\n"), (uint32)(idx)));\
            return _SHR_E_INTERNAL;\
        }\
    } while(0)

#define DNX_SW_STATE_OOB_STATIC_BUFFER_CHECK(idx, size)\
    do{\
        if((idx >= (size)) || (idx < 0))\
        {\
            LOG_ERROR(BSL_LS_SHARED_SWSTATE, (BSL_META_U(unit, "sw state ERROR: value %u is outside of STATIC BUFFER limit %u.\n"), (uint32)(idx), (uint32)(size)));\
            return _SHR_E_INTERNAL;\
        }\
    } while(0)

#define DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(module_id, ptr, idx)\
    do{\
        if(dnx_sw_state_oob_dynamic_bitmap_check(unit, module_id, idx, (void *)ptr) != SOC_E_NONE)\
        {\
            LOG_ERROR(BSL_LS_SHARED_SWSTATE, (BSL_META_U(unit, "sw state ERROR: value %u is outside of DYNAMIC BITMAP limits.\n"), (uint32)(idx)));\
            return _SHR_E_INTERNAL;\
        }\
    } while(0)

#define DNX_SW_STATE_OOB_STATIC_BITMAP_CHECK(idx, size)\
    do{\
        if((idx >= ((size)*SHR_BITWID)) || (idx < 0))\
        {\
            LOG_ERROR(BSL_LS_SHARED_SWSTATE, (BSL_META_U(unit, "sw state ERROR: value %u is outside of STATIC BITMAP limits.\n"), (uint32)(idx)));\
            return _SHR_E_INTERNAL;\
        }\
    } while(0)

/************************/
/* GENERIC CHECK MACROS */
/************************/
#define DNX_SW_STATE_PTR_NULL_CHECK(unit, ptr)\
        do{\
            if((ptr) == NULL) {\
                LOG_ERROR(BSL_LS_SHARED_SWSTATE, (BSL_META_U(unit, "sw state ERROR: trying to access null pointer.\n")));\
                return _SHR_E_INTERNAL;\
            }\
        } while(0)

#define VERIFY_FUNCTION_CALL_ALLOWED(unit, module, func_type, flags)\
    SHR_IF_ERR_EXIT(dnxc_verifications_func_call_allowed(unit, module, func_type, flags));
#else /* DNX_SW_STATE_VERIFICATIONS */
/****************************/
/* Jer2 sw state OOB macros */
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
#define DNX_SW_STATE_PTR_NULL_CHECK(unit, ptr)
#define VERIFY_FUNCTION_CALL_ALLOWED(unit, module, func_type, flags)

#endif /* DNX_SW_STATE_VERIFICATIONS */
/*
 * }
 */

/*
 * FUNCTIONs
 * {
 */

#ifdef DNX_SW_STATE_VERIFICATIONS
/*
 * see .h file for description
 */

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
#ifdef DNX_SW_STATE_VERIFICATIONS
int dnxc_verifications_func_call_allowed(
    int unit,
    uint32 module,
    dnxc_sw_state_function_types_e func_type,
    uint32 flags);
#endif
/**
 * \brief
 *  Get size allocated
 * \param [in] unit    - Device Id 
 * \param [in] module_id    - Module Id
 * \param [in] ptr     - Pointer to data allocated
 * \param [out] allocated_size     - allocation size retrieved
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnx_sw_state_oob_dynamic_allocated_size_get(
    int unit,
    uint32 module_id,
    uint8 *ptr,
    uint32 *allocated_size);

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
    int idx,
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
