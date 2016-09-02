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

#ifndef _DBAL_API_INCLUDED__
#define _DBAL_API_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file %s is for use by DNX (JR2) family only!"
#endif

#include <soc/dnx/dbal_structures.h>

/**************************************************DBAL APIs******************************************************************/

/*!
 * \brief preform de-init to module.
 */
shr_error_e dbal_init(
  int unit);

/*!
 * \brief preform de-init to module. 
 *  
 * TBD implementation needed, check if there is unreleased handles.
 */
shr_error_e dbal_deinit(
  int unit);

/*!
 * \brief setting DBAL SW work modes.
 *
 * when working with SW enable no HW access will be preformed \n
 *
 * \par DIRECT INPUT:
 *   \param [in] unit -
 *   \param [in] enable - \n
 *     1 enable SW mode, 0 disable /sw mode
 */
shr_error_e dbal_sw_work_mode_set(
  int unit,
  int enable);

shr_error_e dbal_log_severity_get(
  int unit,
  bsl_severity_t * severity);

shr_error_e dbal_log_severity_set(
  int unit,
  int severity);

/*!
 * \brief setting DBAL SW work modes.
 *
 * prints status about all handles, \n
 *
 * \par DIRECT INPUT:
 *   \param [in] unit -
 *   \param [in] handle_id - \n
 *     -1 for all handles status, other specific handle status
 */
shr_error_e dbal_handles_info_dump(
  int unit,
  int handle_id);

/*************************Physical DB operations*************************/

/*!
 * \brief prints general info related to physical DB. 
 *  
 * TBD implementation needed
 */
shr_error_e dbal_physical_db_info_print(
  int unit,
  dbal_physical_tables_e physical_db_id);

/*************************Logical Table operations*************************/

/*!
 * \brief delete all entries related to table. 
 *  
 * TBD implementation needed
 */
shr_error_e dbal_table_clear(
  int unit,
  dbal_tables_e table_id,
  uint8 mode);

/*!
 * \brief delete all entries related to table. 
 *  
 */
shr_error_e dbal_table_info_print(
  int unit,
  dbal_tables_e table_id);

/*!
 * \brief dump all entries related to table. 
 *  
 */
shr_error_e dbal_table_dump_all(
  int unit,
  dbal_labels_e label,
  int mode);

/*!
 * \brief return table field info, error if field not related to table. 
 *  
 */
shr_error_e dbal_table_field_info_get(
  int unit,
  dbal_tables_e table_id,
  dbal_fields_e field_id,
  dbal_table_field_info_t * field_info);

/*!
 * \brief temp function need to be deleted.
 *  
 */
shr_error_e dbal_table_entries_print_from_shadow(
  int unit,
  dbal_tables_e table_id,
  uint8 format,
  uint32 max_entries);

/*!
 * \brief return the logical table info, error if table not found. 
 *  
 */
shr_error_e dbal_logical_table_get(
  int unit,
  dbal_tables_e table_id,
  dbal_logical_table_t ** table);

/*************************Fields operations*************************/

/*!
 *\brief dump all existing fields related to a specific label, if label DBAL_NOF_LABEL_TYPES || DBAL_LABEL_NONE prints all efields. 
 *
 * \par DIRECT INPUT:
 *   \param [in] unit -\n
 *   \param [in] label -\n
 */
shr_error_e dbal_fields_dump_all(
  int unit,
  dbal_labels_e label);

/*************************Entry operations*************************/
/*!
 *\brief allocate handle from pool. 
 *
 * \par DIRECT INPUT:
 *   \param [in] unit -  \n
 *   \param [in] table_id -  \n
 *   \param [out] entry_handle_id -  \n
 */
shr_error_e dbal_entry_handle_take(
  int unit,
  dbal_tables_e table_id,
  uint32 * entry_handle_id);

/*!
 *\brief return handle to pool. 
 *
 * \par DIRECT INPUT:
 *   \param [in] unit -  \n
 *   \param [in] entry_handle_id -  \n
 */
shr_error_e dbal_entry_handle_release(
  int unit,
  uint32 entry_handle_id);


/*!
 *\brief get handle from pool. 
 *
 * \par DIRECT INPUT:
 *   \param [in] unit -  \n
 *   \param [in] entry_handle_id -  \n
 *   \param [out] entry_handle -  \n
 */
shr_error_e
dbal_entry_handle_info_get(
  int unit,
  uint32 entry_handle_id,
  dbal_entry_handle_t * entry_handle);

/*!
 * \brief set the entry to HW according to flags.
 *
 * \par DIRECT INPUT:
 *   \param [in]  unit \n
 *   \param [in]  entry_handle_id \n
 *     Layer/source combination:  \n
 *     For example: BSL_L_BCMDNX,BSL_S_INITSEQDNX
 *   \param [in] flags \n
 *     Variable to be loaded by One of the bsl_severity_t levels:\n
 *     bslSeverityFatal,bslSeverityError,etc.
 * \remark
 */
