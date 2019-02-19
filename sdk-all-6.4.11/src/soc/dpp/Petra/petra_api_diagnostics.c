/* $Id: petra_api_diagnostics.c,v 1.9 Broadcom SDK $
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

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>
#include <soc/dpp/SAND/Management/sand_low_level.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Management/sand_general_params.h>
#include <soc/dpp/SAND/Management/sand_callback_handles.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_device_management.h>

#include <soc/dpp/SAND/SAND_FM/sand_trigger.h>
#include <soc/dpp/SAND/SAND_FM/sand_mem_access.h>

#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Utils/sand_bitstream.h>
#include <soc/dpp/SAND/Utils/sand_framework.h>

#include <soc/dpp/Petra/petra_api_diagnostics.h>
#include <soc/dpp/Petra/petra_api_framework.h>
#include <soc/dpp/Petra/petra_api_general.h>
#include <soc/dpp/Petra/petra_sw_db.h>
#include <soc/dpp/Petra/petra_framework.h>
#include <soc/dpp/Petra/petra_ports.h>
#include <soc/dpp/Petra/petra_ingress_packet_queuing.h>

#include <soc/dpp/Petra/petra_api_ingress_packet_queuing.h>
#include <soc/dpp/Petra/petra_diagnostics.h>
#include <soc/dpp/Petra/petra_chip_defines.h>
#include <soc/dpp/Petra/petra_chip_regs.h>
#include <soc/dpp/Petra/petra_chip_tbls.h>
#include <soc/dpp/Petra/petra_reg_access.h>
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

/*****************************************************
 * See details in soc_petra_api_diagnostics.h
 *****************************************************/
