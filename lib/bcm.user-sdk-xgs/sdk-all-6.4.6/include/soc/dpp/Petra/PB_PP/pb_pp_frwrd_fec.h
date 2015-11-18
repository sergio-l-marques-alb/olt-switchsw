/* $Id: pb_pp_frwrd_fec.h,v 1.7 Broadcom SDK $
 * $Copyright: Copyright 2015 Broadcom Corporation.
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
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_PP/include/soc_pb_pp_frwrd_fec.h
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

#ifndef __SOC_PB_PP_FRWRD_FEC_INCLUDED__
/* { */
#define __SOC_PB_PP_FRWRD_FEC_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/Petra/PB_PP/pb_pp_api_frwrd_fec.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_framework.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */
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

typedef enum
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PB_PP_FRWRD_FEC_GLBL_INFO_SET = SOC_PB_PP_PROC_DESC_BASE_FRWRD_FEC_FIRST,
  SOC_PB_PP_FRWRD_FEC_GLBL_INFO_SET_PRINT,
  SOC_PB_PP_FRWRD_FEC_GLBL_INFO_SET_UNSAFE,
  SOC_PB_PP_FRWRD_FEC_GLBL_INFO_SET_VERIFY,
  SOC_PB_PP_FRWRD_FEC_GLBL_INFO_GET,
  SOC_PB_PP_FRWRD_FEC_GLBL_INFO_GET_PRINT,
  SOC_PB_PP_FRWRD_FEC_GLBL_INFO_GET_VERIFY,
  SOC_PB_PP_FRWRD_FEC_GLBL_INFO_GET_UNSAFE,
  SOC_PB_PP_FRWRD_FEC_ENTRY_ADD,
  SOC_PB_PP_FRWRD_FEC_ENTRY_ADD_PRINT,
  SOC_PB_PP_FRWRD_FEC_ENTRY_ADD_UNSAFE,
  SOC_PB_PP_FRWRD_FEC_ENTRY_ADD_VERIFY,
  SOC_PB_PP_FRWRD_FEC_ECMP_ADD,
  SOC_PB_PP_FRWRD_FEC_ECMP_ADD_PRINT,
  SOC_PB_PP_FRWRD_FEC_ECMP_ADD_UNSAFE,
  SOC_PB_PP_FRWRD_FEC_ECMP_ADD_VERIFY,
  SOC_PB_PP_FRWRD_FEC_ECMP_UPDATE,
  SOC_PB_PP_FRWRD_FEC_ECMP_UPDATE_PRINT,
  SOC_PB_PP_FRWRD_FEC_ECMP_UPDATE_UNSAFE,
  SOC_PB_PP_FRWRD_FEC_ECMP_UPDATE_VERIFY,
  SOC_PB_PP_FRWRD_FEC_ENTRY_USE_INFO_GET,
  SOC_PB_PP_FRWRD_FEC_ENTRY_USE_INFO_GET_PRINT,
  SOC_PB_PP_FRWRD_FEC_ENTRY_USE_INFO_GET_UNSAFE,
  SOC_PB_PP_FRWRD_FEC_ENTRY_USE_INFO_GET_VERIFY,
  SOC_PB_PP_FRWRD_FEC_ENTRY_GET,
  SOC_PB_PP_FRWRD_FEC_ENTRY_GET_PRINT,
  SOC_PB_PP_FRWRD_FEC_ENTRY_GET_UNSAFE,
  SOC_PB_PP_FRWRD_FEC_ENTRY_GET_VERIFY,
  SOC_PB_PP_FRWRD_FEC_ECMP_GET,
  SOC_PB_PP_FRWRD_FEC_ECMP_GET_PRINT,
  SOC_PB_PP_FRWRD_FEC_ECMP_GET_UNSAFE,
  SOC_PB_PP_FRWRD_FEC_ECMP_GET_VERIFY,
  SOC_PB_PP_FRWRD_FEC_REMOVE,
  SOC_PB_PP_FRWRD_FEC_REMOVE_PRINT,
  SOC_PB_PP_FRWRD_FEC_REMOVE_UNSAFE,
  SOC_PB_PP_FRWRD_FEC_REMOVE_VERIFY,
  SOC_PB_PP_FRWRD_FEC_PROTECTION_OAM_INSTANCE_STATUS_SET,
  SOC_PB_PP_FRWRD_FEC_PROTECTION_OAM_INSTANCE_STATUS_SET_PRINT,
  SOC_PB_PP_FRWRD_FEC_PROTECTION_OAM_INSTANCE_STATUS_SET_UNSAFE,
  SOC_PB_PP_FRWRD_FEC_PROTECTION_OAM_INSTANCE_STATUS_SET_VERIFY,
  SOC_PB_PP_FRWRD_FEC_PROTECTION_OAM_INSTANCE_STATUS_GET,
  SOC_PB_PP_FRWRD_FEC_PROTECTION_OAM_INSTANCE_STATUS_GET_PRINT,
  SOC_PB_PP_FRWRD_FEC_PROTECTION_OAM_INSTANCE_STATUS_GET_VERIFY,
  SOC_PB_PP_FRWRD_FEC_PROTECTION_OAM_INSTANCE_STATUS_GET_UNSAFE,
  SOC_PB_PP_FRWRD_FEC_PROTECTION_SYS_PORT_STATUS_SET,
  SOC_PB_PP_FRWRD_FEC_PROTECTION_SYS_PORT_STATUS_SET_PRINT,
  SOC_PB_PP_FRWRD_FEC_PROTECTION_SYS_PORT_STATUS_SET_UNSAFE,
  SOC_PB_PP_FRWRD_FEC_PROTECTION_SYS_PORT_STATUS_SET_VERIFY,
  SOC_PB_PP_FRWRD_FEC_PROTECTION_SYS_PORT_STATUS_GET,
  SOC_PB_PP_FRWRD_FEC_PROTECTION_SYS_PORT_STATUS_GET_PRINT,
  SOC_PB_PP_FRWRD_FEC_PROTECTION_SYS_PORT_STATUS_GET_VERIFY,
  SOC_PB_PP_FRWRD_FEC_PROTECTION_SYS_PORT_STATUS_GET_UNSAFE,
  SOC_PB_PP_FRWRD_FEC_GET_BLOCK,
  SOC_PB_PP_FRWRD_FEC_GET_BLOCK_PRINT,
  SOC_PB_PP_FRWRD_FEC_GET_BLOCK_UNSAFE,
  SOC_PB_PP_FRWRD_FEC_GET_BLOCK_VERIFY,
  SOC_PB_PP_FRWRD_FEC_ENTRY_ACCESSED_INFO_SET,
  SOC_PB_PP_FRWRD_FEC_ENTRY_ACCESSED_INFO_SET_PRINT,
  SOC_PB_PP_FRWRD_FEC_ENTRY_ACCESSED_INFO_SET_UNSAFE,
  SOC_PB_PP_FRWRD_FEC_ENTRY_ACCESSED_INFO_SET_VERIFY,
  SOC_PB_PP_FRWRD_FEC_ENTRY_ACCESSED_INFO_GET,
  SOC_PB_PP_FRWRD_FEC_ENTRY_ACCESSED_INFO_GET_PRINT,
  SOC_PB_PP_FRWRD_FEC_ENTRY_ACCESSED_INFO_GET_VERIFY,
  SOC_PB_PP_FRWRD_FEC_ENTRY_ACCESSED_INFO_GET_UNSAFE,
  SOC_PB_PP_FRWRD_FEC_ECMP_HASHING_GLOBAL_INFO_SET,
  SOC_PB_PP_FRWRD_FEC_ECMP_HASHING_GLOBAL_INFO_SET_PRINT,
  SOC_PB_PP_FRWRD_FEC_ECMP_HASHING_GLOBAL_INFO_SET_UNSAFE,
  SOC_PB_PP_FRWRD_FEC_ECMP_HASHING_GLOBAL_INFO_SET_VERIFY,
  SOC_PB_PP_FRWRD_FEC_ECMP_HASHING_GLOBAL_INFO_GET,
  SOC_PB_PP_FRWRD_FEC_ECMP_HASHING_GLOBAL_INFO_GET_PRINT,
  SOC_PB_PP_FRWRD_FEC_ECMP_HASHING_GLOBAL_INFO_GET_VERIFY,
  SOC_PB_PP_FRWRD_FEC_ECMP_HASHING_GLOBAL_INFO_GET_UNSAFE,
  SOC_PB_PP_FRWRD_FEC_ECMP_HASHING_PORT_INFO_SET,
  SOC_PB_PP_FRWRD_FEC_ECMP_HASHING_PORT_INFO_SET_PRINT,
  SOC_PB_PP_FRWRD_FEC_ECMP_HASHING_PORT_INFO_SET_UNSAFE,
  SOC_PB_PP_FRWRD_FEC_ECMP_HASHING_PORT_INFO_SET_VERIFY,
  SOC_PB_PP_FRWRD_FEC_ECMP_HASHING_PORT_INFO_GET,
  SOC_PB_PP_FRWRD_FEC_ECMP_HASHING_PORT_INFO_GET_PRINT,
  SOC_PB_PP_FRWRD_FEC_ECMP_HASHING_PORT_INFO_GET_VERIFY,
  SOC_PB_PP_FRWRD_FEC_ECMP_HASHING_PORT_INFO_GET_UNSAFE,
  SOC_PB_PP_FRWRD_FEC_GET_PROCS_PTR,
  SOC_PB_PP_FRWRD_FEC_GET_ERRS_PTR,
  /*
   * } Auto generated. Do not edit previous section.
   */
   SOC_PB_PP_FRWRD_FEC_SIZE_TO_INDEX_GET,
   SOC_PB_PP_FRWRD_FEC_INDEX_TO_SIZE_GET,
   SOC_PB_PP_FRWRD_FEC_ONE_ENTRY_SET,
   SOC_PB_PP_FRWRD_FEC_ONE_ENTRY_GET,
   SOC_PB_PP_FRWRD_FEC_SIZE_GET,
   SOC_PB_PP_FRWRD_FEC_PROTECT_TYPE_GET,
   SOC_PB_PP_FRWRD_FEC_DEST_ENCODE,
   SOC_PB_PP_FRWRD_FEC_DEST_DECODE,

  /*
   * Last element. Do no touch.
   */
  SOC_PB_PP_FRWRD_FEC_PROCEDURE_DESC_LAST
} SOC_PB_PP_FRWRD_FEC_PROCEDURE_DESC;

