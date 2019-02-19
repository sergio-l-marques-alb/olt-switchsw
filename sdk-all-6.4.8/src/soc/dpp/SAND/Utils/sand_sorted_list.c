/* $Id: sand_sorted_list.c,v 1.8 Broadcom SDK $
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
#include <shared/swstate/access/sw_state_access.h>
#include <soc/dpp/drv.h>

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Utils/sand_sorted_list.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/dpp_wb_engine.h>
#include <soc/dpp/ARAD/arad_tcam.h>


/* } */

/*************
* DEFINES   *
*************/
/* { */

/* $Id: sand_sorted_list.c,v 1.8 Broadcom SDK $
 */
/*
 * Get place of the head of the list into variable 'head_place'.
 * Get place of the tail of the list into variable 'tail_place'.
 * Caller is assumed to have declared the following:
 *   variable 'unit'
 *   variable 'res'
 *   address 'exit'
 * If operation fails then software goes to 'exit' with error index 'err1'.
 */
#define SOC_SAND_SORTED_LIST_HEAD_PLACE_GET(sorted_list_index, head_place, _err1) \
  res = SORTED_LIST_ACCESS_INFO.list_size.get(unit,sorted_list_index,&head_place) ; \
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, _err1, exit) ;
#define SOC_SAND_SORTED_LIST_TAIL_PLACE_GET(sorted_list_index, tail_place, _err1) \
  res = SORTED_LIST_ACCESS_INFO.list_size.get(unit,sorted_list_index,&tail_place) ; \
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, _err1, exit) ; \
  tail_place += 1 ;


/* } */

/*************
*  MACROS   *
*************/
/* { */
#define SORTED_LIST_ACCESS          sw_state_access[unit].dpp.soc.sand.sorted_list  
#define SORTED_LIST_ACCESS_DATA     SORTED_LIST_ACCESS.lists_array.list_data
#define SORTED_LIST_ACCESS_INFO     SORTED_LIST_ACCESS.lists_array.init_info

/*
 * Verify specific sorted list index is marked as 'occupied'. If not, software goes to
 * exit with error code.
 * 
 * Notes:
 *   'unit' is assumed to be defined in the caller's scope.
 *   'res' is assumed to be defined in the caller's scope.
 *   'exit' is assumed to be defined in the caller's scope.
 */
#define SOC_SAND_SORTED_LIST_VERIFY_SORTEDLIST_IS_ACTIVE(_sorted_list_index,_err1,_err2) \
  { \
    uint8 bit_val ; \
    uint32 max_nof_lists ; \
    res = SORTED_LIST_ACCESS.max_nof_lists.get(unit, &max_nof_lists) ; \
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, _err1, exit) ; \
    if (_sorted_list_index >= max_nof_lists) \
    { \
      /* \
       * If sortedlist handle is out of range then quit with error. \
       */ \
      bit_val = 0 ; \
    } \
    else \
    { \
      res = SORTED_LIST_ACCESS.occupied_lists.bit_get(unit, (int)_sorted_list_index, &bit_val) ; \
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, _err1, exit) ; \
    } \
    if (bit_val == 0) \
    { \
      /* \
       * If sortedlist structure is not indicated as 'occupied' then quit \
       * with error. \
       */ \
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_FREE_FAIL, _err2, exit) ; \
    } \
  }
/*
 * Verify specified unit has a legal value. If not, software goes to
 * exit with error code.
 * 
 * Notes:
 *   'exit' is assumed to be defined in the caller's scope.
 */
#define SOC_SAND_SORTED_LIST_VERIFY_UNIT_IS_LEGAL(unit, _err1) \
  if (((int)unit < 0) || ((int)unit >= SOC_MAX_NUM_DEVICES)) \
  { \
    /* \
     * If this is an illegal unit identifier, quit \
     * with error. \
     */ \
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_MAX_NUM_DEVICES_OUT_OF_RANGE_ERR, _err1, exit); \
  }
/*
 * Convert input sorted list handle to index in 'occupied_lists' array.
 * Convert input index in 'occupied_lists' array to sorted list handle.
 * Indices go from 0 -> (occupied_lists - 1)
 * Handles go from 1 -> occupied_lists
 */
#define SOC_SAND_SORTED_LIST_CONVERT_HANDLE_TO_SORTEDLIST_INDEX(_sorted_list_index,_handle) (_sorted_list_index = _handle - 1)
#define SOC_SAND_SORTED_LIST_CONVERT_SORTEDLIST_INDEX_TO_HANDLE(_handle,_sorted_list_index) (_handle = _sorted_list_index + 1)
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
/* { */

/*********************************************************************
* NAME:
*   soc_sand_sorted_list_get_tmp_data_ptr_from_handle
* TYPE:
*   PROC
* DATE:
*   May 18 2015
* FUNCTION:
*   Get value of 'tmp_data' pointer (See SOC_SAND_SORTED_LIST_T)
*   from handle.
* INPUT:
*   SOC_SAND_IN  int                             unit -
*     Identifier of the device to access.
*   SOC_SAND_IN  SOC_SAND_SORTED_LIST_PTR        sorted_list -
*     Handle to the sorted list.
*   SOC_SAND_OUT uint8                           **tmp_data_ptr_ptr -
*     This procedure loads pointed memory by the pointer to the 'tmp_data'
*     internal workspace buffer.
* REMARKS:
*   This procedure is exceptional. It is added here so we can use
*   the buffer pointed by 'tmp_data' as a work space whose address
*   is passed to variuos utilities.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
STATIC uint32
  soc_sand_sorted_list_get_tmp_data_ptr_from_handle(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_SAND_SORTED_LIST_PTR     sorted_list,
    SOC_SAND_OUT uint8                        **tmp_data_ptr_ptr
  )
{
  uint32
    sorted_list_index,
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_SORTED_LIST_VERIFY_UNIT_IS_LEGAL(unit, 2) ;
  SOC_SAND_SORTED_LIST_CONVERT_HANDLE_TO_SORTEDLIST_INDEX(sorted_list_index,sorted_list) ;
  SOC_SAND_SORTED_LIST_VERIFY_SORTEDLIST_IS_ACTIVE(sorted_list_index,4,6) ;

  *tmp_data_ptr_ptr = sw_state[unit]->dpp.soc.sand.sorted_list->lists_array[sorted_list_index]->list_data.tmp_data ;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_sorted_list_get_tmp_data_ptr_from_handle()",0,0);
}

/*********************************************************************
* NAME:
*   soc_sand_sorted_list_get_tmp_key_ptr_from_handle
* TYPE:
*   PROC
* DATE:
*   May 18 2015
* FUNCTION:
*   Get value of 'tmp_key' pointer (See SOC_SAND_SORTED_LIST_T)
*   from handle.
* INPUT:
*   SOC_SAND_IN  int                             unit -
*     Identifier of the device to access.
*   SOC_SAND_IN  SOC_SAND_SORTED_LIST_PTR        sorted_list -
*     Handle to the sorted list.
*   SOC_SAND_OUT uint8                           **tmp_key_ptr_ptr -
*     This procedure loads pointed memory by the pointer to the 'tmp_key'
*     internal workspace buffer.
* REMARKS:
*   This procedure is exceptional. It is added here so we can use
*   the buffer pointed by 'tmp_key' as a work space whose address
*   is passed to variuos utilities.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
STATIC uint32
  soc_sand_sorted_list_get_tmp_key_ptr_from_handle(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_SAND_SORTED_LIST_PTR     sorted_list,
    SOC_SAND_OUT uint8                        **tmp_key_ptr_ptr
  )
{
  uint32
    sorted_list_index,
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_SORTED_LIST_VERIFY_UNIT_IS_LEGAL(unit, 2) ;
  SOC_SAND_SORTED_LIST_CONVERT_HANDLE_TO_SORTEDLIST_INDEX(sorted_list_index,sorted_list) ;
  SOC_SAND_SORTED_LIST_VERIFY_SORTEDLIST_IS_ACTIVE(sorted_list_index,4,6) ;

  *tmp_key_ptr_ptr = sw_state[unit]->dpp.soc.sand.sorted_list->lists_array[sorted_list_index]->list_data.tmp_key ;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_sorted_list_get_tmp_key_ptr_from_handle()",0,0);
}

/*********************************************************************
* NAME:
*     soc_sand_sorted_list_find_match_entry
* TYPE:
*   PROC
* DATE:
*   May 19 2015
* FUNCTION:
*  lookup in the sorted list for the given key and return the data inserted with
*  the given key.
* INPUT:
*   SOC_SAND_IN  int                             unit -
*     Identifier of the device to access.
*   SOC_SAND_IN  SOC_SAND_SORTED_LIST_PTR        sorted_list -
*     Handle to the sorted list.
*   SOC_SAND_IN  SOC_SAND_SORTED_LIST_KEY*       const key -
*     The key to lookup for
*   SOC_SAND_IN  uint8                    first_empty
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
    SOC_SAND_IN int                             unit,
    SOC_SAND_IN SOC_SAND_SORTED_LIST_PTR        sorted_list,
    SOC_SAND_IN SOC_SAND_SORTED_LIST_KEY*       const key,
    SOC_SAND_IN SOC_SAND_SORTED_LIST_DATA*      const data,
    SOC_SAND_IN uint8                           first_match,
    SOC_SAND_OUT  uint8                         *found,
    SOC_SAND_OUT  SOC_SAND_SORTED_LIST_ITER     *prev_node,
    SOC_SAND_OUT  SOC_SAND_SORTED_LIST_ITER     *cur_node
  );

STATIC uint32
  soc_sand_sorted_list_node_alloc(
    SOC_SAND_IN   uint32                       unit,
    SOC_SAND_IN  SOC_SAND_SORTED_LIST_PTR      sorted_list,
    SOC_SAND_IN  SOC_SAND_SORTED_LIST_KEY      *const key,
    SOC_SAND_IN   SOC_SAND_SORTED_LIST_DATA    *const data,
    SOC_SAND_IN   uint32                       prev_node,
    SOC_SAND_IN   uint32                       next_node,
    SOC_SAND_OUT  uint8                        *found
  );

/*********************************************************************
* NAME:
*     soc_sand_sorted_list_get_next_aux
* TYPE:
*   PROC
* DATE:
*   May 19 2015
* FUNCTION:
*  get the next valid entry (key and data) in the sorted list.
*  start traversing from the place pointed by the given iterator.
* INPUT:
*   SOC_SAND_IN  int                             unit -
*     Identifier of the device to access.
*   SOC_SAND_IN  SOC_SAND_SORTED_LIST_PTR        sorted_list -
*     Handle to the sorted list.
*   SOC_SAND_INOUT  SOC_SAND_SORTED_LIST_ITER    *iter
*     iterator points to the entry to start traverse from.
*   SOC_SAND_OUT  SOC_SAND_SORTED_LIST_KEY       *const key -
*     the sorted list key returned
*   SOC_SAND_OUT  SOC_SAND_SORTED_LIST_DATA      data -
*     the sorted list data returned and associated with the key above.
*   SOC_SAND_INOUT SOC_SAND_SORTED_LIST_ITER     *next_or_prev -
*     This procedure loads pointed memory by the next iterator if
*     'forward' is true or by the previous iterator if 'forward' is
*     false.
* REMARKS:
*     - to start traverse the sorted list from the beginning.
*       use SOC_SAND_SORTED_LIST_ITER_BEGIN(iter)
*     - to check if the iterator get to the end of the table use.
*       use SOC_SAND_SORTED_LIST_ITER_END(unit,iter)
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
STATIC uint32
  soc_sand_sorted_list_get_next_aux(
    SOC_SAND_IN    int                           unit,
    SOC_SAND_IN    SOC_SAND_SORTED_LIST_PTR      sorted_list,
    SOC_SAND_IN    SOC_SAND_SORTED_LIST_ITER     curr,
    SOC_SAND_IN    uint8                         forward,
    SOC_SAND_OUT   SOC_SAND_SORTED_LIST_KEY      *const key,
    SOC_SAND_OUT   SOC_SAND_SORTED_LIST_DATA     *const data,
    SOC_SAND_INOUT SOC_SAND_SORTED_LIST_ITER     *next_or_prev
  );

STATIC uint32
  soc_sand_sorted_list_node_link_set(
    SOC_SAND_IN     int                   unit,
    SOC_SAND_IN  SOC_SAND_SORTED_LIST_PTR sorted_list,
    SOC_SAND_IN  uint32                   node1,
    SOC_SAND_IN  uint32                   node2
  );

int32
    soc_sand_sorted_list_tcam_cmp_priority(
      SOC_SAND_IN uint8  *buffer1,
      SOC_SAND_IN uint8  *buffer2,
              uint32 size
    )
{
  uint32
    prio_1,
    prio_2;
  ARAD_TCAM_PRIO_LOCATION 
      prio_location_1,
      prio_location_2;

  if ((buffer1 == NULL) || (buffer2 == NULL)) {
      /* Not supposed to compare NULL pointers */
      assert(0);
  }
  /* Key comparison */
  if (size == ARAD_TCAM_DB_LIST_KEY_SIZE) {
      prio_1 = arad_tcam_db_prio_list_priority_value_decode(buffer1);
      prio_2 = arad_tcam_db_prio_list_priority_value_decode(buffer2);
      return (((int32) prio_1) - ((int32) prio_2));
  }
  else if (size == ARAD_TCAM_DB_LIST_DATA_SIZE) { /* Data comparison */
      ARAD_TCAM_PRIO_LOCATION_clear(&prio_location_1);
      ARAD_TCAM_PRIO_LOCATION_clear(&prio_location_2);
      /* buffer1 and buffer2 can't be null. It have been checked above */
      /* coverity[var_deref_model:FALSE] */
      sal_memcpy(&prio_location_1, buffer1, sizeof(ARAD_TCAM_PRIO_LOCATION));
      /* coverity[var_deref_model:FALSE] */
      sal_memcpy(&prio_location_2, buffer2, sizeof(ARAD_TCAM_PRIO_LOCATION));
      return ((prio_location_1.entry_id_first != prio_location_2.entry_id_first)
              || (prio_location_1.entry_id_last != prio_location_2.entry_id_last))? 1 :0;
  } else {
      /* Unknown input formats */
      assert(0);
      return 0;
  }
}

