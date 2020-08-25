/*! \file simple_bitmap_wrap.h
 * 
 * Internal DNX resource manager APIs 
 * 
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 */
#ifndef ALGO_SIMPLE_BITMAP_WRAP_INCLUDED
/* { */
#define ALGO_SIMPLE_BITMAP_WRAP_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnxc/swstate/types/sw_state_res_mngr.h>

#define DNX_ALGO_RES_SIMPLE_BITMAP_CREATE_ALLOW_IGNORING_TAG            SAL_BIT(25)
#define DNX_ALGO_RES_SIMPLE_BITMAP_CREATE_NO_OPTIMIZATION_PER_TAG       SAL_BIT(26)
#define DNX_ALGO_RES_SIMPLE_BITMAP_CREATE_GET_NOF_ELEMENTS_PER_GRAIN    SAL_BIT(27)
#define DNX_ALGO_RES_SIMPLE_BITMAP_CREATE_ALLOW_FORCING_TAG             SAL_BIT(28)

#define DNX_ALGO_RES_SIMPLE_BITMAP_ALLOC_ALIGN_ZERO                     SAL_BIT(25)
#define DNX_ALGO_RES_SIMPLE_BITMAP_ALLOC_ALIGN                          SAL_BIT(26)
#define DNX_ALGO_RES_SIMPLE_BITMAP_ALWAYS_CHECK_TAG                     SAL_BIT(27)
#define DNX_ALGO_RES_SIMPLE_BITMAP_ALLOC_IGNORE_TAG                     SAL_BIT(28)
#define DNX_ALGO_RES_SIMPLE_BITMAP_ALLOC_SPARSE                         SAL_BIT(29)
#define DNX_ALGO_RES_SIMPLE_BITMAP_ALLOC_IN_RANGE                       SAL_BIT(30)

/**
 * Includes the information user needs to supply for tag_get/tag_set in res_tag_bitmap.
 * \see
 *      sw_state_resource_tag_bitmap_tag_set
 *      sw_state_resource_tag_bitmap_tag_get
 */
typedef struct
{
    /*
     * Element index.
     */
    int element;
    /*
     * How many elements will be set with this tag.
     */
    int nof_elements;
    /*
     * Tag value to use.
     */
    uint32 tag;
    /*
     * If the bitmap was created with the SW_STATE_RESOURCE_TAG_BITMAP_CREATE_ALLOW_FORCING_TAG flag,
     *                          then set this to TRUE to force the tag, and to FALSE to release it.
     */
    int force_tag;
} simple_bitmap_wrap_tag_info_t;

/**
 * Includes the information user needs to supply for res_tag_bitmap creation.
 * The struct can be passed as extra_arguments and then the dnx_algo_res_simple_bitmap_create will
 *       behave as sw_state_resource_tag_bitmap_create.
 * \see sw_state_resource_tag_bitmap_create
 *      dnx_algo_res_simple_bitmap_create
 */
typedef struct
{
    /*
     * Grain size.
     */
    int grain_size;
    /*
     * Max tag value.
     */
    uint32 max_tag_value;
}
simple_bitmap_wrap_create_info_t;

/**
 * Includes the information user needs to supply for res_tag_bitmap allocation.
 * The struct can be passed as extra_arguments and then the dnx_algo_res_simple_bitmap_allocate will
 *       behave as sw_state_resource_tag_bitmap_alloc.
 * \see sw_state_resource_tag_bitmap_alloc
 *      dnx_algo_res_simple_bitmap_allocate
 */
typedef struct
{
    /*
     * Base alignment. Only relevant if ALLOC_ALIGN flag is set.
     */
    int align;
    /*
     * Offset from base alignment. Each option will be checked.
     */
    int offs[SW_STATE_RESOURCE_TAG_BITMAP_MAX_NOF_OFFSETS];
    /*
     * Number of offsets.
     */
    uint8 nof_offsets;
    /*
     * Tag value to use. Only relevant if bitmap is using tags.
     */
    uint32 tag;
    /*
     * Bitmapped pattern of elements. Relevant only if ALLOC_SPARSE flag is set.
     */
    uint32 pattern;
    /*
     * Length of pattern. Relevant only if ALLOC_SPARSE flag is set.
     */
    int length;
    /*
     * Number of iterations of pattern. Relevant only if ALLOC_SPARSE flag is set.
     */
    int repeats;
    /*
     * The start ID of the range in which allocation without ID will occur if the ALLOC_IN_RANGE flag is added.
     */
    int range_start;
    /*
     * The next ID to the last valid ID of the range in which allocation without ID will occur if the ALLOC_IN_RANGE flag is added.
     */
    int range_end;
} simple_bitmap_wrap_alloc_info_t;

