/* $Id: petra_multicast_tests.c,v 1.14 Broadcom SDK $
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
#include <soc/dpp/SAND/Utils/sand_workload_status.h>

#include <soc/dpp/Petra/petra_framework.h>
#include <soc/dpp/Petra/petra_api_general.h>
#include <soc/dpp/Petra/petra_api_multicast_egress.h>
#include <soc/dpp/Petra/petra_api_multicast_ingress.h>
#include <soc/dpp/Petra/petra_multicast_egress.h>
#include <soc/dpp/Petra/petra_multicast_ingress.h>
#include <soc/dpp/Petra/petra_api_framework.h>
#include <soc/dpp/Petra/petra_sw_db.h>
#include <soc/dpp/Petra/petra_general.h>
#include <soc/dpp/Petra/petra_multicast_tests.h>


/* } */

/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PETRA_MULT_TEST_1_ING_MC_GROUP_ID     0
#define SOC_PETRA_MULT_TEST_1_ING_GROUP_SIZE      5

#define SOC_PETRA_MULT_TEST_2_ING_MC_GROUP_ID_1   0
#define SOC_PETRA_MULT_TEST_2_ING_GROUP_SIZE_1    3
#define SOC_PETRA_MULT_TEST_2_ING_MC_GROUP_ID_2   1
#define SOC_PETRA_MULT_TEST_2_ING_GROUP_SIZE_2    3

#define SOC_PETRA_MULT_TEST_3_ING_MC_GROUP_ID_1   0
#define SOC_PETRA_MULT_TEST_3_ING_GROUP_SIZE_1    5
#define SOC_PETRA_MULT_TEST_3_ING_MC_GROUP_ID_2   0
#define SOC_PETRA_MULT_TEST_3_ING_GROUP_SIZE_2    3

#define SOC_PETRA_MULT_TEST_4_ING_MC_GROUP_ID_1   0
#define SOC_PETRA_MULT_TEST_4_ING_GROUP_SIZE_1    2
#define SOC_PETRA_MULT_TEST_4_NOF_ADDITIONS       100

#define SOC_PETRA_MULT_TEST_5_ING_MC_GROUP_ID     0
#define SOC_PETRA_MULT_TEST_5_ING_GROUP_SIZE      10

#define SOC_PETRA_MULT_TEST_6_ING_MC_GROUP_ID_1   0
#define SOC_PETRA_MULT_TEST_6_ING_GROUP_SIZE_1    3
#define SOC_PETRA_MULT_TEST_6_ING_GROUP_SIZE_2    3

#define SOC_PETRA_MULT_TEST_7_ING_MC_GROUP_ID     0
#define SOC_PETRA_MULT_TEST_7_ING_GROUP_SIZE      5
#define SOC_PETRA_MULT_TEST_7_EG_MC_GROUP_ID      16383

#define SOC_PETRA_MULT_TEST_8_ING_MC_GROUP_ID     0
#define SOC_PETRA_MULT_TEST_8_ING_GROUP_SIZE      1

#define SOC_PETRA_MULT_TEST_9_EG_MC_GROUP_ID      0
#define SOC_PETRA_MULT_TEST_9_EG_GROUP_SIZE       2
#define SOC_PETRA_MULT_TEST_9_BITMAP_RNG_LOW      15*1024
#define SOC_PETRA_MULT_TEST_9_BITMAP_RNG_HIGH     16*1024-1
#define SOC_PETRA_MULT_TEST_9_EG_MC_BITMAP_ID     SOC_PETRA_MULT_TEST_9_BITMAP_RNG_LOW

#define SOC_PETRA_MULT_TEST_10_ING_GROUP_SIZE_1   8*1024
#define SOC_PETRA_MULT_TEST_10_ING_MC_GROUP_ID_3  100
#define SOC_PETRA_MULT_TEST_10_ING_GROUP_SIZE_3   1
#define SOC_PETRA_MULT_TEST_10_EG_GROUP_SIZE_2    12*1024
#define SOC_PETRA_MULT_TEST_10_EG_GROUP_START_ID  16*1024-1

#define SOC_PETRA_MULT_TEST_11_ING_GROUP_SIZE_1   8
#define SOC_PETRA_MULT_TEST_11_ING_MC_GROUP_ID_3  5000
#define SOC_PETRA_MULT_TEST_11_ING_GROUP_SIZE_3   1
#define SOC_PETRA_MULT_TEST_11_EG_GROUP_SIZE_2    12


#define SOC_PETRA_MULT_TEST_12_ING_GROUP_SIZE_1   4*1024
#define SOC_PETRA_MULT_TEST_12_ING_MC_GROUP_ID_3  5000
#define SOC_PETRA_MULT_TEST_12_ING_GROUP_SIZE_3   1
#define SOC_PETRA_MULT_TEST_12_ING_GROUP_SIZE_4   4*1024
#define SOC_PETRA_MULT_TEST_12_EG_GROUP_SIZE_2    6*1024
#define SOC_PETRA_MULT_TEST_12_EG_GROUP_START_ID  16*1024-1

#define SOC_PETRA_MULT_TEST_13_EG_GROUP_SIZE_1    7

#define SOC_PETRA_MULT_TEST_14_EG_GROUP_START_ID  0
#define SOC_PETRA_MULT_TEST_14_EG_GROUP_START_ID_2 1
#define SOC_PETRA_MULT_TEST_14_NOF_ADDITIONS      10

#define SOC_PETRA_MULT_TEST_15_EG_GROUP_START_ID  0
#define SOC_PETRA_MULT_TEST_15_EG_GROUP_SIZE      12
#define SOC_PETRA_MULT_TEST_15_EG_GROUP_SIZE_PORT 6
#define SOC_PETRA_MULT_TEST_15_EG_GROUP_SIZE_VLAN 6

#define SOC_PETRA_MULT_TEST_16_EG_GROUP_START_ID  7
#define SOC_PETRA_MULT_TEST_16_EG_GROUP_SIZE      10
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
#if SOC_PETRA_DEBUG_IS_LVL1
/***********************************************************************
* Compare two groups,
* and return enum describing if groups are
* equal/ different sizes/ different in port ids/ different in copy-unique-data.
***********************************************************************/
SOC_SAND_RET
  soc_petra_mult_ing_groups_compare(
    SOC_SAND_IN  SOC_PETRA_MULT_ING_ENTRY        *group_1,
    SOC_SAND_IN  uint32                    size_group_1,
    SOC_SAND_IN  SOC_PETRA_MULT_ING_ENTRY        *group_2,
    SOC_SAND_IN  uint32                    size_group_2,
    SOC_SAND_OUT SOC_PETRA_MULT_TESTS_GROUP_DIFF *diff
  )
{
  SOC_SAND_RET
    soc_sand_ret;
  uint32
    in_group_i;

  soc_sand_ret = SOC_SAND_OK;

  if( (group_1 == NULL) ||
      (group_2 == NULL) ||
      (diff    == NULL)
    )
  {
    soc_sand_ret = SOC_SAND_NULL_POINTER_ERR;
    SOC_PETRA_DO_NOTHING_AND_EXIT;
  }


  if(size_group_1 != size_group_2)
  {
    *diff = SOC_PETRA_MULT_TESTS_GROUP_DIFF_SIZES;
    SOC_PETRA_DO_NOTHING_AND_EXIT;
  }

  for( in_group_i=0;
       in_group_i<size_group_1;
       ++in_group_i
     )
  {

    if( (group_1[in_group_i].destination.id) !=
        (group_2[in_group_i].destination.id)
      )
    {
      *diff = SOC_PETRA_MULT_TESTS_GROUP_DIFF_PORT_IDS;
      SOC_PETRA_DO_NOTHING_AND_EXIT;
    }

    if( (group_1[in_group_i].cud) !=
      (group_2[in_group_i].cud)
      )
    {
      *diff = SOC_PETRA_MULT_TESTS_GROUP_DIFF_CUD;
      SOC_PETRA_DO_NOTHING_AND_EXIT;
    }
  }

  *diff = SOC_PETRA_MULT_TESTS_GROUP_DIFF_EQUAL;
exit:
  return soc_sand_ret;
}

/***********************************************************************
* Compare two groups,
* and return enum describing if groups are
* equal/ different sizes/ different in port ids/ different in copy-unique-data.
***********************************************************************/
SOC_SAND_RET
  soc_petra_mult_eg_groups_compare(
    SOC_SAND_IN  SOC_PETRA_MULT_EG_ENTRY         *group_1,
    SOC_SAND_IN  uint32                    size_group_1,
    SOC_SAND_IN  SOC_PETRA_MULT_EG_ENTRY         *group_2,
    SOC_SAND_IN  uint32                    size_group_2,
    SOC_SAND_OUT SOC_PETRA_MULT_TESTS_GROUP_DIFF *diff
  )
{
  SOC_SAND_RET
    soc_sand_ret;
  uint32
    in_group_i;

  soc_sand_ret = SOC_SAND_OK;

  if( (group_1 == NULL) ||
      (group_2 == NULL) ||
      (diff    == NULL)
    )
  {
    soc_sand_ret = SOC_SAND_NULL_POINTER_ERR;
    SOC_PETRA_DO_NOTHING_AND_EXIT;
  }


  if(size_group_1 != size_group_2)
  {
    *diff = SOC_PETRA_MULT_TESTS_GROUP_DIFF_SIZES;
    SOC_PETRA_DO_NOTHING_AND_EXIT;
  }

  for( in_group_i=0;
       in_group_i<size_group_1;
       ++in_group_i
     )
  {

    if( (group_1[in_group_i].type == SOC_PETRA_MULT_EG_ENTRY_TYPE_OFP) &&
      (group_1[in_group_i].port) !=(group_2[in_group_i].port)
      )
    {
      *diff = SOC_PETRA_MULT_TESTS_GROUP_DIFF_PORT_IDS;
      SOC_PETRA_DO_NOTHING_AND_EXIT;
    }

#ifdef LINK_PB_LIBRARIES
    if( (group_1[in_group_i].type) !=
      (group_2[in_group_i].type)
      )
    {
      *diff = SOC_PETRA_MULT_TESTS_GROUP_DIFF_TYPE;
      SOC_PETRA_DO_NOTHING_AND_EXIT;
    }
    
    if( (group_1[in_group_i].type == SOC_PETRA_MULT_EG_ENTRY_TYPE_VLAN_PTR) &&
        (group_1[in_group_i].vlan_mc_id != group_2[in_group_i].vlan_mc_id))
    {
      *diff = SOC_PETRA_MULT_TESTS_GROUP_DIFF_VLAN_MC_IDS;
      SOC_PETRA_DO_NOTHING_AND_EXIT;
    }

#endif

    if( (group_1[in_group_i].cud) !=
      (group_2[in_group_i].cud)
      )
    {
      *diff = SOC_PETRA_MULT_TESTS_GROUP_DIFF_CUD;
      SOC_PETRA_DO_NOTHING_AND_EXIT;
    }
  }

  *diff = SOC_PETRA_MULT_TESTS_GROUP_DIFF_EQUAL;
exit:
  return soc_sand_ret;
}

void
  soc_petra_multicast_ing_group_print(
    SOC_SAND_IN SOC_PETRA_MULT_ING_ENTRY            *group,
    SOC_SAND_IN uint32                    group_size
  )
{
  uint32
    in_group_i;
  /*
   */

  for(in_group_i=0; in_group_i < group_size; ++in_group_i)
  {
    if( ((in_group_i%2) == 0) &&
        (in_group_i!=0)
      )
    {
      soc_sand_os_printf("\n\r");
    }
    soc_sand_os_printf(
      "{%2d-->%u ; 0x%X} ",
      group[in_group_i].destination.type,
      group[in_group_i].destination.id,
      group[in_group_i].cud
    );
  }
  soc_sand_os_printf("\n\r");
  return;
}

void
  soc_petra_multicast_eg_group_print(
    SOC_SAND_IN SOC_PETRA_MULT_EG_ENTRY            *group,
    SOC_SAND_IN uint32                    group_size
  )
{
  uint32
    val,
    in_group_i;

  for(in_group_i=0; in_group_i < group_size; ++in_group_i)
  {
    if( ((in_group_i%3) == 0) &&
        (in_group_i!=0)
      )
    {
      soc_sand_os_printf("\n\r");
    }
    if (group[in_group_i].type == SOC_PETRA_MULT_EG_ENTRY_TYPE_OFP)
    {
      val = group[in_group_i].port;
    }
    else
    {
      val = group[in_group_i].vlan_mc_id;
    }
    soc_sand_os_printf(
      "{%s; %02d ; 0x%X} ",
      soc_petra_PETRA_MULT_EG_ENTRY_TYPE_to_string(group[in_group_i].type),
      val,
      group[in_group_i].cud
    );
  }
  soc_sand_os_printf("\n\r");
  return;
}



void
  soc_petra_multicast_eg_VLAN_MEMBERSHIP_group_print(
    SOC_SAND_IN SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_GROUP  *group
  )
{
  uint32
    in_group_long,
    in_long_i=0;
  uint32
    port_val,
    bit_val;
  uint8
    bit,
    dont_print = FALSE;

  for(in_group_long=0;
      in_group_long < SOC_PETRA_MULT_EG_NOF_UINT32S_IN_BITMAP;
      ++in_group_long)
  {
    in_long_i = 0;
    while (!dont_print)
    {
      if ((in_long_i != 0) && (in_long_i % 8 == 0))
      {
        soc_sand_os_printf("\n\r");
      }
       bit = ((group->bitmap[in_group_long] & SOC_SAND_BIT(in_long_i)) > 0
             ?
             TRUE : FALSE);

       port_val = (in_group_long * SOC_SAND_NOF_BITS_IN_UINT32) + in_long_i;
       bit_val = (bit==TRUE ? 1 : 0 );
       soc_sand_os_printf("|%02d - %d  ",
                        port_val,
                        bit_val);

       if ((SOC_SAND_NOF_BITS_IN_UINT32 * in_group_long) + in_long_i
           == (SOC_PETRA_NOF_FAP_PORTS - 1))
       {
         dont_print = TRUE;
       }
       in_long_i++;
    }
  }
  soc_sand_os_printf("\n\r");
  return;
}


void
  soc_petra_multicast_group_print(
    SOC_SAND_IN SOC_PETRA_MULT_ENTRY            *group,
    SOC_SAND_IN uint32                    group_size,
    SOC_SAND_IN uint8                   is_ingress
  )
{
  SOC_PETRA_MULT_ING_ENTRY
    *ing_mc_group = NULL;
  SOC_PETRA_MULT_EG_ENTRY
    *eg_mc_group = NULL;
  uint32
    i;
  uint32
    res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  if(NULL == group)
  {
    soc_sand_os_printf("SOC_PETRA_MULT_ING_ENTRY *group --> got NULL pointer\n\r");
    SOC_PETRA_DO_NOTHING_AND_EXIT;
  }

  soc_sand_os_printf(
    "SOC_PETRA_MULT_ING_ENTRY *group --> has %u members",
    group_size
  );

  if(group_size == 0)
  {
    soc_sand_os_printf(".\n\r");
    SOC_PETRA_DO_NOTHING_AND_EXIT;
  }

  soc_sand_os_printf(":\n\r");

  if (is_ingress)
  {
    SOC_PETRA_ALLOC(ing_mc_group, SOC_PETRA_MULT_ING_ENTRY, group_size);

    for (i = 0 ; i < group_size ; i++)
    {
      ing_mc_group[i] = group->ing_entry;
    }
    soc_petra_multicast_ing_group_print(ing_mc_group , group_size);
  }
  else
  {
    SOC_PETRA_ALLOC(eg_mc_group, SOC_PETRA_MULT_EG_ENTRY, group_size);
    for (i = 0 ; i < group_size ; i++)
    {
      eg_mc_group[i] = group->eg_entry;
    }
    soc_petra_multicast_eg_group_print(eg_mc_group , group_size);
  }

  soc_sand_os_printf("\n\r");

exit:
  SOC_PETRA_FREE(ing_mc_group);
  SOC_PETRA_FREE(eg_mc_group);
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  soc_petra_mult_tests_create_ing_multicast_group(
    SOC_SAND_IN  uint32                  mc_group_size,
    SOC_SAND_OUT SOC_PETRA_MULT_ING_ENTRY     *mc_group
  )
{
  uint32
    indx;

  for (indx = 0 ; indx < mc_group_size ; indx++)
  {
    mc_group[indx].destination.type = SOC_PETRA_DEST_TYPE_QUEUE;
    mc_group[indx].destination.id = indx;
    mc_group[indx].cud = 0xabcd;
  }
}

void
  soc_petra_mult_tests_create_eg_multicast_group(
    SOC_SAND_IN  uint32                  mc_group_size,
    SOC_SAND_OUT SOC_PETRA_MULT_EG_ENTRY       *mc_group
  )
{
  uint32
    indx;

  for (indx = 0 ; indx < mc_group_size ; indx++)
  {
    mc_group[indx].port = (indx % 80);
    mc_group[indx].cud = 0xabcd;
  }
}

void
  soc_petra_mult_tests_create_eg_vlan_membership_multicast_group(
    SOC_SAND_OUT SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_GROUP   *bitmap_mc_group
  )
{
  uint32
    indx;

  for (indx = 0 ; indx < SOC_PETRA_MULT_EG_NOF_UINT32S_IN_BITMAP-1 ; indx++)
  {
    bitmap_mc_group->bitmap[indx] = 0xabcdabcd;
  }
  bitmap_mc_group->bitmap[indx] = 0xabcd;
}

uint32
  soc_petra_mult_tests_get_ing_mc_group_from_hw(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint16                  mc_group_id,
    SOC_SAND_OUT SOC_PETRA_MULT_ING_ENTRY      *mc_group,
    SOC_SAND_OUT uint32                 *mc_group_size_exact,
    SOC_SAND_OUT uint16                  *list_of_next_entries
  )
{
  uint16
    next_entry = 1,
    tbl_entry_id;
  SOC_PETRA_MULT_ENTRY
    mc_entry_group[SOC_PETRA_MULT_ING_NOF_DESTINATIONS_PER_ENTRY];
  uint32
    ing_mc_entry_size = SOC_PETRA_MULT_ING_NOF_DESTINATIONS_PER_ENTRY,
    mc_group_size_exact_var = 0,
    i,
    indx = 0;
  uint32
    driver_ret = SOC_SAND_OK;

  tbl_entry_id = mc_group_id;

  while (next_entry != soc_petra_mc_null_id(unit,TRUE))
  {
    if (ing_mc_entry_size == SOC_PETRA_MULT_ING_NOF_DESTINATIONS_PER_ENTRY)
    {
      driver_ret = soc_petra_mult_ing_entry_content_get(
                     unit,
                     tbl_entry_id,
                     mc_entry_group,
                     &ing_mc_entry_size,
                     &next_entry
                   );

      if(soc_sand_get_error_code_from_error_word(driver_ret) != SOC_SAND_OK)
      {
        SOC_PETRA_DO_NOTHING_AND_EXIT;
      }

      list_of_next_entries[indx] = next_entry;

      mc_group_size_exact_var += ing_mc_entry_size;

      for (i = 0 ; i < ing_mc_entry_size ; i++)
      {
        mc_group[indx * SOC_PETRA_MULT_ING_NOF_DESTINATIONS_PER_ENTRY + i] =
          mc_entry_group[i].ing_entry;
      }
    }

    indx++;
    tbl_entry_id = next_entry;

  }

  *mc_group_size_exact = mc_group_size_exact_var;

exit:
  return driver_ret;
}

uint32
  soc_petra_mult_tests_get_eg_mc_group_from_hw(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint16                  mc_group_id,
    SOC_SAND_OUT SOC_PETRA_MULT_EG_ENTRY       *mc_group,
    SOC_SAND_OUT uint32                 *mc_group_size_exact,
    SOC_SAND_OUT uint16                  *list_of_next_entries
  )
{
  uint16
    next_entry = 1,
    tbl_entry_id;
  SOC_PETRA_MULT_ENTRY
    mc_entry_group[SOC_PETRA_MULT_EG_NOF_PORTS_PER_ENTRY];
  uint32
    eg_mc_entry_size = SOC_PETRA_MULT_EG_NOF_PORTS_PER_ENTRY,
    mc_group_size_exact_var = 0,
    i,
    indx = 0;
  uint32
    driver_ret = SOC_SAND_OK;

  /* if egress - concatenate with '1' at msb , and 3 ports per entry */
  tbl_entry_id = mc_group_id | SOC_PETRA_MULT_LL_EGRESS_MC_ID_START_ENTRY;

  while (next_entry != soc_petra_mc_null_id(unit,FALSE))
  {
    if (eg_mc_entry_size == SOC_PETRA_MULT_EG_NOF_PORTS_PER_ENTRY)
    {
      driver_ret = soc_petra_mult_eg_entry_content_get(
                     unit,
                     tbl_entry_id,
                     mc_entry_group,
                     &eg_mc_entry_size,
                     &next_entry
                   );

      if(soc_sand_get_error_code_from_error_word(driver_ret) != SOC_SAND_OK)
      {
        SOC_PETRA_DO_NOTHING_AND_EXIT;
      }

      list_of_next_entries[indx] = next_entry;

      mc_group_size_exact_var += eg_mc_entry_size;

      for (i = 0 ; i < eg_mc_entry_size ; i++)
      {
        mc_group[indx * SOC_PETRA_MULT_EG_NOF_PORTS_PER_ENTRY + i] =
          mc_entry_group[i].eg_entry;
      }
    }

    indx++;
    tbl_entry_id = next_entry;

  }

  *mc_group_size_exact = mc_group_size_exact_var;

exit:
  return driver_ret;
}

uint32
  soc_petra_mult_tests_get_eg_mc_group_new_mode_from_hw(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint16                  mc_group_id,
    SOC_SAND_OUT SOC_PETRA_MULT_EG_ENTRY       *mc_group,
    SOC_SAND_OUT uint32                 *mc_group_size_exact,
    SOC_SAND_OUT uint16                  *list_of_next_entries
  )
{
  uint16
    next_entry = 1,
    tbl_entry_id;
  SOC_PETRA_MULT_ENTRY
    mc_entry_group[SOC_PETRA_MULT_EG_NOF_PORTS_PER_ENTRY];
  uint32
    eg_mc_entry_size = SOC_PETRA_MULT_EG_NOF_PORTS_PER_ENTRY,
    mc_group_size_exact_var = 0,
    i,
    indx = 0,
    progress_index;
  uint32
    driver_ret = SOC_SAND_OK;

  /* if egress - concatenate with '1' at msb , and 3 ports per entry */
  tbl_entry_id = mc_group_id | SOC_PETRA_MULT_LL_EGRESS_MC_ID_START_ENTRY;

  while (next_entry != soc_petra_mc_null_id(unit,FALSE))
  {
          
    driver_ret = soc_petra_mult_progress_index_get(
            unit,
            mc_group_id,
            tbl_entry_id,
            FALSE,
            &progress_index
          );

    if(soc_sand_get_error_code_from_error_word(driver_ret) != SOC_SAND_OK)
    {
      SOC_PETRA_DO_NOTHING_AND_EXIT;
    }

    driver_ret = soc_petra_mult_eg_entry_content_get(
                   unit,
                   tbl_entry_id,
                   mc_entry_group,
                   &eg_mc_entry_size,
                   &next_entry
                 );

    if(soc_sand_get_error_code_from_error_word(driver_ret) != SOC_SAND_OK)
    {
      SOC_PETRA_DO_NOTHING_AND_EXIT;
    }

    list_of_next_entries[indx] = next_entry;

    for (i = 0 ; i < eg_mc_entry_size ; i++)
    {
      mc_group[mc_group_size_exact_var + i] =
        mc_entry_group[i].eg_entry;
    }

    mc_group_size_exact_var += eg_mc_entry_size;
    

    indx++;
    tbl_entry_id = next_entry;

  }

  *mc_group_size_exact = mc_group_size_exact_var;

exit:
  return driver_ret;
}

void
  soc_petra_mult_tests_unoccupied_list_print(
    SOC_SAND_IN int       unit,
    SOC_SAND_IN uint16        mc_group_id,
    SOC_SAND_IN uint16        *list_of_next_entries
  )
{
  uint32
    i = 0;
  uint16
    next_unoccupied;

  next_unoccupied = soc_petra_sw_db_multicast_next_unoccupied_ptr_get(
                      unit,
                      mc_group_id
                    );

  soc_sand_os_printf("%d --> 0x%X \n\r",mc_group_id, (uint32)next_unoccupied);

  while (!soc_petra_mc_is_next_null(unit,list_of_next_entries[i]))
  {
    next_unoccupied = soc_petra_sw_db_multicast_next_unoccupied_ptr_get(
                        unit,
                        list_of_next_entries[i]
                        );
    soc_sand_os_printf("%d --> 0x%X \n\r",list_of_next_entries[i], (uint32)next_unoccupied);

    i++;
  }
}

void
  soc_petra_mult_tests_backwards_list_print(
    SOC_SAND_IN int       unit,
    SOC_SAND_IN uint16        mc_group_id,
    SOC_SAND_IN uint16        *list_of_next_entries
  )
{
  uint32
    i = 0;
  uint16
    backwards_ptr;

  backwards_ptr = soc_petra_sw_db_multicast_backwards_ptr_get(
                      unit,
                      mc_group_id
                    );

  soc_sand_os_printf("%d --> 0x%X \n\r",mc_group_id, (uint32)backwards_ptr);

  while (!soc_petra_mc_is_next_null(unit,list_of_next_entries[i]))
  {
    backwards_ptr = soc_petra_sw_db_multicast_backwards_ptr_get(
                        unit,
                        list_of_next_entries[i]
                      );
    soc_sand_os_printf("%d --> 0x%X \n\r",list_of_next_entries[i], (uint32)backwards_ptr);

    i++;
  }
}

uint8
  soc_petra_mult_tests_unoccupied_list_check_occupation(
    SOC_SAND_IN int       unit,
    SOC_SAND_IN uint16        mc_group_id,
    SOC_SAND_IN uint16        *list_of_next_entries,
    SOC_SAND_IN uint8       silent
  )
{
  uint32
    i = 0;
  uint16
    next_unoccupied;
  uint8
    pass = TRUE;

  next_unoccupied = soc_petra_sw_db_multicast_next_unoccupied_ptr_get(
                      unit,
                      mc_group_id
                    );

  if (next_unoccupied != SOC_PETRA_SW_DB_MULTICAST_FIRST_ENTRY_INDICATOR)
  {
    if (!silent)
    {
      soc_sand_os_printf("\n\rThe MC-ID is not indicated to be a 'First-Entry'\n\r");
    }
    pass = FALSE;
  }

  while (!soc_petra_mc_is_next_null(unit,list_of_next_entries[i]))
  {
    next_unoccupied = soc_petra_sw_db_multicast_next_unoccupied_ptr_get(
                        unit,
                        list_of_next_entries[i]
                        );
    if (!((next_unoccupied == SOC_PETRA_SW_DB_MULTICAST_ING_OCCUPIED_ENTRY_INDICATOR)||
      (next_unoccupied == SOC_PETRA_SW_DB_MULTICAST_EG_OCCUPIED_ENTRY_INDICATOR)))
    {
      if (!silent)
      {
        soc_sand_os_printf("\n\rThe MC group entry is not indicated to be a occupied\n\r");
      }
      pass = FALSE;
    }
    i++;
  }
  return pass;
}

uint8
  soc_petra_mult_tests_backwards_list_check_occupation(
    SOC_SAND_IN int       unit,
    SOC_SAND_IN uint16        mc_group_id,
    SOC_SAND_IN uint16        *list_of_next_entries,
    SOC_SAND_IN uint8       silent
  )
{
  uint32
    i = 0;
  uint16
    backwards_ptr;
  uint8
    pass = TRUE;

  backwards_ptr = soc_petra_sw_db_multicast_backwards_ptr_get(
                      unit,
                      mc_group_id
                    );

  if (backwards_ptr != SOC_PETRA_SW_DB_MULTICAST_LISTS_NULL_POINTER)
  {
    if (!silent)
    {
      soc_sand_os_printf("\n\rThe MC-ID entry backwrds pointer is not pointing at NULL\n\r");
    }
    pass = FALSE;
  }

  while (!soc_petra_mc_is_next_null(unit,list_of_next_entries[i]))
  {
    backwards_ptr = soc_petra_sw_db_multicast_backwards_ptr_get(
                        unit,
                        list_of_next_entries[i]
                      );

    /* Did not handle middle entries */
    i++;
  }
  return pass;
}

uint8
  soc_petra_mult_tests_unoccupied_list_check_occupation_compared_to_old(
    SOC_SAND_IN int       unit,
    SOC_SAND_IN uint16        mc_group_id,
    SOC_SAND_IN uint16        *list_of_next_entries,
    SOC_SAND_IN uint8       silent
  )
{
  uint32
    i = 0;
  uint16
    next_unoccupied;
  uint8
    pass = TRUE;

  next_unoccupied = soc_petra_sw_db_multicast_next_unoccupied_ptr_get(
                      unit,
                      mc_group_id
                    );

  if (next_unoccupied != SOC_PETRA_SW_DB_MULTICAST_FIRST_ENTRY_INDICATOR)
  {
    if (!silent)
    {
      soc_sand_os_printf("\n\rThe MC-ID is not indicated to be a 'First-Entry'\n\r");
    }
    pass = FALSE;
  }

  while (!soc_petra_mc_is_next_null(unit,list_of_next_entries[i]))
  {
    next_unoccupied = soc_petra_sw_db_multicast_next_unoccupied_ptr_get(
                        unit,
                        list_of_next_entries[i]
                      );

    if ((SOC_SAND_BIT(15) & next_unoccupied) != 0)
    {
      if (!silent)
      {
        soc_sand_os_printf("\n\rThe OLD MC group entry indicated as occupied (MSB==1)\n\r");
      }
      pass = FALSE;
    }
    i++;
  }
  return pass;
}


uint8
  soc_petra_mult_tests_unoccupied_list_check_that_not_occupied(
    SOC_SAND_IN int       unit,
    SOC_SAND_IN uint16        mc_group_id,
    SOC_SAND_IN uint16        *list_of_next_entries,
    SOC_SAND_IN uint8       silent
  )
{
  uint32
    i = 0;
  uint16
    next_unoccupied;
  uint8
    pass = TRUE;

  next_unoccupied = soc_petra_sw_db_multicast_next_unoccupied_ptr_get(
                      unit,
                      mc_group_id
                    );

  if ((SOC_SAND_BIT(15) & next_unoccupied) != 0)
  {
    if (!silent)
    {
      soc_sand_os_printf("\n\rThe OLD MC group entry indicated as occupied (MSB==1)\n\r");
    }
    pass = FALSE;
  }

  while (!soc_petra_mc_is_next_null(unit,list_of_next_entries[i]))
  {
    next_unoccupied = soc_petra_sw_db_multicast_next_unoccupied_ptr_get(
      unit,
      list_of_next_entries[i]
      );

    if ((SOC_SAND_BIT(15) & next_unoccupied) != 0)
    {
      if (!silent)
      {
        soc_sand_os_printf("\n\rThe OLD MC group entry indicated as occupied (MSB==1)\n\r");
      }
      pass = FALSE;
    }
    i++;
  }
  return pass;
}


/*****************************************************
*NAME
*  soc_petra_mult_open_TEST_correct_db
*TYPE:
*  PROC
*DATE:
*  15-Feb-2008
*FUNCTION:
*  Test data base consistency for the Open mechanism.
*  Open a MC-GROUP with X entries (MC-ID = 0).
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
  soc_petra_mult_open_TEST_correct_db(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 silent
  )
{
  SOC_PETRA_MULT_ING_ENTRY
    *ing_mc_group_from_user = NULL,
    *mc_group_from_hw = NULL;
  int32
    pass = TRUE,
    i,
    mc_group_size;
  SOC_TMC_ERROR
    mc_err;
  uint32
    mc_group_from_hw_size_exact;
  SOC_PETRA_MULT_TESTS_GROUP_DIFF
    diff;
  uint16
    mc_group_id,
    *list_of_next_entries = NULL;
  uint32
    driver_ret,
    nof_unoccupied,
    used_entries;
  
  if (!silent)
  {
    soc_sand_os_printf("\n\r ********** soc_petra_mult_open_TEST_correct_db : START **********\n\r");
  }

  /*
   * Defining, Setting and Printing a new group
   * type = Ingress
   * size = X
   * mc_id = 0
   */
  /* { */
  mc_group_size = SOC_PETRA_MULT_TEST_1_ING_GROUP_SIZE;
  mc_group_id = SOC_PETRA_MULT_TEST_1_ING_MC_GROUP_ID;

  ing_mc_group_from_user = sal_alloc(sizeof(SOC_PETRA_MULT_ING_ENTRY) * mc_group_size,
    "soc_petra_mult_open_TEST_correct_db.ing_mc_group_from_user");
  if(ing_mc_group_from_user == NULL) {
    pass = FALSE;
    goto exit;
  }

  for (i = 0 ; i < mc_group_size ; i++)
  {
    soc_petra_PETRA_MULT_ING_ENTRY_clear(&ing_mc_group_from_user[i]);
  }

  soc_petra_mult_tests_create_ing_multicast_group(
    mc_group_size,
    ing_mc_group_from_user
  );

  if (!silent)
  {
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
    soc_sand_os_printf("The group from User:\n\r");

    soc_petra_multicast_ing_group_print(
      ing_mc_group_from_user,
      mc_group_size
      );
    soc_sand_os_printf("-------------------------------------------------\n\r");
  }

  /* } */

  /* Opening a new group */
  driver_ret = soc_petra_mult_ing_group_open(
                 unit,
                 mc_group_id,
                 ing_mc_group_from_user,
                 mc_group_size,
                 &mc_err
               );
  if(soc_sand_get_error_code_from_error_word(driver_ret) != SOC_SAND_OK)
  {
    pass = FALSE;
    goto exit;
  }

  /* allocate mc-group before sending to get-from-hw function */
  mc_group_from_hw = sal_alloc(sizeof(SOC_PETRA_MULT_ING_ENTRY) * mc_group_size,
    "soc_petra_mult_open_TEST_correct_db.mc_group_from_hw");
  if(mc_group_from_hw == NULL) {
    pass = FALSE;
    goto exit;
  }
  for (i = 0 ; i < mc_group_size ; i++)
  {
    soc_petra_PETRA_MULT_ING_ENTRY_clear(&mc_group_from_hw[i]);
  }

  list_of_next_entries = sal_alloc(sizeof(uint16)*mc_group_size,
    "soc_petra_mult_open_TEST_correct_db.list_of_next_entries");
  if(list_of_next_entries == NULL) {
    pass = FALSE;
    goto exit;
  }

  /* get-from-hw */
  driver_ret = soc_petra_mult_tests_get_ing_mc_group_from_hw(
                 unit,
                 mc_group_id,
                 mc_group_from_hw,
                 &mc_group_from_hw_size_exact,
                 list_of_next_entries
               );

  if(soc_sand_get_error_code_from_error_word(driver_ret) != SOC_SAND_OK)
  {
    pass = FALSE;
    goto exit;
  }

  if (!silent)
  {
    /*Print group from HW*/
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
    soc_sand_os_printf("The group from HW:\n\r");

    soc_petra_multicast_ing_group_print(
      mc_group_from_hw,
      mc_group_size
    );
    soc_sand_os_printf("-------------------------------------------------\n\r");
  }

  /* compare size and content */
  driver_ret = soc_petra_mult_ing_groups_compare(
                 ing_mc_group_from_user,
                 mc_group_size,
                 mc_group_from_hw,
                 mc_group_from_hw_size_exact,
                 &diff
               );
  if(soc_sand_get_error_code_from_error_word(driver_ret) != SOC_SAND_OK)
  {
    pass = FALSE;
    goto exit;
  }
  
  if (diff != SOC_PETRA_MULT_TESTS_GROUP_DIFF_EQUAL)
  {
     if(!silent)
     {
       soc_sand_os_printf("The group from HW is different from the group from User\n\r"
                      "Difference #:  %d\n\r",
                      diff);
     }
     pass = FALSE;
     soc_sand_os_printf("soc_petra_mult_open_TEST_correct_db : Test FAILED");
     goto exit;
  }

  /* print the number of unocuupied entries */
  nof_unoccupied =
    soc_petra_sw_db_multicast_nof_unoccupied_get(unit);

  if (!silent)
  {
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
    soc_sand_os_printf("Software DataBase: \n\r");
    /*Print SW DATABASE */
    /* Number of unoccupied - should be equal to (total of entries - used)*/
    soc_sand_os_printf("\n\r"
      "# of unoccupied: %u \n\r",nof_unoccupied);

    /* Unoccupied entries list */
    soc_sand_os_printf("\n\r"
      "Unoccupied entries list: \n\r");
    soc_petra_mult_tests_unoccupied_list_print(
      unit,
      mc_group_id,
      list_of_next_entries
      );

    /* Backwards Pointers list */
    soc_sand_os_printf("\n\r"
      "Backwards Pointers list: \n\r");
    soc_petra_mult_tests_backwards_list_print(
      unit,
      mc_group_id,
      list_of_next_entries
    );
    soc_sand_os_printf("-------------------------------------------------\n\r");
  }

  /* check SW-DataBase: {*/
  used_entries = SOC_SAND_DIV_ROUND_UP(mc_group_size,SOC_PETRA_MULT_ING_NOF_DESTINATIONS_PER_ENTRY);

  if (nof_unoccupied != (SOC_PETRA_MULT_TABLE_SIZE - used_entries))
  {
    if(!silent)
    {
      soc_sand_os_printf("The Number of unoccupied entries is different than expected\n\r");
    }
    pass = FALSE;
    soc_sand_os_printf("soc_petra_mult_open_TEST_correct_db : Test FAILED");
    goto exit;
  }

  pass = soc_petra_mult_tests_unoccupied_list_check_occupation(
          unit,
          mc_group_id,
          list_of_next_entries,
          silent
        );

  if (!pass)
  {
    soc_sand_os_printf("soc_petra_mult_open_TEST_correct_db : Test FAILED");
    goto exit;
  }

  pass = soc_petra_mult_tests_backwards_list_check_occupation(
           unit,
           mc_group_id,
           list_of_next_entries,
           silent
         );

  if (!pass)
  {
    soc_sand_os_printf("soc_petra_mult_open_TEST_correct_db : Test FAILED");
    goto exit;
  }
  /* check SW-DataBase: } */

  if (pass)
  {
    soc_sand_os_printf("\n\r* soc_petra_mult_open_TEST_correct_db : Test PASSED\n\r");
  }

exit:
  if (!silent)
  {
    soc_sand_os_printf("\n\r ********** soc_petra_mult_open_TEST_correct_db : END **********\n\r");
  }
  SOC_PETRA_FREE(ing_mc_group_from_user);
  SOC_PETRA_FREE(mc_group_from_hw);
  SOC_PETRA_FREE(list_of_next_entries);
  return pass;
}

