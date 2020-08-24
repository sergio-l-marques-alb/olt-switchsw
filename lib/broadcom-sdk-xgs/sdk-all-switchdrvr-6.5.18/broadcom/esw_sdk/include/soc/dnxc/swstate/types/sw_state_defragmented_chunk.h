/** \file sw_state_defragmented_chunk.h
 *
 * Definitions and prototypes for all common utilities related 
 * to defragmented chunk. this data type can support allocations 
 * of pieces in a chunk of memory. the way in which the pieces 
 * are allocated ensures defragmentation on the fly when needed. 
 * the allocated pieces size must be a power of 2. 
 *
 */
#ifndef SW_STATE_DEFRAGMENTED_CHUNK_H_INCLUDED
/*
 * { 
 */
#define SW_STATE_DEFRAGMENTED_CHUNK_H_INCLUDED

#ifdef BCM_DNX_SUPPORT
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

#include <soc/dnxc/swstate/types/sw_state_cb.h>
/** } */

/*************
 * DEFINES   *
 *************/
/** { */
/** } */

/*************
 * MACROS    *
 *************/
/** { */

#define SW_STATE_DEFRAGMENTED_CHUNK_CREATE(module_id, defragmented_chunk, init_info, chunk_size, alloc_flags)\
    SHR_IF_ERR_EXIT(sw_state_defragmented_chunk_create(unit, module_id, &defragmented_chunk, init_info, chunk_size, alloc_flags))

#define SW_STATE_DEFRAGMENTED_CHUNK_DESTROY(module_id, defragmented_chunk)\
    SHR_IF_ERR_EXIT(sw_state_defragmented_chunk_destroy(unit, module_id, &(defragmented_chunk)))

#define SW_STATE_DEFRAGMENTED_CHUNK_PIECE_ALLOC(module_id, defragmented_chunk, slots_in_piece, move_cb_additional_info, piece_offset)\
    SHR_IF_ERR_EXIT(sw_state_defragmented_chunk_piece_alloc(unit, module_id, defragmented_chunk, slots_in_piece, move_cb_additional_info, piece_offset))

#define SW_STATE_DEFRAGMENTED_CHUNK_PIECE_FREE(module_id, defragmented_chunk, piece_offset)\
    SHR_IF_ERR_EXIT(sw_state_defragmented_chunk_piece_free(unit, module_id, defragmented_chunk, piece_offset))

#define SW_STATE_DEFRAGMENTED_CHUNK_PRINT(module_id, defragmented_chunk)\
    SHR_IF_ERR_EXIT(sw_state_defragmented_chunk_print(unit, module_id, defragmented_chunk))

#define SW_STATE_DEFRAGMENTED_CHUNK_DEFAULT_PRINT(unit, module_id, defragmented_chunk)\
    sw_state_defragmented_chunk_print(unit, module_id, *(defragmented_chunk))

#define SW_STATE_DEFRAGMENTED_CHUNK_SIZE_GET(init_info)\
    sw_state_defragmented_chunk_size_get(init_info)
/** } */

/*************
 * TYPE DEFS *
 *************/
/** { */

/**
 * Includes the information user has to supply for defragmented chunk creation
 */
typedef struct
{
    /**
     * the size of the chunk that needs to be handled as defragmented chunk
     */
    uint32 chunk_size;
    /**
     * move function to use in case a defrag procidure is required
     * Set to NULL if no additional actions are required upon moving entries - this is usually not the case so make sure you understand properly.
     */
    char move_function[SW_STATE_CB_DB_NAME_STR_SIZE];
} sw_state_defragmented_chunk_init_info_t;

/**
 * Structure containing the actual defragmented chunk
 */
