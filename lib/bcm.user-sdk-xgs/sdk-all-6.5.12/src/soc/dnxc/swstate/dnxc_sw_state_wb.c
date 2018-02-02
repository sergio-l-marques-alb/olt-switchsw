/** \file dnxc_sw_state_wb.c
 *  
 * This is a dnxc sw state module.focused on wb access. 
 * Module is implementing the access functions for wb 
 * implementation. meanning it takes warmboot into consideration 
 * when accessing the sw state. this is one of (currently) two 
 * sets of access implementation and is only used when customer 
 * compiled with WARMBOOT compilation flags and enabled warmboot 
 * for the unit using dnx data (soc property). 
 *  
 * Note: even if compiled with WB and unit supports wb, some sw 
 * state modules may still use "plain" (see 
 * dnxc_sw_state_plain.c) implementation if the module does not 
 * require wb. 
 *  
 *  */
/*
 * $Id: $
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

/* 
 * Note! 
 * This include statement must be at the top of every sw state .c file 
 * It points to a set of in-code compilation flags that must be taken into 
 * account for every sw state componnent compilation 
 */
#include <soc/dnxc/swstate/sw_state_features.h>
/* ---------- */

/*
 * Include files
 * {
 */
#include <assert.h>
#include <shared/bsl.h>
#include <shared/mem_measure_tool.h>
#include <soc/dnxc/swstate/sw_state_defs.h>
#include <soc/dnxc/swstate/dnxc_sw_state.h>
#include <soc/dnxc/swstate/dnxc_sw_state_wb.h>
#include <soc/dnxc/swstate/dnxc_sw_state_journal.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dcmn/error.h>
/*
 * }
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

/*
 * Defines
 * {
 */

#define DNXC_SW_STATE_SCACHE_HANDLE_SET(_wb_handle, unit, _id)\
    SOC_SCACHE_HANDLE_SET(_wb_handle, unit, SOC_SCACHE_DNXC_SW_STATE, _id)
    
#define BLOCK_PTR_DIFF ((uint8*)data_block_header->data_ptr - (uint8*)restored_data_ptr)

/*
 * }
 */

/*
 * Typedefs
 * {
 */ 
 
/* scache operations */
typedef enum dnxc_sw_state_scache_oper_e
{
    socDnxcSwStateScacheRetreive,
    socDnxcSwStateScacheCreate,
    socDnxcSwStateScacheRealloc
} dnxc_sw_state_scache_oper_t;

/*
 * }
 */
 
/************* layout ****************/
/* header/roots-array/data/ptr-stack */
/*************************************/

dnxc_sw_state_data_block_header_t           *dnxc_sw_state_data_block_header[SOC_MAX_NUM_DEVICES];

void**  sw_state_roots_array[SOC_MAX_NUM_DEVICES];

static int dnxc_sw_state_data_block_header_init(
    int unit,
    uint32 size);

static int dnxc_sw_state_restore_pointers(
    int unit,
    dnxc_sw_state_data_block_header_t * data_block_header);
    
static int
dnxc_sw_state_scache_ptr_get(int unit, soc_scache_handle_t handle, dnxc_sw_state_scache_oper_t oper,
                             int flags, uint32 *size, uint8 **scache_ptr, int *already_exists);
    
/*
 * see .h file for description
 */