uint32
    soc_sand_sorted_list_default_entry_set(
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
    soc_sand_sorted_list_default_entry_get(
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


/************************************************************************/
/*  End of internals                                                    */
/************************************************************************/
/* } */

/*********************************************************************
* NAME:
*   soc_sand_sorted_list_init
* TYPE:
*   PROC
* DATE:
*   May 19 2015
* FUNCTION:
*   Initialize control structure for ALL sorted list instances expected.
* INPUT:
*   SOC_SAND_IN  int unit -
*     Identifier of the device to access.
*   SOC_SAND_IN  uint32 max_nof_lists -
*     Maximal number of sorted lists which can be sustained simultaneously.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_sand_sorted_list_init(
    SOC_SAND_IN       int                          unit,
    SOC_SAND_IN       uint32                       max_nof_lists
  )
{
  uint32 res ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_SORTED_LIST_INIT) ;

  res = SORTED_LIST_ACCESS.alloc(unit);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  res = SORTED_LIST_ACCESS.lists_array.ptr_alloc(unit, max_nof_lists);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

  res = SORTED_LIST_ACCESS.max_nof_lists.set(unit, max_nof_lists);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  res = SORTED_LIST_ACCESS.in_use.set(unit, 0);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

  res = SORTED_LIST_ACCESS.occupied_lists.alloc_bitmap(unit, max_nof_lists);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 60, exit);

  exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_sorted_list_init()",0,0);
}

/*********************************************************************
* NAME:
*   soc_sand_sorted_list_clear_all_tmps
* TYPE:
*   PROC
* DATE:
*   Aug 02 2015
* FUNCTION:
*   Fill all allocated 'tmp' (sand box) buffers by zeros.
* INPUT:
*   SOC_SAND_IN  int unit -
*     Identifier of the device to access.
* REMARKS:
*   This procedure is to be used at init before 'diff'ing previous sw
*   state buffer with current one. This ensures that such buffers are
*   effectively not 'diff'ed.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_sand_sorted_list_clear_all_tmps(
    SOC_SAND_IN int unit
  )
{
  uint32
    sorted_list_index ;
  uint32
    found,
    res,
    max_nof_lists,
    tmp_size,
    key_size,
    data_size,
    ptr_size,
    in_use ;
  uint8
    bit_val ;
  uint8
    is_allocated ;
  int32
    offset ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_SORTED_LIST_VERIFY_UNIT_IS_LEGAL(unit, 2) ;

  res = SORTED_LIST_ACCESS.in_use.get(unit, &in_use);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 4, exit);

  res = SORTED_LIST_ACCESS.max_nof_lists.get(unit, &max_nof_lists);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 6, exit);

  if (in_use >= max_nof_lists)
  {
    /*
     * If number of occupied bitmap structures is beyond the
     * maximum then quit with error.
     */
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_MALLOC_FAIL, 8, exit);
  }
  /*
   * Find occupied lists (a set bit in 'occupied_lists') and, for each,
   * fill 'tmp' buffers by zeroes.
   *
   * Currently, 'tmp' buffers are:
   *   sw_state[unit]->dpp.soc.sand.sorted_list->lists_array[sorted_list_index]->list_data.tmp_key
   *   sw_state[unit]->dpp.soc.sand.sorted_list->lists_array[sorted_list_index]->list_data.tmp_data
   */
  found = 0 ;
  offset = 0 ;
  for (sorted_list_index = 0 ; sorted_list_index < max_nof_lists ; sorted_list_index++)
  {
    res = SORTED_LIST_ACCESS.occupied_lists.bit_get(unit, sorted_list_index, &bit_val);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 12, exit);
    if (bit_val == 1)
    {
      /*
       * 'sorted_list_index' is now the index of an occupied entry.
       */
      found++ ;
      res = SORTED_LIST_ACCESS_DATA.tmp_key.is_allocated(unit,sorted_list_index,&is_allocated) ;
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 14, exit) ;
      if (!is_allocated)
      {
        /*
         * 'tmp_key' buffer must be allocated, at this point.
         */
        SOC_SAND_SET_ERROR_CODE(SOC_E_INTERNAL, 16, exit);
      }
      res = SORTED_LIST_ACCESS_DATA.tmp_data.is_allocated(unit,sorted_list_index,&is_allocated) ;
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 24, exit) ;
      if (!is_allocated)
      {
        /*
         * 'tmp_data' buffer must be allocated, at this point.
         */
        SOC_SAND_SET_ERROR_CODE(SOC_E_INTERNAL, 18, exit);
      }
      /*
       * Clear 'tmp_key'
       */
      res = SORTED_LIST_ACCESS_INFO.key_size.get(unit,sorted_list_index,&key_size) ;
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
      res = SORTED_LIST_ACCESS_DATA.tmp_key.memset(unit,sorted_list_index,offset,key_size,0) ;
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 22, exit) ;
      /*
       * Clear 'tmp_data'
       */
      res = SORTED_LIST_ACCESS_INFO.data_size.get(unit,sorted_list_index,&data_size) ;
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 24, exit);
      res = SORTED_LIST_ACCESS_DATA.ptr_size.get(unit,sorted_list_index,&ptr_size) ;
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 26, exit) ;
      tmp_size = SOC_SAND_MAX(ptr_size,data_size) ;
      res = SORTED_LIST_ACCESS_DATA.tmp_data.memset(unit,sorted_list_index,offset,tmp_size,0) ;
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 28, exit) ;
      if (found >= in_use)
      {
        /*
         * If all allocated entries have been treated. Quit.
         */
        break ;
      }
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_sorted_list_clear_all_tmps()",0,0);
}

uint32
  soc_sand_sorted_list_create(
    SOC_SAND_IN     int                             unit,
    SOC_SAND_INOUT  SOC_SAND_SORTED_LIST_PTR        *sorted_list_ptr,
    SOC_SAND_IN     SOC_SAND_SORTED_LIST_INIT_INFO  init_info
  )
{
  SOC_SAND_OCC_BM_INIT_INFO
    btmp_init_info ;
  uint32
    tmp_size ;
  uint32
    sorted_list_index ;
  uint32
    found,
    res,
    data_size,
    list_size,
    key_size,
    null_ptr,
    ptr_size,
    max_nof_lists ;
  uint32
    in_use ;
  uint8
    bit_val ;
  SOC_SAND_SORTED_LIST_PTR
    sorted_list ;
  SOC_SAND_OCC_BM_PTR
    memory_use ;
  int32
    offset ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_SORTED_LIST_CREATE) ;

  SOC_SAND_SORTED_LIST_VERIFY_UNIT_IS_LEGAL(unit, 2) ;
  SOC_SAND_CHECK_NULL_INPUT(sorted_list_ptr);

  res = SORTED_LIST_ACCESS.in_use.get(unit, &in_use);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 4, exit);

  res = SORTED_LIST_ACCESS.max_nof_lists.get(unit, &max_nof_lists);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 6, exit);

  if (in_use >= max_nof_lists)
  {
    /*
     * If number of occupied bitmap structures is beyond the
     * maximum then quit with error.
     */
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_MALLOC_FAIL, 8, exit);
  }
  /*
   * Increment number of 'in_use' to cover the one we now intend to capture.
   */
  res = SORTED_LIST_ACCESS.in_use.set(unit, (in_use + 1));
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
  /*
   * Find a free list (a cleared bit in 'occupied_lists'). At this point,
   * there must be one.
   */
  found = 0 ;
  for (sorted_list_index = 0 ; sorted_list_index < max_nof_lists ; sorted_list_index++)
  {
    res = SORTED_LIST_ACCESS.occupied_lists.bit_get(unit, sorted_list_index, &bit_val);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 12, exit);
    if (bit_val == 0)
    {
      /*
       * 'sorted_list_index' is now the index of a free entry.
       */
      found = 1 ;
      break ;
    }
  }
  if (!found)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_MALLOC_FAIL, 14, exit);
  }
  res = SORTED_LIST_ACCESS.occupied_lists.bit_set(unit, sorted_list_index);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 16, exit);

  res = SORTED_LIST_ACCESS.lists_array.alloc(unit, sorted_list_index);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 18, exit);
  /*
   * Note that legal handles start at '1', not at '0'.
   */
  SOC_SAND_SORTED_LIST_CONVERT_SORTEDLIST_INDEX_TO_HANDLE(sorted_list,sorted_list_index) ;
  /*
   * Set output of this procedure.
   */
  *sorted_list_ptr = sorted_list;

  soc_sand_SAND_SORTED_LIST_INFO_clear(unit,sorted_list) ;
  res = SORTED_LIST_ACCESS_INFO.prime_handle.set(unit, sorted_list_index, init_info.prime_handle) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 22, exit);

  res = SORTED_LIST_ACCESS_INFO.sec_handle.set(unit,sorted_list_index,init_info.sec_handle) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 24, exit);
  res = SORTED_LIST_ACCESS_INFO.list_size.set(unit,sorted_list_index,init_info.list_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 26, exit);
  res = SORTED_LIST_ACCESS_INFO.key_size.set(unit,sorted_list_index,init_info.key_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 28, exit);
  res = SORTED_LIST_ACCESS_INFO.data_size.set(unit,sorted_list_index,init_info.data_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);
  if (init_info.get_entry_fun != NULL || init_info.set_entry_fun != NULL )
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_VALUE_OUT_OF_RANGE_ERR, 31, exit);
  }
  res = SORTED_LIST_ACCESS_INFO.get_entry_fun.set(unit,sorted_list_index,init_info.get_entry_fun) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 32, exit);
  res = SORTED_LIST_ACCESS_INFO.set_entry_fun.set(unit,sorted_list_index,init_info.set_entry_fun) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 34, exit);
  res = SORTED_LIST_ACCESS_INFO.cmp_func_type.set(unit,sorted_list_index,init_info.cmp_func_type) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 35, exit);

  res = SORTED_LIST_ACCESS_INFO.key_size.get(unit,sorted_list_index,&key_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 36, exit);
  res = SORTED_LIST_ACCESS_INFO.list_size.get(unit,sorted_list_index,&list_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 38, exit);
  res = SORTED_LIST_ACCESS_INFO.data_size.get(unit,sorted_list_index,&data_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

  if (list_size == 0 || key_size == 0 || data_size == 0)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_VALUE_OUT_OF_RANGE_ERR, 42, exit);
  }
  /*
   * calculate the size of pointers (list head and next) according to table size.
   */
  ptr_size = (soc_sand_log2_round_up(list_size + 2) + (SOC_SAND_NOF_BITS_IN_BYTE - 1)) / SOC_SAND_NOF_BITS_IN_BYTE;
  null_ptr = list_size + 1;
  res = SORTED_LIST_ACCESS_DATA.ptr_size.set(unit,sorted_list_index,ptr_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 44, exit) ;
  res = SORTED_LIST_ACCESS_DATA.null_ptr.set(unit,sorted_list_index,null_ptr) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 46, exit) ;

  tmp_size = SOC_SAND_MAX(ptr_size,data_size) ;
  /*
   * Allocate the temps buffer.
   */
  res = SORTED_LIST_ACCESS_DATA.tmp_data.alloc(unit,sorted_list_index,tmp_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 48, exit) ;
  res = SORTED_LIST_ACCESS_DATA.tmp_key.alloc(unit,sorted_list_index,key_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 50, exit) ;
  /*
   * allocate buffer for keys
   */
  offset = 0 ;
  res = SORTED_LIST_ACCESS_DATA.keys.alloc(unit,sorted_list_index,list_size * key_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 52, exit) ;
  res = SORTED_LIST_ACCESS_DATA.keys.memset(unit,sorted_list_index,offset,list_size * key_size,0) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 54, exit) ;
  /*
   * allocate buffer for next array (to build the linked list) one additional for head.
   * which is the last in the next pointers array.
   */
  res = SORTED_LIST_ACCESS_DATA.next.alloc(unit,sorted_list_index,(list_size + 2) * ptr_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 56, exit) ;
  res = SORTED_LIST_ACCESS_DATA.next.memset(unit,sorted_list_index,offset,(list_size + 2) * ptr_size,0xFF) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 58, exit) ;
  /*
   * allocate buffer for prev array (to build the linked list)
   */
  res = SORTED_LIST_ACCESS_DATA.prev.alloc(unit,sorted_list_index,(list_size + 2) * ptr_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 56, exit) ;
  res = SORTED_LIST_ACCESS_DATA.prev.memset(unit,sorted_list_index,offset,(list_size + 2) * ptr_size,0xFF) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 58, exit) ;
  /*
   * allocate buffer for the data array
   */
  res = SORTED_LIST_ACCESS_DATA.data.alloc(unit,sorted_list_index,list_size * data_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 60, exit) ;
  res = SORTED_LIST_ACCESS_DATA.data.memset(unit,sorted_list_index,offset,list_size * data_size,0) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 62, exit) ;
  {
    uint32
      head_place,
      tail_place ;
    /*
     * connect the head with the tail.
     */
    SOC_SAND_SORTED_LIST_HEAD_PLACE_GET(sorted_list_index,head_place,64) ;
    SOC_SAND_SORTED_LIST_TAIL_PLACE_GET(sorted_list_index,tail_place,66) ;
    res =
      soc_sand_sorted_list_node_link_set(
        unit,
        sorted_list,
        head_place,
        tail_place
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
  }
  /*
   * initialize the data to be mapped to
   */
  soc_sand_SAND_OCC_BM_INIT_INFO_clear(&btmp_init_info);
  btmp_init_info.size = list_size ;

  /* if(SOC_DPP_WB_ENGINE_VAR_NONE != init_info_ptr->wb_var_index) */

  res = soc_sand_occ_bm_create(
          unit,
          &btmp_init_info,
          &memory_use
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
  res = SORTED_LIST_ACCESS_DATA.memory_use.set(unit,sorted_list_index,memory_use) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 82, exit) ;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_sorted_list_create()",0,0);
}