typedef struct
{
    /**
     * Array to include the chunk's slots, every entry is a slot - the value entered to
     * each slot represents size of this piece in  the following way:
     * the value is the exponent to a base of 2 and the reulted power is the size
     * (0 is size 1, 1 is size 2, 2 is size 4, 3 is size 8 etc')
     * a dedicated value (all ones) exists for empty entries.
     * unused slots contains the value 0
     */
    uint8 *slots_array;
    /**
     * remaining available slots to allocate. this is managed when allocating/freeing slots
     */
    uint32 free_slots_available;
    /*
     * information user has to supply for defragmented chunk creation
     */
    sw_state_defragmented_chunk_init_info_t init_info;
    /**
     * move functions cb_db to use in case a defrag procidure is required.
     */
    
    sw_state_cb_t move_cb_db;
}  *sw_state_defragmented_chunk_t;

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
 * \brief
 *   Creates a new defragmented chunk instance.
 *
 * \param [in] unit - Identifier of the device to access.
 * \param [in] module_id - Module ID.
 * \param [in] defragmented_chunk - Defragmented chunk to be created.
 * \param [in] init_info - Pointed memory contains setup parameters required for for the
 *     creation of the chunk.
 * \param [in] chunk_size - Defragmented chuck size. Specified in the relevant sw state xml file.
 * \param [in] alloc_flags - SW state allocation flags.
 *        This flags value is determend by the variable type (dnx data for example) of the additional variables specified in the xml.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e sw_state_defragmented_chunk_create(
    int unit,
    uint32 module_id,
    sw_state_defragmented_chunk_t * defragmented_chunk,
    sw_state_defragmented_chunk_init_info_t * init_info,
    uint32 chunk_size,
    uint32 alloc_flags);

/**
 * \brief
 *   Free the specified chunk instance.
 *
 * \param [in] unit - Identifier of the device to access.
 * \param [in] module_id - Module ID.
 * \param [in] defragmented_chunk - Defragmented chunk to be destroyed.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e sw_state_defragmented_chunk_destroy(
    int unit,
    uint32 module_id,
    sw_state_defragmented_chunk_t * defragmented_chunk);

/**
 * \brief - allocate a piece in a defragmented chunk. the piece 
 *        consists of slots_in_piece slots. the alloc function
 *        will perform defragmentation if needed.
 * 
 * \param [in] unit - unit number
 * \param [in] module_id - Module ID.
 * \param [in] defragmented_chunk - Defragmented chunk which to alloc.
 * \param [in] slots_in_piece - number of slots in the piece to allocate. this number must be a power of 2
 * \param [in] move_cb_additional_info - additional information for move CB.
 * \param [out] piece_offset - returned offset of the allocated piece - to be used as the handle when free function is used.
 *
 * \return
 *   shr_error_e 
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e sw_state_defragmented_chunk_piece_alloc(
    int unit,
    uint32 module_id,
    sw_state_defragmented_chunk_t defragmented_chunk,
    uint32 slots_in_piece,
    void *move_cb_additional_info,
    uint32 *piece_offset);

/**
 * \brief - free a piece in a defragmented chunk.
 * 
 * \param [in] unit - unit number
 * \param [in] module_id - Module ID.
 * \param [in] defragmented_chunk - Defragmented chunk which to free.
 * \param [in] piece_offset - hadle for the piece to free. this 
 *        handle must be a handle that was returned by the alloc
 *        function
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e sw_state_defragmented_chunk_piece_free(
    int unit,
    uint32 module_id,
    sw_state_defragmented_chunk_t defragmented_chunk,
    uint32 piece_offset);

/**
 * \brief - Print function.
 *
 * \param [in] unit - unit number
 * \param [in] module_id - Module ID.
 * \param [in] defragmented_chunk - Defragmented chunk to be printed.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e sw_state_defragmented_chunk_print(
    int unit,
    uint32 module_id,
    sw_state_defragmented_chunk_t defragmented_chunk);

/**
 * \brief
 *   Gets the size of a new defragmented chunk instance.
 *
 * \param [in] init_info - Pointed memory contains setup parameters required for for the
 *     creation of the chunk.
 *
 * \return
 *   int
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int sw_state_defragmented_chunk_size_get(
    sw_state_defragmented_chunk_init_info_t * init_info);

/**
 * \brief - move function is used in sw swstate defragmented chunk ctest
 *
 * \param [in] unit - unit number
 * \param [in] old_offset - old offset of the profile
 * \param [in] new_offset - new offset of the profile
 * \param [in] nof_reaources_profile_use - number of resources
 *        in defragmented chunk that the profile consumes
 * \param [in] move_cb_additional_info - additional info.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e sw_state_defragmented_chunk_move_test(
    int unit,
    uint32 old_offset,
    uint32 new_offset,
    uint32 nof_reaources_profile_use,
    void *move_cb_additional_info);

/** } */
/** } */
#else
/** { */
/**
 * \brief - empty dummy function that does nothing
 *
 * \param [in] unit - unit number
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e sw_state_defragmented_chunk_dummy_empty_function_to_make_compiler_happy(
    int unit);
/** } */
#endif /* BCM_DNX_SUPPORT */
/*
 * }
 */

/*
 * } UTILEX_DEFRAGMENTED_CHUNK_H_INCLUDED
 */
#endif
