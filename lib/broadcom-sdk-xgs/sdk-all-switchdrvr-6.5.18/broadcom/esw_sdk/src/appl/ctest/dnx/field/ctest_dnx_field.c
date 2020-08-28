/*
 * \file ctest_dnx_field.c
 *
 * Purpose:    Routines for handling debug and internal signals
 *
 * $Id: diag_sand_dsig.c,v 1.00 Broadcom SDK $
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 */

#include <appl/diag/sand/diag_sand_framework.h>
#include "../algo/ctest_dnx_algo_field.h"
#include "ctest_dnx_field_cascade.h"
#include "ctest_dnx_field_dir_ext.h"
#include "ctest_dnx_field_dir_ext_semantic.h"
#include "ctest_dnx_field_group.h"
#include "ctest_dnx_field_group_add.h"
#include "ctest_dnx_field_presel_set.h"
#include "ctest_dnx_field_entry.h"
#include "ctest_dnx_field_presel.h"
#include "ctest_dnx_field_qual.h"
#include "ctest_dnx_field_tcam.h"
#include "ctest_dnx_field_trajectory.h"
#include "ctest_dnx_field_context.h"
#include "ctest_dnx_field_instru_int.h"
#include "ctest_dnx_field_context_attach.h"
#include "ctest_dnx_field_exem.h"
#include "ctest_dnx_field_ace.h"
#include "ctest_dnx_field_action.h"
#include "ctest_dnx_field_user_actions.h"
#include "ctest_dnx_field_user_quals.h"
#include "ctest_dnx_field_compare.h"
#include "ctest_dnx_field_map.h"
#include "ctest_dnx_field_udh.h"
#include "ctest_dnx_field_kbp.h"
#include "ctest_dnx_field_qual_offset.h"
#include "ctest_dnx_field_range.h"
#include "ctest_dnx_field_hash.h"
#include "ctest_dnx_field_efes.h"
#include "ctest_dnx_field_ffc.h"
#include "ctest_dnx_field_kbp_sem.h"
#include "ctest_dnx_field_key.h"
#include "ctest_dnx_field_vw.h"
#include "ctest_dnx_field_tcam_hit_bit_indication.h"

#define BSL_LOG_MODULE BSL_LS_APPL_SHELL

/* *INDENT-OFF* */
static sh_sand_cmd_t dnx_field_user_cmds[] = {
    {"actions",       sh_dnx_field_user_actions_cmd,    NULL, Sh_dnx_field_user_actions_options,&Sh_dnx_field_user_actions_man,     NULL, Sh_dnx_field_user_actions_tests},
    {"qualifier",     sh_dnx_field_user_quals_cmd,      NULL, Sh_dnx_field_user_quals_options,&Sh_dnx_field_user_quals_man,         NULL, Sh_dnx_field_user_quals_tests},
    {NULL}
};

/* Sub-commands for qualifier */
sh_sand_cmd_t dnx_field_qualifier_test_cmds[] = {
    {"OFFSet",        sh_dnx_field_qual_offset_cmd,    NULL, Sh_dnx_field_qual_offset_options, &Sh_dnx_field_qual_offset_man,     NULL, Sh_dnx_field_qual_offset_tests},
    {"global",        sh_dnx_field_qual_cmd,           NULL, dnx_field_qual_options,       &sh_dnx_field_qual_man,                NULL, sh_dnx_field_qual_tests},
    {NULL}
};