uint32
  soc_petra_diag_ipt_rate_get(
    SOC_SAND_IN  int                unit,
    SOC_SAND_OUT SOC_SAND_64CNT               *rate
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_diag_ipt_rate_get_unsafe(
          unit,
          rate
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_diag_ipt_rate_get_unsafe()",0,0);
}

uint32
  soc_petra_diag_iddr_set(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  uint32    dram_ndx,
    SOC_SAND_IN  uint32    bank_ndx,
    SOC_SAND_IN  uint32     dram_offset,
    SOC_SAND_IN  uint32     *data,
    SOC_SAND_IN  uint32     size
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DIAG_IDDR_SET);

  SOC_SAND_CHECK_NULL_INPUT(data);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_diag_iddr_set_unsafe(
          unit,
          dram_offset,
          dram_ndx,
          bank_ndx,
          data,
          size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_diag_iddr_set()",0,0);
}

uint32
  soc_petra_diag_iddr_get(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  uint32    dram_ndx,
    SOC_SAND_IN  uint32    bank_ndx,
    SOC_SAND_IN  uint32     dram_offset,
    SOC_SAND_OUT uint32     *data,
    SOC_SAND_IN  uint32     size
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DIAG_IDDR_GET);

  SOC_SAND_CHECK_NULL_INPUT(data);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_diag_iddr_get_unsafe(
          unit,
          dram_offset,
          dram_ndx,
          bank_ndx,
          data,
          size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_diag_iddr_get()",0,0);
}

void
  soc_petra_PETRA_DIAG_IN_PKT_HDR_DATA_clear(
    SOC_SAND_OUT SOC_PETRA_DIAG_ING_PKT_HDR_DATA *info
  )
{

}

void
  soc_petra_PETRA_DIAG_PACKET_WALKTROUGH_clear(
    SOC_SAND_OUT SOC_PETRA_DIAG_PACKET_WALKTROUGH *info
  )
{
  sal_memset(info, 0x0, sizeof(SOC_PETRA_DIAG_PACKET_WALKTROUGH));
  info->dst_fap_id = FALSE;
  info->dst_prt_id = SOC_PETRA_NOF_FAP_PORTS;
  info->flow_id = SOC_PETRA_NOF_FAPS_IN_SYSTEM;
  info->flow_qrtt = 0;
  info->is_composite = FALSE;
  info->queue_id = 0;
  info->queue_qrtt = 0;
  info->sys_phy_port = 0;
  info->valid = TRUE;

  SOC_SAND_MAGIC_NUM_SET;
}

uint32
  soc_petra_diag_packet_walktrough_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                       dest_id,
    SOC_SAND_IN  uint32                      class_id,
    SOC_SAND_OUT SOC_PETRA_DIAG_PACKET_WALKTROUGH  *packet_walktrough
  )
{
  uint32
    res = SOC_SAND_OK;
  uint8
    valid,
    sw_only;
  uint32
    dest_dev_id,
    dest_port_id;
  SOC_PETRA_SCH_QUARTET_MAPPING_INFO
    sch_quartet_mapping_info;
  SOC_PETRA_IPQ_QUARTET_MAP_INFO
    ipq_quartet_map_info;
  uint32
    system_physical_port,
    flow_id;
  uint32
    queue_ndx;
  int 
    core = SOC_CORE_ALL;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DIAG_PACKET_WALKTROUGH_GET);

  soc_petra_PETRA_IPQ_QUARTET_MAP_INFO_clear(&ipq_quartet_map_info);
  soc_petra_PETRA_SCH_QUARTET_MAPPING_INFO_clear(&sch_quartet_mapping_info);

  res = soc_petra_ipq_destination_id_packets_base_queue_id_get(
          unit,
          core,
          dest_id,
          &valid,
          &sw_only,
          &queue_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (valid == FALSE)
  {
    packet_walktrough->valid = FALSE;
    goto exit;
  }

  res = soc_petra_ipq_queue_to_flow_mapping_get(
          unit,
          SOC_CORE_ALL,
          SOC_SAND_DIV_ROUND_DOWN(class_id + queue_ndx, 4),
          &ipq_quartet_map_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = soc_petra_sys_phys_to_local_port_map_get(
          unit,
          ipq_quartet_map_info.system_physical_port,
          &dest_dev_id,
          &dest_port_id
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_ipq_attached_flow_port_get(
          unit,
          SOC_CORE_ALL,
          queue_ndx + class_id,
          &flow_id,
          &system_physical_port
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  packet_walktrough->queue_qrtt = SOC_SAND_DIV_ROUND_DOWN(class_id + queue_ndx, 4);
  packet_walktrough->queue_id = queue_ndx + class_id;
  packet_walktrough->flow_qrtt = ipq_quartet_map_info.flow_quartet_index;
  packet_walktrough->flow_id = flow_id;
  packet_walktrough->is_composite = ipq_quartet_map_info.is_composite;
  packet_walktrough->sys_phy_port = ipq_quartet_map_info.system_physical_port;
  packet_walktrough->dst_fap_id = dest_dev_id;
  packet_walktrough->dst_prt_id = dest_port_id;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_diag_packet_walktrough_get()",0,0);
}



/*********************************************************************
*     Read the last packet header entered the NIF and return
*     the values of the ingress ITMH header fields.
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_petra_nif_diag_last_packet_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_PORTS_ITMH          *last_incoming_header
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_DIAG_LAST_PACKET_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(last_incoming_header);

  /*
   *	Invalid for Soc_petra-B
   */
  SOC_PETRA_API_INVALID_FOR_PETRA_B(5, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_nif_diag_last_packet_get_unsafe(
          unit,
          last_incoming_header
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_diag_last_packet_get()",0,0);
}
/*********************************************************************
*     Write a pattern of 256 bits long into the DRAM via the
*     BIST mechanism.
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_petra_diag_dram_write(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                     dram_ndx,
    SOC_SAND_IN  SOC_PETRA_DIAG_DRAM_ACCESS_INFO *info,
    SOC_SAND_IN  uint32                    *buffer
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DIAG_BIST_WRITE);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(buffer);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_diag_dram_write_unsafe(
          unit,
          dram_ndx,
          info,
          buffer
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_diag_dram_write()",0,0);
}

/*********************************************************************
*     Indicates if a pattern is written in the specified
*     addresses by reading and comparing the written pattern
*     with the expected data via the BIST mechanism.
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_petra_diag_dram_read_and_compare(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  uint32                             dram_ndx,
    SOC_SAND_IN  SOC_PETRA_DIAG_DRAM_ACCESS_INFO         *info,
    SOC_SAND_IN  uint32                            *buff_expected,
    SOC_SAND_OUT SOC_PETRA_DIAG_DRAM_READ_COMPARE_STATUS *status
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DIAG_BIST_READ_AND_COMPARE);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(buff_expected);
  SOC_SAND_CHECK_NULL_INPUT(status);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_diag_dram_read_and_compare_unsafe(
          unit,
          dram_ndx,
          info,
          buff_expected,
          status
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_diag_dram_read_and_compare()",0,0);
}

/*********************************************************************
*     Run BIST test with the given parameters.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_diag_qdr_BIST_test_start(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_DIAG_QDR_BIST_TEST_RUN_INFO *test_info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DIAG_QDR_BIST_TEST_START);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(test_info);

  res = soc_petra_diag_qdr_BIST_test_start_verify(
          unit,
          test_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_diag_qdr_BIST_test_start_unsafe(
    unit,
    test_info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_diag_qdr_BIST_test_start()",0,0);
}

/*********************************************************************
*     Stop BIST test and return the result.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_diag_qdr_BIST_test_result_get(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_OUT SOC_PETRA_DIAG_QDR_BIST_TEST_RES_INFO  *res_info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DIAG_QDR_BIST_TEST_RESULT_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(res_info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_diag_qdr_BIST_test_result_get_unsafe(
          unit,
          res_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_diag_qdr_BIST_test_stop()",0,0);
}

/*********************************************************************
*     Run DRAM BIST test with the given parameters.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_diag_dram_BIST_test_start(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  dram_ndx,
    SOC_SAND_IN  SOC_PETRA_DIAG_DRAM_BIST_TEST_RUN_INFO *test_info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DIAG_DRAM_BIST_TEST_START);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(test_info);

  res = soc_petra_diag_dram_BIST_test_start_verify(
          unit,
          dram_ndx,
          test_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_diag_dram_BIST_test_start_unsafe(
    unit,
    dram_ndx,
    test_info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_diag_dram_BIST_test_start()",0,0);
}

/*********************************************************************
*     Stop DRAM BIST test and return the result.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_diag_dram_BIST_test_result_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 dram_ndx,
    SOC_SAND_OUT SOC_PETRA_DIAG_DRAM_BIST_TEST_RES_INFO *res_info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DIAG_DRAM_BIST_TEST_RESULT_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(res_info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_diag_dram_BIST_test_result_get_unsafe(
    unit,
    dram_ndx,
    res_info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_diag_dram_BIST_test_result_get()",0,0);
}

/*********************************************************************
*     This function is used as a diagnostics tool that gives
*     indications about the dll status.
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_petra_diag_dll_status_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 dram_ndx,
    SOC_SAND_IN  uint32                 dll_ndx,
    SOC_SAND_OUT SOC_PETRA_DIAG_DLL_STATUS_INFO *dll_stt_info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DIAG_DLL_STATUS_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(dll_stt_info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_diag_dll_status_get_unsafe(
          unit,
          dram_ndx,
          dll_ndx,
          dll_stt_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_diag_dll_status_get()",0,0);
}

/*********************************************************************
*     Get DRAM diagnostic.
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_petra_diag_dram_status_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 dram_ndx,
    SOC_SAND_OUT SOC_PETRA_DIAG_DRAM_STATUS_INFO *dram_status
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DIAG_DRAM_STATUS_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(dram_status);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_diag_dram_status_get_unsafe(
          unit,
          dram_ndx,
          dram_status
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_diag_dram_status_get()",0,0);
}

/*********************************************************************
*     Get diagnostic for the DRAM interface training sequence.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_diag_dram_diagnostic_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 dram_ndx,
    SOC_SAND_OUT SOC_PETRA_DIAG_DRAM_ERR_INFO                  *error_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DIAG_DRAM_DIAGNOSTIC_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(error_info);

  res = soc_petra_diag_dram_diagnostic_get_verify(
          unit,
          dram_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_diag_dram_diagnostic_get_unsafe(
          unit,
          dram_ndx,
          error_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_diag_dram_diagnostic_get()",0,0);
}

/*********************************************************************
*     A diagnostic tool that determines the QDR window
 *     validity by using a series of BIST commands.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_diag_qdr_window_validity_get(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PETRA_DIAG_QDR_PATTERN              *pattern,
    SOC_SAND_OUT SOC_PETRA_DIAG_QDR_WINDOW_INFO          *window_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(pattern);
  SOC_SAND_CHECK_NULL_INPUT(window_info);

  res = soc_petra_diag_qdr_window_validity_get_verify(
          unit,
          pattern
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_diag_qdr_window_validity_get_unsafe(
          unit,
          pattern,
          window_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PETRA_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_diag_qdr_window_validity_get()", 0, 0);
}

uint32
  soc_petra_diag_lbg_conf_set(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PETRA_DIAG_LBG_INFO                *lbg_info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DIAG_LBG_CONF_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  SOC_SAND_CHECK_NULL_INPUT(lbg_info);

  res = soc_petra_diag_lbg_conf_set_unsafe(
          unit,
          lbg_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_diag_lbg_conf_set()",0,0);
}

uint32
  soc_petra_diag_lbg_traffic_send(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PETRA_DIAG_LBG_TRAFFIC_INFO        *traffic_info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DIAG_LBG_TRAFFIC_SEND);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;


  SOC_SAND_CHECK_NULL_INPUT(traffic_info);

  res = soc_petra_diag_lbg_traffic_send_unsafe(
          unit,
          traffic_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_diag_lbg_traffic_send()",0,0);
}

uint32
  soc_petra_diag_lbg_result_get(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_OUT  SOC_PETRA_DIAG_LBG_RESULT_INFO        *lbg_info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DIAG_LBG_RESULT_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  SOC_SAND_CHECK_NULL_INPUT(lbg_info);

  res = soc_petra_diag_lbg_result_get_unsafe(
          unit,
          lbg_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_diag_lbg_result_get()",0,0);
}

uint32
  soc_petra_diag_lbg_close(
    SOC_SAND_IN  int                          unit
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DIAG_LBG_CLOSE);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_diag_lbg_close_unsafe(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_diag_lbg_close()",0,0);
}

/* $Id: petra_api_diagnostics.c,v 1.9 Broadcom SDK $
 *  Run MBIST on a specified SMS.
 *  To run on all memories, use SOC_PETRA_DIAG_SMS_ALL as the sms value.
 *  SMS index memory to write the pattern in. 
 *  sms is in range: 1 -  29 (SOC_PETRA_DIAG_SMS_ALL).
 */
uint32
  soc_petra_diag_mbist_run(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  uint32     sms,
    SOC_SAND_OUT SOC_PETRA_DIAG_SOFT_SMS_RESULT   *sms_result[SOC_PETRA_DIAG_SMS_MAX]
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DIAG_MBIST_RUN);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(sms_result);
 
  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_diag_mbist_run_unsafe(
          unit,
          sms,
          sms_result
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PETRA_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_diag_mbist_run()", 0, 0);
}

/*********************************************************************
*     Start the soft-error test analyzing the memory validity.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_diag_soft_error_test_start(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PETRA_DIAG_SOFT_ERR_INFO            *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DIAG_SOFT_ERROR_TEST_START);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_petra_diag_soft_error_test_start_verify(
          unit,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_diag_soft_error_test_start_unsafe(
          unit,
          info,
          FALSE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PETRA_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_diag_soft_error_test_start()", 0, 0);
}

/*********************************************************************
*     Start the soft-error test analyzing the memory validity.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_diag_soft_error_test_result_get(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PETRA_DIAG_SOFT_ERR_INFO            *info,
    SOC_SAND_IN  SOC_PETRA_DIAG_SOFT_COUNT_TYPE          count_type,
    SOC_SAND_OUT SOC_PETRA_DIAG_SOFT_SMS_RESULT          *sms_result[SOC_PETRA_DIAG_SMS_MAX][SOC_PETRA_DIAG_NOF_SONE_SAVED_PER_SMS_MAX],
    SOC_SAND_OUT SOC_PETRA_DIAG_SOFT_ERR_RESULT          *glbl_result
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DIAG_SOFT_ERROR_TEST_RESULT_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(*sms_result);
  SOC_SAND_CHECK_NULL_INPUT(glbl_result);

  res = soc_petra_diag_soft_error_test_result_get_verify(
          unit,
          info,
          count_type
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_diag_soft_error_test_result_get_unsafe(
          unit,
          info,
          count_type,
          sms_result,
          glbl_result
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PETRA_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_diag_soft_error_test_result_get()", 0, 0);
}
/*********************************************************************
*     This function is used to describe what is the recommended recovery action
*     that should be taken, when a given interrupt was indicated.
*     Note: this function only guidelines the user for the recommended action
*     without running any specific action.
*     Details: in the H file. (search for prototype)
*********************************************************************/
void
  soc_petra_diag_interrupt_to_recovery_action(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PETRA_INTERRUPT_CAUSE              cause,
    SOC_SAND_OUT SOC_PETRA_INT_RECOVERY_ACTION          *action
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(action);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;
    
  switch (cause)
  {
    case SOC_PETRA_IDR_ERROR_UNICAST_RECYCLE:
    case SOC_PETRA_IDR_ERROR_MINI_MULTICAST_RECYCLE:
    case SOC_PETRA_IDR_ERROR_FULL_MULTICAST_RECYCLE:
    case SOC_PETRA_IDR_ERROR_CHUNK_STATUS_ECC:
    case SOC_PETRA_IDR_ERROR_CHUNK_FIFO_ECC:
    case SOC_PETRA_IDR_ERROR_UNICAST_DP_ECC:
    case SOC_PETRA_IDR_ERROR_MINI_MULTICAST_DP_ECC:
    case SOC_PETRA_IDR_ERROR_FULL_MULTICAST_DP_ECC:
    case SOC_PETRA_IDR_ERROR_FREE_PCBS_ECC:
    case SOC_PETRA_IDR_ERROR_PCB_LINK_TABLE_ECC:
    case SOC_PETRA_IDR_ERROR_PRM_DP_ECC:
    case SOC_PETRA_IDR_ERROR_PRM_PD_ECC:
    case SOC_PETRA_IDR_ERROR_PRM_CD_ECC:
    case SOC_PETRA_IDR_ERROR_REASSEMBLY_STATUS_ECC:
    case SOC_PETRA_IDR_ERROR_WORD_INDEX_ECC:
    case SOC_PETRA_IDR_ERROR_BUFFER_TYPE_ECC:
    case SOC_PETRA_IDR_ERROR_COMPLETE_PC_ECC:
    case SOC_PETRA_IRR_ERROR_FREE_PCP_ECC:
    case SOC_PETRA_IRR_ERROR_IS_FREE_PCP_ECC :
    case SOC_PETRA_IRR_ERROR_PCP_LINK_TABLE_ECC:
    case SOC_PETRA_IRR_ERROR_IS_PCP_LINK_TABLE_ECC:
    case SOC_PETRA_IRR_ERROR_PCM_ECC:
    case SOC_PETRA_IRR_ERROR_IS_PCM_ECC:
    case SOC_PETRA_IRR_ERROR_UC_ECC:
    case SOC_PETRA_IRR_ERROR_MC_ECC:
    case SOC_PETRA_IRR_ERROR_NIF_ECC:
    case SOC_PETRA_IRR_ERROR_GLAG_MAPPING_ECC:
    case SOC_PETRA_IRR_ERROR_ISF_ECC:
    case SOC_PETRA_IQM_FREE_BDB_OVF:
    case SOC_PETRA_IQM_FULL_USCNT_OVF:
    case SOC_PETRA_IQM_MINI_USCNT_OVF:
    case SOC_PETRA_IQM_UPDT_FIFO_OVF:
    case SOC_PETRA_IQM_TXDSCR_FIFO_OVF:
    case SOC_PETRA_IQM_PD_FIFO_OVF:
    case SOC_PETRA_IQM_QDR_SFT_ERR:
    case SOC_PETRA_IQM_QROLL_OVER:
    case SOC_PETRA_IQM_VSQ_ROLL_OVER:
    case SOC_PETRA_IQM_ECC_INTR_ERR:
    case SOC_PETRA_IQM_TAIL_DSCR_SFT_ERR:
    case SOC_PETRA_IQM_BDBLL_SFT_ERR:
    case SOC_PETRA_IQM_FLUSCNT_SFT_ERR:
    case SOC_PETRA_IQM_MNUSCNT_SFT_ERR:
    case SOC_PETRA_IQM_TXPDM_SFT_ERR:
    case SOC_PETRA_IQM_TXDSCRM_SFT_ERR:
    case SOC_PETRA_IQM_DBFFM_SFT_ERR:
    case SOC_PETRA_IQM_DELFFM_SFT_ERR:
    case SOC_PETRA_IQM_VSQB_QSZ_SFT_ERR:
    case SOC_PETRA_IQM_VSQC_QSZ_SFT_ERR:
    case SOC_PETRA_IQM_VSQD_QSZ_SFT_ERR:
    case SOC_PETRA_IPS_EMPTY_DQCQ_WRITE:
    case SOC_PETRA_IPS_QDESC_ECC_ERR:
    case SOC_PETRA_IPS_DQCQMEM_ECC_ERR:
    case SOC_PETRA_IPS_FSMRQMEM_ECC_ERR:
    case SOC_PETRA_IPT_SOP_ECC_ERROR:
    case SOC_PETRA_IPT_MOP_ECC_ERROR:
    case SOC_PETRA_IPT_FDTC_ECC_ERROR:
    case SOC_PETRA_IPT_EGQC_ECC_ERROR:
    case SOC_PETRA_IPT_BDQ_ECC_ERROR:
    case SOC_PETRA_IPT_PDQ_ECC_ERROR:
    case SOC_PETRA_MMU_ERROR_ECC:
    case SOC_PETRA_MMU_WAFAA_ECC_ERR:
    case SOC_PETRA_MMU_WAFAB_ECC_ERR:
    case SOC_PETRA_MMU_WAFAC_ECC_ERR:
    case SOC_PETRA_MMU_WAFAD_ECC_ERR:
    case SOC_PETRA_MMU_WAFAE_ECC_ERR:
    case SOC_PETRA_MMU_WAFAF_ECC_ERR:
    case SOC_PETRA_MMU_WAFBA_ECC_ERR:
    case SOC_PETRA_MMU_WAFBB_ECC_ERR:
    case SOC_PETRA_MMU_WAFBC_ECC_ERR:
    case SOC_PETRA_MMU_WAFBD_ECC_ERR:
    case SOC_PETRA_MMU_WAFBE_ECC_ERR:
    case SOC_PETRA_MMU_WAFBF_ECC_ERR:
    case SOC_PETRA_MMU_RAFA_ECC_ERR:
    case SOC_PETRA_MMU_RAFB_ECC_ERR:
    case SOC_PETRA_MMU_RAFC_ECC_ERR:
    case SOC_PETRA_MMU_RAFD_ECC_ERR:
    case SOC_PETRA_MMU_RAFE_ECC_ERR:
    case SOC_PETRA_MMU_RAFF_ECC_ERR:
    case SOC_PETRA_MMU_WFAFA_ECC_ERR:
    case SOC_PETRA_MMU_WFAFB_ECC_ERR:
    case SOC_PETRA_MMU_WFAFC_ECC_ERR:
    case SOC_PETRA_MMU_WFAFD_ECC_ERR:
    case SOC_PETRA_MMU_WFAFE_ECC_ERR:
    case SOC_PETRA_MMU_WFAFF_ECC_ERR:
    case SOC_PETRA_MMU_RFAFA_ECC_ERR:
    case SOC_PETRA_MMU_RFAFB_ECC_ERR:
    case SOC_PETRA_MMU_RFAFC_ECC_ERR:
    case SOC_PETRA_MMU_RFAFD_ECC_ERR:
    case SOC_PETRA_MMU_RFAFE_ECC_ERR:
    case SOC_PETRA_MMU_RFAFF_ECC_ERR:
    case SOC_PETRA_MMU_IDF_ECC_ERR:
    case SOC_PETRA_MMU_FDF_ECC_ERR:
      *action = SOC_PETRA_INT_RECOVERY_ACTION_QUICK_RECOVERY;
      break;
    case SOC_PETRA_FDR_FDRA_TAG_ECC_ERR:
    case SOC_PETRA_FDR_FDRB_TAG_ECC_ERR:
    case SOC_PETRA_FDR_FDR_OUT_TAG_ECC_ERR:
    case SOC_PETRA_EGQ_PDM_PAR_ERR:
    case SOC_PETRA_EGQ_PLM_PAR_ERR:
    case SOC_PETRA_EGQ_BUFLINK_ECC_ERR:
    case SOC_PETRA_EGQ_BUFLINK_ECC_FIX:
    case SOC_PETRA_EGQ_RCNT_ECC_ERR:
    case SOC_PETRA_EGQ_RCNT_ECC_FIX:
    case SOC_PETRA_EGQ_RRDM_ECC_ERR:
    case SOC_PETRA_EGQ_RRDM_ECC_FIX:
    case SOC_PETRA_EGQ_RPDM_ECC_ERR:
    case SOC_PETRA_EGQ_RPDM_ECC_FIX:
    case SOC_PETRA_EPN_PRDM_ECC_ERR:
    case SOC_PETRA_EPN_PRDM_ECC_FIX:
    case SOC_PETRA_EPN_MDM_ECC_ERR:
    case SOC_PETRA_EPN_MDM_ECC_FIX:
    case SOC_PETRA_EPN_NADM_ECC_ERR:
    case SOC_PETRA_EPN_NADM_ECC_FIX:
    case SOC_PETRA_EPN_NBDM_ECC_ERR:
    case SOC_PETRA_EPN_NBDM_ECC_FIX:
    case SOC_PETRA_SCH_SMPTHROW_SCL_MSG:
    case SOC_PETRA_SCH_DHDECC:
    case SOC_PETRA_SCH_DCDECC:
    case SOC_PETRA_SCH_FLHHRECC:
    case SOC_PETRA_SCH_FLTHRECC:
    case SOC_PETRA_SCH_FLHCLECC:
    case SOC_PETRA_SCH_FLTCLECC:
    case SOC_PETRA_SCH_FLHFQECC:
    case SOC_PETRA_SCH_FLTFQECC:
    case SOC_PETRA_SCH_FDMSECC:
    case SOC_PETRA_SCH_FDMDECC:
    case SOC_PETRA_SCH_SHDSECC:
    case SOC_PETRA_SCH_SHDDECC:
    case SOC_PETRA_SCH_FQMECC:
    case SOC_PETRA_SCH_SFLHECC:
    case SOC_PETRA_SCH_SFLTECC:
    case SOC_PETRA_SCH_FSMECC:
      *action = SOC_PETRA_INT_RECOVERY_ACTION_FULL_DEVICE_RESET;
      break;
    default:
      *action = SOC_PETRA_INT_RECOVERY_ACTION_UNKNOWN_ACTION;
      break;
  }

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_DIAG_QDR_BIST_TEST_RUN_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_DIAG_QDR_BIST_TEST_RUN_INFO *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_DIAG_QDR_BIST_TEST_RUN_INFO));
  info->nof_cmnds_write = 0;
  info->nof_cmnds_read = 0;
  info->start_addr = 0;
  info->end_addr = 0;
  info->read_offset = 0;
  info->data_mode = SOC_PETRA_DIAG_NOF_QDR_BIST_DATA_MODES;
  info->address_mode = SOC_PETRA_DIAG_NOF_QDR_BIST_ADDR_MODES;
  info->data_pattern = SOC_PETRA_DIAG_NOF_BIST_DATA_PATTERNS;
  for (ind = 0; ind < SOC_PETRA_DIAG_QDR_BIST_PATTERN_SIZE_IN_UINT32S; ++ind)
  {
    info->user_defined_pattern[ind] = 0;
  }
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_DIAG_QDR_BIST_TEST_RES_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_DIAG_QDR_BIST_TEST_RES_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_DIAG_QDR_BIST_TEST_RES_INFO));
  info->is_test_finished = 0;
  info->is_qdr_up = 0;
  info->bit_err_counter = 0;
  info->reply_err_counter = 0;
  info->bits_error_bitmap = 0;
  info->last_addr_err = 0;
  info->last_data_err = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_DIAG_QDR_PATTERN_clear(
    SOC_SAND_OUT SOC_PETRA_DIAG_QDR_PATTERN *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_DIAG_QDR_PATTERN));
  info->mode = SOC_PETRA_DIAG_NOF_QDR_PATTERN_MODES;
  for (ind = 0; ind < SOC_PETRA_DIAG_QDR_BIST_PATTERN_SIZE_IN_UINT32S; ++ind)
  {
    info->data[ind] = 0;
  }
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_DIAG_QDR_WINDOW_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_DIAG_QDR_WINDOW_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_DIAG_QDR_WINDOW_INFO));
  info->start = 0;
  info->end = 0;
  info->sampling_point = 0;
  info->total_size = 0;
  info->ratio_percent = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_DIAG_DRAM_BIST_TEST_RUN_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_DIAG_DRAM_BIST_TEST_RUN_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_DIAG_DRAM_BIST_TEST_RUN_INFO));
  info->is_finite_nof_cmnds = 0;
  info->writes_per_cycle = 0;
  info->reads_per_cycle = 0;
  info->start_addr = 0;
  info->end_addr = 0;
  info->data_mode = SOC_PETRA_DIAG_NOF_DRAM_BIST_DATA_MODES;
  info->data_pattern = SOC_PETRA_DIAG_NOF_BIST_DATA_PATTERNS;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  soc_petra_PETRA_DIAG_DRAM_BIST_TEST_RES_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_DIAG_DRAM_BIST_TEST_RES_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_DIAG_DRAM_BIST_TEST_RES_INFO));
  info->reply_err_counter = 0;
  info->is_test_finished = 0;
  info->is_dram_up = FALSE;

  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  soc_petra_PETRA_DIAG_DRAM_ACCESS_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_DIAG_DRAM_ACCESS_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_DIAG_DRAM_ACCESS_INFO));
  info->address = 0;
  info->is_data_size_bits_256_not_32 = 0;
  info->is_infinite_nof_actions = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  soc_petra_PETRA_DIAG_DRAM_READ_COMPARE_STATUS_clear(
    SOC_SAND_OUT SOC_PETRA_DIAG_DRAM_READ_COMPARE_STATUS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_DIAG_DRAM_READ_COMPARE_STATUS));
  info->success = 0;
  info->error_bits_global = 0;
  info->nof_addr_with_errors = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  soc_petra_PETRA_DIAG_DLL_STATUS_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_DIAG_DLL_STATUS_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_DIAG_DLL_STATUS_INFO));
  info->ddl_control_0 = 0;
  info->ddl_control_1 = 0;
  info->ddl_control_2 = 0;
  info->rnd_trp = 0;
  info->rnd_trp_diff = 0;
  info->dll_init_done = 0;
  info->dll_ph_dn = 0;
  info->dll_ph_up = 0;
  info->main_ph_sel = 0;
  info->ph2sel = 0;
  info->hc_sel_vec = 0;
  info->qc_sel_vec = 0;
  info->sel_vec = 0;
  info->sel_hg = 0;
  info->ph_sel_hc_up = 0;
  info->ins_dly_min_vec = 0;
  info->ddl_init_main_ph_sel_ofst = 0;
  info->ddl_ph_sel_hc_up = 0;
  info->ddl_train_trig_up_limit = 0;
  info->ddl_train_trig_dn_limit = 0;
  info->ph_sel_err = 0;
  info->dly_max_min_mode = 0;
  info->ph_sel = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  soc_petra_PETRA_DIAG_DRAM_STATUS_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_DIAG_DRAM_STATUS_INFO *info
  )
{
  uint32 ind;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_DIAG_DRAM_STATUS_INFO));
  info->training_seq = 0;
  info->calibration_st = 0;
  info->ddl_periodic_training = 0;
  info->dll_mstr_s = 0;
  for (ind=0; ind<SOC_PETRA_DIAG_NOF_DDR_TRAIN_SEQS; ++ind)
  {
    info->ddr_training_sequence[ind] = 0;
  }
  for (ind=0; ind<SOC_PETRA_DIAG_NOF_DLLS_PER_DRAM; ++ind)
  {
    soc_petra_PETRA_DIAG_DLL_STATUS_INFO_clear(&(info->dll_status[ind]));
  }
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_DIAG_DRAM_ERR_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_DIAG_DRAM_ERR_INFO *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_DIAG_DRAM_ERR_INFO));
  info->bit_err_bitmap = 0;
  info->is_clocking_err = 0;
  for (ind = 0; ind < DIAG_DRAM_NOF_DQSS; ++ind)
  {
    info->is_dqs_con_err[ind] = 0;
  }
  info->is_phy_ready_err = 0;
  info->is_rtt_avg_min_err = 0;
  info->is_rtt_avg_max_err = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_DIAG_LBG_TRAFFIC_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_DIAG_LBG_TRAFFIC_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_DIAG_LBG_TRAFFIC_INFO));
  soc_petra_PETRA_DIAG_LBG_PACKET_PATTERN_clear(&(info->pattern));
  info->packet_size = 0;
  info->nof_packets = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_DIAG_LBG_RESULT_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_DIAG_LBG_RESULT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_DIAG_LBG_RESULT_INFO));
  soc_petra_PETRA_STAT_ALL_STATISTIC_COUNTERS_clear(&(info->counters));
  info->ingress_rate = 0;
  info->egress_rate = 0;
  info->credits = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_DIAG_LBG_PATH_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_DIAG_LBG_PATH_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_DIAG_LBG_PATH_INFO));
  info->nof_ports = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_DIAG_LBG_PACKET_PATTERN_clear(
    SOC_SAND_OUT SOC_PETRA_DIAG_LBG_PACKET_PATTERN *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_DIAG_LBG_PACKET_PATTERN));
  info->data_byte_size = 1;

  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_DIAG_LBG_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_DIAG_LBG_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_DIAG_LBG_INFO));
  soc_petra_PETRA_DIAG_LBG_PATH_INFO_clear(&(info->path));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_DIAG_SOFT_ERR_INFO_clear(
    SOC_SAND_IN  int                unit,
    SOC_SAND_OUT SOC_PETRA_DIAG_SOFT_ERR_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_DIAG_SOFT_ERR_INFO));
  info->pattern = SOC_PETRA_DIAG_NOF_SOFT_ERROR_PATTERNS;
  info->sms = SOC_PETRA_DIAG_SMS_ALL;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_DIAG_SOFT_ERR_RESULT_clear(
    SOC_SAND_OUT SOC_PETRA_DIAG_SOFT_ERR_RESULT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_DIAG_SOFT_ERR_RESULT_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_DIAG_SOFT_SMS_RESULT_clear(
    SOC_SAND_OUT SOC_PETRA_DIAG_SOFT_SMS_RESULT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_DIAG_SOFT_SMS_RESULT_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PETRA_DEBUG_IS_LVL1


uint32
  soc_petra_diag_regs_dump(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32  start,
    SOC_SAND_IN uint32  end
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    soc_petra_reg_size,
    *regs_values = NULL,
    print_options_bm = 0;
  uint32
    print_zero = TRUE;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DIAG_REGS_DUMP);

  regs = soc_petra_regs();

  if (end > (SOC_PETRA_TOTAL_SIZE_OF_REGS/SOC_SAND_REG_SIZE_BYTES))
  {
    soc_petra_reg_size = (SOC_PETRA_TOTAL_SIZE_OF_REGS/SOC_SAND_REG_SIZE_BYTES) - start;
  }
  else
  {
    soc_petra_reg_size = end - start + 1;
  }

  SOC_PETRA_ALLOC_ANY_SIZE(regs_values, uint32, soc_petra_reg_size);

  soc_petra_diag_version_print(
    unit
  );

  res = soc_sand_mem_read(
          unit,
          regs_values,
          start * sizeof(uint32),
          soc_petra_reg_size * sizeof(uint32),
          FALSE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  /*
   *  Note! we intentionally access all address space here, including undefined
   *  addresses.
   *  This action will rise the CpuifReadRegTimeout interrupt.
   *  The below clears the interrupt
   */
  SOC_PETRA_FLD_SET(regs->eci.cpu_streaming_if_interrupts_reg.cpuif_read_reg_timeout, 0x1, 25, exit);

  soc_sand_os_printf("SOC_PETRA core memory registers values\n\r");

  print_options_bm |= SOC_SAND_PRINT_RANGES;

  if(!print_zero)
  {
    print_options_bm |= SOC_SAND_DONT_PRINT_ZEROS;
  }

  print_options_bm |= SOC_SAND_SET_BITS_RANGE(start, SOC_SAND_PRINT_START_ADDR_MSB, SOC_SAND_PRINT_START_ADDR_LSB);

  res = soc_sand_print_block(
          unit,
          regs_values,
          soc_petra_reg_size,
          print_options_bm
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit)

exit:
  SOC_PETRA_FREE_ANY_SIZE(regs_values)

  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_diag_regs_dump()",0,0);
}

uint32
  soc_petra_diag_tbls_dump(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint8    block_id,
    SOC_SAND_IN  uint32   tbl_offset,
    SOC_SAND_IN  uint8  print_zero
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DIAG_TBLS_DUMP);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_diag_tbls_dump_unsafe(
          unit,
          block_id,
          tbl_offset,
          print_zero
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_diag_tbls_dump()",0,0);
}


uint32
  soc_petra_diag_tbls_dump_all(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint8  print_zero
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DIAG_TBLS_DUMP);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_diag_tbls_dump_unsafe(
          unit,
          SOC_PETRA_DIAG_TBLS_DUMP_ALL_BLOCKS,
          SOC_PETRA_DIAG_TBLS_DUMP_ALL_TBLS,
          print_zero
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_diag_tbls_dump_all()",0,0);
}

uint32
  soc_petra_diag_dev_tables_dump(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  SOC_PETRA_DIAG_TBLS_DUMP_MODE dump_mode
    )
{

  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DIAG_DEV_TBLS_DUMP);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  /* verify dump_mode is in the right values spectrum */

  if (dump_mode >= SOC_PETRA_DIAG_NOF_TBLS_DUMP_MODES)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_DIAG_DUMP_MODE_OUT_OF_RANGE_ERR, 20, exit);
    }
    

  res = soc_petra_diag_dev_tables_dump_unsafe(
          unit,
          dump_mode
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

  exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_diag_dev_tables_dump()",0,0);

}


