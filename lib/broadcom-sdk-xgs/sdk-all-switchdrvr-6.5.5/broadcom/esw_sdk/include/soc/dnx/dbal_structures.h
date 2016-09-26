/*
 * $Id: dpp_dbal.c,v 1.13 Broadcom SDK $
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
 *
 */

#ifndef _DBAL_STRUCTURES_INCLUDED__
#  define _DBAL_STRUCTURES_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#  include <soc/dnx/dbal_defines.h>
#  include <soc/mem.h>
#  include <soc/drv.h>
#  include <shared/shrextend/shrextend_debug.h>

/* DEFINES */
#define DBAL_TABLE_MAX_NUM_OF_DIRECT_ACCESSES   3
#  define DBAL_FIELD_ARRAY_MAX_SIZE               32    /* the max size of field from type array */
#  define DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS     15
#  define DBAL_TABLE_MAX_NUM_OF_KEY_FIELDS        5
#  define DBAL_MAX_STRING_LENGTH                  64
#  define DBAL_MAX_NUMBER_OF_REGISTERS            10
#  define DBAL_SW_NOF_ENTRY_HANDLES               10
#  define DBAL_PHYSICAL_TABLE_SHADOW_SIZE         100

typedef enum
  {
    DBAL_FIELD_TYPE_NONE,
    DBAL_FIELD_TYPE_BOOL,
    DBAL_FIELD_TYPE_INT32,
    DBAL_FIELD_TYPE_UINT32,
    DBAL_FIELD_TYPE_IP,         /*print method is different from IP to uint32 */
    DBAL_FIELD_TYPE_ARRAY8,
    DBAL_FIELD_TYPE_ARRAY32,
    DBAL_FIELD_TYPE_BITMAP,

    DBAL_NOF_FIELD_TYPES
  } dbal_field_type_e;

typedef enum
  {
    DBAL_FIELD_MODE_IS_KEY,
    DBAL_FIELD_MODE_IS_VALUE,

    DBAL_NOF_FIELD_MODES
  } dbal_field_key_mode_e;

typedef enum
  {
    DBAL_WORK_MODE_NORMAL = 0,
    DBAL_WORK_MODE_SW_ONLY = SAL_BIT(1),

    DBAL_NUM_OF_WORK_MODES
  } dbal_work_modes_e;

typedef enum
  {
    DBAL_ENTRY_HANDLE_STATUS_AVAILABLE,
    DBAL_ENTRY_HANDLE_STATUS_IN_USE,

    DBAL_NOF_ENTRY_HANDLE_STATUSES
  } dbal_entry_handle_status_e;

/* for each encoding type we need to write a function that will be used */
typedef enum
  {
    DBAL_VALUE_ENCODE_NONE,     /*no encoding, value will stay the same */
    DBAL_VALUE_ENCODE_BOOL,     /* if exist a value set te field to "1" */
    DBAL_VALUE_ENCODE_MODULO,
    DBAL_VALUE_ENCODE_DIVIDE,
    DBAL_VALUE_ENCODE_MULTIPLE,
    DBAL_VALUE_ENCODE_PREFIX,
    DBAL_VALUE_ENCODE_SUFFIX,
    DBAL_VALUE_ENCODE_PARENT_FIELD,
    DBAL_VALUE_ENCODE_SUBTRACT,
    DBAL_VALUE_ENCODE_PARTIAL_KEY,
    DBAL_VALUE_ENCODE_HARD_VALUE,

    DBAL_NOF_VALUE_ENCODE_TYPES
  } dbal_value_encode_types_e;

typedef enum
  {
    DBAL_DIRECT_ACCESS_MEMORY,
    DBAL_DIRECT_ACCESS_REGISTER,
    DBAL_DIRECT_ACCESS_PEMLA,

    DBAL_NOF_DIRECT_ACCESS_TYPES
  } dbal_direct_access_types_e;

typedef enum
  {
    DBAL_LABEL_NONE,
    DBAL_LABEL_L2,
    DBAL_LABEL_L3,
    DBAL_LABEL_MPLS,
    DBAL_LABEL_FCOE,
    DBAL_LABEL_ALL_LABELS,

    DBAL_NOF_LABEL_TYPES
  } dbal_labels_e;

typedef enum
  {
    DBAL_ACCESS_PHY_TABLE,
    DBAL_ACCESS_DIRECT,

    DBAL_NOF_ACCESS_TYPES
  } dbal_access_type_e;