int dnxc_sw_state_init_wb(
    int unit,
    uint32 flags,
    uint32 sw_state_max_size)
{
    int                                rc = SOC_E_NONE;
    soc_scache_handle_t                wb_handle_orig = 0;
    int                                scache_flags = 0;
    uint32                             restored_size = 0;
    int                                already_exists = 0;
    uint8                              *scache_ptr = NULL;
    dnxc_sw_state_data_block_header_t  *retrieved_data_block_header;
    dnxc_sw_state_init_mode_t          init_mode;
    
    DNX_SW_STATE_INIT_FUNC_DEFS;
    
    DNXC_SW_STATE_SCACHE_HANDLE_SET(wb_handle_orig, unit, DNXC_SW_STATE_SCACHE_HANDLE_DATA_BLOCK);


    if (SOC_WARM_BOOT(unit)) {
        init_mode = socDnxcSwStateDataBlockRestoreAndOveride;
    } else {
        init_mode = socDnxcSwStateDataBlockRegularInit;
    }
    
    switch (init_mode) {
        case socDnxcSwStateDataBlockRegularInit:

            if (sw_state_max_size == 0) {
                SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL,"Unit:%d sw_state_max_size cannot be 0 \n%s%s", unit, EMPTY, EMPTY);
            }
            SHR_IF_ERR_EXIT(dnxc_sw_state_data_block_header_init(unit, sw_state_max_size));

            break;

        case socDnxcSwStateDataBlockRestoreAndOveride:
            /* retrieve buffer */
            
            rc = dnxc_sw_state_scache_ptr_get(unit, wb_handle_orig, socDnxcSwStateScacheRetreive, scache_flags,
                                    &restored_size, &scache_ptr, &already_exists);
            SHR_IF_ERR_EXIT(rc);
            retrieved_data_block_header = (dnxc_sw_state_data_block_header_t *) scache_ptr;

            SHR_IF_ERR_EXIT(dnxc_sw_state_restore_pointers(unit, retrieved_data_block_header)); 

             dnxc_sw_state_data_block_header[unit] = retrieved_data_block_header;
             
             sw_state_roots_array[unit] = dnxc_sw_state_data_block_header[unit]->roots_array;

            break;
    }

    dnxc_sw_state_data_block_header[unit]->is_init = 1;

    SHR_IF_ERR_EXIT(rc);



    DNX_SW_STATE_FUNC_RETURN;
}

/*
 * Perform regular init for the sw state.
*/
int dnxc_sw_state_data_block_header_init(int unit, uint32 size) {

    int                                rc = SOC_E_NONE;
    soc_scache_handle_t                wb_handle_orig = 0;
    int                                scache_flags=0;
    int                                already_exists = 0;
    uint8                             *scache_ptr = NULL;
    uint32                             total_buffer_size = 0;

    DNX_SW_STATE_INIT_FUNC_DEFS;

    DNXC_SW_STATE_SCACHE_HANDLE_SET(wb_handle_orig, unit, DNXC_SW_STATE_SCACHE_HANDLE_DATA_BLOCK);

    total_buffer_size = size; 

    /* allocate new scache buffer */
    rc = dnxc_sw_state_scache_ptr_get(unit, 
                                     wb_handle_orig, 
                                     socDnxcSwStateScacheCreate, 
                                     scache_flags,
                                     &total_buffer_size, &scache_ptr, &already_exists);
    SHR_IF_ERR_EXIT(rc);

    /* make scache_ptr 4 bytes aligned */
    scache_ptr = (uint8*)(((unsigned long)scache_ptr + 0x3) & (~0x3));

    dnxc_sw_state_data_block_header[unit] = (dnxc_sw_state_data_block_header_t *) scache_ptr;

    dnxc_sw_state_data_block_header[unit]->total_buffer_size = total_buffer_size;
    dnxc_sw_state_data_block_header[unit]->data_size = size - sizeof(dnxc_sw_state_data_block_header_t);
    /* currently data block is empty so size_left==data_size */
    dnxc_sw_state_data_block_header[unit]->size_left = dnxc_sw_state_data_block_header[unit]->data_size;

    /* sets roots_array to be right after the data block header */
    dnxc_sw_state_data_block_header[unit]->roots_array = (void**)(dnxc_sw_state_data_block_header[unit] + 1);
    
    /* pointer to the location where actual data block begins */
    dnxc_sw_state_data_block_header[unit]->data_ptr = (uint8*)dnxc_sw_state_data_block_header[unit]->roots_array;
    
    /* init the next free data slot to the be the first slot in the data block */
    dnxc_sw_state_data_block_header[unit]->next_free_data_slot = (uint8*)(dnxc_sw_state_data_block_header[unit]->roots_array) + sizeof(void*) * NOF_MODULE_ID;
    
    dnxc_sw_state_data_block_header[unit]->ptr_offsets_sp = 
        (dnxc_sw_state_alloc_element_t *) ((uint8*)dnxc_sw_state_data_block_header[unit] + size - sizeof(dnxc_sw_state_alloc_element_t));

    dnxc_sw_state_data_block_header[unit]->ptr_offsets_stack_base = dnxc_sw_state_data_block_header[unit]->ptr_offsets_sp;

    /*init the data to be all zeros*/
    sal_memset(dnxc_sw_state_data_block_header[unit]->data_ptr, 0x0,
                dnxc_sw_state_data_block_header[unit]->data_size);

    dnxc_sw_state_data_block_header[unit]->is_init = 1;


    DNX_SW_STATE_FUNC_RETURN;
}


