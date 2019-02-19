/*
 * $Id: $
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * INFO: This module is one of the components needed for Error Recovery,
 * it is a rollback journal, saving the old values that were overridden
 * by sw_state access calls since the beginning of the last transaction.
 */

/* 
 * Note! 
 * This include statement must be at the top of every sw state .c file 
 * It points to a set of in-code compilation flags that must be taken into 
 * account for every sw state componnent compilation 
 */
#include <soc/dnxc/swstate/sw_state_features.h>
/* ---------- */

#include <assert.h>
#include <shared/bsl.h>
#include <sal/core/thread.h>
#include <soc/dnxc/swstate/dnxc_sw_state.h>
#include <soc/dnxc/swstate/dnxc_sw_state_journal.h>
#include <soc/dnxc/swstate/sw_state_defs.h>
#include <soc/dnxc/dnxc_err_recovery_manager.h>
#include <soc/dnxc/swstate/dnxc_sw_state_dispatcher.h>
#include <sal/core/sync.h>

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL


sw_state_journal_t sw_state_journal[SOC_MAX_NUM_DEVICES];

/**
 * \brief - limit the maximum size of the sw state journal to 10 MB at init
 */
#if 1
#define DNX_SWSTATE_JOURNAL_MAX_SIZE 0
#endif

#define DNX_SW_STATE_JOURNAL_MUTEX_DEFAULT_USEC sal_mutex_FOREVER

#define DNX_SWSTATE_JOURNAL_ENSURE_ON(unit) \
    do{ \
        if(TRUE != dnxc_err_recovery_is_on(unit)) { \
            return SOC_E_NONE; \
        } \
    } while(0)

/**
 * \brief
 *  Recalcuate entry single-pointer addresses, affected by 'free' entry roll-back.
 * \remark
 *  Update address sequence:
 *  - Check if pointer is in range [old_address ;old_address + size] \n
 *  - Update the address to the new location \n
 *  Valid only for 'memcpy' entry type
 */
#define SW_STATE_JOURNAL_UPDATE_ENTRY_ADDRESS(ptr, old_address, new_address, size) \
    do{ \
        if(((ptr) - (old_address) >= 0) && ((old_address) + (size) - (ptr) > 0)) { \
            (ptr) = (new_address) + ((ptr) - (old_address)); \
        } \
    } \
    while(0)

/**
 * \brief
 *  Recalculate entry double-pointer addresses, if affected by 'free' entry roll-back.
 * \remark
 *  Update address sequence:
 *  - Check if double-pointer is in range [old_address ;old_address + size] \n
 *  - Update the address to the new location \n
 *  Double pointer is cast to a single pointer because of pointer arithmetics. \n
 *  Resulting new address after arithmetics is cast to double pointer. \n
 *  Valid only for 'alloc' and 'free' entry types
 */
#define SW_STATE_JOURNAL_UPDATE_ENTRY_PTR_TO_ADDRESS(ptr, old_address, new_address, size) \
    do{ \
        if(((uint8 *)(ptr) - (uint8 *)(old_address) >= 0) \
        && ((uint8 *)(old_address) + (size) - (uint8 *)(ptr) > 0)) { \
            (ptr) = (uint8 **)((uint8 *)(new_address) + ((uint8 *)(ptr) - (uint8 *)(old_address))); \
        } \
    } \
    while(0)

#define SW_STATE_JOURNAL_UPDATE_MUTEX_ENTRY_PTR_TO_ADDRESS(ptr, old_address, new_address, size) \
    do{ \
        if(((uint8 *)(ptr) - (uint8 *)(old_address) >= 0) \
        && ((uint8 *)(old_address) + (size) - (uint8 *)(ptr) > 0)) { \
            (ptr) = (sal_mutex_t *)((uint8 *)(new_address) + ((uint8 *)(ptr) - (uint8 *)(old_address))); \
        } \
    } \
    while(0)


