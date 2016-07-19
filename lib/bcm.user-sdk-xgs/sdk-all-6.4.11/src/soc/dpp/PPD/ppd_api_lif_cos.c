/* $Id: ppd_api_lif_cos.c,v 1.12 Broadcom SDK $
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
* FILENAME:       DuneDriver/ppd/src/soc_ppd_api_lif_cos.c
*
* MODULE PREFIX:  soc_ppd_lif
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
#include <soc/dpp/PPD/ppd_api_lif_cos.h>
#ifdef LINK_PB_LIBRARIES
  #include <soc/dpp/Petra/PB_PP/pb_pp_api_lif_cos.h>
#endif
#ifdef LINK_T20E_LIBRARIES
  #include <soc/dpp/T20E/t20e_api_lif_cos.h>
#endif
#ifdef LINK_ARAD_LIBRARIES
  #include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_lif_cos.h>
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
    Ppd_procedure_desc_element_lif_cos[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LIF_COS_AC_PROFILE_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LIF_COS_AC_PROFILE_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LIF_COS_AC_PROFILE_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LIF_COS_AC_PROFILE_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LIF_COS_PWE_PROFILE_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LIF_COS_PWE_PROFILE_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LIF_COS_PWE_PROFILE_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LIF_COS_PWE_PROFILE_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LIF_COS_PROFILE_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LIF_COS_PROFILE_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LIF_COS_PROFILE_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LIF_COS_PROFILE_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LIF_COS_PROFILE_MAP_L2_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LIF_COS_PROFILE_MAP_L2_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LIF_COS_PROFILE_MAP_L2_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LIF_COS_PROFILE_MAP_L2_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LIF_COS_PROFILE_MAP_IP_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LIF_COS_PROFILE_MAP_IP_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LIF_COS_PROFILE_MAP_IP_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LIF_COS_PROFILE_MAP_IP_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LIF_COS_PROFILE_MAP_MPLS_LABEL_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LIF_COS_PROFILE_MAP_MPLS_LABEL_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LIF_COS_PROFILE_MAP_MPLS_LABEL_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LIF_COS_PROFILE_MAP_MPLS_LABEL_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LIF_COS_PROFILE_MAP_TC_DP_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LIF_COS_PROFILE_MAP_TC_DP_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LIF_COS_PROFILE_MAP_TC_DP_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LIF_COS_PROFILE_MAP_TC_DP_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LIF_COS_OPCODE_TYPES_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LIF_COS_OPCODE_TYPES_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LIF_COS_OPCODE_TYPES_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LIF_COS_OPCODE_TYPES_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LIF_COS_OPCODE_IPV6_TOS_MAP_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LIF_COS_OPCODE_IPV6_TOS_MAP_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LIF_COS_OPCODE_IPV6_TOS_MAP_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LIF_COS_OPCODE_IPV6_TOS_MAP_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LIF_COS_OPCODE_IPV4_TOS_MAP_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LIF_COS_OPCODE_IPV4_TOS_MAP_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LIF_COS_OPCODE_IPV4_TOS_MAP_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LIF_COS_OPCODE_IPV4_TOS_MAP_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LIF_COS_OPCODE_TC_DP_MAP_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LIF_COS_OPCODE_TC_DP_MAP_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LIF_COS_OPCODE_TC_DP_MAP_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LIF_COS_OPCODE_TC_DP_MAP_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LIF_COS_OPCODE_VLAN_TAG_MAP_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LIF_COS_OPCODE_VLAN_TAG_MAP_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LIF_COS_OPCODE_VLAN_TAG_MAP_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LIF_COS_OPCODE_VLAN_TAG_MAP_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LIF_COS_GET_PROCS_PTR),
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
*     Sets COS information of AC COS Profile including (FORCE
 *     to const values, map field from the packet and select
 *     mapping table)
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_lif_cos_ac_profile_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                profile_ndx,
    SOC_SAND_IN  SOC_PPD_LIF_COS_AC_PROFILE_INFO             *profile_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_LIF_COS_AC_PROFILE_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(profile_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(lif_cos_ac_profile_info_set,(unit, profile_ndx, profile_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_lif_cos_ac_profile_info_set_print,(unit,profile_ndx,profile_info));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_lif_cos_ac_profile_info_set()", profile_ndx, 0);
}

/*********************************************************************
*     Sets COS information of AC COS Profile including (FORCE
 *     to const values, map field from the packet and select
 *     mapping table)
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_lif_cos_ac_profile_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                profile_ndx,
    SOC_SAND_OUT SOC_PPD_LIF_COS_AC_PROFILE_INFO             *profile_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_LIF_COS_AC_PROFILE_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(profile_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(lif_cos_ac_profile_info_get,(unit, profile_ndx, profile_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_lif_cos_ac_profile_info_get_print,(unit,profile_ndx));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_lif_cos_ac_profile_info_get()", profile_ndx, 0);
}

/*********************************************************************
*     Sets COS Profile information (FORCE to const values, map
 *     field from the packet and select mapping table)
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_lif_cos_pwe_profile_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                profile_ndx,
    SOC_SAND_IN  SOC_PPD_LIF_COS_PWE_PROFILE_INFO            *profile_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_LIF_COS_PWE_PROFILE_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(profile_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(lif_cos_pwe_profile_info_set,(unit, profile_ndx, profile_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_lif_cos_pwe_profile_info_set_print,(unit,profile_ndx,profile_info));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_lif_cos_pwe_profile_info_set()", profile_ndx, 0);
}

/*********************************************************************
*     Sets COS Profile information (FORCE to const values, map
 *     field from the packet and select mapping table)
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_lif_cos_pwe_profile_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                profile_ndx,
    SOC_SAND_OUT SOC_PPD_LIF_COS_PWE_PROFILE_INFO            *profile_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_LIF_COS_PWE_PROFILE_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(profile_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(lif_cos_pwe_profile_info_get,(unit, profile_ndx, profile_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_lif_cos_pwe_profile_info_get_print,(unit,profile_ndx));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_lif_cos_pwe_profile_info_get()", profile_ndx, 0);
}

/*********************************************************************
*     Set CoS Profile attributes. Incoming LIFs are mapped to
 *     CoS Profile. CoS Profile '0' defined to keep the previous
 *     settings. The other CoS profiles are configured by this
 *     function.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_lif_cos_profile_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                cos_profile_ndx,
    SOC_SAND_IN  SOC_PPD_LIF_COS_PROFILE_INFO                *profile_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_LIF_COS_PROFILE_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(profile_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(lif_cos_profile_info_set,(unit, cos_profile_ndx, profile_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_lif_cos_profile_info_set_print,(unit,cos_profile_ndx,profile_info));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_lif_cos_profile_info_set()", cos_profile_ndx, 0);
}

/*********************************************************************
*     Set CoS Profile attributes. Incoming LIFs are mapped to
 *     CoS Profile. CoS Profile '0' defined to keep the previous
 *     settings. The other CoS profiles are configured by this
 *     function.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_lif_cos_profile_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                cos_profile_ndx,
    SOC_SAND_OUT SOC_PPD_LIF_COS_PROFILE_INFO                *profile_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_LIF_COS_PROFILE_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(profile_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(lif_cos_profile_info_get,(unit, cos_profile_ndx, profile_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_lif_cos_profile_info_get_print,(unit,cos_profile_ndx));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_lif_cos_profile_info_get()", cos_profile_ndx, 0);
}

/*********************************************************************
*     Set mapping from L2 VLAN Tag fields to DP and TC.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_lif_cos_profile_map_l2_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                map_tbl_ndx,
    SOC_SAND_IN  SOC_PPD_LIF_COS_PROFILE_MAP_TBL_L2_KEY      *map_key,
    SOC_SAND_IN  SOC_PPD_LIF_COS_PROFILE_MAP_TBL_ENTRY       *map_value
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_LIF_COS_PROFILE_MAP_L2_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(map_key);
  SOC_SAND_CHECK_NULL_INPUT(map_value);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(lif_cos_profile_map_l2_info_set,(unit, map_tbl_ndx, map_key, map_value));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_lif_cos_profile_map_l2_info_set_print,(unit,map_tbl_ndx,map_key,map_value));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_lif_cos_profile_map_l2_info_set()", map_tbl_ndx, 0);
}

/*********************************************************************
*     Set mapping from L2 VLAN Tag fields to DP and TC.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_lif_cos_profile_map_l2_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                map_tbl_ndx,
    SOC_SAND_IN  SOC_PPD_LIF_COS_PROFILE_MAP_TBL_L2_KEY      *map_key,
    SOC_SAND_OUT SOC_PPD_LIF_COS_PROFILE_MAP_TBL_ENTRY       *map_value
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_LIF_COS_PROFILE_MAP_L2_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(map_key);
  SOC_SAND_CHECK_NULL_INPUT(map_value);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(lif_cos_profile_map_l2_info_get,(unit, map_tbl_ndx, map_key, map_value));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_lif_cos_profile_map_l2_info_get_print,(unit,map_tbl_ndx,map_key));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_lif_cos_profile_map_l2_info_get()", map_tbl_ndx, 0);
}

/*********************************************************************
*     Set mapping from IP header fields (TOS / DSCP) to DP and
 *     TC.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_lif_cos_profile_map_ip_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                map_tbl_ndx,
    SOC_SAND_IN  SOC_PPD_LIF_COS_PROFILE_MAP_TBL_IP_KEY      *map_key,
    SOC_SAND_IN  SOC_PPD_LIF_COS_PROFILE_MAP_TBL_ENTRY       *map_value
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_LIF_COS_PROFILE_MAP_IP_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(map_key);
  SOC_SAND_CHECK_NULL_INPUT(map_value);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(lif_cos_profile_map_ip_info_set,(unit, map_tbl_ndx, map_key, map_value));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_lif_cos_profile_map_ip_info_set_print,(unit,map_tbl_ndx,map_key,map_value));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_lif_cos_profile_map_ip_info_set()", map_tbl_ndx, 0);
}

/*********************************************************************
*     Set mapping from IP header fields (TOS / DSCP) to DP and
 *     TC.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_lif_cos_profile_map_ip_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                map_tbl_ndx,
    SOC_SAND_IN  SOC_PPD_LIF_COS_PROFILE_MAP_TBL_IP_KEY      *map_key,
    SOC_SAND_OUT SOC_PPD_LIF_COS_PROFILE_MAP_TBL_ENTRY       *map_value
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_LIF_COS_PROFILE_MAP_IP_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(map_key);
  SOC_SAND_CHECK_NULL_INPUT(map_value);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(lif_cos_profile_map_ip_info_get,(unit, map_tbl_ndx, map_key, map_value));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_lif_cos_profile_map_ip_info_get_print,(unit,map_tbl_ndx,map_key));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_lif_cos_profile_map_ip_info_get()", map_tbl_ndx, 0);
}

/*********************************************************************
*     Set mapping from label fields (EXP) to DP and TC.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_lif_cos_profile_map_mpls_label_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                map_tbl_ndx,
    SOC_SAND_IN  SOC_PPD_LIF_COS_PROFILE_MAP_TBL_MPLS_KEY    *map_key,
    SOC_SAND_IN  SOC_PPD_LIF_COS_PROFILE_MAP_TBL_ENTRY       *map_value
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_LIF_COS_PROFILE_MAP_MPLS_LABEL_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(map_key);
  SOC_SAND_CHECK_NULL_INPUT(map_value);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(lif_cos_profile_map_mpls_label_info_set,(unit, map_tbl_ndx, map_key, map_value));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_lif_cos_profile_map_mpls_label_info_set_print,(unit,map_tbl_ndx,map_key,map_value));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_lif_cos_profile_map_mpls_label_info_set()", map_tbl_ndx, 0);
}

/*********************************************************************
*     Set mapping from label fields (EXP) to DP and TC.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_lif_cos_profile_map_mpls_label_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                map_tbl_ndx,
    SOC_SAND_IN  SOC_PPD_LIF_COS_PROFILE_MAP_TBL_MPLS_KEY    *map_key,
    SOC_SAND_OUT SOC_PPD_LIF_COS_PROFILE_MAP_TBL_ENTRY       *map_value
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_LIF_COS_PROFILE_MAP_MPLS_LABEL_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(map_key);
  SOC_SAND_CHECK_NULL_INPUT(map_value);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(lif_cos_profile_map_mpls_label_info_get,(unit, map_tbl_ndx, map_key, map_value));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_lif_cos_profile_map_mpls_label_info_get_print,(unit,map_tbl_ndx,map_key));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_lif_cos_profile_map_mpls_label_info_get()", map_tbl_ndx, 0);
}

/*********************************************************************
*     Set remapping from TC and DP to TC and DP.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_lif_cos_profile_map_tc_dp_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                map_tbl_ndx,
    SOC_SAND_IN  SOC_PPD_LIF_COS_PROFILE_MAP_TBL_TC_DP_KEY   *map_key,
    SOC_SAND_IN  SOC_PPD_LIF_COS_PROFILE_MAP_TBL_ENTRY       *map_value
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_LIF_COS_PROFILE_MAP_TC_DP_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(map_key);
  SOC_SAND_CHECK_NULL_INPUT(map_value);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(lif_cos_profile_map_tc_dp_info_set,(unit, map_tbl_ndx, map_key, map_value));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_lif_cos_profile_map_tc_dp_info_set_print,(unit,map_tbl_ndx,map_key,map_value));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_lif_cos_profile_map_tc_dp_info_set()", map_tbl_ndx, 0);
}

/*********************************************************************
*     Set remapping from TC and DP to TC and DP.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_lif_cos_profile_map_tc_dp_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                map_tbl_ndx,
    SOC_SAND_IN  SOC_PPD_LIF_COS_PROFILE_MAP_TBL_TC_DP_KEY   *map_key,
    SOC_SAND_OUT SOC_PPD_LIF_COS_PROFILE_MAP_TBL_ENTRY       *map_value
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_LIF_COS_PROFILE_MAP_TC_DP_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(map_key);
  SOC_SAND_CHECK_NULL_INPUT(map_value);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(lif_cos_profile_map_tc_dp_info_get,(unit, map_tbl_ndx, map_key, map_value));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_lif_cos_profile_map_tc_dp_info_get_print,(unit,map_tbl_ndx,map_key));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_lif_cos_profile_map_tc_dp_info_get()", map_tbl_ndx, 0);
}

/*********************************************************************
*     Set the Opcode type. Set the mapping of Class of Service
 *     attributes to the AC-Offset.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_lif_cos_opcode_types_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                opcode_ndx,
    SOC_SAND_IN  SOC_PPD_LIF_COS_OPCODE_TYPE                 opcode_type
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_LIF_COS_OPCODE_TYPES_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_TMP_DEVICE_CALL(lif_cos_opcode_types_set,(unit, opcode_ndx, opcode_type));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_lif_cos_opcode_types_set_print,(unit,opcode_ndx,opcode_type));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_lif_cos_opcode_types_set()", opcode_ndx, 0);
}

/*********************************************************************
*     Set the Opcode type. Set the mapping of Class of Service
 *     attributes to the AC-Offset.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_lif_cos_opcode_types_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                opcode_ndx,
    SOC_SAND_OUT SOC_PPD_LIF_COS_OPCODE_TYPE                 *opcode_type
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_LIF_COS_OPCODE_TYPES_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(opcode_type);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(lif_cos_opcode_types_get,(unit, opcode_ndx, opcode_type));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_lif_cos_opcode_types_get_print,(unit,opcode_ndx));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_lif_cos_opcode_types_get()", opcode_ndx, 0);
}

/*********************************************************************
*     Set the Opcode according to IPv6 TOS field
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_lif_cos_opcode_ipv6_tos_map_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                opcode_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IPV6_TC                         ipv6_tos_ndx,
    SOC_SAND_IN  SOC_PPD_LIF_COS_OPCODE_ACTION_INFO          *action_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_LIF_COS_OPCODE_IPV6_TOS_MAP_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(action_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(lif_cos_opcode_ipv6_tos_map_set,(unit, opcode_ndx, ipv6_tos_ndx, action_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_lif_cos_opcode_ipv6_tos_map_set_print,(unit,opcode_ndx,ipv6_tos_ndx,action_info));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_lif_cos_opcode_ipv6_tos_map_set()", opcode_ndx, ipv6_tos_ndx);
}

/*********************************************************************
*     Set the Opcode according to IPv6 TOS field
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_lif_cos_opcode_ipv6_tos_map_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                opcode_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IPV6_TC                         ipv6_tos_ndx,
    SOC_SAND_OUT SOC_PPD_LIF_COS_OPCODE_ACTION_INFO          *action_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_LIF_COS_OPCODE_IPV6_TOS_MAP_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(action_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(lif_cos_opcode_ipv6_tos_map_get,(unit, opcode_ndx, ipv6_tos_ndx, action_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_lif_cos_opcode_ipv6_tos_map_get_print,(unit,opcode_ndx,ipv6_tos_ndx));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_lif_cos_opcode_ipv6_tos_map_get()", opcode_ndx, ipv6_tos_ndx);
}

/*********************************************************************
*     Set the Opcode according to IPv4 TOS field
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_lif_cos_opcode_ipv4_tos_map_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                opcode_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IPV4_TOS                        ipv4_tos_ndx,
    SOC_SAND_IN  SOC_PPD_LIF_COS_OPCODE_ACTION_INFO          *action_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_LIF_COS_OPCODE_IPV4_TOS_MAP_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(action_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(lif_cos_opcode_ipv4_tos_map_set,(unit, opcode_ndx, ipv4_tos_ndx, action_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_lif_cos_opcode_ipv4_tos_map_set_print,(unit,opcode_ndx,ipv4_tos_ndx,action_info));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_lif_cos_opcode_ipv4_tos_map_set()", opcode_ndx, ipv4_tos_ndx);
}

/*********************************************************************
*     Set the Opcode according to IPv4 TOS field
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_lif_cos_opcode_ipv4_tos_map_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                opcode_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IPV4_TOS                        ipv4_tos_ndx,
    SOC_SAND_OUT SOC_PPD_LIF_COS_OPCODE_ACTION_INFO          *action_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_LIF_COS_OPCODE_IPV4_TOS_MAP_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(action_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(lif_cos_opcode_ipv4_tos_map_get,(unit, opcode_ndx, ipv4_tos_ndx, action_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_lif_cos_opcode_ipv4_tos_map_get_print,(unit,opcode_ndx,ipv4_tos_ndx));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_lif_cos_opcode_ipv4_tos_map_get()", opcode_ndx, ipv4_tos_ndx);
}

/*********************************************************************
*     Set the Opcode according to Traffic Class and Drop
 *     Precedence
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_lif_cos_opcode_tc_dp_map_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                opcode_ndx,
    SOC_SAND_IN  SOC_SAND_PP_TC                              tc_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DP                              dp_ndx,
    SOC_SAND_IN  SOC_PPD_LIF_COS_OPCODE_ACTION_INFO          *action_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_LIF_COS_OPCODE_TC_DP_MAP_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(action_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(lif_cos_opcode_tc_dp_map_set,(unit, opcode_ndx, tc_ndx, dp_ndx, action_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_lif_cos_opcode_tc_dp_map_set_print,(unit,opcode_ndx,tc_ndx,dp_ndx,action_info));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_lif_cos_opcode_tc_dp_map_set()", opcode_ndx, tc_ndx);
}

/*********************************************************************
*     Set the Opcode according to Traffic Class and Drop
 *     Precedence
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_lif_cos_opcode_tc_dp_map_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                opcode_ndx,
    SOC_SAND_IN  SOC_SAND_PP_TC                              tc_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DP                              dp_ndx,
    SOC_SAND_OUT SOC_PPD_LIF_COS_OPCODE_ACTION_INFO          *action_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_LIF_COS_OPCODE_TC_DP_MAP_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(action_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(lif_cos_opcode_tc_dp_map_get,(unit, opcode_ndx, tc_ndx, dp_ndx, action_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_lif_cos_opcode_tc_dp_map_get_print,(unit,opcode_ndx,tc_ndx,dp_ndx));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_lif_cos_opcode_tc_dp_map_get()", opcode_ndx, tc_ndx);
}

/*********************************************************************
*     Set the Opcode according to VLAN Tag
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_lif_cos_opcode_vlan_tag_map_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                opcode_ndx,
    SOC_SAND_IN  uint8                                 tag_type_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                          pcp_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                         dei_ndx,
    SOC_SAND_IN  SOC_PPD_LIF_COS_OPCODE_ACTION_INFO          *action_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_LIF_COS_OPCODE_VLAN_TAG_MAP_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(action_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(lif_cos_opcode_vlan_tag_map_set,(unit, opcode_ndx, tag_type_ndx, pcp_ndx, dei_ndx, action_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_lif_cos_opcode_vlan_tag_map_set_print,(unit,opcode_ndx,tag_type_ndx,pcp_ndx,dei_ndx,action_info));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_lif_cos_opcode_vlan_tag_map_set()", opcode_ndx, 0);
}

/*********************************************************************
*     Set the Opcode according to VLAN Tag
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_lif_cos_opcode_vlan_tag_map_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                opcode_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_TAG_TYPE                   tag_type_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                          pcp_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                         dei_ndx,
    SOC_SAND_OUT SOC_PPD_LIF_COS_OPCODE_ACTION_INFO          *action_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_LIF_COS_OPCODE_VLAN_TAG_MAP_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(action_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(lif_cos_opcode_vlan_tag_map_get,(unit, opcode_ndx, tag_type_ndx, pcp_ndx, dei_ndx, action_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_lif_cos_opcode_vlan_tag_map_get_print,(unit,opcode_ndx,tag_type_ndx,pcp_ndx,dei_ndx));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_lif_cos_opcode_vlan_tag_map_get()", opcode_ndx, 0);
}

/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     soc_ppd_api_lif_cos module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_PROCEDURE_DESC_ELEMENT*
  soc_ppd_lif_cos_get_procs_ptr(
  )
{
  return Ppd_procedure_desc_element_lif_cos;
}
void
  SOC_PPD_LIF_COS_AC_PROFILE_INFO_clear(
    SOC_SAND_OUT SOC_PPD_LIF_COS_AC_PROFILE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LIF_COS_AC_PROFILE_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_LIF_COS_PWE_PROFILE_INFO_clear(
    SOC_SAND_OUT SOC_PPD_LIF_COS_PWE_PROFILE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LIF_COS_PWE_PROFILE_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_LIF_COS_PROFILE_INFO_clear(
    SOC_SAND_OUT SOC_PPD_LIF_COS_PROFILE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LIF_COS_PROFILE_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_LIF_COS_PROFILE_MAP_TBL_ENTRY_clear(
    SOC_SAND_OUT SOC_PPD_LIF_COS_PROFILE_MAP_TBL_ENTRY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_LIF_COS_PROFILE_MAP_TBL_IP_KEY_clear(
    SOC_SAND_OUT SOC_PPD_LIF_COS_PROFILE_MAP_TBL_IP_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_IP_KEY_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_LIF_COS_PROFILE_MAP_TBL_MPLS_KEY_clear(
    SOC_SAND_OUT SOC_PPD_LIF_COS_PROFILE_MAP_TBL_MPLS_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_MPLS_KEY_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_LIF_COS_PROFILE_MAP_TBL_L2_KEY_clear(
    SOC_SAND_OUT SOC_PPD_LIF_COS_PROFILE_MAP_TBL_L2_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_L2_KEY_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_LIF_COS_PROFILE_MAP_TBL_TC_DP_KEY_clear(
    SOC_SAND_OUT SOC_PPD_LIF_COS_PROFILE_MAP_TBL_TC_DP_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_TC_DP_KEY_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_LIF_COS_OPCODE_ACTION_INFO_clear(
    SOC_SAND_OUT SOC_PPD_LIF_COS_OPCODE_ACTION_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LIF_COS_OPCODE_ACTION_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PPD_DEBUG_IS_LVL1

const char*
  SOC_PPD_LIF_COS_AC_PROFILE_TYPE_to_string(
    SOC_SAND_IN  SOC_PPD_LIF_COS_AC_PROFILE_TYPE enum_val
  )
{
  return SOC_PPC_LIF_COS_AC_PROFILE_TYPE_to_string(enum_val);
}

const char*
  SOC_PPD_LIF_COS_PWE_PROFILE_TYPE_to_string(
    SOC_SAND_IN  SOC_PPD_LIF_COS_PWE_PROFILE_TYPE enum_val
  )
{
  return SOC_PPC_LIF_COS_PWE_PROFILE_TYPE_to_string(enum_val);
}

const char*
  SOC_PPD_LIF_COS_OPCODE_TYPE_to_string(
    SOC_SAND_IN  SOC_PPD_LIF_COS_OPCODE_TYPE enum_val
  )
{
  return SOC_PPC_LIF_COS_OPCODE_TYPE_to_string(enum_val);
}

void
  SOC_PPD_LIF_COS_AC_PROFILE_INFO_print(
    SOC_SAND_IN  SOC_PPD_LIF_COS_AC_PROFILE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LIF_COS_AC_PROFILE_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_LIF_COS_PWE_PROFILE_INFO_print(
    SOC_SAND_IN  SOC_PPD_LIF_COS_PWE_PROFILE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LIF_COS_PWE_PROFILE_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_LIF_COS_PROFILE_INFO_print(
    SOC_SAND_IN  SOC_PPD_LIF_COS_PROFILE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LIF_COS_PROFILE_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_LIF_COS_PROFILE_MAP_TBL_ENTRY_print(
    SOC_SAND_IN  SOC_PPD_LIF_COS_PROFILE_MAP_TBL_ENTRY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_LIF_COS_PROFILE_MAP_TBL_IP_KEY_print(
    SOC_SAND_IN  SOC_PPD_LIF_COS_PROFILE_MAP_TBL_IP_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_IP_KEY_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_LIF_COS_PROFILE_MAP_TBL_MPLS_KEY_print(
    SOC_SAND_IN  SOC_PPD_LIF_COS_PROFILE_MAP_TBL_MPLS_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_MPLS_KEY_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_LIF_COS_PROFILE_MAP_TBL_L2_KEY_print(
    SOC_SAND_IN  SOC_PPD_LIF_COS_PROFILE_MAP_TBL_L2_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_L2_KEY_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_LIF_COS_PROFILE_MAP_TBL_TC_DP_KEY_print(
    SOC_SAND_IN  SOC_PPD_LIF_COS_PROFILE_MAP_TBL_TC_DP_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LIF_COS_PROFILE_MAP_TBL_TC_DP_KEY_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_LIF_COS_OPCODE_ACTION_INFO_print(
    SOC_SAND_IN  SOC_PPD_LIF_COS_OPCODE_ACTION_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LIF_COS_OPCODE_ACTION_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif /* SOC_PPD_DEBUG_IS_LVL1 */

