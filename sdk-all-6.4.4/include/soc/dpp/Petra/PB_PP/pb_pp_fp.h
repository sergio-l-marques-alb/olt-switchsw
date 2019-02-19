/* $Id: pb_pp_fp.h,v 1.8 Broadcom SDK $
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
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_PP/include/soc_pb_pp_fp.h
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

#ifndef __SOC_PB_PP_FP_INCLUDED__
/* { */
#define __SOC_PB_PP_FP_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/Petra/PB_PP/pb_pp_api_fp.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_framework.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_llp_parse.h>

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
  SOC_PB_PP_FP_PACKET_FORMAT_GROUP_SET = SOC_PB_PP_PROC_DESC_BASE_FP_FIRST,
  SOC_PB_PP_FP_PACKET_FORMAT_GROUP_SET_PRINT,
  SOC_PB_PP_FP_PACKET_FORMAT_GROUP_SET_UNSAFE,
  SOC_PB_PP_FP_PACKET_FORMAT_GROUP_SET_VERIFY,
  SOC_PB_PP_FP_PACKET_FORMAT_GROUP_GET,
  SOC_PB_PP_FP_PACKET_FORMAT_GROUP_GET_PRINT,
  SOC_PB_PP_FP_PACKET_FORMAT_GROUP_GET_VERIFY,
  SOC_PB_PP_FP_PACKET_FORMAT_GROUP_GET_UNSAFE,
  SOC_PB_PP_FP_DATABASE_CREATE,
  SOC_PB_PP_FP_DATABASE_CREATE_PRINT,
  SOC_PB_PP_FP_DATABASE_CREATE_UNSAFE,
  SOC_PB_PP_FP_DATABASE_CREATE_VERIFY,
  SOC_PB_PP_FP_DATABASE_GET,
  SOC_PB_PP_FP_DATABASE_GET_PRINT,
  SOC_PB_PP_FP_DATABASE_GET_UNSAFE,
  SOC_PB_PP_FP_DATABASE_GET_VERIFY,
  SOC_PB_PP_FP_DATABASE_DESTROY,
  SOC_PB_PP_FP_DATABASE_DESTROY_PRINT,
  SOC_PB_PP_FP_DATABASE_DESTROY_UNSAFE,
  SOC_PB_PP_FP_DATABASE_DESTROY_VERIFY,
  SOC_PB_PP_FP_ENTRY_ADD,
  SOC_PB_PP_FP_ENTRY_ADD_PRINT,
  SOC_PB_PP_FP_ENTRY_ADD_UNSAFE,
  SOC_PB_PP_FP_ENTRY_ADD_VERIFY,
  SOC_PB_PP_FP_ENTRY_GET,
  SOC_PB_PP_FP_ENTRY_GET_PRINT,
  SOC_PB_PP_FP_ENTRY_GET_UNSAFE,
  SOC_PB_PP_FP_ENTRY_GET_VERIFY,
  SOC_PB_PP_FP_ENTRY_REMOVE,
  SOC_PB_PP_FP_ENTRY_REMOVE_PRINT,
  SOC_PB_PP_FP_ENTRY_REMOVE_UNSAFE,
  SOC_PB_PP_FP_ENTRY_REMOVE_VERIFY,
  SOC_PB_PP_FP_DATABASE_ENTRIES_GET_BLOCK,
  SOC_PB_PP_FP_DATABASE_ENTRIES_GET_BLOCK_PRINT,
  SOC_PB_PP_FP_DATABASE_ENTRIES_GET_BLOCK_UNSAFE,
  SOC_PB_PP_FP_DATABASE_ENTRIES_GET_BLOCK_VERIFY,
  SOC_PB_PP_FP_DIRECT_EXTRACTION_ENTRY_ADD,
  SOC_PB_PP_FP_DIRECT_EXTRACTION_ENTRY_ADD_PRINT,
  SOC_PB_PP_FP_DIRECT_EXTRACTION_ENTRY_ADD_UNSAFE,
  SOC_PB_PP_FP_DIRECT_EXTRACTION_ENTRY_ADD_VERIFY,
  SOC_PB_PP_FP_DIRECT_EXTRACTION_ENTRY_GET,
  SOC_PB_PP_FP_DIRECT_EXTRACTION_ENTRY_GET_PRINT,
  SOC_PB_PP_FP_DIRECT_EXTRACTION_ENTRY_GET_UNSAFE,
  SOC_PB_PP_FP_DIRECT_EXTRACTION_ENTRY_GET_VERIFY,
  SOC_PB_PP_FP_DIRECT_EXTRACTION_ENTRY_REMOVE,
  SOC_PB_PP_FP_DIRECT_EXTRACTION_ENTRY_REMOVE_PRINT,
  SOC_PB_PP_FP_DIRECT_EXTRACTION_ENTRY_REMOVE_UNSAFE,
  SOC_PB_PP_FP_DIRECT_EXTRACTION_ENTRY_REMOVE_VERIFY,
  SOC_PB_PP_FP_DIRECT_EXTRACTION_DB_ENTRIES_GET,
  SOC_PB_PP_FP_DIRECT_EXTRACTION_DB_ENTRIES_GET_PRINT,
  SOC_PB_PP_FP_DIRECT_EXTRACTION_DB_ENTRIES_GET_UNSAFE,
  SOC_PB_PP_FP_DIRECT_EXTRACTION_DB_ENTRIES_GET_VERIFY,
  SOC_PB_PP_FP_CONTROL_SET,
  SOC_PB_PP_FP_CONTROL_SET_PRINT,
  SOC_PB_PP_FP_CONTROL_SET_UNSAFE,
  SOC_PB_PP_FP_CONTROL_SET_VERIFY,
  SOC_PB_PP_FP_CONTROL_GET,
  SOC_PB_PP_FP_CONTROL_GET_PRINT,
  SOC_PB_PP_FP_CONTROL_GET_VERIFY,
  SOC_PB_PP_FP_CONTROL_GET_UNSAFE,
  SOC_PB_PP_FP_EGR_DB_MAP_SET,
  SOC_PB_PP_FP_EGR_DB_MAP_SET_PRINT,
  SOC_PB_PP_FP_EGR_DB_MAP_SET_UNSAFE,
  SOC_PB_PP_FP_EGR_DB_MAP_SET_VERIFY,
  SOC_PB_PP_FP_EGR_DB_MAP_GET,
  SOC_PB_PP_FP_EGR_DB_MAP_GET_PRINT,
  SOC_PB_PP_FP_EGR_DB_MAP_GET_VERIFY,
  SOC_PB_PP_FP_EGR_DB_MAP_GET_UNSAFE,
  SOC_PB_PP_FP_PACKET_DIAG_GET,
  SOC_PB_PP_FP_PACKET_DIAG_GET_PRINT,
  SOC_PB_PP_FP_PACKET_DIAG_GET_UNSAFE,
  SOC_PB_PP_FP_PACKET_DIAG_GET_VERIFY,
  SOC_PB_PP_FP_GET_PROCS_PTR,
  SOC_PB_PP_FP_GET_ERRS_PTR,
  /*
   * } Auto generated. Do not edit previous section.
   */
   SOC_PB_PP_FP_DB_TYPES_VERIFY_FIRST_STAGE,
   SOC_PB_PP_FP_QUAL_TYPES_VERIFY_FIRST_STAGE,
   SOC_PB_PP_FP_ACTION_TYPES_VERIFY_FIRST_STAGE,
   SOC_PB_PP_FP_ACTION_TYPE_TO_CE_COPY,
   SOC_PB_PP_FP_TCAM_CALLBACK,
   SOC_PB_PP_FP_INIT_UNSAFE,
   SOC_PB_PP_FP_ACTION_TYPE_MAX_SIZE_GET,
   SOC_PB_PP_FP_ACTION_TYPE_TO_PMF_CONVERT,
   SOC_PB_PP_FP_FEM_SET,
   SOC_PB_PP_FP_FEM_DB_FIND,
   SOC_PB_PP_FP_QUAL_TYPE_PREDEFINED_KEY_GET,
   SOC_PB_PP_FP_PREDEFINED_KEY_GET,
   SOC_PB_PP_FP_KEY_GET,
   SOC_PB_PP_FP_QUAL_TYPE_TO_TCAM_TYPE_CONVERT,
   SOC_PB_PP_FP_QUAL_TYPE_TO_KEY_FLD_CONVERT,
   SOC_PB_PP_FP_PREDEFINED_KEY_SIZE_AND_TYPE_GET,
   SOC_PB_PP_FP_ENTRY_VALIDITY_GET,
   SOC_PB_PP_FP_QUAL_TYPE_PRESET1,
   SOC_PB_PP_FP_QUAL_VAL_ENCODE,
   SOC_PB_PP_FP_ACTION_LSB_GET,
   SOC_PB_PP_FP_QUAL_LSB_AND_LENGTH_GET,
   SOC_PB_PP_FP_CE_KEY_LENGTH_MINIMAL_GET,
   SOC_PB_PP_FP_KEY_INPUT_GET,
   SOC_PB_PP_FP_ENTRY_NDX_DIRECT_TABLE_GET,
   SOC_PB_PP_FP_QUAL_TYPE_AND_LOCAL_LSB_GET,
   SOC_PB_PP_FP_ACTION_TYPE_FROM_PMF_CONVERT,
   SOC_PB_PP_FP_INGRESS_QUAL_VERIFY,
   SOC_PB_PP_FP_IPV4_HOST_EXTEND_ENABLE,
   SOC_PB_PP_FP_CYCLE_FOR_DB_INFO_VERIFY,


  /*
   * Last element. Do no touch.
   */
  SOC_PB_PP_FP_PROCEDURE_DESC_LAST
} SOC_PB_PP_FP_PROCEDURE_DESC;

