/*
 * $Id: ? $
 * $Copyright: Copyright 2015 Broadcom Corporation.
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
#include <shared/swstate/sw_state.h>
#include <shared/swstate/sw_state_hash_tbl.h>
#include <shared/swstate/sw_state_sync_db.h>

#include <soc/types.h>
#include <soc/error.h>
#include <soc/scache.h>

/**************************** layout **************************/
/* header/ptr-list/data/transaction-ptr-list/transaction-data */
/**************************************************************/



#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_LS_SOC_INIT

#define SHR_SW_STATE_NOF_COPIES (2) /*original and to_be_commited*/
#define PERSISTANT_COPY (0x1)
#define SAND_BOX_COPY   (0x0)
#define SHR_SW_STATE_FLAG_TRANSACTION_SUPPORT (0x1)

#define TRANSACTION_STATUS_IDLE                 (0x0)
#define TRANSACTION_STATUS_LOGGING_STARTED      (0x1)
#define TRANSACTION_STATUS_LOGGING_ENDED        (0x2)
#define TRANSACTION_STATUS_COMMIT_PREP_STARTED  (0x3)
#define TRANSACTION_STATUS_COMMIT_PREP_ENDED    (0x4)
#define TRANSACTION_STATUS_COMMIT_STARTED       (0x5)
#define TRANSACTION_STATUS_COMMIT_ENDED         (TRANSACTION_STATUS_IDLE)



typedef struct shr_sw_state_alloc_element_s {
    uint32 ptr_offset;
    uint8 *ptr_value;
} shr_sw_state_alloc_element_t;

typedef struct shr_sw_state_data_block_header_s {
    uint32 total_buffer_size;
    shr_sw_state_alloc_element_t *ptr_offsets_list;
    uint32 ptr_offsets_len;
    uint32 next_free_offset_list_idx;
    uint8 *data_ptr;
    uint32 data_size;
    uint8 *next_free_data_slot;
    uint32 size_left;
} shr_sw_state_data_block_header_t;

typedef struct shr_sw_state_operation_mode_s {
    uint8 is_init;
    uint8 is_transaction_support;
    uint8 is_crash_recovery;
    uint8 is_error_recovery;
} shr_sw_state_operation_mode_t;

typedef struct shr_sw_state_backup_node_s {
    char   label[256];
    shr_sw_state_data_block_header_t *header;
    struct shr_sw_state_backup_node_s *next;
} shr_sw_state_backup_node_t;



#if 0 /*ISSU*/
shr_sw_state_layout_node_t                 *layout_tree[SOC_MAX_NUM_DEVICES];
#endif

shr_sw_state_operation_mode_t               shr_sw_state_operation_mode[SOC_MAX_NUM_DEVICES];
shr_sw_state_data_block_header_t           *shr_sw_state_data_block_header[SOC_MAX_NUM_DEVICES];
shr_sw_state_backup_node_t                 *shr_sw_state_backup_list[SOC_MAX_NUM_DEVICES];

/* functions for manipulating sw state backup copies */
int shr_sw_state_copy_load(int unit, char *label);
int shr_sw_state_copy_free(int unit, char *label);
int shr_sw_state_copy_import(int unit, char *label, char *filename);
int shr_sw_state_copy_export(int unit, char *label, char *filename);

shr_sw_state_t *sw_state[BCM_MAX_NUM_UNITS];

void 
_sw_state_access_debug_hook(int id) {
    /* 
     * this function is used for debug purposes
     * use breakepoint on this function inorder to easily catch sw_state errors 
     * use id value for conditioned breakpoints 
     * 0 - 
     * 1 - 
     * 2 - 
     * 3 - 
     * 4 -  
     */
    return;
}

int
shr_sw_state_sync(int unit)
{
    soc_scache_handle_t                wb_handle_orig = 0;

    SHR_SW_STATE_SCACHE_HANDLE_SET(wb_handle_orig, unit, SHR_SW_STATE_SCACHE_HANDLE_DATA_BLOCK);

    
    return shr_sw_state_scache_sync(unit, wb_handle_orig, 0, shr_sw_state_data_block_header[unit]->total_buffer_size);
}

