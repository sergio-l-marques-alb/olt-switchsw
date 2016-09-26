/* $Id: sand_hashtable.h,v 1.5 Broadcom SDK $
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
*/
#ifndef UTILEX_HASHTABLE_H_INCLUDED
/* { */
#define UTILEX_HASHTABLE_H_INCLUDED

/*************
* INCLUDES  *
*************/
/* { */
#include <shared/utilex/utilex_framework.h>
#if (0)
/* { */
#include <soc/dpp/SAND/Management/sand_general_macros.h>
/* } */
#endif
#include <shared/utilex/utilex_occupation_bitmap.h>
/* } */

/*************
 * DEFINES   *
 *************/
/* { */
/*
 * Maximal number of hash table entries to assign at init.
 * See utilex_hash_table_init(), utilex_hash_table_create()
 */
#define MAX_NOF_HASHS             (200 * SOC_DPP_DEFS_GET(unit, nof_cores))
#define UTILEX_HASH_TABLE_NULL  UTILEX_U32_MAX
/* } */


/*************
 * MACROS    *
 *************/
/* { */

/* $Id: sand_hashtable.h,v 1.5 Broadcom SDK $
 * Reset the hash table iterator to point to the beginning of the hash table
 */
#define UTILEX_HASH_TABLE_ITER_SET_BEGIN(iter) ((*iter) = 0)
/*
 * Check if the hash table iterator arrives to the end of the hash table
 */
#define UTILEX_HASH_TABLE_ITER_IS_END(iter)    ((*iter) == UTILEX_U32_MAX)

/* } */

/*************
 * TYPE DEFS *
 *************/
/* { */

/*
 * type of the hash table key
 *
 */
typedef uint8 UTILEX_HASH_TABLE_KEY;
/*
 * type of the hash table data
 */
typedef uint8 *UTILEX_HASH_TABLE_DATA ;
/*
 * the ADT of hash table, use this type to manipulate the hash table. 
 * just a workaround to allow cyclic reference from hash func to hash info and vice versa
 */
/* typedef struct UTILEX_HASH_TABLE_INFO_T *UTILEX_HASH_TABLE_PTR_WORKAROUND ; */
/*
 * iterator over the hash table, use this type to traverse the hash table.
 */
typedef uint32 UTILEX_HASH_TABLE_ITER;
/*
 * This is an identifier of an element of type UTILEX_HASH_TABLE_INFO
 *
 * Replace: typedef UTILEX_HASH_TABLE_INFO* UTILEX_HASH_TABLE_PTR;
 * because the new software state does not use pointers, only handles.
 * So now, UTILEX_HASH_TABLE_PTR is just a handle to the 'hash table'
 * structure (actually, index into 'hashs_array' {of pointers})
 *
 * Note that the name is kept as is to minimize changes in current code.
 */
typedef uint32 UTILEX_HASH_TABLE_PTR ;

