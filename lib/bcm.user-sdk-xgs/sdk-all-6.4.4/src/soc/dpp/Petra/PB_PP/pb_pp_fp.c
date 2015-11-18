/* $Id: pb_pp_fp.c,v 1.19 Broadcom SDK $
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

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Utils/sand_bitstream.h>

#include <soc/dpp/Petra/PB_PP/pb_pp_framework.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_fp.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_fp_key.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_fp_egr.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_sw_db.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_diag.h>

#include <soc/dpp/Petra/PB_TM/pb_pmf_low_level_ce.h>
#include <soc/dpp/Petra/PB_TM/pb_pmf_low_level_db.h>
#include <soc/dpp/Petra/PB_TM/pb_pmf_low_level_pgm.h>
#include <soc/dpp/Petra/PB_TM/pb_pmf_low_level_fem_tag.h>
#include <soc/dpp/Petra/PB_TM/pb_pmf_low_level_diag.h>
#include <soc/dpp/Petra/PB_TM/pb_sw_db_tcam_mgmt.h>
#include <soc/dpp/Petra/PB_TM/pb_tbl_access.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_chip_regs.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_reg_access.h>
#include <soc/dpp/Petra/PB_TM/pb_egr_acl.h>
#include <soc/dpp/Petra/PB_TM/pb_flow_control.h>

#include <soc/dpp/Petra/petra_sw_db.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PB_PP_FP_PFG_NDX_MAX                                   (SOC_PB_PP_FP_NOF_PFGS-1)
#define SOC_PB_PP_FP_DB_ID_NDX_MAX                                 (SOC_PB_PP_FP_NOF_DBS-1)
#define SOC_PB_PP_FP_ENTRY_ID_NDX_MAX                              (SOC_PB_PP_FP_NOF_ENTRY_IDS-1)
#define SOC_PB_PP_FP_FWD_TYPE_NDX_MAX                              (SOC_PB_PP_NOF_FP_FWD_TYPES-1)
#define SOC_PB_PP_FP_PORT_PROFILE_NDX_MAX                          (3)
#define SOC_PB_PP_FP_HDR_FORMAT_BMP_MAX                            (SOC_SAND_U32_MAX)
#define SOC_PB_PP_FP_VLAN_TAG_STRUCTURE_BMP_MAX                    (SOC_SAND_U32_MAX)
#define SOC_PB_PP_FP_SUPPORTED_PFGS_MIN                            (1)
#define SOC_PB_PP_FP_SUPPORTED_PFGS_MAX                            (SOC_SAND_U32_MAX)
#define SOC_PB_PP_FP_QUAL_TYPES_MAX                                (SOC_PB_PP_NOF_FP_QUAL_TYPES-1)
#define SOC_PB_PP_FP_ACTION_TYPES_MAX                              (SOC_PB_PP_NOF_FP_ACTION_TYPES-1)
#define SOC_PB_PP_FP_DATABASE_TYPE_MAX                             (SOC_PB_PP_NOF_FP_DATABASE_TYPES-1)
#define SOC_PB_PP_FP_STRENGTH_MAX                                  (SOC_PB_PP_FP_NOF_DBS-1)
#define SOC_PB_PP_FP_VAL_MAX                                       (SOC_SAND_U32_MAX)
#define SOC_PB_PP_FP_PRIORITY_MAX                                  (SOC_SAND_U16_MAX)
#define SOC_PB_PP_FP_FLD_LSB_MAX                                   (SOC_SAND_U32_MAX)
#define SOC_PB_PP_FP_CST_VAL_MAX                                   (SOC_SAND_U32_MAX)
#define SOC_PB_PP_FP_NOF_FIELDS_MAX                                (SOC_PB_PP_FP_DIR_EXTR_MAX_NOF_FIELDS)
#define SOC_PB_PP_FP_BASE_VAL_MAX                                  (16*1024-1)
#define SOC_PB_PP_FP_DE_ENTRY_ID_NDX_MAX                           (15)
#define SOC_PB_PP_FP_DB_ID_MAX                                     (SOC_PB_PP_FP_NOF_DBS-1)
#define SOC_PB_PP_FP_CONTROL_TYPE_MAX                              (SOC_PB_PP_NOF_FP_CONTROL_TYPES-1)

/* $Id: pb_pp_fp.c,v 1.19 Broadcom SDK $
 * Control set API defines
 */
#define SOC_PB_PP_FP_NOF_L4OPS_RANGES                              (24)
#define SOC_PB_PP_FP_NOF_PKT_SZ_RANGES                             (3)
#define SOC_PB_PP_FP_ETHERTYPE_MAX                                 (0xFFFF)
#define SOC_PB_PP_FP_NEXT_PROTOCOL_IP_MAX                          (255)
#define SOC_PB_PP_FP_EGR_PP_PORT_DATA_MAX                          (63)
#define SOC_PB_PP_FP_EGR_IPV4_NEXT_PROTOCOL_MIN                    (1)
#define SOC_PB_PP_FP_EGR_IPV4_NEXT_PROTOCOL_MAX                    (15)
#define SOC_PB_PP_FP_PP_PORT_PROFILE_MAX                           (3)
#define SOC_PB_PP_NOF_FP_UDP_OFFSET_MAX                            (508)
#define SOC_PB_PP_FP_UDP_NOF_BITS_MIN                              (1)
#define SOC_PB_PP_FP_UDP_NOF_BITS_MAX                              (32)

#define SOC_PB_PP_FP_DB_IPV4_HOST_EXTEND_DB_ID                     (SOC_PB_PP_FP_NOF_DBS - 1)


#define SOC_PB_PP_FP_CONTROL_TYPE_HDR_USER_DEF_SUB_HEADER_NDX      (0)
#define SOC_PB_PP_FP_CONTROL_TYPE_HDR_USER_DEF_OFFSET_NDX          (1)
#define SOC_PB_PP_FP_CONTROL_TYPE_HDR_USER_DEF_NOF_BITS_NDX        (2)
#define SOC_PB_PP_FP_CONTROL_TYPE_HDR_USER_DEF_PARTIAL_QUAL_NDX    (3)

#define SOC_PB_PP_FP_ACTION_TOTAL_LENGTH_IN_BITS_DIRECT_TABLE      (2 * SOC_DPP_TCAM_ACTION_WIDTH_PETRAB)
#define SOC_PB_PP_FP_ACTION_TOTAL_LENGTH_IN_BITS_TCAM              (32)

#define SOC_PB_PP_FP_DB_TYPE_TCAM_KEY_MAX_LENGTH_DIRECT_EXTRACTION (32)
#define SOC_PB_PP_FP_DB_TYPE_TCAM_KEY_MAX_LENGTH_TCAM              (288)

#define SOC_PB_PP_FP_DIRECT_TABLE_ENTRY_LENGTH_IN_BITS             (10)

/* } */
/*************
 * MACROS    *
 *************/
/* { */

/* } */
/*************
 * TYPE DEFS *
 *************/
/* { */

typedef struct
{
  SOC_PB_PP_FP_PREDEFINED_ACL_KEY predefined_key;
  SOC_PB_PP_FP_QUAL_TYPE qual_type;
  uint32 lsb;
  uint32 length;
} SOC_PB_PP_FP_PREDEFINED_KEY_INFO;

/* } */
/*************
 * GLOBALS   *
 *************/
/* { */

static
  SOC_PROCEDURE_DESC_ELEMENT
    Soc_pb_pp_procedure_desc_element_fp[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_PACKET_FORMAT_GROUP_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_PACKET_FORMAT_GROUP_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_PACKET_FORMAT_GROUP_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_PACKET_FORMAT_GROUP_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_PACKET_FORMAT_GROUP_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_PACKET_FORMAT_GROUP_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_PACKET_FORMAT_GROUP_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_PACKET_FORMAT_GROUP_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_DATABASE_CREATE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_DATABASE_CREATE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_DATABASE_CREATE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_DATABASE_CREATE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_DATABASE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_DATABASE_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_DATABASE_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_DATABASE_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_DATABASE_DESTROY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_DATABASE_DESTROY_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_DATABASE_DESTROY_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_DATABASE_DESTROY_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_ENTRY_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_ENTRY_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_ENTRY_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_ENTRY_ADD_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_ENTRY_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_ENTRY_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_ENTRY_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_ENTRY_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_ENTRY_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_ENTRY_REMOVE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_ENTRY_REMOVE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_ENTRY_REMOVE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_DATABASE_ENTRIES_GET_BLOCK),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_DATABASE_ENTRIES_GET_BLOCK_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_DATABASE_ENTRIES_GET_BLOCK_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_DATABASE_ENTRIES_GET_BLOCK_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_DIRECT_EXTRACTION_ENTRY_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_DIRECT_EXTRACTION_ENTRY_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_DIRECT_EXTRACTION_ENTRY_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_DIRECT_EXTRACTION_ENTRY_ADD_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_DIRECT_EXTRACTION_ENTRY_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_DIRECT_EXTRACTION_ENTRY_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_DIRECT_EXTRACTION_ENTRY_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_DIRECT_EXTRACTION_ENTRY_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_DIRECT_EXTRACTION_ENTRY_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_DIRECT_EXTRACTION_ENTRY_REMOVE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_DIRECT_EXTRACTION_ENTRY_REMOVE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_DIRECT_EXTRACTION_ENTRY_REMOVE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_DIRECT_EXTRACTION_DB_ENTRIES_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_DIRECT_EXTRACTION_DB_ENTRIES_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_DIRECT_EXTRACTION_DB_ENTRIES_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_DIRECT_EXTRACTION_DB_ENTRIES_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_CONTROL_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_CONTROL_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_CONTROL_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_CONTROL_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_CONTROL_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_CONTROL_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_CONTROL_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_CONTROL_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_EGR_DB_MAP_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_EGR_DB_MAP_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_EGR_DB_MAP_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_EGR_DB_MAP_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_EGR_DB_MAP_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_EGR_DB_MAP_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_EGR_DB_MAP_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_EGR_DB_MAP_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_PACKET_DIAG_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_PACKET_DIAG_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_PACKET_DIAG_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_PACKET_DIAG_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_GET_ERRS_PTR),
  /*
   * } Auto generated. Do not edit previous section.
   */
   SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_QUAL_TYPES_VERIFY_FIRST_STAGE),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_ACTION_TYPES_VERIFY_FIRST_STAGE),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_ACTION_TYPE_TO_CE_COPY),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_TCAM_CALLBACK),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_ACTION_TYPE_TO_PMF_CONVERT),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_ACTION_TYPE_FROM_PMF_CONVERT),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_ACTION_TYPE_MAX_SIZE_GET),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_DB_TYPES_VERIFY_FIRST_STAGE),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_INIT_UNSAFE),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_FEM_SET),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_FEM_DB_FIND),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_QUAL_TYPE_PREDEFINED_KEY_GET),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_PREDEFINED_KEY_GET),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_KEY_GET),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_QUAL_TYPE_TO_TCAM_TYPE_CONVERT),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_QUAL_TYPE_TO_KEY_FLD_CONVERT),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_PREDEFINED_KEY_SIZE_AND_TYPE_GET),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_ENTRY_VALIDITY_GET),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_QUAL_VAL_ENCODE),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_QUAL_TYPE_PRESET1),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_ACTION_LSB_GET),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_QUAL_LSB_AND_LENGTH_GET),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_CE_KEY_LENGTH_MINIMAL_GET),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_KEY_INPUT_GET),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_ENTRY_NDX_DIRECT_TABLE_GET),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_QUAL_TYPE_AND_LOCAL_LSB_GET),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_INGRESS_QUAL_VERIFY),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_IPV4_HOST_EXTEND_ENABLE),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_CYCLE_FOR_DB_INFO_VERIFY),



  /*
   * Last element. Do no touch.
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

static
  SOC_ERROR_DESC_ELEMENT
    Soc_pb_pp_error_desc_element_fp[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  {
    SOC_PB_PP_FP_PFG_NDX_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FP_PFG_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'pfg_ndx' is out of range. \n\r "
    "The range is: 0 - 4.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FP_DB_ID_NDX_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FP_DB_ID_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'db_id_ndx' is out of range. \n\r "
    "The range is: 0 - 127.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FP_SUCCESS_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FP_SUCCESS_OUT_OF_RANGE_ERR",
    "The parameter 'success' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_NOF_SUCCESS_FAILURES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FP_ENTRY_ID_NDX_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FP_ENTRY_ID_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'entry_id_ndx' is out of range. \n\r "
    "The range is: 0 - 4*1024-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FP_IS_FOUND_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FP_IS_FOUND_OUT_OF_RANGE_ERR",
    "The parameter 'is_found' is out of range. \n\r "
    "The range is: 0 - 4*1024-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FP_NOF_ENTRIES_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FP_NOF_ENTRIES_OUT_OF_RANGE_ERR",
    "The parameter 'nof_entries' is out of range. \n\r "
    "The range is: 0 - 127.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FP_FWD_TYPE_NDX_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FP_FWD_TYPE_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'fwd_type_ndx' is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_FP_FWD_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FP_PORT_PROFILE_NDX_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FP_PORT_PROFILE_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'port_profile_ndx' is out of range. \n\r "
    "The range is: 0 - 3.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FP_DB_ID_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FP_DB_ID_OUT_OF_RANGE_ERR",
    "The parameter 'db_id' is out of range. \n\r "
    "The range is: 0 - 63.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FP_TYPE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FP_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'type' is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_FP_QUAL_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FP_HDR_FORMAT_BMP_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FP_HDR_FORMAT_BMP_OUT_OF_RANGE_ERR",
    "The parameter 'hdr_format_bmp' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FP_VLAN_TAG_STRUCTURE_BMP_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FP_VLAN_TAG_STRUCTURE_BMP_OUT_OF_RANGE_ERR",
    "The parameter 'vlan_tag_structure_bmp' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FP_DB_TYPE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FP_DB_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'db_type' is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_FP_DATABASE_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FP_SUPPORTED_PFGS_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FP_SUPPORTED_PFGS_OUT_OF_RANGE_ERR",
    "The parameter 'supported_pfgs' is out of range. \n\r "
    "The range is: 1 (at least one PFG mnust be set) - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FP_QUAL_TYPES_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FP_QUAL_TYPES_OUT_OF_RANGE_ERR",
    "The parameter 'qual_types' is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_FP_QUAL_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FP_DATABASE_TYPE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FP_DATABASE_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'db_type' is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_FP_DATABASE_TYPES.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FP_ACTION_TYPES_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FP_ACTION_TYPES_OUT_OF_RANGE_ERR",
    "The parameter 'action_types' is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_FP_ACTION_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FP_STRENGTH_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FP_STRENGTH_OUT_OF_RANGE_ERR",
    "The parameter 'strength' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FP_VAL_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FP_VAL_OUT_OF_RANGE_ERR",
    "The parameter 'val' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FP_PRIORITY_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FP_PRIORITY_OUT_OF_RANGE_ERR",
    "The parameter 'priority' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FP_FLD_LSB_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FP_FLD_LSB_OUT_OF_RANGE_ERR",
    "The parameter 'fld_lsb' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FP_CST_VAL_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FP_CST_VAL_OUT_OF_RANGE_ERR",
    "The parameter 'cst_val' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FP_NOF_BITS_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FP_NOF_BITS_OUT_OF_RANGE_ERR",
    "The parameter 'nof_bits' is out of range. \n\r "
    "The range is: 0 - 17.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FP_NOF_FIELDS_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FP_NOF_FIELDS_OUT_OF_RANGE_ERR",
    "The parameter 'nof_fields' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FP_BASE_VAL_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FP_BASE_VAL_OUT_OF_RANGE_ERR",
    "The parameter 'base_val' is out of range. \n\r "
    "The range is: 0 - 16*1024-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FP_VAL_NDX_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FP_VAL_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'val_ndx' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  /*
   * } Auto generated. Do not edit previous section.
   */
  {
    SOC_PB_PP_FP_DB_ID_ALREADY_EXIST_ERR,
    "SOC_PB_PP_FP_DB_ID_ALREADY_EXIST_ERR",
    "The database 'db_id_ndx' is already created. \n\r "
    "Delete it to create another database with the same index.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FP_DB_ID_NOT_DIRECT_EXTRACTION_ERR,
    "SOC_PB_PP_FP_DB_ID_NOT_DIRECT_EXTRACTION_ERR",
    "The entry cannot be added to a database which is not \n\r "
    "of type direct extraction.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FP_DB_ID_NOT_LOOKUP_ERR,
    "SOC_PB_PP_FP_DB_ID_NOT_LOOKUP_ERR",
    "The entry cannot be added to a database which is not \n\r "
    "of type lookup (Direct Table, Egress or TCAM).\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FP_DB_ID_DIRECT_TABLE_ALREADY_EXIST_ERR,
    "SOC_PB_PP_FP_DB_ID_DIRECT_TABLE_ALREADY_EXIST_ERR",
    "The database cannot be inserted since a direct table database \n\r "
    "already exists or the VSQ pointer feature for Flow Control is enabled.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FP_ENTRY_QUAL_TYPE_NOT_IN_DB_ERR,
    "SOC_PB_PP_FP_ENTRY_QUAL_TYPE_NOT_IN_DB_ERR",
    "On of the entry qualifiers is not present \n\r "
    "in the entry database qualifier list.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FP_ENTRY_ACTION_TYPE_NOT_IN_DB_ERR,
    "SOC_PB_PP_FP_ENTRY_ACTION_TYPE_NOT_IN_DB_ERR",
    "One of the entry action types is not present \n\r "
    "in the entry database action type list.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FP_ENTRY_ALREADY_EXIST_ERR,
    "SOC_PB_PP_FP_ENTRY_ALREADY_EXIST_ERR",
    "The Database contains already an entry \n\r "
    "with the same index.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FP_TAG_ACTION_ALREADY_EXIST_ERR,
    "SOC_PB_PP_FP_TAG_ACTION_ALREADY_EXIST_ERR",
    "The Database contains a TAG action that is already \n\r "
    "present for a supported PFG.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FP_QUAL_TYPES_NOT_PREDEFINED_ACL_KEY_ERR,
    "SOC_PB_PP_FP_QUAL_TYPES_NOT_PREDEFINED_ACL_KEY_ERR",
    "The Database has a TCAM Database type but \n\r "
    "its qualifier types does not correspond to a predefined ACL key.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FP_DB_CREATION_FOR_IPV4_HOST_EXTEND_ERR,
    "SOC_PB_PP_FP_DB_CREATION_FOR_IPV4_HOST_EXTEND_ERR",
    "The Database creation for the IPv4 Host Extended \n\r "
    "configuration has failed during the initialization.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FP_EGR_DATABASE_NOT_ALREADY_ADDED_ERR,
    "SOC_PB_PP_FP_EGR_DATABASE_NOT_ALREADY_ADDED_ERR",
    "The mapping between (port profile, packet forwarding type) \n\r "
    "and the egress Database can be done only for egress DB alreay created.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FP_QUAL_TYPES_END_OF_LIST_ERR,
    "SOC_PB_PP_FP_QUAL_TYPES_END_OF_LIST_ERR",
    "The parameter 'qual_types' has reached end of list \n\r "
    "but a non SOC_PB_PP_NOF_FP_QUAL_TYPES is inserted after that.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FP_ACTION_TYPES_END_OF_LIST_ERR,
    "SOC_PB_PP_FP_ACTION_TYPES_END_OF_LIST_ERR",
    "The parameter 'action_types' has reached end of list \n\r "
    "but a non SOC_PB_PP_NOF_FP_ACTION_TYPES is inserted after that.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FP_ACTION_LENGTHS_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FP_ACTION_LENGTHS_OUT_OF_RANGE_ERR",
    "The sum of all the action lengths is higher than  \n\r "
    "the HW admissible length (32b for TCAM, 20b for Direct Table).\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FP_QUALS_LENGTHS_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FP_QUALS_LENGTHS_OUT_OF_RANGE_ERR",
    "The sum of all the qualifiers lengths is higher than  \n\r "
    "the HW admissible length (288b for TCAM, 10b for Direct Table, 32b for DE).\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FP_EGRESS_QUAL_USED_FOR_INGRESS_DB_ERR,
    "SOC_PB_PP_FP_EGRESS_QUAL_USED_FOR_INGRESS_DB_ERR",
    "One of the qualifiers is dedicated to egress  \n\r "
    "databases (ERPP) and is used by error for an ingress DB.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FP_QUAL_NOT_EGRESS_ERR,
    "SOC_PB_PP_FP_QUAL_NOT_EGRESS_ERR",
    "The qualifiers do not correspond to a predefined \n\r "
    "egress Key although the Database type is Egress.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FP_ACTIONS_MIXED_WITH_TAG_ERR,
    "SOC_PB_PP_FP_ACTIONS_MIXED_WITH_TAG_ERR",
    "The actions of a database must be all of type TAG \n\r "
    "or no one must be of type TAG.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },


  /*
   * Last element. Do no touch.
   */
  SOC_ERR_DESC_ELEMENT_DEF_LAST
};

