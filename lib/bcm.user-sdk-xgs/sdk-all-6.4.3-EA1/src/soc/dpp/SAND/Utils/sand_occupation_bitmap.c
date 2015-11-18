/* $Id: sand_occupation_bitmap.c,v 1.21 Broadcom SDK $
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
 * $
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

#include <soc/dpp/SAND/Utils/sand_occupation_bitmap.h>
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
#define  SOC_SAND_OCC_BM_ARRAY_MAP_LEN 256
#define  SOC_SAND_OCC_BM_NODE_IS_FULL  8

/* } */

/*************
*  MACROS   *
*************/
/* { */
#define SOC_SAND_OCC_BM_ACTIVE_INST(_bmp_info, _inst) \
      ((_bmp_info->cache_enabled)?(_bmp_info->levels_cache):(_bmp_info->levels))

#define SOC_SAND_OCC_BM_CACHE_BUFFER_OFFSET(_level_indx)                           \
    (bit_map->levels_cache[_level_indx] - bit_map->levels_cache_buffer)

#define SOC_SAND_OCC_BM_BUFFER_OFFSET(_level_indx) \
    (bit_map->levels[_level_indx] - bit_map->levels_buffer)

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

/* $Id: sand_occupation_bitmap.c,v 1.21 Broadcom SDK $
 * static global array to find efficiently the first bit set to one in a uint8
 */
static
  uint8 Soc_sand_bitmap_val_to_first_one[SOC_SAND_OCC_BM_ARRAY_MAP_LEN] =
{
  SOC_SAND_OCC_BM_NODE_IS_FULL,
  0,
  1, 0,
  2, 0, 1, 0,
  3, 0, 1, 0, 2, 0, 1, 0,
  4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
  5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
  6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
  7, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0
};

/*
 * static global array to find efficiently the first bit set to zero in a uint8
 */
static
  uint8 Soc_sand_bitmap_val_to_first_zero[SOC_SAND_OCC_BM_ARRAY_MAP_LEN] =
{
  0,
  1, 0,
  2, 0, 1, 0,
  3, 0, 1, 0, 2, 0, 1, 0,
  4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
  5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
  6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
  7, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
  SOC_SAND_OCC_BM_NODE_IS_FULL
};

/*
 * this array can be replaced with simple calculations but needed for high performances.
 */
static
  uint8 Soc_sand_bitmap_val_to_last_zero[SOC_SAND_OCC_BM_ARRAY_MAP_LEN] =
{
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  3, 3, 3, 3, 3, 3, 3, 3,
  2, 2, 2, 2,
  1, 1,
  0,
  SOC_SAND_OCC_BM_NODE_IS_FULL
};

/* } */

/*************
* FUNCTIONS *
*************/

/*********************************************************************
* NAME:
*     soc_sand_occ_bm_create
* TYPE:
*   PROC
* DATE:
*   Mar  6 2008
* FUNCTION:
*     Creates a new bitmap instance.
* INPUT:
*  SOC_SAND_IN  uint32                 size -
*     the size of the bit map (number of bits)
*  SOC_SAND_OUT  SOC_SAND_OCC_BM_PTR bit_map -
*     The created bitmap.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_sand_occ_bm_create(
    SOC_SAND_IN       SOC_SAND_OCC_BM_INIT_INFO   *init_info,
    SOC_SAND_OUT      SOC_SAND_OCC_BM_PTR         *bit_map
  )
{
  SOC_SAND_OCC_BM_PTR
    new_bitmap;
  uint32
    cur_size,
    level_size,
    all_level_size,
    nof_levels,
    size;
  uint8
    init_val;
  uint32
    indx,
    buffer_offset;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(SOC_SAND_OCC_BM_CREATE);

  SOC_SAND_CHECK_NULL_INPUT(bit_map);
  SOC_SAND_CHECK_NULL_INPUT(init_info);

 /*
  * allocate the bitmap struct
  */
  size = init_info->size;
  if (init_info->init_val == FALSE)
  {
    init_val = 0X0;
  }
  else
  {
    init_val = 0XFF;
  }
  if((init_info->wb_var_index != SOC_DPP_WB_ENGINE_VAR_NONE) && (init_info->unit == -1)){
	  SOC_SAND_SET_ERROR_CODE(SOC_SAND_ILLEGAL_DEVICE_ID, 5, exit);
  }
  new_bitmap = (SOC_SAND_OCC_BM_PTR) soc_sand_os_malloc(sizeof(SOC_SAND_OCC_BM_T), "new_bitmap");
  if (!new_bitmap)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_MALLOC_FAIL, 10, exit);
  }

  soc_sand_os_memset(new_bitmap, 0, sizeof(SOC_SAND_OCC_BM_T));

  new_bitmap->init_val            = init_info->init_val;
  new_bitmap->cache_enabled       = FALSE;
  new_bitmap->support_cache       = init_info->support_cache;
  new_bitmap->levels_buffer       = NULL;
  new_bitmap->levels_cache_buffer = NULL;
  new_bitmap->unit           = init_info->unit;
  new_bitmap->wb_var_index        = init_info->wb_var_index;

  cur_size = SOC_SAND_NOF_BITS_IN_CHAR;/*in bits*/
  nof_levels = 1;
  level_size = SOC_SAND_NOF_BITS_IN_CHAR; /*in bits*/
 /*
  * calculate the number of levels and allocate the array of levels.
  */
  while (cur_size < size)
  {
    cur_size *= SOC_SAND_NOF_BITS_IN_CHAR;
    ++nof_levels;
  }
  new_bitmap->levels = (uint8**) soc_sand_os_malloc(sizeof(uint8*) * nof_levels, "new_bitmap->levels");
  new_bitmap->levels_cache = NULL;
  
  if (!new_bitmap->levels)
  {
    soc_sand_os_free(new_bitmap);
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_MALLOC_FAIL, 20, exit);
  }

  soc_sand_os_memset(new_bitmap->levels, 0, sizeof(uint8*) * nof_levels);

  if (init_info->support_cache)
  {
    new_bitmap->support_cache = TRUE;
    new_bitmap->levels_cache = (uint8**) soc_sand_os_malloc(sizeof(uint8*) * nof_levels, "new_bitmap->levels_cache");
    if (!new_bitmap->levels_cache)
    {
      soc_sand_os_free(new_bitmap->levels);
      soc_sand_os_free(new_bitmap);
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_MALLOC_FAIL, 30, exit);
    }

    soc_sand_os_memset(new_bitmap->levels_cache, 0, sizeof(uint8*) * nof_levels);
  }

  new_bitmap->levels_size= (uint32*) soc_sand_os_malloc(sizeof(uint32) * nof_levels, "new_bitmap->levels_size");
  if (!new_bitmap->levels_size)
  {
    soc_sand_os_free(new_bitmap->levels);
    soc_sand_os_free(new_bitmap);
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_MALLOC_FAIL, 30, exit);
  }

  soc_sand_os_memset(new_bitmap->levels_size, 0, sizeof(uint32) * nof_levels);

 /*
  * now allocate the array for all levels.
  */
  all_level_size = 0;
  level_size = size;
  for (indx = nof_levels; indx > 0 ; --indx)
  {
      level_size = (level_size + (SOC_SAND_NOF_BITS_IN_CHAR - 1)) / SOC_SAND_NOF_BITS_IN_CHAR; /* in chars */
      
      all_level_size += level_size;
  }
  if (new_bitmap->support_cache)
  {
      new_bitmap->levels_cache_buffer = (uint8*) soc_sand_os_malloc(sizeof(uint8) * all_level_size, "new_bitmap->levels_cache_buffer");
      if (!new_bitmap->levels_cache_buffer)
      {
          goto free_and_exit;
      }

      soc_sand_os_memset(new_bitmap->levels_cache_buffer, 0, sizeof(uint8) * all_level_size);
  }
  new_bitmap->levels_buffer = (uint8*) soc_sand_os_malloc(sizeof(uint8) * all_level_size, "new_bitmap->levels_buffer");
  if (!new_bitmap->levels_buffer)
  {
      goto free_and_exit;
  }

  res = soc_sand_os_memset(new_bitmap->levels_buffer,
                           init_val,
                           sizeof(uint8) * all_level_size);

  SOC_SAND_CHECK_FUNC_RESULT(res, 40, free_and_exit);

 
 /*
  * now set the pointers array for all levels.
  */
  level_size    = size;
  buffer_offset = 0;
  for (indx = nof_levels; indx > 0 ; --indx)
  {
    new_bitmap->levels_size[indx - 1] = level_size; /* in bits */
    level_size = (level_size + (SOC_SAND_NOF_BITS_IN_CHAR - 1)) / SOC_SAND_NOF_BITS_IN_CHAR; /* in chars */
    if (new_bitmap->support_cache)
    {
      new_bitmap->levels_cache[indx - 1] = new_bitmap->levels_cache_buffer + buffer_offset;
    }
    
    new_bitmap->levels[indx - 1] = new_bitmap->levels_buffer + buffer_offset;

    /* adding this level size to the buffer offset */
    buffer_offset += level_size;

  }
  new_bitmap->nof_levels  = nof_levels;
  new_bitmap->size        = size;
  new_bitmap->buffer_size = all_level_size;

  *bit_map =  new_bitmap;
  goto exit;

