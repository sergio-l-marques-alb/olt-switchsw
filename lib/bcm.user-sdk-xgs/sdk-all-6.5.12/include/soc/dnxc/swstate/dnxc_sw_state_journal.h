/*
 * $Id: $
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * INFO: This module is one of the components needed for Error Recovery,
 * it is a rollback journal, saving the old values that were overridden
 * by sw_state access calls since the beginning of the last transaction.
 */

#ifndef _DNX_SW_STATE_JOURNAL_H
/* { */
#define _DNX_SW_STATE_JOURNAL_H

#include <soc/types.h>
#include <sal/core/alloc.h>
#include <soc/error.h>
#include <assert.h>
#include <sal/core/thread.h>

/**
 * \brief - describes differene sw state journal entry trypes
 */
typedef enum sw_state_journal_entry_e
{
    SW_JOURNAL_ENTRY_MEMCPY = 0,
    SW_JOURNAL_ENTRY_ALLOC,
    SW_JOURNAL_ENTRY_FREE,
    SW_JOURNAL_ENTRY_MUTEX_CREATE,
    SW_JOURNAL_ENTRY_MUTEX_DESTROY,
    SW_JOURNAL_ENTRY_MUTEX_TAKE,
    SW_JOURNAL_ENTRY_MUTEX_GIVE
} sw_state_journal_entry_type_t;

/**
 * \brief - sw state journal 'memcpy' entry structure
 */
typedef struct sw_state_journal_memcpy_entry_d
{
    uint8 *ptr;
} sw_state_journal_memcpy_entry_t;

/**
 * \brief - sw state journal 'alloc' entry structure
 */
typedef struct sw_state_journal_alloc_entry_d
{
    uint8 *location;
    uint8 **ptr_location;
} sw_state_journal_alloc_entry_t;

/**
 * \brief - sw state journal 'free' entry structure
 */
typedef struct sw_state_journal_free_entry_d
{
    uint8 *location;
    uint8 **ptr_location;
} sw_state_journal_free_entry_t;

/**
 * \brief - sw state journal 'mutex_create' entry structure
 */
typedef struct sw_state_journal_mutex_create_entry_d
{
    sal_mutex_t mtx;
    sal_mutex_t *ptr_mtx;
} sw_state_journal_mutex_create_entry_t;

/**
 * \brief - sw state journal 'mutex_destroy' entry structure
 */
typedef struct sw_state_journal_mutex_destroy_entry_d
{
    sal_mutex_t mtx;
    sal_mutex_t *ptr_mtx;
} sw_state_journal_mutex_destroy_entry_t;

/**
 * \brief - sw state journal 'mutex_take' entry structure
 */
typedef struct sw_state_journal_mutex_take_entry_d
{
    sal_mutex_t mtx;
    uint32 usec;
} sw_state_journal_mutex_take_entry_t;

/**
 * \brief - sw state journal 'mutex_take' entry structure
 */
typedef struct sw_state_journal_mutex_give_entry_d
{
    sal_mutex_t mtx;
} sw_state_journal_mutex_give_entry_t;

/**
 * \brief - sw state journal entry data is union of all entry types
 */
typedef union sw_state_journal_entry_data_d
{
    sw_state_journal_memcpy_entry_t memcpy_data;
    sw_state_journal_alloc_entry_t alloc_data;
    sw_state_journal_free_entry_t free_data;
    sw_state_journal_mutex_create_entry_t mutex_create;
    sw_state_journal_mutex_destroy_entry_t mutex_destroy;
    sw_state_journal_mutex_take_entry_t mutex_take;
    sw_state_journal_mutex_give_entry_t mutex_give;
} sw_state_journal_entry_data_t;

/**
 * \brief - sw state journal entry structure
 */
typedef struct sw_state_journal_entry_d
{
    struct sw_state_journal_entry_d *next;
    sw_state_journal_entry_type_t entry_type;
    uint32 nof_elements;
    uint32 element_size;
    uint32 module_id;
    sw_state_journal_entry_data_t data;
} sw_state_journal_entry_t;

/**
 * \brief - sw state journal main structure
 */
typedef struct sw_state_journal_d
{
    sw_state_journal_entry_t *head;
    uint32 count;
    uint32 cur_size;
    uint32 max_size;
} sw_state_journal_t;

/**
 * \brief
 *  The function will activate the sw state journal
 * \param [in] unit    - Device Id 
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_sw_state_journal_start(
    int unit);

/**
 * \brief
 *  The function will roll-back the current sw state journal transaction
 * \param [in] unit - Device Id 
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_sw_state_journal_roll_back(
    int unit);

/**
 * \brief
 *  The function will clear and deactivate the sw state journal
 * \param [in] unit - Device Id 
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_sw_state_journal_clear(
    int unit);

/**
 * \brief
 *  The function will log a 'free' entry in the sw state journal
 * \param [in] unit - Device Id 
 * \param [in] module_id - module id
 * \param [in] nof_elements - Number of elements
 * \param [in] element_size - Element size in bytes
 * \param [in] ptr - The pointer to the location that holds the pointer to the data to be freed.
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_sw_state_journal_log_free(
    int unit,
    uint32 module_id,
    uint32 nof_elements,
    uint32 element_size,
    uint8 **ptr);

/**
 * \brief
 *  The function will log a 'alloc' entry in the sw state journal
 * \param [in] unit - Device Id 
 * \param [in] module_id - module id
 * \param [in] ptr - The pointer to the location that holds the pointer to the data that was allocated
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_sw_state_journal_log_alloc(
    int unit,
    uint32 module_id,
    uint8 **ptr);

/**
 * \brief
 *  The function will log a 'alloc' entry in the sw state journal
 * \param [in] unit - Device Id 
 * \param [in] module_id - module id
 * \param [in] size - The size of the of the data to be inserted into the log
 * \param [in] ptr - The pointer to the data to be inserted into the log
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_sw_state_journal_log_memcpy(
    int unit,
    uint32 module_id,
    uint32 size,
    uint8 *ptr);

/**
 * \brief
 *  The function will log a 'mutex_create' entry in the sw state journal
 * \param [in] unit - Device Id 
 * \param [in] module_id - Module id
 * \param [in] ptr_mtx - The pointer to the location that hold the mutex data
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_sw_state_journal_mutex_create(
    int unit,
    uint32 module_id,
    sal_mutex_t * ptr_mtx);

/**
 * \brief
 *  The function will log a 'mutex_destroy' entry in the sw state journal
 * \param [in] unit - Device Id 
 * \param [in] module_id - Module id
 * \param [in] ptr_mtx - The pointer to the location that hold the mutex data
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_sw_state_journal_mutex_destroy(
    int unit,
    uint32 module_id,
    sal_mutex_t * ptr_mtx);

/**
 * \brief
 *  The function will log a 'mutex_take' entry in the sw state journal
 * \param [in] unit - Device Id 
 * \param [in] module_id - Module id
 * \param [in] mtx - Mutex
 * \param [in] usec - Mutex usec
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_sw_state_journal_mutex_take(
    int unit,
    uint32 module_id,
    sal_mutex_t mtx,
    uint32 usec);

/**
 * \brief
 *  The function will log a 'mutex_give' entry in the sw state journal
 * \param [in] unit - Device Id 
 * \param [in] module_id - Module id
 * \param [in] mtx - Mutex
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_sw_state_journal_mutex_give(
    int unit,
    uint32 module_id,
    sal_mutex_t mtx);

/* } */
#endif /* _DNX_SW_STATE_JOURNAL_H */
