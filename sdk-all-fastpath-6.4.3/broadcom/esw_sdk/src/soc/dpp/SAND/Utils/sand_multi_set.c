/* $Id: sand_multi_set.c,v 1.15 Broadcom SDK $
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
/******************************************************************
* FILENAME:       soc_sand_multi_set.c
*
* FILE DESCRIPTION:
*   The bit stream modules
* REMARKS:
* SW License Agreement: Dune Networks (c). CONFIDENTIAL PROPRIETARY INFORMATION.
* Any use of this Software is subject to Software License Agreement
* included in the Driver User Manual of this device.
* Any use of this Software constitutes an agreement to the terms
* of the above Software License Agreement.
******************************************************************/

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

#include <soc/dpp/SAND/Utils/sand_multi_set.h>
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
#define SOC_SAND_MULTI_SET_HASH_WIDTH_FACTOR 1

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

static uint32
  soc_sand_multi_set_member_add_internal(
    SOC_SAND_INOUT  SOC_SAND_MULTI_SET_INFO    *multi_set,
    SOC_SAND_IN     SOC_SAND_MULTI_SET_KEY*       const key,
    SOC_SAND_INOUT  uint32                  *data_indx,
    SOC_SAND_IN     uint32                  nof_additions,
    SOC_SAND_OUT    uint8                *first_appear,
    SOC_SAND_OUT    uint8                *success
  );

/*********************************************************************
* NAME:
*     soc_sand_multi_set_create
* TYPE:
*   PROC
* DATE:
*   Mar  6 2008
* FUNCTION:
*     Creates a new Multi set instance.
* INPUT:
*   SOC_SAND_INOUT  SOC_SAND_MULTI_SET_INFO     *multi_set -
*     information to use in order to create the mutli-set (size, duplications...)
* REMARKS:
*   None.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_sand_multi_set_create(
    SOC_SAND_INOUT  SOC_SAND_MULTI_SET_INFO     *multi_set
  )
{
  SOC_SAND_MULTI_SET_INIT_INFO
    *set_init;
  SOC_SAND_MULTI_SET_T
    *set_data;
  SOC_SAND_HASH_TABLE_INIT_INFO
    *hash_init;
  uint32
    res;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(SOC_SAND_HASH_TABLE_CREATE);

  SOC_SAND_CHECK_NULL_INPUT(multi_set);
  SOC_SAND_CHECK_NULL_INPUT(multi_set->init_info.get_entry_fun);
  SOC_SAND_CHECK_NULL_INPUT(multi_set->init_info.set_entry_fun);

  set_init = &(multi_set->init_info);
  set_data = &(multi_set->multiset_data);

  if (set_init->nof_members == 0 || set_init->member_size == 0)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_VALUE_OUT_OF_RANGE_ERR, 10, exit);
  }
  if (set_init->max_duplications == SOC_SAND_U32_MAX)
  {
    set_init->max_duplications = SOC_SAND_U32_MAX - 1;
  }

 /*
  * creat mutli-set.
  */
  soc_sand_SAND_HASH_TABLE_INFO_clear(&(set_data->hash_table));
  hash_init = &(set_data->hash_table.init_info);

  hash_init->data_size = set_init->member_size;
  hash_init->key_size = set_init->member_size;
  hash_init->prime_handle = set_init->prime_handle;
  hash_init->sec_handle = set_init->sec_handle;
  hash_init->table_size = set_init->nof_members;
  hash_init->table_width = set_init->nof_members * SOC_SAND_MULTI_SET_HASH_WIDTH_FACTOR;
  hash_init->get_entry_fun = set_init->get_entry_fun;
  hash_init->set_entry_fun = set_init->set_entry_fun;
  if(SOC_DPP_WB_ENGINE_VAR_NONE != set_init->wb_var_index)
  {
      hash_init->wb_var_index = set_init->wb_var_index + WB_ENGINE_MULTI_SET_HASH_TABLE;      
  }

  /* Calculate global max */
  if (set_init->global_max == 0) {
      set_init->global_max = (set_init->max_duplications == 1) ? set_init->nof_members : set_init->max_duplications;
  }
  
  set_data->global_counter = 0;

  res = soc_sand_hash_table_create(
            &(set_data->hash_table)
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

 /*
  * if there is need to manage the duplications, then allocate array to
  * manage the reference counter.
  */
  if (set_init->max_duplications > 1)
  {
   /*
    * calculate the size of pointers (list head and next) according to table size.
    */
    set_data->counter_size = (soc_sand_log2_round_up(set_init->max_duplications+1) + (SOC_SAND_NOF_BITS_IN_BYTE - 1)) / SOC_SAND_NOF_BITS_IN_BYTE;

    set_data->ref_counter = (uint8*)soc_sand_os_malloc_any_size(set_init->nof_members * set_data->counter_size * sizeof(uint8),"set_data->ref_counter");
    if (!set_data->ref_counter)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_MALLOC_FAIL, 30, exit);
    }
    soc_sand_os_memset(
      set_data->ref_counter,
      0x0,
      set_init->nof_members * set_data->counter_size * sizeof(uint8)
      );
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_multi_set_create()",0,0);
}

/*********************************************************************
* NAME:
*     soc_sand_multi_set_destroy
* TYPE:
*   PROC
* DATE:
*   Mar  6 2008
* FUNCTION:
*     free the mutli-set instance.
* INPUT:
*  SOC_SAND_OUT  SOC_SAND_MULTI_SET_INFO   *multi_set -
*     The mutli-set to destroy.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_sand_multi_set_destroy(
    SOC_SAND_INOUT  SOC_SAND_MULTI_SET_INFO  *multi_set
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(SOC_SAND_MULTI_SET_DESTROY);
  SOC_SAND_CHECK_NULL_INPUT(multi_set);

  res = soc_sand_hash_table_destroy(
          &(multi_set->multiset_data.hash_table)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

 /*
  * if there is need to manage the duplications, then allocate array to
  * manage the reference counter.
  */
  if (multi_set->init_info.max_duplications > 1)
  {
    soc_sand_os_free_any_size(multi_set->multiset_data.ref_counter);
  }

  multi_set->multiset_data.global_counter = 0;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_multi_set_destroy()",0,0);
}

