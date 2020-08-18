/** \file pp_stage.c
 *
 *  Created on: Oct 31, 2017
 *      Author: mf954075
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOC_COMMON

#include <sal/appl/sal.h>

#include <soc/dnx/pp_stage.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_pp.h>

#include <soc/dnx/dbal/dbal.h>
#include <soc/drv.h> 
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_tables.h>

char *
dnx_pp_stage_name(
    int unit,
    dnx_pp_stage_e pp_stage)
{
    if ((pp_stage < 0) || (pp_stage >= dnx_data_pp.stages.params_info_get(unit)->key_size[0]))
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Illegal pp stage:%d\n"), pp_stage));
        return NULL;
    }

    return dnx_data_pp.stages.params_get(unit, pp_stage)->name;
}

shr_error_e
dnx_pp_stage_string_to_id(
    int unit,
    char *pp_stage_name,
    dnx_pp_stage_e * pp_stage_id)
{
    dnx_pp_stage_e pp_stage_index;

    SHR_FUNC_INIT_VARS(unit);

    for (pp_stage_index = DNX_PP_STAGE_FIRST; pp_stage_index < DNX_PP_STAGE_NOF; pp_stage_index++)
    {
        if (sal_strcasecmp(dnx_data_pp.stages.params_get(unit, pp_stage_index)->name, pp_stage_name) == 0)
        {
            *pp_stage_id = pp_stage_index;
            SHR_EXIT();
        }
    }

    SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT;
}

char *
dnx_pp_stage_block_name(
    int unit,
    dnx_pp_stage_e pp_stage)
{
    if ((pp_stage < 0) || (pp_stage >= dnx_data_pp.stages.params_info_get(unit)->key_size[0]))
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Illegal pp stage:%d\n"), pp_stage));
        return NULL;
    }

    return dnx_data_pp.stages.params_get(unit, pp_stage)->block;
}

int
dnx_pp_stage_is_kleap(
    int unit,
    dnx_pp_stage_e pp_stage)
{
    if ((pp_stage < 0) || (pp_stage >= dnx_data_pp.stages.params_info_get(unit)->key_size[0]))
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Illegal pp stage:%d\n"), pp_stage));
        return 0;
    }

    return dnx_data_pp.stages.params_get(unit, pp_stage)->is_kleap;
}

typedef struct
{
    char *kbr_connect_n;
    dbal_tables_e table_id;
    char *stage_n[2];
} pp_klear_kbr_info_t;

/**
 * Defines the number of KBR where there is an option for configuring access to different phys db
 */
#define KBR_MAX_NUM     3

/**
 * Static data describing KBR where different physical DB(one of the pair) may be configured
 */
const static pp_klear_kbr_info_t pp_klear_kbr_info[KBR_MAX_NUM] = {
    {"VTT23", DBAL_TABLE_KLEAP_VT23_RESOURCES_MAPPING, {"VTT2", "VTT3"}},
    {"VTT45", DBAL_TABLE_KLEAP_VT45_RESOURCES_MAPPING, {"VTT4", "VTT5"}},
    {"FWD12", DBAL_TABLE_KLEAP_FWD12_RESOURCES_MAPPING, {"FWD1", "FWD2"}},
};

/*
 * see pp_stage.h
 */
shr_error_e
dnx_pp_stage_kbr_resolve(
    int unit,
    char *stage_n_in,
    char *stage_n_out)
{
    uint32 entry_handle_id;
    uint32 select;
    char **tokens = NULL;
    uint32 realtokens = 0;
    int i_kbr;
    uint32 kbr_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Verify input
     */
    tokens = utilex_str_split(stage_n_in, "_", 2, &realtokens);
    if (realtokens != 2 || (tokens == NULL))
    {
        LOG_DEBUG(BSL_LOG_MODULE, (BSL_META("Stage:%s has no KBR format\n"), stage_n_in));
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        SHR_EXIT();
    }

    for (i_kbr = 0; i_kbr < KBR_MAX_NUM; i_kbr++)
    {
        if (!sal_strcasecmp(tokens[0], pp_klear_kbr_info[i_kbr].kbr_connect_n))
            break;
    }

    if (i_kbr == KBR_MAX_NUM)
    {
        LOG_DEBUG(BSL_LOG_MODULE, (BSL_META("Stage:%s is not KBR supported\n"), stage_n_in));
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        SHR_EXIT();
    }

    /*
     * Once we are here - we have match to kbr
     */
    SHR_SET_CURRENT_ERR(utilex_str_stoul(tokens[1], &kbr_id));
    if (!SHR_FUNC_VAL_IS(_SHR_E_NONE))
    {
        LOG_INFO(BSL_LOG_MODULE, (BSL_META("Stage:%s is not KBR encoded\n"), stage_n_in));
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, pp_klear_kbr_info[i_kbr].table_id, &entry_handle_id));
    /** Set vlan_domain */
    if (!SOC_WARM_BOOT(unit))
    {
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_KBR_SELECTOR,
                                                            kbr_id, &select));

        if (select > 1)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Wrong select value:%d for kbr:%s",
                         select, pp_klear_kbr_info[i_kbr].kbr_connect_n);
        }

        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META("KBR Encoding:%s was resolved as:%s\n"),
                                  stage_n_in, pp_klear_kbr_info[i_kbr].stage_n[select]));

        sal_strncpy(stage_n_out, pp_klear_kbr_info[i_kbr].stage_n[select], RHNAME_MAX_SIZE - 1);
    }

exit:
    utilex_str_split_free(tokens, realtokens);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