static
  const
    SOC_PB_PP_FP_PREDEFINED_KEY_INFO
      Soc_pb_pp_fp_predefined_key_info[] =
{
  /*
   *  L2 predefined ACL key
   */
  {SOC_PB_PP_FP_PREDEFINED_ACL_KEY_L2,   SOC_PB_PP_FP_QUAL_HDR_VLAN_FORMAT,         138, 4 },
  {SOC_PB_PP_FP_PREDEFINED_ACL_KEY_L2,   SOC_PB_PP_FP_QUAL_HDR_FWD_VLAN_TAG,        122, 16},
  {SOC_PB_PP_FP_PREDEFINED_ACL_KEY_L2,   SOC_PB_PP_FP_QUAL_HDR_FWD_2ND_VLAN_TAG,    106, 16},
  {SOC_PB_PP_FP_PREDEFINED_ACL_KEY_L2,   SOC_PB_PP_FP_QUAL_HDR_FWD_SA,              58,  48},
  {SOC_PB_PP_FP_PREDEFINED_ACL_KEY_L2,   SOC_PB_PP_FP_QUAL_HDR_FWD_DA,              10,  48},
  {SOC_PB_PP_FP_PREDEFINED_ACL_KEY_L2,   SOC_PB_PP_FP_QUAL_HDR_FWD_ETHERTYPE,       6,   4 },
  {SOC_PB_PP_FP_PREDEFINED_ACL_KEY_L2,   SOC_PB_PP_FP_QUAL_IRPP_SRC_PP_PORT,        0,   6 },

  /*
   *  L3 IPv4 predefined ACL key
   */
  {SOC_PB_PP_FP_PREDEFINED_ACL_KEY_IPV4, SOC_PB_PP_FP_QUAL_HDR_IPV4_L4OPS_HI,       136, 7 },
  {SOC_PB_PP_FP_PREDEFINED_ACL_KEY_IPV4, SOC_PB_PP_FP_QUAL_HDR_FWD_IPV4_NEXT_PRTCL, 132, 4 },
  {SOC_PB_PP_FP_PREDEFINED_ACL_KEY_IPV4, SOC_PB_PP_FP_QUAL_HDR_FWD_IPV4_DF,         131, 1 },
  {SOC_PB_PP_FP_PREDEFINED_ACL_KEY_IPV4, SOC_PB_PP_FP_QUAL_HDR_FWD_IPV4_MF,         130, 1 },
  /*{SOC_PB_PP_FP_PREDEFINED_ACL_KEY_IPV4, SOC_PB_PP_FP_QUAL_HDR_IPV4_F_OFFSET_NON0,          129, 1 },*/
  /*{SOC_PB_PP_FP_PREDEFINED_ACL_KEY_IPV4, SOC_PB_PP_FP_QUAL_HDR_IPV4_L4OPS_FLAG,          128, 1 },*/
  {SOC_PB_PP_FP_PREDEFINED_ACL_KEY_IPV4, SOC_PB_PP_FP_QUAL_HDR_FWD_IPV4_SIP,        96,  32},
  {SOC_PB_PP_FP_PREDEFINED_ACL_KEY_IPV4, SOC_PB_PP_FP_QUAL_HDR_FWD_IPV4_DIP,        64,  32},
  {SOC_PB_PP_FP_PREDEFINED_ACL_KEY_IPV4, SOC_PB_PP_FP_QUAL_HDR_IPV4_L4OPS_LOW,      32,  16},
  {SOC_PB_PP_FP_PREDEFINED_ACL_KEY_IPV4, SOC_PB_PP_FP_QUAL_HDR_FWD_IPV4_SRC_PORT,   48,  16},
  {SOC_PB_PP_FP_PREDEFINED_ACL_KEY_IPV4, SOC_PB_PP_FP_QUAL_HDR_FWD_IPV4_DEST_PORT,  32,  16},
  {SOC_PB_PP_FP_PREDEFINED_ACL_KEY_IPV4, SOC_PB_PP_FP_QUAL_HDR_FWD_IPV4_TOS,        24,  8 },
  {SOC_PB_PP_FP_PREDEFINED_ACL_KEY_IPV4, SOC_PB_PP_FP_QUAL_HDR_FWD_IPV4_TCP_CTL,    18,  6 },
  {SOC_PB_PP_FP_PREDEFINED_ACL_KEY_IPV4, SOC_PB_PP_FP_QUAL_IRPP_IN_LIF,             0,   16},
  {SOC_PB_PP_FP_PREDEFINED_ACL_KEY_IPV4, SOC_PB_PP_FP_QUAL_IRPP_SRC_PP_PORT,        12,  6 },
  {SOC_PB_PP_FP_PREDEFINED_ACL_KEY_IPV4, SOC_PB_PP_FP_QUAL_HDR_FWD_IPV4_IN_VID,     0,   12},

  /*
   *  L3 IPv6 predefined ACL key
   */
  {SOC_PB_PP_FP_PREDEFINED_ACL_KEY_IPV6, SOC_PB_PP_FP_QUAL_HDR_IPV6_L4OPS,          280, 8 },
  {SOC_PB_PP_FP_PREDEFINED_ACL_KEY_IPV6, SOC_PB_PP_FP_QUAL_HDR_IPV6_SIP_HIGH,       216, 64},
  {SOC_PB_PP_FP_PREDEFINED_ACL_KEY_IPV6, SOC_PB_PP_FP_QUAL_HDR_IPV6_SIP_LOW,        152, 64},
  {SOC_PB_PP_FP_PREDEFINED_ACL_KEY_IPV6, SOC_PB_PP_FP_QUAL_HDR_IPV6_DIP_HIGH,       88,  64},
  {SOC_PB_PP_FP_PREDEFINED_ACL_KEY_IPV6, SOC_PB_PP_FP_QUAL_HDR_IPV6_DIP_LOW,        24,  64},
  {SOC_PB_PP_FP_PREDEFINED_ACL_KEY_IPV6, SOC_PB_PP_FP_QUAL_HDR_IPV6_NEXT_PRTCL,     20,  4 },
  {SOC_PB_PP_FP_PREDEFINED_ACL_KEY_IPV6, SOC_PB_PP_FP_QUAL_HDR_IPV6_TCP_CTL,        14,  6 },
  {SOC_PB_PP_FP_PREDEFINED_ACL_KEY_IPV6, SOC_PB_PP_FP_QUAL_IRPP_SRC_PP_PORT,        14,  6 },
  {SOC_PB_PP_FP_PREDEFINED_ACL_KEY_IPV6, SOC_PB_PP_FP_QUAL_IRPP_IN_LIF,              0,  14},

  /*
   *  L2 predefined Egress ACL key
   */
  {SOC_PB_PP_FP_PREDEFINED_ACL_KEY_EGR_ETH,   SOC_PB_PP_FP_QUAL_HDR_FWD_ETHERTYPE,       139,  4},
  {SOC_PB_PP_FP_PREDEFINED_ACL_KEY_EGR_ETH,   SOC_PB_PP_FP_QUAL_IRPP_ETH_TAG_FORMAT,     134,  5},
  {SOC_PB_PP_FP_PREDEFINED_ACL_KEY_EGR_ETH,   SOC_PB_PP_FP_QUAL_HDR_FWD_VLAN_TAG,        118, 16},
  {SOC_PB_PP_FP_PREDEFINED_ACL_KEY_EGR_ETH,   SOC_PB_PP_FP_QUAL_HDR_FWD_2ND_VLAN_TAG,    102, 16},
  {SOC_PB_PP_FP_PREDEFINED_ACL_KEY_EGR_ETH,   SOC_PB_PP_FP_QUAL_HDR_FWD_SA,              54,  48},
  {SOC_PB_PP_FP_PREDEFINED_ACL_KEY_EGR_ETH,   SOC_PB_PP_FP_QUAL_HDR_FWD_DA,               6,  48},
  {SOC_PB_PP_FP_PREDEFINED_ACL_KEY_EGR_ETH,   SOC_PB_PP_FP_QUAL_ERPP_PP_PORT_DATA,        0,   6},

  /*
   *  L3 IPv4 predefined Egress ACL key
   */
  {SOC_PB_PP_FP_PREDEFINED_ACL_KEY_EGR_IPV4, SOC_PB_PP_FP_QUAL_HDR_FWD_IPV4_TOS,        86,  8 },
  {SOC_PB_PP_FP_PREDEFINED_ACL_KEY_EGR_IPV4, SOC_PB_PP_FP_QUAL_ERPP_IPV4_NEXT_PROTOCOL, 82,  4 },
  {SOC_PB_PP_FP_PREDEFINED_ACL_KEY_EGR_IPV4, SOC_PB_PP_FP_QUAL_HDR_FWD_IPV4_SIP,        50,  32},
  {SOC_PB_PP_FP_PREDEFINED_ACL_KEY_EGR_IPV4, SOC_PB_PP_FP_QUAL_HDR_FWD_IPV4_DIP,        18,  32},
  {SOC_PB_PP_FP_PREDEFINED_ACL_KEY_EGR_IPV4, SOC_PB_PP_FP_QUAL_HDR_FWD_VLAN_TAG,         6,  12},
  {SOC_PB_PP_FP_PREDEFINED_ACL_KEY_EGR_IPV4, SOC_PB_PP_FP_QUAL_ERPP_PP_PORT_DATA,        0,   6},

  /*
   *  MPLS predefined Egress ACL key
   */
  {SOC_PB_PP_FP_PREDEFINED_ACL_KEY_EGR_MPLS, SOC_PB_PP_FP_QUAL_ERPP_FTMH,               76,  64},
  {SOC_PB_PP_FP_PREDEFINED_ACL_KEY_EGR_MPLS, SOC_PB_PP_FP_QUAL_ERPP_PAYLOAD,            38,  38},
  {SOC_PB_PP_FP_PREDEFINED_ACL_KEY_EGR_MPLS, SOC_PB_PP_FP_QUAL_HDR_MPLS_LABEL_ID_FWD,      18,  20},
  {SOC_PB_PP_FP_PREDEFINED_ACL_KEY_EGR_MPLS, SOC_PB_PP_FP_QUAL_HDR_MPLS_EXP_FWD,        15,   3},
  {SOC_PB_PP_FP_PREDEFINED_ACL_KEY_EGR_MPLS, SOC_PB_PP_FP_QUAL_HDR_MPLS_TTL_FWD,         6,   8},
  {SOC_PB_PP_FP_PREDEFINED_ACL_KEY_EGR_MPLS, SOC_PB_PP_FP_QUAL_ERPP_PP_PORT_DATA,        0,   6},

  /*
   *  TM predefined Egress ACL key
   */
  {SOC_PB_PP_FP_PREDEFINED_ACL_KEY_EGR_TM, SOC_PB_PP_FP_QUAL_ERPP_FTMH,               76,  64},
  {SOC_PB_PP_FP_PREDEFINED_ACL_KEY_EGR_TM, SOC_PB_PP_FP_QUAL_ERPP_PAYLOAD,             6,  70},
  {SOC_PB_PP_FP_PREDEFINED_ACL_KEY_EGR_TM, SOC_PB_PP_FP_QUAL_ERPP_PP_PORT_DATA,        0,   6},

};

static
  const
    uint32
      Soc_pb_pp_fp_predefined_key_info_size =
       sizeof(Soc_pb_pp_fp_predefined_key_info) / sizeof(SOC_PB_PP_FP_PREDEFINED_KEY_INFO);

/* } */
/*************
 * FUNCTIONS *
 *************/
/* { */

/*
 * Callback for the TCAM management
 */
STATIC
  uint32
    soc_pb_pp_fp_tcam_callback(
      SOC_SAND_IN int unit,
      SOC_SAND_IN uint32  user_data
    )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    action_ndx,
    pfg_ndx,
    cycle,
    profile_id,
    tcam_db_id,
    fem_id,
    db_id_fem,
    db_id_ndx,
    lkp_profile;
  uint8
    is_tag_action;
  SOC_PB_PP_FP_DATABASE_INFO
    db_info;
  SOC_PB_TCAM_ACCESS_DEVICE
    access_device;
  SOC_PB_PMF_LKP_PROFILE
    lkp_profile_ndx;
  SOC_PB_PMF_FEM_INPUT_INFO
    fem_input_info;
  SOC_PB_PP_FP_FEM_ENTRY
    fem_entry;
  SOC_PB_PMF_TAG_SRC_INFO
    tag_src;
  SOC_PB_PP_FP_KEY_LOCATION
    key_loc;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_TCAM_CALLBACK);

  SOC_PB_PMF_LKP_PROFILE_clear(&lkp_profile_ndx);

  db_id_ndx  = user_data;
  tcam_db_id = soc_pb_egr_fp_tcam_db_id_get(
                 db_id_ndx
               );
  profile_id = tcam_db_id;
  cycle      = soc_pb_sw_db_tcam_access_profile_cycle_get(
                 unit,
                 profile_id
               );

  res = soc_pb_pp_sw_db_fp_db_info_get(
          unit,
          db_id_ndx,
          &db_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*
   *  Enable access to the database's banks
   */
  res = soc_pb_pmf_tcam_lookup_enable_set_unsafe(
          unit,
          tcam_db_id,
          cycle,
          TRUE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  /*
   *  Configure the Lookup-Profile of every PFG that's supported by the database to search the TCAM
   */
  lkp_profile_ndx.cycle_ndx = cycle;
  for (pfg_ndx = 0; pfg_ndx < SOC_PB_PP_FP_NOF_PFGS; ++pfg_ndx)
  {
    if (db_info.supported_pfgs & SOC_SAND_BIT(pfg_ndx))
    {
      lkp_profile_ndx.id = pfg_ndx;
      res = soc_pb_pp_sw_db_fp_db_key_location_get(
              unit,
              db_id_ndx,
              pfg_ndx,
              &key_loc
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
      res = soc_pb_pmf_tcam_lookup_db_add_unsafe(
              unit,
              &lkp_profile_ndx,
              tcam_db_id,
              key_loc.tcam_src
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
    }
  }

  /*
   *  Since we changed some Lookup-Profiles, we have to update the access device configurations for
   *  ALL Lookup-Profiles (this is a long-standing bug in the TCAM code, we should really fix it).
   */
  for (access_device = 0; access_device < SOC_PB_TCAM_NOF_ACCESS_DEVICES; ++access_device)
  {
    for (cycle = 0; cycle < SOC_PB_TCAM_NOF_CYCLES; ++cycle)
    {
      lkp_profile_ndx.cycle_ndx = cycle;

      profile_id = soc_pb_sw_db_tcam_mgmt_access_profile_in_access_device_get(
                     unit,
                     cycle,
                     access_device
                   );
      if (profile_id != SOC_PB_TCAM_MAX_NOF_LISTS)
      {
        tcam_db_id = soc_pb_sw_db_tcam_access_profile_db_get(
                       unit,
                       profile_id
                     );
        for (lkp_profile = 0; lkp_profile < SOC_PB_PMF_NOF_LKP_PROFILE_IDS; ++lkp_profile)
        {
          lkp_profile_ndx.id = lkp_profile;
          res = soc_pb_pmf_tcam_lookup_result_config_unsafe(
                  unit,
                  &lkp_profile_ndx,
                  tcam_db_id,
                  access_device
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
        }
      }
    }
  }

  for (cycle = 0; cycle < SOC_PB_TCAM_NOF_CYCLES; ++cycle)
  {
    lkp_profile_ndx.cycle_ndx = cycle;
    /*
     *  Update the FEM input if it's attached to a TCAM database
     */
    /*
     * Verify it is tag
     */
    for (db_id_ndx = 0; db_id_ndx < SOC_PB_PP_FP_NOF_DBS; ++db_id_ndx)
    {
      res = soc_pb_pp_sw_db_fp_db_info_get(
              unit,
              db_id_ndx,
              &db_info
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

      res = soc_pb_pp_fp_tag_action_type_convert(
              unit,
              db_info.action_types[0],
              &is_tag_action,
              &action_ndx
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 36, exit);

      if (is_tag_action)
      {
        tcam_db_id = soc_pb_egr_fp_tcam_db_id_get(db_id_ndx);
        res = soc_pb_tcam_access_profile_access_device_get_unsafe(
                unit,
                tcam_db_id,
                &access_device
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 38, exit);
          
        SOC_PB_PMF_TAG_SRC_INFO_clear(&tag_src);
        tag_src.stat_tag_lsb_position = 0;
        if (access_device == SOC_PB_TCAM_ACCESS_DEVICE_SEL4)
        {
          continue; /* No HW support of the fourth selector */
        }
        tag_src.val_src = access_device + SOC_PB_PMF_TAG_VAL_SRC_TCAM_0;

        for (pfg_ndx = 0; pfg_ndx < SOC_PB_PP_FP_NOF_PFGS; ++pfg_ndx)
        {
          if ((db_info.supported_pfgs & (1 << pfg_ndx)) != 0)
          {
            lkp_profile_ndx.id = pfg_ndx;
            res = soc_pb_pmf_db_tag_select_set_unsafe(
                    unit,
                    pfg_ndx, /* Tag profile = PFG */
                    action_ndx,
                    &tag_src
                  );
            SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
          }
        }
      }
    }

    for (fem_id = 0; fem_id < SOC_PB_PMF_LOW_LEVEL_NOF_FEMS; ++fem_id)
    {
      SOC_PB_PP_FP_FEM_ENTRY_clear(&fem_entry);
      res = soc_pb_pp_sw_db_fp_fem_entry_get(
              unit,
              cycle,
              fem_id,
              &fem_entry
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);
      db_id_fem = SOC_PB_PP_FP_NOF_DBS;
      if (fem_entry.action_type[0] != SOC_PB_PP_NOF_FP_ACTION_TYPES)
      {
        db_id_fem = fem_entry.db_id;
      }

      if (db_id_fem != SOC_PB_PP_FP_NOF_DBS)
      {
        res = soc_pb_pp_sw_db_fp_db_info_get(
                unit,
                db_id_fem,
                &db_info
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);
        if (db_info.db_type == SOC_PB_PP_FP_DB_TYPE_TCAM)
        {
          /*
           *  Get the access device for this database
           */
          tcam_db_id = soc_pb_egr_fp_tcam_db_id_get(db_id_fem);
          res = soc_pb_tcam_access_profile_access_device_get_unsafe(
                  unit,
                  tcam_db_id,
                  &access_device
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 47, exit);
          
          SOC_PB_PMF_FEM_INPUT_INFO_clear(&fem_input_info);
          fem_input_info.db_id  = db_id_ndx;
          fem_input_info.pgm_id = SOC_PB_PMF_FEM_PGM_FOR_ETH;
          fem_input_info.src    = SOC_PB_PMF_FEM_INPUT_SRC_TCAM_0 + access_device;
          for (pfg_ndx = 0; pfg_ndx < SOC_PB_PP_FP_NOF_PFGS; ++pfg_ndx)
          {
            if ((db_info.supported_pfgs & (1 << pfg_ndx)) != 0)
            {
              lkp_profile_ndx.id = pfg_ndx;
              res = soc_pb_pmf_db_fem_input_set_unsafe(
                      unit,
                      &lkp_profile_ndx,
                      fem_id,
                      &fem_input_info
                    );
              SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
            }
          }
        }
      }
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_tcam_callback()", 0, 0);
}


uint32
  soc_pb_pp_fp_ipv4_host_extend_enable(
    SOC_SAND_IN  int      unit,
    SOC_SAND_IN  uint32      pfg_ndx
  )
{
  uint32
    res;
  uint32
    entry_id_ndx = 0,
    db_id_ndx = SOC_PB_PP_FP_DB_IPV4_HOST_EXTEND_DB_ID;
  uint8
    ipv4_host_extend_enabled;
  SOC_PB_PP_FP_DATABASE_INFO
    db_info_previous,
    db_info;
  SOC_SAND_SUCCESS_FAILURE
    success;
  SOC_PB_PP_FP_DIR_EXTR_ENTRY_INFO
    *entry_info = NULL;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_IPV4_HOST_EXTEND_ENABLE);

  ipv4_host_extend_enabled = soc_pb_pp_sw_db_ipv4_host_extend_enable_get(unit);

  if (ipv4_host_extend_enabled == TRUE)
  {
    res = soc_pb_pp_fp_database_get_unsafe(
            unit,
            db_id_ndx,
            &db_info_previous
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

    res = soc_pb_pp_fp_database_destroy_unsafe(
            unit,
            db_id_ndx
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 7, exit);

    /*
     *  Create the Database
     */
    SOC_PB_PP_FP_DATABASE_INFO_clear(&db_info);
    db_info.db_type = SOC_PB_PP_FP_DB_TYPE_DIRECT_EXTRACTION;
    db_info.action_types[0] = SOC_PB_PP_FP_ACTION_TYPE_OUTLIF;
    db_info.qual_types[0] = SOC_PB_PP_FP_QUAL_IRPP_LEM_2ND_LKUP_ASD;
    db_info.qual_types[1] = SOC_PB_PP_FP_QUAL_IRPP_FWD_TYPE;
    db_info.strength = 0;
    db_info.supported_pfgs = db_info_previous.supported_pfgs;
    SOC_SAND_SET_BIT(db_info.supported_pfgs, 0x1, pfg_ndx);
    res = soc_pb_pp_fp_database_create_unsafe(
            unit,
            db_id_ndx,
            &db_info,
            &success
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    if (success != SOC_SAND_SUCCESS)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FP_DB_CREATION_FOR_IPV4_HOST_EXTEND_ERR, 20, exit);
    }

    /*
     *  Add an entry
     */
    SOC_PETRA_ALLOC(entry_info, SOC_PB_PP_FP_DIR_EXTR_ENTRY_INFO, 1);
    SOC_PB_PP_FP_DIR_EXTR_ENTRY_INFO_clear(entry_info);
    entry_info->priority = 0;
    entry_info->qual_vals[0].type = SOC_PB_PP_FP_QUAL_IRPP_FWD_TYPE;
    entry_info->qual_vals[0].val.arr[0] = SOC_PB_PP_PKT_FRWRD_TYPE_IPV4_UC;
    entry_info->qual_vals[0].is_valid.arr[0] = 0xF; /* 4 bits */
    entry_info->actions[0].type = SOC_PB_PP_FP_ACTION_TYPE_OUTLIF;
    entry_info->actions[0].base_val = 0;
    entry_info->actions[0].nof_fields = 1;
    entry_info->actions[0].fld_ext[0].type = SOC_PB_PP_FP_QUAL_IRPP_LEM_2ND_LKUP_ASD;
    entry_info->actions[0].fld_ext[0].fld_lsb = 0;
    entry_info->actions[0].fld_ext[0].nof_bits = 16; /* Out-LIF size */
    
    res = soc_pb_pp_fp_direct_extraction_entry_add_unsafe(
            unit,
            db_id_ndx,
            entry_id_ndx,
            entry_info,
            &success
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    if (success != SOC_SAND_SUCCESS)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FP_DB_CREATION_FOR_IPV4_HOST_EXTEND_ERR, 40, exit);
    }
  }

  SOC_PB_PP_DO_NOTHING_AND_EXIT;

exit:
  SOC_PETRA_FREE(entry_info);
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_ipv4_host_extend_enable()", 0, 0);
}

uint32
  soc_pb_pp_fp_init_unsafe(
    SOC_SAND_IN  int      unit
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    egr_acl_ndx,
    pfg_ndx;
  SOC_PB_PP_FP_PFG_INFO
    *pfg_info = NULL;
  SOC_SAND_SUCCESS_FAILURE
    success;
  uint8
    ipv4_host_extend_enabled;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_INIT_UNSAFE);

  ipv4_host_extend_enabled = soc_pb_pp_sw_db_ipv4_host_extend_enable_get(unit);

  if (ipv4_host_extend_enabled == TRUE)
  {
    /*
     * Set the last PFG to support IPv4 packets to support IPv4 Host
     */
    pfg_ndx = SOC_PB_PP_FP_NOF_PFGS - 1;
    SOC_PETRA_ALLOC(pfg_info, SOC_PB_PP_FP_PFG_INFO, 1);
    SOC_PB_PP_FP_PFG_INFO_clear(pfg_info);
    pfg_info->hdr_format_bmp = SOC_PB_PP_FP_PKT_HDR_TYPE_IPV4_ETH;
    pfg_info->vlan_tag_structure_bmp = SOC_SAND_U32_MAX;

    /* Support all the PP-Ports */
    pfg_info->pp_ports_bmp.arr[0] = SOC_SAND_U32_MAX;
    pfg_info->pp_ports_bmp.arr[1] = SOC_SAND_U32_MAX;

    res = soc_pb_pp_fp_packet_format_group_set_unsafe(
            unit,
            pfg_ndx,
            pfg_info,
            &success
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    if (success != SOC_SAND_SUCCESS)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FP_DB_CREATION_FOR_IPV4_HOST_EXTEND_ERR, 15, exit);
    }
  }

  /*
   * Init Egress mapping
   */
  for (egr_acl_ndx = 0; egr_acl_ndx < SOC_PB_PP_FP_NOF_EGR_ACL_DBS; ++egr_acl_ndx)
  {
    soc_pb_sw_db_egr_acl_sw_db_id_set(
      unit,
      egr_acl_ndx,
      SOC_PB_PP_FP_NOF_DBS
    );
  }



exit:
  SOC_PETRA_FREE(pfg_info);
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_init_unsafe()", 0, 0);
}


/*
 * Get if the Qualifier type is in the predefined key
 */
STATIC
  uint32
    soc_pb_pp_fp_qual_type_predefined_key_get(
      SOC_SAND_IN  int            unit,
      SOC_SAND_IN  SOC_PB_PP_FP_QUAL_TYPE   qual_type,
      SOC_SAND_OUT uint8            is_in_key[SOC_PB_PP_NOF_FP_PREDEFINED_ACL_KEYS]
    )
{
  uint32
    ind;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_QUAL_TYPE_PREDEFINED_KEY_GET);

  for (ind = 0; ind < SOC_PB_PP_NOF_FP_PREDEFINED_ACL_KEYS; ++ind)
  {
    is_in_key[ind] = FALSE;
  }

  for (ind = 0; ind < Soc_pb_pp_fp_predefined_key_info_size; ++ind)
  {
    if (Soc_pb_pp_fp_predefined_key_info[ind].qual_type == qual_type)
    {
      is_in_key[Soc_pb_pp_fp_predefined_key_info[ind].predefined_key] = TRUE;
    }
  }

  SOC_PB_PP_DO_NOTHING_AND_EXIT;
    
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_qual_type_predefined_key_get()", 0, 0);
}

  uint32
    soc_pb_pp_fp_qual_predefined_size_get(
      SOC_SAND_IN  int               unit,
      SOC_SAND_IN  SOC_PB_PP_FP_PREDEFINED_ACL_KEY predefined_acl_key,
      SOC_SAND_IN  SOC_PB_PP_FP_QUAL_TYPE   qual_type,
      SOC_SAND_OUT uint8                *is_found,
      SOC_SAND_OUT uint32               *length
    )
{
  uint32
    data_idx;
  uint8
    found;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_QUAL_TYPE_PREDEFINED_KEY_GET);

  found = FALSE;
  *length = 0;
  for (data_idx = 0; (!found) && (data_idx < Soc_pb_pp_fp_predefined_key_info_size); ++data_idx)
  {
    if (Soc_pb_pp_fp_predefined_key_info[data_idx].predefined_key == predefined_acl_key
            && Soc_pb_pp_fp_predefined_key_info[data_idx].qual_type == qual_type)
    {
      *length = Soc_pb_pp_fp_predefined_key_info[data_idx].length;
      found = TRUE;
    }
  }

  *is_found = found;

  SOC_PB_PP_DO_NOTHING_AND_EXIT;
    
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_qual_predefined_size_get()", 0, 0);
}

STATIC
  void
    soc_pb_pp_fp_predefined_key_desc_set(
      SOC_SAND_IN int                   unit,
      SOC_SAND_IN uint32                   db_id_ndx,
      SOC_SAND_IN SOC_PB_PP_FP_QUAL_TYPE          qual_types[SOC_PB_PP_FP_NOF_QUALS_PER_DB_MAX],
      SOC_SAND_IN SOC_PB_PP_FP_PREDEFINED_ACL_KEY predef_key
    )
{
  uint32
    idx,
    data_idx;
  uint8
    found;
  SOC_PB_PP_FP_KEY_DESC
    key_desc;

  SOC_PB_PP_FP_KEY_DESC_clear(&key_desc);

  for (idx = 0; idx < SOC_PB_PP_FP_NOF_QUALS_PER_DB_MAX; ++idx)
  {
    if (qual_types[idx] != SOC_PB_PP_NOF_FP_QUAL_TYPES)
    {
      found = FALSE;
      for (data_idx = 0; !found && data_idx < Soc_pb_pp_fp_predefined_key_info_size; ++data_idx)
      {
        if (Soc_pb_pp_fp_predefined_key_info[data_idx].predefined_key == predef_key
              && Soc_pb_pp_fp_predefined_key_info[data_idx].qual_type == qual_types[idx])
        {
          key_desc.fields[idx].lsb    = Soc_pb_pp_fp_predefined_key_info[data_idx].lsb;
          key_desc.fields[idx].length = Soc_pb_pp_fp_predefined_key_info[data_idx].length;
          found                       = TRUE;
        }
      }
    }
  }
  
  soc_pb_pp_sw_db_fp_key_desc_set(
    unit,
    db_id_ndx,
    &key_desc
  );
}

/*
 * Get which predefined key is selected
 */
STATIC
  uint32
    soc_pb_pp_fp_predefined_key_get(
      SOC_SAND_IN  int            unit,
      SOC_SAND_IN  SOC_PB_PP_FP_QUAL_TYPE   *qual_types,
      SOC_SAND_OUT uint8            is_in_key[SOC_PB_PP_NOF_FP_PREDEFINED_ACL_KEYS]
    )
{
  uint8
    is_predefined_key[SOC_PB_PP_NOF_FP_PREDEFINED_ACL_KEYS],
    qual_type_in_key[SOC_PB_PP_NOF_FP_PREDEFINED_ACL_KEYS];
  uint32
    acl_key_ndx,
    ind;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_PREDEFINED_KEY_GET);

  /*
   * Verify it is a predefined key (L2, L3 IPv4 or IPv6)
   */
  for (acl_key_ndx = 0; acl_key_ndx < SOC_PB_PP_NOF_FP_PREDEFINED_ACL_KEYS; ++acl_key_ndx)
  {
    is_predefined_key[acl_key_ndx] = TRUE;
  }

  for (ind = 0; ind < SOC_PB_PP_FP_NOF_QUALS_PER_DB_MAX; ++ind)
  {
    if (qual_types[ind] == SOC_PB_PP_NOF_FP_QUAL_TYPES)
    {
      break;
    }

    res = soc_pb_pp_fp_qual_type_predefined_key_get(
            unit,
            qual_types[ind],
            qual_type_in_key
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    for (acl_key_ndx = 0; acl_key_ndx < SOC_PB_PP_NOF_FP_PREDEFINED_ACL_KEYS; ++acl_key_ndx)
    {
      if (qual_type_in_key[acl_key_ndx] == FALSE)
      {
        is_predefined_key[acl_key_ndx] = FALSE;
      }
    }
  }

  for (acl_key_ndx = 0; acl_key_ndx < SOC_PB_PP_NOF_FP_PREDEFINED_ACL_KEYS; ++acl_key_ndx)
  {
    is_in_key[acl_key_ndx] = is_predefined_key[acl_key_ndx];
  }

  SOC_PB_PP_DO_NOTHING_AND_EXIT;
    
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_predefined_key_get()", 0, 0);
}

/*
 * Get the predefined key
 */
uint32
  soc_pb_pp_fp_key_get(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  uint8                     is_for_egress,
    SOC_SAND_IN  SOC_PB_PP_FP_QUAL_TYPE           *qual_types,
    SOC_SAND_OUT SOC_PB_PP_FP_PREDEFINED_ACL_KEY  *predefined_acl_key
  )
{
  uint8
    is_predefined_key[SOC_PB_PP_NOF_FP_PREDEFINED_ACL_KEYS];
  uint32
    predefined_key_min,
    predefined_key_max,
    predefined_key_ndx;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_KEY_GET);

  /*
   * Verify it is a ingress predefined key (L2, L3 IPv4 or IPv6)
   * or an egress one
   */
  res = soc_pb_pp_fp_predefined_key_get(
          unit,
          qual_types,
          is_predefined_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  *predefined_acl_key = SOC_PB_PP_NOF_FP_PREDEFINED_ACL_KEYS;

  if (is_for_egress == FALSE)
  {
    predefined_key_min = SOC_PB_PP_FP_PREDEFINED_ACL_KEY_L2;
    predefined_key_max = SOC_PB_PP_FP_PREDEFINED_ACL_KEY_IPV6;
  }
  else
  {
    predefined_key_min = SOC_PB_PP_FP_PREDEFINED_ACL_KEY_EGR_ETH;
    predefined_key_max = SOC_PB_PP_FP_PREDEFINED_ACL_KEY_EGR_MPLS;
  }

  for (predefined_key_ndx = predefined_key_min; predefined_key_ndx <= predefined_key_max; predefined_key_ndx++)
  {
    if (is_predefined_key[predefined_key_ndx] == TRUE)
    {
      *predefined_acl_key = predefined_key_ndx;
      break;
    }
  }

  SOC_PB_PP_DO_NOTHING_AND_EXIT;
    
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_key_get()", 0, 0);
}


/*
 * Verify the validity of the entries
 */
STATIC
  uint32
    soc_pb_pp_fp_entry_validity_get(
      SOC_SAND_IN  int                     unit,
      SOC_SAND_IN  SOC_PB_PP_FP_DATABASE_INFO        *fp_database_info,
      SOC_SAND_IN  SOC_PB_PP_FP_QUAL_VAL             qual_vals[SOC_PB_PP_FP_NOF_QUALS_PER_DB_MAX],
      SOC_SAND_IN  SOC_PB_PP_FP_ACTION_TYPE          action_types[SOC_PB_PP_FP_NOF_ACTIONS_PER_DB_MAX]
    )
{
  uint32
    res;
  uint32
    action_type_ndx,
    action_type_ndx2,
    qual_type_ndx,
    qual_type_ndx2;
  uint8
    is_type_found;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_ENTRY_VALIDITY_GET);

  for (qual_type_ndx = 0; qual_type_ndx < SOC_PB_PP_FP_NOF_QUALS_PER_DB_MAX; ++qual_type_ndx)
  {
    if (qual_vals[qual_type_ndx].type != SOC_PB_PP_NOF_FP_QUAL_TYPES)
    {
      is_type_found = FALSE;
      for (qual_type_ndx2 = 0; (qual_type_ndx2 < SOC_PB_PP_FP_NOF_QUALS_PER_DB_MAX) && (is_type_found == FALSE); ++qual_type_ndx2)
      {
        if (qual_vals[qual_type_ndx].type == fp_database_info->qual_types[qual_type_ndx2])
        {
          is_type_found = TRUE;
        }
      }
      if (is_type_found == FALSE)
      {
        SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FP_ENTRY_QUAL_TYPE_NOT_IN_DB_ERR, 50, exit);
      }
    }
  }

  /*
   *	Verify the action types and qualifiers types exist in the Database
   */
  for (action_type_ndx = 0; action_type_ndx < SOC_PB_PP_FP_NOF_ACTIONS_PER_DB_MAX; ++action_type_ndx)
  {
    if (action_types[action_type_ndx] != SOC_PB_PP_NOF_FP_ACTION_TYPES)
    {
      is_type_found = FALSE;
      for (action_type_ndx2 = 0; (action_type_ndx2 < SOC_PB_PP_NOF_FP_ACTION_TYPES) && (is_type_found == FALSE); ++action_type_ndx2)
      {
        if (action_types[action_type_ndx] == fp_database_info->action_types[action_type_ndx2])
        {
          is_type_found = TRUE;
        }
      }
      if (is_type_found == FALSE)
      {
        SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FP_ENTRY_ACTION_TYPE_NOT_IN_DB_ERR, 70, exit);
      }
    }
  }

  SOC_PB_PP_DO_NOTHING_AND_EXIT;
    
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_entry_validity_get()", 0, 0);
}

