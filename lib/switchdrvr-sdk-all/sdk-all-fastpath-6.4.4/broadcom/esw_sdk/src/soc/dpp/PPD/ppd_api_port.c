/* $Id: ppd_api_port.c,v 1.12 Broadcom SDK $
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
* FILENAME:       DuneDriver/ppd/src/soc_ppd_api_port.c
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
#include <soc/dpp/PPD/ppd_api_port.h>
#ifdef LINK_PB_LIBRARIES
  #include <soc/dpp/Petra/PB_PP/pb_pp_api_port.h>
#endif
#ifdef LINK_ARAD_LIBRARIES
  #include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_port.h>
#endif
#ifdef LINK_T20E_LIBRARIES
  #include <soc/dpp/T20E/t20e_api_port.h>
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
    Ppd_procedure_desc_element_port[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_PORT_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_PORT_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_PORT_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_PORT_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_PORT_STP_STATE_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_PORT_STP_STATE_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_PORT_STP_STATE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_PORT_STP_STATE_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_PORT_LOCAL_PORT_TO_SYS_PHY_MAP_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_PORT_LOCAL_PORT_TO_SYS_PHY_MAP_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_PORT_LOCAL_PORT_TO_SYS_PHY_MAP_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_PORT_LOCAL_PORT_TO_SYS_PHY_MAP_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_PORT_TM_TO_PP_MAP_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_PORT_TM_TO_PP_MAP_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_PORT_TM_TO_PP_MAP_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_PORT_TM_TO_PP_MAP_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_PORT_GET_PROCS_PTR),
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
*     Set main attributes of the port. Generally, these
 *     attributes identify the port and may have use in more
 *     than one module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_port_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  int                               core_id,
    SOC_SAND_IN  SOC_PPD_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_PPD_PORT_INFO                           *port_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_PORT_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(port_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(port_info_set,(unit, core_id, local_port_ndx, port_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_port_info_set_print,(unit,local_port_ndx,port_info));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_port_info_set()", local_port_ndx, 0);
}

/*********************************************************************
*     Set main attributes of the port. Generally, these
 *     attributes identify the port and may have use in more
 *     than one module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_port_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  int                               core_id,
    SOC_SAND_IN  SOC_PPD_PORT                                local_port_ndx,
    SOC_SAND_OUT SOC_PPD_PORT_INFO                           *port_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_PORT_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(port_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(port_info_get,(unit, core_id, local_port_ndx, port_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_port_info_get_print,(unit,local_port_ndx));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_port_info_get()", local_port_ndx, 0);
}

/*********************************************************************
*     Sets the STP state of a port in a specific topology ID.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_port_stp_state_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  int                               core_id,
    SOC_SAND_IN  SOC_PPD_PORT                                local_port_ndx,
    SOC_SAND_IN  uint32                                topology_id_ndx,
    SOC_SAND_IN  SOC_PPD_PORT_STP_STATE                      stp_state
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_PORT_STP_STATE_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_TMP_DEVICE_CALL(port_stp_state_set,(unit, core_id, local_port_ndx, topology_id_ndx, stp_state));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_port_stp_state_set_print,(unit,local_port_ndx,topology_id_ndx,stp_state));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_port_stp_state_set()", local_port_ndx, topology_id_ndx);
}

/*********************************************************************
*     Sets the STP state of a port in a specific topology ID.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_port_stp_state_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  int                               core_id,
    SOC_SAND_IN  SOC_PPD_PORT                                local_port_ndx,
    SOC_SAND_IN  uint32                                topology_id_ndx,
    SOC_SAND_OUT SOC_PPD_PORT_STP_STATE                      *stp_state
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_PORT_STP_STATE_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(stp_state);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(port_stp_state_get,(unit, core_id, local_port_ndx, topology_id_ndx, stp_state));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_port_stp_state_get_print,(unit,local_port_ndx,topology_id_ndx));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_port_stp_state_get()", local_port_ndx, topology_id_ndx);
}

/*********************************************************************
*     Map Local port to System Physical Port. The mapping is
 *     unique - single System Physical Port is mapped to a
 *     single local port per specified device. This also
 *     performs the opposite mapping from system port to local
 *     port. This configuration effects Learnt system port and
 *     forwarding.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_port_local_port_to_sys_phy_map_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_PORT                                local_port_ndx,
    SOC_SAND_IN  uint32                                sys_phy_port_id
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_PORT_LOCAL_PORT_TO_SYS_PHY_MAP_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

 switch (SOC_SAND_DEVICE_TYPE_GET(unit))
  {
    case SOC_SAND_DEV_T20E:
      SOC_T20E_DEVICE_CALL(port_local_port_to_sys_phy_map_set,(unit, local_port_ndx, sys_phy_port_id));
      break;
    case SOC_SAND_DEV_PB:
      /*
       *	Not supported for Soc_petra-B.
       *  Use soc_petra_sys_phys_to_local_port_map_set instead
       */
      SOC_SAND_SET_ERROR_CODE(SOC_PPD_FUNC_UNSUPPORTED_ERR, 10, exit);
      break;
    default:
      SOC_SAND_SET_ERROR_CODE(SOC_PPD_INVALID_DEVICE_TYPE_ERR, 12, exit);
      break;
  }

exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_port_local_port_to_sys_phy_map_set_print,(unit,local_port_ndx,sys_phy_port_id));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_port_local_port_to_sys_phy_map_set()", local_port_ndx, 0);
}

/*********************************************************************
*     Map Local port to System Physical Port. The mapping is
 *     unique - single System Physical Port is mapped to a
 *     single local port per specified device. This also
 *     performs the opposite mapping from system port to local
 *     port. This configuration effects Learnt system port and
 *     forwarding.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_port_local_port_to_sys_phy_map_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_PORT                                local_port_ndx,
    SOC_SAND_OUT uint32                                *sys_phy_port_id
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_PORT_LOCAL_PORT_TO_SYS_PHY_MAP_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;
  SOC_SAND_CHECK_NULL_INPUT(sys_phy_port_id);

  switch (SOC_SAND_DEVICE_TYPE_GET(unit))
  {
    case SOC_SAND_DEV_T20E:
      SOC_T20E_DEVICE_CALL(port_local_port_to_sys_phy_map_get,(unit, local_port_ndx, sys_phy_port_id));
      break;
    case SOC_SAND_DEV_PB:
      /*
       *	Not supported for Soc_petra-B.
       *  Use soc_petra_sys_phys_to_local_port_map_get instead
       */
      SOC_SAND_SET_ERROR_CODE(SOC_PPD_FUNC_UNSUPPORTED_ERR, 10, exit);
      break;
    default:
      SOC_SAND_SET_ERROR_CODE(SOC_PPD_INVALID_DEVICE_TYPE_ERR, 12, exit);
      break;
  }

 exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_port_local_port_to_sys_phy_map_get_print,(unit,local_port_ndx));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_port_local_port_to_sys_phy_map_get()", local_port_ndx, 0);
}

/*********************************************************************
*     Set mapping from TM local port to PP local port.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_port_tm_to_pp_map_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_TM_PORT                             local_tm_port_ndx,
    SOC_SAND_IN  SOC_PPD_PORT_DIRECTION                      direction_ndx,
    SOC_SAND_IN  SOC_PPD_PORT                                local_pp_port
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_PORT_TM_TO_PP_MAP_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_DEVICE_CALL(port_tm_to_pp_map_set,(unit, local_tm_port_ndx, direction_ndx, local_pp_port));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_port_tm_to_pp_map_set_print,(unit,local_tm_port_ndx,direction_ndx,local_pp_port));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_port_tm_to_pp_map_set()", local_tm_port_ndx, 0);
}

/*********************************************************************
*     Set mapping from TM local port to PP local port.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_port_tm_to_pp_map_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_TM_PORT                             local_tm_port_ndx,
    SOC_SAND_OUT SOC_PPD_PORT                                *local_pp_port_in,
    SOC_SAND_OUT SOC_PPD_PORT                                *local_pp_port_out
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_PORT_TM_TO_PP_MAP_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(local_pp_port_in);
  SOC_SAND_CHECK_NULL_INPUT(local_pp_port_out);

  SOC_PPD_DEVICE_CALL(port_tm_to_pp_map_get,(unit, local_tm_port_ndx, local_pp_port_in, local_pp_port_out));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_port_tm_to_pp_map_get_print,(unit,local_tm_port_ndx,0));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_port_tm_to_pp_map_get()", local_tm_port_ndx, 0);
}

/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     soc_ppd_api_port module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_PROCEDURE_DESC_ELEMENT*
  soc_ppd_port_get_procs_ptr(
  )
{
  return Ppd_procedure_desc_element_port;
}
void
  SOC_PPD_PORT_EXTEND_P2P_INFO_clear(
    SOC_SAND_OUT SOC_PPD_PORT_EXTEND_P2P_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_PORT_EXTEND_P2P_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_PORT_INFO_clear(
    SOC_SAND_OUT SOC_PPD_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_PORT_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PPD_DEBUG_IS_LVL1

const char*
  SOC_PPD_PORT_DIRECTION_to_string(
    SOC_SAND_IN  SOC_PPD_PORT_DIRECTION enum_val
  )
{
  return SOC_PPC_PORT_DIRECTION_to_string(enum_val);
}

const char*
  SOC_PPD_PORT_STP_STATE_to_string(
    SOC_SAND_IN  SOC_PPD_PORT_STP_STATE enum_val
  )
{
  return SOC_PPC_PORT_STP_STATE_to_string(enum_val);
}

void
  SOC_PPD_PORT_EXTEND_P2P_INFO_print(
    SOC_SAND_IN  SOC_PPD_PORT_EXTEND_P2P_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_PORT_EXTEND_P2P_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_PORT_INFO_print(
    SOC_SAND_IN  SOC_PPD_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_PORT_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif /* SOC_PPD_DEBUG_IS_LVL1 */

