/* $Id: pb_pp_lif.c,v 1.26 Broadcom SDK $
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
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_PP_/src/soc_pb_pp_lif.c
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
#include <soc/dpp/SAND/Utils/sand_bitstream.h>

#include <soc/dpp/Petra/PB_PP/pb_pp_api_framework.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_lif.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_general.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_isem_access.h>
#include <soc/dpp/Petra/PB_TM/pb_parser.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_reg_access.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_tbl_access.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_llp_parse.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_frwrd_mact_mgmt.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_lif_cos.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_sw_db.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PB_PP_LIF_KEY_MAPPING_MAX                              (SOC_PB_PP_NOF_L2_LIF_AC_MAP_KEY_TYPES-1)
#define SOC_PB_PP_LIF_HANDLE_TYPE_MAX                              (SOC_PB_PP_NOF_L2_LIF_L2CP_HANDLE_TYPES-1)
#define SOC_PB_PP_LIF_DEFAULT_FRWD_TYPE_MAX                        (SOC_PB_PP_NOF_L2_LIF_DFLT_FRWRD_SRCS-1)
#define SOC_PB_PP_LIF_ORIENTATION_MAX                              (SOC_SAND_PP_NOF_HUB_SPOKE_ORIENTATIONS-1)
#define SOC_PB_PP_LIF_TPID_PROFILE_INDEX_MAX                       (3)
#define SOC_PB_PP_LIF_COS_PROFILE_MAX                              (15)
#define SOC_PB_PP_LIF_PWE_COS_PROFILE_MAX                          (0)
#define SOC_PB_PP_LIF_PWE_SERVICE_TYPE_MAX                         (SOC_PB_PP_NOF_L2_LIF_PWE_SERVICE_TYPES-1)
#define SOC_PB_PP_LIF_VLAN_DOMAIN_MAX                              (SOC_DPP_NOF_VLAN_DOMAINS_PETRAB - 1)
#define SOC_PB_PP_LIF_RAW_KEY_MAX                                  (SOC_SAND_U32_MAX)
#define SOC_PB_PP_LIF_LEARN_TYPE_MAX                               (SOC_PB_PP_NOF_L2_LIF_AC_LEARN_TYPES-1)
#define SOC_PB_PP_LIF_ING_VLAN_EDIT_PROFILE_MAX                    (7)
#define SOC_PB_PP_LIF_EDIT_PCP_PROFILE_MAX                         (15)
#define SOC_PB_PP_LIF_L2CP_PROFILE_MAX                             (1)
#define SOC_PB_PP_LIF_DA_MAC_ADDRESS_LSB_MAX                       (63)
#define SOC_PB_PP_LIF_OPCODE_ID_MAX                                (3)
#define SOC_PB_PP_LIF_NOF_LIF_ENTRIES_MAX                          (SOC_SAND_UINT_MAX)
#define SOC_PB_PP_LIF_ISID_DOMAIN_MAX                              (SOC_SAND_U32_MAX)
#define SOC_PB_PP_LIF_AC_SERVICE_TYPE_MAX                          (SOC_PB_PP_NOF_L2_LIF_AC_SERVICE_TYPES-1)
#define SOC_PB_PP_LIF_ISID_SERVICE_TYPE_MAX                        (SOC_PB_PP_NOF_L2_LIF_ISID_SERVICE_TYPES-1)

#define SOC_PB_PP_L2_LIF_L2CP_KEY_DA_MAC_ADDRESS_LSB_LSB           (0)
#define SOC_PB_PP_L2_LIF_L2CP_KEY_DA_MAC_ADDRESS_LSB_MSB           (5)
#define SOC_PB_PP_L2_LIF_L2CP_KEY_DA_MAC_ADDRESS_LSB_SHIFT         (SOC_PB_PP_L2_LIF_L2CP_KEY_DA_MAC_ADDRESS_LSB_LSB)
#define SOC_PB_PP_L2_LIF_L2CP_KEY_DA_MAC_ADDRESS_LSB_MASK          (SOC_SAND_BITS_MASK(SOC_PB_PP_L2_LIF_L2CP_KEY_DA_MAC_ADDRESS_LSB_MSB, SOC_PB_PP_L2_LIF_L2CP_KEY_DA_MAC_ADDRESS_LSB_LSB))

#define SOC_PB_PP_L2_LIF_L2CP_KEY_L2CP_PROFILE_LSB                 (6)
#define SOC_PB_PP_L2_LIF_L2CP_KEY_L2CP_PROFILE_MSB                 (6)
#define SOC_PB_PP_L2_LIF_L2CP_KEY_L2CP_PROFILE_SHIFT               (SOC_PB_PP_L2_LIF_L2CP_KEY_L2CP_PROFILE_LSB)
#define SOC_PB_PP_L2_LIF_L2CP_KEY_L2CP_PROFILE_MASK                (SOC_SAND_BITS_MASK(SOC_PB_PP_L2_LIF_L2CP_KEY_L2CP_PROFILE_MSB, SOC_PB_PP_L2_LIF_L2CP_KEY_L2CP_PROFILE_LSB))


/* } */
/*************
 * MACROS    *
 *************/
/* { */

#define SOC_PB_PP_L2_LIF_L2CP_KEY_ENTRY_OFFSET(l2cp_profile, da_mac_address_lsb)  \
          SOC_SAND_SET_FLD_IN_PLACE(da_mac_address_lsb, SOC_PB_PP_L2_LIF_L2CP_KEY_DA_MAC_ADDRESS_LSB_SHIFT, SOC_PB_PP_L2_LIF_L2CP_KEY_DA_MAC_ADDRESS_LSB_MASK) | \
          SOC_SAND_SET_FLD_IN_PLACE(l2cp_profile, SOC_PB_PP_L2_LIF_L2CP_KEY_L2CP_PROFILE_SHIFT, SOC_PB_PP_L2_LIF_L2CP_KEY_L2CP_PROFILE_MASK)

#define SOC_PB_PP_IHP_LLVP_PROG_SEL_OFFSETS_KEY_DEFAULT_ENTRY_OFFSET(packet_format_qual, small_em_key_profile) \
          SOC_PB_PP_IHP_LLVP_PROG_SEL_OFFSETS_KEY_ENTRY_OFFSET(SOC_PB_PARSER_DEFAULT_KEY_PROGRAM_PROFILE, packet_format_qual, small_em_key_profile)

#define SOC_PB_PP_L2_LIF_VLAN_RANGE_TO_INTERNAL_KEY(first_vid,last_vid) \
          ((SOC_SAND_GET_BITS_RANGE(first_vid,7,0)<<8) + SOC_SAND_GET_BITS_RANGE(last_vid,7,0))
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

static SOC_PROCEDURE_DESC_ELEMENT
  Soc_pb_pp_procedure_desc_element_lif[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_LIF_PWE_MAP_RANGE_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_LIF_PWE_MAP_RANGE_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_LIF_PWE_MAP_RANGE_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_LIF_PWE_MAP_RANGE_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_LIF_PWE_MAP_RANGE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_LIF_PWE_MAP_RANGE_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_LIF_PWE_MAP_RANGE_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_LIF_PWE_MAP_RANGE_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_LIF_PWE_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_LIF_PWE_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_LIF_PWE_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_LIF_PWE_ADD_INTERNAL_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_LIF_PWE_ADD_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_LIF_PWE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_LIF_PWE_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_LIF_PWE_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_LIF_PWE_GET_INTERNAL_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_LIF_PWE_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_LIF_PWE_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_LIF_PWE_REMOVE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_LIF_PWE_REMOVE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_LIF_PWE_REMOVE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_LIF_VLAN_COMPRESSION_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_LIF_VLAN_COMPRESSION_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_LIF_VLAN_COMPRESSION_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_LIF_VLAN_COMPRESSION_ADD_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_LIF_VLAN_COMPRESSION_RANGE_VALID_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_LIF_VLAN_COMPRESSION_RANGE_VALID_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_LIF_VLAN_COMPRESSION_IS_VALID_RANGE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_LIF_VLAN_COMPRESSION_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_LIF_VLAN_COMPRESSION_REMOVE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_LIF_VLAN_COMPRESSION_REMOVE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_LIF_VLAN_COMPRESSION_REMOVE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_LIF_AC_MAP_KEY_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_LIF_AC_MAP_KEY_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_LIF_AC_MAP_KEY_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_LIF_AC_MAP_KEY_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_LIF_AC_MAP_KEY_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_LIF_AC_MAP_KEY_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_LIF_AC_MAP_KEY_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_LIF_AC_MAP_KEY_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_LIF_AC_MP_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_LIF_AC_MP_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_LIF_AC_MP_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_LIF_AC_MP_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_LIF_AC_MP_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_LIF_AC_MP_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_LIF_AC_MP_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_LIF_AC_MP_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_LIF_AC_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_LIF_AC_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_LIF_AC_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_LIF_AC_ADD_INTERNAL_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_LIF_AC_ADD_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_LIF_AC_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_LIF_AC_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_LIF_AC_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_LIF_AC_GET_INTERNAL_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_LIF_AC_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_LIF_AC_WITH_COS_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_LIF_AC_WITH_COS_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_LIF_AC_WITH_COS_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_LIF_AC_WITH_COS_ADD_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_LIF_AC_WITH_COS_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_LIF_AC_WITH_COS_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_LIF_AC_WITH_COS_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_LIF_AC_WITH_COS_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_LIF_AC_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_LIF_AC_REMOVE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_LIF_AC_REMOVE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_LIF_AC_REMOVE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_LIF_L2CP_TRAP_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_LIF_L2CP_TRAP_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_LIF_L2CP_TRAP_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_LIF_L2CP_TRAP_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_LIF_L2CP_TRAP_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_LIF_L2CP_TRAP_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_LIF_L2CP_TRAP_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_LIF_L2CP_TRAP_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_LIF_ISID_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_LIF_ISID_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_LIF_ISID_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_LIF_ISID_ADD_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_LIF_ISID_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_LIF_ISID_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_LIF_ISID_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_LIF_ISID_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_LIF_ISID_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_LIF_ISID_REMOVE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_LIF_ISID_REMOVE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_L2_LIF_ISID_REMOVE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_GET_ERRS_PTR),
  /*
   * } Auto generated. Do not edit previous section.
   */

  /*
   * Last element. Do no touch.
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

static SOC_ERROR_DESC_ELEMENT
  Soc_pb_pp_error_desc_element_lif[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  {
    SOC_PB_PP_LIF_SUCCESS_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LIF_SUCCESS_OUT_OF_RANGE_ERR",
    "The parameter 'success' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_NOF_SUCCESS_FAILURES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LIF_PORT_PROFILE_NDX_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LIF_PORT_PROFILE_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'port_profile_ndx' is out of range. \n\r "
    "The range is: No min - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LIF_VLAN_FORMAT_NDX_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LIF_VLAN_FORMAT_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'vlan_format_ndx' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_PP_NOF_ETHERNET_FRAME_VLAN_FORMATS-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LIF_KEY_MAPPING_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LIF_KEY_MAPPING_OUT_OF_RANGE_ERR",
    "The parameter 'key_mapping' is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_L2_LIF_AC_MAP_KEY_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LIF_HANDLE_TYPE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LIF_HANDLE_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'handle_type' is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_L2_LIF_L2CP_HANDLE_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LIF_DEFAULT_FRWD_TYPE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LIF_DEFAULT_FRWD_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'default_frwd_type' is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_L2_LIF_DFLT_FRWRD_SRCS-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LIF_ORIENTATION_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LIF_ORIENTATION_OUT_OF_RANGE_ERR",
    "The parameter 'orientation' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_PP_NOF_HUB_SPOKE_ORIENTATIONS-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LIF_TPID_PROFILE_INDEX_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LIF_TPID_PROFILE_INDEX_OUT_OF_RANGE_ERR",
    "The parameter 'tpid_profile_index' is out of range. \n\r "
    "The range is: 0 - 3.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LIF_COS_PROFILE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LIF_COS_PROFILE_OUT_OF_RANGE_ERR",
    "The parameter 'cos_profile' is out of range. \n\r "
    "The range is: 0 - 15.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LIF_SERVICE_TYPE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LIF_SERVICE_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'service_type' is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_L2_LIF_PWE_SERVICE_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LIF_VLAN_DOMAIN_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LIF_VLAN_DOMAIN_OUT_OF_RANGE_ERR",
    "The parameter 'vlan_domain' is out of range. \n\r "
    "The range is: 0 - 63.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LIF_VID_TAG_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LIF_VID_TAG_OUT_OF_RANGE_ERR",
    "The parameter 'vid_tag' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_PP_NOF_VLAN_TAG_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LIF_VLAN_RANGE_FIRST_OVER_LAST_ERR,
    "SOC_PB_PP_LIF_VLAN_RANGE_FIRST_OVER_LAST_ERR",
    "The parameter 'first_vid' is over 'last_vid'. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LIF_VLAN_RANGE_VLAN_RANGE_ALREADY_DEFINED_ERR,
    "SOC_PB_PP_LIF_VLAN_RANGE_VLAN_RANGE_ALREADY_DEFINED_ERR",
    "The range defined by: 'first_vid' and 'last_vid'. \n\r "
    "Part of the interval is already defined in the given 'local_port_ndx'.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LIF_KEY_TYPE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LIF_KEY_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'key_type' is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_L2_LIF_AC_MAP_KEY_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LIF_RAW_KEY_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LIF_RAW_KEY_OUT_OF_RANGE_ERR",
    "The parameter 'raw_key' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LIF_LEARN_TYPE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LIF_LEARN_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'learn_type' is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_L2_LIF_AC_LEARN_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LIF_ING_VLAN_EDIT_PROFILE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LIF_ING_VLAN_EDIT_PROFILE_OUT_OF_RANGE_ERR",
    "The parameter 'ing_vlan_edit_profile' is out of range. \n\r "
    "The range is: 0 - 7.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LIF_EDIT_PCP_PROFILE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LIF_EDIT_PCP_PROFILE_OUT_OF_RANGE_ERR",
    "The parameter 'edit_pcp_profile' is out of range. \n\r "
    "The range is: 0 - 15.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LIF_L2CP_PROFILE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LIF_L2CP_PROFILE_OUT_OF_RANGE_ERR",
    "The parameter 'l2cp_profile' is out of range. \n\r "
    "The range is: 0 - 1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LIF_DA_MAC_ADDRESS_LSB_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LIF_DA_MAC_ADDRESS_LSB_OUT_OF_RANGE_ERR",
    "The parameter 'da_mac_address_lsb' is out of range. \n\r "
    "The range is: 0 - 63.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LIF_OPCODE_ID_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LIF_OPCODE_ID_OUT_OF_RANGE_ERR",
    "The parameter 'opcode_id' is out of range. \n\r "
    "The range is: 0 - 3.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LIF_NOF_LIF_ENTRIES_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LIF_NOF_LIF_ENTRIES_OUT_OF_RANGE_ERR",
    "The parameter 'nof_lif_entries' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_UINT_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LIF_ISID_DOMAIN_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LIF_ISID_DOMAIN_OUT_OF_RANGE_ERR",
    "The parameter 'isid_domain' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  /*
   * } Auto generated. Do not edit previous section.
   */
  {
    SOC_PB_PP_LIF_VSI_IS_NOT_P2P_ERR,
    "SOC_PB_PP_LIF_VSI_IS_NOT_P2P_ERR",
    "The VSI should not be used for P2P services. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LIF_ILLEGAL_KEY_MAPPING_ERR,
    "SOC_PB_PP_LIF_ILLEGAL_KEY_MAPPING_ERR",
    "AC key cannot be port x vlan x vlan when the packet format is port x vlan. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LIF_INNER_EXISTS_OUTER_NOT_ERR,
    "SOC_PB_PP_LIF_INNER_EXISTS_OUTER_NOT_ERR",
    "Inner vlan exists while the outer vlan is marked 'ignore'. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LIF_PWE_COS_PROFILE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LIF_COS_PROFILE_OUT_OF_RANGE_ERR",
    "The parameter 'cos_profile' for PWE is out of range. \n\r "
    "The range is: 0 - 0.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LIF_AC_P2P_DEFAULT_FRWRD_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LIF_AC_P2P_DEFAULT_FRWRD_OUT_OF_RANGE_ERR",
    "ac_info->default_frwrd.default_frwd_type has to be LIF for P2P. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LIF_AC_MP_DEFAULT_FRWRD_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LIF_AC_MP_DEFAULT_FRWRD_OUT_OF_RANGE_ERR",
    "ac_info->default_frwrd.default_frwd_type has to be VSI for MP. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LIF_PWE_P2P_DEFAULT_FRWRD_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LIF_PWE_P2P_DEFAULT_FRWRD_OUT_OF_RANGE_ERR",
    "pwe_info->default_frwrd.default_frwd_type has to be LIF for P2P. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LIF_PWE_MP_DEFAULT_FRWRD_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LIF_PWE_MP_DEFAULT_FRWRD_OUT_OF_RANGE_ERR",
    "pwe_info->default_frwrd.default_frwd_type has to be VSI for MP. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LIF_AC_P2P_ORIENTATION_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LIF_AC_P2P_ORIENTATION_OUT_OF_RANGE_ERR",
    "orientation of P2P AC has to be SPOKE. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LIF_PWE_P2P_ORIENTATION_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LIF_PWE_P2P_ORIENTATION_OUT_OF_RANGE_ERR",
    "orientation of P2P PWE has to be SPOKE. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
   {
    SOC_PB_PP_LIF_PWE_INFO_INVALID_FIELDS_ERR,
    "SOC_PB_PP_LIF_PWE_INFO_INVALID_FIELDS_ERR",
    "Fields: \"vsi_assignment_mode\", \"lif_profile\", \"oam_instance\", "
    "\"termination_profile\", \"action_profile\", \"default_frwrd_profile\", "
    "\"model\" and \"protection_pass_val\" are invalid in Soc_petraB. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },


  /*
   * Last element. Do no touch.
   */
  SOC_ERR_DESC_ELEMENT_DEF_LAST
};

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
  soc_pb_pp_l2_lif_init_unsafe(
    SOC_SAND_IN  int                                     unit
  )
{
  SOC_PB_PP_REGS
    *pp_regs;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);


  /* prefix of AC MP: ASD: tells:outlif of type AC */
  pp_regs = soc_pb_pp_regs();

  SOC_PB_PP_REG_SET(pp_regs->ihp.asd_ac_prefix_reg, 0x32, 10, exit);

  do
  {
    SOC_PB_PP_IHP_SEM_OPCODE_IP_OFFSETS_TBL_DATA
      data;

    res = soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);
    data.ipv4_sem_offset = 0;
    data.ipv4_opcode_valid = 1;
    data.ipv4_add_offset_to_base = 0;
    data.ipv6_sem_offset = 0;
    data.ipv6_opcode_valid = 1;
    data.ipv6_add_offset_to_base = 0;
    res = soc_pb_pp_ihp_sem_opcode_ip_offsets_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);
  } while(0);

  do
  {
    SOC_PB_PP_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_DATA
      data;

    res = soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);
    data.tc_dp_add_offset_to_base = 0;
    data.tc_dp_opcode_valid = 1;
    data.tc_dp_sem_offset = 0;
    res = soc_pb_pp_ihp_sem_opcode_tc_dp_offsets_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 17, exit);
  } while(0);

  do
  {
    SOC_PB_PP_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_DATA
      data;

    res = soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);
    data.pcp_dei_add_offset_to_base = 0;
    data.pcp_dei_opcode_valid = 1;
    data.pcp_dei_sem_offset = 0;
    res = soc_pb_pp_ihp_sem_opcode_pcp_dei_offsets_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);
  } while(0);

  res = soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_SINGLE_OPERATION);
  SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_l2_lif_init_unsafe()", 0, 0);
}


