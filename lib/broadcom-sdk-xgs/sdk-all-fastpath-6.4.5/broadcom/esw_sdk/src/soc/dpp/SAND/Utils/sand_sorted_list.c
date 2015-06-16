/* $Id: sand_sorted_list.c,v 1.8 Broadcom SDK $
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

#include <soc/dpp/SAND/Utils/sand_sorted_list.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/dpp_wb_engine.h>


/* } */

/*************
* DEFINES   *
*************/
/* { */

/* $Id: sand_sorted_list.c,v 1.8 Broadcom SDK $
 * the place of the head of the list.
 */
#define SOC_SAND_SORTED_LIST_HEAD_PLACE(init_info_ptr)  ((init_info_ptr)->list_size)
#define SOC_SAND_SORTED_LIST_TAIL_PLACE(init_info_ptr)  ((init_info_ptr)->list_size + 1)


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
typedef uint8 SOC_SAND_SORTED_LIST_KEY_TYPE;
typedef uint8 SOC_SAND_SORTED_LIST_DATA_TYPE;


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

/*********************************************************************
* NAME:
*     soc_sand_sorted_list_find_match_entry
* TYPE:
*   PROC
* DATE:
*   Mar  6 2008
* FUNCTION:
*  lookup in the sorted list for the given key and return the data inserted with
*  the given key.
* INPUT:
*  SOC_SAND_IN  SOC_SAND_SORTED_LIST_PTR        sorted_list -
*     The sorted list.
*   SOC_SAND_IN  SOC_SAND_SORTED_LIST_KEY*       const key -
*     The key to lookup for
*   SOC_SAND_IN  uint8                  first_empty
*     whether to return the first empty entry .
*   SOC_SAND_OUT  uint32                  *entry -
*     if the key is present in the sorted list then return the entry the key found at,
*     otherwise it returns the place where the key suppose to be.
*   SOC_SAND_IN  uint8                    *found -
*     whether the key was found in the sorted list
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

STATIC uint32
  soc_sand_sorted_list_find_match_entry(
    SOC_SAND_INOUT  SOC_SAND_SORTED_LIST_INFO  *sorted_list,
    SOC_SAND_IN  SOC_SAND_SORTED_LIST_KEY*       const key,
    SOC_SAND_IN  SOC_SAND_SORTED_LIST_DATA*       const data,
    SOC_SAND_IN  uint8                  first_match,
    SOC_SAND_OUT  uint8                 *found,
    SOC_SAND_OUT  SOC_SAND_SORTED_LIST_ITER    *prev_node,
    SOC_SAND_OUT  SOC_SAND_SORTED_LIST_ITER    *cur_node
  );

STATIC uint32
  soc_sand_sorted_list_node_alloc(
    SOC_SAND_INOUT  SOC_SAND_SORTED_LIST_INFO    *sorted_list,
    SOC_SAND_IN  SOC_SAND_SORTED_LIST_KEY*       const key,
    SOC_SAND_IN   SOC_SAND_SORTED_LIST_DATA*       const data,
    SOC_SAND_IN   uint32                   prev_node,
    SOC_SAND_IN   uint32                   next_node,
    SOC_SAND_OUT  uint8                 *found
  );

/*********************************************************************
* NAME:
*     soc_sand_sorted_list_get_next_aux
* TYPE:
*   PROC
* DATE:
*   Mar  6 2008
* FUNCTION:
*  get the next valid entry (key and data) in the sorted list.
*  start traversing from the place pointed by the given iterator.
* INPUT:
*  SOC_SAND_IN  SOC_SAND_SORTED_LIST_PTR        sorted_list -
*     The sorted list.
*  SOC_SAND_INOUT  SOC_SAND_SORTED_LIST_ITER    *iter
*     iterator points to the entry to start traverse from.
*   SOC_SAND_OUT  SOC_SAND_SORTED_LIST_KEY*       const key -
*     the sorted list key returned
*   SOC_SAND_OUT  SOC_SAND_SORTED_LIST_DATA  data -
*     the sorted list data returned and associated with the key above.
* REMARKS:
*     - to start traverse the sorted list from the beginning.
*       use SOC_SAND_SORTED_LIST_ITER_BEGIN(iter)
*     - to check if the iterator get to the end of the table use.
*       use SOC_SAND_SORTED_LIST_ITER_END(iter)
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_sand_sorted_list_get_next_aux(
    SOC_SAND_IN  SOC_SAND_SORTED_LIST_INFO       *sorted_list,
    SOC_SAND_IN  SOC_SAND_SORTED_LIST_ITER       curr,
    SOC_SAND_IN  uint8                   forward,
    SOC_SAND_OUT  SOC_SAND_SORTED_LIST_KEY*       const key,
    SOC_SAND_OUT  SOC_SAND_SORTED_LIST_DATA*       const data
  );

STATIC uint32
  soc_sand_sorted_list_node_link_set(
    SOC_SAND_INOUT  SOC_SAND_SORTED_LIST_INFO  *sorted_list,
    SOC_SAND_IN  uint32                   node1,
    SOC_SAND_IN  uint32                   node2
  );


/************************************************************************/
/*  End of internals                                                    */
/************************************************************************/