/*********************************************************************
* NAME:
*     UTILEX_HASH_MAP_HASH_FUN_CALL_BACK
* FUNCTION:
*  Hash functions used to map from key to hash value (entry in the hash table)
* INPUT:
*  UTILEX_IN  int                            unit -
*   Identifier of the device to access.
*  UTILEX_IN  UTILEX_HASH_TABLE_PTR        hash_table -
*   the hash table, needed so the hash function
*   can get the properties of the hash table to consider in the
*   calculations
*  UTILEX_IN  UTILEX_HASH_TABLE_KEY*       const key -
*   key to hash
*  UTILEX_IN  uint32                         seed -
*   value to use in the hash calculation
*  UTILEX_OUT  uint32*                       hash_val -
*   the calculated hash value.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
typedef
  uint32
    (*UTILEX_HASH_MAP_HASH_FUN_CALL_BACK)(
      UTILEX_IN  int                         unit,
      UTILEX_IN  UTILEX_HASH_TABLE_PTR     hash_table,
      UTILEX_IN  UTILEX_HASH_TABLE_KEY     *const key,
      UTILEX_IN  uint32                      seed,
      UTILEX_OUT uint32*                     hash_val
    );
/*********************************************************************
* NAME:
*     UTILEX_HASH_MAP_SW_DB_ENTRY_SET
* FUNCTION:
*  call back to set the entry information from the SW DB of the device.
* INPUT:
*  UTILEX_IN  int              prime_handle -
*   handle of the hash table to identify the hash table instance
*  UTILEX_IN  int              prime_handle -
*   secondary identifier to data to be set in the hash table instance.
*  UTILEX_IN  uint32                   offset -
*   offset of the entry in the memory "array".
*  UTILEX_IN  uint32                   len -
*   the length in bytes (uint8) of the entry to write.
*  UTILEX_IN uint8                     *data -
*   the information of to write.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
typedef
  uint32
    (*UTILEX_HASH_MAP_SW_DB_ENTRY_SET)(
      UTILEX_IN  int                   prime_handle,
      UTILEX_IN  uint32                   sec_handle,
      UTILEX_INOUT  uint8                 *buffer,
      UTILEX_IN  uint32                   offset,
      UTILEX_IN  uint32                   len,
      UTILEX_IN uint8                     *data
    );
/*********************************************************************
* NAME:
*     UTILEX_HASH_MAP_SW_DB_ENTRY_GET
* FUNCTION:
*  call back to get the entry information from the SW DB of the device.
* INPUT:
*  UTILEX_IN  int              prime_handle -
*   handle of the hash table to identify the hash table instance
*  UTILEX_IN  int              prime_handle -
*   secondary identifier to data to be set in the hash table instance.
*  UTILEX_IN  uint32                   offset -
*   offset of the entry in the memory "array".
*  UTILEX_IN  uint32                   len -
*   the length in bytes (uint8) of the entry to write.
*  UTILEX_OUT uint8                     *data -
*   the information to read.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
typedef
  uint32
    (*UTILEX_HASH_MAP_SW_DB_ENTRY_GET)(
      UTILEX_IN  int                   prime_handle,
      UTILEX_IN  uint32                   sec_handle,
      UTILEX_IN  uint8                    *buffer,
      UTILEX_IN  uint32                   offset,
      UTILEX_IN  uint32                   len,
      UTILEX_OUT uint8*                   const data
    );

/*
 * Includes the information user has to supply in the hash table creation
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
  UTILEX_HASH_MAP_HASH_FUN_CALL_BACK hash_function;
  /*
   * hash function to use in case the hash function returns entry in use.
   * set to NULL (or don't touch after clear) to use the default rehash function.
   */
  UTILEX_HASH_MAP_HASH_FUN_CALL_BACK rehash_function;

  UTILEX_HASH_MAP_SW_DB_ENTRY_GET get_entry_fun;

  UTILEX_HASH_MAP_SW_DB_ENTRY_SET set_entry_fun;

} UTILEX_HASH_TABLE_INIT_INFO ;

typedef struct
{
  /*
   * array to include entries
   */
  SW_STATE_BUFF *lists_head;
  /*
   * includes the full keys (data of the nodes)
   */
  SW_STATE_BUFF *keys;
  /*
   * point to the next in the list.
   */
  SW_STATE_BUFF *next;
  /*
   * The size of the pointer in bytes.
   */
  uint32        ptr_size;
  /*
   * mapping of the tree memory, for efficient manipulation.
   */
  UTILEX_OCC_BM_PTR memory_use ;
  /*
   * null pointer.
   */
  uint32        null_ptr;
  /*
   * tmp buffer for copy
   */
  SW_STATE_BUFF *tmp_buf ;
  SW_STATE_BUFF *tmp_buf2 ;

} UTILEX_HASH_TABLE_T;


typedef struct
{
  UTILEX_HASH_TABLE_INIT_INFO  init_info ;
  UTILEX_HASH_TABLE_T          hash_data ;
} UTILEX_HASH_TABLE_INFO ;

/* } */

/*
 * Control Structure for all created hash tables. Each hash is pointed
 * by hashs_array. See utilex_hash_table_init()
 */
typedef struct utilex_sw_state_hash_table_s
{
                      uint32                      max_nof_hashs ;
                      uint32                      in_use ;
  PARSER_HINT_ARR_PTR UTILEX_HASH_TABLE_INFO   **hashs_array ;
  PARSER_HINT_ARR     SHR_BITDCL                  *occupied_hashs ;
} utilex_sw_state_hash_table_t ;

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
  utilex_hash_table_simple_hash(
                            UTILEX_IN  int                         unit,
                            UTILEX_IN  UTILEX_HASH_TABLE_PTR     hash_table,
                            UTILEX_IN  UTILEX_HASH_TABLE_KEY*    const key,
                            UTILEX_IN  uint32                seed,
                            UTILEX_OUT  uint32*              hash_val
                            ) ;

