/* $Id: pb_tcam_mgmt.c,v 1.10 Broadcom SDK $
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


/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/Petra/PB_TM/pb_framework.h>
#include <soc/dpp/Petra/PB_TM/pb_tcam_mgmt.h>

#include <soc/dpp/SAND/Utils/sand_sorted_list.h>

#include <soc/dpp/Petra/PB_TM/pb_sw_db_tcam_mgmt.h>
#include <soc/dpp/Petra/PB_TM/pb_tcam.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_chip_regs.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_reg_access.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PB_TCAM_MGMT_PROFILE_ID_MAX                            (SOC_SAND_UINT_MAX)
#define SOC_PB_TCAM_MGMT_TCAM_DB_ID_MAX                            (SOC_SAND_UINT_MAX)
#define SOC_PB_TCAM_MGMT_CYCLE_MAX                                 (SOC_PB_TCAM_NOF_ACCESS_CYCLES-1)
#define SOC_PB_TCAM_MGMT_UNIFORM_PREFIX_MAX                        (SOC_PB_TCAM_NOF_ACCESS_CYCLES-1)
#define SOC_PB_TCAM_MGMT_MIN_BANKS_MAX                             (SOC_PB_TCAM_NOF_ACCESS_CYCLES-1)
#define SOC_PB_TCAM_MGMT_USER_DATA_MAX                             (SOC_SAND_U32_MAX)
#define SOC_PB_TCAM_MGMT_ENTRY_ID_MAX                              (SOC_SAND_UINT_MAX)
#define SOC_PB_TCAM_MGMT_ACTION_MAX                                (SOC_SAND_U32_MAX)
#define SOC_PB_TCAM_MGMT_BANK_ID_MAX                               (SOC_SAND_UINT_MAX)





/* } */
/*************
 * MACROS    *
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

/* $Id: pb_tcam_mgmt.c,v 1.10 Broadcom SDK $
 *  Internal functions
 */

STATIC
  uint8
    soc_pb_tcam_managed_bank_prefix_is_free(
      SOC_SAND_IN int      unit,
      SOC_SAND_IN uint32      bank_id,
      SOC_SAND_IN SOC_PB_TCAM_PREFIX *prefix
    )
{
  uint8
    base,
    ndx;
  uint32
    shift;
  uint8
    is_free = TRUE;

  shift = 4 - prefix->length;
  base = (uint8) (prefix->bits << shift);
  for (ndx = 0; is_free && (ndx < (uint32) (0x1 << shift)); ++ndx)
  {
    if (soc_pb_sw_db_tcam_managed_bank_prefix_db_get(unit, bank_id, base | ndx)
         != SOC_PB_TCAM_MAX_NOF_LISTS)
    {
      is_free = FALSE;
    }
  }
  return is_free;
}

STATIC
  uint8
    soc_pb_tcam_managed_bank_prefix_find(
      SOC_SAND_IN  int      unit,
      SOC_SAND_IN  uint32      bank_id,
      SOC_SAND_IN  uint32      prefix_size,
      SOC_SAND_OUT SOC_PB_TCAM_PREFIX *prefix
    )
{
  uint32
    lsb_ndx,
    msb_ndx;
  uint32
    shift,
    nof_lsb,
    nof_msb;
  uint8
    lsb_free,
    success = FALSE;

  shift   = 4 - prefix_size;
  nof_lsb = 1 << shift;
  nof_msb = 1 << prefix_size;
  for (msb_ndx = 0; !success && (msb_ndx < nof_msb); ++msb_ndx)
  {
    lsb_free = TRUE;
    for (lsb_ndx = 0; lsb_free && (lsb_ndx < nof_lsb); ++lsb_ndx)
    {
      if (soc_pb_sw_db_tcam_managed_bank_prefix_db_get(unit, bank_id, (msb_ndx << shift) | lsb_ndx)
           != SOC_PB_TCAM_MAX_NOF_LISTS)
      {
        lsb_free = FALSE;
      }
    }
    if (lsb_free)
    {
      prefix->bits   = (uint8) msb_ndx;
      prefix->length = prefix_size;
      success = TRUE;
    }
  }
  return success;
}