uint32
  soc_sand_sorted_list_clear(
    SOC_SAND_IN     int                           unit,
    SOC_SAND_IN     SOC_SAND_SORTED_LIST_PTR      sorted_list
  )
{
  uint32
    res;
  uint32
    sorted_list_index ;
  uint32
    data_size,
    list_size,
    key_size,
    ptr_size ;
  SOC_SAND_OCC_BM_PTR
    memory_use ;
  int32
    offset ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_HASH_TABLE_CREATE);
  SOC_SAND_SORTED_LIST_VERIFY_UNIT_IS_LEGAL(unit, 2) ;
  SOC_SAND_SORTED_LIST_CONVERT_HANDLE_TO_SORTEDLIST_INDEX(sorted_list_index,sorted_list) ;
  SOC_SAND_SORTED_LIST_VERIFY_SORTEDLIST_IS_ACTIVE(sorted_list_index,4,6) ;

  /* if(SOC_DPP_WB_ENGINE_VAR_NONE == init_info_ptr->wb_var_index)  */
  {
    offset = 0 ;
    res = SORTED_LIST_ACCESS_DATA.ptr_size.get(unit,sorted_list_index,&ptr_size) ;
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 12, exit) ;
    res = SORTED_LIST_ACCESS_INFO.key_size.get(unit,sorted_list_index,&key_size) ;
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 14, exit);
    res = SORTED_LIST_ACCESS_INFO.list_size.get(unit,sorted_list_index,&list_size) ;
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 16, exit);
    res = SORTED_LIST_ACCESS_INFO.data_size.get(unit,sorted_list_index,&data_size) ;
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 18, exit);

    res = SORTED_LIST_ACCESS_DATA.keys.memset(unit,sorted_list_index,offset,list_size * key_size,0x00) ;
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit) ;

    res = SORTED_LIST_ACCESS_DATA.next.memset(unit,sorted_list_index,offset,(list_size + 2) * ptr_size,0xFF) ;
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 22, exit) ;

    res = SORTED_LIST_ACCESS_DATA.prev.memset(unit,sorted_list_index,offset,(list_size + 2) * ptr_size,0xFF) ;
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 24, exit) ;

    res = SORTED_LIST_ACCESS_DATA.data.memset(unit,sorted_list_index,offset,list_size * data_size,0x00) ;
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 26, exit) ;
  } 
  { 
    /*
     * connect the head with the tail.
     */
    uint32
      head_place,
      tail_place ;
    SOC_SAND_SORTED_LIST_HEAD_PLACE_GET(sorted_list_index,head_place,54) ;
    SOC_SAND_SORTED_LIST_TAIL_PLACE_GET(sorted_list_index,tail_place,56) ;
    res = soc_sand_sorted_list_node_link_set(
          unit,
          sorted_list,
          head_place,
          tail_place
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit) ;
  }

  res = SORTED_LIST_ACCESS_DATA.memory_use.get(unit,sorted_list_index,&memory_use) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 32, exit) ;
  res = soc_sand_occ_bm_clear(
          unit,
          memory_use
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_sorted_list_clear()",0,0);
}

uint32
  soc_sand_sorted_list_destroy(
    SOC_SAND_IN     int                         unit,
    SOC_SAND_IN     SOC_SAND_SORTED_LIST_PTR    sorted_list
    )
{
  uint32
    res,
    sorted_list_index ;
  SOC_SAND_OCC_BM_PTR
    memory_use ;
  uint8
    bit_val ;
  uint32
    in_use ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_SORTED_LIST_DESTROY);
  SOC_SAND_SORTED_LIST_VERIFY_UNIT_IS_LEGAL(unit, 2) ;
  SOC_SAND_SORTED_LIST_CONVERT_HANDLE_TO_SORTEDLIST_INDEX(sorted_list_index,sorted_list) ;
  SOC_SAND_SORTED_LIST_VERIFY_SORTEDLIST_IS_ACTIVE(sorted_list_index,4,6) ;
  /*
   * First, mark this sorted list as 'released'
   */
  res = SORTED_LIST_ACCESS.in_use.get(unit, &in_use);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 4, exit);
  if ((int)in_use < 0)
  {
    /*
     * If number of occupied sortedlist structures goes below zero then quit
     * with error.
     */
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_FREE_FAIL, 8, exit);
  }
  /*
   * Decrement number of 'in_use' to cover the one we now intend to release.
   */
  res = SORTED_LIST_ACCESS.in_use.set(unit, (in_use - 1));
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit) ;
  /*
   * Mark specific sorted list as 'not occupied'
   */
  res = SORTED_LIST_ACCESS.occupied_lists.bit_get(unit, sorted_list_index, &bit_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 12, exit);
  if (bit_val == 0)
  {
    /*
     * If sorted list structure is not indicated as 'occupied' then quit
     * with error.
     */
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_FREE_FAIL, 14, exit) ;
  }
  res = SORTED_LIST_ACCESS.occupied_lists.bit_clear(unit, sorted_list_index);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 14, exit);
  /*
   * Free the temps buffer.
   */
  res = SORTED_LIST_ACCESS_DATA.tmp_data.free(unit,sorted_list_index) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 18, exit) ;
  res = SORTED_LIST_ACCESS_DATA.tmp_key.free(unit,sorted_list_index) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit) ;

  res = SORTED_LIST_ACCESS_DATA.next.free(unit,sorted_list_index) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 24, exit) ;
  res = SORTED_LIST_ACCESS_DATA.keys.free(unit,sorted_list_index) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 28, exit) ;

  res = SORTED_LIST_ACCESS_DATA.prev.free(unit,sorted_list_index) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 32, exit) ;
  res = SORTED_LIST_ACCESS_DATA.data.free(unit,sorted_list_index) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 36, exit) ;

  res = SORTED_LIST_ACCESS_DATA.memory_use.get(unit,sorted_list_index,&memory_use) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit) ;
  res = soc_sand_occ_bm_destroy(
    unit,
    memory_use
    );
  res = SORTED_LIST_ACCESS.lists_array.free(unit, sorted_list_index);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 44, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_sorted_list_destroy()",0,0);
}

uint32
  soc_sand_sorted_list_entry_add(
    SOC_SAND_IN     int                           unit,
    SOC_SAND_IN     SOC_SAND_SORTED_LIST_PTR      sorted_list,
    SOC_SAND_IN     SOC_SAND_SORTED_LIST_KEY      *const key,
    SOC_SAND_IN     SOC_SAND_SORTED_LIST_DATA     *const data,
    SOC_SAND_OUT    uint8                         *success
  )
{
  uint8
    found ;
  uint32
    curr_node,
    prev_node ;
  uint32
    res ;
  uint32
    sorted_list_index ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_SORTED_LIST_ENTRY_ADD);
  SOC_SAND_SORTED_LIST_VERIFY_UNIT_IS_LEGAL(unit, 2) ;
  SOC_SAND_SORTED_LIST_CONVERT_HANDLE_TO_SORTEDLIST_INDEX(sorted_list_index,sorted_list) ;
  SOC_SAND_SORTED_LIST_VERIFY_SORTEDLIST_IS_ACTIVE(sorted_list_index,4,6) ;

  SOC_SAND_CHECK_NULL_INPUT(key);
  SOC_SAND_CHECK_NULL_INPUT(data);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = soc_sand_sorted_list_find_match_entry(
          unit,
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
          unit,
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
    SOC_SAND_IN     SOC_SAND_SORTED_LIST_PTR      sorted_list,
    SOC_SAND_IN     SOC_SAND_SORTED_LIST_ITER     pos,
    SOC_SAND_IN     uint8                         before,
    SOC_SAND_IN     SOC_SAND_SORTED_LIST_KEY      *const key,
    SOC_SAND_IN     SOC_SAND_SORTED_LIST_DATA     *const data,
    SOC_SAND_OUT    uint8                         *success
  )
{
  SOC_SAND_SORTED_LIST_ITER
    prev_node,
    next_node;
  SOC_SAND_SORTED_LIST_KEY_CMP 
    cmp_key_fun;
  uint32
    key_size,
    res;
  uint32
    sorted_list_index ;
  SOC_SAND_SORTED_LIST_CMP_FUNC_TYPE
    cmp_func_type ;
  uint8
    *tmp_key_ptr ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_SORTED_LIST_ENTRY_ADD_BY_ITER);
  SOC_SAND_SORTED_LIST_VERIFY_UNIT_IS_LEGAL(unit, 2) ;
  SOC_SAND_SORTED_LIST_CONVERT_HANDLE_TO_SORTEDLIST_INDEX(sorted_list_index,sorted_list) ;
  SOC_SAND_SORTED_LIST_VERIFY_SORTEDLIST_IS_ACTIVE(sorted_list_index,4,6) ;

  res = SORTED_LIST_ACCESS_INFO.cmp_func_type.get(unit,sorted_list_index,&cmp_func_type) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 8, exit);

  if (cmp_func_type == SOC_SAND_SORTED_LIST_CMP_FUNC_TYPE_TCAM) {
      cmp_key_fun = soc_sand_sorted_list_tcam_cmp_priority ;
  }
  else {
      cmp_key_fun = (SOC_SAND_SORTED_LIST_KEY_CMP)soc_sand_os_memcmp;
  }
  /*
   * Verify we are not adding after the end or before the beginning.
   */
  if (
      (before &&  pos == SOC_SAND_SORTED_LIST_ITER_BEGIN(unit,sorted_list)) ||
      ((!before && pos == SOC_SAND_SORTED_LIST_ITER_END(unit,sorted_list)))
     )
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_SORTED_LIST_ILLEGAL_ITER_ERR, 10, exit);
  }
 /*
  * Verify that this addition keeps the order.
  */
  prev_node = pos;
  next_node = pos;

  if (before)
  {
    res =
      soc_sand_sorted_list_get_next_aux(
        unit,
        sorted_list,
        pos,
        FALSE,
        NULL,
        NULL,&prev_node
      );
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
  }
  else
  {
    res =
      soc_sand_sorted_list_get_next_aux(
        unit,
        sorted_list,
        pos,
        TRUE,
        NULL,
        NULL,&next_node
      );
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 12, exit);
  }
  res = soc_sand_sorted_list_get_tmp_key_ptr_from_handle(unit,sorted_list,&tmp_key_ptr) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 14, exit);
  res = SORTED_LIST_ACCESS_INFO.key_size.get(unit,sorted_list_index,&key_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 16, exit) ;
  if (prev_node != SOC_SAND_SORTED_LIST_ITER_END(unit,sorted_list) &&  prev_node != SOC_SAND_SORTED_LIST_ITER_BEGIN(unit,sorted_list))
  {
    res = soc_sand_sorted_list_entry_value(
            unit,
            sorted_list,
            prev_node,
            tmp_key_ptr,
            NULL
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 18, exit) ;

    if (cmp_key_fun(tmp_key_ptr, key, key_size) > 0 )
    {
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_SORTED_LIST_ILLEGAL_ITER_ERR, 18, exit);
    }
  }
  if (next_node != SOC_SAND_SORTED_LIST_ITER_END(unit,sorted_list) &&  next_node != SOC_SAND_SORTED_LIST_ITER_BEGIN(unit,sorted_list))
  {
    res = soc_sand_sorted_list_entry_value(
            unit,
            sorted_list,
            next_node,
            tmp_key_ptr,
            NULL
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    if (cmp_key_fun(key, tmp_key_ptr, key_size) > 0 )
    {
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_SORTED_LIST_ILLEGAL_ITER_ERR, 22, exit);
    }
  }
  res = soc_sand_sorted_list_node_alloc(
          unit,
          sorted_list,
          key,
          data,
          prev_node,
          next_node,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 24, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_sorted_list_entry_add_by_iter()",0,0);
}