#define SW_STATE_JOURNAL_UPDATE_MUTEX_ENTRY(mutex, old_mutex, new_mutex)\
    do{ \
        if(mutex == old_mutex) {\
            mutex = new_mutex;\
        }\
    } \
    while(0)


/**
 * \brief - push an entry top the to the sw state journal stack
 */
STATIC int dnxc_sw_state_journal_push(int unit, sw_state_journal_entry_t* entry)
{
    DNX_SW_STATE_INIT_FUNC_DEFS;

    if(NULL == entry) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL, "sw state journal ERROR: cannot push null entry into the journal.\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    entry->next = sw_state_journal[unit].head;

    sw_state_journal[unit].head = entry;
    sw_state_journal[unit].count++;

    DNX_SW_STATE_FUNC_RETURN;
}

/**
 * \brief - pop an entry from the top the sw state journal stack.
 */
STATIC int dnxc_sw_state_journal_pop(int unit)
{
    sw_state_journal_entry_t *entry = NULL;

    DNX_SW_STATE_INIT_FUNC_DEFS;

    if(NULL == sw_state_journal[unit].head) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL, "sw state journal ERROR: no entries found in journal to pop.\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    entry = sw_state_journal[unit].head;

    if(NULL == entry->next){
        sw_state_journal[unit].head = NULL;
        sw_state_journal[unit].count = 0;
    } else {
        sw_state_journal[unit].head = entry->next;
        sw_state_journal[unit].count--;
    }

    sal_free(entry);

    DNX_SW_STATE_FUNC_RETURN;
}

/**
 * \brief - peek an entry from the top the sw state journal stack.
 */
STATIC sw_state_journal_entry_t *dnxc_sw_state_journal_peek(int unit)
{
    if(NULL == sw_state_journal[unit].head){
        return NULL;
    }

    return sw_state_journal[unit].head;
}

/**
 * \brief - create a new sw state journal entry
 */
