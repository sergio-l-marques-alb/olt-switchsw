/**
 * \file sw_state_occupation_bitmap.h
 *
 * Definitions and prototypes for all common utilities related
 * to occupation bitmap.
 *
 * Note that this is used by other 'sw_state' packages such
 * as sorted-list utilities or hash-table utilities
 */
#ifndef SW_STATE_OCCUPATION_BITMAP_H_INCLUDED
/*
 * { 
 */
#define SW_STATE_OCCUPATION_BITMAP_H_INCLUDED

/*************
* INCLUDES  *
*************/
/** { */
/* must be first */
#include <soc/dnxc/swstate/sw_state_features.h>

#include <shared/utilex/utilex_framework.h>
/** } */

/*************
* DEFINES   *
*************/
/** { */
/**
 * \brief
 * Number of bit-combinations in a byte
 * See sw_state_bitmap_val_to_first_one()
 */
#define  SW_STATE_OCC_BM_ARRAY_MAP_LEN SAL_BIT(UTILEX_NOF_BITS_IN_CHAR)
/**
 * \brief
 * Indication, on the various sw_state_bitmap_val_to_* arrays, that
 * an entry is not meaningful (for instance, 'first non-zero bit on
 * a byte whose value is 0').
 * See sw_state_bitmap_val_to_first_one()
 */
#define  SW_STATE_OCC_BM_NODE_IS_FULL  UTILEX_NOF_BITS_IN_CHAR
/** } */

/*************
* MACROS    *
*************/
/*
 * { 
 */

#define SW_STATE_OCC_BM_CREATE(module_id, occ_bm, init_info, occ_size, alloc_flags) \
    SHR_IF_ERR_EXIT(sw_state_occ_bm_create(unit, module_id, init_info, &occ_bm, occ_size, alloc_flags));

#define SW_STATE_OCC_BM_DESTROY(module_id, occ_bm) \
    sw_state_occ_bm_destroy(unit, module_id, &occ_bm)

#define SW_STATE_OCC_BM_INIT_INFO_CLEAR(init_info) \
    sw_state_occ_bm_init_info_clear(init_info)

#define SW_STATE_OCC_BM_GET_NEXT_IN_RANGE(module_id, occ_bm, start, end, forward, val, place, found) \
    sw_state_occ_bm_get_next_in_range(unit, module_id, occ_bm, start, end, forward, val, place, found)

#define SW_STATE_OCC_BM_GET_NEXT(module_id, occ_bm, place, found) \
    sw_state_occ_bm_get_next(unit, module_id, occ_bm, place, found)

#define SW_STATE_OCC_BM_STATUS_SET(module_id, occ_bm, place, occupied) \
    sw_state_occ_bm_occup_status_set(unit, module_id, occ_bm, place, occupied)

#define SW_STATE_OCC_BM_IS_OCCUPIED(module_id, occ_bm, place, occupied) \
    sw_state_occ_bm_is_occupied(unit, module_id, occ_bm, place, occupied)

#define SW_STATE_OCC_BM_ALLOC_NEXT(module_id, occ_bm, place, found) \
    sw_state_occ_bm_alloc_next(unit, module_id, occ_bm, place, found)

#define SW_STATE_OCC_BM_CLEAR(module_id, occ_bm) \
    sw_state_occ_bm_clear(unit, module_id, occ_bm)

#define SW_STATE_OCC_BM_PRINT(module_id, occ_bm) \
    sw_state_occ_bm_print(unit, occ_bm)

#define SW_STATE_OCC_BM_DEFAULT_PRINT(unit, occ_bm) \
    sw_state_occ_bm_print(unit, *(occ_bm))

