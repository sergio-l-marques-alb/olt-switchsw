/*
 * $Id: wb_db_hash_tbl.c,v 1.9 Broadcom SDK $
 * $Copyright: Copyright 2012 Broadcom Corporation.
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

/* we need this junk function only to avoid building error of pedantic compilation without the flag BCM_WARM_BOOT_SUPPORT */
void ___junk_function_wb_db_hash_tbl(void){
}
#ifdef BCM_WARM_BOOT_SUPPORT

#ifdef _ERR_MSG_MODULE_NAME 
    #error "_ERR_MSG_MODULE_NAME redefined" 
#endif
#define _ERR_MSG_MODULE_NAME BSL_SOC_INIT

#include <shared/bsl.h>

#include <soc/dcmn/error.h>

#include <soc/types.h>
#include <soc/debug.h>
#include <soc/error.h>
#include <soc/dpp/wb_db_hash_tbl.h>
#include <soc/dpp/dpp_wb_engine.h>
#include <sal/core/libc.h>
#include <shared/alloc.h>
#include <shared/util.h>
#include <sal/core/sync.h>
#include <soc/dpp/SAND/Utils/sand_framework.h>


dpp_wb_hash_tbl_data_t wb_hash_tbl_data[SOC_SAND_MAX_DEVICE][SOC_DPP_WB_HASH_TBLS_NUM];

sal_mutex_t wb_db_hash_tbl_lock;
int current_hash_tbl_idx = -1;
int current_saved_idx    = -1;
int nof_active_htb = 0;

/* a trick to triger the buffer to be marked as dirty at wb_engine although
   it is modified outside of engine*/
#define DPP_WB_HASH_TBL_MARK_BUFFER_AS_DIRTY\
    SOC_DPP_WB_ENGINE_SET_VAR(unit,\
        wb_hash_tbl_data[unit][hash_tbl_idx].wb_var_idx + WB_ENGINE_BCM_HASH_TABLE_NOF_USED_VALUES,\
        &wb_hash_tbl_data[unit][hash_tbl_idx].nof_used_values);

/*
 * Function:
 *   dpp_hash_tbl_wb_init
 * Purpose:
 *   initialize warmboot for a specific hash table.
 * Parameters:
 *   (in)     hash_tbl_idx  - hash table index (taken from SOC_DPP_WB_HASH_TBLS enum)
 *   (in)     ht  - pointer to actual hash table
 *   (in)     key_size  - size of the key
 *   (in)     max_nof_values  - maximum number of items to be inserted to the hash table
 *   (in)     save_fn  - callback function for packing hash entry data item into buffer
 *   (in)     load_fn  - callback function for extracting hash entry data from buffer
 * Returns:
 *   BCM_E_NONE upon success
 *   BCM_E_*    on failure
 * Notes:
 *  The CALLER is responsible for freeing memory allocated during this init by calling dpp_hash_tbl_wb_destroy with the same hash_tbl_idx
 *
 */
int
dpp_hash_tbl_wb_init(int unit,int hash_tbl_idx, int wb_var_idx, shr_htb_hash_table_t ht, int key_size, int max_nof_values, int max_data_size, dpp_hash_tbl_save_fn save_fn, dpp_hash_tbl_load_fn load_fn)
{
    int rv = SOC_E_NONE;

    if (wb_hash_tbl_data[unit][hash_tbl_idx].is_initialized == 1) {
        LOG_VERBOSE(BSL_LS_SOC_INIT,
                    (BSL_META_U(unit,
                                "Unit:%d failed to init hash table dpp_hash_tbl_wb_init - hash table exists\n"), unit));
        return _SHR_E_EXISTS;
    }

    /*create module's mutex if its the first hash table init*/
    if (nof_active_htb == 0) {
        wb_db_hash_tbl_lock = sal_mutex_create("wb_db_hash_tbl");
        if (wb_db_hash_tbl_lock == NULL) {
            sal_free(wb_db_hash_tbl_lock);
            LOG_VERBOSE(BSL_LS_SOC_INIT,
                        (BSL_META_U(unit,
                                    "Unit:%d failed to create mutex\n"), unit));
            return SOC_E_RESOURCE;
        }
    }

    nof_active_htb++;

    wb_hash_tbl_data[unit][hash_tbl_idx].max_nof_values = max_nof_values;
    wb_hash_tbl_data[unit][hash_tbl_idx].max_data_size = max_data_size;
    wb_hash_tbl_data[unit][hash_tbl_idx].save_fn = save_fn;
    wb_hash_tbl_data[unit][hash_tbl_idx].load_fn = load_fn;
    wb_hash_tbl_data[unit][hash_tbl_idx].key_size = key_size;
    wb_hash_tbl_data[unit][hash_tbl_idx].wb_var_idx = wb_var_idx;
    wb_hash_tbl_data[unit][hash_tbl_idx].nof_used_values = 0;
    wb_hash_tbl_data[unit][hash_tbl_idx].ht = ht;
    wb_hash_tbl_data[unit][hash_tbl_idx].is_initialized = 1;


    wb_hash_tbl_data[unit][hash_tbl_idx].keys = sal_alloc(max_nof_values * key_size, "wb_hash_tbl keys array");
    if (wb_hash_tbl_data[unit][hash_tbl_idx].keys == NULL) {
        LOG_VERBOSE(BSL_LS_SOC_INIT,
                    (BSL_META_U(unit,
                                "Unit:%d failed to allocate memory in dpp_hash_tbl_wb_init\n"), unit));
        return SOC_E_MEMORY;
    }
    sal_memset(wb_hash_tbl_data[unit][hash_tbl_idx].keys, -1, max_nof_values * key_size);

    wb_hash_tbl_data[unit][hash_tbl_idx].data = sal_alloc(max_nof_values * max_data_size, "wb_hash_tbl data array");
    if (wb_hash_tbl_data[unit][hash_tbl_idx].data == NULL) {
        LOG_VERBOSE(BSL_LS_SOC_INIT,
                    (BSL_META_U(unit,
                                "Unit:%d failed to allocate memory in dpp_hash_tbl_wb_init\n"), unit));
        return SOC_E_MEMORY;
    }
    sal_memset(wb_hash_tbl_data[unit][hash_tbl_idx].data, -1, max_nof_values * max_data_size);

    return rv;
}

