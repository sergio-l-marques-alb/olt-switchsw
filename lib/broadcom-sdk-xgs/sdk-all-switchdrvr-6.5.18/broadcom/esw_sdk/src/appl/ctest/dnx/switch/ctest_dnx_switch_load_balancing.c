/**
 * \file diag_dnx_switch_load_balancing.c
 *
 * 'Load_Balancing' operations (get/set load_balancing parameters such as 'crc select')
 * for DNX.
 *
 */
/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_SWITCHTESTSDNX

 /*
  * Include files.
  * {
  */
#include "ctest_dnx_switch_load_balancing.h"

#include <bcm_int/dnx/switch/switch_load_balancing.h>
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
 *   Keyword to specify the type of LOAD_BALANCING test to carry out.
 *   See Switch_load_balancing_test_type_enum_table[]
 */
#define DNX_CTEST_LOAD_BALANCING_OPTION_ALL_TEST_TYPE     "type"
/**
 * \brief
 *   Keyword to specify the 'type' element on input tructure 'bcm_switch_control_key_t'.
 *   See bcm_dnx_switch_control_indexed_set()
 */
#define DNX_CTEST_LOAD_BALANCING_OPTION_BCM_TYPE          "mode"
/**
 * \brief
 *   Keyword to specify the 'index' element on input tructure 'bcm_switch_control_key_t'.
 *   See bcm_dnx_switch_control_indexed_set()
 */
#define DNX_CTEST_LOAD_BALANCING_OPTION_BCM_INDEX         "index"
/**
 * \brief
 *   Keyword to specify the 'value' element on input tructure 'bcm_switch_control_info_t'.
 *   See bcm_dnx_switch_control_indexed_set()
 */
#define DNX_CTEST_LOAD_BALANCING_OPTION_BCM_VALUE         "value"
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
 *   List of tests for 'load_balancing'/dnx shell command (to be run on regression, precommit, etc.)
 * \remark
 *   Currently, it will be executed only for regression and precommit.
 */
sh_sand_invoke_t Sh_dnx_switch_load_balancing_dnx_tests[] = {
    {"tcam",             "type=LB_TCAM_INFO",          CTEST_PRECOMMIT}
    ,
    {"phys_log_convert", "type=LB_CLIENT_CONVERT",     CTEST_PRECOMMIT}
    ,
    {"reserved_mpls",    "type=LB_RESERVED_MPLS",      CTEST_PRECOMMIT}
    ,
    {"ident_mpls",       "type=LB_IDENT_MPLS",         CTEST_PRECOMMIT}
    ,
    {"crc_seed",         "type=LB_CRC_SEED",           CTEST_PRECOMMIT}
    ,
    {"bit_select",       "type=LB_BIT_SELECT",         CTEST_PRECOMMIT}
    ,
    {"general_seed",     "type=LB_GENERAL_SEED",       CTEST_PRECOMMIT}
    ,
    /* End of list - must be last. */
    {NULL}
};
/**
 * \brief
 *   List of tests for 'load_balancing'/bcm shell command (to be run on regression, precommit, etc.)
 * \remark
 *   Currently, it will be executed only for regression and precommit.
 */
sh_sand_invoke_t Sh_dnx_switch_load_balancing_bcm_tests[] = {
    {"lb_set_HashSeed", "type=LB_SET mode=HashSeed index=29 value=0x1234",     CTEST_PRECOMMIT}
    ,
    {"lb_get_HashSeed", "type=LB_GET mode=HashSeed index=29",                  CTEST_PRECOMMIT}
    ,
    {"lb_set_ParserHashSeed", "type=LB_SET mode=ParserHashSeed value=0x4321", CTEST_PRECOMMIT}
    ,
    {"lb_get_ParserHashSeed", "type=LB_GET mode=ParserHashSeed",              CTEST_PRECOMMIT}
    ,
    /* End of list - must be last. */
    {NULL}
};
/**
 * \brief
 *   Options list for 'load_balancing/dnx' shell command
 * \remark
 *   Set the default value such that no test is carried out but procedure does return with 'success'
 *   This ensures success on regression and pre-commit:
 */
sh_sand_option_t Sh_dnx_switch_load_balancing_dnx_options[] = {
     /* Name */                                       /* Type */              /* Description */                                                                     /* Default */         /* 'ENUM' table */
    {DNX_CTEST_LOAD_BALANCING_OPTION_ALL_TEST_TYPE,   SAL_FIELD_TYPE_ENUM,    "Type of load_balancing/dnx test (See ctest_dnx_switch_load_balancing_test_type_e)",  "LB_TCAM_INFO", (void *)Switch_load_balancing_test_type_enum_dnx_table},
    /* End of options list - must be last. */
    {NULL}
};
/**
 * \brief
 *   Options list for 'load_balancing' shell command
 * \remark
 *   Set the default value such that no test is carried out but procedure does return with 'success'
 *   This ensures success on regression and pre-commit:
 */
sh_sand_option_t Sh_dnx_switch_load_balancing_bcm_options[] = {
     /* Name */                                       /* Type */              /* Description */                                                       /* Default */         /* 'ENUM' table */
    {DNX_CTEST_LOAD_BALANCING_OPTION_ALL_TEST_TYPE,   SAL_FIELD_TYPE_ENUM,    "Type of load_balancing/bcm test (See ctest_dnx_switch_load_balancing_test_type_e)",  "LB_GET", (void *)Switch_load_balancing_test_type_enum_bcm_table},
    {DNX_CTEST_LOAD_BALANCING_OPTION_BCM_INDEX,       SAL_FIELD_TYPE_UINT32,    "'index' of operation",                                       "29", (void *)NULL},
    {DNX_CTEST_LOAD_BALANCING_OPTION_BCM_TYPE,        SAL_FIELD_TYPE_STR,     "'type' of operation. String from 'bcm_switch_control_t'",    "HgHdrErrToCpu",(void *)NULL},
    {DNX_CTEST_LOAD_BALANCING_OPTION_BCM_VALUE,       SAL_FIELD_TYPE_UINT32,  "'value' of operation",                                       "0",            (void *)NULL},
    /* End of options list - must be last. */
    {NULL}
};
/* *INDENT-ON* */
/*
 * }
 */
