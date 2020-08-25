/** \file diag_dnx_l3_ecmp.c
 *
 * file for l3 intf diagnostics
 *
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DIAG

/*
 * INCLUDE FILES:
 * {
 */
/** bcm */
#include <bcm/l3.h>
#include <bcm_int/dnx/l3/l3_ecmp.h>
#include <bcm_int/dnx/algo/l3/algo_l3.h>
/** soc */
#include <soc/dnx/swstate/auto_generated/access/ecmp_access.h>
/** appl */
#include <appl/diag/diag.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include "diag_dnx_l3.h"
/*
 * }
 */

/*
 * This is the number of columns to use when displaying the ECMP consistent members.
 */
#define DIAG_DNX_PP_ECMP_MEMBERS_TBL_NOF_COLUMN_DISPLAY     32

static sh_sand_man_t dnx_l3_ecmp_dump_man = {
    .brief = "Dump information about the existing ECMP groups regarding their ID, mode, protection, etc.",
    .full = "Print information regarding all existing ECMP groups in a table.\n"
        "If a particular ECMP group ID has been provided and if its mode is consistent, "
        "then its members will be printed as well.",
    .synopsis = "[id=<ECMP_GROUP_ID>]",
    .examples = "\n" "ID=50"
};

static sh_sand_man_t dnx_l3_ecmp_create_man = {
    .brief = "Create an ECMP group",
    .full = "Create an ECMP group by providing an ECMP ID, size of the group and its members.",
    .synopsis = "[ID=<ECMP ID>] size=<group size> fec=\"<FEC members>\"",
    .examples = "size=2 fec=0x2001000020010001" "\n" "ID=50 size=3 fec=0x200100012001000220010003"
    /** Each 32 bits of the fec option value are taken as a new element in the array */
};

static sh_sand_man_t dnx_l3_ecmp_destroy_man = {
    .brief = "Destroy an existing ECMP group",
    .full = "Destroy an existing ECMP group by providing its ID",
    .synopsis = "ID=<ECMP ID>",
    .examples = "ID=50"
};

static sh_sand_man_t dnx_l3_ecmp_add_man = {
    .brief = "Add a member to the ECMP group",
    .full = "Add a member to the ECMP group by providing the ECMP group ID and the new member ID",
    .synopsis = "ID=<ECMP ID> intf=<FEC ID>",
    .examples = "ID=50 intf=0x20010004"
};

static sh_sand_man_t dnx_l3_ecmp_get_man = {
    .brief = "Receive the data for an ECMP group",
    .full = "Receive the data for an ECMP group by providing the ECMP group ID",
    .synopsis = "ID=<ECMP ID>",
    .examples = "ID=50"
};

static sh_sand_man_t dnx_l3_ecmp_delete_man = {
    .brief = "Delete a member to the ECMP group",
    .full = "Delete a member to the ECMP group by providing the ECMP group ID and the member ID",
    .synopsis = "ID=<ECMP ID> intf=<FEC ID>",
    .examples = "ID=50 intf=0x20010004"
};

static sh_sand_man_t dnx_l3_ecmp_allocation_man = {
    .brief = "Show the bank allocation information",
    .full = "Present information for each ECMP bank including its hierarchy and availability.",
    .synopsis = "[bank=<Bank ID>]",
    .examples = "\n" "bank=1"
};

static sh_sand_option_t dnx_l3_ecmp_dump_options[] = {
    {"ID", SAL_FIELD_TYPE_INT32, "The ECMP group index", "-1"},
    {NULL}
};

static sh_sand_option_t dnx_l3_ecmp_create_options[] = {
    {"ID", SAL_FIELD_TYPE_UINT32, "The ECMP group index", "1"},
    {"Size", SAL_FIELD_TYPE_INT32, "The ECMP group size", NULL},
    {"fec", SAL_FIELD_TYPE_ARRAY32, "The ECMP group members", "0x20010000"},
    {NULL}
};

