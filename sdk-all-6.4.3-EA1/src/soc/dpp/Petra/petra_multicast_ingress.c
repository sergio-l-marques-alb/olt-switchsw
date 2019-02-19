/* $Id: petra_multicast_ingress.c,v 1.9 Broadcom SDK $
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
*
* FILENAME:       DuneDriver/soc_petra/src/soc_petra_multicast_ingress.c
*
* MODULE PREFIX:  soc_petra_mult_ing
*
* FILE DESCRIPTION: refer to the H file.
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

#include <soc/dpp/Petra/petra_multicast_ingress.h>
#include <soc/dpp/Petra/petra_multicast_linked_list.h>
#include <soc/dpp/Petra/petra_reg_access.h>
#include <soc/dpp/Petra/petra_tbl_access.h>
#include <soc/dpp/Petra/petra_general.h>
#include <soc/dpp/Petra/petra_sw_db.h>
#include <soc/dpp/Petra/petra_chip_tbls.h>
#include <soc/dpp/Petra/petra_ports.h>

#include <soc/dpp/SAND/Utils/sand_bitstream.h>
#include <soc/dpp/SAND/Utils/sand_conv.h>

#ifdef LINK_PB_LIBRARIES
#include <soc/dpp/Petra/PB_TM/pb_multicast_fabric.h>
#endif

/* } */

/*************
 * DEFINES   *
 *************/
