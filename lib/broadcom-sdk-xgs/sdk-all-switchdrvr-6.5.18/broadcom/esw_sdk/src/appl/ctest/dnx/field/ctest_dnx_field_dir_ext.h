
/** \file diag_dnx_field_dir_ext.h
 * Direct Extraction appl
 */
/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#ifndef DIAG_DNX_DIR_EXT_H_INCLUDED
/*
 * {
 */
#define DIAG_DNX_DIR_EXT_H_INCLUDED
/*
* Include files.
* {
*/
/** shared */
#include <shared/bsl.h>
#include <shared/bslenum.h>
/** appl */
#include <appl/diag/diag.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
/** bcm */
#include <bcm/field.h>
#include <bcm/types.h>
#include <bcm/rx.h>
#include <bcm/error.h>
/** sal */
#include <sal/appl/sal.h>
#include <sal/core/libc.h>

#include <soc/dnx/swstate/auto_generated/types/dnx_field_types.h>

/*
 * }
 */
/*
 * DEFINEs
 * {
 */
/**
 * Define will be used for two purposes:
 *      - to set the qualifier size
 *      - to set how many bits will be extracted and used for action value
 * Note that this definition is implicitely used by AT_Dnx_Cint_field_dir_ext()
 * on regress/bcm/tests/dnx/cints_tests/cintsFieldDvapis.tcl
 */
#define CTEST_DNX_DIR_EXT_NOF_EXTRACTED_BITS        3
/**
 * Shows the mode of the test. IF it is with traffic or not.
 * Relevent only for de_type=NEW.
 */
#define CTEST_DNX_DIR_EXT_TEST_WITH_TRAFFIC         1
/**
 * Number of qualifiers to use for testing NEW interface
 * of direct extraction on DNX level with traffic.
 */
#define CTEST_DNX_DIR_EXT_NUM_QUALS_FOR_TRAFFIC     3
/**
 * Number of actions to use for testing NEW interface
 * of direct extraction on DNX level with traffic.
 */
#define CTEST_DNX_DIR_EXT_NUM_ACTIONS_FOR_TRAFFIC   2
/**
 * For each retry how many seconds to wait.
 * Used in testing NEW interface of DE on DNX level with traffic.
 */
#define CTEST_DNX_DIR_EXT_PACKET_WAIT               5
/**
 * Number of 'field groups' to use for testing 'multiple direct extractions'
 * on DNX level. Each 'field group' gets a different bit-range on the same
 * key (and same context
 */
#define CTEST_DNX_DIR_EXT_NUM_FGS_FOR_MULTIPLE               3
/**
 * Index of 'field groups' to use for testing 'multiple direct extractions'
 * plus traffic test. (between 0 and (CTEST_DNX_DIR_EXT_NUM_FGS_FOR_MULTIPLE - 1)
 */
#define CTEST_DNX_DIR_EXT_INDEX_MULTIPLE_FG_FOR_TRSFFIC      1
/**
 * Number of 'field groups' to use for testing 'multiple direct extractions'
 * on DNX level with traffic. Each 'field group' gets a different 'bit-range'
 * on the same key (and the same context).
 */
#define CTEST_DNX_DIR_EXT_NUM_FGS_FOR_MULTIPLE               3
/**
 * Max number of actions per 'field groups' to use for testing 'multiple direct extractions'
 * on DNX level with traffic.
 */
#define CTEST_DNX_DIR_EXT_MAX_NUM_ACTIONS_PER_FG            10
/**
 * Max number of qualifiers per 'field groups' to use for testing 'multiple direct extractions'
 * on DNX level with traffic.
 */
#define CTEST_DNX_DIR_EXT_MAX_NUM_QUALS_PER_FG              10
/**
 *
 * PACKET HEADER
 * The key is created from the bits of the ETH header,
 * because the input_arg is equal to 0.
 *        _____________________  _______________________
 * field [        ETH.DA       ][          ETH.SA       ]
 *        ~~~~~~~~~~~~~~~~~~~~~  ~~~~~~~~~~~~~~~~~~~~~~~~ 
 *  bits [ 11:96:c6:98:00:01  ][    00:00:00:00:00:01   ]
 *        ~~~~~~~~~~~~~~~~~~~~~  ~~~~~~~~~~~~~~~~~~~~~~~~ 
 *        ______________________________________________________ 
 * field [               ETH.DA in binary                       ]
 *        ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  bits [ 00010001:10010110:11000110:10011000:00000000:00000001]
 *        ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *
 * The position in the key depends on Qualifiers sequence.
 *  * Qualifier 0 is added to LSB.
 *  * Following qualifiers follow in the direction of MSB.
 *     * We get first qual from offset 5 of the ETH header and
 *         it is with size = 5
 *     * Second qual we get from offset 10 of the ETH header and
 *         it is with size = 8
 *     * Third qual we get from offset 20 of the ETH header and
 *         it is with size = 7
 *
 * THE KEY
 *         ________________  ________________  ________________
 * field  [  Qualifier 2  ]  [  Qualifier 1 ]  [  Qualifier 0 ]
 *         ~~~~~~~~~~~~~~~~  ~~~~~~~~~~~~~~~~  ~~~~~~~~~~~~~~~~
 * binary [     0110100   ]  [   01011011   ]  [    00110     ]
 *         ~~~~~~~~~~~~~~~~  ~~~~~~~~~~~~~~~~  ~~~~~~~~~~~~~~~~
 *         ~~~~~~~~~~~~~~~~  ~~~~~~~~~~~~~~~~  ~~~~~~~~~~~~~~~~
 * hexa   [       34      ]  [      5B      ]  [      06      ]
 *         ~~~~~~~~~~~~~~~~  ~~~~~~~~~~~~~~~~  ~~~~~~~~~~~~~~~~
 *      MSB                                                LSB
 *
 * The signals that we expect are actually the actions.
 *  * The actions extract the action values from the key.
 *  * The value of Action 0 is the LSB of the key
 *  * Following actions are extracted from the next portions of 
 *      the FG key in the direction of the MSB
 *      * First action is getting the first 5 bits from the Key,
 *          because it is with size = 5
 *      * The second action is with size 15, so it takes the next 15 bits.
 */