uint32
  soc_sand_sorted_list_entry_update(
    SOC_SAND_IN     int                           unit,
    SOC_SAND_IN     SOC_SAND_SORTED_LIST_PTR      sorted_list,
    SOC_SAND_IN     SOC_SAND_SORTED_LIST_ITER     iter,
    SOC_SAND_IN     SOC_SAND_SORTED_LIST_KEY      *const key,
    SOC_SAND_IN     SOC_SAND_SORTED_LIST_DATA     *const data
  )
{
  uint32
    res ;
  uint32
    sorted_list_index ;
  uint32
    data_size,
    key_size,
    list_size ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_SORTED_LIST_ENTRY_UPDATE);
  SOC_SAND_SORTED_LIST_VERIFY_UNIT_IS_LEGAL(unit, 2) ;
  SOC_SAND_SORTED_LIST_CONVERT_HANDLE_TO_SORTEDLIST_INDEX(sorted_list_index,sorted_list) ;
  SOC_SAND_SORTED_LIST_VERIFY_SORTEDLIST_IS_ACTIVE(sorted_list_index,4,6) ;

  SOC_SAND_CHECK_NULL_INPUT(data);
  SOC_SAND_CHECK_NULL_INPUT(key);

  res = SORTED_LIST_ACCESS_INFO.list_size.get(unit,sorted_list_index,&list_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 8, exit);
  if (iter == SOC_SAND_SORTED_LIST_NULL || iter >= list_size)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_VALUE_OUT_OF_RANGE_ERR, 10, exit);
  }
  /*
   * Check to see if the entry exists
   */
  if (data)
  {
    res = SORTED_LIST_ACCESS_INFO.data_size.get(unit,sorted_list_index,&data_size) ;
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 18, exit);
    /* if(SOC_DPP_WB_ENGINE_VAR_NONE == init_info_ptr->wb_var_index) */
    {
      /*
       * Copy 'data_size' bytes from input 'data' buffer into 'SORTED_LIST_ACCESS_DATA.data'
       * buffer at offset 'data_size * iter'.
       */
      res = SORTED_LIST_ACCESS_DATA.data.memwrite(unit,sorted_list_index,data,iter * data_size, data_size) ;
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 26, exit) ;
    }
  }
  if (key)
  {
    res = SORTED_LIST_ACCESS_INFO.key_size.get(unit,sorted_list_index,&key_size) ;
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 28, exit);
    /* if(SOC_DPP_WB_ENGINE_VAR_NONE == init_info_ptr->wb_var_index) */
    {
      /*
       * Copy 'key_size' bytes from input 'key' buffer into 'SORTED_LIST_ACCESS_DATA.keys'
       * buffer at offset 'key_size * iter'.
       */
      res = SORTED_LIST_ACCESS_DATA.keys.memwrite(unit,sorted_list_index,key,iter * key_size, key_size) ;
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 26, exit) ;
    }
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_sorted_list_entry_update()",0,0);
}

uint32
  soc_sand_sorted_list_entry_get_data(
    SOC_SAND_IN     int                              unit,
    SOC_SAND_IN     SOC_SAND_SORTED_LIST_PTR         sorted_list,
    SOC_SAND_IN     SOC_SAND_SORTED_LIST_ITER        iter,
    SOC_SAND_OUT    SOC_SAND_SORTED_LIST_DATA        *const data
  )
{
  uint32
    res;
  uint32
    sorted_list_index ;
  uint32
    data_size,
    list_size ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_SORTED_LIST_VERIFY_UNIT_IS_LEGAL(unit, 2) ;
  SOC_SAND_SORTED_LIST_CONVERT_HANDLE_TO_SORTEDLIST_INDEX(sorted_list_index,sorted_list) ;
  SOC_SAND_SORTED_LIST_VERIFY_SORTEDLIST_IS_ACTIVE(sorted_list_index,4,6) ;

  SOC_SAND_CHECK_NULL_INPUT(data);

  res = SORTED_LIST_ACCESS_INFO.list_size.get(unit,sorted_list_index,&list_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 8, exit);
  if (iter == SOC_SAND_SORTED_LIST_ITER_END(unit,sorted_list) || iter >= list_size)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_VALUE_OUT_OF_RANGE_ERR, 10, exit);
  }
  res = SORTED_LIST_ACCESS_INFO.data_size.get(unit,sorted_list_index,&data_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 18, exit);
  /*
   * Copy 'data_size' bytes from 'SORTED_LIST_ACCESS_DATA.data' buffer at offset
   * 'data_size * iter' into input 'data' buffer.
   */
  res = SORTED_LIST_ACCESS_DATA.data.memread(unit,sorted_list_index,data,iter * data_size, data_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 26, exit) ;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_sorted_list_entry_get_data()",0,0);
}

uint32
  soc_sand_sorted_list_entry_remove_by_iter(
    SOC_SAND_IN     int                             unit,
    SOC_SAND_IN     SOC_SAND_SORTED_LIST_PTR        sorted_list,
    SOC_SAND_IN     SOC_SAND_SORTED_LIST_ITER       iter
  )
{
  SOC_SAND_SORTED_LIST_ITER
    prev,
    next;
  uint32
    res;
  uint32
    sorted_list_index,
    null_ptr,
    ptr_size ;
  SOC_SAND_OCC_BM_PTR
    memory_use ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_SORTED_LIST_ENTRY_REMOVE_BY_ITER);
  SOC_SAND_SORTED_LIST_VERIFY_UNIT_IS_LEGAL(unit, 2) ;
  SOC_SAND_SORTED_LIST_CONVERT_HANDLE_TO_SORTEDLIST_INDEX(sorted_list_index,sorted_list) ;
  SOC_SAND_SORTED_LIST_VERIFY_SORTEDLIST_IS_ACTIVE(sorted_list_index,4,6) ;

  res = soc_sand_sorted_list_get_next_aux(
           unit,
           sorted_list,
           iter,
           TRUE,
           NULL,
           NULL,&next
         ) ;
  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);
  res = soc_sand_sorted_list_get_next_aux(
           unit,
           sorted_list,
           iter,
           FALSE,
           NULL,
           NULL,&prev
         ) ;
  SOC_SAND_CHECK_FUNC_RESULT(res, 14, exit);
  res = soc_sand_sorted_list_node_link_set(
            unit,
            sorted_list,
            prev,
            next
          ) ;
  SOC_SAND_CHECK_FUNC_RESULT(res, 16, exit);

  res = SORTED_LIST_ACCESS_DATA.null_ptr.get(unit,sorted_list_index,&null_ptr) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit) ;
  res = SORTED_LIST_ACCESS_DATA.ptr_size.get(unit,sorted_list_index,&ptr_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 22, exit) ;

  /* if(SOC_DPP_WB_ENGINE_VAR_NONE == init_info_ptr->wb_var_index) */
  {
    /*
     * Copy 'ptr_size' bytes from '&null_ptr' (treated as a small buffer) into
     * 'SORTED_LIST_ACCESS_DATA.next' buffer at offset 'ptr_size * iter'.
     */
    res = SORTED_LIST_ACCESS_DATA.next.memwrite(unit,sorted_list_index,(uint8 *)&null_ptr,iter * ptr_size, ptr_size) ;
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 24, exit) ;
  }

  /* if(SOC_DPP_WB_ENGINE_VAR_NONE == init_info_ptr->wb_var_index) */
  {
    /*
     * Copy 'ptr_size' bytes from '&null_ptr' (treated as a small buffer) into
     * 'SORTED_LIST_ACCESS_DATA.prev' buffer at offset 'ptr_size * iter'.
     */
    res = SORTED_LIST_ACCESS_DATA.prev.memwrite(unit,sorted_list_index,(uint8 *)&null_ptr,iter * ptr_size, ptr_size) ;
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 26, exit) ;
  }
  res = SORTED_LIST_ACCESS_DATA.memory_use.get(unit,sorted_list_index,&memory_use) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 28, exit) ;
  res = soc_sand_occ_bm_occup_status_set(
          unit,
          memory_use,
          iter,
          FALSE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_sorted_list_entry_remove_by_iter()",0,0);
}

uint32
  soc_sand_sorted_list_entry_lookup(
    SOC_SAND_IN     int                          unit,
    SOC_SAND_IN     SOC_SAND_SORTED_LIST_PTR     sorted_list,
    SOC_SAND_IN     SOC_SAND_SORTED_LIST_KEY     *const key,
    SOC_SAND_IN     SOC_SAND_SORTED_LIST_DATA    *const data,
    SOC_SAND_OUT    uint8                        *found,
    SOC_SAND_OUT    SOC_SAND_SORTED_LIST_ITER    *iter
  )
{
  uint32
    curr_node,
    prev_node;
  uint32
    res;
  uint32
    sorted_list_index,
    null_ptr ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_SORTED_LIST_ENTRY_LOOKUP);
  SOC_SAND_SORTED_LIST_VERIFY_UNIT_IS_LEGAL(unit, 2) ;
  SOC_SAND_SORTED_LIST_CONVERT_HANDLE_TO_SORTEDLIST_INDEX(sorted_list_index,sorted_list) ;
  SOC_SAND_SORTED_LIST_VERIFY_SORTEDLIST_IS_ACTIVE(sorted_list_index,4,6) ;

  SOC_SAND_CHECK_NULL_INPUT(key) ;
  SOC_SAND_CHECK_NULL_INPUT(iter) ;
  /*
   * Check to see whether the entry exists
   */
  if (data != NULL)
  {
    res = soc_sand_sorted_list_find_match_entry(
            unit,
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
            unit,
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
    res = SORTED_LIST_ACCESS_DATA.null_ptr.get(unit,sorted_list_index,&null_ptr) ;
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit) ;
    *iter = null_ptr;
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_sorted_list_entry_lookup()",0,0);
}


