/* $Id: sand_hashtable.c,v 1.12 Broadcom SDK $
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
*/

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_COMMON

/*************
* INCLUDES  *
*************/
/* { */


#include <shared/bsl.h>
#include <soc/dpp/drv.h>



#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Utils/sand_hashtable.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/dpp_wb_engine.h>
#include <soc/dpp/drv.h>


/* } */

/*************
* DEFINES   *
*************/
/* { */

/* $Id: sand_hashtable.c,v 1.12 Broadcom SDK $
 * Maximum number of rehashes before giving up: when trying to add
 * a key, hash the key to get an entry index
 * if the entry in use, try to rehash until finding unused entry
 * after this number of tries return with a fail.
 */
#define SOC_SAND_HASH_TABLE_MAX_NOF_REHASH  50


/* } */

/*************
*  MACROS   *
*************/
/* { */

/* } */

/*************
* TYPE DEFS *
*************/
/* { */
/*
 * the key and data type, used for malloc.
 */
typedef uint8 SOC_SAND_HASH_TABLE_KEY_TYPE;


 /* } */

 /*************
 * GLOBALS   *
 *************/
 /* { */

 /* } */

 /*************
 * FUNCTIONS *
 *************/
 /* { */

/************************************************************************/
/*  Internal functions                                                  */
/************************************************************************/

uint32
sand_hashtable_default_set_entry_fun(
  SOC_SAND_IN    int prime_handle,
  SOC_SAND_IN    uint32 sec_handle,
  SOC_SAND_INOUT uint8  *buffer,
  SOC_SAND_IN    uint32 offset,
  SOC_SAND_IN    uint32 len,
  SOC_SAND_IN    uint8  *data
)
{
  sal_memcpy(
    buffer + (offset * len),
    data,
    len
  );
  return SOC_SAND_OK;
}


uint32
sand_hashtable_default_get_entry_fun(
  SOC_SAND_IN  int prime_handle,
  SOC_SAND_IN  uint32 sec_handle,
  SOC_SAND_IN  uint8  *buffer,
  SOC_SAND_IN  uint32 offset,
  SOC_SAND_IN  uint32 len,
  SOC_SAND_OUT uint8  *data
)
{
  sal_memcpy(
    data,
    buffer + (offset * len),
    len
  );
  return SOC_SAND_OK;
}

 /*********************************************************************
 * NAME:
 *     soc_sand_hash_table_simple_hash
 * FUNCTION:
 *    Hash functions maps from key to hash value.
 *    simple and default implementation of a hash function.
 * INPUT:
 *  SOC_SAND_IN  int                            unit -
 *   Identifier of the device to access.
 *  SOC_SAND_IN  SOC_SAND_HASH_TABLE_PTR        hash_table -
 *   the hash table, needed so the hash function
 *   can get the properties of the hash table to consider in the
 *   calculations
 *  SOC_SAND_IN  SOC_SAND_HASH_TABLE_KEY        key -
 *   key to hash
 *  SOC_SAND_IN  uint32                         seed -
 *   value to use in the hash calculation
 *  SOC_SAND_OUT  uint32*             hash_val -
 *   the calculated hash value.
 * REMARKS:
 *     None.
 * RETURNS:
 *     OK or ERROR indication.
 *********************************************************************/
uint32
  soc_sand_hash_table_simple_hash(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  SOC_SAND_HASH_TABLE_PTR_WORKAROUND     hash_table,
    SOC_SAND_IN  SOC_SAND_HASH_TABLE_KEY*    const key,
    SOC_SAND_IN  uint32                      seed,
    SOC_SAND_OUT uint32*                     hash_val
  )
{
  SOC_SAND_HASH_TABLE_INFO
    *hash_info;
  uint32
    indx,
    tmp;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(hash_table);
  SOC_SAND_CHECK_NULL_INPUT(key);
  SOC_SAND_CHECK_NULL_INPUT(hash_val);

  tmp = 5381;

  hash_info = (SOC_SAND_HASH_TABLE_INFO*) hash_table;
  for (indx = 0; indx < hash_info->init_info.key_size; ++indx)
  {
    tmp = ((tmp  << 5 ) + tmp ) ^ key[indx]  ;
  }

  *hash_val = tmp % hash_info->init_info.table_width;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_hash_table_simple_hash()",0,0);
}


/*********************************************************************
* NAME:
*     soc_sand_hash_table_simple_rehash
* FUNCTION:
*    Hash functions maps from key to hash value.
*    simple and default implementation of a rehash function.
*    returns the next entry in the hash table.
* INPUT:
*  SOC_SAND_IN  int                            unit -
*   Identifier of the device to access.
*  SOC_SAND_IN  SOC_SAND_HASH_TABLE_PTR        hash_table -
*   the hash table, needed so the hash function
*   can get the properties of the hash table to consider in the
*   calculations
*  SOC_SAND_IN  SOC_SAND_HASH_TABLE_KEY        key -
*   key to hash
*  SOC_SAND_IN  uint32                         seed -
*   value to use in the hash calculation
*  SOC_SAND_OUT  uint32*             hash_val -
*   the calculated hash value.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_sand_hash_table_simple_rehash(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_SAND_HASH_TABLE_PTR_WORKAROUND    hash_table,
    SOC_SAND_IN  SOC_SAND_HASH_TABLE_KEY*   const key,
    SOC_SAND_IN  uint32                     seed,
    SOC_SAND_OUT uint32*                    hash_val
  )
{
  SOC_SAND_HASH_TABLE_INFO
    *hash_info;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(hash_table);
  SOC_SAND_CHECK_NULL_INPUT(key);
  SOC_SAND_CHECK_NULL_INPUT(hash_val);

  hash_info = (SOC_SAND_HASH_TABLE_INFO*) hash_table;

  *hash_val = (seed + 1) % hash_info->init_info.table_width;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_hash_table_simple_hash()",0,0);
}



/*********************************************************************
* NAME:
*     soc_sand_hash_table_find_entry
* TYPE:
*   PROC
* DATE:
*   Mar  6 2008
* FUNCTION:
*  lookup in the hash table for the given key and return the data inserted with
*  the given key.
* INPUT:
*  SOC_SAND_IN  int                            unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_SAND_HASH_TABLE_PTR        hash_table -
*     The hash table.
*   SOC_SAND_IN  SOC_SAND_HASH_TABLE_KEY       key -
*     The key to lookup for
*   SOC_SAND_IN  uint8                         first_empty
*     whether to return the first empty entry .
*   SOC_SAND_OUT  uint32                      *entry -
*     if the key is present in the hash table then return the entry the key found at,
*     otherwise it returns the place where the key suppose to be.
*   SOC_SAND_IN  uint8                        *found -
*     whether the key was found in the hash table
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
STATIC uint32
  soc_sand_hash_table_find_entry(
    SOC_SAND_IN     int                          unit,
    SOC_SAND_INOUT  SOC_SAND_HASH_TABLE_INFO    *hash_table,
    SOC_SAND_IN     SOC_SAND_HASH_TABLE_KEY*     const key,
    SOC_SAND_IN     uint8                        first_empty,
    SOC_SAND_IN     uint8                        alloc_by_index,
    SOC_SAND_OUT    uint32                      *entry,
    SOC_SAND_OUT    uint8                       *found,
    SOC_SAND_OUT    uint32                      *prev,
    SOC_SAND_OUT    uint8                       *first
  );

/************************************************************************/
/*  End of internals                                                    */
/************************************************************************/


