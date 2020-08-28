/** \file diag_dnx_field_tcam.c
 * $Id$
 *
 * Database access (incl. TCAM Look-ups) application procedures for DNX.
 *
 */
/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDTESTSDNX

 /*
  * Include files.
  * {
  */
#include <shared/shrextend/shrextend_debug.h>
#include <bcm/field.h>
#include <bcm/error.h>
#include <bcm_int/dnx/field/field.h>
#include <bcm_int/dnx/field/field_entry.h>
#include <bcm_int/dnx/field/field_map.h>
#include <bcm_int/dnx/field/tcam/tcam_handler.h>
#include <bcm_int/dnx/field/tcam/tcam_access_profile_manager.h>
#include <bcm_int/dnx/field/tcam/tcam_location_manager.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <sal/appl/sal.h>

#include <soc/dnx/field/tcam_access/tcam_access.h>

#include "ctest_dnx_field_group.h"
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
 * \brief
 *   Keyword for test type on 'tcam' command (tcam testing)
 *   Number of databases to allocate
 */
#define DNX_DIAG_TCAM_OPTION_NUM_DB                "num_db"
/**
 * \brief
 *   Keyword for test type on 'tcam' command (tcam testing)
 *   Number of entries to allocate per database
 */
#define DNX_DIAG_TCAM_OPTION_NUM_ENTRIES          "num_entries"
/**
 * \brief
 *   Keyword for test type on 'tcam' command (tcam testing)
 *   Whether the databases co-exist with each other or not
 */
#define DNX_DIAG_TCAM_OPTION_CO_EXIST          "co_exist"
/**
 * \brief
 *   Keyword for test type on 'tcam' command (tcam testing)
 *   Whether the entries of each database have the same priority or not
 */
#define DNX_DIAG_TCAM_OPTION_SAME_PRIO          "same_priority"
/**
 * \brief
 *   Keyword for test type on 'tcam' command (tcam testing)
 *   Number of iterations of entries insertion to do
 */
#define DNX_DIAG_TCAM_OPTION_NUM_ITERS          "num_iters"
/**
 * \brief
 *   Keyword for the number of times the test will run
 */
#define DNX_DIAG_ACTION_OPTION_TEST_COUNT        "count"
/*
 * }
 */
 /*
  * Global and Static
  * {
  */
/* *INDENT-OFF* */
/**
 * \brief
 *   Options list for 'tcam' shell command
 * \remark
 *   Set the default value such that no test is carried out but procedure does return with 'success'
 *   This ensures success on regression and pre-commit:
 */
sh_sand_option_t dnx_field_tcam_options[] = {
     /* Name */                                 /* Type */              /* Description */                                 /* Default */
    {DNX_DIAG_TCAM_OPTION_NUM_DB,       SAL_FIELD_TYPE_UINT32,  "Number of dbs to test",                                    "1"},
    {DNX_DIAG_TCAM_OPTION_NUM_ENTRIES,  SAL_FIELD_TYPE_UINT32,  "Number of entries to test",                                "1"},
    {DNX_DIAG_TCAM_OPTION_CO_EXIST,     SAL_FIELD_TYPE_BOOL,    "Whether the databases co-exist with each other or not",    "TRUE"},
    {DNX_DIAG_TCAM_OPTION_SAME_PRIO,    SAL_FIELD_TYPE_BOOL,    "Whether the entries of the database have same priority or not", "FALSE"},
    {DNX_DIAG_TCAM_OPTION_NUM_ITERS,    SAL_FIELD_TYPE_UINT32,  "Number of iterations of entries insertion to do",          "1"},
    {DNX_DIAG_ACTION_OPTION_TEST_COUNT, SAL_FIELD_TYPE_UINT32,  "Number of times test will run",                            "1"},
    {NULL}      /* End of options list - must be last. */
};

/**
 * \brief
 *   List of tests for 'tcam' shell command (to be run on regression, precommit, etc.)
 * \remark
 *   Currently, it will be executed only for regression and precommit.
 */