/*********************************************************************
* NAME:
*     soc_sand_multi_set_member_add
* TYPE:
*   PROC
* DATE:
*   Mar  6 2008
* FUNCTION:
*  Add a member to the mutli-set, if already exist then
*  then update the occurences of this member.
* INPUT:
*   SOC_SAND_INOUT  SOC_SAND_MULTI_SET_INFO   *multi_set -
*     The multi-set to add a member to.
*   SOC_SAND_IN  SOC_SAND_MULTI_SET_KEY  key -
*     The member to add.
*   SOC_SAND_OUT  uint32                 *data_indx -
*     Index identifies the place of the added member.
*     the given key
*   SOC_SAND_OUT  uint8                 *success -
*     whether the add operation success, this may be FALSE,
*     if there are no more space available in the multi-set
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_sand_multi_set_member_add(
    SOC_SAND_INOUT  SOC_SAND_MULTI_SET_INFO    *multi_set,
    SOC_SAND_IN  SOC_SAND_MULTI_SET_KEY*        const key,
    SOC_SAND_OUT  uint32                 *data_indx,
    SOC_SAND_OUT  uint8                *first_appear,
    SOC_SAND_OUT  uint8                *success
  )
{
  int nof_additions = 1;
  uint32 res;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(SOC_SAND_MULTI_SET_MEMBER_ADD);

    *data_indx = SOC_SAND_U32_MAX;  /* In this function, the data index wasn't given. Set to SOC_SAND_U32_MAX to indicate. */
    res = soc_sand_multi_set_member_add_internal(
                multi_set,
                key,
                data_indx,
                nof_additions,
                first_appear,
                success);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_multi_set_member_add()",0,0);
}


