/** \file sw_state_template_mngr.h
 * 
 * Internal DNX template manager APIs 
 * 
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 */
#ifndef SW_STATE_ALGO_TEMPLATE_MNGR_INCLUDED
/*
 * { 
 */
#define SW_STATE_ALGO_TEMPLATE_MNGR_INCLUDED

#include <soc/dnxc/swstate/sw_state_defs.h>
#include <soc/dnxc/swstate/types/sw_state_multi_set.h>
#include <bcm_int/dnx/algo/template_mngr/template_manager_types.h>
#include <soc/dnxc/swstate/dnxc_sw_state_plain.h>
#include <shared/shrextend/shrextend_debug.h>

/*
 * MACROs
 */
#define SW_STATE_TEMP_MNGR_CREATE(module_id, algo_temp, data, extra_arguments, nof_members, alloc_flags)\
        SHR_IF_ERR_EXIT(sw_state_algo_template_create(unit, module_id, &algo_temp, data, extra_arguments, nof_members, alloc_flags))

#define SW_STATE_TEMP_MNGR_DESTROY(module_id, algo_temp, extra_arguments)\
        SHR_IF_ERR_EXIT(sw_state_algo_template_destroy(unit, module_id, &algo_temp, extra_arguments))

#define SW_STATE_TEMP_MNGR_ALLOCATE_SINGLE(module_id, algo_temp, core_id, flags, profile_data, extra_arguments, profile, first_reference)\
        SHR_IF_ERR_EXIT(sw_state_algo_template_allocate(unit, core_id, module_id, algo_temp, flags, profile_data, \
                                                        extra_arguments, profile, first_reference))

#define SW_STATE_TEMP_MNGR_FREE_SINGLE(module_id, algo_temp, core_id, profile, last_reference)\
        SHR_IF_ERR_EXIT(sw_state_algo_template_free(unit, core_id, module_id, algo_temp, profile, last_reference))

#define SW_STATE_TEMP_MNGR_EXCHANGE(module_id, algo_temp, core_id, flags, profile_data, old_profile,extra_arguments, new_profile, first_reference, last_reference)\
        SHR_IF_ERR_EXIT(sw_state_algo_template_exchange(unit, core_id, module_id, algo_temp, flags, profile_data, old_profile, \
                        extra_arguments, new_profile, first_reference, last_reference))

#define SW_STATE_TEMP_MNGR_REPLACE_DATA(module_id, algo_temp, core_id, profile, new_profile_data)\
        SHR_IF_ERR_EXIT(sw_state_algo_template_replace_data(unit, core_id, module_id, algo_temp, profile, new_profile_data))

#define SW_STATE_TEMP_MNGR_PROFILE_DATA_GET(module_id, algo_temp, core_id, profile, ref_count, profile_data)\
        SHR_IF_ERR_EXIT(sw_state_algo_template_profile_data_get(unit, core_id, module_id, algo_temp, profile, ref_count, profile_data))

#define SW_STATE_TEMP_MNGR_PROFILE_GET(module_id, algo_temp, core_id, profile_data, profile)\
        SHR_IF_ERR_EXIT_NO_MSG(sw_state_algo_template_profile_get(unit, core_id, module_id, algo_temp, profile_data, profile))

#define SW_STATE_TEMP_MNGR_CLEAR(module_id, algo_temp, core_id)\
        SHR_IF_ERR_EXIT(sw_state_algo_template_clear(unit, core_id, module_id, algo_temp))

#define SW_STATE_TEMP_MNGR_FREE_ALL(module_id, algo_temp, core_id, profile)\
        SHR_IF_ERR_EXIT(sw_state_algo_template_free_all(unit, core_id, module_id, algo_temp, profile))

#define SW_STATE_TEMP_MNGR_GET_NEXT(module_id, algo_temp, core_id, current_profile)\
        SHR_IF_ERR_EXIT(sw_state_algo_template_get_next(unit, core_id, module_id, algo_temp, current_profile))

#define SW_STATE_TEMP_MNGR_DUMP_INFO_GET(module_id, algo_temp, core_id, info)\
        SHR_IF_ERR_EXIT(sw_state_algo_template_dump_info_get(unit, core_id, module_id, algo_temp, info))