#if SOC_PPD_DEBUG_IS_LVL3

void
  soc_ppd_port_info_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_PPD_PORT_INFO                           *port_info
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "local_port_ndx: %lu\n\r"),local_port_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "port_info:")));
  SOC_PPD_PORT_INFO_print((port_info));

  return;
}
void
  soc_ppd_port_info_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_PORT                                local_port_ndx
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "local_port_ndx: %lu\n\r"),local_port_ndx));

  return;
}
void
  soc_ppd_port_stp_state_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_PORT                                local_port_ndx,
    SOC_SAND_IN  uint32                                topology_id_ndx,
    SOC_SAND_IN  SOC_PPD_PORT_STP_STATE                      stp_state
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "local_port_ndx: %lu\n\r"),local_port_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "topology_id_ndx: %lu\n\r"),topology_id_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "stp_state %s "), SOC_PPD_PORT_STP_STATE_to_string(stp_state)));

  return;
}
void
  soc_ppd_port_stp_state_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_PORT                                local_port_ndx,
    SOC_SAND_IN  uint32                                topology_id_ndx
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "local_port_ndx: %lu\n\r"),local_port_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "topology_id_ndx: %lu\n\r"),topology_id_ndx));

  return;
}
void
  soc_ppd_port_local_port_to_sys_phy_map_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_PORT                                local_port_ndx,
    SOC_SAND_IN  uint32                                sys_phy_port_id
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "local_port_ndx: %lu\n\r"),local_port_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "sys_phy_port_id: %lu\n\r"),sys_phy_port_id));

  return;
}
void
  soc_ppd_port_local_port_to_sys_phy_map_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_PORT                                local_port_ndx
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "local_port_ndx: %lu\n\r"),local_port_ndx));

  return;
}
void
  soc_ppd_port_tm_to_pp_map_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_TM_PORT                             local_tm_port_ndx,
    SOC_SAND_IN  SOC_PPD_PORT_DIRECTION                      direction_ndx,
    SOC_SAND_IN  SOC_PPD_PORT                                local_pp_port
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "local_tm_port_ndx: %lu\n\r"),local_tm_port_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "direction_ndx %s "), SOC_PPD_PORT_DIRECTION_to_string(direction_ndx)));

  LOG_CLI((BSL_META_U(unit,
                      "local_pp_port: %lu\n\r"),local_pp_port));

  return;
}
void
  soc_ppd_port_tm_to_pp_map_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_TM_PORT                             local_tm_port_ndx,
    SOC_SAND_IN  SOC_PPD_PORT_DIRECTION                      direction_ndx
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "local_tm_port_ndx: %lu\n\r"),local_tm_port_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "direction_ndx %s "), SOC_PPD_PORT_DIRECTION_to_string(direction_ndx)));

  return;
}
#endif /* SOC_PPD_DEBUG_IS_LVL3 */
/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