#define SW_STATE_OCC_BM_ACTIVE_INST_GET(_bit_map, _level_indx, _char_indx, _cur_val) \
  { \
    uint8 cache_enabled ; \
    uint32 buffer_offset ; \
    cache_enabled = _bit_map->cache_enabled; \
    if (cache_enabled) \
    { \
      /* \
       * Get offset within 'levels_cache_buffer' (was 'pointer' on original). \
       */ \
      buffer_offset = _bit_map->levels_cache[_level_indx]; \
      _cur_val = _bit_map->levels_cache_buffer[buffer_offset + _char_indx]; \
    } \
    else \
    { \
      /* \
       * Get offset within 'levels_buffer' (was 'pointer' on original). \
       */ \
      buffer_offset = _bit_map->levels[_level_indx]; \
      _cur_val = _bit_map->levels_buffer[buffer_offset + _char_indx]; \
    } \
  }

#define SW_STATE_OCC_BM_SIZE_GET(init_info) \
    sw_state_occ_bm_size_get(init_info);

/**
 * \brief
 * Verify specified unit has a legal value. If not, software goes to \n
 * exit with error code.
 * 
 * Notes: \n
 *   'exit' is assumed to be defined in the caller's scope.
 */
#define SW_STATE_OCC_BM_VERIFY_UNIT_IS_LEGAL(_unit) \
  if (((int)_unit < 0) || (_unit >= SOC_MAX_NUM_DEVICES)) \
  { \
    /* \
     * If this is an illegal unit identifier, quit \
     * with error. \
     */ \
    SHR_SET_CURRENT_ERR(_SHR_E_UNAVAIL) ; \
    SHR_EXIT() ; \
  }
/*
 * } 
 */

/*************
* TYPE DEFS *
*************/
/*
 * { 
 */

/**
 * Structure for one bit map element
 */
typedef struct
{
    /**
     * Array to hold offsets in the levels_buffer.
     */
    uint32 *levels;
    /**
     * Array to hold the size (in bits of every level)
     */
    uint32 *levels_size;
    /**
     * Number of levels in the bitmap (length of levels array)
     */
    uint32 nof_levels;
    /**
     * The size of this bitmap.
     */
    uint32 size;
    /**
     * Init value for bitmap
     */
    uint8 init_val;
    /**
     * Flag: If non-zero - support caching
     */
    uint8 support_cache;
    /**
     * Flag: If non-zero - Enable caching
     */
    uint8 cache_enabled;
    /**
     * Array to hold offsets in the levels_cache_buffer.
     */
    uint32 *levels_cache;
    /**
     * The allocated buffer of 'levels' and 'levels_cache' (Each is a bit map)
     */
    uint8 *levels_buffer;
    uint8 *levels_cache_buffer;
    /**
     * The size (in butes) of levels_buffer/levels_cache_buffer
     */
    uint32 buffer_size;
}  *sw_state_occ_bm_t;

/**
 * \brief
 * Structure including the information user has to supply on bit map creation
 */
typedef struct
{
    /**
     *  The size of the occupation bitmap (in bits)
     */
    uint32 size;
    /**
     *  Initial value to fill the bitmap with.
     */
    uint8 init_val;
    /**
     *  Flag. If non zero then caching is supported
     */
    uint8 support_cache;

} sw_state_occ_bitmap_init_info_t;

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

shr_error_e sw_state_occ_bm_init_info_clear(
    sw_state_occ_bitmap_init_info_t * info);

/**
 * \brief
 *   Creates a new bitmap instance.
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of the device to access.
 *   \param [in] module_id -
 *     Module id.
 *   \param [in] bit_map_ptr -
 *     Pointer to memory to load output into. \n
 *     \b As \b output - \n
 *       This procedure loads pointed memory by handle (identifier) of
 *       newly created bitmap.
 *   \param [in] init_info -
 *     Pointed memory contains setup parameters required for for the
 *     creation of the bitmap.
 *   \param [in] occ_size - Size of the occupational bitmap (in bits). Specified in the relevant sw state xml file.
 *   \param [in] alloc_flags - SW state allocation flags.
 *        This flags value is determend by the variable type (dnx data for example) of the additional variables specified in the xml
 * \par INDIRECT INPUT
 *   SWSTATE system
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT
 *   \b *bit_map_ptr \n
 *     See 'bit_map_ptr' in DIRECT INPUT above
 * \see
 *   * sw_state_occ_bm_init()
 */
