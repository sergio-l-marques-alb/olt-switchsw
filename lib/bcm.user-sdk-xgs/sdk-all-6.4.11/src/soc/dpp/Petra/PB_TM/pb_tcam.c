/* $Id: pb_tcam.c,v 1.14 Broadcom SDK $
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


/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/Petra/PB_TM/pb_framework.h>
#include <soc/dpp/Petra/PB_TM/pb_tcam.h>

#include <soc/dpp/SAND/Utils/sand_bitstream.h>
#include <soc/dpp/SAND/Utils/sand_occupation_bitmap.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_tbl_access.h>
#include <soc/dpp/Petra/PB_TM/pb_tcam_mgmt.h>
#include <soc/dpp/Petra/PB_TM/pb_sw_db_tcam_mgmt.h>
#include <soc/dpp/Petra/PB_TM/pb_api_general.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_chip_regs.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_reg_access.h>


/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PB_TCAM_BANK_ID_MAX                                    (SOC_SAND_UINT_MAX)
#define SOC_PB_TCAM_ENTRY_SIZE_MAX                                 (SOC_PB_TCAM_NOF_BANK_ENTRY_SIZES-1)
#define SOC_PB_TCAM_TCAM_DB_ID_MAX                                 (SOC_PB_TCAM_MAX_NOF_LISTS)
#define SOC_PB_TCAM_PREFIX_SIZE_MAX                                (4)
#define SOC_PB_TCAM_PRIO_MODE_MAX                                  (SOC_PB_TCAM_NOF_DB_PRIO_MODES-1)
#define SOC_PB_TCAM_ENTRY_ID_MAX                                   (SOC_SAND_UINT_MAX)
#define SOC_PB_TCAM_BITS_MAX                                       (SOC_SAND_U32_MAX)
#define SOC_PB_TCAM_LENGTH_MAX                                     (SOC_SAND_UINT_MAX)
#define SOC_PB_TCAM_ENTRY_MAX                                      (SOC_SAND_UINT_MAX)

#define SOC_PB_TCAM_BANK_NOF_LINES                                 (512)
#define SOC_PB_TCAM_WORD_SIZE_IN_BITS                              (144)
#define SOC_PB_TCAM_DB_LIST_KEY_SIZE                               (2)
#define SOC_PB_TCAM_DB_LIST_DATA_SIZE                              (sizeof(uint32) + 2)
#define SOC_PB_TCAM_DB_HASH_TBL_KEY_SIZE                           (sizeof(uint32))

/* } */
/*************
 * MACROS    *
 *************/
/* { */

#define SOC_PB_TCAM_DB_EXISTS_ASSERT                           \
  {                                                        \
    res = soc_pb_tcam_db_exists_assert(unit, tcam_db_id); \
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);                 \
  }

/* } */
/*************
 * TYPE DEFS *
 *************/
/* { */

typedef enum
{
  SOC_PB_TCAM_DIRECTION_FORWARD,
  SOC_PB_TCAM_DIRECTION_BACKWARD
} SOC_PB_TCAM_DIRECTION;

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

/* $Id: pb_tcam.c,v 1.14 Broadcom SDK $
 *  Internal functions
 */

/*
 *  TCAM database priority list helper functions
 */
STATIC
  uint32
    soc_pb_tcam_db_location_encode(
      SOC_SAND_IN  SOC_PB_TCAM_LOCATION *location,
      SOC_SAND_OUT uint8          *key
    )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  if ((location->entry & ~((uint32) 0x3ff)) != 0)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_TCAM_DB_ENTRY_NDX_OUT_OF_RANGE_ERR, 10, exit);
  }

  key[0] = (uint8) location->entry & 0xff;
  key[1] = (uint8) ((location->bank_id << 2) | (location->entry >> 8));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tcam_db_priority_list_key_build", 0, 0);
}

STATIC
  void
    soc_pb_tcam_db_location_decode(
      SOC_SAND_IN  uint8          *key,
      SOC_SAND_OUT SOC_PB_TCAM_LOCATION *location
    )
{
  location->entry   = (((uint32) key[1] & 0x3) << 8) | key[0];
  location->bank_id = key[1] >> 2;
}

STATIC
  void
    soc_pb_tcam_db_priority_encode(
      SOC_SAND_IN  uint16 priority,
      SOC_SAND_OUT uint8  *data
    )
{
  data[4] = (uint8) (priority & 0xff);
  data[5] = (uint8) (priority >> 8);
}

STATIC
  uint16
    soc_pb_tcam_db_priority_decode(
      SOC_SAND_IN uint8 *data
    )
{
  return (((uint16) data[5]) << 8) | data[4];
}

STATIC
  void
    soc_pb_tcam_db_entry_id_encode(
      SOC_SAND_IN  uint32 entry_id,
      SOC_SAND_OUT uint8   *data
    )
{
  uint32
    value;

  value = (uint32)entry_id;
  soc_sand_U32_to_U8(
    &value,
    sizeof(uint32),
    data
  );
}

STATIC
  uint32
    soc_pb_tcam_db_entry_id_decode(
      SOC_SAND_IN uint8 *data
    )
{
  uint32
    entry_id;
  uint32
    value;

  soc_sand_U8_to_U32(
    data,
    sizeof(uint32),
    &value
  );
  entry_id = (uint32)value;
  return entry_id;
}

STATIC
  int32
    soc_pb_tcam_location_cmp_bank(
      SOC_SAND_IN SOC_PB_TCAM_LOCATION *lhs,
      SOC_SAND_IN SOC_PB_TCAM_LOCATION *rhs
    )
{
  int32
    ret;

  if (lhs->bank_id != rhs->bank_id)
  {
    ret = (int32) lhs->bank_id - (int32) rhs->bank_id;
  }
  else
  {
    ret = (int32) lhs->entry - (int32) rhs->entry;
  }

  return ret;
}

STATIC
  int32
    soc_pb_tcam_location_cmp_interlaced(
      SOC_SAND_IN SOC_PB_TCAM_LOCATION *lhs,
      SOC_SAND_IN SOC_PB_TCAM_LOCATION *rhs
    )
{
  int32
    ret;

  if (lhs->entry != rhs->entry)
  {
    ret = (int32) lhs->entry - (int32) rhs->entry;
  }
  else
  {
    ret = (int32) lhs->bank_id - (int32) rhs->bank_id;
  }

  return ret;
}