/*********************************************************************
* NAME:
*     soc_sand_hash_table_create
* TYPE:
*   PROC
* DATE:
*   Mar  6 2008
* FUNCTION:
*     Creates a new Hash table instance.
* INPUT:
*   SOC_SAND_IN     int                           unit -
*     Identifier of the device to access.
*   SOC_SAND_INOUT  SOC_SAND_HASH_TABLE_INFO     *hash_table -
*     information to use in order to create the hash table (size, hash function...)
* REMARKS:
*   None.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_sand_hash_table_create(
    SOC_SAND_IN     int                           unit,
    SOC_SAND_INOUT  SOC_SAND_HASH_TABLE_PTR           *hash_table_ptr,
    SOC_SAND_IN     SOC_SAND_HASH_TABLE_INIT_INFO     init_info
  )
{
  SOC_SAND_HASH_TABLE_INIT_INFO
    *init_info_ptr;
  SOC_SAND_HASH_TABLE_T
    *hash_data_ptr;
  SOC_SAND_OCC_BM_INIT_INFO
    btmp_init_info;
  uint32
    max_buf_size;
  uint32
    res;
  SOC_SAND_HASH_TABLE_INFO
    *hash_table;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_HASH_TABLE_CREATE);

  SOC_SAND_CHECK_NULL_INPUT(hash_table_ptr);
  *hash_table_ptr = soc_sand_os_malloc_any_size(sizeof(SOC_SAND_HASH_TABLE_INFO), "hash_table_ptr");
  soc_sand_SAND_HASH_TABLE_INFO_clear(*hash_table_ptr);
  hash_table = *hash_table_ptr;

  hash_table->init_info.prime_handle  = init_info.prime_handle;
  hash_table->init_info.sec_handle    = init_info.sec_handle;
  hash_table->init_info.table_size    = init_info.table_size;
  hash_table->init_info.table_width   = init_info.table_width;
  hash_table->init_info.key_size      = init_info.key_size;
  hash_table->init_info.data_size     = init_info.data_size;
  hash_table->init_info.get_entry_fun = init_info.get_entry_fun;
  hash_table->init_info.set_entry_fun = init_info.set_entry_fun;
  hash_table->init_info.wb_var_index  = init_info.wb_var_index;
  
  init_info_ptr = &(hash_table->init_info);
  hash_data_ptr = &(hash_table->hash_data);

  if (init_info_ptr->table_size == 0 || init_info_ptr->table_width == 0 ||
      init_info_ptr->key_size == 0 || init_info_ptr->data_size == 0)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_HASH_TABLE_SIZE_OUT_OF_RANGE_ERR, 10, exit);
  }
 /*
  * allocate the hash table struct
  */

 /*
  * calculate the size of pointers (list head and next) according to table size.
  */
  hash_data_ptr->ptr_size = (soc_sand_log2_round_up(hash_table->init_info.table_size + 1) + (SOC_SAND_NOF_BITS_IN_BYTE - 1)) / SOC_SAND_NOF_BITS_IN_BYTE;
  hash_data_ptr->null_ptr = SOC_SAND_BITS_MASK((hash_data_ptr->ptr_size * SOC_SAND_NOF_BITS_IN_BYTE - 1),0);

 /*
  * allocate buffer for keys
  */
  hash_data_ptr->keys = (uint8*)soc_sand_os_malloc_any_size(init_info_ptr->table_size * init_info_ptr->key_size * sizeof(uint8), "hash_data_ptr->keys");
  if (!hash_data_ptr->keys)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_MALLOC_FAIL, 20, exit);
  }
  soc_sand_os_memset(
    hash_data_ptr->keys,
    0x0,
    init_info_ptr->table_size * init_info_ptr->key_size * sizeof(uint8)
  );

 /*
  * allocate buffer for next array (to build the linked list)
  */
  hash_data_ptr->next = (uint8*)soc_sand_os_malloc_any_size(init_info_ptr->table_size * hash_data_ptr->ptr_size * sizeof(uint8), "hash_data_ptr->next");
  if (!hash_data_ptr->next)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_MALLOC_FAIL, 30, exit);
  }
  soc_sand_os_memset(
    hash_data_ptr->next,
    0xFF,
    init_info_ptr->table_size * hash_data_ptr->ptr_size * sizeof(uint8)
  );

 /*
  * allocate buffer for lists_head (to build the linked list)
  */
  hash_data_ptr->lists_head = (uint8*)soc_sand_os_malloc_any_size(init_info_ptr->table_width * hash_data_ptr->ptr_size * sizeof(uint8), "hash_data_ptr->lists_head");
  if (!hash_data_ptr->lists_head)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_MALLOC_FAIL, 40, exit);
  }
  soc_sand_os_memset(
    hash_data_ptr->lists_head,
    0xFF,
    init_info_ptr->table_width * hash_data_ptr->ptr_size * sizeof(uint8)
  );

 /*
  * if the hash function is not given then use the default hash function.
  */
  if (!init_info_ptr->hash_function)
  {
    init_info_ptr->hash_function = soc_sand_hash_table_simple_hash;
  }

 /*
  * if the rehash function is not given then use the default rehash function.
  */
  if (!init_info_ptr->rehash_function)
  {
    init_info_ptr->rehash_function = soc_sand_hash_table_simple_rehash;
  }

  max_buf_size = init_info_ptr->data_size;
  if (init_info_ptr->data_size > max_buf_size)
  {
    max_buf_size = init_info_ptr->data_size;
  }
  if (init_info_ptr->key_size > max_buf_size)
  {
    max_buf_size = init_info_ptr->key_size;
  }
  if (hash_data_ptr->ptr_size > max_buf_size)
  {
    max_buf_size = hash_data_ptr->ptr_size;
  }

  hash_data_ptr->tmp_buf = (uint8*)soc_sand_os_malloc_any_size(max_buf_size * sizeof(uint8), "hash_data_ptr->tmp_buf");
  if (!hash_data_ptr->tmp_buf)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_MALLOC_FAIL, 40, exit);
  }
  soc_sand_os_memset(
    hash_data_ptr->tmp_buf,
    0x0,
    max_buf_size * sizeof(uint8)
  );

  hash_data_ptr->tmp_buf2 = (uint8*)soc_sand_os_malloc_any_size(max_buf_size * sizeof(uint8), "hash_data_ptr->tmp_buf2");
  if (!hash_data_ptr->tmp_buf2)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_MALLOC_FAIL, 45, exit);
  }
  soc_sand_os_memset(
    hash_data_ptr->tmp_buf2,
    0x0,
    max_buf_size * sizeof(uint8)
  );


  soc_sand_SAND_OCC_BM_INIT_INFO_clear(&btmp_init_info);
  btmp_init_info.size = init_info_ptr->table_size;

  if(SOC_DPP_WB_ENGINE_VAR_NONE != init_info_ptr->wb_var_index)
  {
      btmp_init_info.wb_var_index = init_info_ptr->wb_var_index + WB_ENGINE_HASH_TABLE_MEMORY_USE;
      btmp_init_info.unit    = init_info_ptr->prime_handle;      
  }
 /* initialize the data to be mapped to*/

  res = soc_sand_occ_bm_create(
          unit,
          &btmp_init_info,
          &(hash_data_ptr->memory_use)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_hash_table_create()",0,0);
}


