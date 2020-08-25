/** \file init_pemla.c
 *
 * DNX pemla init and deinit sequence functions.
 *
 */

/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_INITSEQDNX

/*
 * Include files.
 * {
 */
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>
#include <soc/dnx/drv.h>
#include <soc/dnx/pemladrv/pemladrv.h>
#include <shared/dbx/dbx_file.h>
#include <shared/utilex/utilex_str.h>
#include <include/soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include <soc/sand/sand_signals.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_pp.h>
#include <soc/dnx/kbp/kbp_common.h>

/*
 * }
 */

/*
 * DEFINEs
 * {
 */
/**
 * path to pemla ucode file
 * (relative to DB path folder)
 */
#define DB_INIT_PEMLA_UCODE_DIR                 "ucode"

/*
 * }
 */
extern shr_error_e dbal_pemla_verify_tables(
    int unit);

/** This function disables the KBP lookups in case the soc properties of the KBP are disabled. It is done be zeroing the
 *  aligner of the KBP key and the FFCs of the KBP KBRs. */
static shr_error_e
pemla_kbp_lookups_update(
    int unit)
{
    uint32 entry_handle_id;
    uint32 ffc_bitmap[2] = { 0 };

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (!dnx_data_elk.application.feature_get(unit, dnx_data_elk_application_ipv4))
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KLEAP_FWD12_ALIGNER_MAPPING, &entry_handle_id));

        /** IPv4 Private UC */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CONTEXT_PROFILE, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV4___PRIVATE_UC);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_KBP_KBR_IDX, DBAL_ENUM_FVAL_KBP_KBR_IDX_KEY_1);

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD_KEY_SIZE, INST_SINGLE, 0);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_KBP_KBR_IDX, DBAL_ENUM_FVAL_KBP_KBR_IDX_KEY_2);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        /** IPv4 MC Private W_BF_W_F2B */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CONTEXT_PROFILE, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV4___MC_PRIVATE_W_BF_W_F2B);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_KBP_KBR_IDX, DBAL_ENUM_FVAL_KBP_KBR_IDX_KEY_1);

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD_KEY_SIZE, INST_SINGLE, 0);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_KBP_KBR_IDX, DBAL_ENUM_FVAL_KBP_KBR_IDX_KEY_2);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        /** IPv4 MC Private WO_BF_W_F2B */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CONTEXT_PROFILE, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV4___MC_PRIVATE_WO_BF_W_F2B);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_KBP_KBR_IDX, DBAL_ENUM_FVAL_KBP_KBR_IDX_KEY_1);

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD_KEY_SIZE, INST_SINGLE, 0);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_KBP_KBR_IDX, DBAL_ENUM_FVAL_KBP_KBR_IDX_KEY_2);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        /** IPv4 MC Private WO_BF_WO_F2B */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CONTEXT_PROFILE, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV4___MC_PRIVATE_WO_BF_WO_F2B);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_KBP_KBR_IDX, DBAL_ENUM_FVAL_KBP_KBR_IDX_KEY_1);

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD_KEY_SIZE, INST_SINGLE, 0);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_KBP_KBR_IDX, DBAL_ENUM_FVAL_KBP_KBR_IDX_KEY_2);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));


        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KLEAP_FWD12_KBR_INFO, &entry_handle_id));

        /** IPv4 Private UC */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD12_KBR_ID, 4);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CONTEXT_PROFILE, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV4___PRIVATE_UC);
        dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_FFC_BITMAP, INST_SINGLE, ffc_bitmap);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD12_KBR_ID, 5);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        /** IPv4 MC Private W_BF_W_F2B */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD12_KBR_ID, 4);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CONTEXT_PROFILE, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV4___MC_PRIVATE_W_BF_W_F2B);
        dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_FFC_BITMAP, INST_SINGLE, ffc_bitmap);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD12_KBR_ID, 5);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        /** IPv4 MC Private WO_BF_W_F2B */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD12_KBR_ID, 4);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CONTEXT_PROFILE, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV4___MC_PRIVATE_WO_BF_W_F2B);
        dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_FFC_BITMAP, INST_SINGLE, ffc_bitmap);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD12_KBR_ID, 5);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        /** IPv4 MC Private WO_BF_WO_F2B */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD12_KBR_ID, 4);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CONTEXT_PROFILE, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV4___MC_PRIVATE_WO_BF_WO_F2B);
        dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_FFC_BITMAP, INST_SINGLE, ffc_bitmap);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD12_KBR_ID, 5);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

    if (!dnx_data_elk.application.feature_get(unit, dnx_data_elk_application_ipv6))
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KLEAP_FWD12_ALIGNER_MAPPING, &entry_handle_id));

        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CONTEXT_PROFILE,
                                   DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV6___PRIVATE_UC);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_KBP_KBR_IDX, DBAL_ENUM_FVAL_KBP_KBR_IDX_KEY_1);

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD_KEY_SIZE, INST_SINGLE, 0);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_KBP_KBR_IDX, DBAL_ENUM_FVAL_KBP_KBR_IDX_KEY_2);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
                /** should disable also IPv6 MC and IPv6 UC RPF programs currently contexts not exits
		 *  DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV6___MC_PRIVATE_WO_BF_WO_F2B,
		 * DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV6___MC_PRIVATE_WO_BF_W_F2B*/

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KLEAP_FWD12_KBR_INFO, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CONTEXT_PROFILE,
                                   DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV6___PRIVATE_UC);

        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD12_KBR_ID, 4);
        dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_FFC_BITMAP, INST_SINGLE, ffc_bitmap);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD12_KBR_ID, 5);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_init_pemla_custom_init(
    int unit,
    const char *ucode_file_name)
{
    char file_path[RHFILE_MAX_SIZE];
    char rel_file_path[RHFILE_MAX_SIZE];
    const char *ucode_rel_path;
    int copied_chars;

    SHR_FUNC_INIT_VARS(unit);

    if (sal_strncmp(ucode_file_name, EMPTY, 2) == 0)
    {
        ucode_rel_path = dnx_data_pp.application.ucode_get(unit)->relative_path;
    }
    else
    {
        ucode_rel_path = ucode_file_name;
    }

    copied_chars = sal_snprintf(rel_file_path, RHFILE_MAX_SIZE, "%s/%s", DB_INIT_PEMLA_UCODE_DIR, ucode_rel_path);

    if ((copied_chars < 0) || (copied_chars >= RHFILE_MAX_SIZE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "ERROR: Relative path to uCode file too long: %s.\n", rel_file_path);
    }

    SHR_IF_ERR_EXIT(dbx_file_get_ucode_location(unit, rel_file_path, file_path));

    /** Check that ucode file exists  */
    {
        FILE *file = fopen(file_path, "r");
        if (!file)
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "ucode file isn't exist: %s\n", file_path);
        }
        else
        {
            fclose(file);
        }
    }

    SHR_IF_ERR_EXIT(dnx_pemladrv_init(unit, SOC_WARM_BOOT(unit), file_path));
    /*
     * Reset signal, to update debug signals per stage allocation
     */
    SHR_IF_ERR_EXIT(sand_signal_reread(unit));

    
    SHR_IF_ERR_EXIT(dbal_pemla_verify_tables(unit));

    if (!SOC_WARM_BOOT(unit))
    {
        /** update the aligner for the KBP in case that KBP lookup is not valid */
        SHR_IF_ERR_EXIT(pemla_kbp_lookups_update(unit));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_init_pemla_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_init_pemla_custom_init(unit, EMPTY));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_init_pemla_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_pemladrv_deinit(unit));

exit:
    SHR_FUNC_EXIT;
}
