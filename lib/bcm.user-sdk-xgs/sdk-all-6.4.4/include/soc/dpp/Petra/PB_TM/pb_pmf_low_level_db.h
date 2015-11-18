/* $Id: pb_pmf_low_level_db.h,v 1.6 Broadcom SDK $
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
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_TM/include/soc_pb_pmf_low_level.h
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

#ifndef __SOC_PB_PMF_LOW_LEVEL_DB_INCLUDED__
/* { */
#define __SOC_PB_PMF_LOW_LEVEL_DB_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/Petra/PB_TM/pb_pmf_low_level.h>
#include <soc/dpp/Petra/PB_TM/pb_api_framework.h>
#include <soc/dpp/Petra/PB_TM/pb_tcam.h>
#include <soc/dpp/Petra/PB_TM/pb_tcam_mgmt.h>
#include <soc/dpp/Petra/PB_TM/pb_tcam_key.h>

#include <soc/dpp/TMC/tmc_api_pmf_low_level_db.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PB_PMF_DIRECT_TBL_KEY_SRC_A_9_0                 SOC_TMC_PMF_DIRECT_TBL_KEY_SRC_A_9_0
#define SOC_PB_PMF_DIRECT_TBL_KEY_SRC_A_19_10               SOC_TMC_PMF_DIRECT_TBL_KEY_SRC_A_19_10
#define SOC_PB_PMF_DIRECT_TBL_KEY_SRC_A_29_20               SOC_TMC_PMF_DIRECT_TBL_KEY_SRC_A_29_20
#define SOC_PB_PMF_DIRECT_TBL_KEY_SRC_A_39_30               SOC_TMC_PMF_DIRECT_TBL_KEY_SRC_A_39_30
#define SOC_PB_PMF_DIRECT_TBL_KEY_SRC_B_9_0                 SOC_TMC_PMF_DIRECT_TBL_KEY_SRC_B_9_0
#define SOC_PB_PMF_DIRECT_TBL_KEY_SRC_B_19_10               SOC_TMC_PMF_DIRECT_TBL_KEY_SRC_B_19_10
#define SOC_PB_PMF_DIRECT_TBL_KEY_SRC_B_29_20               SOC_TMC_PMF_DIRECT_TBL_KEY_SRC_B_29_20
#define SOC_PB_PMF_DIRECT_TBL_KEY_SRC_B_39_30               SOC_TMC_PMF_DIRECT_TBL_KEY_SRC_B_39_30
#define SOC_PB_NOF_PMF_DIRECT_TBL_KEY_SRCS                  SOC_TMC_NOF_PMF_DIRECT_TBL_KEY_SRCS
typedef SOC_TMC_PMF_DIRECT_TBL_KEY_SRC                         SOC_PB_PMF_DIRECT_TBL_KEY_SRC;



typedef SOC_TMC_PMF_TCAM_ENTRY_ID                              SOC_PB_PMF_TCAM_ENTRY_ID;
typedef SOC_TMC_PMF_TCAM_DATA                                  SOC_PB_PMF_TCAM_DATA;
typedef SOC_TMC_PMF_LKP_PROFILE                                SOC_PB_PMF_LKP_PROFILE;
typedef SOC_TMC_PMF_TCAM_KEY_SELECTION_INFO                    SOC_PB_PMF_TCAM_KEY_SELECTION_INFO;
typedef SOC_TMC_PMF_TCAM_BANK_SELECTION                        SOC_PB_PMF_TCAM_BANK_SELECTION;
typedef SOC_TMC_PMF_TCAM_RESULT_INFO                           SOC_PB_PMF_TCAM_RESULT_INFO;
typedef SOC_TMC_PMF_DIRECT_TBL_DATA                            SOC_PB_PMF_DIRECT_TBL_DATA;


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
  soc_pb_pmf_low_level_db_init_unsafe(
    SOC_SAND_IN  int                                 unit
  );

uint32
  soc_pb_pmf_low_level_tcam_bank_enable_set_unsafe(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 bank_id,
    SOC_SAND_IN uint32 cycle,
    SOC_SAND_IN uint8 enabled
  );

uint32
  soc_pb_pmf_tcam_lookup_db_add_unsafe(
    SOC_SAND_IN int             unit,
    SOC_SAND_IN SOC_PB_PMF_LKP_PROFILE    *lkp_profile_ndx,
    SOC_SAND_IN uint32             tcam_db_id,
    SOC_SAND_IN SOC_PB_PMF_TCAM_KEY_SRC   key_src
  );