#define SW_STATE_TEMP_MNGR_PRINT(unit, module_id, algo_temp, print_cb)\
        SHR_IF_ERR_EXIT(sw_state_algo_template_print(unit, module_id, *algo_temp, print_cb))

/*
 * Print cb macros
 */

#define DNX_ALGO_TEMPLATE_PRINT_CB_INIT_VARIABLES(_print_cb_data)\
    dnx_algo_template_print_t *_internal_print_cb_data = _print_cb_data;\
    int _line_index = 0;\
    sal_memset(_internal_print_cb_data, 0, sizeof(dnx_algo_template_print_t) * DNX_ALGO_TEMPLATE_PRINT_CB_MAX_SIZE);

#define DNX_ALGO_TEMPLATE_PRINT_CB_ADD_ARR(variable_type, variable_name, variable_data, variable_comment, variable_format, arr_elements_count)\
    for(int _arr_index = 0; _arr_index < arr_elements_count; _arr_index++)\
    {\
        DNX_ALGO_TEMPLATE_PRINT_CB_ADD_ARR_SINGLE_LINE(variable_type, variable_name, variable_data[_arr_index], variable_comment, variable_format, _arr_index)\
    }

#define DNX_ALGO_TEMPLATE_PRINT_CB_ADD_ARR_SINGLE_LINE(variable_type, variable_name, variable_data, variable_comment, variable_format, array_index)\
    _internal_print_cb_data[_line_index].is_arr_flag = TRUE;\
    _internal_print_cb_data[_line_index].arr_index = _arr_index;\
    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(variable_type, variable_name, variable_data, variable_comment, variable_format);

#define DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(variable_type, variable_name, variable_data, variable_comment, variable_format)\
    _internal_print_cb_data[_line_index].type = variable_type;\
    _internal_print_cb_data[_line_index].name = variable_name;\
    _internal_print_cb_data[_line_index].comment = variable_comment;\
    _internal_print_cb_data[_line_index].format_string = variable_format;\
    DNX_ALGO_TEMPLATE_PRINT_CB_MEMCPY_DATA(&variable_data, _line_index);\
    _line_index++;\
    _internal_print_cb_data[_line_index].sub_struct_level = _internal_print_cb_data[_line_index - 1].sub_struct_level;

#define DNX_ALGO_TEMPLATE_PRINT_CB_FINISH(_print_cb_data)\
    _internal_print_cb_data[_line_index].type = TEMPLATE_MNGR_PRINT_TYPE_COUNT;\
    _line_index = 0;

#define DNX_ALGO_TEMPLATE_PRINT_CB_SUB_STRUCT_START()\
    _internal_print_cb_data[_line_index].sub_struct_level += 1;

#define DNX_ALGO_TEMPLATE_PRINT_CB_SUB_STRUCT_END()\
    if(_internal_print_cb_data[_line_index].sub_struct_level > 0)\
    {\
        _internal_print_cb_data[_line_index].sub_struct_level -= 1;\
    }

#define DNX_ALGO_TEMPLATE_PRINT_CB_MEMCPY_DATA(variable_data, line_index)\
    dnx_algo_template_print_cb_add_line_data(variable_data, &_internal_print_cb_data[line_index])

/**
 * Flags for dnx_algo_template_create_data_t struct 
 *  
 * \see 
 * sw_state_algo_template_create
 */
/**
 * Duplicate this template per core.
 */
#define SW_STATE_ALGO_TEMPLATE_CREATE_DUPLICATE_PER_CORE     SAL_BIT(0)
/**
 * This template uses an advanced algorithm.
 */
#define SW_STATE_ALGO_TEMPLATE_CREATE_USE_ADVANCED_ALGORITHM SAL_BIT(1)
/** 
 * This template uses a default profile to which all entries are allocated on creation.
 * This profile will then never be freed, unless the flag
 * \ref SW_STATE_ALGO_TEMPLATE_CREATE_ALLOW_DEFAULT_PROFILE_OVERRIDE is also set at creation.
 */
#define SW_STATE_ALGO_TEMPLATE_CREATE_USE_DEFAULT_PROFILE    SAL_BIT(2)
/**
 *  Set this flag to allow freeing the default profile. Default profile will never be freed if this flag is not set.
 */
