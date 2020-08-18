/** \file sw_state_resource_tag_bitmap.h
 *
 * Indexed resource management -- tagged bitmap
 *
 */
#ifndef SW_STATE_RESOURCE_TAG_BITMAP_H_INCLUDED
/*
 * { 
 */
#define SW_STATE_RESOURCE_TAG_BITMAP_H_INCLUDED

/** { */
/*************
* INCLUDES  *
*************/
/** { */
/* must be first */
#include <soc/dnxc/swstate/sw_state_features.h>

#include <shared/utilex/utilex_framework.h>
#include <soc/dnxc/swstate/sw_state_defs.h>
#include <soc/dnxc/swstate/dnxc_sw_state.h>
/** } */

/*************
 * DEFINES   *
 *************/
/** { */

/**
 *  Provide WITH_ID when allocating a block and you want to specify the initial
 *  element of that block.
 *
 *  Provide ALIGN_ZERO when allocating an aligned block and you want that block
 *  to be aligned against zero rather than against the low_id value used when
 *  creating the resource.
 *
 *  Provide REPLACE when allocating WITH_ID to indicate you want to replace an
 *  existing block.  Note this requires that the existing block be there in its
 *  entirety; it is an error to try to change the size of a block this way.
 */
#define SW_STATE_RESOURCE_TAG_BITMAP_ALLOC_WITH_ID              0x00000001
#define SW_STATE_RESOURCE_TAG_BITMAP_ALLOC_ALIGN_ZERO           0x00000002
#define SW_STATE_RESOURCE_TAG_BITMAP_ALLOC_ALIGN                0x00000004
#define SW_STATE_RESOURCE_TAG_BITMAP_ALWAYS_CHECK_TAG           0x00000008
#define SW_STATE_RESOURCE_TAG_BITMAP_ALLOC_CHECK_ONLY           0x00000010
#define SW_STATE_RESOURCE_TAG_BITMAP_ALLOC_IGNORE_TAG           0x00000020
#define SW_STATE_RESOURCE_TAG_BITMAP_ALLOC_SPARSE               0x00000040
#define SW_STATE_RESOURCE_TAG_BITMAP_ALLOC_IN_RANGE             0x00000080

/*
 * Flag for \ref sw_state_resource_tag_bitmap_create
 */
#define SW_STATE_RESOURCE_TAG_BITMAP_CREATE_ALLOW_IGNORING_TAG                 0x00000001
#define SW_STATE_RESOURCE_TAG_BITMAP_CREATE_NO_OPTIMIZATION_PER_TAG            0x00000002
#define SW_STATE_RESOURCE_TAG_BITMAP_CREATE_GET_NOF_ELEMENTS_PER_GRAIN         0x00000004
#define SW_STATE_RESOURCE_TAG_BITMAP_CREATE_ALLOW_FORCING_TAG                  0x00000008
/*
 * Define to specify, that we do not have any create flags.
 */
#define SW_STATE_RESOURCE_TAG_BITMAP_CREATE_FLAGS_NONE 0

/*
 * Max numbers of offsets.
 */
#define SW_STATE_RESOURCE_TAG_BITMAP_MAX_NOF_OFFSETS 4

/** } */

/*************
 * MACROS    *
 *************/
/** { */

#define SW_STATE_RESOURCE_TAG_BITMAP_CREATE(module_id, res_tag_bitmap, create_info, count, flags)\
    SHR_IF_ERR_EXIT(sw_state_resource_tag_bitmap_create(unit, module_id, &res_tag_bitmap, create_info, count, flags))

#define SW_STATE_RESOURCE_TAG_BITMAP_DESTROY(module_id, res_tag_bitmap)\
    SHR_IF_ERR_EXIT(sw_state_resource_tag_bitmap_destroy(unit, module_id, &res_tag_bitmap))

#define SW_STATE_RESOURCE_TAG_BITMAP_TAG_SET(module_id, res_tag_bitmap, tag, force_tag, elemIndex, elemCount)\
    SHR_IF_ERR_EXIT(sw_state_resource_tag_bitmap_tag_set(unit, module_id, res_tag_bitmap, tag, force_tag, elemIndex, elemCount))

#define SW_STATE_RESOURCE_TAG_BITMAP_TAG_GET(module_id, res_tag_bitmap, elemIndex, tag)\
    SHR_IF_ERR_EXIT(sw_state_resource_tag_bitmap_tag_get(unit, module_id, res_tag_bitmap, elemIndex, tag))

