
/** \file diag_dnx_field_qual_offset.h
 * Qual offset feature testing.
 */
/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
* Include files.
* {
*/
#include <appl/diag/sand/diag_sand_framework.h>
#include <bcm_int/dnx/field/field_map.h>
/*
 * }
 */

#ifndef DIAG_DNX_FIELD_QUAL_OFFSET_H_INCLUDED
/*
 * {
 */
#define DIAG_DNX_FIELD_QUAL_OFFSET_H_INCLUDED
/*
 *  DEFINES
 * {
 */
/**
 * \brief
 *   Keyword for test type on 'qual_offset' command (qualifier offset testing)
 *   Type can be either PBUS or KEY_TEMPLATE
 */
#define CTEST_DNX_QUAL_OFFSET_OPTION_TYPE         "type"
/*
 * }
 */
/*
 *  Typedefs
 * {
 */
/**
 * Structure for containing info about all qualifiers that are to be tested within appl_dnx_field_qual_offset_run
 * (by calling bcm_field_qualifier_info_get()).
 * The structure contains a brief description of the qualifier to be tested and the expected result.
 */
typedef struct
{
    /**
     * Descriptor of the qualifier to be tested. Mainly related to 'class'
     */
    char *qual_descriptor;
    /**
     * Stage corresponding to specified qualifier. In theory, the same qualifier, on a different stage,
     * may produce a different offset.
     */
    bcm_field_stage_t bcm_stage;
    /**
     * BCM qualifier to test.
     */
    bcm_field_qualify_t bcm_qualifier;
    /**
     * Expected value of 'offset' for specified parameters.
     */
    int expected_offset;
} dnx_field_qual_offset_info_t;
/*
 * }
 */
/*
 * Globals
 * {
 */
/**
 * \brief
 *   Options list for 'Qual_OffSeT' shell command
 */
extern sh_sand_option_t Sh_dnx_field_qual_offset_options[];

/**
 * \brief
 *   man for 'Qual_OffSeT' command
 */
extern sh_sand_man_t Sh_dnx_field_qual_offset_man;
/**
 * \brief
 *   Options list for 'Qual_OffSeT' tests to run on regression, precommit, ...
 */
extern sh_sand_invoke_t Sh_dnx_field_qual_offset_tests[];
/*
 * }
 */
/*
 * Prototypes
 * {
 */
/**
 * \brief - run Qual_OffSeT init sequence in diag shell
 *
 * \param [in] unit - Number of hardware unit used
 *      [in] args - Pointer to args_t struct, not used
 *      [in] sand_control - Pointer to comamnd control structure used for parameter delivery
 *\return
 *   \retval Non-zero (!= BCM_E_NONE) in case of an error
 *   \retval Zero (= BCM_E_NONE) in case of NO ERROR
 *\see
 *   shr_error_e
 */
shr_error_e sh_dnx_field_qual_offset_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);
/*
 * }
 */
/* } */
#endif /* DIAG_DNX_FIELD_QUAL_OFFSET_H_INCLUDED */