#define SW_STATE_ALGO_TEMPLATE_CREATE_ALLOW_DEFAULT_PROFILE_OVERRIDE    SAL_BIT(3)
/**
 * Flags for sw_state_algo_template_allocate and sw_state_algo_template_exchange
 *  
 * \see 
 * sw_state_algo_template_allocate
 * sw_state_algo_template_exchange
 */
/**
 * Allocate with a given ID.
 */
#define SW_STATE_ALGO_TEMPLATE_ALLOCATE_WITH_ID                          SAL_BIT(0)
/**
 * On exchange operation, ignore input data. 
 *   This is used for setting the profile with the WITH_ID flag, without reading the data first.
 *   Therefore, it must be used WITH_ID.
 * \see sw_state_algo_template_exchange
 */
#define SW_STATE_ALGO_TEMPLATE_EXCHANGE_IGNORE_DATA                      SAL_BIT(1)
/**
 * On exchange operation (see below), if the old template does not exist then an error message will not returned. 
 * \see sw_state_algo_template_exchange
 */
#define SW_STATE_ALGO_TEMPLATE_EXCHANGE_IGNORE_NOT_EXIST_OLD_PROFILE    SAL_BIT(2)
/**
 *  Run exchange operation to see if it works but don't change sw state.
 *  \see sw_state_algo_template_exchange
 */
#define SW_STATE_ALGO_TEMPLATE_EXCHANGE_TEST                             SAL_BIT(3)
/**
 *  This flag moves the data and all references from old_profile to provided new_profile.
 *  If this flag is set, then \ref SW_STATE_ALGO_TEMPLATE_EXCHANGE_IGNORE_DATA and SW_STATE_ALGO_TEMPLATE_ALLOCATE_WITH_ID must also be set.
 *  That's because this API is not meant to replace the existing data in the entry, and the new profile must be known,
 *  or else the exchange is irrelevant.
 *  \see sw_state_algo_template_exchange
 */
#define SW_STATE_ALGO_TEMPLATE_EXCHANGE_ALL                             SAL_BIT(4)
/**
 * The maximum amount of references will be allocated.
 */
#define SW_STATE_ALGO_TEMPLATE_ALLOCATE_MAXIMUM_REFERENCES                          SAL_BIT(5)
/**
 * This define will be return from sw_state_algo_template_get_next if we can't provide the next profile.
 * \see sw_state_algo_template_get_next.
 */
#define SW_STATE_ALGO_TEMPLATE_ILLEGAL_PROFILE (-1)

/**
 * Maximum length of a template's name
 */
#define SW_STATE_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH 100

/**
 * \brief Template creation information
 *
 * This structure contains the information required for creating a new template.
 *
 *  \see
 * sw_state_algo_template_create
 */
typedef struct
{
    /*
     *
     * SW_STATE_ALGO_TEMPLATE_CREATE_* flags
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
     * SW_STATE_ALGO_TEMPLATE_CREATE_USE_DEFAULT_PROFILE is set.
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
     * Enum of the relevant advanced algorithm. If we did not have any we should put SW_STATE_ALGO_TEMPLATE_ADVANCED_ALGORITHM_BASIC.
     */
    dnx_algo_template_advanced_algorithms_e advanced_algorithm;
    /*
     * Template name.
     */
    char name[SW_STATE_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH];
} sw_state_algo_template_create_data_t;

/**
 * This structure contains the information required for managing and verifying template use.
 * This data is taken from the sw_state_algo_template_create call.
 */
typedef struct
{
    /*
     * Template name.
     */
    char name[SW_STATE_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH];
    /**
     * Flags used to create this template.
     */
    uint32 flags;
    /**
     * First profile id of the template.
     */
    int first_profile;
    /**
     * How many profiles are in the template.
     */
    int nof_profiles;
    /**
     * Maximum number of pointers to each profile.
     */
    int max_references;
    /**
     * Default profile for the template. To be used if flag SW_STATE_ALGO_TEMPLATE_CREATE_USE_DEFAULT_PROFILE is set.
     */
    int default_profile;
    /**
     * Size of the template's data.
     */
    int data_size;
    /**
     * Advanced algorithm enum.
     */
    dnx_algo_template_advanced_algorithms_e advanced_algorithm;
    /**
     * If flag SW_STATE_ALGO_TEMPLATE_CREATE_USE_DEFAULT_PROFILE is set, put here the data that it will contain.
     */
    DNX_SW_STATE_BUFF *default_data;
    /*
     * Multi set templates.
     */
    sw_state_multi_set_t *multi_set_template;
}  *sw_state_algo_template_t;

