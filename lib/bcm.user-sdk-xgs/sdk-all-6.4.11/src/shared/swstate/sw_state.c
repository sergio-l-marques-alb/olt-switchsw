/*
 * $Id: ? $
 * $Copyright: Copyright 2016 Broadcom Corporation.
 * This program is the proprietary software of Broadcom Corporation
 * and/or its licensors, and may only be used, duplicated, modified
 * or distributed pursuant to the terms and conditions of a separate,
 * written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized
 * License, Broadcom grants no license (express or implied), right
 * to use, or waiver of any kind with respect to the Software, and
 * Broadcom expressly reserves all rights in and to the Software
 * and all intellectual property rights therein.  IF YOU HAVE
 * NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
 * IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
 * ALL USE OF THE SOFTWARE.  
 *  
 * Except as expressly set forth in the Authorized License,
 *  
 * 1.     This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use
 * all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of
 * Broadcom integrated circuit products.
 *  
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
 * PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
 * OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 * 
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 * BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
 * INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
 * ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
 * TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
 * THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR USD 1.00,
 * WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
 * ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
 *
 */

#include <assert.h>
#include <shared/bsl.h>
#include <shared/alloc.h>
#include <sal/appl/io.h>
#include <sal/appl/editline/editline.h>

#include <shared/swstate/sw_state_defs.h>
#include <shared/swstate/sw_state_utils.h>
#include <shared/swstate/access/sw_state_access.h>
#include <shared/swstate/layout/sw_state_layout.h>
#include <shared/swstate/layout/sw_state_defs_layout.h>
#include <shared/swstate/sw_state.h>
#include <shared/swstate/sw_state_hash_tbl.h>
#include <shared/swstate/sw_state_sync_db.h>

#ifdef SW_STATE_ISSU_DIAG
#include <unistd.h>
#include <time.h>
#define SW_STATE_PATH_LEN 256
#endif 

#include <soc/types.h>
#include <soc/error.h>
#include <soc/scache.h>

#ifdef MEMORY_MEASUREMENT_DIAGNOSTICS
#include <shared/mem_measure_tool.h>
#endif

#include <soc/dpp/SAND/Utils/sand_os_interface.h> /* for qsort */

#define SW_STATE_MEMORY_OVERRIDE_CHECK(_unit, location, length)\
    do{ \
        if (shr_sw_state_data_block_header[_unit]->data_ptr > location) {\
            _sw_state_access_debug_hook(5);\
            LOG_ERROR(BSL_LS_SHARED_SWSTATE, (BSL_META_U(_unit,\
            "sw state ERROR: potential memory override\n")));\
            return _SHR_E_MEMORY;\
        }\
        if (shr_sw_state_data_block_header[_unit]->data_ptr + shr_sw_state_data_block_header[_unit]->data_size < location + length) {\
            _sw_state_access_debug_hook(5);\
            LOG_ERROR(BSL_LS_SHARED_SWSTATE, (BSL_META_U(_unit,\
            "sw state ERROR: potential memory override\n")));\
            return _SHR_E_MEMORY;\
        }\
    } while(0)

/**************************** shr_sw_state_alloc **************/
/* Definitions related to alloc/free within sw_state          */
/**************************************************************/
#define SW_STATE_ALLOC_RESOURCE_USAGE_INCR(a_curr, a_max, a_size)
#define SW_STATE_ALLOC_RESOURCE_USAGE_DECR(a_curr, a_size)

/* When we allocate memory in the sw_state data block
   for an element, we need additional space for memory
   management (for example for saving the element's allocated size).
   We use this define in the macro SW_STATE_IS_ENOUGH_SPACE_CHECK
   that checks if there is enough memory to allocate
   a new element in the sw_state and decide whether or not to
   perform defragmentaion of the sw_state data block. */
#define SW_STATE_MARGIN_OF_SAFETY 100 /* Bytes */

#ifdef BROADCOM_DEBUG
/* { */
#ifndef __KERNEL__
/* { */
#if SW_STATE_MEM_MONITOR
/* { */
static unsigned int _sw_state_alloc_max = 0 ;
static unsigned int _sw_state_alloc_curr = 0 ;

#undef SW_STATE_ALLOC_RESOURCE_USAGE_INCR
#undef SW_STATE_ALLOC_RESOURCE_USAGE_DECR
#define SW_STATE_ALLOC_RESOURCE_USAGE_INCR(a_curr, a_max, a_size) \
        a_curr += (a_size);                                       \
        a_max = ((a_curr) > (a_max)) ? (a_curr) : (a_max)

#define SW_STATE_ALLOC_RESOURCE_USAGE_DECR(a_curr, a_size) \
        a_curr -= (a_size)                                        \

/*
 * Function:
 *      sw_state_alloc_resource_usage_get
 * Purpose:
 *      Provides Current/Maximum memory allocation by sw_state.
 * Parameters:
 *      alloc_curr - Current memory usage.
 *      alloc_max  - Memory usage high water mark
 */
void 
sw_state_alloc_resource_usage_get(uint32 *alloc_curr, uint32 *alloc_max)
{
    if (alloc_curr != NULL) {
        *alloc_curr = _sw_state_alloc_curr ;
    }
    if (alloc_max != NULL) {
        *alloc_max = _sw_state_alloc_max ;
    }
}
/*
 * Function:
 *      sw_state_alloc_resource_usage_init
 * Purpose:
 *      Initialize Current/Maximum memory allocation counters of sw_state
 *      (set to zero).
 * Parameters:
 *      Direct:
 *          None
 *      Indirect:
 *          _sw_state_alloc_curr
 *          _sw_state_alloc_max
 */
void 
sw_state_alloc_resource_usage_init(void)
{
    _sw_state_alloc_curr = _sw_state_alloc_max = 0 ;
    return ;
}
/* } */
#endif
/* } */
#endif
/* } */
#endif

/**************************** layout **************************/
/* header/ptr-list/data/transaction-ptr-list/transaction-data */
/**************************************************************/

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_LS_SHARED_SWSTATE

shr_sw_state_data_block_header_t           *shr_sw_state_data_block_header[SOC_MAX_NUM_DEVICES];

shr_sw_state_t *sw_state[BCM_MAX_NUM_UNITS];

void 
_sw_state_access_debug_hook(int id) {
    /* 
     * this function is used for debug purposes
     * use breakepoint on this function in order to easily catch sw_state errors 
     * use id value for conditioned breakpoints 
     * 0 - SW_STATE_NULL_CHECK
     * 1 - SW_STATE_IS_WARM_BOOT_CHECK
     * 2 - SW_STATE_IS_DEINIT_CHECK
     * 3 - SW_STATE_IS_ALREADY_ALLOCATED_CHECK
     * 4 - SW_STATE_OUT_OF_BOUND_CHECK 
     * 5 - MEMORY OVERRIDE RISK 
     */
    return;
}

/* check if the arch is little endian */
int sw_state_is_little_endian(void) {
    int i = 1;
    char *p = (char *)&i;

    if (p[0] == 1)
        return 1;
    else
        return 0;
}


/* Copy numbers with keeping in mind endianness */
void* sw_state_copy_endian_independent ( void* destination, const void* source, size_t dst_size, size_t src_size, size_t size ) {
    if (sw_state_is_little_endian()) {
        sal_memcpy(destination, source, size);
    } else {
        sal_memcpy((uint8*)destination + dst_size - size, (uint8*)source + src_size - size, size);
    }
    return destination;
}
 
void
shr_sw_state_print_all_pointers(shr_sw_state_data_block_header_t block_header)
{
    shr_sw_state_alloc_element_t      *p;
    int                                i = 0;

    p = block_header.ptr_offsets_sp;
    ++p;

    while (p <= block_header.ptr_offsets_stack_base) {
        cli_out("#%d: relative offset %d, relative value %p\n", i, p->ptr_offset, (void *) (p->ptr_value - block_header.data_ptr));
        p++;
        i++;
    }
}

/*
 * current_pointer_in_sw_state: pointer of the current elt in sw state to auto sync
 * size: size of the sw state to auto sync 
 */
int 
shr_sw_state_auto_sync(int unit, uint8* current_pointer_in_sw_state, int size) 
{
    soc_scache_handle_t                wb_handle_orig = 0;
    int                                offset; 

    SOC_INIT_FUNC_DEFS;

    /* perform auto sync only if autosync is enabled */
    if (SOC_AUTOSYNC_IS_ENABLE(unit)) {
        /* get the wb buffer handle */
        SHR_SW_STATE_SCACHE_HANDLE_SET(wb_handle_orig, unit, SHR_SW_STATE_SCACHE_HANDLE_DATA_BLOCK);

        /* get offset */
        offset = ((uint8*) current_pointer_in_sw_state) - ((uint8*) shr_sw_state_data_block_header[unit]); 

        /* perform auto sync on the specific var that has just been set */
        _SOC_IF_ERR_EXIT(shr_sw_state_scache_sync(unit, wb_handle_orig, offset, size)); 
    }

exit:
    SOC_FUNC_RETURN;
}

int shr_sw_state_deinit(int unit) {
    SOC_INIT_FUNC_DEFS;

    sal_free(sw_state_sync_db[unit].dpp.htb_locks);

    _SOC_IF_ERR_EXIT(sw_state_htb_deinit(unit));
    sw_state[unit] = NULL;

#ifndef BCM_WARM_BOOT_SUPPORT
    sal_free(shr_sw_state_data_block_header[unit]->ptr_to_free);
#endif

exit:
    SOC_FUNC_RETURN;
}


/* helper function:
 * allocated_size: allocated size in byte
   return allocated size for allocation done by shr_sw_state_alloc */
int 
shr_sw_state_allocated_size_get(int unit, uint8* ptr, uint32* allocated_size) {

    uint32* p;  

    SOC_INIT_FUNC_DEFS;

    /* get the number of elements allocated (if simple pointer then 1, if was array then n):
     * alloc data is 0x<size_alloc>CCCCCCCC<data>DDDDDDDD
       */
    p = (uint32*) ptr; 

    /* make sure ptr is an allocated element */
    if ( p[-1] != 0xcccccccc || p[p[-2]] != 0xdddddddd) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                       (BSL_META_U(unit,
                          "Unit:%d ptr is an invalid allocated pointer \n"), unit));
    }

    /* p got the size in 32b. allocated_size is in byte. */
    *allocated_size =  p[-2] * 4; 

    SOC_EXIT;

exit:
    SOC_FUNC_RETURN;
}

/*********************************************************************
 * NAME:
 *   shr_sw_state_memset
 * FUNCTION:
 *   modify data inside the sw state data block by copying external
 *   data input into it.
 * INPUT: 
 *   unit
 *   location - a pointer to a location in the data block
 *   input - a value to memset
 *   length - nof bytes to set.
 * RETURNS:
 *   ERROR value.
*********************************************************************/
int
shr_sw_state_memset(int _unit, uint8 *location, int input, uint32 length, uint32 flags){

    uint8 is_modified = FALSE;
    SW_STATE_INIT_FUNC_DEFS;

    if (flags & SW_STATE_IS_WB_CHECK) {
        SW_STATE_IS_WARM_BOOT_CHECK();
    }
    SW_STATE_IS_DEINIT_CHECK();

    SW_STATE_MEMORY_OVERRIDE_CHECK(_unit, location, length);

    sal_memset(location, input, length);

    if (is_modified && SW_STATE_DO_AUTOSYNC(flags & SW_STATE_ALLOW_AUTOSYNC)){
        shr_sw_state_auto_sync(_unit, location, length);
    }

    SW_STATE_FUNC_RETURN;
}


