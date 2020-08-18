/** \file sw_state_res_mngr.h
 *
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef __SW_STATE_RES_MNGR_H__
#define __SW_STATE_RES_MNGR_H__

/* must be first */
#include <soc/dnxc/swstate/sw_state_features.h>


#include <soc/dnxc/swstate/sw_state_defs.h>
#include <soc/dnxc/swstate/sw_state_attribute_packed.h>
#include <soc/dnxc/swstate/types/sw_state_linked_list.h>
#include <soc/dnxc/swstate/types/sw_state_hash_table.h>
#include <soc/dnxc/swstate/types/sw_state_multi_set.h>
#include <soc/dnxc/swstate/types/sw_state_defragmented_chunk.h>
#include <soc/dnxc/swstate/types/sw_state_occupation_bitmap.h>
#include <soc/dnxc/swstate/dnx_sw_state_logging.h>
#include <soc/dnxc/swstate/sw_state_diagnostics.h>
#include <soc/dnxc/swstate/types/sw_state_resource_tag_bitmap.h>
#include <include/bcm_int/dnx/algo/res_mngr/res_mngr_types.h>
#include <soc/dnxc/swstate/dnxc_sw_state_plain.h>

/*
 * MACROs
 */

#define SW_STATE_ALGO_RES_GET_POOL_CORE(_core_id) ((_core_id == _SHR_CORE_ALL) ? 0 : _core_id)

#define SW_STATE_ALGO_RES_NOF_CORES_GET(unit) dnx_data_device.general.nof_cores_get(unit)

#define SW_STATE_ALGO_RES_ALLOCATE_USING_SINGLE_ALLOCATE_CB   (SAL_BIT(15))

#define SW_STATE_RES_MNGR_CREATE(module_id, algo_res, data, extra_arguments, nof_elements, alloc_flags)\
        SHR_IF_ERR_EXIT(sw_state_algo_res_create(unit, module_id, &algo_res, data, extra_arguments, nof_elements, alloc_flags))

#define SW_STATE_RES_MNGR_DESTROY(module_id, algo_res, extra_arguments)\
        SHR_IF_ERR_EXIT(sw_state_algo_res_destroy(unit, module_id, &algo_res, extra_arguments))

#define SW_STATE_RES_MNGR_ALLOCATE_SINGLE(module_id, algo_res, flags, extra_arguments, element)\
    do{\
        if(_SHR_IS_FLAG_SET(flags, SW_STATE_ALGO_RES_ALLOCATE_SIMULATION))\
        {\
            SHR_IF_ERR_EXIT_NO_MSG(sw_state_algo_res_allocate(unit, module_id, algo_res, flags, extra_arguments, element));\
        }\
        else\
        {\
            SHR_IF_ERR_EXIT(sw_state_algo_res_allocate(unit, module_id, algo_res, flags, extra_arguments, element));\
        }\
    }while(0)

#define SW_STATE_RES_MNGR_ALLOCATE_SEVERAL(module_id, algo_res, flags, nof_elements, extra_arguments, element)\
    do{\
        if(_SHR_IS_FLAG_SET(flags, SW_STATE_ALGO_RES_ALLOCATE_SIMULATION))\
        {\
            SHR_IF_ERR_EXIT_NO_MSG(sw_state_algo_res_allocate_several(unit, module_id, algo_res, flags, nof_elements, extra_arguments, element));\
        }\
        else\
        {\
            SHR_IF_ERR_EXIT(sw_state_algo_res_allocate_several(unit, module_id, algo_res, flags, nof_elements, extra_arguments, element));\
        }\
    }while(0)

#define SW_STATE_RES_MNGR_IS_ALLOCATED(module_id, algo_res, element, is_allocated)\
        SHR_IF_ERR_EXIT(sw_state_algo_res_is_allocated(unit, module_id, algo_res, element, is_allocated))

#define SW_STATE_RES_MNGR_FREE_SINGLE(module_id, algo_res, element)\
        SHR_IF_ERR_EXIT(sw_state_algo_res_free(unit, module_id, algo_res, element))

