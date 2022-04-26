/** \file diag_dnx_field_tcam.c
 * $Id$
 *
 * Database access (incl. TCAM Look-ups) application procedures for DNX.
 *
 */
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDTESTSDNX

#include <shared/shrextend/shrextend_debug.h>
#include <bcm/field.h>
#include <bcm/error.h>
#include <bcm_int/dnx/field/field.h>
#include <bcm_int/dnx/field/field_entry.h>
#include <bcm_int/dnx/field/field_map.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <sal/appl/sal.h>
#include <shared/utilex/utilex_framework.h>

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
/*
 * TYPEDEFs
 * {
 */
/*
 * }
 */
/*
 * MACROs
 * {
 */
/**
 *   Number of entries to add/delete, which time will be measured
 */
#define DNX_DIAG_TCAM_OPTION_NUM_ENTRIES          "Entries"
/**
 *   FG key size 80/160/320
 */
#define DNX_DIAG_TCAM_OPTION_KEY_SIZE             "Key_size"
/**
 *   FG payload size 32/64/128
 */
#define DNX_DIAG_TCAM_OPTION_PAYLOAD_SIZE          "Payload_size"
/**
 *   They order of priorities that will be add to TCAM
 *   0 - best performance, decrease priorty order  prio [0 -> N]
 *   1 - wort performance, increase priorty prder  prio [N -> 0]
 *   2 - mid performance, one decrease one increase, e.g. prio seq [500, 501,499,502,498 ..]
 *       this is is basicaly Jr1 best sequance
 *   3 - Same Priorty
 */
#define DNX_DIAG_TCAM_OPTION_SEQUANCE_TYPE         "Seq_type"
/**
 *   Enable cache commit or not
 */
#define DNX_DIAG_TCAM_OPTION_CACHING               "Caching"

/**
 *   Keyword for the number of times the test will run
 */
#define DNX_DIAG_ACTION_OPTION_TEST_COUNT        "Runs"

/**
 *   Amount of entries expected to be add per 1 second
 */
#define DNX_DIAG_ACTION_OPTION_TEST_EXPECTED_RATE        "Expected_Rate"


/* *INDENT-OFF* */
/**

 *   Options list for 'tcam' shell command
 * \remark
 *   Set the default value such that no test is carried out but procedure does return with 'success'
 *   This ensures success on regression and pre-commit:
 */
sh_sand_option_t dnx_field_tcam_options[] = {
     /* Name */                                 /* Type */              /* Description */                                 /* Default */
    {DNX_DIAG_TCAM_OPTION_NUM_ENTRIES,          SAL_FIELD_TYPE_UINT32,  "Number of entries",                            "1000"},
    {DNX_DIAG_TCAM_OPTION_KEY_SIZE,             SAL_FIELD_TYPE_UINT32,  "FG Key size",                                   "80"},
    {DNX_DIAG_TCAM_OPTION_PAYLOAD_SIZE,         SAL_FIELD_TYPE_UINT32,  "FG Payload size",                              "32"},
    {DNX_DIAG_TCAM_OPTION_SEQUANCE_TYPE,        SAL_FIELD_TYPE_UINT32,  "Sequance type",                                "3"},
    {DNX_DIAG_TCAM_OPTION_CACHING,              SAL_FIELD_TYPE_BOOL,    "using cache commit",                           "FALSE"},
    {DNX_DIAG_ACTION_OPTION_TEST_COUNT,         SAL_FIELD_TYPE_UINT32,  "Number of times test will run",                "1"},
    {DNX_DIAG_ACTION_OPTION_TEST_EXPECTED_RATE, SAL_FIELD_TYPE_UINT32,  "Expected Rate",                                "2500"},
    {NULL}      /* End of options list - must be last. */
};

/**
 * \brief
 *   List of tests for 'tcam' shell command (to be run on regression, precommit, etc.)
 * \remark
 *   The values of Expected_Rate are set 15% below what seen in practice
 */