/*********************************************************************
 * NAME:
 *   shr_sw_state_memcpy
 * FUNCTION:
 *   modify data inside the sw state data block by copying external
 *   data input into it.
 * INPUT: 
 *   unit
 *   location - a pointer to a location in the data block
 *   input - a pointer to the data that need to be written
 *   length - nof bytes to copy.
 * RETURNS:
 *   ERROR value.
*********************************************************************/
int
shr_sw_state_memcpy(int _unit, uint8 *location, uint8 *input, uint32 length, uint32 flags){

    uint8 is_modified = FALSE;
    SW_STATE_INIT_FUNC_DEFS;

#ifdef SW_STATE_AUTOSYNC_OPTIMIZATION
    if (sal_memcmp(location, input, length)) {
        is_modified = TRUE;
    }
#endif

    if (flags & SW_STATE_IS_WB_CHECK) {
        SW_STATE_IS_WARM_BOOT_CHECK();
    }
    SW_STATE_IS_DEINIT_CHECK();

    SW_STATE_MEMORY_OVERRIDE_CHECK(_unit, location, length);

    sal_memcpy(location, input, length);

    if (is_modified && SW_STATE_DO_AUTOSYNC(flags & SW_STATE_ALLOW_AUTOSYNC)){
        shr_sw_state_auto_sync(_unit, location, length);
    }

    SW_STATE_FUNC_RETURN;
}


/*********************************************************************
 * NAME:
 *   shr_sw_state_pbmp_operation
 * FUNCTION:
 *   modify/retrieve port bitmap data in the sw state data block 
 * INPUT: 
 *   unit
 *   location - a pointer to a location in the data block
 *   input - a pointer to the data that need to be written
 *   length - nof bytes to copy.
 * RETURNS:
 *   ERROR value.
*********************************************************************/
int
shr_sw_state_pbmp_operation(int _unit, _shr_pbmp_t *path, SW_ST_PBMP op, _shr_pbmp_t *input_pbmp,
                            _shr_pbmp_t *output_pbmp, int input_port, int range, uint8 *result,
                            int *countr, char* buffer, uint32 flags, char *debug_string){

    uint8 need_autosync = 0;
    _shr_pbmp_t tmp_pbmp;

    SW_STATE_INIT_FUNC_DEFS;

    _SHR_PBMP_CLEAR(tmp_pbmp);
    _SHR_PBMP_OR(tmp_pbmp, *((_shr_pbmp_t *) path));;

    switch (op){
    case SW_ST_PBMP_NEQ :
        *result = (_SHR_PBMP_NEQ(*((_shr_pbmp_t *) path), *input_pbmp) > 0) ? 1: 0;
        break;
    case SW_ST_PBMP_EQ :
        *result = (_SHR_PBMP_EQ(*((_shr_pbmp_t *) path), *input_pbmp) > 0) ? 1: 0;
        break;
    case SW_ST_PBMP_MEMBER :
        *result = (_SHR_PBMP_MEMBER(*((_shr_pbmp_t *) path), input_port) > 0) ? 1: 0;
        break;
    case SW_ST_PBMP_NOT_NULL :
        *result = (_SHR_PBMP_NOT_NULL(*((_shr_pbmp_t *) path)) > 0) ? 1: 0;
        break;
    case SW_ST_PBMP_IS_NULL :
        *result = (_SHR_PBMP_IS_NULL(*((_shr_pbmp_t *) path)) > 0) ? 1: 0;
        break;
    case SW_ST_PBMP_COUNT :
        _SHR_PBMP_COUNT(*((_shr_pbmp_t *) path), *countr);
        break;
    case SW_ST_PBMP_XOR :
        SW_STATE_MODIFY_OPS_CHECKS(flags);
        _SHR_PBMP_XOR(*((_shr_pbmp_t *) path), *input_pbmp);
        need_autosync = 1;
        break;
    case SW_ST_PBMP_REMOVE :
        SW_STATE_MODIFY_OPS_CHECKS(flags);
        _SHR_PBMP_REMOVE(*((_shr_pbmp_t *) path), *input_pbmp);
        need_autosync = 1;
        break;
    case SW_ST_PBMP_ASSIGN :
        SW_STATE_MODIFY_OPS_CHECKS(flags);
        _SHR_PBMP_ASSIGN(*((_shr_pbmp_t *) path), *input_pbmp);
        need_autosync = 1;
        break;
    case SW_ST_PBMP_GET :
        _SHR_PBMP_ASSIGN(*output_pbmp, *((_shr_pbmp_t *) path));
        break;
    case SW_ST_PBMP_AND :
        SW_STATE_MODIFY_OPS_CHECKS(flags);
        _SHR_PBMP_AND(*((_shr_pbmp_t *) path), *input_pbmp);
       need_autosync = 1;
        break;
    case SW_ST_PBMP_NEGATE :
        SW_STATE_MODIFY_OPS_CHECKS(flags);
        _SHR_PBMP_NEGATE(*((_shr_pbmp_t *) path), *input_pbmp);
        need_autosync = 1;
        break;
    case SW_ST_PBMP_OR :
        SW_STATE_MODIFY_OPS_CHECKS(flags);
        _SHR_PBMP_OR(*((_shr_pbmp_t *) path), *input_pbmp);
        need_autosync = 1;
        break;
    case SW_ST_PBMP_CLEAR :
        SW_STATE_MODIFY_OPS_CHECKS(flags);
        _SHR_PBMP_CLEAR(*((_shr_pbmp_t *) path));
        need_autosync = 1;
        break;
    case SW_ST_PBMP_PORT_ADD :
        SW_STATE_MODIFY_OPS_CHECKS(flags);
        _SHR_PBMP_PORT_ADD(*((_shr_pbmp_t *) path), input_port);
        need_autosync = 1;
        break;
    case SW_ST_PBMP_PORT_FLIP :
        SW_STATE_MODIFY_OPS_CHECKS(flags);
        _SHR_PBMP_PORT_FLIP(*((_shr_pbmp_t *) path), input_port);
        need_autosync = 1;
        break;
    case SW_ST_PBMP_PORT_REMOVE :
        SW_STATE_MODIFY_OPS_CHECKS(flags);
        _SHR_PBMP_PORT_REMOVE(*((_shr_pbmp_t *) path), input_port);
        need_autosync = 1;
        break;
    case SW_ST_PBMP_PORT_SET :
        SW_STATE_MODIFY_OPS_CHECKS(flags);
        _SHR_PBMP_PORT_SET(*((_shr_pbmp_t *) path), input_port);
        need_autosync = 1;
        break;
    case SW_ST_PBMP_PORTS_RANGE_ADD :
        SW_STATE_MODIFY_OPS_CHECKS(flags);
        _SHR_PBMP_PORTS_RANGE_ADD(*((_shr_pbmp_t *) path), input_port, range);
        need_autosync = 1;
        break;
    case SW_ST_PBMP_FMT :
        SW_STATE_MODIFY_OPS_CHECKS(flags);
        _SHR_PBMP_FMT(*((_shr_pbmp_t *) path), buffer);
        break;
    default :
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                       (BSL_META_U(_unit,
                          "_unit:%d sw state ERROR: trying to perform a PBMP operation with non-existing op code\n"), _unit));

    }

    if (need_autosync && !_SHR_PBMP_BMEQ(*path, tmp_pbmp)) {
        shr_sw_state_auto_sync(_unit,(uint8 *) path, sizeof(bcm_pbmp_t));
    }

    SW_STATE_FUNC_RETURN;
}

/*********************************************************************
 * NAME:
 *   shr_sw_state_alloc
 * FUNCTION:
 *   allocate space from the sw_state data block. space is not allocated by the operating system.
 *   a big chunk of memory was allocated during init in order to store all the future sw state.
 *   this function carves a space of size 'size' out of this data block and store the pointer to it
 *   inside ptr.
 * INPUT: 
 *   _unit
 *   size - size to allocate
 * OUTPUT:
 *   ptr - ptr to the data allocated space
 * RETURNS:
 *   ERROR value.
*********************************************************************/
int
shr_sw_state_alloc(int _unit, uint8** ptr, uint32 size, uint32 flags) {

     /********************************************************* 
                  data_ptr
        __________/________________________
        | header |........................|
        |......size|ccc|data|ddd          |
        |                       /         |
        |                      /          |
        |   next free data slot           |
        |                       x   x   x |
        |_____________________|___|___|___|
                            /           \ 
                           /             \
                 sp - stack pointer    the stack's base
     
     ***********************************************************/ 

    unsigned int  alloc_size = 0;
    uint32                *p = NULL;

#ifdef MEMORY_MEASUREMENT_DIAGNOSTICS
    unsigned int  idx;
#endif

    SOC_INIT_FUNC_DEFS;

    SW_STATE_MODIFY_OPS_CHECKS(flags);

    /* return error if pointer is allready allocated */
    SW_STATE_IS_ALREADY_ALLOCATED_CHECK(*ptr);

    /* return error if ptr is not in range*/
    if ((((uint8 *)ptr) < shr_sw_state_data_block_header[_unit]->data_ptr)
        || (((uint8 *) ptr) > shr_sw_state_data_block_header[_unit]->next_free_data_slot)) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                       (BSL_META_U(_unit,
                          "_unit:%d trying to allocate a pointer and save it outside of the sw_state scope\n"), _unit));
    }


    /* data is 4 bytes alligned */
    SHR_SW_STATE_ALIGN_SIZE(size);

    /*
       |  4 bytes  |     4 bytes     | size bytes |     4 bytes    |
       |    size   |    0xcccccccc   |    data    |   0xdddddddd   |
    */
    alloc_size = size + 12; /* 4 + 4 + size + 4 */

    if (shr_sw_state_data_block_header[_unit]->size_left < alloc_size + SW_STATE_MARGIN_OF_SAFETY) {
       _SOC_IF_ERR_EXIT(shr_sw_state_defrag(_unit, &ptr));
    }

    /* if not enough space exit with error */
    if (shr_sw_state_data_block_header[_unit]->size_left < alloc_size) {
        unsigned long size_left ;
        unsigned long total_buffer_size ;

        size_left = (unsigned long)(shr_sw_state_data_block_header[_unit]->size_left) ;
        total_buffer_size = (unsigned long)(shr_sw_state_data_block_header[_unit]->total_buffer_size) ;
        _SOC_EXIT_WITH_ERR(SOC_E_MEMORY,
               (BSL_META_U(_unit,
                  "_unit:%d shr_sw_state_alloc: not enough memory for allocation. total %lu size_left %lu alloc_size %lu\n"),
                                                    _unit,total_buffer_size,size_left,(unsigned long)alloc_size)) ;
    }

    p = (uint32 *) shr_sw_state_data_block_header[_unit]->next_free_data_slot;

    shr_sw_state_data_block_header[_unit]->next_free_data_slot += alloc_size;
    /* auto sync support */
    shr_sw_state_auto_sync(_unit, 
                           ((void*) &shr_sw_state_data_block_header[_unit]->next_free_data_slot),
                           sizeof(uint8*)); 


    /* space consumption = data size + alloc_element size (in the stack)*/
    shr_sw_state_data_block_header[_unit]->size_left -= (alloc_size + sizeof(shr_sw_state_alloc_element_t));
    /* auto sync support */
    shr_sw_state_auto_sync(_unit, 
                           ((void*) &shr_sw_state_data_block_header[_unit]->size_left),
                           sizeof(uint32)); 



    assert(UINTPTR_TO_PTR(PTR_TO_UINTPTR(p)) == p);

    p[0] = size / 4; /* size in 32b */
    p[1] = 0xcccccccc;
    p[2 + size / 4] = 0xdddddddd;