#define SW_STATE_RES_MNGR_FREE_SEVERAL(module_id, algo_res, nof_elements, element)\
        SHR_IF_ERR_EXIT(sw_state_algo_res_free_several(unit, module_id, algo_res, nof_elements, element))

#define SW_STATE_RES_MNGR_CLEAR(module_id, algo_res)\
        SHR_IF_ERR_EXIT(sw_state_algo_res_clear(unit, module_id, algo_res))

#define SW_STATE_RES_MNGR_CLEAR_ALL(module_id, algo_res)\
        SHR_IF_ERR_EXIT(sw_state_algo_res_clear_all(unit, module_id, algo_res))

#define SW_STATE_RES_MNGR_NOF_FREE_ELEMENTS_GET(module_id, algo_res, nof_free_elements)\
        SHR_IF_ERR_EXIT(sw_state_algo_res_nof_free_elements_get(unit, module_id, algo_res, nof_free_elements))

#define SW_STATE_RES_MNGR_GET_NEXT(module_id, algo_res, element)\
        SHR_IF_ERR_EXIT(sw_state_algo_res_get_next(unit, module_id, algo_res, element))

#define SW_STATE_RES_MNGR_DUMP_INFO_GET(module_id, algo_res, info)\
        SHR_IF_ERR_EXIT(sw_state_algo_res_dump_info_get(unit, module_id, algo_res, info))

#define SW_STATE_RES_MNGR_PRINT(unit, module_id, algo_res)\
        SHR_IF_ERR_EXIT(sw_state_algo_res_print(unit, module_id, algo_res))

#define SW_STATE_RES_MNGR_NOF_ALLOCATED_ELEMENTS_IN_RANGE_GET(module_id, algo_res, range_start, nof_elements_in_range, nof_allocated_elements)\
        SHR_IF_ERR_EXIT(sw_state_algo_res_nof_allocated_elements_in_range_get\
                (unit, module_id, algo_res, range_start, nof_elements_in_range, nof_allocated_elements))

#define SW_STATE_RES_MNGR_ADVANCED_ALGORITHM_INFO_SET(module_id, algo_res, algorithm_info)\
        SHR_IF_ERR_EXIT(sw_state_algo_res_advanced_algorithm_info_set\
                (unit, module_id, algo_res, algorithm_info))

#define SW_STATE_RES_MNGR_ADVANCED_ALGORITHM_INFO_GET(module_id, algo_res, algorithm_info)\
        SHR_IF_ERR_EXIT(sw_state_algo_res_advanced_algorithm_info_get\
                (unit, module_id, algo_res, algorithm_info))

/*
 * DEFINES
 */
/*
 * Resource name max lenght.
 */
#define SW_STATE_ALGO_RES_MNGR_MAX_NAME_LENGTH 100

/**
 * Duplicate this resource per core.
 */
#define SW_STATE_ALGO_RES_CREATE_DUPLICATE_PER_CORE      SAL_BIT(0)
/**
 * This resource uses an advanced algorithm.
 */
#define SW_STATE_ALGO_RES_CREATE_USE_ADVANCED_ALGORITHM  SAL_BIT(1)
/**
 * Create a resource that is an array of identical sub resources, differentiated by index.
 */
#define SW_STATE_ALGO_RES_CREATE_INDEXED_RESOURCE  SAL_BIT(2)
/**
 * This define will be used as a input(sub_resource_index) if
 * indexed pool is not used.
 */
#define SW_STATE_ALGO_RES_DEFAULT_SUB_RESOURCE (0)
/**
 * This define will be return from sw_state_algo_res_get_next if we can't provide the next element.
 * \see sw_state_algo_res_get_next
 */
#define SW_STATE_ALGO_RES_ILLEGAL_ELEMENT (-1)
/**
 * Flags for sw_state_algo_res_allocate
 *
 * \see
 * sw_state_algo_res_allocate
 */
/**
 * Allocate with a given ID.
 */
#define SW_STATE_ALGO_RES_ALLOCATE_WITH_ID       SAL_BIT(0)
/**
 * Verify before allocation if the allocation will succeed.
 */
