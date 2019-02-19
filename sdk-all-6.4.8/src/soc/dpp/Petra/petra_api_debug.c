/* $Id: petra_api_debug.c,v 1.6 Broadcom SDK $
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



/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/Petra/petra_api_debug.h>
#include <soc/dpp/Petra/petra_debug.h>
#include <soc/dpp/Petra/petra_api_framework.h>

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

/* $Id: petra_api_debug.c,v 1.6 Broadcom SDK $
 *  Relevant when working in Mesh mode, Soc_petra-A only;
 *  retrieves the FAPs (FAP-Id 3 least significant bits, indicating Mesh Context) for which
 *  there are no connecting links.
 *  This API can be used to implement a WA for the following 
 *  Soc_petra-A Errata: in Mesh mode - extraction of Soc_petra can cause remaining Soc_petra devices to get stuck.
 *  The WA is to call this API periodically, or after extracting a Soc_petra device.
 *  The bitmap can be compared to the expected. If the bitmap for a specific FAP device is '1', 
 *  i.e. it has no links, but the device is known to be in System, ingress reset should be applied to the device.
 */
uint32
  soc_petra_dbg_faps_with_no_mesh_links_get(
    SOC_SAND_IN  int unit,
    SOC_SAND_OUT uint32  *fap_bitmap
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DBG_FAPS_WITH_NO_MESH_LINKS_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_dbg_faps_with_no_mesh_links_get_unsafe(
          unit,
          fap_bitmap
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_dbg_faps_with_no_mesh_links_get()", 0, 0);
}

/*********************************************************************
*     Enable/disable if the traffic route should be forced. If
*     enabled, the traffic route either goes through the
*     fabric or remains local. Otherwise, the traffic route is
*     not forced.
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_petra_dbg_route_force_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_DBG_FORCE_MODE      force_mode
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DBG_ROUTE_FORCE_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;


  res = soc_petra_dbg_route_force_verify(
          unit,
          force_mode
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_dbg_route_force_set_unsafe(
          unit,
          force_mode
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_dbg_route_force_set()",0,0);
}

/*********************************************************************
*     Enable/disable if the traffic route should be forced. If
*     enabled, the traffic route either goes through the
*     fabric or remains local. Otherwise, the traffic route is
*     not forced.
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_petra_dbg_route_force_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_DBG_FORCE_MODE      *force_mode
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DBG_ROUTE_FORCE_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(force_mode);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_dbg_route_force_get_unsafe(
          unit,
          force_mode
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_dbg_route_force_get()",0,0);
}

/*********************************************************************
*     Configure the Scheduler AutoCredit parameters.
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_petra_dbg_autocredit_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_DBG_AUTOCREDIT_INFO *info,
    SOC_SAND_OUT uint32                  *exact_rate
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DBG_AUTOCREDIT_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(exact_rate);

  res = soc_petra_dbg_autocredit_verify(
          unit,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_dbg_autocredit_set_unsafe(
          unit,
          info,
          exact_rate
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_dbg_autocredit_set()",0,0);
}

/*********************************************************************
*     Configure the Scheduler AutoCredit parameters.
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_petra_dbg_autocredit_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_DBG_AUTOCREDIT_INFO *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DBG_AUTOCREDIT_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_dbg_autocredit_get_unsafe(
          unit,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_dbg_autocredit_get()",0,0);
}

/*********************************************************************
*     Enable/disable the egress shaping.
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_petra_dbg_egress_shaping_enable_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 enable
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DBG_EGRESS_SHAPING_ENABLE_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;


  res = soc_petra_dbg_egress_shaping_enable_verify(
          unit,
          enable
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_dbg_egress_shaping_enable_set_unsafe(
          unit,
          enable
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_dbg_egress_shaping_enable_set()",0,0);
}

/*********************************************************************
*     Enable/disable the egress shaping.
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_petra_dbg_egress_shaping_enable_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT uint8                 *enable
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DBG_EGRESS_SHAPING_ENABLE_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(enable);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_dbg_egress_shaping_enable_get_unsafe(
          unit,
          enable
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_dbg_egress_shaping_enable_get()",0,0);
}

/*********************************************************************
*     Enable/disable device-level flow control.
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_petra_dbg_flow_control_enable_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 enable
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DBG_FLOW_CONTROL_ENABLE_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;


  res = soc_petra_dbg_flow_control_enable_verify(
          unit,
          enable
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_dbg_flow_control_enable_set_unsafe(
          unit,
          enable
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_dbg_flow_control_enable_set()",0,0);
}

/*********************************************************************
*     Enable/disable device-level flow control.
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_petra_dbg_flow_control_enable_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT uint8                 *enable
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DBG_FLOW_CONTROL_ENABLE_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(enable);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_dbg_flow_control_enable_get_unsafe(
          unit,
          enable
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_dbg_flow_control_enable_get()",0,0);
}

/*********************************************************************
*     Flush one queue at the ingress.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_dbg_queue_flush(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 queue_ndx,
    SOC_SAND_IN  SOC_PETRA_DBG_FLUSH_MODE      mode
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DBG_QUEUE_FLUSH);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_dbg_queue_flush_unsafe(
          unit,
          queue_ndx,
          mode
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_dbg_queue_flush()",0,0);
}

/*********************************************************************
*     Flush all the queues at the ingress.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_dbg_queue_flush_all(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_DBG_FLUSH_MODE      mode
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DBG_QUEUE_FLUSH_ALL);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_dbg_queue_flush_all_unsafe(
          unit,
          mode
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_dbg_queue_flush_all()",0,0);
}

/*********************************************************************
*     Resets the ingress pass. The following blocks are
*     soft-reset (running soft-init): IPS, IQM, IPT, MMU,
*     DPRC, IRE, IHP, IDR, IRR. As part of the reset sequence,
*     traffic is stopped, and re-started (according to the
*     original condition).
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_dbg_ingr_reset(
    SOC_SAND_IN  int                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DBG_INGR_RESET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_dbg_ingr_reset_unsafe(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_dbg_ingr_reset()",0,0);
}

/*********************************************************************
 *     Soft-resets the device. As part of the reset sequence,
 *     traffic is stopped, and re-started (according to the
 *     original condition).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_dbg_dev_reset(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_DBG_RST_DOMAIN      rst_domain
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DBG_DEV_RESET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = soc_petra_dbg_dev_reset_verify(
          rst_domain
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_dbg_dev_reset_unsafe(
          unit,
          rst_domain
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PETRA_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_dbg_dev_reset()", 0, 0);
}

/*********************************************************************
*     This function tests the ECI access to Soc_petra.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_dbg_eci_access_tst(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 nof_k_iters,
    SOC_SAND_IN  uint8                 use_dflt_tst_reg,
    SOC_SAND_IN  uint32                  reg1_addr_longs,
    SOC_SAND_IN  uint32                  reg2_addr_longs,
    SOC_SAND_OUT uint8                 *is_valid
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DBG_ECI_ACCESS_TST);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(is_valid);
  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_dbg_eci_access_tst_unsafe(
          unit,
          nof_k_iters,
          use_dflt_tst_reg,
          reg1_addr_longs,
          reg2_addr_longs,
          is_valid
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_dbg_eci_access_tst()",0,0);
}

/*********************************************************************
*     Resets the end-to-end scheduler. The reset is performed
 *     by clearing the internal scheduler pipes, and then
 *     performing soft-reset.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_dbg_sch_reset(
    SOC_SAND_IN  int unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DBG_SCH_RESET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;
  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_dbg_sch_reset_unsafe(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PETRA_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_dbg_sch_reset()", 0, 0);
}

/*********************************************************************
*     Test a Flow Shaper as a potential-stuck-shaper. The
 *     criteria is: - The shaper is active, i.e. configured
 *     max-burst and rate are non-zero. - The shaper bucket is
 *     empty (zero-size). Note: this state is a valid state,
 *     since a bucket can be emptied by received credits. The
 *     stages for detecting a stuck shaper are: 1. Detect a
 *     suspected shaper (empty bucket) 2. Start measuring the
 *     credits to the suspected shaper using a programmable
 *     counter 3. Wait for at least 500 milliseconds to see if
 *     the shaper receives credits 4. Confirm a stuck shaper:
 *     check if the shaper bucket is empty. If empty, and no
 *     credits were received over the measurement period, the
 *     shaper is stuck. This confirmation is done by the
 *     dbg_sch_suspect_spr_confirm API.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_dbg_sch_suspect_spr_detect(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  uint32 flow_id,
    SOC_SAND_OUT uint8 *is_suspect
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DBG_SCH_SUSPECT_SPR_DETECT);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(is_suspect);

  res = soc_petra_dbg_sch_suspect_spr_detect_verify(
          unit,
          flow_id
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_dbg_sch_suspect_spr_detect_unsafe(
          unit,
          flow_id,
          is_suspect
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PETRA_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_dbg_sch_suspect_spr_detect()", 0, 0);
}

/*********************************************************************
*     Confirm a potential-stuck-shaper. The stages for
 *     detecting a stuck shaper are: 1. Detect a suspected
 *     shaper (empty bucket) 2. Start measuring the credits to
 *     the suspected shaper using a programmable counter 3.
 *     Wait for at least 500 milliseconds to see if the shaper
 *     receives credits 4. Confirm a stuck shaper: check if the
 *     shaper bucket is empty. If empty, and no credits were
 *     received over the measurement period, the shaper is
 *     stuck.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_dbg_sch_suspect_spr_confirm(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  uint32 flow_id,
    SOC_SAND_OUT uint8 *is_confirmed
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DBG_SCH_SUSPECT_SPR_CONFIRM);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(is_confirmed);

  res = soc_petra_dbg_sch_suspect_spr_confirm_verify(
          unit,
          flow_id
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_dbg_sch_suspect_spr_confirm_unsafe(
          unit,
          flow_id,
          is_confirmed
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PETRA_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_dbg_sch_suspect_spr_confirm()", 0, 0);
}


/*********************************************************************
*     Measures the PCM values for the device's blocks.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_dbg_pcm_readings_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_DBG_PCM_RESULTS     *results
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DBG_PCM_READINGS_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(results);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_dbg_pcm_readings_get_unsafe(
          unit,
          results
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PETRA_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_dbg_pcm_readings_get()", 0, 0);
}

void
  soc_petra_PETRA_DBG_AUTOCREDIT_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_DBG_AUTOCREDIT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_DBG_AUTOCREDIT_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_DBG_PCM_RESULTS_clear(
    SOC_SAND_OUT SOC_PETRA_DBG_PCM_RESULTS *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_DBG_PCM_RESULTS));
  info->eci = 0;
  info->ips = 0;
  info->iqm = 0;
  for (ind = 0; ind < SOC_PETRA_BLK_NOF_INSTANCES_DPI; ++ind)
  {
    info->dprc[ind] = 0;
  }
  info->ihp = 0;
  info->idr = 0;
  info->fdr = 0;
  info->egq1 = 0;
  info->egq2 = 0;
  info->epni = 0;
  info->msw = 0;
  for (ind = 0; ind < SOC_PETRA_BLK_NOF_INSTANCES_NIF; ++ind)
  {
    info->nif1[ind] = 0;
  }
  for (ind = 0; ind < SOC_PETRA_BLK_NOF_INSTANCES_NIF; ++ind)
  {
    info->nif2[ind] = 0;
  }
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PETRA_DEBUG_IS_LVL1

const char*
  soc_petra_PETRA_DBG_FORCE_MODE_to_string(
    SOC_SAND_IN  SOC_PETRA_DBG_FORCE_MODE enum_val
  )
{
  return SOC_TMC_DBG_FORCE_MODE_to_string(enum_val);
}

const char*
  soc_petra_PETRA_DBG_FLUSH_MODE_to_string(
    SOC_SAND_IN  SOC_PETRA_DBG_FLUSH_MODE enum_val
  )
{
  return SOC_TMC_DBG_FLUSH_MODE_to_string(enum_val);
}

const char*
  soc_petra_PETRA_DBG_RST_DOMAIN_to_string(
    SOC_SAND_IN  SOC_PETRA_DBG_RST_DOMAIN enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PETRA_DBG_RST_DOMAIN_INGR:
    str = "ingr";
  break;
  case SOC_PETRA_DBG_RST_DOMAIN_EGR:
    str = "egr";
  break;
  case SOC_PETRA_DBG_RST_DOMAIN_SCH:
    str = "sch";
  break;
  case SOC_PETRA_DBG_RST_DOMAIN_FULL:
    str = "full";
  break;
  case SOC_PETRA_DBG_NOF_RST_DOMAINS:
    str = " Not initialized";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

void
  soc_petra_PETRA_DBG_AUTOCREDIT_INFO_print(
    SOC_SAND_IN  SOC_PETRA_DBG_AUTOCREDIT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_DBG_AUTOCREDIT_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_DBG_PCM_RESULTS_print(
    SOC_SAND_IN  SOC_PETRA_DBG_PCM_RESULTS *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("eci: %u\n\r",info->eci);
  soc_sand_os_printf("ips: %u\n\r",info->ips);
  soc_sand_os_printf("iqm: %u\n\r",info->iqm);
  for (ind = 0; ind < SOC_PETRA_BLK_NOF_INSTANCES_DPI; ++ind)
  {
    soc_sand_os_printf("dprc[%u]: %u\n\r",ind,info->dprc[ind]);
  }
  soc_sand_os_printf("ihp: %u\n\r",info->ihp);
  soc_sand_os_printf("idr: %u\n\r",info->idr);
  soc_sand_os_printf("fdr: %u\n\r",info->fdr);
  soc_sand_os_printf("egq1: %u\n\r",info->egq1);
  soc_sand_os_printf("egq2: %u\n\r",info->egq2);
  soc_sand_os_printf("epni: %u\n\r",info->epni);
  soc_sand_os_printf("msw: %u\n\r",info->msw);
  for (ind = 0; ind < SOC_PETRA_BLK_NOF_INSTANCES_NIF; ++ind)
  {
    soc_sand_os_printf("nif1[%u]: %u\n\r",ind,info->nif1[ind]);
  }
  for (ind = 0; ind < SOC_PETRA_BLK_NOF_INSTANCES_NIF; ++ind)
  {
    soc_sand_os_printf("nif2[%u]: %u\n\r",ind,info->nif2[ind]);
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif /* SOC_PETRA_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