typedef enum
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PB_PP_FRWRD_FEC_PROTECT_TYPE_OUT_OF_RANGE_ERR = SOC_PB_PP_ERR_DESC_BASE_FRWRD_FEC_FIRST,
  SOC_PB_PP_FRWRD_FEC_SUCCESS_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FRWRD_FEC_NOF_ENTRIES_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FRWRD_FEC_OAM_INSTANCE_NDX_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FRWRD_FEC_UP_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FRWRD_FEC_ECMP_SIZES_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FRWRD_FEC_ECMP_SIZES_NOF_ENTRIES_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FRWRD_FEC_RPF_MODE_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FRWRD_FEC_DIST_TREE_NICK_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FRWRD_FEC_TYPE_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FRWRD_FEC_EEP_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FRWRD_FEC_OAM_INSTANCE_ID_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FRWRD_FEC_VALUE_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FRWRD_FEC_SEED_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FRWRD_FEC_HASH_FUNC_ID_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FRWRD_FEC_KEY_SHIFT_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FRWRD_FEC_NOF_HEADERS_OUT_OF_RANGE_ERR,
  /*
   * } Auto generated. Do not edit previous section.
   */
  SOC_PB_PP_FRWRD_FEC_NO_MATCH_ECMP_SIZE_ERR,
  SOC_PB_PP_FRWRD_FEC_ECMP_SIZE_INDEX_OUT_OF_RANGE_ERR,

  SOC_PB_PP_FRWRD_FEC_PROTECTED_NOT_EVEN_INDEX_ERR,
  SOC_PB_PP_FRWRD_FEC_ECMP_NOT_EVEN_INDEX_ERR,
  SOC_PB_PP_FRWRD_FEC_UPDATE_RANGE_OUT_OF_ECMP_ERR,
  SOC_PB_PP_FRWRD_FEC_UPDATE_RANGE_ILLEGAL_ERR,
  SOC_PB_PP_FRWRD_FEC_DEST_TYPE_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FRWRD_FEC_DEST_VAL_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FRWRD_FEC_IPMC_DEST_NOT_MC_ERR,
  SOC_PB_PP_FRWRD_FEC_IPMC_RPF_MODE_ILLEGAL_ERR,
  SOC_PB_PP_FRWRD_FEC_IPUC_RPF_MODE_ILLEGAL_ERR,
  SOC_PB_PP_FRWRD_FEC_EXPECT_NON_ECMP_ERR,
  SOC_PB_PP_FRWRD_FEC_EXPECT_NON_PROTECT_ERR,
  SOC_PB_PP_FRWRD_FEC_DEST_PHY_PORT_STATUS_ILLEGAL_ERR,
  SOC_PB_PP_FRWRD_FEC_TRAP_ACCESS_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FRWRD_FEC_ACCESSED_OUT_OF_RANGE_ERR,
  

  /*
   * Last element. Do no touch.
   */
  SOC_PB_PP_FRWRD_FEC_ERR_LAST
} SOC_PB_PP_FRWRD_FEC_ERR;

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