shr_error_e sw_state_occ_bm_create(
    int unit,
    uint32 module_id,
    sw_state_occ_bitmap_init_info_t * init_info,
    sw_state_occ_bm_t * bit_map_ptr,
    uint32 occ_size,
    uint32 alloc_flags);
/**
 * \brief
 *   Free the specified bitmap instance (all corresponding memory).
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of the device to access.
 *   \param [in] module_id -
 *     Module id.
 *   \param [in] bit_map_ptr -
 *     Pointer to bitmap to destroy.
 * \par INDIRECT INPUT
 *   SWSTATE system
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *   * sw_state_occ_bm_create()
 */
shr_error_e sw_state_occ_bm_destroy(
    int unit,
    uint32 module_id,
    sw_state_occ_bm_t * bit_map_ptr);

/**
 * \brief
 *   Load (fill) bit map by specified initial value. (No memory is freed!)
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of the device to access.
 *   \param [in] module_id -
 *     Module id.
 *   \param [in] bit_map -
 *     Handle of the bitmap to load by init value. See remarks.
 * \par INDIRECT INPUT
 *   SWSTATE system
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT
 *   All bit map arrays are filled by 'init_val'. See remarks below.
 * \see
 *   * sw_state_occ_bm_create()
 * \remarks
 *   Initial value is specified, upon create, in the 'init_val' element
 *   of init info (which is input to sw_state_occ_bm_create()).
 *   If 'init_val' is FALSE then bit map is filled by '0'. Otherwise,
 *   it is filled by '1'.
 */
shr_error_e sw_state_occ_bm_clear(
    int unit,
    uint32 module_id,
    sw_state_occ_bm_t bit_map);

/**
 * \brief
 *   Get the first zero bit on the bitmap (starting from bit 0 )
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of the device to access.
 *   \param [in] module_id -
 *     Module id.
 *   \param [in] bit_map -
 *     Handle of the bitmap perform the get operation at.
 *   \param [in] place -
 *     Pointer to uint32 \n
 *     \b As \b output - \n
 *     This procedure loads pointed memory by the index of the
 *     bit which was found to be '0'. This is bit index so, say,
 *     for a byte value of 0x03, the index of the first zero bit
 *     is '2'. \n
 *     This is meaningful only if '*found' is non-zero. Otherwise,
 *     this procedure loads '0' into '*place'.
 *   \param [in] found -
 *     Pointer to uint32 \n
 *     \b As \b output - \n
 *     This procedure loads pointed memory by a non-zero value if
 *     a zero bit was, indeed, found. Otherwise, a zero is loaded.
 * \par INDIRECT INPUT
 *   SWSTATE system
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT
 *   * See 'place' in DIRECT INPUT
 *   * See 'found' in DIRECT INPUT
 * \see
 *   * sw_state_occ_bm_alloc_next()
 *   * sw_state_occ_bm_test_1()
 */
shr_error_e sw_state_occ_bm_get_next(
    int unit,
    uint32 module_id,
    sw_state_occ_bm_t bit_map,
    uint32 *place,
    uint8 *found);

/**
 * \brief
 *   Get the first/last bit in a range with a value '0'.
 *   See 'forward'. See remarks below
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of the device to access.
 *   \param [in] module_id -
 *     Module id.
 *   \param [in] bit_map -
 *     Handle of the bitmap perform the get operation at.
 *   \param [in] start -
 *     Index of start bit of the range (index counting starts from zero).
 *   \param [in] end -
 *     Index of end bit of the range (index counting starts from zero).
 *   \param [in] forward -
 *     * If TRUE: look forward (starting from first bit ('start'))
 *     * If FALSE: look backward (starting from last bit ('end'))
 *   \param [in] val -
 *     The val to look for (0/1)
 *   \param [in] place -
 *     Pointer to uint32 \n
 *     \b As \b output - \n
 *     This procedure loads pointed memory by the index of the
 *     bit which was found to be '0'. This is bit index so, say,
 *     for a byte value of 0x03, the index of the first zero bit
 *     is '2'. \n
 *     This is meaningful only if '*found' is non-zero. Otherwise,
 *     this procedure loads '0' into '*place'.
 *   \param [in] found -
 *     Pointer to uint32 \n
 *     \b As \b output - \n
 *     This procedure loads pointed memory by a non-zero value if
 *     a zero bit was, indeed, found. Otherwise, a zero is loaded.
 * \par INDIRECT INPUT
 *   SWSTATE system
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT
 *   * See 'place' in DIRECT INPUT
 *   * See 'found' in DIRECT INPUT
 * \see
 *   * sw_state_occ_bm_alloc_next()
 *   * sw_state_occ_bm_test_1()
 * \remarks
 *   The search includes both edges of the range. For example, if 'start'
 *   is '16' and 'end' is '19' then 4 bits are serached: 16,17,18,19
 */