#define SW_STATE_ALGO_RES_ALLOCATE_SIMULATION    SAL_BIT(1)
/**
 * Each advanced algorithm can add his flags in this range.
 */
#define SW_STATE_ALGO_RES_ADVANCED_ALGOIRTHM_FLAGS  UTILEX_BITS_MASK(25,30)

/*
 * STRUCTs
 */

/**
 * This structure contains the information required for managing and verifying resource use. This data is taken from the sw_state_algo_res_create call.
 */
typedef struct
{
    /**
     * Flags used to create this resource.
     */
    uint32 flags;
    /**
     * First element of the resource.
     */
    int first_element;
    /**
     * How many elements are in the resource.
     */
    int nof_elements;
    /**
     * How many pool indexes are in the resource.
     */
    int nof_resource_pool_indexes;
    /**
     * Advanced algorithm enum.
     */
    dnx_algo_resource_advanced_algorithms_e advanced_algorithm;
    /**
     * Resource name
     */
    char name[SW_STATE_ALGO_RES_MNGR_MAX_NAME_LENGTH];
    /*
     * Res tag bitmaps used by the resource.
     */
    sw_state_resource_tag_bitmap_t res_tag_bitmap;
}  *sw_state_algo_res_t;

/**
 * \brief Resource creation information
 *
 * This structure contains the information required for creating a new resource.
 *
 */
typedef struct
{
    /*
     * *
     * * SW_STATE_ALGO_RES_INIT_* flags
     */
    uint32 flags;
    /*
     * *
     * * First element of the resource
     */
    int first_element;
    /*
     * *
     * * How many elements are in the resource.
     */
    int nof_elements;
    /*
     * *
     * *How many pool indexes are in the resource.
     * *Will be ignored if indexed pool is not used.
     */
    int nof_resource_pool_indexes;
    /*
     * Enum of the relevant advanced algorithm. If we did not have any we should put SW_STATE_ALGO_RESOURCE_ADVANCED_ALGORITHM_BASIC.
     */
    dnx_algo_resource_advanced_algorithms_e advanced_algorithm;
    /**
     * Resource name
     */
    char name[SW_STATE_ALGO_RES_MNGR_MAX_NAME_LENGTH];
    /**
     * Relevant core id. It is not mandatory if the resource is not created with _DUPLICATE_PER_CORE flag.
     */
    uint32 core_id;
    /**
     * Relevant sub resource id. It is not mandatory if the resource is not created with _INDEXED_RESOURCE flag.
     */
    uint32 sub_resource_index;
} sw_state_algo_res_create_data_t;

/**
 * \brief Resource information
 *
 * This structure contains the information that will be displayed with the swstate dump command.
 *
 */
typedef struct
{
    /*
     *This structure contains the information required for creating a new resource.
     */
    sw_state_algo_res_create_data_t create_data;
    /*
     * *
     * *How many elements are used in the resource.
     */
    int nof_used_elements;
    /*
     * Enum of the relevant advanced algorithm. If we did not have any we should put DNX_ALGO_RESOURCE_ADVANCED_ALGORITHM_BASIC.
     */
    dnx_algo_resource_advanced_algorithms_e advanced_algorithm;
    /*
     * algo_instance_id.
     */
    int algo_instance_id;
} sw_state_algo_res_dump_data_t;