free_and_exit:
  if (new_bitmap->levels_buffer)
  {
    soc_sand_os_free(new_bitmap->levels_buffer);
  }
  if (new_bitmap->support_cache)
  {
    if (new_bitmap->levels_cache)
    {
      soc_sand_os_free(new_bitmap->levels_cache);
    }
    if (new_bitmap->levels_cache_buffer)
    {
      soc_sand_os_free(new_bitmap->levels_cache_buffer);
    }
  }
  
  soc_sand_os_free(new_bitmap->levels);
  soc_sand_os_free(new_bitmap);
  SOC_SAND_SET_ERROR_CODE(SOC_SAND_MALLOC_FAIL, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_occ_bm_create()",0,0);

}


/*********************************************************************
* NAME:
*     soc_sand_occ_bm_destroy
* TYPE:
*   PROC
* DATE:
*   Mar  6 2008
* FUNCTION:
*     Creates a new bitmap instance.
* INPUT:
*  SOC_SAND_OUT  SOC_SAND_OCC_BM_PTR bit_map -
*     The bitmap to destroy.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_sand_occ_bm_destroy(
    SOC_SAND_OUT       SOC_SAND_OCC_BM_PTR   bit_map
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(SOC_SAND_OCC_BM_DESTROY);

  SOC_SAND_CHECK_NULL_INPUT(bit_map);

  soc_sand_os_free(bit_map->levels);
  soc_sand_os_free(bit_map->levels_buffer);

  if (bit_map->support_cache)
  {
    soc_sand_os_free(bit_map->levels_cache);
    soc_sand_os_free(bit_map->levels_cache_buffer);
  }
  
  soc_sand_os_free(bit_map->levels_size);
  soc_sand_os_free(bit_map);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_occ_bm_destroy()",0,0);
}



/*********************************************************************
* NAME:
*     soc_sand_occ_bm_destroy
* TYPE:
*   PROC
* DATE:
*   Mar  6 2008
* FUNCTION:
*     clear the bitmap content without freeing the memory.
* INPUT:
*  SOC_SAND_OUT  SOC_SAND_OCC_BM_PTR bit_map -
*     The bitmap to clear.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_sand_occ_bm_clear(
    SOC_SAND_OUT       SOC_SAND_OCC_BM_PTR   bit_map
  )
{
  uint32
    level_indx,
    level_size, res;
  uint8
    init_val;
  int unit = BSL_UNIT_UNKNOWN;



  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_OCC_BM_CLEAR);

  SOC_SAND_CHECK_NULL_INPUT(bit_map);


  if (bit_map->init_val == FALSE)
  {
    init_val = 0X0;
  }
  else
  {
    init_val = 0XFF;
  }

  if(SOC_DPP_WB_ENGINE_VAR_NONE == bit_map->wb_var_index) 
  {
      for (level_indx = 0; level_indx < bit_map->nof_levels; ++level_indx )
      {
          level_size = (bit_map->levels_size[level_indx] + SOC_SAND_NOF_BITS_IN_CHAR - 1) / SOC_SAND_NOF_BITS_IN_CHAR;

          if (bit_map->support_cache && bit_map->cache_enabled)
          {     
              soc_sand_os_memset(bit_map->levels_cache[level_indx],init_val,level_size * sizeof(uint8));
          }
          else
          {
              soc_sand_os_memset(bit_map->levels[level_indx],init_val,level_size * sizeof(uint8));
          }
      }
  }
  else
  {
      unit = (bit_map->unit);

      if (bit_map->support_cache && bit_map->cache_enabled)
      {     
          res = SOC_DPP_WB_ENGINE_MEMSET_ARR(unit, bit_map->wb_var_index + WB_ENGINE_OCC_BM_LEVELS_CACHE_BUFFER, init_val);
          SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
      }
      else
      {
          res = SOC_DPP_WB_ENGINE_MEMSET_ARR(unit, bit_map->wb_var_index + WB_ENGINE_OCC_BM_LEVELS_BUFFER, init_val);
          SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);         
      }
  }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_occ_bm_clear()",0,0);

}



/*********************************************************************
* NAME:
*     soc_sand_occ_bm_get_next_helper
* TYPE:
*   PROC
* DATE:
*   Mar  6 2008
* FUNCTION:
*     Creates a new bitmap instance.
* INPUT:
*  SOC_SAND_IN  SOC_SAND_OCC_BM_PTR bit_map -
*     The bitmap to perform the get operation at.
*   SOC_SAND_IN  uint8                    val -
*     bit value to look for 0/1
*   SOC_SAND_OUT  uint32  *place -
*     start of the found chunk
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
STATIC uint32
  soc_sand_occ_bm_get_next_helper(
    SOC_SAND_IN  SOC_SAND_OCC_BM_PTR              bit_map,
    SOC_SAND_IN  uint8                    val,
    SOC_SAND_IN  uint32                     level_indx,
    SOC_SAND_IN  uint32                     char_indx,
    SOC_SAND_OUT  uint32                    *place,
    SOC_SAND_OUT  uint8                   *found
  )
{
  uint32
    cur_val,
    cur_bit,
    level_size;
  uint32
    res;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(SOC_SAND_OCC_BM_GET_NEXT_HELPER);

  SOC_SAND_CHECK_NULL_INPUT(bit_map);

  cur_val = SOC_SAND_OCC_BM_ACTIVE_INST(bit_map,levels)[level_indx][char_indx];
  level_size = bit_map->levels_size[level_indx];
 /*
  * find the next one/zero in the uint8
  */
  if (val)
  {
    cur_bit = Soc_sand_bitmap_val_to_first_one[cur_val];
  }
  else
  {
    cur_bit = Soc_sand_bitmap_val_to_first_zero[cur_val];
  }
 /*
  * if bit was not found, or was found but after the level size, then return with found = FALSE;
  */
  if (cur_bit == SOC_SAND_OCC_BM_NODE_IS_FULL || cur_bit + SOC_SAND_NOF_BITS_IN_CHAR * char_indx >= level_size)
  {
    *place = 0;
    *found = FALSE;
    goto exit;
  }
 /*
  * Reached to the last level (the bit map) return.
  */
  if (level_indx == bit_map->nof_levels - 1)
  {
    *place = cur_bit + SOC_SAND_NOF_BITS_IN_CHAR * char_indx;
    *found = TRUE;
    goto exit;
  }
 /*
  * else continue to the next level
  */
  res = soc_sand_occ_bm_get_next_helper(
          bit_map,
          val,
          level_indx + 1,
          cur_bit + SOC_SAND_NOF_BITS_IN_CHAR * char_indx,
          place,
          found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_occ_bm_get_next_helper()",0,0);
}

