/* $Id: sand_hashtable.h,v 1.5 Broadcom SDK $
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
#ifndef SOC_SAND_HASHTABLE_H_INCLUDED
/* { */
#define SOC_SAND_HASHTABLE_H_INCLUDED

/*************
* INCLUDES  *
*************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Utils/sand_framework.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Utils/sand_occupation_bitmap.h>
/* } */

/*************
 * DEFINES   *
 *************/
/* { */
#define SOC_SAND_HASH_TABLE_NULL  SOC_SAND_U32_MAX
/* } */


/*************
 * MACROS    *
 *************/
/* { */

/* $Id: sand_hashtable.h,v 1.5 Broadcom SDK $
 * Reset the hash table iterator to point to the beginning of the hash table
 */
#define SOC_SAND_HASH_TABLE_ITER_SET_BEGIN(iter) \
  ((*iter) = 0)
/*
 * Check if the hash table iterator arrives to the end of the hash table
 */
#define SOC_SAND_HASH_TABLE_ITER_IS_END(iter)  \
  ((*iter) == SOC_SAND_U32_MAX)

/* } */

/*************
 * TYPE DEFS *
 *************/
/* { */

/*
 * type of the hash table key
 *
 */
typedef uint8 SOC_SAND_HASH_TABLE_KEY;
/*
 * type of the hash table data
 */
typedef uint8* SOC_SAND_HASH_TABLE_DATA;
/*
 * the ADT of hash table, use this type to manipulate the has table.
 */
typedef struct SOC_SAND_HASH_TABLE_INFO_T*  SOC_SAND_HASH_TABLE_PTR;
/*
 * iterator over the hash table, use this type to traverse the hash table.
 */
typedef uint32 SOC_SAND_HASH_TABLE_ITER;

/*********************************************************************
* NAME:
*     SOC_SAND_HASH_MAP_HASH_FUN_CALL_BACK
* FUNCTION:
*  Hash functions used to map from key to hash value (entry in the hash table)
* INPUT:
*  SOC_SAND_IN  SOC_SAND_HASH_TABLE_PTR        hash_table -
*   the hash table, needed so the hash function
*   can get the properties of the hash table to consider in the
*   calculations
*  SOC_SAND_IN  SOC_SAND_HASH_TABLE_KEY*    const key -
*   key to hash
*  SOC_SAND_IN  uint32               seed -
*   value to use in the hash calculation
*  SOC_SAND_OUT  uint32*             hash_val -
*   the calculated hash value.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
typedef
  uint32
    (*SOC_SAND_HASH_MAP_HASH_FUN_CALL_BACK)(
      SOC_SAND_IN  SOC_SAND_HASH_TABLE_PTR     hash_table,
      SOC_SAND_IN  SOC_SAND_HASH_TABLE_KEY*    const key,
      SOC_SAND_IN  uint32                seed,
      SOC_SAND_OUT  uint32*              hash_val
    );



/*********************************************************************
* NAME:
*     SOC_SAND_HASH_MAP_SW_DB_ENTRY_SET
* FUNCTION:
*  call back to set the entry information from the SW DB of the device.
* INPUT:
*  SOC_SAND_IN  int              prime_handle -
*   handle of the hash table to identify the hash table instance
*  SOC_SAND_IN  int              prime_handle -
*   secondary identifier to data to be set in the hash table instance.
*  SOC_SAND_IN  uint32                   offset -
*   offset of the entry in the memory "array".
*  SOC_SAND_IN  uint32                   len -
*   the length in bytes (uint8) of the entry to write.
*  SOC_SAND_IN uint8                     *data -
*   the information of to write.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
typedef
  uint32
    (*SOC_SAND_HASH_MAP_SW_DB_ENTRY_SET)(
      SOC_SAND_IN  int                   prime_handle,
      SOC_SAND_IN  uint32                   sec_handle,
      SOC_SAND_INOUT  uint8                 *buffer,
      SOC_SAND_IN  uint32                   offset,
      SOC_SAND_IN  uint32                   len,
      SOC_SAND_IN uint8                     *data
    );

/*********************************************************************
* NAME:
*     SOC_SAND_HASH_MAP_SW_DB_ENTRY_GET
* FUNCTION:
*  call back to get the entry information from the SW DB of the device.
* INPUT:
*  SOC_SAND_IN  int              prime_handle -
*   handle of the hash table to identify the hash table instance
*  SOC_SAND_IN  int              prime_handle -
*   secondary identifier to data to be set in the hash table instance.
*  SOC_SAND_IN  uint32                   offset -
*   offset of the entry in the memory "array".
*  SOC_SAND_IN  uint32                   len -
*   the length in bytes (uint8) of the entry to write.
*  SOC_SAND_OUT uint8                     *data -
*   the information to read.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
typedef
  uint32
    (*SOC_SAND_HASH_MAP_SW_DB_ENTRY_GET)(
      SOC_SAND_IN  int                   prime_handle,
      SOC_SAND_IN  uint32                   sec_handle,
      SOC_SAND_IN  uint8                    *buffer,
      SOC_SAND_IN  uint32                   offset,
      SOC_SAND_IN  uint32                   len,
      SOC_SAND_OUT uint8*                   const data
    );
/*
 * includes the information user has to supply in the hash table creation
 */