#if SOC_PPD_DEBUG_IS_LVL3

void
  soc_ppd_lif_cos_ac_profile_info_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                profile_ndx,
    SOC_SAND_IN  SOC_PPD_LIF_COS_AC_PROFILE_INFO             *profile_info
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "profile_ndx: %lu\n\r"),profile_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "profile_info:")));
  SOC_PPD_LIF_COS_AC_PROFILE_INFO_print((profile_info));

  return;
}
void
  soc_ppd_lif_cos_ac_profile_info_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                profile_ndx
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "profile_ndx: %lu\n\r"),profile_ndx));

  return;
}
void
  soc_ppd_lif_cos_pwe_profile_info_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                profile_ndx,
    SOC_SAND_IN  SOC_PPD_LIF_COS_PWE_PROFILE_INFO            *profile_info
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "profile_ndx: %lu\n\r"),profile_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "profile_info:")));
  SOC_PPD_LIF_COS_PWE_PROFILE_INFO_print((profile_info));

  return;
}
void
  soc_ppd_lif_cos_pwe_profile_info_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                profile_ndx
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "profile_ndx: %lu\n\r"),profile_ndx));

  return;
}
void
  soc_ppd_lif_cos_profile_info_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                cos_profile_ndx,
    SOC_SAND_IN  SOC_PPD_LIF_COS_PROFILE_INFO                *profile_info
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "cos_profile_ndx: %lu\n\r"),cos_profile_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "profile_info:")));
  SOC_PPD_LIF_COS_PROFILE_INFO_print((profile_info));

  return;
}
void
  soc_ppd_lif_cos_profile_info_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                cos_profile_ndx
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "cos_profile_ndx: %lu\n\r"),cos_profile_ndx));

  return;
}
void
  soc_ppd_lif_cos_profile_map_l2_info_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                map_tbl_ndx,
    SOC_SAND_IN  SOC_PPD_LIF_COS_PROFILE_MAP_TBL_L2_KEY      *map_key,
    SOC_SAND_IN  SOC_PPD_LIF_COS_PROFILE_MAP_TBL_ENTRY       *map_value
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "map_tbl_ndx: %lu\n\r"),map_tbl_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "map_key:")));
  SOC_PPD_LIF_COS_PROFILE_MAP_TBL_L2_KEY_print((map_key));

  LOG_CLI((BSL_META_U(unit,
                      "map_value:")));
  SOC_PPD_LIF_COS_PROFILE_MAP_TBL_ENTRY_print((map_value));

  return;
}
void
  soc_ppd_lif_cos_profile_map_l2_info_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                map_tbl_ndx,
    SOC_SAND_IN  SOC_PPD_LIF_COS_PROFILE_MAP_TBL_L2_KEY      *map_key
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "map_tbl_ndx: %lu\n\r"),map_tbl_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "map_key:")));
  SOC_PPD_LIF_COS_PROFILE_MAP_TBL_L2_KEY_print((map_key));

  return;
}
void
  soc_ppd_lif_cos_profile_map_ip_info_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                map_tbl_ndx,
    SOC_SAND_IN  SOC_PPD_LIF_COS_PROFILE_MAP_TBL_IP_KEY      *map_key,
    SOC_SAND_IN  SOC_PPD_LIF_COS_PROFILE_MAP_TBL_ENTRY       *map_value
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "map_tbl_ndx: %lu\n\r"),map_tbl_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "map_key:")));
  SOC_PPD_LIF_COS_PROFILE_MAP_TBL_IP_KEY_print((map_key));

  LOG_CLI((BSL_META_U(unit,
                      "map_value:")));
  SOC_PPD_LIF_COS_PROFILE_MAP_TBL_ENTRY_print((map_value));

  return;
}
void
  soc_ppd_lif_cos_profile_map_ip_info_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                map_tbl_ndx,
    SOC_SAND_IN  SOC_PPD_LIF_COS_PROFILE_MAP_TBL_IP_KEY      *map_key
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "map_tbl_ndx: %lu\n\r"),map_tbl_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "map_key:")));
  SOC_PPD_LIF_COS_PROFILE_MAP_TBL_IP_KEY_print((map_key));

  return;
}
void
  soc_ppd_lif_cos_profile_map_mpls_label_info_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                map_tbl_ndx,
    SOC_SAND_IN  SOC_PPD_LIF_COS_PROFILE_MAP_TBL_MPLS_KEY    *map_key,
    SOC_SAND_IN  SOC_PPD_LIF_COS_PROFILE_MAP_TBL_ENTRY       *map_value
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "map_tbl_ndx: %lu\n\r"),map_tbl_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "map_key:")));
  SOC_PPD_LIF_COS_PROFILE_MAP_TBL_MPLS_KEY_print((map_key));

  LOG_CLI((BSL_META_U(unit,
                      "map_value:")));
  SOC_PPD_LIF_COS_PROFILE_MAP_TBL_ENTRY_print((map_value));

  return;
}
void
  soc_ppd_lif_cos_profile_map_mpls_label_info_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                map_tbl_ndx,
    SOC_SAND_IN  SOC_PPD_LIF_COS_PROFILE_MAP_TBL_MPLS_KEY    *map_key
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "map_tbl_ndx: %lu\n\r"),map_tbl_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "map_key:")));
  SOC_PPD_LIF_COS_PROFILE_MAP_TBL_MPLS_KEY_print((map_key));

  return;
}
void
  soc_ppd_lif_cos_profile_map_tc_dp_info_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                map_tbl_ndx,
    SOC_SAND_IN  SOC_PPD_LIF_COS_PROFILE_MAP_TBL_TC_DP_KEY   *map_key,
    SOC_SAND_IN  SOC_PPD_LIF_COS_PROFILE_MAP_TBL_ENTRY       *map_value
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "map_tbl_ndx: %lu\n\r"),map_tbl_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "map_key:")));
  SOC_PPD_LIF_COS_PROFILE_MAP_TBL_TC_DP_KEY_print((map_key));

  LOG_CLI((BSL_META_U(unit,
                      "map_value:")));
  SOC_PPD_LIF_COS_PROFILE_MAP_TBL_ENTRY_print((map_value));

  return;
}
void
  soc_ppd_lif_cos_profile_map_tc_dp_info_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                map_tbl_ndx,
    SOC_SAND_IN  SOC_PPD_LIF_COS_PROFILE_MAP_TBL_TC_DP_KEY   *map_key
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "map_tbl_ndx: %lu\n\r"),map_tbl_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "map_key:")));
  SOC_PPD_LIF_COS_PROFILE_MAP_TBL_TC_DP_KEY_print((map_key));

  return;
}
void
  soc_ppd_lif_cos_opcode_types_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                opcode_ndx,
    SOC_SAND_IN  SOC_PPD_LIF_COS_OPCODE_TYPE                 opcode_type
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "opcode_ndx: %lu\n\r"),opcode_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "opcode_type %s "), SOC_PPD_LIF_COS_OPCODE_TYPE_to_string(opcode_type)));

  return;
}
void
  soc_ppd_lif_cos_opcode_types_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                opcode_ndx
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "opcode_ndx: %lu\n\r"),opcode_ndx));

  return;
}
void
  soc_ppd_lif_cos_opcode_ipv6_tos_map_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                opcode_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IPV6_TC                         ipv6_tos_ndx,
    SOC_SAND_IN  SOC_PPD_LIF_COS_OPCODE_ACTION_INFO          *action_info
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "opcode_ndx: %lu\n\r"),opcode_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "ipv6_tos_ndx: %lu\n\r"),ipv6_tos_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "action_info:")));
  SOC_PPD_LIF_COS_OPCODE_ACTION_INFO_print((action_info));

  return;
}
void
  soc_ppd_lif_cos_opcode_ipv6_tos_map_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                opcode_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IPV6_TC                         ipv6_tos_ndx
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "opcode_ndx: %lu\n\r"),opcode_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "ipv6_tos_ndx: %lu\n\r"),ipv6_tos_ndx));

  return;
}
void
  soc_ppd_lif_cos_opcode_ipv4_tos_map_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                opcode_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IPV4_TOS                        ipv4_tos_ndx,
    SOC_SAND_IN  SOC_PPD_LIF_COS_OPCODE_ACTION_INFO          *action_info
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "opcode_ndx: %lu\n\r"),opcode_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "ipv4_tos_ndx: %lu\n\r"),ipv4_tos_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "action_info:")));
  SOC_PPD_LIF_COS_OPCODE_ACTION_INFO_print((action_info));

  return;
}
void
  soc_ppd_lif_cos_opcode_ipv4_tos_map_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                opcode_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IPV4_TOS                        ipv4_tos_ndx
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "opcode_ndx: %lu\n\r"),opcode_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "ipv4_tos_ndx: %lu\n\r"),ipv4_tos_ndx));

  return;
}
void
  soc_ppd_lif_cos_opcode_tc_dp_map_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                opcode_ndx,
    SOC_SAND_IN  SOC_SAND_PP_TC                              tc_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DP                              dp_ndx,
    SOC_SAND_IN  SOC_PPD_LIF_COS_OPCODE_ACTION_INFO          *action_info
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "opcode_ndx: %lu\n\r"),opcode_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "tc_ndx: %lu\n\r"),tc_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "dp_ndx: %lu\n\r"),dp_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "action_info:")));
  SOC_PPD_LIF_COS_OPCODE_ACTION_INFO_print((action_info));

  return;
}
void
  soc_ppd_lif_cos_opcode_tc_dp_map_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                opcode_ndx,
    SOC_SAND_IN  SOC_SAND_PP_TC                              tc_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DP                              dp_ndx
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "opcode_ndx: %lu\n\r"),opcode_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "tc_ndx: %lu\n\r"),tc_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "dp_ndx: %lu\n\r"),dp_ndx));

  return;
}
void
  soc_ppd_lif_cos_opcode_vlan_tag_map_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                opcode_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_TAG_TYPE                   tag_type_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                          pcp_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                         dei_ndx,
    SOC_SAND_IN  SOC_PPD_LIF_COS_OPCODE_ACTION_INFO          *action_info
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "opcode_ndx: %lu\n\r"),opcode_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "tag_type_ndx: %lu\n\r"),tag_type_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "pcp_ndx: %lu\n\r"),pcp_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "dei_ndx: %lu\n\r"),dei_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "action_info:")));
  SOC_PPD_LIF_COS_OPCODE_ACTION_INFO_print((action_info));

  return;
}
void
  soc_ppd_lif_cos_opcode_vlan_tag_map_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                opcode_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_TAG_TYPE                   tag_type_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                          pcp_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                         dei_ndx
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "opcode_ndx: %lu\n\r"),opcode_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "tag_type_ndx %s "), soc_sand_SAND_PP_VLAN_TAG_TYPE_to_string(tag_type_ndx)));

  LOG_CLI((BSL_META_U(unit,
                      "pcp_ndx: %lu\n\r"),pcp_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "dei_ndx: %lu\n\r"),dei_ndx));

  return;
}
#endif /* SOC_PPD_DEBUG_IS_LVL3 */
/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