uint32
  soc_sand_sorted_list_entry_value(
    SOC_SAND_IN     int                          unit,
    SOC_SAND_IN     SOC_SAND_SORTED_LIST_PTR     sorted_list,
    SOC_SAND_IN     SOC_SAND_SORTED_LIST_ITER    iter,
    SOC_SAND_OUT    SOC_SAND_SORTED_LIST_KEY     *const key,
    SOC_SAND_OUT    SOC_SAND_SORTED_LIST_DATA    *const data
  )
{
  uint32
    res;
  uint32
    sorted_list_index,
    key_size,
    data_size,
    list_size ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_SORTED_LIST_ENTRY_VALUE);
  SOC_SAND_SORTED_LIST_VERIFY_UNIT_IS_LEGAL(unit, 2) ;
  SOC_SAND_SORTED_LIST_CONVERT_HANDLE_TO_SORTEDLIST_INDEX(sorted_list_index,sorted_list) ;
  SOC_SAND_SORTED_LIST_VERIFY_SORTEDLIST_IS_ACTIVE(sorted_list_index,4,6) ;

  res = SORTED_LIST_ACCESS_INFO.list_size.get(unit,sorted_list_index,&list_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 8, exit);

  if (iter == SOC_SAND_SORTED_LIST_ITER_END(unit,sorted_list) || iter >= list_size)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_VALUE_OUT_OF_RANGE_ERR, 10, exit);
  }
  if (key != NULL)
  {
    res = SORTED_LIST_ACCESS_INFO.key_size.get(unit,sorted_list_index,&key_size) ;
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 18, exit);
    /*
     * read keys
     */
    /*
     * Copy 'key_size' bytes from 'SORTED_LIST_ACCESS_DATA.keys' buffer at offset
     * 'key_size * iter' into input 'key' buffer.
     */
    res = SORTED_LIST_ACCESS_DATA.keys.memread(unit,sorted_list_index,key,iter * key_size, key_size) ;
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 26, exit) ;
  }
  if (data != NULL)
  {
    res = SORTED_LIST_ACCESS_INFO.data_size.get(unit,sorted_list_index,&data_size) ;
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 28, exit);
    /*
     * read data
     */
    /*
     * Copy 'data_size' bytes from 'SORTED_LIST_ACCESS_DATA.data' buffer at offset
     * 'data_size * iter' into input 'data' buffer.
     */
    res = SORTED_LIST_ACCESS_DATA.data.memread(unit,sorted_list_index,data,iter * data_size, data_size) ;
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 38, exit) ;
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_sorted_list_entry_value()",0,0);
}


uint32
  soc_sand_sorted_list_get_next(
    SOC_SAND_IN     int                          unit,
    SOC_SAND_IN     SOC_SAND_SORTED_LIST_PTR     sorted_list,
    SOC_SAND_INOUT  SOC_SAND_SORTED_LIST_ITER    *iter,
    SOC_SAND_OUT    SOC_SAND_SORTED_LIST_KEY     *const key,
    SOC_SAND_OUT    SOC_SAND_SORTED_LIST_DATA    *const data
  )
{
  uint32
    res;
  uint32
    sorted_list_index ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_SORTED_LIST_GET_NEXT);
  SOC_SAND_SORTED_LIST_VERIFY_UNIT_IS_LEGAL(unit, 2) ;
  SOC_SAND_SORTED_LIST_CONVERT_HANDLE_TO_SORTEDLIST_INDEX(sorted_list_index,sorted_list) ;
  SOC_SAND_SORTED_LIST_VERIFY_SORTEDLIST_IS_ACTIVE(sorted_list_index,4,6) ;

  if (*iter == SOC_SAND_SORTED_LIST_ITER_END(unit,sorted_list))
  {
    goto exit ;
  }
  res = soc_sand_sorted_list_get_next_aux(
            unit,
            sorted_list,
            *iter,
            TRUE,
            key,
            data,iter
          ) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit) ;
  if (*iter == SOC_SAND_SORTED_LIST_ITER_END(unit,sorted_list))
  {
    goto exit;
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "soc_sand_sorted_list_get_next()",0,0);
}

uint32
  soc_sand_sorted_list_get_prev(
    SOC_SAND_IN     int                           unit,
    SOC_SAND_IN     SOC_SAND_SORTED_LIST_PTR      sorted_list,
    SOC_SAND_INOUT  SOC_SAND_SORTED_LIST_ITER     *iter,
    SOC_SAND_OUT    SOC_SAND_SORTED_LIST_KEY      *const key,
    SOC_SAND_OUT    SOC_SAND_SORTED_LIST_DATA     *const data
  )
{
  uint32
    head_place ;
  uint32
    sorted_list_index ;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_SORTED_LIST_GET_PREV);
  SOC_SAND_SORTED_LIST_VERIFY_UNIT_IS_LEGAL(unit, 2) ;
  SOC_SAND_SORTED_LIST_CONVERT_HANDLE_TO_SORTEDLIST_INDEX(sorted_list_index,sorted_list) ;
  SOC_SAND_SORTED_LIST_VERIFY_SORTEDLIST_IS_ACTIVE(sorted_list_index,4,6) ;

  SOC_SAND_CHECK_NULL_INPUT(data) ;
  SOC_SAND_CHECK_NULL_INPUT(key);

  SOC_SAND_SORTED_LIST_HEAD_PLACE_GET(sorted_list_index,head_place,12) ;
  if (*iter == head_place /* SOC_SAND_SORTED_LIST_HEAD_PLACE(&(sorted_list->init_info)) */)
  {
    goto exit;
  }
  res = soc_sand_sorted_list_get_next_aux(
            unit,
            sorted_list,
            *iter,
            FALSE,
            key,
            data,iter
          );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit) ;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "soc_sand_sorted_list_get_prev()",0,0);
}

uint32
  soc_sand_sorted_list_get_follow(
    SOC_SAND_IN     int                           unit,
    SOC_SAND_IN     SOC_SAND_SORTED_LIST_PTR      sorted_list,
    SOC_SAND_INOUT  SOC_SAND_SORTED_LIST_ITER     *iter,
    SOC_SAND_OUT    uint8                         forward,
    SOC_SAND_OUT    SOC_SAND_SORTED_LIST_KEY      *const key,
    SOC_SAND_OUT    SOC_SAND_SORTED_LIST_DATA     *const data
    )
{
  uint32
    head_place ;
  uint32
    sorted_list_index ;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_SORTED_LIST_GET_FOLLOW);
  SOC_SAND_SORTED_LIST_VERIFY_UNIT_IS_LEGAL(unit, 2) ;
  SOC_SAND_SORTED_LIST_CONVERT_HANDLE_TO_SORTEDLIST_INDEX(sorted_list_index,sorted_list) ;
  SOC_SAND_SORTED_LIST_VERIFY_SORTEDLIST_IS_ACTIVE(sorted_list_index,4,6) ;

  SOC_SAND_CHECK_NULL_INPUT(key);
  SOC_SAND_CHECK_NULL_INPUT(data);

  SOC_SAND_SORTED_LIST_HEAD_PLACE_GET(sorted_list_index,head_place,12) ;
  if (*iter == head_place /* SOC_SAND_SORTED_LIST_HEAD_PLACE(&(sorted_list->init_info)) */ )
  {
    goto exit;
  }
  res = soc_sand_sorted_list_get_next_aux(
            unit,
            sorted_list,
            *iter,
            forward,
            key,
            data,iter
          );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit) ;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "soc_sand_sorted_list_get_follow()",0,0);
}

/*********************************************************************
* NAME:
*     soc_sand_sorted_list_get_size_for_save
* TYPE:
*   PROC
* DATE:
*   May 19 2015
* FUNCTION:
*   Returns the size of the buffer needed to return the sorted list as buffer.
*   in sort to be loaded later
* INPUT:
*   SOC_SAND_IN  int                         unit -
*     Identifier of the device to access.
*   SOC_SAND_IN  SOC_SAND_SORTED_LIST_INFO   *sorted_list -
*     The sorted list to get the size for.
*   SOC_SAND_OUT  uint32                     *size -
*     the size of the buffer.
* REMARKS:
*   None.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
soc_sand_sorted_list_get_size_for_save(
     SOC_SAND_IN   int                            unit,
     SOC_SAND_IN   SOC_SAND_SORTED_LIST_PTR       sorted_list,
     SOC_SAND_OUT  uint32                         *size
   )
{
  uint32
    bmp_size,
    total_size ;
  uint32
    res;
  uint32
    tmp_size;
  uint32
    sorted_list_index,
    ptr_size,
    key_size,
    list_size,
    data_size ;
  SOC_SAND_OCC_BM_PTR
    memory_use ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_SORTED_LIST_VERIFY_UNIT_IS_LEGAL(unit, 2) ;
  SOC_SAND_SORTED_LIST_CONVERT_HANDLE_TO_SORTEDLIST_INDEX(sorted_list_index,sorted_list) ;
  SOC_SAND_SORTED_LIST_VERIFY_SORTEDLIST_IS_ACTIVE(sorted_list_index,4,6) ;

  total_size = 0;
  SOC_SAND_CHECK_NULL_INPUT(size);

  res = SORTED_LIST_ACCESS_DATA.ptr_size.get(unit,sorted_list_index,&ptr_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 12, exit) ;
  res = SORTED_LIST_ACCESS_INFO.data_size.get(unit,sorted_list_index,&data_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 14, exit);
  tmp_size = SOC_SAND_MAX(ptr_size,data_size);

  /* init info */
  total_size += sizeof(SOC_SAND_SORTED_LIST_INIT_INFO);

  /* DS data */
  res = SORTED_LIST_ACCESS_INFO.key_size.get(unit,sorted_list_index,&key_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 16, exit);
  res = SORTED_LIST_ACCESS_INFO.list_size.get(unit,sorted_list_index,&list_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 18, exit);

  total_size += sizeof(uint8) * tmp_size;

  total_size += sizeof(uint8) * key_size;

  total_size += sizeof(uint8) * list_size * key_size;
  
  total_size += sizeof(uint8) * (list_size + 2) * ptr_size;

  total_size += sizeof(uint8) * (list_size + 2) * ptr_size;

  total_size += sizeof(uint8) * list_size * data_size;
  /*
   * Initialize the data to be mapped to
   */
  res = SORTED_LIST_ACCESS_DATA.memory_use.get(unit,sorted_list_index,&memory_use) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 28, exit) ;
  res = soc_sand_occ_bm_get_size_for_save(
    unit,
    memory_use,
    &bmp_size
    );
  SOC_SAND_CHECK_FUNC_RESULT(res,30, exit) ;
  total_size += bmp_size ;

  *size = total_size ;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_hash_table_get_size_for_save()",0,0);
}