/*********************************************************************
* NAME:
*     soc_sand_occ_bm_get_next
* TYPE:
*   PROC
* DATE:
*   Mar  6 2008
* FUNCTION:
*     Creates a new bitmap instance.
* INPUT:
*  SOC_SAND_IN  SOC_SAND_OCC_BM_PTR bit_map -
*     The bitmap to perform the get operation at.
*   SOC_SAND_IN  uint8                    val -
*     bit value to look for 0/1
*   SOC_SAND_OUT  uint32  *place -
*     start of the found chunk
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
STATIC uint32
  soc_sand_occ_bm_calc_mask(
    SOC_SAND_IN  uint32                     nof_levels,
    SOC_SAND_IN  uint32                     level_indx,
    SOC_SAND_IN  uint32                     char_indx,
    SOC_SAND_IN  uint32                     down_limit,
    SOC_SAND_IN  uint32                     up_limit,
    SOC_SAND_IN  uint8                    forward
  )
{
  uint32
    range_start,
    range_end,
    btmp_start,
    btmp_end,
    start,
    end,
    mask_val,
    indx;

  range_start = down_limit;
  range_end = up_limit;

  for (indx = nof_levels - 1; indx > level_indx; --indx )
  {
    range_start = (range_start) / SOC_SAND_NOF_BITS_IN_CHAR;
    range_end = (range_end ) / SOC_SAND_NOF_BITS_IN_CHAR;
  }

  btmp_start = char_indx * SOC_SAND_NOF_BITS_IN_CHAR;
  btmp_end = (char_indx + 1) * SOC_SAND_NOF_BITS_IN_CHAR - 1;

  if (range_start < btmp_start)
  {
    start = 0;
  }
  else if (range_start > btmp_end)
  {
    return SOC_SAND_U32_MAX;
  }
  else
  {
    start = range_start - btmp_start;
  }

  if (range_end > btmp_end)
  {
    end = SOC_SAND_NOF_BITS_IN_UINT32 - 1;
  }
  else if (range_end < btmp_start)
  {
    return SOC_SAND_U32_MAX;
  }
  else
  {
    end = range_end - btmp_start;
  }

  mask_val = 0;
  mask_val = SOC_SAND_ZERO_BITS_MASK(end,start);
  mask_val &= SOC_SAND_ZERO_BITS_MASK(SOC_SAND_NOF_BITS_IN_UINT32 - 1,SOC_SAND_NOF_BITS_IN_CHAR);

  return mask_val;
}


/*********************************************************************
* NAME:
*     soc_sand_occ_bm_get_next
* TYPE:
*   PROC
* DATE:
*   Mar  6 2008
* FUNCTION:
*     Creates a new bitmap instance.
* INPUT:
*  SOC_SAND_IN  SOC_SAND_OCC_BM_PTR bit_map -
*     The bitmap to perform the get operation at.
*   SOC_SAND_IN  uint8                    val -
*     bit value to look for 0/1
*   SOC_SAND_OUT  uint32  *place -
*     start of the found chunk
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
STATIC uint32
  soc_sand_occ_bm_get_next_range_helper(
    SOC_SAND_IN  SOC_SAND_OCC_BM_PTR              bit_map,
    SOC_SAND_IN  uint8                    val,
    SOC_SAND_IN  uint32                     level_indx,
    SOC_SAND_IN  uint32                     char_indx,
    SOC_SAND_IN  uint32                     down_limit,
    SOC_SAND_IN  uint32                     up_limit,
    SOC_SAND_IN  uint8                    forward,
    SOC_SAND_OUT  uint32                    *place,
    SOC_SAND_OUT  uint8                   *found
  )
{
  uint32
    cur_val,
    cur_bit,
    char_iter,
    level_size,
    mask_val;
  uint32
    res;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(SOC_SAND_OCC_BM_GET_NEXT_HELPER);

  SOC_SAND_CHECK_NULL_INPUT(bit_map);

  level_size = bit_map->levels_size[level_indx];

  cur_val = SOC_SAND_OCC_BM_ACTIVE_INST(bit_map,levels)[level_indx][char_indx];

  mask_val = soc_sand_occ_bm_calc_mask(
               bit_map->nof_levels,
               level_indx,
               char_indx,
               down_limit,
               up_limit,
               forward
             );

  cur_val |= mask_val;

 /*
  * maximum there are 8 tries
  */
  for (char_iter = 0; char_iter < SOC_SAND_NOF_BITS_IN_CHAR; ++char_iter)
  {
    if (forward)
    {
     /*
      * find the next one/zero in the uint8
      */
      if (val)
      {
        cur_bit = Soc_sand_bitmap_val_to_first_one[cur_val];
      }
      else
      {
        cur_bit = Soc_sand_bitmap_val_to_first_zero[cur_val];
      }
    }
    else
    {
      if (val)
      {
        cur_bit = Soc_sand_bitmap_val_to_last_zero[255 - cur_val];
      }
      else
      {
        cur_bit = Soc_sand_bitmap_val_to_last_zero[cur_val];
      }
    }
   /*
    * if bit was not found, or was found but after the level size, then return with found = FALSE;
    */
    if (cur_bit == SOC_SAND_OCC_BM_NODE_IS_FULL || cur_bit + SOC_SAND_NOF_BITS_IN_CHAR * char_indx >= level_size)
    {
      *place = 0;
      *found = FALSE;
      goto exit;
    }
   /*
    * Reached to the last level (the bit map) return.
    */
    if (level_indx == bit_map->nof_levels - 1)
    {
      *place = cur_bit + SOC_SAND_NOF_BITS_IN_CHAR * char_indx;
      *found = TRUE;
      goto exit;
    }
   /*
    * this bit was tried if failed then don't try it again!
    */
    cur_val |= SOC_SAND_BIT(cur_bit);
   /*
    * else continue to the next level
    */
    res = soc_sand_occ_bm_get_next_range_helper(
            bit_map,
            val,
            level_indx + 1,
            cur_bit + SOC_SAND_NOF_BITS_IN_CHAR * char_indx,
            down_limit,
            up_limit,
            forward,
            place,
            found
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    if (*found)
    {
      goto exit;
    }
  }
  *place = 0;
  *found = FALSE;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_occ_bm_get_next_helper()",0,0);
}

/*********************************************************************
* NAME:
*     soc_sand_occ_bm_get_next
* TYPE:
*   PROC
* DATE:
*   Mar  6 2008
* FUNCTION:
*     Creates a new bitmap instance.
* INPUT:
*  SOC_SAND_IN  SOC_SAND_OCC_BM_PTR bit_map -
*     The bitmap to perform the get operation at.
*   SOC_SAND_IN  uint8                    val -
*     bit value to look for 0/1
*   SOC_SAND_OUT  uint32  *place -
*     start of the found chunk
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_sand_occ_bm_get_next(
    SOC_SAND_IN  SOC_SAND_OCC_BM_PTR              bit_map,
    SOC_SAND_OUT  uint32                    *place,
    SOC_SAND_OUT  uint8                   *found
  )
{
  uint32
    res;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(SOC_SAND_OCC_BM_GET_NEXT);

  SOC_SAND_CHECK_NULL_INPUT(bit_map);

  res = soc_sand_occ_bm_get_next_helper(
          bit_map,
          0,
          0,
          0,
          place,
          found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  if (*place >= bit_map->size)
  {
    *place = 0;
    *found = FALSE;
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_occ_bm_destroy()",0,0);
}


/*********************************************************************
* NAME:
*     soc_sand_occ_bm_get_next
* TYPE:
*   PROC
* DATE:
*   Mar  6 2008
* FUNCTION:
*     Creates a new bitmap instance.
* INPUT:
*  SOC_SAND_IN  SOC_SAND_OCC_BM_PTR bit_map -
*     The bitmap to perform the get operation at.
*   SOC_SAND_IN  uint8                    val -
*     bit value to look for 0/1
*   SOC_SAND_OUT  uint32  *place -
*     start of the found chunk
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_sand_occ_bm_get_next_in_range(
    SOC_SAND_IN   SOC_SAND_OCC_BM_PTR             bit_map,
    SOC_SAND_IN   uint32                    start,
    SOC_SAND_IN   uint32                    end,
    SOC_SAND_IN  uint8                    forward,
    SOC_SAND_OUT  uint32                    *place,
    SOC_SAND_OUT  uint8                   *found
  )
{
  uint32
    res;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(SOC_SAND_OCC_BM_GET_NEXT);

  SOC_SAND_CHECK_NULL_INPUT(bit_map);

  res = soc_sand_occ_bm_get_next_range_helper(
          bit_map,
          0,
          0,
          0,
          start,
          end,
          forward,
          place,
          found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  if (*place >= bit_map->size)
  {
    *place = 0;
    *found = FALSE;
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_occ_bm_destroy()",0,0);
}


/*********************************************************************
* NAME:
*     soc_sand_occ_bm_alloc_next
* TYPE:
*   PROC
* DATE:
*   Mar  6 2008
* FUNCTION:
*     Creates a new bitmap instance.
* INPUT:
*  SOC_SAND_IN  SOC_SAND_OCC_BM_PTR bit_map -
*     The bitmap to perform the get operation at.
*   SOC_SAND_IN  uint8                    val -
*     bit value to look for 0/1
*   SOC_SAND_OUT  uint32  *place -
*     start of the found chunk
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_sand_occ_bm_alloc_next(
    SOC_SAND_IN  SOC_SAND_OCC_BM_PTR              bit_map,
    SOC_SAND_OUT  uint32                    *place,
    SOC_SAND_OUT  uint8                   *found
  )
{
  uint32
    res;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(SOC_SAND_OCC_BM_GET_NEXT);

  SOC_SAND_CHECK_NULL_INPUT(bit_map);

  res = soc_sand_occ_bm_get_next(
          bit_map,
          place,
          found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (*place >= bit_map->size)
  {
    *place = 0;
    *found = FALSE;
    goto exit;
  }

  res = soc_sand_occ_bm_occup_status_set(
          bit_map,
          *place,
          1
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_occ_bm_alloc_next()",0,0);
}
/* } */



