/** \file dnxc_sw_state_verifications.c
 * This file is a container for verification utilities that may be 
 * used by multiple sw state modules.
 */
/*
 * $Id: $
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 */

/* 
 * Note! 
 * This include statement must be at the top of every sw state .c file 
 * It points to a set of in-code compilation flags that must be taken into 
 * account for every sw state componnent compilation 
 */
#include <soc/dnxc/swstate/sw_state_features.h>
#include <soc/dnxc/swstate/dnxc_sw_state_verifications.h>
/* ---------- */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_SOCDNX_SWSTATEDNX

#ifdef DNX_SW_STATE_VERIFICATIONS
/*
 * Include files
 * {
 */
#include <soc/types.h>
#include <soc/error.h>
#include <soc/dnxc/swstate/sw_state_defs.h>
#include <soc/dnxc/swstate/dnxc_sw_state.h>
#include <soc/dnxc/swstate/dnxc_sw_state_verifications.h>
#include <soc/dnxc/swstate/dnxc_sw_state_dispatcher.h>
/*
 * }
 */
#define DNX_SW_STATE_VERF_MATRIX_GENERALLY      (1 << 0)
#define DNX_SW_STATE_VERF_MATRIX_AFTER_INIT     (1 << 1)
#define DNX_SW_STATE_VERF_MATRIX_DEINIT         (1 << 2)
#define DNX_SW_STATE_VERF_MATRIX_WB             (1 << 3)

#define SW_STATE_VERF_MATRIX_INIT(func_id, generally, init, deinit, wb)\
    dnxc_sw_state_verification_matrix[unit][func_id] =\
        (generally ? DNX_SW_STATE_VERF_MATRIX_GENERALLY : 0) |\
        (init ? DNX_SW_STATE_VERF_MATRIX_AFTER_INIT : 0) |\
        (deinit ? DNX_SW_STATE_VERF_MATRIX_DEINIT : 0) |\
        (wb ? DNX_SW_STATE_VERF_MATRIX_WB : 0)

#define SW_STATE_VERF_MATRIX_CHECK(condition)\
    (dnxc_sw_state_verification_matrix[unit][func_type] & condition)


uint8 dnxc_sw_state_verification_matrix[SOC_MAX_NUM_DEVICES][SW_STATE_FUNC_NOF];

/*
 * see .h file for description
 */
int dnxc_verifications_func_call_allowed(
    int unit,
    uint32 module,
    dnxc_sw_state_function_types_e func_type,
    uint32 flags)
{
    DNX_SW_STATE_INIT_FUNC_DEFS;

    /* remove after "remove deinit" is merged */
    SHR_EXIT();

    /* 
     * This place can be used to temporary allow some modules to not
     * obey the restrictions, mainly to allow time to fix the issue without failing
     * regression or sdk load 
     */ 
    if (module == EXAMPLE_MODULE_ID) {
        /* don't enforce on example module as it has many irregularities */
        SHR_EXIT();
    }

    if (SOC_IS_DETACHING(unit) &&
        !SW_STATE_VERF_MATRIX_CHECK(DNX_SW_STATE_VERF_MATRIX_DEINIT)) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL,
                                 "Trying to modify state during deinit\n%s%s%s",
                                 EMPTY, EMPTY, EMPTY);
    }

    if (SOC_WARM_BOOT(unit) &&
        !SW_STATE_VERF_MATRIX_CHECK(DNX_SW_STATE_VERF_MATRIX_WB)) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL,
                                 "Trying to modify state during wb\n%s%s%s",
                                 EMPTY, EMPTY, EMPTY);
    }

    if (SOC_IS_DONE_INIT(unit) &&
        !SW_STATE_VERF_MATRIX_CHECK(DNX_SW_STATE_VERF_MATRIX_AFTER_INIT)) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL,
                                 "Trying to create new state after init\n%s%s%s",
                                 EMPTY, EMPTY, EMPTY);
    }

    if (!SW_STATE_VERF_MATRIX_CHECK(DNX_SW_STATE_VERF_MATRIX_GENERALLY)) {

        
        if (module == UTILEX_HASH_TABLE_MODULE_ID || module == DBAL_MODULE_ID) {
            SHR_EXIT();
        }

            SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL,
                                 "This sw state function does not exist or is not allowed\n%s%s%s",
                                 EMPTY, EMPTY, EMPTY);
    }

    DNX_SW_STATE_FUNC_RETURN;
}

