/* $Id: pb_api_egr_queuing.c,v 1.9 Broadcom SDK $
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
* FILENAME:       DuneDriver/soc_pb_tm/src/soc_pb_api_egr_queuing.c
*
* MODULE PREFIX:  soc_pb_egr
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
#include <soc/dpp/Petra/PB_TM/pb_api_egr_queuing.h>
#include <soc/dpp/Petra/PB_TM/pb_egr_queuing.h>
#include <soc/dpp/Petra/PB_TM/pb_api_general.h>
#include <soc/dpp/Petra/PB_TM/pb_tbl_access.h>

#include <soc/dpp/error.h>

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
*     Sets egress queue calendar which control the service
 *     times of the nif ports.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_egr_q_nif_cal_set(
    SOC_SAND_IN  int                     unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_EGR_Q_NIF_CAL_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = soc_pb_egr_q_nif_cal_set_verify(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_egr_q_nif_cal_set_unsafe(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_egr_q_nif_cal_set()", 0, 0);
}

/*********************************************************************
*     Sets egress queue calendar which control the service
 *     times of the nif ports.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_egr_q_nif_cal_get(
    SOC_SAND_IN  int                     unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_EGR_Q_NIF_CAL_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = soc_pb_egr_q_nif_cal_get_verify(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_egr_q_nif_cal_get_unsafe(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_egr_q_nif_cal_get()", 0, 0);
}



/*********************************************************************
*     Sets egress queue priority per traffic class and drop
 *     precedence.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_egr_q_prio_set(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  SOC_PETRA_EGR_Q_PRIO_MAPPING_TYPE map_type_ndx,
    SOC_SAND_IN  uint32                     tc_ndx,
    SOC_SAND_IN  uint32                     dp_ndx,
    SOC_SAND_IN  uint32                     map_profile_ndx,
    SOC_SAND_IN  SOC_PB_EGR_Q_PRIORITY             *priority
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_EGR_Q_PRIO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(priority);

  res = soc_pb_egr_q_prio_set_verify(
          unit,
          map_type_ndx,
          tc_ndx,
          dp_ndx,
          map_profile_ndx,
          priority
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_egr_q_prio_set_unsafe(
          unit,
          map_type_ndx,
          tc_ndx,
          dp_ndx,
          map_profile_ndx,
          priority
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_egr_q_prio_set()", 0, tc_ndx);
}

int
  soc_pb_egr_q_prio_set_dispatch(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  int                                core,
    SOC_SAND_IN  SOC_PETRA_EGR_Q_PRIO_MAPPING_TYPE  map_type_ndx,
    SOC_SAND_IN  uint32                             tc_ndx,
    SOC_SAND_IN  uint32                             dp_ndx,
    SOC_SAND_IN  uint32                             map_profile_ndx,
    SOC_SAND_IN  SOC_PB_EGR_Q_PRIORITY              *priority
  )
{
    int rv, res;

    res = soc_pb_egr_q_prio_set(unit, map_type_ndx, tc_ndx, dp_ndx, map_profile_ndx, priority);
    rv = handle_sand_result(res);
    SOC_IF_ERROR_RETURN(rv);

    return SOC_E_NONE;
}

/*********************************************************************
*     Sets egress queue priority per traffic class and drop
 *     precedence.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_egr_q_prio_get(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  SOC_PETRA_EGR_Q_PRIO_MAPPING_TYPE map_type_ndx,
    SOC_SAND_IN  uint32                     tc_ndx,
    SOC_SAND_IN  uint32                     dp_ndx,
    SOC_SAND_IN  uint32                     map_profile_ndx,
    SOC_SAND_OUT SOC_PB_EGR_Q_PRIORITY             *priority
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_EGR_Q_PRIO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(priority);

  res = soc_pb_egr_q_prio_get_verify(
          unit,
          map_type_ndx,
          tc_ndx,
          dp_ndx,
          map_profile_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_egr_q_prio_get_unsafe(
          unit,
          map_type_ndx,
          tc_ndx,
          dp_ndx,
          map_profile_ndx,
          priority
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_egr_q_prio_get()", 0, tc_ndx);
}

int
  soc_pb_egr_q_prio_get_dispatch(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  int                                core,
    SOC_SAND_IN  SOC_PETRA_EGR_Q_PRIO_MAPPING_TYPE  map_type_ndx,
    SOC_SAND_IN  uint32                             tc_ndx,
    SOC_SAND_IN  uint32                             dp_ndx,
    SOC_SAND_IN  uint32                             map_profile_ndx,
    SOC_SAND_OUT SOC_PB_EGR_Q_PRIORITY              *priority
  )
{
    int rv, res;

    res = soc_pb_egr_q_prio_get(unit, map_type_ndx, tc_ndx, dp_ndx, map_profile_ndx, priority);
    rv = handle_sand_result(res);
    SOC_IF_ERROR_RETURN(rv);

    return SOC_E_NONE;
}

/*********************************************************************
*     Function description
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_egr_q_profile_map_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID              ofp_ndx,
    SOC_SAND_IN  uint32                      map_profile_id
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_EGR_Q_PROFILE_MAP_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_egr_q_profile_map_set_verify(
          unit,
          ofp_ndx,
          map_profile_id
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

  res = soc_pb_egr_q_profile_map_set_unsafe(
          unit,
          ofp_ndx,
          map_profile_id
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_egr_q_profile_map_set()", ofp_ndx, map_profile_id);
}

int
  soc_pb_egr_q_profile_map_set_dispatch(
    SOC_SAND_IN     int                   unit,
    SOC_SAND_IN     int                   core_id,
    SOC_SAND_IN     uint32                tm_port,
    SOC_SAND_IN     uint32                map_profile_id
  )
{
    int rv, res;

    res = soc_pb_egr_q_profile_map_set(unit, tm_port, map_profile_id);
    rv = handle_sand_result(res);
    SOC_IF_ERROR_RETURN(rv);

    return SOC_E_NONE;
}

/*********************************************************************
*     Function description
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_egr_q_profile_map_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID              ofp_ndx,
    SOC_SAND_OUT uint32                      *map_profile_id
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_EGR_Q_PROFILE_MAP_GET);
  SOC_SAND_CHECK_NULL_INPUT(map_profile_id);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_egr_q_profile_map_get_verify(
          unit,
          ofp_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

  res = soc_pb_egr_q_profile_map_get_unsafe(
          unit,
          ofp_ndx,
          map_profile_id
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_egr_q_profile_map_get()", ofp_ndx, 0);
}

int
  soc_pb_egr_q_profile_map_get_dispatch(
    SOC_SAND_IN     int                   unit,
    SOC_SAND_IN     int                   core,
    SOC_SAND_IN     uint32                tm_port,
    SOC_SAND_OUT    uint32                *map_profile_id
  )
{
    int rv, res;

    res = soc_pb_egr_q_profile_map_get(unit, tm_port, map_profile_id);
    rv = handle_sand_result(res);
    SOC_IF_ERROR_RETURN(rv);

    return SOC_E_NONE;
}

void
  SOC_PB_EGR_Q_PRIORITY_clear(
    SOC_SAND_OUT SOC_PB_EGR_Q_PRIORITY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_EGR_Q_PRIORITY));
  info->tc = 0;
  info->dp = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PB_DEBUG_IS_LVL1

void
  SOC_PB_EGR_Q_PRIORITY_print(
    SOC_SAND_IN  SOC_PB_EGR_Q_PRIORITY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("tc: %u\n\r",info->tc);
  soc_sand_os_printf("dp: %u\n\r",info->dp);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}
void
  soc_pb_egr_q_nif_cal_get_and_print(
    SOC_SAND_IN int unit
  )
{
  uint32
    fld_val=0,
    entry_offset,
    res;
  SOC_PB_EGQ_FQP_NIF_PORT_MUX_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_EGR_Q_NIF_CAL_GET_AND_PRINT);

  for (entry_offset = 0 ; entry_offset < 128 &&  fld_val != 127 ; entry_offset ++)
  {
    res = soc_pb_egq_fqp_nif_port_mux_tbl_get_unsafe(
            unit,
            entry_offset,
            &tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    soc_sand_os_printf(" Mux table entry: %u\n\r", entry_offset);
    soc_sand_os_printf("Data: %u\n\r", tbl_data.fqp_nif_port_mux);
    fld_val = tbl_data.fqp_nif_port_mux;
  }

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif /* SOC_PB_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