/*********************************************************************
* NAME:
*     soc_sand_sorted_list_save
* TYPE:
*   PROC
* DATE:
*   May 19 2015
* FUNCTION:
*     saves the given sorted list in the given buffer
* INPUT:
*   SOC_SAND_IN  int                           unit -
*     Identifier of the device to access.
*   SOC_SAND_IN  SOC_SAND_SORTED_LIST_PTR      sorted_list -
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
    SOC_SAND_IN  SOC_SAND_SORTED_LIST_PTR      sorted_list,
    SOC_SAND_OUT uint8                         *buffer,
    SOC_SAND_IN  uint32                        buffer_size_bytes,
    SOC_SAND_OUT uint32                        *actual_size_bytes
  )
{
  uint8 
    *cur_ptr ;
  uint32
    tmp_size;
  uint32
    cur_size,
    total_size ;
  uint32
    res;
  uint32
    sorted_list_index,
    ptr_size,
    key_size,
    list_size,
    data_size ;
  SOC_SAND_OCC_BM_PTR
    memory_use ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_SORTED_LIST_VERIFY_UNIT_IS_LEGAL(unit, 2) ;
  SOC_SAND_SORTED_LIST_CONVERT_HANDLE_TO_SORTEDLIST_INDEX(sorted_list_index,sorted_list) ;
  SOC_SAND_SORTED_LIST_VERIFY_SORTEDLIST_IS_ACTIVE(sorted_list_index,4,6) ;

  SOC_SAND_CHECK_NULL_INPUT(buffer);
  SOC_SAND_CHECK_NULL_INPUT(actual_size_bytes);

  cur_ptr = buffer ;
  total_size = 0 ;

  res = SORTED_LIST_ACCESS_DATA.ptr_size.get(unit,sorted_list_index,&ptr_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 12, exit) ;
  res = SORTED_LIST_ACCESS_INFO.data_size.get(unit,sorted_list_index,&data_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 14, exit) ;
  tmp_size = SOC_SAND_MAX(ptr_size,data_size) ;
  /*
   * copy init info structure (SOC_SAND_SORTED_LIST_INIT_INFO) into supplied (input) buffer area.
   */
  res = SORTED_LIST_ACCESS_INFO.get(unit,sorted_list_index,(SOC_SAND_SORTED_LIST_INIT_INFO *)cur_ptr) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 16, exit) ;
  cur_ptr += sizeof(SOC_SAND_SORTED_LIST_INIT_INFO) ;
  /*
   * Copy DS data
   */
  res = SORTED_LIST_ACCESS_INFO.key_size.get(unit,sorted_list_index,&key_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 16, exit);
  res = SORTED_LIST_ACCESS_INFO.list_size.get(unit,sorted_list_index,&list_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 18, exit);

  res = SORTED_LIST_ACCESS_DATA.tmp_data.memwrite(unit,sorted_list_index,cur_ptr,0,tmp_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 22, exit) ;
  cur_ptr += tmp_size ;

  res = SORTED_LIST_ACCESS_DATA.tmp_key.memwrite(unit,sorted_list_index,cur_ptr,0,key_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 24, exit) ;
  cur_ptr += key_size ;

  res = SORTED_LIST_ACCESS_DATA.keys.memwrite(unit,sorted_list_index,cur_ptr,0,list_size * key_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 26, exit) ;
  cur_ptr += (list_size * key_size) ;

  res = SORTED_LIST_ACCESS_DATA.next.memwrite(unit,sorted_list_index,cur_ptr,0,(list_size + 2) * ptr_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 28, exit) ;
  cur_ptr += ((list_size + 2) * ptr_size) ;

  res = SORTED_LIST_ACCESS_DATA.prev.memwrite(unit,sorted_list_index,cur_ptr,0,(list_size + 2) * ptr_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit) ;
  cur_ptr += ((list_size + 2) * ptr_size) ;

  res = SORTED_LIST_ACCESS_DATA.data.memwrite(unit,sorted_list_index,cur_ptr,0,list_size * data_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit) ;
  cur_ptr += (list_size * data_size) ;
  /*
   * Initialize the data to be mapped to
   */
  res = SORTED_LIST_ACCESS_DATA.memory_use.get(unit,sorted_list_index,&memory_use) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 28, exit) ;
  res = soc_sand_occ_bm_save(
          unit,
          memory_use,
          cur_ptr,
          buffer_size_bytes - total_size,
          &cur_size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res,36, exit) ;
  cur_ptr += cur_size ;
  total_size += cur_size ;

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
*   May 19 2015
* FUNCTION:
*   Load sorted list from the given buffer which contains a previously
*   saved sorted list.
* INPUT:
*   SOC_SAND_IN  int                                   unit -
*     Identifier of the device to access.
*   SOC_SAND_IN  uint8                                 **buffer -
*     buffer includes the sorted list
*   SOC_SAND_IN  SOC_SAND_SORTED_LIST_SW_DB_ENTRY_GET  get_entry_fun,
*   SOC_SAND_IN  SOC_SAND_SORTED_LIST_SW_DB_ENTRY_SET  set_entry_fun,
*   SOC_SAND_IN  SOC_SAND_SORTED_LIST_KEY_CMP          cmp_key_fun,
*   SOC_SAND_OUT SOC_SAND_SORTED_LIST_PTR              *sorted_list_ptr -
*     Handle to the sorted list to load.
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
    SOC_SAND_IN  uint8                                 **buffer,
    SOC_SAND_IN  SOC_SAND_SORTED_LIST_SW_DB_ENTRY_GET  get_entry_fun,
    SOC_SAND_IN  SOC_SAND_SORTED_LIST_SW_DB_ENTRY_SET  set_entry_fun,
    SOC_SAND_IN  SOC_SAND_SORTED_LIST_KEY_CMP          cmp_key_fun,                      
    SOC_SAND_OUT SOC_SAND_SORTED_LIST_PTR              *sorted_list_ptr
  )
{
  SOC_SAND_SORTED_LIST_INIT_INFO
    local_init_info ;
  SOC_SAND_IN uint8
    *cur_ptr ;
  uint32
    res;
  uint32
    tmp_size ;
  SOC_SAND_SORTED_LIST_PTR
    sorted_list ;
  uint32
    sorted_list_index,
    ptr_size,
    key_size,
    list_size,
    data_size ;
  SOC_SAND_OCC_BM_PTR
    memory_use ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_SORTED_LIST_VERIFY_UNIT_IS_LEGAL(unit, 2) ;

  SOC_SAND_CHECK_NULL_INPUT(buffer);
  SOC_SAND_CHECK_NULL_INPUT(sorted_list_ptr);

  cur_ptr = buffer[0] ;
  /*
   * copy init info (structure SOC_SAND_SORTED_LIST_INIT_INFO) from supplied (input) buffer
   * area into local memory and create sorted list.
   */
  soc_sand_os_memcpy(&local_init_info, cur_ptr, sizeof(SOC_SAND_SORTED_LIST_INIT_INFO));
  cur_ptr += sizeof(SOC_SAND_SORTED_LIST_INIT_INFO);
  local_init_info.get_entry_fun = get_entry_fun ;
  local_init_info.set_entry_fun = set_entry_fun ;
  /*
   * create DS - will not work!! (petra only code)
   */
  res = soc_sand_sorted_list_create(unit, sorted_list_ptr, local_init_info);
  SOC_SAND_CHECK_FUNC_RESULT(res,20, exit) ;
  sorted_list = *sorted_list_ptr ;
  SOC_SAND_SORTED_LIST_CONVERT_HANDLE_TO_SORTEDLIST_INDEX(sorted_list_index,sorted_list) ;

  res = SORTED_LIST_ACCESS_DATA.ptr_size.get(unit,sorted_list_index,&ptr_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 12, exit) ;
  res = SORTED_LIST_ACCESS_INFO.data_size.get(unit,sorted_list_index,&data_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 14, exit) ;
  tmp_size = SOC_SAND_MAX(ptr_size,data_size);

  /* copy DS data */
  res = SORTED_LIST_ACCESS_INFO.key_size.get(unit,sorted_list_index,&key_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 16, exit);
  res = SORTED_LIST_ACCESS_INFO.list_size.get(unit,sorted_list_index,&list_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 18, exit);

  res = SORTED_LIST_ACCESS_DATA.tmp_data.memread(unit,sorted_list_index,(uint8 *)cur_ptr,0,tmp_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 22, exit) ;
  cur_ptr += tmp_size ;

  res = SORTED_LIST_ACCESS_DATA.tmp_key.memread(unit,sorted_list_index,(uint8 *)cur_ptr,0,key_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 24, exit) ;
  cur_ptr += key_size ;

  res = SORTED_LIST_ACCESS_DATA.keys.memread(unit,sorted_list_index,(uint8 *)cur_ptr,0,list_size * key_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 26, exit) ;
  cur_ptr += (list_size * key_size) ;

  res = SORTED_LIST_ACCESS_DATA.next.memread(unit,sorted_list_index,(uint8 *)cur_ptr,0,(list_size + 2) * ptr_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 28, exit) ;
  cur_ptr += ((list_size + 2) * ptr_size) ;

  res = SORTED_LIST_ACCESS_DATA.prev.memread(unit,sorted_list_index,(uint8 *)cur_ptr,0,(list_size + 2) * ptr_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit) ;
  cur_ptr += ((list_size + 2) * ptr_size) ;

  res = SORTED_LIST_ACCESS_DATA.data.memread(unit,sorted_list_index,(uint8 *)cur_ptr,0,list_size * data_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 32, exit) ;
  cur_ptr += (list_size * data_size) ;

  /* Destroy (release) bitmap */
  res = SORTED_LIST_ACCESS_DATA.memory_use.get(unit,sorted_list_index,&memory_use) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 34, exit) ;
  res = soc_sand_occ_bm_destroy(
    unit,
    memory_use
    );
  SOC_SAND_CHECK_FUNC_RESULT(res,36, exit);

  /* Load bitmap */
  res = soc_sand_occ_bm_load(
    unit,
    &cur_ptr,
    &memory_use
    );
  SOC_SAND_CHECK_FUNC_RESULT(res,38, exit);
  res = SORTED_LIST_ACCESS_DATA.memory_use.set(unit,sorted_list_index,memory_use) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit) ;

  *buffer = cur_ptr ;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_hash_table_load()",0,0);
}

uint32
  soc_sand_sorted_list_print(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_SAND_SORTED_LIST_PTR       sorted_list,
    SOC_SAND_IN  char                           table_header[SOC_SAND_SORTED_LIST_HEADER_SIZE],
    SOC_SAND_IN  SOC_SAND_SORTED_LIST_PRINT_VAL print_key,
    SOC_SAND_IN  SOC_SAND_SORTED_LIST_PRINT_VAL print_data
  )
{
  uint32
    prev,
    curr;
  uint32
    sorted_list_index ;
  uint32
    res;
  uint8
    *tmp_data_ptr ;
  uint8
    *tmp_key_ptr ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_SORTED_LIST_PRINT);
  SOC_SAND_SORTED_LIST_VERIFY_UNIT_IS_LEGAL(unit, 2) ;
  SOC_SAND_SORTED_LIST_CONVERT_HANDLE_TO_SORTEDLIST_INDEX(sorted_list_index,sorted_list) ;
  SOC_SAND_SORTED_LIST_VERIFY_SORTEDLIST_IS_ACTIVE(sorted_list_index,4,6) ;
  /*
   * traverse the sorted list head list.
   */
  LOG_CLI((BSL_META_U(unit,"  %s\n"), table_header));
  SOC_SAND_SORTED_LIST_HEAD_PLACE_GET(sorted_list_index,prev,12) ;
  res = soc_sand_sorted_list_get_tmp_data_ptr_from_handle(unit,sorted_list,&tmp_data_ptr) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
  res = soc_sand_sorted_list_get_tmp_key_ptr_from_handle(unit,sorted_list,&tmp_key_ptr) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
  do
  {
    /*
     * read next entry.
     */
    res = soc_sand_sorted_list_get_next_aux(
            unit,
            sorted_list,
            prev,
            TRUE,
            tmp_key_ptr,
            tmp_data_ptr,&curr
          );
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 12, exit);
    if (curr == SOC_SAND_SORTED_LIST_ITER_END(unit,sorted_list))
    {
      goto exit;
    }
    LOG_CLI((BSL_META_U(unit,"   %-10u"), curr)) ;
    print_data(tmp_data_ptr);
    print_key(tmp_key_ptr);
    LOG_CLI((BSL_META_U(unit,"\n")));

    prev = curr;
  }
  while(curr != SOC_SAND_SORTED_LIST_ITER_END(unit,sorted_list));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_sorted_list_print()",0,0);
}

uint32
  soc_sand_sorted_list_data_cmp(
    SOC_SAND_IN    int                             unit,
    SOC_SAND_IN    SOC_SAND_SORTED_LIST_PTR        sorted_list,
    SOC_SAND_IN    uint32                          data_place,
    SOC_SAND_IN    SOC_SAND_SORTED_LIST_DATA       *const data,
    SOC_SAND_INOUT int32                           *cmp_res_ptr
  )
{
  uint32
    res,
    sorted_list_index,
    data_size ;
  int32
    cmp_res ;
  SOC_SAND_SORTED_LIST_KEY_CMP 
    cmp_key_fun;
  uint8
    *tmp_data_ptr ;
  SOC_SAND_SORTED_LIST_CMP_FUNC_TYPE
    cmp_func_type ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_SORTED_LIST_PRINT);
  SOC_SAND_SORTED_LIST_VERIFY_UNIT_IS_LEGAL(unit, 2) ;
  SOC_SAND_SORTED_LIST_CONVERT_HANDLE_TO_SORTEDLIST_INDEX(sorted_list_index,sorted_list) ;
  SOC_SAND_SORTED_LIST_VERIFY_SORTEDLIST_IS_ACTIVE(sorted_list_index,4,6) ;

  res = SORTED_LIST_ACCESS_INFO.cmp_func_type.get(unit,sorted_list_index,&cmp_func_type) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 8, exit);
  if (cmp_func_type == SOC_SAND_SORTED_LIST_CMP_FUNC_TYPE_TCAM)
  {
    cmp_key_fun = soc_sand_sorted_list_tcam_cmp_priority;
  }
  else
  {
    cmp_key_fun = (SOC_SAND_SORTED_LIST_KEY_CMP)soc_sand_os_memcmp;
  }
  {
    /*
     * read data: Copy 'data' to 'tmp_data'
     */
    res = SORTED_LIST_ACCESS_INFO.data_size.get(unit,sorted_list_index,&data_size) ;
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit) ;
    res = soc_sand_sorted_list_get_tmp_data_ptr_from_handle(unit,sorted_list,&tmp_data_ptr) ;
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 12, exit);
    /*
     * Copy 'data_size' bytes from 'SORTED_LIST_ACCESS_DATA.data' buffer at offset
     * 'data_size * data_place' into 'tmp_data' buffer.
     */
    res = SORTED_LIST_ACCESS_DATA.data.memread(unit,sorted_list_index,tmp_data_ptr,data_place * data_size, data_size) ;
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 14, exit) ;
    /*
     * Now compare input 'data' buffer with data stored in sorted list using
     * indicated compare function.
     */
    cmp_res =
      cmp_key_fun(
        tmp_data_ptr,
        data,
        data_size * sizeof(uint8)
      ) ;
    *cmp_res_ptr = cmp_res;
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_sorted_list_data_cmp()",0,0);
}