#ifdef MEMORY_MEASUREMENT_DIAGNOSTICS
    MEMORY_MEASUREMENT_INITIALIZE;
    for(idx = 0;idx < memory_measurement_tool.count;idx++) {
        if(memory_measurement_tool.elements[idx].is_active && (memory_measurement_tool.elements[idx].thread_id == sal_thread_self())) {
            memory_measurement_tool.elements[idx].sw_state_size += size;
        }
    }
#endif

#ifdef BROADCOM_DEBUG
/* { */
#ifndef __KERNEL__
/* { */
#if SW_STATE_MEM_MONITOR
/* { */
    SW_STATE_ALLOC_RESOURCE_USAGE_INCR(
        _sw_state_alloc_curr,
        _sw_state_alloc_max,
        (size));
/* } */
#endif
/* } */
#endif
/* } */
#endif /* BROADCOM_DEBUG */

    *ptr = (uint8 *) &p[2];

    /* auto sync support */
    shr_sw_state_auto_sync(_unit, 
                           ((void*) p),
                           alloc_size); 


    shr_sw_state_data_block_header[_unit]->ptr_offsets_sp->ptr_offset = 
        (((uint8 *) ptr) - shr_sw_state_data_block_header[_unit]->data_ptr);

    shr_sw_state_data_block_header[_unit]->ptr_offsets_sp->ptr_value = *ptr;

    /* auto sync support */
    shr_sw_state_auto_sync(_unit, 
                           ((void*) shr_sw_state_data_block_header[_unit]->ptr_offsets_sp),
                           sizeof(shr_sw_state_alloc_element_t)); 

    --shr_sw_state_data_block_header[_unit]->ptr_offsets_sp;

    /* auto sync support */
    shr_sw_state_auto_sync(_unit, 
                           ((void*) &(shr_sw_state_data_block_header[_unit]->ptr_offsets_sp)),
                           sizeof(shr_sw_state_alloc_element_t*)); 

exit:
    SOC_FUNC_RETURN;
}

/*********************************************************************
 * NAME:
 *   shr_sw_state_free
 * FUNCTION:
 *   free space that was alloced using shr_sw_state_alloc
 * INPUT: 
 *   _unit
 *   ptr - ptr to the data that should be freed*   
 * RETURNS:
 *   ERROR value.
*********************************************************************/
int
shr_sw_state_free(int _unit, uint8 *ptr, uint32 flags) {

    uint8                        is_allocated = 0;
    uint8                        **freed_location = NULL;
    shr_sw_state_alloc_element_t *p = NULL;
    shr_sw_state_alloc_element_t *stack_base = NULL;
    uint32                       size = 0;
#ifdef MEMORY_MEASUREMENT_DIAGNOSTICS
    uint32 idx;
    uint32 *ap_mem ;
#endif
    SOC_INIT_FUNC_DEFS;

#ifdef MEMORY_MEASUREMENT_DIAGNOSTICS
    ap_mem = (uint32 *)ptr ;

    for(idx = 0;idx < memory_measurement_tool.count;idx++) {
        if(memory_measurement_tool.elements[idx].is_active && (memory_measurement_tool.elements[idx].thread_id == sal_thread_self())) {
            memory_measurement_tool.elements[idx].sw_state_size -= (ap_mem[-2] * 4);
        }
    }
#endif

    
#ifdef BROADCOM_DEBUG
/* { */
#ifndef __KERNEL__
/* { */
#if SW_STATE_MEM_MONITOR
/* { */
    {
        uint32 *ap ;
        ap = (uint32 *)ptr ;
        SW_STATE_ALLOC_RESOURCE_USAGE_DECR(_sw_state_alloc_curr,(ap[-2] * 4));
    }
/* } */
#endif
/* } */
#endif
/* } */
#endif /* BROADCOM_DEBUG */

    if (flags & SW_STATE_IS_WB_CHECK) {
        SW_STATE_IS_WARM_BOOT_CHECK();
    }
    SW_STATE_IS_DEINIT_CHECK();

    stack_base = shr_sw_state_data_block_header[_unit]->ptr_offsets_stack_base;
            
    /* return error if ptr is not in range*/
    if (( ptr < shr_sw_state_data_block_header[_unit]->data_ptr )
        || ( ptr > shr_sw_state_data_block_header[_unit]->next_free_data_slot )) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                       (BSL_META_U(_unit,
                          "_unit:%d trying to free a pointer outside of the sw_state scope\n"), _unit));
    }
        
    p = shr_sw_state_data_block_header[_unit]->ptr_offsets_sp;
    ++p;

    while (p <= stack_base) {
        if (((uint8 *)p->ptr_value == ptr)) {
            is_allocated = 1;
            /* mark as NULL in actual pointer uses */
            freed_location = (uint8 **) 
                (shr_sw_state_data_block_header[_unit]->data_ptr + p->ptr_offset);
            *freed_location = NULL;
            shr_sw_state_auto_sync(_unit, 
                           ((void*) freed_location),
                           sizeof(uint8*)); 

            /* zero the freed memory */
            _SOC_IF_ERR_EXIT(shr_sw_state_allocated_size_get(_unit, (uint8*)ptr, &size));
            size = size + sizeof(uint32) * 3;
            sal_memset((uint32*)ptr - 2, 0, size);
            shr_sw_state_auto_sync(_unit, 
                           ((void*) ((uint32*)ptr - 2)), size); 

            /* auto sync support */

            break;
        }
        p++;
    }
    

    if (is_allocated == 0) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                       (BSL_META_U(_unit,
                          "_unit:%d trying to free an invalid pointer\n"), _unit));
    }

    /* fill the hole in the stack and pop the stack */
    if (p != shr_sw_state_data_block_header[_unit]->ptr_offsets_sp + 1) {
        p->ptr_value = (shr_sw_state_data_block_header[_unit]->ptr_offsets_sp + 1)->ptr_value;
        p->ptr_offset = (shr_sw_state_data_block_header[_unit]->ptr_offsets_sp + 1)->ptr_offset;
        shr_sw_state_auto_sync(_unit, 
               ((void*) freed_location),
               sizeof(uint8*)); 
    }
    /* pop the stack */
    shr_sw_state_data_block_header[_unit]->ptr_offsets_sp ++;

    /* auto sync support */
    shr_sw_state_auto_sync(_unit, 
                           ((void*) &(shr_sw_state_data_block_header[_unit]->ptr_offsets_sp)),
                           sizeof(shr_sw_state_alloc_element_t*));
    SOC_EXIT;
exit:
    SOC_FUNC_RETURN;
}

/*********************************************************************
 * NAME:
 *   shr_sw_state_layout_create
 * FUNCTION:
 *   init sw state layout (struct description) for sw state header. 
 * INPUT: 
 *   unit
 * OUTPUT 
 *   nbr_of_layout_nodes
 * RETURNS:
 *   ERROR value.
*********************************************************************/
int shr_sw_state_layout_create(int unit, uint32* nbr_of_layout_nodes) {

    int root_node_id = 0; 

    SOC_INIT_FUNC_DEFS;

    /* initialize the nof nodes */
    *nbr_of_layout_nodes = 0; 

    _SOC_IF_ERR_EXIT(sw_state_layout_node_create(unit, &root_node_id, nbr_of_layout_nodes)); 

exit:
    SOC_FUNC_RETURN;
}

/* print the differences between the node before ISSU and after ISSU */

static int shr_sw_state_issu_layout_nodes_diff_print(
   int                              unit, 
   shr_sw_state_data_block_header_t *retrieved_sw_state_data_block_header, 
   shr_sw_state_data_block_header_t *updated_sw_state_data_block_header, 
   shr_sw_state_ds_layout_node_t    *retrieved_layout_node, 
   shr_sw_state_ds_layout_node_t    *updated_layout_node) {
     int i = 0, j = 0, is_exist = 0,
         retrieved_node_first_child = retrieved_layout_node->first_child_node_index,
         retrieved_node_last_child = retrieved_layout_node->last_child_node_index,
         updated_node_first_child = updated_layout_node->first_child_node_index,
         updated_node_last_child = updated_layout_node->last_child_node_index;

     SOC_INIT_FUNC_DEFS;
     /* check if the variable's type has changed */
     if (sal_strcmp(retrieved_layout_node->type,updated_layout_node->type) != 0) {
         LOG_WARN(_ERR_MSG_MODULE_NAME, (BSL_META("Unit:%d SW_STATE_ISSU: variable %s type has changed from %s"
             " to %s.\n"), unit, retrieved_layout_node->name, retrieved_layout_node->type, updated_layout_node->type));
     } else if ((retrieved_layout_node->first_child_node_index == 0 || updated_layout_node->first_child_node_index == 0) &&
         (retrieved_layout_node->size != updated_layout_node->size)){
         LOG_WARN(_ERR_MSG_MODULE_NAME, (BSL_META("Unit:%d SW_STATE_ISSU: variable %s size has changed from %d"
             " to %d.\n"), unit, retrieved_layout_node->name, retrieved_layout_node->size, updated_layout_node->size));
     }

     /* check array sizes changes */
     for (i=0; i < sizeof(retrieved_layout_node->array_sizes)/sizeof(retrieved_layout_node->array_sizes[0]); ++i) {
         if (retrieved_layout_node->array_sizes[i] != updated_layout_node->array_sizes[i]) {
             LOG_WARN(_ERR_MSG_MODULE_NAME, (BSL_META("Unit:%d SW_STATE_ISSU: variable %s array_sizes[%d] has changed from %d"
                 " to %d.\n"), unit, retrieved_layout_node->name, i, retrieved_layout_node->array_sizes[i], updated_layout_node->array_sizes[i]));
         }
     }
     /* print the deleted children */
     for (i = retrieved_node_first_child; i < retrieved_node_last_child; i = retrieved_sw_state_data_block_header->ds_layout_nodes[i].next_brother_node_index) {
         is_exist = 0;
         for (j = updated_node_first_child; j < updated_node_last_child; j = updated_sw_state_data_block_header->ds_layout_nodes[j].next_brother_node_index) {
             if (sal_strcmp(retrieved_sw_state_data_block_header->ds_layout_nodes[i].name,
                            updated_sw_state_data_block_header->ds_layout_nodes[j].name) == 0) {
                 is_exist = 1;
                 break;
             }
         }
         if (is_exist == 0) {
         LOG_WARN(_ERR_MSG_MODULE_NAME, (BSL_META("Unit:%d SW_STATE_ISSU: variable %s, %s has been deleted.\n"),
             unit, retrieved_layout_node->name, retrieved_sw_state_data_block_header->ds_layout_nodes[i].name));
         }
     }

     /* print the newly added children */
     for (i = updated_node_first_child; i < updated_node_last_child; i = updated_sw_state_data_block_header->ds_layout_nodes[i].next_brother_node_index) {
         is_exist = 0;
         for (j = retrieved_node_first_child; j < retrieved_node_last_child; j = retrieved_sw_state_data_block_header->ds_layout_nodes[j].next_brother_node_index) {
             if (sal_strcmp(updated_sw_state_data_block_header->ds_layout_nodes[i].name,
                            retrieved_sw_state_data_block_header->ds_layout_nodes[j].name) == 0) {
                 is_exist = 1;
                 break;
             }
         }
         if (is_exist == 0) {
         LOG_WARN(_ERR_MSG_MODULE_NAME, (BSL_META("Unit:%d SW_STATE_ISSU: variable %s, %s has been added.\n"),
             unit, updated_layout_node->name, updated_sw_state_data_block_header->ds_layout_nodes[i].name));
         }
     }
     SOC_FUNC_RETURN;
}