typedef struct {
 /*
  * handle of the hash table to identify the hash table instance
  */
  int prime_handle;
 /*
  * handle of the hash table to identify the hash table instance
  */
  uint32 sec_handle;
 /*
  * size of the hash table maximum number of elements to insert the hash table
  */
  uint32 table_size;
 /*
  * number of unique results from the hash function.
  */
  uint32 table_width;
 /*
  * size of the key (in bytes)
  */
  uint32 key_size;
 /*
  * size of the data (in bytes)
  */
  uint32 data_size;
 /*
  * hash functions used to map from key to hash value (number)
  * set to NULL (or don't touch after clear) to use the default hash function.
  */
  SOC_SAND_HASH_MAP_HASH_FUN_CALL_BACK hash_function;
 /*
  * hash function to use in case the hash function returns entry in use.
  * set to NULL (or don't touch after clear) to use the default rehash function.
  */
  SOC_SAND_HASH_MAP_HASH_FUN_CALL_BACK rehash_function;

  SOC_SAND_HASH_MAP_SW_DB_ENTRY_GET get_entry_fun;

  SOC_SAND_HASH_MAP_SW_DB_ENTRY_SET set_entry_fun;

 /*
  * saving warm boot variable index get/set of memory allocator variables.
  */
  int32 wb_var_index;

} SOC_SAND_HASH_TABLE_INIT_INFO;



typedef struct
{
 /*
  * array to include entries
  */
  uint8 *lists_head;
 /*
  * includes the full keys (data of the nodes)
  */
  uint8 *keys;
 /*
  * point to the next in the list.
  */
  uint8 *next;
 /*
  * The size of the pointer in bytes.
  */
  uint32
    ptr_size;
 /*
  * mapping of the tree memory, for efficient manipulation.
  */
  SOC_SAND_OCC_BM_PTR memory_use;
 /*
  * null pointer.
  */
  uint32
    null_ptr;
 /*
  * tmp buffer for copy
  */
  uint8 *tmp_buf;
  uint8 *tmp_buf2;

} SOC_SAND_HASH_TABLE_T;


typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR

  SOC_SAND_HASH_TABLE_INIT_INFO
    init_info;

  SOC_SAND_HASH_TABLE_T
    hash_data;
} SOC_SAND_HASH_TABLE_INFO;

/* } */


/*typedef SOC_SAND_HASH_TABLE_INFO* SOC_SAND_HASH_TABLE_PTR;*/

/*************
* GLOBALS   *
*************/
/* { */

/* } */

/*************
* FUNCTIONS *
*************/
/* { */

uint32
  soc_sand_hash_table_simple_hash(
                            SOC_SAND_IN  SOC_SAND_HASH_TABLE_PTR     hash_table,
                            SOC_SAND_IN  SOC_SAND_HASH_TABLE_KEY*    const key,
                            SOC_SAND_IN  uint32                seed,
                            SOC_SAND_OUT  uint32*              hash_val
                            );

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
*   SOC_SAND_INOUT  SOC_SAND_HASH_TABLE_INFO     *hash_table -
*     information to use in order to create the hash table (size, hash function...)
* REMARKS:
*   None.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_sand_hash_table_create(
    SOC_SAND_INOUT  SOC_SAND_HASH_TABLE_INFO     *hash_table
  );

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
    SOC_SAND_INOUT  SOC_SAND_HASH_TABLE_INFO  *hash_table
  );

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
*   SOC_SAND_INOUT  SOC_SAND_HASH_TABLE_INFO    *hash_table -
*     The hash table to add a key to.
*   SOC_SAND_IN  SOC_SAND_HASH_TABLE_KEY  key -
*     The key to add into the hash table
*   SOC_SAND_OUT  uint32                  *data_indx -
*     the index and identifier where the value was inserted/found.
*   SOC_SAND_OUT  uint8                 *success -
*     whether the add operation succeeded
* REMARKS:
*
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_sand_hash_table_entry_add(
    SOC_SAND_INOUT  SOC_SAND_HASH_TABLE_INFO    *hash_table,
    SOC_SAND_IN  SOC_SAND_HASH_TABLE_KEY*       const key,
    SOC_SAND_OUT  uint32                  *data_indx,
    SOC_SAND_OUT  uint8                 *success
  );


