/* $Id: pb_pp_api_frwrd_fec.c,v 1.7 Broadcom SDK $
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
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_PP/src/soc_pb_pp_api_frwrd_fec.c
*
* MODULE PREFIX:  soc_pb_pp
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
#include <soc/dpp/Petra/PB_PP/pb_pp_api_frwrd_fec.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_frwrd_fec.h>

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
*     Setting global information of the FEC table (including
 *     resources to use)
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_fec_glbl_info_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_GLBL_INFO                 *glbl_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_FEC_GLBL_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(glbl_info);

  res = soc_pb_pp_frwrd_fec_glbl_info_set_verify(
          unit,
          glbl_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_frwrd_fec_glbl_info_set_unsafe(
          unit,
          glbl_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_fec_glbl_info_set()", 0, 0);
}

/*********************************************************************
*     Setting global information of the FEC table (including
 *     resources to use)
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_fec_glbl_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_FEC_GLBL_INFO                 *glbl_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_FEC_GLBL_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(glbl_info);

  res = soc_pb_pp_frwrd_fec_glbl_info_get_verify(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_frwrd_fec_glbl_info_get_unsafe(
          unit,
          glbl_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_fec_glbl_info_get()", 0, 0);
}

/*********************************************************************
*     Add FEC entry. May include protection of type Facility
 *     or Path.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_fec_entry_add(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FEC_ID                              fec_ndx,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_PROTECT_TYPE              protect_type,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_ENTRY_INFO                *working_fec,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_ENTRY_INFO                *protect_fec,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_PROTECT_INFO              *protect_info,
    SOC_SAND_OUT uint8                                 *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_FEC_ENTRY_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(working_fec);
  SOC_SAND_CHECK_NULL_INPUT(protect_fec);
  SOC_SAND_CHECK_NULL_INPUT(protect_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = soc_pb_pp_frwrd_fec_entry_add_verify(
          unit,
          fec_ndx,
          protect_type,
          working_fec,
          protect_fec,
          protect_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_frwrd_fec_entry_add_unsafe(
          unit,
          fec_ndx,
          protect_type,
          working_fec,
          protect_fec,
          protect_info,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_fec_entry_add()", fec_ndx, 0);
}

/*********************************************************************
*     Add ECMP to the FEC table.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_fec_ecmp_add(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FEC_ID                              fec_ndx,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_ENTRY_INFO                *fec_array,
    SOC_SAND_IN  uint32                                  nof_entries,
    SOC_SAND_OUT uint8                                 *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_FEC_ECMP_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(fec_array);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = soc_pb_pp_frwrd_fec_ecmp_add_verify(
          unit,
          fec_ndx,
          fec_array,
          nof_entries
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_frwrd_fec_ecmp_add_unsafe(
          unit,
          fec_ndx,
          fec_array,
          nof_entries,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_fec_ecmp_add()", fec_ndx, 0);
}

/*********************************************************************
*     Update the content of block of entries from the ECMP.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_fec_ecmp_update(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FEC_ID                              fec_ndx,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_ENTRY_INFO                *fec_array,
    SOC_SAND_IN  SOC_SAND_U32_RANGE                            *fec_range
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_FEC_ECMP_UPDATE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(fec_array);
  SOC_SAND_CHECK_NULL_INPUT(fec_range);

  res = soc_pb_pp_frwrd_fec_ecmp_update_verify(
          unit,
          fec_ndx,
          fec_array,
          fec_range
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_frwrd_fec_ecmp_update_unsafe(
          unit,
          fec_ndx,
          fec_array,
          fec_range
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_fec_ecmp_update()", fec_ndx, 0);
}

/*********************************************************************
*     Get the usage/allocation of the FEC entry pointed by
 *     fec_ndx (ECMP/FEC/protected FEC/none).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_fec_entry_use_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FEC_ID                              fec_ndx,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_FEC_ENTRY_USE_INFO            *fec_entry_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_FEC_ENTRY_USE_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(fec_entry_info);

  res = soc_pb_pp_frwrd_fec_entry_use_info_get_verify(
          unit,
          fec_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_frwrd_fec_entry_use_info_get_unsafe(
          unit,
          fec_ndx,
          fec_entry_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_fec_entry_use_info_get()", fec_ndx, 0);
}

/*********************************************************************
*     Get FEC entry from the FEC table. May include
 *     protection.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_fec_entry_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FEC_ID                              fec_ndx,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_FEC_PROTECT_TYPE              *protect_type,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_FEC_ENTRY_INFO                *working_fec,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_FEC_ENTRY_INFO                *protect_fec,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_FEC_PROTECT_INFO              *protect_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_FEC_ENTRY_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(protect_type);
  SOC_SAND_CHECK_NULL_INPUT(working_fec);
  SOC_SAND_CHECK_NULL_INPUT(protect_fec);
  SOC_SAND_CHECK_NULL_INPUT(protect_info);

  res = soc_pb_pp_frwrd_fec_entry_get_verify(
          unit,
          fec_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_frwrd_fec_entry_get_unsafe(
          unit,
          fec_ndx,
          protect_type,
          working_fec,
          protect_fec,
          protect_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_fec_entry_get()", fec_ndx, 0);
}

/*********************************************************************
*     Update content of range of the ECMP.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_fec_ecmp_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FEC_ID                              fec_ndx,
    SOC_SAND_IN  SOC_SAND_U32_RANGE                            *fec_range,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_FEC_ENTRY_INFO                *fec_array,
    SOC_SAND_OUT uint32                                  *nof_entries
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_FEC_ECMP_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(fec_range);
  SOC_SAND_CHECK_NULL_INPUT(fec_array);
  SOC_SAND_CHECK_NULL_INPUT(nof_entries);

  res = soc_pb_pp_frwrd_fec_ecmp_get_verify(
          unit,
          fec_ndx,
          fec_range
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_frwrd_fec_ecmp_get_unsafe(
          unit,
          fec_ndx,
          fec_range,
          fec_array,
          nof_entries
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_fec_ecmp_get()", fec_ndx, 0);
}

/*********************************************************************
*     Remove FEC entry/entries associated with fec_ndx.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_fec_remove(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FEC_ID                              fec_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_FEC_REMOVE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = soc_pb_pp_frwrd_fec_remove_verify(
          unit,
          fec_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_frwrd_fec_remove_unsafe(
          unit,
          fec_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_fec_remove()", fec_ndx, 0);
}

/*********************************************************************
*     Set the status of the OAM instance. For all PATH
 *     protected FECs that point to this instance, the working
 *     FEC will be used if up is TRUE, and the protect FEC will
 *     be used otherwise.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_fec_protection_oam_instance_status_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  oam_instance_ndx,
    SOC_SAND_IN  uint8                                 up
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_FEC_PROTECTION_OAM_INSTANCE_STATUS_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = soc_pb_pp_frwrd_fec_protection_oam_instance_status_set_verify(
          unit,
          oam_instance_ndx,
          up
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_frwrd_fec_protection_oam_instance_status_set_unsafe(
          unit,
          oam_instance_ndx,
          up
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_fec_protection_oam_instance_status_set()", oam_instance_ndx, 0);
}

/*********************************************************************
*     Set the status of the OAM instance. For all PATH
 *     protected FECs that point to this instance, the working
 *     FEC will be used if up is TRUE, and the protect FEC will
 *     be used otherwise.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_fec_protection_oam_instance_status_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  oam_instance_ndx,
    SOC_SAND_OUT uint8                                 *up
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_FEC_PROTECTION_OAM_INSTANCE_STATUS_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(up);

  res = soc_pb_pp_frwrd_fec_protection_oam_instance_status_get_verify(
          unit,
          oam_instance_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_frwrd_fec_protection_oam_instance_status_get_unsafe(
          unit,
          oam_instance_ndx,
          up
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_fec_protection_oam_instance_status_get()", oam_instance_ndx, 0);
}

/*********************************************************************
*     Set the status of the System Port (LAG or Physical
 *     port).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_fec_protection_sys_port_status_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_SYS_PORT_ID                       *sys_port_ndx,
    SOC_SAND_IN  uint8                                 up
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_FEC_PROTECTION_SYS_PORT_STATUS_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(sys_port_ndx);

  res = soc_pb_pp_frwrd_fec_protection_sys_port_status_set_verify(
          unit,
          sys_port_ndx,
          up
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_frwrd_fec_protection_sys_port_status_set_unsafe(
          unit,
          sys_port_ndx,
          up
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_fec_protection_sys_port_status_set()", 0, 0);
}

/*********************************************************************
*     Set the status of the System Port (LAG or Physical
 *     port).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_fec_protection_sys_port_status_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_SYS_PORT_ID                       *sys_port_ndx,
    SOC_SAND_OUT uint8                                 *up
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_FEC_PROTECTION_SYS_PORT_STATUS_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(sys_port_ndx);
  SOC_SAND_CHECK_NULL_INPUT(up);

  res = soc_pb_pp_frwrd_fec_protection_sys_port_status_get_verify(
          unit,
          sys_port_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_frwrd_fec_protection_sys_port_status_get_unsafe(
          unit,
          sys_port_ndx,
          up
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_fec_protection_sys_port_status_get()", 0, 0);
}

/*********************************************************************
*     Traverse the FEC table (in the specified range) and get
 *     all the FEC entries that match the given rule.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_fec_get_block(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_MATCH_RULE                *rule,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                    *block_range,
    SOC_SAND_OUT uint32                                  *fec_array,
    SOC_SAND_OUT uint32                                  *nof_entries
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_FEC_GET_BLOCK);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(rule);
  SOC_SAND_CHECK_NULL_INPUT(block_range);
  SOC_SAND_CHECK_NULL_INPUT(fec_array);
  SOC_SAND_CHECK_NULL_INPUT(nof_entries);

  res = soc_pb_pp_frwrd_fec_get_block_verify(
          unit,
          rule,
          block_range
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_frwrd_fec_get_block_unsafe(
          unit,
          rule,
          block_range,
          fec_array,
          nof_entries
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_fec_get_block()", 0, 0);
}

/*********************************************************************
*     Set action to do by the device when a packet accesses
 *     the FEC entry.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_fec_entry_accessed_info_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FEC_ID                              fec_id_ndx,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_ENTRY_ACCESSED_INFO       *accessed_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_FEC_ENTRY_ACCESSED_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(accessed_info);
  res = soc_pb_pp_frwrd_fec_entry_accessed_info_set_verify(
          unit,
          fec_id_ndx,
          accessed_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_frwrd_fec_entry_accessed_info_set_unsafe(
          unit,
          fec_id_ndx,
          accessed_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_fec_entry_accessed_info_set()", fec_id_ndx, 0);
}

/*********************************************************************
*     Set action to do by the device when a packet accesses
 *     the FEC entry.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_fec_entry_accessed_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FEC_ID                              fec_id_ndx,
    SOC_SAND_IN  uint8                                 clear_access_stat,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_FEC_ENTRY_ACCESSED_INFO       *accessed_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_FEC_ENTRY_ACCESSED_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;
  SOC_SAND_CHECK_NULL_INPUT(accessed_info);

  res = soc_pb_pp_frwrd_fec_entry_accessed_info_get_verify(
          unit,
          fec_id_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_frwrd_fec_entry_accessed_info_get_unsafe(
          unit,
          fec_id_ndx,
          clear_access_stat,
          accessed_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_fec_entry_accessed_info_get()", fec_id_ndx, 0);
}

/*********************************************************************
*     Set the ECMP hashing global attributes
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_fec_ecmp_hashing_global_info_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_ECMP_HASH_GLOBAL_INFO     *glbl_hash_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_FEC_ECMP_HASHING_GLOBAL_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(glbl_hash_info);

  res = soc_pb_pp_frwrd_fec_ecmp_hashing_global_info_set_verify(
          unit,
          glbl_hash_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_frwrd_fec_ecmp_hashing_global_info_set_unsafe(
          unit,
          glbl_hash_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_fec_ecmp_hashing_global_info_set()", 0, 0);
}

/*********************************************************************
*     Set the ECMP hashing global attributes
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_fec_ecmp_hashing_global_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_FEC_ECMP_HASH_GLOBAL_INFO     *glbl_hash_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_FEC_ECMP_HASHING_GLOBAL_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(glbl_hash_info);

  res = soc_pb_pp_frwrd_fec_ecmp_hashing_global_info_get_verify(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_frwrd_fec_ecmp_hashing_global_info_get_unsafe(
          unit,
          glbl_hash_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_fec_ecmp_hashing_global_info_get()", 0, 0);
}

/*********************************************************************
*     Set the ECMP hashing per-port attributes
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_fec_ecmp_hashing_port_info_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                port_ndx,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_ECMP_HASH_PORT_INFO       *port_hash_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_FEC_ECMP_HASHING_PORT_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(port_hash_info);

  res = soc_pb_pp_frwrd_fec_ecmp_hashing_port_info_set_verify(
          unit,
          port_ndx,
          port_hash_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_frwrd_fec_ecmp_hashing_port_info_set_unsafe(
          unit,
          port_ndx,
          port_hash_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_fec_ecmp_hashing_port_info_set()", port_ndx, 0);
}

/*********************************************************************
*     Set the ECMP hashing per-port attributes
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_fec_ecmp_hashing_port_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                port_ndx,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_FEC_ECMP_HASH_PORT_INFO       *port_hash_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_FEC_ECMP_HASHING_PORT_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(port_hash_info);

  res = soc_pb_pp_frwrd_fec_ecmp_hashing_port_info_get_verify(
          unit,
          port_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_frwrd_fec_ecmp_hashing_port_info_get_unsafe(
          unit,
          port_ndx,
          port_hash_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_fec_ecmp_hashing_port_info_get()", port_ndx, 0);
}

void
  SOC_PB_PP_FRWRD_FEC_GLBL_INFO_clear(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_FEC_GLBL_INFO *info
  )
{
  uint32
    ind;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_FEC_GLBL_INFO_clear(unit, info);
  for (ind = 0; ind < SOC_DPP_ECMP_MAX_SIZE_PETRAB; ++ind)
  {
    info->ecmp_sizes[ind] = 1;
  }
  info->ecmp_sizes_nof_entries = 0;

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_FRWRD_FEC_ENTRY_RPF_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_FRWRD_FEC_ENTRY_RPF_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_FEC_ENTRY_RPF_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_FRWRD_FEC_ENTRY_APP_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_FRWRD_FEC_ENTRY_APP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_FEC_ENTRY_APP_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_FRWRD_FEC_ENTRY_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_FRWRD_FEC_ENTRY_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_FEC_ENTRY_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_FRWRD_FEC_ENTRY_ACCESSED_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_FRWRD_FEC_ENTRY_ACCESSED_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_FEC_ENTRY_ACCESSED_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_FRWRD_FEC_PROTECT_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_FRWRD_FEC_PROTECT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_FEC_PROTECT_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_FRWRD_FEC_ENTRY_USE_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_FRWRD_FEC_ENTRY_USE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_FEC_ENTRY_USE_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_FRWRD_FEC_MATCH_RULE_clear(
    SOC_SAND_OUT SOC_PB_PP_FRWRD_FEC_MATCH_RULE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_FEC_MATCH_RULE_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_FRWRD_FEC_ECMP_HASH_GLOBAL_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_FRWRD_FEC_ECMP_HASH_GLOBAL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_FEC_ECMP_HASH_GLOBAL_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_FRWRD_FEC_ECMP_HASH_PORT_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_FRWRD_FEC_ECMP_HASH_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_FEC_ECMP_HASH_PORT_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PB_PP_DEBUG_IS_LVL1

const char*
  SOC_PB_PP_FEC_TYPE_to_string(
    SOC_SAND_IN  SOC_PB_PP_FEC_TYPE enum_val
  )
{
  return SOC_PPC_FEC_TYPE_to_string(enum_val);
}

const char*
  SOC_PB_PP_FRWRD_FEC_RPF_MODE_to_string(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_RPF_MODE enum_val
  )
{
  return SOC_PPC_FRWRD_FEC_RPF_MODE_to_string(enum_val);
}

const char*
  SOC_PB_PP_FRWRD_FEC_PROTECT_TYPE_to_string(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_PROTECT_TYPE enum_val
  )
{
  return SOC_PPC_FRWRD_FEC_PROTECT_TYPE_to_string(enum_val);
}

const char*
  SOC_PB_PP_FRWRD_FEC_ENTRY_USE_TYPE_to_string(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_ENTRY_USE_TYPE enum_val
  )
{
  return SOC_PPC_FRWRD_FEC_ENTRY_USE_TYPE_to_string(enum_val);
}

const char*
  SOC_PB_PP_FRWRD_FEC_MATCH_RULE_TYPE_to_string(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_MATCH_RULE_TYPE enum_val
  )
{
  return SOC_PPC_FRWRD_FEC_MATCH_RULE_TYPE_to_string(enum_val);
}

void
  SOC_PB_PP_FRWRD_FEC_GLBL_INFO_print(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_GLBL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_FEC_GLBL_INFO_print(unit, info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_FRWRD_FEC_ENTRY_RPF_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_ENTRY_RPF_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_FEC_ENTRY_RPF_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_FRWRD_FEC_ENTRY_APP_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_ENTRY_APP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_FEC_ENTRY_APP_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_FRWRD_FEC_ENTRY_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_ENTRY_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_FEC_ENTRY_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_FRWRD_FEC_ENTRY_ACCESSED_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_ENTRY_ACCESSED_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_FEC_ENTRY_ACCESSED_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_FRWRD_FEC_PROTECT_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_PROTECT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_FEC_PROTECT_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_FRWRD_FEC_ENTRY_USE_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_ENTRY_USE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_FEC_ENTRY_USE_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_FRWRD_FEC_MATCH_RULE_print(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_MATCH_RULE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_FEC_MATCH_RULE_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_FRWRD_FEC_ECMP_HASH_GLOBAL_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_ECMP_HASH_GLOBAL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_FEC_ECMP_HASH_GLOBAL_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_FRWRD_FEC_ECMP_HASH_PORT_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_ECMP_HASH_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_FEC_ECMP_HASH_PORT_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif /* SOC_PB_PP_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