/*****************************************************
*NAME
*  soc_petra_mult_open_2_intersecting_groups_TEST_correct_db
*TYPE:
*  PROC
*DATE:
*  15-Feb-2008
*FUNCTION:
*  Open a MC-GROUP with 2 entries (MC-ID = 0).
*  Then, write another MC-GROUP with 2 entries (MC-ID = 1).
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
  soc_petra_mult_open_2_intersecting_groups_TEST_correct_db(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 silent
  )
{
  SOC_PETRA_MULT_ING_ENTRY
    *ing_mc_group_from_user_1 = NULL,
    *ing_mc_group_from_user_2 = NULL,
    *mc_group_from_hw_1 = NULL,
    *mc_group_from_hw_2 = NULL;
  int32
    pass = TRUE,
    i,
    mc_group_size_1,
    mc_group_size_2;
  SOC_TMC_ERROR
    mc_err;
  uint32
    mc_group_from_hw_size_exact_1,
    mc_group_from_hw_size_exact_2;
  SOC_PETRA_MULT_TESTS_GROUP_DIFF
    diff;
  uint16
    mc_group_id_1,
    mc_group_id_2,
    *list_of_next_entries_1 = NULL,
    *list_of_next_entries_2 = NULL;
  uint32
    driver_ret,
    used_entries,
    nof_unoccupied;

  if (!silent)
  {
    soc_sand_os_printf("\n\r ********** soc_petra_mult_open_2_intersecting_groups_TEST_correct_db : START **********\n\r");
  }

/* $Id: petra_multicast_tests.c,v 1.14 Broadcom SDK $
 * First Group
 */

  /*
   * Defining, Setting and Printing a new group
   * type = Ingress
   * size = 2
   * mc_id = 0
   */
  /* { */
  mc_group_size_1 = SOC_PETRA_MULT_TEST_2_ING_GROUP_SIZE_1;
  mc_group_id_1 = SOC_PETRA_MULT_TEST_2_ING_MC_GROUP_ID_1;

  ing_mc_group_from_user_1 = sal_alloc(sizeof(SOC_PETRA_MULT_ING_ENTRY) * mc_group_size_1,
    "soc_petra_mult_open_2_intersecting_groups_TEST_correct_db.ing_mc_group_from_user_1");
  if(ing_mc_group_from_user_1 == NULL) {
    pass = FALSE;
    goto exit;
  }

  for (i = 0 ; i < mc_group_size_1 ; i++)
  {
    soc_petra_PETRA_MULT_ING_ENTRY_clear(&ing_mc_group_from_user_1[i]);
  }

  soc_petra_mult_tests_create_ing_multicast_group(
    mc_group_size_1,
    ing_mc_group_from_user_1
  );

  if (!silent)
  {
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
    soc_sand_os_printf("The group from User:\n\r");

    soc_petra_multicast_ing_group_print(
      ing_mc_group_from_user_1,
      mc_group_size_1
    );
    soc_sand_os_printf("-------------------------------------------------\n\r");
  }

  /* } */

  /* Opening a new group */
  driver_ret = soc_petra_mult_ing_group_open(
                 unit,
                 mc_group_id_1,
                 ing_mc_group_from_user_1,
                 mc_group_size_1,
                 &mc_err
               );
  if(soc_sand_get_error_code_from_error_word(driver_ret) != SOC_SAND_OK)
  {
    goto exit;
  }

  /* allocate mc-group before sending to get-from-hw function */
  mc_group_from_hw_1 = sal_alloc(sizeof(SOC_PETRA_MULT_ING_ENTRY) * mc_group_size_1,
    "soc_petra_mult_open_2_intersecting_groups_TEST_correct_db.mc_group_from_hw_1");
  if(mc_group_from_hw_1 == NULL) {
    pass = FALSE;
    goto exit;
  }
  for (i = 0 ; i < mc_group_size_1 ; i++)
  {
    soc_petra_PETRA_MULT_ING_ENTRY_clear(&mc_group_from_hw_1[i]);
  }
  list_of_next_entries_1 = sal_alloc(sizeof(uint16) * mc_group_size_1,
    "soc_petra_mult_open_2_intersecting_groups_TEST_correct_db.list_of_next_entries_1");
  if(list_of_next_entries_1 == NULL) {
    pass = FALSE;
    goto exit;
  }

  /* get-from-hw */

  driver_ret = soc_petra_mult_tests_get_ing_mc_group_from_hw(
                 unit,
                 mc_group_id_1,
                 mc_group_from_hw_1,
                 &mc_group_from_hw_size_exact_1,
                 list_of_next_entries_1
               );

  if(soc_sand_get_error_code_from_error_word(driver_ret) != SOC_SAND_OK)
  {
    goto exit;
  }

  if (!silent)
  {
    /*Print group from HW*/
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
    soc_sand_os_printf("The group from HW:\n\r");

    soc_petra_multicast_ing_group_print(
      mc_group_from_hw_1,
      mc_group_size_1
    );
    soc_sand_os_printf("-------------------------------------------------\n\r");
  }

  /* compare size and content */
  driver_ret = soc_petra_mult_ing_groups_compare(
                 ing_mc_group_from_user_1,
                 mc_group_size_1,
                 mc_group_from_hw_1,
                 mc_group_from_hw_size_exact_1,
                 &diff
               );
  if (diff != SOC_PETRA_MULT_TESTS_GROUP_DIFF_EQUAL)
  {
    if(!silent)
    {
      soc_sand_os_printf("The groups from HW is different than the group from SW\n\r");
    }
    pass = FALSE;
  }

  /* print the number of unoccupied entries */
  nof_unoccupied =
    soc_petra_sw_db_multicast_nof_unoccupied_get(unit);

  if (!silent)
  {
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
    soc_sand_os_printf("Software DataBase: \n\r");
    /*Print SW DATABASE */
    /* Number of unoccupied - should be equal to (total of entries - used)*/
    soc_sand_os_printf("\n\r"
      "# of unoccupied: %u \n\r",nof_unoccupied);

    /* Unoccupied entries list */
    soc_sand_os_printf("\n\r"
      "Unoccupied entries list: \n\r");
    soc_petra_mult_tests_unoccupied_list_print(
      unit,
      mc_group_id_1,
      list_of_next_entries_1
      );

    /* Backwards Pointers list */
    soc_sand_os_printf("\n\r"
      "Backwards Pointers list: \n\r");
    soc_petra_mult_tests_backwards_list_print(
      unit,
      mc_group_id_1,
      list_of_next_entries_1
      );
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
  }

  /* check SW-DataBase: {*/
  used_entries = SOC_SAND_DIV_ROUND_UP(mc_group_size_1,SOC_PETRA_MULT_ING_NOF_DESTINATIONS_PER_ENTRY);

  if (nof_unoccupied != (SOC_PETRA_MULT_TABLE_SIZE - used_entries))
  {
    if(!silent)
    {
      soc_sand_os_printf("The Number of unoccupied entries is different than expected\n\r");
    }
    pass = FALSE;
    soc_sand_os_printf("soc_petra_mult_open_2_intersecting_groups_TEST_correct_db : Test FAILED");
    goto exit;
  }

  pass = soc_petra_mult_tests_unoccupied_list_check_occupation(
           unit,
           mc_group_id_1,
           list_of_next_entries_1,
           silent
         );

  if (!pass)
  {
    soc_sand_os_printf("soc_petra_mult_open_2_intersecting_groups_TEST_correct_db : Test FAILED");
    goto exit;
  }

  pass = soc_petra_mult_tests_backwards_list_check_occupation(
           unit,
           mc_group_id_1,
           list_of_next_entries_1,
           silent
         );

  if (!pass)
  {
    soc_sand_os_printf("soc_petra_mult_open_2_intersecting_groups_TEST_correct_db : Test FAILED");
    goto exit;
  }
  /* check SW-DataBase: } */


/*
 * Second Group
 */

  /*
   * Defining, Setting and Printing a new group
   * type = Ingress
   * size = 2
   * mc_id = 1
   */
  /* { */
  mc_group_size_2 = SOC_PETRA_MULT_TEST_2_ING_GROUP_SIZE_2;
  mc_group_id_2 = SOC_PETRA_MULT_TEST_2_ING_MC_GROUP_ID_2;

  ing_mc_group_from_user_2 = sal_alloc(sizeof(SOC_PETRA_MULT_ING_ENTRY) * mc_group_size_2,
    "soc_petra_mult_open_2_intersecting_groups_TEST_correct_db.ing_mc_group_from_user_2");
  if(ing_mc_group_from_user_2 == NULL) {
    pass = FALSE;
    goto exit;
  }

  for (i = 0 ; i < mc_group_size_2 ; i++)
  {
    soc_petra_PETRA_MULT_ING_ENTRY_clear(&ing_mc_group_from_user_2[i]);
  }

  soc_petra_mult_tests_create_ing_multicast_group(
    mc_group_size_2,
    ing_mc_group_from_user_2
  );

  if (!silent)
  {
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
    soc_sand_os_printf("\n\r"
      "The group from User:\n\r");

    soc_petra_multicast_ing_group_print(
      ing_mc_group_from_user_2,
      mc_group_size_2
    );
    soc_sand_os_printf("-------------------------------------------------\n\r");
  }

  /* } */

  /* Opening a new group */
  driver_ret = soc_petra_mult_ing_group_open(
                 unit,
                 mc_group_id_2,
                 ing_mc_group_from_user_2,
                 mc_group_size_2,
                 &mc_err
               );
  if(soc_sand_get_error_code_from_error_word(driver_ret) != SOC_SAND_OK)
  {
    goto exit;
  }


  /* allocate mc-group before sending to get-from-hw function */
  mc_group_from_hw_2 = sal_alloc(sizeof(SOC_PETRA_MULT_ING_ENTRY) * mc_group_size_2,
    "soc_petra_mult_open_2_intersecting_groups_TEST_correct_db.mc_group_from_hw_2");
  if(mc_group_from_hw_2 == NULL) {
    pass = FALSE;
    goto exit;
  }

  for (i = 0 ; i < mc_group_size_2 ; i++)
  {
    soc_petra_PETRA_MULT_ING_ENTRY_clear(&mc_group_from_hw_2[i]);
  }
  list_of_next_entries_2 = sal_alloc(sizeof(uint16) * mc_group_size_2,
    "soc_petra_mult_open_2_intersecting_groups_TEST_correct_db.list_of_next_entries_2");
  if(list_of_next_entries_2 == NULL) {
    pass = FALSE;
    goto exit;
  }

  /* get-from-hw */

  driver_ret = soc_petra_mult_tests_get_ing_mc_group_from_hw(
                 unit,
                 mc_group_id_2,
                 mc_group_from_hw_2,
                 &mc_group_from_hw_size_exact_2,
                 list_of_next_entries_2
               );

  if(soc_sand_get_error_code_from_error_word(driver_ret) != SOC_SAND_OK)
  {
    goto exit;
  }

  if (!silent)
  {
    /*Print group from HW*/
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
    soc_sand_os_printf("The group from HW:\n\r");

    soc_petra_multicast_ing_group_print(
      mc_group_from_hw_2,
      mc_group_size_2
    );
    soc_sand_os_printf("-------------------------------------------------\n\r");
  }


  /* compare size and content */
  driver_ret = soc_petra_mult_ing_groups_compare(
                 ing_mc_group_from_user_2,
                 mc_group_size_2,
                 mc_group_from_hw_2,
                 mc_group_from_hw_size_exact_2,
                 &diff
               );
  if (diff != SOC_PETRA_MULT_TESTS_GROUP_DIFF_EQUAL)
  {
     if(!silent)
     {
       soc_sand_os_printf("The groups from HW is different than the group from SW\n\r");
     }
     pass = FALSE;
  }

  /* print the number of unocuupied entries */
  nof_unoccupied =
    soc_petra_sw_db_multicast_nof_unoccupied_get(unit);

  if (!silent)
  {
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
    soc_sand_os_printf("Software DataBase: \n\r");
    /*Print SW DATABASE */
    /* Number of unoccupied - should be equal to (total of entries - used)*/
    soc_sand_os_printf("\n\r"
      "# of unoccupied: %u \n\r",nof_unoccupied);

    /* Unoccupied entries list */
    soc_sand_os_printf("\n\r"
      "Unoccupied entries list: \n\r");
    soc_petra_mult_tests_unoccupied_list_print(
      unit,
      mc_group_id_2,
      list_of_next_entries_2
    );

    /* Backwards Pointers list */
    soc_sand_os_printf("\n\r"
      "Backwards Pointers list: \n\r");
    soc_petra_mult_tests_backwards_list_print(
      unit,
      mc_group_id_2,
      list_of_next_entries_2
    );
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
  }

  /* check SW-DataBase: {*/
  used_entries = SOC_SAND_DIV_ROUND_UP(mc_group_size_1,SOC_PETRA_MULT_ING_NOF_DESTINATIONS_PER_ENTRY) +
    SOC_SAND_DIV_ROUND_UP(mc_group_size_2,SOC_PETRA_MULT_ING_NOF_DESTINATIONS_PER_ENTRY);

  if (nof_unoccupied != (SOC_PETRA_MULT_TABLE_SIZE - used_entries))
  {
    if(!silent)
    {
      soc_sand_os_printf("The Number of unoccupied entries is different than expected\n\r");
    }
    pass = FALSE;
    soc_sand_os_printf("soc_petra_mult_open_2_intersecting_groups_TEST_correct_db : Test FAILED");
    goto exit;
  }

  pass = soc_petra_mult_tests_unoccupied_list_check_occupation(
           unit,
           mc_group_id_2,
           list_of_next_entries_2,
           silent
         );

  if (!pass)
  {
    soc_sand_os_printf("soc_petra_mult_open_2_intersecting_groups_TEST_correct_db : Test FAILED");
    goto exit;
  }

  pass = soc_petra_mult_tests_backwards_list_check_occupation(
           unit,
           mc_group_id_2,
           list_of_next_entries_2,
           silent
         );

  if (!pass)
  {
    soc_sand_os_printf("soc_petra_mult_open_2_intersecting_groups_TEST_correct_db : Test FAILED");
    goto exit;
  }
  /* check SW-DataBase: } */

  if (pass)
  {
    soc_sand_os_printf("\n\r* soc_petra_mult_open_2_intersecting_groups_TEST_correct_db : Test PASSED\n\r");
  }

exit:
  if (!silent)
  {
    soc_sand_os_printf("\n\r ********** soc_petra_mult_open_2_intersecting_groups_TEST_correct_db : END **********\n\r");
  }

  SOC_PETRA_FREE(ing_mc_group_from_user_1);
  SOC_PETRA_FREE(ing_mc_group_from_user_2);
  SOC_PETRA_FREE(mc_group_from_hw_1);
  SOC_PETRA_FREE(mc_group_from_hw_2);
  SOC_PETRA_FREE(list_of_next_entries_1);
  SOC_PETRA_FREE(list_of_next_entries_2);
  return pass;
}



/*****************************************************
*NAME
*  soc_petra_mult_update_group_TEST_correct_db
*TYPE:
*  PROC
*DATE:
*  15-Feb-2008
*FUNCTION:
*  Open a MC-GROUP with 2 entries (MC-ID = 0).
*  Update (MC-ID = 0) with MC-GROUP with 1 entry.
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
  soc_petra_mult_update_group_TEST_correct_db(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 silent
  )
{
  SOC_PETRA_MULT_ING_ENTRY
    *ing_mc_group_from_user_1 = NULL,
    *ing_mc_group_from_user_2 = NULL,
    *mc_group_from_hw_1 = NULL,
    *mc_group_from_hw_2 = NULL;
  int32
    pass = TRUE,
    i,
    mc_group_size_1,
    mc_group_size_2;
  SOC_TMC_ERROR
    mc_err;
  uint32
    mc_group_from_hw_size_exact_1,
    mc_group_from_hw_size_exact_2;
  SOC_PETRA_MULT_TESTS_GROUP_DIFF
    diff;
  uint16
    mc_group_id_1,
    mc_group_id_2,
    *list_of_next_entries_1 = NULL,
    *list_of_next_entries_2 = NULL;
  uint32
    driver_ret,
    used_entries,
    nof_unoccupied;

  if (!silent)
  {
    soc_sand_os_printf("\n\r ********** soc_petra_mult_update_group_TEST_correct_db : START **********\n\r");
  }

/*
 * First Group
 */

  /*
   * Defining, Setting and Printing a new group
   * type = Ingress
   * size = 5
   * mc_id = 0
   */
  /* { */
  mc_group_size_1 = SOC_PETRA_MULT_TEST_3_ING_GROUP_SIZE_1;
  mc_group_id_1 = SOC_PETRA_MULT_TEST_3_ING_MC_GROUP_ID_1;

  ing_mc_group_from_user_1 = sal_alloc(sizeof(SOC_PETRA_MULT_ING_ENTRY) * mc_group_size_1,
    "soc_petra_mult_update_group_TEST_correct_db.ing_mc_group_from_user_1");
  if(ing_mc_group_from_user_1 == NULL) {
    pass = FALSE;
    goto exit;
  }

  for (i = 0 ; i < mc_group_size_1 ; i++)
  {
    soc_petra_PETRA_MULT_ING_ENTRY_clear(&ing_mc_group_from_user_1[i]);
  }

  soc_petra_mult_tests_create_ing_multicast_group(
    mc_group_size_1,
    ing_mc_group_from_user_1
  );

  if (!silent)
  {
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
    soc_sand_os_printf("The group from User:\n\r");

    soc_petra_multicast_ing_group_print(
      ing_mc_group_from_user_1,
      mc_group_size_1
    );
    soc_sand_os_printf("-------------------------------------------------\n\r");
  }

  /* } */

  /* Opening a new group */
  driver_ret = soc_petra_mult_ing_group_open(
                 unit,
                 mc_group_id_1,
                 ing_mc_group_from_user_1,
                 mc_group_size_1,
                 &mc_err
               );
  if(soc_sand_get_error_code_from_error_word(driver_ret) != SOC_SAND_OK)
  {
    goto exit;
  }

  /* allocate mc-group before sending to get-from-hw function */
  mc_group_from_hw_1 = sal_alloc(sizeof(SOC_PETRA_MULT_ING_ENTRY) * mc_group_size_1,
    "soc_petra_mult_update_group_TEST_correct_db.mc_group_from_hw_1");
  if(mc_group_from_hw_1 == NULL) {
    pass = FALSE;
    goto exit;
  }

  for (i = 0 ; i < mc_group_size_1 ; i++)
  {
    soc_petra_PETRA_MULT_ING_ENTRY_clear(&mc_group_from_hw_1[i]);
  }
  list_of_next_entries_1 = sal_alloc(sizeof(uint16) * mc_group_size_1,
    "soc_petra_mult_update_group_TEST_correct_db.list_of_next_entries_1");
  if(list_of_next_entries_1 == NULL) {
    pass = FALSE;
    goto exit;
  }

  /* get-from-hw */

  driver_ret = soc_petra_mult_tests_get_ing_mc_group_from_hw(
                 unit,
                 mc_group_id_1,
                 mc_group_from_hw_1,
                 &mc_group_from_hw_size_exact_1,
                 list_of_next_entries_1
               );

  if(soc_sand_get_error_code_from_error_word(driver_ret) != SOC_SAND_OK)
  {
    goto exit;
  }

  /*Print group from HW*/
  if (!silent)
  {
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
    soc_sand_os_printf("The group from HW:\n\r");

    soc_petra_multicast_ing_group_print(
      mc_group_from_hw_1,
      mc_group_size_1
    );
    soc_sand_os_printf("-------------------------------------------------\n\r");
  }

  /* compare size and content */
  driver_ret = soc_petra_mult_ing_groups_compare(
                 ing_mc_group_from_user_1,
                 mc_group_size_1,
                 mc_group_from_hw_1,
                 mc_group_from_hw_size_exact_1,
                 &diff
               );
  if (diff != SOC_PETRA_MULT_TESTS_GROUP_DIFF_EQUAL)
  {
    if(!silent)
    {
      soc_sand_os_printf("The groups from HW is different than the group from SW\n\r");
    }
    pass = FALSE;
  }

  /* print the number of unocuupied entries */
  nof_unoccupied =
    soc_petra_sw_db_multicast_nof_unoccupied_get(unit);

  if (!silent)
  {
      /*Print SW DATABASE */
      soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
      soc_sand_os_printf("Software DataBase: \n\r");
    /* Number of unoccupied - should be equal to (total of entries - used)*/
    soc_sand_os_printf("\n\r"
      "# of unoccupied: %u \n\r",nof_unoccupied);

    /* Unoccupied entries list */
    soc_sand_os_printf("\n\r"
      "Unoccupied entries list: \n\r");
    soc_petra_mult_tests_unoccupied_list_print(
      unit,
      mc_group_id_1,
      list_of_next_entries_1
      );

    /* Backwards Pointers list */
    soc_sand_os_printf("\n\r"
      "Backwards Pointers list: \n\r");
    soc_petra_mult_tests_backwards_list_print(
      unit,
      mc_group_id_1,
      list_of_next_entries_1
      );
    soc_sand_os_printf("-------------------------------------------------\n\r");
  }

  /* check SW-DataBase: {*/
  used_entries = SOC_SAND_DIV_ROUND_UP(mc_group_size_1,SOC_PETRA_MULT_ING_NOF_DESTINATIONS_PER_ENTRY);

  if (nof_unoccupied != (SOC_PETRA_MULT_TABLE_SIZE - used_entries))
  {
    if(!silent)
    {
      soc_sand_os_printf("The Number of unoccupied entries is different than expected\n\r");
    }
    pass = FALSE;
    soc_sand_os_printf("soc_petra_mult_update_group_TEST_correct_db : Test FAILED\n\r");
    goto exit;
  }

  pass = soc_petra_mult_tests_unoccupied_list_check_occupation(
    unit,
    mc_group_id_1,
    list_of_next_entries_1,
    silent
    );

  if (!pass)
  {
    soc_sand_os_printf("soc_petra_mult_update_group_TEST_correct_db : Test FAILED\n\r");
    goto exit;
  }

  pass = soc_petra_mult_tests_backwards_list_check_occupation(
    unit,
    mc_group_id_1,
    list_of_next_entries_1,
    silent
    );

  if (!pass)
  {
    soc_sand_os_printf("soc_petra_mult_update_group_TEST_correct_db : Test FAILED\n\r");
    goto exit;
  }
  /* check SW-DataBase: } */