/*********************************************************************
* NAME:
*     soc_sand_hash_table_destroy
* TYPE:
*   PROC
* DATE:
*   Mar  6 2008
* FUNCTION:
*     free the hash table instance.
* INPUT:
*  SOC_SAND_OUT  SOC_SAND_HASH_TABLE_PTR hash_table -
*     The Hash table to destroy.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_sand_hash_table_destroy(
    SOC_SAND_IN     int                        unit,
    SOC_SAND_INOUT  SOC_SAND_HASH_TABLE_INFO  *hash_table
  )
{
  SOC_SAND_HASH_TABLE_T
    *hash_data_ptr;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_HASH_TABLE_DESTROY);
  SOC_SAND_CHECK_NULL_INPUT(hash_table);

  hash_data_ptr = &(hash_table->hash_data);

  soc_sand_os_free_any_size(hash_data_ptr->lists_head);
  soc_sand_os_free_any_size(hash_data_ptr->next);
  soc_sand_os_free_any_size(hash_data_ptr->keys);
  soc_sand_os_free_any_size(hash_data_ptr->tmp_buf);
  soc_sand_os_free_any_size(hash_data_ptr->tmp_buf2);

  res = soc_sand_occ_bm_destroy(
          unit,
          hash_data_ptr->memory_use
        );

  soc_sand_os_free_any_size(hash_table);

  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_hash_table_destroy()",0,0);
}

/*********************************************************************
* NAME:
*     soc_sand_hash_table_entry_add
* TYPE:
*   PROC
* DATE:
*   Mar  6 2008
* FUNCTION:
*  Insert an entry into the hash table, if already exist then
*  the operation returns an error.
* INPUT:
*   SOC_SAND_IN  int                        unit -
*     Identifier of the device to access.
*   SOC_SAND_INOUT  SOC_SAND_HASH_TABLE_PTR hash_table -
*     The hash table to add a key to.
*   SOC_SAND_IN  SOC_SAND_HASH_TABLE_KEY    key -
*     The key to add into the hash table
*   SOC_SAND_IN  uint8                      occupied -
*     the data to add into the hash table and to be associated with
*     the given key
*   SOC_SAND_OUT  uint8                    *success -
*     whether the add operation success, this may be false.
*     if after trying to rehash the key SOC_SAND_HASH_TABLE_MAX_NOF_REHASH times
*     and in all tries fill in in_use entry. to solve this problem try
*     to enlarge the hash table size or use better hash function.
* REMARKS:
*     = if there is already a key with the same key in the hash table error is returned.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_sand_hash_table_entry_add(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_INOUT  SOC_SAND_HASH_TABLE_INFO    *hash_table,
    SOC_SAND_IN  SOC_SAND_HASH_TABLE_KEY*        const key,
    SOC_SAND_OUT  uint32                        *data_indx,
    SOC_SAND_OUT  uint8                         *success
  )
{
  SOC_SAND_HASH_TABLE_INIT_INFO
    *init_info_ptr;
  SOC_SAND_HASH_TABLE_T
    *hash_data_ptr;
  uint8
    found,
    first;
  uint32
    entry_offset,
    prev_entry;
  uint32
    res;
  SOC_SAND_HASH_MAP_SW_DB_ENTRY_SET 
    set_entry_fun;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_HASH_TABLE_ENTRY_ADD);

  SOC_SAND_CHECK_NULL_INPUT(hash_table);
  SOC_SAND_CHECK_NULL_INPUT(key);
  SOC_SAND_CHECK_NULL_INPUT(data_indx);
  SOC_SAND_CHECK_NULL_INPUT(success);




  /*if (hash_table->entries >= hash_table->table_size)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_HASH_TABLE_IS_FULL_ERR, 10, exit);
  }*/

  init_info_ptr = &(hash_table->init_info);
  hash_data_ptr = &(hash_table->hash_data);

  if (init_info_ptr->set_entry_fun) {
      set_entry_fun = init_info_ptr->set_entry_fun;
  }
  else
  {
      set_entry_fun = sand_hashtable_default_set_entry_fun;
  }

  /* check to see if the entry exists */
  res = soc_sand_hash_table_find_entry(
          unit,
          hash_table,
          key,
          TRUE,
          FALSE,
          &entry_offset,
          &found,
          &prev_entry,
          &first
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  if (found)
  {
    *data_indx = entry_offset;
  }
  if (entry_offset == SOC_SAND_HASH_TABLE_NULL)
  {
    *success = FALSE;
    *data_indx = SOC_SAND_HASH_TABLE_NULL;
    goto exit;
  }


  if(SOC_DPP_WB_ENGINE_VAR_NONE == init_info_ptr->wb_var_index)
  {
      res = set_entry_fun(
                                         init_info_ptr->prime_handle,
                                         init_info_ptr->sec_handle,
                                         hash_data_ptr->keys,
                                         entry_offset,
                                         init_info_ptr->key_size,
                                         key
                                         );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }
  else
  {
      res = SOC_DPP_WB_ENGINE_SET_ARR(init_info_ptr->prime_handle, init_info_ptr->wb_var_index + WB_ENGINE_HASH_TABLE_KEYS, 
                                      key, 
                                      entry_offset);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);
      
  }

  *success = TRUE;
  *data_indx = entry_offset;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_hash_table_entry_add()",0,0);
}


uint32
  soc_sand_hash_table_entry_add_at_index(
    SOC_SAND_IN     int                          unit,
    SOC_SAND_INOUT  SOC_SAND_HASH_TABLE_INFO    *hash_table,
    SOC_SAND_IN     SOC_SAND_HASH_TABLE_KEY*     const key,
    SOC_SAND_IN     uint32                       data_indx,
    SOC_SAND_OUT    uint8                       *success
  )
{
  SOC_SAND_HASH_TABLE_INIT_INFO
    *init_info_ptr;
  SOC_SAND_HASH_TABLE_T
    *hash_data_ptr;
  uint8
    found,
    indx_in_use,
    first;
  uint32
    entry_offset,
    prev_entry;
  uint32
    old_index;
  uint32
    res;
  SOC_SAND_HASH_MAP_SW_DB_ENTRY_SET 
    set_entry_fun;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_HASH_TABLE_ENTRY_ADD);

  SOC_SAND_CHECK_NULL_INPUT(hash_table);
  SOC_SAND_CHECK_NULL_INPUT(key);
  SOC_SAND_CHECK_NULL_INPUT(success);

  init_info_ptr = &(hash_table->init_info);
  hash_data_ptr = &(hash_table->hash_data);

  if (init_info_ptr->set_entry_fun) {
      set_entry_fun = init_info_ptr->set_entry_fun;
  }
  else
  {
      set_entry_fun = sand_hashtable_default_set_entry_fun;
  }

  /* check if this key already exist */
  res = soc_sand_hash_table_entry_lookup(
          unit,
          hash_table,
          key,
          &old_index,
          &found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  if (found && old_index != data_indx)
  {
    *success = FALSE;
    goto exit;
  }
  if (found && old_index == data_indx)
  {
    /* found in required index, done */
    *success = TRUE;
    goto exit;
  }

  res = soc_sand_occ_bm_is_occupied(
          unit,
          hash_data_ptr->memory_use,
          data_indx,
          &indx_in_use
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  if (indx_in_use && !found)
  {
    /* index already in use for other usage */
    *success = FALSE;
    goto exit;
  }
  
  entry_offset = data_indx;
  /* check to see if the entry exists */
  res = soc_sand_hash_table_find_entry(
          unit,
          hash_table,
          key,
          TRUE,
          TRUE,
          &entry_offset,
          &found,
          &prev_entry,
          &first
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (entry_offset == SOC_SAND_HASH_TABLE_NULL)
  {
    *success = FALSE;
    goto exit;
  }

  if(SOC_DPP_WB_ENGINE_VAR_NONE == init_info_ptr->wb_var_index) 
  {
      res = set_entry_fun(
                                         init_info_ptr->prime_handle,
                                         init_info_ptr->sec_handle,
                                         hash_data_ptr->keys,
                                         entry_offset,
                                         init_info_ptr->key_size,
                                         key
                                         );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  }
  else
  {
      res = SOC_DPP_WB_ENGINE_SET_ARR(init_info_ptr->prime_handle, init_info_ptr->wb_var_index + WB_ENGINE_HASH_TABLE_KEYS, 
                                      key, 
                                      entry_offset);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);      
  }
  *success = TRUE;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_hash_table_entry_add_at_index()",0,0);
}
/*********************************************************************
* NAME:
*     soc_sand_hash_table_entry_remove
* TYPE:
*   PROC
* DATE:
*   Mar  6 2008
* FUNCTION:
*  Remove an entry from a hash table, if the key is not exist then
*  the operation has no effect.
* INPUT:
*  SOC_SAND_IN  int                        unit -
*     Identifier of the device to access.
*  SOC_SAND_INOUT  SOC_SAND_HASH_TABLE_PTR hash_table -
*     The hash table to add to.
*  SOC_SAND_IN  SOC_SAND_HASH_TABLE_KEY    key -
*     The key to remove from the hash table
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_sand_hash_table_entry_remove(
    SOC_SAND_IN     int                            unit,
    SOC_SAND_INOUT  SOC_SAND_HASH_TABLE_INFO      *hash_table,
    SOC_SAND_IN     SOC_SAND_HASH_TABLE_KEY*       const key
  )
{
  SOC_SAND_HASH_TABLE_INIT_INFO
    *init_info_ptr;
  SOC_SAND_HASH_TABLE_T
    *hash_data_ptr;
  uint8
    found,
    first;
  uint32
    entry_offset,
    prev_entry;
  uint8
    *next_entry;
  uint32
    res;
  SOC_SAND_HASH_MAP_SW_DB_ENTRY_GET 
    get_entry_fun;
  SOC_SAND_HASH_MAP_SW_DB_ENTRY_SET 
    set_entry_fun;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_HASH_TABLE_ENTRY_REMOVE);

  SOC_SAND_CHECK_NULL_INPUT(hash_table);
  SOC_SAND_CHECK_NULL_INPUT(key);

  init_info_ptr = &(hash_table->init_info);
  hash_data_ptr = &(hash_table->hash_data);

  if (init_info_ptr->get_entry_fun) {
      get_entry_fun = init_info_ptr->get_entry_fun;
  }
  else
  {
      get_entry_fun = sand_hashtable_default_get_entry_fun;
  }
  if (init_info_ptr->set_entry_fun) {
      set_entry_fun = init_info_ptr->set_entry_fun;
  }
  else
  {
      set_entry_fun = sand_hashtable_default_set_entry_fun;
  }

  /* check to see if the entry exists */
  res = soc_sand_hash_table_find_entry(
          unit,
          hash_table,
          key,
          FALSE,
          FALSE,
          &entry_offset,
          &found,
          &prev_entry,
          &first
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

 /*
  * if the key to remove is not exist in the hash table then this operation has
  * no side effect.
  */
  if (!found)
  {
    goto exit;
  }
 /* remove node from linked list */

 /*
  * get next pointer
  */
  res = get_entry_fun(
          init_info_ptr->prime_handle,
          init_info_ptr->sec_handle,
          hash_data_ptr->next,
          entry_offset,
          hash_data_ptr->ptr_size,
          hash_data_ptr->tmp_buf
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  next_entry = hash_data_ptr->tmp_buf;
 /*
  * if this is first node in the linked list then set head list
  */
  if (first)
  {
      if(SOC_DPP_WB_ENGINE_VAR_NONE == init_info_ptr->wb_var_index) 
      {
          res = set_entry_fun(
                                             init_info_ptr->prime_handle,
                                             init_info_ptr->sec_handle,
                                             hash_data_ptr->lists_head,
                                             prev_entry,
                                             hash_data_ptr->ptr_size,
                                             next_entry
                                             );
          SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
      }
      else
      {
          res = SOC_DPP_WB_ENGINE_SET_ARR(init_info_ptr->prime_handle, init_info_ptr->wb_var_index + WB_ENGINE_HASH_TABLE_LISTS_HEAD, 
                                          next_entry, 
                                          prev_entry);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);      
      }
  }
  else
  {
      if(SOC_DPP_WB_ENGINE_VAR_NONE == init_info_ptr->wb_var_index) 
      {
          res = set_entry_fun(
                                             init_info_ptr->prime_handle,
                                             init_info_ptr->sec_handle,
                                             hash_data_ptr->next,
                                             prev_entry,
                                             hash_data_ptr->ptr_size,
                                             next_entry
                                             );
          SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
      }
      else
      {
          res = SOC_DPP_WB_ENGINE_SET_ARR(init_info_ptr->prime_handle, init_info_ptr->wb_var_index + WB_ENGINE_HASH_TABLE_NEXT, 
                                          next_entry, 
                                          prev_entry);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 60, exit);      
      }
  }
 /*
  * now the entry 'entry_offset' is not in use. free it.
  */
  res = soc_sand_occ_bm_occup_status_set(
          unit,
          hash_data_ptr->memory_use,
          entry_offset,
          FALSE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

  res = soc_sand_U32_to_U8(
           &(hash_data_ptr->null_ptr),
           hash_data_ptr->ptr_size,
           hash_data_ptr->tmp_buf
                           );
  SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

  if(SOC_DPP_WB_ENGINE_VAR_NONE == init_info_ptr->wb_var_index) 
  {
      res = set_entry_fun(
                                         init_info_ptr->prime_handle,
                                         init_info_ptr->sec_handle,
                                         hash_data_ptr->next,
                                         entry_offset,
                                         hash_data_ptr->ptr_size,
                                         hash_data_ptr->tmp_buf
                                         );
      SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);
  }
  else
  {
      res = SOC_DPP_WB_ENGINE_SET_ARR(init_info_ptr->prime_handle, init_info_ptr->wb_var_index + WB_ENGINE_HASH_TABLE_NEXT, 
                                      hash_data_ptr->tmp_buf, 
                                      entry_offset);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 100, exit);      
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_hash_table_entry_remove()",0,0);
}