static sh_sand_option_t dnx_l3_ecmp_add_options[] = {
    {"ID", SAL_FIELD_TYPE_UINT32, "The ECMP group index", NULL},
    {"INTerFace", SAL_FIELD_TYPE_INT32, "Intf index", NULL},
    {NULL}
};

static sh_sand_option_t dnx_l3_ecmp_delete_options[] = {
    {"ID", SAL_FIELD_TYPE_UINT32, "The ECMP group index", NULL},
    {"INTerFace", SAL_FIELD_TYPE_INT32, "Intf index", NULL},
    {NULL}
};

static sh_sand_option_t dnx_l3_ecmp_get_options[] = {
    {"ID", SAL_FIELD_TYPE_UINT32, "The ECMP group index", NULL},
    {NULL}
};

static sh_sand_option_t dnx_l3_ecmp_destroy_options[] = {
    {"ID", SAL_FIELD_TYPE_UINT32, "The ECMP group index", NULL},
    {NULL}
};

static sh_sand_option_t dnx_l3_ecmp_allocation_options[] = {
    {"bank", SAL_FIELD_TYPE_UINT32, "The ECMP bank index", "20"},
    {NULL}
};

/**
 * \brief
 *    A function that prints the accessed ECMP group and its FEC members.
 *    It is valid for create/add/delete/get/destroy commands.
 * \param [in] unit - unit ID
 * \param [in] ecmp_id - The ID of the ECMP group.
 * \param [in] size - The size of the ECMP group.
 * \param [in] fec_members - The list of members of this ECMP group
 * \param [in] sand_control - passed according to the diag mechanism
 * \param [in] action - string indicatig what the action to the ECMP group was:
 *          * create
 *          * add
 *          * delete
 *          * destroy
 *          * get
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
dnx_l3_ecmp_entry_print(
    int unit,
    uint32 ecmp_id,
    int size,
    int *fec_members,
    sh_sand_control_t * sand_control,
    char *action)
{
    int idx;
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("ECMP entry %s", action);

    PRT_COLUMN_ADD("ECMP ID");
    PRT_COLUMN_ADD("Size");
    PRT_COLUMN_ADD("FEC members");
    for (idx = 0; idx < 7 && idx < size - 1; idx++)
    {
        PRT_COLUMN_ADD("");
    }

    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

    PRT_CELL_SET("%d", ecmp_id);
    PRT_CELL_SET("%d", size);
    for (idx = 0; idx < size; idx++)
    {
        PRT_CELL_SET("0x%04x", fec_members[idx]);
        if ((idx + 1) % 8 == 0 && idx + 1 < size)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SKIP(2);
        }
    }

    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    A diagnostics function that dumps all ECMP groups that have been saved.
 * \param [in] unit - unit ID
 * \param [in] args - arguments passed to the function according to the diag mechanism
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_l3_ecmp_dump_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int ecmp_gr_id;
    uint32 field_result;
    uint32 entry_handle_id_ecmp;
    uint32 nof_row_members = 0;
    uint32 member_row = 0;
    uint32 row_mem_idx = 0;
    uint32 entry_handle_id;
    uint32 max_nof_members;
    uint32 col_idx;
    uint32 nof_rows = 0;
    uint32 row_idx = 0;
    uint32 ecmp_group_profile = 0;
    uint32 member_base_address = 0;
    uint8 fec_member = 0;
    int is_end, entry_counter = 0;
    int intf_array[L3_ECMP_MAX_NOF_ECMP_MEMBERS];
    int size = L3_ECMP_MAX_NOF_ECMP_MEMBERS;
    char buffer[DBAL_MAX_PRINTABLE_BUFFER_SIZE];
    dnx_l3_ecmp_profile_t ecmp_profile;
    dbal_enum_value_field_hierarchy_level_e hierarchy = DBAL_ENUM_FVAL_HIERARCHY_LEVEL_HIERARCHY_LEVEL_1;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("ID", ecmp_gr_id);

    PRT_TITLE_SET("ECMP Groups Data");

    PRT_COLUMN_ADD("ECMP Group ID");
    PRT_COLUMN_ADD("Base FEC address");
    PRT_COLUMN_ADD("Hierarchy Level");
    PRT_COLUMN_ADD("Profile ID");
    PRT_COLUMN_ADD("ECMP Mode");
    PRT_COLUMN_ADD("Group Size");
    PRT_COLUMN_ADD("Protection");
    PRT_COLUMN_ADD("RPFmode");

    /*
     * Allocate handle to the main ECMP table.
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECMP_TABLE, &entry_handle_id_ecmp));
    /*
     * If a particular ECMP group ID has been entered, then only the information regarding it will be printed.
     * If no ID has been provided, then all ECMP groups will be dumped.
     */
    if (ecmp_gr_id != -1)
    {
        ecmp_gr_id = BCM_L3_ITF_VAL_GET(ecmp_gr_id);
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

        dbal_entry_key_field32_set(unit, entry_handle_id_ecmp, DBAL_FIELD_ECMP_ID, ecmp_gr_id);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id_ecmp, DBAL_GET_ALL_FIELDS));
        /** ECMP ID  */
        SHR_IF_ERR_EXIT(dbal_table_field_printable_string_get(unit, DBAL_TABLE_ECMP_TABLE, DBAL_FIELD_ECMP_ID,
                                                              (uint32 *) &ecmp_gr_id, NULL, 0, TRUE, buffer));
        PRT_CELL_SET("%s", buffer);

        /** Base FEC address */
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id_ecmp, DBAL_FIELD_FEC_POINTER_BASE,
                                                            INST_SINGLE, &field_result));
        SHR_IF_ERR_EXIT(dbal_table_field_printable_string_get(unit, DBAL_TABLE_ECMP_TABLE, DBAL_FIELD_FEC_POINTER_BASE,
                                                              &field_result, NULL, 0, FALSE, buffer));
        PRT_CELL_SET("%s", buffer);

        /** Hierarchy level */
        SHR_IF_ERR_EXIT(dnx_algo_l3_ecmp_hierarchy_get(unit, ecmp_gr_id, (uint32 *) &hierarchy));
        SHR_IF_ERR_EXIT(dbal_table_field_printable_string_get
                        (unit, DBAL_TABLE_ECMP_GROUP_PROFILE_TABLE, DBAL_FIELD_HIERARCHY_LEVEL, &hierarchy, NULL, 0,
                         TRUE, buffer));
        PRT_CELL_SET("%s", buffer);

        /** ECMP group profile ID */
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id_ecmp,
                                                            DBAL_FIELD_ECMP_GROUP_PROFILE, INST_SINGLE,
                                                            &ecmp_group_profile));
        SHR_IF_ERR_EXIT(dbal_table_field_printable_string_get
                        (unit, DBAL_TABLE_ECMP_TABLE, DBAL_FIELD_ECMP_GROUP_PROFILE, &ecmp_group_profile, NULL, 0,
                         FALSE, buffer));
        PRT_CELL_SET("%s", buffer);

        SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_get_ecmp_group_info(unit, ecmp_gr_id, &size, intf_array, &ecmp_profile));

        /** ECMP mode */
        SHR_IF_ERR_EXIT(dbal_table_field_printable_string_get(unit, DBAL_TABLE_ECMP_GROUP_PROFILE_TABLE,
                                                              DBAL_FIELD_ECMP_MODE, &ecmp_profile.ecmp_mode, NULL, 0,
                                                              FALSE, buffer));
        PRT_CELL_SET("%s", buffer);

        /** ECMP group size */
        SHR_IF_ERR_EXIT(dbal_table_field_printable_string_get(unit, DBAL_TABLE_ECMP_GROUP_PROFILE_TABLE,
                                                              DBAL_FIELD_MAX_GROUP_SIZE_MINUS_ONE,
                                                              (uint32 *) &ecmp_profile.group_size, NULL,
                                                              0, FALSE, buffer));
        PRT_CELL_SET("%s", buffer);

        /** Protection */
        SHR_IF_ERR_EXIT(dbal_table_field_printable_string_get
                        (unit, DBAL_TABLE_ECMP_GROUP_PROFILE_TABLE, DBAL_FIELD_PROTECTION_ENABLE,
                         (uint32 *) &ecmp_profile.protection_flag, NULL, 0, FALSE, buffer));
        PRT_CELL_SET("%s", buffer);

        /** RPF mode */
        SHR_IF_ERR_EXIT(dbal_table_field_printable_string_get(unit, DBAL_TABLE_ECMP_GROUP_PROFILE_TABLE,
                                                              DBAL_FIELD_ECMP_RPF_MODE,
                                                              (uint32 *) &ecmp_profile.rpf_mode, NULL, 0, FALSE,
                                                              buffer));
        PRT_CELL_SET("%s", buffer);

        PRT_COMMITX;

        /** Print members table */
        if (ecmp_profile.ecmp_mode != DBAL_ENUM_FVAL_ECMP_MODE_MULTIPLY_AND_DIVIDE)
        {
            l3_ecmp_consistent_type_e consistent_table_type;

            dnx_l3_ecmp_consistent_type_t const_table_info;

            /*
             * Get the members base address (relative to table address)
             */
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ECMP_GROUP_PROFILE_TABLE, &entry_handle_id));

            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ECMP_GROUP_PROFILE, ecmp_group_profile);

            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HIERARCHY_LEVEL, hierarchy);

            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ECMP_MEMBERS_BASE, INST_SINGLE,
                                       &member_base_address);

            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

            SHR_IF_ERR_EXIT(dnx_l3_ecmp_consistent_table_type_from_ecmp_mode_get
                            (unit, ecmp_profile.ecmp_mode, &consistent_table_type));

            SHR_IF_ERR_EXIT(dnx_l3_ecmp_consistent_members_table_info_get
                            (unit, consistent_table_type, &const_table_info));

            member_row = member_base_address * const_table_info.nof_rows_in_members_table;

            PRT_TITLE_SET("Members offsets table address 0x%x (tbl type relative addr %d)", member_row,
                          member_base_address);

            for (col_idx = 0; col_idx < DIAG_DNX_PP_ECMP_MEMBERS_TBL_NOF_COLUMN_DISPLAY; col_idx++)
            {
                PRT_COLUMN_ADD("%d", (int) col_idx);
            }

            nof_row_members = const_table_info.nof_members_in_table_row;

            max_nof_members = const_table_info.nof_entries;

            /** Determine the number of iterations */
            nof_rows = max_nof_members / DIAG_DNX_PP_ECMP_MEMBERS_TBL_NOF_COLUMN_DISPLAY;
            if (max_nof_members % DIAG_DNX_PP_ECMP_MEMBERS_TBL_NOF_COLUMN_DISPLAY != 0)
            {
                nof_rows++;
            }

            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FEC_ECMP_MEMBERS, &entry_handle_id));

            for (row_idx = 0, entry_counter = 0; row_idx < nof_rows * 2 - 1; row_idx++)
            {
                PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
                for (col_idx = 0; col_idx < DIAG_DNX_PP_ECMP_MEMBERS_TBL_NOF_COLUMN_DISPLAY; col_idx++)
                {
                    if (row_idx != 0 && row_idx % 2 == 1)
                    {
                        PRT_CELL_SET("%d", (int) entry_counter);
                        entry_counter++;
                    }
                    else
                    {
                        if (row_mem_idx == 0)
                        {
                            /** Keys */
                            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ECMP_MEMBER_ROW_ID,
                                                       member_row);
                            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
                        }
                        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                                        (unit, entry_handle_id, DBAL_FIELD_ECMP_MEMBER_VAL, row_mem_idx, &fec_member));
                        PRT_CELL_SET("%02x", fec_member);
                        if (row_mem_idx + 1 == nof_row_members)
                        {
                            DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FEC_ECMP_MEMBERS, entry_handle_id);
                            member_row++;
                            row_mem_idx = 0;
                        }
                        else
                        {
                            row_mem_idx++;
                        }
                        entry_counter++;
                    }
                }
                if (row_idx % 2 == 1)
                {
                    entry_counter -= DIAG_DNX_PP_ECMP_MEMBERS_TBL_NOF_COLUMN_DISPLAY;
                }
            }

            PRT_COMMITX;
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id_ecmp, DBAL_ITER_MODE_ALL));
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id_ecmp, &is_end));

        /** Iterate over all entries in the ECMP_TABLE */
        while (!is_end)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

            /** ECMP ID  */
            SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id_ecmp, DBAL_FIELD_ECMP_ID,
                                                                  (uint32 *) &ecmp_gr_id));
            SHR_IF_ERR_EXIT(dbal_table_field_printable_string_get(unit, DBAL_TABLE_ECMP_TABLE, DBAL_FIELD_ECMP_ID,
                                                                  (uint32 *) &ecmp_gr_id, NULL, 0, TRUE, buffer));
            PRT_CELL_SET("%s", buffer);

            /** Base FEC address */
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id_ecmp, DBAL_FIELD_FEC_POINTER_BASE, INST_SINGLE, &field_result));
            SHR_IF_ERR_EXIT(dbal_table_field_printable_string_get
                            (unit, DBAL_TABLE_ECMP_TABLE, DBAL_FIELD_FEC_POINTER_BASE, &field_result, NULL, 0, FALSE,
                             buffer));
            PRT_CELL_SET("%s", buffer);

            /** Hierarchy level */
            SHR_IF_ERR_EXIT(dnx_algo_l3_ecmp_hierarchy_get(unit, ecmp_gr_id, (uint32 *) &hierarchy));
            SHR_IF_ERR_EXIT(dbal_table_field_printable_string_get(unit, DBAL_TABLE_ECMP_GROUP_PROFILE_TABLE,
                                                                  DBAL_FIELD_HIERARCHY_LEVEL, &hierarchy, NULL, 0, TRUE,
                                                                  buffer));
            PRT_CELL_SET("%s", buffer);

            /** ECMP group profile ID */
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id_ecmp,
                                                                DBAL_FIELD_ECMP_GROUP_PROFILE, INST_SINGLE,
                                                                &field_result));

            SHR_IF_ERR_EXIT(dbal_table_field_printable_string_get(unit, DBAL_TABLE_ECMP_TABLE,
                                                                  DBAL_FIELD_ECMP_GROUP_PROFILE, &field_result, NULL,
                                                                  0, FALSE, buffer));
            PRT_CELL_SET("%s", buffer);

            SHR_IF_ERR_EXIT(dnx_l3_egress_ecmp_get_ecmp_group_info(unit, ecmp_gr_id, &size, intf_array, &ecmp_profile));
            SHR_IF_ERR_EXIT(dbal_table_field_printable_string_get(unit, DBAL_TABLE_ECMP_GROUP_PROFILE_TABLE,
                                                                  DBAL_FIELD_ECMP_MODE,
                                                                  &ecmp_profile.ecmp_mode, NULL, 0, FALSE, buffer));
            /** ECMP mode */
            PRT_CELL_SET("%s", buffer);

            SHR_IF_ERR_EXIT(dbal_table_field_printable_string_get(unit, DBAL_TABLE_ECMP_GROUP_PROFILE_TABLE,
                                                                  DBAL_FIELD_MAX_GROUP_SIZE_MINUS_ONE,
                                                                  (uint32 *) &ecmp_profile.group_size, NULL,
                                                                  0, FALSE, buffer));
            /** ECMP group size */
            PRT_CELL_SET("%s", buffer);

            SHR_IF_ERR_EXIT(dbal_table_field_printable_string_get(unit, DBAL_TABLE_ECMP_GROUP_PROFILE_TABLE,
                                                                  DBAL_FIELD_PROTECTION_ENABLE,
                                                                  (uint32 *) &ecmp_profile.protection_flag, NULL,
                                                                  0, FALSE, buffer));
            /** Protection */
            PRT_CELL_SET("%s", buffer);

            SHR_IF_ERR_EXIT(dbal_table_field_printable_string_get(unit, DBAL_TABLE_ECMP_GROUP_PROFILE_TABLE,
                                                                  DBAL_FIELD_ECMP_RPF_MODE,
                                                                  (uint32 *) &ecmp_profile.rpf_mode, NULL,
                                                                  0, FALSE, buffer));
            /** RPF mode */
            PRT_CELL_SET("%s", buffer);

            SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id_ecmp, &is_end));
            entry_counter++;
        }
        if (entry_counter > 0)
        {
            PRT_INFO_ADD("Total Entries: %d", entry_counter);
            PRT_COMMITX;
        }
        else
        {
            printf("No ECMP group entries have been found\n\n");
        }
    }

