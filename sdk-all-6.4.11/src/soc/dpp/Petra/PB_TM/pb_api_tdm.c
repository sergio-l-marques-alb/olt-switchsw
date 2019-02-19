
/* $Id: pb_api_tdm.c,v 1.8 Broadcom SDK $
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
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_TM/src/soc_pb_api_tdm.c
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
#include <soc/dpp/Petra/PB_TM/pb_framework.h>
#include <soc/dpp/Petra/PB_TM/pb_api_tdm.h>
#include <soc/dpp/Petra/PB_TM/pb_tdm.h>

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
*     Configure the FTMH header operation
 *     (added/unchanged/removed) at the ingress and egress,
 *     with the FTMH fields if added.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_tdm_ftmh_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core_id,
    SOC_SAND_IN  uint32                   port_ndx,
    SOC_SAND_IN  SOC_PB_TDM_FTMH_INFO            *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_TDM_FTMH_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_tdm_ftmh_set_verify(
          unit,
          port_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

  res = soc_pb_tdm_ftmh_set_unsafe(
          unit,
          port_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tdm_ftmh_set()", port_ndx, 0);
}

/*********************************************************************
*     Configure the FTMH header operation
 *     (added/unchanged/removed) at the ingress and egress,
 *     with the FTMH fields if added.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_tdm_ftmh_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core_id,
    SOC_SAND_IN  uint32                   port_ndx,
    SOC_SAND_OUT SOC_PB_TDM_FTMH_INFO            *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_TDM_FTMH_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_tdm_ftmh_get_verify(
          unit,
          port_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

  res = soc_pb_tdm_ftmh_get_unsafe(
          unit,
          port_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tdm_ftmh_get()", port_ndx, 0);
}

/*********************************************************************
*     Configure the size limitations for the TDM cells in the
 *     Optimized FTMH TDM traffic mode.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_tdm_opt_size_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                    cell_size
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_TDM_OPT_SIZE_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_tdm_opt_size_set_verify(
          unit,
          cell_size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

  res = soc_pb_tdm_opt_size_set_unsafe(
          unit,
          cell_size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tdm_opt_size_set()", 0, 0);
}

/*********************************************************************
*     Configure the size limitations for the TDM cells in the
 *     Optimized FTMH TDM traffic mode.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_tdm_opt_size_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_OUT uint32                    *cell_size
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_TDM_OPT_SIZE_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(cell_size);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_tdm_opt_size_get_verify(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

  res = soc_pb_tdm_opt_size_get_unsafe(
          unit,
          cell_size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tdm_opt_size_get()", 0, 0);
}

/*********************************************************************
*     Configure the size limitations for the TDM cells in the
 *     Standard FTMH TDM traffic mode.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_tdm_stand_size_range_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_SAND_U32_RANGE              *size_range
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_TDM_STAND_SIZE_RANGE_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(size_range);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_tdm_stand_size_range_set_verify(
          unit,
          size_range
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

  res = soc_pb_tdm_stand_size_range_set_unsafe(
          unit,
          size_range
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tdm_stand_size_range_set()", 0, 0);
}

/*********************************************************************
*     Configure the size limitations for the TDM cells in the
 *     Standard FTMH TDM traffic mode.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_tdm_stand_size_range_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_OUT SOC_SAND_U32_RANGE              *size_range
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_TDM_STAND_SIZE_RANGE_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(size_range);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_tdm_stand_size_range_get_verify(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

  res = soc_pb_tdm_stand_size_range_get_unsafe(
          unit,
          size_range
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tdm_stand_size_range_get()", 0, 0);
}
/*********************************************************************
*     Set the TDM multicast static route configuration. Up to
 *     16 routes can be defined. For a TDM Multicast cell, its
 *     selected Route is done according to the 4 MSB of its
 *     Multicast-ID.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_tdm_mc_static_route_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   mc_id_route_ndx,
    SOC_SAND_IN  SOC_PB_TDM_MC_STATIC_ROUTE_INFO *route_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_TDM_MC_STATIC_ROUTE_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(route_info);

  res = soc_pb_tdm_mc_static_route_set_verify(
          unit,
          mc_id_route_ndx,
          route_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_tdm_mc_static_route_set_unsafe(
          unit,
          mc_id_route_ndx,
          route_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tdm_mc_static_route_set()", mc_id_route_ndx, 0);
}

/*********************************************************************
*     Set the TDM multicast static route configuration. Up to
 *     16 routes can be defined. For a TDM Multicast cell, its
 *     selected Route is done according to the 4 MSB of its
 *     Multicast-ID.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_tdm_mc_static_route_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   mc_id_route_ndx,
    SOC_SAND_OUT SOC_PB_TDM_MC_STATIC_ROUTE_INFO *route_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_TDM_MC_STATIC_ROUTE_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(route_info);

  res = soc_pb_tdm_mc_static_route_get_verify(
          unit,
          mc_id_route_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_tdm_mc_static_route_get_unsafe(
          unit,
          mc_id_route_ndx,
          route_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tdm_mc_static_route_get()", mc_id_route_ndx, 0);
}

void
  SOC_PB_TDM_FTMH_OPT_UC_clear(
    SOC_SAND_OUT SOC_PB_TDM_FTMH_OPT_UC *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_TDM_FTMH_OPT_UC_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_TDM_FTMH_OPT_MC_clear(
    SOC_SAND_OUT SOC_PB_TDM_FTMH_OPT_MC *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_TDM_FTMH_OPT_MC_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_TDM_FTMH_STANDARD_UC_clear(
    SOC_SAND_OUT SOC_PB_TDM_FTMH_STANDARD_UC *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_TDM_FTMH_STANDARD_UC_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_TDM_FTMH_STANDARD_MC_clear(
    SOC_SAND_OUT SOC_PB_TDM_FTMH_STANDARD_MC *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_TDM_FTMH_STANDARD_MC_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_TDM_FTMH_clear(
    SOC_SAND_OUT SOC_PB_TDM_FTMH *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_TDM_FTMH_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_TDM_FTMH_INFO_clear(
    SOC_SAND_OUT SOC_PB_TDM_FTMH_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_TDM_FTMH_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}
void
  SOC_PB_TDM_MC_STATIC_ROUTE_INFO_clear(
    SOC_SAND_OUT SOC_PB_TDM_MC_STATIC_ROUTE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_TDM_MC_STATIC_ROUTE_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PB_DEBUG_IS_LVL1

const char*
  SOC_PB_TDM_ING_ACTION_to_string(
    SOC_SAND_IN  SOC_PB_TDM_ING_ACTION enum_val
  )
{
  return SOC_TMC_TDM_ING_ACTION_to_string(enum_val);
}

const char*
  SOC_PB_TDM_EG_ACTION_to_string(
    SOC_SAND_IN  SOC_PB_TDM_EG_ACTION enum_val
  )
{
  return SOC_TMC_TDM_EG_ACTION_to_string(enum_val);
}

void
  SOC_PB_TDM_FTMH_OPT_UC_print(
    SOC_SAND_IN  SOC_PB_TDM_FTMH_OPT_UC *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_TDM_FTMH_OPT_UC_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_TDM_FTMH_OPT_MC_print(
    SOC_SAND_IN  SOC_PB_TDM_FTMH_OPT_MC *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_TDM_FTMH_OPT_MC_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_TDM_FTMH_STANDARD_UC_print(
    SOC_SAND_IN  SOC_PB_TDM_FTMH_STANDARD_UC *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_TDM_FTMH_STANDARD_UC_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_TDM_FTMH_STANDARD_MC_print(
    SOC_SAND_IN  SOC_PB_TDM_FTMH_STANDARD_MC *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_TDM_FTMH_STANDARD_MC_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_TDM_FTMH_print(
    SOC_SAND_IN  SOC_PB_TDM_FTMH *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_TDM_FTMH_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_TDM_FTMH_INFO_print(
    SOC_SAND_IN  SOC_PB_TDM_FTMH_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_TDM_FTMH_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}
void
  SOC_PB_TDM_MC_STATIC_ROUTE_INFO_print(
    SOC_SAND_IN  SOC_PB_TDM_MC_STATIC_ROUTE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_TDM_MC_STATIC_ROUTE_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif /* SOC_PB_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

