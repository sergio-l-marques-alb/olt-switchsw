#include <soc/drv.h>
#include "tcam_access_hw_v1.h"

#include <soc/dnx/dnx_data/auto_generated/dnx_data_field.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_tcam_location_types.h>

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDPROCDNX

shr_error_e
dnx_tcam_access_hw_v1_payload_write(
    int unit,
    int core,
    uint32 action_bank_id,
    uint32 bank_offset,
    uint32 *payload)
{
    uint32 data[MAX_DOUBLE_ACTION_WORD_SIZE] = { 0 };
    soc_mem_t action_tbl;

    SHR_FUNC_INIT_VARS(unit);

    if (action_bank_id < dnx_data_field.tcam.nof_big_banks_get(unit) * 2)
    {
        action_tbl = TCAM_TCAM_ACTIONm;
    }
    else
    {
        action_tbl = TCAM_TCAM_ACTION_SMALLm;
    }

    soc_mem_field_set(unit, action_tbl, data, ACTIONf, payload);

    SHR_IF_ERR_EXIT(soc_mem_array_write(unit, action_tbl, action_bank_id, TCAM_BLOCK(unit, core), bank_offset, data));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_tcam_access_hw_v1_entry_key_read(
    int unit,
    int core,
    uint32 bank_id,
    uint32 bank_offset,
    uint32 *data_key,
    uint32 *data_mask,
    uint8 *key_valid_bits)
{
    soc_reg_above_64_val_t mem_val_read;
    soc_reg_above_64_val_t mem_val_write;
    uint32 tcam_bank_address;

    SHR_FUNC_INIT_VARS(unit);

    SOC_REG_ABOVE_64_CLEAR(mem_val_write);

    tcam_bank_address = DNX_FIELD_TCAM_LOCATION_BANK_TO_ABSOLUTE(bank_id, bank_offset);

    soc_mem_field32_set(unit, TCAM_TCAM_BANK_COMMANDm, mem_val_write, TCAM_CPU_CMD_WRf, 0x0);
    soc_mem_field32_set(unit, TCAM_TCAM_BANK_COMMANDm, mem_val_write, TCAM_CPU_CMD_RDf, 0x1);
    soc_mem_field32_set(unit, TCAM_TCAM_BANK_COMMANDm, mem_val_write, TCAM_CPU_CMD_CMPf, 0x0);
    /*
     * Read key from position
     */
    SHR_IF_ERR_EXIT(WRITE_TCAM_TCAM_BANK_COMMANDm(unit, TCAM_BLOCK(unit, core), tcam_bank_address, mem_val_write));

    SOC_REG_ABOVE_64_CLEAR(mem_val_read);
    SHR_IF_ERR_EXIT(READ_TCAM_TCAM_BANK_REPLYm(unit, TCAM_BLOCK(unit, core), tcam_bank_address, mem_val_read));

    *key_valid_bits = soc_mem_field32_get(unit, TCAM_TCAM_BANK_REPLYm, mem_val_read, TCAM_CPU_REP_VALIDf);
    soc_mem_field_get(unit, TCAM_TCAM_BANK_REPLYm, mem_val_read, TCAM_CPU_REP_DOUTf, data_key);

    /*
     * Read key mask from position + 1
     */
    SHR_IF_ERR_EXIT(WRITE_TCAM_TCAM_BANK_COMMANDm(unit, TCAM_BLOCK(unit, core), tcam_bank_address + 1, mem_val_write));

    SOC_REG_ABOVE_64_CLEAR(mem_val_read);
    SHR_IF_ERR_EXIT(READ_TCAM_TCAM_BANK_REPLYm(unit, TCAM_BLOCK(unit, core), tcam_bank_address + 1, mem_val_read));

    soc_mem_field_get(unit, TCAM_TCAM_BANK_REPLYm, mem_val_read, TCAM_CPU_REP_DOUTf, data_mask);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_tcam_access_hw_v1_entry_key_write(
    int unit,
    int core,
    uint32 bank_id,
    uint32 bank_offset,
    dnx_field_tcam_access_key_mode_e key_mode,
    dnx_field_tcam_access_part_e part,
    uint8 valid,
    uint32 *key,
    uint32 *key_mask)
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
    uint32 tcam_bank_address;

    SHR_FUNC_INIT_VARS(unit);

    SOC_REG_ABOVE_64_CLEAR(mem_val);

    tcam_bank_address = DNX_FIELD_TCAM_LOCATION_BANK_TO_ABSOLUTE(bank_id, bank_offset);

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
    SHR_BITAND_RANGE(data_key, data_mask, 0, SINGLE_KEY_BIT_SIZE + 2 * KEY_MODE_BIT_SIZE, key);

    /** 2) Encode Mask for TCAM */
    SHR_BITNEGATE_RANGE(data_key, 0, SINGLE_KEY_BIT_SIZE + 2 * KEY_MODE_BIT_SIZE, neg_data_key);
    SHR_BITAND_RANGE(neg_data_key, data_mask, 0, SINGLE_KEY_BIT_SIZE + 2 * KEY_MODE_BIT_SIZE, key_mask);

    /** Copy data_mask||data_key to data */
    SHR_BITCOPY_RANGE(data, 0, key, 0, SINGLE_KEY_BIT_SIZE + 2 * KEY_MODE_BIT_SIZE);
    SHR_BITCOPY_RANGE(data, SINGLE_KEY_BIT_SIZE + 2 * KEY_MODE_BIT_SIZE,
                      key_mask, 0, SINGLE_KEY_BIT_SIZE + 2 * KEY_MODE_BIT_SIZE);

    soc_mem_field32_set(unit, TCAM_TCAM_BANK_COMMANDm, mem_val, TCAM_CPU_CMD_WRf, 0x3);
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

    SHR_IF_ERR_EXIT(WRITE_TCAM_TCAM_BANK_COMMANDm(unit, TCAM_BLOCK(unit, core), tcam_bank_address, mem_val));

exit:
    SHR_FUNC_EXIT;
}