/*
 * Second Group
 */

  /*
   * Defining, Setting and Printing a new group
   * type = Ingress
   * size = 3
   * mc_id = 0
   */
  /* { */
  mc_group_size_2 = SOC_PETRA_MULT_TEST_3_ING_GROUP_SIZE_2;
  mc_group_id_2 = SOC_PETRA_MULT_TEST_3_ING_MC_GROUP_ID_2;

  ing_mc_group_from_user_2 = sal_alloc(sizeof(SOC_PETRA_MULT_ING_ENTRY) * mc_group_size_2,
    "soc_petra_mult_update_group_TEST_correct_db.ing_mc_group_from_user_2");
  if(ing_mc_group_from_user_2 == NULL) {
    pass = FALSE;
    goto exit;
  }

  for (i = 0 ; i < mc_group_size_2 ; i++)
  {
    soc_petra_PETRA_MULT_ING_ENTRY_clear(&ing_mc_group_from_user_2[i]);
  }

  soc_petra_mult_tests_create_ing_multicast_group(
    mc_group_size_2,
    ing_mc_group_from_user_2
  );

  if (!silent)
  {
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
    soc_sand_os_printf("The group from User:\n\r");

    soc_petra_multicast_ing_group_print(
      ing_mc_group_from_user_2,
      mc_group_size_2
    );
    soc_sand_os_printf("-------------------------------------------------\n\r");
  }

  /* } */

  /* Opening a new group */
  driver_ret = soc_petra_mult_ing_group_open(
                 unit,
                 mc_group_id_2,
                 ing_mc_group_from_user_2,
                 mc_group_size_2,
                 &mc_err
               );
  if(soc_sand_get_error_code_from_error_word(driver_ret) != SOC_SAND_OK)
  {
    goto exit;
  }


  /* allocate mc-group before sending to get-from-hw function */
  mc_group_from_hw_2 = sal_alloc(sizeof(SOC_PETRA_MULT_ING_ENTRY) * mc_group_size_2,
    "soc_petra_mult_update_group_TEST_correct_db.mc_group_from_hw_2");
  if(mc_group_from_hw_2 == NULL) {
    pass = FALSE;
    goto exit;
  }

  for (i = 0 ; i < mc_group_size_2 ; i++)
  {
    soc_petra_PETRA_MULT_ING_ENTRY_clear(&mc_group_from_hw_2[i]);
  }
  list_of_next_entries_2 = sal_alloc(sizeof(uint16) * mc_group_size_2,
    "soc_petra_mult_update_group_TEST_correct_db.list_of_next_entries_2");
  if(list_of_next_entries_2 == NULL) {
    pass = FALSE;
    goto exit;
  }

  /* get-from-hw */

  driver_ret = soc_petra_mult_tests_get_ing_mc_group_from_hw(
                 unit,
                 mc_group_id_2,
                 mc_group_from_hw_2,
                 &mc_group_from_hw_size_exact_2,
                 list_of_next_entries_2
               );

  if(soc_sand_get_error_code_from_error_word(driver_ret) != SOC_SAND_OK)
  {
    goto exit;
  }

  if (!silent)
  {
      /*Print group from HW*/
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
    soc_sand_os_printf("The group from HW:\n\r");

    soc_petra_multicast_ing_group_print(
      mc_group_from_hw_2,
      mc_group_size_2
    );
    soc_sand_os_printf("-------------------------------------------------\n\r");
  }

  /* compare size and content */
  driver_ret = soc_petra_mult_ing_groups_compare(
                 ing_mc_group_from_user_2,
                 mc_group_size_2,
                 mc_group_from_hw_2,
                 mc_group_from_hw_size_exact_2,
                 &diff
               );
  if (diff != SOC_PETRA_MULT_TESTS_GROUP_DIFF_EQUAL)
  {
     if(!silent)
     {
       soc_sand_os_printf("The groups from HW is different than the group from SW\n\r");
     }
     pass = FALSE;
  }

  /* print the number of unocuupied entries */
  nof_unoccupied =
    soc_petra_sw_db_multicast_nof_unoccupied_get(unit);

  if (!silent)
  {
    /*Print SW DATABASE */
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
    soc_sand_os_printf("Software DataBase: \n\r");

    /* Number of unoccupied - should be equal to (total of entries - used)*/
    soc_sand_os_printf("\n\r"
      "# of unoccupied: %u \n\r",nof_unoccupied);

    /* Unoccupied entries list */
    soc_sand_os_printf("\n\r"
      "Unoccupied entries list \n\r");
    soc_petra_mult_tests_unoccupied_list_print(
      unit,
      mc_group_id_2,
      list_of_next_entries_2
    );

    /* Backwards Pointers list */
    soc_sand_os_printf("\n\r"
      "Backwards Pointers list \n\r");
    soc_petra_mult_tests_backwards_list_print(
      unit,
      mc_group_id_2,
      list_of_next_entries_2
    );
    soc_sand_os_printf("-------------------------------------------------\n\r");
  }

  /* check SW-DataBase: {*/
  used_entries = SOC_SAND_DIV_ROUND_UP(mc_group_size_2,SOC_PETRA_MULT_ING_NOF_DESTINATIONS_PER_ENTRY);

  if (nof_unoccupied != (SOC_PETRA_MULT_TABLE_SIZE - used_entries))
  {
    if(!silent)
    {
      soc_sand_os_printf("The Number of unoccupied entries is different than expected\n\r");
    }
    pass = FALSE;
    soc_sand_os_printf("soc_petra_mult_update_group_TEST_correct_db : Test FAILED\n\r");
    goto exit;
  }

  pass = soc_petra_mult_tests_unoccupied_list_check_occupation(
           unit,
           mc_group_id_2,
           list_of_next_entries_2,
           silent
         );

  if (!pass)
  {
    soc_sand_os_printf("soc_petra_mult_update_group_TEST_correct_db : Test FAILED\n\r");
    goto exit;
  }

  pass = soc_petra_mult_tests_backwards_list_check_occupation(
           unit,
           mc_group_id_2,
           list_of_next_entries_2,
           silent
         );

  if (!pass)
  {
    soc_sand_os_printf("soc_petra_mult_update_group_TEST_correct_db : Test FAILED\n\r");
    goto exit;
  }
  /* check SW-DataBase: } */

  if(!silent)
  {
    /* to check-up on the erased entry*/
    soc_sand_os_printf("\n\r"
      "Unoccupied entries list OLD\n\r");
    soc_petra_mult_tests_unoccupied_list_print(
      unit,
      mc_group_id_1,
      list_of_next_entries_1
    );
    soc_sand_os_printf("\n\r"
      "Backwards Pointers list OLD\n\r");
    soc_petra_mult_tests_backwards_list_print(
      unit,
      mc_group_id_1,
      list_of_next_entries_1
      );
  }

  pass = soc_petra_mult_tests_unoccupied_list_check_occupation_compared_to_old(
           unit,
           mc_group_id_1,
           list_of_next_entries_1,
           silent
         );
  if (!pass)
  {
    soc_sand_os_printf("soc_petra_mult_update_group_TEST_correct_db : Test FAILED\n\r");
    goto exit;
  }

  if (pass)
  {
    soc_sand_os_printf("\n\r* soc_petra_mult_update_group_TEST_correct_db : Test PASSED\n\r");
  }

exit:
  if (!silent)
  {
    soc_sand_os_printf("\n\r ********** soc_petra_mult_update_group_TEST_correct_db : END **********\n\r");
  }

  SOC_PETRA_FREE(ing_mc_group_from_user_1);
  SOC_PETRA_FREE(ing_mc_group_from_user_2);
  SOC_PETRA_FREE(mc_group_from_hw_1);
  SOC_PETRA_FREE(mc_group_from_hw_2);
  SOC_PETRA_FREE(list_of_next_entries_1);
  SOC_PETRA_FREE(list_of_next_entries_2);

  return pass;
}



/*****************************************************
*NAME
*  soc_petra_mult_open_add_to_group_TEST_correct_db
*TYPE:
*  PROC
*DATE:
*  15-Feb-2008
*FUNCTION:
*  Open a MC-GROUP with X entries (MC-ID = 0).
*  Test the ADD and REMOVE functions..
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
  soc_petra_mult_open_add_to_group_TEST_correct_db(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 silent
  )
{
  SOC_PETRA_MULT_ING_ENTRY
    *ing_mc_group_from_user_1 = NULL,
    ing_mc_entry,
    *mc_group_from_hw_1 = NULL,
    *mc_group_from_hw_2 = NULL;
  int32
    pass = TRUE,
    mc_group_size_1,
    mc_group_size_2,
    i,
    nof_additions;
  SOC_TMC_ERROR
    mc_err;
  uint8
    insufficient_memory = FALSE;
  uint32
    mc_group_from_hw_size_exact_1,
    mc_group_from_hw_size_exact_2;
  SOC_PETRA_MULT_TESTS_GROUP_DIFF
    diff;
  uint16
    mc_group_id_1,
    *list_of_next_entries_1 = NULL,
    *list_of_next_entries_2 = NULL;
  uint32
    driver_ret,
    used_entries,
    nof_unoccupied;

  if (!silent)
  {
    soc_sand_os_printf("\n\r ********** soc_petra_mult_open_add_to_group_TEST_correct_db : START **********\n\r");
  }
/*
 * First Group
 */

  /*
   * Defining, Setting and Printing a new group
   * type = Ingress
   * size = 2
   * mc_id = 0
   */
  /* { */
  mc_group_size_1 = SOC_PETRA_MULT_TEST_4_ING_GROUP_SIZE_1;
  mc_group_id_1 = SOC_PETRA_MULT_TEST_4_ING_MC_GROUP_ID_1;

  ing_mc_group_from_user_1 = sal_alloc(sizeof(SOC_PETRA_MULT_ING_ENTRY) * mc_group_size_1,
    "soc_petra_mult_open_add_to_group_TEST_correct_db.ing_mc_group_from_user_1");
  if(ing_mc_group_from_user_1 == NULL) {
    pass = FALSE;
    goto exit;
  }

  for (i = 0 ; i < mc_group_size_1 ; i++)
  {
    soc_petra_PETRA_MULT_ING_ENTRY_clear(&ing_mc_group_from_user_1[i]);
  }

  soc_petra_mult_tests_create_ing_multicast_group(
    mc_group_size_1,
    ing_mc_group_from_user_1
  );

  if (!silent)
  {
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
    soc_sand_os_printf("The group from User:\n\r");

    soc_petra_multicast_ing_group_print(
      ing_mc_group_from_user_1,
      mc_group_size_1
    );
    soc_sand_os_printf("-------------------------------------------------\n\r");
  }

  /* } */

  /* Opening a new group */
  driver_ret = soc_petra_mult_ing_group_open(
                 unit,
                 mc_group_id_1,
                 ing_mc_group_from_user_1,
                 mc_group_size_1,
                 &mc_err
               );
  if(soc_sand_get_error_code_from_error_word(driver_ret) != SOC_SAND_OK)
  {
    goto exit;
  }



  /* allocate mc-group before sending to get-from-hw function */
  mc_group_from_hw_1 = sal_alloc(sizeof(SOC_PETRA_MULT_ING_ENTRY) * mc_group_size_1,
    "soc_petra_mult_open_add_to_group_TEST_correct_db.mc_group_from_hw_1");
  if(mc_group_from_hw_1 == NULL) {
    pass = FALSE;
    goto exit;
  }

  for (i = 0 ; i < mc_group_size_1 ; i++)
  {
    soc_petra_PETRA_MULT_ING_ENTRY_clear(&mc_group_from_hw_1[i]);
  }
  list_of_next_entries_1 = sal_alloc(sizeof(uint16) * mc_group_size_1,
    "soc_petra_mult_open_add_to_group_TEST_correct_db.list_of_next_entries_1");
  if(list_of_next_entries_1 == NULL) {
    pass = FALSE;
    goto exit;
  }

  /* get-from-hw */

  driver_ret = soc_petra_mult_tests_get_ing_mc_group_from_hw(
                 unit,
                 mc_group_id_1,
                 mc_group_from_hw_1,
                 &mc_group_from_hw_size_exact_1,
                 list_of_next_entries_1
               );

  if(soc_sand_get_error_code_from_error_word(driver_ret) != SOC_SAND_OK)
  {
    goto exit;
  }

  if (!silent)
  {
    /*Print group from HW*/
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
    soc_sand_os_printf("The group from HW:\n\r");

    soc_petra_multicast_ing_group_print(
      mc_group_from_hw_1,
      mc_group_size_1
    );
    soc_sand_os_printf("-------------------------------------------------\n\r");
  }

  /* compare size and content */
  driver_ret = soc_petra_mult_ing_groups_compare(
                 ing_mc_group_from_user_1,
                 mc_group_size_1,
                 mc_group_from_hw_1,
                 mc_group_from_hw_size_exact_1,
                 &diff
               );
  if (diff != SOC_PETRA_MULT_TESTS_GROUP_DIFF_EQUAL)
  {
    if(!silent)
    {
      soc_sand_os_printf("The groups from HW is different than the group from SW\n\r");
    }
    pass = FALSE;
  }

  /* print the number of unocuupied entries */
  nof_unoccupied =
    soc_petra_sw_db_multicast_nof_unoccupied_get(unit);

  if (!silent)
  {
    /*Print SW DATABASE */
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
      soc_sand_os_printf("Software DataBase: \n\r");
    /* Number of unoccupied - should be equal to (total of entries - used)*/
    soc_sand_os_printf("\n\r"
      "# of unoccupied: %u \n\r",nof_unoccupied);

    /* Unoccupied entries list */
    soc_sand_os_printf("\n\r"
      "Unoccupied entries list: \n\r");
    soc_petra_mult_tests_unoccupied_list_print(
      unit,
      mc_group_id_1,
      list_of_next_entries_1
      );

    /* Backwards Pointers list */
    soc_sand_os_printf("\n\r"
      "Backwards Pointers list: \n\r");
    soc_petra_mult_tests_backwards_list_print(
      unit,
      mc_group_id_1,
      list_of_next_entries_1
      );
    soc_sand_os_printf("-------------------------------------------------\n\r");
  }

    /* check SW-DataBase: {*/
  used_entries = SOC_SAND_DIV_ROUND_UP(mc_group_size_1,SOC_PETRA_MULT_ING_NOF_DESTINATIONS_PER_ENTRY);

  if (nof_unoccupied != (SOC_PETRA_MULT_TABLE_SIZE - used_entries))
  {
    if(!silent)
    {
      soc_sand_os_printf("The Number of unoccupied entries is different than expected\n\r");
    }
    pass = FALSE;
    soc_sand_os_printf("soc_petra_mult_open_add_to_group_TEST_correct_db : Test FAILED\n\r");
    goto exit;
  }

  pass = soc_petra_mult_tests_unoccupied_list_check_occupation(
    unit,
    mc_group_id_1,
    list_of_next_entries_1,
    silent
    );

  if (!pass)
  {
    soc_sand_os_printf("soc_petra_mult_open_add_to_group_TEST_correct_db : Test FAILED\n\r");
    goto exit;
  }

  pass = soc_petra_mult_tests_backwards_list_check_occupation(
    unit,
    mc_group_id_1,
    list_of_next_entries_1,
    silent
    );

  if (!pass)
  {
    soc_sand_os_printf("soc_petra_mult_open_add_to_group_TEST_correct_db : Test FAILED\n\r");
    goto exit;
  }
  /* check SW-DataBase: } */


/*
 * ADD Elements to mc-group
 */

  /*
   * adding a new element to the group
   * type = Ingress
   * mc_id = 0
   * element = type--> Q#, Q-id--> 100, Outlif-->0xabcd
   * # of elements to add: nof_additions
   */
  /* { */
  soc_petra_mult_eg_all_groups_close(unit);
  soc_petra_mult_ing_all_groups_close(unit);

  nof_additions = SOC_PETRA_MULT_TEST_4_NOF_ADDITIONS;

  soc_petra_PETRA_MULT_ING_ENTRY_clear(&ing_mc_entry);

  ing_mc_entry.destination.type = SOC_PETRA_DEST_TYPE_QUEUE;
  ing_mc_entry.destination.id = 100;
  ing_mc_entry.cud = 0xabcd;

  if (!silent)
  {
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
    soc_sand_os_printf("The entry from User:\n\r");

    soc_sand_os_printf(
      "{%2d-->%u ; 0x%X} \n\r",
      ing_mc_entry.destination.type,
      ing_mc_entry.destination.id,
      ing_mc_entry.cud
      );
    soc_sand_os_printf("-------------------------------------------------\n\r");
  }

  /* } */

  /* add nof_additions times */
  for (i = 0 ; i < nof_additions; i++)
  {
    ing_mc_entry.destination.id++;
    driver_ret = soc_petra_mult_ing_destination_add_unsafe(
                   unit,
                   mc_group_id_1,
                   &ing_mc_entry,
                   &insufficient_memory
                 );
    if(soc_sand_get_error_code_from_error_word(driver_ret) != SOC_SAND_OK)
    {
      goto exit;
    }
  }

  mc_group_size_2 = nof_additions;

  /* allocate mc-group before sending to get-from-hw function */
  mc_group_from_hw_2 = sal_alloc(sizeof(SOC_PETRA_MULT_ING_ENTRY) * mc_group_size_2,
    "soc_petra_mult_open_add_to_group_TEST_correct_db.mc_group_from_hw_2");
  if(mc_group_from_hw_2 == NULL) {
    pass = FALSE;
    goto exit;
  }

  list_of_next_entries_2 = sal_alloc(sizeof(uint16) * mc_group_size_2,
    "soc_petra_mult_open_add_to_group_TEST_correct_db.list_of_next_entries_2");
  if(list_of_next_entries_2 == NULL) {
    pass = FALSE;
    goto exit;
  }

  for (i = 0 ; i < mc_group_size_2 ; i++)
  {
    soc_petra_PETRA_MULT_ING_ENTRY_clear(&mc_group_from_hw_2[i]);
  }
  /* get-from-hw */

  driver_ret = soc_petra_mult_tests_get_ing_mc_group_from_hw(
                 unit,
                 mc_group_id_1,
                 mc_group_from_hw_2,
                 &mc_group_from_hw_size_exact_2,
                 list_of_next_entries_2
               );

  if(soc_sand_get_error_code_from_error_word(driver_ret) != SOC_SAND_OK)
  {
    goto exit;
  }

  if (!silent)
  {
    /*Print group from HW*/
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
    soc_sand_os_printf("The group from HW:\n\r");

    soc_petra_multicast_ing_group_print(
      mc_group_from_hw_2,
      mc_group_size_2
    );
    soc_sand_os_printf("-------------------------------------------------\n\r");
  }

  /* print the number of unoccupied entries */
  nof_unoccupied =
    soc_petra_sw_db_multicast_nof_unoccupied_get(unit);

  if (!silent)
  {
    /*Print SW DATABASE */
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
    soc_sand_os_printf("Software DataBase: \n\r");

    /* Number of unoccupied - should be equal to (total of entries - used)*/
    soc_sand_os_printf("\n\r"
      "# of unoccupied: %u \n\r",nof_unoccupied);

    /* Unoccupied entries list */
    soc_sand_os_printf("\n\r"
      "Unoccupied entries list \n\r");
    soc_petra_mult_tests_unoccupied_list_print(
      unit,
      mc_group_id_1,
      list_of_next_entries_2
    );

    /* Backwards Pointers list */
    soc_sand_os_printf("\n\r"
      "Backwards Pointers list \n\r");
    soc_petra_mult_tests_backwards_list_print(
      unit,
      mc_group_id_1,
      list_of_next_entries_2
    );
    soc_sand_os_printf("-------------------------------------------------\n\r");
  }

  /* check SW-DataBase: {*/
  used_entries = SOC_SAND_DIV_ROUND_UP(mc_group_size_2,SOC_PETRA_MULT_ING_NOF_DESTINATIONS_PER_ENTRY);

  if (nof_unoccupied != (SOC_PETRA_MULT_TABLE_SIZE - used_entries))
  {
    soc_sand_os_printf("The Number of unoccupied entries is different than expected\n\r");
    pass = FALSE;
    soc_sand_os_printf("soc_petra_mult_open_add_to_group_TEST_correct_db : Test FAILED\n\r");
    goto exit;
  }

  pass = soc_petra_mult_tests_unoccupied_list_check_occupation(
           unit,
           mc_group_id_1,
           list_of_next_entries_2,
           silent
         );

  if (!pass)
  {
    soc_sand_os_printf("soc_petra_mult_open_add_to_group_TEST_correct_db : Test FAILED\n\r");
    goto exit;
  }

  pass = soc_petra_mult_tests_backwards_list_check_occupation(
           unit,
           mc_group_id_1,
           list_of_next_entries_2,
           silent
         );

  if (!pass)
  {
    soc_sand_os_printf("soc_petra_mult_open_add_to_group_TEST_correct_db : Test FAILED\n\r");
    goto exit;
  }
  /* check SW-DataBase: } */

  if (pass)
  {
    soc_sand_os_printf("\n\r* soc_petra_mult_open_add_to_group_TEST_correct_db : Test PASSED\n\r");
  }

exit:
  if (!silent)
  {
    soc_sand_os_printf("\n\r ********** soc_petra_mult_open_add_to_group_TEST_correct_db : END **********\n\r");
  }

  SOC_PETRA_FREE(ing_mc_group_from_user_1);
  SOC_PETRA_FREE(mc_group_from_hw_1);
  SOC_PETRA_FREE(mc_group_from_hw_2);
  SOC_PETRA_FREE(list_of_next_entries_1);
  SOC_PETRA_FREE(list_of_next_entries_2);

  return pass;
}

/*****************************************************
*NAME
*  soc_petra_mult_open_remove_from_group_TEST_correct_db
*TYPE:
*  PROC
*DATE:
*  15-Feb-2008
*FUNCTION:
*  Open a MC-GROUP with X entries (MC-ID = 0).
*  Test the ADD and REMOVE functions..
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
  soc_petra_mult_open_remove_from_group_TEST_correct_db(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 silent
  )
{
  SOC_PETRA_MULT_ING_ENTRY
    *ing_mc_group_from_user_1 = NULL,
    ing_mc_entry,
    *mc_group_from_hw_1 = NULL,
    *mc_group_from_hw_2 = NULL;
  int32
    pass = TRUE,
    i,
    mc_group_size_1,
    mc_group_size_2;
  SOC_TMC_ERROR
    mc_err;
  uint32
    mc_group_from_hw_size_exact_1,
    mc_group_from_hw_size_exact_2;
  SOC_PETRA_MULT_TESTS_GROUP_DIFF
    diff;
  uint16
    mc_group_id_1,
    *list_of_next_entries_1 = NULL,
    *list_of_next_entries_2 = NULL;
  uint32
    driver_ret,
    used_entries,
    nof_unoccupied;

  if (!silent)
  {
    soc_sand_os_printf("\n\r ********** soc_petra_mult_open_remove_from_group_TEST_correct_db : START **********\n\r");
  }

/*
 * First Group
 */

  /*
   * Defining, Setting and Printing a new group
   * type = Ingress
   * size = 10
   * mc_id = 0
   */
  /* { */
  mc_group_size_1 = SOC_PETRA_MULT_TEST_5_ING_GROUP_SIZE;
  mc_group_id_1 = SOC_PETRA_MULT_TEST_5_ING_MC_GROUP_ID;

  ing_mc_group_from_user_1 = sal_alloc(sizeof(SOC_PETRA_MULT_ING_ENTRY) * mc_group_size_1,
    "soc_petra_mult_open_remove_from_group_TEST_correct_db.ing_mc_group_from_user_1");
  if(ing_mc_group_from_user_1 == NULL) {
    pass = FALSE;
    goto exit;
  }

  for (i = 0 ; i < mc_group_size_1 ; i++)
  {
    soc_petra_PETRA_MULT_ING_ENTRY_clear(&ing_mc_group_from_user_1[i]);
  }

  soc_petra_mult_tests_create_ing_multicast_group(
    mc_group_size_1,
    ing_mc_group_from_user_1
  );

  if (!silent)
  {
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
    soc_sand_os_printf("The group from User:\n\r");

    soc_petra_multicast_ing_group_print(
      ing_mc_group_from_user_1,
      mc_group_size_1
    );
    soc_sand_os_printf("-------------------------------------------------\n\r");
  }

  /* } */

  /* Opening a new group */
  driver_ret = soc_petra_mult_ing_group_open(
                 unit,
                 mc_group_id_1,
                 ing_mc_group_from_user_1,
                 mc_group_size_1,
                 &mc_err
               );
  if(soc_sand_get_error_code_from_error_word(driver_ret) != SOC_SAND_OK)
  {
    goto exit;
  }



  /* allocate mc-group before sending to get-from-hw function */
  mc_group_from_hw_1 = sal_alloc(sizeof(SOC_PETRA_MULT_ING_ENTRY) * mc_group_size_1,
    "soc_petra_mult_open_remove_from_group_TEST_correct_db.mc_group_from_hw_1");
  if(mc_group_from_hw_1 == NULL) {
    pass = FALSE;
    goto exit;
  }

  list_of_next_entries_1 = sal_alloc(sizeof(uint16) * mc_group_size_1,
    "soc_petra_mult_open_remove_from_group_TEST_correct_db.list_of_next_entries_1");
  if(list_of_next_entries_1 == NULL) {
    pass = FALSE;
    goto exit;
  }

  for (i = 0 ; i < mc_group_size_1 ; i++)
  {
    soc_petra_PETRA_MULT_ING_ENTRY_clear(&mc_group_from_hw_1[i]);
  }
  /* get-from-hw */

  driver_ret = soc_petra_mult_tests_get_ing_mc_group_from_hw(
                 unit,
                 mc_group_id_1,
                 mc_group_from_hw_1,
                 &mc_group_from_hw_size_exact_1,
                 list_of_next_entries_1
               );

  if(soc_sand_get_error_code_from_error_word(driver_ret) != SOC_SAND_OK)
  {
    goto exit;
  }

  if (!silent)
  {
    /*Print group from HW*/
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
    soc_sand_os_printf("The group from HW:\n\r");

    soc_petra_multicast_ing_group_print(
      mc_group_from_hw_1,
      mc_group_size_1
    );
    soc_sand_os_printf("-------------------------------------------------\n\r");
  }

  /* compare size and content */
  driver_ret = soc_petra_mult_ing_groups_compare(
                 ing_mc_group_from_user_1,
                 mc_group_size_1,
                 mc_group_from_hw_1,
                 mc_group_from_hw_size_exact_1,
                 &diff
               );
  if (diff != SOC_PETRA_MULT_TESTS_GROUP_DIFF_EQUAL)
  {
    if(!silent)
    {
      soc_sand_os_printf("The groups from HW is different than the group from SW\n\r");
    }
    pass = FALSE;
  }

  /* print the number of unocuupied entries */
  nof_unoccupied =
    soc_petra_sw_db_multicast_nof_unoccupied_get(unit);

  if (!silent)
  {
    /*Print SW DATABASE */
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
    soc_sand_os_printf("Software DataBase: \n\r");
    /* Number of unoccupied - should be equal to (total of entries - used)*/
    soc_sand_os_printf("\n\r"
      "# of unoccupied: %u \n\r",nof_unoccupied);

    /* Unoccupied entries list */
    soc_sand_os_printf("\n\r"
      "Unoccupied entries list \n\r");
    soc_petra_mult_tests_unoccupied_list_print(
      unit,
      mc_group_id_1,
      list_of_next_entries_1
      );

    /* Backwards Pointers list */
    soc_sand_os_printf("\n\r"
      "Backwards Pointers list \n\r");
    soc_petra_mult_tests_backwards_list_print(
      unit,
      mc_group_id_1,
      list_of_next_entries_1
      );
    soc_sand_os_printf("-------------------------------------------------\n\r");
  }

/*
 * Remove elements from mc-group
 */

  /*
   * removing an element from the group
   * type = Ingress
   * mc_id = 0
   * element = type--> Q#, Q-id--> 7, Outlif-->0xabcd
   */
  /* { */

  soc_petra_PETRA_MULT_ING_ENTRY_clear(&ing_mc_entry);

  ing_mc_entry.destination.type = SOC_PETRA_DEST_TYPE_QUEUE;
  ing_mc_entry.destination.id = 7;
  ing_mc_entry.cud = 0xabcd;

  if (!silent)
  {
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
    soc_sand_os_printf("The entry from User:\n\r");

    soc_sand_os_printf(
      "{%2d-->%u ; 0x%X} \n\r",
      ing_mc_entry.destination.type,
      ing_mc_entry.destination.id,
      ing_mc_entry.cud
      );
    soc_sand_os_printf("-------------------------------------------------\n\r");
  }

  /* } */

  /* remove element */

    driver_ret = soc_petra_mult_ing_destination_remove(
                   unit,
                   mc_group_id_1,
                   &ing_mc_entry,
                   &mc_err
                 );
    if(soc_sand_get_error_code_from_error_word(driver_ret) != SOC_SAND_OK)
    {
      goto exit;
    }

    mc_group_size_2 = mc_group_size_1;
    mc_group_size_2--;


  /*
   * removing an element from the group
   * type = Ingress
   * mc_id = 0
   * element = type--> Q#, Q-id--> 2, Outlif-->0xabcd
   */
  /* { */


  ing_mc_entry.destination.type = SOC_PETRA_DEST_TYPE_QUEUE;
  ing_mc_entry.destination.id = 2;
  ing_mc_entry.cud = 0xabcd;

  if (!silent)
  {
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
    soc_sand_os_printf("The entry from User:\n\r");

    soc_sand_os_printf(
      "{%2d-->%u ; 0x%X} \n\r",
      ing_mc_entry.destination.type,
      ing_mc_entry.destination.id,
      ing_mc_entry.cud
      );
    soc_sand_os_printf("-------------------------------------------------\n\r");
  }

  /* } */

  /* remove element */

    driver_ret = soc_petra_mult_ing_destination_remove(
                   unit,
                   mc_group_id_1,
                   &ing_mc_entry,
                   &mc_err
                 );
    if(soc_sand_get_error_code_from_error_word(driver_ret) != SOC_SAND_OK)
    {
      goto exit;
    }

    mc_group_size_2--;

      /*
   * removing an element from the group
   * type = Ingress
   * mc_id = 0
   * element = type--> Q#, Q-id--> 6, Outlif-->0xabcd
   */
  /* { */

  ing_mc_entry.destination.type = SOC_PETRA_DEST_TYPE_QUEUE;
  ing_mc_entry.destination.id = 6;
  ing_mc_entry.cud = 0xabcd;

  if (!silent)
  {
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
    soc_sand_os_printf("The entry from User:\n\r");

    soc_sand_os_printf(
      "{%2d-->%u ; 0x%X} \n\r",
      ing_mc_entry.destination.type,
      ing_mc_entry.destination.id,
      ing_mc_entry.cud
      );
    soc_sand_os_printf("-------------------------------------------------\n\r");
  }

  /* } */

  /* remove element */

  driver_ret = soc_petra_mult_ing_destination_remove(
                   unit,
                   mc_group_id_1,
                   &ing_mc_entry,
                   &mc_err
                 );
    if(soc_sand_get_error_code_from_error_word(driver_ret) != SOC_SAND_OK)
    {
      goto exit;
    }

  mc_group_size_2--;


  /* allocate mc-group before sending to get-from-hw function */
  mc_group_from_hw_2 = sal_alloc(sizeof(SOC_PETRA_MULT_ING_ENTRY) * mc_group_size_2,
    "soc_petra_mult_open_remove_from_group_TEST_correct_db.mc_group_from_hw_2");
  if(mc_group_from_hw_2 == NULL) {
    pass = FALSE;
    goto exit;
  }

  list_of_next_entries_2 = sal_alloc(sizeof(uint16) * mc_group_size_2,
    "soc_petra_mult_open_remove_from_group_TEST_correct_db.list_of_next_entries_2");
  if(list_of_next_entries_2 == NULL) {
    pass = FALSE;
    goto exit;
  }

  for (i = 0 ; i < mc_group_size_2 ; i++)
  {
    soc_petra_PETRA_MULT_ING_ENTRY_clear(&mc_group_from_hw_2[i]);
  }
  /* get-from-hw */

  driver_ret = soc_petra_mult_tests_get_ing_mc_group_from_hw(
                 unit,
                 mc_group_id_1,
                 mc_group_from_hw_2,
                 &mc_group_from_hw_size_exact_2,
                 list_of_next_entries_2
               );

  if(soc_sand_get_error_code_from_error_word(driver_ret) != SOC_SAND_OK)
  {
    goto exit;
  }

  if (!silent)
  {
    /*Print group from HW*/
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
    soc_sand_os_printf("The group from HW:\n\r");
    soc_petra_multicast_ing_group_print(
      mc_group_from_hw_2,
      mc_group_size_2
    );
    soc_sand_os_printf("-------------------------------------------------\n\r");
  }

  /* print the number of unocuupied entries */
  nof_unoccupied =
    soc_petra_sw_db_multicast_nof_unoccupied_get(unit);

  if (!silent)
  {
    /*Print SW DATABASE */
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
    soc_sand_os_printf("Software DataBase: \n\r");
    /* Number of unoccupied - should be equal to (total of entries - used)*/
    soc_sand_os_printf("\n\r"
      "# of unoccupied: %u \n\r",nof_unoccupied);

    /* Unoccupied entries list */
    soc_sand_os_printf("\n\r"
      "Unoccupied entries list \n\r");
    soc_petra_mult_tests_unoccupied_list_print(
      unit,
      mc_group_id_1,
      list_of_next_entries_2
    );

    /* Backwards Pointers list */
    soc_sand_os_printf("\n\r"
      "Backwards Pointers list \n\r");
    soc_petra_mult_tests_backwards_list_print(
      unit,
      mc_group_id_1,
      list_of_next_entries_2
    );
    soc_sand_os_printf("-------------------------------------------------\n\r");
  }

  /* check SW-DataBase: {*/
  used_entries = SOC_SAND_DIV_ROUND_UP(mc_group_size_2,SOC_PETRA_MULT_ING_NOF_DESTINATIONS_PER_ENTRY);

  if (nof_unoccupied != (SOC_PETRA_MULT_TABLE_SIZE - used_entries))
  {
    if(!silent)
    {
      soc_sand_os_printf("The Number of unoccupied entries is different than expected\n\r");
    }
    pass = FALSE;
    soc_sand_os_printf("soc_petra_mult_open_remove_from_group_TEST_correct_db : Test FAILED\n\r");
    goto exit;
  }

  pass = soc_petra_mult_tests_unoccupied_list_check_occupation(
           unit,
           mc_group_id_1,
           list_of_next_entries_2,
           silent
         );

  if (!pass)
  {
    soc_sand_os_printf("soc_petra_mult_open_remove_from_group_TEST_correct_db : Test FAILED\n\r");
    goto exit;
  }

  pass = soc_petra_mult_tests_backwards_list_check_occupation(
           unit,
           mc_group_id_1,
           list_of_next_entries_2,
           silent
         );

  if (!pass)
  {
    soc_sand_os_printf("soc_petra_mult_open_remove_from_group_TEST_correct_db : Test FAILED\n\r");
    goto exit;
  }
  /* check SW-DataBase: } */

  if(!silent)
  {
    /* to check-up on the erased entry*/
    soc_sand_os_printf("\n\r"
      "Unoccupied entries list OLD\n\r");
    soc_petra_mult_tests_unoccupied_list_print(
      unit,
      mc_group_id_1,
      list_of_next_entries_1
    );
    soc_sand_os_printf("\n\r"
      "Backwards Pointers list OLD\n\r");
    soc_petra_mult_tests_backwards_list_print(
      unit,
      mc_group_id_1,
      list_of_next_entries_1
    );
  }

  pass = soc_petra_mult_tests_unoccupied_list_check_occupation_compared_to_old(
           unit,
           mc_group_id_1,
           list_of_next_entries_1,
           silent
         );
  if (!pass)
  {
    soc_sand_os_printf("soc_petra_mult_update_group_TEST_correct_db : Test FAILED\n\r");
    goto exit;
  }
  if (pass)
  {
    soc_sand_os_printf("\n\r* soc_petra_mult_open_remove_from_group_TEST_correct_db : Test PASSED\n\r");
  }
