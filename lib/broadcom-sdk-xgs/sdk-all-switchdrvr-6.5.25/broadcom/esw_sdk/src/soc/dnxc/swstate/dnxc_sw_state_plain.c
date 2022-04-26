/** \file dnxc_sw_state_plain.c
 * This is the main dnxc sw state module.
 * Module is implementing the access itself. 
 * Plain implementation, meaning pure access without warmboot.
 */
/*
 * $Id: $
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
/* ---------- */

/*
 * Include files
 * {
 */
#include <shared/bsl.h>
#include <shared/mem_measure_tool.h>
#include <soc/dnxc/dnxc_ha.h>
#include <soc/dnxc/swstate/sw_state_defs.h>
#include <soc/dnxc/swstate/dnxc_sw_state.h>
#include <soc/dnxc/swstate/dnxc_sw_state_plain.h>
#include <soc/dnxc/swstate/dnxc_sw_state_journal.h>
#include <soc/dnxc/swstate/dnxc_sw_state_utils.h>
#include <soc/dnxc/swstate/dnxc_sw_state_verifications.h>
#include <soc/dnxc/swstate/auto_generated/types/dnxc_module_ids_types.h>
#include <soc/dnxc/swstate/auto_generated/types/multithread_analyzer_types.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnxc/swstate/dnxc_sw_state_common.h>
#include <soc/dnxc/dnxc_verify.h>
#include <sal/core/libc.h>
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

typedef struct {
    dnxc_sw_state_allocation_dll_t dll;
    uint32 allocated_size;
    uint32 measurement_start_point;
} dnxc_sw_state_plain_db_t;

dnxc_sw_state_plain_db_t *sw_state_plain_db[SOC_MAX_NUM_DEVICES];

#define DNXC_SW_STATE_MEMORY_MEASUREMENT_INCREASE(_size) \
        memory_measurement_dnx_sw_state_sample((_size), 1)

#ifdef DNX_SW_STATE_VERIFICATIONS
/**
 * \brief - validate ptr integrity, start santinel
 */