uint32
  soc_pb_pp_lif_init_unsafe(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = soc_pb_pp_l2_lif_init_unsafe(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lif_init_unsafe()", 0, 0);
}

/*********************************************************************
*     Set MPLS labels that may be used as in-vc-labels
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_l2_lif_pwe_map_range_set_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_IN_VC_RANGE                      *in_vc_range
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_L2_LIF_PWE_MAP_RANGE_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(in_vc_range);

  SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_l2_lif_pwe_map_range_set_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_l2_lif_pwe_map_range_set_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_IN_VC_RANGE                      *in_vc_range
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_L2_LIF_PWE_MAP_RANGE_SET_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_L2_LIF_IN_VC_RANGE, in_vc_range, 10, exit);

  SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_l2_lif_pwe_map_range_set_verify()", 0, 0);
}

uint32
  soc_pb_pp_l2_lif_pwe_map_range_get_verify(
    SOC_SAND_IN  int                                     unit
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_L2_LIF_PWE_MAP_RANGE_GET_VERIFY);

  SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_l2_lif_pwe_map_range_get_verify()", 0, 0);
}

/*********************************************************************
*     Set MPLS labels that may be used as in-vc-labels
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_l2_lif_pwe_map_range_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_OUT SOC_PB_PP_L2_LIF_IN_VC_RANGE                      *in_vc_range
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_L2_LIF_PWE_MAP_RANGE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(in_vc_range);

  SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_l2_lif_pwe_map_range_get_unsafe()", 0, 0);
}

/*********************************************************************
*     Enable an in VC label as a PWE handle, and set the PWE
 *     attributes
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_l2_lif_pwe_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                        in_vc_label,
    SOC_SAND_IN  SOC_PB_PP_LIF_ID                              lif_index,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_PWE_INFO                     *pwe_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_L2_LIF_PWE_ADD_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(pwe_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = soc_pb_pp_l2_lif_pwe_add_internal_unsafe(
          unit,
          in_vc_label,
          FALSE,
          lif_index,
          pwe_info,
          success
       );
  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

 /*
  * success or overwriting exist entry
  */
  if (*success == SOC_SAND_SUCCESS || *success == SOC_SAND_FAILURE_OUT_OF_RESOURCES_2)
  {
    res = soc_pb_pp_sw_db_lif_table_entry_use_set(
            unit,
            lif_index,
            SOC_PB_PP_LIF_ENTRY_TYPE_PWE
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_l2_lif_pwe_add_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_l2_lif_pwe_add_internal_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                        in_vc_label,
    SOC_SAND_IN  uint8                                 ignore_key,
    SOC_SAND_IN  SOC_PB_PP_LIF_ID                              lif_index,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_PWE_INFO                     *pwe_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_ISEM_ACCESS_KEY
    sem_key;
  SOC_PB_PP_ISEM_ACCESS_ENTRY
    sem_entry;
  uint8
    is_sa_drop = FALSE;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_L2_LIF_PWE_ADD_INTERNAL_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(pwe_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  if (!ignore_key)
  {
    SOC_PB_PP_CLEAR(&sem_key, SOC_PB_PP_ISEM_ACCESS_KEY, 1);
    sem_key.key_type = SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_MPLS;
    sem_key.key_info.mpls.label = in_vc_label;
    SOC_PB_PP_CLEAR(&sem_entry, SOC_PB_PP_ISEM_ACCESS_ENTRY, 1);
    sem_entry.sem_result_ndx = lif_index;
    sem_entry.sem_op_code = SOC_PB_PP_LIF_OPCODE_NO_COS;
    res = soc_pb_pp_isem_access_entry_add_unsafe(
            unit,
            &sem_key,
            &sem_entry,
            success
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);
  }
  else
  {
    *success = SOC_SAND_SUCCESS;
  }

 /*
  * success or overwriting exist entry
  */
  if (*success == SOC_SAND_SUCCESS || *success == SOC_SAND_FAILURE_OUT_OF_RESOURCES_2)
  {
    if (pwe_info->service_type == SOC_PB_PP_L2_LIF_PWE_SERVICE_TYPE_P2P)
    {
      SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_PWE_P2P_TBL_DATA
        data;

      res = soc_pb_pp_ihp_sem_result_table_label_pwe_p2p_tbl_get_unsafe(
              unit,
              lif_index,
              &data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 17, exit);
      data.valid = 1;
      data.type = 0x0; /* pwe_p2p */
      data.vsi = SOC_PB_PP_VSI_P2P_SERVICE;
      res = soc_pb_pp_fwd_decision_in_buffer_build(SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_LIF_P2P, &pwe_info->default_frwrd.default_forwarding, 
                                               is_sa_drop, &data.destination, &data.isidor_out_lifor_vc_label);
      SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);
      data.tpid_profile = pwe_info->tpid_profile_index;
      data.has_cw = pwe_info->has_cw;
      data.destination_valid = SOC_SAND_BOOL2NUM(TRUE);
      data.model_is_pipe = SOC_SAND_NUM2BOOL(pwe_info->orientation == SOC_SAND_PP_HUB_SPOKE_ORIENTATION_HUB);
      res = soc_pb_pp_ihp_sem_result_table_label_pwe_p2p_tbl_set_unsafe(
              unit,
              lif_index,
              &data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 27, exit);
    }
    if (pwe_info->service_type == SOC_PB_PP_L2_LIF_PWE_SERVICE_TYPE_MP)
    {
      SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_PWE_MP_TBL_DATA
        data;

      res = soc_pb_pp_ihp_sem_result_table_label_pwe_mp_tbl_get_unsafe(
              unit,
              lif_index,
              &data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);
      data.type = 0x1; /* pwe_mp */
      data.valid = 1;
      data.vsi = pwe_info->vsid;
      data.tt_learn_enable = SOC_SAND_NUM2BOOL(pwe_info->learn_record.enable_learning);
      res = soc_pb_pp_fwd_decision_in_buffer_build(SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_LIF_MP, &pwe_info->learn_record.learn_info, FALSE, 
                                               &data.learn_destination, &data.learn_asd);
      SOC_SAND_CHECK_FUNC_RESULT(res, 37, exit);
      data.tpid_profile = pwe_info->tpid_profile_index;
      data.has_cw = SOC_SAND_NUM2BOOL(pwe_info->has_cw);
      data.orientation_is_hub = SOC_SAND_NUM2BOOL(pwe_info->orientation == SOC_SAND_PP_HUB_SPOKE_ORIENTATION_HUB);
      res = soc_pb_pp_ihp_sem_result_table_label_pwe_mp_tbl_set_unsafe(
              unit,
              lif_index,
              &data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_l2_lif_pwe_add_internal_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_l2_lif_pwe_add_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                        in_vc_label,
    SOC_SAND_IN  SOC_PB_PP_LIF_ID                              lif_index,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_PWE_INFO                         *pwe_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_L2_LIF_PWE_ADD_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(in_vc_label, SOC_SAND_PP_MPLS_LABEL_MAX, SOC_SAND_PP_MPLS_LABEL_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(lif_index, SOC_PB_PP_LIF_ID_MAX, SOC_PB_PP_LIF_ID_OUT_OF_RANGE_ERR, 20, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_L2_LIF_PWE_INFO, pwe_info, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_l2_lif_pwe_add_verify()", 0, 0);
}

/*********************************************************************
*     Get PWE attributes and the LIF-index according to
 *     in_vc_label.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_l2_lif_pwe_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                            in_vc_label,
    SOC_SAND_OUT SOC_PB_PP_LIF_ID                                  *lif_index,
    SOC_SAND_OUT SOC_PB_PP_L2_LIF_PWE_INFO                         *pwe_info,
    SOC_SAND_OUT uint8                                     *found
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_L2_LIF_PWE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(lif_index);
  SOC_SAND_CHECK_NULL_INPUT(pwe_info);
  SOC_SAND_CHECK_NULL_INPUT(found);

  res = soc_pb_pp_l2_lif_pwe_get_internal_unsafe(
          unit,
          in_vc_label,
          FALSE,
          lif_index,
          pwe_info,
          found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_l2_lif_pwe_get_unsafe()", 0, 0);
}


uint32
  soc_pb_pp_l2_lif_pwe_get_internal_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                            in_vc_label,
    SOC_SAND_IN  uint8                                     ignore_key,
    SOC_SAND_OUT SOC_PB_PP_LIF_ID                                  *lif_index,
    SOC_SAND_OUT SOC_PB_PP_L2_LIF_PWE_INFO                         *pwe_info,
    SOC_SAND_OUT uint8                                     *found
  )
{
  uint32
    res = SOC_SAND_OK;
  uint8
    is_sa_drop;
  SOC_PB_PP_ISEM_ACCESS_KEY
    sem_key;
  SOC_PB_PP_ISEM_ACCESS_ENTRY
    sem_entry;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_L2_LIF_PWE_GET_INTERNAL_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(lif_index);
  SOC_SAND_CHECK_NULL_INPUT(pwe_info);
  SOC_SAND_CHECK_NULL_INPUT(found);

  SOC_PB_PP_L2_LIF_PWE_INFO_clear(pwe_info);

  if (!ignore_key)
  {
    SOC_PB_PP_CLEAR(&sem_key, SOC_PB_PP_ISEM_ACCESS_KEY, 1);
    sem_key.key_type = SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_MPLS;
    sem_key.key_info.mpls.label = in_vc_label;
    SOC_PB_PP_CLEAR(&sem_entry, SOC_PB_PP_ISEM_ACCESS_ENTRY, 1);
    res = soc_pb_pp_isem_access_entry_get_unsafe(
            unit,
            &sem_key,
            &sem_entry,
            found
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);
  }
  else
  {
    *found = TRUE;
    sem_entry.sem_result_ndx = *lif_index;
  }

  if (*found)
  {

    *lif_index = sem_entry.sem_result_ndx;

    do
    {
      SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_PWE_P2P_TBL_DATA
        data;

      res = soc_pb_pp_ihp_sem_result_table_label_pwe_p2p_tbl_get_unsafe(
              unit,
              *lif_index,
              &data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 17, exit);
      if (data.type == 0x0) /* pwe_p2p */
      {
        pwe_info->service_type = SOC_PB_PP_L2_LIF_PWE_SERVICE_TYPE_P2P;
        pwe_info->has_cw = SOC_SAND_NUM2BOOL(data.has_cw);
        pwe_info->vsid = data.vsi;
        res = soc_pb_pp_fwd_decision_in_buffer_parse(SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_LIF_P2P, data.destination, data.isidor_out_lifor_vc_label, &pwe_info->default_frwrd.default_forwarding, &is_sa_drop);
        SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);
        pwe_info->tpid_profile_index = data.tpid_profile;
        pwe_info->orientation = SOC_SAND_PP_HUB_SPOKE_ORIENTATION_SPOKE;
        pwe_info->default_frwrd.default_frwd_type = SOC_PB_PP_L2_LIF_DFLT_FRWRD_SRC_LIF;
        goto exit;
      }
    } while(0);

    do
    {
      SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_PWE_MP_TBL_DATA
        data;

      res = soc_pb_pp_ihp_sem_result_table_label_pwe_mp_tbl_get_unsafe(
              unit,
              *lif_index,
              &data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 27, exit);
      if (data.type == 0x1) /* pwe_mp */
      {
        pwe_info->service_type = SOC_PB_PP_L2_LIF_PWE_SERVICE_TYPE_MP;
        pwe_info->has_cw = SOC_SAND_NUM2BOOL(data.has_cw);
        pwe_info->learn_record.enable_learning = SOC_SAND_NUM2BOOL(data.tt_learn_enable);
        res = soc_pb_pp_fwd_decision_in_buffer_parse(SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_LIF_MP, data.learn_destination, data.learn_asd,&(pwe_info->learn_record.learn_info), &is_sa_drop);
        SOC_SAND_CHECK_FUNC_RESULT(res, 37, exit);
        pwe_info->orientation = data.orientation_is_hub ? SOC_SAND_PP_HUB_SPOKE_ORIENTATION_HUB : SOC_SAND_PP_HUB_SPOKE_ORIENTATION_SPOKE;
        pwe_info->tpid_profile_index = data.tpid_profile;
        pwe_info->vsid = data.vsi;
        pwe_info->default_frwrd.default_frwd_type = SOC_PB_PP_L2_LIF_DFLT_FRWRD_SRC_VSI;
        goto exit;
      }
    } while(0);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_l2_lif_pwe_get_internal_unsafe()", 0, 0);
}





uint32
  soc_pb_pp_l2_lif_pwe_get_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                        in_vc_label
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_L2_LIF_PWE_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(in_vc_label, SOC_SAND_PP_MPLS_LABEL_MAX, SOC_SAND_PP_MPLS_LABEL_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_l2_lif_pwe_get_verify()", 0, 0);
}

/*********************************************************************
*     Remvoe the in-VC-label
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_l2_lif_pwe_remove_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                        in_vc_label,
    SOC_SAND_OUT uint32                                      *lif_index
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_ISEM_ACCESS_KEY
    sem_key;
  SOC_PB_PP_ISEM_ACCESS_ENTRY
    sem_entry;
  uint8
    found;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_L2_LIF_PWE_REMOVE_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(lif_index);

  SOC_PB_PP_CLEAR(&sem_key, SOC_PB_PP_ISEM_ACCESS_KEY, 1);
  sem_key.key_type = SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_MPLS;
  sem_key.key_info.mpls.label = in_vc_label;

  SOC_PB_PP_CLEAR(&sem_entry, SOC_PB_PP_ISEM_ACCESS_ENTRY, 1);
  res = soc_pb_pp_isem_access_entry_get_unsafe(
          unit,
          &sem_key,
          &sem_entry,
          &found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);
  *lif_index = sem_entry.sem_result_ndx;

  if (found)
  {
    res = soc_pb_pp_isem_access_entry_remove_unsafe(
            unit,
            &sem_key,
            &found
          );
      SOC_SAND_CHECK_FUNC_RESULT(res, 16, exit);
   /*
    * set entry type
    */
    res = soc_pb_pp_sw_db_lif_table_entry_use_set(
            unit,
            *lif_index,
            SOC_PB_PP_LIF_ENTRY_TYPE_EMPTY
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_l2_lif_pwe_remove_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_l2_lif_pwe_remove_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                        in_vc_label
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_L2_LIF_PWE_REMOVE_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(in_vc_label, SOC_SAND_PP_MPLS_LABEL_MAX, SOC_SAND_PP_MPLS_LABEL_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_l2_lif_pwe_remove_verify()", 0, 0);
}

uint32
  soc_pb_pp_l2_lif_vlan_compression_range_valid_set_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint8                                     is_enable,
    SOC_SAND_IN  SOC_PB_PP_PORT                                    pp_port_ndx,
    SOC_SAND_IN  uint8                                     is_outer,
    SOC_SAND_IN  uint32                                      range_ndx
  )
{
  uint32
    res = SOC_SAND_OK,
    fld_val;
  SOC_PB_PP_IHP_VTT_IN_PP_PORT_VLAN_CONFIG_TBL_DATA
    valid_range_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_L2_LIF_VLAN_COMPRESSION_RANGE_VALID_SET_UNSAFE);

  res = soc_pb_pp_ihp_vtt_in_pp_port_vlan_config_tbl_get_unsafe(
          unit,
          pp_port_ndx,
          &valid_range_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  fld_val = (is_enable == TRUE) ? 0x1:0x0;
  
  if(is_outer == TRUE)
  {
    res = soc_sand_bitstream_set(&valid_range_tbl_data.range_valid0, range_ndx, fld_val);
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }
  else
  {
    res = soc_sand_bitstream_set(&valid_range_tbl_data.range_valid1, range_ndx, fld_val);
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  }

  res = soc_pb_pp_ihp_vtt_in_pp_port_vlan_config_tbl_set_unsafe(
          unit,
          pp_port_ndx,
          &valid_range_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_l2_lif_vlan_compression_range_valid_set_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_l2_lif_vlan_compression_range_valid_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      range_ndx,
    SOC_SAND_OUT uint8                                     *is_enable
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_IHP_VTT_IN_PP_PORT_VLAN_CONFIG_TBL_DATA
    valid_range_tbl_data;
  SOC_PB_PP_PORT
    pp_port_ndx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_L2_LIF_VLAN_COMPRESSION_RANGE_VALID_GET_UNSAFE);

  *is_enable = FALSE;

  for (pp_port_ndx = 0; pp_port_ndx <= SOC_PB_PP_PORT_MAX; pp_port_ndx++)
  {
    res = soc_pb_pp_ihp_vtt_in_pp_port_vlan_config_tbl_get_unsafe(
            unit,
            pp_port_ndx,
            &valid_range_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    if (soc_sand_bitstream_have_one(&valid_range_tbl_data.range_valid0,1) || soc_sand_bitstream_have_one(&valid_range_tbl_data.range_valid1,1))
    {
      *is_enable = TRUE;
    }
  }
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_l2_lif_vlan_compression_range_valid_get_unsafe()", 0, 0);
}

uint32
soc_pb_pp_l2_lif_vlan_compression_is_valid_range(
  SOC_SAND_IN  int                                     unit,
  SOC_SAND_IN  SOC_PB_PP_L2_VLAN_RANGE_KEY                       *vlan_range_key
  )
{
  uint32
    res = SOC_SAND_OK,
    range_bitmap[1];
  SOC_PB_PP_IHP_VTT_IN_PP_PORT_VLAN_CONFIG_TBL_DATA
    valid_range_tbl_data;
  SOC_PB_PP_TBLS
    *tables = NULL;
  uint32
    range_ndx;
  SOC_PB_PP_IHP_VLAN_RANGE_COMPRESSION_TABLE_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_L2_LIF_VLAN_COMPRESSION_IS_VALID_RANGE);

  SOC_PETRA_CLEAR(&tbl_data, SOC_PB_PP_IHP_VLAN_RANGE_COMPRESSION_TABLE_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  res = soc_pb_pp_ihp_vtt_in_pp_port_vlan_config_tbl_get_unsafe(
          unit,
          vlan_range_key->local_port_ndx,
          &valid_range_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if(vlan_range_key->is_outer == TRUE)
  {
    *range_bitmap = valid_range_tbl_data.range_valid0;
  }
  else
  {
    *range_bitmap = valid_range_tbl_data.range_valid1;
  }

  for (range_ndx = 0; range_ndx < SOC_SAND_NOF_BITS_IN_UINT32; range_ndx++)
  {
    /* Go over all valid entries for this given port */
    if(soc_sand_bitstream_test_bit(range_bitmap,range_ndx))
    {
      res =  soc_pb_pp_ihp_vlan_range_compression_table_tbl_get_unsafe(
              unit,
              range_ndx,
              &tbl_data
             );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

      /* Check if part of the interval is already included */
      if (!((tbl_data.vlan_range_lower_limit > vlan_range_key->last_vid)
        || (tbl_data.vlan_range_upper_limit < vlan_range_key->first_vid)))
      {
        SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_LIF_VLAN_RANGE_VLAN_RANGE_ALREADY_DEFINED_ERR, 30, exit);
      }
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_l2_lif_vlan_compression_is_valid_range()", 0, 0);
}

/*********************************************************************
*     Enable VLAN compression when adding Attachment Circuits.
 *     Enables defining an AC according to VLAN domain and a
 *     range of VLANs, and not only according to port*VLAN
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_l2_lif_vlan_compression_add_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_L2_VLAN_RANGE_KEY                       *vlan_range_key,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                          *success
  )
{
  uint32
    res = SOC_SAND_OK;
  uint8
    vlan_compression_range_multiset_first_appear;
  uint32
    ref_count,
    range_internal_key,
    vlan_compression_range_multiset_ndx;
  SOC_PB_PP_IHP_VLAN_RANGE_COMPRESSION_TABLE_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_L2_LIF_VLAN_COMPRESSION_ADD_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(vlan_range_key);
  SOC_SAND_CHECK_NULL_INPUT(success);

  SOC_PETRA_CLEAR(&tbl_data, SOC_PB_PP_IHP_VLAN_RANGE_COMPRESSION_TABLE_TBL_DATA, 1);

  *success = SOC_SAND_SUCCESS;

  /* Internal key for the range table */
  range_internal_key = SOC_PB_PP_L2_LIF_VLAN_RANGE_TO_INTERNAL_KEY(vlan_range_key->first_vid,vlan_range_key->last_vid);
  tbl_data.vlan_range_lower_limit = vlan_range_key->first_vid;
  tbl_data.vlan_range_upper_limit = vlan_range_key->last_vid;

  /* check entry is available in the range table */
  res = soc_pb_sw_db_multiset_lookup(
          unit,
          SOC_PB_PP_SW_DB_MULTI_SET_L2_LIF_VLAN_COMPRESSION_RANGE,
          range_internal_key,
          &vlan_compression_range_multiset_ndx,
          &ref_count
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  if (ref_count == 0)
  {
    /* Range is not existed, add new entry */
    res = soc_pb_sw_db_multiset_add(
      unit,
      SOC_PB_PP_SW_DB_MULTI_SET_L2_LIF_VLAN_COMPRESSION_RANGE,
      range_internal_key,
      &vlan_compression_range_multiset_ndx,
      &vlan_compression_range_multiset_first_appear,
      success
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    if (*success != SOC_SAND_SUCCESS)
    {
      goto exit;
    }

    if (vlan_compression_range_multiset_first_appear)
    {
      /* Add to HW */
      res =  soc_pb_pp_ihp_vlan_range_compression_table_tbl_set_unsafe(
              unit,
              vlan_compression_range_multiset_ndx,
              &tbl_data
             );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
    }
  }

  if (*success == SOC_SAND_SUCCESS)
  {
    /* Enable range on inner, outer tags, depends on vid tag */
    res = soc_pb_pp_l2_lif_vlan_compression_range_valid_set_unsafe(
            unit,
            TRUE,
            vlan_range_key->local_port_ndx,
            vlan_range_key->is_outer,
            vlan_compression_range_multiset_ndx
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);
 }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_l2_lif_vlan_compression_add_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_l2_lif_vlan_compression_add_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_L2_VLAN_RANGE_KEY                       *vlan_range_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_L2_LIF_VLAN_COMPRESSION_ADD_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_L2_VLAN_RANGE_KEY, vlan_range_key, 10, exit);

  /* Check the interval is not already defined in the given port */
  res = soc_pb_pp_l2_lif_vlan_compression_is_valid_range(
          unit,
          vlan_range_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_l2_lif_vlan_compression_add_verify()", 0, 0);
}

/*********************************************************************
*     Remove a VLAN range from the compressed VLAN ranges
 *     database
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_l2_lif_vlan_compression_remove_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_L2_VLAN_RANGE_KEY                       *vlan_range_key
  )
{
  uint32
    res = SOC_SAND_OK,
    range_internal_key,
    vlan_compression_range_multiset_ndx,
    ref_count;
  uint8
    last_appear,
    is_enable;
  SOC_PB_PP_IHP_VLAN_RANGE_COMPRESSION_TABLE_TBL_DATA
    tbl_data;
  SOC_PB_PP_IHP_VTT_IN_PP_PORT_VLAN_CONFIG_TBL_DATA
    valid_range_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_L2_LIF_VLAN_COMPRESSION_REMOVE_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(vlan_range_key);

  SOC_PETRA_CLEAR(&tbl_data, SOC_PB_PP_IHP_VLAN_RANGE_COMPRESSION_TABLE_TBL_DATA, 1);

  res = soc_pb_pp_ihp_vtt_in_pp_port_vlan_config_tbl_get_unsafe(
    unit,
    vlan_range_key->local_port_ndx,
    &valid_range_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  range_internal_key = SOC_PB_PP_L2_LIF_VLAN_RANGE_TO_INTERNAL_KEY(vlan_range_key->first_vid,vlan_range_key->last_vid);

  res = soc_pb_sw_db_multiset_lookup(
          unit,
          SOC_PB_PP_SW_DB_MULTI_SET_L2_LIF_VLAN_COMPRESSION_RANGE,
          range_internal_key,
          &vlan_compression_range_multiset_ndx,
          &ref_count
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  if(ref_count != 0)
  {
    /* Disable range on inner, outer tags, depends on is_outer */
    res = soc_pb_pp_l2_lif_vlan_compression_range_valid_set_unsafe(
            unit,
            FALSE,
            vlan_range_key->local_port_ndx,
            vlan_range_key->is_outer,
            vlan_compression_range_multiset_ndx
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

    /* Check if this range is still needed. If not, remove it. */
    res = soc_pb_pp_l2_lif_vlan_compression_range_valid_get_unsafe(
            unit,
            vlan_compression_range_multiset_ndx,
            &is_enable
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

    /* Remove Range from HW, only if both vlan ranges don't use it */
    if(is_enable == FALSE)
    {
      /* Decrease count for this range */
      res = soc_pb_sw_db_multiset_remove(
              unit,
              SOC_PB_PP_SW_DB_MULTI_SET_L2_LIF_VLAN_COMPRESSION_RANGE,
              range_internal_key,
              &vlan_compression_range_multiset_ndx,
              &last_appear
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

      /* Remove from HW */
      res =  soc_pb_pp_ihp_vlan_range_compression_table_tbl_set_unsafe(
              unit,
              vlan_compression_range_multiset_ndx,
              &tbl_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
    }
    
    
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_l2_lif_vlan_compression_remove_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_l2_lif_vlan_compression_remove_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_L2_VLAN_RANGE_KEY                       *vlan_range_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_L2_LIF_VLAN_COMPRESSION_REMOVE_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_L2_VLAN_RANGE_KEY, vlan_range_key, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_l2_lif_vlan_compression_remove_verify()", 0, 0);
}

/*********************************************************************
 *     Get all compressed VLAN ranges of a port
 *     Details: in the H file. (search for prototype)
 *********************************************************************/
uint32
  soc_pb_pp_l2_lif_vlan_compression_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                             local_port_ndx,
    SOC_SAND_OUT SOC_PB_PP_L2_PORT_VLAN_RANGE_INFO          *vlan_range_info
  )
{
  uint32
    res = SOC_SAND_OK,
    outer_range_bitmap[1],
    inner_range_bitmap[1];
  SOC_PB_PP_IHP_VTT_IN_PP_PORT_VLAN_CONFIG_TBL_DATA
    valid_range_tbl_data;
  uint32
    range_ndx;
  SOC_PB_PP_IHP_VLAN_RANGE_COMPRESSION_TABLE_TBL_DATA
    tbl_data;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_L2_LIF_VLAN_COMPRESSION_GET_UNSAFE);
  
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
  
  res = soc_pb_pp_ihp_vtt_in_pp_port_vlan_config_tbl_get_unsafe(
          unit,
          local_port_ndx,
          &valid_range_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  *outer_range_bitmap = valid_range_tbl_data.range_valid0;
  *inner_range_bitmap = valid_range_tbl_data.range_valid1;

  for (range_ndx = 0; range_ndx < SOC_PB_PP_LIF_VLAN_RANGE_MAX_SIZE; range_ndx++)
  {
    /* Go over all outer range valid entries for this given port */
    if(soc_sand_bitstream_test_bit(outer_range_bitmap, range_ndx))
    {
    
      SOC_PETRA_CLEAR(&tbl_data, SOC_PB_PP_IHP_VLAN_RANGE_COMPRESSION_TABLE_TBL_DATA, 1);
      res =  soc_pb_pp_ihp_vlan_range_compression_table_tbl_get_unsafe(
              unit,
              range_ndx,
              &tbl_data
             );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

      vlan_range_info->outer_vlan_range[range_ndx].first_vid = tbl_data.vlan_range_lower_limit;
      vlan_range_info->outer_vlan_range[range_ndx].last_vid = tbl_data.vlan_range_upper_limit;
    }

     /* Go over all inner range valid entries for this given port */
    if(soc_sand_bitstream_test_bit(inner_range_bitmap, range_ndx))
    {
      SOC_PETRA_CLEAR(&tbl_data, SOC_PB_PP_IHP_VLAN_RANGE_COMPRESSION_TABLE_TBL_DATA, 1);
      res =  soc_pb_pp_ihp_vlan_range_compression_table_tbl_get_unsafe(
              unit,
              range_ndx,
              &tbl_data
             );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

      vlan_range_info->inner_vlan_range[range_ndx].first_vid = tbl_data.vlan_range_lower_limit;
      vlan_range_info->inner_vlan_range[range_ndx].last_vid = tbl_data.vlan_range_upper_limit;
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_l2_lif_vlan_compression_get_unsafe()", local_port_ndx, 0);
}

uint32
  soc_pb_pp_l2_lif_vlan_compression_get_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                             local_port_ndx,
    SOC_SAND_OUT SOC_PB_PP_L2_PORT_VLAN_RANGE_INFO          *vlan_range_info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_L2_LIF_VLAN_COMPRESSION_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(local_port_ndx, (SOC_PETRA_NOF_LOCAL_PORTS-1), SOC_PB_PORT_NDX_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_l2_lif_vlan_compression_get_verify()", local_port_ndx, 0);
}


/*********************************************************************
*     Sets the packets attributes (in-port, VIDs) to consider
 *     when associating an incoming packet to in-AC.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_l2_lif_ac_map_key_set_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_KEY_QUALIFIER                 *qual_key,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_MAP_KEY_TYPE                  key_mapping
  )
{
  uint32
    key_program_profile,
    res = SOC_SAND_OK;
  SOC_PB_PP_IHP_LLVP_PROG_SEL_TBL_DATA
    llvp_prog_sel_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_L2_LIF_AC_MAP_KEY_SET_UNSAFE);

  key_program_profile = SOC_PB_PARSER_DEFAULT_KEY_PROGRAM_PROFILE;
  switch(key_mapping)
  {
  case SOC_PB_PP_L2_LIF_AC_MAP_KEY_TYPE_PORT:
    llvp_prog_sel_tbl_data.llvp_prog_sel = SOC_PB_PP_ISEM_ACCESS_1ST_LOOKUP_PROGRAM_VD;
    break;
  case SOC_PB_PP_L2_LIF_AC_MAP_KEY_TYPE_PORT_VLAN:
    llvp_prog_sel_tbl_data.llvp_prog_sel = SOC_PB_PP_ISEM_ACCESS_1ST_LOOKUP_PROGRAM_VD_VID;
    break;
  case SOC_PB_PP_L2_LIF_AC_MAP_KEY_TYPE_PORT_VLAN_VLAN:
    llvp_prog_sel_tbl_data.llvp_prog_sel = SOC_PB_PP_ISEM_ACCESS_1ST_LOOKUP_PROGRAM_VD_VID_VID;
    break;
  case SOC_PB_PP_L2_LIF_AC_MAP_KEY_TYPE_RAW:
    llvp_prog_sel_tbl_data.llvp_prog_sel = SOC_PB_PP_ISEM_ACCESS_1ST_LOOKUP_PROGRAM_EXT_KEY;
    key_program_profile = SOC_PB_PARSER_INJECTED_KEY_PROGRAM_PROFILE;
    break;
  case SOC_PB_PP_L2_LIF_AC_MAP_KEY_TYPE_RAW_VLAN:
    llvp_prog_sel_tbl_data.llvp_prog_sel = SOC_PB_PP_ISEM_ACCESS_1ST_LOOKUP_PROGRAM_EXT_KEY_VID;
    key_program_profile = SOC_PB_PARSER_INJECTED_KEY_PROGRAM_PROFILE;
    break;
  case SOC_PB_PP_L2_LIF_AC_MAP_KEY_TYPE_RAW_VLAN_VLAN:
    llvp_prog_sel_tbl_data.llvp_prog_sel = SOC_PB_PP_ISEM_ACCESS_1ST_LOOKUP_PROGRAM_EXT_KEY_VID_VID;
    key_program_profile = SOC_PB_PARSER_INJECTED_KEY_PROGRAM_PROFILE;
    break;
  case SOC_PB_PP_L2_LIF_AC_MAP_KEY_TYPE_PORT_COMP_VLAN:
    llvp_prog_sel_tbl_data.llvp_prog_sel = SOC_PB_PP_ISEM_ACCESS_1ST_LOOKUP_PROGRAM_VD_COMP_VID;
    break;
  case SOC_PB_PP_L2_LIF_AC_MAP_KEY_TYPE_PORT_COMP_VLAN_COMP_VLAN:
    llvp_prog_sel_tbl_data.llvp_prog_sel = SOC_PB_PP_ISEM_ACCESS_1ST_LOOKUP_PROGRAM_VD_COMP_VID_COMP_VID;
    break;
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_LIF_KEY_MAPPING_OUT_OF_RANGE_ERR, 10, exit);
  }

  res = soc_pb_pp_ihp_llvp_prog_sel_tbl_set_unsafe(
          unit,
          SOC_PB_PP_IHP_LLVP_PROG_SEL_OFFSETS_KEY_ENTRY_OFFSET(key_program_profile, qual_key->pkt_parse_info, qual_key->port_profile),
          &llvp_prog_sel_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 14, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_l2_lif_ac_map_key_set_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_l2_lif_ac_map_key_set_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_KEY_QUALIFIER                 *qual_key,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_MAP_KEY_TYPE                  key_mapping
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_L2_LIF_AC_MAP_KEY_SET_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_L2_LIF_AC_KEY_QUALIFIER, qual_key, 15, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(key_mapping, SOC_PB_PP_LIF_KEY_MAPPING_MAX, SOC_PB_PP_LIF_KEY_MAPPING_OUT_OF_RANGE_ERR, 30, exit);

  if (qual_key->pkt_parse_info.inner_tpid == SOC_PB_PP_LLP_PARSE_TPID_INDEX_NONE)
  {
    if (key_mapping == SOC_PB_PP_L2_LIF_AC_MAP_KEY_TYPE_PORT_VLAN_VLAN || key_mapping == SOC_PB_PP_L2_LIF_AC_MAP_KEY_TYPE_PORT_COMP_VLAN_COMP_VLAN)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_LIF_ILLEGAL_KEY_MAPPING_ERR, 10, exit);
    }
  }

  if (qual_key->pkt_parse_info.outer_tpid == SOC_PB_PP_LLP_PARSE_TPID_INDEX_NONE
    && qual_key->pkt_parse_info.inner_tpid == SOC_PB_PP_LLP_PARSE_TPID_INDEX_NONE)
  {
    if (key_mapping == SOC_PB_PP_L2_LIF_AC_MAP_KEY_TYPE_PORT_COMP_VLAN)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_LIF_ILLEGAL_KEY_MAPPING_ERR, 20, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_l2_lif_ac_map_key_set_verify()", 0, 0);
}

uint32
  soc_pb_pp_l2_lif_ac_map_key_get_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_KEY_QUALIFIER                 *qual_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_L2_LIF_AC_MAP_KEY_GET_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_L2_LIF_AC_KEY_QUALIFIER, qual_key, 15, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_l2_lif_ac_map_key_get_verify()", 0, 0);
}

/*********************************************************************
*     Sets the packets attributes (in-port, VIDs) to consider
 *     when associating an incoming packet to in-AC.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_l2_lif_ac_map_key_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_KEY_QUALIFIER                 *qual_key,
    SOC_SAND_OUT SOC_PB_PP_L2_LIF_AC_MAP_KEY_TYPE                  *key_mapping
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_IHP_LLVP_PROG_SEL_TBL_DATA
    llvp_prog_sel_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_L2_LIF_AC_MAP_KEY_GET_UNSAFE);

  res = soc_pb_pp_ihp_llvp_prog_sel_tbl_get_unsafe(
          unit,
          SOC_PB_PP_IHP_LLVP_PROG_SEL_OFFSETS_KEY_DEFAULT_ENTRY_OFFSET(qual_key->pkt_parse_info, qual_key->port_profile),
          &llvp_prog_sel_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 14, exit);

  switch(llvp_prog_sel_tbl_data.llvp_prog_sel)
  {
  case SOC_PB_PP_ISEM_ACCESS_1ST_LOOKUP_PROGRAM_VD:
    *key_mapping = SOC_PB_PP_L2_LIF_AC_MAP_KEY_TYPE_PORT;
    break;
  case SOC_PB_PP_ISEM_ACCESS_1ST_LOOKUP_PROGRAM_VD_VID:
    *key_mapping = SOC_PB_PP_L2_LIF_AC_MAP_KEY_TYPE_PORT_VLAN;
    break;
  case SOC_PB_PP_ISEM_ACCESS_1ST_LOOKUP_PROGRAM_VD_VID_VID:
    *key_mapping = SOC_PB_PP_L2_LIF_AC_MAP_KEY_TYPE_PORT_VLAN_VLAN;
    break;
  case SOC_PB_PP_ISEM_ACCESS_1ST_LOOKUP_PROGRAM_EXT_KEY:
    *key_mapping = SOC_PB_PP_L2_LIF_AC_MAP_KEY_TYPE_RAW;
    break;
  case SOC_PB_PP_ISEM_ACCESS_1ST_LOOKUP_PROGRAM_EXT_KEY_VID:
    *key_mapping = SOC_PB_PP_L2_LIF_AC_MAP_KEY_TYPE_RAW_VLAN;
    break;
  case SOC_PB_PP_ISEM_ACCESS_1ST_LOOKUP_PROGRAM_EXT_KEY_VID_VID:
    *key_mapping = SOC_PB_PP_L2_LIF_AC_MAP_KEY_TYPE_RAW_VLAN_VLAN;
    break;
  case SOC_PB_PP_ISEM_ACCESS_1ST_LOOKUP_PROGRAM_VD_COMP_VID:
    *key_mapping = SOC_PB_PP_L2_LIF_AC_MAP_KEY_TYPE_PORT_COMP_VLAN;
    break;
  case SOC_PB_PP_ISEM_ACCESS_1ST_LOOKUP_PROGRAM_VD_COMP_VID_COMP_VID:
    *key_mapping = SOC_PB_PP_L2_LIF_AC_MAP_KEY_TYPE_PORT_COMP_VLAN_COMP_VLAN;
    break;
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_LIF_KEY_MAPPING_OUT_OF_RANGE_ERR, 10, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_l2_lif_ac_map_key_get_unsafe()", 0, 0);
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
  soc_pb_pp_l2_lif_ac_mp_info_set_unsafe(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PB_PP_LIF_ID                       lif_ndx,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_MP_INFO            *info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_AC_TBL_DATA
    data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_L2_LIF_AC_MP_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_pb_pp_ihp_sem_result_table_ac_p2p_to_ac_tbl_get_unsafe(
          unit,
          lif_ndx,
          &data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  data.cfm_trap_valid = info->is_valid;
  data.cfm_max_level = info->mp_level;
  res = soc_pb_pp_ihp_sem_result_table_ac_p2p_to_ac_tbl_set_unsafe(
          unit,
          lif_ndx,
          &data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_l2_lif_ac_mp_info_set_unsafe()", lif_ndx, 0);
}

uint32
  soc_pb_pp_l2_lif_ac_mp_info_set_verify(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PB_PP_LIF_ID                       lif_ndx,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_MP_INFO            *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_L2_LIF_AC_MP_INFO_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(lif_ndx, SOC_PB_PP_LIF_ID_MAX, SOC_PB_PP_LIF_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_L2_LIF_AC_MP_INFO, info, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_l2_lif_ac_mp_info_set_verify()", lif_ndx, 0);
}

uint32
  soc_pb_pp_l2_lif_ac_mp_info_get_verify(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PB_PP_LIF_ID                       lif_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_L2_LIF_AC_MP_INFO_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(lif_ndx, SOC_PB_PP_LIF_ID_MAX, SOC_PB_PP_LIF_ID_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_l2_lif_ac_mp_info_get_verify()", lif_ndx, 0);
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
  soc_pb_pp_l2_lif_ac_mp_info_get_unsafe(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PB_PP_LIF_ID                       lif_ndx,
    SOC_SAND_OUT SOC_PB_PP_L2_LIF_AC_MP_INFO            *info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_AC_TBL_DATA
    data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_L2_LIF_AC_MP_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PB_PP_L2_LIF_AC_MP_INFO_clear(info);

  res = soc_pb_pp_ihp_sem_result_table_ac_p2p_to_ac_tbl_get_unsafe(
          unit,
          lif_ndx,
          &data
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  info->is_valid = SOC_SAND_NUM2BOOL(data.cfm_trap_valid);
  info->mp_level = data.cfm_max_level;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_l2_lif_ac_mp_info_get_unsafe()", lif_ndx, 0);
}

uint32
  soc_pb_pp_l2_lif_ac_key_to_sem_key_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_KEY                           *ac_key,
    SOC_SAND_OUT SOC_PB_PP_ISEM_ACCESS_KEY                         *sem_key
  )
{
  uint8
    is_raw_type,
    is_comp_type=FALSE;
  SOC_PB_PP_L2_LIF_AC_MAP_KEY_TYPE
    simple_type,
    type_vlan;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  if (ac_key->key_type == SOC_PB_PP_L2_LIF_AC_MAP_KEY_TYPE_PORT_COMP_VLAN || ac_key->key_type == SOC_PB_PP_L2_LIF_AC_MAP_KEY_TYPE_PORT_COMP_VLAN_COMP_VLAN)
  {
    is_comp_type = TRUE;
  }

  if (
      (ac_key->key_type != SOC_PB_PP_L2_LIF_AC_MAP_KEY_TYPE_RAW)
      && (ac_key->key_type != SOC_PB_PP_L2_LIF_AC_MAP_KEY_TYPE_RAW_VLAN)
      && (ac_key->key_type != SOC_PB_PP_L2_LIF_AC_MAP_KEY_TYPE_RAW_VLAN_VLAN)
     )
  {
    sem_key->key_info.l2_eth.vlan_domain = ac_key->vlan_domain;
    simple_type = SOC_PB_PP_L2_LIF_AC_MAP_KEY_TYPE_PORT;

    if ((ac_key->key_type == SOC_PB_PP_L2_LIF_AC_MAP_KEY_TYPE_PORT_COMP_VLAN ||
        ac_key->key_type == SOC_PB_PP_L2_LIF_AC_MAP_KEY_TYPE_PORT_COMP_VLAN_COMP_VLAN)
      )
    {
      type_vlan = SOC_PB_PP_L2_LIF_AC_MAP_KEY_TYPE_PORT_COMP_VLAN;
    }
    else
    {
      type_vlan = SOC_PB_PP_L2_LIF_AC_MAP_KEY_TYPE_PORT_VLAN;
    }
    is_raw_type = FALSE;
  }
  else
  {
    sem_key->key_info.raw.raw_val = ac_key->raw_key;
    simple_type = SOC_PB_PP_L2_LIF_AC_MAP_KEY_TYPE_RAW;
    type_vlan = SOC_PB_PP_L2_LIF_AC_MAP_KEY_TYPE_RAW_VLAN;
    is_raw_type = TRUE;
  }

  if ((ac_key->inner_vid == SOC_PB_PP_LIF_IGNORE_INNER_VID &&
      ac_key->outer_vid == SOC_PB_PP_LIF_IGNORE_OUTER_VID)
      || ac_key->key_type == simple_type
     )
  {
    if (is_raw_type == TRUE)
    {
      sem_key->key_type = SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_EXT_KEY;
    }
    else
    {
      sem_key->key_type = SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_VD;
    }
    sem_key->key_info.l2_eth.outer_vid = 0;
    sem_key->key_info.l2_eth.inner_vid = 0;
  }
  else if (ac_key->inner_vid == SOC_PB_PP_LIF_IGNORE_INNER_VID
          || ac_key->key_type == type_vlan)
  {
    if (is_raw_type == TRUE)
    {
      sem_key->key_type = SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_EXT_KEY_VID;
    }
    else if (is_comp_type)
    {
      sem_key->key_type = SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_VD_COMP_VID;
    }
    else
    {
      sem_key->key_type = SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_VD_VID;
    }
    sem_key->key_info.l2_eth.inner_vid = 0;
    sem_key->key_info.l2_eth.outer_vid = ac_key->outer_vid;
  }
  else
  {
    if (is_raw_type == TRUE)
    {
      sem_key->key_type = SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_EXT_KEY_VID_VID;
    }
    else if (is_comp_type)
    {
      sem_key->key_type = SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_VD_COMP_VID_COMP_VID;
    }
    else
    {
      sem_key->key_type = SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_VD_VID_VID;
    }
    sem_key->key_info.l2_eth.inner_vid = ac_key->inner_vid;
    sem_key->key_info.l2_eth.outer_vid = ac_key->outer_vid;
  }

  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_l2_lif_ac_key_to_sem_key_unsafe()", 0, 0);
}

/*********************************************************************
*     Enable an Attachment Circuit, and set the AC attributes.
 *     Bind the ac-key to the LIF, and set the LIF attributes
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_l2_lif_ac_add_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_KEY                           *ac_key,
    SOC_SAND_IN  SOC_PB_PP_LIF_ID                                  lif_index,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_INFO                          *ac_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                          *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_L2_LIF_AC_ADD_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(ac_key);
  SOC_SAND_CHECK_NULL_INPUT(ac_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = soc_pb_pp_l2_lif_ac_add_internal_unsafe(
          unit,
          ac_key,
          FALSE,
          lif_index,
          ac_info,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
 /*
  * set entry type
  */
  if (*success == SOC_SAND_SUCCESS)
  {
    res = soc_pb_pp_sw_db_lif_table_entry_use_set(
            unit,
            lif_index,
            SOC_PB_PP_LIF_ENTRY_TYPE_AC
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_l2_lif_ac_add_unsafe()", 0, 0);
}


uint32
  soc_pb_pp_l2_lif_ac_add_internal_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_KEY                           *ac_key,
    SOC_SAND_IN  uint8                                     ignore_key,
    SOC_SAND_IN  SOC_PB_PP_LIF_ID                                  lif_index,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_INFO                          *ac_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                          *success
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    enc_asd;
  SOC_PB_PP_ISEM_ACCESS_KEY
    sem_key;
  SOC_PB_PP_ISEM_ACCESS_ENTRY
    sem_entry;
  SOC_PB_PP_IHP_VTT_IN_PP_PORT_CONFIG_TBL_DATA
    vtt_port_config_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_L2_LIF_AC_ADD_INTERNAL_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(ac_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  if (!ignore_key)
  {
    SOC_SAND_CHECK_NULL_INPUT(ac_key);
    SOC_PB_PP_CLEAR(&sem_key, SOC_PB_PP_ISEM_ACCESS_KEY, 1);
    
    res = soc_pb_pp_l2_lif_ac_key_to_sem_key_unsafe(unit,ac_key,&sem_key);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    
    SOC_PB_PP_CLEAR(&sem_entry, SOC_PB_PP_ISEM_ACCESS_ENTRY, 1);
    sem_entry.sem_result_ndx = lif_index;
    sem_entry.sem_op_code = SOC_PB_PP_LIF_OPCODE_NO_COS;

    if (sem_key.key_type == SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_VD )
    {
      res = soc_pb_pp_ihp_vtt_in_pp_port_config_tbl_get_unsafe(
              unit,
              ac_key->vlan_domain,
              &vtt_port_config_tbl
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 11, exit);

      vtt_port_config_tbl.default_sem_base = lif_index;
      vtt_port_config_tbl.default_sem_opcode = SOC_PB_PP_LIF_OPCODE_NO_COS;

      res = soc_pb_pp_ihp_vtt_in_pp_port_config_tbl_set_unsafe(
              unit,
              ac_key->vlan_domain,
              &vtt_port_config_tbl
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

      *success = SOC_SAND_SUCCESS;
    }
    else
    {
      res = soc_pb_pp_isem_access_entry_add_unsafe(
              unit,
              &sem_key,
              &sem_entry,
              success
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 14, exit);
    }
  }
  else
  {
    *success = SOC_SAND_SUCCESS;
  }

  if (*success == SOC_SAND_SUCCESS || *success == SOC_SAND_FAILURE_OUT_OF_RESOURCES_2)
  {
    if (ac_info->service_type == SOC_PB_PP_L2_LIF_AC_SERVICE_TYPE_AC2AC)
    {
      SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_AC_TBL_DATA
        data;

      res = soc_pb_pp_ihp_sem_result_table_ac_p2p_to_ac_tbl_get_unsafe(
              unit,
              lif_index,
              &data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 17, exit);
      data.type = 0x0; /* ac_p2p_ac */
      data.cos_profile = ac_info->cos_profile;
      data.vlan_edit_pcp_dei_profile = ac_info->ing_edit_info.edit_pcp_profile;
      data.vlan_edit_profile = ac_info->ing_edit_info.ing_vlan_edit_profile;
      data.vlan_edit_vid = ac_info->ing_edit_info.vid;
      res = soc_pb_pp_fwd_decision_in_buffer_build(SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_LIF_P2P, &ac_info->default_frwrd.default_forwarding, FALSE, 
                                               &data.destination, &data.out_lif);
      SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);
      data.mef_l2_cp_profile = ac_info->l2cp_profile;
      res = soc_pb_pp_ihp_sem_result_table_ac_p2p_to_ac_tbl_set_unsafe(
              unit,
              lif_index,
              &data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 27, exit);
    }
    if (ac_info->service_type == SOC_PB_PP_L2_LIF_AC_SERVICE_TYPE_AC2PWE)
    {
      SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_PWE_TBL_DATA
        data;

      res = soc_pb_pp_ihp_sem_result_table_ac_p2p_to_pwe_tbl_get_unsafe(
              unit,
              lif_index,
              &data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);
      data.type = 0x1; /* ac_p2p_pwe */
      data.cos_profile = ac_info->cos_profile;
      data.vlan_edit_pcp_dei_profile = ac_info->ing_edit_info.edit_pcp_profile;
      data.vlan_edit_profile = ac_info->ing_edit_info.ing_vlan_edit_profile;
      res = soc_pb_pp_fwd_decision_in_buffer_build(SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_LIF_P2P, &ac_info->default_frwrd.default_forwarding, FALSE, 
                                               &data.destination, &data.out_lif_or_vc_label);
      SOC_SAND_CHECK_FUNC_RESULT(res, 37, exit);
      data.mef_l2_cp_profile = ac_info->l2cp_profile;
      res = soc_pb_pp_ihp_sem_result_table_ac_p2p_to_pwe_tbl_set_unsafe(
              unit,
              lif_index,
              &data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);
    }
    if (ac_info->service_type == SOC_PB_PP_L2_LIF_AC_SERVICE_TYPE_AC2PBB)
    {
      SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_PBB_TBL_DATA
        data;

      res = soc_pb_pp_ihp_sem_result_table_ac_p2p_to_pbb_tbl_get_unsafe(
              unit,
              lif_index,
              &data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 47, exit);
      data.type = 0x2; /* ac_p2p_pbb */
      data.cos_profile = ac_info->cos_profile;
      data.vlan_edit_pcp_dei_profile = ac_info->ing_edit_info.edit_pcp_profile;
      data.vlan_edit_profile = ac_info->ing_edit_info.ing_vlan_edit_profile;
      res = soc_pb_pp_fwd_decision_in_buffer_build(SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_LIF_P2P, &ac_info->default_frwrd.default_forwarding, FALSE, 
                                               &data.destination, &data.isid);
      SOC_SAND_CHECK_FUNC_RESULT(res, 52, exit);
      data.mef_l2_cp_profile = ac_info->l2cp_profile;
      res = soc_pb_pp_ihp_sem_result_table_ac_p2p_to_pbb_tbl_set_unsafe(
              unit,
              lif_index,
              &data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 57, exit);
    }
    if (ac_info->service_type == SOC_PB_PP_L2_LIF_AC_SERVICE_TYPE_MP)
    {
      SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_MP_TBL_DATA
        data;

      res = soc_pb_pp_ihp_sem_result_table_ac_mp_tbl_get_unsafe(
              unit,
              lif_index,
              &data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 62, exit);
      data.type = 0x3; /* ac_mp */
      data.cos_profile = ac_info->cos_profile;
      data.vlan_edit_pcp_dei_profile = ac_info->ing_edit_info.edit_pcp_profile;
      data.vlan_edit_profile = ac_info->ing_edit_info.ing_vlan_edit_profile;
      data.vlan_edit_vid = ac_info->ing_edit_info.vid;
      data.learn_destination = 0; /* Learn system port */
      if (!((ac_info->learn_record.learn_type == SOC_PB_PP_L2_LIF_AC_LEARN_SYS_PORT) || (ac_info->learn_record.learn_type == SOC_PB_PP_L2_LIF_AC_LEARN_DISABLE)))
      {
        res = soc_pb_pp_fwd_decision_in_buffer_build(SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_LIF_MP, &ac_info->learn_record.learn_info, FALSE, 
                                                 &data.learn_destination, &enc_asd);
        SOC_SAND_CHECK_FUNC_RESULT(res, 67, exit);
      }
      data.tt_learn_enable = SOC_SAND_BOOL2NUM(ac_info->learn_record.learn_type != SOC_PB_PP_L2_LIF_AC_LEARN_DISABLE);
      data.orientation_is_hub = SOC_SAND_BOOL2NUM(ac_info->orientation == SOC_SAND_PP_HUB_SPOKE_ORIENTATION_HUB);
      data.vsi = ac_info->vsid;
      data.mef_l2_cp_profile = ac_info->l2cp_profile;
      res = soc_pb_pp_ihp_sem_result_table_ac_mp_tbl_set_unsafe(
              unit,
              lif_index,
              &data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 72, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_l2_lif_ac_add_internal_unsafe()", 0, 0);
}



uint32
  soc_pb_pp_l2_lif_ac_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_KEY                       *ac_key,
    SOC_SAND_IN  SOC_PB_PP_LIF_ID                              lif_index,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_INFO                      *ac_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_L2_LIF_AC_ADD_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_L2_LIF_AC_KEY, ac_key, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(lif_index, SOC_PB_PP_LIF_ID_MAX, SOC_PB_PP_LIF_ID_OUT_OF_RANGE_ERR, 20, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_L2_LIF_AC_INFO, ac_info, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_l2_lif_ac_add_verify()", 0, 0);
}

/*********************************************************************
*     Get an Attachment Circuit, according to AC key.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_l2_lif_ac_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_KEY                           *ac_key,
    SOC_SAND_OUT SOC_PB_PP_LIF_ID                                  *lif_index,
    SOC_SAND_OUT SOC_PB_PP_L2_LIF_AC_INFO                          *ac_info,
    SOC_SAND_OUT uint8                                     *found
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_L2_LIF_AC_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(ac_key);
  SOC_SAND_CHECK_NULL_INPUT(lif_index);
  SOC_SAND_CHECK_NULL_INPUT(ac_info);
  SOC_SAND_CHECK_NULL_INPUT(found);

  res = soc_pb_pp_l2_lif_ac_get_internal_unsafe(
          unit,
          ac_key,
          FALSE,
          lif_index,
          ac_info,
          found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_l2_lif_ac_get_unsafe()", 0, 0);
}



uint32
  soc_pb_pp_l2_lif_ac_get_internal_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_KEY                           *ac_key,
    SOC_SAND_IN  uint8                                     ignore_key,
    SOC_SAND_OUT SOC_PB_PP_LIF_ID                                  *lif_index,
    SOC_SAND_OUT SOC_PB_PP_L2_LIF_AC_INFO                          *ac_info,
    SOC_SAND_OUT uint8                                     *found
  )
{
  uint32
    res = SOC_SAND_OK;
  uint8
    is_sa_drop;
  SOC_PB_PP_ISEM_ACCESS_KEY
    sem_key;
  SOC_PB_PP_ISEM_ACCESS_ENTRY
    sem_entry;
  SOC_PB_PP_IHP_VTT_IN_PP_PORT_CONFIG_TBL_DATA
    vtt_port_config_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_L2_LIF_AC_GET_INTERNAL_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(lif_index);
  SOC_SAND_CHECK_NULL_INPUT(ac_info);
  SOC_SAND_CHECK_NULL_INPUT(found);

  if (!ignore_key)
  {
    SOC_SAND_CHECK_NULL_INPUT(ac_key);
  }

  SOC_PB_PP_L2_LIF_AC_INFO_clear(ac_info);

  SOC_PB_PP_CLEAR(&sem_key, SOC_PB_PP_ISEM_ACCESS_KEY, 1);

  if(!ignore_key)
  {
    res = soc_pb_pp_l2_lif_ac_key_to_sem_key_unsafe(unit,ac_key,&sem_key);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);


    SOC_PB_PP_CLEAR(&sem_entry, SOC_PB_PP_ISEM_ACCESS_ENTRY, 1);

    if (sem_key.key_type == SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_VD )
    {
      res = soc_pb_pp_ihp_vtt_in_pp_port_config_tbl_get_unsafe(
              unit,
              ac_key->vlan_domain,
              &vtt_port_config_tbl
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 11, exit);

      *lif_index = vtt_port_config_tbl.default_sem_base;
      sem_entry.sem_result_ndx = vtt_port_config_tbl.default_sem_base;
      
      *found = TRUE;
    }
    else
    {
      res = soc_pb_pp_isem_access_entry_get_unsafe(
              unit,
              &sem_key,
              &sem_entry,
              found
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 14, exit);
    }
  }
  else
  {
    *found = TRUE;
    sem_entry.sem_result_ndx = *lif_index;
  }

  if (*found)
  {
    *lif_index = sem_entry.sem_result_ndx;

    do
    {
      SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_AC_TBL_DATA
        data;

      res = soc_pb_pp_ihp_sem_result_table_ac_p2p_to_ac_tbl_get_unsafe(
              unit,
              *lif_index,
              &data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 17, exit);

      if (data.type == 0x0) /* ac_p2p_ac */
      {
        ac_info->service_type = SOC_PB_PP_L2_LIF_AC_SERVICE_TYPE_AC2AC;
        ac_info->cos_profile = data.cos_profile;
        ac_info->ing_edit_info.edit_pcp_profile = data.vlan_edit_pcp_dei_profile;
        ac_info->ing_edit_info.ing_vlan_edit_profile = data.vlan_edit_profile;
        ac_info->ing_edit_info.vid = data.vlan_edit_vid;
        res = soc_pb_pp_fwd_decision_in_buffer_parse(SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_LIF_P2P, data.destination, data.out_lif, 
                                                 &ac_info->default_frwrd.default_forwarding, &is_sa_drop);
        SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);
        ac_info->default_frwrd.default_frwd_type = SOC_PB_PP_L2_LIF_DFLT_FRWRD_SRC_LIF;
        ac_info->l2cp_profile = data.mef_l2_cp_profile;
        ac_info->orientation = SOC_SAND_PP_HUB_SPOKE_ORIENTATION_SPOKE;
        ac_info->learn_record.learn_type = SOC_PB_PP_L2_LIF_AC_LEARN_DISABLE;
        goto exit;
      }
    } while(0);

    do
    {
      SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_PWE_TBL_DATA
        data;

      res = soc_pb_pp_ihp_sem_result_table_ac_p2p_to_pwe_tbl_get_unsafe(
              unit,
              *lif_index,
              &data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 27, exit);
      if (data.type == 0x1) /* ac_p2p_pwe */
      {
        ac_info->service_type = SOC_PB_PP_L2_LIF_AC_SERVICE_TYPE_AC2PWE;
        ac_info->cos_profile = data.cos_profile;
        ac_info->ing_edit_info.edit_pcp_profile = data.vlan_edit_pcp_dei_profile;
        ac_info->ing_edit_info.ing_vlan_edit_profile = data.vlan_edit_profile;
        res = soc_pb_pp_fwd_decision_in_buffer_parse(SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_LIF_P2P, data.destination, data.out_lif_or_vc_label, 
                                                 &ac_info->default_frwrd.default_forwarding, &is_sa_drop);
        SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);
        ac_info->default_frwrd.default_frwd_type = SOC_PB_PP_L2_LIF_DFLT_FRWRD_SRC_LIF;
        ac_info->l2cp_profile = data.mef_l2_cp_profile;
        ac_info->orientation = SOC_SAND_PP_HUB_SPOKE_ORIENTATION_SPOKE;
        ac_info->learn_record.learn_type = SOC_PB_PP_L2_LIF_AC_LEARN_DISABLE;
        goto exit;
      }
    } while(0);

    do
    {
      SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_PBB_TBL_DATA
        data;

      res = soc_pb_pp_ihp_sem_result_table_ac_p2p_to_pbb_tbl_get_unsafe(
              unit,
              *lif_index,
              &data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 37, exit);
      if (data.type == 0x2) /* ac_p2p_pbb */
      {
        ac_info->service_type = SOC_PB_PP_L2_LIF_AC_SERVICE_TYPE_AC2PBB;
        ac_info->cos_profile = data.cos_profile;
        ac_info->ing_edit_info.edit_pcp_profile = data.vlan_edit_pcp_dei_profile;
        ac_info->ing_edit_info.ing_vlan_edit_profile = data.vlan_edit_profile;
        res = soc_pb_pp_fwd_decision_in_buffer_parse(SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_LIF_P2P, data.destination, data.isid, 
                                                 &ac_info->default_frwrd.default_forwarding, &is_sa_drop);
        SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);
        ac_info->default_frwrd.default_frwd_type = SOC_PB_PP_L2_LIF_DFLT_FRWRD_SRC_LIF;
        ac_info->default_frwrd.default_forwarding.additional_info.eei.type = SOC_PB_PP_EEI_TYPE_MIM;
        ac_info->default_frwrd.default_forwarding.additional_info.eei.val.isid = data.isid;
        ac_info->l2cp_profile = data.mef_l2_cp_profile;
        ac_info->orientation = SOC_SAND_PP_HUB_SPOKE_ORIENTATION_SPOKE;
        ac_info->learn_record.learn_type = SOC_PB_PP_L2_LIF_AC_LEARN_DISABLE;
        goto exit;
      }
    } while(0);

    do
    {
      SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_MP_TBL_DATA
        data;

      res = soc_pb_pp_ihp_sem_result_table_ac_mp_tbl_get_unsafe(
              unit,
              *lif_index,
              &data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 47, exit);
      if (data.type == 0x3) /* ac_mp */
      {
        ac_info->service_type = SOC_PB_PP_L2_LIF_AC_SERVICE_TYPE_MP;
        ac_info->cos_profile = data.cos_profile;
        ac_info->ing_edit_info.edit_pcp_profile = data.vlan_edit_pcp_dei_profile;
        ac_info->ing_edit_info.ing_vlan_edit_profile = data.vlan_edit_profile;
        ac_info->ing_edit_info.vid = data.vlan_edit_vid;
        if (data.tt_learn_enable == 0)
        {
          ac_info->learn_record.learn_type = SOC_PB_PP_L2_LIF_AC_LEARN_DISABLE;
        }
        else if (data.learn_destination == 0)
        {
          ac_info->learn_record.learn_type = SOC_PB_PP_L2_LIF_AC_LEARN_SYS_PORT;
        }
        else
        {
          ac_info->learn_record.learn_type = SOC_PB_PP_L2_LIF_AC_LEARN_INFO;
        }
        
        if (data.learn_destination != 0 && ac_info->learn_record.learn_type == SOC_PB_PP_L2_LIF_AC_LEARN_INFO)
        {
          res = soc_pb_pp_fwd_decision_in_buffer_parse(SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_LIF_MP, data.learn_destination, *lif_index, 
                                                   &ac_info->learn_record.learn_info, &is_sa_drop);
          SOC_SAND_CHECK_FUNC_RESULT(res, 52, exit);
        }
        ac_info->orientation = data.orientation_is_hub ? SOC_SAND_PP_HUB_SPOKE_ORIENTATION_HUB : SOC_SAND_PP_HUB_SPOKE_ORIENTATION_SPOKE;
        ac_info->vsid = data.vsi;
        ac_info->l2cp_profile = data.mef_l2_cp_profile;
        ac_info->default_frwrd.default_frwd_type = SOC_PB_PP_L2_LIF_DFLT_FRWRD_SRC_VSI;
        goto exit;
      }
    } while(0);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_l2_lif_ac_get_internal_unsafe()", 0, 0);
}


uint32
  soc_pb_pp_l2_lif_ac_get_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_KEY                           *ac_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_L2_LIF_AC_GET_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_L2_LIF_AC_KEY, ac_key, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_l2_lif_ac_get_verify()", 0, 0);
}

/*********************************************************************
*     Add Attachment Circuit (AC) group. Enable defining ACs,
 *     according to Quality of Service attributes, on top of
 *     the usual port * VID [*VID] identification
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_l2_lif_ac_with_cos_add_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_KEY                           *ac_key,
    SOC_SAND_IN  SOC_PB_PP_LIF_ID                              base_lif_index,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_GROUP_INFO                    *acs_group_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                          *success
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    enc_asd;
  SOC_PB_PP_ISEM_ACCESS_KEY
    sem_key;
  SOC_PB_PP_ISEM_ACCESS_ENTRY
    sem_entry;
  uint32
    idx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_L2_LIF_AC_WITH_COS_ADD_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(ac_key);
  SOC_SAND_CHECK_NULL_INPUT(acs_group_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  SOC_PB_PP_CLEAR(&sem_key, SOC_PB_PP_ISEM_ACCESS_KEY, 1);
  res = soc_pb_pp_l2_lif_ac_key_to_sem_key_unsafe(unit,ac_key,&sem_key);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_PB_PP_CLEAR(&sem_entry, SOC_PB_PP_ISEM_ACCESS_ENTRY, 1);
  sem_entry.sem_result_ndx = base_lif_index;
  sem_entry.sem_op_code = acs_group_info->opcode_id;
  res = soc_pb_pp_isem_access_entry_add_unsafe(
          unit,
          &sem_key,
          &sem_entry,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

  if (*success == SOC_SAND_SUCCESS || *success == SOC_SAND_FAILURE_OUT_OF_RESOURCES_2)
  {
    for (idx = 0; idx < acs_group_info->nof_lif_entries; ++idx)
    {
      const SOC_PB_PP_L2_LIF_AC_INFO
        *ac_info = acs_group_info->acs_info + idx;

      if (ac_info->service_type == SOC_PB_PP_L2_LIF_AC_SERVICE_TYPE_AC2AC)
      {
        SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_AC_TBL_DATA
          data;

        res = soc_pb_pp_ihp_sem_result_table_ac_p2p_to_ac_tbl_get_unsafe(
                unit,
                base_lif_index + idx,
                &data
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 17, exit);
        data.type = 0x0; /* ac_p2p_ac */
        data.cos_profile = ac_info->cos_profile;
        data.vlan_edit_pcp_dei_profile = ac_info->ing_edit_info.edit_pcp_profile;
        data.vlan_edit_profile = ac_info->ing_edit_info.ing_vlan_edit_profile;
        data.vlan_edit_vid = ac_info->ing_edit_info.vid;
        res = soc_pb_pp_fwd_decision_in_buffer_build(SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_LIF_P2P, &ac_info->default_frwrd.default_forwarding, FALSE, 
                                                 &data.destination, &data.out_lif);
        SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);
        data.mef_l2_cp_profile = ac_info->l2cp_profile;
        res = soc_pb_pp_ihp_sem_result_table_ac_p2p_to_ac_tbl_set_unsafe(
                unit,
                base_lif_index + idx,
                &data
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 27, exit);
      }
      if (ac_info->service_type == SOC_PB_PP_L2_LIF_AC_SERVICE_TYPE_AC2PWE)
      {
        SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_PWE_TBL_DATA
          data;

        res = soc_pb_pp_ihp_sem_result_table_ac_p2p_to_pwe_tbl_get_unsafe(
                unit,
                base_lif_index + idx,
                &data
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);
        data.type = 0x1; /* ac_p2p_pwe */
        data.cos_profile = ac_info->cos_profile;
        data.vlan_edit_pcp_dei_profile = ac_info->ing_edit_info.edit_pcp_profile;
        data.vlan_edit_profile = ac_info->ing_edit_info.ing_vlan_edit_profile;
        res = soc_pb_pp_fwd_decision_in_buffer_build(SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_LIF_P2P, &ac_info->default_frwrd.default_forwarding, FALSE, 
                                                 &data.destination, &data.out_lif_or_vc_label);
        SOC_SAND_CHECK_FUNC_RESULT(res, 37, exit);
        data.mef_l2_cp_profile = ac_info->l2cp_profile;
        res = soc_pb_pp_ihp_sem_result_table_ac_p2p_to_pwe_tbl_set_unsafe(
                unit,
                base_lif_index + idx,
                &data
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);
      }
      if (ac_info->service_type == SOC_PB_PP_L2_LIF_AC_SERVICE_TYPE_AC2PBB)
      {
        SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_PBB_TBL_DATA
          data;

        res = soc_pb_pp_ihp_sem_result_table_ac_p2p_to_pbb_tbl_get_unsafe(
                unit,
                base_lif_index + idx,
                &data
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 47, exit);
        data.type = 0x2; /* ac_p2p_pbb */
        data.cos_profile = ac_info->cos_profile;
        data.vlan_edit_pcp_dei_profile = ac_info->ing_edit_info.edit_pcp_profile;
        data.vlan_edit_profile = ac_info->ing_edit_info.ing_vlan_edit_profile;
        res = soc_pb_pp_fwd_decision_in_buffer_build(SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_LIF_P2P, &ac_info->default_frwrd.default_forwarding, FALSE, 
                                                 &data.destination, &data.isid);
        SOC_SAND_CHECK_FUNC_RESULT(res, 52, exit);
        data.mef_l2_cp_profile = ac_info->l2cp_profile;
        res = soc_pb_pp_ihp_sem_result_table_ac_p2p_to_pbb_tbl_set_unsafe(
                unit,
                base_lif_index + idx,
                &data
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 57, exit);
      }
      if (ac_info->service_type == SOC_PB_PP_L2_LIF_AC_SERVICE_TYPE_MP)
      {
        SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_MP_TBL_DATA
          data;

        res = soc_pb_pp_ihp_sem_result_table_ac_mp_tbl_get_unsafe(
                unit,
                base_lif_index + idx,
                &data
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 62, exit);
        data.type = 0x3; /* ac_mp */
        data.cos_profile = ac_info->cos_profile;
        data.vlan_edit_pcp_dei_profile = ac_info->ing_edit_info.edit_pcp_profile;
        data.vlan_edit_profile = ac_info->ing_edit_info.ing_vlan_edit_profile;
        data.vlan_edit_vid = ac_info->ing_edit_info.vid;
        data.learn_destination = 0; /* Learn system port */
        if (!((ac_info->learn_record.learn_type == SOC_PB_PP_L2_LIF_AC_LEARN_SYS_PORT) || (ac_info->learn_record.learn_type == SOC_PB_PP_L2_LIF_AC_LEARN_DISABLE)))
        {
          res = soc_pb_pp_fwd_decision_in_buffer_build(SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_LIF_MP, &ac_info->learn_record.learn_info, FALSE, 
                                                   &data.learn_destination, &enc_asd);
          SOC_SAND_CHECK_FUNC_RESULT(res, 67, exit);
        }
        data.tt_learn_enable = SOC_SAND_BOOL2NUM(ac_info->learn_record.learn_type != SOC_PB_PP_L2_LIF_AC_LEARN_DISABLE);
        data.orientation_is_hub = SOC_SAND_BOOL2NUM(ac_info->orientation == SOC_SAND_PP_HUB_SPOKE_ORIENTATION_HUB);
        data.vsi = ac_info->vsid;
        data.mef_l2_cp_profile = ac_info->l2cp_profile;
        res = soc_pb_pp_ihp_sem_result_table_ac_mp_tbl_set_unsafe(
                unit,
                base_lif_index + idx,
                &data
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 72, exit);
      }

      res = soc_pb_pp_sw_db_lif_table_entry_use_set(
              unit,
              base_lif_index + idx,
              (idx==0)?SOC_PB_PP_LIF_ENTRY_TYPE_FIRST_AC_IN_GROUP:SOC_PB_PP_LIF_ENTRY_TYPE_MIDDLE_AC_IN_GROUP
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_l2_lif_ac_with_cos_add_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_l2_lif_ac_with_cos_add_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_KEY                           *ac_key,
    SOC_SAND_IN  SOC_PB_PP_LIF_ID                              base_lif_index,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_GROUP_INFO                    *acs_group_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_L2_LIF_AC_WITH_COS_ADD_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_L2_LIF_AC_KEY, ac_key, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(base_lif_index, SOC_PB_PP_LIF_ID_MAX, SOC_PB_PP_LIF_ID_OUT_OF_RANGE_ERR, 20, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_L2_LIF_AC_GROUP_INFO, acs_group_info, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_l2_lif_ac_with_cos_add_verify()", 0, 0);
}

STATIC uint32
  soc_pb_pp_l2_lif_ac_with_cos_opcode_max_offset_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                     opcode_id,
    SOC_SAND_OUT uint32                                     *opcode_max_offset
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_LIF_COS_OPCODE_TYPE
    opcode_type;
  SOC_PB_PP_LIF_COS_OPCODE_ACTION_INFO
    opcode_action_info;
  uint32
    nof_lif_entries = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_L2_LIF_AC_WITH_COS_GET_UNSAFE);

  res = soc_pb_pp_lif_cos_opcode_types_get_unsafe(
          unit,
          opcode_id,
          &opcode_type
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (opcode_type & SOC_PB_PP_LIF_COS_OPCODE_TYPE_L2)
  {
    SOC_SAND_PP_VLAN_TAG_TYPE
      outer_tag;
    SOC_SAND_PP_PCP_UP
      pcp;
    SOC_SAND_PP_DEI_CFI
      dei;

    /* Scan table for all possible outer_tag, PCP, DEI */
    for (outer_tag = 0; outer_tag <= SOC_PB_PP_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_KEY_OUTER_TAG_MAX; ++outer_tag)
    {
      for (pcp = 0; pcp <= SOC_PB_PP_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_KEY_PCP_MAX; ++pcp)
      {
        for (dei = 0; dei <= SOC_PB_PP_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_KEY_DEI_MAX; ++dei)
        {
          res = soc_pb_pp_lif_cos_opcode_vlan_tag_map_get_unsafe(unit, opcode_id, outer_tag, pcp, dei, &opcode_action_info);
          SOC_SAND_CHECK_FUNC_RESULT(res, 17, exit);
          nof_lif_entries = ((opcode_action_info.ac_offset > nof_lif_entries) && (opcode_action_info.is_packet_valid) && 
                             (!opcode_action_info.is_qos_only)) ? opcode_action_info.ac_offset : nof_lif_entries;
        }
      }
    }
  }

  if ((opcode_type & SOC_PB_PP_LIF_COS_OPCODE_TYPE_TC_DP) || (opcode_type & SOC_PB_PP_LIF_COS_OPCODE_TYPE_L3))
  {
    SOC_SAND_PP_TC
      tc;
    SOC_SAND_PP_DP
      dp;

    for (tc = 0; tc <= SOC_PB_PP_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_KEY_TC_MAX; ++tc)
    {
      for (dp = 0; dp <= SOC_PB_PP_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_KEY_DP_MAX; ++dp)
      {
        res = soc_pb_pp_lif_cos_opcode_tc_dp_map_get_unsafe(unit, opcode_id, tc, dp, &opcode_action_info);
        SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);
        nof_lif_entries = ((opcode_action_info.ac_offset > nof_lif_entries) && (opcode_action_info.is_packet_valid) && 
                           (!opcode_action_info.is_qos_only)) ? opcode_action_info.ac_offset : nof_lif_entries;
      }
    }
  }

  if (opcode_type & SOC_PB_PP_LIF_COS_OPCODE_TYPE_L3)
  {
    SOC_SAND_PP_IPV4_TOS
      tos;
    uint32
      tos_ndx;

    /* Scan table for all possible TOS */
    for (tos_ndx = 0; tos_ndx <= SOC_PB_PP_IHP_SEM_OPCODE_IP_OFFSETS_TBL_KEY_TOS_MAX; ++tos_ndx)
    {
      /*
       * To prevent an infinite loop 'for' on all the possible values of
       * SOC_SAND_PP_IPV4_TOS (condition always True)
       */
      tos = (SOC_SAND_PP_IPV4_TOS) tos_ndx;

      res = soc_pb_pp_lif_cos_opcode_ipv4_tos_map_get_unsafe(unit, opcode_id, tos, &opcode_action_info);
      SOC_SAND_CHECK_FUNC_RESULT(res, 17, exit);
      nof_lif_entries = ((opcode_action_info.ac_offset > nof_lif_entries) && (opcode_action_info.is_packet_valid) && 
                         (!opcode_action_info.is_qos_only)) ? opcode_action_info.ac_offset : nof_lif_entries;

      res = soc_pb_pp_lif_cos_opcode_ipv6_tos_map_get_unsafe(unit, opcode_id, tos, &opcode_action_info);
      SOC_SAND_CHECK_FUNC_RESULT(res, 17, exit);
      nof_lif_entries = ((opcode_action_info.ac_offset > nof_lif_entries) && (opcode_action_info.is_packet_valid) && 
                         (!opcode_action_info.is_qos_only)) ? opcode_action_info.ac_offset : nof_lif_entries;
    }
  }

  *opcode_max_offset = nof_lif_entries;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_l2_lif_ac_with_cos_opcode_max_offset_get()", 0, 0);
}

/*********************************************************************
*     Get Attachment Circuit (AC) group attributes
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_l2_lif_ac_with_cos_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_KEY                           *ac_key,
    SOC_SAND_OUT SOC_PB_PP_LIF_ID                              *base_lif_index,
    SOC_SAND_OUT SOC_PB_PP_L2_LIF_AC_GROUP_INFO                    *acs_group_info,
    SOC_SAND_OUT uint8                                     *found
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_ISEM_ACCESS_KEY
    sem_key;
  SOC_PB_PP_ISEM_ACCESS_ENTRY
    sem_entry;
  uint8
    is_sa_drop;
  uint32
    idx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_L2_LIF_AC_WITH_COS_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(base_lif_index);
  SOC_SAND_CHECK_NULL_INPUT(ac_key);
  SOC_SAND_CHECK_NULL_INPUT(acs_group_info);
  SOC_SAND_CHECK_NULL_INPUT(found);

  SOC_PB_PP_L2_LIF_AC_GROUP_INFO_clear(acs_group_info);

  acs_group_info->nof_lif_entries = 0;

  SOC_PB_PP_CLEAR(&sem_key, SOC_PB_PP_ISEM_ACCESS_KEY, 1);

  res = soc_pb_pp_l2_lif_ac_key_to_sem_key_unsafe(unit,ac_key,&sem_key);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_PB_PP_CLEAR(&sem_entry, SOC_PB_PP_ISEM_ACCESS_ENTRY, 1);
  res = soc_pb_pp_isem_access_entry_get_unsafe(
          unit,
          &sem_key,
          &sem_entry,
          found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

  if (*found)
  {
    *base_lif_index = sem_entry.sem_result_ndx;
    acs_group_info->opcode_id = sem_entry.sem_op_code;

    res = soc_pb_pp_l2_lif_ac_with_cos_opcode_max_offset_get(
            unit,
            acs_group_info->opcode_id,
            &acs_group_info->nof_lif_entries
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    for (idx = 0; idx < acs_group_info->nof_lif_entries; ++idx)
    {
      SOC_PB_PP_L2_LIF_AC_INFO
        *ac_info = acs_group_info->acs_info + idx;

      do
      {
        SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_AC_TBL_DATA
          data;

        res = soc_pb_pp_ihp_sem_result_table_ac_p2p_to_ac_tbl_get_unsafe(
                unit,
                sem_entry.sem_result_ndx + idx,
                &data
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

        if (data.type == 0x0) /* ac_p2p_ac */
        {
          ac_info->service_type = SOC_PB_PP_L2_LIF_AC_SERVICE_TYPE_AC2AC;
          ac_info->cos_profile = data.cos_profile;
          ac_info->ing_edit_info.edit_pcp_profile = data.vlan_edit_pcp_dei_profile;
          ac_info->ing_edit_info.ing_vlan_edit_profile = data.vlan_edit_profile;
          ac_info->ing_edit_info.vid = data.vlan_edit_vid;
          res = soc_pb_pp_fwd_decision_in_buffer_parse(SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_LIF_P2P, data.destination, data.out_lif, 
                                                   &ac_info->default_frwrd.default_forwarding, &is_sa_drop);
          SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);
          ac_info->l2cp_profile = data.mef_l2_cp_profile;
          continue;
        }
      } while(0);

      do
      {
        SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_PWE_TBL_DATA
          data;

        res = soc_pb_pp_ihp_sem_result_table_ac_p2p_to_pwe_tbl_get_unsafe(
                unit,
                sem_entry.sem_result_ndx + idx,
                &data
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);
        if (data.type == 0x1) /* ac_p2p_pwe */
        {
          ac_info->service_type = SOC_PB_PP_L2_LIF_AC_SERVICE_TYPE_AC2PWE;
          ac_info->cos_profile = data.cos_profile;
          ac_info->ing_edit_info.edit_pcp_profile = data.vlan_edit_pcp_dei_profile;
          ac_info->ing_edit_info.ing_vlan_edit_profile = data.vlan_edit_profile;
          res = soc_pb_pp_fwd_decision_in_buffer_parse(SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_LIF_P2P, data.destination, data.out_lif_or_vc_label, &ac_info->default_frwrd.default_forwarding, &is_sa_drop);
          SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);
          ac_info->l2cp_profile = data.mef_l2_cp_profile;
          continue;
        }
      } while(0);

      do
      {
        SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_PBB_TBL_DATA
          data;

        res = soc_pb_pp_ihp_sem_result_table_ac_p2p_to_pbb_tbl_get_unsafe(
                unit,
                sem_entry.sem_result_ndx + idx,
                &data
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 52, exit);
        if (data.type == 0x2) /* ac_p2p_pbb */
        {
          ac_info->service_type = SOC_PB_PP_L2_LIF_AC_SERVICE_TYPE_AC2PBB;
          ac_info->cos_profile = data.cos_profile;
          ac_info->ing_edit_info.edit_pcp_profile = data.vlan_edit_pcp_dei_profile;
          ac_info->ing_edit_info.ing_vlan_edit_profile = data.vlan_edit_profile;
          res = soc_pb_pp_fwd_decision_in_buffer_parse(SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_LIF_P2P, data.destination, data.isid, 
                                                   &ac_info->default_frwrd.default_forwarding, &is_sa_drop);
          SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);
          ac_info->l2cp_profile = data.mef_l2_cp_profile;
          continue;
        }
      } while(0);

      do
      {
        SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_MP_TBL_DATA
          data;

        res = soc_pb_pp_ihp_sem_result_table_ac_mp_tbl_get_unsafe(
                unit,
                sem_entry.sem_result_ndx + idx,
                &data
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 62, exit);
        if (data.type == 0x3) /* ac_mp */
        {
          ac_info->service_type = SOC_PB_PP_L2_LIF_AC_SERVICE_TYPE_MP;
          ac_info->cos_profile = data.cos_profile;
          ac_info->ing_edit_info.edit_pcp_profile = data.vlan_edit_pcp_dei_profile;
          ac_info->ing_edit_info.ing_vlan_edit_profile = data.vlan_edit_profile;
          ac_info->ing_edit_info.vid = data.vlan_edit_vid;
          ac_info->orientation = data.orientation_is_hub ? SOC_SAND_PP_HUB_SPOKE_ORIENTATION_HUB : SOC_SAND_PP_HUB_SPOKE_ORIENTATION_SPOKE;
          ac_info->vsid = data.vsi;
          ac_info->l2cp_profile = data.mef_l2_cp_profile;
          if (data.tt_learn_enable == 0)
          {
            ac_info->learn_record.learn_type = SOC_PB_PP_L2_LIF_AC_LEARN_DISABLE;
          }
          else if (data.learn_destination == 0)
          {
            ac_info->learn_record.learn_type = SOC_PB_PP_L2_LIF_AC_LEARN_SYS_PORT;
          }
          else
          {
            ac_info->learn_record.learn_type = SOC_PB_PP_L2_LIF_AC_LEARN_INFO;
          }
          if (data.learn_destination != 0 && ac_info->learn_record.learn_type == SOC_PB_PP_L2_LIF_AC_LEARN_INFO)
          {
            res = soc_pb_pp_fwd_decision_in_buffer_parse(SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_LIF_MP, data.learn_destination, 
                                                     sem_entry.sem_result_ndx + idx, &ac_info->learn_record.learn_info, &is_sa_drop);
            SOC_SAND_CHECK_FUNC_RESULT(res, 67, exit);
          }
          continue;
        }
      } while(0);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_l2_lif_ac_with_cos_get_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_l2_lif_ac_with_cos_get_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_KEY                           *ac_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_L2_LIF_AC_WITH_COS_GET_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_L2_LIF_AC_KEY, ac_key, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_l2_lif_ac_with_cos_get_verify()", 0, 0);
}

/*********************************************************************
*     Remove an Attachment Circuit
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_l2_lif_ac_remove_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_KEY                           *ac_key,
    SOC_SAND_OUT SOC_PB_PP_LIF_ID                              *lif_index
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_ISEM_ACCESS_KEY
    sem_key;
  SOC_PB_PP_ISEM_ACCESS_ENTRY
    sem_entry;
  uint8
    found;
  SOC_PB_PP_IHP_VTT_IN_PP_PORT_CONFIG_TBL_DATA
    vtt_port_config_tbl;
  uint8
    success;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_L2_LIF_AC_REMOVE_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(ac_key);
  SOC_SAND_CHECK_NULL_INPUT(lif_index);

  SOC_PB_PP_CLEAR(&sem_key, SOC_PB_PP_ISEM_ACCESS_KEY, 1);
  res = soc_pb_pp_l2_lif_ac_key_to_sem_key_unsafe(unit,ac_key,&sem_key);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  if (sem_key.key_type == SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_VD )
  {
    res = soc_pb_pp_ihp_vtt_in_pp_port_config_tbl_get_unsafe(
            unit,
            ac_key->vlan_domain,
            &vtt_port_config_tbl
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 11, exit);

    vtt_port_config_tbl.default_sem_base = 0;
    vtt_port_config_tbl.default_sem_opcode = 0;

    res = soc_pb_pp_ihp_vtt_in_pp_port_config_tbl_set_unsafe(
            unit,
            ac_key->vlan_domain,
            &vtt_port_config_tbl
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);
    goto exit;
  }

  SOC_PB_PP_CLEAR(&sem_entry, SOC_PB_PP_ISEM_ACCESS_ENTRY, 1);
  res = soc_pb_pp_isem_access_entry_get_unsafe(
          unit,
          &sem_key,
          &sem_entry,
          &found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

  *lif_index = sem_entry.sem_result_ndx;

  if(found)
  {
    res = soc_pb_pp_isem_access_entry_remove_unsafe(
            unit,
            &sem_key,
            &success
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 17, exit);

   /*
    * set entry type
    */
    res = soc_pb_pp_sw_db_lif_table_entry_use_set(
            unit,
            *lif_index,
            SOC_PB_PP_LIF_ENTRY_TYPE_EMPTY
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_l2_lif_ac_remove_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_l2_lif_ac_remove_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_KEY                           *ac_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_L2_LIF_AC_REMOVE_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_L2_LIF_AC_KEY, ac_key, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_l2_lif_ac_remove_verify()", 0, 0);
}

/*********************************************************************
*     Sets Trap information for Layer 2 control protocol
 *     frames. Packet is an MEF layer 2 control protocol
 *     service frame When DA matches 01-80-c2-00-00-XX where XX
 * = 8'b00xx_xxxx.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_l2_lif_l2cp_trap_set_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_L2CP_KEY                         *l2cp_key,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_L2CP_HANDLE_TYPE                 handle_type
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    index;
  uint32
    buffer[SOC_SAND_MAX3(SOC_PB_PP_IHP_MEF_L2CP_TRANSPARENT_BITMAP_REG_MULT_NOF_REGS, SOC_PB_PP_IHB_MEF_L2CP_DROP_BITMAP_REG_MULT_NOF_REGS, 
                     SOC_PB_PP_IHB_MEF_L2CP_PEER_BITMAP_REG_MULT_NOF_REGS)];
  SOC_PB_PP_REGS
    *regs = soc_pb_pp_regs();

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_L2_LIF_L2CP_TRAP_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(l2cp_key);

  index = SOC_PB_PP_L2_LIF_L2CP_KEY_ENTRY_OFFSET(l2cp_key->l2cp_profile, l2cp_key->da_mac_address_lsb);

  SOC_PB_PP_REG_BUFFER_GET(regs->ihb.mef_l2cp_peer_bitmap_reg, SOC_PB_PP_IHB_MEF_L2CP_PEER_BITMAP_REG_MULT_NOF_REGS, buffer, 10, exit);
  res = soc_sand_bitstream_set(buffer, index, SOC_SAND_NUM2BOOL(handle_type == SOC_PB_PP_L2_LIF_L2CP_HANDLE_TYPE_PEER));
  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);
  SOC_PB_PP_REG_BUFFER_SET(regs->ihb.mef_l2cp_peer_bitmap_reg, SOC_PB_PP_IHB_MEF_L2CP_PEER_BITMAP_REG_MULT_NOF_REGS, buffer, 10, exit);

  SOC_PB_PP_REG_BUFFER_GET(regs->ihb.mef_l2cp_drop_bitmap_reg, SOC_PB_PP_IHB_MEF_L2CP_PEER_BITMAP_REG_MULT_NOF_REGS, buffer, 10, exit);
  res = soc_sand_bitstream_set(buffer, index, SOC_SAND_NUM2BOOL(handle_type == SOC_PB_PP_L2_LIF_L2CP_HANDLE_TYPE_DROP));
  SOC_SAND_CHECK_FUNC_RESULT(res, 17, exit);
  SOC_PB_PP_REG_BUFFER_SET(regs->ihb.mef_l2cp_drop_bitmap_reg, SOC_PB_PP_IHB_MEF_L2CP_PEER_BITMAP_REG_MULT_NOF_REGS, buffer, 10, exit);

  SOC_PB_PP_REG_BUFFER_GET(regs->ihp.mef_l2cp_transparent_bitmap_reg, SOC_PB_PP_IHB_MEF_L2CP_PEER_BITMAP_REG_MULT_NOF_REGS, buffer, 10, exit);
  res = soc_sand_bitstream_set(buffer, index, SOC_SAND_NUM2BOOL(handle_type == SOC_PB_PP_L2_LIF_L2CP_HANDLE_TYPE_TUNNEL));
  SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);
  SOC_PB_PP_REG_BUFFER_SET(regs->ihp.mef_l2cp_transparent_bitmap_reg, SOC_PB_PP_IHB_MEF_L2CP_PEER_BITMAP_REG_MULT_NOF_REGS, buffer, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_l2_lif_l2cp_trap_set_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_l2_lif_l2cp_trap_set_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_L2CP_KEY                         *l2cp_key,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_L2CP_HANDLE_TYPE                 handle_type
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_L2_LIF_L2CP_TRAP_SET_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_L2_LIF_L2CP_KEY, l2cp_key, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(handle_type, SOC_PB_PP_LIF_HANDLE_TYPE_MAX, SOC_PB_PP_LIF_HANDLE_TYPE_OUT_OF_RANGE_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_l2_lif_l2cp_trap_set_verify()", 0, 0);
}

uint32
  soc_pb_pp_l2_lif_l2cp_trap_get_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_L2CP_KEY                         *l2cp_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_L2_LIF_L2CP_TRAP_GET_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_L2_LIF_L2CP_KEY, l2cp_key, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_l2_lif_l2cp_trap_get_verify()", 0, 0);
}

/*********************************************************************
*     Sets Trap information for Layer 2 control protocol
 *     frames. Packet is an MEF layer 2 control protocol
 *     service frame When DA matches 01-80-c2-00-00-XX where XX
 * = 8'b00xx_xxxx.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_l2_lif_l2cp_trap_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_L2CP_KEY                         *l2cp_key,
    SOC_SAND_OUT SOC_PB_PP_L2_LIF_L2CP_HANDLE_TYPE                 *handle_type
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    index;
  uint32
    buffer[SOC_SAND_MAX3(SOC_PB_PP_IHP_MEF_L2CP_TRANSPARENT_BITMAP_REG_MULT_NOF_REGS, SOC_PB_PP_IHB_MEF_L2CP_DROP_BITMAP_REG_MULT_NOF_REGS, 
                     SOC_PB_PP_IHB_MEF_L2CP_PEER_BITMAP_REG_MULT_NOF_REGS)];
  SOC_PB_PP_REGS
    *regs = soc_pb_pp_regs();

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_L2_LIF_L2CP_TRAP_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(l2cp_key);

  *handle_type = SOC_PB_PP_L2_LIF_L2CP_HANDLE_TYPE_NORMAL;
  index = SOC_PB_PP_L2_LIF_L2CP_KEY_ENTRY_OFFSET(l2cp_key->l2cp_profile, l2cp_key->da_mac_address_lsb);

  SOC_PB_PP_REG_BUFFER_GET(regs->ihb.mef_l2cp_peer_bitmap_reg, SOC_PB_PP_IHB_MEF_L2CP_PEER_BITMAP_REG_MULT_NOF_REGS, buffer, 10, exit);
  *handle_type = soc_sand_bitstream_test_bit(buffer, index) ? SOC_PB_PP_L2_LIF_L2CP_HANDLE_TYPE_PEER : *handle_type;

  SOC_PB_PP_REG_BUFFER_GET(regs->ihb.mef_l2cp_drop_bitmap_reg, SOC_PB_PP_IHB_MEF_L2CP_PEER_BITMAP_REG_MULT_NOF_REGS, buffer, 10, exit);
  *handle_type = soc_sand_bitstream_test_bit(buffer, index) ? SOC_PB_PP_L2_LIF_L2CP_HANDLE_TYPE_DROP : *handle_type;

  SOC_PB_PP_REG_BUFFER_GET(regs->ihp.mef_l2cp_transparent_bitmap_reg, SOC_PB_PP_IHB_MEF_L2CP_PEER_BITMAP_REG_MULT_NOF_REGS, buffer, 10, exit);
  *handle_type = soc_sand_bitstream_test_bit(buffer, index) ? SOC_PB_PP_L2_LIF_L2CP_HANDLE_TYPE_TUNNEL : *handle_type;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_l2_lif_l2cp_trap_get_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_l2_lif_isid_add_internal_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_SYS_VSI_ID                              vsi_ndx,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_ISID_KEY                         *isid_key,
    SOC_SAND_IN  uint8                                     ignore_key,
    SOC_SAND_IN  SOC_PB_PP_LIF_ID                                  lif_index,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_ISID_INFO                        *isid_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                          *success
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_ISEM_ACCESS_KEY
    sem_key;
  SOC_PB_PP_ISEM_ACCESS_ENTRY
    sem_entry;
  SOC_PB_PP_IHP_VSI_ISID_TBL_DATA
    isid_tbl_data;
  uint8
    found;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_L2_LIF_ISID_ADD_INTERNAL_UNSAFE);

  if (!ignore_key)
  {
    SOC_SAND_CHECK_NULL_INPUT(isid_key);
  }
  SOC_SAND_CHECK_NULL_INPUT(isid_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  if (!ignore_key) {
    SOC_PB_PP_CLEAR(&sem_key, SOC_PB_PP_ISEM_ACCESS_KEY, 1);

    sem_key.key_type = SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_PBB;
    sem_key.key_info.pbb.isid_domain = isid_key->isid_domain;
    sem_key.key_info.pbb.isid = isid_key->isid_id;

    res = soc_pb_pp_isem_access_entry_get_unsafe(
      unit,
      &sem_key,
      &sem_entry,
      &found
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    sem_entry.sem_result_ndx = lif_index;
    sem_entry.sem_op_code = SOC_PB_PP_LIF_OPCODE_NO_COS;

    res = soc_pb_pp_isem_access_entry_add_unsafe(
      unit,
      &sem_key,
      &sem_entry,
      success
     );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    if(*success != SOC_SAND_SUCCESS)
    {
      goto exit;
    }
  }
  else
  {
    *success = SOC_SAND_SUCCESS;
  }

  if (isid_info->service_type == SOC_PB_PP_L2_LIF_ISID_SERVICE_TYPE_P2P)
  {
    SOC_PB_PP_IHP_SEM_RESULT_TABLE_ISID_P2P_TBL_DATA
      data;
    SOC_PB_PP_MGMT_OPERATION_MODE
      op_mode;

    res = soc_pb_pp_ihp_sem_result_table_isid_p2p_tbl_get_unsafe(
            unit,
            lif_index,
            &data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 17, exit);

    data.type = 0x0; /* pbb_p2p */
    data.service_type_lsb = 0x0; 

    if (isid_info->default_frwrd.default_forwarding.additional_info.eei.type == SOC_PB_PP_EEI_TYPE_MIM)
    {
      /* vsi is mac-in-mac-vsi */
      res = soc_pb_pp_mgmt_operation_mode_get_unsafe(
        unit,
        &op_mode
        );
      SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);

      data.vsi = op_mode.p2p_info.mim_vsi;
    }
    else {
      /* vsi is vsi-p2p-service */
      data.vsi = SOC_PB_PP_VSI_P2P_SERVICE;
    }

    /* if p2p to isid: isidor_out_lifor_vc_label = isid
       else (p2p-ac): isidor_out_lifor_vc_label = 24:0'b outlif {16:1'b valid ,15:1 (out-ac))
       else (p2p-mpls): isidor_out_lifor_vc_label = 24:1'b eei {16:1'b valid ,15:1 (out-ac))
     */
    res = soc_pb_pp_fwd_decision_in_buffer_build(
      SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_LIF_P2P, 
      &isid_info->default_frwrd.default_forwarding, 
      FALSE, 
      &data.destination, 
      &data.isidor_out_lifor_vc_label
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);

    data.tpid_profile_index = isid_info->tpid_profile_index; 
    data.cos_profile = isid_info->cos_profile;

    res = soc_pb_pp_ihp_sem_result_table_isid_p2p_tbl_set_unsafe(
            unit,
            lif_index,
            &data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 27, exit);
  }
  if (isid_info->service_type == SOC_PB_PP_L2_LIF_ISID_SERVICE_TYPE_MP)
  {
    SOC_PB_PP_IHP_SEM_RESULT_TABLE_ISID_MP_TBL_DATA
      data;

    res = soc_pb_pp_ihp_sem_result_table_isid_mp_tbl_get_unsafe(
            unit,
            lif_index,
            &data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

    data.type = 0x1; /* pbb_mp */
    data.service_type_lsb = 0x1; 
    data.vsi = vsi_ndx;
    data.tt_learn_enable = isid_info->learn_enable;
    /* the destination is not valid and therefore 0 */
    data.destination = 0x0;
    data.destination_valid = 0x0;
    data.orientation_is_hub = SOC_SAND_NUM2BOOL(isid_info->orientation == SOC_SAND_PP_HUB_SPOKE_ORIENTATION_HUB);
    data.tpid_profile = isid_info->tpid_profile_index; 
    data.cos_profile = isid_info->cos_profile;

    res = soc_pb_pp_ihp_sem_result_table_isid_mp_tbl_set_unsafe(
            unit,
            lif_index,
            &data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 37, exit);

    if (!ignore_key)
    {
        /* vsi->isid mapping is done only for MP */
        res = soc_pb_pp_ihp_vsi_isid_tbl_get_unsafe(
                unit,
                vsi_ndx,
                &isid_tbl_data
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);
        isid_tbl_data.vsi_or_isid = isid_key->isid_id;
        res = soc_pb_pp_ihp_vsi_isid_tbl_set_unsafe(
                unit,
                vsi_ndx,
                &isid_tbl_data
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 47, exit);
    }
  }

  /* set entry type as isid */
  if (*success == SOC_SAND_SUCCESS)
  {
    res = soc_pb_pp_sw_db_lif_table_entry_use_set(
            unit,
            lif_index,
            SOC_PB_PP_LIF_ENTRY_TYPE_ISID
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_l2_lif_isid_add_internal_unsafe()", vsi_ndx, 0);
}

/*********************************************************************
*     Map an I-SID to VSI and vice versa. Set I-SID related
 *     attributes. Relevant when the packet arrives from the
 *     PBP
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_l2_lif_isid_add_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_SYS_VSI_ID                              vsi_ndx,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_ISID_KEY                         *isid_key,
    SOC_SAND_IN  SOC_PB_PP_LIF_ID                                  lif_index,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_ISID_INFO                        *isid_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                          *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_L2_LIF_ISID_ADD_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(isid_key);
  SOC_SAND_CHECK_NULL_INPUT(isid_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = soc_pb_pp_l2_lif_isid_add_internal_unsafe(
    unit,
    vsi_ndx,
    isid_key,
    FALSE,
    lif_index,
    isid_info,
    success
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_l2_lif_isid_add_unsafe()", vsi_ndx, 0);
}

uint32
  soc_pb_pp_l2_lif_isid_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_SYS_VSI_ID                          vsi_ndx,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_ISID_KEY                     *isid_key,
    SOC_SAND_IN  SOC_PB_PP_LIF_ID                              lif_index,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_ISID_INFO                    *isid_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_L2_LIF_ISID_ADD_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(vsi_ndx, SOC_PB_PP_SYS_VSI_ID_MAX, SOC_PB_PP_SYS_VSI_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_L2_LIF_ISID_KEY, isid_key, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(lif_index, SOC_PB_PP_LIF_ID_MAX, SOC_PB_PP_LIF_ID_OUT_OF_RANGE_ERR, 30, exit);
  /* isid_domain can be either 0 or 16-63 */
  SOC_SAND_ERR_IF_ABOVE_MAX(isid_key->isid_domain, SOC_PB_PP_ISID_DOMAIN_MAX, SOC_PB_PP_ISID_DOMAIN_OUT_OF_RANGE_ERR, 30, exit);
  if (isid_key->isid_domain != 0 && isid_key->isid_domain < 16)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_ISID_DOMAIN_OUT_OF_RANGE_ERR, 30, exit);
  }
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_L2_LIF_ISID_INFO, isid_info, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_l2_lif_isid_add_verify()", vsi_ndx, 0);
}