/**
* \brief
*   Create a new instance of resource manager.
*
*  \param [in] unit -
*    Relevant unit.
*  \param [in] module_id - Module ID.
*  \param [in] algo_res - Resource to be created.
*  \param [in] data -
*    Pointed memory contains setup parameters required for the
*    creation of the resouce. See \ref sw_state_algo_res_create_data_t
*  \param [in] extra_arguments
*    Pointer for extra arguments to be passed to the create function of an advanced algorithm.
*    Leave NULL if data->flags doesn't contain \ref SW_STATE_ALGO_RES_CREATE_USE_ADVANCED_ALGORITHM
*   The sw state autocoder will put 0 if the resource is not indexed.
*  \param [in] nof_elements - Number of elements of the resource manager. Specified in the relevant sw state xml file.
*  \param [in] alloc_flags - SW state allocation flags.
*       This flags value is determend by the variable type (dnx data for example) of the additional variables specified in the xml
*  \return
*     shr_error_e
*
*  \remark
*    None
*  \see
*    sw_state_algo_res_create_data_t
*    dnx_algo_resource_advanced_algorithms_e
*    shr_error_e
*****************************************************/
shr_error_e sw_state_algo_res_create(
    int unit,
    uint32 module_id,
    sw_state_algo_res_t * algo_res,
    sw_state_algo_res_create_data_t * data,
    void *extra_arguments,
    uint32 nof_elements,
    uint32 alloc_flags);

/**
* \brief
*   Destroy an instance of resouce manager.
*
*  \param [in] unit - Relevant unit.
*  \param [in] module_id - Module ID.
*  \param [in] algo_res - Resource to be destroyed.
*  \param [in] extra_arguments - Pointed memory to hold extra arguemnts required for creating this resource. \n
*     The content of this pointer depends on the algorithm. It's the algorithm's \n
*     responsibility to verify it.
*     If the algorithm doesn't require extra arguemnts, this can be set to NULL.
*
*  /return
*     shr_error_e
*
*  \remark
*    None
*  \see
*    sw_state_algo_res_create
*    shr_error_e
*/
shr_error_e sw_state_algo_res_destroy(
    int unit,
    uint32 module_id,
    sw_state_algo_res_t * algo_res,
    void *extra_arguments);

/**
* \brief
*   Allocate a free element.
*
*  \param [in] unit - Relevant unit.
*  \param [in] module_id - Module ID.
*  \param [in] algo_res - Resource to be allocated into.
*  \param [in] flags - SW_STATE_ALGO_RES_ALLOCATE_* flags. E.g., \ref SW_STATE_ALGO_RES_ALLOCATE_WITH_ID
*  \param [in] extra_arguments - Pointer to memory holding extra arguments to be passed to the allocation function of an advanced algorithm.
*    If no advanced algorithm is used, use NULL.
*  \param [in] element - Pointer to place the allocated element.
*    If flag \ref SW_STATE_ALGO_RES_ALLOCATE_WITH_ID is set, pass the element to be allocated here.
*
*  \return
*    shr_error_e
*
*  \remark
*    None
*  \see
*    sw_state_algo_res_create_data_t
*    sw_state_algo_res_advanced_alogrithm_cb_t
*    shr_error_e
*****************************************************/
shr_error_e sw_state_algo_res_allocate(
    int unit,
    uint32 module_id,
    sw_state_algo_res_t algo_res,
    uint32 flags,
    void *extra_arguments,
    int *element);

/**
* \brief
*   Check whether an element was allocated.
*
*  \param [in] unit - Relevant unit.
*  \param [in] module_id - Module ID.
*  \param [in] algo_res - Resource to be check if it is allocated into.
*  \param [in] element - Element to be checked.
*  \param [in] is_allocated - Pointer to memory to load output into. \n
*      Will be filled with boolean indication of the element's existence.
*
*  \return
*    shr_error_e
*
*  \remark
*    None
*  \see
*    sw_state_algo_res_create_data_t
*    sw_state_algo_res_advanced_alogrithm_cb_t
*    shr_error_e
*****************************************************/
shr_error_e sw_state_algo_res_is_allocated(
    int unit,
    uint32 module_id,
    sw_state_algo_res_t algo_res,
    int element,
    uint8 *is_allocated);

/**
* \brief
*   Free allocated element.
*
*  \param [in] unit - Relevant unit.
*  \param [in] module_id - Module ID.
*  \param [in] algo_res - Resource to free from.
*  \param [in] element - Element to be freed.
*
*  \return
*    shr_error_e
*
*  \remark
*    None
*  \see
*    sw_state_algo_res_create_data_t
*    sw_state_algo_res_advanced_alogrithm_cb_t
*    shr_error_e
*****************************************************/
shr_error_e sw_state_algo_res_free(
    int unit,
    uint32 module_id,
    sw_state_algo_res_t algo_res,
    int element);