uint32
  soc_pb_pmf_tcam_lookup_result_config_unsafe(
    SOC_SAND_IN int             unit,
    SOC_SAND_IN SOC_PB_PMF_LKP_PROFILE    *lkp_profile_ndx,
    SOC_SAND_IN uint32             tcam_db_id,
    SOC_SAND_IN SOC_PB_TCAM_ACCESS_DEVICE access_device
  );

uint32
  soc_pb_pmf_tcam_lookup_enable_set_unsafe(
    SOC_SAND_IN int            unit,
    SOC_SAND_IN uint32            tcam_db_id,
    SOC_SAND_IN SOC_PB_TCAM_ACCESS_CYCLE cycle,
    SOC_SAND_IN uint8            enabled
  );

/*********************************************************************
* NAME:
 *   soc_pb_pmf_tcam_lookup_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Select the key to be searched in the TCAM.
 * INPUT:
 *   SOC_SAND_IN  int                            unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PMF_LKP_PROFILE                *lkp_profile_ndx -
 *     Lookup-Profile information (id and cycle).
 *   SOC_SAND_IN  uint32                            db_id_ndx -
 *     Database-ID. Different Lookup-Profiles can share the
 *     same Database. A Lookup-Profile can refer to several
 *     Databases if there are sufficient
 *     Priority-Decoders/Selectors to get all the TCAM output
 *     result. Range: 0 - 16K-1.
 *   SOC_SAND_IN  SOC_PB_PMF_TCAM_KEY_SELECTION_INFO    *info -
 *     TCAM Key Selection Information: the TCAM Key source and
 *     the id of the TCAM Key Database
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                 *success -
 *     Indicate if the database insertion into the lookup
 *     process has succeeded.
 * REMARKS:
 *   The Database-ID is set with the soc_ppd_tcam_db_create API
 *   with a PMF usage.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pmf_tcam_lookup_set_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PB_PMF_LKP_PROFILE                *lkp_profile_ndx,
    SOC_SAND_IN  uint32                            tcam_db_id,
    SOC_SAND_IN  SOC_PB_PMF_TCAM_KEY_SELECTION_INFO    *info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                 *success
  );

uint32
  soc_pb_pmf_tcam_lookup_set_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PB_PMF_LKP_PROFILE                *lkp_profile_ndx,
    SOC_SAND_IN  uint32                            db_id_ndx,
    SOC_SAND_IN  SOC_PB_PMF_TCAM_KEY_SELECTION_INFO    *info
  );

uint32
  soc_pb_pmf_tcam_lookup_get_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PB_PMF_LKP_PROFILE                *lkp_profile_ndx,
    SOC_SAND_IN  uint32                            db_id_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pmf_tcam_lookup_set_unsafe" API.
 *     Refer to "soc_pb_pmf_tcam_lookup_set_unsafe" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_pmf_tcam_lookup_get_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PB_PMF_LKP_PROFILE                *lkp_profile_ndx,
    SOC_SAND_IN  uint32                            db_id_ndx,
    SOC_SAND_OUT SOC_PB_PMF_TCAM_KEY_SELECTION_INFO    *info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pmf_tcam_entry_add_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Add an entry (in the format of a PMF TCAM Key) in a TCAM
 *   Bank.
 * INPUT:
 *   SOC_SAND_IN  int                            unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PMF_TCAM_ENTRY_ID              *entry_ndx -
 *     TCAM Entry-ID.
 *   SOC_SAND_IN  SOC_PB_PMF_TCAM_DATA                  *tcam_data -
 *     TCAM Data: entry priority, key with raw bitmap
 *     presentation and TCAM output value. Helper functions are
 *     provided in the PMF TCAM module to build the key per
 *     field and the action (TCAM output) according to a
 *     template.
 *   SOC_SAND_IN  uint8                            allow_new_bank -
 *     If True, then the entry is added to the Database even if
 *     the Database must use another TCAM (Bank, Cycle) than it
 *     already uses. Otherwise, the entry is added only if the
 *     Database has already entries in this TCAM (Bank, Cycle).
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                 *success -
 *     Indicate the entry insertion into the Database has
 *     succeeded.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pmf_tcam_entry_add_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PB_PMF_TCAM_ENTRY_ID              *entry_ndx,
    SOC_SAND_IN  SOC_PB_PMF_TCAM_DATA                  *tcam_data,
    SOC_SAND_IN  uint8                            allow_new_bank,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                 *success
  );

uint32
  soc_pb_pmf_tcam_entry_add_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PB_PMF_TCAM_ENTRY_ID              *entry_ndx,
    SOC_SAND_IN  SOC_PB_PMF_TCAM_DATA                  *tcam_data,
    SOC_SAND_IN  uint8                            allow_new_bank
  );

/*********************************************************************
* NAME:
 *   soc_pb_pmf_tcam_entry_get_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get an entry (in the format of a PMF TCAM Key) in a TCAM
 *   Bank.
 * INPUT:
 *   SOC_SAND_IN  int                            unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PMF_TCAM_ENTRY_ID              *entry_ndx -
 *     TCAM Entry-ID.
 *   SOC_SAND_OUT SOC_PB_PMF_TCAM_DATA                  *tcam_data -
 *     TCAM Data: entry priority, key with raw bitmap
 *     presentation and TCAM output value.
 *   SOC_SAND_OUT uint8                            *is_found -
 *     If True, then the entry is found in the Database.
 *     Otherwise, no such entry is present in this Database.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pmf_tcam_entry_get_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PB_PMF_TCAM_ENTRY_ID              *entry_ndx,
    SOC_SAND_OUT SOC_PB_PMF_TCAM_DATA                  *tcam_data,
    SOC_SAND_OUT uint8                            *is_found
  );

uint32
  soc_pb_pmf_tcam_entry_get_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PB_PMF_TCAM_ENTRY_ID              *entry_ndx
  );

/*********************************************************************
* NAME:
 *   soc_pb_pmf_tcam_entry_remove_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Remove an entry (in the format of a PMF TCAM Key) from a
 *   TCAM.
 * INPUT:
 *   SOC_SAND_IN  int                            unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PMF_TCAM_ENTRY_ID              *entry_ndx -
 *     TCAM Entry-ID.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pmf_tcam_entry_remove_unsafe(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PB_PMF_TCAM_ENTRY_ID              *entry_ndx,
    SOC_SAND_OUT uint8                         *is_found
  );

uint32
  soc_pb_pmf_tcam_entry_remove_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PB_PMF_TCAM_ENTRY_ID              *entry_ndx
  );


/*********************************************************************
* NAME:
 *   soc_pb_pmf_db_direct_tbl_key_src_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Select the key source for the direct table.
 * INPUT:
 *   SOC_SAND_IN  int                            unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PMF_LKP_PROFILE                *lkp_profile_ndx -
 *     Lookup-Profile information (id and cycle).
 *   SOC_SAND_IN  SOC_PB_PMF_DIRECT_TBL_KEY_SRC         key_src -
 *     Entry index source for the direct table
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pmf_db_direct_tbl_key_src_set_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PB_PMF_LKP_PROFILE                *lkp_profile_ndx,
    SOC_SAND_IN  SOC_PB_PMF_DIRECT_TBL_KEY_SRC         key_src
  );

uint32
  soc_pb_pmf_db_direct_tbl_key_src_set_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PB_PMF_LKP_PROFILE                *lkp_profile_ndx,
    SOC_SAND_IN  SOC_PB_PMF_DIRECT_TBL_KEY_SRC         key_src
  );

uint32
  soc_pb_pmf_db_direct_tbl_key_src_get_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PB_PMF_LKP_PROFILE                *lkp_profile_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pmf_db_direct_tbl_key_src_set_unsafe" API.
 *     Refer to "soc_pb_pmf_db_direct_tbl_key_src_set_unsafe"
 *     API for details.
*********************************************************************/
uint32
  soc_pb_pmf_db_direct_tbl_key_src_get_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PB_PMF_LKP_PROFILE                *lkp_profile_ndx,
    SOC_SAND_OUT SOC_PB_PMF_DIRECT_TBL_KEY_SRC         *key_src
  );