/*********************************************************************
 * NAME:
 *   shr_sw_state_issu_update
 * FUNCTION:
 *   update the sw state data block after ISSU, using retreived sw state block
 *   Precondition: both value pointed by the current address are compatible, and value before ISSU can update value after ISSU.
 *                 current nodes describe the value pointed by the current address.
 *                 values being compatible mean they have the same nof pointers, the same nof array dimension.
 *   Limitations: support up to 2 dimensions arrays. 
 * INPUT: 
 *   unit
 *   retrieved_sw_state_data_block_header - sw state block saved in FILE before ISSU.
 *   updated_sw_state_data_block_header - sw state block to use after ISSU, updated using retrieved_sw_state_data_block_header.
 *   retrieved_layout_node - current node before ISSU 
 *   updated_layout_node - current node after ISSU 
 *   retrieved_address - address of the data in the data block. The retrieved layout node describe the data pointed by this address.
 *   updated_address - address of the node in the data block. The updated layout node describe the data pointed by this address.
 *   current_level_pointer - indicate if pointer have already been treated.
 *                           ex: 0 indicate no pointer have been treated, 1: indicate 1 pointer have been treated.
 *                           ex: for int*, layout_node.nof_pointer = 1, If current_level_pointer 1 => will be treated as int. 
 * RETURNS:
 *   ERROR value.
*********************************************************************/
int shr_sw_state_issu_update_internal(
   int                              unit, 
   shr_sw_state_data_block_header_t *retrieved_sw_state_data_block_header, 
   shr_sw_state_data_block_header_t *updated_sw_state_data_block_header, 
   shr_sw_state_ds_layout_node_t    *retrieved_layout_node, 
   shr_sw_state_ds_layout_node_t    *updated_layout_node, 
   uint8                            *retrieved_address, 
   uint8                            *updated_address, 
   uint32                              current_level_pointer, 
   uint8                            is_array_already_processed,
   uint8                            is_already_printed
   ) {
    SOC_INIT_FUNC_DEFS;

    /* print layout differences */
    if (!is_already_printed) { 
        shr_sw_state_issu_layout_nodes_diff_print(unit,
            retrieved_sw_state_data_block_header, updated_sw_state_data_block_header,
            retrieved_layout_node, updated_layout_node);
    }
    /* current nodes are array and have not been processed yet */
    if ((updated_layout_node->array_sizes[0] > 0) && !is_array_already_processed) {
		/* In case of member of the type "type* arr[]" the size of each element in
		  the array equals to the size of pointer in the system*/
		int retrieved_layout_arr_element_size = (retrieved_layout_node->nof_pointer > 0) ? sizeof(uint8*) : retrieved_layout_node->size;
		int updated_layout_arr_element_size = (updated_layout_node->nof_pointer > 0) ? sizeof(uint8*) : updated_layout_node->size;
        /* 1 dimension array*/
        if (updated_layout_node->array_sizes[1] == 0) {
            int array_index; 
            int array_size = SOC_SAND_MIN(retrieved_layout_node->array_sizes[0], updated_layout_node->array_sizes[0]); 
            for (array_index = 0; array_index < array_size; array_index++) {
               /* compare and update the element of the array */
               _SOC_IF_ERR_EXIT(shr_sw_state_issu_update_internal(unit, 
                     retrieved_sw_state_data_block_header, 
                     updated_sw_state_data_block_header, 
                     retrieved_layout_node, 
                     updated_layout_node, 
                     retrieved_address + array_index * retrieved_layout_arr_element_size,  
                     updated_address + array_index * updated_layout_arr_element_size,
                     current_level_pointer, 
                     TRUE,   /* array has been processed */
                     TRUE)); /* diff already printed */  
            }
        } 
        /* 2 dimension array */
        else {
            int array_index_0, array_index_1; 
            int min_array_length_0 = SOC_SAND_MIN(retrieved_layout_node->array_sizes[0], updated_layout_node->array_sizes[0]); 
            int min_array_length_1 = SOC_SAND_MIN(retrieved_layout_node->array_sizes[1], updated_layout_node->array_sizes[1]); 
            int retrieved_array_length_1 = retrieved_layout_node->array_sizes[1]; 
            int updated_array_length_1 = updated_layout_node->array_sizes[1]; 

            for (array_index_0 = 0; array_index_0 < min_array_length_0; array_index_0++) {
                for (array_index_1 = 0; array_index_1 < min_array_length_1; array_index_1++) {
                    /* compare and update the element of the array */
                   _SOC_IF_ERR_EXIT(shr_sw_state_issu_update_internal(unit, 
                                                                      retrieved_sw_state_data_block_header, 
                                                                      updated_sw_state_data_block_header, 
                                                                      retrieved_layout_node, 
                                                                      updated_layout_node, 
                                                                      retrieved_address + ((array_index_0 * retrieved_array_length_1 + array_index_1) * retrieved_layout_arr_element_size),  
                                                                      updated_address + ((array_index_0 * updated_array_length_1 + array_index_1) * updated_layout_arr_element_size),
                                                                      current_level_pointer, 
                                                                      TRUE,    /* array has been processed */
                                                                      TRUE));  /* diff already printed */
                }
            }
        }
        SOC_EXIT;  
    }


    /* current nodes are pointers */
    if (retrieved_layout_node->nof_pointer > current_level_pointer) {
        uint32 allocated_size; 
        uint32 nof_allocated_elements; 
        int allocated_element_index; 
        uint8** retrieved_address_p = (uint8**) retrieved_address; 
        uint8** updated_address_p = (uint8**) updated_address; 
        /* indicate if it's a single pointer. It's relative to the current_level_pointer.*/
        uint8 is_single_pointer = ((retrieved_layout_node->nof_pointer - current_level_pointer) == 1); 

        /* check if pointer value before ISSU was null */
        if (*retrieved_address_p == 0) {
            /* the pointer wasn't allocated, nothing to update */
            SOC_EXIT; 
        }

        /* get number of elements allocated (before ISSU) */
        _SOC_IF_ERR_EXIT(shr_sw_state_allocated_size_get(unit, *retrieved_address_p, &allocated_size)); 
        if (is_single_pointer) {
            nof_allocated_elements = allocated_size / retrieved_layout_node->size; 
        } 
        else {
            /* for double pointer, we have allocated an array of pointer */
            nof_allocated_elements = allocated_size / sizeof(uint8*); 
        }

        /* allocate */

        /* the current value is a single pointer, allocate an array of type */
        if (is_single_pointer) {
            /* size to allocate: nof elements allocated before ISSU * size of an element (after ISSU) */
            _SOC_IF_ERR_EXIT(shr_sw_state_alloc(unit, updated_address_p, nof_allocated_elements * updated_layout_node->size, SW_STATE_NONE)); 
        } 
        /* the current value to allocate is a multiple pointer, allocate an array of pointer of type */
        else {
            /* size to allocate: nof elements allocated before ISSU * size of pointer of an element */
            _SOC_IF_ERR_EXIT(shr_sw_state_alloc(unit, updated_address_p, nof_allocated_elements * sizeof(uint8*), SW_STATE_NONE)); 
        }

        /* update each allocated element */
        for (allocated_element_index = 0; allocated_element_index <nof_allocated_elements; allocated_element_index++) {

           /* address of allocated element */
           uint8* retrieved_allocated_element_address; 
           uint8* updated_allocated_element_address; 

           /* single pointer, */
           if (is_single_pointer) {
               retrieved_allocated_element_address = *retrieved_address_p + allocated_element_index * retrieved_layout_node->size; 
               updated_allocated_element_address = *updated_address_p + allocated_element_index * updated_layout_node->size; 
           }
           /* multiple pointer */
           else {
               retrieved_allocated_element_address = *retrieved_address_p + allocated_element_index * sizeof(uint8*); 
               updated_allocated_element_address = *updated_address_p + allocated_element_index * sizeof(uint8*); 
           }

           /* compare and update the pointed value */
           _SOC_IF_ERR_EXIT(shr_sw_state_issu_update_internal(unit, 
                                                              retrieved_sw_state_data_block_header, 
                                                              updated_sw_state_data_block_header, 
                                                              retrieved_layout_node, 
                                                              updated_layout_node, 
                                                              retrieved_allocated_element_address,  
                                                              updated_allocated_element_address, 
                                                              current_level_pointer + 1, /* we now compare the pointed value */
                                                              is_array_already_processed,
                                                              TRUE)); /* diff already printed */ 
       }
       SOC_EXIT; 

    }
    /* current nodes are no longer pointers (pointers have already been allocated, and the adddress is at their value) */
    else if ((retrieved_layout_node->nof_pointer > 0) && (current_level_pointer == retrieved_layout_node->nof_pointer)) {
        /* compare their value, continue */
    }

    /* pre-condition: both nodes aren't pointers (we already have treated this case) */

    /* leaf: current updated node doesn't have children */
    if (updated_layout_node->first_child_node_index == 0)  {
        /* node before ISSU doesn't have children neither */
        if (retrieved_layout_node->first_child_node_index == 0) {
            /* copy content from variable before ISSU to variable after ISSU */
            /* using the minimal size between both data */

            sw_state_copy_endian_independent(
                (void *)(updated_address),
                (void *)(retrieved_address),
                (size_t)updated_layout_node->size,
                (size_t)retrieved_layout_node->size,
                (size_t)SOC_SAND_MIN(updated_layout_node->size, retrieved_layout_node->size)
            ); 
            SOC_EXIT; 
            
        }
       /* the node before ISSU had children, ignore them */
       else if (retrieved_layout_node->first_child_node_index > 0) {
           /* nothing to update */
           SOC_EXIT; 
       }
    }
    /* compare nodes children between both versions */
    else {
        /* retrieved node doesn't have children */
        if (retrieved_layout_node->first_child_node_index ==0) {
            /* nothing to udpate */
        }
        /* compare children between both version */
        else {

            int updated_child_node_index = updated_layout_node->first_child_node_index; 

            while (updated_child_node_index != 0) {
                /* children layout node after ISSU */
                shr_sw_state_ds_layout_node_t updated_child_node = updated_sw_state_data_block_header->ds_layout_nodes[updated_child_node_index]; 
                /* search in layout nodes before ISSU if layout node after ISSU exist */
                int retrieved_child_node_index = retrieved_layout_node->first_child_node_index;  

                while (retrieved_child_node_index != 0) {
                    /* children node */
                    shr_sw_state_ds_layout_node_t retrieved_child_node = retrieved_sw_state_data_block_header->ds_layout_nodes[retrieved_child_node_index]; 
                    /* both nodes have the same name */
                    if (sal_strcmp(retrieved_child_node.name, updated_child_node.name) == 0) {
                        /* address of the child */
                        uint8* retrieved_child_address = retrieved_address + retrieved_child_node.offset; 
                        uint8* updated_child_address = updated_address + updated_child_node.offset; 

                        /* make sure that both children can be compared:
                           if nof pointers is different, then don't update */
                        if (updated_child_node.nof_pointer != retrieved_child_node.nof_pointer) {
                            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                                                (BSL_META_U(unit, "nof pointers are not compatible for %s: before ISSU: %d, during ISSU: %d \n"), 
                                                 retrieved_child_node.name, retrieved_child_node.nof_pointer, updated_child_node.nof_pointer));
                        }

                        /* if nof dimensions for array are differnet, then don't update */
                        if (((updated_child_node.array_sizes[0] > 0) != (retrieved_child_node.array_sizes[0] > 0))
                            || ((updated_child_node.array_sizes[1] > 0) != (retrieved_child_node.array_sizes[1] > 0))) {
                            int nof_retrieved_dimensions = (retrieved_child_node.array_sizes[0] > 0) + (retrieved_child_node.array_sizes[1] > 0); 
                            int nod_updated_dimensions   = (updated_child_node.array_sizes[0] > 0)   + (updated_child_node.array_sizes[1] > 0); 


                            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                                                (BSL_META_U(unit, "nof dimensions are not compatible for %s: before ISSU: %d, during ISSU: %d \n"), 
                                                retrieved_child_node.name, nof_retrieved_dimensions, nod_updated_dimensions));
                        }

                        /* compare and update child */
                        _SOC_IF_ERR_EXIT(shr_sw_state_issu_update_internal(unit, 
                                                       retrieved_sw_state_data_block_header, 
                                                       updated_sw_state_data_block_header, 
                                                       &retrieved_child_node, 
                                                       &updated_child_node, 
                                                       retrieved_child_address,
                                                        updated_child_address, 
                                                       0, 
                                                       FALSE,
                                                       FALSE)); 
                        break; 
                    } 
                    /* nodes don't have the same name, check next node */
                    else {
                        retrieved_child_node_index = retrieved_child_node.next_brother_node_index; 
                    }
                }
                /* didn't find the layout node after ISSU in the nodes before ISSU, it's a new node */
                if (retrieved_child_node_index == 0) {
                    /* nothing to update */
                }
                /* get next updated layout node */
                updated_child_node_index = updated_child_node.next_brother_node_index; 
            } 

        }
        SOC_EXIT; 
    }
