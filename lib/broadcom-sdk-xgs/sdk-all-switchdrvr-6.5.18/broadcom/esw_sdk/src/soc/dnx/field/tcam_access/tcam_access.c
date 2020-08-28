/**
 * \file       tcam_access.c
 * $Id$
 *   This file implements all the necessary logic for the TCAM access layer.
 *   The TCAM access layer is responsible for writing/reading raw data to
 */
/**
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */


#include <bcm_int/dnx/field/tcam/tcam_location_algo.h>
#include <bcm_int/dnx/field/field_entry.h>
#include <bcm_int/dnx/field/tcam/field_tcam.h>

#include <soc/dnx/field/tcam_access/tcam_access.h>

#include <soc/dnx/swstate/auto_generated/access/dnx_field_tcam_access_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_tcam_access_mapper_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_tcam_access_profile_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_tcam_cache_access.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_types.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_tcam_location_types.h>

#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_field.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_field.h>

#include <shared/utilex/utilex_bitstream.h>
#include <soc/register.h>
#include <soc/feature.h>
#include <soc/drv.h> 

#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDPROCDNX

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

#define DNX_FIELD_TCAM_IS_STAGE_PMF(stage) \
    (((stage) == DNX_FIELD_TCAM_STAGE_IPMF1) || \
    ((stage) == DNX_FIELD_TCAM_STAGE_IPMF2) || \
    ((stage) == DNX_FIELD_TCAM_STAGE_IPMF3) || \
    ((stage) == DNX_FIELD_TCAM_STAGE_EPMF))

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

/**
 * This struct contains the information needed by TCAM Access module
 * regarding a certain database (field group).
 */
typedef struct
{
        /**
     * Whether this database is Direct Table or not
     */
    uint8 direct_table;
    /**
     * The size of the TCAM key needed by this database in bits.
     * In JR2, there are 3 fixed TCAM key sizes:
     * 1) 80b  (half key)
     * 2) 160b (single key)
     * 3) 320b (double key)
     */
    uint32 key_size;
    /**
     * The size of the TCAM payload needed by this database in bits.
     * In JR2, there are 3 fixed TCAM payload sizes:
     * 1) 32b  (half payload)
     * 2) 64b  (single payload)
     * 3) 128b (double payload)
     */
    uint32 payload_size;
    /**
     * The prefix value used by this database.
     */
    uint32 prefix_value;
    /**
     * The prefix size in bits for the prefix used by this database (up to 5b).
     */
    uint32 prefix_size;
    /*
     * The ID of the bank used by this DT database
     */
    int dt_bank_id;
    /*
     * Stage of the DB
     */
    dnx_field_tcam_stage_e stage;

} dnx_field_tcam_access_db_info;

/**
 * \brief
 *  Gets the database info for the given field group ID.
 *
 * \param [in] unit     - Device ID
 * \param [in] fg_id    - The field group ID to get the database info for
 * \param [out] db_info - The database info for the given fg_id
 *
 * \return
 *   \retval _SHR_E_NONE success
 *
 * \remark
 * SPECIAL INPUT:
 *      SWSTATE:
 *         dnx_field_tcam_access_sw.fg_params saves params for each field group
 */
static shr_error_e
dnx_field_tcam_access_info_get(
    int unit,
    int fg_id,
    dnx_field_tcam_access_db_info * db_info)
{
    dnx_field_tcam_access_fg_params_t fg_params;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.get(unit, fg_id, &fg_params));
    db_info->key_size = fg_params.key_size;
    db_info->payload_size = fg_params.action_size;
    db_info->prefix_value = fg_params.prefix_value;
    db_info->prefix_size = fg_params.prefix_size;
    db_info->direct_table = fg_params.direct_table;
    db_info->dt_bank_id = fg_params.dt_bank_id;
    db_info->stage = fg_params.stage;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Returns the position for the given access ID on the given core.
 *
 * \param [in] unit      - Device ID
 * \param [in] core      - Core ID
 * \param [in] access_id - The access ID to get the postion for
 * \param [out] position - The postion for the given access_id, this postion is
 *                         composed out of 2 elements: (bank_id, bank_offset)
 *                         by the following formula:
 *                              position = (bank_id * BANK_SIZE) + bank_offset
 *
 * \return
 *   \retval _SHR_E_NONE success
 *
 * \remark
 * SPECIAL INPUT:
 *      SWSTATE:
 *         dnx_field_tcam_access_sw.entry_location_hash maps every entry to its
 *         corresponding location
 */