STATIC
  int32
    soc_pb_tcam_location_cmp(
      SOC_SAND_IN int        unit,
      SOC_SAND_IN uint32        tcam_db_id,
      SOC_SAND_IN SOC_PB_TCAM_LOCATION *lhs,
      SOC_SAND_IN SOC_PB_TCAM_LOCATION *rhs
    )
{
  int32
    ret;
  SOC_PB_TCAM_DB_PRIO_MODE
    prio_mode;

  prio_mode = soc_pb_sw_db_tcam_db_priority_mode_get(
                unit,
                tcam_db_id
              );

  if (prio_mode == SOC_PB_TCAM_DB_PRIO_MODE_BANK)
  {
    ret = soc_pb_tcam_location_cmp_bank(
            lhs,
            rhs
          );
  }
  else
  {
    ret = soc_pb_tcam_location_cmp_interlaced(
            lhs,
            rhs
          );
  }

  return ret;
}


  int32
    soc_pb_tcam_db_priority_list_cmp_bank(
      SOC_SAND_IN uint8  *buffer1,
      SOC_SAND_IN uint8  *buffer2,
              uint32 size
    )
{
  SOC_PB_TCAM_LOCATION
    lhs,
    rhs;

  soc_pb_tcam_db_location_decode(buffer1, &lhs);
  soc_pb_tcam_db_location_decode(buffer2, &rhs);

  return soc_pb_tcam_location_cmp_bank(&lhs, &rhs);
}


  int32
    soc_pb_tcam_db_priority_list_cmp_interlaced(
      SOC_SAND_IN uint8  *buffer1,
      SOC_SAND_IN uint8  *buffer2,
              uint32 size
    )
{
  SOC_PB_TCAM_LOCATION
    lhs,
    rhs;

  soc_pb_tcam_db_location_decode(buffer1, &lhs);
  soc_pb_tcam_db_location_decode(buffer2, &rhs);

  return soc_pb_tcam_location_cmp_interlaced(&lhs, &rhs);
}


  uint32
    soc_pb_tcam_db_data_structure_entry_set(
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
    soc_pb_tcam_db_data_structure_entry_get(
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

STATIC
  uint32
    soc_pb_tcam_bank_entry_size_to_entry_count_get(
      SOC_SAND_IN  SOC_PB_TCAM_BANK_ENTRY_SIZE entry_size
    )
{
  uint32
    ret = 0;

  switch (entry_size)
  {
  case SOC_PB_TCAM_BANK_ENTRY_SIZE_72_BITS:
    ret = 2 * SOC_PB_TCAM_BANK_NOF_LINES;
    break;

  case SOC_PB_TCAM_BANK_ENTRY_SIZE_144_BITS:
    ret = SOC_PB_TCAM_BANK_NOF_LINES;
    break;

  case SOC_PB_TCAM_BANK_ENTRY_SIZE_288_BITS:
    ret = SOC_PB_TCAM_BANK_NOF_LINES / 2;
    break;
  default:
    break;
  }

  return ret;
}

STATIC
  uint32
    soc_pb_tcam_db_entries_per_bank_get(
      SOC_SAND_IN int unit,
      SOC_SAND_IN uint32 tcam_db_id
    )
{
  return soc_pb_tcam_bank_entry_size_to_entry_count_get(
           soc_pb_sw_db_tcam_db_entry_size_get(
             unit,
             tcam_db_id
           )
         );
}

/*
 *  Hardware access functions
 */
STATIC
  uint32
    soc_pb_tcam_entry_word_extract(
      SOC_SAND_IN  SOC_PB_TCAM_ENTRY                *entry,
      SOC_SAND_IN  uint8                    take_lsb,
      SOC_SAND_OUT SOC_PB_PP_IHB_TCAM_BANK_TBL_DATA *tbl_data
    )
{
  uint32
    first_bit,
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  first_bit = take_lsb ? 0 : SOC_PB_TCAM_WORD_SIZE_IN_BITS;
  res = soc_sand_bitstream_get_any_field(
          entry->value,
          first_bit,
          SOC_PB_TCAM_WORD_SIZE_IN_BITS,
          tbl_data->value
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  res = soc_sand_bitstream_get_any_field(
          entry->mask,
          first_bit,
          SOC_PB_TCAM_WORD_SIZE_IN_BITS,
          tbl_data->mask
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tcam_entry_word_extract()", 0, 0);
}

STATIC
  uint32
    soc_pb_tcam_entry_word_recover(
      SOC_SAND_IN  SOC_PB_PP_IHB_TCAM_BANK_TBL_DATA *tbl_data,
      SOC_SAND_IN  uint8                    take_lsb,
      SOC_SAND_OUT SOC_PB_TCAM_ENTRY                *entry
    )
{
  uint32
    first_bit,
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  first_bit = take_lsb ? 0 : SOC_PB_TCAM_WORD_SIZE_IN_BITS;
  res = soc_sand_bitstream_set_any_field(
          tbl_data->value,
          first_bit,
          SOC_PB_TCAM_WORD_SIZE_IN_BITS,
          entry->value
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  res = soc_sand_bitstream_set_any_field(
          tbl_data->mask,
          first_bit,
          SOC_PB_TCAM_WORD_SIZE_IN_BITS,
          entry->mask
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tcam_entry_word_recover()", 0, 0);
}

STATIC
  uint32
    soc_pb_tcam_tbl_bank_write(
      SOC_SAND_IN  int               unit,
      SOC_SAND_IN  uint32               bank_id,
      SOC_SAND_IN  SOC_PB_TCAM_BANK_ENTRY_SIZE entry_size,
      SOC_SAND_IN  uint32                base_address,
      SOC_SAND_IN  SOC_PB_TCAM_ENTRY           *entry
    )
{
  uint32
    nof_words,
    word,
    res = SOC_SAND_OK;
  SOC_PB_PP_IHB_TCAM_BANK_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  /*
   *  Copy the entry data and write it to the table
   */
  tbl_data.valid = 1;
  nof_words = (entry_size == SOC_PB_TCAM_BANK_ENTRY_SIZE_288_BITS) ? 2 : 1;
  for (word = 0; word < nof_words; ++ word)
  {
    res = soc_pb_tcam_entry_word_extract(
            entry,
            (word == 0) ? TRUE : FALSE,
            &tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    res = soc_pb_pp_ihb_tcam_tbl_write_unsafe(
            unit,
            bank_id,
            (entry_size == SOC_PB_TCAM_BANK_ENTRY_SIZE_72_BITS) ? 2 : 1,
            base_address + word,
            &tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tcam_tbl_bank_write()", 0, 0);
}

STATIC
  uint32
    soc_pb_tcam_tbl_bank_read(
      SOC_SAND_IN  int               unit,
      SOC_SAND_IN  uint32               bank_id,
      SOC_SAND_IN  SOC_PB_TCAM_BANK_ENTRY_SIZE entry_size,
      SOC_SAND_IN  uint32                base_address,
      SOC_SAND_OUT SOC_PB_TCAM_ENTRY           *entry
    )
{
  uint32
    nof_words,
    word,
    res = SOC_SAND_OK;
  SOC_PB_PP_IHB_TCAM_BANK_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  nof_words = (entry_size == SOC_PB_TCAM_BANK_ENTRY_SIZE_288_BITS) ? 2 : 1;
  for (word = 0; word < nof_words; ++ word)
  {
    res = soc_pb_pp_ihb_tcam_tbl_read_unsafe(
            unit,
            bank_id,
            (entry_size == SOC_PB_TCAM_BANK_ENTRY_SIZE_72_BITS) ? 2 : 1,
            base_address + word,
            &tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    res = soc_pb_tcam_entry_word_recover(
            &tbl_data,
            (word == 0) ? TRUE : FALSE,
            entry
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tcam_tbl_bank_read()", 0, 0);
}

STATIC
  uint32
    soc_pb_tcam_tbl_bank_compare(
      SOC_SAND_IN  int     unit,
      SOC_SAND_IN  uint32     bank_id,
      SOC_SAND_IN  SOC_PB_TCAM_ENTRY *key,
      SOC_SAND_OUT uint32     *base_address,
      SOC_SAND_OUT uint8     *found
    )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    ndx;
  SOC_PB_TCAM_BANK_ENTRY_SIZE
    entry_size;
  SOC_PB_PP_IHB_TCAM_BANK_TBL_DATA
    compare_data;
  SOC_PB_PP_IHB_TCAM_COMPARE_DATA
    found_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  for (ndx = 0; ndx < SOC_PB_PP_IHB_TCAM_DATA_WIDTH; ++ndx)
  {
    compare_data.value[ndx] = key->value[ndx];
    compare_data.mask[ndx]  = key->mask[ndx];
  }

  entry_size = soc_pb_sw_db_tcam_bank_entry_size_get(
                 unit,
                 bank_id
               );

  res = soc_pb_pp_ihb_tcam_tbl_compare_unsafe(
          unit,
          bank_id,
          (entry_size == SOC_PB_TCAM_BANK_ENTRY_SIZE_72_BITS) ? 2 : 1,
          &compare_data,
          &found_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  *found = SOC_SAND_NUM2BOOL(found_data.found);
  if (*found)
  {
    *base_address = found_data.address;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tcam_tbl_bank_compare()", 0, 0);
}

STATIC
  uint32
    soc_pb_tcam_tbl_action_write(
      SOC_SAND_IN  int unit,
      SOC_SAND_IN  uint32 bank_id,
      SOC_SAND_IN  uint32  base_address,
      SOC_SAND_IN  uint8 write_high_word,
      SOC_SAND_IN  uint32  action
    )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_IHB_TCAM_ACTION_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = soc_pb_pp_ihb_tcam_action_tbl_get_unsafe(
	        unit,
	        bank_id,
	        base_address,
	        &tbl_data
	      );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (write_high_word)
  {
    tbl_data.high = action;
  }
  else
  {
    tbl_data.low = action;
  }

  res = soc_pb_pp_ihb_tcam_action_tbl_set_unsafe(
	        unit,
	        bank_id,
	        base_address,
	        &tbl_data
	      );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tcam_tbl_action_write()", 0, 0);
}

STATIC
  uint32
    soc_pb_tcam_tbl_action_read(
      SOC_SAND_IN  int unit,
      SOC_SAND_IN  uint32 bank_id,
      SOC_SAND_IN  uint32 base_address,
      SOC_SAND_IN  uint8 read_high_word,
      SOC_SAND_OUT uint32  *action
    )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_IHB_TCAM_ACTION_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = soc_pb_pp_ihb_tcam_action_tbl_get_unsafe(
	        unit,
	        bank_id,
	        base_address,
	        &tbl_data
	      );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  *action = read_high_word ? tbl_data.high : tbl_data.low;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tcam_tbl_action_read()", 0, 0);
}

STATIC
  uint32
    soc_pb_tcam_db_exists_assert(
      SOC_SAND_IN int unit,
      SOC_SAND_IN uint32 tcam_db_id
    )
{
  uint8
    valid;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  valid = soc_pb_sw_db_tcam_db_valid_get(
            unit,
            tcam_db_id
          );
  
  if (!valid)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_TCAM_DB_DOESNT_EXIST_ERR, 10, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tcam_db_exists_assert", 0, 0);
}

STATIC
  uint8
    soc_pb_tcam_db_has_banks(
      SOC_SAND_IN  int unit,
      SOC_SAND_IN  uint32 tcam_db_id
    )
{
  uint32
    bank_id;

  for(bank_id = 0; bank_id < SOC_PB_TCAM_NOF_BANKS; ++bank_id)
  {
    if (soc_pb_sw_db_tcam_db_bank_used_get(unit, tcam_db_id, bank_id))
    {
      return TRUE;
    }
  }
  return FALSE;
}

STATIC
  uint32
    soc_pb_tcam_db_priority_list_insertion_range_find(
      SOC_SAND_IN  int     unit,
      SOC_SAND_IN  uint32     tcam_db_id,
      SOC_SAND_IN  uint16      priority,
      SOC_SAND_OUT SOC_PB_TCAM_RANGE *range
    )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_SORTED_LIST_PTR
    list;
  SOC_SAND_SORTED_LIST_ITER
    current,
    previous;
  uint8
    priority_found;
  uint8
    key_buffer[SOC_PB_TCAM_DB_LIST_KEY_SIZE],
    data_buffer[SOC_PB_TCAM_DB_LIST_DATA_SIZE];
  uint16
    node_priority;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(range);

  SOC_PB_TCAM_DB_EXISTS_ASSERT;

  list = soc_pb_sw_db_tcam_db_priorities_get(
           unit,
           tcam_db_id
         );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  /*
   *  Find the lower bound (the last entry in the database before the range of entries with the
   *  same priority as the new entry, or the beginning of the database)
   */
  priority_found = FALSE;
  previous = SOC_SAND_SORTED_LIST_ITER_BEGIN(unit,list);
  current  = SOC_SAND_SORTED_LIST_ITER_BEGIN(unit,list);
  res = soc_sand_sorted_list_get_next(
          unit,
          list,
          &current,
          key_buffer,
          data_buffer
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  while (!priority_found && (current != SOC_SAND_SORTED_LIST_ITER_END(unit,list)))
  {
    node_priority = soc_pb_tcam_db_priority_decode(data_buffer);
    if (priority <= node_priority)
    {
      priority_found = TRUE;
    }
    else
    {
      previous = current;
      res = soc_sand_sorted_list_get_next(
              unit,
              list,
              &current,
              key_buffer,
              data_buffer
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
    }
  }
  if (previous != SOC_SAND_SORTED_LIST_ITER_BEGIN(unit,list))
  {
    res = soc_sand_sorted_list_entry_value(
            unit,
            list,
            previous,
            key_buffer,
            data_buffer
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
    soc_pb_tcam_db_location_decode(
      key_buffer,
      &range->min
    );
  }
  else
  {
    range->min.bank_id = 0;
    range->min.entry   = 0;
  }

  /*
   *  Continue the list traversal to find the upper bound (the first entry with a higher priority
   *  than the new entry, or the end of the database)
   */
  priority_found = FALSE;
  current = previous;
  res = soc_sand_sorted_list_get_next(
          unit,
          list,
          &current,
          key_buffer,
          data_buffer
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
  while (!priority_found && (current != SOC_SAND_SORTED_LIST_ITER_END(unit,list)))
  {
    node_priority = soc_pb_tcam_db_priority_decode(data_buffer);
    if (priority != node_priority)
    {
      priority_found = TRUE;
    }
    else
    {
      res = soc_sand_sorted_list_get_next(
              unit,
              list,
              &current,
              key_buffer,
              data_buffer
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
    }
  }
  if (current != SOC_SAND_SORTED_LIST_ITER_END(unit,list))
  {
    soc_pb_tcam_db_location_decode(
      key_buffer,
      &range->max
    );
  }
  else
  {
    range->max.bank_id = SOC_PB_TCAM_NOF_BANKS - 1;
    range->max.entry   = soc_pb_tcam_db_entries_per_bank_get(
                           unit,
                           tcam_db_id
                         ) - 1;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tcam_db_priority_list_insertion_range_find", 0, 0);
}

STATIC
  uint32
    soc_pb_tcam_bank_entry_set_unsafe(
      SOC_SAND_IN  int            unit,
      SOC_SAND_IN  uint32            bank_id,
      SOC_SAND_IN  uint32             address,
      SOC_SAND_IN  SOC_PB_TCAM_ENTRY        *entry,
      SOC_SAND_IN  uint32             action
    )
{
  uint32
    base_address,
    res = SOC_SAND_OK;
  SOC_PB_TCAM_BANK_ENTRY_SIZE
    entry_size;
  uint8
    write_high_word;
  SOC_SAND_OCC_BM_PTR
    occ_bm;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  if (!soc_pb_sw_db_tcam_bank_valid_get(unit, bank_id))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_TCAM_BANK_UNINITIALIZED_ERR, 10, exit);
  }

  entry_size   = soc_pb_sw_db_tcam_bank_entry_size_get(
                   unit,
                   bank_id
                 );
  base_address = (entry_size == SOC_PB_TCAM_BANK_ENTRY_SIZE_288_BITS) ? address << 1 : address;
  
  res = soc_pb_tcam_tbl_bank_write(
          unit,
          bank_id,
          entry_size,
          base_address,
          entry
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  if (entry_size == SOC_PB_TCAM_BANK_ENTRY_SIZE_72_BITS)
  {
    write_high_word = (address & 0x1) ? TRUE : FALSE;
    base_address    = address >> 1;
  }
  else
  {
    write_high_word = FALSE;
  }
  res = soc_pb_tcam_tbl_action_write(
          unit,
          bank_id,
          base_address,
          write_high_word,
          action
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  /*
   *  Update the occupation bitmap
   */
  occ_bm = soc_pb_sw_db_tcam_bank_entries_used_get(
             unit,
             bank_id
           );
  res = soc_sand_occ_bm_occup_status_set(
          unit,
          occ_bm,
          address,
          TRUE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tcam_bank_entry_set_unsafe()", 0, 0);
}

STATIC
  uint32
    soc_pb_tcam_bank_entry_get_unsafe(
      SOC_SAND_IN  int     unit,
      SOC_SAND_IN  uint32     bank_id,
      SOC_SAND_IN  uint32      address,
      SOC_SAND_OUT SOC_PB_TCAM_ENTRY *entry,
      SOC_SAND_OUT uint32      *action
    )
{
  uint32
    base_address,
    res = SOC_SAND_OK;
  SOC_PB_TCAM_BANK_ENTRY_SIZE
    entry_size;
  uint8
    read_high_word;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  if (!soc_pb_sw_db_tcam_bank_valid_get(unit, bank_id))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_TCAM_BANK_UNINITIALIZED_ERR, 10, exit);
  }

  entry_size   = soc_pb_sw_db_tcam_bank_entry_size_get(
                   unit,
                   bank_id
                 );
  base_address = (entry_size == SOC_PB_TCAM_BANK_ENTRY_SIZE_288_BITS) ? address << 1 : address;
  
  res = soc_pb_tcam_tbl_bank_read(
          unit,
          bank_id,
          entry_size,
          base_address,
          entry
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  if (entry_size == SOC_PB_TCAM_BANK_ENTRY_SIZE_72_BITS)
  {
    read_high_word = (address & 0x1) ? TRUE : FALSE;
    base_address   = address >> 1;
  }
  else
  {
    read_high_word = FALSE;
  }
  res = soc_pb_tcam_tbl_action_read(
          unit,
          bank_id,
          base_address,
          read_high_word,
          action
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tcam_bank_entry_get_unsafe()", 0, 0);
}

STATIC
  uint32
    soc_pb_tcam_bank_entry_invalidate_unsafe(
      SOC_SAND_IN int unit,
      SOC_SAND_IN uint32 bank_id,
      SOC_SAND_IN uint32  address
    )
{
  uint32
    res = SOC_SAND_OK,
    base_address;
  SOC_SAND_OCC_BM_PTR
    occ_bm;
  SOC_PB_TCAM_BANK_ENTRY_SIZE
    entry_size;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  if (!soc_pb_sw_db_tcam_bank_valid_get(unit, bank_id))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_TCAM_BANK_UNINITIALIZED_ERR, 10, exit);
  }

  /*
   *  Mark the entry as available
   */
  occ_bm = soc_pb_sw_db_tcam_bank_entries_used_get(
             unit,
             bank_id
           );
  res = soc_sand_occ_bm_occup_status_set(
          unit,
          occ_bm,
          address,
          FALSE
        );

  /*
   *  Invalidate the hardware entry
   */
  entry_size   = soc_pb_sw_db_tcam_bank_entry_size_get(
                   unit,
                   bank_id
                 );
  base_address = (entry_size == SOC_PB_TCAM_BANK_ENTRY_SIZE_288_BITS) ? address << 1 : address;
  res = soc_pb_pp_ihb_tcam_valid_bit_tbl_set_unsafe(
          unit,
          bank_id,
          (entry_size == SOC_PB_TCAM_BANK_ENTRY_SIZE_72_BITS) ? 2 : 1,
          base_address,
          FALSE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tcam_bank_entry_invalidate_unsafe()", 0, 0);
}

STATIC
  void
    soc_pb_tcam_db_bit_search_range_in_bank_mode_get(
      SOC_SAND_IN  SOC_PB_TCAM_RANGE *range,
      SOC_SAND_IN  uint32     bank_id,
      SOC_SAND_IN  uint32     entry_count,
      SOC_SAND_OUT uint32      *start,
      SOC_SAND_OUT uint32      *end,
      SOC_SAND_OUT uint8     *skip
    )
{
  if (bank_id < range->min.bank_id || bank_id > range->max.bank_id)
  {
    *skip = TRUE;
  }
  else
  {
    *skip  = FALSE;
    *start = (bank_id == range->min.bank_id) ? range->min.entry : 0;
    *end   = (bank_id == range->max.bank_id) ? range->max.entry : entry_count - 1;
  }
}

STATIC
  void
    soc_pb_tcam_db_bit_search_range_in_interlaced_mode_get(
      SOC_SAND_IN  SOC_PB_TCAM_RANGE *range,
      SOC_SAND_IN  uint32     bank_id,
      SOC_SAND_OUT uint32      *start,
      SOC_SAND_OUT uint32      *end,
      SOC_SAND_OUT uint8     *skip
    )
{
  if (range->max.entry - range->min.entry == 0)
  {
    if ((bank_id < range->min.bank_id) || (bank_id > range->max.bank_id))
    {
      *skip = TRUE;
    }
  }
  else if (range->max.entry - range->min.entry == 1)
  {
    if ((bank_id < range->min.bank_id) && (bank_id > range->max.bank_id))
    {
      *skip = TRUE;
    }
  }

  if (!*skip)
  {
    *start = range->min.entry + ((bank_id < range->min.bank_id) ? 1 : 0);
    *end   = range->max.entry - ((bank_id > range->max.bank_id) ? 1 : 0);
  }
}

STATIC
  void
    soc_pb_tcam_db_bit_search_range_get(
      SOC_SAND_IN  int     unit,
      SOC_SAND_IN  uint32     tcam_db_id,
      SOC_SAND_IN  SOC_PB_TCAM_RANGE *range,
      SOC_SAND_IN  uint32     bank_id,
      SOC_SAND_OUT uint32      *start,
      SOC_SAND_OUT uint32      *end,
      SOC_SAND_OUT uint8     *skip
    )
{
  uint8
    bank_used;
  SOC_PB_TCAM_DB_PRIO_MODE
    prio_mode;
  uint32
    entry_count;

  bank_used = soc_pb_sw_db_tcam_db_bank_used_get(
                unit,
                tcam_db_id,
                bank_id
              );
  if (!bank_used)
  {
    *skip = TRUE;
  }
  else
  {
    *skip = FALSE;
    prio_mode = soc_pb_sw_db_tcam_db_priority_mode_get(
                  unit,
                  tcam_db_id
                );
    switch (prio_mode)
    {
    case SOC_PB_TCAM_DB_PRIO_MODE_BANK:
      entry_count = soc_pb_tcam_db_entries_per_bank_get(
                      unit,
                      tcam_db_id
                    );
      soc_pb_tcam_db_bit_search_range_in_bank_mode_get(
        range,
        bank_id,
        entry_count,
        start,
        end,
        skip
      );
      break;

    case SOC_PB_TCAM_DB_PRIO_MODE_INTERLACED:
      soc_pb_tcam_db_bit_search_range_in_interlaced_mode_get(
        range,
        bank_id,
        start,
        end,
        skip
      );
      break;

    default:
      break;
    }
  }
}

STATIC
  uint32
    soc_pb_tcam_db_empty_location_in_range_find(
      SOC_SAND_IN  int            unit,
      SOC_SAND_IN  uint32            tcam_db_id,
      SOC_SAND_IN  SOC_PB_TCAM_RANGE        *range,
      SOC_SAND_OUT SOC_PB_TCAM_LOCATION     *location,
      SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE *success
    )
{
  uint32
    res = SOC_SAND_OK,
    start_bit,
    end_bit;
  uint32
    bank_id;
  uint8
    found,
    skip;
  SOC_SAND_OCC_BM_PTR
    entries_used;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(range);
  SOC_SAND_CHECK_NULL_INPUT(location);
  SOC_SAND_CHECK_NULL_INPUT(success);

  found = FALSE;
  for (bank_id = 0; !found && (bank_id < SOC_PB_TCAM_NOF_BANKS); ++bank_id)
  {
    soc_pb_tcam_db_bit_search_range_get(
      unit,
      tcam_db_id,
      range,
      bank_id,
      &start_bit,
      &end_bit,
      &skip
    );
    if (!skip)
    {
      entries_used = soc_pb_sw_db_tcam_bank_entries_used_get(
                       unit,
                       bank_id
                     );
      res = soc_sand_occ_bm_get_next_in_range(
              unit,
              entries_used,
              start_bit,
              end_bit,
              TRUE,
              (uint32 *) &location->entry,
              &found
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

      location->bank_id = bank_id;
    }
  }

  *success = found ? SOC_SAND_SUCCESS : SOC_SAND_FAILURE_OUT_OF_RESOURCES;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tcam_db_empty_location_in_range_find()", 0, 0);
}

STATIC
  uint32
    soc_pb_tcam_db_priority_list_entry_add(
      SOC_SAND_IN  int            unit,
      SOC_SAND_IN  uint32            tcam_db_id,
      SOC_SAND_IN  SOC_PB_TCAM_LOCATION     *location,
      SOC_SAND_IN  uint32            entry_id,
      SOC_SAND_IN  uint16             priority,
      SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE *success
    )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_SORTED_LIST_PTR
    prio_list;
  uint8
    key_buffer[SOC_PB_TCAM_DB_LIST_KEY_SIZE],
    data_buffer[SOC_PB_TCAM_DB_LIST_DATA_SIZE];
  uint8
    entry_added;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  soc_pb_tcam_db_location_encode(
    location,
    key_buffer
  );

  soc_pb_tcam_db_entry_id_encode(
    entry_id,
    data_buffer
  );
  soc_pb_tcam_db_priority_encode(
    priority,
    data_buffer
  );

  prio_list = soc_pb_sw_db_tcam_db_priorities_get(
                unit,
                tcam_db_id
              );
  res = soc_sand_sorted_list_entry_add(
          unit,
          prio_list,
          key_buffer,
          data_buffer,
          &entry_added
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  *success = entry_added ? SOC_SAND_SUCCESS : SOC_SAND_FAILURE_OUT_OF_RESOURCES;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tcam_db_priority_list_entry_add()", 0, 0);
}

STATIC
  uint32
    soc_pb_tcam_db_priority_list_entry_iter_get(
      SOC_SAND_IN  int             unit,
      SOC_SAND_IN  uint32             tcam_db_id,
      SOC_SAND_IN  SOC_PB_TCAM_LOCATION      *location,
      SOC_SAND_OUT SOC_SAND_SORTED_LIST_ITER *iter
    )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_SORTED_LIST_PTR
    prio_list;
  uint8
    key_buffer[SOC_PB_TCAM_DB_LIST_KEY_SIZE];
  uint8
    found;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  soc_pb_tcam_db_location_encode(
    location,
    key_buffer
  );

  prio_list = soc_pb_sw_db_tcam_db_priorities_get(
                unit,
                tcam_db_id
              );
  res = soc_sand_sorted_list_entry_lookup(
          unit,
          prio_list,
          key_buffer,
          NULL,
          &found,
          iter
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (!found)
  {
    /* Error */
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tcam_db_priority_list_entry_iter_get()", 0, 0);
}

STATIC
  uint32
    soc_pb_tcam_db_priority_list_entry_get(
      SOC_SAND_IN  int        unit,
      SOC_SAND_IN  uint32        tcam_db_id,
      SOC_SAND_IN  SOC_PB_TCAM_LOCATION *location,
      SOC_SAND_OUT uint32        *entry_id,
      SOC_SAND_OUT uint16         *priority
    )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_SORTED_LIST_PTR
    prio_list;
  uint8
    key_buffer[SOC_PB_TCAM_DB_LIST_KEY_SIZE],
    data_buffer[SOC_PB_TCAM_DB_LIST_DATA_SIZE];
  SOC_SAND_SORTED_LIST_ITER
    iter;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = soc_pb_tcam_db_priority_list_entry_iter_get(
          unit,
          tcam_db_id,
          location,
          &iter
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  prio_list = soc_pb_sw_db_tcam_db_priorities_get(
                unit,
                tcam_db_id
              );
  res = soc_sand_sorted_list_entry_value(
          unit,
          prio_list,
          iter,
          key_buffer,
          data_buffer
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  *priority = soc_pb_tcam_db_priority_decode(
                data_buffer
              );
  *entry_id = soc_pb_tcam_db_entry_id_decode(
                data_buffer
              );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tcam_db_priority_list_entry_get()", 0, 0);
}

STATIC
  uint32
    soc_pb_tcam_db_priority_list_entry_remove(
      SOC_SAND_IN int        unit,
      SOC_SAND_IN uint32        tcam_db_id,
      SOC_SAND_IN SOC_PB_TCAM_LOCATION *location
    )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_SORTED_LIST_PTR
    prio_list;
  SOC_SAND_SORTED_LIST_ITER
    iter;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = soc_pb_tcam_db_priority_list_entry_iter_get(
          unit,
          tcam_db_id,
          location,
          &iter
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  prio_list = soc_pb_sw_db_tcam_db_priorities_get(
                unit,
                tcam_db_id
              );
  res = soc_sand_sorted_list_entry_remove_by_iter(
          unit,
          prio_list,
          iter
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tcam_db_priority_list_entry_remove()", 0, 0);
}

STATIC
  uint32
    soc_pb_tcam_db_entry_id_to_location_entry_add(
      SOC_SAND_IN int        unit,
      SOC_SAND_IN uint32        tcam_db_id,
      SOC_SAND_IN uint32        entry_id,
      SOC_SAND_IN SOC_PB_TCAM_LOCATION *location
    )
{
  uint32
    res = SOC_SAND_OK,
    data_indx;
  SOC_SAND_HASH_TABLE_PTR
    hash_tbl;
  uint8
    key_buffer[SOC_PB_TCAM_DB_HASH_TBL_KEY_SIZE];
  uint8
    success;
  SOC_PB_TCAM_LOCATION
    *location_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  hash_tbl = soc_pb_sw_db_tcam_db_entry_id_to_location_get(
               unit,
               tcam_db_id
             );

  soc_pb_tcam_db_entry_id_encode(
    entry_id,
    key_buffer
  );

  res = soc_sand_hash_table_entry_add(
          unit,
          hash_tbl,
          key_buffer,
          &data_indx,
          &success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (!success)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_MALLOC_FAIL, 20, exit);
  }

  location_tbl = soc_pb_sw_db_tcam_db_location_tbl_get(
                   unit,
                   tcam_db_id
                 );
  location_tbl[data_indx] = *location;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tcam_db_entry_id_to_location_entry_add()", 0, 0);
}

STATIC
  uint32
    soc_pb_tcam_db_entry_id_to_location_entry_index_get(
      SOC_SAND_IN  int unit,
      SOC_SAND_IN  uint32 tcam_db_id,
      SOC_SAND_IN  uint32 entry_id,
      SOC_SAND_OUT uint32  *index,
      SOC_SAND_OUT uint8 *found
    )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_HASH_TABLE_PTR
    entry_id_to_location ;
  uint8
    key_buffer[SOC_PB_TCAM_DB_HASH_TBL_KEY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  entry_id_to_location = soc_pb_sw_db_tcam_db_entry_id_to_location_get(
                           unit,
                           tcam_db_id
                         );

  soc_pb_tcam_db_entry_id_encode(
    entry_id,
    key_buffer
  );

  res = soc_sand_hash_table_entry_lookup(
          unit,
          entry_id_to_location,
          key_buffer,
          index,
          found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tcam_db_entry_id_to_location_entry_index_get()", 0, 0);
}


uint32
  soc_pb_tcam_db_entry_id_to_location_entry_get(
    SOC_SAND_IN  int        unit,
    SOC_SAND_IN  uint32        tcam_db_id,
    SOC_SAND_IN  uint32        entry_id,
    SOC_SAND_OUT SOC_PB_TCAM_LOCATION *location,
    SOC_SAND_OUT uint8        *found
  )
{
  uint32
    res = SOC_SAND_OK,
    data_indx;
  SOC_PB_TCAM_LOCATION
    *location_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = soc_pb_tcam_db_entry_id_to_location_entry_index_get(
          unit,
          tcam_db_id,
          entry_id,
          &data_indx,
          found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (*found)
  {
    location_tbl = soc_pb_sw_db_tcam_db_location_tbl_get(
                     unit,
                     tcam_db_id
                   );
    *location = location_tbl[data_indx];
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tcam_db_entry_id_to_location_entry_get()", 0, 0);
}

STATIC
  uint32
    soc_pb_tcam_db_entry_id_to_location_entry_remove(
      SOC_SAND_IN int unit,
      SOC_SAND_IN uint32 tcam_db_id,
      SOC_SAND_IN uint32 entry_id
    )
{
  uint32
    res = SOC_SAND_OK,
    data_indx;
  SOC_SAND_HASH_TABLE_PTR
    entry_id_to_location ;
  uint8
    found;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = soc_pb_tcam_db_entry_id_to_location_entry_index_get(
          unit,
          tcam_db_id,
          entry_id,
          &data_indx,
          &found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (found)
  {
    entry_id_to_location = soc_pb_sw_db_tcam_db_entry_id_to_location_get(
                             unit,
                             tcam_db_id
                           );
    res = soc_sand_hash_table_entry_remove_by_index(
            unit,
            entry_id_to_location,
            data_indx
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tcam_db_entry_id_to_location_entry_remove()", 0, 0);
}

STATIC
  uint32
    soc_pb_tcam_db_entry_prefix_stamp(
      SOC_SAND_IN    int     unit,
      SOC_SAND_IN    uint32     tcam_db_id,
      SOC_SAND_IN    uint32     bank_id,
      SOC_SAND_INOUT SOC_PB_TCAM_ENTRY *entry
    )
{
  uint32
    res = SOC_SAND_OK,
    mask;
  SOC_PB_TCAM_PREFIX
    prefix;
  SOC_PB_TCAM_BANK_ENTRY_SIZE
    entry_size;
  uint32
    entry_nof_bits;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  soc_pb_sw_db_tcam_db_prefix_get(
    unit,
    tcam_db_id,
    bank_id,
    &prefix
  );

  if (prefix.length > 0)
  {
    entry_size = soc_pb_sw_db_tcam_db_entry_size_get(
                   unit,
                   tcam_db_id
                 );
    switch (entry_size)
    {
    case SOC_PB_TCAM_BANK_ENTRY_SIZE_72_BITS:
      entry_nof_bits = 72;
      break;

    case SOC_PB_TCAM_BANK_ENTRY_SIZE_144_BITS:
      entry_nof_bits = 144;
      break;

    case SOC_PB_TCAM_BANK_ENTRY_SIZE_288_BITS:
      entry_nof_bits = 288;
      break;

    default:
      /*
       *  Don't get here
       */
      entry_nof_bits = (uint32) -1;
      break;
    }

    res = soc_sand_bitstream_set_any_field(
            &prefix.bits,
            entry_nof_bits - prefix.length,
            prefix.length,
            entry->value
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    mask = SOC_SAND_BITS_MASK(prefix.length - 1, 0);
    res = soc_sand_bitstream_set_any_field(
            &mask,
            entry_nof_bits - prefix.length,
            prefix.length,
            entry->mask
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tcam_db_entry_prefix_stamp()", 0, 0);
}

STATIC
  uint32
    soc_pb_tcam_location_distance_get(
      SOC_SAND_IN SOC_PB_TCAM_LOCATION        *low,
      SOC_SAND_IN SOC_PB_TCAM_LOCATION        *high,
      SOC_SAND_IN SOC_PB_TCAM_BANK_ENTRY_SIZE entry_size,
      SOC_SAND_IN SOC_PB_TCAM_DB_PRIO_MODE    prio_mode
    )
{
  uint32
    distance,
    entry_count;

  switch (prio_mode)
  {
  case SOC_PB_TCAM_DB_PRIO_MODE_BANK:
    if (low->bank_id == high->bank_id)
    {
      distance = high->entry - low->entry;
    }
    else
    {
      entry_count = soc_pb_tcam_bank_entry_size_to_entry_count_get(entry_size);
      distance    = high->entry
                      + (high->bank_id - low->bank_id - 1) * entry_count
                      + (entry_count - low->entry);
    }
    break;

  case SOC_PB_TCAM_DB_PRIO_MODE_INTERLACED:
    if (low->entry == high->entry)
    {
      distance = high->bank_id - low->bank_id;
    }
    else
    {
      distance = high->bank_id
                   + (high->entry - low->entry - 1) * SOC_PB_TCAM_NOF_BANKS
                   + (SOC_PB_TCAM_NOF_BANKS - low->bank_id);
    }
    break;

  default:
    distance = 0xffffffff;
    break;
  }

  return distance;
}

STATIC
  uint32
    soc_pb_tcam_closest_free_location_find(
      SOC_SAND_IN  int         unit,
      SOC_SAND_IN  uint32         tcam_db_id,
      SOC_SAND_IN  SOC_PB_TCAM_LOCATION  *location,
      SOC_SAND_IN  SOC_PB_TCAM_DIRECTION direction,
      SOC_SAND_OUT SOC_PB_TCAM_LOCATION  *free_location,
      SOC_SAND_OUT uint32         *distance,
      SOC_SAND_OUT uint8         *success
    )
{
  uint32
    res = SOC_SAND_OK,
    start,
    end,
    place;
  uint32
    entry_count,
    bank_id,
    current_distance;
  uint8
    skip,
    found;
  SOC_PB_TCAM_BANK_ENTRY_SIZE
    entry_size;
  SOC_PB_TCAM_RANGE
    search_range;
  SOC_SAND_OCC_BM_PTR
    bank_bm;
  SOC_PB_TCAM_LOCATION
    current;
  SOC_PB_TCAM_DB_PRIO_MODE
    prio_mode;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_PB_TCAM_RANGE_clear(&search_range);
  SOC_PB_TCAM_LOCATION_clear(&current);

  entry_size  = soc_pb_sw_db_tcam_db_entry_size_get(
                  unit,
                  tcam_db_id
                );
  entry_count = soc_pb_tcam_bank_entry_size_to_entry_count_get(
                  entry_size
                );
  prio_mode   = soc_pb_sw_db_tcam_db_priority_mode_get(
                  unit,
                  tcam_db_id
                );

  /*
   *  Search for a free entry below the range
   */
  switch (direction)
  {
  case SOC_PB_TCAM_DIRECTION_BACKWARD:
    search_range.min.bank_id = 0;
    search_range.min.entry   = 0;
    search_range.max.bank_id = location->bank_id;
    search_range.max.entry   = location->entry;
    break;

  case SOC_PB_TCAM_DIRECTION_FORWARD:
    search_range.min.bank_id = location->bank_id;
    search_range.min.entry   = location->entry;
    search_range.max.bank_id = SOC_PB_TCAM_NOF_BANKS - 1;
    search_range.max.entry   = entry_count - 1;
    break;

  default:
    break;
  }

  *success = FALSE;
  for (bank_id = 0; bank_id < SOC_PB_TCAM_NOF_BANKS; ++bank_id)
  {
    soc_pb_tcam_db_bit_search_range_get(
      unit,
      tcam_db_id,
      &search_range,
      bank_id,
      &start,
      &end,
      &skip
    );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    if (!skip)
    {
      bank_bm = soc_pb_sw_db_tcam_bank_entries_used_get(
                  unit,
                  bank_id
                );
      res = soc_sand_occ_bm_get_next_in_range(
              unit,
              bank_bm,
              start,
              end,
              (direction == SOC_PB_TCAM_DIRECTION_FORWARD) ? TRUE : FALSE,
              &place,
              &found
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

      if (found)
      {
        current.bank_id  = bank_id;
        current.entry    = place;
        current_distance = soc_pb_tcam_location_distance_get(
                             (direction == SOC_PB_TCAM_DIRECTION_FORWARD)? location : &current, /* low */
                             (direction == SOC_PB_TCAM_DIRECTION_FORWARD)? &current : location, /* high */
                             entry_size,
                             prio_mode
                           );
        if (!*success || (current_distance < *distance))
        {
          *free_location = current;
          *distance      = current_distance;
          *success       = TRUE;
        }
      }
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tcam_closest_free_location_find()", 0, 0);
}

STATIC
  uint32
    soc_pb_tcam_closest_free_location_to_range_find(
      SOC_SAND_IN  int            unit,
      SOC_SAND_IN  uint32            tcam_db_id,
      SOC_SAND_IN  SOC_PB_TCAM_RANGE        *range,
      SOC_SAND_OUT SOC_PB_TCAM_LOCATION     *free_location,
      SOC_SAND_OUT SOC_PB_TCAM_DIRECTION    *direction,
      SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE *success
    )
{
  uint32
    res = SOC_SAND_OK;
  uint8
    found_below,
    found_above;
  uint32
    distance_below,
    distance_above;
  SOC_PB_TCAM_LOCATION
    below,
    above;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = soc_pb_tcam_closest_free_location_find(
          unit,
          tcam_db_id,
          &range->min,
          SOC_PB_TCAM_DIRECTION_BACKWARD,
          &below,
          &distance_below,
          &found_below
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_pb_tcam_closest_free_location_find(
          unit,
          tcam_db_id,
          &range->max,
          SOC_PB_TCAM_DIRECTION_FORWARD,
          &above,
          &distance_above,
          &found_above
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  if (!found_below && !found_above)
  {
    *success = SOC_SAND_FAILURE_OUT_OF_RESOURCES;
  }
  else
  {
    *success = SOC_SAND_SUCCESS;
    if (found_below)
    {
      *free_location = below;
      *direction     = SOC_PB_TCAM_DIRECTION_BACKWARD;
      if (found_above && (distance_above < distance_below))
      {
        *free_location = above;
        *direction     = SOC_PB_TCAM_DIRECTION_FORWARD;
      }
    }
    else
    {
      *free_location = above;
      *direction     = SOC_PB_TCAM_DIRECTION_FORWARD;
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tcam_closest_free_location_to_range_find()", 0, 0);
}

STATIC
  uint32
    soc_pb_tcam_db_entry_move(
      SOC_SAND_IN int        unit,
      SOC_SAND_IN uint32        tcam_db_id,
      SOC_SAND_IN SOC_PB_TCAM_LOCATION *source,
      SOC_SAND_IN SOC_PB_TCAM_LOCATION *destination
    )
{
  uint32
    res = SOC_SAND_OK,
    action,
    data_indx;
  uint32
    entry_id;
  SOC_SAND_SORTED_LIST_PTR
    prio_list;
  SOC_SAND_HASH_TABLE_PTR
    hash_tbl;
  uint8
    key_buffer[SOC_PB_TCAM_DB_LIST_KEY_SIZE],
    data_buffer[SOC_PB_TCAM_DB_LIST_DATA_SIZE],
    hash_key[SOC_PB_TCAM_DB_HASH_TBL_KEY_SIZE];
  uint8
    success;
  SOC_SAND_SORTED_LIST_ITER
    iter;
  SOC_PB_TCAM_ENTRY
    entry;
  SOC_PB_TCAM_LOCATION
    *location_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  /*
   *  Move the entry in the database's priority list
   */
  prio_list = soc_pb_sw_db_tcam_db_priorities_get(
                unit,
                tcam_db_id
              );
  res = soc_pb_tcam_db_location_encode(
          source,
          key_buffer
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  res = soc_sand_sorted_list_entry_lookup(
          unit,
          prio_list,
          key_buffer,
          NULL,
          &success,
          &iter
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = soc_sand_sorted_list_entry_value(
          unit,
          prio_list,
          iter,
          key_buffer,
          data_buffer
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_sand_sorted_list_entry_remove_by_iter(
          unit,
          prio_list,
          iter
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_pb_tcam_db_location_encode(
          destination,
          key_buffer
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  res = soc_sand_sorted_list_entry_add(
          unit,
          prio_list,
          key_buffer,
          data_buffer,
          &success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

  /*
   *  Update the entry_id -> location hash table
   */
  hash_tbl = soc_pb_sw_db_tcam_db_entry_id_to_location_get(
               unit,
               tcam_db_id
             );
  entry_id = soc_pb_tcam_db_entry_id_decode(
               data_buffer
             );
  soc_pb_tcam_db_entry_id_encode(
    entry_id,
    hash_key
  );
  res = soc_sand_hash_table_entry_lookup(
          unit,
          hash_tbl,
          hash_key,
          &data_indx,
          &success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

  location_tbl = soc_pb_sw_db_tcam_db_location_tbl_get(
                   unit,
                   tcam_db_id
                 );
  location_tbl[data_indx] = *destination;

  /*
   *  Move the entry in the hardware
   */
  res = soc_pb_tcam_bank_entry_get_unsafe(
          unit,
          source->bank_id,
          source->entry,
          &entry,
          &action
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

  res = soc_pb_tcam_bank_entry_invalidate_unsafe(
          unit,
          source->bank_id,
          source->entry
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

  res = soc_pb_tcam_bank_entry_set_unsafe(
          unit,
          destination->bank_id,
          destination->entry,
          &entry,
          action
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tcam_db_entry_move()", 0, 0);
}

STATIC
  uint32
    soc_pb_tcam_db_entries_migrate(
      SOC_SAND_IN  int         unit,
      SOC_SAND_IN  uint32         tcam_db_id,
      SOC_SAND_IN  SOC_PB_TCAM_LOCATION  *empty_location,
      SOC_SAND_IN  uint16          target_priority,
      SOC_SAND_IN  SOC_PB_TCAM_DIRECTION direction,
      SOC_SAND_OUT SOC_PB_TCAM_LOCATION  *free_location
    )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_SORTED_LIST_PTR
    prio_list;
  SOC_SAND_SORTED_LIST_ITER
    next;
  uint8
    found;
  uint8
    key_buffer[SOC_PB_TCAM_DB_LIST_KEY_SIZE],
    data_buffer[SOC_PB_TCAM_DB_LIST_DATA_SIZE];
  uint16
    range_priority,
    next_priority;
  SOC_PB_TCAM_LOCATION
    current_location,
    next_location,
    empty,
    range_start,
    range_end;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_PB_TCAM_LOCATION_clear(&current_location);
  SOC_PB_TCAM_LOCATION_clear(&next_location);
  SOC_PB_TCAM_LOCATION_clear(&empty);
  SOC_PB_TCAM_LOCATION_clear(&range_start);
  SOC_PB_TCAM_LOCATION_clear(&range_start);
  SOC_PB_TCAM_LOCATION_clear(free_location);

  empty = *empty_location;

  /*
   *  Find the priority of the first entry after the empty location
   */
  prio_list = soc_pb_sw_db_tcam_db_priorities_get(
                unit,
                tcam_db_id
              );

  res = soc_pb_tcam_db_location_encode(
          empty_location,
          key_buffer
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_sand_sorted_list_entry_lookup(
          unit,
          prio_list,
          key_buffer,
          NULL,
          &found,
          &next
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  if (direction == SOC_PB_TCAM_DIRECTION_BACKWARD)
  {
    res = soc_sand_sorted_list_get_next(
            unit,
            prio_list,
            &next,
            key_buffer,
            data_buffer
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }
  else
  {
    res = soc_sand_sorted_list_entry_value(
            unit,
            prio_list,
            next,
            key_buffer,
            data_buffer
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  }

  next_priority = soc_pb_tcam_db_priority_decode(
                    data_buffer
                  );
  soc_pb_tcam_db_location_decode(
    key_buffer,
    &next_location
  );

  while (
    (direction == SOC_PB_TCAM_DIRECTION_BACKWARD) ?
      ((next != SOC_SAND_SORTED_LIST_ITER_END(unit,prio_list)) && (next_priority < target_priority))
        : ((next != SOC_SAND_SORTED_LIST_ITER_BEGIN(unit,prio_list)) && (next_priority > target_priority)))
  {
    /*
     *  Find the last entry whose priority is current_priority
     */
    range_start    = next_location;
    range_priority = next_priority;
    while ((direction == SOC_PB_TCAM_DIRECTION_BACKWARD ? 
              next != SOC_SAND_SORTED_LIST_ITER_END(unit,prio_list)
                : next != SOC_SAND_SORTED_LIST_ITER_BEGIN(unit,prio_list))
             && next_priority == range_priority)
    {
      current_location = next_location;
      if (direction == SOC_PB_TCAM_DIRECTION_BACKWARD)
      {
        res = soc_sand_sorted_list_get_next(
                unit,
                prio_list,
                &next,
                key_buffer,
                data_buffer
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
      }
      else
      {
        res = soc_sand_sorted_list_get_prev(
                unit,
                prio_list,
                &next,
                key_buffer,
                data_buffer
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
      }
      soc_pb_tcam_db_location_decode(
        key_buffer,
        &next_location
      );
      next_priority = soc_pb_tcam_db_priority_decode(
                        data_buffer
                      );
    }
    range_end = current_location;

    /*
     *  Move entries
     */
    res = soc_pb_tcam_db_entry_move(
            unit,
            tcam_db_id,
            &range_start,
            &empty
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

    if ((range_start.bank_id != range_end.bank_id) || (range_start.entry != range_end.entry))
    {
      res = soc_pb_tcam_db_entry_move(
              unit,
              tcam_db_id,
              &range_end,
              &range_start
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
    }

    empty = range_end;
  }
  *free_location = empty;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tcam_db_entries_migrate()", 0, 0);
}

STATIC
  uint32
    soc_pb_tcam_db_free_location_allocate(
      SOC_SAND_IN  int            unit,
      SOC_SAND_IN  uint32            tcam_db_id,
      SOC_SAND_IN  uint16             priority,
      SOC_SAND_OUT SOC_PB_TCAM_LOCATION     *location,
      SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE *success
    )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_TCAM_RANGE
    range;
  SOC_PB_TCAM_LOCATION
    free_location;
  SOC_PB_TCAM_DIRECTION
    direction=0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_PB_TCAM_RANGE_clear(&range);
  SOC_PB_TCAM_LOCATION_clear(&free_location);

  /*
   *  Find the appropriate range for equal-priority entries
   */
  res = soc_pb_tcam_db_priority_list_insertion_range_find(
          unit,
          tcam_db_id,
          priority,
          &range
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*
   *  Search for an empty entry in the proper range
   */
  res = soc_pb_tcam_db_empty_location_in_range_find(
          unit,
          tcam_db_id,
          &range,
          location,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  if (*success != SOC_SAND_SUCCESS)
  {
    /*
     *  Move entries to make room for the new one
     */
    res = soc_pb_tcam_closest_free_location_to_range_find(
            unit,
            tcam_db_id,
            &range,
            &free_location,
            &direction,
            success
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    if (*success == SOC_SAND_SUCCESS)
    {
      res = soc_pb_tcam_db_entries_migrate(
              unit,
              tcam_db_id,
              &free_location,
              priority,
              direction,
              location
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tcam_db_free_location_allocate()", 0, 0);
}

STATIC
  uint32
    soc_pb_tcam_db_entry_exists_get(
      SOC_SAND_IN  int unit,
      SOC_SAND_IN  uint32 tcam_db_id,
      SOC_SAND_IN  uint32 entry_id,
      SOC_SAND_OUT uint8 *found
    )
{
  uint32
    res = SOC_SAND_OK,
    data_indx;
  SOC_SAND_HASH_TABLE_PTR
    hash_tbl;
  uint8
    key_buffer[SOC_PB_TCAM_DB_HASH_TBL_KEY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  hash_tbl = soc_pb_sw_db_tcam_db_entry_id_to_location_get(
               unit,
               tcam_db_id
             );
  soc_pb_tcam_db_entry_id_encode(
    entry_id,
    key_buffer
  );
  res = soc_sand_hash_table_entry_lookup(
          unit,
          hash_tbl,
          key_buffer,
          &data_indx,
          found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tcam_db_entry_exists_get()", 0, 0);
}

/*
 *  API functions
 */
/*********************************************************************
*     This function initializes a TCAM bank.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_tcam_bank_init_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               bank_id,
    SOC_SAND_IN  SOC_PB_TCAM_BANK_ENTRY_SIZE entry_size
  )
{
  uint32
    res = SOC_SAND_OK;
  uint8
    valid,
    create_bitmap = FALSE;
  SOC_PB_TCAM_BANK_ENTRY_SIZE
    old_entry_size;
  uint32
    nof_entries;
  SOC_SAND_OCC_BM_PTR
    occ_bm;
  SOC_SAND_OCC_BM_INIT_INFO
    occ_bm_init_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_TCAM_BANK_INIT_UNSAFE);

  /*
   *  Verify
   */
  res = soc_pb_tcam_bank_init_verify(
          unit,
          bank_id,
          entry_size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  /*
   *  Flush bank
   */
  res = soc_pb_pp_ihb_tcam_tbl_flush_unsafe(
          unit,
          bank_id
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*
   *  Check whether this bank's been initialized before
   */
  valid = soc_pb_sw_db_tcam_bank_valid_get(
            unit,
            bank_id
          );
  if (!valid)
  {
    create_bitmap = TRUE;
  }
  else
  {
    /*
     *  Check if we can reuse the old occupation bitmap
     */
    old_entry_size = soc_pb_sw_db_tcam_bank_entry_size_get(
                       unit,
                       bank_id
                     );
    occ_bm         = soc_pb_sw_db_tcam_bank_entries_used_get(
                       unit,
                       bank_id
                     );

    if (entry_size == old_entry_size)
    {
      /*
       *  Reset existing occupation bitmap
       */
      soc_sand_occ_bm_clear(unit, occ_bm);
    }
    else
    {
      soc_sand_occ_bm_destroy(unit, occ_bm);
      create_bitmap = TRUE;
    }
  }
  
  /*
   *  Calculate the number of entries in this bank
   */
  nof_entries = soc_pb_tcam_bank_entry_size_to_entry_count_get(entry_size);
  soc_pb_sw_db_tcam_bank_nof_entries_free_set(
    unit,
    bank_id,
    nof_entries
  );

  if (create_bitmap)
  {
    /*
     *  Create a new occupation bitmap
     */
    soc_sand_SAND_OCC_BM_INIT_INFO_clear(&occ_bm_init_info);
    occ_bm_init_info.size     = nof_entries;
    occ_bm_init_info.init_val = FALSE;
    res = soc_sand_occ_bm_create(
            unit,
            &occ_bm_init_info,
            &occ_bm
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    soc_pb_sw_db_tcam_bank_entries_used_set(unit, bank_id, occ_bm);

    /*
     *  We reset the entry size whenever we create a new bitmap
     */
    soc_pb_sw_db_tcam_bank_entry_size_set(unit, bank_id, entry_size);
  }

  soc_pb_sw_db_tcam_bank_valid_set(
    unit,
    bank_id,
    TRUE
  );

exit:
/*
 * COVERITY
 *
 * The variable occ_bm doesn't go out of scope - it is assigned inside soc_pb_sw_db_tcam_bank_entries_used_set.
 */
/* coverity[leaked_storage] */
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tcam_bank_init_unsafe()", 0, 0);
}

uint32
  soc_pb_tcam_bank_init_verify(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               bank_id,
    SOC_SAND_IN  SOC_PB_TCAM_BANK_ENTRY_SIZE entry_size
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_TCAM_BANK_INIT_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(bank_id, SOC_PB_TCAM_BANK_ID_MAX, SOC_PB_TCAM_BANK_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(entry_size, SOC_PB_TCAM_ENTRY_SIZE_MAX, SOC_PB_TCAM_ENTRY_SIZE_OUT_OF_RANGE_ERR, 20, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tcam_bank_init_verify()", 0, 0);
}

/*********************************************************************
*     Creates a new TCAM database.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_tcam_db_create_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_IN  SOC_PB_TCAM_BANK_ENTRY_SIZE entry_size,
    SOC_SAND_IN  uint32               prefix_size,
    SOC_SAND_IN  SOC_PB_TCAM_DB_PRIO_MODE    prio_mode
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    entry_count_per_bank,
    max_nof_entries,
    bank_id;
  uint8
    valid;
  SOC_SAND_SORTED_LIST_PTR
    priorities;
  SOC_SAND_HASH_TABLE_PTR
    entry_id_to_location ;
  SOC_PB_TCAM_LOCATION
    *location_tbl;
  SOC_SAND_SORTED_LIST_INIT_INFO
    sorted_init_info ;
  SOC_SAND_HASH_TABLE_INIT_INFO
    hash_init_info ;
  extern
    SOC_PETRA_SW_DB
      Soc_petra_sw_db;

  
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_TCAM_DB_CREATE_UNSAFE);

  /*
   *  Verify
   */
  res = soc_pb_tcam_db_create_verify(
          unit,
          tcam_db_id,
          entry_size,
          prefix_size,
          prio_mode
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  /*
   * Check whether this database is already initialized
   */
  valid = soc_pb_sw_db_tcam_db_valid_get(
            unit,
            tcam_db_id
          );
  if (valid)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_TCAM_DATABASE_ALREADY_EXISTS_ERR, 10, exit);
  }

  entry_count_per_bank = soc_pb_tcam_bank_entry_size_to_entry_count_get(
                           entry_size
                         );
  max_nof_entries      = entry_count_per_bank * SOC_PB_TCAM_NOF_BANKS;

  /*
   * Initialize the entry priority list
   */
  /*priorities = soc_pb_sw_db_tcam_db_priorities_get(                 unit,tcam_db_id); */
  /* soc_sand_SAND_SORTED_LIST_INFO_clear(unit,priorities); */

  sorted_init_info.prime_handle  = unit;
  sorted_init_info.sec_handle    = 0;
  sorted_init_info.list_size     = max_nof_entries;
  sorted_init_info.key_size      = SOC_PB_TCAM_DB_LIST_KEY_SIZE * sizeof(uint8);
  sorted_init_info.data_size     = SOC_PB_TCAM_DB_LIST_DATA_SIZE * sizeof(uint8);
  sorted_init_info.get_entry_fun = soc_pb_tcam_db_data_structure_entry_get;
  sorted_init_info.set_entry_fun = soc_pb_tcam_db_data_structure_entry_set;

  /*won't work anymore!!!*/
  priorities = (SOC_SAND_SORTED_LIST_PTR)(-1) ;
  res = soc_sand_sorted_list_create(
          unit,
          &priorities,
          sorted_init_info
        );
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->tcam.tcam_db[tcam_db_id].priorities = priorities ;
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  /*
   * Initialize the entry_id -> location hash table
   */
  /* entry_id_to_location = soc_pb_sw_db_tcam_db_entry_id_to_location_get(
                           unit,
                           tcam_db_id
                         ); */
  /* soc_sand_SAND_HASH_TABLE_INFO_clear(entry_id_to_location); */

  hash_init_info.prime_handle  = unit;
  hash_init_info.sec_handle    = 0;
  hash_init_info.table_size    = max_nof_entries;
  hash_init_info.table_width   = max_nof_entries;
  hash_init_info.key_size      = SOC_PB_TCAM_DB_HASH_TBL_KEY_SIZE * sizeof(uint8);
  hash_init_info.data_size     = SOC_PB_TCAM_DB_HASH_TBL_KEY_SIZE * sizeof(uint8);
  hash_init_info.get_entry_fun = soc_pb_tcam_db_data_structure_entry_get;
  hash_init_info.set_entry_fun = soc_pb_tcam_db_data_structure_entry_set;
  /* will not work!! */
  res = soc_sand_hash_table_create(
          unit,
          &entry_id_to_location,
          hash_init_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  Soc_petra_sw_db.soc_petra_device_sw_db[unit]->tcam.tcam_db[tcam_db_id].entry_id_to_location = entry_id_to_location ;
  /*
   * Initialize the location table
   */
  location_tbl = (SOC_PB_TCAM_LOCATION *) sal_dpp_alloc(
                                        max_nof_entries * sizeof(SOC_PB_TCAM_LOCATION),
                                        "location_tbl MEM"
                                      );
  soc_pb_sw_db_tcam_db_location_tbl_set(
    unit,
    tcam_db_id,
    location_tbl
  );

  soc_pb_sw_db_tcam_db_entry_size_set(
    unit,
    tcam_db_id,
    entry_size
  );

  soc_pb_sw_db_tcam_db_prefix_size_set(
    unit,
    tcam_db_id,
    prefix_size
  );

  soc_pb_sw_db_tcam_db_priority_mode_set(
    unit,
    tcam_db_id,
    prio_mode
  );

  for (bank_id = 0; bank_id < SOC_PB_TCAM_NOF_BANKS; ++bank_id)
  {
    soc_pb_sw_db_tcam_db_bank_used_set(
      unit,
      tcam_db_id,
      bank_id,
      FALSE
    );
  }

  soc_pb_sw_db_tcam_db_valid_set(
    unit,
    tcam_db_id,
    TRUE
  );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tcam_db_create_unsafe()", 0, 0);
}

uint32
  soc_pb_tcam_db_create_verify(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_IN  SOC_PB_TCAM_BANK_ENTRY_SIZE entry_size,
    SOC_SAND_IN  uint32               prefix_size,
    SOC_SAND_IN  SOC_PB_TCAM_DB_PRIO_MODE    prio_mode
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_TCAM_DB_CREATE_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(tcam_db_id, SOC_PB_TCAM_TCAM_DB_ID_MAX, SOC_PB_TCAM_TCAM_DB_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(entry_size, SOC_PB_TCAM_ENTRY_SIZE_MAX, SOC_PB_TCAM_ENTRY_SIZE_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(prefix_size, SOC_PB_TCAM_PREFIX_SIZE_MAX, SOC_PB_TCAM_PREFIX_SIZE_OUT_OF_RANGE_ERR, 30, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(prio_mode, SOC_PB_TCAM_PRIO_MODE_MAX, SOC_PB_TCAM_PRIO_MODE_OUT_OF_RANGE_ERR, 40, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tcam_db_create_verify()", 0, 0);
}

/*********************************************************************
*     Destroys a TCAM database and frees the resources
 *     allocated to it.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_tcam_db_destroy_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id
  )
{
  uint32
    res = SOC_SAND_OK,
    data_indx;
  uint32
    bank_id,
    shift,
    nof_dbs;
  SOC_SAND_HASH_TABLE_PTR
    entry_id_to_location;
  SOC_SAND_HASH_TABLE_ITER
    iter;
  uint8
    key_buffer[SOC_PB_TCAM_DB_HASH_TBL_KEY_SIZE],
    base,
    ndx;
  SOC_PB_TCAM_LOCATION
    *location_tbl;
  SOC_SAND_SORTED_LIST_PTR
    prio_list;
  SOC_PB_TCAM_PREFIX
    prefix;
  SOC_SAND_OCC_BM_PTR
    occ_bm;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_TCAM_DB_DESTROY_UNSAFE);

  SOC_PB_TCAM_DB_EXISTS_ASSERT;

  /*
   *  Verify
   */
  res = soc_pb_tcam_db_destroy_verify(
          unit,
          tcam_db_id
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  /*
   *  Invalidate all entries
   */
  location_tbl         = soc_pb_sw_db_tcam_db_location_tbl_get(
                           unit,
                           tcam_db_id
                         );
  entry_id_to_location = soc_pb_sw_db_tcam_db_entry_id_to_location_get(
                           unit,
                           tcam_db_id
                         );
  SOC_SAND_HASH_TABLE_ITER_SET_BEGIN(&iter);
  res = soc_sand_hash_table_get_next(
          unit,
          entry_id_to_location,
          &iter,
          key_buffer,
          &data_indx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  while (!SOC_SAND_HASH_TABLE_ITER_IS_END(&iter))
  {
    res = soc_pb_tcam_bank_entry_invalidate_unsafe(
            unit,
            location_tbl[data_indx].bank_id,
            location_tbl[data_indx].entry
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    res = soc_sand_hash_table_get_next(
            unit,
            entry_id_to_location,
            &iter,
            key_buffer,
            &data_indx
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }

  for (bank_id = 0; bank_id < SOC_PB_TCAM_NOF_BANKS; ++bank_id)
  {
    if (soc_pb_sw_db_tcam_db_bank_used_get(unit, tcam_db_id, bank_id))
    {
      /*
       *  Release grabbed bank prefixes (the interface is broken here, it should really get fixed)
       */
      soc_pb_sw_db_tcam_db_prefix_get(
        unit,
        tcam_db_id,
        bank_id,
        &prefix
      );

      shift = 4 - prefix.length;
      base = (uint8) (prefix.bits << shift);
      for (ndx = 0; ndx < SOC_SAND_BIT(shift); ++ndx)
      {
        soc_pb_sw_db_tcam_managed_bank_prefix_db_set(
          unit,
          bank_id,
          base | ndx,
          SOC_PB_TCAM_MAX_NOF_LISTS
        );
      }

      /*
       *  If the bank is not used at all now, mark it uninitialized
       */
      nof_dbs = soc_pb_sw_db_tcam_managed_bank_nof_db_get(
                  unit,
                  bank_id
                );
      soc_pb_sw_db_tcam_managed_bank_nof_db_set(
        unit,
        bank_id,
        --nof_dbs
      );
      if (nof_dbs == 0)
      {
        soc_pb_sw_db_tcam_bank_valid_set(
          unit,
          bank_id,
          FALSE
        );
        occ_bm = soc_pb_sw_db_tcam_bank_entries_used_get(
                   unit,
                   bank_id
                 );
        soc_sand_occ_bm_destroy(unit, occ_bm);
      }
    }
  }

  /*
   * Free allocated memory
   */
  prio_list = soc_pb_sw_db_tcam_db_priorities_get(
                unit,
                tcam_db_id
              );
  sal_free(location_tbl);
  res = soc_sand_sorted_list_destroy(
          unit,
          prio_list
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  res = soc_sand_hash_table_destroy(
          unit,
          entry_id_to_location
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  soc_pb_sw_db_tcam_db_valid_set(
    unit,
    tcam_db_id,
    FALSE
  );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tcam_db_destroy_unsafe()", 0, 0);
}

uint32
  soc_pb_tcam_db_destroy_verify(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_TCAM_DB_DESTROY_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(tcam_db_id, SOC_PB_TCAM_TCAM_DB_ID_MAX, SOC_PB_TCAM_TCAM_DB_ID_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tcam_db_destroy_verify()", 0, 0);
}

/*********************************************************************
*     Adds a TCAM bank to a database.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_tcam_db_bank_add_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_IN  uint32               bank_id,
    SOC_SAND_IN  SOC_PB_TCAM_PREFIX          *prefix
  )
{
  uint32
    res = SOC_SAND_OK;
  uint8
    is_used;
  SOC_PB_TCAM_BANK_ENTRY_SIZE
    db_entry_size,
    bank_entry_size;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_TCAM_DB_BANK_ADD_UNSAFE);
  
  SOC_SAND_CHECK_NULL_INPUT(prefix);

  SOC_PB_TCAM_DB_EXISTS_ASSERT;

  /*
   *  Verify
   */
  res = soc_pb_tcam_db_bank_add_verify(
          unit,
          tcam_db_id,
          bank_id,
          prefix
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  /*
   *  Check whether this bank is already used by this database
   */
  is_used = soc_pb_sw_db_tcam_db_bank_used_get(
              unit,
              tcam_db_id,
              bank_id
            );
  if (is_used)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_TCAM_DB_BANK_ALREADY_USED_ERR, 20, exit);
  }

  /*
   *  Make sure the new bank's entry size is the same as the database's
   */
  db_entry_size   = soc_pb_sw_db_tcam_db_entry_size_get(
                      unit,
                      tcam_db_id
                    );
  bank_entry_size = soc_pb_sw_db_tcam_bank_entry_size_get(
                      unit,
                      bank_id
                    );
  if (db_entry_size != bank_entry_size)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_TCAM_DB_ENTRY_SIZE_MISMATCH_ERR, 30, exit);
  }

  /*
   *  Mark the bank as used by this database
   */
  soc_pb_sw_db_tcam_db_bank_used_set(
    unit,
    tcam_db_id,
    bank_id,
    TRUE
  );

  soc_pb_sw_db_tcam_db_prefix_set(
    unit,
    tcam_db_id,
    bank_id,
    prefix
  );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tcam_db_bank_add_unsafe()", 0, 0);
}

uint32
  soc_pb_tcam_db_bank_add_verify(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_IN  uint32               bank_id,
    SOC_SAND_IN  SOC_PB_TCAM_PREFIX          *prefix
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_TCAM_DB_BANK_ADD_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(tcam_db_id, SOC_PB_TCAM_TCAM_DB_ID_MAX, SOC_PB_TCAM_TCAM_DB_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(bank_id, SOC_PB_TCAM_BANK_ID_MAX, SOC_PB_TCAM_BANK_ID_OUT_OF_RANGE_ERR, 20, exit);
  SOC_PB_STRUCT_VERIFY(SOC_PB_TCAM_PREFIX, prefix, 30, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tcam_db_bank_add_verify()", 0, 0);
}

/*********************************************************************
*     Removes a bank from the database's resource pool.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_tcam_db_bank_remove_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_IN  uint32               bank_id
  )
{
  uint32
    res = SOC_SAND_OK;
  uint8
    is_used;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_TCAM_DB_BANK_REMOVE_UNSAFE);

  SOC_PB_TCAM_DB_EXISTS_ASSERT;

  /*
   *  Verify
   */
  res = soc_pb_tcam_db_bank_remove_verify(
          unit,
          tcam_db_id,
          bank_id
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  /*
   *  Make sure the bank is used by this database
   */
  is_used = soc_pb_sw_db_tcam_db_bank_used_get(
              unit,
              tcam_db_id,
              bank_id
            );
  if (!is_used)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_TCAM_DB_BANK_NOT_USED_ERR, 20, exit);
  }

  soc_pb_sw_db_tcam_db_bank_used_set(
    unit,
    tcam_db_id,
    bank_id,
    FALSE
  );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tcam_db_bank_remove_unsafe()", 0, 0);
}

uint32
  soc_pb_tcam_db_bank_remove_verify(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_IN  uint32               bank_id
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_TCAM_DB_BANK_REMOVE_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(tcam_db_id, SOC_PB_TCAM_TCAM_DB_ID_MAX, SOC_PB_TCAM_TCAM_DB_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(bank_id, SOC_PB_TCAM_BANK_ID_MAX, SOC_PB_TCAM_BANK_ID_OUT_OF_RANGE_ERR, 20, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tcam_db_bank_remove_verify()", 0, 0);
}

/*********************************************************************
*     Returns the number of banks occupied by the database.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_tcam_db_nof_banks_get_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_OUT uint32               *nof_banks
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    bank_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_TCAM_DB_NOF_BANKS_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(nof_banks);

  /*
   *  Verify
   */
  res = soc_pb_tcam_db_nof_banks_get_verify(
          unit,
          tcam_db_id
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_PB_TCAM_DB_EXISTS_ASSERT;

  *nof_banks = 0;
  for (bank_id = 0; bank_id < SOC_PB_TCAM_NOF_BANKS; ++bank_id)
  {
    if (soc_pb_sw_db_tcam_db_bank_used_get(unit, tcam_db_id, bank_id))
    {
      ++*nof_banks;
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tcam_db_nof_banks_get_unsafe()", 0, 0);
}

uint32
  soc_pb_tcam_db_nof_banks_get_verify(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_TCAM_DB_NOF_BANKS_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(tcam_db_id, SOC_PB_TCAM_TCAM_DB_ID_MAX, SOC_PB_TCAM_TCAM_DB_ID_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tcam_db_nof_banks_get_verify()", 0, 0);
}

/*********************************************************************
*     Returns the database's prefix in the bank.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_tcam_db_bank_prefix_get_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_IN  uint32               bank_id,
    SOC_SAND_OUT SOC_PB_TCAM_PREFIX          *prefix
  )
{
  uint32
    res = SOC_SAND_OK;
  uint8
    has_bank;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_TCAM_DB_BANK_PREFIX_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(prefix);

  /*
   *  Verify
   */
  res = soc_pb_tcam_db_bank_prefix_get_verify(
          unit,
          tcam_db_id,
          bank_id
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  SOC_PB_TCAM_PREFIX_clear(prefix);

  SOC_PB_TCAM_DB_EXISTS_ASSERT;

  has_bank = soc_pb_sw_db_tcam_db_bank_used_get(
               unit,
               tcam_db_id,
               bank_id
             );
  if (!has_bank)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_TCAM_DB_BANK_NOT_USED_ERR, 20, exit);
  }

  soc_pb_sw_db_tcam_db_prefix_get(
    unit,
    tcam_db_id,
    bank_id,
    prefix
  );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tcam_db_bank_prefix_get_unsafe()", 0, 0);
}

uint32
  soc_pb_tcam_db_bank_prefix_get_verify(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_IN  uint32               bank_id
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_TCAM_DB_BANK_PREFIX_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(tcam_db_id, SOC_PB_TCAM_TCAM_DB_ID_MAX, SOC_PB_TCAM_TCAM_DB_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(bank_id, SOC_PB_TCAM_BANK_ID_MAX, SOC_PB_TCAM_BANK_ID_OUT_OF_RANGE_ERR, 20, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tcam_db_bank_prefix_get_verify()", 0, 0);
}

/*********************************************************************
*     Returns the database's entry size.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_tcam_db_entry_size_get_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_OUT SOC_PB_TCAM_BANK_ENTRY_SIZE *entry_size
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_TCAM_DB_ENTRY_SIZE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(entry_size);

  /*
   *  Verify
   */
  res = soc_pb_tcam_db_entry_size_get_verify(
          unit,
          tcam_db_id
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  SOC_PB_TCAM_DB_EXISTS_ASSERT;

  *entry_size = soc_pb_sw_db_tcam_db_entry_size_get(
                  unit,
                  tcam_db_id
                );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tcam_db_entry_size_get_unsafe()", 0, 0);
}

uint32
  soc_pb_tcam_db_entry_size_get_verify(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_TCAM_DB_ENTRY_SIZE_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(tcam_db_id, SOC_PB_TCAM_TCAM_DB_ID_MAX, SOC_PB_TCAM_TCAM_DB_ID_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tcam_db_entry_size_get_verify()", 0, 0);
}

/*********************************************************************
*     Add an entry to a database.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_tcam_db_entry_add_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_IN  uint32               entry_id,
    SOC_SAND_IN  uint16                priority,
    SOC_SAND_IN  SOC_PB_TCAM_ENTRY           *entry,
    SOC_SAND_IN  uint32                action,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE    *success
  )
{
  uint32
    res = SOC_SAND_OK;
  uint8
    has_banks,
    entry_exists;
  SOC_PB_TCAM_LOCATION
    location;
  SOC_PB_TCAM_ENTRY
    entry_buffer;
  uint32
    nof_entries_free;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_TCAM_DB_ENTRY_ADD_UNSAFE);

  /*
   *  Verify
   */
  res = soc_pb_tcam_db_entry_add_verify(
          unit,
          tcam_db_id,
          entry_id,
          priority,
          entry,
          action
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_PB_TCAM_DB_EXISTS_ASSERT;

  /*
   *  Check whether this database has any banks
   */
  has_banks = soc_pb_tcam_db_has_banks(
                unit,
                tcam_db_id
              );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  if (!has_banks)
  {
    *success = SOC_SAND_FAILURE_OUT_OF_RESOURCES;
    SOC_PB_DO_NOTHING_AND_EXIT;
  }

  /*
   *  The user has to remove the entry first, if it exists
   */
  res = soc_pb_tcam_db_entry_exists_get(
          unit,
          tcam_db_id,
          entry_id,
          &entry_exists
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  if (entry_exists)
  {
    *success = SOC_SAND_FAILURE_REMOVE_ENTRY_FIRST;
    SOC_PB_DO_NOTHING_AND_EXIT;
  }

  res = soc_pb_tcam_db_free_location_allocate(
          unit,
          tcam_db_id,
          priority,
          &location,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  if (*success == SOC_SAND_SUCCESS)
  {
    /*
     *  Add the new entry to the priorities list
     */
    res = soc_pb_tcam_db_priority_list_entry_add(
            unit,
            tcam_db_id,
            &location,
            entry_id,
            priority,
            success
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

    /*
     *  Add the new entry to the entry_id -> location hash table
     */
    res = soc_pb_tcam_db_entry_id_to_location_entry_add(
            unit,
            tcam_db_id,
            entry_id,
            &location
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

    entry_buffer = *entry;
    res = soc_pb_tcam_db_entry_prefix_stamp(
            unit,
            tcam_db_id,
            location.bank_id,
            &entry_buffer
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

    res = soc_pb_tcam_bank_entry_set_unsafe(
            unit,
            location.bank_id,
            location.entry,
            &entry_buffer,
            action
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
	nof_entries_free = soc_pb_sw_db_tcam_bank_nof_entries_free_get(
						unit,
						location.bank_id);

	soc_pb_sw_db_tcam_bank_nof_entries_free_set(
		unit,
		location.bank_id,
		nof_entries_free-1);
  }
  else
  {
    *success = SOC_SAND_FAILURE_OUT_OF_RESOURCES;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tcam_db_entry_add_unsafe()", 0, 0);
}

uint32
  soc_pb_tcam_db_entry_add_verify(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_IN  uint32               entry_id,
    SOC_SAND_IN  uint16                priority,
    SOC_SAND_IN  SOC_PB_TCAM_ENTRY           *entry,
    SOC_SAND_IN  uint32                action
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_TCAM_DB_ENTRY_ADD_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(tcam_db_id, SOC_PB_TCAM_TCAM_DB_ID_MAX, SOC_PB_TCAM_TCAM_DB_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(entry_id, SOC_PB_TCAM_ENTRY_ID_MAX, SOC_PB_TCAM_ENTRY_ID_OUT_OF_RANGE_ERR, 20, exit);

  SOC_PB_STRUCT_VERIFY(SOC_PB_TCAM_ENTRY, entry, 40, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tcam_db_entry_add_verify()", 0, 0);
}

/*********************************************************************
*     Reads an entry from the database.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_tcam_db_entry_get_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_IN  uint32               entry_id,
    SOC_SAND_OUT uint16                *priority,
    SOC_SAND_OUT SOC_PB_TCAM_ENTRY           *entry,
    SOC_SAND_OUT uint32                *action,
    SOC_SAND_OUT uint8               *found
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    stored_entry_id;
  SOC_PB_TCAM_LOCATION
    location;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_TCAM_DB_ENTRY_GET_UNSAFE);

  /*
   *  Verify
   */
  res = soc_pb_tcam_db_entry_get_verify(
          unit,
          tcam_db_id,
          entry_id
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  SOC_SAND_CHECK_NULL_INPUT(priority);
  SOC_SAND_CHECK_NULL_INPUT(entry);
  SOC_SAND_CHECK_NULL_INPUT(action);
  SOC_SAND_CHECK_NULL_INPUT(found);

  SOC_PB_TCAM_ENTRY_clear(entry);

  SOC_PB_TCAM_DB_EXISTS_ASSERT;

  res = soc_pb_tcam_db_entry_id_to_location_entry_get(
          unit,
          tcam_db_id,
          entry_id,
          &location,
          found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  if (*found)
  {
    /*
     * Get the TCAM key and action
     */
    res = soc_pb_tcam_bank_entry_get_unsafe(
            unit,
            location.bank_id,
            location.entry,
            entry,
            action
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    /*
     * Get the entry's priority
     */
    res = soc_pb_tcam_db_priority_list_entry_get(
            unit,
            tcam_db_id,
            &location,
            &stored_entry_id,
            priority
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    if (stored_entry_id != entry_id)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PB_TCAM_DB_METADATA_INCONSISTENT_ERR, 50, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tcam_db_entry_get_unsafe()", 0, 0);
}

uint32
  soc_pb_tcam_db_entry_get_verify(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_IN  uint32               entry_id
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_TCAM_DB_ENTRY_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(tcam_db_id, SOC_PB_TCAM_TCAM_DB_ID_MAX, SOC_PB_TCAM_TCAM_DB_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(entry_id, SOC_PB_TCAM_ENTRY_ID_MAX, SOC_PB_TCAM_ENTRY_ID_OUT_OF_RANGE_ERR, 20, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tcam_db_entry_get_verify()", 0, 0);
}

/*********************************************************************
*     Search for an entry in a database using the same logic
 *     used by the hardware.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_tcam_db_entry_search_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_IN  SOC_PB_TCAM_ENTRY           *key,
    SOC_SAND_OUT uint32               *entry_id,
    SOC_SAND_OUT uint8               *found
  )
{
  uint32
    res = SOC_SAND_OK;
  uint8
    hit=0;
  int32
    cmp;
  uint16
    priority;
  SOC_PB_TCAM_BANK_ENTRY_SIZE
    entry_size;
  SOC_PB_TCAM_LOCATION
    first_hit,
    current;
  SOC_PB_TCAM_ENTRY
    stamped;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_TCAM_DB_ENTRY_SEARCH_UNSAFE);

  /*
   *  Verify
   */
  res = soc_pb_tcam_db_entry_search_verify(
          unit,
          tcam_db_id,
          key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  SOC_SAND_CHECK_NULL_INPUT(key);
  SOC_SAND_CHECK_NULL_INPUT(entry_id);
  SOC_SAND_CHECK_NULL_INPUT(found);

  SOC_PB_TCAM_DB_EXISTS_ASSERT;

  SOC_PB_TCAM_LOCATION_clear(&current);
  SOC_PB_TCAM_LOCATION_clear(&first_hit);

  /*
   *  Make sure the database's keys are not 288-bit wide
   */
  entry_size = soc_pb_sw_db_tcam_db_entry_size_get(
                 unit,
                 tcam_db_id
               );
  if (entry_size == SOC_PB_TCAM_BANK_ENTRY_SIZE_288_BITS)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_TCAM_DB_ENTRY_SEARCH_SIZE_NOT_SUPPORTED_ERR, 20, exit);
  }

  /*
   *  Search for the first match
   */
  *found = FALSE;
  for (current.bank_id = 0; current.bank_id < SOC_PB_TCAM_NOF_BANKS; ++current.bank_id)
  {
    if (soc_pb_sw_db_tcam_db_bank_used_get(unit, tcam_db_id, current.bank_id))
    {
      /*
       *  Stamp the key to make sure we only check the relevant database
       */
      stamped = *key;
      res = soc_pb_tcam_db_entry_prefix_stamp(
              unit,
              tcam_db_id,
              current.bank_id,
              &stamped
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

      res = soc_pb_tcam_tbl_bank_compare(
              unit,
              current.bank_id,
              key,
              &current.entry,
              &hit
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

      if (hit)
      {
        if (!*found)
        {
          first_hit = current;
          *found    = TRUE;
        }
        else
        {
          cmp = soc_pb_tcam_location_cmp(
                  unit,
                  tcam_db_id,
                  &first_hit,
                  &current
                );
          if (cmp < 0)
          {
            first_hit = current;
          }
        }
      }
    }
  }

  if (*found)
  {
    /*
     *  Translate the physical location into entry_id
     */
    res = soc_pb_tcam_db_priority_list_entry_get(
            unit,
            tcam_db_id,
            &first_hit,
            entry_id,
            &priority
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tcam_db_entry_search_unsafe()", 0, 0);
}

uint32
  soc_pb_tcam_db_entry_search_verify(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_IN  SOC_PB_TCAM_ENTRY           *key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_TCAM_DB_ENTRY_SEARCH_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(tcam_db_id, SOC_PB_TCAM_TCAM_DB_ID_MAX, SOC_PB_TCAM_TCAM_DB_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_STRUCT_VERIFY(SOC_PB_TCAM_ENTRY, key, 20, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tcam_db_entry_search_verify()", 0, 0);
}

/*********************************************************************
*     Removes an entry from the database.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_tcam_db_entry_remove_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_IN  uint32               entry_id
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_TCAM_LOCATION
    location;
  uint8
    found;
  uint32
    nof_entries_free;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_TCAM_DB_ENTRY_REMOVE_UNSAFE);

  SOC_PB_TCAM_DB_EXISTS_ASSERT;

  /*
   *  Verify
   */
  res = soc_pb_tcam_db_entry_remove_verify(
          unit,
          tcam_db_id,
          entry_id
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  res = soc_pb_tcam_db_entry_id_to_location_entry_get(
          unit,
          tcam_db_id,
          entry_id,
          &location,
          &found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  if (found)
  {
    res = soc_pb_tcam_bank_entry_invalidate_unsafe(
            unit,
            location.bank_id,
            location.entry
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    res = soc_pb_tcam_db_priority_list_entry_remove(
            unit,
            tcam_db_id,
            &location
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    res = soc_pb_tcam_db_entry_id_to_location_entry_remove(
            unit,
            tcam_db_id,
            entry_id
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
	nof_entries_free = soc_pb_sw_db_tcam_bank_nof_entries_free_get(
						unit,
						location.bank_id);

	soc_pb_sw_db_tcam_bank_nof_entries_free_set(
		unit,
		location.bank_id,
		nof_entries_free+1);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tcam_db_entry_remove_unsafe()", 0, 0);
}

uint32
  soc_pb_tcam_db_entry_remove_verify(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_IN  uint32               entry_id
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_TCAM_DB_ENTRY_REMOVE_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(tcam_db_id, SOC_PB_TCAM_TCAM_DB_ID_MAX, SOC_PB_TCAM_TCAM_DB_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(entry_id, SOC_PB_TCAM_ENTRY_ID_MAX, SOC_PB_TCAM_ENTRY_ID_OUT_OF_RANGE_ERR, 20, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tcam_db_entry_remove_verify()", 0, 0);
}

/*********************************************************************
*     Queries whether the bank belongs to the database or not.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_tcam_db_is_bank_used_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_IN  uint32               bank_id,
    SOC_SAND_OUT uint8               *is_used
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_TCAM_DB_IS_BANK_USED_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(is_used);

  SOC_PB_TCAM_DB_EXISTS_ASSERT;

  /*
   *  Verify
   */
  res = soc_pb_tcam_db_is_bank_used_verify(
          unit,
          tcam_db_id,
          bank_id
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  *is_used = soc_pb_sw_db_tcam_db_bank_used_get(
               unit,
               tcam_db_id,
               bank_id
             );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tcam_db_is_bank_used_unsafe()", 0, 0);
}

uint32
  soc_pb_tcam_db_is_bank_used_verify(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_IN  uint32               bank_id
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_TCAM_DB_IS_BANK_USED_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(tcam_db_id, SOC_PB_TCAM_TCAM_DB_ID_MAX, SOC_PB_TCAM_TCAM_DB_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(bank_id, SOC_PB_TCAM_BANK_ID_MAX, SOC_PB_TCAM_BANK_ID_OUT_OF_RANGE_ERR, 20, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tcam_db_is_bank_used_verify()", 0, 0);
}

void
  SOC_PB_TCAM_PREFIX_clear(
    SOC_SAND_OUT SOC_PB_TCAM_PREFIX *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_TCAM_PREFIX));
  info->bits = 0;
  info->length = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_TCAM_LOCATION_clear(
    SOC_SAND_OUT SOC_PB_TCAM_LOCATION *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_TCAM_LOCATION));
  info->bank_id = 0;
  info->entry = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_TCAM_RANGE_clear(
    SOC_SAND_OUT SOC_PB_TCAM_RANGE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_TCAM_RANGE));
  SOC_PB_TCAM_LOCATION_clear(&(info->min));
  SOC_PB_TCAM_LOCATION_clear(&(info->max));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_TCAM_ENTRY_clear(
    SOC_SAND_OUT SOC_PB_TCAM_ENTRY *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_TCAM_ENTRY));
  for (ind = 0; ind < SOC_PB_TCAM_ENTRY_MAX_LEN; ++ind)
  {
    info->value[ind] = 0;
  }
  for (ind = 0; ind < SOC_PB_TCAM_ENTRY_MAX_LEN; ++ind)
  {
    info->mask[ind] = 0;
  }
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

uint32
  SOC_PB_TCAM_PREFIX_verify(
    SOC_SAND_IN  SOC_PB_TCAM_PREFIX *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->bits, SOC_PB_TCAM_BITS_MAX, SOC_PB_TCAM_BITS_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->length, SOC_PB_TCAM_LENGTH_MAX, SOC_PB_TCAM_LENGTH_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_TCAM_PREFIX_verify()",0,0);
}

uint32
  SOC_PB_TCAM_LOCATION_verify(
    SOC_SAND_IN  SOC_PB_TCAM_LOCATION *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->bank_id, SOC_PB_TCAM_BANK_ID_MAX, SOC_PB_TCAM_BANK_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->entry, SOC_PB_TCAM_ENTRY_MAX, SOC_PB_TCAM_ENTRY_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_TCAM_LOCATION_verify()",0,0);
}

uint32
  SOC_PB_TCAM_RANGE_verify(
    SOC_SAND_IN  SOC_PB_TCAM_RANGE *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PB_STRUCT_VERIFY(SOC_PB_TCAM_LOCATION, &(info->min), 10, exit);
  SOC_PB_STRUCT_VERIFY(SOC_PB_TCAM_LOCATION, &(info->max), 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_TCAM_RANGE_verify()",0,0);
}

uint32
  SOC_PB_TCAM_ENTRY_verify(
    SOC_SAND_IN  SOC_PB_TCAM_ENTRY *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind = 0; ind < SOC_PB_TCAM_ENTRY_MAX_LEN; ++ind)
  {
  }
  for (ind = 0; ind < SOC_PB_TCAM_ENTRY_MAX_LEN; ++ind)
  {
  }

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_TCAM_ENTRY_verify()",0,0);
}

#if SOC_PB_DEBUG_IS_LVL1

const char*
  SOC_PB_TCAM_BANK_ENTRY_SIZE_to_string(
    SOC_SAND_IN  SOC_PB_TCAM_BANK_ENTRY_SIZE enum_val
  )
{
  return SOC_TMC_TCAM_BANK_ENTRY_SIZE_to_string(enum_val);
}

const char*
  SOC_PB_TCAM_DB_PRIO_MODE_to_string(
    SOC_SAND_IN  SOC_PB_TCAM_DB_PRIO_MODE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PB_TCAM_DB_PRIO_MODE_BANK:
    str = "bank";
  break;
  case SOC_PB_TCAM_DB_PRIO_MODE_INTERLACED:
    str = "interlaced";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

void
  SOC_PB_TCAM_PREFIX_print(
    SOC_SAND_IN  SOC_PB_TCAM_PREFIX *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("bits: %u\n\r",info->bits);
  soc_sand_os_printf("length: %u\n\r",info->length);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_TCAM_LOCATION_print(
    SOC_SAND_IN  SOC_PB_TCAM_LOCATION *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("bank_id: %u\n\r",info->bank_id);
  soc_sand_os_printf("entry: %u\n\r",info->entry);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_TCAM_RANGE_print(
    SOC_SAND_IN  SOC_PB_TCAM_RANGE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("min:");
  SOC_PB_TCAM_LOCATION_print(&(info->min));
  soc_sand_os_printf("max:");
  SOC_PB_TCAM_LOCATION_print(&(info->max));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_TCAM_ENTRY_print(
    SOC_SAND_IN  SOC_PB_TCAM_ENTRY *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind = 0; ind < SOC_PB_TCAM_ENTRY_MAX_LEN; ++ind)
  {
    soc_sand_os_printf("value[%u]: %u\n\r",ind,info->value[ind]);
  }
  for (ind = 0; ind < SOC_PB_TCAM_ENTRY_MAX_LEN; ++ind)
  {
    soc_sand_os_printf("mask[%u]: %u\n\r",ind,info->mask[ind]);
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif /* SOC_PB_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

