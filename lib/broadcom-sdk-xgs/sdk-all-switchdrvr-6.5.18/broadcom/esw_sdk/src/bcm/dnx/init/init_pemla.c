/** \file init_pemla.c
 *
 * DNX pemla init and deinit sequence functions.
 *
 */

/*
 * $Copyright: (c) 2019 Broadcom.
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
/** allow drv.h include explicitly */
#ifdef _SOC_DRV_H
#undef _SOC_DRV_H
#endif
#include <soc/drv.h> /** needed for soc property functions */
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnxc/swstate/dnxc_sw_state_utils.h>
#include <soc/dnx/pemladrv/pemladrv.h>
#include <shared/dbx/dbx_file.h>
#include <shared/utilex/utilex_str.h>
#include <include/soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include <soc/dnx/pemladrv/auto_generated/pemladrv.h>
#include <soc/sand/sand_signals.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_pp.h>
#include <soc/dnx/kbp/kbp_common.h>
#include <bcm_int/dnx/kbp/kbp_mngr.h>
#include <appl/diag/system.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_debug.h>

#ifdef BCM_SHARED_LIB_SDK
#include <src/sal/appl/pre_compiled_bridge_router_pemla_init_db.h>
#endif /* BCM_SHARED_LIB_SDK */

/*
 * }
 */

/*
 * DEFINEs
 * {
 */

/*
 * }
 */
extern void dbal_pemla_status_set(
    int unit,
    dbal_status_e status);

extern shr_error_e dbal_pemla_verify_tables(
    int unit);

#ifndef BCM_SHARED_LIB_SDK
static shr_error_e
soc_prop_verify(
    int unit,
    const char *file_name)
{
    FILE *fp = NULL;
    char line[512];             /* input line */
    int line_no = 0;
    int read_soc = 0;
    const char *fname;

    SHR_FUNC_INIT_VARS(unit);

    if (file_name != NULL)
    {
        fname = file_name;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "ERROR: ucode file name must be provided.\n");
    }

    if ((fp = fopen(fname, "r")) == NULL)
    {
        /*
         * printf("Can't open file '%s'. Exit.\n", file_name);
         */
        SHR_ERR_EXIT(_SHR_E_PARAM, "ERROR: failed to read from %s.\n", fname);
    }

    sal_memset(line, 0, sizeof(line));
    /*
     * Read one line per iteration and parse it 
     */
    while (fgets(line, sizeof(line), fp) != NULL)
    {
        /*
         * DBUG INFO
         */
        line_no++;
        {
            /*
             * Skip empty lines and comments 
             */
            if ((strlen(line) == 0) || (line[0] == '\n') || (line[0] == '#'))
            {
            }
            else if (strncmp(line, "/*PEMLA_SOC_START", sizeof("/*PEMLA_SOC_START") - 1) == 0)
            {
                read_soc = 1;
            }
            else if (strncmp(line, "PEMLA_SOC_END*/", sizeof("PEMLA_SOC_END*/") - 1) == 0)
            {
                break;
            }
            else if (read_soc)
            {
                char soc_name[SOC_PROPERTY_NAME_MAX];
                char soc_pemla_value[SOC_PROPERTY_VALUE_MAX];
                char *soc_sdk_value;
                sal_memset(soc_name, 0, sizeof(soc_name));
                sal_memset(soc_pemla_value, 0, sizeof(soc_pemla_value));
                if (sscanf(line, "%s %s", soc_name, soc_pemla_value) != 2)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Bad line format. Skip and continue with next line.\n");
                }
                else
                {
                    soc_sdk_value = soc_property_get_str(unit, soc_name);
                    if (soc_sdk_value)
                    {
                        if (sal_strcmp(soc_pemla_value, soc_sdk_value) != 0)
                        {
                            SHR_ERR_EXIT(_SHR_E_PARAM,
                                         "SOC property values not aligned. In PEMLA %s = %s, but in SDK it is %s \n",
                                         soc_name, soc_pemla_value, soc_sdk_value);
                        }
                    }
                    else
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM,
                                     "SOC property values not defined. In PEMLA %s = %s, but in SDK it is %s \n",
                                     soc_name, soc_pemla_value, soc_sdk_value);
                    }
                }
            }
        }
        sal_memset(line, 0, sizeof(line));
    }   /* End of while loop over all lines in file */