uint32
  soc_sand_occ_bm_occup_status_set_helper(
    SOC_SAND_INOUT  SOC_SAND_OCC_BM_PTR     bit_map,
    SOC_SAND_IN  uint32               level_indx,
    SOC_SAND_IN  uint32               char_indx,
    SOC_SAND_IN  uint32               bit_indx,
    SOC_SAND_IN  uint8              occupied
  )
{
  uint32
    old_val,
    new_val;
  uint32
    res;
  int unit = BSL_UNIT_UNKNOWN;
  uint8 new_val_uint8;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_OCC_BM_OCCUP_STATUS_SET);

  SOC_SAND_CHECK_NULL_INPUT(bit_map);


  old_val = SOC_SAND_OCC_BM_ACTIVE_INST(bit_map,levels)[level_indx][char_indx];
  new_val = old_val;

  res = soc_sand_set_field(
          &new_val,
          bit_indx,
          bit_indx,
          occupied
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if(SOC_DPP_WB_ENGINE_VAR_NONE == bit_map->wb_var_index) 
  {
      SOC_SAND_OCC_BM_ACTIVE_INST(bit_map,levels)[level_indx][char_indx] = (uint8)new_val;
  }
  else
  {
      unit = (bit_map->unit);

      new_val_uint8 = (uint8)new_val;
      
      if(bit_map->cache_enabled)
      {
          res = SOC_DPP_WB_ENGINE_SET_ARR(unit, bit_map->wb_var_index + WB_ENGINE_OCC_BM_LEVELS_CACHE_BUFFER, 
                                          &(new_val_uint8), 
                                          SOC_SAND_OCC_BM_CACHE_BUFFER_OFFSET(level_indx) + char_indx);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
      
      }
      else
      {
          res = SOC_DPP_WB_ENGINE_SET_ARR(unit, bit_map->wb_var_index + WB_ENGINE_OCC_BM_LEVELS_BUFFER, 
                                          &(new_val_uint8), 
                                          SOC_SAND_OCC_BM_BUFFER_OFFSET(level_indx) + char_indx);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
      
      }
  }

 /*
  * check if there is need to update the higher levels
  */
  if (level_indx == 0)
  {
    goto exit;
  }

  if ((occupied && old_val != 0xFF && new_val == 0xFF) ||
      (!occupied && old_val == 0xFF && new_val != 0xFF) )
  {
    res = soc_sand_occ_bm_occup_status_set_helper(
            bit_map,
            level_indx - 1,
            char_indx / SOC_SAND_NOF_BITS_IN_CHAR,
            char_indx % SOC_SAND_NOF_BITS_IN_CHAR,
            occupied
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_occ_bm_occup_status_set_helper()",0,0);
}


/*********************************************************************
* NAME:
*     soc_sand_occ_bm_occup_status_set
* TYPE:
*   PROC
* DATE:
*   Mar  6 2008
* FUNCTION:
*     Set the occupation status a of sequence of bits.
* INPUT:
*  SOC_SAND_INOUT  SOC_SAND_OCC_BM_PTR bit_map -
*     The bitmap to perform the set operation at.
*  SOC_SAND_IN  uint32  place -
*     bit to start the setting from
*  SOC_SAND_IN  uint32  len -
*     number of bits to set the status for.
*  SOC_SAND_IN  uint8  occupied -
*     the status (occupied/unoccupied) to set for the give bits.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_sand_occ_bm_occup_status_set(
    SOC_SAND_INOUT  SOC_SAND_OCC_BM_PTR     bit_map,
    SOC_SAND_IN  uint32               place,
    SOC_SAND_IN  uint8              occupied
  )
{
  uint32
    res;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(SOC_SAND_OCC_BM_OCCUP_STATUS_SET);

  SOC_SAND_CHECK_NULL_INPUT(bit_map);

  res = soc_sand_occ_bm_occup_status_set_helper(
          bit_map,
          bit_map->nof_levels - 1,
          place / SOC_SAND_NOF_BITS_IN_CHAR,
          place % SOC_SAND_NOF_BITS_IN_CHAR,
          occupied
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_occ_bm_destroy()",0,0);

}


/*********************************************************************
* NAME:
*     soc_sand_occ_bm_occup_status_set
* TYPE:
*   PROC
* DATE:
*   Mar  6 2008
* FUNCTION:
*     Set the occupation status a of sequence of bits.
* INPUT:
*  SOC_SAND_IN  SOC_SAND_OCC_BM_PTR bit_map -
*     The bitmap to perform the get operation at.
*  SOC_SAND_IN  uint32  place -
*     bit to get the status (occupied/unoccupied) for.
*  SOC_SAND_OUT  uint8  *occupied -
*     the status (occupied/unoccupied) of the given bit
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_sand_occ_bm_is_occupied(
    SOC_SAND_IN  SOC_SAND_OCC_BM_PTR        bit_map,
    SOC_SAND_IN  uint32               place,
    SOC_SAND_OUT  uint8             *occupied
  )
{
  uint32
    char_indx,
    val,
    bit_indx;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(SOC_SAND_OCC_BM_GET_NEXT);

  SOC_SAND_CHECK_NULL_INPUT(bit_map);
  SOC_SAND_CHECK_NULL_INPUT(occupied);


  char_indx = place / SOC_SAND_NOF_BITS_IN_CHAR;
  val = SOC_SAND_OCC_BM_ACTIVE_INST(bit_map,levels)[bit_map->nof_levels - 1 ][char_indx];
  bit_indx = place % SOC_SAND_NOF_BITS_IN_CHAR;

  val = SOC_SAND_GET_BITS_RANGE(
          val,
          bit_indx,
          bit_indx
      );

  *occupied = SOC_SAND_NUM2BOOL(val);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_occ_bm_is_occupied()",0,0);
}

uint32
  soc_sand_occ_bm_cache_set(
    SOC_SAND_IN  SOC_SAND_OCC_BM_PTR        bit_map,
    SOC_SAND_IN  uint8             cached
  )
{
  uint32
    level_indx,
    level_size, res;
  int unit = BSL_UNIT_UNKNOWN;


  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(bit_map);


  if (!bit_map->support_cache && cached)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR, 10, exit);
  }
  /* if not changing status skip it */
  if (bit_map->cache_enabled == cached)
  {
    goto exit;
  }

  if(SOC_DPP_WB_ENGINE_VAR_NONE == bit_map->wb_var_index) 
  {
      bit_map->cache_enabled = cached;
  }
  else
  {
      unit = (bit_map->unit);

      res = SOC_DPP_WB_ENGINE_SET_VAR(unit, bit_map->wb_var_index + WB_ENGINE_OCC_BM_CACHE_ENABLED, &cached);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);      
  }

 /*
  * if cache enabled, copy status to cache instance
  */
  if (cached)
  {
      if(SOC_DPP_WB_ENGINE_VAR_NONE == bit_map->wb_var_index) 
      {   
          for (level_indx = 0; level_indx < bit_map->nof_levels; ++level_indx )
          {      
              level_size = (bit_map->levels_size[level_indx] + SOC_SAND_NOF_BITS_IN_CHAR - 1) / SOC_SAND_NOF_BITS_IN_CHAR;

              soc_sand_os_memcpy(bit_map->levels_cache[level_indx],bit_map->levels[level_indx],level_size * sizeof(uint8));
          }
      }
      else
      {
          unit = (bit_map->unit);

          res = SOC_DPP_WB_ENGINE_MEMCPY_ARR(unit, bit_map->wb_var_index + WB_ENGINE_OCC_BM_LEVELS_CACHE_BUFFER, 
                                             bit_map->levels_buffer,
                                             0,
                                             bit_map->buffer_size);
          SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
      }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_occ_bm_cache_set()",0,0);
}

uint32
  soc_sand_occ_bm_cache_commit(
    SOC_SAND_IN  SOC_SAND_OCC_BM_PTR        bit_map,
    SOC_SAND_IN  uint32               flags
  )
{
  uint32
    level_indx,
    level_size, res;
  int unit = BSL_UNIT_UNKNOWN;


  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(bit_map);


  if (!bit_map->support_cache)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR, 10, exit);
  }
 /*
  * if cached disabled
  */
  if (!bit_map->cache_enabled)
  {
    goto exit;
  }
 /*
  * if cache enabled, copy status to cache instance
  */
  if(SOC_DPP_WB_ENGINE_VAR_NONE == bit_map->wb_var_index) 
  {
      for (level_indx = 0; level_indx < bit_map->nof_levels; ++level_indx )
      {
          level_size = (bit_map->levels_size[level_indx] + SOC_SAND_NOF_BITS_IN_CHAR - 1) / SOC_SAND_NOF_BITS_IN_CHAR;

          soc_sand_os_memcpy(bit_map->levels[level_indx],bit_map->levels_cache[level_indx],level_size * sizeof(uint8));
      }
  }
  else
  {
      unit = (bit_map->unit);
      
      res = SOC_DPP_WB_ENGINE_MEMCPY_ARR(unit, bit_map->wb_var_index + WB_ENGINE_OCC_BM_LEVELS_BUFFER, 
                                         bit_map->levels_cache_buffer, 
                                         0,
                                         bit_map->buffer_size);
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_occ_bm_cache_commit()",0,0);
}