void
  soc_petra_PETRA_DIAG_LBG_TRAFFIC_INFO_print(
    SOC_SAND_IN SOC_PETRA_DIAG_LBG_TRAFFIC_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("Nof_packets: %u[Packets]\n\r",info->nof_packets);
  soc_sand_os_printf("Packet_size: %u[Bytes]\n\r",info->packet_size);
  soc_petra_PETRA_DIAG_LBG_PACKET_PATTERN_print(&info->pattern);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  soc_petra_PETRA_DIAG_LBG_RESULT_INFO_print(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN SOC_PETRA_DIAG_LBG_RESULT_INFO *info
  )
{
  uint8
    level,
    print_format;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  print_format = SOC_SAND_BIT(SOC_SAND_PRINT_FLAVORS_SHORT) | SOC_SAND_BIT(SOC_SAND_PRINT_FLAVORS_NO_ZEROS);
  level = SOC_PETRA_STAT_PRINT_LEVEL_ALL;

  soc_petra_PETRA_STAT_ALL_STATISTIC_COUNTERS_print(unit, &(info->counters),level, print_format);
  soc_sand_os_printf("\n\r Ingress_rate: %u[Kbps]\n\r",info->ingress_rate);
  soc_sand_os_printf("\n\r Egress_rate: %u[Kbps]\n\r",info->egress_rate);
  soc_sand_os_printf("\n\r Credits: %u[Kbps]\n\r",info->credits);

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_DIAG_LBG_PATH_INFO_print(
    SOC_SAND_IN SOC_PETRA_DIAG_LBG_PATH_INFO *info
  )
{
  uint32
    indx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (indx = 0; indx < info->nof_ports; ++indx)
  {
    soc_sand_os_printf("%u \n\r", info->ports[indx] );
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_DIAG_LBG_INFO_print(
    SOC_SAND_IN SOC_PETRA_DIAG_LBG_INFO *info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_petra_PETRA_DIAG_LBG_PATH_INFO_print(&info->path);

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


uint32
  soc_petra_egq_resources_print(
    int unit
  )
{
  uint32
    res = 0;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_egq_resources_print_unsafe(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_egq_resources_print()",0,0);
}
void
  soc_petra_diag_packet_walktrough_print(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                       dest_id,
    SOC_SAND_IN  uint32                      class_id
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PETRA_DIAG_PACKET_WALKTROUGH
    packet_walktrough;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  soc_petra_PETRA_DIAG_PACKET_WALKTROUGH_clear(&packet_walktrough);

  res = soc_petra_diag_packet_walktrough_get(
          unit,
          dest_id,
          class_id,
          &packet_walktrough
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (packet_walktrough.valid == FALSE)
  {
    soc_sand_os_printf("Dest_id %u is not valid\n\r", dest_id);
  }
  else
  {
    soc_sand_os_printf("Dest_id         : %u\n\r", dest_id);
    soc_sand_os_printf("------------------------\n\r");
    soc_sand_os_printf("  Queue_info    :\n\r");
    soc_sand_os_printf("    Queue_qrtt  : %u\n\r", packet_walktrough.queue_qrtt);
    soc_sand_os_printf("    Queue_id    : %u\n\r", packet_walktrough.queue_id);
    soc_sand_os_printf("    Flow_qrtt   : %u\n\r", packet_walktrough.flow_qrtt);
    soc_sand_os_printf("    Flow_id     : %u\n\r", packet_walktrough.flow_id);
    soc_sand_os_printf("    Is_composite: %d \n\r" ,packet_walktrough.is_composite);
    soc_sand_os_printf("    Sys_phy_port: %u\n\r", packet_walktrough.sys_phy_port);
    soc_sand_os_printf("      Dst_fap_id: %d\n\r" , packet_walktrough.dst_fap_id);
    soc_sand_os_printf("      Dst_prt_id: %d\n\r" , packet_walktrough.dst_prt_id);
  }

exit:
  if (ex != no_err)
  {
    soc_sand_os_printf("Error occurred in %s.\n\r", "packet_walktrough_print");
  }
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_diag_all2all_packet_walktrough_print(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   min_dest_id,
    SOC_SAND_IN  uint32   max_dest_id
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    class_i,
    dest_id;
  SOC_PETRA_DIAG_PACKET_WALKTROUGH
    packet_walktrough;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  soc_sand_os_printf("|--------|-----|--------|--------|---|---------|--------|--------|\n\r");
  soc_sand_os_printf("| dst_id | cls | que_id | flo_id | c | phy_prt | ds_fap | ds_prt |\n\r");
  soc_sand_os_printf("|--------|-----|--------|--------|---|---------|--------|--------|\n\r");
  for (dest_id = min_dest_id; dest_id <= max_dest_id; ++dest_id)
  {
    for (class_i = 0; class_i < SOC_PETRA_NOF_TRAFFIC_CLASSES; ++class_i)
    {
      soc_petra_PETRA_DIAG_PACKET_WALKTROUGH_clear(&packet_walktrough);

      res = soc_petra_diag_packet_walktrough_get(
              unit,
              dest_id,
              class_i,
              &packet_walktrough
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

      if (packet_walktrough.valid)
      {
        soc_sand_os_printf(
          "| %6d | %3d | %6u | %6u | %s | %7u | %6d | %6d |\n\r",
          dest_id,
          class_i,
          packet_walktrough.queue_id,
          packet_walktrough.flow_id,
          packet_walktrough.is_composite ? "+" : "-",
          packet_walktrough.sys_phy_port,
          packet_walktrough.dst_fap_id,
          packet_walktrough.dst_prt_id
        );
      }
    }
  }
  soc_sand_os_printf("|--------|-----|--------|--------|---|---------|--------|--------|\n\r");

exit:
  if (ex != no_err)
  {
    soc_sand_os_printf("Error occurred in %s.\n\r", "all2all_packet_walktrough_print");
  }
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_diag_version_print(
    SOC_SAND_IN  int unit
  )
{
  SOC_SAND_RET
    ret = SOC_SAND_OK;
  char
    string_buff[SOC_SAND_VER_STRING_SIZE];
  SOC_SAND_DEVICE_TYPE
    chip_type;
  uint32
    chip_ver,
    dbg_ver;
  const char
    *version_str;
  const char
    *device_name;


  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  /*
   * Soc_sand Drivers
   */
  soc_get_sand_string_ver(string_buff);
  soc_sand_os_printf("SOC_SAND   Driver Version: %s\n\r",string_buff);

  ret = soc_sand_get_device_type(
          unit,
          &chip_type,
          &chip_ver,
          &dbg_ver
        );
  SOC_SAND_CHECK_FUNC_RESULT(ret, 10, exit);

  version_str = "Not SOC_PETRA";
  if(chip_type == SOC_SAND_DEV_PETRA)
  {
    version_str = "SOC_PETRA";
  }
  else if(chip_type == SOC_SAND_DEV_PB)
  {
    version_str = "SOC_PETRA-B";
  }

  device_name = soc_sand_DEVICE_TYPE_to_str(chip_type);
  soc_sand_os_printf(
    "  The device is %s with hardware version: %d.%d.(%s)\n\r",
    device_name,
    chip_ver,
    dbg_ver,
    version_str
  );

exit:

  if (ex != no_err)
  {
    soc_sand_os_printf("Error occurred in %s.\n\r", "version_print");
  }
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


const char*
  soc_petra_PETRA_DIAG_QDR_BIST_ADDR_MODE_to_string(
    SOC_SAND_IN SOC_PETRA_DIAG_QDR_BIST_ADDR_MODE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PETRA_DIAG_QDR_BIST_ADDR_MODE_NORMAL:
    str = "QDR_BIST_ADDR_MODE_NORMAL";
  break;

  case SOC_PETRA_DIAG_QDR_BIST_ADDR_MODE_ADDRESS_SHIFT:
    str = "QDR_BIST_ADDR_MODE_ADDRESS_SHIFT";
  break;

  case SOC_PETRA_DIAG_QDR_BIST_ADDR_MODE_ADDRESS_TEST:
    str = "QDR_BIST_ADDR_MODE_ADDRESS_TEST";
  break;

  default:
    str = " Unknown";
  }
  return str;
}


const char*
  soc_petra_PETRA_DIAG_QDR_BIST_DATA_MODE_to_string(
    SOC_SAND_IN SOC_PETRA_DIAG_QDR_BIST_DATA_MODE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PETRA_DIAG_QDR_BIST_DATA_MODE_NORMAL:
    str = "QDR_BIST_DATA_MODE_NORMAL";
  break;

  case SOC_PETRA_DIAG_QDR_BIST_DATA_MODE_PATTERN_BIT:
    str = "QDR_BIST_DATA_MODE_PATTERN_BIT";
  break;

  case SOC_PETRA_DIAG_QDR_BIST_DATA_MODE_RANDOM:
    str = "QDR_BIST_DATA_MODE_RANDOM";
  break;

  case SOC_PETRA_DIAG_QDR_BIST_DATA_MODE_DATA_SHIFT:
    str = "QDR_BIST_DATA_MODE_DATA_SHIFT";
  break;

  default:
    str = " Unknown";
  }
  return str;
}


const char*
  soc_petra_PETRA_DIAG_BIST_DATA_PATTERN_to_string(
    SOC_SAND_IN SOC_PETRA_DIAG_BIST_DATA_PATTERN enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PETRA_DIAG_BIST_DATA_PATTERN_DIFF:
    str = "Differential";
  break;

  case SOC_PETRA_DIAG_BIST_DATA_PATTERN_ONE:
    str = "All-1";
    break;

  case SOC_PETRA_DIAG_BIST_DATA_PATTERN_ZERO:
    str = "All-0";
    break;

  case SOC_PETRA_DIAG_BIST_DATA_PATTERN_P1:
    str = "P1-Pattern (QDR clear in Parity protection mode)";
    break;

  case SOC_PETRA_DIAG_BIST_DATA_PATTERN_P2:
    str = "P2-Pattern (QDR clear in ECC protection mode)";
    break;
  case SOC_PETRA_DIAG_BIST_DATA_PATTERN_USER_DEF:
    str = "user_def";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  soc_petra_PETRA_DIAG_QDR_PATTERN_MODE_to_string(
    SOC_SAND_IN  SOC_PETRA_DIAG_QDR_PATTERN_MODE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PETRA_DIAG_QDR_PATTERN_MODE_RANDOM:
    str = "random";
  break;
  case SOC_PETRA_DIAG_QDR_PATTERN_MODE_DIFF:
    str = "diff";
  break;
  case SOC_PETRA_DIAG_QDR_PATTERN_MODE_USER_DEF:
    str = "user_def";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  soc_petra_PETRA_DIAG_DRAM_BIST_DATA_MODE_to_string(
    SOC_SAND_IN SOC_PETRA_DIAG_DRAM_BIST_DATA_MODE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PETRA_DIAG_DRAM_BIST_DATA_MODE_NORMAL:
    str = "DRAM_BIST_DATA_MODE_NORMAL";
  break;

  case SOC_PETRA_DIAG_DRAM_BIST_DATA_MODE_PATTERN_BIT:
    str = "DRAM_BIST_DATA_MODE_PATTERN_BIT";
  break;

  case SOC_PETRA_DIAG_DRAM_DATA_MODE_RANDOM:
    str = "DRAM_DATA_MODE_RANDOM";
  break;

  default:
    str = " Unknown";
  }
  return str;
}

const char*
  soc_petra_PETRA_DIAG_SOFT_ERROR_PATTERN_to_string(
    SOC_SAND_IN  SOC_PETRA_DIAG_SOFT_ERROR_PATTERN enum_val
  )
{
  return SOC_TMC_DIAG_SOFT_ERROR_PATTERN_to_string(enum_val);
}

const char*
  soc_petra_PETRA_DIAG_SOFT_COUNT_TYPE_to_string(
    SOC_SAND_IN  SOC_PETRA_DIAG_SOFT_COUNT_TYPE enum_val
  )
{
  return SOC_TMC_DIAG_SOFT_COUNT_TYPE_to_string(enum_val);
}

void
  soc_petra_PETRA_DIAG_QDR_BIST_TEST_RUN_INFO_print(
    SOC_SAND_IN SOC_PETRA_DIAG_QDR_BIST_TEST_RUN_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("Nof Write Commands: %u[Commands]\n\r",info->nof_cmnds_write);
  soc_sand_os_printf("Nof Write Commands: %u[Commands]\n\r",info->nof_cmnds_read);
  soc_sand_os_printf("Start_addr: %u\n\r",info->start_addr);
  soc_sand_os_printf("End_addr: %u\n\r",info->end_addr);
  soc_sand_os_printf("Read Offset: %u[Commands]\n\r",info->read_offset);
  soc_sand_os_printf(
    "Data_mode %s \n\r",
    soc_petra_PETRA_DIAG_QDR_BIST_DATA_MODE_to_string(info->data_mode)
  );
  soc_sand_os_printf(
    "Address_mode %s \n\r",
    soc_petra_PETRA_DIAG_QDR_BIST_ADDR_MODE_to_string(info->address_mode)
  );
  soc_sand_os_printf(
    "Data_pattern %s \n\r",
    soc_petra_PETRA_DIAG_BIST_DATA_PATTERN_to_string(info->data_pattern)
  );
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}



void
  soc_petra_PETRA_DIAG_QDR_BIST_TEST_RES_INFO_print(
    SOC_SAND_IN SOC_PETRA_DIAG_QDR_BIST_TEST_RES_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  if (info->is_test_finished == FALSE)
  {
    soc_sand_os_printf("Test not finished...\n\r");
  }
  else
  {
    if (info->is_qdr_up == FALSE)
    {
      soc_sand_os_printf("QDR interface is down!\n\r");
    }
    else
    {
      soc_sand_os_printf("QDR interface is up: \n\r");
      soc_sand_os_printf("Bit_err_counter: %u[Bits]\n\r",info->bit_err_counter);
      soc_sand_os_printf("Reply_err_counter: %u[Errors]\n\r",info->reply_err_counter);
      soc_sand_os_printf("Bits_error_bitmap: 0x%x\n\r",info->bits_error_bitmap);
      soc_sand_os_printf("Last_addr_err: %u\n\r",info->last_addr_err);
      soc_sand_os_printf("Last_data_err: %u\n\r",info->last_data_err);
    }
  }

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_DIAG_QDR_PATTERN_print(
    SOC_SAND_IN  SOC_PETRA_DIAG_QDR_PATTERN *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("mode %s ", soc_petra_PETRA_DIAG_QDR_PATTERN_MODE_to_string(info->mode));
  for (ind = 0; ind < SOC_PETRA_DIAG_QDR_BIST_PATTERN_SIZE_IN_UINT32S; ++ind)
  {
    soc_sand_os_printf("data[%u]: %u\n\r",ind,info->data[ind]);
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_DIAG_QDR_WINDOW_INFO_print(
    SOC_SAND_IN  SOC_PETRA_DIAG_QDR_WINDOW_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("start: %u[tap]\n\r",info->start);
  soc_sand_os_printf("end: %u[tap]\n\r",info->end);
  soc_sand_os_printf("sampling_point: %u[tap]\n\r",info->sampling_point);
  soc_sand_os_printf("bit_time_in_taps: %u[tap]\n\r",info->total_size);
  soc_sand_os_printf("ratio_percent: %u[%%]\n\r",info->ratio_percent);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_DIAG_DRAM_BIST_TEST_RUN_INFO_print(
    SOC_SAND_IN SOC_PETRA_DIAG_DRAM_BIST_TEST_RUN_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("Finite_nof_cmnds: %u[Commands]\n\r",info->is_finite_nof_cmnds);
  soc_sand_os_printf("Writes_per_cycle: %u[Write commands]\n\r",info->writes_per_cycle);
  soc_sand_os_printf("Reads_per_cycle: %u[Read commands]\n\r",info->reads_per_cycle);
  soc_sand_os_printf("Start_addr: %u\n\r",info->start_addr);
  soc_sand_os_printf("End_addr: %u\n\r",info->end_addr);
  soc_sand_os_printf(
    "data_mode %s \n\r",
    soc_petra_PETRA_DIAG_DRAM_BIST_DATA_MODE_to_string(info->data_mode)
  );
  soc_sand_os_printf(
    "data_pattern %s \n\r",
    soc_petra_PETRA_DIAG_BIST_DATA_PATTERN_to_string(info->data_pattern)
  );
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_DIAG_DRAM_BIST_TEST_RES_INFO_print(
    SOC_SAND_IN SOC_PETRA_DIAG_DRAM_BIST_TEST_RES_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  if (info->is_test_finished == FALSE)
  {
    soc_sand_os_printf("Test not finished...\n\r");
  }
  else
  {
    if (info->is_dram_up == FALSE)
    {
      soc_sand_os_printf("DRAM interface is down!\n\r");
    }
    else
    {
      soc_sand_os_printf("DRAM interface is up. ");
      soc_sand_os_printf("Error counter: %u[Errors]\n\r",info->reply_err_counter);
    }
  }

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}



void
  soc_petra_PETRA_DIAG_DRAM_ACCESS_INFO_print(
    SOC_SAND_IN SOC_PETRA_DIAG_DRAM_ACCESS_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("address: %u\n\r",info->address);
  soc_sand_os_printf("is_data_size_bits_256_not_32: %u\n\r",info->is_data_size_bits_256_not_32);
  soc_sand_os_printf("is_infinite_nof_actions: %u\n\r",info->is_infinite_nof_actions);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}



void
  soc_petra_PETRA_DIAG_DRAM_READ_COMPARE_STATUS_print(
    SOC_SAND_IN SOC_PETRA_DIAG_DRAM_READ_COMPARE_STATUS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("success: %u\n\r",info->success);
  soc_sand_os_printf("error_bits_global: %u\n\r",info->error_bits_global);
  soc_sand_os_printf("nof_addr_with_errors: %u\n\r",info->nof_addr_with_errors);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  soc_petra_PETRA_DIAG_DLL_STATUS_INFO_print(
    SOC_SAND_IN SOC_PETRA_DIAG_DLL_STATUS_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("DDL control 0                         : 0x%08X\n\r",info->ddl_control_0);
  soc_sand_os_printf("DDL control 1                         : 0x%08X\n\r",info->ddl_control_1);
  soc_sand_os_printf("DDL control 2                         : 0x%08X\n\r",info->ddl_control_2);
  soc_sand_os_printf("DLL finished initialization           : %u\n\r",info->dll_init_done);
  soc_sand_os_printf("Round trip delay value                : 0x%04X\n\r", info->rnd_trp);
  soc_sand_os_printf("Round trip diff delay value           : 0x%04X\n\r", info->rnd_trp_diff);
  soc_sand_os_printf("Phase selection down indication       : 0x%04X\n\r", info->dll_ph_dn);
  soc_sand_os_printf("Phase selection up indication         : 0x%04X\n\r", info->dll_ph_up);
  soc_sand_os_printf("Output selected phase                 : 0x%04X\n\r", info->main_ph_sel);
  soc_sand_os_printf("Phase selected for sync clock 2       : 0x%04X\n\r", info->ph2sel);
  soc_sand_os_printf("Half cycle count vector               : 0x%04X\n\r", info->hc_sel_vec);
  soc_sand_os_printf("Quarter cycle count vector            : 0x%04X\n\r", info->qc_sel_vec);
  soc_sand_os_printf("Count vector                          : 0x%04X\n\r", info->sel_vec);
  soc_sand_os_printf("Half granularity indication           : %u\n\r",info->sel_hg);
  soc_sand_os_printf("Phase selected for half cycle up      : %u\n\r",info->ph_sel_hc_up);
  soc_sand_os_printf("Insertion delay compensation vector   : 0x%04X\n\r", info->ins_dly_min_vec);
  soc_sand_os_printf("Phase selection offset                : %u\n\r",info->ddl_init_main_ph_sel_ofst);
  soc_sand_os_printf("DDL phase selected for half cycle up  : %u\n\r",info->ddl_ph_sel_hc_up);
  soc_sand_os_printf("DDL train trigger up limit            : %u\n\r",info->ddl_train_trig_up_limit);
  soc_sand_os_printf("DDL train trigger down limit          : %u\n\r",info->ddl_train_trig_dn_limit);
  soc_sand_os_printf("Phase selection error                 : %u\n\r",info->ph_sel_err);
  soc_sand_os_printf("Delay max min mode                    : %u\n\r",info->dly_max_min_mode);
  soc_sand_os_printf("Phase selected                        : 0x%04X\n\r", info->ph_sel);
  soc_sand_os_printf("Max selvec                            : 0x%04X\n\r", info->max_sel_vec);
  soc_sand_os_printf("Min selvec                            : 0x%04X\n\r", info->min_sel_vec);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}



void
  soc_petra_PETRA_DIAG_DRAM_STATUS_INFO_print(
    SOC_SAND_IN SOC_PETRA_DIAG_DRAM_STATUS_INFO *info
  )
{
  uint32
    ind,
    ind2,
    flag;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("\n\r");
  soc_sand_os_printf("DRAM training sequence control        : 0x%08X\n\r",info->training_seq);
  soc_sand_os_printf("Io calibration status                 : 0x%08X\n\r",info->calibration_st);
  soc_sand_os_printf("DDL periodic training                 : 0x%08X\n\r",info->ddl_periodic_training);
  soc_sand_os_printf("DLL master control vector             : 0x%04X\n\r", info->dll_mstr_s);
  soc_sand_os_printf("\n\r");

  for (ind=0; ind<SOC_PETRA_DIAG_NOF_DDR_TRAIN_SEQS; ++ind)
  {
    ind2 = SOC_PETRA_DIAG_NOF_DDR_TRAIN_SEQS - ind - 1;
    soc_sand_os_printf("DDR training sequence[%u]              : 0x%08X\n\r",ind2,info->ddr_training_sequence[ind2]);
  }

  soc_sand_os_printf("\n\r");
  soc_sand_os_printf("--PER-DLL STATUS--\n\r");
  soc_sand_os_printf("\n\r");

  for (ind=0; ind<SOC_PETRA_DIAG_NOF_DLLS_PER_DRAM; ++ind)
  {
    soc_sand_os_printf("--DLL %u--\n\r",ind);
    soc_petra_PETRA_DIAG_DLL_STATUS_INFO_print(&(info->dll_status[ind]));
    soc_sand_os_printf("\n\r");
  }

  flag = 0;
  for (ind=0; ind<SOC_PETRA_DRAM_DLL_RAM_TABLE_SIZE; ++ind)
  {
    if (info->dll_ram_tbl_data[ind].dll_ram[0] != 0 || info->dll_ram_tbl_data[ind].dll_ram[1] != 0)
    {
      flag = 1;
    }
  }

  if (flag == 0)
  {
    soc_sand_os_printf("DLL RAM                   : all zeros\n\r");
  }
  else
  {
    soc_sand_os_printf("DLL RAM                   :\n\r ");

    for (ind=0; ind<SOC_PETRA_DRAM_DLL_RAM_TABLE_SIZE; ++ind)
    {
      soc_sand_os_printf(" Offset %u:LSB: 0x%X MSB: 0x%X\n\r", ind, info->dll_ram_tbl_data[ind].dll_ram[0], info->dll_ram_tbl_data[ind].dll_ram[1]);
    }
  }

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_DIAG_DRAM_ERR_INFO_print(
    SOC_SAND_IN  SOC_PETRA_DIAG_DRAM_ERR_INFO *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("bit_err_bitmap: %u\n\r",info->bit_err_bitmap);
  soc_sand_os_printf("is_clocking_err: %u\n\r",info->is_clocking_err);
  for (ind = 0; ind < DIAG_DRAM_NOF_DQSS; ++ind)
  {
    soc_sand_os_printf("is_dqs_con_err[%u]: %u\n\r",ind,info->is_dqs_con_err[ind]);
  }
  soc_sand_os_printf("is_phy_ready_err: %u\n\r",info->is_phy_ready_err);
  soc_sand_os_printf("is_rtt_avg_min_err: %u\n\r",info->is_rtt_avg_min_err);
  soc_sand_os_printf("is_rtt_avg_max_err: %u\n\r",info->is_rtt_avg_max_err);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}
void
  soc_petra_PETRA_DIAG_LBG_PACKET_PATTERN_print(
    SOC_SAND_IN SOC_PETRA_DIAG_LBG_PACKET_PATTERN *info
  )
{
  uint32 ind=0;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind=0; ind<info->data_byte_size; ++ind)
  {
    soc_sand_os_printf("Data[%u]:       %02x\n\r", ind,info->data[ind]);
  }
  soc_sand_os_printf("Data_byte_size: %u\n\r",info->data_byte_size);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}
void
  soc_petra_PETRA_DIAG_SOFT_ERR_INFO_print(
    SOC_SAND_IN  SOC_PETRA_DIAG_SOFT_ERR_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_DIAG_SOFT_ERR_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_DIAG_SOFT_ERR_RESULT_print(
    SOC_SAND_IN  SOC_PETRA_DIAG_SOFT_ERR_RESULT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_DIAG_SOFT_ERR_RESULT_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_DIAG_SOFT_SMS_RESULT_print(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  uint32                  sms_ndx,
    SOC_SAND_IN  uint32                  sone_ndx,
    SOC_SAND_IN  SOC_PETRA_DIAG_SOFT_SMS_RESULT *info
  )
{
  uint32
    ind;
  uint32
    res,
    sel_size;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_petra_diag_simple_mbist_sms_sel_size_get(unit, sms_ndx, &sel_size);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (info->nof_errs.err_dp + info->nof_errs.err_rf + info->nof_errs.err_sp)
  {
    if(sone_ndx == SOC_SAND_INTERN_VAL_INVALID_32)
    {
      soc_sand_os_printf("SMS: %d, ", sms_ndx);
    }
    else
    {
      soc_sand_os_printf("SMS: %d, SONE: %d, ", sms_ndx, sone_ndx);
    }
    soc_sand_os_printf("diag_chain: 0x");
    for (ind = 0; ind < ((sel_size - 1) / 32) + 1; ++ind)
    {
      soc_sand_os_printf("%08x",info->diag_chain[ind]);
    }
    soc_sand_os_printf("\n\r");
    soc_sand_os_printf("nof_errs:");
    SOC_TMC_DIAG_SOFT_ERR_RESULT_print(&(info->nof_errs));
  }

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif /* SOC_PETRA_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