exit:
    SOC_FUNC_RETURN;
}



/*********************************************************************
 * NAME:
 *   shr_sw_state_issu_update
 * FUNCTION:
 *   update the sw state data block after ISSU, using retreived sw state block 
 * INPUT: 
 *   unit
 *   retrieved_sw_state_data_block_header - sw state block saved in FILE before ISSU
 *   updated_sw_state_data_block_header - sw state block to use after ISSU, updated using retrieved_sw_state_data_block_header
 * RETURNS:
 *   ERROR value.
*********************************************************************/

int shr_sw_state_issu_update(int                              unit, 
                             shr_sw_state_data_block_header_t *retrieved_sw_state_data_block_header, 
                             shr_sw_state_data_block_header_t *updated_sw_state_data_block_header) {
#ifdef SW_STATE_ISSU_DIAG
    clock_t clock_start, clock_end;
    float seconds = 0;
#endif
    SOC_INIT_FUNC_DEFS;
#ifdef SW_STATE_ISSU_DIAG
    clock_start = clock();
    shr_sw_state_print_node(unit, 0, retrieved_sw_state_data_block_header->ds_layout_nodes, "before_issu");
    shr_sw_state_print_node(unit, 0, updated_sw_state_data_block_header->ds_layout_nodes  , "after_issu");
    clock_end = clock();
    seconds = (float)(clock_end - clock_start) / CLOCKS_PER_SEC;
    cli_out("SW_STATE: print sw_state layout took: %g seconds\n"
    "    run \"meld before_issu after_issu\" or \"kompare before_issu after_issu\" to see the differences\n", seconds);
#endif
    
    _SOC_IF_ERR_EXIT(shr_sw_state_issu_update_internal(unit, 
                                      retrieved_sw_state_data_block_header, 
                                      updated_sw_state_data_block_header, 
                                      &(retrieved_sw_state_data_block_header->ds_layout_nodes[0]),  /* retrieved node id: by convention, dpp is at index 0 in layout nodes array */
                                      &(updated_sw_state_data_block_header->ds_layout_nodes[0]),  /* updated node id: by convention, dpp is at index 0 in layout nodes array */
                                      retrieved_sw_state_data_block_header->data_ptr, /* by convention, dpp is the 1st element in data */
                                      updated_sw_state_data_block_header->data_ptr, /* by convention, dpp is the 1st element in data */
                                      0,   /* pointer level */
                                      FALSE,   /* array have already been treated */
                                      FALSE    /* print ISSU node's diff */
                                     )); 
exit:
    SOC_FUNC_RETURN;
}


/*
 * Perform regular init for the sw state.
 * reallocate_scache: reallocate scache instead of create it.
*/
int shr_sw_state_data_block_header_init(int unit, uint32 size, uint8 reallocate_scache) {

    int                                rc = SOC_E_NONE;
    soc_scache_handle_t                wb_handle_orig = 0;
    int                                scache_flags=0;
    int                                already_exists;
    uint8                             *scache_ptr;
#ifndef BCM_WARM_BOOT_SUPPORT
    uint8                             *ptr_to_free;
#endif
    uint32                             total_buffer_size = 0;

    SOC_INIT_FUNC_DEFS;

    SHR_SW_STATE_SCACHE_HANDLE_SET(wb_handle_orig, unit, SHR_SW_STATE_SCACHE_HANDLE_DATA_BLOCK);

    total_buffer_size = size; 

    /* allocate (or reallocate) new scache buffer */
    rc = shr_sw_state_scache_ptr_get(unit, 
                                     wb_handle_orig, 
                                     (reallocate_scache? socSwStateScacheRealloc : socSwStateScacheCreate), 
                                     scache_flags,
                                     &total_buffer_size, &scache_ptr, &already_exists);
    _SOC_IF_ERR_EXIT(rc);

#ifndef BCM_WARM_BOOT_SUPPORT
    ptr_to_free = scache_ptr;
#endif

    /* make scache_ptr 4 bytes aligned */
    scache_ptr = (uint8*)(((unsigned long)scache_ptr + 0x3) & (~0x3));

    shr_sw_state_data_block_header[unit] = (shr_sw_state_data_block_header_t *) scache_ptr;
    shr_sw_state_data_block_header[unit]->total_buffer_size = total_buffer_size;

#ifndef BCM_WARM_BOOT_SUPPORT
    shr_sw_state_data_block_header[unit]->ptr_to_free = ptr_to_free;
#endif

    shr_sw_state_data_block_header[unit]->ptr_offsets_stack_base = 
        (shr_sw_state_alloc_element_t *) ((uint8*)shr_sw_state_data_block_header[unit] + size - sizeof(shr_sw_state_alloc_element_t));
    shr_sw_state_data_block_header[unit]->ptr_offsets_sp = shr_sw_state_data_block_header[unit]->ptr_offsets_stack_base;

    /* pointer to the location where layout nodes array begins */
    shr_sw_state_data_block_header[unit]->ds_layout_nodes = 
        (shr_sw_state_ds_layout_node_t *) (shr_sw_state_data_block_header[unit] + 1);

    /* init sw state layout: description of all struct. (Help to perform ISSU):
     * update layout nodes array
     * update nof layout nodes */
    _SOC_IF_ERR_EXIT(shr_sw_state_layout_create(unit, &(shr_sw_state_data_block_header[unit]->nof_ds_layout_nodes))); 

    /* update max nof layout nodes */
    shr_sw_state_data_block_header[unit]->nof_max_ds_layout_nodes = SHR_SW_STATE_DEFS_LAYOUT_NOF_LAYOUT_NODES; 

    /* pointer to the location where actual data block begins */
    shr_sw_state_data_block_header[unit]->data_ptr = (uint8 *) ((shr_sw_state_data_block_header[unit]->ds_layout_nodes) + 
                                                                 SHR_SW_STATE_DEFS_LAYOUT_NOF_LAYOUT_NODES);

    shr_sw_state_data_block_header[unit]->data_size = 
        size - sizeof(shr_sw_state_data_block_header_t) - sizeof(shr_sw_state_ds_layout_node_t) * SHR_SW_STATE_DEFS_LAYOUT_NOF_LAYOUT_NODES;

    /* init the next free data slot to the be the first slot in the data block */
    shr_sw_state_data_block_header[unit]->next_free_data_slot = 
        shr_sw_state_data_block_header[unit]->data_ptr + sizeof(shr_sw_state_t *);

    /* currently data block is empty so size_left==data_size */
    shr_sw_state_data_block_header[unit]->size_left = shr_sw_state_data_block_header[unit]->data_size;

    /*init the data to be all zeros*/
    sal_memset(shr_sw_state_data_block_header[unit]->data_ptr, 0x0,
                shr_sw_state_data_block_header[unit]->data_size);

exit:
    SOC_FUNC_RETURN;
}
 

/* restore pointer for data block header */
int shr_sw_state_restore_pointers(int unit, shr_sw_state_data_block_header_t *data_block_header){
        uint8                            **tmp_ptr;
        uint8                             *restored_data_ptr;
        shr_sw_state_alloc_element_t      *p;
        int                                diff; 
    SOC_INIT_FUNC_DEFS;

    /* save based ptr of the saved data block for calculations below */
    restored_data_ptr = data_block_header->data_ptr;

    /* update layout nodes pointer */
    data_block_header->ds_layout_nodes = 
        (shr_sw_state_ds_layout_node_t *) (data_block_header + 1);

    /* update data ptr */
    data_block_header->data_ptr = 
        (uint8 *) ((data_block_header->ds_layout_nodes) + 
                   data_block_header->nof_max_ds_layout_nodes);

    diff = data_block_header->data_ptr - restored_data_ptr;

    data_block_header->ptr_offsets_stack_base = (shr_sw_state_alloc_element_t*)
        ((uint8*)data_block_header->ptr_offsets_stack_base + diff);
             
    data_block_header->ptr_offsets_sp = (shr_sw_state_alloc_element_t*)
        ((uint8*)data_block_header->ptr_offsets_sp + diff);

    data_block_header->next_free_data_slot =
        ((uint8*)data_block_header->next_free_data_slot + diff);

    /* update the stack's elements and the sw_state pointers*/
    p = data_block_header->ptr_offsets_sp;
    ++p;

    while (p <= data_block_header->ptr_offsets_stack_base) {
        tmp_ptr = (uint8 **) (data_block_header->data_ptr + p->ptr_offset);
        *tmp_ptr += diff;
        p->ptr_value = *tmp_ptr;
        p++;
    }

    SOC_EXIT;
exit:
    SOC_FUNC_RETURN;
}