uint32
  soc_sand_sorted_list_create(
    SOC_SAND_IN     int                            unit,
    SOC_SAND_INOUT  SOC_SAND_SORTED_LIST_INFO     *sorted_list
  )
{
  SOC_SAND_SORTED_LIST_INIT_INFO
    *init_info_ptr;
  SOC_SAND_SORTED_LIST_T
    *list_data_ptr;
  SOC_SAND_OCC_BM_INIT_INFO
    btmp_init_info;
  uint32
    tmp_size;
  uint32
    res;
  int32 orig_wb_var_index;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_HASH_TABLE_CREATE);

  SOC_SAND_CHECK_NULL_INPUT(sorted_list);
  SOC_SAND_CHECK_NULL_INPUT(sorted_list->init_info.get_entry_fun);
  SOC_SAND_CHECK_NULL_INPUT(sorted_list->init_info.set_entry_fun);

  init_info_ptr = &(sorted_list->init_info);
  list_data_ptr = &(sorted_list->list_data);
  if (sorted_list->init_info.cmp_key_fun == NULL)
  {
    sorted_list->init_info.cmp_key_fun = (SOC_SAND_SORTED_LIST_KEY_CMP)soc_sand_os_memcmp;
  }

  if (init_info_ptr->list_size == 0 || init_info_ptr->key_size == 0 ||
      init_info_ptr->data_size == 0)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_VALUE_OUT_OF_RANGE_ERR, 10, exit);
  }
 /*
  * allocate the sorted list struct
  */

 /*
  * calculate the size of pointers (list head and next) according to table size.
  */
  list_data_ptr->ptr_size = (soc_sand_log2_round_up(sorted_list->init_info.list_size + 2) + (SOC_SAND_NOF_BITS_IN_BYTE - 1)) / SOC_SAND_NOF_BITS_IN_BYTE;
  list_data_ptr->null_ptr = sorted_list->init_info.list_size + 1;

  tmp_size = SOC_SAND_MAX(list_data_ptr->ptr_size,init_info_ptr->data_size);
 /*
  * allocate buffer the temps
  */
  list_data_ptr->tmp_data = (uint8*)soc_sand_os_malloc_any_size(tmp_size * sizeof(uint8), "list_data_ptr->tmp_data");
  list_data_ptr->tmp_key = (uint8*)soc_sand_os_malloc_any_size(init_info_ptr->key_size * sizeof(uint8),"list_data_ptr->tmp_key");
 /*
  * allocate buffer for keys
  */
  list_data_ptr->keys = (uint8*)soc_sand_os_malloc_any_size(init_info_ptr->list_size * init_info_ptr->key_size * sizeof(uint8),"list_data_ptr->keys");
  if (!list_data_ptr->keys)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_MALLOC_FAIL, 20, exit);
  }
  soc_sand_os_memset(
    list_data_ptr->keys,
    0x0,
    init_info_ptr->list_size * init_info_ptr->key_size * sizeof(uint8)
  );

 /*
  * allocate buffer for next array (to build the linked list) one additional for head.
  * which is the last in the next pointers array.
  */
  list_data_ptr->next = (uint8*)soc_sand_os_malloc_any_size((init_info_ptr->list_size + 2) * list_data_ptr->ptr_size * sizeof(uint8), "list_data_ptr->next");
  if (!list_data_ptr->next)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_MALLOC_FAIL, 30, exit);
  }
  soc_sand_os_memset(
    list_data_ptr->next,
    0xFF,
    (init_info_ptr->list_size + 2) * list_data_ptr->ptr_size * sizeof(uint8)
  );

 /*
  * allocate buffer for prev array (to build the linked list)
  */
  list_data_ptr->prev = (uint8*)soc_sand_os_malloc_any_size((init_info_ptr->list_size + 2) * list_data_ptr->ptr_size * sizeof(uint8), "list_data_ptr->prev");
  if (!list_data_ptr->prev)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_MALLOC_FAIL, 40, exit);
  }
  soc_sand_os_memset(
    list_data_ptr->prev,
    0xFF,
    (init_info_ptr->list_size + 2) * list_data_ptr->ptr_size * sizeof(uint8)
  );


 /*
  * allocate buffer for the data array
  */
  list_data_ptr->data = (uint8*)soc_sand_os_malloc_any_size(init_info_ptr->list_size * init_info_ptr->data_size * sizeof(uint8), "list_data_ptr->data");
  if (!list_data_ptr->data)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_MALLOC_FAIL, 50, exit);
  }

  soc_sand_os_memset(
    list_data_ptr->data,
    0x0,
    init_info_ptr->list_size * init_info_ptr->data_size * sizeof(uint8)
  );


 /*
  * connect the head with the tail.
  */
  /* temporary setting wb_var_index to NONE, since buffer is not initialized yet, */
  /* soc_sand_sorted_list_node_link_set function can not call to SOC_DPP_WB_ENGINE_SET_ARR  */
  orig_wb_var_index           = init_info_ptr->wb_var_index;
  init_info_ptr->wb_var_index = SOC_DPP_WB_ENGINE_VAR_NONE;

  res = soc_sand_sorted_list_node_link_set(
          sorted_list,
          SOC_SAND_SORTED_LIST_HEAD_PLACE(init_info_ptr),
          SOC_SAND_SORTED_LIST_TAIL_PLACE(init_info_ptr)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

  init_info_ptr->wb_var_index = orig_wb_var_index;

 /* initialize the data to be mapped to*/
  soc_sand_SAND_OCC_BM_INIT_INFO_clear(&btmp_init_info);
  btmp_init_info.size = init_info_ptr->list_size;

  if(SOC_DPP_WB_ENGINE_VAR_NONE != init_info_ptr->wb_var_index)
  {
      btmp_init_info.wb_var_index = init_info_ptr->wb_var_index + WB_ENGINE_SORTED_LIST_MEMORY_USE;
      btmp_init_info.unit    = init_info_ptr->unit;      
  }

  res = soc_sand_occ_bm_create(
          unit,
          &btmp_init_info,
          &(list_data_ptr->memory_use)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_sorted_list_create()",0,0);
}


uint32
  soc_sand_sorted_list_clear(
    SOC_SAND_IN     int                            unit,
    SOC_SAND_INOUT  SOC_SAND_SORTED_LIST_INFO     *sorted_list
  )
{
  SOC_SAND_SORTED_LIST_INIT_INFO
    *init_info_ptr;
  SOC_SAND_SORTED_LIST_T
    *list_data_ptr;
  uint32
    res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_HASH_TABLE_CREATE);

  SOC_SAND_CHECK_NULL_INPUT(sorted_list);
  SOC_SAND_CHECK_NULL_INPUT(sorted_list->init_info.get_entry_fun);
  SOC_SAND_CHECK_NULL_INPUT(sorted_list->init_info.set_entry_fun);

  init_info_ptr = &(sorted_list->init_info);
  list_data_ptr = &(sorted_list->list_data);



  if(SOC_DPP_WB_ENGINE_VAR_NONE == init_info_ptr->wb_var_index) 
  {

      soc_sand_os_memset(list_data_ptr->keys,
                         0x0,
                         init_info_ptr->list_size * init_info_ptr->key_size * sizeof(uint8));

      soc_sand_os_memset(list_data_ptr->next,
                         0xFF,
                         (init_info_ptr->list_size + 2) * list_data_ptr->ptr_size * sizeof(uint8));

      soc_sand_os_memset(list_data_ptr->prev,
                         0xFF,
                         (init_info_ptr->list_size + 2) * list_data_ptr->ptr_size * sizeof(uint8));


      soc_sand_os_memset(list_data_ptr->data,
                         0x0,
                         init_info_ptr->list_size * init_info_ptr->data_size * sizeof(uint8));
  } 
  else 
  {
      res = SOC_DPP_WB_ENGINE_MEMSET_ARR(init_info_ptr->unit, 
                                         init_info_ptr->wb_var_index + WB_ENGINE_SORTED_LIST_KEYS, 
                                         0x0);
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
          
      res = SOC_DPP_WB_ENGINE_MEMSET_ARR(init_info_ptr->unit, 
                                         init_info_ptr->wb_var_index + WB_ENGINE_SORTED_LIST_NEXT,
                                         0xFF);
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
          
      res = SOC_DPP_WB_ENGINE_MEMSET_ARR(init_info_ptr->unit, 
                                         init_info_ptr->wb_var_index + WB_ENGINE_SORTED_LIST_PREV,
                                         0xFF);
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
          
      res = SOC_DPP_WB_ENGINE_MEMSET_ARR(init_info_ptr->unit, 
                                         init_info_ptr->wb_var_index + WB_ENGINE_SORTED_LIST_DATA, 
                                         0x0);
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);       
  }
 
 /*
  * connect the head with the tail.
  */
  res = soc_sand_sorted_list_node_link_set(
          sorted_list,
          SOC_SAND_SORTED_LIST_HEAD_PLACE(init_info_ptr),
          SOC_SAND_SORTED_LIST_TAIL_PLACE(init_info_ptr)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);


  res = soc_sand_occ_bm_clear(
          unit,
          list_data_ptr->memory_use
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_sorted_list_clear()",0,0);
}

uint32
  soc_sand_sorted_list_destroy(
    SOC_SAND_IN     int                         unit,
    SOC_SAND_INOUT  SOC_SAND_SORTED_LIST_INFO  *sorted_list
    )
{
  SOC_SAND_SORTED_LIST_T
    *list_data_ptr;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_SORTED_LIST_DESTROY);
  SOC_SAND_CHECK_NULL_INPUT(sorted_list);

  list_data_ptr = &(sorted_list->list_data);

  soc_sand_os_free_any_size(list_data_ptr->tmp_data);
  soc_sand_os_free_any_size(list_data_ptr->tmp_key);
  soc_sand_os_free_any_size(list_data_ptr->next);
  soc_sand_os_free_any_size(list_data_ptr->keys);
  soc_sand_os_free_any_size(list_data_ptr->prev);
  soc_sand_os_free_any_size(list_data_ptr->data);

  res = soc_sand_occ_bm_destroy(
    unit,
    list_data_ptr->memory_use
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_sorted_list_destroy()",0,0);
}

