/**
 * \file ctest_dnx_kbp_combo.c Contains all of the KBP ctest combo APIs
 */
/*
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_KBPDNX
#include <shared/bsl.h>

 /*
  * Include files.
  * {
  */
#include <soc/dnx/dbal/dbal_dynamic.h>
#include <soc/sand/sand_signals.h>

#include <shared/bsl.h>
#include <sal/appl/sal.h>
#include <shared/bslnames.h>
#include <soc/mcm/memregs.h>
#include <bcm_int/dnx/kbp/kbp_mngr.h>
#include <appl/diag/diag.h>
#include <bcm_int/dnx_dispatch.h>
#include <soc/dnxc/swstate/dnxc_sw_state_utils.h>
#include <bcm_int/dnx/field/field.h>
#include <bcm_int/dnx/field/field_entry.h>
#include <bcm_int/dnx/field/field_group.h>
#include <bcm_int/dnx/field/field_presel.h>
#include <bcm_int/dnx/field/field_context.h>
#include <bcm_int/dnx/field/field_map.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_group_types.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_group_access.h>
#include <shared/dbx/dbx_xml.h>
#include <shared/dbx/dbx_file.h>

#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <shared/utilex/utilex_bitstream.h>
#include "src/soc/dnx/dbal/dbal_internal.h"

#include "ctest_dnx_kbp_combo.h"

#define CTEST_DNX_KBP_COMBO_DEFAULT_XML_FILE_PATH "kbp/ctest_dnx_kbp_combo_configurations.xml"
static char *dnx_kbp_combo_xml_file_path;

#define DNX_KBP_COMBO_XML_GET(_input_xml_) \
    dnx_kbp_combo_xml_file_path = ((_input_xml_ == NULL) || (_input_xml_[0] == '\0')) ? \
        CTEST_DNX_KBP_COMBO_DEFAULT_XML_FILE_PATH : _input_xml_

#define DNX_KBP_COMBO_API_EXECUTE_NO_ERR_EXIT(_api_, _info_struct_, _msg_, _status_) \
    do \
    { \
        SHR_SET_CURRENT_ERR(_api_); \
        if (SHR_GET_CURRENT_ERR() != _SHR_E_NONE) \
        { \
            DNX_KBP_COMBO_LOG(DNX_KBP_COMBO_LOG_ERROR, "\"%s\" " _msg_ "\n", (_info_struct_)->name); \
            (_info_struct_)->status = _status_; \
            SHR_SET_CURRENT_ERR(_SHR_E_NONE); \
            SHR_EXIT(); \
        } \
    } while (0)

#define DNX_KBP_COMBO_API_EXECUTE_NO_ERR_EXIT_ROLLBACK(_api_, _info_struct_, _msg_, _status_, _rollback_) \
    do \
    { \
        SHR_SET_CURRENT_ERR(_api_); \
        if (SHR_GET_CURRENT_ERR() != _SHR_E_NONE) \
        { \
            DNX_KBP_COMBO_LOG(DNX_KBP_COMBO_LOG_ERROR, "\"%s\" " _msg_ "\n", (_info_struct_)->name); \
            (_info_struct_)->status = _status_; \
            SHR_SET_CURRENT_ERR(_SHR_E_NONE); \
            SHR_IF_ERR_EXIT(_rollback_); \
            SHR_EXIT(); \
        } \
    } while (0)

#define DNX_KBP_COMBO_XML_NODE_GET(_root_, _node_, _node_name_) \
    do \
    { \
        _node_ = dbx_xml_child_get_first(_root_, _node_name_); \
        if (_node_ == NULL) \
        { \
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Cannot find %s node in file: %s\n", \
                    _node_name_, dnx_kbp_combo_xml_file_path); \
        } \
    } while (0)

#define DNX_KBP_COMBO_LOG_OPEN_OUTPUT_FILE(_file_path_) \
    do \
    { \
        g_output_file = fopen(_file_path_, "w"); \
        if (g_output_file == NULL) \
        { \
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Cannot open \"%s\" for writing\n", _file_path_); \
        } \
    } while(0)

#define DNX_KBP_COMBO_LOG_CLOSE_OUTPUT_FILE() \
    do \
    { \
        fclose(g_output_file); \
        g_output_file = NULL; \
    } while(0)

#define DNX_KBP_COMBO_LOG(_log_, ...) \
    do \
    { \
        if ((g_dnx_kbp_combo_log & DNX_KBP_COMBO_LOG_STDOUT) && (g_dnx_kbp_combo_log & (_log_))) { \
            sal_fprintf(stdout, "%s", g_dnx_kbp_combo_log_indent); \
            sal_fprintf(stdout, __VA_ARGS__); \
        } \
        if ((g_dnx_kbp_combo_log & DNX_KBP_COMBO_LOG_FILE) && (g_output_file != NULL)) { \
            sal_fprintf(g_output_file, "%s", g_dnx_kbp_combo_log_indent); \
            sal_fprintf(g_output_file, __VA_ARGS__); \
        } \
    } while(0)

#define DNX_KBP_COMBO_LOG_NO_INDENT(_log_, ...) \
    do \
    { \
        if ((g_dnx_kbp_combo_log & DNX_KBP_COMBO_LOG_STDOUT) && (g_dnx_kbp_combo_log & (_log_))) { \
            sal_fprintf(stdout, __VA_ARGS__); \
        } \
        if ((g_dnx_kbp_combo_log & DNX_KBP_COMBO_LOG_FILE) && (g_output_file != NULL)) { \
            sal_fprintf(g_output_file, __VA_ARGS__); \
        } \
    } while(0)

#define DNX_KBP_COMBO_LOG_INDENT_PUSH { g_dnx_kbp_combo_log_indent[g_dnx_kbp_combo_log_indent_index++] = '\t';
#define DNX_KBP_COMBO_LOG_INDENT_POP g_dnx_kbp_combo_log_indent[--g_dnx_kbp_combo_log_indent_index] = '\0'; }

static dnx_kbp_combo_log_e g_dnx_kbp_combo_log = DNX_KBP_COMBO_LOG_ALL;
static char g_dnx_kbp_combo_log_indent[16] = "";
static int g_dnx_kbp_combo_log_indent_index = 0;
static char g_entries_added_bmp[CTEST_DNX_KBP_COMBO_MAX_NOF_ENTRIES] = { 0 };

static const char group_status_str[DNX_KBP_COMBO_GROUP_STATUS_NOF][64] = {
    [DNX_KBP_COMBO_GROUP_STATUS_INVALID] = "Invalid",
    [DNX_KBP_COMBO_GROUP_STATUS_VALID] = "Valid",
    [DNX_KBP_COMBO_GROUP_STATUS_FAILED_XML_READ] = "Failed during XML read",
    [DNX_KBP_COMBO_GROUP_STATUS_FAILED_DBAL_TABLE_CREATION] = "Failed during DBAL table creation",
    [DNX_KBP_COMBO_GROUP_STATUS_FAILED_KBP_MNGR_DB_CONFIGURATION] = "Failed during KBP manager configuration"
};

static const char opcode_status_str[DNX_KBP_COMBO_OPCODE_STATUS_NOF][64] = {
    [DNX_KBP_COMBO_OPCODE_STATUS_INVALID] = "Invalid",
    [DNX_KBP_COMBO_OPCODE_STATUS_VALID] = "Valid",
    [DNX_KBP_COMBO_OPCODE_STATUS_FAILED_XML_READ] = "Failed during XML read",
    [DNX_KBP_COMBO_OPCODE_STATUS_FAILED_KBP_MNGR_CREATION] = "Failed during KBP manager creation",
    [DNX_KBP_COMBO_OPCODE_STATUS_FAILED_MASTER_KEY_OVERLAY_FIELDS_ADD] = "Failed during master key overlay fields add",
    [DNX_KBP_COMBO_OPCODE_STATUS_FAILED_MASTER_KEY_UPDATE] = "Failed during master key update",
    [DNX_KBP_COMBO_OPCODE_STATUS_FAILED_LOOKUPS_ADD] = "Failed during adding lookups"
};

static const char test_status_str[DNX_KBP_COMBO_TEST_STATUS_NOF][64] = {
    [DNX_KBP_COMBO_TEST_STATUS_INVALID] = "Invalid",
    [DNX_KBP_COMBO_TEST_STATUS_SKIPPED] = "Skipped",
    [DNX_KBP_COMBO_TEST_STATUS_IN_PROGRESS] = "In progress",
    [DNX_KBP_COMBO_TEST_STATUS_FAILED_STEP_PARSING] = "Failed during parsing step",
    [DNX_KBP_COMBO_TEST_STATUS_FAILED_ENTRY_MANAGEMENT] = "Failed during entry management",
    [DNX_KBP_COMBO_TEST_STATUS_FAILED_SEARCH] = "Failed during performing a search",
    [DNX_KBP_COMBO_TEST_STATUS_PASSED] = "Passed"
};

static const char env_status_str[DNX_KBP_COMBO_ENV_STATUS_NOF][64] = {
    [DNX_KBP_COMBO_ENV_STATUS_INVALID] = "Invalid",
    [DNX_KBP_COMBO_ENV_STATUS_INIT_SKIPPED] = "Initialization skipped",
    [DNX_KBP_COMBO_ENV_STATUS_INIT_IN_PROGRESS] = "Initialization in progress",
    [DNX_KBP_COMBO_ENV_STATUS_INIT_IN_PROGRESS_WITH_ERROR] = "Initialization in progress and encountered error",
    [DNX_KBP_COMBO_ENV_STATUS_SYNC_FAILED] = "KBP sync failed",
    [DNX_KBP_COMBO_ENV_STATUS_INIT_DONE] = "Initialized with no error",
    [DNX_KBP_COMBO_ENV_STATUS_INIT_DONE_WITH_ERROR] = "Initialized with error",
};

static dnx_kbp_combo_env_info_t g_env_info = { 0 };
static FILE *g_output_file;

/**
 * \brief
 * Read all key or result fields info for a group.
 */
static shr_error_e
dnx_kbp_combo_fields_xml_read(
    int unit,
    void *upper_node,
    dbal_table_field_input_info_t * field_info,
    int *nof_fields,
    dnx_kbp_combo_field_indication_e field_indication)
{
    void *fields_node;
    void *iter_node;

    char field_node_name[DNX_KBP_COMBO_FIELD_NOF][16] = { "KeyFields", "ResultFields", "OverlayFields" };
    char field_property[DNX_KBP_COMBO_FIELD_NOF][16] = { "IsRanged", "IsValid", "Offset" };
    char field_name[DBAL_MAX_STRING_LENGTH] = { 0 };
    char tmp_bool[8] = { 0 };
    int max_fields_allowed[DNX_KBP_COMBO_FIELD_NOF] = {
        DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP,
        DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS_PER_RES_TYPE,
        DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY
    };
    int tmp_xml_int_reader;

    SHR_FUNC_INIT_VARS(unit);
    DNX_KBP_COMBO_LOG_INDENT_PUSH;

    SHR_NULL_CHECK(upper_node, _SHR_E_PARAM, "upper_node");
    SHR_NULL_CHECK(field_info, _SHR_E_PARAM, "field_info");
    SHR_NULL_CHECK(nof_fields, _SHR_E_PARAM, "nof_fields");

    (*nof_fields) = 0;

    /** Get fields node */
    DNX_KBP_COMBO_XML_NODE_GET(upper_node, fields_node, field_node_name[field_indication]);

    /** Iterate over the fields and populate the field_info */
    RHDATA_ITERATOR(iter_node, fields_node, "Field")
    {
        /** Check if the number of fields exceed the number of available ones */
        if ((*nof_fields) > max_fields_allowed[field_indication])
        {
            SHR_ERR_EXIT(_SHR_E_RESOURCE,
                         "The number of allowed %s for a group in the XML file exceeds the maximum allowed %d\n",
                         field_node_name[field_indication], max_fields_allowed[field_indication]);
        }

        /** Clean the field info structure */
        sal_memset(&(field_info[(*nof_fields)]), 0x0, sizeof(dbal_table_field_input_info_t));

        /** Set the field ID */
        RHDATA_GET_STR_STOP(iter_node, "Name", field_name);
        if (dbal_field_string_to_id_no_error(unit, field_name, &(field_info[(*nof_fields)].field_id)))
        {
            SHR_IF_ERR_EXIT(dbal_fields_field_create(unit, DBAL_FIELD_TYPE_DEF_UINT, field_name,
                                                     &(field_info[(*nof_fields)].field_id)));
        }

        /** Set the field size */
        RHDATA_GET_INT_DEF(iter_node, "BitSize", tmp_xml_int_reader, 0);
        /** When field size is not specified or 0, get the size from the field */
        if (tmp_xml_int_reader == 0)
        {
            SHR_IF_ERR_EXIT(dbal_fields_max_size_get
                            (unit, field_info[(*nof_fields)].field_id, (uint32 *) &tmp_xml_int_reader));
        }
        field_info[(*nof_fields)].field_nof_bits = tmp_xml_int_reader;

        switch (field_indication)
        {
            case DNX_KBP_COMBO_FIELD_KEY:
            {
                /** For results, set is_valid_indication_needed indication */
                RHDATA_GET_STR_DEF(iter_node, field_property[field_indication], tmp_bool, "FALSE");
                if (!strncmp(tmp_bool, "TRUE", 8))
                {
                    field_info[(*nof_fields)].is_ranged = TRUE;
                }
                break;
            }
            case DNX_KBP_COMBO_FIELD_RESULT:
            {
                /** For keys, set is_ranged indication */
                RHDATA_GET_STR_DEF(iter_node, field_property[field_indication], tmp_bool, "FALSE");
                if (!strncmp(tmp_bool, "TRUE", 8))
                {
                    field_info[(*nof_fields)].is_valid_indication_needed = TRUE;
                }
                break;
            }
            case DNX_KBP_COMBO_FIELD_OVERLAY:
            {
                /** For overlay fields, set offset to arr_prefix */
                /*
                 * NOTE: Used only for overlay fields parsing
                 * The ctest combo env is not using the arr_prefix for its purpose when parsing the overlay fields.
                 * It is instead using it to pass back the overlay offset.
                 */
                int offset = DNX_KBP_OVERLAY_FIELD_INVALID;
                RHDATA_GET_INT_DEF(iter_node, field_property[field_indication], offset, DNX_KBP_OVERLAY_FIELD_INVALID);
                field_info[(*nof_fields)].arr_prefix = offset;
                sal_itoa(tmp_bool, offset, 10, TRUE, 3);
                break;
            }
            default:
            {
                /** Left empty. This should not be reached. */
            }
        }

        DNX_KBP_COMBO_LOG((field_indication == DNX_KBP_COMBO_FIELD_OVERLAY ?
                           DNX_KBP_COMBO_LOG_OPCODE : DNX_KBP_COMBO_LOG_GROUP),
                          "Field \"%s\" BitSize=%d %s=%s\n",
                          field_name, field_info[(*nof_fields)].field_nof_bits,
                          field_property[field_indication], tmp_bool);

        (*nof_fields)++;
    }

exit:
    DNX_KBP_COMBO_LOG_INDENT_POP;
    SHR_FUNC_EXIT;
}

/**
 *  \brief
 *  Read a single group configuration and apply it.
 *
 *  1. Read the group key fields.
 *  2. Read the group result fields.
 *  3. Create a DBAL table according to the key and result configurations.
 *  4. Configure the KBP DB according to the created DBAL table (Rollback on failure).
 */
static shr_error_e
dnx_kbp_combo_group_xml_read_configure(
    int unit,
    void *group_node,
    dnx_kbp_combo_group_info_t * group_info)
{
    int nof_key_fields;
    dbal_table_field_input_info_t key_field_info[DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP];
    int nof_res_fields;
    dbal_table_field_input_info_t res_field_info[DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS_PER_RES_TYPE];
    char tmp_string[DBAL_MAX_STRING_LENGTH] = { 0 };

    SHR_FUNC_INIT_VARS(unit);
    DNX_KBP_COMBO_LOG_INDENT_PUSH;

    SHR_NULL_CHECK(group_node, _SHR_E_PARAM, "group_node");
    SHR_NULL_CHECK(group_info, _SHR_E_PARAM, "group_info");

    /** Clean the group info structure */
    sal_memset(group_info, 0x0, sizeof(dnx_kbp_combo_group_info_t));

    /** Get group name */
    RHDATA_GET_STR_STOP(group_node, "Name", group_info->name);

    /** Check the valid indication of the group */
    RHDATA_GET_STR_DEF(group_node, "Valid", tmp_string, "TRUE");
    if (sal_strncmp(tmp_string, "TRUE", 16))
    {
        DNX_KBP_COMBO_LOG(DNX_KBP_COMBO_LOG_INFO | DNX_KBP_COMBO_LOG_GROUP, "Skip group \"%s\"\n", group_info->name);
        group_info->status = DNX_KBP_COMBO_GROUP_STATUS_INVALID;
        SHR_EXIT();
    }

    DNX_KBP_COMBO_LOG(DNX_KBP_COMBO_LOG_INFO | DNX_KBP_COMBO_LOG_GROUP, "Configure group \"%s\"\n", group_info->name);

    /** Get table type */
    RHDATA_GET_STR_DEF(group_node, "Type", tmp_string, "TCAM_BY_ID");
    SHR_IF_ERR_EXIT(dbal_logical_table_type_string_to_id(unit, tmp_string, &group_info->dbal_table_type));

    DNX_KBP_COMBO_LOG(DNX_KBP_COMBO_LOG_INFO | DNX_KBP_COMBO_LOG_GROUP, "Group type \"%s\"\n",
                      dbal_table_type_to_string(unit, group_info->dbal_table_type));

    /** Get key fields */
    DNX_KBP_COMBO_LOG(DNX_KBP_COMBO_LOG_GROUP, "KeyFields\n");
    DNX_KBP_COMBO_API_EXECUTE_NO_ERR_EXIT(dnx_kbp_combo_fields_xml_read
                                          (unit, group_node, key_field_info, &nof_key_fields, DNX_KBP_COMBO_FIELD_KEY),
                                          group_info, "failed XML read keys",
                                          DNX_KBP_COMBO_GROUP_STATUS_FAILED_XML_READ);

    /** Get result fields */
    DNX_KBP_COMBO_LOG(DNX_KBP_COMBO_LOG_GROUP, "ResultFields\n");
    DNX_KBP_COMBO_API_EXECUTE_NO_ERR_EXIT(dnx_kbp_combo_fields_xml_read
                                          (unit, group_node, res_field_info, &nof_res_fields,
                                           DNX_KBP_COMBO_FIELD_RESULT), group_info, "failed XML read results",
                                          DNX_KBP_COMBO_GROUP_STATUS_FAILED_XML_READ);

    /** Create DBAL table for the group using the parsed XML configuration */
    DNX_KBP_COMBO_API_EXECUTE_NO_ERR_EXIT(dbal_tables_table_create
                                          (unit, DBAL_ACCESS_METHOD_KBP, group_info->dbal_table_type,
                                           DBAL_CORE_MODE_SBC, nof_key_fields, key_field_info, nof_res_fields,
                                           res_field_info, group_info->name, &(group_info->dbal_table_id)), group_info,
                                          "failed DBAL table creation",
                                          DNX_KBP_COMBO_GROUP_STATUS_FAILED_DBAL_TABLE_CREATION);

    /** Create the KBP DB configuration for the group */
    DNX_KBP_COMBO_API_EXECUTE_NO_ERR_EXIT_ROLLBACK(kbp_mngr_db_create(unit, FALSE, group_info->dbal_table_id, 0,
                                                                      DNX_KBP_DB_PROPERTIES_ALGORITHMIC_MODE_DEFAULT_INDICATION),
                                                   group_info, "failed KBP DB configuration",
                                                   DNX_KBP_COMBO_GROUP_STATUS_FAILED_KBP_MNGR_DB_CONFIGURATION,
                                                   dbal_tables_table_destroy(unit, group_info->dbal_table_id));

    group_info->status = DNX_KBP_COMBO_GROUP_STATUS_VALID;

    DNX_KBP_COMBO_LOG(DNX_KBP_COMBO_LOG_INFO | DNX_KBP_COMBO_LOG_GROUP, "\n");

exit:
    DNX_KBP_COMBO_LOG_INDENT_POP;
    SHR_FUNC_EXIT;
}

/**
 *  \brief
 *  Read all group configurations and apply the configurations.
 *
 *  Each group is read and then configured before continuing with the next one.
 *  In case a group fails a read/configure step, a rollback on the group is performed
 *  and the group status in the combo env_info is updated according to the failure.
 */
static shr_error_e
dnx_kbp_combo_groups_xml_read_configure(
    int unit,
    void *root,
    dnx_kbp_combo_env_info_t * env_info)
{
    void *node;
    void *iter_node;

    SHR_FUNC_INIT_VARS(unit);
    DNX_KBP_COMBO_LOG_INDENT_PUSH;

    SHR_NULL_CHECK(root, _SHR_E_PARAM, "root");
    SHR_NULL_CHECK(env_info, _SHR_E_PARAM, "combo_env_info");

    DNX_KBP_COMBO_LOG(DNX_KBP_COMBO_LOG_INFO | DNX_KBP_COMBO_LOG_GROUP, "Configure all groups\n");

    /** Get groups node */
    DNX_KBP_COMBO_XML_NODE_GET(root, node, "GroupList");
    env_info->nof_groups = 0;

    /** Iterate over all groups */
    RHDATA_ITERATOR(iter_node, node, "Group")
    {
        /** Check the available combo test resources */
        if (env_info->nof_groups >= CTEST_DNX_KBP_COMBO_MAX_NOF_GROUPS)
        {
            SHR_ERR_EXIT(_SHR_E_RESOURCE, "The number of groups in the XML file exceeds the maximum available %d\n"
                         "Please update the define CTEST_DNX_KBP_COMBO_MAX_NOF_GROUPS "
                         "according to the number of groups used\n", CTEST_DNX_KBP_COMBO_MAX_NOF_GROUPS);
        }

        /** Read and configure a group */
        SHR_IF_ERR_EXIT(dnx_kbp_combo_group_xml_read_configure(unit, iter_node,
                                                               &(env_info->group_info[env_info->nof_groups])));

        if ((env_info->group_info[env_info->nof_groups].status != DNX_KBP_COMBO_GROUP_STATUS_VALID) &&
            (env_info->group_info[env_info->nof_groups].status != DNX_KBP_COMBO_GROUP_STATUS_INVALID))
        {
            env_info->status = DNX_KBP_COMBO_ENV_STATUS_INIT_IN_PROGRESS_WITH_ERROR;
        }

        env_info->nof_groups++;
    }

    DNX_KBP_COMBO_LOG(DNX_KBP_COMBO_LOG_INFO | DNX_KBP_COMBO_LOG_GROUP, "All groups configured successfully\n\n");

exit:
    DNX_KBP_COMBO_LOG_INDENT_POP;
    SHR_FUNC_EXIT;
}

/**
 *  \brief
 *  Rollback all groups configurations in case of KBP sync failure.
 */
static shr_error_e
dnx_kbp_combo_groups_rollback(
    int unit,
    dnx_kbp_combo_env_info_t * env_info)
{
    int index;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_KBP_COMBO_LOG_INDENT_PUSH;

    SHR_NULL_CHECK(env_info, _SHR_E_PARAM, "env_info");

    DNX_KBP_COMBO_LOG(DNX_KBP_COMBO_LOG_INFO | DNX_KBP_COMBO_LOG_GROUP, "Rollback all groups\n");

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_DB_INFO, &entry_handle_id));

    for (index = 0; index < env_info->nof_groups; index++)
    {
        if (env_info->group_info[index].status == DNX_KBP_COMBO_GROUP_STATUS_VALID)
        {
            uint32 db_id;

            /** Clear the KBP DB configuration */
            SHR_IF_ERR_EXIT(dbal_tables_app_db_id_get(unit, env_info->group_info[index].dbal_table_id, &db_id, NULL));
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DB_ID, db_id);
            SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

            /** Destroy the DBAL table for the KBP group */
            SHR_IF_ERR_EXIT(dbal_tables_table_destroy(unit, env_info->group_info[index].dbal_table_id));
        }
    }

