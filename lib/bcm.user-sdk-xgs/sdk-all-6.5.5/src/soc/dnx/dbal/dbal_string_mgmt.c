/*! \file dbal_string_mgmt.c
 *
 * Main functions for dbal strings (names) conversion
 * basically string_to_id and to_string functions
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

#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DBALDNX

/*************
 * INCLUDES  *
 *************/
#include <sal/appl/sal.h>
#include <shared/bsl.h>
#include <shared/utilex/utilex_str.h>
#include "dbal_string_mgmt.h"

/*************
 * STATICS   *
 *************/
/* 
   Strings mapping array - will be removed in two stages:
   1. when auto-coder will be integrated
   2. effiecient hashing function
*/
static char unknown_string[DBAL_MAX_STRING_LENGTH] = "unknown_id";

extern char dbal_field_strings[DBAL_NOF_FIELDS][DBAL_MAX_STRING_LENGTH];
extern char dbal_table_strings[DBAL_NOF_TABLES][DBAL_MAX_STRING_LENGTH];
extern dbal_hw_entity_mapping dbal_hw_entities_strings[DBAL_NOF_HW_ENTITIES];


/* 
* Strings mapping array for dbal pyisical tables:
*/
static char dbal_physical_table_strings[DBAL_NOF_PHYSICAL_TABLES][DBAL_MAX_STRING_LENGTH] = {
  {"NONE"},
  {"TCAM"},
  {"LPM"},
  {"ISEM_1"},
  {"INLIF_1"},
  {"IVSI"},
  {"ISEM_2"},
  {"INLIF_2"},
  {"ISEM_3"},
  {"INLIF_3"},
  {"LEM"},
  {"IOEM_0"},
  {"IOEM_1"},
  {"MAP"},
  {"FEC_1"},
  {"FEC_2"},
  {"FEC_3"},
  {"MC_ID"},
  {"GLEM_0"},
  {"GLEM_1"},
  {"LL_1"},
  {"LL_2"},
  {"LL_3"},
  {"LL_4"},
  {"EEDB_1"},
  {"EEDB_2"},
  {"EEDB_3"},
  {"EEDB_4"},
  {"EOEM_0"},
  {"EOEM_1"},
  {"ESEM"},
  {"EVSI"},
  {"EXEM_1"},
  {"EXEM_2"},
  {"EXEM_3"},
  {"EXEM_4"},
  {"RMEP"}
};

/* 
* Strings mapping array for dbal labels:
*/
static char dbal_label_strings[DBAL_NOF_LABEL_TYPES][DBAL_MAX_STRING_LENGTH] = {
  {"NONE"},
  {"L2"},
  {"L3"},
  {"MPLS"},
  {"FCOE"},
  {"ALL"}
};

/* 
* Strings mapping array for dbal core modes:
*/
static char dbal_core_mode_strings[DBAL_NOF_CORE_MODE_TYPES][DBAL_MAX_STRING_LENGTH] = {
  {"NONE"},
  {"DPC"},
  {"SBC"}
};

/* 
* Strings mapping array for dbal condition types:
*/
static char dbal_condition_strings[DBAL_NOF_CONDITION_TYPES][DBAL_MAX_STRING_LENGTH] = {
  {"NONE"},
  {"BIGGER_THAN"},
  {"LOWER_THAN"},
  {"EQUAL_TO"},
  {"IS_EVEN"},
  {"IS_ODD"}
};

/* 
* Strings mapping array for dbal field types:
*/
static char dbal_field_type_strings[DBAL_NOF_FIELD_TYPES][DBAL_MAX_STRING_LENGTH] = {
  {"NONE"},
  {"BOOL"},
  {"INT32"},
  {"UINT32"},
  {"IP"},
  {"ARRAY8"},
  {"ARRAY32"}
};

/* 
* Strings mapping array for dbal encode types:
*/
static char dbal_encode_type_strings[DBAL_NOF_VALUE_ENCODE_TYPES][DBAL_MAX_STRING_LENGTH] = {
  {"NONE"},
  {"BOOL"},
  {"MODULO"},
  {"DIVIDE"},
  {"MULTIPLE"},
  {"PREFIX"},
  {"SUFFIX"},
  {"PARENT_FIELD"},
  {"SUBTRACT"},
  {"PARTIAL_KEY"},
  {"HARD_VALUE"}
};

/* 
* Strings mapping array for dbal action flags:
*/
static char dbal_action_flag_strings[DBAL_COMMIT_NOF_OPCODES][DBAL_MAX_STRING_LENGTH] = {
  {"NORMAL"},
  {"USE_DEFAULT"},
  {"KEEP_HANDLE"},
  {"OVERRUN_ENTRY"}
};