uint32
  soc_pb_pp_frwrd_fec_init_unsafe(
    SOC_SAND_IN  int                                 unit
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_fec_glbl_info_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Setting global information of the FEC table (including
 *   resources to use)
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_GLBL_INFO                 *glbl_info -
 *     Global information.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_fec_glbl_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_GLBL_INFO                 *glbl_info
  );

uint32
  soc_pb_pp_frwrd_fec_glbl_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_GLBL_INFO                 *glbl_info
  );

uint32
  soc_pb_pp_frwrd_fec_glbl_info_get_verify(
    SOC_SAND_IN  int                                 unit
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_frwrd_fec_glbl_info_set_unsafe" API.
 *     Refer to "soc_pb_pp_frwrd_fec_glbl_info_set_unsafe" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_fec_glbl_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_FEC_GLBL_INFO                 *glbl_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_fec_entry_add_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Add FEC entry. May include protection of type Facility
 *   or Path.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_FEC_ID                              fec_ndx -
 *     Index in the FEC table. Range: 0 - 16383. Has to be even
 *     for protected FECs in Soc_petra-B.
 *   SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_PROTECT_TYPE              protect_type -
 *     Protection type may be None, Path, or Facility.
 *   SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_ENTRY_INFO                *working_fec -
 *     Working FEC entry.
 *   SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_ENTRY_INFO                *protect_fec -
 *     Protecting FEC entry. Relevant when there is protection,
 *     i.e., protection type is not
 *     SOC_PPD_FRWRD_FEC_PROTECT_TYPE_NONE.
 *   SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_PROTECT_INFO              *protect_info -
 *     Protection information. Relevant only for Path protected
 *     FEC, i.e., if the protection type is
 *     SOC_PPD_FRWRD_FEC_PROTECT_TYPE_PATH. Includes the
 *     OAM-instance that the above FECs are associated with and
 *     determines which FEC to use in order to forward the
 *     packets.
 *   SOC_SAND_OUT uint8                                 *success -
 *     Whether the operation succeeds. Add operation may fail
 *     if there is no place in the FEC DB (LEM).
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
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
  );

