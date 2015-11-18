/* $Id: pb_pp_api_lif.c,v 1.11 Broadcom SDK $
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
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_PP_/src/soc_pb_pp_api_lif.c
*
* MODULE PREFIX:  soc_pb_pp_pp
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
#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>

#include <soc/dpp/Petra/PB_PP/pb_pp_framework.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_api_lif.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_lif.h>

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
*     Set MPLS labels that may be used as in-vc-labels
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_l2_lif_pwe_map_range_set(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_IN_VC_RANGE                      *in_vc_range
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_L2_LIF_PWE_MAP_RANGE_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(in_vc_range);

  res = soc_pb_pp_l2_lif_pwe_map_range_set_verify(
          unit,
          in_vc_range
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_l2_lif_pwe_map_range_set_unsafe(
          unit,
          in_vc_range
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_l2_lif_pwe_map_range_set()", 0, 0);
}

/*********************************************************************
*     Set MPLS labels that may be used as in-vc-labels
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_l2_lif_pwe_map_range_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_OUT SOC_PB_PP_L2_LIF_IN_VC_RANGE                      *in_vc_range
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_L2_LIF_PWE_MAP_RANGE_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(in_vc_range);

  res = soc_pb_pp_l2_lif_pwe_map_range_get_verify(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_l2_lif_pwe_map_range_get_unsafe(
          unit,
          in_vc_range
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_l2_lif_pwe_map_range_get()", 0, 0);
}

/*********************************************************************
*     Enable an in VC label as a PWE handle, and set the PWE
 *     attributes
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_l2_lif_pwe_add(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                        in_vc_label,
    SOC_SAND_IN  SOC_PB_PP_LIF_ID                              lif_index,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_PWE_INFO                         *pwe_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                          *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_L2_LIF_PWE_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(pwe_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = soc_pb_pp_l2_lif_pwe_add_verify(
          unit,
          in_vc_label,
          lif_index,
          pwe_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_l2_lif_pwe_add_unsafe(
          unit,
          in_vc_label,
          lif_index,
          pwe_info,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_l2_lif_pwe_add()", 0, 0);
}

/*********************************************************************
*     Get PWE attributes and the LIF-index according to
 *     in_vc_label.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_l2_lif_pwe_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                            in_vc_label,
    SOC_SAND_OUT SOC_PB_PP_LIF_ID                                  *lif_index,
    SOC_SAND_OUT SOC_PB_PP_L2_LIF_PWE_INFO                         *pwe_info,
    SOC_SAND_OUT uint8                                     *found
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_L2_LIF_PWE_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(pwe_info);
  SOC_SAND_CHECK_NULL_INPUT(found);

  res = soc_pb_pp_l2_lif_pwe_get_verify(
          unit,
          in_vc_label
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_l2_lif_pwe_get_unsafe(
          unit,
          in_vc_label,
          lif_index,
          pwe_info,
          found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_l2_lif_pwe_get()", 0, 0);
}

/*********************************************************************
*     Remvoe the in-VC-label
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_l2_lif_pwe_remove(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                        in_vc_label,
    SOC_SAND_IN  uint8                                remove_from_sem_dummy,
    SOC_SAND_OUT uint32                                      *lif_index
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_L2_LIF_PWE_REMOVE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(lif_index);

  res = soc_pb_pp_l2_lif_pwe_remove_verify(
          unit,
          in_vc_label
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_l2_lif_pwe_remove_unsafe(
          unit,
          in_vc_label,
          lif_index
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_l2_lif_pwe_remove()", 0, 0);
}

/*********************************************************************
*     Enable VLAN compression when adding Attachment Circuits.
 *     Enables defining an AC according to VLAN domain and a
 *     range of VLANs, and not only according to port*VLAN
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_l2_lif_vlan_compression_add(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_L2_VLAN_RANGE_KEY                       *vlan_range_key,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                          *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_L2_LIF_VLAN_COMPRESSION_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(vlan_range_key);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = soc_pb_pp_l2_lif_vlan_compression_add_verify(
          unit,
          vlan_range_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_l2_lif_vlan_compression_add_unsafe(
          unit,
          vlan_range_key,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_l2_lif_vlan_compression_add()", 0, 0);
}

/*********************************************************************
*     Remove a VLAN range from the compressed VLAN ranges
 *     database
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_l2_lif_vlan_compression_remove(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_L2_VLAN_RANGE_KEY                       *vlan_range_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_L2_LIF_VLAN_COMPRESSION_REMOVE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(vlan_range_key);

  res = soc_pb_pp_l2_lif_vlan_compression_remove_verify(
          unit,
          vlan_range_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_l2_lif_vlan_compression_remove_unsafe(
          unit,
          vlan_range_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_l2_lif_vlan_compression_remove()", 0, 0);
}

/*********************************************************************
 *     Get all compressed VLAN ranges of a port
 *     Details: in the H file. (search for prototype)
 *********************************************************************/