typedef enum
  {
    DBAL_CORE_NONE,
    DBAL_CORE_BY_INPUT, /*DPC*/
    DBAL_CORE_ALL, /*SBC*/

    DBAL_NOF_CORE_MODE_TYPES

}dbal_core_mode_e;

typedef enum
  {
    DBAL_CORE_NOT_INTIATED = (-2),
    DBAL_CORE_ANY = (-1),
    DBAL_CORE_0,
    DBAL_CORE_1
  } dbal_core_e;

typedef enum
  {
    DBAL_CONDITION_NONE,
    DBAL_CONDITION_BIGGER_THAN,
    DBAL_CONDITION_LOWER_THAN,
    DBAL_CONDITION_IS_EQUAL_TO,
    DBAL_CONDITION_IS_EVEN,
    DBAL_CONDITION_IS_ODD,

      /* pssible more conditions: DIFFERENT_THAN, */

    DBAL_NOF_CONDITION_TYPES
  } dbal_condition_types_e;

typedef enum
  {
    /*
     * will return all entries including "0" entries and default entries 
     */
    DBAL_ITER_MODE_ALL,

    /*
     * will return all entries without entries that equals to default value if not defined default value "0" is used 
     */
    DBAL_ITER_MODE_GET_ALL_BUT_DEFAULT_ENTRIES,

    DBAL_NOF_ITER_TYPES
  } dbal_iterator_mode_e;

typedef enum
  {
    DBAL_COMMIT_NORMAL = 0,
    DBAL_COMMIT_USE_DEFAULT = SAL_BIT(1),
    DBAL_COMMIT_KEEP_HANDLE = SAL_BIT(2),
    DBAL_COMMIT_OVERRUN_ENTERY = SAL_BIT(3),    /* used for direct access when no need to read the entry */

    DBAL_COMMIT_NOF_OPCODES
  } dbal_entry_action_flags_e;

typedef shr_error_e(
  *DBAL_FIELD32_ENCODING_FUNC) (
  int unit,
  uint32 field_val,
  uint32 in_param1,
  uint32 in_param2,
  uint32 * field_val_out);

typedef shr_error_e(
  *DBAL_FIELD_ARRAY_ENCODING_FUNC) (
  int unit,
  uint32 in_param1,
  uint32 in_param2);

typedef shr_error_e(
  *DBAL_FIELD32_DECODING_FUNC) (
  int unit,
  uint32 field_val,
  uint32 in_param1,
  uint32 in_param2,
  uint32 * field_val_out);

typedef struct
  {
    char name[DBAL_MAX_STRING_LENGTH];
    uint32 max_size;            /*for array num of bytes */
    dbal_field_type_e type;
    dbal_labels_e labels[DBAL_NOF_LABEL_TYPES];
    dbal_fields_e parent_field_id;
    uint8 is_default_value_valid;
    uint32 default_value;
    dbal_value_encode_types_e encode_type;
    uint32 value_input_param1;  /* for example if the index is input_key%8 + input_key/8  than offset_input_param=8 */
    uint32 value_input_param2;  /* for example if the index is input_key%8 + input_key/8  than offset_input_param=8 */
    DBAL_FIELD32_ENCODING_FUNC field_32_encoding_func;  /* pointer to a function that will perform the encoding */
    DBAL_FIELD32_DECODING_FUNC field_32_decoding_func;  /* pointer to a function that will perform the decoding */

  } dbal_field_basic_info_t;

typedef struct
  {
    dbal_fields_e field_id;
    uint32 field_val;
    dbal_condition_types_e condition;

  } dbal_iterator_rule_info_t;


typedef struct
  {
    dbal_fields_e field_id;
    uint32* field_val;

  } dbal_returned_field_t;


typedef struct
  {
    /*
     * the index of the iterator 0 - first entry (this is for internal use)
     */
    uint32 entry_index;

    /*
     * number of entries found. 
     */
    uint32 entry_number;

    dbal_tables_e table_id;

    /*
     * indicates that the iterator run over all entries. current entry is not valid
     */
    int    is_end;
    int    max_num_of_iterations;

    /*
     * which entries will be chosen 
     */
    dbal_iterator_rule_info_t   rule;    
    dbal_iterator_mode_e        mode;

    int num_of_fields;
    /*
     * pointers of the requested fields to the value returned from the table
     */
    dbal_returned_field_t requested_fields_info[DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS];

  } dbal_iterator_info_t;

