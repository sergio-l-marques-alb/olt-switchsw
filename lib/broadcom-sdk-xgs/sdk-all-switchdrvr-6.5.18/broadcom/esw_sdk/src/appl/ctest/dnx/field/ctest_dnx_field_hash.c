/** \file ctest_dnx_field_hash.c
 * $Id$
 *
 * Hash iPMF use-case for DNX.
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
#include <soc/dnxc/swstate/dnxc_sw_state_utils.h>
/** shared */
#include <shared/shrextend/shrextend_debug.h>
/** appl */
#include <appl/diag/sand/diag_sand_utils.h>
#include <appl/diag/sand/diag_sand_framework.h>

#include <bcm_int/dnx/field/field.h>
#include <bcm_int/dnx/field/field_entry.h>
#include <bcm_int/dnx/field/field_group.h>
#include <bcm_int/dnx/field/field_presel.h>
#include <bcm_int/dnx/field/field_context.h>
#include <bcm_int/dnx/field/field_map.h>
#include <bcm_int/dnx/instru/instru.h>
#include "ctest_dnx_field_utils.h"
#include <bcm_int/dnx/field/field_init.h>
#include <appl/diag/dnx/diag_dnx_field.h>
#include <include/soc/dnx/swstate/auto_generated/types/dnx_field_group_types.h>

/*
 * }
 */
/*
 * DEFINEs
 * {
 */
/**
 * \brief
 *   Keyword for test type:
 *   Type can be either BCM or DNX
 */
#define CTEST_DNX_FIELD_HASH_OPTION_TYPE        "type"

/**
 * \brief
 *   Keyword for the number of times the test will run
 */
#define CTEST_DNX_FIELD_HASH_OPTION_COUNT              "count"

/**
 * \brief
 *   Keyword for controlling whether to release resources at end of test.
 *   clean can be either 0 or 1, if the ctest will clean-up after its finished setting-up.
 */
#define CTEST_DNX_FIELD_HASH_OPTION_CLEAN              "clean"
/*
 * }
 */

/* *INDENT-OFF* */

/**
 * \brief
 *   Options list for 'hash' shell command
 * \remark
 */
sh_sand_option_t dnx_field_hash_options[] = {
     /* Name */                                 /* Type */              /* Description */                                     /* Default */
    {CTEST_DNX_FIELD_HASH_OPTION_TYPE,       SAL_FIELD_TYPE_ENUM,    "Type (level) of test (dnx or bcm)",        "DNX",     (void *)Field_level_enum_table},
    {CTEST_DNX_FIELD_HASH_OPTION_CLEAN,      SAL_FIELD_TYPE_BOOL,    "Will test perform HW and SW-state clean-up or not",  "Yes"},
    {CTEST_DNX_FIELD_HASH_OPTION_COUNT,      SAL_FIELD_TYPE_UINT32,  "Number of times test will run",            "1"},
    {NULL}      /* End of options list - must be last. */
};
/**
 * \brief
 *   List of tests for 'hash' shell command (to be run on regression, precommit, etc.)
 * \remark
 *   Currently, it will be executed only for regression and precommit.
 */
sh_sand_invoke_t sh_dnx_field_hash_tests[] = {
    {"DNX_hash_setup", "type=DNX clean=yes count=6", CTEST_POSTCOMMIT},
    {"BCM_hash_setup", "type=BCM clean=yes count=6", CTEST_POSTCOMMIT},
    {NULL}
};
/* *INDENT-ON* */
/**
 *  hash shell command leaf details
 */
sh_sand_man_t sh_dnx_field_hash_man = {
    "Field Hash feature tests",
    "Creating a new context with hashing enable\n"
        "Create the hash configuration and then perform hash_get to compare both results.\n"
        "Cleaning up all resources used after the test finishes\n"
        "The 'count' variable defines how many times the test will run",
    "ctest field hash type=<DNX | BCM> count=1"
};

