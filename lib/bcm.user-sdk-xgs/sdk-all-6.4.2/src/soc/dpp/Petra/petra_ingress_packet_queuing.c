/* $Id: petra_ingress_packet_queuing.c,v 1.14 Broadcom SDK $
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

#include <soc/dpp/Petra/petra_ingress_packet_queuing.h>
#include <soc/dpp/Petra/petra_api_nif.h>
#include <soc/dpp/Petra/petra_nif.h>
#include <soc/dpp/Petra/petra_scheduler_end2end.h>

#include <soc/dpp/Petra/petra_ports.h>

#include <soc/dpp/Petra/petra_reg_access.h>
#include <soc/dpp/Petra/petra_tbl_access.h>
#include <soc/dpp/Petra/petra_general.h>
#include <soc/dpp/Petra/petra_mgmt.h>
#include <soc/dpp/Petra/petra_sw_db.h>
#include <soc/dpp/Petra/petra_chip_tbls.h>
#include <soc/dpp/Petra/petra_debug.h>

#include <soc/dpp/cosq.h>



#include <soc/dpp/SAND/Utils/sand_bitstream.h>
#include <soc/dpp/SAND/Utils/sand_u64.h>

#ifdef LINK_PB_LIBRARIES
#include <soc/dpp/Petra/PB_TM/pb_ingress_packet_queuing.h>
#endif


/* } */

/*************
 * DEFINES   *
 *************/
/* { */

/* $Id: petra_ingress_packet_queuing.c,v 1.14 Broadcom SDK $
 * Port to interface mapping register value indicating
 * unmapped interface
 */

/* Max & min values for enum SOC_PETRA_IPQ_TR_CLS_RNG:      */
#define SOC_PETRA_IPQ_TR_CLS_RNG_MIN 0
#define SOC_PETRA_IPQ_TR_CLS_RNG_MAX (SOC_PETRA_IPQ_TR_CLS_RNG_LAST-1)

/* Max & min values for struct SOC_PETRA_IPQ_EXPLICIT_MAPPING_MODE_INFO:      */
#define SOC_PETRA_IPQ_EXPLICIT_MAPPING_MODE_INFO_BASE_QUEUE_ID_MIN      0
#define SOC_PETRA_IPQ_EXPLICIT_MAPPING_MODE_INFO_BASE_QUEUE_ID_MAX      SOC_PETRA_NOF_QUEUES-1

/* Max & min values for dest_ndx and base_queue:      */
#define SOC_PETRA_IPQ_DESTINATION_ID_PACKETS_BASE_DEST_NDX_MIN     0
#define SOC_PETRA_IPQ_DESTINATION_ID_PACKETS_BASE_DEST_NDX_MAX     4095

#define SOC_PETRA_IPQ_DESTINATION_ID_PACKETS_BASE_BASE_QUEUE_MIN     0
#define SOC_PETRA_IPQ_DESTINATION_ID_PACKETS_BASE_BASE_QUEUE_MAX     SOC_PETRA_NOF_QUEUES-1


/* Max & min values for sys_physical_port_ndx:      */

/* Max & min values for dest_fap_id:      */

/* Max & min values for dest_fap_id:      */


/* Max & min values for Mapping of Queues to Flows:      */
#define SOC_PETRA_IPQ_MIN_INTERDIGIT_FLOW_QUARTET   6144

#define SOC_PETRA_IPQ_QUARTET_MAP_INFO_SYSTEM_PHYSICAL_PORT_MIN    0
#define SOC_PETRA_IPQ_QUARTET_MAP_INFO_SYSTEM_PHYSICAL_PORT_MAX    SOC_PETRA_MAX_SYSTEM_PHYSICAL_PORT_ID

#define  SOC_PETRA_INGR_QUEUE_TABLE_BYTE_RESOL 16

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
*     Configures Base-Q configuration to an invalid value.
*     This configuration is changed by
*     soc_petra_ipq_explicit_mapping_mode_info_set.
*     API-s that are dependent on a valid Base-Q configuration
*     may use this value to verify Base-Q is already set.
*********************************************************************/
STATIC uint32
  soc_petra_ipq_base_q_dflt_invalid_set_unsafe(
    SOC_SAND_IN  int  unit
  )
{
  uint32
    res;
  SOC_PETRA_IPQ_EXPLICIT_MAPPING_MODE_INFO
    base_q_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IPQ_BASE_Q_DFLT_INVALID_SET_UNSAFE);

  soc_petra_PETRA_IPQ_EXPLICIT_MAPPING_MODE_INFO_clear(&base_q_info);

  base_q_info.base_queue_id = 0;
  base_q_info.queue_id_add_not_decrement = FALSE;

  res = soc_petra_ipq_explicit_mapping_mode_info_set_unsafe(
          unit,
          &base_q_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ipq_base_q_dflt_invalid_set_unsafe()",0,0);
}

