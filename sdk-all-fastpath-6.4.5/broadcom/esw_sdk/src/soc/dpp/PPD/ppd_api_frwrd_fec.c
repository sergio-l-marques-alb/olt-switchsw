/* $Id: ppd_api_frwrd_fec.c,v 1.16 Broadcom SDK $
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
* FILENAME:       DuneDriver/ppd/src/soc_ppd_api_frwrd_fec.c
*
* MODULE PREFIX:  soc_ppd_frwrd
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
#include <soc/dpp/PPD/ppd_api_frwrd_fec.h>
#ifdef LINK_PB_LIBRARIES
  #include <soc/dpp/Petra/PB_PP/pb_pp_api_frwrd_fec.h>
#endif
#ifdef LINK_T20E_LIBRARIES
  #include <soc/dpp/T20E/t20e_api_frwrd_fec.h>
#endif
#ifdef LINK_ARAD_LIBRARIES
  #include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_frwrd_fec.h>
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
    Ppd_procedure_desc_element_frwrd_fec[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_FEC_GLBL_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_FEC_GLBL_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_FEC_GLBL_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_FEC_GLBL_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_FEC_ENTRY_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_FEC_ENTRY_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_FEC_ECMP_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_FEC_ECMP_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_FEC_ECMP_UPDATE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_FEC_ECMP_UPDATE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_FEC_ENTRY_USE_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_FEC_ENTRY_USE_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_FEC_ENTRY_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_FEC_ENTRY_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_FEC_ECMP_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_FEC_ECMP_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_FEC_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_FEC_REMOVE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_FEC_PROTECTION_OAM_INSTANCE_STATUS_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_FEC_PROTECTION_OAM_INSTANCE_STATUS_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_FEC_PROTECTION_OAM_INSTANCE_STATUS_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_FEC_PROTECTION_OAM_INSTANCE_STATUS_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_FEC_PROTECTION_SYS_PORT_STATUS_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_FEC_PROTECTION_SYS_PORT_STATUS_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_FEC_PROTECTION_SYS_PORT_STATUS_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_FEC_PROTECTION_SYS_PORT_STATUS_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_FEC_GET_BLOCK),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_FEC_GET_BLOCK_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_FEC_ENTRY_ACCESSED_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_FEC_ENTRY_ACCESSED_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_FEC_ENTRY_ACCESSED_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_FEC_ENTRY_ACCESSED_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_FEC_ECMP_HASHING_GLOBAL_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_FEC_ECMP_HASHING_GLOBAL_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_FEC_ECMP_HASHING_GLOBAL_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_FEC_ECMP_HASHING_GLOBAL_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_FEC_ECMP_HASHING_PORT_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_FEC_ECMP_HASHING_PORT_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_FEC_ECMP_HASHING_PORT_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_FEC_ECMP_HASHING_PORT_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_FEC_ECMP_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_FEC_ECMP_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_FEC_GET_PROCS_PTR),
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
*     Setting global information of the FEC table (including
 *     resources to use)
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_fec_glbl_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_FEC_GLBL_INFO                 *glbl_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_FEC_GLBL_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(glbl_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(frwrd_fec_glbl_info_set,(unit, glbl_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_fec_glbl_info_set_print,(unit,glbl_info));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_fec_glbl_info_set()", 0, 0);
}

/*********************************************************************
*     Setting global information of the FEC table (including
 *     resources to use)
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_fec_glbl_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT SOC_PPD_FRWRD_FEC_GLBL_INFO                 *glbl_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_FEC_GLBL_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(glbl_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(frwrd_fec_glbl_info_get,(unit, glbl_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_fec_glbl_info_get_print,(unit));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_fec_glbl_info_get()", 0, 0);
}

/*********************************************************************
*     Add FEC entry. May include protection of type Facility
 *     or Path.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_fec_entry_add(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_FEC_ID                              fec_ndx,
    SOC_SAND_IN  SOC_PPD_FRWRD_FEC_PROTECT_TYPE              protect_type,
    SOC_SAND_IN  SOC_PPD_FRWRD_FEC_ENTRY_INFO                *working_fec,
    SOC_SAND_IN  SOC_PPD_FRWRD_FEC_ENTRY_INFO                *protect_fec,
    SOC_SAND_IN  SOC_PPD_FRWRD_FEC_PROTECT_INFO              *protect_info,
    SOC_SAND_OUT uint8                               *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_FEC_ENTRY_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(working_fec);
  SOC_SAND_CHECK_NULL_INPUT(protect_fec);
  SOC_SAND_CHECK_NULL_INPUT(protect_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(frwrd_fec_entry_add,(unit, fec_ndx, protect_type, working_fec, protect_fec, protect_info, success));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_fec_entry_add_print,(unit,fec_ndx,protect_type,working_fec,protect_fec,protect_info));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_fec_entry_add()", fec_ndx, 0);
}

/*********************************************************************
*     Add ECMP to the FEC table.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_fec_ecmp_add(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_FEC_ID                              fec_ndx,
    SOC_SAND_IN  SOC_PPD_FRWRD_FEC_ENTRY_INFO                *fec_array,
    SOC_SAND_IN  uint32                                nof_entries,
    SOC_SAND_OUT uint8                               *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_FEC_ECMP_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(fec_array);
  SOC_SAND_CHECK_NULL_INPUT(success);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(frwrd_fec_ecmp_add,(unit, fec_ndx, fec_array, nof_entries, success));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_fec_ecmp_add_print,(unit,fec_ndx,fec_array,nof_entries));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_fec_ecmp_add()", fec_ndx, 0);
}

/*********************************************************************
*     Update the content of block of entries from the ECMP.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_fec_ecmp_update(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_FEC_ID                              fec_ndx,
    SOC_SAND_IN  SOC_PPD_FRWRD_FEC_ENTRY_INFO                *fec_array,
    SOC_SAND_IN  SOC_SAND_U32_RANGE                          *fec_range
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_FEC_ECMP_UPDATE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(fec_array);
  SOC_SAND_CHECK_NULL_INPUT(fec_range);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(frwrd_fec_ecmp_update,(unit, fec_ndx, fec_array, fec_range));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_fec_ecmp_update_print,(unit,fec_ndx,fec_array,fec_range));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_fec_ecmp_update()", fec_ndx, 0);
}

/*********************************************************************
*     Get the usage/allocation of the FEC entry pointed by
 *     fec_ndx (ECMP/FEC/protected FEC/none).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_fec_ecmp_info_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN SOC_PPD_FEC_ID ecmp_ndx,
    SOC_SAND_IN SOC_PPD_FRWRD_FEC_ECMP_INFO *ecmp_info)
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_FEC_ECMP_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(ecmp_info);

  ARAD_PP_DEVICE_CALL(frwrd_fec_ecmp_info_set,(unit, ecmp_ndx, ecmp_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_fec_ecmp_info_set()", ecmp_ndx, 0);
}


/*********************************************************************
*     Get the usage/allocation of the FEC entry pointed by
 *     fec_ndx (ECMP/FEC/protected FEC/none).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_fec_ecmp_info_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN SOC_PPD_FEC_ID ecmp_ndx,
    SOC_SAND_OUT SOC_PPD_FRWRD_FEC_ECMP_INFO *ecmp_info)
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_FEC_ECMP_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(ecmp_info);

  ARAD_PP_DEVICE_CALL(frwrd_fec_ecmp_info_get,(unit, ecmp_ndx, ecmp_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_fec_ecmp_info_get()", ecmp_ndx, 0);
}

/*********************************************************************
*     Get the usage/allocation of the FEC entry pointed by
 *     fec_ndx (ECMP/FEC/protected FEC/none).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_fec_entry_use_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_FEC_ID                              fec_ndx,
    SOC_SAND_OUT SOC_PPD_FRWRD_FEC_ENTRY_USE_INFO            *fec_entry_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_FEC_ENTRY_USE_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(fec_entry_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(frwrd_fec_entry_use_info_get,(unit, fec_ndx, fec_entry_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_fec_entry_use_info_get_print,(unit,fec_ndx));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_fec_entry_use_info_get()", fec_ndx, 0);
}

/*********************************************************************
*     Get FEC entry from the FEC table. May include
 *     protection.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_fec_entry_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_FEC_ID                              fec_ndx,
    SOC_SAND_OUT SOC_PPD_FRWRD_FEC_PROTECT_TYPE              *protect_type,
    SOC_SAND_OUT SOC_PPD_FRWRD_FEC_ENTRY_INFO                *working_fec,
    SOC_SAND_OUT SOC_PPD_FRWRD_FEC_ENTRY_INFO                *protect_fec,
    SOC_SAND_OUT SOC_PPD_FRWRD_FEC_PROTECT_INFO              *protect_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_FEC_ENTRY_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(protect_type);
  SOC_SAND_CHECK_NULL_INPUT(working_fec);
  SOC_SAND_CHECK_NULL_INPUT(protect_fec);
  SOC_SAND_CHECK_NULL_INPUT(protect_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(frwrd_fec_entry_get,(unit, fec_ndx, protect_type, working_fec, protect_fec, protect_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_fec_entry_get_print,(unit,fec_ndx));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_fec_entry_get()", fec_ndx, 0);
}

/*********************************************************************
*     Update content of range of the ECMP.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_fec_ecmp_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_FEC_ID                              fec_ndx,
    SOC_SAND_IN  SOC_SAND_U32_RANGE                          *fec_range,
    SOC_SAND_OUT SOC_PPD_FRWRD_FEC_ENTRY_INFO                *fec_array,
    SOC_SAND_OUT uint32                                *nof_entries
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_FEC_ECMP_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(fec_range);
  SOC_SAND_CHECK_NULL_INPUT(fec_array);
  SOC_SAND_CHECK_NULL_INPUT(nof_entries);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(frwrd_fec_ecmp_get,(unit, fec_ndx, fec_range, fec_array, nof_entries));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_fec_ecmp_get_print,(unit,fec_ndx,fec_range));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_fec_ecmp_get()", fec_ndx, 0);
}

/*********************************************************************
*     Remove FEC entry/entries associated with fec_ndx.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_fec_remove(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_FEC_ID                              fec_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_FEC_REMOVE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_TMP_DEVICE_CALL(frwrd_fec_remove,(unit, fec_ndx));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_fec_remove_print,(unit,fec_ndx));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_fec_remove()", fec_ndx, 0);
}

/*********************************************************************
*     Set the status of the OAM instance. For all PATH
 *     protected FECs that point to this instance, the working
 *     FEC will be used if up is TRUE, and the protect FEC will
 *     be used otherwise.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_fec_protection_oam_instance_status_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                oam_instance_ndx,
    SOC_SAND_IN  uint8                               up
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_FEC_PROTECTION_OAM_INSTANCE_STATUS_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_TMP_DEVICE_CALL(frwrd_fec_protection_oam_instance_status_set,(unit, oam_instance_ndx, up));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_fec_protection_oam_instance_status_set_print,(unit,oam_instance_ndx,up));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_fec_protection_oam_instance_status_set()", oam_instance_ndx, 0);
}

/*********************************************************************
*     Set the status of the OAM instance. For all PATH
 *     protected FECs that point to this instance, the working
 *     FEC will be used if up is TRUE, and the protect FEC will
 *     be used otherwise.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_fec_protection_oam_instance_status_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                oam_instance_ndx,
    SOC_SAND_OUT uint8                               *up
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_FEC_PROTECTION_OAM_INSTANCE_STATUS_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(up);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(frwrd_fec_protection_oam_instance_status_get,(unit, oam_instance_ndx, up));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_fec_protection_oam_instance_status_get_print,(unit,oam_instance_ndx));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_fec_protection_oam_instance_status_get()", oam_instance_ndx, 0);
}

/*********************************************************************
*     Set the status of the System Port (LAG or Physical
 *     port).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_fec_protection_sys_port_status_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_SAND_PP_SYS_PORT_ID                     *sys_port_ndx,
    SOC_SAND_IN  uint8                               up
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_FEC_PROTECTION_SYS_PORT_STATUS_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(sys_port_ndx);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(frwrd_fec_protection_sys_port_status_set,(unit, sys_port_ndx, up));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_fec_protection_sys_port_status_set_print,(unit,sys_port_ndx,up));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_fec_protection_sys_port_status_set()", 0, 0);
}

/*********************************************************************
*     Set the status of the System Port (LAG or Physical
 *     port).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_fec_protection_sys_port_status_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_SAND_PP_SYS_PORT_ID                     *sys_port_ndx,
    SOC_SAND_OUT uint8                               *up
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_FEC_PROTECTION_SYS_PORT_STATUS_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(sys_port_ndx);
  SOC_SAND_CHECK_NULL_INPUT(up);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(frwrd_fec_protection_sys_port_status_get,(unit, sys_port_ndx, up));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_fec_protection_sys_port_status_get_print,(unit,sys_port_ndx));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_fec_protection_sys_port_status_get()", 0, 0);
}

/*********************************************************************
*     Traverse the FEC table (in the specified range) and get
 *     all the FEC entries that match the given rule.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_fec_get_block(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_FEC_MATCH_RULE                *rule,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                  *block_range,
    SOC_SAND_OUT uint32                                *fec_array,
    SOC_SAND_OUT uint32                                *nof_entries
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_FEC_GET_BLOCK);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(rule);
  SOC_SAND_CHECK_NULL_INPUT(block_range);
  SOC_SAND_CHECK_NULL_INPUT(fec_array);
  SOC_SAND_CHECK_NULL_INPUT(nof_entries);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(frwrd_fec_get_block,(unit, rule, block_range, fec_array, nof_entries));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_fec_get_block_print,(unit,rule,block_range));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_fec_get_block()", 0, 0);
}

#if SOC_PPD_DEBUG_IS_LVL1
/*********************************************************************
*     Traverse the FEC table (in the specified range) and get
 *     all the FEC entries that match the given rule.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_fec_print_block(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_FEC_MATCH_RULE                *rule,
    SOC_SAND_IN SOC_SAND_TABLE_BLOCK_RANGE                  *block_range,
    SOC_SAND_IN uint32                                *fec_array,
    SOC_SAND_IN uint32                                nof_entries
  )
{
  uint32
    indx;
  SOC_PPD_FRWRD_FEC_ENTRY_USE_INFO
    fec_entry_use_info;
  SOC_PPD_FRWRD_FEC_ENTRY_INFO
    working_fec,
    protect_fec;
  SOC_PPD_FRWRD_FEC_PROTECT_TYPE
    protect_type;
  SOC_PPD_FRWRD_FEC_PROTECT_INFO
    protect_info;
  SOC_PPD_FRWRD_FEC_ENTRY_ACCESSED_INFO
    accessed_info;
  SOC_SAND_PP_SYS_PORT_ID
    sys_port;
  uint8
    stts;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_FEC_GET_BLOCK);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(rule);
  SOC_SAND_CHECK_NULL_INPUT(block_range);
  SOC_SAND_CHECK_NULL_INPUT(fec_array);

  LOG_CLI((BSL_META_U(unit,
                      " -----------------------------------------------------------------------------\n\r"
                      "|                                   FEC Table                                 |\n\r"
                      " -----------------------------------------------------------------------------\n\r"
                      "|      |                              |   |   Destination   |                 |\n\r"
                      "|offset|   type   |size|protect| inst |stt| Type     | Val  |EEP/AC | RIF |acc|\n\r"
                      " -----------------------------------------------------------------------------\n\r"
               )));



  for (indx = 0; indx < nof_entries; ++indx)
  {
    LOG_CLI((BSL_META_U(unit,
                        "| %-5u"), fec_array[indx]));
    res = soc_ppd_frwrd_fec_entry_use_info_get(
            unit,
            fec_array[indx],
            &fec_entry_use_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    res = soc_ppd_frwrd_fec_entry_get(
            unit,
            fec_array[indx],
            &protect_type,
            &working_fec,
            &protect_fec,
            &protect_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    
    LOG_CLI((BSL_META_U(unit,
                        "|%-10s"), SOC_PPD_FEC_TYPE_to_string_short(working_fec.type)));
    
    LOG_CLI((BSL_META_U(unit,
                        "|%-4u"), fec_entry_use_info.nof_entries));
    if (fec_entry_use_info.type == SOC_PPD_FRWRD_FEC_ENTRY_USE_TYPE_FACILITY_PROTECT)
    {
      if(
          (working_fec.dest.dest_type == SOC_SAND_PP_DEST_LAG) ||
          (working_fec.dest.dest_type == SOC_SAND_PP_DEST_SINGLE_PORT)
        )
      {
        sys_port.sys_port_type = (working_fec.dest.dest_type == SOC_SAND_PP_DEST_LAG)?
        SOC_SAND_PP_SYS_PORT_TYPE_LAG: SOC_SAND_PP_SYS_PORT_TYPE_SINGLE_PORT;

        sys_port.sys_id = working_fec.dest.dest_val;

        res = soc_ppd_frwrd_fec_protection_sys_port_status_get(
                unit,
                &(sys_port),
                &stts
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
      }
      else
      {
        stts = TRUE;
      }
      LOG_CLI((BSL_META_U(unit,
                          "|%-7s"), " fac"));
      LOG_CLI((BSL_META_U(unit,
                          "|%-4s"), " Dest"));
      LOG_CLI((BSL_META_U(unit,
                          "| %s "), (stts)?"V":"X"));
    }
    else if (fec_entry_use_info.type == SOC_PPD_FRWRD_FEC_ENTRY_USE_TYPE_PATH_PROTECT)
    {
      LOG_CLI((BSL_META_U(unit,
                          "|%-7s"), " path"));
      LOG_CLI((BSL_META_U(unit,
                          "| %-5u"), protect_info.oam_instance_id));
      res = soc_ppd_frwrd_fec_protection_oam_instance_status_get(
              unit,
              protect_info.oam_instance_id,
              &stts
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

      LOG_CLI((BSL_META_U(unit,
                          "| %s "), (stts)?"V":"X"));
    }
    else
    {
      LOG_CLI((BSL_META_U(unit,
                          "|%-7s"), ""));
      LOG_CLI((BSL_META_U(unit,
                          "|%-6s"), ""));
      LOG_CLI((BSL_META_U(unit,
                          "| %s "), " "));
    }

    LOG_CLI((BSL_META_U(unit,
                        "|")));
    soc_sand_SAND_PP_DESTINATION_ID_table_format_print(&(working_fec.dest));

    res = soc_ppd_frwrd_fec_entry_accessed_info_get(
            unit,
            fec_array[indx],
            TRUE,
            &accessed_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

#ifdef LINK_PB_LIBRARIES
    if (working_fec.type == SOC_PPD_FEC_TYPE_BRIDGING_WITH_AC)
    {
      LOG_CLI((BSL_META_U(unit,
                          "| %-6u"), working_fec.app_info.out_ac_id));
      LOG_CLI((BSL_META_U(unit,
                          "| %-4s"), ""));
    }
    else if (working_fec.type == SOC_PPD_FEC_TYPE_IP_UC || working_fec.type == SOC_PPD_FEC_TYPE_MPLS_LSR || working_fec.type ==  SOC_PPD_FEC_TYPE_ROUTING)
    {
      LOG_CLI((BSL_META_U(unit,
                          " %-6u"), working_fec.eep));
      LOG_CLI((BSL_META_U(unit,
                          "| %-4u"), working_fec.app_info.out_rif));
    }
    else if (working_fec.type == SOC_PPD_FEC_TYPE_TRILL_MC)
    {
      LOG_CLI((BSL_META_U(unit,
                          " %-6u"), working_fec.eep));
      LOG_CLI((BSL_META_U(unit,
                          "| %-4u"), working_fec.app_info.dist_tree_nick));
    }
    else
#endif
    {
      LOG_CLI((BSL_META_U(unit,
                          " %-6u"), working_fec.eep));
      LOG_CLI((BSL_META_U(unit,
                          "| %-4s"), ""));
    }
    LOG_CLI((BSL_META_U(unit,
                        "| %s "), (accessed_info.accessed)?"V":"X"));

    LOG_CLI((BSL_META_U(unit,
                        "|\n\r")));
  }

  LOG_CLI((BSL_META_U(unit,
                      " -----------------------------------------------------------------------------\n\r"
               )));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_fec_print_block()", 0, 0);
}
#endif
/*********************************************************************
*     Set action to do by the device when a packet accesses
 *     the FEC entry.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_fec_entry_accessed_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_FEC_ID                              fec_id_ndx,
    SOC_SAND_IN  SOC_PPD_FRWRD_FEC_ENTRY_ACCESSED_INFO       *accessed_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_FEC_ENTRY_ACCESSED_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(accessed_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(frwrd_fec_entry_accessed_info_set,(unit, fec_id_ndx, accessed_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_fec_entry_accessed_info_set_print,(unit,fec_id_ndx,accessed_info));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_fec_entry_accessed_info_set()", fec_id_ndx, 0);
}

/*********************************************************************
*     Set action to do by the device when a packet accesses
 *     the FEC entry.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_fec_entry_accessed_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_FEC_ID                              fec_id_ndx,
    SOC_SAND_IN  uint8                               clear_access_stat,
    SOC_SAND_OUT SOC_PPD_FRWRD_FEC_ENTRY_ACCESSED_INFO       *accessed_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_FEC_ENTRY_ACCESSED_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(accessed_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(frwrd_fec_entry_accessed_info_get,(unit, fec_id_ndx, clear_access_stat, accessed_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_fec_entry_accessed_info_get_print,(unit,fec_id_ndx,clear_access_stat));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_fec_entry_accessed_info_get()", fec_id_ndx, 0);
}

/*********************************************************************
*     Set the ECMP hashing global attributes
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_fec_ecmp_hashing_global_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_FEC_ECMP_HASH_GLOBAL_INFO     *glbl_hash_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_FEC_ECMP_HASHING_GLOBAL_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(glbl_hash_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(frwrd_fec_ecmp_hashing_global_info_set,(unit, glbl_hash_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_fec_ecmp_hashing_global_info_set_print,(unit,glbl_hash_info));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_fec_ecmp_hashing_global_info_set()", 0, 0);
}

/*********************************************************************
*     Set the ECMP hashing global attributes
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_fec_ecmp_hashing_global_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT SOC_PPD_FRWRD_FEC_ECMP_HASH_GLOBAL_INFO     *glbl_hash_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_FEC_ECMP_HASHING_GLOBAL_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(glbl_hash_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(frwrd_fec_ecmp_hashing_global_info_get,(unit, glbl_hash_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_fec_ecmp_hashing_global_info_get_print,(unit));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_fec_ecmp_hashing_global_info_get()", 0, 0);
}

/*********************************************************************
*     Set the ECMP hashing per-port attributes
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_fec_ecmp_hashing_port_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  int                                core_id,
    SOC_SAND_IN  SOC_PPD_PORT                                port_ndx,
    SOC_SAND_IN  SOC_PPD_FRWRD_FEC_ECMP_HASH_PORT_INFO       *port_hash_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_FEC_ECMP_HASHING_PORT_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(port_hash_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(frwrd_fec_ecmp_hashing_port_info_set,(unit, core_id, port_ndx, port_hash_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_fec_ecmp_hashing_port_info_set_print,(unit,port_ndx,port_hash_info));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_fec_ecmp_hashing_port_info_set()", port_ndx, 0);
}

/*********************************************************************
*     Set the ECMP hashing per-port attributes
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_fec_ecmp_hashing_port_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  int                               core_id,
    SOC_SAND_IN  SOC_PPD_PORT                                port_ndx,
    SOC_SAND_OUT SOC_PPD_FRWRD_FEC_ECMP_HASH_PORT_INFO       *port_hash_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_FEC_ECMP_HASHING_PORT_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(port_hash_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(frwrd_fec_ecmp_hashing_port_info_get,(unit, core_id, port_ndx, port_hash_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_fec_ecmp_hashing_port_info_get_print,(unit,port_ndx));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_fec_ecmp_hashing_port_info_get()", port_ndx, 0);
}

/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     soc_ppd_api_frwrd_fec module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_PROCEDURE_DESC_ELEMENT*
  soc_ppd_frwrd_fec_get_procs_ptr(
  )
{
  return Ppd_procedure_desc_element_frwrd_fec;
}
void
  SOC_PPD_FRWRD_FEC_GLBL_INFO_clear(
  SOC_SAND_IN  int                         unit,
    SOC_SAND_OUT SOC_PPD_FRWRD_FEC_GLBL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_FEC_GLBL_INFO_clear(unit, info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_FEC_ENTRY_RPF_INFO_clear(
    SOC_SAND_OUT SOC_PPD_FRWRD_FEC_ENTRY_RPF_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_FEC_ENTRY_RPF_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_FEC_ENTRY_APP_INFO_clear(
    SOC_SAND_OUT SOC_PPD_FRWRD_FEC_ENTRY_APP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_FEC_ENTRY_APP_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_FEC_ENTRY_INFO_clear(
    SOC_SAND_OUT SOC_PPD_FRWRD_FEC_ENTRY_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_FEC_ENTRY_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_FEC_ENTRY_ACCESSED_INFO_clear(
    SOC_SAND_OUT SOC_PPD_FRWRD_FEC_ENTRY_ACCESSED_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_FEC_ENTRY_ACCESSED_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_FEC_PROTECT_INFO_clear(
    SOC_SAND_OUT SOC_PPD_FRWRD_FEC_PROTECT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_FEC_PROTECT_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_FEC_ECMP_INFO_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_FEC_ECMP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_FEC_ECMP_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_FEC_ENTRY_USE_INFO_clear(
    SOC_SAND_OUT SOC_PPD_FRWRD_FEC_ENTRY_USE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_FEC_ENTRY_USE_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_FEC_MATCH_RULE_clear(
    SOC_SAND_OUT SOC_PPD_FRWRD_FEC_MATCH_RULE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_FEC_MATCH_RULE_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_FEC_ECMP_HASH_GLOBAL_INFO_clear(
    SOC_SAND_OUT SOC_PPD_FRWRD_FEC_ECMP_HASH_GLOBAL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_FEC_ECMP_HASH_GLOBAL_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_FEC_ECMP_HASH_PORT_INFO_clear(
    SOC_SAND_OUT SOC_PPD_FRWRD_FEC_ECMP_HASH_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_FEC_ECMP_HASH_PORT_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PPD_DEBUG_IS_LVL1

const char*
  SOC_PPD_FEC_TYPE_to_string(
    SOC_SAND_IN  SOC_PPD_FEC_TYPE enum_val
  )
{
  return SOC_PPC_FEC_TYPE_to_string(enum_val);
}

const char*
  SOC_PPD_FEC_TYPE_to_string_short(
    SOC_SAND_IN  SOC_PPD_FEC_TYPE enum_val
  )
{
  return SOC_PPC_FEC_TYPE_to_string_short(enum_val);
}

const char*
  SOC_PPD_FRWRD_FEC_RPF_MODE_to_string(
    SOC_SAND_IN  SOC_PPD_FRWRD_FEC_RPF_MODE enum_val
  )
{
  return SOC_PPC_FRWRD_FEC_RPF_MODE_to_string(enum_val);
}

const char*
  SOC_PPD_FRWRD_FEC_PROTECT_TYPE_to_string(
    SOC_SAND_IN  SOC_PPD_FRWRD_FEC_PROTECT_TYPE enum_val
  )
{
  return SOC_PPC_FRWRD_FEC_PROTECT_TYPE_to_string(enum_val);
}

const char*
  SOC_PPD_FRWRD_FEC_ENTRY_USE_TYPE_to_string(
    SOC_SAND_IN  SOC_PPD_FRWRD_FEC_ENTRY_USE_TYPE enum_val
  )
{
  return SOC_PPC_FRWRD_FEC_ENTRY_USE_TYPE_to_string(enum_val);
}

const char*
  SOC_PPD_FRWRD_FEC_MATCH_RULE_TYPE_to_string(
    SOC_SAND_IN  SOC_PPD_FRWRD_FEC_MATCH_RULE_TYPE enum_val
  )
{
  return SOC_PPC_FRWRD_FEC_MATCH_RULE_TYPE_to_string(enum_val);
}

void
  SOC_PPD_FRWRD_FEC_GLBL_INFO_print(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_FEC_GLBL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_FEC_GLBL_INFO_print(unit, info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_FEC_ENTRY_RPF_INFO_print(
    SOC_SAND_IN  SOC_PPD_FRWRD_FEC_ENTRY_RPF_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_FEC_ENTRY_RPF_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_FEC_ENTRY_APP_INFO_print(
    SOC_SAND_IN  SOC_PPD_FRWRD_FEC_ENTRY_APP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_FEC_ENTRY_APP_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_FEC_ENTRY_INFO_print(
    SOC_SAND_IN  SOC_PPD_FRWRD_FEC_ENTRY_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_FEC_ENTRY_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_FEC_ENTRY_ACCESSED_INFO_print(
    SOC_SAND_IN  SOC_PPD_FRWRD_FEC_ENTRY_ACCESSED_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_FEC_ENTRY_ACCESSED_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_FEC_PROTECT_INFO_print(
    SOC_SAND_IN  SOC_PPD_FRWRD_FEC_PROTECT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_FEC_PROTECT_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_FEC_ECMP_INFO_print(
    SOC_SAND_IN  SOC_PPD_FRWRD_FEC_ECMP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_FEC_ECMP_INFO_print(info);

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  SOC_PPD_FRWRD_FEC_ENTRY_USE_INFO_print(
    SOC_SAND_IN  SOC_PPD_FRWRD_FEC_ENTRY_USE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_FEC_ENTRY_USE_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_FEC_MATCH_RULE_print(
    SOC_SAND_IN  SOC_PPD_FRWRD_FEC_MATCH_RULE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_FEC_MATCH_RULE_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_FEC_ECMP_HASH_GLOBAL_INFO_print(
    SOC_SAND_IN  SOC_PPD_FRWRD_FEC_ECMP_HASH_GLOBAL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_FEC_ECMP_HASH_GLOBAL_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_FEC_ECMP_HASH_PORT_INFO_print(
    SOC_SAND_IN  SOC_PPD_FRWRD_FEC_ECMP_HASH_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_FEC_ECMP_HASH_PORT_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif /* SOC_PPD_DEBUG_IS_LVL1 */