/*********************************************************************
*     Set a Packet Format Group (PFG). The packet format group
 *     defines the supported Packet formats. The user must
 *     indicate for each Database which Packet format(s) are
 *     associated with this Database. E.g.: A Packet Format
 *     Group including only IPv6 packets can be defined to use
 *     Databases with IPv6 Destination-IP qualifiers.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_fp_packet_format_group_set_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    pfg_ndx,
    SOC_SAND_IN  SOC_PB_PP_FP_PFG_INFO            *info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE         *success
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PMF_PGM_MGMT_NDX
    pgm_ndx;
  SOC_PB_PMF_PGM_MGMT_PARAMS
    *pgm_params = NULL;
  uint32
    qual_ndx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_PACKET_FORMAT_GROUP_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  /*
   * Use the generic new element insertion function
   */
  SOC_PB_PMF_PGM_MGMT_NDX_clear(&pgm_ndx);
  SOC_PETRA_ALLOC(pgm_params, SOC_PB_PMF_PGM_MGMT_PARAMS, 1);
  SOC_PB_PMF_PGM_MGMT_PARAMS_clear(pgm_params);
  pgm_ndx.pfg_ndx = pfg_ndx;

  if (!info->is_array_qualifier) {
      /* Simple copy in case of not-array */
      SOC_PETRA_COPY(&(pgm_params->pfg_info), info, SOC_PB_PP_FP_PFG_INFO, 1);
  }
  else {
      /* Fill it according to the Parameters */
      for (qual_ndx = 0; qual_ndx < SOC_PPC_FP_NOF_QUALS_PER_PFG_MAX; qual_ndx++)
      {
        if (info->qual_vals[qual_ndx].type == SOC_PB_PP_NOF_FP_QUAL_TYPES)
        {
          continue;
        }

        switch (info->qual_vals[qual_ndx].type) {
        case SOC_PPC_FP_QUAL_IRPP_PKT_HDR_TYPE:
            pgm_params->pfg_info.hdr_format_bmp = info->qual_vals[qual_ndx].val.arr[0];
            break;
        case SOC_PPC_FP_QUAL_IRPP_ETH_TAG_FORMAT:
            pgm_params->pfg_info.vlan_tag_structure_bmp = info->qual_vals[qual_ndx].val.arr[0];
            break;
        case SOC_PPC_FP_QUAL_IRPP_IN_PORT_BITMAP:
            pgm_params->pfg_info.pp_ports_bmp.arr[0] = info->qual_vals[qual_ndx].val.arr[0];
            pgm_params->pfg_info.pp_ports_bmp.arr[1] = info->qual_vals[qual_ndx].val.arr[1];
            break;
        default:
            SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FP_ENTRY_QUAL_TYPE_NOT_IN_DB_ERR, 15, exit);
        }
      }
  }

  SOC_PB_PORT_PP_PORT_INFO_clear(&(pgm_params->pp_port));
  res = soc_pb_pmf_pgm_mgmt_insert_unsafe(
          unit,
          SOC_PB_PMF_PGM_MGMT_SOURCE_FP,
          &pgm_ndx,
          pgm_params,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if ((info->hdr_format_bmp & SOC_PB_PP_FP_PKT_HDR_TYPE_IPV4_ETH) != 0)
  {
    res = soc_pb_pp_fp_ipv4_host_extend_enable(
            unit,
            pfg_ndx
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }


exit:
  SOC_PETRA_FREE(pgm_params);
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_packet_format_group_set_unsafe()", pfg_ndx, 0);
}

uint32
  soc_pb_pp_fp_packet_format_group_set_verify(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    pfg_ndx,
    SOC_SAND_IN  SOC_PB_PP_FP_PFG_INFO            *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_PACKET_FORMAT_GROUP_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(pfg_ndx, SOC_PB_PP_FP_PFG_NDX_MAX, SOC_PB_PP_FP_PFG_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_FP_PFG_INFO, info, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_packet_format_group_set_verify()", pfg_ndx, 0);
}

uint32
  soc_pb_pp_fp_packet_format_group_get_verify(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    pfg_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_PACKET_FORMAT_GROUP_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(pfg_ndx, SOC_PB_PP_FP_PFG_NDX_MAX, SOC_PB_PP_FP_PFG_NDX_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_packet_format_group_get_verify()", pfg_ndx, 0);
}

/*********************************************************************
*     Set a Packet Format Group (PFG). The packet format group
 *     defines the supported Packet formats. The user must
 *     indicate for each Database which Packet format(s) are
 *     associated with this Database. E.g.: A Packet Format
 *     Group including only IPv6 packets can be defined to use
 *     Databases with IPv6 Destination-IP qualifiers.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_fp_packet_format_group_get_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    pfg_ndx,
    SOC_SAND_OUT SOC_PB_PP_FP_PFG_INFO            *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_PACKET_FORMAT_GROUP_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PB_PP_FP_PFG_INFO_clear(info);

  /*
   * The SW DB indicates the current PFG
   */
  soc_pb_sw_db_pfg_info_get(
    unit,
    pfg_ndx,
    info
  );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_packet_format_group_get_unsafe()", pfg_ndx, 0);
}

uint32
  soc_pb_pp_fp_ce_key_length_minimal_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PB_PP_FP_DATABASE_INFO       *info,
    SOC_SAND_OUT uint32                     *length_min
  )
{
  uint32
    total_length,
    res;
  uint32
    qual_length,
    qual_ndx;
  SOC_PB_PP_FP_PREDEFINED_ACL_KEY
    predefined_acl_key;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_CE_KEY_LENGTH_MINIMAL_GET);

  SOC_SAND_CHECK_NULL_INPUT(info);

  total_length = 0;

  if (info->db_type == SOC_PB_PP_FP_DB_TYPE_TCAM)
  {
    /*
     *  Check whether we can use a predefined key
     */
    res = soc_pb_pp_fp_key_get(
            unit,
            FALSE,
            info->qual_types,
            &predefined_acl_key
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  
    if (predefined_acl_key == SOC_PB_PP_FP_PREDEFINED_ACL_KEY_L2)
    {
      total_length = 142;
    }
    else if (predefined_acl_key == SOC_PB_PP_FP_PREDEFINED_ACL_KEY_IPV4)
    {
      total_length = 143;
    }
    else if (predefined_acl_key == SOC_PB_PP_FP_PREDEFINED_ACL_KEY_IPV6)
    {
      total_length = 288;
    }
  }
  
  if (total_length == 0)
  {
    for (qual_ndx = 0; qual_ndx < SOC_PB_PP_FP_NOF_QUALS_PER_DB_MAX; qual_ndx++)
    {
      if (info->qual_types[qual_ndx] == SOC_PB_PP_NOF_FP_QUAL_TYPES)
      {
        continue;
      }

      res = soc_pb_pp_fp_key_length_get_unsafe(
              unit,
              info->qual_types[qual_ndx],
              TRUE, /* with padding */
              &qual_length
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

      total_length += qual_length;
    }
  }

  *length_min = total_length;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_ce_key_length_minimal_get()", 0, 0);
}

uint32
  soc_pb_pp_fp_ingress_qual_verify(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PB_PP_FP_DATABASE_INFO       *info
  )
{
  uint32
    res;
  uint32
    qual_ndx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_INGRESS_QUAL_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);

  for (qual_ndx = 0; qual_ndx < SOC_PB_PP_FP_NOF_QUALS_PER_DB_MAX; qual_ndx++)
  {
    if (
        (info->qual_types[qual_ndx] == SOC_PB_PP_FP_QUAL_ERPP_PP_PORT_DATA)
        || (info->qual_types[qual_ndx] == SOC_PB_PP_FP_QUAL_ERPP_IPV4_NEXT_PROTOCOL)
        || (info->qual_types[qual_ndx] == SOC_PB_PP_FP_QUAL_ERPP_FTMH)
        || (info->qual_types[qual_ndx] == SOC_PB_PP_FP_QUAL_ERPP_PAYLOAD)
       )
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FP_EGRESS_QUAL_USED_FOR_INGRESS_DB_ERR, 10 + qual_ndx, exit);
    }
  }

  SOC_PB_PP_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_ingress_qual_verify()", 0, 0);
}

/*
 * Verify if this cycle is acceptable for this Database info
 */
STATIC
  uint32
    soc_pb_pp_fp_cycle_for_db_info_verify(
      SOC_SAND_IN  int                unit,
      SOC_SAND_IN  SOC_PB_PP_FP_DATABASE_INFO  *info,
      SOC_SAND_IN  uint32                cycle_ndx,
      SOC_SAND_OUT uint8               *is_acceptable
    )
{
  uint32
    qual_type_ndx,
    action_ndx;
  uint32
    res = SOC_SAND_OK;
  uint8
    is_acceptable_lcl,
    is_tag_action;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_CYCLE_FOR_DB_INFO_VERIFY);

  is_acceptable_lcl = TRUE;
  
  /*
   * Verify it is tag
   */
  res = soc_pb_pp_fp_tag_action_type_convert(
          unit,
          info->action_types[0],
          &is_tag_action,
          &action_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if ((is_tag_action == TRUE) && (cycle_ndx == 0))
  {
    /*
     * In case of Tag, use cycle 1
     */
    is_acceptable_lcl = FALSE;
  }

  /*
   * Verify if one of the actions is Change Key in cycle 1
   */
  if (cycle_ndx == 1)
  {
    for (action_ndx = 0; action_ndx < SOC_PB_PP_FP_NOF_ACTIONS_PER_DB_MAX; action_ndx ++)
    {
      if (info->action_types[action_ndx] == SOC_PB_PP_FP_ACTION_TYPE_CHANGE_KEY)
      {
        is_acceptable_lcl = FALSE;
        break;
      }
    }
  }

  /*
   * Verify if one of the qualifiers is Key Changed
   */
  if (cycle_ndx == 0)
  {
    for (qual_type_ndx = 0; qual_type_ndx < SOC_PB_PP_FP_NOF_QUALS_PER_DB_MAX; ++qual_type_ndx)
    {
      if (info->qual_types[qual_type_ndx] == SOC_PB_PP_FP_QUAL_IRPP_KEY_CHANGED)
      {
        is_acceptable_lcl = FALSE;
        break;
      }
    }
  }

  *is_acceptable = is_acceptable_lcl;

  SOC_PB_PP_DO_NOTHING_AND_EXIT;
    
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_cycle_for_db_info_verify()", 0, 0);
}