/*********************************************************************
* NAME:
*     soc_sand_multi_set_member_add_at_index
* TYPE:
*   PROC
* DATE:
*   Mar  6 2008
* FUNCTION:
*  Add one member to the mutli-set to a specific index, if already exist then
*  then update the occurences of this member.
* INPUT:
*   SOC_SAND_INOUT  SOC_SAND_MULTI_SET_INFO   *multi_set -
*     The multi-set to add a member to.
*   SOC_SAND_IN  SOC_SAND_MULTI_SET_KEY  key -
*     The member to add.
*   SOC_SAND_OUT  uint32                 *data_indx -
*     Index identifies the place of the added member.
*     The function assumes that it already exists. 
*   SOC_SAND_OUT  uint8                 *success -
*     whether the add operation success, this may be FALSE,
*     if there are no more space available in the multi-set
* Remarks: 
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_sand_multi_set_member_add_at_index(
    SOC_SAND_INOUT  SOC_SAND_MULTI_SET_INFO    *multi_set,
    SOC_SAND_IN  SOC_SAND_MULTI_SET_KEY*       const key,
    SOC_SAND_IN  uint32                  data_indx,
    SOC_SAND_OUT  uint8                *first_appear,
    SOC_SAND_OUT  uint8                *success
  )
{
    uint32 res;
    const int nof_additions = 1;
    SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(SOC_SAND_MULTI_SET_MEMBER_ADD);

    res = soc_sand_multi_set_member_add_at_index_nof_additions(
                multi_set,
                key,
                data_indx,
                nof_additions,
                first_appear,
                success);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_multi_set_member_add()",0,0);
}


/*********************************************************************
* NAME:
*     soc_sand_multi_set_member_add_at_index_nof_additions
* TYPE:
*   PROC
* DATE:
*   Mar  6 2008
* FUNCTION:
*  Add nof members to the mutli-set to a specific index, if already exist then
*  then update the occurences of this member.
* INPUT:
*   SOC_SAND_INOUT  SOC_SAND_MULTI_SET_INFO   *multi_set -
*     The multi-set to add a member to.
*   SOC_SAND_IN  SOC_SAND_MULTI_SET_KEY  key -
*     The member to add.
*   SOC_SAND_OUT  uint32                 *data_indx -
*     Index identifies the place of the added member.
*     The function assumes that it already exists. 
*   SOC_SAND_IN  uint32                 nof_additions -
*     Declare nof_additions to add the given key.
*     If given SOC_SAND_U32_MAX, the maximum number of entries will be added.
*   SOC_SAND_OUT  uint8                 *success -
*     whether the add operation success, this may be FALSE,
*     if there are no more space available in the multi-set
* Remarks: 
*     Similair to  soc_sand_multi_set_member_add_at_index,
*     where nof_additions can be more than 1
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_sand_multi_set_member_add_at_index_nof_additions(
    SOC_SAND_INOUT  SOC_SAND_MULTI_SET_INFO    *multi_set,
    SOC_SAND_IN  SOC_SAND_MULTI_SET_KEY*       const key,
    SOC_SAND_IN  uint32                  data_indx,
    SOC_SAND_IN  uint32                  nof_additions,
    SOC_SAND_OUT  uint8                *first_appear,
    SOC_SAND_OUT  uint8                *success
  )
{
    uint32 res;
    uint32 data_index_ptr = data_indx;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(SOC_SAND_MULTI_SET_MEMBER_ADD);
  res = soc_sand_multi_set_member_add_internal(
              multi_set,
              key,
              &data_index_ptr,
              nof_additions,
              first_appear,
              success);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_multi_set_member_add_at_index_ref_count()",0,0);
}


/*********************************************************************
* NAME:
*     soc_sand_multi_set_member_add_internal
* TYPE:
*   PROC
* DATE:
*   Mar  6 2008
* FUNCTION:
*  Add nof members to the mutli-set to either a specific index or a new index. If already exist then
*  then update the occurences of this member.
* INPUT:
*   SOC_SAND_INOUT  SOC_SAND_MULTI_SET_INFO   *multi_set -
*     The multi-set to add a member to.
*   SOC_SAND_IN  SOC_SAND_MULTI_SET_KEY  key -
*     The member to add.
*   SOC_SAND_OUT  uint32                 *data_indx -
*     Index identifies the place of the added member.
*     If it's a new object, should be -1.
*   SOC_SAND_IN  uint32                 nof_additions -
*     Declare nof_additions to add the given key.
*     If given SOC_SAND_U32_MAX, the maximum number of entries will be added.
*   SOC_SAND_OUT  uint8                 *success -
*     whether the add operation success, this may be FALSE,
*     if there are no more space available in the multi-set
* Remarks: 
*     Similair to  soc_sand_multi_set_member_add_at_index,
*     where nof_additions can be more than 1
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_sand_multi_set_member_add_internal(
    SOC_SAND_INOUT  SOC_SAND_MULTI_SET_INFO    *multi_set,
    SOC_SAND_IN     SOC_SAND_MULTI_SET_KEY*       const key,
    SOC_SAND_INOUT  uint32                  *data_indx,
    SOC_SAND_IN     uint32                  nof_additions,
    SOC_SAND_OUT    uint8                *first_appear,
    SOC_SAND_OUT    uint8                *success
  )
{
  SOC_SAND_MULTI_SET_INIT_INFO
    *set_init;
  SOC_SAND_MULTI_SET_T
    *set_data;
  uint8
    tmp_cnt[sizeof(uint32)];
  uint32
    ref_count,
    found_index, 
    adjusted_additions;
  uint8
    exist,
    with_id = (*data_indx != SOC_SAND_U32_MAX);
  uint32
    res;
  int unit = BSL_UNIT_UNKNOWN;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_MULTI_SET_MEMBER_ADD);

  SOC_SAND_CHECK_NULL_INPUT(multi_set);
  SOC_SAND_CHECK_NULL_INPUT(key);
  SOC_SAND_CHECK_NULL_INPUT(data_indx);
  SOC_SAND_CHECK_NULL_INPUT(success);

  if (nof_additions == 0)
  {
    *success = FALSE;
    goto exit;
  }

  set_data = &(multi_set->multiset_data);
  set_init = &(multi_set->init_info);


  /* In case of singlton set, it's enough to just check if the entry exists. */
  if (set_init->max_duplications <= 1)
  {
    res = soc_sand_hash_table_entry_lookup(
            &(set_data->hash_table),
            key,
            &found_index,
            &exist
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    if (with_id) {
        /* if allocating by index and entry already exist in a different index, return error. */ 
        if (exist && found_index != *data_indx)
        {
          *first_appear = FALSE;
          *success = FALSE;
          goto exit;
        }
    } else {
        /* If allocating for the first time, keep the index. */
        *data_indx = found_index;
    }


    if (exist && found_index != SOC_SAND_HASH_TABLE_NULL)
    {
      *first_appear = FALSE;
      *success = TRUE;
      goto exit;
    }
  }

  /* If given index, add at index. Otherwise, add without index. */
  if (with_id) {
      found_index = *data_indx;

      res = soc_sand_hash_table_entry_add_at_index(
              &(set_data->hash_table),
              key,
              found_index,
              success
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  } else {
      res = soc_sand_hash_table_entry_add(
              &(set_data->hash_table),
              key,
              &found_index,
              success
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }

  /* If it's a singleton, and we didn't find it in the lookup, then it appears first. */
  if (set_init->max_duplications <= 1)
  {
    *first_appear = TRUE;
    goto exit;
  }


  if(!*success || found_index == SOC_SAND_HASH_TABLE_NULL)
  {
      *success = FALSE;
      *first_appear = (with_id) ? FALSE : TRUE; /* If we were given the index, we assume the user allocated the entry before the call. 
                                                            If we weren't given the index, we assume the operation failed because the table is 
                                                            full. */
      goto exit;
  }
  
  /* Save the index. */
  *data_indx = found_index;

  res = set_init->get_entry_fun(
          set_init->prime_handle,
          set_init->sec_handle,
          set_data->ref_counter,
          *data_indx,
          set_data->counter_size,
          tmp_cnt
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  ref_count = 0;
  res = soc_sand_U8_to_U32(
          tmp_cnt,
          set_data->counter_size,
          &ref_count
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  /* Set if maximum entries is required. */
  if (nof_additions == SOC_SAND_U32_MAX) {
      adjusted_additions  = (set_init->max_duplications - ref_count);
  } else {
      adjusted_additions = nof_additions;
  }

  /*
   * Same value referenced more than initialized ed value times, return
   * operation fail.
   */
  if ((ref_count + adjusted_additions) > SOC_SAND_BITS_MASK((SOC_SAND_NOF_BITS_IN_BYTE * set_data->counter_size - 1),0))
  {
    /* Overflowing the counter. */
    *success = FALSE;
    goto exit;
  }
  if ((ref_count + adjusted_additions) > set_init->max_duplications) {    /* Adding more entries than there are available */
      *success = FALSE;
      goto exit;
  }
  if ((set_data->global_counter + adjusted_additions) > set_init->global_max) { /* Adding more entries than the global counter allows. */
  }
  if (ref_count == 0)
  {
    *first_appear = TRUE;
  }
  else
  {
    *first_appear = FALSE;
  }
  ref_count+=adjusted_additions;

  /* Update global counter. */
  set_data->global_counter += adjusted_additions;

  res = soc_sand_U32_to_U8(
          &ref_count,
          set_data->counter_size,
          tmp_cnt
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  if(SOC_DPP_WB_ENGINE_VAR_NONE == set_init->wb_var_index)
  {
      res = set_init->set_entry_fun(
                                    set_init->prime_handle,
                                    set_init->sec_handle,
                                    set_data->ref_counter,
                                    *data_indx,
                                    set_data->counter_size,
                                    tmp_cnt
                                    );
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  }
  else
  {
      unit = (set_init->prime_handle);

      res = SOC_DPP_WB_ENGINE_SET_ARR(unit, set_init->wb_var_index + WB_ENGINE_MULTI_SET_NEXT_REF_COUNTER, 
                                      tmp_cnt, 
                                      *data_indx);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 60, exit);      
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_multi_set_member_add_at_index_ref_count()",0,0);
}


/*********************************************************************
* NAME:
*     soc_sand_multi_set_member_remove
* TYPE:
*   PROC
* DATE:
*   Mar  6 2008
* FUNCTION:
*  Remove a member from a mutli-set, if the member is not belong to the multi-set
*  the operation has no effect.
* INPUT:
*  SOC_SAND_INOUT  SOC_SAND_MULTI_SET_INFO   *multi_set -
*     The mutli-set instance.
*  SOC_SAND_IN  SOC_SAND_MULTI_SET_KEY  key -
*     The member to remove from the mutli-set
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_sand_multi_set_member_remove(
    SOC_SAND_INOUT  SOC_SAND_MULTI_SET_INFO      *multi_set,
    SOC_SAND_IN   SOC_SAND_MULTI_SET_KEY*        const key,
    SOC_SAND_OUT  uint32                   *data_indx,
    SOC_SAND_OUT  uint8                  *last_appear
  )
{
  SOC_SAND_MULTI_SET_T
    *set_data;
  uint8
    found;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(SOC_SAND_MULTI_SET_MEMBER_REMOVE);

  SOC_SAND_CHECK_NULL_INPUT(multi_set);
  SOC_SAND_CHECK_NULL_INPUT(key);

  set_data = &(multi_set->multiset_data);


  res = soc_sand_hash_table_entry_lookup(
          &(set_data->hash_table),
          key,
          data_indx,
          &found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (!found)
  {
    *data_indx = SOC_SAND_MULTI_SET_NULL;
    *last_appear = FALSE;
    goto exit;
  }

  res = soc_sand_multi_set_member_remove_by_index_multiple(multi_set, *data_indx, 1, last_appear);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_multi_set_member_remove()",0,0);
}

/*********************************************************************
* NAME:
*     soc_sand_multi_set_member_remove_by_index
* TYPE:
*   PROC
* DATE:
*   Mar  6 2008
* FUNCTION:
*  Remove a member from a mutli-set, if the member is not belong to the multi-set
*  the operation has no effect.
* INPUT:
*  SOC_SAND_INOUT  SOC_SAND_MULTI_SET_INFO   *multi_set -
*     The mutli-set instance.
*  SOC_SAND_IN  uint32                      data_indx - 
*       The index of the entry to be deleted.
*  SOC_SAND_OUT  uint8                  *last_appear -
*       Will be set if it's the last appearance of the entry. 
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_sand_multi_set_member_remove_by_index(
    SOC_SAND_INOUT  SOC_SAND_MULTI_SET_INFO      *multi_set,
    SOC_SAND_IN  uint32                    data_indx,
    SOC_SAND_OUT  uint8                  *last_appear
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(SOC_SAND_MULTI_SET_MEMBER_REMOVE_BY_INDEX);
  res = soc_sand_multi_set_member_remove_by_index_multiple(multi_set, data_indx, 1, last_appear);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_multi_set_member_remove_by_index()",0,0);
}

/*********************************************************************
* NAME:
*     soc_sand_multi_set_member_remove_by_index_multiple
* TYPE:
*   PROC
* DATE:
*   Mar  6 2008
* FUNCTION:
*  Remove a member from a mutli-set, if the member is not belong to the multi-set
*  the operation has no effect.
* INPUT:
*  SOC_SAND_INOUT  SOC_SAND_MULTI_SET_INFO   *multi_set -
*     The mutli-set instance.
*  SOC_SAND_IN  uint32                      data_indx - 
*       The index of the entry to be deleted.
*  SOC_SAND_IN  uint32                    remove_amount -
*       How many references of the entry to be removed. 
*  SOC_SAND_OUT  uint8                  *last_appear -
*       Will be set if it's the last appearance of the entry. 
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_sand_multi_set_member_remove_by_index_multiple(
    SOC_SAND_INOUT  SOC_SAND_MULTI_SET_INFO      *multi_set,
    SOC_SAND_IN  uint32                    data_indx,
    SOC_SAND_IN  uint32                    remove_amount,
    SOC_SAND_OUT  uint8                  *last_appear
  )
{
  SOC_SAND_MULTI_SET_INIT_INFO
    *set_init;
  SOC_SAND_MULTI_SET_T
    *set_data;
  uint32
    ref_count,
    adjusted_remove_amount;
  uint8
    tmp_cnt[sizeof(uint32)];
  uint32
    res;
  int unit = BSL_UNIT_UNKNOWN;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_MULTI_SET_MEMBER_REMOVE_BY_INDEX);

  SOC_SAND_CHECK_NULL_INPUT(multi_set);
  SOC_SAND_CHECK_NULL_INPUT(last_appear);

  set_data = &(multi_set->multiset_data);
  set_init = &(multi_set->init_info);

  ref_count = 0;

  if (set_init->max_duplications > 1)
  {
    res = set_init->get_entry_fun(
            set_init->prime_handle,
            set_init->sec_handle,
            set_data->ref_counter,
            data_indx,
            set_data->counter_size,
            tmp_cnt
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    soc_sand_U8_to_U32(
      tmp_cnt,
      set_data->counter_size,
      &ref_count
    );

    if (ref_count == 0)
    {
      *last_appear = TRUE;
      goto exit;
    }

    /* Remove all entries */
    if ((remove_amount > ref_count)) {
        adjusted_remove_amount = ref_count;
    } else {
        adjusted_remove_amount = remove_amount;
    }

    set_data->global_counter -= adjusted_remove_amount;
    ref_count -= adjusted_remove_amount;
    *last_appear = (ref_count == 0) ? TRUE : FALSE;
    
    res = soc_sand_U32_to_U8(
            &ref_count,
            set_data->counter_size,
            tmp_cnt
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    if(SOC_DPP_WB_ENGINE_VAR_NONE == set_init->wb_var_index)
    {
        res = set_init->set_entry_fun(
                                      set_init->prime_handle,
                                      set_init->sec_handle,
                                      set_data->ref_counter,
                                      data_indx,
                                      set_data->counter_size,
                                      tmp_cnt
                                      );
        SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    }
    else
    {
        unit = (set_init->prime_handle);

        res = SOC_DPP_WB_ENGINE_SET_ARR(unit, set_init->wb_var_index + WB_ENGINE_MULTI_SET_NEXT_REF_COUNTER, 
                                        tmp_cnt, 
                                        data_indx);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 60, exit);      
    }

  }
  if (*last_appear || set_init->max_duplications <= 1)
  {
    res = soc_sand_hash_table_entry_remove_by_index(
            &(set_data->hash_table),
            data_indx
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_multi_set_member_remove_by_index()",0,0);
}

/*********************************************************************
* NAME:
*     soc_sand_multi_set_member_lookup
* TYPE:
*   PROC
* DATE:
*   Mar  6 2008
* FUNCTION:
*  lookup in the mutli-set for a member and return the occurrences/duplications of this
*  member in the multi-set, and the index identifying this member place.
*  the given key.
* INPUT:
*  SOC_SAND_IN  SOC_SAND_MULTI_SET_INFO   *        multi_set -
*     The mutli-set.
*   SOC_SAND_IN  SOC_SAND_MULTI_SET_KEY   key -
*     The member to lookup
*   SOC_SAND_OUT  uint32            *data_indx -
*     index identifying this member place.
*   SOC_SAND_OUT  uint32                  *ref_count -
*     the occurrences/duplications of this member in the multi-set
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_sand_multi_set_member_lookup(
    SOC_SAND_INOUT  SOC_SAND_MULTI_SET_INFO     *multi_set,
    SOC_SAND_IN  SOC_SAND_MULTI_SET_KEY*        const key,
    SOC_SAND_OUT  uint32                  *data_indx,
    SOC_SAND_OUT  uint32                  *ref_count
  )
{
  SOC_SAND_MULTI_SET_INIT_INFO
    *set_init;
  SOC_SAND_MULTI_SET_T
    *set_data;
  uint8
    tmp_cnt[sizeof(uint32)];
  uint8
    found;
  uint32
    ref_count_lcl[1];
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(SOC_SAND_MULTI_SET_MEMBER_LOOKUP);

  SOC_SAND_CHECK_NULL_INPUT(multi_set);
  SOC_SAND_CHECK_NULL_INPUT(key);
  SOC_SAND_CHECK_NULL_INPUT(data_indx);

  set_data = &(multi_set->multiset_data);
  set_init = &(multi_set->init_info);

  res = soc_sand_hash_table_entry_lookup(
          &(set_data->hash_table),
          key,
          data_indx,
          &found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (!found)
  {
    *ref_count = 0;
    goto exit;
  }

  if (set_init->max_duplications > 1)
  {
    *ref_count_lcl = 0;

    res = set_init->get_entry_fun(
            set_init->prime_handle,
            set_init->sec_handle,
            set_data->ref_counter,
            *data_indx,
            set_data->counter_size,
            tmp_cnt
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    res = soc_sand_U8_to_U32(
            tmp_cnt,
            set_data->counter_size,
            ref_count_lcl
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    *ref_count = *ref_count_lcl;
  }
  else
  {
    *ref_count = 1;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_multi_set_member_lookup()",0,0);
}

/*********************************************************************
* NAME:
*     soc_sand_multi_set_get_next
* TYPE:
*   PROC
* DATE:
*   Mar  6 2008
* FUNCTION:
*  get the next valid entry (key and data) in the multiset.
*  start traversing from the place pointed by the given iterator.
* INPUT:
*  SOC_SAND_IN  SOC_SAND_MULTI_SET_PTR        multi_set -
*     The multiset.
*  SOC_SAND_INOUT  SOC_SAND_MULTI_SET_ITER    *iter
*     iterator points to the entry to start traverse from.
*   SOC_SAND_OUT  SOC_SAND_MULTI_SET_KEY*   const key -
*     the multiset key returned
*   SOC_SAND_OUT  SOC_SAND_MULTI_SET_DATA  data -
*     the multiset data returned and associated with the key above.
* REMARKS:
*     - to start traverse the multiset from the beginning.
*       use SOC_SAND_MULTI_SET_ITER_BEGIN(iter)
*     - to check if the iterator get to the end of the table use.
*       use SOC_SAND_MULTI_SET_ITER_END(iter)
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_sand_multi_set_get_next(
    SOC_SAND_IN  SOC_SAND_MULTI_SET_INFO        *multi_set,
    SOC_SAND_INOUT  SOC_SAND_MULTI_SET_ITER    *iter,
    SOC_SAND_OUT  SOC_SAND_MULTI_SET_KEY        *key,
    SOC_SAND_OUT  uint32                   *data_indx,
    SOC_SAND_OUT  uint32                   *ref_count
  )
{
  SOC_SAND_IN SOC_SAND_MULTI_SET_INIT_INFO
    *set_init;
  SOC_SAND_IN SOC_SAND_MULTI_SET_T
    *set_data;
  uint32
    tmp_ref_count;
  uint8
    tmp_cnt[sizeof(uint32)];
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(SOC_SAND_MULTI_SET_MEMBER_LOOKUP);

  SOC_SAND_CHECK_NULL_INPUT(multi_set);

  set_data = &(multi_set->multiset_data);
  set_init = &(multi_set->init_info);
 /*
  * traverse the mutli-set and print
  */
  if (SOC_SAND_HASH_TABLE_ITER_IS_END(iter))
  {
    goto exit;
  }
  res = soc_sand_hash_table_get_next(
          &(set_data->hash_table),
          iter,
          key,
          data_indx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  if (SOC_SAND_HASH_TABLE_ITER_IS_END(iter))
  {
    goto exit;
  }
  tmp_ref_count = 1;
  if (set_init->max_duplications > 1)
  {
    tmp_ref_count = 0;
    res = set_init->get_entry_fun(
            set_init->prime_handle,
            set_init->sec_handle,
            set_data->ref_counter,
            *data_indx,
            set_data->counter_size,
            tmp_cnt
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    res = soc_sand_U8_to_U32(
           tmp_cnt,
           set_data->counter_size,
           &tmp_ref_count
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }
  *ref_count = tmp_ref_count;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_multi_set_get_next()",0,0);
}


/*********************************************************************
* NAME:
*     soc_sand_multi_set_get_by_index
* TYPE:
*   PROC
* DATE:
*   Mar  6 2008
* FUNCTION:
*  get the next valid entry (key and data) in the multiset.
*  start traversing from the place pointed by the given iterator.
* INPUT:
*  SOC_SAND_IN  SOC_SAND_MULTI_SET_PTR        multi_set -
*     The multiset.
*  SOC_SAND_IN  uint32                    data_indx -
*     where the key was found.
*   SOC_SAND_OUT  SOC_SAND_MULTI_SET_KEY*   key -
*     the multiset key returned
*   SOC_SAND_OUT  SOC_SAND_MULTI_SET_DATA  data -
*     the multiset data returned and associated with the key above.
* REMARKS:
*     - to start traverse the multiset from the beginning.
*       use SOC_SAND_MULTI_SET_ITER_BEGIN(iter)
*     - to check if the iterator get to the end of the table use.
*       use SOC_SAND_MULTI_SET_ITER_END(iter)
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_sand_multi_set_get_by_index(
    SOC_SAND_IN  SOC_SAND_MULTI_SET_INFO        *multi_set,
    SOC_SAND_IN  uint32                   data_indx,
    SOC_SAND_OUT  SOC_SAND_MULTI_SET_KEY        *key,
    SOC_SAND_OUT  uint32                  *ref_count
  )
{
  SOC_SAND_IN SOC_SAND_MULTI_SET_INIT_INFO
    *set_init;
  SOC_SAND_IN SOC_SAND_MULTI_SET_T
    *set_data;
  uint32
    tmp_ref_count;
  uint8
    found;
  uint8
    tmp_cnt[sizeof(uint32)];
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(SOC_SAND_MULTI_SET_MEMBER_LOOKUP);

  SOC_SAND_CHECK_NULL_INPUT(multi_set);

  set_data = &(multi_set->multiset_data);
  set_init = &(multi_set->init_info);

  res = soc_sand_hash_table_get_by_index(
          &(set_data->hash_table),
          data_indx,
          key,
          &found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  if (!found)
  {
    *ref_count = 0;
    goto exit;
  }
  tmp_ref_count = 1;
  if (set_init->max_duplications > 1)
  {
    tmp_ref_count = 0;
    res = set_init->get_entry_fun(
            set_init->prime_handle,
            set_init->sec_handle,
            set_data->ref_counter,
            data_indx,
            set_data->counter_size,
            tmp_cnt
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    res = soc_sand_U8_to_U32(
           tmp_cnt,
           set_data->counter_size,
           &tmp_ref_count
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }
  *ref_count = tmp_ref_count;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_multi_set_get_next()",0,0);
}

uint32
  soc_sand_multi_set_clear(
    SOC_SAND_INOUT  SOC_SAND_MULTI_SET_INFO     *multi_set
  )
{
  SOC_SAND_MULTI_SET_INIT_INFO
    *set_init;
  SOC_SAND_MULTI_SET_T
    *set_data;
  uint32
    res;
  int unit = BSL_UNIT_UNKNOWN;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_HASH_TABLE_CREATE);

  SOC_SAND_CHECK_NULL_INPUT(multi_set);
  SOC_SAND_CHECK_NULL_INPUT(multi_set->init_info.get_entry_fun);
  SOC_SAND_CHECK_NULL_INPUT(multi_set->init_info.set_entry_fun);

  set_init = &(multi_set->init_info);
  set_data = &(multi_set->multiset_data);


 /*
  * clear mutli-set.
  */

  res = soc_sand_hash_table_clear(
            &(set_data->hash_table)
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

 /*
  * if there is need to manage the duplications, then clear array to
  * manage the reference counter.
  */
  if (set_init->max_duplications > 1)
  {
      if(SOC_DPP_WB_ENGINE_VAR_NONE == set_init->wb_var_index)
      {
          /*
           * calculate the size of pointers (list head and next) according to table size.
           */
          set_data->counter_size = (soc_sand_log2_round_up(set_init->max_duplications+1) + (SOC_SAND_NOF_BITS_IN_BYTE - 1)) / SOC_SAND_NOF_BITS_IN_BYTE;

          soc_sand_os_memset(
                             set_data->ref_counter,
                             0x0,
                             set_init->nof_members * set_data->counter_size * sizeof(uint8)
                             );
      }
      else
      {
          unit = (set_init->prime_handle);

          res = SOC_DPP_WB_ENGINE_MEMSET_ARR(unit, set_init->wb_var_index + WB_ENGINE_MULTI_SET_NEXT_REF_COUNTER, 0x0);
          SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);      
      }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_multi_set_clear()",0,0);
}

void
soc_sand_SAND_MULTI_SET_INFO_clear(
                                   SOC_SAND_OUT SOC_SAND_MULTI_SET_INFO *info
                                   )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_memset(info, 0x0, sizeof(SOC_SAND_MULTI_SET_INFO));

  info->init_info.max_duplications = 1;
  info->init_info.get_entry_fun    = 0;
  info->init_info.set_entry_fun    = 0;
  info->init_info.nof_members      = 0;
  info->init_info.member_size      = 0;
  info->init_info.prime_handle     = 0;
  info->init_info.sec_handle       = 0;
  info->init_info.wb_var_index     = SOC_DPP_WB_ENGINE_VAR_NONE;

  info->multiset_data.counter_size = 0;
  info->multiset_data.ref_counter  = 0;

  soc_sand_SAND_HASH_TABLE_INFO_clear(&(info->multiset_data.hash_table));

  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}
#ifdef SOC_SAND_DEBUG

/*********************************************************************
* NAME:
*     soc_sand_multi_set_print
* TYPE:
*   PROC
* DATE:
*   Mar  6 2008
* FUNCTION:
*     prints the mutli-set content, the members in the multi set, the number of occurrences of
*     each member and the index.
* INPUT:
*  SOC_SAND_IN  SOC_SAND_MULTI_SET_INFO   *multi_set -
*     The mutli-set to print.
*  SOC_SAND_IN  SOC_SAND_MULTI_SET_PRINT_VAL print_fun -
*     call back to print the member.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_sand_multi_set_print(
    SOC_SAND_IN  SOC_SAND_MULTI_SET_INFO      *multi_set,
    SOC_SAND_IN  SOC_SAND_MULTI_SET_PRINT_VAL print_fun,
    SOC_SAND_IN  uint8                clear_on_print
  )
{
    SOC_SAND_IN SOC_SAND_MULTI_SET_INIT_INFO
        *set_init;
    SOC_SAND_IN SOC_SAND_MULTI_SET_T
        *set_data;
    uint32
        data_indx;
    SOC_SAND_HASH_TABLE_ITER
        iter;
    uint8
        key[100];
    uint32
        ref_count,
        org_ref_count;
    uint8
        tmp_cnt[sizeof(uint32)];
    uint32
        res;

    SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(SOC_SAND_MULTI_SET_PRINT);

    SOC_SAND_CHECK_NULL_INPUT(multi_set);

    set_data = &(multi_set->multiset_data);
    set_init = &(multi_set->init_info);


    /*
     * traverse the mutli-set and print
     */
    SOC_SAND_HASH_TABLE_ITER_SET_BEGIN(&iter);

    while (!SOC_SAND_HASH_TABLE_ITER_IS_END(&iter))
    {
        res = soc_sand_hash_table_get_next(
                                           &(set_data->hash_table),
                                           &iter,
                                           key,
                                           &data_indx
                                           );
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
        if (SOC_SAND_HASH_TABLE_ITER_IS_END(&iter))
        {
            goto exit;
        }
        org_ref_count = 1;
        if (set_init->max_duplications > 1)
        {
            ref_count = 0;
            res = set_init->get_entry_fun(
                                          set_init->prime_handle,
                                          set_init->sec_handle,
                                          set_data->ref_counter,
                                          data_indx,
                                          set_data->counter_size,
                                          tmp_cnt
                                          );
            SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
            res = soc_sand_U8_to_U32(
                                     tmp_cnt,
                                     set_data->counter_size,
                                     &ref_count
                                     );
            SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

            org_ref_count = ref_count;
            if (clear_on_print)
            {
                ref_count = 0;

                res = soc_sand_U32_to_U8(
                                         &ref_count,
                                         set_data->counter_size,
                                         tmp_cnt
                                         );
                SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

                if(SOC_DPP_WB_ENGINE_VAR_NONE == set_init->wb_var_index)
                {
                    res = set_init->set_entry_fun(
                                                  set_init->prime_handle,
                                                  set_init->sec_handle,
                                                  set_data->ref_counter,
                                                  data_indx,
                                                  set_data->counter_size,
                                                  tmp_cnt
                                                  );
                    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);


                }
                else
                {
                    unit = (set_init->prime_handle);

                    res = SOC_DPP_WB_ENGINE_SET_ARR(unit, set_init->wb_var_index + WB_ENGINE_MULTI_SET_NEXT_REF_COUNTER, 
                                                    tmp_cnt, 
                                                    data_indx);
                    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 60, exit);   
                }   



            }
        }
    if (org_ref_count)
    {
      LOG_CLI((BSL_META_U(unit,
                          "| %-8u|"), data_indx));
      print_fun(key);
      LOG_CLI((BSL_META_U(unit,
                          "| %-8u|\n\r"), org_ref_count));
    }
  }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_multi_set_print()",0,0);
}

uint32
  soc_sand_multi_set_get_size_for_save(
    SOC_SAND_IN  SOC_SAND_MULTI_SET_INFO              *multi_set,
    SOC_SAND_OUT  uint32                        *size
  )
{
  const SOC_SAND_MULTI_SET_INIT_INFO
    *set_init;
  const SOC_SAND_MULTI_SET_T
    *set_data;
  uint32
    cur_size,
    total_size=0;
  uint32
    res;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);

  SOC_SAND_CHECK_NULL_INPUT(multi_set);
  SOC_SAND_CHECK_NULL_INPUT(size);

  /* copy init info */
  set_init = &(multi_set->init_info);
  set_data = &(multi_set->multiset_data);

  cur_size = sizeof(SOC_SAND_HASH_TABLE_INIT_INFO);
  total_size += cur_size;

  /* load hash table*/
  res = soc_sand_hash_table_get_size_for_save(
            &(set_data->hash_table),
            &cur_size
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  total_size += cur_size;
 /*
  * if there is need to manage the duplications, then copy array to
  * manage the reference counter.
  */
  if (set_init->max_duplications > 1)
  {
    total_size += set_init->nof_members * set_data->counter_size;
  }

  *size= total_size;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_multi_set_save()",0,0);
}

/*********************************************************************
* NAME:
*     soc_sand_multi_set_save
* TYPE:
*   PROC
* DATE:
*   Mar  6 2008
* FUNCTION:
*     saves the given multiset in the given buffer
* INPUT:
*   SOC_SAND_IN  SOC_SAND_MULTI_SET_PTR multi_set -
*     The multiset to save.
*   SOC_SAND_OUT  uint8                 *buffer -
*     buffer to include the hast table
* REMARKS:
*   - the size of the buffer has to be at least as the value returned
*     by soc_sand_multi_set_get_size_for_save.
*   - call back functions are not saved.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_sand_multi_set_save(
    SOC_SAND_IN  SOC_SAND_MULTI_SET_INFO     *multi_set,
    SOC_SAND_OUT uint8                 *buffer,
    SOC_SAND_IN  uint32                buffer_size_bytes,
    SOC_SAND_OUT uint32                *actual_size_bytes
  )
{
  const SOC_SAND_MULTI_SET_INIT_INFO
    *set_init;
  const SOC_SAND_MULTI_SET_T
    *set_data;
  uint8
    *cur_ptr = (uint8*)buffer;
  uint32
    cur_size,
    total_size=0;
  uint32
    res;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);

  SOC_SAND_CHECK_NULL_INPUT(multi_set);
  SOC_SAND_CHECK_NULL_INPUT(multi_set->init_info.get_entry_fun);
  SOC_SAND_CHECK_NULL_INPUT(multi_set->init_info.set_entry_fun);
  SOC_SAND_CHECK_NULL_INPUT(actual_size_bytes);

  set_init = &(multi_set->init_info);
  set_data = &(multi_set->multiset_data);

  /* copy init info */
  SOC_SAND_COPY_TO_BUFF_AND_INC(cur_ptr,set_init,SOC_SAND_MULTI_SET_INIT_INFO,1);

  /* copy DS data */
  res = soc_sand_hash_table_save(
            &(set_data->hash_table),
            cur_ptr,
            buffer_size_bytes - total_size,
            &cur_size
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  cur_ptr += cur_size;
  total_size += cur_size;
 /*
  * if there is need to manage the duplications, then copy array to
  * manage the reference counter.
  */
  if (set_init->max_duplications > 1)
  {
    SOC_SAND_COPY_TO_BUFF_AND_INC(cur_ptr,set_data->ref_counter,uint8,set_init->nof_members * set_data->counter_size);
  }

  *actual_size_bytes = total_size;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_multi_set_save()",0,0);
}

uint32
  soc_sand_multi_set_load(
    SOC_SAND_IN  uint8                           **buffer,
    SOC_SAND_IN  SOC_SAND_MULTISET_SW_DB_ENTRY_SET     set_function,
    SOC_SAND_IN  SOC_SAND_MULTISET_SW_DB_ENTRY_GET     get_function,
    SOC_SAND_OUT SOC_SAND_MULTI_SET_INFO               *multi_set
  )
{
  SOC_SAND_MULTI_SET_INIT_INFO
    *set_init;
  SOC_SAND_MULTI_SET_T
    *set_data;
  SOC_SAND_IN uint8
    *cur_ptr = (SOC_SAND_IN uint8*)buffer[0];
  uint32
    res;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);

  SOC_SAND_CHECK_NULL_INPUT(multi_set);
  SOC_SAND_CHECK_NULL_INPUT(buffer);

  /* copy init info */
  soc_sand_SAND_MULTI_SET_INFO_clear(multi_set);
  soc_sand_os_memcpy(&(multi_set->init_info), cur_ptr,sizeof(SOC_SAND_MULTI_SET_INIT_INFO));
  cur_ptr += sizeof(SOC_SAND_MULTI_SET_INIT_INFO);
  multi_set->init_info.set_entry_fun = set_function;
  multi_set->init_info.get_entry_fun = get_function;

  set_init = &(multi_set->init_info);
  set_data = &(multi_set->multiset_data);

  /* create DS*/
  res = soc_sand_multi_set_create(
          multi_set
        );
  SOC_SAND_CHECK_FUNC_RESULT(res,20, exit);

  res = soc_sand_hash_table_destroy(
          &(set_data->hash_table)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res,20, exit);

  /* load hash table*/
  res = soc_sand_hash_table_load(
            &cur_ptr,
            set_function,
            get_function,
            NULL,
            NULL, 
            &(set_data->hash_table)
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
 /*
  * if there is need to manage the duplications, then copy array to
  * manage the reference counter.
  */
  if (set_init->max_duplications > 1)
  {
    soc_sand_os_memcpy(set_data->ref_counter,cur_ptr,set_init->nof_members * set_data->counter_size);
    cur_ptr += set_init->nof_members * set_data->counter_size;
  }
  *buffer = cur_ptr;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_multi_set_load()",0,0);
}

void
  soc_sand_SAND_MULTI_SET_INFO_print(
    SOC_SAND_IN SOC_SAND_MULTI_SET_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "info->init_info.max_duplications: %u\n"),info->init_info.max_duplications));
  LOG_CLI((BSL_META_U(unit,
                      "info->init_info.nof_members: %u\n"),info->init_info.nof_members));
  LOG_CLI((BSL_META_U(unit,
                      "info->init_info.member_size: %u\n"),info->init_info.member_size));
  LOG_CLI((BSL_META_U(unit,
                      "info->multiset_data.counter_size: %u\n"),info->multiset_data.counter_size));
  LOG_CLI((BSL_META_U(unit,
                      "info->init_info.prime_handle: %u\n"),info->init_info.prime_handle));
  LOG_CLI((BSL_META_U(unit,
                      "info->init_info.sec_handle: %u\n"),info->init_info.sec_handle));

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif /* SOC_SAND_DEBUG */

#include <soc/dpp/SAND/Utils/sand_footer.h>