exit:
  if (!silent)
  {
    soc_sand_os_printf("\n\r ********** soc_petra_mult_open_remove_from_group_TEST_correct_db : END **********\n\r");
  }

  SOC_PETRA_FREE(mc_group_from_hw_1);
  SOC_PETRA_FREE(mc_group_from_hw_2);
  SOC_PETRA_FREE(list_of_next_entries_1);
  SOC_PETRA_FREE(list_of_next_entries_2);
  SOC_PETRA_FREE(ing_mc_group_from_user_1);
  return pass;
}

/*****************************************************
*NAME
*  soc_petra_mult_open_close_group_TEST_correct_db
*TYPE:
*  PROC
*DATE:
*  15-Feb-2008
*FUNCTION:
*  Open 2 MC-GROUP with X entries (MC-ID = 0).
*  Close MC-GROUP (MC-ID = 0) and MC-GROUP (MC-ID = 1).
*  Check that DB is empty.
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
  soc_petra_mult_open_close_group_TEST_correct_db(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 silent
  )
{
   SOC_PETRA_MULT_ING_ENTRY
    *ing_mc_group_from_user_1 = NULL,
    *ing_mc_group_from_user_2 = NULL,
    *mc_group_from_hw_1 = NULL,
    *mc_group_from_hw_2 = NULL;
  int32
    pass = TRUE,
    i,
    mc_group_size_1,
    mc_group_size_2;
  SOC_TMC_ERROR
    mc_err;
  uint32
    mc_group_from_hw_size_exact_1,
    mc_group_from_hw_size_exact_2;
  SOC_PETRA_MULT_TESTS_GROUP_DIFF
    diff;
  uint16
    mc_group_id_1,
    mc_group_id_2,
    *list_of_next_entries_1 = NULL,
    *list_of_next_entries_2 = NULL;
  uint32
    driver_ret,
    nof_unoccupied;

  if (!silent)
  {
    soc_sand_os_printf("\n\r ********** soc_petra_mult_open_close_group_TEST_correct_db : START **********\n\r");
  }

/*
 * First Group
 */

  /*
   * Defining, Setting and Printing a new group
   * type = Ingress
   * size = 3
   * mc_id = 0
   */
  /* { */
  mc_group_size_1 = SOC_PETRA_MULT_TEST_6_ING_GROUP_SIZE_1;
  mc_group_id_1 = SOC_PETRA_MULT_TEST_6_ING_MC_GROUP_ID_1;

  ing_mc_group_from_user_1 = sal_alloc(sizeof(SOC_PETRA_MULT_ING_ENTRY) * mc_group_size_1,
    "soc_petra_mult_open_close_group_TEST_correct_db.ing_mc_group_from_user_1");
  if(ing_mc_group_from_user_1 == NULL) {
    pass = FALSE;
    goto exit;
  }

  for (i = 0 ; i < mc_group_size_1 ; i++)
  {
    soc_petra_PETRA_MULT_ING_ENTRY_clear(&ing_mc_group_from_user_1[i]);
  }

  soc_petra_mult_tests_create_ing_multicast_group(
    mc_group_size_1,
    ing_mc_group_from_user_1
  );

  if (!silent)
  {
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
    soc_sand_os_printf("The group from User:\n\r");

    soc_petra_multicast_ing_group_print(
      ing_mc_group_from_user_1,
      mc_group_size_1
    );
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
  }

  /* } */

  /* Opening a new group */
  driver_ret = soc_petra_mult_ing_group_open(
                 unit,
                 mc_group_id_1,
                 ing_mc_group_from_user_1,
                 mc_group_size_1,
                 &mc_err
               );
  if(soc_sand_get_error_code_from_error_word(driver_ret) != SOC_SAND_OK)
  {
    goto exit;
  }



  /* allocate mc-group before sending to get-from-hw function */
  mc_group_from_hw_1 = sal_alloc(sizeof(SOC_PETRA_MULT_ING_ENTRY) * mc_group_size_1,
    "soc_petra_mult_open_close_group_TEST_correct_db.mc_group_from_hw_1");
  if(mc_group_from_hw_1 == NULL) {
    pass = FALSE;
    goto exit;
  }

  list_of_next_entries_1 = sal_alloc(sizeof(uint16) * mc_group_size_1,
    "soc_petra_mult_open_close_group_TEST_correct_db.list_of_next_entries_1");
  if(list_of_next_entries_1 == NULL) {
    pass = FALSE;
    goto exit;
  }

  for (i = 0 ; i < mc_group_size_1 ; i++)
  {
    soc_petra_PETRA_MULT_ING_ENTRY_clear(&mc_group_from_hw_1[i]);
  }
  /* get-from-hw */

  driver_ret = soc_petra_mult_tests_get_ing_mc_group_from_hw(
                 unit,
                 mc_group_id_1,
                 mc_group_from_hw_1,
                 &mc_group_from_hw_size_exact_1,
                 list_of_next_entries_1
               );

  if(soc_sand_get_error_code_from_error_word(driver_ret) != SOC_SAND_OK)
  {
    goto exit;
  }

  if (!silent)
  {
    /*Print group from HW*/
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
    soc_sand_os_printf("The group from HW:\n\r");

    soc_petra_multicast_ing_group_print(
      mc_group_from_hw_1,
      mc_group_size_1
    );
    soc_sand_os_printf("-------------------------------------------------\n\r");
  }

  /* compare size and content */
  driver_ret = soc_petra_mult_ing_groups_compare(
                 ing_mc_group_from_user_1,
                 mc_group_size_1,
                 mc_group_from_hw_1,
                 mc_group_from_hw_size_exact_1,
                 &diff
               );
  if (diff != SOC_PETRA_MULT_TESTS_GROUP_DIFF_EQUAL)
  {
    if(!silent)
    {
      soc_sand_os_printf("The groups from HW is different than the group from SW\n\r");
    }
    pass = FALSE;
  }

  /* print the number of unoccupied entries */
  nof_unoccupied =
    soc_petra_sw_db_multicast_nof_unoccupied_get(unit);

  if (!silent)
  {
    /*Print SW DATABASE */
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
    soc_sand_os_printf("Software DataBase: \n\r");
    /* Number of unoccupied - should be equal to (total of entries - used)*/
    soc_sand_os_printf("\n\r"
      "# of unoccupied: %u \n\r",nof_unoccupied);

    /* Unoccupied entries list */
    soc_sand_os_printf("\n\r"
      "Unoccupied entries list: \n\r");
    soc_petra_mult_tests_unoccupied_list_print(
      unit,
      mc_group_id_1,
      list_of_next_entries_1
      );

    /* Backwards Pointers list */
    soc_sand_os_printf("\n\r"
      "Backwards Pointers list: \n\r");
    soc_petra_mult_tests_backwards_list_print(
      unit,
      mc_group_id_1,
      list_of_next_entries_1
      );
    soc_sand_os_printf("-------------------------------------------------\n\r");
  }

/*
 * Second Group
 */

  /*
   * Defining, Setting and Printing a new group
   * type = Ingress
   * size = 3
   * mc_id = 1
   */
  /* { */
  mc_group_size_2 = SOC_PETRA_MULT_TEST_6_ING_GROUP_SIZE_2;
  mc_group_id_2 = SOC_PETRA_MULT_TEST_6_ING_GROUP_SIZE_2;

  ing_mc_group_from_user_2 = sal_alloc(sizeof(SOC_PETRA_MULT_ING_ENTRY) * mc_group_size_2,
    "soc_petra_mult_open_close_group_TEST_correct_db.ing_mc_group_from_user_2");
  if(ing_mc_group_from_user_2 == NULL) {
    pass = FALSE;
    goto exit;
  }

  for (i = 0 ; i < mc_group_size_2 ; i++)
  {
    soc_petra_PETRA_MULT_ING_ENTRY_clear(&ing_mc_group_from_user_2[i]);
  }

  soc_petra_mult_tests_create_ing_multicast_group(
    mc_group_size_2,
    ing_mc_group_from_user_2
  );

  if (!silent)
  {
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
    soc_sand_os_printf("The group from User:\n\r");

    soc_petra_multicast_ing_group_print(
      ing_mc_group_from_user_2,
      mc_group_size_2
    );
    soc_sand_os_printf("-------------------------------------------------\n\r");
  }

  /* } */

  /* Opening a new group */
  driver_ret = soc_petra_mult_ing_group_open(
                 unit,
                 mc_group_id_2,
                 ing_mc_group_from_user_2,
                 mc_group_size_2,
                 &mc_err
               );
  if(soc_sand_get_error_code_from_error_word(driver_ret) != SOC_SAND_OK)
  {
    goto exit;
  }


  /* allocate mc-group before sending to get-from-hw function */
  mc_group_from_hw_2 = sal_alloc(sizeof(SOC_PETRA_MULT_ING_ENTRY) * mc_group_size_2,
    "soc_petra_mult_open_close_group_TEST_correct_db.mc_group_from_hw_2");
  if(mc_group_from_hw_2 == NULL) {
    pass = FALSE;
    goto exit;
  }

  list_of_next_entries_2 = sal_alloc(sizeof(uint16) * mc_group_size_2,
    "soc_petra_mult_open_close_group_TEST_correct_db.list_of_next_entries_2");
  if(list_of_next_entries_2 == NULL) {
    pass = FALSE;
    goto exit;
  }

  for (i = 0 ; i < mc_group_size_2 ; i++)
  {
    soc_petra_PETRA_MULT_ING_ENTRY_clear(&mc_group_from_hw_2[i]);
  }
  /* get-from-hw */

  driver_ret = soc_petra_mult_tests_get_ing_mc_group_from_hw(
                 unit,
                 mc_group_id_2,
                 mc_group_from_hw_2,
                 &mc_group_from_hw_size_exact_2,
                 list_of_next_entries_2
               );

  if(soc_sand_get_error_code_from_error_word(driver_ret) != SOC_SAND_OK)
  {
    goto exit;
  }

  if (!silent)
  {
    /*Print group from HW*/
    /* Petra code. Almost not in use. Ignore coverity defects */
    /* coverity[copy_paste_error] */
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
    soc_sand_os_printf("The group from HW:\n\r");

    soc_petra_multicast_ing_group_print(
      mc_group_from_hw_2,
      mc_group_size_2
    );
    soc_sand_os_printf("-------------------------------------------------\n\r");
  }

  /* compare size and content */
  driver_ret = soc_petra_mult_ing_groups_compare(
                 ing_mc_group_from_user_2,
                 mc_group_size_2,
                 mc_group_from_hw_2,
                 mc_group_from_hw_size_exact_2,
                 &diff
               );
  if (diff != SOC_PETRA_MULT_TESTS_GROUP_DIFF_EQUAL)
  {
     if(!silent)
     {
       soc_sand_os_printf("The groups from HW is different than the group from SW\n\r");
     }
     pass = FALSE;
  }

  /* print the number of unoccupied entries */
  nof_unoccupied =
    soc_petra_sw_db_multicast_nof_unoccupied_get(unit);

  if (!silent)
  {
    /*Print SW DATABASE */
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
    soc_sand_os_printf("Software DataBase: \n\r");
    /* Number of unoccupied - should be equal to (total of entries - used)*/
    soc_sand_os_printf("\n\r"
      "# of unoccupied: %u \n\r",nof_unoccupied);

    /* Unoccupied entries list */
    soc_sand_os_printf("\n\r"
      "Unoccupied entries list: \n\r");
    soc_petra_mult_tests_unoccupied_list_print(
      unit,
      mc_group_id_2,
      list_of_next_entries_2
    );

    /* Backwards Pointers list */
    soc_sand_os_printf("\n\r"
      "Backwards Pointers list: \n\r");
    soc_petra_mult_tests_backwards_list_print(
      unit,
      mc_group_id_2,
      list_of_next_entries_2
    );
    soc_sand_os_printf("-------------------------------------------------\n\r");
  }

 driver_ret = soc_petra_mult_ing_all_groups_close(unit);
 if(soc_sand_get_error_code_from_error_word(driver_ret) != SOC_SAND_OK)
 {
   goto exit;
 }

 /* print the number of unoccupied entries */
 nof_unoccupied =
    soc_petra_sw_db_multicast_nof_unoccupied_get(unit);

 /*Print SW DATABASE 1*/
 if (!silent)
 {
   /*Print SW DATABASE */
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
   /* Number of unoccupied - should be equal to (total of entries - used)*/
   soc_sand_os_printf("\n\r"
     "# of unoccupied: %u \n\r",nof_unoccupied);

   /* Unoccupied entries list */
   soc_sand_os_printf("\n\r"
     "Unoccupied entries list: \n\r");
   soc_petra_mult_tests_unoccupied_list_print(
     unit,
     mc_group_id_1,
     list_of_next_entries_1
     );

   /* Backwards Pointers list */
   soc_sand_os_printf("\n\r"
     "Backwards Pointers list: \n\r");
   soc_petra_mult_tests_backwards_list_print(
     unit,
     mc_group_id_1,
     list_of_next_entries_1
      );
   soc_sand_os_printf("-------------------------------------------------\n\r");
 }

 /* print the number of unoccupied entries */
 nof_unoccupied =
    soc_petra_sw_db_multicast_nof_unoccupied_get(unit);

 /*Print SW DATABASE 2*/
 if (!silent)
 {
   /*Print SW DATABASE */
   soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
   /* Number of unoccupied - should be equal to (total of entries - used)*/
   soc_sand_os_printf("\n\r"
     "# of unoccupied: %u \n\r",nof_unoccupied);

   /* Unoccupied entries list */
   soc_sand_os_printf("\n\r"
     "Unoccupied entries list \n\r");
   soc_petra_mult_tests_unoccupied_list_print(
     unit,
     mc_group_id_2,
     list_of_next_entries_2
     );

   /* Backwards Pointers list */
   soc_sand_os_printf("\n\r"
     "Backwards Pointers list \n\r");
   soc_petra_mult_tests_backwards_list_print(
     unit,
     mc_group_id_2,
     list_of_next_entries_2
    );
   soc_sand_os_printf("-------------------------------------------------\n\r");
 }

  /* check SW-DataBase: {*/

  if (nof_unoccupied != SOC_PETRA_MULT_TABLE_SIZE)
  {
    if(!silent)
    {
      soc_sand_os_printf("The Number of unoccupied entries is different than expected\n\r");
    }
    pass = FALSE;
    soc_sand_os_printf("soc_petra_mult_open_close_group_TEST_correct_db : Test FAILED\n\r");
    goto exit;
  }

  pass = soc_petra_mult_tests_unoccupied_list_check_that_not_occupied(
           unit,
           mc_group_id_1,
           list_of_next_entries_1,
           silent
         );

  if (!pass)
  {
    soc_sand_os_printf("soc_petra_mult_open_close_group_TEST_correct_db : Test FAILED\n\r");
    goto exit;
  }

  pass = soc_petra_mult_tests_unoccupied_list_check_that_not_occupied(
           unit,
           mc_group_id_2,
           list_of_next_entries_2,
           silent
         );

  if (!pass)
  {
    soc_sand_os_printf("soc_petra_mult_open_close_group_TEST_correct_db : Test FAILED\n\r");
    goto exit;
  }
  /* check SW-DataBase: } */

  if (pass)
  {
    soc_sand_os_printf("\n\r*   soc_petra_mult_open_close_group_TEST_correct_db : Test PASSED\n\r");
  }

exit:
  if (!silent)
  {
   soc_sand_os_printf("\n\r ********** soc_petra_mult_open_close_group_TEST_correct_db : END **********\n\r");
  }

  SOC_PETRA_FREE(ing_mc_group_from_user_1);
  SOC_PETRA_FREE(ing_mc_group_from_user_2);
  SOC_PETRA_FREE(mc_group_from_hw_1);
  SOC_PETRA_FREE(mc_group_from_hw_2);
  SOC_PETRA_FREE(list_of_next_entries_1);
  SOC_PETRA_FREE(list_of_next_entries_2);

  return pass;
}


/*****************************************************
*NAME
*  soc_petra_mult_open_relocation_TEST_correct_db
*TYPE:
*  PROC
*DATE:
*  15-Feb-2008
*FUNCTION:
*  Check the reloaction Operation is performed correctly.
*  Relocation is required when a MC-Group is to be opened with
*  a MC-ID that is occupied in the multicast table by another MC-Group
*  linked-list.
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
  soc_petra_mult_open_relocation_TEST_correct_db(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 silent
  )
{
   SOC_PETRA_MULT_ING_ENTRY
    *ing_mc_group_from_user_1 = NULL,
    *ing_mc_group_from_hw_1 = NULL;
   SOC_PETRA_MULT_EG_ENTRY
     *eg_mc_group_from_user_2 = NULL,
     *eg_mc_group_from_hw_2 = NULL;
  int32
    pass = TRUE,
    i,
    ing_mc_group_size_1,
    eg_mc_group_size_2;
  uint8
    insufficient_memory = FALSE;
  SOC_TMC_ERROR
    mc_err;
  uint32
    mc_group_from_hw_size_exact_1,
    mc_group_from_hw_size_exact_2;
  SOC_PETRA_MULT_TESTS_GROUP_DIFF
    diff;
  uint16
    ing_mc_group_id_1,
    eg_mc_group_id_2,
    *ing_list_of_next_entries_1 = NULL,
    *eg_list_of_next_entries_2 = NULL;
  uint32
    driver_ret,
    used_entries,
    nof_unoccupied;

  if (!silent)
  {
    soc_sand_os_printf("\n\r ********** soc_petra_mult_open_relocation_TEST_correct_db : START **********\n\r");
  }

/*
 * First Group
 */

  /*
   * Defining, Setting and Printing a new group
   * type = Ingress
   * size = 5
   * mc_id = 0
   */
  /* { */
  ing_mc_group_size_1 = SOC_PETRA_MULT_TEST_7_ING_GROUP_SIZE;
  ing_mc_group_id_1 = SOC_PETRA_MULT_TEST_7_ING_MC_GROUP_ID;

  ing_mc_group_from_user_1 = sal_alloc(sizeof(SOC_PETRA_MULT_ING_ENTRY) * ing_mc_group_size_1,
    "soc_petra_mult_open_relocation_TEST_correct_db.ing_mc_group_from_user_1");
  if(ing_mc_group_from_user_1 == NULL) {
    pass = FALSE;
    goto exit;
  }

  for (i = 0 ; i < ing_mc_group_size_1 ; i++)
  {
    soc_petra_PETRA_MULT_ING_ENTRY_clear(&ing_mc_group_from_user_1[i]);
  }

  soc_petra_mult_tests_create_ing_multicast_group(
    ing_mc_group_size_1,
    ing_mc_group_from_user_1
  );

  if (!silent)
  {
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
    soc_sand_os_printf("The ING group from User:\n\r");

    soc_petra_multicast_ing_group_print(
      ing_mc_group_from_user_1,
      ing_mc_group_size_1
    );
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
  }

  /* } */

  /* Opening a new group */
  driver_ret = soc_petra_mult_ing_group_open(
                 unit,
                 ing_mc_group_id_1,
                 ing_mc_group_from_user_1,
                 ing_mc_group_size_1,
                 &mc_err
               );
  if(soc_sand_get_error_code_from_error_word(driver_ret) != SOC_SAND_OK)
  {
    goto exit;
  }



  /* allocate mc-group before sending to get-from-hw function */
  ing_mc_group_from_hw_1 = sal_alloc(sizeof(SOC_PETRA_MULT_ING_ENTRY) * ing_mc_group_size_1,
    "soc_petra_mult_open_relocation_TEST_correct_db.ing_mc_group_from_hw_1");
  if(ing_mc_group_from_hw_1 == NULL) {
    pass = FALSE;
    goto exit;
  }

  ing_list_of_next_entries_1 = sal_alloc(sizeof(uint16) * ing_mc_group_size_1,
    "soc_petra_mult_open_relocation_TEST_correct_db.ing_list_of_next_entries_1");
  if(ing_list_of_next_entries_1 == NULL) {
    pass = FALSE;
    goto exit;
  }

  for (i = 0 ; i < ing_mc_group_size_1 ; i++)
  {
    soc_petra_PETRA_MULT_ING_ENTRY_clear(&ing_mc_group_from_hw_1[i]);
  }
  /* get-from-hw */

  driver_ret = soc_petra_mult_tests_get_ing_mc_group_from_hw(
                 unit,
                 ing_mc_group_id_1,
                 ing_mc_group_from_hw_1,
                 &mc_group_from_hw_size_exact_1,
                 ing_list_of_next_entries_1
               );

  if(soc_sand_get_error_code_from_error_word(driver_ret) != SOC_SAND_OK)
  {
    goto exit;
  }

  if (!silent)
  {
    /*Print group from HW*/
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
    soc_sand_os_printf("The group from HW:\n\r");

    soc_petra_multicast_ing_group_print(
      ing_mc_group_from_hw_1,
      ing_mc_group_size_1
    );
    soc_sand_os_printf("-------------------------------------------------\n\r");
  }

  /* compare size and content */
  driver_ret = soc_petra_mult_ing_groups_compare(
                 ing_mc_group_from_user_1,
                 ing_mc_group_size_1,
                 ing_mc_group_from_hw_1,
                 mc_group_from_hw_size_exact_1,
                 &diff
               );
  if (diff != SOC_PETRA_MULT_TESTS_GROUP_DIFF_EQUAL)
  {
    if(!silent)
    {
      soc_sand_os_printf("The groups from HW is different than the group from SW\n\r");
    }
    pass = FALSE;
  }

  /* print the number of unoccupied entries */
  nof_unoccupied =
    soc_petra_sw_db_multicast_nof_unoccupied_get(unit);

  if (!silent)
  {
    /*Print SW DATABASE */
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
    soc_sand_os_printf("Software DataBase: \n\r");
    /* Number of unoccupied - should be equal to (total of entries - used)*/
    soc_sand_os_printf("\n\r"
      "# of unoccupied: %u \n\r",nof_unoccupied);

    /* Unoccupied entries list */
    soc_sand_os_printf("\n\r"
      "Unoccupied entries list: \n\r");
    soc_petra_mult_tests_unoccupied_list_print(
      unit,
      ing_mc_group_id_1,
      ing_list_of_next_entries_1
      );

    /* Backwards Pointers list */
    soc_sand_os_printf("\n\r"
      "Backwards Pointers list: \n\r");
    soc_petra_mult_tests_backwards_list_print(
      unit,
      ing_mc_group_id_1,
      ing_list_of_next_entries_1
      );
    soc_sand_os_printf("-------------------------------------------------\n\r");
  }

  /* check SW-DataBase: {*/
  used_entries = SOC_SAND_DIV_ROUND_UP(ing_mc_group_size_1,SOC_PETRA_MULT_ING_NOF_DESTINATIONS_PER_ENTRY);

  if (nof_unoccupied != (SOC_PETRA_MULT_TABLE_SIZE - used_entries))
  {
    if(!silent)
    {
      soc_sand_os_printf("The Number of unoccupied entries is different than expected\n\r");
    }
    pass = FALSE;
    soc_sand_os_printf("soc_petra_mult_open_relocation_TEST_correct_db : Test FAILED\n\r");
    goto exit;
  }

  pass = soc_petra_mult_tests_unoccupied_list_check_occupation(
           unit,
           ing_mc_group_id_1,
           ing_list_of_next_entries_1,
           silent
         );

  if (!pass)
  {
    soc_sand_os_printf("soc_petra_mult_open_relocation_TEST_correct_db : Test FAILED\n\r");
    goto exit;
  }

  pass = soc_petra_mult_tests_backwards_list_check_occupation(
           unit,
           ing_mc_group_id_1,
           ing_list_of_next_entries_1,
           silent
         );

  if (!pass)
  {
    soc_sand_os_printf("soc_petra_mult_open_relocation_TEST_correct_db : Test FAILED\n\r");
    goto exit;
  }
  /* check SW-DataBase: } */


/*
 * Second Group (Egress)
 */

  /*
   * Defining, Setting and Printing a new group
   * type = Egress
   * size = 2
   * mc_id = 16383
   */
  /* { */
  eg_mc_group_size_2 = SOC_PETRA_MULT_TEST_7_EG_MC_GROUP_ID;
  eg_mc_group_id_2 = SOC_PETRA_MULT_TEST_7_EG_MC_GROUP_ID;

  eg_mc_group_from_user_2 = sal_alloc(sizeof(SOC_PETRA_MULT_EG_ENTRY) * eg_mc_group_size_2,
    "soc_petra_mult_open_relocation_TEST_correct_db.eg_mc_group_from_user_2");
  if(eg_mc_group_from_user_2 == NULL) {
    pass = FALSE;
    goto exit;
  }

  for (i = 0 ; i < eg_mc_group_size_2 ; i++)
  {
    soc_petra_PETRA_MULT_EG_ENTRY_clear(&eg_mc_group_from_user_2[i]);
  }

  soc_petra_mult_tests_create_eg_multicast_group(
    eg_mc_group_size_2,
    eg_mc_group_from_user_2
  );

  if (!silent)
  {
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
    soc_sand_os_printf("The EG group from User:\n\r");

    soc_petra_multicast_eg_group_print(
      eg_mc_group_from_user_2,
      eg_mc_group_size_2
    );
    soc_sand_os_printf("-------------------------------------------------\n\r");
  }

  /* } */

  /* Opening a new group */
  driver_ret = soc_petra_mult_eg_group_open(
                 unit,
                 eg_mc_group_id_2,
                 eg_mc_group_from_user_2,
                 eg_mc_group_size_2,
                 &insufficient_memory
               );
  if(soc_sand_get_error_code_from_error_word(driver_ret) != SOC_SAND_OK)
  {
    goto exit;
  }


  /* allocate mc-group before sending to get-from-hw function */
  eg_mc_group_from_hw_2 = sal_alloc(sizeof(SOC_PETRA_MULT_EG_ENTRY) * eg_mc_group_size_2,
    "soc_petra_mult_open_relocation_TEST_correct_db.eg_mc_group_from_hw_2");
  if(eg_mc_group_from_hw_2 == NULL) {
    pass = FALSE;
    goto exit;
  }

  eg_list_of_next_entries_2 = sal_alloc(sizeof(uint16) * eg_mc_group_size_2,
    "soc_petra_mult_open_relocation_TEST_correct_db.eg_list_of_next_entries_2");
  if(eg_list_of_next_entries_2 == NULL) {
    pass = FALSE;
    goto exit;
  }

  for (i = 0 ; i < eg_mc_group_size_2 ; i++)
  {
    soc_petra_PETRA_MULT_EG_ENTRY_clear(&eg_mc_group_from_hw_2[i]);
  }

  /* get-from-hw */

  driver_ret = soc_petra_mult_tests_get_eg_mc_group_from_hw(
                 unit,
                 eg_mc_group_id_2,
                 eg_mc_group_from_hw_2,
                 &mc_group_from_hw_size_exact_2,
                 eg_list_of_next_entries_2
               );

  if(soc_sand_get_error_code_from_error_word(driver_ret) != SOC_SAND_OK)
  {
    goto exit;
  }

  if (!silent)
  {
    /*Print group from HW*/
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
    soc_sand_os_printf("The group from HW:\n\r");

    soc_petra_multicast_eg_group_print(
      eg_mc_group_from_hw_2,
      eg_mc_group_size_2
    );
    soc_sand_os_printf("-------------------------------------------------\n\r");
  }

  /* compare size and content */
  driver_ret = soc_petra_mult_eg_groups_compare(
                 eg_mc_group_from_user_2,
                 eg_mc_group_size_2,
                 eg_mc_group_from_hw_2,
                 mc_group_from_hw_size_exact_2,
                 &diff
               );
  if (diff != SOC_PETRA_MULT_TESTS_GROUP_DIFF_EQUAL)
  {
     if(!silent)
     {
       soc_sand_os_printf("The groups from HW is different than the group from SW\n\r");
     }
     pass = FALSE;
  }

  /* print the number of unoccupied entries */
  nof_unoccupied =
    soc_petra_sw_db_multicast_nof_unoccupied_get(unit);


  if (!silent)
  {
    /*Print SW DATABASE */
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
    soc_sand_os_printf("Software DataBase: \n\r");
    /* Number of unoccupied - should be equal to (total of entries - used)*/
    soc_sand_os_printf("\n\r"
      "# of unoccupied: %u \n\r",nof_unoccupied);

    /* Unoccupied entries list */
    /* if egress - concatenate with '1' at msb , and 3 ports per entry */
    soc_sand_os_printf("\n\r"
      "Unoccupied entries list: \n\r");
    soc_petra_mult_tests_unoccupied_list_print(
      unit,
      (uint16)(eg_mc_group_id_2 | SOC_PETRA_MULT_LL_EGRESS_MC_ID_START_ENTRY),
      eg_list_of_next_entries_2
    );

    /* Backwards Pointers list */
    /* if egress - concatenate with '1' at msb , and 3 ports per entry */
    soc_sand_os_printf("\n\r"
      "Backwards Pointers list: \n\r");
    soc_petra_mult_tests_backwards_list_print(
      unit,
      (uint16)(eg_mc_group_id_2 | SOC_PETRA_MULT_LL_EGRESS_MC_ID_START_ENTRY),
      eg_list_of_next_entries_2
    );
    soc_sand_os_printf("-------------------------------------------------\n\r");
  }

    /* check SW-DataBase: {*/
  used_entries = SOC_SAND_DIV_ROUND_UP(ing_mc_group_size_1,SOC_PETRA_MULT_ING_NOF_DESTINATIONS_PER_ENTRY)
    + SOC_SAND_DIV_ROUND_UP(eg_mc_group_size_2,SOC_PETRA_MULT_EG_NOF_PORTS_PER_ENTRY);

  if (nof_unoccupied != (SOC_PETRA_MULT_TABLE_SIZE - used_entries))
  {
    if(!silent)
    {
      soc_sand_os_printf("The Number of unoccupied entries is different than expected\n\r");
    }
    pass = FALSE;
    soc_sand_os_printf("soc_petra_mult_open_relocation_TEST_correct_db : Test FAILED\n\r");
    goto exit;
  }

  pass = soc_petra_mult_tests_unoccupied_list_check_occupation(
           unit,
           (uint16)(eg_mc_group_id_2 | SOC_PETRA_MULT_LL_EGRESS_MC_ID_START_ENTRY),
           eg_list_of_next_entries_2,
           silent
         );

  if (!pass)
  {
    soc_sand_os_printf("soc_petra_mult_open_relocation_TEST_correct_db : Test FAILED\n\r");
    goto exit;
  }

  pass = soc_petra_mult_tests_backwards_list_check_occupation(
           unit,
           (uint16)(eg_mc_group_id_2 | SOC_PETRA_MULT_LL_EGRESS_MC_ID_START_ENTRY),
           eg_list_of_next_entries_2,
           silent
         );

  if (!pass)
  {
    soc_sand_os_printf("soc_petra_mult_open_relocation_TEST_correct_db : Test FAILED\n\r");
    goto exit;
  }
  /* check SW-DataBase: } */

  /* check that the new MC-Group is the same as previous: */

  /* get-from-hw */
  driver_ret = soc_petra_mult_tests_get_ing_mc_group_from_hw(
                 unit,
                 ing_mc_group_id_1,
                 ing_mc_group_from_hw_1,
                 &mc_group_from_hw_size_exact_1,
                 ing_list_of_next_entries_1
               );

  if(soc_sand_get_error_code_from_error_word(driver_ret) != SOC_SAND_OK)
  {
    goto exit;
  }

  if (!silent)
  {
    /*Print group from HW*/
    /* Petra code. Almost not in use. Ignore coverity defects */
    /* coverity[copy_paste_error] */
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
    soc_sand_os_printf("The group from HW:\n\r");

    soc_petra_multicast_ing_group_print(
      ing_mc_group_from_hw_1,
      ing_mc_group_size_1
    );
    soc_sand_os_printf("-------------------------------------------------\n\r");
  }

  /* compare size and content */
  driver_ret = soc_petra_mult_ing_groups_compare(
                 ing_mc_group_from_user_1,
                 ing_mc_group_size_1,
                 ing_mc_group_from_hw_1,
                 mc_group_from_hw_size_exact_1,
                 &diff
               );
  if (diff != SOC_PETRA_MULT_TESTS_GROUP_DIFF_EQUAL)
  {
    if(!silent)
    {
      soc_sand_os_printf("The groups from HW is different than the group from SW\n\r");
    }
    pass = FALSE;
  }

  /* print the number of unoccupied entries */
  nof_unoccupied =
    soc_petra_sw_db_multicast_nof_unoccupied_get(unit);

  if (!silent)
  {
    /*Print SW DATABASE */
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
    soc_sand_os_printf("Software DataBase: \n\r");
    /* Number of unoccupied - should be equal to (total of entries - used)*/
    soc_sand_os_printf("\n\r"
      "# of unoccupied: %u \n\r",nof_unoccupied);

    /* Unoccupied entries list */
    soc_sand_os_printf("\n\r"
      "Unoccupied entries list: \n\r");
    soc_petra_mult_tests_unoccupied_list_print(
      unit,
      ing_mc_group_id_1,
      ing_list_of_next_entries_1
      );

    /* Backwards Pointers list */
    soc_sand_os_printf("\n\r"
      "Backwards Pointers list: \n\r");
    soc_petra_mult_tests_backwards_list_print(
      unit,
      ing_mc_group_id_1,
      ing_list_of_next_entries_1
      );
    soc_sand_os_printf("-------------------------------------------------\n\r");
  }

  /* check SW-DataBase: {*/
  used_entries = SOC_SAND_DIV_ROUND_UP(ing_mc_group_size_1,SOC_PETRA_MULT_ING_NOF_DESTINATIONS_PER_ENTRY)
    + SOC_SAND_DIV_ROUND_UP(eg_mc_group_size_2,SOC_PETRA_MULT_EG_NOF_PORTS_PER_ENTRY);

  if (nof_unoccupied != (SOC_PETRA_MULT_TABLE_SIZE - used_entries))
  {
    if(!silent)
    {
      soc_sand_os_printf("The Number of unoccupied entries is different than expected\n\r");
    }
    pass = FALSE;
    soc_sand_os_printf("soc_petra_mult_open_relocation_TEST_correct_db : Test FAILED\n\r");
    goto exit;
  }

  pass = soc_petra_mult_tests_unoccupied_list_check_occupation(
    unit,
    ing_mc_group_id_1,
    ing_list_of_next_entries_1,
    silent
    );

  if (!pass)
  {
    soc_sand_os_printf("soc_petra_mult_open_relocation_TEST_correct_db : Test FAILED\n\r");
    goto exit;
  }

  pass = soc_petra_mult_tests_backwards_list_check_occupation(
    unit,
    ing_mc_group_id_1,
    ing_list_of_next_entries_1,
    silent
    );

  if (!pass)
  {
    soc_sand_os_printf("soc_petra_mult_open_relocation_TEST_correct_db : Test FAILED\n\r");
    goto exit;
  }
  /* check SW-DataBase: } */

  if (pass)
  {
    soc_sand_os_printf("\n\r*   soc_petra_mult_open_relocation_TEST_correct_db : Test PASSED\n\r");
  }