/*********************************************************************
* NAME:
 *   soc_pb_pmf_db_direct_tbl_entry_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set an entry to the Database direct table.
 * INPUT:
 *   SOC_SAND_IN  int                            unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                            entry_ndx -
 *     Entry index in the Direct table. Range: 0 - 1023.
 *     (Soc_petra-B)
 *   SOC_SAND_IN  SOC_PB_PMF_DIRECT_TBL_DATA            *data -
 *     Direct table output value. For Soc_petra-B, it is encoded in
 *     20 bits. Helper functions are provided in the PMF
 *     Database module to build this output per template.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pmf_db_direct_tbl_entry_set_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  uint32                            entry_ndx,
    SOC_SAND_IN  SOC_PB_PMF_DIRECT_TBL_DATA            *data
  );

uint32
  soc_pb_pmf_db_direct_tbl_entry_set_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  uint32                            entry_ndx,
    SOC_SAND_IN  SOC_PB_PMF_DIRECT_TBL_DATA            *data
  );

uint32
  soc_pb_pmf_db_direct_tbl_entry_get_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  uint32                            entry_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pmf_db_direct_tbl_entry_set_unsafe" API.
 *     Refer to "soc_pb_pmf_db_direct_tbl_entry_set_unsafe" API
 *     for details.
*********************************************************************/
uint32
  soc_pb_pmf_db_direct_tbl_entry_get_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  uint32                            entry_ndx,
    SOC_SAND_OUT SOC_PB_PMF_DIRECT_TBL_DATA            *data
  );


