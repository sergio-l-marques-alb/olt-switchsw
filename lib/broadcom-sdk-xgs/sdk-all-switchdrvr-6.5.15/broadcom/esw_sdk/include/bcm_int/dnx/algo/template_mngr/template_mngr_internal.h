/*! \file template_mngr_internal.h
 * 
 * Internal DNX template manager APIs 
 * 
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 */
#ifndef DNX_ALGO_TEMPLATE_MNGR_INTERNAL_H_INCLUDED
/*
 * {
 */
#define DNX_ALGO_TEMPLATE_MNGR_INTERNAL_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <bcm_int/dnx/algo/template_mngr/template_manager_types.h>
#include <bcm_int/dnx/algo/template_mngr/template_mngr_callbacks.h>

/*
 * DEFINES 
 * { 
 */

/**
 * Number of supported templates in the system.
 */
#define DNX_ALGO_TEMPLATE_MNGR_NOF_TEMPLATES (130)

/**
 * Invalid template index, used to indicate that the template does not exist.
 */
#define DNX_ALGO_TEMPLATE_INVALID_INDEX -1

/**
 * Number of supported test templates in the system.
 */
#define DNX_ALGO_TEMPLATE_MNGR_NOF_TEST_TEMPLATES 5

/*
 * }
 */

extern const sw_state_dnx_algo_template_advanced_alogrithm_cb_t
    Template_callbacks_map_sw[DNX_ALGO_TEMPLATE_ADVANCED_ALGORITHM_COUNT];

/**
 * FUNCTIONS 
 * { 
 */

/**
 * \brief
 *   Given a template name, return the callback of the print function printing this template.
 *   
 *  \par DIRECT INPUT:
 *    \param [in] name -
 *      Name used to create this template.
 *    \param [in] *print_cb-
 *        dnx_algo_template_print_data_cb pointer to memory to load output into. \n
 *      \b As \b output - \n
 *        Will be filled with the template's print callback (see above). \n
 *        If the template doesn't exist, will be filled with NULL.
 *  \par DIRECT OUTPUT:
 *    \retval Zero if no error was detected
 *    \retval Negative if error was detected. See \ref shr_error_e
 *  \par INDIRECT OUTPUT:
 *    \b *print_cb_index \n
 *     See DIRECT INPUT above
 *  \remark
 *    None
 *  \see
 *    dnx_algo_template_print_data_cb
 *    shr_error_e
 *****************************************************/
shr_error_e dnx_algo_template_print_callback_get(
    dnx_algo_template_name_t name,
    dnx_algo_template_print_data_cb * print_cb);

/**
 * \brief
 *   Adding a entry to the template map, by given template name and a callback of the print function.
 *  \param [in] template_name -
 *      Name used to create the template.
 *  \param [in] print_cb-
 *      dnx_algo_template_print_data_cb variable that needs to
 *      be added.
 *      \n
 *  \return
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *  \remark
 *      None
 *  \see
 *    dnx_algo_template_print_data_cb
 *    shr_error_e
 *****************************************************/
shr_error_e dnx_algo_template_test_print_cb_add(
    dnx_algo_template_name_t template_name,
    dnx_algo_template_print_data_cb print_cb);

/**
 * \brief
 *   Removing a entry from the template map, by given template name.
 *  \param [in] template_name -
 *      Name used to create this template.
 *  \return
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *  \remark
 *      None
 *  \see
 *    dnx_algo_template_print_data_cb
 *    shr_error_e
 *****************************************************/
shr_error_e dnx_algo_template_test_print_cb_remove(
    dnx_algo_template_name_t template_name);

/**
* \brief
*   Print an entry of the unit test data template. See
*       \ref dnx_algo_template_print_data_cb for more details.
*
*  \par DIRECT INPUT:
*    \param [in] unit -
*     Identifier of the device to access.
*    \param [in] data -
*      Pointer of the struct to be printed.
*      \b As \b input - \n
*       The pointer is expected to hold a struct of type template_data_t. \n
*       It's the user's responsibility to verify it beforehand.
*    \param [in] print_cb_data - Pointer of the print callback data.
*  \par INDIRECT INPUT:
*    \b *data \n
*     See DIRECT INPUT above
*  \par DIRECT OUTPUT:
*    None.
*  \par INDIRECT OUTPUT:
*      The default output stream.
*  \remark
*    None
*  \see
*    dnx_algo_template_print_data_cb
*    shr_error_e
*****************************************************/
void dnx_algo_template_test_print_cb(
    int unit,
    const void *data,
    dnx_algo_template_print_t * print_cb_data);

/**
 * } 
 */

/*
 * Internal template manager tests print functions declarations. 
 * { 
 */

/*
 * }
 */

#endif /* DNX_ALGO_TEMPLATE_MNGR_INTERNAL_H_INCLUDED */
