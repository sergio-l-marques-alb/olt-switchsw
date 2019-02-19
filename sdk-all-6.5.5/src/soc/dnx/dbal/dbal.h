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

#ifndef _DBAL_INCLUDED__
#  define _DBAL_INCLUDED__

#  include <soc/dnx/dbal_api.h>
#  include <shared/shrextend/shrextend_debug.h>
#  include <shared/utilex/utilex_str.h>
#  include <soc/dnx/dbal_defines.h>
#  include "dbal_string_mgmt.h"
#  include "dbal_db_init.h"

/************** shared dbal functionality **************/
int dbal_work_mode_is_only_sw(
  int unit);

shr_error_e dbal_action_prints(
  int unit,
  uint32 entry_handle_id,
  dbal_entry_action_flags_e flags,
  char *src_str);

/* getting the value of fields with type uint8/16/32/IP*/
shr_error_e dbal_specific_field_from_buffer_get(
  int unit,
  dbal_entry_handle_t * entry_handle,
  dbal_fields_e field_id,
  uint32 * field_val_returned,
  uint32 * is_found);
shr_error_e dbal_entry_save_in_shadow(
  int unit,
  dbal_entry_handle_t * entry_handle);
shr_error_e dbal_entry_remove_from_shadow(
  int unit,
  dbal_entry_handle_t * entry_handle);

/************** Field APIs **************/
shr_error_e dbal_fields_init(
  int unit);
shr_error_e dbal_fields_is_field_encoded(
  int unit,
  dbal_fields_e field_id);
shr_error_e dbal_fields_field32_encode(
  int unit,
  dbal_fields_e field_id,
  uint32 field_val,
  uint32 * field_value_out);
shr_error_e dbal_fields_field32_decode(
  int unit,
  dbal_fields_e field_id,
  uint32 field_val,
  uint32 * field_value_out);
shr_error_e dbal_fields_parent_field_id_get(
  int unit,
  dbal_fields_e field_id,
  dbal_fields_e * parent_field_id);
shr_error_e dbal_fields_max_size_get(
  int unit,
  dbal_fields_e field_id,
  uint32 * field_size);
shr_error_e dbal_fields_type_get(
  int unit,
  dbal_fields_e field_id,
  dbal_field_type_e* field_type);
shr_error_e dbal_fields_encode_type_get(
  int unit,
  dbal_fields_e field_id,
  dbal_value_encode_types_e* encode_type);
shr_error_e dbal_fields_field_info_get(
  int unit,
  dbal_fields_e field_id,
  dbal_field_basic_info_t ** field_info);

shr_error_e dbal_fields_field_info_print(
  int unit,
  dbal_fields_e field_id);
/************** Physical table APIs **************/
shr_error_e dbal_phy_table_entry_add(
  int unit,
  dbal_physical_tables_e physical_db_id,
  uint32 app_id,
  dbal_physical_entry_t * phy_entry);
shr_error_e dbal_phy_table_entry_get(
  int unit,
  dbal_physical_tables_e physical_db_id,
  uint32 app_id,
  dbal_physical_entry_t * phy_entry);
shr_error_e dbal_phy_table_entry_delete(
  int unit,
  dbal_physical_tables_e physical_db_id,
  uint32 app_id,
  dbal_physical_entry_t * phy_entry);
shr_error_e dbal_max_entry_size_get(
  int unit,
  dbal_physical_tables_e physical_table,
  int *max_entry_size);
/************** Direct access APIs **************/
shr_error_e dbal_direct_entry_set(
  int unit,
  dbal_entry_handle_t * entry_handle);
shr_error_e dbal_direct_entry_get(
  int unit,
  dbal_entry_handle_t * entry_handle);

shr_error_e dbal_direct_entry_get_next(
  int unit,
  uint32 entry_handle_id, dbal_iterator_info_t* iterator_info);

/************** Logical table APIs **************/
shr_error_e dbal_logical_tables_init(
  int unit);

shr_error_e dbal_field_from_buffer_print(
  int unit,
  dbal_fields_e field_id,
  uint32 field_val[DBAL_FIELD_ARRAY_MAX_SIZE],
  uint32 field_mask[DBAL_FIELD_ARRAY_MAX_SIZE],
  int size,
  int offset);                  /* add type EM/LPM/TCAM */

shr_error_e dbal_fields_from_buffer_get(
  int unit,
  dbal_entry_handle_t * entry_handle,
  int is_only_payload_fields);

#  if 0
#    define  DBAL_DOCUMENTATION_MODE_PRINTS                                                         \
  if (1)                                                                                        \
  {                                                                                             \
    int i;                                                                                      \
    /* Documentation mode, not access the HW just printing info */                              \
    LOG_CLI((BSL_META("\nLogical table %s configured \n"), table->table_name));                 \
    LOG_CLI((BSL_META("\tUpdated fields:  \n\t")));                                             \
    for (i = 0; i < entry_handle->num_of_fields; i++)                                           \
    {                                                                                           \
      if ( i != entry_handle->num_of_fields-1)                                                  \
      {                                                                                         \
        LOG_CLI((BSL_META("%s, "), dbal_field_name_get(unit, entry_handle->field_ids[i]) ));    \
      }                                                                                         \
      else                                                                                      \
      {                                                                                         \
        LOG_CLI((BSL_META("%s \n\n"), dbal_field_name_get(unit, entry_handle->field_ids[i]) )); \
      }                                                                                         \
    }                                                                                           \
    SHR_EXIT();                                                                                 \
  }
#  else
#    define  DBAL_DOCUMENTATION_MODE_PRINTS
#  endif

#endif/*_DBAL_INCLUDED__*/