exit:
    PRT_FREE;
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    A diagnostics function that creates an ECMP group.
 * \param [in] unit - unit ID
 * \param [in] args - arguments passed to the function according to the diag mechanism
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_l3_ecmp_create_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_l3_egress_ecmp_t ecmp_entry;
    bcm_l3_egress_t egr_entry;
    uint32 *fec_members;
    uint32 ecmp_id;
    uint32 egr_intf = 0x40003000;
    uint32 flags = BCM_L3_INGRESS_ONLY | BCM_L3_WITH_ID;
    int size;
    int idx;
    int rv;
    int count;

    SHR_FUNC_INIT_VARS(unit);
    SH_SAND_GET_UINT32("ID", ecmp_id);
    SH_SAND_GET_INT32("Size", size);
    SH_SAND_GET_ARRAY32("fec", fec_members);

    bcm_l3_egress_ecmp_t_init(&ecmp_entry);

    ecmp_entry.flags = BCM_L3_WITH_ID;
    ecmp_entry.ecmp_intf = ecmp_id;
    /** If the ECMP group is in the extended range, add the needed flag. */
    if (ecmp_id >= dnx_data_l3.ecmp.max_ecmp_basic_mode_get(unit))
    {
        ecmp_entry.ecmp_group_flags = BCM_L3_ECMP_EXTENDED;
    }

    for (idx = 0; idx < size; idx++)
    {
        rv = bcm_l3_egress_get(unit, fec_members[idx], &egr_entry);
        if (rv == _SHR_E_NOT_FOUND)
        {
            egr_entry.intf = egr_intf;
            SHR_IF_ERR_EXIT(bcm_l3_egress_create(unit, flags, &egr_entry, (int *) &fec_members[idx]));
        }
        else
        {
            SHR_IF_ERR_EXIT(rv);
        }
    }
    SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_create(unit, &ecmp_entry, size, (int *) fec_members));
    SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_get(unit, &ecmp_entry, size, (int *) fec_members, &count));
    SHR_IF_ERR_EXIT(dnx_l3_ecmp_entry_print(unit, ecmp_id, count, (int *) fec_members, sand_control, "create"));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    A diagnostics function that adds a FEC member to an existing ECMP group.
 * \param [in] unit - unit ID
 * \param [in] args - arguments passed to the function according to the diag mechanism
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_l3_ecmp_add_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_l3_egress_ecmp_t ecmp_entry;
    bcm_l3_egress_t egr_entry;
    uint32 ecmp_id;
    uint32 flags = BCM_L3_INGRESS_ONLY | BCM_L3_WITH_ID;
    int intf;
    uint32 egr_intf = 0x40003000;
    int rv;
    int count;
    int intf_array[L3_ECMP_MAX_NOF_ECMP_MEMBERS];
    int size = L3_ECMP_MAX_NOF_ECMP_MEMBERS;

    SHR_FUNC_INIT_VARS(unit);
    SH_SAND_GET_UINT32("ID", ecmp_id);
    SH_SAND_GET_INT32("INTerFace", intf);
    BCM_L3_ITF_SET(ecmp_id, BCM_L3_ITF_TYPE_FEC, BCM_L3_ITF_VAL_GET(ecmp_id));
    bcm_l3_egress_ecmp_t_init(&ecmp_entry);
    ecmp_entry.ecmp_intf = ecmp_id;

    rv = bcm_l3_egress_get(unit, intf, &egr_entry);
    if (rv == _SHR_E_NOT_FOUND)
    {
        egr_entry.intf = egr_intf;
        SHR_IF_ERR_EXIT(bcm_l3_egress_create(unit, flags, &egr_entry, &intf));
    }
    else
    {
        SHR_IF_ERR_EXIT(rv);
    }

    SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_add(unit, &ecmp_entry, intf));
    SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_get(unit, &ecmp_entry, size, intf_array, &count));
    SHR_IF_ERR_EXIT(dnx_l3_ecmp_entry_print(unit, ecmp_id, count, intf_array, sand_control, "add"));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    A diagnostics function that deletes a FEC member from an existing ECMP group.
 * \param [in] unit - unit ID
 * \param [in] args - arguments passed to the function according to the diag mechanism
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_l3_ecmp_delete_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_l3_egress_ecmp_t ecmp_entry;
    uint32 ecmp_id;
    int intf;
    int count;
    int intf_array[L3_ECMP_MAX_NOF_ECMP_MEMBERS];
    int size = L3_ECMP_MAX_NOF_ECMP_MEMBERS;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32("ID", ecmp_id);
    SH_SAND_GET_INT32("INTerFace", intf);
    BCM_L3_ITF_SET(ecmp_id, BCM_L3_ITF_TYPE_FEC, BCM_L3_ITF_VAL_GET(ecmp_id));
    bcm_l3_egress_ecmp_t_init(&ecmp_entry);
    ecmp_entry.ecmp_intf = ecmp_id;
    SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_delete(unit, &ecmp_entry, intf));
    SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_get(unit, &ecmp_entry, size, intf_array, &count));
    SHR_IF_ERR_EXIT(dnx_l3_ecmp_entry_print(unit, ecmp_id, count, intf_array, sand_control, "delete"));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    A diagnostics function that destroys an existing ECMP group.
 * \param [in] unit - unit ID
 * \param [in] args - arguments passed to the function according to the diag mechanism
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_l3_ecmp_destroy_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_l3_egress_ecmp_t ecmp_entry;
    uint32 ecmp_id;
    int intf_array[L3_ECMP_MAX_NOF_ECMP_MEMBERS];
    int size = L3_ECMP_MAX_NOF_ECMP_MEMBERS;
    int count;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32("ID", ecmp_id);
    BCM_L3_ITF_SET(ecmp_id, BCM_L3_ITF_TYPE_FEC, BCM_L3_ITF_VAL_GET(ecmp_id));
    bcm_l3_egress_ecmp_t_init(&ecmp_entry);
    ecmp_entry.ecmp_intf = ecmp_id;
    SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_get(unit, &ecmp_entry, size, intf_array, &count));
    SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_destroy(unit, &ecmp_entry));
    SHR_IF_ERR_EXIT(dnx_l3_ecmp_entry_print(unit, ecmp_id, count, intf_array, sand_control, "destroy"));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    A diagnostics function that present the ECMP bank allocation data
 * \param [in] unit - unit ID
 * \param [in] args - arguments passed to the function according to the diag mechanism
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_l3_ecmp_allocation_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 bank_id = 0;
    uint32 hierarchy;
    uint32 first_in_bank;
    uint32 nof_banks =
        dnx_data_l3.ecmp.nof_ecmp_banks_basic_get(unit) + dnx_data_l3.ecmp.nof_ecmp_banks_extended_get(unit);
    uint32 nof_elements;
    uint32 bank_size;
    char *hierarchies[4] = { "1ST hierarchy", "2ND hierarchy", "3RD hierarchy", "NO hierarchy" };

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32("bank", bank_id);
    PRT_TITLE_SET("ECMP bank allocation");

    PRT_COLUMN_ADD("Bank ID");
    PRT_COLUMN_ADD("Hierarchy");
    PRT_COLUMN_ADD("Allocated/Total");
    PRT_COLUMN_ADD("Reserved for failover");
    if (bank_id < nof_banks)
    {
        nof_banks = bank_id + 1;
    }
    else
    {
        bank_id = 0;
    }
    for (; bank_id < nof_banks; bank_id++)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

        bank_size = dnx_data_l3.ecmp.ecmp_stage_map_size_get(unit);
        first_in_bank = bank_id * bank_size;
        SHR_IF_ERR_EXIT(dnx_algo_l3_ecmp_hierarchy_get(unit, first_in_bank, &hierarchy));
        SHR_IF_ERR_EXIT(ecmp_db_info.ecmp_nof_used_elements_per_grain.get(unit, bank_id, &nof_elements));
        if (bank_id == 0)
        {
            nof_elements--;
            bank_size--;
        }
        PRT_CELL_SET("%u", bank_id);
        if (nof_elements == 0)
        {
            PRT_CELL_SET("NO hierarchy");
        }
        else
        {
            PRT_CELL_SET("%s", hierarchies[hierarchy]);
        }

        PRT_CELL_SET("%u/%u", nof_elements, bank_size);
        if (bank_id >= dnx_data_l3.ecmp.nof_ecmp_banks_basic_get(unit))
        {
            if (hierarchy == DBAL_ENUM_FVAL_HIERARCHY_LEVEL_NO_HIERARCHY)
            {
                PRT_CELL_SET("Yes");
            }
            else
            {
                PRT_CELL_SET("No");
            }
        }
        else
        {
            PRT_CELL_SET("N/A");
        }
    }

    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    A diagnostics function that receives the data for an existing ECMP group.
 * \param [in] unit - unit ID
 * \param [in] args - arguments passed to the function according to the diag mechanism
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_l3_ecmp_get_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_l3_egress_ecmp_t ecmp_entry;
    uint32 ecmp_id;
    int intf_array[L3_ECMP_MAX_NOF_ECMP_MEMBERS];
    int size = L3_ECMP_MAX_NOF_ECMP_MEMBERS;
    int count = 0;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32("ID", ecmp_id);
    BCM_L3_ITF_SET(ecmp_id, BCM_L3_ITF_TYPE_FEC, BCM_L3_ITF_VAL_GET(ecmp_id));
    bcm_l3_egress_ecmp_t_init(&ecmp_entry);
    ecmp_entry.ecmp_intf = ecmp_id;
    SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_get(unit, &ecmp_entry, size, intf_array, &count));
    SHR_IF_ERR_EXIT(dnx_l3_ecmp_entry_print(unit, ecmp_id, count, intf_array, sand_control, "get"));
