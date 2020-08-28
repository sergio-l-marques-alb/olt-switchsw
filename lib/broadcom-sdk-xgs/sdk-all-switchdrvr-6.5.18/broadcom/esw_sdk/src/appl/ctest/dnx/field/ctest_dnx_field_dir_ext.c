/** \file diag_dnx_field_dir_ext.c
 *
 * Direct Extraction application procedures for DNX.
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
#include <bcm_int/dnx/field/field_group.h>
#include <bcm_int/dnx/field/field_entry.h>
#include <bcm_int/dnx/field/field_presel.h>
#include <bcm_int/dnx/field/field_map.h>
#include <bcm_int/dnx/field/field_context.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/dnx/diag_dnx_field.h>
#include "ctest_dnx_field_utils.h"
#include "ctest_dnx_field_group.h"
#include "ctest_dnx_field_dir_ext.h"
#include <soc/sand/sand_signals.h>
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
 *   Options list for 'database' shell command
 * \remark
 *   Set the default value such that no test is carried out but procedure does return with 'success'
 *   This ensures success on regression and pre-commit:
 */
sh_sand_option_t Sh_dnx_field_dir_ext_options[] = {
     /* Name */                                 /* Type */              /* Description */                                /* Default */
    {CTEST_DNX_DIR_EXT_OPTION_TYPE,       SAL_FIELD_TYPE_ENUM,    "Type (level) of test (dnx or bcm)",                     "DNX",     (void *)Field_level_enum_table},
    {CTEST_DNX_DIR_EXT_OPTION_DE_TYPE,    SAL_FIELD_TYPE_ENUM,    "Direct extraction style of test (single, multiple, for_cint)",  "SINGLE",  (void *)Field_de_type_enum_table},
    {CTEST_DNX_DIR_EXT_OPTION_COUNT,      SAL_FIELD_TYPE_UINT32,  "Number of times test will run",                         "1"},
    {CTEST_DNX_DIR_EXT_OPTION_CLEAN,      SAL_FIELD_TYPE_BOOL,    "Will test perform HW and SW-state clean-up or not",     "Yes"},
    {CTEST_DNX_DIR_EXT_OPTION_TRAFFIC,    SAL_FIELD_TYPE_BOOL,    "Will the test indlude traffic test or not",             "No"},
     /* End of options list - must be last. */
    {NULL}
};

/**
 * \brief
 *   List of tests for 'dir_ext' ctest command (to be run on regression, precommit, etc.)
 * \remark
 *   Currently, it will be executed only for regression and precommit.
 */
sh_sand_invoke_t Sh_dnx_field_dir_ext_tests[] = {
    {"DNX_dir_ext_multi", "type=DNX de_type=MULTIPLE count=3", CTEST_POSTCOMMIT}
    ,
    {"DNX_dir_ext_traffic", "type=DNX de_type=SINGLE count=1 traffic=YES", CTEST_POSTCOMMIT}
    ,
#if(0)
    /** We are taking it out of ctest fld run' list for now */
    {"BCM_dir_ext", "type=BCM count=1", CTEST_POSTCOMMIT}
    ,
#endif
    {NULL}
};

/**
 *  Direct Extraction leaf details
 */
sh_sand_man_t Sh_dnx_dir_ext_man = {
    "Direct Extraction CTEST",
    "Start the CTEST for Direct Extraction via BCM or DNX APIs.\r\n"
    "Setting 'de_type' to 'MULTIPLE' generates a few field groups which use\r\n"
    " the same key on the same context.\r\n"
    "Setting 'de_type' to 'FOR_CINT' generates a setup which is suitable for DVAPI:\r\n"
    " AT_Dnx_Cint_field_dir_ext().\r\n"
    "Setting 'clean' to 'yes' will release all resources after test is done.\r\n"
    "Setting 'count' to, say, '2' will make the test run twice.\r\n"
    "Setting 'traffic' to 'yes' will run the test with traffic.\r\n",
    "ctest field dir_ext type=<BCM | DNX> de_type=<SINGLE | MULTIPLE | FOR_CINT> count=<integer> clean=<yes | no> traffic=<yes | no>",
    "type=DNX de_type=MULTIPLE"
};
/*
 * Input parameters for the 'multiple' direct extraction style.
 * First table is qualifiers' parameters and second table contains the matching actions.
 * Note that the total size for both needs to be the same!
 * They are repeated for each field group.
 */
static dnx_ctest_dir_ext_quals_for_n_de_t Dnx_ctest_dir_ext_quals_for_multiple_de[CTEST_DNX_DIR_EXT_NUM_FGS_FOR_MULTIPLE][CTEST_DNX_DIR_EXT_MAX_NUM_ACTIONS_PER_FG] =
{
         /* name */    /* size */  /* input type */                     /* input arg */    /* offset */
    {
        {"user qual  7",   32,    {DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE,      0,                 35       }},
        {NULL,              0,    {0,                                        0,                  0       }},
    },
    {
        {"user qual  0",    5,    {DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE,      0,                 5        }},

        {"user qual  1",    8,    {DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE,      0,                 10       }},
        {"user qual  2",    7,    {DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE,      0,                 20       }},

        {"user qual  3",    3,    {DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE,      0,                 27       }},
        {"user qual  4",    5,    {DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE,      0,                 30       }},
        {"user qual  5",    4,    {DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE,      0,                 35       }},

        {"user qual  6",   32,    {DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE,      0,                 35       }},
        {NULL,              0,    {0,                                        0,                  0       }},
    },
    {
        {"user qual  8",   32,    {DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE,      0,                 35       }},
        {"user qual  9",   32,    {DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE,      0,                 35       }},
        {NULL,              0,    {0,                                        0,                  0       }},
    },
};

