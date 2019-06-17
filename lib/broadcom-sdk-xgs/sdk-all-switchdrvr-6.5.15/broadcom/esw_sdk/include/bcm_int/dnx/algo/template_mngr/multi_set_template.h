/** \file multi_set_template.h
 * 
 * Internal DNX template manager APIs 
 * 
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 */
#ifndef MULTI_SET_TEMPLATE_H_INCLUDED
/*
 * { 
 */
#define MULTI_SET_TEMPLATE_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/**
* INCLUDE FILES:
* {
*/
#include <bcm_int/dnx/algo/template_mngr/template_manager_types.h>
#include <soc/dnxc/swstate/types/sw_state_template_mngr.h>
/**
 * }
 */

/**
 * Multi set template is the default algorithm for template manager. It is implemented as an advanced algorithm.
 *
 * It's a wrapper for sw_state_multi_set, but adds further logic on top of it:
 *   - sw_state_multi_set only supports profile ids from 0 to x. Each function in multi_set_template performs mapping
 *     from the external profile id to this internal profile id.
 *   - multi_set_template_exchange performs the complicated logic needed to fully support exchange operation.
 *
 * It is highly recommeneded to use this library as an internal implementation of your advanced algorithm.
 */

/**
* \brief
*   Create a new instance of multi_set_template.
*
*
*    \param [in] unit - Relevant unit.
*    \param [in] module_id - Module ID.
*    \param [in] multi_set_template - Multi set template.
*    \param [in] create_data - Pointed memory contains setup parameters required for the
*      creation of the template.
*    \param [in] extra_arguments - Placeholder to fill prototype requirement for advanced algorithms.
*      Should be NULL for this procedure.
*  \param [in] nof_members - Number of memberst in the multiset. Specified in the relevant sw state xml file.
*  \param [in] alloc_flags - SW state allocation flags.
*       This flags value is determend by the variable type (dnx data for example) of the additional variables specified in the xml
*  \return
*    \retval Zero if no error was detected
*    \retval Negative if error was detected.
*  \remark
*    None
*  \see
*    multi_set_template_create
*    shr_error_e
*/
shr_error_e multi_set_template_create(
    int unit,
    uint32 module_id,
    sw_state_multi_set_t * multi_set_template,
    sw_state_algo_template_create_data_t * create_data,
    void *extra_arguments,
    uint32 nof_members,
    uint32 alloc_flags);

/**
* \brief
*   Destroy an instance of multi set template.
*
*
*    \param [in] unit - Relevant unit.
*    \param [in] module_id - Module ID.
*    \param [in] multi_set_template - Multi set template.
*    \param [in] extra_arguments - Placeholder to fill prototype requirement for advanced algorithms.
*      Should be NULL for this procedure.
*  \return
*    \retval Zero if no error was detected
*    \retval Negative if error was detected.
*  \remark
*    None
*  \see
*    sw_state_algo_template_destroy
*    sw_state_multi_set_destroy
*    shr_error_e
*/
shr_error_e multi_set_template_destroy(
    int unit,
    uint32 module_id,
    sw_state_multi_set_t * multi_set_template,
    void *extra_arguments);

/**
* \brief
*   Callback to allocate a free profile of an advanced algorithm.
*
*
*    \param [in] unit - Relevant unit.
*    \param [in] module_id - Module ID.
*    \param [in] multi_set_template - Multi set template.
*    \param [in] flags - SW_SATTE_ALGO_TEMPLATE_ALLOCATE_* flags
*    \param [in] nof_references - Number of references to be allocated.
*    \param [in] profile_data - Pointer to memory holding template data to be saved to a profile.
*    \param [in] extra_arguments - Pointer to memory holding extra arguments to be passed to the allocation function of an advanced algorithm.
*    \param [in] profile - Pointer to place the allocated profile.
*      \b As \b output - \n
*        Holds the allocated profile.
*      \b As \b input - \n
*        If flag DNX_ALGO_TEMPLATE_ALLOCATE_WITH_ID is set, pass the profile to be allocated here.
*    \param [in] first_reference - Pointer to memory to load output into. \n
*      \b As \b output - \n
*        Will be filled with boolean indication of the profile's prior existence. \n
*        If TRUE, this is the first reference to the profile, and the data needs to be
*          written to the relevant table.
*  \return
*    shr_error_e -
*      Error return value
*  \remark
*    None
*  \see
*    multi_set_template_allocate
*    shr_error_e
*****************************************************/
shr_error_e multi_set_template_allocate(
    int unit,
    uint32 module_id,
    sw_state_multi_set_t multi_set_template,
    uint32 flags,
    int nof_references,
    void *profile_data,
    void *extra_arguments,
    int *profile,
    uint8 *first_reference);

/**
* \brief
*   Callback to free an allocated profile in an advanced algorithm.
*
*    \param [in] unit - Relevant unit.
*    \param [in] module_id - Module ID.
*    \param [in] multi_set_template - Multi set template.
*    \param [in] profile - Profile to be freed.
*    \param [in] nof_references - Number of references to be freed.
*    \param [in] last_reference - Pointer to memory to load output into. \n
*      \b As \b output - \n
*        Will be filled with boolean indication of whether it's the last reference to this profile.
*  \return
*    shr_error_e -
*      Error return value
*  \remark
*    None
*  \see
*    multi_set_template_free
*    shr_error_e
*****************************************************/
shr_error_e multi_set_template_free(
    int unit,
    uint32 module_id,
    sw_state_multi_set_t multi_set_template,
    int profile,
    int nof_references,
    uint8 *last_reference);

/**
* \brief
*   Callback to free all references to a profile in an advanced algorithm.
*
*
*   \param [in] unit - Relevant unit.
*   \param [in] module_id - Module ID.
*   \param [in] multi_set_template - Multi set template.
*   \param [in] profile - All references, and the data, for this profile will be freed.
*   \return
*       \retval Zero if no error was detected
*       \retval Negative if error was detected.
*   \remark
*       None
*   \see
*    sw_state_algo_template_free_all
*****************************************************/
shr_error_e multi_set_template_free_all(
    int unit,
    uint32 module_id,
    sw_state_multi_set_t multi_set_template,
    int profile);

/**
* \brief
*   Get the data pointed by a template's profile.
*
*    \param [in] unit - Relevant unit.
*    \param [in] module_id - Module ID.
*    \param [in] multi_set_template - Multi set template.
*    \param [in] profile - Profile to get the data from.
*    \param [in] ref_count - Pointer to memory to load output into. \n
*      \b As \b output - \n
*        Will be filled with number of refernces to this profile.
*    \param [in] data - Pointer to memory to load output into. \n
*      \b As \b output - \n
*        Will be filled with data stored in this profile.
*   \return
*    \retval Zero if no error was detected
*    \retval Negative if error was detected.
*  \remark
*    None
*  \see
*    multi_set_template_create_data_t
*    multi_set_template_advanced_alogrithm_cb_t
*    shr_error_e
*****************************************************/
shr_error_e multi_set_template_profile_data_get(
    int unit,
    uint32 module_id,
    sw_state_multi_set_t multi_set_template,
    int profile,
    int *ref_count,
    void *data);

/**
* \brief
*   Get a template's profile by given data.
*
*    \param [in] unit - Relevant unit.
*    \param [in] module_id - Module ID.
*    \param [in] multi_set_template - Multi set template.
*    \param [in] data - Pointer to memory for data to lookup. \n
*      \b As \b input - \n
*        Fill this with the data to find.
*    \param [in] profile - Pointer to place the allocated profile.
*      \b As \b output - \n
*        Holds the profile that holds given data.
*   \return
*    \retval Zero if no error was detected
*    \retval Negative if error was detected.
*  \remark
*    None
*  \see
*    multi_set_template_create_data_t
*    multi_set_template_advanced_alogrithm_cb_t
*    shr_error_e
*****************************************************/
shr_error_e multi_set_template_profile_get(
    int unit,
    uint32 module_id,
    sw_state_multi_set_t multi_set_template,
    const void *data,
    int *profile);

/**
* \brief
*   Free all reference to all profiles of this template.
*
*    \param [in] unit - Relevant unit.
*    \param [in] module_id - Module ID.
*    \param [in] multi_set_template - Multi set template.
*   \return
*    \retval Zero if no error was detected
*    \retval Negative if error was detected.
*  \remark
*    None
*  \see
*    multi_set_template_create_data_t
*    multi_set_template_advanced_alogrithm_cb_t
*    shr_error_e
*****************************************************/
shr_error_e multi_set_template_clear(
    int unit,
    uint32 module_id,
    sw_state_multi_set_t multi_set_template);

/*
 * } 
 */
#endif/*_MULTI_SET_TEMPLATE_H_INCLUDED__*/
