/* $Id: ppd_api_diag.c,v 1.26 Broadcom SDK $
 * $Copyright: Copyright 2012 Broadcom Corporation.
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
* FILENAME:       DuneDriver/ppd/src/soc_ppd_api_diag.c
*
* MODULE PREFIX:  ppd
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

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_PPD

#include <shared/bsl.h>
#include <soc/dpp/drv.h>

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dcmn/error.h>

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>

#include <soc/dpp/PPD/ppd_api_framework.h>
#include <soc/dpp/PPD/ppd_api_diag.h>
#ifdef LINK_PB_LIBRARIES
  #include <soc/dpp/Petra/PB_PP/pb_pp_api_diag.h>
#endif
#ifdef LINK_ARAD_LIBRARIES
  #include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_diag.h>
#endif



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

CONST STATIC
  SOC_PROCEDURE_DESC_ELEMENT
    Ppd_procedure_desc_element_diag[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_DIAG_SAMPLE_ENABLE_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_DIAG_SAMPLE_ENABLE_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_DIAG_SAMPLE_ENABLE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_DIAG_SAMPLE_ENABLE_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_DIAG_MODE_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_DIAG_MODE_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_DIAG_MODE_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_DIAG_MODE_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_DIAG_PKT_TRACE_CLEAR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_DIAG_PKT_TRACE_CLEAR_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_DIAG_RECEIVED_PACKET_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_DIAG_RECEIVED_PACKET_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_DIAG_PARSING_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_DIAG_PARSING_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_DIAG_TERMINATION_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_DIAG_TERMINATION_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_DIAG_FRWRD_LKUP_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_DIAG_FRWRD_LKUP_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_DIAG_FRWRD_LPM_LKUP_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_DIAG_FRWRD_LPM_LKUP_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_DIAG_TRAPS_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_DIAG_TRAPS_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_DIAG_TRAPPED_PACKET_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_DIAG_TRAPPED_PACKET_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_DIAG_TRAPS_ALL_TO_CPU),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_DIAG_TRAPS_ALL_TO_CPU_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_DIAG_TRAPS_STAT_RESTORE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_DIAG_TRAPS_STAT_RESTORE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_DIAG_FRWRD_DECISION_TRACE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_DIAG_FRWRD_DECISION_TRACE_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_DIAG_LEARNING_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_DIAG_LEARNING_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_DIAG_ING_VLAN_EDIT_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_DIAG_ING_VLAN_EDIT_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_DIAG_PKT_ASSOCIATED_TM_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_DIAG_PKT_ASSOCIATED_TM_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_DIAG_ENCAP_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_DIAG_ENCAP_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_DIAG_EG_DROP_LOG_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_DIAG_EG_DROP_LOG_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_DIAG_DB_LIF_LKUP_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_DIAG_DB_LIF_LKUP_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_DIAG_DB_LEM_LKUP_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_DIAG_DB_LEM_LKUP_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_DIAG_DB_TCAM_LKUP_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_DIAG_DB_TCAM_LKUP_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_DIAG_PKT_SEND),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_DIAG_PKT_SEND_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_DIAG_GET_PROCS_PTR),
  /*
   * } Auto generated. Do not edit previous section.
   */



  /*
   * Last element. Do no touch.
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};


/* } */
/*************
 * FUNCTIONS *
 *************/
/* { */