STATIC
  void
    soc_pb_tcam_managed_bank_prefix_grab(
      SOC_SAND_IN int      unit,
      SOC_SAND_IN uint32      bank_id,
      SOC_SAND_IN SOC_PB_TCAM_PREFIX *prefix,
      SOC_SAND_IN uint32      tcam_db_id
    )
{
  uint8
    base,
    ndx;
  uint32
    shift;

  shift = 4 - prefix->length;
  base = (uint8) (prefix->bits << shift);
  for (ndx = 0; ndx < (0x1 << shift); ++ndx)
  {
    soc_pb_sw_db_tcam_managed_bank_prefix_db_set(
      unit,
      bank_id,
      base | ndx,
      tcam_db_id
    );
  }
}

STATIC
  uint8
    soc_pb_tcam_access_profile_bank_find(
      SOC_SAND_IN    int               unit,
      SOC_SAND_IN    SOC_PB_TCAM_ACCESS_CYCLE    cycle,
      SOC_SAND_IN    SOC_PB_TCAM_BANK_ENTRY_SIZE entry_size,
      SOC_SAND_IN    uint32               prefix_size,
      SOC_SAND_IN    uint8               force_prefix,
      SOC_SAND_INOUT SOC_PB_TCAM_PREFIX          *prefix,
      SOC_SAND_OUT   uint32               *bank_id
    )
{
  uint8
    success = FALSE;
  uint32
    bank_ndx;

  for (bank_ndx = 0; !success && (bank_ndx < SOC_PB_TCAM_NOF_BANKS); ++bank_ndx)
  {
		if(soc_pb_sw_db_tcam_bank_nof_entries_free_get(unit,bank_ndx) == 0)
		{
			continue;
		}

    if (!soc_pb_sw_db_tcam_bank_valid_get(unit, bank_ndx))
    {
      /*
       *  None of the prefixes are taken, we can choose arbitrarily
       */
      prefix->bits   = 0;
      prefix->length = prefix_size;

      success = TRUE;
    }
    else if ((soc_pb_sw_db_tcam_bank_entry_size_get(unit, bank_ndx) == entry_size)
              && (soc_pb_sw_db_tcam_managed_bank_access_profile_get(unit, bank_ndx, cycle) == SOC_PB_TCAM_MAX_NOF_LISTS))
    {
      if (force_prefix)
      {
        if (soc_pb_tcam_managed_bank_prefix_is_free(unit, bank_ndx, prefix))
        {
          success = TRUE;
        }
      }
      else
      {
        success = soc_pb_tcam_managed_bank_prefix_find(
                    unit,
                    bank_ndx,
                    prefix_size,
                    prefix
                  );
      }
    }
    *bank_id = bank_ndx;
  }

  return success;
}

STATIC
  void
    soc_pb_tcam_mgmt_access_profiles_interlaced_get(
      SOC_SAND_IN  int            unit,
      SOC_SAND_IN  SOC_PB_TCAM_ACCESS_CYCLE cycle,
      SOC_SAND_OUT uint32            *nof_profiles,
      SOC_SAND_OUT uint32            accessed_profile_ids[]
    )
{
  uint32
    bank_id,
    profile_id,
    accessed_profile_id;
  uint8
    is_interlaced,
    is_new_profile;

  *nof_profiles = 0;
  for (bank_id = 0; bank_id < SOC_PB_TCAM_NOF_BANKS; ++bank_id)
  {
    profile_id = soc_pb_sw_db_tcam_managed_bank_access_profile_get(
                   unit,
                   bank_id,
                   cycle
                 );
    if (profile_id != SOC_PB_TCAM_MAX_NOF_LISTS)
    {
      is_interlaced = (soc_pb_sw_db_tcam_db_priority_mode_get(
                         unit,
                         soc_pb_sw_db_tcam_access_profile_db_get(
                           unit,
                           profile_id
                         )
                       ) == SOC_PB_TCAM_DB_PRIO_MODE_INTERLACED) ? TRUE : FALSE;
      if (is_interlaced)
      {
        /*
         *  Check if we already encountered this profile_id
         */
        is_new_profile = TRUE;
        for (accessed_profile_id = 0; accessed_profile_id < *nof_profiles; ++accessed_profile_id)
        {
          if (accessed_profile_ids[accessed_profile_id] == profile_id)
          {
            is_new_profile = FALSE;
          }
        }
        if (is_new_profile)
        {
          accessed_profile_ids[(*nof_profiles)++] = profile_id;
        }
      }
    }
  }
}