shr_error_e dbal_entry_commit(
  int unit,
  uint32 entry_handle_id,
  dbal_entry_action_flags_e flags);

/*!
 * \brief set the entry to HW according to flags.
 *
 * \par DIRECT INPUT:
 *   \param [in]  unit \n
 *   \param [in]  entry_handle_id \n
 *     Layer/source combination:  \n
 *     For example: BSL_L_BCMDNX,BSL_S_INITSEQDNX
 *   \param [in] flags \n
 * \remark
 * 
 */
shr_error_e dbal_entry_get(
  int unit,
  uint32 entry_handle_id,
  dbal_entry_action_flags_e flags);

/*!
 * \brief delete the entry from HW according to flags.
 *
 * \par DIRECT INPUT:
 *   \param [in]  unit  \n
 *   \param [in]  entry_handle_id  \n
 *   \param [in] flags \n
 * \remark
 * applicable only for MDB tables.
 */
shr_error_e dbal_entry_delete(
  int unit,
  uint32 entry_handle_id,
  dbal_entry_action_flags_e flags);

shr_error_e dbal_entry_print(
  int unit,
  uint32 entry_handle_id,
  uint8 format);


/* iterator APIs */

/*!
 *\brief init the iterator, must be called first. the iterator allow to run over all entries related to a specific table 
 *
 *IMPORTANT NOTE: when using the iterator no other operation allowed on the table. (other operations will print a warning)
 *
 * \par DIRECT INPUT:
 *   \param [in]  unit          -\n
 *   \param [in]  table_id      - the related table of the iterator\n
 *   \param [in]  mode          - which entries are valid for the iterator\n
 *   \param [in]  rule          - rule to specify which entries will be returned (example only cor_id == 0)\n
 *   \param [in]  returned_fields_info - the number of fields and all related pointers to the data\n
 *   \param [out] iterator_info - returned value the info of the iterator\n
 */
shr_error_e dbal_table_iterator_init(
  int                                       unit,  
  dbal_tables_e                             table_id,
  dbal_iterator_mode_e                      mode,
  dbal_iterator_info_t* iterator_info);


/*!
 *\brief sets information related to the returned values of the iterator. must set this values before calling dbal_table_iterator_get_next() 
 * 
 * \par DIRECT INPUT:
 *   \param [in]  field_id          - field ID related to the pointer\n
 *   \param [in]  field_val_pointer - a pointer to the data\n
 *   \param [out] iterator_info - returned value the info of the iterator\n
 */

#define DBAL_ITERATOR_RETURNED_INFO_SET(dbal_iterator, field_id, field_val_pointer)                                       \
                    if(1)                                                                                                 \
                    {                                                                                                     \
                        int  num_of_fields = dbal_iterator.num_of_fields;                                                 \
                        dbal_iterator.requested_fields_info[num_of_fields].field_id  = field_id;                          \
                        dbal_iterator.requested_fields_info[num_of_fields].field_val = field_val_pointer;                 \
                        dbal_iterator.num_of_fields++;                                                                    \
                    }

/*!
 *\brief sets information related to a iterator rule can be called before dbal_table_iterator_get_next() 
 *
 * \par DIRECT INPUT:
 *   \param [in]  field_id  - field ID related to the pointer\n
 *   \param [in]  field_val - the value to relate in the condition \n
 *   \param [in]  condition - condition (equal_to\bigger_than...)\n
 *   \param [out] iterator_info - updated iterator info with the new rule\n
 */
#define DBAL_ITERATOR_RULE_SET(dbal_iterator, field_id, field_val, condition)                            \
                    dbal_iterator.rule.field_id  = field_id;                                             \
                    dbal_iterator.rule.field_val = field_val;                                            \
                    dbal_iterator.rule.condition = condition;

/*!
 *\brief return the next entry of the table. 
 *
 * \par DIRECT INPUT:
 *   \param [in]  unit -\n
 *   \param [in/out] iterator_info - in: the last entry info returned. out:next entry info\n
 */
shr_error_e dbal_table_iterator_get_next(
  int unit,
  dbal_iterator_info_t* iterator_info);


/*!
 *\brief destroy the iterator, must be called when finishing to work with the iterator.
 *
 * \par DIRECT INPUT:
 *   \param [in]  unit -\n
 *   \param [in]  iterator_info - the iterator that \n
 */
shr_error_e dbal_table_iterator_destroy(
  int unit,
  dbal_iterator_info_t* iterator_info);



/*!
 *\brief print iterator information for debug.
 *
 * \par DIRECT INPUT:
 *   \param [in]  unit -\n
 *   \param [in]  iterator_info - the iterator to print\n
 */
