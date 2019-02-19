/* $Id: pb_api_ports.c,v 1.9 Broadcom SDK $
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
* FILENAME:       DuneDriver/soc_pb_tm/src/soc_pb_api_ports.c
*
* MODULE PREFIX:  pb
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

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/Petra/PB_TM/pb_api_framework.h>
#include <soc/dpp/Petra/PB_TM/pb_api_ports.h>
#include <soc/dpp/Petra/PB_TM/pb_ports.h>
#include <soc/dpp/Petra/PB_TM/pb_api_general.h>

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
*     Configure the Port profile for ports of type TM and Raw.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_port_pp_port_set(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                 pp_port_ndx,
    SOC_SAND_IN  SOC_PB_PORT_PP_PORT_INFO         *info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE         *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PORT_PP_PORT_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = soc_pb_port_pp_port_set_verify(
          unit,
          pp_port_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_port_pp_port_set_unsafe(
          unit,
          pp_port_ndx,
          info,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_port_pp_port_set()", pp_port_ndx, 0);
}

uint32
  soc_pb_port_pp_port_set_dispatch(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  int                    core,
    SOC_SAND_IN  uint32                 pp_port_ndx,
    SOC_SAND_IN  SOC_PB_PORT_PP_PORT_INFO         *info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE         *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PORT_PP_PORT_SET);

  res = soc_pb_port_pp_port_set(
          unit,
          pp_port_ndx,
          info,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_port_pp_port_set_dispatch()", pp_port_ndx, 0);
}

/*********************************************************************
*     Get the Port profile settings.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_port_pp_port_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  int 					core_id,
    SOC_SAND_IN  uint32                    pp_port_ndx,
    SOC_SAND_OUT SOC_PB_PORT_PP_PORT_INFO     *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PORT_PP_PORT_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_pb_port_pp_port_get_verify(
          unit,
          pp_port_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_port_pp_port_get_unsafe(
          unit,
          pp_port_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_port_pp_port_get()", pp_port_ndx, 0);
}

/*********************************************************************
*     Map the Port to its Port profile for ports of type TM
 *     and Raw.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_port_to_pp_port_map_set(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    port_ndx,
    SOC_SAND_IN  SOC_PETRA_PORT_DIRECTION           direction_ndx,
    SOC_SAND_IN  uint32                    pp_port
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PORT_TO_PP_PORT_MAP_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_port_to_pp_port_map_set_verify(
          unit,
          port_ndx,
          direction_ndx,
          pp_port
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

  res = soc_pb_port_to_pp_port_map_set_unsafe(
          unit,
          port_ndx,
          direction_ndx,
          pp_port
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_port_to_pp_port_map_set()", port_ndx, 0);
}

uint32
  soc_pb_port_to_pp_port_map_set_dispatch(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  soc_port_t                 port_ndx,
    SOC_SAND_IN  SOC_PETRA_PORT_DIRECTION   direction_ndx
  )
{
    int res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PORT_TO_PP_PORT_MAP_SET);

    res = soc_pb_port_to_pp_port_map_set(unit, (uint32)port_ndx, direction_ndx, (uint32)port_ndx);

    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_port_to_pp_port_map_set_dispatch()", port_ndx, 0);
}
/*********************************************************************
*     Map the Port to its Port profile for ports of type TM
 *     and Raw.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_port_to_pp_port_map_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    port_ndx,
    SOC_SAND_OUT uint32                    *pp_port_in,
    SOC_SAND_OUT uint32                    *pp_port_out
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PORT_TO_PP_PORT_MAP_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(pp_port_in);
  SOC_SAND_CHECK_NULL_INPUT(pp_port_out);

  res = soc_pb_port_to_pp_port_map_get_verify(
          unit,
          port_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_port_to_pp_port_map_get_unsafe(
          unit,
          port_ndx,
          pp_port_in,
          pp_port_out
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_port_to_pp_port_map_get()", port_ndx, 0);
}

/*********************************************************************
*      Define the Forwarding header parameters for Raw ports.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_port_forwarding_header_set(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                       pp_port_ndx,
    SOC_SAND_IN  SOC_PB_PORTS_FORWARDING_HEADER_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PORT_FORWARDING_HEADER_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_port_forwarding_header_set_verify(
          unit,
          pp_port_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

  res = soc_pb_port_forwarding_header_set_unsafe(
          unit,
          pp_port_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_port_forwarding_header_set()", pp_port_ndx, 0);
}

/*********************************************************************
*    Define the Forwarding header parameters for Raw ports.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_port_forwarding_header_get(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                       pp_port_ndx,
    SOC_SAND_OUT SOC_PB_PORTS_FORWARDING_HEADER_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PORT_FORWARDING_HEADER_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_port_forwarding_header_get_verify(
          unit,
          pp_port_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

  res = soc_pb_port_forwarding_header_get_unsafe(
          unit,
          pp_port_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_port_forwarding_header_get()", pp_port_ndx, 0);
}

void
  SOC_PB_PORT_PP_PORT_INFO_clear(
    SOC_SAND_OUT SOC_PB_PORT_PP_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PORT_PP_PORT_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PORT_COUNTER_INFO_clear(
    SOC_SAND_OUT SOC_PB_PORT_COUNTER_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PORT_COUNTER_INFO_clear(info);

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PORTS_FORWARDING_HEADER_INFO_clear(
    SOC_SAND_OUT SOC_PB_PORTS_FORWARDING_HEADER_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PORTS_FORWARDING_HEADER_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PB_DEBUG_IS_LVL1

const char*
  SOC_PB_PORTS_FC_TYPE_to_string(
    SOC_SAND_IN  SOC_PB_PORTS_FC_TYPE enum_val
  )
{
  return SOC_TMC_PORTS_FC_TYPE_to_string(enum_val);
}

void
  SOC_PB_PORT_PP_PORT_INFO_print(
    SOC_SAND_IN  SOC_PB_PORT_PP_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PORT_PP_PORT_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PORT_COUNTER_INFO_print(
    SOC_SAND_IN  SOC_PB_PORT_COUNTER_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PORT_COUNTER_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PORTS_FORWARDING_HEADER_INFO_print(
    SOC_SAND_IN  SOC_PB_PORTS_FORWARDING_HEADER_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PORTS_FORWARDING_HEADER_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif /* SOC_PB_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

