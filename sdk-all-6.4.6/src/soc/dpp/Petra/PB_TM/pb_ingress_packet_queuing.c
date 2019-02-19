/* $Id: pb_ingress_packet_queuing.c,v 1.6 Broadcom SDK $
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
/******************************************************************
*
* FILENAME:       DuneDriver/soc_petra/src/soc_pb_ingress_packet_queuing.c
*
* MODULE PREFIX:  soc_pb_ipq
*
* FILE DESCRIPTION:
*
* REMARKS:
* SW License Agreement: Dune Networks (c). CONFIDENTIAL PROPRIETARY INFORMATION.
* Any use of this Software is subject to Software License Agreement
* included in the Driver User Manual of this device.
* Any use of this Software constitutes an agreement to the terms
* of the above Software License Agreement.
*******************************************************************/


/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Utils/sand_bitstream.h>
#include <soc/dpp/SAND/Utils/sand_u64.h>

#include <soc/dpp/Petra/PB_TM/pb_ingress_packet_queuing.h>
#include <soc/dpp/Petra/PB_TM/pb_api_framework.h>
#include <soc/dpp/Petra/PB_TM/pb_pmf_low_level_fem_tag.h>
#include <soc/dpp/Petra/PB_TM/pb_tbl_access.h>
/* } */

/*************
 * DEFINES   *
 *************/
/* { */
/* Four entries for Dest-Sys-Port, two entries for Dest-Flow-Id for  */
#define SOC_PB_IPQ_NOF_UC_DEST_TC_ENTRIES         (6)