/*********************************************************************
* NAME:
*     soc_petra_ipq_init
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
  soc_petra_ipq_init(
    SOC_SAND_IN  int                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IPQ_INIT);

  res = soc_petra_ipq_base_q_dflt_invalid_set_unsafe(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

#ifdef LINK_PB_LIBRARIES
  if (SOC_PETRA_IS_DEV_PETRA_B_OR_ABOVE)
  {
    res = soc_pb_ipq_init(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  }
#endif

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ipq_init()",0,0);
}

/*********************************************************************
*     Sets the Explicit Flow Unicast packets mapping to queue.
*     Doesn't affect packets that arrive with destination_id
*     in the header.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ipq_explicit_mapping_mode_info_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_IPQ_EXPLICIT_MAPPING_MODE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IPQ_EXPLICIT_MAPPING_MODE_INFO_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

  /*
   * SOC_PETRA_IPQ_EXPLICIT_MAPPING_MODE_INFO_BASE_QUEUE_ID_MIN may be changed and be grater then zero.
   */
  /* coverity[unsigned_compare] */
  if ((info->base_queue_id < SOC_PETRA_IPQ_EXPLICIT_MAPPING_MODE_INFO_BASE_QUEUE_ID_MIN) ||
    (info->base_queue_id > SOC_PETRA_IPQ_EXPLICIT_MAPPING_MODE_INFO_BASE_QUEUE_ID_MAX))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_IPQ_EXPLICIT_MAPPING_MODE_BASE_QUEUE_ID_OUT_OF_RANGE_ERR, 10, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ipq_explicit_mapping_mode_info_verify()",0,0);
}

/*********************************************************************
*     Sets the Explicit Flow Unicast packets mapping to queue.
*     Doesn't affect packets that arrive with destination_id
*     in the header.
*     Details: in the H file. (search for prototype)
*********************************************************************/
STATIC
  uint32
    soc_pa_ipq_explicit_mapping_mode_info_set_unsafe(
      SOC_SAND_IN  int                 unit,
      SOC_SAND_IN  SOC_PETRA_IPQ_EXPLICIT_MAPPING_MODE_INFO *info
    )
{
  uint32
    res;
  SOC_PETRA_REGS
    *regs;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PA_IPQ_EXPLICIT_MAPPING_MODE_INFO_SET_UNSAFE);

  regs = soc_petra_regs();

  SOC_SAND_CHECK_NULL_INPUT(info);

  /* set base_queue_id, increment_indicator { */
  SOC_PA_FLD_SET(regs->ihp.general_configuration_reg.uc_flow_base_qnum, info->base_queue_id, 20, exit);

  SOC_PA_FLD_SET(regs->ihp.general_configuration_reg.uc_flow_base_qnum_add, info->queue_id_add_not_decrement, 30, exit);
  /* set base_queue_id, increment_indicator } */

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pa_ipq_explicit_mapping_mode_info_set_unsafe()",0,0);
}

uint32
  soc_petra_ipq_explicit_mapping_mode_info_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_IPQ_EXPLICIT_MAPPING_MODE_INFO *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IPQ_EXPLICIT_MAPPING_MODE_INFO_SET_UNSAFE);

  SOC_PETRA_DIFF_DEVICE_CALL(ipq_explicit_mapping_mode_info_set_unsafe,(unit, info));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ipq_explicit_mapping_mode_info_set_unsafe()",0,0);
}


/*********************************************************************
*     Sets the Explicit Flow Unicast packets mapping to queue.
*     Doesn't affect packets that arrive with destination_id
*     in the header.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pa_ipq_explicit_mapping_mode_info_get_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_OUT SOC_PETRA_IPQ_EXPLICIT_MAPPING_MODE_INFO *info
  )
{
  uint32
    res,
    queue_id_add_not_decrement;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PA_IPQ_EXPLICIT_MAPPING_MODE_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  regs = soc_petra_regs();


  /* get base_queue_id, increment_indicator { */
  SOC_PA_FLD_GET(regs->ihp.general_configuration_reg.uc_flow_base_qnum, info->base_queue_id, 10, exit);

  SOC_PA_FLD_GET(regs->ihp.general_configuration_reg.uc_flow_base_qnum_add, queue_id_add_not_decrement, 20, exit);

  info->queue_id_add_not_decrement =
    ((queue_id_add_not_decrement > 0) ? TRUE : FALSE);

  /* get base_queue_id, increment_indicator } */

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pa_ipq_explicit_mapping_mode_info_get_unsafe()",0,0);
}

uint32
  soc_petra_ipq_explicit_mapping_mode_info_get_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_OUT SOC_PETRA_IPQ_EXPLICIT_MAPPING_MODE_INFO *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IPQ_EXPLICIT_MAPPING_MODE_INFO_GET_UNSAFE);

  SOC_PETRA_DIFF_DEVICE_CALL(ipq_explicit_mapping_mode_info_get_unsafe,(unit, info));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ipq_explicit_mapping_mode_info_get_unsafe()",0,0);
}

