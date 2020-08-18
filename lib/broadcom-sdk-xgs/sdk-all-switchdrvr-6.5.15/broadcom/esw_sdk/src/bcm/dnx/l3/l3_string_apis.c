/*
 * $Id: algo_string_apis.c,v 1.13 Broadcom SDK $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 */

#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_L3

#include <bcm_int/dnx/l3/l3_fec.h>
#include <shared/bsl.h>
#include <sal/appl/sal.h>
#include <soc/sand/sand_signals.h>

shr_error_e
dnx_cint_l3_fec_hierarchy_stage_map_set(
    int unit,
    int *bank_ids,
    char* hierarchy_name)
{
    dbal_field_types_basic_info_t *field_info;
    dbal_enum_value_field_hierarchy_level_e hierarchy = DBAL_NOF_ENUM_HIERARCHY_LEVEL_VALUES;
    uint8 enum_index;
    int str_len;

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dbal_fields_field_types_info_get(unit, DBAL_FIELD_HIERARCHY_LEVEL, &field_info));

    str_len = sal_strlen(field_info->name) + 1;

    for (enum_index = 0; enum_index < field_info->nof_enum_values; enum_index++)
    {
       if (sal_strcasecmp(hierarchy_name, &field_info->enum_val_info[enum_index].name[str_len]) == 0)
       {
           hierarchy = enum_index;
           break;
       }
    }
    if (enum_index == field_info->nof_enum_values)
    {
       SHR_ERR_EXIT(_SHR_E_PARAM, "%s field %s enum val wasn't found \n", field_info->name, hierarchy_name);
    }

    SHR_IF_ERR_EXIT(dnx_l3_fec_hierarchy_stage_map_set(unit, bank_ids, hierarchy));

exit:
    SHR_FUNC_EXIT;
}