#if 0
STATIC int
shr_sw_state_transaction_sync(int unit)
{
    soc_scache_handle_t                wb_handle_orig = 0;
    int offset;
    int size;

    SOC_INIT_FUNC_DEFS;

    SHR_SW_STATE_SCACHE_HANDLE_SET(wb_handle_orig, unit, SHR_SW_STATE_SCACHE_HANDLE_DATA_BLOCK);

    /* sync the actual data */
    offset = (uint8 *) shr_sw_state_data_block_header[unit]->transaction_data.ptr_offsets_list -
             (uint8 *) shr_sw_state_data_block_header[unit];

    size = shr_sw_state_data_block_header[unit]->data_size +
           (shr_sw_state_data_block_header[unit]->ptr_offsets_len * sizeof(shr_sw_state_alloc_element_t));

    _SOC_IF_ERR_EXIT(shr_sw_state_scache_sync(unit, wb_handle_orig, offset, size));

    /* sync the transaction's structure */
    offset = ((uint8 *) &(shr_sw_state_data_block_header[unit]->transaction_data)) -
             (uint8 *) shr_sw_state_data_block_header[unit];

    size = sizeof(shr_sw_state_transaction_data_t);

    _SOC_IF_ERR_EXIT(shr_sw_state_scache_sync(unit, wb_handle_orig, offset, size));

    SOC_EXIT;
exit:
    SOC_FUNC_RETURN;
}

STATIC int
shr_sw_state_transaction_status_sync(int unit)
{
    soc_scache_handle_t                wb_handle_orig = 0;
    int offset;
    int size;

    SOC_INIT_FUNC_DEFS;

    SHR_SW_STATE_SCACHE_HANDLE_SET(wb_handle_orig, unit, SHR_SW_STATE_SCACHE_HANDLE_DATA_BLOCK);

    /* sync the transaction's status */
    offset = ((uint8 *) &(shr_sw_state_data_block_header[unit]->transaction_data.status)) -
             (uint8 *) shr_sw_state_data_block_header[unit];

    size = sizeof(uint8);

    _SOC_IF_ERR_EXIT(shr_sw_state_scache_sync(unit, wb_handle_orig, offset, size));

    SOC_EXIT;
exit:
    SOC_FUNC_RETURN;
}

int shr_sw_state_transaction_start(int unit){
    SOC_INIT_FUNC_DEFS;

    if (shr_sw_state_operation_mode[unit].is_transaction_support != 0x1) {
        /* nothing to do (error?)*/ 
        SOC_EXIT;
    }

    if (shr_sw_state_data_block_header[unit]->transaction_data.status != TRANSACTION_STATUS_IDLE) {
        _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL,
                   (BSL_META_U(unit,
                      "Unit:%d trying to start a new transaction before the "
                      "previous transaction is fully commited\n"), unit));
    }
    shr_sw_state_data_block_header[unit]->transaction_data.status = TRANSACTION_STATUS_LOGGING_STARTED;

    SOC_EXIT;
exit:
    SOC_FUNC_RETURN;
}


int shr_sw_state_transaction_end(int unit){
    SOC_INIT_FUNC_DEFS;

    if (shr_sw_state_operation_mode[unit].is_transaction_support != 0x1) {
        /* nothing to do (error?)*/ 
        SOC_EXIT;
    }

    if (shr_sw_state_data_block_header[unit]->transaction_data.status != TRANSACTION_STATUS_LOGGING_STARTED) {
        _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL,
                   (BSL_META_U(unit,
                      "Unit:%d trying to end a transaction although there is no open transaction\n"), unit));
    }
    shr_sw_state_data_block_header[unit]->transaction_data.status = TRANSACTION_STATUS_LOGGING_ENDED;

    SOC_EXIT;
exit:
    SOC_FUNC_RETURN;
}