typedef enum
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PB_PP_FP_PFG_NDX_OUT_OF_RANGE_ERR = SOC_PB_PP_ERR_DESC_BASE_FP_FIRST,
  SOC_PB_PP_FP_DB_ID_NDX_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FP_SUCCESS_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FP_ENTRY_ID_NDX_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FP_IS_FOUND_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FP_NOF_ENTRIES_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FP_FWD_TYPE_NDX_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FP_PORT_PROFILE_NDX_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FP_TYPE_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FP_HDR_FORMAT_BMP_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FP_VLAN_TAG_STRUCTURE_BMP_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FP_DB_TYPE_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FP_SUPPORTED_PFGS_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FP_QUAL_TYPES_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FP_DATABASE_TYPE_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FP_ACTION_TYPES_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FP_STRENGTH_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FP_VAL_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FP_PRIORITY_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FP_FLD_LSB_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FP_CST_VAL_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FP_NOF_BITS_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FP_NOF_FIELDS_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FP_BASE_VAL_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FP_DB_ID_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FP_VAL_NDX_OUT_OF_RANGE_ERR,
  /*
   * } Auto generated. Do not edit previous section.
   */
   SOC_PB_PP_FP_DB_ID_ALREADY_EXIST_ERR,
   SOC_PB_PP_FP_DB_ID_NOT_DIRECT_EXTRACTION_ERR,
   SOC_PB_PP_FP_DB_ID_NOT_LOOKUP_ERR,
   SOC_PB_PP_FP_ENTRY_QUAL_TYPE_NOT_IN_DB_ERR,
   SOC_PB_PP_FP_ENTRY_ACTION_TYPE_NOT_IN_DB_ERR,
   SOC_PB_PP_FP_ENTRY_ALREADY_EXIST_ERR,
   SOC_PB_PP_FP_QUAL_TYPES_NOT_PREDEFINED_ACL_KEY_ERR,
   SOC_PB_PP_FP_DB_CREATION_FOR_IPV4_HOST_EXTEND_ERR,
   SOC_PB_PP_FP_EGR_DATABASE_NOT_ALREADY_ADDED_ERR,
   SOC_PB_PP_FP_QUAL_TYPES_END_OF_LIST_ERR,
   SOC_PB_PP_FP_ACTION_TYPES_END_OF_LIST_ERR,
   SOC_PB_PP_FP_DB_ID_DIRECT_TABLE_ALREADY_EXIST_ERR,
   SOC_PB_PP_FP_ACTION_LENGTHS_OUT_OF_RANGE_ERR,
   SOC_PB_PP_FP_QUALS_LENGTHS_OUT_OF_RANGE_ERR,
   SOC_PB_PP_FP_EGRESS_QUAL_USED_FOR_INGRESS_DB_ERR,
   SOC_PB_PP_FP_TAG_ACTION_ALREADY_EXIST_ERR,
   SOC_PB_PP_FP_QUAL_NOT_EGRESS_ERR,
   SOC_PB_PP_FP_ACTIONS_MIXED_WITH_TAG_ERR,


  /*
   * Last element. Do no touch.
   */
  SOC_PB_PP_FP_ERR_LAST
} SOC_PB_PP_FP_ERR;

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
  soc_pb_pp_fp_key_get(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  uint8                     is_for_egress,
    SOC_SAND_IN  SOC_PB_PP_FP_QUAL_TYPE           *qual_types,
    SOC_SAND_OUT SOC_PB_PP_FP_PREDEFINED_ACL_KEY  *predefined_acl_key
  );

  uint32
    soc_pb_pp_fp_qual_predefined_size_get(
      SOC_SAND_IN  int               unit,
      SOC_SAND_IN  SOC_PB_PP_FP_PREDEFINED_ACL_KEY predefined_acl_key,
      SOC_SAND_IN  SOC_PB_PP_FP_QUAL_TYPE   qual_type,
      SOC_SAND_OUT uint8                *is_found,
      SOC_SAND_OUT uint32               *length
    );

