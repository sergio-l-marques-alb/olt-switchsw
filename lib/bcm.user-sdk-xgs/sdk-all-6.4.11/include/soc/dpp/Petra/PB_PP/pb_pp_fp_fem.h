/* $Id: soc_pb_pp_fp_fem.h,v 1.7 Broadcom SDK $
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

#ifndef __SOC_PB_PP_FP_FEM__INCLUDED__
/* { */
#define __SOC_PB_PP_FP_FEM__INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/Petra/PB_PP/pb_pp_framework.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_api_fp.h>

#include <soc/dpp/PPC/ppc_api_fp_fem.h>

#include <soc/dpp/Petra/PB_TM/pb_pmf_low_level_fem_tag.h>

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

typedef SOC_PPC_FP_FEM_ENTRY                                   SOC_PB_PP_FP_FEM_ENTRY;
typedef SOC_PPC_FP_FEM_CYCLE                                    SOC_PB_PP_FP_FEM_CYCLE;


typedef enum
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PB_PP_FP_FEM_INSERT = SOC_PB_PP_PROC_DESC_BASE_FP_FEM_FIRST,
  SOC_PB_PP_FP_FEM_INSERT_PRINT,
  SOC_PB_PP_FP_FEM_INSERT_UNSAFE,
  SOC_PB_PP_FP_FEM_INSERT_VERIFY,
  SOC_PB_PP_FP_FEM_IS_PLACE_GET,
  SOC_PB_PP_FP_FEM_IS_PLACE_GET_PRINT,
  SOC_PB_PP_FP_FEM_IS_PLACE_GET_UNSAFE,
  SOC_PB_PP_FP_FEM_IS_PLACE_GET_VERIFY,
  SOC_PB_PP_FP_FEM_TAG_SET,
  SOC_PB_PP_FP_FEM_TAG_SET_PRINT,
  SOC_PB_PP_FP_FEM_TAG_SET_UNSAFE,
  SOC_PB_PP_FP_FEM_TAG_SET_VERIFY,
  SOC_PB_PP_FP_FEM_TAG_GET,
  SOC_PB_PP_FP_FEM_TAG_GET_PRINT,
  SOC_PB_PP_FP_FEM_TAG_GET_VERIFY,
  SOC_PB_PP_FP_FEM_TAG_GET_UNSAFE,
  SOC_PB_PP_FP_FEM_GET_PROCS_PTR,
  SOC_PB_PP_FP_FEM_GET_ERRS_PTR,
  /*
   * } Auto generated. Do not edit previous section.
   */
   SOC_PB_PP_FP_TAG_ACTION_TYPE_CONVERT,
   SOC_PB_PP_FP_FEM_IS_PLACE_GET_FOR_CYCLE,
   SOC_PB_PP_FP_FEM_IS_FEM_BLOCKING_GET,
   SOC_PB_PP_FP_FEM_DUPLICATE,
   SOC_PB_PP_FP_FEM_CONFIGURE,
   SOC_PB_PP_FP_FEM_CONFIGURATION_GET,
   SOC_PB_PP_FP_FEM_REMOVE,
   SOC_PB_PP_FP_FEM_REORGANIZE,



  /*
   * Last element. Do no touch.
   */
  SOC_PB_PP_FP_FEM_PROCEDURE_DESC_LAST
} SOC_PB_PP_FP_FEM_PROCEDURE_DESC;

typedef enum
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PB_PP_FP_FEM_PFG_NDX_OUT_OF_RANGE_ERR = SOC_PB_PP_ERR_DESC_BASE_FP_FEM_FIRST,
  SOC_PB_PP_FP_FEM_DB_ID_NDX_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FP_FEM_DB_STRENGTH_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FP_FEM_DB_ID_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FP_FEM_ENTRY_STRENGTH_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FP_FEM_ENTRY_ID_OUT_OF_RANGE_ERR,
  /*
   * } Auto generated. Do not edit previous section.
   */



  /*
   * Last element. Do no touch.
   */
  SOC_PB_PP_FP_FEM_ERR_LAST
} SOC_PB_PP_FP_FEM_ERR;

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
  soc_pb_pp_fp_tag_action_type_convert(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PB_PP_FP_ACTION_TYPE         action_type,
    SOC_SAND_OUT uint8                    *is_tag_action,
    SOC_SAND_OUT uint32                    *action_ndx
  );

uint32
  soc_pb_pp_fp_action_type_from_pmf_convert(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  SOC_PB_PMF_FEM_ACTION_TYPE  pmf_fem_action_type,
    SOC_SAND_OUT SOC_PB_PP_FP_ACTION_TYPE    *fp_action_type
  );

uint32
  soc_pb_pp_fp_qual_lsb_and_length_get(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  uint32                  db_id_ndx,
    SOC_SAND_IN  SOC_PB_PP_FP_QUAL_TYPE         qual_type,
    SOC_SAND_IN  SOC_PB_PP_FP_DATABASE_INFO     *fp_database_info,
    SOC_SAND_OUT uint32                   *qual_lsb,
    SOC_SAND_OUT uint32                   *qual_length_no_padding
  );

uint32
  soc_pb_pp_fp_action_type_max_size_get(
    SOC_SAND_IN  int            unit,
    SOC_SAND_IN  SOC_PB_PP_FP_ACTION_TYPE fp_action_type,
    SOC_SAND_OUT uint32            *action_size_in_bits,
    SOC_SAND_OUT uint32            *action_size_in_bits_in_fem
  );

uint32
  soc_pb_pp_fp_action_lsb_get(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  SOC_PB_PP_FP_ACTION_TYPE       action_type,
    SOC_SAND_IN  SOC_PB_PP_FP_DATABASE_INFO     *fp_database_info,
    SOC_SAND_OUT uint32                   *action_lsb
  );