uint32
  soc_sand_sorted_list_get_next_aux(
    SOC_SAND_IN    int                         unit,
    SOC_SAND_IN    SOC_SAND_SORTED_LIST_PTR    sorted_list,
    SOC_SAND_IN    SOC_SAND_SORTED_LIST_ITER   curr,
    SOC_SAND_IN    uint8                       forward,
    SOC_SAND_OUT   SOC_SAND_SORTED_LIST_KEY    *const key,
    SOC_SAND_OUT   SOC_SAND_SORTED_LIST_DATA   *const data,
    SOC_SAND_INOUT SOC_SAND_SORTED_LIST_ITER   *next_or_prev
  )
{
  uint32
    ptr_long;
  uint32
    res;
  uint32
    null_ptr,
    sorted_list_index ;
  uint8
    *tmp_data_ptr ;
  uint32
    ptr_size,
    key_size,
    data_size ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_SORTED_LIST_VERIFY_UNIT_IS_LEGAL(unit, 2) ;
  SOC_SAND_SORTED_LIST_CONVERT_HANDLE_TO_SORTEDLIST_INDEX(sorted_list_index,sorted_list) ;
  SOC_SAND_SORTED_LIST_VERIFY_SORTEDLIST_IS_ACTIVE(sorted_list_index,4,6) ;

  SOC_SAND_CHECK_NULL_INPUT(next_or_prev);

  res = SORTED_LIST_ACCESS_DATA.null_ptr.get(unit,sorted_list_index,&null_ptr) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 8, exit) ;
  /*
   * read next entry.
   */
  if (
    (!forward &&  curr == SOC_SAND_SORTED_LIST_ITER_BEGIN(unit,sorted_list)) ||
    ((forward && curr == SOC_SAND_SORTED_LIST_ITER_END(unit,sorted_list)))
    )
  {
    *next_or_prev = null_ptr ;
    goto exit ;
  }
  res = soc_sand_sorted_list_get_tmp_data_ptr_from_handle(unit,sorted_list,&tmp_data_ptr) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 12, exit);
  res = SORTED_LIST_ACCESS_DATA.ptr_size.get(unit,sorted_list_index,&ptr_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 14, exit) ;
  if (forward)
  {
    /*
     * Copy 'ptr_size' bytes from 'SORTED_LIST_ACCESS_DATA.next' buffer at offset
     * 'curr * ptr_size' into 'tmp_data' buffer.
     */
    res = SORTED_LIST_ACCESS_DATA.next.memread(unit,sorted_list_index,tmp_data_ptr,curr * ptr_size,ptr_size) ;
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 16, exit) ;
  }
  else
  {
    /*
     * Copy 'ptr_size' bytes from 'SORTED_LIST_ACCESS_DATA.prev' buffer at offset
     * 'curr * ptr_size' into 'tmp_data' buffer.
     */
    res = SORTED_LIST_ACCESS_DATA.prev.memread(unit,sorted_list_index,tmp_data_ptr,curr * ptr_size,ptr_size) ;
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 18, exit) ;
  }
  /*
   * Check whether the list node is null.
   */
  ptr_long = 0 ;
  soc_sand_U8_to_U32(
      tmp_data_ptr,
      ptr_size,
      &ptr_long
  ) ;
  if (ptr_long == null_ptr)
  {
    *next_or_prev = null_ptr ;
    goto exit ;
  }
  if (ptr_long == SOC_SAND_SORTED_LIST_ITER_BEGIN(unit,sorted_list))
  {
    *next_or_prev = ptr_long ;
    goto exit ;
  }
  if (key != NULL)
  {
    /*
     * Read keys
     */
    res = SORTED_LIST_ACCESS_INFO.key_size.get(unit,sorted_list_index,&key_size) ;
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit) ;
    /*
     * Copy 'key_size' bytes from 'SORTED_LIST_ACCESS_DATA.keys' buffer at offset
     * 'ptr_long * key_size' into input 'key' buffer.
     */
    res = SORTED_LIST_ACCESS_DATA.keys.memread(unit,sorted_list_index,key,ptr_long * key_size,key_size) ;
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 22, exit) ;
  }
  if (data != NULL)
  {
    /*
     * read data
     */
    res = SORTED_LIST_ACCESS_INFO.data_size.get(unit,sorted_list_index,&data_size) ;
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 24, exit) ;
    /*
     * Copy 'data_size' bytes from 'SORTED_LIST_ACCESS_DATA.data' buffer at offset
     * 'ptr_long * data_size' into input 'data' buffer.
     */
    res = SORTED_LIST_ACCESS_DATA.data.memread(unit,sorted_list_index,data,ptr_long * data_size,data_size) ;
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 26, exit) ;
  }
  *next_or_prev = ptr_long ;
  goto exit ;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_sorted_list_get_next_aux()",0,0);
}

uint32
  soc_sand_SAND_SORTED_LIST_INFO_clear(
    SOC_SAND_IN int                      unit,
    SOC_SAND_IN SOC_SAND_SORTED_LIST_PTR sorted_list
  )
{
  uint32
    sorted_list_index ;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_SORTED_LIST_VERIFY_UNIT_IS_LEGAL(unit, 2) ;
  SOC_SAND_SORTED_LIST_CONVERT_HANDLE_TO_SORTEDLIST_INDEX(sorted_list_index,sorted_list) ;
  SOC_SAND_SORTED_LIST_VERIFY_SORTEDLIST_IS_ACTIVE(sorted_list_index,4,6) ;

  res = SORTED_LIST_ACCESS_INFO.data_size.set(unit, sorted_list_index, 0);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 8, exit);
  res = SORTED_LIST_ACCESS_INFO.get_entry_fun.set(unit, sorted_list_index, 0);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
  res = SORTED_LIST_ACCESS_INFO.key_size.set(unit, sorted_list_index, 0);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 12, exit);
  res = SORTED_LIST_ACCESS_INFO.set_entry_fun.set(unit, sorted_list_index, 0);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 14, exit);
  res = SORTED_LIST_ACCESS_INFO.list_size.set(unit, sorted_list_index, 0);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 16, exit);
  res = SORTED_LIST_ACCESS_INFO.prime_handle.set(unit, sorted_list_index, 0);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 18, exit);
  res = SORTED_LIST_ACCESS_INFO.sec_handle.set(unit, sorted_list_index, 0);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
  res = SORTED_LIST_ACCESS_INFO.cmp_func_type.set(unit, sorted_list_index, SOC_SAND_SORTED_LIST_CMP_FUNC_TYPE_DEFAULT);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 22, exit);

  res = SORTED_LIST_ACCESS_DATA.ptr_size.set(unit, sorted_list_index, 0);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 24, exit);
  res = SORTED_LIST_ACCESS_DATA.memory_use.set(unit, sorted_list_index, 0);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 26, exit);
  res = SORTED_LIST_ACCESS_DATA.null_ptr.set(unit, sorted_list_index, 0);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 28, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_sand_SAND_SORTED_LIST_INFO_clear()", 0, 0);
}

#if SOC_SAND_DEBUG
/* { */
uint32
  soc_sand_SAND_SORTED_LIST_INFO_print(
    SOC_SAND_IN int                      unit,
    SOC_SAND_IN SOC_SAND_SORTED_LIST_PTR sorted_list
  )
{
  uint32
    res,
    key_size,
    list_size,
    data_size,
    sec_handle,
    sorted_list_index ;
  int
    prime_handle ;    

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_SORTED_LIST_VERIFY_UNIT_IS_LEGAL(unit, 2) ;
  SOC_SAND_SORTED_LIST_CONVERT_HANDLE_TO_SORTEDLIST_INDEX(sorted_list_index,sorted_list) ;
  SOC_SAND_SORTED_LIST_VERIFY_SORTEDLIST_IS_ACTIVE(sorted_list_index,4,6) ;

  res = SORTED_LIST_ACCESS_INFO.data_size.get(unit,sorted_list_index,&data_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 8, exit) ;
  res = SORTED_LIST_ACCESS_INFO.key_size.get(unit,sorted_list_index,&key_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit) ;
  res = SORTED_LIST_ACCESS_INFO.list_size.get(unit,sorted_list_index,&list_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 12, exit) ;
  res = SORTED_LIST_ACCESS_INFO.prime_handle.get(unit,sorted_list_index,&prime_handle) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 14, exit) ;
  res = SORTED_LIST_ACCESS_INFO.sec_handle.get(unit,sorted_list_index,&sec_handle) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 16, exit) ;

  LOG_CLI((BSL_META_U(unit,"init_info.data_size   : %u\n"),data_size)) ;
  LOG_CLI((BSL_META_U(unit,"init_info.key_size    : %u\n"),key_size)) ;
  LOG_CLI((BSL_META_U(unit,"init_info.list_size   : %u\n"),list_size)) ;
  LOG_CLI((BSL_META_U(unit,"init_info.prime_handle: %u\n"),prime_handle)) ;
  LOG_CLI((BSL_META_U(unit,"init_info.sec_handle  : %u\n"),sec_handle)) ;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_sand_SAND_SORTED_LIST_INFO_print()", 0, 0) ;
}
/* } */
#endif /* SOC_SAND_DEBUG */

