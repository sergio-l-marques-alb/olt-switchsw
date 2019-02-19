/* $Id: petra_multicast_egress.c,v 1.12 Broadcom SDK $
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
/******************************************************************
*
* FILENAME:       DuneDriver/soc_petra/src/soc_petra_multicast_egress.c
*
* MODULE PREFIX:  soc_petra_mult_eg
*
* FILE DESCRIPTION: refer to H file.
*
* REMARKS:
* SW License Agreement: Dune Networks (c). CONFIDENTIAL PROPRIETARY INFORMATION.
* Any use of this Software is subject to Software License Agreement
* included in the Driver User Manual of this device.
* Any use of this Software constitutes an agreement to the terms
* of the above Software License Agreement.
******************************************************************/


/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/Petra/petra_multicast_egress.h>
#include <soc/dpp/Petra/petra_multicast_linked_list.h>

#include <soc/dpp/Petra/petra_reg_access.h>
#include <soc/dpp/Petra/petra_tbl_access.h>
#include <soc/dpp/Petra/petra_chip_tbls.h>
#include <soc/dpp/Petra/petra_general.h>
#include <soc/dpp/Petra/petra_sw_db.h>

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

#define SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE_MAX          (4*1024)
#define SOC_PETRA_MULT_EG_UNOCCUPIED_PORT_INDICATION               0x7F

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

STATIC uint32
  soc_petra_mult_eg_group_entries_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_EG_ENTRY       *mc_group,
    SOC_SAND_IN  uint32                  mc_group_size,
    SOC_SAND_OUT SOC_PETRA_MULT_ENTRY          *mc_group_inner
  )
{
  uint32
    res,
    nof_vlan_bitmaps;
  uint8
    first_time = TRUE;
  uint32
    indx,
    last_index_port = 0,
    indx2;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_EG_GROUP_ENTRIES_SET);

  nof_vlan_bitmaps =
    soc_petra_mult_nof_vlan_bitmaps_in_group_get(
    unit,
    mc_group,
    mc_group_size,
    FALSE
    );

  if (nof_vlan_bitmaps == 0)
  {
    for (indx = 0 ; indx < mc_group_size ; indx++)
    {
      SOC_PETRA_COPY(&(mc_group_inner[indx].eg_entry), &(mc_group[indx]), SOC_PETRA_MULT_EG_ENTRY, 1);
    }
  }
  else
  {
    if (SOC_PETRA_IS_DEV_PETRA_B_OR_ABOVE)
    {
      /* multicast group include vlan bitmaps, order entries as first entry type port then bitmaps. */
      /* special case: needed two entries of format B. this happens when nof_port % 3 = 2*/
      indx2 = 0;
      for (indx = 0 ; indx < mc_group_size ; indx++)
      {
        if (mc_group[indx].type != SOC_PETRA_MULT_EG_ENTRY_TYPE_VLAN_PTR)
        {
          SOC_PETRA_COPY(&(mc_group_inner[indx2].eg_entry), &(mc_group[indx]), SOC_PETRA_MULT_EG_ENTRY, 1);
          last_index_port = indx;
          indx2++;
        }
      }
      
      first_time = TRUE;

      for (indx = 0 ; indx < mc_group_size ; indx++)
      {
        if (mc_group[indx].type == SOC_PETRA_MULT_EG_ENTRY_TYPE_VLAN_PTR)
        {
          if (first_time && ((mc_group_size-nof_vlan_bitmaps) % 3 == 2))
          {
            /*special case: insert vlan bitmap before port*/
            SOC_PETRA_COPY(&(mc_group_inner[indx2].eg_entry), &(mc_group[last_index_port]), SOC_PETRA_MULT_EG_ENTRY, 1);
            SOC_PETRA_COPY(&(mc_group_inner[indx2-1].eg_entry), &(mc_group[indx]), SOC_PETRA_MULT_EG_ENTRY, 1);
          }
          else
          {
            SOC_PETRA_COPY(&(mc_group_inner[indx2].eg_entry), &(mc_group[indx]), SOC_PETRA_MULT_EG_ENTRY, 1);
          }
          indx2++;
          first_time = FALSE;
        }
      }
    }
    else
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_MULT_EG_ILLEGAL_VLAN_PTR_ENTRY_IN_LINK_LIST_ERR, 20, exit);
    }
  }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_eg_group_entries_set()",0,0);
}
/*********************************************************************
*     This procedure configures the range of values of the
*     multicast ids entry points that their multicast groups
*     are to be found according to a bitmap (as opposed to a
*     Link List).
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_mult_eg_vlan_membership_group_range_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

  if (info->mc_id_high < info->mc_id_low)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_MULT_EG_ILLEGAL_GROUP_RANG_CONFIG_ERR, 10, exit);
  }

  /* SOC_PETRA_MULT_ID_MIN may be changed and be grater than 0*/
  /* coverity[unsigned_compare : FALSE] */
  if ((info->mc_id_low < SOC_PETRA_MULT_ID_MIN )||(info->mc_id_low >  SOC_PETRA_MULT_ID_MAX))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_MULT_EG_ILLEGAL_MULT_ID_ERR, 20, exit);
  }

  /* SOC_PETRA_MULT_ID_MIN may be changed and be grater than 0*/
  /* coverity[unsigned_compare : FALSE] */
  if ((info->mc_id_high < SOC_PETRA_MULT_ID_MIN )||(info->mc_id_high >  SOC_PETRA_MULT_ID_MAX))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_MULT_EG_ILLEGAL_MULT_ID_ERR, 30, exit);
  }

  if ((info->mc_id_high - info->mc_id_low) >= SOC_PETRA_MULT_NOF_MULT_VLAN_ERP_GROUPS)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_MULT_EG_ILLEGAL_MULT_ID_ERR, 40, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_eg_vlan_membership_group_range_verify()",0,0);
}