STATIC
  uint32
    soc_pb_tcam_mgmt_access_profile_sort_by_nof_banks(
      SOC_SAND_IN    int unit,
      SOC_SAND_IN    uint32 nof_profiles,
      SOC_SAND_INOUT uint32 profile_id[]
    )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    i,
    j,
    lhs_banks,
    rhs_banks,
    tmp;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  if (nof_profiles == 0)
  {
    SOC_PB_DO_NOTHING_AND_EXIT;
  }

  /*
   *  Nothing fancy, just bubble sort
   */
  for (i = 0; i < nof_profiles - 1; ++i)
  {
    for (j = 0; j < nof_profiles - i - 1; ++j)
    {
      res = soc_pb_tcam_db_nof_banks_get_unsafe(
              unit,
              soc_pb_sw_db_tcam_access_profile_db_get(
                unit,
                profile_id[j]
              ),
              &lhs_banks
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
      res = soc_pb_tcam_db_nof_banks_get_unsafe(
              unit,
              soc_pb_sw_db_tcam_access_profile_db_get(
                unit,
                profile_id[j + 1]
              ),
              &rhs_banks
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

      if (lhs_banks < rhs_banks)
      {
        tmp               = profile_id[j];
        profile_id[j]     = profile_id[j + 1];
        profile_id[j + 1] = tmp;
      }
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tcam_mgmt_access_profile_sort_by_nof_banks()", 0, 0);
}

STATIC
  uint32
    soc_pb_tcam_mgmt_access_profile_access_device_set(
      SOC_SAND_IN int             unit,
      SOC_SAND_IN uint32             profile_id,
      SOC_SAND_IN SOC_PB_TCAM_ACCESS_CYCLE  cycle,
      SOC_SAND_IN SOC_PB_TCAM_ACCESS_DEVICE access_device
    )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  soc_pb_sw_db_tcam_access_profile_access_device_set(
    unit,
    profile_id,
    access_device
  );
  soc_pb_sw_db_tcam_mgmt_access_profile_in_access_device_set(
    unit,
    cycle,
    access_device,
    profile_id
  );

  SOC_PB_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tcam_mgmt_access_profile_access_device_set()", 0, 0);
}

STATIC
  uint32
    soc_pb_tcam_mgmt_access_devices_allocate(
      SOC_SAND_IN int unit
    )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    cycle,
    nof_profiles,
    accessed_profile,
    accessed_profile_ids[SOC_PB_TCAM_NOF_BANKS];

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  /*
   *  Per access cycle, sort the databases with priority mode SOC_PB_TCAM_DB_PRIO_MODE_INTERLACED
   *  according to the number of banks they occupy in descending order and assign them to PD1, PD2,
   *  SEL3, SEL4
   */
  for (cycle = 0; cycle < SOC_PB_TCAM_NOF_CYCLES; ++cycle)
  {
    /*
     *  Check which access profiles are accessed in this cycle
     */
    soc_pb_tcam_mgmt_access_profiles_interlaced_get(
      unit,
      cycle,
      &nof_profiles,
      accessed_profile_ids
    );

    /*
     *  Sort the accessed profiles according to bank count
     */
    res = soc_pb_tcam_mgmt_access_profile_sort_by_nof_banks(
            unit,
            nof_profiles,
            accessed_profile_ids
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    /*
     *  Assign access devices to access profiles
     */
    for (accessed_profile = 0; accessed_profile < nof_profiles; ++accessed_profile)
    {
      soc_pb_tcam_mgmt_access_profile_access_device_set(
        unit,
        accessed_profile_ids[accessed_profile],
        cycle,
        accessed_profile
      );
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tcam_mgmt_access_devices_allocate()", 0, 0);
}

STATIC
  uint32
    soc_pb_tcam_access_profile_bank_add(
      SOC_SAND_IN  int            unit,
      SOC_SAND_IN  uint32            profile_id,
      SOC_SAND_IN  uint32            tcam_db_id,
      SOC_SAND_IN  SOC_PB_TCAM_ACCESS_CYCLE cycle,
      SOC_SAND_IN  uint8            uniform_prefix,
      SOC_SAND_IN  SOC_PB_TCAM_MGMT_SIGNAL  callback,
      SOC_SAND_IN  uint32             user_data,
      SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE *success
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    bank_id=0,
    prefix_size,
    nof_dbs;
  uint8
    already_has_prefix,
    bank_found;
  SOC_PB_TCAM_PREFIX
    prefix;
  SOC_PB_TCAM_BANK_ENTRY_SIZE
    entry_size;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  /*
   *  First, we check for prefix constraints
   */
  already_has_prefix = FALSE;
  SOC_PB_TCAM_PREFIX_clear(&prefix);
  if (uniform_prefix)
  {
    for (bank_id = 0; !already_has_prefix && (bank_id < SOC_PB_TCAM_NOF_BANKS); ++bank_id)
    {
      if (soc_pb_sw_db_tcam_db_bank_used_get(unit, tcam_db_id, bank_id))
      {
        soc_pb_sw_db_tcam_db_prefix_get(
          unit,
          tcam_db_id,
          bank_id,
          &prefix
        );
        already_has_prefix = TRUE;
      }
    }
  }

  /*
   *  Now we search for a bank that can be accessed in the requested cycle, and has a free
   *  compatible prefix
   */
  entry_size  = soc_pb_sw_db_tcam_db_entry_size_get(
                  unit,
                  tcam_db_id
                );
  prefix_size = soc_pb_sw_db_tcam_db_prefix_size_get(
                  unit,
                  tcam_db_id
                );
  bank_found  = soc_pb_tcam_access_profile_bank_find(
                  unit,
                  cycle,
                  entry_size,
                  prefix_size,
                  uniform_prefix && already_has_prefix,
                  &prefix,
                  &bank_id
                );
  if (bank_found)
  {
    if (!soc_pb_sw_db_tcam_bank_valid_get(unit, bank_id))
    {
      /*
       *  This bank is not yet initialized
       */
      res = soc_pb_tcam_bank_init_unsafe(
              unit,
              bank_id,
              entry_size
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    }

    nof_dbs = soc_pb_sw_db_tcam_managed_bank_nof_db_get(
                unit,
                bank_id
              );
    soc_pb_sw_db_tcam_managed_bank_nof_db_set(
      unit,
      bank_id,
      nof_dbs + 1
    );

    soc_pb_tcam_managed_bank_prefix_grab(
      unit,
      bank_id,
      &prefix,
      tcam_db_id
    );

    /*
     *  Add the bank to the TCAM database only if it isn't already there
     */
    if (!soc_pb_sw_db_tcam_db_bank_used_get(unit, tcam_db_id, bank_id))
    {
      res = soc_pb_tcam_db_bank_add_unsafe(
              unit,
              tcam_db_id,
              bank_id,
              &prefix
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    }

    /*
     *  Mark the bank as used in this cycle
     */
    soc_pb_sw_db_tcam_managed_bank_access_profile_set(
      unit,
      bank_id,
      cycle,
      profile_id
    );

    if (soc_pb_sw_db_tcam_db_priority_mode_get(unit, tcam_db_id) == SOC_PB_TCAM_DB_PRIO_MODE_INTERLACED)
    {
      res = soc_pb_tcam_mgmt_access_devices_allocate(
              unit
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    }

    /*
     *  Signal the owner of the database about the new bank
     */
    res = callback(
            unit,
            user_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }

  *success = bank_found ? SOC_SAND_SUCCESS : SOC_SAND_FAILURE_OUT_OF_RESOURCES;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tcam_access_profile_bank_add()", 0, 0);
}

/*
 *  API functions
 */

/*********************************************************************
*     Creates a new access profile for an existing TCAM
 *     database.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_tcam_access_profile_create_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               profile_id,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_IN  SOC_PB_TCAM_ACCESS_CYCLE    cycle,
    SOC_SAND_IN  uint8               uniform_prefix,
    SOC_SAND_IN  uint32               min_banks,
    SOC_SAND_IN  SOC_PB_TCAM_MGMT_SIGNAL     callback,
    SOC_SAND_IN  uint32                user_data,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE    *success
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    ndx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_TCAM_ACCESS_PROFILE_CREATE_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(success);

  if (soc_pb_sw_db_tcam_access_profile_valid_get(unit, profile_id))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_TCAM_ACCESS_PROFILE_ALREADY_EXISTS_ERR, 10, exit);
  }

  if (!soc_pb_sw_db_tcam_db_valid_get(unit, tcam_db_id))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_TCAM_DB_DOESNT_EXIST_ERR, 20, exit);
  }

  /*
   * Verify
   */
  res = soc_pb_tcam_access_profile_create_verify(
          unit,
          profile_id,
          tcam_db_id,
          cycle,
          uniform_prefix,
          min_banks,
          callback,
          user_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  soc_pb_sw_db_tcam_access_profile_db_set(
    unit,
    profile_id,
    tcam_db_id
  );

  soc_pb_sw_db_tcam_access_profile_cycle_set(
    unit,
    profile_id,
    cycle
  );

  soc_pb_sw_db_tcam_access_profile_uniform_prefix_set(
    unit,
    profile_id,
    uniform_prefix
  );

  soc_pb_sw_db_tcam_access_profile_min_banks_set(
    unit,
    profile_id,
    min_banks
  );

  soc_pb_sw_db_tcam_access_profile_callback_set(
    unit,
    profile_id,
    callback
  );

  soc_pb_sw_db_tcam_access_profile_user_data_set(
    unit,
    profile_id,
    user_data
  );

  *success = SOC_SAND_SUCCESS;
  for (ndx = 0; (*success == SOC_SAND_SUCCESS) && (ndx < min_banks); ++ndx)
  {
    res = soc_pb_tcam_access_profile_bank_add(
            unit,
            profile_id,
            tcam_db_id,
            cycle,
            uniform_prefix,
            callback,
            user_data,
            success
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }

  if (*success == SOC_SAND_SUCCESS)
  {
    soc_pb_sw_db_tcam_access_profile_valid_set(
      unit,
      profile_id,
      TRUE
    );
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tcam_access_profile_create_unsafe()", 0, 0);
}

uint32
  soc_pb_tcam_access_profile_create_verify(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               profile_id,
    SOC_SAND_IN  uint32               tcam_db_id,
    SOC_SAND_IN  SOC_PB_TCAM_ACCESS_CYCLE    cycle,
    SOC_SAND_IN  uint8               uniform_prefix,
    SOC_SAND_IN  uint32               min_banks,
    SOC_SAND_IN  SOC_PB_TCAM_MGMT_SIGNAL     callback,
    SOC_SAND_IN  uint32                user_data
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_TCAM_ACCESS_PROFILE_CREATE_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(profile_id, SOC_PB_TCAM_MGMT_PROFILE_ID_MAX, SOC_PB_TCAM_MGMT_PROFILE_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(tcam_db_id, SOC_PB_TCAM_MGMT_TCAM_DB_ID_MAX, SOC_PB_TCAM_MGMT_TCAM_DB_ID_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(cycle, SOC_PB_TCAM_MGMT_CYCLE_MAX, SOC_PB_TCAM_MGMT_CYCLE_OUT_OF_RANGE_ERR, 30, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(uniform_prefix, SOC_PB_TCAM_MGMT_UNIFORM_PREFIX_MAX, SOC_PB_TCAM_MGMT_UNIFORM_PREFIX_OUT_OF_RANGE_ERR, 40, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(min_banks, SOC_PB_TCAM_MGMT_MIN_BANKS_MAX, SOC_PB_TCAM_MGMT_MIN_BANKS_OUT_OF_RANGE_ERR, 50, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(user_data, SOC_PB_TCAM_MGMT_USER_DATA_MAX, SOC_PB_TCAM_MGMT_USER_DATA_OUT_OF_RANGE_ERR, 60, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tcam_access_profile_create_verify()", 0, 0);
}

uint32
  soc_pb_tcam_access_profile_destroy_unsafe(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 profile_id
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    bank_id,
    stored_id,
    tcam_db_id;
  SOC_PB_TCAM_ACCESS_CYCLE
    cycle;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  soc_pb_sw_db_tcam_access_profile_valid_set(
    unit,
    profile_id,
    FALSE
  );

  /*
   *  Release bank accesses
   */
  for (bank_id = 0; bank_id < SOC_PB_TCAM_NOF_BANKS; ++bank_id)
  {
    for (cycle = SOC_PB_TCAM_ACCESS_CYCLE_0; cycle < SOC_PB_TCAM_NOF_ACCESS_CYCLES; ++cycle)
    {
      stored_id = soc_pb_sw_db_tcam_managed_bank_access_profile_get(
                    unit,
                    bank_id,
                    cycle
                  );
      if (stored_id == profile_id)
      {
        soc_pb_sw_db_tcam_managed_bank_access_profile_set(
          unit,
          bank_id,
          cycle,
          SOC_PB_TCAM_MAX_NOF_LISTS
        );
      }
    }
  }

  tcam_db_id = soc_pb_sw_db_tcam_access_profile_db_get(
                 unit,
                 profile_id
               );
  if (soc_pb_sw_db_tcam_db_priority_mode_get(unit, tcam_db_id) == SOC_PB_TCAM_DB_PRIO_MODE_INTERLACED)
  {
    res = soc_pb_tcam_mgmt_access_devices_allocate(
      unit
    );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tcam_access_profile_destroy_unsafe()", 0, 0);
}

/*********************************************************************
*     Adds an entry to a TCAM database, and allocates more
 *     TCAM banks, as needed.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_tcam_managed_db_entry_add_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               profile_id,
    SOC_SAND_IN  uint32               entry_id,
    SOC_SAND_IN  uint16                priority,
    SOC_SAND_IN  SOC_PB_TCAM_ENTRY           *entry,
    SOC_SAND_IN  uint32                action,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE    *success
  )
{
  uint32
    res = SOC_SAND_OK,
    user_data;
  uint32
    tcam_db_id;
  SOC_PB_TCAM_ACCESS_CYCLE
    cycle;
  uint8
    uniform_prefix;
  SOC_PB_TCAM_MGMT_SIGNAL
    callback;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_TCAM_MANAGED_DB_ENTRY_ADD_UNSAFE);

  /*
   * Verify
   */
  res = soc_pb_tcam_managed_db_entry_add_verify(
          unit,
          profile_id,
          entry_id,
          priority,
          entry,
          action
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  if (!soc_pb_sw_db_tcam_access_profile_valid_get(unit, profile_id))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_TCAM_ACCESS_PROFILE_DOESNT_EXIST_ERR, 10, exit);
  }

  /*
   *  Get managed database parameters
   */
  tcam_db_id     = soc_pb_sw_db_tcam_access_profile_db_get(
                     unit,
                     profile_id
                   );
  cycle          = soc_pb_sw_db_tcam_access_profile_cycle_get(
                     unit,
                     profile_id
                   );
  uniform_prefix = soc_pb_sw_db_tcam_access_profile_uniform_prefix_get(
                     unit,
                     profile_id
                   );
  callback       = soc_pb_sw_db_tcam_access_profile_callback_get(
                     unit,
                     profile_id
                   );
  user_data      = soc_pb_sw_db_tcam_access_profile_user_data_get(
                     unit,
                     profile_id
                   );

  /*
   *  First attempt at entry insertion
   */
  res = soc_pb_tcam_db_entry_add_unsafe(
          unit,
          tcam_db_id,
          entry_id,
          priority,
          entry,
          action,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  if (*success == SOC_SAND_FAILURE_OUT_OF_RESOURCES)
  {
    res = soc_pb_tcam_access_profile_bank_add(
            unit,
            profile_id,
            tcam_db_id,
            cycle,
            uniform_prefix,
            callback,
            user_data,
            success
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    if (*success == SOC_SAND_SUCCESS)
    {
      /*
       *  Second attempt at insertion, this should not fail
       */
      res = soc_pb_tcam_db_entry_add_unsafe(
              unit,
              tcam_db_id,
              entry_id,
              priority,
              entry,
              action,
              success
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tcam_managed_db_entry_add_unsafe()", 0, 0);
}

uint32
  soc_pb_tcam_managed_db_entry_add_verify(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               profile_id,
    SOC_SAND_IN  uint32               entry_id,
    SOC_SAND_IN  uint16                priority,
    SOC_SAND_IN  SOC_PB_TCAM_ENTRY           *entry,
    SOC_SAND_IN  uint32                action
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_TCAM_MANAGED_DB_ENTRY_ADD_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(profile_id, SOC_PB_TCAM_MGMT_PROFILE_ID_MAX, SOC_PB_TCAM_MGMT_PROFILE_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(entry_id, SOC_PB_TCAM_MGMT_ENTRY_ID_MAX, SOC_PB_TCAM_MGMT_ENTRY_ID_OUT_OF_RANGE_ERR, 20, exit);
  SOC_PB_STRUCT_VERIFY(SOC_PB_TCAM_ENTRY, entry, 40, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(action, SOC_PB_TCAM_MGMT_ACTION_MAX, SOC_PB_TCAM_MGMT_ACTION_OUT_OF_RANGE_ERR, 50, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tcam_managed_db_entry_add_verify()", 0, 0);
}

/*********************************************************************
*     Adds an entry to a TCAM database, and allocates more
 *     TCAM banks, as needed.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_tcam_managed_db_entry_remove_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               profile_id,
    SOC_SAND_IN  uint32               entry_id
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    tcam_db_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_TCAM_MANAGED_DB_ENTRY_REMOVE_UNSAFE);

  if (!soc_pb_sw_db_tcam_access_profile_valid_get(unit, profile_id))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_TCAM_ACCESS_PROFILE_DOESNT_EXIST_ERR, 10, exit);
  }

  /*
   * Verify
   */
  res = soc_pb_tcam_managed_db_entry_remove_verify(
          unit,
          profile_id,
          entry_id
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  tcam_db_id = soc_pb_sw_db_tcam_access_profile_db_get(
                 unit,
                 profile_id
               );

  res = soc_pb_tcam_db_entry_remove_unsafe(
          unit,
          tcam_db_id,
          entry_id
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tcam_managed_db_entry_remove_unsafe()", 0, 0);
}

uint32
  soc_pb_tcam_managed_db_entry_remove_verify(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               profile_id,
    SOC_SAND_IN  uint32               entry_id
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_TCAM_MANAGED_DB_ENTRY_REMOVE_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(profile_id, SOC_PB_TCAM_MGMT_PROFILE_ID_MAX, SOC_PB_TCAM_MGMT_PROFILE_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(entry_id, SOC_PB_TCAM_MGMT_ENTRY_ID_MAX, SOC_PB_TCAM_MGMT_ENTRY_ID_OUT_OF_RANGE_ERR, 20, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tcam_managed_db_entry_remove_verify()", 0, 0);
}

/*********************************************************************
*     Returns the ID of the database that is accessed in a
 *     bank in a particular cycle.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_tcam_managed_bank_accessed_db_get_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               bank_id,
    SOC_SAND_IN  SOC_PB_TCAM_ACCESS_CYCLE    cycle,
    SOC_SAND_OUT uint32               *tcam_db_id
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    profile_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_TCAM_MANAGED_BANK_ACCESSED_DB_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(tcam_db_id);

  /*
   * Verify
   */
  res = soc_pb_tcam_managed_bank_accessed_db_get_verify(
          unit,
          bank_id,
          cycle
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  profile_id = soc_pb_sw_db_tcam_managed_bank_access_profile_get(
                  unit,
                  bank_id,
                  cycle
                );
  if (profile_id == SOC_PB_TCAM_MAX_NOF_LISTS)
  {
    *tcam_db_id = SOC_PB_TCAM_MAX_NOF_LISTS;
  }
  else
  {
    *tcam_db_id = soc_pb_sw_db_tcam_access_profile_db_get(
                    unit,
                    profile_id
                  );
  }

  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tcam_managed_bank_accessed_db_get_unsafe()", 0, 0);
}

uint32
  soc_pb_tcam_managed_bank_accessed_db_get_verify(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               bank_id,
    SOC_SAND_IN  SOC_PB_TCAM_ACCESS_CYCLE    cycle
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_TCAM_MANAGED_BANK_ACCESSED_DB_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(bank_id, SOC_PB_TCAM_MGMT_BANK_ID_MAX, SOC_PB_TCAM_MGMT_BANK_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(cycle, SOC_PB_TCAM_MGMT_CYCLE_MAX, SOC_PB_TCAM_MGMT_CYCLE_OUT_OF_RANGE_ERR, 20, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tcam_managed_bank_accessed_db_get_verify()", 0, 0);
}

/*********************************************************************
*     Returns the access device assigned to an access profile.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_tcam_access_profile_access_device_get_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               profile_id,
    SOC_SAND_OUT SOC_PB_TCAM_ACCESS_DEVICE   *access_device
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_TCAM_ACCESS_PROFILE_ACCESS_DEVICE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(access_device);

   /*
    * Verify
    */
   res = soc_pb_tcam_access_profile_access_device_get_verify(
           unit,
           profile_id
         );
   SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  *access_device = soc_pb_sw_db_tcam_access_profile_access_device_get(
                     unit,
                     profile_id
                   );

  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tcam_access_profile_access_device_get_unsafe()", 0, 0);
}

uint32
  soc_pb_tcam_access_profile_access_device_get_verify(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               profile_id
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_TCAM_ACCESS_PROFILE_ACCESS_DEVICE_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(profile_id, SOC_PB_TCAM_MGMT_PROFILE_ID_MAX, SOC_PB_TCAM_MGMT_PROFILE_ID_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tcam_access_profile_access_device_get_verify()", 0, 0);
}

void
  soc_pb_PB_TCAM_ENTRY_clear(
    SOC_SAND_OUT SOC_PB_TCAM_ENTRY *info
  )
{
  uint32 ind;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_TCAM_ENTRY));
  for (ind=0; ind<SOC_PB_TCAM_ENTRY_MAX_LEN; ++ind)
  {
    info->value[ind] = 0;
  }
  for (ind=0; ind<SOC_PB_TCAM_ENTRY_MAX_LEN; ++ind)
  {
    info->mask[ind] = 0;
  }
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PB_DEBUG
void
  soc_pb_PB_TCAM_ENTRY_print(
    SOC_SAND_IN SOC_PB_TCAM_ENTRY *info
  )
{
  uint32 ind=0;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind=0; ind<SOC_PB_TCAM_ENTRY_MAX_LEN; ++ind)
  {
    soc_sand_os_printf("value[%u]: %u\n\r",ind,info->value[ind]);
  }
  for (ind=0; ind<SOC_PB_TCAM_ENTRY_MAX_LEN; ++ind)
  {
    soc_sand_os_printf("mask[%u]: %u\n\r",ind,info->mask[ind]);
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}
#endif /* SOC_PB_DEBUG */

#if SOC_PB_DEBUG_IS_LVL1

const char*
  SOC_PB_TCAM_ACCESS_CYCLE_to_string(
    SOC_SAND_IN  SOC_PB_TCAM_ACCESS_CYCLE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PB_TCAM_ACCESS_CYCLE_0:
    str = "cycle_0";
  break;
  case SOC_PB_TCAM_ACCESS_CYCLE_1:
    str = "cycle_1";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_PB_TCAM_ACCESS_DEVICE_to_string(
    SOC_SAND_IN  SOC_PB_TCAM_ACCESS_DEVICE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PB_TCAM_ACCESS_DEVICE_PD1:
    str = "pd1";
  break;
  case SOC_PB_TCAM_ACCESS_DEVICE_PD2:
    str = "pd2";
  break;
  case SOC_PB_TCAM_ACCESS_DEVICE_SEL3:
    str = "sel3";
  break;
  case SOC_PB_TCAM_ACCESS_DEVICE_SEL4:
    str = "sel4";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

#endif /* SOC_PB_DEBUG_IS_LVL1 */

#include <soc/dpp/SAND/Utils/sand_footer.h>

