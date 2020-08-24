/** \file diag_dnx_arr.c
 *
 * Main diagnostics for ARR All CLI commands, that are related to ARR, are gathered in this file.
 *
 */
/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DIAGDNX

/*************
 * INCLUDES  *
 *************/
#  include <sal/appl/sal.h>
#  include <shared/bslnames.h>
#  include <appl/diag/diag.h>
#  include <appl/diag/shell.h>
#  include <appl/diag/cmdlist.h>
#  include <appl/diag/sand/diag_sand_prt.h>
#  include <appl/diag/sand/diag_sand_framework.h>
#  include <shared/utilex/utilex_framework.h>
#  include <shared/utilex/utilex_str.h>
#  include <shared/utilex/utilex_rhlist.h>
#  include <soc/sand/sand_signals.h>
/** allow drv.h include excplictly */
#ifdef _SOC_DRV_H
#undef _SOC_DRV_H
#endif
#  include <soc/drv.h> /** needed for soc_is_xxx*/
#include <soc/dnx/arr/arr.h>

#include "diag_dnx_arr.h"

/**
 * \brief
 * dump the set of formats.
 */
static shr_error_e
arr_set_dump(
    int unit,
    arr_set_info_t * arr_set_info,
    sh_sand_control_t * sand_control)
{
    int ii, jj;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("ARR - %s", arr_set_info->name);

    PRT_INFO_ADD("ARR info: max fields %d, size %d, instances %d",
                 arr_set_info->max_nof_fields, arr_set_info->size, arr_set_info->nof_instances);

    PRT_COLUMN_ADD("Format Name");
    PRT_COLUMN_ADD("");
    PRT_COLUMN_ADD("Field");
    PRT_COLUMN_ADD("offset");
    PRT_COLUMN_ADD("Size");
    PRT_COLUMN_ADD("prefix");

    for (ii = 0; ii < arr_set_info->nof_formats; ii++)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s", arr_set_info->arr_format[ii].name);
        PRT_CELL_SET("");
        PRT_CELL_SET("");
        PRT_CELL_SET("");
        PRT_CELL_SET("");
        PRT_CELL_SET("");

        for (jj = 0; jj < arr_set_info->arr_format[ii].nof_fields; jj++)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET("");
            PRT_CELL_SET("");
            PRT_CELL_SET("%s", arr_set_info->arr_format[ii].arr_field[jj].name);
            PRT_CELL_SET("%d", arr_set_info->arr_format[ii].arr_field[jj].offset);
            PRT_CELL_SET("%d", arr_set_info->arr_format[ii].arr_field[jj].size);
            PRT_CELL_SET("%d", arr_set_info->arr_format[ii].arr_field[jj].prefix);
        }
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * dump ingress/egress ARR info
 */