exit:
    DNX_KBP_COMBO_LOG_INDENT_POP;
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 *  \brief
 *  Read all lookup configurations for an opcode.
 */
static shr_error_e
dnx_kbp_combo_opcode_lookups_xml_read(
    int unit,
    void *opcode_node,
    kbp_opcode_lookup_info_t * lookup_info,
    int *nof_lookups)
{
    void *lookups_node;
    void *iter_node;

    char table_name[DBAL_MAX_STRING_LENGTH] = { 0 };
    int tmp_xml_int_reader;

    SHR_FUNC_INIT_VARS(unit);
    DNX_KBP_COMBO_LOG_INDENT_PUSH;

    SHR_NULL_CHECK(opcode_node, _SHR_E_PARAM, "opcode_node");
    SHR_NULL_CHECK(lookup_info, _SHR_E_PARAM, "lookup_info");
    SHR_NULL_CHECK(nof_lookups, _SHR_E_PARAM, "nof_lookups");

    (*nof_lookups) = 0;

    /** Get lookups node */
    DNX_KBP_COMBO_XML_NODE_GET(opcode_node, lookups_node, "Lookups");

    /** Iterate over the lookups and populate the lookup_info */
    RHDATA_ITERATOR(iter_node, lookups_node, "Lookup")
    {
        /** Check if the lookups exceed the number available per opcode */
        if ((*nof_lookups) >= DNX_KBP_MAX_NOF_LOOKUPS)
        {
            SHR_ERR_EXIT(_SHR_E_RESOURCE,
                         "The number of lookups for an opcode in the XML file exceeds the maximum allowed %d\n",
                         DNX_KBP_MAX_NOF_LOOKUPS);
        }

        /** Clean the lookup info structure */
        sal_memset(&(lookup_info[(*nof_lookups)]), 0x0, sizeof(kbp_opcode_lookup_info_t));

        /** Set the lookup DBAL table ID */
        RHDATA_GET_STR_STOP(iter_node, "Group", table_name);
        SHR_IF_ERR_EXIT(dbal_logical_table_string_to_id(unit, table_name,
                                                        &(lookup_info[(*nof_lookups)].dbal_table_id)));

        /** Set the lookup result index */
        RHDATA_GET_INT_STOP(iter_node, "ResultIndex", tmp_xml_int_reader);
        lookup_info[(*nof_lookups)].result_index = tmp_xml_int_reader;

        /** Set the lookup result offset in the result buffer */
        RHDATA_GET_INT_STOP(iter_node, "ResultOffset", tmp_xml_int_reader);
        lookup_info[(*nof_lookups)].result_offset = tmp_xml_int_reader;

        DNX_KBP_COMBO_LOG(DNX_KBP_COMBO_LOG_OPCODE, "Lookup table \"%s\" ResultLocation=%d ResultOffset=%d\n",
                          table_name, lookup_info[(*nof_lookups)].result_index,
                          lookup_info[(*nof_lookups)].result_offset);

        (*nof_lookups)++;
    }

    if ((*nof_lookups) == 0)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "New opcode is created with no new lookups\n");
    }

exit:
    DNX_KBP_COMBO_LOG_INDENT_POP;
    SHR_FUNC_EXIT;
}

/**
 *  \brief
 *  Read and add the overlay fields, if any are present.
 */
static shr_error_e
dnx_kbp_combo_opcode_master_key_overlay_fields_add(
    int unit,
    void *opcode_node,
    uint8 opcode_id,
    dnx_kbp_combo_opcode_info_t * opcode_info)
{
    int iter;
    int nof_overlay_fields;
    dbal_table_field_input_info_t overlay_field_info[DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY];
    kbp_mngr_key_segment_t overlay_field[DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY] = { {0} };

    SHR_FUNC_INIT_VARS(unit);
    DNX_KBP_COMBO_LOG_INDENT_PUSH;

    SHR_NULL_CHECK(opcode_node, _SHR_E_PARAM, "opcode_node");

    /** Get result fields */
    DNX_KBP_COMBO_LOG(DNX_KBP_COMBO_LOG_OPCODE, "OverlayFields\n");
    DNX_KBP_COMBO_API_EXECUTE_NO_ERR_EXIT(dnx_kbp_combo_fields_xml_read
                                          (unit, opcode_node, overlay_field_info, &nof_overlay_fields,
                                           DNX_KBP_COMBO_FIELD_OVERLAY), opcode_info, "failed XML read overlay fields",
                                          DNX_KBP_COMBO_OPCODE_STATUS_FAILED_XML_READ);

    for (iter = 0; iter < nof_overlay_fields; iter++)
    {
        overlay_field[iter].is_overlay_field = TRUE;
        overlay_field[iter].overlay_offset_bytes = BITS2BYTES(overlay_field_info[iter].arr_prefix);
        overlay_field[iter].nof_bytes = BITS2BYTES(overlay_field_info[iter].field_nof_bits);
        sal_strncpy(overlay_field[iter].name, dbal_field_to_string(unit, overlay_field_info[iter].field_id),
                    DNX_KBP_KEY_SEGMENT_NAME_SIZE_IN_BYTES);
    }

    SHR_IF_ERR_EXIT(kbp_mngr_opcode_master_key_segments_add
                    (unit, opcode_id, DNX_KBP_INVALID_SEGMENT_ID, nof_overlay_fields, overlay_field));

exit:
    DNX_KBP_COMBO_LOG_INDENT_POP;
    SHR_FUNC_EXIT;
}

/**
 *  \brief
 *  Update the opcode master key according to the key segments of the lookup DBs.
 *  Update the lookup_info with the key segment name and the segment location in the newly updated master key.
 */