#define SW_STATE_RESOURCE_TAG_BITMAP_BITMAP_ALLOC(module_id, res_tag_bitmap, alloc_info, elem)\
    SHR_IF_ERR_EXIT(sw_state_resource_tag_bitmap_alloc(unit, module_id, res_tag_bitmap, alloc_info, elem))

#define SW_STATE_RESOURCE_TAG_BITMAP_BITMAP_FREE(module_id, res_tag_bitmap, free_info, elem)\
    SHR_IF_ERR_EXIT(sw_state_resource_tag_bitmap_free(unit, module_id, res_tag_bitmap, free_info, elem))

#define SW_STATE_RESOURCE_TAG_BITMAP_CLEAR(module_id, res_tag_bitmap)\
    SHR_IF_ERR_EXIT(sw_state_resource_tag_bitmap_clear(unit, module_id, res_tag_bitmap))

#define SW_STATE_RESOURCE_TAG_BITMAP_CHECK(module_id, res_tag_bitmap, count, elem)\
    SHR_IF_ERR_EXIT(sw_state_resource_tag_bitmap_check(unit, module_id, res_tag_bitmap, count, elem))

#define SW_STATE_RESOURCE_TAG_BITMAP_CHECK_ALL(module_id, res_tag_bitmap, count, elem)\
    SHR_IF_ERR_EXIT(sw_state_resource_tag_bitmap_check_all(unit, module_id, res_tag_bitmap, count, elem))

#define SW_STATE_RESOURCE_TAG_BITMAP_CHECK_ALL_TAG(module_id, res_tag_bitmap, tag, count, elem)\
    SHR_IF_ERR_EXIT(sw_state_resource_tag_bitmap_check_all_tag(unit, module_id, res_tag_bitmap, tag, count, elem))

#define SW_STATE_RESOURCE_TAG_BITMAP_NOF_FREE_ELEMENTS_GET(module_id, res_tag_bitmap, nof_free_elements)\
    SHR_IF_ERR_EXIT(sw_state_resource_tag_bitmap_nof_free_elements_get(unit, module_id, res_tag_bitmap, nof_free_elements))

#define SW_STATE_RESOURCE_TAG_BITMAP_NOF_USED_ELEMENTS_IN_GRAIN_GET(module_id, res_tag_bitmap, grain_index, nof_allocated_elements)\
    SHR_IF_ERR_EXIT(sw_state_resource_tag_bitmap_nof_used_elements_in_grain_get(unit, module_id, res_tag_bitmap, grain_index, nof_allocated_elements))

#define SW_STATE_RESOURCE_TAG_BITMAP_SIZE_GET(create_info)\
    sw_state_resource_tag_bitmap_size_get(create_info)

#define SW_STATE_RESOURCE_TAG_BITMAP_CREATE_INFO_GET(module_id, res_tag_bitmap, create_info)\
    SHR_IF_ERR_EXIT(sw_state_resource_tag_bitmap_create_info_get(unit, module_id, res_tag_bitmap, create_info))

#define SW_STATE_RESOURCE_TAG_BITMAP_NOF_ALLOCATED_ELEMENTS_IN_RANGE_GET(module_id, res_tag_bitmap, range_start, nof_elements_in_range, nof_allocated_elements)\
        SHR_IF_ERR_EXIT(sw_state_resource_nof_allocated_elements_in_range_get\
            (unit, module_id, res_tag_bitmap, range_start, nof_elements_in_range, nof_allocated_elements))

#define SW_STATE_RESOURCE_TAG_BITMAP_PRINT(unit, module_id, res_tag_bitmap)\
        SHR_IF_ERR_EXIT(sw_state_resource_tag_bitmap_print(unit, module_id, *res_tag_bitmap))

/** } */

/*************
 * TYPE DEFS *
 *************/
/** { */

/**
 * Includes the information user needs to supply for res_tag_bitmap creation.
 */
typedef struct
{
    /*
     * Minimum valid element ID.
     */
    int low_id;
    /*
     * Total number of elements.
     */
    int count;
    /*
     * Number of elements per grain. Only relevant if tags are used.
     */
    int grain_size;
    /*
     * Maximum tag value. A max_tag_value of 0 means the bitmap doesn't use tags.
     */
    uint32 max_tag_value;
    /*
     * Flags used to create this res_tag_bitmap.
     */
    uint32 flags;
} sw_state_resource_tag_bitmap_create_info_t;

/**
 * Includes the information user needs to supply for allocation/free in res_tag_bitmap.
 */