shr_error_e sw_state_occ_bm_get_next_in_range(
    int unit,
    uint32 module_id,
    sw_state_occ_bm_t bit_map,
    uint32 start,
    uint32 end,
    uint8 forward,
    uint8 val,
    uint32 *place,
    uint8 *found);

/**
 * \brief
 *   Creates a new bitmap instance: Find the first bit with a value '0'
 *   within the bit_map and set it to '1'.
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of the device to access.
 *   \param [in] module_id -
 *     Module id.
 *   \param [in] bit_map -
 *     Handle of the bitmap perform the operation on.
 *   \param [in] place -
 *     Pointer to uint32 \n
 *     \b As \b output - \n
 *     This procedure loads pointed memory by the index of the
 *     bit which was found to be '0' (and set to be '1'). This is
 *     bit index so, say, for a byte value of 0x03, the index of the
 *     first zero bit is '2'. \n
 *     This is meaningful only if '*found' is non-zero. Otherwise,
 *     this procedure loads '0' into '*place'.
 *   \param [in] found -
 *     Pointer to uint32 \n
 *     \b As \b output - \n
 *     This procedure loads pointed memory by a non-zero value if
 *     a zero bit was, indeed, found. Otherwise, a zero is loaded.
 * \par INDIRECT INPUT
 *   SWSTATE system 
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT
 *   * See 'place' in DIRECT INPUT
 *   * See 'found' in DIRECT INPUT
 * \see
 *   * sw_state_occ_bm_get_next()
 *   * sw_state_occ_bm_occup_status_set()
 */
shr_error_e sw_state_occ_bm_alloc_next(
    int unit,
    uint32 module_id,
    sw_state_occ_bm_t bit_map,
    uint32 *place,
    uint8 *found);
/**
 * \brief
 *   Set the occupation status a of the given bit location:
 *   either '1' or '0'.
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of the device to access.
 *   \param [in] module_id -
 *     Module id.
 *   \param [in] bit_map -
 *     Handle of the bitmap perform the operation on.
 *   \param [in] place -
 *     The index of the bit to be set as per 'occupied' ('0' or '1').
 *   \param [in] occupied -
 *     The status (occupied/unoccupied, '1'/'0') to set for the 
 *     indicated 'place'
 * \par INDIRECT INPUT
 *   SWSTATE system 
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT
 *   * The changed status of specified bit (Note that if bit
 *     was already at the requested status, nothing will be changed.)
 * \see
 *   * sw_state_occ_bm_occup_status_set_helper()
 *   * sw_state_occ_bm_alloc_next()
 */
shr_error_e sw_state_occ_bm_occup_status_set(
    int unit,
    uint32 module_id,
    sw_state_occ_bm_t bit_map,
    uint32 place,
    uint8 occupied);

/**
 * \brief
 *   Get the occupation status a of the given of bit:
 *   Essentially indicate whether it is '1' or '0'.
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of the device to access.
 *   \param [in] module_id -
 *     Module id.
 *   \param [in] bit_map -
 *     Handle of the bitmap perform the operation on.
 *   \param [in] place -
 *     The index of the bit to be get status of
 *   \param [in] occupied -
 *     Pointer to uint8 - \n
 *     \b As \b output - \n
 *     This procedure loads pointed memory by the status
 *     (occupied/unoccupied, '1'/'0') of the bit at the 
 *     indicated 'place'
 * \par INDIRECT INPUT
 *   SWSTATE system 
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT
 *   * See 'occupied' in DIRECT INPUT
 * \see
 *   * sw_state_occ_bm_occup_status_set_helper()
 *   * sw_state_occ_bm_alloc_next()
 */