static shr_error_e
dnx_kbp_combo_opcode_master_key_update(
    int unit,
    uint8 opcode_id,
    kbp_opcode_lookup_info_t * lookup_info,
    int nof_lookups)
{
    int iter;
    kbp_mngr_key_segment_t mk_segment[DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY] = { {0} };
    kbp_mngr_key_segment_t new_mk_segment[DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY] = { {0} };
    int lookup_index;
    uint32 nof_mk_segments;
    uint32 nof_new_mk_segments = 0;
    const dbal_logical_table_t *table;

    SHR_FUNC_INIT_VARS(unit);
    DNX_KBP_COMBO_LOG_INDENT_PUSH;

    SHR_NULL_CHECK(lookup_info, _SHR_E_PARAM, "lookup_info");

    for (iter = 0; iter < DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY; iter++)
    {
        kbp_mngr_key_segment_t_init(unit, &mk_segment[iter]);
        kbp_mngr_key_segment_t_init(unit, &new_mk_segment[iter]);
    }

    /** Get the existing master key segments */
    SHR_IF_ERR_EXIT(kbp_mngr_opcode_master_key_get(unit, opcode_id, &nof_mk_segments, mk_segment));

    /*
     * Iterate over the new lookups in order to update the master key
     * and their segments info (nof_segments and key_segment_index)
     */
    for (lookup_index = 0; lookup_index < nof_lookups; lookup_index++)
    {
        int key_field_index;
        int mk_segment_index;

        /** Get the DBAL table info */
        SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, lookup_info[lookup_index].dbal_table_id, &table));

        lookup_info[lookup_index].nof_segments = table->nof_key_fields;

        /** Iterate over the DBAL table key fields */
        for (key_field_index = 0; key_field_index < table->nof_key_fields; key_field_index++)
        {
            int key_segment_location = -1; /** -1 means not found and needs to be added */
            const char *field_name = dbal_field_to_string(unit, table->keys_info[key_field_index].field_id);

            /** Check if the lookup key field is valid */
            if (!sal_strncmp(field_name, "unknown_id", DBAL_MAX_STRING_LENGTH))
            {
                SHR_ERR_EXIT(_SHR_E_CONFIG, "Invalid key field name for DBAL table %s on result index %d\n",
                             table->table_name, lookup_info[lookup_index].result_index);
            }

            /** Check if the lookup key field is present in the master key (shared) */
            for (mk_segment_index = 0; (mk_segment_index < nof_mk_segments)
                 && (key_segment_location == -1); mk_segment_index++)
            {
                if (!sal_strncmp(mk_segment[mk_segment_index].name, field_name, DNX_KBP_KEY_SEGMENT_NAME_SIZE_IN_BYTES))
                {
                    key_segment_location = mk_segment_index;
                }
            }
            /** Check if the lookup key field will be added with the new master key fields */
            for (mk_segment_index = 0; (mk_segment_index < nof_new_mk_segments)
                 && (key_segment_location == -1); mk_segment_index++)
            {
                if (!sal_strncmp(new_mk_segment[mk_segment_index].name, field_name,
                                 DNX_KBP_KEY_SEGMENT_NAME_SIZE_IN_BYTES))
                {
                    key_segment_location = nof_mk_segments + mk_segment_index;
                }
            }

            /** Add the new field to the master key if it is not shared */
            if (key_segment_location == -1)
            {
                key_segment_location = nof_mk_segments + nof_new_mk_segments;
                new_mk_segment[nof_new_mk_segments].nof_bytes =
                    BITS2BYTES(table->keys_info[key_field_index].field_nof_bits);
                sal_strncpy(new_mk_segment[nof_new_mk_segments].name, field_name,
                            DNX_KBP_KEY_SEGMENT_NAME_SIZE_IN_BYTES);
                DNX_KBP_COMBO_LOG(DNX_KBP_COMBO_LOG_OPCODE, "Master key %s\n",
                                  new_mk_segment[nof_new_mk_segments].name);
                nof_new_mk_segments++;
            }

            /** Update the location of the lookup key segment in the master key */
            lookup_info[lookup_index].key_segment_index[key_field_index] = key_segment_location;
        }
    }

    /** Update the opcode master key with the new key segments */
    SHR_IF_ERR_EXIT(kbp_mngr_opcode_master_key_segments_add
                    (unit, opcode_id, DNX_KBP_INVALID_SEGMENT_ID, nof_new_mk_segments, new_mk_segment));

exit:
    DNX_KBP_COMBO_LOG_INDENT_POP;
    SHR_FUNC_EXIT;
}

/**
 *  \brief
 *  Add lookups to an opcode.
 */
static shr_error_e
dnx_kbp_combo_opcode_lookups_add(
    int unit,
    uint8 opcode_id,
    kbp_opcode_lookup_info_t * lookup_info,
    int nof_lookups)
{
    int lookup_index;

    SHR_FUNC_INIT_VARS(unit);
    DNX_KBP_COMBO_LOG_INDENT_PUSH;

    SHR_NULL_CHECK(lookup_info, _SHR_E_PARAM, "lookup_info");

    /** Add the lookups */
    for (lookup_index = 0; lookup_index < nof_lookups; lookup_index++)
    {
        SHR_IF_ERR_EXIT(kbp_mngr_opcode_lookup_add(unit, opcode_id, &(lookup_info[lookup_index])));
    }

exit:
    DNX_KBP_COMBO_LOG_INDENT_POP;
    SHR_FUNC_EXIT;
}

/**
 *  \brief
 *  Read a single opcode configuration and apply it.
 *
 *  1. Read the lookup DBs info.
 *  2. Create new opcode from existing source opcode.
 *  3. Update the opcode master key segments and the lookup key segments (Rollback on failure).
 *  4. Add the new lookups to the opcode (Rollback on failure).
 */
static shr_error_e
dnx_kbp_combo_opcode_xml_read_configure(
    int unit,
    void *opcode_node,
    dnx_kbp_combo_opcode_info_t * opcode_info)
{
    char tmp_string[DBAL_MAX_STRING_LENGTH] = { 0 };
    uint32 source_opcode;
    uint8 acl_context;

    kbp_opcode_lookup_info_t lookup_info[DNX_KBP_MAX_NOF_LOOKUPS];
    int nof_lookups;

    SHR_FUNC_INIT_VARS(unit);
    DNX_KBP_COMBO_LOG_INDENT_PUSH;

    SHR_NULL_CHECK(opcode_node, _SHR_E_PARAM, "opcode_node");
    SHR_NULL_CHECK(opcode_info, _SHR_E_PARAM, "opcode_info");

    /** Clean the opcode info structure */
    sal_memset(opcode_info, 0x0, sizeof(dnx_kbp_combo_opcode_info_t));

    /** Get opcode name */
    RHDATA_GET_STR_STOP(opcode_node, "Name", opcode_info->name);
    RHDATA_GET_STR_DEF(opcode_node, "Valid", tmp_string, "TRUE");
    if (sal_strncmp(tmp_string, "TRUE", 16))
    {
        DNX_KBP_COMBO_LOG(DNX_KBP_COMBO_LOG_INFO | DNX_KBP_COMBO_LOG_GROUP, "Skip opcode \"%s\"\n", opcode_info->name);
        opcode_info->status = DNX_KBP_COMBO_OPCODE_STATUS_INVALID;
        SHR_EXIT();
    }
    DNX_KBP_COMBO_LOG(DNX_KBP_COMBO_LOG_INFO | DNX_KBP_COMBO_LOG_OPCODE,
                      "Configure opcode \"%s\"\n", opcode_info->name);

    /** Get the source opcode */
    RHDATA_GET_STR_STOP(opcode_node, "SourceOpcode", tmp_string);
    DNX_KBP_COMBO_API_EXECUTE_NO_ERR_EXIT(dbal_enum_string_to_id
                                          (unit, DBAL_FIELD_KBP_FWD_OPCODE, tmp_string, &source_opcode), opcode_info,
                                          "failed XML read: invalid source opcode",
                                          DNX_KBP_COMBO_OPCODE_STATUS_FAILED_XML_READ);

    /** Get the lookups */
    DNX_KBP_COMBO_LOG(DNX_KBP_COMBO_LOG_OPCODE, "Lookups\n");
    DNX_KBP_COMBO_API_EXECUTE_NO_ERR_EXIT(dnx_kbp_combo_opcode_lookups_xml_read
                                          (unit, opcode_node, lookup_info, &nof_lookups), opcode_info,
                                          "failed XML read: lookups", DNX_KBP_COMBO_OPCODE_STATUS_FAILED_XML_READ);

    RHDATA_GET_STR_DEF(opcode_node, "Update", tmp_string, "FALSE");
    if (sal_strncmp(tmp_string, "FALSE", 16))
    {
        /** Update the default FWD opcode */
        opcode_info->opcode_id = source_opcode;
    }
    else
    {
        /** Create the new opcode */
        DNX_KBP_COMBO_API_EXECUTE_NO_ERR_EXIT(kbp_mngr_opcode_create
                                              (unit, (uint8) source_opcode, opcode_info->name, &acl_context,
                                               &(opcode_info->opcode_id)), opcode_info, "failed KBP_MNGR creation",
                                              DNX_KBP_COMBO_OPCODE_STATUS_FAILED_KBP_MNGR_CREATION);
    }

    /** Read and add the overlay keys, in case any are present */
    DNX_KBP_COMBO_API_EXECUTE_NO_ERR_EXIT_ROLLBACK(dnx_kbp_combo_opcode_master_key_overlay_fields_add
                                                   (unit, opcode_node, opcode_info->opcode_id, opcode_info),
                                                   opcode_info, "failed adding master key overlay fields",
                                                   DNX_KBP_COMBO_OPCODE_STATUS_FAILED_MASTER_KEY_OVERLAY_FIELDS_ADD,
                                                   kbp_mngr_opcode_clear(unit, opcode_info->opcode_id));

    /** Update the opcode master key and the new lookups segments info */
    DNX_KBP_COMBO_API_EXECUTE_NO_ERR_EXIT_ROLLBACK(dnx_kbp_combo_opcode_master_key_update
                                                   (unit, opcode_info->opcode_id, lookup_info, nof_lookups),
                                                   opcode_info, "failed updating master key",
                                                   DNX_KBP_COMBO_OPCODE_STATUS_FAILED_MASTER_KEY_UPDATE,
                                                   kbp_mngr_opcode_clear(unit, opcode_info->opcode_id));

    /** Add the new lookups segments */
    DNX_KBP_COMBO_API_EXECUTE_NO_ERR_EXIT_ROLLBACK(dnx_kbp_combo_opcode_lookups_add
                                                   (unit, opcode_info->opcode_id, lookup_info, nof_lookups),
                                                   opcode_info, "failed adding the new lookups",
                                                   DNX_KBP_COMBO_OPCODE_STATUS_FAILED_LOOKUPS_ADD,
                                                   kbp_mngr_opcode_clear(unit, opcode_info->opcode_id));

    opcode_info->status = DNX_KBP_COMBO_OPCODE_STATUS_VALID;

    DNX_KBP_COMBO_LOG(DNX_KBP_COMBO_LOG_INFO | DNX_KBP_COMBO_LOG_OPCODE, "\n");

exit:
    DNX_KBP_COMBO_LOG_INDENT_POP;
    SHR_FUNC_EXIT;
}

/**
 *  \brief
 *  Read all opcode configurations and apply the configurations.
 *
 *  Each opcode is read and then configured before continuing with the next one.
 *  In case an opcode fails a read/configure step, a rollback on the opcode is performed
 *  and the opcode status in the combo env_info is updated according to the failure.
 */
static shr_error_e
dnx_kbp_combo_opcodes_xml_read_configure(
    int unit,
    void *root,
    dnx_kbp_combo_env_info_t * env_info)
{
    void *node;
    void *iter_node;

    SHR_FUNC_INIT_VARS(unit);
    DNX_KBP_COMBO_LOG_INDENT_PUSH;

    SHR_NULL_CHECK(root, _SHR_E_PARAM, "root");
    SHR_NULL_CHECK(env_info, _SHR_E_PARAM, "env_info");

    DNX_KBP_COMBO_LOG(DNX_KBP_COMBO_LOG_INFO | DNX_KBP_COMBO_LOG_OPCODE, "Configure all opcodes\n");

    /** Get opcodes node */
    DNX_KBP_COMBO_XML_NODE_GET(root, node, "NewOpcodes");
    env_info->nof_opcodes = 0;

    /** Iterate over all opcodes */
    RHDATA_ITERATOR(iter_node, node, "Opcode")
    {
        /** Check the available combo test resources */
        if (env_info->nof_opcodes >= CTEST_DNX_KBP_COMBO_MAX_NOF_OPCODES)
        {
            SHR_ERR_EXIT(_SHR_E_RESOURCE, "The number of new opcodes in the XML file exceeds the maximum available %d\n"
                         "Please update the define CTEST_DNX_KBP_COMBO_MAX_NOF_OPCODES "
                         "according to the number of new opcodes used\n", CTEST_DNX_KBP_COMBO_MAX_NOF_OPCODES);
        }

        /** Read and configure an opcode */
        SHR_IF_ERR_EXIT(dnx_kbp_combo_opcode_xml_read_configure(unit, iter_node,
                                                                &(env_info->opcode_info[env_info->nof_opcodes])));

        if ((env_info->opcode_info[env_info->nof_opcodes].status != DNX_KBP_COMBO_OPCODE_STATUS_VALID) &&
            (env_info->opcode_info[env_info->nof_opcodes].status != DNX_KBP_COMBO_OPCODE_STATUS_INVALID))
        {
            env_info->status = DNX_KBP_COMBO_ENV_STATUS_INIT_IN_PROGRESS_WITH_ERROR;
        }

        env_info->nof_opcodes++;
    }

    DNX_KBP_COMBO_LOG(DNX_KBP_COMBO_LOG_INFO | DNX_KBP_COMBO_LOG_GROUP, "All opcodes configured successfully\n\n");

exit:
    DNX_KBP_COMBO_LOG_INDENT_POP;
    SHR_FUNC_EXIT;
}

/**
 *  \brief
 *  Rollback all opcode configurations in case of KBP sync failure.
 */
static shr_error_e
dnx_kbp_combo_opcodes_rollback(
    int unit,
    dnx_kbp_combo_env_info_t * env_info)
{
    int index;

    SHR_FUNC_INIT_VARS(unit);
    DNX_KBP_COMBO_LOG_INDENT_PUSH;

    SHR_NULL_CHECK(env_info, _SHR_E_PARAM, "env_info");

    DNX_KBP_COMBO_LOG(DNX_KBP_COMBO_LOG_INFO | DNX_KBP_COMBO_LOG_OPCODE, "Rollback all opcodes\n");

    for (index = 0; index < env_info->nof_opcodes; index++)
    {
        if (env_info->opcode_info[index].status == DNX_KBP_COMBO_OPCODE_STATUS_VALID)
        {
            /** Clear the KBP opcode configuration */
            SHR_IF_ERR_EXIT(kbp_mngr_opcode_clear(unit, env_info->opcode_info[index].opcode_id));
        }
    }

exit:
    DNX_KBP_COMBO_LOG_INDENT_POP;
    SHR_FUNC_EXIT;
}

/**
 *  \brief
 *  Read all group and opcode configurations and apply the configurations.
 *
 *  Each group/opcode is read and then configured before continuing with the next one.
 *  In case a group/opcode fails a read/configure step, a rollback on the group/opcode is performed
 *  and the group/opcode status in the combo env_info is updated according to the failure.
 */
static shr_error_e
dnx_kbp_combo_env_config_xml_read_apply(
    int unit,
    void *root,
    dnx_kbp_combo_env_info_t * env_info)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_KBP_COMBO_LOG_INDENT_PUSH;

    SHR_NULL_CHECK(root, _SHR_E_PARAM, "root");
    SHR_NULL_CHECK(env_info, _SHR_E_PARAM, "env_info");

    env_info->status = DNX_KBP_COMBO_ENV_STATUS_INIT_IN_PROGRESS;

    /** Read the XML and configure the groups */
    SHR_IF_ERR_EXIT(dnx_kbp_combo_groups_xml_read_configure(unit, root, env_info));

    /** Read the XML and configure the opcodes */
    SHR_IF_ERR_EXIT(dnx_kbp_combo_opcodes_xml_read_configure(unit, root, env_info));

    /** Perform KBP sync */
    SHR_SET_CURRENT_ERR(kbp_mngr_sync(unit));
    if (SHR_GET_CURRENT_ERR() != _SHR_E_NONE)
    {
        /** Rollback: Delete all group and opcode configurations */
        SHR_IF_ERR_EXIT(dnx_kbp_combo_groups_rollback(unit, env_info));
        SHR_IF_ERR_EXIT(dnx_kbp_combo_opcodes_rollback(unit, env_info));
        env_info->status = DNX_KBP_COMBO_ENV_STATUS_SYNC_FAILED;
        SHR_SET_CURRENT_ERR(_SHR_E_CONFIG);
        SHR_EXIT();
    }

    /** Update the status to DONE if the status has not been updated from IN_PROGRESS */
    env_info->status = (env_info->status == DNX_KBP_COMBO_ENV_STATUS_INIT_IN_PROGRESS) ?
        DNX_KBP_COMBO_ENV_STATUS_INIT_DONE : DNX_KBP_COMBO_ENV_STATUS_INIT_DONE_WITH_ERROR;

