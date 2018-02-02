/** \file test_dnxc_init_deinit.c
 * 
 * DNX init deinit test - Testing DNXC init deinit procedure 
 * For additional details about Device Data Component goto 'appl_ref_init_deinit.h' 
 */
/*
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DATA

/*
 * INCLUDE FILES:
 * {
 */
/*appl*/
#include <appl/diag/test.h>
#include <appl/diag/diag.h>
#include <appl/diag/parse.h>
#include <appl/diag/shell.h>
/*soc*/
#include <soc/drv.h>
/*sal*/
#include <sal/appl/sal.h>

#if defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT)
#include <appl/reference/dnxc/appl_ref_init_deinit.h>
/*
 * }
 */

/*
* Definitions:
* {
*/
typedef struct test_dnxc_init_params {

    /** DNXC Init params */
    appl_dnxc_init_param_t appl_init_params;

    /** is warmboot */
    int warmboot;

    /** nof times to repeat the test */
    int repeat;

} test_dnxc_init_params_t;

STATIC test_dnxc_init_params_t test_init_params[SOC_MAX_NUM_DEVICES];

char test_init_deinit_usage[] = 
#ifdef COMPILER_STRING_CONST_LIMIT
"Warmboot Test Usage:\n"
"  Repeat=<value>, NoInit=<value>, NoDeinit=<value> Warmboot=<value>.\n"
#else
"Warmboot Test Usage:\n"
"  Repeat=<value>         the test will loop as many times as stated. (default=1)\n"
"  Warmboot=<value>       1: The test will perform Warmboot sequence.\n"
"  NoInit=<value>         1: The test will not Init the Chip. \n"
"  NoDeinit=<value>       1: The test will not deinit the Chip.\n"
#endif
;

/*
 * }
 */


int
test_dnxc_init_params_parse(int unit, args_t *a, test_dnxc_init_params_t *test_params)
{
    parse_table_t pt;
    int rv;

    parse_table_init(unit, &pt);

    /** parse dnxc appl init params */
    {
        parse_table_add(&pt, "NoDeinit", PQ_INT, (void *) 0,
          &(test_params->appl_init_params.no_deinit), NULL);

        parse_table_add(&pt, "NoInit", PQ_INT, (void *) 0,
          &(test_params->appl_init_params.no_init), NULL);
    }

    /** parse dnxc test params */
    {
        parse_table_add(&pt, "Warmboot", PQ_INT, (void *) 0,
          &(test_params->warmboot), NULL);

        parse_table_add(&pt, "Repeat", PQ_INT, (void *) 1,
          &(test_params->repeat), NULL);
    }


    rv = parse_arg_eq(a, &pt);
    if (rv < 0)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error - failed parsing test arguments.\n")));
        return 1; 
    }

    /*
     * make sure that no extra args were recieved as input
     */
    if (ARG_CNT(a) != 0)
    {
        cli_out("%s: extra options starting with \"%s\"\n",
                ARG_CMD(a), ARG_CUR(a));
        cli_out("%s\n", test_init_deinit_usage);

        return 1;
    }

    /*
     * This is used to free memory allocated for parse_table_init
     */
    parse_arg_eq_done(&pt);

    return 0;
}

/**
 * \brief - Main init deinit test procedure
 * 
 */
int
test_dnxc_init_test(int unit, args_t *a, void *p)
{
    int rv, i;
    test_dnxc_init_params_t *test_params;

    test_params = &test_init_params[unit];

    /** iterate according to "repeat" test parameter */
    for (i = 0; i < test_params->repeat; i++)
    {
        /** Run init deinit application sequence */
        rv = appl_dnxc_init_step_list_run(unit, NULL, &(test_params->appl_init_params));
        if (rv != 0)
        {
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error - dnxc init deinit test failed.\n")));
            return CMD_FAIL;       /** fail */
        }
    }

    return CMD_OK;
}

/**
 * \brief - Init function for init deinit test procedure 
 * This function parses test arguments 
 */
int
test_dnxc_init_test_init(int unit, args_t *a, void **p)
{
    int rv;
    test_dnxc_init_params_t *test_params;

    test_params = &test_init_params[unit];

    cli_out("DNXC Init-Deinit Test - Start\n");

    sal_memset(test_params, 0x0, sizeof(test_dnxc_init_params_t));

    rv = test_dnxc_init_params_parse(unit, a, test_params);
    if (rv != 0)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error - failed parsing arguments.\n")));
        return CMD_FAIL;       /** fail */
    }

    return CMD_OK;
}

/**
 * \brief - Done function for init deinit test procedure 
 */
int
test_dnxc_init_test_done(int unit, void *p)
{
    cli_out("DNXC Init-Deinit Test - End\n");

    return CMD_OK;
}

#endif /* defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT) */