/*********************************************************************
*     Configures Base-Q configuration to an invalid value.
*     This configuration is changed by
*     soc_petra_ipq_explicit_mapping_mode_info_set.
*     API-s that are dependent on a valid Base-Q configuration
*     may use this value to verify Base-Q is already set.
*********************************************************************/
uint32
  soc_petra_ipq_base_q_is_valid_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_OUT uint8  *is_valid
  )
{
  uint32
    res;
  SOC_PETRA_IPQ_EXPLICIT_MAPPING_MODE_INFO
    base_q_info;
  uint8
    is_invalid;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IPQ_BASE_Q_IS_VALID_GET_UNSAFE);

  soc_petra_PETRA_IPQ_EXPLICIT_MAPPING_MODE_INFO_clear(&base_q_info);

  res = soc_petra_ipq_explicit_mapping_mode_info_get_unsafe(
          unit,
          &base_q_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  is_invalid = SOC_SAND_NUM2BOOL(
            (base_q_info.base_queue_id == 0) &&
            (base_q_info.queue_id_add_not_decrement == FALSE)
          );

  *is_valid = !(is_invalid);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ipq_base_q_is_valid_get_unsafe()",0,0);
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
  soc_petra_ipq_traffic_class_map_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_IPQ_TR_CLS          tr_cls_ndx,
    SOC_SAND_IN  SOC_PETRA_IPQ_TR_CLS          new_class
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IPQ_TRAFFIC_CLASS_MAP_VERIFY);

  /* 
   * COVERITY
   * In case SOC_PETRA_IPQ_TR_CLS_RNG_MIN will be bigger than 0 
   */
/* coverity[unsigned_compare] */
  if ((tr_cls_ndx < SOC_PETRA_IPQ_TR_CLS_RNG_MIN)||
    (tr_cls_ndx > SOC_PETRA_IPQ_TR_CLS_RNG_MAX))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_IPQ_TRAFFIC_CLASS_MAP_TR_CLS_OUT_OF_RANGE_ERR, 10, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ipq_traffic_class_map_verify()",0,0);
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
  soc_pa_ipq_traffic_class_map_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_IPQ_TR_CLS          tr_cls_ndx,
    SOC_SAND_IN  SOC_PETRA_IPQ_TR_CLS          new_class
  )
{
  uint32
    res;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PA_IPQ_TRAFFIC_CLASS_MAP_SET_UNSAFE);

  regs = soc_petra_regs();

  SOC_PA_FLD_SET(regs->irr.unicast_traffic_class_mapping_reg.unicast_traffic_class_mapping_register[tr_cls_ndx], new_class, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pa_ipq_traffic_class_map_set_unsafe()",0,0);
}
uint32
  soc_petra_ipq_traffic_class_map_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_IPQ_TR_CLS          tr_cls_ndx,
    SOC_SAND_IN  SOC_PETRA_IPQ_TR_CLS          new_class
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IPQ_TRAFFIC_CLASS_MAP_SET_UNSAFE);

  SOC_PETRA_DIFF_DEVICE_CALL(ipq_traffic_class_map_set_unsafe,(unit, tr_cls_ndx, new_class));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ipq_traffic_class_map_set_unsafe()",0,0);
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
  soc_pa_ipq_traffic_class_map_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_IPQ_TR_CLS          tr_cls_ndx,
    SOC_SAND_OUT SOC_PETRA_IPQ_TR_CLS          *new_class
  )
{
  uint32
    res;
  SOC_PETRA_REGS
    *regs;
  uint32
    new_class_value;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PA_IPQ_TRAFFIC_CLASS_MAP_GET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(new_class);

  regs = soc_petra_regs();

  SOC_PA_FLD_GET(regs->irr.unicast_traffic_class_mapping_reg.unicast_traffic_class_mapping_register[tr_cls_ndx], new_class_value, 10, exit);
  *new_class = (SOC_PETRA_IPQ_TR_CLS)new_class_value;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pa_ipq_traffic_class_map_get_unsafe()",0,0);
}

uint32
  soc_petra_ipq_traffic_class_map_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_IPQ_TR_CLS          tr_cls_ndx,
    SOC_SAND_OUT SOC_PETRA_IPQ_TR_CLS          *new_class
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IPQ_TRAFFIC_CLASS_MAP_GET_UNSAFE);

  SOC_PETRA_DIFF_DEVICE_CALL(ipq_traffic_class_map_get_unsafe,(unit, tr_cls_ndx, new_class));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ipq_traffic_class_map_get_unsafe()",0,0);
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
  soc_petra_ipq_destination_id_packets_base_queue_id_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  dest_ndx,
    SOC_SAND_IN  uint8                 valid,
    SOC_SAND_IN  uint32                  base_queue
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IPQ_DESTINATION_ID_PACKETS_BASE_QUEUE_ID_VERIFY);

  /* 
   * COVERITY
   * In case SOC_PETRA_IPQ_DESTINATION_ID_PACKETS_BASE_DEST_NDX_MIN will be bigger than 0 
   */