uint32
  soc_sand_sorted_list_entry_add(
    SOC_SAND_IN     int                           unit,
    SOC_SAND_INOUT  SOC_SAND_SORTED_LIST_INFO    *sorted_list,
    SOC_SAND_IN     SOC_SAND_SORTED_LIST_KEY*     const key,
    SOC_SAND_IN     SOC_SAND_SORTED_LIST_DATA*    const data,
    SOC_SAND_OUT    uint8                        *success
  )
{
  uint8
    found;
  uint32
    curr_node,
    prev_node;
  uint32
    res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_SORTED_LIST_ENTRY_ADD);

  SOC_SAND_CHECK_NULL_INPUT(sorted_list);
  SOC_SAND_CHECK_NULL_INPUT(key);
  SOC_SAND_CHECK_NULL_INPUT(data);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = soc_sand_sorted_list_find_match_entry(
          sorted_list,
          key,
          data,
          FALSE,
          &found,
          &prev_node,
          &curr_node
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  if (found)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_SORTED_LIST_KEY_DATA_ALREADY_EXIST_ERR, 20, exit);
  }
 /*
  * Allocate new node.
  */
  res = soc_sand_sorted_list_node_alloc(
          sorted_list,
          key,
          data,
          prev_node,
          curr_node,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_sorted_list_entry_add()",0,0);
}


uint32
  soc_sand_sorted_list_entry_add_by_iter(
    SOC_SAND_IN     int                           unit,
    SOC_SAND_INOUT  SOC_SAND_SORTED_LIST_INFO    *sorted_list,
    SOC_SAND_IN     SOC_SAND_SORTED_LIST_ITER     pos,
    SOC_SAND_IN     uint8                         before,
    SOC_SAND_IN     SOC_SAND_SORTED_LIST_KEY*     const key,
    SOC_SAND_IN     SOC_SAND_SORTED_LIST_DATA*    const data,
    SOC_SAND_OUT    uint8                        *success
  )
{
  SOC_SAND_SORTED_LIST_INIT_INFO
    *init_info_ptr;
  SOC_SAND_SORTED_LIST_T
    *list_data_ptr;
  SOC_SAND_SORTED_LIST_ITER
    prev_node,
    next_node;
  uint32
    res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_SORTED_LIST_ENTRY_ADD_BY_ITER);

  SOC_SAND_CHECK_NULL_INPUT(sorted_list);

  init_info_ptr = &(sorted_list->init_info);
  list_data_ptr = &(sorted_list->list_data);
 /*
  * cehck if adding after the end or before the beginning.
  */
  if (
      (before &&  pos == SOC_SAND_SORTED_LIST_ITER_BEGIN(sorted_list)) ||
      ((!before && pos == SOC_SAND_SORTED_LIST_ITER_END(sorted_list)))
     )
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_SORTED_LIST_ILLEGAL_ITER_ERR, 10, exit);
  }
 /*
  * check that this adding keep the order.
  */
  prev_node = pos;
  next_node = pos;

  if (before)
  {
    prev_node =
      soc_sand_sorted_list_get_next_aux(
        sorted_list,
        pos,
        FALSE,
        NULL,
        NULL
      );
  }
  else
  {
    next_node =
      soc_sand_sorted_list_get_next_aux(
        sorted_list,
        pos,
        TRUE,
        NULL,
        NULL
      );
  }
  if (prev_node != SOC_SAND_SORTED_LIST_ITER_END(sorted_list) &&  prev_node != SOC_SAND_SORTED_LIST_ITER_BEGIN(sorted_list))
  {
    res = soc_sand_sorted_list_entry_value(
            unit,
            sorted_list,
            prev_node,
            list_data_ptr->tmp_key,
            NULL
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    if (init_info_ptr->cmp_key_fun(list_data_ptr->tmp_key, key, init_info_ptr->key_size) > 0 )
    {
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_SORTED_LIST_ILLEGAL_ITER_ERR, 30, exit);
    }
  }
  if (next_node != SOC_SAND_SORTED_LIST_ITER_END(sorted_list) &&  next_node != SOC_SAND_SORTED_LIST_ITER_BEGIN(sorted_list))
  {
    res = soc_sand_sorted_list_entry_value(
            unit,
            sorted_list,
            next_node,
            list_data_ptr->tmp_key,
            NULL
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    if (init_info_ptr->cmp_key_fun(key, list_data_ptr->tmp_key, init_info_ptr->key_size) > 0 )
    {
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_SORTED_LIST_ILLEGAL_ITER_ERR, 20, exit);
    }
  }
  res = soc_sand_sorted_list_node_alloc(
          sorted_list,
          key,
          data,
          prev_node,
          next_node,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_sorted_list_entry_add_by_iter()",0,0);
}


uint32
  soc_sand_sorted_list_entry_update(
    SOC_SAND_IN     int                               unit,
    SOC_SAND_INOUT  SOC_SAND_SORTED_LIST_INFO        *sorted_list,
    SOC_SAND_IN     SOC_SAND_SORTED_LIST_ITER         iter,
    SOC_SAND_IN     SOC_SAND_SORTED_LIST_KEY*         const key,
    SOC_SAND_IN     SOC_SAND_SORTED_LIST_DATA*        const data
  )
{
  SOC_SAND_SORTED_LIST_INIT_INFO
    *init_info_ptr;
  SOC_SAND_SORTED_LIST_T
    *list_data_ptr;
  uint32
    res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_SORTED_LIST_ENTRY_UPDATE);

  SOC_SAND_CHECK_NULL_INPUT(sorted_list);

  if (iter == SOC_SAND_SORTED_LIST_NULL || iter >= sorted_list->init_info.list_size)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_VALUE_OUT_OF_RANGE_ERR, 10, exit);
  }

  init_info_ptr = &(sorted_list->init_info);
  list_data_ptr = &(sorted_list->list_data);

  /* check to see if the entry exists */
  if (data)
  {
      if(SOC_DPP_WB_ENGINE_VAR_NONE == init_info_ptr->wb_var_index)
      {
          res = init_info_ptr->set_entry_fun(init_info_ptr->prime_handle,
                                             init_info_ptr->sec_handle,
                                             list_data_ptr->data,
                                             iter,
                                             init_info_ptr->data_size,
                                             data);
          SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
      }
      else
      {
          res = SOC_DPP_WB_ENGINE_SET_ARR(init_info_ptr->unit, 
                                          init_info_ptr->wb_var_index + WB_ENGINE_SORTED_LIST_DATA, 
                                          data, 
                                          iter);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);      
      }
  }
  if (key)
  {
      if(SOC_DPP_WB_ENGINE_VAR_NONE == init_info_ptr->wb_var_index)
      {
          res = init_info_ptr->set_entry_fun(init_info_ptr->prime_handle,
                                             init_info_ptr->sec_handle,
                                             list_data_ptr->keys,
                                             iter,
                                             init_info_ptr->key_size,
                                             key);
          SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
      }
      else
      {
          res = SOC_DPP_WB_ENGINE_SET_ARR(init_info_ptr->unit, 
                                          init_info_ptr->wb_var_index + WB_ENGINE_SORTED_LIST_KEYS, 
                                          key, 
                                          iter);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);      
      }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_sorted_list_entry_update()",0,0);
}


uint32
  soc_sand_sorted_list_entry_get_data(
    SOC_SAND_INOUT  SOC_SAND_SORTED_LIST_INFO        *sorted_list,
    SOC_SAND_IN     SOC_SAND_SORTED_LIST_ITER        iter,
    SOC_SAND_OUT     SOC_SAND_SORTED_LIST_DATA*       const data
  )
{
  SOC_SAND_SORTED_LIST_INIT_INFO
    *init_info_ptr;
  SOC_SAND_SORTED_LIST_T
    *list_data_ptr;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);

  SOC_SAND_CHECK_NULL_INPUT(sorted_list);
  SOC_SAND_CHECK_NULL_INPUT(data);

  if (iter == SOC_SAND_SORTED_LIST_ITER_END(sorted_list) || iter >= sorted_list->init_info.list_size)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_VALUE_OUT_OF_RANGE_ERR, 10, exit);
  }

  init_info_ptr = &(sorted_list->init_info);
  list_data_ptr = &(sorted_list->list_data);

  res = init_info_ptr->get_entry_fun(
          init_info_ptr->prime_handle,
          init_info_ptr->sec_handle,
          list_data_ptr->data,
          iter,
          init_info_ptr->data_size,
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_sorted_list_entry_get_data()",0,0);
}