exit:
    if (fp)
    {
        fclose(fp);
    }
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_init_pemla_release_verify(
    int unit,
    const char *file_name)
{
    FILE *fp = NULL;
    const char *fname;
    char line[512];             /* input line */
    int line_no = 0;
    int read_sdk_version = 0;
    VersionInfo *pemla_version;
    char dummy[SOC_PROPERTY_NAME_MAX];
    char sdk_version_value[SOC_PROPERTY_VALUE_MAX];
    int sdk_match = 0;
    char chip_name[SOC_PROPERTY_VALUE_MAX];
    char chip_revision[SOC_PROPERTY_VALUE_MAX];
    char pemla_chip_name[SOC_PROPERTY_VALUE_MAX];
    char pemla_chip_revision[SOC_PROPERTY_VALUE_MAX];
    int chip_dev, pemla_chip_dev;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(dummy, 0, sizeof(dummy));
    sal_memset(sdk_version_value, 0, sizeof(sdk_version_value));
    sal_memset(chip_name, 0, sizeof(chip_name));
    sal_memset(chip_revision, 0, sizeof(chip_revision));
    sal_memset(pemla_chip_name, 0, sizeof(pemla_chip_name));
    sal_memset(pemla_chip_revision, 0, sizeof(pemla_chip_revision));

    pemla_version = dnx_get_version(unit, 0);

    if (file_name != NULL)
    {
        fname = file_name;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "ERROR: ucode file name must be provided.\n");
    }

    if ((fp = fopen(fname, "r")) == NULL)
    {
        /*
         * printf("Can't open file '%s'. Exit.\n", file_name);
         */
        SHR_ERR_EXIT(_SHR_E_PARAM, "ERROR: failed to read from %s.\n", fname);
    }

    sal_memset(line, 0, sizeof(line));
    /*
     * Read one line per iteration and parse it 
     */
    while (fgets(line, sizeof(line), fp) != NULL)
    {
        /*
         * DBUG INFO
         */
        line_no++;
        {
            if ((strlen(line) == 0) || (line[0] == '\n') || (line[0] == '#'))
            {
                /*
                 * Skip empty lines and comments 
                 */
            }
            else if (strncmp(line, "/*SDK_VERSION_START", sizeof("/*SDK_VERSION_START") - 1) == 0)
            {
                read_sdk_version = 1;
            }
            else if (strncmp(line, "SDK_VERSION_END*/", sizeof("SDK_VERSION_END*/") - 1) == 0)
            {
                read_sdk_version = 0;
                break;
            }
            else if (read_sdk_version)
            {
                if (sscanf(line, "%s %s", dummy, sdk_version_value) != 2)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Bad line format.\n");
                }
                else if (strncmp(sdk_version_value, _build_release, sizeof(_build_release) - 1) != 0)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "SDK version is not alidned: SDK(%s) != UCODE(%s).\n", _build_release,
                                 sdk_version_value);
                }
                else
                {
                    sdk_match = 1;
                }
            }
        }
        sal_memset(line, 0, sizeof(line));
    }

    if (!sdk_match)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Missing SDK version validation.\n");
    }

    /** construct file name using the device ID and revision */
    if (sscanf(soc_dev_name(unit), "BCM8%[^_]_%s", chip_name, chip_revision) != 2)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Bad device name format.\n");
    }
    chip_dev = sal_strtoul(chip_name, NULL, 16);
    chip_dev &= (0xFFFF << dnx_data_device.general.nof_sku_bits_get(unit));
    chip_revision[1] = '0';

    if (sscanf(pemla_version->device_str, "%[^_]_%s", pemla_chip_name, pemla_chip_revision) != 2)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Bad pemla device name format.\n");
    }
    pemla_chip_dev = sal_strtoul(pemla_chip_name, NULL, 16);
    /** change lower case letter do upper case */
    utilex_str_to_upper(pemla_chip_revision);

    if ((chip_dev != pemla_chip_dev)
        && (strncmp(chip_revision, pemla_chip_revision, sizeof(pemla_chip_revision) - 1) != 0))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Device version is not alidned: Device(%s) != UCODE(%s).\n",
                     soc_dev_name(unit), pemla_version->device_str);
    }

exit:
    if (fp)
    {
        fclose(fp);
    }
    SHR_FUNC_EXIT;
}
#endif /* BCM_SHARED_LIB_SDK */

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

    copied_chars = sal_snprintf(rel_file_path, RHFILE_MAX_SIZE, "%s", ucode_rel_path);

    if ((copied_chars < 0) || (copied_chars >= RHFILE_MAX_SIZE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "ERROR: Relative path to uCode file too long: %s.\n", rel_file_path);
    }

    /*
     * get ucode full path 
     */
    SHR_IF_ERR_EXIT(dbx_file_get_file_path(unit, rel_file_path, CONF_OPEN_PER_DEVICE, file_path));

#ifndef BCM_SHARED_LIB_SDK
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
#endif /* BCM_SHARED_LIB_SDK */

    SHR_IF_ERR_EXIT(dnx_pemladrv_init(unit, sw_state_is_warm_boot(unit), file_path));

#ifndef BCM_SHARED_LIB_SDK
    /*
     * Verify PEMLA and SDK versions
     */
    SHR_IF_ERR_EXIT(dnx_init_pemla_release_verify(unit, file_path));
    /*
     * verify soc_properties according to uCode 
     */
    SHR_IF_ERR_EXIT(soc_prop_verify(unit, file_path));
#endif /* BCM_SHARED_LIB_SDK */

    if (dnx_data_debug.hw_bug.feature_get(unit, dnx_data_debug_hw_bug_pem_vt_init_fix))
    {
        
        soc_reg_above_64_val_t ippf_reg_0310;
        uint32 field_17_17 = 0;
        SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, IPPF_REG_0310r, REG_PORT_ANY, 0, ippf_reg_0310));
        field_17_17 = soc_reg_above_64_field32_get(unit, IPPF_REG_0310r, ippf_reg_0310, FIELD_17_17f);
        if (field_17_17)
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG, "ucode contains PEM access on VTT1 - This is not allowed\n");
        }
    }

    /*
     * Reset signal, to update debug signals per stage allocation
     SHR_IF_ERR_EXIT(sand_signal_reread(unit));
     */

    
    SHR_IF_ERR_EXIT(dbal_pemla_verify_tables(unit));

    dbal_pemla_status_set(unit, DBAL_STATUS_DBAL_INIT_DONE);
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
