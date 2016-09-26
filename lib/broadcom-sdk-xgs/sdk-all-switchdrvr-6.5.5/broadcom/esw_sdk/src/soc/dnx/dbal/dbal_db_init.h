
/*! \file dbal_db_init.h
 *
 * Main functions for init the dbal fields and logical tables DB
 *
 */
/*
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

#ifndef _DBAL_DB_INIT_INCLUDED__
#  define _DBAL_DB_INIT_INCLUDED__

/*************
 * INCLUDES   *
 *************/
#  include <soc/dnx/dbal_structures.h>
#  include <shared/dbx/dbx_xml.h>
#  include <shared/dbx/dbx_file.h>

/*************
 *  DEFINES  *
 *************/
#  define DBAL_INIT_FLAGS_NONE        0x0
#  define DBAL_INIT_FLAGS_VALIDATION  0x1

/*************
 * TYPEDEFS  *
 *************/
/*field DB - read from xml */
typedef struct
  {
    char name[DBAL_MAX_STRING_LENGTH];
    int size;
    char type[DBAL_MAX_STRING_LENGTH];
    char parent[DBAL_MAX_STRING_LENGTH];
    char labels[DBAL_MAX_STRING_LENGTH];
    int default_val;
    uint8 default_val_valid;
    char encode_type[DBAL_MAX_STRING_LENGTH];
    int encode_param1;
    int encode_param2;
  } field_db_struct_t;

/*logical table DB (per field DB)- read from xml */
typedef struct
  {
    char name[DBAL_MAX_STRING_LENGTH];
    int size;
    int offset;
  } table_db_field_params_struct_t;

/*logical table DB (access layer DB)- read from xml */
typedef struct
  {
    uint8 access_is_memory;
    char access_field_name[DBAL_MAX_STRING_LENGTH];
    char access_name[DBAL_MAX_STRING_LENGTH];
    int access_size;
    int access_offset;
    char access_condition_type[DBAL_MAX_STRING_LENGTH];
    int access_condition_value;
    char array_offset_type[DBAL_MAX_STRING_LENGTH];
    char array_offset_field[DBAL_MAX_STRING_LENGTH];
    int array_offset_value;
    char entry_offset_type[DBAL_MAX_STRING_LENGTH];
    char entry_offset_field[DBAL_MAX_STRING_LENGTH];
    int entry_offset_value;
    char data_offset_type[DBAL_MAX_STRING_LENGTH];
    char data_offset_field[DBAL_MAX_STRING_LENGTH];
    int data_offset_value;
    char hw_field[DBAL_MAX_STRING_LENGTH];
  } table_db_access_params_struct_t;

/*logical table DB - read from xml */
typedef struct
  {
    char name[DBAL_MAX_STRING_LENGTH];
    char type[DBAL_MAX_STRING_LENGTH];
    int nof_key_fields;
    table_db_field_params_struct_t key_fields[DBAL_TABLE_MAX_NUM_OF_KEY_FIELDS];
    int nof_res_fields;
    table_db_field_params_struct_t result_fields[DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS];
    char labels[DBAL_MAX_STRING_LENGTH];
    char core_mode[DBAL_MAX_STRING_LENGTH];
    /*
     * mdb parametes
     */
    int app_db_id;
    char phy_db[DBAL_MAX_STRING_LENGTH];
    /*
     * hard logic parametes
     */
    int nof_access;
    table_db_access_params_struct_t access[DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS];
  } table_db_struct_t;

/*************
 * FUNCTIONS *
 *************/
/*! ****************************************************
* \brief
* The function initialize dbal component with default values
*
*  \par DIRECT INPUT:
*    \param [in] unit
*    \param [in] field_info - \n
*      dbal tables info DB
*  \par DIRECT OUTPUT:
*    shr_error_e - \n
*      Error code
*****************************************************/
shr_error_e dbal_db_init_fields_set_default(
  int unit,
  dbal_field_basic_info_t * field_info);
/*! ****************************************************
* \brief
* The function initialize dbal component with default values
*
*  \par DIRECT INPUT:
*    \param [in] unit
*    \param [in] table_info - \n
*      dbal tables info DB
*  \par DIRECT OUTPUT:
*    shr_error_e - \n
*      Error code
*****************************************************/
shr_error_e dbal_db_init_logical_tables_set_default(
  int unit,
  dbal_logical_table_t * table_info);
/*! ****************************************************
* \brief
* The function fill the field DB from xml 
*
*  \par DIRECT INPUT:
*    \param [in] unit
*    \param [in] flags
*    \param [in] field_info - \n
*      dbal tables info DB
*  \par DIRECT OUTPUT:
*    shr_error_e - \n
*      Error code
*****************************************************/
shr_error_e dbal_db_init_fields(
  int unit,
  int flags,
  dbal_field_basic_info_t * field_info);

/*! ****************************************************
* \brief
* The function fill the tables DB from xml 
*
*  \par DIRECT INPUT:
*    \param [in] unit
*    \param [in] flags
*    \param [in] table_info - \n
*      dbal tables info DB
*  \par DIRECT OUTPUT:
*    shr_error_e - \n
*      Error code
*****************************************************/
shr_error_e dbal_db_init_hard_logic_logical_tables(
  int unit,
  int flags,
  dbal_logical_table_t * table_info);

/*! ****************************************************
* \brief
* The function fill the tables DB from xml 
*
*  \par DIRECT INPUT:
*    \param [in] unit
*    \param [in] flags
*    \param [in] table_info - \n
*      dbal tables info DB
*  \par DIRECT OUTPUT:
*    shr_error_e - \n
*      Error code
*****************************************************/
shr_error_e dbal_db_init_mdb_logical_tables(
  int unit,
  int flags,
  dbal_logical_table_t * table_info);
#endif /*_DBAL_DB_INIT_INCLUDED__*/