/* restore pointer for data block header */
int dnxc_sw_state_restore_pointers(int unit, dnxc_sw_state_data_block_header_t *data_block_header){
    
    uint8                            **tmp_ptr;
    uint8                             *restored_data_ptr;
    dnxc_sw_state_alloc_element_t      *p;
    DNX_SW_STATE_INIT_FUNC_DEFS;

    /* save based ptr of the saved data block for calculations below */
    restored_data_ptr = data_block_header->data_ptr;

    data_block_header->data_ptr = (uint8*)(data_block_header + 1);
    
    data_block_header->roots_array = (void**)(data_block_header->data_ptr);

    data_block_header->next_free_data_slot =
        ((uint8*)(data_block_header->next_free_data_slot) + BLOCK_PTR_DIFF);

    data_block_header->ptr_offsets_sp = (dnxc_sw_state_alloc_element_t*)
        ((uint8*)(data_block_header->ptr_offsets_sp) + BLOCK_PTR_DIFF);
    
    data_block_header->ptr_offsets_stack_base = (dnxc_sw_state_alloc_element_t*)
        ((uint8*)(data_block_header->ptr_offsets_stack_base) + BLOCK_PTR_DIFF);

    /* update the stack's elements and the sw_state pointers*/
    p = data_block_header->ptr_offsets_sp;
    ++p;

    while (p <= data_block_header->ptr_offsets_stack_base) {
        tmp_ptr = (uint8 **) ((uint8*)(data_block_header->data_ptr) + p->ptr_offset);
        *tmp_ptr += BLOCK_PTR_DIFF;
        p->ptr_value = *tmp_ptr;
        p++;
    }

    SOC_EXIT;

    DNX_SW_STATE_FUNC_RETURN;
}

int dnxc_sw_state_deinit_wb(int unit, uint32 flags) {
    DNX_SW_STATE_INIT_FUNC_DEFS;

    if (dnxc_sw_state_data_block_header[unit] != NULL) {
        sal_memset((void**)(dnxc_sw_state_data_block_header[unit] + 1), 0, sizeof(void*) * NOF_MODULE_ID);
        dnxc_sw_state_data_block_header[unit]->is_init = 0;
    }

    DNX_SW_STATE_FUNC_RETURN;
}

int dnxc_sw_state_module_init_wb(
    int unit,
    uint32 module_id,
    uint32 size,
    uint32 flags,
    char *dbg_string) {

    DNX_SW_STATE_INIT_FUNC_DEFS;
    
    SHR_IF_ERR_EXIT(dnxc_sw_state_alloc_wb(unit, module_id, (uint8**)&(dnxc_sw_state_data_block_header[unit]->roots_array[module_id]), 1, size, flags, dbg_string));
    sw_state_roots_array[unit][module_id] = dnxc_sw_state_data_block_header[unit]->roots_array[module_id];
    
    SOC_EXIT;

    DNX_SW_STATE_FUNC_RETURN; 
}

int dnxc_sw_state_module_deinit_wb(
    int unit,
    uint32 module_id,
    uint32 flags,
    char *dbg_string) {
        
    DNX_SW_STATE_INIT_FUNC_DEFS;
    
    sw_state_roots_array[unit][module_id] = NULL;
    dnxc_sw_state_data_block_header[unit]->roots_array[module_id] = NULL;
    
    SOC_EXIT;

    DNX_SW_STATE_FUNC_RETURN;

}


