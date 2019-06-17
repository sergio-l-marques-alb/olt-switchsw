/**
 * \file smart_template.h
 */

#ifndef SMART_TEMPLATE_H_INCLUDED
/*
 * { 
 */
#define SMART_TEMPLATE_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <bcm/types.h>
#include <soc/dnxc/swstate/types/sw_state_template_mngr.h>
#include <bcm_int/dnx/algo/res_mngr/simple_bitmap_wrap.h>

/**
 * \brief Smart template manager.
 *
 * This template manager advanced algorithm is used to allocate template manager profiles
 * in an advanced manner by using a built in resource manager. All the options available to use
 * via the simple_bitmap_wrap will be in effect here.
 *
 * NOTE: The smart template currently doesn't support allocating more than one profile per entry.
 * Attempting to allocate would be successful, but there's currently no way to free all allocated profiles.
 */

/*
 * TYPEDEFS:
 * { 
 */

/**
 * \brief
 * Pass this struct when creating the smart template to customize the bitmap that the
 * tagged bitmap that the template will be using.
 */
typedef struct
{
    uint32 resource_flags;
    simple_bitmap_wrap_create_info_t resource_create_info;
} smart_template_create_info_t;

/**
 * \brief
 * Pass this struct when allocating entries to provide specific parameters for the allocation.
 */
typedef struct
{
    uint32 resource_flags;
    simple_bitmap_wrap_alloc_info_t resource_alloc_info;
} smart_template_alloc_info_t;

/*
 * } 
 */

/**
 * \brief
 *   Callback to allocate a free profile of an advanced algorithm.
 *
 *  \param [in] unit -
 *      Relevant unit.
 *  \param [in] module_id - Module Id.
 *  \param [in] multi_set_template - Mutli set template.
 *  \param [in] flags -
 *      SW_STATEALGO_TEMPLATE_ALLOCATE_* flags
 *   \param [in] nof_references -
 *      Number of references to be allocated.
 *  \param [in] profile_data -
 *      Pointer to memory holding template data to be saved to a profile.
 *  \param [in] extra_arguments -
 *      A pointer to smart_template_create_info_t. See \ref dnx_algo_res_simple_bitmap_create for
 *      more info on the inner struct.
 *  \param [in,out] profile -
 *      Pointer to place the allocated profile.
 *      \b As \b output - \n
 *        Holds the allocated profile.
 *      \b As \b input - \n
 *        If flag \ref SW_STATE_ALGO_TEMPLATE_ALLOCATE_WITH_ID is set, pass the profile to be allocated here.
 *  \param [in,out] first_reference -
 *      Pointer to memory to load output into. \n
 *      \b As \b output - \n
 *        Will be filled with boolean indication of the profile's prior existence. \n
 *        If TRUE, this is the first reference to the profile, and the data needs to be
 *          written to the relevant table.
 *
 * \return
 *   shr_error_e
 * \remark
 *      None
 * \see
 *      shr_error_e
 *****************************************************/
shr_error_e dnx_algo_smart_template_allocate(
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
 *   Create a new instance of dnx_algo_smart_template.
 *
 *  \param [in] unit -
 *      Relevant unit.
 *  \param [in] module_id - Module Id.
 *  \param [in] multi_set_template - Mutli set template.
 *  \param [in] create_data -
 *      Pointed memory contains setup parameters required for the
 *      creation of the template.
 *  \param [in] extra_arguments -
 *      A pointer to smart_template_alloc_info_t. See \ref dnx_algo_res_simple_bitmap_allocate for
 *      more info on the inner struct.
 *  \param [in] nof_members - Number of members. Specified in the relevant sw state xml file.
 *  \param [in] alloc_flags - SW state allocation flags.
 *       This flags value is determend by the variable type (dnx data for example) of the additional variables specified in the xml
 * \return
 *   shr_error_e
 * \remark
 *      None
 * \see
 *      shr_error_e
 *****************************************************/
shr_error_e dnx_algo_smart_template_create(
    int unit,
    uint32 module_id,
    sw_state_multi_set_t * multi_set_template,
    sw_state_algo_template_create_data_t * create_data,
    void *extra_arguments,
    uint32 nof_members,
    uint32 alloc_flags);

/**
 * \brief
 *   Callback to free an allocated profile in an advanced algorithm.
 *
 *  \param [in] unit -
 *      Relevant unit.
 *  \param [in] module_id - Module Id.
 *  \param [in] multi_set_template - Mutli set template.
 *  \param [in] profile -
 *      Profile to be freed.
 *   \param [in] nof_references -
 *      Number of references to be freed.
 *  \param [in,out] last_reference -
 *      Pointer to memory to load output into. \n
 *      \b As \b output - \n
 *        Will be filled with boolean indication of whether it's the last reference to this profile.
 * \return
 *   shr_error_e
 * \remark
 *      None
 * \see
 *      shr_error_e
 *****************************************************/
shr_error_e dnx_algo_smart_template_free(
    int unit,
    uint32 module_id,
    sw_state_multi_set_t multi_set_template,
    int profile,
    int nof_references,
    uint8 *last_reference);

/**
 * \brief
 *   Free all reference to all profiles of this template.
 *
 *  \param [in] unit -
 *      Relevant unit.
 *  \param [in] module_id - Module Id.
 *  \param [in] multi_set_template - Mutli set template.
 * \return
 *   shr_error_e
 * \remark
 *      None
 * \see
 *      shr_error_e
 *****************************************************/
shr_error_e dnx_algo_smart_template_clear(
    int unit,
    uint32 module_id,
    sw_state_multi_set_t multi_set_template);
/*
 * } 
 */
#endif /* SMART_TEMPLATE_H_INCLUDED */
