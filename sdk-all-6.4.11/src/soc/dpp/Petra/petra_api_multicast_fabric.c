/* $Id: petra_api_multicast_fabric.c,v 1.7 Broadcom SDK $
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
* FILENAME:       DuneDriver/soc_petra/src/soc_petra_api_multicast_fabric.c
*
* MODULE PREFIX:  soc_petra_mult_fabric
*
* FILE DESCRIPTION: refer to H file
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

#include <soc/dpp/Petra/petra_multicast_fabric.h>
#include <soc/dpp/Petra/petra_api_general.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
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
*     a multicast class (0..3). This multicast class will be
*     further used for egress/fabric replication.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_mult_fabric_traffic_class_to_multicast_cls_map_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_TR_CLS         tr_cls_ndx,
    SOC_SAND_IN  SOC_PETRA_MULT_FABRIC_CLS     new_mult_cls
  )
{
  uint32  res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_FABRIC_TRAFFIC_CLASS_TO_MULTICAST_CLS_MAP_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;


  res = soc_petra_mult_fabric_traffic_class_to_multicast_cls_map_verify(
    unit,
    tr_cls_ndx,
    new_mult_cls
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_mult_fabric_traffic_class_to_multicast_cls_map_set_unsafe(
    unit,
    tr_cls_ndx,
    new_mult_cls
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_fabric_traffic_class_to_multicast_cls_map_set()",0,0);
}

/*********************************************************************
*     Maps the embedded traffic class in the packet header to
*     a multicast class (0..3). This multicast class will be
*     further used for egress/fabric replication.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_mult_fabric_traffic_class_to_multicast_cls_map_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_TR_CLS         tr_cls_ndx,
    SOC_SAND_OUT SOC_PETRA_MULT_FABRIC_CLS     *new_mult_cls
  )
{
  uint32  res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_FABRIC_TRAFFIC_CLASS_TO_MULTICAST_CLS_MAP_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(new_mult_cls);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_mult_fabric_traffic_class_to_multicast_cls_map_get_unsafe(
    unit,
    tr_cls_ndx,
    new_mult_cls
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_fabric_traffic_class_to_multicast_cls_map_get()",0,0);
}

/*********************************************************************
*     This procedure configures the base queue of the
*     multicast egress/fabric.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_mult_fabric_base_queue_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  queue_id
  )
{
  uint32  res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_FABRIC_BASE_QUEUE_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;


  res = soc_petra_mult_fabric_base_queue_verify(
    unit,
    queue_id
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_mult_fabric_base_queue_set_unsafe(
    unit,
    queue_id
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_fabric_base_queue_set()",0,0);
}

/*********************************************************************
*     This procedure configures the base queue of the
*     multicast egress/fabric.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_mult_fabric_base_queue_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT uint32                  *queue_id
  )
{
  uint32  res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_FABRIC_BASE_QUEUE_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(queue_id);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_mult_fabric_base_queue_get_unsafe(
    unit,
    queue_id
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_fabric_base_queue_get()",0,0);
}

/*********************************************************************
*     Set the Fabric Multicast credit generator configuration
*     for the Default Fabric Multicast Queue configuration.
*     The fabric multicast queues are 0 - 3, and the credits
*     comes either directly to these queues or according to a
*     scheduler scheme.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_mult_fabric_credit_source_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_FABRIC_INFO    *info,
    SOC_SAND_OUT SOC_PETRA_MULT_FABRIC_INFO    *exact_info
  )
{
  uint32  res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_FABRIC_CREDIT_SOURCE_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(exact_info);

  res = soc_petra_mult_fabric_credit_source_verify(
          unit,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_mult_fabric_credit_source_set_unsafe(
    unit,
    info,
    exact_info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_fabric_credit_source_set()",0,0);
}

/*********************************************************************
*     Set the Fabric Multicast credit generator configuration
*     for the Default Fabric Multicast Queue configuration.
*     The fabric multicast queues are 0 - 3, and the credits
*     comes either directly to these queues or according to a
*     scheduler scheme.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_mult_fabric_credit_source_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_MULT_FABRIC_INFO    *info
  )
{
  uint32  res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_FABRIC_CREDIT_SOURCE_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_mult_fabric_credit_source_get_unsafe(
    unit,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_fabric_credit_source_get()",0,0);
}

/*********************************************************************
*     Configure the Enhanced Fabric Multicast Queue
*     configuration: the fabric multicast queues are defined
*     in a configured range, and the credits are coming to
*     these queues according to a scheduler scheme.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_mult_fabric_enhanced_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  SOC_SAND_U32_RANGE                            *queue_range
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_FABRIC_ENHANCED_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(queue_range);

  res = soc_petra_mult_fabric_enhanced_set_verify(
          unit,
          queue_range
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_mult_fabric_enhanced_set_unsafe(
          unit,
          queue_range
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_fabric_enhanced_set()",0,0);
}

/*********************************************************************
*     Configure the Enhanced Fabric Multicast Queue
*     configuration: the fabric multicast queues are defined
*     in a configured range, and the credits are coming to
*     these queues according to a scheduler scheme.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_mult_fabric_enhanced_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_OUT SOC_SAND_U32_RANGE                            *queue_range
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_FABRIC_ENHANCED_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(queue_range);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_mult_fabric_enhanced_get_unsafe(
          unit,
          queue_range
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_fabric_enhanced_get()",0,0);
}

/*********************************************************************
*     This procedure sets the FAP links that are eligible for
*     multicast fabric traffic.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_mult_fabric_active_links_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_FABRIC_ACTIVE_LINKS *links,
    SOC_SAND_IN  uint8                 tbl_refresh_enable
  )
{
  uint32  res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_FABRIC_ACTIVE_LINKS_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(links);

  res = soc_petra_mult_fabric_active_links_verify(
    unit,
    links,
    tbl_refresh_enable
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_mult_fabric_active_links_set_unsafe(
    unit,
    links,
    tbl_refresh_enable
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_fabric_active_links_set()",0,0);
}

/*********************************************************************
*     This procedure sets the FAP links that are eligible for
*     multicast fabric traffic.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_mult_fabric_active_links_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_MULT_FABRIC_ACTIVE_LINKS *links,
    SOC_SAND_OUT uint8                 *tbl_refresh_enable
  )
{
  uint32  res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_FABRIC_ACTIVE_LINKS_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(links);
  SOC_SAND_CHECK_NULL_INPUT(tbl_refresh_enable);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_mult_fabric_active_links_get_unsafe(
    unit,
    links,
    tbl_refresh_enable
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_fabric_active_links_get()",0,0);
}

void
  soc_petra_PETRA_MULT_FABRIC_PORT_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_MULT_FABRIC_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_MULT_FABRIC_PORT_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_MULT_FABRIC_SHAPER_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_MULT_FABRIC_SHAPER_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_MULT_FABRIC_SHAPER_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_MULT_FABRIC_BE_CLASS_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_MULT_FABRIC_BE_CLASS_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_MULT_FABRIC_BE_CLASS_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_MULT_FABRIC_BE_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_MULT_FABRIC_BE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_MULT_FABRIC_BE_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_MULT_FABRIC_GR_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_MULT_FABRIC_GR_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_MULT_FABRIC_GR_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_MULT_FABRIC_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_MULT_FABRIC_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_MULT_FABRIC_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_MULT_FABRIC_ACTIVE_LINKS_clear(
    SOC_SAND_IN uint32 unit,
    SOC_SAND_OUT SOC_PETRA_MULT_FABRIC_ACTIVE_LINKS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_MULT_FABRIC_ACTIVE_LINKS_clear(unit, info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PETRA_DEBUG_IS_LVL1

const char*
  soc_petra_PETRA_MULT_FABRIC_CLS_RNG_to_string(
    SOC_SAND_IN  SOC_PETRA_MULT_FABRIC_CLS_RNG enum_val
  )
{
  return SOC_TMC_MULT_FABRIC_CLS_RNG_to_string(enum_val);
}

void
  soc_petra_PETRA_MULT_FABRIC_PORT_INFO_print(
    SOC_SAND_IN  SOC_PETRA_MULT_FABRIC_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_MULT_FABRIC_PORT_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_MULT_FABRIC_SHAPER_INFO_print(
    SOC_SAND_IN  SOC_PETRA_MULT_FABRIC_SHAPER_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_MULT_FABRIC_SHAPER_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_MULT_FABRIC_BE_CLASS_INFO_print(
    SOC_SAND_IN  SOC_PETRA_MULT_FABRIC_BE_CLASS_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_MULT_FABRIC_BE_CLASS_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_MULT_FABRIC_BE_INFO_print(
    SOC_SAND_IN  SOC_PETRA_MULT_FABRIC_BE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_MULT_FABRIC_BE_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_MULT_FABRIC_GR_INFO_print(
    SOC_SAND_IN  SOC_PETRA_MULT_FABRIC_GR_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_MULT_FABRIC_GR_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_MULT_FABRIC_INFO_print(
    SOC_SAND_IN  SOC_PETRA_MULT_FABRIC_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_MULT_FABRIC_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_MULT_FABRIC_ACTIVE_LINKS_print(
    SOC_SAND_IN uint32 unit,
    SOC_SAND_IN  SOC_PETRA_MULT_FABRIC_ACTIVE_LINKS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_MULT_FABRIC_ACTIVE_LINKS_print(unit, info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif /* SOC_PETRA_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