uint32
  soc_sand_occ_bm_cache_rollback(
    SOC_SAND_IN  SOC_SAND_OCC_BM_PTR        bit_map,
    SOC_SAND_IN  uint32              flags
  )
{
  uint32
    level_indx,
    level_size;
  int unit = BSL_UNIT_UNKNOWN, res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(bit_map);


  if (!bit_map->support_cache)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR, 10, exit);
  }
 /*
  * if cached disabled
  */
  if (!bit_map->cache_enabled)
  {
    goto exit;
  }
 /*
  * if cache enabled, copy status to cache instance
  */
  if(SOC_DPP_WB_ENGINE_VAR_NONE == bit_map->wb_var_index) 
  {
      for (level_indx = 0; level_indx < bit_map->nof_levels; ++level_indx )
      {
          level_size = (bit_map->levels_size[level_indx] + SOC_SAND_NOF_BITS_IN_CHAR - 1) / SOC_SAND_NOF_BITS_IN_CHAR;

          soc_sand_os_memcpy(bit_map->levels_cache[level_indx],bit_map->levels[level_indx],level_size * sizeof(uint8));
      }
  }
  else
  {
      unit = (bit_map->unit);

      res = SOC_DPP_WB_ENGINE_MEMCPY_ARR(unit, bit_map->wb_var_index + WB_ENGINE_OCC_BM_LEVELS_CACHE_BUFFER, 
                                         bit_map->levels_buffer,
                                         0,
                                         bit_map->buffer_size);
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_occ_bm_cache_rollback()",0,0);
}

uint32
  soc_sand_occ_bm_get_size_for_save(
    SOC_SAND_IN  SOC_SAND_OCC_BM_PTR               bit_map,
    SOC_SAND_OUT  uint32                     *size
  )
{
  uint32
    level_indx,
    level_size;
  uint32
    cur_size=0;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(bit_map);
  
  cur_size += sizeof(uint8)*2;
  cur_size += sizeof(uint32)*1;


  /* copy DS data */
  for (level_indx = 0; level_indx < bit_map->nof_levels; ++level_indx )
  {
    level_size = (bit_map->levels_size[level_indx] + SOC_SAND_NOF_BITS_IN_CHAR - 1) / SOC_SAND_NOF_BITS_IN_CHAR;
    cur_size += level_size * sizeof(uint8);
  }

  if (bit_map->cache_enabled)
  {
    for (level_indx = 0; level_indx < bit_map->nof_levels; ++level_indx )
    {
      level_size = (bit_map->levels_size[level_indx] + SOC_SAND_NOF_BITS_IN_CHAR - 1) / SOC_SAND_NOF_BITS_IN_CHAR;
      cur_size += level_size * sizeof(uint8);
    }
  }
  

  *size = cur_size;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_occ_bm_save()",0,0);
}

uint32
  soc_sand_occ_bm_save(
    SOC_SAND_IN  SOC_SAND_OCC_BM_PTR          bit_map,
    SOC_SAND_OUT  uint8                 *buffer,
    SOC_SAND_IN  uint32                 buffer_size_bytes,
    SOC_SAND_OUT uint32                 *actual_size_bytes
  )
{
  uint32
    level_indx,
    level_size;
  uint8
    *cur_ptr = (uint8*)buffer;
  uint32
    cur_size;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(bit_map);
  
  /* copy init info */
  soc_sand_os_memcpy(cur_ptr, &(bit_map->init_val),sizeof(uint8));
  cur_ptr += sizeof(uint8);

  soc_sand_os_memcpy(cur_ptr, &(bit_map->size),sizeof(uint32));
  cur_ptr += sizeof(uint32); 

  soc_sand_os_memcpy(cur_ptr, &(bit_map->support_cache),sizeof(uint8));
  cur_ptr += sizeof(uint8);

  /* copy DS data */
  for (level_indx = 0; level_indx < bit_map->nof_levels; ++level_indx )
  {
    level_size = (bit_map->levels_size[level_indx] + SOC_SAND_NOF_BITS_IN_CHAR - 1) / SOC_SAND_NOF_BITS_IN_CHAR;
    cur_size = level_size * sizeof(uint8);
    soc_sand_os_memcpy(cur_ptr, bit_map->levels[level_indx],cur_size);
    cur_ptr += cur_size;
  }

  if (bit_map->cache_enabled)
  {
    for (level_indx = 0; level_indx < bit_map->nof_levels; ++level_indx )
    {
      level_size = (bit_map->levels_size[level_indx] + SOC_SAND_NOF_BITS_IN_CHAR - 1) / SOC_SAND_NOF_BITS_IN_CHAR;
      cur_size = level_size * sizeof(uint8);
      soc_sand_os_memcpy(cur_ptr, bit_map->levels_cache[level_indx],cur_size);
      cur_ptr += cur_size;
    }
  }
  *actual_size_bytes = cur_ptr - buffer;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_occ_bm_save()",0,0);
}