/*********************************************************************
*     Enable/disable diagnostic APIs.affects only APIs with
 *     type: need_sample
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_diag_sample_enable_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint8                               enable
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_DIAG_SAMPLE_ENABLE_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_TMP_DEVICE_CALL(diag_sample_enable_set,(unit, enable));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_diag_sample_enable_set_print,(unit,enable));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_diag_sample_enable_set()", 0, 0);
}

/*********************************************************************
*     Enable/disable diagnostic APIs.affects only APIs with
 *     type: need_sample
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_diag_sample_enable_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT uint8                               *enable
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_DIAG_SAMPLE_ENABLE_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(enable);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(diag_sample_enable_get,(unit, enable));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_diag_sample_enable_get_print,(unit));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_diag_sample_enable_get()", 0, 0);
}

/*********************************************************************
*     Set the mode configuration for diag module, including
 *     diag-flavor
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_diag_mode_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_DIAG_MODE_INFO                      *mode_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_DIAG_MODE_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(mode_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(diag_mode_info_set,(unit, mode_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_diag_mode_info_set_print,(unit,mode_info));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_diag_mode_info_set()", 0, 0);
}

/*********************************************************************
*     Set the mode configuration for diag module, including
 *     diag-flavor
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_diag_mode_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT SOC_PPD_DIAG_MODE_INFO                      *mode_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_DIAG_MODE_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(mode_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(diag_mode_info_get,(unit, mode_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_diag_mode_info_get_print,(unit));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_diag_mode_info_get()", 0, 0);
}

/*********************************************************************
 *     Get the VSI information of the last packet.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_diag_vsi_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT SOC_PPD_DIAG_VSI_INFO                      *vsi_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_DIAG_VSI_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(vsi_info);

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(diag_vsi_info_get,(unit, vsi_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_diag_vsi_info_get_print,(unit));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_diag_vsi_info_get()", 0, 0);
}

/*********************************************************************
*     Clear the trace of transmitted packet, so next trace
 *     info will relate to next packets to transmit
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_diag_pkt_trace_clear(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                pkt_trace
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_DIAG_PKT_TRACE_CLEAR);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_TMP_DEVICE_CALL(diag_pkt_trace_clear,(unit, pkt_trace));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_diag_pkt_trace_clear_print,(unit,pkt_trace));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_diag_pkt_trace_clear()", 0, 0);
}

/*********************************************************************
*     Returns the header of last received packet entered the
 *     device and the its association to TM/system/PP ports.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_diag_received_packet_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT SOC_PPD_DIAG_RECEIVED_PACKET_INFO           *rcvd_pkt_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_DIAG_RECEIVED_PACKET_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(rcvd_pkt_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(diag_received_packet_info_get,(unit, rcvd_pkt_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_diag_received_packet_info_get_print,(unit));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_diag_received_packet_info_get()", 0, 0);
}

/*********************************************************************
*     Returns network headers qualified on packet upon
 *     parsing, including packet format, L2 headers fields,...
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_diag_parsing_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT SOC_PPD_DIAG_PARSING_INFO                   *pars_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_DIAG_PARSING_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(pars_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(diag_parsing_info_get,(unit, pars_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_diag_parsing_info_get_print,(unit));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_diag_parsing_info_get()", 0, 0);
}

/*********************************************************************
*     Returns information obtained by termination including
 *     terminated headers
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_diag_termination_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT SOC_PPD_DIAG_TERM_INFO                      *term_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_DIAG_TERMINATION_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(term_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(diag_termination_info_get,(unit, term_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_diag_termination_info_get_print,(unit));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_diag_termination_info_get()", 0, 0);
}

/*********************************************************************
*     Returns the forwarding lookup performed including:
 *     forwarding type (bridging, routing, ILM, ...), the key
 *     used for the lookup and the result of the lookup
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_diag_frwrd_lkup_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT SOC_PPD_DIAG_FRWRD_LKUP_INFO                *frwrd_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_DIAG_FRWRD_LKUP_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(frwrd_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(diag_frwrd_lkup_info_get,(unit, frwrd_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_diag_frwrd_lkup_info_get_print,(unit));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_diag_frwrd_lkup_info_get()", 0, 0);
}

/*********************************************************************
*     Simulate IP lookup in the device tables and return
 *     FEC-pointer
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_diag_frwrd_lpm_lkup_get(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PPD_DIAG_IPV4_VPN_KEY              *lpm_key,
    SOC_SAND_OUT uint32                           *fec_ptr,
    SOC_SAND_OUT uint8                          *found
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_DIAG_FRWRD_LPM_LKUP_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(lpm_key);
  SOC_SAND_CHECK_NULL_INPUT(fec_ptr);
  SOC_SAND_CHECK_NULL_INPUT(found);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(diag_frwrd_lpm_lkup_get,(unit, lpm_key, fec_ptr, found));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_diag_frwrd_lpm_lkup_get_print,(unit,lpm_key));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_diag_frwrd_lpm_lkup_get()", 0, 0);
}

/*********************************************************************
*     Returns information regard packet trapping/snooping,
 *     including which traps/snoops were fulfilled, which
 *     trap/snoop was committed, and whether packet was
 *     forwarded/processed according to trap or according to
 *     normal packet processing flow.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_diag_traps_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT SOC_PPD_DIAG_TRAPS_INFO                     *traps_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_DIAG_TRAPS_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(traps_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(diag_traps_info_get,(unit, traps_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_diag_traps_info_get_print,(unit));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_diag_traps_info_get()", 0, 0);
}

/*********************************************************************
*     Returns packet trapped to CPU with additional
 *     information: identifying the reason of trapping (if
 *     any), the source system port, and pointer to the packet
 *     payload.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_diag_trapped_packet_info_get(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_INOUT uint8                          *buff,
    SOC_SAND_IN  uint32                           max_size,
    SOC_SAND_OUT uint32                           *buff_len,
    SOC_SAND_OUT SOC_PPD_DIAG_TRAP_PACKET_INFO          *packet_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_DIAG_TRAPPED_PACKET_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(buff);
  SOC_SAND_CHECK_NULL_INPUT(buff_len);
  SOC_SAND_CHECK_NULL_INPUT(packet_info);

  SOC_PPD_DEVICE_CALL(diag_trapped_packet_info_get,(unit, buff, max_size, buff_len, packet_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_diag_trapped_packet_info_get_print,(unit,buff,max_size));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_diag_trapped_packet_info_get()", 0, 0);
}

/*********************************************************************
*     Set the destination of all traps to be CPU, so trapped
 *     packet can be captured and examined by CPU
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_diag_traps_all_to_cpu(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_DIAG_TRAP_TO_CPU_INFO               *trap_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_DIAG_TRAPS_ALL_TO_CPU);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(trap_info);
  SOC_PPD_DEVICE_CALL(diag_traps_all_to_cpu,(unit, trap_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_diag_traps_all_to_cpu_print,(unit,trap_info));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_diag_traps_all_to_cpu()", 0, 0);
}

/*********************************************************************
*     Restore the configuration of all traps, to be as
 *     configured before last calling for
 *     soc_ppd_diag_traps_all_to_cpu()
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_diag_traps_stat_restore(
    SOC_SAND_IN  int                               unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_DIAG_TRAPS_STAT_RESTORE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_DEVICE_CALL(diag_traps_stat_restore,(unit));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_diag_traps_stat_restore_print,(unit));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_diag_traps_stat_restore()", 0, 0);
}

/*********************************************************************
*     Return the trace (changes) for forwarding decision for
 *     last packet in several phases in processing
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_diag_frwrd_decision_trace_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT SOC_PPD_DIAG_FRWRD_DECISION_TRACE_INFO      *frwrd_trace_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_DIAG_FRWRD_DECISION_TRACE_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(frwrd_trace_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(diag_frwrd_decision_trace_get,(unit, frwrd_trace_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_diag_frwrd_decision_trace_get_print,(unit));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_diag_frwrd_decision_trace_get()", 0, 0);
}

/*********************************************************************
*     Get the information to be learned for the incoming
 *     packet. This is the information that the processing
 *     determine to be learned, the MACT supposed to learn this
 *     information.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_diag_learning_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT SOC_PPD_DIAG_LEARN_INFO                     *learn_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_DIAG_LEARNING_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(learn_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(diag_learning_info_get,(unit, learn_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_diag_learning_info_get_print,(unit));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_diag_learning_info_get()", 0, 0);
}

/*********************************************************************
*     Get the result of ingress vlan editing,
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_diag_ing_vlan_edit_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT SOC_PPD_DIAG_VLAN_EDIT_RES                  *vec_res
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_DIAG_ING_VLAN_EDIT_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(vec_res);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(diag_ing_vlan_edit_info_get,(unit, vec_res));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_diag_ing_vlan_edit_info_get_print,(unit));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_diag_ing_vlan_edit_info_get()", 0, 0);
}

/*********************************************************************
*     The Traffic management information associated with the
 *     packet including meter, DP, TC, etc...
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_diag_pkt_associated_tm_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT SOC_PPD_DIAG_PKT_TM_INFO                    *pkt_tm_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_DIAG_PKT_ASSOCIATED_TM_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(pkt_tm_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(diag_pkt_associated_tm_info_get,(unit, pkt_tm_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_diag_pkt_associated_tm_info_get_print,(unit));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_diag_pkt_associated_tm_info_get()", 0, 0);
}

/*********************************************************************
*     Get the encapsulation and editing information applied to
 *     last packet
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_diag_encap_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT SOC_PPD_DIAG_ENCAP_INFO                     *encap_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_DIAG_ENCAP_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(encap_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(diag_encap_info_get,(unit, encap_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_diag_encap_info_get_print,(unit));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_diag_encap_info_get()", 0, 0);
}

/*********************************************************************
*     Get the reason for packet discard
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_diag_eg_drop_log_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT SOC_PPD_DIAG_EG_DROP_LOG_INFO               *eg_drop_log
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_DIAG_EG_DROP_LOG_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(eg_drop_log);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(diag_eg_drop_log_get,(unit, eg_drop_log));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_diag_eg_drop_log_get_print,(unit));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_diag_eg_drop_log_get()", 0, 0);
}

/*********************************************************************
*     Returns the lookup key and result used in the LIF DB
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_diag_db_lif_lkup_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_DIAG_DB_USE_INFO                    *db_info,
    SOC_SAND_OUT SOC_PPD_DIAG_LIF_LKUP_INFO                  *lkup_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_DIAG_DB_LIF_LKUP_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(db_info);
  SOC_SAND_CHECK_NULL_INPUT(lkup_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(diag_db_lif_lkup_info_get,(unit, db_info, lkup_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_diag_db_lif_lkup_info_get_print,(unit,db_info));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_diag_db_lif_lkup_info_get()", 0, 0);
}

/*********************************************************************
*     Returns the lookup key and result used in the LEM DB
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_diag_db_lem_lkup_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_DIAG_DB_USE_INFO                    *db_info,
    SOC_SAND_OUT SOC_PPD_DIAG_LEM_LKUP_TYPE                  *type,
    SOC_SAND_OUT SOC_PPD_DIAG_LEM_KEY                        *key,
    SOC_SAND_OUT SOC_PPD_DIAG_LEM_VALUE                      *val,
    SOC_SAND_OUT uint8                               *valid
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_DIAG_DB_LEM_LKUP_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(db_info);
  SOC_SAND_CHECK_NULL_INPUT(type);
  SOC_SAND_CHECK_NULL_INPUT(key);
  SOC_SAND_CHECK_NULL_INPUT(val);
  SOC_SAND_CHECK_NULL_INPUT(valid);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(diag_db_lem_lkup_info_get,(unit, db_info, type, key, val, valid));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_diag_db_lem_lkup_info_get_print,(unit,db_info));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_diag_db_lem_lkup_info_get()", 0, 0);
}

/*********************************************************************
*     Returns the lookup key and result used in the TCAM DB
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_diag_db_tcam_lkup_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_DIAG_TCAM_USE_INFO                  *use_info,
    SOC_SAND_OUT SOC_PPD_DIAG_LEM_LKUP_TYPE                  *type,
    SOC_SAND_OUT SOC_PPD_DIAG_TCAM_KEY                       *key,
    SOC_SAND_OUT SOC_PPD_DIAG_TCAM_VALUE                     *val,
    SOC_SAND_OUT uint8                               *valid
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_DIAG_DB_TCAM_LKUP_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(use_info);
  SOC_SAND_CHECK_NULL_INPUT(type);
  SOC_SAND_CHECK_NULL_INPUT(key);
  SOC_SAND_CHECK_NULL_INPUT(val);
  SOC_SAND_CHECK_NULL_INPUT(valid);

  SOC_PPD_DEVICE_CALL(diag_db_tcam_lkup_info_get,(unit, use_info, type, key, val, valid));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_diag_db_tcam_lkup_info_get_print,(unit,use_info));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_diag_db_tcam_lkup_info_get()", 0, 0);
}

/* ******************************************************************** 
 * Return the egress vlan edit command informations
 * ********************************************************************/
