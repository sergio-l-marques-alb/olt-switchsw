/*
 * $Id: bare_metal_dynamic_loading.c
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOC_COMMON

#include <sal/types.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_pp.h>
#include <soc/dnx/utils/dnx_pp_programmability_utils.h>
#include <soc/dnx/utils/bare_metal_dynamic_loading.h>

shr_error_e
dnx_arr_prefix_dynamic_file_get(
    int unit,
    char arr_prefix_dynamic_file[RHFILE_MAX_SIZE])
{
    char file_name[RHNAME_MAX_SIZE] = { 0 };
    char *image_name;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_pp_prgm_device_image_name_legacy_get(unit, &image_name));
    sal_strncat(file_name, "/dnx_data/auto_generated/arr/", RHNAME_MAX_SIZE - sal_strlen(file_name));
    sal_strncat(file_name, image_name, RHNAME_MAX_SIZE - sal_strlen(file_name));
    sal_strncat(file_name, "_arr.xml", RHNAME_MAX_SIZE - sal_strlen(file_name));
    sal_strncpy(arr_prefix_dynamic_file, file_name, RHNAME_MAX_SIZE);

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_aod_tables_dynamic_file_get(
    int unit,
    char aod_dynamic_file[RHFILE_MAX_SIZE])
{
    char file_name[RHNAME_MAX_SIZE] = { 0 };
    char *image_name;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_pp_prgm_device_image_name_legacy_get(unit, &image_name));
    sal_strncpy(file_name, "/dnx_data/auto_generated/aod/", RHNAME_MAX_SIZE);
    sal_strncat(file_name, image_name, RHNAME_MAX_SIZE - sal_strlen(file_name));
    sal_strncat(file_name, "_aod.xml", RHNAME_MAX_SIZE - sal_strlen(file_name));
    sal_strncpy(aod_dynamic_file, file_name, RHNAME_MAX_SIZE);

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_mdb_app_db_dynamic_file_get(
    int unit,
    char mdb_app_db_dynamic_file[RHFILE_MAX_SIZE])
{
    char file_name[RHNAME_MAX_SIZE] = { 0 };
    char *image_name;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_pp_prgm_device_image_name_legacy_get(unit, &image_name));
    sal_strncpy(file_name, "/dnx_data/auto_generated/mdb_app_db/", RHNAME_MAX_SIZE);
    sal_strncat(file_name, image_name, RHNAME_MAX_SIZE - sal_strlen(file_name));
    sal_strncat(file_name, "_mdb_app_db.xml", RHNAME_MAX_SIZE - sal_strlen(file_name));
    sal_strncpy(mdb_app_db_dynamic_file, file_name, RHNAME_MAX_SIZE);

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_pemla_soc_dynamic_file_get(
    int unit,
    char pemal_soc_file[RHFILE_MAX_SIZE])
{
    char file_name[RHNAME_MAX_SIZE] = { 0 };
    char *image_name;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_pp_prgm_device_image_name_legacy_get(unit, &image_name));
    sal_strncpy(file_name, "/dnx_data/auto_generated/pemla_soc/", RHNAME_MAX_SIZE);
    sal_strncat(file_name, image_name, RHNAME_MAX_SIZE - sal_strlen(file_name));
    sal_strncat(file_name, "_pemla_soc.xml", RHNAME_MAX_SIZE - sal_strlen(file_name));
    sal_strncpy(pemal_soc_file, file_name, RHNAME_MAX_SIZE);

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_aod_sizes_dynamic_file_get(
    int unit,
    char aod_sizes_file[RHFILE_MAX_SIZE])
{
    char file_name[RHNAME_MAX_SIZE] = { 0 };
    char *image_name;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_pp_prgm_device_image_name_legacy_get(unit, &image_name));
    sal_strncpy(file_name, "/dnx_data/auto_generated/aod_sizes/", RHNAME_MAX_SIZE);
    sal_strncat(file_name, image_name, RHNAME_MAX_SIZE - sal_strlen(file_name));
    sal_strncat(file_name, "_aod_sizes.xml", RHNAME_MAX_SIZE - sal_strlen(file_name));
    sal_strncpy(aod_sizes_file, file_name, RHNAME_MAX_SIZE);

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}