static dnx_ctest_dir_ext_actions_for_n_de_t Dnx_ctest_dir_ext_actions_for_multiple_de[CTEST_DNX_DIR_EXT_NUM_FGS_FOR_MULTIPLE][CTEST_DNX_DIR_EXT_MAX_NUM_QUALS_PER_FG] =
{
         /* name */      /* size */  /*prefix_size*/     /* DBAL action id */                             /* Priority */
    {
        {"user action 6",   20,           12,          DBAL_ENUM_FVAL_IPMF1_ACTION_DST_DATA,      {  BCM_FIELD_ACTION_POSITION(0, 9) }},
        {"user action 7",   10,           22,          DBAL_ENUM_FVAL_IPMF1_ACTION_DST_DATA,      {  BCM_FIELD_ACTION_POSITION(0, 8) }},
        {"user action 8",    2,           30,          DBAL_ENUM_FVAL_IPMF1_ACTION_DST_DATA,      {  BCM_FIELD_ACTION_POSITION(0, 7) }},
        {NULL,               0,            0,                                             0,      {                                                                       0  }},
    },
    {
        {"user action 0",    5,           27,          DBAL_ENUM_FVAL_IPMF1_ACTION_USER_HEADER_1, {  BCM_FIELD_ACTION_POSITION(0,15) }},
        {"user action 1",   15,           17,          DBAL_ENUM_FVAL_IPMF1_ACTION_DST_DATA,      {  BCM_FIELD_ACTION_POSITION(0,14) }},
        {"user action 2",   12,           20,          DBAL_ENUM_FVAL_IPMF1_ACTION_DST_DATA,      {  BCM_FIELD_ACTION_POSITION(0,13) }},

        {"user action 3",   12,           20,          DBAL_ENUM_FVAL_IPMF1_ACTION_DST_DATA,      {  BCM_FIELD_ACTION_POSITION(0,12) }},
        {"user action 4",    5,           27,          DBAL_ENUM_FVAL_IPMF1_ACTION_DST_DATA,      {  BCM_FIELD_ACTION_POSITION(0,11) }},
        {"user action 5",   15,           17,          DBAL_ENUM_FVAL_IPMF1_ACTION_DST_DATA,      {  BCM_FIELD_ACTION_POSITION(0,10) }},
        {NULL,               0,            0,                                             0,      {                                                                       0  }},
    },
    {
        {"user action 9",   32,            0,          DBAL_ENUM_FVAL_IPMF1_ACTION_DST_DATA,      {  BCM_FIELD_ACTION_POSITION(0, 6) }},
        {"user action 10",  32,            0,          DBAL_ENUM_FVAL_IPMF1_ACTION_DST_DATA,      {  BCM_FIELD_ACTION_POSITION(0, 5) }},
        {NULL,               0,            0,                                             0,      {                                                                       0  }},
    },
};
/*
 * Input parameters for the 'new' direct extraction style. To be used on the 'positive' test.
 * First table is qualifiers' parameters and second table contains the matching actions.
 * Note that the total size for both needs to be the same!
 * Note that for the traffic test we are using only the first CTEST_DNX_DIR_EXT_NUM_QUALS_FOR_TRAFFIC qualifiers 
 * and the first CTEST_DNX_DIR_EXT_NUM_ACTIONS_FOR_TRAFFIC actions.
 */
static dnx_ctest_dir_ext_quals_for_n_de_t Dnx_ctest_dir_ext_quals_for_n_de[] =
{
     /* name */    /* size */  /* input type */                     /* input arg */    /* offset */
    {"user qual  0",    5,    {DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE,      0,                 5       }},

    {"user qual  1",    8,    {DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE,      0,                 10       }},
    {"user qual  2",    7,    {DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE,      0,                 20       }},

    {"user qual  3",    3,    {DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE,      0,                 27       }},
    {"user qual  4",    5,    {DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE,      0,                 30       }},
    {"user qual  5",    4,    {DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE,      0,                 35       }},

    {"user qual  6",   32,    {DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE,      0,                 35       }},

    {"user qual  7",   32,    {DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE,      0,                 35       }},

    {"user qual  8",   32,    {DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE,      0,                 35       }},
    {"user qual  9",   32,    {DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE,      0,                 35       }}
};
static dnx_ctest_dir_ext_actions_for_n_de_t Dnx_ctest_dir_ext_actions_for_n_de[] =
{
     /* name */      /* size */  /*prefix_size*/     /* DBAL action id */                             /* Priority */
    {"user action 0",    5,           27,          DBAL_ENUM_FVAL_IPMF1_ACTION_USER_HEADER_1, {  BCM_FIELD_ACTION_POSITION(0,15)  }},
    {"user action 1",   15,           17,          DBAL_ENUM_FVAL_IPMF1_ACTION_DST_DATA,      {  BCM_FIELD_ACTION_POSITION(0,14)  }},
    {"user action 2",   12,           20,          DBAL_ENUM_FVAL_IPMF1_ACTION_DST_DATA,      {  BCM_FIELD_ACTION_POSITION(0,13)  }},

    {"user action 3",   12,           20,          DBAL_ENUM_FVAL_IPMF1_ACTION_DST_DATA,      {  BCM_FIELD_ACTION_POSITION(0,12)  }},
    {"user action 4",    5,           27,          DBAL_ENUM_FVAL_IPMF1_ACTION_DST_DATA,      {  BCM_FIELD_ACTION_POSITION(0,11)  }},
    {"user action 5",   15,           17,          DBAL_ENUM_FVAL_IPMF1_ACTION_DST_DATA,      {  BCM_FIELD_ACTION_POSITION(0,10)  }},

    {"user action 6",   20,           12,          DBAL_ENUM_FVAL_IPMF1_ACTION_DST_DATA,      {  BCM_FIELD_ACTION_POSITION(0,9)  }},
    {"user action 7",   10,           22,          DBAL_ENUM_FVAL_IPMF1_ACTION_DST_DATA,      {  BCM_FIELD_ACTION_POSITION(0,8)  }},

    {"user action 8",   32,            0,          DBAL_ENUM_FVAL_IPMF1_ACTION_DST_DATA,      {  BCM_FIELD_ACTION_POSITION(0,7)  }},
    {"user action 9",   32,            0,          DBAL_ENUM_FVAL_IPMF1_ACTION_DST_DATA,      {  BCM_FIELD_ACTION_POSITION(0,6)  }},

    {"user action 10",   2,           30,          DBAL_ENUM_FVAL_IPMF1_ACTION_DST_DATA,      {  BCM_FIELD_ACTION_POSITION(0,5)  }},
};
/*
 * Input parameters for the 'new' direct extraction style. To be used on the 'positive' test.
 * First table is qualifiers' parameters and second table contains the matching actions.
 * Note that the total size for both needs to be the same!
 * Note that this is for setup only. User of this setup is AT_Dnx_Cint_field_dir_ext() on
 * regress/bcm/tests/dnx/cints_tests/cintsFieldDvapis.tcl.
 */
/*
 * Set qualifier on offset 13 from the beginning of ETH header (I.e., offset 13 from DA which
 * points to the last 3 bits of the second byte.
 */
static dnx_ctest_dir_ext_quals_for_n_de_t Dnx_ctest_dir_ext_quals_for_c_de[] =
{
     /* name */                   /* size */                           /* input type */                 /* input arg */    /* offset */
    {"QUAL_EXTRACTED_PMF2", CTEST_DNX_DIR_EXT_NOF_EXTRACTED_BITS, {DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE,      0,                 13       }},
    /*
     * DO NOT REMOVE: End of array.
     */
    {NULL,0,{0,0,0}}
};
/*
 * Set action as 'TC'. This means that the signal 'TC' should carry the LS 3 bits of the second byts
 * of the DA MAC address.
 */