uint32
  soc_pb_pp_fp_init_unsafe(
    SOC_SAND_IN  int                                 unit
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_fp_packet_format_group_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set a Packet Format Group (PFG). The packet format group
 *   defines the supported Packet formats. The user must
 *   indicate for each Database which Packet format(s) are
 *   associated with this Database. E.g.: A Packet Format
 *   Group including only IPv6 packets can be defined to use
 *   Databases with IPv6 Destination-IP qualifiers.
 * INPUT:
 *   SOC_SAND_IN  int                    unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                    pfg_ndx -
 *     Packet Format Group index. Range: 0 - 4.
 *   SOC_SAND_IN  SOC_PB_PP_FP_PFG_INFO            *info -
 *     Packet-Format-Group parameters.
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE         *success -
 *     Indicate if the database is created successfully.
 * REMARKS:
 *   The user should set a minimal number of Packet Format
 *   Groups since each one uses many Hardware resources.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_fp_packet_format_group_set_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    pfg_ndx,
    SOC_SAND_IN  SOC_PB_PP_FP_PFG_INFO            *info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE         *success
  );

uint32
  soc_pb_pp_fp_packet_format_group_set_verify(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    pfg_ndx,
    SOC_SAND_IN  SOC_PB_PP_FP_PFG_INFO            *info
  );