/*
 * see .h file for description
 */
int dnxc_sw_state_verifications_deinit(
    int unit){

    int index = 0;

    for (index = 0; index < SW_STATE_FUNC_NOF; index++) {
        SW_STATE_VERF_MATRIX_INIT(index, 0, 0, 0, 0);
    }

    return SOC_E_NONE;
}

/*
 * see .h file for description
 */
int dnxc_sw_state_verifications_init(
    int unit){
/* 
 * This code segments determined the matrix for which function is allowed to be called
 * At different times during device execution
 * the format is:
    SW_STATE_VERF_MATRIX_INIT(FUNCTION_ENUM_IDENTIFIER,         generally,  after init,  deinit,  wb);
 */
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_VERIFY,                1,    1,    1,    1);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_CREATE,                1,    0,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_DESTROY,               0,    0,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_GET_NEXT,              1,    1,    1,    1);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_GET_NEXT_IN_RANGE,     1,    1,    1,    1);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_STATUS_SET,            1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_IS_OCCUPIED,           1,    1,    1,    1);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_ALLOC_NEXT,            1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_CLEAR,                 1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_PRINT,                 1,    1,    1,    1);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_FIND,                  1,    1,    1,    1);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_INSERT,                1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_ITERATE,               1,    1,    1,    1);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_CREATE_EMPTY,          1,    0,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_NOF_ELEMENTS,          1,    1,    1,    1);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_NODE_KEY,              1,    1,    1,    1);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_NODE_VALUE,            1,    1,    1,    1);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_NODE_UPDATE,           1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_NEXT_NODE,             1,    1,    1,    1);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_PREVIOUS_NODE,         1,    1,    1,    1);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_ADD,                   1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_ADD_FIRST,             1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_ADD_LAST,              1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_ADD_BEFORE,            1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_ADD_AFTER,             1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_REMOVE_NODE,           1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_GET_LAST,              1,    1,    1,    1);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_GET_FIRST,             1,    1,    1,    1);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_TAKE,                  1,    1,    1,    1);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_GIVE,                  1,    1,    1,    1);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_ALLOC,                 1,    0,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_PTR_ALLOC,             1,    0,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_FREE,                  1,    0,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_PTR_FREE,              1,    0,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_IS_ALLOCATED,          1,    0,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_SET,                   1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_GET,                   1,    1,    1,    1);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_INC,                   1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_DEC,                   1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_RANGE_READ,            1,    1,    1,    1);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_RANGE_WRITE,           1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_RANGE_FILL,            1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_ALLOC_BITMAP,          1,    0,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_BIT_SET,               1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_BIT_CLEAR,             1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_BIT_GET,               1,    1,    1,    1);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_BIT_RANGE_READ,        1,    1,    1,    1);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_BIT_RANGE_WRITE,       1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_BIT_RANGE_AND,         1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_BIT_RANGE_OR,          1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_BIT_RANGE_XOR,         1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_BIT_RANGE_REMOVE,      1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_BIT_RANGE_NEGATE,      1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_BIT_RANGE_CLEAR,       1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_BIT_RANGE_SET,         1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_BIT_RANGE_NULL,        1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_BIT_RANGE_TEST,        1,    1,    1,    1);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_BIT_RANGE_EQ,          1,    1,    1,    1);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_BIT_RANGE_COUNT,       1,    1,    1,    1);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_MEMREAD,               1,    1,    1,    1);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_MEMWRITE,              1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_MEMCMP,                1,    1,    1,    1);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_MEMSET,                1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_PBMP_NEQ,              1,    1,    1,    1);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_PBMP_EQ,               1,    1,    1,    1);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_PBMP_MEMBER,           1,    1,    1,    1);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_PBMP_NOT_NULL,         1,    1,    1,    1);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_PBMP_IS_NULL,          1,    1,    1,    1);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_PBMP_COUNT,            1,    1,    1,    1);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_PBMP_XOR,              1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_PBMP_REMOVE,           1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_PBMP_ASSIGN,           1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_PBMP_GET,              1,    1,    1,    1);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_PBMP_AND,              1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_PBMP_NEGATE,           1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_PBMP_OR,               1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_PBMP_CLEAR,            1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_PBMP_PORT_ADD,         1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_PBMP_PORT_FLIP,        1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_PBMP_PORT_REMOVE,      1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_PBMP_PORT_SET,         1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_PBMP_PORTS_RANGE_ADD,  1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_PBMP_FMT,              1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_DUMP,                  1,    1,    1,    1);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_INIT,                  1,    0,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_DEINIT,                0,    0,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_IS_INIT,               1,    1,    1,    1);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_GET_NAME,              1,    1,    1,    1);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_GET_CB,                1,    1,    1,    1);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_REGISTER_CB,           1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_UNREGISTER_CB,         1,    1,    0,    0);
    return SOC_E_NONE;
}