/*********************************************************************
* NAME:
*     soc_sand_hash_table_entry_remove_by_index
* TYPE:
*   PROC
* DATE:
*   Mar  6 2008
* FUNCTION:
*  Remove an entry from a hash table given the index identify
*  this entry.
*  SOC_SAND_IN  int                        unit -
*     Identifier of the device to access.
* INPUT:
*  SOC_SAND_INOUT  SOC_SAND_HASH_TABLE_PTR hash_table -
*     The hash table to add to.
*  SOC_SAND_IN  uint32                     indx -
*     index of the entry to remove.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_sand_hash_table_entry_remove_by_index(
    SOC_SAND_IN     int                        unit,
    SOC_SAND_INOUT  SOC_SAND_HASH_TABLE_INFO  *hash_table,
    SOC_SAND_IN     uint32                     data_indx
  )
{
  SOC_SAND_HASH_TABLE_INIT_INFO
    *init_info_ptr;
  SOC_SAND_HASH_TABLE_T
    *hash_data_ptr;
  uint8
    *cur_key;
  uint8
    in_use;
  uint32
    res;
  SOC_SAND_HASH_MAP_SW_DB_ENTRY_GET 
    get_entry_fun;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_HASH_TABLE_ENTRY_REMOVE_BY_INDEX);

  SOC_SAND_CHECK_NULL_INPUT(hash_table);

  init_info_ptr = &(hash_table->init_info);
  hash_data_ptr = &(hash_table->hash_data);

  if (init_info_ptr->get_entry_fun) {
      get_entry_fun = init_info_ptr->get_entry_fun;
  }
  else
  {
      get_entry_fun = sand_hashtable_default_get_entry_fun;
  }

  if (data_indx > init_info_ptr->table_size)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_VALUE_OUT_OF_RANGE_ERR, 10, exit);
  }

 /*
  * check if the entry 'entry_offset' is in use.
  */
  res = soc_sand_occ_bm_is_occupied(
          unit,
          hash_data_ptr->memory_use,
          data_indx,
          &in_use
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
 /*
  * check if the entry 'entry_offset' is in use.
  */
  if (!in_use)
  {
    goto exit;
  }
  res = get_entry_fun(
          init_info_ptr->prime_handle,
          init_info_ptr->sec_handle,
          hash_data_ptr->keys,
          data_indx,
          init_info_ptr->key_size,
          hash_data_ptr->tmp_buf2/*&cur_key*/
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);


  cur_key = hash_data_ptr->tmp_buf2;

  res = soc_sand_hash_table_entry_remove(
          unit,
          hash_table,
          cur_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_hash_table_entry_remove_by_index()",data_indx,(hash_table)?hash_table->init_info.table_size: 0);
}

/*********************************************************************
* NAME:
*     soc_sand_hash_table_entry_lookup
* TYPE:
*   PROC
* DATE:
*   Mar  6 2008
* FUNCTION:
*  lookup in the hash table for the given key and return the data inserted with
*  the given key.
* INPUT:
*   SOC_SAND_IN  int                           unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_SAND_HASH_TABLE_PTR        hash_table -
*     The hash table.
*   SOC_SAND_IN  SOC_SAND_HASH_TABLE_KEY       key -
*     The key to lookup
*   SOC_SAND_OUT  SOC_SAND_HASH_TABLE_DATA    *data -
*     the data associated with the given key, valid only found is true.
*   SOC_SAND_IN  uint8                        *found -
*     whether the key was found in the hash table
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_sand_hash_table_entry_lookup(
    SOC_SAND_IN     int                          unit,
    SOC_SAND_INOUT  SOC_SAND_HASH_TABLE_INFO    *hash_table,
    SOC_SAND_IN     SOC_SAND_HASH_TABLE_KEY*     const key,
    SOC_SAND_OUT    uint32                      *data_indx,
    SOC_SAND_OUT    uint8                       *found
  )
{
  uint8
    is_found,
    first;
  uint32
    entry_offset,
    prev_entry;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_HASH_TABLE_ENTRY_LOOKUP);

  SOC_SAND_CHECK_NULL_INPUT(hash_table);
  SOC_SAND_CHECK_NULL_INPUT(key);
  SOC_SAND_CHECK_NULL_INPUT(data_indx);
  SOC_SAND_CHECK_NULL_INPUT(found);


  /* check to see if the entry exists */
  res = soc_sand_hash_table_find_entry(
          unit,
          hash_table,
          key,
          FALSE,
          FALSE,
          &entry_offset,
          &is_found,
          &prev_entry,
          &first
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
 /*
  * if the key to remove is not exist in the hash table then this operation has
  * no side effect.
  */
  if (!is_found)
  {
    *found = FALSE;
    *data_indx = SOC_SAND_HASH_TABLE_NULL;
    goto exit;
  }
  *found = TRUE;
  *data_indx = entry_offset;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_hash_table_entry_lookup()",0,0);
}

/*********************************************************************
* NAME:
*     soc_sand_hash_table_entry_get_by_index
* TYPE:
*   PROC
* DATE:
*   Mar  6 2008
* FUNCTION:
*  return an entry from a hash table given the index identify
*  this entry.
* INPUT:
*  SOC_SAND_IN  int                        unit -
*     Identifier of the device to access.
*  SOC_SAND_INOUT  SOC_SAND_HASH_TABLE_PTR hash_table -
*     The hash table to add to.
*  SOC_SAND_IN  uint32                     data_indx -
*     index of the entry to return.
*  SOC_SAND_OUT  SOC_SAND_HASH_TABLE_KEY*  const key -
*     key resides in this entry
*  SOC_SAND_OUT  uint8                    *found -
*     whether this key is valid
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_sand_hash_table_get_by_index(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_SAND_HASH_TABLE_INFO      *hash_table,
    SOC_SAND_IN  uint32                         data_indx,
    SOC_SAND_OUT SOC_SAND_HASH_TABLE_KEY*       key,
    SOC_SAND_OUT uint8                         *found
  )
{
  SOC_SAND_IN SOC_SAND_HASH_TABLE_INIT_INFO
    *init_info_ptr;
  SOC_SAND_IN SOC_SAND_HASH_TABLE_T
    *hash_data_ptr;
  uint32
    res;
  SOC_SAND_HASH_MAP_SW_DB_ENTRY_GET 
    get_entry_fun;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_HASH_TABLE_ENTRY_LOOKUP);

  SOC_SAND_CHECK_NULL_INPUT(hash_table);

  init_info_ptr = &(hash_table->init_info);
  hash_data_ptr = &(hash_table->hash_data);

  if (init_info_ptr->get_entry_fun) {
      get_entry_fun = init_info_ptr->get_entry_fun;
  }
  else
  {
      get_entry_fun = sand_hashtable_default_get_entry_fun;
  }

  if (data_indx > init_info_ptr->table_size)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_VALUE_OUT_OF_RANGE_ERR, 10, exit);
  }
 /*
  * check if the entry 'entry_offset' is in use.
  */
  res = soc_sand_occ_bm_is_occupied(
          unit,
          hash_data_ptr->memory_use,
          data_indx,
          found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
 /*
  * check if the entry 'entry_offset' is in use.
  */
  if (!*found)
  {
    goto exit;
  }
  res = get_entry_fun(
          init_info_ptr->prime_handle,
          init_info_ptr->sec_handle,
          hash_data_ptr->keys,
          data_indx,
          init_info_ptr->key_size,
          key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_hash_table_entry_get_by_index()",data_indx,(hash_table)?hash_table->init_info.table_size: 0);
}

/*********************************************************************
* NAME:
*     soc_sand_hash_table_clear
* TYPE:
*   PROC
* DATE:
*   Mar  6 2008
* FUNCTION:
*     Clear the hash table content without free the memory
* INPUT:
*   SOC_SAND_IN     int                           unit -
*     Identifier of the device to access.
*   SOC_SAND_INOUT  SOC_SAND_HASH_TABLE_INFO     *hash_table -
*     the hash table instance
* REMARKS:
*   None.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_sand_hash_table_clear(
    SOC_SAND_IN     int                           unit,
    SOC_SAND_INOUT  SOC_SAND_HASH_TABLE_INFO     *hash_table
  )
{
  SOC_SAND_HASH_TABLE_INIT_INFO
    *init_info_ptr;
  SOC_SAND_HASH_TABLE_T
    *hash_data_ptr;
  uint32
    max_buf_size;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_HASH_TABLE_CREATE);

  SOC_SAND_CHECK_NULL_INPUT(hash_table);

  init_info_ptr = &(hash_table->init_info);
  hash_data_ptr = &(hash_table->hash_data);



  if(SOC_DPP_WB_ENGINE_VAR_NONE == init_info_ptr->wb_var_index) 
  {

      soc_sand_os_memset(
                         hash_data_ptr->keys,
                         0x0,
                         init_info_ptr->table_size * init_info_ptr->key_size * sizeof(uint8)
                         );

      /*
       * clear buffer for next array (to build the linked list)
       */
      soc_sand_os_memset(
                         hash_data_ptr->next,
                         0xFF,
                         init_info_ptr->table_size * hash_data_ptr->ptr_size * sizeof(uint8)
                         );
      /*
       * clear buffer for lists_head (to build the linked list)
       */
      soc_sand_os_memset(
                         hash_data_ptr->lists_head,
                         0xFF,
                         init_info_ptr->table_width * hash_data_ptr->ptr_size * sizeof(uint8)
                         );
  }
  else
  {
      res = SOC_DPP_WB_ENGINE_MEMSET_ARR(init_info_ptr->prime_handle, init_info_ptr->wb_var_index + WB_ENGINE_HASH_TABLE_KEYS, 0);
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);                

      res = SOC_DPP_WB_ENGINE_MEMSET_ARR(init_info_ptr->prime_handle, init_info_ptr->wb_var_index + WB_ENGINE_HASH_TABLE_NEXT, 0xFF);
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);                

      res = SOC_DPP_WB_ENGINE_MEMSET_ARR(init_info_ptr->prime_handle, init_info_ptr->wb_var_index + WB_ENGINE_HASH_TABLE_LISTS_HEAD, 0xFF);
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);                

  }
  max_buf_size = init_info_ptr->data_size;
  if (init_info_ptr->data_size > max_buf_size)
  {
    max_buf_size = init_info_ptr->data_size;
  }
  if (init_info_ptr->key_size > max_buf_size)
  {
    max_buf_size = init_info_ptr->key_size;
  }
  if (hash_data_ptr->ptr_size > max_buf_size)
  {
    max_buf_size = hash_data_ptr->ptr_size;
  }
  soc_sand_os_memset(
    hash_data_ptr->tmp_buf,
    0x0,
    max_buf_size * sizeof(uint8)
  );
  soc_sand_os_memset(
    hash_data_ptr->tmp_buf2,
    0x0,
    max_buf_size * sizeof(uint8)
  );

 /* initialize the data to be mapped to*/

  res = soc_sand_occ_bm_clear(
          unit,
          hash_data_ptr->memory_use
        );
  SOC_SAND_CHECK_FUNC_RESULT(res,40, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_hash_table_clear()",0,0);
}