uint32
  soc_pb_pp_l2_lif_isid_get_internal_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_ISID_KEY                         *isid_key,
    SOC_SAND_IN  uint8                                     ignore_key, 
    SOC_SAND_OUT SOC_PB_PP_LIF_ID                                  *lif_index,
    SOC_SAND_OUT SOC_PB_PP_SYS_VSI_ID                              *vsi_index,
    SOC_SAND_OUT SOC_PB_PP_L2_LIF_ISID_INFO                        *isid_info,
    SOC_SAND_OUT uint8                                     *found
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_ISEM_ACCESS_KEY
    sem_key;
  SOC_PB_PP_ISEM_ACCESS_ENTRY
    sem_entry;
  uint8
      is_sa_drop;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_L2_LIF_ISID_GET_INTERNAL_UNSAFE);

  if (!ignore_key)
  {
    SOC_SAND_CHECK_NULL_INPUT(isid_key);
  }
  SOC_SAND_CHECK_NULL_INPUT(lif_index);
  SOC_SAND_CHECK_NULL_INPUT(vsi_index);
  SOC_SAND_CHECK_NULL_INPUT(isid_info);
  SOC_SAND_CHECK_NULL_INPUT(found);

  SOC_PB_PP_L2_LIF_ISID_INFO_clear(isid_info);
  *vsi_index = 0;

  if (!ignore_key)
  {
    *lif_index = 0;
    SOC_PB_PP_CLEAR(&sem_key, SOC_PB_PP_ISEM_ACCESS_KEY, 1);
    SOC_PB_PP_CLEAR(&sem_entry, SOC_PB_PP_ISEM_ACCESS_ENTRY, 1);

    sem_key.key_type = SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_PBB;
    sem_key.key_info.pbb.isid_domain = isid_key->isid_domain;
    sem_key.key_info.pbb.isid = isid_key->isid_id;

    res = soc_pb_pp_isem_access_entry_get_unsafe(
          unit,
          &sem_key,
          &sem_entry,
          found
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);
  }
  else
  {
    *found = TRUE;
    sem_entry.sem_result_ndx = *lif_index;
  }

  if (*found)
  {
      *lif_index = sem_entry.sem_result_ndx;

      do
      {
          SOC_PB_PP_IHP_SEM_RESULT_TABLE_ISID_P2P_TBL_DATA
              data;

          res = soc_pb_pp_ihp_sem_result_table_isid_p2p_tbl_get_unsafe(  
              unit,
              sem_entry.sem_result_ndx,
              &data
              );
          SOC_SAND_CHECK_FUNC_RESULT(res, 17, exit);

          if (data.type == 0x0) /* isid_p2p */
          {
              res = soc_pb_pp_fwd_decision_in_buffer_parse(SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_LIF_P2P, data.destination, 
                                                   data.isidor_out_lifor_vc_label, &isid_info->default_frwrd.default_forwarding, &is_sa_drop);
              SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);

              isid_info->service_type = SOC_PB_PP_L2_LIF_ISID_SERVICE_TYPE_P2P;
              isid_info->cos_profile = data.cos_profile;
              isid_info->tpid_profile_index = data.tpid_profile_index;

              *vsi_index = data.vsi;
          }
      } while(0);

      do
      {
          SOC_PB_PP_IHP_SEM_RESULT_TABLE_ISID_MP_TBL_DATA
              data;

          res = soc_pb_pp_ihp_sem_result_table_isid_mp_tbl_get_unsafe(
              unit,
              sem_entry.sem_result_ndx,
              &data
              );
          SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);

          if (data.type == 0x1) /* isid_mp */
          {
              isid_info->service_type = SOC_PB_PP_L2_LIF_ISID_SERVICE_TYPE_MP;
              isid_info->cos_profile = data.cos_profile;
              isid_info->tpid_profile_index = data.tpid_profile;
              /* learn_enable and orientation are needed only in MP */
              isid_info->learn_enable = (uint8)data.tt_learn_enable; 
              isid_info->orientation = data.orientation_is_hub;

              *vsi_index = data.vsi;
          }
      } while(0);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_l2_lif_isid_get_internal_unsafe()", 0, 0);
}