STATIC int dnxc_sw_state_journal_get_new_entry(int unit, uint32 payload_size, sw_state_journal_entry_t **ptr_entry)
{
    uint32 total_size = 0;
    sw_state_journal_entry_t *entry = NULL;

    DNX_SW_STATE_INIT_FUNC_DEFS;

    total_size = payload_size + sizeof(sw_state_journal_entry_t);

    /* check if we are going to exceeding the maximum size allowed */
    if((0 != sw_state_journal[unit].max_size)
      && (sw_state_journal[unit].max_size <
          (sw_state_journal[unit].cur_size + total_size)))
    {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL, "sw state journal ERROR: sw state journal exceeds to maximum size allowed .\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    entry = sal_alloc(total_size, "new sw state journal entry");
    if(NULL == entry) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_MEMORY, "sw state journal ERROR: failed to alloc entry's data.\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    sw_state_journal[unit].cur_size += total_size;

    *ptr_entry = entry;

    DNX_SW_STATE_FUNC_RETURN;
}

/**
 * \brief - inserts data in a newly created sw state journal entry
 */
STATIC int dnxc_sw_state_journal_insert_data_in_entry(int unit, uint32 size, uint8 *data, sw_state_journal_entry_t *entry)
{
    uint8 *entry_data_location = NULL;

    DNX_SW_STATE_INIT_FUNC_DEFS;

    /* validation of input */
    if(NULL == entry) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL, "sw state journal ERROR: cannot insert data to NULL entry.\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    if(NULL == data) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL, "sw state journal ERROR: cannot insert NULL data to entry.\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    entry_data_location = ((uint8 *)entry) + sizeof(sw_state_journal_entry_t);

    sal_memcpy(entry_data_location, data, size);

    DNX_SW_STATE_FUNC_RETURN;
}

/**
 * \brief - update all entry locations after 'free' entry has been roll-backed
 */
STATIC int dnxc_sw_state_journal_roll_back_update_entry_locations(int unit, uint8 *old_location, uint8 *new_location, uint32 size)
{
    sw_state_journal_entry_t *entry = NULL;

    DNX_SW_STATE_INIT_FUNC_DEFS;

    /* skip the first entry, because it is the current one */
    entry = sw_state_journal[unit].head;

    while(NULL != entry->next) {

        entry = entry->next;

        switch(entry->entry_type) {
            case SW_JOURNAL_ENTRY_MEMCPY:
            SW_STATE_JOURNAL_UPDATE_ENTRY_ADDRESS(entry->data.memcpy_data.ptr, old_location, new_location, size);
            break;
            case SW_JOURNAL_ENTRY_ALLOC:
            SW_STATE_JOURNAL_UPDATE_ENTRY_ADDRESS(entry->data.alloc_data.location, old_location, new_location, size);
            SW_STATE_JOURNAL_UPDATE_ENTRY_PTR_TO_ADDRESS(entry->data.alloc_data.ptr_location, old_location, new_location, size);
            break;
            case SW_JOURNAL_ENTRY_FREE:
            SW_STATE_JOURNAL_UPDATE_ENTRY_PTR_TO_ADDRESS(entry->data.free_data.ptr_location, old_location, new_location, size);
            break;
            case SW_JOURNAL_ENTRY_MUTEX_CREATE:
            SW_STATE_JOURNAL_UPDATE_MUTEX_ENTRY_PTR_TO_ADDRESS(entry->data.mutex_create.ptr_mtx, old_location, new_location, size);
            break;
            case SW_JOURNAL_ENTRY_MUTEX_DESTROY:
            SW_STATE_JOURNAL_UPDATE_MUTEX_ENTRY_PTR_TO_ADDRESS(entry->data.mutex_destroy.ptr_mtx, old_location, new_location, size);
            break;
            case SW_JOURNAL_ENTRY_MUTEX_TAKE:
            break;
            case SW_JOURNAL_ENTRY_MUTEX_GIVE:
            break;
            default:
            SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL, "sw state journal ERROR: Unknown entry type detected.\n%s%s%s", EMPTY, EMPTY, EMPTY);
        }
    }

    DNX_SW_STATE_FUNC_RETURN;
}

/**
 * \brief - update all entry locations after 'mutex_destroy' entry has been roll-backed
 */
STATIC int dnxc_sw_state_journal_roll_back_update_mutex_entry_locations(int unit, sal_mutex_t old_mutex, sal_mutex_t new_mutex)
{
    sw_state_journal_entry_t *entry = NULL;

    DNX_SW_STATE_INIT_FUNC_DEFS;

    /* skip the first entry, because it is the current one */
    entry = sw_state_journal[unit].head;

    while(NULL != entry->next) {

        entry = entry->next;

        switch(entry->entry_type) {
            case SW_JOURNAL_ENTRY_MUTEX_CREATE:
            SW_STATE_JOURNAL_UPDATE_MUTEX_ENTRY(entry->data.mutex_create.mtx, old_mutex, new_mutex);
            SW_STATE_JOURNAL_UPDATE_MUTEX_ENTRY(*(entry->data.mutex_create.ptr_mtx), old_mutex, new_mutex);
            break;
            case SW_JOURNAL_ENTRY_MUTEX_TAKE:
            SW_STATE_JOURNAL_UPDATE_MUTEX_ENTRY(entry->data.mutex_take.mtx, old_mutex, new_mutex);
            break;
            case SW_JOURNAL_ENTRY_MUTEX_GIVE:
            SW_STATE_JOURNAL_UPDATE_MUTEX_ENTRY(entry->data.mutex_give.mtx, old_mutex, new_mutex);
            break;
            default:
            break;
        }
    }

    DNX_SW_STATE_FUNC_RETURN;
}


/**
 * \brief - roll-back a 'free' entry
 */