/*
 * see .h file for description
 */
int dnx_sw_state_oob_dynamic_allocated_size_get(
    int unit,
    uint32 module_id,
    uint8 *ptr,
    uint32 *allocated_size)
{
    uint32 size = 0;
    uint32 nof_elements_retrieved = 0;
    uint32 element_size_retrieved = 0;

    if(DNX_SW_STATE_ALLOC_SIZE(unit, module_id, (uint8 *)ptr, &nof_elements_retrieved, &element_size_retrieved) != SOC_E_NONE)
    {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META_U(unit, "sw state OUT OF BOUND ERROR: Failed to retrieved size\n")));
        return SOC_E_INTERNAL;
    }
    size = nof_elements_retrieved * element_size_retrieved;
    *allocated_size = size;

    return SOC_E_NONE;
}

/*
 * see .h file for description
 */
int dnx_sw_state_oob_dynamic_array_check(
    int unit,
    uint32 module_id,
    int idx,
    void *ptr,
    uint32 type_size)
{
    uint32 size = 0;
    uint32 nof_elements_retrieved = 0;
    uint32 element_size_retrieved = 0;
    uint32 nof_elements = 0;

    if(DNX_SW_STATE_ALLOC_SIZE(unit, module_id, (uint8 *)ptr, &nof_elements_retrieved, &element_size_retrieved) != SOC_E_NONE)
    {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META_U(unit, "sw state OUT OF BOUND ERROR: Failed to retrieved size\n")));
        return SOC_E_INTERNAL;
    }

    
    size = nof_elements_retrieved * element_size_retrieved;
    
    nof_elements = (size / type_size);

    if ((idx < 0) || (idx >= nof_elements)) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META_U(unit, "sw state OUT OF BOUND ERROR: size - %d. nof_elements - %d, idx - %d \n"), size,
                   nof_elements, idx));
        return SOC_E_INTERNAL;
    }

    return SOC_E_NONE;
}

/*
 * see .h file for description
 */
int dnx_sw_state_oob_dynamic_buffer_check(
    int unit,
    uint32 module_id,
    int idx,
    void *ptr_void,
    uint32 type_size)
{
    return dnx_sw_state_oob_dynamic_array_check(unit, module_id, idx, ptr_void, type_size);
}

/*
 * see .h file for description
 */
int dnx_sw_state_oob_dynamic_bitmap_check(
    int unit,
    uint32 module_id,
    int bit_idx,
    void *ptr)
{
    uint32 nof_bits = 0;
    uint32 nof_elements_retrieved = 0;
    uint32 element_size_retrieved = 0;

    /* get number of elements and element size */
    if(DNX_SW_STATE_ALLOC_SIZE(unit, module_id, (uint8 *)ptr, &nof_elements_retrieved, &element_size_retrieved) != SOC_E_NONE)
    {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META_U(unit, "sw state OUT OF BOUND ERROR: Failed to retrieved size\n")));
        return SOC_E_INTERNAL;
    }

    nof_bits = (nof_elements_retrieved * element_size_retrieved * SHR_BITWID) / sizeof(SHR_BITDCL);

    /*
     * check if bit index is out of bound 
     */
    if((bit_idx < 0) || (bit_idx >= nof_bits)) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META_U(unit, "sw state OUT OF BOUND ERROR: nof_bits - %d, idx - %d \n"),
                   nof_bits, bit_idx));
        return SOC_E_INTERNAL;
    }

    return SOC_E_NONE;
}

#else
/** { */

/**
 * This is DUMMY code. It is only for compilers that do not accept empty files
 * and is never to be used.
 */
shr_error_e dnx_sw_state_verification_dummy_func_to_make_compiler_happy(int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_FUNC_EXIT;
}

/** } */
#endif /* DNX_SW_STATE_VERIFICATIONS */

#undef _ERR_MSG_MODULE_NAME