/* coverity[unsigned_compare] */
  if ((dest_ndx < SOC_PETRA_IPQ_DESTINATION_ID_PACKETS_BASE_DEST_NDX_MIN)||
    (dest_ndx > SOC_PETRA_IPQ_DESTINATION_ID_PACKETS_BASE_DEST_NDX_MAX))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_IPQ_DESTINATION_ID_PACKETS_BASE_DEST_NDX_OUT_OF_RANGE_ERR, 10, exit);
  }
  /* 
   * COVERITY
   * In case SOC_PETRA_IPQ_DESTINATION_ID_PACKETS_BASE_BASE_QUEUE_MIN will be bigger than 0 
   */
/* coverity[unsigned_compare] */
  if ((base_queue < SOC_PETRA_IPQ_DESTINATION_ID_PACKETS_BASE_BASE_QUEUE_MIN)||
    (base_queue > SOC_PETRA_IPQ_DESTINATION_ID_PACKETS_BASE_BASE_QUEUE_MAX))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_IPQ_DESTINATION_ID_PACKETS_BASE_BASE_QUEUE_OUT_OF_RANGE_ERR, 20, exit);
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ipq_destination_id_packets_base_queue_id_verify()",0,0);
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
  soc_pa_ipq_destination_id_packets_base_queue_id_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  dest_ndx,
    SOC_SAND_IN  uint8                 valid,
    SOC_SAND_IN  uint32                  base_queue
  )
{
  uint32
    res;
  SOC_PETRA_IRR_DESTINATION_TABLE_TBL_DATA
    irr_destination_table_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PA_IPQ_DESTINATION_ID_PACKETS_BASE_QUEUE_ID_SET_UNSAFE);

  res = soc_petra_irr_destination_table_tbl_get_unsafe(
          unit,
          dest_ndx,
          &irr_destination_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  irr_destination_table_tbl_data.queue_valid =
    valid;
  irr_destination_table_tbl_data.queue_number =
    base_queue;

  res = soc_petra_irr_destination_table_tbl_set_unsafe(
          unit,
          dest_ndx,
          &irr_destination_table_tbl_data
      );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pa_ipq_destination_id_packets_base_queue_id_set_unsafe()",0,0);
}

uint32
  soc_petra_ipq_destination_id_packets_base_queue_id_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  dest_ndx,
    SOC_SAND_IN  uint8                 valid,
    SOC_SAND_IN  uint32                  base_queue
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IPQ_DESTINATION_ID_PACKETS_BASE_QUEUE_ID_SET_UNSAFE);

  SOC_PETRA_DIFF_DEVICE_CALL(ipq_destination_id_packets_base_queue_id_set_unsafe,(unit, dest_ndx, valid, base_queue));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ipq_destination_id_packets_base_queue_id_set_unsafe()",0,0);
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
  soc_pa_ipq_destination_id_packets_base_queue_id_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  dest_ndx,
    SOC_SAND_OUT uint8                 *valid,
    SOC_SAND_OUT uint32                  *base_queue
  )
{
  uint32
    res;
  SOC_PETRA_IRR_DESTINATION_TABLE_TBL_DATA
    irr_destination_table_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PA_IPQ_DESTINATION_ID_PACKETS_BASE_QUEUE_ID_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(valid);
  SOC_SAND_CHECK_NULL_INPUT(base_queue);

  res = SOC_SAND_OK; sal_memset(
          &irr_destination_table_tbl_data,
          0,
          sizeof(SOC_PETRA_IRR_DESTINATION_TABLE_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = soc_petra_irr_destination_table_tbl_get_unsafe(
          unit,
          dest_ndx,
          &irr_destination_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  *valid = SOC_SAND_NUM2BOOL(irr_destination_table_tbl_data.queue_valid);
  *base_queue = irr_destination_table_tbl_data.queue_number;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pa_ipq_destination_id_packets_base_queue_id_get_unsafe()",0,0);
}

uint32
  soc_petra_ipq_destination_id_packets_base_queue_id_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  dest_ndx,
    SOC_SAND_OUT uint8                 *valid,
    SOC_SAND_OUT uint32                  *base_queue
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IPQ_DESTINATION_ID_PACKETS_BASE_QUEUE_ID_GET_UNSAFE);

  SOC_PETRA_DIFF_DEVICE_CALL(ipq_destination_id_packets_base_queue_id_get_unsafe,(unit, dest_ndx, valid, base_queue));


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ipq_destination_id_packets_base_queue_id_get_unsafe()",0,0);
}

/*********************************************************************
*     Map 1K of queues (256 quartets) to be in interdigitated
*     mode or not. Queue Quartets that configured to be in
*     interdigitated mode should only be configured with
*     interdigitated flow quartets, and the other-way around.
*     When interdigitated mode is set, all queue quartets
*     range are reset using soc_petra_ipq_quartet_reset(), in
*     order to prevent illegal interdigitated/composite state.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
soc_petra_ipq_queue_interdigitated_mode_verify(
  SOC_SAND_IN  int                 unit,
  SOC_SAND_IN  uint32                  k_queue_ndx,
    SOC_SAND_IN  uint8                 is_interdigitated
  )
{
  uint32
    queue_index;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IPQ_QUEUE_INTERDIGITATED_MODE_VERIFY);

  queue_index = k_queue_ndx << 10;
  if (queue_index > (SOC_PETRA_NOF_QUEUES-1))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_IPQ_K_QUEUE_INDEX_OUT_OF_RANGE_ERR, 10, exit);
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ipq_queue_interdigitated_mode_verify()",0,0);
}

/*********************************************************************
*     Map 1K of queues (256 quartets) to be in interdigitated
*     mode or not. Queue Quartets that configured to be in
*     interdigitated mode should only be configured with
*     interdigitated flow quartets, and the other-way around.
*     When interdigitated mode is set, all queue quartets
*     range are reset using soc_petra_ipq_quartet_reset(), in
*     order to prevent illegal interdigitated/composite state.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ipq_queue_interdigitated_mode_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  k_queue_ndx,
    SOC_SAND_IN  uint8                 is_interdigitated
  )
{
  uint32
    res,
    buffer;
  uint32
    queue_quartet_i;
  SOC_PETRA_REGS
    *regs = soc_petra_regs();
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IPQ_QUEUE_INTERDIGITATED_MODE_SET_UNSAFE);

  SOC_PETRA_REG_GET(regs->ips.interdigitated_mode_reg, buffer, 10, exit);

  if (is_interdigitated)
  {
    /* turn bit on */
    buffer |= SOC_SAND_BIT(k_queue_ndx);
  }
  else
  {
    /* turn bit off */
    buffer &= SOC_SAND_RBIT(k_queue_ndx);
  }

  SOC_PETRA_REG_SET(regs->ips.interdigitated_mode_reg, buffer, 20, exit);

  /* reseting all queues in the k_queues_index */
  for (queue_quartet_i = (k_queue_ndx << 8);
       queue_quartet_i < ((k_queue_ndx+1) << 8);
       queue_quartet_i++)
  {
    soc_petra_ipq_quartet_reset_unsafe(
      unit,
      queue_quartet_i
    );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ipq_queue_interdigitated_mode_set_unsafe()",0,0);
}