/*********************************************************************
*     Map an I-SID to VSI and vice versa. Set I-SID related
 *     attributes. Relevant when the packet arrives from the
 *     PBP
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_l2_lif_isid_get_unsafe(
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

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_L2_LIF_ISID_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(isid_key);
  SOC_SAND_CHECK_NULL_INPUT(lif_index);
  SOC_SAND_CHECK_NULL_INPUT(vsi_index);
  SOC_SAND_CHECK_NULL_INPUT(isid_info);
  SOC_SAND_CHECK_NULL_INPUT(found);

  res = soc_pb_pp_l2_lif_isid_get_internal_unsafe(
    unit,
    isid_key,
    FALSE,
    lif_index,
    vsi_index,
    isid_info,
    found
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_l2_lif_isid_get_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_l2_lif_isid_get_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_ISID_KEY                         *isid_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_L2_LIF_ISID_GET_VERIFY);
  /* isid_domain can be either 0 or 16-63 */
  SOC_SAND_ERR_IF_ABOVE_MAX(isid_key->isid_domain, SOC_PB_PP_ISID_DOMAIN_MAX, SOC_PB_PP_ISID_DOMAIN_OUT_OF_RANGE_ERR, 30, exit);
  if (isid_key->isid_domain != 0 && isid_key->isid_domain < 16)
  {
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_ISID_DOMAIN_OUT_OF_RANGE_ERR, 30, exit);
  }
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_L2_LIF_ISID_KEY, isid_key, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_l2_lif_isid_get_verify()", 0, 0);
}