uint32 
  soc_ppd_diag_egress_vlan_edit_info_get( 
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT SOC_PPD_DIAG_EGRESS_VLAN_EDIT_RES           *prm_vec_res
  )
{
    uint32
      res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_DRIVER_AND_DEVICE;

    SOC_SAND_CHECK_NULL_INPUT(prm_vec_res);

    SOC_PPD_ARAD_ONLY_DEVICE_CALL(diag_egress_vlan_edit_info_get,(unit, prm_vec_res));

    SOC_PPD_DO_NOTHING_AND_EXIT;
  exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_diag_egress_vlan_edit_info_get()", 0, 0);

}


/*********************************************************************
*     Inject packets from CPU port to device.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_diag_pkt_send(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_DIAG_PACKET_INFO                    *pkt_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_DIAG_PKT_SEND);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(pkt_info);

  SOC_PB_PP_DEVICE_CALL(diag_pkt_send,(unit, pkt_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_diag_pkt_send_print,(unit,pkt_info));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_diag_pkt_send()", 0, 0);
}

/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     soc_ppd_api_diag module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_PROCEDURE_DESC_ELEMENT*
  soc_ppd_diag_get_procs_ptr(
  )
{
  return Ppd_procedure_desc_element_diag;
}
void
  SOC_PPD_DIAG_BUFFER_clear(
    SOC_SAND_OUT SOC_PPD_DIAG_BUFFER *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_DIAG_BUFFER_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_DIAG_MODE_INFO_clear(
    SOC_SAND_OUT SOC_PPD_DIAG_MODE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_DIAG_MODE_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_DIAG_VSI_INFO_clear(
    SOC_SAND_OUT SOC_PPD_DIAG_VSI_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_DIAG_VSI_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_DIAG_DB_USE_INFO_clear(
    SOC_SAND_OUT SOC_PPD_DIAG_DB_USE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_DIAG_DB_USE_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_DIAG_TCAM_USE_INFO_clear(
    SOC_SAND_OUT SOC_PPD_DIAG_TCAM_USE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_DIAG_TCAM_USE_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_DIAG_RECEIVED_PACKET_INFO_clear(
    SOC_SAND_OUT SOC_PPD_DIAG_RECEIVED_PACKET_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_DIAG_RECEIVED_PACKET_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_DIAG_TRAP_PACKET_INFO_clear(
    SOC_SAND_OUT SOC_PPD_DIAG_TRAP_PACKET_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_DIAG_TRAP_PACKET_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_DIAG_TRAP_INFO_clear(
    SOC_SAND_OUT SOC_PPD_DIAG_TRAP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_DIAG_TRAP_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_DIAG_SNOOP_INFO_clear(
    SOC_SAND_OUT SOC_PPD_DIAG_SNOOP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_DIAG_SNOOP_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_DIAG_TRAPS_INFO_clear(
    SOC_SAND_OUT SOC_PPD_DIAG_TRAPS_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_DIAG_TRAPS_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_DIAG_TRAP_TO_CPU_INFO_clear(
    SOC_SAND_OUT SOC_PPD_DIAG_TRAP_TO_CPU_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_DIAG_TRAP_TO_CPU_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_DIAG_LEM_KEY_clear(
    SOC_SAND_OUT SOC_PPD_DIAG_LEM_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_DIAG_LEM_KEY_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_DIAG_LIF_KEY_clear(
    SOC_SAND_OUT SOC_PPD_DIAG_LIF_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_DIAG_LIF_KEY_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_DIAG_LIF_VALUE_clear(
    SOC_SAND_OUT SOC_PPD_DIAG_LIF_VALUE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_DIAG_LIF_VALUE_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_DIAG_LIF_LKUP_INFO_clear(
    SOC_SAND_OUT SOC_PPD_DIAG_LIF_LKUP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_DIAG_LIF_LKUP_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_DIAG_TCAM_KEY_clear(
    SOC_SAND_OUT SOC_PPD_DIAG_TCAM_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_DIAG_TCAM_KEY_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_DIAG_TCAM_VALUE_clear(
    SOC_SAND_OUT SOC_PPD_DIAG_TCAM_VALUE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_DIAG_TCAM_VALUE_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_DIAG_LEM_VALUE_clear(
    SOC_SAND_OUT SOC_PPD_DIAG_LEM_VALUE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_DIAG_LEM_VALUE_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_DIAG_PARSING_L2_INFO_clear(
    SOC_SAND_OUT SOC_PPD_DIAG_PARSING_L2_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_DIAG_PARSING_L2_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_DIAG_PARSING_MPLS_INFO_clear(
    SOC_SAND_OUT SOC_PPD_DIAG_PARSING_MPLS_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_DIAG_PARSING_MPLS_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_DIAG_PARSING_IP_INFO_clear(
    SOC_SAND_OUT SOC_PPD_DIAG_PARSING_IP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_DIAG_PARSING_IP_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_DIAG_PARSING_HEADER_INFO_clear(
    SOC_SAND_OUT SOC_PPD_DIAG_PARSING_HEADER_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_DIAG_PARSING_HEADER_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_DIAG_PARSING_INFO_clear(
    SOC_SAND_OUT SOC_PPD_DIAG_PARSING_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_DIAG_PARSING_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_DIAG_TERM_MPLS_LABEL_INFO_clear(
    SOC_SAND_OUT SOC_PPD_DIAG_TERM_MPLS_LABEL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_DIAG_TERM_MPLS_LABEL_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_DIAG_TERMINATED_MPLS_LABEL_INFO_clear(
    SOC_SAND_OUT SOC_PPD_DIAG_TERMINATED_MPLS_LABEL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_DIAG_TERMINATED_MPLS_LABEL_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_DIAG_TERM_MPLS_INFO_clear(
    SOC_SAND_OUT SOC_PPD_DIAG_TERM_MPLS_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_DIAG_TERM_MPLS_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_DIAG_TERM_INFO_clear(
    SOC_SAND_OUT SOC_PPD_DIAG_TERM_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_DIAG_TERM_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_DIAG_IPV4_VPN_KEY_clear(
    SOC_SAND_OUT SOC_PPD_DIAG_IPV4_VPN_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_DIAG_IPV4_VPN_KEY_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_DIAG_IPV6_VPN_KEY_clear(
    SOC_SAND_OUT SOC_PPD_DIAG_IPV6_VPN_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_DIAG_IPV6_VPN_KEY_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_DIAG_FRWRD_LKUP_KEY_clear(
    SOC_SAND_OUT SOC_PPD_DIAG_FRWRD_LKUP_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_DIAG_FRWRD_LKUP_KEY_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_DIAG_FRWRD_LKUP_VALUE_clear(
    SOC_SAND_OUT SOC_PPD_DIAG_FRWRD_LKUP_VALUE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_DIAG_FRWRD_LKUP_VALUE_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_DIAG_FRWRD_LKUP_INFO_clear(
    SOC_SAND_OUT SOC_PPD_DIAG_FRWRD_LKUP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_DIAG_FRWRD_LKUP_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_DIAG_LEARN_INFO_clear(
    SOC_SAND_OUT SOC_PPD_DIAG_LEARN_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_DIAG_LEARN_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_DIAG_VLAN_EDIT_RES_clear(
    SOC_SAND_OUT SOC_PPD_DIAG_VLAN_EDIT_RES *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_DIAG_VLAN_EDIT_RES_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_DIAG_FRWRD_DECISION_INFO_clear(
    SOC_SAND_OUT SOC_PPD_DIAG_FRWRD_DECISION_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_DIAG_FRWRD_DECISION_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_DIAG_FRWRD_DECISION_TRACE_INFO_clear(
    SOC_SAND_OUT SOC_PPD_DIAG_FRWRD_DECISION_TRACE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_DIAG_FRWRD_DECISION_TRACE_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_DIAG_PKT_TM_INFO_clear(
    SOC_SAND_OUT SOC_PPD_DIAG_PKT_TM_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_DIAG_PKT_TM_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_DIAG_ENCAP_INFO_clear(
    SOC_SAND_OUT SOC_PPD_DIAG_ENCAP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_DIAG_ENCAP_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_DIAG_EG_DROP_LOG_INFO_clear(
    SOC_SAND_OUT SOC_PPD_DIAG_EG_DROP_LOG_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_DIAG_EG_DROP_LOG_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_DIAG_ETH_PACKET_INFO_clear(
    SOC_SAND_OUT SOC_PPD_DIAG_ETH_PACKET_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_DIAG_ETH_PACKET_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_DIAG_PACKET_INFO_clear(
    SOC_SAND_OUT SOC_PPD_DIAG_PACKET_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_DIAG_PACKET_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void 
  SOC_PPD_DIAG_EGRESS_VLAN_EDIT_INFO_clear(
     SOC_SAND_OUT SOC_PPD_DIAG_EGRESS_VLAN_EDIT_RES *prm_vec_res 
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(prm_vec_res);

  SOC_PPC_DIAG_EGRESS_VLAN_EDIT_INFO_clear(prm_vec_res);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_DIAG_LIF_PROFILE_INFO_clear(
	 SOC_SAND_OUT SOC_PPD_DIAG_LIF_PROFILE_INFO *info
  )
{
	SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
	SOC_SAND_CHECK_NULL_INPUT(info);

	SOC_PPC_DIAG_LIF_PROFILE_INFO_clear(info);
  exit:
	SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PPD_DEBUG_IS_LVL1

const char*
  SOC_PPD_DIAG_FWD_LKUP_TYPE_to_string(
    SOC_SAND_IN  SOC_PPD_DIAG_FWD_LKUP_TYPE enum_val
  )
{
  return SOC_PPC_DIAG_FWD_LKUP_TYPE_to_string(enum_val);
}

const char*
  SOC_PPD_DIAG_TCAM_USAGE_to_string(
    SOC_SAND_IN  SOC_PPD_DIAG_TCAM_USAGE enum_val
  )
{
  return SOC_PPC_DIAG_TCAM_USAGE_to_string(enum_val);
}

const char*
  SOC_PPD_DIAG_FLAVOR_to_string(
    SOC_SAND_IN  SOC_PPD_DIAG_FLAVOR enum_val
  )
{
  return SOC_PPC_DIAG_FLAVOR_to_string(enum_val);
}

const char*
  SOC_PPD_DIAG_LEM_LKUP_TYPE_to_string(
    SOC_SAND_IN  SOC_PPD_DIAG_LEM_LKUP_TYPE enum_val
  )
{
  return SOC_PPC_DIAG_LEM_LKUP_TYPE_to_string(enum_val);
}

const char*
  SOC_PPD_DIAG_LIF_LKUP_TYPE_to_string(
    SOC_SAND_IN  SOC_PPD_DIAG_LIF_LKUP_TYPE enum_val
  )
{
  return SOC_PPC_DIAG_LIF_LKUP_TYPE_to_string(enum_val);
}

const char*
  SOC_PPD_DIAG_DB_TYPE_to_string(
    SOC_SAND_IN  SOC_PPD_DIAG_DB_TYPE enum_val
  )
{
  return SOC_PPC_DIAG_DB_TYPE_to_string(enum_val);
}

const char*
  SOC_PPD_DIAG_FRWRD_DECISION_PHASE_to_string(
    SOC_SAND_IN  SOC_PPD_DIAG_FRWRD_DECISION_PHASE enum_val
  )
{
  return SOC_PPC_DIAG_FRWRD_DECISION_PHASE_to_string(enum_val);
}

const char*
  SOC_PPD_DIAG_MPLS_TERM_TYPE_to_string(
    SOC_SAND_IN  SOC_PPD_DIAG_MPLS_TERM_TYPE enum_val
  )
{
  return SOC_PPC_DIAG_MPLS_TERM_TYPE_to_string(enum_val);
}

const char*
  SOC_PPD_DIAG_PKT_TRACE_to_string(
    SOC_SAND_IN  SOC_PPD_DIAG_PKT_TRACE enum_val
  )
{
  return SOC_PPC_DIAG_PKT_TRACE_to_string(enum_val);
}

const char*
  SOC_PPD_DIAG_EG_DROP_REASON_to_string(
    SOC_SAND_IN  SOC_PPD_DIAG_EG_DROP_REASON enum_val
  )
{
  return SOC_PPC_DIAG_EG_DROP_REASON_to_string(enum_val);
}

const char*
  SOC_PPD_DIAG_PKT_TM_FIELD_to_string(
    SOC_SAND_IN  SOC_PPD_DIAG_PKT_TM_FIELD enum_val
  )
{
  return SOC_PPC_DIAG_PKT_TM_FIELD_to_string(enum_val);
}

void
  SOC_PPD_DIAG_BUFFER_print(
    SOC_SAND_IN  SOC_PPD_DIAG_BUFFER *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_DIAG_BUFFER_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_DIAG_MODE_INFO_print(
    SOC_SAND_IN  SOC_PPD_DIAG_MODE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_DIAG_MODE_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_DIAG_VSI_INFO_print(
    SOC_SAND_IN  SOC_PPD_DIAG_VSI_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_DIAG_VSI_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_DIAG_DB_USE_INFO_print(
    SOC_SAND_IN  SOC_PPD_DIAG_DB_USE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_DIAG_DB_USE_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_DIAG_TCAM_USE_INFO_print(
    SOC_SAND_IN  SOC_PPD_DIAG_TCAM_USE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_DIAG_TCAM_USE_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_DIAG_RECEIVED_PACKET_INFO_print(
    SOC_SAND_IN  SOC_PPD_DIAG_RECEIVED_PACKET_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_DIAG_RECEIVED_PACKET_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_DIAG_TRAP_PACKET_INFO_print(
    SOC_SAND_IN  SOC_PPD_DIAG_TRAP_PACKET_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_DIAG_TRAP_PACKET_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_DIAG_TRAP_INFO_print(
    SOC_SAND_IN  SOC_PPD_DIAG_TRAP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_DIAG_TRAP_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_DIAG_SNOOP_INFO_print(
    SOC_SAND_IN  SOC_PPD_DIAG_SNOOP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_DIAG_SNOOP_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_DIAG_TRAPS_INFO_print(
    SOC_SAND_IN  SOC_PPD_DIAG_TRAPS_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_DIAG_TRAPS_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_DIAG_TRAP_TO_CPU_INFO_print(
    SOC_SAND_IN  SOC_PPD_DIAG_TRAP_TO_CPU_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_DIAG_TRAP_TO_CPU_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_DIAG_LEM_KEY_print(
    SOC_SAND_IN  SOC_PPD_DIAG_LEM_KEY *info,
    SOC_SAND_IN  SOC_PPD_DIAG_LEM_LKUP_TYPE type
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_DIAG_LEM_KEY_print(info,type);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_DIAG_LIF_KEY_print(
    SOC_SAND_IN  SOC_PPD_DIAG_LIF_KEY *info,
    SOC_SAND_IN  SOC_PPD_DIAG_LIF_LKUP_TYPE lkup_type
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_DIAG_LIF_KEY_print(info,lkup_type);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_DIAG_LIF_VALUE_print(
    SOC_SAND_IN  SOC_PPD_DIAG_LIF_VALUE *info,
    SOC_SAND_IN  SOC_PPD_DIAG_LIF_LKUP_TYPE lkup_type
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_DIAG_LIF_VALUE_print(info,lkup_type);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_DIAG_LIF_LKUP_INFO_print(
    SOC_SAND_IN  SOC_PPD_DIAG_LIF_LKUP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_DIAG_LIF_LKUP_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_DIAG_TCAM_KEY_print(
    SOC_SAND_IN  SOC_PPD_DIAG_TCAM_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_DIAG_TCAM_KEY_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_DIAG_TCAM_VALUE_print(
    SOC_SAND_IN  SOC_PPD_DIAG_TCAM_VALUE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_DIAG_TCAM_VALUE_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_DIAG_LEM_VALUE_print(
    SOC_SAND_IN  SOC_PPD_DIAG_LEM_VALUE *info,
    SOC_SAND_IN  SOC_PPD_DIAG_LEM_LKUP_TYPE type
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_DIAG_LEM_VALUE_print(info,type);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_DIAG_PARSING_L2_INFO_print(
    SOC_SAND_IN  SOC_PPD_DIAG_PARSING_L2_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_DIAG_PARSING_L2_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_DIAG_PARSING_MPLS_INFO_print(
    SOC_SAND_IN  SOC_PPD_DIAG_PARSING_MPLS_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_DIAG_PARSING_MPLS_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_DIAG_PARSING_IP_INFO_print(
    SOC_SAND_IN  SOC_PPD_DIAG_PARSING_IP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_DIAG_PARSING_IP_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_DIAG_PARSING_HEADER_INFO_print(
    SOC_SAND_IN  SOC_PPD_DIAG_PARSING_HEADER_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_DIAG_PARSING_HEADER_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_DIAG_PARSING_INFO_print(
    SOC_SAND_IN  SOC_PPD_DIAG_PARSING_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_DIAG_PARSING_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_DIAG_TERM_MPLS_LABEL_INFO_print(
    SOC_SAND_IN  SOC_PPD_DIAG_TERM_MPLS_LABEL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_DIAG_TERM_MPLS_LABEL_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_DIAG_TERMINATED_MPLS_LABEL_INFO_print(
    SOC_SAND_IN  SOC_PPD_DIAG_TERMINATED_MPLS_LABEL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_DIAG_TERMINATED_MPLS_LABEL_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_DIAG_TERM_MPLS_INFO_print(
    SOC_SAND_IN  SOC_PPD_DIAG_TERM_MPLS_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_DIAG_TERM_MPLS_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_DIAG_TERM_INFO_print(
    SOC_SAND_IN  SOC_PPD_DIAG_TERM_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_DIAG_TERM_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_DIAG_IPV4_VPN_KEY_print(
    SOC_SAND_IN  SOC_PPD_DIAG_IPV4_VPN_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_DIAG_IPV4_VPN_KEY_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_DIAG_IPV6_VPN_KEY_print(
    SOC_SAND_IN  SOC_PPD_DIAG_IPV6_VPN_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_DIAG_IPV6_VPN_KEY_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_DIAG_FRWRD_LKUP_KEY_print(
    SOC_SAND_IN  SOC_PPD_DIAG_FRWRD_LKUP_KEY *info,
    SOC_SAND_IN  SOC_PPD_DIAG_FWD_LKUP_TYPE  frwrd_type,
    SOC_SAND_IN  uint8                       is_kbp
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_DIAG_FRWRD_LKUP_KEY_print(info,frwrd_type, is_kbp);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_DIAG_FRWRD_LKUP_VALUE_print(
    SOC_SAND_IN  SOC_PPD_DIAG_FRWRD_LKUP_VALUE *info,
    SOC_SAND_IN  SOC_PPD_DIAG_FWD_LKUP_TYPE  frwrd_type
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_DIAG_FRWRD_LKUP_VALUE_print(info,frwrd_type);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_DIAG_FRWRD_LKUP_INFO_print(
    SOC_SAND_IN  SOC_PPD_DIAG_FRWRD_LKUP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_DIAG_FRWRD_LKUP_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_DIAG_LEARN_INFO_print(
    SOC_SAND_IN  SOC_PPD_DIAG_LEARN_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_DIAG_LEARN_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_DIAG_VLAN_EDIT_RES_print(
    SOC_SAND_IN  SOC_PPD_DIAG_VLAN_EDIT_RES *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_DIAG_VLAN_EDIT_RES_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_DIAG_FRWRD_DECISION_INFO_print(
    SOC_SAND_IN  SOC_PPD_DIAG_FRWRD_DECISION_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_DIAG_FRWRD_DECISION_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_DIAG_FRWRD_DECISION_TRACE_INFO_print(
    SOC_SAND_IN  SOC_PPD_DIAG_FRWRD_DECISION_TRACE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_DIAG_FRWRD_DECISION_TRACE_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_DIAG_PKT_TM_INFO_print(
    SOC_SAND_IN  SOC_PPD_DIAG_PKT_TM_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_DIAG_PKT_TM_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_DIAG_ENCAP_INFO_print(
    SOC_SAND_IN  SOC_PPD_DIAG_ENCAP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_DIAG_ENCAP_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_DIAG_EG_DROP_LOG_INFO_print(
    SOC_SAND_IN  SOC_PPD_DIAG_EG_DROP_LOG_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_DIAG_EG_DROP_LOG_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_DIAG_ETH_PACKET_INFO_print(
    SOC_SAND_IN  SOC_PPD_DIAG_ETH_PACKET_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_DIAG_ETH_PACKET_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_DIAG_PACKET_INFO_print(
    SOC_SAND_IN  SOC_PPD_DIAG_PACKET_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_DIAG_PACKET_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void 
  SOC_PPD_DIAG_EGRESS_VLAN_EDIT_INFO_print(
     SOC_SAND_IN SOC_PPD_DIAG_EGRESS_VLAN_EDIT_RES *prm_vec_res)
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(prm_vec_res);

  SOC_PPC_DIAG_EGRESS_VLAN_EDIT_INFO_print(prm_vec_res);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void 
  SOC_PPD_DIAG_LIF_PROFILE_INFO_print(
     SOC_SAND_IN SOC_PPD_DIAG_LIF_PROFILE_INFO *info
  ) {
	SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
	SOC_SAND_CHECK_NULL_INPUT(info);

	SOC_PPC_DIAG_LIF_PROFILE_INFO_print(info);
  exit:
	SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif /* SOC_PPD_DEBUG_IS_LVL1 */