uint32
  soc_pb_pp_fp_fem_remove(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PB_PP_FP_FEM_ENTRY           *entry_ndx
  );

uint32
  soc_pb_pp_fp_fem_configuration_de_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    fem_id_ndx,
    SOC_SAND_IN  uint32                    cycle_ndx,
    SOC_SAND_OUT SOC_PB_PP_FP_FEM_ENTRY           *entry_ndx,
    SOC_SAND_OUT SOC_PB_PP_FP_DIR_EXTR_ACTION_VAL *fem_info,
    SOC_SAND_OUT SOC_PB_PP_FP_QUAL_VAL            *qual_info
  );


/*********************************************************************
* NAME:
 *   soc_pb_pp_fp_fem_insert_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Compute the best configuration to add greedily Direct
 *   Extraction entries (preference to the new
 *   Database-ID). If set, set all the FEM (selected bits,
 *   actions) and its input. Look at the previous FEM
 *   configuration to shift the FEMs if necessary. The FEM
 *   input can be changed again upon the new TCAM DB
 *   creation.
 * INPUT:
 *   SOC_SAND_IN  int                    unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_FP_FEM_ENTRY           *entry_ndx -
 *     Entry requiring a FEM.
 *   SOC_SAND_IN  SOC_PB_PP_FP_DIR_EXTR_ACTION_VAL *fem_info -
 *     Parameters of the FEM necessary to know if a place if
 *     free.
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE         *success -
 *     Indicate if the database is created successfully.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_fp_fem_insert_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PB_PP_FP_FEM_ENTRY           *entry_ndx,
    SOC_SAND_IN  SOC_PB_PP_FP_FEM_CYCLE           *fem_cycle,
    SOC_SAND_IN  SOC_PB_PP_FP_DIR_EXTR_ENTRY_INFO *fem_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE         *success
  );

uint32
  soc_pb_pp_fp_fem_insert_verify(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PB_PP_FP_FEM_ENTRY           *entry_ndx,
    SOC_SAND_IN  SOC_PB_PP_FP_FEM_CYCLE           *fem_cycle,
    SOC_SAND_IN  SOC_PB_PP_FP_DIR_EXTR_ENTRY_INFO *fem_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_fp_fem_is_place_get_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Check out if there is an empty FEM for this entry.
 * INPUT:
 *   SOC_SAND_IN  int                    unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_FP_FEM_ENTRY           *entry_ndx -
 *     Entry requiring a FEM.
 *   SOC_SAND_IN  SOC_PB_PP_FP_FEM_CYCLE            *fem_info -
 *     Parameters of the FEM necessary to know if a place if
 *     free.
 *   SOC_SAND_OUT uint8                    *place_found -
 *     If True, then a place (i.e., FEM) is found for this
 *     entry.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_fp_fem_is_place_get_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PB_PP_FP_FEM_ENTRY           *entry_ndx,
    SOC_SAND_IN  SOC_PB_PP_FP_FEM_CYCLE            *fem_info,
    SOC_SAND_OUT uint8                    *place_found
  );

uint32
  soc_pb_pp_fp_fem_is_place_get_verify(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PB_PP_FP_FEM_ENTRY           *entry_ndx,
    SOC_SAND_IN  SOC_PB_PP_FP_FEM_CYCLE            *fem_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_fp_fem_tag_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Check out if there is an empty FEM for this entry.
 * INPUT:
 *   SOC_SAND_IN  int                    unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                    db_id_ndx -
 *     Database-ID. Range: 0 - 127.
 *   SOC_SAND_IN  SOC_PB_PP_FP_ACTION_TYPE         action_type -
 *     Tag action type.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_fp_fem_tag_set_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    db_id_ndx,
    SOC_SAND_IN  SOC_PB_PP_FP_ACTION_TYPE           action_type
  );

uint32
  soc_pb_pp_fp_fem_tag_set_verify(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    db_id_ndx,
    SOC_SAND_IN  SOC_PB_PP_FP_ACTION_TYPE           action_type
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_fp_fem_get_procs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of procedures of the
 *   soc_pb_pp_fp_fem module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
SOC_PROCEDURE_DESC_ELEMENT*
  soc_pb_pp_fp_fem_get_procs_ptr(void);

/*********************************************************************
* NAME:
 *   soc_pb_pp_fp_fem_get_errs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of errors of the
 *   soc_pb_pp_fp_fem module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
SOC_ERROR_DESC_ELEMENT*
  soc_pb_pp_fp_fem_get_errs_ptr(void);

uint32
  SOC_PB_PP_FP_FEM_ENTRY_verify(
    SOC_SAND_IN  SOC_PB_PP_FP_FEM_ENTRY *info
  );

uint32
  SOC_PB_PP_FP_FEM_CYCLE_verify(
    SOC_SAND_IN  SOC_PB_PP_FP_FEM_CYCLE *info
  );

/* } */

void
  SOC_PB_PP_FP_FEM_ENTRY_clear(
    SOC_SAND_OUT SOC_PB_PP_FP_FEM_ENTRY *info
  );

void
  SOC_PB_PP_FP_FEM_CYCLE_clear(
    SOC_SAND_OUT SOC_PB_PP_FP_FEM_CYCLE *info
  );

#if SOC_PB_PP_DEBUG_IS_LVL1

void
  SOC_PB_PP_FP_FEM_ENTRY_print(
    SOC_SAND_IN  SOC_PB_PP_FP_FEM_ENTRY *info
  );

void
  SOC_PB_PP_FP_FEM_CYCLE_print(
    SOC_SAND_IN  SOC_PB_PP_FP_FEM_CYCLE *info
  );

#endif /* SOC_PB_PP_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PB_PP_FP_FEM__INCLUDED__*/
#endif