/**
 *  Descriptions of testing options.
 */
sh_sand_man_t Sh_dnx_switch_load_balancing_dnx_man = {
    .brief = "'Load_balancing/dnx' related test utilities",
    .full =
        "Activate 'load_balancing' related test utilities as specified by 'type'\r\n"
        "  If type is 'LB_TCAM_INFO' then test\r\n"
        "    dnx_switch_load_balancing_tcam_info_set(),\r\n"
        "    dnx_switch_load_balancing_tcam_info_get()\r\n"
        "  If type is 'LB_CLIENT_CONVERT' then test\r\n"
        "    dnx_switch_lb_physical_client_to_logical(),\r\n"
        "    dnx_switch_lb_logical_client_to_physical()\r\n"
        "  If type is 'LB_RESERVED_MPLS' then test\r\n"
        "    dnx_switch_load_balancing_mpls_reserved_label_set(),\r\n"
        "    dnx_switch_load_balancing_mpls_reserved_label_get()\r\n"
        "  If type is 'LB_IDENT_MPLS' then test\r\n"
        "    dnx_switch_load_balancing_mpls_identification_set(),\r\n"
        "    dnx_switch_load_balancing_mpls_identification_get()\r\n"
        "  If type is 'LB_CRC_SEED' then test\r\n"
        "    dnx_switch_load_balancing_crc_seed_set(),\r\n"
        "    dnx_switch_load_balancing_crc_seed_get()\r\n"
        "  If type is 'LB_BIT_SELECT' then test\r\n"
        "    dnx_switch_load_balancing_lsms_crc_select_set(),\r\n"
        "    dnx_switch_load_balancing_lsms_crc_select_get()\r\n"
        "  If type is 'LB_GENERAL_SEED' then test\r\n"
        "    dnx_switch_load_balancing_general_seeds_set(),\r\n"
        "    dnx_switch_load_balancing_general_seeds_get()\r\n",
    .synopsis =
        "type=<LB_TCAM_INFO | LB_CLIENT_CONVERT | LB_RESERVED_MPLS | LB_IDENT_MPLS | LB_CRC_SEED | LB_BIT_SELECT | LB_GENERAL_SEED>"
        " level=<BCM | DNX> ",
    .examples = "type=LB_TCAM_INFO\r\n" "type=LB_IDENT_MPLS\r\n" "type=LB_CRC_SEED"
};

sh_sand_man_t Sh_dnx_switch_load_balancing_bcm_man = {
    .brief = "'Load_balancing/bcm' related test utilities",
    .full =
        "Activate 'load_balancing' related test utilities as specified by 'type'\r\n"
        "  If type is 'LB_SET' then test\r\n"
        "    bcm_dnx_switch_control_indexed_set()\r\n"
        "  If type is 'LB_GET' then test\r\n"
        "    bcm_dnx_switch_control_indexed_get()\r\n"
        "  'mode' is the requested operation as per long list\r\n"
        "    on 'bcm_switch_control_t' (See BCM_SWITCH_CONTROL_STR and\r\n"
        "    list on bcm_dnx_switch_control_indexed_set())\r\n"
        "    Current acceptable list for load balancing:\r\n"
        "      HashSeed                 (dnx_switch_load_balancing_crc_seed_set()/get())         \r\n"
        "      LayerRecordModeSelection (dnx_switch_load_balancing_lsms_crc_select_set()/get()) \r\n"
        "      ECMPHashConfig           (dnx_switch_load_balancing_tcam_info_get()/set())       \r\n"
        "      ECMPSecondHierHashConfig (dnx_switch_load_balancing_tcam_info_get()/set())       \r\n"
        "      ECMPThirdHierHashConfig  (dnx_switch_load_balancing_tcam_info_get()/set())       \r\n"
        "      TrunkHashConfig          (dnx_switch_load_balancing_tcam_info_get()/set())       \r\n"
        "      NwkHashConfig            (dnx_switch_load_balancing_tcam_info_get()/set())       \r\n"
        "      MplsStack0HashSeed       (dnx_switch_load_balancing_general_seeds_get()/set())   \r\n"
        "      MplsStack1HashSeed       (dnx_switch_load_balancing_general_seeds_get()/set())   \r\n"
        "      ParserHashSeed           (dnx_switch_load_balancing_general_seeds_get()/set())   \r\n"
        "  'value' and 'index' are general parameters corresponding to\r\n"
        "    specified 'mode'                                                              \r\n",
    .synopsis =
        "type=<LB_SET | LB_GET> mode=<HashSeed, LayerRecordModeSelection, ECMPHashConfig, ...> value=<General value related to 'mode'> index=<General index related to 'mode'>",
    .examples = "type=LB_SET mode=HashSeed index=29 value=0x1234 \r\n" "type=LB_GET mode=HashSeed index=29"
};