exit:
    DNX_KBP_COMBO_LOG_INDENT_POP;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Find an entry or search by their ID in the XML configuration file. Return the XML node to be used by upper APIs.
 */
static shr_error_e
dnx_kbp_combo_node_id_get(
    int unit,
    uint8 is_search_indication,
    int id,
    void *list,
    void **node)
{
    int get_id;
    void *iter_node;
    char id_node_name[2][8] = { "Entry", "Search" };
    uint8 id_found = FALSE;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(list, _SHR_E_PARAM, "list");
    SHR_NULL_CHECK(node, _SHR_E_PARAM, "node");
    *node = NULL;

    /** Update the input indication in order to use it as array index */
    is_search_indication = (is_search_indication ? 1 : 0);

    /** Iterate over all entries/searches in order to find the requested ID */
    RHDATA_ITERATOR(iter_node, list, id_node_name[is_search_indication])
    {
        RHDATA_GET_INT_STOP(iter_node, "ID", get_id);
        if (id == get_id)
        {
            id_found = TRUE;
            break;
        }
    }

    if (id_found == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "%sID %d was not found\n", id_node_name[is_search_indication], id);
    }

    *node = iter_node;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Parse the entry add/update expectation type from XML and return an appropriate enum
 */
static shr_error_e
dnx_kbp_combo_entry_add_expectation_parse(
    int unit,
    void *step_node,
    dnx_kbp_combo_entry_add_expectation_e * expectation)
{
    int index;
    char expectation_string_get[16] = "";
    char expectation_strings[4][16] = { "PASS", "ERROR", "EXISTS", "NOT_FOUND" };
    dnx_kbp_combo_entry_add_expectation_e expectations[4] = {
        DNX_KBP_COMBO_ENTRY_ADD_EXPECTATION_PASS,
        DNX_KBP_COMBO_ENTRY_ADD_EXPECTATION_ERROR,
        DNX_KBP_COMBO_ENTRY_ADD_EXPECTATION_EXISTS,
        DNX_KBP_COMBO_ENTRY_ADD_EXPECTATION_NOT_FOUND
    };

    SHR_FUNC_INIT_VARS(unit);
    DNX_KBP_COMBO_LOG_INDENT_PUSH;

    SHR_NULL_CHECK(step_node, _SHR_E_PARAM, "step_node");
    SHR_NULL_CHECK(expectation, _SHR_E_PARAM, "expectation");

    (*expectation) = DNX_KBP_COMBO_EXECUTE_ACTION_INVALID;

    RHDATA_GET_STR_DEF(step_node, "Expectation", expectation_string_get, "PASS");

    for (index = 0; index < 5; index++)
    {
        if (!sal_strncmp(expectation_string_get, expectation_strings[index], 16))
        {
            (*expectation) = expectations[index];
            SHR_EXIT();
        }
    }

    SHR_ERR_EXIT(_SHR_E_CONFIG, "Entry add/update expectation \"%s\" is not valid\n", expectation_string_get);

exit:
    DNX_KBP_COMBO_LOG_INDENT_POP;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Parse the entry get expectation type from XML and return an appropriate enum
 */
static shr_error_e
dnx_kbp_combo_entry_get_expectation_parse(
    int unit,
    void *step_node,
    dnx_kbp_combo_entry_get_expectation_e * expectation)
{
    int index;
    char expectation_string_get[16] = "";
    char expectation_strings[4][16] = { "FOUND", "NOT_FOUND", "VALIDATE", "VALIDATE_ERROR" };
    dnx_kbp_combo_entry_get_expectation_e expectations[4] = {
        DNX_KBP_COMBO_ENTRY_GET_EXPECTATION_FOUND,
        DNX_KBP_COMBO_ENTRY_GET_EXPECTATION_NOT_FOUND,
        DNX_KBP_COMBO_ENTRY_GET_EXPECTATION_VALIDATE,
        DNX_KBP_COMBO_ENTRY_GET_EXPECTATION_VALIDATE_ERROR
    };

    SHR_FUNC_INIT_VARS(unit);
    DNX_KBP_COMBO_LOG_INDENT_PUSH;

    SHR_NULL_CHECK(step_node, _SHR_E_PARAM, "step_node");
    SHR_NULL_CHECK(expectation, _SHR_E_PARAM, "expectation");

    (*expectation) = DNX_KBP_COMBO_EXECUTE_ACTION_INVALID;

    RHDATA_GET_STR_DEF(step_node, "Expectation", expectation_string_get, "FOUND");

    for (index = 0; index < 5; index++)
    {
        if (!sal_strncmp(expectation_string_get, expectation_strings[index], 16))
        {
            (*expectation) = expectations[index];
            SHR_EXIT();
        }
    }

    SHR_ERR_EXIT(_SHR_E_CONFIG, "Entry get expectation \"%s\" is not valid\n", expectation_string_get);

exit:
    DNX_KBP_COMBO_LOG_INDENT_POP;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Parse the entry delete expectation type from XML and return an appropriate enum
 */
static shr_error_e
dnx_kbp_combo_entry_delete_expectation_parse(
    int unit,
    void *step_node,
    dnx_kbp_combo_entry_delete_expectation_e * expectation)
{
    int index;
    char expectation_string_get[16] = "";
    char expectation_strings[3][16] = { "PASS", "ERROR", "NOT_FOUND" };
    dnx_kbp_combo_entry_delete_expectation_e expectations[3] = {
        DNX_KBP_COMBO_ENTRY_DELETE_EXPECTATION_PASS,
        DNX_KBP_COMBO_ENTRY_DELETE_EXPECTATION_ERROR,
        DNX_KBP_COMBO_ENTRY_DELETE_EXPECTATION_NOT_FOUND
    };

    SHR_FUNC_INIT_VARS(unit);
    DNX_KBP_COMBO_LOG_INDENT_PUSH;

    SHR_NULL_CHECK(step_node, _SHR_E_PARAM, "step_node");
    SHR_NULL_CHECK(expectation, _SHR_E_PARAM, "expectation");

    (*expectation) = DNX_KBP_COMBO_EXECUTE_ACTION_INVALID;

    RHDATA_GET_STR_DEF(step_node, "Expectation", expectation_string_get, "PASS");

    for (index = 0; index < 4; index++)
    {
        if (!sal_strncmp(expectation_string_get, expectation_strings[index], 16))
        {
            (*expectation) = expectations[index];
            SHR_EXIT();
        }
    }

    SHR_ERR_EXIT(_SHR_E_CONFIG, "Entry add/update expectation \"%s\" is not valid\n", expectation_string_get);

exit:
    DNX_KBP_COMBO_LOG_INDENT_POP;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Parse and set the entry access ID from the XML configuration file.
 */
static shr_error_e
dnx_kbp_combo_entry_access_id_parse_action(
    int unit,
    void *entry_node,
    uint32 entry_handle_id)
{
    int access_id;

    SHR_FUNC_INIT_VARS(unit);
    DNX_KBP_COMBO_LOG_INDENT_PUSH;

    SHR_NULL_CHECK(entry_node, _SHR_E_PARAM, "entry_node");

    RHDATA_GET_INT_DEF(entry_node, "AccessID", access_id, 0);
    if (access_id != 0)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, access_id));
        DNX_KBP_COMBO_LOG(DNX_KBP_COMBO_LOG_ENTRY, "Access ID %d\n", access_id);
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "AccessID is not set\n");
    }

exit:
    DNX_KBP_COMBO_LOG_INDENT_POP;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Parse and set the entry access ID from the XML configuration file.
 */
static shr_error_e
dnx_kbp_combo_entry_access_id_and_priority_dump_info(
    int unit,
    void *entry_node)
{
    int access_id;
    int priority;

    SHR_FUNC_INIT_VARS(unit);
    DNX_KBP_COMBO_LOG_INDENT_PUSH;

    SHR_NULL_CHECK(entry_node, _SHR_E_PARAM, "entry_node");

    RHDATA_GET_INT_DEF(entry_node, "AccessID", access_id, -1);
    RHDATA_GET_INT_DEF(entry_node, "Priority", priority, -1);
    if (access_id != -1)
    {
        DNX_KBP_COMBO_LOG(DNX_KBP_COMBO_LOG_ENTRY, "AccessID %d", access_id);
        if (priority != -1)
        {
            DNX_KBP_COMBO_LOG_NO_INDENT(DNX_KBP_COMBO_LOG_ENTRY, "\tPriority %d", priority);
        }
        DNX_KBP_COMBO_LOG_NO_INDENT(DNX_KBP_COMBO_LOG_ENTRY, "\n");
    }
    else if (priority != -1)
    {
        DNX_KBP_COMBO_LOG(DNX_KBP_COMBO_LOG_ENTRY, "Priority %d\n", priority);
    }

exit:
    DNX_KBP_COMBO_LOG_INDENT_POP;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Parse and return input entry key type and values.
 */
shr_error_e
dnx_kbp_combo_entry_key_type_values_parse(
    int unit,
    void *key_field_node,
    dnx_kbp_combo_entry_key_value_type_e * type,
    uint32 out_value[2])
{
    int index;
    int value[4] = { 0 };
    uint8 prop_bmp = 0;
    char field_property[4][16] = { "Value", "Mask", "Min", "Max" };
    char tmp_bool[2][8] = { "FALSE", "TRUE" };

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(key_field_node, _SHR_E_PARAM, "key_field_node");
    SHR_NULL_CHECK(type, _SHR_E_PARAM, "type");

    (*type) = DNX_KBP_COMBO_ENTRY_KEY_VALUE_TYPE_NOF;

    /*
     * Iterate over the valid key field properties, get their values and and set a bitmap of their usage
     */
    for (index = 0; index < 4; index++)
    {
        RHDATA_GET_INT_CONT(key_field_node, field_property[index], value[index]);
        prop_bmp |= (1 << index);
    }

    if (prop_bmp == 0x1 /** Value only */ )
    {
        /** Value only */
        *type = DNX_KBP_COMBO_ENTRY_KEY_VALUE_TYPE_VALUE_ONLY;
        out_value[0] = value[0];
    }
    else if (prop_bmp == 0x3 /** Value and Mask */ )
    {
        /** Value and mask */
        *type = DNX_KBP_COMBO_ENTRY_KEY_VALUE_TYPE_VALUE_AND_MASK;
        out_value[0] = value[0];
        out_value[1] = value[1];
    }
    else if (prop_bmp == 0xc /** Min and Max */ )
    {
        /** Range */
        *type = DNX_KBP_COMBO_ENTRY_KEY_VALUE_TYPE_RANGE;
        out_value[0] = value[2];
        out_value[1] = value[3];

    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "Combination of invalid entry key properties was provided: "
                     "Value=%s Mask=%s Min=%s Max=%s. Valid combinations are Valid, Valid + Mask, Min + Max\n",
                     tmp_bool[prop_bmp & 0x1], tmp_bool[prop_bmp & 0x2],
                     tmp_bool[prop_bmp & 0x4], tmp_bool[prop_bmp & 0x8]);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Parse a single entry result field from the XML configuration file.
 * Add the result field to a new entry or validate the result against existing entry.
 */
static shr_error_e
dnx_kbp_combo_entry_key_field_manage(
    int unit,
    dnx_kbp_combo_entry_field_e action,
    void *field_node,
    uint32 entry_handle_id,
    dbal_tables_e dbal_table_id)
{
    dbal_fields_e field_id;
    char field_name[DBAL_MAX_STRING_LENGTH] = "";
    dnx_kbp_combo_entry_key_value_type_e type;
    uint32 value[2];
    uint32 get_value[2];

    SHR_FUNC_INIT_VARS(unit);

    /** Get the field name */
    RHDATA_GET_STR_STOP(field_node, "Name", field_name);
    SHR_IF_ERR_EXIT(dbal_field_string_to_id(unit, field_name, &field_id));

    /** Parse the entry input type and values */
    SHR_IF_ERR_EXIT(dnx_kbp_combo_entry_key_type_values_parse(unit, field_node, &type, value));

    /** Execute the appropriate set or get+compare command according to the key value type */
    switch (type)
    {
        case DNX_KBP_COMBO_ENTRY_KEY_VALUE_TYPE_VALUE_ONLY:
        {
            /** Value only */
            DNX_KBP_COMBO_LOG(DNX_KBP_COMBO_LOG_ENTRY, "KeyField \"%s\" Value=0x%08X (%d)", field_name, value[0],
                              value[0]);
            if (action == DNX_KBP_COMBO_ENTRY_FIELD_ADD)
            {
                dbal_entry_key_field32_set(unit, entry_handle_id, field_id, value[0]);
            }
            else if (action == DNX_KBP_COMBO_ENTRY_FIELD_VALIDATE)
            {
                SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, field_id, &get_value[0]));
                if (value[0] == get_value[0])
                {
                    DNX_KBP_COMBO_LOG_NO_INDENT(DNX_KBP_COMBO_LOG_ENTRY, " as expected");
                }
                else
                {
                    DNX_KBP_COMBO_LOG_NO_INDENT(DNX_KBP_COMBO_LOG_ENTRY,
                                                " not the same as the received Value=0x%08X (%d)\n", get_value[0],
                                                get_value[0]);
                    SHR_ERR_EXIT(_SHR_E_FAIL, "Key field %s failed validation\n", field_name);
                }
            }
            break;
        }
        case DNX_KBP_COMBO_ENTRY_KEY_VALUE_TYPE_VALUE_AND_MASK:
        {
            /** Value and mask */
            DNX_KBP_COMBO_LOG(DNX_KBP_COMBO_LOG_ENTRY,
                              "KeyField \"%s\" Value=0x%08X (%d) Mask=0x%08X (%d)", field_name, value[0], value[0],
                              value[1], value[1]);
            if (action == DNX_KBP_COMBO_ENTRY_FIELD_ADD)
            {
                dbal_entry_key_field32_masked_set(unit, entry_handle_id, field_id, value[0], value[1]);
            }
            else if (action == DNX_KBP_COMBO_ENTRY_FIELD_VALIDATE)
            {
                SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_masked_get
                                (unit, entry_handle_id, field_id, &get_value[0], &get_value[1]));
                if ((value[0] == get_value[0]) && (value[1] == get_value[1]))
                {
                    DNX_KBP_COMBO_LOG_NO_INDENT(DNX_KBP_COMBO_LOG_ENTRY, " as expected");
                }
                else
                {
                    DNX_KBP_COMBO_LOG_NO_INDENT(DNX_KBP_COMBO_LOG_ENTRY,
                                                " not the same as the received Value=0x%08X (%d) Mask=0x%08X (%d)\n",
                                                get_value[0], get_value[0], get_value[1], get_value[1]);
                    SHR_ERR_EXIT(_SHR_E_FAIL, "Key field %s failed validation\n", field_name);
                }
            }
            break;
        }
        case DNX_KBP_COMBO_ENTRY_KEY_VALUE_TYPE_RANGE:
        {
            /** Range */
            DNX_KBP_COMBO_LOG(DNX_KBP_COMBO_LOG_ENTRY,
                              "KeyField \"%s\" Min=0x%08X (%d) Max=0x%08X (%d)", field_name, value[0], value[0],
                              value[1], value[1]);
            if (action == DNX_KBP_COMBO_ENTRY_FIELD_ADD)
            {
                dbal_entry_key_field32_range_set(unit, entry_handle_id, field_id, value[0], value[1]);
            }
            else if (action == DNX_KBP_COMBO_ENTRY_FIELD_VALIDATE)
            {
                SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_range_get
                                (unit, entry_handle_id, field_id, &get_value[0], &get_value[1]));
                if ((value[0] == get_value[0]) && (value[1] == get_value[1]))
                {
                    DNX_KBP_COMBO_LOG_NO_INDENT(DNX_KBP_COMBO_LOG_ENTRY, " as expected");
                }
                else
                {
                    DNX_KBP_COMBO_LOG_NO_INDENT(DNX_KBP_COMBO_LOG_ENTRY,
                                                " not the same as the received Min=0x%08X (%d) Max=0x%08X (%d)\n",
                                                get_value[0], get_value[0], get_value[1], get_value[1]);
                    SHR_ERR_EXIT(_SHR_E_FAIL, "Key field %s failed validation\n", field_name);
                }
            }
            break;
        }
        default:
        {
            /** Left empty. This should not be reached. */
        }
    }
    DNX_KBP_COMBO_LOG_NO_INDENT(DNX_KBP_COMBO_LOG_ENTRY, "\n");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Parse and set the entry key fields from the XML configuration file.
 */