#define DNXC_SW_STATE_VALIDATE_PTR_START_SANTINEL(_ptr)                                                                                                                                 \
do{                                                                                                                                                                                     \
    if(((dnxc_sw_state_allocation_data_prefix_t *)(((uint8 *)_ptr) - sizeof(dnxc_sw_state_allocation_data_prefix_t)))->start_santinel != DNXC_SW_STATE_MEMORY_ALLOCATION_START_SANTINEL)\
    {                                                                                                                                                                                   \
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error in swstate START_SANTINEL validation");                                                                                                    \
    }                                                                                                                                                                                   \
} while(0)

/**
 * \brief - validate ptr integrity, end santinel
 */
#define DNXC_SW_STATE_VALIDATE_PTR_END_SANTINEL(_ptr, _size)                                                                                \
do{                                                                                                                                         \
    if(((dnxc_sw_state_allocation_data_suffix_t *)(((uint8 *)_ptr) + _size))->end_santinel != DNXC_SW_STATE_MEMORY_ALLOCATION_END_SANTINEL) \
    {                                                                                                                                       \
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error in swstate START_SANTINEL validation");                                                        \
    }                                                                                                                                       \
} while(0)

/**
 * \brief - validate ptr integrity
 */
#define DNXC_SW_STATE_VALIDATE_PTR(_unit, _ptr,_size)           \
do {                                                            \
    if(DNXC_VERIFY_ALLOWED_GET(_unit))                          \
    {                                                           \
        DNXC_SW_STATE_VALIDATE_PTR_START_SANTINEL(_ptr);        \
        DNXC_SW_STATE_VALIDATE_PTR_END_SANTINEL(_ptr, _size);   \
    }                                                           \
} while(0)

/**
 * \brief - validate dll entry integrity
 */
#define DNXC_SW_STATE_VALIDATE_DLL_ENTRY(_unit, _dll_entry)                                     \
do {                                                                                            \
    if(DNXC_VERIFY_ALLOWED_GET(_unit))                                                          \
    {                                                                                           \
        if(_dll_entry->start_santinel != DNXC_SW_STATE_MEMORY_ALLOCATION_START_SANTINEL)        \
        {                                                                                       \
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error in swstate dll START_SANTINEL validation");    \
        }                                                                                       \
        if(_dll_entry->end_santinel != DNXC_SW_STATE_MEMORY_ALLOCATION_END_SANTINEL)            \
        {                                                                                       \
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error in swstate dll END_SANTINEL validation");      \
        }                                                                                       \
    }                                                                                           \
} while(0)

#else /* DNX_SW_STATE_VERIFICATIONS */

#define DNXC_SW_STATE_VALIDATE_PTR_START_SANTINEL(_ptr)
#define DNXC_SW_STATE_VALIDATE_PTR_END_SANTINEL(_ptr, _size)
#define DNXC_SW_STATE_VALIDATE_PTR(_unit, _ptr,_size)
#define DNXC_SW_STATE_VALIDATE_DLL_ENTRY(_unit, _dll_entry)

#endif /* DNX_SW_STATE_VERIFICATIONS */

/*
 * }
 */

/**
 * \brief - allocate memory, record allocated memory
 */
STATIC int dnxc_sw_state_alloc_plain_internal(int unit, uint8 **ptr, uint32 size, char *dbg_string)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    *ptr = sal_alloc2(size, SAL_ALLOC_F_ZERO, dbg_string);
    if(NULL == (*ptr)) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_MEMORY, "sw state ERROR: failed to allocate data\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }
    DNXC_SW_STATE_MEMORY_MEASUREMENT_INCREASE(size);
    sal_set_alloc_counters_offset(size, 0);
    sal_set_alloc_counters_offset_main_thr(size, 0);

    DNXC_SW_STATE_FUNC_RETURN;
}

/**
 * \brief - free memory, record freed memory
 */
STATIC int dnxc_sw_state_free_plain_internal (int unit, uint8 **ptr, uint32 size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    SHR_FREE(*ptr);
    sal_set_alloc_counters_offset(0, size);
    sal_set_alloc_counters_offset_main_thr(0, size);

    DNXC_SW_STATE_FUNC_RETURN;
}

/**
 * \brief - see .h file
 */
int dnxc_sw_state_dll_entry_add(int unit, uint8 **ptr_location, dnxc_sw_state_allocation_data_prefix_t *ptr_prefix, dnxc_sw_state_dll_entry_type_e type)
{
    dnxc_sw_state_allocation_dll_entry_t *entry = NULL;
    dnxc_sw_state_allocation_dll_entry_t *prev = NULL;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    SHR_IF_ERR_EXIT(dnxc_sw_state_alloc_plain_internal(unit, (uint8 **) &entry, sizeof(dnxc_sw_state_allocation_dll_entry_t), "sw state dll entry"));

    /*
     * Update sw state allocated size.
     */
    sw_state_plain_db[unit]->allocated_size += sizeof(dnxc_sw_state_allocation_dll_entry_t);

    entry->prev = NULL;
    entry->next = NULL;
    entry->ptr = *ptr_location;
    entry->ptr_location = ptr_location;
    entry->type = type;
    entry->start_santinel = DNXC_SW_STATE_MEMORY_ALLOCATION_START_SANTINEL;
    entry->end_santinel = DNXC_SW_STATE_MEMORY_ALLOCATION_END_SANTINEL;

    /*
     * add entry to double linked list
     * don't store ptr_location for swstate structure allocations
     */
    if(NULL == sw_state_plain_db[unit]->dll.head) {
        sw_state_plain_db[unit]->dll.head = entry;
        sw_state_plain_db[unit]->dll.tail = entry;
    } else {
        prev = sw_state_plain_db[unit]->dll.tail;
        entry->prev = prev;
        prev->next = entry;
        sw_state_plain_db[unit]->dll.tail = entry;
    }

    /*
     * add a reference to the dll for non mutex creation cases
     */
    if (ptr_prefix != NULL) {
        ptr_prefix->dll_entry = entry;
    }

    sw_state_plain_db[unit]->dll.entry_counter++;

    DNXC_SW_STATE_FUNC_RETURN;
}

/**
 * \brief - disconnects a double linked list entry from the 
 *        list.
 */
STATIC int dnxc_sw_state_dll_entry_free(int unit, dnxc_sw_state_allocation_dll_entry_t *dll_entry)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    DNXC_SW_STATE_VERIFICATIONS_ONLY(SHR_NULL_CHECK(dll_entry, SOC_E_INTERNAL, "sw state ERROR: cannot free NULL dll_entry"));

    /*
     * validate that the dll entry is intact
     */
    DNXC_SW_STATE_VALIDATE_DLL_ENTRY(unit, dll_entry);

    /*
     * disconnect entry from linked list
     */
    if((NULL == dll_entry->next) && (NULL == dll_entry->prev )) {
        /*
         * only entry in the linked list scenario, no need to disconnect
         */
        sw_state_plain_db[unit]->dll.head = NULL;
        sw_state_plain_db[unit]->dll.tail = NULL;
    } else {
        /*
         * disconnect previous, if needed
         */
        if(NULL == dll_entry->prev){
            sw_state_plain_db[unit]->dll.head = dll_entry->next;
            sw_state_plain_db[unit]->dll.head->prev = NULL;
        } else {
            dll_entry->prev->next = dll_entry->next;
        }

        /*
         * disconnect next, if needed
         */
        if(NULL == dll_entry->next){
            sw_state_plain_db[unit]->dll.tail = dll_entry->prev;
            sw_state_plain_db[unit]->dll.tail->next = NULL;
        } else {
            dll_entry->next->prev = dll_entry->prev;
        }
    }

    /*
     * free entry, decrease counter
     */
    SHR_IF_ERR_EXIT(dnxc_sw_state_free_plain_internal(unit, (uint8 **) &dll_entry, sizeof(dnxc_sw_state_allocation_dll_entry_t)));
    sw_state_plain_db[unit]->dll.entry_counter--;

    DNXC_SW_STATE_FUNC_RETURN;
}


/**
 * \brief - see .h file.
 */
int dnxc_sw_state_dll_entry_free_by_ptr_location(int unit, uint8 **ptr_location)
{

    dnxc_sw_state_allocation_dll_entry_t *entry = sw_state_plain_db[unit]->dll.head;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    DNXC_SW_STATE_VERIFICATIONS_ONLY(SHR_NULL_CHECK(ptr_location, SOC_E_INTERNAL, "sw state ERROR: invalid pointer location"));
    DNXC_SW_STATE_VERIFICATIONS_ONLY(SHR_NULL_CHECK(*ptr_location, SOC_E_INTERNAL, "sw state ERROR: invalid pointer location"));

    while(entry != NULL)
    {
        if(entry->ptr_location == ptr_location)
        {
            break;
        }
        entry = entry->next;
    }

    DNXC_SW_STATE_VERIFICATIONS_ONLY(SHR_NULL_CHECK(entry, SOC_E_INTERNAL, "sw state ERROR: associated dll entry not found"));
    
    SHR_IF_ERR_EXIT(dnxc_sw_state_dll_entry_free(unit, entry));

    DNXC_SW_STATE_FUNC_RETURN;
}

/**
 * \brief - Calculates the location of the real pointer and frees the allocated data.
 */
STATIC int dnxc_sw_state_free_sw_state_pointer(int unit, uint32 module_id, uint8 *ptr)
{
    uint32 size = 0;
    uint32 nof_elements = 0;
    uint32 element_size = 0;
    uint8 *real_ptr = NULL;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    /* 
     * get the full allocation size, pointer integration test is performed internally
     */
    SHR_IF_ERR_EXIT(dnxc_sw_state_alloc_size_plain(unit, module_id, ptr, &nof_elements, &element_size));
    size = nof_elements * element_size;
    size = DNXC_SW_STATE_ALIGN_SIZE(size);
    size += (sizeof(dnxc_sw_state_allocation_data_prefix_t) + sizeof(dnxc_sw_state_allocation_data_suffix_t));

    /*
     * calculate the location of the real pointer and free memory
     */
    real_ptr = (ptr - sizeof(dnxc_sw_state_allocation_data_prefix_t));

    SHR_IF_ERR_EXIT(dnxc_sw_state_free_plain_internal(unit, &real_ptr, size));

    DNXC_SW_STATE_FUNC_RETURN;
}

/*
 * see .h file for description
 */
int dnxc_sw_state_init_plain(
    int unit,
    uint32 flags)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    sw_state_plain_db[unit] = sal_alloc(sizeof(dnxc_sw_state_plain_db_t), "sw_state_plain_db");
    SHR_NULL_CHECK(sw_state_plain_db[unit], SOC_E_MEMORY, "could not allocate memory for sw_state_plain_db");

    sw_state_plain_db[unit]->dll.entry_counter = 0;
    sw_state_plain_db[unit]->dll.head = NULL;
    sw_state_plain_db[unit]->dll.tail = NULL;
    sw_state_plain_db[unit]->allocated_size = 0;
    sw_state_plain_db[unit]->measurement_start_point = 0;

#if defined(BCM_WARM_BOOT_SUPPORT)
    /* in case we are compiling with warmboot but loading without warmboot soc property, plain implementation need to allocate roots_Array instead of sw_state_wb implementation this is because the access layer is refering to the roots_array */
    if(dnxc_sw_state_is_warmboot_supported_get(unit) == FALSE) {
        sw_state_roots_array[unit] = sal_alloc2(sizeof(void*) * NOF_MODULE_ID, SAL_ALLOC_F_ZERO, "alloc sw state roots array (plain implementation)");
    }
#else
    sw_state_roots_array[unit] = sal_alloc2(sizeof(void*) * NOF_MODULE_ID, SAL_ALLOC_F_ZERO, "alloc sw state roots array (plain implementation)");
#endif

    SHR_EXIT();
    DNXC_SW_STATE_FUNC_RETURN;
}

