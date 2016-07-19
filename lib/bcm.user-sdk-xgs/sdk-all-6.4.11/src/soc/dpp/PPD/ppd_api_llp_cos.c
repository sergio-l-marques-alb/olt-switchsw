/* $Id: ppd_api_llp_cos.c,v 1.11 Broadcom SDK $
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
* FILENAME:       DuneDriver/ppd/src/soc_ppd_api_llp_cos.c
*
* MODULE PREFIX:  soc_ppd_llp
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
#include <soc/dpp/PPD/ppd_api_llp_cos.h>
#ifdef LINK_PB_LIBRARIES
  #include <soc/dpp/Petra/PB_PP/pb_pp_api_llp_cos.h>
#endif
#ifdef LINK_T20E_LIBRARIES
  #include <soc/dpp/T20E/t20e_api_llp_cos.h>
#endif
#ifdef LINK_ARAD_LIBRARIES
  #include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_llp_cos.h>
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
    Ppd_procedure_desc_element_llp_cos[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LLP_COS_GLBL_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LLP_COS_GLBL_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LLP_COS_GLBL_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LLP_COS_GLBL_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LLP_COS_PORT_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LLP_COS_PORT_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LLP_COS_PORT_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LLP_COS_PORT_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LLP_COS_MAPPING_TABLE_ENTRY_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LLP_COS_MAPPING_TABLE_ENTRY_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LLP_COS_MAPPING_TABLE_ENTRY_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LLP_COS_MAPPING_TABLE_ENTRY_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LLP_COS_L4_PORT_RANGE_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LLP_COS_L4_PORT_RANGE_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LLP_COS_L4_PORT_RANGE_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LLP_COS_L4_PORT_RANGE_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LLP_COS_L4_PORT_RANGE_TO_TC_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LLP_COS_L4_PORT_RANGE_TO_TC_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LLP_COS_L4_PORT_RANGE_TO_TC_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LLP_COS_L4_PORT_RANGE_TO_TC_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LLP_COS_IPV4_SUBNET_BASED_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LLP_COS_IPV4_SUBNET_BASED_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LLP_COS_IPV4_SUBNET_BASED_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LLP_COS_IPV4_SUBNET_BASED_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LLP_COS_PROTOCOL_BASED_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LLP_COS_PROTOCOL_BASED_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LLP_COS_PROTOCOL_BASED_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LLP_COS_PROTOCOL_BASED_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LLP_COS_GET_PROCS_PTR),
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
*     Sets global information for COS resolution, including
 *     default drop precedence.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_llp_cos_glbl_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_LLP_COS_GLBL_INFO                   *glbl_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_LLP_COS_GLBL_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(glbl_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(llp_cos_glbl_info_set,(unit, glbl_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_llp_cos_glbl_info_set_print,(unit,glbl_info));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_llp_cos_glbl_info_set()", 0, 0);
}

/*********************************************************************
*     Sets global information for COS resolution, including
 *     default drop precedence.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_llp_cos_glbl_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT SOC_PPD_LLP_COS_GLBL_INFO                   *glbl_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_LLP_COS_GLBL_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(glbl_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(llp_cos_glbl_info_get,(unit, glbl_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_llp_cos_glbl_info_get_print,(unit));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_llp_cos_glbl_info_get()", 0, 0);
}

/*********************************************************************
*     Sets port information for COS resolution, including
 *     which mechanisms to perform, table instance to use for
 *     mapping,...
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_llp_cos_port_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  int                                         core_id,
    SOC_SAND_IN  SOC_PPD_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_PPD_LLP_COS_PORT_INFO                   *port_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_LLP_COS_PORT_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(port_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(llp_cos_port_info_set,(unit, core_id, local_port_ndx, port_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_llp_cos_port_info_set_print,(unit,local_port_ndx,port_info));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_llp_cos_port_info_set()", local_port_ndx, 0);
}

/*********************************************************************
*     Sets port information for COS resolution, including
 *     which mechanisms to perform, table instance to use for
 *     mapping,...
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_llp_cos_port_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  int                                         core_id,
    SOC_SAND_IN  SOC_PPD_PORT                                local_port_ndx,
    SOC_SAND_OUT SOC_PPD_LLP_COS_PORT_INFO                   *port_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_LLP_COS_PORT_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(port_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(llp_cos_port_info_get,(unit, core_id, local_port_ndx, port_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_llp_cos_port_info_get_print,(unit,local_port_ndx));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_llp_cos_port_info_get()", local_port_ndx, 0);
}

/*********************************************************************
*     Set mapping from packet fields to TC/DP, includes the
 *     following mappings:- UP to DE and TC.- IPv4 TOS to DP
 *     and TC.- IPv6 TC to DP and TC.- DE to DP- UP to DP.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_llp_cos_mapping_table_entry_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_LLP_COS_MAPPING_TABLE               mapping_tbl_ndx,
    SOC_SAND_IN  uint32                                table_id_ndx,
    SOC_SAND_IN  uint32                                entry_ndx,
    SOC_SAND_IN  SOC_PPD_LLP_COS_MAPPING_TABLE_ENTRY_INFO    *entry
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_LLP_COS_MAPPING_TABLE_ENTRY_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(entry);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(llp_cos_mapping_table_entry_set,(unit, mapping_tbl_ndx, table_id_ndx, entry_ndx, entry));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_llp_cos_mapping_table_entry_set_print,(unit,mapping_tbl_ndx,table_id_ndx,entry_ndx,entry));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_llp_cos_mapping_table_entry_set()", 0, table_id_ndx);
}

/*********************************************************************
*     Set mapping from packet fields to TC/DP, includes the
 *     following mappings:- UP to DE and TC.- IPv4 TOS to DP
 *     and TC.- IPv6 TC to DP and TC.- DE to DP- UP to DP.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_llp_cos_mapping_table_entry_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_LLP_COS_MAPPING_TABLE               mapping_tbl_ndx,
    SOC_SAND_IN  uint32                                table_id_ndx,
    SOC_SAND_IN  uint32                                entry_ndx,
    SOC_SAND_OUT SOC_PPD_LLP_COS_MAPPING_TABLE_ENTRY_INFO    *entry
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_LLP_COS_MAPPING_TABLE_ENTRY_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(entry);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(llp_cos_mapping_table_entry_get,(unit, mapping_tbl_ndx, table_id_ndx, entry_ndx, entry));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_llp_cos_mapping_table_entry_get_print,(unit,mapping_tbl_ndx,table_id_ndx,entry_ndx));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_llp_cos_mapping_table_entry_get()", 0, table_id_ndx);
}

/*********************************************************************
*     Sets range of L4 ports. Packets L4 ports will be
 *     compared to this range in order to set TC value
 *     correspondingly.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_llp_cos_l4_port_range_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                range_ndx,
    SOC_SAND_IN  SOC_PPD_LLP_COS_L4_PORT_RANGE_INFO          *range_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_LLP_COS_L4_PORT_RANGE_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(range_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(llp_cos_l4_port_range_info_set,(unit, range_ndx, range_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_llp_cos_l4_port_range_info_set_print,(unit,range_ndx,range_info));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_llp_cos_l4_port_range_info_set()", range_ndx, 0);
}

/*********************************************************************
*     Sets range of L4 ports. Packets L4 ports will be
 *     compared to this range in order to set TC value
 *     correspondingly.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_llp_cos_l4_port_range_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                range_ndx,
    SOC_SAND_OUT SOC_PPD_LLP_COS_L4_PORT_RANGE_INFO          *range_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_LLP_COS_L4_PORT_RANGE_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(range_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(llp_cos_l4_port_range_info_get,(unit, range_ndx, range_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_llp_cos_l4_port_range_info_get_print,(unit,range_ndx));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_llp_cos_l4_port_range_info_get()", range_ndx, 0);
}

/*********************************************************************
*     Set mapping to Traffic class according to L4 information
 *     includingSource/Destination and protocol type (TCP or
 *     UDP). Packet L4 destination and source ports are
 *     compared to the ranges set by
 *     soc_ppd_llp_cos_l4_port_range_info_set(). Then according to
 *     comparison result: - 0-3 for source port (where 3 means
 *     was not match) - 0-3 for destination port (where 3 means
 *     was not match)and according to protocol type (UDP/TCP),
 *     this API is used to determine the TC value to set to the
 *     packet
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_llp_cos_l4_port_range_to_tc_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_SAND_PP_L4_PRTCL_TYPE                   l4_prtcl_type_ndx,
    SOC_SAND_IN  uint32                                src_port_match_range_ndx,
    SOC_SAND_IN  uint32                                dest_port_match_range_ndx,
    SOC_SAND_IN  SOC_PPD_LLP_COS_TC_INFO                     *tc_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_LLP_COS_L4_PORT_RANGE_TO_TC_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(tc_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(llp_cos_l4_port_range_to_tc_info_set,(unit, l4_prtcl_type_ndx, src_port_match_range_ndx, dest_port_match_range_ndx, tc_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_llp_cos_l4_port_range_to_tc_info_set_print,(unit,l4_prtcl_type_ndx,src_port_match_range_ndx,dest_port_match_range_ndx,tc_info));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_llp_cos_l4_port_range_to_tc_info_set()", 0, src_port_match_range_ndx);
}

/*********************************************************************
*     Set mapping to Traffic class according to L4 information
 *     includingSource/Destination and protocol type (TCP or
 *     UDP). Packet L4 destination and source ports are
 *     compared to the ranges set by
 *     soc_ppd_llp_cos_l4_port_range_info_set(). Then according to
 *     comparison result: - 0-3 for source port (where 3 means
 *     was not match) - 0-3 for destination port (where 3 means
 *     was not match)and according to protocol type (UDP/TCP),
 *     this API is used to determine the TC value to set to the
 *     packet
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_llp_cos_l4_port_range_to_tc_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_SAND_PP_L4_PRTCL_TYPE                   l4_prtcl_type_ndx,
    SOC_SAND_IN  uint32                                src_port_match_range_ndx,
    SOC_SAND_IN  uint32                                dest_port_match_range_ndx,
    SOC_SAND_OUT SOC_PPD_LLP_COS_TC_INFO                     *tc_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_LLP_COS_L4_PORT_RANGE_TO_TC_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(tc_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(llp_cos_l4_port_range_to_tc_info_get,(unit, l4_prtcl_type_ndx, src_port_match_range_ndx, dest_port_match_range_ndx, tc_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_llp_cos_l4_port_range_to_tc_info_get_print,(unit,l4_prtcl_type_ndx,src_port_match_range_ndx,dest_port_match_range_ndx));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_llp_cos_l4_port_range_to_tc_info_get()", 0, src_port_match_range_ndx);
}

/*********************************************************************
*     Set COS parameters based on source IPv4 subnet.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_llp_cos_ipv4_subnet_based_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                entry_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IPV4_SUBNET                     *subnet,
    SOC_SAND_IN  SOC_PPD_LLP_COS_IPV4_SUBNET_INFO            *subnet_based_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_LLP_COS_IPV4_SUBNET_BASED_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(subnet);
  SOC_SAND_CHECK_NULL_INPUT(subnet_based_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(llp_cos_ipv4_subnet_based_set,(unit, entry_ndx, subnet, subnet_based_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_llp_cos_ipv4_subnet_based_set_print,(unit,entry_ndx,subnet,subnet_based_info));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_llp_cos_ipv4_subnet_based_set()", entry_ndx, 0);
}

/*********************************************************************
*     Set COS parameters based on source IPv4 subnet.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_llp_cos_ipv4_subnet_based_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                entry_ndx,
    SOC_SAND_OUT SOC_SAND_PP_IPV4_SUBNET                     *subnet,
    SOC_SAND_OUT SOC_PPD_LLP_COS_IPV4_SUBNET_INFO            *subnet_based_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_LLP_COS_IPV4_SUBNET_BASED_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(subnet);
  SOC_SAND_CHECK_NULL_INPUT(subnet_based_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(llp_cos_ipv4_subnet_based_get,(unit, entry_ndx, subnet, subnet_based_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_llp_cos_ipv4_subnet_based_get_print,(unit,entry_ndx));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_llp_cos_ipv4_subnet_based_get()", entry_ndx, 0);
}

/*********************************************************************
*     Sets mapping from Ethernet Type and profile to COS
 *     parameters.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_llp_cos_protocol_based_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_ETHER_TYPE                      ether_type_ndx,
    SOC_SAND_IN  SOC_PPD_LLP_COS_PRTCL_INFO                  *prtcl_assign_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                    *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_LLP_COS_PROTOCOL_BASED_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(prtcl_assign_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(llp_cos_protocol_based_set,(unit, profile_ndx, ether_type_ndx, prtcl_assign_info, success));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_llp_cos_protocol_based_set_print,(unit,profile_ndx,ether_type_ndx,prtcl_assign_info));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_llp_cos_protocol_based_set()", profile_ndx, ether_type_ndx);
}

/*********************************************************************
*     Sets mapping from Ethernet Type and profile to COS
 *     parameters.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_llp_cos_protocol_based_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_ETHER_TYPE                      ether_type_ndx,
    SOC_SAND_OUT SOC_PPD_LLP_COS_PRTCL_INFO                  *prtcl_assign_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_LLP_COS_PROTOCOL_BASED_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(prtcl_assign_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(llp_cos_protocol_based_get,(unit, profile_ndx, ether_type_ndx, prtcl_assign_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_llp_cos_protocol_based_get_print,(unit,profile_ndx,ether_type_ndx));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_llp_cos_protocol_based_get()", profile_ndx, ether_type_ndx);
}

/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     soc_ppd_api_llp_cos module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_PROCEDURE_DESC_ELEMENT*
  soc_ppd_llp_cos_get_procs_ptr(
  )
{
  return Ppd_procedure_desc_element_llp_cos;
}
void
  SOC_PPD_LLP_COS_GLBL_INFO_clear(
    SOC_SAND_OUT SOC_PPD_LLP_COS_GLBL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LLP_COS_GLBL_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_LLP_COS_TC_INFO_clear(
    SOC_SAND_OUT SOC_PPD_LLP_COS_TC_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LLP_COS_TC_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_LLP_COS_UP_USE_clear(
    SOC_SAND_OUT SOC_PPD_LLP_COS_UP_USE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LLP_COS_UP_USE_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_LLP_COS_PORT_L2_TABLE_INDEXES_clear(
    SOC_SAND_OUT SOC_PPD_LLP_COS_PORT_L2_TABLE_INDEXES *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LLP_COS_PORT_L2_TABLE_INDEXES_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_LLP_COS_PORT_L2_INFO_clear(
    SOC_SAND_OUT SOC_PPD_LLP_COS_PORT_L2_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LLP_COS_PORT_L2_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_LLP_COS_PORT_L3_INFO_clear(
    SOC_SAND_OUT SOC_PPD_LLP_COS_PORT_L3_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LLP_COS_PORT_L3_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_LLP_COS_PORT_L4_INFO_clear(
    SOC_SAND_OUT SOC_PPD_LLP_COS_PORT_L4_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LLP_COS_PORT_L4_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_LLP_COS_PORT_INFO_clear(
    SOC_SAND_OUT SOC_PPD_LLP_COS_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LLP_COS_PORT_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_LLP_COS_MAPPING_TABLE_ENTRY_INFO_clear(
    SOC_SAND_OUT SOC_PPD_LLP_COS_MAPPING_TABLE_ENTRY_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LLP_COS_MAPPING_TABLE_ENTRY_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_LLP_COS_IPV4_SUBNET_INFO_clear(
    SOC_SAND_OUT SOC_PPD_LLP_COS_IPV4_SUBNET_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LLP_COS_IPV4_SUBNET_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_LLP_COS_PRTCL_INFO_clear(
    SOC_SAND_OUT SOC_PPD_LLP_COS_PRTCL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LLP_COS_PRTCL_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_LLP_COS_L4_PORT_RANGE_INFO_clear(
    SOC_SAND_OUT SOC_PPD_LLP_COS_L4_PORT_RANGE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LLP_COS_L4_PORT_RANGE_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PPD_DEBUG_IS_LVL1

const char*
  SOC_PPD_LLP_COS_MAPPING_TABLE_to_string(
    SOC_SAND_IN  SOC_PPD_LLP_COS_MAPPING_TABLE enum_val
  )
{
  return SOC_PPC_LLP_COS_MAPPING_TABLE_to_string(enum_val);
}

void
  SOC_PPD_LLP_COS_GLBL_INFO_print(
    SOC_SAND_IN  SOC_PPD_LLP_COS_GLBL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LLP_COS_GLBL_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_LLP_COS_TC_INFO_print(
    SOC_SAND_IN  SOC_PPD_LLP_COS_TC_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LLP_COS_TC_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_LLP_COS_UP_USE_print(
    SOC_SAND_IN  SOC_PPD_LLP_COS_UP_USE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LLP_COS_UP_USE_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_LLP_COS_PORT_L2_TABLE_INDEXES_print(
    SOC_SAND_IN  SOC_PPD_LLP_COS_PORT_L2_TABLE_INDEXES *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LLP_COS_PORT_L2_TABLE_INDEXES_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_LLP_COS_PORT_L2_INFO_print(
    SOC_SAND_IN  SOC_PPD_LLP_COS_PORT_L2_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LLP_COS_PORT_L2_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_LLP_COS_PORT_L3_INFO_print(
    SOC_SAND_IN  SOC_PPD_LLP_COS_PORT_L3_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LLP_COS_PORT_L3_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_LLP_COS_PORT_L4_INFO_print(
    SOC_SAND_IN  SOC_PPD_LLP_COS_PORT_L4_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LLP_COS_PORT_L4_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_LLP_COS_PORT_INFO_print(
    SOC_SAND_IN  SOC_PPD_LLP_COS_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LLP_COS_PORT_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_LLP_COS_MAPPING_TABLE_ENTRY_INFO_print(
    SOC_SAND_IN  SOC_PPD_LLP_COS_MAPPING_TABLE_ENTRY_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LLP_COS_MAPPING_TABLE_ENTRY_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_LLP_COS_IPV4_SUBNET_INFO_print(
    SOC_SAND_IN  SOC_PPD_LLP_COS_IPV4_SUBNET_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LLP_COS_IPV4_SUBNET_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_LLP_COS_PRTCL_INFO_print(
    SOC_SAND_IN  SOC_PPD_LLP_COS_PRTCL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LLP_COS_PRTCL_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_LLP_COS_L4_PORT_RANGE_INFO_print(
    SOC_SAND_IN  SOC_PPD_LLP_COS_L4_PORT_RANGE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LLP_COS_L4_PORT_RANGE_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif /* SOC_PPD_DEBUG_IS_LVL1 */