/* 
* Strings mapping array for dbal field modes:
*/
static char dbal_field_mode_strings[DBAL_NOF_FIELD_MODES][DBAL_MAX_STRING_LENGTH] = {
  {"IS_KEY"},
  {"IS_VALUE"}
};

/* 
* Strings mapping array for dbal access types:
*/
static char dbal_access_type_strings[DBAL_NOF_ACCESS_TYPES][DBAL_MAX_STRING_LENGTH] = {
  {"Phy table"},
  {"Direct"}
};

/*************
 * FUNCTIONS *
 *************/
shr_error_e
dbal_field_string_to_id(
  int unit,
  char *str,
  dbal_fields_e * field_id)
{
  dbal_fields_e i;

  SHR_FUNC_INIT_VARS(unit);

  for (i = 0; i < DBAL_NOF_FIELDS; i++)
  {
    if (sal_strcasecmp(dbal_field_strings[i], str) == 0)
    {
      *field_id = i;
      SHR_EXIT();
    }
  }

  SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Error. dbal_field_string_to_id: " "field not found (%s)\n", str);
exit:
  SHR_FUNC_EXIT;
}

shr_error_e
dbal_logical_table_string_to_id(
  int unit,
  char *str,
  dbal_tables_e * log_table_id)
{
  int i;

  SHR_FUNC_INIT_VARS(unit);

  for (i = 0; i < DBAL_NOF_TABLES; i++)
  {
    if (sal_strcasecmp(dbal_table_strings[i], str) == 0)
    {
      *log_table_id = (dbal_tables_e) i;
      SHR_EXIT();
    }
  }
  SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Error. dbal_logical_table_string_to_id: " "log table not found (%s)\n", str);
exit:
  SHR_FUNC_EXIT;
}

shr_error_e
dbal_hw_entity_string_to_id(
  int unit,
  char *str,
  int *hw_entity_id)
{
  int i;

  SHR_FUNC_INIT_VARS(unit);

  for (i = 0; i < DBAL_NOF_HW_ENTITIES; i++)
  {
    if (sal_strcasecmp(dbal_hw_entities_strings[i].hw_entity_name, str) == 0)
    {
      *hw_entity_id = dbal_hw_entities_strings[i].hw_entity_val;
      SHR_EXIT();
    }
  }
  SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Error. dbal_hw_entities_string_to_id: " "hw entity not found (%s)\n", str);
exit:
  SHR_FUNC_EXIT;
}

shr_error_e
dbal_physical_table_string_to_id(
  int unit,
  char *str,
  dbal_physical_tables_e * phy_table_id)
{
  int i;

  SHR_FUNC_INIT_VARS(unit);

  for (i = 0; i < DBAL_NOF_PHYSICAL_TABLES; i++)
  {
    if (sal_strcasecmp(dbal_physical_table_strings[i], str) == 0)
    {
      *phy_table_id = (dbal_physical_tables_e) i;
      SHR_EXIT();
    }
  }
  SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Error. dbal_physical_table_string_to_id: " "phy table not found (%s)\n", str);
exit:
  SHR_FUNC_EXIT;
}

shr_error_e
dbal_label_string_to_id(
  int unit,
  char *str,
  dbal_labels_e * label_id)
{
  int i;

  SHR_FUNC_INIT_VARS(unit);

  for (i = 0; i < DBAL_NOF_LABEL_TYPES; i++)
  {
    if (sal_strcasecmp(dbal_label_strings[i], str) == 0)
    {
      *label_id = (dbal_labels_e) i;
      SHR_EXIT();
    }
  }

  SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Error. dbal_label_string_to_id: " "label not found (%s)\n", str);
exit:
  SHR_FUNC_EXIT;
}

shr_error_e
dbal_core_mode_string_to_id(
  int unit,
  char *str,
  dbal_core_mode_e * core_mode_id)
{
  int i;

  SHR_FUNC_INIT_VARS(unit);

  for (i = 0; i < DBAL_NOF_CORE_MODE_TYPES; i++)
  {
    if (sal_strcasecmp(dbal_core_mode_strings[i], str) == 0)
    {
      *core_mode_id = (dbal_core_mode_e) i;
      SHR_EXIT();
    }
  }

  SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Error. dbal_core_mode_string_to_id: " "core mode not found (%s)\n", str);
exit:
  SHR_FUNC_EXIT;
}

shr_error_e
dbal_condition_string_to_id(
  int unit,
  char *str,
  dbal_condition_types_e * condition_id)
{
  int i;

  SHR_FUNC_INIT_VARS(unit);

  for (i = 0; i < DBAL_NOF_CONDITION_TYPES; i++)
  {
    if (sal_strcasecmp(dbal_condition_strings[i], str) == 0)
    {
      *condition_id = (dbal_condition_types_e) i;
      SHR_EXIT();
    }
  }

  SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Error. dbal_condition_string_to_id: " "condition type not found (%s)\n", str);
exit:
  SHR_FUNC_EXIT;
}

