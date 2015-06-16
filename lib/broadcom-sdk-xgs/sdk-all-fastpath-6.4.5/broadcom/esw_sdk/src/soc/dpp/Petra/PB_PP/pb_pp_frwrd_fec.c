/* $Id: pb_pp_frwrd_fec.c,v 1.15 Broadcom SDK $
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

#include <soc/dpp/dpp_config_defs.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Utils/sand_bitstream.h>

#include <soc/dpp/Petra/PB_PP/pb_pp_framework.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_frwrd_fec.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_general.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_trap_mgmt.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_tbl_access.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_chip_regs.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_reg_access.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_chip_tbls.h>

#include <soc/dpp/Petra/PB_PP/pb_pp_sw_db.h>


/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PB_PP_FRWRD_FEC_PROTECT_TYPE_MAX                       (SOC_PB_PP_NOF_FRWRD_FEC_PROTECT_TYPES-1)
#define SOC_PB_PP_FRWRD_FEC_NOF_ENTRIES_MAX                        (SOC_DPP_NOF_FECS_PETRAB - 1)
#define SOC_PB_PP_FRWRD_FEC_OAM_INSTANCE_NDX_MIN                   (1) /*0 saved for no protecion*/
#define SOC_PB_PP_FRWRD_FEC_OAM_INSTANCE_NDX_MAX                   (2*1024-2) /*4095 saved for facility*/
#define SOC_PB_PP_FRWRD_FEC_UP_MAX                                 (1)
#define SOC_PB_PP_FRWRD_FEC_PORTECT_DEST_MAX_PHY_CHECK             (4*1024 - 256 - 1)
#define SOC_PB_PP_FRWRD_FEC_TRAP_ABLE_MAX_INDEX                    (63)
#define SOC_PB_PP_FRWRD_FEC_ECMP_SIZES_MIN                         (1)
#define SOC_PB_PP_FRWRD_FEC_ECMP_SIZES_MAX                         (288)
#define SOC_PB_PP_FRWRD_FEC_RPF_MODE_MAX                           (SOC_PB_PP_FRWRD_FEC_RPF_MODE_MC_USE_SIP_WITH_ECMP)
#define SOC_PB_PP_FRWRD_FEC_DIST_TREE_NICK_MAX                     (SOC_SAND_U32_MAX)
#define SOC_PB_PP_FRWRD_FEC_TYPE_MAX                               (SOC_PB_PP_NOF_FEC_TYPES-1)
#define SOC_PB_PP_FRWRD_FEC_EEP_MIN                                (2)
#define SOC_PB_PP_FRWRD_FEC_EEP_MAX                                (SOC_DPP_NOF_GLOBAL_LIFS_PETRAB - 1)
#define SOC_PB_PP_FRWRD_FEC_OAM_INSTANCE_ID_MIN                    (1)
#define SOC_PB_PP_FRWRD_FEC_OAM_INSTANCE_ID_MAX                    (2*1024-2)
#define SOC_PB_PP_FRWRD_FEC_VALUE_MAX                              (SOC_SAND_U32_MAX)
#define SOC_PB_PP_FRWRD_FEC_SEED_MAX                               (SOC_SAND_U16_MAX)
#define SOC_PB_PP_FRWRD_FEC_HASH_FUNC_ID_MAX                       (15)
#define SOC_PB_PP_FRWRD_FEC_KEY_SHIFT_MAX                          (15)
#define SOC_PB_PP_FRWRD_FEC_NOF_HEADERS_MIN                        (1)
#define SOC_PB_PP_FRWRD_FEC_NOF_HEADERS_MAX                        (2)

/* protection pointer value for no protection */
#define SOC_PB_PP_FRWRD_FEC_PROTECT_PTR_VAL_NO_PROTECT   (0)
/* protection pointer value for facility protection */
#define SOC_PB_PP_FRWRD_FEC_PROTECT_PTR_VAL_FAC_PROTECT   (0x7FF)
/* reserved EEP Value */
#define SOC_PB_PP_FRWRD_FEC_EEP_RSRVED_VAL          (1)

/* } */
/*************
 * MACROS    *
 *************/
/* { */
/* check if this even FEC */
#define SOC_PB_PP_FRWRD_FEC_EVEN_FEC(fec_ndx) (((fec_ndx)%2) == 0)
#define SOC_PB_PP_FRWRD_FEC_NEXT_ENTRY(fec_ndx) ((fec_ndx)+1)
#define SOC_PB_PP_FRWRD_FEC_SUPER_ENTRY_INDEX(fec_ndx) ((fec_ndx)/2)
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
  Soc_pb_pp_procedure_desc_element_frwrd_fec[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_FEC_GLBL_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_FEC_GLBL_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_FEC_GLBL_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_FEC_GLBL_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_FEC_GLBL_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_FEC_GLBL_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_FEC_GLBL_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_FEC_GLBL_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_FEC_ENTRY_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_FEC_ENTRY_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_FEC_ENTRY_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_FEC_ENTRY_ADD_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_FEC_ECMP_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_FEC_ECMP_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_FEC_ECMP_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_FEC_ECMP_ADD_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_FEC_ECMP_UPDATE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_FEC_ECMP_UPDATE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_FEC_ECMP_UPDATE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_FEC_ECMP_UPDATE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_FEC_ENTRY_USE_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_FEC_ENTRY_USE_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_FEC_ENTRY_USE_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_FEC_ENTRY_USE_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_FEC_ENTRY_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_FEC_ENTRY_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_FEC_ENTRY_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_FEC_ENTRY_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_FEC_ECMP_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_FEC_ECMP_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_FEC_ECMP_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_FEC_ECMP_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_FEC_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_FEC_REMOVE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_FEC_REMOVE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_FEC_REMOVE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_FEC_PROTECTION_OAM_INSTANCE_STATUS_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_FEC_PROTECTION_OAM_INSTANCE_STATUS_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_FEC_PROTECTION_OAM_INSTANCE_STATUS_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_FEC_PROTECTION_OAM_INSTANCE_STATUS_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_FEC_PROTECTION_OAM_INSTANCE_STATUS_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_FEC_PROTECTION_OAM_INSTANCE_STATUS_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_FEC_PROTECTION_OAM_INSTANCE_STATUS_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_FEC_PROTECTION_OAM_INSTANCE_STATUS_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_FEC_PROTECTION_SYS_PORT_STATUS_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_FEC_PROTECTION_SYS_PORT_STATUS_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_FEC_PROTECTION_SYS_PORT_STATUS_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_FEC_PROTECTION_SYS_PORT_STATUS_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_FEC_PROTECTION_SYS_PORT_STATUS_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_FEC_PROTECTION_SYS_PORT_STATUS_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_FEC_PROTECTION_SYS_PORT_STATUS_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_FEC_PROTECTION_SYS_PORT_STATUS_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_FEC_GET_BLOCK),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_FEC_GET_BLOCK_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_FEC_GET_BLOCK_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_FEC_GET_BLOCK_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_FEC_ENTRY_ACCESSED_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_FEC_ENTRY_ACCESSED_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_FEC_ENTRY_ACCESSED_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_FEC_ENTRY_ACCESSED_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_FEC_ENTRY_ACCESSED_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_FEC_ENTRY_ACCESSED_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_FEC_ENTRY_ACCESSED_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_FEC_ENTRY_ACCESSED_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_FEC_ECMP_HASHING_GLOBAL_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_FEC_ECMP_HASHING_GLOBAL_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_FEC_ECMP_HASHING_GLOBAL_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_FEC_ECMP_HASHING_GLOBAL_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_FEC_ECMP_HASHING_GLOBAL_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_FEC_ECMP_HASHING_GLOBAL_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_FEC_ECMP_HASHING_GLOBAL_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_FEC_ECMP_HASHING_GLOBAL_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_FEC_ECMP_HASHING_PORT_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_FEC_ECMP_HASHING_PORT_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_FEC_ECMP_HASHING_PORT_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_FEC_ECMP_HASHING_PORT_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_FEC_ECMP_HASHING_PORT_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_FEC_ECMP_HASHING_PORT_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_FEC_ECMP_HASHING_PORT_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_FEC_ECMP_HASHING_PORT_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_FEC_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_FEC_GET_ERRS_PTR),
  /*
   * } Auto generated. Do not edit previous section.
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_FEC_SIZE_TO_INDEX_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_FEC_INDEX_TO_SIZE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_FEC_ONE_ENTRY_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_FEC_ONE_ENTRY_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_FEC_SIZE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_FEC_PROTECT_TYPE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_FEC_DEST_ENCODE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_FEC_DEST_DECODE),
   
  /*
   * Last element. Do no touch.
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

static SOC_ERROR_DESC_ELEMENT
  Soc_pb_pp_error_desc_element_frwrd_fec[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  {
    SOC_PB_PP_FRWRD_FEC_PROTECT_TYPE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FRWRD_FEC_PROTECT_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'protect_type' is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_FRWRD_FEC_PROTECT_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_FEC_SUCCESS_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FRWRD_FEC_SUCCESS_OUT_OF_RANGE_ERR",
    "The parameter 'success' is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_FRWRD_FEC_PROTECT_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_FEC_NOF_ENTRIES_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FRWRD_FEC_NOF_ENTRIES_OUT_OF_RANGE_ERR",
    "The parameter 'nof_entries' is out of range. \n\r "
    "The range is: 0 - 16383.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_FEC_OAM_INSTANCE_NDX_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FRWRD_FEC_OAM_INSTANCE_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'oam_instance_ndx' is out of range. \n\r "
    "The range is: Petra - B: 1-2*1024-2, T20E:0-16K-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_FEC_UP_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FRWRD_FEC_UP_OUT_OF_RANGE_ERR",
    "The parameter 'up' is out of range. \n\r "
    "The range is: 0 - 1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_FEC_ECMP_SIZES_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FRWRD_FEC_ECMP_SIZES_OUT_OF_RANGE_ERR",
    "The parameter 'ecmp_sizes' is out of range. \n\r "
    "The range is: 1 - 288.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_FEC_ECMP_SIZES_NOF_ENTRIES_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FRWRD_FEC_ECMP_SIZES_NOF_ENTRIES_OUT_OF_RANGE_ERR",
    "The parameter 'ecmp_sizes_nof_entries' is out of range. \n\r "
    "The range is: 1 - 16.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_FEC_RPF_MODE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FRWRD_FEC_RPF_MODE_OUT_OF_RANGE_ERR",
    "The parameter 'rpf_mode' is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_FRWRD_FEC_RPF_MODES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_FEC_DIST_TREE_NICK_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FRWRD_FEC_DIST_TREE_NICK_OUT_OF_RANGE_ERR",
    "The parameter 'dist_tree_nick' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_FEC_TYPE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FRWRD_FEC_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'type' is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_FEC_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_FEC_EEP_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FRWRD_FEC_EEP_OUT_OF_RANGE_ERR",
    "The parameter 'eep' is out of range. \n\r "
    "The range is: 2 - 12K.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_FEC_OAM_INSTANCE_ID_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FRWRD_FEC_OAM_INSTANCE_ID_OUT_OF_RANGE_ERR",
    "The parameter 'oam_instance_id' is out of range. \n\r "
    "The range is: Petra - B:1-2*1024-2, T20E:0-16K-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_FEC_VALUE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FRWRD_FEC_VALUE_OUT_OF_RANGE_ERR",
    "The parameter 'value' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_FEC_SEED_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FRWRD_FEC_SEED_OUT_OF_RANGE_ERR",
    "The parameter 'seed' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U16_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_FEC_HASH_FUNC_ID_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FRWRD_FEC_HASH_FUNC_ID_OUT_OF_RANGE_ERR",
    "The parameter 'hash_func_id' is out of range. \n\r "
    "The range is: 0 - 15.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_FEC_KEY_SHIFT_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FRWRD_FEC_KEY_SHIFT_OUT_OF_RANGE_ERR",
    "The parameter 'key_shift' is out of range. \n\r "
    "The range is: 0 - 15.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_FEC_NOF_HEADERS_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FRWRD_FEC_NOF_HEADERS_OUT_OF_RANGE_ERR",
    "The parameter 'nof_headers' is out of range. \n\r "
    "The range is: 1 - 2.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  /*
   * } Auto generated. Do not edit previous section.
   */
  {
    SOC_PB_PP_FRWRD_FEC_NO_MATCH_ECMP_SIZE_ERR,
    "SOC_PB_PP_FRWRD_FEC_NO_MATCH_ECMP_SIZE_ERR",
    "the ECMP's size (to add) doesn't \n\r "
    "match any of the size set by \n\r "
    "soc_pb_pp_frwrd_fec_glbl_info_set()\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_FEC_ECMP_SIZE_INDEX_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FRWRD_FEC_ECMP_SIZE_INDEX_OUT_OF_RANGE_ERR",
    "ECMP size index is out of range \n\r "
    "range is 0-min(15,global configuration \n\r "
    "(ecmp_sizes_nof_entries)\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_FEC_PROTECTED_NOT_EVEN_INDEX_ERR,
    "SOC_PB_PP_FRWRD_FEC_PROTECTED_NOT_EVEN_INDEX_ERR",
    "protected FEC has to start in even index (fec_ndx)\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_FEC_ECMP_NOT_EVEN_INDEX_ERR,
    "SOC_PB_PP_FRWRD_FEC_ECMP_NOT_EVEN_INDEX_ERR",
    "ECMP has to start in even index (fec_ndx)\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_FEC_UPDATE_RANGE_OUT_OF_ECMP_ERR,
    "SOC_PB_PP_FRWRD_FEC_UPDATE_RANGE_OUT_OF_ECMP_ERR",
    "update ECMP using soc_pb_pp_frwrd_fec_ecmp_update()\n\r "
    "where end/start index is out of ECMP size \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_FEC_UPDATE_RANGE_ILLEGAL_ERR,
    "SOC_PB_PP_FRWRD_FEC_UPDATE_RANGE_ILLEGAL_ERR",
    "update ECMP using soc_pb_pp_frwrd_fec_ecmp_update()\n\r "
    "where end < start \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_FEC_DEST_TYPE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FRWRD_FEC_DEST_TYPE_OUT_OF_RANGE_ERR",
    "illegal destination type. in FEC entry \n\r "
    "destination cannot be route,FEC,Trap\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_FEC_DEST_VAL_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FRWRD_FEC_DEST_VAL_OUT_OF_RANGE_ERR",
    "value of the destination is out of range\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_FEC_IPMC_DEST_NOT_MC_ERR,
    "SOC_PB_PP_FRWRD_FEC_IPMC_DEST_NOT_MC_ERR",
    "add/update FEC entry for IP-MC application\n\r "
    "where FEC.destination is not Multicast\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_FEC_IPMC_RPF_MODE_ILLEGAL_ERR,
    "SOC_PB_PP_FRWRD_FEC_IPMC_RPF_MODE_ILLEGAL_ERR",
    "add/update FEC entry for IP-MC application\n\r "
    "where RPF check is for IP-UC\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_FEC_IPUC_RPF_MODE_ILLEGAL_ERR,
    "SOC_PB_PP_FRWRD_FEC_IPUC_RPF_MODE_ILLEGAL_ERR",
    "add/update FEC entry for IP-UC application\n\r "
    "where RPF check is for IP-MC\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_FEC_EXPECT_NON_ECMP_ERR,
    "SOC_PB_PP_FRWRD_FEC_EXPECT_NON_ECMP_ERR",
    "calling soc_pb_pp_frwrd_fec_entry_get() where fec_ndx \n\r "
    "points to ECMP\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_FEC_EXPECT_NON_PROTECT_ERR,
    "SOC_PB_PP_FRWRD_FEC_EXPECT_NON_PROTECT_ERR",
    "calling soc_pb_pp_frwrd_fec_ecmp_get() where fec_ndx \n\r "
    "points to protected FEC\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_FEC_DEST_PHY_PORT_STATUS_ILLEGAL_ERR,
    "SOC_PB_PP_FRWRD_FEC_DEST_PHY_PORT_STATUS_ILLEGAL_ERR",
    "for physical system port > 4k - 256 status \n\r "
    "must be UP = true\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_FEC_TRAP_ACCESS_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FRWRD_FEC_TRAP_ACCESS_OUT_OF_RANGE_ERR",
    "trap if accessed supported only to FEC-ids in the \n\r "
    "range 0-63 \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_FEC_ACCESSED_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FRWRD_FEC_ACCESSED_OUT_OF_RANGE_ERR",
    "accessed cannot be set, relevant only for get\n\r ",
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
STATIC uint32
  soc_pb_pp_frwrd_fec_size_to_index_get(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              size,
    SOC_SAND_OUT  uint32             *index
  )
{
  uint32
    indx;
  SOC_PB_PP_FRWRD_FEC_GLBL_INFO
    glbl_info;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_FEC_SIZE_TO_INDEX_GET);

  SOC_PB_PP_FRWRD_FEC_GLBL_INFO_clear(unit, &glbl_info);
  
  /* size 1 has to be in index 0 */
  if (size == 1)
  {
    *index = 0;
    goto exit;
  }

  res = soc_pb_pp_sw_db_fec_glbl_info_get(
          unit,
          &glbl_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  for (indx = 0; indx < glbl_info.ecmp_sizes_nof_entries; ++indx)
  {
    if (glbl_info.ecmp_sizes[indx] == size)
    {
      *index = indx;
      goto exit;
    }
  }
  /* if no suitable index was found*/
  SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FRWRD_FEC_NO_MATCH_ECMP_SIZE_ERR,10,exit)

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_fec_size_to_index_get()", 0, 0);
}