/*********************************************************************
 * NAME:
 *   shr_sw_state_init
 * FUNCTION:
 *   init/restore the sw state data block. 
 * INPUT: 
 *   unit
 *   flags - used to determine the operation mode
 *   init_mode - init or restore(wb)
 *   size - size of the data block in bytes, net size, actual size will be bigger,
 *          this size is a limit on the amount of memory available for sw_state_allocations.
 *          -1 is expected if init_mode is "restore".
 * RETURNS:
 *   ERROR value.
*********************************************************************/
int shr_sw_state_init(int unit, int flags, shr_sw_state_init_mode_t init_mode, uint32 size) {
    int                                rc = SOC_E_NONE;
    soc_scache_handle_t                wb_handle_orig = 0;
    int                                scache_flags=0;
    int                                already_exists;
    uint32                             restored_size = 0;
    uint8                              *scache_ptr;
    shr_sw_state_data_block_header_t  *retrieved_data_block_header;  
    uint8                              is_issu=0; 
    uint32                             nof_layout_nodes; 

    SOC_INIT_FUNC_DEFS;

    INIT_SW_STATE_MEM ;
    DISPLAY_SW_STATE_MEM ;
    DISPLAY_SW_STATE_MEM_PRINTF(("%s(): unit %d: Entry\r\n",__FUNCTION__,unit)) ;

    SHR_SW_STATE_SCACHE_HANDLE_SET(wb_handle_orig, unit, SHR_SW_STATE_SCACHE_HANDLE_DATA_BLOCK);
#ifdef BCM_WARM_BOOT_SUPPORT
    if (size > SHR_SW_STATE_SCACHE_RESERVE_SIZE) {
        /* leave some room to legacy warmboot storage */
        size -= SHR_SW_STATE_SCACHE_RESERVE_SIZE;
    }
#endif

    /* init the sw_state access callback data structure */
    sw_state_access_cb_init(unit);

    switch (init_mode) {
        case socSwStateDataBlockRegularInit:
 
            _SOC_IF_ERR_EXIT(shr_sw_state_data_block_header_init(unit, size, FALSE)); 

            shr_sw_state_auto_sync(unit, 
                       ((void*) (shr_sw_state_data_block_header[unit])),
                       sizeof(*shr_sw_state_data_block_header[unit])); 

            /* allocate the root of the sw state */
            _SOC_IF_ERR_EXIT(shr_sw_state_alloc(unit, (uint8 **) shr_sw_state_data_block_header[unit]->data_ptr, sizeof(shr_sw_state_t), SW_STATE_NONE));

            break;

        case socSwStateDataBlockRestoreAndOveride:
            /* retrieve buffer */

            rc = shr_sw_state_scache_ptr_get(unit, wb_handle_orig, socSwStateScacheRetreive, scache_flags,
                                    &restored_size, &scache_ptr, &already_exists);
            _SOC_IF_ERR_EXIT(rc);
            retrieved_data_block_header = (shr_sw_state_data_block_header_t *) scache_ptr;

            _SOC_IF_ERR_EXIT(shr_sw_state_restore_pointers(unit, retrieved_data_block_header)); 

            /* now we have restored the sw state, check if we are in ISSU */

            /* find out if ISSU: compare layout nodes. */
            /* init sw state layout (description of all struct) for the current version. */
            /* allocate memory for header only.  */

            shr_sw_state_data_block_header[unit] = 
                sal_alloc(sizeof (shr_sw_state_data_block_header_t) + 
                          SHR_SW_STATE_DEFS_LAYOUT_NOF_LAYOUT_NODES * sizeof(shr_sw_state_ds_layout_node_t)
                          , "sw state header"); 
            /* update layout nodes ptr
               Note: no offset list needed here. We only need sw state layout */
            shr_sw_state_data_block_header[unit]->ds_layout_nodes = 
                (shr_sw_state_ds_layout_node_t *) (shr_sw_state_data_block_header[unit] + 1);

            /* init sw state layout */
            _SOC_IF_ERR_EXIT(shr_sw_state_layout_create(unit, &nof_layout_nodes)); 

            is_issu = (retrieved_data_block_header->nof_ds_layout_nodes != nof_layout_nodes); 

            if (!is_issu) {
                /* compare previous layout nodes with current layout nodes
                   Precondition: layout nodes have same size (garanteed by previous check) */

                is_issu = sal_memcmp(retrieved_data_block_header->ds_layout_nodes, shr_sw_state_data_block_header[unit]->ds_layout_nodes, nof_layout_nodes * sizeof(shr_sw_state_ds_layout_node_t)); 
            }

            /* free memory allocated for the current data block header.
             * (which allowed to check if issu).
             * In case no no issu, current data block header equal the restored one
               In case issu, need to allocate memory using scache. */
            sal_free(shr_sw_state_data_block_header[unit]); 

            if (!is_issu) {
                shr_sw_state_data_block_header[unit] = retrieved_data_block_header; 
            }
            /* issu: update current sw state with restored sw state
             * 1. move retrieved sw state from scache to allocated memory
             * 2. update pointers for retrieved sw state.
             * 3. init a new sw state in scache
             * 4. update new sw state with retrieved sw state
             * 5. free retrieved sw state */ 
            else {
                /* 1. move retrieved sw state from scache to allocated memory */
                void* retrieved_sw_state_temp; 
                /* alloc mem for retrieved sw state */
                retrieved_sw_state_temp = sal_alloc(restored_size, "retrieved sw state to temporary memory"); 

                /* copy retrieved sw state to temp memory */
                sal_memcpy(retrieved_sw_state_temp, retrieved_data_block_header, restored_size); 

                /* retrieved data block header point now to temporary memory */
                retrieved_data_block_header = retrieved_sw_state_temp; 

                /* 2. update pointers for retrieved sw state. */
                _SOC_IF_ERR_EXIT(shr_sw_state_restore_pointers(unit, retrieved_data_block_header)); 

                /* 3. init a new sw state in scache */
                /* Need to reallocate scache */
                _SOC_IF_ERR_EXIT(shr_sw_state_data_block_header_init(unit, size, TRUE)); 

                /* 4. update new sw state with retrieved sw state */
                /* compare retrieved layout with new layout, update new data with retrieved data */
                _SOC_IF_ERR_EXIT(shr_sw_state_issu_update(unit, retrieved_data_block_header, shr_sw_state_data_block_header[unit])); 

                /* 5. free retrieved sw state */
                sal_free(retrieved_data_block_header); 
            }


            break;

        default:
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                       (BSL_META_U(unit,
                          "Unit:%d tried to initialize data block with unsupported mode\n"), unit));
    }

    sw_state[unit] = (shr_sw_state_t *) (*((uint8 **) shr_sw_state_data_block_header[unit]->data_ptr));

    sw_state_sync_db[unit].dpp.htb_locks = sal_alloc(sizeof(sal_mutex_t) * SW_STATE_MAX_NOF_HASH_TABLES, "hash table mutexes");
    _SOC_IF_ERR_EXIT(sw_state_htb_init(unit, SW_STATE_MAX_NOF_HASH_TABLES));

    _SOC_IF_ERR_EXIT(rc);

exit:


    DISPLAY_SW_STATE_MEM ;
    DISPLAY_SW_STATE_MEM_PRINTF(("%s(): unit %d: Exit\r\n",__FUNCTION__,unit)) ;

    SOC_FUNC_RETURN;
}


/*********************************************************************
 * NAME:
 *   shr_sw_state_block_dump
 * FUNCTION:
 *   dump the data block in hexa (omiting ptrs). 
 * INPUT: 
 *   unit
*********************************************************************/
void shr_sw_state_block_dump(int unit, char *file_name, char *mode)
{
#ifndef __KERNEL__
    FILE *output_file;
    uint8 *buff_to_print;
    uint8 **ptr_location;
    unsigned int i;
    unsigned int len;
    shr_sw_state_alloc_element_t       *p;
    int HEXDUMP_COLS = 32;
    uint8 is_dry_run = 1;
    uint8 is_all_zeroes = 1;



    if ((output_file = sal_fopen(file_name, mode)) == 0) {
        cli_out("Error opening sw dump file %s\n", file_name);
        return;
    }
    if (SOC_FAILURE(bcm_dpp_counter_state_diag_mask(unit))){
        cli_out("Error masking out counters state diagnostics\n");
    }
    if (SOC_FAILURE(handle_sand_result(soc_sand_hash_table_clear_all_tmps(unit)))){
        cli_out("Error masking out sand_hash_tables tmp buffers state\n");
    }
    if (SOC_FAILURE(handle_sand_result(soc_sand_sorted_list_clear_all_tmps(unit)))){
        cli_out("Error masking out sand_sorted_lists tmp buffers state\n");
    }
    

    len = shr_sw_state_data_block_header[unit]->data_size - shr_sw_state_data_block_header[unit]->size_left;


    buff_to_print = sal_alloc(sizeof(uint8) * len, "sw state block dump");

    sal_memcpy(buff_to_print, shr_sw_state_data_block_header[unit]->data_ptr, len);

    /* cover pointer for cleaner diff (put NULL) */

    p = shr_sw_state_data_block_header[unit]->ptr_offsets_sp;
    ++p;

    while (p <= shr_sw_state_data_block_header[unit]->ptr_offsets_stack_base) {
        ptr_location = (uint8 **) (buff_to_print + p->ptr_offset);
        *ptr_location = NULL;
        p++;
    }

    
#if defined(BCM_88675_A0) && defined(INCLUDE_KBP) && !defined(BCM_88030)
    if (sw_state[unit]->dpp.soc.arad.pp && sw_state[unit]->dpp.soc.arad.pp->kaps_db) {
        sal_memset((buff_to_print + 
                    ((uint8 *) sw_state[unit]->dpp.soc.arad.pp->kaps_db - (uint8 *) shr_sw_state_data_block_header[unit]->data_ptr)),
                    0x0, sizeof(JER_KAPS));
    }
#endif /*defined(BCM_88675_A0) && defined(INCLUDE_KBP) && !defined(BCM_88030)*/

    sal_fprintf(output_file, " ************************ \n");
    sal_fprintf(output_file, " *****sw state block***** \n");
    sal_fprintf(output_file, " ************************ \n");

    for(i = 0; i < len + ((len % HEXDUMP_COLS) ? (HEXDUMP_COLS - len % HEXDUMP_COLS) : 0); i++)
    {
        /* print offset */
        if(i % HEXDUMP_COLS == 0)
        {
            if (!is_dry_run) {
                sal_fprintf(output_file, "\n%08x:", i);
            }
        }

        /* print hex data */
        if(i < len)
        {
            if (!is_dry_run) {
                sal_fprintf(output_file, "%02x", 0xFF & ((char*)buff_to_print)[i]);
            }
            else{
                if (0xFF & ((char*)buff_to_print)[i]) {
                    /* found a non zero value */
                    is_all_zeroes = 0 ;
                }
            }
        }
        else /* end of block, just aligning for ASCII dump */
        {
            if (!is_dry_run) {
                sal_fprintf(output_file, "\n");
            }
        }

        /* restart this line in a "wet" run if necessary */
        if (is_dry_run && !is_all_zeroes) {
            is_dry_run = 0;
            i = ((i/HEXDUMP_COLS) * HEXDUMP_COLS) - 1;
			continue;
        }

        /* turn of wet run at the end of line */
        if (!is_dry_run && (((i+1) % HEXDUMP_COLS) == 0)) {
            is_dry_run = 1;
            is_all_zeroes = 1;
        }
    }
    if (SOC_FAILURE(bcm_dpp_counter_state_diag_unmask(unit))) {
        cli_out("Error masking out counters state diagnostics\n");
    }

    sal_fclose(output_file);
    sal_free(buff_to_print);

    return;
#else
    cli_out("This function is not supported in kernek mode\n");
#endif /* ! __KERNEL__ */
}

int
shr_sw_state_sizes_get(int unit, uint32* in_use, uint32* left) {
    *left = shr_sw_state_data_block_header[unit]->size_left;
    *in_use = shr_sw_state_data_block_header[unit]->data_size - *left;
    return _SHR_E_NONE;
}


int alloc_element_compare(void *ae1, void *ae2) {
    shr_sw_state_alloc_element_t *aae1 = NULL;
    shr_sw_state_alloc_element_t *aae2 = NULL;
    aae1 = (shr_sw_state_alloc_element_t*)ae1;
    aae2 = (shr_sw_state_alloc_element_t*)ae2;
    /* in reverse order */
    return (aae2->ptr_value - aae1->ptr_value);
}


static void *
shr_sw_state_memmove(void *dst_void, const void *src_void, uint32 len)
{
    unsigned char *dst = dst_void;
    const unsigned char *src = src_void;
    if (dst < src) {
        while (len--) {
            *dst++ = *src++;
        }
    } else if (src < dst) {
        while (len--) {
            *(dst+len) = *(src+len);
        }
    }
    return dst_void;
}