/*
 * see .h file for description
 */
int dnxc_sw_state_deinit_plain(
    int unit,
    uint32 flags)
{
    /*
     * use a default module_id in freeing all sw state memory
     */
    uint32 module_id = 0;

    dnxc_sw_state_allocation_dll_entry_t *entry = NULL;
    dnxc_sw_state_allocation_dll_entry_t *prev = NULL;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (sw_state_plain_db[unit] != NULL)
    {
        entry = sw_state_plain_db[unit]->dll.tail;
    }

    /*
     * traverse double linked list, tail --> head
     * free double linked list entry, free associated sw state memory
     */
    while(entry != NULL) {
        /*
         * validate that the dll entry is intact
         */
        DNXC_SW_STATE_VALIDATE_DLL_ENTRY(unit, entry);

        /*
         * free sw state pointer (or destroy it if it's a mutex), put null in sw state location
         */
        if (entry->type == DNXC_SW_STATE_DLL_POINTER)
        {
            SHR_IF_ERR_EXIT(dnxc_sw_state_free_sw_state_pointer(unit, module_id, entry->ptr));

            /*
             * set null in sw state only if the pointer kept in the dynamic linked list entry
             * is the same as the one pointed to by ptr_location
             */
            if(entry->ptr == *(entry->ptr_location))
            {
                *(entry->ptr_location) = NULL;
            }
        }
        else if(entry->type == DNXC_SW_STATE_DLL_MUTEX)
        { 
            sal_mutex_destroy(((sw_state_mutex_t*)(entry->ptr_location))->mtx);
            ((sw_state_mutex_t*)(entry->ptr_location))->mtx = NULL;
        }
        else if(entry->type == DNXC_SW_STATE_DLL_SEM)
        { 
            sal_sem_destroy(((sw_state_sem_t*)(entry->ptr_location))->sem);
            ((sw_state_sem_t*)(entry->ptr_location))->sem = NULL;
        }
        else
        {
            SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL, "sw state ERROR: unrecognized entry type\n%s%s%s", EMPTY, EMPTY, EMPTY);
        }

        /*
         * free associated double linked list entry and move to the next entry
         */
        prev = entry->prev;
        SHR_IF_ERR_EXIT(dnxc_sw_state_dll_entry_free(unit, entry));

        entry = prev;
    }

    if (sw_state_plain_db[unit] != NULL)
    {
        sw_state_plain_db[unit]->dll.entry_counter = 0;
        sw_state_plain_db[unit]->dll.head = NULL;
        sw_state_plain_db[unit]->dll.tail = NULL;
    }