/**
 * \brief
 *   Performs test of writing and reading from 'IPPB_LOAD_BALANCING_CRC_SELECTION_CAM'.
 *   Test is destructive!!
 * \param [in] unit -
 *   The unit number.
 * \param [in] args -
 *   Null terminated string. 'Options': List of arguments as shown on screen (typed by caller/user)
 * \param [in] sand_control -
 *   Control information related to each of the 'options' entered by the caller (and contained in
 *   'args')
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
appl_dnx_lb_test_cam(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int index;
    dnx_switch_lb_tcam_key_t set_tcam_key, get_tcam_key;
    dnx_switch_lb_tcam_result_t set_tcam_result, get_tcam_result;
    dnx_switch_lb_valid_t set_tcam_valid, get_tcam_valid;
    int ii;
    /**
     * This variables stores the severity of the load balancing on dnx
     */
    bsl_severity_t original_severity_lb;
    int rv;

    SHR_FUNC_INIT_VARS(unit);
    /**
     * Since this procedure includes negative tests, error logs are expected.
     * To avoid seeing these messages, we store the original severity here and, in case of negative tests,
     * increase the severity to 'fatal' so that standard errors will not show on the screen.
     * Upon exit, we restore the original severity.
     */
    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_LB, original_severity_lb);

    sal_memset(&set_tcam_result, 0, sizeof(dnx_switch_lb_tcam_result_t));
    sal_memset(&get_tcam_result, 0, sizeof(dnx_switch_lb_tcam_result_t));
    index = 0;
    set_tcam_valid = 1;
    set_tcam_key.lb_destination = 0x012345;
    set_tcam_key.lb_destination_mask = 0x1ABCDE;
    set_tcam_key.outlif_0 = 0x0354321;
    set_tcam_key.outlif_0_mask = 0x3EDCBA;

    for (ii = 0; ii < dnx_data_switch.load_balancing.nof_lb_clients_get(unit); ii++)
    {
        set_tcam_result.client_x_crc_16_sel[ii] = ii;
    }
    SHR_IF_ERR_EXIT(dnx_switch_load_balancing_tcam_info_set
                    (unit, index, &set_tcam_key, &set_tcam_result, set_tcam_valid));
    /*
     * Now retrieve the same entry.
     */
    SHR_IF_ERR_EXIT(dnx_switch_load_balancing_tcam_info_get
                    (unit, index, &get_tcam_key, &get_tcam_result, &get_tcam_valid));
    /*
     * Compare 'get' and 'set'.
     */
    if (sal_memcmp(&set_tcam_key, &get_tcam_key, sizeof(set_tcam_key)) != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "'TCAM key' on 'get' is not the same as on 'set'. Probably internal error (sw or hw).");
    }
    if (sal_memcmp(&set_tcam_result, &get_tcam_result, sizeof(set_tcam_result)) != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "'TCAM result' on 'get' is not the same as on 'set'. Probably internal error (sw or hw).");
    }
    if (set_tcam_valid != get_tcam_valid)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "'TCAM valid' on 'get' (%d) is not the same as on 'set' (%d). Probably internal error (sw or hw).",
                     get_tcam_valid, set_tcam_valid);
    }
    {
        /*
         * Negative test:
         * Try two CRC functions which are the same for two different clients. Should fail.
         */
        /**
         * Increase the severity to 'fatal' to avoid seing errors messages on the screen.
         */
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_LB, bslSeverityFatal);
        set_tcam_result.client_x_crc_16_sel[0] = 0;
        set_tcam_result.client_x_crc_16_sel[1] = 0;
        rv = dnx_switch_load_balancing_tcam_info_set(unit, index, &set_tcam_key, &set_tcam_result, set_tcam_valid);
        if (rv != _SHR_E_NONE)
        {
            LOG_INFO_EX(BSL_LOG_MODULE,
                        "%s(), line %d, NEGATIVE TEST for dnx_switch_load_balancing_tcam_info_set() (Two similar CRC functions) has failed AS EXPECTED! %s%s\r\n",
                        __FUNCTION__, __LINE__, EMPTY, EMPTY);
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_FAIL,
                         "NEGATIVE TEST for dnx_switch_load_balancing_tcam_info_set() (Two similar CRC functions) has succeeded. Test has FAILED!\r\n");
        }
        /**
         *  Restore the original severity after the end of Negative test.
         */
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_LB, original_severity_lb);
    }
exit:
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_LB, original_severity_lb);
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   Performs test of writing and reading from 'LOAD_BALANCING_GENERAL_SEEDS'.
 *   Test is destructive!!
 * \param [in] unit -
 *   The unit number.
 * \param [in] args -
 *   Null terminated string. 'Options': List of arguments as shown on
 *   screen (typed by caller/user)
 * \param [in] sand_control -
 *   Control information related to each of the 'options' entered by the
 *   caller (and contained in 'args')
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
appl_dnx_lb_test_general_seed(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    dnx_switch_lb_general_seeds_t set_general_seeds, get_general_seeds;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Do this in case of alignment issue
     */
    sal_memset(&set_general_seeds, 0, sizeof(set_general_seeds));
    sal_memset(&get_general_seeds, 0, sizeof(get_general_seeds));
    set_general_seeds.mpls_stack_0 = 0xABCDDCBA;
    set_general_seeds.mpls_stack_1 = 0x12344321;
    set_general_seeds.parser = 0x56788765;

    SHR_IF_ERR_EXIT(dnx_switch_load_balancing_general_seeds_set(unit, &set_general_seeds));
    /*
     * Now retrieve the same entry.
     */
    SHR_IF_ERR_EXIT(dnx_switch_load_balancing_general_seeds_get(unit, &get_general_seeds));
    /*
     * Compare 'get' and 'set'.
     */
    if (sal_memcmp(&set_general_seeds, &get_general_seeds, sizeof(set_general_seeds)) != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "'general seeds' on 'get' is not the same as on 'set'. Probably internal error (sw or hw).");
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   Performs test of writing and reading from 'IPPB_MPLS_RESERVED_LABEL_TYPES'.
 *   Test is destructive!!
 * \param [in] unit -
 *   The unit number.
 * \param [in] args -
 *   Null terminated string. 'Options': List of arguments as shown on
 *   screen (typed by caller/user)
 * \param [in] sand_control -
 *   Control information related to each of the 'options' entered by the
 *   caller (and contained in 'args')
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
appl_dnx_lb_test_reserved(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    dnx_switch_lb_mpls_reserved_t set_mpls_reserved, get_mpls_reserved;
    /**
     * This variables stores the severity of the load balancing on dnx
     */
    bsl_severity_t original_severity_lb;
    int rv;

    SHR_FUNC_INIT_VARS(unit);
    /**
     * Since this procedure includes negative tests, error logs are expected.
     * To avoid seeing these messages, we store the original severity here and, in case of negative tests,
     * increase the severity to 'fatal' so that standard errors will not show on the screen.
     * Upon exit, we restore the original severity.
     */
    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_LB, original_severity_lb);

    sal_memset(&set_mpls_reserved, 0, sizeof(dnx_switch_lb_mpls_reserved_t));
    set_mpls_reserved.reserved_current_label_indication = 0xABCD;
    set_mpls_reserved.reserved_current_label_force_layer = 0xDCBA;
    set_mpls_reserved.reserved_next_label_indication = 0x1234;
    if (dnx_data_switch.load_balancing.reserved_next_label_valid_get(unit))
    {
        set_mpls_reserved.reserved_next_label_valid = 0x1234;
    }
    SHR_IF_ERR_EXIT(dnx_switch_load_balancing_mpls_reserved_label_set(unit, &set_mpls_reserved));
    /*
     * Now retrieve the same entry.
     */
    SHR_IF_ERR_EXIT(dnx_switch_load_balancing_mpls_reserved_label_get(unit, &get_mpls_reserved));
    /*
     * Compare 'get' and 'set'.
     */
    if (sal_memcmp(&set_mpls_reserved, &get_mpls_reserved, sizeof(set_mpls_reserved)) != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "'mpls_reserved' on 'get' is not the same as on 'set'. Probably internal error (sw or hw).");
    }
    if (dnx_data_switch.load_balancing.reserved_next_label_valid_get(unit))    
    {
        /*
         * Negative test:
         * Try 'reserved_next_label_valid' which is not equal to 'reserved_next_label_indication'. Should fail.
         */
        /**
         * Increase the severity to 'fatal' to avoid seing errors messages on the screen.
         */
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_LB, bslSeverityFatal);
        set_mpls_reserved.reserved_next_label_valid = 0x4321;
        rv = dnx_switch_load_balancing_mpls_reserved_label_set(unit, &set_mpls_reserved);
        if (rv != _SHR_E_NONE)
        {
            LOG_INFO_EX(BSL_LOG_MODULE,
                        "%s(), line %d, NEGATIVE TEST for dnx_switch_load_balancing_mpls_reserved_label_set() ('valid' is not the same as 'next_label') has failed AS EXPECTED! %s%s\r\n",
                        __FUNCTION__, __LINE__, EMPTY, EMPTY);
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_FAIL,
                         "NEGATIVE TEST for dnx_switch_load_balancing_mpls_reserved_label_set() ('valid' is not the same as 'next_label') has succeeded. Test has FAILED!\r\n");
        }
        /**
         *  Restore the original severity after the end of Negative test.
         */
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_LB, original_severity_lb);
    }