shr_error_e dbal_table_iterator_info_dump(
  int unit,
  dbal_iterator_info_t* iterator_info);







/* OPTION 2 */
/*!
 *\brief retrieve field value from the entry returned by the iterator. 
 *
 * \par DIRECT INPUT:
 *   \param [in]  unit -\n
 *   \param [in]  field_id - the field id which information will be returned for\n
 *   \param [out] field_val - returned value the info of the iterator value returned has uin32[].\n
 */
shr_error_e dbal_iterator_field_array32_get(
  int unit,  
  dbal_iterator_info_t* iterator_info,
  dbal_fields_e field_id,
  uint32* field_val);



/*************************Entry field operations*************************/
shr_error_e dbal_entry_field32_set(
  int unit,
  uint32 entry_handle_id,
  dbal_field_key_mode_e key_mode,
  dbal_fields_e field_id,
  uint32 field_val);

shr_error_e dbal_entry_field8_set(
  int unit,
  uint32 entry_handle_id,
  dbal_field_key_mode_e key_mode,
  dbal_fields_e field_id,
  uint8 field_val);

shr_error_e dbal_entry_field16_set(
  int unit,
  uint32 entry_handle_id,
  dbal_field_key_mode_e key_mode,
  dbal_fields_e field_id,
  uint16 field_val);

shr_error_e dbal_entry_field64_set(
  int unit,
  uint32 entry_handle_id,
  dbal_field_key_mode_e key_mode,
  dbal_fields_e field_id,
  uint64 field_val);

shr_error_e dbal_entry_array8_set(
  int unit,
  uint32 entry_handle_id,
  dbal_field_key_mode_e key_mode,
  dbal_fields_e field_id,
  uint8 field_val[DBAL_FIELD_ARRAY_MAX_SIZE]);

shr_error_e dbal_entry_array32_set(
  int unit,
  uint32 entry_handle_id,
  dbal_field_key_mode_e key_mode,
  dbal_fields_e field_id,
  uint32 field_val[DBAL_FIELD_ARRAY_MAX_SIZE / 4]);

shr_error_e dbal_entry_field32_set_mask(
  int unit,
  uint32 entry_handle_id,
  dbal_field_key_mode_e key_mode,
  dbal_fields_e field_id,
  uint32 field_val,
  uint32 field_mask);

shr_error_e dbal_entry_field8_set_mask(
  int unit,
  uint32 entry_handle_id,
  dbal_field_key_mode_e key_mode,
  dbal_fields_e field_id,
  uint8 field_val,
  uint8 field_mask);

shr_error_e dbal_entry_field16_set_mask(
  int unit,
  uint32 entry_handle_id,
  dbal_field_key_mode_e key_mode,
  dbal_fields_e field_id,
  uint16 field_val,
  uint16 field_mask);

shr_error_e dbal_entry_field64_set_mask(
  int unit,
  uint32 entry_handle_id,
  dbal_field_key_mode_e key_mode,
  dbal_fields_e field_id,
  uint64 field_val,
  uint64 field_mask);

shr_error_e dbal_entry_array8_set_mask(
  int unit,
  uint32 entry_handle_id,
  dbal_field_key_mode_e key_mode,
  dbal_fields_e field_id,
  uint8 field_val[DBAL_FIELD_ARRAY_MAX_SIZE],
  uint8 field_mask[DBAL_FIELD_ARRAY_MAX_SIZE]);

shr_error_e dbal_entry_array32_set_mask(
  int unit,
  uint32 entry_handle_id,
  dbal_field_key_mode_e key_mode,
  dbal_fields_e field_id,
  uint32 field_val[DBAL_FIELD_ARRAY_MAX_SIZE],
  uint8 field_mask[DBAL_FIELD_ARRAY_MAX_SIZE]);

shr_error_e dbal_entry_field8_get(
  int unit,
  uint32 entry_handle_id,
  dbal_field_key_mode_e key_mode,
  dbal_fields_e field_id,
  uint8 * field_val);

shr_error_e dbal_entry_field16_get(
  int unit,
  uint32 entry_handle_id,
  dbal_field_key_mode_e key_mode,
  dbal_fields_e field_id,
  uint16 * field_val);

shr_error_e dbal_entry_field32_get(
  int unit,
  uint32 entry_handle_id,
  dbal_field_key_mode_e key_mode,
  dbal_fields_e field_id,
  uint32 * field_val);

shr_error_e dbal_entry_array8_get(
  int unit,
  uint32 entry_handle_id,
  dbal_field_key_mode_e key_mode,
  dbal_fields_e field_id,
  uint8 * field_val);

shr_error_e dbal_entry_array32_get(
  int unit,
  uint32 entry_handle_id,
  dbal_field_key_mode_e key_mode,
  dbal_fields_e field_id,
  uint32 * field_val);

/**********************************************************************************************/

#endif/*_DBAL_INCLUDED__*/