uint32
  utilex_hash_table_get_illegal_hashtable_handle(
    void
  ) ;

/*********************************************************************
* NAME:
*     utilex_hash_table_get_table_size
* TYPE:
*   PROC
* DATE:
*   Mar 26 2015
* FUNCTION:
*   Get element 'table_size' for hash table.
* INPUT:
*   UTILEX_IN     int                           unit -
*     Identifier of the device to access.
*   UTILEX_IN  UTILEX_HASH_TABLE_PTR          hash_table -
*     Handle to the hash table.
*   UTILEX_INOUT uint32                         *table_size_ptr -
*     This procedure loads pointed memory by the info element 'table_size'.
* REMARKS:
*   For external users nots aware of 'new sw state' structures.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  utilex_hash_table_get_table_size(
    UTILEX_IN     int                              unit,
    UTILEX_IN    UTILEX_HASH_TABLE_PTR           hash_table,
    UTILEX_INOUT uint32                            *table_size_ptr
  ) ;

/*********************************************************************
* NAME:
*   utilex_hash_table_init
* TYPE:
*   PROC
* DATE:
*   May 19 2015
* FUNCTION:
*   Initialize control structure for ALL hash table instances expected.
* INPUT:
*   UTILEX_IN  int unit -
*     Identifier of the device to access.
*   UTILEX_IN  uint32 max_nof_hashs -
*     Maximal number of hash tables which can be sustained simultaneously.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  utilex_hash_table_init(
    UTILEX_IN       int                          unit,
    UTILEX_IN       uint32                       max_nof_hashs
  ) ;
/*********************************************************************
* NAME:
*   utilex_hash_table_clear_all_tmps
* TYPE:
*   PROC
* DATE:
*   Aug 02 2015
* FUNCTION:
*   Fill all allocated 'tmp' (sand box) buffers by zeros.
* INPUT:
*   UTILEX_IN  int unit -
*     Identifier of the device to access.
* REMARKS:
*   This procedure is to be used at init before 'diff'ing previous sw
*   state buffer with current one. This ensures that such buffers are
*   effectively not 'diff'ed.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  utilex_hash_table_clear_all_tmps(
    UTILEX_IN int unit
  ) ;
/*********************************************************************
* NAME:
*     utilex_hash_table_create
* TYPE:
*   PROC
* DATE:
*   Mar  6 2008
* FUNCTION:
*     Creates a new Hash table instance.
* INPUT:
*   UTILEX_IN  int                              unit -
*     Identifier of the device to access.
*   UTILEX_INOUT  UTILEX_HASH_TABLE_INFO     *hash_table -
*     information to use in order to create the hash table (size, hash function...)
* REMARKS:
*   None.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  utilex_hash_table_create(
    UTILEX_IN     int                           unit,
    UTILEX_INOUT  UTILEX_HASH_TABLE_PTR           *hash_table,
    UTILEX_IN     UTILEX_HASH_TABLE_INIT_INFO     init_info
  );

/*********************************************************************
* NAME:
*     utilex_hash_table_destroy
* TYPE:
*   PROC
* DATE:
*   Mar  6 2008
* FUNCTION:
*     free the hash table instance.
* INPUT:
*  UTILEX_IN  int                      unit -
*     Identifier of the device to access.
*  UTILEX_OUT  UTILEX_HASH_TABLE_PTR hash_table -
*     The Hash table to destroy.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  utilex_hash_table_destroy(
    UTILEX_IN     int                        unit,
    UTILEX_INOUT  UTILEX_HASH_TABLE_PTR    hash_table
  ) ;

/*********************************************************************
* NAME:
*     utilex_hash_table_entry_add
* TYPE:
*   PROC
* DATE:
*   Mar  6 2008
* FUNCTION:
*  Insert an entry into the hash table, if already exist then
*  the operation returns an error.
* INPUT:
*   UTILEX_IN  int                             unit -
*     Identifier of the device to access.
*   UTILEX_IN  UTILEX_HASH_TABLE_PTR         hash_table -
*     The hash table to add a key to.
*   UTILEX_IN  UTILEX_HASH_TABLE_KEY         key -
*     The key to add into the hash table
*   UTILEX_OUT  uint32                        *data_indx -
*     the index and identifier where the value was inserted/found.
*   UTILEX_OUT  uint8                         *success -
*     whether the add operation succeeded
* REMARKS:
*
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  utilex_hash_table_entry_add(
    UTILEX_IN    int                        unit,
    UTILEX_IN    UTILEX_HASH_TABLE_PTR    hash_table,
    UTILEX_IN    UTILEX_HASH_TABLE_KEY    *const key,
    UTILEX_OUT   uint32                     *data_indx,
    UTILEX_OUT   uint8                      *success
  ) ;

uint32
  utilex_hash_table_entry_add_at_index(
    UTILEX_IN     int                          unit,
    UTILEX_IN     UTILEX_HASH_TABLE_PTR      hash_table,
    UTILEX_IN     UTILEX_HASH_TABLE_KEY      *const key,
    UTILEX_IN     uint32                       data_indx,
    UTILEX_OUT    uint8                        *success
  ) ;

/*********************************************************************
* NAME:
*     utilex_hash_table_entry_remove
* TYPE:
*   PROC
* DATE:
*   Mar  6 2008
* FUNCTION:
*  Remove an entry from a hash table, if the key is not exist then
*  the operation has no effect.
* INPUT:
*  UTILEX_IN  int                        unit -
*     Identifier of the device to access.
*  UTILEX_INOUT  UTILEX_HASH_TABLE_PTR hash_table -
*     The hash table to add to.
*  UTILEX_IN  UTILEX_HASH_TABLE_KEY    key -
*     The key to remove from the hash table
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  utilex_hash_table_entry_remove(
    UTILEX_IN     int                          unit,
    UTILEX_IN     UTILEX_HASH_TABLE_PTR      hash_table,
    UTILEX_IN     UTILEX_HASH_TABLE_KEY      *const key
  ) ;

/*********************************************************************
* NAME:
*     utilex_hash_table_entry_remove_by_index
* TYPE:
*   PROC
* DATE:
*   Mar  6 2008
* FUNCTION:
*  Remove an entry from a hash table given the index identify
*  this entry.
* INPUT:
*  UTILEX_IN  int                        unit -
*     Identifier of the device to access.
*  UTILEX_INOUT  UTILEX_HASH_TABLE_PTR hash_table -
*     The hash table to add to.
*  UTILEX_IN  uint32                     indx -
*     index of the entry to remove.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  utilex_hash_table_entry_remove_by_index(
    UTILEX_IN     int                     unit,
    UTILEX_IN     UTILEX_HASH_TABLE_PTR hash_table,
    UTILEX_IN     uint32                  data_indx
  ) ;

/*********************************************************************
* NAME:
*     utilex_hash_table_entry_lookup
* TYPE:
*   PROC
* DATE:
*   Mar  6 2008
* FUNCTION:
*  lookup in the hash table for the given key and return the data inserted with
*  the given key.
* INPUT:
*   UTILEX_IN  int                            unit -
*     Identifier of the device to access.
*   UTILEX_IN  UTILEX_HASH_TABLE_PTR        hash_table -
*     The hash table.
*   UTILEX_IN  UTILEX_HASH_TABLE_KEY        key -
*     The key to lookup
*   UTILEX_OUT  UTILEX_HASH_TABLE_DATA     *data -
*     the data associated with the given key, valid only found is true.
*   UTILEX_IN  uint8                        *found -
*     whether the key was found in the hash table
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  utilex_hash_table_entry_lookup(
    UTILEX_IN     int                         unit,
    UTILEX_INOUT  UTILEX_HASH_TABLE_PTR     hash_table,
    UTILEX_IN     UTILEX_HASH_TABLE_KEY     *const key,
    UTILEX_OUT    uint32                      *data_indx,
    UTILEX_OUT    uint8                       *found
  ) ;
/*********************************************************************
* NAME:
*     utilex_hash_table_get_by_index
* TYPE:
*   PROC
* DATE:
*   Mar  6 2008
* FUNCTION:
*  return an entry from a hash table given the index identify
*  this entry.
* INPUT:
*  UTILEX_IN  int                        unit -
*     Identifier of the device to access.
*  UTILEX_INOUT  UTILEX_HASH_TABLE_PTR hash_table -
*     The hash table to add to.
*  UTILEX_IN  uint32                     data_indx -
*     index of the entry to return.
*  UTILEX_OUT  UTILEX_HASH_TABLE_KEY*  const key -
*     key resides in this entry
*  UTILEX_OUT  uint8                    *found -
*     whether this key is valid
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  utilex_hash_table_get_by_index(
    UTILEX_IN  int                           unit,
    UTILEX_IN  UTILEX_HASH_TABLE_PTR       hash_table,
    UTILEX_IN  uint32                        data_indx,
    UTILEX_OUT UTILEX_HASH_TABLE_KEY       *key,
    UTILEX_OUT uint8                         *found
  ) ;
/*********************************************************************
* NAME:
*     utilex_hash_table_clear
* TYPE:
*   PROC
* DATE:
*   Mar  6 2008
* FUNCTION:
*  clear the Hash table content without freeing the memory
* INPUT:
*  UTILEX_IN  int                            unit -
*     Identifier of the device to access.
*  UTILEX_IN  UTILEX_HASH_TABLE_PTR        hash_table -
*     The hash table.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  utilex_hash_table_clear(
    UTILEX_IN     int                          unit,
    UTILEX_INOUT  UTILEX_HASH_TABLE_PTR    hash_table
  ) ;

/*********************************************************************
* NAME:
*     utilex_hash_table_get_next
* TYPE:
*   PROC
* DATE:
*   Mar  6 2008
* FUNCTION:
*  get the next valid entry (key and data) in the hash table.
*  start traversing from the place pointed by the given iterator.
* INPUT:
*  UTILEX_IN  int                            unit -
*     Identifier of the device to access.
*  UTILEX_IN  UTILEX_HASH_TABLE_PTR        hash_table -
*     The hash table.
*  UTILEX_INOUT  UTILEX_HASH_TABLE_ITER   *iter
*     iterator points to the entry to start traverse from.
*   UTILEX_OUT  UTILEX_HASH_TABLE_KEY      key -
*     the hash table key returned
*   UTILEX_OUT  UTILEX_HASH_TABLE_DATA     data -
*     the hash table data returned and associated with the key above.
* REMARKS:
*     - to start traverse the hash table from the beginning.
*       use UTILEX_HASH_TABLE_ITER_BEGIN(iter)
*     - to check if the iterator get to the end of the table use.
*       use UTILEX_HASH_TABLE_ITER_END(iter)
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  utilex_hash_table_get_next(
    UTILEX_IN     int                         unit,
    UTILEX_IN     UTILEX_HASH_TABLE_PTR     hash_table,
    UTILEX_INOUT  UTILEX_HASH_TABLE_ITER    *iter,
    UTILEX_OUT    UTILEX_HASH_TABLE_KEY     *const key,
    UTILEX_OUT    uint32                      *data_indx
  );


/*********************************************************************
* NAME:
*     utilex_hash_table_get_size_for_save
* TYPE:
*   PROC
* DATE:
*   Mar  6 2008
* FUNCTION:
*     returns the size of the buffer needed to return the hash table as buffer.
*     in order to be loaded later
* INPUT:
*   UTILEX_IN  int                     unit -
*     Identifier of the device to access.
*   UTILEX_IN  UTILEX_HASH_TABLE_PTR hash_table -
*     The Hash table to get the size for.
*   UTILEX_OUT  uint32                *size -
*     the size of the buffer.
* REMARKS:
*   None.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  utilex_hash_table_get_size_for_save(
    UTILEX_IN   int                            unit,
    UTILEX_IN   UTILEX_HASH_TABLE_PTR        hash_table,
    UTILEX_OUT  uint32                         *size
  );

/*********************************************************************
* NAME:
*     utilex_hash_table_save
* TYPE:
*   PROC
* DATE:
*   Mar  6 2008
* FUNCTION:
*     saves the given hash table in the given buffer
* INPUT:
*   UTILEX_IN  int                     unit -
*     Identifier of the device to access.
*   UTILEX_IN  UTILEX_HASH_TABLE_PTR hash_table -
*     The Hash table to save.
*   UTILEX_OUT  uint8                 *buffer -
*     buffer to include the hast table
* REMARKS:
*   - the size of the buffer has to be at least as the value returned
*     by utilex_hash_table_get_size_for_save.
*   - the hash and rehash functions are not saved.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  utilex_hash_table_save(
    UTILEX_IN  int                          unit,
    UTILEX_IN  UTILEX_HASH_TABLE_PTR      hash_table,
    UTILEX_OUT uint8                        *buffer,
    UTILEX_IN  uint32                       buffer_size_bytes,
    UTILEX_OUT uint32                       *actual_size_bytes
  );


/*********************************************************************
* NAME:
*     utilex_hash_table_load
* TYPE:
*   PROC
* DATE:
*   Mar  6 2008
* FUNCTION:
*     Load from the given buffer the hash table saved in this buffer.
* INPUT:
*   UTILEX_IN  int                                   unit -
*     Identifier of the device to access.
*   UTILEX_IN  uint8                                 **buffer -
*     buffer includes the hast table
*   UTILEX_IN  UTILEX_HASH_MAP_HASH_FUN_CALL_BACK  hash_function -
*     the hash function of the hash table.
*   UTILEX_IN  UTILEX_HASH_MAP_HASH_FUN_CALL_BACK  hash_function -
*     the hash function of the hash table.
*   UTILEX_OUT  UTILEX_HASH_TABLE_PTR              *hash_table_ptr -
*     This procedure loads pointed memory by the handle of the
*     newly created (and newly loaded from 'buffer) Hash table.
* REMARKS:
*   - the size of the buffer has to be at least as the value returned
*     by utilex_hash_table_get_size_for_save.
*   - there is need to supply the hash and rehash function (in case they are not
*     the default implementation, cause in the save they are not saved.
*     by utilex_hash_table_get_size_for_save.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  utilex_hash_table_load(
    UTILEX_IN  int                                   unit,
    UTILEX_IN  uint8                                 **buffer,
    UTILEX_IN  UTILEX_HASH_MAP_SW_DB_ENTRY_SET     set_entry_fun,
    UTILEX_IN  UTILEX_HASH_MAP_SW_DB_ENTRY_GET     get_entry_fun,
    UTILEX_IN  UTILEX_HASH_MAP_HASH_FUN_CALL_BACK  hash_function,
    UTILEX_IN  UTILEX_HASH_MAP_HASH_FUN_CALL_BACK  rehash_function,
    UTILEX_OUT UTILEX_HASH_TABLE_PTR               *hash_table_ptr
  );

uint32
  utilex_SAND_HASH_TABLE_INFO_clear(
    UTILEX_IN  int                      unit,
    UTILEX_IN  UTILEX_HASH_TABLE_PTR  hash_table
  ) ;


#ifdef UTILEX_DEBUG
/* { */
/*********************************************************************
* NAME:
*     utilex_hash_table_print
* TYPE:
*   PROC
* DATE:
*   Mar  6 2008
* FUNCTION:
*     prints the hash table content, all entries including entries not in use.
* INPUT:
*  UTILEX_IN  UTILEX_HASH_TABLE_PTR hash_table -
*     The hash table to print.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  utilex_hash_table_print(
    UTILEX_IN  int                     unit,
    UTILEX_IN  UTILEX_HASH_TABLE_PTR hash_table
  );

/*********************************************************************
* NAME:
*     utilex_hash_table_tests
* TYPE:
*   PROC
* DATE:
*   Mar  6 2008
* FUNCTION:
*     Tests the hash table module
*
*INPUT:
*  UTILEX_IN  int     unit -
*    Identifier of the device to access.
*  UTILEX_DIRECT:
*    UTILEX_IN uint32 silent -
*    Indicator.
*    1 - Do not print debuging info.
*    0 - Print various debuging info.
*  UTILEX_INDIRECT:
*    None.
*OUTPUT:
*  UTILEX_DIRECT:
*    uint32 -
*    Indicator.
*    1 - Test pass.
*    0 - Test fail.
*  UTILEX_INDIRECT:
*    NON
*REMARKS:* SW License Agreement: Dune Networks (c). CONFIDENTIAL PROPRIETARY INFORMATION.
* Any use of this Software is subject to Software License Agreement
* included in the Driver User Manual of this device.
* Any use of this Software constitutes an agreement to the terms
* of the above Software License Agreement.
********************************************************************/
uint32
  utilex_hash_table_tests(
    UTILEX_IN int unit,
    UTILEX_IN uint8 silent
);

uint32
  utilex_SAND_HASH_TABLE_INFO_print(
    UTILEX_IN int                      unit,
    UTILEX_IN  UTILEX_HASH_TABLE_PTR hash_table
  ) ;
/* } */
#endif /* UTILEX_DEBUG */

/* } */

/* } UTILEX_HASHTABLE_H_INCLUDED*/
#endif