exit:
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_LB, original_severity_lb);
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   Performs test of writing and reading from 'IPPB_MPLS_LAYER_IDENTIFICATION_BITMAP_CFG'.
 *   Test is destructive!!
 * \param [in] unit -
 *   The unit number.
 * \param [in] args -
 *   Null terminated string. 'Options': List of arguments as shown on
 *   screen (typed by caller/user)
 * \param [in] sand_control -
 *   Control information related to each of the 'options' entered by the
 *   caller (and contained in 'args')
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
appl_dnx_lb_test_ident(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    dnx_switch_lb_mpls_identification_t set_mpls_identification, get_mpls_identification;
    /**
     * This variables stores the severity of the load balancing on dnx
     */
    bsl_severity_t original_severity_lb;
    int rv;

    SHR_FUNC_INIT_VARS(unit);
    /**
     * Since this procedure includes negative tests, error logs are expected.
     * To avoid seeing these messages, we store the original severity here and, in case of negative tests,
     * increase the severity to 'fatal' so that standard errors will not show on the screen.
     * Upon exit, we restore the original severity.
     */
    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_LB, original_severity_lb);
    set_mpls_identification.mpls_identification = SAL_BIT(10);
    SHR_IF_ERR_EXIT(dnx_switch_load_balancing_mpls_identification_set(unit, &set_mpls_identification));
    /*
     * Now retrieve the same entry.
     */
    SHR_IF_ERR_EXIT(dnx_switch_load_balancing_mpls_identification_get(unit, &get_mpls_identification));
    /*
     * Compare 'get' and 'set'.
     */
    if (sal_memcmp(&set_mpls_identification, &get_mpls_identification, sizeof(set_mpls_identification)) != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "'mpls_identification' on 'get' is not the same as on 'set'. Probably internal error (sw or hw).");
    }
    {
        /*
         * Negative test:
         * Try number of MPLS protocol identifiers which is larger than supported. Should fail.
         */
        /**
         * Increase the severity to 'fatal' to avoid seing errors messages on the screen.
         */
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_LB, bslSeverityFatal);
        set_mpls_identification.mpls_identification = SAL_BIT(18) | SAL_BIT(10) | SAL_BIT(20) | SAL_BIT(22);
        rv = dnx_switch_load_balancing_mpls_identification_set(unit, &set_mpls_identification);
        if (rv != _SHR_E_NONE)
        {
            LOG_INFO_EX(BSL_LOG_MODULE,
                        "%s(), line %d, NEGATIVE TEST for dnx_switch_load_balancing_mpls_identification_set() (Two bits set on identifying reg.) has failed AS EXPECTED! %s%s\r\n",
                        __FUNCTION__, __LINE__, EMPTY, EMPTY);
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_FAIL,
                         "NEGATIVE TEST for dnx_switch_load_balancing_mpls_identification_set() (Two bits set on identifying reg.) has succeeded. Test has FAILED!\r\n");
        }
        /**
         *  Restore the original severity after the end of Negative test.
         */
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_LB, original_severity_lb);
    }