static shr_error_e
dnx_field_tcam_access_index_get(
    int unit,
    int core,
    uint32 access_id,
    uint32 *position)
{
    uint8 found;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get entry position from entry_location_hash SWSTATE 
     */
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.entry_location_hash.find(unit, core, &access_id, position, &found));

    if (!found)
    {
        LOG_DEBUG_EX(BSL_LOG_MODULE, "Entry with access_id: %d not found in entry->location Hash %s%s%s\n", access_id,
                     EMPTY, EMPTY, EMPTY);
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Reads a single size key/key_mask from the shadow of the given core
 *
 * \param [in] unit              - Device ID
 * \param [in] core              - Core ID
 * \param [in] bank_id           - The bank ID to read from
 * \param [in] tcam_bank_offset  - The offset inside the bank to write to, twice the action bank offset.
 * \param [out] data             - The buffer to copy read data to.
 * \param [out] valid_bits       - The entry's valid bits.
 *
 * \return
 *   \retval _SHR_E_NONE success
 *
 * \remark
 * SPECIAL INPUT:
 *  * None.
 */
static shr_error_e
dnx_field_tcam_access_entry_key_read_shadow(
    int unit,
    int core,
    uint32 bank_id,
    uint32 tcam_bank_offset,
    uint32 *data,
    uint8 *valid_bits)
{
    uint8 shadow_data[DNX_FIELD_TCAM_ENTRY_KEY_HW_SIZE_MAX_ALL_DEVICE];
    uint32 shadow_data_32[BYTES2WORDS(DNX_FIELD_TCAM_ENTRY_KEY_HW_SIZE_MAX_ALL_DEVICE)];
    uint32 entry_half_key_size;
    /** Vaild bits are an array instead of a pointer due to coverity error of bitrange treating it as an array.*/
    uint32 valid_bits_arr[1];

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(shadow_data, 0, sizeof(shadow_data));
    entry_half_key_size = HALF_KEY_BIT_SIZE_HW;

    SHR_IF_ERR_EXIT(dnx_field_tcam_cache_shadow.tcam_bank.
                    entry_key.range_read(unit, core, bank_id, tcam_bank_offset, DNX_FIELD_TCAM_ENTRY_KEY_HW_SIZE,
                                         shadow_data));

    SHR_IF_ERR_EXIT(utilex_U8_to_U32(shadow_data, DNX_FIELD_TCAM_ENTRY_KEY_HW_SIZE, shadow_data_32));

    valid_bits_arr[0] = 0;
    SHR_BITCOPY_RANGE(valid_bits_arr, 0, shadow_data_32, 2 * entry_half_key_size, 2);
    (*valid_bits) = valid_bits_arr[0];

    SHR_BITCOPY_RANGE(data, 0, shadow_data_32, 0, 2 * entry_half_key_size);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Writes a single size key/key_mask to the shadow of the given core
 *
 * \param [in] unit              - Device ID
 * \param [in] core              - Core ID
 * \param [in] bank_id           - The bank ID to write to
 * \param [in] tcam_bank_offset  - The offset inside the bank to write to, twice the action bank offset.
 * \param [in] valid             - 2 bit valid indication for both key parts
 * \param [in] part              - The part of the entry to write to.
 * \param [in] data              - The buffer to copy write data to.

 *
 * \return
 *   \retval _SHR_E_NONE success
 *
 * \remark
 * SPECIAL INPUT:
 *  * Writes to TCAM shadow.
 */
static shr_error_e
dnx_field_tcam_access_entry_key_write_shadow(
    int unit,
    int core,
    uint32 bank_id,
    uint32 tcam_bank_offset,
    int valid,
    dnx_field_tcam_access_part_e part,
    uint32 *data)
{
    uint8 shadow_data[DNX_FIELD_TCAM_ENTRY_KEY_HW_SIZE_MAX_ALL_DEVICE];
    uint32 shadow_data_32[BYTES2WORDS(DNX_FIELD_TCAM_ENTRY_KEY_HW_SIZE_MAX_ALL_DEVICE)];
    uint8 shadow_read_data[DNX_FIELD_TCAM_ENTRY_KEY_HW_SIZE_MAX_ALL_DEVICE];
    uint32 entry_half_key_size;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(shadow_data, 0, sizeof(shadow_data));
    sal_memset(shadow_data_32, 0, sizeof(shadow_data_32));
    sal_memset(shadow_read_data, 0, sizeof(shadow_read_data));
    entry_half_key_size = HALF_KEY_BIT_SIZE_HW;

    if (part == DNX_FIELD_TCAM_ACCESS_PART_LSB || part == DNX_FIELD_TCAM_ACCESS_PART_MSB)
    {
        /*
         * We have a partial write, and therefore have to read the entry before.
         */
        SHR_IF_ERR_EXIT(dnx_field_tcam_cache_shadow.tcam_bank.
                        entry_key.range_read(unit, core, bank_id, tcam_bank_offset, DNX_FIELD_TCAM_ENTRY_KEY_HW_SIZE,
                                             shadow_read_data));
        SHR_IF_ERR_EXIT(utilex_U8_to_U32(shadow_read_data, DNX_FIELD_TCAM_ENTRY_KEY_HW_SIZE, shadow_data_32));
    }

    switch (part)
    {
        case DNX_FIELD_TCAM_ACCESS_PART_LSB:
        {
            SHR_BITCOPY_RANGE(shadow_data_32, 0, data, 0, entry_half_key_size);
            SHR_BITWRITE(shadow_data_32, 2 * entry_half_key_size, valid & DNX_FIELD_TCAM_ACCESS_PART_LSB);
            break;
        }
        case DNX_FIELD_TCAM_ACCESS_PART_MSB:
        {
            SHR_BITCOPY_RANGE(shadow_data_32, entry_half_key_size, data, entry_half_key_size, entry_half_key_size);
            SHR_BITWRITE(shadow_data_32, 2 * entry_half_key_size + 1, (valid & DNX_FIELD_TCAM_ACCESS_PART_MSB) >> 1);
            break;
        }
        case DNX_FIELD_TCAM_ACCESS_PART_ALL:
        {
            SHR_BITCOPY_RANGE(shadow_data_32, 0, data, 0, 2 * entry_half_key_size);
            SHR_BITWRITE(shadow_data_32, 2 * entry_half_key_size, valid & DNX_FIELD_TCAM_ACCESS_PART_LSB);
            SHR_BITWRITE(shadow_data_32, 2 * entry_half_key_size + 1, (valid & DNX_FIELD_TCAM_ACCESS_PART_MSB) >> 1);
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unknown entry part: %d.\n", part);
            break;
        }
    }

    SHR_IF_ERR_EXIT(utilex_U32_to_U8(shadow_data_32, DNX_FIELD_TCAM_ENTRY_KEY_HW_SIZE, shadow_data));

    SHR_IF_ERR_EXIT(dnx_field_tcam_cache_shadow.tcam_bank.
                    entry_key.range_write(unit, core, bank_id, tcam_bank_offset, DNX_FIELD_TCAM_ENTRY_KEY_HW_SIZE,
                                          shadow_data));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Reads a single entry payload from the shadow of the given core
 *
 * \param [in] unit               - Device ID
 * \param [in] core               - Core ID
 * \param [in] action_bank_id     - The actiont able to write to (there are two for each bank ID).
 * \param [in] action_bank_offset - The offset inside the bank to read from. Half the offset in the TCAM bank.
 * \param [out] payload           - The buffer to copy read data to (should be
 *                                  pre-allocated to at least standard payload size)
 *
 * \return
 *   \retval _SHR_E_NONE success
 *
 * \remark
 *  * None.
 */
static shr_error_e
dnx_field_tcam_access_entry_payload_read_shadow(
    int unit,
    int core,
    uint32 action_bank_id,
    uint32 action_bank_offset,
    uint32 *payload)
{
    uint8 shadow_data[DNX_FIELD_TCAM_ENTRY_HALF_PAYLOAD_HW_SIZE_MAX_ALL_DEVICE];

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(shadow_data, 0, sizeof(shadow_data));

    SHR_IF_ERR_EXIT(dnx_field_tcam_cache_shadow.payload_table.
                    entry_payload.range_read(unit, core, action_bank_id, action_bank_offset,
                                             DNX_FIELD_TCAM_ENTRY_HALF_PAYLOAD_HW_SIZE, shadow_data));

    SHR_IF_ERR_EXIT(utilex_U8_to_U32(shadow_data, DNX_FIELD_TCAM_ENTRY_HALF_PAYLOAD_HW_SIZE, payload));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Writes a single size payload to the shadow of the given core
 *
 * \param [in] unit               - Device ID
 * \param [in] core               - Core ID
 * \param [in] action_bank_id     - The actiont able to write to (there are two for each bank ID).
 * \param [in] action_bank_offset - The offset inside the bank to read from. Half the offset in the TCAM bank.
 * \param [in] payload            - The buffer to copy write data to (should be
 *                                  pre-allocated to at least standard payload size)
 *
 * \return
 *   \retval _SHR_E_NONE success
 *
 * \remark
 * SPECIAL INPUT:
 *  * Wrties to TCAM shadow
 */
static shr_error_e
dnx_field_tcam_access_entry_payload_write_shadow(
    int unit,
    int core,
    uint32 action_bank_id,
    uint32 action_bank_offset,
    uint32 *payload)
{
    uint8 shadow_data[DNX_FIELD_TCAM_ENTRY_HALF_PAYLOAD_HW_SIZE_MAX_ALL_DEVICE];

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(shadow_data, 0, sizeof(shadow_data));

    SHR_IF_ERR_EXIT(utilex_U32_to_U8(payload, DNX_FIELD_TCAM_ENTRY_HALF_PAYLOAD_HW_SIZE, shadow_data));

    SHR_IF_ERR_EXIT(dnx_field_tcam_cache_shadow.payload_table.
                    entry_payload.range_write(unit, core, action_bank_id, action_bank_offset,
                                              DNX_FIELD_TCAM_ENTRY_HALF_PAYLOAD_HW_SIZE, shadow_data));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Reads a single size key/key_mask from TCAM HW of the given core.
 *
 * \param [in] unit        - Device ID
 * \param [in] core        - Core ID
 * \param [in] bank_id     - The TCAM_BANK ID to read from
 * \param [in] bank_offset - The offset inside the TCAM_BANK to read from, by counting Single sized entries.
 *                           Equal to the TCAM bank offset, twice the action bank offset.
 * \param [in] part        - Part of the key to read
 * \param [out] key        - The key buffer to read to (should be pre-allocated
 *                           to at least single key size)
 * \param [out] key_mask   - The key_mask buffer to read to (should be
 *                           pre-allocated to at least single key size)
 * \param [out] valid_bits - Two bits entry valid (bit for each half part)
 *
 * \return
 *   \retval _SHR_E_NONE success
 *
 * \remark
 * SPECIAL INPUT:
 *     HW:
 *        TCAM_TCAM_BANK_REPLY - The memory to read the data from
 * SPECIAL OUTPUT:
 *     HW:
 *        TCAM_TCAM_BANK_COMMAND - The memory to write data command to
 */
static shr_error_e
dnx_field_tcam_access_entry_key_read(
    int unit,
    int core,
    uint32 bank_id,
    uint32 bank_offset,
    dnx_field_tcam_access_part_e part,
    uint32 *key,
    uint32 *key_mask,
    uint8 *valid_bits)
{
    /*
     * +1 to have extra space for mode
     */
    uint32 data_key[(MAX_SINGLE_KEY_WORD_SIZE + 1)] = { 0 };
    uint32 data_mask[(MAX_SINGLE_KEY_WORD_SIZE + 1)] = { 0 };
    soc_reg_above_64_val_t mem_val_read;
    soc_reg_above_64_val_t mem_val_write;
    uint32 tcam_bank_address;
    uint8 key_valid_bits;
    uint8 shadow_valid_bits;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * We now find the position within the TCAM table.
     */
    tcam_bank_address = DNX_FIELD_TCAM_LOCATION_BANK_TO_ABSOLUTE(bank_id, bank_offset);

    if (DNX_FIELD_TCAM_SHADOW_READ)
    {
        /*
         * Read key from shadow.
         */
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_entry_key_read_shadow
                        (unit, core, bank_id, bank_offset, data_key, &key_valid_bits));

        /*
         * Read mask from shadow position + 1.
         */
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_entry_key_read_shadow
                        (unit, core, bank_id, bank_offset + 1, data_mask, &shadow_valid_bits));

    }
    else if (ACCESS_HW)
    {
        SOC_REG_ABOVE_64_CLEAR(mem_val_write);
        soc_mem_field32_set(unit, TCAM_TCAM_BANK_COMMANDm, mem_val_write, TCAM_CPU_CMD_WRf, 0x0);
        soc_mem_field32_set(unit, TCAM_TCAM_BANK_COMMANDm, mem_val_write, TCAM_CPU_CMD_RDf, 0x1);
        soc_mem_field32_set(unit, TCAM_TCAM_BANK_COMMANDm, mem_val_write, TCAM_CPU_CMD_CMPf, 0x0);
        /*
         * Read key from position
         */
        SHR_IF_ERR_EXIT(WRITE_TCAM_TCAM_BANK_COMMANDm(unit, TCAM_BLOCK(unit, core), tcam_bank_address, mem_val_write));

        SOC_REG_ABOVE_64_CLEAR(mem_val_read);
        SHR_IF_ERR_EXIT(READ_TCAM_TCAM_BANK_REPLYm(unit, TCAM_BLOCK(unit, core), tcam_bank_address, mem_val_read));

        key_valid_bits = soc_mem_field32_get(unit, TCAM_TCAM_BANK_REPLYm, mem_val_read, TCAM_CPU_REP_VALIDf);
        soc_mem_field_get(unit, TCAM_TCAM_BANK_REPLYm, mem_val_read, TCAM_CPU_REP_DOUTf, data_key);

        /*
         * Read key mask from position + 1
         */
        SHR_IF_ERR_EXIT(WRITE_TCAM_TCAM_BANK_COMMANDm
                        (unit, TCAM_BLOCK(unit, core), tcam_bank_address + 1, mem_val_write));

        SOC_REG_ABOVE_64_CLEAR(mem_val_read);
        SHR_IF_ERR_EXIT(READ_TCAM_TCAM_BANK_REPLYm(unit, TCAM_BLOCK(unit, core), tcam_bank_address + 1, mem_val_read));

        shadow_valid_bits = soc_mem_field32_get(unit, TCAM_TCAM_BANK_REPLYm, mem_val_read, TCAM_CPU_REP_VALIDf);
        soc_mem_field_get(unit, TCAM_TCAM_BANK_REPLYm, mem_val_read, TCAM_CPU_REP_DOUTf, data_mask);

    }

    if (key_valid_bits != shadow_valid_bits)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Valid bits of read key/mask are differnet, key valid bits: %d, mask valid bits: %d\n",
                     key_valid_bits, shadow_valid_bits);
    }
    *valid_bits = key_valid_bits;

    /*
     * This code section is responsible for taking out key mode from the read data
     * buffer from TCAM HW.
     *
     * Each TCAM key consists of both data and key mode, in which key mode is
     * placed in the 2 bits prior to each 80 bit data chunck, as in the
     * following:
     *
     * When reading a key, we should take out the regions marked with *
     *
     *    Should be taken out                 Should be taken out
     *        |                                |
     *        v                                v
     *       (1*)             (2)             (3*)              (4)
     * +-----------------------------------------------------------------+
     * | key mode (2b) | key data (80b) | key mode (2b) | key data (80b) |
     * +=================================================================+
     * 0                                                                164
     *
     * While mask look as follows:
     *
     *       Should be taken out             Should be taken out
     *        |                                |
     *        v                                v
     *       (5*)             (6)             (7*)              (8)
     * +-----------------------------------------------------------------+
     * |       11      | key mask (80b) |       11      | key mask (80b) |
     * +=================================================================+
     * 160                                                              327
     */
    SHR_BITCOPY_RANGE(key, 0, data_key, KEY_MODE_BIT_SIZE, HALF_KEY_BIT_SIZE);
    SHR_BITCOPY_RANGE(key, HALF_KEY_BIT_SIZE, data_key, 2 * KEY_MODE_BIT_SIZE + HALF_KEY_BIT_SIZE, HALF_KEY_BIT_SIZE);
    SHR_BITCOPY_RANGE(key_mask, 0, data_mask, KEY_MODE_BIT_SIZE, HALF_KEY_BIT_SIZE);
    SHR_BITCOPY_RANGE(key_mask, HALF_KEY_BIT_SIZE, data_mask,
                      2 * KEY_MODE_BIT_SIZE + HALF_KEY_BIT_SIZE, HALF_KEY_BIT_SIZE);

    /*
     * Mask decoding:
     * In TCAM, because key/mask are written encoded, mask needs to be decoded after reading.
     * The decoding is done as following:
     * 1) mask = mask | key;
     */
    /** 1) Decode mask */
    SHR_BITOR_RANGE(key_mask, key, 0, SINGLE_KEY_BIT_SIZE, key_mask);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Writes the given key and key_mask to the given position, also writes
 *  the mode (used as search size) and valid (which parts of the key are
 *  valid). The write is performed on TCAM at the given core.
 *  NOTE: this function can write up to single-sized keys in TCAM,
 *  but, it also can be called twice to write double-keys to TCAM.
 *
 * \param [in] unit        - Device ID
 * \param [in] core        - Core ID
 * \param [in] bank_id     - The TCAM_BANK ID to read from
 * \param [in] bank_offset - The offset inside the TCAM_BANK to read from, by counting Single sized entries.
 *                           Equal to the TCAM bank offset, twice the action bank offset.
 * \param [in] key         - The key buffer to write
 * \param [in] key_mask    - The key_mask buffer to write
 * \param [in] key_mode    - The search mode (is set according to key size)
 * \param [in] part        - Which part of the key to write to
 * \param [in] valid       - Specifies whether entry is valid or not
 *
 * \return
 *   \retval _SHR_E_NONE success
 *
 * \remark
 * SPECIAL OUTPUT:
 *     HW:
 *        TCAM_TCAM_BANK_COMMAND - The memory to write data/command to
 */
static shr_error_e
dnx_field_tcam_access_entry_key_write(
    int unit,
    int core,
    uint32 bank_id,
    uint32 bank_offset,
    uint32 *key,
    uint32 *key_mask,
    dnx_field_tcam_access_key_mode_e key_mode,
    dnx_field_tcam_access_part_e part,
    uint8 valid)
{
    soc_reg_above_64_val_t mem_val;
    /*
     * Double key size because data holds both key and mask.
     * +1 to have extra space for mode (true for all 3 vars)
     */
    uint32 data[MAX_DOUBLE_KEY_WORD_SIZE + 1] = { 0 };
    uint32 data_key[MAX_SINGLE_KEY_WORD_SIZE + 1] = { 0 };
    uint32 neg_data_key[MAX_SINGLE_KEY_WORD_SIZE + 1] = { 0 };
    uint32 data_mask[MAX_SINGLE_KEY_WORD_SIZE + 1] = { 0 };
    uint32 encoded_key[MAX_SINGLE_KEY_WORD_SIZE + 1] = { 0 };
    uint32 encoded_mask[MAX_SINGLE_KEY_WORD_SIZE + 1] = { 0 };
    uint32 tcam_bank_address;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * We now find the position within the TCAM table.
     */
    tcam_bank_address = DNX_FIELD_TCAM_LOCATION_BANK_TO_ABSOLUTE(bank_id, bank_offset);

    SOC_REG_ABOVE_64_CLEAR(mem_val);

    /*
     * This code section is responsible for placing key mode along with key
     * buffer inside the TCAM key array which is going to be written to HW.
     *
     * Each TCAM key consists of both data and key mode, in which key mode is
     * placed in the 2 bits prior to each 80 bit data chunck, as in the
     * following:
     *
     *       (1)             (2)             (3)              (4)
     * +-----------------------------------------------------------------+
     * | key mode (2b) | key data (80b) | key mode (2b) | key data (80b) |
     * +=================================================================+
     *
     * While mask should look as follows:
     *       (5)             (6)             (7)              (8)
     * +-----------------------------------------------------------------+
     * |       11      | key mask (80b) |       11      | key mask (80b) |
     * +=================================================================+
     */
    if (part & DNX_FIELD_TCAM_ACCESS_PART_LSB)
    {
        /** (1) */
        SHR_BITCOPY_RANGE(data_key, 0, &key_mode, 0, KEY_MODE_BIT_SIZE);
        /** (2) */
        SHR_BITCOPY_RANGE(data_key, KEY_MODE_BIT_SIZE, key, 0, HALF_KEY_BIT_SIZE);

        /** (6) */
        SHR_BITCOPY_RANGE(data_mask, KEY_MODE_BIT_SIZE, key_mask, 0, HALF_KEY_BIT_SIZE);

        /** Set key_mode mask only when entry is valid (otherwise, ECC errors will occur on entry delete) */
        if (valid & DNX_FIELD_TCAM_ACCESS_PART_LSB)
        {
            /** (5) */
            SHR_BITSET_RANGE(data_mask, 0, KEY_MODE_BIT_SIZE);
        }
    }
    if (part & DNX_FIELD_TCAM_ACCESS_PART_MSB)
    {
        /** (3) */
        SHR_BITCOPY_RANGE(data_key, HALF_KEY_BIT_SIZE + KEY_MODE_BIT_SIZE, &key_mode, 0, KEY_MODE_BIT_SIZE);
        /** (4) */
        SHR_BITCOPY_RANGE(data_key, HALF_KEY_BIT_SIZE + 2 * KEY_MODE_BIT_SIZE, key, HALF_KEY_BIT_SIZE,
                          HALF_KEY_BIT_SIZE);
        /** (8) */
        SHR_BITCOPY_RANGE(data_mask, HALF_KEY_BIT_SIZE + 2 * KEY_MODE_BIT_SIZE, key_mask, HALF_KEY_BIT_SIZE,
                          HALF_KEY_BIT_SIZE);

        /** Set key_mode mask only when entry is valid (otherwise, ECC errors will occur on entry delete) */
        if (valid & DNX_FIELD_TCAM_ACCESS_PART_MSB)
        {
            /** (7) */
            SHR_BITSET_RANGE(data_mask, HALF_KEY_BIT_SIZE + KEY_MODE_BIT_SIZE, KEY_MODE_BIT_SIZE);
        }
    }

    /*
     * Key/Mask encoding:
     * In TCAM, before writing the key/mask they need to be encoded, the encoded is done as the following:
     * 1) key = key & mask
     * 2) mask = ~key & mask
     */
    /** 1) Encode Key for TCAM */
    SHR_BITAND_RANGE(data_key, data_mask, 0, SINGLE_KEY_BIT_SIZE + 2 * KEY_MODE_BIT_SIZE, encoded_key);

    /** 2) Encode Mask for TCAM */
    SHR_BITNEGATE_RANGE(data_key, 0, SINGLE_KEY_BIT_SIZE + 2 * KEY_MODE_BIT_SIZE, neg_data_key);
    SHR_BITAND_RANGE(neg_data_key, data_mask, 0, SINGLE_KEY_BIT_SIZE + 2 * KEY_MODE_BIT_SIZE, encoded_mask);

    /** Copy data_mask||data_key to data */
    SHR_BITCOPY_RANGE(data, 0, encoded_key, 0, SINGLE_KEY_BIT_SIZE + 2 * KEY_MODE_BIT_SIZE);
    SHR_BITCOPY_RANGE(data, SINGLE_KEY_BIT_SIZE + 2 * KEY_MODE_BIT_SIZE,
                      encoded_mask, 0, SINGLE_KEY_BIT_SIZE + 2 * KEY_MODE_BIT_SIZE);

    soc_mem_field32_set(unit, TCAM_TCAM_BANK_COMMANDm, mem_val, TCAM_CPU_CMD_WRf, part);
    soc_mem_field32_set(unit, TCAM_TCAM_BANK_COMMANDm, mem_val, TCAM_CPU_CMD_RDf, 0x0);
    soc_mem_field32_set(unit, TCAM_TCAM_BANK_COMMANDm, mem_val, TCAM_CPU_CMD_CMPf, 0x0);
    soc_mem_field_set(unit, TCAM_TCAM_BANK_COMMANDm, mem_val, TCAM_CPU_CMD_DIf, data);

    /*
     * Valid is two bits, one per half-key
     *
     *    VALID
     *  bit1 bit0
     * +----+----+
     * | X  | X  |
     * +====+====+
     *
     * bit0 is for first 80b
     * bit1 is for second 80b
     */
    soc_mem_field32_set(unit, TCAM_TCAM_BANK_COMMANDm, mem_val, TCAM_CPU_CMD_VALIDf, valid);

    if (ACCESS_HW)
    {
        SHR_IF_ERR_EXIT(WRITE_TCAM_TCAM_BANK_COMMANDm(unit, TCAM_BLOCK(unit, core), tcam_bank_address, mem_val));
    }

    if (DNX_FIELD_TCAM_SHADOW_KEEP)
    {
        /*
         * Write to shadow.
         */
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_entry_key_write_shadow
                        (unit, core, bank_id, bank_offset, valid, part, encoded_key));
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_entry_key_write_shadow
                        (unit, core, bank_id, bank_offset + 1, valid, part, encoded_mask));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Reads half-payload size bits payload(action) of the given position composed of (bank_id, bank_offset) into
 *  given payload buffer. The read is performed from TCAM ACTION HW at the given core.
 *
 * \param [in] unit           - Device ID
 * \param [in] core           - Core ID
 * \param [in] action_bank_id - The actiont able to write to (there are two for each bank ID)
 * \param [in] bank_offset    - The offset inside the bank to read from
 * \param [out] payload       - The buffer to copy read data to (should be
 *                              pre-allocated to at least half-payload size)
 *
 * \return
 *   \retval _SHR_E_NONE success
 *
 * \remark
 * SPECIAL INPUT:
 *     HW:
 *        TCAM_TCAM_ACTION(_SMALL) - The memory to read the data from
 */
static shr_error_e
dnx_field_tcam_access_entry_payload_read(
    int unit,
    int core,
    uint32 action_bank_id,
    uint32 bank_offset,
    uint32 *payload)
{
    uint32 data[MAX_DOUBLE_ACTION_WORD_SIZE] = { 0 };
    soc_mem_t action_tbl;

    SHR_FUNC_INIT_VARS(unit);

    if (DNX_FIELD_TCAM_SHADOW_READ)
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_entry_payload_read_shadow
                        (unit, core, action_bank_id, bank_offset, payload));
    }
    else if (ACCESS_HW)
    {
        if (action_bank_id < dnx_data_field.tcam.nof_big_banks_get(unit) * 2)
        {
            action_tbl = TCAM_TCAM_ACTIONm;
        }
        else
        {
            action_tbl = TCAM_TCAM_ACTION_SMALLm;
        }

        SHR_IF_ERR_EXIT(soc_mem_array_read
                        (unit, action_tbl, action_bank_id, TCAM_BLOCK(unit, core), bank_offset, data));

        soc_mem_field_get(unit, action_tbl, data, ACTIONf, payload);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Writes half-payload size from the given payload(action) buffer to the given
 *  postion composed of (bank_id,bank_offset) in memory. The write is performed
 *  on TCAM ACTION HW at the given core.
 *
 * \param [in] unit               - Device ID
 * \param [in] core               - Core ID
 * \param [in] action_bank_id     - The actiont able to write to (there are two for each bank ID)
 * \param [in] bank_offset        - The offset inside the bank to write to
 * \param [in] payload            - The payload to write, (should be pre-allocated to
 *                                  at least half-payload size)
 *
 * \return
 *   \retval _SHR_E_NONE success
 *
 * \remark
 * SPECIAL OUTPUT:
 *     HW:
 *        TCAM_TCAM_ACTION(_SMALL) - The memory to write the data to
 */
static shr_error_e
dnx_field_tcam_access_entry_payload_write(
    int unit,
    int core,
    uint32 action_bank_id,
    uint32 bank_offset,
    uint32 *payload)
{
    uint32 data[MAX_DOUBLE_ACTION_WORD_SIZE] = { 0 };
    soc_mem_t action_tbl;

    SHR_FUNC_INIT_VARS(unit);

    if (ACCESS_HW)
    {
        if (action_bank_id < dnx_data_field.tcam.nof_big_banks_get(unit) * 2)
        {
            action_tbl = TCAM_TCAM_ACTIONm;
        }
        else
        {
            action_tbl = TCAM_TCAM_ACTION_SMALLm;
        }

        soc_mem_field_set(unit, action_tbl, data, ACTIONf, payload);

        SHR_IF_ERR_EXIT(soc_mem_array_write
                        (unit, action_tbl, action_bank_id, TCAM_BLOCK(unit, core), bank_offset, data));
    }
    if (DNX_FIELD_TCAM_SHADOW_KEEP)
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_entry_payload_write_shadow
                        (unit, core, action_bank_id, bank_offset, payload));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief
 *  Reads DT entry data from the given position from TCAM ACTION HW at the given core
 *
 * \param [in] unit             - Device ID
 * \param [in] core             - Core ID
 * \param [in] app_id           - The app ID that defines the context for the data to write
 * \param [in] position         - Position to write DT entry to
 * \param [in] payload          - Payload of the DT entry to write
 * \param [in] ent_payload_size - The entry payload size to be read
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
dnx_field_tcam_access_entry_dt_read_aux(
    int unit,
    int core,
    uint32 app_id,
    uint32 position,
    uint32 payload[],
    uint32 ent_payload_size)
{
    uint32 payload_msb_aligned[MAX_DOUBLE_ACTION_WORD_SIZE] = { 0 };
    uint32 payload_bank_id;
    uint32 payload_bank_offset;
    dnx_field_tcam_access_db_info db_info;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * app_id is equal to the tcam_handler_id in TCAM Manager, through which we retrieve the the key_size and payload_size
     */
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_info_get(unit, app_id, &db_info));

    /*
     * Convert absolute position to payload_bank_id/payload_bank_offset pair
     * Note that action_bank_id is (bank_id * 2) + 1 and action_bank_offset is "bank_offset / 2"
     */
    payload_bank_id = DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_ID(position) * 2 + 1;
    payload_bank_offset = DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_OFFSET(position) / 2;

    /*
     * For DT entries, we only write payload 
     */
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_entry_payload_read
                    (unit, core, payload_bank_id, payload_bank_offset, payload_msb_aligned));

    /*
     * Align payload back to lsb 
     */
    SHR_BITCOPY_RANGE(payload, 0, payload_msb_aligned, db_info.payload_size - ent_payload_size, ent_payload_size);

    /*
     * Payload 0 means entry is not valid 
     */
    if (payload[0] == 0x0)
    {
        SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief
 *  Writes DT entry data to the given position to TCAM ACTION HW at the given core
 *
 * \param [in] unit             - Device ID
 * \param [in] core             - Core ID
 * \param [in] app_id           - The app ID that defines the context for the data to write
 * \param [in] position         - Position to write DT entry to
 * \param [in] payload          - Payload of the DT entry to write
 * \param [in] ent_payload_size - Entry payload size
 * \param [in] is_update        - Whether or not to update the entry
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
dnx_field_tcam_access_entry_dt_write_aux(
    int unit,
    int core,
    uint32 app_id,
    uint32 position,
    uint32 payload[],
    uint32 ent_payload_size,
    uint8 is_update)
{
    uint32 payload_msb_aligned[MAX_DOUBLE_ACTION_WORD_SIZE] = { 0 };
    uint32 payload_bank_id;
    uint32 payload_bank_offset;
    dnx_field_tcam_access_db_info db_info;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * app_id is equal to the tcam_handler_id in TCAM Manager, through which we retrieve the the key_size and payload_size
     */
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_info_get(unit, app_id, &db_info));

    /*
     * Align payload to msb 
     */
    SHR_BITCOPY_RANGE(payload_msb_aligned, db_info.payload_size - ent_payload_size, payload, 0, ent_payload_size);

    /*
     * Convert absolute position to payload_bank_id/payload_bank_offset pair
     * Note that action_bank_id is in msb part and action_bank_offset is "bank_offset / 2"
     */
    payload_bank_id = (DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_ID(position) * 2) + 1;
    payload_bank_offset = DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_OFFSET(position) / 2;

    /*
     * For DT entries, we only write payload 
     */
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_entry_payload_write
                    (unit, core, payload_bank_id, payload_bank_offset, payload_msb_aligned));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Converts the given key_size to its corresponding mode bits.
 *  The mode bits are then set into TCAM HW in order to define the search
 *  key-size (See dnx_field_tcam_access_entry_key_write).
 *
 * \param [in] unit      - Device ID
 * \param [in] key_size  - The key_size to convert
 * \param [out] key_mode - The corresponding key mode for the given key_size
 *
 * \return
 *   _SHR_E_NONE - Success
 *
 * \remark
 */
static shr_error_e
dnx_field_tcam_access_size_to_mode(
    int unit,
    uint32 key_size,
    dnx_field_tcam_access_key_mode_e * key_mode)
{
    SHR_FUNC_INIT_VARS(NO_UNIT);
    *key_mode = DNX_FIELD_TCAM_ACCESS_KEY_MODE_INVALID;
    if (key_size == dnx_data_field.tcam.key_size_half_get(unit))
    {
        *key_mode = DNX_FIELD_TCAM_ACCESS_KEY_MODE_HALF;
    }
    else if (key_size == dnx_data_field.tcam.key_size_single_get(unit))
    {
        *key_mode = DNX_FIELD_TCAM_ACCESS_KEY_MODE_SINGLE;
    }
    else if (key_size == dnx_data_field.tcam.key_size_double_get(unit))
    {
        *key_mode = DNX_FIELD_TCAM_ACCESS_KEY_MODE_DOUBLE;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid key_size: %d", key_size);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Reads a single-sized entry(key/payload) from TCAM at the given bank_id/bank_offset.
 *  The read operation is performed on TCAM HW at the given core.
 *
 * \param [in] unit        - Device ID
 * \param [in] core        - Core ID
 * \param [in] bank_id     - The bank ID to read from
 * \param [in] bank_offset - The offset inside the bank to read from
 * \param [in] key_mode    - Key mode to read (depends on key size)
 * \param [in] part        - Specifies which part of the key to read
 * \param [in] is_special  - Whether the write operation is for the special (single_key/half_payload) DB or not
 * \param [out] key        - Buffer to copy the read key into
 * \param [out] key_mask   - Buffer to copy the read key_mask into
 * \param [out] payload    - Buffer to copy the read payload into
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
dnx_field_tcam_access_single_entry_read(
    int unit,
    int core,
    uint32 bank_id,
    uint32 bank_offset,
    dnx_field_tcam_access_key_mode_e key_mode,
    dnx_field_tcam_access_part_e part,
    uint8 is_special,
    uint32 *key,
    uint32 *key_mask,
    uint32 *payload)
{
    uint32 action_bank_offset;
    uint32 action_lsb_bank_id;
    uint32 action_msb_bank_id;
    uint8 valid_bits;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Note that when filling TCAM BANK and TCAM ACTION that:
     * 1) The former is filled with granularity of "single
     *    sized key", while the latter is filled with granularity of "half action size".
     * 2) If TCAM_BANK ID is <n>, then the corresponding TCAM_BANK_ACTION ID for it is <2*n>
     *    for the half lsb part of the payload, and <2*n+1> for the half msb part of the payload
     * 3) Since TCAM_BANK odd entries are masks therefore making the offset being multiplied by 2,
     *    TCAM_BANK_ACTION offsets are half the value of TCAM_BANK offsets, so for offset <m> that
     *    we want to access, TCAM_BANK will access offset <2 * m> while TCAM_ACTION will access offset
     *    <m>
     *
     * JR2 example:
     *
     *        TCAM BANK <n>             TCAM ACTION <2*n>    TCAM ACTION <2*n+1>
     * +---------------------------+  +------------------+  +------------------+
     * |<2*m>| 160b key + 160b mask|  |<m>|  32b payload |  |<m>|  32b payload |
     * +-==========================+  +==================+  +==================+
     */
    action_bank_offset = bank_offset / 2;

    /**
     * Determine action lsb part/msb part bank id.
     * In JR2_B0, an ECO was introduced to flip between msb and lsb action banks of the result.
     * This change only affects non-special databases and non half-key size databases.
     */
    if (!is_special && key_mode != DNX_FIELD_TCAM_ACCESS_KEY_MODE_HALF)
    {
        action_lsb_bank_id = (dnx_data_field.features.tcam_result_flip_eco_get(unit)) ? 2 * bank_id + 1 : 2 * bank_id;
        action_msb_bank_id = (dnx_data_field.features.tcam_result_flip_eco_get(unit)) ? 2 * bank_id : 2 * bank_id + 1;
    }
    else
    {
        action_lsb_bank_id = 2 * bank_id;
        action_msb_bank_id = 2 * bank_id + 1;
    }
    /*
     * Payload read
     */
    if (part & DNX_FIELD_TCAM_ACCESS_PART_LSB)
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_entry_payload_read
                        (unit, core, action_lsb_bank_id, action_bank_offset, payload));
    }
    /*
     * Skip MSB part for special FG 
     */
    if (!is_special && (part & DNX_FIELD_TCAM_ACCESS_PART_MSB))
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_entry_payload_read
                        (unit, core, action_msb_bank_id, action_bank_offset, payload + HALF_ACTION_WORD_SIZE));
    }
    /*
     * Key read
     */
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_entry_key_read
                    (unit, core, bank_id, bank_offset, part, key, key_mask, &valid_bits));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Returns the key/key_mask/payload for the given app_id at the given TCAM position.
 *  The entry is read from TCAM HW at the given core.
 *
 * \param [in] unit             - Device ID
 * \param [in] core             - Core ID
 * \param [in] app_id           - The app ID that defines the context for the data to get
 * \param [in] position         - The position to retrieve the data from
 * \param [in] ent_payload_size - Entry payload size
 * \param [out] key             - Key data at the given position (Size is according to app_id)
 * \param [out] key_mask        - Key mask at the given position (Size is according to app_id)
 * \param [out] payload         - Payload data at the given position (Size is according to app_id)
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
dnx_field_tcam_access_entry_get_aux(
    int unit,
    int core,
    uint32 app_id,
    uint32 position,
    uint32 ent_payload_size,
    uint32 key[],
    uint32 key_mask[],
    uint32 payload[])
{
    dnx_field_tcam_access_db_info db_info;
    uint32 bank_id;
    uint32 bank_offset;
    uint8 odd;
    uint32 payload_msb_aligned[MAX_DOUBLE_ACTION_WORD_SIZE] = { 0 };
    uint32 key_read[MAX_DOUBLE_KEY_WORD_SIZE] = { 0 };
    uint32 mask_read[MAX_DOUBLE_KEY_WORD_SIZE] = { 0 };
    uint32 payload_read[MAX_DOUBLE_ACTION_WORD_SIZE] = { 0 };
    dnx_field_tcam_access_key_mode_e key_mode;
    dnx_field_tcam_access_part_e part;
    uint8 is_special = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * app_id is equal to the tcam_handler_id in TCAM Manager, through which we retrieve the the key_size and payload_size
     */
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_info_get(unit, app_id, &db_info));

    /*
     * Position has 80b granularity and odd position point to the 80b msb-part of the entry.
     * Since databases with single/double-sized key are defined to start only from the 80b lsb-part of the entry, it's
     * an error to have odd position for anything other than databases with half-sized key.
     *
     *
     *                 single/double-sized keys
     *                 can only start from here
     *                         |
     *                         |
     *                         v
     *
     * position:   (n + 1)     n            (n is even)
     *           +---------+-----------+
     * key:      | 80 msb  |  80 lsb   |
     *           +---------+-----------+
     *              ^           ^
     *              |           |
     *              |           |
     * valid:       x           x          (2 bits, one for each half)
     */
    odd = (position % 2);
    if (odd && db_info.key_size != dnx_data_field.tcam.key_size_half_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Database %d with key_size %d can't start from position %d", app_id,
                     db_info.key_size, position);
    }

    /*
     * Set which part of the key/payload to read
     */
    if (db_info.key_size == dnx_data_field.tcam.key_size_half_get(unit))
    {
        part = (odd) ? DNX_FIELD_TCAM_ACCESS_PART_MSB : DNX_FIELD_TCAM_ACCESS_PART_LSB;
    }
    else
    {
        part = DNX_FIELD_TCAM_ACCESS_PART_ALL;
    }

    /*
     * Since we've taken care of the oddness of the position, we set it back to be even 
     */
    position = position - (position % 2);

    /*
     * Special DBs are the ones that can reside on the same bank with DT DBs.
     * The sepcial thing about those DBs are that they have single_key_size/half_action_size combination
     * and therefore can use the whole bank with part of the payload, leaving the other part of the payload
     * to DT DBs.
     *
     * S for special DB, DT is for direct table DB:
     *
     * BANK<n> | ACTION<2*n> ACTION<2*n+1> 
     * +---+   |  +---+       +----+
     * | S |   |  | S |       | DT |
     * +===+   |  +===+       +====+
     *
     */
    if (db_info.key_size == dnx_data_field.tcam.key_size_single_get(unit)
        && db_info.payload_size == dnx_data_field.tcam.action_size_half_get(unit))
    {
        is_special = TRUE;
    }

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_size_to_mode(unit, db_info.key_size, &key_mode));

    /*
     * Convert absolute position to bank_id/bank_offset pair
     */
    bank_id = DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_ID(position);
    bank_offset = DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_OFFSET(position);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_single_entry_read
                    (unit, core, bank_id, bank_offset, key_mode, part, is_special, key_read, mask_read, payload_read));

    if (db_info.key_size == dnx_data_field.tcam.key_size_double_get(unit))
    {
        /*
         * When double key is used in TCAM, then two conditions need to be met:
         * 1) The TCAM_BANK ID used is even
         * 2) The msb part single key needs to be read from the TCAM_BANK next to the one given in the
         *    address. For example, if address is for TCAM_BANK ID <n>, then the msb part is read from
         *    TCAM_BANK ID <n+1> while the lsb part if read from TCAM_BANK ID <n>
         *
         * So in addition to the normal single-sized key/payload we read from the given address, we need
         * to read another single-sized key/payload from the bank next to the given bank.
         *
         * +------------------------+-----+
         * | All sizes read         |  |  |
         * |                        |  v  |
         * +------------------------+-----+
         * | Double-size only read  |  "  |
         * |                        |  v  |
         * +========================+=====+
         * 
         *   |        "          |            |                 "             "
         *   v        v          v            v                 v             v
         * BANK<n> BANK<n+1> | ACTION<2*n> ACTION<2*n+1> ACTION<2*(n+1)> ACTION<2*(n+1)+1>
         * +---+    +---+    |  +---+       +---+             +---+         +---+
         * | x |    | x |    |  | x |       | x |             | x |         | x |
         * +===+    +===+    |  +===+       +===+             +===+         +===+
         */
        uint32 *msb_key_part = key_read + SINGLE_KEY_WORD_SIZE;
        uint32 *msb_key_mask_part = mask_read + SINGLE_KEY_WORD_SIZE;
        uint32 *msb_payload_part = payload_read + SINGLE_ACTION_WORD_SIZE;
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_single_entry_read
                        (unit, core, bank_id + 1, bank_offset, key_mode, part, FALSE, msb_key_part,
                         msb_key_mask_part, msb_payload_part));
    }

    /*
     * If odd is TRUE then this is a half-sized key that is located at the msb, so, we shift all buffers by
     * key_size/payload_size accordingly to place the data on lsb side.
     */
    if (odd)
    {
        SHR_BITCOPY_RANGE(key, 0, key_read, db_info.key_size, db_info.key_size);
        SHR_BITCOPY_RANGE(key_mask, 0, mask_read, db_info.key_size, db_info.key_size);
        SHR_BITCOPY_RANGE(payload_msb_aligned, 0, payload_read, db_info.payload_size, db_info.payload_size);
    }
    else
    {
        SHR_BITCOPY_RANGE(key, 0, key_read, 0, db_info.key_size);
        SHR_BITCOPY_RANGE(key_mask, 0, mask_read, 0, db_info.key_size);
        SHR_BITCOPY_RANGE(payload_msb_aligned, 0, payload_read, 0, db_info.payload_size);
    }

    /**
     * Since we write payload aligned to msb side to support ARR HW
     * (ARR HW assumes payload starts from msb), we read the payload aligned
     * to msb. Since DBAL expects the payload read to be aligned to lsb rather
     * than msb, we need to realign the payload back to lsb.
     *
     * So, when reading the following payload:
     *
     * TCAM payload size
     *      |
     *      v
     *      +----------+-----------------+
     *      |xxxxxxxxxx|                 |
     *      +==========+=================+
     *      msb                          lsb
     *
     * We align the payload data to the msb side of the TCAM payload size,
     * before we actually write it to HW:
     *
     *     payload is right-shifted by
     *      (tcam_payload_size -
     *       actual_payload_size)
     *             |
     *             v
     *       --------------->>
     *      +-----------------+----------+
     *      |                 |xxxxxxxxxx|
     *      +=================+==========+
     *      msb                          lsb
     */
    SHR_BITCOPY_RANGE(payload, 0, payload_msb_aligned, db_info.payload_size - ent_payload_size, ent_payload_size);

    /*
     * Remove prefix from key by clearing "prefix_size" bits from the end of key/mask
     */
    if (db_info.prefix_size > 0)
    {
        SHR_BITCLR_RANGE(key, db_info.key_size - db_info.prefix_size, db_info.prefix_size);
        SHR_BITCLR_RANGE(key_mask, db_info.key_size - db_info.prefix_size, db_info.prefix_size);
        LOG_DEBUG_EX(BSL_LOG_MODULE, "Upper Key(128b) Read: 0x%08x%08x%08x%08x\n", key[4], key[3], key[2], key[1]);
        if (db_info.key_size == dnx_data_field.tcam.key_size_double_get(unit))
        {
            LOG_DEBUG_EX(BSL_LOG_MODULE,
                         "Upper Key(128b) Read MSB: 0x%08x%08x%08x%08x\n", key[9], key[8], key[7], key[6]);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Get hit indication info for a specific TCAM entry for a specific core.
* \param [in] unit           - Device ID
* \param [in] core_id        - Core ID on which entry is located.
* \param [in] entry_handle   - Unique entry handle
* \param [out] entry_hit_p   - 1 bit to indicate if the given entry
*  was hit on the given core. In case yes it will return 'TRUE',
*  otherwise 'FALSE'.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_tcam_access_tcam_hit_per_core_get(
    int unit,
    dnx_field_tcam_core_e core_id,
    uint32 entry_handle,
    uint8 *entry_hit_p)
{
    uint32 bank_id, bank_line_id, bank_sram_id, sram_line_entry_id;
    uint32 max_nof_entries_in_two_srams;
    uint32 max_nof_srams_per_tcam_bank;
    uint32 max_nof_big_bank_lines, max_nof_small_bank_lines;
    uint8 hit_indication_value;
    soc_mem_t mem = INVALIDm;
    uint32 position;
    uint32 mem_entry_data;
    uint32 mem_hit_field_value;

    SHR_FUNC_INIT_VARS(unit);

    /** Get max values from DNX data. */
    max_nof_srams_per_tcam_bank = dnx_data_field.tcam.action_width_selector_size_get(unit);
    max_nof_entries_in_two_srams =
        (dnx_data_field.tcam.hit_indication_entries_per_byte_get(unit) * max_nof_srams_per_tcam_bank);

    /** Clear the hit indication variables to be sure that no garbage values will be used. */
    hit_indication_value = 0;
    *entry_hit_p = 0;

    /** entry_handle is equal to the entry_id in TCAM Manager, so we use it to get the entry position. */
    SHR_IF_ERR_EXIT_NO_MSG(dnx_field_tcam_access_index_get(unit, core_id, entry_handle, &position));
    /** Convert absolute position to bank_id. */
    bank_id = DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_ID(position);

    /** Check if entry is located in TCAM big banks. */
    if (FIELD_TCAM_BANK_IS_BIG_BANK(bank_id))
    {
        mem = TCAM_TCAM_ACTION_HIT_INDICATIONm;

        /** Get max values from DNX data. */
        max_nof_big_bank_lines = dnx_data_field.tcam.nof_big_bank_lines_get(unit);
        bank_line_id = ((position % max_nof_big_bank_lines) / max_nof_entries_in_two_srams);
    }
    else
    {
        mem = TCAM_TCAM_ACTION_HIT_INDICATION_SMALLm;

        /** Get max values from DNX data. */
        max_nof_small_bank_lines = dnx_data_field.tcam.nof_small_bank_lines_get(unit);
        bank_line_id = ((position % max_nof_small_bank_lines) / max_nof_entries_in_two_srams);
    }

    bank_sram_id = ((bank_id * max_nof_srams_per_tcam_bank) + (position % 2));
    sram_line_entry_id =
        (((position % max_nof_entries_in_two_srams) - (bank_sram_id % 2)) / max_nof_srams_per_tcam_bank);

    mem_entry_data = 0;
    mem_hit_field_value = 0;
    SHR_IF_ERR_EXIT(soc_mem_array_read
                    (unit, mem, bank_sram_id, TCAM_BLOCK(unit, core_id), bank_line_id, &mem_entry_data));
    soc_mem_field_get(unit, mem, &mem_entry_data, HITf, &mem_hit_field_value);

    hit_indication_value = utilex_bitstream_test_bit(&mem_hit_field_value, sram_line_entry_id);

    /** Check whether we have a hit. */
    if (hit_indication_value)
    {
        /** In case we have a HIT for the given entry, set the output hit indication to TRUE. */
        *entry_hit_p = TRUE;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Get hit indication info for a specific TCAM entry.
* \param [in] unit           - Device ID
* \param [in] entry_handle   - Unique entry handle
* \param [out] entry_hit_p   - Bitmap, according to number of cores,
*  to indicate on which cores the given entry was hit.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_tcam_access_tcam_hit_get(
    int unit,
    uint32 entry_handle,
    uint8 *entry_hit_p)
{
    uint8 entry_hit;
    dnx_field_tcam_core_e core = DNX_FIELD_TCAM_ENTRY_CORE_ID(entry_handle);
    uint8 internal_entry_hit[1];

    SHR_FUNC_INIT_VARS(unit);

    *entry_hit_p = 0;
    internal_entry_hit[0] = 0;

    if (core == DNX_FIELD_TCAM_CORE_ALL)
    {
        uint32 nof_cores = dnx_data_device.general.nof_cores_get(unit);
        for (core = 0; core < nof_cores; core++)
        {
            SHR_IF_ERR_EXIT(dnx_field_tcam_access_tcam_hit_per_core_get(unit, core, entry_handle, &entry_hit));

            /** Check whether we have a hit. */
            if (entry_hit == TRUE)
            {
                /** In case we have a HIT for the given entry, set the relevant bit in the bitmap. */
                SHR_BITSET(&internal_entry_hit[0], core);
            }
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_tcam_hit_per_core_get(unit, core, entry_handle, &entry_hit));

        /** Check whether we have a hit. */
        if (entry_hit == TRUE)
        {
            /** In case we have a HIT for the given entry, set the relevant bit in the bitmap. */
            SHR_BITSET(&internal_entry_hit[0], core);
        }
    }

    *entry_hit_p = internal_entry_hit[0];

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Flush/Clear hit indication info for a specific TCAM entry and core.
* \param [in] unit           - Device ID
* \param [in] core_id        - Core ID on which entry is located.
* \param [in] entry_handle   - Unique entry handle
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_tcam_access_tcam_hit_per_core_flush(
    int unit,
    dnx_field_tcam_core_e core_id,
    uint32 entry_handle)
{
    soc_reg_above_64_val_t reg_val_zero;
    soc_mem_t mem = INVALIDm;
    uint32 bank_id, bank_sram_id, bank_line_id, sram_line_entry_id;
    uint32 max_nof_big_bank_lines, max_nof_small_bank_lines;
    uint32 tcam_indirect_mask;
    uint32 max_nof_entries_in_two_srams;
    uint32 max_nof_srams_per_tcam_bank;
    uint32 position;

#ifdef ADAPTER_SERVER_MODE
    uint32 mem_entry_data = 0;
    uint32 mem_hit_field_value = 0;
#else
    uint32 mem_addr;
    uint8 at;
    uint32 reg_val = 0;
#endif

    SHR_FUNC_INIT_VARS(unit);

    bank_sram_id = bank_id = bank_line_id = sram_line_entry_id = 0;
    max_nof_big_bank_lines = max_nof_small_bank_lines = 0;
    max_nof_entries_in_two_srams = max_nof_srams_per_tcam_bank = 0;

    SOC_REG_ABOVE_64_CLEAR(reg_val_zero);

    /** entry_handle is equal to the entry_id in TCAM Manager, so we use it to get the entry position. */
    SHR_IF_ERR_EXIT_NO_MSG(dnx_field_tcam_access_index_get(unit, core_id, entry_handle, &position));

    /** Get the bank_id from entry absolute location. */
    bank_id = DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_ID(position);

    /** Get max values from DNX data. */
    max_nof_srams_per_tcam_bank = dnx_data_field.tcam.action_width_selector_size_get(unit);
    max_nof_entries_in_two_srams =
        (dnx_data_field.tcam.hit_indication_entries_per_byte_get(unit) * max_nof_srams_per_tcam_bank);

    /** Check if entry is located in TCAM big banks. */
    if (FIELD_TCAM_BANK_IS_BIG_BANK(bank_id))
    {
        mem = TCAM_TCAM_ACTION_HIT_INDICATIONm;

        /** Get max values from DNX data. */
        max_nof_big_bank_lines = dnx_data_field.tcam.nof_big_bank_lines_get(unit);
        /** Calculate the bank_line_id for big banks. */
        bank_line_id = ((position % max_nof_big_bank_lines) / max_nof_entries_in_two_srams);
    }
    else
    {
        mem = TCAM_TCAM_ACTION_HIT_INDICATION_SMALLm;

        /** Get max values from DNX data. */
        max_nof_small_bank_lines = dnx_data_field.tcam.nof_small_bank_lines_get(unit);
        /** Calculate the bank_line_id for small banks. */
        bank_line_id = ((position % max_nof_small_bank_lines) / max_nof_entries_in_two_srams);
    }

    /** Calculate the bank_sram_id and sram_line_entry_id. */
    bank_sram_id = ((bank_id * max_nof_srams_per_tcam_bank) + (position % 2));
    sram_line_entry_id =
        (((position % max_nof_entries_in_two_srams) - (bank_sram_id % 2)) / max_nof_srams_per_tcam_bank);
    /** Mask the exact bit, which is corresponding to the entry, which have to be cleared. */
    tcam_indirect_mask = 0x1 << sram_line_entry_id;

    /** On Cmodel we perform direct write, on device indirect. */
#ifdef ADAPTER_SERVER_MODE
    /** On Cmodel only core 0 is available, no need to do change the value for other cores. */
    if (core_id == 0)
    {
        SHR_IF_ERR_EXIT(soc_mem_array_read
                        (unit, mem, bank_sram_id, TCAM_BLOCK(unit, core_id), bank_line_id, &mem_entry_data));
        soc_mem_field_get(unit, mem, &mem_entry_data, HITf, &mem_hit_field_value);

        /** Clear the bit coresponding to the given entry. */
        mem_entry_data = mem_hit_field_value - tcam_indirect_mask;
        SHR_IF_ERR_EXIT(soc_mem_array_write
                        (unit, mem, bank_sram_id, TCAM_BLOCK(unit, core_id), bank_line_id, &mem_entry_data));
    }
#else
    /** Get the memory address by array_index, block_index and index. */
    mem_addr = soc_mem_addr_get(unit, mem, bank_sram_id, TCAM_BLOCK(unit, core_id), bank_line_id, &at);

    /**
     * In case we have specific entry, which have to be cleared, we should
     * add to the base memory address the bank line id on which the entry is located.
     */
    if (position != DNX_FIELD_ENTRY_ACCESS_ID_INVALID)
    {
        mem_addr += bank_line_id;
    }

    SHR_IF_ERR_EXIT(WRITE_TCAM_INDIRECT_COMMANDr(unit, core_id, 0xe00003));
    soc_reg_field_set(unit, TCAM_INDIRECT_COMMAND_ADDRESSr, &reg_val, INDIRECT_COMMAND_ADDRf, mem_addr);
    soc_reg_field_set(unit, TCAM_INDIRECT_COMMAND_ADDRESSr, &reg_val, INDIRECT_COMMAND_TYPEf, 0x0);
    SHR_IF_ERR_EXIT(WRITE_TCAM_INDIRECT_COMMAND_ADDRESSr(unit, core_id, reg_val));
    SHR_IF_ERR_EXIT(WRITE_TCAM_INDIRECT_WR_MASKr(unit, core_id, tcam_indirect_mask));
    SHR_IF_ERR_EXIT(WRITE_TCAM_INDIRECT_COMMAND_WR_DATAr(unit, core_id, reg_val_zero));
#endif

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Flush/Clear hit indication info for a specific TCAM entry.
* \param [in] unit           - Device ID
* \param [in] entry_handle   - Unique entry handle
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_tcam_access_tcam_hit_flush(
    int unit,
    uint32 entry_handle)
{
    dnx_field_tcam_core_e core = DNX_FIELD_TCAM_ENTRY_CORE_ID(entry_handle);

    SHR_FUNC_INIT_VARS(unit);

    if (core == DNX_FIELD_TCAM_CORE_ALL)
    {
        uint32 nof_cores = dnx_data_device.general.nof_cores_get(unit);
        /** Iterate over nof_cores and clear hit info, on all cores, for the given entry. */
        for (core = 0; core < nof_cores; core++)
        {
            /** Clear hit info for the given entry. */
            SHR_IF_ERR_EXIT(dnx_field_tcam_access_tcam_hit_per_core_flush(unit, core, entry_handle));
        }
    }
    else
    {
        /** Clear hit info for the given entry. */
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_tcam_hit_per_core_flush(unit, core, entry_handle));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_access_entry_get(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_t * entry,
    uint32 flags)
{
    uint32 position;
    dnx_field_tcam_access_db_info db_info;
    dnx_field_tcam_core_e core = DNX_FIELD_TCAM_ENTRY_CORE_ID(entry->entry_hw_id);
    uint32 unsupported_hitbit_flags;
    uint8 hitbit_flags;

    SHR_FUNC_INIT_VARS(unit);

    hitbit_flags = entry->hitbit;
    if ((unsupported_hitbit_flags =
         (hitbit_flags &
          ~(DBAL_PHYSICAL_KEY_HITBIT_GET | DBAL_PHYSICAL_KEY_HITBIT_PRIMARY | DBAL_PHYSICAL_KEY_HITBIT_SECONDARY |
            DBAL_PHYSICAL_KEY_HITBIT_CLEAR))))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Flags 0x%X are not supported!", unsupported_hitbit_flags);
    }

    if (core == DNX_FIELD_TCAM_CORE_ALL)
    {
        /**
         * When all cores are requested for get operation, it's sufficient to get the entry
         * from the first core only
         */
        core = DNX_FIELD_TCAM_CORE_FIRST;
    }

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_info_get(unit, app_id, &db_info));
    /**
     * From now on, we exit with no message since expected errors (like not_found) can happen regularly
     * and we want to refrain from spamming output with unnecessary messages, leaving real errors
     * reporting/handling to the upper layers
     */
    /*
     * entry_hw_id is equal to the entry_id in TCAM Manager, so we use it to get the entry position
     */
    SHR_IF_ERR_EXIT_NO_MSG(dnx_field_tcam_access_index_get(unit, core, entry->entry_hw_id, &position));

    if (db_info.direct_table)
    {
        SHR_IF_ERR_EXIT_NO_MSG(dnx_field_tcam_access_entry_dt_read_aux
                               (unit, core, app_id, position, entry->payload, entry->payload_size));
    }
    else
    {
        uint8 entry_in_use;
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.entry_in_use_bmp.v_bit.bit_get(unit, core, position, &entry_in_use));
        if (!entry_in_use)
        {
            /** If the entry is not found, the hitbit value should be zeroed. */
            entry->hitbit = 0;
            SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
            SHR_EXIT();
        }
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_entry_get_aux
                        (unit, core, app_id, position, entry->payload_size, entry->key, entry->k_mask, entry->payload));

        if (hitbit_flags &
            (DBAL_PHYSICAL_KEY_HITBIT_GET | DBAL_PHYSICAL_KEY_HITBIT_PRIMARY | DBAL_PHYSICAL_KEY_HITBIT_SECONDARY |
             DBAL_PHYSICAL_KEY_HITBIT_CLEAR))
        {
            SHR_IF_ERR_EXIT(dnx_field_tcam_access_tcam_hit_get(unit, entry->entry_hw_id, &entry->hitbit));
            /** In case CLEAR flag is requested, call the flush API. */
            if (hitbit_flags & DBAL_PHYSICAL_KEY_HITBIT_CLEAR)
            {
                SHR_IF_ERR_EXIT(dnx_field_tcam_access_tcam_hit_flush(unit, entry->entry_hw_id));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  The SW mock uses a hash to store HW information.
 *  This function adds a new key/mask pair to the SW mock at the given position at the given core.
 *
 * \param [in] unit      - Device ID
 * \param [in] core      - Core ID
 * \param [in] core      - Core ID
 * \param [in] key       - The key to add to the SW mock
 * \param [in] key_mask  - The key_mask to add to the SW mock
 * \param [in] key_size  - Size of key
 * \param [in] position  - The position as to where to add the key to in the SW mock
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
dnx_field_tcam_access_sw_hash_key_add(
    int unit,
    int core,
    uint32 key[DBAL_PHYSICAL_KEY_SIZE_IN_WORDS],
    uint32 key_mask[DBAL_PHYSICAL_KEY_SIZE_IN_WORDS],
    uint32 key_size,
    uint32 position)
{
    dnx_field_tcam_access_mapper_hash_key hash_key = { {0}, {0} };
    uint8 success;
    int bank_id;
    int bank_offset;

    SHR_FUNC_INIT_VARS(unit);

    /** Add encoded key to hash */
    SHR_BITAND_RANGE(key, key_mask, 0, key_size, key);

    SHR_BITCOPY_RANGE(hash_key.key, 0, key, 0, key_size);
    SHR_BITCOPY_RANGE(hash_key.key_mask, 0, key_mask, 0, key_size);

    bank_id = DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_ID(position);
    bank_offset = DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_OFFSET(position);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_mapper_sw.
                    key_2_entry_id_hash.insert_at_index(unit, core, bank_id, &hash_key, bank_offset, &success));
    if (!success)
    {
        SHR_ERR_EXIT(_SHR_E_EXISTS,
                     "Failed adding new entry to position %d in SW hash - Check if entry already exists%s%s%s\n",
                     position, EMPTY, EMPTY, EMPTY);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  The SW mock uses a hash to store HW information.
 *  Removes the key at the given position index from SW mock at the given core.
 *
 * \param [in] unit     - Device ID
 * \param [in] core     - Core ID
 * \param [in] position - The index to delete from SW mock
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
dnx_field_tcam_access_sw_hash_key_remove(
    int unit,
    int core,
    uint32 position)
{
    int bank_id;
    int bank_offset;

    SHR_FUNC_INIT_VARS(unit);

    bank_id = DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_ID(position);
    bank_offset = DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_OFFSET(position);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_mapper_sw.
                    key_2_entry_id_hash.delete_by_index(unit, core, bank_id, bank_offset));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Writes a single-sized entry(key/payload) to TCAM at the given bank_id/bank_offset.
 *  The write operation is performed on TCAM HW at the given core.
 *
 * \param [in] unit        - Device ID
 * \param [in] core        - Core ID
 * \param [in] bank_id     - The TCAM_BANK ID to read from
 * \param [in] bank_offset - The offset inside the TCAM_BANK to read from
 * \param [in] key         - Buffer of the key to write
 * \param [in] key_mask    - Buffer of the key mask to write
 * \param [in] payload     - Buffer of the payload to write
 * \param [in] key_mode    - Key mode to write (depends on key size)
 * \param [in] part        - Which parts of the key should be written
 * \param [in] valid       - Specifies whether entry is valid or not
 * \param [in] is_special  - Whether the write operation is for the special (single_key/half_payload) DB or not
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
dnx_field_tcam_access_single_entry_write(
    int unit,
    int core,
    uint32 bank_id,
    uint32 bank_offset,
    uint32 *key,
    uint32 *key_mask,
    uint32 *payload,
    dnx_field_tcam_access_key_mode_e key_mode,
    dnx_field_tcam_access_part_e part,
    uint8 valid,
    uint8 is_special)
{
    uint32 action_bank_offset;
    uint32 action_lsb_bank_id;
    uint32 action_msb_bank_id;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Note that when filling TCAM BANK and TCAM ACTION that:
     * 1) The former is filled with granularity of "single
     *    sized key", while the latter is filled with granularity of "half action size".
     * 2) If TCAM_BANK ID is <n>, then the corresponding TCAM_BANK_ACTION ID for it is <2*n>
     *    for the half lsb part of the payload, and <2*n+1> for the half msb part of the payload
     * 3) Since TCAM_BANK odd entries are masks therefore making the offset being multiplied by 2,
     *    TCAM_BANK_ACTION offsets are half the value of TCAM_BANK offsets, so for offset <m> that
     *    we want to access, TCAM_BANK will access offset <2 * m> while TCAM_ACTION will access offset
     *    <m>
     *
     * JR2 example:
     *
     *        TCAM BANK <n>             TCAM ACTION <2*n>    TCAM ACTION <2*n+1>
     * +---------------------------+  +------------------+  +------------------+
     * |<2*m>| 160b key + 160b mask|  |<m>|  32b payload |  |<m>|  32b payload |
     * +-==========================+  +==================+  +==================+
     */
    action_bank_offset = bank_offset / 2;
    /**
     * Determine action lsb part/msb part bank id.
     * In JR2_B0, an ECO was introduced to flip between msb and lsb action banks of the result.
     * This change only affects non-special databases and non half-key size databases.
     */
    if (!is_special && key_mode != DNX_FIELD_TCAM_ACCESS_KEY_MODE_HALF)
    {
        action_lsb_bank_id = (dnx_data_field.features.tcam_result_flip_eco_get(unit)) ? 2 * bank_id + 1 : 2 * bank_id;
        action_msb_bank_id = (dnx_data_field.features.tcam_result_flip_eco_get(unit)) ? 2 * bank_id : 2 * bank_id + 1;
    }
    else
    {
        action_lsb_bank_id = 2 * bank_id;
        action_msb_bank_id = 2 * bank_id + 1;
    }
    /*
     * Payload write.
     * For is_special we ignore the MSB part of the payload, as it uses
     * only half payload size and the MSB part is used by DT DBs.
     */
    if (part & DNX_FIELD_TCAM_ACCESS_PART_LSB)
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_entry_payload_write
                        (unit, core, action_lsb_bank_id, action_bank_offset, payload));
    }
    if (!is_special && (part & DNX_FIELD_TCAM_ACCESS_PART_MSB))
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_entry_payload_write
                        (unit, core, action_msb_bank_id, action_bank_offset, payload + HALF_ACTION_WORD_SIZE));
    }
    /*
     * Key write
     */
    if (dnx_data_field.features.ecc_wa_enable_get(unit) && part != DNX_FIELD_TCAM_ACCESS_PART_ALL)
    {
        /*
         * Check if complementary half is in use 
         */
        uint8 entry_in_use;
        uint32 position =
            DNX_FIELD_TCAM_LOCATION_BANK_TO_ABSOLUTE(bank_id, bank_offset + (DNX_FIELD_TCAM_ACCESS_PART_MSB - part));
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.entry_in_use_bmp.v_bit.bit_get(unit, core, position, &entry_in_use));
        if (entry_in_use)
        {
            /**
             * ECC is calculated for the whole entry when written, without any consideration for half-key writes.
             * Therefore, we read the complementary half and add it to the write data (if found) before writing
             * the requested half instead of just writing half-entries, in order to prevent ECC miscalculation.
             */
            uint32 key_read[MAX_SINGLE_KEY_WORD_SIZE + 1];
            uint32 key_read_m[MAX_SINGLE_KEY_WORD_SIZE + 1];
            int copy_from = 0;
            uint8 valid_bits;
            SHR_IF_ERR_EXIT(dnx_field_tcam_access_entry_key_read
                            (unit, core, bank_id, bank_offset, DNX_FIELD_TCAM_ACCESS_PART_ALL - part, key_read,
                             key_read_m, &valid_bits));
            if (part == DNX_FIELD_TCAM_ACCESS_PART_LSB)
            {
                copy_from = SINGLE_KEY_BIT_SIZE / 2;
            }
            /*
             * Update valid for complementary half-entry 
             */
            valid |= valid_bits & (DNX_FIELD_TCAM_ACCESS_PART_ALL - part);
            /** Copy the complementary half into key/key_mask buffer to be written */
            SHR_BITCOPY_RANGE(key, copy_from, key_read, copy_from, SINGLE_KEY_BIT_SIZE / 2);
            SHR_BITCOPY_RANGE(key_mask, copy_from, key_read_m, copy_from, SINGLE_KEY_BIT_SIZE / 2);
            /** Write both line parts */
            part = DNX_FIELD_TCAM_ACCESS_PART_ALL;
        }
    }
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_entry_key_write
                    (unit, core, bank_id, bank_offset, key, key_mask, key_mode, part, valid));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Invalidates the entry in the given position in TCAM HW at the given core.
 *
 * \param [in] unit        - Device ID
 * \param [in] core        - Core ID
 * \param [in] position    - The position to invalidate
 * \param [in] part        - Part to invalidate
 * \param [in] is_special  - If FG is 'special' - (160b/32b)
 *
 * \return
 *   \retval _SHR_E_NONE success
 *
 * \remark
 * SPECIAL OUTPUT:
 *     HW:
 *        TCAM_TCAM_MANAGER_0 - The memory to validate from that the invalidate
 *                              operation is done
 * SPECIAL OUTPUT:
 *     HW:
 *        TCAM_TCAM_MANAGER_1 - The memory to request the invalidation
 *                              opreation from
 */
