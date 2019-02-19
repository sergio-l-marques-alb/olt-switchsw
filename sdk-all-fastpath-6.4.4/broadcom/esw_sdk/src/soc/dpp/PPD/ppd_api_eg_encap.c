/* $Id: ppd_api_eg_encap.c,v 1.28 Broadcom SDK $
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
* FILENAME:       DuneDriver/ppd/src/soc_ppd_api_eg_encap.c
*
* MODULE PREFIX:  soc_ppd_eg
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
#include <soc/dpp/PPD/ppd_api_eg_encap.h>
#ifdef LINK_PB_LIBRARIES
  #include <soc/dpp/Petra/PB_PP/pb_pp_api_eg_encap.h>
#endif
#ifdef LINK_ARAD_LIBRARIES
  #include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_eg_encap.h>
#endif
#ifdef LINK_T20E_LIBRARIES
  #include <soc/dpp/T20E/t20e_api_eg_encap.h>
#endif

#include <soc/dpp/JER/JER_PP/jer_pp_eg_encap.h>

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
    Ppd_procedure_desc_element_eg_encap[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_ENCAP_RANGE_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_ENCAP_RANGE_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_ENCAP_RANGE_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_ENCAP_RANGE_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_ENCAP_NULL_LIF_ENTRY_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_ENCAP_NULL_LIF_ENTRY_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_ENCAP_DATA_LIF_ENTRY_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_ENCAP_DATA_LIF_ENTRY_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_ENCAP_AC_ENTRY_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_ENCAP_AC_ENTRY_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_ENCAP_SWAP_COMMAND_ENTRY_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_ENCAP_SWAP_COMMAND_ENTRY_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_ENCAP_PWE_ENTRY_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_ENCAP_PWE_ENTRY_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_ENCAP_POP_COMMAND_ENTRY_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_ENCAP_POP_COMMAND_ENTRY_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_ENCAP_VSI_ENTRY_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_ENCAP_VSI_ENTRY_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_ENCAP_MPLS_ENTRY_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_ENCAP_MPLS_ENTRY_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_ENCAP_IPV4_ENTRY_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_ENCAP_IPV4_ENTRY_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_ENCAP_IPV6_ENTRY_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_ENCAP_IPV6_ENTRY_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_ENCAP_MIRROR_ENTRY_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_ENCAP_MIRROR_ENTRY_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_ENCAP_PORT_ERSPAN_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_ENCAP_PORT_ERSPAN_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_ENCAP_LL_ENTRY_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_ENCAP_LL_ENTRY_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_ENCAP_ENTRY_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_ENCAP_ENTRY_REMOVE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_ENCAP_ENTRY_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_ENCAP_ENTRY_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_ENCAP_ENTRY_TYPE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_ENCAP_ENTRY_TYPE_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_ENCAP_PUSH_PROFILE_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_ENCAP_PUSH_PROFILE_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_ENCAP_PUSH_PROFILE_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_ENCAP_PUSH_PROFILE_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_ENCAP_PUSH_EXP_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_ENCAP_PUSH_EXP_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_ENCAP_PUSH_EXP_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_ENCAP_PUSH_EXP_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_ENCAP_PWE_GLBL_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_ENCAP_PWE_GLBL_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_ENCAP_PWE_GLBL_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_ENCAP_PWE_GLBL_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_ENCAP_GLBL_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_ENCAP_GLBL_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_ENCAP_GLBL_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_ENCAP_GLBL_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_ENCAP_IPV4_TUNNEL_GLBL_SRC_IP_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_ENCAP_IPV4_TUNNEL_GLBL_SRC_IP_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_ENCAP_IPV4_TUNNEL_GLBL_SRC_IP_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_ENCAP_IPV4_TUNNEL_GLBL_SRC_IP_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_ENCAP_IPV4_TUNNEL_GLBL_TTL_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_ENCAP_IPV4_TUNNEL_GLBL_TTL_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_ENCAP_IPV4_TUNNEL_GLBL_TTL_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_ENCAP_IPV4_TUNNEL_GLBL_TTL_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_ENCAP_IPV4_TUNNEL_GLBL_TOS_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_ENCAP_IPV4_TUNNEL_GLBL_TOS_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_ENCAP_IPV4_TUNNEL_GLBL_TOS_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_ENCAP_IPV4_TUNNEL_GLBL_TOS_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_ENCAP_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_INIT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_GET),
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
*     Inits devision of the Egress Encapsulation Table between
 *     the different usages (Link layer/ IP tunnels/ MPLS
 *     tunnels).
 *     This configuration only take effect the entry type is not ARAD_PP_EG_ENCAP_ACCESS_PREFIX_TYPE_OTHER and 
 *     ARAD_PP_EG_ENCAP_ACCESS_PREFIX_TYPE_NONE.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_eg_encap_entry_init(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               lif_eep_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(eg_encap_entry_init,(unit, lif_eep_ndx));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_encap_entry_init()", 0, 0);
}


/*********************************************************************
*     Sets devision of the Egress Encapsulation Table between
 *     the different usages (Link layer/ IP tunnels/ MPLS
 *     tunnels).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_eg_encap_range_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_RANGE_INFO                 *range_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_EG_ENCAP_RANGE_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(range_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(eg_encap_range_info_set,(unit, range_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_eg_encap_range_info_set_print,(unit,range_info));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_encap_range_info_set()", 0, 0);
}

/*********************************************************************
*     Sets devision of the Egress Encapsulation Table between
 *     the different usages (Link layer/ IP tunnels/ MPLS
 *     tunnels).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_eg_encap_range_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT SOC_PPD_EG_ENCAP_RANGE_INFO                 *range_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_EG_ENCAP_RANGE_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(range_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(eg_encap_range_info_get,(unit, range_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_eg_encap_range_info_get_print,(unit));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_encap_range_info_get()", 0, 0);
}

/*********************************************************************
*     Set LIF Editing entry to be NULL Entry.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_eg_encap_null_lif_entry_add(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                lif_eep_ndx,
    SOC_SAND_IN  uint32                                next_eep
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_EG_ENCAP_NULL_LIF_ENTRY_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_TMP_DEVICE_CALL(eg_encap_null_lif_entry_add,(unit, lif_eep_ndx, next_eep));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_eg_encap_null_lif_entry_add_print,(unit,lif_eep_ndx,next_eep));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_encap_null_lif_entry_add()", lif_eep_ndx, 0);
}


/*********************************************************************
*     Set LIF Editing entry to be NULL Entry.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_eg_encap_next_outlif_update(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                lif_eep_ndx,
    SOC_SAND_IN  uint32                                flags,
    SOC_SAND_IN  uint32                                next_outlif
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_EG_ENCAP_NULL_LIF_ENTRY_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(eg_encap_next_outlif_update,(unit, lif_eep_ndx, flags, next_outlif));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_encap_null_lif_entry_add()", lif_eep_ndx, next_outlif);
}


/*********************************************************************
*     Set LIF Editing entry to be NULL Entry.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_eg_encap_lif_field_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                lif_eep_ndx,
    SOC_SAND_IN  uint32                                flags,
    SOC_SAND_OUT  uint32                               *val
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_EG_ENCAP_NULL_LIF_ENTRY_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(eg_encap_lif_field_get,(unit, lif_eep_ndx, flags, val));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_encap_null_lif_entry_add()", lif_eep_ndx, flags);
}

/*********************************************************************
*     Init CUD global extension.
*     
*********************************************************************/
uint32
  soc_ppd_eg_encap_data_local_to_global_cud_init(
    SOC_SAND_IN  int                                 unit
  )
{
    uint32 
        res = SOC_SAND_OK;
    SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_INIT);
    SOC_SAND_CHECK_DRIVER_AND_DEVICE;

    ARAD_PP_DEVICE_CALL(eg_encap_data_local_to_global_cud_init,(unit));
    SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_encap_data_local_to_global_cud_init()", 0, 0);
}