/*********************************************************************
* NAME:
*     soc_sand_hash_table_get_next
* TYPE:
*   PROC
* DATE:
*   Mar  6 2008
* FUNCTION:
*  get the next valid entry (key and data) in the hash table.
*  start traversing from the place pointed by the given iterator.
* INPUT:
*  SOC_SAND_IN  SOC_SAND_HASH_TABLE_PTR        hash_table -
*     The hash table.
*  SOC_SAND_INOUT  SOC_SAND_HASH_TABLE_ITER    *iter
*     iterator points to the entry to start traverse from.
*   SOC_SAND_OUT  SOC_SAND_HASH_TABLE_KEY   key -
*     the hash table key returned
*   SOC_SAND_OUT  SOC_SAND_HASH_TABLE_DATA  data -
*     the hash table data returned and associated with the key above.
* REMARKS:
*     - to start traverse the hash table from the beginning.
*       use SOC_SAND_HASH_TABLE_ITER_BEGIN(iter)
*     - to check if the iterator get to the end of the table use.
*       use SOC_SAND_HASH_TABLE_ITER_END(iter)
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_sand_hash_table_get_next(
    SOC_SAND_IN     int                          unit,
    SOC_SAND_IN     SOC_SAND_HASH_TABLE_INFO    *hash_table,
    SOC_SAND_INOUT  SOC_SAND_HASH_TABLE_ITER    *iter,
    SOC_SAND_OUT    SOC_SAND_HASH_TABLE_KEY*     const key,
    SOC_SAND_OUT    uint32                      *data_indx
  )
{
  SOC_SAND_IN SOC_SAND_HASH_TABLE_INIT_INFO
    *init_info_ptr;
  SOC_SAND_IN SOC_SAND_HASH_TABLE_T
    *hash_data_ptr;
  uint32
    indx;
  uint8
    occupied;
  uint32
    res;
  SOC_SAND_HASH_MAP_SW_DB_ENTRY_GET 
    get_entry_fun;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_HASH_TABLE_GET_NEXT);

  SOC_SAND_CHECK_NULL_INPUT(hash_table);
  SOC_SAND_CHECK_NULL_INPUT(iter);
  SOC_SAND_CHECK_NULL_INPUT(data_indx);

  init_info_ptr = &(hash_table->init_info);
  hash_data_ptr = &(hash_table->hash_data);

  if (init_info_ptr->get_entry_fun) {
      get_entry_fun = init_info_ptr->get_entry_fun;
  }
  else
  {
      get_entry_fun = sand_hashtable_default_get_entry_fun;
  }

 /*
  * traverse the hash table head list.
  */
  for (indx = *iter; indx < init_info_ptr->table_size   ; ++indx)
  {
    res = soc_sand_occ_bm_is_occupied(
            unit,
            hash_data_ptr->memory_use,
            indx,
            &occupied
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    if (occupied)
    {
      *data_indx = indx;
      *iter = indx + 1;

      res = get_entry_fun(
              init_info_ptr->prime_handle,
              init_info_ptr->sec_handle,
              hash_data_ptr->keys,
              indx,
              init_info_ptr->key_size,
              key
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

      goto exit;
    }
  }
  *iter = SOC_SAND_U32_MAX;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_hash_table_get_next()",0,0);
}

uint32
  soc_sand_hash_table_get_size_for_save(
    SOC_SAND_IN   int                             unit,
    SOC_SAND_IN   SOC_SAND_HASH_TABLE_INFO       *hash_table,
    SOC_SAND_OUT  uint32                         *size
  )
{
  const SOC_SAND_HASH_TABLE_INIT_INFO
    *init_info_ptr;
  const SOC_SAND_HASH_TABLE_T
    *hash_data_ptr;
  uint32
    bmp_size,
    total_size=0;
  uint32
    res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(hash_table);
  SOC_SAND_CHECK_NULL_INPUT(size);

  init_info_ptr = &(hash_table->init_info);
  hash_data_ptr = &(hash_table->hash_data);

  /* init info */
  total_size += sizeof(SOC_SAND_HASH_TABLE_INIT_INFO);

  /* DS data */
 
  total_size += sizeof(uint8) * init_info_ptr->table_size * init_info_ptr->key_size;

  total_size += sizeof(uint8) * init_info_ptr->table_size * hash_data_ptr->ptr_size;

  total_size += sizeof(uint8) * init_info_ptr->table_width * hash_data_ptr->ptr_size;

  /* initialize the data to be mapped to*/
  res = soc_sand_occ_bm_get_size_for_save(
          unit,
          hash_data_ptr->memory_use,
          &bmp_size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res,20, exit);
  total_size += bmp_size;

  *size = total_size;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_hash_table_get_size_for_save()",0,0);
}

/*********************************************************************
* NAME:
*     soc_sand_hash_table_save
* TYPE:
*   PROC
* DATE:
*   Mar  6 2008
* FUNCTION:
*     saves the given hash table in the given buffer
* INPUT:
*   SOC_SAND_IN  int                     unit -
*     Identifier of the device to access.
*   SOC_SAND_IN  SOC_SAND_HASH_TABLE_PTR hash_table -
*     The Hash table to save.
*   SOC_SAND_OUT  uint8                 *buffer -
*     buffer to include the hast table
* REMARKS:
*   - the size of the buffer has to be at least as the value returned
*     by soc_sand_hash_table_get_size_for_save.
*   - the hash and rehash functions are not saved.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_sand_hash_table_save(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_SAND_HASH_TABLE_INFO     *hash_table,
    SOC_SAND_OUT  uint8                 *buffer,
    SOC_SAND_IN  uint32                 buffer_size_bytes,
    SOC_SAND_OUT uint32                 *actual_size_bytes
  )
{
  const SOC_SAND_HASH_TABLE_INIT_INFO
    *init_info_ptr;
  const SOC_SAND_HASH_TABLE_T
    *hash_data_ptr;
  uint8
    *cur_ptr = (uint8*)buffer;
  uint32
    cur_size,
    total_size=0;
  uint32
    res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(hash_table);
  SOC_SAND_CHECK_NULL_INPUT(buffer);
  SOC_SAND_CHECK_NULL_INPUT(actual_size_bytes);

  init_info_ptr = &(hash_table->init_info);
  hash_data_ptr = &(hash_table->hash_data);

  /* copy init info */
  SOC_SAND_COPY_TO_BUFF_AND_INC(cur_ptr,init_info_ptr,SOC_SAND_HASH_TABLE_INIT_INFO,1);

  /* copy DS data */
 
  SOC_SAND_COPY_TO_BUFF_AND_INC(cur_ptr,hash_data_ptr->keys,uint8,init_info_ptr->table_size * init_info_ptr->key_size);

  SOC_SAND_COPY_TO_BUFF_AND_INC(cur_ptr,hash_data_ptr->next,uint8,init_info_ptr->table_size * hash_data_ptr->ptr_size);

  SOC_SAND_COPY_TO_BUFF_AND_INC(cur_ptr,hash_data_ptr->lists_head,uint8,init_info_ptr->table_width * hash_data_ptr->ptr_size);

  /* initialize the data to be mapped to*/
  res = soc_sand_occ_bm_save(
          unit,
          hash_data_ptr->memory_use,
          cur_ptr,
          buffer_size_bytes - total_size,
          &cur_size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res,20, exit);
  cur_ptr += cur_size;
  total_size += cur_size;

  *actual_size_bytes = total_size;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_hash_table_save()",0,0);
}