exit:
  if (!silent)
  {
   soc_sand_os_printf("\n\r ********** soc_petra_mult_open_relocation_TEST_correct_db : END **********\n\r");
  }

  SOC_PETRA_FREE(ing_mc_group_from_user_1);
  SOC_PETRA_FREE(ing_mc_group_from_hw_1);
  SOC_PETRA_FREE(eg_mc_group_from_user_2);
  SOC_PETRA_FREE(eg_mc_group_from_hw_2);
  SOC_PETRA_FREE(ing_list_of_next_entries_1);
  SOC_PETRA_FREE(eg_list_of_next_entries_2);

  return pass;
}


/*****************************************************
*NAME
*  soc_petra_mult_occupied_but_empty_TEST_correct_db
*TYPE:
*  PROC
*DATE:
*  15-Feb-2008
*FUNCTION:
* Verification that Data Base skips over an occupied but empty entry for allocation
* 1.  Allocate a MC-Group with no entries.
* 2.  Try and Allocate in the same space (with a new MC-Group).
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
  soc_petra_mult_occupied_but_empty_TEST_correct_db(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 silent
  )
{
  SOC_PETRA_MULT_ING_ENTRY
    *ing_mc_group_from_user_1 = NULL,
    *mc_group_from_hw_1 = NULL;
  int32
    pass = TRUE,
    i,
    mc_group_size_1;
  SOC_TMC_ERROR
    mc_err;
  uint32
    mc_group_from_hw_size_exact_1;
  SOC_PETRA_MULT_TESTS_GROUP_DIFF
    diff;
  uint16
    mc_group_id_1,
    *list_of_next_entries_1 = NULL;
  uint32
    driver_ret,
    used_entries,
    nof_unoccupied;

  if (!silent)
  {
    soc_sand_os_printf("\n\r ********** soc_petra_mult_occupied_but_empty_TEST_correct_db : START **********\n\r");
  }

/*
 * First Group
 */

  /*
   * Defining, Setting and Printing a new group
   * type = Ingress
   * size = 0
   * mc_id = 0
   */
  /* { */
  mc_group_size_1 = SOC_PETRA_MULT_TEST_8_ING_GROUP_SIZE;
  mc_group_id_1 = SOC_PETRA_MULT_TEST_8_ING_MC_GROUP_ID;

  ing_mc_group_from_user_1 = sal_alloc(sizeof(SOC_PETRA_MULT_ING_ENTRY) * mc_group_size_1,
    "soc_petra_mult_occupied_but_empty_TEST_correct_db.ing_mc_group_from_user_1");
  if(ing_mc_group_from_user_1 == NULL) {
    pass = FALSE;
    goto exit;
  }

  for (i = 0 ; i < mc_group_size_1 ; i++)
  {
    soc_petra_PETRA_MULT_ING_ENTRY_clear(&ing_mc_group_from_user_1[i]);
  }

  soc_petra_mult_tests_create_ing_multicast_group(
    mc_group_size_1,
    ing_mc_group_from_user_1
  );

  if (!silent)
  {
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
    soc_sand_os_printf("The group from User:\n\r");

    soc_petra_multicast_ing_group_print(
      ing_mc_group_from_user_1,
      mc_group_size_1
    );
    soc_sand_os_printf("-------------------------------------------------\n\r");
  }

  /* } */

  /* Opening a new group */
  driver_ret = soc_petra_mult_ing_group_open(
                 unit,
                 mc_group_id_1,
                 ing_mc_group_from_user_1,
                 mc_group_size_1,
                 &mc_err
               );
  if(soc_sand_get_error_code_from_error_word(driver_ret) != SOC_SAND_OK)
  {
    goto exit;
  }

  /* allocate mc-group before sending to get-from-hw function */
  mc_group_from_hw_1 = sal_alloc(sizeof(SOC_PETRA_MULT_ING_ENTRY) * mc_group_size_1,
    "soc_petra_mult_occupied_but_empty_TEST_correct_db.mc_group_from_hw_1");
  if(mc_group_from_hw_1 == NULL) {
    pass = FALSE;
    goto exit;
  }

  list_of_next_entries_1 = sal_alloc(sizeof(uint16) * mc_group_size_1,
    "soc_petra_mult_occupied_but_empty_TEST_correct_db.list_of_next_entries_1");
  if(list_of_next_entries_1 == NULL) {
    pass = FALSE;
    goto exit;
  }

  for (i = 0 ; i < mc_group_size_1 ; i++)
  {
    soc_petra_PETRA_MULT_ING_ENTRY_clear(&mc_group_from_hw_1[i]);
  }
  /* get-from-hw */

  driver_ret = soc_petra_mult_tests_get_ing_mc_group_from_hw(
                 unit,
                 mc_group_id_1,
                 mc_group_from_hw_1,
                 &mc_group_from_hw_size_exact_1,
                 list_of_next_entries_1
               );

  if(soc_sand_get_error_code_from_error_word(driver_ret) != SOC_SAND_OK)
  {
    goto exit;
  }

  if (!silent)
  {
    /*Print group from HW*/
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
    soc_sand_os_printf("The group from HW:\n\r");

    soc_petra_multicast_ing_group_print(
      mc_group_from_hw_1,
      mc_group_size_1
    );
    soc_sand_os_printf("-------------------------------------------------\n\r");
  }

  /* compare size and content */
  driver_ret = soc_petra_mult_ing_groups_compare(
                 ing_mc_group_from_user_1,
                 mc_group_size_1,
                 mc_group_from_hw_1,
                 mc_group_from_hw_size_exact_1,
                 &diff
               );
  if (diff != SOC_PETRA_MULT_TESTS_GROUP_DIFF_EQUAL)
  {
    if(!silent)
    {
      soc_sand_os_printf("The groups from HW is different than the group from SW\n\r");
    }
    pass = FALSE;
  }

  /* print the number of unoccupied entries */
  nof_unoccupied =
    soc_petra_sw_db_multicast_nof_unoccupied_get(unit);

  if (!silent)
  {
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
    soc_sand_os_printf("Software DataBase: \n\r");
    /*Print SW DATABASE */
    /* Number of unoccupied - should be equal to (total of entries - used)*/
    soc_sand_os_printf("\n\r"
      "# of unoccupied: %u \n\r",nof_unoccupied);

    /* Unoccupied entries list */
    soc_sand_os_printf("\n\r"
      "Unoccupied entries list: \n\r");
    soc_petra_mult_tests_unoccupied_list_print(
      unit,
      mc_group_id_1,
      list_of_next_entries_1
      );

    /* Backwards Pointers list */
    soc_sand_os_printf("\n\r"
      "Backwards Pointers list: \n\r");
    soc_petra_mult_tests_backwards_list_print(
      unit,
      mc_group_id_1,
      list_of_next_entries_1
      );
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
  }

  /* check SW-DataBase: {*/
  if (mc_group_size_1 == 0)
  {
    /* Petra code. Almost not in use. Ignore coverity defects */
    /* coverity[dead_error_line] */
    used_entries = 1;
  }
  else
  {
    used_entries = SOC_SAND_DIV_ROUND_UP(mc_group_size_1,SOC_PETRA_MULT_ING_NOF_DESTINATIONS_PER_ENTRY);
  }

  if (nof_unoccupied != (SOC_PETRA_MULT_TABLE_SIZE - used_entries))
  {
    if(!silent)
    {
      soc_sand_os_printf("The Number of unoccupied entries is different than expected\n\r");
    }
    pass = FALSE;
    soc_sand_os_printf("soc_petra_mult_occupied_but_empty_TEST_correct_db : Test FAILED");
    goto exit;
  }

  pass = soc_petra_mult_tests_unoccupied_list_check_occupation(
           unit,
           mc_group_id_1,
           list_of_next_entries_1,
           silent
         );

  if (!pass)
  {
    soc_sand_os_printf("soc_petra_mult_occupied_but_empty_TEST_correct_db : Test FAILED");
    goto exit;
  }

  pass = soc_petra_mult_tests_backwards_list_check_occupation(
           unit,
           mc_group_id_1,
           list_of_next_entries_1,
           silent
         );

  if (!pass)
  {
    soc_sand_os_printf("soc_petra_mult_occupied_but_empty_TEST_correct_db : Test FAILED");
    goto exit;
  }
  /* check SW-DataBase: } */

  if (pass)
  {
    soc_sand_os_printf("\n\r* soc_petra_mult_occupied_but_empty_TEST_correct_db : Test PASSED\n\r");
  }
exit:
  if (!silent)
  {
    soc_sand_os_printf("\n\r ********** soc_petra_mult_occupied_but_empty_TEST_correct_db : END **********\n\r");
  }

  SOC_PETRA_FREE(ing_mc_group_from_user_1);
  SOC_PETRA_FREE(list_of_next_entries_1);
  SOC_PETRA_FREE(mc_group_from_hw_1);
  return pass;
}


/*****************************************************
*NAME
*  soc_petra_mult_bitmap_allocation_TEST
*TYPE:
*  PROC
*DATE:
*  15-Feb-2008
*FUNCTION:
* Verification of Bitmap Allocation - tests that after
* a configuration of a bitmap range the program react correctly,
* meaning that a linked-list mc-group cannot be opened in that range
* and that a bitmap mc-group group cannot be opened out of this range.
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
*   This Test does not change the 'pass' variable -
*   it will always return TRUE at current implementation.
*SEE ALSO:
*****************************************************/
uint32
  soc_petra_mult_bitmap_allocation_TEST(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 silent
  )
{
  SOC_PETRA_MULT_EG_ENTRY
    *eg_mc_group_from_user_1 = NULL,
    *mc_group_from_hw_1 = NULL;
  SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_GROUP
    bitmap_mc_group,
    bitmap_mc_group_from_hw;
  int32
    pass = TRUE,
    i,
    mc_group_size_1;
  uint8
    insufficient_memory = FALSE;
  uint32
    mc_group_from_hw_size_exact_1;
  uint16
    mc_group_id_1,
    bitmap_mc_group_id_2,
    *list_of_next_entries_1 = NULL;
  uint32
    driver_ret;
  SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE
    info;

  if (!silent)
  {
    soc_sand_os_printf("\n\r ********** soc_petra_mult_bitmap_allocation_TEST : START **********\n\r");
  }

  soc_petra_PETRA_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE_clear(&info);

  info.mc_id_low = SOC_PETRA_MULT_TEST_9_BITMAP_RNG_LOW ;
  info.mc_id_high = SOC_PETRA_MULT_TEST_9_BITMAP_RNG_HIGH ;

  driver_ret = soc_petra_mult_eg_vlan_membership_group_range_set(
                 unit,
                 &info
               );
  if(soc_sand_get_error_code_from_error_word(driver_ret) != SOC_SAND_OK)
  {
    goto exit;
  }

  soc_petra_PETRA_MULT_EG_VLAN_MEMBERSHIP_GROUP_clear(&bitmap_mc_group);
  soc_petra_PETRA_MULT_EG_VLAN_MEMBERSHIP_GROUP_clear(&bitmap_mc_group_from_hw);

  /*
   * First Group
   */

  /*
   * Defining, Setting and Printing a new group
   * type = Egress
   * size = 1
   * mc_id = 0
   */
  /* { */
  mc_group_size_1 = SOC_PETRA_MULT_TEST_9_EG_GROUP_SIZE;
  mc_group_id_1 = SOC_PETRA_MULT_TEST_9_EG_MC_GROUP_ID;

  eg_mc_group_from_user_1 = sal_alloc(sizeof(SOC_PETRA_MULT_EG_ENTRY) * mc_group_size_1,
    "soc_petra_mult_bitmap_allocation_TEST.eg_mc_group_from_user_1");
  if(eg_mc_group_from_user_1 == NULL) {
    pass = FALSE;
    goto exit;
  }

  for (i = 0 ; i < mc_group_size_1 ; i++)
  {
    soc_petra_PETRA_MULT_EG_ENTRY_clear(&eg_mc_group_from_user_1[i]);
  }

  soc_petra_mult_tests_create_eg_multicast_group(
    mc_group_size_1,
    eg_mc_group_from_user_1
  );

  if (!silent)
  {
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
    soc_sand_os_printf("The group from User:\n\r");

    soc_petra_multicast_eg_group_print(
      eg_mc_group_from_user_1,
      mc_group_size_1
    );
    soc_sand_os_printf("-------------------------------------------------\n\r");
  }

  /* } */

  /* Opening a new group */
  driver_ret = soc_petra_mult_eg_group_open(
                 unit,
                 mc_group_id_1,
                 eg_mc_group_from_user_1,
                 mc_group_size_1,
                 &insufficient_memory
               );
  if(soc_sand_get_error_code_from_error_word(driver_ret) != SOC_SAND_OK)
  {
    goto exit;
  }

  /* allocate mc-group before sending to get-from-hw function */
  mc_group_from_hw_1 = sal_alloc(sizeof(SOC_PETRA_MULT_EG_ENTRY) * mc_group_size_1,
    "soc_petra_mult_bitmap_allocation_TEST.mc_group_from_hw_1");
  if(mc_group_from_hw_1 == NULL) {
    pass = FALSE;
    goto exit;
  }

  list_of_next_entries_1 = sal_alloc(sizeof(uint16) * mc_group_size_1,
    "soc_petra_mult_bitmap_allocation_TEST.list_of_next_entries_1");
  if(list_of_next_entries_1 == NULL) {
    pass = FALSE;
    goto exit;
  }

  for (i = 0 ; i < mc_group_size_1 ; i++)
  {
    soc_petra_PETRA_MULT_EG_ENTRY_clear(&mc_group_from_hw_1[i]);
  }
  /* get-from-hw */

  driver_ret = soc_petra_mult_tests_get_eg_mc_group_from_hw(
                 unit,
                 mc_group_id_1,
                 mc_group_from_hw_1,
                 &mc_group_from_hw_size_exact_1,
                 list_of_next_entries_1
               );

  if(soc_sand_get_error_code_from_error_word(driver_ret) != SOC_SAND_OK)
  {
    goto exit;
  }

  if (!silent)
  {
    /*Print group from HW*/
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
    soc_sand_os_printf("The group from HW:\n\r");

    soc_petra_multicast_eg_group_print(
      mc_group_from_hw_1,
      mc_group_size_1
    );
    soc_sand_os_printf("-------------------------------------------------\n\r");
  }


  /*
   * 2nd - Bitmap Multicast Group
   */

  /*
   * Egress bitmap mc-group - define and open
   */
  /* { */
  bitmap_mc_group_id_2 = SOC_PETRA_MULT_TEST_9_EG_MC_BITMAP_ID;

 soc_petra_mult_tests_create_eg_vlan_membership_multicast_group(
    &bitmap_mc_group
  );

  if (!silent)
  {
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
    soc_sand_os_printf("The BITMAP group from User:\n\r");

    soc_petra_multicast_eg_VLAN_MEMBERSHIP_group_print(
      &bitmap_mc_group
    );
    soc_sand_os_printf("-------------------------------------------------\n\r");
  }

  /* } */


  /* Opening a new group */
  driver_ret = soc_petra_mult_eg_vlan_membership_group_open(
                 unit,
                 bitmap_mc_group_id_2,
                 &bitmap_mc_group
               );
  if(soc_sand_get_error_code_from_error_word(driver_ret) != SOC_SAND_OK)
  {
    goto exit;
  }

  /* get-from-hw */
  driver_ret = soc_petra_mult_eg_vlan_membership_group_get(
                 unit,
                 bitmap_mc_group_id_2,
                 &bitmap_mc_group_from_hw
               );

  if(soc_sand_get_error_code_from_error_word(driver_ret) != SOC_SAND_OK)
  {
    goto exit;
  }

  if (!silent)
  {
    /*Print group from HW*/
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
    soc_sand_os_printf("The group from HW:\n\r");

    soc_petra_multicast_eg_VLAN_MEMBERSHIP_group_print(
      &bitmap_mc_group_from_hw
    );
    soc_sand_os_printf("-------------------------------------------------\n\r");
  }

  if (pass)
  {
    soc_sand_os_printf("\n\r* soc_petra_mult_bitmap_allocation_TEST : Test PASSED\n\r");
  }

exit:
  if (!silent)
  {
    soc_sand_os_printf("\n\r ********** soc_petra_mult_bitmap_allocation_TEST : END **********\n\r");
  }

  SOC_PETRA_FREE(eg_mc_group_from_user_1);
  SOC_PETRA_FREE(mc_group_from_hw_1);
  SOC_PETRA_FREE(list_of_next_entries_1);

  return pass;
}


/*****************************************************
*NAME
*  soc_petra_mult_full_table_TEST_1
*TYPE:
*  PROC
*DATE:
*  15-Feb-2008
*FUNCTION:
* Verification that opening new MC-Groups will fail when table is full.
* Small amount of LARGE groups:
* 1.  Open a small amount of LARGE groups (4*4K Ingress) and (4*4K Egress) MC-Groups.
* 2.  Then try and open an Ingress MC-Group with one entry (MC-ID = Don't Care).
*     Expect INSUFFICIENT MEMORY value to be TRUE.
* 3.  Close Egress Group (MC-ID = 0).
* 4.  Open an Ingress MC-Group with one entry (MC-ID = 1). Expect Success.
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
  soc_petra_mult_full_table_TEST_1(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 silent
  )
{
  SOC_PETRA_MULT_ING_ENTRY
    *ing_mc_group_from_user_1 = NULL,
    *ing_mc_group_from_user_3 = NULL;
  SOC_PETRA_MULT_EG_ENTRY
    *eg_mc_group_from_user_2 = NULL;
  int32
    pass = TRUE,
    i,
    ing_mc_group_size_1,
    eg_mc_group_size_2,
    ing_mc_group_size_3;
  uint8
    insufficient_memory = FALSE;
  SOC_TMC_ERROR
    mc_err;
  uint32
    indx;
  uint16
    ing_mc_group_id_1,
    eg_mc_group_id_2,
    ing_mc_group_id_3;
  uint32
    driver_ret;

  if (!silent)
  {
    soc_sand_os_printf("\n\r ********** soc_petra_mult_full_table_TEST_1 : START **********\n\r");
  }


  /*
   * Opening Ingress 4k groups
   */
  /*
   * Defining and Setting a new group
   * type = Ingress
   * size = 8k (2*4k)
   * mc_id = 0-3
   */
  /* { */
  ing_mc_group_size_1 = SOC_PETRA_MULT_TEST_10_ING_GROUP_SIZE_1;
  /*ing_mc_group_id_1 = 0;*/
  
  ing_mc_group_from_user_1 = sal_alloc(sizeof(SOC_PETRA_MULT_ING_ENTRY) * ing_mc_group_size_1,
    "soc_petra_mult_full_table_TEST_1.ing_mc_group_from_user_1");
  if(ing_mc_group_from_user_1 == NULL) {
    pass = FALSE;
    goto exit;
  }

  
  for (i = 0 ; i < ing_mc_group_size_1 ; i++)
  {
    soc_petra_PETRA_MULT_ING_ENTRY_clear(&ing_mc_group_from_user_1[i]);
  }
  
  
  soc_petra_mult_tests_create_ing_multicast_group(
    ing_mc_group_size_1,
    ing_mc_group_from_user_1
  );

  
  if (!silent)
  {
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
    soc_sand_os_printf("Opening 4 times a 4k INGRESS group:\n\r");
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
  }

  for (indx = 0 ; indx < 4; indx++)
  {
    ing_mc_group_id_1 = (uint16)indx;

    /* Opening a new group */
    driver_ret = soc_petra_mult_ing_group_open(
                   unit,
                   ing_mc_group_id_1,
                   ing_mc_group_from_user_1,
                   ing_mc_group_size_1,
                   &mc_err
                 );
    if(soc_sand_get_error_code_from_error_word(driver_ret) != SOC_SAND_OK)
    {
      goto exit;
    }
  }

  /*
   * Opening Egress 4k groups
   */
  /*
   * Defining and Setting a new group
   * type = Egress
   * size = 16k (4*4k)
   * mc_id = 16K-1 - 0/1/2/3
   */
  /* { */
  eg_mc_group_size_2 = SOC_PETRA_MULT_TEST_10_EG_GROUP_SIZE_2;
  
  eg_mc_group_from_user_2 = sal_alloc(sizeof(SOC_PETRA_MULT_EG_ENTRY) * eg_mc_group_size_2,
    "soc_petra_mult_full_table_TEST_1.eg_mc_group_from_user_2");
  if(eg_mc_group_from_user_2 == NULL) {
    pass = FALSE;
    goto exit;
  }

  for (i = 0 ; i < eg_mc_group_size_2 ; i++)
  {
    soc_petra_PETRA_MULT_EG_ENTRY_clear(&eg_mc_group_from_user_2[i]);
  }

  soc_petra_mult_tests_create_eg_multicast_group(
    eg_mc_group_size_2,
    eg_mc_group_from_user_2
  );

  if (!silent)
  {
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
    soc_sand_os_printf("Opening 4 times a 4k EGRESS group:\n\r");
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
  }

  for (indx = 0 ; indx < 4; indx++)
  {
     
    eg_mc_group_id_2 = (uint16)((SOC_PETRA_MULT_TEST_10_EG_GROUP_START_ID)-indx);

    /* Opening a new group */
    driver_ret = soc_petra_mult_eg_group_open(
                   unit,
                   eg_mc_group_id_2,
                   eg_mc_group_from_user_2,
                   eg_mc_group_size_2,
                   &insufficient_memory
                 );
    if(soc_sand_get_error_code_from_error_word(driver_ret) != SOC_SAND_OK)
    {
      goto exit;
    }
  }

  /*
   * Opening Ingress group - 1 entry
   */
  /*
   * Defining and Setting a new group
   * type = Ingress
   * size = 1
   * mc_id = 100
   */
  /* { */
  ing_mc_group_size_3 = SOC_PETRA_MULT_TEST_10_ING_GROUP_SIZE_3;
  ing_mc_group_id_3 = SOC_PETRA_MULT_TEST_10_ING_MC_GROUP_ID_3;

  ing_mc_group_from_user_3 = sal_alloc(sizeof(SOC_PETRA_MULT_ING_ENTRY) * ing_mc_group_size_3,
    "soc_petra_mult_full_table_TEST_1.ing_mc_group_from_user_3");
  if(ing_mc_group_from_user_3 == NULL) {
    pass = FALSE;
    goto exit;
  }

  for (i = 0 ; i < ing_mc_group_size_3 ; i++)
  {
    soc_petra_PETRA_MULT_ING_ENTRY_clear(&ing_mc_group_from_user_3[i]);
  }

  soc_petra_mult_tests_create_ing_multicast_group(
    ing_mc_group_size_3,
    ing_mc_group_from_user_3
  );

  if (!silent)
  {
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
    soc_sand_os_printf("Opening an INGRESS group (1 entry):\n\r");
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
  }

  /* Opening a new group */
  driver_ret = soc_petra_mult_ing_group_open(
                 unit,
                 ing_mc_group_id_3,
                 ing_mc_group_from_user_3,
                 ing_mc_group_size_3,
                 &mc_err
               );
  if(soc_sand_get_error_code_from_error_word(driver_ret) != SOC_SAND_OK)
  {
    goto exit;
  }
  if (mc_err != _SHR_E_FULL)
  {
    if (!silent)
    {
      soc_sand_os_printf("\n\rWhen trying to open a new group -"
                          "there is sufficient memory\n\r");
    }
    pass = FALSE;
    soc_sand_os_printf("\n\r soc_petra_mult_full_table_TEST_1 : Test FAILED\n\r");
    goto exit;
  }

  /* Close an egress group MC-ID=SOC_PETRA_MULT_TEST_10_EG_GROUP_START_ID*/
  eg_mc_group_id_2 = SOC_PETRA_MULT_TEST_10_EG_GROUP_START_ID;

  driver_ret = soc_petra_mult_eg_group_close(
                 unit,
                 eg_mc_group_id_2
               );
  if(soc_sand_get_error_code_from_error_word(driver_ret) != SOC_SAND_OK)
  {
    goto exit;
  }

  /* Try again to Open new group */
  driver_ret = soc_petra_mult_ing_group_open(
                 unit,
                 ing_mc_group_id_3,
                 ing_mc_group_from_user_3,
                 ing_mc_group_size_3,
                 &mc_err
               );
  if(soc_sand_get_error_code_from_error_word(driver_ret) != SOC_SAND_OK)
  {
    goto exit;
  }
  if (mc_err == _SHR_E_FULL)
  {
    if (!silent)
    {
      soc_sand_os_printf("\n\rWhen trying to open the new group again (after deleting)-"
        "there isn't sufficient memory\n\r");
    }
    pass = FALSE;
    soc_sand_os_printf("\n\r soc_petra_mult_full_table_TEST_1 : Test FAILED\n\r");
    goto exit;
  }

  if (pass)
  {
    soc_sand_os_printf("\n\r* soc_petra_mult_full_table_TEST_1 : Test PASSED\n\r");
  }
exit:
  if (!silent)
  {
    soc_sand_os_printf("\n\r ********** soc_petra_mult_full_table_TEST_1 : END **********\n\r");
  }

  SOC_PETRA_FREE(ing_mc_group_from_user_1);
  SOC_PETRA_FREE(ing_mc_group_from_user_3);
  SOC_PETRA_FREE(eg_mc_group_from_user_2);
  return pass;
}