/*!
* \brief
*   Create a new instance of simple bitmap resource manager.
* 
*
*  \par DIRECT INPUT:
*    \param [in] unit -
*      Relevant unit.
*    \param [in] module_id - Module ID.
*    \param [in] res_tag_bitmap - Resource tag bitmap to be created.
*    \param [in] core_id -
*      Relevant core.
*    \param [in] sub_resource_index -
*      Relevant sub resource index.
*    \param [in] create_data -
*      Pointed memory contains setup parameters required for the
*      creation of the resouce. See \ref sw_state_algo_res_create_data_t
*    \param [in] extra_arguments
*      Placeholder to fill prototype requirement for advanced algorithms.
*      The simple_bitmap_wrap_create_info_t struct can be passed as extra_arguments
*       and then the dnx_algo_res_simple_bitmap_create will behave as sw_state_resource_tag_bitmap_create.
*    \param [in] nof_elements - Number of els in the res tag bitmap. Specified in the relevant sw state xml file.
*    \param [in] alloc_flags - SW state allocation flags.
*       This flags value is determend by the variable type (dnx data for example) of the additional variables specified in the xml
* \see sw_state_resource_tag_bitmap_alloc
*  \par DIRECT OUTPUT:
*    \retval Zero if no error was detected
*    \retval Negative if error was detected. See \ref shr_error_e
*  \remark
*    None
*  \see
*    sw_state_algo_res_create
*    sw_state_res_tag_bitmap_create
*    shr_error_e
*    simple_bitmap_wrap_create_info_t
*/
shr_error_e dnx_algo_res_simple_bitmap_create(
    int unit,
    uint32 module_id,
    sw_state_resource_tag_bitmap_t * res_tag_bitmap,
    int core_id,
    int sub_resource_index,
    sw_state_algo_res_create_data_t * create_data,
    void *extra_arguments,
    uint32 nof_elements,
    uint32 alloc_flags);

/**
* \brief
*   Destroy an instance of simple bitmap resource manager.
* 
*
*  \par DIRECT INPUT:
*    \param [in] unit -
*      Relevant unit.
*    \param [in] module_id - Module ID.
*    \param [in] res_tag_bitmap - Resource tag bitmap.
*    \param [in] extra_arguments
*      Placeholder to fill prototype requirement for advanced algorithms.
*      Should be NULL for this procedure.
*  \par DIRECT OUTPUT:
*    \retval Zero if no error was detected
*    \retval Negative if error was detected. See \ref shr_error_e
*  \par INDIRECT OUTPUT:
*    The res tag bitmap sw state for this resource will be destroyed.
*    sw_state_legacy_access[unit].shareddnx.res_tag_bmp_info
*  \remark
*    None
*  \see
*    sw_state_algo_res_destroy
*    sw_state_res_tag_bitmap_destroy
*    sw_state_res_tag_bitmap_destroy
*    shr_error_e
*/
shr_error_e dnx_algo_res_simple_bitmap_destroy(
    int unit,
    uint32 module_id,
    sw_state_resource_tag_bitmap_t res_tag_bitmap,
    void *extra_arguments);

/*!
* \brief
*   Allocate a free element.
* 
*
*  \par DIRECT INPUT:
*    \param [in] unit -
*      Relevant unit.
*    \param [in] module_id - Module ID.
*    \param [in] res_tag_bitmap - Resource tag bitmap.
*    \param [in] flags -
*      SW_STATE_ALGO_RES_ALLOCATE_* flags
*    \param [in] extra_arguments -
*      Placeholder to fill prototype requirement for advanced algorithms.
*      The simple_bitmap_wrap_alloc_info_t struct can be passed as extra_arguments
*       and then the dnx_algo_res_simple_bitmap_allocate will behave as sw_state_resource_tag_bitmap_alloc.
*    \param [in] element -
*      Pointer to place the allocated element.
*      \b As \b output - \n
*        Holds the allocated element.
*      \b As \b input - \n
*        If flag \ref SW_STATE_ALGO_RES_ALLOCATE_WITH_ID is set, pass the element to be allocated here.
*  \par INDIRECT OUTPUT:
*    *element -
*      Value of the allocated element.
*  \par DIRECT OUTPUT:
*    shr_error_e - 
*      Error return value
*  \remark
*    None
*  \see
*    sw_state_algo_res_allocate
*    sw_state_res_tag_bitmap_alloc
*    shr_error_e
*    simple_bitmap_wrap_alloc_info_t
*****************************************************/
shr_error_e dnx_algo_res_simple_bitmap_allocate(
    int unit,
    uint32 module_id,
    sw_state_resource_tag_bitmap_t res_tag_bitmap,
    uint32 flags,
    void *extra_arguments,
    int *element);