#if defined(BCM_WARM_BOOT_SUPPORT)
    if(dnxc_sw_state_is_warmboot_supported_get(unit) == FALSE) {
        SHR_FREE(sw_state_roots_array[unit]);
    }
#else
        SHR_FREE(sw_state_roots_array[unit]);
#endif

    SHR_FREE(sw_state_plain_db[unit]);

    DNXC_SW_STATE_FUNC_RETURN;
}

/*
 * see .h file for description
 */
int dnxc_sw_state_module_init_plain(int unit, uint32 module_id, uint32 size, uint32 flags, char *dbg_string)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    SHR_IF_ERR_EXIT(dnxc_sw_state_alloc_plain(unit, module_id, (uint8**)&sw_state_roots_array[unit][module_id], 1, size, flags, dbg_string));

    DNXC_SW_STATE_FUNC_RETURN;
}

/*
 * see .h file for description
 */
int dnxc_sw_state_module_deinit_plain(int unit, uint32 module_id, uint32 flags, char *dbg_string)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    SHR_IF_ERR_EXIT(dnxc_sw_state_free_plain(unit, module_id, (uint8**)&sw_state_roots_array[unit][module_id], flags, dbg_string));

    DNXC_SW_STATE_FUNC_RETURN;
}

/*
 * see .h file for description
 */