uint32
  soc_sand_occ_bm_load(
    SOC_SAND_IN  uint8                           **buffer,
    SOC_SAND_OUT  SOC_SAND_OCC_BM_PTR                  *bit_map
  )
{
  SOC_SAND_OCC_BM_INIT_INFO
    bm_restored;
  uint32
    level_indx,
    level_size;
  SOC_SAND_IN uint8
    *cur_ptr = (SOC_SAND_IN uint8*)buffer[0];
  uint32
    cur_size;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  
  soc_sand_SAND_OCC_BM_INIT_INFO_clear(&bm_restored);

  /* copy init info */
  soc_sand_os_memcpy(&(bm_restored.init_val), cur_ptr, sizeof(uint8));
  cur_ptr += sizeof(uint8);

  soc_sand_os_memcpy(&(bm_restored.size), cur_ptr, sizeof(uint32));
  cur_ptr += sizeof(uint32);
 
  soc_sand_os_memcpy(&(bm_restored.support_cache), cur_ptr, sizeof(uint8));
  cur_ptr += sizeof(uint8);


  /* create DS */
  res = soc_sand_occ_bm_create(&bm_restored, bit_map);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
          
  /* fill DS info*/
  for (level_indx = 0; level_indx < (*bit_map)->nof_levels; ++level_indx )
  {
    level_size = ((*bit_map)->levels_size[level_indx] + SOC_SAND_NOF_BITS_IN_CHAR - 1) / SOC_SAND_NOF_BITS_IN_CHAR;
    cur_size = level_size * sizeof(uint8);
    soc_sand_os_memcpy((*bit_map)->levels[level_indx], cur_ptr, cur_size);
    cur_ptr += cur_size;
  }

  if ((*bit_map)->cache_enabled)
  {
      for (level_indx = 0; level_indx < (*bit_map)->nof_levels; ++level_indx )
      {
          level_size = ((*bit_map)->levels_size[level_indx] + SOC_SAND_NOF_BITS_IN_CHAR - 1) / SOC_SAND_NOF_BITS_IN_CHAR;
          cur_size = level_size * sizeof(uint8);
          soc_sand_os_memcpy((*bit_map)->levels_cache[level_indx], cur_ptr, cur_size);
          cur_ptr += cur_size;
      }
  }
    
  *buffer = cur_ptr;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_occ_bm_load()",0,0);
}


void
  soc_sand_SAND_OCC_BM_INIT_INFO_clear(
    SOC_SAND_INOUT SOC_SAND_OCC_BM_INIT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_memset(info, 0x0, sizeof(SOC_SAND_OCC_BM_INIT_INFO));
  info->size          = 0;
  info->init_val      = FALSE;
  info->support_cache = FALSE;
  info->wb_var_index  = SOC_DPP_WB_ENGINE_VAR_NONE;
  info->unit     = -1; /* not relevan when wb_var_index=SOC_DPP_WB_ENGINE_VAR_NONE */

  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_SAND_DEBUG

uint32
  soc_sand_occ_bm_print(
    SOC_SAND_IN  SOC_SAND_OCC_BM_PTR                  bit_map
  )
{
uint32
  char_indx,
  level_indx,
  level_size;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(SOC_SAND_OCC_BM_PRINT);
  SOC_SAND_CHECK_NULL_INPUT(bit_map);

  for (level_indx = 0; level_indx < bit_map->nof_levels; ++level_indx )
  {
    level_size = (bit_map->levels_size[level_indx] + SOC_SAND_NOF_BITS_IN_CHAR - 1) / SOC_SAND_NOF_BITS_IN_CHAR;
    LOG_CLI((BSL_META_U(unit,
                        "level %u:"),level_indx));
    for (char_indx = 0; char_indx < level_size; ++char_indx )
    {
      LOG_CLI((BSL_META_U(unit,
                          "%02x  "), (uint8)bit_map->levels[level_indx][char_indx]));
    }
    LOG_CLI((BSL_META_U(unit,
                        "\n")));
  }
  LOG_CLI((BSL_META_U(unit,
                      "\n\n\n")));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_occ_bm_destroy()",0,0);

}



/*****************************************************
*NAME
*  soc_sand_occ_bm_TEST_1
*TYPE:
*  PROC
*DATE:
*  15-Feb-2008
*FUNCTION:
* Verification that a MC-Group that requires more than available will fail to open
* 1.  Open many large groups (8*2K Ingress) and (7*2K Egress) MC-Groups.
*     And open one more Egress MC-Group with 1 entry (MC-ID = 16K-2K = 14K).
* 2.  Then try and open an Ingress MC-Group with 2K entries (MC-ID = 1).
*     Expect INSUFFICIENT MEMORY value to be TRUE.
* 3.  Delete the Egress group with 1 entry (MC-ID = 14K).
* 4.  Open an Ingress MC-Group with 2K entries (MC-ID = 1). Expect success.
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
uint32
soc_sand_occ_bm_TEST_1(
  SOC_SAND_IN int unit,
  SOC_SAND_IN uint8 silent
)
{
  int32
    pass = TRUE;
  uint32
    indx,
    place;
  uint8
    found;
  SOC_SAND_OCC_BM_PTR
    bitmap;
  SOC_SAND_OCC_BM_INIT_INFO
    btmp_init_info;
  uint32
    soc_sand_ret;

  soc_sand_SAND_OCC_BM_INIT_INFO_clear(&btmp_init_info);
  btmp_init_info.size = 64;


  soc_sand_ret = soc_sand_occ_bm_create(
               &btmp_init_info,
               &bitmap
             );
  if(soc_sand_get_error_code_from_error_word(soc_sand_ret) != SOC_SAND_OK)
  {
    goto exit;
  }

  for(indx = 0 ; indx < 66; ++indx)
  {
    soc_sand_ret = soc_sand_occ_bm_get_next(
                 bitmap,
                 &place,
                 &found
               );
    if(soc_sand_get_error_code_from_error_word(soc_sand_ret) != SOC_SAND_OK)
    {
      goto exit;
    }
    if (!silent)
    {
      LOG_INFO(BSL_LS_SOC_COMMON,
               (BSL_META_U(unit,
                           "found %u \n"),place));
      if (found)
      {
        soc_sand_occ_bm_print(bitmap);
      }
    }

    soc_sand_ret = soc_sand_occ_bm_occup_status_set(
                 bitmap,
                 place,
                 TRUE
               );
    if(soc_sand_get_error_code_from_error_word(soc_sand_ret) != SOC_SAND_OK)
    {
      goto exit;
    }

  }

exit:
  if (bitmap)
  {
    soc_sand_ret = soc_sand_occ_bm_destroy(
               bitmap
             );

    if(soc_sand_get_error_code_from_error_word(soc_sand_ret) != SOC_SAND_OK)
    {
        LOG_INFO(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "Failed in soc_sand_occ_bm_destroy\n")));
        pass = FALSE;
    }
  }
  if (!silent)
  {
    LOG_INFO(BSL_LS_SOC_COMMON,
             (BSL_META_U(unit,
                         "\n\r ********** soc_sand_occ_bm_TEST_1 : END **********\n")));
  }
  return pass;
}



uint32
  soc_sand_occ_bm_TEST_2(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 silent
)
{
  int32
    pass = TRUE;
  uint32
    indx,
    place;
  uint32
    expected_places[5];
  uint8
    found;
  SOC_SAND_OCC_BM_PTR
    bitmap;
  SOC_SAND_OCC_BM_INIT_INFO
    btmp_init_info;
  uint32
    soc_sand_ret;

  soc_sand_SAND_OCC_BM_INIT_INFO_clear(&btmp_init_info);
  btmp_init_info.size = 64;

  soc_sand_ret = soc_sand_occ_bm_create(
               &btmp_init_info,
               &bitmap
             );
  if(soc_sand_get_error_code_from_error_word(soc_sand_ret) != SOC_SAND_OK)
  {
    goto exit;
  }

  for(indx = 0 ; indx < 66; ++indx)
  {
    soc_sand_ret = soc_sand_occ_bm_get_next(
                 bitmap,
                 &place,
                 &found
               );
    if(soc_sand_get_error_code_from_error_word(soc_sand_ret) != SOC_SAND_OK)
    {
      goto exit;
    }
    if (!silent)
    {
      LOG_INFO(BSL_LS_SOC_COMMON,
               (BSL_META_U(unit,
                           "found %u \n"),place));
      if (found)
      {
        soc_sand_occ_bm_print(bitmap);
      }
    }

    soc_sand_ret = soc_sand_occ_bm_occup_status_set(
                 bitmap,
                 place,
                 TRUE
               );
    if(soc_sand_get_error_code_from_error_word(soc_sand_ret) != SOC_SAND_OK)
    {
      goto exit;
    }
  }
 /*
  * free one bit on 20
  */
  soc_sand_ret = soc_sand_occ_bm_occup_status_set(
               bitmap,
               20,
               FALSE
             );
  if(soc_sand_get_error_code_from_error_word(soc_sand_ret) != SOC_SAND_OK)
  {
    goto exit;
  }
 /*
  * get next free place (has to be 20)
  */
  soc_sand_ret = soc_sand_occ_bm_get_next(
             bitmap,
             &place,
             &found
           );
  if(soc_sand_get_error_code_from_error_word(soc_sand_ret) != SOC_SAND_OK)
  {
    goto exit;
  }
  if (!silent)
  {
    LOG_INFO(BSL_LS_SOC_COMMON,
             (BSL_META_U(unit,
                         "found %u \n"),place));
    if (found)
    {
      soc_sand_occ_bm_print(bitmap);
    }
  }

  if (place != 20 )
  {
    pass = FALSE;
    if (!silent)
    {
      LOG_INFO(BSL_LS_SOC_COMMON,
               (BSL_META_U(unit,
                           "found %u instead of 20\n"),place));
    }
  }

  soc_sand_ret = soc_sand_occ_bm_occup_status_set(
               bitmap,
               place,
               TRUE
             );
  if(soc_sand_get_error_code_from_error_word(soc_sand_ret) != SOC_SAND_OK)
  {
    goto exit;
  }
 /*
  * free 4 bits on 63, 0, 15, 32
  */
  expected_places [0] = 0;
  expected_places [1] = 15;
  expected_places [2] = 32;
  expected_places [3] = 63;
  expected_places [4] = 0;

  soc_sand_ret = soc_sand_occ_bm_occup_status_set(
               bitmap,
               63,
               FALSE
             );
  if(soc_sand_get_error_code_from_error_word(soc_sand_ret) != SOC_SAND_OK)
  {
    goto exit;
  }
  soc_sand_ret = soc_sand_occ_bm_occup_status_set(
               bitmap,
               0,
               FALSE
             );
  if(soc_sand_get_error_code_from_error_word(soc_sand_ret) != SOC_SAND_OK)
  {
    goto exit;
  }
  soc_sand_ret = soc_sand_occ_bm_occup_status_set(
               bitmap,
               15,
               FALSE
             );
  if(soc_sand_get_error_code_from_error_word(soc_sand_ret) != SOC_SAND_OK)
  {
    goto exit;
  }
  soc_sand_ret = soc_sand_occ_bm_occup_status_set(
               bitmap,
               32,
               FALSE
             );
  if(soc_sand_get_error_code_from_error_word(soc_sand_ret) != SOC_SAND_OK)
  {
    goto exit;
  }

 /*
  * allocate 5 bits 4 should be 63, 0, 15, 32 and one not found.
  */
  for(indx = 0 ; indx < 5; ++indx)
  {
    soc_sand_ret = soc_sand_occ_bm_get_next(
                 bitmap,
                 &place,
                 &found
               );
    if(soc_sand_get_error_code_from_error_word(soc_sand_ret) != SOC_SAND_OK)
    {
      goto exit;
    }
    if (!silent)
    {
      LOG_INFO(BSL_LS_SOC_COMMON,
               (BSL_META_U(unit,
                           "found %u \n"),place));
      if (found)
      {
        soc_sand_occ_bm_print(bitmap);
      }
    }
    if (place != expected_places[indx] )
    {
      pass = FALSE;
      if (!silent)
      {
        LOG_INFO(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "found %u instead of %u\n"),place, expected_places[indx]));
      }
    }

    soc_sand_ret = soc_sand_occ_bm_occup_status_set(
                 bitmap,
                 place,
                 TRUE
               );
    if(soc_sand_get_error_code_from_error_word(soc_sand_ret) != SOC_SAND_OK)
    {
      goto exit;
    }
  }