/* { */

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
*     Maps the embedded traffic class in the packet header to
*     a logical traffic class. This logical traffic class will
*     be further used for traffic management. Note that a class
*     that is mapped to class '0' is equivalent to disabling
*     adding the class.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_mult_ing_traffic_class_map_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_ING_TR_CLS_MAP *map
  )
{
  uint32
    res;
  uint8
    tr_cls_ndx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_ING_TRAFFIC_CLASS_MAP_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(map);

  SOC_SAND_MAGIC_NUM_VERIFY(map);

  for (tr_cls_ndx = 0; tr_cls_ndx < SOC_PETRA_NOF_TRAFFIC_CLASSES; tr_cls_ndx++)
  {
    res = soc_petra_traffic_class_verify(map->map[tr_cls_ndx]);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_ing_traffic_class_map_verify()",0,0);
}

/*********************************************************************
*     Maps the embedded traffic class in the packet header to
*     a logical traffic class. This logical traffic class will
*     be further used for traffic management. Note that a class
*     that is mapped to class '0' is equivalent to disabling
*     adding the class.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_mult_ing_traffic_class_map_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_ING_TR_CLS_MAP *map
  )
{
  uint32
    new_class,
    res;
  uint8
    tr_cls_ndx;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_ING_TRAFFIC_CLASS_MAP_SET_UNSAFE);

  regs = soc_petra_regs();

  SOC_SAND_CHECK_NULL_INPUT(map);

 for (tr_cls_ndx = 0; tr_cls_ndx < SOC_PETRA_NOF_TRAFFIC_CLASSES; tr_cls_ndx++)
  {
    new_class = map->map[tr_cls_ndx];
    SOC_PA_FLD_SET(regs->irr.ingress_mc_tc_mapping_reg.ingress_mc_tc_mapping[tr_cls_ndx], new_class, 10, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_ing_traffic_class_map_set_unsafe()",0,0);
}

/*********************************************************************
*     Maps the embedded traffic class in the packet header to
*     a logical traffic class. This logical traffic class will
*     be further used for traffic management. Note that a class
*     that is mapped to class '0' is equivalent to disabling
*     adding the class.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_mult_ing_traffic_class_map_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_MULT_ING_TR_CLS_MAP *map
  )
{
  uint32
    res,
    new_class;
  uint8
    tr_cls_ndx;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_ING_TRAFFIC_CLASS_MAP_GET_UNSAFE);

  regs = soc_petra_regs();

  SOC_SAND_CHECK_NULL_INPUT(map);

  for (tr_cls_ndx = 0; tr_cls_ndx < SOC_PETRA_NOF_TRAFFIC_CLASSES; tr_cls_ndx++)
  {
    SOC_PA_FLD_GET(regs->irr.ingress_mc_tc_mapping_reg.ingress_mc_tc_mapping[tr_cls_ndx], new_class, 10, exit);
    map->map[tr_cls_ndx] = new_class;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_ing_traffic_class_map_get_unsafe()",0,0);
}




/*********************************************************************
*     This API enables the ingress-multicast-replication for
*     the specific multicast-id, and creates in the device the
*     needed link-list. The user specifies the multicast-id,
*     which is as the entry point to the link-list. All inner
*     link-list nodes are allocated and handled by the driver.
*     This function also configures the table which indicates
*     per Multicast ID whether to perform ingress replication
*     or not, if ingress replication is not chosen, fabric or
*     egress multicast will be performed on the packet.
*********************************************************************/
uint32
  soc_petra_mult_ing_group_open_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_ID             multicast_id_ndx,
    SOC_SAND_IN  SOC_PETRA_MULT_ING_ENTRY      *mc_group,
    SOC_SAND_IN  uint32                  mc_group_size,
    SOC_SAND_OUT uint8                 *insufficient_memory
  )
{
  uint32
    res;
  uint32
    indx;
  SOC_PETRA_MULT_ENTRY
    *mc_group_inner = NULL;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_ING_GROUP_OPEN_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(mc_group);

  SOC_SAND_CHECK_NULL_INPUT(insufficient_memory);

  SOC_PETRA_ALLOC(mc_group_inner, SOC_PETRA_MULT_ENTRY, mc_group_size);

  for (indx = 0 ; indx < mc_group_size ; indx++)
  {
    SOC_PETRA_COPY(&(mc_group_inner[indx].ing_entry), &(mc_group[indx]), SOC_PETRA_MULT_ING_ENTRY, 1);
  }

#ifdef LINK_PB_LIBRARIES
  /*
   * Verify the Multicast Group size for Soc_petra-B
   */
  res = soc_pb_mult_fabric_ingress_single_copy_range_b0_verify(
          unit,
          multicast_id_ndx,
          mc_group_size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);
#endif


  res = soc_petra_mult_group_open_unsafe_joint(
          unit,
          multicast_id_ndx,
          mc_group_inner,
          mc_group_size,
          TRUE,
          insufficient_memory
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_PETRA_FREE(mc_group_inner);
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_ing_group_open_unsafe()",0,0);
}

/*********************************************************************
*     This API updates ingress-multicast-replication for the
*     specific multicast-id, and re-allocate if needed the
*     devices' link-list. The user only specifies the
*     multicast-id, which is as the entry point to the
*     link-list. The driver handles all inner link-list nodes
*     for allocation/free/moves. This function also configures
*     the table which indicates per Multicast ID whether to
*     perform ingress replication or not, if ingress
*     replication is not chosen, fabric or egress multicast
*     will be performed on the packet.
*********************************************************************/
uint32
  soc_petra_mult_ing_group_update_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_ID             multicast_id_ndx,
    SOC_SAND_IN  SOC_PETRA_MULT_ING_ENTRY      *mc_group,
    SOC_SAND_IN  uint32                  mc_group_size,
    SOC_SAND_OUT uint8                 *insufficient_memory
  )
{
  uint32
    res;
  uint32
    indx;
  SOC_PETRA_MULT_ENTRY
    *mc_group_inner = NULL;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_ING_GROUP_UPDATE_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(mc_group);
  SOC_SAND_CHECK_NULL_INPUT(insufficient_memory);

  SOC_PETRA_ALLOC(mc_group_inner, SOC_PETRA_MULT_ENTRY, mc_group_size);

  for (indx = 0 ; indx < mc_group_size ; indx++)
  {
    SOC_PETRA_COPY(&(mc_group_inner[indx].ing_entry), &(mc_group[indx]), SOC_PETRA_MULT_ING_ENTRY, 1);
  }

#ifdef LINK_PB_LIBRARIES
  /*
   * Verify the Multicast Group size for Soc_petra-B
   */
  res = soc_pb_mult_fabric_ingress_single_copy_range_b0_verify(
          unit,
          multicast_id_ndx,
          mc_group_size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);
#endif

  res = soc_petra_mult_group_update_unsafe_joint(
          unit,
          multicast_id_ndx,
          mc_group_inner,
          mc_group_size,
          TRUE,
          insufficient_memory
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);


exit:
  SOC_PETRA_FREE(mc_group_inner);
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_ing_group_update_unsafe()",0,0);

}

/*********************************************************************
*     This API closes ingress-multicast-replication for the
*     specific multicast-id, and frees the device's link-list.
*     The user only specifies the multicast-id, which is as
*     the entry point to the link-list. All inner link-list
*     nodes are freed and handled by the driver. This function
*     also configures the table which indicates per Multicast
*     ID whether to perform ingress replication or not, if
*     ingress replication is not chosen, fabric or egress
*     multicast will be performed on the packet.
*********************************************************************/
uint32
  soc_petra_mult_ing_group_close_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_ID             multicast_id_ndx
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_ING_GROUP_CLOSE_UNSAFE);


  res = soc_petra_mult_group_close_unsafe_joint(
          unit,
          multicast_id_ndx,
          TRUE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_ing_group_close_unsafe()",0,0);
}