exit:
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_LB, original_severity_lb);
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   Performs test of writing and reading from 'IPPB_MPLS_CRC_16_SEED'.
 *   Test is destructive!!
 * \param [in] unit -
 *   The unit number.
 * \param [in] args -
 *   Null terminated string. 'Options': List of arguments as shown on
 *   screen (typed by caller/user)
 * \param [in] sand_control -
 *   Control information related to each of the 'options' entered by the
 *   caller (and contained in 'args')
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
appl_dnx_lb_test_crc_seed(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 crc_hw_function_index = 4; /** Just a CRC function HW index from the range */
    uint16 seed_value = 0x5AA5;
    uint16 get_crc_seed;
    /**
     * This variables stores the severity of the load balancing on dnx
     */
    bsl_severity_t original_severity_lb;
    int rv;

    SHR_FUNC_INIT_VARS(unit);
    /**
     * Since this procedure includes negative tests, error logs are expected.
     * To avoid seeing these messages, we store the original severity here and, in case of negative tests,
     * increase the severity to 'fatal' so that standard errors will not show on the screen.
     * Upon exit, we restore the original severity.
     */
    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_LB, original_severity_lb);

    SHR_IF_ERR_EXIT(dnx_switch_load_balancing_crc_seed_set(unit, crc_hw_function_index, seed_value));
    /*
     * Now retrieve the same entry.
     */
    SHR_IF_ERR_EXIT(dnx_switch_load_balancing_crc_seed_get(unit, crc_hw_function_index, &get_crc_seed));
    /*
     * Compare 'get' and 'set'.
     */
    if (get_crc_seed != seed_value)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "'crc seed' on 'get' is not the same as on 'set'. Probably internal error (sw or hw).");
    }
    {
        /*
         * Negative test:
         * Try crc function identifier which is out of range. Should fail.
         */
        /**
         * Increase the severity to 'fatal' to avoid seing errors messages on the screen.
         */
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_LB, bslSeverityFatal);

        crc_hw_function_index = 2 * dnx_data_switch.load_balancing.nof_crc_functions_get(unit);

        rv = dnx_switch_load_balancing_crc_seed_set(unit, crc_hw_function_index, seed_value);
        if (rv != _SHR_E_NONE)
        {
            LOG_INFO_EX(BSL_LOG_MODULE,
                        "%s(), line %d, NEGATIVE TEST for dnx_switch_load_balancing_crc_seed_set() (crc function too high) has failed AS EXPECTED! %s%s\r\n",
                        __FUNCTION__, __LINE__, EMPTY, EMPTY);
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_FAIL,
                         "NEGATIVE TEST for dnx_switch_load_balancing_crc_seed_set() (crc function too high) has succeeded. Test has FAILED!\r\n");
        }
        /**
         *  Restore the original severity after the end of Negative test.
         */
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_LB, original_severity_lb);
    }
exit:
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_LB, original_severity_lb);
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   Performs test of writing and reading from 'IPPB_LB_CLIENTS_CRC_32_BITS_SELECTION_BITMAP'.
 *   Test is destructive!!
 * \param [in] unit -
 *   The unit number.
 * \param [in] args -
 *   Null terminated string. 'Options': List of arguments as shown on
 *   screen (typed by caller/user)
 * \param [in] sand_control -
 *   Control information related to each of the 'options' entered by the
 *   caller (and contained in 'args')
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
appl_dnx_lb_test_bit_select(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    dnx_switch_lb_lsms_crc_select_t set_bit_select, get_bit_select;
    /**
     * This variables stores the severity of the load balancing on dnx
     */
    bsl_severity_t original_severity_lb;
    int rv;
    uint8 lb_client, max_lb_client;

    SHR_FUNC_INIT_VARS(unit);
    /**
     * Since this procedure includes negative tests, error logs are expected.
     * To avoid seeing these messages, we store the original severity here and, in case of negative tests,
     * increase the severity to 'fatal' so that standard errors will not show on the screen.
     * Upon exit, we restore the original severity.
     */
    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_LB, original_severity_lb);
    max_lb_client = dnx_data_switch.load_balancing.nof_lb_clients_get(unit);
    for (lb_client = 0; lb_client < max_lb_client; lb_client++)
    {
        sal_memset(&set_bit_select, 0, sizeof(set_bit_select));
        sal_memset(&get_bit_select, 0, sizeof(get_bit_select));
        set_bit_select.lb_client = lb_client;
        set_bit_select.lb_selection_bit_map = 0x00A0 + lb_client;
        SHR_IF_ERR_EXIT(dnx_switch_load_balancing_lsms_crc_select_set(unit, &set_bit_select));
        /*
         * Now retrieve the same entry.
         */
        get_bit_select.lb_client = set_bit_select.lb_client;
        SHR_IF_ERR_EXIT(dnx_switch_load_balancing_lsms_crc_select_get(unit, &get_bit_select));
        /*
         * Compare 'get' and 'set'.
         */
        if (sal_memcmp(&set_bit_select, &get_bit_select, sizeof(set_bit_select)) != 0)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "'select bitmap' on 'get' is not the same as on 'set' for client %d. Probably internal error (sw or hw).",
                         lb_client);
        }
    }
    {
        /*
         * Negative test:
         * Try crc function identifier which is out of range. Should fail.
         */
        /**
         * Increase the severity to 'fatal' to avoid seing errors messages on the screen.
         */
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_LB, bslSeverityFatal);
        set_bit_select.lb_client = 26;
        rv = dnx_switch_load_balancing_lsms_crc_select_set(unit, &set_bit_select);
        if (rv != _SHR_E_NONE)
        {
            LOG_INFO_EX(BSL_LOG_MODULE,
                        "%s(), line %d, NEGATIVE TEST for dnx_switch_load_balancing_lsms_crc_select_set() (lb_client too high) has failed AS EXPECTED! %s%s\r\n",
                        __FUNCTION__, __LINE__, EMPTY, EMPTY);
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_FAIL,
                         "NEGATIVE TEST for dnx_switch_load_balancing_lsms_crc_select_set() (lb_client too high) has succeeded. Test has FAILED!\r\n");
        }
        set_bit_select.lb_client = 0;
        set_bit_select.lb_selection_bit_map = -1;
        rv = dnx_switch_load_balancing_lsms_crc_select_set(unit, &set_bit_select);
        if (rv != _SHR_E_NONE)
        {
            LOG_INFO_EX(BSL_LOG_MODULE,
                        "%s(), line %d, NEGATIVE TEST for dnx_switch_load_balancing_lsms_crc_select_set() (bit map too high) has failed AS EXPECTED! %s%s\r\n",
                        __FUNCTION__, __LINE__, EMPTY, EMPTY);
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_FAIL,
                         "NEGATIVE TEST for dnx_switch_load_balancing_lsms_crc_select_set() (bit map too high) has succeeded. Test has FAILED!\r\n");
        }
        /**
         *  Restore the original severity after the end of Negative test.
         */
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_LB, original_severity_lb);
    }
