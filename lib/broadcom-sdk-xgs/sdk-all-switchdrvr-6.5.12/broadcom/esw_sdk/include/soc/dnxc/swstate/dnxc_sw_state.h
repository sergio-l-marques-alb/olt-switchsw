/** \file dnxc_sw_state.h
 * This is the main dnxc sw state module.
 * Module is implementing the access for wb and plain implementations.
 */
/*
 * $Id: $
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef _DNXC_SW_STATE_H
/* { */
#define _DNXC_SW_STATE_H

/*
 * Include files
 * {
 */
#include <assert.h>
#include <shared/bsl.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnxc/swstate/dnxc_sw_state_dispatcher.h>
/*
 * }
 */

/*
 * MACROs
 * {
 */
#define DNX_SW_STATE_ALLOC_MAX_BYTES 100*1000

/**
 * \brief - initialize a sw state module
 */
#define DNX_SW_STATE_MODULE_INIT(unit, module_id, implementation, type, flags, dbg_string)\
    do {\
        SHR_IF_ERR_EXIT(dnxc_sw_state_dispatcher_module_init(unit, module_id, implementation));\
        SHR_IF_ERR_EXIT(dnxc_sw_state_dispatcher[unit][module_id].module_init(unit, module_id, sizeof(type), flags, dbg_string));\
    } while(0)

/**
 * \brief - deinitialize a sw state module
 */
#define DNX_SW_STATE_MODULE_DEINIT(unit, module_id, flags, dbg_string)\
    do {\
        SHR_IF_ERR_EXIT(dnxc_sw_state_dispatcher[unit][module_id].module_deinit(unit, module_id, flags, dbg_string));\
        SHR_IF_ERR_EXIT(dnxc_sw_state_dispatcher_module_deinit(unit));\
    } while(0)

/**
 * \brief - allocate sw state memory, basic
 */
#define DNX_SW_STATE_ALLOC_BASIC(unit, module_id, ptr_location, num_of_elements, size, flags, dbg_string)\
    do {\
        SHR_IF_ERR_EXIT(dnxc_sw_state_dispatcher[unit][module_id].alloc(unit, module_id, ptr_location, num_of_elements, size, flags, dbg_string));\
    } while(0)

/**
 * \brief - allocate sw state memory
 */
#define DNX_SW_STATE_ALLOC(unit, module_id, location, type, num_of_elements, flags, dbg_string)\
        DNX_SW_STATE_ALLOC_BASIC(unit, module_id, (uint8 **)(&location), num_of_elements, sizeof(type), flags, dbg_string)

/**
 * \brief - free sw state memory, basic
 */
#define DNX_SW_STATE_FREE_BASIC(unit, module_id, ptr_location, flags, dbg_string)\
    do {\
        SHR_IF_ERR_EXIT(dnxc_sw_state_dispatcher[unit][module_id].free(unit, module_id, ptr_location, flags, dbg_string));\
    } while(0)

/**
 * \brief - free sw state memory
 */
#define DNX_SW_STATE_FREE(unit, module_id, location, dbg_string)\
    DNX_SW_STATE_FREE_BASIC(unit, module_id, (uint8 **)(&location), 0, dbg_string)

/**
 * \brief - get allocation size for sw state memory
 */
#define DNX_SW_STATE_ALLOC_SIZE(unit, module_id, location, ptr_nof_elements, ptr_element_size)\
    dnxc_sw_state_dispatcher[unit][module_id].alloc_size(unit, module_id, location, ptr_nof_elements, ptr_element_size)

/**
 * \brief - set sw state value
 */
#define DNX_SW_STATE_SET(unit, module_id, location, input, type, flags, dbg_string)\
    do {\
        SHR_IF_ERR_EXIT(dnxc_sw_state_dispatcher[unit][module_id].memcpy(unit, module_id, sizeof(type), (uint8 *)(&location), (uint8 *)(&input), flags, dbg_string));\
    } while(0)

/**
 * \brief - increment sw state counter value
 */
#define DNX_SW_STATE_COUNTER_INC(unit, module_id, location, inc_val, type, flags, dbg_string)\
    do {\
        SHR_IF_ERR_EXIT(dnxc_sw_state_dispatcher[unit][module_id].counter_inc(unit, module_id, (uint8 *)(&location), inc_val, sizeof(type), flags, dbg_string));\
    } while(0)