/*****************************************************
*NAME
*  soc_petra_mult_full_set_and_get_start_from_mid_1
*TYPE:
*  PROC
*DATE:
*  15-Feb-2008
*FUNCTION:
* Verification that opening new MC-Groups will fail when table is full.
* Large amount of SMALL groups:
* 1.  Open a large amount of SMALL groups (4K*4 Ingress) and (4K*4 Egress) MC-Groups.
* 2.  Then try and open an Ingress MC-Group with one entry (MC-ID = Don't Care). Expect INSUFFICIENT MEMORY value to be TRUE.
* 2a. Validate that groups are as expected by getting all the groups.
* 3.  Close Egress Group (MC-ID = 0). Close Ingress Group (MC-ID = 16K-4).
* 4.  Open an Ingress MC-Group with 8 entries (MC-ID = 1). Expect Success.

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
  soc_petra_mult_full_set_and_get_start_from_mid_1(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 silent
  )
{
  SOC_PETRA_MULT_ING_ENTRY
    *ing_mc_group_from_user_1 = NULL,
    *ing_mc_group_actual_1 = NULL;
  SOC_PETRA_MULT_EG_ENTRY
    *eg_mc_group_from_user_2 = NULL,
    *eg_mc_group_actual_2 = NULL;
  int32
    pass = TRUE;
  uint32
    i,
    ing_mc_group_size_1,
    eg_mc_group_size_2,
    ing_mc_group_size_actual_1=0,
    eg_mc_group_size_actual_2=0;
  uint8
    is_opened,
    insufficient_memory = FALSE;
  SOC_TMC_ERROR
    mc_err;
  uint32
    indx;
  uint16
    ing_mc_group_id_1,
    eg_mc_group_id_2;
  uint32
    driver_ret;

  if (!silent)
  {
    soc_sand_os_printf("\n\r ********** soc_petra_mult_full_set_and_get_start_from_mid_1 : START **********\n\r");
  }


  /*
   * Opening Ingress 4k groups
   */
  /*
   * Defining and Setting a new group
   * type = Ingress
   * size = 8 (2*4)
   * mc_id = 0-(4k-1)
   */
  /* { */
  ing_mc_group_size_1 = SOC_PETRA_MULT_TEST_11_ING_GROUP_SIZE_1;
  /*ing_mc_group_id_1 = 0;*/

  ing_mc_group_from_user_1 = sal_alloc(sizeof(SOC_PETRA_MULT_ING_ENTRY) * ing_mc_group_size_1,
    "soc_petra_mult_full_set_and_get_start_from_mid_1.ing_mc_group_from_user_1");
  if(ing_mc_group_from_user_1 == NULL) {
    pass = FALSE;
    goto exit;
  }

  ing_mc_group_actual_1 = sal_alloc(sizeof(SOC_PETRA_MULT_ING_ENTRY) * ing_mc_group_size_1,
    "soc_petra_mult_full_set_and_get_start_from_mid_1.ing_mc_group_actual_1");
  if(ing_mc_group_actual_1 == NULL) {
    pass = FALSE;
    goto exit;
  }

  for (i = 0 ; i < ing_mc_group_size_1 ; i++)
  {
    soc_petra_PETRA_MULT_ING_ENTRY_clear(&ing_mc_group_from_user_1[i]);
  }

  soc_petra_mult_tests_create_ing_multicast_group(
    ing_mc_group_size_1,
    ing_mc_group_from_user_1
  );

  if (!silent)
  {
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
    soc_sand_os_printf("Opening 4k times an INGRESS group (with 4 entries):\n\r");
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
  }

  for (indx = 1 ; indx < 4097 ; indx++)
  {
    ing_mc_group_id_1 = (uint16)indx;

    /* Opening a new group */
    driver_ret = soc_petra_mult_ing_group_open(
                   unit,
                   ing_mc_group_id_1,
                   ing_mc_group_from_user_1,
                   ing_mc_group_size_1,
                   &mc_err
                 );
    if(soc_sand_get_error_code_from_error_word(driver_ret) != SOC_SAND_OK)
    {
      goto exit;
    }
  }

  /*
   * Opening Egress 4k groups
   */
  /*
   * Defining and Setting a new group
   * type = Egress
   * size = 12 (3*4)
   * mc_id = 0/1/2/3
   */
  /* { */
  eg_mc_group_size_2 = SOC_PETRA_MULT_TEST_11_EG_GROUP_SIZE_2;
  /*eg_mc_group_id_2 = 0;*/

  eg_mc_group_from_user_2 = sal_alloc(sizeof(SOC_PETRA_MULT_EG_ENTRY) * eg_mc_group_size_2,
    "soc_petra_mult_full_set_and_get_start_from_mid_1.eg_mc_group_from_user_2");
  if(eg_mc_group_from_user_2 == NULL) {
    pass = FALSE;
    goto exit;
  }

  eg_mc_group_actual_2 = sal_alloc(sizeof(SOC_PETRA_MULT_EG_ENTRY) * eg_mc_group_size_2,
    "soc_petra_mult_full_set_and_get_start_from_mid_1.eg_mc_group_actual_2");
  if(eg_mc_group_actual_2 == NULL) {
    pass = FALSE;
    goto exit;
  }

  for (i = 0 ; i < eg_mc_group_size_2 ; i++)
  {
    soc_petra_PETRA_MULT_EG_ENTRY_clear(&eg_mc_group_from_user_2[i]);
  }

  soc_petra_mult_tests_create_eg_multicast_group(
    eg_mc_group_size_2,
    eg_mc_group_from_user_2
  );

  if (!silent)
  {
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
    soc_sand_os_printf("Opening 4k times an EGRESS group (with 4 entries): \n\r");
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
  }

  for (indx = 1 ; indx < 4097 ; indx++)
  {
    eg_mc_group_id_2 = (uint16)indx;

    /* Opening a new group */
    driver_ret = soc_petra_mult_eg_group_open(
                   unit,
                   eg_mc_group_id_2,
                   eg_mc_group_from_user_2,
                   eg_mc_group_size_2,
                   &insufficient_memory
                 );
    if(soc_sand_get_error_code_from_error_word(driver_ret) != SOC_SAND_OK)
    {
      goto exit;
    }
    if(insufficient_memory && (eg_mc_group_id_2 < 4096))
    {
      soc_sand_os_printf("\n\r Not planned insufficient_memory\n\r");
      pass = FALSE;
      soc_sand_os_printf("\n\r soc_petra_mult_full_set_and_get_start_from_mid_1 : Test FAILED\n\r");
      goto exit;
    }
    if(!insufficient_memory && (eg_mc_group_id_2 == 4096))
    {
      soc_sand_os_printf("\n\r Expected insufficient_memory due to entry 0 status\n\r");
      pass = FALSE;
      soc_sand_os_printf("\n\r soc_petra_mult_full_set_and_get_start_from_mid_1 : Test FAILED\n\r");
      goto exit;
    }
  }

  for (indx = 1 ; indx < 4096 ; indx++)
  {
    eg_mc_group_id_2 = (uint16)indx;

    /* Get the groups */
    driver_ret = soc_petra_mult_eg_group_get_unsafe(
      unit,
      eg_mc_group_id_2,
      eg_mc_group_size_2,
      eg_mc_group_actual_2,
      &eg_mc_group_size_actual_2,
      &is_opened
      );
    if(soc_sand_get_error_code_from_error_word(driver_ret) != SOC_SAND_OK)
    {
      goto exit;
    }
    if(!is_opened)
    {
      soc_sand_os_printf("\n\r Group was not opened\n\r");
      pass = FALSE;
      soc_sand_os_printf("\n\r soc_petra_mult_full_set_and_get_start_from_mid_1 : Test FAILED\n\r");
      goto exit;
    }
    else if(eg_mc_group_size_2 != eg_mc_group_size_actual_2)
    {
      soc_sand_os_printf("\n\r Invalid group size\n\r");
      pass = FALSE;
      soc_sand_os_printf("\n\r soc_petra_mult_full_set_and_get_start_from_mid_1 : Test FAILED\n\r");
      goto exit;
    }
    else
    {
      for(i=0;i<eg_mc_group_size_2;i++)
      {
        if( (eg_mc_group_from_user_2[i].cud != eg_mc_group_actual_2[i].cud) ||
            (eg_mc_group_from_user_2[i].port != eg_mc_group_actual_2[i].port)
          )
        {
          soc_sand_os_printf("\n\r Invalid group members\n\r");
          pass = FALSE;
          soc_sand_os_printf("\n\r soc_petra_mult_full_set_and_get_start_from_mid_1 : Test FAILED\n\r");
          goto exit;
        }
      }

    }
  }

  for (indx = 1 ; indx < 4097 ; indx++)
  {
    ing_mc_group_id_1 = (uint16)indx;

    /* Get the groups */
    driver_ret = soc_petra_mult_ing_group_get_unsafe(
      unit,
      ing_mc_group_id_1,
      ing_mc_group_size_1,
      ing_mc_group_actual_1,
      &ing_mc_group_size_actual_1,
      &is_opened
      );
    if(soc_sand_get_error_code_from_error_word(driver_ret) != SOC_SAND_OK)
    {
      goto exit;
    }
    if(!is_opened)
    {
      soc_sand_os_printf("\n\r Group was not opened\n\r");
      pass = FALSE;
      soc_sand_os_printf("\n\r soc_petra_mult_full_set_and_get_start_from_mid_1 : Test FAILED\n\r");
      goto exit;
    }
    else if(ing_mc_group_size_1 != ing_mc_group_size_actual_1)
    {
      soc_sand_os_printf("\n\r Invalid group size\n\r");
      pass = FALSE;
      soc_sand_os_printf("\n\r soc_petra_mult_full_set_and_get_start_from_mid_1 : Test FAILED\n\r");
      goto exit;
    }
    else
    {
      for(i=0;i<ing_mc_group_size_1;i++)
      {
        if( (ing_mc_group_from_user_1[i].destination.type != ing_mc_group_actual_1[i].destination.type) ||
            (ing_mc_group_from_user_1[i].destination.id != ing_mc_group_actual_1[i].destination.id) ||
            (ing_mc_group_from_user_1[i].cud != ing_mc_group_actual_1[i].cud)
          )
        {
          soc_sand_os_printf("\n\r Invalid group members\n\r");
          pass = FALSE;
          soc_sand_os_printf("\n\r soc_petra_mult_full_set_and_get_start_from_mid_1 : Test FAILED\n\r");
          goto exit;
        }
      }
    }
  }
  if (pass)
  {
    soc_sand_os_printf("\n\r* soc_petra_mult_full_set_and_get_start_from_mid_1 : Test PASSED\n\r");
  }
exit:
  if (!silent)
  {
    soc_sand_os_printf("\n\r ********** soc_petra_mult_full_set_and_get_start_from_mid_1 : END **********\n\r");
  }

  SOC_PETRA_FREE(ing_mc_group_from_user_1);
  SOC_PETRA_FREE(ing_mc_group_actual_1);
  SOC_PETRA_FREE(eg_mc_group_from_user_2);
  SOC_PETRA_FREE(eg_mc_group_actual_2);

  return pass;
}

/*****************************************************
*NAME
*  soc_petra_mult_full_set_and_get
*TYPE:
*  PROC
*DATE:
*  15-Feb-2008
*FUNCTION:
* Verification that opening new MC-Groups will fail when table is full.
* Large amount of SMALL groups:
* 1.  Open a large amount of SMALL groups (4K*4 Ingress) and (4K*4 Egress) MC-Groups.
* 2.  Then try and open an Ingress MC-Group with one entry (MC-ID = Don't Care). Expect INSUFFICIENT MEMORY value to be TRUE.
* 2a. Validate that groups are as expected by getting all the groups.
* 3.  Close Egress Group (MC-ID = 0). Close Ingress Group (MC-ID = 16K-4).
* 4.  Open an Ingress MC-Group with 8 entries (MC-ID = 1). Expect Success.

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
  soc_petra_mult_full_set_and_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 silent
  )
{
  SOC_PETRA_MULT_ING_ENTRY
    *ing_mc_group_from_user_1 = NULL,
    *ing_mc_group_actual_1 = NULL,
    *ing_mc_group_from_user_3 = NULL;
  SOC_PETRA_MULT_EG_ENTRY
    *eg_mc_group_from_user_2 = NULL,
    *eg_mc_group_actual_2 = NULL;
  int32
    pass = TRUE;
  uint32
    i,
    ing_mc_group_size_1,
    eg_mc_group_size_2,
    ing_mc_group_size_actual_1=0,
    eg_mc_group_size_actual_2=0,
    ing_mc_group_size_3;
  uint8
    is_opened,
    insufficient_memory = FALSE;
  SOC_TMC_ERROR
    mc_err;
  uint32
    indx;
  uint16
    ing_mc_group_id_1,
    eg_mc_group_id_2,
    ing_mc_group_id_3;
  uint32
    driver_ret;

  if (!silent)
  {
    soc_sand_os_printf("\n\r ********** soc_petra_mult_full_set_and_get : START **********\n\r");
  }


  /*
   * Opening Ingress 4k groups
   */
  /*
   * Defining and Setting a new group
   * type = Ingress
   * size = 8 (2*4)
   * mc_id = 0-(4k-1)
   */
  /* { */
  ing_mc_group_size_1 = SOC_PETRA_MULT_TEST_11_ING_GROUP_SIZE_1;
  /*ing_mc_group_id_1 = 0;*/

  ing_mc_group_from_user_1 = sal_alloc(sizeof(SOC_PETRA_MULT_ING_ENTRY) * ing_mc_group_size_1,
    "soc_petra_mult_full_set_and_get.ing_mc_group_from_user_1");
  if(ing_mc_group_from_user_1 == NULL) {
    pass = FALSE;
    goto exit;
  }

  ing_mc_group_actual_1 = sal_alloc(sizeof(SOC_PETRA_MULT_ING_ENTRY) * ing_mc_group_size_1,
    "soc_petra_mult_full_set_and_get.ing_mc_group_actual_1");
  if(ing_mc_group_actual_1 == NULL) {
    pass = FALSE;
    goto exit;
  }

  for (i = 0 ; i < ing_mc_group_size_1 ; i++)
  {
    soc_petra_PETRA_MULT_ING_ENTRY_clear(&ing_mc_group_from_user_1[i]);
  }

  soc_petra_mult_tests_create_ing_multicast_group(
    ing_mc_group_size_1,
    ing_mc_group_from_user_1
  );

  if (!silent)
  {
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
    soc_sand_os_printf("Opening 4k times an INGRESS group (with 4 entries):\n\r");
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
  }

  for (indx = 0 ; indx < 4096 ; indx++)
  {
    ing_mc_group_id_1 = (uint16)indx;

    /* Opening a new group */
    driver_ret = soc_petra_mult_ing_group_open(
                   unit,
                   ing_mc_group_id_1,
                   ing_mc_group_from_user_1,
                   ing_mc_group_size_1,
                   &mc_err
                 );
    if(soc_sand_get_error_code_from_error_word(driver_ret) != SOC_SAND_OK)
    {
      goto exit;
    }
  }

  /*
   * Opening Egress 4k groups
   */
  /*
   * Defining and Setting a new group
   * type = Egress
   * size = 12 (3*4)
   * mc_id = 0/1/2/3
   */
  /* { */
  eg_mc_group_size_2 = SOC_PETRA_MULT_TEST_11_EG_GROUP_SIZE_2;
  /*eg_mc_group_id_2 = 0;*/

  eg_mc_group_from_user_2 = sal_alloc(sizeof(SOC_PETRA_MULT_EG_ENTRY) * eg_mc_group_size_2,
    "soc_petra_mult_full_set_and_get.eg_mc_group_from_user_2");
  if(eg_mc_group_from_user_2 == NULL) {
    pass = FALSE;
    goto exit;
  }

  eg_mc_group_actual_2 = sal_alloc(sizeof(SOC_PETRA_MULT_EG_ENTRY) * eg_mc_group_size_2,
    "soc_petra_mult_full_set_and_get.eg_mc_group_actual_2");
  if(eg_mc_group_actual_2 == NULL) {
    pass = FALSE;
    goto exit;
  }

  for (i = 0 ; i < eg_mc_group_size_2 ; i++)
  {
    soc_petra_PETRA_MULT_EG_ENTRY_clear(&eg_mc_group_from_user_2[i]);
  }

  soc_petra_mult_tests_create_eg_multicast_group(
    eg_mc_group_size_2,
    eg_mc_group_from_user_2
  );

  if (!silent)
  {
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
    soc_sand_os_printf("Opening 4k-1 times an EGRESS group (with 4 entries): \n\r");
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
  }

  for (indx = 0 ; indx < 4095 ; indx++)
  {
    eg_mc_group_id_2 = (uint16)indx;

    /* Opening a new group */
    driver_ret = soc_petra_mult_eg_group_open(
                   unit,
                   eg_mc_group_id_2,
                   eg_mc_group_from_user_2,
                   eg_mc_group_size_2,
                   &insufficient_memory
                 );
    if(soc_sand_get_error_code_from_error_word(driver_ret) != SOC_SAND_OK)
    {
      goto exit;
    }
  }

  for (indx = 0 ; indx < 4095 ; indx++)
  {
    eg_mc_group_id_2 = (uint16)indx;

    /* Get the groups */
    driver_ret = soc_petra_mult_eg_group_get_unsafe(
      unit,
      eg_mc_group_id_2,
      eg_mc_group_size_2,
      eg_mc_group_actual_2,
      &eg_mc_group_size_actual_2,
      &is_opened
      );
    if(soc_sand_get_error_code_from_error_word(driver_ret) != SOC_SAND_OK)
    {
      goto exit;
    }
    if(!is_opened)
    {
      soc_sand_os_printf("\n\r Group was not opened\n\r");
      pass = FALSE;
      soc_sand_os_printf("\n\r soc_petra_mult_full_set_and_get : Test FAILED\n\r");
      goto exit;
    }
    else if(eg_mc_group_size_2 != eg_mc_group_size_actual_2)
    {
      soc_sand_os_printf("\n\r Invalid group size\n\r");
      pass = FALSE;
      soc_sand_os_printf("\n\r soc_petra_mult_full_set_and_get : Test FAILED\n\r");
      goto exit;
    }
    else
    {
      for(i=0;i<eg_mc_group_size_2;i++)
      {
        if( (eg_mc_group_from_user_2[i].cud != eg_mc_group_actual_2[i].cud) ||
            (eg_mc_group_from_user_2[i].port != eg_mc_group_actual_2[i].port)
          )
        {
          soc_sand_os_printf("\n\r Invalid group members\n\r");
          pass = FALSE;
          soc_sand_os_printf("\n\r soc_petra_mult_full_set_and_get : Test FAILED\n\r");
          goto exit;
        }
      }

    }
  }

  for (indx = 0 ; indx < 4096 ; indx++)
  {
    ing_mc_group_id_1 = (uint16)indx;

    /* Get the groups */
    driver_ret = soc_petra_mult_ing_group_get_unsafe(
      unit,
      ing_mc_group_id_1,
      ing_mc_group_size_1,
      ing_mc_group_actual_1,
      &ing_mc_group_size_actual_1,
      &is_opened
      );
    if(soc_sand_get_error_code_from_error_word(driver_ret) != SOC_SAND_OK)
    {
      goto exit;
    }
    if(!is_opened)
    {
      soc_sand_os_printf("\n\r Group was not opened\n\r");
      pass = FALSE;
      soc_sand_os_printf("\n\r soc_petra_mult_full_set_and_get : Test FAILED\n\r");
      goto exit;
    }
    else if(ing_mc_group_size_1 != ing_mc_group_size_actual_1)
    {
      soc_sand_os_printf("\n\r Invalid group size\n\r");
      pass = FALSE;
      soc_sand_os_printf("\n\r soc_petra_mult_full_set_and_get : Test FAILED\n\r");
      goto exit;
    }
    else
    {
      for(i=0;i<ing_mc_group_size_1;i++)
      {
        if( (ing_mc_group_from_user_1[i].destination.type != ing_mc_group_actual_1[i].destination.type) ||
            (ing_mc_group_from_user_1[i].destination.id != ing_mc_group_actual_1[i].destination.id) ||
            (ing_mc_group_from_user_1[i].cud != ing_mc_group_actual_1[i].cud)
          )
        {
          soc_sand_os_printf("\n\r Invalid group members\n\r");
          pass = FALSE;
          soc_sand_os_printf("\n\r soc_petra_mult_full_set_and_get : Test FAILED\n\r");
          goto exit;
        }
      }
    }
  }
  /*
   * Opening Ingress group - 1 entry
   */
  /*
   * Defining and Setting a new group
   * type = Ingress
   * size = 1
   * mc_id = 5000
   */
  /* { */
  ing_mc_group_size_3 = SOC_PETRA_MULT_TEST_11_ING_GROUP_SIZE_3;
  ing_mc_group_id_3 = SOC_PETRA_MULT_TEST_11_ING_MC_GROUP_ID_3;

  ing_mc_group_from_user_3 = sal_alloc(sizeof(SOC_PETRA_MULT_ING_ENTRY) * ing_mc_group_size_3,
    "soc_petra_mult_full_set_and_get.ing_mc_group_from_user_3");
  if(ing_mc_group_from_user_3 == NULL) {
    pass = FALSE;
    goto exit;
  }

  for (i = 0 ; i < ing_mc_group_size_3 ; i++)
  {
    soc_petra_PETRA_MULT_ING_ENTRY_clear(&ing_mc_group_from_user_3[i]);
  }

  soc_petra_mult_tests_create_ing_multicast_group(
    ing_mc_group_size_3,
    ing_mc_group_from_user_3
  );

  if (!silent)
  {
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
    soc_sand_os_printf("Opening an INGRESS group (1 entry):\n\r");
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
  }

  /* Opening a new group */
  driver_ret = soc_petra_mult_ing_group_open(
                 unit,
                 ing_mc_group_id_3,
                 ing_mc_group_from_user_3,
                 ing_mc_group_size_3,
                 &mc_err
               );
  if(soc_sand_get_error_code_from_error_word(driver_ret) != SOC_SAND_OK)
  {
    goto exit;
  }
	if (mc_err == _SHR_E_FULL)
	{
	  if (!silent)
	  {
		soc_sand_os_printf("\n\rpetra_mult_full_table_TEST_2 : "
		  "\n\rWhen trying to open a new group -"
		  "there is sufficient memory\n\r");
	  }
	  pass = FALSE;
	  soc_sand_os_printf("\n\r soc_petra_mult_full_set_and_get : Test FAILED\n\r");
	  goto exit;
	}

  /* Close an egress group MC-ID=0*/
  eg_mc_group_id_2 = 0;

  driver_ret = soc_petra_mult_eg_group_close(
                 unit,
                 eg_mc_group_id_2
               );
  if(soc_sand_get_error_code_from_error_word(driver_ret) != SOC_SAND_OK)
  {
    goto exit;
  }

  /* Try again to Open new group */
  driver_ret = soc_petra_mult_ing_group_open(
                 unit,
                 ing_mc_group_id_3,
                 ing_mc_group_from_user_3,
                 ing_mc_group_size_3,
                 &mc_err
               );
  if(soc_sand_get_error_code_from_error_word(driver_ret) != SOC_SAND_OK)
  {
    goto exit;
  }
  if (mc_err == _SHR_E_FULL)
  {
    if (!silent)
    {
      soc_sand_os_printf("\n\rpetra_mult_full_table_TEST_2 : "
                     "\n\rWhen trying to open the new group again (after deleting)-"
                     "there isn't sufficient memory\n\r");
    }
    pass = FALSE;
    soc_sand_os_printf("\n\r soc_petra_mult_full_set_and_get : Test FAILED\n\r");
    goto exit;
  }

  if (pass)
  {
    soc_sand_os_printf("\n\r* soc_petra_mult_full_set_and_get : Test PASSED\n\r");
  }
exit:
  if (!silent)
  {
    soc_sand_os_printf("\n\r ********** soc_petra_mult_full_set_and_get : END **********\n\r");
  }

  SOC_PETRA_FREE(ing_mc_group_from_user_1);
  SOC_PETRA_FREE(ing_mc_group_actual_1);
  SOC_PETRA_FREE(ing_mc_group_from_user_3);
  SOC_PETRA_FREE(eg_mc_group_from_user_2);
  SOC_PETRA_FREE(eg_mc_group_actual_2);

  return pass;
}

/*****************************************************
*NAME
*  soc_petra_mult_full_table_TEST_3
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
  soc_petra_mult_full_table_TEST_3(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 silent
  )
{
  SOC_PETRA_MULT_ING_ENTRY
    *ing_mc_group_from_user_1 = NULL,
    *ing_mc_group_from_user_4 = NULL;
  SOC_PETRA_MULT_EG_ENTRY
    *eg_mc_group_from_user_2 = NULL,
    *eg_mc_group_from_user_3 = NULL;
  int32
    pass = TRUE,
    i,
    ing_mc_group_size_1,
    eg_mc_group_size_2,
    eg_mc_group_size_3,
    ing_mc_group_size_4;
  uint8
    insufficient_memory = FALSE;
  SOC_TMC_ERROR
    mc_err;
  uint32
    indx;
  uint16
    ing_mc_group_id_1,
    eg_mc_group_id_2,
    eg_mc_group_id_3,
    ing_mc_group_id_4;
  uint32
    driver_ret;

  if (!silent)
  {
    soc_sand_os_printf("\n\r ********** soc_petra_mult_full_table_TEST_3 : START **********\n\r");
  }

  /*
   * Opening Ingress 2k groups
   */
  /*
   * Defining and Setting a new group
   * type = Ingress
   * size = 4k (2*2k)
   * mc_id = 0-7
   */
  /* { */
  ing_mc_group_size_1 = SOC_PETRA_MULT_TEST_12_ING_GROUP_SIZE_1;
  /*ing_mc_group_id_1 = 0;*/

  ing_mc_group_from_user_1 = sal_alloc(sizeof(SOC_PETRA_MULT_ING_ENTRY) * ing_mc_group_size_1,
    "soc_petra_mult_full_table_TEST_3.ing_mc_group_from_user_1");
  if(ing_mc_group_from_user_1 == NULL) {
    pass = FALSE;
    goto exit;
  }

  for (i = 0 ; i < ing_mc_group_size_1 ; i++)
  {
    soc_petra_PETRA_MULT_ING_ENTRY_clear(&ing_mc_group_from_user_1[i]);
  }

  soc_petra_mult_tests_create_ing_multicast_group(
    ing_mc_group_size_1,
    ing_mc_group_from_user_1
  );

  if (!silent)
  {
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
    soc_sand_os_printf("Opening 8 times a 2k INGRESS group:\n\r");
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
  }

  for (indx = 0 ; indx < 8; indx++)
  {
    ing_mc_group_id_1 = (uint16)indx;
    
    /* Opening a new group */
    driver_ret = soc_petra_mult_ing_group_open(
                   unit,
                   ing_mc_group_id_1,
                   ing_mc_group_from_user_1,
                   ing_mc_group_size_1,
                   &mc_err
                 );
    if(soc_sand_get_error_code_from_error_word(driver_ret) != SOC_SAND_OK)
    {
      goto exit;
    }
  }

  /*
   * Opening Egress 2k groups
   */
  /*
   * Defining and Setting a new group
   * type = Egress
   * size = 6k (3*2k)
   * mc_id = 0/1/2/3
   */
  /* { */
  eg_mc_group_size_2 = SOC_PETRA_MULT_TEST_12_EG_GROUP_SIZE_2;
  /*eg_mc_group_id_2 = 0;*/

  eg_mc_group_from_user_2 = sal_alloc(sizeof(SOC_PETRA_MULT_EG_ENTRY) * eg_mc_group_size_2,
    "soc_petra_mult_full_table_TEST_3.eg_mc_group_from_user_2");
  if(eg_mc_group_from_user_2 == NULL) {
    pass = FALSE;
    goto exit;
  }

  for (i = 0 ; i < eg_mc_group_size_2 ; i++)
  {
    soc_petra_PETRA_MULT_EG_ENTRY_clear(&eg_mc_group_from_user_2[i]);
  }

  soc_petra_mult_tests_create_eg_multicast_group(
    eg_mc_group_size_2,
    eg_mc_group_from_user_2
  );

  if (!silent)
  {
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
    soc_sand_os_printf("Opening 7 times a 2k EGRESS group:\n\r");
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
  }

  for (indx = 0 ; indx < 7; indx++)
  {
    eg_mc_group_id_2 = (uint16)((SOC_PETRA_MULT_TEST_12_EG_GROUP_START_ID)-indx);

    /* Opening a new group */
    driver_ret = soc_petra_mult_eg_group_open(
                   unit,
                   eg_mc_group_id_2,
                   eg_mc_group_from_user_2,
                   eg_mc_group_size_2,
                   &insufficient_memory
                 );
    if(soc_sand_get_error_code_from_error_word(driver_ret) != SOC_SAND_OK)
    {
      goto exit;
    }
  }

  /*
   * Opening one more Egress group - 1 entry
   */
  /*
   * Defining and Setting a new group
   * type = Egress
   * size = 1
   * mc_id = 5000
   */
  /* { */
  eg_mc_group_size_3 = SOC_PETRA_MULT_TEST_12_ING_GROUP_SIZE_3;
  eg_mc_group_id_3 = SOC_PETRA_MULT_TEST_12_ING_MC_GROUP_ID_3;

  eg_mc_group_from_user_3 = sal_alloc(sizeof(SOC_PETRA_MULT_EG_ENTRY) * eg_mc_group_size_3,
    "soc_petra_mult_full_table_TEST_3.eg_mc_group_from_user_3");
  if(eg_mc_group_from_user_3 == NULL) {
    pass = FALSE;
    goto exit;
  }

  for (i = 0 ; i < eg_mc_group_size_3 ; i++)
  {
    soc_petra_PETRA_MULT_EG_ENTRY_clear(&eg_mc_group_from_user_3[i]);
  }

  soc_petra_mult_tests_create_eg_multicast_group(
    eg_mc_group_size_3,
    eg_mc_group_from_user_3
  );

  if (!silent)
  {
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
    soc_sand_os_printf("Opening one more EGRESS group (1 entry):\n\r");
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
  }

  /* Opening a new group */
  driver_ret = soc_petra_mult_eg_group_open(
                 unit,
                 eg_mc_group_id_3,
                 eg_mc_group_from_user_3,
                 eg_mc_group_size_3,
                 &insufficient_memory
               );
  if(soc_sand_get_error_code_from_error_word(driver_ret) != SOC_SAND_OK)
  {
    goto exit;
  }

  /*
   * Try to open an Ingress group - 2k entries
   */
  /*
   * Defining and Setting a new group
   * type = Ingress
   * size = 4k (2*2k)
   * mc_id = 5000
   */
  /* { */
  ing_mc_group_size_4 = SOC_PETRA_MULT_TEST_12_ING_GROUP_SIZE_4;
  ing_mc_group_id_4 = SOC_PETRA_MULT_TEST_12_ING_MC_GROUP_ID_3;

  ing_mc_group_from_user_4 = sal_alloc(sizeof(SOC_PETRA_MULT_ING_ENTRY) * ing_mc_group_size_4,
    "soc_petra_mult_full_table_TEST_3.ing_mc_group_from_user_4");
  if(ing_mc_group_from_user_4 == NULL) {
    pass = FALSE;
    goto exit;
  }

  for (i = 0 ; i < ing_mc_group_size_4 ; i++)
  {
    soc_petra_PETRA_MULT_ING_ENTRY_clear(&ing_mc_group_from_user_4[i]);
  }

  soc_petra_mult_tests_create_ing_multicast_group(
    ing_mc_group_size_4,
    ing_mc_group_from_user_4
  );

  if (!silent)
  {
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
    soc_sand_os_printf("Trying to open an INGRESS group with 2k entries:\n\r");
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
  }

  /* Opening a new group */
  driver_ret = soc_petra_mult_ing_group_open(
                 unit,
                 ing_mc_group_id_4,
                 ing_mc_group_from_user_4,
                 ing_mc_group_size_4,
                 &mc_err
               );
  if(soc_sand_get_error_code_from_error_word(driver_ret) != SOC_SAND_OK)
  {
    goto exit;
  }
  if (mc_err != _SHR_E_FULL)
  {
    if (!silent)
    {
      soc_sand_os_printf("\n\rpetra_mult_full_table_TEST_3 : "
                     "\n\rWhen trying to open a new group -"
                          "there is sufficient memory\n\r");
    }
    pass = FALSE;
    soc_sand_os_printf("\n\r soc_petra_mult_full_table_TEST_3 : Test FAILED\n\r");
    goto exit;
  }

   /* Close an egress group with 1 entry egress_MC-ID_2*/
  if (!silent)
  {
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
    soc_sand_os_printf(" Close Egress group with 1 entry:\n\r");
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
  }
  driver_ret = soc_petra_mult_eg_group_close(
                 unit,
                 eg_mc_group_id_3
               );
  if(soc_sand_get_error_code_from_error_word(driver_ret) != SOC_SAND_OK)
  {
    goto exit;
  }

  /* Try again to Open new group */
  if (!silent)
  {
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
    soc_sand_os_printf("Trying to open an INGRESS group with 2k entries:\n\r");
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
  }
  driver_ret = soc_petra_mult_ing_group_open(
                 unit,
                 ing_mc_group_id_4,
                 ing_mc_group_from_user_4,
                 ing_mc_group_size_4,
                 &mc_err
               );
  if(soc_sand_get_error_code_from_error_word(driver_ret) != SOC_SAND_OK)
  {
    goto exit;
  }
  if (mc_err == _SHR_E_FULL)
  {
    if (!silent)
    {
      soc_sand_os_printf("\n\rpetra_mult_full_table_TEST_3 : "
        "\n\rWhen trying to open the new group again (after deleting)-"
        "there isn't sufficient memory\n\r");
    }
    pass = FALSE;
    soc_sand_os_printf("\n\r soc_petra_mult_full_table_TEST_3 : Test FAILED\n\r");
    goto exit;
  }

  if (pass)
  {
    soc_sand_os_printf("\n\r* soc_petra_mult_full_table_TEST_3 : Test PASSED\n\r");
  }
exit:
  if (!silent)

  {
    soc_sand_os_printf("\n\r ********** soc_petra_mult_full_table_TEST_3 : END **********\n\r");
  }

  SOC_PETRA_FREE(ing_mc_group_from_user_1);
  SOC_PETRA_FREE(ing_mc_group_from_user_4);
  SOC_PETRA_FREE(eg_mc_group_from_user_2);
  SOC_PETRA_FREE(eg_mc_group_from_user_3);

  return pass;
}