uint32
  soc_pb_pp_fp_packet_format_group_get_verify(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    pfg_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_fp_packet_format_group_set_unsafe" API.
 *     Refer to "soc_pb_pp_fp_packet_format_group_set_unsafe" API
 *     for details.
*********************************************************************/
uint32
  soc_pb_pp_fp_packet_format_group_get_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    pfg_ndx,
    SOC_SAND_OUT SOC_PB_PP_FP_PFG_INFO            *info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_fp_database_create_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Create a database. Each database specifies the action
 *   types to perform and the qualifier fields for this
 *   Database. Entries in the database specify the specific
 *   actions to be taken upon specific values of the
 *   packet. E.g.: Policy Based Routing database update the
 *   FEC value according to DSCP DIP and In-RIF. An entry in
 *   the database may set the FEC of a packet with DIP
 *   1.2.2.3, DSCP value 7 and In-RIF 3 to be 9.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                 db_id_ndx -
 *     Database-ID. Range: 0 - 127.
 *   SOC_SAND_IN  SOC_PB_PP_FP_DATABASE_INFO                    *info -
 *     Database parameters.
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success -
 *     Indicate if the database is created successfully.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_fp_database_create_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_IN  SOC_PB_PP_FP_DATABASE_INFO                    *info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );

uint32
  soc_pb_pp_fp_database_create_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_IN  SOC_PB_PP_FP_DATABASE_INFO                    *info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_fp_database_get_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the database parameters.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                 db_id_ndx -
 *     Database-ID. Range: 0 - 127.
 *   SOC_SAND_OUT SOC_PB_PP_FP_DATABASE_INFO                    *info -
 *     Database parameters.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_fp_database_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_OUT SOC_PB_PP_FP_DATABASE_INFO                    *info
  );