static shr_error_e
dnx_kbp_combo_entry_keys_manage(
    int unit,
    dnx_kbp_combo_entry_field_e action,
    void *entry_node,
    uint32 entry_handle_id,
    dbal_tables_e dbal_table_id)
{
    void *key_fields_node;
    void *iter_node;

    SHR_FUNC_INIT_VARS(unit);
    DNX_KBP_COMBO_LOG_INDENT_PUSH;

    SHR_NULL_CHECK(entry_node, _SHR_E_PARAM, "entry_node");

    /** Get entry key fields node */
    DNX_KBP_COMBO_XML_NODE_GET(entry_node, key_fields_node, "KeyFields");

    RHDATA_ITERATOR(iter_node, key_fields_node, "Field")
    {
        SHR_IF_ERR_EXIT(dnx_kbp_combo_entry_key_field_manage(unit, action, iter_node, entry_handle_id, dbal_table_id));
    }

exit:
    DNX_KBP_COMBO_LOG_INDENT_POP;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Parse a single entry result field from the XML configuration file.
 * Add the result field to a new entry or validate the result against existing entry.
 */
static shr_error_e
dnx_kbp_combo_entry_result_field_manage(
    int unit,
    dnx_kbp_combo_entry_field_e action,
    void *field_node,
    uint32 entry_handle_id,
    dbal_tables_e dbal_table_id)
{
    dbal_fields_e field_id;
    char field_name[DBAL_MAX_STRING_LENGTH] = "";
    uint32 value = 0;
    uint32 get_value;
    int tmp_value;
    char str_value[DBAL_MAX_STRING_LENGTH] = "";

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(field_node, _SHR_E_PARAM, "field_node");

    /** Get the field name */
    RHDATA_GET_STR_STOP(field_node, "Name", field_name);
    SHR_IF_ERR_EXIT(dbal_field_string_to_id(unit, field_name, &field_id));

    /** Get the field from XML */
    if (field_id == DBAL_FIELD_RESULT_TYPE)
    {
        RHDATA_GET_STR_STOP(field_node, "Value", str_value);
        if (action != DNX_KBP_COMBO_ENTRY_FIELD_INFO)
        {
            dbal_result_type_string_to_id(unit, dbal_table_id, str_value, &value);
        }
        DNX_KBP_COMBO_LOG(DNX_KBP_COMBO_LOG_ENTRY,
                          "ResultField \"%s\" Value=%s - 0x%08X (%d)", field_name, str_value, value, value);
    }
    else
    {
        RHDATA_GET_INT_STOP(field_node, "Value", tmp_value);
        value = tmp_value;
        DNX_KBP_COMBO_LOG(DNX_KBP_COMBO_LOG_ENTRY, "ResultField \"%s\" Value=0x%08X (%d)", field_name, value, value);
    }

    if (action == DNX_KBP_COMBO_ENTRY_FIELD_ADD)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, field_id, INST_SINGLE, value);
    }
    else if (action == DNX_KBP_COMBO_ENTRY_FIELD_VALIDATE)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, field_id, INST_SINGLE, &get_value));

        if (value == get_value)
        {
            DNX_KBP_COMBO_LOG_NO_INDENT(DNX_KBP_COMBO_LOG_ENTRY, " as expected");
        }
        else
        {
            DNX_KBP_COMBO_LOG_NO_INDENT(DNX_KBP_COMBO_LOG_ENTRY,
                                        " not the same as the received Value=0x%08X (%d)\n", get_value, get_value);
            SHR_ERR_EXIT(_SHR_E_FAIL, "Result field %s failed validation\n", field_name);
        }
    }
    DNX_KBP_COMBO_LOG_NO_INDENT(DNX_KBP_COMBO_LOG_ENTRY, "\n");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Parse and set the entry result fields from the XML configuration file.
 */
static shr_error_e
dnx_kbp_combo_entry_results_manage(
    int unit,
    dnx_kbp_combo_entry_field_e action,
    void *entry_node,
    uint32 entry_handle_id,
    dbal_tables_e dbal_table_id)
{
    void *res_fields_node;
    void *iter_node;

    SHR_FUNC_INIT_VARS(unit);
    DNX_KBP_COMBO_LOG_INDENT_PUSH;

    SHR_NULL_CHECK(entry_node, _SHR_E_PARAM, "entry_node");

    /** Get entry result fields node */
    DNX_KBP_COMBO_XML_NODE_GET(entry_node, res_fields_node, "ResultFields");

    RHDATA_ITERATOR(iter_node, res_fields_node, "Field")
    {
        SHR_IF_ERR_EXIT(dnx_kbp_combo_entry_result_field_manage
                        (unit, action, iter_node, entry_handle_id, dbal_table_id));
    }

exit:
    DNX_KBP_COMBO_LOG_INDENT_POP;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Parse the entry results from XML and validate them against the results from the entry get
 */
static shr_error_e
dnx_kbp_combo_entry_get_validate(
    int unit,
    void *entry_node,
    uint32 entry_handle_id,
    dbal_tables_e dbal_table_id)
{
    void *fields_node;
    void *iter_node;
    int priority;
    uint32 get_priority;
    const dbal_logical_table_t *table;

    SHR_FUNC_INIT_VARS(unit);
    DNX_KBP_COMBO_LOG_INDENT_PUSH;

    SHR_NULL_CHECK(entry_node, _SHR_E_PARAM, "entry_node");

    /** Get the table info */
    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, dbal_table_id, &table));

    /** Request the priority if the table type is TCAM */
    if (table->table_type == DBAL_TABLE_TYPE_TCAM || table->table_type == DBAL_TABLE_TYPE_TCAM_BY_ID)
    {
        SHR_IF_ERR_EXIT(dbal_entry_attribute_request(unit, entry_handle_id, DBAL_ENTRY_ATTR_PRIORITY, &get_priority));
    }

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    /** Validate the priority and the key fields if the table type is TCAM */
    if (table->table_type == DBAL_TABLE_TYPE_TCAM || table->table_type == DBAL_TABLE_TYPE_TCAM_BY_ID)
    {
        RHDATA_GET_INT_STOP(entry_node, "Priority", priority);
        if (priority != get_priority)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "The get priority %d is not the same as the expected priority %d\n",
                         get_priority, priority);
        }

        /** Get entry key fields node */
        DNX_KBP_COMBO_XML_NODE_GET(entry_node, fields_node, "KeyFields");

        /** Validate each key field with the values from the XML */
        RHDATA_ITERATOR(iter_node, fields_node, "Field")
        {
            SHR_IF_ERR_EXIT(dnx_kbp_combo_entry_key_field_manage(unit, DNX_KBP_COMBO_ENTRY_FIELD_VALIDATE, iter_node,
                                                                 entry_handle_id, dbal_table_id));
        }
    }

    /** Get entry result fields node */
    DNX_KBP_COMBO_XML_NODE_GET(entry_node, fields_node, "ResultFields");

    /** Validate each retrieved result field with the values from the XML */
    RHDATA_ITERATOR(iter_node, fields_node, "Field")
    {
        SHR_IF_ERR_EXIT(dnx_kbp_combo_entry_result_field_manage(unit, DNX_KBP_COMBO_ENTRY_FIELD_VALIDATE, iter_node,
                                                                entry_handle_id, dbal_table_id));
    }

exit:
    DNX_KBP_COMBO_LOG_INDENT_POP;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Execute entry add. Parse and set the key and result fields. For ACL entries parse and set priority and access ID.
 */
static shr_error_e
dnx_kbp_combo_entry_execute_add_action(
    int unit,
    dnx_kbp_combo_test_action_e action,
    void *step_node,
    void *entries_root,
    uint8 is_update)
{
    int id;
    void *entry_node;
    char table_name[DBAL_MAX_STRING_LENGTH];
    uint32 dbal_table_id;
    uint32 entry_handle_id = 0;
    const dbal_logical_table_t *table = NULL;
    dnx_kbp_combo_entry_field_e field_action = (action == DNX_KBP_COMBO_TEST_ACTION_EXECUTE) ?
        DNX_KBP_COMBO_ENTRY_FIELD_ADD : DNX_KBP_COMBO_ENTRY_FIELD_INFO;
    dnx_kbp_combo_entry_add_expectation_e expectation = 0;
    char expect_str[DNX_KBP_COMBO_ENTRY_ADD_EXPECTATION_NOF][16] = {
        "INVALID", "PASS", "ERROR", "EXISTS", "NOT_FOUND"
    };

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_KBP_COMBO_LOG_INDENT_PUSH;

    /** Get the entry node from the XML file */
    RHDATA_GET_INT_STOP(step_node, "EntryID", id);
    if (id >= CTEST_DNX_KBP_COMBO_MAX_NOF_ENTRIES)
    {
        SHR_ERR_EXIT(_SHR_E_RESOURCE, "Entry with ID %d exceeds the maximum available %d\n"
                     "Please update the define CTEST_DNX_KBP_COMBO_MAX_NOF_ENTRIES "
                     "according to the number of entries used\n", id, CTEST_DNX_KBP_COMBO_MAX_NOF_ENTRIES);
    }

    DNX_KBP_COMBO_LOG(DNX_KBP_COMBO_LOG_ENTRY, "Entry %s ID %d ", is_update ? "update" : "add", id);
    SHR_IF_ERR_EXIT(dnx_kbp_combo_node_id_get(unit, FALSE, id, entries_root, &entry_node));

    /** Get the entry table name */
    RHDATA_GET_STR_STOP(entry_node, "Table", table_name);
    DNX_KBP_COMBO_LOG_NO_INDENT(DNX_KBP_COMBO_LOG_ENTRY, "to table \"%s\"\n", table_name);

    /** Parse the validation expectations */
    SHR_IF_ERR_EXIT(dnx_kbp_combo_entry_add_expectation_parse(unit, step_node, &expectation));

    if ((!is_update && expectation == DNX_KBP_COMBO_ENTRY_ADD_EXPECTATION_NOT_FOUND) ||
        (is_update && expectation == DNX_KBP_COMBO_ENTRY_ADD_EXPECTATION_EXISTS))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Performing entry \"%s\" but expecting \"%s\" error\n",
                     is_update ? "UPDATE" : "ADD", expect_str[expectation]);
    }
    DNX_KBP_COMBO_LOG(DNX_KBP_COMBO_LOG_ENTRY, "Expect %s %s\n", is_update ? "update" : "add", expect_str[expectation]);

    if (action == DNX_KBP_COMBO_TEST_ACTION_EXECUTE)
    {
        /** Get the DBAL table ID and allocate handle for it */
        SHR_IF_ERR_EXIT(dbal_logical_table_string_to_id(unit, table_name, &dbal_table_id));
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));

        /** Get the table info */
        SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, dbal_table_id, &table));

        /** Set entry priority if the table type is TCAM */
        if (table->table_type == DBAL_TABLE_TYPE_TCAM || table->table_type == DBAL_TABLE_TYPE_TCAM_BY_ID)
        {
            int priority;
            RHDATA_GET_INT_DEF(entry_node, "Priority", priority, 0);
            SHR_IF_ERR_EXIT(dbal_entry_attribute_set(unit, entry_handle_id, DBAL_ENTRY_ATTR_PRIORITY, priority));
            DNX_KBP_COMBO_LOG(DNX_KBP_COMBO_LOG_ENTRY, "\tPriority %d\n", priority);
        }
        /** Set entry access ID if the table type is TCAM by ID */
        if (table->table_type == DBAL_TABLE_TYPE_TCAM_BY_ID)
        {
            SHR_IF_ERR_EXIT(dnx_kbp_combo_entry_access_id_parse_action(unit, entry_node, entry_handle_id));
        }
    }
    else
    {
        /** Handle the AccessID and Priority for dump info separately */
        SHR_IF_ERR_EXIT(dnx_kbp_combo_entry_access_id_and_priority_dump_info(unit, entry_node));
    }

    /** Parse and set the key fields for the entry */
    SHR_IF_ERR_EXIT(dnx_kbp_combo_entry_keys_manage(unit, field_action, entry_node, entry_handle_id, dbal_table_id));

    /** Parse and set the result fields for the entry */
    SHR_IF_ERR_EXIT(dnx_kbp_combo_entry_results_manage(unit, field_action, entry_node, entry_handle_id, dbal_table_id));

    if (action == DNX_KBP_COMBO_TEST_ACTION_EXECUTE)
    {
        /** Add the entry */
        SHR_SET_CURRENT_ERR(dbal_entry_commit(unit, entry_handle_id, is_update ? DBAL_COMMIT_UPDATE : DBAL_COMMIT));

        /** Validate the add action according to the expectation */
        if ((expectation == DNX_KBP_COMBO_ENTRY_ADD_EXPECTATION_PASS) && (SHR_GET_CURRENT_ERR() != _SHR_E_NONE))
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Expected \"%s\" but got \"%s\" for entry ID %d %s expectation\n",
                         _SHR_ERRMSG(_SHR_E_NONE), _SHR_ERRMSG(SHR_GET_CURRENT_ERR()),
                         id, is_update ? "update" : "add");
        }
        else if ((expectation == DNX_KBP_COMBO_ENTRY_ADD_EXPECTATION_EXISTS) &&
                 (SHR_GET_CURRENT_ERR() != _SHR_E_EXISTS))
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Expected \"%s\" but got \"%s\" for entry ID %d %s expectation\n",
                         _SHR_ERRMSG(_SHR_E_EXISTS), _SHR_ERRMSG(SHR_GET_CURRENT_ERR()),
                         id, is_update ? "update" : "add");
        }
        else if ((expectation == DNX_KBP_COMBO_ENTRY_ADD_EXPECTATION_NOT_FOUND) &&
                 (SHR_GET_CURRENT_ERR() != _SHR_E_NOT_FOUND))
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Expected \"%s\" but got \"%s\" for entry ID %d %s expectation\n",
                         _SHR_ERRMSG(_SHR_E_NOT_FOUND), _SHR_ERRMSG(SHR_GET_CURRENT_ERR()),
                         id, is_update ? "update" : "add");
        }
        else if ((expectation == DNX_KBP_COMBO_ENTRY_ADD_EXPECTATION_ERROR) && (SHR_GET_CURRENT_ERR() == _SHR_E_NONE))
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Expected any error but got \"%s\" for entry ID %d %s expectation\n",
                         _SHR_ERRMSG(SHR_GET_CURRENT_ERR()), id, is_update ? "update" : "add");
        }

        SHR_SET_CURRENT_ERR(_SHR_E_NONE);
        if (expectation == DNX_KBP_COMBO_ENTRY_ADD_EXPECTATION_PASS)
        {
            if (is_update)
            {
                int old_entry_id = -1;
                RHDATA_GET_INT_DEF(entry_node, "OldEntryId", old_entry_id, -1);
                if (old_entry_id != -1)
                {
                    g_entries_added_bmp[old_entry_id] = FALSE;
                }
            }
            g_entries_added_bmp[id] = TRUE;
        }

        DNX_KBP_COMBO_LOG(DNX_KBP_COMBO_LOG_ENTRY,
                          "Entry %s ID %d expectation %s passed\n", is_update ? "update" : "add", id,
                          expect_str[expectation]);
    }