exit:
    SHR_FUNC_EXIT;
}

sh_sand_cmd_t sh_dnx_l3_ecmp_cmds[] = {

    /*
     * keyword, action, command, options, man
     */
    {
     "create", sh_dnx_l3_ecmp_create_cmd, NULL, dnx_l3_ecmp_create_options, &dnx_l3_ecmp_create_man},
    {
     "dump", sh_dnx_l3_ecmp_dump_cmd, NULL, dnx_l3_ecmp_dump_options, &dnx_l3_ecmp_dump_man},
    {
     "add", sh_dnx_l3_ecmp_add_cmd, NULL, dnx_l3_ecmp_add_options, &dnx_l3_ecmp_add_man},
    {
     "delete", sh_dnx_l3_ecmp_delete_cmd, NULL, dnx_l3_ecmp_delete_options, &dnx_l3_ecmp_delete_man},
    {
     "get", sh_dnx_l3_ecmp_get_cmd, NULL, dnx_l3_ecmp_get_options, &dnx_l3_ecmp_get_man},
    {
     "destroy", sh_dnx_l3_ecmp_destroy_cmd, NULL, dnx_l3_ecmp_destroy_options, &dnx_l3_ecmp_destroy_man},
    {
     "allocation", sh_dnx_l3_ecmp_allocation_cmd, NULL, dnx_l3_ecmp_allocation_options, &dnx_l3_ecmp_allocation_man},
    {
     NULL}
};