exit:
  if (bitmap)
  {
    soc_sand_ret = soc_sand_occ_bm_destroy(
               bitmap
             );

    if(soc_sand_get_error_code_from_error_word(soc_sand_ret) != SOC_SAND_OK)
    {
        LOG_INFO(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "Failed in soc_sand_occ_bm_destroy\n")));
        pass = FALSE;
    }
  }
  if (!silent)
  {
    LOG_INFO(BSL_LS_SOC_COMMON,
             (BSL_META_U(unit,
                         "\n\r ********** soc_sand_occ_bm_TEST_2 : END **********\n")));
  }
  return pass;
}



uint32
  soc_sand_occ_bm_TEST_3(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 silent
)
{
  int32
    pass = TRUE;
  uint32
    indx,
    place;
  uint32
    expected_places[5];
  uint8
    found;
  SOC_SAND_OCC_BM_PTR
    bitmap;
  SOC_SAND_OCC_BM_INIT_INFO
    btmp_init_info;
  uint32
    soc_sand_ret;


  soc_sand_SAND_OCC_BM_INIT_INFO_clear(&btmp_init_info);
  btmp_init_info.size = 700;

  soc_sand_ret = soc_sand_occ_bm_create(
                 &btmp_init_info,
               &bitmap
             );
  if(soc_sand_get_error_code_from_error_word(soc_sand_ret) != SOC_SAND_OK)
  {
    goto exit;
  }

  for(indx = 0 ; indx < 702; ++indx)
  {
    soc_sand_ret = soc_sand_occ_bm_get_next(
                 bitmap,
                 &place,
                 &found
               );
    if(soc_sand_get_error_code_from_error_word(soc_sand_ret) != SOC_SAND_OK)
    {
      goto exit;
    }
    if (!silent)
    {
      LOG_INFO(BSL_LS_SOC_COMMON,
               (BSL_META_U(unit,
                           "found %u \n"),place));
      if (found)
      {
        soc_sand_occ_bm_print(bitmap);
      }
    }
    if ((indx < 700 && place != indx) || (indx >= 700 && found))
    {
      pass = FALSE;
      if (!silent)
      {
        LOG_INFO(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "found %u instead of %u\n"),place, indx));
      }
    }

    soc_sand_ret = soc_sand_occ_bm_occup_status_set(
                 bitmap,
                 place,
                 TRUE
               );
    if(soc_sand_get_error_code_from_error_word(soc_sand_ret) != SOC_SAND_OK)
    {
      goto exit;
    }
  }
 /*
  * free one bit on 20
  */
  soc_sand_ret = soc_sand_occ_bm_occup_status_set(
               bitmap,
               200,
               FALSE
             );
  if(soc_sand_get_error_code_from_error_word(soc_sand_ret) != SOC_SAND_OK)
  {
    goto exit;
  }
 /*
  * get next free place (has to be 200)
  */
  soc_sand_ret = soc_sand_occ_bm_get_next(
             bitmap,
             &place,
             &found
           );
  if(soc_sand_get_error_code_from_error_word(soc_sand_ret) != SOC_SAND_OK)
  {
    goto exit;
  }
  if (!silent)
  {
    LOG_INFO(BSL_LS_SOC_COMMON,
             (BSL_META_U(unit,
                         "found %u \n"),place));
    if (found)
    {
      soc_sand_occ_bm_print(bitmap);
    }
  }

  if (place != 200 )
  {
    pass = FALSE;
    if (!silent)
    {
      LOG_INFO(BSL_LS_SOC_COMMON,
               (BSL_META_U(unit,
                           "found %u instead of 200\n"),place));
    }
  }

  soc_sand_ret = soc_sand_occ_bm_occup_status_set(
               bitmap,
               place,
               TRUE
             );
  if(soc_sand_get_error_code_from_error_word(soc_sand_ret) != SOC_SAND_OK)
  {
    goto exit;
  }
 /*
  * free 4 bits on 630, 0, 150, 320
  */
  expected_places [0] = 0;
  expected_places [1] = 150;
  expected_places [2] = 320;
  expected_places [3] = 630;
  expected_places [4] = 0;

  soc_sand_ret = soc_sand_occ_bm_occup_status_set(
               bitmap,
               630,
               FALSE
             );
  if(soc_sand_get_error_code_from_error_word(soc_sand_ret) != SOC_SAND_OK)
  {
    goto exit;
  }
  soc_sand_ret = soc_sand_occ_bm_occup_status_set(
               bitmap,
               0,
               FALSE
             );
  if(soc_sand_get_error_code_from_error_word(soc_sand_ret) != SOC_SAND_OK)
  {
    goto exit;
  }
  soc_sand_ret = soc_sand_occ_bm_occup_status_set(
               bitmap,
               150,
               FALSE
             );
  if(soc_sand_get_error_code_from_error_word(soc_sand_ret) != SOC_SAND_OK)
  {
    goto exit;
  }
  soc_sand_ret = soc_sand_occ_bm_occup_status_set(
               bitmap,
               320,
               FALSE
             );
  if(soc_sand_get_error_code_from_error_word(soc_sand_ret) != SOC_SAND_OK)
  {
    goto exit;
  }

 /*
  * allocate 5 bits 4 should be 630, 0, 150, 320 and one not found.
  */
  for(indx = 0 ; indx < 5; ++indx)
  {
    soc_sand_ret = soc_sand_occ_bm_get_next(
                 bitmap,
                 &place,
                 &found
               );
    if(soc_sand_get_error_code_from_error_word(soc_sand_ret) != SOC_SAND_OK)
    {
      goto exit;
    }
    if (!silent)
    {
      LOG_INFO(BSL_LS_SOC_COMMON,
               (BSL_META_U(unit,
                           "found %u \n"),place));
      if (found)
      {
        soc_sand_occ_bm_print(bitmap);
      }
    }
    if (place != expected_places[indx] )
    {
      pass = FALSE;
      if (!silent)
      {
        LOG_INFO(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "found %u instead of %u\n"),place, expected_places[indx]));
      }
    }

    soc_sand_ret = soc_sand_occ_bm_occup_status_set(
                 bitmap,
                 place,
                 TRUE
               );
    if(soc_sand_get_error_code_from_error_word(soc_sand_ret) != SOC_SAND_OK)
    {
      goto exit;
    }
  }