typedef struct
  {
    dbal_value_encode_types_e encode_mode;
    uint32 input_param;
    dbal_fields_e field_id;     /* in-case of the offset of the entry is used only by one fields from the key fields */
  } dbal_value_encode_info_t;

typedef struct
  {
    dbal_condition_types_e index_mode;
    uint32 input_param;
  } dbal_acces_condition_info_t;

typedef struct
  {
    /*
     * logical 2 physical direct access field info 
     */
    dbal_fields_e field_id;
    dbal_fields_e field_pos_in_table;
    uint32 offset;              /* start bit TBD can be updated in table init and used without checking */
    uint32 nof_bits;            /* if zero using full length */
    soc_mem_t memory;
    soc_reg_t reg[DBAL_MAX_NUMBER_OF_REGISTERS];
    soc_field_t hw_field;       /* uint32 for pemla */

    dbal_value_encode_info_t    mem_array_offset_info;
	dbal_value_encode_info_t    memory_offset_info;/* entry offset */
    dbal_value_encode_info_t field_offset_info;

    dbal_condition_types_e condition_mode;
    uint32 condition_input_param;
  } dbal_direct_l2p_field_info_t;

typedef struct
  {
    uint8 num_of_access_fields;
    dbal_direct_l2p_field_info_t l2p_fields_info[DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS];
  } dbal_direct_l2p_info_t;

/* generic field info */
typedef struct
  {
    dbal_fields_e field_id;
    int field_nof_bits;         /* if zero, full length will be used. */
    int bits_offset_in_buffer;  /* offset in the handle buffer  */
    uint8 is_key;
    uint8 is_read_only;         /* can be only cleared */
    uint8 is_default_valid;
    uint32 default_val;         /*? (what about array default value) */
    uint8 is_field_encoded;
    

  } dbal_table_field_info_t;

typedef struct
  {
    uint8 key_size;
    uint32 key[(DBAL_FIELD_ARRAY_MAX_SIZE * DBAL_TABLE_MAX_NUM_OF_KEY_FIELDS) / 4];
    uint32 k_mask[(DBAL_FIELD_ARRAY_MAX_SIZE * DBAL_TABLE_MAX_NUM_OF_KEY_FIELDS) / 4];
    uint8 payload_size;
    uint8 payload_type;         /*? */
    uint32 payload[(DBAL_FIELD_ARRAY_MAX_SIZE * DBAL_TABLE_MAX_NUM_OF_KEY_FIELDS) / 4];
    uint32 p_mask[(DBAL_FIELD_ARRAY_MAX_SIZE * DBAL_TABLE_MAX_NUM_OF_KEY_FIELDS) / 4];
    /*
     * if there is big fields use dynamic allocation 
     */
    uint8 hitbit;
  } dbal_physical_entry_t;

typedef struct
  {
    dbal_field_type_e type;
    void *returned_pointer;
  } dbal_user_output_info_t;

typedef struct
  {
    uint8 size;
    uint8 is_key;
    uint8 value[DBAL_FIELD_ARRAY_MAX_SIZE];
    uint8 v_mask[DBAL_FIELD_ARRAY_MAX_SIZE];
  } dbal_physical_field_t;

typedef struct
  {
    /*
     * input parameters 
     */
    char table_name[DBAL_MAX_STRING_LENGTH];
    dbal_labels_e labels[DBAL_NOF_LABEL_TYPES];
    uint8 is_table_initiated;
    uint8 sw_shadow_enabled;    /* can be in modes, save only key / save complete entries */
    dbal_core_mode_e core_mode;
    uint32 max_capacity;        /* if 0, dynamic according to HW limitations otherwise the max number of entries */
    uint32 min_index;
    dbal_access_type_e access_type;
    dbal_physical_tables_e physical_db_id;
    int nof_entries;            /* calculated parameters */
    /*
     * uint32                          db_identifier;field_id, size, offset operator can be multiple fields (something like prefix today) this can help when we don't want to save SW for all entries but we want to identify entries from HW 
     */
    int entry_payload_size;     /* the maximum size of an entry */
    int num_of_fields;
    /*
     * uint32                          max_index; calculated, min_index+max_capacity 
     */
    dbal_table_field_info_t fields_info[DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS];
    uint32 app_id;
    dbal_direct_l2p_info_t          l2p_direct_info[DBAL_NOF_DIRECT_ACCESS_TYPES];
    dbal_physical_entry_t *entries_shadow;      /* this should be allocated when shadowing is enabled */
    dbal_physical_field_t *sw_fields;   /* this should be allocated when there is fields in the wb_sw_state */

  } dbal_logical_table_t;