static dnx_ctest_dir_ext_actions_for_n_de_t Dnx_ctest_dir_ext_actions_for_c_de[] =
{
     /* name */          /* size */                  /*prefix_size*/     /* DBAL action id */                             /* Priority */
    {"ACTION_TC", CTEST_DNX_DIR_EXT_NOF_EXTRACTED_BITS, 0, DBAL_ENUM_FVAL_IPMF1_ACTION_TC, {  BCM_FIELD_ACTION_POSITION(0,15)  }},
    /*
     * DO NOT REMOVE: End of array.
     */
    {NULL,0,0, 0,{0}}
};
/**
 * An array, which contains all needed signal information for the traffic testing:
 * core, block, from, to, signal name, size of the buffer and
 * to match on the qualifier.
 * Expected value isn't taken from here, and is merely initialized.
 * We use only the first CTEST_DNX_DIR_EXT_NUM_ACTIONS_FOR_TRAFFIC from the array,
 * that's why the size of the array is equal to CTEST_DNX_DIR_EXT_NUM_ACTIONS_FOR_TRAFFIC.
 */
static dnx_field_utils_signal_info_t Packet_sig_info[CTEST_DNX_DIR_EXT_NUM_ACTIONS_FOR_TRAFFIC] = {
    
    { 0, "IRPP", "IPMF1", "", "User_Header_1", 1, CTEST_DNX_DIR_EXT_USER_HEADER_EXPECTED_VALUE },
    { 0, "IRPP", "IPMF1", "", "Fwd_Action_Dst", 1, CTEST_DNX_DIR_EXT_FWD_ACTION_DST_EXPECTED_VALUE }
};
/**
 *  The packet to be sent.
 *  The packet is constructed with 3 VLAN tags to match the configured PMF program.
 */
static dnx_field_utils_packet_info_t Packet_info = {
    {"PTCH_2", "PTCH_2.PP_SSP", "27", "ETH3", "ETH3.DA", "11:96:c6:96:00:01", "ETH3.SA", "00:00:00:00:00:01",
            "ETH3.Outer_VLAN.TPID", "0x9100", "ETH3.Middle_VLAN.TPID", "0x8100", "ETH3.Inner_VLAN.TPID", "0x9200", ""}
};
/* *INDENT-ON* */
/*
 * }
 */
 /**
 * \brief
 *   This function is using the ctest_dnx_field_utils_packet_tx
 * utility for traffic sending and sand_signal_verify to verify that the signals
 * are as expected
 *
 * \param [in] unit - The unit number.
 * \param [in] packet_info_p - Pointer to the packet,
 *                      which will be transmitted. For more info, please search for
 *                      ctest_dnx_field_utils_packet_tx() in ctest_dnx_field_utils.h
 * \param [in] sig_info_p - Pointer to an array, which contains all needed information
 *                      about signals verification: core, block, from, to,
 *                      size of the buffer and expected value.
 * \return
 *   Error code (as per 'bcm_error_e').
 * \see
 *   bcm_error_e
 */