exit:
    DNX_KBP_COMBO_LOG_INDENT_POP;
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \ brief
 * Execute entry get. Validate if it is added. (not yet implemented)
 */
static shr_error_e
dnx_kbp_combo_entry_execute_get_action(
    int unit,
    dnx_kbp_combo_test_action_e action,
    void *step_node,
    void *entries_root)
{
    int id;
    void *entry_node;
    char table_name[DBAL_MAX_STRING_LENGTH];
    uint32 dbal_table_id;
    uint32 entry_handle_id = 0;
    const dbal_logical_table_t *table;
    dnx_kbp_combo_entry_field_e field_action = (action == DNX_KBP_COMBO_TEST_ACTION_EXECUTE) ?
        DNX_KBP_COMBO_ENTRY_FIELD_ADD : DNX_KBP_COMBO_ENTRY_FIELD_INFO;
    dnx_kbp_combo_entry_get_expectation_e expectation = 0;
    char expect_str[DNX_KBP_COMBO_ENTRY_GET_EXPECTATION_NOF][16] = {
        "INVALID", "FOUND", "NOT_FOUND", "VALIDATE", "VALIDATE_ERROR"
    };

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_KBP_COMBO_LOG_INDENT_PUSH;

    /** Get the entry node from the XML file */
    RHDATA_GET_INT_STOP(step_node, "EntryID", id);
    if (id >= CTEST_DNX_KBP_COMBO_MAX_NOF_ENTRIES)
    {
        SHR_ERR_EXIT(_SHR_E_RESOURCE, "Entry with ID %d exceeds the maximum available %d\n"
                     "Please update the define CTEST_DNX_KBP_COMBO_MAX_NOF_ENTRIES "
                     "according to the number of entries used\n", id, CTEST_DNX_KBP_COMBO_MAX_NOF_ENTRIES);
    }

    DNX_KBP_COMBO_LOG(DNX_KBP_COMBO_LOG_ENTRY, "Entry get ID %d ", id);
    SHR_IF_ERR_EXIT(dnx_kbp_combo_node_id_get(unit, FALSE, id, entries_root, &entry_node));

    /** Get the entry table name */
    RHDATA_GET_STR_STOP(entry_node, "Table", table_name);
    DNX_KBP_COMBO_LOG_NO_INDENT(DNX_KBP_COMBO_LOG_ENTRY, "from table \"%s\"\n", table_name);

    /** Parse the validation expectations */
    SHR_IF_ERR_EXIT(dnx_kbp_combo_entry_get_expectation_parse(unit, step_node, &expectation));
    DNX_KBP_COMBO_LOG(DNX_KBP_COMBO_LOG_ENTRY, "Expect get %s\n", expect_str[expectation]);

    if (action == DNX_KBP_COMBO_TEST_ACTION_EXECUTE)
    {
        /** Get the DBAL table ID and allocate handle for it */
        SHR_IF_ERR_EXIT(dbal_logical_table_string_to_id(unit, table_name, &dbal_table_id));
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));

        /** Get the table info */
        SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, dbal_table_id, &table));

        /** Set entry access ID if the table type is TCAM by ID */
        if (table->table_type == DBAL_TABLE_TYPE_TCAM_BY_ID)
        {
            SHR_IF_ERR_EXIT(dnx_kbp_combo_entry_access_id_parse_action(unit, entry_node, entry_handle_id));
        }
        else
        {
            /** Parse and set the key fields for the entry */
            SHR_IF_ERR_EXIT(dnx_kbp_combo_entry_keys_manage
                            (unit, field_action, entry_node, entry_handle_id, dbal_table_id));
        }

        /** Get the entry and validate it according to the expectation */
        if ((expectation == DNX_KBP_COMBO_ENTRY_GET_EXPECTATION_VALIDATE) ||
            (expectation == DNX_KBP_COMBO_ENTRY_GET_EXPECTATION_VALIDATE_ERROR))
        {
            SHR_SET_CURRENT_ERR(dnx_kbp_combo_entry_get_validate(unit, entry_node, entry_handle_id, dbal_table_id));
            if (expectation == DNX_KBP_COMBO_ENTRY_GET_EXPECTATION_VALIDATE)
            {
                if (SHR_GET_CURRENT_ERR() != _SHR_E_NONE)
                {
                    SHR_ERR_EXIT(_SHR_E_FAIL, "Expected \"%s\" but got \"%s\" for entry ID %d get expectation\n",
                                 _SHR_ERRMSG(_SHR_E_NONE), _SHR_ERRMSG(SHR_GET_CURRENT_ERR()), id);
                }
            }
            else
            {
                if (SHR_GET_CURRENT_ERR() == _SHR_E_NONE)
                {
                    SHR_ERR_EXIT(_SHR_E_FAIL, "Expected any error but got \"%s\" for entry ID %d get expectation\n",
                                 _SHR_ERRMSG(SHR_GET_CURRENT_ERR()), id);
                }
            }
        }
        else
        {
            SHR_SET_CURRENT_ERR(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
            if (expectation == DNX_KBP_COMBO_ENTRY_GET_EXPECTATION_NOT_FOUND)
            {
                if (SHR_GET_CURRENT_ERR() != _SHR_E_NOT_FOUND)
                {
                    SHR_ERR_EXIT(_SHR_E_FAIL, "Expected \"%s\" but got \"%s\" for entry ID %d get expectation\n",
                                 _SHR_ERRMSG(_SHR_E_NOT_FOUND), _SHR_ERRMSG(SHR_GET_CURRENT_ERR()), id);
                }
            }
            else if (SHR_GET_CURRENT_ERR() != _SHR_E_NONE)
            {
                SHR_ERR_EXIT(_SHR_E_FAIL, "Expected no error but got \"%s\" for entry ID %d get expectation\n",
                             _SHR_ERRMSG(SHR_GET_CURRENT_ERR()), id);
            }
        }

        SHR_SET_CURRENT_ERR(_SHR_E_NONE);

        DNX_KBP_COMBO_LOG(DNX_KBP_COMBO_LOG_ENTRY,
                          "Entry get ID %d expectation %s passed\n", id, expect_str[expectation]);
    }
    else
    {
        /** Print AccessID priority and keys in all cases */
        SHR_IF_ERR_EXIT(dnx_kbp_combo_entry_access_id_and_priority_dump_info(unit, entry_node));
        SHR_IF_ERR_EXIT(dnx_kbp_combo_entry_keys_manage
                        (unit, field_action, entry_node, entry_handle_id, dbal_table_id));

        /** Only print the expected result fields if expecting the entry is found */
        if (expectation != DNX_KBP_COMBO_ENTRY_GET_EXPECTATION_NOT_FOUND)
        {
            SHR_IF_ERR_EXIT(dnx_kbp_combo_entry_results_manage
                            (unit, field_action, entry_node, entry_handle_id, dbal_table_id));
        }
    }

exit:
    DNX_KBP_COMBO_LOG_INDENT_POP;
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Execute entry delete. Use access ID for ACL entries. Parse and set the key fields for FWD entries.
 */
static shr_error_e
dnx_kbp_combo_entry_execute_delete_action(
    int unit,
    dnx_kbp_combo_test_action_e action,
    void *step_node,
    void *entries_root)
{
    int id;
    void *entry_node;
    char table_name[DBAL_MAX_STRING_LENGTH];
    uint32 dbal_table_id;
    uint32 entry_handle_id = 0;
    const dbal_logical_table_t *table;
    dnx_kbp_combo_entry_field_e field_action = (action == DNX_KBP_COMBO_TEST_ACTION_EXECUTE) ?
        DNX_KBP_COMBO_ENTRY_FIELD_ADD : DNX_KBP_COMBO_ENTRY_FIELD_INFO;
    dnx_kbp_combo_entry_delete_expectation_e expectation = 0;
    char expect_str[DNX_KBP_COMBO_ENTRY_DELETE_EXPECTATION_NOF][16] = {
        "INVALID", "PASS", "ERROR", "NOT_FOUND"
    };

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_KBP_COMBO_LOG_INDENT_PUSH;

    /** Get the entry node from the XML file */
    RHDATA_GET_INT_STOP(step_node, "EntryID", id);
    if (id >= CTEST_DNX_KBP_COMBO_MAX_NOF_ENTRIES)
    {
        SHR_ERR_EXIT(_SHR_E_RESOURCE, "Entry with ID %d exceeds the maximum available %d\n"
                     "Please update the define CTEST_DNX_KBP_COMBO_MAX_NOF_ENTRIES "
                     "according to the number of entries used\n", id, CTEST_DNX_KBP_COMBO_MAX_NOF_ENTRIES);
    }

    DNX_KBP_COMBO_LOG(DNX_KBP_COMBO_LOG_ENTRY, "Entry delete ID %d ", id);
    SHR_IF_ERR_EXIT(dnx_kbp_combo_node_id_get(unit, FALSE, id, entries_root, &entry_node));

    /** Get the entry table name */
    RHDATA_GET_STR_STOP(entry_node, "Table", table_name);
    DNX_KBP_COMBO_LOG_NO_INDENT(DNX_KBP_COMBO_LOG_ENTRY, "from table \"%s\"\n", table_name);

    /** Parse the validation expectations */
    SHR_IF_ERR_EXIT(dnx_kbp_combo_entry_delete_expectation_parse(unit, step_node, &expectation));
    DNX_KBP_COMBO_LOG(DNX_KBP_COMBO_LOG_ENTRY, "Expect delete %s\n", expect_str[expectation]);

    if (action == DNX_KBP_COMBO_TEST_ACTION_EXECUTE)
    {
        /** Get the DBAL table ID and allocate handle for it */
        SHR_IF_ERR_EXIT(dbal_logical_table_string_to_id(unit, table_name, &dbal_table_id));
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));

        /** Get the table info */
        SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, dbal_table_id, &table));

        /** Set entry access ID if the table type is TCAM by ID */
        if (table->table_type == DBAL_TABLE_TYPE_TCAM_BY_ID)
        {
            SHR_IF_ERR_EXIT(dnx_kbp_combo_entry_access_id_parse_action(unit, entry_node, entry_handle_id));
        }
        else
        {
            /** Parse and set the key fields for the entry */
            SHR_IF_ERR_EXIT(dnx_kbp_combo_entry_keys_manage
                            (unit, field_action, entry_node, entry_handle_id, dbal_table_id));
        }

        if (action == DNX_KBP_COMBO_TEST_ACTION_EXECUTE)
        {
            /** Delete the entry */
            SHR_SET_CURRENT_ERR(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

            if ((expectation == DNX_KBP_COMBO_ENTRY_DELETE_EXPECTATION_PASS) && (SHR_GET_CURRENT_ERR() != _SHR_E_NONE))
            {
                SHR_ERR_EXIT(_SHR_E_FAIL, "Expected \"%s\" but got \"%s\" for entry ID %d delete expectation\n",
                             _SHR_ERRMSG(_SHR_E_NONE), _SHR_ERRMSG(SHR_GET_CURRENT_ERR()), id);
            }
            else if ((expectation == DNX_KBP_COMBO_ENTRY_DELETE_EXPECTATION_NOT_FOUND) &&
                     (SHR_GET_CURRENT_ERR() != _SHR_E_NOT_FOUND))
            {
                SHR_ERR_EXIT(_SHR_E_FAIL, "Expected \"%s\" but got \"%s\" for entry ID %d delete expectation\n",
                             _SHR_ERRMSG(_SHR_E_NOT_FOUND), _SHR_ERRMSG(SHR_GET_CURRENT_ERR()), id);
            }
            else if ((expectation == DNX_KBP_COMBO_ENTRY_DELETE_EXPECTATION_ERROR) &&
                     (SHR_GET_CURRENT_ERR() == _SHR_E_NONE))
            {
                SHR_ERR_EXIT(_SHR_E_FAIL, "Expected any error but got \"%s\" for entry ID %d delete expectation\n",
                             _SHR_ERRMSG(SHR_GET_CURRENT_ERR()), id);
            }

            SHR_SET_CURRENT_ERR(_SHR_E_NONE);
            if (expectation == DNX_KBP_COMBO_ENTRY_DELETE_EXPECTATION_PASS)
            {
                g_entries_added_bmp[id] = FALSE;
            }
        }
    }
    else
    {
        /** Print AccessID priority and keys in all cases */
        SHR_IF_ERR_EXIT(dnx_kbp_combo_entry_access_id_and_priority_dump_info(unit, entry_node));
        SHR_IF_ERR_EXIT(dnx_kbp_combo_entry_keys_manage
                        (unit, field_action, entry_node, entry_handle_id, dbal_table_id));
    }

exit:
    DNX_KBP_COMBO_LOG_INDENT_POP;
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Parse the search into master key buffer.
 */