int dnxc_sw_state_alloc_wb(
    int unit,
    uint32 module_id,
    uint8 **ptr_location,
    uint32 nof_elements,
    uint32 element_size,
    uint32 flags,
    char *dbg_string) {

     /**********************************************************************************************************
                  data_ptr
        __________/______________________________________________________________________________
        | header |roots_array|...................................................................|
        |........................................................................................|
        |.....start_santinel | NULL | nof_elements |element_size | DATA | end_santinel           |
        |                                                                             /          |
        |                                                                            /           |
        |                                                         next free data slot            |
        |                                                                                        |
        |                                                                              x   x   x |
        |____________________________________________________________________________|___|___|___|
                                                                                  /            \ 
                                                                                 /              \
                                                                     sp - stack pointer    the stack's base
     
     ***********************************************************************************************************/ 
     
    unsigned int                      alloc_size = 0;
    uint32                            *p = NULL;
    uint32                            size = nof_elements * element_size;
    /*
     * points to the prefix portion of the allocated chunk
     */
    dnxc_sw_state_allocation_data_prefix_t *ptr_prefix = NULL;

    /*
     * points to the suffix portion of the allocated chunk
     */
    dnxc_sw_state_allocation_data_suffix_t *ptr_suffix = NULL;
    
    DNX_SW_STATE_INIT_FUNC_DEFS;

    /* return error if not initialized */
    if (!dnxc_sw_state_data_block_header[unit]->is_init) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INIT,"unit:%d trying to allocate a SW state var w/o initializing the SW state\n%s%s", unit, EMPTY, EMPTY);
    }

    /* return error if pointer is allready allocated */
    if (*ptr_location != NULL) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL,"unit:%d trying to allocate already alocated pointer\n%s%s", unit, EMPTY, EMPTY);
    }

    /* return error if ptr_location is not in range*/
    if ((((uint8 *)ptr_location) < (uint8 *)dnxc_sw_state_data_block_header[unit]->data_ptr)
        || (((uint8 *) ptr_location) > (uint8 *)dnxc_sw_state_data_block_header[unit]->next_free_data_slot)) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_PARAM,"unit:%d trying to allocate a pointer and save it outside of the sw_state scope\n%s%s", unit, EMPTY, EMPTY);
    }


    /* data is 4 bytes alligned */
    DNXC_SW_STATE_ALIGN_SIZE(size);

     /*
     * calculate the full size as size of data + prefix + suffix.
     * allocate memory for the entire chunk, clear memory
     */
    
    alloc_size = sizeof(dnxc_sw_state_allocation_data_prefix_t) + size + sizeof(dnxc_sw_state_allocation_data_suffix_t);

    /* if not enough space exit with error */
    if (dnxc_sw_state_data_block_header[unit]->size_left < alloc_size) {
        unsigned long size_left ;
        unsigned long total_buffer_size ;

        size_left = (unsigned long)(dnxc_sw_state_data_block_header[unit]->size_left) ;
        total_buffer_size = (unsigned long)(dnxc_sw_state_data_block_header[unit]->total_buffer_size) ;
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_MEMORY,"shr_sw_state_alloc: not enough memory for allocation. total %lu size_left %lu alloc_size %lu\n",
                                                    total_buffer_size,size_left,(unsigned long)alloc_size);
    }

    p = (uint32 *) dnxc_sw_state_data_block_header[unit]->next_free_data_slot;
    
    dnxc_sw_state_data_block_header[unit]->next_free_data_slot = (uint8*)dnxc_sw_state_data_block_header[unit]->next_free_data_slot + alloc_size;
    
    dnxc_sw_state_data_block_header[unit]->size_left = dnxc_sw_state_data_block_header[unit]->size_left - (alloc_size + sizeof(dnxc_sw_state_alloc_element_t));

    assert(UINTPTR_TO_PTR(PTR_TO_UINTPTR(p)) == p);


    ptr_prefix = (dnxc_sw_state_allocation_data_prefix_t*)p;

    ptr_prefix->start_santinel = DNX_SWSTATE_MEMORY_ALLOCATION_START_SANTINEL;
    ptr_prefix->nof_elements = nof_elements;
    ptr_prefix->element_size = element_size;
    
    ptr_suffix = (dnxc_sw_state_allocation_data_suffix_t *)(((uint8 *)ptr_prefix) + sizeof(dnxc_sw_state_allocation_data_prefix_t) + size);
    ptr_suffix->end_santinel = DNX_SWSTATE_MEMORY_ALLOCATION_END_SANTINEL;

    *ptr_location = ((uint8*)p) + sizeof(dnxc_sw_state_allocation_data_prefix_t);

    dnxc_sw_state_data_block_header[unit]->ptr_offsets_sp->ptr_offset = (((uint8 *) ptr_location) - (uint8*)dnxc_sw_state_data_block_header[unit]->data_ptr);
    dnxc_sw_state_data_block_header[unit]->ptr_offsets_sp->ptr_value = *ptr_location;
    

    dnxc_sw_state_data_block_header[unit]->ptr_offsets_sp = dnxc_sw_state_data_block_header[unit]->ptr_offsets_sp - 1;

    sal_memset(*ptr_location, 0, size);


    DNX_SW_STATE_FUNC_RETURN;
     
}