typedef struct
  {
    dbal_tables_e table_id;
    dbal_logical_table_t *table;
    dbal_fields_e field_ids[DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS];
    uint32 table_field_pos[DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS];
    uint8 num_of_fields;
    uint8 trans_id;
    dbal_entry_handle_status_e status;
    uint32 index;               /*not used */
    dbal_physical_entry_t phy_entry;

    /*
     * core_id holds the core to preform the action, 
     * in table that works in core_mode by input if the core ID was not added we use the default value 
     */
    int core_id;
    dbal_user_output_info_t user_output_info[DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS];
  } dbal_entry_handle_t;

/*****************************************************PHYSICAL TABLE DEFENITIONS***************************************************************/
typedef shr_error_e(
  *PHYSICAL_TABLE_ENTRY_ADD) (
  int unit,
  dbal_physical_tables_e physical_table,
  uint32 app_id,
  dbal_physical_entry_t * entry);

typedef shr_error_e(
  *PHYSICAL_TABLE_ENTRY_GET) (
  int unit,
  dbal_physical_tables_e physical_table,
  uint32 app_id,
  dbal_physical_entry_t * entry);

typedef shr_error_e(
  *PHYSICAL_TABLE_ENTRY_DELETE) (
  int unit,
  dbal_physical_tables_e physical_table,
  uint32 app_id,
  dbal_physical_entry_t * entry);

typedef shr_error_e(
  *PHYSICAL_TABLE_CLEAR) (
  int unit);

typedef shr_error_e(
  *PHYSICAL_TABLE_DEFAULT_VALUES_SET) (
  int unit);

typedef shr_error_e(
  *PHYSICAL_TABLE_INIT) (
  int unit);

typedef struct
  {
    /**************************************** Database information ******************************************/
    char physical_name[DBAL_MAX_STRING_LENGTH];
    dbal_physical_tables_e physical_db_type;
    int max_entry_size;
    int max_capacity;
    int nof_entries;

    /**************************************** Database operations ********************************************/
    PHYSICAL_TABLE_ENTRY_ADD entry_add;
    PHYSICAL_TABLE_ENTRY_GET entry_get;
    PHYSICAL_TABLE_ENTRY_DELETE entry_delete;
    PHYSICAL_TABLE_CLEAR table_clear;
    PHYSICAL_TABLE_DEFAULT_VALUES_SET table_default_values_set;
    PHYSICAL_TABLE_INIT table_init;
    /*
     * TBD add iterator functions
     */

  } dbal_physical_table_def_t;

shr_error_e dbal_lpm_entry_add(
  int unit,
  dbal_physical_tables_e physical_table,
  dbal_physical_entry_t * entry);
shr_error_e dbal_tcam_entry_add(
  int unit,
  dbal_physical_tables_e physical_table,
  dbal_physical_entry_t * entry);
shr_error_e dbal_em_entry_add(
  int unit,
  dbal_physical_tables_e physical_table,
  dbal_physical_entry_t * entry);
shr_error_e dbal_physical_table_get(
  int unit,
  dbal_physical_tables_e physical_table_id,
  dbal_physical_table_def_t ** physical_table);
shr_error_e dbal_physical_table_init(
  int unit);
const char *dbal_physical_table_name_get(
  int unit,
  dbal_physical_tables_e physical_table_id);

typedef struct
  {
    dbal_physical_table_def_t physical_tables[DBAL_NOF_PHYSICAL_TABLES];

  } dbal_physical_mngr_info_t;

typedef struct
  {
    dbal_logical_table_t logical_tables[DBAL_NOF_TABLES];

  } dbal_logical_tables_info_t;

typedef struct
  {
    dbal_entry_handle_t   entry_handles_pool[DBAL_SW_NOF_ENTRY_HANDLES];
    uint8 num_of_entry_handles_used;
    dbal_work_modes_e work_mode;
    uint8 is_intiated;
  } dbal_mngr_info_t;

typedef struct
  {
    char hw_entity_name[DBAL_MAX_STRING_LENGTH]; 
    int hw_entity_val;
  } dbal_hw_entity_mapping;

#endif/*_DBAL_STRUCTURES_INCLUDED__*/