/*
 * Function:
 *   dpp_hash_tbl_wb_destroy
 * Purpose:
 *   free memory allocated for a specific hash table warmboot.
 * Parameters:
 *   (in)     hash_tbl_idx  - hash table index (taken from SOC_DPP_WB_HASH_TBLS enum)
 * Returns:
 *   BCM_E_NONE upon success
 *   BCM_E_*    on failure
 */
int
dpp_hash_tbl_wb_destroy(int unit,int hash_tbl_idx)
{
    int rv = SOC_E_NONE;

    if (wb_hash_tbl_data[unit][hash_tbl_idx].is_initialized == 0) {
        LOG_VERBOSE(BSL_LS_SOC_INIT,
                    (BSL_META_U(unit,
                                "Unit:%d failed to destroy hash table dpp_hash_tbl_wb_destroy - hash table doesnt exists\n"), unit));
        return _SHR_E_NOT_FOUND;
    }

    nof_active_htb--;

    if (nof_active_htb == 0)
    {
        /*last active hash table destroy the module's mutex*/
        sal_mutex_destroy(wb_db_hash_tbl_lock);
    }
    
    if (wb_hash_tbl_data[unit][hash_tbl_idx].keys == NULL) {
        LOG_VERBOSE(BSL_LS_SOC_INIT,
                    (BSL_META_U(unit,
                                "Unit:%d try to free memory that was not allocated dpp_hash_tbl_wb_destroy\n"), unit));
        rv = SOC_E_INTERNAL;
    } else {
        sal_free(wb_hash_tbl_data[unit][hash_tbl_idx].keys);
    }

    if (wb_hash_tbl_data[unit][hash_tbl_idx].data == NULL) {
        LOG_VERBOSE(BSL_LS_SOC_INIT,
                    (BSL_META_U(unit,
                                "Unit:%d try to free memory that was not allocated dpp_hash_tbl_wb_destroy\n"), unit));
        rv = SOC_E_INTERNAL;
    } else {
        sal_free(wb_hash_tbl_data[unit][hash_tbl_idx].data);
    }

    sal_memset(&wb_hash_tbl_data[unit][hash_tbl_idx], 0x0, sizeof(wb_hash_tbl_data[unit][hash_tbl_idx]));

    return rv;
}

/*
 * Function:
 *   dpp_wb_flat_hash_tbl_data
 * Purpose:
 *   internal callback to be used with hash_table iterator.
 */
STATIC int
dpp_wb_flat_hash_tbl_data(int unit, shr_htb_key_t key, shr_htb_data_t data)
{
    wb_hash_tbl_data[unit][current_hash_tbl_idx].nof_used_values++;

    /*save the key*/
    sal_memcpy(wb_hash_tbl_data[unit][current_hash_tbl_idx].keys + (current_saved_idx * wb_hash_tbl_data[unit][current_hash_tbl_idx].key_size), key, wb_hash_tbl_data[unit][current_hash_tbl_idx].key_size);

    /*save the data (using callback)*/
    if (wb_hash_tbl_data[unit][current_hash_tbl_idx].save_fn) {
        wb_hash_tbl_data[unit][current_hash_tbl_idx].save_fn(data, (wb_hash_tbl_data[unit][current_hash_tbl_idx].data + (current_saved_idx * wb_hash_tbl_data[unit][current_hash_tbl_idx].max_data_size)));
    }
    else {
        LOG_VERBOSE(BSL_LS_SOC_INIT,
                    (BSL_META_U(unit,
                                "Unit:%d try to use callback that is not registered dpp_wb_flat_hash_tbl_data\n"), unit));
        return SOC_E_INTERNAL;
    }

    current_saved_idx++;

    return SOC_E_NONE;
}

