/* $Id: soc_pb_pp_fp_egr.h,v 1.6 Broadcom SDK $
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

#ifndef __SOC_PB_PP_FP_EGR_INCLUDED__
/* { */
#define __SOC_PB_PP_FP_EGR_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPC/ppc_api_fp_egr.h>

#include <soc/dpp/Petra/PB_TM/pb_api_general.h>

#include <soc/dpp/Petra/PB_PP/pb_pp_framework.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_api_fp.h>

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

#define SOC_PB_PP_FP_EGR_MANAGE_TYPE_ADD                       SOC_PPC_FP_EGR_MANAGE_TYPE_ADD
#define SOC_PB_PP_FP_EGR_MANAGE_TYPE_RMV                       SOC_PPC_FP_EGR_MANAGE_TYPE_RMV
#define SOC_PB_PP_FP_EGR_MANAGE_TYPE_GET                       SOC_PPC_FP_EGR_MANAGE_TYPE_GET
#define SOC_PB_PP_NOF_FP_EGR_MANAGE_TYPES                      SOC_PPC_NOF_FP_EGR_MANAGE_TYPES
typedef SOC_PPC_FP_EGR_MANAGE_TYPE                             SOC_PB_PP_FP_EGR_MANAGE_TYPE;


typedef enum
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PB_PP_FP_EGR_DATABASE_CREATE = SOC_PB_PP_PROC_DESC_BASE_FP_EGR_FIRST,
  SOC_PB_PP_FP_EGR_DATABASE_CREATE_PRINT,
  SOC_PB_PP_FP_EGR_DATABASE_CREATE_UNSAFE,
  SOC_PB_PP_FP_EGR_DATABASE_CREATE_VERIFY,
  SOC_PB_PP_FP_EGR_DATABASE_GET,
  SOC_PB_PP_FP_EGR_DATABASE_GET_PRINT,
  SOC_PB_PP_FP_EGR_DATABASE_GET_UNSAFE,
  SOC_PB_PP_FP_EGR_DATABASE_GET_VERIFY,
  SOC_PB_PP_FP_EGR_DATABASE_DESTROY,
  SOC_PB_PP_FP_EGR_DATABASE_DESTROY_PRINT,
  SOC_PB_PP_FP_EGR_DATABASE_DESTROY_UNSAFE,
  SOC_PB_PP_FP_EGR_DATABASE_DESTROY_VERIFY,
  SOC_PB_PP_FP_EGR_ENTRY_ADD,
  SOC_PB_PP_FP_EGR_ENTRY_ADD_PRINT,
  SOC_PB_PP_FP_EGR_ENTRY_ADD_UNSAFE,
  SOC_PB_PP_FP_EGR_ENTRY_ADD_VERIFY,
  SOC_PB_PP_FP_EGR_ENTRY_GET,
  SOC_PB_PP_FP_EGR_ENTRY_GET_PRINT,
  SOC_PB_PP_FP_EGR_ENTRY_GET_UNSAFE,
  SOC_PB_PP_FP_EGR_ENTRY_GET_VERIFY,
  SOC_PB_PP_FP_EGR_ENTRY_REMOVE,
  SOC_PB_PP_FP_EGR_ENTRY_REMOVE_PRINT,
  SOC_PB_PP_FP_EGR_ENTRY_REMOVE_UNSAFE,
  SOC_PB_PP_FP_EGR_ENTRY_REMOVE_VERIFY,
  SOC_PB_PP_FP_EGR_DB_ID_MANAGE,
  SOC_PB_PP_FP_EGR_DB_ID_MANAGE_PRINT,
  SOC_PB_PP_FP_EGR_DB_ID_MANAGE_UNSAFE,
  SOC_PB_PP_FP_EGR_DB_ID_MANAGE_VERIFY,
  SOC_PB_PP_FP_EGR_GET_PROCS_PTR,
  SOC_PB_PP_FP_EGR_GET_ERRS_PTR,
  /*
   * } Auto generated. Do not edit previous section.
   */
   SOC_PB_PP_FP_EGR_FWD_TYPE_CONVERT,



  /*
   * Last element. Do no touch.
   */
  SOC_PB_PP_FP_EGR_PROCEDURE_DESC_LAST
} SOC_PB_PP_FP_EGR_PROCEDURE_DESC;