/*********************************************************************
*     Map 1K of queues (256 quartets) to be in interdigitated
*     mode or not. Queue Quartets that configured to be in
*     interdigitated mode should only be configured with
*     interdigitated flow quartets, and the other-way around.
*     When interdigitated mode is set, all queue quartets
*     range are reset using soc_petra_ipq_quartet_reset(), in
*     order to prevent illegal interdigitated/composite state.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ipq_queue_interdigitated_mode_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  k_queue_ndx,
    SOC_SAND_OUT uint8                 *is_interdigitated
  )
{
  uint32
    res;
  uint32
    buffer;
  SOC_PETRA_REGS
    *regs;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IPQ_QUEUE_INTERDIGITATED_MODE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(is_interdigitated);

  regs = soc_petra_regs();


  SOC_PETRA_REG_GET(regs->ips.interdigitated_mode_reg, buffer, 10, exit);

  /* true only if the the k_queue_index is on */
  *is_interdigitated = ((buffer & SOC_SAND_BIT(k_queue_ndx)) != 0 ? TRUE : FALSE);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ipq_queue_interdigitated_mode_get_unsafe()",0,0);
}

/*********************************************************************
*     Map a queues-quartet to system port and flow-quartet(s).
*     This function should only be called after calling the
*     soc_petra_ipq_queue_interdigitated_mode_set()
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ipq_queue_to_flow_mapping_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  queue_quartet_ndx,
    SOC_SAND_IN  SOC_PETRA_IPQ_QUARTET_MAP_INFO *info
  )
{
  uint32
    res,
    queue_index,
    flow_index;
  uint8
    is_interdigitated;
  SOC_PETRA_IPQ_QUARTET_MAP_INFO
    prev_flow_quartet_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IPQ_QUEUE_TO_FLOW_MAPPING_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

  queue_index = queue_quartet_ndx *4;
  if (queue_index > (SOC_PETRA_NOF_QUEUES-1))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_QUEUE_ID_OUT_OF_RANGE_ERR, 10, exit);
  }

  flow_index = (info->flow_quartet_index) *4;
  if (flow_index > (SOC_PETRA_NOF_FLOWS-1))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_FLOW_ID_OUT_OF_RANGE_ERR, 20, exit);
  }

  /* 
   * COVERITY
   * In case SOC_PETRA_IPQ_QUARTET_MAP_INFO_SYSTEM_PHYSICAL_PORT_MIN will be bigger than 0 
   */