shr_error_e sw_state_occ_bm_is_occupied(
    int unit,
    uint32 module_id,
    sw_state_occ_bm_t bit_map,
    uint32 place,
    uint8 *occupied);

/**
 * \brief
 *   Activate/deactivate cache on specified bitmap and copy
 *   bitmap buffer to its cache image. See remarks.
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of the device to access.
 *   \param [in] module_id -
 *     Module id.
 *   \param [in] bit_map -
 *     Handle of the bitmap perform the operation on.
 *   \param [in] cached -
 *     Flag indicating whether to activate the cache (non-zero)
 *     or to deactivate it. If current status is the same as requested,
 *     no operation is carried out.
 * \par INDIRECT INPUT
 *   SWSTATE system 
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT
 *   * Changed status of activation of cache (if required status is
 *     different from current status)
 *   * Newly loaded cache image (if required status is different
 *     from current status and is 'activate')
 * \see
 *   * sw_state_occ_bm_create()
 *   * SW_STATE_OCC_BM_ACTIVE_INST_GET
 *   * SW_STATE_OCC_BM_ACTIVE_INST_SET
 *   * sw_state_occ_bm_cache_rollback()
 *   * sw_state_occ_bm_cache_commit()
 *   * sw_state_occ_bm_clear()
 * \remarks
 *   If required operation is to activate the cache ('cached' is non-zero)
 *   and this bitmap is not marked as suppoting cache ('support_cache' is zero)
 *   then this procedure will return with error (_SHR_E_INTERNAL)
 */
shr_error_e sw_state_occ_bm_cache_set(
    int unit,
    uint32 module_id,
    sw_state_occ_bm_t bit_map,
    uint8 cached);
/**
 * \brief
 *   Copy cache image on specified bitmap to bitmap buffer. See remarks.
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of the device to access.
 *   \param [in] module_id -
 *     Module id.
 *   \param [in] bit_map -
 *     Handle of the bitmap perform the operation on.
 *   \param [in] flags -
 *     Control flags. Currently not in use.
 * \par INDIRECT INPUT
 *   SWSTATE system 
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT
 *   * Newly loaded bitmap buffer (if cache is activated)
 * \see
 *   * sw_state_occ_bm_create()
 *   * SW_STATE_OCC_BM_ACTIVE_INST_GET
 *   * SW_STATE_OCC_BM_ACTIVE_INST_SET
 *   * sw_state_occ_bm_cache_rollback()
 *   * sw_state_occ_bm_clear()
 * \remarks
 *   If cache is not supported
 *   then this procedure will return with error (_SHR_E_INTERNAL)
 *   If cache is not active then no operation is taken.
 */
shr_error_e sw_state_occ_bm_cache_commit(
    int unit,
    uint32 module_id,
    sw_state_occ_bm_t bit_map,
    uint32 flags);
/**
 * \brief
 *   Copy bitmap buffer to cache image on specified bitmap. See remarks.
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of the device to access.
 *   \param [in] module_id -
 *     Module id.
 *   \param [in] bit_map -
 *     Handle of the bitmap perform the operation on.
 *   \param [in] flags -
 *     Control flags. Currently not in use.
 * \par INDIRECT INPUT
 *   SWSTATE system 
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT
 *   * Newly loaded cache image (if cache is activated)
 * \see
 *   * sw_state_occ_bm_create()
 *   * SW_STATE_OCC_BM_ACTIVE_INST_GET
 *   * SW_STATE_OCC_BM_ACTIVE_INST_SET
 *   * sw_state_occ_bm_cache_commit()
 *   * sw_state_occ_bm_clear()
 * \remarks
 *   If cache is not supported
 *   then this procedure will return with error (_SHR_E_INTERNAL)
 *   If cache is not active then no operation is taken.
 */