#if SOC_PPD_DEBUG_IS_LVL3

void
  soc_ppd_llp_cos_glbl_info_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_LLP_COS_GLBL_INFO                   *glbl_info
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "glbl_info:")));
  SOC_PPD_LLP_COS_GLBL_INFO_print((glbl_info));

  return;
}
void
  soc_ppd_llp_cos_glbl_info_get_print(
    SOC_SAND_IN  int                               unit
  )
{

  return;
}
void
  soc_ppd_llp_cos_port_info_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_PPD_LLP_COS_PORT_INFO                   *port_info
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "local_port_ndx: %lu\n\r"),local_port_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "port_info:")));
  SOC_PPD_LLP_COS_PORT_INFO_print((port_info));

  return;
}
void
  soc_ppd_llp_cos_port_info_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_PORT                                local_port_ndx
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "local_port_ndx: %lu\n\r"),local_port_ndx));

  return;
}
void
  soc_ppd_llp_cos_mapping_table_entry_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_LLP_COS_MAPPING_TABLE               mapping_tbl_ndx,
    SOC_SAND_IN  uint32                                table_id_ndx,
    SOC_SAND_IN  uint32                                entry_ndx,
    SOC_SAND_IN  SOC_PPD_LLP_COS_MAPPING_TABLE_ENTRY_INFO    *entry
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "mapping_tbl_ndx %s "), SOC_PPD_LLP_COS_MAPPING_TABLE_to_string(mapping_tbl_ndx)));

  LOG_CLI((BSL_META_U(unit,
                      "table_id_ndx: %lu\n\r"),table_id_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "entry_ndx: %lu\n\r"),entry_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "entry:")));
  SOC_PPD_LLP_COS_MAPPING_TABLE_ENTRY_INFO_print((entry));

  return;
}
void
  soc_ppd_llp_cos_mapping_table_entry_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_LLP_COS_MAPPING_TABLE               mapping_tbl_ndx,
    SOC_SAND_IN  uint32                                table_id_ndx,
    SOC_SAND_IN  uint32                                entry_ndx
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "mapping_tbl_ndx %s "), SOC_PPD_LLP_COS_MAPPING_TABLE_to_string(mapping_tbl_ndx)));

  LOG_CLI((BSL_META_U(unit,
                      "table_id_ndx: %lu\n\r"),table_id_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "entry_ndx: %lu\n\r"),entry_ndx));

  return;
}
void
  soc_ppd_llp_cos_l4_port_range_info_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                range_ndx,
    SOC_SAND_IN  SOC_PPD_LLP_COS_L4_PORT_RANGE_INFO          *range_info
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "range_ndx: %lu\n\r"),range_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "range_info:")));
  SOC_PPD_LLP_COS_L4_PORT_RANGE_INFO_print((range_info));

  return;
}
void
  soc_ppd_llp_cos_l4_port_range_info_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                range_ndx
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "range_ndx: %lu\n\r"),range_ndx));

  return;
}
void
  soc_ppd_llp_cos_l4_port_range_to_tc_info_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_SAND_PP_L4_PRTCL_TYPE                   l4_prtcl_type_ndx,
    SOC_SAND_IN  uint32                                src_port_match_range_ndx,
    SOC_SAND_IN  uint32                                dest_port_match_range_ndx,
    SOC_SAND_IN  SOC_PPD_LLP_COS_TC_INFO                     *tc_info
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "l4_prtcl_type_ndx %s "), soc_sand_SAND_PP_L4_PRTCL_TYPE_to_string(l4_prtcl_type_ndx)));

  LOG_CLI((BSL_META_U(unit,
                      "src_port_match_range_ndx: %lu\n\r"),src_port_match_range_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "dest_port_match_range_ndx: %lu\n\r"),dest_port_match_range_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "tc_info:")));
  SOC_PPD_LLP_COS_TC_INFO_print((tc_info));

  return;
}
void
  soc_ppd_llp_cos_l4_port_range_to_tc_info_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_SAND_PP_L4_PRTCL_TYPE                   l4_prtcl_type_ndx,
    SOC_SAND_IN  uint32                                src_port_match_range_ndx,
    SOC_SAND_IN  uint32                                dest_port_match_range_ndx
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "l4_prtcl_type_ndx %s "), soc_sand_SAND_PP_L4_PRTCL_TYPE_to_string(l4_prtcl_type_ndx)));

  LOG_CLI((BSL_META_U(unit,
                      "src_port_match_range_ndx: %lu\n\r"),src_port_match_range_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "dest_port_match_range_ndx: %lu\n\r"),dest_port_match_range_ndx));

  return;
}
void
  soc_ppd_llp_cos_ipv4_subnet_based_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                entry_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IPV4_SUBNET                     *subnet,
    SOC_SAND_IN  SOC_PPD_LLP_COS_IPV4_SUBNET_INFO            *subnet_based_info
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "entry_ndx: %lu\n\r"),entry_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "subnet:")));
  soc_sand_SAND_PP_IPV4_SUBNET_print((subnet));

  LOG_CLI((BSL_META_U(unit,
                      "subnet_based_info:")));
  SOC_PPD_LLP_COS_IPV4_SUBNET_INFO_print((subnet_based_info));

  return;
}
void
  soc_ppd_llp_cos_ipv4_subnet_based_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                entry_ndx
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "entry_ndx: %lu\n\r"),entry_ndx));

  return;
}
void
  soc_ppd_llp_cos_protocol_based_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_ETHER_TYPE                      ether_type_ndx,
    SOC_SAND_IN  SOC_PPD_LLP_COS_PRTCL_INFO                  *prtcl_assign_info
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "profile_ndx: %lu\n\r"),profile_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "ether_type_ndx: %u\n\r"),ether_type_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "prtcl_assign_info:")));
  SOC_PPD_LLP_COS_PRTCL_INFO_print((prtcl_assign_info));

  return;
}
void
  soc_ppd_llp_cos_protocol_based_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_ETHER_TYPE                      ether_type_ndx
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "profile_ndx: %lu\n\r"),profile_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "ether_type_ndx: %u\n\r"),ether_type_ndx));

  return;
}
#endif /* SOC_PPD_DEBUG_IS_LVL3 */
/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