uint32
  soc_pb_pp_frwrd_fec_entry_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FEC_ID                              fec_ndx,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_PROTECT_TYPE              protect_type,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_ENTRY_INFO                *working_fec,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_ENTRY_INFO                *protect_fec,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_PROTECT_INFO              *protect_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_fec_ecmp_add_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Add ECMP to the FEC table.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_FEC_ID                              fec_ndx -
 *     Index in the FEC table. Range: 0 - 16383. Must be even.
 *   SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_ENTRY_INFO                *fec_array -
 *     The FEC array (ECMP).
 *   SOC_SAND_IN  uint32                                  nof_entries -
 *     Size of ECMP and number of valid entries in fec_array.
 *   SOC_SAND_OUT uint8                                 *success -
 *     Whether the operation succeeds. Add operation may fail
 *     if there is no place in the FEC DB (LEM).
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_fec_ecmp_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FEC_ID                              fec_ndx,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_ENTRY_INFO                *fec_array,
    SOC_SAND_IN  uint32                                  nof_entries,
    SOC_SAND_OUT uint8                                 *success
  );

uint32
  soc_pb_pp_frwrd_fec_ecmp_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FEC_ID                              fec_ndx,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_ENTRY_INFO                *fec_array,
    SOC_SAND_IN  uint32                                  nof_entries
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_fec_ecmp_update_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Update the content of block of entries from the ECMP.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_FEC_ID                              fec_ndx -
 *     Index in the FEC table. Range: 0 - 16383. Must be even.
 *   SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_ENTRY_INFO                *fec_array -
 *     FEC array to update the selected range of the ECMP. The
 *     size of this array should be as the length of the array
 *     (fec_range.end - fec_rang.start + 1)
 *   SOC_SAND_IN  SOC_SAND_U32_RANGE                            *fec_range -
 *     Range includes start FEC and end FEC to update.
 *     fec_range.end has to be smaller than the updated ECMP
 *     size.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_fec_ecmp_update_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FEC_ID                              fec_ndx,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_ENTRY_INFO                *fec_array,
    SOC_SAND_IN  SOC_SAND_U32_RANGE                            *fec_range
  );