static shr_error_e
arr_info_dump(
    int unit,
    root_arr_info_t * arr_info,
    int is_ingress,
    sh_sand_control_t * sand_control)
{
    int nof_sets = 0, ii;
    arr_set_info_t *arr_set_info;

    SHR_FUNC_INIT_VARS(unit);

    LOG_CLI((BSL_META("\nDumping information for %s ARR\n"), (is_ingress == 1) ? "Ingress" : "Egress"));
    LOG_CLI((BSL_META("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n")));

    if (is_ingress)
    {
        nof_sets = arr_info->nof_ingress_arr_sets;
        arr_set_info = arr_info->ingress_arr;
    }
    else
    {
        nof_sets = arr_info->nof_egress_arr_sets;
        arr_set_info = arr_info->egress_arr;
    }

    for (ii = 0; ii < nof_sets; ii++)
    {
        LOG_CLI((BSL_META("\n")));
        SHR_IF_ERR_EXIT(arr_set_dump(unit, &(arr_set_info[ii]), sand_control));
        LOG_CLI((BSL_META("\n")));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * function that dump ARR info. parse the XML file if needed and store it in a global variable.
 * dump relevant info according to the input parameters dir=<ingress/egress> or name=arr_name
 */
static shr_error_e
cmd_arr_info_dump(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *arr_dir, *arr_name;
    int ii;
    root_arr_info_t *arr_info = NULL;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    arr_info = sal_alloc(sizeof(root_arr_info_t), "root_arr_info");
    SHR_NULL_CHECK(arr_info, _SHR_E_MEMORY, "arr_info");

    SHR_IF_ERR_EXIT(arr_parse_file(unit, arr_info));

    SH_SAND_GET_STR("direction", arr_dir);
    SH_SAND_GET_STR("name", arr_name);

    if (!ISEMPTY(arr_dir))
    {
        if (sal_strcasecmp("ingress", arr_dir) == 0)
        {
            SHR_IF_ERR_EXIT(arr_info_dump(unit, arr_info, 1, sand_control));
        }
        else if (sal_strcasecmp("egress", arr_dir) == 0)
        {
            SHR_IF_ERR_EXIT(arr_info_dump(unit, arr_info, 0, sand_control));
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "invalid input %s, valid inputs: arr dump dir=ingress, arr dump dir=egress\n",
                         arr_dir);
        }
    }
    else if (!ISEMPTY(arr_name))
    {
        if (sal_strcasecmp("ingress", arr_name) == 0)
        {
            SHR_IF_ERR_EXIT(arr_info_dump(unit, arr_info, 1, sand_control));
        }
        else if (sal_strcasecmp("egress", arr_name) == 0)
        {
            SHR_IF_ERR_EXIT(arr_info_dump(unit, arr_info, 0, sand_control));
        }
        else
        {
            int found = 0;
            for (ii = 0; ii < arr_info->nof_ingress_arr_sets; ii++)
            {
                if (sal_strcasecmp(arr_info->ingress_arr[ii].name, arr_name) == 0)
                {
                    LOG_CLI((BSL_META("\n")));
                    SHR_IF_ERR_EXIT(arr_set_dump(unit, &(arr_info->ingress_arr[ii]), sand_control));
                    LOG_CLI((BSL_META("\n")));
                    found = 1;
                }
            }

            for (ii = 0; ii < arr_info->nof_egress_arr_sets; ii++)
            {
                if (sal_strcasecmp(arr_info->egress_arr[ii].name, arr_name) == 0)
                {
                    LOG_CLI((BSL_META("\n")));
                    SHR_IF_ERR_EXIT(arr_set_dump(unit, &(arr_info->egress_arr[ii]), sand_control));
                    LOG_CLI((BSL_META("\n")));
                    found = 1;
                }
            }

            if (found == 0)
            {
                LOG_CLI((BSL_META("string %s not matches to any valid ARRs\n"), arr_name));
            }
        }
    }
    else
    {
        LOG_CLI((BSL_META("\nList of all valid ARRs:\n")));
        PRT_TITLE_SET("ARR Names");

        PRT_COLUMN_ADD("ARR Name");
        PRT_COLUMN_ADD("Direction");
        PRT_COLUMN_ADD("Number of formats");
        PRT_COLUMN_ADD("Size");
        PRT_COLUMN_ADD("Instances");

        for (ii = 0; ii < arr_info->nof_ingress_arr_sets; ii++)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET("%s", arr_info->ingress_arr[ii].name);
            PRT_CELL_SET("Ingress");
            PRT_CELL_SET("%d", arr_info->ingress_arr[ii].nof_formats);
            PRT_CELL_SET("%d", arr_info->ingress_arr[ii].size);
            PRT_CELL_SET("%d", arr_info->ingress_arr[ii].nof_instances);
        }

        for (ii = 0; ii < arr_info->nof_egress_arr_sets; ii++)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET("%s", arr_info->egress_arr[ii].name);
            PRT_CELL_SET("Egress");
            PRT_CELL_SET("%d", arr_info->egress_arr[ii].nof_formats);
            PRT_CELL_SET("%d", arr_info->egress_arr[ii].size);
            PRT_CELL_SET("%d", arr_info->egress_arr[ii].nof_instances);
        }

        PRT_COMMITX;
    }

exit:
    PRT_FREE;
    if (arr_info)
    {
        sal_free(arr_info);
    }
    SHR_FUNC_EXIT;
}