sh_sand_invoke_t sh_dnx_field_tcam_tests[] = {
    {"DNX_tcam_half_80_32",                         "Entries=1000 Key_size=80 Payload_size=32 Seq_type=0 Caching=0 Runs=1 Expected_Rate=2300",  CTEST_POSTCOMMIT},
    {"DNX_tcam_half_80_32_cache_worst",             "Entries=1000 Key_size=80 Payload_size=32 Seq_type=1 Caching=1 Runs=1 Expected_Rate=2300",  CTEST_POSTCOMMIT},
    {"DNX_tcam_single_160_32",                      "Entries=1000 Key_size=160 Payload_size=32 Seq_type=0 Caching=0 Runs=1 Expected_Rate=2500",  CTEST_POSTCOMMIT},
    {"DNX_tcam_single_80_64",                       "Entries=1000 Key_size=80 Payload_size=64 Seq_type=0 Caching=0 Runs=1 Expected_Rate=2500",  CTEST_POSTCOMMIT},
    {"DNX_tcam_single_160_64",                      "Entries=1000 Key_size=160 Payload_size=64 Seq_type=0 Caching=0 Runs=1 Expected_Rate=2500",  CTEST_POSTCOMMIT},
    {"DNX_tcam_single_160_64_cache_worst",          "Entries=1000 Key_size=160 Payload_size=64 Seq_type=1 Caching=1 Runs=1 Expected_Rate=2500",  CTEST_POSTCOMMIT},
    {"DNX_tcam_double_320_32",                      "Entries=1000 Key_size=320 Payload_size=32 Seq_type=0 Caching=0 Runs=1 Expected_Rate=2000",  CTEST_POSTCOMMIT},
    {"DNX_tcam_double_80_128",                      "Entries=1000 Key_size=80 Payload_size=128 Seq_type=0 Caching=0 Runs=1 Expected_Rate=2000",  CTEST_POSTCOMMIT},
    {"DNX_tcam_double_320_128",                     "Entries=1000 Key_size=320 Payload_size=128 Seq_type=0 Caching=0 Runs=1 Expected_Rate=2000",  CTEST_POSTCOMMIT},
    {"DNX_tcam_double_320_128_cache_worst",         "Entries=1000 Key_size=320 Payload_size=128 Seq_type=1 Caching=1 Runs=1 Expected_Rate=2000",  CTEST_POSTCOMMIT},
    {NULL}
};

/* *INDENT-ON* */
/*
 * }
 */
/**
 * Manual for TCAM tests
 */
sh_sand_man_t sh_dnx_field_tcam_man = {
    "Performance of entry add and delete for TCAM Field groups\n",
    "ctest field tcam Entries=<nof_entries> Key_size=<80/160/320> Payload_size=<32/64/128> Seq_type=<0-Best/1-Worst/2-Mid/3-same priority> Caching=<TRUE/FALSE> Runs=<nof_runs> Expected_Rate=<Expected_Rate>",
    "Entries=1000 Key_size=80 Payload_size=32 Seq_type=0 Caching=0 Runs=1",
};