/* coverity[unsigned_compare] */
  if ((info->system_physical_port < SOC_PETRA_IPQ_QUARTET_MAP_INFO_SYSTEM_PHYSICAL_PORT_MIN)||
    (info->system_physical_port > SOC_PETRA_IPQ_QUARTET_MAP_INFO_SYSTEM_PHYSICAL_PORT_MAX))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_SYS_PHYSICAL_PORT_NDX_OUT_OF_RANGE_ERR, 30, exit);
  }

  /*
   * Verify that interdigitated/composite/base_queue does not conflict
   */

  res = soc_petra_ipq_queue_interdigitated_mode_get_unsafe(
          unit,
          SOC_SAND_DIV_ROUND_DOWN(queue_quartet_ndx * 4 , 1024) /* k_queue_index */,
          &is_interdigitated
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  if (queue_quartet_ndx > 0)
  {
    res = soc_petra_ipq_queue_to_flow_mapping_get_unsafe(
            unit,
            queue_quartet_ndx-1,
            &prev_flow_quartet_info
            );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);


    if ((is_interdigitated) &
        (info->flow_quartet_index < SOC_PETRA_IPQ_MIN_INTERDIGIT_FLOW_QUARTET))
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_IPQ_BASE_FLOW_FOR_INTERDIGIT_QUEUE_QUARTET_TOO_LOW_ERR, 60, exit);
    }

    if (((is_interdigitated && !info->is_composite) ||
         (!is_interdigitated && info->is_composite)) &&
         (!soc_sand_is_even(info->flow_quartet_index)))
    {
      /* if interdigitated or composite (but not both), base flow quartet must
         be even */
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_IPQ_BASE_FLOW_QUARTET_NOT_EVEN_ERR, 80, exit);
    }

    if (is_interdigitated && !info->is_composite &&
        ((info->flow_quartet_index % 4) != 0))
    {
      /* if interdigitated but not composite, and not a multiply of 4,
         previous base flow quartet must not be composite as well */
      if (prev_flow_quartet_info.is_composite)
      {
        SOC_SAND_SET_ERROR_CODE(SOC_PETRA_IPQ_BASE_FLOW_ALREADY_MAPPED_BY_PREVIOUS_QUEUE_QUARTET_ERR, 90, exit);
      }
    }

    if ((is_interdigitated && info->is_composite) &&
        ((info->flow_quartet_index % 4) != 0))
    {
      /* if interdigitated and composite, base flow quartet must be a multiply
         of 4 */
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_IPQ_BASE_FLOW_QUARTET_NOT_MULTIPLY_OF_FOUR_ERR, 100, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ipq_queue_to_flow_mapping_verify()",0,0);
}