typedef struct
{
    /*
     * Flags providing specifics of what/how to allocate.
     */
    uint32 flags;
    /*
     * Base alignment. Only relevant if ALLOC_ALIGN flag is set.
     */
    int align;
    /*
     * Offest from base alignment for first element. Only relevant if ALLOC_ALIGN flag is set.
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
     * Number of elements to allocate in the block. If ALLOC_SPARSE flag is not set repeats is equal to count.
     */
    int count;
    /*
     * Starting ID of range in which to allocate without ID if ALLOC_IN_RANGE flag is set.
     */
    int range_start;
    /*
     * First invalid ID after the allocated range.
     */
    int range_end;
} sw_state_resource_tag_bitmap_alloc_info_t;

typedef struct
{
    /**
     * lowest element ID
     */
    int low;
    /**
     * number of elements
     */
    int count;
    /**
     * number of elements in use
     */
    int used;
    /**
     * elements per tag grain
     */
    int grainSize;
    /**
     * bytes per tag
     */
    int tagSize;
    /**
     * first element of last freed block
     */
    int *lastFree;
    /**
     * next element after last alloc block
     */
    int *nextAlloc;
    /**
     * pointer to base of tag data (after data)
     */
    SHR_BITDCL *tagData;
    /**
     * For each grain, indicate whether it's forced tag or a modifiable tag.
     */
    SHR_BITDCL *forced_tag_indication;
    /**
     * Data.
     */
    SHR_BITDCL *data;
    /**
     * Maximum tag value.
     */
    int max_tag_value;
    /**
     * Tag tracking bitmap.
     */
    SHR_BITDCL *tag_tracking_bitmap;
    /**
     * Flags used to create this res_tag_bitmap.
     */
    uint32 flags;
    /**
     * Number of allocated bits in one grain.
     */
    uint32 *nof_allocated_elements_per_grain;
}  *sw_state_resource_tag_bitmap_t;

/** } */

/*************
* GLOBALS   *
*************/
/*
 * { 
 */
/*
 * } 
 */

/*************
* FUNCTIONS *
*************/
/*
 * { 
 */

/**
 * \brief - Create a tagged bitmap resource.
 *
 * \param [in] unit - ID of the device to be used with this bitmap.
 * \param [in] module_id - Module ID.
 * \param [in] res_tag_bitmap - Resource tag bitmap to be destroyed.
 * \param [in] create_info - Relevant create information.
 * \param [in] count - Count of the resource tag bitmap. Specified in the relevant sw state xml file.
 * \param [in] flags - SW state allocation flags.
 *       This flags value is determend by the variable type (dnx data for example) of the additional variables specified in the xml
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   There are count / grain_size grains, and each one has a tag associated.
 * \see
 *   * None
 */
shr_error_e sw_state_resource_tag_bitmap_create(
    int unit,
    uint32 module_id,
    sw_state_resource_tag_bitmap_t * res_tag_bitmap,
    sw_state_resource_tag_bitmap_create_info_t create_info,
    uint32 count,
    uint32 flags);

/**
 * \brief - Destroy a tagged bitmap resource
 *
 * \param [in] unit - ID of the device to be used with this bitmap.
 * \param [in] module_id - Module ID.
 * \param [in] res_tag_bitmap - Resource tag bitmap to be destroyed.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e sw_state_resource_tag_bitmap_destroy(
    int unit,
    uint32 module_id,
    sw_state_resource_tag_bitmap_t * res_tag_bitmap);

/*
 *  Ensure that all grains in an alloc are tagged accordingly.
 */

