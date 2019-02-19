/* $Id: petra_api_serdes_utils.c,v 1.8 Broadcom SDK $
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

#include <soc/dpp/Petra/petra_serdes_utils.h>
#include <soc/dpp/Petra/petra_api_general.h>
#include <soc/dpp/Petra/petra_general.h>
#include <soc/dpp/Petra/petra_reg_access.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>

/* } */

/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PETRA_SRD_EYE_SCAN_INVALID (SOC_SAND_U32_MAX - 1)

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
*     Write 8 bit value to the SerDes.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_srd_reg_write(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_SRD_ENTITY_TYPE     entity,
    SOC_SAND_IN  uint32                 entity_ndx,
    SOC_SAND_IN  SOC_PETRA_SRD_REGS_ADDR       *reg,
    SOC_SAND_IN  uint8                   val
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SRD_REG_WRITE);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(reg);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_srd_reg_write_unsafe(
    unit,
    entity,
    entity_ndx,
    reg,
    val
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_srd_reg_write()",0,0);
}

/*********************************************************************
*     Read 8 bit value from the SerDes.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_srd_reg_read(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_SRD_ENTITY_TYPE     entity,
    SOC_SAND_IN  uint32                 entity_ndx,
    SOC_SAND_IN  SOC_PETRA_SRD_REGS_ADDR       *reg,
    SOC_SAND_OUT uint8                   *val
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SRD_REG_READ);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(reg);
  SOC_SAND_CHECK_NULL_INPUT(val);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_srd_reg_read_unsafe(
    unit,
    entity,
    entity_ndx,
    reg,
    val
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_srd_reg_read()",0,0);
}

/*********************************************************************
*     Write a field (up to 8 bit) value to the SerDes.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_srd_fld_write(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_SRD_ENTITY_TYPE     entity,
    SOC_SAND_IN  uint32                 entity_ndx,
    SOC_SAND_IN  SOC_PETRA_SRD_REGS_FIELD      *fld,
    SOC_SAND_IN  uint8                   val
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SRD_FLD_WRITE);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(fld);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_srd_fld_write_unsafe(
    unit,
    entity,
    entity_ndx,
    fld,
    val
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_srd_fld_write()",0,0);
}

/*********************************************************************
*     Read 8 bit value from the SerDes.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_srd_fld_read(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_SRD_ENTITY_TYPE     entity,
    SOC_SAND_IN  uint32                 entity_ndx,
    SOC_SAND_IN  SOC_PETRA_SRD_REGS_FIELD      *fld,
    SOC_SAND_OUT uint8                   *val
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SRD_FLD_READ);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(fld);
  SOC_SAND_CHECK_NULL_INPUT(val);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_srd_fld_read_unsafe(
    unit,
    entity,
    entity_ndx,
    fld,
    val
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_srd_fld_read()",0,0);
}

/*********************************************************************
*     This function sets the equalizer mode and retrieves the
*     eye-sample.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_srd_link_rx_eye_monitor(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 lane_ndx,
    SOC_SAND_IN  SOC_PETRA_SRD_LANE_EQ_MODE    equalizer_mode,
    SOC_SAND_OUT uint32                  *eye_sample
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SRD_LINK_RX_EYE_MONITOR);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(eye_sample);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    equalizer_mode,
    SOC_PETRA_SRD_NOF_LANE_EQ_MODES,
    SOC_PETRA_SRD_LANE_EQ_MODE_OUT_OF_RANGE_ERR,
    10,
    exit
  );

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_srd_link_rx_eye_monitor_unsafe(
          unit,
          lane_ndx,
          equalizer_mode,
          eye_sample
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_srd_link_rx_eye_monitor()",0,0);
}

/*********************************************************************
*     This function sets the loopback mode of a specified
*     lane.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_srd_lane_loopback_mode_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 lane_ndx,
    SOC_SAND_IN  SOC_PETRA_SRD_LANE_LOOPBACK_MODE loopback_mode
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SRD_LANE_LOOPBACK_MODE_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;


  res = soc_petra_srd_lane_loopback_mode_verify(
          unit,
          lane_ndx,
          loopback_mode
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_srd_lane_loopback_mode_set_unsafe(
          unit,
          lane_ndx,
          loopback_mode
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_srd_lane_loopback_mode_set()",0,0);
}

/*********************************************************************
*     This function sets the loopback mode of a specified
*     lane.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_srd_lane_loopback_mode_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 lane_ndx,
    SOC_SAND_OUT SOC_PETRA_SRD_LANE_LOOPBACK_MODE *loopback_mode
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SRD_LANE_LOOPBACK_MODE_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(loopback_mode);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_srd_lane_loopback_mode_get_unsafe(
          unit,
          lane_ndx,
          loopback_mode
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_srd_lane_loopback_mode_get()",0,0);
}

/*********************************************************************
*     Sets the PRBS pattern.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_srd_prbs_mode_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 lane_ndx,
    SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION direction_ndx,
    SOC_SAND_IN  SOC_PETRA_SRD_PRBS_MODE       mode
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SRD_PRBS_MODE_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;


  res = soc_petra_srd_prbs_mode_verify(
    unit,
    lane_ndx,
    direction_ndx,
    mode
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_srd_prbs_mode_set_unsafe(
    unit,
    lane_ndx,
    direction_ndx,
    mode
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_srd_prbs_mode_set()",0,0);
}

/*********************************************************************
*     Sets the PRBS pattern.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_srd_prbs_mode_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 lane_ndx,
    SOC_SAND_OUT SOC_PETRA_SRD_PRBS_MODE       *rx_mode,
    SOC_SAND_OUT SOC_PETRA_SRD_PRBS_MODE       *tx_mode
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SRD_PRBS_MODE_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(rx_mode);
  SOC_SAND_CHECK_NULL_INPUT(tx_mode);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_srd_prbs_mode_get_unsafe(
          unit,
          lane_ndx,
          rx_mode,
          tx_mode
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_srd_prbs_mode_get()",0,0);
}

/*********************************************************************
*     Start PRBS - generation (TX), reception (RX) or both.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_srd_prbs_start(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 lane_ndx,
    SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION direction_ndx
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SRD_PRBS_START);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;


  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_srd_prbs_start_unsafe(
    unit,
    lane_ndx,
    direction_ndx
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_srd_prbs_start()",0,0);
}

/*********************************************************************
*     Stop PRBS - generation (TX), reception (RX) or both.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_srd_prbs_stop(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 lane_ndx,
    SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION direction_ndx
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SRD_PRBS_STOP);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;


  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_srd_prbs_stop_unsafe(
    unit,
    lane_ndx,
    direction_ndx
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_srd_prbs_stop()",0,0);
}

/*********************************************************************
*     Read the status on the receiver side. All indications
*     are cleared on read.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_srd_prbs_get_and_clear_stat(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 lane_ndx,
    SOC_SAND_OUT SOC_PETRA_SRD_PRBS_RX_STATUS  *status
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SRD_PRBS_GET_AND_CLEAR_STAT);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(status);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_srd_prbs_get_and_clear_stat_unsafe(
    unit,
    lane_ndx,
    status
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_srd_prbs_get_and_clear_stat()",0,0);
}

/*********************************************************************
*     This function is used as a diagnostics tool per lane
*     that gives an indication about the lane and CMU's
*     status. It indicates per lane - loss of signal,
*     frequency lock is lost. And per CMU - PLL lock is lost.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_srd_lane_status_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 lane_ndx,
    SOC_SAND_OUT SOC_PETRA_SRD_LANE_STATUS_INFO *lane_stt_info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SRD_LANE_STATUS_INFO_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(lane_stt_info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  /*
   * Read twice to clear sticky indications
   */
  res = soc_petra_srd_lane_status_get_unsafe(
          unit,
          lane_ndx,
          lane_stt_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

  res = soc_petra_srd_lane_status_get_unsafe(
          unit,
          lane_ndx,
          lane_stt_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_srd_lane_status_get()",0,0);
}

/*********************************************************************
*     Prints SerDes diagnostics, per SerDes quartet
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_srd_qrtt_status_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 qrtt_id,
    SOC_SAND_OUT  SOC_PETRA_SRD_QRTT_STATUS_INFO *qrtt_status
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SRD_QRTT_STATUS_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(qrtt_status);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  /*
   * Read twice to clear sticky indications
   */
  res = soc_petra_srd_qrtt_status_get_unsafe(
    unit,
    qrtt_id,
    qrtt_status
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

  res = soc_petra_srd_qrtt_status_get_unsafe(
    unit,
    qrtt_id,
    qrtt_status
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_srd_qrtt_status_get()",0,0);
}

/*********************************************************************
*     Runs the eye scan, in order to map between serdes rx
*     parameters and amount of crc errors. Eye scan sequence
*     is as following: For each tlth * dfelth pair, run prbs
*     for a given time, and collect results to a matrix, per
*     lane. The matrix is used to find optimal rx parameters.
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_petra_srd_eye_scan_run(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_SRD_EYE_SCAN_INFO   *info,
    SOC_SAND_IN  uint8                 silent,
    SOC_SAND_OUT SOC_PETRA_SRD_EYE_SCAN_RES    *result
  )
{
  uint32
    res,
    lane_i;
  SOC_PETRA_SRD_EYE_SCAN_RES
    *res_multi_alloc[SOC_PETRA_SRD_NOF_LANES];

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SRD_EYE_SCAN_RUN);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(result);

  for (lane_i = 0; lane_i < info->nof_lane_ndx_entries; ++lane_i)
  {
    res_multi_alloc[lane_i] = &result[lane_i];
  }

  res = soc_petra_srd_eye_scan_run_multiple_alloc(
    unit,
    info,
    silent,
    res_multi_alloc
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_srd_eye_scan_run()",0,0);
}

/*********************************************************************
*   This functions is the same as soc_petra_srd_eye_scan_run, but receives an
*     array of pointers for the result, instead of an array. This should be
*     used by user who a cannot allocate a consecutive array of result
*     structures.
*********************************************************************/

uint32
  soc_petra_srd_eye_scan_run_multiple_alloc(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_SRD_EYE_SCAN_INFO   *info,
    SOC_SAND_IN  uint8                 silent,
    SOC_SAND_OUT SOC_PETRA_SRD_EYE_SCAN_RES    **result
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SRD_EYE_SCAN_RUN_MULTIPLE_ALLOC);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(result);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_srd_eye_scan_run_multiple_alloc_verify(
          unit,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

  res = soc_petra_srd_eye_scan_run_multiple_alloc_unsafe(
    unit,
    info,
    silent,
    result
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_srd_eye_scan_run_multiple_alloc()",0,0);
}

/*********************************************************************
*     Enables/disables the SCIF interface. The SCIF is a
*     utility serial interface that can be used to access the
*     SerDes for debug configuration and diagnostics. It is
*     not required for normal operation, but may be used to
*     access the SerDes by external tools.
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_petra_srd_scif_enable_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 is_enabled
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SRD_SCIF_ENABLE_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;


  res = soc_petra_srd_scif_enable_verify(
    unit,
    is_enabled
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_srd_scif_enable_set_unsafe(
    unit,
    is_enabled
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_srd_scif_enable_set()",0,0);
}

/*********************************************************************
*     Enables/disables the SCIF interface. The SCIF is a
*     utility serial interface that can be used to access the
*     SerDes for debug configuration and diagnostics. It is
*     not required for normal operation, but may be used to
*     access the SerDes by external tools.
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_petra_srd_scif_enable_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT uint8                 *is_enabled
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SRD_SCIF_ENABLE_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(is_enabled);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_srd_scif_enable_get_unsafe(
    unit,
    is_enabled
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_srd_scif_enable_get()",0,0);
}

/*********************************************************************
*     Performs SerDes CDR relock
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_srd_relock(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    lane_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SRD_RELOCK);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_srd_relock_unsafe(
          unit,
          lane_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_srd_relock()",0,0);
}



void
  soc_petra_PETRA_SRD_PRBS_RX_STATUS_clear(
    SOC_SAND_OUT SOC_PETRA_SRD_PRBS_RX_STATUS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_SRD_PRBS_RX_STATUS));
  info->rx_signal_present = 0;
  info->prbs_signal_stat = SOC_PETRA_SRD_NOF_PRBS_SIGNAL_STATS;
  info->error_cnt = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  soc_petra_PETRA_SRD_LANE_STATUS_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_SRD_LANE_STATUS_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_SRD_LANE_STATUS_INFO));
  info->rx_signal_loss = 0;
  info->rx_not_locked = 0;
  info->rx_signal_detected = 0;
  info->rx_sig_det_changed = 0;
  info->rx_reset = 0;
  info->rx_freq_invalid = 0;
  info->rx_pcs_synced = 0;
  info->rx_code_errors_detected = 0;
  info->rx_disparity_errors_detected = 0;
  info->rx_comma_allignment = 0;
  info->rx_comma_reallign = 0;
  info->rate_divisor = SOC_PETRA_SRD_NOF_RATE_DIVISORS;
  info->rx_power_off = 0;
  info->tx_power_off = 0;
  info->is_in_lpbck = FALSE;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  soc_petra_PETRA_SRD_QRTT_STATUS_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_SRD_QRTT_STATUS_INFO *info
  )
{
  uint32 ind;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_SRD_QRTT_STATUS_INFO));
  info->version = 0;
  info->is_active = FALSE;
  info->pllm = 0;
  info->plln = 0;
  info->pll_not_locked = 0;
  info->pll_vco_not_locked = 0;
  for (ind=0; ind<SOC_PETRA_SRD_NOF_LANES_PER_QRTT; ++ind)
  {
    soc_petra_PETRA_SRD_LANE_STATUS_INFO_clear(&(info->lane_status[ind]));
  }
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  soc_petra_PETRA_SRD_EYE_SCAN_MATRIX_clear(
    SOC_SAND_OUT SOC_PETRA_SRD_EYE_SCAN_MATRIX *info
  )
{
  uint32
    dfelth_i, tlth_i;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_SRD_EYE_SCAN_MATRIX));
  for (tlth_i=0; tlth_i<SOC_PETRA_SRD_EYE_SCAN_TLTH_MAX+1; ++tlth_i)
  {
    for (dfelth_i=0; dfelth_i<SOC_PETRA_SRD_EYE_SCAN_DFELTH_MAX+1; ++dfelth_i)
    {
      info->entries[tlth_i][dfelth_i] = SOC_PETRA_SRD_EYE_SCAN_INVALID;
    }
  }
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  soc_petra_PETRA_SRD_EYE_SCAN_OPTIMUM_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_SRD_EYE_SCAN_OPTIMUM_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_SRD_EYE_SCAN_OPTIMUM_INFO));
  info->tlth = 0;
  info->dfelth = 0;
  info->tlth_margin = 0;
  info->dfelth_margin = 0;
  info->is_valid = FALSE;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_SRD_TRAFFIC_PARAMS_PRBS_clear(
    SOC_SAND_OUT SOC_PETRA_SRD_TRAFFIC_PARAMS_PRBS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_SRD_TRAFFIC_PARAMS_PRBS));
  info->cnt_src = SOC_PETRA_SRD_NOF_CNT_SRCS;
  info->is_tx_prbs_enabled = 0;
  info->mode = SOC_PETRA_SRD_NOF_PRBS_MODES;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_SRD_TRAFFIC_PARAMS_EXT_clear(
    SOC_SAND_OUT SOC_PETRA_SRD_TRAFFIC_PARAMS_EXT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_SRD_TRAFFIC_PARAMS_EXT));
  info->cnt_src = SOC_PETRA_SRD_NOF_CNT_SRCS;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_SRD_TRAFFIC_PARAMS_clear(
    SOC_SAND_OUT SOC_PETRA_SRD_TRAFFIC_PARAMS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_SRD_TRAFFIC_PARAMS));
  soc_petra_PETRA_SRD_TRAFFIC_PARAMS_PRBS_clear(&(info->prbs));
  soc_petra_PETRA_SRD_TRAFFIC_PARAMS_EXT_clear(&(info->ext));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_SRD_EYE_SCAN_RANGE_clear(
    SOC_SAND_OUT SOC_PETRA_SRD_EYE_SCAN_RANGE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_SRD_EYE_SCAN_RANGE));
  info->tlth_min = 0;
  info->tlth_max = SOC_PETRA_SRD_EYE_SCAN_TLTH_MAX;
  info->dfelth_min = 0;
  info->dfelth_max = SOC_PETRA_SRD_EYE_SCAN_DFELTH_MAX;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  soc_petra_PETRA_SRD_EYE_SCAN_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_SRD_EYE_SCAN_INFO *info
  )
{
  uint32 ind;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_SRD_EYE_SCAN_INFO));

  soc_petra_PETRA_SRD_EYE_SCAN_RANGE_clear(&(info->range));

  info->nof_lane_ndx_entries = 0;
  for (ind=0; ind<SOC_PETRA_SRD_NOF_LANES; ++ind)
  {
    info->lane_ndx[ind] = 0;
  }
  info->duration_min_sec = 1;
  info->resolution = 1;
  info->traffic_src = SOC_PETRA_SRD_NOF_TRAFFIC_SRCS;
  soc_petra_PETRA_SRD_TRAFFIC_PARAMS_clear(&(info->params));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  soc_petra_PETRA_SRD_EYE_SCAN_RES_clear(
    SOC_SAND_OUT SOC_PETRA_SRD_EYE_SCAN_RES *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_SRD_EYE_SCAN_RES));
  soc_petra_PETRA_SRD_EYE_SCAN_MATRIX_clear(&(info->matrix));
  soc_petra_PETRA_SRD_EYE_SCAN_OPTIMUM_INFO_clear(&(info->optimum));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_SRD_REGS_ADDR_clear(
    SOC_SAND_OUT SOC_PETRA_SRD_REGS_ADDR *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_SRD_REGS_ADDR));
  info->element = 0;
  info->offset = 0;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  soc_petra_PETRA_SRD_REGS_FIELD_clear(
    SOC_SAND_OUT SOC_PETRA_SRD_REGS_FIELD *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_SRD_REGS_FIELD));
  soc_petra_PETRA_SRD_REGS_ADDR_clear(&(info->addr));
  info->msb = 0;
  info->lsb = 0;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}



#if SOC_PETRA_DEBUG_IS_LVL1



uint32
  soc_petra_srd_diag_regs_dump(
    SOC_SAND_IN  int                 unit
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;


  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_srd_diag_regs_dump_unsafe(
    unit
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_srd_diag_regs_dump()",0,0);
}

const char*
  soc_petra_PETRA_SRD_ENTITY_TYPE_to_string(
    SOC_SAND_IN SOC_PETRA_SRD_ENTITY_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PETRA_SRD_ENTITY_TYPE_LANE:
    str = "LANE";
  break;

  case SOC_PETRA_SRD_ENTITY_TYPE_CMU:
    str = "CMU";
  break;

  case SOC_PETRA_SRD_ENTITY_TYPE_IPU:
    str = "IPU";
  break;

  case SOC_PETRA_SRD_NOF_ENTITY_TYPE_LANES:
    str = "NOF_ENTITY_TYPE_LANES";
  break;

  default:
    str = " Unknown";
  }
  return str;
}


const char*
  soc_petra_PETRA_SRD_LANE_ELEMENT_to_string(
    SOC_SAND_IN SOC_PETRA_SRD_LANE_ELEMENT enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PETRA_SRD_LANE_ELEMENT_RX:
    str = "RX";
  break;

  case SOC_PETRA_SRD_LANE_ELEMENT_TX:
    str = "TX";
  break;

  case SOC_PETRA_SRD_LANE_ELEMENT_PRBS:
    str = "PRBS";
  break;

  case SOC_PETRA_SRD_LANE_ELEMENT_RX_LOS:
    str = "RX_LOS";
  break;

  case SOC_PETRA_SRD_LANE_ELEMENT_VCDL:
    str = "VCDL";
  break;

  case SOC_PETRA_SRD_LANE_ELEMENT_RX_FE:
    str = "RX_FE";
  break;

  case SOC_PETRA_SRD_LANE_ELEMENT_RX_HS:
    str = "RX_HS";
  break;

  case SOC_PETRA_SRD_LANE_ELEMENT_RX_SM:
    str = "RX_SM";
  break;

  case SOC_PETRA_SRD_LANE_ELEMENT_PREAMP:
    str = "PREAMP";
  break;

  case SOC_PETRA_SRD_LANE_ELEMENT_TX_HS:
    str = "TX_HS";
  break;

  case SOC_PETRA_SRD_LANE_NOF_ELEMENTS:
    str = "LANE_NOF_ELEMENTS";
  break;

  default:
    str = " Unknown";
  }
  return str;
}


const char*
  soc_petra_PETRA_SRD_CMU_ELEMENT_to_string(
    SOC_SAND_IN SOC_PETRA_SRD_CMU_ELEMENT enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PETRA_SRD_CMU_ELEMENT_CONTROL:
    str = "CONTROL";
  break;

  case SOC_PETRA_SRD_CMU_ELEMENT_ARBITRATION:
    str = "ARBITRATION";
  break;

  case SOC_PETRA_SRD_CMU_NOF_ELEMENTS:
    str = "CMU_NOF_ELEMENTS";
  break;

  default:
    str = " Unknown";
  }
  return str;
}


const char*
  soc_petra_PETRA_SRD_IPU_ELEMENT_to_string(
    SOC_SAND_IN SOC_PETRA_SRD_IPU_ELEMENT enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PETRA_SRD_IPU_ELEMENT_CONTROL:
    str = "CONTROL";
  break;

  case SOC_PETRA_SRD_IPU_ELEMENT_ARBITRATION:
    str = "ARBITRATION";
  break;

  case SOC_PETRA_SRD_IPU_NOF_ELEMENTS:
    str = "IPU_NOF_ELEMENTS";
  break;

  default:
    str = " Unknown";
  }
  return str;
}


const char*
  soc_petra_PETRA_SRD_LANE_EQ_MODE_to_string(
    SOC_SAND_IN SOC_PETRA_SRD_LANE_EQ_MODE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PETRA_SRD_LANE_EQ_MODE_OFF:
    str = "OFF";
  break;

  case SOC_PETRA_SRD_LANE_EQ_MODE_BLIND:
    str = "BLIND";
  break;

  case SOC_PETRA_SRD_LANE_EQ_MODE_PRESET:
    str = "PRESET";
  break;

  case SOC_PETRA_SRD_LANE_EQ_MODE_SS:
    str = "SS";
  break;

  case SOC_PETRA_SRD_NOF_LANE_EQ_MODES:
    str = "NOF_LANE_EQ_MODES";
  break;

  default:
    str = " Unknown";
  }
  return str;
}


const char*
  soc_petra_PETRA_SRD_LANE_LOOPBACK_MODE_to_string(
    SOC_SAND_IN SOC_PETRA_SRD_LANE_LOOPBACK_MODE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PETRA_SRD_LANE_LOOPBACK_NONE:
    str = "NONE";
  break;

  case SOC_PETRA_SRD_LANE_LOOPBACK_NSILB:
    str = "NSILB";
  break;

  case SOC_PETRA_SRD_LANE_LOOPBACK_NPILB:
    str = "NPILB";
  break;

  case SOC_PETRA_SRD_LANE_LOOPBACK_IPILB:
    str = "IPILB";
  break;

  case SOC_PETRA_SRD_NOF_LANE_LOOPBACK_MODES:
    str = "NOF_LANE_LOOPBACK_MODES";
  break;

  default:
    str = " Unknown";
  }
  return str;
}


const char*
  soc_petra_PETRA_SRD_PRBS_MODE_to_string(
    SOC_SAND_IN SOC_PETRA_SRD_PRBS_MODE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PETRA_SRD_PRBS_MODE_POLY_7:
    str = "POLY_7";
  break;

  case SOC_PETRA_SRD_PRBS_MODE_POLY_15:
    str = "POLY_15";
  break;

  case SOC_PETRA_SRD_PRBS_MODE_POLY_23:
    str = "POLY_23";
  break;

  case SOC_PETRA_SRD_PRBS_MODE_POLY_31:
    str = "POLY_31";
  break;

  case SOC_PETRA_SRD_PRBS_MODE_POLY_23_SWAP_POLARITY:
    str = "POLY_23_SWAP_POLARITY";
  break;

  case SOC_PETRA_SRD_NOF_PRBS_MODES:
    str = "NOF_PRBS_MODES";
  break;

  default:
    str = " Unknown";
  }
  return str;
}

const char*
  soc_petra_PETRA_SRD_CNT_SRC_to_string(
    SOC_SAND_IN SOC_PETRA_SRD_CNT_SRC enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PETRA_SRD_CNT_SRC_CRC_MAC:
    str = "CRC_MAC";
  break;

  case SOC_PETRA_SRD_CNT_SRC_PRBS:
    str = "PRBS";
  break;

  case SOC_PETRA_SRD_CNT_SRC_FEC_BER:
    str = "FEC_BER";
  break;

  case SOC_PETRA_SRD_CNT_SRC_FEC_CER:
    str = "FEC_CER";
  break;

  default:
    str = " Unknown";
  }
  return str;
}

const char*
  soc_petra_PETRA_SRD_TRAFFIC_SRC_to_string(
    SOC_SAND_IN SOC_PETRA_SRD_TRAFFIC_SRC enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PETRA_SRD_TRAFFIC_SRC_PRBS:
    str = "PRBS";
  break;

  case SOC_PETRA_SRD_TRAFFIC_SRC_EXT:
    str = "External";
  break;

  default:
    str = " Unknown";
  }
  return str;
}

const char*
  soc_petra_PETRA_SRD_PRBS_SIGNAL_STAT_to_string(
    SOC_SAND_IN SOC_PETRA_SRD_PRBS_SIGNAL_STAT enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PETRA_SRD_PRBS_SIGNAL_STAT_LOCKED_NO_LOSS:
    str = "LOCKED_NO_LOSS";
  break;

  case SOC_PETRA_SRD_PRBS_SIGNAL_STAT_LOCKED_AFTER_LOSS:
    str = "LOCKED_AFTER_LOSS";
  break;

  case SOC_PETRA_SRD_PRBS_SIGNAL_STAT_NOT_LOCKED:
    str = "NOT_LOCKED";
  break;

  case SOC_PETRA_SRD_NOF_PRBS_SIGNAL_STATS:
    str = "NOF_PRBS_SIGNAL_STATS";
  break;

  default:
    str = " Unknown";
  }
  return str;
}

void
  soc_petra_PETRA_SRD_PRBS_RX_STATUS_print(
    SOC_SAND_IN SOC_PETRA_SRD_PRBS_RX_STATUS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf(
    "Rx_signal_present %u stat %s err_cnt %d\n\r",
    info->rx_signal_present,
    soc_petra_PETRA_SRD_PRBS_SIGNAL_STAT_to_string(info->prbs_signal_stat),
    info->error_cnt
  );
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}



void
  soc_petra_PETRA_SRD_LANE_STATUS_INFO_print(
    SOC_SAND_IN SOC_PETRA_SRD_LANE_STATUS_INFO *info
  )
{
  uint8
    status_ok = TRUE;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  if (
      (info->rate_divisor == SOC_PETRA_SRD_NOF_RATE_DIVISORS) &&
      (info->rx_power_off == TRUE) &&
      (info->tx_power_off == TRUE)
     )
  {
    soc_sand_os_printf("SerDes Quartet is Powered Down, or mis-configured SerDes\n\r");
    status_ok = FALSE;
  }
  else
  {
  soc_sand_os_printf("rate_divisor                  : %s \n\r", soc_petra_PETRA_SRD_RATE_DIVISOR_to_string(info->rate_divisor));
  soc_sand_os_printf("RX signal loss                : %s (RXLOS = %d)",(SOC_SAND_NUM2BOOL(info->rx_signal_loss) == TRUE?"TRUE ":"FALSE"), info->rx_signal_loss);
  soc_sand_os_printf("%s", (info->rx_signal_loss && !(info->is_in_lpbck))?" ***\n\r":"\n\r");
  soc_sand_os_printf("RX signal not frequency-locked: %s (RXLLK = %d)",(SOC_SAND_NUM2BOOL(info->rx_not_locked) == TRUE?"TRUE ":"FALSE"),info->rx_not_locked);
  soc_sand_os_printf("%s", (info->rx_not_locked)?"  ***\n\r":"\n\r");
  soc_sand_os_printf("RX signal detected            : %s",(SOC_SAND_NUM2BOOL(info->rx_signal_detected == TRUE)?"TRUE":"FALSE"));
  soc_sand_os_printf("%s", (!(info->rx_signal_detected))?"  ***\n\r":"\n\r");
  soc_sand_os_printf("RX sig-det changed            : %s",(SOC_SAND_NUM2BOOL(info->rx_sig_det_changed == TRUE)?"TRUE":"FALSE"));
  soc_sand_os_printf("%s", (info->rx_sig_det_changed)?"  (*)\n\r":"\n\r");
  soc_sand_os_printf("RX reset                      : %s",(SOC_SAND_NUM2BOOL(info->rx_reset == TRUE)?"TRUE":"FALSE"));
  soc_sand_os_printf("%s", (info->rx_reset)?"  ***\n\r":"\n\r");
  soc_sand_os_printf("RX freq_invalid               : %s",(SOC_SAND_NUM2BOOL(info->rx_freq_invalid == TRUE)?"TRUE":"FALSE"));
  soc_sand_os_printf("%s", (info->rx_freq_invalid)?"  ***\n\r":"\n\r");
  soc_sand_os_printf("RX pcs synced                 : %s",(SOC_SAND_NUM2BOOL(info->rx_pcs_synced == TRUE)?"TRUE":"FALSE"));
  soc_sand_os_printf("%s", (info->rx_pcs_synced)?"\n\r":" ***\n\r");
  soc_sand_os_printf("RX code errors detected       : %s",(SOC_SAND_NUM2BOOL(info->rx_code_errors_detected == TRUE)?"TRUE":"FALSE"));
  soc_sand_os_printf("%s", (info->rx_code_errors_detected)?"  ***\n\r":"\n\r");
  soc_sand_os_printf("RX disparity_errors_detected  : %s",(SOC_SAND_NUM2BOOL(info->rx_disparity_errors_detected == TRUE)?"TRUE":"FALSE"));
  soc_sand_os_printf("%s", (info->rx_disparity_errors_detected)?"  ***\n\r":"\n\r");
  soc_sand_os_printf("RX comma alignment position   : %u\n\r",info->rx_comma_allignment);
  soc_sand_os_printf("RX comma realign              : %s",(SOC_SAND_NUM2BOOL(info->rx_comma_reallign == TRUE)?"TRUE":"FALSE"));
  soc_sand_os_printf("%s", (info->rx_comma_reallign)?"  (*)\n\r":"\n\r");
  soc_sand_os_printf("Is in loopback                : %s\n\r",(SOC_SAND_NUM2BOOL(info->is_in_lpbck == TRUE)?"TRUE":"FALSE"));
  }

  soc_sand_os_printf("RX power  off                 : %s",(SOC_SAND_NUM2BOOL(info->rx_power_off == TRUE)?"TRUE":"FALSE"));
  soc_sand_os_printf("%s", (info->rx_power_off)?"  ***\n\r":"\n\r");
  soc_sand_os_printf("TX power  off                 : %s",(SOC_SAND_NUM2BOOL(info->tx_power_off == TRUE)?"TRUE":"FALSE"));
  soc_sand_os_printf("%s", (info->tx_power_off)?"  ***\n\r":"\n\r");

  status_ok =  (status_ok && soc_petra_srd_lane_status_is_ok(info));

  soc_sand_os_printf("\n\rLane Status: %s\n\r", (status_ok == TRUE)?"OK":"**ERROR");

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}



void
  soc_petra_PETRA_SRD_QRTT_STATUS_INFO_print(
    SOC_SAND_IN SOC_PETRA_SRD_QRTT_STATUS_INFO *info
  )
{
  uint8
    status_ok = TRUE;
  uint32 ind=0;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("--CMU STATUS--\n\r");
  soc_sand_os_printf("CMU Powered %s\n\r", info->is_active?"Up":"Down!");
  if (info->is_active)
  {
    soc_sand_os_printf("CMU version: %u\n\r", info->version);
    soc_sand_os_printf("PLL-M                   : %u\n\r", info->pllm);
    soc_sand_os_printf("PLL-N                   : %u\n\r", info->plln);
    soc_sand_os_printf("PLL not frequency-locked (too fast): %s (CMULLK = %d)\n\r",(SOC_SAND_NUM2BOOL(info->pll_not_locked) == TRUE?"TRUE ":"FALSE"),info->pll_not_locked);
    soc_sand_os_printf("PLL not frequency-locked (too slow): %s (VCOSLOW = %d)\n\r",(SOC_SAND_NUM2BOOL(info->pll_not_locked) == TRUE?"TRUE ":"FALSE"),info->pll_vco_not_locked);
    soc_sand_os_printf("\n\r--PER-LANE STATUS--\n\r");
    for (ind=0; ind<SOC_PETRA_SRD_NOF_LANES_PER_QRTT; ++ind)
    {
      soc_sand_os_printf("\n\r--LANE %u--\n\r",ind);
      soc_petra_PETRA_SRD_LANE_STATUS_INFO_print(&(info->lane_status[ind]));
    }
  }

  if (
      (info->is_active == FALSE) ||
      (info->version != 25) ||
      (info->pll_not_locked) ||
      (info->pll_vco_not_locked)
     )
  {
    status_ok = FALSE;
  }

  for (ind = 0; ind < SOC_PETRA_SRD_NOF_LANES_PER_QRTT; ind++)
  {
    status_ok = (status_ok && soc_petra_srd_lane_status_is_ok(&(info->lane_status[ind])));
  }

  soc_sand_os_printf("\n\rQuartet Status: %s\n\r", (status_ok == TRUE)?"OK":"**ERROR");

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}



void
  soc_petra_PETRA_SRD_EYE_SCAN_MATRIX_print(
    SOC_SAND_IN SOC_PETRA_SRD_EYE_SCAN_MATRIX *info,
    SOC_SAND_IN SOC_PETRA_SRD_EYE_SCAN_OPTIMUM_INFO *optimum
  )
{
  uint32
    dfelth_i,
    tlth_i;

  for (dfelth_i = 0; dfelth_i <= SOC_PETRA_SRD_EYE_SCAN_DFELTH_MAX; dfelth_i++)
  {
    for (tlth_i = 0; tlth_i <= SOC_PETRA_SRD_EYE_SCAN_TLTH_MAX; tlth_i++)
    {
      soc_sand_os_printf("%2d ", info->entries[tlth_i][dfelth_i]);
    }

    soc_sand_os_printf("\n\r");
  }
}

void
  soc_petra_PETRA_SRD_EYE_SCAN_RES_print(
    SOC_SAND_IN SOC_PETRA_SRD_EYE_SCAN_RES *info
  )
{
  uint32
    tlth_i,
    legend_ndx;
  const char
    legend[] = {' ','.','o','b','P','D','B','M','W','X'},
    optimum_char = '+',
    error_char = '!',
    invalid_char = '?';
  int32
    dfelth_i,
    first_description_dfelth_ndx = 13,
    first_description_ber_dfelth_ndx = 9;
  const char *description_other[] = {
    "   Description:",
    "   =============",
    "   '+'  Latch",
    "   '?'  No data"};
  const char *description_ber[] = {
    "   '%c' BER <10-9",
    "   '%c' BER ~10-9",
    "   '%c' BER ~10-8",
    "   '%c' BER ~10-7",
    "   '%c' BER ~10-6",
    "   '%c' BER ~10-5",
    "   '%c' BER ~10-4",
    "   '%c' BER ~10-3",
    "   '%c' BER ~10-2",
    "   '%c' BER >10-2"};
  const uint32
    nof_legend_chars = sizeof(legend) / sizeof(char);
  char
    print_char;
  uint32
    tmp;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);


  soc_sand_os_printf("\n\r[DFELTH]--------------------------------------------\n\r");

  for (dfelth_i = SOC_PETRA_SRD_EYE_SCAN_DFELTH_MAX; dfelth_i >= 0; dfelth_i--)
  {
    /* Print Y axis and values */
    if ((dfelth_i % 5) == 0)
    {
      soc_sand_os_printf("%2d-+", dfelth_i);
    }
    else
    {
      soc_sand_os_printf("   |");
    }

    for (tlth_i = 0; tlth_i <= SOC_PETRA_SRD_EYE_SCAN_TLTH_MAX; tlth_i++)
    {
      print_char = error_char;

      /* Print matrix entries */
      if (info->matrix.entries[tlth_i][dfelth_i] == SOC_PETRA_SRD_EYE_SCAN_INVALID)
      {
        print_char = invalid_char;
      }
      else if ((info->optimum.dfelth == (uint8)dfelth_i) && (info->optimum.tlth == (uint8)tlth_i))
      {
        print_char = optimum_char;
      }
      else
      {
        /* set legend_ndx to log 10 of entry in BER */
        tmp = info->matrix.entries[tlth_i][dfelth_i];
        legend_ndx = 0;
        while (tmp > 0)
        {
          tmp /= 10;
          legend_ndx++;
        }

        /* If exceeds legend size, round legend_ndx down */
        if (legend_ndx >= nof_legend_chars)
        {
          legend_ndx = (nof_legend_chars - 1);
        }

        print_char = legend[legend_ndx];
      }

      soc_sand_os_printf("%c", print_char);
    }

    soc_sand_os_printf("|");

    /* print description on relevant lines */
    if (first_description_dfelth_ndx >= dfelth_i)
    {
      if (first_description_ber_dfelth_ndx < dfelth_i)
      {
        soc_sand_os_printf(
          description_other[first_description_dfelth_ndx - dfelth_i]);
      }
      else
      {
        soc_sand_os_printf(
          description_ber[first_description_ber_dfelth_ndx - dfelth_i],
          legend[first_description_ber_dfelth_ndx - dfelth_i]);
      }
    }

    soc_sand_os_printf("\n\r");
  }

  /* print x axis */
  soc_sand_os_printf("   -");

  for (tlth_i = 0; tlth_i < SOC_PETRA_SRD_EYE_SCAN_TLTH_MAX - 3; tlth_i++)
  {
    soc_sand_os_printf("%c", ((tlth_i % 5) ? '-' : '+'));
  }

  soc_sand_os_printf("[TLTH]\n\r");
  soc_sand_os_printf("    ");

  for (tlth_i = 0; tlth_i <= SOC_PETRA_SRD_EYE_SCAN_TLTH_MAX; tlth_i++)
  {
    soc_sand_os_printf("%c", ((tlth_i % 5) ? ' ' : '|'));
  }

  soc_sand_os_printf("\n\r");
  soc_sand_os_printf("    ");

  for (tlth_i = 0; tlth_i <= SOC_PETRA_SRD_EYE_SCAN_TLTH_MAX; tlth_i++)
  {
    if (tlth_i % 5)
    {
      soc_sand_os_printf(" ");
    }
    else
    {
      soc_sand_os_printf("%2d", (tlth_i++));
    }
  }

  soc_sand_os_printf("\n\r");
  soc_petra_PETRA_SRD_EYE_SCAN_OPTIMUM_INFO_print(&(info->optimum));
  soc_sand_os_printf("\n\r");

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}



void
  soc_petra_PETRA_SRD_EYE_SCAN_OPTIMUM_INFO_print(
    SOC_SAND_IN SOC_PETRA_SRD_EYE_SCAN_OPTIMUM_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("valid: %u\n\r", info->is_valid);
  soc_sand_os_printf("tlth: %u\n\r", info->tlth);
  soc_sand_os_printf("dfelth: %u\n\r", info->dfelth);
  soc_sand_os_printf("tlth_margin: %u\n\r", info->tlth_margin);
  soc_sand_os_printf("dfelth_margin: %u\n\r", info->dfelth_margin);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}



void
  soc_petra_PETRA_SRD_EYE_SCAN_INFO_print(
    SOC_SAND_IN SOC_PETRA_SRD_EYE_SCAN_INFO *info
  )
{
  uint32 ind;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("nof_lane_ndx_entries: %u\n\r",info->nof_lane_ndx_entries);
  soc_sand_os_printf("lane_ndx: ");
  for (ind=0; ind<info->nof_lane_ndx_entries; ++ind)
  {
    soc_sand_os_printf("%d ", info->lane_ndx[ind]);
  }
  soc_sand_os_printf("\n\r");
  soc_sand_os_printf("Prbs_duration_min_sec:  %u\n\r",info->duration_min_sec);
  soc_sand_os_printf("Resolution:             %u\n\r",info->resolution);

  if (info->traffic_src == SOC_PETRA_SRD_TRAFFIC_SRC_PRBS)
  {
    soc_sand_os_printf("Prbs_mode: %s\n\r",soc_petra_PETRA_SRD_PRBS_MODE_to_string(info->params.prbs.mode));
    soc_sand_os_printf("Is_tx_prbs_enabled: %u\n\r", info->params.prbs.is_tx_prbs_enabled);
    soc_sand_os_printf("Cnt_src: %s\n\r",soc_petra_PETRA_SRD_CNT_SRC_to_string(info->params.prbs.cnt_src));
  }
  else
  {
    soc_sand_os_printf("Cnt_src: %s\n\r",soc_petra_PETRA_SRD_CNT_SRC_to_string(info->params.ext.cnt_src));
  }

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif /* SOC_PETRA_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

