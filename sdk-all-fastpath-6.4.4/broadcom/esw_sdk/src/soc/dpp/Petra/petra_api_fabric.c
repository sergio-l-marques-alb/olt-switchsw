/* $Id: petra_api_fabric.c,v 1.8 Broadcom SDK $
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
* FILENAME:       DuneDriver/soc_petra/src/soc_petra_api_fabric.c
*
* MODULE PREFIX:  soc_petra_fabric
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

#include <soc/dpp/Petra/petra_api_fabric.h>
#include <soc/dpp/Petra/petra_fabric.h>
#include <soc/dpp/Petra/petra_serdes.h>
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

/*********************************************************************
*     This procedure handles the SerDes configuration of a
*     SerDes quartet attached to the fabric interface. It is
*     called during fabric initialization sequence. Also, in
*     the case that a SerDes quartet is reset (e.g. in order
*     to change the SerDes rate), this API must be called to
*     first shut down all the relevant links (is_oor ==
*     FALSE), and then, after serdes quartet reset, with
*     is_oor == TRUE.
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_petra_fabric_srd_qrtt_reset(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 srd_qrtt_ndx,
    SOC_SAND_IN  uint8                 is_oor
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_FABRIC_SRD_QRTT_RESET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;


  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_fabric_srd_qrtt_reset_unsafe(
    unit,
    srd_qrtt_ndx,
    is_oor
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_fabric_srd_qrtt_reset()",0,0);
}

/*********************************************************************
*     This procedure enables/disables flow-control on fabric
*     links.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_fabric_fc_enable_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION direction_ndx,
    SOC_SAND_IN  SOC_PETRA_FABRIC_FC           *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_FABRIC_FC_ENABLE_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_petra_fabric_fc_enable_verify(
          unit,
          direction_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_fabric_fc_enable_set_unsafe(
          unit,
          direction_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_fabric_fc_enable_set()",0,0);
}

/*********************************************************************
*     This procedure enables/disables flow-control on fabric
*     links.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_fabric_fc_enable_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_FABRIC_FC           *info_rx,
    SOC_SAND_OUT SOC_PETRA_FABRIC_FC           *info_tx
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_FABRIC_FC_ENABLE_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info_rx);
  SOC_SAND_CHECK_NULL_INPUT(info_tx);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_fabric_fc_enable_get_unsafe(
          unit,
          info_rx,
          info_tx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_fabric_fc_enable_get()",0,0);
}


/*********************************************************************
*     This procedure sets fabric links operation mode.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_fabric_cell_format_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_FABRIC_CELL_FORMAT  *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_FABRIC_CELL_FORMAT_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_fabric_cell_format_get_unsafe(
    unit,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_fabric_cell_format_get()",0,0);
}

/*********************************************************************
*     Sets coexist parameters in order to distribute traffic
*     more evenly over links. Coexist mode is a mode in which
*     the fabric connects a soc_petra device to fap20/21 device.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_fabric_coexist_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FABRIC_COEXIST_INFO *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_FABRIC_COEXIST_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_petra_fabric_coexist_verify(
    unit,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_fabric_coexist_set_unsafe(
    unit,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_fabric_coexist_set()",0,0);
}

/*********************************************************************
*     Sets coexist parameters in order to distribute traffic
*     more evenly over links. Coexist mode is a mode in which
*     the fabric connects a soc_petra device to fap20/21 device.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_fabric_coexist_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_FABRIC_COEXIST_INFO *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_FABRIC_COEXIST_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_fabric_coexist_get_unsafe(
    unit,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_fabric_coexist_get()",0,0);
}

/*********************************************************************
*     Configures a FAP to be in stand_alone mode (internal
*     switching). to the presence of FE in the system. If the
*     system is designed to be with ONE fap: - set STANDALONE
*     mode constantly. If the system is designed to be with
*     more than one FAP: - if it starts from ONE fap, set the
*     FAP to STANDALONE mode. - when at least one more FAP is
*     in the system, unset the mode STANDALONE mode. - if the
*     system is with many FAPs, and all FAPs (but one) are
*     removed, set the remaining FAP to be in STANDALONE mode
*     (again, change it when another FAP is in).
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_fabric_stand_alone_fap_mode_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT uint8                 *is_single_fap_mode
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_FABRIC_STAND_ALONE_FAP_MODE_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(is_single_fap_mode);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_fabric_stand_alone_fap_mode_get_unsafe(
    unit,
    is_single_fap_mode
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_fabric_stand_alone_fap_mode_get()",0,0);
}

/*********************************************************************
*     Configure the fabric mode to work in one of the
*     following modes: FE, back to back, mesh or multi stage
*     FE.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_fabric_connect_mode_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FABRIC_CONNECT_MODE fabric_mode
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_FABRIC_CONNECT_MODE_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;


  res = soc_petra_fabric_connect_mode_verify(
    unit,
    fabric_mode
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_fabric_connect_mode_set_unsafe(
    unit,
    fabric_mode
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_fabric_connect_mode_set()",0,0);
}

/*********************************************************************
*     Configure the fabric mode to work in one of the
*     following modes: FE, back to back, mesh or multi stage
*     FE.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_fabric_connect_mode_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_FABRIC_CONNECT_MODE *fabric_mode
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_FABRIC_CONNECT_MODE_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(fabric_mode);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_fabric_connect_mode_get_unsafe(
          unit,
          fabric_mode
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_fabric_connect_mode_get()",0,0);
}

/*********************************************************************
*     Set/unset fap20 device identification. Cells to fap20
*     devices have different format.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_fabric_fap20_map_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 other_device_ndx,
    SOC_SAND_IN  uint8                 is_fap20_device
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_FABRIC_FAP20_MAP_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;


  res = soc_petra_fabric_fap20_map_verify(
    unit,
    other_device_ndx,
    is_fap20_device
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_fabric_fap20_map_set_unsafe(
    unit,
    other_device_ndx,
    is_fap20_device
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_fabric_fap20_map_set()",0,0);
}

/*********************************************************************
*     Set/unset fap20 device identification. Cells to fap20
*     devices have different format.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_fabric_fap20_map_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 other_device_ndx,
    SOC_SAND_OUT uint8                 *is_fap20_device
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_FABRIC_FAP20_MAP_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(is_fap20_device);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_fabric_fap20_map_get_unsafe(
    unit,
    other_device_ndx,
    is_fap20_device
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_fabric_fap20_map_get()",0,0);
}

/*********************************************************************
*     Retrieve the connectivity map from the device.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_fabric_topology_status_connectivity_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                    link_index_min,
    SOC_SAND_IN  int                    link_index_max,
    SOC_SAND_OUT SOC_PETRA_FABRIC_LINKS_CON_STAT_INFO_ARR *connectivity_map
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_TOPOLOGY_STATUS_CONNECTIVITY_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(connectivity_map);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_fabric_topology_status_connectivity_get_unsafe(
    unit,
    link_index_min,
    link_index_max,
    connectivity_map
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_fabric_topology_status_connectivity_get()",0,0);
}

/*********************************************************************
*     Get Real-time lower level indications and errors of the
*     links
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_fabric_links_status_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 read_crc_counter,
    SOC_SAND_OUT SOC_PETRA_FABRIC_LINKS_STATUS_ALL    *links_status
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_LINKS_STATUS_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(links_status);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_fabric_links_status_get_unsafe(
    unit,
    read_crc_counter,
    links_status
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_fabric_links_status_get()",0,0);
}

/*********************************************************************
*     Set Fabric link, and optionally, the appropriate SerDes,
*     on/off state.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_link_on_off_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 link_ndx,
    SOC_SAND_IN  SOC_PETRA_LINK_STATE_INFO     *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_LINK_ON_OFF_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_petra_link_on_off_verify(
    unit,
    link_ndx,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_link_on_off_set_unsafe(
    unit,
    link_ndx,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_link_on_off_set()",0,0);
}

/*********************************************************************
*     Set Fabric link, and optionally, the appropriate SerDes,
*     on/off state.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_link_on_off_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 link_ndx,
    SOC_SAND_OUT SOC_PETRA_LINK_STATE_INFO     *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_LINK_ON_OFF_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_link_on_off_get_unsafe(
    unit,
    link_ndx,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_link_on_off_get()",0,0);
}

void
  soc_petra_PETRA_FABRIC_LINKS_CONNECT_MAP_STAT_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_FABRIC_LINKS_CONNECT_MAP_STAT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_FABRIC_LINKS_CONNECT_MAP_STAT_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_FABRIC_LINKS_CON_STAT_INFO_ARR_clear(
    SOC_SAND_OUT SOC_PETRA_FABRIC_LINKS_CON_STAT_INFO_ARR *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_FABRIC_LINKS_CON_STAT_INFO_ARR_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_FABRIC_CELL_FORMAT_clear(
    SOC_SAND_OUT SOC_PETRA_FABRIC_CELL_FORMAT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_FABRIC_CELL_FORMAT_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_FABRIC_FC_clear(
    SOC_SAND_IN uint32 unit,
    SOC_SAND_OUT SOC_PETRA_FABRIC_FC *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_FABRIC_FC_clear(unit, info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_FABRIC_COEXIST_INFO_clear(
    SOC_SAND_IN uint32 unit,
    SOC_SAND_OUT SOC_PETRA_FABRIC_COEXIST_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_FABRIC_COEXIST_INFO_clear(unit, info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_FABRIC_LINKS_STATUS_SINGLE_clear(
    SOC_SAND_OUT SOC_PETRA_FABRIC_LINKS_STATUS_SINGLE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_FABRIC_LINKS_STATUS_SINGLE_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_FABRIC_LINKS_STATUS_ALL_clear(
    SOC_SAND_IN uint32 unit,
    SOC_SAND_OUT SOC_PETRA_FABRIC_LINKS_STATUS_ALL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_FABRIC_LINKS_STATUS_ALL_clear(unit,info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_LINK_STATE_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_LINK_STATE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_LINK_STATE_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PETRA_DEBUG_IS_LVL1

/*********************************************************************
*     Print the connectivity map.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_fabric_topology_status_connectivity_print(
    SOC_SAND_IN uint32 unit,
    SOC_SAND_IN  SOC_PETRA_FABRIC_LINKS_CON_STAT_INFO_ARR *connectivity_map
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_TOPOLOGY_STATUS_CONNECTIVITY_PRINT);
  SOC_SAND_CHECK_NULL_INPUT(connectivity_map);

  soc_petra_PETRA_FABRIC_LINKS_CON_STAT_INFO_ARR_print(unit, connectivity_map, TRUE);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_fabric_topology_status_connectivity_print()",0,0);
}

const char*
  soc_petra_PETRA_LINK_ERR_TYPE_to_string(
    SOC_SAND_IN  SOC_PETRA_LINK_ERR_TYPE enum_val
  )
{
  return SOC_TMC_LINK_ERR_TYPE_to_string(enum_val);
}

const char*
  soc_petra_PETRA_LINK_INDICATE_TYPE_to_string(
    SOC_SAND_IN  SOC_PETRA_LINK_INDICATE_TYPE enum_val
  )
{
  return SOC_TMC_LINK_INDICATE_TYPE_to_string(enum_val);
}

const char*
  soc_petra_PETRA_FABRIC_CONNECT_MODE_to_string(
    SOC_SAND_IN  SOC_PETRA_FABRIC_CONNECT_MODE enum_val
  )
{
  return SOC_TMC_FABRIC_CONNECT_MODE_to_string(enum_val);
}

const char*
  soc_petra_PETRA_LINK_STATE_to_string(
    SOC_SAND_IN  SOC_PETRA_LINK_STATE enum_val
  )
{
  return SOC_TMC_LINK_STATE_to_string(enum_val);
}

const char*
  soc_petra_PETRA_FABRIC_LINE_CODING_to_string(
    SOC_SAND_IN  SOC_PETRA_FABRIC_LINE_CODING enum_val
  )
{
  return SOC_TMC_FABRIC_LINE_CODING_to_string(enum_val);
}


void
  soc_petra_PETRA_FABRIC_CELL_FORMAT_print(
    SOC_SAND_IN  SOC_PETRA_FABRIC_CELL_FORMAT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_FABRIC_CELL_FORMAT_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_FABRIC_FC_print(
    SOC_SAND_IN uint32 unit,
    SOC_SAND_IN  SOC_PETRA_FABRIC_FC *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_FABRIC_FC_print(unit, info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_FABRIC_COEXIST_INFO_print(
    SOC_SAND_IN uint32 unit,
    SOC_SAND_IN  SOC_PETRA_FABRIC_COEXIST_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_FABRIC_COEXIST_INFO_print(unit, info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_FABRIC_LINKS_CONNECT_MAP_STAT_INFO_print(
    SOC_SAND_IN SOC_PETRA_FABRIC_LINKS_CONNECT_MAP_STAT_INFO *info,
    SOC_SAND_IN uint8                         table_format
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_FABRIC_LINKS_CONNECT_MAP_STAT_INFO_print(info, table_format);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_FABRIC_LINKS_CON_STAT_INFO_ARR_print(
    SOC_SAND_IN uint32 unit,
    SOC_SAND_IN SOC_PETRA_FABRIC_LINKS_CON_STAT_INFO_ARR *info,
    SOC_SAND_IN uint8                     table_format
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_FABRIC_LINKS_CON_STAT_INFO_ARR_print(unit, info, table_format);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_FABRIC_LINKS_STATUS_ALL_print(
    SOC_SAND_IN uint32 unit,
    SOC_SAND_IN SOC_PETRA_FABRIC_LINKS_STATUS_ALL *info,
    SOC_SAND_IN SOC_SAND_PRINT_FLAVORS     print_flavour
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_FABRIC_LINKS_STATUS_ALL_print(unit, info, print_flavour);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_LINK_STATE_INFO_print(
    SOC_SAND_IN  SOC_PETRA_LINK_STATE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_LINK_STATE_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif /* SOC_PETRA_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