uint32
  soc_sand_sorted_list_entry_remove_by_iter(
    SOC_SAND_IN     int                             unit,
    SOC_SAND_INOUT  SOC_SAND_SORTED_LIST_INFO      *sorted_list,
    SOC_SAND_IN     SOC_SAND_SORTED_LIST_ITER       iter
  )
{
  SOC_SAND_SORTED_LIST_INIT_INFO
    *init_info_ptr;
  SOC_SAND_SORTED_LIST_T
    *list_data_ptr;
  SOC_SAND_SORTED_LIST_ITER
    prev,
    next;
  uint32
    res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_SORTED_LIST_ENTRY_REMOVE_BY_ITER);

  SOC_SAND_CHECK_NULL_INPUT(sorted_list);

  init_info_ptr = &(sorted_list->init_info);
  list_data_ptr = &(sorted_list->list_data);

  next = soc_sand_sorted_list_get_next_aux(
           sorted_list,
           iter,
           TRUE,
           NULL,
           NULL
         );

  prev = soc_sand_sorted_list_get_next_aux(
           sorted_list,
           iter,
           FALSE,
           NULL,
           NULL
         );

  res = soc_sand_sorted_list_node_link_set(
            sorted_list,
            prev,
            next
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

 /*
  * now the entry 'iter' is not in use. free it.
  */
  if(SOC_DPP_WB_ENGINE_VAR_NONE == init_info_ptr->wb_var_index)
  {
      res = init_info_ptr->set_entry_fun(init_info_ptr->prime_handle,
                                         init_info_ptr->sec_handle,
                                         list_data_ptr->next,
                                         iter,
                                         list_data_ptr->ptr_size,
                                         (uint8*)&list_data_ptr->null_ptr);
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }
  else
  {
      res = SOC_DPP_WB_ENGINE_SET_ARR(init_info_ptr->unit, 
                                      init_info_ptr->wb_var_index + WB_ENGINE_SORTED_LIST_NEXT, 
                                      (uint8*)&list_data_ptr->null_ptr, 
                                      iter);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);      
  }





  if(SOC_DPP_WB_ENGINE_VAR_NONE == init_info_ptr->wb_var_index)
  {
      res = init_info_ptr->set_entry_fun(init_info_ptr->prime_handle,
                                         init_info_ptr->sec_handle,
                                         list_data_ptr->prev,
                                         iter,
                                         list_data_ptr->ptr_size,
                                         (uint8*)&list_data_ptr->null_ptr);
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  }
  else
  {
      res = SOC_DPP_WB_ENGINE_SET_ARR(init_info_ptr->unit, 
                                      init_info_ptr->wb_var_index + WB_ENGINE_SORTED_LIST_PREV, 
                                      (uint8*)&list_data_ptr->null_ptr, 
                                      iter);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 50, exit);      
  }

  res = soc_sand_occ_bm_occup_status_set(
          unit,
          list_data_ptr->memory_use,
          iter,
          FALSE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_sorted_list_entry_remove_by_iter()",0,0);
}


uint32
  soc_sand_sorted_list_entry_lookup(
    SOC_SAND_IN     int                           unit,
    SOC_SAND_INOUT  SOC_SAND_SORTED_LIST_INFO    *sorted_list,
    SOC_SAND_IN     SOC_SAND_SORTED_LIST_KEY*     const key,
    SOC_SAND_IN     SOC_SAND_SORTED_LIST_DATA*    const data,
    SOC_SAND_OUT    uint8                        *found,
    SOC_SAND_OUT    SOC_SAND_SORTED_LIST_ITER    *iter
  )
{
  SOC_SAND_SORTED_LIST_T
    *list_data_ptr;
  uint32
    curr_node,
    prev_node;
  uint32
    res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_SORTED_LIST_ENTRY_LOOKUP);

  SOC_SAND_CHECK_NULL_INPUT(sorted_list);
  SOC_SAND_CHECK_NULL_INPUT(key);
  SOC_SAND_CHECK_NULL_INPUT(iter);

  list_data_ptr = &(sorted_list->list_data);

  /* check to see if the entry exists */

  if (data != NULL)
  {
    res = soc_sand_sorted_list_find_match_entry(
            sorted_list,
            key,
            data,
            FALSE,
            found,
            &prev_node,
            &curr_node
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }
  else
  {
    res = soc_sand_sorted_list_find_match_entry(
            sorted_list,
            key,
            data,
            TRUE,
            found,
            &prev_node,
            &curr_node
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    if (*found)
    {
      *iter = curr_node;
    }
    else
    {
      *iter = prev_node;
    }
    goto exit;
  }

  if (found)
  {
    *iter = curr_node;
  }
  else
  {
    *iter = list_data_ptr->null_ptr;
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_sorted_list_entry_lookup()",0,0);
}



uint32
  soc_sand_sorted_list_entry_value(
    SOC_SAND_IN     int                           unit,
    SOC_SAND_INOUT  SOC_SAND_SORTED_LIST_INFO   *sorted_list,
    SOC_SAND_IN     SOC_SAND_SORTED_LIST_ITER    iter,
    SOC_SAND_OUT    SOC_SAND_SORTED_LIST_KEY*    const key,
    SOC_SAND_OUT    SOC_SAND_SORTED_LIST_DATA*   const data
  )
{
  SOC_SAND_SORTED_LIST_INIT_INFO
    *init_info_ptr;
  SOC_SAND_SORTED_LIST_T
    *list_data_ptr;
  uint32
    res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_SORTED_LIST_ENTRY_VALUE);

  SOC_SAND_CHECK_NULL_INPUT(sorted_list);

  if (iter == SOC_SAND_SORTED_LIST_ITER_END(sorted_list) || iter >= sorted_list->init_info.list_size)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_VALUE_OUT_OF_RANGE_ERR, 10, exit);
  }

  init_info_ptr = &(sorted_list->init_info);
  list_data_ptr = &(sorted_list->list_data);

  if (key != NULL)
  {
   /*
    * read keys
    */
    res = init_info_ptr->get_entry_fun(
            init_info_ptr->prime_handle,
            init_info_ptr->sec_handle,
            list_data_ptr->keys,
            iter,
            init_info_ptr->key_size,
            key
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }
  if (data != NULL)
  {
   /*
    * read data
    */
    res = init_info_ptr->get_entry_fun(
            init_info_ptr->prime_handle,
            init_info_ptr->sec_handle,
            list_data_ptr->data,
            iter,
            init_info_ptr->data_size,
            data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_sorted_list_entry_value()",0,0);
}


uint32
  soc_sand_sorted_list_get_next(
    SOC_SAND_IN     int                           unit,
    SOC_SAND_IN     SOC_SAND_SORTED_LIST_INFO    *sorted_list,
    SOC_SAND_INOUT  SOC_SAND_SORTED_LIST_ITER    *iter,
    SOC_SAND_OUT    SOC_SAND_SORTED_LIST_KEY*     const key,
    SOC_SAND_OUT    SOC_SAND_SORTED_LIST_DATA*    const data
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_SORTED_LIST_GET_NEXT);

  SOC_SAND_CHECK_NULL_INPUT(sorted_list);

  if (*iter == SOC_SAND_SORTED_LIST_ITER_END(sorted_list))
  {
    goto exit;
  }


  *iter = soc_sand_sorted_list_get_next_aux(
            sorted_list,
            *iter,
            TRUE,
            key,
            data
          );

  if (*iter == SOC_SAND_SORTED_LIST_ITER_END(sorted_list))
  {
    goto exit;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "soc_sand_sorted_list_get_next()",0,0);
}


uint32
  soc_sand_sorted_list_get_prev(
    SOC_SAND_IN     int                           unit,
    SOC_SAND_IN     SOC_SAND_SORTED_LIST_INFO    *sorted_list,
    SOC_SAND_INOUT  SOC_SAND_SORTED_LIST_ITER    *iter,
    SOC_SAND_OUT    SOC_SAND_SORTED_LIST_KEY*     const key,
    SOC_SAND_OUT    SOC_SAND_SORTED_LIST_DATA*    const data
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_SORTED_LIST_GET_PREV);

  SOC_SAND_CHECK_NULL_INPUT(sorted_list);
  SOC_SAND_CHECK_NULL_INPUT(key);

  if (*iter == SOC_SAND_SORTED_LIST_HEAD_PLACE(&(sorted_list->init_info)))
  {
    goto exit;
  }

  *iter = soc_sand_sorted_list_get_next_aux(
            sorted_list,
            *iter,
            FALSE,
            key,
            data
          );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "soc_sand_sorted_list_get_prev()",0,0);
}