/*********************************************************************
*     Create a database. Each database specifies the action
 *     types to perform and the qualifier fields for this
 *     Database. Entries in the database specify the specific
 *     actions to be taken upon specific values of the
 *     packet. E.g.: Policy Based Routing database update the
 *     FEC value according to DSCP DIP and In-RIF. An entry in
 *     the database may set the FEC of a packet with DIP
 *     1.2.2.3, DSCP value 7 and In-RIF 3 to be 9.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_fp_database_create_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_IN  SOC_PB_PP_FP_DATABASE_INFO                    *info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  )
{
  uint32
    db_key_length_min,
    entry_size_in_b,
    prefix_length = 0,
    tcam_db_id,
    res = SOC_SAND_OK;
  SOC_PB_PP_FP_DATABASE_INFO
    fp_database_info;
  SOC_PB_PP_FP_FEM_ENTRY
    fem_entry;
  SOC_PB_PP_FP_FEM_CYCLE
    fem_cycle;
  uint32
    pfg_ndx,
    cycle_ndx,
    cycle_ndx_fin = 0,
    action_ndx;
  uint8
    is_acceptable_cycle =0,
    is_tag_action,
    admissible_cycle;
  SOC_PB_PP_FP_DIR_EXTR_ENTRY_INFO
    *dir_extr_entry_info = NULL;
  SOC_SAND_SUCCESS_FAILURE
    success_ce = SOC_SAND_FAILURE_OUT_OF_RESOURCES,
    success_tcam,
    success_fem;
  SOC_PB_TCAM_BANK_ENTRY_SIZE
    entry_size,
    entry_size_fin = 0;
  SOC_PB_PP_FP_KEY_CONSTRAINT
    key_cons;
  SOC_PB_PMF_LKP_PROFILE
    lkp_profile_ndx;
  SOC_PB_PP_FP_KEY_LOCATION
    key_loc;
  SOC_PB_PP_FP_PREDEFINED_ACL_KEY
    predef_key;
  SOC_PB_PP_FP_KEY_LOCATION
    key_src;


  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_DATABASE_CREATE_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(success);
  *success = SOC_SAND_SUCCESS;

  /*
   *	Verify the Database does not already exist and set its type
   */
  SOC_PB_PP_FP_DATABASE_INFO_clear(&fp_database_info);
  res = soc_pb_pp_sw_db_fp_db_info_get(
          unit,
          db_id_ndx,
          &fp_database_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if ((fp_database_info.db_type != SOC_PB_PP_NOF_FP_DATABASE_TYPES) && (fp_database_info.db_type != SOC_PPC_NOF_FP_DATABASE_TYPES))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FP_DB_ID_ALREADY_EXIST_ERR, 12, exit);
  }

  /*
   *	Set the SW DB
   */
  res = soc_pb_pp_sw_db_fp_db_info_set(
          unit,
          db_id_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 14, exit);


  if (info->db_type == SOC_PB_PP_FP_DB_TYPE_EGRESS)
  {
    /*
     *  Different path for an Egress Database
     */
    res = soc_pb_pp_fp_egr_database_create_unsafe(
            unit,
            db_id_ndx,
            info,
            success
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    if (*success != SOC_SAND_SUCCESS)
    {
      goto exit_fail;
    }
    SOC_PETRA_DO_NOTHING_AND_EXIT;
  }

  /*
   * Verify a FEM can be found for this Database
   */
  SOC_PB_PP_FP_FEM_ENTRY_clear(&fem_entry);
  SOC_PB_PP_FP_FEM_CYCLE_clear(&fem_cycle);
  fem_entry.is_for_entry = FALSE;
  fem_entry.db_id = db_id_ndx;
  fem_entry.db_strength = info->strength;
  for (action_ndx = 0; action_ndx < SOC_PB_PP_FP_NOF_ACTIONS_PER_DB_MAX; action_ndx ++)
  {
    fem_entry.action_type[action_ndx] = info->action_types[action_ndx];
  }


  /*
   * Find FEM cycle constraints before TCAM bank add
   */
  for (entry_size = 0; (entry_size < SOC_PB_TCAM_NOF_BANK_ENTRY_SIZES) && (success_ce != SOC_SAND_SUCCESS); entry_size++)
  {
    if ((info->db_type != SOC_PB_PP_FP_DB_TYPE_TCAM) && (entry_size != 0))
    {
      continue;
    }
    else if (info->db_type == SOC_PB_PP_FP_DB_TYPE_TCAM)
    {
      /*
       * Compute the DB key minimal length
       */
      res = soc_pb_pp_fp_ce_key_length_minimal_get(
              unit,
              info,
              &db_key_length_min
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);

      switch (entry_size)
      {
      case SOC_PB_TCAM_BANK_ENTRY_SIZE_72_BITS:
        entry_size_in_b = SOC_PB_TCAM_BANK_ENTRY_SIZE_72_BITS_SIZE_IN_BITS;
      	break;
      case SOC_PB_TCAM_BANK_ENTRY_SIZE_144_BITS:
        entry_size_in_b = SOC_PB_TCAM_BANK_ENTRY_SIZE_144_BITS_SIZE_IN_BITS;
        break;
      case SOC_PB_TCAM_BANK_ENTRY_SIZE_288_BITS:
        entry_size_in_b = SOC_PB_TCAM_BANK_ENTRY_SIZE_288_BITS_SIZE_IN_BITS;
        break;
      default:
        entry_size_in_b = 0;
      }
      if (entry_size_in_b < db_key_length_min)
      {
        /* No space for prefix, minimal 0b due to the predefined keys */
        continue;
      }
      else
      {
        prefix_length = SOC_SAND_MIN(4, entry_size_in_b - db_key_length_min);
      }
    }

    for (cycle_ndx = 0; (cycle_ndx <= SOC_PB_PMF_LOW_LEVEL_CYCLE_NDX_MAX) && (success_ce != SOC_SAND_SUCCESS); ++cycle_ndx)
    {
      if ((info->db_type == SOC_PB_PP_FP_DB_TYPE_TCAM) || (info->db_type == SOC_PB_PP_FP_DB_TYPE_DIRECT_TABLE))
      {
        fem_cycle.is_cycle_fixed = TRUE;
      }
      else
      {
        fem_cycle.is_cycle_fixed = FALSE;
      }
      /* Start with Cycle 0 for TCAM consideration */
      fem_cycle.cycle_id = SOC_SAND_NUM2BOOL(cycle_ndx);

      res = soc_pb_pp_fp_fem_is_place_get_unsafe(
              unit,
              &fem_entry,
              &fem_cycle,
              &admissible_cycle
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

      if (admissible_cycle == FALSE)
      {
        continue;
      }

      /*
       * Verify if this cycle is acceptable
       */
      res = soc_pb_pp_fp_cycle_for_db_info_verify(
              unit,
              info,
              cycle_ndx,
              &is_acceptable_cycle
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

      if (is_acceptable_cycle == FALSE)
      {
        /*
         * Not acceptable cycle
         */
        continue;
      }


      /*
       * In case of TCAM verify all the admissible TCAM key
       */
      if (info->db_type == SOC_PB_PP_FP_DB_TYPE_TCAM)
      {
        tcam_db_id = soc_pb_egr_fp_tcam_db_id_get(db_id_ndx);
        res = soc_pb_tcam_db_create_unsafe(
                unit,
                tcam_db_id,
                entry_size,
                prefix_length,
                SOC_PB_TCAM_DB_PRIO_MODE_INTERLACED
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

        res = soc_pb_tcam_access_profile_create_unsafe(
                unit,
                tcam_db_id,
                tcam_db_id,
                fem_cycle.cycle_id,
                FALSE,
                1,
                soc_pb_pp_fp_tcam_callback,
                db_id_ndx,
                &success_tcam
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

        if (success_tcam != SOC_SAND_SUCCESS)
        {
          res = soc_pb_tcam_db_destroy_unsafe(
                  unit,
                  tcam_db_id
                );
          continue;
        }

        /*
         * Remove it to add it on the HW step
         */
        res = soc_pb_tcam_db_destroy_unsafe(
                unit,
                tcam_db_id
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

        res = soc_pb_tcam_access_profile_destroy_unsafe(
                unit,
                tcam_db_id
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

        res = soc_pb_pp_fp_key_get(
                unit,
                FALSE,
                info->qual_types,
                &predef_key
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);
        if (predef_key != SOC_PB_PP_NOF_FP_PREDEFINED_ACL_KEYS)
        {
          soc_pb_pp_fp_predefined_key_desc_set(
            unit,
            db_id_ndx,
            info->qual_types,
            predef_key
          );

          for (pfg_ndx = 0; pfg_ndx < SOC_PB_PP_FP_NOF_PFGS; ++pfg_ndx)
          {
            if ((info->supported_pfgs & (1 << pfg_ndx)) != 0)
            {
              key_src.tcam_src = predef_key; /* Mapping 1x1 for L2 and L3 IPv4/6 */

               soc_pb_pp_sw_db_fp_db_key_location_set(
                  unit,
                  db_id_ndx,
                  pfg_ndx,
                  &key_src
                );
            }
          }
          success_ce = SOC_SAND_SUCCESS;
        }
        else
        {
          /*
           * Compute with the CE with this cycle and Key size
           * for the necessary PFGs
           */
          for (pfg_ndx = 0; pfg_ndx < SOC_PB_PP_FP_NOF_PFGS; ++pfg_ndx)
          {
            if ((info->supported_pfgs & (1 << pfg_ndx)) != 0)
            {
              res = soc_pb_pp_fp_key_space_alloc_unsafe(
                      unit,
                      pfg_ndx,
                      db_id_ndx,
                      info,
                      fem_cycle.cycle_id,
                      entry_size, /* Same encoding between KEY_CONSTRAINT and ENTRY_SIZE for TCAM */
                      &success_ce
                    );
              SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);
              if (success_ce != SOC_SAND_SUCCESS)
              {
                break;
              }
            }
          }
        }
        cycle_ndx_fin = fem_cycle.cycle_id;
        entry_size_fin = entry_size;
      }
      else
      {
        /*
         * Compute with the CE with this cycle
         */
        if (info->db_type == SOC_PB_PP_FP_DB_TYPE_DIRECT_TABLE)
        {
          key_cons = SOC_PB_PP_FP_KEY_CONS_DIR_TBL;
        }
        else
        {
          key_cons = SOC_PB_PP_FP_KEY_CONS_FEM;

          /*
           * Verify it is tag
           */
          res = soc_pb_pp_fp_tag_action_type_convert(
                  unit,
                  info->action_types[0],
                  &is_tag_action,
                  &action_ndx
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 47, exit);

          if (is_tag_action)
          {
            key_cons = SOC_PB_PP_FP_KEY_CONS_TAG;
          }
        }

        for (pfg_ndx = 0; pfg_ndx < SOC_PB_PP_FP_NOF_PFGS; ++pfg_ndx)
        {
          if ((info->supported_pfgs & (1 << pfg_ndx)) != 0)
          {
            res = soc_pb_pp_fp_key_space_alloc_unsafe(
                    unit,
                    pfg_ndx,
                    db_id_ndx,
                    info,
                    fem_cycle.cycle_id,
                    key_cons,
                    &success_ce
                  );
            SOC_SAND_CHECK_FUNC_RESULT(res, 48, exit);

            if (success_ce != SOC_SAND_SUCCESS)
            {
              /* Remove the allocation and stop with this cycle */
              res = soc_pb_pp_fp_key_space_free_unsafe(
                      unit,
                      db_id_ndx
                    );
              SOC_SAND_CHECK_FUNC_RESULT(res, 49, exit);
              break;
            }
          }
        }
      }
      cycle_ndx_fin = fem_cycle.cycle_id;
      entry_size_fin = entry_size;
    }
  }

  if (success_ce != SOC_SAND_SUCCESS)
  {
    goto exit_fail;
  }
  else
  {
    soc_pb_pp_sw_db_fp_db_id_cycle_set(
      unit,
      db_id_ndx,
      SOC_SAND_NUM2BOOL(cycle_ndx_fin)
    );
  }


  /*
   * HW set stage: FEM - should not fail (same verification)
   */
  SOC_PETRA_ALLOC(dir_extr_entry_info, SOC_PB_PP_FP_DIR_EXTR_ENTRY_INFO, 1);
  SOC_PB_PP_FP_DIR_EXTR_ENTRY_INFO_clear(dir_extr_entry_info);
  for (action_ndx = 0; action_ndx < SOC_PB_PP_FP_NOF_ACTIONS_PER_DB_MAX; action_ndx ++)
  {
    dir_extr_entry_info->actions[action_ndx].type = info->action_types[action_ndx];
  }
  res = soc_pb_pp_fp_fem_insert_unsafe(
          unit,
          &fem_entry,
          &fem_cycle,
          dir_extr_entry_info,
          &success_fem
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  /*
   * TCAM Database set (save entry size)
   */
  if (info->db_type == SOC_PB_PP_FP_DB_TYPE_TCAM)
  {
    tcam_db_id = soc_pb_egr_fp_tcam_db_id_get(db_id_ndx);
    res = soc_pb_tcam_db_create_unsafe(
            unit,
            tcam_db_id,
            entry_size_fin,
            prefix_length,
            SOC_PB_TCAM_DB_PRIO_MODE_INTERLACED
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

    res = soc_pb_tcam_access_profile_create_unsafe(
            unit,
            tcam_db_id,
            tcam_db_id,
            cycle_ndx_fin,
            FALSE,
            1,
            soc_pb_pp_fp_tcam_callback,
            db_id_ndx,
            &success_tcam
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 62, exit);

    soc_pb_pp_sw_db_fp_tcam_db_key_size_set(
      unit,
      db_id_ndx,
      entry_size_fin
    );
  }
  else if (info->db_type == SOC_PB_PP_FP_DB_TYPE_DIRECT_TABLE)
  {
    SOC_PB_PMF_LKP_PROFILE_clear(&lkp_profile_ndx);
    lkp_profile_ndx.cycle_ndx = cycle_ndx_fin;
    for (pfg_ndx = 0; pfg_ndx < SOC_PB_PP_FP_NOF_PFGS; ++pfg_ndx)
    {
      if ((info->supported_pfgs & (1 << pfg_ndx)) != 0)
      {
        lkp_profile_ndx.id = pfg_ndx;
        SOC_PB_PP_FP_KEY_LOCATION_clear(&key_loc);
        res = soc_pb_pp_sw_db_fp_db_key_location_get(
                unit,
                db_id_ndx,
                pfg_ndx,
                &key_loc
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 65, exit);

        res = soc_pb_pmf_db_direct_tbl_key_src_set_unsafe(
                unit,
                &lkp_profile_ndx,
                key_loc.dir_tbl_src
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 67, exit);
      }
    }

    /* 
     * Disable the Raw access to Direct Table 
     */
    res = soc_pb_pmf_low_level_raw_pgm_dt_disable(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 68, exit);
  }

  /*
   * Update all the needed PFG-PMF-Pgm: for Copy Engine mainly
   */
  for (pfg_ndx = 0; pfg_ndx < SOC_PB_PP_FP_NOF_PFGS; ++pfg_ndx)
  {
    if ((info->supported_pfgs & (1 << pfg_ndx)) != 0)
    {
      res = soc_pb_pmf_pgm_mgmt_update(
              unit,
              pfg_ndx
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
    }
  }

  SOC_PB_PP_DO_NOTHING_AND_EXIT; /* Skip the exit_fail */

exit_fail:
  /*
   *	Set the SW DB to void
   */
  SOC_PB_PP_FP_DATABASE_INFO_clear(&fp_database_info);
  res = soc_pb_pp_sw_db_fp_db_info_set(
          unit,
          db_id_ndx,
          &fp_database_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 300, exit);

  *success = success_ce;


exit:
  SOC_PETRA_FREE(dir_extr_entry_info);
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_database_create_unsafe()", db_id_ndx, 0);
}

uint32
  soc_pb_pp_fp_database_create_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_IN  SOC_PB_PP_FP_DATABASE_INFO                    *info
  )
{
  uint32
    db_key_length_min,
    key_max_length,
    action_total_length,
    action_length_max,
    action_size_in_bits,
    action_size_in_bits_in_fem,
    res = SOC_SAND_OK;
  uint8
    is_all_action_tag = FALSE,
    is_tag_action,
    is_dt_exist = FALSE,
    is_vsq_enable = FALSE;
  uint32
    pfg_ndx,
    action_ndx,
    action_type_ndx,
    db_ndx;
  SOC_PB_PP_FP_DATABASE_INFO
    database_info;
  SOC_PB_PP_FP_PREDEFINED_ACL_KEY
    predefined_acl_key;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_DATABASE_CREATE_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(db_id_ndx, SOC_PB_PP_FP_DB_ID_NDX_MAX, SOC_PB_PP_FP_DB_ID_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_FP_DATABASE_INFO, info, 20, exit);

  /*
   * If Direct table, verify it is the single one and not used by VSQ FC
   */
  if (info->db_type == SOC_PB_PP_FP_DB_TYPE_DIRECT_TABLE)
  {
    res = soc_pb_fc_vsq_by_incoming_nif_get_unsafe(
            unit,
            &is_vsq_enable
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    for (db_ndx = 0; db_ndx < SOC_PB_PP_FP_NOF_DBS; ++db_ndx)
    {
      SOC_PB_PP_FP_DATABASE_INFO_clear(&database_info);
      res = soc_pb_pp_fp_database_get_unsafe(
              unit,
              db_ndx,
              &database_info
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

      if (database_info.db_type == SOC_PB_PP_FP_DB_TYPE_DIRECT_TABLE)
      {
        is_dt_exist = TRUE;
        break;
      }
    }

    if ((is_vsq_enable == TRUE) || (is_dt_exist == TRUE))
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FP_DB_ID_DIRECT_TABLE_ALREADY_EXIST_ERR, 50, exit);
    }
  }

  /*
   * Verify the qualifier lengths can be inserted in an admissible key
   */
  if (info->db_type != SOC_PB_PP_FP_DB_TYPE_EGRESS)
  {
    res = soc_pb_pp_fp_ingress_qual_verify(
            unit,
            info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 51, exit);

    res = soc_pb_pp_fp_ce_key_length_minimal_get(
            unit,
            info,
            &db_key_length_min
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 52, exit);

    switch (info->db_type)
    {
    case SOC_PB_PP_FP_DB_TYPE_TCAM:
      key_max_length = SOC_PB_PP_FP_DB_TYPE_TCAM_KEY_MAX_LENGTH_TCAM;
  	  break;
    case SOC_PB_PP_FP_DB_TYPE_DIRECT_TABLE:
      key_max_length = SOC_DPP_TCAM_BIG_BANK_KEY_NOF_BITS_PETRAB;
      break;
    case SOC_PB_PP_FP_DB_TYPE_DIRECT_EXTRACTION:
      key_max_length = SOC_PB_PP_FP_DB_TYPE_TCAM_KEY_MAX_LENGTH_DIRECT_EXTRACTION;
      break;
    default:
      key_max_length = 0;
    }

    if (db_key_length_min > key_max_length)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FP_QUALS_LENGTHS_OUT_OF_RANGE_ERR, 60, exit);
    }

    /*
     * Verify the action lengths can be inserted in
     * an admissible length for DT and TCAM
     */
    action_total_length = 0;
    for (action_type_ndx = 0; action_type_ndx < SOC_PB_PP_FP_NOF_ACTIONS_PER_DB_MAX; ++action_type_ndx)
    {
      /*
       * Get the action size
       */
      res = soc_pb_pp_fp_action_type_max_size_get(
              unit,
              info->action_types[action_type_ndx],
              &action_size_in_bits,
              &action_size_in_bits_in_fem
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
      action_total_length += action_size_in_bits;
    }

    if (info->db_type == SOC_PB_PP_FP_DB_TYPE_TCAM)
    {
      action_length_max = SOC_PB_PP_FP_ACTION_TOTAL_LENGTH_IN_BITS_TCAM;
    }
    else if (info->db_type == SOC_PB_PP_FP_DB_TYPE_DIRECT_TABLE)
    {
      action_length_max = SOC_PB_PP_FP_ACTION_TOTAL_LENGTH_IN_BITS_DIRECT_TABLE;
    }
    else
    {
      action_length_max = SOC_SAND_U32_MAX;
    }

    if (action_total_length > action_length_max)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FP_ACTION_LENGTHS_OUT_OF_RANGE_ERR, 80, exit);
    }

    /*
     * Verify the action TAG (if TAG) is not already used
     *  and all actions are TAG or are not
     */
    is_all_action_tag = FALSE;
    for (action_type_ndx = 0; action_type_ndx < SOC_PB_PP_FP_NOF_ACTIONS_PER_DB_MAX; ++action_type_ndx)
    {
      if (info->action_types[action_type_ndx] == SOC_PB_PP_NOF_FP_ACTION_TYPES)
      {
        continue;
      }

      res = soc_pb_pp_fp_tag_action_type_convert(
              unit,
              info->action_types[action_type_ndx],
              &is_tag_action,
              &action_ndx
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

      if (action_type_ndx == 0)
      {
        is_all_action_tag = is_tag_action;
      }
      else
      {
        if (is_tag_action != is_all_action_tag)
        {
          SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FP_ACTIONS_MIXED_WITH_TAG_ERR, 83, exit);
        }
      }

      if (is_tag_action == TRUE)
      {
        for (pfg_ndx = 0; pfg_ndx < SOC_PB_PP_FP_NOF_PFGS; ++pfg_ndx)
        {
          if ((info->supported_pfgs & (1 << pfg_ndx)) != 0)
          {
             db_ndx = soc_pb_pp_sw_db_fp_is_tag_used_get(
                         unit,
                         pfg_ndx,
                         action_ndx
                       );
             if (db_ndx != SOC_PB_PP_FP_NOF_DBS)
             {
               SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FP_TAG_ACTION_ALREADY_EXIST_ERR, 100 + pfg_ndx, exit);
             }
          }
        }
      }
    }
  } /* Not egress */
  else
  {
    /* Verify inside an egress key */
    res = soc_pb_pp_fp_key_get(
            unit,
            TRUE,
            info->qual_types,
            &predefined_acl_key
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);

    if (predefined_acl_key == SOC_PB_PP_NOF_FP_PREDEFINED_ACL_KEYS)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FP_QUAL_NOT_EGRESS_ERR, 130, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_database_create_verify()", db_id_ndx, 0);
}

/*********************************************************************
*     Get the database parameters.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_fp_database_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_OUT SOC_PB_PP_FP_DATABASE_INFO                    *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_DATABASE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PB_PP_FP_DATABASE_INFO_clear(info);

  /*
   *	Get the DB type and verify if it exists
   */
  res = soc_pb_pp_sw_db_fp_db_info_get(
          unit,
          db_id_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_database_get_unsafe()", db_id_ndx, 0);
}

uint32
  soc_pb_pp_fp_database_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_DATABASE_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(db_id_ndx, SOC_PB_PP_FP_DB_ID_NDX_MAX, SOC_PB_PP_FP_DB_ID_NDX_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_database_get_verify()", db_id_ndx, 0);
}

/*********************************************************************
*     Destroy the database: all its entries are suppressed and
 *     the Database-ID is freed.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_fp_database_destroy_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx
  )
{
  uint32
    tcam_db_id,
    res = SOC_SAND_OK;
  SOC_PB_PP_FP_DATABASE_INFO
    fp_database_info;
  SOC_PB_PP_FP_FEM_ENTRY
    entry_ndx;
  uint32
    pfg_ndx,
    action_id,
    action_type_ndx,
    entry_ndx_dt,
    action_ndx;
  uint8
    is_tag_action;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_DATABASE_DESTROY_UNSAFE);

  /*
   *	1. Verify the Database exists, otherwise exit
   */
  SOC_PB_PP_FP_DATABASE_INFO_clear(&fp_database_info);
  res = soc_pb_pp_fp_database_get_unsafe(
          unit,
          db_id_ndx,
          &fp_database_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if ((fp_database_info.db_type == SOC_PB_PP_NOF_FP_DATABASE_TYPES) || (fp_database_info.db_type == SOC_PPC_NOF_FP_DATABASE_TYPES))
  {
    SOC_PB_PP_DO_NOTHING_AND_EXIT;
  }
  /*
   * Special case for egress
   */
  else if (fp_database_info.db_type == SOC_PB_PP_FP_DB_TYPE_EGRESS)
  {
    res = soc_pb_pp_fp_egr_database_destroy_unsafe(
            unit,
            db_id_ndx
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

    goto exit1;
  }

  /*
   * Remove the FEM configuration
   */
  SOC_PB_PP_FP_FEM_ENTRY_clear(&entry_ndx);
  entry_ndx.is_for_entry = FALSE;
  entry_ndx.db_id = db_id_ndx;
  entry_ndx.db_strength = fp_database_info.strength;
  for (action_ndx = 0; action_ndx < SOC_PB_PP_FP_NOF_ACTIONS_PER_DB_MAX; action_ndx ++)
  {
    entry_ndx.action_type[action_ndx] = fp_database_info.action_types[action_ndx];
  }

  res = soc_pb_pp_fp_fem_remove(
          unit,
          &entry_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  /*
   * If TCAM, remove the Database
   */
  if (fp_database_info.db_type == SOC_PB_PP_FP_DB_TYPE_TCAM)
  {
    tcam_db_id = soc_pb_egr_fp_tcam_db_id_get(db_id_ndx);
    res = soc_pb_tcam_db_destroy_unsafe(
            unit,
            tcam_db_id
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    res = soc_pb_tcam_access_profile_destroy_unsafe(
            unit,
            tcam_db_id
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);
  }
  /*
   * If Direct Table, empty it
   */
  else if (fp_database_info.db_type == SOC_PB_PP_FP_DB_TYPE_DIRECT_TABLE)
  {
    for (entry_ndx_dt = 0; entry_ndx_dt < SOC_PB_PP_FP_NOF_ENTRY_IDS; ++entry_ndx_dt)
    {
      res = soc_pb_pp_fp_entry_remove_unsafe(
              unit,
              db_id_ndx,
              entry_ndx_dt
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
    }
  }

  /*
   * Remove CE configuration
   */
  res = soc_pb_pp_fp_key_space_free_unsafe(
          unit,
          db_id_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  /*
   *  If Tag actions, update the SW DB
   */
  for (action_type_ndx = 0; action_type_ndx < SOC_PB_PP_FP_NOF_ACTIONS_PER_DB_MAX; ++action_type_ndx)
  {
    if (fp_database_info.action_types[action_type_ndx] == SOC_PB_PP_NOF_FP_ACTION_TYPES)
    {
      continue;
    }

    res = soc_pb_pp_fp_tag_action_type_convert(
            unit,
            fp_database_info.action_types[action_type_ndx],
            &is_tag_action,
            &action_id
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 55, exit);

    if (is_tag_action == TRUE)
    {
      for (pfg_ndx = 0; pfg_ndx < SOC_PB_PP_FP_NOF_PFGS; ++pfg_ndx)
      {
        if ((fp_database_info.supported_pfgs & (1 << pfg_ndx)) == 0)
        {
          continue;
        }

        soc_pb_pp_sw_db_fp_is_tag_used_set(
          unit,
          pfg_ndx,
          action_id,
          SOC_PB_PP_FP_NOF_DBS
        );
      }
    }
  }

exit1:
  /*
   * Update all the PMF-Programs:
   * not needed for the CE since these instructions are not used
   */

  /*
   * Update SW DB
   */
  SOC_PB_PP_FP_DATABASE_INFO_clear(&fp_database_info);
  res = soc_pb_pp_sw_db_fp_db_info_set(
          unit,
          db_id_ndx,
          &fp_database_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);


  SOC_PB_PP_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_database_destroy_unsafe()", db_id_ndx, 0);
}

uint32
  soc_pb_pp_fp_database_destroy_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_DATABASE_DESTROY_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(db_id_ndx, SOC_PB_PP_FP_DB_ID_NDX_MAX, SOC_PB_PP_FP_DB_ID_NDX_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_database_destroy_verify()", db_id_ndx, 0);
}

uint32
  soc_pb_pp_fp_entry_ndx_direct_table_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_IN  SOC_PB_PP_FP_ENTRY_INFO                       *info,
    SOC_SAND_OUT uint32                                  *entry_dt_typical,
    SOC_SAND_OUT uint32                                  *bit_meaningful_mask
  )
{
  uint32
    qual_lsb,
    qual_length_no_padding,
    entry_dt_first,
    is_dt_bit_meaningful = 0,
    res = SOC_SAND_OK;
  SOC_PB_PP_FP_DATABASE_INFO
    db_info;
  uint32
    bit_ndx,
    qual_type_ndx;
  SOC_PB_PP_FP_KEY_DESC
    fp_key_desc;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_ENTRY_NDX_DIRECT_TABLE_GET);

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_pb_pp_sw_db_fp_db_info_get(
          unit,
          db_id_ndx,
          &db_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*
   * Replicate the entry as long as needed
   */
  SOC_PB_PP_FP_KEY_DESC_clear(&fp_key_desc);
  res = soc_pb_pp_fp_key_desc_get_unsafe(
          unit,
          db_id_ndx,
          &fp_key_desc
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  entry_dt_first = 0;
  for (qual_type_ndx = 0; qual_type_ndx < SOC_PB_PP_FP_NOF_QUALS_PER_DB_MAX; ++qual_type_ndx)
  {
    if (info->qual_vals[qual_type_ndx].type == SOC_PB_PP_NOF_FP_QUAL_TYPES)
    {
      continue;
    }

    for (bit_ndx = 0; bit_ndx < SOC_PB_PP_FP_DIRECT_TABLE_ENTRY_LENGTH_IN_BITS; ++bit_ndx)
    {
      if (
          (bit_ndx >= fp_key_desc.fields[qual_type_ndx].lsb)
          && (bit_ndx < fp_key_desc.fields[qual_type_ndx].lsb + fp_key_desc.fields[qual_type_ndx].length)
         )
      {
        if ((info->qual_vals[qual_type_ndx].is_valid.arr[0] & (1 << (bit_ndx - fp_key_desc.fields[qual_type_ndx].lsb))) != 0)
        {
          SOC_SAND_SET_BIT(is_dt_bit_meaningful, 0x1, bit_ndx);
        }
      }
    }

    res = soc_pb_pp_fp_qual_lsb_and_length_get(
            unit,
            db_id_ndx,
            info->qual_vals[qual_type_ndx].type,
            &db_info,
            &qual_lsb,
            &qual_length_no_padding
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 52, exit);

    entry_dt_first |= SOC_SAND_SET_BITS_RANGE(info->qual_vals[qual_type_ndx].val.arr[0], qual_lsb + qual_length_no_padding - 1, qual_lsb);
  }

  *entry_dt_typical = entry_dt_first;
  *bit_meaningful_mask = is_dt_bit_meaningful;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_entry_ndx_direct_table_get()", 0, 0);
}



/*********************************************************************
*     Add an entry to the Database. The database entry is
 *     selected if the entire relevant packet field values are
 *     matched to the database entry qualifiers values. When
 *     the packet is qualified to several entries, the entry
 *     with the strongest priority is chosen.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_fp_entry_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_IN  uint32                                 entry_id_ndx,
    SOC_SAND_IN  SOC_PB_PP_FP_ENTRY_INFO                       *info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  )
{
  uint32
    entry_dt_first,
    is_dt_bit_meaningful = 0,
    action_lsb,
    action_size,
    action_size_in_bits_in_fem,
    res = SOC_SAND_OK;
  SOC_PB_PP_SW_DB_FP_ENTRY
    sw_db_fp_entry;
  SOC_PB_PP_FP_DATABASE_INFO
    db_info;
  SOC_PB_PMF_DIRECT_TBL_DATA
    dir_tbl_data;
  uint32
    entry_ndx_dt,
    action_type_ndx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_ENTRY_ADD_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = soc_pb_pp_sw_db_fp_db_info_get(
          unit,
          db_id_ndx,
          &db_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (
      (db_info.db_type == SOC_PB_PP_FP_DB_TYPE_EGRESS)
      || (db_info.db_type == SOC_PB_PP_FP_DB_TYPE_TCAM)
      )
  {
    /*
     * Different path for Egress or TCAM
     */
    res = soc_pb_pp_fp_tcam_entry_add_unsafe(
            unit,
            db_id_ndx,
            entry_id_ndx,
            info,
            success
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

    goto exit1;
  }
  else if (db_info.db_type == SOC_PB_PP_FP_DB_TYPE_DIRECT_TABLE)
  {
    SOC_PB_PMF_DIRECT_TBL_DATA_clear(&dir_tbl_data);
    /*
     * Build the action similarly to the TCAM
     */
    dir_tbl_data.val = 0;
    for (action_type_ndx = 0; action_type_ndx < SOC_PB_PP_FP_NOF_ACTIONS_PER_DB_MAX; ++action_type_ndx)
    {
      if (info->actions[action_type_ndx].type == SOC_PB_PP_NOF_FP_ACTION_TYPES)
      {
        continue;
      }

      res = soc_pb_pp_fp_action_lsb_get(
              unit,
              info->actions[action_type_ndx].type,
              &db_info,
              &action_lsb
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

      res = soc_pb_pp_fp_action_type_max_size_get(
              unit,
              info->actions[action_type_ndx].type,
              &action_size,
              &action_size_in_bits_in_fem
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);
      dir_tbl_data.val |= SOC_SAND_SET_BITS_RANGE(info->actions[action_type_ndx].val, action_size + action_lsb - 1, action_lsb);
    }

    /*
     * Replicate the entry as long as needed
     */
    res = soc_pb_pp_fp_entry_ndx_direct_table_get(
            unit,
            db_id_ndx,
            info,
            &entry_dt_first,
            &is_dt_bit_meaningful
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);


    for (entry_ndx_dt = 0; entry_ndx_dt < SOC_PB_PMF_LOW_LEVEL_ENTRY_NDX_MAX; ++entry_ndx_dt)
    {
      if ((entry_ndx_dt & is_dt_bit_meaningful) == (entry_dt_first & is_dt_bit_meaningful))
      {
        res = soc_pb_pmf_db_direct_tbl_entry_set_unsafe(
                unit,
                entry_ndx_dt,
                &dir_tbl_data
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 55, exit);
      }
    }

    /*
     * Save it in SW DB (impossible extraction)
     */
    res = soc_pb_pp_sw_db_fp_dt_entry_set(
            unit,
            entry_id_ndx,
            info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 57, exit);
  }

exit1:
  if (*success == SOC_SAND_SUCCESS)
  {
    /*
     * Update the nof-entries per Database
     */
    res = soc_pb_pp_sw_db_fp_db_entries_get(
            unit,
            db_id_ndx,
            &sw_db_fp_entry
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

    sw_db_fp_entry.nof_db_entries ++;

    res = soc_pb_pp_sw_db_fp_db_entries_set(
            unit,
            db_id_ndx,
            &sw_db_fp_entry
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_entry_add_unsafe()", db_id_ndx, entry_id_ndx);
}

uint32
  soc_pb_pp_fp_entry_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_IN  uint32                                 entry_id_ndx,
    SOC_SAND_IN  SOC_PB_PP_FP_ENTRY_INFO                       *info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_FP_DATABASE_INFO
    fp_database_info;
  SOC_PB_PP_FP_ACTION_TYPE
    fp_action_type[SOC_PB_PP_FP_NOF_ACTIONS_PER_DB_MAX];
  uint32
    action_type_ndx;
  uint8
    is_found;
  SOC_PB_PP_FP_ENTRY_INFO
    entry_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_ENTRY_ADD_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(db_id_ndx, SOC_PB_PP_FP_DB_ID_NDX_MAX, SOC_PB_PP_FP_DB_ID_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(entry_id_ndx, SOC_PB_PP_FP_ENTRY_ID_NDX_MAX, SOC_PB_PP_FP_ENTRY_ID_NDX_OUT_OF_RANGE_ERR, 20, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_FP_ENTRY_INFO, info, 30, exit);

  /*
   *	Verify the Database exists and is of type TCAM or direct table
   */
  SOC_PB_PP_FP_DATABASE_INFO_clear(&fp_database_info);
  res = soc_pb_pp_fp_database_get_unsafe(
          unit,
          db_id_ndx,
          &fp_database_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  if ((fp_database_info.db_type != SOC_PB_PP_FP_DB_TYPE_TCAM)
        && (fp_database_info.db_type != SOC_PB_PP_FP_DB_TYPE_DIRECT_TABLE)
        && (fp_database_info.db_type != SOC_PB_PP_FP_DB_TYPE_EGRESS))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FP_DB_ID_NOT_LOOKUP_ERR, 45, exit);
  }

  /*
   * Verify the entry validity (vs the Database)
   */
  for (action_type_ndx = 0; action_type_ndx < SOC_PB_PP_FP_NOF_ACTIONS_PER_DB_MAX; ++action_type_ndx)
  {
    fp_action_type[action_type_ndx] = info->actions[action_type_ndx].type;
  }
  res = soc_pb_pp_fp_entry_validity_get(
          unit,
          &fp_database_info,
          info->qual_vals,
          fp_action_type
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  /*
   * Verify the entry does not already exist
   */
  SOC_PB_PP_FP_ENTRY_INFO_clear(&entry_info);
  res = soc_pb_pp_fp_entry_get_unsafe(
          unit,
          db_id_ndx,
          entry_id_ndx,
          &is_found,
          &entry_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
  if(is_found == TRUE)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FP_ENTRY_ALREADY_EXIST_ERR, 65, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_entry_add_verify()", db_id_ndx, entry_id_ndx);
}

/*********************************************************************
*     Get an entry from the Database.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_fp_entry_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_IN  uint32                                 entry_id_ndx,
    SOC_SAND_OUT uint8                                 *is_found,
    SOC_SAND_OUT SOC_PB_PP_FP_ENTRY_INFO                       *info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_FP_DATABASE_INFO
    fp_database_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_ENTRY_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(is_found);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PB_PP_FP_ENTRY_INFO_clear(info);

  SOC_PB_PP_FP_DATABASE_INFO_clear(&fp_database_info);
  res = soc_pb_pp_fp_database_get_unsafe(
          unit,
          db_id_ndx,
          &fp_database_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if ((fp_database_info.db_type == SOC_PB_PP_FP_DB_TYPE_TCAM)
      || (fp_database_info.db_type == SOC_PB_PP_FP_DB_TYPE_EGRESS))
  {
    res = soc_pb_pp_fp_tcam_entry_get_unsafe(
            unit,
            db_id_ndx,
            entry_id_ndx,
            is_found,
            info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  }
  else if (fp_database_info.db_type == SOC_PB_PP_FP_DB_TYPE_DIRECT_TABLE)
  {
    res = soc_pb_pp_sw_db_fp_dt_entry_get(
            unit,
            entry_id_ndx,
            info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    if (info->actions[0].type == SOC_PB_PP_NOF_FP_ACTION_TYPES)
    {
      *is_found = FALSE;
    }
    else
    {
      *is_found = TRUE;
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_entry_get_unsafe()", db_id_ndx, entry_id_ndx);
}

uint32
  soc_pb_pp_fp_entry_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_IN  uint32                                 entry_id_ndx
  )
{
  SOC_PB_PP_FP_DATABASE_INFO
    fp_database_info;
  uint32
    res = SOC_SAND_OK;
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_ENTRY_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(db_id_ndx, SOC_PB_PP_FP_DB_ID_NDX_MAX, SOC_PB_PP_FP_DB_ID_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(entry_id_ndx, SOC_PB_PP_FP_ENTRY_ID_NDX_MAX, SOC_PB_PP_FP_ENTRY_ID_NDX_OUT_OF_RANGE_ERR, 20, exit);

  /*
   * Verify the Database is of type TCAM or direct table
   */
  SOC_PB_PP_FP_DATABASE_INFO_clear(&fp_database_info);
  res = soc_pb_pp_fp_database_get_unsafe(
          unit,
          db_id_ndx,
          &fp_database_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  if ((fp_database_info.db_type != SOC_PB_PP_FP_DB_TYPE_TCAM)
        && (fp_database_info.db_type != SOC_PB_PP_FP_DB_TYPE_DIRECT_TABLE)
        && (fp_database_info.db_type != SOC_PB_PP_FP_DB_TYPE_EGRESS))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FP_DB_ID_NOT_LOOKUP_ERR, 40, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_entry_get_verify()", db_id_ndx, entry_id_ndx);
}

/*********************************************************************
*     Remove an entry from the Database.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_fp_entry_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_IN  uint32                                 entry_id_ndx
  )
{
  uint32
    entry_dt_first,
    is_dt_bit_meaningful,
    res = SOC_SAND_OK;
  SOC_PB_PP_SW_DB_FP_ENTRY
    sw_db_fp_entry;
  uint8
    is_found = TRUE;
  SOC_PB_PP_FP_DATABASE_INFO
    fp_database_info;
  SOC_PB_PP_FP_ENTRY_INFO
    fp_entry_info;
  uint32
    entry_ndx_dt;
  SOC_PB_PMF_DIRECT_TBL_DATA
    dir_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_ENTRY_REMOVE_UNSAFE);

  res = soc_pb_pp_fp_database_get_unsafe(
          unit,
          db_id_ndx,
          &fp_database_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if ((fp_database_info.db_type == SOC_PB_PP_FP_DB_TYPE_TCAM)
    || (fp_database_info.db_type == SOC_PB_PP_FP_DB_TYPE_EGRESS))
  {
    /*
     *  TCAM-based DB
     */
    res = soc_pb_pp_fp_tcam_entry_remove_unsafe(
            unit,
            db_id_ndx,
            entry_id_ndx,
            &is_found
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }
  else
  {
    SOC_PB_PP_FP_ENTRY_INFO_clear(&fp_entry_info);
    res = soc_pb_pp_fp_entry_get_unsafe(
            unit,
            db_id_ndx,
            entry_id_ndx,
            &is_found,
            &fp_entry_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    if (is_found == TRUE)
    {
      res = soc_pb_pp_fp_entry_ndx_direct_table_get(
              unit,
              db_id_ndx,
              &fp_entry_info,
              &entry_dt_first,
              &is_dt_bit_meaningful
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

      SOC_PB_PMF_DIRECT_TBL_DATA_clear(&dir_tbl_data);
      dir_tbl_data.val = 0;
      for (entry_ndx_dt = 0; entry_ndx_dt < SOC_PB_PMF_LOW_LEVEL_ENTRY_NDX_MAX; ++entry_ndx_dt)
      {
        if ((entry_ndx_dt & is_dt_bit_meaningful) == (entry_dt_first & is_dt_bit_meaningful))
        {
          res = soc_pb_pmf_db_direct_tbl_entry_set_unsafe(
                  unit,
                  entry_ndx_dt,
                  &dir_tbl_data
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 55, exit);
        }
      }

      /*
       * Remove from the SW DB
       */
      SOC_PB_PP_FP_ENTRY_INFO_clear(&fp_entry_info);
      res = soc_pb_pp_sw_db_fp_dt_entry_set(
              unit,
              entry_id_ndx,
              &fp_entry_info
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 57, exit);
    }
  }

  if (is_found)
  {
    /*
     *	Update the SW DB
     */
     res = soc_pb_pp_sw_db_fp_db_entries_get(
            unit,
            db_id_ndx,
            &sw_db_fp_entry
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    sw_db_fp_entry.nof_db_entries --;
    res = soc_pb_pp_sw_db_fp_db_entries_set(
            unit,
            db_id_ndx,
            &sw_db_fp_entry
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_entry_remove_unsafe()", db_id_ndx, entry_id_ndx);
}

uint32
  soc_pb_pp_fp_entry_remove_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_IN  uint32                                 entry_id_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_ENTRY_REMOVE_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(db_id_ndx, SOC_PB_PP_FP_DB_ID_NDX_MAX, SOC_PB_PP_FP_DB_ID_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(entry_id_ndx, SOC_PB_PP_FP_ENTRY_ID_NDX_MAX, SOC_PB_PP_FP_ENTRY_ID_NDX_OUT_OF_RANGE_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_entry_remove_verify()", db_id_ndx, entry_id_ndx);
}

/*********************************************************************
*     Get the Database entries. The function returns list of
 *     entries that were added to a database with database ID
 *     'db_id_ndx'.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_fp_database_entries_get_block_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                    *block_range,
    SOC_SAND_OUT SOC_PB_PP_FP_ENTRY_INFO                       *entries,
    SOC_SAND_OUT uint32                                  *nof_entries
  )
{
  uint32
    nof_valid_entries = 0,
    res = SOC_SAND_OK;
  uint32
    entry_ndx;
  SOC_PB_PP_SW_DB_FP_ENTRY
    sw_db_fp_entry;
  uint8
    is_found;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_DATABASE_ENTRIES_GET_BLOCK_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(block_range);
  SOC_SAND_CHECK_NULL_INPUT(entries);
  SOC_SAND_CHECK_NULL_INPUT(nof_entries);

  res = soc_pb_pp_sw_db_fp_db_entries_get(
          unit,
          db_id_ndx,
          &sw_db_fp_entry
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (sw_db_fp_entry.nof_db_entries != 0)
  {
    for (entry_ndx = block_range->iter; entry_ndx < block_range->iter + block_range->entries_to_scan; ++entry_ndx)
    {
      if (nof_valid_entries >= block_range->entries_to_act)
      {
        /*
         *	No need to go further, maximal number of entries have been found
         */
        break;
      }
      res = soc_pb_pp_fp_entry_get_unsafe(
              unit,
              db_id_ndx,
              entry_ndx,
              &is_found,
              &(entries[nof_valid_entries])
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

      if (is_found == TRUE)
      {
        nof_valid_entries ++;
      }
    }
  }
  else
  {
    entry_ndx = block_range->iter + block_range->entries_to_scan;
    nof_valid_entries = 0;
  }

  *nof_entries = nof_valid_entries;
  block_range->iter = entry_ndx;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_database_entries_get_block_unsafe()", db_id_ndx, 0);
}

uint32
  soc_pb_pp_fp_database_entries_get_block_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                    *block_range
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_DATABASE_ENTRIES_GET_BLOCK_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(db_id_ndx, SOC_PB_PP_FP_DB_ID_NDX_MAX, SOC_PB_PP_FP_DB_ID_NDX_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_database_entries_get_block_verify()", db_id_ndx, 0);
}

/*********************************************************************
*     Add an entry to the Database. The database entry is
 *     selected if all the Packet Qualifier field values are in
 *     the Database entry range.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_fp_direct_extraction_entry_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_IN  uint32                                 entry_id_ndx,
    SOC_SAND_IN  SOC_PB_PP_FP_DIR_EXTR_ENTRY_INFO              *info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_FP_FEM_ENTRY
    fem_entry;
  SOC_PB_PP_FP_FEM_CYCLE
    fem_cycle;
  uint8
    fem_is_found;
  SOC_PB_PP_FP_DATABASE_INFO
    database_info;
  uint32
    action_ndx;
  SOC_PB_PP_SW_DB_FP_ENTRY
    sw_db_fp_entry;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_DIRECT_EXTRACTION_ENTRY_ADD_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  SOC_PB_PP_FP_DATABASE_INFO_clear(&database_info);
  res = soc_pb_pp_fp_database_get_unsafe(
          unit,
          db_id_ndx,
          &database_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  /*
   *	Find if there is a free FEM
   */
  SOC_PB_PP_FP_FEM_ENTRY_clear(&fem_entry);
  SOC_PB_PP_FP_FEM_CYCLE_clear(&fem_cycle);
  fem_entry.is_for_entry = TRUE;
  fem_entry.db_id = db_id_ndx;
  fem_entry.db_strength = database_info.strength;
  fem_entry.entry_id = entry_id_ndx;
  fem_entry.entry_strength = info->priority;
  for (action_ndx = 0; action_ndx < SOC_PB_PP_FP_NOF_ACTIONS_PER_DB_MAX; action_ndx ++)
  {
    fem_entry.action_type[action_ndx] = info->actions[action_ndx].type;
    fem_entry.is_base_positive[action_ndx] = SOC_SAND_NUM2BOOL(info->actions[action_ndx].base_val);
  }

  fem_cycle.is_cycle_fixed = FALSE;
  res = soc_pb_pp_fp_fem_is_place_get_unsafe(
          unit,
          &fem_entry,
          &fem_cycle,
          &fem_is_found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);


  if (fem_is_found == FALSE)
  {
    *success = SOC_SAND_FAILURE_OUT_OF_RESOURCES;
    SOC_PB_DO_NOTHING_AND_EXIT;
  }

  /*
   * Set it
   */
  res = soc_pb_pp_fp_fem_insert_unsafe(
          unit,
          &fem_entry,
          &fem_cycle,
          info,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);


  res = soc_pb_pp_sw_db_fp_db_entries_get(
          unit,
          db_id_ndx,
          &sw_db_fp_entry
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  sw_db_fp_entry.nof_db_entries ++;

  res = soc_pb_pp_sw_db_fp_db_entries_set(
          unit,
          db_id_ndx,
          &sw_db_fp_entry
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_direct_extraction_entry_add_unsafe()", db_id_ndx, entry_id_ndx);
}

uint32
  soc_pb_pp_fp_direct_extraction_entry_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_IN  uint32                                 entry_id_ndx,
    SOC_SAND_IN  SOC_PB_PP_FP_DIR_EXTR_ENTRY_INFO              *info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_FP_DATABASE_INFO
    fp_database_info;
  SOC_PB_PP_FP_ACTION_TYPE
    fp_action_type[SOC_PB_PP_FP_NOF_ACTIONS_PER_DB_MAX];
  uint32
    action_type_ndx;
  uint8
    is_found;
  SOC_PB_PP_FP_DIR_EXTR_ENTRY_INFO
    *dir_extr_entry_info = NULL;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_DIRECT_EXTRACTION_ENTRY_ADD_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(db_id_ndx, SOC_PB_PP_FP_DB_ID_NDX_MAX, SOC_PB_PP_FP_DB_ID_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(entry_id_ndx, SOC_PB_PP_FP_DE_ENTRY_ID_NDX_MAX, SOC_PB_PP_FP_ENTRY_ID_NDX_OUT_OF_RANGE_ERR, 20, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_FP_DIR_EXTR_ENTRY_INFO, info, 30, exit);

  /*
   *	Verify the Database exists and is of type DIrect Extraction
   */
  SOC_PB_PP_FP_DATABASE_INFO_clear(&fp_database_info);
  res = soc_pb_pp_fp_database_get_unsafe(
          unit,
          db_id_ndx,
          &fp_database_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  if (fp_database_info.db_type != SOC_PB_PP_FP_DB_TYPE_DIRECT_EXTRACTION)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FP_DB_ID_NOT_DIRECT_EXTRACTION_ERR, 45, exit);
  }

  /*
   *	Verify the action types and qualifiers types exist in the Database
   */
  for (action_type_ndx = 0; action_type_ndx < SOC_PB_PP_FP_NOF_ACTIONS_PER_DB_MAX; ++action_type_ndx)
  {
    fp_action_type[action_type_ndx] = info->actions[action_type_ndx].type;
  }

  res = soc_pb_pp_fp_entry_validity_get(
          unit,
          &fp_database_info,
          info->qual_vals,
          fp_action_type
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  /*
   *	Verify it is the entry already exists
   */
  SOC_PETRA_ALLOC(dir_extr_entry_info, SOC_PB_PP_FP_DIR_EXTR_ENTRY_INFO, 1);
  SOC_PB_PP_FP_DIR_EXTR_ENTRY_INFO_clear(dir_extr_entry_info);
  res = soc_pb_pp_fp_direct_extraction_entry_get_unsafe(
          unit,
          db_id_ndx,
          entry_id_ndx,
          &is_found,
          dir_extr_entry_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
  if (is_found == TRUE)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FP_ENTRY_ALREADY_EXIST_ERR, 65, exit);
  }

exit:
  SOC_PETRA_FREE(dir_extr_entry_info);
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_direct_extraction_entry_add_verify()", db_id_ndx, entry_id_ndx);
}

/*********************************************************************
*     Get an entry from the Database.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_fp_direct_extraction_entry_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_IN  uint32                                 entry_id_ndx,
    SOC_SAND_OUT uint8                                 *is_found,
    SOC_SAND_OUT SOC_PB_PP_FP_DIR_EXTR_ENTRY_INFO              *info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_FP_DATABASE_INFO
    fp_database_info;
  uint8
    fem_is_found;
  uint32
    action_ndx,
    fem_id_ndx,
    cycle_ndx;
  SOC_PB_PP_FP_FEM_ENTRY
    fem_entry;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_DIRECT_EXTRACTION_ENTRY_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(is_found);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PB_PP_FP_DIR_EXTR_ENTRY_INFO_clear(info);

  /*
   *	Get the FEM configuration and Database properties
   */
  SOC_PB_PP_FP_DATABASE_INFO_clear(&fp_database_info);
  res = soc_pb_pp_fp_database_get_unsafe(
          unit,
          db_id_ndx,
          &fp_database_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*
   * Find if there is a FEM for this entry
   */
  fem_is_found = FALSE;
  action_ndx = 0;
  for (cycle_ndx = 0; (cycle_ndx < SOC_PB_PMF_NOF_CYCLES) && (fem_is_found == FALSE); ++cycle_ndx)
  {
    for (fem_id_ndx = 0; (fem_id_ndx <= SOC_PB_PMF_LOW_LEVEL_FEM_ID_MAX) && (fem_is_found == FALSE); ++fem_id_ndx)
    {
      SOC_PB_PP_FP_FEM_ENTRY_clear(&fem_entry);
      res = soc_pb_pp_sw_db_fp_fem_entry_get(
              unit,
              cycle_ndx,
              fem_id_ndx,
              &fem_entry
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

      if ((fem_entry.db_id == db_id_ndx) && (fem_entry.entry_id == entry_id_ndx) && (fem_entry.is_for_entry == TRUE))
      {
         res = soc_pb_pp_fp_fem_configuration_de_get(
                unit,
                fem_id_ndx,
                cycle_ndx,
                &fem_entry,
                &(info->actions[action_ndx]),
                &(info->qual_vals[0])
              );
         SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
         info->priority = fem_entry.entry_strength;
         action_ndx ++;
         fem_is_found = TRUE;
      }
    }
  }

  *is_found = fem_is_found;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_direct_extraction_entry_get_unsafe()", db_id_ndx, entry_id_ndx);
}

uint32
  soc_pb_pp_fp_direct_extraction_entry_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_IN  uint32                                 entry_id_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_DIRECT_EXTRACTION_ENTRY_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(db_id_ndx, SOC_PB_PP_FP_DB_ID_NDX_MAX, SOC_PB_PP_FP_DB_ID_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(entry_id_ndx, SOC_PB_PP_FP_DE_ENTRY_ID_NDX_MAX, SOC_PB_PP_FP_ENTRY_ID_NDX_OUT_OF_RANGE_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_direct_extraction_entry_get_verify()", db_id_ndx, entry_id_ndx);
}

/*********************************************************************
*     Remove an entry from the Database.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_fp_direct_extraction_entry_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_IN  uint32                                 entry_id_ndx
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_SW_DB_FP_ENTRY
    sw_db_fp_entry;
  uint8
    fem_is_found;
  SOC_PB_PP_FP_DATABASE_INFO
    fp_database_info;
  uint32
    action_ndx;
  SOC_PB_PP_FP_FEM_ENTRY
    fem_entry;
  SOC_PB_PP_FP_DIR_EXTR_ENTRY_INFO
    *dir_extr_entry_info = NULL;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_DIRECT_EXTRACTION_ENTRY_REMOVE_UNSAFE);


  /*
   *	Find the FEM
   */
  SOC_PB_PP_FP_DATABASE_INFO_clear(&fp_database_info);
  res = soc_pb_pp_fp_database_get_unsafe(
          unit,
          db_id_ndx,
          &fp_database_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  fem_is_found = FALSE;

  SOC_PETRA_ALLOC(dir_extr_entry_info, SOC_PB_PP_FP_DIR_EXTR_ENTRY_INFO, 1);
  SOC_PB_PP_FP_DIR_EXTR_ENTRY_INFO_clear(dir_extr_entry_info);
  res = soc_pb_pp_fp_direct_extraction_entry_get_unsafe(
          unit,
          db_id_ndx,
          entry_id_ndx,
          &fem_is_found,
          dir_extr_entry_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  /*
   * Build the entry struct
   */
  SOC_PB_PP_FP_FEM_ENTRY_clear(&fem_entry);
  fem_entry.is_for_entry = TRUE;
  fem_entry.db_id = db_id_ndx;
  fem_entry.entry_id = entry_id_ndx;
  for (action_ndx = 0; action_ndx < SOC_PB_PP_FP_NOF_ACTIONS_PER_DB_MAX; action_ndx ++)
  {
    fem_entry.action_type[action_ndx] = dir_extr_entry_info->actions[action_ndx].type;
  }

  /*
   * Remove from the HW (and reorganize the FEMs)
   */
  res = soc_pb_pp_fp_fem_remove(
          unit,
          &fem_entry
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  /*
   *	Reset the FEM as in the DB create API
   */
  if (fem_is_found == TRUE)
  {
    res = soc_pb_pp_sw_db_fp_db_entries_get(
            unit,
            db_id_ndx,
            &sw_db_fp_entry
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

    /*
     *	Update the SW DB
     */
    sw_db_fp_entry.nof_db_entries --;

    res = soc_pb_pp_sw_db_fp_db_entries_set(
            unit,
            db_id_ndx,
            &sw_db_fp_entry
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  }

exit:
  SOC_PETRA_FREE(dir_extr_entry_info);
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_direct_extraction_entry_remove_unsafe()", db_id_ndx, entry_id_ndx);
}

uint32
  soc_pb_pp_fp_direct_extraction_entry_remove_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_IN  uint32                                 entry_id_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_DIRECT_EXTRACTION_ENTRY_REMOVE_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(db_id_ndx, SOC_PB_PP_FP_DB_ID_NDX_MAX, SOC_PB_PP_FP_DB_ID_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(entry_id_ndx, SOC_PB_PP_FP_DE_ENTRY_ID_NDX_MAX, SOC_PB_PP_FP_ENTRY_ID_NDX_OUT_OF_RANGE_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_direct_extraction_entry_remove_verify()", db_id_ndx, entry_id_ndx);
}

/*********************************************************************
*     Get the Database entries.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_fp_direct_extraction_db_entries_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                  *block_range,
    SOC_SAND_OUT SOC_PB_PP_FP_DIR_EXTR_ENTRY_INFO              *entries,
    SOC_SAND_OUT uint32                                  *nof_entries
  )
{
  uint32
    nof_valid_entries = 0,
    res = SOC_SAND_OK;
  uint32
    entry_ndx;
  SOC_PB_PP_SW_DB_FP_ENTRY
    sw_db_fp_entry;
  uint8
    is_found;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_DIRECT_EXTRACTION_DB_ENTRIES_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(block_range);
  SOC_SAND_CHECK_NULL_INPUT(entries);
  SOC_SAND_CHECK_NULL_INPUT(nof_entries);

  SOC_PB_PP_FP_DIR_EXTR_ENTRY_INFO_clear(entries);

  res = soc_pb_pp_sw_db_fp_db_entries_get(
          unit,
          db_id_ndx,
          &sw_db_fp_entry
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (sw_db_fp_entry.nof_db_entries != 0)
  {
    for (entry_ndx = block_range->iter; entry_ndx < block_range->iter + block_range->entries_to_scan; ++entry_ndx)
    {
      if (nof_valid_entries >= block_range->entries_to_act)
      {
        /*
         *	No need to go further, maximal number of entries have been found
         */
        break;
      }
      res = soc_pb_pp_fp_direct_extraction_entry_get_unsafe(
              unit,
              db_id_ndx,
              entry_ndx,
              &is_found,
              &(entries[nof_valid_entries])
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

      if (is_found == TRUE)
      {
        nof_valid_entries ++;
      }
    }
  }
  else
  {
    entry_ndx = block_range->iter + block_range->entries_to_scan;
    nof_valid_entries = 0;
  }

  *nof_entries = nof_valid_entries;
  block_range->iter = entry_ndx;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_direct_extraction_db_entries_get_unsafe()", db_id_ndx, 0);
}

uint32
  soc_pb_pp_fp_direct_extraction_db_entries_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                 *block_range
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_DIRECT_EXTRACTION_DB_ENTRIES_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(db_id_ndx, SOC_PB_PP_FP_DB_ID_NDX_MAX, SOC_PB_PP_FP_DB_ID_NDX_OUT_OF_RANGE_ERR, 10, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_direct_extraction_db_entries_get_verify()", db_id_ndx, 0);
}

/*********************************************************************
*     Set one of the control options.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_fp_control_set_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PB_PP_FP_CONTROL_INDEX       *control_ndx,
    SOC_SAND_IN  SOC_PB_PP_FP_CONTROL_INFO        *info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE         *success
  )
{
  uint32
    reg_ndx,
    fld_ndx,
    internal_ndx,
    res = SOC_SAND_OK;
  SOC_PB_IHB_SRC_DEST_PORT_FOR_L3_ACL_KEY_TBL_DATA
    src_dest_port_for_l3_acl_key_tbl;
  SOC_PB_PP_REGS
    *pp_regs;
  SOC_PB_EGR_ACL_PORT_INFO
    acl_port_info;  
  uint8
    internal_ndx8;
  SOC_PB_EGR_ACL_DP_VALUES
    dp_val;
  SOC_PB_PP_FP_KEY_QUAL_INFO
    qual_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_CONTROL_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(control_ndx);
  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  pp_regs = soc_pb_pp_regs();
  *success = SOC_SAND_SUCCESS;

  switch (control_ndx->type)
  {
  case SOC_PB_PP_FP_CONTROL_TYPE_L4OPS_RANGE:
    src_dest_port_for_l3_acl_key_tbl.source_port_min = info->val[0];
    src_dest_port_for_l3_acl_key_tbl.source_port_max = info->val[1];
    src_dest_port_for_l3_acl_key_tbl.destination_port_min = info->val[2];
    src_dest_port_for_l3_acl_key_tbl.destination_port_max = info->val[3];

    res = soc_pb_ihb_src_dest_port_for_l3_acl_key_tbl_set_unsafe(
            unit,
            control_ndx->val_ndx,
            &src_dest_port_for_l3_acl_key_tbl
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    break;

  case SOC_PB_PP_FP_CONTROL_TYPE_PACKET_SIZE_RANGE:
    SOC_PB_PP_FLD_SET(pp_regs->ihb.packet_header_size_range_reg[control_ndx->val_ndx].pkt_hdr_size_range_low, info->val[0] - 1, 40, exit);
    SOC_PB_PP_FLD_SET(pp_regs->ihb.packet_header_size_range_reg[control_ndx->val_ndx].pkt_hdr_size_range_high, info->val[1] - 1, 42, exit);
    break;

  case SOC_PB_PP_FP_CONTROL_TYPE_L2_L3_KEY_IN_LIF_ENABLE:
    SOC_PB_PP_FLD_SET(pp_regs->ihb.pmfgeneral_configuration1_reg.fixed_keys_use_ac, info->val[0], 50, exit);
    break;

  case SOC_PB_PP_FP_CONTROL_TYPE_L3_IPV6_TCP_CTL_ENABLE:
    SOC_PB_PP_FLD_SET(pp_regs->ihb.pmfgeneral_configuration1_reg.fixed_keys_use_tcp_control, info->val[0], 60, exit);
    break;

  case SOC_PB_PP_FP_CONTROL_TYPE_EGR_PP_PORT_DATA:
  case SOC_PB_PP_FP_CONTROL_TYPE_PP_PORT_PROFILE:
    SOC_PB_EGR_ACL_PORT_INFO_clear(&acl_port_info);
    res = soc_pb_egr_acl_port_get_unsafe(
            unit,
            control_ndx->val_ndx,
            &acl_port_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

    if (control_ndx->type == SOC_PB_PP_FP_CONTROL_TYPE_EGR_PP_PORT_DATA)
    {
      acl_port_info.acl_data = info->val[0];
    }
    else /* SOC_PB_PP_FP_CONTROL_TYPE_PP_PORT_PROFILE */
    {
      acl_port_info.profile = info->val[0];
    }

    res = soc_pb_egr_acl_port_set_unsafe(
            unit,
            control_ndx->val_ndx,
            &acl_port_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 72, exit);
    break;

  case SOC_PB_PP_FP_CONTROL_TYPE_EGR_L2_ETHERTYPES:
    reg_ndx = (control_ndx->val_ndx == 8)? 0 : ((control_ndx->val_ndx - 1) / 2 - 3); /* 8 to 0, the 9 & 10 to 1, etc.*/
    fld_ndx = (control_ndx->val_ndx == 8)? 0 : (control_ndx->val_ndx + 1) % 2; /* 9 & 8 to 0, the  10 to 1, etc.*/
    SOC_PB_PP_FLD_SET(pp_regs->egq.ethernet_types_reg[reg_ndx].eth_type[fld_ndx], info->val[0], 75, exit);
    break;

  case SOC_PB_PP_FP_CONTROL_TYPE_EGR_IPV4_NEXT_PROTOCOL:
    res = soc_pb_egr_acl_l4_protocol_code_set_unsafe(
            unit,
            control_ndx->val_ndx,
            info->val[0]
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
    break;

  case SOC_PB_PP_FP_CONTROL_TYPE_INNER_ETH_NOF_VLAN_TAGS:
    soc_pb_pp_sw_db_fp_inner_eth_nof_tags_set(
      unit,
      control_ndx->val_ndx,
      info->val[0]
    );
    break;

  case SOC_PB_PP_FP_CONTROL_TYPE_KEY_CHANGE_SIZE:
    soc_pb_pp_sw_db_fp_key_change_size_set(
      unit,
      (uint8)   info->val[0]
      );
    break;

  case SOC_PB_PP_FP_CONTROL_TYPE_EGRESS_DP:
    SOC_PB_EGR_ACL_DP_VALUES_clear(&dp_val);
    res = soc_pb_egr_acl_dp_values_get_unsafe(
            unit,
            &dp_val
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 85, exit);

    dp_val.val[control_ndx->val_ndx] = info->val[0];

    res = soc_pb_egr_acl_dp_values_set_unsafe(
            unit,
            &dp_val
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 87, exit);
    break;

  case SOC_PB_PP_FP_CONTROL_TYPE_HDR_USER_DEF:
    sal_memset(&qual_info, 0x0, sizeof(SOC_PB_PP_FP_KEY_QUAL_INFO));

    /*
     * Build the instruction
     */
	if (control_ndx->db_id == 0) {
		/* 
		 * instruction is built according to offset+size from packet headers
		 */
		SOC_PB_PMF_CE_SUB_HEADER
			base_header_type;
		switch (info->val[SOC_PB_PP_FP_CONTROL_TYPE_HDR_USER_DEF_SUB_HEADER_NDX])
		{
		case SOC_PB_PP_FP_BASE_HEADER_TYPE_HEADER_0:
		  base_header_type = SOC_PB_PMF_CE_SUB_HEADER_0;
		  break;
		case SOC_PB_PP_FP_BASE_HEADER_TYPE_HEADER_1:
		  base_header_type = SOC_PB_PMF_CE_SUB_HEADER_1;
		  break;
		case SOC_PB_PP_FP_BASE_HEADER_TYPE_HEADER_2:
		  base_header_type = SOC_PB_PMF_CE_SUB_HEADER_2;
		  break;
		case SOC_PB_PP_FP_BASE_HEADER_TYPE_HEADER_3:
		  base_header_type = SOC_PB_PMF_CE_SUB_HEADER_3;
		  break;
		case SOC_PB_PP_FP_BASE_HEADER_TYPE_HEADER_4:
		  base_header_type = SOC_PB_PMF_CE_SUB_HEADER_4;
		  break;
		case SOC_PB_PP_FP_BASE_HEADER_TYPE_HEADER_5:
		  base_header_type = SOC_PB_PMF_CE_SUB_HEADER_5;
		  break;
		case SOC_PB_PP_FP_BASE_HEADER_TYPE_FWD:
		  base_header_type = SOC_PB_PMF_CE_SUB_HEADER_FWD;
		  break;
		case SOC_PB_PP_FP_BASE_HEADER_TYPE_FWD_POST:
		  base_header_type = SOC_PB_PMF_CE_SUB_HEADER_FWD_POST;
		  break;
		default:
		  SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FP_VAL_NDX_OUT_OF_RANGE_ERR, 82, exit);
		}
		qual_info.qual_type    = SOC_PB_PP_FP_QUAL_HDR_USER_DEF_0 + control_ndx->val_ndx;
		qual_info.data_src     = SOC_PB_PP_FP_KEY_SRC_PKT_HDR;
		qual_info.header_ndx_0 = base_header_type;
		qual_info.msb          = info->val[SOC_PB_PP_FP_CONTROL_TYPE_HDR_USER_DEF_OFFSET_NDX];
		qual_info.lsb          = qual_info.msb
								   + info->val[SOC_PB_PP_FP_CONTROL_TYPE_HDR_USER_DEF_NOF_BITS_NDX] - 1;
	} else {
		/* 
		 * instruction is built by taking some of the bits of a defined qualifier
		 */
		SOC_PB_PP_FP_QUAL_TYPE     qual_type = info->val[SOC_PB_PP_FP_CONTROL_TYPE_HDR_USER_DEF_PARTIAL_QUAL_NDX];
		if (qual_type >= SOC_PB_PP_NOF_FP_QUAL_TYPES) {
			SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FP_VAL_NDX_OUT_OF_RANGE_ERR, 86, exit);
		} else if (SOC_PB_PP_FP_QUAL_HDR_USER_DEF_0 <= qual_type && qual_type <= SOC_PB_PP_FP_QUAL_HDR_USER_DEF_9) {
			/* 
			 * user-defining a qualifier from a user-defined qualifier doesn't make much sense...
			 */
			SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FP_VAL_NDX_OUT_OF_RANGE_ERR, 87, exit);
		} else {
			int lsb, msb;
			uint8 found = soc_pb_pp_fp_key_qual_info_find(unit,qual_type,&qual_info);
			if (!found) {
				SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FP_VAL_NDX_OUT_OF_RANGE_ERR, 88, exit);
			}
			msb = qual_info.msb;
			lsb = qual_info.lsb;
			qual_info.qual_type    = qual_type;
			qual_info.lsb          -= info->val[SOC_PB_PP_FP_CONTROL_TYPE_HDR_USER_DEF_OFFSET_NDX];
			qual_info.msb          = qual_info.lsb - info->val[SOC_PB_PP_FP_CONTROL_TYPE_HDR_USER_DEF_NOF_BITS_NDX] + 1;
			if (qual_info.msb > qual_info.lsb || 
				qual_info.msb < msb || qual_info.msb > lsb ||
				qual_info.lsb < msb || qual_info.lsb > lsb) {
				SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FP_VAL_NDX_OUT_OF_RANGE_ERR, 89, exit);
			}
		}
		
	}

    res = soc_pb_pp_sw_db_fp_db_udf_set(
            unit,
            control_ndx->val_ndx,
            &qual_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);
    break;

  case SOC_PB_PP_FP_CONTROL_TYPE_ETHERTYPE:
    if (info->val[0] == FALSE)
    {
      res = soc_pb_pp_l2_next_prtcl_type_deallocate(
              unit,
              control_ndx->val_ndx
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);
    }
    else
    {
      res = soc_pb_pp_l2_next_prtcl_type_allocate(
              unit,
              control_ndx->val_ndx,
              &internal_ndx,
              success
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 92, exit);
    }
    break;

  case SOC_PB_PP_FP_CONTROL_TYPE_NEXT_PROTOCOL_IP:
    if (info->val[0] == FALSE)
    {
      res = soc_pb_pp_l3_next_protocol_remove(
              unit,
              (uint8) control_ndx->val_ndx
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);
    }
    else
    {
      res = soc_pb_pp_l3_next_protocol_add(
              unit,
              (uint8) (control_ndx->val_ndx),
              &internal_ndx8,
              success
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 102, exit);
    }
    break;

  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FP_TYPE_OUT_OF_RANGE_ERR, 1000, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_control_set_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_fp_control_set_verify(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PB_PP_FP_CONTROL_INDEX       *control_ndx,
    SOC_SAND_IN  SOC_PB_PP_FP_CONTROL_INFO        *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_CONTROL_SET_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_FP_CONTROL_INDEX, control_ndx, 10, exit);
  res = SOC_PB_PP_FP_CONTROL_INFO_verify(
          control_ndx->type,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_control_set_verify()", 0, 0);
}

uint32
  soc_pb_pp_fp_control_get_verify(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PB_PP_FP_CONTROL_INDEX       *control_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_CONTROL_GET_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_FP_CONTROL_INDEX, control_ndx, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_control_get_verify()", 0, 0);
}

/*********************************************************************
*     Set one of the control options.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_fp_control_get_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PB_PP_FP_CONTROL_INDEX       *control_ndx,
    SOC_SAND_OUT SOC_PB_PP_FP_CONTROL_INFO        *info
  )
{
  uint32
    reg_ndx,
    fld_ndx,
    internal_ndx,
    res = SOC_SAND_OK;
  SOC_PB_IHB_SRC_DEST_PORT_FOR_L3_ACL_KEY_TBL_DATA
    src_dest_port_for_l3_acl_key_tbl;
  SOC_PB_PP_REGS
    *pp_regs;
  SOC_PB_EGR_ACL_PORT_INFO
    acl_port_info;
  SOC_PB_PP_FP_BASE_HEADER_TYPE
    base_header_type;
  uint8
    found;
  uint8
    internal_ndx8;
  SOC_PB_EGR_ACL_DP_VALUES
    dp_val;
  SOC_PB_PP_FP_KEY_QUAL_INFO
    qual_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_CONTROL_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(control_ndx);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PB_PP_FP_CONTROL_INFO_clear(info);

  pp_regs = soc_pb_pp_regs();

  switch (control_ndx->type)
  {
  case SOC_PB_PP_FP_CONTROL_TYPE_L4OPS_RANGE:
    res = soc_pb_ihb_src_dest_port_for_l3_acl_key_tbl_get_unsafe(
            unit,
            control_ndx->val_ndx,
            &src_dest_port_for_l3_acl_key_tbl
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    info->val[0] = src_dest_port_for_l3_acl_key_tbl.source_port_min;
    info->val[1] = src_dest_port_for_l3_acl_key_tbl.source_port_max;
    info->val[2] = src_dest_port_for_l3_acl_key_tbl.destination_port_min;
    info->val[3] = src_dest_port_for_l3_acl_key_tbl.destination_port_max;
    break;

  case SOC_PB_PP_FP_CONTROL_TYPE_PACKET_SIZE_RANGE:
    SOC_PB_PP_FLD_GET(pp_regs->ihb.packet_header_size_range_reg[control_ndx->val_ndx].pkt_hdr_size_range_low, info->val[0], 40, exit);
    SOC_PB_PP_FLD_GET(pp_regs->ihb.packet_header_size_range_reg[control_ndx->val_ndx].pkt_hdr_size_range_high, info->val[1], 42, exit);
    info->val[0] ++;
    info->val[1] ++;
    break;


  case SOC_PB_PP_FP_CONTROL_TYPE_L2_L3_KEY_IN_LIF_ENABLE:
    SOC_PB_PP_FLD_GET(pp_regs->ihb.pmfgeneral_configuration1_reg.fixed_keys_use_ac, info->val[0], 50, exit);
    break;

  case SOC_PB_PP_FP_CONTROL_TYPE_L3_IPV6_TCP_CTL_ENABLE:
    SOC_PB_PP_FLD_GET(pp_regs->ihb.pmfgeneral_configuration1_reg.fixed_keys_use_tcp_control, info->val[0], 60, exit);
    break;

  case SOC_PB_PP_FP_CONTROL_TYPE_EGR_PP_PORT_DATA:
  case SOC_PB_PP_FP_CONTROL_TYPE_PP_PORT_PROFILE:
    SOC_PB_EGR_ACL_PORT_INFO_clear(&acl_port_info);
    res = soc_pb_egr_acl_port_get_unsafe(
            unit,
            control_ndx->val_ndx,
            &acl_port_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

    if (control_ndx->type == SOC_PB_PP_FP_CONTROL_TYPE_EGR_PP_PORT_DATA)
    {
      info->val[0] = acl_port_info.acl_data;
    }
    else /* SOC_PB_PP_FP_CONTROL_TYPE_PP_PORT_PROFILE */
    {
      info->val[0] = acl_port_info.profile;
    }
    break;

  case SOC_PB_PP_FP_CONTROL_TYPE_EGR_L2_ETHERTYPES:
    reg_ndx = (control_ndx->val_ndx == 8)? 0 : ((control_ndx->val_ndx - 1) / 2 - 3); /* 8 to 0, the 9 & 10 to 1, etc.*/
    fld_ndx = (control_ndx->val_ndx == 8)? 0 : (control_ndx->val_ndx + 1) % 2; /* 9 & 8 to 0, the  10 to 1, etc.*/
    SOC_PB_PP_FLD_GET(pp_regs->egq.ethernet_types_reg[reg_ndx].eth_type[fld_ndx], info->val[0], 75, exit);
    break;


  case SOC_PB_PP_FP_CONTROL_TYPE_EGR_IPV4_NEXT_PROTOCOL:
    res = soc_pb_egr_acl_l4_protocol_code_get_unsafe(
            unit,
            control_ndx->val_ndx,
            &(info->val[0])
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
    break;

  case SOC_PB_PP_FP_CONTROL_TYPE_INNER_ETH_NOF_VLAN_TAGS:
    info->val[0] = soc_pb_pp_sw_db_fp_inner_eth_nof_tags_get(
                     unit,
                     control_ndx->val_ndx
                  );
    break;

  case SOC_PB_PP_FP_CONTROL_TYPE_KEY_CHANGE_SIZE:
    info->val[0] = soc_pb_pp_sw_db_fp_key_change_size_get(unit);
    break;

  case SOC_PB_PP_FP_CONTROL_TYPE_EGRESS_DP:
    SOC_PB_EGR_ACL_DP_VALUES_clear(&dp_val);
    res = soc_pb_egr_acl_dp_values_get_unsafe(
            unit,
            &dp_val
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 85, exit);

    info->val[0] = dp_val.val[control_ndx->val_ndx];
    break;

  case SOC_PB_PP_FP_CONTROL_TYPE_HDR_USER_DEF:
    /*
     * Build the instruction
     */
    res = soc_pb_pp_sw_db_fp_db_udf_get(
            unit,
            control_ndx->val_ndx,
            &qual_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

	if (SOC_PB_PP_FP_QUAL_HDR_USER_DEF_0 <= qual_info.qual_type && qual_info.qual_type <= SOC_PB_PP_FP_QUAL_HDR_USER_DEF_9) {
		/* User-defined qualifier built from packet headers */
		switch (qual_info.header_ndx_0)
		{
		case SOC_PB_PMF_CE_SUB_HEADER_0:
		  base_header_type = SOC_PB_PP_FP_BASE_HEADER_TYPE_HEADER_0;
		  break;
		case SOC_PB_PMF_CE_SUB_HEADER_1:
		  base_header_type = SOC_PB_PP_FP_BASE_HEADER_TYPE_HEADER_1;
		  break;
		case SOC_PB_PMF_CE_SUB_HEADER_2:
		  base_header_type = SOC_PB_PP_FP_BASE_HEADER_TYPE_HEADER_2;
		  break;
		case SOC_PB_PMF_CE_SUB_HEADER_3:
		  base_header_type = SOC_PB_PMF_CE_SUB_HEADER_3;
		  break;
		case SOC_PB_PMF_CE_SUB_HEADER_4:
		  base_header_type = SOC_PB_PP_FP_BASE_HEADER_TYPE_HEADER_4;
		  break;
		case SOC_PB_PMF_CE_SUB_HEADER_5:
		  base_header_type = SOC_PB_PP_FP_BASE_HEADER_TYPE_HEADER_5;
		  break;
		case SOC_PB_PMF_CE_SUB_HEADER_FWD:
		  base_header_type = SOC_PB_PP_FP_BASE_HEADER_TYPE_FWD;
		  break;
		case SOC_PB_PMF_CE_SUB_HEADER_FWD_POST:
		  base_header_type = SOC_PB_PP_FP_BASE_HEADER_TYPE_FWD_POST;
		  break;
		default:
		  SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FP_VAL_NDX_OUT_OF_RANGE_ERR, 82, exit);
		}
		info->val[SOC_PB_PP_FP_CONTROL_TYPE_HDR_USER_DEF_SUB_HEADER_NDX] = base_header_type;
		info->val[SOC_PB_PP_FP_CONTROL_TYPE_HDR_USER_DEF_OFFSET_NDX] = qual_info.msb;
		info->val[SOC_PB_PP_FP_CONTROL_TYPE_HDR_USER_DEF_NOF_BITS_NDX] = qual_info.lsb - qual_info.msb + 1;
	} else {
		/* User-defined qualifier built from predefined qualifier */
		uint8 found;
		SOC_PB_PP_FP_KEY_QUAL_INFO predef_qual_info;
		info->val[SOC_PB_PP_FP_CONTROL_TYPE_HDR_USER_DEF_PARTIAL_QUAL_NDX] = qual_info.qual_type;
		found = soc_pb_pp_fp_key_qual_info_find(unit,qual_info.qual_type,&predef_qual_info);
		if (!found) {
			SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FP_VAL_NDX_OUT_OF_RANGE_ERR, 83, exit);
		} else {			
			info->val[SOC_PB_PP_FP_CONTROL_TYPE_HDR_USER_DEF_OFFSET_NDX] = predef_qual_info.lsb - qual_info.lsb;
			info->val[SOC_PB_PP_FP_CONTROL_TYPE_HDR_USER_DEF_NOF_BITS_NDX] = qual_info.lsb - qual_info.msb + 1;
			info->val[SOC_PB_PP_FP_CONTROL_TYPE_HDR_USER_DEF_SUB_HEADER_NDX] = SOC_PB_NOF_PMF_CE_SUB_HEADERS;
		}
	}
    break;

  case SOC_PB_PP_FP_CONTROL_TYPE_ETHERTYPE:
    res = soc_pb_pp_l2_next_prtcl_type_find(
            unit,
            control_ndx->val_ndx,
            &internal_ndx,
            &found
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);
    info->val[0] = found;
    info->val[1] = internal_ndx;
    break;

  case SOC_PB_PP_FP_CONTROL_TYPE_NEXT_PROTOCOL_IP:
    res = soc_pb_pp_l3_next_protocol_find(
            unit,
            (uint8) (control_ndx->val_ndx),
            &internal_ndx8,
            &found
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);
    info->val[0] = found;
    info->val[1] = internal_ndx8;
    break;

  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FP_TYPE_OUT_OF_RANGE_ERR, 60, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_control_get_unsafe()", 0, 0);
}

/*********************************************************************
*     Set the mapping between the Packet forward type and the
 *     Port profile to the Database-ID.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_fp_egr_db_map_set_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PB_PP_FP_FWD_TYPE            fwd_type_ndx,
    SOC_SAND_IN  uint32                    port_profile_ndx,
    SOC_SAND_IN  uint32                     db_id
  )
{
  uint32
    hw_egr_db,
    res = SOC_SAND_OK;
  SOC_SAND_SUCCESS_FAILURE
    success = SOC_SAND_SUCCESS;
  SOC_PB_PKT_FRWRD_TYPE
    fwd_type;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_EGR_DB_MAP_SET_UNSAFE);

  /*
   *  1. Get the HW DB-id
   */
  res = soc_pb_pp_fp_egr_db_id_manage_unsafe(
          unit,
          db_id,
          SOC_PB_PP_FP_EGR_MANAGE_TYPE_GET,
          &hw_egr_db,
          &success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  if (success != SOC_SAND_SUCCESS)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FP_EGR_DATABASE_NOT_ALREADY_ADDED_ERR, 20, exit);
  }

  /*
   *  2. Convert the enum and map it
   */
  res = soc_pb_pp_fp_egr_fwd_type_convert(
          unit,
          fwd_type_ndx,
          &fwd_type
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  /*
   *   3. Set it
   */
  res = soc_pb_egr_acl_key_profile_map_set_unsafe(
          unit,
          fwd_type,
          port_profile_ndx,
          hw_egr_db
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_egr_db_map_set_unsafe()", 0, port_profile_ndx);
}

uint32
  soc_pb_pp_fp_egr_db_map_set_verify(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PB_PP_FP_FWD_TYPE            fwd_type_ndx,
    SOC_SAND_IN  uint32                    port_profile_ndx,
    SOC_SAND_IN  uint32                     db_id
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_EGR_DB_MAP_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(fwd_type_ndx, SOC_PB_PP_FP_FWD_TYPE_NDX_MAX, SOC_PB_PP_FP_FWD_TYPE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(port_profile_ndx, SOC_PB_PP_FP_PORT_PROFILE_NDX_MAX, SOC_PB_PP_FP_PORT_PROFILE_NDX_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(db_id, SOC_PB_PP_FP_DB_ID_MAX, SOC_PB_PP_FP_DB_ID_OUT_OF_RANGE_ERR, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_egr_db_map_set_verify()", 0, port_profile_ndx);
}

uint32
  soc_pb_pp_fp_egr_db_map_get_verify(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PB_PP_FP_FWD_TYPE            fwd_type_ndx,
    SOC_SAND_IN  uint32                    port_profile_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_EGR_DB_MAP_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(fwd_type_ndx, SOC_PB_PP_FP_FWD_TYPE_NDX_MAX, SOC_PB_PP_FP_FWD_TYPE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(port_profile_ndx, SOC_PB_PP_FP_PORT_PROFILE_NDX_MAX, SOC_PB_PP_FP_PORT_PROFILE_NDX_OUT_OF_RANGE_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_egr_db_map_get_verify()", 0, port_profile_ndx);
}

/*********************************************************************
*     Set the mapping between the Packet forward type and the
 *     Port profile to the Database-ID.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_fp_egr_db_map_get_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PB_PP_FP_FWD_TYPE            fwd_type_ndx,
    SOC_SAND_IN  uint32                    port_profile_ndx,
    SOC_SAND_OUT uint32                     *db_id
  )
{
  uint32
    hw_egr_db,
    res = SOC_SAND_OK;
  SOC_PB_PKT_FRWRD_TYPE
    fwd_type;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_EGR_DB_MAP_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(db_id);

  /*
   *  1. Convert the enum
   */
  res = soc_pb_pp_fp_egr_fwd_type_convert(
          unit,
          fwd_type_ndx,
          &fwd_type
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  /*
   * 2. Get the Mapping
   */
  res = soc_pb_egr_acl_key_profile_map_get_unsafe(
          unit,
          fwd_type,
          port_profile_ndx,
          &hw_egr_db
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*
   *  3. Return the SW DB mapping (a not existing DB-Id has an invalid ID [NOF])
   */
  *db_id = soc_pb_sw_db_egr_acl_sw_db_id_get(unit, hw_egr_db);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_egr_db_map_get_unsafe()", 0, port_profile_ndx);
}

/*********************************************************************
*     Get the Field Processing of the last packets.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_fp_packet_diag_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PB_PP_FP_PACKET_DIAG_INFO *info
  )
{
  uint32
    db_key_lsb,
    fem_input,
    is_dt_bit_meaningful,
    entry_dt_first,
    key_buff[SOC_PB_PMF_DIAG_BUFF_MAX_SIZE],
    tcam_action_val,
    action_size_in_bits,
    action_size_in_bits_in_fem,
    action_lsb,
    qual_lsb,
    qual_length_no_padding,
    db_id_curr,
    pmf_pgm_ndx,
    pmf_progs_bmp,
    key_lsb_cycle_0[SOC_PB_PMF_LOW_LEVEL_PMF_KEY_MAX+1],
    key_val_dt,
    pfc,
    lookup_profile,
    val[SOC_PB_PP_DIAG_DBG_VAL_LEN],
    res = SOC_SAND_OK;
  SOC_PB_PP_DIAG_RECEIVED_PACKET_INFO
    rcvd_pkt_info;
  SOC_PB_PP_DIAG_REG_FIELD
    reg_fld;
  uint32
    selector_ndx,
    fem_macro_ndx,
    tcam_db_id,
    entry_id_ndx,
    action_type_ndx,
    cycle_ndx,
    ce_instruction_ndx,
    bank_ndx,
    key_ndx,
    qual_type_ndx,
    db_id_ndx,
    hdr_type_ndx = 0;
  SOC_PB_PMF_PGM_PKT_HDR_TYPE
    hdr_type = SOC_PB_PMF_PGM_NOF_PKT_HDR_TYPES;
  SOC_PB_SW_DB_PMF_PGM_PROFILE
    pgm_profile;
  SOC_PB_PMF_DIAG_KEYS_INFO
    built_keys;
  SOC_PB_PP_FP_DATABASE_INFO
    fp_database_info;
  SOC_PB_PP_IHB_TCAM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA
    tcam_key_profile_resolved_data_tbl;
  SOC_PB_PMF_LKP_PROFILE
    lkp_profile_ndx;
  SOC_PB_PMF_DIRECT_TBL_KEY_SRC
    dt_key_src;
  SOC_PB_SW_DB_PMF_CE
    ce_instr;
  uint8
    cycle_db_id,
    is_found;
  SOC_PB_PP_FP_ENTRY_INFO
    *fp_entry_info = NULL;
  SOC_PB_PP_IHB_DIRECT_ACTION_TABLE_TBL_DATA
    direct_action_table_tbl;
  SOC_PB_PMF_FEM_NDX
    fem_ndx;
  SOC_PB_PP_FP_FEM_ENTRY
    fem_entry;
  SOC_PB_PP_FP_DIR_EXTR_ACTION_VAL
    dir_extr_action_val;
  SOC_PB_PP_FP_QUAL_VAL
    qual_val;
  SOC_PB_PMF_DIAG_FEM_INFO
    fems_info[SOC_PB_PP_FP_NOF_MACROS];
  SOC_PB_PMF_DIAG_FEM_FREEZE_INFO
    freeze_info;
  SOC_PB_PP_FP_KEY_DESC
    fp_key_desc;
  SOC_PB_PP_FP_KEY_LOCATION
    key_loc;
  SOC_PB_PMF_FEM_INPUT_INFO
    fem_input_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_PACKET_DIAG_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PB_PP_FP_PACKET_DIAG_INFO_clear(info);

  /*
   * 1. Parser
   */
  SOC_PB_PP_DIAG_RECEIVED_PACKET_INFO_clear(&rcvd_pkt_info);
  res = soc_pb_pp_diag_received_packet_info_get_unsafe(
          unit,
          &rcvd_pkt_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  info->parser.tm_port = rcvd_pkt_info.in_tm_port;
  info->parser.pp_port = rcvd_pkt_info.in_pp_port;
  info->parser.header_type = rcvd_pkt_info.pp_context;

  /* Get the Packet format code */
  sal_memset(val,0x0,SOC_PB_PP_DIAG_DBG_VAL_LEN * 4);
  reg_fld.msb = 241;
  reg_fld.lsb = 236;
  reg_fld.addr.base = 393220;
  reg_fld.addr.step = 0;
  res = soc_pb_pp_diag_dbg_val_get_unsafe(
          unit,
          21,
          &reg_fld,
          val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  for (hdr_type_ndx = 0; hdr_type_ndx < SOC_PB_PMF_PGM_NOF_PKT_HDR_TYPES; hdr_type_ndx++)
  {
    hdr_type = 1 << hdr_type_ndx;
    res = soc_pb_pmf_pgm_packet_format_code_convert(
            unit,
            hdr_type,
            &pfc,
            &lookup_profile
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    if (pfc == val[0])
    {
      break;
    }
  }
  info->parser.hdr_format = hdr_type;

  /* Get the VLAN Structure */
  sal_memset(val,0x0,SOC_PB_PP_DIAG_DBG_VAL_LEN * 4);
  reg_fld.msb = 217;
  reg_fld.lsb = 214;
  reg_fld.addr.base = 393221;
  reg_fld.addr.step = 0;
  res = soc_pb_pp_diag_dbg_val_get_unsafe(
          unit,
          21,
          &reg_fld,
          val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  info->parser.vlan_tag_structure = val[0];

  /*
   * 2. PMF-Program
   */
  /* Get the PMF-Program index, assumption of a single program */
  res = soc_pb_pmf_diag_selected_progs_get(
          unit,
          &pmf_progs_bmp
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  sal_msleep(1100);

  res = soc_pb_pmf_diag_selected_progs_get(
          unit,
          &pmf_progs_bmp
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  for (pmf_pgm_ndx = 0; pmf_pgm_ndx <= SOC_PB_PMF_LOW_LEVEL_PMF_PGM_NDX_MAX + 1; ++pmf_pgm_ndx)
  {
    if (SOC_SAND_GET_BIT(pmf_progs_bmp, pmf_pgm_ndx) == 0x1)
    {
      /* PMF Program found */
      break;
    }
  }
  /* Get the PMF Program attributes */
  soc_pb_sw_db_pgm_profile_get(
    unit,
    pmf_pgm_ndx,
    &pgm_profile
  );
  info->pgm.pfg_id[SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF] = pgm_profile.pfg_ndx; /* Possible improvement to take another PFG */
  info->pgm.pgm_id[SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF] = pmf_pgm_ndx; 

  if (pgm_profile.pfg_ndx >= SOC_PB_PP_FP_NOF_PFGS)
  {
    SOC_PB_PP_DO_NOTHING_AND_EXIT;
  }

  /*
   * Decode the PP port info
   */
  res = soc_pb_pmf_low_level_pgm_port_profile_decode(
          unit,
          pgm_profile.pp_port_info_encoded,
          FALSE,
          &(info->pgm.pp_port_info),
          &(info->pgm.pfg_id[SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

  /*
   * Compute the CE offsets for both keys (A and B)
   */
  key_lsb_cycle_0[0] = 0;
  key_lsb_cycle_0[1] = 0;
  for (key_ndx = 0; key_ndx < SOC_PB_PMF_LOW_LEVEL_PMF_KEY_MAX+1; ++key_ndx)
  {
    for (ce_instruction_ndx = 4; ce_instruction_ndx < SOC_PB_PMF_LOW_LEVEL_CE_NDX_MAX+1; ce_instruction_ndx ++)
    {
      soc_pb_sw_db_pgm_ce_instr_get(
        unit,
        pgm_profile.pfg_ndx,
        key_ndx,
        ce_instruction_ndx,
        &ce_instr
        );
      if (ce_instr.is_valid == TRUE)
      {
        key_lsb_cycle_0[key_ndx] += ce_instr.bit_count + 1;
      }
    }
  }

  /*
   * 3. CE
   */
  SOC_PB_PMF_DIAG_KEYS_INFO_clear(&built_keys);
  res = soc_pb_pmf_diag_built_keys_get(
          unit,
          &built_keys
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  db_id_curr = 0;
  for (db_id_ndx = 0; db_id_ndx < SOC_PB_PP_FP_NOF_DBS; ++db_id_ndx)
  {
    /*
     * Currently, get the DB info, and if this PFG is in it, consider it
     */
    SOC_PB_PP_FP_DATABASE_INFO_clear(&fp_database_info);
    res = soc_pb_pp_fp_database_get_unsafe(
            unit,
            db_id_ndx,
            &fp_database_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

    if ((fp_database_info.supported_pfgs & (1 << pgm_profile.pfg_ndx)) == 0)
    {
      continue;
    }

    /*
     * Get the Database cycle through SW DB
     */
    cycle_db_id = soc_pb_pp_sw_db_fp_db_id_cycle_get(unit, db_id_ndx);

    /*
     * Get the qualifier places locally and the DB place
     */
    SOC_PB_PP_FP_KEY_DESC_clear(&fp_key_desc);
    res = soc_pb_pp_fp_key_desc_get_unsafe(
            unit,
            db_id_ndx,
            &fp_key_desc
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

    res = soc_pb_pp_sw_db_fp_db_key_location_get(
            unit,
            db_id_ndx,
            pgm_profile.pfg_ndx,
            &key_loc
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

    db_key_lsb = 0;
    if (fp_database_info.db_type == SOC_PB_PP_FP_DB_TYPE_TCAM)
    {
      switch (key_loc.tcam_src)
      {
      case SOC_PB_PMF_TCAM_KEY_SRC_L2:
        SOC_PETRA_COPY(key_buff, built_keys.eth.buff, uint32, SOC_PB_PMF_DIAG_BUFF_MAX_SIZE);
      	break;
      case SOC_PB_PMF_TCAM_KEY_SRC_L3_IPV4:
        SOC_PETRA_COPY(key_buff, built_keys.ipv4.buff, uint32, SOC_PB_PMF_DIAG_BUFF_MAX_SIZE);
        break;
      case SOC_PB_PMF_TCAM_KEY_SRC_L3_IPV6:
        SOC_PETRA_COPY(key_buff, built_keys.ipv6.buff, uint32, SOC_PB_PMF_DIAG_BUFF_MAX_SIZE);
        break;
      case SOC_PB_PMF_TCAM_KEY_SRC_B_A:
        /* Copy the 4.5 remaining longs from key A & B */
        res = soc_sand_bitstream_set_any_field(
                built_keys.custom_a.buff,
                0,
                SOC_PB_TCAM_BANK_ENTRY_SIZE_144_BITS_SIZE_IN_BITS,
                key_buff
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 48, exit);

        res = soc_sand_bitstream_set_any_field(
                built_keys.custom_b.buff,
                SOC_PB_TCAM_BANK_ENTRY_SIZE_144_BITS_SIZE_IN_BITS,
                SOC_PB_TCAM_BANK_ENTRY_SIZE_144_BITS_SIZE_IN_BITS,
                key_buff
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

        break;
      case SOC_PB_PMF_TCAM_KEY_SRC_A_71_0:
      case SOC_PB_PMF_TCAM_KEY_SRC_A_103_32:
      case SOC_PB_PMF_TCAM_KEY_SRC_A_143_0:
      case SOC_PB_PMF_TCAM_KEY_SRC_A_175_32:
        SOC_PETRA_COPY(key_buff, built_keys.custom_a.buff, uint32, SOC_PB_PMF_DIAG_BUFF_MAX_SIZE);
        db_key_lsb = ((key_loc.tcam_src == SOC_PB_PMF_TCAM_KEY_SRC_A_103_32) || (key_loc.tcam_src == SOC_PB_PMF_TCAM_KEY_SRC_A_175_32))? 32 : 0;
        db_key_lsb += (cycle_db_id == 0)? key_lsb_cycle_0[0]: 0;
        break;
      case SOC_PB_PMF_TCAM_KEY_SRC_B_71_0:
      case SOC_PB_PMF_TCAM_KEY_SRC_B_103_32:
      case SOC_PB_PMF_TCAM_KEY_SRC_B_143_0:
      case SOC_PB_PMF_TCAM_KEY_SRC_B_175_32:
        SOC_PETRA_COPY(key_buff, built_keys.custom_b.buff, uint32, SOC_PB_PMF_DIAG_BUFF_MAX_SIZE);
        db_key_lsb = ((key_loc.tcam_src == SOC_PB_PMF_TCAM_KEY_SRC_B_103_32) || (key_loc.tcam_src == SOC_PB_PMF_TCAM_KEY_SRC_B_175_32))? 32 : 0;
        db_key_lsb += (cycle_db_id == 0)? key_lsb_cycle_0[1]: 0;
        break;
      default:
        break;
      }
    }
    else if (fp_database_info.db_type == SOC_PB_PP_FP_DB_TYPE_DIRECT_TABLE)
    {
      db_key_lsb = (key_loc.dir_tbl_src % 4) * 10;
      if (key_loc.dir_tbl_src < SOC_PB_PMF_DIRECT_TBL_KEY_SRC_B_9_0)
      {
        SOC_PETRA_COPY(key_buff, built_keys.custom_a.buff, uint32, SOC_PB_PMF_DIAG_BUFF_MAX_SIZE);
        db_key_lsb += (cycle_db_id == 0)? key_lsb_cycle_0[0]: 0;
      }
      else
      {
        SOC_PETRA_COPY(key_buff, built_keys.custom_b.buff, uint32, SOC_PB_PMF_DIAG_BUFF_MAX_SIZE);
        db_key_lsb += (cycle_db_id == 0)? key_lsb_cycle_0[1]: 0;
      }
    }
    else /* Direct Extraction */
    {
      if (key_loc.fem_src < SOC_PB_PMF_FEM_INPUT_SRC_B_31_0)
      {
        SOC_PETRA_COPY(key_buff, built_keys.custom_a.buff, uint32, SOC_PB_PMF_DIAG_BUFF_MAX_SIZE);
      }
      else if (key_loc.fem_src <= SOC_PB_PMF_FEM_INPUT_SRC_B_95_64)
      {
        SOC_PETRA_COPY(key_buff, built_keys.custom_b.buff, uint32, SOC_PB_PMF_DIAG_BUFF_MAX_SIZE);
      }
      db_key_lsb = (key_loc.dir_tbl_src % 5) * 16;
    }

    info->key.db_id_quals[db_id_curr].db_id = db_id_ndx;
    info->key.db_id_quals[db_id_curr].stage = SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF; /* Only Ingress in Soc_petra-B */
    for (qual_type_ndx = 0; qual_type_ndx < SOC_PB_PP_FP_NOF_QUALS_PER_DB_MAX; ++qual_type_ndx)
    {
      info->key.db_id_quals[db_id_curr].qual[qual_type_ndx].type = fp_database_info.qual_types[qual_type_ndx];
      res = soc_pb_pp_fp_qual_lsb_and_length_get(
              unit,
              db_id_ndx,
              fp_database_info.qual_types[qual_type_ndx],
              &fp_database_info,
              &qual_lsb,
              &qual_length_no_padding
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 52, exit);

      /*
       * Extract the value
       */
      res = soc_sand_bitstream_get_any_field(
              key_buff,
              db_key_lsb + qual_lsb,
              qual_length_no_padding,
              info->key.db_id_quals[db_id_curr].qual[qual_type_ndx].val
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 54, exit);
    }
    db_id_curr ++;
  }


  /*
   * 4. TCAM
   */
  for (cycle_ndx = 0; cycle_ndx < SOC_PB_PMF_NOF_CYCLES; ++cycle_ndx)
  {
    res = soc_pb_pp_ihb_tcam_key_profile_resolved_data_tbl_get_unsafe(
            unit,
            pgm_profile.pfg_ndx,
            cycle_ndx,
            &tcam_key_profile_resolved_data_tbl
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

    for (bank_ndx = 0; bank_ndx < SOC_PB_TCAM_NOF_BANKS; ++bank_ndx)
    {
      /* Get if match: verify before if the TCAM bank is part of the lookup */
      if (
          (tcam_key_profile_resolved_data_tbl.bank_key_select[bank_ndx] < SOC_PB_PMF_TCAM_KEY_SRC_L2_FLD_VAL)
          || ((tcam_key_profile_resolved_data_tbl.bank_key_select[bank_ndx] > SOC_PB_PMF_TCAM_KEY_SRC_L3_IPV6_FLD_VAL)
               && (tcam_key_profile_resolved_data_tbl.bank_key_select[bank_ndx] < SOC_PB_PMF_TCAM_KEY_SRC_B_A_FLD_VAL))
         )
      {
        info->tcam[cycle_ndx][bank_ndx].is_match = FALSE;
        continue;
      }
      /*
       * Search the first selector with this bank
       */
      if (SOC_SAND_GET_BIT(tcam_key_profile_resolved_data_tbl.tcam_pd1_members, bank_ndx) == 0x1)
      {
        selector_ndx = 0;
      }
      else if (SOC_SAND_GET_BIT(tcam_key_profile_resolved_data_tbl.tcam_pd2_members, bank_ndx) == 0x1)
      {
        selector_ndx = 1;
      }
      else if (SOC_SAND_GET_BIT(tcam_key_profile_resolved_data_tbl.tcam_sel3_member, bank_ndx) == 0x1)
      {
        selector_ndx = 2;
      }
      else /* Sel4 */
      {
        selector_ndx = 3;
      }
	  
      /* Get the signal */
      sal_memset(val,0x0,SOC_PB_PP_DIAG_DBG_VAL_LEN * 4);
      if (cycle_ndx == 0)
      {
        reg_fld.msb = 7 - selector_ndx;
        reg_fld.lsb = 7 - selector_ndx;
      }
      else
      {
        reg_fld.msb = 131 - selector_ndx;
        reg_fld.lsb = 131 - selector_ndx;
      }
      reg_fld.addr.base = 720897 - cycle_ndx; /* 720896 for cycle 1*/
      reg_fld.addr.step = 0;
      res = soc_pb_pp_diag_dbg_val_get_unsafe(
              unit,
              21,
              &reg_fld,
              val
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 52, exit);

      if (val[0] != 0)
      {
        info->tcam[cycle_ndx][bank_ndx].is_match = TRUE;
        /* Get the DB-Id */
        res = soc_pb_tcam_managed_bank_accessed_db_get_unsafe(
                unit,
                bank_ndx,
                cycle_ndx,
                &tcam_db_id
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 54, exit);

        info->tcam[cycle_ndx][bank_ndx].db_id = soc_pb_egr_fp_db_id_from_tcam_db_id_get(tcam_db_id);
        SOC_PB_PP_FP_DATABASE_INFO_clear(&fp_database_info);
        res = soc_pb_pp_fp_database_get_unsafe(
                unit,
                info->tcam[cycle_ndx][bank_ndx].db_id,
                &fp_database_info
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 54, exit);


        /* Get the Action values */
        sal_memset(val,0x0,SOC_PB_PP_DIAG_DBG_VAL_LEN * 4);
        if (cycle_ndx == 0)
        {
          reg_fld.msb = SOC_SAND_MIN(259 - (selector_ndx * 32), 255);
          reg_fld.lsb = 228 - (selector_ndx * 32);
        }
        else
        {
          reg_fld.msb = 127 - (selector_ndx * 32);
          reg_fld.lsb = 96 - (selector_ndx * 32);
        }
        reg_fld.addr.base = 720896;
        reg_fld.addr.step = 0;
        res = soc_pb_pp_diag_dbg_val_get_unsafe(
                unit,
                21,
                &reg_fld,
                val
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 56, exit);
        tcam_action_val = val[0];

        /* Special correction for cycle 0 bank 0 (pd1) */
        if ((selector_ndx == 0) && (cycle_ndx == 0))
        {
          sal_memset(val,0x0,SOC_PB_PP_DIAG_DBG_VAL_LEN * 4);
          reg_fld.msb = 3;
          reg_fld.lsb = 0;
          reg_fld.addr.base = 720897;
          reg_fld.addr.step = 0;
          res = soc_pb_pp_diag_dbg_val_get_unsafe(
                  unit,
                  21,
                  &reg_fld,
                  val
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 56, exit);
          tcam_action_val = tcam_action_val + (val[0] << 27);
        }

        /* Extract each action type and val */
        for (action_type_ndx = 0; action_type_ndx < SOC_PB_PP_FP_NOF_ACTIONS_PER_DB_MAX; ++action_type_ndx)
        {
          info->tcam[cycle_ndx][bank_ndx].actions[action_type_ndx].type = fp_database_info.action_types[action_type_ndx];
          if (fp_database_info.action_types[action_type_ndx] == SOC_PB_PP_NOF_FP_ACTION_TYPES)
          {
            continue;
          }

          res = soc_pb_pp_fp_action_lsb_get(
                  unit,
                  fp_database_info.action_types[action_type_ndx],
                  &fp_database_info,
                  &action_lsb
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 58, exit);

          res = soc_pb_pp_fp_action_type_max_size_get(
                  unit,
                  fp_database_info.action_types[action_type_ndx],
                  &action_size_in_bits,
                  &action_size_in_bits_in_fem
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

          info->tcam[cycle_ndx][bank_ndx].actions[action_type_ndx].val =
            SOC_SAND_GET_BITS_RANGE(tcam_action_val, action_lsb + action_size_in_bits - 1, action_lsb);
        }
      }
      else
      {
        info->tcam[cycle_ndx][bank_ndx].is_match = FALSE;
      }
    }
  }

  /*
   * 5. Direct Table - Take cycle
   */
  SOC_PB_PMF_LKP_PROFILE_clear(&lkp_profile_ndx);
  /* Look if there is a valid Database ID for this PFG */
  info->dt[0].is_match = FALSE;
  info->dt[1].is_match = FALSE;
  for (db_id_ndx = 0; db_id_ndx < SOC_PB_PP_FP_NOF_DBS; ++db_id_ndx)
  {
    SOC_PB_PP_FP_DATABASE_INFO_clear(&fp_database_info);
    res = soc_pb_pp_fp_database_get_unsafe(
            unit,
            db_id_ndx,
            &fp_database_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
    if (fp_database_info.db_type != SOC_PB_PP_FP_DB_TYPE_DIRECT_TABLE)
    {
      continue;
    }
    if ((fp_database_info.supported_pfgs & (1 << pgm_profile.pfg_ndx)) != 0)
    {
      info->dt[0].is_match = TRUE;
      info->dt[1].is_match = TRUE;
      info->dt[0].db_id = db_id_ndx;
      info->dt[1].db_id = db_id_ndx;
    }
    break;
  }

  if (info->dt[0].is_match == TRUE)
  {
    SOC_PETRA_ALLOC(fp_entry_info, SOC_PB_PP_FP_ENTRY_INFO, 1);
    for (cycle_ndx = 0; cycle_ndx < SOC_PB_PMF_NOF_CYCLES; ++cycle_ndx)
    {
      lkp_profile_ndx.cycle_ndx = cycle_ndx;
      lkp_profile_ndx.id = pgm_profile.pfg_ndx;
      res = soc_pb_pmf_db_direct_tbl_key_src_get_unsafe(
              unit,
              &lkp_profile_ndx,
              &dt_key_src
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

      /*
       * Get the Key looked up: for cycle 0 compute before
       * the lsb of CE instructions #3
       */
      key_ndx = (dt_key_src < SOC_PB_PMF_DIRECT_TBL_KEY_SRC_B_9_0_FLD_VAL)? 0 : 1;
      if (key_ndx == 0)
      {
        SOC_PETRA_COPY(key_buff, built_keys.custom_a.buff, uint32, SOC_PB_PMF_DIAG_BUFF_MAX_SIZE);
      }
      else
      {
        SOC_PETRA_COPY(key_buff, built_keys.custom_b.buff, uint32, SOC_PB_PMF_DIAG_BUFF_MAX_SIZE);
      }
      key_val_dt = 0;
      res = soc_sand_bitstream_get_any_field(
            key_buff,
            key_lsb_cycle_0[key_ndx] + ((dt_key_src % 4) * 10),
            10,
            &(key_val_dt)
          );
      SOC_SAND_CHECK_FUNC_RESULT(res, 72, exit);

      /* Look if an entry match */
      for (entry_id_ndx = 0; entry_id_ndx < SOC_PB_PP_FP_NOF_ENTRY_IDS; ++entry_id_ndx)
      {
        SOC_PB_PP_FP_ENTRY_INFO_clear(fp_entry_info);
        res = soc_pb_pp_fp_entry_get_unsafe(
                unit,
                info->dt[cycle_ndx].db_id,
                entry_id_ndx,
                &is_found,
                fp_entry_info
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 74, exit);

        if (is_found == FALSE)
        {
          continue;
        }
        res = soc_pb_pp_fp_entry_ndx_direct_table_get(
                unit,
                info->dt[cycle_ndx].db_id,
                fp_entry_info,
                &entry_dt_first,
                &is_dt_bit_meaningful
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 76, exit);
    
        if ((key_val_dt & is_dt_bit_meaningful) == (entry_dt_first & is_dt_bit_meaningful))
        {
          /* The entry is found */
          for (action_type_ndx = 0; action_type_ndx < SOC_PB_PP_FP_NOF_ACTIONS_PER_DB_MAX; ++action_type_ndx)
          {
            info->dt[cycle_ndx].actions[action_type_ndx].type = fp_database_info.action_types[action_type_ndx];
            if (fp_database_info.action_types[action_type_ndx] == SOC_PB_PP_NOF_FP_ACTION_TYPES)
            {
              continue;
            }

            res = soc_pb_pp_fp_action_lsb_get(
                    unit,
                    fp_database_info.action_types[action_type_ndx],
                    &fp_database_info,
                    &action_lsb
                  );
            SOC_SAND_CHECK_FUNC_RESULT(res, 78, exit);

            res = soc_pb_pp_fp_action_type_max_size_get(
                    unit,
                    fp_database_info.action_types[action_type_ndx],
                    &action_size_in_bits,
                    &action_size_in_bits_in_fem
                  );
            SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

            /* Get directly the entry value */
            res = soc_pb_pp_ihb_direct_action_table_tbl_get_unsafe(
                    unit,
                    key_val_dt,
                    &direct_action_table_tbl
                  );
            SOC_SAND_CHECK_FUNC_RESULT(res, 82, exit);

            info->dt[cycle_ndx].actions[action_type_ndx].val =
              SOC_SAND_GET_BITS_RANGE(direct_action_table_tbl.direct_action_table, action_lsb + action_size_in_bits - 1, action_lsb);
          } /* Action type */
          break;
        } /* DT entry matched */
      } /* DT entry id */
    } /* Cycle */
  } /* Database of this PFG */

  /*
   * 6. FEM
   */
  for (cycle_ndx = 0; cycle_ndx < SOC_PB_PMF_NOF_CYCLES; ++cycle_ndx)
  {
    /* Set the enable and wait 10 ms */
    SOC_PB_PMF_DIAG_FEM_FREEZE_INFO_clear(&freeze_info);
    res = soc_pb_pmf_diag_fem_freeze_get(
            unit,
            &freeze_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

    freeze_info.pass_num = cycle_ndx;
    res = soc_pb_pmf_diag_fem_freeze_set(
            unit,
            &freeze_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 92, exit);

    sal_msleep(1100);

    res = soc_pb_pmf_diag_fems_info_get(
            unit,
            FALSE,
            fems_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 94, exit);

    for (fem_macro_ndx = 0; fem_macro_ndx < SOC_PB_PP_FP_NOF_MACROS; ++fem_macro_ndx)
    {
      /*
       * Get the source
       */
      lkp_profile_ndx.cycle_ndx = cycle_ndx;
      lkp_profile_ndx.id = pgm_profile.pfg_ndx;
      SOC_PB_PMF_FEM_INPUT_INFO_clear(&fem_input_info);
      res = soc_pb_pmf_db_fem_input_get_unsafe(
              unit,
              &lkp_profile_ndx,
              fem_macro_ndx,
              &fem_input_info
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 95, exit);

      if ((fems_info[fem_macro_ndx].in_prg != SOC_PB_PMF_FEM_PGM_FOR_ETH)
           || (fem_input_info.src == SOC_PB_PMF_FEM_INPUT_SRC_NOP))
      {
        /* This FEM is skipped */
        continue;
      }

      /* Get the DB-Id [and Entry-Id] */
      SOC_PB_PMF_FEM_NDX_clear(&fem_ndx);
      fem_ndx.cycle_ndx = cycle_ndx;
      fem_ndx.id = fem_macro_ndx;
      SOC_PB_PP_FP_FEM_ENTRY_clear(&fem_entry);
      SOC_PB_PP_FP_DIR_EXTR_ACTION_VAL_clear(&dir_extr_action_val);
      SOC_PB_PP_FP_QUAL_VAL_clear(&qual_val);
      res = soc_pb_pp_fp_fem_configuration_de_get(
              unit,
              fem_macro_ndx,
              cycle_ndx,
              &fem_entry,
              &dir_extr_action_val,
              &qual_val
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 96, exit);

      fem_input = fems_info[fem_macro_ndx].in_key;

      info->macro[cycle_ndx][fem_macro_ndx].db_id = fem_entry.db_id;
      SOC_PB_PP_FP_DATABASE_INFO_clear(&fp_database_info);
      res = soc_pb_pp_fp_database_get_unsafe(
              unit,
              fem_entry.db_id,
              &fp_database_info
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 98, exit);

      if (fem_entry.is_for_entry == TRUE)
      {
        info->macro[cycle_ndx][fem_macro_ndx].entry_id = fem_entry.entry_id;
        /* Get the Qualifier mask in this case*/
        info->macro[cycle_ndx][fem_macro_ndx].qual_mask.type = qual_val.type;
        /* Extract the value from the input */
        res = soc_pb_pp_fp_qual_lsb_and_length_get(
                unit,
                fem_entry.db_id,
                qual_val.type,
                &fp_database_info,
                &qual_lsb,
                &qual_length_no_padding
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);
        info->macro[cycle_ndx][fem_macro_ndx].qual_mask.val[0] = SOC_SAND_GET_BITS_RANGE(fem_input, qual_lsb + qual_length_no_padding - 1, qual_lsb);
      }

      if (fems_info[fem_macro_ndx].out_action.is_valid == FALSE)
      {
        info->macro[cycle_ndx][fem_macro_ndx].action.type = SOC_PB_PP_FP_ACTION_TYPE_NOP;
        info->macro[cycle_ndx][fem_macro_ndx].action.val = 0;
        break;
      }

      /* Get the action */
      res = soc_pb_pp_fp_action_type_from_pmf_convert(
              unit,
              fems_info[fem_macro_ndx].out_action.type,
              &(info->macro[cycle_ndx][fem_macro_ndx].action.type)
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);
      info->macro[cycle_ndx][fem_macro_ndx].action.val = fems_info[fem_macro_ndx].out_action.value;
    }
  }

exit:
  SOC_PETRA_FREE(fp_entry_info);
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_packet_diag_get_unsafe()", 0, 0);
}

/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     soc_pb_pp_api_fp module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
SOC_PROCEDURE_DESC_ELEMENT*
  soc_pb_pp_fp_get_procs_ptr(void)
{
  return Soc_pb_pp_procedure_desc_element_fp;
}
/*********************************************************************
*     Get the pointer to the list of errors of the
 *     soc_pb_pp_api_fp module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
SOC_ERROR_DESC_ELEMENT*
  soc_pb_pp_fp_get_errs_ptr(void)
{
  return Soc_pb_pp_error_desc_element_fp;
}
uint32
  SOC_PB_PP_FP_QUAL_VAL_verify(
    SOC_SAND_IN  SOC_PB_PP_FP_QUAL_VAL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->type, SOC_PB_PP_NOF_FP_QUAL_TYPES, SOC_PB_PP_FP_QUAL_TYPES_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_FP_QUAL_VAL_verify()",0,0);
}

uint32
  SOC_PB_PP_FP_PFG_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_FP_PFG_INFO *info
  )
{
    uint32
      qual_ndx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  /* Different verification if array mode or not */
  if (!info->is_array_qualifier) {
      SOC_SAND_ERR_IF_ABOVE_MAX(info->hdr_format_bmp, SOC_PB_PP_FP_HDR_FORMAT_BMP_MAX, SOC_PB_PP_FP_HDR_FORMAT_BMP_OUT_OF_RANGE_ERR, 10, exit);
      SOC_SAND_ERR_IF_ABOVE_MAX(info->vlan_tag_structure_bmp, SOC_PB_PP_FP_VLAN_TAG_STRUCTURE_BMP_MAX, SOC_PB_PP_FP_VLAN_TAG_STRUCTURE_BMP_OUT_OF_RANGE_ERR, 11, exit);
  }
  else {
      /* Fill it according to the Parameters */
      for (qual_ndx = 0; qual_ndx < SOC_PPC_FP_NOF_QUALS_PER_PFG_MAX; qual_ndx++)
      {
        if (info->qual_vals[qual_ndx].type == SOC_PB_PP_NOF_FP_QUAL_TYPES)
        {
          continue;
        }

        switch (info->qual_vals[qual_ndx].type) {
        case SOC_PPC_FP_QUAL_IRPP_PKT_HDR_TYPE:
            SOC_SAND_ERR_IF_ABOVE_MAX(info->qual_vals[qual_ndx].val.arr[0], SOC_PB_PP_FP_HDR_FORMAT_BMP_MAX, SOC_PB_PP_FP_HDR_FORMAT_BMP_OUT_OF_RANGE_ERR, 20, exit);
            break;
        case SOC_PPC_FP_QUAL_IRPP_ETH_TAG_FORMAT:
            SOC_SAND_ERR_IF_ABOVE_MAX(info->qual_vals[qual_ndx].val.arr[0], SOC_PB_PP_FP_VLAN_TAG_STRUCTURE_BMP_MAX, 
                                      SOC_PB_PP_FP_VLAN_TAG_STRUCTURE_BMP_OUT_OF_RANGE_ERR, 21, exit);
            break;
        case SOC_PPC_FP_QUAL_IRPP_IN_PORT_BITMAP:
            break;
        default:
            SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FP_ENTRY_QUAL_TYPE_NOT_IN_DB_ERR, 25, exit);
        }
      }
  }

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_FP_PFG_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_FP_DATABASE_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_FP_DATABASE_INFO *info
  )
{
  uint32
    ind;
  uint8
    end_of_list;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->db_type, SOC_PB_PP_FP_DATABASE_TYPE_MAX, SOC_PB_PP_FP_DATABASE_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  if (info->db_type != SOC_PB_PP_FP_DB_TYPE_EGRESS)
  {
    SOC_SAND_ERR_IF_OUT_OF_RANGE(info->supported_pfgs, SOC_PB_PP_FP_SUPPORTED_PFGS_MIN, SOC_PB_PP_FP_SUPPORTED_PFGS_MAX, SOC_PB_PP_FP_SUPPORTED_PFGS_OUT_OF_RANGE_ERR, 11, exit);
  }

  end_of_list = FALSE;
  for (ind = 0; ind < SOC_PB_PP_FP_NOF_QUALS_PER_DB_MAX; ++ind)
  {
    if (end_of_list == FALSE)
    {
      SOC_SAND_ERR_IF_ABOVE_MAX(info->qual_types[ind], SOC_PB_PP_NOF_FP_QUAL_TYPES, SOC_PB_PP_FP_QUAL_TYPES_OUT_OF_RANGE_ERR, 11, exit);
    }
    else /* end_of_list == TRUE */
    {
      SOC_SAND_ERR_IF_OUT_OF_RANGE(info->qual_types[ind], SOC_PB_PP_NOF_FP_QUAL_TYPES, SOC_PB_PP_NOF_FP_QUAL_TYPES, SOC_PB_PP_FP_QUAL_TYPES_END_OF_LIST_ERR, 111, exit);
    }
    if (info->qual_types[ind] == SOC_PB_PP_NOF_FP_QUAL_TYPES)
    {
      end_of_list = TRUE;
    }
  }


  end_of_list = FALSE;
  for (ind = 0; ind < SOC_PB_PP_FP_NOF_ACTIONS_PER_DB_MAX; ++ind)
  {
    if (end_of_list == FALSE)
    {
      SOC_SAND_ERR_IF_ABOVE_MAX(info->action_types[ind], SOC_PB_PP_NOF_FP_ACTION_TYPES, SOC_PB_PP_FP_ACTION_TYPES_OUT_OF_RANGE_ERR, 12, exit);
    }
    else /* end_of_list == TRUE */
    {
      SOC_SAND_ERR_IF_OUT_OF_RANGE(info->action_types[ind], SOC_PB_PP_NOF_FP_ACTION_TYPES, SOC_PB_PP_NOF_FP_ACTION_TYPES, SOC_PB_PP_FP_ACTION_TYPES_END_OF_LIST_ERR, 121, exit);
    }
    if (info->action_types[ind] == SOC_PB_PP_NOF_FP_ACTION_TYPES)
    {
      end_of_list = TRUE;
    }
  }

  SOC_SAND_ERR_IF_ABOVE_MAX(info->strength, SOC_PB_PP_FP_STRENGTH_MAX, SOC_PB_PP_FP_STRENGTH_OUT_OF_RANGE_ERR, 13, exit);


  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_FP_DATABASE_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_FP_ACTION_VAL_verify(
    SOC_SAND_IN  SOC_PB_PP_FP_ACTION_VAL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->type, SOC_PB_PP_NOF_FP_ACTION_TYPES, SOC_PB_PP_FP_ACTION_TYPES_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->val, SOC_PB_PP_FP_VAL_MAX, SOC_PB_PP_FP_VAL_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_FP_ACTION_VAL_verify()",0,0);
}

uint32
  SOC_PB_PP_FP_ENTRY_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_FP_ENTRY_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind = 0; ind < SOC_PB_PP_FP_NOF_QUALS_PER_DB_MAX; ++ind)
  {
    SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_FP_QUAL_VAL, &(info->qual_vals[ind]), 10, exit);
  }
  for (ind = 0; ind < SOC_PB_PP_FP_NOF_ACTIONS_PER_DB_MAX; ++ind)
  {
    SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_FP_ACTION_VAL, &(info->actions[ind]), 11, exit);
  }
  SOC_SAND_ERR_IF_ABOVE_MAX(info->priority, SOC_PB_PP_FP_PRIORITY_MAX, SOC_PB_PP_FP_PRIORITY_OUT_OF_RANGE_ERR, 12, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_FP_ENTRY_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_FP_DIR_EXTR_ACTION_LOC_verify(
    SOC_SAND_IN  SOC_PB_PP_FP_DIR_EXTR_ACTION_LOC *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->type, SOC_PB_PP_FP_QUAL_TYPES_MAX, SOC_PB_PP_FP_QUAL_TYPES_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->fld_lsb, SOC_PB_PP_FP_FLD_LSB_MAX, SOC_PB_PP_FP_FLD_LSB_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->cst_val, SOC_PB_PP_FP_CST_VAL_MAX, SOC_PB_PP_FP_CST_VAL_OUT_OF_RANGE_ERR, 12, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->nof_bits, SOC_DPP_FEM_MAX_ACTION_SIZE_NOF_BITS_PETRAB, SOC_PB_PP_FP_NOF_BITS_OUT_OF_RANGE_ERR, 12, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_FP_DIR_EXTR_ACTION_LOC_verify()",0,0);
}

uint32
  SOC_PB_PP_FP_DIR_EXTR_ACTION_VAL_verify(
    SOC_SAND_IN  SOC_PB_PP_FP_DIR_EXTR_ACTION_VAL *info
  )
{
  uint32
    res = SOC_SAND_OK;

  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->type, SOC_PB_PP_FP_ACTION_TYPES_MAX, SOC_PB_PP_FP_ACTION_TYPES_OUT_OF_RANGE_ERR, 10, exit);
  for (ind = 0; ind < SOC_PB_PP_FP_DIR_EXTR_MAX_NOF_FIELDS; ++ind)
  {
    if (info->fld_ext[ind].type != SOC_PB_PP_NOF_FP_QUAL_TYPES)
    {
      SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_FP_DIR_EXTR_ACTION_LOC, &(info->fld_ext[ind]), 11, exit);
    }
  }
  SOC_SAND_ERR_IF_ABOVE_MAX(info->nof_fields, SOC_PB_PP_FP_NOF_FIELDS_MAX, SOC_PB_PP_FP_NOF_FIELDS_OUT_OF_RANGE_ERR, 12, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->base_val, SOC_PB_PP_FP_BASE_VAL_MAX, SOC_PB_PP_FP_BASE_VAL_OUT_OF_RANGE_ERR, 13, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_FP_DIR_EXTR_ACTION_VAL_verify()",0,0);
}

uint32
  SOC_PB_PP_FP_DIR_EXTR_ENTRY_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_FP_DIR_EXTR_ENTRY_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind = 0; ind < SOC_PB_PP_FP_NOF_QUALS_PER_DB_MAX; ++ind)
  {
    if (info->qual_vals[ind].type != SOC_PB_PP_NOF_FP_QUAL_TYPES)
    {
      SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_FP_QUAL_VAL, &(info->qual_vals[ind]), 10, exit);
    }
  }
  for (ind = 0; ind < SOC_PB_PP_FP_NOF_ACTIONS_PER_DB_MAX; ++ind)
  {
    if (info->actions[ind].type != SOC_PB_PP_NOF_FP_ACTION_TYPES)
    {
      SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_FP_DIR_EXTR_ACTION_VAL, &(info->actions[ind]), 11, exit);
    }
  }
  SOC_SAND_ERR_IF_ABOVE_MAX(info->priority, SOC_PB_PP_FP_PRIORITY_MAX, SOC_PB_PP_FP_PRIORITY_OUT_OF_RANGE_ERR, 12, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_FP_DIR_EXTR_ENTRY_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_FP_CONTROL_INDEX_verify(
    SOC_SAND_IN  SOC_PB_PP_FP_CONTROL_INDEX *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->db_id, SOC_PB_PP_FP_DB_ID_MAX, SOC_PB_PP_FP_DB_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->type, SOC_PB_PP_FP_CONTROL_TYPE_MAX, SOC_PB_PP_FP_TYPE_OUT_OF_RANGE_ERR, 11, exit);

  switch (info->type)
  {
  case SOC_PB_PP_FP_CONTROL_TYPE_L4OPS_RANGE:
    SOC_SAND_ERR_IF_ABOVE_MAX(info->val_ndx, SOC_PB_PP_FP_NOF_L4OPS_RANGES, SOC_PB_PP_FP_VAL_NDX_OUT_OF_RANGE_ERR, 13, exit);
    break;

  case SOC_PB_PP_FP_CONTROL_TYPE_PACKET_SIZE_RANGE:
    SOC_SAND_ERR_IF_ABOVE_MAX(info->val_ndx, SOC_PB_PP_FP_NOF_PKT_SZ_RANGES, SOC_PB_PP_FP_VAL_NDX_OUT_OF_RANGE_ERR, 14, exit);
    break;

  case SOC_PB_PP_FP_CONTROL_TYPE_ETHERTYPE:
    SOC_SAND_ERR_IF_ABOVE_MAX(info->val_ndx, SOC_PB_PP_FP_ETHERTYPE_MAX, SOC_PB_PP_FP_VAL_NDX_OUT_OF_RANGE_ERR, 15, exit);
    break;

  case SOC_PB_PP_FP_CONTROL_TYPE_NEXT_PROTOCOL_IP:
    SOC_SAND_ERR_IF_ABOVE_MAX(info->val_ndx, SOC_PB_PP_FP_NEXT_PROTOCOL_IP_MAX, SOC_PB_PP_FP_VAL_NDX_OUT_OF_RANGE_ERR, 16, exit);
    break;

  case SOC_PB_PP_FP_CONTROL_TYPE_EGR_PP_PORT_DATA:
  case SOC_PB_PP_FP_CONTROL_TYPE_PP_PORT_PROFILE:
    SOC_SAND_ERR_IF_ABOVE_MAX(info->val_ndx, SOC_PB_PP_NOF_PORTS, SOC_PB_PP_FP_VAL_NDX_OUT_OF_RANGE_ERR, 17, exit);
    break;

  case SOC_PB_PP_FP_CONTROL_TYPE_EGR_L2_ETHERTYPES:
    SOC_SAND_ERR_IF_OUT_OF_RANGE(info->val_ndx, 8, 15, SOC_PB_PP_FP_VAL_NDX_OUT_OF_RANGE_ERR, 18, exit);
    break;

  case SOC_PB_PP_FP_CONTROL_TYPE_EGR_IPV4_NEXT_PROTOCOL:
    SOC_SAND_ERR_IF_OUT_OF_RANGE(info->val_ndx, SOC_PB_PP_FP_EGR_IPV4_NEXT_PROTOCOL_MIN, SOC_PB_PP_FP_EGR_IPV4_NEXT_PROTOCOL_MAX, SOC_PB_PP_FP_VAL_NDX_OUT_OF_RANGE_ERR, 19, exit);
    break;

  case SOC_PB_PP_FP_CONTROL_TYPE_L2_L3_KEY_IN_LIF_ENABLE:
  case SOC_PB_PP_FP_CONTROL_TYPE_L3_IPV6_TCP_CTL_ENABLE:
  case SOC_PB_PP_FP_CONTROL_TYPE_KEY_CHANGE_SIZE:
    SOC_SAND_ERR_IF_ABOVE_MAX(info->val_ndx, 0, SOC_PB_PP_FP_VAL_NDX_OUT_OF_RANGE_ERR, 20, exit);
    break;

  case SOC_PB_PP_FP_CONTROL_TYPE_HDR_USER_DEF:
    SOC_SAND_ERR_IF_ABOVE_MAX(info->val_ndx, SOC_PB_PP_FP_NOF_HDR_USER_DEFS, SOC_PB_PP_FP_VAL_NDX_OUT_OF_RANGE_ERR, 21, exit);
    break;

  case SOC_PB_PP_FP_CONTROL_TYPE_EGRESS_DP:
    SOC_SAND_ERR_IF_ABOVE_MAX(info->val_ndx, 1, SOC_PB_PP_FP_VAL_NDX_OUT_OF_RANGE_ERR, 22, exit);
    break;

  case SOC_PB_PP_FP_CONTROL_TYPE_INNER_ETH_NOF_VLAN_TAGS:
    SOC_SAND_ERR_IF_ABOVE_MAX(info->val_ndx, SOC_PB_PP_FP_NOF_PFGS, SOC_PB_PP_FP_VAL_NDX_OUT_OF_RANGE_ERR, 23, exit);
    break;

  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FP_TYPE_OUT_OF_RANGE_ERR, 22, exit);
  }

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_FP_CONTROL_INDEX_verify()",0,0);
}

uint32
  SOC_PB_PP_FP_CONTROL_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_FP_CONTROL_TYPE type,
    SOC_SAND_IN  SOC_PB_PP_FP_CONTROL_INFO *info
  )
{
  uint32
    val_ndx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  switch (type)
  {
  case SOC_PB_PP_FP_CONTROL_TYPE_L4OPS_RANGE:
    for (val_ndx = 0; val_ndx < 4; val_ndx++)
    {
      SOC_SAND_ERR_IF_ABOVE_MAX(info->val[val_ndx], ((1<<16)-1), SOC_PB_PP_FP_VAL_OUT_OF_RANGE_ERR, 20, exit);
    }
    break;

  case SOC_PB_PP_FP_CONTROL_TYPE_PACKET_SIZE_RANGE:
    for (val_ndx = 0; val_ndx < 2; val_ndx++)
    {
      SOC_SAND_ERR_IF_OUT_OF_RANGE(info->val[val_ndx], 1, ((1<<7)-1), SOC_PB_PP_FP_VAL_OUT_OF_RANGE_ERR, 30, exit);
    }
    break;

  case SOC_PB_PP_FP_CONTROL_TYPE_ETHERTYPE:
  case SOC_PB_PP_FP_CONTROL_TYPE_NEXT_PROTOCOL_IP:
  case SOC_PB_PP_FP_CONTROL_TYPE_L2_L3_KEY_IN_LIF_ENABLE:
  case SOC_PB_PP_FP_CONTROL_TYPE_L3_IPV6_TCP_CTL_ENABLE:
    SOC_SAND_ERR_IF_ABOVE_MAX(info->val[0], 1, SOC_PB_PP_FP_VAL_NDX_OUT_OF_RANGE_ERR, 32, exit);
    break;

  case SOC_PB_PP_FP_CONTROL_TYPE_EGR_PP_PORT_DATA:
    SOC_SAND_ERR_IF_ABOVE_MAX(info->val[0], SOC_PB_PP_FP_EGR_PP_PORT_DATA_MAX, SOC_PB_PP_FP_VAL_NDX_OUT_OF_RANGE_ERR, 34, exit);
    break;

  case SOC_PB_PP_FP_CONTROL_TYPE_EGR_L2_ETHERTYPES:
    SOC_SAND_ERR_IF_ABOVE_MAX(info->val[0], (1 << 16) - 1, SOC_PB_PP_FP_VAL_NDX_OUT_OF_RANGE_ERR, 35, exit);
    break;

  case SOC_PB_PP_FP_CONTROL_TYPE_EGR_IPV4_NEXT_PROTOCOL:
    SOC_SAND_ERR_IF_ABOVE_MAX(info->val[0], SOC_PB_PP_FP_NEXT_PROTOCOL_IP_MAX, SOC_PB_PP_FP_VAL_NDX_OUT_OF_RANGE_ERR, 36, exit);
    break;

  case SOC_PB_PP_FP_CONTROL_TYPE_PP_PORT_PROFILE:
    SOC_SAND_ERR_IF_ABOVE_MAX(info->val[0], SOC_PB_PP_FP_PP_PORT_PROFILE_MAX, SOC_PB_PP_FP_VAL_NDX_OUT_OF_RANGE_ERR, 38, exit);
    break;

  case SOC_PB_PP_FP_CONTROL_TYPE_HDR_USER_DEF:
    SOC_SAND_ERR_IF_ABOVE_MAX(info->val[0], SOC_PB_PP_NOF_FP_BASE_HEADER_TYPES, SOC_PB_PP_FP_VAL_NDX_OUT_OF_RANGE_ERR, 40, exit);
    SOC_SAND_ERR_IF_ABOVE_MAX(info->val[1], SOC_PB_PP_NOF_FP_UDP_OFFSET_MAX, SOC_PB_PP_FP_VAL_NDX_OUT_OF_RANGE_ERR, 42, exit);
    SOC_SAND_ERR_IF_OUT_OF_RANGE(info->val[2], SOC_PB_PP_FP_UDP_NOF_BITS_MIN, SOC_PB_PP_FP_UDP_NOF_BITS_MAX, SOC_PB_PP_FP_VAL_NDX_OUT_OF_RANGE_ERR, 44, exit);
    break;

  case SOC_PB_PP_FP_CONTROL_TYPE_EGRESS_DP:
    SOC_SAND_ERR_IF_ABOVE_MAX(info->val[0], SOC_PETRA_NOF_DROP_PRECEDENCE, SOC_PB_PP_FP_VAL_NDX_OUT_OF_RANGE_ERR, 46, exit);
    break;

  case SOC_PB_PP_FP_CONTROL_TYPE_INNER_ETH_NOF_VLAN_TAGS:
    SOC_SAND_ERR_IF_ABOVE_MAX(info->val[0], 3, SOC_PB_PP_FP_VAL_NDX_OUT_OF_RANGE_ERR, 48, exit);
    break;

  case SOC_PB_PP_FP_CONTROL_TYPE_KEY_CHANGE_SIZE:
    SOC_SAND_ERR_IF_OUT_OF_RANGE(info->val[0], 1, 12, SOC_PB_PP_FP_VAL_NDX_OUT_OF_RANGE_ERR, 50, exit);
    break;

  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FP_TYPE_OUT_OF_RANGE_ERR, 60, exit);
  }

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_FP_CONTROL_INFO_verify()",0,0);
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