static shr_error_e
dnx_kbp_combo_search_key_parse(
    int unit,
    dnx_kbp_combo_test_action_e action,
    void *search_node,
    uint32 opcode_id,
    uint8 *master_key_buffer)
{
    int index;
    void *key_fields_node;
    void *iter_node;
    uint32 nof_master_keys;
    kbp_mngr_key_segment_t master_key_info[DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY];

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(search_node, _SHR_E_PARAM, "search_node");
    SHR_NULL_CHECK(master_key_buffer, _SHR_E_PARAM, "master_key_buffer");

    if (action == DNX_KBP_COMBO_TEST_ACTION_EXECUTE)
    {
        /** Get the master key info */
        SHR_IF_ERR_EXIT(kbp_mngr_opcode_master_key_get(unit, opcode_id, &nof_master_keys, master_key_info));

        /** Iterate over the fields and build the master key buffer */
        DNX_KBP_COMBO_XML_NODE_GET(search_node, key_fields_node, "KeyFields");
        RHDATA_ITERATOR(iter_node, key_fields_node, "Field")
        {
            int byte_offset = 0;
            char field_name[DBAL_MAX_STRING_LENGTH] = { 0 };

            /** Get the field name and try to find it in the master key */
            RHDATA_GET_STR_STOP(iter_node, "Name", field_name);
            for (index = 0; index < nof_master_keys; index++)
            {
                if (sal_strncmp(field_name, master_key_info[index].name, DNX_KBP_KEY_SEGMENT_NAME_SIZE_IN_BYTES))
                {
                    /** Segment does not correspond to this field. Update the master key buffer byte offset */
                    byte_offset += master_key_info[index].nof_bytes;
                }
                else
                {
                    int value;
                    uint32 value32;
                    RHDATA_GET_INT_STOP(iter_node, "Value", value);
                    value32 = value;

                    /** Write the parsed value to the correct part of the master key */

                    SHR_IF_ERR_EXIT(dnx_kbp_buffer_dbal_to_kbp
                                    (unit, BYTES2BITS(master_key_info[index].nof_bytes), &value32,
                                     &master_key_buffer[byte_offset]));

                    break;
                }
            }
        }

        DNX_KBP_COMBO_LOG(DNX_KBP_COMBO_LOG_SEARCH, "Master key: ");
        for (index = 0; index < DNX_KBP_MAX_KEY_LENGTH_IN_BYTES; index++)
        {
            DNX_KBP_COMBO_LOG_NO_INDENT(DNX_KBP_COMBO_LOG_SEARCH, "%02X", master_key_buffer[index]);
        }
        DNX_KBP_COMBO_LOG_NO_INDENT(DNX_KBP_COMBO_LOG_SEARCH, "\n");
    }
    else
    {
        /** Print the search keys using the entry keys utility */
        SHR_IF_ERR_EXIT(dnx_kbp_combo_entry_keys_manage(unit, DNX_KBP_COMBO_ENTRY_FIELD_INFO, search_node, 0, 0));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Parse the search result and compare it against the expected search results.
 */
static shr_error_e
dnx_kbp_combo_search_result_parse(
    int unit,
    void *search_node,
    uint32 opcode_id,
    struct kbp_search_result *search_result)
{
    int index;
    void *expected_results_node;
    void *results_node;
    void *iter_node;
    int hit_bits;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(search_node, _SHR_E_PARAM, "search_node");
    SHR_NULL_CHECK(search_result, _SHR_E_PARAM, "search_result");

    /** Validate all expected HitBits */
    DNX_KBP_COMBO_XML_NODE_GET(search_node, expected_results_node, "ExpectedResults");
    RHDATA_GET_INT_STOP(expected_results_node, "HitBits", hit_bits);

    for (index = 0; index < DNX_KBP_MAX_NOF_RESULTS; index++)
    {
        if (search_result->hit_or_miss[index])
        {
            if (hit_bits & (1 << (7 - index)))
            {
                /** Hit as expected */
                DNX_KBP_COMBO_LOG(DNX_KBP_COMBO_LOG_SEARCH, "Search %d hit as expected\n", index);
            }
            else
            {
                /** Expected hit, but got miss */
                DNX_KBP_COMBO_LOG(DNX_KBP_COMBO_LOG_SEARCH, "Search %d hit but expected miss\n", index);
            }
        }
        else
        {
            if (hit_bits & (1 << (7 - index)))
            {
                /** Expected miss, but got hit */
                DNX_KBP_COMBO_LOG(DNX_KBP_COMBO_LOG_SEARCH, "Search %d miss but expected hit\n", index);
            }
            else
            {
                /** Miss as expected */
                DNX_KBP_COMBO_LOG(DNX_KBP_COMBO_LOG_SEARCH, "Search %d miss as expected\n", index);
            }
        }
    }

    RHDATA_ITERATOR(results_node, expected_results_node, "Field")
    {
        int hit_bit;
        /** Get the result hit bit; use it as result ID */
        RHDATA_GET_INT_STOP(results_node, "HitBit", hit_bit);

        RHDATA_ITERATOR(iter_node, results_node, "Field")
        {
            int value;
            char field_name[DBAL_MAX_STRING_LENGTH] = { 0 };

            RHDATA_GET_STR_STOP(iter_node, "Name", field_name);
            RHDATA_GET_INT_STOP(iter_node, "Value", value);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Execute search.
 */
static shr_error_e
dnx_kbp_combo_search_execute_action(
    int unit,
    dnx_kbp_combo_test_action_e action,
    void *step_node,
    void *searches_root)
{
    int id;
    void *search_node;
    char opcode_name[DBAL_MAX_STRING_LENGTH];
    char opcode_name_get[DBAL_MAX_STRING_LENGTH];
    uint32 opcode_id;
    uint32 entry_handle_id;
    int is_end;
    uint8 master_key_buffer[DNX_KBP_MAX_KEY_LENGTH_IN_BYTES] = { 0 };
    struct kbp_search_result search_result;
    kbp_instruction_handles_t inst_handle;
    uint8 is_valid = TRUE;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_KBP_COMBO_LOG_INDENT_PUSH;

    /** Get the search node from the XML file */
    RHDATA_GET_INT_STOP(step_node, "SearchID", id);
    DNX_KBP_COMBO_LOG(DNX_KBP_COMBO_LOG_ENTRY, "Search ID %d ", id);
    SHR_IF_ERR_EXIT(dnx_kbp_combo_node_id_get(unit, TRUE, id, searches_root, &search_node));

    /** Get the opcode name */
    RHDATA_GET_STR_STOP(search_node, "Opcode", opcode_name);
    DNX_KBP_COMBO_LOG_NO_INDENT(DNX_KBP_COMBO_LOG_ENTRY, "using opcode \"%s\"\n", opcode_name);

    if (action == DNX_KBP_COMBO_TEST_ACTION_EXECUTE)
    {
        /** Find the opcode ID */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_OPCODE_INFO, &entry_handle_id));
        SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
        SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr8_rule_add
                        (unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, DBAL_CONDITION_EQUAL_TO, &is_valid,
                         &is_valid));
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

        while (!is_end)
        {
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr8_get
                            (unit, entry_handle_id, DBAL_FIELD_OPCODE_NAME, INST_SINGLE, (uint8 *) opcode_name_get));

            /** Check for the opcode name */
            if (!sal_strncmp(opcode_name, opcode_name_get, DBAL_MAX_STRING_LENGTH))
            {
                break;
            }

            SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        }

        if (is_end)
        {
            /** Opcode was not found */
            DNX_KBP_COMBO_LOG(DNX_KBP_COMBO_LOG_SEARCH, "Performing search with opcode \"%s\" which was not found\n",
                              opcode_name);
            SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
            SHR_EXIT();
        }

        /** Get the opcode ID */
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_OPCODE_ID, &opcode_id));
    }

    /** Parse the values of the search into a master key buffer */
    SHR_IF_ERR_EXIT(dnx_kbp_combo_search_key_parse(unit, action, search_node, opcode_id, master_key_buffer));

    /** Perform the search */
    if (action == DNX_KBP_COMBO_TEST_ACTION_EXECUTE)
    {
        DNX_KBP_COMBO_LOG(DNX_KBP_COMBO_LOG_SEARCH, "Search opcode %d\n", opcode_id);
        sal_memset(&search_result, 0, sizeof(struct kbp_search_result));
        SHR_IF_ERR_EXIT(KBP_ACCESS.instruction_info.get(unit, opcode_id, &inst_handle));
        SHR_IF_ERR_EXIT(kbp_instruction_search(inst_handle.inst_p[0], master_key_buffer, 0, &search_result));

        /** Parse and validate the search results */
        SHR_IF_ERR_EXIT(dnx_kbp_combo_search_result_parse(unit, search_node, opcode_id, &search_result));
    }
    else
    {
        void *expected_results_node;
        int hit_bits;
        /** Print the expected hit bits */
        DNX_KBP_COMBO_XML_NODE_GET(search_node, expected_results_node, "ExpectedResults");
        RHDATA_GET_INT_STOP(expected_results_node, "HitBits", hit_bits);
        DNX_KBP_COMBO_LOG(DNX_KBP_COMBO_LOG_SEARCH, "\tExpected hit bits 0x%2X\n", hit_bits);
    }

exit:
    DNX_KBP_COMBO_LOG_INDENT_POP;
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Parse the action type from XML and return an appropriate enum
 */
static shr_error_e
dnx_kbp_combo_test_action_parse(
    int unit,
    void *step_node,
    dnx_kbp_combo_execute_action_e * action)
{
    int index;
    char action_string_get[16] = "";
    char action_strings[5][8] = { "ADD", "UPDATE", "GET", "DELETE", "SEARCH" };
    dnx_kbp_combo_execute_action_e actions[5] = {
        DNX_KBP_COMBO_EXECUTE_ACTION_ENTRY_ADD,
        DNX_KBP_COMBO_EXECUTE_ACTION_ENTRY_UPDATE,
        DNX_KBP_COMBO_EXECUTE_ACTION_ENTRY_GET,
        DNX_KBP_COMBO_EXECUTE_ACTION_ENTRY_DELETE,
        DNX_KBP_COMBO_EXECUTE_ACTION_SEARCH
    };

    SHR_FUNC_INIT_VARS(unit);
    DNX_KBP_COMBO_LOG_INDENT_PUSH;

    SHR_NULL_CHECK(step_node, _SHR_E_PARAM, "step_node");
    SHR_NULL_CHECK(action, _SHR_E_PARAM, "action");

    (*action) = DNX_KBP_COMBO_EXECUTE_ACTION_INVALID;

    RHDATA_GET_STR_STOP(step_node, "Action", action_string_get);

    for (index = 0; index < 4; index++)
    {
        if (!sal_strncmp(action_string_get, action_strings[index], 8))
        {
            (*action) = actions[index];
            SHR_EXIT();
        }
    }

    SHR_ERR_EXIT(_SHR_E_CONFIG, "Action \"%s\" is not valid\n", action_string_get);

exit:
    DNX_KBP_COMBO_LOG_INDENT_POP;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Parse and execute a single test step
 */
static shr_error_e
dnx_kbp_combo_test_step_xml_read_action(
    int unit,
    dnx_kbp_combo_test_action_e action,
    void *step_node,
    dnx_kbp_combo_env_root_nodes_info_t * root_nodes_info,
    dnx_kbp_combo_test_info_t * test_info)
{
    dnx_kbp_combo_execute_action_e execute_action;

    SHR_FUNC_INIT_VARS(unit);
    DNX_KBP_COMBO_LOG_INDENT_PUSH;

    SHR_NULL_CHECK(step_node, _SHR_E_PARAM, "step_node");
    SHR_NULL_CHECK(root_nodes_info, _SHR_E_PARAM, "root_nodes_info");
    SHR_NULL_CHECK(test_info, _SHR_E_PARAM, "test_info");

    DNX_KBP_COMBO_API_EXECUTE_NO_ERR_EXIT(dnx_kbp_combo_test_action_parse(unit, step_node, &execute_action),
                                          test_info, "failed action parsing",
                                          DNX_KBP_COMBO_TEST_STATUS_FAILED_STEP_PARSING);

    switch (execute_action)
    {
        case DNX_KBP_COMBO_EXECUTE_ACTION_ENTRY_ADD:
        {
            DNX_KBP_COMBO_API_EXECUTE_NO_ERR_EXIT(dnx_kbp_combo_entry_execute_add_action
                                                  (unit, action, step_node, root_nodes_info->entries_root, FALSE),
                                                  test_info, "failed entry add",
                                                  DNX_KBP_COMBO_TEST_STATUS_FAILED_ENTRY_MANAGEMENT);
            break;
        }
        case DNX_KBP_COMBO_EXECUTE_ACTION_ENTRY_UPDATE:
        {
            DNX_KBP_COMBO_API_EXECUTE_NO_ERR_EXIT(dnx_kbp_combo_entry_execute_add_action
                                                  (unit, action, step_node, root_nodes_info->entries_root, TRUE),
                                                  test_info, "failed entry update",
                                                  DNX_KBP_COMBO_TEST_STATUS_FAILED_ENTRY_MANAGEMENT);
            break;
        }
        case DNX_KBP_COMBO_EXECUTE_ACTION_ENTRY_GET:
        {
            DNX_KBP_COMBO_API_EXECUTE_NO_ERR_EXIT(dnx_kbp_combo_entry_execute_get_action
                                                  (unit, action, step_node, root_nodes_info->entries_root),
                                                  test_info, "failed entry get",
                                                  DNX_KBP_COMBO_TEST_STATUS_FAILED_ENTRY_MANAGEMENT);
            break;
        }
        case DNX_KBP_COMBO_EXECUTE_ACTION_ENTRY_DELETE:
        {
            DNX_KBP_COMBO_API_EXECUTE_NO_ERR_EXIT(dnx_kbp_combo_entry_execute_delete_action
                                                  (unit, action, step_node, root_nodes_info->entries_root),
                                                  test_info, "failed entry delete",
                                                  DNX_KBP_COMBO_TEST_STATUS_FAILED_ENTRY_MANAGEMENT);
            break;
        }
        case DNX_KBP_COMBO_EXECUTE_ACTION_SEARCH:
        {
            DNX_KBP_COMBO_API_EXECUTE_NO_ERR_EXIT(dnx_kbp_combo_search_execute_action
                                                  (unit, action, step_node, root_nodes_info->searches_root),
                                                  test_info, "failed search", DNX_KBP_COMBO_TEST_STATUS_FAILED_SEARCH);
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG, "Unsupported action %d\n", action);
        }
    }

exit:
    DNX_KBP_COMBO_LOG_INDENT_POP;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Read and execute a single test case by executing its steps one by one
 */
static shr_error_e
dnx_kbp_combo_test_xml_read_action(
    int unit,
    dnx_kbp_combo_test_action_e action,
    void *test_node,
    dnx_kbp_combo_env_root_nodes_info_t * root_nodes_info,
    dnx_kbp_combo_test_info_t * test_info)
{
    void *node;
    void *iter_node;
    char tmp_string[16] = "";

    SHR_FUNC_INIT_VARS(unit);
    DNX_KBP_COMBO_LOG_INDENT_PUSH;

    SHR_NULL_CHECK(test_node, _SHR_E_PARAM, "test_node");
    SHR_NULL_CHECK(root_nodes_info, _SHR_E_PARAM, "root_nodes_info");
    SHR_NULL_CHECK(test_info, _SHR_E_PARAM, "test_info");

    /** Clean the test info structure */
    sal_memset(test_info, 0x0, sizeof(dnx_kbp_combo_test_info_t));
    sal_memset(g_entries_added_bmp, 0x0, sizeof(char) * CTEST_DNX_KBP_COMBO_MAX_NOF_ENTRIES);

    /** Get test name */
    RHDATA_GET_STR_STOP(test_node, "Name", test_info->name);

    /** Check if the tests is marked Valid and should be run */
    RHDATA_GET_STR_STOP(test_node, "Valid", tmp_string);
    if (sal_strncmp(tmp_string, "TRUE", 16))
    {
        DNX_KBP_COMBO_LOG(DNX_KBP_COMBO_LOG_INFO, "Skip test case \"%s\"\n", test_info->name);
        test_info->status = DNX_KBP_COMBO_TEST_STATUS_SKIPPED;
        SHR_EXIT();
    }

    DNX_KBP_COMBO_LOG(DNX_KBP_COMBO_LOG_INFO, "Run test case \"%s\"\n", test_info->name);
    test_info->status = DNX_KBP_COMBO_TEST_STATUS_IN_PROGRESS;

    DNX_KBP_COMBO_XML_NODE_GET(test_node, node, "ExecutionSteps");

    /** Iterate over all execution steps */
    RHDATA_ITERATOR(iter_node, node, "Step")
    {
        /** Read and execute step */
        SHR_IF_ERR_EXIT(dnx_kbp_combo_test_step_xml_read_action(unit, action, iter_node, root_nodes_info, test_info));
        if (test_info->status != DNX_KBP_COMBO_TEST_STATUS_IN_PROGRESS)
        {
            /** Test failed */
            SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
            DNX_KBP_COMBO_LOG(DNX_KBP_COMBO_LOG_INFO, "Test case \"%s\" failed\n", test_info->name);
            SHR_EXIT();
        }
    }

    test_info->status = DNX_KBP_COMBO_TEST_STATUS_PASSED;
    DNX_KBP_COMBO_LOG(DNX_KBP_COMBO_LOG_INFO, "Test case \"%s\" passed\n", test_info->name);

exit:
    DNX_KBP_COMBO_LOG_INDENT_POP;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Delete all entries added during the test case. Used to clean a passed test case or rollback a failed test case.
 */
static shr_error_e
dnx_kbp_combo_test_rollback(
    int unit,
    void *entries_root)
{
    int index;
    void *entry_node;
    char table_name[DBAL_MAX_STRING_LENGTH];
    uint32 dbal_table_id;
    uint32 entry_handle_id = 0;
    const dbal_logical_table_t *table = NULL;
    dnx_kbp_combo_log_e stashed_combo_log = g_dnx_kbp_combo_log;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_KBP_COMBO_LOG_INDENT_PUSH;

    DNX_KBP_COMBO_LOG(DNX_KBP_COMBO_LOG_ENTRY, "Rollback test\n");

    SHR_NULL_CHECK(entries_root, _SHR_E_PARAM, "entries_root");

    /** Set the logging to none for the rollback */
    g_dnx_kbp_combo_log = DNX_KBP_COMBO_LOG_NONE;

    for (index = 0; index < CTEST_DNX_KBP_COMBO_MAX_NOF_ENTRIES; index++)
    {
        /** Entry with ID = index was not added; Skip it */
        if (!g_entries_added_bmp[index])
        {
            continue;
        }

        /** Use the execute delete sequence to delete the entry */
        SHR_IF_ERR_EXIT(dnx_kbp_combo_node_id_get(unit, FALSE, index, entries_root, &entry_node));
        RHDATA_GET_STR_STOP(entry_node, "Table", table_name);
        SHR_IF_ERR_EXIT(dbal_logical_table_string_to_id(unit, table_name, &dbal_table_id));
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
        SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, dbal_table_id, &table));
        if (table->table_type == DBAL_TABLE_TYPE_TCAM_BY_ID)
        {
            SHR_IF_ERR_EXIT(dnx_kbp_combo_entry_access_id_parse_action(unit, entry_node, entry_handle_id));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_kbp_combo_entry_keys_manage
                            (unit, DNX_KBP_COMBO_ENTRY_FIELD_ADD, entry_node, entry_handle_id, dbal_table_id));
        }
        if (dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT))
        {
            DNX_KBP_COMBO_LOG(DNX_KBP_COMBO_LOG_ENTRY, "Entry ID %d failed to rollback\n", index);
        }
        g_entries_added_bmp[index] = FALSE;
    }