/*****************************************************
*NAME
*  soc_petra_mult_egress_new_mode_allocation_TEST
*TYPE:
*  PROC
*DATE:
*  15-Feb-2008
*FUNCTION:
* Verification of Multicast egress new mode - tests different scenarios
* Of the new mode allocation.
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
*   This Test does not change the 'pass' variable -
*   it will always return TRUE at current implementation.
*SEE ALSO:
*****************************************************/
uint32
  soc_petra_mult_egress_new_mode_allocation_TEST(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 silent
  )
{
  SOC_PETRA_MULT_EG_ENTRY
    *eg_mc_group_from_user_1 = NULL,
    *mc_group_from_hw_1 = NULL;
  int32
    pass = TRUE,
    i,
    mc_group_size_1;
  uint8
    insufficient_memory = FALSE;
  uint32
    mc_group_from_hw_size_exact_1;
  uint16
    mc_group_id_1,
    *list_of_next_entries_1 = NULL;
  uint32
    driver_ret;
  uint32
    nof_unoccupied,
    used_entries;
  SOC_PETRA_MULT_TESTS_GROUP_DIFF
    diff;



  if (!silent)
  {
    soc_sand_os_printf("\n\r ********** soc_petra_mult_egress_new_mode_allocation_TEST : START **********\n\r");
  }

  /*
   * First Group
   */

  /*
   * Defining, Setting and Printing a new group
   * type = Egress
   * size = 7
   * mc_id = 0
   */
  /* { */
  mc_group_size_1 = SOC_PETRA_MULT_TEST_13_EG_GROUP_SIZE_1;
  mc_group_id_1 = SOC_PETRA_MULT_TEST_10_EG_GROUP_START_ID;

  eg_mc_group_from_user_1 = sal_alloc(sizeof(SOC_PETRA_MULT_EG_ENTRY) * mc_group_size_1,
    "soc_petra_mult_egress_new_mode_allocation_TEST.eg_mc_group_from_user_1");
  if(eg_mc_group_from_user_1 == NULL) {
    pass = FALSE;
    goto exit;
  }

  for (i = 0 ; i < mc_group_size_1 ; i++)
  {
    soc_petra_PETRA_MULT_EG_ENTRY_clear(&eg_mc_group_from_user_1[i]);
  }

  soc_petra_mult_tests_create_eg_multicast_group(
    mc_group_size_1-3,
    eg_mc_group_from_user_1
  );

  /* Add another 3 entries - 3 vlans */
  for (i = 0; i < 3; i++)
  {
  	eg_mc_group_from_user_1[mc_group_size_1-(3-i)].type = SOC_PETRA_MULT_EG_ENTRY_TYPE_VLAN_PTR;
  	eg_mc_group_from_user_1[mc_group_size_1-(3-i)].cud = 0xabcd;
  	eg_mc_group_from_user_1[mc_group_size_1-(3-i)].vlan_mc_id = i+1;
  	
  }


  if (!silent)
  {
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
    soc_sand_os_printf("The group from User:\n\r");

    soc_petra_multicast_eg_group_print(
      eg_mc_group_from_user_1,
      mc_group_size_1
    );
    soc_sand_os_printf("-------------------------------------------------\n\r");
  }

  /* } */

  /* Opening a new group */
  driver_ret = soc_petra_mult_eg_group_open(
                 unit,
                 mc_group_id_1,
                 eg_mc_group_from_user_1,
                 mc_group_size_1,
                 &insufficient_memory
               );
  if(soc_sand_get_error_code_from_error_word(driver_ret) != SOC_SAND_OK)
  {
    goto exit;
  }

  /* allocate mc-group before sending to get-from-hw function */
  mc_group_from_hw_1 = sal_alloc(sizeof(SOC_PETRA_MULT_EG_ENTRY) * mc_group_size_1,
    "soc_petra_mult_egress_new_mode_allocation_TEST.mc_group_from_hw_1");
  if(mc_group_from_hw_1 == NULL) {
    pass = FALSE;
    goto exit;
  }

  list_of_next_entries_1 = sal_alloc(sizeof(uint16) * mc_group_size_1,
    "soc_petra_mult_egress_new_mode_allocation_TEST.list_of_next_entries_1");
  if(list_of_next_entries_1 == NULL) {
    pass = FALSE;
    goto exit;
  }

  for (i = 0 ; i < mc_group_size_1 ; i++)
  {
    soc_petra_PETRA_MULT_EG_ENTRY_clear(&mc_group_from_hw_1[i]);
  }
  /* get-from-hw */

  driver_ret = soc_petra_mult_tests_get_eg_mc_group_new_mode_from_hw(
                 unit,
                 mc_group_id_1,
                 mc_group_from_hw_1,
                 &mc_group_from_hw_size_exact_1,
                 list_of_next_entries_1
               );

  if(soc_sand_get_error_code_from_error_word(driver_ret) != SOC_SAND_OK)
  {
    goto exit;
  }

  if (!silent)
  {
    /*Print group from HW*/
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
    soc_sand_os_printf("The group from HW:\n\r");

    soc_petra_multicast_eg_group_print(
      mc_group_from_hw_1,
      mc_group_size_1
    );
    soc_sand_os_printf("-------------------------------------------------\n\r");
  }

  /* compare size and content */
  driver_ret = soc_petra_mult_eg_groups_compare(
    eg_mc_group_from_user_1,
    mc_group_size_1,
    mc_group_from_hw_1,
    mc_group_from_hw_size_exact_1,
    &diff
    );
  if (diff != SOC_PETRA_MULT_TESTS_GROUP_DIFF_EQUAL)
  {
    if(!silent)
    {
      soc_sand_os_printf("The groups from HW is different than the group from SW\n\r");
    }
    pass = FALSE;
  }

  /* print the number of unoccupied entries */
  nof_unoccupied =
    soc_petra_sw_db_multicast_nof_unoccupied_get(unit);


  if (!silent)
  {
    /*Print SW DATABASE */
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
    soc_sand_os_printf("Software DataBase: \n\r");
    /* Number of unoccupied - should be equal to (total of entries - used)*/
    soc_sand_os_printf("\n\r"
      "# of unoccupied: %u \n\r",nof_unoccupied);

    /* Unoccupied entries list */
    /* if egress - concatenate with '1' at msb , and 3 ports per entry */
    soc_sand_os_printf("\n\r"
      "Unoccupied entries list: \n\r");
    soc_petra_mult_tests_unoccupied_list_print(
      unit,
      (uint16)(mc_group_id_1 | SOC_PETRA_MULT_LL_EGRESS_MC_ID_START_ENTRY),
      list_of_next_entries_1
      );

    /* Backwards Pointers list */
    /* if egress - concatenate with '1' at msb , and 3 ports per entry */
    soc_sand_os_printf("\n\r"
      "Backwards Pointers list: \n\r");
    soc_petra_mult_tests_backwards_list_print(
      unit,
      (uint16)(mc_group_id_1 | SOC_PETRA_MULT_LL_EGRESS_MC_ID_START_ENTRY),
      list_of_next_entries_1
      );
    soc_sand_os_printf("-------------------------------------------------\n\r");
  }

  /* check SW-DataBase: {*/
  used_entries = SOC_SAND_DIV_ROUND_UP(mc_group_size_1-4,SOC_PETRA_MULT_EG_NOF_PORTS_PER_ENTRY)
    + SOC_SAND_DIV_ROUND_UP(2,SOC_PETRA_MULT_EG_FORMAT_C_PER_ENTRY)
    + SOC_SAND_DIV_ROUND_UP(2,SOC_PETRA_MULT_EG_FORMAT_B_PER_ENTRY) ;

  if (nof_unoccupied != (SOC_PETRA_MULT_TABLE_SIZE - used_entries))
  {
    if(!silent)
    {
      soc_sand_os_printf("The Number of unoccupied entries is different than expected\n\r");
    }
    pass = FALSE;
    soc_sand_os_printf("soc_petra_mult_egress_new_mode_allocation_TEST : Test FAILED\n\r");
    goto exit;
  }

  pass = soc_petra_mult_tests_unoccupied_list_check_occupation(
    unit,
    (uint16)(mc_group_id_1 | SOC_PETRA_MULT_LL_EGRESS_MC_ID_START_ENTRY),
    list_of_next_entries_1,
    silent
    );

  if (!pass)
  {
    soc_sand_os_printf("soc_petra_mult_egress_new_mode_allocation_TEST : Test FAILED\n\r");
    goto exit;
  }

  pass = soc_petra_mult_tests_backwards_list_check_occupation(
    unit,
    (uint16)(mc_group_id_1 | SOC_PETRA_MULT_LL_EGRESS_MC_ID_START_ENTRY),
    list_of_next_entries_1,
    silent
    );

  if (!pass)
  {
    soc_sand_os_printf("soc_petra_mult_egress_new_mode_allocation_TEST : Test FAILED\n\r");
    goto exit;
  }

  if (pass)
  {
    soc_sand_os_printf("\n\r* soc_petra_mult_egress_new_mode_allocation_TEST : Test PASSED\n\r");
  }

exit:
  if (!silent)
  {
    soc_sand_os_printf("\n\r ********** soc_petra_mult_egress_new_mode_allocation_TEST : END **********\n\r");
  }

  SOC_PETRA_FREE(eg_mc_group_from_user_1);
  SOC_PETRA_FREE(mc_group_from_hw_1);
  SOC_PETRA_FREE(list_of_next_entries_1);

  return pass;
}

/*****************************************************
*NAME
*  soc_petra_mult_egress_new_mode_port_add_TEST
*TYPE:
*  PROC
*DATE:
*  15-Feb-2008
*FUNCTION:
* Verification of Multicast egress new mode - tests port adding of two scenraios:
* format B and format C.
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
*   This Test does not change the 'pass' variable -
*   it will always return TRUE at current implementation.
*SEE ALSO:
*****************************************************/
uint32
  soc_petra_mult_egress_new_mode_port_add_TEST(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 silent
  )
{
  SOC_PETRA_MULT_EG_ENTRY
   *mc_group_from_hw_1 = NULL,
   *mc_group_from_hw_2 = NULL;
  int32
    pass = TRUE,
    i,
    nof_additions,
    mc_group_size_1,
    mc_group_size_2;
  uint8
    insufficient_memory = FALSE;
  uint32
    mc_group_from_hw_size_exact_1,
    mc_group_from_hw_size_exact_2;
  uint16
    mc_group_id_1,
    mc_group_id_2,
    *list_of_next_entries_1 = NULL,
    *list_of_next_entries_2 = NULL;
  uint32
    driver_ret;
  uint32
    nof_unoccupied,
    used_entries,
    used_entries2;
  SOC_PETRA_MULT_TESTS_GROUP_DIFF
    diff;
  SOC_PETRA_MULT_EG_ENTRY
    eg_mc_entry,
    eg_mc_entry_2;


  
  if (!silent)
  {
    soc_sand_os_printf("\n\r ********** soc_petra_mult_egress_new_mode_port_add_TEST : START **********\n\r");
  }

  mc_group_id_1 = SOC_PETRA_MULT_TEST_14_EG_GROUP_START_ID;
  nof_additions = SOC_PETRA_MULT_TEST_14_NOF_ADDITIONS;
  mc_group_size_1 = SOC_PETRA_MULT_TEST_14_NOF_ADDITIONS * 2;

/*
 * ADD Elements to mc-group
 * type = Egress
 * mc_id = 0
 * element = type--> PORT#, p-id--> [2,2+nof_addition], Outlif-->0xabcd
 * element2 = type--> VLAN_PTR#, v-id--> [2,2+nof_addition], Outlif->0xabcd
 * # of elements to add: 2*nof_additions
 */
  
  soc_petra_PETRA_MULT_EG_ENTRY_clear(&eg_mc_entry);
  soc_petra_PETRA_MULT_EG_ENTRY_clear(&eg_mc_entry_2);

  eg_mc_entry.type = SOC_PETRA_MULT_EG_ENTRY_TYPE_OFP;
  eg_mc_entry.cud = 0xabcd;
  eg_mc_entry.port = 1;
  eg_mc_entry_2.type = SOC_PETRA_MULT_EG_ENTRY_TYPE_VLAN_PTR;
  eg_mc_entry_2.cud = 0xabcd;
  eg_mc_entry_2.vlan_mc_id = 1;

  if (!silent)
  {
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
    soc_sand_os_printf("The entries from User:\n\r");

    soc_sand_os_printf(
      "{%s; %02d ; 0x%X} ",
      soc_petra_PETRA_MULT_EG_ENTRY_TYPE_to_string(eg_mc_entry.type),
      eg_mc_entry.port,
      eg_mc_entry.cud
      );
    soc_sand_os_printf(
      "{%s; %02d ; 0x%X} \n\r",
      soc_petra_PETRA_MULT_EG_ENTRY_TYPE_to_string(eg_mc_entry_2.type),
      eg_mc_entry_2.vlan_mc_id,
      eg_mc_entry_2.cud
      );
    soc_sand_os_printf("-------------------------------------------------\n\r");
  }

  /* add nof_additions times */
  for (i = 0 ; i < nof_additions; i++)
  {
    eg_mc_entry.port++;
    eg_mc_entry_2.vlan_mc_id++;
    driver_ret = soc_petra_mult_eg_port_add_unsafe(
      unit,
      mc_group_id_1,
      &eg_mc_entry,
      &insufficient_memory
      );
    if(soc_sand_get_error_code_from_error_word(driver_ret) != SOC_SAND_OK)
    {
      goto exit;
    }

    driver_ret = soc_petra_mult_eg_port_add_unsafe(
      unit,
      mc_group_id_1,
      &eg_mc_entry_2,
      &insufficient_memory
      );
    if(soc_sand_get_error_code_from_error_word(driver_ret) != SOC_SAND_OK)
    {
      goto exit;
    }

  }

  mc_group_from_hw_1 = sal_alloc(sizeof(SOC_PETRA_MULT_EG_ENTRY) * mc_group_size_1,
    "soc_petra_mult_egress_new_mode_port_add_TEST.mc_group_from_hw_1");
  if(mc_group_from_hw_1 == NULL) {
    pass = FALSE;
    goto exit;
  }

  list_of_next_entries_1 = sal_alloc(sizeof(uint16) * mc_group_size_1,
    "soc_petra_mult_egress_new_mode_port_add_TEST.list_of_next_entries_1");
  if(list_of_next_entries_1 == NULL) {
    pass = FALSE;
    goto exit;
  }

  for (i = 0 ; i < mc_group_size_1 ; i++)
  {
    soc_petra_PETRA_MULT_EG_ENTRY_clear(&mc_group_from_hw_1[i]);
  }
  /* get-from-hw */

  driver_ret = soc_petra_mult_tests_get_eg_mc_group_new_mode_from_hw(
    unit,
    mc_group_id_1,
    mc_group_from_hw_1,
    &mc_group_from_hw_size_exact_1,
    list_of_next_entries_1
    );

  if(soc_sand_get_error_code_from_error_word(driver_ret) != SOC_SAND_OK)
  {
    goto exit;
  }
  
  if (!silent)
  {
    /*Print group from HW*/
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
    soc_sand_os_printf("The group from HW:\n\r");

    soc_petra_multicast_eg_group_print(
      mc_group_from_hw_1,
      mc_group_size_1
      );
    soc_sand_os_printf("-------------------------------------------------\n\r");
  }

  /* print the number of unoccupied entries */
  nof_unoccupied =
    soc_petra_sw_db_multicast_nof_unoccupied_get(unit);

  if (!silent)
  {
    /*Print SW DATABASE */
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
    soc_sand_os_printf("Software DataBase: \n\r");

    /* Number of unoccupied - should be equal to (total of entries - used)*/
    soc_sand_os_printf("\n\r"
      "# of unoccupied: %u \n\r",nof_unoccupied);

    /* Unoccupied entries list */
    soc_sand_os_printf("\n\r"
      "Unoccupied entries list \n\r");
    soc_petra_mult_tests_unoccupied_list_print(
      unit,
      mc_group_id_1,
      list_of_next_entries_1
      );

    /* Backwards Pointers list */
    soc_sand_os_printf("\n\r"
      "Backwards Pointers list \n\r");
    soc_petra_mult_tests_backwards_list_print(
      unit,
      mc_group_id_1,
      list_of_next_entries_1
      );
    soc_sand_os_printf("-------------------------------------------------\n\r");
  }
  
  /* compare size and content */
  eg_mc_entry.port = 1;
  eg_mc_entry_2.vlan_mc_id = 1;

  for (i = 0; i < nof_additions; i++)
  {
    eg_mc_entry.port++;
    eg_mc_entry_2.vlan_mc_id++;

    driver_ret = soc_petra_mult_eg_groups_compare(
      &eg_mc_entry,
      1,
      &mc_group_from_hw_1[i*2],
      1,
      &diff
      );
    if (diff != SOC_PETRA_MULT_TESTS_GROUP_DIFF_EQUAL)
    {
      if(!silent)
      {
        soc_sand_os_printf("The groups from HW is different than the group from SW\n\r");
      }
      pass = FALSE;
    }
    driver_ret = soc_petra_mult_eg_groups_compare(
      &eg_mc_entry_2,
      1,
      &mc_group_from_hw_1[i*2+1],
      1,
      &diff
      );
    if (diff != SOC_PETRA_MULT_TESTS_GROUP_DIFF_EQUAL)
    {
      if(!silent)
      {
        soc_sand_os_printf("The groups from HW is different than the group from SW\n\r");
      }
      pass = FALSE;
    }
  }
  
  /* check SW-DataBase: {*/
  used_entries = SOC_SAND_DIV_ROUND_UP(mc_group_size_1,SOC_PETRA_MULT_EG_FORMAT_B_PER_ENTRY);

  if (nof_unoccupied != (SOC_PETRA_MULT_TABLE_SIZE - used_entries))
  {
    soc_sand_os_printf("The Number of unoccupied entries is different than expected\n\r");
    pass = FALSE;
    soc_sand_os_printf("soc_petra_mult_egress_new_mode_port_add_TEST : Test FAILED\n\r");
    goto exit;
  }

  pass = soc_petra_mult_tests_unoccupied_list_check_occupation(
    unit,
    (uint16)(mc_group_id_1 | SOC_PETRA_MULT_LL_EGRESS_MC_ID_START_ENTRY),
    list_of_next_entries_1,
    silent
    );

  if (!pass)
  {
    soc_sand_os_printf("soc_petra_mult_egress_new_mode_port_add_TEST : Test FAILED\n\r");
    goto exit;
  }

  pass = soc_petra_mult_tests_backwards_list_check_occupation(
    unit,
    (uint16)(mc_group_id_1 | SOC_PETRA_MULT_LL_EGRESS_MC_ID_START_ENTRY),
    list_of_next_entries_1,
    silent
    );

  if (!pass)
  {
    soc_sand_os_printf("soc_petra_mult_egress_new_mode_port_add_TEST : Test FAILED\n\r");
    goto exit;
  }

  mc_group_id_2 = SOC_PETRA_MULT_TEST_14_EG_GROUP_START_ID_2;
  nof_additions = SOC_PETRA_MULT_TEST_14_NOF_ADDITIONS;
  mc_group_size_2 = SOC_PETRA_MULT_TEST_14_NOF_ADDITIONS;

  /*
 * ADD Elements to mc-group
 * type = Egress
 * mc_id = 1
 * element = type--> VLAN_PTR#, p-id--> [2,2+nof_addition], Outlif-->0xabcd
 * # of elements to add: nof_additions
 */
  
  soc_petra_PETRA_MULT_EG_ENTRY_clear(&eg_mc_entry);

  eg_mc_entry.type = SOC_PETRA_MULT_EG_ENTRY_TYPE_VLAN_PTR;
  eg_mc_entry.cud = 0xabcd;
  eg_mc_entry.vlan_mc_id = 1;

  if (!silent)
  {
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
    soc_sand_os_printf("The entry from User:\n\r");

    soc_sand_os_printf(
      "{%s; %02d ; 0x%X} \n\r",
      soc_petra_PETRA_MULT_EG_ENTRY_TYPE_to_string(eg_mc_entry.type),
      eg_mc_entry.vlan_mc_id,
      eg_mc_entry.cud
      );
    soc_sand_os_printf("-------------------------------------------------\n\r");
  }

  /* add nof_additions times */
  for (i = 0 ; i < nof_additions; i++)
  {
    eg_mc_entry.vlan_mc_id++;
    driver_ret = soc_petra_mult_eg_port_add_unsafe(
      unit,
      mc_group_id_2,
      &eg_mc_entry,
      &insufficient_memory
      );
    if(soc_sand_get_error_code_from_error_word(driver_ret) != SOC_SAND_OK)
    {
      goto exit;
    }

  }

  mc_group_from_hw_2 = sal_alloc(sizeof(SOC_PETRA_MULT_EG_ENTRY) * mc_group_size_2,
    "soc_petra_mult_egress_new_mode_port_add_TEST.mc_group_from_hw_2");
  if(mc_group_from_hw_2 == NULL) {
    pass = FALSE;
    goto exit;
  }

  list_of_next_entries_2 = sal_alloc(sizeof(uint16) * mc_group_size_2,
    "soc_petra_mult_egress_new_mode_port_add_TEST.list_of_next_entries_2");
  if(list_of_next_entries_2 == NULL) {
    pass = FALSE;
    goto exit;
  }

  for (i = 0 ; i < mc_group_size_2 ; i++)
  {
    soc_petra_PETRA_MULT_EG_ENTRY_clear(&mc_group_from_hw_2[i]);
  }
  /* get-from-hw */

  driver_ret = soc_petra_mult_tests_get_eg_mc_group_new_mode_from_hw(
    unit,
    mc_group_id_2,
    mc_group_from_hw_2,
    &mc_group_from_hw_size_exact_2,
    list_of_next_entries_2
    );

  if(soc_sand_get_error_code_from_error_word(driver_ret) != SOC_SAND_OK)
  {
    goto exit;
  }
  
  if (!silent)
  {
    /*Print group from HW*/
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
    soc_sand_os_printf("The group from HW:\n\r");

    soc_petra_multicast_eg_group_print(
      mc_group_from_hw_2,
      mc_group_size_2
      );
    soc_sand_os_printf("-------------------------------------------------\n\r");
  }

  /* print the number of unoccupied entries */
  nof_unoccupied =
    soc_petra_sw_db_multicast_nof_unoccupied_get(unit);

  if (!silent)
  {
    /*Print SW DATABASE */
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
    soc_sand_os_printf("Software DataBase: \n\r");

    /* Number of unoccupied - should be equal to (total of entries - used)*/
    soc_sand_os_printf("\n\r"
      "# of unoccupied: %u \n\r",nof_unoccupied);

    /* Unoccupied entries list */
    soc_sand_os_printf("\n\r"
      "Unoccupied entries list \n\r");
    soc_petra_mult_tests_unoccupied_list_print(
      unit,
      mc_group_id_2,
      list_of_next_entries_2
      );

    /* Backwards Pointers list */
    soc_sand_os_printf("\n\r"
      "Backwards Pointers list \n\r");
    soc_petra_mult_tests_backwards_list_print(
      unit,
      mc_group_id_2,
      list_of_next_entries_2
      );
    soc_sand_os_printf("-------------------------------------------------\n\r");
  }
  
  /* compare size and content */
  eg_mc_entry.vlan_mc_id = 1;

  for (i = 0; i < nof_additions; i++)
  {
    eg_mc_entry.vlan_mc_id++;

    driver_ret = soc_petra_mult_eg_groups_compare(
      &eg_mc_entry,
      1,
      &mc_group_from_hw_2[i],
      1,
      &diff
      );
    if (diff != SOC_PETRA_MULT_TESTS_GROUP_DIFF_EQUAL)
    {
      if(!silent)
      {
        soc_sand_os_printf("The groups from HW is different than the group from SW\n\r");
      }
      pass = FALSE;
    }
  }
  
  /* check SW-DataBase: {*/
  used_entries2 = SOC_SAND_DIV_ROUND_UP(mc_group_size_2,SOC_PETRA_MULT_EG_FORMAT_C_PER_ENTRY);

  if (nof_unoccupied != (SOC_PETRA_MULT_TABLE_SIZE - used_entries - used_entries2))
  {
    soc_sand_os_printf("The Number of unoccupied entries is different than expected\n\r");
    pass = FALSE;
    soc_sand_os_printf("soc_petra_mult_egress_new_mode_port_add_TEST : Test FAILED\n\r");
    goto exit;
  }

  pass = soc_petra_mult_tests_unoccupied_list_check_occupation(
    unit,
    (uint16)(mc_group_id_2 | SOC_PETRA_MULT_LL_EGRESS_MC_ID_START_ENTRY),
    list_of_next_entries_2,
    silent
    );

  if (!pass)
  {
    soc_sand_os_printf("soc_petra_mult_egress_new_mode_port_add_TEST : Test FAILED\n\r");
    goto exit;
  }

  pass = soc_petra_mult_tests_backwards_list_check_occupation(
    unit,
    (uint16)(mc_group_id_2 | SOC_PETRA_MULT_LL_EGRESS_MC_ID_START_ENTRY),
    list_of_next_entries_2,
    silent
    );

  if (!pass)
  {
    soc_sand_os_printf("soc_petra_mult_egress_new_mode_port_add_TEST : Test FAILED\n\r");
    goto exit;
  }
  
  if (pass)
  {
    soc_sand_os_printf("\n\r* soc_petra_mult_egress_new_mode_port_add_TEST : Test PASSED\n\r");
  }

exit:
  if (!silent)
  {
    soc_sand_os_printf("\n\r ********** soc_petra_mult_egress_new_mode_port_add_TEST : END **********\n\r");
  }

  SOC_PETRA_FREE(mc_group_from_hw_1);
  SOC_PETRA_FREE(list_of_next_entries_1);
  SOC_PETRA_FREE(mc_group_from_hw_2);
  SOC_PETRA_FREE(list_of_next_entries_2);

  return pass;
}

/*****************************************************
*NAME
*  soc_petra_mult_egress_new_mode_port_add_2_TEST
*TYPE:
*  PROC
*DATE:
*  15-Feb-2008
*FUNCTION:
* Verification of Multicast egress new mode - tests port adding of all special cases:
* different last entry + adding vlan / port.
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
*   This Test does not change the 'pass' variable -
*   it will always return TRUE at current implementation.
*SEE ALSO:
*****************************************************/
uint32
soc_petra_mult_egress_new_mode_port_add_2_TEST(
  SOC_SAND_IN int unit,
  SOC_SAND_IN uint8 silent
  )
{
  SOC_PETRA_MULT_EG_ENTRY
    *mc_group_from_hw_1 = NULL;
  int32
    pass = TRUE,
    i,
    j,
    indx1 = 0,
    indx2 = 0,
    mc_group_size_1,
    mc_group_size_port,
    mc_group_size_vlan;
  uint8
    insufficient_memory = FALSE;
  uint32
    mc_group_from_hw_size_exact_1;
  uint16
    mc_group_id_1,
    *list_of_next_entries_1 = NULL;
  uint32
    driver_ret;
  uint32
    nof_unoccupied,
    used_entries[SOC_PETRA_MULT_TEST_15_EG_GROUP_SIZE] = {1,1,2,2,3,3,4,4,5,5,5,5},
    which_type[SOC_PETRA_MULT_TEST_15_EG_GROUP_SIZE-1] = {0,1,0,1,1,1,1,0,0,0,0},
    val;
  SOC_PETRA_MULT_TESTS_GROUP_DIFF
    diff;
  SOC_PETRA_MULT_EG_ENTRY
    eg_mc_group_temp;
  SOC_PETRA_MULT_EG_ENTRY
    *eg_mc_group_from_user_1 = NULL,
    *eg_mc_group_from_user_port = NULL,
    *eg_mc_group_from_user_vlan = NULL;


  if (!silent)
  {
    soc_sand_os_printf("\n\r ********** soc_petra_mult_egress_new_mode_port_add_2_TEST : START **********\n\r");
  }

  mc_group_id_1 = SOC_PETRA_MULT_TEST_15_EG_GROUP_START_ID;
  mc_group_size_1 = SOC_PETRA_MULT_TEST_15_EG_GROUP_SIZE;
  mc_group_size_port = SOC_PETRA_MULT_TEST_15_EG_GROUP_SIZE_PORT;
  mc_group_size_vlan = SOC_PETRA_MULT_TEST_15_EG_GROUP_SIZE_VLAN;

  eg_mc_group_from_user_1 = sal_alloc(sizeof(SOC_PETRA_MULT_EG_ENTRY) * mc_group_size_1,
    "soc_petra_mult_egress_new_mode_port_add_2_TEST.eg_mc_group_from_user_1");
  if(eg_mc_group_from_user_1 == NULL) {
    pass = FALSE;
    goto exit;
  }

  eg_mc_group_from_user_port = sal_alloc(sizeof(SOC_PETRA_MULT_EG_ENTRY) * mc_group_size_port,
    "soc_petra_mult_egress_new_mode_port_add_2_TEST.eg_mc_group_from_user_port");
  if(eg_mc_group_from_user_port == NULL) {
    pass = FALSE;
    goto exit;
  }

  eg_mc_group_from_user_vlan = sal_alloc(sizeof(SOC_PETRA_MULT_EG_ENTRY) * mc_group_size_vlan,
    "soc_petra_mult_egress_new_mode_port_add_2_TEST.eg_mc_group_from_user_vlan");
  if(eg_mc_group_from_user_vlan == NULL) {
    pass = FALSE;
    goto exit;
  }

  mc_group_from_hw_1 = sal_alloc(sizeof(SOC_PETRA_MULT_EG_ENTRY) * mc_group_size_1,
    "soc_petra_mult_egress_new_mode_port_add_2_TEST.mc_group_from_hw_1");
  if(mc_group_from_hw_1 == NULL) {
    pass = FALSE;
    goto exit;
  }

  list_of_next_entries_1 = sal_alloc(sizeof(uint16) * mc_group_size_1,
    "soc_petra_mult_egress_new_mode_port_add_2_TEST.list_of_next_entries_1");
  if(list_of_next_entries_1 == NULL) {
    pass = FALSE;
    goto exit;
  }

  for (i = 0 ; i < mc_group_size_1 ; i++)
  {
    soc_petra_PETRA_MULT_EG_ENTRY_clear(&eg_mc_group_from_user_1[i]);
  }
  for (i = 0; i < mc_group_size_port; i++)
  {
    soc_petra_PETRA_MULT_EG_ENTRY_clear(&eg_mc_group_from_user_port[i]);
  }
  for (i = 0; i < mc_group_size_vlan; i++)
  {
    soc_petra_PETRA_MULT_EG_ENTRY_clear(&eg_mc_group_from_user_vlan[i]);
  }
  soc_petra_PETRA_MULT_EG_ENTRY_clear(&eg_mc_group_temp);

/*
 * Note: This logical test is a very specific scenario, any logic change in egress multicast might
 * effect this test.
 */

/*
 * ADD Elements to mc-group by this order:
 * 1. from ____ => P__
 * 2. from P__ => PP_
 * 3. from PP_ => PV P__ (update)
 * 4. from PV P__ => PV PV
 * 5. from PV PV => PV PV V_
 * 6. from PV PV V_ => PV PV VV
 * 7. from PV PV VV => PV PV VV V_
 * 8. from PV PV VV V_ => PV PV VV PV
 * 9. from PV PV VV PV => PV PV VV PV P__
 * 10. from PV PV VV PV P__ => PV PV VV PV PP_
 * 11. from PV PV VV PV PP_ => PV PV VV PV PPP
 * 12. from PV PV VV PV PPP => PPP PPP VV VV VV (update)
 */
  for (i = 0 ; i < mc_group_size_port ; i++)
  {
    eg_mc_group_from_user_port[i].type = SOC_PETRA_MULT_EG_ENTRY_TYPE_OFP;
    eg_mc_group_from_user_port[i].cud = 0xabcd;
    eg_mc_group_from_user_port[i].port = i+1;
  }
  for (i = 0 ; i < mc_group_size_vlan ; i++)
  {
    eg_mc_group_from_user_vlan[i].type = SOC_PETRA_MULT_EG_ENTRY_TYPE_VLAN_PTR;
    eg_mc_group_from_user_vlan[i].cud = 0xabcd;
    eg_mc_group_from_user_vlan[i].vlan_mc_id = i+1;
  }
  
  eg_mc_group_from_user_1[0] = eg_mc_group_from_user_port[0];
  eg_mc_group_from_user_1[1] = eg_mc_group_from_user_port[1];
  eg_mc_group_from_user_1[2] = *eg_mc_group_from_user_vlan;
  
  for (i = 0; i < mc_group_size_1; i++)
  {
    if (!silent)
    {
      soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
      soc_sand_os_printf("The entry from User:\n\r");
      if (eg_mc_group_from_user_1[i].type == SOC_PETRA_MULT_EG_ENTRY_TYPE_OFP)
      {
        val = eg_mc_group_from_user_1[i].port;
      }
      else
      {
        val = eg_mc_group_from_user_1[i].vlan_mc_id;
      }
      soc_sand_os_printf(
        "{%s; %02d ; 0x%X} \n\r",
        soc_petra_PETRA_MULT_EG_ENTRY_TYPE_to_string(eg_mc_group_from_user_1[i].type),
        val,
        eg_mc_group_from_user_1[i].cud
        );
      soc_sand_os_printf("-------------------------------------------------\n\r");
    }

    driver_ret = soc_petra_mult_eg_port_add_unsafe(
      unit,
      mc_group_id_1,
      &eg_mc_group_from_user_1[i],
      &insufficient_memory
      );
	
    if(soc_sand_get_error_code_from_error_word(driver_ret) != SOC_SAND_OK)
    {
      goto exit;
    }

    driver_ret = soc_petra_mult_tests_get_eg_mc_group_new_mode_from_hw(
      unit,
      mc_group_id_1,
      mc_group_from_hw_1,
      &mc_group_from_hw_size_exact_1,
      list_of_next_entries_1
      );

    if(soc_sand_get_error_code_from_error_word(driver_ret) != SOC_SAND_OK)
    {
      goto exit;
    }

    if (!silent)
    {
      /*Print group from HW*/
      soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
      soc_sand_os_printf("The group from HW:\n\r");
      soc_petra_multicast_eg_group_print(
        mc_group_from_hw_1,
        mc_group_from_hw_size_exact_1
        );
      soc_sand_os_printf("-------------------------------------------------\n\r");
    }
    /* update, rearrange order of entries */
    if (i == 2)
    {
      for (j = 0; j < mc_group_size_1 - 1; j++)
      {
        if (which_type[j] == 0)
        {
          eg_mc_group_from_user_1[j] = eg_mc_group_from_user_port[indx1];
          indx1++;
        }
        else
        {
          eg_mc_group_from_user_1[j] = eg_mc_group_from_user_vlan[indx2];
          indx2++;
        }
        
      }
    }
    if (i == 7)
    {
      /* change order of the last two entries in output */
      eg_mc_group_temp = eg_mc_group_from_user_1[i-1];
      eg_mc_group_from_user_1[i-1] = eg_mc_group_from_user_1[i];
      eg_mc_group_from_user_1[i] = eg_mc_group_temp;
    }
    if (i == mc_group_size_1-1)
    {
      /* check special case: rearrange order as ports first then vlans */
      sal_memcpy(eg_mc_group_from_user_1, eg_mc_group_from_user_port, sizeof(SOC_PETRA_MULT_EG_ENTRY) * mc_group_size_port);
      sal_memcpy(&(eg_mc_group_from_user_1[mc_group_size_port]), eg_mc_group_from_user_vlan, sizeof(SOC_PETRA_MULT_EG_ENTRY) * mc_group_size_vlan);
    }


    /* compare size and content */
    /* Petra code. Almost not in use. Ignore coverity defects */
    /* coverity[returned_value] */
    driver_ret = soc_petra_mult_eg_groups_compare(
      eg_mc_group_from_user_1,
      i+1,
      mc_group_from_hw_1,
      mc_group_from_hw_size_exact_1,
      &diff
      );
    if (diff != SOC_PETRA_MULT_TESTS_GROUP_DIFF_EQUAL)
    {
      if(!silent)
      {
        soc_sand_os_printf("The groups from HW is different than the group from SW\n\r");
      }
      pass = FALSE;
    }
    /* check SW-DataBase: {*/

    nof_unoccupied = soc_petra_sw_db_multicast_nof_unoccupied_get(unit);

    if (nof_unoccupied != (SOC_PETRA_MULT_TABLE_SIZE - used_entries[i]))
    {
      soc_sand_os_printf("The Number of unoccupied entries is different than expected\n\r");
      pass = FALSE;
      soc_sand_os_printf("soc_petra_mult_egress_new_mode_port_add_TEST : Test FAILED\n\r");
      goto exit;
    }

  }

  if (!pass)
  {
    soc_sand_os_printf("soc_petra_mult_egress_new_mode_port_add_2_TEST : Test FAILED\n\r");
    goto exit;
  }

  if (pass)
  {
    soc_sand_os_printf("\n\r* soc_petra_mult_egress_new_mode_port_add_2_TEST : Test PASSED\n\r");
  }

exit:
  if (!silent)
  {
    soc_sand_os_printf("\n\r ********** soc_petra_mult_egress_new_mode_port_add_2_TEST : END **********\n\r");
  }

  SOC_PETRA_FREE(mc_group_from_hw_1);
  SOC_PETRA_FREE(list_of_next_entries_1);
  SOC_PETRA_FREE(eg_mc_group_from_user_1);
  SOC_PETRA_FREE(eg_mc_group_from_user_port);
  SOC_PETRA_FREE(eg_mc_group_from_user_vlan);

  return pass;
}