/*********************************************************************
 * NAME:
 *   shr_sw_state_defrag
 * FUNCTION:
 *   defragmentation of the data block to be continuous
 *   algorithm:
 *   1) sort the stack by ptr_value
 *   2) iterate through the stack and move the blocks to the first
 *       unused place in the data block
 *       2.1) update all neccesary data of the header and the stack's elements
 * INPUT: 
 *   unit, old_ptr - (reference to) pointer to sw_state member
 * RETURNS:
 *   ERROR value, old_ptr- the (reference to) old_ptr after the shifting of the defragmentation
*********************************************************************/
int shr_sw_state_defrag(int unit, uint8*** old_ptr) {
    uint8* curr_ptr = NULL;
    uint32* temp_ptr = NULL;
    uint8** orig_ptr = NULL;
    uint32 num_of_alloc_mem = 0;
    uint32 alloc_size = 0;
    shr_sw_state_alloc_element_t *p = NULL;
    shr_sw_state_alloc_element_t *p2 = NULL;
    uint32 old_ptr_shift = 0;

    SOC_INIT_FUNC_DEFS;
    /* in this implementation every block of data moves once. if u change this change also in (*1) */
    /* a pointer to the next location to be filled */
    curr_ptr = (uint8*)((uint32*)sw_state[unit] - 2); /* the -2 is for reaching the location of size, before the ccc*/
    num_of_alloc_mem = shr_sw_state_data_block_header[unit]->ptr_offsets_stack_base
        - shr_sw_state_data_block_header[unit]->ptr_offsets_sp;
    /* 
      sort the stack by ptr_value
      with minimun value in the stack's base 
    */
    soc_sand_os_qsort(shr_sw_state_data_block_header[unit]->ptr_offsets_sp + 1,
                        num_of_alloc_mem, sizeof(shr_sw_state_alloc_element_t),alloc_element_compare);

    p = shr_sw_state_data_block_header[unit]->ptr_offsets_stack_base;

    /* iterate the stack from the base to the sp */
    while (p > shr_sw_state_data_block_header[unit]->ptr_offsets_sp) {
        /*auto sync p (because of the sorting)*/
        shr_sw_state_auto_sync(unit, ((void*) p), sizeof(shr_sw_state_alloc_element_t)); 
        temp_ptr = ((uint32 *)(p->ptr_value));
        _SOC_IF_ERR_EXIT(shr_sw_state_allocated_size_get(unit, (uint8*)temp_ptr, &alloc_size));
        alloc_size = alloc_size + 3*sizeof(uint32); /* 3 is for the |size|c..c|d..d| */
        --temp_ptr; --temp_ptr;
        if (((uint8 *)temp_ptr != curr_ptr)) {
            shr_sw_state_memmove(curr_ptr, (uint8*)temp_ptr, alloc_size);
            if (((uint8*)(*old_ptr) >= (uint8*)temp_ptr) && ((uint8*)(*old_ptr) < (uint8*)temp_ptr + alloc_size)) {
              /*(*1)*/
              old_ptr_shift = (uint8*)temp_ptr - curr_ptr;
            }
            sal_memset(curr_ptr + alloc_size, 0, (uint8*)temp_ptr - curr_ptr);
            /*auto sync the new allocated memory*/
            shr_sw_state_auto_sync(unit,((void*) curr_ptr), alloc_size);
            /* need to sync in case of overlap */
            shr_sw_state_auto_sync(unit,((void*) temp_ptr), alloc_size);
            
            orig_ptr = (uint8**)((uint8*)(shr_sw_state_data_block_header[unit]->data_ptr) +
                p->ptr_offset);
            *orig_ptr = (uint8*)((uint32*)curr_ptr + 2);
            /*auto sync orig_pointer (the pointer to the allocation block) */ 
            shr_sw_state_auto_sync(unit, ((void*) orig_ptr), sizeof(uint8*));            
            p->ptr_value = (uint8*)((uint32*)curr_ptr + 2);
            /*auto sync p - update the ptr_value */
            shr_sw_state_auto_sync(unit, ((void*) p), sizeof(shr_sw_state_alloc_element_t)); 

            /* update the stack with the inner pointers in the moved block */
            p2 = shr_sw_state_data_block_header[unit]->ptr_offsets_stack_base;
            while (p2 > shr_sw_state_data_block_header[unit]->ptr_offsets_sp) {
                if (((shr_sw_state_data_block_header[unit]->data_ptr + p2->ptr_offset) >= 
                        ((uint8*)((uint32*)temp_ptr+2)))
                    && ((p2->ptr_offset + shr_sw_state_data_block_header[unit]->data_ptr) <
                        ((uint8*)(((uint32*)temp_ptr+2)) + alloc_size))) {
                    p2->ptr_offset -= ((uint8*)temp_ptr - curr_ptr);
                    shr_sw_state_auto_sync(unit, ((void*) p2), sizeof(shr_sw_state_alloc_element_t));
                }
                p2--;
            }

        }
        curr_ptr += alloc_size;
        p--;
        
    }
    /* update the header with the new properties */
    shr_sw_state_data_block_header[unit]->next_free_data_slot = curr_ptr;
    /* auto sync the header's next_free_data_slot */
    shr_sw_state_auto_sync(unit, ((void*) &shr_sw_state_data_block_header[unit]->next_free_data_slot), sizeof(uint8*));
    shr_sw_state_data_block_header[unit]->size_left =
        (uint8*)(shr_sw_state_data_block_header[unit]->ptr_offsets_sp) - curr_ptr; 
    /* auto sync the header's size_left */
    shr_sw_state_auto_sync(unit, ((void*) &shr_sw_state_data_block_header[unit]->size_left), sizeof(uint32));
    SOC_EXIT;
exit:
    *old_ptr = (uint8**)((uint8*)*old_ptr - old_ptr_shift);
    SOC_FUNC_RETURN;
}


/* get the num of bits allocated */
int shr_sw_state_shr_bitdcl_nof_bits_get(int unit, uint8 *ptr, uint32 *num_of_bits_allocated) {
  /* the difference between the number of allocated bits to
		 the num of requested bits to alllocate */
    uint8  extra_bits 		 = 0;
    uint32 allocated_size  = 0;
    uint8  *extra_bits_ptr = NULL;
	    
    SOC_INIT_FUNC_DEFS;
  
    _SOC_IF_ERR_EXIT(shr_sw_state_allocated_size_get(unit, ptr, &allocated_size));
    /* we save the num of extra bits in the last byte of the data */
    extra_bits_ptr = ptr + allocated_size - 1;
    extra_bits = *extra_bits_ptr;
    *num_of_bits_allocated = allocated_size * NOF_BITS_IN_BYTE - extra_bits;
    SOC_EXIT;
exit:
    SOC_FUNC_RETURN;
}

/* update the num of bits over-allocated */
int shr_sw_state_shr_bitdcl_nof_bits_update(int unit, uint8 *ptr, uint32 num_of_bits) {
    uint32 allocated_size  = 0;
    uint8  *extra_bits_ptr = NULL;
	    
    SOC_INIT_FUNC_DEFS;
  
    _SOC_IF_ERR_EXIT(shr_sw_state_allocated_size_get(unit, ptr, &allocated_size));
    /* we save the num of extra bits in the last byte of the data */
    extra_bits_ptr = ptr + allocated_size - 1;
    *extra_bits_ptr = (uint8)(allocated_size * NOF_BITS_IN_BYTE - num_of_bits);
    SOC_EXIT;
exit:
    SOC_FUNC_RETURN;
}


/* Result is 1 (in result) if
   bits1 op bits2 = bits1 */
int shr_sw_state_shr_bitdcl_independent(CONST SHR_BITDCL *bits1,
                   CONST SHR_BITDCL *bits2,
                   int bits1_first,
                   int bits2_first,
                   int range,
                   shrbitdcl_op_t op,
                   int *result) {
    int cmp = 0;
    SHR_BITDCL *bits1_aux = NULL;
    SHR_BITDCL *bits2_aux = NULL;
    SHR_BITDCL *bits_result_aux = NULL;
    SOC_INIT_FUNC_DEFS;

    bits1_aux = sal_alloc(SHR_BITALLOCSIZE(range)
        , "sw state shr_bitdcl_1 auxiliary alloc for bitcl_cmp");
    if (!bits1_aux) {
        _SOC_IF_ERR_EXIT(_SHR_E_MEMORY);
    }
    bits2_aux = sal_alloc(SHR_BITALLOCSIZE(range)
        , "sw state shr_bitdcl_2 auxiliary alloc for bitcl_cmp");
    if (!bits2_aux) {
        _SOC_IF_ERR_EXIT(_SHR_E_MEMORY);
    }

    bits_result_aux = sal_alloc(SHR_BITALLOCSIZE(range)
        , "sw state shr_bitdcl_result auxiliary alloc for bitcl_cmp");
    if (!bits_result_aux) {
        _SOC_IF_ERR_EXIT(_SHR_E_MEMORY);
    }
    sal_memset(bits1_aux, 0, SHR_BITALLOCSIZE(range));
    sal_memset(bits2_aux, 0, SHR_BITALLOCSIZE(range));
    sal_memset(bits_result_aux, 0, SHR_BITALLOCSIZE(range));

    SHR_BITCOPY_RANGE(bits1_aux, 0, bits1, bits1_first, range);
    SHR_BITCOPY_RANGE(bits2_aux, 0, bits2, bits2_first, range);

    switch (op) {
    case SHR_BITDCL_ASSIGNMENT :
        break;
    case SHR_BITDCL_AND :
        SHR_BITAND_RANGE(bits1, bits2, 0, range, bits_result_aux);
        break;
    case SHR_BITDCL_OR :
        SHR_BITOR_RANGE(bits1, bits2, 0, range, bits_result_aux);
        break;
    case SHR_BITDCL_XOR :
        SHR_BITXOR_RANGE(bits1, bits2, 0, range, bits_result_aux);
        break;
    case SHR_BITDCL_REMOVE :
        SHR_BITREMOVE_RANGE(bits1, bits2, 0, range, bits_result_aux);
        break;
    case SHR_BITDCL_CLEAR :
        SHR_BITCOPY_RANGE(bits_result_aux, 0, bits1_aux, 0, range);
        SHR_BITCLR_RANGE(bits_result_aux, 0, range);
        break;
    case SHR_BITDCL_SET :
        SHR_BITCOPY_RANGE(bits_result_aux, 0, bits1_aux, 0, range);
        SHR_BITSET_RANGE(bits_result_aux, 0, range);
        break;
    }

    cmp = SHR_BITEQ_RANGE(bits1_aux, bits_result_aux, 0, range);


    sal_free(bits1_aux);
    bits1_aux = NULL;
    sal_free(bits2_aux);
    bits2_aux = NULL;
    sal_free(bits_result_aux);
    bits_result_aux = NULL;

    *result = cmp;
    SOC_EXIT;
exit:
    if (bits1_aux) {
        sal_free(bits1_aux);
    }
    if (bits2_aux) {
        sal_free(bits2_aux);
    }
    SOC_FUNC_RETURN;
}


/* Result is 1 (in result) if
   sw_state_buff op input_buff = sw_state_buff */