/**
 * \brief - Force set a tag to a range of elements..
 * 
 * \param [in] unit - ID of the device to be used with this bitmap.
 * \param [in] module_id - Module ID.
 * \param [in] res_tag_bitmap - Resource tag bitmap to set tag on.
 * \param [in] tag - Tag value to use.
 * \param [in] force_tag - If the bitmap was created with the SW_STATE_RESOURCE_TAG_BITMAP_CREATE_ALLOW_FORCING_TAG flag,
 *                          then set this to TRUE to force the tag, and to FALSE to release it.
 * \param [in] elemIndex - Start of the range to set.
 * \param [in] elemCount - How many elements in the range.
 *
 * \return
 *   shr_error_e 
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e sw_state_resource_tag_bitmap_tag_set(
    int unit,
    uint32 module_id,
    sw_state_resource_tag_bitmap_t res_tag_bitmap,
    uint32 tag,
    int force_tag,
    int elemIndex,
    int elemCount);

/**
 * \brief - Get the tag for a specific element.
 * 
 * \param [in] unit - ID of the device to be used with this bitmap.
 * \param [in] module_id - Module ID.
 * \param [in] res_tag_bitmap - Resource tag bitmap to get the tag from.
 * \param [in] elemIndex - Element index to be checked.
 * \param [out] tag - Tag configured for this element.
 *
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e sw_state_resource_tag_bitmap_tag_get(
    int unit,
    uint32 module_id,
    sw_state_resource_tag_bitmap_t res_tag_bitmap,
    int elemIndex,
    uint32 *tag);

/**
 * \brief
 *   Allocate an element or block of elements of a particular resource
 *
 *   \param [in] unit - Relevant unit.
 *   \param [in] module_id - Module ID.
 *   \param [in] res_tag_bitmap - Resource tag bitmap to be allocated into.
 *   \param [in] alloc_info - All relevant data needed during the allocation.
 *   \param [in,out] elem - Pointer to place the allocated element.
 *       \b As \b output - \n
 *       Holds the first *allocated* element in the pattern.
 *       \b As \b input - \n
 *       If flag \ref SW_STATE_ALGO_RES_ALLOCATE_WITH_ID is set, pass the first element to be *allocated* here.
 *   \return
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   \remark
 *      This will allocate a single block of the requested number of elements
 *      of this resource (each of which may be a number of elements taken from
 *      the underlying pool).
 *
 *      If the ALLOC_ALIGN flag is set the first element of the
 *      returned block will be at ((n * align) + offset), where n is some
 *      integer.  If it is not possible to allocate a block with the requested
 *      constraints, the call will fail. Note that the alignment is within
 *      the specified range of the resource, and not specifically aligned against
 *      the absolute value zero; to request the alignment be against zero,
 *      specify the ALIGN_ZERO flag.
 *
 *      If offset >= align, BCM_E_PARAM. If align is zero or negative, it will
 *      be treated as if it were 1.
 *
 *      If WITH_ID is specified, and the requested base element does not
 *      comply with the indicated alignment, BCM_E_PARAM will be returned.
 *
 *      If the ALLOC_SPARSE flag is set allocates according to a pattern.
 *      The pattern argument is a bitmap of the elements that are of interest
 *      in a single iteration of the pattern (and only the least significant
 *      'length' bits are used; higher bits are ignored).  The bit with value
 *      (1 << k) set indicates the element at (elem + k) must be in the block;
 *      clear it indicates the element at (elem + k) is not in the block.  This
 *      repeats for as many iterations as indicated by 'repeats'.
 *
 *      The caller must track the pattern, length and repeats values and
 *      provide these values along with the elem value when freeing the block.
 *
 *      Any allocation made through this function with ALLOC_SPARSE flag must be freed using the
 *      same flag in sw_state_resource_bitmap_free function.
 *   \see
 *   * None
*****************************************************/
shr_error_e sw_state_resource_tag_bitmap_alloc(
    int unit,
    uint32 module_id,
    sw_state_resource_tag_bitmap_t res_tag_bitmap,
    sw_state_resource_tag_bitmap_alloc_info_t alloc_info,
    int *elem);

/**
 * \brief
 *   Free an element or block of elements of a particular
 *   resource
 *
 *
 *   \param [in] unit -
 *       Relevant unit.
 *   \param [in] module_id - Module ID.
 *   \param [in] res_tag_bitmap - Resource tag bitmap to be freed from.
 *   \param [in] free_info -
 *       The only fields taken into consideration are the ALLOC_SPARSE flag and the fields related to it.
 *   \param [in] elem -
 *      The element to free.
 *   \return
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   \remark
 *      This will free a sparse block of elements, starting
 *      at the specified element and proceeding as specified.
 *
 *      This should only be called with valid data (base element, pattern,
 *      length, repeats).  Trying to free a block that is not in use or trying
 *      to free something that spans multiple allocated blocks may not work.
 *
 *   \see
 *     * None
*****************************************************/
shr_error_e sw_state_resource_tag_bitmap_free(
    int unit,
    uint32 module_id,
    sw_state_resource_tag_bitmap_t res_tag_bitmap,
    sw_state_resource_tag_bitmap_alloc_info_t free_info,
    int elem);

