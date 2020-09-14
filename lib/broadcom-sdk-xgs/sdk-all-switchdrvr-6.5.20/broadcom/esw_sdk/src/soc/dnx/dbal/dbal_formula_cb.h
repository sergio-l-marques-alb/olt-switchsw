/**
 * \file dbal_formula_cb.h
 * $Id$
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * DBAL tables formula callbacks definition and helpers
 *
 */

#ifndef DBAL_FORMULA_CB_H_INCLUDED
#  define DBAL_FORMULA_CB_H_INCLUDED

/*************
 * INCLUDES  *
 *************/
#include <soc/dnx/dbal/dbal_structures.h>

/*************
 *  DEFINES  *
 *************/

/**
 * \brief formula offset cb indicator
 */
#define DBAL_INIT_FORMULA_CB_INDICATOR                "CB"

#define DBAL_FORMULA_CB_LAST_CB                       "last_cb"

/*************
 *  MACROS  *
 *************/
/**
 * \brief
 * Get full key size 
 */
#define DBAL_FORMULA_CB_GET_FULL_KEY_SIZE(unit, entry_handle, size) \
        do                                                      \
        {                                                       \
            size = entry_handle->table->key_size;            \
        }while (0);

/**
 * \brief
 * Get specific key size
 */
#define DBAL_FORMULA_CB_GET_KEY_FIELD_SIZE(unit, entry_handle, field_id, size) \
        do                      \
        {                       \
            dbal_table_field_info_t field_info;                 \
            SHR_IF_ERR_EXIT(dbal_tables_field_info_get_no_err(unit, entry_handle->table_id, field_id, TRUE, 0, 0, &field_info));     \
            size = field_info.field_nof_bits;   \
        }while(0);

/**
 * \brief
 * Get full key value
 */
#define DBAL_FORMULA_CB_GET_FULL_KEY_VALUE(unit, entry_handle, key_value) \
        do                      \
        {                       \
            key_value = entry_handle->phy_entry.key[0];            \
        }while(0);

/**
 * \brief
 * Get specific key value
 */
#define DBAL_FORMULA_CB_GET_KEY_VALUE(unit, entry_handle, field_id, key_value) \
        do                                  \
        {                                   \
            uint32 field_val_arr[1];        \
            SHR_IF_ERR_EXIT(dbal_entry_enum_hw_value_or_key_field_get(unit, entry_handle, field_id, field_val_arr, NULL, DBAL_POINTER_TYPE_ARR_UINT32)); \
            key_value = field_val_arr[0];  \
        }while(0);

/**
 * \brief
 * Get full result size
 */
#define DBAL_FORMULA_CB_GET_FULL_RESULT_SIZE(unit, entry_handle, result_size) \
        do                      \
        {                       \
            result_size = entry_handle->table->multi_res_info[entry_handle->cur_res_type].entry_payload_size;            \
        }while(0);

/**
 * \brief
 * Get specific result field size
 */
#define DBAL_FORMULA_CB_GET_RESULT_FIELD_SIZE(unit, entry_handle, field_id, size) \
        do                      \
        {                       \
            dbal_table_field_info_t field_info;                 \
            dbal_entry_handle_t * handle = (dbal_entry_handle_t *) entry_handle; \
            SHR_IF_ERR_EXIT(dbal_tables_field_info_get_no_err(unit, handle->table_id, field_id, FALSE, handle->cur_res_type, 0, &field_info));     \
            size = field_info.field_nof_bits;   \
        }while(0);

/** 
 * \brief 
 * Get instance index 
 */
#define DBAL_FORMULA_CB_GET_INSTANCE_IDX(unit, field_id, instance_idx) \
        do                      \
        {                       \
            instance_idx = dbal_fields_instance_idx_get(unit, field_id); \
        }while(0);

/*************
 * TYPEDEFS  *
 *************/

typedef struct offset_cb_from_str_s
{
    char *cb_str;
    dnx_dbal_formula_offset_cb cb;
} offset_cb_from_str_t;

/*************
 * FUNCTIONS *
 *************/

/**
   \brief 
 * generic name for instance * current field size
 * \param [in] unit - Device Id
 * \param [in] entry_handle - entry handle 
 * \param [in] current_mapped_field_id - current mapped field id
 * \param [out] offset - calculated offset
 * \return
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e instance_multiplied_by_result_field_size_cb(
    int unit,
    void *entry_handle,
    dbal_fields_e current_mapped_field_id,
    uint32 *offset);

/**
 * \brief
 *  Callback calculate example for table EXAMPLE_TABLE_REGISTER_GROUP, field DESTINATION_ENCODING.
 *  For reference, see tools/autocoder/dbal/dnx/tables/tables_hard_logic/hl_tables_examples_definition.xml
 * \param [in] unit - Device Id
 * \param [in] entry_handle - entry handle 
 * \param [in] current_mapped_field_id - current mapped field id
 * \param [out] offset - calculated offset
 * \return
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e example_formula_cb(
    int unit,
    void *entry_handle,
    dbal_fields_e current_mapped_field_id,
    uint32 *offset);

/**
 * \brief
 *  Callback calculate for FC tables in IMB. Calculates internal lane in ETU from Lane in core
 * \param [in] unit - Device Id
 * \param [in] entry_handle - entry handle 
 * \param [in] current_mapped_field_id - current mapped field id
 * \param [out] offset - calculated offset
 * \return
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e imb_fc_lane_in_core_to_internal_lane_in_ethu_cb(
    int unit,
    void *entry_handle,
    dbal_fields_e current_mapped_field_id,
    uint32 *offset);

/**
 * \brief
 *  Callback calculate for FC tables in IMB. Calculates ETHU per type ID (CDU/CLU) from Lane in core
 * \param [in] unit - Device Id
 * \param [in] entry_handle - entry handle
 * \param [in] current_mapped_field_id - current mapped field id
 * \param [out] offset - calculated offset
 * \return
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e imb_fc_lane_in_core_to_ethu_type_index_cb(
    int unit,
    void *entry_handle,
    dbal_fields_e current_mapped_field_id,
    uint32 *offset);

/**
 * \brief
 *  Callback calculate for FC tables in IMB. Calculates ETHU per type ID (CDU/CLU) from ETHU ID
 * \param [in] unit - Device Id
 * \param [in] entry_handle - entry handle
 * \param [in] current_mapped_field_id - current mapped field id
 * \param [out] offset - calculated offset
 * \return
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e imb_fc_ethu_id_to_ethu_type_index_cb(
    int unit,
    void *entry_handle,
    dbal_fields_e current_mapped_field_id,
    uint32 *offset);

extern offset_cb_from_str_t formula_offset_cb_map[];

#endif /* DBAL_FORMULA_CB_H_INCLUDED */
