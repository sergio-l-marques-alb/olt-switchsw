/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * File:        ctest_dnx_packet.c
 * Purpose:     Utility to send packet and verify signals as a result
 * Requires:
 */
#include <soc/sand/sand_signals.h>

#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_packet.h>

#include <shared/shrextend/shrextend_error.h>
#include <shared/dbx/dbx_file.h>
#include <shared/dbx/dbx_xml.h>
#include <shared/gport.h>

#include <sal/appl/sal.h>

#define BSL_LOG_MODULE BSL_LS_APPL_SHELL

static shr_error_e
ctest_dnx_verify_perform(
    int unit,
    int core,
    xml_node curSet)
{
    xml_node curSignal;
    char name[RHNAME_MAX_SIZE];
    char signal_n[RHNAME_MAX_SIZE], block_n[RHNAME_MAX_SIZE], from_n[RHNAME_MAX_SIZE];

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    RHDATA_GET_STR_DEF_NULL(curSet, "name", name);

    PRT_TITLE_SET("Signal for test:%s", name);
    PRT_COLUMN_ADD("Signal");
    PRT_COLUMN_ADD("From");
    PRT_COLUMN_ADD("Block");
    PRT_COLUMN_ADD("Expected");
    PRT_COLUMN_ADD("Observed");
    PRT_COLUMN_ADD("Status");
    RHDATA_ITERATOR(curSignal, curSet, "signal")
    {
        char expected_value[DSIG_MAX_SIZE_STR], return_value[DSIG_MAX_SIZE_STR];
        RHDATA_GET_STR_DEF_NULL(curSignal, "name", signal_n);
        RHDATA_GET_STR_DEF_NULL(curSignal, "block", block_n);
        RHDATA_GET_STR_DEF_NULL(curSignal, "from", from_n);
        RHDATA_GET_STR_STOP(curSignal, "value", expected_value);
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s", signal_n);
        PRT_CELL_SET("%s", from_n);
        PRT_CELL_SET("%s", block_n);
        PRT_CELL_SET("%s", expected_value);

        SHR_SET_CURRENT_ERR(sand_signal_verify(unit, core, block_n, from_n, NULL, signal_n,
                                               NULL, 0, expected_value, return_value));
        switch (SHR_GET_CURRENT_ERR())
        {
            case _SHR_E_NOT_FOUND:
                PRT_CELL_SKIP(1);
                PRT_CELL_SET("Not Found");
                break;
            case _SHR_E_FAIL:
                PRT_CELL_SET("%s", return_value);
                PRT_CELL_SET("Oops");
                break;
            case _SHR_E_NONE:
                PRT_CELL_SKIP(1);
                PRT_CELL_SET("OK");
                break;
            default:
                PRT_CELL_SKIP(1);
                PRT_CELL_SET("Error");
                break;
        }
    }
    PRT_COMMIT;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static shr_error_e
ctest_dnx_packet_perform(
    int unit,
    int core,
    uint32 src_port,
    xml_node curPacket)
{
    xml_node curProto;
    rhhandle_t packet_handle = NULL;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(diag_sand_packet_create(unit, &packet_handle));

    RHDATA_ITERATOR(curProto, curPacket, "proto")
    {
        char proto_name[RHNAME_MAX_SIZE];
        xml_node curField;
        RHDATA_GET_STR_STOP(curProto, "name", proto_name);
        /*
         * Header argument
         */
        SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_handle, proto_name));
        RHDATA_ITERATOR(curField, curProto, "field")
        {
            char field_name[RHNAME_MAX_SIZE], field_value[RHNAME_MAX_SIZE], full_name[RHNAME_MAX_SIZE];
            RHDATA_GET_STR_STOP(curField, "name", field_name);
            RHDATA_GET_STR_STOP(curField, "value", field_value);
            sal_snprintf(full_name, RHNAME_MAX_SIZE - 1, "%s.%s", proto_name, field_name);
            SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_handle, full_name, field_value));
        }
    }

    SHR_IF_ERR_EXIT(diag_sand_packet_send(unit, src_port, packet_handle, TRUE));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