#define SOC_PB_IPQ_TC_PROFILE_DFLT                (0)

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
*     soc_pb_ipq_init
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
  soc_pb_ipq_init(
    SOC_SAND_IN  int                 unit
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PETRA_IPQ_TR_CLS
    cls_id;


  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IPQ_INIT);

  /*
   *  Traffic Class mapping -
   *	initialize to "no change"
   */
  for (cls_id = SOC_PETRA_IPQ_TR_CLS_MIN; cls_id <= SOC_PETRA_IPQ_TR_CLS_MAX; cls_id++)
  {
    res = soc_pb_ipq_traffic_class_map_set_unsafe(
            unit,
            cls_id,
            cls_id
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_ipq_init()",0,0);
}

/*********************************************************************
*     Sets the Explicit Flow Unicast packets mapping to queue.
*     Doesn't affect packets that arrive with destination_id
*     in the header.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_ipq_explicit_mapping_mode_info_set_unsafe(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  SOC_PETRA_IPQ_EXPLICIT_MAPPING_MODE_INFO *info
  )
{
  uint32
    offset,
    res;
  uint32
    fem_id,
    action_format_ndx;
  SOC_PB_PMF_FEM_NDX
    fem;
  SOC_PB_PMF_FEM_ACTION_FORMAT_INFO
    action_format_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IPQ_EXPLICIT_MAPPING_MODE_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  /*
   *	Modify in the TM Destination FEM (all TM Programs) the offset of the Unicast Flow
   *  See the PMF TM init
   */
  for (fem_id = 6; fem_id <= 7; ++fem_id)
  {
    SOC_PB_PMF_FEM_NDX_clear(&fem);
    fem.id = fem_id;
    fem.cycle_ndx = 1;
    action_format_ndx = 1;
    res = soc_pb_pmf_fem_action_format_get_unsafe(
            unit,
            &fem,
            action_format_ndx,
            &action_format_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    /*
     *	Compute the offset: the number if add, the 2's complement if decrement (17b)
     */
    if (info->queue_id_add_not_decrement == TRUE)
    {
      offset = info->base_queue_id;
    }
    else
    {
      if (info->base_queue_id == 0)
      {
        offset = 0;
      }
      else
      {
        offset = 0x1FFFF - (info->base_queue_id - 1);
      }
    }

    action_format_info.base_value = offset;

    res = soc_pb_pmf_fem_action_format_set_unsafe(
            unit,
            &fem,
            action_format_ndx,
            &action_format_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_ipq_explicit_mapping_mode_info_set_unsafe()",0,0);
}

/*********************************************************************
*     Sets the Explicit Flow Unicast packets mapping to queue.
*     Doesn't affect packets that arrive with destination_id
*     in the header.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_ipq_explicit_mapping_mode_info_get_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_OUT SOC_PETRA_IPQ_EXPLICIT_MAPPING_MODE_INFO *info
  )
{
  uint32
    offset,
    res;
  uint32
    action_format_ndx;
  SOC_PB_PMF_FEM_NDX
    fem;
  SOC_PB_PMF_FEM_ACTION_FORMAT_INFO
    action_format_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IPQ_EXPLICIT_MAPPING_MODE_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  /*
   *	Modify in the TM Destination FEM (all TM Programs) the offset of the Unicast Flow
   *  See the PMF TM init
   */
  SOC_PB_PMF_FEM_NDX_clear(&fem);
  fem.id = 6;
  fem.cycle_ndx = 1;
  action_format_ndx = 1;
  res = soc_pb_pmf_fem_action_format_get_unsafe(
          unit,
          &fem,
          action_format_ndx,
          &action_format_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*
   *	Extrapolate the offset: the number if add, the 2's complement if decrement (17b)
   */
  offset = action_format_info.base_value;
  if (SOC_SAND_GET_BIT(offset, 15) != 0x1)
  {
    info->queue_id_add_not_decrement = TRUE;
    info->base_queue_id = offset;
  }
  else /* Negative value*/
  {
    info->queue_id_add_not_decrement = FALSE;
    if (SOC_SAND_SET_BITS_RANGE(offset, 14, 0) == 0)
    {
      info->base_queue_id = offset;
    }
    else
    {
      info->base_queue_id = 0x1FFFF - (offset - 1);
    }
  }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_ipq_explicit_mapping_mode_info_get_unsafe()",0,0);
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
  soc_pb_ipq_traffic_class_map_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_IPQ_TR_CLS          tr_cls_ndx,
    SOC_SAND_IN  SOC_PETRA_IPQ_TR_CLS          new_class
  )
{
  uint32
    entry_offset,
    res;
  SOC_PB_IRR_TRAFFIC_CLASS_MAPPING_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IPQ_TRAFFIC_CLASS_MAP_SET_UNSAFE);

  for (entry_offset = 0; entry_offset < SOC_PB_IPQ_NOF_UC_DEST_TC_ENTRIES; ++entry_offset)
  {
    res = soc_pb_irr_traffic_class_mapping_tbl_get_unsafe(
            unit,
            entry_offset,
            &tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    tbl_data.traffic_class_mapping[tr_cls_ndx] = new_class;

    res = soc_pb_irr_traffic_class_mapping_tbl_set_unsafe(
            unit,
            entry_offset,
            &tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_ipq_traffic_class_map_set_unsafe()",0,0);
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
  soc_pb_ipq_traffic_class_map_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_IPQ_TR_CLS          tr_cls_ndx,
    SOC_SAND_OUT SOC_PETRA_IPQ_TR_CLS          *new_class
  )
{
  uint32
    entry_offset,
    res;
  SOC_PB_IRR_TRAFFIC_CLASS_MAPPING_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IPQ_TRAFFIC_CLASS_MAP_GET_UNSAFE);

  entry_offset = 0;

  res = soc_pb_irr_traffic_class_mapping_tbl_get_unsafe(
          unit,
          entry_offset,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  *new_class = tbl_data.traffic_class_mapping[tr_cls_ndx];

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_ipq_traffic_class_map_get_unsafe()",0,0);
}

/*********************************************************************
*     Sets the destination-id packets mapping to queue
*     information. Doesn't affect packets that arrive with
*     explicit queue-id in the header. Each destination ID is
*     mapped to a base_queue, when the packet is stored in
*     queue: base_queue + class
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_ipq_destination_id_packets_base_queue_id_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  dest_ndx,
    SOC_SAND_IN  uint8                 valid,
    SOC_SAND_IN  uint32                  base_queue
  )
{
  uint32
    res;
  SOC_PB_IRR_DESTINATION_TABLE_TBL_DATA
    irr_destination_table_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IPQ_DESTINATION_ID_PACKETS_BASE_QUEUE_ID_SET_UNSAFE);

  res = soc_pb_irr_destination_table_tbl_get_unsafe(
          unit,
          dest_ndx,
          &irr_destination_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  irr_destination_table_tbl_data.queue_valid = valid;
  irr_destination_table_tbl_data.queue_number = base_queue;
  irr_destination_table_tbl_data.tc_profile = SOC_PB_IPQ_TC_PROFILE_DFLT;

  res = soc_pb_irr_destination_table_tbl_set_unsafe(
          unit,
          dest_ndx,
          &irr_destination_table_tbl_data
      );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_ipq_destination_id_packets_base_queue_id_set_unsafe()",0,0);
}

/*********************************************************************
*     Sets the destination-id packets mapping to queue
*     information. Doesn't affect packets that arrive with
*     explicit queue-id in the header. Each destination ID is
*     mapped to a base_queue, when the packet is stored in
*     queue: base_queue + class
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_ipq_destination_id_packets_base_queue_id_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  dest_ndx,
    SOC_SAND_OUT uint8                 *valid,
    SOC_SAND_OUT uint32                  *base_queue
  )
{
  uint32
    res;
  SOC_PB_IRR_DESTINATION_TABLE_TBL_DATA
    irr_destination_table_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IPQ_DESTINATION_ID_PACKETS_BASE_QUEUE_ID_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(valid);
  SOC_SAND_CHECK_NULL_INPUT(base_queue);

  res = soc_pb_irr_destination_table_tbl_get_unsafe(
          unit,
          dest_ndx,
          &irr_destination_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  *valid = SOC_SAND_NUM2BOOL(irr_destination_table_tbl_data.queue_valid);
  *base_queue = irr_destination_table_tbl_data.queue_number;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_ipq_destination_id_packets_base_queue_id_get_unsafe()",0,0);
}

#include <soc/dpp/SAND/Utils/sand_footer.h>