/**
 * \brief - decrement sw state counter value
 */
#define DNX_SW_STATE_COUNTER_DEC(unit, module_id, location, dec_val, type, flags, dbg_string)\
    do {\
        SHR_IF_ERR_EXIT(dnxc_sw_state_dispatcher[unit][module_id].counter_dec(unit, module_id, (uint8 *)(&location), dec_val, sizeof(type), flags, dbg_string));\
    } while(0)

/**
 * \brief - sw state basic memcpy
 */
#define DNX_SW_STATE_MEMCPY_BASIC(unit, module_id, destination, source, size, flags, dbg_string) \
    do {\
        SHR_IF_ERR_EXIT(dnxc_sw_state_dispatcher[unit][module_id].memcpy(unit, module_id, size, (uint8 *)(destination), (uint8 *)(source), flags, dbg_string));\
    } while(0)

/**
 * \brief - sw state memcpy
 */
#define DNX_SW_STATE_MEMCPY(unit, module_id, location, input, type, flags, dbg_string)\
    DNX_SW_STATE_MEMCPY_BASIC(unit, module_id, &(location), input, sizeof(type), flags, dbg_string);

/**
 * \brief - sw state range copy
 */
#define DNX_SW_STATE_RANGE_COPY(unit, module_id, location, input, type, num_of_elements, flags, dbg_string) \
    DNX_SW_STATE_MEMCPY_BASIC(unit, module_id, location, input, num_of_elements * sizeof(type), flags, dbg_string);

/**
 * \brief - sw state range fill
 */
#define DNX_SW_STATE_RANGE_FILL(unit, module_id, location, type, from_idx, nof_elements, value, flags, dbg_string)\
    do {\
        int i;\
        int last_idx = from_idx + nof_elements;\
        for (i = from_idx; i < last_idx; ++i) {\
            DNX_SW_STATE_MEMCPY_BASIC(unit, module_id, (type*)location + i, &value, sizeof(type), flags, dbg_string);\
        }\
    } while(0)

/**
 * \brief - check if sw state pointer is allocated
 */
#define DNX_SW_STATE_IS_ALLOC(unit, location, is_alloc, flags, dbg_string)\
    *is_alloc = (location) ? 1 : 0;

/**
 * \brief - sw state valid value range
 */
#define DNX_SW_STATE_VALID_VALUE_RANGE(unit, input, min, max)\
        do{\
            if(input < min || input > max)\
            {\
                LOG_ERROR(BSL_LS_SWSTATEDNX_GENERAL, (BSL_META_U(unit, "sw state ERROR: trying set invalid value.\n")));\
                return _SHR_E_INTERNAL;\
            }\
        } while(0)

/**
 * \brief - sw state memread
 */
#define DNX_SW_STATE_MEMREAD(unit, dst, location, offset, length, flags, debug_str)\
    sal_memcpy((void *) dst, (void *)((uint8 *)(location) + offset), length);

/**
 * \brief - sw state memcmp
 */
#define DNX_SW_STATE_MEMCMP(unit, buffer, location, offset, length, output, flags, debug_str)\
    *output = sal_memcmp((void *) buffer, (void *) ((uint8 *)(location) + offset), length);

/**
 * \brief - sw state memset
 */
#define DNX_SW_STATE_MEMSET(unit, module_id, location, offset, value, length, flags, dbg_string)\
    do { \
        SHR_IF_ERR_EXIT(dnxc_sw_state_dispatcher[unit][module_id].memset(unit, module_id, (uint8 *)(location) + offset, value, length, flags, dbg_string));\
    } while(0)

#define DNX_SW_STATE_MEMWRITE_BASIC(unit, module_id, src, src_offset, dest, dest_offset, length, flags, dbg_string)\
    do {\
        SHR_IF_ERR_EXIT(dnxc_sw_state_dispatcher[unit][module_id].memcpy(unit, module_id, length, (uint8 *)(dest) + dest_offset, (uint8 *)(src) + src_offset, flags, dbg_string));\
    } while(0)

/**
 * \brief - sw state memwrite
 */
#define DNX_SW_STATE_MEMWRITE(unit, module_id, input, location, offset, length, flags, dbg_string)\
    DNX_SW_STATE_MEMWRITE_BASIC(unit, module_id, input, 0, location, offset, length, flags, dbg_string)