shr_error_e sw_state_occ_bm_cache_rollback(
    int unit,
    uint32 module_id,
    sw_state_occ_bm_t bit_map,
    uint32 flags);

/**
 * \brief
 *   Returns the size, in bytes, of the memory buffer assigned to
 *   each of the two buffers containing all levels of the bitmap:
 *   'levels_cache_buffer' and 'levels_buffer'
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of the device to access.
 *   \param [in] bit_map -
 *     Handle of the bitmap to get the size of. Bitmap is assumed
 *     to have been created.
 *   \param [in] buffer_size_p -
 *     Pointer to uint32. \n
 *     \b As \b output - \n
 *     This procedure loads '*buffer_size_p' by the number of bytes occupied by
 *     each of the bimap images: 'levels_cache_buffer' and 'levels_buffer'
 * \par INDIRECT INPUT
 *   * SWSTATE system 
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT
 *   * See \b 'buffer_size_p' in DIRECT INPUT
 */
shr_error_e sw_state_occ_bm_get_buffer_size(
    int unit,
    sw_state_occ_bm_t bit_map,
    uint32 *buffer_size_p);

/**
 * \brief
 *   Returns the flag indicating whether cache is supported of
 *   specified bitmap
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of the device to access.
 *   \param [in] bit_map -
 *     Handle of the bitmap to get the size of. Bitmap is assumed
 *     to have been created.
 *   \param [in] support_cache_p -
 *     Pointer to uint8. \n
 *     \b As \b output - \n
 *     This procedure loads '*support_cache_p' by the the flag assigned to
 *     ethis bitmap, which indicates whether cache is supported (non-zero)
 *     or not.
 * \par INDIRECT INPUT
 *   * SWSTATE system 
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT
 *   * See \b 'support_cache_p' in DIRECT INPUT
 * \see
 *   * sw_state_occ_bm_create()
 */
shr_error_e sw_state_occ_bm_get_support_cache(
    int unit,
    sw_state_occ_bm_t bit_map,
    uint8 *support_cache_p);
/*
 * Get handle to occ bitmap which will be considered illegal
 * by all occ bitmap utilities.
 */
/**
 * \brief
 *   Get handle to occ bitmap which will be considered illegal
 *   by all occ bitmap utilities.
 * \par DIRECT INPUT
 *   None
 * \par INDIRECT INPUT
 *   None
 * \par DIRECT OUTPUT
 *   Value of handle which is considered 'illegal'
 * \par INDIRECT OUTPUT
 *   * None
 */
uint32 sw_state_occ_bm_get_illegal_bitmap_handle(
    void);
/*********************************************************************
* NAME:
*   sw_state_occ_is_bitmap_active
* TYPE:
*   PROC
* DATE:
*   May 13 2015
* FUNCTION:
*   Get indication on whether specified bitmap is currently in use.
* INPUT:
*  int unit -
*    Identifier of the device to access.
*  sw_state_occ_bm_t bit_map -
*    Handle of the bitmap to perform the check on.
*  uint8 *in_use -
*    Return a non-zero value if bit map is in use.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
shr_error_e sw_state_occ_is_bitmap_active(
    int unit,
    sw_state_occ_bm_t bit_map,
    uint8 *in_use);

/*********************************************************************
* NAME:
*     sw_state_occ_bm_print
* TYPE:
*   PROC
* DATE:
*   Mar  6 2008
* FUNCTION:
*     print the bitmap content.
* INPUT:
*  int    unit -
*     Identifier of the device to access.
*  sw_state_occ_bm_t bit_map -
*     The bitmap to print.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
shr_error_e sw_state_occ_bm_print(
    int unit,
    sw_state_occ_bm_t bit_map);

int sw_state_occ_bm_size_get(
    sw_state_occ_bitmap_init_info_t * init_info);

/*
 * } 
 */

/*
 * } SW_STATE_OCCUPATION_BITMAP_H_INCLUDED
 */
#endif