exit:
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_LB, original_severity_lb);
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   Performs test of converting logical client to physical (as on 'IPPB_LOAD_BALANCING_CRC_SELECTION_CAM')
 *   and back.
 * \param [in] unit -
 *   The unit number.
 * \param [in] args -
 *   Null terminated string. 'Options': List of arguments as shown on screen (typed by caller/user)
 * \param [in] sand_control -
 *   Control information related to each of the 'options' entered by the caller (and contained in
 *   'args')
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
appl_dnx_lb_test_phys_log_convert(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 ii, num_selects, physical_client;
    bcm_switch_control_t logical_client;

    SHR_FUNC_INIT_VARS(unit);
    num_selects = DNX_DATA_MAX_SWITCH_LOAD_BALANCING_NOF_LB_CLIENTS;
    for (ii = 0; ii < num_selects; ii++)
    {
        SHR_IF_ERR_EXIT(dnx_switch_lb_physical_client_to_logical(unit, ii, &logical_client));
        SHR_IF_ERR_EXIT(dnx_switch_lb_logical_client_to_physical(unit, logical_client, &physical_client));
        if (physical_client != ii)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "'physical client' id (%d) was converted to logical (%d) but when convertin back, it was (%d). Probably internal error (sw or hw).",
                         ii, logical_client, physical_client);
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   This function is the basic 'load_balancing/bcm' testing
 *   application.
 *
 * \param [in] unit -
 *   The unit number.
 * \param [in] args -
 *   Null terminated string. 'Options': List of arguments as shown on screen (typed by caller/user)
 * \param [in] sand_control -
 *   Control information related to each of the 'options' entered by the caller (and contained in
 *   'args')
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
appl_dnx_load_balancing_bcm_starter(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    ctest_dnx_switch_load_balancing_test_type_e lb_test_type;

    SHR_FUNC_INIT_VARS(unit);
    SH_SAND_GET_ENUM(DNX_CTEST_LOAD_BALANCING_OPTION_ALL_TEST_TYPE, lb_test_type);
    {
        bcm_switch_control_key_t key;
        bcm_switch_control_info_t value_for_set, value_for_get;
        char *switch_str;

        SH_SAND_GET_STR(DNX_CTEST_LOAD_BALANCING_OPTION_BCM_TYPE, switch_str);
        /*
         * Convert input string, as per 'bcm_switch_control_t' to the value of that enunmeration.
         */
        SHR_IF_ERR_EXIT(dnx_switch_control_t_value(unit, switch_str, &(key.type)));
        if ((int) key.type < 0)
        {
            LOG_INFO_EX(BSL_LOG_MODULE,
                        "%s(), line %d, String enumeration (%s) could not be found on 'bcm_switch_control_t'. Wrong input %s\r\n",
                        __FUNCTION__, __LINE__, switch_str, EMPTY);
            SHR_IF_ERR_EXIT(_SHR_E_PARAM);
        }
        SH_SAND_GET_UINT32(DNX_CTEST_LOAD_BALANCING_OPTION_BCM_INDEX, key.index);
        SH_SAND_GET_UINT32(DNX_CTEST_LOAD_BALANCING_OPTION_BCM_VALUE, value_for_set.value);
        switch (lb_test_type)
        {
            case CTEST_DNX_SWITCH_LOAD_BALANCING_TEST_CTRL_INDEX_SET_TYPE:
            {
                LOG_INFO_EX(BSL_LOG_MODULE,
                            "%s(), line %d, BCM: Tesing general bcm_dnx_switch_control_indexed_set(). %s%s\r\n",
                            __FUNCTION__, __LINE__, EMPTY, EMPTY);
                if ((key.type == bcmSwitchECMPHashConfig) ||
                    (key.type == bcmSwitchECMPSecondHierHashConfig) ||
                    (key.type == bcmSwitchECMPThirdHierHashConfig) ||
                    (key.type == bcmSwitchTrunkHashConfig) ||
                    (key.type == bcmSwitchNwkHashConfig) ||
                    (key.type == bcmSwitchParserHashSeed) ||
                    (key.type == bcmSwitchMplsStack1HashSeed) || (key.type == bcmSwitchMplsStack0HashSeed))
                {
                    bcm_switch_control_t type;
                    int arg_for_set;
                    int arg_for_get;
                    type = (bcm_switch_control_t) (key.type);
                    arg_for_set = (int) (value_for_set.value);
                    SHR_IF_ERR_EXIT(bcm_switch_control_set(unit, type, arg_for_set));
                    SHR_IF_ERR_EXIT(bcm_switch_control_get(unit, type, &arg_for_get));
                    if (arg_for_get != arg_for_set)
                    {
                        LOG_INFO_EX(BSL_LOG_MODULE,
                                    "%s(), line %d, 'Value' on set (%d, 0x%08X) is not equal to 'value' on get. Probably internal error (sw or hw)\r\n",
                                    __FUNCTION__, __LINE__, arg_for_set, arg_for_get);
                        SHR_IF_ERR_EXIT(_SHR_E_INTERNAL);
                    }
                    LOG_INFO_EX(BSL_LOG_MODULE,
                                "%s(), line %d, BCM Option %s. Tesing general bcm_dnx_switch_control_set() returned 'success' flag. %s\r\n",
                                __FUNCTION__, __LINE__,
                                SH_SAND_GET_ENUM_STR(DNX_CTEST_LOAD_BALANCING_OPTION_ALL_TEST_TYPE, lb_test_type),
                                EMPTY);
                }
                else
                {
                    SHR_IF_ERR_EXIT(bcm_switch_control_indexed_set(unit, key, value_for_set));
                    SHR_IF_ERR_EXIT(bcm_switch_control_indexed_get(unit, key, &value_for_get));
                    if (value_for_get.value != value_for_set.value)
                    {
                        LOG_INFO_EX(BSL_LOG_MODULE,
                                    "%s(), line %d, 'Value' on set (%d, 0x%08X) is not equal to 'value' on get. Probably internal error (sw or hw)\r\n",
                                    __FUNCTION__, __LINE__, value_for_set.value, value_for_get.value);
                        SHR_IF_ERR_EXIT(_SHR_E_INTERNAL);
                    }
                    LOG_INFO_EX(BSL_LOG_MODULE,
                                "%s(), line %d, BCM Option %s. Tesing general bcm_dnx_switch_control_indexed_set() returned 'success' flag. %s\r\n",
                                __FUNCTION__, __LINE__,
                                SH_SAND_GET_ENUM_STR(DNX_CTEST_LOAD_BALANCING_OPTION_ALL_TEST_TYPE, lb_test_type),
                                EMPTY);
                }
                break;
            }
            case CTEST_DNX_SWITCH_LOAD_BALANCING_TEST_CTRL_INDEX_GET_TYPE:
            {
                LOG_INFO_EX(BSL_LOG_MODULE,
                            "%s(), line %d, BCM: Tesing general bcm_dnx_switch_control_indexed_get(). %s%s\r\n",
                            __FUNCTION__, __LINE__, EMPTY, EMPTY);
                if ((key.type == bcmSwitchECMPHashConfig) ||
                    (key.type == bcmSwitchECMPSecondHierHashConfig) ||
                    (key.type == bcmSwitchECMPThirdHierHashConfig) ||
                    (key.type == bcmSwitchTrunkHashConfig) ||
                    (key.type == bcmSwitchNwkHashConfig) ||
                    (key.type == bcmSwitchParserHashSeed) ||
                    (key.type == bcmSwitchMplsStack1HashSeed) || (key.type == bcmSwitchMplsStack0HashSeed))
                {
                    bcm_switch_control_t type;
                    int arg_for_get;
                    type = (bcm_switch_control_t) (key.type);
                    SHR_IF_ERR_EXIT(bcm_switch_control_get(unit, type, &arg_for_get));
                    LOG_CLI_EX("Returned 'value' is %d (0x%08X) %s%s\r\n", arg_for_get, arg_for_get, EMPTY, EMPTY);
                    LOG_INFO_EX(BSL_LOG_MODULE,
                                "%s(), line %d, BCM Option %s. Tesing general bcm_dnx_switch_control_set() returned 'success' flag. %s\r\n",
                                __FUNCTION__, __LINE__,
                                SH_SAND_GET_ENUM_STR(DNX_CTEST_LOAD_BALANCING_OPTION_ALL_TEST_TYPE, lb_test_type),
                                EMPTY);
                }
                else
                {
                    SHR_IF_ERR_EXIT(bcm_switch_control_indexed_get(unit, key, &value_for_get));
                    LOG_CLI_EX("Returned 'value' is %d (0x%08X) %s%s\r\n",
                               value_for_get.value, value_for_get.value, EMPTY, EMPTY);
                    LOG_INFO_EX(BSL_LOG_MODULE,
                                "%s(), line %d, BCM Option %s. Tesing general bcm_dnx_switch_control_indexed_get() returned 'success' flag. %s\r\n",
                                __FUNCTION__, __LINE__,
                                SH_SAND_GET_ENUM_STR(DNX_CTEST_LOAD_BALANCING_OPTION_ALL_TEST_TYPE, lb_test_type),
                                EMPTY);
                }
                break;
            }
            default:
            {
                LOG_INFO_EX(BSL_LOG_MODULE,
                            "%s(), line %d, Option %d for 'test type' on 'BCM' is not implemented. Illegal parameter. %s\r\n",
                            __FUNCTION__, __LINE__, lb_test_type, EMPTY);
                SHR_IF_ERR_EXIT(_SHR_E_PARAM);
                break;
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function is the basic 'load_balancing/dnx' testing
 *   application.
 *
 * \param [in] unit -
 *   The unit number.
 * \param [in] args -
 *   Null terminated string. 'Options': List of arguments as shown on screen (typed by caller/user)
 * \param [in] sand_control -
 *   Control information related to each of the 'options' entered by the caller (and contained in
 *   'args')
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
appl_dnx_load_balancing_dnx_starter(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    ctest_dnx_switch_load_balancing_test_type_e lb_test_type;

    SHR_FUNC_INIT_VARS(unit);
    SH_SAND_GET_ENUM(DNX_CTEST_LOAD_BALANCING_OPTION_ALL_TEST_TYPE, lb_test_type);
    switch (lb_test_type)
    {
        case CTEST_DNX_SWITCH_LOAD_BALANCING_TEST_TCAM_INFO_TYPE:
        {
            LOG_INFO_EX(BSL_LOG_MODULE,
                        "%s(), line %d, Tesing 'IPPB_LOAD_BALANCING_CRC_SELECTION_CAM' for get and set. %s%s\r\n",
                        __FUNCTION__, __LINE__, EMPTY, EMPTY);
            SHR_IF_ERR_EXIT(appl_dnx_lb_test_cam(unit, args, sand_control));
            LOG_INFO_EX(BSL_LOG_MODULE,
                        "%s(), line %d, DNX option %s. Testing of 'IPPB_LOAD_BALANCING_CRC_SELECTION_CAM' returned 'success' flag. %s\r\n",
                        __FUNCTION__, __LINE__,
                        SH_SAND_GET_ENUM_STR(DNX_CTEST_LOAD_BALANCING_OPTION_ALL_TEST_TYPE, lb_test_type), EMPTY);
            break;
        }
        case CTEST_DNX_SWITCH_LOAD_BALANCING_TEST_CLIENT_CONVERT_TYPE:
        {
            LOG_INFO_EX(BSL_LOG_MODULE,
                        "%s(), line %d, Tesing convertions: physical<-> logical client. %s%s\r\n",
                        __FUNCTION__, __LINE__, EMPTY, EMPTY);
            SHR_IF_ERR_EXIT(appl_dnx_lb_test_phys_log_convert(unit, args, sand_control));
            LOG_INFO_EX(BSL_LOG_MODULE,
                        "%s(), line %d, DNX option %s. Testing of 'convertions: physical<-> logical client' returned 'success' flag. %s\r\n",
                        __FUNCTION__, __LINE__,
                        SH_SAND_GET_ENUM_STR(DNX_CTEST_LOAD_BALANCING_OPTION_ALL_TEST_TYPE, lb_test_type), EMPTY);
            break;
        }
        case CTEST_DNX_SWITCH_LOAD_BALANCING_TEST_MPLS_RESERVED_TYPE:
        {
            LOG_INFO_EX(BSL_LOG_MODULE,
                        "%s(), line %d, Tesing loading of controls on 'reserved' mpls labels. %s%s\r\n",
                        __FUNCTION__, __LINE__, EMPTY, EMPTY);
            SHR_IF_ERR_EXIT(appl_dnx_lb_test_reserved(unit, args, sand_control));
            LOG_INFO_EX(BSL_LOG_MODULE,
                        "%s(), line %d, DNX option %s. Testing of Tesing controls on 'reserved' mpls labels returned 'success' flag. %s\r\n",
                        __FUNCTION__, __LINE__,
                        SH_SAND_GET_ENUM_STR(DNX_CTEST_LOAD_BALANCING_OPTION_ALL_TEST_TYPE, lb_test_type), EMPTY);
            break;
        }
        case CTEST_DNX_SWITCH_LOAD_BALANCING_TEST_MPLS_IDENT_TYPE:
        {
            LOG_INFO_EX(BSL_LOG_MODULE,
                        "%s(), line %d, Tesing loading of controls on 'identification' of mpls protocol. %s%s\r\n",
                        __FUNCTION__, __LINE__, EMPTY, EMPTY);
            SHR_IF_ERR_EXIT(appl_dnx_lb_test_ident(unit, args, sand_control));
            LOG_INFO_EX(BSL_LOG_MODULE,
                        "%s(), line %d, DNX option %s. Tesing controls on 'identification' of mpls protocol returned 'success' flag. %s\r\n",
                        __FUNCTION__, __LINE__,
                        SH_SAND_GET_ENUM_STR(DNX_CTEST_LOAD_BALANCING_OPTION_ALL_TEST_TYPE, lb_test_type), EMPTY);
            break;
        }
        case CTEST_DNX_SWITCH_LOAD_BALANCING_TEST_CRC_SEED_TYPE:
        {
            LOG_INFO_EX(BSL_LOG_MODULE,
                        "%s(), line %d, Tesing loading crc seed for some crc function. %s%s\r\n",
                        __FUNCTION__, __LINE__, EMPTY, EMPTY);
            SHR_IF_ERR_EXIT(appl_dnx_lb_test_crc_seed(unit, args, sand_control));
            LOG_INFO_EX(BSL_LOG_MODULE,
                        "%s(), line %d, DNX option %s. Tesing loading crc seed for some crc function returned 'success' flag. %s\r\n",
                        __FUNCTION__, __LINE__,
                        SH_SAND_GET_ENUM_STR(DNX_CTEST_LOAD_BALANCING_OPTION_ALL_TEST_TYPE, lb_test_type), EMPTY);
            break;
        }
        case CTEST_DNX_SWITCH_LOAD_BALANCING_TEST_BIT_SELECT_TYPE:
        {
            LOG_INFO_EX(BSL_LOG_MODULE,
                        "%s(), line %d, Tesing LS/MS selection of crc per layer per client. %s%s\r\n",
                        __FUNCTION__, __LINE__, EMPTY, EMPTY);
            SHR_IF_ERR_EXIT(appl_dnx_lb_test_bit_select(unit, args, sand_control));
            LOG_INFO_EX(BSL_LOG_MODULE,
                        "%s(), line %d, DNX option %s. Tesing LS/MS selection of crc per layer per client returned 'success' flag. %s\r\n",
                        __FUNCTION__, __LINE__,
                        SH_SAND_GET_ENUM_STR(DNX_CTEST_LOAD_BALANCING_OPTION_ALL_TEST_TYPE, lb_test_type), EMPTY);
            break;
        }
        case CTEST_DNX_SWITCH_LOAD_BALANCING_TEST_GENERAL_SEED_TYPE:
        {
            LOG_INFO_EX(BSL_LOG_MODULE,
                        "%s(), line %d, Tesing general seed of crc per MPLS stack and parser. %s%s\r\n",
                        __FUNCTION__, __LINE__, EMPTY, EMPTY);
            SHR_IF_ERR_EXIT(appl_dnx_lb_test_general_seed(unit, args, sand_control));
            LOG_INFO_EX(BSL_LOG_MODULE,
                        "%s(), line %d, DNX option %s. Tesing general seed of crc per MPLS stack and parser returned 'success' flag. %s\r\n",
                        __FUNCTION__, __LINE__,
                        SH_SAND_GET_ENUM_STR(DNX_CTEST_LOAD_BALANCING_OPTION_ALL_TEST_TYPE, lb_test_type), EMPTY);
            break;
        }
        default:
        {
            LOG_INFO_EX(BSL_LOG_MODULE,
                        "%s(), line %d, Option %d 'test type' on 'DNX' is not implemented. Illegal parameter. %s\r\n",
                        __FUNCTION__, __LINE__, lb_test_type, EMPTY);
            SHR_IF_ERR_EXIT(_SHR_E_PARAM);
            break;
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - run 'load_balancing/bcm' sequence in diag shell
 */
shr_error_e
sh_dnx_switch_load_balancing_bcm_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(args, _SHR_E_PARAM, "args");
    SHR_NULL_CHECK(sand_control, _SHR_E_PARAM, "sand_control");

    SHR_IF_ERR_EXIT(appl_dnx_load_balancing_bcm_starter(unit, args, sand_control));
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - run 'load_balancing/dnx' sequence in diag shell
 */
shr_error_e
sh_dnx_switch_load_balancing_dnx_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(args, _SHR_E_PARAM, "args");
    SHR_NULL_CHECK(sand_control, _SHR_E_PARAM, "sand_control");

    SHR_IF_ERR_EXIT(appl_dnx_load_balancing_dnx_starter(unit, args, sand_control));
exit:
    SHR_FUNC_EXIT;
}