exit:
  if (bitmap)
  {
    soc_sand_ret = soc_sand_occ_bm_destroy(
               bitmap
             );

    if(soc_sand_get_error_code_from_error_word(soc_sand_ret) != SOC_SAND_OK)
    {
        LOG_INFO(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "Failed in soc_sand_occ_bm_destroy\n")));
        pass = FALSE;
    }
  }
  if (!silent)
  {
    LOG_INFO(BSL_LS_SOC_COMMON,
             (BSL_META_U(unit,
                         "\n\r ********** soc_sand_occ_bm_TEST_3 : END **********\n")));
  }
  return pass;
}



uint32
  soc_sand_occ_bm_TEST_4(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 silent
)
{
  int32
    pass = TRUE;
  uint32
    indx,
    place,
    free_place;
  uint8
    found;
  SOC_SAND_OCC_BM_PTR
    bitmap;
  SOC_SAND_OCC_BM_INIT_INFO
    btmp_init_info;
  uint32
    soc_sand_ret;

  soc_sand_SAND_OCC_BM_INIT_INFO_clear(&btmp_init_info);
  btmp_init_info.size = 700;

  soc_sand_ret = soc_sand_occ_bm_create(
               &btmp_init_info,
               &bitmap
             );
  if(soc_sand_get_error_code_from_error_word(soc_sand_ret) != SOC_SAND_OK)
  {
    goto exit;
  }

  for(indx = 0 ; indx < 702; ++indx)
  {
    soc_sand_ret = soc_sand_occ_bm_get_next(
                 bitmap,
                 &place,
                 &found
               );
    if(soc_sand_get_error_code_from_error_word(soc_sand_ret) != SOC_SAND_OK)
    {
      goto exit;
    }
    if (!silent)
    {
      LOG_INFO(BSL_LS_SOC_COMMON,
               (BSL_META_U(unit,
                           "found %u \n"),place));
      if (found)
      {
        soc_sand_occ_bm_print(bitmap);
      }
    }
    if ((indx < 700 && place != indx) || (indx >= 700 && found))
    {
      pass = FALSE;
      if (!silent)
      {
        LOG_INFO(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "found %u instead of %u\n"),place, indx));
      }
    }

    soc_sand_ret = soc_sand_occ_bm_occup_status_set(
                 bitmap,
                 place,
                 TRUE
               );
    if(soc_sand_get_error_code_from_error_word(soc_sand_ret) != SOC_SAND_OK)
    {
      goto exit;
    }
  }
  for (indx = 0; indx < 700; ++indx)
  {
   /*
    * free one bit one by one
    */
    free_place = indx;
    soc_sand_ret = soc_sand_occ_bm_occup_status_set(
                 bitmap,
                 free_place,
                 FALSE
               );
    if(soc_sand_get_error_code_from_error_word(soc_sand_ret) != SOC_SAND_OK)
    {
      goto exit;
    }
   /*
    * get next free place (has to be free_place)
    */
    soc_sand_ret = soc_sand_occ_bm_get_next(
               bitmap,
               &place,
               &found
             );
    if(soc_sand_get_error_code_from_error_word(soc_sand_ret) != SOC_SAND_OK)
    {
      goto exit;
    }
    if (!silent)
    {
      LOG_INFO(BSL_LS_SOC_COMMON,
               (BSL_META_U(unit,
                           "found %u \n"),place));
      if (found)
      {
        soc_sand_occ_bm_print(bitmap);
      }
    }

    if (place != free_place )
    {
      pass = FALSE;
      if (!silent)
      {
        LOG_INFO(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "found %u instead of %u\n"),place,free_place));
      }
    }

    soc_sand_ret = soc_sand_occ_bm_occup_status_set(
                 bitmap,
                 place,
                 TRUE
               );
    if(soc_sand_get_error_code_from_error_word(soc_sand_ret) != SOC_SAND_OK)
    {
      goto exit;
    }
  }
exit:
  if (bitmap)
  {
    soc_sand_ret = soc_sand_occ_bm_destroy(
               bitmap
             );

    if(soc_sand_get_error_code_from_error_word(soc_sand_ret) != SOC_SAND_OK)
    {
        LOG_INFO(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "Failed in soc_sand_occ_bm_destroy\n")));
        pass = FALSE;
    }
  }
  if (!silent)
  {
    LOG_INFO(BSL_LS_SOC_COMMON,
             (BSL_META_U(unit,
                         "\n\r ********** soc_sand_occ_bm_TEST_4 : END **********\n")));
  }
  return pass;
}

uint32
  soc_sand_occ_bm_tests(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 silent
)
{
  int32
    pass = TRUE;

  pass &= soc_sand_occ_bm_TEST_1(
               unit,
               silent
             );

  pass &= soc_sand_occ_bm_TEST_2(
               unit,
               silent
             );

  pass &= soc_sand_occ_bm_TEST_3(
               unit,
               silent
             );


  pass &= soc_sand_occ_bm_TEST_4(
               unit,
               silent
             );
  if (!silent)
  {
    LOG_INFO(BSL_LS_SOC_COMMON,
             (BSL_META_U(unit,
                         "\n\r ********** soc_sand_occ_bm_tests : END **********\n")));
  }
  return pass;
}


#endif /* SOC_SAND_DEBUG */

#include <soc/dpp/SAND/Utils/sand_footer.h>