int shr_sw_state_transaction_prepare_for_commit(int unit){
    SOC_INIT_FUNC_DEFS;

    if (shr_sw_state_operation_mode[unit].is_transaction_support != 0x1) {
        /* nothing to do (error?)*/ 
        SOC_EXIT;
    }

    if (shr_sw_state_data_block_header[unit]->transaction_data.status != TRANSACTION_STATUS_LOGGING_ENDED) {
        _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL,
                   (BSL_META_U(unit,
                      "Unit:%d trying to prepare for commit a transaction that is still open\n"), unit));
    }
    shr_sw_state_data_block_header[unit]->transaction_data.status = TRANSACTION_STATUS_COMMIT_PREP_STARTED;

    /* save the transaction's data */
    sal_memcpy(shr_sw_state_data_block_header[unit]->transaction_data.data_ptr,
               shr_sw_state_data_block_header[unit]->data_ptr,
               shr_sw_state_data_block_header[unit]->data_size);
    /* save the transaction's pointer list */
    sal_memcpy(shr_sw_state_data_block_header[unit]->transaction_data.ptr_offsets_list,
               shr_sw_state_data_block_header[unit]->ptr_offsets_list,
               (shr_sw_state_data_block_header[unit]->ptr_offsets_len * sizeof(shr_sw_state_alloc_element_t)));

    /* real next_free_data_slot (the one from the real header) */
    shr_sw_state_data_block_header[unit]->transaction_data.next_free_data_slot = 
        shr_sw_state_data_block_header[unit]->next_free_data_slot;

    /* real next_free_offset_list_idx (the one from the real header) */
    shr_sw_state_data_block_header[unit]->transaction_data.next_free_offset_list_idx = 
        shr_sw_state_data_block_header[unit]->next_free_offset_list_idx;

    /* real size_left (the one from the real header) */
    shr_sw_state_data_block_header[unit]->transaction_data.size_left = 
        shr_sw_state_data_block_header[unit]->size_left;

    shr_sw_state_data_block_header[unit]->transaction_data.status = TRANSACTION_STATUS_COMMIT_PREP_ENDED;

    _SOC_IF_ERR_EXIT(shr_sw_state_transaction_sync(unit));

    SOC_EXIT;
exit:
    SOC_FUNC_RETURN;
}

int shr_sw_state_transaction_commit(int unit){
    SOC_INIT_FUNC_DEFS;

    if (shr_sw_state_operation_mode[unit].is_transaction_support != 0x1) {
        /* nothing to do (error?)*/ 
        SOC_EXIT;
    }

    if (shr_sw_state_data_block_header[unit]->transaction_data.status != TRANSACTION_STATUS_COMMIT_PREP_ENDED) {
        _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL,
                   (BSL_META_U(unit,
                      "Unit:%d trying to commit a transaction that wasn't properly prepared for commit\n"), unit));
    }

    /* sync the entire sw_state */
    shr_sw_state_data_block_header[unit]->transaction_data.status = TRANSACTION_STATUS_COMMIT_STARTED;
    _SOC_IF_ERR_EXIT(shr_sw_state_sync(unit));

    /* mark transaction as done and sync the new status */
    shr_sw_state_data_block_header[unit]->transaction_data.status = TRANSACTION_STATUS_COMMIT_ENDED;
    _SOC_IF_ERR_EXIT(shr_sw_state_transaction_status_sync(unit));

    SOC_EXIT;
exit:
    SOC_FUNC_RETURN;
}

int shr_sw_state_transaction_abort(int unit){

    SOC_INIT_FUNC_DEFS;

    if (shr_sw_state_operation_mode[unit].is_error_recovery != 0x1) {
        _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL,
                   (BSL_META_U(unit,
                      "Unit:%d aborting a transaction is only supported in error recovery modet\n"), unit));
    }

    if (shr_sw_state_data_block_header[unit]->transaction_data.status != TRANSACTION_STATUS_LOGGING_STARTED) {
        _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL,
                   (BSL_META_U(unit,
                      "Unit:%d trying to abort a transaction that wasn't started or already during commit\n"), unit));
    }

    /* restore from transaction's data */
    sal_memcpy(shr_sw_state_data_block_header[unit]->data_ptr,
               shr_sw_state_data_block_header[unit]->transaction_data.data_ptr,
               shr_sw_state_data_block_header[unit]->data_size);
    /* restore from transaction's pointer list */
    sal_memcpy(shr_sw_state_data_block_header[unit]->ptr_offsets_list,
               shr_sw_state_data_block_header[unit]->transaction_data.ptr_offsets_list,
               (shr_sw_state_data_block_header[unit]->ptr_offsets_len * sizeof(shr_sw_state_alloc_element_t)));

    SOC_EXIT;
