/* $Id: petra_multicast_linked_list.c,v 1.8 Broadcom SDK $
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

#include <soc/dpp/Petra/petra_multicast_linked_list.h>
#include <soc/dpp/Petra/petra_multicast_ingress.h>
#include <soc/dpp/Petra/petra_multicast_egress.h>
#include <soc/dpp/Petra/petra_reg_access.h>
#include <soc/dpp/Petra/petra_tbl_access.h>
#include <soc/dpp/Petra/petra_general.h>
#include <soc/dpp/Petra/petra_sw_db.h>
#include <soc/dpp/Petra/petra_chip_tbls.h>
#include <soc/dpp/Petra/petra_multicast_fabric.h>

#include <soc/dpp/SAND/Utils/sand_bitstream.h>
#include <soc/dpp/SAND/Utils/sand_conv.h>

#ifdef LINK_PB_LIBRARIES
#include <soc/dpp/Petra/PB_TM/pb_multicast_egress.h>
#endif

/* } */

/*************
 * DEFINES   *
 *************/
/* { */
#define SOC_PETRA_MC_NULL_ID_DFLT       0x0
#define SOC_PETRA_MC_NULL_ID_PB_INGR    0x7FFF
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

/*********************************************************************
* NAME:
*     soc_petra_mc_regs_init
* FUNCTION:
*   Initialization of the Soc_petra blocks configured in this module.
*   This function directly accesses registers/tables for
*   initializations that are not covered by API-s
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*   Called as part of the initialization sequence.
*********************************************************************/
STATIC uint32
  soc_petra_mc_regs_init(
    SOC_SAND_IN  int                 unit
  )
{
  uint32
    res;
  SOC_PETRA_REGS
    *regs;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MC_REGS_INIT);

  regs = soc_petra_regs();

  SOC_PETRA_FLD_SET(regs->rtp.multicast_distribution_configuration_reg.mul_num_trav, 0x3, 10, exit);

  /*
   *  Disable the hardware control of the length of the link lists
   *  without modifying the initial value of the length control
   *  (20*3 = 60 members)
   */
  SOC_PETRA_FLD_SET(regs->egq.multicast_link_list_max_length_reg.link_list_max_length_vld, 0x0, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mc_regs_init()",0,0);
}