#if SOC_PPD_DEBUG_IS_LVL3

void
  soc_ppd_frwrd_fec_glbl_info_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_FEC_GLBL_INFO                 *glbl_info
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "glbl_info:")));
  SOC_PPD_FRWRD_FEC_GLBL_INFO_print((glbl_info));

  return;
}
void
  soc_ppd_frwrd_fec_glbl_info_get_print(
    SOC_SAND_IN  int                               unit
  )
{

  return;
}
void
  soc_ppd_frwrd_fec_entry_add_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_FEC_ID                              fec_ndx,
    SOC_SAND_IN  SOC_PPD_FRWRD_FEC_PROTECT_TYPE              protect_type,
    SOC_SAND_IN  SOC_PPD_FRWRD_FEC_ENTRY_INFO                *working_fec,
    SOC_SAND_IN  SOC_PPD_FRWRD_FEC_ENTRY_INFO                *protect_fec,
    SOC_SAND_IN  SOC_PPD_FRWRD_FEC_PROTECT_INFO              *protect_info
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "fec_ndx: %lu\n\r"),fec_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "protect_type %s "), SOC_PPD_FRWRD_FEC_PROTECT_TYPE_to_string(protect_type)));

  LOG_CLI((BSL_META_U(unit,
                      "working_fec:")));
  SOC_PPD_FRWRD_FEC_ENTRY_INFO_print((working_fec));

  LOG_CLI((BSL_META_U(unit,
                      "protect_fec:")));
  SOC_PPD_FRWRD_FEC_ENTRY_INFO_print((protect_fec));

  LOG_CLI((BSL_META_U(unit,
                      "protect_info:")));
  SOC_PPD_FRWRD_FEC_PROTECT_INFO_print((protect_info));

  return;
}
void
  soc_ppd_frwrd_fec_ecmp_add_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_FEC_ID                              fec_ndx,
    SOC_SAND_IN  SOC_PPD_FRWRD_FEC_ENTRY_INFO                *fec_array,
    SOC_SAND_IN  uint32                                nof_entries
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "fec_ndx: %lu\n\r"),fec_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "fec_array:")));
  SOC_PPD_FRWRD_FEC_ENTRY_INFO_print((fec_array));

  LOG_CLI((BSL_META_U(unit,
                      "nof_entries: %lu\n\r"),nof_entries));

  return;
}
void
  soc_ppd_frwrd_fec_ecmp_update_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_FEC_ID                              fec_ndx,
    SOC_SAND_IN  SOC_PPD_FRWRD_FEC_ENTRY_INFO                *fec_array,
    SOC_SAND_IN  SOC_SAND_U32_RANGE                          *fec_range
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "fec_ndx: %lu\n\r"),fec_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "fec_array:")));
  SOC_PPD_FRWRD_FEC_ENTRY_INFO_print((fec_array));

  LOG_CLI((BSL_META_U(unit,
                      "fec_range: %lu\n\r"),*fec_range));

  return;
}
void
  soc_ppd_frwrd_fec_entry_use_info_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_FEC_ID                              fec_ndx
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "fec_ndx: %lu\n\r"),fec_ndx));

  return;
}
void
  soc_ppd_frwrd_fec_entry_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_FEC_ID                              fec_ndx
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "fec_ndx: %lu\n\r"),fec_ndx));

  return;
}
void
  soc_ppd_frwrd_fec_ecmp_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_FEC_ID                              fec_ndx,
    SOC_SAND_IN  SOC_SAND_U32_RANGE                          *fec_range
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "fec_ndx: %lu\n\r"),fec_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "fec_range: %lu\n\r"),*fec_range));

  return;
}
void
  soc_ppd_frwrd_fec_remove_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_FEC_ID                              fec_ndx
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "fec_ndx: %lu\n\r"),fec_ndx));

  return;
}
void
  soc_ppd_frwrd_fec_protection_oam_instance_status_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                oam_instance_ndx,
    SOC_SAND_IN  uint8                               up
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "oam_instance_ndx: %lu\n\r"),oam_instance_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "up: %u\n\r"),up));

  return;
}
void
  soc_ppd_frwrd_fec_protection_oam_instance_status_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                oam_instance_ndx
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "oam_instance_ndx: %lu\n\r"),oam_instance_ndx));

  return;
}
void
  soc_ppd_frwrd_fec_protection_sys_port_status_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_SAND_PP_SYS_PORT_ID                     *sys_port_ndx,
    SOC_SAND_IN  uint8                               up
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "sys_port_ndx:")));
  soc_sand_SAND_PP_SYS_PORT_ID_print((sys_port_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "up: %u\n\r"),up));

  return;
}
void
  soc_ppd_frwrd_fec_protection_sys_port_status_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_SAND_PP_SYS_PORT_ID                     *sys_port_ndx
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "sys_port_ndx:")));
  soc_sand_SAND_PP_SYS_PORT_ID_print((sys_port_ndx));

  return;
}
void
  soc_ppd_frwrd_fec_get_block_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_FEC_MATCH_RULE                *rule,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                  *block_range
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "rule:")));
  SOC_PPD_FRWRD_FEC_MATCH_RULE_print((rule));

  LOG_CLI((BSL_META_U(unit,
                      "block_range:")));
  soc_sand_SAND_TABLE_BLOCK_RANGE_print((block_range));

  return;
}
void
  soc_ppd_frwrd_fec_entry_accessed_info_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_FEC_ID                              fec_id_ndx,
    SOC_SAND_IN  SOC_PPD_FRWRD_FEC_ENTRY_ACCESSED_INFO       *accessed_info
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "fec_id_ndx: %lu\n\r"),fec_id_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "accessed_info:")));
  SOC_PPD_FRWRD_FEC_ENTRY_ACCESSED_INFO_print((accessed_info));

  return;
}
void
  soc_ppd_frwrd_fec_entry_accessed_info_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_FEC_ID                              fec_id_ndx,
    SOC_SAND_IN  uint8                               clear_access_stat
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "fec_id_ndx: %lu\n\r"),fec_id_ndx));
  LOG_CLI((BSL_META_U(unit,
                      "clear_access_stat: %lu\n\r"),clear_access_stat));

  return;
}
void
  soc_ppd_frwrd_fec_ecmp_hashing_global_info_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_FEC_ECMP_HASH_GLOBAL_INFO     *glbl_hash_info
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "glbl_hash_info:")));
  SOC_PPD_FRWRD_FEC_ECMP_HASH_GLOBAL_INFO_print((glbl_hash_info));

  return;
}
void
  soc_ppd_frwrd_fec_ecmp_hashing_global_info_get_print(
    SOC_SAND_IN  int                               unit
  )
{

  return;
}
void
  soc_ppd_frwrd_fec_ecmp_hashing_port_info_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_PORT                                port_ndx,
    SOC_SAND_IN  SOC_PPD_FRWRD_FEC_ECMP_HASH_PORT_INFO       *port_hash_info
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "port_ndx: %lu\n\r"),port_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "port_hash_info:")));
  SOC_PPD_FRWRD_FEC_ECMP_HASH_PORT_INFO_print((port_hash_info));

  return;
}
void
  soc_ppd_frwrd_fec_ecmp_hashing_port_info_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_PORT                                port_ndx
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "port_ndx: %lu\n\r"),port_ndx));

  return;
}
#endif /* SOC_PPD_DEBUG_IS_LVL3 */
/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