shr_error_e
dbal_field_type_string_to_id(
  int unit,
  char *str,
  dbal_field_type_e * field_type_id)
{
  int i;

  SHR_FUNC_INIT_VARS(unit);

  for (i = 0; i < DBAL_NOF_FIELD_TYPES; i++)
  {
    if (sal_strcasecmp(dbal_field_type_strings[i], str) == 0)
    {
      *field_type_id = (dbal_field_type_e) i;
      SHR_EXIT();
    }
  }

  SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Error. dbal_field_type_string_to_id: " "field type not found (%s)\n", str);
exit:
  SHR_FUNC_EXIT;
}

shr_error_e
dbal_encode_type_string_to_id(
  int unit,
  char *str,
  dbal_value_encode_types_e * encode_type_id)
{
  int i;

  SHR_FUNC_INIT_VARS(unit);

  for (i = 0; i < DBAL_NOF_VALUE_ENCODE_TYPES; i++)
  {
    if (sal_strcasecmp(dbal_encode_type_strings[i], str) == 0)
    {
      *encode_type_id = (dbal_value_encode_types_e) i;
      SHR_EXIT();
    }
  }

  SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Error. dbal_encode_type_string_to_id: " "encode type not found (%s)\n", str);
exit:
  SHR_FUNC_EXIT;
}

char *
dbal_field_to_string(
  int unit,
  dbal_fields_e field_id)
{
  if (field_id < DBAL_NOF_FIELDS)
  {
    return dbal_field_strings[field_id];
  }
  return unknown_string;
}

char *
dbal_logical_table_to_string(
  int unit,
  dbal_tables_e log_table_id)
{
  if (log_table_id < DBAL_NOF_TABLES)
  {
    return dbal_table_strings[log_table_id];
  }
  return unknown_string;
}

char *
dbal_hw_entity_to_string(
  int unit,
  int hw_entity_id)
{
  int i;

  for (i = 0; i < DBAL_NOF_HW_ENTITIES; i++)
  {
    if (dbal_hw_entities_strings[i].hw_entity_val == hw_entity_id)
    {
      return dbal_hw_entities_strings[i].hw_entity_name;
    }
  }
  return unknown_string;
}

char *
dbal_physical_table_to_string(
  int unit,
  dbal_physical_tables_e phy_table_id)
{
  if (phy_table_id < DBAL_NOF_PHYSICAL_TABLES)
  {
    return dbal_physical_table_strings[phy_table_id];
  }
  return unknown_string;
}

char *
dbal_label_to_string(
  int unit,
  dbal_labels_e label_id)
{
  if (label_id < DBAL_NOF_LABEL_TYPES)
  {
    return dbal_label_strings[label_id];
  }

  return unknown_string;
}

char *
dbal_core_mode_to_string(
  int unit,
  dbal_core_mode_e core_mode_id)
{
  if (core_mode_id < DBAL_NOF_CORE_MODE_TYPES)
  {
    return dbal_core_mode_strings[core_mode_id];
  }
  return unknown_string;
}

char *
dbal_condition_to_string(
  int unit,
  dbal_condition_types_e condition_id)
{
  if (condition_id < DBAL_NOF_CONDITION_TYPES)
  {
    return dbal_condition_strings[condition_id];
  }
  return unknown_string;
}

char *
dbal_field_type_to_string(
  int unit,
  dbal_field_type_e field_type_id)
{
  if (field_type_id < DBAL_NOF_FIELD_TYPES)
  {
    return dbal_field_type_strings[field_type_id];
  }
  return unknown_string;
}

char *
dbal_encode_type_to_string(
  int unit,
  dbal_value_encode_types_e encode_type_id)
{
  if (encode_type_id < DBAL_NOF_VALUE_ENCODE_TYPES)
  {
    return dbal_encode_type_strings[encode_type_id];
  }
  return unknown_string;
}

char *
dbal_action_flags_to_string(
  int unit,
  dbal_entry_action_flags_e action_flag_id)
{
  if (action_flag_id < DBAL_COMMIT_NOF_OPCODES)
  {
    return dbal_action_flag_strings[action_flag_id];
  }
  return unknown_string;
}

char *
dbal_key_mode_to_string(
  int unit,
  dbal_field_key_mode_e key_mode_id)
{
  if (key_mode_id < DBAL_NOF_FIELD_MODES)
  {
    return dbal_field_mode_strings[key_mode_id];
  }
  return unknown_string;
}

char *
dbal_access_type_to_string(
  int unit,
  dbal_access_type_e access_type_id)
{
  if (access_type_id < DBAL_NOF_ACCESS_TYPES)
  {
    return dbal_access_type_strings[access_type_id];
  }
  return unknown_string;
}