STATIC uint32
  soc_pb_pp_frwrd_fec_index_to_size_get(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              index,
    SOC_SAND_OUT  uint32             *size
  )
{
  SOC_PB_PP_FRWRD_FEC_GLBL_INFO
    glbl_info;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_FEC_INDEX_TO_SIZE_GET);

  SOC_PB_PP_FRWRD_FEC_GLBL_INFO_clear(unit, &glbl_info);
  
  /* size 0 has to be in index 0 */
  if (index == 0)
  {
    *size = 1;
    goto exit;
  }

  res = soc_pb_pp_sw_db_fec_glbl_info_get(
          unit,
          &glbl_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_ERR_IF_ABOVE_MAX(index, SOC_DPP_ECMP_MAX_SIZE_PETRAB-1, SOC_PB_PP_FRWRD_FEC_ECMP_SIZE_INDEX_OUT_OF_RANGE_ERR, 20, exit);

  *size = glbl_info.ecmp_sizes[index];

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_fec_index_to_size_get()", 0, 0);
}

/*
 * given FEC entry, find the encoding for fields: Destination, outlif
 */
STATIC uint32
  soc_pb_pp_frwrd_fec_dest_encode(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_ENTRY_INFO     *fec_entry,
    SOC_SAND_OUT  uint32                      *dest_enc,
    SOC_SAND_OUT  uint32                      *outlif_enc
  )
{
  SOC_PB_PP_FRWRD_DECISION_INFO
    fwd_decision;
  SOC_PB_PP_TRAP_CODE_INTERNAL
    trap_code_internal;
  SOC_PETRA_REG_FIELD
    *strength_fld_fwd,
    *strength_fld_snp;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_FEC_DEST_ENCODE);

  SOC_SAND_CHECK_NULL_INPUT(fec_entry);
  SOC_SAND_CHECK_NULL_INPUT(dest_enc);
  SOC_SAND_CHECK_NULL_INPUT(outlif_enc);

  SOC_PB_PP_FRWRD_DECISION_INFO_clear(&fwd_decision);

  /* Setting destination */

  if (fec_entry->type == SOC_PB_PP_FEC_TYPE_DROP || fec_entry->type == SOC_PB_PP_NOF_FEC_TYPES)
  {
    fwd_decision.type = SOC_PB_PP_FRWRD_DECISION_TYPE_DROP;
    fwd_decision.additional_info.outlif.type = SOC_PB_PP_OUTLIF_ENCODE_TYPE_NONE;
  }
  /* setting destination info */
  if (fec_entry->type != SOC_PB_PP_FEC_TYPE_DROP && fec_entry->type != SOC_PB_PP_NOF_FEC_TYPES)
  {
    switch(fec_entry->dest.dest_type)
    {
      case SOC_PB_PP_FEC_TYPE_DROP:
        fwd_decision.type = SOC_PB_PP_FRWRD_DECISION_TYPE_DROP;
      break;
      case SOC_SAND_PP_DEST_SINGLE_PORT:
        fwd_decision.type = SOC_PB_PP_FRWRD_DECISION_TYPE_UC_PORT;
      break;
      case SOC_SAND_PP_DEST_MULTICAST:
        fwd_decision.type = SOC_PB_PP_FRWRD_DECISION_TYPE_MC;
      break;
      case SOC_SAND_PP_DEST_LAG:
        fwd_decision.type = SOC_PB_PP_FRWRD_DECISION_TYPE_UC_LAG;
      break;
      case SOC_SAND_PP_DEST_EXPLICIT_FLOW:
        fwd_decision.type = SOC_PB_PP_FRWRD_DECISION_TYPE_UC_FLOW;
      break;
      case SOC_SAND_PP_DEST_TRAP:
        /* will be set later*/;
      break;
      default:
        SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FRWRD_FEC_DEST_TYPE_OUT_OF_RANGE_ERR, 30,exit);
    }
    fwd_decision.dest_id = fec_entry->dest.dest_val;
  }
  /* Setting additional info*/
  switch(fec_entry->type)
  {
    case SOC_PB_PP_FEC_TYPE_DROP:
      fwd_decision.additional_info.outlif.type = SOC_PB_PP_OUTLIF_ENCODE_TYPE_NONE;
    break;
    case SOC_PB_PP_FEC_TYPE_IP_MC:
      fwd_decision.additional_info.eei.type = SOC_PB_PP_EEI_TYPE_EMPTY;
    break;
    case SOC_PB_PP_FEC_TYPE_BRIDGING_WITH_AC:
      fwd_decision.additional_info.outlif.type = SOC_PB_PP_OUTLIF_ENCODE_TYPE_AC;
      fwd_decision.additional_info.outlif.val = fec_entry->app_info.out_ac_id;
    break;
    case SOC_PB_PP_FEC_TYPE_IP_UC:
    case SOC_PB_PP_FEC_TYPE_TRILL_UC:
    case SOC_PB_PP_FEC_TYPE_BRIDGING_INTO_TUNNEL:
    case SOC_PB_PP_FEC_TYPE_MPLS_LSR:
      fwd_decision.additional_info.outlif.type = SOC_PB_PP_OUTLIF_ENCODE_TYPE_EEP;
      fwd_decision.additional_info.outlif.val = (fec_entry->eep==SOC_PB_PP_EEP_NULL)?SOC_PB_PP_FRWRD_FEC_EEP_RSRVED_VAL:fec_entry->eep;
    break;
    default:
      fwd_decision.additional_info.outlif.type = SOC_PB_PP_OUTLIF_ENCODE_TYPE_NONE;
    break;
  }
  /* if destination is trap then override forwarding decision */
  if(fec_entry->dest.dest_type == SOC_SAND_PP_DEST_TRAP)
  {
    fwd_decision.type = SOC_PB_PP_FRWRD_DECISION_TYPE_TRAP;


    res = soc_pb_pp_trap_mgmt_trap_code_to_internal(
            fec_entry->trap_info.trap_code,
            &trap_code_internal,
            &strength_fld_fwd,
            &strength_fld_snp
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

    SOC_PETRA_COPY(
      &(fwd_decision.additional_info.trap_info.action_profile),
      &(fec_entry->trap_info),
      SOC_PB_PP_ACTION_PROFILE,
      1
    );

    fwd_decision.additional_info.trap_info.action_profile.trap_code = (SOC_PB_PP_TRAP_CODE)trap_code_internal;

  }
  res = soc_pb_pp_fwd_decision_in_buffer_build(
          SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_FEC,
          &fwd_decision,
          FALSE,
          dest_enc,
          outlif_enc
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_fec_dest_encode()", 0, 0);
}

STATIC uint32
  soc_pb_pp_frwrd_fec_dest_decode(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                       dest_enc,
    SOC_SAND_IN  uint32                       outlif_enc,
    SOC_SAND_OUT  SOC_PB_PP_FRWRD_FEC_ENTRY_INFO    *fec_entry
  )
{
  SOC_PB_PP_FRWRD_DECISION_INFO
    fwd_decision;
  uint8
    dummy;
  SOC_PB_PP_TRAP_CODE
    trap_code;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_FEC_DEST_DECODE);

  SOC_SAND_CHECK_NULL_INPUT(fec_entry);

  SOC_PB_PP_FRWRD_DECISION_INFO_clear(&fwd_decision);

  res = soc_pb_pp_fwd_decision_in_buffer_parse(
          SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_FEC,
          dest_enc,
          outlif_enc,
          &fwd_decision,
          &dummy
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /* Setting destination */
  if (fwd_decision.type == SOC_PB_PP_FRWRD_DECISION_TYPE_TRAP)
  {
    fec_entry->dest.dest_type = SOC_SAND_PP_DEST_TRAP;
    SOC_PETRA_COPY(
      &(fec_entry->trap_info),
      &(fwd_decision.additional_info.trap_info.action_profile),
      SOC_PB_PP_ACTION_PROFILE,
      1
    );

    res = soc_pb_pp_trap_cpu_trap_code_from_internal_unsafe(
            unit,
            fwd_decision.additional_info.trap_info.action_profile.trap_code,
            &trap_code
          );
      SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

    fec_entry->trap_info.trap_code = trap_code;

  }
  else if (fec_entry->type == SOC_PB_PP_FEC_TYPE_DROP)
  {
    fec_entry->dest.dest_type = SOC_PB_PP_FRWRD_DECISION_TYPE_DROP;
  }
  /* setting destination info */
  else if (fec_entry->type != SOC_PB_PP_FEC_TYPE_DROP)
  {
    switch(fwd_decision.type)
    {
      case SOC_PB_PP_FRWRD_DECISION_TYPE_DROP:
        fec_entry->dest.dest_type = SOC_PB_PP_FEC_TYPE_DROP;
      break;
      case SOC_PB_PP_FRWRD_DECISION_TYPE_UC_PORT:
        fec_entry->dest.dest_type = SOC_SAND_PP_DEST_SINGLE_PORT;
      break;
      case SOC_PB_PP_FRWRD_DECISION_TYPE_MC:
        fec_entry->dest.dest_type = SOC_SAND_PP_DEST_MULTICAST;
      break;
      case SOC_PB_PP_FRWRD_DECISION_TYPE_UC_LAG:
        fec_entry->dest.dest_type = SOC_SAND_PP_DEST_LAG;
      break;
      case SOC_PB_PP_FRWRD_DECISION_TYPE_UC_FLOW:
        fec_entry->dest.dest_type = SOC_SAND_PP_DEST_EXPLICIT_FLOW;
      break;
      default:
        SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FRWRD_FEC_DEST_TYPE_OUT_OF_RANGE_ERR, 30,exit);
    }
    fec_entry->dest.dest_val = fwd_decision.dest_id;
  }
  /* Setting additional info*/
  switch(fec_entry->type)
  {
    case SOC_PB_PP_FEC_TYPE_IP_MC:
    case SOC_PB_PP_FEC_TYPE_DROP:
    break;
    case SOC_PB_PP_FEC_TYPE_BRIDGING_WITH_AC:
      fec_entry->app_info.out_ac_id = fwd_decision.additional_info.outlif.val;
    break;
    case SOC_PB_PP_FEC_TYPE_IP_UC:
    case SOC_PB_PP_FEC_TYPE_TRILL_UC:
    case SOC_PB_PP_FEC_TYPE_BRIDGING_INTO_TUNNEL:
    case SOC_PB_PP_FEC_TYPE_MPLS_LSR:
      fec_entry->eep = fwd_decision.additional_info.outlif.val;
      if (fec_entry->eep == SOC_PB_PP_FRWRD_FEC_EEP_RSRVED_VAL)
      {
        fec_entry->eep=SOC_PB_PP_EEP_NULL;
      }
    break;
    default:
    break;
  }
  if (fec_entry->dest.dest_type == SOC_SAND_PP_DEST_EXPLICIT_FLOW && fec_entry->dest.dest_val == SOC_PETRA_MAX_QUEUE_ID)
  {
    fec_entry->dest.dest_type = SOC_SAND_PP_DEST_TYPE_DROP;
    fec_entry->dest.dest_val = 0;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_fec_dest_decode()", 0, 0);
}

STATIC uint32
  soc_pb_pp_frwrd_fec_rpf_mode_to_hw_val_map(
    SOC_SAND_IN SOC_PB_PP_FRWRD_FEC_RPF_MODE  rpf_mode
  )
{
  switch(rpf_mode)
  {
  case SOC_PB_PP_FRWRD_FEC_RPF_MODE_UC_STRICT:
    return 0;
  break;
  case SOC_PB_PP_FRWRD_FEC_RPF_MODE_UC_LOOSE:
    return 1;
  break;
  case SOC_PB_PP_FRWRD_FEC_RPF_MODE_NONE:
    return 0;
  break;
  case SOC_PB_PP_FRWRD_FEC_RPF_MODE_MC_EXPLICIT:
    return 1;
  break;
  case SOC_PB_PP_FRWRD_FEC_RPF_MODE_MC_USE_SIP_WITH_ECMP:
    return 2;/*SIP as is*/
  break;
  case SOC_PB_PP_FRWRD_FEC_RPF_MODE_MC_USE_SIP:
  default:
    return 3;
  break;
  }
}



STATIC SOC_PB_PP_FRWRD_FEC_RPF_MODE
  soc_pb_pp_frwrd_fec_rpf_mode_from_hw_val_map(
    SOC_SAND_IN   uint32    rpf_mode_val,
    SOC_SAND_IN   uint8   is_uc_rpf
  )
{
	if(is_uc_rpf)
	{
    switch(rpf_mode_val)
    {
      case 0:
        return SOC_PB_PP_FRWRD_FEC_RPF_MODE_UC_STRICT;
      case 1:
      default:
        return SOC_PB_PP_FRWRD_FEC_RPF_MODE_UC_LOOSE;
    }
  }
  else
  {
    switch(rpf_mode_val)
    {
      case 0:
        return SOC_PB_PP_FRWRD_FEC_RPF_MODE_NONE;
      case 1:
        return SOC_PB_PP_FRWRD_FEC_RPF_MODE_MC_EXPLICIT;
      case 2:
        return SOC_PB_PP_FRWRD_FEC_RPF_MODE_MC_USE_SIP_WITH_ECMP;/*SIP as is*/
      case 3:
      default:
        return SOC_PB_PP_FRWRD_FEC_RPF_MODE_MC_USE_SIP;
    }
   }
}

STATIC uint32
  soc_pb_pp_frwrd_fec_one_entry_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                       fec_ndx,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_ENTRY_INFO    *fec_entry
  )
{
  uint32
    dest_enc,
    outlif_enc;
  SOC_PB_PP_FRWRD_FEC_GLBL_INFO
    glbl_info;
  SOC_PB_PP_IHB_FEC_ENTRY_GENERAL_TBL_DATA
    dest_tbl_data;
  SOC_PB_PP_IHB_FEC_ENTRY_ETH_OR_TRILL_TBL_DATA
    eth_or_trill_tbl_data;
  SOC_PB_PP_IHB_FEC_ENTRY_IP_TBL_DATA
    ip_tbl_data;
  SOC_PB_PP_IHB_FEC_ENTRY_DEFAULT_TBL_DATA
    mpls_tbl_data;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_FEC_ONE_ENTRY_SET);

  SOC_PB_PP_FRWRD_FEC_GLBL_INFO_clear(unit, &glbl_info);
  /* set the destination field */
  res = soc_pb_pp_sw_db_fec_entry_type_set(
          unit,
          fec_ndx,
          fec_entry->type
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  /* extract from fec entry, destination and outlif encoding */
  res = soc_pb_pp_frwrd_fec_dest_encode(
          unit,
          fec_entry,
          &dest_enc,
          &outlif_enc
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  /* SOC_PB_PP_NOF_FEC_TYPES for remove */
  if (fec_entry->type == SOC_PB_PP_FEC_TYPE_DROP || fec_entry->type == SOC_PB_PP_NOF_FEC_TYPES)
  {
    res = soc_pb_pp_ihb_fec_entry_general_tbl_get_unsafe(
            unit,
            fec_ndx,
            &dest_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);
    dest_tbl_data.destination = dest_enc;
    res = soc_pb_pp_ihb_fec_entry_general_tbl_set_unsafe(
            unit,
            fec_ndx,
            &dest_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 27, exit);
  }
  /* get before set, only to enable sharing same entry between IP-MC and IP-UC, if needed*/
  if (fec_entry->type == SOC_PB_PP_FEC_TYPE_IP_UC || fec_entry->type == SOC_PB_PP_FEC_TYPE_IP_MC)
  {
    res = soc_pb_pp_ihb_fec_entry_ip_tbl_get_unsafe(
            unit,
            fec_ndx,
            &ip_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }
  if (fec_entry->type == SOC_PB_PP_FEC_TYPE_IP_UC)
  {
    ip_tbl_data.destination = dest_enc;
    ip_tbl_data.rif = fec_entry->app_info.out_rif;
    /* hw mapping: strict = 0, loose = 1*/
    ip_tbl_data.uc_rpf_mode = soc_pb_pp_frwrd_fec_rpf_mode_to_hw_val_map(fec_entry->rpf_info.rpf_mode);
    ip_tbl_data.out_lif_type = SOC_SAND_GET_BIT(outlif_enc,13);
    ip_tbl_data.out_lif_lsb = SOC_SAND_GET_BITS_RANGE(outlif_enc,12,0);
  }
  else if (fec_entry->type == SOC_PB_PP_FEC_TYPE_IP_MC)
  {
    ip_tbl_data.destination = dest_enc;
    if (fec_entry->rpf_info.rpf_mode == SOC_PB_PP_FRWRD_FEC_RPF_MODE_MC_EXPLICIT)
    {
      ip_tbl_data.rif = fec_entry->rpf_info.expected_in_rif;
    }
    ip_tbl_data.mc_rpf_mode = soc_pb_pp_frwrd_fec_rpf_mode_to_hw_val_map(fec_entry->rpf_info.rpf_mode);
    /* no need to update outlif as it not used for IP-MC, outlif is according to MC.cud */
  }
  if (fec_entry->type == SOC_PB_PP_FEC_TYPE_IP_UC || fec_entry->type == SOC_PB_PP_FEC_TYPE_IP_MC)
  {
    res = soc_pb_pp_ihb_fec_entry_ip_tbl_set_unsafe(
            unit,
            fec_ndx,
            &ip_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
    goto exit;
  }
  if (fec_entry->type == SOC_PB_PP_FEC_TYPE_MPLS_LSR)
  {
    mpls_tbl_data.destination = dest_enc;
    mpls_tbl_data.out_lif = outlif_enc;
    mpls_tbl_data.out_vsi_lsb = fec_entry->app_info.out_rif;
    res = soc_pb_pp_ihb_fec_entry_default_tbl_set_unsafe(
            unit,
            fec_ndx,
            &mpls_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  }

  if (
        fec_entry->type == SOC_PB_PP_FEC_TYPE_BRIDGING_WITH_AC ||
        fec_entry->type == SOC_PB_PP_FEC_TYPE_BRIDGING_INTO_TUNNEL ||
        fec_entry->type == SOC_PB_PP_FEC_TYPE_TRILL_MC ||
        fec_entry->type == SOC_PB_PP_FEC_TYPE_TRILL_UC
     )
  {
    eth_or_trill_tbl_data.destination = dest_enc;
    if (fec_entry->type == SOC_PB_PP_FEC_TYPE_TRILL_MC)
    {
      eth_or_trill_tbl_data.is_out_lif = 0;
      eth_or_trill_tbl_data.out_lif_or_dist_tree_nick = fec_entry->app_info.dist_tree_nick;
    }
    else
    {
      eth_or_trill_tbl_data.is_out_lif = 1;
      eth_or_trill_tbl_data.out_lif_or_dist_tree_nick = outlif_enc;
    }

    res = soc_pb_pp_ihb_fec_entry_eth_or_trill_tbl_set_unsafe(
            unit,
            fec_ndx,
            &eth_or_trill_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_fec_one_entry_set()", 0, 0);
}

STATIC uint32
  soc_pb_pp_frwrd_fec_one_entry_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                       fec_ndx,
    SOC_SAND_OUT  SOC_PB_PP_FRWRD_FEC_ENTRY_INFO    *fec_entry
  )
{
  uint32
    dest_enc=0,
    outlif_enc=0;
  SOC_PB_PP_IHB_FEC_ENTRY_GENERAL_TBL_DATA
    dest_tbl_data;
  SOC_PB_PP_IHB_FEC_ENTRY_ETH_OR_TRILL_TBL_DATA
    eth_or_trill_tbl_data;
  SOC_PB_PP_IHB_FEC_ENTRY_IP_TBL_DATA
    ip_tbl_data;
  SOC_PB_PP_IHB_FEC_ENTRY_DEFAULT_TBL_DATA
    mpls_tbl_data;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_FEC_ONE_ENTRY_GET);

  /* set the destination field */
  res = soc_pb_pp_sw_db_fec_entry_type_get(
          unit,
          fec_ndx,
          &fec_entry->type
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  /* extract from fec entry, destination and outlif encoding */
  if (fec_entry->type == SOC_PB_PP_FEC_TYPE_DROP || fec_entry->type == SOC_PB_PP_NOF_FEC_TYPES)
  {
    res = soc_pb_pp_ihb_fec_entry_general_tbl_get_unsafe(
            unit,
            fec_ndx,
            &dest_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);
    dest_enc = dest_tbl_data.destination;
  }
  /* get before set, only to enable sharing same entry between IP-MC and IP-UC, if needed*/
  if (fec_entry->type == SOC_PB_PP_FEC_TYPE_IP_UC || fec_entry->type == SOC_PB_PP_FEC_TYPE_IP_MC)
  {
    res = soc_pb_pp_ihb_fec_entry_ip_tbl_get_unsafe(
            unit,
            fec_ndx,
            &ip_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    dest_enc = ip_tbl_data.destination;
    if (fec_entry->type == SOC_PB_PP_FEC_TYPE_IP_UC)
    {
      fec_entry->app_info.out_rif = ip_tbl_data.rif;
      /* hw mapping: strict = 0, loose = 1*/
      fec_entry->rpf_info.rpf_mode = soc_pb_pp_frwrd_fec_rpf_mode_from_hw_val_map(ip_tbl_data.uc_rpf_mode, TRUE);
      outlif_enc = ip_tbl_data.out_lif_lsb;
      outlif_enc |= ip_tbl_data.out_lif_type << 13;
    }
    else if (fec_entry->type == SOC_PB_PP_FEC_TYPE_IP_MC)
    {
      fec_entry->rpf_info.rpf_mode = soc_pb_pp_frwrd_fec_rpf_mode_from_hw_val_map(ip_tbl_data.mc_rpf_mode, FALSE);
      if (fec_entry->rpf_info.rpf_mode == SOC_PB_PP_FRWRD_FEC_RPF_MODE_MC_EXPLICIT)
      {
        fec_entry->rpf_info.expected_in_rif = ip_tbl_data.rif;
      }
      /* no need to update outlif as it not used for IP-MC, outlif is according to MC.cud */
    }
  }
  if (fec_entry->type == SOC_PB_PP_FEC_TYPE_MPLS_LSR)
  {
    res = soc_pb_pp_ihb_fec_entry_default_tbl_get_unsafe(
            unit,
            fec_ndx,
            &mpls_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
    dest_enc = mpls_tbl_data.destination;
    outlif_enc = mpls_tbl_data.out_lif;
    fec_entry->app_info.out_rif = mpls_tbl_data.out_vsi_lsb;
  }

  if (
        fec_entry->type == SOC_PB_PP_FEC_TYPE_BRIDGING_WITH_AC ||
        fec_entry->type == SOC_PB_PP_FEC_TYPE_BRIDGING_INTO_TUNNEL ||
        fec_entry->type == SOC_PB_PP_FEC_TYPE_TRILL_MC ||
        fec_entry->type == SOC_PB_PP_FEC_TYPE_TRILL_UC
     )
  {
    res = soc_pb_pp_ihb_fec_entry_eth_or_trill_tbl_get_unsafe(
            unit,
            fec_ndx,
            &eth_or_trill_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

    dest_enc = eth_or_trill_tbl_data.destination;
    if (fec_entry->type == SOC_PB_PP_FEC_TYPE_TRILL_MC)
    {
      fec_entry->app_info.dist_tree_nick = eth_or_trill_tbl_data.out_lif_or_dist_tree_nick;
    }
    else
    {
      outlif_enc = eth_or_trill_tbl_data.out_lif_or_dist_tree_nick;
    }
  }
  res = soc_pb_pp_frwrd_fec_dest_decode(
          unit,
          dest_enc,
          outlif_enc,
          fec_entry
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_fec_one_entry_get()", 0, 0);
}

STATIC uint32
  soc_pb_pp_frwrd_fec_size_get(
    SOC_SAND_IN   int          unit,
    SOC_SAND_IN   uint32           fec_ndx,
    SOC_SAND_OUT  uint32          *fec_size
  )
{
  SOC_PB_PP_IHB_FEC_SUPER_ENTRY_TBL_DATA
    ihb_fec_super_entry_tbl_data;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_FEC_SIZE_GET);

  SOC_SAND_CHECK_NULL_INPUT(fec_size);

  /* if not even then size is 1*/
  if (!SOC_PB_PP_FRWRD_FEC_EVEN_FEC(fec_ndx))
  {
    *fec_size = 1;
    goto exit;
  }
  /* get size from super FEC entry*/
  res = soc_pb_pp_ihb_fec_super_entry_tbl_get_unsafe(
          unit,
          SOC_PB_PP_FRWRD_FEC_SUPER_ENTRY_INDEX(fec_ndx),
          &ihb_fec_super_entry_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  /* map index into size */
  res = soc_pb_pp_frwrd_fec_index_to_size_get(
          unit,
          ihb_fec_super_entry_tbl_data.ecmp_group_size_index,
          fec_size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_fec_size_get()", 0, 0);
}


STATIC uint32
  soc_pb_pp_frwrd_fec_protect_type_get(
    SOC_SAND_IN   int                     unit,
    SOC_SAND_IN   uint32                      fec_ndx,
    SOC_SAND_OUT  SOC_PB_PP_FRWRD_FEC_PROTECT_TYPE  *fec_protect_type
  )
{
  SOC_PB_PP_IHB_FEC_SUPER_ENTRY_TBL_DATA
    ihb_fec_super_entry_tbl_data;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_FEC_PROTECT_TYPE_GET);

  SOC_SAND_CHECK_NULL_INPUT(fec_protect_type);

  /* if not even then protect_type is 1*/
  if (!SOC_PB_PP_FRWRD_FEC_EVEN_FEC(fec_ndx))
  {
    *fec_protect_type = SOC_PB_PP_FRWRD_FEC_PROTECT_TYPE_NONE;
    goto exit;
  }
  /* get protect_type from super FEC entry*/
  res = soc_pb_pp_ihb_fec_super_entry_tbl_get_unsafe(
          unit,
          SOC_PB_PP_FRWRD_FEC_SUPER_ENTRY_INDEX(fec_ndx),
          &ihb_fec_super_entry_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  /* map value into protect_type */

  if (ihb_fec_super_entry_tbl_data.protection_pointer == SOC_PB_PP_FRWRD_FEC_PROTECT_PTR_VAL_NO_PROTECT)
  {
    *fec_protect_type = SOC_PB_PP_FRWRD_FEC_PROTECT_TYPE_NONE;
  }
  else if (ihb_fec_super_entry_tbl_data.protection_pointer == SOC_PB_PP_FRWRD_FEC_PROTECT_PTR_VAL_FAC_PROTECT)
  {
    *fec_protect_type = SOC_PB_PP_FRWRD_FEC_PROTECT_TYPE_FACILITY;
  }
  else
  {
    *fec_protect_type = SOC_PB_PP_FRWRD_FEC_PROTECT_TYPE_PATH;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_fec_protect_type_get()", 0, 0);
}



/* mapping system port to index into the protection table */
STATIC uint32
  soc_pb_pp_frwrd_fec_facility_port_to_index(
    SOC_SAND_IN  SOC_SAND_PP_SYS_PORT_ID                       *sys_port_ndx
  )
{

  if (sys_port_ndx->sys_port_type == SOC_SAND_PP_SYS_PORT_TYPE_LAG)
  {
    return sys_port_ndx->sys_id + SOC_PB_PP_FRWRD_FEC_PORTECT_DEST_MAX_PHY_CHECK + 1;
  }
  else
  {
    return sys_port_ndx->sys_id;
  }
}

STATIC uint32
  soc_pb_pp_frwrd_fec_hash_index_to_hw_val(
    SOC_SAND_IN  uint8                   hash_index
  )
{
  switch(hash_index)
  {
  case 0:/* 4'd0 - Use polynomial 0x17111 */
    return 1;
  case 1:/* 4'b1 - Use polynomial 0x10491 */
    return 0;
  case 2:/* 4'b2 - Use polynomial 0x155f5 */
    return 3;
  case 3:/* 4'b3 - Use polynomial 0x19715 */
    return 2;
  case 4:/* 4'b4 - Use polynomial 0x13965 */
    return 4;
  case 5:/* 4'b5 - Use polynomial 0x1698d */
    return 5;
  case 6:/* 4'b6 - Use polynomial 0x1105d */
    return 6;
  case 7:/* 4'b7 - Use LB-Key-Packet-Data directly */
    return 7;
  case 8:/* 4'b8 - Use counter incremented every packet */
    return 8;
  default:
  case 9:/* 4'b9 - User counter incremented every two clocks */
    return 9;
  }
}

STATIC uint32
  soc_pb_pp_frwrd_fec_hash_index_from_hw_val(
    SOC_SAND_IN  uint8                   hash_index_val
  )
{
  switch(hash_index_val)
  {
  case 1:/* 4'd0 - Use polynomial 0x17111 */
    return 0;
  case 0:/* 4'b1 - Use polynomial 0x10491 */
    return 1;
  case 3:/* 4'b2 - Use polynomial 0x155f5 */
    return 2;
  case 2:/* 4'b3 - Use polynomial 0x19715 */
    return 3;
  case 4:/* 4'b4 - Use polynomial 0x13965 */
    return 4;
  case 5:/* 4'b5 - Use polynomial 0x1698d */
    return 5;
  case 6:/* 4'b6 - Use polynomial 0x1105d */
    return 6;
  case 7:/* 4'b7 - Use LB-Key-Packet-Data directly */
    return 7;
  case 8:/* 4'b8 - Use counter incremented every packet */
    return 8;
  default:
  case 9:/* 4'b9 - User counter incremented every two clocks */
    return 9;
  }
}


uint32
  soc_pb_pp_frwrd_fec_init_unsafe(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_IHB_FEC_ENTRY_ACCESSED_TBL_DATA
    ihb_fec_entry_accessed_tbl_data;
  SOC_PB_PP_IHB_DESTINATION_STATUS_TBL_DATA
    ihb_destination_status_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  ihb_fec_entry_accessed_tbl_data.fec_entry_accessed = 0;
  soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
  res = soc_pb_pp_ihb_fec_entry_accessed_tbl_set_unsafe(
          unit,
          SOC_PB_PP_FIRST_TBL_ENTRY,
          &ihb_fec_entry_accessed_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  ihb_destination_status_tbl_data.destination_valid = 0xff;
  soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
  res = soc_pb_pp_ihb_destination_status_tbl_set_unsafe(
          unit,
          SOC_PB_PP_FIRST_TBL_ENTRY,
          &ihb_destination_status_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_SINGLE_OPERATION);
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_fec_init_unsafe()", 0, 0);
}

/*********************************************************************
*     Setting global information of the FEC table (including
 *     resources to use)
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_fec_glbl_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_GLBL_INFO                 *glbl_info
  )
{
  uint32
    indx;
  SOC_PB_PP_IHB_ECMP_GROUP_SIZE_TBL_DATA
    ihb_ecmp_group_size_tbl_data;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_FEC_GLBL_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(glbl_info);

  for (indx = 0; indx < glbl_info->ecmp_sizes_nof_entries; ++indx)
  {
    ihb_ecmp_group_size_tbl_data.ecmp_group_size = glbl_info->ecmp_sizes[indx];
    res = soc_pb_pp_ihb_ecmp_group_size_tbl_set_unsafe(
            unit,
            indx,
            &ihb_ecmp_group_size_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }

  res = soc_pb_pp_sw_db_fec_glbl_info_set(
          unit,
          glbl_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_fec_glbl_info_set_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_frwrd_fec_glbl_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_GLBL_INFO                 *glbl_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_FEC_GLBL_INFO_SET_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_FRWRD_FEC_GLBL_INFO, glbl_info, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_fec_glbl_info_set_verify()", 0, 0);
}

uint32
  soc_pb_pp_frwrd_fec_glbl_info_get_verify(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_FEC_GLBL_INFO_GET_VERIFY);


  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_fec_glbl_info_get_verify()", 0, 0);
}

/*********************************************************************
*     Setting global information of the FEC table (including
 *     resources to use)
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_fec_glbl_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_FEC_GLBL_INFO                 *glbl_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_FEC_GLBL_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(glbl_info);

  SOC_PB_PP_FRWRD_FEC_GLBL_INFO_clear(unit, glbl_info);

  res = soc_pb_pp_sw_db_fec_glbl_info_get(
          unit,
          glbl_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_fec_glbl_info_get_unsafe()", 0, 0);
}

/*********************************************************************
*     Add FEC entry. May include protection of type Facility
 *     or Path.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_fec_entry_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FEC_ID                              fec_ndx,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_PROTECT_TYPE              protect_type,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_ENTRY_INFO                *working_fec,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_ENTRY_INFO                *protect_fec,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_PROTECT_INFO              *protect_info,
    SOC_SAND_OUT uint8                                 *success
  )
{

  SOC_PB_PP_IHB_FEC_SUPER_ENTRY_TBL_DATA
    ihb_fec_super_entry_tbl_data;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_FEC_ENTRY_ADD_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(working_fec);
  SOC_SAND_CHECK_NULL_INPUT(protect_fec);
  SOC_SAND_CHECK_NULL_INPUT(protect_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  /* if even FEC then update super FEC entry */
  if(SOC_PB_PP_FRWRD_FEC_EVEN_FEC(fec_ndx))
  {
    res = soc_pb_pp_frwrd_fec_size_to_index_get(
            unit,
            1,/* fec size is one, as no ECMP*/
            &(ihb_fec_super_entry_tbl_data.ecmp_group_size_index)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    /* protection pointer setting*/
    if (protect_type == SOC_PB_PP_FRWRD_FEC_PROTECT_TYPE_NONE)
    {
      ihb_fec_super_entry_tbl_data.protection_pointer = SOC_PB_PP_FRWRD_FEC_PROTECT_PTR_VAL_NO_PROTECT;
    }
    /* if path protection set oam-instance in protection pointer */
    else if (protect_type == SOC_PB_PP_FRWRD_FEC_PROTECT_TYPE_PATH)
    {
      ihb_fec_super_entry_tbl_data.protection_pointer = protect_info->oam_instance_id;
    }
    else
    {
      ihb_fec_super_entry_tbl_data.protection_pointer = SOC_PB_PP_FRWRD_FEC_PROTECT_PTR_VAL_FAC_PROTECT;
    }
    res = soc_pb_pp_ihb_fec_super_entry_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FRWRD_FEC_SUPER_ENTRY_INDEX(fec_ndx),
            &ihb_fec_super_entry_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }
 /*
  * write working FEC
  */
  res = soc_pb_pp_frwrd_fec_one_entry_set(
          unit,
          fec_ndx,
          working_fec
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
 /*
  * if there is protection, write protecting FEC
  */
  if (protect_type != SOC_PB_PP_FRWRD_FEC_PROTECT_TYPE_NONE)
  {
    res = soc_pb_pp_frwrd_fec_one_entry_set(
            unit,
            SOC_PB_PP_FRWRD_FEC_NEXT_ENTRY(fec_ndx),
            protect_fec
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }

  *success = TRUE;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_fec_entry_add_unsafe()", fec_ndx, 0);
}

uint32
  soc_pb_pp_frwrd_fec_entry_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FEC_ID                              fec_ndx,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_PROTECT_TYPE              protect_type,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_ENTRY_INFO                *working_fec,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_ENTRY_INFO                *protect_fec,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_PROTECT_INFO              *protect_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_FEC_ENTRY_ADD_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(fec_ndx, SOC_PB_PP_FEC_ID_MAX, SOC_PB_PP_FEC_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(protect_type, SOC_PB_PP_FRWRD_FEC_PROTECT_TYPE_MAX, SOC_PB_PP_FRWRD_FEC_PROTECT_TYPE_OUT_OF_RANGE_ERR, 20, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_FRWRD_FEC_ENTRY_INFO, working_fec, 30, exit);

  /* if FEC with protection then has to be in even FEC-index*/
  if (protect_type != SOC_PB_PP_FRWRD_FEC_PROTECT_TYPE_NONE && !SOC_PB_PP_FRWRD_FEC_EVEN_FEC(fec_ndx))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FRWRD_FEC_PROTECTED_NOT_EVEN_INDEX_ERR,30,exit)
  }

  if (protect_type != SOC_PB_PP_FRWRD_FEC_PROTECT_TYPE_NONE)
  {
    SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_FRWRD_FEC_ENTRY_INFO, protect_fec, 40, exit);
    if (protect_type == SOC_PB_PP_FRWRD_FEC_PROTECT_TYPE_PATH)
    {
      SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_FRWRD_FEC_PROTECT_INFO, protect_info, 50, exit);
    }
  }

  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_fec_entry_add_verify()", fec_ndx, 0);
}

/*********************************************************************
*     Add ECMP to the FEC table.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_fec_ecmp_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FEC_ID                              fec_ndx,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_ENTRY_INFO                *fec_array,
    SOC_SAND_IN  uint32                                  nof_entries,
    SOC_SAND_OUT uint8                                 *success
  )
{
  uint32
    indx,
    fec_tbl_indx;
  SOC_PB_PP_IHB_FEC_SUPER_ENTRY_TBL_DATA
    ihb_fec_super_entry_tbl_data;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_FEC_ECMP_ADD_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(fec_array);
  SOC_SAND_CHECK_NULL_INPUT(success);

  /* if even FEC then update super FEC entry/ has to be!*/
  if(SOC_PB_PP_FRWRD_FEC_EVEN_FEC(fec_ndx))
  {
    res = soc_pb_pp_frwrd_fec_size_to_index_get(
            unit,
            nof_entries,/* fec size is one, as no ECMP*/
            &(ihb_fec_super_entry_tbl_data.ecmp_group_size_index)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    /* protection pointer setting to none, as no protection with ECMP*/
    ihb_fec_super_entry_tbl_data.protection_pointer = SOC_PB_PP_FRWRD_FEC_PROTECT_PTR_VAL_NO_PROTECT;
    
    res = soc_pb_pp_ihb_fec_super_entry_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FRWRD_FEC_SUPER_ENTRY_INDEX(fec_ndx),
            &ihb_fec_super_entry_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }
 /*
  * write ECMP entries into HW
  */
  for (fec_tbl_indx=fec_ndx, indx=0; indx < nof_entries; ++indx)
  {
    res = soc_pb_pp_frwrd_fec_one_entry_set(
            unit,
            fec_tbl_indx,
            &(fec_array[indx])
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    fec_tbl_indx = SOC_PB_PP_FRWRD_FEC_NEXT_ENTRY(fec_tbl_indx);
  }

  *success = TRUE;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_fec_ecmp_add_unsafe()", fec_ndx, 0);
}

uint32
  soc_pb_pp_frwrd_fec_ecmp_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FEC_ID                              fec_ndx,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_ENTRY_INFO                *fec_array,
    SOC_SAND_IN  uint32                                  nof_entries
  )
{
  uint32
    indx;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_FEC_ECMP_ADD_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(fec_ndx, SOC_PB_PP_FEC_ID_MAX, SOC_PB_PP_FEC_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(fec_ndx+nof_entries-1, SOC_PB_PP_FEC_ID_MAX, SOC_PB_PP_FEC_ID_OUT_OF_RANGE_ERR, 15, exit);

  for (indx=0; indx < nof_entries; ++indx)
  {
    SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_FRWRD_FEC_ENTRY_INFO, &fec_array[indx], 100+indx, exit);
  }
  SOC_SAND_ERR_IF_ABOVE_MAX(nof_entries, SOC_PB_PP_FRWRD_FEC_NOF_ENTRIES_MAX, SOC_PB_PP_FRWRD_FEC_NOF_ENTRIES_OUT_OF_RANGE_ERR, 30, exit);

  /* if ECMP then has to be in even FEC-index*/
  if (nof_entries > 1 && !SOC_PB_PP_FRWRD_FEC_EVEN_FEC(fec_ndx))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FRWRD_FEC_ECMP_NOT_EVEN_INDEX_ERR,40,exit)
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_fec_ecmp_add_verify()", fec_ndx, 0);
}

/*********************************************************************
*     Update the content of block of entries from the ECMP.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_fec_ecmp_update_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FEC_ID                              fec_ndx,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_ENTRY_INFO                *fec_array,
    SOC_SAND_IN  SOC_SAND_U32_RANGE                            *fec_range
  )
{
  uint32
    fec_tbl_indx,
    indx;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_FEC_ECMP_UPDATE_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(fec_array);
  SOC_SAND_CHECK_NULL_INPUT(fec_range);

 /*
  * write ECMP (sub-range) entries into HW
  */
  for (fec_tbl_indx=fec_ndx+fec_range->start, indx=0; indx < fec_range->end - fec_range->start + 1; ++indx)
  {
    res = soc_pb_pp_frwrd_fec_one_entry_set(
            unit,
            fec_tbl_indx,
            &(fec_array[indx])
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    fec_tbl_indx = SOC_PB_PP_FRWRD_FEC_NEXT_ENTRY(fec_tbl_indx);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_fec_ecmp_update_unsafe()", fec_ndx, 0);
}

uint32
  soc_pb_pp_frwrd_fec_ecmp_update_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FEC_ID                              fec_ndx,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_ENTRY_INFO                *fec_array,
    SOC_SAND_IN  SOC_SAND_U32_RANGE                            *fec_range
  )
{
  uint32
    indx,
    fec_size;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_FEC_ECMP_UPDATE_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(fec_ndx, SOC_PB_PP_FEC_ID_MAX, SOC_PB_PP_FEC_ID_OUT_OF_RANGE_ERR, 10, exit);
  
  /* check if range is legal */
  if (fec_range->end < fec_range->start)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FRWRD_FEC_UPDATE_RANGE_ILLEGAL_ERR,20,exit);
  }
  for (indx=0; indx < fec_range->end - fec_range->start; ++indx)
  {
    SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_FRWRD_FEC_ENTRY_INFO, &fec_array[indx], 100+indx, exit);
  }
  /* check Range fill in ECMP range*/
  res = soc_pb_pp_frwrd_fec_size_get(
          unit,
          fec_ndx,
          &fec_size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  
  if (fec_range->end >= fec_size)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FRWRD_FEC_UPDATE_RANGE_OUT_OF_ECMP_ERR,40 , exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_fec_ecmp_update_verify()", fec_ndx, 0);
}

/*********************************************************************
*     Get the usage/allocation of the FEC entry pointed by
 *     fec_ndx (ECMP/FEC/protected FEC/none).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_fec_entry_use_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FEC_ID                              fec_ndx,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_FEC_ENTRY_USE_INFO            *fec_entry_info
  )
{
  SOC_PB_PP_FRWRD_FEC_PROTECT_TYPE
    protect_type;
  uint32
    fec_size;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_FEC_ENTRY_USE_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(fec_entry_info);

  SOC_PB_PP_FRWRD_FEC_ENTRY_USE_INFO_clear(fec_entry_info);

  /* check if ECMP*/
  /* note when entry is both ECMP and protected, then it considered as ECMP without protection */
  res = soc_pb_pp_frwrd_fec_size_get(
          unit,
          fec_ndx,
          &fec_size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  if (fec_size > 1)
  {
    fec_entry_info->nof_entries = fec_size;
    fec_entry_info->type = SOC_PB_PP_FRWRD_FEC_ENTRY_USE_TYPE_ECMP;
    goto exit;
  }

  /* get protection type, if any */
  res = soc_pb_pp_frwrd_fec_protect_type_get(
          unit,
          fec_ndx,
          &protect_type
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  if (protect_type == SOC_PB_PP_FRWRD_FEC_PROTECT_TYPE_NONE)
  {
    fec_entry_info->type = SOC_PB_PP_FRWRD_FEC_ENTRY_USE_TYPE_ONE_FEC;
    fec_entry_info->nof_entries = 1;
  }
  else if (protect_type == SOC_PB_PP_FRWRD_FEC_PROTECT_TYPE_FACILITY)
  {
    fec_entry_info->type = SOC_PB_PP_FRWRD_FEC_ENTRY_USE_TYPE_FACILITY_PROTECT;
    fec_entry_info->nof_entries = 2;
  }
  else if (protect_type == SOC_PB_PP_FRWRD_FEC_PROTECT_TYPE_PATH)
  {
    fec_entry_info->type = SOC_PB_PP_FRWRD_FEC_ENTRY_USE_TYPE_PATH_PROTECT;
    fec_entry_info->nof_entries = 2;
  }
  else
  {
    fec_entry_info->type = SOC_PB_PP_FRWRD_FEC_ENTRY_USE_TYPE_NONE;
    fec_entry_info->nof_entries = 0;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_fec_entry_use_info_get_unsafe()", fec_ndx, 0);
}

uint32
  soc_pb_pp_frwrd_fec_entry_use_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FEC_ID                              fec_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_FEC_ENTRY_USE_INFO_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(fec_ndx, SOC_PB_PP_FEC_ID_MAX, SOC_PB_PP_FEC_ID_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_fec_entry_use_info_get_verify()", fec_ndx, 0);
}

/*********************************************************************
*     Get FEC entry from the FEC table. May include
 *     protection.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_fec_entry_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FEC_ID                              fec_ndx,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_FEC_PROTECT_TYPE              *protect_type,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_FEC_ENTRY_INFO                *working_fec,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_FEC_ENTRY_INFO                *protect_fec,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_FEC_PROTECT_INFO              *protect_info
  )
{
  SOC_PB_PP_IHB_FEC_SUPER_ENTRY_TBL_DATA
    ihb_fec_super_entry_tbl_data;
  uint32
    fec_size;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_FEC_ENTRY_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(protect_type);
  SOC_SAND_CHECK_NULL_INPUT(working_fec);
  SOC_SAND_CHECK_NULL_INPUT(protect_fec);
  SOC_SAND_CHECK_NULL_INPUT(protect_info);

  SOC_PB_PP_FRWRD_FEC_ENTRY_INFO_clear(working_fec);
  SOC_PB_PP_FRWRD_FEC_ENTRY_INFO_clear(protect_fec);
  SOC_PB_PP_FRWRD_FEC_PROTECT_INFO_clear(protect_info);


  /* check size */
  res = soc_pb_pp_frwrd_fec_size_get(
          unit,
          fec_ndx,
          &fec_size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
/*  if (fec_size > 1)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FRWRD_FEC_EXPECT_NON_ECMP_ERR,20,exit);
  }*/
 res = soc_pb_pp_frwrd_fec_protect_type_get(
          unit,
          fec_ndx,
          protect_type
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  /* if path protection set oam-instance in protection pointer */
  if (*protect_type == SOC_PB_PP_FRWRD_FEC_PROTECT_TYPE_PATH)
  {
    res = soc_pb_pp_ihb_fec_super_entry_tbl_get_unsafe(
            unit,
            SOC_PB_PP_FRWRD_FEC_SUPER_ENTRY_INDEX(fec_ndx),
            &ihb_fec_super_entry_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    protect_info->oam_instance_id = ihb_fec_super_entry_tbl_data.protection_pointer;
  }
 /*
  * read working FEC
  */
  res = soc_pb_pp_frwrd_fec_one_entry_get(
          unit,
          fec_ndx,
          working_fec
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
 /*
  * if there is protection, read protecting FEC
  */
  if (*protect_type != SOC_PB_PP_FRWRD_FEC_PROTECT_TYPE_NONE)
  {
    res = soc_pb_pp_frwrd_fec_one_entry_get(
            unit,
            SOC_PB_PP_FRWRD_FEC_NEXT_ENTRY(fec_ndx),
            protect_fec
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_fec_entry_get_unsafe()", fec_ndx, 0);
}

uint32
  soc_pb_pp_frwrd_fec_entry_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FEC_ID                              fec_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_FEC_ENTRY_GET_VERIFY);

  SOC_SAND_ERR_IF_OUT_OF_RANGE((/* just to avoid coverity defect... */int)fec_ndx,
    SOC_PB_PP_FEC_ID_MIN, SOC_PB_PP_FEC_ID_MAX, SOC_PB_PP_FEC_ID_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_fec_entry_get_verify()", fec_ndx, 0);
}

/*********************************************************************
*     Update content of range of the ECMP.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_fec_ecmp_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FEC_ID                              fec_ndx,
    SOC_SAND_IN  SOC_SAND_U32_RANGE                            *fec_range,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_FEC_ENTRY_INFO                *fec_array,
    SOC_SAND_OUT uint32                                  *nof_entries
  )
{
  uint32
    fec_size,
    fec_tbl_indx,
    indx;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_FEC_ECMP_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(fec_range);
  SOC_SAND_CHECK_NULL_INPUT(fec_array);
  SOC_SAND_CHECK_NULL_INPUT(nof_entries);

  SOC_PB_PP_FRWRD_FEC_ENTRY_INFO_clear(fec_array);

  res = soc_pb_pp_frwrd_fec_size_get(
          unit,
          fec_ndx,
          &fec_size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
 /*
  * read ECMP entries from HW
  */
  for (fec_tbl_indx=fec_ndx, indx=0; indx < fec_size; ++indx)
  {
    SOC_PB_PP_FRWRD_FEC_ENTRY_INFO_clear(&(fec_array[indx]));

    res = soc_pb_pp_frwrd_fec_one_entry_get(
            unit,
            fec_tbl_indx,
            &(fec_array[indx])
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    fec_tbl_indx = SOC_PB_PP_FRWRD_FEC_NEXT_ENTRY(fec_tbl_indx);
  }

  *nof_entries = fec_size;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_fec_ecmp_get_unsafe()", fec_ndx, 0);
}

uint32
  soc_pb_pp_frwrd_fec_ecmp_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FEC_ID                              fec_ndx,
    SOC_SAND_IN  SOC_SAND_U32_RANGE                            *fec_range
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_FEC_ECMP_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(fec_ndx, SOC_PB_PP_FEC_ID_MAX, SOC_PB_PP_FEC_ID_OUT_OF_RANGE_ERR, 10, exit);
  /* SOC_PB_PP_STRUCT_VERIFY(SOC_SAND_U32_RANGE, fec_range, 20, exit); */

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_fec_ecmp_get_verify()", fec_ndx, 0);
}

/*********************************************************************
*     Remove FEC entry/entries associated with fec_ndx.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_fec_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FEC_ID                              fec_ndx
  )
{
  SOC_PB_PP_FRWRD_FEC_ENTRY_INFO
    fec_entry;
  SOC_PB_PP_FRWRD_FEC_ENTRY_USE_INFO
    fec_entry_use;
  SOC_PB_PP_FRWRD_FEC_PROTECT_INFO
    protect_info;
  uint8
    success;
  uint32
    indx,
    tbl_indx;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_FEC_REMOVE_UNSAFE);
  SOC_PB_PP_FRWRD_FEC_ENTRY_INFO_clear(&fec_entry);
  SOC_PB_PP_FRWRD_FEC_PROTECT_INFO_clear(&protect_info);
  /* remove is actually set to drop*/
  fec_entry.type = SOC_PB_PP_FEC_TYPE_DROP;

  res = soc_pb_pp_frwrd_fec_entry_use_info_get_unsafe(
          unit,
          fec_ndx,
          &fec_entry_use
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  if (fec_entry_use.nof_entries == 0)
  {
    goto exit;
  }
 /*
  * first FEC entry add, calling the API
  * to update Super-FEC info
  */
  res = soc_pb_pp_frwrd_fec_entry_add_unsafe(
          unit,
          fec_ndx,
          SOC_PB_PP_FRWRD_FEC_PROTECT_TYPE_NONE,
          &fec_entry,
          &fec_entry,
          &protect_info,
          &success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = soc_pb_pp_sw_db_fec_entry_type_set(
          unit,
          fec_ndx,
          SOC_PB_PP_NOF_FEC_TYPES
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);

  if (fec_entry_use.nof_entries == 1)
  {
    goto exit;
  }

  /* rest set directly into the HW*/
  fec_entry.type = SOC_PB_PP_NOF_FEC_TYPES;
  tbl_indx = fec_ndx;
  for (indx = 0 ; indx<fec_entry_use.nof_entries; ++indx)
  {
    res = soc_pb_pp_frwrd_fec_one_entry_set(
            unit,
            tbl_indx,
            &fec_entry
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    tbl_indx = SOC_PB_PP_FRWRD_FEC_NEXT_ENTRY(tbl_indx);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_fec_remove_unsafe()", fec_ndx, 0);
}

uint32
  soc_pb_pp_frwrd_fec_remove_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FEC_ID                              fec_ndx
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_FEC_REMOVE_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(fec_ndx, SOC_PB_PP_FEC_ID_MAX, SOC_PB_PP_FEC_ID_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_fec_remove_verify()", fec_ndx, 0);
}

/*********************************************************************
*     Set the status of the OAM instance. For all PATH
 *     protected FECs that point to this instance, the working
 *     FEC will be used if up is TRUE, and the protect FEC will
 *     be used otherwise.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_fec_protection_oam_instance_status_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  oam_instance_ndx,
    SOC_SAND_IN  uint8                                 up
  )
{
  SOC_PB_PP_IHB_PATH_SELECT_TBL_DATA
    ihb_path_select_tbl_data;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_FEC_PROTECTION_OAM_INSTANCE_STATUS_SET_UNSAFE);

  res = soc_pb_pp_ihb_path_select_tbl_get_unsafe(
          unit,
          SOC_PB_PP_IHB_PATH_SELECT_TBL_KEY_ENTRY_OFFSET(oam_instance_ndx),
          &ihb_path_select_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  ihb_path_select_tbl_data.path_select[oam_instance_ndx%8] = SOC_SAND_BOOL2NUM_INVERSE(up);

  res = soc_pb_pp_ihb_path_select_tbl_set_unsafe(
          unit,
          SOC_PB_PP_IHB_PATH_SELECT_TBL_KEY_ENTRY_OFFSET(oam_instance_ndx),
          &ihb_path_select_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);



exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_fec_protection_oam_instance_status_set_unsafe()", oam_instance_ndx, 0);
}

uint32
  soc_pb_pp_frwrd_fec_protection_oam_instance_status_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  oam_instance_ndx,
    SOC_SAND_IN  uint8                                 up
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_FEC_PROTECTION_OAM_INSTANCE_STATUS_SET_VERIFY);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(oam_instance_ndx, SOC_PB_PP_FRWRD_FEC_OAM_INSTANCE_NDX_MIN, SOC_PB_PP_FRWRD_FEC_OAM_INSTANCE_NDX_MAX, SOC_PB_PP_FRWRD_FEC_OAM_INSTANCE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(up, SOC_PB_PP_FRWRD_FEC_UP_MAX, SOC_PB_PP_FRWRD_FEC_UP_OUT_OF_RANGE_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_fec_protection_oam_instance_status_set_verify()", oam_instance_ndx, 0);
}

uint32
  soc_pb_pp_frwrd_fec_protection_oam_instance_status_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  oam_instance_ndx
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_FEC_PROTECTION_OAM_INSTANCE_STATUS_GET_VERIFY);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(oam_instance_ndx, SOC_PB_PP_FRWRD_FEC_OAM_INSTANCE_NDX_MIN, SOC_PB_PP_FRWRD_FEC_OAM_INSTANCE_NDX_MAX, SOC_PB_PP_FRWRD_FEC_OAM_INSTANCE_NDX_OUT_OF_RANGE_ERR, 10, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_fec_protection_oam_instance_status_get_verify()", oam_instance_ndx, 0);
}

/*********************************************************************
*     Set the status of the OAM instance. For all PATH
 *     protected FECs that point to this instance, the working
 *     FEC will be used if up is TRUE, and the protect FEC will
 *     be used otherwise.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_fec_protection_oam_instance_status_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  oam_instance_ndx,
    SOC_SAND_OUT uint8                                 *up
  )
{
  SOC_PB_PP_IHB_PATH_SELECT_TBL_DATA
    ihb_path_select_tbl_data;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_FEC_PROTECTION_OAM_INSTANCE_STATUS_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(up);

  res = soc_pb_pp_ihb_path_select_tbl_get_unsafe(
          unit,
          SOC_PB_PP_IHB_PATH_SELECT_TBL_KEY_ENTRY_OFFSET(oam_instance_ndx),
          &ihb_path_select_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  *up = SOC_SAND_NUM2BOOL_INVERSE(ihb_path_select_tbl_data.path_select[oam_instance_ndx%8]);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_fec_protection_oam_instance_status_get_unsafe()", oam_instance_ndx, 0);
}

/*********************************************************************
*     Set the status of the System Port (LAG or Physical
 *     port).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_fec_protection_sys_port_status_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_SYS_PORT_ID                       *sys_port_ndx,
    SOC_SAND_IN  uint8                                 up
  )
{
  SOC_PB_PP_IHB_DESTINATION_STATUS_TBL_DATA
    ihb_destination_status_tbl_data;
  uint32
    dest_index,
    tmp;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_FEC_PROTECTION_SYS_PORT_STATUS_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(sys_port_ndx);

  dest_index = soc_pb_pp_frwrd_fec_facility_port_to_index(sys_port_ndx);

  res = soc_pb_pp_ihb_destination_status_tbl_get_unsafe(
          unit,
          SOC_PB_PP_IHB_DESTINATION_STATUS_TBL_KEY_ENTRY_OFFSET(dest_index),
          &ihb_destination_status_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  tmp = SOC_SAND_BOOL2NUM(up);
  res = soc_sand_set_field(
          &(ihb_destination_status_tbl_data.destination_valid),
          dest_index % 8,
          dest_index % 8,
          tmp
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
         
  res = soc_pb_pp_ihb_destination_status_tbl_set_unsafe(
          unit,
          SOC_PB_PP_IHB_DESTINATION_STATUS_TBL_KEY_ENTRY_OFFSET(dest_index),
          &ihb_destination_status_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_fec_protection_sys_port_status_set_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_frwrd_fec_protection_sys_port_status_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_SYS_PORT_ID                       *sys_port_ndx,
    SOC_SAND_IN  uint8                                 up
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_FEC_PROTECTION_SYS_PORT_STATUS_SET_VERIFY);

  /* if physical port > 4k - 256 then status has to be UP */
  if (sys_port_ndx->sys_port_type == SOC_SAND_PP_SYS_PORT_TYPE_SINGLE_PORT
      && sys_port_ndx->sys_id > SOC_PB_PP_FRWRD_FEC_PORTECT_DEST_MAX_PHY_CHECK
      && up
     )
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FRWRD_FEC_DEST_PHY_PORT_STATUS_ILLEGAL_ERR, 10,exit);
  }
  /* check dest port */
  switch(sys_port_ndx->sys_port_type)
  {
    case SOC_SAND_PP_SYS_PORT_TYPE_SINGLE_PORT:
      SOC_SAND_ERR_IF_ABOVE_MAX(sys_port_ndx->sys_id,SOC_TMC_MAX_SYSTEM_PHYSICAL_PORT_ID,SOC_PB_PP_FRWRD_FEC_DEST_VAL_OUT_OF_RANGE_ERR,16,exit);
    break;
    case SOC_SAND_PP_SYS_PORT_TYPE_LAG:
      SOC_SAND_ERR_IF_ABOVE_MAX(sys_port_ndx->sys_id,SOC_TMC_MAX_LAG_GROUP_ID,SOC_PB_PP_FRWRD_FEC_DEST_VAL_OUT_OF_RANGE_ERR,18,exit);
    break;
    default:
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FRWRD_FEC_DEST_TYPE_OUT_OF_RANGE_ERR, 30,exit);
  }



exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_fec_protection_sys_port_status_set_verify()", 0, 0);
}

uint32
  soc_pb_pp_frwrd_fec_protection_sys_port_status_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_SYS_PORT_ID                       *sys_port_ndx
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_FEC_PROTECTION_SYS_PORT_STATUS_GET_VERIFY);

  /* check dest port */
  switch(sys_port_ndx->sys_port_type)
  {
    case SOC_SAND_PP_SYS_PORT_TYPE_SINGLE_PORT:
      SOC_SAND_ERR_IF_ABOVE_MAX(sys_port_ndx->sys_id,SOC_TMC_MAX_SYSTEM_PHYSICAL_PORT_ID,SOC_PB_PP_FRWRD_FEC_DEST_VAL_OUT_OF_RANGE_ERR,16,exit);
    break;
    case SOC_SAND_PP_SYS_PORT_TYPE_LAG:
      SOC_SAND_ERR_IF_ABOVE_MAX(sys_port_ndx->sys_id,SOC_TMC_MAX_LAG_GROUP_ID,SOC_PB_PP_FRWRD_FEC_DEST_VAL_OUT_OF_RANGE_ERR,18,exit);
    break;
    default:
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FRWRD_FEC_DEST_TYPE_OUT_OF_RANGE_ERR, 30,exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_fec_protection_sys_port_status_get_verify()", 0, 0);
}

/*********************************************************************
*     Set the status of the System Port (LAG or Physical
 *     port).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_fec_protection_sys_port_status_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_SYS_PORT_ID                       *sys_port_ndx,
    SOC_SAND_OUT uint8                                 *up
  )
{
  SOC_PB_PP_IHB_DESTINATION_STATUS_TBL_DATA
    ihb_destination_status_tbl_data;
  uint32
    dest_index,
    tmp;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_FEC_PROTECTION_SYS_PORT_STATUS_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(sys_port_ndx);
  SOC_SAND_CHECK_NULL_INPUT(up);


  dest_index = soc_pb_pp_frwrd_fec_facility_port_to_index(sys_port_ndx);

  res = soc_pb_pp_ihb_destination_status_tbl_get_unsafe(
          unit,
          SOC_PB_PP_IHB_DESTINATION_STATUS_TBL_KEY_ENTRY_OFFSET(dest_index),
          &ihb_destination_status_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  tmp = SOC_SAND_GET_BIT(ihb_destination_status_tbl_data.destination_valid,dest_index % 8);
  *up = SOC_SAND_NUM2BOOL(tmp);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_fec_protection_sys_port_status_get_unsafe()", 0, 0);
}

/*********************************************************************
*     Traverse the FEC table (in the specified range) and get
 *     all the FEC entries that match the given rule.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_fec_get_block_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_MATCH_RULE                *rule,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                  *block_range,
    SOC_SAND_OUT uint32                                  *fec_array,
    SOC_SAND_OUT uint32                                  *nof_entries
  )
{
  uint32
    entry_indx,
    valid_entries=0;
  SOC_PB_PP_FRWRD_FEC_PROTECT_TYPE
    fec_protect_type;
  SOC_PB_PP_IHB_FEC_SUPER_ENTRY_TBL_DATA
    ihb_fec_super_entry_tbl_data;
  SOC_PB_PP_FEC_TYPE
    fec_type;
  SOC_PB_PP_FRWRD_FEC_ENTRY_ACCESSED_INFO
    accessed_info;
  SOC_PB_PP_IHB_FEC_ENTRY_GENERAL_TBL_DATA
    dest_tbl_data;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_FEC_GET_BLOCK_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(rule);
  SOC_SAND_CHECK_NULL_INPUT(block_range);
  SOC_SAND_CHECK_NULL_INPUT(fec_array);
  SOC_SAND_CHECK_NULL_INPUT(nof_entries);

  *nof_entries = 0;

  if (SOC_SAND_TBL_ITER_IS_END(&block_range->iter))
  {
    *nof_entries = 0;
    goto exit;
  }
  if (block_range->iter > SOC_PB_PP_FRWRD_FEC_NOF_ENTRIES_MAX)
  {
    SOC_SAND_TBL_ITER_SET_END(&block_range->iter);
    *nof_entries = 0;
    goto exit;
  }

  for (entry_indx = block_range->iter; ((entry_indx - block_range->iter < block_range->entries_to_scan) && (entry_indx <= SOC_PB_PP_FRWRD_FEC_NOF_ENTRIES_MAX)); entry_indx = SOC_PB_PP_FRWRD_FEC_NEXT_ENTRY(entry_indx))
  {
    /* check if entry is allocated at all*/
    res = soc_pb_pp_sw_db_fec_entry_type_get(
            unit,
            entry_indx,
            &fec_type
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    if (fec_type == SOC_PB_PP_NOF_FEC_TYPES)
    {
      continue;
    }
    /* match rule according to app-type */
    if (rule->type == SOC_PB_PP_FRWRD_FEC_MATCH_RULE_TYPE_APP_TYPE)
    {
      if (fec_type != (SOC_PB_PP_FEC_TYPE)rule->value)
      {
        continue;
      }
    }

    if (rule->type == SOC_PB_PP_FRWRD_FEC_MATCH_RULE_TYPE_PATH_PROTECTED || rule->type == SOC_PB_PP_FRWRD_FEC_MATCH_RULE_TYPE_PATH_PROTECTED)
    {
      res = soc_pb_pp_frwrd_fec_protect_type_get(
              unit,
              entry_indx,
              &fec_protect_type
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

      /* if not same protection type*/
      if ((fec_protect_type == SOC_PB_PP_FRWRD_FEC_PROTECT_TYPE_PATH && rule->type != SOC_PB_PP_FRWRD_FEC_MATCH_RULE_TYPE_PATH_PROTECTED) || 
          (fec_protect_type == SOC_PB_PP_FRWRD_FEC_PROTECT_TYPE_FACILITY && rule->type != SOC_PB_PP_FRWRD_FEC_MATCH_RULE_TYPE_FACILITY_PROTECTED)
         )
      {
        continue;
      }
      /* check if same protected value */
      /* if path protection check the protected OAM-instance*/
      if (rule->type == SOC_PB_PP_FRWRD_FEC_MATCH_RULE_TYPE_PATH_PROTECTED)
      {
        res = soc_pb_pp_ihb_fec_super_entry_tbl_get_unsafe(
                unit,
                entry_indx,
                &ihb_fec_super_entry_tbl_data
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
        /* check protected oam with match rule value*/
        if (ihb_fec_super_entry_tbl_data.protection_pointer != rule->value)
        {
          continue;
        }
      }
      /* if facility protection check the protected system port*/
      if (rule->type == SOC_PB_PP_FRWRD_FEC_MATCH_RULE_TYPE_FACILITY_PROTECTED)
      {
        res = soc_pb_pp_ihb_fec_entry_general_tbl_get_unsafe(
                unit,
                entry_indx,
                &dest_tbl_data
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
        /* check protected oam with match rule value*/
        if (dest_tbl_data.destination != rule->value)
        {
          continue;
        }
      }
    }
    if (rule->type == SOC_PB_PP_FRWRD_FEC_MATCH_RULE_TYPE_ACCESSED)
    {
      res = soc_pb_pp_frwrd_fec_entry_accessed_info_get_unsafe(
              unit,
              entry_indx,
              FALSE,
              &accessed_info
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
      if (accessed_info.accessed == FALSE)
      {
        continue;
      }
    }

    /* entry is in match, get it!*/
    fec_array[valid_entries] = entry_indx;
    ++valid_entries;
    if (valid_entries >= block_range->entries_to_act)
    {
      break;
    }
  }

  *nof_entries = valid_entries;
  block_range->iter = entry_indx+1;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_fec_get_block_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_frwrd_fec_get_block_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_MATCH_RULE                *rule,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                  *block_range
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_FEC_GET_BLOCK_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_FRWRD_FEC_MATCH_RULE, rule, 10, exit);
  /* SOC_PB_PP_STRUCT_VERIFY(SOC_SAND_TABLE_BLOCK_RANGE, block_range, 20, exit); */

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_fec_get_block_verify()", 0, 0);
}

/*********************************************************************
*     Set action to do by the device when a packet accesses
 *     the FEC entry.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_fec_entry_accessed_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FEC_ID                              fec_id_ndx,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_ENTRY_ACCESSED_INFO       *accessed_info
  )
{
  SOC_PB_PP_REGS
    *regs = NULL;
  uint32
    tmp,
    reg_val=0,
    reg_index,
    bit_index;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_FEC_ENTRY_ACCESSED_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(accessed_info);

  /* set trap if accessed*/
  /* if bigger than 63 then cannot be trapped */
  if (fec_id_ndx > SOC_PB_PP_FRWRD_FEC_TRAP_ABLE_MAX_INDEX)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FRWRD_FEC_TRAP_ACCESS_OUT_OF_RANGE_ERR,10,exit)
  }

  if(accessed_info->trap_if_accessed ) {
     SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FRWRD_FEC_TRAP_ACCESS_OUT_OF_RANGE_ERR,10,exit)
  }  

  regs = soc_pb_pp_regs();
  reg_index = fec_id_ndx/32;
  bit_index = fec_id_ndx % 32;
  /* * COVERITY * 
   * fec_id_ndx is verified above to be <= 63, so reg_ndx <= 2 */
  /* coverity[overrun-local] */
  SOC_PB_PP_REG_GET(regs->ihb.trap_if_accessed_reg[reg_index],reg_val, 20, exit);

  tmp = SOC_SAND_BOOL2NUM(accessed_info->trap_if_accessed);
  res = soc_sand_set_field(
          &(reg_val),
          bit_index,
          bit_index,
          tmp
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  SOC_PB_PP_REG_SET(regs->ihb.trap_if_accessed_reg[reg_index],reg_val, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_fec_entry_accessed_info_set_unsafe()", fec_id_ndx, 0);
}

uint32
  soc_pb_pp_frwrd_fec_entry_accessed_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FEC_ID                              fec_id_ndx,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_ENTRY_ACCESSED_INFO       *accessed_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_FEC_ENTRY_ACCESSED_INFO_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(fec_id_ndx, SOC_PB_PP_FEC_ID_MAX, SOC_PB_PP_FEC_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_FRWRD_FEC_ENTRY_ACCESSED_INFO, accessed_info, 20, exit);
  if (fec_id_ndx > SOC_PB_PP_FRWRD_FEC_TRAP_ABLE_MAX_INDEX && accessed_info->trap_if_accessed)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FRWRD_FEC_TRAP_ACCESS_OUT_OF_RANGE_ERR,30,exit)
  }
  if (accessed_info->accessed != 0 )
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FRWRD_FEC_ACCESSED_OUT_OF_RANGE_ERR,40,exit)
  }
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_fec_entry_accessed_info_set_verify()", fec_id_ndx, 0);
}

uint32
  soc_pb_pp_frwrd_fec_entry_accessed_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FEC_ID                              fec_id_ndx
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_FEC_ENTRY_ACCESSED_INFO_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(fec_id_ndx, SOC_PB_PP_FEC_ID_MAX, SOC_PB_PP_FEC_ID_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_fec_entry_accessed_info_get_verify()", fec_id_ndx, 0);
}

/*********************************************************************
*     Set action to do by the device when a packet accesses
 *     the FEC entry.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_fec_entry_accessed_info_get_unsafe(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  SOC_PB_PP_FEC_ID                           fec_id_ndx,
    SOC_SAND_IN  uint8                              clear_access_stat,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_FEC_ENTRY_ACCESSED_INFO    *accessed_info
  )
{
  SOC_PB_PP_IHB_FEC_ENTRY_ACCESSED_TBL_DATA
    ihb_fec_entry_accessed_tbl_data;
  SOC_PB_PP_REGS
    *regs = NULL;
  uint32
    tmp,
    reg_val=0,
    reg_index,
    bit_index;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_FEC_ENTRY_ACCESSED_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(accessed_info);

  SOC_PB_PP_FRWRD_FEC_ENTRY_ACCESSED_INFO_clear(accessed_info);
  regs = soc_pb_pp_regs();

  /* was entry accessed */
  res = soc_pb_pp_ihb_fec_entry_accessed_tbl_get_unsafe(
          unit,
          SOC_PB_PP_IHB_FEC_ENTRY_ACCESSED_TBL_KEY_ENTRY_OFFSET(fec_id_ndx),
          &ihb_fec_entry_accessed_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  tmp = SOC_SAND_GET_BIT(ihb_fec_entry_accessed_tbl_data.fec_entry_accessed,(fec_id_ndx/2) % 8);
  accessed_info->accessed = SOC_SAND_NUM2BOOL(tmp);

  /* clear accessed bit if needed*/
  if (clear_access_stat)
  {
    tmp = 0;
    res = soc_sand_set_field(
            &(ihb_fec_entry_accessed_tbl_data.fec_entry_accessed),
            fec_id_ndx/2 % 8,
            fec_id_ndx/2 % 8,
            tmp
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    res = soc_pb_pp_ihb_fec_entry_accessed_tbl_set_unsafe(
            unit,
            SOC_PB_PP_IHB_FEC_ENTRY_ACCESSED_TBL_KEY_ENTRY_OFFSET(fec_id_ndx),
            &ihb_fec_entry_accessed_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }

  /* get trap if accessed*/
  /* if bigger the 63 then cannot be trapped */
  if (fec_id_ndx > SOC_PB_PP_FRWRD_FEC_TRAP_ABLE_MAX_INDEX)
  {
    accessed_info->trap_if_accessed = FALSE;
    goto exit;
  }

  reg_index = fec_id_ndx/32;
  bit_index = fec_id_ndx % 32;

  SOC_PB_PP_REG_GET(regs->ihb.trap_if_accessed_reg[reg_index],reg_val, 2, exit);
  tmp = SOC_SAND_GET_BIT(reg_val,bit_index);
  accessed_info->trap_if_accessed = SOC_SAND_NUM2BOOL(tmp);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_fec_entry_accessed_info_get_unsafe()", fec_id_ndx, 0);
}

/*********************************************************************
*     Set the ECMP hashing global attributes
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_fec_ecmp_hashing_global_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_ECMP_HASH_GLOBAL_INFO     *glbl_hash_info
  )
{
  SOC_PB_PP_REGS
    *regs = NULL;
  uint32
    reg_val=0,
    tmp;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_FEC_ECMP_HASHING_GLOBAL_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(glbl_hash_info);

  regs = soc_pb_pp_regs();

  tmp = soc_pb_pp_frwrd_fec_hash_index_to_hw_val(glbl_hash_info->hash_func_id);
  SOC_PB_PP_FLD_TO_REG(regs->ihb.ecmp_lb_key_cfg_reg.ecmp_lb_hash_index,tmp,reg_val, 10, exit);

  tmp = glbl_hash_info->key_shift;
  SOC_PB_PP_FLD_TO_REG(regs->ihb.ecmp_lb_key_cfg_reg.ecmp_lb_key_shift,tmp,reg_val, 20, exit);

  tmp = glbl_hash_info->seed;
  SOC_PB_PP_FLD_TO_REG(regs->ihb.ecmp_lb_key_cfg_reg.ecmp_lb_key_seed,tmp,reg_val, 30, exit);

  tmp = SOC_SAND_BOOL2NUM(glbl_hash_info->use_port_id);
  SOC_PB_PP_FLD_TO_REG(regs->ihb.ecmp_lb_key_cfg_reg.ecmp_lb_key_use_in_port,tmp,reg_val, 40, exit);

  SOC_PB_PP_REG_SET(regs->ihb.ecmp_lb_key_cfg_reg.ecmp_lb_key_seed,reg_val, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_fec_ecmp_hashing_global_info_set_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_frwrd_fec_ecmp_hashing_global_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_ECMP_HASH_GLOBAL_INFO     *glbl_hash_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_FEC_ECMP_HASHING_GLOBAL_INFO_SET_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_FRWRD_FEC_ECMP_HASH_GLOBAL_INFO, glbl_hash_info, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_fec_ecmp_hashing_global_info_set_verify()", 0, 0);
}

uint32
  soc_pb_pp_frwrd_fec_ecmp_hashing_global_info_get_verify(
    SOC_SAND_IN  int                                 unit
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_FEC_ECMP_HASHING_GLOBAL_INFO_GET_VERIFY);

  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_fec_ecmp_hashing_global_info_get_verify()", 0, 0);
}

/*********************************************************************
*     Set the ECMP hashing global attributes
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_fec_ecmp_hashing_global_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_FEC_ECMP_HASH_GLOBAL_INFO     *glbl_hash_info
  )
{
  SOC_PB_PP_REGS
    *regs = NULL;
  uint32
    reg_val=0,
    tmp;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_FEC_ECMP_HASHING_GLOBAL_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(glbl_hash_info);

  SOC_PB_PP_FRWRD_FEC_ECMP_HASH_GLOBAL_INFO_clear(glbl_hash_info);

  regs = soc_pb_pp_regs();

  SOC_PB_PP_REG_GET(regs->ihb.ecmp_lb_key_cfg_reg.ecmp_lb_key_seed,reg_val, 10, exit);

  tmp = 0;
  SOC_PB_PP_FLD_FROM_REG(regs->ihb.ecmp_lb_key_cfg_reg.ecmp_lb_hash_index,tmp,reg_val, 20, exit);
  glbl_hash_info->hash_func_id = (uint8)soc_pb_pp_frwrd_fec_hash_index_from_hw_val((uint8)tmp);

  tmp = 0;
  SOC_PB_PP_FLD_FROM_REG(regs->ihb.ecmp_lb_key_cfg_reg.ecmp_lb_key_shift,tmp,reg_val, 30, exit);
  glbl_hash_info->key_shift = (uint8)tmp;

  tmp = 0;
  SOC_PB_PP_FLD_FROM_REG(regs->ihb.ecmp_lb_key_cfg_reg.ecmp_lb_key_seed,tmp,reg_val, 40, exit);
  glbl_hash_info->seed = (uint16)tmp;

  tmp = 0;
  SOC_PB_PP_FLD_FROM_REG(regs->ihb.ecmp_lb_key_cfg_reg.ecmp_lb_key_use_in_port,tmp,reg_val, 50, exit);
  glbl_hash_info->use_port_id = SOC_SAND_NUM2BOOL(tmp);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_fec_ecmp_hashing_global_info_get_unsafe()", 0, 0);
}

/*********************************************************************
*     Set the ECMP hashing per-port attributes
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_fec_ecmp_hashing_port_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                port_ndx,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_ECMP_HASH_PORT_INFO       *port_hash_info
  )
{
  SOC_PB_PP_IHB_PINFO_FER_TBL_DATA
    ihb_pinfo_fer_tbl_data;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_FEC_ECMP_HASHING_PORT_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(port_hash_info);

  res = soc_pb_pp_ihb_pinfo_fer_tbl_get_unsafe(
          unit,
          port_ndx,
          &ihb_pinfo_fer_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  ihb_pinfo_fer_tbl_data.ecmp_lb_key_count = (port_hash_info->nof_headers-1);

  res = soc_pb_pp_ihb_pinfo_fer_tbl_set_unsafe(
          unit,
          port_ndx,
          &ihb_pinfo_fer_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_fec_ecmp_hashing_port_info_set_unsafe()", port_ndx, 0);
}

uint32
  soc_pb_pp_frwrd_fec_ecmp_hashing_port_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                port_ndx,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_ECMP_HASH_PORT_INFO       *port_hash_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_FEC_ECMP_HASHING_PORT_INFO_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(port_ndx, SOC_PB_PP_PORT_MAX, SOC_PB_PP_PORT_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_FRWRD_FEC_ECMP_HASH_PORT_INFO, port_hash_info, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_fec_ecmp_hashing_port_info_set_verify()", port_ndx, 0);
}

uint32
  soc_pb_pp_frwrd_fec_ecmp_hashing_port_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                port_ndx
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_FEC_ECMP_HASHING_PORT_INFO_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(port_ndx, SOC_PB_PP_PORT_MAX, SOC_PB_PP_PORT_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_fec_ecmp_hashing_port_info_get_verify()", port_ndx, 0);
}

/*********************************************************************
*     Set the ECMP hashing per-port attributes
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_fec_ecmp_hashing_port_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                port_ndx,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_FEC_ECMP_HASH_PORT_INFO       *port_hash_info
  )
{
  SOC_PB_PP_IHB_PINFO_FER_TBL_DATA
    ihb_pinfo_fer_tbl_data;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_FEC_ECMP_HASHING_PORT_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(port_hash_info);

  SOC_PB_PP_FRWRD_FEC_ECMP_HASH_PORT_INFO_clear(port_hash_info);

  res = soc_pb_pp_ihb_pinfo_fer_tbl_get_unsafe(
          unit,
          port_ndx,
          &ihb_pinfo_fer_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  port_hash_info->nof_headers = (uint8)(ihb_pinfo_fer_tbl_data.ecmp_lb_key_count+1);
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_fec_ecmp_hashing_port_info_get_unsafe()", port_ndx, 0);
}

/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     soc_pb_pp_api_frwrd_fec module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
SOC_PROCEDURE_DESC_ELEMENT*
  soc_pb_pp_frwrd_fec_get_procs_ptr(void)
{
  return Soc_pb_pp_procedure_desc_element_frwrd_fec;
}
/*********************************************************************
*     Get the pointer to the list of errors of the
 *     soc_pb_pp_api_frwrd_fec module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
SOC_ERROR_DESC_ELEMENT*
  soc_pb_pp_frwrd_fec_get_errs_ptr(void)
{
  return Soc_pb_pp_error_desc_element_frwrd_fec;
}
uint32
  SOC_PB_PP_FRWRD_FEC_GLBL_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_GLBL_INFO *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind = 0; ind < SOC_DPP_NOF_ECMP_PETRAB; ++ind)
  {
    SOC_SAND_ERR_IF_OUT_OF_RANGE(info->ecmp_sizes[ind], SOC_PB_PP_FRWRD_FEC_ECMP_SIZES_MIN, SOC_PB_PP_FRWRD_FEC_ECMP_SIZES_MAX, SOC_PB_PP_FRWRD_FEC_ECMP_SIZES_OUT_OF_RANGE_ERR, 10, exit);
  }
  SOC_SAND_ERR_IF_OUT_OF_RANGE(info->ecmp_sizes_nof_entries, SOC_DPP_ECMP_MAX_SIZE_PETRAB, SOC_DPP_ECMP_MAX_SIZE_PETRAB, SOC_PB_PP_FRWRD_FEC_ECMP_SIZES_NOF_ENTRIES_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_FRWRD_FEC_GLBL_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_FRWRD_FEC_ENTRY_RPF_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_ENTRY_RPF_INFO *info
  )
{
  uint32
    tmp,
    nof_set_bits;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->rpf_mode, SOC_PB_PP_FRWRD_FEC_RPF_MODE_MAX, SOC_PB_PP_FRWRD_FEC_RPF_MODE_OUT_OF_RANGE_ERR, 10, exit);
  tmp = info->rpf_mode;
  nof_set_bits = soc_sand_bitstream_get_nof_on_bits(&tmp,1);
  if(nof_set_bits > 1) 
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FRWRD_FEC_RPF_MODE_OUT_OF_RANGE_ERR, 11,exit);
  }
  SOC_SAND_ERR_IF_ABOVE_MAX(info->expected_in_rif, SOC_PB_PP_RIF_ID_MAX, SOC_PB_PP_RIF_ID_OUT_OF_RANGE_ERR, 12, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_FRWRD_FEC_ENTRY_RPF_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_FRWRD_FEC_ENTRY_APP_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_ENTRY_APP_INFO *info,
    SOC_SAND_IN  SOC_PB_PP_FEC_TYPE                 entry_type
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  if (entry_type == SOC_PB_PP_FEC_TYPE_BRIDGING_WITH_AC)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(info->out_ac_id, SOC_PB_PP_AC_ID_MAX, SOC_PB_PP_AC_ID_OUT_OF_RANGE_ERR, 10, exit);
  }
  if (entry_type == SOC_PB_PP_FEC_TYPE_IP_UC || entry_type == SOC_PB_PP_FEC_TYPE_MPLS_LSR)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(info->out_rif, SOC_PB_PP_RIF_ID_MAX, SOC_PB_PP_RIF_ID_OUT_OF_RANGE_ERR, 11, exit);
  }
  if (entry_type == SOC_PB_PP_FEC_TYPE_TRILL_MC)
  {
   SOC_SAND_ERR_IF_ABOVE_MAX(info->dist_tree_nick, SOC_PB_PP_FRWRD_FEC_DIST_TREE_NICK_MAX, SOC_PB_PP_FRWRD_FEC_DIST_TREE_NICK_OUT_OF_RANGE_ERR, 12, exit);
  }

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_FRWRD_FEC_ENTRY_APP_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_FRWRD_FEC_ENTRY_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_ENTRY_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->type, SOC_PB_PP_FRWRD_FEC_TYPE_MAX, SOC_PB_PP_FRWRD_FEC_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  if (info->eep != SOC_PB_PP_EEP_NULL && info->type != SOC_PB_PP_FEC_TYPE_BRIDGING_WITH_AC && info->type != SOC_PB_PP_FEC_TYPE_IP_MC && info->type != SOC_PB_PP_FEC_TYPE_DROP)
  {
    SOC_SAND_ERR_IF_OUT_OF_RANGE(info->eep, SOC_PB_PP_FRWRD_FEC_EEP_MIN, SOC_PB_PP_FRWRD_FEC_EEP_MAX, SOC_PB_PP_FRWRD_FEC_EEP_OUT_OF_RANGE_ERR, 12, exit);
  }
  res = SOC_PB_PP_FRWRD_FEC_ENTRY_APP_INFO_verify(&(info->app_info),info->type);
  SOC_SAND_CHECK_FUNC_RESULT(res, 13, exit);
  if (info->type == SOC_PB_PP_FEC_TYPE_IP_MC || info->type == SOC_PB_PP_FEC_TYPE_IP_UC)
  {
    SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_FRWRD_FEC_ENTRY_RPF_INFO, &(info->rpf_info), 14, exit);
  }
  /* check destination */
  switch(info->dest.dest_type)
  {
  case SOC_SAND_PP_DEST_TYPE_DROP:
    SOC_SAND_ERR_IF_ABOVE_MAX(info->dest.dest_val,0,SOC_PB_PP_FRWRD_FEC_DEST_VAL_OUT_OF_RANGE_ERR,15,exit);
  break;
    case SOC_SAND_PP_DEST_SINGLE_PORT:
      SOC_SAND_ERR_IF_ABOVE_MAX(info->dest.dest_val,SOC_TMC_MAX_SYSTEM_PHYSICAL_PORT_ID,SOC_PB_PP_FRWRD_FEC_DEST_VAL_OUT_OF_RANGE_ERR,16,exit);
    break;
    case SOC_SAND_PP_DEST_MULTICAST:
      SOC_SAND_ERR_IF_ABOVE_MAX(info->dest.dest_val,SOC_PETRA_MULT_ID_MAX,SOC_PB_PP_FRWRD_FEC_DEST_VAL_OUT_OF_RANGE_ERR,17,exit);
    break;
    case SOC_SAND_PP_DEST_LAG:
      SOC_SAND_ERR_IF_ABOVE_MAX(info->dest.dest_val,SOC_TMC_MAX_LAG_GROUP_ID,SOC_PB_PP_FRWRD_FEC_DEST_VAL_OUT_OF_RANGE_ERR,18,exit);
    break;
    case SOC_SAND_PP_DEST_EXPLICIT_FLOW:
      SOC_SAND_ERR_IF_ABOVE_MAX(info->dest.dest_val,SOC_DPP_NOF_FLOWS_PETRAB-1,SOC_PB_PP_FRWRD_FEC_DEST_VAL_OUT_OF_RANGE_ERR,20,exit);
    break;
    case SOC_SAND_PP_DEST_TRAP:
      SOC_PB_PP_ACTION_PROFILE_verify(&(info->trap_info));
    break;
    case SOC_SAND_PP_DEST_TYPE_ROUTER:
    case SOC_SAND_PP_DEST_FEC:
    default:
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FRWRD_FEC_DEST_TYPE_OUT_OF_RANGE_ERR, 30,exit);
    break;
  }
  /* if FEC type is MC then Dest-type has to be Multicast*/
  if (info->type == SOC_PB_PP_FEC_TYPE_IP_MC && info->dest.dest_type != SOC_SAND_PP_DEST_MULTICAST)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FRWRD_FEC_IPMC_DEST_NOT_MC_ERR, 35,exit);
  }
  /* RPF macth entry type */
  /* IP-MC, has to be with MC RPF */
  if (
       info->type == SOC_PB_PP_FEC_TYPE_IP_MC &&
       ( info->rpf_info.rpf_mode == SOC_PB_PP_FRWRD_FEC_RPF_MODE_UC_STRICT ||
         info->rpf_info.rpf_mode == SOC_PB_PP_FRWRD_FEC_RPF_MODE_UC_LOOSE
       )
     )
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FRWRD_FEC_IPMC_RPF_MODE_ILLEGAL_ERR, 40,exit);
  }
  /* IP-UC, has to be with UC RPF */
  if (
       info->type == SOC_PB_PP_FEC_TYPE_IP_UC &&
       info->rpf_info.rpf_mode != SOC_PB_PP_FRWRD_FEC_RPF_MODE_UC_STRICT &&
       info->rpf_info.rpf_mode != SOC_PB_PP_FRWRD_FEC_RPF_MODE_UC_LOOSE
     )
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FRWRD_FEC_IPUC_RPF_MODE_ILLEGAL_ERR, 50,exit);
  }



  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_FRWRD_FEC_ENTRY_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_FRWRD_FEC_ENTRY_ACCESSED_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_ENTRY_ACCESSED_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);


  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_FRWRD_FEC_ENTRY_ACCESSED_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_FRWRD_FEC_PROTECT_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_PROTECT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(info->oam_instance_id, SOC_PB_PP_FRWRD_FEC_OAM_INSTANCE_ID_MIN, SOC_PB_PP_FRWRD_FEC_OAM_INSTANCE_ID_MAX, SOC_PB_PP_FRWRD_FEC_OAM_INSTANCE_ID_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_FRWRD_FEC_PROTECT_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_FRWRD_FEC_ENTRY_USE_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_ENTRY_USE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->type, SOC_PB_PP_FRWRD_FEC_TYPE_MAX, SOC_PB_PP_FRWRD_FEC_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->nof_entries, SOC_PB_PP_FRWRD_FEC_NOF_ENTRIES_MAX, SOC_PB_PP_FRWRD_FEC_NOF_ENTRIES_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_FRWRD_FEC_ENTRY_USE_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_FRWRD_FEC_MATCH_RULE_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_MATCH_RULE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->type, SOC_PB_PP_FRWRD_FEC_TYPE_MAX, SOC_PB_PP_FRWRD_FEC_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->value, SOC_PB_PP_FRWRD_FEC_VALUE_MAX, SOC_PB_PP_FRWRD_FEC_VALUE_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_FRWRD_FEC_MATCH_RULE_verify()",0,0);
}