uint32
  soc_sand_hash_table_load(
    SOC_SAND_IN  int                                   unit,
    SOC_SAND_IN  uint8                               **buffer,
    SOC_SAND_IN  SOC_SAND_HASH_MAP_SW_DB_ENTRY_SET     set_entry_fun,
    SOC_SAND_IN  SOC_SAND_HASH_MAP_SW_DB_ENTRY_GET     get_entry_fun,
    SOC_SAND_IN  SOC_SAND_HASH_MAP_HASH_FUN_CALL_BACK  hash_function,
    SOC_SAND_IN  SOC_SAND_HASH_MAP_HASH_FUN_CALL_BACK  rehash_function,
    SOC_SAND_OUT  SOC_SAND_HASH_TABLE_INFO             *hash_table
  )
{
  const SOC_SAND_HASH_TABLE_INIT_INFO
    *init_info_ptr;
  SOC_SAND_HASH_TABLE_T
    *hash_data_ptr;
  SOC_SAND_IN uint8
    *cur_ptr = (SOC_SAND_IN uint8*)buffer[0];
  uint32
    res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(hash_table);
  SOC_SAND_CHECK_NULL_INPUT(buffer);

  soc_sand_SAND_HASH_TABLE_INFO_clear(hash_table);

  /* copy init info */
  soc_sand_os_memcpy(&(hash_table->init_info), cur_ptr, sizeof(SOC_SAND_HASH_TABLE_INIT_INFO));
  cur_ptr += sizeof(SOC_SAND_HASH_TABLE_INIT_INFO);
  hash_table->init_info.hash_function = hash_function;
  hash_table->init_info.rehash_function = rehash_function;
  hash_table->init_info.set_entry_fun = set_entry_fun;
  hash_table->init_info.get_entry_fun = get_entry_fun;

  /* create DS*/

  /* will not work!! (petra only code) */
  res = soc_sand_hash_table_create(unit, &hash_table, hash_table->init_info);
  SOC_SAND_CHECK_FUNC_RESULT(res,20, exit);

  init_info_ptr = &(hash_table->init_info);
  hash_data_ptr = &(hash_table->hash_data);
  /* copy DS data */

  soc_sand_os_memcpy(hash_data_ptr->keys, cur_ptr, init_info_ptr->table_size * init_info_ptr->key_size);
  cur_ptr += init_info_ptr->table_size * init_info_ptr->key_size;

  soc_sand_os_memcpy(hash_data_ptr->next, cur_ptr, init_info_ptr->table_size * hash_data_ptr->ptr_size);
  cur_ptr += init_info_ptr->table_size * hash_data_ptr->ptr_size;

  soc_sand_os_memcpy(hash_data_ptr->lists_head, cur_ptr, init_info_ptr->table_width * hash_data_ptr->ptr_size);
  cur_ptr += init_info_ptr->table_width * hash_data_ptr->ptr_size;

  /* load bitmap*/
  res = soc_sand_occ_bm_destroy(
          unit,
          hash_data_ptr->memory_use
        );
  SOC_SAND_CHECK_FUNC_RESULT(res,30, exit);

  /* load bitmap*/
  res = soc_sand_occ_bm_load(
          unit,
          &cur_ptr,
          &hash_data_ptr->memory_use
        );
  SOC_SAND_CHECK_FUNC_RESULT(res,30, exit);

  *buffer = cur_ptr;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_hash_table_load()",0,0);
}