STATIC int dnxc_sw_state_journal_roll_back_FREE_entry(int unit, sw_state_journal_entry_t *entry)
{
    uint8 *entry_data_location = NULL;
    uint32 payload_size = entry->element_size * entry->nof_elements;

    DNX_SW_STATE_INIT_FUNC_DEFS;

    if(NULL == entry->data.free_data.location || NULL == entry->data.free_data.ptr_location) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL, "sw state journal ERROR: invalid entry data detected.\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    DNX_SW_STATE_ALLOC_BASIC(unit, entry->module_id, entry->data.free_data.ptr_location, entry->element_size, entry->nof_elements, DNX_SW_STATE_JOURNAL_ROLLING_BACK, "sw state journal roll-back free entry");

    entry_data_location = ((uint8 *)entry) + sizeof(sw_state_journal_entry_t);

    DNX_SW_STATE_MEMCPY_BASIC(unit, entry->module_id, *(entry->data.free_data.ptr_location), entry_data_location, payload_size, DNX_SW_STATE_JOURNAL_ROLLING_BACK, "sw state roll back free entry");

    /* go down the stack and update all entries that need to be updated */
    SHR_IF_ERR_EXIT(dnxc_sw_state_journal_roll_back_update_entry_locations(unit, entry->data.free_data.location, *(entry->data.free_data.ptr_location), payload_size));

    DNX_SW_STATE_FUNC_RETURN;
}

/**
 * \brief - roll-back an 'alloc' entry
 */
STATIC int dnxc_sw_state_journal_roll_back_ALLOC_entry(int unit, sw_state_journal_entry_t *entry)
{
    DNX_SW_STATE_INIT_FUNC_DEFS;

    if(NULL == entry->data.alloc_data.location || NULL == entry->data.alloc_data.ptr_location) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL, "sw state journal ERROR: invalid entry data detected.\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    /* FREE data and set pointer to NULL. Disable journaling */
    DNX_SW_STATE_FREE_BASIC(unit, entry->module_id, entry->data.alloc_data.ptr_location, DNX_SW_STATE_JOURNAL_ROLLING_BACK, "journal rollback alloc entry");

    *(entry->data.alloc_data.ptr_location) = NULL;

    DNX_SW_STATE_FUNC_RETURN;
}

/**
 * \brief - roll-back a 'memcpy' entry
 */
STATIC int dnxc_sw_state_journal_roll_back_MEMCPY_entry(int unit, sw_state_journal_entry_t *entry)
{
    uint8 *entry_data_location = NULL;
    uint32 payload_size = entry->nof_elements * entry->element_size;

    DNX_SW_STATE_INIT_FUNC_DEFS;

    entry_data_location = ((uint8 *)entry) + sizeof(sw_state_journal_entry_t);

    if(NULL == entry->data.memcpy_data.ptr || NULL == entry_data_location) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL, "sw state journal ERROR: invalid entry data detected.\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    DNX_SW_STATE_MEMCPY_BASIC(unit, entry->module_id, entry->data.memcpy_data.ptr, entry_data_location, payload_size, DNX_SW_STATE_JOURNAL_ROLLING_BACK, "sw state journal roll back memcpy");

    DNX_SW_STATE_FUNC_RETURN;
}

/**
 * \brief - roll-back an 'mutex_create' entry
 */
STATIC int dnxc_sw_state_journal_roll_back_MUTEX_CREATE_entry(int unit, sw_state_journal_entry_t *entry)
{
    uint32 module_id = 0;

    DNX_SW_STATE_INIT_FUNC_DEFS;

    if(NULL == entry->data.mutex_create.ptr_mtx) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL, "sw state journal ERROR: invalid entry data detected.\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    module_id = entry->module_id;

    SHR_IF_ERR_EXIT(dnxc_sw_state_dispatcher[unit][module_id].mutex_destroy(unit, module_id, entry->data.mutex_create.ptr_mtx, DNX_SW_STATE_JOURNAL_ROLLING_BACK)); 

    DNX_SW_STATE_FUNC_RETURN;
}

