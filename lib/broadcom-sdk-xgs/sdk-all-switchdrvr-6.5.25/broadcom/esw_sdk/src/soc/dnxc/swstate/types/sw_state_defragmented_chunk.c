/** \file sw_state_defragmented_chunk.c
 *
 * All common utilities related to defragmented chunks.
 * Only for use by 'DNX' code!!! (This is due to the fact that this
 * utility uses a part of SWSTATE which is assigned to DNX only.)
 *
 * Note:
 * Since some compilers do not accept empty files ("ISO C forbids an empty source file")
 * then dummy code ia added for non-DNX compilation.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SHAREDSWDNX_DEFRAGCHUNKDNX

/*************
* INCLUDES  *
*************/
/** { */
#include <shared/bsl.h>
#include <sal/core/libc.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
#include <soc/dnxc/swstate/sw_state_defs.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/swstate/types/sw_state_defragmented_chunk.h>
#include <soc/dnxc/swstate/types/sw_state_cb.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/swstate/auto_generated/access/dnx_sw_state_defragmented_chunk_access.h>

/** } */

/*************
* DEFINES   *
*************/
/** { */
/** used to mark an empty piece */
#define SW_STATE_DEFRAGMENTED_CHUNK_INVALID_EXPONENT 0xff
/** used to mark an in progress of defragmentation piece */
#define SW_STATE_DEFRAGMENTED_CHUNK_SAVED_FOR_FUTURE_USE_EXPONENT 0xfe
/** used to mark a reserved piece - reserved pieces cannot be defragmented until they are unreserved */
#define SW_STATE_DEFRAGMENTED_CHUNK_RESERVED_EXPONENT 0xfd
/** } */

/*************
*  MACROS   *
*************/
/** { */
/** } */

/*************
* TYPE DEFS *
*************/
/** { */
/** } */

 /*************
 * GLOBALS   *
 *************/
/** { */
/** } */
#ifdef BCM_DNX_SUPPORT
/** { */
/*************
* FUNCTIONS *
*************/
/** { */

/**
 * see header file
 */
shr_error_e
sw_state_defragmented_chunk_create(
    int unit,
    uint32 module_id,
    sw_state_defragmented_chunk_t * defragmented_chunk,
    sw_state_defragmented_chunk_init_info_t * init_info,
    uint32 chunk_size,
    uint32 alloc_flags)
{
    uint8 tmp_exponent;
    uint32 _chunk_size;
    SHR_FUNC_INIT_VARS(unit);

    tmp_exponent = SW_STATE_DEFRAGMENTED_CHUNK_INVALID_EXPONENT;

    if (*defragmented_chunk != NULL) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_EXISTS, "sw state defragmented_chunk create ERROR: defragmented_chunk already exist.\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    /** sanity checks */
    SHR_NULL_CHECK(init_info, _SHR_E_PARAM, "init_info");

    /** check that init_info is valid */
    if (init_info->chunk_size == 0 && chunk_size == 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid chunk_size\n");
    }

    if (chunk_size != 0)
    {
        _chunk_size = chunk_size;
    }
    else
    {
        _chunk_size = init_info->chunk_size;
    }
    DNX_SW_STATE_ALLOC(unit, module_id, *defragmented_chunk, **defragmented_chunk, /* number of elements */ 1,
                       DNXC_SW_STATE_NO_FLAGS, "sw_state defragmented_chunk");

    /** set init parameters to init struct */
    DNX_SW_STATE_MEMCPY_BASIC(unit, module_id, &((*defragmented_chunk)->init_info), init_info,
                              sizeof(sw_state_defragmented_chunk_init_info_t), 0,
                              "sw_state defragmented_chunk init_info");

    /** allocate and init chunk data */
    DNX_SW_STATE_ALLOC(unit, module_id, (*defragmented_chunk)->slots_array, uint8,
            _chunk_size, alloc_flags, "defragmented_chunk init_info");

    DNX_SW_STATE_RANGE_FILL(unit, module_id, (*defragmented_chunk)->slots_array, uint8,
                            0,
                            _chunk_size,
                            tmp_exponent,
                            0,
                            "slots_array");

    /** set free slots available  */
    DNX_SW_STATE_SET(unit, module_id, (*defragmented_chunk)->free_slots_available, _chunk_size, uint32,
                     0,
                     "free_slots_available");

    SW_STATE_CB_DB_REGISTER_CB(module_id,
            (*defragmented_chunk)->move_cb_db,
            init_info->move_function,
            sw_state_defragmented_cunk_move_cb_get_cb);

exit:
    SHR_FUNC_EXIT;
}