int dnxc_sw_state_free_wb(
    int unit,
    uint32 module_id,
    uint8 **ptr_location,
    uint32 flags,
    char *dbg_string) {

    uint8                             is_allocated = 0;
    uint8                             **freed_location = NULL;
    dnxc_sw_state_alloc_element_t     *p = NULL;
    dnxc_sw_state_alloc_element_t     *stack_base = NULL;
    uint32                            size = 0;
    DNX_SW_STATE_INIT_FUNC_DEFS;
    
    stack_base = dnxc_sw_state_data_block_header[unit]->ptr_offsets_stack_base;
            
    /* return error if ptr_location is not in range*/
    if (( *ptr_location < (uint8*)dnxc_sw_state_data_block_header[unit]->data_ptr )
        || ( *ptr_location > (uint8*)dnxc_sw_state_data_block_header[unit]->next_free_data_slot )) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_PARAM,"unit:%d trying to free a pointer outside of the sw_state scope\n%s%s", unit, EMPTY, EMPTY);
    }
    
    p = dnxc_sw_state_data_block_header[unit]->ptr_offsets_sp;
    ++p;

    while (p <= stack_base) {
        if (p->ptr_value == *ptr_location) {
            
            uint32 nof_elements;
            uint32 element_size;
            is_allocated = 1;
            /* mark as NULL in actual pointer uses */
            freed_location = (uint8 **) 
                ((uint8*)dnxc_sw_state_data_block_header[unit]->data_ptr + p->ptr_offset);

            *freed_location = NULL;

            /* zero the freed memory */
            
            SHR_IF_ERR_EXIT(dnxc_sw_state_alloc_size_wb(unit, module_id, *ptr_location, &nof_elements, &element_size));

            
            size = nof_elements * element_size;
            size = size + sizeof(uint32) * 4;
            
            DNXC_SW_STATE_ALIGN_SIZE(size);
            
            sal_memset((uint32*)(*ptr_location) - 3, 0, size);

            break;
        }
        p++;
    }
    

    if (is_allocated == 0) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_PARAM,"unit:%d trying to free an invalid pointer\n%s%s", unit, EMPTY, EMPTY);
    }

    /* fill the hole in the stack and pop the stack */
    if (p != dnxc_sw_state_data_block_header[unit]->ptr_offsets_sp + 1) {
        p->ptr_value = (dnxc_sw_state_data_block_header[unit]->ptr_offsets_sp + 1)->ptr_value;
        p->ptr_offset = (dnxc_sw_state_data_block_header[unit]->ptr_offsets_sp + 1)->ptr_offset;
    }

    /* pop the stack */
    sal_memset(dnxc_sw_state_data_block_header[unit]->ptr_offsets_sp, 0, sizeof(dnxc_sw_state_alloc_element_t));
    dnxc_sw_state_data_block_header[unit]->ptr_offsets_sp = dnxc_sw_state_data_block_header[unit]->ptr_offsets_sp + 1;

    SOC_EXIT;

    DNX_SW_STATE_FUNC_RETURN;
}