/**
 * \brief Template information
 *
 * This structure contains the information that will be displayed with the dump command.
 *
 *  \see
 * sw_state_algo_template_dump_info_get
 */
typedef struct
{
    /*
     *This structure contains the information required for creating a new template.
     */
    sw_state_algo_template_create_data_t create_data;
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
} sw_state_algo_template_dump_data_t;

/**
* \brief
*   Create a new instance of template manager.
* 
*
*    \param [in] unit - Relevant unit.
*    \param [in] module_id - Module ID.
*    \param [in] algo_temp - Template to be created.
*    \param [in] data - Pointed memory contains setup parameters required for the
*      creation of the template.
*    \param [in] extra_arguments - Pointer for extra arguments to be passed to the create function of an advanced algorithm.
*      Leave NULL if data->flags doesn't contain \ref SW_STATE_ALGO_TEMPLATE_CREATE_USE_ADVANCED_ALGORITHM
*    \param [in] nof_members - Number of template members. Specified in the relevant sw state xml file.
*    \param [in] alloc_flags - SW state allocation flags.
*       This flags value is determend by the variable type (dnx data for example) of the additional variables specified in the xml
*   \return
*    \retval Zero if no error was detected
*    \retval Negative if error was detected.
*  \remark
*    None
*  \see
*    sw_state_algo_template_create_data_t
*    dnx_algo_template_advanced_alogrithm_cb_t
*    shr_error_e
*****************************************************/
shr_error_e sw_state_algo_template_create(
    int unit,
    uint32 module_id,
    sw_state_algo_template_t * algo_temp,
    sw_state_algo_template_create_data_t * data,
    void *extra_arguments,
    uint32 nof_members,
    uint32 alloc_flags);

/**
* \brief
*   Destroy an instance of template manager.
* 
*
*    \param [in] unit - Relevant unit.
*    \param [in] module_id - Module ID.
*    \param [in] algo_temp - Template to be destroyed.
*    \param [in] extra_arguments - Pointed memory to hold extra arguemnts required for destroying this template. \n
*       The content of this pointer depends on the algorithm. It's the algorithm's \n
*       responsibility to verify it.
*       If the algorithm doesn't require extra arguemnts, this can be set to NULL.
*   \return
*    \retval Zero if no error was detected
*    \retval Negative if error was detected.
*  \remark
*    None
*  \see
*    dnx_algo_template_destroy_cb
*    dnx_algo_template_advanced_alogrithm_cb_t
*    sw_state_algo_template_create
*    shr_error_e
*/
shr_error_e sw_state_algo_template_destroy(
    int unit,
    uint32 module_id,
    sw_state_algo_template_t * algo_temp,
    void *extra_arguments);