/*********************************************************************
*     Set CUD global extension.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_eg_encap_data_local_to_global_cud_set(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_IN  uint32                                  in_cud,
    SOC_SAND_IN  uint32                                  out_cud
  )
{
    uint32 
        res = SOC_SAND_OK;
    SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_SET);

    SOC_SAND_CHECK_DRIVER_AND_DEVICE;

    ARAD_PP_DEVICE_CALL(eg_encap_data_local_to_global_cud_set,(unit, in_cud, out_cud));

    SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_encap_data_local_to_global_cud_set()", in_cud, out_cud);
}

/*********************************************************************
*     Set CUD global extension.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_eg_encap_data_local_to_global_cud_get(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_IN  uint32                                  in_cud,
    SOC_SAND_OUT uint32*                                 out_cud
  )
{
    uint32 
        res = SOC_SAND_OK;
    SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_GET);

    SOC_SAND_CHECK_DRIVER_AND_DEVICE;

    ARAD_PP_DEVICE_CALL(eg_encap_data_local_to_global_cud_get,(unit, in_cud, out_cud));

    SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_encap_data_local_to_global_cud_get()", in_cud, 0);
}

/*********************************************************************
*     Set LIF Editing entry to be DATA Entry.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_eg_encap_data_lif_entry_add(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_IN  uint32                                  lif_eep_ndx,
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_DATA_INFO                  *data_info,
    SOC_SAND_IN  uint8                                   next_eep_valid,
    SOC_SAND_IN  uint32                                  next_eep
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_EG_ENCAP_NULL_LIF_ENTRY_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  ARAD_PP_DEVICE_CALL(eg_encap_data_lif_entry_add,(unit, lif_eep_ndx, data_info, next_eep_valid, next_eep));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_eg_encap_data_lif_entry_add_print,(unit,lif_eep_ndx,next_eep));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_encap_data_lif_entry_add()", lif_eep_ndx, 0);
}

/*********************************************************************
*     Set LIF Editing entry to hold AC ID. Actually maps from
 *     CUD to AC.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_eg_encap_ac_entry_add(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                lif_eep_ndx,
    SOC_SAND_IN  SOC_PPD_AC_ID                               ac_id
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_EG_ENCAP_AC_ENTRY_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_TMP_DEVICE_CALL(eg_encap_ac_entry_add,(unit, lif_eep_ndx, ac_id));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_eg_encap_ac_entry_add_print,(unit,lif_eep_ndx,ac_id));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_encap_ac_entry_add()", lif_eep_ndx, 0);
}

/*********************************************************************
*     Set LIF Editing entry to hold MPLS LSR SWAP label.
 *     Needed for MPLS multicast services.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_eg_encap_swap_command_entry_add(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                lif_eep_ndx,
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_SWAP_INFO                  *swap_info,
    SOC_SAND_IN  uint32                                next_eep
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_EG_ENCAP_SWAP_COMMAND_ENTRY_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(swap_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(eg_encap_swap_command_entry_add,(unit, lif_eep_ndx, swap_info, next_eep));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_eg_encap_swap_command_entry_add_print,(unit,lif_eep_ndx,swap_info,next_eep));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_encap_swap_command_entry_add()", lif_eep_ndx, 0);
}

/*********************************************************************
*     Set LIF Editing entry to hold PWE info (VC label and
 *     push profile).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_eg_encap_pwe_entry_add(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                lif_eep_ndx,
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_PWE_INFO                   *pwe_info,
    SOC_SAND_IN  uint32                                next_eep
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_EG_ENCAP_PWE_ENTRY_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(pwe_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(eg_encap_pwe_entry_add,(unit, lif_eep_ndx, pwe_info, next_eep));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_eg_encap_pwe_entry_add_print,(unit,lif_eep_ndx,pwe_info,next_eep));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_encap_pwe_entry_add()", lif_eep_ndx, 0);
}

/*********************************************************************
*     Set LIF Editing entry to hold MPLS LSR POP command.
 *     Needed for MPLS multicast services.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_eg_encap_pop_command_entry_add(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                lif_eep_ndx,
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_POP_INFO                   *pop_info,
    SOC_SAND_IN  uint32                                next_eep
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_EG_ENCAP_POP_COMMAND_ENTRY_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(pop_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(eg_encap_pop_command_entry_add,(unit, lif_eep_ndx, pop_info, next_eep));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_eg_encap_pop_command_entry_add_print,(unit,lif_eep_ndx,pop_info,next_eep));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_encap_pop_command_entry_add()", lif_eep_ndx, 0);
}

/*********************************************************************
*     Set LIF Editing entry to hold out-VSI. Needed for
 *     IPv4/IPv6 Multicast Application, and MVR (Multicast VLAN
 *     Replication).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_eg_encap_vsi_entry_add(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  lif_eep_ndx,
    SOC_SAND_IN SOC_PPD_EG_ENCAP_VSI_ENCAP_INFO             *vsi_info, 
    SOC_SAND_IN uint8                                  next_eep_valid, 
    SOC_SAND_IN  uint32                                  next_eep
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_EG_ENCAP_VSI_ENTRY_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_TMP_DEVICE_CALL(eg_encap_vsi_entry_add,(unit, lif_eep_ndx, vsi_info, next_eep_valid, next_eep));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_eg_encap_vsi_entry_add_print,(unit, lif_eep_ndx, vsi_info, next_eep_valid, next_eep));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_encap_vsi_entry_add()", lif_eep_ndx, 0);
}

/*********************************************************************
*     Add MPLS tunnels encapsulation entry to the Tunnels
 *     Editing Table.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_eg_encap_mpls_entry_add(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                tunnel_eep_ndx,
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_MPLS_ENCAP_INFO            *mpls_encap_info,
    SOC_SAND_IN  uint32                                ll_eep
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_EG_ENCAP_MPLS_ENTRY_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(mpls_encap_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(eg_encap_mpls_entry_add,(unit, tunnel_eep_ndx, mpls_encap_info, ll_eep));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_eg_encap_mpls_entry_add_print,(unit,tunnel_eep_ndx,mpls_encap_info,ll_eep));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_encap_mpls_entry_add()", tunnel_eep_ndx, 0);
}

/*********************************************************************
*     Add IPv4 tunnels encapsulation entry to the Egress
 *     Encapsulation Tunnels Editing Table.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_eg_encap_ipv4_entry_add(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                tunnel_eep_ndx,
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_IPV4_ENCAP_INFO            *ipv4_encap_info,
    SOC_SAND_IN  uint32                                ll_eep
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_EG_ENCAP_IPV4_ENTRY_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(ipv4_encap_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(eg_encap_ipv4_entry_add,(unit, tunnel_eep_ndx, ipv4_encap_info, ll_eep));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_eg_encap_ipv4_entry_add_print,(unit,tunnel_eep_ndx,ipv4_encap_info,ll_eep));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_encap_ipv4_entry_add()", tunnel_eep_ndx, 0);
}



/*********************************************************************
*     Add IPV6 tunnels encapsulation entry to the Egress
 *     Encapsulation Tunnels Editing Table.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_eg_encap_ipv6_entry_add(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                tunnel_eep_ndx,
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_IPV6_ENCAP_INFO            *ipv6_encap_info,
    SOC_SAND_IN  uint32                                ll_eep
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_EG_ENCAP_IPV6_ENTRY_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(ipv6_encap_info);

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(eg_encap_ipv6_entry_add,(unit, tunnel_eep_ndx, ipv6_encap_info, ll_eep));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_eg_encap_ipv6_entry_add_print,(unit,tunnel_eep_ndx,ipv6_encap_info,ll_eep));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_encap_ipv6_entry_add()", tunnel_eep_ndx, 0);
}


/*********************************************************************
*     Add overlay arp encapsulation entry. 
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_eg_encap_overlay_arp_data_entry_add(
     SOC_SAND_IN  int                                    unit,
     SOC_SAND_IN  uint32                                 overlay_ll_eep_ndx, 
     SOC_SAND_INOUT  SOC_PPC_EG_ENCAP_OVERLAY_ARP_ENCAP_INFO *ovelay_ll_encap_info
     ) 
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(ovelay_ll_encap_info);

  SOC_PPD_ARAD_OR_JERICHO_ONLY_DEVICE_CALL(eg_encap_overlay_arp_data_entry_add,(unit, overlay_ll_eep_ndx, ovelay_ll_encap_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_eg_encap_overlay_arp_data_entry_add_print,(unit, overlay_ll_eep_ndx, ovelay_ll_encap_info));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_encap_overlay_arp_data_entry_add()", overlay_ll_eep_ndx, 0);
}

/*********************************************************************
*     Add Mirror ERSPAN encapsulation entries to the prge memory.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_eg_encap_mirror_entry_set(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  uint32                              mirror_ndx,
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_MIRROR_ENCAP_INFO *mirror_encap_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_EG_ENCAP_MIRROR_ENTRY_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(mirror_encap_info);

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(eg_encap_mirror_entry_set,(unit, mirror_ndx, mirror_encap_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_eg_encap_mirror_entry_set_print,(unit,mirror_ndx,mirror_encap_info));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_encap_mirror_entry_set()", mirror_ndx, 0);
}

/*********************************************************************
*     Get Mirror ERSPAN encapsulation entries from the prge memory.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_eg_encap_mirror_entry_get(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  uint32                              mirror_ndx,
    SOC_SAND_OUT SOC_PPD_EG_ENCAP_MIRROR_ENCAP_INFO *mirror_encap_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_EG_ENCAP_MIRROR_ENTRY_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(mirror_encap_info);

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(eg_encap_mirror_entry_get,(unit, mirror_ndx, mirror_encap_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_eg_encap_mirror_entry_set_print,(unit,mirror_ndx,mirror_encap_info));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_encap_mirror_entry_get()", mirror_ndx, 0);
}

/*********************************************************************
*     Set whether ERSPAN is disabled/enabled per port.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_eg_encap_port_erspan_set(
    SOC_SAND_IN  int                                 unit,
	SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  SOC_PPD_PORT                           local_port_ndx,
    SOC_SAND_IN  uint8                                  is_erspan
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_EG_ENCAP_PORT_ERSPAN_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(eg_encap_port_erspan_set,(unit, core_id, local_port_ndx, is_erspan));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_encap_port_erspan_set()", local_port_ndx, is_erspan);
}

/*********************************************************************
*     Get whether ERSPAN is disabled/enabled per port.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_eg_encap_port_erspan_get(
    SOC_SAND_IN  int                                 unit,
	SOC_SAND_IN  int                                 	core_id,
    SOC_SAND_IN  SOC_PPD_PORT                           local_port_ndx,
    SOC_SAND_OUT uint8                                  *is_erspan
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_EG_ENCAP_PORT_ERSPAN_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(is_erspan);

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(eg_encap_port_erspan_get,(unit, core_id, local_port_ndx, is_erspan));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_encap_port_erspan_get()", local_port_ndx, 0);
}

/*********************************************************************
*     Add LL encapsulation entry.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_eg_encap_ll_entry_add(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                ll_eep_ndx,
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_LL_INFO                    *ll_encap_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_EG_ENCAP_LL_ENTRY_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(ll_encap_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(eg_encap_ll_entry_add,(unit, ll_eep_ndx, ll_encap_info)); 
  
  SOC_PPD_DO_NOTHING_AND_EXIT;

exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_eg_encap_ll_entry_add_print,(unit, ll_eep_ndx, ll_encap_info));
  
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_encap_ll_entry_add()", ll_eep_ndx, 0);
}

/*********************************************************************
*     Remove entry from the encapsulation Table.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_eg_encap_entry_remove(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_EEP_TYPE                   eep_type_ndx,
    SOC_SAND_IN  uint32                                eep_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_EG_ENCAP_ENTRY_REMOVE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_TMP_DEVICE_CALL(eg_encap_entry_remove,(unit, eep_type_ndx, eep_ndx));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_eg_encap_entry_remove_print,(unit,eep_type_ndx,eep_ndx));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_encap_entry_remove()", 0, eep_ndx);
}

/*********************************************************************
*     Get entry information from the Egress encapsulation
 *     tables.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_eg_encap_entry_type_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 eep_ndx,
    SOC_SAND_OUT SOC_PPD_EG_ENCAP_ENTRY_TYPE                *entry_type
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_EG_ENCAP_ENTRY_TYPE_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(entry_type);

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(eg_encap_entry_type_get,(unit, eep_ndx, entry_type));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_eg_encap_entry_type_get_print,(unit,eep_ndx,entry_type));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_encap_entry_type_get()", 0, eep_ndx);
}

/*********************************************************************
*     Get entry information from the Egress encapsulation
 *     tables.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_eg_encap_entry_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_EEP_TYPE                   eep_type_ndx,
    SOC_SAND_IN  uint32                                eep_ndx,
    SOC_SAND_IN  uint32                                depth,
    SOC_SAND_OUT SOC_PPD_EG_ENCAP_ENTRY_INFO                 encap_entry_info[SOC_PPD_NOF_EG_ENCAP_EEP_TYPES],
    SOC_SAND_OUT uint32                                next_eep[SOC_PPD_NOF_EG_ENCAP_EEP_TYPES],
    SOC_SAND_OUT uint32                                *nof_entries
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_EG_ENCAP_ENTRY_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(nof_entries);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(eg_encap_entry_get,(unit, eep_type_ndx, eep_ndx, depth, encap_entry_info, next_eep, nof_entries));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_eg_encap_get_print,(unit,eep_type_ndx,eep_ndx,depth));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_encap_entry_get()", 0, eep_ndx);
}







/*********************************************************************
*     Parse data info to get overlay arp encap info.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
soc_ppd_eg_encap_entry_data_info_to_overlay_arp_encap_info(
 SOC_SAND_IN  int                                    unit, 
   SOC_SAND_OUT SOC_PPD_EG_ENCAP_ENTRY_INFO                 *encap_entry_info
   ) { 
    uint32
       res = SOC_SAND_OK; 
    
    SOC_SAND_INIT_ERROR_DEFINITIONS(0); 
    
    SOC_SAND_CHECK_DRIVER_AND_DEVICE; 
    
    
    SOC_PPD_ARAD_ONLY_DEVICE_CALL(eg_encap_entry_data_info_to_overlay_arp_encap_info, (unit, encap_entry_info)); 
    
    SOC_PPD_DO_NOTHING_AND_EXIT; 
    
exit:
    SOC_PPD_FUNC_PRINT(soc_ppd_eg_encap_entry_data_info_to_overlay_arp_encap_info, (unit, overlay_ll_eep_ndx, is_overlay_arp_data_entry)); 
    SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_encap_entry_data_info_to_overlay_arp_encap_info()", 0, 0);
}




/*********************************************************************
*     Setting the push profile info - specifying how to build
 *     the label header.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_eg_encap_push_profile_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                profile_ndx,
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_PUSH_PROFILE_INFO          *profile_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_EG_ENCAP_PUSH_PROFILE_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(profile_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(eg_encap_push_profile_info_set,(unit, profile_ndx, profile_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_eg_encap_push_profile_info_set_print,(unit,profile_ndx,profile_info));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_encap_push_profile_info_set()", profile_ndx, 0);
}

/*********************************************************************
*     Setting the push profile info - specifying how to build
 *     the label header.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_eg_encap_push_profile_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                profile_ndx,
    SOC_SAND_OUT SOC_PPD_EG_ENCAP_PUSH_PROFILE_INFO          *profile_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_EG_ENCAP_PUSH_PROFILE_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(profile_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(eg_encap_push_profile_info_get,(unit, profile_ndx, profile_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_eg_encap_push_profile_info_get_print,(unit,profile_ndx));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_encap_push_profile_info_get()", profile_ndx, 0);
}

/*********************************************************************
*     Set the EXP value of the pushed label as mapping of the
 *     TC and DP.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_eg_encap_push_exp_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_PUSH_EXP_KEY               *exp_key,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_EXP                        exp
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_EG_ENCAP_PUSH_EXP_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(exp_key);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(eg_encap_push_exp_info_set,(unit, exp_key, exp));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_eg_encap_push_exp_info_set_print,(unit,exp_key,exp));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_encap_push_exp_info_set()", 0, 0);
}

/*********************************************************************
*     Set the EXP value of the pushed label as mapping of the
 *     TC and DP.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_eg_encap_push_exp_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_PUSH_EXP_KEY               *exp_key,
    SOC_SAND_OUT SOC_SAND_PP_MPLS_EXP                        *exp
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_EG_ENCAP_PUSH_EXP_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(exp_key);
  SOC_SAND_CHECK_NULL_INPUT(exp);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(eg_encap_push_exp_info_get,(unit, exp_key, exp));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_eg_encap_push_exp_info_get_print,(unit,exp_key));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_encap_push_exp_info_get()", 0, 0);
}

/*********************************************************************
*     Set Global information for PWE Encapsulation.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_eg_encap_pwe_glbl_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_PWE_GLBL_INFO              *glbl_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_EG_ENCAP_PWE_GLBL_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(glbl_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(eg_encap_pwe_glbl_info_set,(unit, glbl_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_eg_encap_pwe_glbl_info_set_print,(unit,glbl_info));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_encap_pwe_glbl_info_set()", 0, 0);
}

/*********************************************************************
*     Set Global information for PWE Encapsulation.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_eg_encap_pwe_glbl_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT SOC_PPD_EG_ENCAP_PWE_GLBL_INFO              *glbl_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_EG_ENCAP_PWE_GLBL_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(glbl_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(eg_encap_pwe_glbl_info_get,(unit, glbl_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_eg_encap_pwe_glbl_info_get_print,(unit));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_encap_pwe_glbl_info_get()", 0, 0);
}


/*********************************************************************
*     Set Global information for PWE Encapsulation.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_eg_encap_glbl_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_GLBL_INFO              *glbl_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_EG_ENCAP_GLBL_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(glbl_info);

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(eg_encap_glbl_info_set,(unit, glbl_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_eg_encap_glbl_info_set_print,(unit,glbl_info));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_encap_glbl_info_set()", 0, 0);
}

/*********************************************************************
*     Set Global information for PWE Encapsulation.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_eg_encap_glbl_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT SOC_PPD_EG_ENCAP_GLBL_INFO              *glbl_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_EG_ENCAP_GLBL_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(glbl_info);

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(eg_encap_glbl_info_get,(unit, glbl_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_eg_encap_glbl_info_get_print,(unit));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_encap_glbl_info_get()", 0, 0);
}


/*********************************************************************
*     Set source IP address for IPv4 Tunneling.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_eg_encap_ipv4_tunnel_glbl_src_ip_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                entry_ndx,
    SOC_SAND_IN  uint32                                src_ip
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_EG_ENCAP_IPV4_TUNNEL_GLBL_SRC_IP_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_TMP_DEVICE_CALL(eg_encap_ipv4_tunnel_glbl_src_ip_set,(unit, entry_ndx, src_ip));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_eg_encap_ipv4_tunnel_glbl_src_ip_set_print,(unit,entry_ndx,src_ip));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_encap_ipv4_tunnel_glbl_src_ip_set()", entry_ndx, 0);
}

/*********************************************************************
*     Set source IP address for IPv4 Tunneling.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_eg_encap_ipv4_tunnel_glbl_src_ip_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                entry_ndx,
    SOC_SAND_OUT uint32                                *src_ip
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_EG_ENCAP_IPV4_TUNNEL_GLBL_SRC_IP_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(src_ip);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(eg_encap_ipv4_tunnel_glbl_src_ip_get,(unit, entry_ndx, src_ip));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_eg_encap_ipv4_tunnel_glbl_src_ip_get_print,(unit,entry_ndx));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_encap_ipv4_tunnel_glbl_src_ip_get()", entry_ndx, 0);
}

/*********************************************************************
*     Set TTL for IPv4 Tunneling.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_eg_encap_ipv4_tunnel_glbl_ttl_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                entry_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IP_TTL                          ttl
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_EG_ENCAP_IPV4_TUNNEL_GLBL_TTL_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_TMP_DEVICE_CALL(eg_encap_ipv4_tunnel_glbl_ttl_set,(unit, entry_ndx, ttl));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_eg_encap_ipv4_tunnel_glbl_ttl_set_print,(unit,entry_ndx,ttl));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_encap_ipv4_tunnel_glbl_ttl_set()", entry_ndx, 0);
}

/*********************************************************************
*     Set TTL for IPv4 Tunneling.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_eg_encap_ipv4_tunnel_glbl_ttl_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                entry_ndx,
    SOC_SAND_OUT SOC_SAND_PP_IP_TTL                          *ttl
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_EG_ENCAP_IPV4_TUNNEL_GLBL_TTL_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(ttl);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(eg_encap_ipv4_tunnel_glbl_ttl_get,(unit, entry_ndx, ttl));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_eg_encap_ipv4_tunnel_glbl_ttl_get_print,(unit,entry_ndx));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_encap_ipv4_tunnel_glbl_ttl_get()", entry_ndx, 0);
}

/*********************************************************************
*     Set TOS for IPv4 Tunneling.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_eg_encap_ipv4_tunnel_glbl_tos_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                entry_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IPV4_TOS                        tos
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_EG_ENCAP_IPV4_TUNNEL_GLBL_TOS_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_TMP_DEVICE_CALL(eg_encap_ipv4_tunnel_glbl_tos_set,(unit, entry_ndx, tos));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_eg_encap_ipv4_tunnel_glbl_tos_set_print,(unit,entry_ndx,tos));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_encap_ipv4_tunnel_glbl_tos_set()", entry_ndx, 0);
}

/*********************************************************************
*     Set TOS for IPv4 Tunneling.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_eg_encap_ipv4_tunnel_glbl_tos_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                entry_ndx,
    SOC_SAND_OUT SOC_SAND_PP_IPV4_TOS                        *tos
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_EG_ENCAP_IPV4_TUNNEL_GLBL_TOS_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(tos);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(eg_encap_ipv4_tunnel_glbl_tos_get,(unit, entry_ndx, tos));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_eg_encap_ipv4_tunnel_glbl_tos_get_print,(unit,entry_ndx));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_encap_ipv4_tunnel_glbl_tos_get()", entry_ndx, 0);
}

/*********************************************************************
*     Set MPLS PIPE mode to do copy EXP (1) or set EXP (0).
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_eg_encap_mpls_pipe_mode_is_exp_copy_set(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_IN  uint8                                is_exp_copy
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(eg_encap_mpls_pipe_mode_is_exp_copy_set,(unit, is_exp_copy));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_eg_encap_mpls_pipe_mode_is_exp_copy_set,(unit,is_exp_copy));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_encap_mpls_pipe_mode_is_exp_copy_set()", is_exp_copy, 0);
}

/*********************************************************************
*     Get MPLS PIPE mode to do copy EXP (1) or set EXP (0).
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_eg_encap_mpls_pipe_mode_is_exp_copy_get(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_OUT uint8                                *is_exp_copy
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(eg_encap_mpls_pipe_mode_is_exp_copy_get,(unit, is_exp_copy));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_eg_encap_mpls_pipe_mode_is_exp_copy_get,(unit,is_exp_copy));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_eg_encap_mpls_pipe_mode_is_exp_copy_get()", 0, 0);
}

/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     soc_ppd_api_eg_encap module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_PROCEDURE_DESC_ELEMENT*
  soc_ppd_eg_encap_get_procs_ptr(
  )
{
  return Ppd_procedure_desc_element_eg_encap;
}
void
  SOC_PPD_EG_ENCAP_RANGE_INFO_clear(
    SOC_SAND_OUT SOC_PPD_EG_ENCAP_RANGE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_ENCAP_RANGE_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_EG_ENCAP_SWAP_INFO_clear(
    SOC_SAND_OUT SOC_PPD_EG_ENCAP_SWAP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_ENCAP_SWAP_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_EG_ENCAP_PWE_INFO_clear(
    SOC_SAND_OUT SOC_PPD_EG_ENCAP_PWE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_ENCAP_PWE_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_EG_ENCAP_POP_INTO_ETH_INFO_clear(
    SOC_SAND_OUT SOC_PPD_EG_ENCAP_POP_INTO_ETH_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_ENCAP_POP_INTO_ETH_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_EG_ENCAP_POP_INFO_clear(
    SOC_SAND_OUT SOC_PPD_EG_ENCAP_POP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_ENCAP_POP_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_EG_ENCAP_VSI_ENCAP_INFO_clear(
    SOC_SAND_OUT SOC_PPD_EG_ENCAP_VSI_ENCAP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_ENCAP_VSI_ENCAP_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_EG_ENCAP_MPLS_TUNNEL_INFO_clear(
    SOC_SAND_OUT SOC_PPD_EG_ENCAP_MPLS_TUNNEL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_ENCAP_MPLS_TUNNEL_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_EG_ENCAP_MPLS_ENCAP_INFO_clear(
    SOC_SAND_OUT SOC_PPD_EG_ENCAP_MPLS_ENCAP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_ENCAP_MPLS_ENCAP_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_EG_ENCAP_IPV4_TUNNEL_INFO_clear(
    SOC_SAND_OUT SOC_PPD_EG_ENCAP_IPV4_TUNNEL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_ENCAP_IPV4_TUNNEL_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_EG_ENCAP_IPV4_ENCAP_INFO_clear(
    SOC_SAND_OUT SOC_PPD_EG_ENCAP_IPV4_ENCAP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_ENCAP_IPV4_ENCAP_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_EG_ENCAP_IPV6_TUNNEL_INFO_clear(
    SOC_SAND_OUT SOC_PPD_EG_ENCAP_IPV6_TUNNEL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_ENCAP_IPV6_TUNNEL_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_EG_ENCAP_IPV6_ENCAP_INFO_clear(
    SOC_SAND_OUT SOC_PPD_EG_ENCAP_IPV6_ENCAP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_ENCAP_IPV6_ENCAP_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_EG_ENCAP_MIRROR_TUNNEL_INFO_clear(
    SOC_SAND_OUT SOC_PPD_EG_ENCAP_MIRROR_TUNNEL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_ENCAP_MIRROR_TUNNEL_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_EG_ENCAP_MIRROR_ENCAP_INFO_clear(
    SOC_SAND_OUT SOC_PPD_EG_ENCAP_MIRROR_ENCAP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_ENCAP_MIRROR_ENCAP_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_EG_ENCAP_LL_INFO_clear(
    SOC_SAND_OUT SOC_PPD_EG_ENCAP_LL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_ENCAP_LL_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_EG_ENCAP_PUSH_PROFILE_INFO_clear(
    SOC_SAND_OUT SOC_PPD_EG_ENCAP_PUSH_PROFILE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_ENCAP_PUSH_PROFILE_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_EG_ENCAP_PUSH_EXP_KEY_clear(
    SOC_SAND_OUT SOC_PPD_EG_ENCAP_PUSH_EXP_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_ENCAP_PUSH_EXP_KEY_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_EG_ENCAP_PWE_GLBL_INFO_clear(
    SOC_SAND_OUT SOC_PPD_EG_ENCAP_PWE_GLBL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_ENCAP_PWE_GLBL_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_EG_ENCAP_GLBL_INFO_clear(
    SOC_SAND_OUT SOC_PPD_EG_ENCAP_GLBL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_ENCAP_GLBL_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_EG_ENCAP_ENTRY_VALUE_clear(
    SOC_SAND_OUT SOC_PPD_EG_ENCAP_ENTRY_VALUE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_ENCAP_ENTRY_VALUE_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_EG_ENCAP_ENTRY_INFO_clear(
    SOC_SAND_OUT SOC_PPD_EG_ENCAP_ENTRY_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_ENCAP_ENTRY_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  SOC_PPD_EG_ENCAP_DATA_INFO_clear(
    SOC_SAND_OUT SOC_PPD_EG_ENCAP_DATA_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_ENCAP_DATA_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}



#if SOC_PPD_DEBUG_IS_LVL1

const char*
  SOC_PPD_EG_ENCAP_EEP_TYPE_to_string(
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_EEP_TYPE enum_val
  )
{
  return SOC_PPC_EG_ENCAP_EEP_TYPE_to_string(enum_val);
}

const char*
  SOC_PPD_EG_ENCAP_ENCAPSULATION_MODE_to_string(
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_ENCAPSULATION_MODE enum_val
  )
{
  return SOC_PPC_EG_ENCAP_ENCAPSULATION_MODE_to_string(enum_val);
}

const char*
  SOC_PPD_EG_ENCAP_EXP_MARK_MODE_to_string(
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_EXP_MARK_MODE enum_val
  )
{
  return SOC_PPC_EG_ENCAP_EXP_MARK_MODE_to_string(enum_val);
}

const char*
  SOC_PPD_EG_ENCAP_ENTRY_TYPE_to_string(
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_ENTRY_TYPE enum_val
  )
{
  return SOC_PPC_EG_ENCAP_ENTRY_TYPE_to_string(enum_val);
}

const char*
  SOC_PPD_EG_ENCAP_ACCESS_PHASE_to_string(
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_ACCESS_PHASE enum_val
  )
{
  return SOC_PPC_EG_ENCAP_ACCESS_PHASE_to_string(enum_val);
}

void
  SOC_PPD_EG_ENCAP_RANGE_INFO_print(
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_RANGE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_ENCAP_RANGE_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_EG_ENCAP_SWAP_INFO_print(
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_SWAP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_ENCAP_SWAP_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_EG_ENCAP_PWE_INFO_print(
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_PWE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_ENCAP_PWE_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_EG_ENCAP_POP_INTO_ETH_INFO_print(
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_POP_INTO_ETH_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_ENCAP_POP_INTO_ETH_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_EG_ENCAP_POP_INFO_print(
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_POP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_ENCAP_POP_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_EG_ENCAP_MPLS_TUNNEL_INFO_print(
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_MPLS_TUNNEL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_ENCAP_MPLS_TUNNEL_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_EG_ENCAP_MPLS_ENCAP_INFO_print(
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_MPLS_ENCAP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_ENCAP_MPLS_ENCAP_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_EG_ENCAP_IPV4_TUNNEL_INFO_print(
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_IPV4_TUNNEL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_ENCAP_IPV4_TUNNEL_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_EG_ENCAP_IPV4_ENCAP_INFO_print(
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_IPV4_ENCAP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_ENCAP_IPV4_ENCAP_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_EG_ENCAP_IPV6_TUNNEL_INFO_print(
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_IPV6_TUNNEL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_ENCAP_IPV6_TUNNEL_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_EG_ENCAP_IPV6_ENCAP_INFO_print(
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_IPV6_ENCAP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_ENCAP_IPV6_ENCAP_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_EG_ENCAP_MIRROR_TUNNEL_INFO_print(
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_MIRROR_TUNNEL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_ENCAP_MIRROR_TUNNEL_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_EG_ENCAP_MIRROR_ENCAP_INFO_print(
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_MIRROR_ENCAP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_ENCAP_MIRROR_ENCAP_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_EG_ENCAP_LL_INFO_print(
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_LL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_ENCAP_LL_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_EG_ENCAP_PUSH_PROFILE_INFO_print(
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_PUSH_PROFILE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_ENCAP_PUSH_PROFILE_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_EG_ENCAP_PUSH_EXP_KEY_print(
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_PUSH_EXP_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_ENCAP_PUSH_EXP_KEY_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_EG_ENCAP_PWE_GLBL_INFO_print(
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_PWE_GLBL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_ENCAP_PWE_GLBL_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_EG_ENCAP_GLBL_INFO_print(
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_GLBL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_ENCAP_GLBL_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  SOC_PPD_EG_ENCAP_ENTRY_VALUE_print(
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_ENTRY_VALUE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_ENCAP_ENTRY_VALUE_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_EG_ENCAP_ENTRY_INFO_print(
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_ENTRY_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_ENCAP_ENTRY_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif /* SOC_PPD_DEBUG_IS_LVL1 */