/*!
* \brief
*   Check whether an element was allocated.
* 
*
*  \par DIRECT INPUT:
*    \param [in] unit -
*      Relevant unit.
*    \param [in] module_id - Module ID.
*    \param [in] res_tag_bitmap - Resource tag bitmap.
*    \param [in] element -
*      Element to be checked.
*    \param [in] is_allocated -
*      Pointer to be filled with allocated / free indication.
*  \par INDIRECT OUTPUT:
*    *is_allocated -
*       boolean indication of the element's existence.
*  \par DIRECT OUTPUT:
*    shr_error_e - 
*      Error return value
*  \remark
*    None
*  \see
*    sw_state_algo_res_allocate
*    sw_state_res_tag_bitmap_check
*    shr_error_e
*****************************************************/
shr_error_e dnx_algo_res_simple_bitmap_is_allocated(
    int unit,
    uint32 module_id,
    sw_state_resource_tag_bitmap_t res_tag_bitmap,
    int element,
    uint8 *is_allocated);

/*!
* \brief
*   Free allocated element.
* 
*
*  \par DIRECT INPUT:
*    \param [in] unit -
*      Relevant unit.
*    \param [in] module_id - Module ID.
*    \param [in] res_tag_bitmap - Resource tag bitmap.
*    \param [in] element -
*      Element to be freed.
*  \par DIRECT OUTPUT:
*    shr_error_e - 
*      Error return value
*  \remark
*    None
*  \see
*    sw_state_algo_res_free
*    sw_state_res_tag_bitmap_free
*    shr_error_e
*****************************************************/
shr_error_e dnx_algo_res_simple_bitmap_free(
    int unit,
    uint32 module_id,
    sw_state_resource_tag_bitmap_t res_tag_bitmap,
    int element);

/**
 * \brief
 *   Get the number of currently free elements.
 * 
 *
 *  \par DIRECT INPUT:
 *    \param [in] unit -
 *      Relevant unit.
 *    \param [in] module_id - Module ID.
 *    \param [in] res_tag_bitmap - Resource tag bitmap.
 *    \param [in] nof_free_elements -
 *      Int pointer to memory to load output into. \n
 *      \b As \b output - \n
 *        This procedure loads the memory with the number of free elements in this resource.
 *  \par DIRECT OUTPUT:
 *    \retval Zero if no error was detected
 *    \retval Negative if error was detected. See \ref shr_error_e
 *  \par INDIRECT INPUT:
 *    The sw state used by res bitmap.
 *    sw_state_legacy_access[unit].shareddnx.res_tag_bmp_info
 *  \par INDIRECT OUTPUT:
 *   \b *nof_free_elements \n
 *     See DIRECT INPUT above
 *  \remark
 *    None
 *  \see
 *    sw_state_algo_res_create_data_t
 *    sw_state_res_tag_bitmap_nof_free_elements_get
 *    shr_error_e
 *****************************************************/
shr_error_e dnx_algo_res_simple_bitmap_nof_free_elements_get(
    int unit,
    uint32 module_id,
    sw_state_resource_tag_bitmap_t res_tag_bitmap,
    int *nof_free_elements);

/**
 * \brief
 *   Clear the allocated elements.
 * 
 *
 *  \param [in] unit -
 *      Relevant unit.
 *  \param [in] module_id - Module ID.
 *  \param [in] res_tag_bitmap - Resource tag bitmap.
 *   \return
 *       \retval Zero if no error was detected
 *       \retval Negative if error was detected. See \ref shr_error_e
 *  \remark
 *    None
 *  \see
 *    sw_state_algo_res_clear
 *    dnx_algo_res_advanced_alogrithm_cb_t
 *    shr_error_e
 *****************************************************/
shr_error_e dnx_algo_res_simple_bitmap_clear(
    int unit,
    uint32 module_id,
    sw_state_resource_tag_bitmap_t res_tag_bitmap);