/**
 * \brief
 *  Function to  call 'dnx_field_context_hash_info_get' using the supplied and 'context_id'
 *  Which is used in the corresponding 'set' operation (dnx_field_context_hash_create())
 *  We compare it to the 'set_hash_info_p' supplied to the function.
 *  We have a single qualifier in the attach info, hence why
 *  we only compare the first element of the corresponding arrays.
 *
 * \param [in] unit             - Device ID
 * \param [in] context_id       - Context ID
 * \param [in] set_hash_info_p   - Context hash info to be compared against.
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
ctest_dnx_field_context_hash_compare_dnx(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_context_hash_info_t * set_hash_info_p)
{
    dnx_field_context_hash_info_t hash_get_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_context_hash_info_t_init(unit, &hash_get_info));

    hash_get_info.hash_config.action_key = DBAL_ENUM_FVAL_FIELD_HASH_LB_KEY_ADDITIONAL_LB;

    SHR_IF_ERR_EXIT(dnx_field_context_hash_info_get(unit, DNX_FIELD_STAGE_IPMF1, context_id, &hash_get_info));

    /** Comparing the hash_function */
    if (set_hash_info_p->hash_function != hash_get_info.hash_function)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "set_hash_info_p hash_function (0x%08X) is not the same as hash_get_info hash_function (0x%08X)\r\n",
                     set_hash_info_p->hash_function, hash_get_info.hash_function);
    }

    /** Comparing the order */
    if (set_hash_info_p->order != hash_get_info.order)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "set_hash_info_p order (0x%08X) is not the same as hash_get_info order (0x%08X)\r\n",
                     set_hash_info_p->order, hash_get_info.order);
    }

    /** Comparing the hash_action */
    if (set_hash_info_p->hash_config.hash_action != hash_get_info.hash_config.hash_action)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "set_hash_info_p hash_action (0x%08X) is not the same as hash_get_info hash_action (0x%08X)\r\n",
                     set_hash_info_p->hash_config.hash_action, hash_get_info.hash_config.hash_action);
    }

    /** Comparing the single DNX Qualifier */
    if (set_hash_info_p->key_info.dnx_quals[0] != hash_get_info.key_info.dnx_quals[0])
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "set_hash_info_p dnx_qual (%s) is not the same as hash_get_info dnx_qual (%s)\r\n",
                     dnx_field_dnx_qual_text(unit, set_hash_info_p->key_info.dnx_quals[0]),
                     dnx_field_dnx_qual_text(unit, hash_get_info.key_info.dnx_quals[0]));
    }

    /** Comparing the single DNX Qualifier Info - input_type */
    if (set_hash_info_p->key_info.qual_info[0].input_type != hash_get_info.key_info.qual_info[0].input_type)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "set_hash_info_p qual input_type (0x%08X) is not the same as hash_get_info qual input_type (0x%08X)\r\n",
                     set_hash_info_p->key_info.qual_info[0].input_type, hash_get_info.key_info.qual_info[0].input_type);
    }

    /** Comparing the single DNX Qualifier Info - input_arg */
    if (set_hash_info_p->key_info.qual_info[0].input_arg != hash_get_info.key_info.qual_info[0].input_arg)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "set_hash_info_p qual input_arg (0x%08X) is not the same as hash_get_info qual input_arg (0x%08X)\r\n",
                     set_hash_info_p->key_info.qual_info[0].input_arg, hash_get_info.key_info.qual_info[0].input_arg);
    }

    /** Comparing the single DNX Qualifier Info - offset */
    if (set_hash_info_p->key_info.qual_info[0].offset != hash_get_info.key_info.qual_info[0].offset)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "set_hash_info_p qual offset (0x%08X) is not the same as hash_get_info qual offset (0x%08X)\r\n",
                     set_hash_info_p->key_info.qual_info[0].offset, hash_get_info.key_info.qual_info[0].offset);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Function to  call 'bcm_field_context_hash_info_get' using the supplied and 'context_id'
 *  Which is used in the corresponding 'set' operation (bcm_field_context_hash_create())
 *  We compare it to the 'set_hash_info_p' supplied to the function.
 *  We have a single qualifier in the attach info, hence why
 *  we only compare the first element of the corresponding arrays.
 *
 * \param [in] unit             - Device ID
 * \param [in] context_id       - Context ID
 * \param [in] set_hash_info_p   - Context hash info to be compared against.
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
ctest_dnx_field_context_hash_compare_bcm(
    int unit,
    bcm_field_context_t context_id,
    bcm_field_context_hash_info_t * set_hash_info_p)
{
    bcm_field_context_hash_info_t hash_get_info;

    SHR_FUNC_INIT_VARS(unit);

    bcm_field_context_hash_info_t_init(&hash_get_info);

    hash_get_info.hash_config.action_key = bcmFieldContextHashActionKeyAdditionalLbKey;

    SHR_IF_ERR_EXIT(bcm_field_context_hash_info_get(unit, bcmFieldStageIngressPMF1, context_id, &hash_get_info));

    /** Comparing the hash_function */
    if (set_hash_info_p->hash_function != hash_get_info.hash_function)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "set_hash_info_p hash_function (0x%08X) is not the same as hash_get_info hash_function (0x%08X)\r\n",
                     set_hash_info_p->hash_function, hash_get_info.hash_function);
    }

    /** Comparing the order */
    if (set_hash_info_p->order != hash_get_info.order)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "set_hash_info_p order (0x%08X) is not the same as hash_get_info order (0x%08X)\r\n",
                     set_hash_info_p->order, hash_get_info.order);
    }

    /** Comparing the hash_action */
    if (set_hash_info_p->hash_config.function_select != hash_get_info.hash_config.function_select)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "set_hash_info_p hash_action (0x%08X) is not the same as hash_get_info hash_action (0x%08X)\r\n",
                     set_hash_info_p->hash_config.function_select, hash_get_info.hash_config.function_select);
    }

    /** Comparing the single DNX Qualifier */
    if (set_hash_info_p->key_info.qual_types[0] != hash_get_info.key_info.qual_types[0])
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "set_hash_info_p dnx_qual (%s) is not the same as hash_get_info dnx_qual (%s)\r\n",
                     dnx_field_bcm_qual_text(unit, set_hash_info_p->key_info.qual_types[0]),
                     dnx_field_bcm_qual_text(unit, hash_get_info.key_info.qual_types[0]));
    }

    /** Comparing the single DNX Qualifier Info - input_type */
    if (set_hash_info_p->key_info.qual_info[0].input_type != hash_get_info.key_info.qual_info[0].input_type)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "set_hash_info_p qual input_type (0x%08X) is not the same as hash_get_info qual input_type (0x%08X)\r\n",
                     set_hash_info_p->key_info.qual_info[0].input_type, hash_get_info.key_info.qual_info[0].input_type);
    }

    /** Comparing the single DNX Qualifier Info - input_arg */
    if (set_hash_info_p->key_info.qual_info[0].input_arg != hash_get_info.key_info.qual_info[0].input_arg)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "set_hash_info_p qual input_arg (0x%08X) is not the same as hash_get_info qual input_arg (0x%08X)\r\n",
                     set_hash_info_p->key_info.qual_info[0].input_arg, hash_get_info.key_info.qual_info[0].input_arg);
    }

    /** Comparing the single DNX Qualifier Info - offset */
    if (set_hash_info_p->key_info.qual_info[0].offset != hash_get_info.key_info.qual_info[0].offset)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "set_hash_info_p qual offset (0x%08X) is not the same as hash_get_info qual offset (0x%08X)\r\n",
                     set_hash_info_p->key_info.qual_info[0].offset, hash_get_info.key_info.qual_info[0].offset);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Runs the hash tests according to user input.
 * \param [in] unit - Device ID
 * \param [in] args - Extra arguments
 * \param [in] sand_control -
 *   Control information related to each of the 'options' entered by the caller (and contained in'args')
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
appl_dnx_field_hash_run(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    dnx_field_context_mode_t context_mode;
    dnx_field_context_t context_id;
    dnx_field_context_hash_info_t hash_info;

    bcm_field_context_info_t bcm_context_mode;
    bcm_field_context_t bcm_context_id;
    bcm_field_context_hash_info_t bcm_hash_info;

    unsigned int hash_test_clean;
    unsigned int test_type;

    SHR_FUNC_INIT_VARS(unit);

    hash_test_clean = 0;
    test_type = 0;
    SH_SAND_GET_BOOL(CTEST_DNX_FIELD_HASH_OPTION_CLEAN, hash_test_clean);
    SH_SAND_GET_ENUM(CTEST_DNX_FIELD_HASH_OPTION_TYPE, test_type);

    if (test_type == 0)
    {
        bcm_field_context_info_t_init(&bcm_context_mode);
        bcm_context_mode.hashing_enabled = TRUE;
        SHR_IF_ERR_EXIT(bcm_field_context_create
                        (unit, 0, bcmFieldStageIngressPMF1, &bcm_context_mode, &bcm_context_id));

        bcm_field_context_hash_info_t_init(&bcm_hash_info);
        bcm_hash_info.hash_function = bcmFieldContextHashFunctionCrc16Bisync;
        bcm_hash_info.order = TRUE;
        bcm_hash_info.hash_config.function_select = bcmFieldContextHashActionValueReplaceCrc;
        bcm_hash_info.hash_config.action_key = bcmFieldContextHashActionKeyAdditionalLbKey;
        bcm_hash_info.key_info.nof_quals = 1;
        bcm_hash_info.key_info.qual_types[0] = bcmFieldQualifySrcIp;
        bcm_hash_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
        bcm_hash_info.key_info.qual_info[0].input_arg = 1;
        bcm_hash_info.key_info.qual_info[0].offset = 0;

        SHR_IF_ERR_EXIT(bcm_field_context_hash_create
                        (unit, 0, bcmFieldStageIngressPMF1, bcm_context_id, &bcm_hash_info));

        /** Compare results of the set_hash_info_p with the retrieved from the get API. */
        SHR_IF_ERR_EXIT(ctest_dnx_field_context_hash_compare_bcm(unit, bcm_context_id, &bcm_hash_info));
    }
    else if (test_type == 1)
    {
        SHR_IF_ERR_EXIT(dnx_field_context_mode_t_init(unit, &context_mode));

        /**
        * Create and configure the context
        */
        context_mode.context_ipmf1_mode.hash_mode = DNX_FIELD_CONTEXT_HASH_MODE_ENABLED;
        SHR_IF_ERR_EXIT(dnx_field_context_create(unit, 0, DNX_FIELD_STAGE_IPMF1, &context_mode, &context_id));

        /**
        * Build the hash key attach to iPMF-1 context
        */
        SHR_IF_ERR_EXIT(dnx_field_context_hash_info_t_init(unit, &hash_info));

        hash_info.hash_function = DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC16_BISYNC;
        hash_info.order = TRUE;
        hash_info.hash_config.action_key = DBAL_ENUM_FVAL_FIELD_HASH_LB_KEY_ADDITIONAL_LB;
        hash_info.hash_config.hash_action = DNX_FIELD_CONTEXT_HASH_ACTION_REPLACE_CRC16;

        hash_info.key_info.dnx_quals[0] =
            DNX_QUAL(DNX_FIELD_QUAL_CLASS_HEADER, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_HEADER_QUAL_IPV4_SRC);

        hash_info.key_info.qual_info[0].input_type = DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE;
        hash_info.key_info.qual_info[0].input_arg = 1;
        hash_info.key_info.qual_info[0].offset = 0;

        SHR_IF_ERR_EXIT(dnx_field_context_hash_create
                        (unit, DNX_FIELD_CONTEXT_HASH_FLAGS_NONE, DNX_FIELD_STAGE_IPMF1, context_id, &hash_info));

        /** Compare results of the set_hash_info_p with the retrieved from the get API. */
        SHR_IF_ERR_EXIT(ctest_dnx_field_context_hash_compare_dnx(unit, context_id, &hash_info));
    }
    else
    {
        LOG_INFO_EX(BSL_LOG_MODULE,
                    "%s(), line %d, Option %d (for 'test_type') is not implemented. Illegal parameter. %s\r\n",
                    __FUNCTION__, __LINE__, test_type, EMPTY);
        SHR_IF_ERR_EXIT(_SHR_E_PARAM);
    }
exit:
    if (hash_test_clean)
    {
        if (test_type == 1)
        {
            CTEST_DNX_FIELD_UTIL_ERR(dnx_field_context_hash_destroy
                                     (unit, DNX_FIELD_CONTEXT_HASH_FLAGS_NONE, DNX_FIELD_STAGE_IPMF1, context_id));
            CTEST_DNX_FIELD_UTIL_ERR(dnx_field_context_destroy(unit, DNX_FIELD_STAGE_IPMF1, context_id));
        }
        else if (test_type == 0)
        {
            bcm_field_context_hash_destroy(unit, bcmFieldStageIngressPMF1, bcm_context_id);
            bcm_field_context_destroy(unit, bcmFieldStageIngressPMF1, bcm_context_id);
        }
    }
    SHR_FUNC_EXIT;
}

/**
 * \brief - run hash init sequence in shell
 *
 * \param [in] unit - the unit number in system
 * \param [in,out] args - pointer to args_t, not passed down
 * \param [in,out] sand_control - pointer to sh_sand_control_t, not
 *  passed down
 *
 */
shr_error_e
sh_dnx_field_hash_cmd(
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
        SHR_IF_ERR_EXIT(appl_dnx_field_hash_run(unit, args, sand_control));
    }
exit:
    SHR_FUNC_EXIT;
}