/*********************************************************************
*     Get the I-SID related to the VSI
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_l2_lif_vsi_to_isid_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_SYS_VSI_ID                              vsi_ndx,
    SOC_SAND_OUT SOC_SAND_PP_ISID                                  *isid_id
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_IHP_VSI_ISID_TBL_DATA
    isid_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_L2_LIF_VSI_TO_ISID_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(isid_id);

  res = soc_pb_pp_ihp_vsi_isid_tbl_get_unsafe(
              unit,
              vsi_ndx,
              &isid_tbl_data
            );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  *isid_id = isid_tbl_data.vsi_or_isid;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_l2_lif_isid_get_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_l2_lif_vsi_to_isid_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_SYS_VSI_ID                              vsi_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_L2_LIF_VSI_TO_ISID_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(vsi_ndx, SOC_PB_PP_SYS_VSI_ID_MAX, SOC_PB_PP_SYS_VSI_ID_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_l2_lif_isid_get_verify()", 0, 0);
}

/*********************************************************************
*     Unbind an I-SID from VSI and vice versa
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_l2_lif_isid_remove_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_ISID_KEY                         *isid_key,
    SOC_SAND_OUT SOC_PB_PP_LIF_ID                                  *lif_index
  )
{
  uint32
    vsi_ndx = 0,
    res = SOC_SAND_OK;
  SOC_PB_PP_ISEM_ACCESS_KEY
      sem_key;
  SOC_PB_PP_ISEM_ACCESS_ENTRY
      sem_entry;
  uint8
      found,
      success;
  SOC_PB_PP_IHP_VSI_ISID_TBL_DATA
    isid_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_L2_LIF_ISID_REMOVE_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(isid_key);
  SOC_SAND_CHECK_NULL_INPUT(lif_index);

  SOC_PB_PP_CLEAR(&sem_key, SOC_PB_PP_ISEM_ACCESS_KEY, 1);
  sem_key.key_type = SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_PBB;
  sem_key.key_info.pbb.isid_domain = isid_key->isid_domain;
  sem_key.key_info.pbb.isid = isid_key->isid_id;
  SOC_PB_PP_CLEAR(&sem_entry, SOC_PB_PP_ISEM_ACCESS_ENTRY, 1);
  res = soc_pb_pp_isem_access_entry_get_unsafe(
          unit,
          &sem_key,
          &sem_entry,
          &found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

  if (found)
  {
      *lif_index = sem_entry.sem_result_ndx;
      
      res = soc_pb_pp_isem_access_entry_remove_unsafe(
          unit,
          &sem_key,
          &success
          );
      SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

      /* 
       * restore table entry
       */
      do
      {
          SOC_PB_PP_IHP_SEM_RESULT_TABLE_ISID_P2P_TBL_DATA
              data;
              
      res = soc_pb_pp_ihp_sem_result_table_isid_p2p_tbl_set_unsafe(  
        unit,
        sem_entry.sem_result_ndx,
        &data
        );
      SOC_SAND_CHECK_FUNC_RESULT(res, 17, exit);
      vsi_ndx = data.vsi;
          
          data.cos_profile = 0;
          data.destination = 0;
          data.isidor_out_lifor_vc_label = 0;
          data.service_type_lsb = 0;
          data.tpid_profile_index = 0;
          data.type = 0;
          data.vsi = 0;
          
          res = soc_pb_pp_ihp_sem_result_table_isid_p2p_tbl_set_unsafe(  
              unit,
              sem_entry.sem_result_ndx,
              &data
              );
          SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);
      } while(0);

      do
      {
          SOC_PB_PP_IHP_SEM_RESULT_TABLE_ISID_MP_TBL_DATA
              data;

      res = soc_pb_pp_ihp_sem_result_table_isid_mp_tbl_set_unsafe(  
        unit,
        sem_entry.sem_result_ndx,
        &data
        );
      SOC_SAND_CHECK_FUNC_RESULT(res, 17, exit);
      vsi_ndx = data.vsi;
      
          data.cos_profile = 0;
          data.destination = 0;
          data.destination_valid = 0;
          data.orientation_is_hub = 0;
          data.service_type_lsb = 0;
          data.tpid_profile = 0;
          data.tt_learn_enable = 0;
          data.type = 0;
          data.vsi = 0;

          res = soc_pb_pp_ihp_sem_result_table_isid_mp_tbl_set_unsafe(  
              unit,
              sem_entry.sem_result_ndx,
              &data
              );
          SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);
      } while(0);
  }
  
  /* remove mapping of VSI to ISID, as well */
  isid_tbl_data.vsi_or_isid = 0;
  res = soc_pb_pp_ihp_vsi_isid_tbl_set_unsafe(
    unit,
    vsi_ndx,
    &isid_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 27, exit);

  /* set entry type as empty */
  res = soc_pb_pp_sw_db_lif_table_entry_use_set(
          unit,
          *lif_index,
          SOC_PB_PP_LIF_ENTRY_TYPE_EMPTY
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_l2_lif_isid_remove_unsafe()",0,0);
}