exit:
    /** Set the combo log to its previous state */
    g_dnx_kbp_combo_log = stashed_combo_log;
    DNX_KBP_COMBO_LOG_INDENT_POP;
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 *  \brief
 *  Read all tests one by one and execute them step by step.
 *
 *  Each step can be entry management related (ADD, GET, DELETE) or a KBP search.
 *  When a test finishes (with pass and/or fail) a rollback is performed, which clears all tables.
 */
static shr_error_e
dnx_kbp_combo_env_tests_xml_read_execute(
    int unit,
    void *root,
    int test_id,
    dnx_kbp_combo_env_info_t * env_info)
{
    void *node;
    void *iter_node;
    dnx_kbp_combo_env_root_nodes_info_t root_nodes_info;

    SHR_FUNC_INIT_VARS(unit);
    DNX_KBP_COMBO_LOG_INDENT_PUSH;

    SHR_NULL_CHECK(root, _SHR_E_PARAM, "root");
    SHR_NULL_CHECK(env_info, _SHR_E_PARAM, "env_info");

    DNX_KBP_COMBO_XML_NODE_GET(root, node, "TestCases");
    env_info->nof_tests = 0;

    /** Populate the root nodes so they don't need to be read inside the execution API */
    DNX_KBP_COMBO_XML_NODE_GET(root, root_nodes_info.groups_root, "GroupList");
    DNX_KBP_COMBO_XML_NODE_GET(root, root_nodes_info.opcodes_root, "NewOpcodes");
    DNX_KBP_COMBO_XML_NODE_GET(root, root_nodes_info.entries_root, "Entries");
    DNX_KBP_COMBO_XML_NODE_GET(root, root_nodes_info.searches_root, "Searches");

    /** Iterate over all test cases */
    RHDATA_ITERATOR(iter_node, node, "TestCase")
    {
        /** Check the available combo test resources */
        if (env_info->nof_tests >= CTEST_DNX_KBP_COMBO_MAX_NOF_TESTS)
        {
            SHR_ERR_EXIT(_SHR_E_RESOURCE, "The number of tests in the XML file exceeds the maximum available %d\n"
                         "Please update the define CTEST_DNX_KBP_COMBO_MAX_NOF_TESTS "
                         "according to the number of tests executed\n", CTEST_DNX_KBP_COMBO_MAX_NOF_TESTS);
        }

        /** Iterate until the required test is found */
        if ((test_id > -1) && (test_id != env_info->nof_tests))
        {
            env_info->nof_tests++;
            continue;
        }

        /** Read a test case and execute steps */
        DNX_KBP_COMBO_API_EXECUTE_NO_ERR_EXIT_ROLLBACK(dnx_kbp_combo_test_xml_read_action
                                                       (unit, DNX_KBP_COMBO_TEST_ACTION_EXECUTE, iter_node,
                                                        &root_nodes_info, &(env_info->test_info[env_info->nof_tests])),
                                                       &(env_info->test_info[env_info->nof_tests]), "failed",
                                                       env_info->test_info[env_info->nof_tests].status,
                                                       dnx_kbp_combo_test_rollback(unit, root_nodes_info.entries_root));

        /** Do test rollback after the test ends */
        if (env_info->test_info[env_info->nof_tests].status != DNX_KBP_COMBO_TEST_STATUS_SKIPPED)
        {
            SHR_IF_ERR_EXIT(dnx_kbp_combo_test_rollback(unit, root_nodes_info.entries_root));
        }

        env_info->nof_tests++;

        /** A single test was required to run and at this point it is supposed to have ended */
        if (test_id > -1)
        {
            break;
        }
    }

exit:
    DNX_KBP_COMBO_LOG_INDENT_POP;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Read and dump the info for a single test case and its steps one by one
 */
static shr_error_e
dnx_kbp_combo_env_tests_xml_read_dump_info(
    int unit,
    void *root,
    int test_id)
{
    void *node;
    void *iter_node;
    int nof_tests = 0;
    dnx_kbp_combo_env_root_nodes_info_t root_nodes_info;
    dnx_kbp_combo_test_info_t test_info;

    SHR_FUNC_INIT_VARS(unit);
    DNX_KBP_COMBO_LOG_INDENT_PUSH;

    SHR_NULL_CHECK(root, _SHR_E_PARAM, "root");

    DNX_KBP_COMBO_XML_NODE_GET(root, node, "TestCases");

    /** Populate the root nodes so they don't need to be read inside the info API */
    DNX_KBP_COMBO_XML_NODE_GET(root, root_nodes_info.groups_root, "GroupList");
    DNX_KBP_COMBO_XML_NODE_GET(root, root_nodes_info.opcodes_root, "NewOpcodes");
    DNX_KBP_COMBO_XML_NODE_GET(root, root_nodes_info.entries_root, "Entries");
    DNX_KBP_COMBO_XML_NODE_GET(root, root_nodes_info.searches_root, "Searches");

    nof_tests = 0;

    /** Iterate over all test cases */
    RHDATA_ITERATOR(iter_node, node, "TestCase")
    {
        /** Check the available combo test resources */
        if (nof_tests >= CTEST_DNX_KBP_COMBO_MAX_NOF_TESTS)
        {
            SHR_ERR_EXIT(_SHR_E_RESOURCE, "The number of tests in the XML file exceeds the maximum available %d\n"
                         "Please update the define CTEST_DNX_KBP_COMBO_MAX_NOF_TESTS "
                         "according to the number of tests executed\n", CTEST_DNX_KBP_COMBO_MAX_NOF_TESTS);
        }

        /** Iterate until the required test is found */
        if ((test_id > -1) && (test_id != nof_tests))
        {
            nof_tests++;
            continue;
        }

        /** Read and dump the test info */
        SHR_IF_ERR_EXIT(dnx_kbp_combo_test_xml_read_action
                        (unit, DNX_KBP_COMBO_TEST_ACTION_DUMP_INFO, iter_node, &root_nodes_info, &test_info));
        nof_tests++;

        /** A single test was required to be dumped and at this point it is supposed to be already dumped */
        if (test_id > -1)
        {
            break;
        }
    }

exit:
    DNX_KBP_COMBO_LOG_INDENT_POP;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Perform a print of the execution of the combo test.
 */
static shr_error_e
dnx_kbp_combo_env_status_print(
    int unit,
    int test_id,
    uint8 configure,
    dnx_kbp_combo_env_info_t * env_info)
{
    int index;
    int iter_start = (test_id < 0) ? 0 : test_id;
    int iter_end = env_info->nof_tests;

    SHR_FUNC_INIT_VARS(unit);
    DNX_KBP_COMBO_LOG_INDENT_PUSH;
    DNX_KBP_COMBO_LOG_INDENT_PUSH;

    /** Print the environment status */
    DNX_KBP_COMBO_LOG_NO_INDENT(DNX_KBP_COMBO_LOG_ALL, "\n");
    DNX_KBP_COMBO_LOG(DNX_KBP_COMBO_LOG_ALL, "Environment status %s\n", env_status_str[env_info->status]);

    /** Print the groups and opcodes status */
    if (configure)
    {
        DNX_KBP_COMBO_LOG_NO_INDENT(DNX_KBP_COMBO_LOG_ALL, "\n");
        DNX_KBP_COMBO_LOG(DNX_KBP_COMBO_LOG_ALL, "Groups\n");
        DNX_KBP_COMBO_LOG_INDENT_PUSH;
        for (index = 0; index < env_info->nof_groups; index++)
        {
            DNX_KBP_COMBO_LOG(DNX_KBP_COMBO_LOG_ALL, "Group \"%s\" status %s\n", env_info->group_info[index].name,
                              group_status_str[env_info->group_info[index].status]);
        }
        DNX_KBP_COMBO_LOG_INDENT_POP;
        DNX_KBP_COMBO_LOG_NO_INDENT(DNX_KBP_COMBO_LOG_ALL, "\n");
        DNX_KBP_COMBO_LOG(DNX_KBP_COMBO_LOG_ALL, "Opcodes\n");
        DNX_KBP_COMBO_LOG_INDENT_PUSH;
        for (index = 0; index < env_info->nof_opcodes; index++)
        {
            DNX_KBP_COMBO_LOG(DNX_KBP_COMBO_LOG_ALL, "Opcode \"%s\" ID %d status %s\n",
                              env_info->opcode_info[index].name, env_info->opcode_info[index].opcode_id,
                              opcode_status_str[env_info->opcode_info[index].status]);
        }
        DNX_KBP_COMBO_LOG_INDENT_POP;
    }

    /** Do not print the tests, since no test was run */
    if (env_info->status == DNX_KBP_COMBO_ENV_STATUS_SYNC_FAILED)
    {
        SHR_EXIT();
    }

    DNX_KBP_COMBO_LOG_NO_INDENT(DNX_KBP_COMBO_LOG_ALL, "\n");
    DNX_KBP_COMBO_LOG(DNX_KBP_COMBO_LOG_ALL, "Tests\n");
    DNX_KBP_COMBO_LOG_INDENT_PUSH;
    /** Print the tests status */
    for (index = iter_start; index < iter_end; index++)
    {
        DNX_KBP_COMBO_LOG(DNX_KBP_COMBO_LOG_ALL, "Test \"%s\" status %s\n", env_info->test_info[index].name,
                          test_status_str[env_info->test_info[index].status]);
    }
    DNX_KBP_COMBO_LOG_INDENT_POP;
    DNX_KBP_COMBO_LOG_NO_INDENT(DNX_KBP_COMBO_LOG_ALL, "\n");

exit:
    DNX_KBP_COMBO_LOG_INDENT_POP;
    DNX_KBP_COMBO_LOG_INDENT_POP;
    SHR_FUNC_EXIT;
}

/**
 *  \brief
 *  The KBP ctest combo test command. Handles the combo tests configurations, execution and status print.
 */
shr_error_e
dnx_kbp_combo_test(
    int unit,
    char *input_xml,
    char *output,
    int test_id,
    uint8 configure)
{
    void *root = NULL;
    dnx_kbp_combo_log_e stashed_combo_log = g_dnx_kbp_combo_log;

    SHR_FUNC_INIT_VARS(unit);

    /** Set the logging level to all */
    g_dnx_kbp_combo_log = DNX_KBP_COMBO_LOG_ALL;

    if (sal_strncmp(output, "stdout", sal_strlen(output)))
    {
        /** Output to file */
        DNX_KBP_COMBO_LOG_OPEN_OUTPUT_FILE(output);

        /** Disable logging to screen */
        g_dnx_kbp_combo_log &= (~DNX_KBP_COMBO_LOG_STDOUT);
    }

    DNX_KBP_COMBO_XML_GET(input_xml);
    root = dbx_file_get_xml_top(unit, dnx_kbp_combo_xml_file_path, "CtestKbpComboRoot", CONF_OPEN_PER_DEVICE);
    if (root == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Cannot find KBP ctest combo xml file: %s\n", dnx_kbp_combo_xml_file_path);
    }

    /** Read and apply the XML configurations for groups and opcodes. Then perform KBP sync. */
    if (configure)
    {
        sal_memset(&g_env_info, 0x0, sizeof(dnx_kbp_combo_env_info_t));
        SHR_IF_ERR_EXIT(dnx_kbp_combo_env_config_xml_read_apply(unit, root, &g_env_info));
    }
    else
    {
        g_env_info.status = DNX_KBP_COMBO_ENV_STATUS_INIT_SKIPPED;
    }

    /** Read the XML and execute each test step. */
    SHR_IF_ERR_EXIT(dnx_kbp_combo_env_tests_xml_read_execute(unit, root, test_id, &g_env_info));

    /** Print the status of all groups, opcodes and tests TBD */
    SHR_IF_ERR_EXIT(dnx_kbp_combo_env_status_print(unit, test_id, configure, &g_env_info));

exit:
    /** Set the combo log to its previous state */
    g_dnx_kbp_combo_log = stashed_combo_log;
    dbx_xml_top_close(root);
    if (sal_strncmp(output, "stdout", sal_strlen(output)))
    {
        DNX_KBP_COMBO_LOG_CLOSE_OUTPUT_FILE();
    }
    SHR_FUNC_EXIT;
}

/**
 *  \brief
 *  The KBP ctest combo info command. Outputs information for all test cases
 */
shr_error_e
dnx_kbp_combo_info(
    int unit,
    char *input_xml,
    char *output,
    int test_id)
{
    void *root = NULL;
    dnx_kbp_combo_log_e stashed_combo_log = g_dnx_kbp_combo_log;

    SHR_FUNC_INIT_VARS(unit);

    /** Set the logging level to all */
    g_dnx_kbp_combo_log = DNX_KBP_COMBO_LOG_ALL;

    if (sal_strncmp(output, "stdout", sal_strlen(output)))
    {
        /** Output to file */
        DNX_KBP_COMBO_LOG_OPEN_OUTPUT_FILE(output);

        /** Disable logging to screen */
        g_dnx_kbp_combo_log &= (~DNX_KBP_COMBO_LOG_STDOUT);
    }

    DNX_KBP_COMBO_XML_GET(input_xml);
    root = dbx_file_get_xml_top(unit, dnx_kbp_combo_xml_file_path, "CtestKbpComboRoot", CONF_OPEN_PER_DEVICE);
    if (root == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Cannot find KBP ctest combo xml file: %s\n", dnx_kbp_combo_xml_file_path);
    }

    /** output stuff here */
    SHR_IF_ERR_EXIT(dnx_kbp_combo_env_tests_xml_read_dump_info(unit, root, test_id));

exit:
    /** Set the combo log to its previous state */
    g_dnx_kbp_combo_log = stashed_combo_log;
    dbx_xml_top_close(root);
    if (sal_strncmp(output, "stdout", sal_strlen(output)))
    {
        DNX_KBP_COMBO_LOG_CLOSE_OUTPUT_FILE();
    }
    SHR_FUNC_EXIT;
}
