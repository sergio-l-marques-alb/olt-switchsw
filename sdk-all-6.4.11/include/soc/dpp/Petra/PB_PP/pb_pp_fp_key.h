/* $Id: pb_pp_fp_key.h,v 1.8 Broadcom SDK $
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

#ifndef __SOC_PB_PP_FP_KEY_INCLUDED__
/* { */
#define __SOC_PB_PP_FP_KEY_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/Petra/PB_PP/pb_pp_framework.h>

#include <soc/dpp/Petra/PB_PP/pb_pp_fp.h>
#include <soc/dpp/Petra/PB_TM/pb_pmf_low_level_fem_tag.h>
#include <soc/dpp/Petra/PB_TM/pb_pmf_low_level_ce.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */



#define SOC_PB_PP_FP_KEY_NOF_FREE_SEGMENTS_IN_BUFFER_MAX 4

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
  SOC_PB_PP_FP_KEY_LENGTH_GET = SOC_PB_PP_PROC_DESC_BASE_FP_KEY_FIRST,
  SOC_PB_PP_FP_KEY_LENGTH_GET_PRINT,
  SOC_PB_PP_FP_KEY_LENGTH_GET_UNSAFE,
  SOC_PB_PP_FP_KEY_LENGTH_GET_VERIFY,
  SOC_PB_PP_FP_KEY_SPACE_ALLOC,
  SOC_PB_PP_FP_KEY_SPACE_ALLOC_PRINT,
  SOC_PB_PP_FP_KEY_SPACE_ALLOC_UNSAFE,
  SOC_PB_PP_FP_KEY_SPACE_ALLOC_VERIFY,
  SOC_PB_PP_FP_KEY_SPACE_FREE,
  SOC_PB_PP_FP_KEY_SPACE_FREE_PRINT,
  SOC_PB_PP_FP_KEY_SPACE_FREE_UNSAFE,
  SOC_PB_PP_FP_KEY_SPACE_FREE_VERIFY,
  SOC_PB_PP_FP_KEY_DESC_GET,
  SOC_PB_PP_FP_KEY_DESC_GET_PRINT,
  SOC_PB_PP_FP_KEY_DESC_GET_UNSAFE,
  SOC_PB_PP_FP_KEY_DESC_GET_VERIFY,
  SOC_PB_PP_FP_KEY_GET_PROCS_PTR,
  SOC_PB_PP_FP_KEY_GET_ERRS_PTR,
  /*
   * } Auto generated. Do not edit previous section.
   */
   SOC_PB_PP_FP_KEY_HEADER_NDX_FIND,
   SOC_PB_PP_FP_NOF_VLAN_TAGS_GET,
   SOC_PB_PP_FP_KEY_TOTAL_SIZE_GET,
   SOC_PB_PP_FP_KEY_CE_INSTR_BOUNDS_GET,
   SOC_PB_PP_FP_KEY_CE_INSTR_INSTALL,
   SOC_PB_PP_FP_KEY_NEW_DB_POSSIBLE_CONFS_GET_UNSAFE,



  /*
   * Last element. Do no touch.
   */
  SOC_PB_PP_FP_KEY_PROCEDURE_DESC_LAST
} SOC_PB_PP_FP_KEY_PROCEDURE_DESC;

typedef enum
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PB_PP_FP_KEY_PFG_OUT_OF_RANGE_ERR = SOC_PB_PP_ERR_DESC_BASE_FP_KEY_FIRST,
  SOC_PB_PP_FP_KEY_DB_ID_NDX_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FP_KEY_CYCLE_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FP_KEY_LSB_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FP_KEY_LENGTH_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FP_KEY_KEY_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FP_KEY_UNKNOWN_QUAL_ERR,
  SOC_PB_PP_FP_KEY_BAD_PADDING_ERR,
  SOC_PB_PP_FP_KEY_COPY_OVERFLOW_ERR,
  SOC_PB_PP_FP_KEY_TOO_MANY_BITS_ERR,
  /*
   * } Auto generated. Do not edit previous section.
   */



  /*
   * Last element. Do no touch.
   */
  SOC_PB_PP_FP_KEY_ERR_LAST
} SOC_PB_PP_FP_KEY_ERR;

typedef enum
{
  /*
   *  Key A in the PMF.
   */
  SOC_PB_PP_FP_PMF_KEY_A = 0,
  /*
   *  Key B in the PMF.
   */
  SOC_PB_PP_FP_PMF_KEY_B = 1,
  /*
   *  Number of types in SOC_PB_PP_FP_PMF_KEY
   */
  SOC_PB_PP_NOF_FP_PMF_KEYS = 2
}SOC_PB_PP_FP_PMF_KEY;