uint32
  soc_sand_hash_table_entry_add_at_index(
    SOC_SAND_INOUT  SOC_SAND_HASH_TABLE_INFO    *hash_table,
    SOC_SAND_IN  SOC_SAND_HASH_TABLE_KEY*    const key,
    SOC_SAND_IN  uint32                  data_indx,
    SOC_SAND_OUT  uint8                 *success
  );

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
*  SOC_SAND_INOUT  SOC_SAND_HASH_TABLE_PTR hash_table -
*     The hash table to add to.
*  SOC_SAND_IN  SOC_SAND_HASH_TABLE_KEY  key -
*     The key to remove from the hash table
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_sand_hash_table_entry_remove(
    SOC_SAND_INOUT  SOC_SAND_HASH_TABLE_INFO      *hash_table,
    SOC_SAND_IN  SOC_SAND_HASH_TABLE_KEY*         const key
  );

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
* INPUT:
*  SOC_SAND_INOUT  SOC_SAND_HASH_TABLE_PTR hash_table -
*     The hash table to add to.
*  SOC_SAND_IN  uint32                   indx -
*     index of the entry to remove.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_sand_hash_table_entry_remove_by_index(
    SOC_SAND_INOUT  SOC_SAND_HASH_TABLE_INFO      *hash_table,
    SOC_SAND_IN  uint32                     indx
  );

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
*  SOC_SAND_IN  SOC_SAND_HASH_TABLE_PTR        hash_table -
*     The hash table.
*   SOC_SAND_IN  SOC_SAND_HASH_TABLE_KEY   key -
*     The key to lookup
*   SOC_SAND_OUT  SOC_SAND_HASH_TABLE_DATA  *data -
*     the data associated with the given key, valid only found is true.
*   SOC_SAND_IN  uint8                    *found -
*     whether the key was found in the hash table
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_sand_hash_table_entry_lookup(
    SOC_SAND_INOUT  SOC_SAND_HASH_TABLE_INFO    *hash_table,
    SOC_SAND_IN  SOC_SAND_HASH_TABLE_KEY*       const key,
    SOC_SAND_OUT  uint32                  *data_indx,
    SOC_SAND_OUT  uint8                 *found
  );