#define CTEST_DNX_DIR_EXT_USER_HEADER_EXPECTED_VALUE      0x6
#define CTEST_DNX_DIR_EXT_FWD_ACTION_DST_EXPECTED_VALUE   0x345B
/**
 * \brief
 *   Keyword for test type on 'dir_ext' command (direct extraction testing)
 *   Type can be either BCM or DNX
 */
#define CTEST_DNX_DIR_EXT_OPTION_TYPE         "type"
/**
 * \brief
 *   Keyword for test DE type on 'dir_ext' command (direct extraction testing)
 *   Type can be either SINGLE de style or MULTIPLE de style.
 *   See DNX_FIELD_DE_TYPE_SINGLE/DNX_FIELD_DE_TYPE_MULTIPLE
 */
#define CTEST_DNX_DIR_EXT_OPTION_DE_TYPE      "de_type"
/**
 * \brief
 *   Keyword for the number of times the test will run
 */
#define CTEST_DNX_DIR_EXT_OPTION_COUNT        "count"
/**
 * \brief
 *   Keyword for controlling whether to release resources at end of test.
 *   clean can be either 0 or 1, if the ctest will clean-up after its finished setting-up.
 */
#define CTEST_DNX_DIR_EXT_OPTION_CLEAN        "clean"
/**
 * \brief
 *   Keyword for controlling whether to perform tracfic test. can be either 0 or 1.
 *   Only relevant for test de_type NEW.
 */
#define CTEST_DNX_DIR_EXT_OPTION_TRAFFIC      "traffic"
/*
 * }
 */
/*
 * TYPEDEFs
 * {
 */
/**
 * \brief
 *   Structure of descriptors of qualifiers to use for testing new interface
 *   for direct extraction.
 * \see
 *   DNX_CTEST_DIR_EXT_NUM_QUALS_FOR_N_DE
 */
typedef struct
{
    /**
     * Qualifier's Name
     */
    char *qual_name;
    /**
     * Number of bits to assign to this user defined qualifier
     */
    unsigned int qual_nof_bits;
    /**
     * input_type/input_arg/offset to assign to this user defined qualifier.
     */
    dnx_field_qual_attach_info_t qual_attach_info;
} dnx_ctest_dir_ext_quals_for_n_de_t;
/**
 * \brief
 *   Structure of descriptors of actions to use for testing new interface
 *   for direct extraction.
 *   Note that we are only handling user defined actions (class = DNX_FIELD_ACTION_CLASS_USER)
 * \see
 *   DNX_CTEST_DIR_EXT_NUM_QUALS_FOR_N_DE
 */
typedef struct
{
    /**
     * Action's Name
     */
    char *action_name;
    /**
     * Number of bits to assign to this user defined action
     */
    unsigned int action_nof_bits;
    /**
     * Prefix size for this user defined qualifier.
     */
    unsigned int prefix_nof_bits;
    /**
     * DBAL identifier for this action. Also used as index to get base action's
     * properties. See dnx_ipmf1_action_info[].
     */
    dbal_enum_value_field_ipmf1_action_e dbal_action_id;
    /*
     * Action info required for attaching context to filed group.
     * Currently, only contains 'priority'.
     */
    dnx_field_action_attach_info_t action_attach_info;
    /**
     * Memory space to load identifier of this user defined action (result
     * of dnx_field_action_create()).
     */
    dnx_field_action_t dnx_action;
} dnx_ctest_dir_ext_actions_for_n_de_t;
/*
 * }
 */

extern sh_sand_man_t Sh_dnx_dir_ext_man;

extern sh_sand_option_t Sh_dnx_field_dir_ext_options[];

extern sh_sand_invoke_t Sh_dnx_field_dir_ext_tests[];

/**
 * \brief - run Direct Extraction init sequence in diag shell
 */
shr_error_e sh_dnx_dir_ext_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

shr_error_e appl_dnx_dir_ext_init(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);
/*
 * }
 */

#endif /* DIAG_DNX_DIR_EXT_H_INCLUDED */