typedef enum
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PB_PP_FP_EGR_DB_ID_NDX_OUT_OF_RANGE_ERR = SOC_PB_PP_ERR_DESC_BASE_FP_EGR_FIRST,
  SOC_PB_PP_FP_EGR_ENTRY_ID_NDX_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FP_EGR_IS_FOUND_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FP_EGR_HW_EGR_DB_OUT_OF_RANGE_ERR,
  /*
   * } Auto generated. Do not edit previous section.
   */
   SOC_PB_PP_FP_EGR_MANAGE_TYPE_OUT_OF_RANGE_ERR,
   SOC_PB_PP_FP_EGR_QUALS_NOT_PREDEFINED_KEY_ERR,
   SOC_PB_PP_FP_ACTION_TYPES_NOT_EGRESS_ERR,



  /*
   * Last element. Do no touch.
   */
  SOC_PB_PP_FP_EGR_ERR_LAST
} SOC_PB_PP_FP_EGR_ERR;

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
  soc_pb_pp_fp_egr_fwd_type_convert(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PB_PP_FP_FWD_TYPE            fwd_type_ndx,
    SOC_SAND_OUT SOC_PB_PKT_FRWRD_TYPE            *fwd_type
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_fp_egr_database_create_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Create an Egress ACL database.
 * INPUT:
 *   SOC_SAND_IN  int                unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                db_id_ndx -
 *     Database-ID. Range: 0 - 127.
 *   SOC_SAND_IN  SOC_PB_PP_FP_DATABASE_INFO   *info -
 *     Database parameters.
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE     *success -
 *     Indicate if the database is created successfully.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_fp_egr_database_create_unsafe(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32                db_id_ndx,
    SOC_SAND_IN  SOC_PB_PP_FP_DATABASE_INFO     *info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE     *success
  );

uint32
  soc_pb_pp_fp_egr_database_create_verify(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32                db_id_ndx,
    SOC_SAND_IN  SOC_PB_PP_FP_DATABASE_INFO     *info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_fp_egr_database_get_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the database parameters.
 * INPUT:
 *   SOC_SAND_IN  int                unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                db_id_ndx -
 *     Database-ID. Range: 0 - 127.
 *   SOC_SAND_OUT SOC_PB_PP_FP_DATABASE_INFO   *info -
 *     Database parameters.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_fp_egr_database_get_unsafe(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32                db_id_ndx,
    SOC_SAND_OUT SOC_PB_PP_FP_DATABASE_INFO     *info
  );

uint32
  soc_pb_pp_fp_egr_database_get_verify(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32                db_id_ndx
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_fp_egr_database_destroy_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Destroy the database: all its entries are suppressed and
 *   the Database-ID is freed.
 * INPUT:
 *   SOC_SAND_IN  int                unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                db_id_ndx -
 *     Database-ID. Range: 0 - 127.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_fp_egr_database_destroy_unsafe(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32                db_id_ndx
  );

uint32
  soc_pb_pp_fp_egr_database_destroy_verify(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32                db_id_ndx
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_fp_tcam_entry_add_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Add an entry to the Database.
 * INPUT:
 *   SOC_SAND_IN  int                unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                db_id_ndx -
 *     Database-ID. Range: 0 - 127.
 *   SOC_SAND_IN  uint32                entry_id_ndx -
 *     Entry handle ID.
 *   SOC_SAND_IN  SOC_PB_PP_FP_ENTRY_INFO      *info -
 *     Entry parameters.
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE     *success -
 *     Indicate if the database entry is created successfully.
 * REMARKS:
 *   None
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_fp_tcam_entry_add_unsafe(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32                db_id_ndx,
    SOC_SAND_IN  uint32                entry_id_ndx,
    SOC_SAND_IN  SOC_PB_PP_FP_ENTRY_INFO        *info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE     *success
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_fp_tcam_entry_get_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get an entry from the Database.
 * INPUT:
 *   SOC_SAND_IN  int                unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                db_id_ndx -
 *     Database-ID. Range: 0 - 127.
 *   SOC_SAND_IN  uint32                entry_id_ndx -
 *     Entry-ID. Range: 0 - 4K-1.
 *   SOC_SAND_OUT uint8                *is_found -
 *     If True, then the entry is found and the entry
 *     parameters are returned in the 'info' structure.
 *     Otherwise, the entry is not present in the Database.
 *   SOC_SAND_OUT SOC_PB_PP_FP_ENTRY_INFO      *info -
 *     Entry parameters. Meaningful only the entry is found.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_fp_tcam_entry_get_unsafe(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32                db_id_ndx,
    SOC_SAND_IN  uint32                entry_id_ndx,
    SOC_SAND_OUT uint8                *is_found,
    SOC_SAND_OUT SOC_PB_PP_FP_ENTRY_INFO        *info
  );

uint32
  soc_pb_pp_fp_tcam_entry_get_verify(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32                db_id_ndx,
    SOC_SAND_IN  uint32                entry_id_ndx
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_fp_tcam_entry_remove_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Remove an entry from the Database.
 * INPUT:
 *   SOC_SAND_IN  int                unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                db_id_ndx -
 *     Database-ID. Range: 0 - 127.
 *   SOC_SAND_IN  uint32                entry_id_ndx -
 *     Entry-ID. Range: 0 - 4K-1.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_fp_tcam_entry_remove_unsafe(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32                db_id_ndx,
    SOC_SAND_IN  uint32                entry_id_ndx,
    SOC_SAND_OUT uint8                *is_found
  );

uint32
  soc_pb_pp_fp_tcam_entry_remove_verify(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32                db_id_ndx,
    SOC_SAND_IN  uint32                entry_id_ndx
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_fp_egr_db_id_manage_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Manage the mapping between the Database-ID and the
 *   Hardware egress ACL id.
 * INPUT:
 *   SOC_SAND_IN  int                unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                db_id_ndx -
 *     Database-ID. Range: 0 - 127.
 *   SOC_SAND_IN  SOC_PB_PP_FP_EGR_MANAGE_TYPE manage_type -
 *     Manage type: add, remove or get.
 *   SOC_SAND_OUT uint32                 *hw_egr_db -
 *     Hardware database. Range: 0 - 7.
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE     *success -
 *     Indicate if the hardware profile is added successfully.
 *     Relevant only for add type.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_fp_egr_db_id_manage_unsafe(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32                db_id_ndx,
    SOC_SAND_IN  SOC_PB_PP_FP_EGR_MANAGE_TYPE   manage_type,
    SOC_SAND_OUT uint32                 *hw_egr_db,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE     *success
  );

uint32
  soc_pb_pp_fp_egr_db_id_manage_verify(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32                db_id_ndx,
    SOC_SAND_IN  SOC_PB_PP_FP_EGR_MANAGE_TYPE   manage_type
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_fp_egr_get_procs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of procedures of the
 *   soc_pb_pp_api_fp_egr module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
SOC_PROCEDURE_DESC_ELEMENT*
  soc_pb_pp_fp_egr_get_procs_ptr(void);

/*********************************************************************
* NAME:
 *   soc_pb_pp_fp_egr_get_errs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of errors of the
 *   soc_pb_pp_api_fp_egr module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
SOC_ERROR_DESC_ELEMENT*
  soc_pb_pp_fp_egr_get_errs_ptr(void);
#if SOC_PB_PP_DEBUG_IS_LVL1

const char*
  SOC_PB_PP_FP_EGR_MANAGE_TYPE_to_string(
    SOC_SAND_IN  SOC_PB_PP_FP_EGR_MANAGE_TYPE  enum_val
  );

#endif /* SOC_PB_PP_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PB_PP_FP_EGR_INCLUDED__*/
#endif