/**
* \brief
*   Allocate a reference to profile_data. If this is the first reference, the first_reference
*     indication will be true. 
* 
*
*    \param [in] unit - Relevant unit.
*    \param [in] core_id - Relevant core. If template is per core (created with SW_STATE_ALGO_TEMPLATE_CREATE_DUPLICATE_PER_CORE flag),
*         it must be a valid core id, otherwise, must be _SHR_CORE_ALL.
*    \param [in] module_id - Module ID.
*    \param [in] algo_temp - Template instance.
*    \param [in] flags - SW_STATE_ALGO_TEMPLATE_ALLOCATE_* flags.
*    \param [in] profile_data - Pointer to memory holding template data to be saved to a profile.
*        Fill with the data to be saved.
*    \param [in] extra_arguments - Pointer to memory holding extra arguments to be passed to the allocation function of an advanced algorithm.
*      If no advanced algorithm is used, use NULL.
*    \param [in] profile - Pointer to place the allocated profile.
*      \b As \b output - \n
*        Holds the allocated profile.
*      \b As \b input - \n
*        If flag \ref SW_STATE_ALGO_TEMPLATE_ALLOCATE_WITH_ID is set, pass the profile to be allocated here.
*    \param [in] first_reference
*      Pointer to memory to load output into. \n
*      \b As \b output - \n
*        Will be filled with boolean indication of the profile's prior existence. \n
*   \return
*    \retval Zero if no error was detected
*    \retval Negative if error was detected.
*  \remark
*      If \ref SW_STATE_ALGO_TEMPLATE_ALLOCATE_WITH_ID (WITH_ID) is set and the specified
*      profile is not yet allocated, this will allocate it and set its data to the
*      specified data.
*
*      If WITH_ID is set and the specified profile is allocated, this will first
*      verify that the new data is 'equal' to the existing data for the profile,
*      and: if so, will increment the reference count for the specified profile;
*      if not, will return _SHR_E_EXISTS.
*
*      If WITH_ID is clear, this will look for a in-use profile whose data are
*      'equal' to the specified data.  If it finds such a profile, it will
*      increment that profile's reference count and return it.  If it does not
*      find such a profile, it marks an available profile as in use, sets its
*      reference count to 1, copies the data to the profile's data, and returns
*      this profile.  If there is no 'equal' or free profile, _SHR_E_RESOURCE.
*
*      Whether WITH_ID is provided or not, on success the int pointed to by the
*      is_allocated argument will be updated.  It will be set TRUE if the profile
*      was free before, and FALSE if the profile was already in use.  If the
*      value is TRUE, appropriate resources should be updated by the caller (such
*      as programming the data values to hardware registers).
*  
*  \see
*    sw_state_algo_template_create_data_t
*    dnx_algo_template_advanced_alogrithm_cb_t
*    shr_error_e
*****************************************************/
shr_error_e sw_state_algo_template_allocate(
    int unit,
    int core_id,
    uint32 module_id,
    sw_state_algo_template_t algo_temp,
    uint32 flags,
    void *profile_data,
    void *extra_arguments,
    int *profile,
    uint8 *first_reference);