ctest_dnx_case_verify(
    int unit,
    int core,
    uint32 src_port,
    char *packet_n,
    char *signal_set_n)
{
    xml_node curTop = NULL, curSubTop = NULL, curLine;
    char filename[RHSTRING_MAX_SIZE];

    SHR_FUNC_INIT_VARS(unit);

    if ((curTop = dbx_file_get_xml_top(unit, "DNX-Devices.xml", "top", 0)) == NULL)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        SHR_EXIT();
    }
    if ((curSubTop = dbx_xml_child_get_first(curTop, "case-objects")) == NULL)
    {
        /*
         * No parsing objects - just leave
         */
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
    }

    RHDATA_ITERATOR(curLine, curSubTop, "include")
    {
        char type[RHNAME_MAX_SIZE];

        RHDATA_GET_STR_DEF_NULL(curLine, "type", type);
        if (!sal_strcasecmp(type, "tests"))
        {
            xml_node curFile, curSection, cur;
            int device_specific = 0;
            int flags = 0;

            RHDATA_GET_INT_DEF(curLine, "device_specific", device_specific, 0);
            if (device_specific == TRUE)
            {
                flags = CONF_OPEN_PER_DEVICE;
            }

            RHDATA_GET_STR_CONT(curLine, "file", filename);
            if ((curFile = dbx_file_get_xml_top(unit, filename, "top", flags)) == NULL)
                continue;

            /*
             * Go through packets and found the requested one
             */
            if (!ISEMPTY(packet_n) && ((curSection = dbx_xml_child_get_first(curFile, "packets")) != NULL))
            {
                RHDATA_ITERATOR(cur, curSection, "packet")
                {
                    char name[RHNAME_MAX_SIZE];
                    RHDATA_GET_STR_DEF_NULL(cur, "name", name);
                    if (!sal_strcasecmp(packet_n, name))
                    {
                        SHR_CLI_EXIT_IF_ERR(ctest_dnx_packet_perform(unit, core, src_port, cur), "");
                    }
                }
            }
            /*
             * Go through signals and found the requested one
             */
            if (!ISEMPTY(signal_set_n) && ((curSection = dbx_xml_child_get_first(curFile, "signals")) != NULL))
            {
                RHDATA_ITERATOR(cur, curSection, "set")
                {
                    char name[RHNAME_MAX_SIZE];
                    RHDATA_GET_STR_DEF_NULL(cur, "name", name);
                    if (!sal_strcasecmp(signal_set_n, name))
                    {
                        SHR_CLI_EXIT_IF_ERR(ctest_dnx_verify_perform(unit, core, cur), "");
                    }
                }
            }
            dbx_xml_top_close(curFile);
        }
    }

exit:
    dbx_xml_top_close(curTop);
    SHR_FUNC_EXIT;
}

sh_sand_option_t dnx_packet_test_options[] = {
    {"packet", SAL_FIELD_TYPE_STR, "Packet name from XML", "default"}
    ,
    {"set", SAL_FIELD_TYPE_STR, "Signal set name from XML", "default"}
    ,
    {"show", SAL_FIELD_TYPE_BOOL, "Show packet content", "no"}
    ,
    {"port", SAL_FIELD_TYPE_UINT32, "Use specific source TM_PORT", "0"}
    ,
    {NULL}
};

sh_sand_man_t dnx_packet_test_man = {
    "Test any case from XML file"
};

shr_error_e
dnx_packet_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *packet_n, *set_n;
    int core;
    int show_flag;
    uint32 src_port;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_STR("packet", packet_n);
    SH_SAND_GET_STR("set", set_n);
    SH_SAND_GET_INT32("core", core);
    SH_SAND_GET_BOOL("show", show_flag);
    SH_SAND_GET_UINT32("port", src_port);

    if (core == _SHR_CORE_ALL)
    {
        /*
         * We assume that core 0 will be the best choice if no core provided on on inpup
         */
        core = 0;
    }
    /*
     * Verify the case, but do not exit even on error before showing the packet if needed
     */
    SHR_SET_CURRENT_ERR(ctest_dnx_case_verify(unit, core, src_port, packet_n, set_n));

    if (show_flag == TRUE)
    {   /* We are not interested in return status */
        diag_sand_packet_last_show(unit, core);
    }

exit:
    SHR_FUNC_EXIT;
}