exit:
    SOC_FUNC_RETURN;
}
#endif

int shr_sw_state_deinit(int unit) {
    SOC_INIT_FUNC_DEFS;

    sal_free(sw_state_sync_db[unit].dpp.htb_locks);

    _SOC_IF_ERR_EXIT(sw_state_htb_deinit(unit));
    _SOC_IF_ERR_EXIT(shr_sw_state_free(unit, (uint8*) sw_state[unit]));
    sw_state[unit] = NULL;

exit:
    SOC_FUNC_RETURN;
}

/*********************************************************************
 * NAME:
 *   shr_sw_state_free
 * FUNCTION:
 *   allocate space from the sw_state data block. space is not allocated by the operating system.
 *   a big chunk of memory was allocated during init in order to store all the future sw state.
 *   this function carves a space of size 'size' out of this data block and store the pointer to it
 *   inside ptr.
 * INPUT: 
 *   unit
 *   size - size to allocate
 * OUTPUT:
 *   ptr - ptr to the data allocated space
 * RETURNS:
 *   ERROR value.
*********************************************************************/
int
shr_sw_state_alloc(int unit, uint8** ptr, uint32 size) {

    uint32 elem_index;
    unsigned int orig_size, alloc_size;
    uint32	*p;

    SOC_INIT_FUNC_DEFS;

    

    /* return error if ptr is not in range*/
    if ((!(shr_sw_state_data_block_header[unit]->data_ptr <= ((uint8 *) ptr))) ||
         (!((shr_sw_state_data_block_header[unit]->data_ptr + shr_sw_state_data_block_header[unit]->data_size) >= ((uint8 *) ptr)))) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                       (BSL_META_U(unit,
                          "Unit:%d trying to allocate a pointer and save it outside of the sw_state scope\n"), unit));
    }

    orig_size = size;

    size = (size + 3) & ~3;

    alloc_size = size + 12;

    if (alloc_size < orig_size) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                       (BSL_META_U(unit,
                          "Unit:%d bad size input for shr_sw_state_alloc\n"), unit));
    }

    if (shr_sw_state_data_block_header[unit]->size_left < alloc_size) {
        _SOC_EXIT_WITH_ERR(SOC_E_MEMORY,
               (BSL_META_U(unit,
                  "Unit:%d shr_sw_state_alloc: not enough memory for allocation\n"), unit));
    }

    elem_index = shr_sw_state_data_block_header[unit]->next_free_offset_list_idx;

    if (elem_index == shr_sw_state_data_block_header[unit]->ptr_offsets_len) {
        _SOC_EXIT_WITH_ERR(SOC_E_MEMORY,
                           (BSL_META_U(unit,
                              "Unit:%d sw_state alloc failed\n"), unit));
    }

    if (shr_sw_state_data_block_header[unit]->ptr_offsets_list[elem_index].ptr_offset != -1) {
        _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL,
                           (BSL_META_U(unit,
                              "Unit:%d sw_state layout alloc overwrite existing data\n"), unit));
    }

    p = (uint32 *) shr_sw_state_data_block_header[unit]->next_free_data_slot;

    shr_sw_state_data_block_header[unit]->next_free_data_slot += alloc_size;
    shr_sw_state_data_block_header[unit]->size_left -= alloc_size;


    assert(UINTPTR_TO_PTR(PTR_TO_UINTPTR(p)) == p);

    p[0] = size / 4;
    p[1] = 0xcccccccc;
    p[2 + size / 4] = 0xdddddddd;

    *ptr = (uint8 *) &p[2];

    shr_sw_state_data_block_header[unit]->ptr_offsets_list[elem_index].ptr_offset = 
        (((uint8 *) ptr) - shr_sw_state_data_block_header[unit]->data_ptr);

    shr_sw_state_data_block_header[unit]->ptr_offsets_list[elem_index].ptr_value = *ptr;
    shr_sw_state_data_block_header[unit]->next_free_offset_list_idx++;
    