int dnxc_sw_state_alloc_size_wb(
    int unit,
    uint32 module,
    uint8 *location,
    uint32 *nof_elements,
    uint32 *element_size) {
    dnxc_sw_state_allocation_data_prefix_t* prefix;

    DNX_SW_STATE_INIT_FUNC_DEFS;

    if ((NULL == location) || (NULL == nof_elements) || (NULL == element_size)) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL, "sw state ERROR: invalid parameter \n%s%s%s", EMPTY, EMPTY, EMPTY);
    }
    
    prefix = (dnxc_sw_state_allocation_data_prefix_t*)(location - sizeof(dnxc_sw_state_allocation_data_prefix_t));

    *element_size = prefix->element_size;
    *nof_elements = prefix->nof_elements;

    DNX_SW_STATE_FUNC_RETURN;
}


int dnxc_sw_state_memcpy_wb(
    int unit,
    uint32 module,
    uint32 size,
    uint8 *dest,
    uint8 *src,
    uint32 flags,
    char *dbg_string) {

    DNX_SW_STATE_INIT_FUNC_DEFS;    

    if(NULL == dest || NULL == src) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL, "sw state ERROR: invalid parameter \n%s%s%s", EMPTY, EMPTY, EMPTY);
    }
    
    sal_memcpy(dest, src, size);

    DNX_SW_STATE_FUNC_RETURN;

}


int dnxc_sw_state_memset_wb(
    int unit,
    uint32 module_id,
    uint8 *dest,
    uint32 value,
    uint32 size,
    uint32 flags,
    char *dbg_string) {
        
    DNX_SW_STATE_INIT_FUNC_DEFS;

    if(NULL == dest) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL, "sw state ERROR: invalid parameter \n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    sal_memset(dest, value, size);

    DNX_SW_STATE_FUNC_RETURN;

}

