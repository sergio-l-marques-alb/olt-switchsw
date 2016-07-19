/* $Id: ppd_api_mymac.c,v 1.13 Broadcom SDK $
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
* FILENAME:       DuneDriver/ppd/src/soc_ppd_api_mymac.c
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
#include <soc/dpp/PPD/ppd_api_mymac.h>
#ifdef LINK_PB_LIBRARIES
  #include <soc/dpp/Petra/PB_PP/pb_pp_api_mymac.h>
#endif
#ifdef LINK_T20E_LIBRARIES
  #include <soc/dpp/T20E/t20e_api_mymac.h>
#endif
#ifdef LINK_ARAD_LIBRARIES
  #include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_mymac.h>
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
    Ppd_procedure_desc_element_mymac[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_MYMAC_MSB_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_MYMAC_MSB_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_MYMAC_MSB_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_MYMAC_MSB_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_MYMAC_VSI_LSB_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_MYMAC_VSI_LSB_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_MYMAC_VSI_LSB_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_MYMAC_VSI_LSB_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_MYMAC_VRRP_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_MYMAC_VRRP_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_MYMAC_VRRP_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_MYMAC_VRRP_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_MYMAC_VRRP_MAC_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_MYMAC_VRRP_MAC_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_MYMAC_VRRP_MAC_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_MYMAC_VRRP_MAC_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_MYMAC_TRILL_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_MYMAC_TRILL_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_MYMAC_TRILL_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_MYMAC_TRILL_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LIF_MY_BMAC_MSB_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LIF_MY_BMAC_MSB_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LIF_MY_BMAC_MSB_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LIF_MY_BMAC_MSB_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LIF_MY_BMAC_PORT_LSB_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LIF_MY_BMAC_PORT_LSB_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LIF_MY_BMAC_PORT_LSB_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LIF_MY_BMAC_PORT_LSB_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_MYMAC_GET_PROCS_PTR),
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
*     Set the MSB of the MAC address of the device. Used for
 *     ingress termination and egress encapsulation.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_mymac_msb_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                     *my_mac_msb
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_MYMAC_MSB_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(my_mac_msb);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(mymac_msb_set,(unit, my_mac_msb));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_mymac_msb_set_print,(unit,my_mac_msb));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_mymac_msb_set()", 0, 0);
}

/*********************************************************************
*     Set the MSB of the MAC address of the device. Used for
 *     ingress termination and egress encapsulation.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_mymac_msb_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT SOC_SAND_PP_MAC_ADDRESS                     *my_mac_msb
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_MYMAC_MSB_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(my_mac_msb);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(mymac_msb_get,(unit, my_mac_msb));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_mymac_msb_get_print,(unit));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_mymac_msb_get()", 0, 0);
}

/*********************************************************************
*     Set MAC address LSB according to VSI.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_mymac_vsi_lsb_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_VSI_ID                              vsi_ndx,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                     *my_mac_lsb
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_MYMAC_VSI_LSB_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(my_mac_lsb);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(mymac_vsi_lsb_set,(unit, vsi_ndx, my_mac_lsb));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_mymac_vsi_lsb_set_print,(unit,vsi_ndx,my_mac_lsb));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_mymac_vsi_lsb_set()", vsi_ndx, 0);
}

/*********************************************************************
*     Set MAC address LSB according to VSI.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_mymac_vsi_lsb_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_VSI_ID                              vsi_ndx,
    SOC_SAND_OUT SOC_SAND_PP_MAC_ADDRESS                     *my_mac_lsb
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_MYMAC_VSI_LSB_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(my_mac_lsb);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(mymac_vsi_lsb_get,(unit, vsi_ndx, my_mac_lsb));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_mymac_vsi_lsb_get_print,(unit,vsi_ndx));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_mymac_vsi_lsb_get()", vsi_ndx, 0);
}

/*********************************************************************
*     Set My-MAC according to Virtual Router Redundancy
 *     Protocol.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_mymac_vrrp_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_MYMAC_VRRP_INFO                     *vrrp_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_MYMAC_VRRP_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(vrrp_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(mymac_vrrp_info_set,(unit, vrrp_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_mymac_vrrp_info_set_print,(unit,vrrp_info));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_mymac_vrrp_info_set()", 0, 0);
}

/*********************************************************************
*     Set My-MAC according to Virtual Router Redundancy
 *     Protocol.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_mymac_vrrp_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT SOC_PPD_MYMAC_VRRP_INFO                     *vrrp_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_MYMAC_VRRP_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(vrrp_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(mymac_vrrp_info_get,(unit, vrrp_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_mymac_vrrp_info_get_print,(unit));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_mymac_vrrp_info_get()", 0, 0);
}

/*********************************************************************
*     Enable / Disable MyMac/MyVRID according to VRRP-ID and
 *     Mac Address LSB.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_mymac_vrrp_mac_set(
    SOC_SAND_IN  int            unit,
    SOC_SAND_IN  uint32             vrrp_id_ndx,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS  *vrrp_mac_lsb_key,
    SOC_SAND_IN  uint8            enable
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_MYMAC_VRRP_MAC_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(vrrp_mac_lsb_key);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(mymac_vrrp_mac_set,(unit, vrrp_id_ndx, vrrp_mac_lsb_key, enable));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_mymac_vrrp_mac_set_print,(unit,vrrp_id_ndx,vrrp_mac_lsb_key,enable));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_mymac_vrrp_mac_set()", vrrp_id_ndx, 0);
}

/*********************************************************************
*     Enable / Disable MyMac/MyVRID according to VRRP-ID and
 *     Mac Address LSB.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_mymac_vrrp_mac_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32             vrrp_id_ndx,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS  *vrrp_mac_lsb_key,
    SOC_SAND_OUT uint8            *enable
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_MYMAC_VRRP_MAC_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(vrrp_mac_lsb_key);
  SOC_SAND_CHECK_NULL_INPUT(enable);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(mymac_vrrp_mac_get,(unit, vrrp_id_ndx, vrrp_mac_lsb_key, enable));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_mymac_vrrp_mac_get_print,(unit,vrrp_id_ndx,vrrp_mac_lsb_key));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_mymac_vrrp_mac_get()", vrrp_id_ndx, 0);
}

/*********************************************************************
*     Set TRILL My-Nick-Name and reserved Nick-Name.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_mymac_trill_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_MYMAC_TRILL_INFO                    *trill_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_MYMAC_TRILL_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(trill_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(mymac_trill_info_set,(unit, trill_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_mymac_trill_info_set_print,(unit,trill_info));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_mymac_trill_info_set()", 0, 0);
}

/*********************************************************************
*     Set TRILL My-Nick-Name and reserved Nick-Name.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_mymac_trill_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT SOC_PPD_MYMAC_TRILL_INFO                    *trill_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_MYMAC_TRILL_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(trill_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(mymac_trill_info_get,(unit, trill_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_mymac_trill_info_get_print,(unit));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_mymac_trill_info_get()", 0, 0);
}

/*********************************************************************
*     Set the MSB of the My-B-MAC. My-B-MAC forwards the
 *     packets to the I Component, and is added as the SA when
 *     sending toward the backbone network.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_lif_my_bmac_msb_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                     *my_bmac_msb
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_LIF_MY_BMAC_MSB_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(my_bmac_msb);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(lif_my_bmac_msb_set,(unit, my_bmac_msb));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_lif_my_bmac_msb_set_print,(unit,my_bmac_msb));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_lif_my_bmac_msb_set()", 0, 0);
}

/*********************************************************************
*     Set the MSB of the My-B-MAC. My-B-MAC forwards the
 *     packets to the I Component, and is added as the SA when
 *     sending toward the backbone network.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_lif_my_bmac_msb_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT SOC_SAND_PP_MAC_ADDRESS                     *my_bmac_msb
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_LIF_MY_BMAC_MSB_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(my_bmac_msb);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(lif_my_bmac_msb_get,(unit, my_bmac_msb));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_lif_my_bmac_msb_get_print,(unit));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_lif_my_bmac_msb_get()", 0, 0);
}

/*********************************************************************
*     Set the My-B-MAC LSB according to the source system
 *     port.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_lif_my_bmac_port_lsb_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_SAND_PP_SYS_PORT_ID                     *src_sys_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                     *my_mac_lsb,
    SOC_SAND_IN  uint8                               enable
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_LIF_MY_BMAC_PORT_LSB_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(src_sys_port_ndx);
  SOC_SAND_CHECK_NULL_INPUT(my_mac_lsb);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(lif_my_bmac_port_lsb_set,(unit, src_sys_port_ndx, my_mac_lsb, enable));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_lif_my_bmac_port_lsb_set_print,(unit,src_sys_port_ndx,my_mac_lsb,enable));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_lif_my_bmac_port_lsb_set()", 0, 0);
}

/*********************************************************************
*     Set the My-B-MAC LSB according to the source system
 *     port.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_lif_my_bmac_port_lsb_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_SAND_PP_SYS_PORT_ID                     *src_sys_port_ndx,
    SOC_SAND_INOUT SOC_SAND_PP_MAC_ADDRESS                     *my_mac_lsb,
    SOC_SAND_OUT uint8                               *enable
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_LIF_MY_BMAC_PORT_LSB_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(src_sys_port_ndx);
  SOC_SAND_CHECK_NULL_INPUT(my_mac_lsb);
  SOC_SAND_CHECK_NULL_INPUT(enable);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(lif_my_bmac_port_lsb_get,(unit, src_sys_port_ndx, my_mac_lsb, enable));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_lif_my_bmac_port_lsb_get_print,(unit,src_sys_port_ndx));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_lif_my_bmac_port_lsb_get()", 0, 0);
}

/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     soc_ppd_api_mymac module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_PROCEDURE_DESC_ELEMENT*
  soc_ppd_mymac_get_procs_ptr(
  )
{
  return Ppd_procedure_desc_element_mymac;
}
void
  SOC_PPD_MYMAC_VRRP_INFO_clear(
    SOC_SAND_OUT SOC_PPD_MYMAC_VRRP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_MYMAC_VRRP_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_MYMAC_TRILL_INFO_clear(
    SOC_SAND_OUT SOC_PPD_MYMAC_TRILL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_MYMAC_TRILL_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PPD_DEBUG_IS_LVL1

const char*
  SOC_PPD_MYMAC_VRRP_MODE_to_string(
    SOC_SAND_IN  SOC_PPD_MYMAC_VRRP_MODE enum_val
  )
{
  return SOC_PPC_MYMAC_VRRP_MODE_to_string(enum_val);
}

void
  SOC_PPD_MYMAC_VRRP_INFO_print(
    SOC_SAND_IN  SOC_PPD_MYMAC_VRRP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_MYMAC_VRRP_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_MYMAC_TRILL_INFO_print(
    SOC_SAND_IN  SOC_PPD_MYMAC_TRILL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_MYMAC_TRILL_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif /* SOC_PPD_DEBUG_IS_LVL1 */