uint32
  soc_pb_pp_fp_database_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_fp_database_destroy_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Destroy the database: all its entries are suppressed and
 *   the Database-ID is freed.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                 db_id_ndx -
 *     Database-ID. Range: 0 - 127.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_fp_database_destroy_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx
  );

uint32
  soc_pb_pp_fp_database_destroy_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_fp_entry_add_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Add an entry to the Database. The database entry is
 *   selected if the entire relevant packet field values are
 *   matched to the database entry qualifiers values. When
 *   the packet is qualified to several entries, the entry
 *   with the strongest priority is chosen.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                 db_id_ndx -
 *     Database-ID. Range: 0 - 127.
 *   SOC_SAND_IN  uint32                                 entry_id_ndx -
 *     Entry handle ID. The entry index is a SW handle, to
 *     enable retrieving the entry attributes by the
 *     soc_ppd_fp_entry_get() function, and remove it by the
 *     soc_ppd_fp_entry_remove() function. The actual location of
 *     the entry in the database is selected according to the
 *     entry's priority. Range: 0 - 4K-1.
 *   SOC_SAND_IN  SOC_PB_PP_FP_ENTRY_INFO                       *info -
 *     Entry parameters.
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success -
 *     Indicate if the database entry is created successfully.
 * REMARKS:
 *   1. The database must be created before the insertion of
 *   an entry. To create a Database, call the
 *   soc_ppd_fp_database_create API.2. The database must be NOT
 *   of type 'direct extraction'3. For a Database of type
 *   'Direct Table', the entry qualifier value must be not
 *   masked.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_fp_entry_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_IN  uint32                                 entry_id_ndx,
    SOC_SAND_IN  SOC_PB_PP_FP_ENTRY_INFO                       *info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );

uint32
  soc_pb_pp_fp_entry_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_IN  uint32                                 entry_id_ndx,
    SOC_SAND_IN  SOC_PB_PP_FP_ENTRY_INFO                       *info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_fp_entry_get_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get an entry from the Database.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                 db_id_ndx -
 *     Database-ID. Range: 0 - 127.
 *   SOC_SAND_IN  uint32                                 entry_id_ndx -
 *     Entry-ID. Range: 0 - 4K-1.
 *   SOC_SAND_OUT uint8                                 *is_found -
 *     If True, then the entry is found and the entry
 *     parameters are returned in the 'info' structure.
 *     Otherwise, the entry is not present in the Database.
 *   SOC_SAND_OUT SOC_PB_PP_FP_ENTRY_INFO                       *info -
 *     Entry parameters. Meaningful only the entry is found.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_fp_entry_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_IN  uint32                                 entry_id_ndx,
    SOC_SAND_OUT uint8                                 *is_found,
    SOC_SAND_OUT SOC_PB_PP_FP_ENTRY_INFO                       *info
  );