uint32
  soc_pb_pp_frwrd_fec_ecmp_update_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FEC_ID                              fec_ndx,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_ENTRY_INFO                *fec_array,
    SOC_SAND_IN  SOC_SAND_U32_RANGE                            *fec_range
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_fec_entry_use_info_get_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the usage/allocation of the FEC entry pointed by
 *   fec_ndx (ECMP/FEC/protected FEC/none).
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_FEC_ID                              fec_ndx -
 *     Index in the FEC table. Range: 0 - 16383.
 *   SOC_SAND_OUT SOC_PB_PP_FRWRD_FEC_ENTRY_USE_INFO            *fec_entry_info -
 *     The usage of the FEC entry. - For ECMP/protection
 *     returns pointer to first FEC in the ECMP/protection.-
 *     For ECMP returns the size of the ECMP
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_fec_entry_use_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FEC_ID                              fec_ndx,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_FEC_ENTRY_USE_INFO            *fec_entry_info
  );

uint32
  soc_pb_pp_frwrd_fec_entry_use_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FEC_ID                              fec_ndx
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_fec_entry_get_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get FEC entry from the FEC table. May include
 *   protection.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_FEC_ID                              fec_ndx -
 *     Index in the FEC table. Range: 0 - 16383. Has to be even
 *     for Protected FECs in Soc_petra-B.
 *   SOC_SAND_OUT SOC_PB_PP_FRWRD_FEC_PROTECT_TYPE              *protect_type -
 *     Protection type may be None, Path, or Facility.
 *   SOC_SAND_OUT SOC_PB_PP_FRWRD_FEC_ENTRY_INFO                *working_fec -
 *     Working FEC entry.
 *   SOC_SAND_OUT SOC_PB_PP_FRWRD_FEC_ENTRY_INFO                *protect_fec -
 *     Protecting FEC entry. Relevant when there is protection,
 *     i.e., protection type is not
 *     SOC_PPD_FRWRD_FEC_PROTECT_TYPE_NONE.
 *   SOC_SAND_OUT SOC_PB_PP_FRWRD_FEC_PROTECT_INFO              *protect_info -
 *     Protection information. Relevant only for Path protected
 *     FEC, i.e., if the protection type is
 *     SOC_PPD_FRWRD_FEC_PROTECT_TYPE_PATH. Includes the
 *     OAM-instance that the above FECs are associated with and
 *     determines which FEC to use in order to forward the
 *     packets.
 * REMARKS:
 *   - fec_ndx should be allocated and not point to ECMP,
 *   Error will be returned if so.- In case of protection
 *   fec_ndx has to point to the first FEC in the
 *   protection.- In order to get the allocation type of the
 *   FEC, use soc_ppd_frwrd_fec_entry_alloc_info_get(fec_ndx) -
 *   to get ECMP use soc_ppd_frwrd_fec_ecmp_get().
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_fec_entry_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FEC_ID                              fec_ndx,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_FEC_PROTECT_TYPE              *protect_type,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_FEC_ENTRY_INFO                *working_fec,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_FEC_ENTRY_INFO                *protect_fec,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_FEC_PROTECT_INFO              *protect_info
  );