/**
 * \brief - sw state mutex create
 */
#define DNX_SW_STATE_MUTEX_CREATE(unit, module_id, mtx, desc)\
    do { \
        SHR_IF_ERR_EXIT(dnxc_sw_state_dispatcher[unit][module_id].mutex_create(unit, module_id, (&(mtx)), desc, 0)); \
    } while(0)

/**
 * \brief - sw state mutex destroy
 */
#define DNX_SW_STATE_MUTEX_DESTROY(unit, module_id, mtx)\
    do { \
        SHR_IF_ERR_EXIT(dnxc_sw_state_dispatcher[unit][module_id].mutex_destroy(unit, module_id, (&(mtx)), 0)); \
    } while(0)

/**
 * \brief - sw state mutex take
 */
#define DNX_SW_STATE_MUTEX_TAKE(unit, module_id, mtx, usec)\
    do { \
        SHR_IF_ERR_EXIT(dnxc_sw_state_dispatcher[unit][module_id].mutex_take(unit, module_id, mtx, usec, 0)); \
    } while(0)

/**
 * \brief - sw state mutex give
 */
#define DNX_SW_STATE_MUTEX_GIVE(unit, module_id, mtx)\
    do { \
        SHR_IF_ERR_EXIT(dnxc_sw_state_dispatcher[unit][module_id].mutex_give(unit, module_id, mtx, 0)); \
    } while(0)

#define DNX_SW_STATE_DEFAULT_VALUE_LOOP(idx, size)\
    for(idx=0; idx<(size); idx++)

#define DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(variable, value)\
    do{\
        (variable) = (value);\
    } while(0)

#define DNX_SW_STATE_BUFFER_DEFAULT_VALUE_SET(ptr, value, _nof_bytes_to_alloc)\
    do {\
        memset((void *)(ptr), (value), _nof_bytes_to_alloc);\
    } while(0)

/* List of indexes that are being used in default value assignments loops. Value initialized per index in loop */
#define DNX_SW_STATE_DEFAULT_VALUE_DEFS\
    int def_val_idx[15] = {0}

#define DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME(src, value, e)\
    do {\
        if((value)==(e)) {\
            return src;\
        }\
    } while(0)

/**
 * \brief - begin and end santinel values
 */
#define DNX_SWSTATE_MEMORY_ALLOCATION_START_SANTINEL 0xcccccccc
#define DNX_SWSTATE_MEMORY_ALLOCATION_END_SANTINEL 0xdddddddd

/*
 * }
 */

extern void **sw_state_roots_array[SOC_MAX_NUM_DEVICES];

/*
 * TYPEDEFs
 * {
 */

typedef struct dnxc_sw_state_allocation_dll_entry_d
{
    struct dnxc_sw_state_allocation_dll_entry_d *prev;
    struct dnxc_sw_state_allocation_dll_entry_d *next;
    uint8 *ptr;
    uint8 **ptr_location;
} dnxc_sw_state_allocation_dll_entry_t;

typedef struct dnxc_sw_state_allocation_dll_d
{
    uint32 entry_counter;
    struct dnxc_sw_state_allocation_dll_entry_d *head;
    struct dnxc_sw_state_allocation_dll_entry_d *tail;
} dnxc_sw_state_allocation_dll_t;

typedef struct dnxc_sw_state_allocation_data_prefix_d
{
    uint32 start_santinel;
    dnxc_sw_state_allocation_dll_entry_t *dll_entry;
    uint32 nof_elements;
    uint32 element_size;
} dnxc_sw_state_allocation_data_prefix_t;

typedef struct dnxc_sw_state_allocation_data_suffix_d
{
    uint32 end_santinel;
} dnxc_sw_state_allocation_data_suffix_t;

/*
 * }
 */

/**
 * \brief - perform sw state module init
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] warmboot -  is warmboot supported on this unit 
 *   \param [in] sw_state_max_size - the size of the
 *   memory preallocated for sw state use
 * \par DIRECT OUTPUT:
 *   shr_error_e 
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_sw_state_init(
    int unit,
    uint32 warmboot,
    uint32 sw_state_max_size);

/**
 * \brief - perform sw state module deinit
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID 
 * \par DIRECT OUTPUT:
 *   shr_error_e 
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_sw_state_deinit(
    int unit);

/* } */
#endif /* _DNXC_SW_STATE_H */