static shr_error_e
ctest_dnx_tcam_configuration_set(
    int unit,
    int nof_quals,
    int nof_actions,
    bcm_field_group_t * fg_id,
    bcm_field_group_info_t * fg_info)
{

    bcm_field_group_attach_info_t attach_info;
    int ii = 0;

    SHR_FUNC_INIT_VARS(unit);

    bcm_field_group_info_t_init(fg_info);
    fg_info->fg_type = bcmFieldGroupTypeTcam;
    fg_info->stage = bcmFieldStageIngressPMF2;

    fg_info->nof_quals = nof_quals;
    fg_info->qual_types[0] = bcmFieldQualifySrcIp;
    fg_info->qual_types[1] = bcmFieldQualifySrcIp6;
    fg_info->qual_types[2] = bcmFieldQualifyForwardingLayerIndex;

    /*
     * Set actions
     */
    fg_info->nof_actions = nof_actions;

    fg_info->action_with_valid_bit[0] = FALSE;
    fg_info->action_types[0] = bcmFieldActionVoid;
    fg_info->action_types[1] = bcmFieldActionSnoopRaw;
    fg_info->action_types[2] = bcmFieldActionTrapRaw;

    sal_strncpy_s((char *) (fg_info->name), "TCAM_Performance", sizeof(fg_info->name));
    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, 0, fg_info, fg_id));

    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info->nof_quals;
    attach_info.payload_info.nof_actions = fg_info->nof_actions;

    for (ii = 0; ii < fg_info->nof_quals; ii++)
    {
        attach_info.key_info.qual_types[ii] = fg_info->qual_types[ii];
    }
    for (ii = 0; ii < fg_info->nof_actions; ii++)
    {
        attach_info.payload_info.action_types[ii] = fg_info->action_types[ii];
    }

    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info.key_info.qual_info[0].input_arg = 1;
    attach_info.key_info.qual_info[0].offset = 0;
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info.key_info.qual_info[1].input_arg = 2;
    attach_info.key_info.qual_info[1].offset = 0;
    attach_info.key_info.qual_info[2].input_type = bcmFieldInputTypeMetaData;

    SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, *fg_id, 0, &attach_info));
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
ctest_dnx_tcam_configuration_delete(
    int unit,
    bcm_field_group_t fg_id)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_field_group_context_detach(unit, fg_id, 0));
    SHR_IF_ERR_EXIT(bcm_field_group_delete(unit, fg_id));
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
ctest_dnx_tcam_entries_generate(
    int unit,
    bcm_field_group_t fg_id,
    bcm_field_group_info_t * fg_info,
    int nof_entries,
    int nof_quals,
    int nof_actions,
    int seq_type,
    bcm_field_entry_info_t * entry_info)
{
    int entries_index, index;
    int priority = nof_entries + 1;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(entry_info, _SHR_E_INTERNAL, "entry_info");

    for (entries_index = 0; entries_index < nof_entries; entries_index++)
    {
        bcm_field_entry_info_t_init(&entry_info[entries_index]);

        for (index = 0; index < nof_quals; index++)
        {
            entry_info[entries_index].entry_qual[index].type = fg_info->qual_types[index];
            entry_info[entries_index].entry_qual[index].value[0] = 1;
            entry_info[entries_index].entry_qual[index].mask[0] = -1;
        }
        entry_info[entries_index].nof_entry_quals = nof_quals;

        for (index = 0; index < nof_actions; index++)
        {
            entry_info[entries_index].entry_action[index].type = fg_info->action_types[index];
            entry_info[entries_index].entry_action[index].value[0] = 1;
        }
        entry_info[entries_index].nof_entry_actions = nof_actions;

        if (seq_type == 0)
        {
            entry_info[entries_index].priority = entries_index;
        }
        else if (seq_type == 1)
        {
            entry_info[entries_index].priority = priority;
            priority--;
        }
        else if (seq_type == 2)
        {
            priority = (nof_entries + 1) / 2;
            if ((entries_index % 2) == 0)
            {
                priority = priority + entries_index / 2;
            }
            else
            {
                priority = priority - (entries_index + 1) / 2;
            }
            entry_info[entries_index].priority = priority;

        }
        else if (seq_type == 3)
        {
            entry_info[entries_index].priority = 0;
        }

    }

exit:
    SHR_FUNC_EXIT;
}

/**

 *   This function is the starter for tcam basic test.
 * \param [in] unit - The unit number.
 *
 * \return
 *   Error code (as per 'bcm_error_e').
 * \see
 *   bcm_error_e
 */