typedef enum
{
  /*
   *  The key will be fed to a 72b-wide TCAM bank.
   */
  SOC_PB_PP_FP_KEY_CONS_TCAM_72B = 0,
  /*
   *  The key will be fed to a 144b-wide TCAM bank.
   */
  SOC_PB_PP_FP_KEY_CONS_TCAM_144B = 1,
  /*
   *  The key will be fed to a 288b-wide TCAM bank.
   */
  SOC_PB_PP_FP_KEY_CONS_TCAM_288B = 2,
  /*
   *  The key will be fed to the direct table.
   */
  SOC_PB_PP_FP_KEY_CONS_DIR_TBL = 3,
  /*
   *  The key will be fed to a FEM.
   */
  SOC_PB_PP_FP_KEY_CONS_FEM = 4,
  /*
   *  The key will be fed to a TAG.
   */
  SOC_PB_PP_FP_KEY_CONS_TAG = 5,
  /*
   *  Number of types in SOC_PB_PP_FP_KEY_CONSTRAINT
   */
  SOC_PB_PP_NOF_FP_KEY_CONSTRAINTS = 6
}SOC_PB_PP_FP_KEY_CONSTRAINT;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  The location of the LSB of the field, relative to the
   *  LSB of the containing key.
   */
  uint32 lsb;
  /*
   *  The number of bits in the field's encoding.
   */
  uint32 length;

} SOC_PB_PP_FP_KEY_FLD_DESC;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  The location of the LSB of the field, relative to the
   *  LSB of the containing key.
   */
  SOC_PB_PP_FP_KEY_FLD_DESC fields[SOC_PB_PP_FP_NOF_QUALS_PER_DB_MAX];

} SOC_PB_PP_FP_KEY_DESC;

typedef union
{
  SOC_PB_PMF_FEM_INPUT_SRC fem_src;

  SOC_PB_PMF_DIRECT_TBL_KEY_SRC dir_tbl_src;

  SOC_PB_PMF_TCAM_KEY_SRC tcam_src;

  SOC_PB_PMF_TAG_VAL_SRC tag_src;

} SOC_PB_PP_FP_KEY_LOCATION;

typedef struct
{
  /*
   *  The appropriate key (A or B).
   */
  uint32 key;
  /*
   *  The first free instruction (counted from 7 to 0).
   */
  uint32 start_instr;
  /*
   *  The last free instruction (counted from 7 to 0).
   */
  uint32 end_instr;
  /*
   *  The LSB of the segment.
   */
  uint32 lsb;
  /*
   *  The MSB of the segment.
   */
  uint32 msb;
  /*
   *  Indicates whether the overall segment length must remain the same (in order to keep the
   *  qualifiers of other databases intact) or not (i.e. the segment is the last one in the buffer).
   */
  uint8 const_length;

} SOC_PB_PP_FP_KEY_SEGMENT;

typedef enum
{
  SOC_PB_PP_FP_KEY_SRC_PKT_HDR,
  SOC_PB_PP_FP_KEY_SRC_IRPP
} SOC_PB_PP_FP_KEY_SRC;

typedef struct
{
  SOC_PB_PP_FP_QUAL_TYPE   qual_type;
  SOC_PB_PP_FP_KEY_SRC     data_src;
  uint32            msb;
  uint32            lsb;
  SOC_PB_PMF_CE_SUB_HEADER header_ndx_0; /* Possible header indices */
  SOC_PB_PMF_CE_SUB_HEADER header_ndx_1;
} SOC_PB_PP_FP_KEY_QUAL_INFO;

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