static shr_error_e
dnx_field_tcam_access_entry_invalidate(
    int unit,
    int core,
    int position,
    dnx_field_tcam_access_part_e part,
    uint8 is_special)
{
    uint32 zero_key[MAX_SINGLE_KEY_WORD_SIZE + 1] = { 0 };
    uint32 zero_mask[MAX_SINGLE_KEY_WORD_SIZE + 1] = { 0 };
    uint32 zero_payload[MAX_SINGLE_ACTION_WORD_SIZE + 1] = { 0 };
    uint32 valid = 0;
    int bank_id = DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_ID(position);
    int bank_offset = DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_OFFSET(position);

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_single_entry_write(unit, core, bank_id, bank_offset,
                                                             zero_key, zero_mask, zero_payload,
                                                             0, part, valid, is_special));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Adds prefix_value to the key and 1s to the mask (at the msb):
 *  NOTE: IF mask is NULL, then the prefix is added to the key without
 *        setting anything to the mask.
 *
 *        prefix_size
 *             |
 *             |
 *          '''''''
 *         +---------------------------+
 *  key =  | prefix |        key       |
 *         +---------------------------+
 *  mask = | 1....1 |        mask      |
 *         +---------------------------+
 *          '''''''''''''''''''''''''''
 *                     |
 *                     |
 *                  key_size
 *
 * \param [in] unit         - Device ID
 * \param [in] key          - Key buffer to add prefix to
 * \param [in] key_mask     - Key mask buffer to add 1s to
 * \param [in] key_size     - Size of the key
 * \param [in] prefix_value - Prefix value to add to the key
 * \param [in] prefix_size  - Size of the prefix
 * \return
 *  \retval _SHR_E_NONE - success
 *
 */
static shr_error_e
dnx_field_tcam_access_key_prefix_add(
    int unit,
    uint32 *key,
    uint32 *key_mask,
    uint32 key_size,
    uint32 prefix_value,
    uint32 prefix_size)
{
    SHR_FUNC_INIT_VARS(unit);

    if (prefix_size > 0)
    {
        LOG_DEBUG_EX(BSL_LOG_MODULE, "Writing prefix %d (size %d)%s%s\n", prefix_value, prefix_size, EMPTY, EMPTY);
        SHR_BITCOPY_RANGE(key, key_size - prefix_size, &prefix_value, 0, prefix_size);
        if (key_mask != NULL)
        {
            SHR_BITSET_RANGE(key_mask, key_size - prefix_size, prefix_size);
        }
    }

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Adds the given key/key_mask/payload to TCAM at the given position.
 *  The entry is added to the TCAM HW at the given core.
 *
 * \param [in] unit             - Device ID
 * \param [in] core             - Core ID
 * \param [in] app_id           - The app ID that defines the context for the data to write
 * \param [in] position         - Position in TCAM to write the data to
 * \param [in] key              - Key data to write (Size is according to app_id)
 * \param [in] key_mask         - Key_mask to write (Size is according to app_id)
 * \param [in] payload          - Payload to write (Size is according to app_id)
 * \param [in] ent_payload_size - Entry payload size
 * \param [in] valid            - TRUE if entry is valid (when not, entry is written but not active)
 * \param [in] is_update        - If true, then entry exists and we want to update payload only
 *
 * \return
 *   \retval _SHR_E_NONE success
 */
static shr_error_e
dnx_field_tcam_access_entry_add_aux(
    int unit,
    int core,
    uint32 app_id,
    uint32 position,
    uint32 key[],
    uint32 key_mask[],
    uint32 payload[],
    uint32 ent_payload_size,
    uint8 valid,
    uint8 is_update)
{
    dnx_field_tcam_access_part_e part;
    uint8 odd;
    dnx_field_tcam_access_db_info db_info;
    dnx_field_tcam_access_key_mode_e key_mode;
    uint32 bank_id;
    uint32 bank_offset;
    uint32 key_to_write[MAX_DOUBLE_KEY_WORD_SIZE] = { 0 };
    uint32 mask_to_write[MAX_DOUBLE_KEY_WORD_SIZE] = { 0 };
    uint32 payload_to_write[MAX_DOUBLE_ACTION_WORD_SIZE] = { 0 };
    uint32 payload_msb_aligned[MAX_DOUBLE_ACTION_WORD_SIZE] = { 0 };
    uint8 is_special = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * app_id is equal to the tcam_handler_id in TCAM Manager, through which we retrieve the the key_size and payload_size
     */
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_info_get(unit, app_id, &db_info));

    /*
     * Position has 80b granularity and odd position point to the 80b msb-part of the entry.
     * Since databases with single/double-sized key are defined to start only from the 80b lsb-part of the entry, it's
     * an error to have odd position for anything other than databases with half-sized key.
     *
     *
     *                 single/double-sized keys
     *                 can only start from here
     *                         |
     *                         |
     *                         v
     *
     * position:   (n + 1)     n            (n is even)
     *           +---------+-----------+
     * key:      | 80 msb  |  80 lsb   |
     *           +---------+-----------+
     */
    odd = (position % 2);
    if (odd && db_info.key_size != dnx_data_field.tcam.key_size_half_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Database %d with key_size %d can't start from position %d", app_id,
                     db_info.key_size, position);
    }

    /**
     * Align payload to msb side to support ARR HW.
     * (ARR HW assumes payload starts from msb)
     *
     * For the given payload buffer:
     *
     * TCAM payload size  actual payload size
     *      |                 |
     *      v                 v
     *      +-----------------+----------+
     *      |                 |xxxxxxxxxx|
     *      +=================+==========+
     *      msb                          lsb
     *
     * We align the payload data to the msb side of the TCAM payload size,
     * before we actually write it to HW:
     *
     *                 payload is left-shifted by
     *                  (tcam_payload_size -
     *                   actual_payload_size)
     *                         |
     *                         v
     *                  <<---------------
     *      +----------+-----------------+
     *      |xxxxxxxxxx|                 |
     *      +==========+=================+
     *      msb                          lsb
     */
    SHR_BITCOPY_RANGE(payload_msb_aligned, db_info.payload_size - ent_payload_size, payload, 0, ent_payload_size);

    /*
     * Copy the key/payload to internal buffer, then add prefix to key/mask (in order not to modify input buffers)
     */
    SHR_BITCOPY_RANGE(key_to_write, 0, key, 0, db_info.key_size);
    SHR_BITCOPY_RANGE(mask_to_write, 0, key_mask, 0, db_info.key_size);
    SHR_BITCOPY_RANGE(payload_to_write, 0, payload_msb_aligned, 0, db_info.payload_size);
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_key_prefix_add
                    (unit, key_to_write, mask_to_write, db_info.key_size, db_info.prefix_value, db_info.prefix_size));

    /**
     * We add key/mask to SW MOCK at this stage, because later in the function, key/mask data might be shifted
     */
    if (ACCESS_SW_MOCK && !DNX_FIELD_TCAM_IS_STAGE_PMF(db_info.stage))
    {
        /*
         * For the scenario where we change the entry's key/mask, we delete from SW mock before writing the data. 
         */
        if (is_update)
        {
            SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw_hash_key_remove(unit, core, position));
        }
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw_hash_key_add
                        (unit, core, key_to_write, mask_to_write, db_info.key_size, position));
    }

    if (db_info.key_size == dnx_data_field.tcam.key_size_half_get(unit))
    {
        /*
         * For half-sized key, we write only to the upper/lower part of the key.
         * We treat odd addresses as MSB part and even addresses as LSB part.
         */
        part = (odd) ? DNX_FIELD_TCAM_ACCESS_PART_MSB : DNX_FIELD_TCAM_ACCESS_PART_LSB;

        /*
         * Copy the key to its relevant part (lsb/msb)
         */
        if (odd)
        {
            SHR_BITCOPY_RANGE(key_to_write, HALF_KEY_BIT_SIZE, key_to_write, 0, HALF_KEY_BIT_SIZE);
            SHR_BITCOPY_RANGE(mask_to_write, HALF_KEY_BIT_SIZE, mask_to_write, 0, HALF_KEY_BIT_SIZE);
            SHR_BITCOPY_RANGE(payload_to_write, HALF_ACTION_BIT_SIZE, payload_msb_aligned, 0, HALF_ACTION_BIT_SIZE);

            SHR_BITCLR_RANGE(key_to_write, 0, HALF_KEY_BIT_SIZE);
            SHR_BITCLR_RANGE(mask_to_write, 0, HALF_KEY_BIT_SIZE);
            SHR_BITCLR_RANGE(payload_to_write, 0, HALF_ACTION_BIT_SIZE);
        }
        /*
         * Since we've taken care of the oddness of the position, we can set it back to be even 
         */
        position = position - (position % 2);
    }
    else
    {
        /*
         * We write to both parts of the key for single/double-sized keys.
         */
        part = DNX_FIELD_TCAM_ACCESS_PART_ALL;
    }
    /**
     * Set valid according to part when non-zero.
     * 'part' value equals the 2 valid bits indication
     */
    if (valid != 0)
    {
        valid = part;
    }

    LOG_DEBUG_EX(BSL_LOG_MODULE, "Key(160b) after prefix: 0x%08x%08x%08x%08x", key[4], key[3], key[2], key[1]);
    LOG_DEBUG_EX(BSL_LOG_MODULE, "%08x%s%s%s\n", key[0], EMPTY, EMPTY, EMPTY);
    if (db_info.key_size == dnx_data_field.tcam.key_size_double_get(unit))
    {
        LOG_DEBUG_EX(BSL_LOG_MODULE, "MSB Key(160b) after prefix: 0x%08x%08x%08x%08x", key[9], key[8], key[7], key[6]);
        LOG_DEBUG_EX(BSL_LOG_MODULE, "%08x%s%s%s\n", key[5], EMPTY, EMPTY, EMPTY);
    }

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_size_to_mode(unit, db_info.key_size, &key_mode));
    LOG_DEBUG_EX(BSL_LOG_MODULE, "Key size: %d, key mode: %d%s%s\n", db_info.key_size, key_mode, EMPTY, EMPTY);
    /*
     * Convert absolute position to bank_id/bank_offset pair
     */
    bank_id = DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_ID(position);
    bank_offset = DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_OFFSET(position);

    if (db_info.key_size == dnx_data_field.tcam.key_size_double_get(unit))
    {
        /*
         * When double key is used in TCAM, then two conditions need to be met:
         * 1) The TCAM_BANK ID used is even
         * 2) The msb part single key needs to be written to the TCAM_BANK next to the one given in the
         *    address. For example, if address is for TCAM_BANK ID <n>, then the msb part is saved in
         *    TCAM_BANK ID <n+1> while the lsb part if saved in TCAM_BANK ID <n>
         *
         * So in addition to the normal single-sized key/payload we write to the given address, we need
         * to write another single-sized key/payload to the bank next to the given bank.
         *
         * +------------------------+-----+
         * | All sizes write        |  |  |
         * |                        |  v  |
         * +------------------------+-----+
         * | Double-size only write |  "  |
         * |                        |  v  |
         * +========================+=====+
         * 
         *   |        "          |            |                 "             "
         *   v        v          v            v                 v             v
         * BANK<n> BANK<n+1> | ACTION<2*n> ACTION<2*n+1> ACTION<2*(n+1)> ACTION<2*(n+1)+1>
         * +---+    +---+    |  +---+       +---+             +---+         +---+
         * | x |    | x |    |  | x |       | x |             | x |         | x |
         * +===+    +===+    |  +===+       +===+             +===+         +===+
         */
        uint32 *msb_key_part = key_to_write + SINGLE_KEY_WORD_SIZE;
        uint32 *msb_key_mask_part = mask_to_write + SINGLE_KEY_WORD_SIZE;
        uint32 *msb_payload_part = payload_to_write + SINGLE_ACTION_WORD_SIZE;
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_single_entry_write(unit, core, bank_id + 1, bank_offset,
                                                                 msb_key_part, msb_key_mask_part, msb_payload_part,
                                                                 key_mode, part, valid, FALSE));
    }

    /*
     * Special DBs are the ones that can reside on the same bank with DT DBs.
     * The sepcial thing about those DBs are that they have single_key_size/half_action_size combination
     * and therefore can use the whole bank with part of the payload, leaving the other part of the payload
     * to DT DBs.
     *
     * S for special DB, DT is for direct table DB:
     *
     * BANK<n> | ACTION<2*n> ACTION<2*n+1> 
     * +---+   |  +---+       +----+
     * | S |   |  | S |       | DT |
     * +===+   |  +===+       +====+
     *
     */
    if (db_info.key_size == dnx_data_field.tcam.key_size_single_get(unit)
        && db_info.payload_size == dnx_data_field.tcam.action_size_half_get(unit))
    {
        is_special = TRUE;
    }
    /*
     * Write single-sized key/payload to TCAM
     */
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_single_entry_write(unit, core, bank_id, bank_offset,
                                                             key_to_write, mask_to_write, payload_to_write,
                                                             key_mode, part, valid, is_special));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_access_entry_add(
    int unit,
    dbal_physical_tables_e physical_tables[DBAL_MAX_NUM_OF_PHY_DB_IN_TABLE],
    uint32 app_id,
    dbal_physical_entry_t * entry,
    uint32 flags)
{
    uint32 position;
    dnx_field_tcam_access_db_info db_info;
    dnx_field_tcam_core_e core = DNX_FIELD_TCAM_ENTRY_CORE_ID(entry->entry_hw_id);
    dnx_field_tcam_core_e core_iter;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_info_get(unit, app_id, &db_info));

    for (core_iter = 0; core_iter < dnx_data_device.general.nof_cores_get(unit); core_iter++)
    {
        if (core == DNX_FIELD_TCAM_CORE_ALL || core == core_iter)
        {
            /*
             * entry_hw_id is equal to the entry_id in TCAM Manager, so we use it to retrieve entry position
             */
            SHR_IF_ERR_EXIT(dnx_field_tcam_access_index_get(unit, core_iter, entry->entry_hw_id, &position));

            if (db_info.direct_table)
            {
                SHR_IF_ERR_EXIT(dnx_field_tcam_access_entry_dt_write_aux
                                (unit, core_iter, app_id, position, entry->payload, entry->payload_size,
                                 entry->indirect_commit_mode));
            }
            else
            {
                uint8 valid;
                /*
                 * Get valid bit 
                 */
                SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.valid_bmp.v_bit.bit_get(unit, core_iter, position, &valid));
                SHR_IF_ERR_EXIT(dnx_field_tcam_access_entry_add_aux
                                (unit, core_iter, app_id, position, entry->key, entry->k_mask, entry->payload,
                                 entry->payload_size, valid, entry->indirect_commit_mode));
                SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.entry_in_use_bmp.v_bit.bit_set(unit, core_iter, position));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Searches for the given key in the given bank_id at the given core (using TCAM CMP method)
 *  And returns the bank_offset for the given key if found.
 *
 * \param [in] unit         - Device ID
 * \param [in] core         - Core ID
 * \param [in] bank_id      - The bank ID to search in
 * \param [in] key          - The key to search for
 * \param [out] bank_offset - Bank offset for the given key if found
 * \param [out] found       - 1 if key was found in the given bank ID, 0 otherwise
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
dnx_field_tcam_access_compare(
    int unit,
    int core,
    int bank_id,
    uint32 key[DBAL_PHYSICAL_KEY_SIZE_IN_WORDS],
    int *bank_offset,
    uint8 *found)
{
    soc_reg_above_64_val_t mem_val;
    uint32 data[MAX_DOUBLE_KEY_WORD_SIZE] = { 0 };
    uint32 position;

    SHR_FUNC_INIT_VARS(unit);

    SOC_REG_ABOVE_64_CLEAR(mem_val);

    /*
     * In order to do compare for a certain bank, we only need to use a random position in the bank itself, so we
     * choose the first index of the bank as our position. 
     */
    position = DNX_FIELD_TCAM_LOCATION_BANK_TO_ABSOLUTE(bank_id, 0);

    SHR_BITCOPY_RANGE(data, 0, key, 0, SINGLE_KEY_BIT_SIZE);

    /*
     * Send compare command 
     */
    soc_mem_field32_set(unit, TCAM_TCAM_BANK_COMMANDm, mem_val, TCAM_CPU_CMD_WRf, 0x0);
    soc_mem_field32_set(unit, TCAM_TCAM_BANK_COMMANDm, mem_val, TCAM_CPU_CMD_RDf, 0x0);
    soc_mem_field32_set(unit, TCAM_TCAM_BANK_COMMANDm, mem_val, TCAM_CPU_CMD_CMPf, 0x1);
    soc_mem_field_set(unit, TCAM_TCAM_BANK_COMMANDm, mem_val, TCAM_CPU_CMD_DIf, data);

    SHR_IF_ERR_EXIT(WRITE_TCAM_TCAM_BANK_COMMANDm(unit, TCAM_BLOCK(unit, core), position, mem_val));

    /*
     * Check compare result 
     */
    SHR_IF_ERR_EXIT(READ_TCAM_TCAM_BANK_REPLYm(unit, TCAM_BLOCK(unit, core), position, mem_val));

    *found = soc_mem_field32_get(unit, TCAM_TCAM_BANK_REPLYm, mem_val, TCAM_CPU_REP_MATCHf);
    if (*found)
    {
        *bank_offset = soc_mem_field32_get(unit, TCAM_TCAM_BANK_REPLYm, mem_val, TCAM_CPU_REP_AINDEXf);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * /brief
 *  Returns the acces_id of the entry placed in the given absolute location
 *  in TCAM at the given core.
 *
 * \param [in] unit              - Device ID
 * \param [in] core              - Core ID
 * \param [in] absolute_location - The absolute location of the entry to returns the access
 *                                 id for.
 * \param [out] access_id        - The access_id for the entry in the given absolute location
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
dnx_field_tcam_access_entry_id_get(
    int unit,
    int core,
    uint32 absolute_location,
    uint32 *access_id)
{
    uint8 found = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get the access_id from the location_entry_hash
     */
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.
                    entry_location_hash.get_by_index(unit, core, absolute_location, access_id, &found));

    if (!found)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Entry at location: %d not found in location->entry Hash", absolute_location);
    }

exit:
    SHR_FUNC_EXIT;

}

static shr_error_e
dnx_field_tcam_access_key_to_access_id_hw(
    int unit,
    int core,
    uint32 key[DBAL_PHYSICAL_KEY_SIZE_IN_WORDS],
    uint32 *access_id)
{
    int bank_id;
    int bank_offset = 0;
    uint8 found = FALSE;
    uint32 absolute_location;

    SHR_FUNC_INIT_VARS(unit);

    for (bank_id = 0; bank_id < dnx_data_field.tcam.nof_banks_get(unit); bank_id++)
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_compare(unit, core, bank_id, key, &bank_offset, &found));
        if (found)
        {
            absolute_location = DNX_FIELD_TCAM_LOCATION_BANK_TO_ABSOLUTE(bank_id, bank_offset);
            /*
             * Retrieve access id from location 
             */
            SHR_IF_ERR_EXIT(dnx_field_tcam_access_entry_id_get(unit, core, absolute_location, access_id));
            break;
        }
    }

    if (!found)
    {
        SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_tcam_access_key_to_access_id_sw(
    int unit,
    int core,
    uint32 key[DBAL_PHYSICAL_KEY_SIZE_IN_WORDS],
    uint32 key_mask[DBAL_PHYSICAL_KEY_SIZE_IN_WORDS],
    uint32 key_size,
    uint32 *access_id)
{
    dnx_field_tcam_access_mapper_hash_key hash_key = { {0}, {0} };
    uint8 found;
    int bank_id;
    uint32 bank_offset;
    uint32 position;

    SHR_FUNC_INIT_VARS(unit);

    /** Get encoded key from hash */
    SHR_BITAND_RANGE(key, key_mask, 0, key_size, key);

    SHR_BITCOPY_RANGE(hash_key.key, 0, key, 0, key_size);
    SHR_BITCOPY_RANGE(hash_key.key_mask, 0, key_mask, 0, key_size);

    /**
     * Search all banks for the given key
     */
    for (bank_id = 0; bank_id < dnx_data_field.tcam.nof_banks_get(unit); bank_id++)
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_mapper_sw.
                        key_2_entry_id_hash.find(unit, core, bank_id, &hash_key, &bank_offset, &found));
        if (found)
        {
            break;
        }
    }

    if (!found)
    {
        SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_NOT_FOUND);
    }

    /*
     * Convert bank_id/offset found to the absolute position
     */
    position = DNX_FIELD_TCAM_LOCATION_BANK_TO_ABSOLUTE(bank_id, bank_offset);
    /*
     * Convert found position to the corresponding access_id
     */
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_entry_id_get(unit, core, position, access_id));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_access_key_to_access_id(
    int unit,
    int core,
    uint32 app_id,
    uint32 key[DBAL_PHYSICAL_KEY_SIZE_IN_WORDS],
    uint32 key_mask[DBAL_PHYSICAL_KEY_SIZE_IN_WORDS],
    uint32 *access_id)
{
    dnx_field_tcam_access_db_info db_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_info_get(unit, app_id, &db_info));

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_key_prefix_add
                    (unit, key, key_mask, db_info.key_size, db_info.prefix_value, db_info.prefix_size));

    if (SEARCH_KEY_IN_TCAM_HW)
    {
        SHR_IF_ERR_EXIT_NO_MSG(dnx_field_tcam_access_key_to_access_id_hw(unit, core, key, access_id));
    }
    else
    {
        SHR_IF_ERR_EXIT_NO_MSG(dnx_field_tcam_access_key_to_access_id_sw
                               (unit, core, key, key_mask, db_info.key_size, access_id));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Deletes the entry at the given position from TCAM at the given core.
 *  Deletion is performed by setting valid to 0.
 *
 * \param [in] unit     - Device ID
 * \param [in] core     - Core ID
 * \param [in] app_id   - The app ID that defines the context for the data to get
 * \param [in] position - Position to detele entry from
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
dnx_field_tcam_access_entry_delete_aux(
    int unit,
    int core,
    uint32 app_id,
    uint32 position)
{
    dnx_field_tcam_access_db_info db_info;
    dnx_field_tcam_access_part_e part;
    uint8 is_special = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_info_get(unit, app_id, &db_info));

    if (ACCESS_SW_MOCK && !DNX_FIELD_TCAM_IS_STAGE_PMF(db_info.stage))
    {
        /**
         * Delete entry from SW mock
         */
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw_hash_key_remove(unit, core, position));
    }

    if (db_info.key_size == dnx_data_field.tcam.key_size_half_get(unit))
    {
        part = (position % 2) ? DNX_FIELD_TCAM_ACCESS_PART_MSB : DNX_FIELD_TCAM_ACCESS_PART_LSB;
        position -= (position % 2);
    }
    else
    {
        part = DNX_FIELD_TCAM_ACCESS_PART_ALL;
    }

    if (db_info.key_size == dnx_data_field.tcam.key_size_single_get(unit)
        && db_info.payload_size == dnx_data_field.tcam.action_size_half_get(unit))
    {
        is_special = TRUE;
    }

    /*
     * Invalidate entry 
     */
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_entry_invalidate(unit, core, position, part, is_special));
    if (db_info.key_size == dnx_data_field.tcam.key_size_double_get(unit))
    {
        /*
         * If double key, invalidate the corresponding entry in the consecutive bank.
         */
        int bank_id = DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_ID(position) + 1;
        int bank_offset = DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_OFFSET(position);
        int second_part_double_position = DNX_FIELD_TCAM_LOCATION_BANK_TO_ABSOLUTE(bank_id, bank_offset);
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_entry_invalidate(unit, core, second_part_double_position, part, FALSE));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_access_entry_delete(
    int unit,
    dbal_physical_tables_e physical_tables[DBAL_MAX_NUM_OF_PHY_DB_IN_TABLE],
    uint32 app_id,
    dbal_physical_entry_t * entry,
    uint32 flags)
{
    dnx_field_tcam_access_db_info db_info;
    uint32 position;
    dnx_field_tcam_core_e core = DNX_FIELD_TCAM_ENTRY_CORE_ID(entry->entry_hw_id);
    dnx_field_tcam_core_e core_iter;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_info_get(unit, app_id, &db_info));
    for (core_iter = 0; core_iter < dnx_data_device.general.nof_cores_get(unit); core_iter++)
    {
        if (core == DNX_FIELD_TCAM_CORE_ALL || core == core_iter)
        {
            /*
             * entry_hw_id is equal to the entry_id in TCAM Manager, so we use it to retrieve entry position
             */
            SHR_IF_ERR_EXIT(dnx_field_tcam_access_index_get(unit, core_iter, entry->entry_hw_id, &position));

            if (db_info.direct_table)
            {
                uint32 payload[DBAL_PHYSICAL_RES_SIZE_IN_WORDS] = { 0 };

                /*
                 * We delete DT entry by writing 0's to its payload 
                 */
                SHR_IF_ERR_EXIT(dnx_field_tcam_access_entry_dt_write_aux
                                (unit, core_iter, app_id, position, payload, entry->payload_size, TRUE));
            }
            else
            {
                SHR_IF_ERR_EXIT(dnx_field_tcam_access_entry_delete_aux(unit, core_iter, app_id, position));
                SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.entry_in_use_bmp.v_bit.bit_clear(unit, core_iter, position));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_access_move(
    int unit,
    int core,
    uint32 app_id,
    uint32 position_from,
    uint32 position_to)
{
    uint32 key[MAX_DOUBLE_KEY_WORD_SIZE + 1];
    uint32 key_mask[MAX_DOUBLE_KEY_WORD_SIZE + 1];
    uint32 payload[MAX_DOUBLE_ACTION_WORD_SIZE];
    dnx_field_tcam_access_db_info db_info;
    uint8 valid;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_info_get(unit, app_id, &db_info));

    /*
     * We use full DB payload size as entry size since we are doing a move operation
     * and specifying full DB payload size disables msb alignment during get/add which is
     * not needed in this case.
     */
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_entry_get_aux
                    (unit, core, app_id, position_from, db_info.payload_size, key, key_mask, payload));
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_entry_delete_aux(unit, core, app_id, position_from));
    /*
     * Disable entry_in_use for deleted entry 
     */
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.entry_in_use_bmp.v_bit.bit_clear(unit, core, position_from));

    /*
     * Get valid for new position 
     */
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.valid_bmp.v_bit.bit_get(unit, core, position_to, &valid));
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_entry_add_aux(unit, core, app_id, position_to, key, key_mask, payload, db_info.payload_size, valid, FALSE     /* is_update 
                                                                                                                                                         */ ));
    /*
     * Enable entry_in_use for added entry 
     */
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.entry_in_use_bmp.v_bit.bit_set(unit, core, position_to));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_access_iterator_init(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_iterator_t * physical_entry_iterator)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * First location to start iterating from 
     */
    /*
     * The entry index is what we call location/position in TCAM 
     */
    physical_entry_iterator->mdb_entry_index = -1;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_access_iterator_get_next(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_iterator_t * physical_entry_iterator,
    dbal_physical_entry_t * entry,
    uint8 *is_end)
{
    uint32 *location = &(physical_entry_iterator->mdb_entry_index);
    uint8 found;
    dnx_field_tcam_access_db_info db_info;
    int core = 0;
    uint32 unsupported_hitbit_flags;
    uint8 hitbit_flags = entry->hitbit;

    SHR_FUNC_INIT_VARS(unit);

    if ((unsupported_hitbit_flags =
         (hitbit_flags &
          ~(DBAL_PHYSICAL_KEY_HITBIT_GET | DBAL_PHYSICAL_KEY_HITBIT_PRIMARY | DBAL_PHYSICAL_KEY_HITBIT_SECONDARY |
            DBAL_PHYSICAL_KEY_HITBIT_CLEAR))))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Flags 0x%X are not supported!", unsupported_hitbit_flags);
    }

    /*
     * Move to next location 
     */
    
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_info_get(unit, app_id, &db_info));

    while (TRUE)
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_handler_next_get(unit, core, app_id, *location, location, &found));

        if (found)
        {
            uint32 access_id;
            /*
             * Get the entry data at the next location 
             */
            SHR_SET_CURRENT_ERR(dnx_field_tcam_access_entry_get_aux
                                (unit, core, app_id, *location, entry->payload_size, entry->key, entry->k_mask,
                                 entry->payload));

            if (SHR_FUNC_VAL_IS(_SHR_E_NOT_FOUND))
            {
                /*
                 * Ignore HW not-found errors and continue to look for the next entry 
                 */
                SHR_SET_CURRENT_ERR(_SHR_E_NONE);
                continue;
            }
            else
            {
                /*
                 * Exit if any other error occurred 
                 */
                SHR_IF_ERR_EXIT(SHR_GET_CURRENT_ERR());
            }

            /*
             * Updates the access ID that was found in the entry handle.
             */
            SHR_IF_ERR_EXIT(dnx_field_tcam_access_entry_id_get(unit, core, *location, &access_id));
            entry->entry_hw_id = access_id;

            if (hitbit_flags &
                (DBAL_PHYSICAL_KEY_HITBIT_GET | DBAL_PHYSICAL_KEY_HITBIT_PRIMARY | DBAL_PHYSICAL_KEY_HITBIT_SECONDARY |
                 DBAL_PHYSICAL_KEY_HITBIT_CLEAR))
            {
                SHR_IF_ERR_EXIT(dnx_field_tcam_access_tcam_hit_get(unit, entry->entry_hw_id, &entry->hitbit));
                /** In case CLEAR flag is requested, call the flush API. */
                if (hitbit_flags & DBAL_PHYSICAL_KEY_HITBIT_CLEAR)
                {
                    SHR_IF_ERR_EXIT(dnx_field_tcam_access_tcam_hit_flush(unit, entry->entry_hw_id));
                }
            }
        }
        /*
         * Exit loop when no more access IDs exist, or when entry was found 
         */
        break;
    }

    /*
     * If not found, we're at the end 
     */
    *is_end = !found;

    /** If the entry is not found, the hitbit value should be zeroed. */
    if (!found)
    {
        entry->hitbit = 0;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_access_iterator_deinit(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_iterator_t * physical_entry_iterator)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_FUNC_EXIT;
}

static void
dnx_ser_tcam_single_entry_print(
    int unit,
    uint32 entryId,
    uint32 valid_bits,
    uint32 *key,
    uint32 *mask,
    uint32 ecc_key,
    uint32 ecc_mask)
{
    uint32 key_mode0 = 0, key_mode1 = 0, key_mode;
    uint32 half_key0[MAX_HALF_KEY_WORD_SIZE] = { 0 };
    uint32 half_key1[MAX_HALF_KEY_WORD_SIZE] = { 0 };
    char *str_key_mode[] = { "Half", "Single", "Invalid", "Double" };
    char str_key0[(MAX_HALF_KEY_WORD_SIZE * 8) + 3];
    char str_key1[(MAX_HALF_KEY_WORD_SIZE * 8) + 3];

    SHR_BITCOPY_RANGE(&key_mode0, 0, key, 0, KEY_MODE_BIT_SIZE);
    SHR_BITCOPY_RANGE(&key_mode1, 0, key, HALF_KEY_BIT_SIZE + KEY_MODE_BIT_SIZE, KEY_MODE_BIT_SIZE);
    SHR_BITCOPY_RANGE(half_key0, 0, key, KEY_MODE_BIT_SIZE, HALF_KEY_BIT_SIZE);
    SHR_BITCOPY_RANGE(half_key1, 0, key, HALF_KEY_BIT_SIZE + 2 * KEY_MODE_BIT_SIZE, HALF_KEY_BIT_SIZE);

    if (valid_bits == 0)
    {
        cli_out("Error: tcam entry is not valid!!\n");
        return;
    }
    else if (valid_bits == 3)
    {
        /*
         * 2 half entries are both valid, make sure 2 mode are match 
         */
        if (key_mode0 != key_mode1)
        {
            cli_out("Error: key_mode0 and key_mode1 are not matched!!(%d vs %d)\n", key_mode0, key_mode1);
            return;
        }
        key_mode = key_mode0;
    }
    else
    {
        /*
         * Only half entry is valid, key mode must be Half 
         */
        if (valid_bits == 1)
        {
            key_mode = key_mode0;
        }
        else
        {
            key_mode = key_mode1;
        }
        if (key_mode != DNX_FIELD_TCAM_ACCESS_KEY_MODE_HALF)
        {
            cli_out("Error: key mode is error, valid_bits=%d, key_mode0=%d, key_mode1=%d\n",
                    valid_bits, key_mode0, key_mode1);
            return;
        }
    }
    _shr_format_long_integer(str_key0, half_key0, BITS2BYTES(HALF_KEY_BIT_SIZE));
    _shr_format_long_integer(str_key1, half_key1, BITS2BYTES(HALF_KEY_BIT_SIZE));
    cli_out("%-8d%-8d%-10s%-26s%-26s0x%-8X\n", entryId, valid_bits,
            str_key_mode[key_mode], str_key0, str_key1, ecc_key);

    /*
     * print mask 
     */
    SHR_BITCOPY_RANGE(&key_mode0, 0, mask, 0, KEY_MODE_BIT_SIZE);
    SHR_BITCOPY_RANGE(&key_mode1, 0, mask, HALF_KEY_BIT_SIZE + KEY_MODE_BIT_SIZE, KEY_MODE_BIT_SIZE);
    SHR_BITCOPY_RANGE(half_key0, 0, mask, KEY_MODE_BIT_SIZE, HALF_KEY_BIT_SIZE);
    SHR_BITCOPY_RANGE(half_key1, 0, mask, HALF_KEY_BIT_SIZE + 2 * KEY_MODE_BIT_SIZE, HALF_KEY_BIT_SIZE);
    _shr_format_long_integer(str_key0, half_key0, BITS2BYTES(HALF_KEY_BIT_SIZE));
    _shr_format_long_integer(str_key1, half_key1, BITS2BYTES(HALF_KEY_BIT_SIZE));
    cli_out("%-16s%d/%d%-7s%-26s%-26s0x%-8X\n", "", key_mode0, key_mode1, "", str_key0, str_key1, ecc_mask);
}

static int
dnx_ser_tcam_entry_is_equal(
    int unit,
    uint32 valid_bit1,
    uint32 *key1,
    uint32 *mask1,
    uint32 valid_bit2,
    uint32 *key2,
    uint32 *mask2)
{
    int rc = TRUE;
    if (valid_bit1 != valid_bit2)
    {
        return FALSE;
    }

    if (valid_bit1 & 0x1)
    {
        /*
         * First half entry is valid 
         */
        if (!SHR_BITEQ_RANGE(key1, key2, 0, HALF_KEY_BIT_SIZE + KEY_MODE_BIT_SIZE))
        {
            rc = FALSE;
        }
        if (!SHR_BITEQ_RANGE(mask1, mask2, 0, HALF_KEY_BIT_SIZE + KEY_MODE_BIT_SIZE))
        {
            rc = FALSE;
        }
    }
    if (valid_bit1 & 0x2)
    {
        /*
         * Second half entry is valid 
         */
        if (!SHR_BITEQ_RANGE(key1, key2, HALF_KEY_BIT_SIZE + KEY_MODE_BIT_SIZE, HALF_KEY_BIT_SIZE + KEY_MODE_BIT_SIZE))
        {
            rc = FALSE;
        }
        if (!SHR_BITEQ_RANGE
            (mask1, mask2, HALF_KEY_BIT_SIZE + KEY_MODE_BIT_SIZE, HALF_KEY_BIT_SIZE + KEY_MODE_BIT_SIZE))
        {
            rc = FALSE;
        }
    }

    return rc;
}

/**
 * \brief
 *  Dump changed tcam entries for specific bank id
 *
 * \param [in] unit - unit
 * \param [in] core_id - Core ID
 * \param [in] bank_id - The TCAM bank id
 * \param [in] mode - dump mode
 *                      0: dump from HW
 *                      1: dump from shadow
 *                      2: match check between shsadow & HW
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
shr_error_e
dnx_ser_tcam_access_entry_key_dump(
    int unit,
    int core_id,
    uint32 bank_id,
    uint32 mode)
{
    uint32 ii, bank_offset, max_entry, tcam_bank_address;
    uint8 shadow_key_valid_bits, key_valid_bits;
    uint32 shadow_key[(MAX_SINGLE_KEY_WORD_SIZE + 1)] = { 0 };
    uint32 shadow_mask[(MAX_SINGLE_KEY_WORD_SIZE + 1)] = { 0 };
    uint32 data_key[(MAX_SINGLE_KEY_WORD_SIZE + 1)] = { 0 };
    uint32 data_mask[(MAX_SINGLE_KEY_WORD_SIZE + 1)] = { 0 };
    soc_reg_above_64_val_t mem_val_read;
    soc_reg_above_64_val_t mem_val_write;
    soc_mem_t ecc_mem;
    uint32 ecc_key, ecc_mask;
    int core = core_id;

    SHR_FUNC_INIT_VARS(unit);

    cli_out("dnx_ser_tcam_access_entry_key_dump: core_id=%d, bank_id=%d, mode=%d\n", core_id, bank_id, mode);

    if (bank_id < dnx_data_field.tcam.nof_big_banks_get(unit))
    {
        max_entry = dnx_data_field.tcam.nof_big_bank_lines_get(unit) / 2;
        ecc_mem = TCAM_TCAM_ENTRY_ECCm;
    }
    else if (bank_id < dnx_data_field.tcam.nof_big_banks_get(unit) + dnx_data_field.tcam.nof_small_banks_get(unit))
    {
        max_entry = dnx_data_field.tcam.nof_small_bank_lines_get(unit) / 2;
        ecc_mem = TCAM_TCAM_ENTRY_ECC_SMALLm;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error. tcam bank_id (%d) was not supported.\n", bank_id);
    }

    cli_out("%-8s%-8s%-10s%-26s%-26s%-8s\n", "EntryID", "Valid", "KeyMode",
            "Half-Key&Mask-0", "Half-Key&Mask-1", "ECC");
    cli_out("-----------------------------------------------------------------------\n");
    for (ii = 0; ii < max_entry; ii++)
    {
        bank_offset = ii * 2;
        tcam_bank_address = DNX_FIELD_TCAM_LOCATION_BANK_TO_ABSOLUTE(bank_id, bank_offset);

        /*
         * Read mask from shadow position + 1. 
         */
         /* coverity[ARRAY_VS_SINGLETON:FALSE]  */
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_entry_key_read_shadow
                        (unit, core, bank_id, bank_offset + 1, shadow_mask, &shadow_key_valid_bits));

        /*
         * Read key from shadow. 
         */
         /* coverity[ARRAY_VS_SINGLETON:FALSE]  */
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_entry_key_read_shadow
                        (unit, core, bank_id, bank_offset, shadow_key, &shadow_key_valid_bits));

        /*
         * Read TCAM entry from HW 
         */
        SOC_REG_ABOVE_64_CLEAR(mem_val_write);
        soc_mem_field32_set(unit, TCAM_TCAM_BANK_COMMANDm, mem_val_write, TCAM_CPU_CMD_WRf, 0x0);
        soc_mem_field32_set(unit, TCAM_TCAM_BANK_COMMANDm, mem_val_write, TCAM_CPU_CMD_RDf, 0x1);
        soc_mem_field32_set(unit, TCAM_TCAM_BANK_COMMANDm, mem_val_write, TCAM_CPU_CMD_CMPf, 0x0);
        /*
         * Read key from hw position(key) 
         */
        SHR_IF_ERR_EXIT(WRITE_TCAM_TCAM_BANK_COMMANDm(unit, TCAM_BLOCK(unit, core), tcam_bank_address, mem_val_write));
        SOC_REG_ABOVE_64_CLEAR(mem_val_read);
        SHR_IF_ERR_EXIT(READ_TCAM_TCAM_BANK_REPLYm(unit, TCAM_BLOCK(unit, core), tcam_bank_address, mem_val_read));
        key_valid_bits = soc_mem_field32_get(unit, TCAM_TCAM_BANK_REPLYm, mem_val_read, TCAM_CPU_REP_VALIDf);
        soc_mem_field_get(unit, TCAM_TCAM_BANK_REPLYm, mem_val_read, TCAM_CPU_REP_DOUTf, data_key);

        /*
         * Read key mask from position + 1(mask) 
         */
        SHR_IF_ERR_EXIT(WRITE_TCAM_TCAM_BANK_COMMANDm
                        (unit, TCAM_BLOCK(unit, core), tcam_bank_address + 1, mem_val_write));
        SOC_REG_ABOVE_64_CLEAR(mem_val_read);
        SHR_IF_ERR_EXIT(READ_TCAM_TCAM_BANK_REPLYm(unit, TCAM_BLOCK(unit, core), tcam_bank_address + 1, mem_val_read));
        soc_mem_field_get(unit, TCAM_TCAM_BANK_REPLYm, mem_val_read, TCAM_CPU_REP_DOUTf, data_mask);

        /*
         * Read ECC value from ECC memory
         */
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, ecc_mem, bank_id, TCAM_BLOCK(unit, core), bank_offset, &ecc_key));
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, ecc_mem, bank_id, TCAM_BLOCK(unit, core), bank_offset + 1, &ecc_mask));

        if (mode == 0)
        {
            /*
             * dump tcam entry from HW 
             */
            if (key_valid_bits)
            {
                dnx_ser_tcam_single_entry_print(unit, ii, key_valid_bits, data_key, data_mask, ecc_key, ecc_mask);
            }
        }
        else if (mode == 1)
        {
            /*
             * dump tcam entry from shadow 
             */
            if (shadow_key_valid_bits)
            {
                dnx_ser_tcam_single_entry_print(unit, ii, shadow_key_valid_bits, shadow_key, shadow_mask, ecc_key,
                                                ecc_mask);
            }
        }
        else if (mode == 2)
        {
            /*
             * Check if shadow value match HW value 
             */
            if (!dnx_ser_tcam_entry_is_equal(unit, key_valid_bits, data_key, data_mask,
                                             shadow_key_valid_bits, shadow_key, shadow_mask))
            {
                cli_out("%-d:    TCAM shadow value is not match HW\n", ii);
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Writes the given key and key_mask to the given position, Just for SER
 *  debug purpose, Don't update shadow at this function.
 *
 * \param [in] unit        - Device ID
 * \param [in] core_id     - Core ID
 * \param [in] bank_id     - The TCAM_BANK ID to read from
 * \param [in] bank_offset - The offset inside the TCAM_BANK to read from, by counting Single sized entries.
 *                           Equal to the TCAM bank offset, twice the action bank offset.
 * \param [in] valid       - The valid bit
 * \param [in] key_mode    - The search mode (is set according to key size)
 * \param [in] key         - The key buffer to write
 * \param [in] mask        - The key_mask buffer to write
 *
 * \return
 *   \retval _SHR_E_NONE success
 *
 * \remark
 * SPECIAL OUTPUT:
 *     HW:
 *        TCAM_TCAM_BANK_COMMAND - The memory to write data/command to
 */
shr_error_e
dnx_ser_tcam_access_write_hw_entry(
    int unit,
    int core_id,
    uint32 bank_id,
    uint32 bank_offset,
    uint32 valid,
    uint32 key_mode,
    uint32 *key,
    uint32 *mask)
{
    soc_reg_above_64_val_t mem_val;
    uint32 data[MAX_DOUBLE_KEY_WORD_SIZE + 1] = { 0 };
    uint32 encoded_key[MAX_SINGLE_KEY_WORD_SIZE + 1] = { 0 };
    uint32 encoded_mask[MAX_SINGLE_KEY_WORD_SIZE + 1] = { 0 };
    uint32 tcam_bank_address;
    int core = core_id;

    SHR_FUNC_INIT_VARS(unit);

    tcam_bank_address = DNX_FIELD_TCAM_LOCATION_BANK_TO_ABSOLUTE(bank_id, bank_offset);

    SHR_BITCOPY_RANGE(encoded_key, 0, &key_mode, 0, KEY_MODE_BIT_SIZE);
    SHR_BITCOPY_RANGE(encoded_key, KEY_MODE_BIT_SIZE, key, 0, HALF_KEY_BIT_SIZE);
    SHR_BITCOPY_RANGE(encoded_key, HALF_KEY_BIT_SIZE + KEY_MODE_BIT_SIZE, &key_mode, 0, KEY_MODE_BIT_SIZE);
    SHR_BITCOPY_RANGE(encoded_key, HALF_KEY_BIT_SIZE + 2 * KEY_MODE_BIT_SIZE, key,
                      HALF_KEY_BIT_SIZE, HALF_KEY_BIT_SIZE);
    SHR_BITSET_RANGE(encoded_mask, 0, KEY_MODE_BIT_SIZE);
    SHR_BITCOPY_RANGE(encoded_mask, KEY_MODE_BIT_SIZE, mask, 0, HALF_KEY_BIT_SIZE);
    SHR_BITSET_RANGE(encoded_mask, HALF_KEY_BIT_SIZE + KEY_MODE_BIT_SIZE, KEY_MODE_BIT_SIZE);
    SHR_BITCOPY_RANGE(encoded_mask, HALF_KEY_BIT_SIZE + 2 * KEY_MODE_BIT_SIZE, mask,
                      HALF_KEY_BIT_SIZE, HALF_KEY_BIT_SIZE);

    SHR_BITCOPY_RANGE(data, 0, encoded_key, 0, SINGLE_KEY_BIT_SIZE + 2 * KEY_MODE_BIT_SIZE);
    SHR_BITCOPY_RANGE(data, SINGLE_KEY_BIT_SIZE + 2 * KEY_MODE_BIT_SIZE,
                      encoded_mask, 0, SINGLE_KEY_BIT_SIZE + 2 * KEY_MODE_BIT_SIZE);

    soc_mem_field32_set(unit, TCAM_TCAM_BANK_COMMANDm, mem_val, TCAM_CPU_CMD_WRf, 0x3);
    soc_mem_field32_set(unit, TCAM_TCAM_BANK_COMMANDm, mem_val, TCAM_CPU_CMD_RDf, 0x0);
    soc_mem_field32_set(unit, TCAM_TCAM_BANK_COMMANDm, mem_val, TCAM_CPU_CMD_CMPf, 0x0);
    soc_mem_field_set(unit, TCAM_TCAM_BANK_COMMANDm, mem_val, TCAM_CPU_CMD_DIf, data);
    soc_mem_field32_set(unit, TCAM_TCAM_BANK_COMMANDm, mem_val, TCAM_CPU_CMD_VALIDf, valid);
    if (ACCESS_HW)
    {
        SHR_IF_ERR_EXIT(WRITE_TCAM_TCAM_BANK_COMMANDm(unit, TCAM_BLOCK(unit, core), tcam_bank_address, mem_val));
    }

exit:
    cli_out("dnx_ser_tcam_access_write_hw_entry, core_id=%d, bank_id=%d, bank_offset=%d, valid=%d, key_mode=%d\n",
            core_id, bank_id, bank_offset, valid, key_mode);
    cli_out("\t%-16s%08X %08X %08X %08X %08X\n", "Key:", key[0], key[1], key[2], key[3], key[4]);
    cli_out("\t%-16s%08X %08X %08X %08X %08X\n", "Mask:", mask[0], mask[1], mask[2], mask[3], mask[4]);
    cli_out("\t%-16s%08X %08X %08X %08X %08X %08X\n", "encoded_key:",
            encoded_key[0], encoded_key[1], encoded_key[2], encoded_key[3], encoded_key[4], encoded_key[5]);
    cli_out("\t%-16s%08X %08X %08X %08X %08X %08X\n", "encoded_mask:",
            encoded_mask[0], encoded_mask[1], encoded_mask[2], encoded_mask[3], encoded_mask[4], encoded_mask[5]);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Rewrite an tcam entry from shadow. Used for TCAM ser correction
 *
 * \param [in] unit         - unit
 * \param [in] core_id      - Core ID
 * \param [in] bank_id      - The TCAM bank id
 * \param [in] bank_offset  - The TCAM entry offset
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
shr_error_e
dnx_ser_tcam_access_rewrite_key_from_shadow(
    int unit,
    int core_id,
    uint32 bank_id,
    uint32 bank_offset)
{
    soc_reg_above_64_val_t mem_val;
    uint32 data[MAX_DOUBLE_KEY_WORD_SIZE + 1] = { 0 };
    uint32 shadow_key[(MAX_SINGLE_KEY_WORD_SIZE + 1)] = { 0 };
    uint32 shadow_mask[(MAX_SINGLE_KEY_WORD_SIZE + 1)] = { 0 };
    uint8 shadow_key_valid, shadow_mask_valid;
    uint32 tcam_bank_address;
    int core = core_id;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Make sure tcam shadow is keeped 
     */
    if (!DNX_FIELD_TCAM_SHADOW_KEEP)
    {
        SHR_IF_ERR_EXIT(_SHR_E_RESOURCE);
    }

    /*
     * adjust bank_offset to make sure it point key offset 
     */
    bank_offset = bank_offset - (bank_offset & 0x1);
    /*
     * Read key from shadow. 
     */
     /* coverity[ARRAY_VS_SINGLETON:FALSE]  */
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_entry_key_read_shadow
                    (unit, core, bank_id, bank_offset, shadow_key, &shadow_key_valid));
    /*
     * Read mask from shadow position + 1. 
     */
     /* coverity[ARRAY_VS_SINGLETON:FALSE]  */
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_entry_key_read_shadow
                    (unit, core, bank_id, bank_offset + 1, shadow_mask, &shadow_mask_valid));

    /** Copy data_mask||data_key to data */
    SHR_BITCOPY_RANGE(data, 0, shadow_key, 0, SINGLE_KEY_BIT_SIZE + 2 * KEY_MODE_BIT_SIZE);
    SHR_BITCOPY_RANGE(data, SINGLE_KEY_BIT_SIZE + 2 * KEY_MODE_BIT_SIZE,
                      shadow_mask, 0, SINGLE_KEY_BIT_SIZE + 2 * KEY_MODE_BIT_SIZE);

    cli_out("dnx_ser_tcam_access_rewrite_key_from_shadow: core_id=%d, bank_id=%d, bank_offset=%d\n",
            core_id, bank_id, bank_offset);
    dnx_ser_tcam_single_entry_print(unit, bank_offset, shadow_key_valid, shadow_key, shadow_mask, 0, 0);

    tcam_bank_address = DNX_FIELD_TCAM_LOCATION_BANK_TO_ABSOLUTE(bank_id, bank_offset);
    soc_mem_field32_set(unit, TCAM_TCAM_BANK_COMMANDm, mem_val, TCAM_CPU_CMD_WRf, 3);
    soc_mem_field32_set(unit, TCAM_TCAM_BANK_COMMANDm, mem_val, TCAM_CPU_CMD_RDf, 0x0);
    soc_mem_field32_set(unit, TCAM_TCAM_BANK_COMMANDm, mem_val, TCAM_CPU_CMD_CMPf, 0x0);
    soc_mem_field_set(unit, TCAM_TCAM_BANK_COMMANDm, mem_val, TCAM_CPU_CMD_DIf, data);
    soc_mem_field32_set(unit, TCAM_TCAM_BANK_COMMANDm, mem_val, TCAM_CPU_CMD_VALIDf, shadow_key_valid);
    if (ACCESS_HW)
    {
        SHR_IF_ERR_EXIT(WRITE_TCAM_TCAM_BANK_COMMANDm(unit, TCAM_BLOCK(unit, core), tcam_bank_address, mem_val));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Enable/Disable tcam ecc protect machine
 *
 * \param [in] unit - unit
 * \param [in] enable - 1: enable machine; 0:disable machine
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
shr_error_e
dnx_ser_tcam_protect_machine_enable(
    int unit,
    int enable)
{
    SHR_FUNC_INIT_VARS(unit);

    if (enable)
    {
        SHR_IF_ERR_EXIT(soc_reg_field32_modify(unit, TCAM_TCAM_PROTECTION_MACHINE_CONFIGURATIONr,
                                               REG_PORT_ANY, TCAM_PROTECTION_BANK_BITMAPf, 0xFFFF));
        SHR_IF_ERR_EXIT(soc_reg_field32_modify(unit, TCAM_TCAM_ECC_FIX_ENr, REG_PORT_ANY, TCAM_ECC_FIX_ENf, 0x1));
        SHR_IF_ERR_EXIT(soc_reg_field32_modify(unit, TCAM_TCAM_PROTECTION_TRIGGERr,
                                               REG_PORT_ANY, TCAM_PROTECTION_TRIGGERf, 0x1));
    }
    else
    {
        SHR_IF_ERR_EXIT(soc_reg_field32_modify(unit, TCAM_TCAM_PROTECTION_TRIGGERr,
                                               REG_PORT_ANY, TCAM_PROTECTION_TRIGGERf, 0));
        SHR_IF_ERR_EXIT(soc_reg_field32_modify(unit, TCAM_TCAM_ECC_FIX_ENr, REG_PORT_ANY, TCAM_ECC_FIX_ENf, 0));
        SHR_IF_ERR_EXIT(soc_reg_field32_modify(unit, TCAM_TCAM_PROTECTION_MACHINE_CONFIGURATIONr,
                                               REG_PORT_ANY, TCAM_PROTECTION_BANK_BITMAPf, 0));
    }
exit:
    SHR_FUNC_EXIT;
}