int dnxc_sw_state_alloc_plain(int unit, uint32 module_id, uint8 **ptr_location, uint32 nof_elements, uint32 element_size, uint32 flags, char* dbg_string)
{

/**
 *                                   STRUCTURE OF SW STATE ALLOCATION
 *  +----------------+----------------+--------------+--------------+-------------------------+--------------+
 *  | start_santinel |     ptr_ll     | nof_elements | element_size |         DATA            | end_santinel |
 *  +----------------+----------------+--------------+--------------+-------------------------+--------------+
 *       4 bytes     sizeof(dll_entry*)    4 bytes        4 bytes     n bytes (4 byte rounded)     4 bytes    
 *
 *                            (total size = size + 16 + sizeof(uint8 *) + alignment padding)
 */

    uint32 size = 0;
    uint32 full_size = 0;
    uint8  is_skip_alloc_mode = 0;
    uint32 _nof_elements = nof_elements;

    /*
     * pointers to the prefix and suffix portion of the allocated chunk
     */
    dnxc_sw_state_allocation_data_prefix_t *ptr_prefix = NULL;
    dnxc_sw_state_allocation_data_suffix_t *ptr_suffix = NULL;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    SHR_IF_ERR_EXIT(dnxc_sw_state_skip_alloc_mode_get(unit, &is_skip_alloc_mode));
    if (is_skip_alloc_mode) {
        SHR_EXIT();
    }

    /*
     * Set the nof_elements to 1, if the user is using invalid number.
     */
    if (_nof_elements < 1)
    {
        _nof_elements = 1;
    }

    /*
     * Verify input.
     */
    DNXC_SW_STATE_IF_ERR_EXIT(dnxc_sw_state_alloc_verify_common(unit, module_id, ptr_location, _nof_elements, element_size, flags, dbg_string));

    /*
     * calculate allocation size and round it up to next 32 bit
     */
    size = _nof_elements * element_size;
    size = DNXC_SW_STATE_ALIGN_SIZE(size);

    /*
     * calculate the full size as size of data + prefix + suffix.
     * allocate memory for the entire chunk, clear memory
     */
    full_size = sizeof(dnxc_sw_state_allocation_data_prefix_t) + size + sizeof(dnxc_sw_state_allocation_data_suffix_t);

    SHR_IF_ERR_EXIT(dnxc_sw_state_alloc_plain_internal(unit, (uint8 **) ptr_location, full_size, dbg_string));

    /*
     * Update sw state allocated size.
     */
    sw_state_plain_db[unit]->allocated_size += full_size;

    /*
     * calculate the location of the prefix segment, set prefix fields
     */
    ptr_prefix = (dnxc_sw_state_allocation_data_prefix_t *)(*ptr_location);
    ptr_prefix->start_santinel = DNXC_SW_STATE_MEMORY_ALLOCATION_START_SANTINEL;
    ptr_prefix->nof_elements = _nof_elements;
    ptr_prefix->element_size = element_size;

    /*
     * adjust ptr_location to point to the data segment 
     */
    *ptr_location += sizeof(dnxc_sw_state_allocation_data_prefix_t);

    /*
     * calculate the location of the suffix segment, set suffix fields
     */
    ptr_suffix = (dnxc_sw_state_allocation_data_suffix_t *)(*ptr_location + size);
    ptr_suffix->end_santinel = DNXC_SW_STATE_MEMORY_ALLOCATION_END_SANTINEL;

    /*
     * create a new dynamic linked list entry and add a reference to it in memory 
     * this is used to free all alocations during deinit 
     * Corner case: don't add allocs coming from MULTITHREAD_ANALYZER_MODULE_ID as they should survive deinit
     */
    if (module_id != MULTITHREAD_ANALYZER_MODULE_ID) {
        SHR_IF_ERR_EXIT(dnxc_sw_state_dll_entry_add(unit, ptr_location, ptr_prefix, DNXC_SW_STATE_DLL_POINTER));
    }

#ifdef BCM_DNX_SUPPORT
    /* 
     * don't journal allocations that came from the journal itself
     */
    if (dnxc_sw_state_journal_is_done_init(unit) && !sw_state_is_flag_on(flags,  DNXC_SW_STATE_JOURNAL_ROLLING_BACK)) {
        SHR_IF_ERR_EXIT(dnxc_sw_state_journal_log_alloc(unit, module_id, ptr_location));
    }
#endif /* BCM_DNX_SUPPORT */

    DNXC_SW_STATE_FUNC_RETURN;
}