/*********************************************************************
*     Map a queues-quartet to system port and flow-quartet(s).
*     This function should only be called after calling the
*     soc_petra_ipq_queue_interdigitated_mode_set()
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ipq_queue_to_flow_mapping_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  queue_quartet_ndx,
    SOC_SAND_IN  SOC_PETRA_IPQ_QUARTET_MAP_INFO *info
  )
{
  uint32
    res;
  SOC_PETRA_IPS_FLOW_ID_LOOKUP_TABLE_TBL_DATA
    ips_flow_id_lookup_table_tbl_data;
  SOC_PETRA_IPS_SYSTEM_PHYSICAL_PORT_LOOKUP_TABLE_TBL_DATA
    ips_system_physical_port_lookup_table_tbl_data;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IPQ_QUEUE_TO_FLOW_MAPPING_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  /*
   * Set base flow {
   */
  res = soc_petra_ips_flow_id_lookup_table_tbl_get_unsafe(
          unit,
          queue_quartet_ndx,
          &ips_flow_id_lookup_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_petra_ips_system_physical_port_lookup_table_tbl_get_unsafe(
          unit,
          queue_quartet_ndx,
          &ips_system_physical_port_lookup_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  if (!(
        (ips_flow_id_lookup_table_tbl_data.base_flow == SOC_PETRA_IPQ_INVALID_FLOW_QUARTET) &&
        (ips_flow_id_lookup_table_tbl_data.sub_flow_mode == 0x0) &&
        (ips_system_physical_port_lookup_table_tbl_data.sys_phy_port == SOC_PETRA_MAX_SYSTEM_PHYSICAL_PORT_ID)
        )
     )
  {
    /*
     *	This is a re-map. First unmap and flush all queues
     *  for a clean transition
     */
    res = soc_petra_ipq_queue_qrtt_unmap_unsafe(
            unit,
            queue_quartet_ndx
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }

  /* multiply by 4 from quartet to base flow */
  ips_flow_id_lookup_table_tbl_data.base_flow =
    info->flow_quartet_index;

  ips_flow_id_lookup_table_tbl_data.sub_flow_mode =
    info->is_composite;

  res = soc_petra_ips_flow_id_lookup_table_tbl_set_unsafe(
          unit,
          queue_quartet_ndx,
          &ips_flow_id_lookup_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  /*
   * Set base flow }
   */

  ips_system_physical_port_lookup_table_tbl_data.sys_phy_port =
    info->system_physical_port;

  res = soc_petra_ips_system_physical_port_lookup_table_tbl_set_unsafe(
          unit,
          queue_quartet_ndx,
          &ips_system_physical_port_lookup_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ipq_queue_to_flow_mapping_set_unsafe()",0,0);
}

/*********************************************************************
*     Unmap a queues-quartet, by mapping it to invalid
*     destination. Also, flush all the queues in the quartet.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ipq_queue_qrtt_unmap_unsafe(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  uint32  queue_quartet_ndx
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    q_id,
    baseq_id;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IPQ_QUEUE_QRTT_UNMAP_UNSAFE);

  baseq_id = SOC_PETRA_IPQ_QRTT_TO_Q_ID(queue_quartet_ndx);
  SOC_SAND_ERR_IF_ABOVE_MAX(baseq_id, SOC_PETRA_NOF_QUEUES-1, SOC_PETRA_QUEUE_ID_OUT_OF_RANGE_ERR, 10, exit);

  res = soc_petra_ipq_quartet_reset_unsafe(
          unit,
          queue_quartet_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  if (!SOC_PETRA_IS_DEV_PETRA_A)
  {
    /*
     *  For Soc_petra-B,
     *	Flush all (4) queues in the quartet
     */
    for (q_id = baseq_id; q_id <= baseq_id + 3; q_id++)
    {
      res = soc_petra_dbg_queue_flush_unsafe(
              unit,
              q_id,
              SOC_PETRA_DBG_FLUSH_MODE_DELETE
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    }
  }
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ipq_queue_qrtt_unmap_unsafe()",queue_quartet_ndx,0);
}


/*********************************************************************
*     Map a queues-quartet to system port and flow-quartet(s).
*     This function should only be called after calling the
*     soc_petra_ipq_queue_interdigitated_mode_set()
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ipq_queue_to_flow_mapping_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  queue_quartet_ndx,
    SOC_SAND_OUT SOC_PETRA_IPQ_QUARTET_MAP_INFO *info
  )
{
  uint32
    res;
  SOC_PETRA_IPS_FLOW_ID_LOOKUP_TABLE_TBL_DATA
    ips_flow_id_lookup_table_tbl_data;
  SOC_PETRA_IPS_SYSTEM_PHYSICAL_PORT_LOOKUP_TABLE_TBL_DATA
    ips_system_physical_port_lookup_table_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IPQ_QUEUE_TO_FLOW_MAPPING_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_petra_ips_flow_id_lookup_table_tbl_get_unsafe(
    unit,
    queue_quartet_ndx,
    &ips_flow_id_lookup_table_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  /* divide by 4 from quartet to base flow */
  info->flow_quartet_index =
    ips_flow_id_lookup_table_tbl_data.base_flow;

  info->is_composite =
    (uint8)ips_flow_id_lookup_table_tbl_data.sub_flow_mode;

  res = soc_petra_ips_system_physical_port_lookup_table_tbl_get_unsafe(
          unit,
          queue_quartet_ndx,
          &ips_system_physical_port_lookup_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  info->system_physical_port =
    ips_system_physical_port_lookup_table_tbl_data.sys_phy_port;


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ipq_queue_to_flow_mapping_get_unsafe()",0,0);
}

/*********************************************************************
*     Resets a quartet to default values.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ipq_quartet_reset_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  queue_quartet_ndx
  )
{
  uint32
    queue_index;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IPQ_QUARTET_RESET_VERIFY);

  queue_index = queue_quartet_ndx *4;
  if (queue_index > (SOC_PETRA_NOF_QUEUES-1))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_QUEUE_ID_OUT_OF_RANGE_ERR, 10, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ipq_quartet_reset_verify()",0,0);
}


/*********************************************************************
*     Resets a quartet to default values.
*********************************************************************/
uint32
  soc_petra_ipq_quartet_reset_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  queue_quartet_ndx
  )
{
  uint32
    res;
  uint32
    baseq_id;
  SOC_PETRA_IPS_FLOW_ID_LOOKUP_TABLE_TBL_DATA
    ips_flow_id_lookup_table_tbl_data;
  SOC_PETRA_IPS_SYSTEM_PHYSICAL_PORT_LOOKUP_TABLE_TBL_DATA
    ips_system_physical_port_lookup_table_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IPQ_QUARTET_RESET_UNSAFE);

  baseq_id = SOC_PETRA_IPQ_QRTT_TO_Q_ID(queue_quartet_ndx);
  SOC_SAND_ERR_IF_ABOVE_MAX(baseq_id, SOC_PETRA_NOF_QUEUES-1, SOC_PETRA_QUEUE_ID_OUT_OF_RANGE_ERR, 10, exit);

  ips_system_physical_port_lookup_table_tbl_data.sys_phy_port = SOC_PETRA_MAX_SYSTEM_PHYSICAL_PORT_ID;

  res = soc_petra_ips_system_physical_port_lookup_table_tbl_set_unsafe(
          unit,
          queue_quartet_ndx,
          &ips_system_physical_port_lookup_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

  /*
   * Set base flow {
   */
  /* multiply by 4 from quartet to base flow */
  ips_flow_id_lookup_table_tbl_data.base_flow = SOC_PETRA_IPQ_INVALID_FLOW_QUARTET;
  ips_flow_id_lookup_table_tbl_data.sub_flow_mode = 0x0;

  res = soc_petra_ips_flow_id_lookup_table_tbl_set_unsafe(
          unit,
          queue_quartet_ndx,
          &ips_flow_id_lookup_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  /*
   * Set base flow }
   */

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ipq_quartet_reset_unsafe()",0,0);
}


uint32
  soc_petra_ipq_attached_flow_port_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32  queue_ndx,
    SOC_SAND_OUT uint32  *flow_id,
    SOC_SAND_OUT uint32  *sys_port
  )
{
  uint32
    res = SOC_SAND_OK;
  uint8
    is_interdigitated;
  SOC_PETRA_IPQ_QUARTET_MAP_INFO
    flow_quartet_info;
  uint32
    flow_offset_in_quartet,
    quartet_offset,
    base_flow;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IPQ_ATTACHED_FLOW_PORT_GET_UNSAFE);

  res = soc_petra_ipq_queue_interdigitated_mode_get_unsafe(
          unit,
          queue_ndx / 1024,
          &is_interdigitated
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 2, exit);

  res = soc_petra_ipq_queue_to_flow_mapping_get_unsafe(
          unit,
          queue_ndx / 4,
          &flow_quartet_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 2, exit);
  *sys_port = flow_quartet_info.system_physical_port;

  base_flow = (flow_quartet_info.flow_quartet_index * 4);

  if (flow_quartet_info.is_composite)
  {
    if (is_interdigitated)
    {
      quartet_offset = (queue_ndx % 4);
      flow_offset_in_quartet = 2;
    }
    else
    {
      quartet_offset = (queue_ndx % 4) / 2;
      flow_offset_in_quartet = (queue_ndx % 2) * 2;
    }
  }
  else
  {
    if (is_interdigitated)
    {
      quartet_offset = (queue_ndx % 4) / 2;
      flow_offset_in_quartet = 2 + (queue_ndx % 2);
    }
    else
    {
      quartet_offset = 0;
      flow_offset_in_quartet = (queue_ndx % 4);
    }
  }

  *flow_id = ((base_flow) + (quartet_offset * 4) + (flow_offset_in_quartet));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ipq_attached_flow_port_get()",0,0);
}

uint32
  soc_petra_ipq_queue_id_verify(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  uint32    queue_id
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IPQ_QUEUE_ID_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(queue_id, SOC_PETRA_NOF_QUEUES-1, SOC_PETRA_IPQ_INVALID_QUEUE_ID_ERR,10,exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ipq_queue_id_verify()",0,0);
}

#if SOC_PETRA_DEBUG
uint32
  soc_petra_ips_non_empty_queues_info_get_unsafe(
   SOC_SAND_IN  int                unit,
   SOC_SAND_IN  uint32                first_queue,
   SOC_SAND_IN  uint32                max_array_size,
   SOC_SAND_OUT soc_ips_queue_info_t* queues,
   SOC_SAND_OUT uint32*               nof_queues_filled,
   SOC_SAND_OUT uint32*               next_queue,
   SOC_SAND_OUT uint32*               reached_end
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PETRA_IQM_DYNAMIC_TBL_DATA
    iqm_dynamic_tbl;
  uint32
    nof_queues,
    queue_byte_size;
  uint32
    queue_id;
  uint8
    got_flow_info = FALSE;
  uint32
    local_fap,
    target_fap_id,
    target_data_port_id,
    system_physical_port,
    target_flow_id = 0,
    k=0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  (*nof_queues_filled)=0;
  (*reached_end)=1;

  res = soc_petra_mgmt_system_fap_id_get_unsafe(
          unit,
          &local_fap
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 2, exit);

  nof_queues = SOC_PETRA_NOF_QUEUES;

  /*iterate over queues*/
  for(queue_id = first_queue; queue_id < nof_queues; ++queue_id)
  {
    res = soc_petra_iqm_dynamic_tbl_get_unsafe(
            unit,
            queue_id,
            &iqm_dynamic_tbl
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 6, exit);

   /*
    * Calc Queue size
    */
    if (iqm_dynamic_tbl.que_not_empty)
    {
      queue_byte_size = iqm_dynamic_tbl.pq_inst_que_size * SOC_PETRA_INGR_QUEUE_TABLE_BYTE_RESOL;
    }
    else
    {
      queue_byte_size = 0;
    }

    /*if queue isn't empty*/
    if(queue_byte_size)
    {
      res = soc_petra_ipq_attached_flow_port_get_unsafe(
              unit,
              queue_id,
              &target_flow_id,
              &system_physical_port
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 8, exit);

      res = soc_petra_sys_phys_to_local_port_map_get(
              unit,
              system_physical_port,
              &target_fap_id,
              &target_data_port_id
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

      if(target_fap_id == local_fap)
      {
        got_flow_info = TRUE;
      }

      /* filling the queue array */
      if ((*nof_queues_filled)<max_array_size)
      {
          queues[k].queue_id=queue_id;
          queues[k].queue_byte_size=queue_byte_size;
          queues[k].target_flow_id=target_flow_id;
          queues[k].got_flow_info=got_flow_info;
          queues[k].target_flow_id=target_flow_id;
          queues[k].target_data_port_id=target_data_port_id;
          k++;
          (*nof_queues_filled)++;
      } else {
          (*next_queue)=queue_id; /* reached maximum size for array,  save the next queue's id and break*/
          (*reached_end)=0;
          break;
      }

    }
  }
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ips_non_empty_queues_print_unsafe()",0,0);
}
#endif

#include <soc/dpp/SAND/Utils/sand_footer.h>