/**
* \brief
*   Allocate several elements.
*
*
*   \param [in] unit -
*       Relevant unit.
*   \param [in] module_id - Module ID.
*   \param [in] res_tag_bitmap - Resource tag bitmap.
*   \param [in] flags -
*       SW_STATE_ALGO_RES_ALLOCATE_* flags
*   \param [in] nof_elements -
*       Number of elements.
*   \param [in] extra_arguments -
*       Placeholder to fill prototype requirement for advanced algorithms.
*       Should be NULL for this procedure.
*   \param [in,out] element -
*       Pointer to place the allocated element.
*       \b As \b output - \n
*       Holds the allocated element.
*       \b As \b input - \n
*       If flag \ref SW_STATE_ALGO_RES_ALLOCATE_WITH_ID is set, pass the element to be allocated here.
*   \return
*       shr_error_e -
*       Error return value
*   \remark
*       None
*   \see
*       sw_state_algo_res_allocate_several
*       sw_state_res_tag_bitmap_allocate_several
*       shr_error_e
*****************************************************/
shr_error_e dnx_algo_res_simple_bitmap_allocate_several(
    int unit,
    uint32 module_id,
    sw_state_resource_tag_bitmap_t res_tag_bitmap,
    uint32 flags,
    uint32 nof_elements,
    void *extra_arguments,
    int *element);

/**
* \brief
*   Free several allocated elements.
*
*
*   \param [in] unit -
*       Relevant unit.
*   \param [in] module_id - Module ID.
*   \param [in] res_tag_bitmap - Resource tag bitmap.
*   \param [in] nof_elements -
*       Numbers of elements to be freed.
*   \param [in] element -
*       Element to be freed.
*   \return
*       shr_error_e -
*       Error return value
*   \remark
*       None
*   \see
*       sw_state_algo_res_free_several
*       sw_state_res_tag_bitmap_free
*       shr_error_e
*****************************************************/
shr_error_e dnx_algo_res_simple_bitmap_free_several(
    int unit,
    uint32 module_id,
    sw_state_resource_tag_bitmap_t res_tag_bitmap,
    uint32 nof_elements,
    int element);

/**
* \brief
*   Gets number of used elements in range.
*
*    \param [in] unit - Relevant unit.
*    \param [in] module_id - Module ID.
*    \param [in] res_tag_bitmap - Relevant res tag bitmap.
*    \param [in] range_start - Range start.
*    \param [in] nof_elements_in_range - Range end.
*    \param [out] nof_allocated_elements - Pointer to the memory where the return value will be saved.
*  \return
*    shr_error_e - Error return value
*  \remark
*    None
*  \see
*    shr_error_e
*****************************************************/
shr_error_e dnx_algo_res_simple_bitmap_nof_allocated_elements_in_range_get(
    int unit,
    uint32 module_id,
    sw_state_resource_tag_bitmap_t res_tag_bitmap,
    int range_start,
    int nof_elements_in_range,
    int *nof_allocated_elements);

/**
* \brief
*   Sets tag to a specific element.
*
*    \param [in] unit - Relevant unit.
*    \param [in] module_id - Module ID.
*    \param [in] res_tag_bitmap - Relevant res tag bitmap.
*    \param [in] algorithm_info - Structure that contains element, tag and force tag flag.
*  \return
*    shr_error_e - Error return value
*  \remark
*    None
*  \see
*    shr_error_e
*****************************************************/
shr_error_e dnx_algo_res_simple_bitmap_tag_set(
    int unit,
    uint32 module_id,
    sw_state_resource_tag_bitmap_t res_tag_bitmap,
    simple_bitmap_wrap_tag_info_t * algorithm_info);

/**
* \brief
*   Get the tag of specific element.
*
*    \param [in] unit - Relevant unit.
*    \param [in] module_id - Module ID.
*    \param [in] res_tag_bitmap - Relevant res tag bitmap.
*    \param [out] algorithm_info - Ponter to save the element, tag and force tag flag.
*  \return
*    shr_error_e - Error return value
*  \remark
*    None
*  \see
*    shr_error_e
*****************************************************/
shr_error_e dnx_algo_res_simple_bitmap_tag_get(
    int unit,
    uint32 module_id,
    sw_state_resource_tag_bitmap_t res_tag_bitmap,
    simple_bitmap_wrap_tag_info_t * algorithm_info);

#endif /* ALGO_SIMPLE_BITMAP_WRAP_INCLUDED */