/*
 * Function:
 *   dpp_fill_wb_arrays_from_hash_tbl
 * Purpose:
 *   prepare the mirror of the hash table that is later saved to external storage.
 * Parameters:
 *   (in)     hash_tbl_idx  - hash table index (taken from SOC_DPP_WB_HASH_TBLS enum)
 * Returns:
 *   BCM_E_NONE upon success
 *   BCM_E_*    on failure
 *
 */
int
dpp_fill_wb_arrays_from_hash_tbl(int unit, int hash_tbl_idx)
{
    int                 rv = SOC_E_NONE;

    if (!wb_hash_tbl_data[unit][hash_tbl_idx].is_initialized) {
        /*nothing to do*/
        return SOC_E_NONE;
    }

    sal_mutex_take(wb_db_hash_tbl_lock, sal_mutex_FOREVER);

	sal_memset(wb_hash_tbl_data[unit][hash_tbl_idx].keys, -1, wb_hash_tbl_data[unit][hash_tbl_idx].max_nof_values * wb_hash_tbl_data[unit][hash_tbl_idx].key_size);
    sal_memset(wb_hash_tbl_data[unit][hash_tbl_idx].data, -1, wb_hash_tbl_data[unit][hash_tbl_idx].max_nof_values * wb_hash_tbl_data[unit][hash_tbl_idx].max_data_size);

    wb_hash_tbl_data[unit][hash_tbl_idx].nof_used_values = 0;
    current_saved_idx = 0;
    current_hash_tbl_idx = hash_tbl_idx;

    shr_htb_iterate(unit, wb_hash_tbl_data[unit][hash_tbl_idx].ht, dpp_wb_flat_hash_tbl_data);

    current_saved_idx = -1;
    current_hash_tbl_idx = -1;

    /*mark buffer as dirty*/
    rv = DPP_WB_HASH_TBL_MARK_BUFFER_AS_DIRTY;

    sal_mutex_give(wb_db_hash_tbl_lock);

    return(rv);
}

/*
 * Function:
 *   dpp_extract_hash_tbl_from_wb_arrays
 * Purpose:
 *   fill the hash table with data recovered in warmboot.
 * Parameters:
 *   (in)     hash_tbl_idx  - hash table index (taken from SOC_DPP_WB_HASH_TBLS enum)
 *   (in)     ht  - the actual hash table to operate on. 
 * Returns:
 *   BCM_E_NONE upon success
 *   BCM_E_*    on failure
 * Notes:
 *   expect an empty hash_table ht initialized with same properties as the hash table saved to external storage.
 */
int
dpp_extract_hash_tbl_from_wb_arrays(int unit, int hash_tbl_idx)
{
    int                 rv = SOC_E_NONE;
    int                 saved_idx = 0;
    shr_htb_data_t      data;
    shr_htb_key_t       key;

    data = NULL;

    if (wb_hash_tbl_data[unit][hash_tbl_idx].keys    == NULL ||
        wb_hash_tbl_data[unit][hash_tbl_idx].data    == NULL) 
    {
        LOG_VERBOSE(BSL_LS_SOC_INIT,
                    (BSL_META_U(unit,
                                "Unit:%d hash table #%d wb arrays don't exist dpp_extract_hash_tbl_from_wb_arrays\n"), unit, hash_tbl_idx));
        return SOC_E_INTERNAL;
    }

    for (saved_idx = 0; saved_idx < wb_hash_tbl_data[unit][hash_tbl_idx].nof_used_values; saved_idx++) {
        /*save the data (using callback)*/
        if (wb_hash_tbl_data[unit][hash_tbl_idx].load_fn) {
            wb_hash_tbl_data[unit][hash_tbl_idx].load_fn((wb_hash_tbl_data[unit][hash_tbl_idx].data + (saved_idx * wb_hash_tbl_data[unit][hash_tbl_idx].max_data_size)), &data);
        }
        else {
            LOG_VERBOSE(BSL_LS_SOC_INIT,
                        (BSL_META_U(unit,
                                    "Unit:%d try to use callback that is not registered dpp_extract_hash_tbl_from_wb_arrays\n"), unit));
            return SOC_E_INTERNAL;
        }

        key = wb_hash_tbl_data[unit][hash_tbl_idx].keys + (saved_idx * wb_hash_tbl_data[unit][hash_tbl_idx].key_size);

        rv = shr_htb_insert(wb_hash_tbl_data[unit][hash_tbl_idx].ht, key, data);
    }

    return(rv);
}

#endif /*BCM_WARM_BOOT_SUPPORT*/