uint32
  soc_pb_pp_fp_entry_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_IN  uint32                                 entry_id_ndx
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_fp_entry_remove_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Remove an entry from the Database.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                 db_id_ndx -
 *     Database-ID. Range: 0 - 127.
 *   SOC_SAND_IN  uint32                                 entry_id_ndx -
 *     Entry-ID. Range: 0 - 4K-1.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_fp_entry_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_IN  uint32                                 entry_id_ndx
  );

uint32
  soc_pb_pp_fp_entry_remove_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_IN  uint32                                 entry_id_ndx
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_fp_database_entries_get_block_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the Database entries. The function returns list of
 *   entries that were added to a database with database ID
 *   'db_id_ndx'.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                 db_id_ndx -
 *     Database-ID. Range: 0 - 127.
 *   SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                    *block_range -
 *     Block range to get entries in this Database.
 *   SOC_SAND_OUT SOC_PB_PP_FP_ENTRY_INFO                       *entries -
 *     Database entries.
 *   SOC_SAND_OUT uint32                                  *nof_entries -
 *     Number of valid entries.
 * REMARKS:
 *   This API can be called only if the Database is NOT of
 *   type 'direct extraction'.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_fp_database_entries_get_block_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                    *block_range,
    SOC_SAND_OUT SOC_PB_PP_FP_ENTRY_INFO                       *entries,
    SOC_SAND_OUT uint32                                  *nof_entries
  );

uint32
  soc_pb_pp_fp_database_entries_get_block_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                    *block_range
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_fp_direct_extraction_entry_add_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Add an entry to the Database. The database entry is
 *   selected if all the Packet Qualifier field values are in
 *   the Database entry range.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                 db_id_ndx -
 *     Database-ID. Range: 0 - 127.
 *   SOC_SAND_IN  uint32                                 entry_id_ndx -
 *     Entry-ID. Range: 0 - 15.
 *   SOC_SAND_IN  SOC_PB_PP_FP_DIR_EXTR_ENTRY_INFO              *info -
 *     Entry parameters.
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success -
 *     Indicate if the database entry is created successfully.
 * REMARKS:
 *   1. The database must be created before the insertion of
 *   an entry. To create a Database, call the
 *   soc_ppd_fp_database_create API.2. The database must be of
 *   type 'direct extraction'3. The priority enables
 *   selection between two database entries with a
 *   superposition in the Qualifier field ranges.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_fp_direct_extraction_entry_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_IN  uint32                                 entry_id_ndx,
    SOC_SAND_IN  SOC_PB_PP_FP_DIR_EXTR_ENTRY_INFO              *info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );

uint32
  soc_pb_pp_fp_direct_extraction_entry_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_IN  uint32                                 entry_id_ndx,
    SOC_SAND_IN  SOC_PB_PP_FP_DIR_EXTR_ENTRY_INFO              *info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_fp_direct_extraction_entry_get_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get an entry from the Database.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                 db_id_ndx -
 *     Database-ID. Range: 0 - 127.
 *   SOC_SAND_IN  uint32                                 entry_id_ndx -
 *     Entry-ID. Range: 0 - 15.
 *   SOC_SAND_OUT uint8                                 *is_found -
 *     If True, then the entry is found and the entry
 *     parameters are returned in the 'info' structure.
 *     Otherwise, the entry is not present in the Database.
 *   SOC_SAND_OUT SOC_PB_PP_FP_DIR_EXTR_ENTRY_INFO              *info -
 *     Entry parameters. Meaningful only the entry is found.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_fp_direct_extraction_entry_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_IN  uint32                                 entry_id_ndx,
    SOC_SAND_OUT uint8                                 *is_found,
    SOC_SAND_OUT SOC_PB_PP_FP_DIR_EXTR_ENTRY_INFO              *info
  );