/*
 * see .h file for description
 */
int dnxc_sw_state_alloc_size_plain(int unit, uint32 module, uint8 *location, uint32 *nof_elements, uint32 *element_size)
{
    uint32 size = 0;
    dnxc_sw_state_allocation_data_prefix_t *ptr_prefix = NULL;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    /*
     * Verify input.
     */
    DNXC_SW_STATE_IF_ERR_EXIT(dnxc_sw_state_alloc_size_verify_common(unit, module, location, nof_elements, element_size));

    /*
     * adjust location to beginning of allocation
     */
    ptr_prefix = (dnxc_sw_state_allocation_data_prefix_t *)(location - sizeof(dnxc_sw_state_allocation_data_prefix_t));

    *nof_elements = ptr_prefix->nof_elements;
    *element_size = ptr_prefix->element_size;

    size = ptr_prefix->nof_elements * ptr_prefix->element_size;
    size = DNXC_SW_STATE_ALIGN_SIZE(size);

    DNXC_SW_STATE_VALIDATE_PTR(unit, location, size);

    DNXC_SW_STATE_FUNC_RETURN;
}

#ifdef DNX_SW_STATE_VERIFICATIONS
#ifdef DNXC_VERIFICATION
/*
 * Internal sw state free verify.
 */
STATIC int dnxc_sw_state_free_plain_internal_verify(int unit, uint32 module_id, uint8 **ptr_location, uint32 flags, char *dbg_string, dnxc_sw_state_allocation_dll_entry_t *dll_entry)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if(((dll_entry->ptr_location != ptr_location) && !sw_state_is_flag_on(flags, DNXC_SW_STATE_LINKED_LIST_FREE)) || (dll_entry->ptr != *ptr_location))
    {

        if(dll_entry->ptr != NULL)
        {
            LOG_CLI((BSL_META("dll_entry->ptr: %p\n"), (void *)dll_entry->ptr));
        }

        if(dll_entry->ptr_location != NULL)
        {
            LOG_CLI((BSL_META("dll_entry->ptr_location: %p\n"), (void *)dll_entry->ptr_location));
        }

        if(dll_entry->next != NULL)
        {
            LOG_CLI((BSL_META("dll_entry->next: %p\n"), (void *)(dll_entry->next)));
        }

        if(dll_entry->prev != NULL)
        {
            LOG_CLI((BSL_META("dll_entry->prev: %p\n"), (void *)(dll_entry->prev)));
        }

        if(dll_entry->type == DNXC_SW_STATE_DLL_POINTER)
        {
            LOG_CLI((BSL_META("swstate ptr entry\n")));
        }
        else
        {
            LOG_CLI((BSL_META("swstate mutex entry\n")));
        }

        if(*ptr_location != NULL)
        {
            LOG_CLI((BSL_META("*ptr_location: %p\n"), (void *)(*ptr_location)));
        }

        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL, "sw state ERROR: associated linked list entry stores a different sw state pointer value \n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    SHR_EXIT();

    DNXC_SW_STATE_FUNC_RETURN;
}
#endif /* DNXC_VERIFICATION */
#endif /* DNX_SW_STATE_VERIFICATIONS */