/**
 * \brief - roll-back an 'mutex_destroy' entry
 */
STATIC int dnxc_sw_state_journal_roll_back_MUTEX_DESTROY_entry(int unit, sw_state_journal_entry_t *entry)
{
    uint32 module_id = 0;
    char *desc = "journal_mutex_roll_back";
    DNX_SW_STATE_INIT_FUNC_DEFS;

    if(NULL == entry->data.mutex_destroy.ptr_mtx || NULL == entry->data.mutex_destroy.mtx) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL, "sw state journal ERROR: invalid entry data detected.\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    module_id = entry->module_id;

    /* re-create mutex */
    SHR_IF_ERR_EXIT(dnxc_sw_state_dispatcher[unit][module_id].mutex_create(unit, module_id, entry->data.mutex_destroy.ptr_mtx, desc, DNX_SW_STATE_JOURNAL_ROLLING_BACK)); 

    /* update mutex journal entries */
    SHR_IF_ERR_EXIT(dnxc_sw_state_journal_roll_back_update_mutex_entry_locations(unit, entry->data.mutex_destroy.mtx, *(entry->data.mutex_destroy.ptr_mtx)));

    DNX_SW_STATE_FUNC_RETURN;
}

/**
 * \brief - roll-back an 'mutex_take' entry
 */
STATIC int dnxc_sw_state_journal_roll_back_MUTEX_TAKE_entry(int unit, sw_state_journal_entry_t *entry)
{
    uint32 module_id = 0;

    DNX_SW_STATE_INIT_FUNC_DEFS;

    if(NULL == entry->data.mutex_take.mtx) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL, "sw state journal ERROR: invalid entry data detected.\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    module_id = entry->module_id;

    SHR_IF_ERR_EXIT(dnxc_sw_state_dispatcher[unit][module_id].mutex_give(unit, module_id, entry->data.mutex_take.mtx, DNX_SW_STATE_JOURNAL_ROLLING_BACK));

    DNX_SW_STATE_FUNC_RETURN;
}

/**
 * \brief - roll-back an 'mutex_give' entry
 */
STATIC int dnxc_sw_state_journal_roll_back_MUTEX_GIVE_entry(int unit, sw_state_journal_entry_t *entry)
{
    uint32 usec = 0;
    uint32 module_id = 0;
    DNX_SW_STATE_INIT_FUNC_DEFS;

    if(NULL == entry->data.mutex_give.mtx) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL, "sw state journal ERROR: invalid entry data detected.\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    module_id = entry->module_id;
    usec = DNX_SW_STATE_JOURNAL_MUTEX_DEFAULT_USEC;

    SHR_IF_ERR_EXIT(dnxc_sw_state_dispatcher[unit][module_id].mutex_take(unit, module_id, entry->data.mutex_give.mtx, usec, DNX_SW_STATE_JOURNAL_ROLLING_BACK));

    DNX_SW_STATE_FUNC_RETURN;
}

/*
 * see .h file for description
 */
int dnxc_sw_state_journal_start(int unit)
{
    DNX_SW_STATE_INIT_FUNC_DEFS;

    sw_state_journal[unit].head = NULL;
    sw_state_journal[unit].count = 0;
    sw_state_journal[unit].max_size = DNX_SWSTATE_JOURNAL_MAX_SIZE;
    sw_state_journal[unit].cur_size = 0;

    DNX_SW_STATE_FUNC_RETURN;
}

/*
 * see .h file for description
 */