/** diag ARR menu info */

static sh_sand_option_t dnx_arr_options[] = {
    {"direction", SAL_FIELD_TYPE_STR, "arr dump dir", ""},
    {"name", SAL_FIELD_TYPE_STR, "arr dump name", ""},
    {NULL}
};

static sh_sand_man_t dnx_arr_dump_man = {
    .brief = "dump ARR info",
    .full = "dump ARR info",
    .synopsis = NULL,
    .examples = "dir=ingress\n" "name=IRPP_VSI_FORMATS"
};

/** \brief prints the result of ARR decoding */
static shr_error_e
dnx_arr_decode_print(
    int unit,
    rhlist_t * parsed_info,
    sh_sand_control_t * sand_control)
{
    int field_col_id;
    signal_output_t *field;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("Parsed entries");

    PRT_COLUMN_ADDX(PRT_XML_CHILD, PRT_TITLE_ID, &field_col_id, "Field");
    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, field_col_id, NULL, "Value");
    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, field_col_id, NULL, "size");

    RHITERATOR(field, parsed_info)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s", RHNAME(field));
        PRT_CELL_SET("%s", field->print_value);
        PRT_CELL_SET("%3db", field->size);
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 *  \brief
 *      Implementation of the 'arr decode' diag command.
 *  \see
 *      dnx_arr_decode
 */
static shr_error_e
cmd_arr_decode(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 *data;
    uint32 size;

    rhlist_t *flist = NULL;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Read parameters
     */
    SH_SAND_GET_ARRAY32("data", data);
    SH_SAND_GET_UINT32("size", size);
    /*
     * Call parsing service
     */
    SHR_IF_ERR_EXIT(dnx_arr_decode(unit, SOC_CORE_DEFAULT, NULL, data, size, &flist, NULL, NULL));
    if (NULL == flist)
    {
        /*
         * Decoding did not find any suitable format.
         */
        uint32 data_type = 0;
        SHR_IF_ERR_EXIT(dnx_arr_decode_data_type_get(unit, data, size, &data_type));
        SHR_ERR_EXIT(_SHR_E_FAIL, "Could not find suitable formats for data type:\"%d\"\n", data_type);
    }
    /*
     * Print result list
     */
    SHR_IF_ERR_EXIT(dnx_arr_decode_print(unit, flist, sand_control));
exit:
    if (flist)
    {
        sand_signal_list_free(flist);
    }
    SHR_FUNC_EXIT;
}

static sh_sand_option_t dnx_arr_dcd_options[] = {
    {"data", SAL_FIELD_TYPE_ARRAY32, "EES/ETPS payload", NULL},
    {"size", SAL_FIELD_TYPE_UINT32, "Payload size in bits", "130"},
    {NULL}
};

static sh_sand_man_t dnx_arr_dcd_man = {
    .brief = "Decode EES/ETPS entry using ARR",
    .full = "Decode EES/ETPS entry using ARR",
    .synopsis = "data=<hex-string>",
    .examples = "data=0x0010032000000000000000000000000000\n" "data=0x02000000000cd1d0006400000000000000 size=64"
};

/** manual for the main menu */
sh_sand_man_t sh_dnx_arr_man = {
    .brief = "Miscellaneous ARR related commands",
    .full = NULL
};

/* INDENT-OFF */
/**
 * \brief DNX ARR diagnostic pack
 * List of the supported commands, pointer to command function and command usage function.
 */
sh_sand_cmd_t sh_dnx_arr_cmds[] = {
   /***********************************************************************************************
    * CMD_NAME *     CMD_ACTION         * Next       *        Options         *     MAN           *
    *          *                        * Level      *                        *                   *
    *          *                        * CMD        *                        *                   *
    ***********************************************************************************************/
    {"Dump", cmd_arr_info_dump, NULL, dnx_arr_options, &dnx_arr_dump_man},
    {"Decode", cmd_arr_decode, NULL, dnx_arr_dcd_options, &dnx_arr_dcd_man},
    {NULL}
};
/* INDENT-ON */