uint32
  SOC_PB_PMF_TCAM_ENTRY_ID_verify(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  SOC_PB_PMF_TCAM_ENTRY_ID *info
  );

uint32
  SOC_PB_PMF_LKP_PROFILE_verify(
    SOC_SAND_IN  SOC_PB_PMF_LKP_PROFILE *info
  );

uint32
  SOC_PB_PMF_TCAM_KEY_SELECTION_INFO_verify(
    SOC_SAND_IN  SOC_PB_PMF_TCAM_KEY_SELECTION_INFO *info
  );
uint32
  SOC_PB_PMF_TCAM_BANK_SELECTION_verify(
    SOC_SAND_IN  SOC_PB_PMF_TCAM_BANK_SELECTION *info
  );

uint32
  SOC_PB_PMF_TCAM_RESULT_INFO_verify(
    SOC_SAND_IN  SOC_PB_PMF_TCAM_RESULT_INFO *info
  );


uint32
  SOC_PB_PMF_TCAM_DATA_verify(
    SOC_SAND_IN  SOC_PB_PMF_TCAM_DATA *info
  );

uint32
  SOC_PB_PMF_DIRECT_TBL_DATA_verify(
    SOC_SAND_IN  SOC_PB_PMF_DIRECT_TBL_DATA *info
  );

void
  SOC_PB_PMF_TCAM_ENTRY_ID_clear(
    SOC_SAND_OUT SOC_PB_PMF_TCAM_ENTRY_ID *info
  );

void
  SOC_PB_PMF_LKP_PROFILE_clear(
    SOC_SAND_OUT SOC_PB_PMF_LKP_PROFILE *info
  );

void
  SOC_PB_PMF_TCAM_KEY_SELECTION_INFO_clear(
    SOC_SAND_OUT SOC_PB_PMF_TCAM_KEY_SELECTION_INFO *info
  );

void
  SOC_PB_PMF_TCAM_BANK_SELECTION_clear(
    SOC_SAND_OUT SOC_PB_PMF_TCAM_BANK_SELECTION *info
  );

void
  SOC_PB_PMF_TCAM_RESULT_INFO_clear(
    SOC_SAND_OUT SOC_PB_PMF_TCAM_RESULT_INFO *info
  );

void
  SOC_PB_PMF_TCAM_DATA_clear(
    SOC_SAND_OUT SOC_PB_PMF_TCAM_DATA *info
  );

void
  SOC_PB_PMF_DIRECT_TBL_DATA_clear(
    SOC_SAND_OUT SOC_PB_PMF_DIRECT_TBL_DATA *info
  );

#if SOC_PB_DEBUG_IS_LVL1
const char*
  SOC_PB_PMF_DIRECT_TBL_KEY_SRC_to_string(
    SOC_SAND_IN  SOC_PB_PMF_DIRECT_TBL_KEY_SRC enum_val
  );

void
  SOC_PB_PMF_TCAM_ENTRY_ID_print(
    SOC_SAND_IN  SOC_PB_PMF_TCAM_ENTRY_ID *info
  );

void
  SOC_PB_PMF_LKP_PROFILE_print(
    SOC_SAND_IN  SOC_PB_PMF_LKP_PROFILE *info
  );

void
  SOC_PB_PMF_TCAM_KEY_SELECTION_INFO_print(
    SOC_SAND_IN  SOC_PB_PMF_TCAM_KEY_SELECTION_INFO *info
  );

void
  SOC_PB_PMF_TCAM_BANK_SELECTION_print(
    SOC_SAND_IN  SOC_PB_PMF_TCAM_BANK_SELECTION *info
  );

void
  SOC_PB_PMF_TCAM_RESULT_INFO_print(
    SOC_SAND_IN  SOC_PB_PMF_TCAM_RESULT_INFO *info
  );

void
  SOC_PB_PMF_TCAM_DATA_print(
    SOC_SAND_IN  SOC_PB_PMF_TCAM_DATA *info
  );

void
  SOC_PB_PMF_DIRECT_TBL_DATA_print(
    SOC_SAND_IN  SOC_PB_PMF_DIRECT_TBL_DATA *info
  );

#endif /* SOC_PB_DEBUG_IS_LVL1 */


/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PB_PMF_LOW_LEVEL_DB_INCLUDED__*/
#endif