uint32
  soc_pb_pp_fp_direct_extraction_entry_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_IN  uint32                                 entry_id_ndx
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_fp_direct_extraction_entry_remove_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Remove an entry from the Database.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                 db_id_ndx -
 *     Database-ID. Range: 0 - 127.
 *   SOC_SAND_IN  uint32                                 entry_id_ndx -
 *     Entry-ID. Range: 0 - 15.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_fp_direct_extraction_entry_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_IN  uint32                                 entry_id_ndx
  );

uint32
  soc_pb_pp_fp_direct_extraction_entry_remove_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_IN  uint32                                 entry_id_ndx
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_fp_direct_extraction_db_entries_get_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the Database entries.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                 db_id_ndx -
 *     Database-ID. Range: 0 - 127.
 *   SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                    *block_range -
 *     Block range to get entries in this Database.
 *   SOC_SAND_OUT SOC_PB_PP_FP_DIR_EXTR_ENTRY_INFO              *entries -
 *     Database entries.
 *   SOC_SAND_OUT uint32                                  *nof_entries -
 *     Number of valid entries.
 * REMARKS:
 *   This API can be called only if the Database is of type
 *   'direct extraction'.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_fp_direct_extraction_db_entries_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                    *block_range,
    SOC_SAND_OUT SOC_PB_PP_FP_DIR_EXTR_ENTRY_INFO              *entries,
    SOC_SAND_OUT uint32                                  *nof_entries
  );

uint32
  soc_pb_pp_fp_direct_extraction_db_entries_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                    *block_range
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_fp_control_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set one of the control options.
 * INPUT:
 *   SOC_SAND_IN  int                    unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_FP_CONTROL_INDEX       *control_ndx -
 *     Index for the control set API.
 *   SOC_SAND_IN  SOC_PB_PP_FP_CONTROL_INFO        *info -
 *     Type and Values of selected control option.
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE         *success -
 *     Indicate if the operation has succeeded.
 * REMARKS:
 *   The exact semantics of the info are determined by the
 *   control option specified by the type.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_fp_control_set_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PB_PP_FP_CONTROL_INDEX       *control_ndx,
    SOC_SAND_IN  SOC_PB_PP_FP_CONTROL_INFO        *info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE         *success
  );

uint32
  soc_pb_pp_fp_control_set_verify(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PB_PP_FP_CONTROL_INDEX      * control_ndx,
    SOC_SAND_IN  SOC_PB_PP_FP_CONTROL_INFO        *info
  );

uint32
  soc_pb_pp_fp_control_get_verify(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PB_PP_FP_CONTROL_INDEX      * control_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_fp_control_set_unsafe" API.
 *     Refer to "soc_pb_pp_fp_control_set_unsafe" API for details.
*********************************************************************/
uint32
  soc_pb_pp_fp_control_get_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PB_PP_FP_CONTROL_INDEX      * control_ndx,
    SOC_SAND_OUT SOC_PB_PP_FP_CONTROL_INFO        *info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_fp_egr_db_map_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the mapping between the Packet forward type and the
 *   Port profile to the Database-ID.
 * INPUT:
 *   SOC_SAND_IN  int                    unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_FP_FWD_TYPE            fwd_type_ndx -
 *     Packet forward type.
 *   SOC_SAND_IN  uint32                    port_profile_ndx -
 *     PP-Port profile. Range: 0 - 3.
 *   SOC_SAND_IN  uint32                     db_id -
 *     Database-Id to use for these packets. Range: 0 - 63.
 * REMARKS:
 *   1. The mapping between PP-Port and PP-Port profile is
 *   set via the soc_ppd_fp_control_set API with type
 *   SOC_PPD_FP_CONTROL_TYPE_PP_PORT_PROFILE.2. The Database with
 *   this Database-Id must exist and correspond to an Egress
 *   ACL Database
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_fp_egr_db_map_set_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PB_PP_FP_FWD_TYPE            fwd_type_ndx,
    SOC_SAND_IN  uint32                    port_profile_ndx,
    SOC_SAND_IN  uint32                     db_id
  );