/**
 * \brief
 *   Get the number of currently free elements.
 *
 *  \param [in] unit - Relevant unit.
 *  \param [in] module_id - Module ID.
 *  \param [in] algo_res - Resource.
 *  \param [out] nof_free_elements - Int pointer to memory to load output into. \n
 *      This procedure loads the memory with the number of free elements in this resource.
 *  /return
 *    shr_error_e
 *
 *  \remark
 *    None
 *  \see
 *    sw_state_algo_res_create_data_t
 *    sw_state_algo_res_advanced_alogrithm_cb_t
 *    shr_error_e
 *****************************************************/
shr_error_e sw_state_algo_res_nof_free_elements_get(
    int unit,
    uint32 module_id,
    sw_state_algo_res_t algo_res,
    int *nof_free_elements);

/**
 * \brief
 *   Clear all allocated elements in this resource.
 *
 *  \param [in] unit - Relevant unit.
 *  \param [in] module_id - Module ID.
 *  \param [in] algo_res - Resource.
 *
 *  \return
 *    shr_error_e
 *
 *  \remark
 *    None
 *  \see
 *    sw_state_algo_res_create_data_t
 *    sw_state_algo_res_advanced_alogrithm_cb_t
 *    shr_error_e
 *****************************************************/
shr_error_e sw_state_algo_res_clear(
    int unit,
    uint32 module_id,
    sw_state_algo_res_t algo_res);

shr_error_e sw_state_algo_res_clear_all(
    int unit,
    uint32 module_id,
    sw_state_algo_res_t algo_res);
/**
 * \brief
 *      Given a resource name, core, sub resource index and element, returns the next allocated element
 *      for this resource on this core.
 *      The returned element will be equal to or greater than the given element. It is the user's
 *      responsibility to update it between iterations.
 *      When there are no more allocated elements, SW_STATE_ALGO_RES_ILLEGAL_ELEMENT will be returned.
 *
 *  \param [in] unit - Relevant unit.
 *  \param [in] module_id - Module ID.
 *  \param [in] algo_res - Resource.
 *  \param [in,out] element -
 *       \b As \b input - \n
 *       The element to start searching for the next allocated element from.
 *       \b As \b output - \n
 *       he next allocated element. It will be >= from the element given as input.
 *
 *   \return
 *       \retval Zero if no error was detected
 *       \retval Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 *      It is the user's responsibility to update the element between iterations. This usually means
 *       giving it a ++.
 * \see
 *      None
 *****************************************************/
shr_error_e sw_state_algo_res_get_next(
    int unit,
    uint32 module_id,
    sw_state_algo_res_t algo_res,
    int *element);

/**
* \brief
*   Allocation manager API, which can allocate a continues block of several elements.
*
*
*  \param [in] unit - Relevant unit.
*  \param [in] module_id - Module ID.
*  \param [in] algo_res - Resource.
*  \param [in] flags - SW_STATE_ALGO_RES_ALLOCATE_* flags. E.g., \ref SW_STATE_ALGO_RES_ALLOCATE_WITH_ID
*  \param [in] nof_elements - Number of elements used to create this resource.
*  \param [in] extra_arguments - Pointer to memory holding extra arguments to be passed to the allocation function of an advanced algorithm.
*       If no advanced algorithm is used, use NULL. If flag \ref SW_STATE_ALGO_RES_ALLOCATE_WITH_ID is set, pass the element to be allocated here.
*  \param [in] element - Pointer to place the allocated element.
*  \return
*     \retval Zero if no error was detected
*     \retval Negative if error was detected. See \ref shr_error_e
*  \remark
*     None
*  \see
*     sw_state_algo_res_create_data_t
*     sw_state_algo_res_advanced_alogrithm_cb_t
*     shr_error_e
*****************************************************/
shr_error_e sw_state_algo_res_allocate_several(
    int unit,
    uint32 module_id,
    sw_state_algo_res_t algo_res,
    uint32 flags,
    uint32 nof_elements,
    void *extra_arguments,
    int *element);