uint32
  soc_pb_pp_l2_lif_isid_remove_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_ISID_KEY                         *isid_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_L2_LIF_ISID_REMOVE_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_L2_LIF_ISID_KEY, isid_key, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_l2_lif_isid_remove_verify()",0,0);
}

/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     soc_pb_pp_api_lif module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
SOC_PROCEDURE_DESC_ELEMENT*
  soc_pb_pp_lif_get_procs_ptr(void)
{
  return Soc_pb_pp_procedure_desc_element_lif;
}
/*********************************************************************
*     Get the pointer to the list of errors of the
 *     soc_pb_pp_api_lif module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
SOC_ERROR_DESC_ELEMENT*
  soc_pb_pp_lif_get_errs_ptr(void)
{
  return Soc_pb_pp_error_desc_element_lif;
}

uint32
  SOC_PB_PP_L2_LIF_AC_KEY_QUALIFIER_verify(
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_KEY_QUALIFIER *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_LLP_PARSE_INFO, &info->pkt_parse_info, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->port_profile, SOC_PB_PP_LIF_PORT_PROFILE_NDX_MAX, SOC_PB_PP_FRWRD_MACT_PORT_PROFILE_NDX_OUT_OF_RANGE_ERR, 20, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_L2_LIF_AC_KEY_QUALIFIER_verify()",0,0);
}

uint32
  SOC_PB_PP_L2_LIF_IN_VC_RANGE_verify(
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_IN_VC_RANGE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->first_label, SOC_SAND_PP_MPLS_LABEL_MAX, SOC_SAND_PP_MPLS_LABEL_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->last_label, SOC_SAND_PP_MPLS_LABEL_MAX, SOC_SAND_PP_MPLS_LABEL_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_L2_LIF_IN_VC_RANGE_verify()",0,0);
}

uint32
  SOC_PB_PP_L2_LIF_DEFAULT_FORWARDING_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_DEFAULT_FORWARDING_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->default_frwd_type, SOC_PB_PP_LIF_DEFAULT_FRWD_TYPE_MAX, SOC_PB_PP_LIF_DEFAULT_FRWD_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_FRWRD_DECISION_INFO, &(info->default_forwarding), 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_L2_LIF_DEFAULT_FORWARDING_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_L2_LIF_PWE_LEARN_RECORD_verify(
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_PWE_LEARN_RECORD *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  if (info->enable_learning)
  {
    SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_FRWRD_DECISION_INFO, &(info->learn_info), 11, exit);
  }

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_L2_LIF_PWE_LEARN_RECORD_verify()",0,0);
}

uint32
  SOC_PB_PP_L2_LIF_PWE_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_PWE_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->vsid, SOC_PB_PP_VSI_ID_MAX, SOC_PB_PP_VSI_ID_OUT_OF_RANGE_ERR, 10, exit);
  
  if (info->vsi_assignment_mode || info->lif_profile || info->protection_pointer || info->oam_valid || 
     info->term_profile || info->action_profile || info->default_forward_profile || (info->model != SOC_SAND_PP_NOF_MPLS_TUNNEL_MODELS) || 
     info->protection_pass_value || !(info->use_lif)) {
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_LIF_PWE_INFO_INVALID_FIELDS_ERR, 40, exit);
  }

  if (info->service_type  == SOC_PB_PP_L2_LIF_PWE_SERVICE_TYPE_MP)
  {
    if (info->default_frwrd.default_frwd_type != SOC_PB_PP_L2_LIF_DFLT_FRWRD_SRC_VSI)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_LIF_PWE_MP_DEFAULT_FRWRD_OUT_OF_RANGE_ERR, 15, exit);
    }
  }
  else
  {
    if (info->default_frwrd.default_frwd_type != SOC_PB_PP_L2_LIF_DFLT_FRWRD_SRC_LIF)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_LIF_PWE_P2P_DEFAULT_FRWRD_OUT_OF_RANGE_ERR, 16, exit);
    }
    if (info->orientation != SOC_SAND_PP_HUB_SPOKE_ORIENTATION_SPOKE)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_LIF_PWE_P2P_ORIENTATION_OUT_OF_RANGE_ERR, 17, exit);
    }
  }
  if (info->service_type == SOC_PB_PP_L2_LIF_PWE_SERVICE_TYPE_P2P)
  {
    SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_L2_LIF_DEFAULT_FORWARDING_INFO, &(info->default_frwrd), 11, exit);
  }
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_L2_LIF_PWE_LEARN_RECORD, &(info->learn_record), 12, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->orientation, SOC_PB_PP_LIF_ORIENTATION_MAX, SOC_PB_PP_LIF_ORIENTATION_OUT_OF_RANGE_ERR, 12, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->service_type, SOC_PB_PP_LIF_PWE_SERVICE_TYPE_MAX, SOC_PB_PP_LIF_SERVICE_TYPE_OUT_OF_RANGE_ERR, 17, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->tpid_profile_index, SOC_PB_PP_LIF_TPID_PROFILE_INDEX_MAX, SOC_PB_PP_LIF_TPID_PROFILE_INDEX_OUT_OF_RANGE_ERR, 13, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->cos_profile, SOC_PB_PP_LIF_PWE_COS_PROFILE_MAX, SOC_PB_PP_LIF_PWE_COS_PROFILE_OUT_OF_RANGE_ERR, 14, exit);

  if (info->service_type == SOC_PB_PP_L2_LIF_PWE_SERVICE_TYPE_P2P)
  {
    if (info->vsid != SOC_PB_PP_VSI_P2P_SERVICE)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_LIF_VSI_IS_NOT_P2P_ERR, 11, exit);
    }
  }
  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_L2_LIF_PWE_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_L2_VLAN_RANGE_KEY_verify(
    SOC_SAND_IN  SOC_PB_PP_L2_VLAN_RANGE_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->local_port_ndx, SOC_PB_PP_PORT_MAX, SOC_PB_PP_PORT_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->first_vid, SOC_SAND_PP_VLAN_ID_MAX, SOC_SAND_PP_VLAN_ID_OUT_OF_RANGE_ERR, 12, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->last_vid, SOC_SAND_PP_VLAN_ID_MAX, SOC_SAND_PP_VLAN_ID_OUT_OF_RANGE_ERR, 13, exit);
  if(info->first_vid > info->last_vid)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_LIF_VLAN_RANGE_FIRST_OVER_LAST_ERR, 20, exit);
  }

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_L2_VLAN_RANGE_KEY_verify()",0,0);
}

uint32
  SOC_PB_PP_L2_LIF_AC_KEY_verify(
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->vlan_domain, SOC_PB_PP_LIF_VLAN_DOMAIN_MAX, SOC_PB_PP_LIF_VLAN_DOMAIN_OUT_OF_RANGE_ERR, 10, exit);

  if (info->outer_vid != SOC_PB_PP_LIF_IGNORE_OUTER_VID)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(info->outer_vid, SOC_SAND_PP_VLAN_ID_MAX, SOC_SAND_PP_VLAN_ID_OUT_OF_RANGE_ERR, 11, exit);
  }

  if (info->inner_vid != SOC_PB_PP_LIF_IGNORE_INNER_VID)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(info->inner_vid, SOC_SAND_PP_VLAN_ID_MAX, SOC_SAND_PP_VLAN_ID_OUT_OF_RANGE_ERR, 12, exit);
  }

  SOC_SAND_ERR_IF_ABOVE_MAX(info->raw_key, SOC_PB_PP_LIF_RAW_KEY_MAX, SOC_PB_PP_LIF_RAW_KEY_OUT_OF_RANGE_ERR, 14, exit);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_L2_LIF_AC_KEY_verify()",0,0);
}

uint32
  SOC_PB_PP_L2_LIF_AC_LEARN_RECORD_verify(
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_LEARN_RECORD *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->learn_type, SOC_PB_PP_LIF_LEARN_TYPE_MAX, SOC_PB_PP_LIF_LEARN_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  if (!((info->learn_type == SOC_PB_PP_L2_LIF_AC_LEARN_SYS_PORT) || (info->learn_type == SOC_PB_PP_L2_LIF_AC_LEARN_DISABLE)))
  {
    SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_FRWRD_DECISION_INFO, &(info->learn_info), 11, exit);
  }

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_L2_LIF_AC_LEARN_RECORD_verify()",0,0);
}

uint32
  SOC_PB_PP_L2_LIF_AC_ING_EDIT_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_ING_EDIT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->ing_vlan_edit_profile, SOC_PB_PP_LIF_ING_VLAN_EDIT_PROFILE_MAX, SOC_PB_PP_LIF_ING_VLAN_EDIT_PROFILE_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->vid, SOC_SAND_PP_VLAN_ID_MAX, SOC_SAND_PP_VLAN_ID_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->edit_pcp_profile, SOC_PB_PP_LIF_EDIT_PCP_PROFILE_MAX, SOC_PB_PP_LIF_EDIT_PCP_PROFILE_OUT_OF_RANGE_ERR, 12, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_L2_LIF_AC_ING_EDIT_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_L2_LIF_AC_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->service_type, SOC_PB_PP_LIF_AC_SERVICE_TYPE_MAX, SOC_PB_PP_LIF_SERVICE_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->vsid, SOC_PB_PP_VSI_ID_MAX, SOC_PB_PP_VSI_ID_OUT_OF_RANGE_ERR, 11, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_L2_LIF_AC_LEARN_RECORD, &(info->learn_record), 12, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_L2_LIF_AC_ING_EDIT_INFO, &(info->ing_edit_info), 13, exit);
  if (info->default_frwrd.default_frwd_type != SOC_PB_PP_L2_LIF_DFLT_FRWRD_SRC_VSI)
  {
    if (info->service_type != SOC_PB_PP_L2_LIF_AC_SERVICE_TYPE_MP)
    {
      SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_L2_LIF_DEFAULT_FORWARDING_INFO, &(info->default_frwrd), 14, exit);
    }
  }
  if (info->service_type == SOC_PB_PP_L2_LIF_AC_SERVICE_TYPE_MP)
  {
    if (info->default_frwrd.default_frwd_type != SOC_PB_PP_L2_LIF_DFLT_FRWRD_SRC_VSI)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_LIF_AC_MP_DEFAULT_FRWRD_OUT_OF_RANGE_ERR, 15, exit);
    }
  }
  else
  {
    if (info->default_frwrd.default_frwd_type != SOC_PB_PP_L2_LIF_DFLT_FRWRD_SRC_LIF)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_LIF_AC_P2P_DEFAULT_FRWRD_OUT_OF_RANGE_ERR, 16, exit);
    }
    if (info->orientation != SOC_SAND_PP_HUB_SPOKE_ORIENTATION_SPOKE)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_LIF_AC_P2P_ORIENTATION_OUT_OF_RANGE_ERR, 16, exit);
    }
  }
  SOC_SAND_ERR_IF_ABOVE_MAX(info->orientation, SOC_PB_PP_LIF_ORIENTATION_MAX, SOC_PB_PP_LIF_ORIENTATION_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->cos_profile, SOC_PB_PP_LIF_COS_PROFILE_MAX, SOC_PB_PP_LIF_COS_PROFILE_OUT_OF_RANGE_ERR, 21, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->l2cp_profile, SOC_PB_PP_LIF_L2CP_PROFILE_MAX, SOC_PB_PP_LIF_L2CP_PROFILE_OUT_OF_RANGE_ERR, 22, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_L2_LIF_AC_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_L2_LIF_AC_MP_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_MP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->mp_level, SOC_PB_PP_MP_LEVEL_MAX, SOC_PB_PP_MP_LEVEL_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_L2_LIF_AC_MP_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_L2_LIF_L2CP_KEY_verify(
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_L2CP_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->l2cp_profile, SOC_PB_PP_LIF_L2CP_PROFILE_MAX, SOC_PB_PP_LIF_L2CP_PROFILE_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->da_mac_address_lsb, SOC_PB_PP_LIF_DA_MAC_ADDRESS_LSB_MAX, SOC_PB_PP_LIF_DA_MAC_ADDRESS_LSB_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_L2_LIF_L2CP_KEY_verify()",0,0);
}

uint32
  SOC_PB_PP_L2_LIF_AC_GROUP_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_GROUP_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind = 0; ind < info->nof_lif_entries; ++ind)
  {
    SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_L2_LIF_AC_INFO, &(info->acs_info[ind]), 10, exit);
  }
  SOC_SAND_ERR_IF_ABOVE_MAX(info->opcode_id, SOC_PB_PP_LIF_OPCODE_ID_MAX, SOC_PB_PP_LIF_OPCODE_ID_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->nof_lif_entries, SOC_PB_PP_LIF_NOF_LIF_ENTRIES_MAX, SOC_PB_PP_LIF_NOF_LIF_ENTRIES_OUT_OF_RANGE_ERR, 12, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_L2_LIF_AC_GROUP_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_L2_LIF_ISID_KEY_verify(
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_ISID_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->isid_id, SOC_SAND_PP_ISID_MAX, SOC_SAND_PP_ISID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->isid_domain, SOC_PB_PP_LIF_ISID_DOMAIN_MAX, SOC_PB_PP_LIF_ISID_DOMAIN_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_L2_LIF_ISID_KEY_verify()",0,0);
}

uint32
  SOC_PB_PP_L2_LIF_ISID_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_ISID_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->service_type, SOC_PB_PP_LIF_ISID_SERVICE_TYPE_MAX, SOC_PB_PP_LIF_SERVICE_TYPE_OUT_OF_RANGE_ERR, 14, exit);
  if (info->service_type == SOC_PB_PP_L2_LIF_ISID_SERVICE_TYPE_MP) 
  {
      SOC_SAND_ERR_IF_ABOVE_MAX(info->orientation, SOC_PB_PP_LIF_ORIENTATION_MAX, SOC_PB_PP_LIF_ORIENTATION_OUT_OF_RANGE_ERR, 12, exit);
  }
  else { /* P2P */
      SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_L2_LIF_DEFAULT_FORWARDING_INFO, &(info->default_frwrd), 10, exit);
  }
  SOC_SAND_ERR_IF_ABOVE_MAX(info->tpid_profile_index, SOC_PB_PP_LIF_TPID_PROFILE_INDEX_MAX, SOC_PB_PP_LIF_TPID_PROFILE_INDEX_OUT_OF_RANGE_ERR, 13, exit); 
  SOC_SAND_ERR_IF_ABOVE_MAX(info->cos_profile, SOC_PB_PP_LIF_COS_PROFILE_MAX, SOC_PB_PP_LIF_COS_PROFILE_OUT_OF_RANGE_ERR, 13, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_L2_LIF_ISID_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_L2_LIF_TRILL_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_TRILL_INFO *info
  )
{
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->learn_info.dest_id, SOC_PB_PP_FEC_ID_MAX, SOC_PB_PP_FEC_ID_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_L2_LIF_TRILL_INFO_verify()",0,0);
}
/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