int dnxc_sw_state_journal_roll_back(int unit)
{
    sw_state_journal_entry_t *entry = NULL;

    DNX_SW_STATE_INIT_FUNC_DEFS;

    DNX_SWSTATE_JOURNAL_ENSURE_ON(unit);

    while(sw_state_journal[unit].count > 0) {

        /* get the first entry from the stack */
        entry = dnxc_sw_state_journal_peek(unit);

        /* integrity of the journal breached if entry is NULL */
        if(NULL == entry) {
            SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL, "sw state journal ERROR: NULL entry retrieved from journal, but one or more entries present in it.\n%s%s%s", EMPTY, EMPTY, EMPTY);
        }

        switch(entry->entry_type) {
            case SW_JOURNAL_ENTRY_MEMCPY:
            SHR_IF_ERR_EXIT(dnxc_sw_state_journal_roll_back_MEMCPY_entry(unit, entry));
            break;
            case SW_JOURNAL_ENTRY_ALLOC:
            SHR_IF_ERR_EXIT(dnxc_sw_state_journal_roll_back_ALLOC_entry(unit, entry));
            break;
            case SW_JOURNAL_ENTRY_FREE:
            SHR_IF_ERR_EXIT(dnxc_sw_state_journal_roll_back_FREE_entry(unit, entry));
            break;
            case SW_JOURNAL_ENTRY_MUTEX_CREATE:
            SHR_IF_ERR_EXIT(dnxc_sw_state_journal_roll_back_MUTEX_CREATE_entry(unit,entry));
            break;
            case SW_JOURNAL_ENTRY_MUTEX_DESTROY:
            SHR_IF_ERR_EXIT(dnxc_sw_state_journal_roll_back_MUTEX_DESTROY_entry(unit,entry));
            break;
            case SW_JOURNAL_ENTRY_MUTEX_TAKE:
            SHR_IF_ERR_EXIT(dnxc_sw_state_journal_roll_back_MUTEX_TAKE_entry(unit,entry));
            break;
            case SW_JOURNAL_ENTRY_MUTEX_GIVE:
            SHR_IF_ERR_EXIT(dnxc_sw_state_journal_roll_back_MUTEX_GIVE_entry(unit,entry));
            break;
            default:
            SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL, "sw state journal ERROR: Unknown entry type detected.\n%s%s%s", EMPTY, EMPTY, EMPTY);
        }

        /* pop entry from the journal stack */
        SHR_IF_ERR_EXIT(dnxc_sw_state_journal_pop(unit));
    }

    DNX_SW_STATE_FUNC_RETURN;
}

/*
 * see .h file for description
 */
int dnxc_sw_state_journal_clear(int unit)
{
    DNX_SW_STATE_INIT_FUNC_DEFS;

    DNX_SWSTATE_JOURNAL_ENSURE_ON(unit);

    /* pop all of the entries in the journal */
    while(sw_state_journal[unit].count > 0) {
        SHR_IF_ERR_EXIT(dnxc_sw_state_journal_pop(unit));
    }

    DNX_SW_STATE_FUNC_RETURN;
}

/*
 * see .h file for description
 */