exit:
    SOC_FUNC_RETURN;
}

/*********************************************************************
 * NAME:
 *   shr_sw_state_free
 * FUNCTION:
 *   free space that was alloced using shr_sw_state_alloc
 * INPUT: 
 *   unit
 *   ptr - ptr to the data that should be freed*   
 * RETURNS:
 *   ERROR value.
*********************************************************************/
int
shr_sw_state_free(int unit, uint8 *ptr) {

    uint32 indx;
    uint32 *arr_len = 0;
    uint8 **freed_location;

    SOC_INIT_FUNC_DEFS;

    arr_len = &(shr_sw_state_data_block_header[unit]->next_free_offset_list_idx);
    
    /* iterate and remove all occurrences of ptr in the active ptr array */
    for(indx = 0; indx < *arr_len; indx++) {
        if (((uint8 *) shr_sw_state_data_block_header[unit]->ptr_offsets_list[indx].ptr_value) == ptr) {
            /* mark as NULL in actual pointer uses */
            freed_location = (uint8 **) (shr_sw_state_data_block_header[unit]->data_ptr +
                                         shr_sw_state_data_block_header[unit]->ptr_offsets_list[indx].ptr_offset);
            *freed_location = NULL;

            /* remove from active pointers list */ 
            shr_sw_state_data_block_header[unit]->ptr_offsets_list[indx].ptr_value = NULL;
            shr_sw_state_data_block_header[unit]->ptr_offsets_list[indx].ptr_offset = -1;

            /* move last item of list to fill the void */
            if (indx < (*arr_len - 1)) {
                shr_sw_state_data_block_header[unit]->ptr_offsets_list[indx] = 
                    shr_sw_state_data_block_header[unit]->ptr_offsets_list[*arr_len - 1];
                shr_sw_state_data_block_header[unit]->ptr_offsets_list[*arr_len - 1].ptr_value = NULL;
                shr_sw_state_data_block_header[unit]->ptr_offsets_list[*arr_len - 1].ptr_offset = -1;
                *arr_len -= 1;
                indx -= 1;
            }
        }
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
    uint8                             *scache_ptr;
    uint32                             alloc_size;
    uint32                             indx;
    uint8                            **tmp_ptr;
    uint8                             *restored_data_ptr;
    uint32                             total_buffer_size = 0;
    uint32                             j;

    SOC_INIT_FUNC_DEFS;

    SHR_SW_STATE_SCACHE_HANDLE_SET(wb_handle_orig, unit, SHR_SW_STATE_SCACHE_HANDLE_DATA_BLOCK);

    /* init the sw_state access callback data structure */
    sw_state_access_cb_init(unit);

    /* init operation mode */
    if (flags & SHR_SW_STATE_FLAG_TRANSACTION_SUPPORT) {
        shr_sw_state_operation_mode[unit].is_transaction_support = 0x1;
    }
    else {
        shr_sw_state_operation_mode[unit].is_transaction_support = 0x0;
    }

    switch (init_mode) {
        case socSwStateDataBlockRegularInit:

            total_buffer_size = size + 
                                sizeof(shr_sw_state_data_block_header_t) +
                                (SHR_SW_STATE_MAX_NOF_ALLOCATINS * sizeof(shr_sw_state_alloc_element_t));

            /* allocate new scache buffer */
            alloc_size = total_buffer_size;
            rc = shr_sw_state_scache_ptr_get(unit, wb_handle_orig, socSwStateScacheCreate, scache_flags,
                                    &alloc_size, &scache_ptr, &already_exists);
            _SOC_IF_ERR_EXIT(rc);
            
            shr_sw_state_data_block_header[unit] = (shr_sw_state_data_block_header_t *) scache_ptr;
            shr_sw_state_data_block_header[unit]->total_buffer_size = alloc_size;

            /* pointer to the location to the list of pointers locations inside the data block */
            shr_sw_state_data_block_header[unit]->ptr_offsets_list = 
                (shr_sw_state_alloc_element_t *) (shr_sw_state_data_block_header[unit] + 1);

            shr_sw_state_data_block_header[unit]->ptr_offsets_len = SHR_SW_STATE_MAX_NOF_ALLOCATINS;

            /* pointer to the location where actual data block begins */
            shr_sw_state_data_block_header[unit]->data_ptr = (uint8 *) ((shr_sw_state_data_block_header[unit]->ptr_offsets_list) + 
                                                                         SHR_SW_STATE_MAX_NOF_ALLOCATINS);

            shr_sw_state_data_block_header[unit]->data_size = size;

            /* init the next free data slot to the be the first slot in the data block */
            shr_sw_state_data_block_header[unit]->next_free_data_slot = shr_sw_state_data_block_header[unit]->data_ptr;

            /* offset_list_idx is an array, first free index is zero */
            shr_sw_state_data_block_header[unit]->next_free_offset_list_idx = 0;

            /* currently data block is empty so size_left==size */
            shr_sw_state_data_block_header[unit]->size_left = size;

            /*init the data to be all zeros*/
            sal_memset(shr_sw_state_data_block_header[unit]->data_ptr, 0x0, size);
            /*init the pointer list to be all zeros*/
            sal_memset(shr_sw_state_data_block_header[unit]->ptr_offsets_list, 0x0, (SHR_SW_STATE_MAX_NOF_ALLOCATINS * sizeof(shr_sw_state_alloc_element_t)));

            /* init ptr_offsets to be -1 to indicate 'not used' */
            for (j=0; j<SHR_SW_STATE_MAX_NOF_ALLOCATINS; j++) {
                shr_sw_state_data_block_header[unit]->ptr_offsets_list[j].ptr_offset = -1;
            }

            /* mark sw state as initialized */
            shr_sw_state_operation_mode[unit].is_init = 0x1;

            /* allocate the root of the sw state */
            _SOC_IF_ERR_EXIT(shr_sw_state_alloc(unit, (uint8 **) shr_sw_state_data_block_header[unit]->data_ptr, sizeof(shr_sw_state_t)));
            sw_state[unit] =  (shr_sw_state_t *) (*((uint8 **) shr_sw_state_data_block_header[unit]->data_ptr));
            
            break;

        case socSwStateDataBlockRestoreAndOveride:
            /* retrieve buffer */

            if (size != -1) {
                _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                           (BSL_META_U(unit,
                              "Unit:%d data block init don't expect positive size when init mode is socSwStateDataBlockRestoreAndOveride\n"), unit));
            }

            rc = shr_sw_state_scache_ptr_get(unit, wb_handle_orig, socSwStateScacheRetreive, scache_flags,
                                    &restored_size, &scache_ptr, &already_exists);
            _SOC_IF_ERR_EXIT(rc);
            shr_sw_state_data_block_header[unit] = (shr_sw_state_data_block_header_t *) scache_ptr;

            /* save based ptr of the saved data block for calculations below */
            restored_data_ptr = shr_sw_state_data_block_header[unit]->data_ptr;

            /* update header with base ptr in new data block */
            shr_sw_state_data_block_header[unit]->ptr_offsets_list = 
                ((shr_sw_state_alloc_element_t *) (((uint8 *) shr_sw_state_data_block_header[unit]) + sizeof(shr_sw_state_data_block_header_t)));

            shr_sw_state_data_block_header[unit]->data_ptr = 
                ((uint8 *) shr_sw_state_data_block_header[unit]->ptr_offsets_list) + 
                (shr_sw_state_data_block_header[unit]->ptr_offsets_len * sizeof(shr_sw_state_alloc_element_t));

            /* fix ptr value of next free slot (since ptr values have changed due to the restore)*/
            shr_sw_state_data_block_header[unit]->next_free_data_slot += (shr_sw_state_data_block_header[unit]->data_ptr - restored_data_ptr);

            

            /* Calculate and change restored (invalid) ptrs to represent ptrs in the new data block */
            for (indx = 0; indx < shr_sw_state_data_block_header[unit]->next_free_offset_list_idx; indx++) {
                if (shr_sw_state_data_block_header[unit]->ptr_offsets_list[indx].ptr_offset == -1) {
                    /* deleted pointer (due to free) */
                    continue;
                }
                tmp_ptr = (uint8 **) (shr_sw_state_data_block_header[unit]->data_ptr + shr_sw_state_data_block_header[unit]->ptr_offsets_list[indx].ptr_offset);
                /* from the recovered pointer stored at offset subtract the restored base ptr and add the new base ptr */
                *tmp_ptr += (shr_sw_state_data_block_header[unit]->data_ptr - restored_data_ptr);
            }

            /*direct sw_state to it's restored location*/
            sw_state[unit] = (shr_sw_state_t *) (*((uint8 **) shr_sw_state_data_block_header[unit]->data_ptr));

            /* NO ISSU - use the restored data block with fixed pointers as the new data block */
            /* ISSU will be added as a different init */

            break;

        default:
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                       (BSL_META_U(unit,
                          "Unit:%d tried to initialize data block with unsupported mode\n"), unit));
    }

    shr_sw_state_operation_mode[unit].is_init = 0x1;

    sw_state_sync_db[unit].dpp.htb_locks = sal_alloc(sizeof(sal_mutex_t) * SW_STATE_MAX_NOF_HASH_TABLES, "hash table mutexes");
    _SOC_IF_ERR_EXIT(sw_state_htb_init(unit, SW_STATE_MAX_NOF_HASH_TABLES));

    _SOC_IF_ERR_EXIT(rc);