uint32
  soc_pb_pp_frwrd_fec_entry_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FEC_ID                              fec_ndx
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_fec_ecmp_get_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Update content of range of the ECMP.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_FEC_ID                              fec_ndx -
 *     Index in the FEC table. Range: 0 - 16383. Must be even
 *     and start of ECMP.
 *   SOC_SAND_IN  SOC_SAND_U32_RANGE                            *fec_range -
 *     Range includes start FEC and end FEC to return.
 *     fec_range.end has to be smaller than the ECMP size.
 *   SOC_SAND_OUT SOC_PB_PP_FRWRD_FEC_ENTRY_INFO                *fec_array -
 *     Block of FEC entries of the ECMP.
 *   SOC_SAND_OUT uint32                                  *nof_entries -
 *     Number of entries in the returned fec_array
 * REMARKS:
 *   - fec_ndx has to point to fec of type
 *   SOC_PPD_FRWRD_FEC_ENTRY_USE_TYPE_ECMP. Error will be
 *   returned otherwise.- fec_ndx has to point to the first
 *   FEC in the ECMP.- In order to get the allocation type of
 *   the FEC, use soc_ppd_frwrd_fec_entry_alloc_info_get(fec_ndx)
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_fec_ecmp_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FEC_ID                              fec_ndx,
    SOC_SAND_IN  SOC_SAND_U32_RANGE                            *fec_range,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_FEC_ENTRY_INFO                *fec_array,
    SOC_SAND_OUT uint32                                  *nof_entries
  );

uint32
  soc_pb_pp_frwrd_fec_ecmp_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FEC_ID                              fec_ndx,
    SOC_SAND_IN  SOC_SAND_U32_RANGE                            *fec_range
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_fec_remove_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Remove FEC entry/entries associated with fec_ndx.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_FEC_ID                              fec_ndx -
 *     Index in the FEC table. Range: 0 - 16383.
 * REMARKS:
 *   - When removing ECMP, then the fec_ndx must point to the
 *   first FEC of this ECMP (error will be returned
 *   otherwise), and then all FEC entries of the ECMP will be
 *   removed. - When removing protected FEC (path/facility),
 *   then the fec_ndx must point to the first FEC (even
 *   index) (error will be returned otherwise), and then both
 *   FEC entries will be removed.- User cannot remove FEC
 *   that is part of the ECMP/protection without moving all
 *   the FEC entries in the ECMP/protection.- It is the user
 *   responsibility to remove all usages/pointers to the FEC
 *   entry before removing it.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_fec_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FEC_ID                              fec_ndx
  );

uint32
  soc_pb_pp_frwrd_fec_remove_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FEC_ID                              fec_ndx
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_fec_protection_oam_instance_status_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the status of the OAM instance. For all PATH
 *   protected FECs that point to this instance, the working
 *   FEC will be used if up is TRUE, and the protect FEC will
 *   be used otherwise.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                  oam_instance_ndx -
 *     OAM instance ID. Range: Soc_petra-B: 0-4K-1, T20E:0-16K-1.
 *   SOC_SAND_IN  uint8                                 up -
 *     OAM instance status.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_fec_protection_oam_instance_status_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  oam_instance_ndx,
    SOC_SAND_IN  uint8                                 up
  );

uint32
  soc_pb_pp_frwrd_fec_protection_oam_instance_status_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  oam_instance_ndx,
    SOC_SAND_IN  uint8                                 up
  );