int dnxc_sw_state_journal_log_free(int unit, uint32 module_id, uint32 nof_elements, uint32 element_size, uint8 **ptr)
{
    sw_state_journal_entry_t *entry = NULL;
    uint32 size = nof_elements * element_size;

    DNX_SW_STATE_INIT_FUNC_DEFS;
    if(unit < 0)
    {
        return SOC_E_NONE;
    }

    DNX_SWSTATE_JOURNAL_ENSURE_ON(unit);

    /* validate input */
    if(NULL == ptr || NULL == *ptr){
         SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL, "sw state journal ERROR: NULL data attempted to be inserted to journal.\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    /* get new FREE entry */
    SHR_IF_ERR_EXIT(dnxc_sw_state_journal_get_new_entry(unit, size, &entry));

    /* fill new non-null ALLOC entry with data */
    entry->next = NULL;
    entry->entry_type = SW_JOURNAL_ENTRY_FREE;
    entry->nof_elements = nof_elements;
    entry->element_size = element_size;
    entry->module_id = module_id;
    entry->data.free_data.location = *ptr;
    entry->data.free_data.ptr_location = ptr;
    if(dnxc_sw_state_journal_insert_data_in_entry(unit, size, *ptr, entry) != SOC_E_NONE) {
        sal_free(entry);
        return SOC_E_INTERNAL;
    }

    /* push entry to journal */
    SHR_IF_ERR_EXIT(dnxc_sw_state_journal_push(unit, entry));

    DNX_SW_STATE_FUNC_RETURN;
}

/*
 * see .h file for description
 */
int dnxc_sw_state_journal_log_alloc(int unit, uint32 module_id, uint8 **ptr)
{
    sw_state_journal_entry_t *entry = NULL;

    DNX_SW_STATE_INIT_FUNC_DEFS;

    DNX_SWSTATE_JOURNAL_ENSURE_ON(unit);

    /* validate input */
    if(NULL == ptr || NULL == *ptr){
         SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL, "sw state journal ERROR: NULL data attempted to be inserted to journal.\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    /* get new ALLOC entry, alloc entries carry no content data */
    SHR_IF_ERR_EXIT(dnxc_sw_state_journal_get_new_entry(unit, 0, &entry));

    /* fill new non-null ALLOC entry with data */
    entry->next = NULL;
    entry->entry_type = SW_JOURNAL_ENTRY_ALLOC;
    entry->nof_elements = 0;
    entry->element_size = 0;
    entry->module_id = module_id;
    entry->data.alloc_data.location = *ptr;
    entry->data.alloc_data.ptr_location = ptr;

    /* push entry to journal */
    SHR_IF_ERR_EXIT(dnxc_sw_state_journal_push(unit, entry));

    DNX_SW_STATE_FUNC_RETURN;
}

/*
 * see .h file for description
 */
int dnxc_sw_state_journal_log_memcpy(int unit, uint32 module_id, uint32 size, uint8 *ptr)
{
    sw_state_journal_entry_t *entry = NULL;

    DNX_SW_STATE_INIT_FUNC_DEFS;

    DNX_SWSTATE_JOURNAL_ENSURE_ON(unit);

    /* validate input */
    if(NULL == ptr){
         SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL, "sw state journal ERROR: NULL data attempted to be inserted to journal.\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    if(0 == size){
         SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL, "sw state journal ERROR: data with size zero attempted to be inserted to journal.\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    /* get new MEMCPY entry if journaling is enabled */
    SHR_IF_ERR_EXIT(dnxc_sw_state_journal_get_new_entry(unit, size, &entry));

    /* fill new non-null MEMCPY entry with data */
    entry->next = NULL;
    entry->entry_type = SW_JOURNAL_ENTRY_MEMCPY;
    entry->nof_elements = 1;
    entry->element_size = size;
    entry->module_id = module_id;
    entry->data.memcpy_data.ptr = ptr;
    if(dnxc_sw_state_journal_insert_data_in_entry(unit, size, ptr, entry) != SOC_E_NONE) {
        sal_free(entry);
        return SOC_E_INTERNAL;
    }

    /* push entry to journal */
    SHR_IF_ERR_EXIT(dnxc_sw_state_journal_push(unit, entry));

    DNX_SW_STATE_FUNC_RETURN;
}

/*
 * see .h file for description
 */
int dnxc_sw_state_journal_mutex_create(int unit, uint32 module_id, sal_mutex_t *ptr_mtx)
{
    sw_state_journal_entry_t *entry = NULL;
    uint32 size = 0;

    DNX_SW_STATE_INIT_FUNC_DEFS;

    DNX_SWSTATE_JOURNAL_ENSURE_ON(unit);

    if(NULL == ptr_mtx){
         SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL, "sw state journal ERROR: NULL data attempted to be inserted to journal.\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    SHR_IF_ERR_EXIT(dnxc_sw_state_journal_get_new_entry(unit, size, &entry));

    entry->next = NULL;
    entry->entry_type = SW_JOURNAL_ENTRY_MUTEX_CREATE;
    entry->nof_elements = 0;
    entry->element_size = 0;
    entry->module_id = module_id;
    entry->data.mutex_create.ptr_mtx = ptr_mtx;
    entry->data.mutex_create.mtx = *ptr_mtx;

    SHR_IF_ERR_EXIT(dnxc_sw_state_journal_push(unit, entry));

    DNX_SW_STATE_FUNC_RETURN;
}

/*
 * see .h file for description
 */
int dnxc_sw_state_journal_mutex_destroy(int unit, uint32 module_id, sal_mutex_t *ptr_mtx)
{
    sw_state_journal_entry_t *entry = NULL;
    uint32 size = 0;

    DNX_SW_STATE_INIT_FUNC_DEFS;

    DNX_SWSTATE_JOURNAL_ENSURE_ON(unit);

    if(NULL == ptr_mtx){
         SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL, "sw state journal ERROR: NULL data attempted to be inserted to journal.\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    size = sizeof(sal_mutex_t);

    SHR_IF_ERR_EXIT(dnxc_sw_state_journal_get_new_entry(unit, size, &entry));

    entry->next = NULL;
    entry->entry_type = SW_JOURNAL_ENTRY_MUTEX_DESTROY;
    entry->nof_elements = 0;
    entry->element_size = 0;
    entry->module_id = module_id;
    entry->data.mutex_destroy.ptr_mtx = ptr_mtx;
    entry->data.mutex_destroy.mtx = *ptr_mtx;

    if(dnxc_sw_state_journal_insert_data_in_entry(unit, size, (uint8 *)(*ptr_mtx), entry) != SOC_E_NONE) {
        sal_free(entry);
        return SOC_E_INTERNAL;
    }

    SHR_IF_ERR_EXIT(dnxc_sw_state_journal_push(unit, entry));

    DNX_SW_STATE_FUNC_RETURN;
}

/*
 * see .h file for description
 */
int dnxc_sw_state_journal_mutex_take(int unit, uint32 module_id, sal_mutex_t mtx, uint32 usec)
{
    sw_state_journal_entry_t *entry = NULL;
    uint32 size = 0;

    DNX_SW_STATE_INIT_FUNC_DEFS;

    DNX_SWSTATE_JOURNAL_ENSURE_ON(unit);

    SHR_IF_ERR_EXIT(dnxc_sw_state_journal_get_new_entry(unit, size, &entry));

    entry->next = NULL;
    entry->entry_type = SW_JOURNAL_ENTRY_MUTEX_TAKE;
    entry->nof_elements = 0;
    entry->element_size = 0;
    entry->module_id = module_id;
    entry->data.mutex_take.mtx = mtx;
    entry->data.mutex_take.usec = usec;

    SHR_IF_ERR_EXIT(dnxc_sw_state_journal_push(unit, entry));

    DNX_SW_STATE_FUNC_RETURN;
}

/*
 * see .h file for description
 */
int dnxc_sw_state_journal_mutex_give(int unit, uint32 module_id, sal_mutex_t mtx)
{
    sw_state_journal_entry_t *entry = NULL;
    uint32 size = 0;

    DNX_SW_STATE_INIT_FUNC_DEFS;

    DNX_SWSTATE_JOURNAL_ENSURE_ON(unit);

    SHR_IF_ERR_EXIT(dnxc_sw_state_journal_get_new_entry(unit, size, &entry));

    entry->next = NULL;
    entry->entry_type = SW_JOURNAL_ENTRY_MUTEX_GIVE;
    entry->nof_elements = 0;
    entry->element_size = 0;
    entry->module_id = module_id;
    entry->data.mutex_give.mtx = mtx;

    SHR_IF_ERR_EXIT(dnxc_sw_state_journal_push(unit, entry));

    DNX_SW_STATE_FUNC_RETURN;
}



#undef _ERR_MSG_MODULE_NAME