sh_sand_invoke_t sh_dnx_field_tcam_tests[] = {
    {"DNX_tcam_default",          "num_db=1 num_entries=1 count=1",                                    CTEST_POSTCOMMIT},
    {"DNX_tcam_default_plus_dp",  "num_db=6 num_entries=20 co_exist=TRUE same_priority=FALSE count=6", CTEST_POSTCOMMIT},
    {"DNX_tcam_bank_add",         "num_db=1 num_entries=4097 co_exist=TRUE same_priority=TRUE count=2",CTEST_POSTCOMMIT},
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
    "'TCAM' related test utilities",
    "Activate 'tcam' related test utilities. This covers TCAM manager module only\r\n",
    "ctest field tcam num_db=<nof_databases> num_entries=<nof_entries> co_exist=<TRUE|FALSE> same_priority=<TRUE|FALSE>",
    "num_db=2\r\n" "NDB=2 NENT=3 COEX=TRUE SMPRIO=FALSE",
};

shr_error_e
diag_dnx_field_tcam_base_test(
    int unit,
    int num_db,
    int num_entries,
    int num_iter,
    uint8 co_exist,
    uint8 same_priority)
{
    int ii;
    int jj;
    int kk;
    int db_id;
    dnx_field_tcam_database_t db;
    dnx_field_tcam_entry_t entry = { 0 };
    uint32 prio[40000] = { 0 };
    int core = 0;
    /*
     * 64 is max number of handlers
     */
    uint32 handler_ids[64];

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_database_t_init(unit, &db));

    for (kk = 0; kk < num_iter; kk++)
    {
        for (ii = 0; ii < num_db; ii++)
        {
            db_id = ii + 10;
            if (kk == 0)
            {
                db.stage = DNX_FIELD_STAGE_EXTERNAL;
                db.is_direct = 0;
                /*
                 * 0 for half, 1 for single, 2 for double 
                 */
                db.key_size = (ii % 3) + 1;
                db.action_size = DNX_FIELD_ACTION_LENGTH_TYPE_TCAM_SINGLE;
                db.prefix_size = 3;
                db.prefix_value = DNX_FIELD_TCAM_PREFIX_VAL_AUTO;

                if (LOG_CHECK(BSL_LOG_MODULE))
                {
                    LOG_INFO_EX(BSL_LOG_MODULE, "Allocating database %d%s%s%s", db_id, EMPTY, EMPTY, EMPTY);
                }
                SHR_IF_ERR_EXIT(dnx_field_tcam_handler_create
                                (unit, &db, FIELD_TCAM_ACCESS_PROFILE_ID_AUTO,
                                 DNX_FIELD_TCAM_HANDLER_MODE_DEFAULT, &handler_ids[ii]));
            }

            LOG_INFO_EX(BSL_LOG_MODULE, "*******************Handler_id: %d****************\r\n %s%s%s", handler_ids[ii],
                        EMPTY, EMPTY, EMPTY);
            for (jj = 0; jj < num_entries / num_iter; jj++)
            {
                entry.id = jj + db_id * 2048 + kk * num_entries / num_iter;
                if (same_priority)
                {
                    entry.priority = 0;
                }
                else
                {
                    prio[entry.id] = sal_rand() % 20;
                    entry.priority = prio[entry.id];
                }
                LOG_INFO_EX(BSL_LOG_MODULE, "Allocating entry %d, with priority %d\r\n %s%s", entry.id, entry.priority,
                            EMPTY, EMPTY);
                SHR_IF_ERR_EXIT(dnx_field_tcam_handler_entry_location_alloc(unit, core, handler_ids[ii], &entry));
                {
                    dbal_physical_entry_t dbal_entry = { 0 };
                    uint32 key_val = jj + db_id * num_entries;
                    dbal_entry.key[(ii % 3) * 4] = key_val;
                    dbal_entry.k_mask[(ii % 3) * 4] = 0xffff;
                    dbal_entry.payload[0] = jj;
                    dbal_entry.entry_hw_id = entry.id;
                    SHR_IF_ERR_EXIT(dnx_field_tcam_access_entry_add(unit, 0, handler_ids[ii], &dbal_entry, 0));
                    SHR_IF_ERR_EXIT(dnx_field_tcam_access_entry_get(unit, 0, handler_ids[ii], &dbal_entry, 0));
                    if (dbal_entry.key[(ii % 3) * 4] != key_val)
                    {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Wrote 0x%x as key, received 0x%x\n", jj, dbal_entry.key[0]);
                    }
                    if (dbal_entry.payload[0] != jj)
                    {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Wrote 0x%x as payload, received 0x%x\n", jj,
                                     dbal_entry.payload[0]);
                    }
                }
            }
            if (!co_exist)
            {
                /*
                 * Databases can't co-exist, delete the existing before creating another
                 */
                for (jj = 0; jj < num_entries / num_iter; jj++)
                {
                    dbal_physical_entry_t dbal_entry = { 0 };
                    entry.id = jj + db_id * 2048 + kk * num_entries / num_iter;
                    dbal_entry.entry_hw_id = entry.id;

                    LOG_INFO_EX(BSL_LOG_MODULE, "Freeing entry %d\n %s%s%s", entry.id, EMPTY, EMPTY, EMPTY);
                    SHR_IF_ERR_EXIT(dnx_field_tcam_access_entry_delete(unit, 0, handler_ids[ii], &dbal_entry, 0));
                    SHR_IF_ERR_EXIT(dnx_field_tcam_handler_entry_location_free(unit, handler_ids[ii], entry.id));
                }
            }
        }
    }
    for (ii = 0; ii < num_db; ii++)
    {
        db_id = ii + 10;
        if (co_exist)
        {
            for (kk = 0; kk < num_iter; kk++)
            {
                for (jj = 0; jj < num_entries / num_iter; jj++)
                {
                    dbal_physical_entry_t dbal_entry = { 0 };
                    entry.id = jj + db_id * 2048 + kk * num_entries / num_iter;
                    dbal_entry.entry_hw_id = entry.id;
                    LOG_INFO_EX(BSL_LOG_MODULE, "Freeing entry %d\n %s%s%s", entry.id, EMPTY, EMPTY, EMPTY);
                    SHR_IF_ERR_EXIT(dnx_field_tcam_access_entry_delete(unit, 0, handler_ids[ii], &dbal_entry, 0));
                    SHR_IF_ERR_EXIT(dnx_field_tcam_handler_entry_location_free(unit, handler_ids[ii], entry.id));
                }
            }
        }
        LOG_INFO_EX(BSL_LOG_MODULE, "Deleting handler: %d\n %s%s%s", handler_ids[ii], EMPTY, EMPTY, EMPTY);
        SHR_IF_ERR_EXIT(dnx_field_tcam_handler_destroy(unit, handler_ids[ii]));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function is the starter for tcam basic test.
 * \param [in] unit - The unit number.
 *
 * \return
 *   Error code (as per 'bcm_error_e').
 * \see
 *   bcm_error_e
 */
static shr_error_e
appl_dnx_tcam_starter(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int tcam_test_db_num;
    int tcam_test_entries_num;
    int tcam_test_iter_num;
    uint8 tcam_test_co_exist;
    uint8 tcam_test_same_priority;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get the inputs from Shell
     */
    SH_SAND_GET_UINT32(DNX_DIAG_TCAM_OPTION_NUM_DB, tcam_test_db_num);
    SH_SAND_GET_UINT32(DNX_DIAG_TCAM_OPTION_NUM_ENTRIES, tcam_test_entries_num);
    SH_SAND_GET_BOOL(DNX_DIAG_TCAM_OPTION_CO_EXIST, tcam_test_co_exist);
    SH_SAND_GET_BOOL(DNX_DIAG_TCAM_OPTION_SAME_PRIO, tcam_test_same_priority);
    SH_SAND_GET_UINT32(DNX_DIAG_TCAM_OPTION_NUM_ITERS, tcam_test_iter_num);
    SHR_IF_ERR_EXIT(diag_dnx_field_tcam_base_test
                    (unit, tcam_test_db_num, tcam_test_entries_num, tcam_test_iter_num, tcam_test_co_exist,
                     tcam_test_same_priority));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - run TCAM tests in diag shell
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
    SH_SAND_GET_UINT32("count", count);
    for (count_iter = 0; count_iter < count; count_iter++)
    {
        SHR_IF_ERR_EXIT(appl_dnx_tcam_starter(unit, args, sand_control));
    }
exit:
    SHR_FUNC_EXIT;
}
