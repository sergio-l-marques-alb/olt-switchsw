/*! \file template_manager_types.h
 * 
 * Internal DNX template manager APIs 
 * 
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 */
#ifndef ALGO_TEMPLATE_MNGR_TYPES_INCLUDED
/*
 * { 
 */
#define ALGO_TEMPLATE_MNGR_TYPES_INCLUDED

/**
 *  
 *   \brief General types for template manager.
 *  
 */

/**
 * Test template names. Included here so they can be attached to a print callback.
 * {
 */

#define TEMPLATE_TEST_BAD_VALUES        "template_test_bad_values"

#define TEMPLATE_TEST_WITH_DEFAULT      "template_with_default"

#define TEMPLATE_TEST_WITHOUT_DEFAULT   "template_test_without_default"

#define TEMPLATE_TEST_PER_CORE          "template_test_per_core"

#define TEMPLATE_TEST_WITH_DEFAULT_CHECK_VALUES      "template_with_default_check_values"

#define TEMPLATE_TEST_WITHOUT_DEFAULT_CHECK_VALUES    "template_test_without_default_check_values"

#define TEMPLATE_TEST_PER_CORE_CHECK_VALUES           "template_test_per_core_check_values"

/**
 * Maximum number of print call backs.
 */
#define DNX_ALGO_TEMPLATE_PRINT_CB_MAX_SIZE 2000

/**
 * Maximum print string size when the user use a different print format.
 */
#define DNX_ALGO_TEMPLATE_PRINT_CB_MAX_PRINT_STRING_SIZE 2048

/*
 * Each of these enum members represents an type that can be printed with _print_cb.
 */
typedef enum
{
    TEMPLATE_MNGR_PRINT_TYPE_INVALID = -1,
    TEMPLATE_MNGR_PRINT_TYPE_UINT8,
    TEMPLATE_MNGR_PRINT_TYPE_UINT16,
    TEMPLATE_MNGR_PRINT_TYPE_UINT32,
    TEMPLATE_MNGR_PRINT_TYPE_CHAR,
    TEMPLATE_MNGR_PRINT_TYPE_SHORT,
    TEMPLATE_MNGR_PRINT_TYPE_INT,
    TEMPLATE_MNGR_PRINT_TYPE_MAC,
    TEMPLATE_MNGR_PRINT_TYPE_IPV4,
    TEMPLATE_MNGR_PRINT_TYPE_IPV6,
    TEMPLATE_MNGR_PRINT_TYPE_STRING,
    TEMPLATE_MNGR_PRINT_TYPE_COUNT
} template_mngr_print_variable_type_e;

/*
 * Each of these enum members represents an advanced algorithm used by the template manager.
 */
typedef enum
{
    DNX_ALGO_TEMPLATE_ADVANCED_ALGORITHM_INVALID = -1,
    DNX_ALGO_TEMPLATE_ADVANCED_ALGORITHM_BASIC = 0,
    DNX_ALGO_TEMPLATE_ADVANCED_ALGORITHM_SMART_TEMPLATE,
    DNX_ALGO_TEMPLATE_ADVANCED_ALGORITHM_L3_SOURCE_TABLE,
    DNX_ALGO_TEMPLATE_ADVANCED_ALGORITHM_TRAP_MTU_PROFILE,
    DNX_ALGO_TEMPLATE_ADVANCED_ALGORITHM_TRAP_ETPP_LIF_PROFILE,
    DNX_ALGO_TEMPLATE_ADVANCED_ALGORITHM_COUNT
} dnx_algo_template_advanced_algorithms_e;

/**
 * \brief The name of the template manager instance, used with all template manager functions. 
 */
typedef char *dnx_algo_template_name_t;

typedef struct
{
    /*
     * Enum of the relevant a variable type.
     */
    template_mngr_print_variable_type_e type;
    /*
     * Variable name. If the user do not want print the variable name this variable should be NULL.
     */
    char *name;
    /*
     * Format string. If the user do not want a custom printing format this variable should be NULL.
     */
    char *format_string;
    /*
     * Variable data. Must not be NULL.
     */
    uint8 data[DNX_ALGO_TEMPLATE_PRINT_CB_MAX_PRINT_STRING_SIZE];
    /*
     * Comment. If the user do not want leave a comment this variable should be NULL.
     */
    char *comment;
    /*
     * Sub Struct Level. Specify if the sub structures to be indented properly.
     */
    uint8 sub_struct_level;
    /*
     * Is array flag
     */
    uint8 is_arr_flag;
    /*
     * Array index
     */
    uint8 arr_index;
} dnx_algo_template_print_t;

/**
 * \brief Callback to print the data stored in template manager. 
 *      Use the framework starting with \ref DNX_ALGO_TEMPLATE_PRINT_CB_INIT_VARIABLES
 *      to print your resource data.
 *      See \ref dnx_algo_template_test_print_cb for example.
 *
 *  \par DIRECT INPUT:
 *   \param [in] unit -
 *     Identifier of the device to access.
 *    \param [in] data -
 *      Pointer of the struct to be printed.
 *    \param [in] print_cb_data -
 *      Pointer of the print callback data.
 *  \par DIRECT OUTPUT:
 *      None
 *  \remark
 *    None
 *  \see
 *    dnx_algo_template_dump
 *****************************************************/
typedef void (
    *dnx_algo_template_print_data_cb) (
    int unit,
    const void *data,
    dnx_algo_template_print_t * print_cb_data);

/**
 * \brief Template creation information
 *
 * This structure contains the information required for creating a new template.
 * 
 *  \see 
 * dnx_algo_template_create
 */
typedef struct
{
    /*
     *
     * DNX_ALGO_TEMPLATE_INIT_* flags
     */
    uint32 flags;
    /*
     *
     * First profile id of the template.
     */
    int first_profile;
    /*
     *
     * How many profiles are in the template.
     */
    int nof_profiles;
    /*
     *
     * Maximum number of pointers to each profile.
     */
    int max_references;
    /*
     * 
     * Default profile for the template. To be used if flag 
     * DNX_ALGO_TEMPLATE_INIT_USE_DEFAULT_PROFILE is set.
     */
    int default_profile;
    /*
     *
     * Size of the template's data.
     */
    int data_size;
    /*
     * 
     * If flag SW_STATE_ALGO_TEMPLATE_CREATE_USE_DEFAULT_PROFILE is set, put here the data that it will contain.
     */
    void *default_data;
    /*
     * Enum of the relevant advanced algorithm. If we did not have any we should put DNX_ALGO_TEMPLATE_ADVANCED_ALGORITHM_BASIC.
     */
    dnx_algo_template_advanced_algorithms_e advanced_algorithm;
} dnx_algo_template_create_data_t;

/**
 * \brief Template information
 *
 * This structure contains the information that will be displayed with the diag template command.
 *
 *  \see
 * dnx_algo_template_dump_info_get
 */
typedef struct
{
    /*
     *This structure contains the information required for creating a new template.
     */
    dnx_algo_template_create_data_t create_data;
    /*
     * *
     * *How many profiles are used in the template.
     */
    int nof_used_profiles;
    /*
     * *
     * *How many profiles are free in the template.
     */
    int nof_free_profiles;
} dnx_algo_template_dump_data_t;

/*
 * } 
 */
#endif/*_ALGO_TEMPLATE_MNGR_CALLBACKS_INCLUDED__*/