uint32
  soc_sand_sorted_list_get_follow(
    SOC_SAND_IN     int                           unit,
    SOC_SAND_IN     SOC_SAND_SORTED_LIST_INFO    *sorted_list,
    SOC_SAND_INOUT  SOC_SAND_SORTED_LIST_ITER    *iter,
    SOC_SAND_OUT    uint8                         forward,
    SOC_SAND_OUT    SOC_SAND_SORTED_LIST_KEY*     const key,
    SOC_SAND_OUT    SOC_SAND_SORTED_LIST_DATA*    const data
    )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_SORTED_LIST_GET_FOLLOW);

  SOC_SAND_CHECK_NULL_INPUT(sorted_list);
  SOC_SAND_CHECK_NULL_INPUT(key);

  if (*iter == SOC_SAND_SORTED_LIST_HEAD_PLACE(&(sorted_list->init_info)))
  {
    goto exit;
  }

  *iter = soc_sand_sorted_list_get_next_aux(
            sorted_list,
            *iter,
            forward,
            key,
            data
          );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "soc_sand_sorted_list_get_follow()",0,0);
}

/*********************************************************************
* NAME:
*     soc_sand_sorted_list_get_size_for_save
* TYPE:
*   PROC
* DATE:
*   Mar  6 2008
* FUNCTION:
*     returns the size of the buffer needed to return the sorted list as buffer.
*     in sort to be loaded later
* INPUT:
*   SOC_SAND_IN  int                         unit -
*     Identifier of the device to access.
*   SOC_SAND_IN  SOC_SAND_SORTED_LIST_INFO  *sorted_list -
*     The sorted list to get the size for.
*   SOC_SAND_OUT  uint32                    *size -
*     the size of the buffer.
* REMARKS:
*   None.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
soc_sand_sorted_list_get_size_for_save(
     SOC_SAND_IN   int                             unit,
     SOC_SAND_IN   SOC_SAND_SORTED_LIST_INFO      *sorted_list,
     SOC_SAND_OUT  uint32                         *size
   )
{
  const SOC_SAND_SORTED_LIST_INIT_INFO
    *init_info_ptr;
  const SOC_SAND_SORTED_LIST_T
    *list_data_ptr;
  uint32
    bmp_size,
    total_size=0;
  uint32
    res;
  uint32
    tmp_size;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(sorted_list);
  SOC_SAND_CHECK_NULL_INPUT(size);

  init_info_ptr = &(sorted_list->init_info);
  list_data_ptr = &(sorted_list->list_data);

  tmp_size = SOC_SAND_MAX(list_data_ptr->ptr_size,init_info_ptr->data_size);

  /* init info */
  total_size += sizeof(SOC_SAND_SORTED_LIST_INIT_INFO);

  /* DS data */

  total_size += sizeof(uint8) * tmp_size;

  total_size += sizeof(uint8) * init_info_ptr->key_size;

  total_size += sizeof(uint8) * init_info_ptr->list_size * init_info_ptr->key_size;
  
  total_size += sizeof(uint8) * (init_info_ptr->list_size + 2) * list_data_ptr->ptr_size;

  total_size += sizeof(uint8) * (init_info_ptr->list_size + 2) * list_data_ptr->ptr_size;

  total_size += sizeof(uint8) * init_info_ptr->list_size * init_info_ptr->data_size;


  /* initialize the data to be mapped to*/
  res = soc_sand_occ_bm_get_size_for_save(
    unit,
    list_data_ptr->memory_use,
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
*     soc_sand_sorted_list_save
* TYPE:
*   PROC
* DATE:
*   Mar  6 2008
* FUNCTION:
*     saves the given sorted list in the given buffer
* INPUT:
*   SOC_SAND_IN  int                           unit -
*     Identifier of the device to access.
*   SOC_SAND_IN  SOC_SAND_SORTED_LIST_INFO    *sorted_list -
*     The sorted list to save.
*   SOC_SAND_OUT  uint8                       *buffer -
*     buffer to include the hast table
*   SOC_SAND_IN  uint32                        buffer_size_bytes,
*   SOC_SAND_OUT uint32                       *actual_size_bytes
* REMARKS:
*   - the size of the buffer has to be at least as the value returned
*     by soc_sand_sorted_list_get_size_for_save.
*   - the hash and rehash functions are not saved.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
soc_sand_sorted_list_save(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_SAND_SORTED_LIST_INFO    *sorted_list,
    SOC_SAND_OUT uint8                        *buffer,
    SOC_SAND_IN  uint32                        buffer_size_bytes,
    SOC_SAND_OUT uint32                       *actual_size_bytes
  )
{
  const SOC_SAND_SORTED_LIST_INIT_INFO
    *init_info_ptr;
  const SOC_SAND_SORTED_LIST_T
    *list_data_ptr;
  uint8 
    *cur_ptr = (uint8*)buffer;
  uint32
    tmp_size;
  uint32
    cur_size,
    total_size=0;
  uint32
    res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(sorted_list);
  SOC_SAND_CHECK_NULL_INPUT(buffer);
  SOC_SAND_CHECK_NULL_INPUT(actual_size_bytes);

  init_info_ptr = &(sorted_list->init_info);
  list_data_ptr = &(sorted_list->list_data);



  tmp_size = SOC_SAND_MAX(list_data_ptr->ptr_size,init_info_ptr->data_size);

  /* copy init info */
  SOC_SAND_COPY_TO_BUFF_AND_INC(cur_ptr,init_info_ptr,SOC_SAND_SORTED_LIST_INIT_INFO,1);  

  /* copy DS data */
  SOC_SAND_COPY_TO_BUFF_AND_INC(cur_ptr,list_data_ptr->tmp_data, uint8, tmp_size);

  SOC_SAND_COPY_TO_BUFF_AND_INC(cur_ptr,list_data_ptr->tmp_key, uint8, init_info_ptr->key_size);

  SOC_SAND_COPY_TO_BUFF_AND_INC(cur_ptr,list_data_ptr->keys, uint8, init_info_ptr->list_size * init_info_ptr->key_size);

  SOC_SAND_COPY_TO_BUFF_AND_INC(cur_ptr,list_data_ptr->next,uint8,(init_info_ptr->list_size + 2) * list_data_ptr->ptr_size);

  SOC_SAND_COPY_TO_BUFF_AND_INC(cur_ptr,list_data_ptr->prev,uint8,(init_info_ptr->list_size + 2) * list_data_ptr->ptr_size);

  SOC_SAND_COPY_TO_BUFF_AND_INC(cur_ptr,list_data_ptr->data,uint8,init_info_ptr->list_size * init_info_ptr->data_size);

  /* initialize the data to be mapped to*/
  res = soc_sand_occ_bm_save(
          unit,
          list_data_ptr->memory_use,
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


/*********************************************************************
* NAME:
*     soc_sand_sorted_list_load
* TYPE:
*   PROC
* DATE:
*   Mar  6 2008
* FUNCTION:
*     load from the given buffer the sorted list saved in this buffer.
* INPUT:
*   SOC_SAND_IN  int                                   unit -
*     Identifier of the device to access.
*   SOC_SAND_IN  uint8                               **buffer -
*     buffer includes the sorted list
*   SOC_SAND_IN  SOC_SAND_SORTED_LIST_SW_DB_ENTRY_GET  get_entry_fun,
*   SOC_SAND_IN  SOC_SAND_SORTED_LIST_SW_DB_ENTRY_SET  set_entry_fun,
*   SOC_SAND_IN  SOC_SAND_SORTED_LIST_KEY_CMP          cmp_key_fun,
*   SOC_SAND_OUT  SOC_SAND_SORTED_LIST_INFO           *sorted_list -
*     The sorted list to load.
* REMARKS:
*   - the size of the buffer has to be at least as the value returned
*     by soc_sand_sorted_list_get_size_for_save.
*   - there is need to supply the hash and rehash function (in case they are not
*     the default implementation, cause in the save they are not saved.
*     by soc_sand_sorted_list_get_size_for_save.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_sand_sorted_list_load(
    SOC_SAND_IN  int                                   unit,
    SOC_SAND_IN  uint8                               **buffer,
    SOC_SAND_IN  SOC_SAND_SORTED_LIST_SW_DB_ENTRY_GET  get_entry_fun,
    SOC_SAND_IN  SOC_SAND_SORTED_LIST_SW_DB_ENTRY_SET  set_entry_fun,
    SOC_SAND_IN  SOC_SAND_SORTED_LIST_KEY_CMP          cmp_key_fun,                      
    SOC_SAND_OUT SOC_SAND_SORTED_LIST_INFO            *sorted_list                      
  )
{
  const SOC_SAND_SORTED_LIST_INIT_INFO
    *init_info_ptr;
  SOC_SAND_SORTED_LIST_T
    *list_data_ptr;
  SOC_SAND_IN uint8
    *cur_ptr = (SOC_SAND_IN uint8*)buffer[0];
  uint32
    res;
  uint32
    tmp_size;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(sorted_list);
  SOC_SAND_CHECK_NULL_INPUT(buffer);

  soc_sand_SAND_SORTED_LIST_INFO_clear(sorted_list);

  /* copy init info */
  soc_sand_os_memcpy(&(sorted_list->init_info), cur_ptr, sizeof(SOC_SAND_SORTED_LIST_INIT_INFO));
  cur_ptr += sizeof(SOC_SAND_SORTED_LIST_INIT_INFO);
  sorted_list->init_info.get_entry_fun = get_entry_fun;
  sorted_list->init_info.set_entry_fun = set_entry_fun;
  sorted_list->init_info.cmp_key_fun = cmp_key_fun;  

  /* create DS*/
  res = soc_sand_sorted_list_create(unit, sorted_list);
  SOC_SAND_CHECK_FUNC_RESULT(res,20, exit);

  init_info_ptr = &(sorted_list->init_info);
  list_data_ptr = &(sorted_list->list_data);

  tmp_size = SOC_SAND_MAX(list_data_ptr->ptr_size,init_info_ptr->data_size);

  /* copy DS data */

  soc_sand_os_memcpy(list_data_ptr->tmp_data, cur_ptr, sizeof(uint8) * tmp_size);
  cur_ptr += sizeof(uint8) * tmp_size;

  soc_sand_os_memcpy(list_data_ptr->tmp_key, cur_ptr, sizeof(uint8) * init_info_ptr->key_size);
  cur_ptr += sizeof(uint8) * init_info_ptr->key_size;

  soc_sand_os_memcpy(list_data_ptr->keys, cur_ptr, sizeof(uint8) * init_info_ptr->list_size * init_info_ptr->key_size);
  cur_ptr += sizeof(uint8) * init_info_ptr->list_size * init_info_ptr->key_size;

  soc_sand_os_memcpy(list_data_ptr->next, cur_ptr, sizeof(uint8) * (init_info_ptr->list_size + 2) * list_data_ptr->ptr_size);
  cur_ptr += sizeof(uint8) * (init_info_ptr->list_size + 2) * list_data_ptr->ptr_size;

  soc_sand_os_memcpy(list_data_ptr->prev, cur_ptr, sizeof(uint8) * (init_info_ptr->list_size + 2) * list_data_ptr->ptr_size);
  cur_ptr += sizeof(uint8) * (init_info_ptr->list_size + 2) * list_data_ptr->ptr_size;

  soc_sand_os_memcpy(list_data_ptr->data, cur_ptr, sizeof(uint8) * init_info_ptr->list_size * init_info_ptr->data_size);
  cur_ptr += sizeof(uint8) * init_info_ptr->list_size * init_info_ptr->data_size;

  /* load bitmap*/
  res = soc_sand_occ_bm_destroy(
    unit,
    list_data_ptr->memory_use
    );
  SOC_SAND_CHECK_FUNC_RESULT(res,30, exit);

  /* load bitmap*/
  res = soc_sand_occ_bm_load(
    unit,
    &cur_ptr,
    &list_data_ptr->memory_use
    );
  SOC_SAND_CHECK_FUNC_RESULT(res,30, exit);

  *buffer = cur_ptr;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_hash_table_load()",0,0);



}



uint32
  soc_sand_sorted_list_print(
    SOC_SAND_IN  SOC_SAND_SORTED_LIST_INFO         *sorted_list,
    SOC_SAND_IN  char                     table_header[SOC_SAND_SORTED_LIST_HEADER_SIZE],
    SOC_SAND_IN  SOC_SAND_SORTED_LIST_PRINT_VAL    print_key,
    SOC_SAND_IN  SOC_SAND_SORTED_LIST_PRINT_VAL    print_data
  )
{
  SOC_SAND_IN SOC_SAND_SORTED_LIST_INIT_INFO
    *init_info_ptr;
  SOC_SAND_IN SOC_SAND_SORTED_LIST_T
    *list_data_ptr;
  uint32
    prev,
    curr;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(SOC_SAND_SORTED_LIST_PRINT);

  SOC_SAND_CHECK_NULL_INPUT(sorted_list);

  init_info_ptr = &(sorted_list->init_info);
  list_data_ptr = &(sorted_list->list_data);

 /*
  * traverse the sorted list head list.
  */

  LOG_CLI((BSL_META_U(unit,
                      "  %s\n"), table_header));
  prev = SOC_SAND_SORTED_LIST_HEAD_PLACE(init_info_ptr);
  do
  {
   /*
    * read next entry.
    */
    curr = soc_sand_sorted_list_get_next_aux(
            sorted_list,
            prev,
            TRUE,
            list_data_ptr->tmp_key,
            list_data_ptr->tmp_data
          );
    if (curr == SOC_SAND_SORTED_LIST_ITER_END(sorted_list))
    {
      goto exit;
    }
    LOG_CLI((BSL_META_U(unit,
                        "   %-10u"), curr));
    print_data(list_data_ptr->tmp_data);
    print_key(list_data_ptr->tmp_key);
    LOG_CLI((BSL_META_U(unit,
                        "\n")));

    prev = curr;
  }
  while(curr != SOC_SAND_SORTED_LIST_ITER_END(sorted_list));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_sorted_list_print()",0,0);
}



int32
  soc_sand_sorted_list_data_cmp(
    SOC_SAND_IN  SOC_SAND_SORTED_LIST_INFO       *sorted_list,
    SOC_SAND_IN  uint32                     data_place,
    SOC_SAND_IN  SOC_SAND_SORTED_LIST_DATA*       const data
  )
{
  SOC_SAND_IN SOC_SAND_SORTED_LIST_INIT_INFO
    *init_info_ptr;
  SOC_SAND_IN SOC_SAND_SORTED_LIST_T
    *list_data_ptr;
  uint32
    cmp_res;

  init_info_ptr = &(sorted_list->init_info);
  list_data_ptr = &(sorted_list->list_data);

 /*
  * traverse the sorted list head list.
  */
   /*
    * read next entry.
    */
     /*
      * read data
      */
      init_info_ptr->get_entry_fun(
              init_info_ptr->prime_handle,
              init_info_ptr->sec_handle,
              list_data_ptr->data,
              data_place,
              init_info_ptr->data_size,
              list_data_ptr->tmp_data
            );

      cmp_res = init_info_ptr->cmp_key_fun(
          list_data_ptr->tmp_data,
          data,
          (init_info_ptr->data_size * sizeof(uint8))
        );

      return cmp_res;
}



SOC_SAND_SORTED_LIST_ITER
  soc_sand_sorted_list_get_next_aux(
    SOC_SAND_IN  SOC_SAND_SORTED_LIST_INFO       *sorted_list,
    SOC_SAND_IN  SOC_SAND_SORTED_LIST_ITER       curr,
    SOC_SAND_IN  uint8                   forward,
    SOC_SAND_OUT  SOC_SAND_SORTED_LIST_KEY*       const key,
    SOC_SAND_OUT  SOC_SAND_SORTED_LIST_DATA*       const data
  )
{
  SOC_SAND_IN SOC_SAND_SORTED_LIST_INIT_INFO
    *init_info_ptr;
  SOC_SAND_IN SOC_SAND_SORTED_LIST_T
    *list_data_ptr;
  uint32
    ptr_long;

  init_info_ptr = &(sorted_list->init_info);
  list_data_ptr = &(sorted_list->list_data);

 /*
  * traverse the sorted list head list.
  */
 /*
  * read next entry.
  */
  if (
    (!forward &&  curr == SOC_SAND_SORTED_LIST_ITER_BEGIN(sorted_list)) ||
    ((forward && curr == SOC_SAND_SORTED_LIST_ITER_END(sorted_list)))
    )
  {
    return list_data_ptr->null_ptr;
  }

  if (forward)
  {
      init_info_ptr->get_entry_fun(
        init_info_ptr->prime_handle,
        init_info_ptr->sec_handle,
        list_data_ptr->next,
        curr,
        list_data_ptr->ptr_size,
        list_data_ptr->tmp_data
      );
  }
  else
  {
      init_info_ptr->get_entry_fun(
        init_info_ptr->prime_handle,
        init_info_ptr->sec_handle,
        list_data_ptr->prev,
        curr,
        list_data_ptr->ptr_size,
        list_data_ptr->tmp_data
      );
  }
   /*
    * check if the list node is null.
    */
    ptr_long = 0;

    soc_sand_U8_to_U32(
        list_data_ptr->tmp_data,
        list_data_ptr->ptr_size,
        &ptr_long
      );

    if (ptr_long == list_data_ptr->null_ptr)
    {
      return list_data_ptr->null_ptr;
    }
    if (ptr_long == SOC_SAND_SORTED_LIST_ITER_BEGIN(sorted_list)) {
      return ptr_long;
    }
    if (key != NULL)
    {
     /*
      * read keys
      */
      init_info_ptr->get_entry_fun(
        init_info_ptr->prime_handle,
        init_info_ptr->sec_handle,
        list_data_ptr->keys,
        ptr_long,
        init_info_ptr->key_size,
        key
      );
    }
    if (data != NULL)
    {
     /*
      * read keys
      */
      init_info_ptr->get_entry_fun(
        init_info_ptr->prime_handle,
        init_info_ptr->sec_handle,
        list_data_ptr->data,
        ptr_long,
        init_info_ptr->data_size,
        data
      );
    }
    return ptr_long;
}

void
  soc_sand_SAND_SORTED_LIST_INFO_clear(
    SOC_SAND_OUT SOC_SAND_SORTED_LIST_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_memset(info, 0x0, sizeof(SOC_SAND_SORTED_LIST_INFO));
  info->init_info.data_size = 0;
  info->init_info.get_entry_fun = 0;
  info->init_info.key_size = 0;
  info->init_info.set_entry_fun = 0;
  info->init_info.list_size = 0;
  info->init_info.prime_handle = 0;
  info->init_info.sec_handle = 0;
  info->init_info.wb_var_index  = SOC_DPP_WB_ENGINE_VAR_NONE;
  info->init_info.unit     = 0; /* not relevan when wb_var_index=SOC_DPP_WB_ENGINE_VAR_NONE */
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_SAND_DEBUG

void
  soc_sand_SAND_SORTED_LIST_INFO_print(
    SOC_SAND_IN SOC_SAND_SORTED_LIST_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "info->init_info.data_size: %u\n"),info->init_info.data_size));
  LOG_CLI((BSL_META_U(unit,
                      "info->init_info.key_size: %u\n"),info->init_info.key_size));
  LOG_CLI((BSL_META_U(unit,
                      "info->init_info.list_size: %u\n"),info->init_info.list_size));
  LOG_CLI((BSL_META_U(unit,
                      "info->init_info.prime_handle: %u\n"),info->init_info.prime_handle));
  LOG_CLI((BSL_META_U(unit,
                      "info->init_info.sec_handle: %u\n"),info->init_info.sec_handle));


exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif /* SOC_SAND_DEBUG */


STATIC uint32
  soc_sand_sorted_list_node_link_set(
    SOC_SAND_INOUT  SOC_SAND_SORTED_LIST_INFO  *sorted_list,
    SOC_SAND_IN  uint32                   node1,
    SOC_SAND_IN  uint32                   node2
  )
{
  SOC_SAND_SORTED_LIST_INIT_INFO
    *init_info_ptr;
  SOC_SAND_SORTED_LIST_T
    *list_data_ptr;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);

  SOC_SAND_CHECK_NULL_INPUT(sorted_list);

  init_info_ptr = &(sorted_list->init_info);
  list_data_ptr = &(sorted_list->list_data);

    res = soc_sand_U32_to_U8(
            &node2,
            list_data_ptr->ptr_size,
            list_data_ptr->tmp_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    if(SOC_DPP_WB_ENGINE_VAR_NONE == init_info_ptr->wb_var_index)
    {
        res = init_info_ptr->set_entry_fun(init_info_ptr->prime_handle,
                                           init_info_ptr->sec_handle,
                                           list_data_ptr->next,
                                           node1,
                                           list_data_ptr->ptr_size,
                                           list_data_ptr->tmp_data);
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    }
    else
    {
        res = SOC_DPP_WB_ENGINE_SET_ARR(init_info_ptr->unit, 
                                        init_info_ptr->wb_var_index + WB_ENGINE_SORTED_LIST_NEXT, 
                                        list_data_ptr->tmp_data, 
                                        node1);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);      
    }




    res = soc_sand_U32_to_U8(
            &node1,
            list_data_ptr->ptr_size,
            list_data_ptr->tmp_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    if(SOC_DPP_WB_ENGINE_VAR_NONE == init_info_ptr->wb_var_index)
    {
        res = init_info_ptr->set_entry_fun(init_info_ptr->prime_handle,
                                           init_info_ptr->sec_handle,
                                           list_data_ptr->prev,
                                           node2,
                                           list_data_ptr->ptr_size,
                                           list_data_ptr->tmp_data);
        SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
    }
    else
    {
        res = SOC_DPP_WB_ENGINE_SET_ARR(init_info_ptr->unit, 
                                        init_info_ptr->wb_var_index + WB_ENGINE_SORTED_LIST_PREV, 
                                        list_data_ptr->tmp_data, 
                                        node2);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 60, exit);      
    }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_sorted_list_find_entry()",0,0);
}
/*********************************************************************
* NAME:
*     soc_sand_sorted_list_find_match_entry
* TYPE:
*   PROC
* DATE:
*   Mar  6 2008
* FUNCTION:
*  lookup in the sorted list for the given key and return the data inserted with
*  the given key.
* INPUT:
*  SOC_SAND_IN  SOC_SAND_SORTED_LIST_PTR        sorted_list -
*     The sorted list.
*   SOC_SAND_IN  SOC_SAND_SORTED_LIST_KEY*       const key -
*     The key sort to lookup for
*   SOC_SAND_IN  SOC_SAND_SORTED_LIST_DATA*       const data
*     the data to look up for.
*   SOC_SAND_IN  uint8                  first_match
*     whether to return the first match key. regardless of the data.
*   SOC_SAND_IN  uint8                    *found -
*     whether the data with the given key was found in the sorted list
*   SOC_SAND_OUT  uint32                  *entry -
*     if the key is present in the sorted list then return the entry the key found at,
*     otherwise it returns the place where the key suppose to be.
*   SOC_SAND_OUT  SOC_SAND_SORTED_LIST_ITER    *prev_node
*     iterator points to one node before the searched node.
*   SOC_SAND_OUT  SOC_SAND_SORTED_LIST_ITER    *next_node
*     iterator points to searched node or node after.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
STATIC uint32
  soc_sand_sorted_list_find_match_entry(
    SOC_SAND_INOUT  SOC_SAND_SORTED_LIST_INFO  *sorted_list,
    SOC_SAND_IN  SOC_SAND_SORTED_LIST_KEY*     const key,
    SOC_SAND_IN  SOC_SAND_SORTED_LIST_DATA*    const data,
    SOC_SAND_IN  uint8                 first_match,
    SOC_SAND_OUT  uint8                *found,
    SOC_SAND_OUT  SOC_SAND_SORTED_LIST_ITER    *prev_node,
    SOC_SAND_OUT  SOC_SAND_SORTED_LIST_ITER    *cur_node
  )
{
  SOC_SAND_SORTED_LIST_INIT_INFO
    *init_info_ptr;
  SOC_SAND_SORTED_LIST_T
    *list_data_ptr;
  uint32
    prev,
    curr;
  int32
    compare_res;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(SOC_SAND_SORTED_LIST_FIND_MATCH_ENTRY);

  SOC_SAND_CHECK_NULL_INPUT(sorted_list);
  SOC_SAND_CHECK_NULL_INPUT(key);
  SOC_SAND_CHECK_NULL_INPUT(found);

  init_info_ptr = &(sorted_list->init_info);
  list_data_ptr = &(sorted_list->list_data);

  *found = FALSE;
  prev = SOC_SAND_SORTED_LIST_TAIL_PLACE(init_info_ptr);
  do
  {
   /*
    * read next entry. (if inverted_order - read previous)
    */

     curr = soc_sand_sorted_list_get_next_aux(
            sorted_list,
            prev,
            FALSE, /* Scan the list from the end - faster in TCAM for sorted entries */
            list_data_ptr->tmp_key,
            NULL
          );

    if (curr == SOC_SAND_SORTED_LIST_ITER_BEGIN(sorted_list))
    {
      *found = FALSE;
      *prev_node = SOC_SAND_SORTED_LIST_ITER_BEGIN(sorted_list);
      *cur_node = prev;
      goto exit;
    }

    compare_res = init_info_ptr->cmp_key_fun(
                  list_data_ptr->tmp_key,
                  key,
                  (init_info_ptr->key_size * sizeof(SOC_SAND_SORTED_LIST_KEY_TYPE))
                );
     /*
      * if key was found
      */
      if (compare_res == 0)
      {
        if (first_match)
        {
            *found = TRUE;
            prev = curr;
            curr = soc_sand_sorted_list_get_next_aux(
                   sorted_list,
                   prev,
                   FALSE,
                   list_data_ptr->tmp_key,
                   NULL
                 );
            *prev_node = curr;
            *cur_node =  prev;
            goto exit;
        }

        if (data != NULL) {
            compare_res =
              soc_sand_sorted_list_data_cmp(
                sorted_list,
                curr,
                data
              );
            if (compare_res)
            {
              *found = TRUE;
                prev = curr;
                curr = soc_sand_sorted_list_get_next_aux(
                       sorted_list,
                       prev,
                       FALSE,
                       list_data_ptr->tmp_key,
                       NULL
                     );
              *prev_node = curr;
              *cur_node =  prev;
              goto exit;
            }
        }
      }
      else if (compare_res < 0)
      {
        *found = FALSE;
        *prev_node = (curr == SOC_SAND_SORTED_LIST_ITER_END(sorted_list))? list_data_ptr->null_ptr: curr;
        *cur_node = prev;
        goto exit;
      }
    prev = curr;
  }
  while(!(*found));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_sorted_list_find_match_entry()",0,0);
}