/**
* \brief
*   Free a continues block of allocated elements.
*
*   \param [in] unit - Relevant unit.
*   \param [in] module_id - Module ID.
*   \param [in] algo_res - Resource.
*   \param [in] element - Element to be freed.
*   \param [in] nof_elements - Number of elements to be freed.
*
*  \return
*    \retval Zero if no error was detected
*    \retval Negative if error was detected. See \ref shr_error_e
*  \remark
*    None
*  \see
*    sw_state_algo_res_create_data_t
*    sw_state_algo_res_advanced_alogrithm_cb_t
*    shr_error_e
*****************************************************/
shr_error_e sw_state_algo_res_free_several(
    int unit,
    uint32 module_id,
    sw_state_algo_res_t algo_res,
    uint32 nof_elements,
    int element);

/**
* \brief
*   Retuns data for the relevant resource.
*
*
*   \param [in] unit -  Relevant unit.
*   \param [in] module_id - Module ID.
*   \param [in] algo_res - Resource.
*   \param [out] info - Pointed memory to save the resource info See \ref sw_state_algo_res_dump_data_t
*  \return
*    \retval Zero if no error was detected
*    \retval Negative if error was detected. See \ref shr_error_e
*  \remark
*    None
*  \see
*    sw_state_algo_res_dump_data_t
*    shr_error_e
*****************************************************/
shr_error_e sw_state_algo_res_dump_info_get(
    int unit,
    uint32 module_id,
    sw_state_algo_res_t algo_res,
    sw_state_algo_res_dump_data_t * info);

/**
* \brief
*   Prints resource data
*
*
*   \param [in] unit -  Relevant unit.
*   \param [in] module_id - Module ID.
*   \param [in] algo_res - Resource.
*  \return
*    \retval Zero if no error was detected
*    \retval Negative if error was detected. See \ref shr_error_e
*  \remark
*    None
*  \see
*    None
*****************************************************/

int sw_state_algo_res_print(
    int unit,
    uint32 module_id,
    sw_state_algo_res_t * algo_res);

/**
* \brief
*   Gets number of used elements in range.
*
*    \param [in] unit - Relevant unit.
*    \param [in] module_id - Module ID.
*    \param [in] algo_res - Resource.
*    \param [in] range_start - Range start.
*    \param [in] nof_elements_in_range - Range size.
*    \param [out] nof_allocated_elements - Pointer to the memory where the return value will be saved.
*  \return
*    shr_error_e - Error return value
*  \remark
*    None
*  \see
*    shr_error_e
*/
shr_error_e sw_state_algo_res_nof_allocated_elements_in_range_get(
    int unit,
    uint32 module_id,
    sw_state_algo_res_t algo_res,
    int range_start,
    int nof_elements_in_range,
    int *nof_allocated_elements);

/**
* \brief
*   Set advanced algorithm info.
*
*    \param [in] unit - Relevant unit.
*    \param [in] module_id - Module ID.
*    \param [in] algo_res - Resource.
*    \param [in] algorithm_info - algorithm info to be set
*  \return
*    shr_error_e - Error return value
*  \remark
*    None
*  \see
*    shr_error_e
*/
shr_error_e sw_state_algo_res_advanced_algorithm_info_set(
    int unit,
    uint32 module_id,
    sw_state_algo_res_t algo_res,
    void *algorithm_info);

/**
* \brief
*   Get advanced algorithm info.
*
*    \param [in] unit - Relevant unit.
*    \param [in] module_id - Module ID.
*    \param [in] algo_res - Resource.
*    \param [out] algorithm_info - Pointer where the algorithm will be save.
*  \return
*    shr_error_e - Error return value
*  \remark
*    None
*  \see
*    shr_error_e
*/
shr_error_e sw_state_algo_res_advanced_algorithm_info_get(
    int unit,
    uint32 module_id,
    sw_state_algo_res_t algo_res,
    void *algorithm_info);

#endif /* __SW_STATE_RES_MNGR_H__ */