#if SOC_PPD_DEBUG_IS_LVL3

void
  soc_ppd_mymac_msb_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                     *my_mac_msb
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "my_mac_msb:")));
  soc_sand_SAND_PP_MAC_ADDRESS_print((my_mac_msb));

  return;
}
void
  soc_ppd_mymac_msb_get_print(
    SOC_SAND_IN  int                               unit
  )
{

  return;
}
void
  soc_ppd_mymac_vsi_lsb_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_VSI_ID                              vsi_ndx,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                     *my_mac_lsb
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "vsi_ndx: %lu\n\r"),vsi_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "my_mac_lsb:")));
  soc_sand_SAND_PP_MAC_ADDRESS_print((my_mac_lsb));

  return;
}
void
  soc_ppd_mymac_vsi_lsb_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_VSI_ID                              vsi_ndx
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "vsi_ndx: %lu\n\r"),vsi_ndx));

  return;
}
void
  soc_ppd_mymac_vrrp_info_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_MYMAC_VRRP_INFO                     *vrrp_info
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "vrrp_info:")));
  SOC_PPD_MYMAC_VRRP_INFO_print((vrrp_info));

  return;
}
void
  soc_ppd_mymac_vrrp_info_get_print(
    SOC_SAND_IN  int                               unit
  )
{

  return;
}
void
  soc_ppd_mymac_vrrp_mac_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                vrrp_id_ndx,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS  *vrrp_mac_lsb_key,
    SOC_SAND_IN  uint8                               enable
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "vrrp_id_ndx: %lu\n\r"),vrrp_id_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "vrrp_mac_lsb_key:")));
  soc_sand_SAND_PP_MAC_ADDRESS_print((vrrp_mac_lsb_key));

  LOG_CLI((BSL_META_U(unit,
                      "enable: %u\n\r"),enable));

  return;
}
void
  soc_ppd_mymac_vrrp_mac_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32             vrrp_id_ndx,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS  *vrrp_mac_lsb_key
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "vrrp_id_ndx: %lu\n\r"),vrrp_id_ndx));
  LOG_CLI((BSL_META_U(unit,
                      "vrrp_mac_lsb_key:")));
  soc_sand_SAND_PP_MAC_ADDRESS_print((vrrp_mac_lsb_key));

  return;
}
void
  soc_ppd_mymac_trill_info_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_MYMAC_TRILL_INFO                    *trill_info
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "trill_info:")));
  SOC_PPD_MYMAC_TRILL_INFO_print((trill_info));

  return;
}
void
  soc_ppd_mymac_trill_info_get_print(
    SOC_SAND_IN  int                               unit
  )
{

  return;
}
void
  soc_ppd_lif_my_bmac_msb_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                     *my_bmac_msb
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "my_bmac_msb:")));
  soc_sand_SAND_PP_MAC_ADDRESS_print((my_bmac_msb));

  return;
}
void
  soc_ppd_lif_my_bmac_msb_get_print(
    SOC_SAND_IN  int                               unit
  )
{

  return;
}
void
  soc_ppd_lif_my_bmac_port_lsb_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_SAND_PP_SYS_PORT_ID                     *src_sys_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                     *my_mac_lsb,
    SOC_SAND_IN  uint8                               enable
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "src_sys_port_ndx:")));
  soc_sand_SAND_PP_SYS_PORT_ID_print((src_sys_port_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "my_mac_lsb:")));
  soc_sand_SAND_PP_MAC_ADDRESS_print((my_mac_lsb));

  LOG_CLI((BSL_META_U(unit,
                      "enable: %u\n\r"),enable));

  return;
}
void
  soc_ppd_lif_my_bmac_port_lsb_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_SAND_PP_SYS_PORT_ID                     *src_sys_port_ndx
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "src_sys_port_ndx:")));
  soc_sand_SAND_PP_SYS_PORT_ID_print((src_sys_port_ndx));

  return;
}
#endif /* SOC_PPD_DEBUG_IS_LVL3 */
/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

