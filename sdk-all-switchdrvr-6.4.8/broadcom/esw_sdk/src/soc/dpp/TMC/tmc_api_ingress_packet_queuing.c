/* $Id: tmc_api_ingress_packet_queuing.c,v 1.7 Broadcom SDK $
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
* FILENAME:       DuneDriver/tmc/src/soc_tmcapi_ingress_packet_queuing.c
*
* MODULE PREFIX:  soc_tmcipq
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


#include <shared/bsl.h>

#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/TMC/tmc_api_general.h>
#include <soc/dpp/TMC/tmc_api_ingress_packet_queuing.h>

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

void
  SOC_TMC_IPQ_EXPLICIT_MAPPING_MODE_INFO_clear(
    SOC_SAND_OUT SOC_TMC_IPQ_EXPLICIT_MAPPING_MODE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_IPQ_EXPLICIT_MAPPING_MODE_INFO));
  info->base_queue_id = 0;
  /*
   *  Invalid configuration - the user must change it before setting
   */
  info->queue_id_add_not_decrement = FALSE;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_IPQ_BASEQ_MAP_INFO_clear(
    SOC_SAND_OUT SOC_TMC_IPQ_BASEQ_MAP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_IPQ_BASEQ_MAP_INFO));
  info->valid = 0;
  info->base_queue = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_IPQ_QUARTET_MAP_INFO_clear(
    SOC_SAND_OUT SOC_TMC_IPQ_QUARTET_MAP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_IPQ_QUARTET_MAP_INFO));
  info->flow_quartet_index = SOC_TMC_IPQ_INVALID_FLOW_QUARTET;
  info->is_composite = 0;
  info->system_physical_port = SOC_TMC_MAX_SYSTEM_PHYSICAL_PORT_ID;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_TMC_DEBUG_IS_LVL1

const char*
  SOC_TMC_IPQ_TR_CLS_RNG_to_string(
    SOC_SAND_IN SOC_TMC_IPQ_TR_CLS_RNG enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_IPQ_TR_CLS_MIN:
    str = "TR_CLS_MIN";
  break;

  case SOC_TMC_IPQ_TR_CLS_MAX:
    str = "TR_CLS_MAX";
  break;

  default:
    str = " Unknown";
  }
  return str;
}

void
  SOC_TMC_IPQ_EXPLICIT_MAPPING_MODE_INFO_print(
    SOC_SAND_IN SOC_TMC_IPQ_EXPLICIT_MAPPING_MODE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "Base_queue_id: %u\n\r"),info->base_queue_id));
  LOG_CLI((BSL_META_U(unit,
                      "Queue_id_add_not_decrement: %d\n\r"),info->queue_id_add_not_decrement));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_IPQ_BASEQ_MAP_INFO_print(
    SOC_SAND_IN  SOC_TMC_IPQ_BASEQ_MAP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "valid: %u\n\r"),info->valid));
  LOG_CLI((BSL_META_U(unit,
                      "base_queue: %u\n\r"),info->base_queue));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_IPQ_QUARTET_MAP_INFO_print(
    SOC_SAND_IN SOC_TMC_IPQ_QUARTET_MAP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  if (
      (info->flow_quartet_index == SOC_TMC_IPQ_INVALID_FLOW_QUARTET) &&
      (info->is_composite == 0) &&
      (info->system_physical_port == SOC_TMC_MAX_SYSTEM_PHYSICAL_PORT_ID)
     )
  {
    LOG_CLI((BSL_META_U(unit,
                        "The queue quartet is unmapped.\n\r")));
  }
  else
  {
    LOG_CLI((BSL_META_U(unit,
                        "Flow_quartet_index: %u\n\r"),info->flow_quartet_index));
    LOG_CLI((BSL_META_U(unit,
                        "Is_composite: %d\n\r"),info->is_composite));
    LOG_CLI((BSL_META_U(unit,
                        "System_physical_port: %u\n\r"),info->system_physical_port));
    LOG_CLI((BSL_META_U(unit,
                        "Fap_id: %u\n\r"),((unsigned)info->fap_id)));
    LOG_CLI((BSL_META_U(unit,
                        "Fap_port_id: %u\n\r"),((unsigned)info->fap_port_id)));
  }

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif /* SOC_TMC_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