/*********************************************************************
*     This procedure configures the range of values of the
*     multicast ids entry points that their multicast groups
*     are to be found according to a bitmap (as opposed to a
*     Link List).
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_mult_eg_vlan_membership_group_range_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE *info
  )
{
  uint32
    res;
  SOC_PETRA_REGS
    *regs = soc_petra_regs();

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PETRA_FLD_SET(regs->egq.egress_replication_bitmap_group_value_configuration_reg.egress_rep_bitmap_group_value_bot, info->mc_id_low, 10, exit);

  SOC_PETRA_FLD_SET(regs->egq.egress_replication_bitmap_group_value_configuration_reg.egress_rep_bitmap_group_value_top, info->mc_id_high, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_eg_vlan_membership_group_range_set_unsafe()",0,0);
}

/*********************************************************************
*     This procedure configures the range of values of the
*     multicast ids entry points that their multicast groups
*     are to be found according to a bitmap (as opposed to a
*     Link List).
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_mult_eg_vlan_membership_group_range_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE *info
  )
{
  uint32
    res;
  SOC_PETRA_REGS
    *regs = soc_petra_regs();

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PETRA_FLD_GET(regs->egq.egress_replication_bitmap_group_value_configuration_reg.egress_rep_bitmap_group_value_bot, (info->mc_id_low), 10, exit);

  SOC_PETRA_FLD_GET(regs->egq.egress_replication_bitmap_group_value_configuration_reg.egress_rep_bitmap_group_value_top, (info->mc_id_high), 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_eg_vlan_membership_group_range_get_unsafe()",0,0);
}

/*********************************************************************
*     This API enables the egress-multicast-replication for
*     the specific multicast-id, and creates in the device the
*     needed link-list/bitmap. The user only specifies the
*     multicast-id and copies. The user may open a multicast
*     group with no members, In this case the Multicast Id is
*     in use. All inner link-list nodes and bitmap are
*     allocated and handled by the driver.
*********************************************************************/
uint32
  soc_petra_mult_eg_group_open_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_ID             multicast_id_ndx,
    SOC_SAND_IN  SOC_PETRA_MULT_EG_ENTRY       *mc_group,
    SOC_SAND_IN  uint32                  mc_group_size,
    SOC_SAND_OUT uint8                 *insufficient_memory
  )
{
  uint32
    res;
  SOC_PETRA_MULT_ENTRY
    *mc_group_inner = NULL;


  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_EG_GROUP_OPEN_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(mc_group);
  SOC_SAND_CHECK_NULL_INPUT(insufficient_memory);

  SOC_PETRA_ALLOC(mc_group_inner, SOC_PETRA_MULT_ENTRY, mc_group_size);

  res = soc_petra_mult_eg_group_entries_set(
          unit,
          mc_group,
          mc_group_size,
          mc_group_inner
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

  res = soc_petra_mult_group_open_unsafe_joint(
          unit,
          multicast_id_ndx,
          mc_group_inner,
          mc_group_size,
          FALSE,
          insufficient_memory
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_PETRA_FREE(mc_group_inner);
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_eg_group_open_unsafe()",0,0);

}


/*********************************************************************
*     This API updates the egress-multicast-replication
*     definitions for the specific multicast-id, and creates
*     in the device the needed link-list/bitmap. The user only
*     specifies the multicast-id and copies. All inner
*     link-list nodes and bitmap are allocated and handled by
*     the driver.
*********************************************************************/
uint32
  soc_petra_mult_eg_group_update_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_ID             multicast_id_ndx,
    SOC_SAND_IN  SOC_PETRA_MULT_EG_ENTRY      *mc_group,
    SOC_SAND_IN  uint32                  mc_group_size,
    SOC_SAND_OUT uint8                 *insufficient_memory
  )
{
  uint32
    res;
  SOC_PETRA_MULT_ENTRY
    *mc_group_inner = NULL;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_EG_GROUP_UPDATE_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(mc_group);
  SOC_SAND_CHECK_NULL_INPUT(insufficient_memory);

  SOC_PETRA_ALLOC(mc_group_inner, SOC_PETRA_MULT_ENTRY, mc_group_size);

  res = soc_petra_mult_eg_group_entries_set(
      unit,
      mc_group,
      mc_group_size,
      mc_group_inner
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

  res = soc_petra_mult_group_open_unsafe_joint(
          unit,
          multicast_id_ndx,
          mc_group_inner,
          mc_group_size,
          FALSE,
          insufficient_memory
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_PETRA_FREE(mc_group_inner);
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_eg_group_update_unsafe()",0,0);

}


/*********************************************************************
*     This API closes egress-multicast-replication group for
*     the specific multicast-id. The user only specifies the
*     multicast-id. All inner link-list/bitmap nodes are freed
*     and handled by the driver
*********************************************************************/
uint32
  soc_petra_mult_eg_group_close_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_ID             multicast_id_ndx
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_EG_GROUP_CLOSE_UNSAFE);

  res = soc_petra_mult_group_close_unsafe_joint(
          unit,
          multicast_id_ndx,
          FALSE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_eg_group_close_unsafe()",0,0);

}


/*********************************************************************
*     Add port members of the Egress-Multicast and/or modify
*     the number of logical copies required at port.
*********************************************************************/
uint32
  soc_petra_mult_eg_port_add_unsafe_inner(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_ID             multicast_id_ndx,
    SOC_SAND_IN  SOC_PETRA_MULT_EG_ENTRY       *entry,
    SOC_SAND_OUT uint8                 *insufficient_memory
  )
{
  uint32
    size,
    res;
  SOC_PETRA_MULT_ENTRY
    *entry_inner = NULL;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_EG_PORT_ADD_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(entry);
  SOC_SAND_CHECK_NULL_INPUT(insufficient_memory);

  SOC_PETRA_ALLOC(entry_inner, SOC_PETRA_MULT_ENTRY, 1);
  entry_inner->eg_entry = *entry;
  
  res = soc_petra_mult_eg_group_size_get_unsafe(
          unit,
          multicast_id_ndx,
          &size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if(size>0)
  {
    res = soc_petra_mult_destination_add_unsafe_joint(
      unit,
      multicast_id_ndx,
      entry_inner,
      FALSE,
      insufficient_memory
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }
  else
  {
    res = soc_petra_mult_eg_group_open_unsafe(
      unit,
      multicast_id_ndx,
      entry,
      1,
      insufficient_memory
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }

exit:
  SOC_PETRA_FREE(entry_inner);
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_eg_port_add_unsafe()",0,0);
}
uint32
  soc_petra_mult_eg_port_add_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_ID             multicast_id_ndx,
    SOC_SAND_IN  SOC_PETRA_MULT_EG_ENTRY       *entry,
    SOC_SAND_OUT uint8                 *insufficient_memory
  )
{
  uint32
    res;
#if SOC_PETRA_MULT_PORT_ADD_PREVENT_DUPLICATIONS
  uint32
    mc_group_size = 0,
    exact_mc_group_size,
    indx;
  SOC_PETRA_MULT_EG_ENTRY
    *mc_group = NULL;
  uint8
    is_group_open;
#endif /*SOC_PETRA_MULT_PORT_ADD_PREVENT_DUPLICATIONS*/
  uint8
    perform_copy = TRUE;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_EG_PORT_ADD_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(entry);

  if (SOC_PETRA_IS_DEV_PETRA_A)
  {
    if (entry->type == SOC_PETRA_MULT_EG_ENTRY_TYPE_VLAN_PTR)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_MULT_EG_ILLEGAL_VLAN_PTR_ENTRY_IN_LINK_LIST_ERR, 10, exit);
    }
  }
  

#if SOC_PETRA_MULT_PORT_ADD_PREVENT_DUPLICATIONS
  res = soc_petra_mult_eg_group_size_get_unsafe(
    unit,
    multicast_id_ndx,
    &mc_group_size
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_PETRA_ALLOC(mc_group, SOC_PETRA_MULT_EG_ENTRY, (mc_group_size+1));

  res = soc_petra_mult_eg_group_get_unsafe(
    unit,
    multicast_id_ndx,
    mc_group_size,
    mc_group,
    &exact_mc_group_size,
    &is_group_open
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  mc_group_size = exact_mc_group_size;

  for (indx = 0 ; indx < mc_group_size ; indx++)
  {
    if (SOC_PETRA_IS_DEV_PETRA_A)
    {
      if (mc_group[indx].port == entry->port && mc_group[indx].cud == entry->cud)
      {
        /* Copy already exist */
        perform_copy = FALSE;
      }
    }
#ifdef LINK_PB_LIBRARIES
    if (SOC_PETRA_IS_DEV_PETRA_B_OR_ABOVE)
    {
      /* vlan pointer match */
      if (entry->type == SOC_PETRA_MULT_EG_ENTRY_TYPE_VLAN_PTR &&
        mc_group[indx].type == entry->type && mc_group[indx].vlan_mc_id == entry->vlan_mc_id &&
        mc_group[indx].cud == entry->cud)
      {
        /* Copy already exist */
        perform_copy = FALSE;
      }
      /* port match */
      if (entry->type == SOC_PETRA_MULT_EG_ENTRY_TYPE_OFP &&
        mc_group[indx].type == entry->type && mc_group[indx].port == entry->port &&
        mc_group[indx].cud == entry->cud)
      {
        /* Copy already exist */
        perform_copy = FALSE;
      }
    }
#endif
  }
#endif /*SOC_PETRA_MULT_PORT_ADD_PREVENT_DUPLICATIONS*/
  if (perform_copy)
  {
#if SOC_PETRA_MULT_USE_SMART_ADD_ENTRY
    if (SOC_PETRA_IS_DEV_PETRA_A)
    {
      res = soc_petra_mult_eg_port_add_unsafe_inner(
        unit,
        multicast_id_ndx,
        entry,
        insufficient_memory
        );
      SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);
    }
#ifdef LINK_PB_LIBRARIES
    if (SOC_PETRA_IS_DEV_PETRA_B_OR_ABOVE)
    {
      /* Check special case: when adding vlan pointer and last 2 entries are PP then perform update */
      if (mc_group_size >= 2 && entry->type == SOC_PETRA_MULT_EG_ENTRY_TYPE_VLAN_PTR &&
        mc_group[mc_group_size-1].type == SOC_PETRA_MULT_EG_ENTRY_TYPE_OFP &&
        mc_group[mc_group_size-2].type == SOC_PETRA_MULT_EG_ENTRY_TYPE_OFP)
      {
        mc_group[mc_group_size].type  = entry->type;
        mc_group[mc_group_size].vlan_mc_id = entry->vlan_mc_id;
        mc_group[mc_group_size].port  = entry->port;
        mc_group[mc_group_size].cud   = entry->cud;

        mc_group_size++;

        res = soc_petra_mult_eg_group_update_unsafe(
          unit,
          multicast_id_ndx,
          mc_group,
          mc_group_size,
          insufficient_memory
          );
        SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
       }
      else
      {
        res = soc_petra_mult_eg_port_add_unsafe_inner(
          unit,
          multicast_id_ndx,
          entry,
          insufficient_memory
          );
        SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);
      }
    }
#endif
    
#else /*SOC_PETRA_MULT_USE_SMART_ADD_ENTRY*/
    mc_group[mc_group_size].cud   = entry->cud;
    if (SOC_PETRA_IS_DEV_PETRA_A)
    {
      mc_group[mc_group_size].port  = entry->port;
    }
#ifdef LINK_PB_LIBRARIES
    if (SOC_PETRA_IS_DEV_PETRA_B_OR_ABOVE)
    {
      mc_group[mc_group_size].type  = entry->type;
      mc_group[mc_group_size].vlan_mc_id = entry->vlan_mc_id;
      mc_group[mc_group_size].port  = entry->port;
    }
#endif
    
    mc_group_size++;

    res = soc_petra_mult_eg_group_update_unsafe(
      unit,
      multicast_id_ndx,
      mc_group,
      mc_group_size,
      insufficient_memory
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
#endif /*SOC_PETRA_MULT_USE_SMART_ADD_ENTRY*/
  }
  /* else the port to add did not exist in the group*/

exit:
#if SOC_PETRA_MULT_PORT_ADD_PREVENT_DUPLICATIONS
  SOC_PETRA_FREE(mc_group);
#endif /*SOC_PETRA_MULT_PORT_ADD_PREVENT_DUPLICATIONS*/
  /*If 'entry' is not NULL, lets print it's information in the error printout*/
  if(entry)
  {
    if (ex != no_err)
    {
      soc_sand_error_handler(
        ex, "error in soc_petra_mult_eg_port_add_unsafe()", exit_place,multicast_id_ndx,entry->port,entry->cud,0,0
        );
    }
    return ex;
  }
  else
  {
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_eg_port_add_unsafe()",multicast_id_ndx,0);
  }
}

/*********************************************************************
*     Removes a port member of the egress multicast.
*********************************************************************/
uint32
  soc_petra_mult_eg_port_remove_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_ID             multicast_id_ndx,
    SOC_SAND_IN  SOC_PETRA_MULT_EG_ENTRY       *entry
  )
{
  uint32
    res;
  uint32
    mc_group_size = 0,
    exact_mc_group_size,
    indx;
  SOC_PETRA_MULT_EG_ENTRY
    *mc_group = NULL;
  uint8
    is_group_open,
    insufficient_memory,
    perform_copy = FALSE;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_EG_PORT_REMOVE_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(entry);

  res = soc_petra_mult_eg_group_size_get_unsafe(
          unit,
          multicast_id_ndx,
          &mc_group_size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (mc_group_size == 0)
  {
    SOC_PETRA_DO_NOTHING_AND_EXIT;
  }

  SOC_PETRA_ALLOC(mc_group, SOC_PETRA_MULT_EG_ENTRY, mc_group_size);

  res = soc_petra_mult_eg_group_get_unsafe(
          unit,
          multicast_id_ndx,
          mc_group_size,
          mc_group,
          &exact_mc_group_size,
          &is_group_open
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  mc_group_size = exact_mc_group_size;

  for (indx = 0 ; indx < mc_group_size ; indx++)
  {
    if (SOC_PETRA_IS_DEV_PETRA_A)
    {
      if (mc_group[indx].port == entry->port && mc_group[indx].cud == entry->cud)
      {
        /* we have to start the shift of the list */
        perform_copy = TRUE;
      }
    }
#ifdef LINK_PB_LIBRARIES
    if (SOC_PETRA_IS_DEV_PETRA_B_OR_ABOVE)
    {
      /* Perform copy if there is a match */
      /* vlan pointer match */
      if (entry->type == SOC_PETRA_MULT_EG_ENTRY_TYPE_VLAN_PTR &&
        mc_group[indx].type == entry->type && mc_group[indx].vlan_mc_id == entry->vlan_mc_id &&
        mc_group[indx].cud == entry->cud)
      {
        perform_copy = TRUE;
      }
      /* port match */
      if (entry->type == SOC_PETRA_MULT_EG_ENTRY_TYPE_OFP &&
        mc_group[indx].type == entry->type && mc_group[indx].port == entry->port &&
        mc_group[indx].cud == entry->cud)
      {
        perform_copy = TRUE;
      }
    }
#endif

    if (perform_copy)
    {
      if (indx == (mc_group_size - 1))
      {
        /*
         *  Last entry is ignored
         */
        soc_petra_PETRA_MULT_EG_ENTRY_clear(&(mc_group[indx]));
      }
      else
      {
        mc_group[indx] = mc_group[indx + 1];
      }
    }
    
  }
  if (perform_copy)
  {
    mc_group_size--;

    res = soc_petra_mult_eg_group_update_unsafe(
            unit,
            multicast_id_ndx,
            mc_group,
            mc_group_size,
            &insufficient_memory
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    if (insufficient_memory == TRUE)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_MULT_REMOVE_ELEMENT_DID_NOT_SUCCEED_ERROR, 50, exit);
    }
  }
  /* else the port to remove did not exist in the group*/

exit:
  SOC_PETRA_FREE(mc_group);
  /*If 'entry' is not NULL, lets print it's information in the error printout*/
  if(entry)
  {
    if (ex != no_err)
    {
      soc_sand_error_handler(
      ex, "error in soc_petra_mult_eg_port_remove_unsafe()", exit_place,multicast_id_ndx,entry->port,entry->cud,mc_group_size,0
      );
    }
    return ex;
  }
  else
  {
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_eg_port_remove_unsafe()",multicast_id_ndx,0);
  }
}


/*********************************************************************
*     Returns the size of the multicast group with the
*     specified multicast id.
*********************************************************************/
uint32
  soc_petra_mult_eg_group_size_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_ID             multicast_id_ndx,
    SOC_SAND_OUT uint32                  *mc_group_size
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_EG_GROUP_SIZE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(mc_group_size);

  res = soc_petra_mult_group_size_get_unsafe_joint(
          unit,
          multicast_id_ndx,
          FALSE,
          mc_group_size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_eg_group_size_get_unsafe()",0,0);

}


/*********************************************************************
*     Gets the multicast group with the specified multicast
*     id.
*********************************************************************/
uint32
  soc_petra_mult_eg_group_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_ID             multicast_id_ndx,
    SOC_SAND_IN  uint32                  mc_group_size,
    SOC_SAND_OUT SOC_PETRA_MULT_EG_ENTRY       *mc_group,
    SOC_SAND_OUT uint32                  *exact_mc_group_size,
    SOC_SAND_OUT uint8                 *is_open
  )
{
  uint32
    res;
  uint16
    tbl_entry_id;
  uint32
    indx,
    indx_in_entry;
  uint8
    is_relevent_get = TRUE;
  SOC_PETRA_IRR_EGRESS_REPLICATION_MULTICAST_DB_TBL_DATA
    irr_egress_replication_multicast_db_tbl_data;
#ifdef LINK_PB_LIBRARIES
  uint32
    nof_vlan_bitmaps;
#endif

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_EG_GROUP_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(mc_group);
  SOC_SAND_CHECK_NULL_INPUT(exact_mc_group_size);
  SOC_SAND_CHECK_NULL_INPUT(is_open);

  res = SOC_SAND_OK; sal_memset(
          &irr_egress_replication_multicast_db_tbl_data,
          0,
          sizeof(SOC_PETRA_IRR_EGRESS_REPLICATION_MULTICAST_DB_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);


  tbl_entry_id = (uint16)(multicast_id_ndx | SOC_PETRA_MULT_LL_EGRESS_MC_ID_START_ENTRY);

  /* check that size is ok */
  res = soc_petra_mult_eg_group_size_get_unsafe(
          unit,
          multicast_id_ndx,
          exact_mc_group_size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (*exact_mc_group_size > mc_group_size)
  {
     SOC_SAND_SET_ERROR_CODE(SOC_PETRA_MULT_MC_GROUP_REAL_SIZE_LARGER_THAN_GIVEN_SIZE_ERR, 10, exit);
  }
  /*
   *
   */
  
  /* Check if Occupied but Empty */
  *is_open = *exact_mc_group_size == 0 ?
    soc_petra_mult_is_multicast_id_group_exist(unit, tbl_entry_id) :
    TRUE;

#ifdef LINK_PB_LIBRARIES
  if (SOC_PETRA_IS_DEV_PETRA_B_OR_ABOVE)
  {
    nof_vlan_bitmaps =
      soc_pb_sw_db_multicast_eg_mult_nof_vlan_bitmaps_get(
      unit,
      multicast_id_ndx
      );
    if (nof_vlan_bitmaps != 0)
    {
      is_relevent_get = FALSE;
    }
  }
#endif

  if (is_relevent_get)
  {
    for (indx = 0 ; indx < *exact_mc_group_size ; indx++)
    {
      indx_in_entry =
        indx % SOC_PETRA_MULT_EG_NOF_PORTS_PER_ENTRY;

      if (indx_in_entry == 0)
      {
        res = soc_petra_irr_egress_replication_multicast_db_tbl_get_unsafe(
          unit,
          tbl_entry_id,
          &irr_egress_replication_multicast_db_tbl_data
          );
        SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

        tbl_entry_id =
          (uint16)irr_egress_replication_multicast_db_tbl_data.link_ptr;
      }
      
      mc_group[indx].type = SOC_PETRA_MULT_EG_ENTRY_TYPE_OFP;
      mc_group[indx].port =
        irr_egress_replication_multicast_db_tbl_data.port[indx_in_entry];
      mc_group[indx].cud =
        irr_egress_replication_multicast_db_tbl_data.out_lif[indx_in_entry];
    }
  }
#ifdef LINK_PB_LIBRARIES
  else
  {
    if (SOC_PETRA_IS_DEV_PETRA_B_OR_ABOVE)
    {
      res = soc_pb_mult_eg_fill_in_group(
        unit,
        tbl_entry_id,
        *exact_mc_group_size,
        mc_group
        );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    }
  }
#endif

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_eg_group_get_unsafe()",0,0);

}


/*********************************************************************
*     Closes all opened egress multicast groups.
*********************************************************************/
uint32
  soc_petra_mult_eg_all_groups_close_unsafe(
    SOC_SAND_IN  int                 unit
  )
{
  uint32
    res;
  uint32
    indx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_EG_ALL_GROUPS_CLOSE_UNSAFE);

  for (indx = 0 ; indx < SOC_PETRA_MULT_NOF_MULTICAST_GROUPS ; indx++)
  {
    res = soc_petra_mult_eg_group_close_unsafe(
            unit,
            indx
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_eg_all_groups_close_unsafe()",0,0);

}

/************************************************************************/
/*                                                                      */
/************************************************************************/
uint32
  soc_petra_mult_eg_vlan_membership_multicast_id_ndx_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_ID             multicast_id_ndx
  )
{
  uint32
    res;
  SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE
    info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_MULTICAST_ID_NDX_VERIFY);

  res = soc_petra_mult_eg_vlan_membership_group_range_get_unsafe(
          unit,
          &info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if ((multicast_id_ndx < info.mc_id_low) ||
    (multicast_id_ndx > info.mc_id_high))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_MULT_EG_MULTICAST_ID_NOT_IN_VLAN_MEMBERSHIP_RNG_ERR, 10, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_eg_vlan_membership_multicast_id_ndx_verify()",0,0);
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
uint32
  soc_petra_mult_eg_group_open_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_ID             multicast_id_ndx
  )
{
  uint32
    res;
  SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE
    info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_MULTICAST_ID_NDX_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    multicast_id_ndx, SOC_PETRA_MULT_NOF_MULTICAST_GROUPS-1,
    SOC_PETRA_MULT_MC_ID_OUT_OF_RANGE_ERR, 10, exit
  );

  res = soc_petra_mult_eg_vlan_membership_group_range_get(
          unit,
          &info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  if ((multicast_id_ndx >= info.mc_id_low ) &&
    (multicast_id_ndx <= info.mc_id_high ))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_MULT_EG_MULTICAST_ID_IS_IN_VLAN_MEMBERSHIP_RNG_ERR, 30, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_eg_group_open_verify()",0,0);
}

/*********************************************************************
*     This API enables the egress-multicast-replication for
*     the specific multicast-id, and creates in the device the
*     needed link-list/bitmap. The user only specifies the
*     multicast-id and copies. All inner link-list nodes and
*     bitmap are allocated and handled by the driver.
*********************************************************************/
uint32
  soc_petra_mult_eg_vlan_membership_group_open_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_ID             multicast_id_ndx,
    SOC_SAND_IN  SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_GROUP *group
  )
{
  uint32
    res,
    tbl_offset;
  uint32
    long_in_bitmap_i;
  SOC_PETRA_EGQ_VLAN_TABLE_TBL_DATA
    egq_vlan_table_tbl_data;
  SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE
    info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_GROUP_OPEN_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(group);

  res = soc_petra_mult_eg_vlan_membership_group_range_get_unsafe(
          unit,
          &info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  SOC_SAND_ERR_IF_BELOW_MIN(
    multicast_id_ndx, info.mc_id_low,
    SOC_PETRA_MULT_VLAN_MEMB_ID_OUT_OF_RANGE_ERR, 10, exit
  );

  tbl_offset = multicast_id_ndx - info.mc_id_low;

  SOC_SAND_ERR_IF_ABOVE_MAX(
    tbl_offset, (SOC_PETRA_MULT_EG_VLAN_NOF_IDS_MAX - 1),
    SOC_PETRA_MULT_VLAN_MEMB_ID_OUT_OF_RANGE_ERR, 15, exit
  );

  res = soc_petra_egq_vlan_table_tbl_get_unsafe(
          unit,
          tbl_offset,
          &egq_vlan_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  for (long_in_bitmap_i = 0; long_in_bitmap_i < SOC_PETRA_MULT_EG_NOF_UINT32S_IN_BITMAP; long_in_bitmap_i++)
  {
    egq_vlan_table_tbl_data.vlan_membership[long_in_bitmap_i] =
      group->bitmap[long_in_bitmap_i];
  }

  res = soc_petra_egq_vlan_table_tbl_set_unsafe(
          unit,
          tbl_offset,
          &egq_vlan_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_eg_vlan_membership_group_open_unsafe()",0,0);
}


/*********************************************************************
*     This API updates the egress-multicast-replication
*     definitions for the specific multicast-id, and creates
*     in the device the needed link-list/bitmap. The user only
*     specifies the multicast-id and copies. All inner
*     link-list nodes and bitmap are allocated and handled by
*     the driver.
*********************************************************************/
uint32
  soc_petra_mult_eg_vlan_membership_group_update_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_ID             multicast_id_ndx,
    SOC_SAND_IN  SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_GROUP *group
  )
{
  uint32
    res,
    tbl_offset;
  uint32
    long_in_bitmap_i;
  SOC_PETRA_EGQ_VLAN_TABLE_TBL_DATA
    egq_vlan_table_tbl_data;
  SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE
    info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_GROUP_UPDATE_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(group);

  res = soc_petra_mult_eg_vlan_membership_group_range_get_unsafe(
          unit,
          &info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  SOC_SAND_ERR_IF_BELOW_MIN(
    multicast_id_ndx, info.mc_id_low,
    SOC_PETRA_MULT_VLAN_MEMB_ID_OUT_OF_RANGE_ERR, 10, exit
  );

  tbl_offset = multicast_id_ndx - info.mc_id_low;

  SOC_SAND_ERR_IF_ABOVE_MAX(
    tbl_offset, (SOC_PETRA_MULT_EG_VLAN_NOF_IDS_MAX - 1),
    SOC_PETRA_MULT_VLAN_MEMB_ID_OUT_OF_RANGE_ERR, 15, exit
  );

  res = soc_petra_egq_vlan_table_tbl_get_unsafe(
          unit,
          tbl_offset,
          &egq_vlan_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  for (long_in_bitmap_i = 0; long_in_bitmap_i < SOC_PETRA_MULT_EG_NOF_UINT32S_IN_BITMAP; long_in_bitmap_i++)
  {
    egq_vlan_table_tbl_data.vlan_membership[long_in_bitmap_i] =
      group->bitmap[long_in_bitmap_i];
  }

  res = soc_petra_egq_vlan_table_tbl_set_unsafe(
          unit,
          tbl_offset,
          &egq_vlan_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_eg_vlan_membership_group_update_unsafe()",0,0);
}


/*********************************************************************
*     This API closes egress-multicast-replication group for
*     the specific multicast-id. The user only specifies the
*     multicast-id. All inner link-list/bitmap nodes are freed
*     and handled by the driver
*********************************************************************/
uint32
  soc_petra_mult_eg_vlan_membership_group_close_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_ID             multicast_id_ndx
  )
{
  uint32
    res,
    tbl_offset;
  uint32
    long_in_bitmap_i;
  SOC_PETRA_EGQ_VLAN_TABLE_TBL_DATA
    egq_vlan_table_tbl_data;
  SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE
    info;


  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_GROUP_CLOSE_UNSAFE);

  res = soc_petra_mult_eg_vlan_membership_group_range_get_unsafe(
          unit,
          &info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  SOC_SAND_ERR_IF_BELOW_MIN(
    multicast_id_ndx, info.mc_id_low,
    SOC_PETRA_MULT_VLAN_MEMB_ID_OUT_OF_RANGE_ERR, 10, exit
  );

  tbl_offset = multicast_id_ndx - info.mc_id_low;

  SOC_SAND_ERR_IF_ABOVE_MAX(
    tbl_offset, (SOC_PETRA_MULT_EG_VLAN_NOF_IDS_MAX - 1),
    SOC_PETRA_MULT_VLAN_MEMB_ID_OUT_OF_RANGE_ERR, 15, exit
  );

  res = soc_petra_egq_vlan_table_tbl_get_unsafe(
          unit,
          tbl_offset,
          &egq_vlan_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  for (long_in_bitmap_i = 0; long_in_bitmap_i < SOC_PETRA_MULT_EG_NOF_UINT32S_IN_BITMAP; long_in_bitmap_i++)
  {
    egq_vlan_table_tbl_data.vlan_membership[long_in_bitmap_i] = 0;
  }

  res = soc_petra_egq_vlan_table_tbl_set_unsafe(
          unit,
          tbl_offset,
          &egq_vlan_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_eg_vlan_membership_group_close_unsafe()",0,0);
}


/*********************************************************************
*     Add port members of the Egress-Multicast and/or modify
*     the number of logical copies required at port.
*********************************************************************/
uint32
  soc_petra_mult_eg_vlan_membership_port_add_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_ID             multicast_id_ndx,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         port
  )
{
  uint32
    res,
    tbl_offset,
    val;
  uint32
    port_inner_i,
    long_in_bitmap_i;
  SOC_PETRA_EGQ_VLAN_TABLE_TBL_DATA
    egq_vlan_table_tbl_data;
  SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE
    info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_PORT_ADD_UNSAFE);

  res = soc_petra_mult_eg_vlan_membership_group_range_get_unsafe(
          unit,
          &info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  SOC_SAND_ERR_IF_BELOW_MIN(
    multicast_id_ndx, info.mc_id_low,
    SOC_PETRA_MULT_VLAN_MEMB_ID_OUT_OF_RANGE_ERR, 10, exit
  );

  tbl_offset = multicast_id_ndx - info.mc_id_low;

  SOC_SAND_ERR_IF_ABOVE_MAX(
    tbl_offset, (SOC_PETRA_MULT_EG_VLAN_NOF_IDS_MAX - 1),
    SOC_PETRA_MULT_VLAN_MEMB_ID_OUT_OF_RANGE_ERR, 15, exit
  );

  res = soc_petra_egq_vlan_table_tbl_get_unsafe(
          unit,
          tbl_offset,
          &egq_vlan_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  long_in_bitmap_i = (uint32)(port / SOC_SAND_REG_SIZE_BITS);
  port_inner_i = port - (long_in_bitmap_i * SOC_SAND_REG_SIZE_BITS);

  val = 0x1;
  SOC_SAND_SET_BIT(egq_vlan_table_tbl_data.vlan_membership[long_in_bitmap_i],
    val ,
    port_inner_i);

  res = soc_petra_egq_vlan_table_tbl_set_unsafe(
          unit,
          tbl_offset,
          &egq_vlan_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_eg_vlan_membership_port_add_unsafe()",0,0);
}


/*********************************************************************
*     Removes a port member of the egress multicast.
*********************************************************************/
uint32
  soc_petra_mult_eg_vlan_membership_port_remove_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_ID             multicast_id_ndx,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID        port
  )
{
  uint32
    res,
    tbl_offset,
    val;
  uint32
    port_inner_i,
    long_in_bitmap_i;
  SOC_PETRA_EGQ_VLAN_TABLE_TBL_DATA
    egq_vlan_table_tbl_data;
  SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE
    info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_PORT_REMOVE_UNSAFE);

  res = soc_petra_mult_eg_vlan_membership_group_range_get_unsafe(
          unit,
          &info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  SOC_SAND_ERR_IF_BELOW_MIN(
    multicast_id_ndx, info.mc_id_low,
    SOC_PETRA_MULT_VLAN_MEMB_ID_OUT_OF_RANGE_ERR, 10, exit
  );

  tbl_offset = multicast_id_ndx - info.mc_id_low;

  SOC_SAND_ERR_IF_ABOVE_MAX(
    tbl_offset, (SOC_PETRA_MULT_EG_VLAN_NOF_IDS_MAX - 1),
    SOC_PETRA_MULT_VLAN_MEMB_ID_OUT_OF_RANGE_ERR, 15, exit
  );

  res = soc_petra_egq_vlan_table_tbl_get_unsafe(
          unit,
          tbl_offset,
          &egq_vlan_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  long_in_bitmap_i = (uint32)(port / SOC_SAND_REG_SIZE_BITS);
  port_inner_i = port - (long_in_bitmap_i * SOC_SAND_REG_SIZE_BITS);

  val = 0x0;
  SOC_SAND_SET_BIT(egq_vlan_table_tbl_data.vlan_membership[long_in_bitmap_i],
    val ,
    port_inner_i);

  res = soc_petra_egq_vlan_table_tbl_set_unsafe(
          unit,
          tbl_offset,
          &egq_vlan_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_eg_vlan_membership_port_remove_unsafe()",0,0);

}


/*********************************************************************
*     Gets the multicast group with the specified multicast
*     id.
*********************************************************************/
uint32
  soc_petra_mult_eg_vlan_membership_group_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_ID             multicast_id_ndx,
    SOC_SAND_OUT SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_GROUP *group
  )
{
  uint32
    res,
    tbl_offset;
  uint32
    long_in_bitmap_i;
  SOC_PETRA_EGQ_VLAN_TABLE_TBL_DATA
    egq_vlan_table_tbl_data;
  SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE
    info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_GROUP_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(group);

  res = soc_petra_mult_eg_vlan_membership_group_range_get_unsafe(
          unit,
          &info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  tbl_offset = multicast_id_ndx - info.mc_id_low;

  res = soc_petra_egq_vlan_table_tbl_get_unsafe(
          unit,
          tbl_offset,
          &egq_vlan_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  for (long_in_bitmap_i = 0; long_in_bitmap_i < SOC_PETRA_MULT_EG_NOF_UINT32S_IN_BITMAP; long_in_bitmap_i++)
  {
     group->bitmap[long_in_bitmap_i] =
       egq_vlan_table_tbl_data.vlan_membership[long_in_bitmap_i];
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_eg_vlan_membership_group_get_unsafe()",0,0);

}


/*********************************************************************
*     Closes all opened egress multicast groups in range of
*     vlan membership.
*********************************************************************/
uint32
  soc_petra_mult_eg_vlan_membership_all_groups_close_unsafe(
    SOC_SAND_IN  int                 unit
  )
{
  uint32
    res,
    tbl_offset;
  uint32
    long_in_bitmap_i;
  SOC_PETRA_EGQ_VLAN_TABLE_TBL_DATA
    egq_vlan_table_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_ALL_GROUPS_CLOSE_UNSAFE);

  res = SOC_SAND_OK; sal_memset(
          &egq_vlan_table_tbl_data,
          0,
          sizeof(SOC_PETRA_EGQ_VLAN_TABLE_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  for (tbl_offset = 0 ; tbl_offset < SOC_PETRA_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE_MAX; tbl_offset++)
  {
    for (long_in_bitmap_i = 0; long_in_bitmap_i < SOC_PETRA_MULT_EG_NOF_UINT32S_IN_BITMAP; long_in_bitmap_i++)
    {
      egq_vlan_table_tbl_data.vlan_membership[long_in_bitmap_i] = 0;
    }
    res = soc_petra_egq_vlan_table_tbl_set_unsafe(
            unit,
            tbl_offset,
            &egq_vlan_table_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_eg_vlan_membership_all_groups_close_unsafe()",0,0);

}



/*********************************************************************
*     Enters the multicast group part into the specified entry.
*********************************************************************/
uint32
  soc_petra_mult_eg_multicast_group_entry_to_tbl(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 in_group_index,
    SOC_SAND_IN  uint32                  offset,
    SOC_SAND_IN  SOC_PETRA_MULT_ENTRY          *mc_group,
    SOC_SAND_IN  uint32                  mc_group_size,
    SOC_SAND_IN  uint16                  next_entry
  )
{
  uint32
    res;
  uint32
    in_group_i,
    indx,
    progress_index_by;
  SOC_PETRA_IRR_EGRESS_REPLICATION_MULTICAST_DB_TBL_DATA
    irr_egress_replication_multicast_db_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_EG_MULTICAST_GROUP_ENTRY_TO_TBL);

  SOC_SAND_CHECK_NULL_INPUT(mc_group);

  /*in egress- 3 elements per entry*/
  progress_index_by = SOC_PETRA_MULT_EG_NOF_PORTS_PER_ENTRY;

  res = SOC_SAND_OK; sal_memset(
          &irr_egress_replication_multicast_db_tbl_data,
          0,
          sizeof(SOC_PETRA_IRR_EGRESS_REPLICATION_MULTICAST_DB_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);


  for (in_group_i = in_group_index; in_group_i < (in_group_index + progress_index_by); in_group_i++)
  {
    indx = in_group_i - in_group_index;

    if (in_group_i < mc_group_size)
    {
      irr_egress_replication_multicast_db_tbl_data.port[indx] =
        mc_group[in_group_i].eg_entry.port;
      irr_egress_replication_multicast_db_tbl_data.out_lif[indx] =
        mc_group[in_group_i].eg_entry.cud;
    }
    else
    {
      /* 'fill occupied but empty' entry port with 0xFF */
      irr_egress_replication_multicast_db_tbl_data.port[indx] =
        SOC_PETRA_MULT_EG_UNOCCUPIED_PORT_INDICATION;  /* 0xFF */
      irr_egress_replication_multicast_db_tbl_data.out_lif[indx] =
        SOC_PETRA_MULT_OCCUPIED_BUT_EMPTY_ENTRY_CUD_INDICATION;
    }

  }

  irr_egress_replication_multicast_db_tbl_data.link_ptr =
    next_entry;

   /* Write entry to HW*/

  res = soc_petra_irr_egress_replication_multicast_db_tbl_set_unsafe(
          unit,
          offset,
          &irr_egress_replication_multicast_db_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_eg_multicast_group_entry_to_tbl()",0,0);
}


/*********************************************************************
*     Returns a multicast group containing the elements of a mc-id
*     entry in the multicast table.
*********************************************************************/
uint32
  soc_petra_mult_eg_entry_content_get(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  SOC_PETRA_MULT_ID            multicast_id_ndx,
    SOC_SAND_OUT SOC_PETRA_MULT_ENTRY         *mc_group,
    SOC_SAND_OUT uint32                *mc_group_size,
    SOC_SAND_OUT uint16                 *next_entry
  )
{
  uint32
    res;
  uint32
    indx;
  uint8
    is_relevent_entry = TRUE;
  SOC_PETRA_IRR_EGRESS_REPLICATION_MULTICAST_DB_TBL_DATA
    irr_egress_replication_multicast_db_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_EG_ENTRY_CONTENT_GET);

  SOC_SAND_CHECK_NULL_INPUT(mc_group);
  SOC_SAND_CHECK_NULL_INPUT(mc_group_size);
  SOC_SAND_CHECK_NULL_INPUT(next_entry);

  res = SOC_SAND_OK; sal_memset(
          &irr_egress_replication_multicast_db_tbl_data,
          0,
          sizeof(SOC_PETRA_IRR_EGRESS_REPLICATION_MULTICAST_DB_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  *mc_group_size = SOC_PETRA_MULT_EG_NOF_PORTS_PER_ENTRY;

  res = soc_petra_irr_egress_replication_multicast_db_tbl_get_unsafe(
          unit,
          multicast_id_ndx,
          &irr_egress_replication_multicast_db_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  
#ifdef LINK_PB_LIBRARIES
  if (SOC_PETRA_IS_DEV_PETRA_B_OR_ABOVE)
  {
    /* Format select indicates entry format */
    if (irr_egress_replication_multicast_db_tbl_data.port[0] ==
      SOC_PB_MULT_EG_FORMAT_SELECT_B)
    {
      res = soc_pb_mult_eg_entry_content_get(
              unit,
              multicast_id_ndx,
              SOC_PETRA_MULT_EG_FORMAT_B,
              mc_group,
              mc_group_size,
              next_entry
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

      is_relevent_entry = FALSE;
    }
    else if (irr_egress_replication_multicast_db_tbl_data.port[0] ==
      SOC_PB_MULT_EG_FORMAT_SELECT_C)
    {
      res = soc_pb_mult_eg_entry_content_get(
        unit,
        multicast_id_ndx,
        SOC_PETRA_MULT_EG_FORMAT_C,
        mc_group,
        mc_group_size,
        next_entry
        );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

      is_relevent_entry = FALSE;
    }
  }
#endif
  
  if (is_relevent_entry)
  {
    for (indx = 0 ; indx < SOC_PETRA_MULT_EG_NOF_PORTS_PER_ENTRY; indx++)
    {
      if (irr_egress_replication_multicast_db_tbl_data.port[indx] ==
        SOC_PETRA_MULT_EG_UNOCCUPIED_PORT_INDICATION)
      {
        *mc_group_size = *mc_group_size - 1 ;
      }
      
      mc_group[indx].eg_entry.type = SOC_PETRA_MULT_EG_ENTRY_TYPE_OFP;
      mc_group[indx].eg_entry.port =
        irr_egress_replication_multicast_db_tbl_data.port[indx];
      mc_group[indx].eg_entry.cud =
        irr_egress_replication_multicast_db_tbl_data.out_lif[indx];
    }

    *next_entry =
      (uint16)irr_egress_replication_multicast_db_tbl_data.link_ptr;
  }
 

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_multicast_assert_egress_replication_hw_table()",0,0);
}

/*********************************************************************
*     This function adds the content of the entry into the last entry of
*     mc-group link list, if the entry has an unoccupied space.
*********************************************************************/
uint32
  soc_petra_mult_eg_fill_in_last_entry(
    SOC_SAND_IN int               unit,
    SOC_SAND_IN uint16                tbl_entry_id,
    SOC_SAND_IN SOC_PETRA_MULT_ENTRY        *entry,
    SOC_SAND_OUT uint8              *success
  )
{
  uint32
    res;
  uint32
    indx;
  uint8
    succeeded = FALSE;
  SOC_PETRA_IRR_EGRESS_REPLICATION_MULTICAST_DB_TBL_DATA
    irr_egress_replication_multicast_db_tbl_data;


  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_EG_FILL_IN_LAST_ENTRY);

  SOC_SAND_CHECK_NULL_INPUT(entry);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = SOC_SAND_OK; sal_memset(
          &irr_egress_replication_multicast_db_tbl_data,
          0,
          sizeof(SOC_PETRA_IRR_EGRESS_REPLICATION_MULTICAST_DB_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_irr_egress_replication_multicast_db_tbl_get_unsafe(
          unit,
          tbl_entry_id,
          &irr_egress_replication_multicast_db_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  for (indx = 0;
       indx < SOC_PETRA_MULT_EG_NOF_PORTS_PER_ENTRY;
       indx++)
  {
     if (!succeeded)
     {
       if (irr_egress_replication_multicast_db_tbl_data.port[indx]==
         SOC_PETRA_MULT_EG_UNOCCUPIED_PORT_INDICATION)
       {
         /*this part of entry is not-occupied*/
         irr_egress_replication_multicast_db_tbl_data.port[indx] =
           entry->eg_entry.port;
         irr_egress_replication_multicast_db_tbl_data.out_lif[indx] =
           entry->eg_entry.cud;

         succeeded = TRUE;
       }
     }
  }
  irr_egress_replication_multicast_db_tbl_data.link_ptr = soc_petra_mc_null_id(unit, FALSE);
  res = soc_petra_irr_egress_replication_multicast_db_tbl_set_unsafe(
          unit,
          tbl_entry_id,
          &irr_egress_replication_multicast_db_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  *success = succeeded;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_eg_fill_in_last_entry()",0,0);
}

/*********************************************************************
*     This function returns the number of elements that are used per entry.
*********************************************************************/
uint32
  soc_petra_mult_eg_nof_occupied_elements_in_tbl_entry(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint16                tbl_entry_id,
    SOC_SAND_OUT uint32               *size
  )
{
  uint32
    res;
  uint32
    indx,
    tmp_size = 0;
  SOC_PETRA_IRR_EGRESS_REPLICATION_MULTICAST_DB_TBL_DATA
    irr_egress_replication_multicast_db_tbl_data;


  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_EG_NOF_OCCUPIED_ELEMENTS_IN_TBL_ENTRY);

  SOC_SAND_CHECK_NULL_INPUT(size);

  res = SOC_SAND_OK; sal_memset(
          &irr_egress_replication_multicast_db_tbl_data,
          0,
          sizeof(SOC_PETRA_IRR_EGRESS_REPLICATION_MULTICAST_DB_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_irr_egress_replication_multicast_db_tbl_get_unsafe(
          unit,
          tbl_entry_id,
          &irr_egress_replication_multicast_db_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  
  for (indx = 0;
       indx < SOC_PETRA_MULT_EG_NOF_PORTS_PER_ENTRY;
       indx++)
  {
     if (irr_egress_replication_multicast_db_tbl_data.port[indx]!=
       SOC_PETRA_MULT_EG_UNOCCUPIED_PORT_INDICATION)
     {
       /*this part of entry is occupied*/
       tmp_size++;
     }
  }

  *size = tmp_size;

#ifdef LINK_PB_LIBRARIES
  if (SOC_PETRA_IS_DEV_PETRA_B_OR_ABOVE)
  {
    /* Format select indicates format entry, nof occupied changes per entry */
    if (irr_egress_replication_multicast_db_tbl_data.port[0]==
      SOC_PB_MULT_EG_FORMAT_SELECT_B)
    {
      res = soc_pb_mult_eg_nof_occupied_elements_in_tbl_entry(
              unit,
              tbl_entry_id,
              SOC_PETRA_MULT_EG_FORMAT_B,
              size
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
    }
    if (irr_egress_replication_multicast_db_tbl_data.port[0]==
      SOC_PB_MULT_EG_FORMAT_SELECT_C)
    {
      res = soc_pb_mult_eg_nof_occupied_elements_in_tbl_entry(
              unit,
              tbl_entry_id,
              SOC_PETRA_MULT_EG_FORMAT_C,
              size
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

    }
  }
#endif

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_eg_nof_occupied_elements_in_tbl_entry()",0,0);
}

#include <soc/dpp/SAND/Utils/sand_footer.h>
