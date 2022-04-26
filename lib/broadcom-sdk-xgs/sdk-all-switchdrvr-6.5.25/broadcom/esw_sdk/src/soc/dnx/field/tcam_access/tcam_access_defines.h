/**
 * \file       tcam_access_defines.h
 * $Id$
 *   this file includes TCAM Access Module proprietary defines
 */
/**
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifndef TCAM_ACCESS_DEFINES_H_INCLUDED
/** { */
#define TCAM_ACCESS_DEFINES_H_INCLUDED

/**
 * Key Mode is 2 bits, see where it's used for more info.
 */
#define KEY_MODE_BIT_SIZE \
    dnx_data_field.tcam.key_mode_size_get(unit)

/**
 * Defines whether to search for the key in TCAM HW or get it from SW-STATE
 */
#define SEARCH_KEY_IN_TCAM_HW FALSE

/**
 * Specifies whether to prevent the module from accessing HW for reading/writing.
 */
#define ACCESS_HW TRUE

/**
 * Specifies whether to access the SW MOCK to save the add keys in SW or not.
 */
#define ACCESS_SW_MOCK TRUE

/*
 * half-single-double key/action bit/byte sizes defines to shorten names.
 */
#define HALF_KEY_BIT_SIZE \
    dnx_data_field.tcam.key_size_half_get(unit)
#define SINGLE_KEY_BIT_SIZE \
    dnx_data_field.tcam.key_size_single_get(unit)
#define DOUBLE_KEY_BIT_SIZE \
    dnx_data_field.tcam.key_size_double_get(unit)
#define HALF_ACTION_BIT_SIZE \
    dnx_data_field.tcam.action_size_half_get(unit)
#define SINGLE_ACTION_BIT_SIZE \
    dnx_data_field.tcam.action_size_single_get(unit)
#define DOUBLE_ACTION_BIT_SIZE \
    dnx_data_field.tcam.action_size_double_get(unit)
#define HALF_KEY_WORD_SIZE \
    (BITS2WORDS(HALF_KEY_BIT_SIZE))
#define SINGLE_KEY_WORD_SIZE \
    (BITS2WORDS(SINGLE_KEY_BIT_SIZE))
#define DOUBLE_KEY_WORD_SIZE \
    (BITS2WORDS(DOUBLE_KEY_BIT_SIZE))
#define HALF_ACTION_WORD_SIZE \
    (BITS2WORDS(HALF_ACTION_BIT_SIZE))
#define SINGLE_ACTION_WORD_SIZE \
    (BITS2WORDS(SINGLE_ACTION_BIT_SIZE))
#define DOUBLE_ACTION_WORD_SIZE \
    (BITS2WORDS(DOUBLE_ACTION_BIT_SIZE))

#define MAX_HALF_KEY_BIT_SIZE \
    DNX_DATA_MAX_FIELD_TCAM_KEY_SIZE_HALF
#define MAX_SINGLE_KEY_BIT_SIZE \
    DNX_DATA_MAX_FIELD_TCAM_KEY_SIZE_SINGLE
#define MAX_DOUBLE_KEY_BIT_SIZE \
    DNX_DATA_MAX_FIELD_TCAM_KEY_SIZE_DOUBLE
#define MAX_HALF_ACTION_BIT_SIZE \
    DNX_DATA_MAX_FIELD_TCAM_ACTION_SIZE_HALF
#define MAX_SINGLE_ACTION_BIT_SIZE \
    DNX_DATA_MAX_FIELD_TCAM_ACTION_SIZE_SINGLE
#define MAX_DOUBLE_ACTION_BIT_SIZE \
    DNX_DATA_MAX_FIELD_TCAM_ACTION_SIZE_DOUBLE
#define MAX_HALF_KEY_WORD_SIZE \
    (BITS2WORDS(MAX_HALF_KEY_BIT_SIZE))
#define MAX_SINGLE_KEY_WORD_SIZE \
    (BITS2WORDS(MAX_SINGLE_KEY_BIT_SIZE))
#define MAX_DOUBLE_KEY_WORD_SIZE \
    (BITS2WORDS(MAX_DOUBLE_KEY_BIT_SIZE))
#define MAX_HALF_ACTION_WORD_SIZE \
    (BITS2WORDS(MAX_HALF_ACTION_BIT_SIZE))
#define MAX_SINGLE_ACTION_WORD_SIZE \
    (BITS2WORDS(MAX_SINGLE_ACTION_BIT_SIZE))
#define MAX_DOUBLE_ACTION_WORD_SIZE \
    (BITS2WORDS(MAX_DOUBLE_ACTION_BIT_SIZE))

/**
 * The size of half an entry key in HW in bits.
 * Includes entry_size (mode) but not valid bits.
 */
#define HALF_KEY_BIT_SIZE_HW \
    (dnx_data_field.tcam.entry_size_single_key_hw_get(unit) / 2)

/**
 * Key mode for TCAM as defined in HW.
 * \see
 *  HW spec for TCAM key mode
 */
typedef enum
{
    DNX_FIELD_TCAM_ACCESS_KEY_MODE_INVALID = 2,
    DNX_FIELD_TCAM_ACCESS_KEY_MODE_HALF = 0,
    DNX_FIELD_TCAM_ACCESS_KEY_MODE_SINGLE = 1,
    DNX_FIELD_TCAM_ACCESS_KEY_MODE_DOUBLE = 3,
} dnx_field_tcam_access_key_mode_e;

/**
 * Enum that represents what part to write to in the key or/and payload
 */
typedef enum
{
    DNX_FIELD_TCAM_ACCESS_PART_INVALID = 0,
    DNX_FIELD_TCAM_ACCESS_PART_LSB = 1,
    DNX_FIELD_TCAM_ACCESS_PART_MSB = 2,
    DNX_FIELD_TCAM_ACCESS_PART_ALL = 3,
} dnx_field_tcam_access_part_e;

/** } */
#endif