/**
 * see header file
 */
shr_error_e
sw_state_defragmented_chunk_destroy(
    int unit,
    uint32 module_id,
    sw_state_defragmented_chunk_t * defragmented_chunk)
{
    SHR_FUNC_INIT_VARS(unit);

    /** free slots array*/
    DNX_SW_STATE_FREE(unit, module_id, (*defragmented_chunk)->slots_array, "sw state defragmented_chunk slots_array");

    /** free chunk */
    DNX_SW_STATE_FREE(unit, module_id, (*defragmented_chunk), "sw state defragmented_chunk");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - search an empty piece in the chunk
 *
 * \param [in] unit - unit number
 * \param [in] module_id - Module ID.
 * \param [in] defragmented_chunk - Defragmented chunk.
 * \param [in] wanted_size - the wanted size of the empty piece
 * \param [out] piece_offset - returned piece offset in the
 *        chunk
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
sw_state_defragmented_chunk_empty_piece_search(
    int unit,
    uint32 module_id,
    sw_state_defragmented_chunk_t defragmented_chunk,
    uint32 wanted_size,
    uint32 *piece_offset)
{
    uint32 slot_in_piece;
    uint32 chunk_size;
    uint32 current_offset;
    uint8 exponent;

    SHR_FUNC_INIT_VARS(unit);

    /** search only in needed size offsets, if first slot is
     *  available continue to scan next slots in that offset and
     *  break if wanted number 0f consecutive slots is found */
    chunk_size = defragmented_chunk->init_info.chunk_size;
    for (current_offset = 0; current_offset <= (chunk_size - wanted_size); current_offset += wanted_size)
    {
        exponent = defragmented_chunk->slots_array[current_offset];
        if (exponent == SW_STATE_DEFRAGMENTED_CHUNK_INVALID_EXPONENT)
        {
            /** at this point we know that the slot in this offset is
             *  available, now need to make sure that the rest of the
             *  piece is also available */
            *piece_offset = current_offset;
            for (slot_in_piece = 0; slot_in_piece < wanted_size; ++slot_in_piece)
            {
                exponent = defragmented_chunk->slots_array[current_offset + slot_in_piece];
                if (exponent != SW_STATE_DEFRAGMENTED_CHUNK_INVALID_EXPONENT)
                {
                    break;
                }
            }

            if (slot_in_piece == wanted_size)
            {
                /** if reached end of the loop without failing it means that
                 *  an available place was found for the piece */
                SHR_EXIT();
            }
        }
    }

    /** if managed to finish loop it means that a value was not
     *  found, need to exit with NOT_FOUND error */
    SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - search for the emptiest piece of given size in the
 *        chunk. if a piece contains a SW_STATE_DEFRAGMENTED_CHUNK_RESERVED_PIECE_EXPONENT slot
 *        it cannot be picked
 *
 * \param [in] unit - unit number
 * \param [in] module_id - Module ID.
 * \param [in] defragmented_chunk - Defragmented chunk.
 * \param [in] wanted_size - wanted size of the relativly empty
 *        piece
 * \param [out] piece_offset - returned piece offset in the
 *        trunk
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
sw_state_defragmented_chunk_emptier_piece_search(
    int unit,
    uint32 module_id,
    sw_state_defragmented_chunk_t defragmented_chunk,
    uint32 wanted_size,
    uint32 *piece_offset)
{
    uint8 exponent;
    uint32 chunk_size;
    uint32 offset;
    uint32 slot;
    uint32 nof_clear_slots_in_piece;
    uint32 max_nof_clear_slots_in_piece = 0;
    SHR_FUNC_INIT_VARS(unit);

    chunk_size = defragmented_chunk->init_info.chunk_size;
    /** iterate on piece of wanted size */
    for (offset = 0; offset <= chunk_size - wanted_size; offset += wanted_size)
    {
        /** iterate over slots of wanted size */
        for (nof_clear_slots_in_piece = 0, slot = 0; slot < wanted_size; ++slot)
        {
            /** count cleared slots */
            exponent = defragmented_chunk->slots_array[offset + slot];
            if (exponent == SW_STATE_DEFRAGMENTED_CHUNK_INVALID_EXPONENT)
            {
                ++nof_clear_slots_in_piece;
            }
            else if (exponent == SW_STATE_DEFRAGMENTED_CHUNK_RESERVED_EXPONENT)
            {
                /** if piece contains a reserved slot, mark all piece as full and move to next piece */
                nof_clear_slots_in_piece = 0;
                break;
            }
        }
        /** if an emptier piece is found, save it */
        if (max_nof_clear_slots_in_piece < nof_clear_slots_in_piece)
        {
            max_nof_clear_slots_in_piece = nof_clear_slots_in_piece;
            *piece_offset = offset;
        }
    }

    if(max_nof_clear_slots_in_piece == 0)
    {
        /** reaching here means that although theoretically there are sufficient empty slot,
         * they cannot be cleared because of the reserved slots restrictions */
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "failed to find a piece that doesn't contain reserved slots\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - mark as saved the empty slots in a piece, this is to
 *        prevent placement of other pieces there during a defrag
 *        routine
 *
 * \param [in] unit - unit nubmer
 * \param [in] module_id - Module ID.
 * \param [in] defragmented_chunk - Defragmented chunk.
 * \param [in] size - size of the piece in which to mark empty
 *        slots
 * \param [in] piece_offset - offset of the piece in the chunk
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
sw_state_defragmented_chunk_empty_slots_in_piece_as_saved_mark(
    int unit,
    uint32 module_id,
    sw_state_defragmented_chunk_t defragmented_chunk,
    uint32 size,
    uint32 piece_offset)
{
    uint8 exponent, tmp_exponent;
    uint32 offset;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(defragmented_chunk, _SHR_E_PARAM, "defragmented_chunk");

    /*
     * Set the tmp_exponent to equal to the macro, because we can't copy the macro value directly.
     */
    tmp_exponent = SW_STATE_DEFRAGMENTED_CHUNK_SAVED_FOR_FUTURE_USE_EXPONENT;

    /** iterate slots in piece */
    for (offset = piece_offset; offset < piece_offset + size; ++offset)
    {
        /** if slot is empty change the value from empty to saved */
        exponent = defragmented_chunk->slots_array[offset];
        if (exponent == SW_STATE_DEFRAGMENTED_CHUNK_INVALID_EXPONENT)
        {
            DNX_SW_STATE_MEMCPY_BASIC(unit,
                    module_id,
                    &defragmented_chunk->slots_array[offset],
                    &tmp_exponent,
                    sizeof(tmp_exponent),
                    DNXC_SW_STATE_NO_FLAGS,
                    "defragmented_chunk->slots_array");
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/** internal procedure to defrag,
 *  see sw_state_defragmented_chunk_defrag,
 *  without verifications and defraged slots clearing */
static shr_error_e
sw_state_defragmented_chunk_defrag_internal(
    int unit,
    uint32 module_id,
    sw_state_defragmented_chunk_t defragmented_chunk,
    uint32 wanted_size,
    void *move_cb_additional_info,
    uint32 *piece_offset)
{
    int rv;
    uint32 iterated_size;
    uint32 offset;
    uint32 iteration_exponent;
    uint32 new_location_offset;
    uint8 exponent;
    uint8 tmp_exponent;
    sw_state_defragmented_cunk_move_cb move_cb;

    SHR_FUNC_INIT_VARS(unit);

    /** Set the tmp_exponent to equal to the macro, because we can't the macro directly. */
    tmp_exponent = SW_STATE_DEFRAGMENTED_CHUNK_SAVED_FOR_FUTURE_USE_EXPONENT;

    /** search emptier first piece */
    SHR_IF_ERR_EXIT(sw_state_defragmented_chunk_emptier_piece_search
                    (unit, module_id, defragmented_chunk, wanted_size, piece_offset));
    /** mark empty slots in found piece as saved */
    SHR_IF_ERR_EXIT(sw_state_defragmented_chunk_empty_slots_in_piece_as_saved_mark
                    (unit, module_id, defragmented_chunk, wanted_size, *piece_offset));
    /** iterate over all subset of sizes in the defragmented
     *  chunk and clear taken slots */
    iterated_size = wanted_size;
    do
    {
        iterated_size /= 2;
        iteration_exponent = utilex_log2_round_down(iterated_size);
        /** iterate current iteration size and move taken slots */
        for (offset = *piece_offset; offset < (*piece_offset + wanted_size); offset += iterated_size)
        {
            exponent = defragmented_chunk->slots_array[offset];
            if (exponent != iteration_exponent)
            {
                continue;
            }
            /** search for a place to move piece */
            rv = sw_state_defragmented_chunk_empty_piece_search(unit, module_id, defragmented_chunk, iterated_size,
                                                                &new_location_offset);
            if (rv == _SHR_E_NOT_FOUND)
            {
                /** if not found start defrag for the little piece */
                SHR_IF_ERR_EXIT(sw_state_defragmented_chunk_defrag_internal
                                (unit, module_id, defragmented_chunk, iterated_size, move_cb_additional_info,
                                 &new_location_offset));
            }
            else
            {
                SHR_IF_ERR_EXIT(rv);
            }
            /** allocate new space */
            DNX_SW_STATE_RANGE_FILL(unit, module_id, defragmented_chunk->slots_array, uint8,
                                    new_location_offset,
                                    iterated_size,
                                    exponent,
                                    0,
                                    "slots_array");
            /** run move CB */
            rv = sw_state_defragmented_cunk_move_cb_get_cb(&defragmented_chunk->move_cb_db, 1 /*dry run*/, NULL);
            SW_STATE_CB_DB_GET_CB(module_id,
                    defragmented_chunk->move_cb_db,
                    &move_cb,
                    sw_state_defragmented_cunk_move_cb_get_cb);

            if (rv != _SHR_E_NOT_FOUND )
            {
                SHR_IF_ERR_EXIT((*move_cb) (unit, offset, new_location_offset, iterated_size, move_cb_additional_info));
            }
            /** mark emptied cells as saved */
            DNX_SW_STATE_RANGE_FILL(unit, module_id, defragmented_chunk->slots_array, uint8,
                                    offset,
                                    iterated_size,
                                    tmp_exponent,
                                    0,
                                    "slots_array");
        }
    }
    while (iterated_size > 1);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
sw_state_defragmented_chunk_defrag(
    int unit,
    uint32 module_id,
    sw_state_defragmented_chunk_t defragmented_chunk,
    uint32 wanted_size,
    void *move_cb_additional_info,
    uint32 *piece_offset)
{
    uint8 exponent;
    uint32 free_slots_available;
    SHR_FUNC_INIT_VARS(unit);

    /** check if there's sufficient available free slots in chunk for wanted size without addressing
     * reservation limitations (offset that contains reserved slot cannot be defragmented) */
    free_slots_available = defragmented_chunk->free_slots_available;
    if (wanted_size > free_slots_available)
    {
        SHR_ERR_EXIT(_SHR_E_FULL, "Chunk is too full, no place for piece of size %d\n", wanted_size);
    }

    /** verify that wanted_size is a power of 2 */
    if (!utilex_is_power_of_2(wanted_size))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid wanted_size, has to be a power of 2\n");
    }

    /** actual defrag */
    SHR_IF_ERR_EXIT(sw_state_defragmented_chunk_defrag_internal(unit, module_id, defragmented_chunk, wanted_size, move_cb_additional_info, piece_offset));

    /** mark slots as cleared (cleanup for defrag routine) - nof free slots is not changed due to a defrag */
    exponent = SW_STATE_DEFRAGMENTED_CHUNK_INVALID_EXPONENT;
    DNX_SW_STATE_RANGE_FILL(unit, module_id, defragmented_chunk->slots_array, uint8,
                            *piece_offset,
                            wanted_size,
                            exponent,
                            0,
                            "slots_array");
exit:
    SHR_FUNC_EXIT;
}

/**
 * see header file
 */
shr_error_e
sw_state_defragmented_chunk_offset_reserve(
    int unit,
    uint32 module_id,
    sw_state_defragmented_chunk_t defragmented_chunk,
    uint32 slots_in_offset,
    uint32 piece_offset)
{
    uint32 chunk_size;
    uint32 current_offset;
    uint32 empty_count = 0;
    uint32 tmp_free_slots_available;
    uint8 exponent;
    SHR_FUNC_INIT_VARS(unit);

    /** verify that nof of slots in the allocated piece is valid
     *  - a power of 2
     *  - can fit in chunk
     *  - offset is valid relative to size of piece */
    if (!utilex_is_power_of_2(slots_in_offset))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid slots_in_offset size, has to be a power of 2\n");
    }

    chunk_size = defragmented_chunk->init_info.chunk_size;
    if (piece_offset + slots_in_offset > chunk_size)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "requested piece to reserve does not fit in chunk\n");
    }

    if(piece_offset % slots_in_offset != 0)
    {
       SHR_ERR_EXIT(_SHR_E_PARAM, "provided piece offset is invalid for piece size");
    }

    /** go over piece and make sure operation is allowed */
    for (current_offset = piece_offset; current_offset < piece_offset + slots_in_offset; ++current_offset)
    {
        exponent = defragmented_chunk->slots_array[current_offset];
        /** slots have to be either empty or already reserved */
        if (    (exponent != SW_STATE_DEFRAGMENTED_CHUNK_INVALID_EXPONENT) && (exponent != SW_STATE_DEFRAGMENTED_CHUNK_RESERVED_EXPONENT))
        {
            SHR_ERR_EXIT(_SHR_E_RESOURCE, "requested piece to reserve is not empty\n");
        }

        /** empty slots has to be counted so once they are changed to reserved - they can be decreased from count */
        if(exponent == SW_STATE_DEFRAGMENTED_CHUNK_INVALID_EXPONENT)
        {
            ++empty_count;
        }
    }

    /** reserve piece:
     *  - remove used slots from available slots count
     *  - fill used slots with exponent value of reserved exponent */
    tmp_free_slots_available = defragmented_chunk->free_slots_available - empty_count;
    DNX_SW_STATE_MEMCPY_BASIC(unit,
            module_id,
            &defragmented_chunk->free_slots_available,
            &tmp_free_slots_available,
            sizeof(tmp_free_slots_available),
            DNXC_SW_STATE_NO_FLAGS,
            "defragmented_chunk->free_slots_available");

    exponent = SW_STATE_DEFRAGMENTED_CHUNK_RESERVED_EXPONENT;
    DNX_SW_STATE_RANGE_FILL(unit, module_id, defragmented_chunk->slots_array, uint8,
                            piece_offset,
                            slots_in_offset,
                            exponent,
                            0,
                            "slots_array");

exit:
    SHR_FUNC_EXIT;
}

/**
 * see header file
 */
shr_error_e
sw_state_defragmented_chunk_offset_unreserve(
    int unit,
    uint32 module_id,
    sw_state_defragmented_chunk_t defragmented_chunk,
    uint32 slots_in_offset,
    uint32 piece_offset)
{
    uint32 chunk_size;
    uint32 current_offset;
    uint32 tmp_free_slots_available;
    uint8 exponent;
    SHR_FUNC_INIT_VARS(unit);

    /** verify that nof of slots in the allocated piece is valid
     *  - a power of 2
     *  - can fit in chunk
     *  - offset is valid relative to size of piece */
    if (!utilex_is_power_of_2(slots_in_offset))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid slots_in_offset size, has to be a power of 2\n");
    }

    chunk_size = defragmented_chunk->init_info.chunk_size;
    if (piece_offset + slots_in_offset > chunk_size)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "requested piece to unreserve does not fit in chunk\n");
    }

    if(piece_offset % slots_in_offset != 0)
    {
       SHR_ERR_EXIT(_SHR_E_PARAM, "provided piece offset is invalid for piece size");
    }

    /** go over piece and make sure operation is allowed */
    for (current_offset = piece_offset; current_offset < piece_offset + slots_in_offset; ++current_offset)
    {
        exponent = defragmented_chunk->slots_array[current_offset];
        /** slots have to be already reserved */
        if (exponent != SW_STATE_DEFRAGMENTED_CHUNK_RESERVED_EXPONENT)
        {
            SHR_ERR_EXIT(_SHR_E_RESOURCE, "requested piece to unreserve is not fully reserved\n");
        }
    }

    /** unreserve piece:
     *  - add unreserved slots to available slots count
     *  - fill unreserved slots with exponent value of invalid exponent */
    tmp_free_slots_available = defragmented_chunk->free_slots_available + slots_in_offset;
    DNX_SW_STATE_MEMCPY_BASIC(unit,
            module_id,
            &defragmented_chunk->free_slots_available,
            &tmp_free_slots_available,
            sizeof(tmp_free_slots_available),
            DNXC_SW_STATE_NO_FLAGS,
            "defragmented_chunk->free_slots_available");

    exponent = SW_STATE_DEFRAGMENTED_CHUNK_INVALID_EXPONENT;
    DNX_SW_STATE_RANGE_FILL(unit, module_id, defragmented_chunk->slots_array, uint8,
                            piece_offset,
                            slots_in_offset,
                            exponent,
                            0,
                            "slots_array");

exit:
    SHR_FUNC_EXIT;
}

/**
 * see header file
 */
shr_error_e
sw_state_defragmented_chunk_piece_alloc(
    int unit,
    uint32 module_id,
    sw_state_defragmented_chunk_t defragmented_chunk,
    uint32 slots_in_piece,
    void *move_cb_additional_info,
    uint32 *piece_offset)
{
    int rv;
    uint8 exponent;
    uint32 chunk_size;
    uint32 free_slots_available;
    uint32 tmp_free_slots_available;
    SHR_FUNC_INIT_VARS(unit);

    /** verify that nof of slots in the allocated piece is valid
     *  - a power of 2
     *  - not bigger then allocated chunk size */
    if (!utilex_is_power_of_2(slots_in_piece))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid slots_in_piece size, has to be a power of 2\n");
    }

    chunk_size = defragmented_chunk->init_info.chunk_size;
    if (slots_in_piece > chunk_size)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid slots_in_piece size, bigger the total chunk size\n");
    }

    /** verify that there is sufficant place availavle in chunk */
    free_slots_available = defragmented_chunk->free_slots_available;
    if (slots_in_piece > free_slots_available)
    {
        SHR_ERR_EXIT(_SHR_E_FULL, "Chunk is full, no place for piece of size %d\n", slots_in_piece);
    }

    /** search an empty piece in chunk */
    rv = sw_state_defragmented_chunk_empty_piece_search(unit, module_id, defragmented_chunk, slots_in_piece,
                                                        piece_offset);
    if (rv == _SHR_E_NOT_FOUND)
    {
        /** if not found an empty spot run defrag to clear one */
        SHR_IF_ERR_EXIT(sw_state_defragmented_chunk_defrag_internal
                        (unit, module_id, defragmented_chunk, slots_in_piece, move_cb_additional_info, piece_offset));
    }
    else
    {
        SHR_IF_ERR_EXIT(rv);
    }

    /** allocate piece:
     *  - remove used slots from available slots count
     *  - fill used slots with exponent value of piece size */
    tmp_free_slots_available = free_slots_available - slots_in_piece;
    DNX_SW_STATE_MEMCPY_BASIC(unit,
            module_id,
            &defragmented_chunk->free_slots_available,
            &tmp_free_slots_available,
            sizeof(tmp_free_slots_available),
            DNXC_SW_STATE_NO_FLAGS,
            "defragmented_chunk->free_slots_available");

    exponent = utilex_log2_round_down(slots_in_piece);
    DNX_SW_STATE_RANGE_FILL(unit, module_id, defragmented_chunk->slots_array, uint8,
                            *piece_offset,
                            slots_in_piece,
                            exponent,
                            0,
                            "slots_array");