sh_sand_cmd_t dnx_field_test_cmds[] = {
    {"algo",          sh_dnx_algo_field_cmd,           NULL, dnx_algo_field_options,       &sh_dnx_algo_field_man,                 NULL, sh_dnx_algo_field_tests},
    {"action",        sh_dnx_field_action_cmd,         NULL, Sh_dnx_field_action_options,  &Sh_dnx_field_action_man,               NULL, Sh_dnx_field_action_tests},
    {"GRouP",         sh_dnx_field_group_cmd,          NULL, dnx_field_group_options,      &sh_dnx_field_group_man,                NULL, sh_dnx_field_group_tests},
    {"group_add",     sh_dnx_field_group_add_cmd,      NULL, Sh_dnx_field_group_add_options,  &Sh_dnx_field_group_add_man,         NULL, Sh_dnx_field_group_add_tests},
    {"PReseL_set",    sh_dnx_field_presel_set_cmd,     NULL, Sh_dnx_field_presel_set_options, &Sh_dnx_field_presel_set_man,        NULL, Sh_dnx_field_presel_set_tests},
    {"key",           sh_dnx_field_key_test_cmd,       NULL, Dnx_field_key_test_options,   &Sh_dnx_field_key_test_man,             NULL, Sh_dnx_field_key_tests},
    {"tcam",          sh_dnx_field_tcam_cmd,           NULL, dnx_field_tcam_options,       &sh_dnx_field_tcam_man,                 NULL, sh_dnx_field_tcam_tests},
    {"contextselect", sh_dnx_field_presel_cmd,         NULL, dnx_field_presel_options,     &sh_dnx_field_presel_man,               NULL, sh_dnx_field_presel_tests},
    {"QuALifier",     NULL,                            dnx_field_qualifier_test_cmds},
    {"dir_ext",       sh_dnx_dir_ext_cmd,              NULL, Sh_dnx_field_dir_ext_options, &Sh_dnx_dir_ext_man,                    NULL, Sh_dnx_field_dir_ext_tests},
    {"dir_ext_sem",   sh_dnx_dir_ext_sem_cmd,          NULL, Sh_dnx_field_dir_ext_sem_options, &Sh_dnx_dir_ext_sem_man,            NULL, Sh_dnx_field_dir_ext_sem_tests},
    {"CaScaDe",       sh_dnx_field_cascade_cmd,        NULL, dnx_field_cascade_options,    &sh_dnx_field_cascade_man,              NULL, sh_dnx_field_cascade_tests},
    {"Context",       sh_dnx_field_context_cmd,        NULL, dnx_field_context_options,    &sh_dnx_field_context_man,              NULL, sh_dnx_field_context_tests},
    {"trajectory",    sh_dnx_field_trajectory_cmd,     NULL, dnx_field_trajectory_options, &sh_dnx_field_trajectory_man,           NULL, sh_dnx_field_trajectory_tests},
    {"ENTry",         sh_dnx_field_entry_cmd,          NULL, dnx_field_entry_options,      &sh_dnx_field_entry_man,                NULL, sh_dnx_field_entry_tests},
    {"InstruINT",     sh_dnx_field_instru_int_cmd,     NULL, dnx_field_instru_int_options, &sh_dnx_field_instru_int_man,           NULL, sh_dnx_field_instru_int_tests},
    {"ConTeXt_Attach",sh_dnx_field_context_attach_cmd, NULL, Sh_dnx_field_context_attach_options,  &Sh_dnx_field_context_attach_man,  NULL, Sh_dnx_field_context_attach_tests},
    {"EXeM",          sh_dnx_field_exem_cmd,           NULL, Sh_dnx_field_exem_options,    &Sh_dnx_field_exem_man,                 NULL, Sh_dnx_field_exem_tests},
    {"ACE",           sh_dnx_field_ace_cmd,            NULL, Sh_dnx_field_ace_options,     &Sh_dnx_field_ace_man,                  NULL, Sh_dnx_field_ace_tests},
    {"User",          NULL,                            dnx_field_user_cmds},
    {"mapping",       NULL,                            dnx_field_map_test_cmds},
    {"CoMPare",       sh_dnx_field_compare_cmd,        NULL, Sh_dnx_field_compare_options, &Sh_dnx_field_compare_man,              NULL, Sh_dnx_field_compare_tests},
    {"UDH",           sh_dnx_field_udh_cmd,            NULL, Sh_dnx_field_udh_options,     &Sh_dnx_field_udh_man,                  NULL, Sh_dnx_field_udh_tests},
    {"KBP",           sh_dnx_field_kbp_cmd,            NULL, dnx_field_kbp_options,        &sh_dnx_field_kbp_man,                  NULL, sh_dnx_field_kbp_tests, SH_CMD_SKIP_EXEC},
    {"RaNGe",         sh_dnx_field_range_cmd,          NULL, Sh_dnx_field_range_options,   &Sh_dnx_field_range_man,                NULL, Sh_dnx_field_range_tests},
    {"HASH",          sh_dnx_field_hash_cmd,           NULL, dnx_field_hash_options,       &sh_dnx_field_hash_man,                 NULL, sh_dnx_field_hash_tests},
    {"EFES",          sh_dnx_field_efes_cmd,           NULL, Sh_dnx_field_efes_options,    &Sh_dnx_field_efes_man,                 NULL, Sh_dnx_field_efes_tests, SH_CMD_SKIP_EXEC},
    {"FFC",           sh_dnx_field_ffc_cmd,            NULL, Sh_dnx_field_ffc_options,     &Sh_dnx_field_ffc_man,                  NULL, Sh_dnx_field_ffc_tests},
    {"KBP_SEM",       sh_dnx_field_kbp_sem_cmd,        NULL, Sh_dnx_field_kbp_sem_options, &Sh_dnx_field_kbp_sem_man,              NULL, Sh_dnx_field_kbp_sem_tests, SH_CMD_SKIP_EXEC},
    {"virtual_wire",  sh_dnx_field_vw_cmd,             NULL, Sh_dnx_field_vw_options,      &Sh_dnx_field_vw_man,                   NULL, Sh_dnx_field_vw_tests},
    {"tcam_hit",      sh_dnx_field_tcam_hit_bit_indication_cmd,             NULL, Sh_dnx_field_tcam_hit_bit_indication_options,      &Sh_dnx_field_tcam_hit_bit_indication_man,                   NULL, Sh_dnx_field_tcam_hit_bit_indication_tests},
    {NULL}
};
/* *INDENT-ON* */