/*********************************************************************
*     Add an entry, including a destination and an copy-unique-data, to
*     a multicast group indicated by a multicast-id. An error
*     is carried out in case of an invalid entry.
*********************************************************************/
uint32
  soc_petra_mult_ing_destination_add_unsafe_inner(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_ID             multicast_id_ndx,
    SOC_SAND_IN  SOC_PETRA_MULT_ING_ENTRY      *entry,
    SOC_SAND_OUT uint8                 *insufficient_memory
  )
{
  uint32
    size,
    res;
  SOC_PETRA_MULT_ENTRY
    *entry_inner = NULL;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_ING_DESTINATION_ADD_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(entry);
  SOC_SAND_CHECK_NULL_INPUT(insufficient_memory);

  SOC_PETRA_ALLOC(entry_inner, SOC_PETRA_MULT_ENTRY, 1);
  SOC_PETRA_COPY(&(entry_inner->ing_entry), entry, SOC_PETRA_MULT_ENTRY, 1);
  res = soc_petra_mult_ing_group_size_get_unsafe(
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
      TRUE,
      insufficient_memory
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }
  else
  {
    res = soc_petra_mult_ing_group_open_unsafe(
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
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_ing_destination_add_unsafe()",0,0);
}

uint32
  soc_petra_mult_ing_destination_add_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_ID             multicast_id_ndx,
    SOC_SAND_IN  SOC_PETRA_MULT_ING_ENTRY      *entry,
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
  SOC_PETRA_MULT_ING_ENTRY
    *mc_group = NULL;
  uint8
    is_group_open;
#endif /*SOC_PETRA_MULT_PORT_ADD_PREVENT_DUPLICATIONS*/
  uint8
    perform_copy = TRUE;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_ING_DESTINATION_ADD_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(entry);

  if (insufficient_memory != NULL)
  {
     *insufficient_memory = 0;
  }

#if SOC_PETRA_MULT_PORT_ADD_PREVENT_DUPLICATIONS
  res = soc_petra_mult_ing_group_size_get_unsafe(
    unit,
    multicast_id_ndx,
    &mc_group_size
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

#ifdef LINK_PB_LIBRARIES
  /*
   * Verify the Multicast Group size for Soc_petra-B
   */
  res = soc_pb_mult_fabric_ingress_single_copy_range_b0_verify(
          unit,
          multicast_id_ndx,
          mc_group_size+1 /* The new destination not added already */
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);
#endif

  SOC_PETRA_ALLOC(mc_group, SOC_PETRA_MULT_ING_ENTRY, (mc_group_size+1));

  res = soc_petra_mult_ing_group_get_unsafe(
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
    if (mc_group[indx].destination.type == entry->destination.type &&
        mc_group[indx].destination.id == entry->destination.id &&
        mc_group[indx].cud == entry->cud)
    {
      /* Copy already exist */
      perform_copy = FALSE;
    }
  }
#endif /*SOC_PETRA_MULT_PORT_ADD_PREVENT_DUPLICATIONS*/

  if (perform_copy)
  {
#if SOC_PETRA_MULT_USE_SMART_ADD_ENTRY
    res = soc_petra_mult_ing_destination_add_unsafe_inner(
      unit,
      multicast_id_ndx,
      entry,
      insufficient_memory
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
#else /*SOC_PETRA_MULT_USE_SMART_ADD_ENTRY*/
    mc_group[mc_group_size].cud   = entry->cud;
    mc_group[mc_group_size].destination.id  = entry->destination.id;
    mc_group[mc_group_size].destination.type  = entry->destination.type;
    mc_group_size++;

    res = soc_petra_mult_ing_group_open_unsafe(
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
        ex, "error in soc_petra_mult_ing_port_add_unsafe()", exit_place,multicast_id_ndx,entry->destination.type,entry->destination.id,entry->cud,0
        );
    }
    return ex;
  }
  else
  {
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_ing_port_add_unsafe()",multicast_id_ndx,0);
  }
}
/*********************************************************************
*     Removes an entry, including a destination and an copy-unique-data,
*     from a multicast group indicated by a multicast-id. An
*     error is carried out in case of an invalid entry.
*********************************************************************/
uint32
  soc_petra_mult_ing_destination_remove_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_ID             multicast_id_ndx,
    SOC_SAND_IN  SOC_PETRA_MULT_ING_ENTRY      *entry
  )
{
  uint32
    res;
  uint32
    mc_group_size,
    exact_mc_group_size,
    indx;
  SOC_PETRA_MULT_ING_ENTRY
    *mc_group = NULL;
  uint8
    is_group_open,
    insufficient_memory,
    perform_copy = FALSE;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_ING_DESTINATION_REMOVE_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(entry);

  res = soc_petra_mult_ing_group_size_get_unsafe(
          unit,
          multicast_id_ndx,
          &mc_group_size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (mc_group_size == 0)
  {
    SOC_PETRA_DO_NOTHING_AND_EXIT;
  }

  SOC_PETRA_ALLOC(mc_group, SOC_PETRA_MULT_ING_ENTRY, mc_group_size);

  res = soc_petra_mult_ing_group_get_unsafe(
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
    if ((mc_group[indx].cud == entry->cud) &&
        (mc_group[indx].destination.id == entry->destination.id) &&
        (mc_group[indx].destination.type == entry->destination.type))
    {
       /* we have to start the shift of the list */
       perform_copy = TRUE;
    }
    if (perform_copy)
    {
      if (indx == (mc_group_size - 1))
      {
        mc_group[indx].destination.id =
          SOC_PETRA_MULT_ING_UNOCCUPIED_BASE_QUEUE_INDICATION;
        mc_group[indx].destination.type =
          SOC_PETRA_DEST_TYPE_QUEUE;
        mc_group[indx].cud =
          SOC_PETRA_MULT_OCCUPIED_BUT_EMPTY_ENTRY_CUD_INDICATION;
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

    res = soc_petra_mult_ing_group_update_unsafe(
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

exit:
  SOC_PETRA_FREE(mc_group);
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_ing_destination_remove_unsafe()",0,0);

}

/*********************************************************************
*     Returns the size of the multicast group with the
*     specified multicast id.
*********************************************************************/
uint32
  soc_petra_mult_ing_group_size_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_ID             multicast_id_ndx,
    SOC_SAND_OUT uint32                  *mc_group_size
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_ING_GROUP_SIZE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(mc_group_size);

  res = soc_petra_mult_group_size_get_unsafe_joint(
          unit,
          multicast_id_ndx,
          TRUE,
          mc_group_size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_ing_group_size_get_unsafe()",0,0);
}

/*********************************************************************
*     Gets the multicast group with the specified multicast
*     id.
*********************************************************************/
uint32
  soc_petra_mult_ing_group_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_ID             multicast_id_ndx,
    SOC_SAND_IN  uint32                 mc_group_size,
    SOC_SAND_OUT SOC_PETRA_MULT_ING_ENTRY      *mc_group,
    SOC_SAND_OUT uint32                  *exact_mc_group_size,
    SOC_SAND_OUT uint8                 *is_open
  )
{
  uint32
    res,
    lag_id,
    lag_member_id,
    sys_phys_port_id;
  uint16
    tbl_entry_id;
  uint32
    indx,
    indx_in_entry;
  uint8
    is_lag_not_phys;
  SOC_PETRA_IRR_INGRESS_REPLICATION_MULTICAST_DB_TBL_DATA
    irr_ingress_replication_multicast_db_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_ING_GROUP_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(mc_group);
  SOC_SAND_CHECK_NULL_INPUT(exact_mc_group_size);
  SOC_SAND_CHECK_NULL_INPUT(is_open);

  res = SOC_SAND_OK; sal_memset(
          &irr_ingress_replication_multicast_db_tbl_data,
          0,
          sizeof(SOC_PETRA_IRR_INGRESS_REPLICATION_MULTICAST_DB_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);


  /* check that size is ok */
  res = soc_petra_mult_ing_group_size_get_unsafe(
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
  tbl_entry_id = (uint16)multicast_id_ndx;

  for (indx = 0 ; indx < *exact_mc_group_size ; indx++)
  {
    indx_in_entry =
      indx % SOC_PETRA_MULT_ING_NOF_DESTINATIONS_PER_ENTRY;

    if (indx_in_entry == 0)
    {
      res = soc_petra_irr_ingress_replication_multicast_db_tbl_get_unsafe(
              unit,
              tbl_entry_id,
              &irr_ingress_replication_multicast_db_tbl_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

      tbl_entry_id =
        (uint16)irr_ingress_replication_multicast_db_tbl_data.link_ptr;
    }

    if (irr_ingress_replication_multicast_db_tbl_data.is_queue_number[indx_in_entry]
          == TRUE)
    {
      mc_group[indx].destination.type = SOC_PETRA_DEST_TYPE_QUEUE;
      mc_group[indx].destination.id =
        irr_ingress_replication_multicast_db_tbl_data.base_queue[indx_in_entry];
    }
    else
    {
      if (
        (SOC_SAND_BIT(SOC_PETRA_SYS_PORT_ID_MULT_ID_BIT) /*16384*/ &
        irr_ingress_replication_multicast_db_tbl_data.base_queue[indx_in_entry])
        != 0
        )
      {
        mc_group[indx].destination.type = SOC_PETRA_DEST_TYPE_MULTICAST;
        mc_group[indx].destination.id =
          irr_ingress_replication_multicast_db_tbl_data.base_queue[indx_in_entry] -
          SOC_PETRA_MULT_LL_EGRESS_MC_ID_START_ENTRY;
      }
      else
      {
        res = soc_petra_ports_logical_sys_id_parse(
                irr_ingress_replication_multicast_db_tbl_data.base_queue[indx_in_entry],
                &is_lag_not_phys,
                &lag_id,
                &lag_member_id,
                &sys_phys_port_id
              );
       SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

       if (is_lag_not_phys)
       {
         mc_group[indx].destination.type = SOC_PETRA_DEST_TYPE_LAG;
         mc_group[indx].destination.id = lag_id;
       }
       else /* type == sys_phy_port*/
       {
         mc_group[indx].destination.type = SOC_PETRA_DEST_TYPE_SYS_PHY_PORT;
         mc_group[indx].destination.id = sys_phys_port_id;
       }
      }
    }
    mc_group[indx].cud =
      irr_ingress_replication_multicast_db_tbl_data.out_lif[indx_in_entry];
  }

  *is_open = *exact_mc_group_size == 0 ?
    soc_petra_mult_is_multicast_id_group_exist(unit, multicast_id_ndx) :
    TRUE;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_ing_group_get_unsafe()",0,0);
}

/*********************************************************************
*     Closes all opened egress multicast groups.
*********************************************************************/
uint32
  soc_petra_mult_ing_all_groups_close_unsafe(
    SOC_SAND_IN  int                 unit
  )
{
  uint32
    res;
  uint32
    indx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_ING_ALL_GROUPS_CLOSE_UNSAFE);

  for (indx = 0 ; indx < SOC_PETRA_MULT_NOF_MULTICAST_GROUPS ; indx++)
  {
    res = soc_petra_mult_ing_group_close_unsafe(
            unit,
            indx
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_ing_all_groups_close_unsafe()",0,0);
}


/*********************************************************************
*     Enters the multicast group part into the specified entry.
*********************************************************************/
uint32
  soc_petra_mult_ing_multicast_group_entry_to_tbl(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 in_group_index,
    SOC_SAND_IN  uint32                  offset,
    SOC_SAND_IN  SOC_PETRA_MULT_ENTRY          *mc_group,
    SOC_SAND_IN  uint32                  mc_group_size,
    SOC_SAND_IN  uint16                  next_entry
  )
{
  uint32
    res,
    sys_port;
  uint32
    in_group_i,
    indx,
    progress_index_by;
  SOC_PETRA_IRR_INGRESS_REPLICATION_MULTICAST_DB_TBL_DATA
    irr_ingress_replication_multicast_db_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_ING_MULTICAST_GROUP_ENTRY_TO_TBL);

  SOC_SAND_CHECK_NULL_INPUT(mc_group);

  /*in ingress- 2 elements per entry*/
  progress_index_by = SOC_PETRA_MULT_ING_NOF_DESTINATIONS_PER_ENTRY;

  res = SOC_SAND_OK; sal_memset(
          &irr_ingress_replication_multicast_db_tbl_data,
          0,
          sizeof(SOC_PETRA_IRR_INGRESS_REPLICATION_MULTICAST_DB_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);


  for (in_group_i = in_group_index; in_group_i < (in_group_index + progress_index_by); in_group_i++)
  {
    indx = in_group_i % progress_index_by;

    if (in_group_i < mc_group_size)
    {
      if (mc_group[in_group_i].ing_entry.destination.type == SOC_PETRA_DEST_TYPE_QUEUE)
      {
        /* direct Queue_id */
        irr_ingress_replication_multicast_db_tbl_data.is_queue_number[indx] =
          TRUE;
        irr_ingress_replication_multicast_db_tbl_data.base_queue[indx] =
          mc_group[in_group_i].ing_entry.destination.id;
      }
      else
      {
        irr_ingress_replication_multicast_db_tbl_data.is_queue_number[indx] =
          FALSE;
        if (mc_group[in_group_i].ing_entry.destination.type == SOC_PETRA_DEST_TYPE_MULTICAST)
        {
          /* multicast_id */
          irr_ingress_replication_multicast_db_tbl_data.base_queue[indx] =
          (SOC_PETRA_MULT_LL_EGRESS_MC_ID_START_ENTRY | mc_group[in_group_i].ing_entry.destination.id);
        }
        else
        {
          /* sys_port_id */

          if (mc_group[in_group_i].ing_entry.destination.type
                == SOC_PETRA_DEST_TYPE_LAG)
          {
            res = soc_petra_ports_logical_sys_id_build(
                    TRUE,
                    mc_group[in_group_i].ing_entry.destination.id,
                    0,
                    0,
                    &sys_port
                  );
            SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
          }
          else  /* destination-type == sys_phy_port*/
          {
            res = soc_petra_ports_logical_sys_id_build(
                    FALSE,
                    0,
                    0,
                    mc_group[in_group_i].ing_entry.destination.id,
                    &sys_port
                  );
            SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
          }

          irr_ingress_replication_multicast_db_tbl_data.base_queue[indx] =
            sys_port;
        }
      }

      irr_ingress_replication_multicast_db_tbl_data.out_lif[indx] =
        mc_group[in_group_i].ing_entry.cud;

    }
    else
    {
      /* 'fill occupied but empty' entry destination with 0xBFFF */
      irr_ingress_replication_multicast_db_tbl_data.is_queue_number[indx] =
        TRUE;
      irr_ingress_replication_multicast_db_tbl_data.base_queue[indx] =
        SOC_PETRA_MULT_ING_UNOCCUPIED_BASE_QUEUE_INDICATION;  /* 0x7FFF */
      irr_ingress_replication_multicast_db_tbl_data.out_lif[indx] =
        SOC_PETRA_MULT_OCCUPIED_BUT_EMPTY_ENTRY_CUD_INDICATION;
    }

  }

  irr_ingress_replication_multicast_db_tbl_data.link_ptr =
    next_entry;

   /* Write entry to HW*/
  res = soc_petra_irr_ingress_replication_multicast_db_tbl_set_unsafe(
          unit,
          offset,
          &irr_ingress_replication_multicast_db_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_ing_multicast_group_entry_to_tbl()",0,0);
}

/*********************************************************************
*     Returns a multicast group containing the elements of a mc-id
*     entry in the multicast table.
*********************************************************************/
uint32
 soc_petra_mult_ing_entry_content_get(
   SOC_SAND_IN  int                unit,
   SOC_SAND_IN  SOC_PETRA_MULT_ID            multicast_id_ndx,
   SOC_SAND_OUT SOC_PETRA_MULT_ENTRY         *mc_group,
   SOC_SAND_OUT uint32                *mc_group_size,
   SOC_SAND_OUT uint16                 *next_entry
 )
{
  uint32
    res,
    lag_id,
    lag_member_id,
    sys_phys_port_id;
  uint8
    is_lag_not_phys;
  uint32
    indx,
    mc_group_size_var;
  SOC_PETRA_IRR_INGRESS_REPLICATION_MULTICAST_DB_TBL_DATA
    irr_ingress_replication_multicast_db_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_ING_ENTRY_CONTENT_GET);

  SOC_SAND_CHECK_NULL_INPUT(mc_group);
  SOC_SAND_CHECK_NULL_INPUT(mc_group_size);
  SOC_SAND_CHECK_NULL_INPUT(next_entry);

  mc_group_size_var = SOC_PETRA_MULT_ING_NOF_DESTINATIONS_PER_ENTRY;

  res = SOC_SAND_OK; sal_memset(
          &irr_ingress_replication_multicast_db_tbl_data,
          0,
          sizeof(SOC_PETRA_IRR_INGRESS_REPLICATION_MULTICAST_DB_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);


  res = soc_petra_irr_ingress_replication_multicast_db_tbl_get_unsafe(
          unit,
          multicast_id_ndx,
          &irr_ingress_replication_multicast_db_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  for (indx = 0 ; indx < SOC_PETRA_MULT_ING_NOF_DESTINATIONS_PER_ENTRY; indx++)
  {
    if (irr_ingress_replication_multicast_db_tbl_data.is_queue_number[indx]
          == TRUE)
    {
      if (irr_ingress_replication_multicast_db_tbl_data.base_queue[indx] ==
           SOC_PETRA_MULT_ING_UNOCCUPIED_BASE_QUEUE_INDICATION)
      {
        mc_group_size_var-- ;
      }
      mc_group[indx].ing_entry.destination.type = SOC_PETRA_DEST_TYPE_QUEUE;
      mc_group[indx].ing_entry.destination.id =
        irr_ingress_replication_multicast_db_tbl_data.base_queue[indx];
    }
    else
    {
      if (
           (SOC_SAND_BIT(SOC_PETRA_SYS_PORT_ID_MULT_ID_BIT) /*16384*/ &
             irr_ingress_replication_multicast_db_tbl_data.base_queue[indx])
             != 0
         )
      {
        mc_group[indx].ing_entry.destination.type = SOC_PETRA_DEST_TYPE_MULTICAST;
        mc_group[indx].ing_entry.destination.id =
          irr_ingress_replication_multicast_db_tbl_data.base_queue[indx];
      }
      else
      {
        res = soc_petra_ports_logical_sys_id_parse(
                irr_ingress_replication_multicast_db_tbl_data.base_queue[indx],
                &is_lag_not_phys,
                &lag_id,
                &lag_member_id,
                &sys_phys_port_id
              );
       SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

       if (is_lag_not_phys)
       {
         mc_group[indx].ing_entry.destination.type = SOC_PETRA_DEST_TYPE_LAG;
         mc_group[indx].ing_entry.destination.id = lag_id;
       }
       else /* type == sys_phy_port*/
       {
         mc_group[indx].ing_entry.destination.type = SOC_PETRA_DEST_TYPE_SYS_PHY_PORT;
         mc_group[indx].ing_entry.destination.id = sys_phys_port_id;
       }
      }
    }

    mc_group[indx].ing_entry.cud =
      irr_ingress_replication_multicast_db_tbl_data.out_lif[indx];

  }
  *mc_group_size =
    mc_group_size_var;
  *next_entry =
    (uint16)irr_ingress_replication_multicast_db_tbl_data.link_ptr;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_multicast_assert_ingress_replication_hw_table()",0,0);
}

/*********************************************************************
*     Returns the id of the next entry in the table
*     (pointed at by the reference to the linked-list).
*********************************************************************/
uint32
  soc_petra_mult_ing_next_link_list_ptr_set(
    SOC_SAND_IN  int           unit,
    SOC_SAND_IN  uint16            tbl_entry_id,
    SOC_SAND_IN  uint16            link_list_ptr
  )
{
  uint32
    res;
  SOC_PETRA_IRR_INGRESS_REPLICATION_MULTICAST_DB_TBL_DATA
    irr_ingress_replication_multicast_db_tbl_data;


  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_ING_NEXT_LINK_LIST_PTR_SET);

  res = SOC_SAND_OK; sal_memset(
    &irr_ingress_replication_multicast_db_tbl_data,
    0,
    sizeof(SOC_PETRA_IRR_INGRESS_REPLICATION_MULTICAST_DB_TBL_DATA)
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = soc_petra_irr_ingress_replication_multicast_db_tbl_get_unsafe(
          unit,
          tbl_entry_id,
          &irr_ingress_replication_multicast_db_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  irr_ingress_replication_multicast_db_tbl_data.link_ptr =
    link_list_ptr;

  res = soc_petra_irr_ingress_replication_multicast_db_tbl_set_unsafe(
          unit,
          tbl_entry_id,
          &irr_ingress_replication_multicast_db_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_ing_next_link_list_ptr_set()",0,0);
}

/*********************************************************************
*     Returns the id of the next entry in the table
*     (pointed at by the reference to the linked-list).
*********************************************************************/
uint32
  soc_petra_mult_ing_next_link_list_ptr_get(
    SOC_SAND_IN  int           unit,
    SOC_SAND_IN  uint16            tbl_entry_id,
    SOC_SAND_OUT uint16            *link_list_ptr
  )
{
  uint32
    res;
  SOC_PETRA_IRR_INGRESS_REPLICATION_MULTICAST_DB_TBL_DATA
    irr_ingress_replication_multicast_db_tbl_data;


  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_ING_NEXT_LINK_LIST_PTR_GET);

  SOC_SAND_CHECK_NULL_INPUT(link_list_ptr);

  res = SOC_SAND_OK; sal_memset(
    &irr_ingress_replication_multicast_db_tbl_data,
    0,
    sizeof(SOC_PETRA_IRR_INGRESS_REPLICATION_MULTICAST_DB_TBL_DATA)
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = soc_petra_irr_ingress_replication_multicast_db_tbl_get_unsafe(
          unit,
          tbl_entry_id,
          &irr_ingress_replication_multicast_db_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  *link_list_ptr =
    (uint16)irr_ingress_replication_multicast_db_tbl_data.link_ptr;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_ing_next_link_list_ptr_get()",0,0);
}

/*********************************************************************
*     This function adds the content of the entry into the last entry of
*     mc-group link list, if the entry has an unoccupied space.
*********************************************************************/
uint32
  soc_petra_mult_ing_fill_in_last_entry(
    SOC_SAND_IN int               unit,
    SOC_SAND_IN uint16                tbl_entry_id,
    SOC_SAND_IN SOC_PETRA_MULT_ENTRY        *entry,
    SOC_SAND_OUT uint8              *success
  )
{
  uint32
    res,
    sys_port;
  uint32
    indx;
  uint8
    succeeded = FALSE;
  SOC_PETRA_IRR_INGRESS_REPLICATION_MULTICAST_DB_TBL_DATA
    irr_ingress_replication_multicast_db_tbl_data;


  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_ING_FILL_IN_LAST_ENTRY);

  SOC_SAND_CHECK_NULL_INPUT(entry);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = SOC_SAND_OK; sal_memset(
          &irr_ingress_replication_multicast_db_tbl_data,
          0,
          sizeof(SOC_PETRA_IRR_INGRESS_REPLICATION_MULTICAST_DB_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = soc_petra_irr_ingress_replication_multicast_db_tbl_get_unsafe(
          unit,
          tbl_entry_id,
          &irr_ingress_replication_multicast_db_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  for (indx = 0;
       indx < SOC_PETRA_MULT_ING_NOF_DESTINATIONS_PER_ENTRY;
       indx++)
  {
     if (!succeeded)
     {
        if (irr_ingress_replication_multicast_db_tbl_data.base_queue[indx]==
              SOC_PETRA_MULT_ING_UNOCCUPIED_BASE_QUEUE_INDICATION)
        {
          /*this part of entry is not-occupied*/
          if (entry->ing_entry.destination.type == SOC_PETRA_DEST_TYPE_QUEUE)
          {
            irr_ingress_replication_multicast_db_tbl_data.is_queue_number[indx] =
            TRUE;
            irr_ingress_replication_multicast_db_tbl_data.base_queue[indx]=
            entry->ing_entry.destination.id;
          }
          else
          {
            irr_ingress_replication_multicast_db_tbl_data.is_queue_number[indx] =
              FALSE;
            if (entry->ing_entry.destination.type == SOC_PETRA_DEST_TYPE_MULTICAST)
            {
              irr_ingress_replication_multicast_db_tbl_data.base_queue[indx] =
              (SOC_PETRA_MULT_LL_EGRESS_MC_ID_START_ENTRY | entry->ing_entry.destination.id);
            }
            else
            {
              if (entry->ing_entry.destination.type == SOC_PETRA_DEST_TYPE_LAG)
              {
                res = soc_petra_ports_logical_sys_id_build(
                        TRUE,
                        entry->ing_entry.destination.id,
                        0,
                        0,
                        &sys_port
                      );
                SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
              }
              else  /* destination-type == sys_phy_port*/
              {
                res = soc_petra_ports_logical_sys_id_build(
                        FALSE,
                        0,
                        0,
                        entry->ing_entry.destination.id,
                        &sys_port
                      );
                SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
              }
              irr_ingress_replication_multicast_db_tbl_data.base_queue[indx] =
                sys_port;
            }
          }
          irr_ingress_replication_multicast_db_tbl_data.out_lif[indx] =
            entry->ing_entry.cud;

          succeeded = TRUE;
        }
    }
  }

  irr_ingress_replication_multicast_db_tbl_data.link_ptr = soc_petra_mc_null_id(unit, TRUE);
  res = soc_petra_irr_ingress_replication_multicast_db_tbl_set_unsafe(
          unit,
          tbl_entry_id,
          &irr_ingress_replication_multicast_db_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  *success = succeeded;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_ing_fill_in_last_entry()",0,0);
}

/*********************************************************************
*     This function returns the number of elements that are used per entry.
*********************************************************************/
uint32
  soc_petra_mult_ing_nof_occupied_elements_in_tbl_entry(
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
  SOC_PETRA_IRR_INGRESS_REPLICATION_MULTICAST_DB_TBL_DATA
    irr_ingress_replication_multicast_db_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_ING_NOF_OCCUPIED_ELEMENTS_IN_TBL_ENTRY);

  SOC_SAND_CHECK_NULL_INPUT(size);

  res = SOC_SAND_OK; sal_memset(
          &irr_ingress_replication_multicast_db_tbl_data,
          0,
          sizeof(SOC_PETRA_IRR_INGRESS_REPLICATION_MULTICAST_DB_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = soc_petra_irr_ingress_replication_multicast_db_tbl_get_unsafe(
          unit,
          tbl_entry_id,
          &irr_ingress_replication_multicast_db_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  for (indx = 0;
       indx < SOC_PETRA_MULT_ING_NOF_DESTINATIONS_PER_ENTRY;
       indx++)
  {
     if ((irr_ingress_replication_multicast_db_tbl_data.base_queue[indx]==
       SOC_PETRA_MULT_ING_UNOCCUPIED_BASE_QUEUE_INDICATION) &&
       (irr_ingress_replication_multicast_db_tbl_data.is_queue_number[indx]==
       TRUE))
     {
       /*this part of entry is not-occupied*/
       tmp_size++;
     }
  }

  *size = SOC_PETRA_MULT_ING_NOF_DESTINATIONS_PER_ENTRY - tmp_size;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_ing_nof_occupied_elements_in_tbl_entry()",0,0);
}


#include <soc/dpp/SAND/Utils/sand_footer.h>