STATIC uint32
  soc_sand_sorted_list_node_link_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_SAND_SORTED_LIST_PTR sorted_list,
    SOC_SAND_IN  uint32                   node1,
    SOC_SAND_IN  uint32                   node2
  )
{
  uint32
    sorted_list_index,
    ptr_size,
    res ;
  uint8
    *tmp_data_ptr ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0) ;
  SOC_SAND_SORTED_LIST_VERIFY_UNIT_IS_LEGAL(unit, 2) ;
  SOC_SAND_SORTED_LIST_CONVERT_HANDLE_TO_SORTEDLIST_INDEX(sorted_list_index,sorted_list) ;
  SOC_SAND_SORTED_LIST_VERIFY_SORTEDLIST_IS_ACTIVE(sorted_list_index,4,6) ;

  res = soc_sand_sorted_list_get_tmp_data_ptr_from_handle(unit,sorted_list,&tmp_data_ptr) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 12, exit);
  res = SORTED_LIST_ACCESS_DATA.ptr_size.get(unit,sorted_list_index,&ptr_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 14, exit) ;
  res =
    soc_sand_U32_to_U8(
      &node2,
      ptr_size,
      tmp_data_ptr
    ) ;
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  /* if(SOC_DPP_WB_ENGINE_VAR_NONE == init_info_ptr->wb_var_index) */
  {
    /*
     * Copy 'ptr_size' bytes from 'tmp_data_ptr' into
     * 'SORTED_LIST_ACCESS_DATA.next' buffer at offset 'ptr_size * node1'.
     */
    res = SORTED_LIST_ACCESS_DATA.next.memwrite(unit,sorted_list_index,tmp_data_ptr,node1 * ptr_size, ptr_size) ;
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit) ;
  }
  res =
    soc_sand_U32_to_U8(
      &node1,
      ptr_size,
      tmp_data_ptr
    ) ;
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  /* if(SOC_DPP_WB_ENGINE_VAR_NONE == init_info_ptr->wb_var_index) */
  {
    /*
     * Copy 'ptr_size' bytes from 'tmp_data_ptr' into
     * 'SORTED_LIST_ACCESS_DATA.prev' buffer at offset 'ptr_size * node2'.
     */
    res = SORTED_LIST_ACCESS_DATA.prev.memwrite(unit,sorted_list_index,tmp_data_ptr,node2 * ptr_size, ptr_size) ;
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit) ;
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_sorted_list_node_link_set()",0,0);
}
/*********************************************************************
* NAME:
*     soc_sand_sorted_list_find_match_entry
* TYPE:
*   PROC
* DATE:
*   May 19 2015
* FUNCTION:
*  lookup in the sorted list for the given key and return the data inserted with
*  the given key.
* INPUT:
*   SOC_SAND_IN  int                              unit -
*     Identifier of the device to access.
*   SOC_SAND_IN  SOC_SAND_SORTED_LIST_PTR         sorted_list -
*     Handle to the sorted list.
*   SOC_SAND_IN  SOC_SAND_SORTED_LIST_KEY         *const key -
*     The key sort to lookup for
*   SOC_SAND_IN  SOC_SAND_SORTED_LIST_DATA        *const data
*     This procedure loads pointed memory by data
*     of matching entry. If set to NULL then this
*     procedure does not load indicated data.
*   SOC_SAND_IN  uint8                            first_match
*     whether to return the first match key. regardless of the data.
*   SOC_SAND_IN  uint8                            *found -
*     whether the data with the given key was found in the sorted list
*   SOC_SAND_OUT  uint32                          *entry -
*     if the key is present in the sorted list then return the entry the key found at,
*     otherwise it returns the place where the key suppose to be.
*   SOC_SAND_OUT  SOC_SAND_SORTED_LIST_ITER       *prev_node
*     iterator points to one node before the searched node.
*   SOC_SAND_OUT  SOC_SAND_SORTED_LIST_ITER       *next_node
*     iterator points to searched node or node after.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
STATIC uint32
  soc_sand_sorted_list_find_match_entry(
    SOC_SAND_IN   int                           unit,
    SOC_SAND_IN   SOC_SAND_SORTED_LIST_PTR      sorted_list,
    SOC_SAND_IN   SOC_SAND_SORTED_LIST_KEY      *const key,
    SOC_SAND_IN   SOC_SAND_SORTED_LIST_DATA     *const data,
    SOC_SAND_IN   uint8                         first_match,
    SOC_SAND_OUT  uint8                         *found,
    SOC_SAND_OUT  SOC_SAND_SORTED_LIST_ITER     *prev_node,
    SOC_SAND_OUT  SOC_SAND_SORTED_LIST_ITER     *cur_node
  )
{
  uint32
    key_size,
    null_ptr,
    prev,
    curr;
  SOC_SAND_SORTED_LIST_KEY_CMP 
    cmp_key_fun ;
  SOC_SAND_SORTED_LIST_CMP_FUNC_TYPE
    cmp_func_type ;
  int32
    compare_res ;
  uint32
    sorted_list_index,
    res;
  uint8
    *tmp_key_ptr ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_SORTED_LIST_FIND_MATCH_ENTRY);
  SOC_SAND_SORTED_LIST_VERIFY_UNIT_IS_LEGAL(unit, 2) ;
  SOC_SAND_SORTED_LIST_CONVERT_HANDLE_TO_SORTEDLIST_INDEX(sorted_list_index,sorted_list) ;
  SOC_SAND_SORTED_LIST_VERIFY_SORTEDLIST_IS_ACTIVE(sorted_list_index,4,6) ;

  SOC_SAND_CHECK_NULL_INPUT(key) ;
  SOC_SAND_CHECK_NULL_INPUT(found) ;
  SOC_SAND_CHECK_NULL_INPUT(prev_node) ;
  SOC_SAND_CHECK_NULL_INPUT(cur_node) ;

  res = SORTED_LIST_ACCESS_INFO.cmp_func_type.get(unit,sorted_list_index,&cmp_func_type) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 8, exit);
  if (cmp_func_type == SOC_SAND_SORTED_LIST_CMP_FUNC_TYPE_TCAM)
  {
    cmp_key_fun = soc_sand_sorted_list_tcam_cmp_priority ;
  }
  else
  {
    cmp_key_fun = (SOC_SAND_SORTED_LIST_KEY_CMP)soc_sand_os_memcmp ;
  }
  *found = FALSE ;
  SOC_SAND_SORTED_LIST_TAIL_PLACE_GET(sorted_list_index,prev,12) ;

  res = soc_sand_sorted_list_get_tmp_key_ptr_from_handle(unit,sorted_list,&tmp_key_ptr) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 16, exit);
  res = SORTED_LIST_ACCESS_INFO.key_size.get(unit,sorted_list_index,&key_size) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 18, exit) ;
  do
  {
    /*
     * Read next entry. (if inverted_order - read previous)
     */
    res = soc_sand_sorted_list_get_next_aux(
            unit,
            sorted_list,
            prev,
            FALSE, /* Scan the list from the end - faster in TCAM for sorted entries */
            tmp_key_ptr,
            NULL,&curr
          );
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit) ;
    if (curr == SOC_SAND_SORTED_LIST_ITER_BEGIN(unit,sorted_list))
    {
      *found = FALSE;
      *prev_node = SOC_SAND_SORTED_LIST_ITER_BEGIN(unit,sorted_list);
      *cur_node = prev;
      goto exit;
    }
    compare_res =
      cmp_key_fun(
        tmp_key_ptr,
        key,
        (key_size * sizeof(SOC_SAND_SORTED_LIST_KEY_TYPE))
      ) ;
    /*
     * If key was found
     */
    if (compare_res == 0)
    {
      if (first_match)
      {
          *found = TRUE;
          prev = curr;
          res = soc_sand_sorted_list_get_next_aux(
                 unit,
                 sorted_list,
                 prev,
                 FALSE,
                 tmp_key_ptr,
                 NULL,&curr
               );
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit) ;
          *prev_node = curr;
          *cur_node =  prev;
          goto exit;
      }
      if (data != NULL)
      {
        res =
          soc_sand_sorted_list_data_cmp(
            unit,
            sorted_list,
            curr,
            data, &compare_res
          );
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit) ;
        if (compare_res)
        {
          *found = TRUE;
          prev = curr;
          curr = soc_sand_sorted_list_get_next_aux(
                   unit,
                   sorted_list,
                   prev,
                   FALSE,
                   tmp_key_ptr,
                   NULL,&curr
                 );
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 50, exit) ;
          *prev_node = curr;
          *cur_node =  prev;
          goto exit;
        }
      }
    }
    else if (compare_res < 0)
    {
      res = SORTED_LIST_ACCESS_DATA.null_ptr.get(unit,sorted_list_index,&null_ptr) ;
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 8, exit) ;
      *found = FALSE ;
      *prev_node = (curr == SOC_SAND_SORTED_LIST_ITER_END(unit,sorted_list)) ? null_ptr : curr ;
      *cur_node = prev ;
      goto exit ;
    }
    prev = curr ;
  }
  while (!(*found)) ;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_sorted_list_find_match_entry()",0,0);
}

/*********************************************************************
* NAME:
*     soc_sand_sorted_list_node_alloc
* TYPE:
*   PROC
* DATE:
*   May 19 2015
* FUNCTION:
*  Insert an entry into the sorted list, if already exist then
*  the operation returns an error.
* INPUT:
*   SOC_SAND_IN  int                              unit -
*     Identifier of the device to access.
*   SOC_SAND_IN  SOC_SAND_SORTED_LIST_PTR         sorted_list -
*     Handle to the sorted list.
*   SOC_SAND_IN  SOC_SAND_SORTED_LIST_KEY         *key -
*     The key to add into the sorted list
*   SOC_SAND_IN  SOC_SAND_SORTED_LIST_DATA        *data -
*     The data to add into the sorted list and to be associated with
*     the given key
*   SOC_SAND_OUT  uint8                           *success -
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
    SOC_SAND_IN   uint32                    unit,
    SOC_SAND_IN  SOC_SAND_SORTED_LIST_PTR   sorted_list,
    SOC_SAND_IN  SOC_SAND_SORTED_LIST_KEY   *const key,
    SOC_SAND_IN  SOC_SAND_SORTED_LIST_DATA  *const data,
    SOC_SAND_IN   uint32                    prev_node,
    SOC_SAND_IN   uint32                    next_node,
    SOC_SAND_OUT   uint8                    *found
  )
{
  uint32
    new_node_ptr;
  uint32
    sorted_list_index,
    res;
  uint32
    key_size,
    data_size ;
  SOC_SAND_OCC_BM_PTR
    memory_use ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_SORTED_LIST_VERIFY_UNIT_IS_LEGAL(unit, 2) ;
  SOC_SAND_SORTED_LIST_CONVERT_HANDLE_TO_SORTEDLIST_INDEX(sorted_list_index,sorted_list) ;
  SOC_SAND_SORTED_LIST_VERIFY_SORTEDLIST_IS_ACTIVE(sorted_list_index,4,6) ;

  SOC_SAND_CHECK_NULL_INPUT(key) ;
  SOC_SAND_CHECK_NULL_INPUT(data) ;
  SOC_SAND_CHECK_NULL_INPUT(found) ;
  /*
   * Check to see whether the entry exists
   */
  res = SORTED_LIST_ACCESS_DATA.memory_use.get(unit,sorted_list_index,&memory_use) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit) ;
  res =
    soc_sand_occ_bm_alloc_next(
      unit,
      memory_use,
      &new_node_ptr,
      found
    ) ;
  if (*found == FALSE)
  {
    goto exit;
  }
  /* if(SOC_DPP_WB_ENGINE_VAR_NONE == init_info_ptr->wb_var_index) */
  {
    res = SORTED_LIST_ACCESS_INFO.key_size.get(unit,sorted_list_index,&key_size) ;
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 18, exit) ;
    /*
     * Copy 'key_size' bytes from input 'keys' buffer into 'SORTED_LIST_ACCESS_DATA.keys'
     * buffer at offset 'key_size * new_node_ptr'.
     */
    res = SORTED_LIST_ACCESS_DATA.keys.memwrite(unit,sorted_list_index,key,new_node_ptr * key_size, key_size) ;
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 26, exit) ;
  }
  /* if(SOC_DPP_WB_ENGINE_VAR_NONE == init_info_ptr->wb_var_index) */
  {
    res = SORTED_LIST_ACCESS_INFO.data_size.get(unit,sorted_list_index,&data_size) ;
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 8, exit) ;
    /*
     * Copy 'data_size' bytes from input 'data' buffer into 'SORTED_LIST_ACCESS_DATA.data'
     * buffer at offset 'data_size * new_node_ptr'.
     */
    res = SORTED_LIST_ACCESS_DATA.data.memwrite(unit,sorted_list_index,data,new_node_ptr * data_size, data_size) ;
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 26, exit) ;
  }
  soc_sand_sorted_list_node_link_set(
    unit,
    sorted_list,
    prev_node,
    new_node_ptr
  ) ;
  soc_sand_sorted_list_node_link_set(
    unit,
    sorted_list,
    new_node_ptr,
    next_node
  );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_sorted_list_node_alloc()",0,0);

}

/*********************************************************************
* NAME:
*   soc_sand_sorted_list_get_iter_begin_or_end
* TYPE:
*   PROC
* DATE:
*   May 19 2015
* FUNCTION:
*   Get the first iterator of the sorted list.
* INPUT:
*   SOC_SAND_IN  int                              unit -
*     Identifier of the device to access.
*   SOC_SAND_IN  SOC_SAND_SORTED_LIST_PTR         sorted_list -
*     Handle to the sorted list.
*   SOC_SAND_IN  int                              get_begin -
*     If non-zero then get iterator at the beginning of the list. Otherwise
*     get iterator at end of list
* REMARKS:
*     None.
* RETURNS:
*   First iterator. If an error is encountered, a negative value is returned.
*********************************************************************/
uint32
  soc_sand_sorted_list_get_iter_begin_or_end(
    SOC_SAND_IN   int                           unit,
    SOC_SAND_IN   SOC_SAND_SORTED_LIST_PTR      sorted_list,
    SOC_SAND_IN   int                           get_begin
  )
{
  uint32
    res;
  uint32
    sorted_list_index ;
  uint32
    list_size ;

  list_size = (uint32)(-1) ;
  if ((unit < 0) || (unit >= SOC_MAX_NUM_DEVICES))
  { 
    /*
     * If this is an illegal unit identifier, quit
     * with error.
     */
    goto exit ;
  }
  SOC_SAND_SORTED_LIST_CONVERT_HANDLE_TO_SORTEDLIST_INDEX(sorted_list_index,sorted_list) ;
  {
    uint8 bit_val ;
    uint32 max_nof_lists ;
    res = SORTED_LIST_ACCESS.max_nof_lists.get(unit, &max_nof_lists) ;
    if (res != SOC_E_NONE)
    {
      FUNC_RESULT_SOC_PRINT(res) ;
      goto exit ;
    }
    if (sorted_list_index >= max_nof_lists)
    {
      /*
       * If sortedlist handle is out of range then quit with error.
       */
      bit_val = 0 ;
    }
    else
    {
      res = SORTED_LIST_ACCESS.occupied_lists.bit_get(unit, (int)sorted_list_index, &bit_val) ;
      if (res != SOC_E_NONE)
      {
        FUNC_RESULT_SOC_PRINT(res) ;
        goto exit ;
      }
    }
    if (bit_val == 0)
    {
      /*
       * If sortedlist structure is not indicated as 'occupied' then quit
       * with error.
       */
      goto exit ;
    }
  }
  res = SORTED_LIST_ACCESS_INFO.list_size.get(unit,sorted_list_index,&list_size) ;
  if (res != SOC_E_NONE)
  {
    FUNC_RESULT_SOC_PRINT(res) ;
    goto exit ;
  }
  if (get_begin == 0)
  {
    list_size += 1 ;
  }
exit:
  return (list_size) ;
}

#include <soc/dpp/SAND/Utils/sand_footer.h>