uint32
  soc_pb_pp_frwrd_fec_protection_oam_instance_status_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  oam_instance_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_frwrd_fec_protection_oam_instance_status_set_unsafe"
 *     API.
 *     Refer to
 *     "soc_pb_pp_frwrd_fec_protection_oam_instance_status_set_unsafe"
 *     API for details.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_fec_protection_oam_instance_status_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  oam_instance_ndx,
    SOC_SAND_OUT uint8                                 *up
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_fec_protection_sys_port_status_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the status of the System Port (LAG or Physical
 *   port).
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_SAND_PP_SYS_PORT_ID                       *sys_port_ndx -
 *     System Port (LAG or Physical port).
 *   SOC_SAND_IN  uint8                                 up -
 *     System port status.
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_fec_protection_sys_port_status_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_SYS_PORT_ID                       *sys_port_ndx,
    SOC_SAND_IN  uint8                                 up
  );

uint32
  soc_pb_pp_frwrd_fec_protection_sys_port_status_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_SYS_PORT_ID                       *sys_port_ndx,
    SOC_SAND_IN  uint8                                 up
  );

uint32
  soc_pb_pp_frwrd_fec_protection_sys_port_status_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_SYS_PORT_ID                       *sys_port_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_frwrd_fec_protection_sys_port_status_set_unsafe"
 *     API.
 *     Refer to
 *     "soc_pb_pp_frwrd_fec_protection_sys_port_status_set_unsafe"
 *     API for details.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_fec_protection_sys_port_status_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_SYS_PORT_ID                       *sys_port_ndx,
    SOC_SAND_OUT uint8                                 *up
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_fec_get_block_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Traverse the FEC table (in the specified range) and get
 *   all the FEC entries that match the given rule.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_MATCH_RULE                *rule -
 *     Get only entries that match this rule.
 *   SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                    *block_range -
 *     Range for iteration
 *   SOC_SAND_OUT uint32                                  *fec_array -
 *     Array to include FEC IDs.
 *   SOC_SAND_OUT uint32                                  *nof_entries -
 *     Number of valid entries in fec_array.
 * REMARKS:
 *   - if rule is according to application type, then for
 *   ECMP and protection only the id of the first FEC of the
 *   ECMP/protection is returned.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_fec_get_block_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_MATCH_RULE                *rule,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                    *block_range,
    SOC_SAND_OUT uint32                                  *fec_array,
    SOC_SAND_OUT uint32                                  *nof_entries
  );

uint32
  soc_pb_pp_frwrd_fec_get_block_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_MATCH_RULE                *rule,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                    *block_range
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_fec_entry_accessed_info_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set action to do by the device when a packet accesses
 *   the FEC entry.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_FEC_ID                              fec_id_ndx -
 *     Index in the FEC table. Range: 0 - 16383.
 *   SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_ENTRY_ACCESSED_INFO       *accessed_info -
 *     Actions to do when packet accesses the FEC.
 * REMARKS:
 *   - May be set only for FEC entries in the Range 0-63- Not
 *   supported in T20E.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_fec_entry_accessed_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FEC_ID                              fec_id_ndx,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_ENTRY_ACCESSED_INFO       *accessed_info
  );

uint32
  soc_pb_pp_frwrd_fec_entry_accessed_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FEC_ID                              fec_id_ndx,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_ENTRY_ACCESSED_INFO       *accessed_info
  );

uint32
  soc_pb_pp_frwrd_fec_entry_accessed_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FEC_ID                              fec_id_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_frwrd_fec_entry_accessed_info_set_unsafe" API.
 *     Refer to
 *     "soc_pb_pp_frwrd_fec_entry_accessed_info_set_unsafe" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_fec_entry_accessed_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FEC_ID                              fec_id_ndx,
    SOC_SAND_IN  uint8                                 clear_access_stat,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_FEC_ENTRY_ACCESSED_INFO       *accessed_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_fec_ecmp_hashing_global_info_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the ECMP hashing global attributes
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_ECMP_HASH_GLOBAL_INFO     *glbl_hash_info -
 *     ECMP Hashing global settings
 * REMARKS:
 *   The hashing function result is 16 bit value. The PP use
 *   the value to choose the ECMP FEC member, to which the
 *   packet is sent
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_fec_ecmp_hashing_global_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_ECMP_HASH_GLOBAL_INFO     *glbl_hash_info
  );