static shr_error_e
ctest_dnx_dir_ext_traffic_test(
    int unit,
    dnx_field_utils_packet_info_t * packet_info_p,
    dnx_field_utils_signal_info_t * sig_info_p)
{
    int rv, sig_iterator;
    char return_value[DSIG_MAX_SIZE_STR];
    bcm_port_mapping_info_t mapping_info;
    bcm_port_interface_info_t interface_info;
    uint32 flags = 0;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Send traffic.
     */
    LOG_INFO_EX(BSL_LOG_MODULE,
                "%s(), line %d, Send packet and verify signals. Packet header: %s, %s, ...  \r\n"
                "Long operation.  Wait...\r\n",
                __FUNCTION__, __LINE__, packet_info_p->header_info[0], packet_info_p->header_info[1]);

    SHR_IF_ERR_EXIT(bcm_port_get(unit, 201, &flags, &interface_info, &mapping_info));
    sal_snprintf(packet_info_p->header_info[2], DNX_FIELD_UTILS_STR_SIZE, "%d", mapping_info.pp_port);
    SHR_IF_ERR_EXIT(ctest_dnx_field_utils_packet_tx(unit, 201, packet_info_p));
    /**
     * Iterate between all of the signals and check them.
     */
    for (sig_iterator = 0; sig_iterator < CTEST_DNX_DIR_EXT_NUM_ACTIONS_FOR_TRAFFIC; sig_iterator++)
    {
        LOG_INFO_EX(BSL_LOG_MODULE,
                    "Verify signal: block %s, from %s, to %s, name %s\r\n",
                    sig_info_p[sig_iterator].block, sig_info_p[sig_iterator].from, sig_info_p[sig_iterator].to,
                    sig_info_p[sig_iterator].sig_name);

        LOG_INFO_EX(BSL_LOG_MODULE,
                    "Expected value 0x%08X, size (in longs) %d %s %s\r\n",
                    sig_info_p[sig_iterator].expected_value, sig_info_p[sig_iterator].size, EMPTY, EMPTY);
        rv = sand_signal_verify(unit, sig_info_p[sig_iterator].core,
                                sig_info_p[sig_iterator].block,
                                sig_info_p[sig_iterator].from,
                                sig_info_p[sig_iterator].to,
                                sig_info_p[sig_iterator].sig_name,
                                &sig_info_p[sig_iterator].expected_value,
                                sig_info_p[sig_iterator].size, NULL, return_value, 0);
        SHR_IF_ERR_EXIT_WITH_LOG(rv, "DE traffic test failed. Result of signal verify is %s. Expected 0x%08X %s",
                                 return_value, sig_info_p[sig_iterator].expected_value, EMPTY);
        LOG_INFO_EX(BSL_LOG_MODULE, "%s(), line %d, Value of signal \"%s\" is as expected! %s \r\n",
                    __FUNCTION__, __LINE__, sig_info_p[sig_iterator].sig_name, EMPTY);

    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   This function contains the Direct Extraction
 *   application.This function sets all required HW
 *   configuration for Direct Extraction to be performed.
 *
 * \param [in] unit - The unit number.
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
shr_error_e
appl_dnx_dir_ext_init(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{

    unsigned int dir_ext_test_type, dir_ext_de_type, dir_ext_test_clean;
    unsigned int dir_ext_test_traffic;
    bcm_field_group_config_t grp;
    bcm_field_aset_t aset;
    bcm_field_entry_t ent;
    bcm_field_extraction_action_t extract;
    bcm_field_extraction_field_t ext_inlif[2];

    dnx_field_context_mode_t context_mode;
    dnx_field_context_t context_id_array;
    dnx_field_presel_entry_id_t presel_entry_id;
    dnx_field_presel_qual_data_t presel_qual[1];
    dnx_field_presel_entry_data_t presel_data;
    dnx_field_group_info_t fg_info;
    dnx_field_group_attach_info_t attach_info;
    dnx_field_group_t fg_id;

    SHR_FUNC_INIT_VARS(unit);
    fg_id = 20;
    dir_ext_test_clean = 0;
    /*
     * Get the inputs from Shell
     */
    SH_SAND_GET_BOOL(CTEST_DNX_DIR_EXT_OPTION_CLEAN, dir_ext_test_clean);
    SH_SAND_GET_ENUM(CTEST_DNX_DIR_EXT_OPTION_TYPE, dir_ext_test_type);
    SH_SAND_GET_ENUM(CTEST_DNX_DIR_EXT_OPTION_DE_TYPE, dir_ext_de_type);
    SH_SAND_GET_BOOL(CTEST_DNX_DIR_EXT_OPTION_TRAFFIC, dir_ext_test_traffic);

    if (dir_ext_test_type == 0)
    {
        LOG_DEBUG_EX(BSL_LOG_MODULE,
                     "%s(), line %d, Testing direct extraction on %s level: %s()\r\n",
                     __FUNCTION__, __LINE__, "BCM", "dnx_field_dir_ext");

        /*
         * Create a Direct Extraction Database in the field processor.
         * Set the qualifier for this database to be InLIF.
         * Set the action to be a counter, with value set to the
         * value of the InLIF from the incoming data.
         */

        /*
         * SHR_FUNC_INIT_VARS(unit);
         */

        bcm_field_group_config_t_init(&grp);
        grp.group = 100;

        /*
         * Define the QSET - use InLIF as qualifier.
         */
        BCM_FIELD_QSET_INIT(grp.qset);
        BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyStageIngress);
        BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyInVPort0);

        /*
         *  Define the ASET - use counter 0.
         */
        BCM_FIELD_ASET_INIT(aset);
        BCM_FIELD_ASET_ADD(aset, bcmFieldActionStat);
        BCM_FIELD_ASET_ADD(aset, bcmFieldActionDrop);

        /*
         * Create the Field group with type Direct Extraction
         */
        grp.priority = BCM_FIELD_GROUP_PRIO_ANY;
        grp.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_ID;
        grp.mode = bcmFieldGroupModeDirectExtraction;
        SHR_IF_ERR_EXIT(bcm_field_group_config_create(unit, &grp));

        /*
         * Attach the action set
         */
        SHR_IF_ERR_EXIT(bcm_field_group_action_set(unit, grp.group, aset));

        /*
         * Create the Direct Extraction entry: * 1. create the entry * 2. Construct the action: * bit 0 = action valid
         * * bits 16:0 = InLIF * 3. Install entry (HW configuration)
         */
        SHR_IF_ERR_EXIT(bcm_field_entry_create(unit, grp.group, &ent));

        bcm_field_extraction_action_t_init(&extract);
        bcm_field_extraction_field_t_init(&ext_inlif[0]);
        bcm_field_extraction_field_t_init(&ext_inlif[1]);

        extract.action = bcmFieldActionStat;
        extract.bias = 0;

        /*
         * First extraction structure indicates action is valid
         */
        ext_inlif[0].flags = BCM_FIELD_EXTRACTION_FLAG_CONSTANT;
        ext_inlif[0].bits = 1;
        ext_inlif[0].value = 1;

        /*
         * second extraction structure indicates to use InLif qualifier
         */
        ext_inlif[1].flags = 0;
        ext_inlif[1].bits = 16;
        ext_inlif[1].lsb = 0;
        ext_inlif[1].qualifier = bcmFieldQualifyInVPort0;

        SHR_IF_ERR_EXIT(bcm_field_direct_extraction_action_add(unit, ent, extract, 2 /* count */ ,
                                                               ext_inlif));

        /*
         * Write entry to HW
         */
        SHR_IF_ERR_EXIT(bcm_field_entry_install(unit, ent));

    }
    else if (dir_ext_test_type == 1)
    {
        bcm_field_qualify_t bcm_qual;

        LOG_INFO_EX(BSL_LOG_MODULE,
                    "%s(), line %d, Testing direct extraction on %s level: %s()\r\n",
                    __FUNCTION__, __LINE__, "DNX", "dnx_field_dir_ext");
        if (dir_ext_de_type == 2)
        {
            unsigned int qual_index, action_index;
            dnx_field_qual_flags_e qual_flags;
            dnx_field_action_flags_e action_flags;
            dnx_field_group_add_flags_e group_add_flags;
            dnx_field_action_t user_action;
            dnx_field_action_in_info_t user_action_info;
            dnx_field_qualifier_in_info_t user_qual_info;
            bcm_field_action_t bcm_mapped_action;
            /**
             * Number of qualifiers to use for testing FOR_CINT scenario
             * of direct extraction on DNX level.
             */
            unsigned int dnx_ctest_dir_ext_num_quals_for_c_de;
            /**
             * Number of actions to use for testing FOR_CINT scenario
             * of direct extraction on DNX level.
             */
            unsigned int dnx_ctest_dir_ext_num_actions_for_c_de;

            LOG_INFO_EX(BSL_LOG_MODULE,
                        "%s(), line %d, Use %s style scenario for direct extraction %s\r\n",
                        __FUNCTION__, __LINE__, "FOR_CINT", EMPTY);
            SHR_IF_ERR_EXIT(dnx_field_context_mode_t_init(unit, &context_mode));
            SHR_IF_ERR_EXIT(dnx_field_context_create(unit, 0, DNX_FIELD_STAGE_IPMF1, &context_mode, &context_id_array));
            presel_qual[0].qual_type_dbal_field = DBAL_FIELD_FWD_LAYER_TYPE_0;
            presel_qual[0].qual_value = DBAL_ENUM_FVAL_LAYER_TYPES_ETHERNET;
            presel_qual[0].qual_mask = 0;

            presel_entry_id.presel_id = 10;
            presel_entry_id.stage = DNX_FIELD_STAGE_IPMF1;

            presel_data.context_id = context_id_array;
            presel_data.nof_qualifiers = 1;
            presel_data.entry_valid = 1;
            sal_memcpy(&(presel_data.qual_data[0]), &(presel_qual[0]), sizeof(presel_data.qual_data[0]));

            SHR_IF_ERR_EXIT(dnx_field_presel_set(unit, 0, &presel_entry_id, &presel_data));

            dnx_ctest_dir_ext_num_quals_for_c_de =
                sizeof(Dnx_ctest_dir_ext_quals_for_c_de) / sizeof(Dnx_ctest_dir_ext_quals_for_c_de[0]);
            dnx_ctest_dir_ext_num_actions_for_c_de =
                sizeof(Dnx_ctest_dir_ext_actions_for_c_de) / sizeof(Dnx_ctest_dir_ext_actions_for_c_de[0]);
            /**
             * For traffic, on AT_Dnx_Cint_field_dir_ext(), the number of quals is set to 1
             * and the number of actions is set to 1 as well
             */
            dnx_field_group_info_t_init(unit, &fg_info);
            fg_info.field_stage = DNX_FIELD_STAGE_IPMF2;
            fg_info.fg_type = DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION;
            for (qual_index = 0; qual_index < dnx_ctest_dir_ext_num_quals_for_c_de; qual_index++)
            {
                if (Dnx_ctest_dir_ext_quals_for_c_de[qual_index].qual_name == NULL)
                {
                    dnx_ctest_dir_ext_num_quals_for_c_de = qual_index;
                    break;
                }
                SHR_IF_ERR_EXIT(dnx_field_qual_in_info_init(unit, &user_qual_info));

                sal_strncpy_s(user_qual_info.name, Dnx_ctest_dir_ext_quals_for_c_de[qual_index].qual_name,
                              sizeof(user_qual_info.name));
                user_qual_info.size = Dnx_ctest_dir_ext_quals_for_c_de[qual_index].qual_nof_bits;
                /*
                 * 4th parameter should be pointer to bcm qualifier, if requested
                 */
                qual_flags = 0;
                SHR_IF_ERR_EXIT(dnx_field_qual_create(unit, qual_flags, &user_qual_info, &bcm_qual));
                SHR_IF_ERR_EXIT(dnx_field_map_qual_bcm_to_dnx
                                (unit, DNX_FIELD_STAGE_IPMF2, bcm_qual, &(fg_info.dnx_quals[qual_index])));
            }
            /*
             * We assume that dnx_ctest_dir_ext_num_quals_for_c_de is smaller than DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG !!!
             * We assume that dnx_ctest_dir_ext_num_actions_for_c_de is smaller than DNX_DATA_MAX_FIELD_GROUP_NOF_ACTION_PER_FG !!!
             */
            /*
             * Note that 'fg_info_p.dnx_actions' has been initiated in 'dnx_field_group_info_t_init' above.
             * Note that, in this test, we refer to the first '2msb' combination only. Currently,
             * the code only supports that.
             */
            for (action_index = 0; action_index < dnx_ctest_dir_ext_num_actions_for_c_de; action_index++)
            {
                bcm_field_action_t bcm_action;
                if (Dnx_ctest_dir_ext_actions_for_c_de[action_index].action_name == NULL)
                {
                    dnx_ctest_dir_ext_num_actions_for_c_de = action_index;
                    break;
                }

                action_flags = 0;
                user_action = DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC,
                                         DNX_FIELD_STAGE_IPMF2,
                                         Dnx_ctest_dir_ext_actions_for_c_de[action_index].dbal_action_id);

                /** The action create API requires BCM action, we map the supplied DNX to BCM */
                SHR_IF_ERR_EXIT(dnx_field_map_action_dnx_to_bcm(unit,
                                                                DNX_FIELD_STAGE_IPMF2,
                                                                user_action, &bcm_mapped_action));
                SHR_IF_ERR_EXIT(dnx_field_action_in_info_t_init(unit, &user_action_info));
                user_action_info.bcm_action = bcm_mapped_action;
                sal_strncpy_s(user_action_info.name, Dnx_ctest_dir_ext_actions_for_c_de[action_index].action_name,
                              sizeof(user_action_info.name));
                user_action_info.size = Dnx_ctest_dir_ext_actions_for_c_de[action_index].action_nof_bits;
                user_action_info.prefix_size = Dnx_ctest_dir_ext_actions_for_c_de[action_index].prefix_nof_bits;
                user_action_info.stage = DNX_FIELD_STAGE_IPMF2;
                /*
                 * 4th parameter should be pointer to bcm action, if requested
                 */
                SHR_IF_ERR_EXIT(dnx_field_action_create(unit,
                                                        action_flags,
                                                        &user_action_info,
                                                        &bcm_action,
                                                        &(Dnx_ctest_dir_ext_actions_for_c_de
                                                          [action_index].dnx_action)));
                fg_info.dnx_actions[action_index] = Dnx_ctest_dir_ext_actions_for_c_de[action_index].dnx_action;
                fg_info.use_valid_bit[action_index] = FALSE;
            }
            group_add_flags = 0;
            SHR_IF_ERR_EXIT(dnx_field_group_add(unit, group_add_flags, &fg_info, &fg_id));
            /**
             * Attach context ID to FG
             */
            SHR_IF_ERR_EXIT(dnx_field_group_attach_info_t_init(unit, &attach_info));
            for (action_index = 0; action_index < dnx_ctest_dir_ext_num_actions_for_c_de; action_index++)
            {
                attach_info.dnx_actions[action_index] = fg_info.dnx_actions[action_index];
                attach_info.action_info[action_index].priority =
                    Dnx_ctest_dir_ext_actions_for_c_de[action_index].action_attach_info.priority;
            }
            for (qual_index = 0; qual_index < dnx_ctest_dir_ext_num_quals_for_c_de; qual_index++)
            {
                attach_info.dnx_quals[qual_index] = fg_info.dnx_quals[qual_index];
                attach_info.qual_info[qual_index].input_type =
                    Dnx_ctest_dir_ext_quals_for_c_de[qual_index].qual_attach_info.input_type;
                attach_info.qual_info[qual_index].input_arg =
                    Dnx_ctest_dir_ext_quals_for_c_de[qual_index].qual_attach_info.input_arg;
                attach_info.qual_info[qual_index].offset =
                    Dnx_ctest_dir_ext_quals_for_c_de[qual_index].qual_attach_info.offset;
            }
            SHR_IF_ERR_EXIT(dnx_field_group_context_attach(unit, 0, fg_id, presel_data.context_id, &attach_info));
        }
        else if (dir_ext_de_type == 1)
        {
            unsigned int qual_index, action_index;
            dnx_field_qual_flags_e qual_flags;
            dnx_field_action_flags_e action_flags;
            dnx_field_group_add_flags_e group_add_flags;
            dnx_field_action_t user_action;
            dnx_field_action_in_info_t user_action_info;
            dnx_field_qualifier_in_info_t user_qual_info;
            bcm_field_action_t bcm_mapped_action;
            /**
             * Number of qualifiers to use for testing NEW interface
             * of direct extraction on DNX level.
             */
            unsigned int dnx_ctest_dir_ext_num_quals_for_n_de;
            /**
             * Number of actions to use for testing NEW interface
             * of direct extraction on DNX level.
             */
            unsigned int dnx_ctest_dir_ext_num_actions_for_n_de;

            LOG_INFO_EX(BSL_LOG_MODULE,
                        "%s(), line %d, Use %s style interface for direct extraction %s\r\n",
                        __FUNCTION__, __LINE__, "SINGLE", EMPTY);
            SHR_IF_ERR_EXIT(dnx_field_context_mode_t_init(unit, &context_mode));
            SHR_IF_ERR_EXIT(dnx_field_context_create(unit, 0, DNX_FIELD_STAGE_IPMF1, &context_mode, &context_id_array));
            presel_qual[0].qual_type_dbal_field = DBAL_FIELD_FWD_LAYER_TYPE_0;
            presel_qual[0].qual_value = DBAL_ENUM_FVAL_LAYER_TYPES_ETHERNET;
            presel_qual[0].qual_mask = 0;

            presel_entry_id.presel_id = 10;
            presel_entry_id.stage = DNX_FIELD_STAGE_IPMF1;

            presel_data.context_id = context_id_array;
            presel_data.nof_qualifiers = 1;
            presel_data.entry_valid = 1;
            sal_memcpy(&(presel_data.qual_data[0]), &(presel_qual[0]), sizeof(presel_data.qual_data[0]));

            SHR_IF_ERR_EXIT(dnx_field_presel_set(unit, 0, &presel_entry_id, &presel_data));

            dnx_ctest_dir_ext_num_quals_for_n_de =
                sizeof(Dnx_ctest_dir_ext_quals_for_n_de) / sizeof(Dnx_ctest_dir_ext_quals_for_n_de[0]);
            dnx_ctest_dir_ext_num_actions_for_n_de =
                sizeof(Dnx_ctest_dir_ext_actions_for_n_de) / sizeof(Dnx_ctest_dir_ext_actions_for_n_de[0]);
            /**
             * If we are in traffic mode the number of quals is set to CTEST_DNX_DIR_EXT_NUM_QUALS_FOR_TRAFFIC
             * and the number of actions is set to CTEST_DNX_DIR_EXT_NUM_ACTIONS_FOR_TRAFFIC
             */
            if (dir_ext_test_traffic == CTEST_DNX_DIR_EXT_TEST_WITH_TRAFFIC)
            {
                dnx_ctest_dir_ext_num_quals_for_n_de = CTEST_DNX_DIR_EXT_NUM_QUALS_FOR_TRAFFIC;
                dnx_ctest_dir_ext_num_actions_for_n_de = CTEST_DNX_DIR_EXT_NUM_ACTIONS_FOR_TRAFFIC;
            }
            dnx_field_group_info_t_init(unit, &fg_info);
            fg_info.field_stage = DNX_FIELD_STAGE_IPMF2;
            fg_info.fg_type = DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION;
            for (qual_index = 0; qual_index < dnx_ctest_dir_ext_num_quals_for_n_de; qual_index++)
            {
                if (Dnx_ctest_dir_ext_quals_for_n_de[qual_index].qual_name == NULL)
                {
                    break;
                }
                SHR_IF_ERR_EXIT(dnx_field_qual_in_info_init(unit, &user_qual_info));

                sal_strncpy_s(user_qual_info.name, Dnx_ctest_dir_ext_quals_for_n_de[qual_index].qual_name,
                              sizeof(user_qual_info.name));
                user_qual_info.size = Dnx_ctest_dir_ext_quals_for_n_de[qual_index].qual_nof_bits;
                /*
                 * 4th parameter should be pointer to bcm qualifier, if requested
                 */
                qual_flags = 0;
                SHR_IF_ERR_EXIT(dnx_field_qual_create(unit, qual_flags, &user_qual_info, &bcm_qual));
                SHR_IF_ERR_EXIT(dnx_field_map_qual_bcm_to_dnx
                                (unit, DNX_FIELD_STAGE_IPMF2, bcm_qual, &(fg_info.dnx_quals[qual_index])));
            }
            /*
             * We assume that dnx_ctest_dir_ext_num_quals_for_n_de is smaller than DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG !!!
             * We assume that dnx_ctest_dir_ext_num_actions_for_n_de is smaller than DNX_DATA_MAX_FIELD_GROUP_NOF_ACTION_PER_FG !!!
             */
            /*
             * Note that 'fg_info_p.dnx_actions' has been initiated in 'dnx_field_group_info_t_init' above.
             * Note that, in this test, we refer to the first '2msb' combination only. Currently,
             * the code only supports that.
             */
            for (action_index = 0; action_index < dnx_ctest_dir_ext_num_actions_for_n_de; action_index++)
            {
                bcm_field_action_t bcm_action;
                if (Dnx_ctest_dir_ext_actions_for_n_de[action_index].action_name == NULL)
                {
                    break;
                }

                action_flags = 0;
                user_action = DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC,
                                         DNX_FIELD_STAGE_IPMF2,
                                         Dnx_ctest_dir_ext_actions_for_n_de[action_index].dbal_action_id);

                /** The action create API requires BCM action, we map the supplied DNX to BCM */
                SHR_IF_ERR_EXIT(dnx_field_map_action_dnx_to_bcm(unit,
                                                                DNX_FIELD_STAGE_IPMF2,
                                                                user_action, &bcm_mapped_action));
                SHR_IF_ERR_EXIT(dnx_field_action_in_info_t_init(unit, &user_action_info));
                user_action_info.bcm_action = bcm_mapped_action;
                sal_strncpy_s(user_action_info.name, Dnx_ctest_dir_ext_actions_for_n_de[action_index].action_name,
                              sizeof(user_action_info.name));
                user_action_info.size = Dnx_ctest_dir_ext_actions_for_n_de[action_index].action_nof_bits;
                user_action_info.prefix_size = Dnx_ctest_dir_ext_actions_for_n_de[action_index].prefix_nof_bits;
                user_action_info.stage = DNX_FIELD_STAGE_IPMF2;
                /*
                 * 4th parameter should be pointer to bcm action, if requested
                 */
                SHR_IF_ERR_EXIT(dnx_field_action_create(unit,
                                                        action_flags,
                                                        &user_action_info,
                                                        &bcm_action,
                                                        &(Dnx_ctest_dir_ext_actions_for_n_de
                                                          [action_index].dnx_action)));
                fg_info.dnx_actions[action_index] = Dnx_ctest_dir_ext_actions_for_n_de[action_index].dnx_action;
                fg_info.use_valid_bit[action_index] = FALSE;
            }
            group_add_flags = 0;
            SHR_IF_ERR_EXIT(dnx_field_group_add(unit, group_add_flags, &fg_info, &fg_id));
            /**
             * Attach context ID to FG
             */
            SHR_IF_ERR_EXIT(dnx_field_group_attach_info_t_init(unit, &attach_info));
            for (action_index = 0; action_index < dnx_ctest_dir_ext_num_actions_for_n_de; action_index++)
            {
                attach_info.dnx_actions[action_index] = fg_info.dnx_actions[action_index];
                attach_info.action_info[action_index].priority =
                    Dnx_ctest_dir_ext_actions_for_n_de[action_index].action_attach_info.priority;
            }
            for (qual_index = 0; qual_index < dnx_ctest_dir_ext_num_quals_for_n_de; qual_index++)
            {
                attach_info.dnx_quals[qual_index] = fg_info.dnx_quals[qual_index];
                attach_info.qual_info[qual_index].input_type =
                    Dnx_ctest_dir_ext_quals_for_n_de[qual_index].qual_attach_info.input_type;
                attach_info.qual_info[qual_index].input_arg =
                    Dnx_ctest_dir_ext_quals_for_n_de[qual_index].qual_attach_info.input_arg;
                attach_info.qual_info[qual_index].offset =
                    Dnx_ctest_dir_ext_quals_for_n_de[qual_index].qual_attach_info.offset;
            }
            SHR_IF_ERR_EXIT(dnx_field_group_context_attach(unit, 0, fg_id, presel_data.context_id, &attach_info));
           /**
            * Perform traffic test if required by 'traffic' option .
            */
            if (dir_ext_test_traffic == CTEST_DNX_DIR_EXT_TEST_WITH_TRAFFIC)
            {
                /**
                 * Sending the packet and checking the signals
                 */
                SHR_IF_ERR_EXIT(ctest_dnx_dir_ext_traffic_test(unit, &Packet_info, Packet_sig_info));
            }
        }
        else if (dir_ext_de_type == 0)
        {
            unsigned int qual_index, action_index;
            dnx_field_qual_flags_e qual_flags;
            dnx_field_action_flags_e action_flags;
            dnx_field_group_add_flags_e group_add_flags;
            dnx_field_action_t user_action;
            dnx_field_action_in_info_t user_action_info;
            dnx_field_qualifier_in_info_t user_qual_info;
            bcm_field_action_t bcm_mapped_action;
            dnx_field_group_t fg_id_index;
            /**
             * Number of qualifiers to use for testing MULTIPLE
             * direct extraction on DNX level.
             */
            unsigned int dnx_ctest_dir_ext_num_quals_for_multiple_de;
            /**
             * Number of actions to use for testing MULTIPLE MULTIPLE
             * direct extraction on DNX level.
             */
            unsigned int dnx_ctest_dir_ext_num_actions_for_multiple_de;

            LOG_INFO_EX(BSL_LOG_MODULE,
                        "%s(), line %d, Use %s style interface for direct extraction %s\r\n",
                        __FUNCTION__, __LINE__, "MULTIPLE", EMPTY);
            SHR_IF_ERR_EXIT(dnx_field_context_mode_t_init(unit, &context_mode));
            SHR_IF_ERR_EXIT(dnx_field_context_create(unit, 0, DNX_FIELD_STAGE_IPMF1, &context_mode, &context_id_array));
            presel_qual[0].qual_type_dbal_field = DBAL_FIELD_FWD_LAYER_TYPE_0;
            presel_qual[0].qual_value = DBAL_ENUM_FVAL_LAYER_TYPES_ETHERNET;
            presel_qual[0].qual_mask = 0;

            presel_entry_id.presel_id = 10;
            presel_entry_id.stage = DNX_FIELD_STAGE_IPMF1;

            presel_data.context_id = context_id_array;
            presel_data.nof_qualifiers = 1;
            presel_data.entry_valid = 1;
            sal_memcpy(&(presel_data.qual_data[0]), &(presel_qual[0]), sizeof(presel_data.qual_data[0]));

            SHR_IF_ERR_EXIT(dnx_field_presel_set(unit, 0, &presel_entry_id, &presel_data));
            for (fg_id_index = 0; fg_id_index < CTEST_DNX_DIR_EXT_NUM_FGS_FOR_MULTIPLE; fg_id_index++)
            {
                for (qual_index = 0;; qual_index++)
                {
                    if (Dnx_ctest_dir_ext_quals_for_multiple_de[fg_id_index][qual_index].qual_name == NULL)
                    {
                        break;
                    }
                }
                dnx_ctest_dir_ext_num_quals_for_multiple_de = qual_index;
                for (action_index = 0;; action_index++)
                {
                    if (Dnx_ctest_dir_ext_actions_for_multiple_de[fg_id_index][action_index].action_name == NULL)
                    {
                        break;
                    }
                }
                dnx_ctest_dir_ext_num_actions_for_multiple_de = action_index;
                /*
                 * If we are in traffic mode the number of quals is set to CTEST_DNX_DIR_EXT_NUM_QUALS_FOR_TRAFFIC
                 * and the number of actions is set to CTEST_DNX_DIR_EXT_NUM_ACTIONS_FOR_TRAFFIC
                 */
                if (dir_ext_test_traffic == CTEST_DNX_DIR_EXT_TEST_WITH_TRAFFIC)
                {
                    if (fg_id_index == CTEST_DNX_DIR_EXT_INDEX_MULTIPLE_FG_FOR_TRSFFIC)
                    {
                        dnx_ctest_dir_ext_num_quals_for_multiple_de = CTEST_DNX_DIR_EXT_NUM_QUALS_FOR_TRAFFIC;
                        dnx_ctest_dir_ext_num_actions_for_multiple_de = CTEST_DNX_DIR_EXT_NUM_ACTIONS_FOR_TRAFFIC;
                    }
                }
                dnx_field_group_info_t_init(unit, &fg_info);
                fg_info.field_stage = DNX_FIELD_STAGE_IPMF2;
                fg_info.fg_type = DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION;
                for (qual_index = 0; qual_index < dnx_ctest_dir_ext_num_quals_for_multiple_de; qual_index++)
                {
                    SHR_IF_ERR_EXIT(dnx_field_qual_in_info_init(unit, &user_qual_info));

                    sal_strncpy_s(user_qual_info.name,
                                  Dnx_ctest_dir_ext_quals_for_multiple_de[fg_id_index][qual_index].qual_name,
                                  sizeof(user_qual_info.name));
                    user_qual_info.size =
                        Dnx_ctest_dir_ext_quals_for_multiple_de[fg_id_index][qual_index].qual_nof_bits;
                    /*
                     * 4th parameter should be pointer to bcm qualifier, if requested
                     */
                    qual_flags = 0;
                    SHR_IF_ERR_EXIT(dnx_field_qual_create(unit, qual_flags, &user_qual_info, &bcm_qual));
                    SHR_IF_ERR_EXIT(dnx_field_map_qual_bcm_to_dnx
                                    (unit, DNX_FIELD_STAGE_IPMF2, bcm_qual, &(fg_info.dnx_quals[qual_index])));
                }
                /*
                 * We assume that dnx_ctest_dir_ext_num_quals_for_multiple_de is smaller than DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG !!!
                 * We assume that dnx_ctest_dir_ext_num_actions_for_multiple_de is smaller than DNX_DATA_MAX_FIELD_GROUP_NOF_ACTION_PER_FG !!!
                 */
                /*
                 * Note that 'fg_info_p.dnx_actions' has been initiated in 'dnx_field_group_info_t_init' above.
                 * Note that, in this test, we refer to the first '2msb' combination only. Currently,
                 * the code only supports that.
                 */
                for (action_index = 0; action_index < dnx_ctest_dir_ext_num_actions_for_multiple_de; action_index++)
                {
                    bcm_field_action_t bcm_action;
                    action_flags = 0;
                    user_action = DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC,
                                             DNX_FIELD_STAGE_IPMF2,
                                             Dnx_ctest_dir_ext_actions_for_multiple_de[fg_id_index]
                                             [action_index].dbal_action_id);

                    /*
                     * The action create API requires BCM action, we map the supplied DNX to BCM
                     */
                    SHR_IF_ERR_EXIT(dnx_field_map_action_dnx_to_bcm(unit,
                                                                    DNX_FIELD_STAGE_IPMF2,
                                                                    user_action, &bcm_mapped_action));
                    SHR_IF_ERR_EXIT(dnx_field_action_in_info_t_init(unit, &user_action_info));
                    user_action_info.bcm_action = bcm_mapped_action;
                    sal_strncpy_s(user_action_info.name,
                                  Dnx_ctest_dir_ext_actions_for_multiple_de[fg_id_index][action_index].action_name,
                                  sizeof(user_action_info.name));
                    user_action_info.size =
                        Dnx_ctest_dir_ext_actions_for_multiple_de[fg_id_index][action_index].action_nof_bits;
                    user_action_info.prefix_size =
                        Dnx_ctest_dir_ext_actions_for_multiple_de[fg_id_index][action_index].prefix_nof_bits;
                    user_action_info.stage = DNX_FIELD_STAGE_IPMF2;
                    /*
                     * 4th parameter should be pointer to bcm action, if requested
                     */
                    SHR_IF_ERR_EXIT(dnx_field_action_create(unit,
                                                            action_flags,
                                                            &user_action_info,
                                                            &bcm_action,
                                                            &(Dnx_ctest_dir_ext_actions_for_multiple_de[fg_id_index]
                                                              [action_index].dnx_action)));
                    fg_info.dnx_actions[action_index] =
                        Dnx_ctest_dir_ext_actions_for_multiple_de[fg_id_index][action_index].dnx_action;
                    fg_info.use_valid_bit[action_index] = FALSE;
                }
                group_add_flags = 0;
                SHR_IF_ERR_EXIT(dnx_field_group_add(unit, group_add_flags, &fg_info, &fg_id));
                /**
                 * Attach context ID to FG
                 */
                SHR_IF_ERR_EXIT(dnx_field_group_attach_info_t_init(unit, &attach_info));
                for (action_index = 0; action_index < dnx_ctest_dir_ext_num_actions_for_multiple_de; action_index++)
                {
                    attach_info.dnx_actions[action_index] = fg_info.dnx_actions[action_index];
                    attach_info.action_info[action_index].priority =
                        Dnx_ctest_dir_ext_actions_for_multiple_de[fg_id_index][action_index].
                        action_attach_info.priority;
                }
                for (qual_index = 0; qual_index < dnx_ctest_dir_ext_num_quals_for_multiple_de; qual_index++)
                {
                    attach_info.dnx_quals[qual_index] = fg_info.dnx_quals[qual_index];
                    attach_info.qual_info[qual_index].input_type =
                        Dnx_ctest_dir_ext_quals_for_multiple_de[fg_id_index][qual_index].qual_attach_info.input_type;
                    attach_info.qual_info[qual_index].input_arg =
                        Dnx_ctest_dir_ext_quals_for_multiple_de[fg_id_index][qual_index].qual_attach_info.input_arg;
                    attach_info.qual_info[qual_index].offset =
                        Dnx_ctest_dir_ext_quals_for_multiple_de[fg_id_index][qual_index].qual_attach_info.offset;
                }
                SHR_IF_ERR_EXIT(dnx_field_group_context_attach(unit, 0, fg_id, presel_data.context_id, &attach_info));
               /**
                * Perform traffic test if required by 'traffic' option .
                */
                if (dir_ext_test_traffic == CTEST_DNX_DIR_EXT_TEST_WITH_TRAFFIC)
                {
                    if (fg_id_index == CTEST_DNX_DIR_EXT_INDEX_MULTIPLE_FG_FOR_TRSFFIC)
                    {
                        /**
                         * Sending the packet and checking the signals
                         */
                        SHR_IF_ERR_EXIT(ctest_dnx_dir_ext_traffic_test(unit, &Packet_info, Packet_sig_info));
                    }
                }
            }
        }
        else
        {
            /*
             * This should not happen since CLI machine should not allow these values.
             */
            LOG_INFO_EX(BSL_LOG_MODULE,
                        "%s(), line %d, Option %d (for 'dir_ext_de_type') is not implemented. Illegal parameter. %s\r\n",
                        __FUNCTION__, __LINE__, dir_ext_de_type, EMPTY);
            SHR_IF_ERR_EXIT(_SHR_E_PARAM);
        }
    }
    else
    {
        /*
         * This should not happen since CLI machine should not allow these values.
         */
        LOG_INFO_EX(BSL_LOG_MODULE,
                    "%s(), line %d, Option %d (for 'dir_ext_test_type') is not implemented. Illegal parameter. %s\r\n",
                    __FUNCTION__, __LINE__, dir_ext_test_type, EMPTY);
        SHR_IF_ERR_EXIT(_SHR_E_PARAM);
    }

exit:
    if (dir_ext_test_clean == TRUE)
    {
        CTEST_DNX_FIELD_UTIL_ERR(ctest_dnx_field_util_clean(unit));
    }
    SHR_FUNC_EXIT;

}
/**
 * \brief - run Direct Extraction sequence in diag shell
 */
shr_error_e
sh_dnx_dir_ext_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int count_iter, count;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(args, _SHR_E_PARAM, "args");
    SHR_NULL_CHECK(sand_control, _SHR_E_PARAM, "sand_control");
    SH_SAND_GET_UINT32(CTEST_DNX_DIR_EXT_OPTION_COUNT, count);
    for (count_iter = 0; count_iter < count; count_iter++)
    {
        SHR_IF_ERR_EXIT(appl_dnx_dir_ext_init(unit, args, sand_control));
    }
exit:
    SHR_FUNC_EXIT;
}