/**
* \brief
*   This function performs several replace operations:
*     1. Free one reference from old_profile, and add one to new_profile.
*     2. Change the data in new_profile. This is used by using the WITH_ID flag
*        without the IGNORE_DATA flag.
* 
*
*    \param [in] unit - Relevant unit.
*    \param [in] core_id - Relevant core. If template is per core (created with SW_STATE_ALGO_TEMPLATE_CREATE_DUPLICATE_PER_CORE flag),
*         it must be a valid core id, otherwise, must be _SHR_CORE_ALL.
*    \param [in] module_id - Module ID.
*    \param [in] algo_temp - Template instance.
*    \param [in] flags -
*      SW_STATE_ALGO_TEMPLATE_ALLOCATE_* or SW_STATE_ALGO_TEMPLATE_EXCHANGE_* flags. See remarks below.
*    \param [in] profile_data -
*      Pointer to memory holding template data to be saved to a profile. Not in use if IGNORE_DATA flag is set.
*      \b As \b input - \n
*        Fill with the data to be saved.
*    \param [in] old_profile -
*      One reference will be freed from this profile.
*    \param [in] extra_arguments -
*      Pointer to memory holding extra arguments to be passed to the allocation function of an advanced algorithm.
*      If no advanced algorithm is used, use NULL.
*    \param [in] new_profile -
*      Pointer to place the allocated profile.
*      \b As \b output - \n
*        Holds the allocated profile.
*      \b As \b input - \n
*        If flag \ref SW_STATE_ALGO_TEMPLATE_ALLOCATE_WITH_ID is set, pass the profile to be allocated here.
*    \param [in] first_reference
*      Pointer to memory to load output into. \n
*      \b As \b output - \n
*        Will be filled with boolean indication of the new profile's prior existence. \n
*    \param [in] last_reference -
*      Pointer to memory to load output into. \n
*      \b As \b output - \n
*        Will be filled with boolean indication of whether it's the last reference to old_profile.
*   \return
*    \retval Zero if no error was detected
*    \retval Negative if error was detected.
*  \remark
*
*      Whether SW_STATE_ALGO_TEMPLATE_ALLOCATE_WITH_ID is provided or not, if the allocation / update was successful,
*      the profile with ID provided in old_profile will be freed.  Note this only
*      happens if the selection of a new profile succeeds, so the old profile
*      should not be cleared if this function fails.
*      If old_profile wasn't allocated before, and the flag
*      SW_STATE_ALGO_TEMPLATE_EXCHANGE_IGNORE_NOT_EXIST_OLD_TEMPLATE is not set,
*      then _SHR_E_PARAM will be returned. If the flag is set, then a new refrence 
*      will be allocated without deleting the old one.
* 
*      If SW_STATE_ALGO_TEMPLATE_ALLOCATE_WITH_ID is set and the profile specified in *new_profile is not yet allocated,
*      this will allocate it and set its data to the specified data. 
*
*      If SW_STATE_ALGO_TEMPLATE_ALLOCATE_WITH_ID and SW_STATE_ALGO_TEMPLATE_EXCHANGE_IGNORE_DATA are both set,
*      this will increment the reference count for the specified profile. This combination is used
*      to skip reading the data of the new profile before 
*      If the profile does not exist then _SHR_E_NOT_FOUND is returned.
* 
*      If SW_STATE_ALGO_TEMPLATE_ALLOCATE_WITH_ID is clear, this procedure will look for a in-use profile whose data is
*      identical to profile_data.  If it finds such a profile, it will
*      increment that profile's reference count and return it. If it does not
*      find such a profile, it will allocate a new free profile.
*      If there is no 'equal' or free profile, _SHR_E_RESOURCE.
*
*
*      Whether WITH_ID is provided or not, on success the int pointed to by the
*      first_reference argument will be updated.  It will be set TRUE if the new
*      profile was free before, and FALSE if the new profile was already in use.
*      If the value is TRUE, appropriate resources should be updated by the caller
*      (such as programming the data values to hardware registers).
*
*      It is not valid to specify IGNORE_DATA without WITH_ID.
*  \see
*    sw_state_algo_template_allocate
*    sw_state_algo_template_free
*    shr_error_e
*****************************************************/
shr_error_e sw_state_algo_template_exchange(
    int unit,
    int core_id,
    uint32 module_id,
    sw_state_algo_template_t algo_temp,
    uint32 flags,
    const void *profile_data,
    int old_profile,
    const void *extra_arguments,
    int *new_profile,
    uint8 *first_reference,
    uint8 *last_reference);

/**
* \brief
*   Template manager API, which can replace a data pointer with new data.
*
*
*   \param [in] unit - Relevant unit.
*   \param [in] core_id - Relevant core. If resource is per core, it must be a valid core id, otherwise, must be _SHR_CORE_ALL.
*   \param [in] module_id - Module ID.
*   \param [in] algo_temp - Template instance.
*   \param [in] profile - The data for this profile will be replaced with the provided new data.
*   \param [in] new_profile_data - Pointer to memory holding template data to be saved to a profile. Not in use if IGNORE_DATA flag is set.
*   \return
*       \retval Zero if no error was detected
*       \retval Negative if error was detected.
*   \remark
*       None
*   \see
*    sw_state_algo_template_allocate
*    sw_state_algo_template_free
*    shr_error_e
*****************************************************/

shr_error_e sw_state_algo_template_replace_data(
    int unit,
    int core_id,
    uint32 module_id,
    sw_state_algo_template_t algo_temp,
    int profile,
    const void *new_profile_data);