/*****************************************************
*NAME
*  soc_petra_mult_egress_new_mode_port_remove_TEST
*TYPE:
*  PROC
*DATE:
*  15-Feb-2008
*FUNCTION:
* Verification of Multicast egress new mode - test port remove.
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
*   This Test does not change the 'pass' variable -
*   it will always return TRUE at current implementation.
*SEE ALSO:
*****************************************************/
uint32
soc_petra_mult_egress_new_mode_port_remove_TEST(
  SOC_SAND_IN int unit,
  SOC_SAND_IN uint8 silent
  )
{
  SOC_PETRA_MULT_EG_ENTRY
    *eg_mc_group_from_user_1 = NULL,
    *eg_mc_group_expected_order = NULL,
    eg_mc_entry,
    *mc_group_from_hw_1 = NULL;
  uint32
    pass = TRUE,
    i,
    index1 = 1,
    index2 = 1,
    mc_group_size_1;
  SOC_TMC_ERROR
    mc_err;
  uint32
    mc_group_from_hw_size_exact_1;
  SOC_PETRA_MULT_TESTS_GROUP_DIFF
    diff;
  uint16
    mc_group_id_1,
    *list_of_next_entries_1 = NULL;
  uint32
    driver_ret,
    used_entries,
    nof_unoccupied,
    expected_order[SOC_PETRA_MULT_TEST_16_EG_GROUP_SIZE-1] = {0,2,4,6,1,8,5,7,9},
    expected_order_2[SOC_PETRA_MULT_TEST_16_EG_GROUP_SIZE-2] = {0,2,4,8,1,5,7,9};

  if (!silent)
  {
    soc_sand_os_printf("\n\r ********** soc_petra_mult_egress_new_mode_port_remove_TEST : START **********\n\r");
  }

/*
 * First Group
 */

  /*
   * Defining, Setting and Printing a new group
   * type = Egress
   * size = 10
   * mc_id = 7
   */
  /* { */
  mc_group_size_1 = SOC_PETRA_MULT_TEST_16_EG_GROUP_SIZE;
  mc_group_id_1 = SOC_PETRA_MULT_TEST_16_EG_GROUP_START_ID;

  eg_mc_group_from_user_1 = sal_alloc(sizeof(SOC_PETRA_MULT_EG_ENTRY) * mc_group_size_1,
    "soc_petra_mult_egress_new_mode_port_remove_TEST.eg_mc_group_from_user_1");
  if(eg_mc_group_from_user_1 == NULL) {
    pass = FALSE;
    goto exit;
  }

  eg_mc_group_expected_order = sal_alloc(sizeof(SOC_PETRA_MULT_EG_ENTRY) * mc_group_size_1,
    "soc_petra_mult_egress_new_mode_port_remove_TEST.eg_mc_group_expected_order");
  if(eg_mc_group_expected_order == NULL) {
    pass = FALSE;
    goto exit;
  }

  for (i = 0 ; i < mc_group_size_1 ; i++)
  {
    soc_petra_PETRA_MULT_EG_ENTRY_clear(&eg_mc_group_from_user_1[i]);
    soc_petra_PETRA_MULT_EG_ENTRY_clear(&eg_mc_group_expected_order[i]);
  }

  for (i = 0 ; i < mc_group_size_1 ; i++)
  {
    if (i % 2 == 0)
    {
      eg_mc_group_from_user_1[i].type = SOC_PETRA_MULT_EG_ENTRY_TYPE_OFP;
      eg_mc_group_from_user_1[i].port = index1++;
      eg_mc_group_from_user_1[i].cud = 0xabcd;
    }
    else
    {
      eg_mc_group_from_user_1[i].type = SOC_PETRA_MULT_EG_ENTRY_TYPE_VLAN_PTR;
      eg_mc_group_from_user_1[i].vlan_mc_id = index2++;
      eg_mc_group_from_user_1[i].cud = 0xabcd;
    }
    
  }

  /* Add all entries by port add*/
  for (i = 0; i < mc_group_size_1; i++)
  {
    driver_ret = soc_petra_mult_eg_port_add(
      unit,
      mc_group_id_1,
      &eg_mc_group_from_user_1[i],
      &mc_err
      );
    if(soc_sand_get_error_code_from_error_word(driver_ret) != SOC_SAND_OK)
    {
      goto exit;
    }
  }

  /* allocate mc-group before sending to get-from-hw function */
  mc_group_from_hw_1 = sal_alloc(sizeof(SOC_PETRA_MULT_EG_ENTRY) * mc_group_size_1,
    "soc_petra_mult_egress_new_mode_port_remove_TEST.mc_group_from_hw_1");
  if(mc_group_from_hw_1 == NULL) {
    pass = FALSE;
    goto exit;
  }

  list_of_next_entries_1 = sal_alloc(sizeof(uint16) * mc_group_size_1,
    "soc_petra_mult_egress_new_mode_port_remove_TEST.list_of_next_entries_1");
  if(list_of_next_entries_1 == NULL) {
    pass = FALSE;
    goto exit;
  }

  for (i = 0 ; i < mc_group_size_1 ; i++)
  {
    soc_petra_PETRA_MULT_EG_ENTRY_clear(&mc_group_from_hw_1[i]);
  }
  /* get-from-hw */

  driver_ret = soc_petra_mult_tests_get_eg_mc_group_new_mode_from_hw(
                 unit,
                 mc_group_id_1,
                 mc_group_from_hw_1,
                 &mc_group_from_hw_size_exact_1,
                 list_of_next_entries_1
               );

  if(soc_sand_get_error_code_from_error_word(driver_ret) != SOC_SAND_OK)
  {
    goto exit;
  }

  if (!silent)
  {
    /*Print group from HW*/
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
    soc_sand_os_printf("The group from HW:\n\r");

    soc_petra_multicast_eg_group_print(
      mc_group_from_hw_1,
      mc_group_size_1
    );
    soc_sand_os_printf("-------------------------------------------------\n\r");
  }

  /* Expected order as add: */
  /* PV PV PV PV PV */
  /* compare size and content */
  driver_ret = soc_petra_mult_eg_groups_compare(
                 eg_mc_group_from_user_1,
                 mc_group_size_1,
                 mc_group_from_hw_1,
                 mc_group_from_hw_size_exact_1,
                 &diff
               );
  if (diff != SOC_PETRA_MULT_TESTS_GROUP_DIFF_EQUAL)
  {
    if(!silent)
    {
      soc_sand_os_printf("The groups from HW is different than the group from SW\n\r");
    }
    pass = FALSE;
  }

  /* print the number of unocuupied entries */
  nof_unoccupied =
    soc_petra_sw_db_multicast_nof_unoccupied_get(unit);

  if (!silent)
  {
    /*Print SW DATABASE */
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
    soc_sand_os_printf("Software DataBase: \n\r");
    /* Number of unoccupied - should be equal to (total of entries - used)*/
    soc_sand_os_printf("\n\r"
      "# of unoccupied: %u \n\r",nof_unoccupied);

    /* Unoccupied entries list */
    soc_sand_os_printf("\n\r"
      "Unoccupied entries list \n\r");
    soc_petra_mult_tests_unoccupied_list_print(
      unit,
      mc_group_id_1,
      list_of_next_entries_1
      );

    /* Backwards Pointers list */
    soc_sand_os_printf("\n\r"
      "Backwards Pointers list \n\r");
    soc_petra_mult_tests_backwards_list_print(
      unit,
      mc_group_id_1,
      list_of_next_entries_1
      );
    soc_sand_os_printf("-------------------------------------------------\n\r");
  }
  
  used_entries = SOC_SAND_DIV_ROUND_UP(mc_group_size_1,SOC_PETRA_MULT_EG_FORMAT_B_PER_ENTRY);

  if (nof_unoccupied != (SOC_PETRA_MULT_TABLE_SIZE - used_entries))
  {
    soc_sand_os_printf("The Number of unoccupied entries is different than expected\n\r");
    pass = FALSE;
    soc_sand_os_printf("soc_petra_mult_egress_new_mode_port_remove_TEST : Test FAILED\n\r");
    goto exit;
  }

/*
 * Remove elements from mc-group
 */
  
 /*
 * removing an element from the group
 * type = Egress
 * mc_id = 7
 * element = type--> vlan_mc_id#, vlan-id--> 2, Outlif-->0xabcd
 */
/* { */
  soc_petra_PETRA_MULT_EG_ENTRY_clear(&eg_mc_entry);

  eg_mc_entry.type = SOC_PETRA_MULT_EG_ENTRY_TYPE_VLAN_PTR;
  eg_mc_entry.vlan_mc_id = 2;
  eg_mc_entry.cud = 0xabcd;
  
  if (!silent)
  {
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
    soc_sand_os_printf("The entry to remove:\n\r");

    soc_sand_os_printf(
      "{%s; %02d ; 0x%X} \n\r",
      soc_petra_PETRA_MULT_EG_ENTRY_TYPE_to_string(eg_mc_entry.type),
      eg_mc_entry.vlan_mc_id,
      eg_mc_entry.cud
      );
    soc_sand_os_printf("-------------------------------------------------\n\r");
  }
  
  /* } */

  /* remove element */

  driver_ret = soc_petra_mult_eg_port_remove(
    unit,
    mc_group_id_1,
    &eg_mc_entry,
    &mc_err
    );
  if(soc_sand_get_error_code_from_error_word(driver_ret) != SOC_SAND_OK)
  {
    goto exit;
  }

  mc_group_size_1--;
  for (i = 0 ; i < mc_group_size_1 ; i++)
  {
    soc_petra_PETRA_MULT_EG_ENTRY_clear(&mc_group_from_hw_1[i]);
  }
  /* get-from-hw */

  driver_ret = soc_petra_mult_tests_get_eg_mc_group_new_mode_from_hw(
    unit,
    mc_group_id_1,
    mc_group_from_hw_1,
    &mc_group_from_hw_size_exact_1,
    list_of_next_entries_1
    );

  if(soc_sand_get_error_code_from_error_word(driver_ret) != SOC_SAND_OK)
  {
    goto exit;
  }

  /* rearrange order of the entries as expected should be */
  /* expected order PPP PV PV VV */
  for (i = 0 ; i < mc_group_size_1 ; i++)
  {
    eg_mc_group_expected_order[i] = eg_mc_group_from_user_1[expected_order[i]];
  }

  if (!silent)
  {
    /*Print group from HW*/
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
    soc_sand_os_printf("The group from HW:\n\r");

    soc_petra_multicast_eg_group_print(
      mc_group_from_hw_1,
      mc_group_size_1
      );
    soc_sand_os_printf("-------------------------------------------------\n\r");
  }

  /* compare size and content */
  driver_ret = soc_petra_mult_eg_groups_compare(
    eg_mc_group_expected_order,
    mc_group_size_1,
    mc_group_from_hw_1,
    mc_group_from_hw_size_exact_1,
    &diff
    );
  if (diff != SOC_PETRA_MULT_TESTS_GROUP_DIFF_EQUAL)
  {
    if(!silent)
    {
      soc_sand_os_printf("The groups from HW is different than the group from SW\n\r");
    }
    pass = FALSE;
  }

  /* print the number of unocuupied entries */
  nof_unoccupied =
    soc_petra_sw_db_multicast_nof_unoccupied_get(unit);

  if (!silent)
  {
    /*Print SW DATABASE */
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
    soc_sand_os_printf("Software DataBase: \n\r");
    /* Number of unoccupied - should be equal to (total of entries - used)*/
    soc_sand_os_printf("\n\r"
      "# of unoccupied: %u \n\r",nof_unoccupied);

    /* Unoccupied entries list */
    soc_sand_os_printf("\n\r"
      "Unoccupied entries list \n\r");
    soc_petra_mult_tests_unoccupied_list_print(
      unit,
      mc_group_id_1,
      list_of_next_entries_1
      );

    /* Backwards Pointers list */
    soc_sand_os_printf("\n\r"
      "Backwards Pointers list \n\r");
    soc_petra_mult_tests_backwards_list_print(
      unit,
      mc_group_id_1,
      list_of_next_entries_1
      );
    soc_sand_os_printf("-------------------------------------------------\n\r");
  }

  used_entries = SOC_SAND_DIV_ROUND_UP(3,SOC_PETRA_MULT_EG_FORMAT_A_PER_ENTRY) +
     SOC_SAND_DIV_ROUND_UP(4,SOC_PETRA_MULT_EG_FORMAT_B_PER_ENTRY) +
     SOC_SAND_DIV_ROUND_UP(2,SOC_PETRA_MULT_EG_FORMAT_C_PER_ENTRY);

  if (nof_unoccupied != (SOC_PETRA_MULT_TABLE_SIZE - used_entries))
  {
    soc_sand_os_printf("The Number of unoccupied entries is different than expected\n\r");
    pass = FALSE;
    soc_sand_os_printf("soc_petra_mult_egress_new_mode_port_remove_TEST : Test FAILED\n\r");
    goto exit;
  }



/*
 * removing an element from the group
 * type = Egress
 * mc_id = 7
 * element = type--> port#, port-id--> 4, Outlif-->0xabcd
 */
/* { */
  soc_petra_PETRA_MULT_EG_ENTRY_clear(&eg_mc_entry);

  eg_mc_entry.type = SOC_PETRA_MULT_EG_ENTRY_TYPE_OFP;
  eg_mc_entry.port = 4;
  eg_mc_entry.cud = 0xabcd;
  
  if (!silent)
  {
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
    soc_sand_os_printf("The entry to remove:\n\r");

    soc_sand_os_printf(
      "{%s; %02d ; 0x%X} \n\r",
      soc_petra_PETRA_MULT_EG_ENTRY_TYPE_to_string(eg_mc_entry.type),
      eg_mc_entry.port,
      eg_mc_entry.cud
      );
    soc_sand_os_printf("-------------------------------------------------\n\r");
  }
  
  /* } */
  /* remove another element */

  driver_ret = soc_petra_mult_eg_port_remove(
    unit,
    mc_group_id_1,
    &eg_mc_entry,
    &mc_err
    );
  if(soc_sand_get_error_code_from_error_word(driver_ret) != SOC_SAND_OK)
  {
    goto exit;
  }

  mc_group_size_1--;
  for (i = 0 ; i < mc_group_size_1 ; i++)
  {
    soc_petra_PETRA_MULT_EG_ENTRY_clear(&mc_group_from_hw_1[i]);
  }
  /* get-from-hw */

  driver_ret = soc_petra_mult_tests_get_eg_mc_group_new_mode_from_hw(
    unit,
    mc_group_id_1,
    mc_group_from_hw_1,
    &mc_group_from_hw_size_exact_1,
    list_of_next_entries_1
    );

  if(soc_sand_get_error_code_from_error_word(driver_ret) != SOC_SAND_OK)
  {
    goto exit;
  }

  /* rearrange order of the entries as expected should be */
  /* expected order PPP PV PV VV */
  for (i = 0 ; i < mc_group_size_1 ; i++)
  {
    eg_mc_group_expected_order[i] = eg_mc_group_from_user_1[expected_order_2[i]];
  }

  if (!silent)
  {
    /*Print group from HW*/
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
    soc_sand_os_printf("The group from HW:\n\r");

    soc_petra_multicast_eg_group_print(
      mc_group_from_hw_1,
      mc_group_size_1
      );
    soc_sand_os_printf("-------------------------------------------------\n\r");
  }

  /* compare size and content */
  driver_ret = soc_petra_mult_eg_groups_compare(
    eg_mc_group_expected_order,
    mc_group_size_1,
    mc_group_from_hw_1,
    mc_group_from_hw_size_exact_1,
    &diff
    );
  if (diff != SOC_PETRA_MULT_TESTS_GROUP_DIFF_EQUAL)
  {
    if(!silent)
    {
      soc_sand_os_printf("The groups from HW is different than the group from SW\n\r");
    }
    pass = FALSE;
  }

  /* print the number of unocuupied entries */
  nof_unoccupied =
    soc_petra_sw_db_multicast_nof_unoccupied_get(unit);

  if (!silent)
  {
    /*Print SW DATABASE */
    soc_sand_os_printf("\n\r-------------------------------------------------\n\r");
    soc_sand_os_printf("Software DataBase: \n\r");
    /* Number of unoccupied - should be equal to (total of entries - used)*/
    soc_sand_os_printf("\n\r"
      "# of unoccupied: %u \n\r",nof_unoccupied);

    /* Unoccupied entries list */
    soc_sand_os_printf("\n\r"
      "Unoccupied entries list \n\r");
    soc_petra_mult_tests_unoccupied_list_print(
      unit,
      mc_group_id_1,
      list_of_next_entries_1
      );

    /* Backwards Pointers list */
    soc_sand_os_printf("\n\r"
      "Backwards Pointers list \n\r");
    soc_petra_mult_tests_backwards_list_print(
      unit,
      mc_group_id_1,
      list_of_next_entries_1
      );
    soc_sand_os_printf("-------------------------------------------------\n\r");
  }

  used_entries = SOC_SAND_DIV_ROUND_UP(3,SOC_PETRA_MULT_EG_FORMAT_A_PER_ENTRY) +
    SOC_SAND_DIV_ROUND_UP(2,SOC_PETRA_MULT_EG_FORMAT_B_PER_ENTRY) +
    SOC_SAND_DIV_ROUND_UP(3,SOC_PETRA_MULT_EG_FORMAT_C_PER_ENTRY);

  if (nof_unoccupied != (SOC_PETRA_MULT_TABLE_SIZE - used_entries))
  {
    soc_sand_os_printf("The Number of unoccupied entries is different than expected\n\r");
    pass = FALSE;
    soc_sand_os_printf("soc_petra_mult_egress_new_mode_port_remove_TEST : Test FAILED\n\r");
    goto exit;
  }

  pass = soc_petra_mult_tests_unoccupied_list_check_occupation(
    unit,
    (uint16)(mc_group_id_1 | SOC_PETRA_MULT_LL_EGRESS_MC_ID_START_ENTRY),
    list_of_next_entries_1,
    silent
    );

  if (!pass)
  {
    soc_sand_os_printf("soc_petra_mult_egress_new_mode_port_remove_TEST : Test FAILED\n\r");
    goto exit;
  }

  pass = soc_petra_mult_tests_backwards_list_check_occupation(
    unit,
    (uint16)(mc_group_id_1 | SOC_PETRA_MULT_LL_EGRESS_MC_ID_START_ENTRY),
    list_of_next_entries_1,
    silent
    );

  if (!pass)
  {
    soc_sand_os_printf("soc_petra_mult_egress_new_mode_port_remove_TEST : Test FAILED\n\r");
    goto exit;
  }
  /* check SW-DataBase: } */

  if(!silent)
  {
    /* to check-up on the erased entry*/
    soc_sand_os_printf("\n\r"
      "Unoccupied entries list OLD\n\r");
    soc_petra_mult_tests_unoccupied_list_print(
      unit,
      mc_group_id_1,
      list_of_next_entries_1
      );
    soc_sand_os_printf("\n\r"
      "Backwards Pointers list OLD\n\r");
    soc_petra_mult_tests_backwards_list_print(
      unit,
      mc_group_id_1,
      list_of_next_entries_1
      );
  }

  if (pass)
  {
    soc_sand_os_printf("\n\r* soc_petra_mult_egress_new_mode_port_remove_TEST : Test PASSED\n\r");
  }
exit:
  if (!silent)
  {
    soc_sand_os_printf("\n\r ********** soc_petra_mult_egress_new_mode_port_remove_TEST : END **********\n\r");
  }

  SOC_PETRA_FREE(mc_group_from_hw_1);
  SOC_PETRA_FREE(eg_mc_group_from_user_1);
  SOC_PETRA_FREE(eg_mc_group_expected_order);
  SOC_PETRA_FREE(list_of_next_entries_1);
  return pass;
}

/*****************************************************
*NAME
*  soc_petra_mult_logical_test_all
*TYPE:
*  PROC
*DATE:
*  03-Apr-2008
*FUNCTION:
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
  soc_petra_mult_logical_test_all(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 silent
  )
{
  uint32
    pass = TRUE;
  SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE
    info;
  uint32
    res;


  /* White-Box Tests */
  soc_petra_mult_ing_all_groups_close(unit);
  soc_petra_mult_eg_all_groups_close(unit);
  
  soc_petra_PETRA_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE_clear(&info);

  info.mc_id_low = 0 ;
  info.mc_id_high = SOC_PETRA_MULT_EG_VLAN_NOF_IDS_MAX-1 ;

  res = soc_petra_mult_eg_vlan_membership_group_range_set(
    unit,
    &info
    );
  if(soc_sand_get_error_code_from_error_word(res) != SOC_SAND_OK)
  {
    goto exit;
  }
  
  pass = pass && soc_petra_mult_open_TEST_correct_db(
    unit,
    silent
    );


  res =  soc_petra_mult_ing_all_groups_close(unit);
  if(soc_sand_get_error_code_from_error_word(res) != SOC_SAND_OK)
  {
    pass = FALSE;
    goto exit;
  }

  pass = pass && soc_petra_mult_open_2_intersecting_groups_TEST_correct_db(
    unit,
    silent
    );


  res =  soc_petra_mult_ing_all_groups_close(unit);
  if(soc_sand_get_error_code_from_error_word(res) != SOC_SAND_OK)
  {
    pass = FALSE;
    goto exit;
  }

  pass = pass && soc_petra_mult_update_group_TEST_correct_db(
    unit,
    silent
    );


  res =  soc_petra_mult_ing_all_groups_close(unit);
  if(soc_sand_get_error_code_from_error_word(res) != SOC_SAND_OK)
  {
    pass = FALSE;
    goto exit;
  }

  pass = pass && soc_petra_mult_open_add_to_group_TEST_correct_db(
    unit,
    silent
    );


  res =  soc_petra_mult_ing_all_groups_close(unit);
  if(soc_sand_get_error_code_from_error_word(res) != SOC_SAND_OK)
  {
    pass = FALSE;
    goto exit;
  }

 
  pass = pass && soc_petra_mult_open_close_group_TEST_correct_db(
    unit,
    silent
    );



  res =  soc_petra_mult_ing_all_groups_close(unit);
  if(soc_sand_get_error_code_from_error_word(res) != SOC_SAND_OK)
  {
    pass = FALSE;
    goto exit;
  }

  pass = pass && soc_petra_mult_open_relocation_TEST_correct_db(
    unit,
    silent
    );


  res =  soc_petra_mult_eg_all_groups_close(unit);
  if(soc_sand_get_error_code_from_error_word(res) != SOC_SAND_OK)
  {
    pass = FALSE;
    goto exit;
  }

  res =  soc_petra_mult_ing_all_groups_close(unit);
  if(soc_sand_get_error_code_from_error_word(res) != SOC_SAND_OK)
  {
    pass = FALSE;
    goto exit;
  }

  pass = pass && soc_petra_mult_occupied_but_empty_TEST_correct_db(
    unit,
    silent
    );


  res =  soc_petra_mult_eg_all_groups_close(unit);
  if(soc_sand_get_error_code_from_error_word(res) != SOC_SAND_OK)
  {
    pass = FALSE;
    goto exit;
  }

  res =  soc_petra_mult_ing_all_groups_close(unit);
  if(soc_sand_get_error_code_from_error_word(res) != SOC_SAND_OK)
  {
    pass = FALSE;
    goto exit;
  }

  pass = pass && soc_petra_mult_bitmap_allocation_TEST(
    unit,
    silent
    );

  /* Black-Box Tests */

  res =  soc_petra_mult_eg_all_groups_close(unit);
  if(soc_sand_get_error_code_from_error_word(res) != SOC_SAND_OK)
  {
    pass = FALSE;
    goto exit;
  }

  res =  soc_petra_mult_ing_all_groups_close(unit);
  if(soc_sand_get_error_code_from_error_word(res) != SOC_SAND_OK)
  {
    pass = FALSE;
    goto exit;
  }

  info.mc_id_low = 15000;
  info.mc_id_high = 16*1024-10;
  res = soc_petra_mult_eg_vlan_membership_group_range_set(
    unit,
    &info
    );
  if(soc_sand_get_error_code_from_error_word(res) != SOC_SAND_OK)
  {
    pass = FALSE;
    goto exit;
  }

  pass = pass && soc_petra_mult_full_set_and_get_start_from_mid_1(
    unit,
    silent
    );

  res =  soc_petra_mult_eg_all_groups_close(unit);
  if(soc_sand_get_error_code_from_error_word(res) != SOC_SAND_OK)
  {
    pass = FALSE;
    goto exit;
  }

  res =  soc_petra_mult_ing_all_groups_close(unit);
  if(soc_sand_get_error_code_from_error_word(res) != SOC_SAND_OK)
  {
    pass = FALSE;
    goto exit;
  }

  pass = pass && soc_petra_mult_full_table_TEST_1(
    unit,
    silent
    );
  
  res =  soc_petra_mult_eg_all_groups_close(unit);
  if(soc_sand_get_error_code_from_error_word(res) != SOC_SAND_OK)
  {
    pass = FALSE;
    goto exit;
  }

  res =  soc_petra_mult_ing_all_groups_close(unit);
  if(soc_sand_get_error_code_from_error_word(res) != SOC_SAND_OK)
  {
    pass = FALSE;
    goto exit;
  }

  pass = pass && soc_petra_mult_full_set_and_get(
    unit,
    silent
    );

  res =  soc_petra_mult_eg_all_groups_close(unit);
  if(soc_sand_get_error_code_from_error_word(res) != SOC_SAND_OK)
  {
    pass = FALSE;
    goto exit;
  }

  res =  soc_petra_mult_ing_all_groups_close(unit);
  if(soc_sand_get_error_code_from_error_word(res) != SOC_SAND_OK)
  {
    pass = FALSE;
    goto exit;
  }
  
  pass = pass && soc_petra_mult_full_table_TEST_3(
    unit,
    silent
    );

  res =  soc_petra_mult_eg_all_groups_close(unit);
  if(soc_sand_get_error_code_from_error_word(res) != SOC_SAND_OK)
  {
    pass = FALSE;
    goto exit;
  }

  res =  soc_petra_mult_ing_all_groups_close(unit);
  if(soc_sand_get_error_code_from_error_word(res) != SOC_SAND_OK)
  {
    pass = FALSE;
    goto exit;
  }

  if(SOC_PETRA_IS_DEV_PETRA_B_OR_ABOVE) {
	  /* New multicast mode - Logical Tests */
	  pass = pass && soc_petra_mult_egress_new_mode_allocation_TEST(
		unit,
		silent
		);
	  
	  res =  soc_petra_mult_eg_all_groups_close(unit);
      if(soc_sand_get_error_code_from_error_word(res) != SOC_SAND_OK)
      {
        pass = FALSE;
        goto exit;
      }

	  res =  soc_petra_mult_ing_all_groups_close(unit);
      if(soc_sand_get_error_code_from_error_word(res) != SOC_SAND_OK)
      {
        pass = FALSE;
        goto exit;
      }

	  pass = pass && soc_petra_mult_egress_new_mode_port_add_TEST(
		unit,
		silent
		);
	  
	  res =  soc_petra_mult_eg_all_groups_close(unit);
      if(soc_sand_get_error_code_from_error_word(res) != SOC_SAND_OK)
      {
        pass = FALSE;
        goto exit;
      }

	  res =  soc_petra_mult_ing_all_groups_close(unit);
      if(soc_sand_get_error_code_from_error_word(res) != SOC_SAND_OK)
      {
        pass = FALSE;
        goto exit;
      }

	  pass = pass && soc_petra_mult_egress_new_mode_port_add_2_TEST(
		unit,
		silent
		);
	  
	  res =  soc_petra_mult_eg_all_groups_close(unit);
      if(soc_sand_get_error_code_from_error_word(res) != SOC_SAND_OK)
      {
        pass = FALSE;
        goto exit;
      }

	  res =  soc_petra_mult_ing_all_groups_close(unit);
      if(soc_sand_get_error_code_from_error_word(res) != SOC_SAND_OK)
      {
        pass = FALSE;
        goto exit;
      }

	  pass = pass && soc_petra_mult_egress_new_mode_port_remove_TEST(
		unit,
		silent
		);
	  /* End of New multicast mode - Logical Tests */
  }
exit:
  return pass;
}
#endif
#include <soc/dpp/SAND/Utils/sand_footer.h>
