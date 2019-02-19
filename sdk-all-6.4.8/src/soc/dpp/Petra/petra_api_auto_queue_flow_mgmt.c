/* $Id: petra_api_auto_queue_flow_mgmt.c,v 1.7 Broadcom SDK $
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
* FILENAME:       DuneDriver/soc_petra/src/soc_petra_api_auto_queue_flow_mgmt.c
*
* MODULE PREFIX:  soc_petra_aqfm
*
* FILE DESCRIPTION:
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

#include <soc/dpp/Petra/petra_auto_queue_flow_mgmt.h>
#include <soc/dpp/Petra/petra_api_auto_queue_flow_mgmt.h>
#include <soc/dpp/Petra/petra_auto_queue_flow_mgmt_port.h>
#include <soc/dpp/Petra/petra_auto_queue_flow_mgmt_queue.h>
#include <soc/dpp/Petra/petra_auto_queue_flow_mgmt_aggregate.h>
#include <soc/dpp/Petra/petra_auto_queue_flow_mgmt_flow.h>
#include <soc/dpp/Petra/petra_scheduler_elements.h>
#include <soc/dpp/Petra/petra_sw_db.h>
#include <soc/dpp/Petra/petra_general.h>

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

uint8
  soc_petra_aqfm_module_is_initialized_get(
    SOC_SAND_IN int unit
  )
{
  return soc_petra_sw_db_auto_scheme_is_initialized(
           unit
         );
}

/*********************************************************************
*     This function saves the provided data in the SW database
*     for future use.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_aqfm_system_info_save(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_AQFM_SYSTEM_INFO   *p_info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_AQFM_SYSTEM_INFO_SAVE);

  SOC_SAND_CHECK_NULL_INPUT(p_info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_aqfm_system_info_save_unsafe(
          unit,
          p_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_aqfm_system_info_save()",0,0);
}

/*********************************************************************
*     Each structure in the auto management system has a
*     function soc_petra_aqfm_XXX_defaults(). This function will:
*     1. Clean the structure; 2. Set default values to
*     parameters that have logical common values. 3. Set
*     invalid values to parameters that have to be configured
*     by user. The function
*     soc_petra_aqfm_system_info_defaults_get might be called
*     before calling to soc_petra_api_auto_init(). The function
*     get the structure SOC_PETRA_AQFM_SYSTEM_INFO as parameter,
*     and fill it with some default values.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_aqfm_system_info_defaults_get(
    SOC_SAND_OUT SOC_PETRA_AQFM_SYSTEM_INFO   *p_info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_AQFM_SYSTEM_INFO_DEFAULTS_GET);

  SOC_SAND_CHECK_NULL_INPUT(p_info);

  p_info->max_nof_faps_in_system     = SOC_PETRA_AQFM_MAX_NOF_FAPS;
  p_info->max_nof_ports_per_fap      = SOC_PETRA_NOF_FAP_PORTS;

  p_info->nof_traffic_classes        = SOC_PETRA_NOF_TRAFFIC_CLASSES;
  p_info->nof_nif_ports              = 64;
  p_info->nof_cpu_ports              = SOC_PETRA_AQFM_MAX_NOF_CPU_PORTS;
  p_info->rcy_port_id                = SOC_PETRA_AQFM_RCY_PORT_ID_DEFAULT;

  p_info->agg_base_id.hr.min         = SOC_PETRA_HR_SE_ID_MIN + SOC_PETRA_NOF_FAP_PORTS;
  p_info->agg_base_id.hr.max         = SOC_PETRA_HR_SE_ID_MAX;
  p_info->agg_base_id.cl.min         = SOC_PETRA_CL_SE_ID_MIN;
  p_info->agg_base_id.cl.max         = SOC_PETRA_CL_SE_ID_MAX;
  p_info->agg_base_id.fq.min         = SOC_PETRA_FQ_SE_ID_MIN;
  p_info->agg_base_id.fq.max         = SOC_PETRA_FQ_SE_ID_MAX;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_aqfm_system_info_defaults_get()",0,0);
}

/*********************************************************************
*     The function should be called at the initialization
*     phase The function will set the SOC_PETRA_AQFM_PORT_INFO as
*     the information of all the ports, and will also open all
*     the ports in the system according to that.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_aqfm_all_ports_open(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_AQFM_PORT_INFO     *p_info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_AQFM_ALL_PORTS_OPEN);

  SOC_SAND_CHECK_NULL_INPUT(p_info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_aqfm_all_ports_open_unsafe(
          unit,
          p_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_aqfm_all_ports_open()",0,0);
}

/*********************************************************************
*     The function should be called at the initialization
*     phase The function will set the
*     SOC_PETRA_AQFM_AGG_INFO[class_i] as the information of all
*     the aggs of class_i, and will also open all the aggs in
*     the system according to that.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_aqfm_all_aggregates_open(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_AQFM_AGG_INFO      *p_info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_AQFM_ALL_AGGREGATES_OPEN);

  SOC_SAND_CHECK_NULL_INPUT(p_info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_aqfm_all_aggregates_open_unsafe(
          unit,
          p_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_aqfm_all_aggregates_open()",0,0);
}

/*********************************************************************
*     The function should be called at the initialization
*     phase The function will set the
*     SOC_PETRA_AQFM_QUEUE_INFO[class_i] as the information of
*     all the queues of class_i, and will also open all the
*     flows in the system according to that.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_aqfm_all_queues_open(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_AQFM_QUEUE_INFO    *p_info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_AQFM_ALL_QUEUES_OPEN);

  SOC_SAND_CHECK_NULL_INPUT(p_info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_aqfm_all_queues_open_unsafe(
          unit,
          p_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_aqfm_all_queues_open()",0,0);
}

/*********************************************************************
*     The function should be called at the initialization
*     phase The function will set the SOC_PETRA_AQFM_FLOW_INFO
*     [class_i] as the information of all the flows of
*     class_i, and will also open all the flows in the system
*     according to that
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_aqfm_all_flows_open(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_AQFM_FLOW_INFO     *p_info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_AQFM_ALL_FLOWS_OPEN);

  SOC_SAND_CHECK_NULL_INPUT(p_info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_aqfm_all_flows_open_unsafe(
          unit,
          p_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_aqfm_all_flows_open()",0,0);
}

uint32
  soc_petra_aqfm_port_open(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  uint32              port_id,
    SOC_SAND_IN  SOC_PETRA_AQFM_PORT_INFO  *port_info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_AQFM_PORT_OPEN);

  SOC_SAND_CHECK_NULL_INPUT(port_info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_aqfm_port_open_unsafe(
          unit,
          port_id,
          port_info,
          FALSE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_aqfm_port_open()",0,0);
}

uint32
  soc_petra_aqfm_port_update(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32             port_id,
    SOC_SAND_IN  SOC_PETRA_AQFM_PORT_INFO *p_port_info,
    SOC_SAND_OUT SOC_PETRA_AQFM_PORT_INFO *exact_port_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_AQFM_PORT_UPDATE);

  SOC_SAND_CHECK_NULL_INPUT(p_port_info);
  SOC_SAND_CHECK_NULL_INPUT(exact_port_info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_aqfm_port_update_unsafe(
          unit,
          port_id,
          p_port_info,
          exact_port_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_aqfm_port_update()",0,0);
}

uint32
  soc_petra_aqfm_port_close(
    SOC_SAND_IN  int            unit,
    SOC_SAND_IN  uint32            port_id
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_AQFM_PORT_CLOSE);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_aqfm_port_close_unsafe(
          unit,
          port_id
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_aqfm_port_close()",0,0);
}

uint32
  soc_petra_aqfm_agg_port_aggs_open(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32             port_id,
    SOC_SAND_IN  SOC_PETRA_AQFM_AGG_INFO*  agg_info,
    SOC_SAND_OUT SOC_PETRA_AQFM_AGG_INFO*  exact_aggs_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_AQFM_AGG_PORT_AGGS_OPEN);

  SOC_SAND_CHECK_NULL_INPUT(agg_info);
  SOC_SAND_CHECK_NULL_INPUT(exact_aggs_info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_aqfm_agg_port_aggs_open_unsafe(
          unit,
          port_id,
          agg_info,
          exact_aggs_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_aqfm_agg_port_aggs_open()",0,0);
}

uint32
  soc_petra_aqfm_agg_port_aggs_close(
    SOC_SAND_IN  int            unit,
    SOC_SAND_IN  uint32            port_id
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_AQFM_AGG_PORT_AGGS_CLOSE);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_aqfm_agg_port_aggs_close_unsafe(
          unit,
          port_id
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_aqfm_agg_port_aggs_close()",0,0);
}

uint32
  soc_petra_aqfm_agg_port_aggs_update(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  uint32                  port_id,
    SOC_SAND_IN  SOC_PETRA_AQFM_AGG_INFO       *aggs_info,
    SOC_SAND_OUT SOC_PETRA_AQFM_AGG_INFO       *exact_aggs_info,
    SOC_SAND_IN  SOC_PETRA_AQFM_FLOW_INFO      *flows_info,
    SOC_SAND_OUT SOC_PETRA_AQFM_FLOW_INFO      *exact_flows_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_AQFM_AGG_PORT_AGGS_UPDATE);

  SOC_SAND_CHECK_NULL_INPUT(aggs_info);
  SOC_SAND_CHECK_NULL_INPUT(exact_aggs_info);
  SOC_SAND_CHECK_NULL_INPUT(flows_info);
  SOC_SAND_CHECK_NULL_INPUT(exact_flows_info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_aqfm_agg_port_aggs_update_unsafe(
          unit,
          port_id,
          aggs_info,
          exact_aggs_info,
          flows_info,
          exact_flows_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_aqfm_agg_port_aggs_update()",0,0);
}

uint32
  soc_petra_aqfm_agg_port_single_aggs_update(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  uint32                  port_id,
    SOC_SAND_IN  uint32                  agg_level,
    SOC_SAND_IN  uint32                  agg_index,
    SOC_SAND_IN  SOC_PETRA_AQFM_AGG_ITEM_INFO  *aggs_info_item,
    SOC_SAND_OUT SOC_PETRA_AQFM_AGG_ITEM_INFO  *exact_aggs_info_item
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_AQFM_AGG_PORT_SINGLE_AGGS_UPDATE);

  SOC_SAND_CHECK_NULL_INPUT(aggs_info_item);
  SOC_SAND_CHECK_NULL_INPUT(exact_aggs_info_item);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_aqfm_agg_port_single_aggs_update_unsafe(
          unit,
          port_id,
          agg_level,
          agg_index,
          aggs_info_item,
          exact_aggs_info_item
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_aqfm_agg_port_single_aggs_update()",0,0);
}

uint32
  soc_petra_aqfm_queues_open(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  uint32              destination_fap_id,
    SOC_SAND_IN  uint32              destination_port_id,
    SOC_SAND_IN  SOC_PETRA_AQFM_QUEUE_INFO *auto_queue_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_AQFM_QUEUES_OPEN);

  SOC_SAND_CHECK_NULL_INPUT(auto_queue_info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_aqfm_queues_open_unsafe(
          unit,
          destination_fap_id,
          destination_port_id,
          auto_queue_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_aqfm_queues_open()",0,0);
}

uint32
  soc_petra_aqfm_queues_update(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  uint32              destination_fap_id,
    SOC_SAND_IN  uint32              destination_port_id,
    SOC_SAND_IN  SOC_PETRA_AQFM_QUEUE_INFO *queues_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_AQFM_QUEUES_UPDATE);

  SOC_SAND_CHECK_NULL_INPUT(queues_info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_aqfm_queues_update_unsafe(
          unit,
          destination_fap_id,
          destination_port_id,
          queues_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_aqfm_queues_update()",0,0);
}

uint32
  soc_petra_aqfm_queues_close(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  uint32              destination_fap_id,
    SOC_SAND_IN  uint32              destination_port_id
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_AQFM_QUEUES_CLOSE);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_aqfm_queues_close_unsafe(
          unit,
          destination_fap_id,
          destination_port_id
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_aqfm_queues_close()",0,0);
}

uint32
  soc_petra_aqfm_flow_open(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  uint32              port_id,
    SOC_SAND_IN  SOC_PETRA_AQFM_FLOW_INFO  *flow_info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_AQFM_FLOW_OPEN);

  SOC_SAND_CHECK_NULL_INPUT(flow_info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_aqfm_flow_open_unsafe(
          unit,
          port_id,
          flow_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_aqfm_flow_open()",0,0);
}

uint32
  soc_petra_aqfm_flow_update(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32                port_id,
    SOC_SAND_IN  SOC_PETRA_AQFM_FLOW_INFO    *flows_info,
    SOC_SAND_OUT SOC_PETRA_AQFM_FLOW_INFO    *exact_flows_info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_AQFM_FLOW_UPDATE);

  SOC_SAND_CHECK_NULL_INPUT(flows_info);
  SOC_SAND_CHECK_NULL_INPUT(exact_flows_info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_aqfm_flow_update_unsafe(
          unit,
          port_id,
          flows_info,
          exact_flows_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_aqfm_flow_update()",0,0);
}

uint32
  soc_petra_aqfm_flow_close(
    SOC_SAND_IN  int            unit,
    SOC_SAND_IN  uint32            port_id
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_AQFM_FLOW_CLOSE);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_aqfm_flow_close_unsafe(
          unit,
          port_id
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_aqfm_flow_close()",0,0);
}


void
  soc_petra_PETRA_AQFM_PORT_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_AQFM_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_AQFM_PORT_INFO));
  soc_petra_PETRA_OFP_RATE_INFO_clear(&(info->eg_port));
  info->port_info.hr_mode = SOC_PETRA_SCH_SE_HR_MODE_LAST;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_AQFM_AGG_ITEM_INFO_clear(
    int unit,
    SOC_SAND_OUT SOC_PETRA_AQFM_AGG_ITEM_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_AQFM_AGG_ITEM_INFO));
  soc_petra_PETRA_SCH_SE_INFO_clear(&(info->se));
  soc_petra_PETRA_SCH_FLOW_clear(unit, &(info->flow));
  info->valid = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  soc_petra_PETRA_AQFM_AGG_INFO_clear(
    int unit,
    SOC_SAND_OUT SOC_PETRA_AQFM_AGG_INFO *info
  )
{
  uint32
    ind_i,
    ind_j;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_AQFM_AGG_INFO));
  for (ind_i=0; ind_i<SOC_PETRA_AQFM_NOF_HIERARCY; ++ind_i)
  {
    for (ind_j=0; ind_j<SOC_PETRA_AQFM_NOF_INDEX; ++ind_j)
    {
      soc_petra_PETRA_AQFM_AGG_ITEM_INFO_clear(unit, &(info->item[ind_i][ind_j]));
    }
  }
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  soc_petra_PETRA_AQFM_FLOW_INFO_clear(
    int unit,
    SOC_SAND_OUT SOC_PETRA_AQFM_FLOW_INFO *info
  )
{
  uint32 ind;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_AQFM_FLOW_INFO));
  for (ind=0; ind<SOC_PETRA_NOF_TRAFFIC_CLASSES; ++ind)
  {
    soc_petra_PETRA_SCH_FLOW_clear(unit, &(info->flows[ind]));
  }
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  soc_petra_PETRA_AQFM_QUEUE_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_AQFM_QUEUE_INFO *info
  )
{
  uint32 ind;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_AQFM_QUEUE_INFO));
  for (ind=0; ind<SOC_PETRA_NOF_TRAFFIC_CLASSES; ++ind)
  {
    soc_petra_PETRA_ITM_QUEUE_INFO_clear(&info->queues[ind]);
  }
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_AQFM_SYSTEM_SE_RANGE_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_AQFM_SYSTEM_SE_RANGE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_AQFM_SYSTEM_SE_RANGE_INFO));
  info->min = 0;
  info->max = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_AQFM_AGG_BASE_IDS_clear(
    SOC_SAND_OUT SOC_PETRA_AQFM_AGG_BASE_IDS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_AQFM_AGG_BASE_IDS));
  soc_petra_PETRA_AQFM_SYSTEM_SE_RANGE_INFO_clear(&(info->hr));
  soc_petra_PETRA_AQFM_SYSTEM_SE_RANGE_INFO_clear(&(info->cl));
  soc_petra_PETRA_AQFM_SYSTEM_SE_RANGE_INFO_clear(&(info->fq));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_AQFM_SYSTEM_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_AQFM_SYSTEM_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_AQFM_SYSTEM_INFO));
  info->max_nof_faps_in_system = 0;
  info->max_nof_ports_per_fap = 0;
  info->nof_nif_ports = 0;
  info->nof_cpu_ports = 0;
  info->rcy_port_id = SOC_PETRA_FAP_PORT_ID_INVALID;
  info->nof_traffic_classes = 0;
  info->support_olp_port = FALSE;
  soc_petra_PETRA_AQFM_AGG_BASE_IDS_clear(&(info->agg_base_id));

  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PETRA_DEBUG_IS_LVL1


void
  soc_petra_PETRA_AQFM_PORT_INFO_print(
    SOC_SAND_IN SOC_PETRA_AQFM_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("Eg_port: ");
  soc_petra_PETRA_OFP_RATE_INFO_print(&(info->eg_port));
  soc_sand_os_printf(
    "Hr_mode %s \n\r",
    soc_petra_PETRA_SCH_SE_HR_MODE_to_string(info->port_info.hr_mode)
  );
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_AQFM_AGG_ITEM_INFO_print(
    SOC_SAND_IN SOC_PETRA_AQFM_AGG_ITEM_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("Schedule element: ");
  soc_petra_PETRA_SCH_SE_INFO_print(&(info->se));
  soc_sand_os_printf("Flow: ");
  soc_petra_PETRA_SCH_FLOW_print(&(info->flow), TRUE);
  soc_sand_os_printf("Valid: %d\n\r",info->valid);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_AQFM_AGG_INFO_print(
    SOC_SAND_IN SOC_PETRA_AQFM_AGG_INFO *info
  )
{
  uint32
    ind_i,
    ind_j;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind_i=0; ind_i<SOC_PETRA_AQFM_NOF_HIERARCY; ++ind_i)
  {
    for (ind_j=0; ind_j<SOC_PETRA_AQFM_NOF_INDEX; ++ind_j)
    {
      soc_petra_PETRA_AQFM_AGG_ITEM_INFO_print(&(info->item[ind_i][ind_j]));
    }
  }

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_AQFM_FLOW_INFO_print(
    SOC_SAND_IN SOC_PETRA_AQFM_FLOW_INFO *info
  )
{
  uint32 ind=0;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind=0; ind<SOC_PETRA_NOF_TRAFFIC_CLASSES; ++ind)
  {
    soc_sand_os_printf("Flows[%u]: ",ind);
    soc_petra_PETRA_SCH_FLOW_print(&(info->flows[ind]), TRUE);
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_AQFM_QUEUE_INFO_print(
    SOC_SAND_IN SOC_PETRA_AQFM_QUEUE_INFO *info
  )
{
  uint32 ind=0;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind=0; ind<SOC_PETRA_NOF_TRAFFIC_CLASSES; ++ind)
  {
    soc_petra_PETRA_ITM_QUEUE_INFO_print(&info->queues[ind]);
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_AQFM_SYSTEM_SE_RANGE_INFO_print(
    SOC_SAND_IN SOC_PETRA_AQFM_SYSTEM_SE_RANGE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("    Min: %d\n\r",info->min);
  soc_sand_os_printf("    Max: %d\n\r",info->max);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_AQFM_AGG_BASE_IDS_print(
    SOC_SAND_IN SOC_PETRA_AQFM_AGG_BASE_IDS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("  HR\n\r");
  soc_petra_PETRA_AQFM_SYSTEM_SE_RANGE_INFO_print(&(info->hr));
  soc_sand_os_printf("  CL\n\r");
  soc_petra_PETRA_AQFM_SYSTEM_SE_RANGE_INFO_print(&(info->cl));
  soc_sand_os_printf("  FQ\n\r");
  soc_petra_PETRA_AQFM_SYSTEM_SE_RANGE_INFO_print(&(info->fq));

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_AQFM_SYSTEM_INFO_print(
    SOC_SAND_IN SOC_PETRA_AQFM_SYSTEM_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("Max_nof_faps_in_system: %u[Faps]\n\r",info->max_nof_faps_in_system);
  soc_sand_os_printf("Max_nof_ports_per_fap: %u[Ports]\n\r",info->max_nof_ports_per_fap);
  soc_sand_os_printf("Nof_cpu_ports: %u[CPU ports]\n\r",info->nof_cpu_ports);
  soc_sand_os_printf("Nof_nif_ports: %u[NIF ports]\n\r",info->nof_nif_ports);
  soc_sand_os_printf("Support_olp_port: %u\n\r",info->support_olp_port);
  soc_sand_os_printf("Rcy_port_id: %u\n\r",info->rcy_port_id);
  soc_sand_os_printf("Nof_traffic_classes: %u[Traffic classes]\n\r",info->nof_traffic_classes);
  soc_petra_PETRA_AQFM_AGG_BASE_IDS_print(&(info->agg_base_id));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif /* SOC_PETRA_DEBUG_IS_LVL1 */

#include <soc/dpp/SAND/Utils/sand_footer.h>