/**
* \brief
*   Get the data pointed by a template's profile.
*
*    \param [in] unit - Relevant unit.
*    \param [in] core_id - Relevant core. If template is per core (created with SW_STATE_ALGO_TEMPLATE_CREATE_DUPLICATE_PER_CORE flag),
*        it must be a valid core id, otherwise, must be _SHR_CORE_ALL.
*    \param [in] module_id - Module ID.
*    \param [in] algo_temp - Template instance.
*    \param [in] profile - Profile to get the data from.
*    \param [in] ref_count - Pointer to memory to load output into. \n
*      \b As \b output - \n
*        Will be filled with number of refernces to this profile.
*        This can be set to NULL, if the user doesn't require ref_count as an output, but then 
*          profile_data must not be NULL.
*    \param [in] profile_data -
*      Pointer to memory to load output into. \n
*      \b As \b output - \n
*        Will be filled with data stored in this profile.
*        This can be set to NULL, if the user doesn't require profile_data as an output, but then 
*          ref_count must not be NULL.
*   \return
*    \retval Zero if no error was detected
*    \retval Negative if error was detected.
*  \remark
*    None
*  \see
*    sw_state_algo_template_create_data_t
*    dnx_algo_template_advanced_alogrithm_cb_t
*    shr_error_e
*****************************************************/
shr_error_e sw_state_algo_template_profile_data_get(
    int unit,
    int core_id,
    uint32 module_id,
    sw_state_algo_template_t algo_temp,
    int profile,
    int *ref_count,
    void *profile_data);

/**
* \brief
*   Get a template's profile by given data.
*
*    \param [in] unit - Relevant unit.
*    \param [in] core_id - Relevant core. If template is per core (created with SW_STATE_ALGO_TEMPLATE_CREATE_DUPLICATE_PER_CORE flag),
*         it must be a valid core id, otherwise, must be _SHR_CORE_ALL.
*    \param [in] module_id - Module ID.
*    \param [in] algo_temp - Template instance.
*    \param [in] profile_data -
*      Pointer to memory for data to lookup. \n
*      \b As \b input - \n
*        Fill this with the data to find.
*    \param [in] profile -
*      Pointer to place the allocated profile.
*      \b As \b output - \n
*        Holds the profile that holds given data.
*   \return
*    \retval Zero if no error was detected
*    \retval Negative if error was detected.
*  \remark
*    None
*  \see
*    sw_state_algo_template_create_data_t
*    dnx_algo_template_advanced_alogrithm_cb_t
*    shr_error_e
*****************************************************/
shr_error_e sw_state_algo_template_profile_get(
    int unit,
    int core_id,
    uint32 module_id,
    sw_state_algo_template_t algo_temp,
    const void *profile_data,
    int *profile);

/**
* \brief
*   Free a reference to a template profile.
* 
*
*    \param [in] unit - Relevant unit.
*    \param [in] core_id - Relevant core. If template is per core (created with SW_STATE_ALGO_TEMPLATE_CREATE_DUPLICATE_PER_CORE flag),
*         it must be a valid core id, otherwise, must be _SHR_CORE_ALL.
*    \param [in] module_id - Module ID.
*    \param [in] algo_temp - Template instance.
*    \param [in] profile - Profile to be freed.
*    \param [in] last_reference - Pointer to memory to load output into. \n
*      \b As \b output - \n
*        Will be filled with boolean indication of whether it's the last reference to this profile.
*   \return
*    \retval Zero if no error was detected
*    \retval Negative if error was detected.
*  \remark
*    None
*  \see
*    sw_state_algo_template_create_data_t
*    dnx_algo_template_advanced_alogrithm_cb_t
*    shr_error_e
*****************************************************/
shr_error_e sw_state_algo_template_free(
    int unit,
    int core_id,
    uint32 module_id,
    sw_state_algo_template_t algo_temp,
    int profile,
    uint8 *last_reference);

/**
* \brief
*   Free all reference to all profiles of this template. 
*
*    \param [in] unit - Relevant unit.
*    \param [in] core_id - Relevant core. If template is per core (created with SW_STATE_ALGO_TEMPLATE_CREATE_DUPLICATE_PER_CORE flag),
*         it must be a valid core id, otherwise, must be _SHR_CORE_ALL.
*    \param [in] module_id - Module ID.
*    \param [in] algo_temp - Template instance.
*   \return
*     \retval Zero if no error was detected
*     \retval Negative if error was detected.
*  \remark
*    None
*  \see
*    sw_state_algo_template_create_data_t
*    dnx_algo_template_advanced_alogrithm_cb_t
*    shr_error_e
*****************************************************/
shr_error_e sw_state_algo_template_clear(
    int unit,
    int core_id,
    uint32 module_id,
    sw_state_algo_template_t algo_temp);