#if SOC_PPD_DEBUG_IS_LVL3

void
  soc_ppd_eg_encap_range_info_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_RANGE_INFO                 *range_info
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "range_info:")));
  SOC_PPD_EG_ENCAP_RANGE_INFO_print((range_info));

  return;
}
void
  soc_ppd_eg_encap_range_info_get_print(
    SOC_SAND_IN  int                               unit
  )
{

  return;
}
void
  soc_ppd_eg_encap_null_lif_entry_add_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                lif_eep_ndx,
    SOC_SAND_IN  uint32                                next_eep
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "lif_eep_ndx: %lu\n\r"),lif_eep_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "next_eep: %lu\n\r"),next_eep));

  return;
}
void
  soc_ppd_eg_encap_ac_entry_add_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                lif_eep_ndx,
    SOC_SAND_IN  SOC_PPD_AC_ID                               ac_id
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "lif_eep_ndx: %lu\n\r"),lif_eep_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "ac_id: %lu\n\r"),ac_id));

  return;
}
void
  soc_ppd_eg_encap_swap_command_entry_add_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                lif_eep_ndx,
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_SWAP_INFO                  *swap_info,
    SOC_SAND_IN  uint32                                next_eep
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "lif_eep_ndx: %lu\n\r"),lif_eep_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "swap_info:")));
  SOC_PPD_EG_ENCAP_SWAP_INFO_print((swap_info));

  LOG_CLI((BSL_META_U(unit,
                      "next_eep: %lu\n\r"),next_eep));

  return;
}
void
  soc_ppd_eg_encap_pwe_entry_add_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                lif_eep_ndx,
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_PWE_INFO                   *pwe_info,
    SOC_SAND_IN  uint32                                next_eep
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "lif_eep_ndx: %lu\n\r"),lif_eep_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "pwe_info:")));
  SOC_PPD_EG_ENCAP_PWE_INFO_print((pwe_info));

  LOG_CLI((BSL_META_U(unit,
                      "next_eep: %lu\n\r"),next_eep));

  return;
}
void
  soc_ppd_eg_encap_pop_command_entry_add_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                lif_eep_ndx,
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_POP_INFO                   *pop_info,
    SOC_SAND_IN  uint32                                next_eep
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "lif_eep_ndx: %lu\n\r"),lif_eep_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "pop_info:")));
  SOC_PPD_EG_ENCAP_POP_INFO_print((pop_info));

  LOG_CLI((BSL_META_U(unit,
                      "next_eep: %lu\n\r"),next_eep));

  return;
}
void
  soc_ppd_eg_encap_vsi_entry_add_print(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  lif_eep_ndx,
    SOC_SAND_IN ARAD_PP_EG_ENCAP_VSI_ENCAP_INFO             *vsi_info, 
    SOC_SAND_IN uint8                                  next_eep_valid, 
    SOC_SAND_IN  uint32                                  next_eep
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "lif_eep_ndx: %lu\n\r"),lif_eep_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "vsi_info: ")));
  ARAD_PP_EG_ENCAP_VSI_ENCAP_INFO_print(vsi_info);

  LOG_CLI((BSL_META_U(unit,
                      "next_eep_valid: %B\n\r"),next_eep_valid));
  LOG_CLI((BSL_META_U(unit,
                      "next_eep: %lu\n\r"),next_eep));

  return;
}
void
  soc_ppd_eg_encap_mpls_entry_add_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                tunnel_eep_ndx,
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_MPLS_ENCAP_INFO            *mpls_encap_info,
    SOC_SAND_IN  uint32                                ll_eep
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "tunnel_eep_ndx: %lu\n\r"),tunnel_eep_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "mpls_encap_info:")));
  SOC_PPD_EG_ENCAP_MPLS_ENCAP_INFO_print((mpls_encap_info));

  LOG_CLI((BSL_META_U(unit,
                      "ll_eep: %lu\n\r"),ll_eep));

  return;
}
void
  soc_ppd_eg_encap_ipv4_entry_add_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                tunnel_eep_ndx,
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_IPV4_ENCAP_INFO            *ipv4_encap_info,
    SOC_SAND_IN  uint32                                ll_eep
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "tunnel_eep_ndx: %lu\n\r"),tunnel_eep_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "ipv4_encap_info:")));
  SOC_PPD_EG_ENCAP_IPV4_ENCAP_INFO_print((ipv4_encap_info));

  LOG_CLI((BSL_META_U(unit,
                      "ll_eep: %lu\n\r"),ll_eep));

  return;
}
void
  soc_ppd_eg_encap_ipv6_entry_add_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                tunnel_eep_ndx,
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_IPV6_ENCAP_INFO            *ipv6_encap_info,
    SOC_SAND_IN  uint32                                ll_eep
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "tunnel_eep_ndx: %lu\n\r"),tunnel_eep_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "ipv6_encap_info:")));
  SOC_PPD_EG_ENCAP_IPV6_ENCAP_INFO_print((ipv6_encap_info));

  LOG_CLI((BSL_META_U(unit,
                      "ll_eep: %lu\n\r"),ll_eep));

  return;
}
void
  soc_ppd_eg_encap_mirror_entry_set_print(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  uint32                              tunnel_eep_ndx,
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_MIRROR_ENCAP_INFO *mirror_encap_info
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "tunnel_eep_ndx: %lu\n\r"),tunnel_eep_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "mirror_encap_info:")));
  SOC_PPD_EG_ENCAP_MIRROR_ENCAP_INFO_print((mirror_encap_info));

  return;
}
void
  soc_ppd_eg_encap_ll_entry_add_print(
      SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                ll_eep_ndx,
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_LL_INFO                    *ll_encap_info,
    SOC_SAND_IN uint8                                out_ac_valid, 
    SOC_SAND_IN uint32                                 out_ac_lsb
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "ll_eep_ndx: %lu\n\r"),ll_eep_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "ll_encap_info:")));
  SOC_PPD_EG_ENCAP_LL_INFO_print((ll_encap_info));
  return;
}
void
  soc_ppd_eg_encap_entry_remove_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_EEP_TYPE                   eep_type_ndx,
    SOC_SAND_IN  uint32                                eep_ndx
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "eep_type_ndx %s "), SOC_PPD_EG_ENCAP_EEP_TYPE_to_string(eep_type_ndx)));

  LOG_CLI((BSL_META_U(unit,
                      "eep_ndx: %lu\n\r"),eep_ndx));

  return;
}
void
  soc_ppd_eg_encap_entry_type_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                eep_ndx
  )
{
  LOG_CLI((BSL_META_U(unit,
                      "eep_ndx: %lu\n\r"),eep_ndx));

  return;
}
void
  soc_ppd_eg_encap_entry_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_EEP_TYPE                   eep_type_ndx,
    SOC_SAND_IN  uint32                                eep_ndx,
    SOC_SAND_IN  uint32                                depth
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "eep_type_ndx %s "), SOC_PPD_EG_ENCAP_EEP_TYPE_to_string(eep_type_ndx)));

  LOG_CLI((BSL_META_U(unit,
                      "eep_ndx: %lu\n\r"),eep_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "depth: %lu\n\r"),depth));

  return;
}
void
  soc_ppd_eg_encap_push_profile_info_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                profile_ndx,
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_PUSH_PROFILE_INFO          *profile_info
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "profile_ndx: %lu\n\r"),profile_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "profile_info:")));
  SOC_PPD_EG_ENCAP_PUSH_PROFILE_INFO_print((profile_info));

  return;
}
void
  soc_ppd_eg_encap_push_profile_info_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                profile_ndx
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "profile_ndx: %lu\n\r"),profile_ndx));

  return;
}
void
  soc_ppd_eg_encap_push_exp_info_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_PUSH_EXP_KEY               *exp_key,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_EXP                        exp
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "exp_key:")));
  SOC_PPD_EG_ENCAP_PUSH_EXP_KEY_print((exp_key));

  LOG_CLI((BSL_META_U(unit,
                      "exp: %u\n\r"), exp));

  return;
}
void
  soc_ppd_eg_encap_push_exp_info_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_PUSH_EXP_KEY               *exp_key
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "exp_key:")));
  SOC_PPD_EG_ENCAP_PUSH_EXP_KEY_print((exp_key));

  return;
}
void
  soc_ppd_eg_encap_pwe_glbl_info_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_PWE_GLBL_INFO              *glbl_info
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "glbl_info:")));
  SOC_PPD_EG_ENCAP_PWE_GLBL_INFO_print((glbl_info));

  return;
}
void
  soc_ppd_eg_encap_pwe_glbl_info_get_print(
    SOC_SAND_IN  int                               unit
  )
{

  return;
}
void
  soc_ppd_eg_encap_glbl_info_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_GLBL_INFO              *glbl_info
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "glbl_info:")));
  SOC_PPD_EG_ENCAP_GLBL_INFO_print((glbl_info));

  return;
}
void
  soc_ppd_eg_encap_glbl_info_get_print(
    SOC_SAND_IN  int                               unit
  )
{

  return;
}
void
  soc_ppd_eg_encap_ipv4_tunnel_glbl_src_ip_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                entry_ndx,
    SOC_SAND_IN  uint32                                src_ip
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "entry_ndx: %lu\n\r"),entry_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "src_ip: %lu\n\r"),src_ip));

  return;
}
void
  soc_ppd_eg_encap_ipv4_tunnel_glbl_src_ip_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                entry_ndx
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "entry_ndx: %lu\n\r"),entry_ndx));

  return;
}
void
  soc_ppd_eg_encap_ipv4_tunnel_glbl_ttl_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                entry_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IP_TTL                          ttl
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "entry_ndx: %lu\n\r"),entry_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "ttl: %lu\n\r"),ttl));

  return;
}
void
  soc_ppd_eg_encap_ipv4_tunnel_glbl_ttl_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                entry_ndx
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "entry_ndx: %lu\n\r"),entry_ndx));

  return;
}
void
  soc_ppd_eg_encap_ipv4_tunnel_glbl_tos_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                entry_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IPV4_TOS                        tos
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "entry_ndx: %lu\n\r"),entry_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "tos: %lu\n\r"),tos));

  return;
}
void
  soc_ppd_eg_encap_ipv4_tunnel_glbl_tos_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                entry_ndx
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "entry_ndx: %lu\n\r"),entry_ndx));

  return;
}
#endif /* SOC_PPD_DEBUG_IS_LVL3 */
/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