int shr_sw_state_buff_independent(CONST SW_STATE_BUFF *sw_state_buff,
                   CONST SW_STATE_BUFF *input_buff,
                   int _value,
                   int _offset,
                   unsigned int _len,
                   sw_state_buff_op_t op,
                   int *result) {
    SOC_INIT_FUNC_DEFS;

    switch (op) {
    case SW_STATE_BUFF_ASSIGNMENT :
        *result = !sal_memcmp(sw_state_buff + _offset, input_buff, _len);
        break;
    case SW_STATE_BUFF_SET :
        *result = 1;
        while (_len-- != 0) {
            if (*(sw_state_buff + _offset + _len) != _value) {
                *result = 0;
                break;
            }
        }
        break;
    }

    SOC_EXIT;
exit:
    SOC_FUNC_RETURN;
}

#ifdef SW_STATE_ISSU_DIAG
int shr_sw_state_print_node(int unit, int node_idx, shr_sw_state_ds_layout_node_t *nodes_arr,
        char* prefix) {
    int i = 0, first_child_idx = 0, sizeof_filename = 0, sizeof_mkdir_cmd = 0, sizeof_dirname = 0;
    char dirname [SW_STATE_PATH_LEN], filename[SW_STATE_PATH_LEN], mkdir_cmd[SW_STATE_PATH_LEN];
    shr_sw_state_ds_layout_node_t *node = &(nodes_arr[node_idx]);
    FILE* curr_file = NULL;
    SOC_INIT_FUNC_DEFS;
    sizeof_dirname   = sizeof(dirname);
    sizeof_filename  = sizeof(filename);
    sizeof_mkdir_cmd = sizeof(mkdir_cmd);
    
    sal_memset(dirname  , 0, sizeof_dirname);
    sal_memset(filename , 0, sizeof_filename);
    sal_memset(mkdir_cmd, 0, sizeof_mkdir_cmd);
    
    sal_strncpy(dirname, prefix    , sizeof_dirname);
    sal_strncat(dirname, "/"       , sizeof_dirname - sal_strlen(dirname) - 1);
    sal_strncat(dirname, node->name, sizeof_dirname - sal_strlen(dirname) - 1);

    sal_strncpy(filename, dirname    , sizeof_filename);
    sal_strncat(filename , "/"       , sizeof_filename - sal_strlen(filename) - 1);
    sal_strncat(filename, node->name , sizeof_filename - sal_strlen(filename) - 1);
    sal_strncat(filename, ".info" , sizeof_filename - sal_strlen(filename) - 1);
    
    sal_strncpy(mkdir_cmd, "mkdir -p ", sizeof_mkdir_cmd);
    sal_strncat(mkdir_cmd, dirname    , sizeof_mkdir_cmd - sal_strlen(mkdir_cmd) - 1);

    if (mkdir_cmd[SW_STATE_PATH_LEN-1] != '\0') {
       _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
          (BSL_META_U(unit,
              "Unit:%d Path length is too long. Change SW_STATE_PATH_LEN definition.\n"), unit));
    }
    system(mkdir_cmd);
    
    first_child_idx = node->first_child_node_index;
    
    if ((curr_file = sal_fopen(filename, "w+")) == 0) {
        cli_out("Error opening file %s\n",filename);
    }
    sal_fprintf(curr_file,"name:          %s\n"
                          "size:          %d\n"
                          "nof pointers:  %d\n"
                          "type:          %s\n"
                          "array_sizes:   [%d][%d]\n"
                          ,node->name
                          ,node->size
                          ,node->nof_pointer
                          ,node->type
                          ,node->array_sizes[0]
                          ,node->array_sizes[1]);
                          
    /* the node has children */
    if (first_child_idx != 0) {
        sal_fprintf(curr_file, "\n\nchildren: ");
        for (i = first_child_idx; i != 0; i = nodes_arr[i].next_brother_node_index) {
            shr_sw_state_print_node(unit, i, nodes_arr, dirname);
            sal_fprintf(curr_file, "\n    %s", nodes_arr[i].name);
        }
        sal_fprintf(curr_file,"\n");
    }
    sal_fclose(curr_file);
    SOC_EXIT;
exit:
    SOC_FUNC_RETURN;
}

#endif /* SW_STATE_ISSU_DIAG */

int shr_sw_state_bit_op(int _unit, sw_state_bitdcl_param_t param)
{
    uint8 bit_before = 0;
    int unmodified = 0;
    SOC_INIT_FUNC_DEFS;
    if (param.op != SW_ST_BIT_GET &&
        param.op != SW_ST_BIT_RANGE_READ &&
        param.op != SW_ST_BIT_RANGE_NULL &&
        param.op != SW_ST_BIT_RANGE_TEST &&
        param.op != SW_ST_BIT_RANGE_EQ &&
        param.op != SW_ST_BIT_RANGE_COUNT) {
       SW_STATE_MODIFY_OPS_CHECKS(param.flags);
    }
#ifdef SW_STATE_AUTOSYNC_OPTIMIZATION
    {
      shrbitdcl_op_t bitdcl_op = (param.op == SW_ST_BIT_RANGE_AND)    ? SHR_BITDCL_AND :
                                 (param.op == SW_ST_BIT_RANGE_OR)     ? SHR_BITDCL_OR  :
                                 (param.op == SW_ST_BIT_RANGE_XOR)    ? SHR_BITDCL_XOR :
                                 (param.op == SW_ST_BIT_RANGE_REMOVE) ? SHR_BITDCL_REMOVE : SHR_BITDCL_ASSIGNMENT;
    
      switch (param.op) {
      case SW_ST_BIT_SET:
      case SW_ST_BIT_CLEAR:
        bit_before = (SHR_BITGET(param.sw_state_bmp, param._bit_num) > 0) ? 1: 0;
        break;
      case SW_ST_BIT_RANGE_WRITE:
        shr_sw_state_shr_bitdcl_independent(param.sw_state_bmp, param.input_bmp, param.sw_state_bmp_first, param.input_bmp_first, param._range, SHR_BITDCL_ASSIGNMENT, &unmodified);
        break;
      case SW_ST_BIT_RANGE_AND:
      case SW_ST_BIT_RANGE_OR:     
      case SW_ST_BIT_RANGE_XOR:
      case SW_ST_BIT_RANGE_REMOVE:
          shr_sw_state_shr_bitdcl_independent(param.sw_state_bmp, param.input_bmp, param.sw_state_bmp_first, param.sw_state_bmp_first, param._range, bitdcl_op, &unmodified);
          break;
      default:
        break;
      }
    }
#endif /* SW_STATE_AUTOSYNC_OPTIMIZATION */
    switch(param.op) {
      case SW_ST_BIT_SET:
        SHR_BITSET(param.sw_state_bmp, param._bit_num);
        break;
      case SW_ST_BIT_CLEAR:
        SHR_BITCLR(param.sw_state_bmp, param._bit_num);
        break;
      case SW_ST_BIT_GET:
        *(param.bit_result) = (SHR_BITGET(param.sw_state_bmp, param._bit_num) > 0) ? 1: 0;
        break;
      case SW_ST_BIT_RANGE_READ:
        SHR_BITCOPY_RANGE(param.result_bmp, param.result_first, param.sw_state_bmp, param.sw_state_bmp_first, param._range);
        break;
      case SW_ST_BIT_RANGE_WRITE:
        SHR_BITCOPY_RANGE(param.sw_state_bmp, param.sw_state_bmp_first, param.input_bmp, param.input_bmp_first, param._range);
        break;
      case SW_ST_BIT_RANGE_AND:
        SHR_BITAND_RANGE(param.input_bmp, param.sw_state_bmp, param.sw_state_bmp_first, param._range, param.sw_state_bmp);
        break;
      case SW_ST_BIT_RANGE_OR:
        SHR_BITOR_RANGE(param.input_bmp, param.sw_state_bmp, param.sw_state_bmp_first, param._range, param.sw_state_bmp);
        break;
      case SW_ST_BIT_RANGE_XOR:
        SHR_BITXOR_RANGE(param.input_bmp, param.sw_state_bmp, param.sw_state_bmp_first, param._range, param.sw_state_bmp);
        break;
      case SW_ST_BIT_RANGE_REMOVE:
        SHR_BITREMOVE_RANGE(param.input_bmp, param.sw_state_bmp, param.sw_state_bmp_first, param._range, param.sw_state_bmp);
        break;
      case SW_ST_BIT_RANGE_NEGATE:
        SHR_BITNEGATE_RANGE(param.sw_state_bmp, param.sw_state_bmp_first, param._range, param.sw_state_bmp);
        break;
      case SW_ST_BIT_RANGE_CLEAR:
        SHR_BITCLR_RANGE(param.sw_state_bmp, param.sw_state_bmp_first, param._range);
        break;
      case SW_ST_BIT_RANGE_SET:
        SHR_BITSET_RANGE(param.sw_state_bmp, param.sw_state_bmp_first, param._range);
        break;
      case SW_ST_BIT_RANGE_NULL:
        *(param.bool_result) = (SHR_BITNULL_RANGE(param.sw_state_bmp, param.sw_state_bmp_first, param._range) > 0) ? 1: 0;
        break;
      case SW_ST_BIT_RANGE_TEST: 
        SHR_BITTEST_RANGE(param.sw_state_bmp, param.sw_state_bmp_first, param._range, *(param.bool_result));
        break;
      case SW_ST_BIT_RANGE_EQ:
        *(param.bool_result) = (SHR_BITEQ_RANGE(param.input_bmp, param.sw_state_bmp, param.sw_state_bmp_first, param._range) > 0) ? 1: 0;
        break;
      case SW_ST_BIT_RANGE_COUNT:
        shr_bitop_range_count(param.sw_state_bmp, param.sw_state_bmp_first, param._range, param.int_result);
        break;
      default:
        break;
    }
/* autosync */
    switch(param.op) {
      case SW_ST_BIT_SET:
      case SW_ST_BIT_CLEAR:
        if (SW_STATE_DO_AUTOSYNC(1)
            && (((bit_before == 0) && param.op == SW_ST_BIT_SET) 
                  || (bit_before == 1 && param.op == SW_ST_BIT_CLEAR))){
        /* note: we are syncing the whole SHR_BITDCL */
            shr_sw_state_auto_sync(_unit, ((uint8*) &(param.sw_state_bmp)[param._bit_num / SHR_BITWID]), SHR_BITALLOCSIZE_FROM_START_BIT(param._bit_num, 1));                    
        }     
        break;
      case SW_ST_BIT_RANGE_WRITE:
      case SW_ST_BIT_RANGE_AND:
      case SW_ST_BIT_RANGE_OR:
      case SW_ST_BIT_RANGE_XOR:
      case SW_ST_BIT_RANGE_REMOVE:
        if (!unmodified && SW_STATE_DO_AUTOSYNC(1)){
        /* note: we are syncing from sw_state_bmp_first to sw_state_bmp_first + _range */ 
          shr_sw_state_auto_sync(_unit, ((uint8*) &(param.sw_state_bmp)[param.sw_state_bmp_first / SHR_BITWID]), SHR_BITALLOCSIZE_FROM_START_BIT(param.sw_state_bmp_first, param._range));
        }
        break;
      case SW_ST_BIT_RANGE_NEGATE:
      case SW_ST_BIT_RANGE_CLEAR:
      case SW_ST_BIT_RANGE_SET:
        if (!unmodified && SW_STATE_DO_AUTOSYNC(1)){
            /* note: we are syncing from _first to _first + _range */ 
            shr_sw_state_auto_sync(_unit, ((uint8*) &(param.sw_state_bmp)[param.sw_state_bmp_first / SHR_BITWID]), SHR_BITALLOCSIZE(param.sw_state_bmp_first + param._range));
        }      
        break;
      default:
        break;
    }
    SOC_EXIT;
exit:
    SOC_FUNC_RETURN;
}