/*********************************************************************
* NAME:
*     soc_sand_sorted_list_node_alloc
* TYPE:
*   PROC
* DATE:
*   Mar  6 2008
* FUNCTION:
*  Insert an entry into the sorted list, if already exist then
*  the operation returns an error.
* INPUT:
*   SOC_SAND_INOUT  SOC_SAND_SORTED_LIST_PTR sorted_list -
*     The sorted list to add a key to.
*   SOC_SAND_IN  SOC_SAND_SORTED_LIST_KEY  key -
*     The key to add into the sorted list
*   SOC_SAND_IN  uint8  occupied -
*     the data to add into the sorted list and to be associated with
*     the given key
*   SOC_SAND_OUT  uint8                 *success -
*     whether the add operation success, this may be false.
*     if after trying to relist the key SOC_SAND_SORTED_LIST_MAX_NOF_RELIST times
*     and in all tries fill in in_use entry. to solve this problem try
*     to enlarge the sorted list size or use better list function.
* REMARKS:
*     = if there is already a key with the same key in the sorted list error is returned.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
STATIC uint32
  soc_sand_sorted_list_node_alloc(
    SOC_SAND_INOUT  SOC_SAND_SORTED_LIST_INFO    *sorted_list,
    SOC_SAND_IN  SOC_SAND_SORTED_LIST_KEY*       const key,
    SOC_SAND_IN   SOC_SAND_SORTED_LIST_DATA*     const data,
    SOC_SAND_IN   uint32                   prev_node,
    SOC_SAND_IN   uint32                   next_node,
    SOC_SAND_OUT   uint8                 *found
  )
{
  SOC_SAND_SORTED_LIST_INIT_INFO
    *init_info_ptr;
  SOC_SAND_SORTED_LIST_T
    *list_data_ptr;
  uint32
    new_node_ptr;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);

  SOC_SAND_CHECK_NULL_INPUT(sorted_list);
  SOC_SAND_CHECK_NULL_INPUT(key);
  SOC_SAND_CHECK_NULL_INPUT(data);


  init_info_ptr = &(sorted_list->init_info);
  list_data_ptr = &(sorted_list->list_data);

  /* check to see if the entry exists */

  res = soc_sand_occ_bm_alloc_next(
          unit,
          list_data_ptr->memory_use,
          &new_node_ptr,
          found
        );
  if (*found == FALSE)
  {
    goto exit;
  }

  if(SOC_DPP_WB_ENGINE_VAR_NONE == init_info_ptr->wb_var_index)
  {
      init_info_ptr->set_entry_fun(init_info_ptr->prime_handle,
                                   init_info_ptr->sec_handle,
                                   list_data_ptr->keys,
                                   new_node_ptr,
                                   init_info_ptr->key_size,
                                   key);
  }
  else
  {
      res = SOC_DPP_WB_ENGINE_SET_ARR(init_info_ptr->unit, 
                                      init_info_ptr->wb_var_index + WB_ENGINE_SORTED_LIST_KEYS, 
                                      key, 
                                      new_node_ptr);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);      
  }



  if(SOC_DPP_WB_ENGINE_VAR_NONE == init_info_ptr->wb_var_index)
  {
      init_info_ptr->set_entry_fun(init_info_ptr->prime_handle,
                                   init_info_ptr->sec_handle,
                                   list_data_ptr->data,
                                   new_node_ptr,
                                   init_info_ptr->data_size,
                                   data);
  }
  else
  {
      res = SOC_DPP_WB_ENGINE_SET_ARR(init_info_ptr->unit, 
                                      init_info_ptr->wb_var_index + WB_ENGINE_SORTED_LIST_DATA, 
                                      data, 
                                      new_node_ptr);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);      
  }

  soc_sand_sorted_list_node_link_set(
    sorted_list,
    prev_node,
    new_node_ptr
  );

  soc_sand_sorted_list_node_link_set(
    sorted_list,
    new_node_ptr,
    next_node
  );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_sorted_list_node_alloc()",0,0);

}


#include <soc/dpp/SAND/Utils/sand_footer.h>