/*********************************************************************
* NAME:
*     soc_petra_mc_init
* FUNCTION:
*     Initialization of the Soc_petra blocks configured in this module.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*   Called as part of the initialization sequence.
*********************************************************************/
uint32
  soc_petra_mc_init(
    SOC_SAND_IN  int                 unit
  )
{
  uint32
    reg_idx,
    fld_idx,
    res = SOC_SAND_OK;
  uint32
    mc_tc,
    idx;
  SOC_PETRA_MULT_FABRIC_ACTIVE_LINKS
    links;
  SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE
    eg_vlan_rep;
  SOC_SAND_U32_RANGE
    fmc_q_range;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MC_INIT);

  res = soc_petra_mc_regs_init(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_petra_mult_rplct_tbl_entry_unoccupied_set_all(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

  /*
   *  Traffic Class to multicast traffic class mapping - set MULT-TC = TC/2
   */
  for (idx = 0; idx < SOC_PETRA_NOF_TRAFFIC_CLASSES; idx ++)
  {
    mc_tc = idx/2;

    res = soc_petra_mult_fabric_traffic_class_to_multicast_cls_map_set_unsafe(
            unit,
            idx,
            mc_tc
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);
  }

  /*
   *  Enable auto-detect of active fabric links, with no mask.
   *  For this purpose - set all links as "active", and then enable auto-refresh
   */
  for (idx = 0; idx < SOC_DPP_DEFS_GET(unit, nof_fabric_links); idx++)
  {
    reg_idx = SOC_PETRA_REG_IDX_GET(idx, SOC_SAND_REG_SIZE_BITS);
    fld_idx = SOC_PETRA_FLD_IDX_GET(idx, SOC_SAND_REG_SIZE_BITS);
    /* Petra code. Almost not in use. Ignore coverity defects */
    /* coverity[uninit_use] */
    SOC_SAND_SET_BIT(links.bitmap[reg_idx], 0x1, fld_idx);
  }

  res = soc_petra_mult_fabric_active_links_set_unsafe(
          unit,
          &links,
          TRUE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  eg_vlan_rep.mc_id_low  = 0;
  eg_vlan_rep.mc_id_high = SOC_PETRA_MULT_EG_VLAN_NOF_IDS_MAX - 1;
  res = soc_petra_mult_eg_vlan_membership_group_range_set_unsafe(
          unit,
          &eg_vlan_rep
        );
  
  if SOC_PETRA_IS_DEV_PETRA_B_OR_ABOVE
  {
    soc_sand_SAND_U32_RANGE_clear(&fmc_q_range);
    fmc_q_range.start = 0;
    fmc_q_range.end = 3;
    res = soc_petra_mult_fabric_enhanced_set_unsafe(
            unit,
            &fmc_q_range
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mc_init()",0,0);
}

uint16
  soc_petra_mc_null_id(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 is_ingr_not_egr
  )
{
  uint16
    mc_null_id = SOC_PETRA_MC_NULL_ID_DFLT;

  if ((SOC_PETRA_IS_DEV_PETRA_B_OR_ABOVE) && is_ingr_not_egr)
  {
    mc_null_id = SOC_PETRA_MC_NULL_ID_PB_INGR;
  }

  return mc_null_id;
}

/*********************************************************************
*     Returns nof vlan bitmaps exists in group given.
*********************************************************************/
uint32
  soc_petra_mult_nof_vlan_bitmaps_in_group_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN SOC_PETRA_MULT_EG_ENTRY *mc_group,
    SOC_SAND_IN uint32 mc_group_size,
    SOC_SAND_IN uint8 is_ingress
  )
{
  uint32
    index;
  uint32
    nof_vlan_bitmaps = 0;
  
  if (!is_ingress)
  {
    for (index = 0; index < mc_group_size; index++)
    {
      if (mc_group[index].type == SOC_PETRA_MULT_EG_ENTRY_TYPE_VLAN_PTR)
      {
        nof_vlan_bitmaps++;
      }
    }
  }
 
  return nof_vlan_bitmaps;
  
}

/*********************************************************************
*     This function returns the nof port/vlans per entry
*     giving by format table (A/B/C).
*********************************************************************/
STATIC uint32
soc_petra_mult_eg_progress_per_entry_format_get(
  SOC_SAND_IN int unit,
  SOC_SAND_IN SOC_PETRA_MULT_EG_FORMAT_TYPE format_type
  )
{
  uint32
    process_index_by;

  switch (format_type)
  {
  case SOC_PETRA_MULT_EG_FORMAT_A:
    process_index_by = SOC_PETRA_MULT_EG_NOF_PORTS_PER_ENTRY;
    break;
  case SOC_PETRA_MULT_EG_FORMAT_B:
    process_index_by = SOC_PETRA_MULT_EG_FORMAT_B_PER_ENTRY;
    break;
  case SOC_PETRA_MULT_EG_FORMAT_C:
    process_index_by = SOC_PETRA_MULT_EG_FORMAT_C_PER_ENTRY;
    break;
  default:
    process_index_by = SOC_PETRA_MULT_EG_NOF_PORTS_PER_ENTRY;
  }

  return process_index_by;
}

/*********************************************************************
*     This function returns format type entry of the in_index_group
*     index in mc group given.
*********************************************************************/
STATIC SOC_PETRA_MULT_EG_FORMAT_TYPE
  soc_petra_mult_eg_format_entry_type_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN SOC_PETRA_MULT_ENTRY *mc_group,
    SOC_SAND_IN uint32 mc_group_size,
    SOC_SAND_IN uint32 in_index_group
  )
{
  SOC_PETRA_MULT_EG_FORMAT_TYPE
    format_type = SOC_PETRA_MULT_EG_FORMAT_A;
  
  /* progress index can be changed when vlan bitmaps exist, depends Formay type */
  if (in_index_group < mc_group_size &&
        mc_group[in_index_group].eg_entry.type == SOC_PETRA_MULT_EG_ENTRY_TYPE_VLAN_PTR)
  {
    format_type = SOC_PETRA_MULT_EG_FORMAT_C;
  }
  else
  {
    if (in_index_group+1 < mc_group_size &&
            mc_group[in_index_group+1].eg_entry.type == SOC_PETRA_MULT_EG_ENTRY_TYPE_VLAN_PTR)
    {
       format_type = SOC_PETRA_MULT_EG_FORMAT_B;
    }
  }

  return format_type;
}

uint8
  soc_petra_mc_is_next_null(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint16  next_ptr
  )
{
  uint8
    is_null;
  
  is_null = SOC_SAND_NUM2BOOL((next_ptr == soc_petra_mc_null_id(unit, FALSE)) || (next_ptr == soc_petra_mc_null_id(unit, TRUE)));

  return is_null;
}

/*********************************************************************
* Details: refer to H file.
**********************************************************************/
uint8
  soc_petra_mult_is_insufficient_memory(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  mc_group_size,
    SOC_SAND_IN  uint8                 is_ingress,
	  SOC_SAND_IN  uint32  		           nof_vlan_bitmaps
  )
{
  uint32
    nof_unoccupied,
    required_tbl_entries = 0;
  uint16
    entry_null_1_status,
    entry_null_2_status;
  uint32
    nof_ports;

  nof_unoccupied = soc_petra_sw_db_multicast_nof_unoccupied_get(unit);

  entry_null_1_status = soc_petra_sw_db_multicast_next_unoccupied_ptr_get(unit, SOC_PETRA_MC_NULL_ID_DFLT);
  
 
  /* in case that entry 0 of the mc-table is not occupied there is one
   * less unoccupied entries
   */
  if (entry_null_1_status != SOC_PETRA_SW_DB_MULTICAST_FIRST_ENTRY_INDICATOR)
  {
    nof_unoccupied = nof_unoccupied - 1;
  }
  
  if (SOC_PETRA_IS_DEV_PETRA_B_OR_ABOVE)
  {
    /* in case that entry NUL_ID_PB_INGR of the mc-table is not occupied there is one
    * less unoccupied entries
    */
    entry_null_2_status = soc_petra_sw_db_multicast_next_unoccupied_ptr_get(unit, SOC_PETRA_MC_NULL_ID_PB_INGR);
    if (entry_null_2_status != SOC_PETRA_SW_DB_MULTICAST_FIRST_ENTRY_INDICATOR)
    {
      nof_unoccupied = nof_unoccupied - 1;
    }
  }
  

  if (is_ingress)
  {
    required_tbl_entries =
      SOC_SAND_DIV_ROUND_UP(mc_group_size,SOC_PETRA_MULT_ING_NOF_DESTINATIONS_PER_ENTRY);
  }
  else
  {
    if (nof_vlan_bitmaps == 0)
    {
      required_tbl_entries =
        SOC_SAND_DIV_ROUND_UP(mc_group_size,SOC_PETRA_MULT_EG_NOF_PORTS_PER_ENTRY);
    }
    else
    {
      if (SOC_PETRA_IS_DEV_PETRA_B_OR_ABOVE)
      {
        /* Soc_petra-B only: entires including vlan bitmaps, size depends on nof vlan bitmap*/
        nof_ports = mc_group_size - nof_vlan_bitmaps;
        if (nof_ports % SOC_PETRA_MULT_EG_NOF_PORTS_PER_ENTRY == 0)
        {
          required_tbl_entries =
            SOC_SAND_DIV_ROUND_UP(nof_ports,SOC_PETRA_MULT_EG_FORMAT_A_PER_ENTRY)
            + SOC_SAND_DIV_ROUND_UP(nof_vlan_bitmaps,SOC_PETRA_MULT_EG_FORMAT_C_PER_ENTRY);
        }
        else if (nof_ports % SOC_PETRA_MULT_EG_NOF_PORTS_PER_ENTRY == 1)
        {
          required_tbl_entries =
            SOC_SAND_DIV_ROUND_UP(nof_ports-1,SOC_PETRA_MULT_EG_FORMAT_A_PER_ENTRY)
            + SOC_SAND_DIV_ROUND_UP(2,SOC_PETRA_MULT_EG_FORMAT_B_PER_ENTRY)
            + SOC_SAND_DIV_ROUND_UP(nof_vlan_bitmaps-1,SOC_PETRA_MULT_EG_FORMAT_C_PER_ENTRY);
        }
        else
        {
          required_tbl_entries =
            SOC_SAND_DIV_ROUND_UP(nof_ports-2,SOC_PETRA_MULT_EG_FORMAT_A_PER_ENTRY)
            + SOC_SAND_DIV_ROUND_UP(4,SOC_PETRA_MULT_EG_FORMAT_B_PER_ENTRY)
            + SOC_SAND_DIV_ROUND_UP(nof_vlan_bitmaps-1,SOC_PETRA_MULT_EG_FORMAT_C_PER_ENTRY);
        }
      }
    }
     
  }
  
  if (required_tbl_entries > nof_unoccupied)
  {
    return TRUE;
  }

  return FALSE;
}
/*********************************************************************
* Details: refer to H file.
**********************************************************************/
uint8
  soc_petra_mult_is_multicast_id_group_exist(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN uint32                   tbl_entry_id
  )
{
  uint16
    next_unoccupied_ptr;
  uint8
    is_first_entry = FALSE;

  next_unoccupied_ptr =
    soc_petra_sw_db_multicast_next_unoccupied_ptr_get(
      unit,
      tbl_entry_id
    );

  /*
   * If the 'next-unoccupied-table' software data-base is indicated to
   * be a first entry (or multicast-id) in the tbl_entry_id then the
   * function returns TRUE.
   */
  if (next_unoccupied_ptr == SOC_PETRA_SW_DB_MULTICAST_FIRST_ENTRY_INDICATOR)
  {
     is_first_entry = TRUE;
  }

  return is_first_entry;
}

/*********************************************************************
* Details: refer to H file.
**********************************************************************/
uint8
  soc_petra_mult_is_multicast_id_occupied(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN uint32                   tbl_entry_id
  )
{
  uint16
    next_unoccupied_ptr;
  uint8
    is_occupied = FALSE;

  next_unoccupied_ptr =
    soc_petra_sw_db_multicast_next_unoccupied_ptr_get(
      unit,
      tbl_entry_id
    );
  /*
   * If the 'next-unoccupied-table' software data-base is indicated to
   * be a ingress or egress occupied indicator then the
   * function returns TRUE.
   */
  if ((next_unoccupied_ptr == SOC_PETRA_SW_DB_MULTICAST_ING_OCCUPIED_ENTRY_INDICATOR)||
    (next_unoccupied_ptr == SOC_PETRA_SW_DB_MULTICAST_EG_OCCUPIED_ENTRY_INDICATOR))
  {
    is_occupied = TRUE;
  }

  return is_occupied;
}


/*********************************************************************
* Details: refer to H file.
**********************************************************************/
uint32
  soc_petra_mult_group_open_unsafe_joint(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_ID             multicast_id_ndx,
    SOC_SAND_IN  SOC_PETRA_MULT_ENTRY          *mc_group,
    SOC_SAND_IN  uint32                  mc_group_size,
    SOC_SAND_IN  uint8                 is_ingress,
    SOC_SAND_OUT uint8                 *insufficient_memory
  )
{
  uint32
    res;
  uint16
    tbl_entry_id;
  uint8
    mc_group_exist,
    mc_id_occupied,
    not_enough_mem_val;
  uint32
	  nof_vlan_bitmaps = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_ING_GROUP_OPEN_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(mc_group);
  SOC_SAND_CHECK_NULL_INPUT(insufficient_memory);

  if (is_ingress)
  {
    tbl_entry_id = (uint16)multicast_id_ndx;
  }
  else
  {
    /* if egress - concatenate with '1' at msb , and 3 ports per entry */
    tbl_entry_id = (uint16)(multicast_id_ndx | SOC_PETRA_MULT_LL_EGRESS_MC_ID_START_ENTRY);
  }

  nof_vlan_bitmaps =
    soc_petra_mult_nof_vlan_bitmaps_in_group_get(
      unit,
      &(mc_group->eg_entry),
      mc_group_size,
      is_ingress
    );

  not_enough_mem_val =
    soc_petra_mult_is_insufficient_memory(
      unit,
      mc_group_size,
      is_ingress,
      nof_vlan_bitmaps
    );

  if (not_enough_mem_val == TRUE)
  {
    *insufficient_memory = TRUE;
    SOC_PETRA_DO_NOTHING_AND_EXIT;
  }

  mc_group_exist =
    soc_petra_mult_is_multicast_id_group_exist(
      unit,
      tbl_entry_id
    );

  if (mc_group_exist == TRUE)
  {
    res = soc_petra_mult_group_update_unsafe_joint(
            unit,
            multicast_id_ndx,
            mc_group,
            mc_group_size,
            is_ingress,
            insufficient_memory
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }
  else
  {
    mc_id_occupied =
      soc_petra_mult_is_multicast_id_occupied(
        unit,
        tbl_entry_id
      );

    if (mc_id_occupied == TRUE)
    {
      /* Re-allocation */

      res = soc_petra_mult_multicast_id_relocation(
              unit,
              tbl_entry_id
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    }

    res = soc_petra_mult_multicast_group_input(
            unit,
            tbl_entry_id,
            mc_group,
            mc_group_size,
            is_ingress
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    *insufficient_memory = not_enough_mem_val;
  }

#ifdef LINK_PB_LIBRARIES
  if (SOC_PETRA_IS_DEV_PETRA_B_OR_ABOVE)
  {
   if (!not_enough_mem_val && !is_ingress)
   {
      soc_pb_sw_db_multicast_eg_mult_nof_vlan_bitmaps_set(
        unit,
        multicast_id_ndx,
        nof_vlan_bitmaps
      );
   }
  }
#endif
  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_group_open_unsafe_joint()",0,0);
}

/*********************************************************************
* Details: refer to H file.
**********************************************************************/
uint32
  soc_petra_mult_multicast_group_input_first_entry_db_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint16                  tbl_entry_id
  )
{
  uint16
    prev_unoccupied_ptr,
    next_unoccupied_ptr;

  /* Change previous unoccupied ptr to the next.next of this mc-id */

  next_unoccupied_ptr = soc_petra_sw_db_multicast_next_unoccupied_ptr_get(
                          unit,
                          tbl_entry_id
                        );

  prev_unoccupied_ptr = soc_petra_sw_db_multicast_backwards_ptr_get(
                          unit,
                          tbl_entry_id
                        );
  if (prev_unoccupied_ptr != SOC_PETRA_SW_DB_MULTICAST_LISTS_NULL_POINTER)
  {
    soc_petra_sw_db_multicast_next_unoccupied_ptr_set(
      unit,
      prev_unoccupied_ptr,
      next_unoccupied_ptr
    );
  }

  if ((next_unoccupied_ptr != SOC_PETRA_SW_DB_MULTICAST_LISTS_NULL_POINTER)&&
    (next_unoccupied_ptr != SOC_PETRA_SW_DB_MULTICAST_FIRST_ENTRY_INDICATOR))
  {
    soc_petra_sw_db_multicast_backwards_ptr_set(
      unit,
      next_unoccupied_ptr,
      prev_unoccupied_ptr
    );
  }

  /* Turn to first entry in SW */
  soc_petra_sw_db_multicast_next_unoccupied_ptr_set(
    unit,
    tbl_entry_id,
    SOC_PETRA_SW_DB_MULTICAST_FIRST_ENTRY_INDICATOR
    );

  /* update occupied backwards ptr */
  soc_petra_sw_db_multicast_backwards_ptr_set(
    unit,
    tbl_entry_id,
    SOC_PETRA_SW_DB_MULTICAST_LISTS_NULL_POINTER
    );

  if (soc_petra_sw_db_multicast_unoccupied_list_head_get(unit) ==
    tbl_entry_id)
  {
    soc_petra_sw_db_multicast_unoccupied_list_head_set(
      unit,
      next_unoccupied_ptr);
  }

  return SOC_SAND_OK;
}


/*********************************************************************
* Details: refer to H file.
**********************************************************************/
uint32
  soc_petra_mult_multicast_group_input_middle_entry_db_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint16                  prev_entry,
    SOC_SAND_IN  uint8                 is_ingress,
    SOC_SAND_OUT uint16                  *entry
  )
{
  uint16
    unoccupied_list_head,
    unoccupied_first,
    next_unoccupied_first,
    next_unoccupied_list_head,
    occupied_type;

  /* get first unoccupied */
  unoccupied_list_head =
    soc_petra_sw_db_multicast_unoccupied_list_head_get(unit);

  next_unoccupied_list_head = soc_petra_sw_db_multicast_next_unoccupied_ptr_get(
    unit,
    unoccupied_list_head
    );
  if (soc_petra_mc_is_next_null(unit, unoccupied_list_head))
  {
    
    if (soc_petra_mc_is_next_null(unit, next_unoccupied_list_head))
    {
      unoccupied_first = soc_petra_sw_db_multicast_next_unoccupied_ptr_get(unit, next_unoccupied_list_head);
    }
    else
    {
      unoccupied_first = next_unoccupied_list_head;
    }
  }
  else
  {
    unoccupied_first = unoccupied_list_head;
  }

  /* get next to first unoccupied - to progress the head */
  next_unoccupied_first = soc_petra_sw_db_multicast_next_unoccupied_ptr_get(
                unit,
                unoccupied_first
              );

  /* set unoccupied to occupied */
  occupied_type = (is_ingress ?
      SOC_PETRA_SW_DB_MULTICAST_ING_OCCUPIED_ENTRY_INDICATOR :
      SOC_PETRA_SW_DB_MULTICAST_EG_OCCUPIED_ENTRY_INDICATOR
    );
  soc_petra_sw_db_multicast_next_unoccupied_ptr_set(
    unit,
    unoccupied_first,
    occupied_type
  );

  /* point occupied to previous entry in mc_group list */
  soc_petra_sw_db_multicast_backwards_ptr_set(
    unit,
    unoccupied_first,
    prev_entry
  );

  /* progress head of unoccupied list */
  if (!soc_petra_mc_is_next_null(unit, unoccupied_list_head))
  {
    soc_petra_sw_db_multicast_unoccupied_list_head_set(
      unit,
      next_unoccupied_first
    );
    if (next_unoccupied_first != SOC_PETRA_SW_DB_MULTICAST_LISTS_NULL_POINTER)
    {
      soc_petra_sw_db_multicast_backwards_ptr_set(
        unit,
        next_unoccupied_first,
        SOC_PETRA_SW_DB_MULTICAST_LISTS_NULL_POINTER
      );
    }
  }
  else
  {
    if (soc_petra_mc_is_next_null(unit, next_unoccupied_list_head))
    {
      /* next unoccupied might come after the next unoccupied list head (if it is null) */
      soc_petra_sw_db_multicast_next_unoccupied_ptr_set(
        unit,
        next_unoccupied_list_head,
        next_unoccupied_first
        );

      if (next_unoccupied_first != SOC_PETRA_SW_DB_MULTICAST_LISTS_NULL_POINTER)
      {
        soc_petra_sw_db_multicast_backwards_ptr_set(
          unit,
          next_unoccupied_first,
          next_unoccupied_list_head
          );
      }

    }
    else
    {
      soc_petra_sw_db_multicast_next_unoccupied_ptr_set(
        unit,
        unoccupied_list_head,
        next_unoccupied_first
        );

      if (next_unoccupied_first != SOC_PETRA_SW_DB_MULTICAST_LISTS_NULL_POINTER)
      {
        soc_petra_sw_db_multicast_backwards_ptr_set(
          unit,
          next_unoccupied_first,
          unoccupied_list_head
          );
      }
    }
    

     
  }

  *entry = unoccupied_first;

  return SOC_SAND_OK;
}

/*********************************************************************
* Details: refer to H file.
**********************************************************************/
void
  soc_petra_mult_multicast_id_as_unoccupied_head_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint16                  tbl_entry_id
  )
{
  uint16
    unoccupied_list_head;

  /* get first unoccupied */
  unoccupied_list_head =
    soc_petra_sw_db_multicast_unoccupied_list_head_get(unit);

  /* set backwards of old-head to new-head */
  soc_petra_sw_db_multicast_backwards_ptr_set(
    unit,
    unoccupied_list_head,
    tbl_entry_id
  );

  /* set next unoccupied of new-head to old-head */
  soc_petra_sw_db_multicast_next_unoccupied_ptr_set(
    unit,
    tbl_entry_id,
    unoccupied_list_head
  );

  /* set backwards ptr of old-head to NULL */
  soc_petra_sw_db_multicast_backwards_ptr_set(
    unit,
    tbl_entry_id,
    SOC_PETRA_SW_DB_MULTICAST_LISTS_NULL_POINTER
  );

  /* set head to new-head */
  soc_petra_sw_db_multicast_unoccupied_list_head_set(
    unit,
    tbl_entry_id
  );
}


/*********************************************************************
* Details: refer to H file.
**********************************************************************/
uint32
  soc_petra_mult_multicast_group_entry_to_tbl(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 in_group_index,
    SOC_SAND_IN  uint32                  offset,
    SOC_SAND_IN  SOC_PETRA_MULT_ENTRY          *mc_group,
    SOC_SAND_IN  uint32                  mc_group_size,
    SOC_SAND_IN  uint8                 is_ingress,
    SOC_SAND_IN  uint16                  next_entry
  )
{
  uint32
    res;
  SOC_PETRA_MULT_EG_FORMAT_TYPE
    format_type;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_MULTICAST_GROUP_ENTRY_TO_TBL);

  SOC_SAND_CHECK_NULL_INPUT(mc_group);

  if (is_ingress)
  {
    res = soc_petra_mult_ing_multicast_group_entry_to_tbl(
            unit,
            in_group_index,
            offset,
            mc_group,
            mc_group_size,
            next_entry
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }
  else
  {
    format_type =
      soc_petra_mult_eg_format_entry_type_get(
        unit,
        mc_group,
        mc_group_size,
        in_group_index
      );
    
    if (format_type == SOC_PETRA_MULT_EG_FORMAT_A)
    {
      /* Multicast egress format default */
      res = soc_petra_mult_eg_multicast_group_entry_to_tbl(
          unit,
          in_group_index,
          offset,
          mc_group,
          mc_group_size,
          next_entry
        );
    }
#ifdef LINK_PB_LIBRARIES
    else
    {
      if (SOC_PETRA_IS_DEV_PETRA_B_OR_ABOVE)
      {
        res = soc_pb_mult_eg_multicast_group_entry_to_tbl(
          unit,
          in_group_index,
          offset,
          mc_group,
          mc_group_size,
          format_type,
          next_entry
          );
      }
      else
      {
        /* Illegal entry for Soc_petra A */
        SOC_SAND_SET_ERROR_CODE(SOC_PETRA_MULT_LL_ILLEGAL_ENTRY_ERR, 20, exit);
      }
      
    }
#endif
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_multicast_group_entry_to_tbl()",0,0);
}




/*********************************************************************
* Details: refer to H file.
**********************************************************************/
uint32
  soc_petra_mult_multicast_group_input(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint16                  tbl_entry_id,
    SOC_SAND_IN  SOC_PETRA_MULT_ENTRY          *mc_group,
    SOC_SAND_IN  uint32                  mc_group_size,
    SOC_SAND_IN  uint8                 is_ingress
  )
{
  uint32
    res,
    nof_unoccupied;
  uint16
    next_entry,
    prev_entry,
    second_entry = 0,	
    tbl_entry_id_var;
  uint32
    mc_id,
    in_group_i = 0,
    progress_index_by;
  SOC_PETRA_MULT_EG_FORMAT_TYPE
    format_type;


  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_MULTICAST_GROUP_INPUT);

  SOC_SAND_CHECK_NULL_INPUT(mc_group);
  tbl_entry_id_var = tbl_entry_id;
  mc_id = SOC_PETRA_MULT_ID_FROM_LL_TBL_ID(tbl_entry_id);

  if (is_ingress)
  {
    progress_index_by = SOC_PETRA_MULT_ING_NOF_DESTINATIONS_PER_ENTRY;
  }
  else
  {
    /* if egress - entry size depends on entry type */
    format_type =
      soc_petra_mult_eg_format_entry_type_get(
        unit,
        mc_group,
        mc_group_size,
        0
      );
    
    progress_index_by =
      soc_petra_mult_eg_progress_per_entry_format_get(
        unit,
        format_type
      );
  }
 

  /* In case size==0 ('occupied but empty') so we will enter loop: */
  if (mc_group_size == 0)
  {
    /* Handle First Entry { */
    soc_petra_mult_multicast_group_input_first_entry_db_set(
        unit,
        tbl_entry_id_var
      );

    /* Fill in 'occupied but empty' tbl entry. */
    res = soc_petra_mult_rplct_tbl_entry_occupied_but_empty_set(
            unit,
            tbl_entry_id_var,
            is_ingress
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

     /* reduce unoccupied_variable by 1 */
    nof_unoccupied = soc_petra_sw_db_multicast_nof_unoccupied_get(unit);
    soc_petra_sw_db_multicast_nof_unoccupied_set(
      unit,
      (nof_unoccupied - 1)
    );
  }
  else
  {
    prev_entry = tbl_entry_id_var;

    for ( in_group_i = 0 ;
        in_group_i < mc_group_size;
        in_group_i += progress_index_by)
    {

      /* calculate new progress */
      if (!is_ingress)
      {
        format_type =
          soc_petra_mult_eg_format_entry_type_get(
          unit,
          mc_group,
          mc_group_size,
          in_group_i
          );

        progress_index_by =
          soc_petra_mult_eg_progress_per_entry_format_get(
          unit,
          format_type
          );
      }

      if (in_group_i == 0)
      {
        /* Handle First Entry { */
        soc_petra_mult_multicast_group_input_first_entry_db_set(
          unit,
          tbl_entry_id_var
        );
      }
      else
      {
        /* Handle Middle Entries { */

        soc_petra_mult_multicast_group_input_middle_entry_db_set(
          unit,
          prev_entry,
          is_ingress,
          &tbl_entry_id_var
          );
      }
      if ((in_group_i + progress_index_by) < mc_group_size)
      {
        next_entry = soc_petra_sw_db_multicast_unoccupied_list_head_get(unit);
        if(soc_petra_mc_is_next_null(unit,next_entry))
        {
          next_entry = soc_petra_sw_db_multicast_next_unoccupied_ptr_get(
            unit,
            next_entry
            );
          if(soc_petra_mc_is_next_null(unit,next_entry))
          {
            if (SOC_PETRA_IS_DEV_PETRA_A)
            {
              SOC_SAND_SET_ERROR_CODE(SOC_PETRA_MULT_LL_ENTRY_NULL_CANT_BE_IN_MIDDLE_OF_LINK_LIST_ERR,19, exit);
            }
            else
            {
              /* next entry could be entry null : SOC_PETRA_MC_NULL_ID_PB_INGR or SOC_PETRA_MC_NULL_ID_DFLT */
              next_entry = soc_petra_sw_db_multicast_next_unoccupied_ptr_get(
                unit,
                next_entry
                );

              if(soc_petra_mc_is_next_null(unit,next_entry))
              {
                SOC_SAND_SET_ERROR_CODE(SOC_PETRA_MULT_LL_ENTRY_NULL_CANT_BE_IN_MIDDLE_OF_LINK_LIST_ERR,19, exit);
              }
            }
          }
        }
      }
      else
      {
        next_entry = soc_petra_mc_null_id(unit, is_ingress);
      }
     
      /* 
       * Process of commit is done in two stages:
       * First, allocate new entries range: second-end.
       * Second, commit first entry and point it to the new linked list.
       * This is done to avoid traffic hit.
       */

      /* First stage: Commit in HW only if not first entry */
      if (in_group_i > 0) 
      {
        
        res = soc_petra_mult_multicast_group_entry_to_tbl(
                unit,
                in_group_i,
                tbl_entry_id_var,
                mc_group,
                mc_group_size,
                is_ingress,
                next_entry
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
      }
      else 
      {
        /* in_group_i < progress_index_by, i.e. first entry */
        
        /* 
         *Save second entry for later use in 
         * second stage of HW commit
         */
        second_entry = next_entry;		
      }
      

      prev_entry = tbl_entry_id_var;


      /* reduce unoccupied_variable by 1 */
      nof_unoccupied = soc_petra_sw_db_multicast_nof_unoccupied_get(unit);
      soc_petra_sw_db_multicast_nof_unoccupied_set(
        unit,
        (nof_unoccupied - 1)
      );

      
   }

   /* 
    * Second stage: Commit in HW first entry.
    * Get next entry from sw db (that already updated)
    */
   in_group_i = 0;
   tbl_entry_id_var = tbl_entry_id;

   res = soc_petra_mult_multicast_group_entry_to_tbl(
     unit,
     in_group_i,
     tbl_entry_id_var,
     mc_group,
     mc_group_size,
     is_ingress,
     second_entry
     );
   SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  
  }/* MC group size == 0 */

  


  /* Set is_ingress_replication HW table at MC-ID - TRUE if ingress */
  if(is_ingress)
  {
    res = soc_petra_mult_multicast_ingress_replication_hw_table_set(
      unit,
      mc_id,
      TRUE
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit)
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_multicast_group_input()",0,0);
}

/*********************************************************************
* Details: refer to H file.
**********************************************************************/
uint32
  soc_petra_mult_multicast_id_relocation(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint16                  tbl_entry_id
  )
{
  uint32
    res;
  uint16
    next_unoccupied_ptr,
    unoccupied_id,
    prev_entry,
    next_entry;
  uint8
    occupied_type = FALSE;
  uint32
    mc_group_size;
  SOC_PETRA_MULT_ENTRY
    mc_group[SOC_PETRA_MULT_EG_NOF_PORTS_PER_ENTRY];

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_MULTICAST_ID_RELOCATION);

  next_unoccupied_ptr = soc_petra_sw_db_multicast_next_unoccupied_ptr_get(
                          unit,
                          tbl_entry_id
                        );

  if (next_unoccupied_ptr ==
     SOC_PETRA_SW_DB_MULTICAST_ING_OCCUPIED_ENTRY_INDICATOR)
  {
    /*ingress entry*/
     occupied_type = TRUE;
  }
  else
    if (next_unoccupied_ptr ==
     SOC_PETRA_SW_DB_MULTICAST_EG_OCCUPIED_ENTRY_INDICATOR)
    {
      /*egress entry*/
        occupied_type = FALSE;
    }
    else
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_MULT_LL_ILLEGAL_ENTRY_FOR_RELOCATION_ERR, 10, exit);
    }

  res = soc_petra_mult_entry_content_get(
          unit,
          tbl_entry_id,
          occupied_type,
          mc_group,
          &mc_group_size,
          &next_entry
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  prev_entry = soc_petra_sw_db_multicast_backwards_ptr_get(
                 unit,
                 tbl_entry_id
               );

  res = soc_petra_mult_multicast_group_input_middle_entry_db_set(
          unit,
          prev_entry,
          occupied_type,
          &unoccupied_id
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  /* the same for ingress and egress - pointer occupies the same bits in table */
  res = soc_petra_mult_ing_next_link_list_ptr_set(
          unit,
          prev_entry,
          unoccupied_id
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  soc_petra_sw_db_multicast_backwards_ptr_set(
    unit,
    next_entry,
    unoccupied_id
  );

  res = soc_petra_mult_multicast_group_entry_to_tbl(
          unit,
          0,
          unoccupied_id,
          mc_group,
          mc_group_size,
          occupied_type,
          next_entry
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  soc_petra_mult_multicast_id_as_unoccupied_head_set(
    unit,
    tbl_entry_id
  );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_multicast_id_relocation()",0,0);
}



/*********************************************************************
* Details: refer to H file.
**********************************************************************/
uint32
  soc_petra_mult_entry_content_get(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  SOC_PETRA_MULT_ID            multicast_id_ndx,
    SOC_SAND_IN  uint8                is_ingress,
    SOC_SAND_OUT SOC_PETRA_MULT_ENTRY         *mc_group,
    SOC_SAND_OUT uint32                *mc_group_size,
    SOC_SAND_OUT uint16                 *next_entry
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_ENTRY_CONTENT_GET);

  SOC_SAND_CHECK_NULL_INPUT(mc_group);
  SOC_SAND_CHECK_NULL_INPUT(mc_group_size);
  SOC_SAND_CHECK_NULL_INPUT(next_entry);

  if (is_ingress)
  {
    res = soc_petra_mult_ing_entry_content_get(
                unit,
                multicast_id_ndx,
                mc_group,
                mc_group_size,
                next_entry
              );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }
  else
  {
    res = soc_petra_mult_eg_entry_content_get(
                unit,
                multicast_id_ndx,
                mc_group,
                mc_group_size,
                next_entry
              );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_entry_content_get()",0,0);
}

/*********************************************************************
* Details: refer to H file.
**********************************************************************/
uint32
  soc_petra_mult_group_update_unsafe_joint(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_ID             multicast_id_ndx,
    SOC_SAND_IN  SOC_PETRA_MULT_ENTRY          *mc_group,
    SOC_SAND_IN  uint32                  mc_group_size,
    SOC_SAND_IN  uint8                 is_ingress,
    SOC_SAND_OUT uint8                 *insufficient_memory
  )
{
  uint32
    res;
  uint16
    tbl_entry_id=0,
    old_list_ptr;
  uint8
    mc_group_exist,
    not_enough_mem_val;
  uint32
    nof_vlan_bitmaps = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_GROUP_UPDATE_UNSAFE_JOINT);

  SOC_SAND_CHECK_NULL_INPUT(mc_group);
  SOC_SAND_CHECK_NULL_INPUT(insufficient_memory);

  if (is_ingress)
  {
    tbl_entry_id = (uint16)multicast_id_ndx;
  }
  else
  {
    /* if egress - concatenate with '1' at msb , and 3 ports per entry */
    tbl_entry_id = (uint16)(multicast_id_ndx | SOC_PETRA_MULT_LL_EGRESS_MC_ID_START_ENTRY);
  }

  mc_group_exist =
    soc_petra_mult_is_multicast_id_group_exist(
      unit,
      tbl_entry_id
    );

  if (!mc_group_exist)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_MULT_ENTRY_DOES_NOT_EXIST, 10, exit);
  }

  nof_vlan_bitmaps =
    soc_petra_mult_nof_vlan_bitmaps_in_group_get(
      unit,
      &(mc_group->eg_entry),
      mc_group_size,
      is_ingress
    );

  not_enough_mem_val =
    soc_petra_mult_is_insufficient_memory(
      unit,
      mc_group_size,
      is_ingress,
      nof_vlan_bitmaps
    );

  if (not_enough_mem_val == TRUE)
  {
    *insufficient_memory = TRUE;
    SOC_PETRA_DO_NOTHING_AND_EXIT;
  }

  res = soc_petra_mult_link_list_ptr_get(
          unit,
          tbl_entry_id,
          &old_list_ptr
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_mult_multicast_group_input(
          unit,
          tbl_entry_id,
          mc_group,
          mc_group_size,
          is_ingress
        );
  SOC_SAND_CHECK_FUNC_RESULT(res,20,exit);

  /* for first entry input */

  soc_petra_sw_db_multicast_nof_unoccupied_set(
    unit,
    (soc_petra_sw_db_multicast_nof_unoccupied_get(unit) + 1)
  );

  /* erase previous mc-group entries, only if next pointer not null */
  if (soc_petra_mc_null_id(unit,is_ingress) != old_list_ptr)
  {
    res = soc_petra_mult_erase_multicast_group(
      unit,
      old_list_ptr,
      is_ingress
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  }
  

  *insufficient_memory = not_enough_mem_val;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_group_update_unsafe_joint()",multicast_id_ndx,tbl_entry_id);
}

/*********************************************************************
* Details: refer to H file.
**********************************************************************/
uint32
 soc_petra_mult_link_list_ptr_get(
   SOC_SAND_IN  int           unit,
   SOC_SAND_IN  uint16            tbl_entry_id,
   SOC_SAND_OUT uint16            *link_list_ptr
 )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_LINK_LIST_PTR_GET);

  SOC_SAND_CHECK_NULL_INPUT(link_list_ptr);

  res = soc_petra_mult_ing_next_link_list_ptr_get(
          unit,
          tbl_entry_id,
          link_list_ptr
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_link_list_ptr_get()",0,0);
}

/*********************************************************************
* Details: refer to H file.
**********************************************************************/
uint32
  soc_petra_mult_erase_multicast_group(
    SOC_SAND_IN int           unit,
    SOC_SAND_IN uint16            tbl_entry_id,
    SOC_SAND_IN uint8           is_ingress
  )
{
  uint32
    res;
  uint16
    next_link_list_ptr,
    tbl_entry_id_i = tbl_entry_id;
  uint8
    first_time = TRUE;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_ERASE_MULTICAST_GROUP);
  
  /* Always allow first entry to erase even if it's null entry id */
  while (tbl_entry_id_i != soc_petra_mc_null_id(unit, is_ingress) || first_time)
  {
    first_time = FALSE;

    res = soc_petra_mult_link_list_ptr_get(
            unit,
            tbl_entry_id_i,
            &next_link_list_ptr
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
   
    res = soc_petra_mult_erase_one_entry(
            unit,
            tbl_entry_id_i,
            is_ingress
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    
    tbl_entry_id_i = next_link_list_ptr;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_erase_multicast_group()",0,0);
}

/*********************************************************************
* Details: refer to H file.
**********************************************************************/
uint32
  soc_petra_mult_erase_one_entry(
    SOC_SAND_IN int           unit,
    SOC_SAND_IN uint16            tbl_entry_id,
    SOC_SAND_IN uint8           is_ingress
  )
{
  uint32
    res;
  uint16
    unoccupied_head;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_ERASE_ONE_ENTRY);

  unoccupied_head =
    soc_petra_sw_db_multicast_unoccupied_list_head_get(unit);

  if (unoccupied_head == SOC_PETRA_SW_DB_MULTICAST_LISTS_NULL_POINTER)
  {
    soc_petra_sw_db_multicast_unoccupied_list_head_set(
      unit,
      tbl_entry_id
    );
  }
  else
  {
    soc_petra_sw_db_multicast_backwards_ptr_set(
      unit,
      unoccupied_head,
      tbl_entry_id
    );
  }

  soc_petra_sw_db_multicast_next_unoccupied_ptr_set(
    unit,
    tbl_entry_id,
    unoccupied_head
  );

  soc_petra_sw_db_multicast_backwards_ptr_set(
    unit,
    tbl_entry_id,
    SOC_PETRA_SW_DB_MULTICAST_LISTS_NULL_POINTER
  );

  soc_petra_sw_db_multicast_unoccupied_list_head_set(
    unit,
    tbl_entry_id
    );

  soc_petra_sw_db_multicast_nof_unoccupied_set(
    unit,
    (soc_petra_sw_db_multicast_nof_unoccupied_get(unit) + 1)
  );

  /*it doesn't matter if it is ing/eg writing same thing to HW*/
  res = soc_petra_mult_rplct_tbl_entry_unoccupied_set(
          unit,
          tbl_entry_id,
          is_ingress
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_erase_one_entry()",0,0);
}

/*********************************************************************
* Details: refer to H file.
**********************************************************************/
uint32
  soc_petra_mult_group_close_unsafe_joint(
    SOC_SAND_IN int               unit,
    SOC_SAND_IN SOC_PETRA_MULT_ID           multicast_id_ndx,
    SOC_SAND_IN uint8               is_ingress
  )
{
  uint32
    res;
  uint16
    tbl_entry_id;
  uint8
    mc_group_exist;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_GROUP_CLOSE_UNSAFE_JOINT);

  if (is_ingress)
  {
    tbl_entry_id = (uint16)multicast_id_ndx;
  }
  else
  {
    /* if egress - concatenate with '1' at msb , and 3 ports per entry */
    tbl_entry_id = (uint16)(multicast_id_ndx | SOC_PETRA_MULT_LL_EGRESS_MC_ID_START_ENTRY);
  }

  mc_group_exist =
    soc_petra_mult_is_multicast_id_group_exist(
      unit,
      tbl_entry_id
    );

  if (mc_group_exist == TRUE)
  {
    res = soc_petra_mult_erase_multicast_group(
            unit,
            tbl_entry_id,
            is_ingress
            );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    if (is_ingress)
    {
      /* reset HW table- is ingress replication*/
      res = soc_petra_mult_multicast_ingress_replication_hw_table_set(
              unit,
              tbl_entry_id,
              FALSE
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    }
    /* Else - the egress does not have a HW table to assert */
#ifdef LINK_PB_LIBRARIES
    else
    {
      if (SOC_PETRA_IS_DEV_PETRA_B_OR_ABOVE)
      {
        /* Reset nof vlan bitmaps to 0 */
        soc_pb_sw_db_multicast_eg_mult_nof_vlan_bitmaps_set(
          unit,
          multicast_id_ndx,
          0
        );
      }
    }
#endif
  }
/* $Id: petra_multicast_linked_list.c,v 1.8 Broadcom SDK $
 *  if the mc-group does not exist then the close operation
 *  does not need doing.
 */

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_group_close_unsafe_joint()",0,0);
}

/*********************************************************************
* Details: refer to H file.
**********************************************************************/
uint32
  soc_petra_mult_destination_add_unsafe_joint(
    SOC_SAND_IN int                unit,
    SOC_SAND_IN SOC_PETRA_MULT_ID            multicast_id_ndx,
    SOC_SAND_IN SOC_PETRA_MULT_ENTRY          *entry,
    SOC_SAND_IN uint8                is_ingress,
    SOC_SAND_OUT uint8                *insufficient_memory
  )
{
  uint32
    res;
  uint16
    tbl_entry_id,
    ptr_to_last_entry;
  uint8
    success,
    not_enough_mem_val;
  uint32
    nof_vlan_bitmaps = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_ING_DESTINATION_ADD_UNSAFE_INNER);

  SOC_SAND_CHECK_NULL_INPUT(entry);
  SOC_SAND_CHECK_NULL_INPUT(insufficient_memory);

  if (is_ingress)
  {
    tbl_entry_id = (uint16)multicast_id_ndx;
  }
  else
  {
    /* if egress - concatenate with '1' at msb , and 3 ports per entry */
    tbl_entry_id = (uint16)(multicast_id_ndx | SOC_PETRA_MULT_LL_EGRESS_MC_ID_START_ENTRY);
  }

  nof_vlan_bitmaps =
    soc_petra_mult_nof_vlan_bitmaps_in_group_get(
      unit,
      &(entry->eg_entry),
      1,
      is_ingress
    );
  
 not_enough_mem_val =
    soc_petra_mult_is_insufficient_memory(
      unit,
      1,
      is_ingress,
      nof_vlan_bitmaps
    );

  if (not_enough_mem_val == TRUE)
  {
    *insufficient_memory = TRUE;
    goto exit;
  }


  /* find last entry of link list mc-group in table */
  res = soc_petra_mult_last_entry_in_list_get(
          unit,
          tbl_entry_id,
          &ptr_to_last_entry
        );

  /* check if there's room in last entry:
   *    yes - enter
   *    no - open new entry
   */

  res = soc_petra_mult_fill_in_last_entry(
            unit,
            ptr_to_last_entry,
            entry,
            is_ingress,
            &success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);


  if (!success)
  {
    res = soc_petra_mult_add_entry_in_end_of_link_list(
            unit,
            ptr_to_last_entry,
            entry,
            is_ingress
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }
  
#ifdef LINK_PB_LIBRARIES
  /* Update nof vlan bitmaps */
  if (SOC_PETRA_IS_DEV_PETRA_B_OR_ABOVE)
  {
    if (!not_enough_mem_val && !is_ingress && entry->eg_entry.type == SOC_PETRA_MULT_EG_ENTRY_TYPE_VLAN_PTR)
    {
      soc_pb_sw_db_multicast_eg_mult_nof_vlan_bitmaps_set(
        unit,
        multicast_id_ndx,
        soc_pb_sw_db_multicast_eg_mult_nof_vlan_bitmaps_get(unit,multicast_id_ndx) + 1
      );
    }
  }
#endif
  *insufficient_memory = not_enough_mem_val;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_ing_destination_add_unsafe_inner()",0,0);
}

/*********************************************************************
* Details: refer to H file.
**********************************************************************/
uint32
  soc_petra_mult_last_entry_in_list_get(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint16                 tbl_entry_id,
    SOC_SAND_OUT uint16                 *ptr_to_last_entry
  )
{
  uint32
    res;
  uint16
    next_link_list_ptr,
    tbl_entry_id_i = tbl_entry_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_LAST_ENTRY_IN_LIST_GET);

  SOC_SAND_CHECK_NULL_INPUT(ptr_to_last_entry);

  res = soc_petra_mult_link_list_ptr_get(
          unit,
          tbl_entry_id_i,
          &next_link_list_ptr
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  while (soc_petra_mc_is_next_null(unit, next_link_list_ptr) == FALSE)
  {
    tbl_entry_id_i = next_link_list_ptr;

    res = soc_petra_mult_link_list_ptr_get(
            unit,
            tbl_entry_id_i,
            &next_link_list_ptr
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }

  *ptr_to_last_entry = tbl_entry_id_i;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_last_entry_in_list_get()",0,0);

}

/*********************************************************************
* Details: refer to H file.
**********************************************************************/
uint32
  soc_petra_mult_add_entry_in_end_of_link_list(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint16                 tbl_entry_id,
    SOC_SAND_IN  SOC_PETRA_MULT_ENTRY         *entry,
    SOC_SAND_IN  uint8                is_ingress
  )
{
  uint32
    res,
    nof_unoccupied;
  uint16
    entry_id;
  uint8
    success;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_ADD_ENTRY_IN_END_OF_LINK_LIST);

  SOC_SAND_CHECK_NULL_INPUT(entry);

  soc_petra_mult_multicast_group_input_middle_entry_db_set(
    unit,
    tbl_entry_id,
    is_ingress,
    &entry_id
  );

  /* reduce unoccupied_variable by 1 */
  nof_unoccupied = soc_petra_sw_db_multicast_nof_unoccupied_get(unit);
  soc_petra_sw_db_multicast_nof_unoccupied_set(
    unit,
    (nof_unoccupied - 1)
    );

  res = soc_petra_mult_ing_next_link_list_ptr_set(
          unit,
          tbl_entry_id,
          entry_id
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_petra_mult_fill_in_last_entry(
          unit,
          entry_id,
          entry,
          is_ingress,
          &success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_add_entry_in_end_of_link_list()",0,0);
}

/*********************************************************************
* Details: refer to H file.
**********************************************************************/
uint32
  soc_petra_mult_fill_in_last_entry(
    SOC_SAND_IN int               unit,
    SOC_SAND_IN uint16                tbl_entry_id,
    SOC_SAND_IN SOC_PETRA_MULT_ENTRY        *entry,
    SOC_SAND_IN uint8               is_ingress,
    SOC_SAND_OUT uint8               *success
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_FILL_IN_LAST_ENTRY);

  SOC_SAND_CHECK_NULL_INPUT(entry);
  SOC_SAND_CHECK_NULL_INPUT(success);

  if (is_ingress)
  {
    res = soc_petra_mult_ing_fill_in_last_entry(
            unit,
            tbl_entry_id,
            entry,
            success
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }
  else
  {
    if (SOC_PETRA_IS_DEV_PETRA_A)
    {
      res = soc_petra_mult_eg_fill_in_last_entry(
        unit,
        tbl_entry_id,
        entry,
        success
        );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    }
#ifdef LINK_PB_LIBRARIES
    if (SOC_PETRA_IS_DEV_PETRA_B_OR_ABOVE)
    {
      res = soc_pb_mult_eg_fill_in_last_entry(
        unit,
        tbl_entry_id,
        entry,
        success
        );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    }
#endif
    
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_fill_in_last_entry()",0,0);
}

uint32
  soc_petra_mult_progress_index_get(
    SOC_SAND_IN  int      unit,
    SOC_SAND_IN  SOC_PETRA_MULT_ID  multicast_id_ndx,
    SOC_SAND_IN  uint16       tbl_entry_id,
    SOC_SAND_IN  uint8      is_ingress,
    SOC_SAND_OUT uint32      *progress_index_by
  )
{
#ifdef LINK_PB_LIBRARIES
  uint32
    res;
  SOC_PETRA_MULT_EG_FORMAT_TYPE
    entry_format;
  uint32
    nof_vlan_bitmaps;
#endif

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_EG_PROGRESS_INDEX_GET);

  SOC_SAND_CHECK_NULL_INPUT(progress_index_by);

  if (is_ingress)
  {
    *progress_index_by = SOC_PETRA_MULT_ING_NOF_DESTINATIONS_PER_ENTRY;
  }
  else
  {
    /* if egress - 3 ports per entry in Soc_petraA */
    *progress_index_by = SOC_PETRA_MULT_EG_NOF_PORTS_PER_ENTRY;
  }

#ifdef LINK_PB_LIBRARIES
  if (SOC_PETRA_IS_DEV_PETRA_B_OR_ABOVE)
  {
    /* Calculate progress index in soc_petra b */
    if (!is_ingress)
    {
      /* progress index by depends on entry format */
      nof_vlan_bitmaps =
        soc_pb_sw_db_multicast_eg_mult_nof_vlan_bitmaps_get(
        unit,
        multicast_id_ndx
      );
      if (nof_vlan_bitmaps != 0)
      {
        res = soc_pb_mult_eg_tbl_entry_format_get(
                unit,
                tbl_entry_id,
                &entry_format
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

        *progress_index_by =
          soc_petra_mult_eg_progress_per_entry_format_get(
            unit,
            entry_format
          );
      }
    }
  }
#endif

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_eg_progress_index_get()",0,0);
}

/*********************************************************************
* Details: refer to H file.
**********************************************************************/
uint32
  soc_petra_mult_group_size_get_unsafe_joint(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_ID             multicast_id_ndx,
    SOC_SAND_IN  uint8                 is_ingress,
    SOC_SAND_OUT uint32                  *mc_group_size
  )
{
  uint32
    res;
  uint16
    tbl_entry_id = (uint16)multicast_id_ndx,
    next_entry_id;
  uint32
    size = 0,
    progress_index_by,
    nof_occupied_in_entry;
  uint8
    mc_group_exist;


  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_ING_GROUP_SIZE_GET_UNSAFE_INNER);

  SOC_SAND_CHECK_NULL_INPUT(mc_group_size);

  if (is_ingress)
  {
    tbl_entry_id = (uint16)multicast_id_ndx;
  }
  else
  {
    /* if egress - concatenate with '1' at msb */
    tbl_entry_id = (uint16)(multicast_id_ndx | SOC_PETRA_MULT_LL_EGRESS_MC_ID_START_ENTRY);
  }
  
  mc_group_exist =
    soc_petra_mult_is_multicast_id_group_exist(
    unit,
    tbl_entry_id
    );

  /* get size only if multicast group exist */
  if (mc_group_exist == TRUE)
  {

    res = soc_petra_mult_ing_next_link_list_ptr_get(
            unit,
            tbl_entry_id,
            &next_entry_id
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    while (next_entry_id != soc_petra_mc_null_id(unit, is_ingress))
    {
      res = soc_petra_mult_progress_index_get(
        unit,
        multicast_id_ndx,
        tbl_entry_id,
        is_ingress,
        &progress_index_by
        );
      SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

      size += progress_index_by;

      tbl_entry_id = next_entry_id;

      res = soc_petra_mult_ing_next_link_list_ptr_get(
              unit,
              tbl_entry_id,
              &next_entry_id
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    }

    res = soc_petra_mult_nof_occupied_elements_in_tbl_entry(
            unit,
            tbl_entry_id,
            is_ingress,
            &nof_occupied_in_entry
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    size +=nof_occupied_in_entry;

    *mc_group_size = (uint32)size;
  }
  else
  {
    *mc_group_size = 0;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_ing_group_size_get_unsafe_inner()",0,0);
}

/*********************************************************************
* Details: refer to H file.
**********************************************************************/
uint32
  soc_petra_mult_nof_occupied_elements_in_tbl_entry(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint16                tbl_entry_id,
    SOC_SAND_IN  uint8               is_ingress,
    SOC_SAND_OUT uint32               *size
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_NOF_OCCUPIED_ELEMENTS_IN_TBL_ENTRY);

  SOC_SAND_CHECK_NULL_INPUT(size);

  if (is_ingress)
  {
     res = soc_petra_mult_ing_nof_occupied_elements_in_tbl_entry(
             unit,
             tbl_entry_id,
             size
           );
     SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }
  else
  {
    res = soc_petra_mult_eg_nof_occupied_elements_in_tbl_entry(
             unit,
             tbl_entry_id,
             size
           );
     SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_nof_occupied_elements_in_tbl_entry()",0,0);
}

/*********************************************************************
*     This function sets ingress replication hardware table,
*     indicating whether an entry is multicast-id or not
*********************************************************************/
uint32
  soc_petra_mult_multicast_ingress_replication_hw_table_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_ID             multicast_id_ndx,
    SOC_SAND_IN  uint8                 is_assert
  )
{
  uint32
    res;
  uint32
    is_ingress_rep_inner_offset,
    is_ingress_rep_tbl_offset;
  SOC_PETRA_IRR_IS_INGRESS_REPLICATION_DB_TBL_DATA
    irr_is_ingress_replication_db_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_MULTICAST_ASSERT_INGRESS_REPLICATION_HW_TABLE);

  is_ingress_rep_inner_offset = SOC_PETRA_FLD_IDX_GET(multicast_id_ndx, SOC_PETRA_MULT_ING_REP_TBL_NOF_FLDS_PER_DEVICE);
  is_ingress_rep_tbl_offset = SOC_PETRA_REG_IDX_GET(multicast_id_ndx, SOC_PETRA_MULT_ING_REP_TBL_NOF_FLDS_PER_DEVICE);

  res = soc_petra_irr_is_ingress_replication_db_tbl_get_unsafe(
          unit,
          is_ingress_rep_tbl_offset,
          &irr_is_ingress_replication_db_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  irr_is_ingress_replication_db_tbl_data.is_ingress_replication[is_ingress_rep_inner_offset] = is_assert;

  res = soc_petra_irr_is_ingress_replication_db_tbl_set_unsafe(
          unit,
          is_ingress_rep_tbl_offset,
          &irr_is_ingress_replication_db_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_multicast_assert_ingress_replication_hw_table()",0,0);
}

STATIC void
  soc_petra_mult_rplct_tbl_unoccupied_entry_set(
   SOC_SAND_IN    int                                           unit,
   SOC_SAND_IN    uint8                                           is_ingress,
   SOC_SAND_INOUT SOC_PETRA_IRR_INGRESS_REPLICATION_MULTICAST_DB_TBL_DATA *irr_ingress_replication_multicast_db_tbl_data
  )
{
  uint32
    mc_dest_id;

  if (irr_ingress_replication_multicast_db_tbl_data != NULL)
  {
    irr_ingress_replication_multicast_db_tbl_data->egress_data = SOC_PETRA_MULT_EGR_DATA_DFLT;
    irr_ingress_replication_multicast_db_tbl_data->link_ptr = soc_petra_mc_null_id(unit,is_ingress);
    for (mc_dest_id = 0; mc_dest_id < SOC_PETRA_MULT_ING_NOF_DESTINATIONS_PER_ENTRY; mc_dest_id++)
    {
      irr_ingress_replication_multicast_db_tbl_data->base_queue[mc_dest_id] = SOC_PETRA_MULT_ING_UNOCCUPIED_BASE_QUEUE_INDICATION;
      irr_ingress_replication_multicast_db_tbl_data->is_queue_number[mc_dest_id] = 1;
      irr_ingress_replication_multicast_db_tbl_data->out_lif[mc_dest_id] = SOC_PETRA_MULT_UNOCCUPIED_CUD_INDICATION;
    }
  }
}

/*********************************************************************
* This function sets the specified entry to be marked as unoccupied.
*********************************************************************/
uint32
  soc_petra_mult_rplct_tbl_entry_unoccupied_set(
    SOC_SAND_IN int               unit,
    SOC_SAND_IN uint16                tbl_entry_id,
    SOC_SAND_IN uint8               is_ingress
  )
{
  uint32
    res;
  SOC_PETRA_IRR_INGRESS_REPLICATION_MULTICAST_DB_TBL_DATA
    irr_ingress_replication_multicast_db_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_RPLCT_TBL_ENTRY_UNOCCUPIED_SET);

  soc_petra_mult_rplct_tbl_unoccupied_entry_set(unit, is_ingress, &irr_ingress_replication_multicast_db_tbl_data);

  res = soc_petra_irr_ingress_replication_multicast_db_tbl_set_unsafe(
          unit,
          tbl_entry_id,
          &irr_ingress_replication_multicast_db_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_rplct_tbl_entry_unoccupied_set()",0,0);
}

/*********************************************************************
* Initialize MC replication database
* The initialization accesses the replication table as if it was an
* Ingress replication, for all entries (including Egress MC)
**********************************************************************/
uint32
  soc_petra_mult_rplct_tbl_entry_unoccupied_set_all(
    SOC_SAND_IN  int               unit
  )
{
  uint32
    res,
    index,
    first_entry,
    lines_max,
    nof_lines_total,
    nof_lines;
  uint8
    is_ingress;
  SOC_PETRA_REGS
    *regs;
  SOC_PETRA_IRR_INGRESS_REPLICATION_MULTICAST_DB_TBL_DATA
    irr_ingress_replication_multicast_db_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_RPLCT_TBL_ENTRY_UNOCCUPIED_SET_ALL);

  regs = soc_petra_regs();

  
  
  /* Configuration split into two iterations: one for ingress, and the other for egress. */
  for(index = 0; index < 2; index++)
  {
    if (index == 0)
    {
      /* Ingress */
      is_ingress = TRUE;
      nof_lines_total = SOC_PETRA_MULT_ING_NOF_ENTRIES;
      first_entry = SOC_PETRA_MULT_LL_INGRESS_MC_ID_START_ENTRY;
    }
    else
    {
      /* Egress */
      is_ingress = FALSE;
      nof_lines_total = SOC_PETRA_MULT_EGR_NOF_ENTRIES;
      first_entry = SOC_PETRA_MULT_LL_EGRESS_MC_ID_START_ENTRY;
    }
    
    soc_petra_mult_rplct_tbl_unoccupied_entry_set(
      unit,
      is_ingress,
      &irr_ingress_replication_multicast_db_tbl_data
      );
  
  
    lines_max = SOC_PETRA_FLD_MAX(SOC_PETRA_REG_DB_ACC(regs->irr.indirect_command_reg.indirect_command_count)) - 1;
    do {
         nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
         nof_lines_total -= nof_lines;
         
         res = soc_petra_irr_set_reps_for_tbl_unsafe(unit,nof_lines);
         SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

         res = soc_petra_irr_ingress_replication_multicast_db_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &irr_ingress_replication_multicast_db_tbl_data
               );
        SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
         first_entry += nof_lines;
    } while(nof_lines_total > 0);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_rplct_tbl_entry_unoccupied_set_all()",0,0);
}

/*********************************************************************
 *  Check if the replication DB table entry is marked as occupied.
 *  Note: same entry value marks unoccupied entry for Ingress/Egress MC
 *********************************************************************/
uint32
  soc_petra_mult_rplct_tbl_entry_is_occupied(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint16   tbl_entry_id,
    SOC_SAND_OUT uint8  *is_occupied
  )
{
  uint32
    res;
  SOC_PETRA_IRR_INGRESS_REPLICATION_MULTICAST_DB_TBL_DATA
    irr_ingress_replication_multicast_db_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_RPLCT_TBL_ENTRY_IS_OCCUPIED);
  SOC_SAND_CHECK_NULL_INPUT(is_occupied);

  res = soc_petra_irr_ingress_replication_multicast_db_tbl_get_unsafe(
          unit,
          tbl_entry_id,
          &irr_ingress_replication_multicast_db_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  *is_occupied =
    SOC_SAND_NUM2BOOL_INVERSE(irr_ingress_replication_multicast_db_tbl_data.out_lif[0] == SOC_PETRA_MULT_UNOCCUPIED_CUD_INDICATION);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_rplct_tbl_entry_is_occupied()",0,0);
}

/*********************************************************************
* This function sets the specified entry be Occupied but Empty.
* Note: same entry value marks occupied-but-empty entry for Ingress/Egress MC
*********************************************************************/
uint32
  soc_petra_mult_rplct_tbl_entry_occupied_but_empty_set(
    SOC_SAND_IN int               unit,
    SOC_SAND_IN uint16                tbl_entry_id,
    SOC_SAND_IN uint8               is_ingress
  )
{
  uint32
    res;
  uint32
    mc_dest_id;
  SOC_PETRA_IRR_INGRESS_REPLICATION_MULTICAST_DB_TBL_DATA
    irr_ingress_replication_multicast_db_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_RPLCT_TBL_ENTRY_OCCUPIED_BUT_EMPTY_SET);

  soc_petra_mult_rplct_tbl_unoccupied_entry_set(
      unit,
      is_ingress,
      &irr_ingress_replication_multicast_db_tbl_data
      );
  for (mc_dest_id = 0; mc_dest_id < SOC_PETRA_MULT_ING_NOF_DESTINATIONS_PER_ENTRY; mc_dest_id++)
  {
    irr_ingress_replication_multicast_db_tbl_data.out_lif[mc_dest_id] = SOC_PETRA_MULT_OCCUPIED_BUT_EMPTY_ENTRY_CUD_INDICATION;
  }

  res = soc_petra_irr_ingress_replication_multicast_db_tbl_set_unsafe(
          unit,
          tbl_entry_id,
          &irr_ingress_replication_multicast_db_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_rplct_tbl_entry_unoccupied_set()",0,0);
}

/*********************************************************************
 *  Check if the replication DB table entry is marked as occupied.
 *  Note: same entry value marks occupied-but-empty entry for Ingress/Egress MC
 *********************************************************************/
uint32
  soc_petra_mult_rplct_tbl_entry_is_occupied_but_empty(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint16   tbl_entry_id,
    SOC_SAND_OUT uint8  *is_occupied_but_empty
  )
{
  uint32
    res;
  SOC_PETRA_IRR_INGRESS_REPLICATION_MULTICAST_DB_TBL_DATA
    irr_ingress_replication_multicast_db_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_RPLCT_TBL_ENTRY_IS_OCCUPIED);
  SOC_SAND_CHECK_NULL_INPUT(is_occupied_but_empty);

  res = soc_petra_irr_ingress_replication_multicast_db_tbl_get_unsafe(
          unit,
          tbl_entry_id,
          &irr_ingress_replication_multicast_db_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  *is_occupied_but_empty =
    SOC_SAND_NUM2BOOL(irr_ingress_replication_multicast_db_tbl_data.out_lif[0] == SOC_PETRA_MULT_OCCUPIED_BUT_EMPTY_ENTRY_CUD_INDICATION);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_rplct_tbl_entry_is_occupied()",0,0);
}


/*********************************************************************
 *  Check if the replication DB table entry is marked as empty.
 *  Note: same entry value marks unoccupied/occupied-but-empty entry
 *  for Ingress/Egress MC
 *********************************************************************/
uint32
  soc_petra_mult_rplct_tbl_entry_is_empty(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint16   tbl_entry_id,
    SOC_SAND_OUT uint8  *is_empty
  )
{
  uint32
    res;
  SOC_PETRA_IRR_INGRESS_REPLICATION_MULTICAST_DB_TBL_DATA
    irr_ingress_replication_multicast_db_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_RPLCT_TBL_ENTRY_IS_EMPTY_BUT_OCCUPIED);
  SOC_SAND_CHECK_NULL_INPUT(is_empty);

  res = soc_petra_irr_ingress_replication_multicast_db_tbl_get_unsafe(
          unit,
          tbl_entry_id,
          &irr_ingress_replication_multicast_db_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  *is_empty =
    SOC_SAND_NUM2BOOL(
      (irr_ingress_replication_multicast_db_tbl_data.out_lif[0] == SOC_PETRA_MULT_UNOCCUPIED_CUD_INDICATION) ||
      (irr_ingress_replication_multicast_db_tbl_data.out_lif[0] == SOC_PETRA_MULT_OCCUPIED_BUT_EMPTY_ENTRY_CUD_INDICATION)
    );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_rplct_tbl_entry_is_empty_but_occupied()",0,0);
}

#include <soc/dpp/SAND/Utils/sand_footer.h>