/*
 * see .h file for description
 */
int dnxc_sw_state_free_plain(int unit, uint32 module_id, uint8 **ptr_location, uint32 flags, char *dbg_string)
{
    int result = SOC_E_NONE;
    uint32 nof_elements = 0;
    uint32 element_size = 0;
    /*
     * points to the prefix portion of the allocated chunk
     */
    dnxc_sw_state_allocation_data_prefix_t *ptr_prefix = NULL;
    /*
     * the double linked list entry associated with the allocation
     */
    dnxc_sw_state_allocation_dll_entry_t *dll_entry = NULL;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    /*
     * Verify input.
     */
    DNXC_SW_STATE_IF_ERR_EXIT(dnxc_sw_state_free_verify_common(unit, module_id, ptr_location, flags, dbg_string));

    if (NULL == (*ptr_location)) {
        return SOC_E_NONE;
    }

    dnxc_sw_state_alloc_size_plain(unit, module_id, *ptr_location, &nof_elements, &element_size);

#ifdef BCM_DNX_SUPPORT
    if (dnxc_sw_state_journal_is_done_init(unit) && !sw_state_is_flag_on(flags, DNXC_SW_STATE_JOURNAL_ROLLING_BACK)) {
        result = dnxc_sw_state_journal_log_free(unit, module_id, nof_elements, element_size, ptr_location);
    }
#endif /* BCM_DNX_SUPPORT */

    /* 
     * calculate the location of the prefix segment
     */
    ptr_prefix = (dnxc_sw_state_allocation_data_prefix_t *)((uint8 *)(*ptr_location) - sizeof(dnxc_sw_state_allocation_data_prefix_t));

    /*
     * Corner case: don't add allocs coming from MULTITHREAD_ANALYZER_MODULE_ID as they should survive deinit
     */
    if (module_id != MULTITHREAD_ANALYZER_MODULE_ID) {

        /*
         * retrieve the associated dynamic linked list entry, make sure it is not a mutex
         */
        dll_entry = ptr_prefix->dll_entry;
        if(dll_entry->type != DNXC_SW_STATE_DLL_POINTER)
        {
            SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL, "sw state ERROR: associated linked list entry type is not for a swstate ptr\n%s%s%s", EMPTY, EMPTY, EMPTY);
        }

        /*
         * validate that the dll entry is intact
         */
        DNXC_SW_STATE_VALIDATE_DLL_ENTRY(unit, dll_entry);

#ifdef DNX_SW_STATE_VERIFICATIONS
        /*
         * sanity check the double linked list pointer.
         */
        DNXC_VERIFY_INVOKE(SHR_IF_ERR_EXIT(dnxc_sw_state_free_plain_internal_verify(unit, module_id, ptr_location, flags, dbg_string, dll_entry)));

#endif /* DNX_SW_STATE_VERIFICATIONS */

    }

    /*
     * free sw state pointer, put null in sw state location
     */
    SHR_IF_ERR_EXIT(dnxc_sw_state_free_sw_state_pointer(unit, module_id, *ptr_location));
    *ptr_location = NULL;

    /*
     * Corner case: don't add allocs coming from MULTITHREAD_ANALYZER_MODULE_ID as they should survive deinit
     */
    if (module_id != MULTITHREAD_ANALYZER_MODULE_ID) {
        /*
         * free associated double linked list entry
         */
        SHR_IF_ERR_EXIT(dnxc_sw_state_dll_entry_free(unit, dll_entry));
    }

    SHR_IF_ERR_EXIT(result);

    DNXC_SW_STATE_FUNC_RETURN;
}