static shr_error_e
ctest_dnx_tcam_starter(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    shr_error_e rv = _SHR_E_NONE;
    int nof_entries, key_size, payload_size, seq_type;
    int caching;
    bcm_field_group_t fg_id;
    int nof_quals = 3;
    int nof_actions = 2;
    bcm_field_group_info_t fg_info;
    bcm_field_entry_info_t *entry_info = NULL;
    bcm_field_entry_t *entry_id = NULL;
    int i_entry = 0;
    uint32 timer_idx[2] = { 1, 2 };
    uint32 timers_group = UTILEX_LL_TIMER_NO_GROUP;
    char group_name[UTILEX_LL_TIMER_MAX_NOF_CHARS_IN_TIMER_GROUP_NAME];
    int start_time = 0;
    int run_time;
    double rate = 0;
    int expected_rate;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get the inputs from Shell
     */
    SH_SAND_GET_UINT32(DNX_DIAG_TCAM_OPTION_NUM_ENTRIES, nof_entries);
    SH_SAND_GET_UINT32(DNX_DIAG_TCAM_OPTION_KEY_SIZE, key_size);
    SH_SAND_GET_UINT32(DNX_DIAG_TCAM_OPTION_PAYLOAD_SIZE, payload_size);
    SH_SAND_GET_UINT32(DNX_DIAG_TCAM_OPTION_SEQUANCE_TYPE, seq_type);
    SH_SAND_GET_BOOL(DNX_DIAG_TCAM_OPTION_CACHING, caching);
    SH_SAND_GET_UINT32(DNX_DIAG_ACTION_OPTION_TEST_EXPECTED_RATE, expected_rate);

    if (key_size == 80)
    {
        nof_quals = 1;
    }
    else if (key_size == 160)
    {
        nof_quals = 2;
    }
    else if (key_size == 320)
    {
        nof_quals = 3;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illigal key_size %d", key_size);
    }

    if (payload_size == 32)
    {
        nof_actions = 1;
    }
    else if (payload_size == 64)
    {
        nof_actions = 2;
    }
    else if (payload_size == 128)
    {
        nof_actions = 3;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illigal payload_size %d", key_size);
    }

    SHR_IF_ERR_EXIT(ctest_dnx_tcam_configuration_set(unit, nof_quals, nof_actions, &fg_id, &fg_info));

    entry_info = (bcm_field_entry_info_t *) sal_alloc(sizeof(bcm_field_entry_info_t) * nof_entries, "entry_info");
    if (entry_info == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Could not allocate memory for %d entries\n", nof_entries);
    }
    entry_id = (bcm_field_entry_t *) sal_alloc(sizeof(bcm_field_entry_t) * nof_entries, "entry_id");
    if (entry_id == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Could not allocate memory for %d entry IDs\n", nof_entries);
    }

    SHR_IF_ERR_EXIT(ctest_dnx_tcam_entries_generate
                    (unit, fg_id, &fg_info, nof_entries, nof_quals, nof_actions, seq_type, entry_info));

    sal_strncpy(group_name, "TCAM performance", UTILEX_LL_TIMER_MAX_NOF_CHARS_IN_TIMER_GROUP_NAME - 1);
    /** Allocate timer group ID with a given name*/
    rv = utilex_ll_timer_group_allocate(group_name, &timers_group);
    if (rv != _SHR_E_NONE || timers_group == UTILEX_LL_TIMER_NO_GROUP)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Allocation of group has failed with error code %d.\n\r", rv);
    }
    utilex_ll_timer_clear_all(timers_group);

    utilex_ll_timer_set("bcm_field_entry_add create", timers_group, timer_idx[0]);
    if (caching)
    {
        SHR_IF_ERR_EXIT(bcm_field_group_cache(unit, 0, fg_id, bcmFieldGroupCacheModeStart));
    }
    start_time = sal_time_usecs();
    for (i_entry = 0; i_entry < nof_entries; i_entry++)
    {
        SHR_IF_ERR_EXIT(bcm_field_entry_add(unit, 0, fg_id, &entry_info[i_entry], &entry_id[i_entry]));
    }
    if (caching)
    {
        SHR_IF_ERR_EXIT(bcm_field_group_cache(unit, 0, fg_id, bcmFieldGroupCacheModeInstall));
    }
    run_time = sal_time_usecs() - start_time;
    utilex_ll_timer_stop(timers_group, timer_idx[0]);

    utilex_ll_timer_set("bcm_field_entry_delete", timers_group, timer_idx[1]);
    for (i_entry = 0; i_entry < nof_entries; i_entry++)
    {
        SHR_IF_ERR_EXIT(bcm_field_entry_delete(unit, fg_id, NULL, entry_id[i_entry]));
    }
    utilex_ll_timer_stop(timers_group, timer_idx[1]);

    SHR_IF_ERR_EXIT(ctest_dnx_tcam_configuration_delete(unit, fg_id));

    utilex_ll_timer_stop_all(timers_group);
    utilex_ll_timer_print_all(timers_group);
    utilex_ll_timer_clear_all(timers_group);

    rate = ((double) nof_entries / (double) run_time) * 1000000;
    LOG_CLI((BSL_META("Nof entries in 1 sec - %.2f\n"), rate));
    if (expected_rate > rate)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "\n Rate is lower than expected Expected (%d) , Rate(%.2f) \n", expected_rate, rate);
    }

exit:
    sal_free(entry_info);
    sal_free(entry_id);
    SHR_IF_ERR_CONT(utilex_ll_timer_group_free(timers_group));
    SHR_FUNC_EXIT;
}

/**
 * run TCAM tests
 */
shr_error_e
sh_dnx_field_tcam_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int count_iter, count;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(args, _SHR_E_PARAM, "args");
    SHR_NULL_CHECK(sand_control, _SHR_E_PARAM, "sand_control");
    SH_SAND_GET_UINT32("Runs", count);
    for (count_iter = 0; count_iter < count; count_iter++)
    {
        SHR_IF_ERR_EXIT(ctest_dnx_tcam_starter(unit, args, sand_control));
    }
exit:
    SHR_FUNC_EXIT;
}