uint32
  soc_pb_pp_frwrd_fec_ecmp_hashing_global_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_ECMP_HASH_GLOBAL_INFO     *glbl_hash_info
  );

uint32
  soc_pb_pp_frwrd_fec_ecmp_hashing_global_info_get_verify(
    SOC_SAND_IN  int                                 unit
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_frwrd_fec_ecmp_hashing_global_info_set_unsafe"
 *     API.
 *     Refer to
 *     "soc_pb_pp_frwrd_fec_ecmp_hashing_global_info_set_unsafe"
 *     API for details.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_fec_ecmp_hashing_global_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_FEC_ECMP_HASH_GLOBAL_INFO     *glbl_hash_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_fec_ecmp_hashing_port_info_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the ECMP hashing per-port attributes
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_PORT                                port_ndx -
 *     Port ID.
 *   SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_ECMP_HASH_PORT_INFO       *port_hash_info -
 *     ECMP Hashing per-port settings
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_fec_ecmp_hashing_port_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                port_ndx,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_ECMP_HASH_PORT_INFO       *port_hash_info
  );

uint32
  soc_pb_pp_frwrd_fec_ecmp_hashing_port_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                port_ndx,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_ECMP_HASH_PORT_INFO       *port_hash_info
  );

uint32
  soc_pb_pp_frwrd_fec_ecmp_hashing_port_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                port_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_frwrd_fec_ecmp_hashing_port_info_set_unsafe" API.
 *     Refer to
 *     "soc_pb_pp_frwrd_fec_ecmp_hashing_port_info_set_unsafe" API
 *     for details.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_fec_ecmp_hashing_port_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                port_ndx,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_FEC_ECMP_HASH_PORT_INFO       *port_hash_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_fec_get_procs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of procedures of the
 *   soc_pb_pp_api_frwrd_fec module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
SOC_PROCEDURE_DESC_ELEMENT*
  soc_pb_pp_frwrd_fec_get_procs_ptr(void);

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_fec_get_errs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of errors of the
 *   soc_pb_pp_api_frwrd_fec module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
SOC_ERROR_DESC_ELEMENT*
  soc_pb_pp_frwrd_fec_get_errs_ptr(void);

uint32
  SOC_PB_PP_FRWRD_FEC_GLBL_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_GLBL_INFO *info
  );

uint32
  SOC_PB_PP_FRWRD_FEC_ENTRY_RPF_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_ENTRY_RPF_INFO *info
  );

uint32
  SOC_PB_PP_FRWRD_FEC_ENTRY_APP_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_ENTRY_APP_INFO *info,
    SOC_SAND_IN  SOC_PB_PP_FEC_TYPE                 entry_type
  );

uint32
  SOC_PB_PP_FRWRD_FEC_ENTRY_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_ENTRY_INFO *info
  );

uint32
  SOC_PB_PP_FRWRD_FEC_ENTRY_ACCESSED_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_ENTRY_ACCESSED_INFO *info
  );

uint32
  SOC_PB_PP_FRWRD_FEC_PROTECT_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_PROTECT_INFO *info
  );

uint32
  SOC_PB_PP_FRWRD_FEC_ENTRY_USE_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_ENTRY_USE_INFO *info
  );

uint32
  SOC_PB_PP_FRWRD_FEC_MATCH_RULE_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_MATCH_RULE *info
  );

uint32
  SOC_PB_PP_FRWRD_FEC_ECMP_HASH_GLOBAL_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_ECMP_HASH_GLOBAL_INFO *info
  );

uint32
  SOC_PB_PP_FRWRD_FEC_ECMP_HASH_PORT_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_FEC_ECMP_HASH_PORT_INFO *info
  );

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PB_PP_FRWRD_FEC_INCLUDED__*/
#endif