/** 
 * see .h file for documentation
 */
int dnxc_sw_state_plain_start_size_measurement_point(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    /*
     * Set the measurement
     */
    sw_state_plain_db[unit]->measurement_start_point = sw_state_plain_db[unit]->allocated_size;
    SHR_EXIT();

    DNXC_SW_STATE_FUNC_RETURN;
}

/** 
 * see .h file for documentation
 */
uint32 dnxc_sw_state_plain_size_measurement_get(int unit)
{
    uint32 size;
    size = sw_state_plain_db[unit]->allocated_size - sw_state_plain_db[unit]->measurement_start_point;
    return size;
}

/**
 * \brief - see .h file.
 */
int dnxc_sw_state_total_size_get_plain(int unit, uint32 *size)
{
    uint32 nof_elements;
    uint32 element_size;

    dnxc_sw_state_allocation_dll_entry_t *entry = sw_state_plain_db[unit]->dll.head;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    *size = 0;

    while(entry != NULL)
    {
        /* module_id = -1 as we don't have it and it's not used in this function */
        SHR_IF_ERR_EXIT(dnxc_sw_state_alloc_size_plain(unit, -1, *(entry->ptr_location), &nof_elements, &element_size));
        *size = *size + (nof_elements * element_size);
    }

    DNXC_SW_STATE_FUNC_RETURN;
}

/*
 * service_var - service variables used to help with returning the right type
 */
uint8* dnxc_sw_state_plain_calc_pointer(int unit, sal_vaddr_t offset)
{
    return INT_TO_PTR(offset);
}

sal_vaddr_t dnxc_sw_state_plain_calc_offset(int unit, uint8* ptr)
{
    return PTR_TO_UINTPTR(ptr);
}

uint8** dnxc_sw_state_plain_calc_double_pointer(int unit, sal_vaddr_t offset)
{
    return INT_TO_PTR(offset);
}

sal_vaddr_t dnxc_sw_state_plain_calc_offset_from_dptr(int unit, uint8** ptr)
{
    return PTR_TO_UINTPTR(ptr);
}

/** 
 * see .h file for documentation
 */
void dnxc_sw_state_plain_module_size_update(int unit, uint32 module_id, uint32 size, uint32 flags)
{

    return;

}

/** 
 * see .h file for documentation
 */
uint32 dnxc_sw_state_plain_module_size_get(int unit, uint32 module_id)
{

    return 0;

}

#undef _ERR_MSG_MODULE_NAME