int dnxc_sw_state_counter_inc_wb(
    int unit,
    uint32 module_id,
    uint8 *ptr_location,
    uint32 inc_value,
    uint32 type_size,
    uint32 flags,
    char *dbg_string) {
        
    DNX_SW_STATE_INIT_FUNC_DEFS;

    switch(type_size) {
        case 1:
            (*(uint8 *)(ptr_location))+=inc_value;
            break;
        case 4:
            (*(uint32 *)(ptr_location))+=inc_value;
            break;
        default:
            SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL, "sw state ERROR: Unsupported counter type \n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    DNX_SW_STATE_FUNC_RETURN;
}


int dnxc_sw_state_counter_dec_wb(
    int unit,
    uint32 module_id,
    uint8 *ptr_location,
    uint32 dec_value,
    uint32 type_size,
    uint32 flags,
    char *dbg_string){
        
    DNX_SW_STATE_INIT_FUNC_DEFS;

    switch(type_size) {
        case 1:
            (*(uint8 *)(ptr_location))-=dec_value;
            break;
        case 4:
            (*(uint32 *)(ptr_location))-=dec_value;
            break;
        default:
            SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL, "sw state ERROR: Unsupported counter type \n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    DNX_SW_STATE_FUNC_RETURN;
}

/* 
 * this function does the interaction with scache module,
 * fetching\creating\modifying an scache buffer and return a pointer to it
 */
int
dnxc_sw_state_scache_ptr_get(int unit, soc_scache_handle_t handle, dnxc_sw_state_scache_oper_t oper,
                             int flags, uint32 *size, uint8 **scache_ptr, int *already_exists)
{
    int        rc = SOC_E_NONE;  
    uint32     allocated_size;
    int        alloc_size;
    int        incr_size;


    DNX_SW_STATE_INIT_FUNC_DEFS;

    if (scache_ptr == NULL) {
     SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_PARAM,"Unit:%d scache_ptr is null.\n%s%s", unit, EMPTY, EMPTY);

          /* return(SOC_E_PARAM); */
    }

    if (oper == socDnxcSwStateScacheCreate) {
        if (size == NULL) {
         SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_PARAM,"Unit:%d size is null.\n%s%s", unit, EMPTY, EMPTY);
           /* return(SOC_E_PARAM); */
        }
        if (already_exists == NULL) {
         SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_PARAM,"Unit:%d already_exist is null.\n%s%s", unit, EMPTY, EMPTY);
          /* return(SOC_E_PARAM); */
        }

        DNXC_SW_STATE_ALIGN_SIZE(*size);
        alloc_size = (*size);

        rc = soc_scache_ptr_get(unit, handle, scache_ptr, &allocated_size);
        if ((rc != SOC_E_NONE) && (rc != SOC_E_NOT_FOUND) ) {
            return(rc);
        }

        if (rc == SOC_E_NONE) { /* already exists */
            (*already_exists) = TRUE;
             SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_PARAM,"Unit:%d scache already_exist.\n%s%s", unit, EMPTY, EMPTY);
            /* return(SOC_E_PARAM); */
        }
        else { /* need to create */
            (*already_exists) = FALSE;
            rc = soc_scache_alloc(unit, handle, alloc_size);
            if (rc != SOC_E_NONE) {
                return(rc);
            }

            rc = soc_scache_ptr_get(unit, handle, scache_ptr, &allocated_size);
            if (rc != SOC_E_NONE) {
                return(rc);
            }
            if ((*scache_ptr) == NULL) {
             SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_PARAM, "Unit:%d Memory failure.\n%s%s", unit, EMPTY, EMPTY);
             /* return(SOC_E_MEMORY); */
            }
        }

        if (alloc_size != allocated_size) {
            SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL, "Unit:%d Allocation size is not enough.\n%s%s", unit, EMPTY, EMPTY);
             /* return(SOC_E_INTERNAL); */
        }
    }

    else if (oper == socDnxcSwStateScacheRetreive) {
        if (size == NULL) {
             SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_PARAM, "Unit:%d Size is null.\n%s%s", unit, EMPTY, EMPTY);
            /* return(SOC_E_PARAM); */
        }

        rc = soc_scache_ptr_get(unit, handle, scache_ptr, &allocated_size);
        if (rc != SOC_E_NONE) {
            return(rc);
        }
        (*size) = allocated_size;
        
        if (already_exists != NULL) {
            (*already_exists) = TRUE;
        }
    }

    else if (oper == socDnxcSwStateScacheRealloc) {
        if (size == NULL) {
             SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_PARAM, "Unit:%d Size is null.\n%s%s", unit, EMPTY, EMPTY);
            /* return(SOC_E_PARAM); */
        }


        /* get current size */
        rc = soc_scache_ptr_get(unit, handle, scache_ptr, &allocated_size);
        if (rc != SOC_E_NONE) {
            return(rc);
        }

        /* allocate new size */
        DNXC_SW_STATE_ALIGN_SIZE(*size);
        alloc_size = (*size);
        incr_size = alloc_size - allocated_size;

        rc = soc_scache_realloc(unit, handle, incr_size);
        if (rc != SOC_E_NONE) {
            return(rc);
        }

        /* get the new pointer */
        rc = soc_scache_ptr_get(unit, handle, scache_ptr, &allocated_size);
        if (rc != SOC_E_NONE) {
            return(rc);
        }

        if (already_exists != NULL) {
            (*already_exists) = TRUE;
        }
    }
    else {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_PARAM, "Unit:%d Invalid parameter.\n%s%s", unit, EMPTY, EMPTY);
       /* return(SOC_E_PARAM); */
    }

    *size = allocated_size; /* update size */

    DNX_SW_STATE_FUNC_RETURN;

}

#undef _ERR_MSG_MODULE_NAME