void
  soc_sand_SAND_HASH_TABLE_INFO_clear(
    SOC_SAND_OUT SOC_SAND_HASH_TABLE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_memset(info, 0x0, sizeof(SOC_SAND_HASH_TABLE_INFO));
  info->init_info.data_size       = 0;
  info->init_info.get_entry_fun   = 0;
  info->init_info.hash_function   = 0;
  info->init_info.key_size        = 0;
  info->init_info.rehash_function = 0;
  info->init_info.set_entry_fun   = 0;
  info->init_info.table_size      = 0;
  info->init_info.table_width     = 0;
  info->init_info.wb_var_index    = SOC_DPP_WB_ENGINE_VAR_NONE;

  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_SAND_DEBUG

/*********************************************************************
* NAME:
*     soc_sand_hash_table_print
* TYPE:
*   PROC
* DATE:
*   Mar  6 2008
* FUNCTION:
*     prints the hash table content, all entries including entries not in use.
* INPUT:
*  SOC_SAND_IN  SOC_SAND_HASH_TABLE_PTR hash_table -
*     The hash table to print.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_sand_hash_table_print(
    SOC_SAND_IN  SOC_SAND_HASH_TABLE_INFO  *hash_table
  )
{
  SOC_SAND_IN SOC_SAND_HASH_TABLE_INIT_INFO
    *init_info_ptr;
  SOC_SAND_IN SOC_SAND_HASH_TABLE_T
    *hash_data_ptr;
  uint32
    indx;
  uint32
    ptr_long,
    print_indx;
  uint8
    *list_head,
    *cur_key,
    *next;
  uint32
    res;
  SOC_SAND_HASH_MAP_SW_DB_ENTRY_GET 
    get_entry_fun;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(SOC_SAND_HASH_TABLE_PRINT);

  SOC_SAND_CHECK_NULL_INPUT(hash_table);

  init_info_ptr = &(hash_table->init_info);
  hash_data_ptr = &(hash_table->hash_data);

  if (init_info_ptr->get_entry_fun) {
      get_entry_fun = init_info_ptr->get_entry_fun;
  }
  else
  {
      get_entry_fun = sand_hashtable_default_get_entry_fun;
  }

 /*
  * traverse the hash table head list.
  */
  for (indx = 0; indx < hash_table->init_info.table_width   ; ++indx)
  {
    res = get_entry_fun(
            init_info_ptr->prime_handle,
            init_info_ptr->sec_handle,
            hash_data_ptr->lists_head,
            indx,
            hash_data_ptr->ptr_size,
            hash_data_ptr->tmp_buf
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    list_head = hash_data_ptr->tmp_buf;


    ptr_long = 0;

    res = soc_sand_U8_to_U32(
            list_head,
            hash_data_ptr->ptr_size,
            &ptr_long
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    if (ptr_long == hash_data_ptr->null_ptr)
    {
      continue;
    }
    LOG_CLI((BSL_META_U(unit,
                        " entry %u:  "), indx));

    while (ptr_long != hash_data_ptr->null_ptr)
    {
     /*
      * read keys
      */
      res = get_entry_fun(
              init_info_ptr->prime_handle,
              init_info_ptr->sec_handle,
              hash_data_ptr->keys,
              ptr_long,
              init_info_ptr->key_size,
              hash_data_ptr->tmp_buf
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

      cur_key = hash_data_ptr->tmp_buf;

      LOG_CLI((BSL_META_U(unit,
                          "(0x")));
       for(print_indx = 0; print_indx < init_info_ptr->key_size; ++print_indx)
       {
          LOG_CLI((BSL_META_U(unit,
                              "%02x"), cur_key[init_info_ptr->key_size - print_indx - 1]));
       }
       LOG_CLI((BSL_META_U(unit,
                           "--> %u)"), ptr_long - 1));

       LOG_CLI((BSL_META_U(unit,
                           "\t")));

     /*
      * get next node
      */
      res = get_entry_fun(
              init_info_ptr->prime_handle,
              init_info_ptr->sec_handle,
              hash_data_ptr->next,
              ptr_long,
              hash_data_ptr->ptr_size,
              hash_data_ptr->tmp_buf
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

      next = hash_data_ptr->tmp_buf;

      ptr_long = 0;

      res = soc_sand_U8_to_U32(
              next,
              hash_data_ptr->ptr_size,
              &ptr_long
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

      LOG_CLI((BSL_META_U(unit,
                          "ptr_long--> %u)"), ptr_long ));
    }
    LOG_CLI((BSL_META_U(unit,
                        "\n")));

  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_hash_table_print()",0,0);
}



void
  soc_sand_SAND_HASH_TABLE_INFO_print(
    SOC_SAND_IN SOC_SAND_HASH_TABLE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "info->init_info.data_size: %u\n"),info->init_info.data_size));
  LOG_CLI((BSL_META_U(unit,
                      "info->init_info.key_size: %u\n"),info->init_info.key_size));
  LOG_CLI((BSL_META_U(unit,
                      "info->init_info.table_size: %u\n"),info->init_info.table_size));
  LOG_CLI((BSL_META_U(unit,
                      "info->init_info.table_width: %u\n"),info->init_info.table_width));


exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}



/*****************************************************
*NAME
*  soc_sand_hash_table_TEST_1
*TYPE:
*  PROC
*DATE:
*  15-Feb-2008
*FUNCTION:
* Verification add and lookup operation act well in a simple test
* 1.  Create hash table
* 2.  Add key and data to the hash table and lookup each time
*     Expect to find the key and the data in each lookup
*
*INPUT:
*  SOC_SAND_DIRECT:
*    SOC_SAND_IN uint32 silent -
*    Indicator.
*    1 - Do not print debuging info.
*    0 - Print various debuging info.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    uint32 -
*    Indicator.
*    1 - Test pass.
*    0 - Test fail.
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*
*SEE ALSO:
*****************************************************/
#endif /* SOC_SAND_DEBUG */


/*********************************************************************
* NAME:
*     soc_sand_hash_table_find_entry
* TYPE:
*   PROC
* DATE:
*   Mar  6 2008
* FUNCTION:
*  lookup in the hash table for the given key and return the data inserted with
*  the given key.
* INPUT:
*  SOC_SAND_IN  int                            unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_SAND_HASH_TABLE_PTR        hash_table -
*     The hash table.
*   SOC_SAND_IN  SOC_SAND_HASH_TABLE_KEY       key -
*     The key to lookup for
*   SOC_SAND_IN  uint8                         get_first_empty
*     whether to return the first empty entry .
*   SOC_SAND_OUT  uint32                      *entry -
*     if the key is present in the hash table then return the entry the key found at,
*     otherwise it returns the place where the key suppose to be.
*   SOC_SAND_IN  uint8                        *found -
*     whether the key was found in the hash table
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
STATIC uint32
  soc_sand_hash_table_find_entry(
    SOC_SAND_IN     int                          unit,
    SOC_SAND_INOUT  SOC_SAND_HASH_TABLE_INFO    *hash_table,
    SOC_SAND_IN     SOC_SAND_HASH_TABLE_KEY*     const key,
    SOC_SAND_IN     uint8                        get_first_empty,
    SOC_SAND_IN     uint8                        alloc_by_index,
    SOC_SAND_OUT    uint32                      *entry,
    SOC_SAND_OUT    uint8                       *found,
    SOC_SAND_OUT    uint32                      *prev_entry,
    SOC_SAND_OUT    uint8                       *first
  )
{
    SOC_SAND_HASH_TABLE_INIT_INFO
        *init_info_ptr;
    SOC_SAND_HASH_TABLE_T
        *hash_data_ptr;
    uint8
        *cur_key,
        *next;
    uint32
        hash_val,
        ptr_long,
        next_node;
    uint8
        not_found,
        found_new;
    uint32
        res;
    SOC_SAND_HASH_MAP_SW_DB_ENTRY_GET 
        get_entry_fun;
    SOC_SAND_HASH_MAP_SW_DB_ENTRY_SET 
        set_entry_fun;

    SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_HASH_TABLE_FIND_ENTRY);

    SOC_SAND_CHECK_NULL_INPUT(hash_table);
    SOC_SAND_CHECK_NULL_INPUT(key);
    SOC_SAND_CHECK_NULL_INPUT(found);

    init_info_ptr = &(hash_table->init_info);
    hash_data_ptr = &(hash_table->hash_data);

    if (init_info_ptr->get_entry_fun) {
        get_entry_fun = init_info_ptr->get_entry_fun;
    }
    else
    {
        get_entry_fun = sand_hashtable_default_get_entry_fun;
    }
    if (init_info_ptr->set_entry_fun) {
        set_entry_fun = init_info_ptr->set_entry_fun;
    }
    else
    {
        set_entry_fun = sand_hashtable_default_set_entry_fun;
    }

    ptr_long = 0;

    *prev_entry = SOC_SAND_HASH_TABLE_NULL;
    *first = TRUE;
    /*
     * hash the key to get list head
     */
    res = init_info_ptr->hash_function(
                                       unit,
                                       (SOC_SAND_HASH_TABLE_PTR_WORKAROUND) hash_table,
                                       key,
                                       0,
                                       &hash_val
                                       );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    *prev_entry = hash_val;


    /*
     * read list head.
     */
    res = get_entry_fun(
                                       init_info_ptr->prime_handle,
                                       init_info_ptr->sec_handle,
                                       hash_data_ptr->lists_head,
                                       hash_val,
                                       hash_data_ptr->ptr_size,
                                       hash_data_ptr->tmp_buf
                                       );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    /*
     * check if the list head is null.
     */
    ptr_long = 0;

    res = soc_sand_U8_to_U32(
                             hash_data_ptr->tmp_buf,
                             hash_data_ptr->ptr_size,
                             &ptr_long
                             );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);


    if (ptr_long == hash_data_ptr->null_ptr)
    {
        if (get_first_empty)
        {
            if (alloc_by_index)
            {
                /* don't check if this index already in use */
                res = soc_sand_occ_bm_occup_status_set(
                                                       unit,
                                                       hash_data_ptr->memory_use,
                                                       *entry,
                                                       TRUE
                                                       );
                SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
                found_new = TRUE;

                ptr_long = *entry;
            }
            else
            {
                res = soc_sand_occ_bm_alloc_next(
                                                 unit,
                                                 hash_data_ptr->memory_use,
                                                 &ptr_long,
                                                 &found_new
                                                 );
                SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
            }
            if (!found_new)
            {
                ptr_long = SOC_SAND_HASH_TABLE_NULL;
                *entry = SOC_SAND_HASH_TABLE_NULL;
            }
            else
            {
                /**list_head = 0;*/
                res = soc_sand_U32_to_U8(
                                         &ptr_long,
                                         hash_data_ptr->ptr_size,
                                         hash_data_ptr->tmp_buf
                                         );
                SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);


                if(SOC_DPP_WB_ENGINE_VAR_NONE == init_info_ptr->wb_var_index) 
                {
                    res = set_entry_fun(
                                                       init_info_ptr->prime_handle,
                                                       init_info_ptr->sec_handle,
                                                       hash_data_ptr->lists_head,
                                                       hash_val,
                                                       hash_data_ptr->ptr_size,
                                                       hash_data_ptr->tmp_buf
                                                       );
                    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
                }
                else
                {
                    res = SOC_DPP_WB_ENGINE_SET_ARR(init_info_ptr->prime_handle, init_info_ptr->wb_var_index + WB_ENGINE_HASH_TABLE_LISTS_HEAD, 
                                                    hash_data_ptr->tmp_buf, 
                                                    hash_val);
                    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 80, exit);      
                }
                *entry = ptr_long;
            }
            *found = FALSE;
            goto exit;
        }
        *found = FALSE;
        *entry = SOC_SAND_HASH_TABLE_NULL;
        goto exit;
    }

    not_found = TRUE;

    while (ptr_long != hash_data_ptr->null_ptr)
    {
        /*
         * read keys
         */
        res = get_entry_fun(
                                           init_info_ptr->prime_handle,
                                           init_info_ptr->sec_handle,
                                           hash_data_ptr->keys,
                                           ptr_long,
                                           init_info_ptr->key_size,
                                           hash_data_ptr->tmp_buf
                                           );
        SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

        cur_key = hash_data_ptr->tmp_buf;


        not_found = (uint8)soc_sand_os_memcmp(
                                              cur_key,
                                              key,
                                              (init_info_ptr->key_size * sizeof(SOC_SAND_HASH_TABLE_KEY_TYPE))
                                              );

        /*
         * if key was found
         */
        if (not_found == FALSE)
        {
            *found = TRUE;
            *entry = ptr_long;
            goto exit;
        }
        /*
         * if wasn't found then look in the next node in the list.
         */
        *first = FALSE;
        /*
         * get next node
         */
        res = get_entry_fun(
                                           init_info_ptr->prime_handle,
                                           init_info_ptr->sec_handle,
                                           hash_data_ptr->next,
                                           ptr_long,
                                           hash_data_ptr->ptr_size,
                                           hash_data_ptr->tmp_buf
                                           );
        SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

        next = hash_data_ptr->tmp_buf;


        *prev_entry = ptr_long;
        ptr_long = 0;

        res = soc_sand_U8_to_U32(
                                 next,
                                 hash_data_ptr->ptr_size,
                                 &ptr_long
                                 );
        SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);


    }
    /*
     * Didn't find anything, return not found
     * but the entry point to the place where the
     * key suppose to be.
     */
    *found = FALSE;
    /*
     * yet, if tries make rehashed but fail, then set entry
     * to point to the end of the table or the first empty entry according to the given parameter.
     */
    if (get_first_empty)
    {
        if (alloc_by_index)
        {
            /* don't check if this index already in use */
            res = soc_sand_occ_bm_occup_status_set(
                                                   unit,
                                                   hash_data_ptr->memory_use,
                                                   *entry,
                                                   TRUE
                                                   );
            SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);
            found_new = TRUE;
            next_node = *entry;
        }
        else
        {
            res = soc_sand_occ_bm_alloc_next(
                                             unit,
                                             hash_data_ptr->memory_use,
                                             &next_node,
                                             &found_new
                                             );
            SOC_SAND_CHECK_FUNC_RESULT(res, 130, exit);
        }
        if (!found_new)
        {
            *entry = SOC_SAND_HASH_TABLE_NULL;
        }
        else
        {
            res = soc_sand_U32_to_U8(
                                     &next_node,
                                     hash_data_ptr->ptr_size,
                                     hash_data_ptr->tmp_buf
                                     );
            SOC_SAND_CHECK_FUNC_RESULT(res, 140, exit);


            if(SOC_DPP_WB_ENGINE_VAR_NONE == init_info_ptr->wb_var_index) 
            {
                res = set_entry_fun(
                                                   init_info_ptr->prime_handle,
                                                   init_info_ptr->sec_handle,
                                                   hash_data_ptr->next,
                                                   *prev_entry,
                                                   hash_data_ptr->ptr_size,
                                                   hash_data_ptr->tmp_buf
                                                   );
                SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);

            }
            else
            {
                res = SOC_DPP_WB_ENGINE_SET_ARR(init_info_ptr->prime_handle, init_info_ptr->wb_var_index + WB_ENGINE_HASH_TABLE_NEXT, 
                                                hash_data_ptr->tmp_buf, 
                                                *prev_entry);
                SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 160, exit);      
            }

            *entry = next_node;
        }
    }
    else
    {
        *entry = SOC_SAND_HASH_TABLE_NULL;
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_hash_table_find_entry()",0,0);
}

#include <soc/dpp/SAND/Utils/sand_footer.h>
