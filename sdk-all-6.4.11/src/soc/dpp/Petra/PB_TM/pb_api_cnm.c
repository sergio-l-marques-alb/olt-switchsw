/* $Id: pb_api_cnm.c,v 1.7 Broadcom SDK $
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
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_TM/src/soc_pb_api_cnm.c
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
#include <soc/dpp/Petra/PB_TM/pb_api_cnm.h>
#include <soc/dpp/Petra/PB_TM/pb_cnm.h>

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
*     Set the Congestion Point functionality, in particular
 *     the packet generation mode and the fields of the CNM
 *     packet.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_cnm_cp_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PB_CNM_CP_INFO              *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CNM_CP_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_pb_cnm_cp_set_verify(
          unit,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_cnm_cp_set_unsafe(
          unit,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_cnm_cp_set()", 0, 0);
}

/*********************************************************************
*     Set the Congestion Point functionality, in particular
 *     the packet generation mode and the fields of the CNM
 *     packet.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_cnm_cp_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_OUT SOC_PB_CNM_CP_INFO              *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CNM_CP_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_pb_cnm_cp_get_verify(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_cnm_cp_get_unsafe(
          unit,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_cnm_cp_get()", 0, 0);
}

/*********************************************************************
*     Set the mapping from the pair (destination, traffic
 *     class) to the CP Queue.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_cnm_q_mapping_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PB_CNM_Q_MAPPING_INFO       *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CNM_Q_MAPPING_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_pb_cnm_q_mapping_set_verify(
          unit,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_cnm_q_mapping_set_unsafe(
          unit,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_cnm_q_mapping_set()", 0, 0);
}

/*********************************************************************
*     Set the mapping from the pair (destination, traffic
 *     class) to the CP Queue.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_cnm_q_mapping_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_OUT SOC_PB_CNM_Q_MAPPING_INFO       *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CNM_Q_MAPPING_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_pb_cnm_q_mapping_get_verify(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_cnm_q_mapping_get_unsafe(
          unit,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_cnm_q_mapping_get()", 0, 0);
}

/*********************************************************************
*     Set parameters of the congestion test.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_cnm_congestion_test_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PB_CNM_CONGESTION_TEST_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CNM_CONGESTION_TEST_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_pb_cnm_congestion_test_set_verify(
          unit,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_cnm_congestion_test_set_unsafe(
          unit,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_cnm_congestion_test_set()", 0, 0);
}

/*********************************************************************
*     Set parameters of the congestion test.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_cnm_congestion_test_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_OUT SOC_PB_CNM_CONGESTION_TEST_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CNM_CONGESTION_TEST_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_pb_cnm_congestion_test_get_verify(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_cnm_congestion_test_get_unsafe(
          unit,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_cnm_congestion_test_get()", 0, 0);
}

/*********************************************************************
*     Configure an entry into the Congestion Point Profile
 *     Table.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_cnm_cp_profile_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   profile_ndx,
    SOC_SAND_IN  SOC_PB_CNM_CP_PROFILE_INFO      *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CNM_CP_PROFILE_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_pb_cnm_cp_profile_set_verify(
          unit,
          profile_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_cnm_cp_profile_set_unsafe(
          unit,
          profile_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_cnm_cp_profile_set()", profile_ndx, 0);
}

/*********************************************************************
*     Configure an entry into the Congestion Point Profile
 *     Table.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_cnm_cp_profile_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   profile_ndx,
    SOC_SAND_OUT SOC_PB_CNM_CP_PROFILE_INFO      *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CNM_CP_PROFILE_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_pb_cnm_cp_profile_get_verify(
          unit,
          profile_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_cnm_cp_profile_get_unsafe(
          unit,
          profile_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_cnm_cp_profile_get()", profile_ndx, 0);
}

/*********************************************************************
*     Configure an entry into the Congestion Point Profile
 *     Table for the sampling mode.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_cnm_sampling_profile_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   profile_ndx,
    SOC_SAND_IN  uint32                    sampling_rate
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CNM_SAMPLING_PROFILE_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = soc_pb_cnm_sampling_profile_set_verify(
          unit,
          profile_ndx,
          sampling_rate
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_cnm_sampling_profile_set_unsafe(
          unit,
          profile_ndx,
          sampling_rate
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_cnm_sampling_profile_set()", profile_ndx, 0);
}

/*********************************************************************
*     Configure an entry into the Congestion Point Profile
 *     Table for the sampling mode.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_cnm_sampling_profile_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   profile_ndx,
    SOC_SAND_OUT uint32                    *sampling_rate
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CNM_SAMPLING_PROFILE_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(sampling_rate);

  res = soc_pb_cnm_sampling_profile_get_verify(
          unit,
          profile_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_cnm_sampling_profile_get_unsafe(
          unit,
          profile_ndx,
          sampling_rate
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_cnm_sampling_profile_get()", profile_ndx, 0);
}

/*********************************************************************
*     Configure the CP Queue attributes into an entry of the
 *     Congestion Point Table.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_cnm_cpq_pp_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   cpq_ndx,
    SOC_SAND_IN  SOC_PB_CNM_CPQ_INFO             *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CNM_CPQ_PP_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_pb_cnm_cpq_pp_set_verify(
          unit,
          cpq_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_cnm_cpq_pp_set_unsafe(
          unit,
          cpq_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_cnm_cpq_pp_set()", cpq_ndx, 0);
}

/*********************************************************************
*     Configure the CP Queue attributes into an entry of the
 *     Congestion Point Table.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_cnm_cpq_pp_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   cpq_ndx,
    SOC_SAND_OUT SOC_PB_CNM_CPQ_INFO             *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CNM_CPQ_PP_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_pb_cnm_cpq_pp_get_verify(
          unit,
          cpq_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_cnm_cpq_pp_get_unsafe(
          unit,
          cpq_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_cnm_cpq_pp_get()", cpq_ndx, 0);
}

/*********************************************************************
*     Configure the CP Queue attributes into an entry of the
 *     Congestion Point Table.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_cnm_cpq_sampling_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   cpq_ndx,
    SOC_SAND_IN  SOC_PB_CNM_CPQ_INFO             *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CNM_CPQ_SAMPLING_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_pb_cnm_cpq_sampling_set_verify(
          unit,
          cpq_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_cnm_cpq_sampling_set_unsafe(
          unit,
          cpq_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_cnm_cpq_sampling_set()", cpq_ndx, 0);
}

/*********************************************************************
*     Configure the CP Queue attributes into an entry of the
 *     Congestion Point Table.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_cnm_cpq_sampling_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   cpq_ndx,
    SOC_SAND_OUT SOC_PB_CNM_CPQ_INFO             *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CNM_CPQ_SAMPLING_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_pb_cnm_cpq_sampling_get_verify(
          unit,
          cpq_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_cnm_cpq_sampling_get_unsafe(
          unit,
          cpq_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_cnm_cpq_sampling_get()", cpq_ndx, 0);
}

/*********************************************************************
*     Set the timer delay of the CNM Intercept functionality
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_cnm_intercept_timer_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                     delay
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CNM_INTERCEPT_TIMER_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = soc_pb_cnm_intercept_timer_set_verify(
          unit,
          delay
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_cnm_intercept_timer_set_unsafe(
          unit,
          delay
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_cnm_intercept_timer_set()", 0, 0);
}

/*********************************************************************
*     Set the timer delay of the CNM Intercept functionality
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_cnm_intercept_timer_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_OUT uint32                    *delay
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CNM_INTERCEPT_TIMER_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(delay);

  res = soc_pb_cnm_intercept_timer_get_verify(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_cnm_intercept_timer_get_unsafe(
          unit,
          delay
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_cnm_intercept_timer_get()", 0, 0);
}

void
  SOC_PB_CNM_Q_MAPPING_INFO_clear(
    SOC_SAND_OUT SOC_PB_CNM_Q_MAPPING_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_CNM_Q_MAPPING_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_CNM_CONGESTION_TEST_INFO_clear(
    SOC_SAND_OUT SOC_PB_CNM_CONGESTION_TEST_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_CNM_CONGESTION_TEST_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_CNM_CP_PROFILE_INFO_clear(
    SOC_SAND_OUT SOC_PB_CNM_CP_PROFILE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_CNM_CP_PROFILE_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_CNM_CPQ_INFO_clear(
    SOC_SAND_OUT SOC_PB_CNM_CPQ_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_CNM_CPQ_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_CNM_PPH_clear(
    SOC_SAND_OUT SOC_PB_CNM_PPH *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_CNM_PPH_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_CNM_PDU_clear(
    SOC_SAND_OUT SOC_PB_CNM_PDU *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_CNM_PDU_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_CNM_PETRA_B_PP_clear(
    SOC_SAND_OUT SOC_PB_CNM_PETRA_B_PP *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_CNM_PETRA_B_PP_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_CNM_PACKET_clear(
    SOC_SAND_OUT SOC_PB_CNM_PACKET *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_CNM_PACKET_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_CNM_CP_INFO_clear(
    SOC_SAND_OUT SOC_PB_CNM_CP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_CNM_CP_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PB_DEBUG_IS_LVL1

const char*
  SOC_PB_CNM_GEN_MODE_to_string(
    SOC_SAND_IN  SOC_PB_CNM_GEN_MODE enum_val
  )
{
  return SOC_TMC_CNM_GEN_MODE_to_string(enum_val);
}

const char*
  SOC_PB_CNM_Q_SET_to_string(
    SOC_SAND_IN  SOC_PB_CNM_Q_SET enum_val
  )
{
  return SOC_TMC_CNM_Q_SET_to_string(enum_val);
}

void
  SOC_PB_CNM_Q_MAPPING_INFO_print(
    SOC_SAND_IN  SOC_PB_CNM_Q_MAPPING_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_CNM_Q_MAPPING_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_CNM_CONGESTION_TEST_INFO_print(
    SOC_SAND_IN  SOC_PB_CNM_CONGESTION_TEST_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_CNM_CONGESTION_TEST_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_CNM_CP_PROFILE_INFO_print(
    SOC_SAND_IN  SOC_PB_CNM_CP_PROFILE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_CNM_CP_PROFILE_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_CNM_CPQ_INFO_print(
    SOC_SAND_IN  SOC_PB_CNM_CPQ_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_CNM_CPQ_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_CNM_PPH_print(
    SOC_SAND_IN  SOC_PB_CNM_PPH *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_CNM_PPH_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_CNM_PDU_print(
    SOC_SAND_IN  SOC_PB_CNM_PDU *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_CNM_PDU_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_CNM_PETRA_B_PP_print(
    SOC_SAND_IN  SOC_PB_CNM_PETRA_B_PP *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_CNM_PETRA_B_PP_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_CNM_PACKET_print(
    SOC_SAND_IN  SOC_PB_CNM_PACKET *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_CNM_PACKET_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_CNM_CP_INFO_print(
    SOC_SAND_IN  SOC_PB_CNM_CP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_CNM_CP_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif /* SOC_PB_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