/**
 * \brief
 *   Clear all elements of a particular resource
 *
 *
 *   \param [in] unit -
 *       Relevant unit.
 *   \param [in] module_id - Module ID.
 *   \param [in] res_tag_bitmap - Resource tag bitmap to be cleared.
 *   \return
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   \remark
 *      None
 *   \see
 *       dnx_algo_res_simple_bitmap_clear
*****************************************************/
shr_error_e sw_state_resource_tag_bitmap_clear(
    int unit,
    uint32 module_id,
    sw_state_resource_tag_bitmap_t res_tag_bitmap);

/**
 * \brief - Check the status of a specific element.
 *
 * \param [in] unit - ID of the device to be used with this bitmap.
 * \param [in] module_id - Module ID.
 * \param [in] res_tag_bitmap - Resource tag bitmap.
 * \param [in] count - Number of elements to check in this block.
 * \param [in] elem - The element to check (or base of the block to check).
 *
 * \return
 *   shr_error_e
 *    _SHR_E_NOT_FOUND if the element is not in use.
 *    _SHR_E_EXISTS if the element is in use.
 *    _SHR_E_PARAM if the element is not valid.
 *    _SHR_E_* as appropriate otherwise.
 *
 * \remark
 *      This will check whether the requested block of the resource is
 *      allocated.  Note that if any element of the resource in the range of
 *      [elem..(elem+count-1)] (inclusive) is not free, it returns
 *      BCM_E_EXISTS; it will only return BCM_E_NOT_FOUND if all elements
 *      within the specified block are free.
 *
 *      Normally this should be called to check on a specific block (one that
 *      is thought to exist or in preparation for allocating it WITH_ID).
 * \see
 *   * None
 */
shr_error_e sw_state_resource_tag_bitmap_check(
    int unit,
    uint32 module_id,
    sw_state_resource_tag_bitmap_t res_tag_bitmap,
    int count,
    int elem);

/**
 * \brief - Check the status of a block of elements.
 *
 * \param [in] unit - ID of the device to be used with this bitmap.
 * \param [in] module_id - Module ID.
 * \param [in] res_tag_bitmap - Resource tag bitmap.
 * \param [in] tag - Value of the tag to check.
 * \param [in] count - Number of elements to check in this block.
 * \param [in] elem - The element to check (or base of the block to check).
 *
 * \return
 *   shr_error_e
 *      _SHR_E_EMPTY if none of the elements are in use.
 *      _SHR_E_FULL if all of the elements are in use.
 *      _SHR_E_CONFIG if elements are in use but block(s) do not match the tag.
 *      _SHR_E_EXISTS if some of the elements are in use but not all of them.
 *      _SHR_E_PARAM if any of the elements is not valid.
 *      _SHR_E_* as appropriate otherwise.
 * \remark
 *      As \ref sw_state_resource_tag_bitmap_check except this:
 *
 *      WANRING: The tagged bitmap allocator does not track blocks internally
 *      and so it is possible that if there are two adjacent blocks with
 *      identical tags both allocated and this is called to check whether safe
 *      to 'reallocate', will falsely indicate that it can be done.  However,
 *      this will consider different tags to indicate different blocks, so will
 *      not assert false true for the case if the adjacent blocks have
 *      different tags, unless the block size is smaller than the tag size, in
 *      which case it still could falsely claim the operation is valid. Also,
 *      'reallocate' in a similar manner of a large block to a smaller block
 *      could leak underlying resources.
 * \see
 *   sw_state_resource_tag_bitmap_check
 */
shr_error_e sw_state_resource_tag_bitmap_check_all_tag(
    int unit,
    uint32 module_id,
    sw_state_resource_tag_bitmap_t res_tag_bitmap,
    uint32 tag,
    int count,
    int elem);

/**
 * \brief - Check the status of a block of elements.
 *
 * \param [in] unit - ID of the device to be used with this bitmap.
 * \param [in] module_id - Module ID.
 * \param [in] res_tag_bitmap - Resource tag bitmap.
 * \param [in] count - Number of elements to check in this block.
 * \param [in] elem - The element to check (or base of the block to check).
 *
 * \return
 *   shr_error_e
 *      _SHR_E_EMPTY if none of the elements are in use.
 *      _SHR_E_FULL if all of the elements are in use.
 *      _SHR_E_CONFIG if elements are in use but block(s) do not match the tag.
 *      _SHR_E_EXISTS if some of the elements are in use but not all of them.
 *      _SHR_E_PARAM if any of the elements is not valid.
 *      _SHR_E_* as appropriate otherwise.
 * \remark
 *      As \ref sw_state_resource_tag_bitmap_check_all_tag except without explicit tag.
 *
 *      Unlike the other supported operations that work without explicit tag, this
 *      function assumes the first tag in the block is the desired tag.  If you
 *      want the normal default tag behaviour, provide NULL as the tag argument and
 *      call sw_state_resource_tag_bitmap_check_all_tag instead.
 * \see
 *   sw_state_resource_tag_bitmap_check
 */