uint32
  soc_pb_pp_l2_lif_vlan_compression_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                             local_port_ndx,
    SOC_SAND_OUT SOC_PB_PP_L2_PORT_VLAN_RANGE_INFO          *vlan_range_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_L2_LIF_VLAN_COMPRESSION_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(vlan_range_info);

  res = soc_pb_pp_l2_lif_vlan_compression_get_verify(
          unit,
          local_port_ndx,
          vlan_range_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_l2_lif_vlan_compression_get_unsafe(
          unit,
          local_port_ndx,
          vlan_range_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_l2_lif_vlan_compression_get()", local_port_ndx, 0);
}


/*********************************************************************
*     Sets the packets attributes (in-port, VIDs) to consider
 *     when associating an incoming packet to in-AC.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_l2_lif_ac_map_key_set(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_KEY_QUALIFIER                 *qual_key,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_MAP_KEY_TYPE                  key_mapping
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_L2_LIF_AC_MAP_KEY_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = soc_pb_pp_l2_lif_ac_map_key_set_verify(
          unit,
          qual_key,
          key_mapping
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_l2_lif_ac_map_key_set_unsafe(
          unit,
          qual_key,
          key_mapping
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_l2_lif_ac_map_key_set()", 0, 0);
}

/*********************************************************************
*     Sets the packets attributes (in-port, VIDs) to consider
 *     when associating an incoming packet to in-AC.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_l2_lif_ac_map_key_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_KEY_QUALIFIER                 *qual_key,
    SOC_SAND_OUT SOC_PB_PP_L2_LIF_AC_MAP_KEY_TYPE                  *key_mapping
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_L2_LIF_AC_MAP_KEY_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(key_mapping);

  res = soc_pb_pp_l2_lif_ac_map_key_get_verify(
          unit,
          qual_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_l2_lif_ac_map_key_get_unsafe(
          unit,
          qual_key,
          key_mapping
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_l2_lif_ac_map_key_get()", 0, 0);
}


/*********************************************************************
*     This function is used to define a Maintenance Point (MP)
 *     on an incoming Attachment-Circuit (port x VLAN x VLAN)
 *     and MD-level, and to determine the action to perform. If
 *     the MP is one of the 4K accelerated MEPs, the function
 *     configures the related OAMP databases and associates the
 *     AC and MD-Level with a user-provided handle. This handle
 *     is later used by user to access OAMP database for this
 *     MEP.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_l2_lif_ac_mp_info_set(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PB_PP_LIF_ID                       lif_ndx,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_MP_INFO            *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_L2_LIF_AC_MP_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_pb_pp_l2_lif_ac_mp_info_set_verify(
          unit,
          lif_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_l2_lif_ac_mp_info_set_unsafe(
          unit,
          lif_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_l2_lif_ac_mp_info_set()", lif_ndx, 0);
}

/*********************************************************************
*     This function is used to define a Maintenance Point (MP)
 *     on an incoming Attachment-Circuit (port x VLAN x VLAN)
 *     and MD-level, and to determine the action to perform. If
 *     the MP is one of the 4K accelerated MEPs, the function
 *     configures the related OAMP databases and associates the
 *     AC and MD-Level with a user-provided handle. This handle
 *     is later used by user to access OAMP database for this
 *     MEP.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_l2_lif_ac_mp_info_get(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PB_PP_LIF_ID                       lif_ndx,
    SOC_SAND_OUT SOC_PB_PP_L2_LIF_AC_MP_INFO            *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_L2_LIF_AC_MP_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_pb_pp_l2_lif_ac_mp_info_get_verify(
          unit,
          lif_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_l2_lif_ac_mp_info_get_unsafe(
          unit,
          lif_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_l2_lif_ac_mp_info_get()", lif_ndx, 0);
}

/*********************************************************************
*     Enable an Attachment Circuit, and set the AC attributes.
 *     Bind the ac-key to the LIF, and set the LIF attributes
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_l2_lif_ac_add(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_KEY                           *ac_key,
    SOC_SAND_IN  SOC_PB_PP_LIF_ID                                  lif_index,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_INFO                          *ac_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                          *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_L2_LIF_AC_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(ac_key);
  SOC_SAND_CHECK_NULL_INPUT(ac_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = soc_pb_pp_l2_lif_ac_add_verify(
          unit,
          ac_key,
          lif_index,
          ac_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_l2_lif_ac_add_unsafe(
          unit,
          ac_key,
          lif_index,
          ac_info,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_l2_lif_ac_add()", 0, 0);
}

/*********************************************************************
*     Get an Attachment Circuit, according to AC key.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_l2_lif_ac_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_KEY                           *ac_key,
    SOC_SAND_OUT SOC_PB_PP_LIF_ID                              *lif_index,
    SOC_SAND_OUT SOC_PB_PP_L2_LIF_AC_INFO                          *ac_info,
    SOC_SAND_OUT uint8                                     *found
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_L2_LIF_AC_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(ac_key);
  SOC_SAND_CHECK_NULL_INPUT(lif_index);
  SOC_SAND_CHECK_NULL_INPUT(ac_info);
  SOC_SAND_CHECK_NULL_INPUT(found);

  res = soc_pb_pp_l2_lif_ac_get_verify(
          unit,
          ac_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_l2_lif_ac_get_unsafe(
          unit,
          ac_key,
          lif_index,
          ac_info,
          found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_l2_lif_ac_get()", 0, 0);
}

/*********************************************************************
*     Add Attachment Circuit (AC) group. Enable defining ACs,
 *     according to Quality of Service attributes, on top of
 *     the usual port * VID [*VID] identification
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_l2_lif_ac_with_cos_add(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_KEY                           *ac_key,
    SOC_SAND_IN  SOC_PB_PP_LIF_ID                              base_lif_index,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_GROUP_INFO                    *acs_group_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                          *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_L2_LIF_AC_WITH_COS_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(ac_key);
  SOC_SAND_CHECK_NULL_INPUT(acs_group_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = soc_pb_pp_l2_lif_ac_with_cos_add_verify(
          unit,
          ac_key,
          base_lif_index,
          acs_group_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_l2_lif_ac_with_cos_add_unsafe(
          unit,
          ac_key,
          base_lif_index,
          acs_group_info,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_l2_lif_ac_with_cos_add()", 0, 0);
}

/*********************************************************************
*     Get Attachment Circuit (AC) group attributes
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_l2_lif_ac_with_cos_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_KEY                           *ac_key,
    SOC_SAND_OUT SOC_PB_PP_LIF_ID                              *base_lif_index,
    SOC_SAND_OUT SOC_PB_PP_L2_LIF_AC_GROUP_INFO                    *acs_group_info,
    SOC_SAND_OUT uint8                                     *found
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_L2_LIF_AC_WITH_COS_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(ac_key);
  SOC_SAND_CHECK_NULL_INPUT(base_lif_index);
  SOC_SAND_CHECK_NULL_INPUT(acs_group_info);
  SOC_SAND_CHECK_NULL_INPUT(found);

  res = soc_pb_pp_l2_lif_ac_with_cos_get_verify(
          unit,
          ac_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_l2_lif_ac_with_cos_get_unsafe(
          unit,
          ac_key,
          base_lif_index,
          acs_group_info,
          found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_l2_lif_ac_with_cos_get()", 0, 0);
}

/*********************************************************************
*     Remove an Attachment Circuit
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_l2_lif_ac_remove(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_KEY                           *ac_key,
    SOC_SAND_OUT SOC_PB_PP_LIF_ID                              *lif_index
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_L2_LIF_AC_REMOVE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(ac_key);
  SOC_SAND_CHECK_NULL_INPUT(lif_index);

  res = soc_pb_pp_l2_lif_ac_remove_verify(
          unit,
          ac_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_l2_lif_ac_remove_unsafe(
          unit,
          ac_key,
          lif_index
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_l2_lif_ac_remove()", 0, 0);
}

/*********************************************************************
*     Sets Trap information for Layer 2 control protocol
 *     frames. Packet is an MEF layer 2 control protocol
 *     service frame When DA matches 01-80-c2-00-00-XX where XX
 *     = 8'b00xx_xxxx.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_l2_lif_l2cp_trap_set(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_L2CP_KEY                         *l2cp_key,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_L2CP_HANDLE_TYPE                 handle_type
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_L2_LIF_L2CP_TRAP_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(l2cp_key);

  res = soc_pb_pp_l2_lif_l2cp_trap_set_verify(
          unit,
          l2cp_key,
          handle_type
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_l2_lif_l2cp_trap_set_unsafe(
          unit,
          l2cp_key,
          handle_type
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_l2_lif_l2cp_trap_set()", 0, 0);
}

/*********************************************************************
*     Sets Trap information for Layer 2 control protocol
 *     frames. Packet is an MEF layer 2 control protocol
 *     service frame When DA matches 01-80-c2-00-00-XX where XX
 *     = 8'b00xx_xxxx.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_l2_lif_l2cp_trap_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_L2CP_KEY                         *l2cp_key,
    SOC_SAND_OUT SOC_PB_PP_L2_LIF_L2CP_HANDLE_TYPE                 *handle_type
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_L2_LIF_L2CP_TRAP_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(l2cp_key);
  SOC_SAND_CHECK_NULL_INPUT(handle_type);

  res = soc_pb_pp_l2_lif_l2cp_trap_get_verify(
          unit,
          l2cp_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_l2_lif_l2cp_trap_get_unsafe(
          unit,
          l2cp_key,
          handle_type
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_l2_lif_l2cp_trap_get()", 0, 0);
}

/*********************************************************************
*     Map an I-SID to VSI and vice versa. Set I-SID related
 *     attributes. Relevant when the packet arrives from the
 *     PBP
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_l2_lif_isid_add(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_SYS_VSI_ID                          vsi_ndx,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_ISID_KEY                         *isid_key,
    SOC_SAND_IN  SOC_PB_PP_LIF_ID                              lif_index,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_ISID_INFO                        *isid_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                          *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_L2_LIF_ISID_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(isid_key);
  SOC_SAND_CHECK_NULL_INPUT(isid_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = soc_pb_pp_l2_lif_isid_add_verify(
          unit,
          vsi_ndx,
          isid_key,
          lif_index,
          isid_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_l2_lif_isid_add_unsafe(
          unit,
          vsi_ndx,
          isid_key,
          lif_index,
          isid_info,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_l2_lif_isid_add()", vsi_ndx, 0);
}

/*********************************************************************
*     Map an I-SID to VSI and vice versa. Set I-SID related
 *     attributes. Relevant when the packet arrives from the
 *     PBP
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_l2_lif_isid_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_ISID_KEY                         *isid_key,
    SOC_SAND_OUT SOC_PB_PP_LIF_ID                                  *lif_index,
    SOC_SAND_OUT SOC_PB_PP_SYS_VSI_ID                              *vsi_index,
    SOC_SAND_OUT SOC_PB_PP_L2_LIF_ISID_INFO                        *isid_info,
    SOC_SAND_OUT uint8                                     *found
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_L2_LIF_ISID_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(isid_key);
  SOC_SAND_CHECK_NULL_INPUT(lif_index);
  SOC_SAND_CHECK_NULL_INPUT(vsi_index);
  SOC_SAND_CHECK_NULL_INPUT(isid_info);
  SOC_SAND_CHECK_NULL_INPUT(found);

  res = soc_pb_pp_l2_lif_isid_get_verify(
          unit,
          isid_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_l2_lif_isid_get_unsafe(
          unit,
          isid_key,
          lif_index,
          vsi_index,
          isid_info,
          found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_l2_lif_isid_get()", 0, 0);
}

/*********************************************************************
*     Get the I-SID related to the VSI
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_l2_lif_vsi_to_isid(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_SYS_VSI_ID                          vsi_ndx,
    SOC_SAND_OUT SOC_SAND_PP_ISID 		    				   *isid_id
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_L2_LIF_VSI_TO_ISID);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(isid_id);

  res = soc_pb_pp_l2_lif_vsi_to_isid_verify(
          unit,
          vsi_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_l2_lif_vsi_to_isid_unsafe(
          unit,
		  vsi_ndx,
		  isid_id
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_l2_lif_vsi_to_isid()", 0, 0);
}

/*********************************************************************
*     Unbind an I-SID from VSI and vice versa
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_l2_lif_isid_remove(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_ISID_KEY                         *isid_key,
    SOC_SAND_OUT SOC_PB_PP_LIF_ID                                  *lif_index
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_L2_LIF_ISID_REMOVE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(isid_key);

  res = soc_pb_pp_l2_lif_isid_remove_verify(
          unit,
          isid_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_l2_lif_isid_remove_unsafe(
          unit,
          isid_key,
          lif_index
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_l2_lif_isid_remove()", 0, 0);
}

void
  SOC_PB_PP_L2_LIF_AC_KEY_QUALIFIER_clear(
    SOC_SAND_OUT SOC_PB_PP_L2_LIF_AC_KEY_QUALIFIER *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_L2_LIF_AC_KEY_QUALIFIER_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_L2_LIF_IN_VC_RANGE_clear(
    SOC_SAND_OUT SOC_PB_PP_L2_LIF_IN_VC_RANGE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_L2_LIF_IN_VC_RANGE_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_L2_LIF_DEFAULT_FORWARDING_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_L2_LIF_DEFAULT_FORWARDING_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_L2_LIF_DEFAULT_FORWARDING_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_L2_LIF_PWE_LEARN_RECORD_clear(
    SOC_SAND_OUT SOC_PB_PP_L2_LIF_PWE_LEARN_RECORD *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_L2_LIF_PWE_LEARN_RECORD_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_L2_LIF_PWE_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_L2_LIF_PWE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_L2_LIF_PWE_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_L2_VLAN_RANGE_KEY_clear(
    SOC_SAND_OUT SOC_PB_PP_L2_VLAN_RANGE_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_L2_VLAN_RANGE_KEY_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_L2_PORT_VLAN_RANGE_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_L2_PORT_VLAN_RANGE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_L2_PORT_VLAN_RANGE_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_L2_LIF_AC_KEY_clear(
    SOC_SAND_OUT SOC_PB_PP_L2_LIF_AC_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_L2_LIF_AC_KEY_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_L2_LIF_AC_LEARN_RECORD_clear(
    SOC_SAND_OUT SOC_PB_PP_L2_LIF_AC_LEARN_RECORD *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_L2_LIF_AC_LEARN_RECORD_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_L2_LIF_AC_ING_EDIT_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_L2_LIF_AC_ING_EDIT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_L2_LIF_AC_ING_EDIT_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_L2_LIF_AC_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_L2_LIF_AC_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_L2_LIF_AC_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_L2_LIF_AC_MP_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_L2_LIF_AC_MP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_L2_LIF_AC_MP_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_L2_LIF_L2CP_KEY_clear(
    SOC_SAND_OUT SOC_PB_PP_L2_LIF_L2CP_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_L2_LIF_L2CP_KEY_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_L2_LIF_AC_GROUP_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_L2_LIF_AC_GROUP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_L2_LIF_AC_GROUP_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_L2_LIF_ISID_KEY_clear(
    SOC_SAND_OUT SOC_PB_PP_L2_LIF_ISID_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_L2_LIF_ISID_KEY_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_L2_LIF_ISID_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_L2_LIF_ISID_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_L2_LIF_ISID_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PB_PP_DEBUG_IS_LVL1

const char*
  SOC_PB_PP_L2_LIF_AC_MAP_KEY_TYPE_to_string(
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_MAP_KEY_TYPE enum_val
  )
{
  return SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_to_string(enum_val);
}

const char*
  SOC_PB_PP_L2_LIF_DFLT_FRWRD_SRC_to_string(
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_DFLT_FRWRD_SRC enum_val
  )
{
  return SOC_PPC_L2_LIF_DFLT_FRWRD_SRC_to_string(enum_val);
}

const char*
  SOC_PB_PP_L2_LIF_AC_SERVICE_TYPE_to_string(
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_SERVICE_TYPE enum_val
  )
{
  return SOC_PPC_L2_LIF_AC_SERVICE_TYPE_to_string(enum_val);
}

const char*
  SOC_PB_PP_L2_LIF_PWE_SERVICE_TYPE_to_string(
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_PWE_SERVICE_TYPE enum_val
  )
{
  return SOC_PPC_L2_LIF_PWE_SERVICE_TYPE_to_string(enum_val);
}

const char*
  SOC_PB_PP_L2_LIF_ISID_SERVICE_TYPE_to_string(
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_ISID_SERVICE_TYPE enum_val
  )
{
  return SOC_PPC_L2_LIF_ISID_SERVICE_TYPE_to_string(enum_val);
}

const char*
  SOC_PB_PP_L2_LIF_AC_LEARN_TYPE_to_string(
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_LEARN_TYPE enum_val
  )
{
  return SOC_PPC_L2_LIF_AC_LEARN_TYPE_to_string(enum_val);
}

const char*
  SOC_PB_PP_L2_LIF_L2CP_HANDLE_TYPE_to_string(
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_L2CP_HANDLE_TYPE enum_val
  )
{
  return SOC_PPC_L2_LIF_L2CP_HANDLE_TYPE_to_string(enum_val);
}

void
  SOC_PB_PP_L2_LIF_AC_KEY_QUALIFIER_print(
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_KEY_QUALIFIER *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_L2_LIF_AC_KEY_QUALIFIER_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_L2_LIF_IN_VC_RANGE_print(
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_IN_VC_RANGE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_L2_LIF_IN_VC_RANGE_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_L2_LIF_DEFAULT_FORWARDING_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_DEFAULT_FORWARDING_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_L2_LIF_DEFAULT_FORWARDING_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_L2_LIF_PWE_LEARN_RECORD_print(
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_PWE_LEARN_RECORD *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_L2_LIF_PWE_LEARN_RECORD_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_L2_LIF_PWE_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_PWE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_L2_LIF_PWE_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_L2_VLAN_RANGE_KEY_print(
    SOC_SAND_IN  SOC_PB_PP_L2_VLAN_RANGE_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_L2_VLAN_RANGE_KEY_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_L2_PORT_VLAN_RANGE_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_L2_PORT_VLAN_RANGE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_L2_PORT_VLAN_RANGE_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_L2_LIF_AC_KEY_print(
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_L2_LIF_AC_KEY_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_L2_LIF_AC_LEARN_RECORD_print(
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_LEARN_RECORD *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_L2_LIF_AC_LEARN_RECORD_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_L2_LIF_AC_ING_EDIT_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_ING_EDIT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_L2_LIF_AC_ING_EDIT_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_L2_LIF_AC_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_L2_LIF_AC_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_L2_LIF_AC_MP_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_MP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_L2_LIF_AC_MP_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_L2_LIF_L2CP_KEY_print(
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_L2CP_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_L2_LIF_L2CP_KEY_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_L2_LIF_AC_GROUP_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_GROUP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_L2_LIF_AC_GROUP_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_L2_LIF_ISID_KEY_print(
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_ISID_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_L2_LIF_ISID_KEY_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_L2_LIF_ISID_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_ISID_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_L2_LIF_ISID_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif /* SOC_PB_PP_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