uint32
  soc_pb_pp_fp_egr_db_map_set_verify(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PB_PP_FP_FWD_TYPE            fwd_type_ndx,
    SOC_SAND_IN  uint32                    port_profile_ndx,
    SOC_SAND_IN  uint32                     db_id
  );

uint32
  soc_pb_pp_fp_egr_db_map_get_verify(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PB_PP_FP_FWD_TYPE            fwd_type_ndx,
    SOC_SAND_IN  uint32                    port_profile_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_fp_egr_db_map_set_unsafe" API.
 *     Refer to "soc_pb_pp_fp_egr_db_map_set_unsafe" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_pp_fp_egr_db_map_get_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PB_PP_FP_FWD_TYPE            fwd_type_ndx,
    SOC_SAND_IN  uint32                    port_profile_ndx,
    SOC_SAND_OUT uint32                     *db_id
  );
/*********************************************************************
* NAME:
 *   soc_pb_pp_fp_packet_diag_get_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the Field Processing of the last packets.
 * INPUT:
 *   SOC_SAND_IN  int                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_OUT SOC_PB_PP_FP_PACKET_DIAG_INFO *info -
 *     Field Processor specifications for this packet.
 * REMARKS:
 *   This API must be called during a continuous stream of
 *   the identical packets coming from the same source.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_fp_packet_diag_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PB_PP_FP_PACKET_DIAG_INFO *info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_fp_get_procs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of procedures of the
 *   soc_pb_pp_api_fp module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
SOC_PROCEDURE_DESC_ELEMENT*
  soc_pb_pp_fp_get_procs_ptr(void);

/*********************************************************************
* NAME:
 *   soc_pb_pp_fp_get_errs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of errors of the
 *   soc_pb_pp_api_fp module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
SOC_ERROR_DESC_ELEMENT*
  soc_pb_pp_fp_get_errs_ptr(void);

uint32
  SOC_PB_PP_FP_QUAL_VAL_verify(
    SOC_SAND_IN  SOC_PB_PP_FP_QUAL_VAL *info
  );

uint32
  SOC_PB_PP_FP_PFG_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_FP_PFG_INFO *info
  );

uint32
  SOC_PB_PP_FP_DATABASE_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_FP_DATABASE_INFO *info
  );

uint32
  SOC_PB_PP_FP_ACTION_VAL_verify(
    SOC_SAND_IN  SOC_PB_PP_FP_ACTION_VAL *info
  );

uint32
  SOC_PB_PP_FP_ENTRY_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_FP_ENTRY_INFO *info
  );

uint32
  SOC_PB_PP_FP_DIR_EXTR_ACTION_LOC_verify(
    SOC_SAND_IN  SOC_PB_PP_FP_DIR_EXTR_ACTION_LOC *info
  );

uint32
  SOC_PB_PP_FP_DIR_EXTR_ACTION_VAL_verify(
    SOC_SAND_IN  SOC_PB_PP_FP_DIR_EXTR_ACTION_VAL *info
  );

uint32
  SOC_PB_PP_FP_DIR_EXTR_ENTRY_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_FP_DIR_EXTR_ENTRY_INFO *info
  );

uint32
  SOC_PB_PP_FP_CONTROL_INDEX_verify(
    SOC_SAND_IN  SOC_PB_PP_FP_CONTROL_INDEX *info
  );

uint32
  SOC_PB_PP_FP_CONTROL_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_FP_CONTROL_TYPE type,
    SOC_SAND_IN  SOC_PB_PP_FP_CONTROL_INFO *info
  );

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PB_PP_FP_INCLUDED__*/
#endif