#if SOC_PPD_DEBUG_IS_LVL3

void
  soc_ppd_diag_sample_enable_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint8                               enable
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "enable: %u\n\r"),enable));

  return;
}
void
  soc_ppd_diag_sample_enable_get_print(
    SOC_SAND_IN  int                               unit
  )
{

  return;
}
void
  soc_ppd_diag_mode_info_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_DIAG_MODE_INFO                      *mode_info
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "mode_info:")));
  SOC_PPD_DIAG_MODE_INFO_print((mode_info));

  return;
}
void
  soc_ppd_diag_mode_info_get_print(
    SOC_SAND_IN  int                               unit
  )
{

  return;
}
void
  soc_ppd_diag_pkt_trace_clear_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_DIAG_PKT_TRACE                      pkt_trace
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "pkt_trace %s "), SOC_PPD_DIAG_PKT_TRACE_to_string(pkt_trace)));

  return;
}
void
  soc_ppd_diag_received_packet_info_get_print(
    SOC_SAND_IN  int                               unit
  )
{

  return;
}
void
  soc_ppd_diag_parsing_info_get_print(
    SOC_SAND_IN  int                               unit
  )
{

  return;
}
void
  soc_ppd_diag_termination_info_get_print(
    SOC_SAND_IN  int                               unit
  )
{

  return;
}
void
  soc_ppd_diag_frwrd_lkup_info_get_print(
    SOC_SAND_IN  int                               unit
  )
{

  return;
}
void
  soc_ppd_diag_frwrd_lpm_lkup_get_print(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PPD_DIAG_IPV4_VPN_KEY              *lpm_key
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "lpm_key:")));
  SOC_PPD_DIAG_IPV4_VPN_KEY_print((lpm_key));

  return;
}
void
  soc_ppd_diag_traps_info_get_print(
    SOC_SAND_IN  int                               unit
  )
{

  return;
}
void
  soc_ppd_diag_trapped_packet_info_get_print(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_INOUT uint8                            *buff,
    SOC_SAND_IN  uint32                           max_size
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "buff: %u\n\r"), *buff));

  LOG_CLI((BSL_META_U(unit,
                      "max_size: %lu\n\r"),max_size));

  return;
}
void
  soc_ppd_diag_traps_all_to_cpu_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_DIAG_TRAP_TO_CPU_INFO               *trap_info
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "trap_info:")));
  SOC_PPD_DIAG_TRAP_TO_CPU_INFO_print((trap_info));

  return;
}
void
  soc_ppd_diag_traps_stat_restore_print(
    SOC_SAND_IN  int                               unit
  )
{

  return;
}
void
  soc_ppd_diag_frwrd_decision_trace_get_print(
    SOC_SAND_IN  int                               unit
  )
{

  return;
}
void
  soc_ppd_diag_learning_info_get_print(
    SOC_SAND_IN  int                               unit
  )
{

  return;
}
void
  soc_ppd_diag_ing_vlan_edit_info_get_print(
    SOC_SAND_IN  int                               unit
  )
{

  return;
}
void
  soc_ppd_diag_pkt_associated_tm_info_get_print(
    SOC_SAND_IN  int                               unit
  )
{

  return;
}
void
  soc_ppd_diag_encap_info_get_print(
    SOC_SAND_IN  int                               unit
  )
{

  return;
}
void
  soc_ppd_diag_eg_drop_log_get_print(
    SOC_SAND_IN  int                               unit
  )
{

  return;
}
void
  soc_ppd_diag_db_lif_lkup_info_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_DIAG_DB_USE_INFO                    *db_info
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "db_info:")));
  SOC_PPD_DIAG_DB_USE_INFO_print((db_info));

  return;
}
void
  soc_ppd_diag_db_lem_lkup_info_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_DIAG_DB_USE_INFO                    *db_info
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "db_info:")));
  SOC_PPD_DIAG_DB_USE_INFO_print((db_info));

  return;
}
void
  soc_ppd_diag_db_tcam_lkup_info_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_DIAG_TCAM_USE_INFO                  *use_info
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "use_info:")));
  SOC_PPD_DIAG_TCAM_USE_INFO_print((use_info));

  return;
}
void
  soc_ppd_diag_pkt_send_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_DIAG_PACKET_INFO                    *pkt_info
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "pkt_info:")));
  SOC_PPD_DIAG_PACKET_INFO_print((pkt_info));

  return;
}



#endif /* SOC_PPD_DEBUG_IS_LVL3 */
/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

