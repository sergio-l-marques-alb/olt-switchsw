/*
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * File:        ctest_dnx_pparse.c
 * Purpose:     Unit tests for pparse (packet parser and composer) functionality
 */

#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPL_SHELL

#include <shared/shrextend/shrextend_error.h>
#include <shared/utilex/utilex_rhlist.h>
#include <sal/appl/field_types.h>
#include <sal/appl/sal.h>

#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_packet.h>

#include <soc/sand/sand_pparse.h>

typedef enum
{
    PPARSE_COMPOSE_TEST_SIMPLE = 0,
    PPARSE_COMPOSE_TEST_EXPLICIT_CHECKSUM,
    PPARSE_COMPOSE_TEST_EXPLICIT_CHECKSUM_AND_LEN,
    PPARSE_COMPOSE_TEST_IPV4_RAW,

    PPARSE_COMPOSE_NOF_TEST_CASES
} pparse_compose_test_case_e;

typedef struct
{
    int size;
    char *data_str;
} pparse_compose_test_result_t;

/* *INDENT-OFF* */
static pparse_compose_test_result_t pparse_compose_exp_results[PPARSE_COMPOSE_NOF_TEST_CASES] = {
        [PPARSE_COMPOSE_TEST_SIMPLE] = {
                .size = 44 * 8,
                .data_str = "80c900010203040500060708090a0800450000700000000040113c340a141e290a0b0c021314131500000000",
        },
        [PPARSE_COMPOSE_TEST_EXPLICIT_CHECKSUM] = {
                .size = 44 * 8,
                .data_str = "80c900010203040500060708090a0800450000f000000000401155550a141e290a0b0c021314131500000000",
        },
        [PPARSE_COMPOSE_TEST_EXPLICIT_CHECKSUM_AND_LEN] = {
                .size = (18 + 4 + 20 + 8) * 8,
                .data_str = "00010203040500060708090a810000018847000001ff4500abcd000000004011dcba0a0b0c010a0b0c021314131500000000",
        },
        [PPARSE_COMPOSE_TEST_IPV4_RAW] = {
                .size = 20 * 8,
                .data_str = "450000a00000000040004e460a0b0c010a0b0c02",
        },
};
/* *INDENT-ON* */

static shr_error_e
pparse_compose_load_test_case_headers(
    int unit,
    pparse_compose_test_case_e test_case,
    rhhandle_t * packet)
{
    char *test_case_name;
    bcm_port_t src_port;
    SHR_FUNC_INIT_VARS(unit);

    switch (test_case)
    {
        case PPARSE_COMPOSE_TEST_SIMPLE:
            test_case_name = "simple";
            break;
        case PPARSE_COMPOSE_TEST_EXPLICIT_CHECKSUM:
            test_case_name = "ipv4_checksum_error";
            break;
        case PPARSE_COMPOSE_TEST_EXPLICIT_CHECKSUM_AND_LEN:
            test_case_name = "ipv4_explicit_cs_and_len";
            break;
        case PPARSE_COMPOSE_TEST_IPV4_RAW:
            test_case_name = "ipv4_raw";
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "");
    }

    SHR_IF_ERR_EXIT(diag_sand_packet_create(unit, packet));
    SHR_IF_ERR_EXIT(diag_sand_packet_load(unit, test_case_name, *packet, &src_port));

    LOG_CLI(("Test case: %-20s --> ", test_case_name));
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
pparse_compose_validate_test_case_result(
    int unit,
    pparse_compose_test_case_e test_case,
    uint32 *data,
    uint32 size)
{
    char print_data[DSIG_MAX_SIZE_STR];

    SHR_FUNC_INIT_VARS(unit);

    SHR_ASSERT_EQ(pparse_compose_exp_results[test_case].size, size);

    sand_signal_value_to_str(unit, SAL_FIELD_TYPE_NONE, data, print_data, size, PRINT_BIG_ENDIAN);
    if (sal_strncasecmp(print_data, pparse_compose_exp_results[test_case].data_str, DSIG_MAX_SIZE_STR))
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Composed is not as expected.\n%-12s '%s'\n%-12s '%s'",
                     "Expected:", pparse_compose_exp_results[test_case].data_str, "Composed:", print_data);
    }

    LOG_CLI(("OK\n"));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
pparse_compose_execute_test_case(
    int unit,
    pparse_compose_test_case_e test_case)
{
    rhhandle_t packet = NULL;
    uint32 data[DSIG_MAX_SIZE_UINT32];
    uint32 size = 0;

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(pparse_compose_load_test_case_headers(unit, test_case, &packet));
    SHR_IF_ERR_EXIT(pparse_compose(unit, PACKET_PROTO_LIST(packet), PACKET_SIZE(packet),
                                   DSIG_MAX_SIZE_BITS, data, &size));
    SHR_IF_ERR_EXIT(pparse_compose_validate_test_case_result(unit, test_case, data, size));
exit:
    if (packet)
    {
        diag_sand_packet_free(unit, packet);
    }
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_pparse_compose_test(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    pparse_compose_test_case_e test_case;
    SHR_FUNC_INIT_VARS(unit);
    for (test_case = 0; test_case < PPARSE_COMPOSE_NOF_TEST_CASES; ++test_case)
    {
        SHR_IF_ERR_EXIT(pparse_compose_execute_test_case(unit, test_case));
    }
exit:
    SHR_FUNC_EXIT;
}

static sh_sand_option_t dnx_pparse_compose_test_options[] = {
    {NULL}
};

static sh_sand_man_t dnx_pparse_compose_test_man = {
    "Test some cases from XML file"
};

/** List of pparse tests   */
sh_sand_cmd_t dnx_pparse_test_cmds[] = {
    {"compose", dnx_pparse_compose_test, NULL, dnx_pparse_compose_test_options, &dnx_pparse_compose_test_man, NULL,
     NULL, CTEST_PRECOMMIT},
    {NULL}
};