exit:
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
    int index;
    uint8 **ptr_location;
    unsigned int i;
    unsigned int len;
    int HEXDUMP_COLS = 8;

    if ((output_file = sal_fopen(file_name, mode)) == 0) {
        cli_out("Error opening sw dump file %s\n", file_name);
        return;
    }

    len = shr_sw_state_data_block_header[unit]->data_size - shr_sw_state_data_block_header[unit]->size_left;


    buff_to_print = sal_alloc(sizeof(uint8) * len, "sw state block dump");

    sal_memcpy(buff_to_print, shr_sw_state_data_block_header[unit]->data_ptr, len);

    /* cover pointer for cleaner diff (put NULL) */
    for (index=0; index < shr_sw_state_data_block_header[unit]->next_free_offset_list_idx; index++) {
        ptr_location = (uint8 **) 
            (buff_to_print + shr_sw_state_data_block_header[unit]->ptr_offsets_list[index].ptr_offset);
        *ptr_location = NULL;
    }

    sal_fprintf(output_file, " ************************ \n");
    sal_fprintf(output_file, " *****sw state block***** \n");
    sal_fprintf(output_file, " ************************ \n");

    for(i = 0; i < len + ((len % HEXDUMP_COLS) ? (HEXDUMP_COLS - len % HEXDUMP_COLS) : 0); i++)
    {
            /* print offset */
            if(i % HEXDUMP_COLS == 0)
            {
                    sal_fprintf(output_file, "\n0x%06x: ", i);
            }

            /* print hex data */
            if(i < len)
            {
                    sal_fprintf(output_file, "%02x ", 0xFF & ((char*)buff_to_print)[i]);
            }
            else /* end of block, just aligning for ASCII dump */
            {
                    sal_fprintf(output_file, "\n");
            }
    }

    sal_fclose(output_file);
    sal_free(buff_to_print);

    return;
#else
    cli_out("This function is not supported in kernek mode\n");
#endif /* ! __KERNEL__ */
}

#undef _ERR_MSG_MODULE_NAME