/*********************************************************************
* NAME:
*     soc_sand_hash_table_get_by_index
* TYPE:
*   PROC
* DATE:
*   Mar  6 2008
* FUNCTION:
*  return an entry from a hash table given the index identify
*  this entry.
* INPUT:
*  SOC_SAND_INOUT  SOC_SAND_HASH_TABLE_PTR hash_table -
*     The hash table to add to.
*  SOC_SAND_IN  uint32                   data_indx -
*     index of the entry to return.
*  SOC_SAND_OUT  SOC_SAND_HASH_TABLE_KEY*        const key -
*     key resides in this entry
*  SOC_SAND_OUT  uint8                   *found -
*     whether this key is valid
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_sand_hash_table_get_by_index(
    SOC_SAND_IN  SOC_SAND_HASH_TABLE_INFO      *hash_table,
    SOC_SAND_IN  uint32                     data_indx,
    SOC_SAND_OUT  SOC_SAND_HASH_TABLE_KEY*        key,
    SOC_SAND_OUT  uint8                   *found
  );


/*********************************************************************
* NAME:
*     soc_sand_hash_table_clear
* TYPE:
*   PROC
* DATE:
*   Mar  6 2008
* FUNCTION:
*  clear the Hash table content without freeing the memory
* INPUT:
*  SOC_SAND_IN  SOC_SAND_HASH_TABLE_PTR        hash_table -
*     The hash table.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_sand_hash_table_clear(
    SOC_SAND_INOUT  SOC_SAND_HASH_TABLE_INFO    *hash_table
  );

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
    SOC_SAND_IN  SOC_SAND_HASH_TABLE_INFO       *hash_table,
    SOC_SAND_INOUT  SOC_SAND_HASH_TABLE_ITER    *iter,
    SOC_SAND_OUT  SOC_SAND_HASH_TABLE_KEY*      const key,
    SOC_SAND_OUT  uint32                  *data_indx
  );


/*********************************************************************
* NAME:
*     soc_sand_hash_table_get_size_for_save
* TYPE:
*   PROC
* DATE:
*   Mar  6 2008
* FUNCTION:
*     returns the size of the buffer needed to return the hash table as buffer.
*     in order to be loaded later
* INPUT:
*   SOC_SAND_IN  SOC_SAND_HASH_TABLE_PTR hash_table -
*     The Hash table to get the size for.
*   SOC_SAND_OUT  uint32   *size -
*     the size of the buffer.
* REMARKS:
*   None.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_sand_hash_table_get_size_for_save(
    SOC_SAND_IN  SOC_SAND_HASH_TABLE_INFO              *hash_table,
    SOC_SAND_OUT  uint32                         *size
  );

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
    SOC_SAND_IN  SOC_SAND_HASH_TABLE_INFO     *hash_table,
    SOC_SAND_OUT  uint8                 *buffer,
    SOC_SAND_IN  uint32                 buffer_size_bytes,
    SOC_SAND_OUT uint32                 *actual_size_bytes
  );


/*********************************************************************
* NAME:
*     soc_sand_hash_table_load
* TYPE:
*   PROC
* DATE:
*   Mar  6 2008
* FUNCTION:
*     Load from the given buffer the hash table saved in this buffer.
* INPUT:
*   SOC_SAND_IN  uint8                 *buffer -
*     buffer includes the hast table
*   SOC_SAND_IN  SOC_SAND_HASH_MAP_HASH_FUN_CALL_BACK  hash_function -
*     the hash function of the hash table.
*   SOC_SAND_IN  SOC_SAND_HASH_MAP_HASH_FUN_CALL_BACK  hash_function -
*     the hash function of the hash table.
*   SOC_SAND_OUT  SOC_SAND_HASH_TABLE_PTR hash_table -
*     The Hash table to load.
* REMARKS:
*   - the size of the buffer has to be at least as the value returned
*     by soc_sand_hash_table_get_size_for_save.
*   - there is need to supply the hash and rehash function (in case they are not
*     the default implementation, cause in the save they are not saved.
*     by soc_sand_hash_table_get_size_for_save.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_sand_hash_table_load(
    SOC_SAND_IN  uint8                           **buffer,
    SOC_SAND_IN  SOC_SAND_HASH_MAP_SW_DB_ENTRY_SET     set_entry_fun,
    SOC_SAND_IN  SOC_SAND_HASH_MAP_SW_DB_ENTRY_GET     get_entry_fun,
    SOC_SAND_IN  SOC_SAND_HASH_MAP_HASH_FUN_CALL_BACK  hash_function,
    SOC_SAND_IN  SOC_SAND_HASH_MAP_HASH_FUN_CALL_BACK  rehash_function,
    SOC_SAND_OUT  SOC_SAND_HASH_TABLE_INFO             *hash_table
  );

void
  soc_sand_SAND_HASH_TABLE_INFO_clear(
    SOC_SAND_OUT SOC_SAND_HASH_TABLE_INFO *info
  );


#ifdef SOC_SAND_DEBUG

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
  );

/*********************************************************************
* NAME:
*     soc_sand_hash_table_tests
* TYPE:
*   PROC
* DATE:
*   Mar  6 2008
* FUNCTION:
*     Tests the hash table module
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
*REMARKS:* SW License Agreement: Dune Networks (c). CONFIDENTIAL PROPRIETARY INFORMATION.
* Any use of this Software is subject to Software License Agreement
* included in the Driver User Manual of this device.
* Any use of this Software constitutes an agreement to the terms
* of the above Software License Agreement.
********************************************************************/
uint32
  soc_sand_hash_table_tests(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 silent
);

void
  soc_sand_SAND_HASH_TABLE_INFO_print(
    SOC_SAND_IN SOC_SAND_HASH_TABLE_INFO *info
  );

#endif /* SOC_SAND_DEBUG */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>


/* } SOC_SAND_HASHTABLE_H_INCLUDED*/
#endif