shr_error_e sw_state_resource_tag_bitmap_check_all(
    int unit,
    uint32 module_id,
    sw_state_resource_tag_bitmap_t res_tag_bitmap,
    int count,
    int elem);

/**
 * \brief - Returns the number of free elements for this resource.
 *
 * \param [in] unit - ID of the device to be used with this bitmap.
 * \param [in] module_id - Module ID.
 * \param [in] res_tag_bitmap - Resource tag bitmap.
 * \param [out] nof_free_elements - Pointer will be filled with the number
 *                                  of free elements in this resource.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e sw_state_resource_tag_bitmap_nof_free_elements_get(
    int unit,
    uint32 module_id,
    sw_state_resource_tag_bitmap_t res_tag_bitmap,
    int *nof_free_elements);

/**
 * \brief - Returns the number of allocated elements per grain.
 *          Note that this api will be active only if _CREATE_GET_NOF_ELEMENTS_PER_GRAIN flag was given
 *          during creation of the resource.
 *
 * \param [in] unit - ID of the device to be used with this bitmap.
 * \param [in] module_id - Module ID.
 * \param [in] res_tag_bitmap - Resource tag bitmap.
 * \param [in] grain_index - Relevant grain index.
 * \param [out] nof_allocated_elements - Pointer will be filled with the number
 *                                  of allocated bits in this grain.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e sw_state_resource_tag_bitmap_nof_used_elements_in_grain_get(
    int unit,
    uint32 module_id,
    sw_state_resource_tag_bitmap_t res_tag_bitmap,
    uint32 grain_index,
    uint32 *nof_allocated_elements);

/**
 * \brief - Returned a tagged bitmap resource.
 *
 * \param [in] create_info - Relevant create information.
 *
 * \return
 *   int
 *
 * \remark
 *   None
 * \see
 *   None
 */
int sw_state_resource_tag_bitmap_size_get(
    sw_state_resource_tag_bitmap_create_info_t create_info);

/**
 * \brief - Returns the create info used during create.
 *
 * \param [in] unit - ID of the device to be used with this bitmap.
 * \param [in] module_id - Module ID.
 * \param [in] res_tag_bitmap - Resource tag bitmap.
 * \param [in] create_info - Pointer to location in which will be saved the create info.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e sw_state_resource_tag_bitmap_create_info_get(
    int unit,
    uint32 module_id,
    sw_state_resource_tag_bitmap_t res_tag_bitmap,
    sw_state_resource_tag_bitmap_create_info_t * create_info);

/**
* \brief
*   Gets number of used elements in range.
*
*    \param [in] unit - Relevant unit.
*    \param [in] module_id - Module ID.
*    \param [in] res_tag_bitmap - Relevant res tag bitmap.
*    \param [in] range_start - Range start.
*    \param [in] nof_elements_in_range - Range size.
*    \param [out] nof_allocated_elements - Pointer to the memory where the return value will be saved.
*  \return
*    shr_error_e - Error return value
*  \remark
*    None
*  \see
*    shr_error_e
*****************************************************/
shr_error_e sw_state_resource_nof_allocated_elements_in_range_get(
    int unit,
    uint32 module_id,
    sw_state_resource_tag_bitmap_t res_tag_bitmap,
    int range_start,
    int nof_elements_in_range,
    int *nof_allocated_elements);

/**
* \brief
*   Prints res tag bitmap data.
*
*
*    \param [in] unit - Relevant unit.
*    \param [in] module_id - Module ID.
*    \param [in] res_tag_bitmap - Relevant res tag bitmap.
*  \return
*    \retval Zero if no error was detected
*    \retval Negative if error was detected. See \ref shr_error_e
*  \remark
*    None
*  \see
*    None
*****************************************************/
int sw_state_resource_tag_bitmap_print(
    int unit,
    uint32 module_id,
    sw_state_resource_tag_bitmap_t res_tag_bitmap);

/*
 * }
 */

/*
 * } SW_STATE_RESOURCE_TAG_BITMAP_H_INCLUDED
 */
#endif