uint32
  SOC_PB_PP_FRWRD_FEC_ECMP_HASH_GLOBAL_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_ECMP_HASH_GLOBAL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  /* SOC_SAND_ERR_IF_ABOVE_MAX(info->seed, SOC_PB_PP_FRWRD_FEC_SEED_MAX, SOC_PB_PP_FRWRD_FEC_SEED_OUT_OF_RANGE_ERR, 11, exit); */
  SOC_SAND_ERR_IF_ABOVE_MAX(info->hash_func_id, SOC_PB_PP_FRWRD_FEC_HASH_FUNC_ID_MAX, SOC_PB_PP_FRWRD_FEC_HASH_FUNC_ID_OUT_OF_RANGE_ERR, 12, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->key_shift, SOC_PB_PP_FRWRD_FEC_KEY_SHIFT_MAX, SOC_PB_PP_FRWRD_FEC_KEY_SHIFT_OUT_OF_RANGE_ERR, 13, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_FRWRD_FEC_ECMP_HASH_GLOBAL_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_FRWRD_FEC_ECMP_HASH_PORT_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_ECMP_HASH_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(info->nof_headers, SOC_PB_PP_FRWRD_FEC_NOF_HEADERS_MIN, SOC_PB_PP_FRWRD_FEC_NOF_HEADERS_MAX, SOC_PB_PP_FRWRD_FEC_NOF_HEADERS_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_FRWRD_FEC_ECMP_HASH_PORT_INFO_verify()",0,0);
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