/**
* \brief
*   Free all references to a profile.
*
*
*   \param [in] unit - Relevant unit.
*   \param [in] core_id - Relevant core. If resource is per core, it must be a valid core id, otherwise, must be _SHR_CORE_ALL.
*   \param [in] module_id - Module ID.
*   \param [in] algo_temp - Template instance.
*   \param [in] profile - All references, and the data, for this profile will be freed.
*   \return
*       \retval Zero if no error was detected
*       \retval Negative if error was detected.
*   \remark
*       None
*   \see
*    sw_state_algo_template_free
*****************************************************/
shr_error_e sw_state_algo_template_free_all(
    int unit,
    int core_id,
    uint32 module_id,
    sw_state_algo_template_t algo_temp,
    int profile);

/**
 * \brief
 *      Given a template name, core and profile, returns the next allocated profile
 *      for this template on this core.
 *      The returned profile will be equal to or greater than the given profile. It is the user's
 *      responsibility to update it between iterations.
 *      When there are no more allocated profiles, SW_STATE_ALGO_TEMPLATE_ILLEGAL_PROFILE will be returned.
 *
 *  \param [in] unit - Relevant unit.
 *  \param [in] core_id - Relevant core. If template is per core (created with SW_STATE_ALGO_TEMPLATE_CREATE_DUPLICATE_PER_CORE flag),
 *      it must be a valid core id, otherwise, must be _SHR_CORE_ALL.
 *  \param [in] module_id - Module ID.
 *  \param [in] algo_temp - Template instance.
 *  \param [in,out] current_profile -
 *       \b As \b input - \n
 *       The profile to start searching for the next allocated profile from.
 *       \b As \b output - \n
 *       he next allocated profile. It will be >= from the profile given as input.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *      It is the user's responsibility to update the current_profile between iterations. This usually means
 *       giving it a ++.
 * \see
 *      None
 *****************************************************/
shr_error_e sw_state_algo_template_get_next(
    int unit,
    int core_id,
    uint32 module_id,
    sw_state_algo_template_t algo_temp,
    int *current_profile);

/**
* \brief
*   Returns data for the relevant template.
*
*
*   \param [in] unit - Relevant unit.
*   \param [in] core_id - Relevant core. If template is per core (created with SW_STATE_ALGO_TEMPLATE_CREATE_DUPLICATE_PER_CORE flag),
*      it must be a valid core id, otherwise, must be _SHR_CORE_ALL.
*   \param [in] module_id - Module ID.
*   \param [in] algo_temp - Template instance.
*   \param [out] info -
*      Pointed memory to save the template info See \ref sw_state_algo_template_dump_data_t
*  \return
*    \retval Zero if no error was detected
*    \retval Negative if error was detected. See \ref shr_error_e
*  \remark
*    None
*  \see
*    dnx_algo_res_dump_data_t
*    shr_error_e
*****************************************************/
shr_error_e sw_state_algo_template_dump_info_get(
    int unit,
    int core_id,
    uint32 module_id,
    sw_state_algo_template_t algo_temp,
    sw_state_algo_template_dump_data_t * info);

/**
* \brief
*   Prints template data
*
*   \param [in] unit -  Relevant unit.
*   \param [in] module_id - Module ID.
*   \param [in] algo_temp - template.
*   \param [in] print_cb - Template print cb.
*  \return
*    \retval Zero if no error was detected
*    \retval Negative if error was detected. See \ref shr_error_e
*  \remark
*    None
*  \see
*    None
*****************************************************/

int sw_state_algo_template_print(
    int unit,
    uint32 module_id,
    sw_state_algo_template_t algo_temp,
    dnx_algo_template_print_data_cb print_cb);

/**
* \brief
*   Saves relevant data in the print cb struct.
*
*   \param [in] data -
*       Pointer to the data we need to save
*   \param [in] print_cb_data -
*      Pointer to the print cb data structure to save the data.
*  \return
*    None
*  \remark
*    None
*  \see
*    None
*****************************************************/
void dnx_algo_template_print_cb_add_line_data(
    void *data,
    dnx_algo_template_print_t * print_cb_data);

/*
 * } 
 */
#endif /* SW_STATE_ALGO_TEMPLATE_MNGR_INCLUDED */
