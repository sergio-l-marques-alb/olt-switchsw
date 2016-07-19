/* $Id: petra_api_ingress_scheduler.c,v 1.7 Broadcom SDK $
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
* FILENAME:       DuneDriver/soc_petra/src/soc_petra_api_ingress_scheduler.c
*
* MODULE PREFIX:  soc_petra_ingress_scheduler
*
* FILE DESCRIPTION:  in the H file.
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

#include <soc/dpp/Petra/petra_ingress_scheduler.h>

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
*     MESH fabric (no FE) configure the ingress scheduler. The
*     configuration includes: [per-destination]-shaper-rates,
*     [per-destination]-weights )
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ingress_scheduler_mesh_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_ING_SCH_MESH_INFO   *mesh_info,
    SOC_SAND_OUT SOC_PETRA_ING_SCH_MESH_INFO   *exact_mesh_info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_INGRESS_SCHEDULER_MESH_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(mesh_info);
  SOC_SAND_CHECK_NULL_INPUT(exact_mesh_info);

  SOC_SAND_SET_ERROR_CODE(SOC_PETRA_API_NOT_FUNCTIONAL_ERR, 5, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ingress_scheduler_mesh_set()",0,0);
}

/*********************************************************************
*     MESH fabric (no FE) configure the ingress scheduler. The
*     configuration includes: [per-destination]-shaper-rates,
*     [per-destination]-weights )
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ingress_scheduler_mesh_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_ING_SCH_MESH_INFO   *mesh_info
  )
{
  uint32  res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_INGRESS_SCHEDULER_MESH_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(mesh_info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_ingress_scheduler_mesh_get_unsafe(
    unit,
    mesh_info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ingress_scheduler_mesh_get()",0,0);
}

/*********************************************************************
*     This procedure configure the ingress scheduler when
*     working with SOC_SAND CLOS fabric (that is SOC_SAND_FE200/SOC_SAND_FE600). The
*     configuration includes: [local/fabric]-shaper-rates,
*     [local/fabric]-weights.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ingress_scheduler_clos_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_ING_SCH_CLOS_INFO   *clos_info,
    SOC_SAND_OUT SOC_PETRA_ING_SCH_CLOS_INFO   *exact_clos_info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_INGRESS_SCHEDULER_CLOS_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(clos_info);
  SOC_SAND_CHECK_NULL_INPUT(exact_clos_info);

  SOC_SAND_SET_ERROR_CODE(SOC_PETRA_API_NOT_FUNCTIONAL_ERR, 5, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ingress_scheduler_clos_set()",0,0);
}

/*********************************************************************
*     This procedure configure the ingress scheduler when
*     working with SOC_SAND CLOS fabric (that is SOC_SAND_FE200/SOC_SAND_FE600). The
*     configuration includes: [local/fabric]-shaper-rates,
*     [local/fabric]-weights.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ingress_scheduler_clos_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_ING_SCH_CLOS_INFO   *clos_info
  )
{
  uint32  res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_INGRESS_SCHEDULER_CLOS_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(clos_info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_ingress_scheduler_clos_get_unsafe(
    unit,
    clos_info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ingress_scheduler_clos_get()",0,0);
}

void
  soc_petra_PETRA_ING_SCH_SHAPER_clear(
    SOC_SAND_OUT SOC_PETRA_ING_SCH_SHAPER *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ING_SCH_SHAPER_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_ING_SCH_MESH_CONTEXT_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_ING_SCH_MESH_CONTEXT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ING_SCH_MESH_CONTEXT_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_ING_SCH_MESH_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_ING_SCH_MESH_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ING_SCH_MESH_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_ING_SCH_CLOS_WFQ_ELEMENT_clear(
    SOC_SAND_OUT SOC_PETRA_ING_SCH_CLOS_WFQ_ELEMENT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ING_SCH_CLOS_WFQ_ELEMENT_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_ING_SCH_CLOS_WFQS_clear(
    SOC_SAND_OUT SOC_PETRA_ING_SCH_CLOS_WFQS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ING_SCH_CLOS_WFQS_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_ING_SCH_CLOS_HP_SHAPERS_clear(
    SOC_SAND_OUT SOC_PETRA_ING_SCH_CLOS_HP_SHAPERS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ING_SCH_CLOS_HP_SHAPERS_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_ING_SCH_CLOS_SHAPERS_clear(
    SOC_SAND_OUT SOC_PETRA_ING_SCH_CLOS_SHAPERS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ING_SCH_CLOS_SHAPERS_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_ING_SCH_CLOS_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_ING_SCH_CLOS_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ING_SCH_CLOS_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PETRA_DEBUG_IS_LVL1

const char*
  soc_petra_PETRA_ING_SCH_MESH_CONTEXTS_to_string(
    SOC_SAND_IN  SOC_PETRA_ING_SCH_MESH_CONTEXTS enum_val
  )
{
  return SOC_TMC_ING_SCH_MESH_CONTEXTS_to_string(enum_val);
}

void
  soc_petra_PETRA_ING_SCH_SHAPER_print(
    SOC_SAND_IN  SOC_PETRA_ING_SCH_SHAPER *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ING_SCH_SHAPER_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_ING_SCH_MESH_CONTEXT_INFO_print(
    SOC_SAND_IN  SOC_PETRA_ING_SCH_MESH_CONTEXT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ING_SCH_MESH_CONTEXT_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_ING_SCH_MESH_INFO_print(
    SOC_SAND_IN  SOC_PETRA_ING_SCH_MESH_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ING_SCH_MESH_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_ING_SCH_CLOS_WFQ_ELEMENT_print(
    SOC_SAND_IN  SOC_PETRA_ING_SCH_CLOS_WFQ_ELEMENT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ING_SCH_CLOS_WFQ_ELEMENT_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_ING_SCH_CLOS_WFQS_print(
    SOC_SAND_IN  SOC_PETRA_ING_SCH_CLOS_WFQS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ING_SCH_CLOS_WFQS_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_ING_SCH_CLOS_HP_SHAPERS_print(
    SOC_SAND_IN  SOC_PETRA_ING_SCH_CLOS_HP_SHAPERS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ING_SCH_CLOS_HP_SHAPERS_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_ING_SCH_CLOS_SHAPERS_print(
    SOC_SAND_IN  SOC_PETRA_ING_SCH_CLOS_SHAPERS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ING_SCH_CLOS_SHAPERS_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_ING_SCH_CLOS_INFO_print(
    SOC_SAND_IN  SOC_PETRA_ING_SCH_CLOS_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_ING_SCH_CLOS_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif /* SOC_PETRA_DEBUG_IS_LVL1 */

uint8
  soc_petra_ingress_scheduler_conversion_test_api(
    SOC_SAND_IN uint8 is_regression,
    SOC_SAND_IN uint8 silent
  )
{
  uint8 pass;

  pass = soc_petra_ingress_scheduler_conversion_test(
           is_regression,
           silent
         );

  if (!pass)
  {
    soc_sand_os_printf(
      "The soc_petra_ingress_scheduler_conversion_test has FAILED!"
      "\n\r"
    );
    goto exit;
  }
  else
  {
    soc_sand_os_printf(
      "The soc_petra_ingress_scheduler_conversion_test has passed successfully!"
      "\n\r"
    );
  }

exit:
  return pass;

}

#include <soc/dpp/SAND/Utils/sand_footer.h>