/*********************************************************************
* NAME:
 *   soc_pb_pp_fp_key_qual_info_find
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Returns the key_qual_info associated with a certain qualifier type
 * INPUT:
 *   SOC_SAND_IN  SOC_PB_PP_FP_QUAL_TYPE     qual_type -
 *     The qualifier type in question.
 *   SOC_SAND_OUT SOC_PB_PP_FP_KEY_QUAL_INFO *qual_info -
 *     The qualifier info.
 * REMARKS:
 *   None.
 * RETURNS:
 *   Found indication.
*********************************************************************/
uint8
  soc_pb_pp_fp_key_qual_info_find(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  SOC_PB_PP_FP_QUAL_TYPE     qual_type,
    SOC_SAND_OUT SOC_PB_PP_FP_KEY_QUAL_INFO *qual_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_fp_key_length_get_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Returns the number of bits required to hold a qualifier
 *   of type qual_type. If with_padding is set then the
 *   returned value is the actual number of bits required
 *   (with extra padding due to hardware limitations),
 *   otherwise the returned value is the exact number of bits
 *   required to store the qualifier.
 * INPUT:
 *   SOC_SAND_IN  SOC_PB_PP_FP_QUAL_TYPE     qual_type -
 *     The qualifier type in question.
 *   SOC_SAND_IN  uint8              with_padding -
 *     Determines whether the return value regards padding bits
 *     or not.
 *   SOC_SAND_OUT uint32              *length -
 *     The return value.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_fp_key_length_get_unsafe(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  SOC_PB_PP_FP_QUAL_TYPE     qual_type,
    SOC_SAND_IN  uint8              with_padding,
    SOC_SAND_OUT uint32              *length
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_fp_key_space_alloc_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Allocates a place in one of the PMF custom keys for the
 *   qualifiers of a field processor database, and configures
 *   the relevant copy engines (in the software database) to
 *   copy the qualifiers to that position.
 * INPUT:
 *   SOC_SAND_IN  int               unit -
 *     ID of the relevant device.
 *   SOC_SAND_IN  uint32               db_id_ndx -
 *     ID of the new database.
 *   SOC_SAND_IN  SOC_PB_PP_FP_DATABASE_INFO  *db_info -
 *     The database parameters.
 *   SOC_SAND_IN  uint32               cycle -
 *     The cycle in which the actions are to be resolved.
 *   SOC_SAND_IN  SOC_PB_PP_FP_KEY_CONSTRAINT key_cons -
 *     Constraints that should be considered when allocating
 *     space to the key.
 *   SOC_SAND_IN  SOC_PB_PP_FP_KEY_CALLBACK   key_callback -
 *     A callback function that will be called in case the
 *     location of the key in the hardware changes.
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE    *success -
 *     Success or failure indication.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_fp_key_space_alloc_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               pfg_ndx,
    SOC_SAND_IN  uint32               db_id_ndx,
    SOC_SAND_IN  SOC_PB_PP_FP_DATABASE_INFO  *db_info,
    SOC_SAND_IN  uint32               cycle,
    SOC_SAND_IN  SOC_PB_PP_FP_KEY_CONSTRAINT key_cons,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE    *success
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_fp_key_space_free_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Frees the PMF key space allocated to a database.
 * INPUT:
 *   SOC_SAND_IN  int              unit -
 *     ID of the relevant device.
 *   SOC_SAND_IN  uint32              db_id_ndx -
 *     The database whose resources should be deallocated.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_fp_key_space_free_unsafe(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  uint32              db_id_ndx
  );

uint32
  soc_pb_pp_fp_qual_length_get(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  SOC_PB_PP_FP_DATABASE_STAGE    stage,
    SOC_SAND_IN  SOC_PB_PP_FP_QUAL_TYPE         qual_type,
    SOC_SAND_OUT uint32                     *found,
    SOC_SAND_OUT uint32                     *length_padded,
    SOC_SAND_OUT uint32                     *length_logical
  );


/*********************************************************************
* NAME:
 *   soc_pb_pp_fp_key_desc_get_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Returns the description of a database's key structure.
 * INPUT:
 *   SOC_SAND_IN  int              unit -
 *     ID of the relevant device.
 *   SOC_SAND_IN  uint32              db_id_ndx -
 *     The database whose resources should be deallocated.
 *   SOC_SAND_OUT SOC_PB_PP_FP_KEY_DESC      *key_desc -
 *     Contains a description of the allocated key bits, to
 *     enable automatic generation of appropriate keys for this
 *     database.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_fp_key_desc_get_unsafe(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  uint32              db_id_ndx,
    SOC_SAND_OUT SOC_PB_PP_FP_KEY_DESC      *key_desc
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_fp_key_get_procs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of procedures of the
 *   soc_pb_pp_api_fp_key module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
SOC_PROCEDURE_DESC_ELEMENT*
  soc_pb_pp_fp_key_get_procs_ptr(void);

/*********************************************************************
* NAME:
 *   soc_pb_pp_fp_key_get_errs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of errors of the
 *   soc_pb_pp_api_fp_key module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
SOC_ERROR_DESC_ELEMENT*
  soc_pb_pp_fp_key_get_errs_ptr(void);

void
  SOC_PB_PP_FP_KEY_FLD_DESC_clear(
    SOC_SAND_OUT SOC_PB_PP_FP_KEY_FLD_DESC *info
  );

void
  SOC_PB_PP_FP_KEY_DESC_clear(
    SOC_SAND_OUT SOC_PB_PP_FP_KEY_DESC *info
  );

void
  SOC_PB_PP_FP_KEY_LOCATION_clear(
    SOC_SAND_OUT SOC_PB_PP_FP_KEY_LOCATION *info
  );

#if SOC_PB_PP_DEBUG_IS_LVL1

const char*
  SOC_PB_PP_FP_PMF_KEY_to_string(
    SOC_SAND_IN  SOC_PB_PP_FP_PMF_KEY enum_val
  );

const char*
  SOC_PB_PP_FP_KEY_CONSTRAINT_to_string(
    SOC_SAND_IN  SOC_PB_PP_FP_KEY_CONSTRAINT enum_val
  );

void
  SOC_PB_PP_FP_KEY_FLD_DESC_print(
    SOC_SAND_IN  SOC_PB_PP_FP_KEY_FLD_DESC *info
  );

void
  SOC_PB_PP_FP_KEY_DESC_print(
    SOC_SAND_IN  SOC_PB_PP_FP_KEY_DESC *info
  );

void
  SOC_PB_PP_FP_KEY_LOCATION_print(
    SOC_SAND_IN  SOC_PB_PP_FP_KEY_LOCATION *info
  );

#endif /* SOC_PB_PP_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PB_PP_FP_KEY_INCLUDED__*/
#endif