exit:
    SHR_FUNC_EXIT;
}

/**
 * see header file
 */
shr_error_e
sw_state_defragmented_chunk_piece_free(
    int unit,
    uint32 module_id,
    sw_state_defragmented_chunk_t defragmented_chunk,
    uint32 piece_offset)
{
    uint32 chunk_size;
    uint32 nof_free_slots;
    uint8 exponent, tmp_exponent;
    uint32 piece_size;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Set the tmp_exponent to equal to the macro, because we can't the macro directly.
     */
    tmp_exponent = SW_STATE_DEFRAGMENTED_CHUNK_INVALID_EXPONENT;

    /** Check piece offset is valid and can fit to chunk */
    chunk_size = defragmented_chunk->init_info.chunk_size;
    if (piece_offset >= chunk_size)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid piece_offset value - " "provided offset is too big for provided chunk\n");
    }

    /** Check the value in the first slot in given piece_offset. the
     *  value represents the size of the piece (the exponent to the
     *  base 2 - 0 is corresponding to size 1, 1 to 2, 2 to 4, 3 to 8
     *  etc', Invalid value which means an empty entry, has a
     *  dedicated value) and all of the slots in the piece will have
     *  the same value. the offset should be the offset of the first
     *  slot of the piece. this can be checked using modulus between
     *  the offset and value - valid result should be zero. */

    exponent = defragmented_chunk->slots_array[piece_offset];
    if (exponent == SW_STATE_DEFRAGMENTED_CHUNK_INVALID_EXPONENT)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Invalid piece offset value - " "provided offset was not allocated\n");
    }
    piece_size = 1 << exponent;
    if (piece_offset % piece_size != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid piece_offset value - "
                     "provided offset is not the offset of the first slot in piece\n");
    }

    /** clear slots */
    DNX_SW_STATE_RANGE_FILL(unit, module_id, defragmented_chunk->slots_array, uint8,
                            piece_offset,
                            piece_size,
                            tmp_exponent,
                            0,
                            "slots_array");

    /** increase free slots count by piece size */
    nof_free_slots = defragmented_chunk->free_slots_available + piece_size;

    DNX_SW_STATE_MEMCPY_BASIC(unit, module_id, &defragmented_chunk->free_slots_available, &nof_free_slots, sizeof(nof_free_slots), DNXC_SW_STATE_NO_FLAGS, "defragmented_chunk->free_slots_available");

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
sw_state_defragmented_chunk_print(
    int unit,
    uint32 module_id,
    sw_state_defragmented_chunk_t defragmented_chunk)
{
    uint32 i;
    uint32 chunk_size;
    uint8 exponent;

    SHR_FUNC_INIT_VARS(unit);

    if(defragmented_chunk == NULL) {
        DNX_SW_STATE_PRINT(unit, "NULL\n");
        goto exit;
    }

    chunk_size = defragmented_chunk->init_info.chunk_size;
    for (i = 0; i < chunk_size; ++i)
    {
        DNX_SW_STATE_DUMP_UPDATE_CURRENT_IDX(unit, i);
        exponent = defragmented_chunk->slots_array[i];
        DNX_SW_STATE_PRINT_WITH_STRIDE_UPDATE(unit, exponent, "offset %03d: %d\n", i, exponent);
    }
    DNX_SW_STAET_DUMP_END_OF_STRIDE(unit);

exit:
    SHR_FUNC_EXIT;
}

int sw_state_defragmented_chunk_size_get(
    sw_state_defragmented_chunk_init_info_t * init_info)
{
    int size = 0;

    size += (sizeof(sw_state_defragmented_chunk_t));

    size += (sizeof(uint8) * init_info->chunk_size);

    return size;
}

shr_error_e
sw_state_defragmented_chunk_move_test(
    int unit,
    uint32 old_offset,
    uint32 new_offset,
    uint32 nof_reaources_profile_use,
    void *move_cb_additional_info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/** } */
/** } */
#else
/** { */
/**
 * This is DUMMY code. It is only for compilers that do not accept empty files
 * and is never to be used.
 */
shr_error_e
sw_state_defragmented_chunk_dummy_empty_function_to_make_compiler_happy(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_FUNC_EXIT;
}
/** } */
#endif
