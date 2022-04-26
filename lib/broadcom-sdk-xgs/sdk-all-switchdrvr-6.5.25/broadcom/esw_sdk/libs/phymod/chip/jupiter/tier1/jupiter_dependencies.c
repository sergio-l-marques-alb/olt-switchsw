
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *  $Id$
 */

#include <phymod/phymod.h>
#include <phymod/acc/phymod_tsc_iblk.h>
#include "jupiter_dependencies.h"
#include "jupiter_core.h"

#define LANE0_OFFSET 0x02000000 // this can be derived from jupiter_core_csr_defines.h
#define LANE_BROADCAST 0x40000000
#define SRAM_BANK_MASK 0xc0000000
#define SRAM_BANK_0 0x80000000
#define SRAM_BANK_1 0xc0000000

int pmd_set_lane(mss_access_t *sa__, uint32_t lane)
{
    // if broadcast bit is set, lane bits are dont care
    if (lane == 99) {
        sa__->lane_mask = 0xFF;
    } else if (lane > JUPITER_LANE_MAX) {
        USR_PRINTF(("[pmd_set_lane]: Lane number %d does not exist in the design\n", lane));
        return 1;
    } else {
        sa__->lane_mask = (1 << lane);
    }

    return 0;
}

int pmd_write_field(mss_access_t *sa__, uint32_t addr, uint32_t fld_mask, uint32_t fld_offset, uint32_t wval)
{
    uint32_t i;
    uint32_t final_addr;
    uint32_t error_code = 0;
    uint32_t data;

    if (addr < LANE0_OFFSET) {
        data = (wval << fld_offset) & 0xFFFF;
        data |= (fld_mask & 0xFFFF) << 16;
        if (data) {
            if ((data >> 16) == 0xFFFF) {
                /* Mask is 0xFFFF. SBUS needs it to be changed to 0x0000.
                 * hence clear the mask
                 */
                data &= 0xFFFF;
            }
            error_code = phymod_tscj_iblk_write(sa__, (0x10000 | addr), data);
        }

        data = ((wval << fld_offset) >> 16) & 0xFFFF;
        data |= ((fld_mask >> 16) & 0xFFFF) << 16;
        if (data) {
            if ((data >> 16) == 0xFFFF) {
                /* Mask is 0xFFFF. SBUS needs it to be changed to 0x0000.
                 * hence clear the mask
                 */
                data &= 0xFFFF;
            }
            error_code += phymod_tscj_iblk_write(sa__, (0x10000 | (addr+2)), data);
        }

    } else if (sa__->lane_mask == 0xFF) {
        final_addr = addr + LANE_BROADCAST - LANE0_OFFSET;

        data = (wval << fld_offset) & 0xFFFF;
        data |= (fld_mask & 0xFFFF) << 16;
        if (data) {
            if ((data >> 16) == 0xFFFF) {
                /* Mask is 0xFFFF. SBUS needs it to be changed to 0x0000.
                 * hence clear the mask
                 */
                data &= 0xFFFF;
            }
            error_code = phymod_tscj_iblk_write(sa__, (0x10000 | final_addr), data);
        }


        data = ((wval << fld_offset) >> 16) & 0xFFFF;
        data |= ((fld_mask >> 16) & 0xFFFF) << 16;
        if (data) {
            if ((data >> 16) == 0xFFFF) {
                /* Mask is 0xFFFF. SBUS needs it to be changed to 0x0000.
                 * hence clear the mask
                 */
                data &= 0xFFFF;
            }
            error_code += phymod_tscj_iblk_write(sa__, (0x10000 | (final_addr+2)), data);
        }

    } else {
        for (i = 0; i <= JUPITER_LANE_MAX; i++) {
            if ((1 << i) & sa__->lane_mask) {
                final_addr = addr + (i * LANE0_OFFSET);

                data = (wval << fld_offset) & 0xFFFF;
                data |= (fld_mask & 0xFFFF) << 16;
                if (data) {
                    if ((data >> 16) == 0xFFFF) {
                        /* Mask is 0xFFFF. SBUS needs it to be changed to 0x0000.
                         * hence clear the mask
                         */
                        data &= 0xFFFF;
                    }
                    error_code = phymod_tscj_iblk_write(sa__, (0x10000 | final_addr), data);
                }

                data = ((wval << fld_offset) >> 16) & 0xFFFF;
                data |= ((fld_mask >> 16) & 0xFFFF) << 16;
                if (data) {
                    if ((data >> 16) == 0xFFFF) {
                        /* Mask is 0xFFFF. SBUS needs it to be changed to 0x0000.
                         * hence clear the mask
                         */
                        data &= 0xFFFF;
                    }
                    error_code += phymod_tscj_iblk_write(sa__, (0x10000 | (final_addr+2)), data);
                }
            }
        }
    }

    return error_code;
}

int pmd_read_field(mss_access_t *sa__, uint32_t addr, uint32_t fld_mask, uint32_t fld_offset, uint32_t *rdval)
{
    uint32_t i;
    uint32_t final_addr = addr;
    uint32_t rddata;
    uint32_t error_code = 0;
    uint32_t data;

    if (addr < LANE0_OFFSET) {
        final_addr = addr;
    } else {
        for (i = 0; i <= JUPITER_LANE_MAX; i++) {
            if ((1 << i) & (sa__->lane_mask)) {
                final_addr = addr + (i * LANE0_OFFSET);
                break;
            }
        }
    }

    if (final_addr >= LANE_BROADCAST) {
        USR_PRINTF(("[pmd_read_field]: Cannot read register while .sa__->lane_offset has lane broadcast set.\n"));
        return 1;
    }

    error_code = phymod_tscj_iblk_read(sa__, (0x10000 | final_addr),  &data);
    rddata = data & 0xFFFF;

    error_code += phymod_tscj_iblk_read(sa__, (0x10000 | (final_addr+2)),  &data);
    rddata |= ((data & 0xFFFF) << 16);

    *rdval = (rddata & fld_mask) >> fld_offset;
    return error_code;
}

int pmd_poll_field(mss_access_t *sa__, uint32_t addr, uint32_t fld_mask, uint32_t fld_offset, uint32_t poll_val, uint32_t timeout_us)
{
    uint32_t i = 0;
    uint32_t rd_val = 0;
    uint32_t error_code = 0;
    while (i < timeout_us) {
        i++;
        USR_USLEEP(1);
        error_code = pmd_read_field(sa__, addr, fld_mask, fld_offset, &rd_val);
        if (error_code || (rd_val == poll_val)) {
            break;
        }
    }

    if (!error_code && (rd_val == poll_val)) {
        return 0;
    } else {
        return 1;
    }
}

int pmd_read_check_field(mss_access_t *sa__, uint32_t addr, uint32_t fld_mask, uint32_t fld_offset, pmd_rd_opcode_t rd_opcode, uint32_t *rdval, uint32_t rdcheck_val1, uint32_t rdcheck_val2)
{
    uint32_t i;
    uint32_t final_addr = addr;
    uint32_t data, rddata;
    uint32_t error_code = 0;

    if (addr < LANE0_OFFSET) {
        final_addr = addr;
    } else {
        for (i = 0; i <= JUPITER_LANE_MAX; i++) {
            if ((1 << i) & (sa__->lane_mask)) {
                final_addr = addr + (i * LANE0_OFFSET);
                break;
            }
        }
    }

    if (final_addr >= LANE_BROADCAST) {
        USR_PRINTF(("[pmd_read_check_field]: Cannot read register while sa__->lane_offset has lane broadcast set.\n"));
        return 1;
    }

    error_code = phymod_tscj_iblk_read(sa__, (0x10000 | final_addr),  &data);
    rddata = data & 0xFFFF;

    error_code += phymod_tscj_iblk_read(sa__, (0x10000 | (final_addr+2)),  &data);
    rddata |= ((data & 0xFFFF) << 16);

    *rdval = (rddata & fld_mask) >> fld_offset;

    if (rd_opcode == RD_EQ) {
        if (*rdval == rdcheck_val1) {
            USR_PRINTF(("[pmd_read_check_field]: Register check passed. Expected value = 0x%X. Field value = 0x%X\n", rdcheck_val1, *rdval));
            return 0;
        } else {
            USR_PRINTF(("[pmd_read_check_field]: ERROR. Register check failed. Expected value = 0x%X. Field value = 0x%X\n", rdcheck_val1, *rdval));
            return -1;
        }
    } else if (rd_opcode == RD_GT) {
        if (*rdval > rdcheck_val1) {
            USR_PRINTF(("[pmd_read_check_field]: Register check passed. Expected value > 0x%X. Field value = 0x%X\n", rdcheck_val1, *rdval));
            return 0;
        } else {
            USR_PRINTF(("[pmd_read_check_field]: ERROR. Register check failed. Expected > 0x%X. Field value = 0x%X\n", rdcheck_val1, *rdval));
            return -1;
        }
    } else if (rd_opcode == RD_GTE) {
        if (*rdval >= rdcheck_val1) {
            USR_PRINTF(("[pmd_read_check_field]: Register check passed. Expected value >= 0x%X. Field value = 0x%X\n", rdcheck_val1, *rdval));
            return 0;
        } else {
            USR_PRINTF(("[pmd_read_check_field]: ERROR. Register check failed. Expected value >= 0x%X. Field value = 0x%X\n", rdcheck_val1, *rdval));
            return -1;
        }
    } else if (rd_opcode == RD_LT) {
        if (*rdval < rdcheck_val1) {
            USR_PRINTF(("[pmd_read_check_field]: Register check passed. Expected value < 0x%X. Field value = 0x%X\n", rdcheck_val1, *rdval));
            return 0;
        } else {
            USR_PRINTF(("[pmd_read_check_field]: ERROR. Register check failed. Expected < 0x%X. Field value = 0x%X\n", rdcheck_val1, *rdval));
            return -1;
        }
    } else if (rd_opcode == RD_LTE) {
        if (*rdval <= rdcheck_val1) {
            USR_PRINTF(("[pmd_read_check_field]: Register check passed. Expected value <= 0x%X. Field value = 0x%X\n", rdcheck_val1, *rdval));
            return 0;
        } else {
            USR_PRINTF(("[pmd_read_check_field]: ERROR. Register check failed. Expected <= 0x%X. Field value = 0x%X\n", rdcheck_val1, *rdval));
            return -1;
        }
    } else if (rd_opcode == RD_RANGE) {
        if (*rdval >= rdcheck_val1 && *rdval <= rdcheck_val2) {
            USR_PRINTF(("[pmd_read_check_field]: Register check passed. Expected range = 0x%X -> 0x%X. Field value = 0x%X\n", rdcheck_val1, rdcheck_val2, *rdval));
            return 0;
        } else {
            USR_PRINTF(("[pmd_read_check_field]: ERROR. Register check failed. Expected range = 0x%X -> 0x%X. Field value = 0x%X\n", rdcheck_val1, rdcheck_val2, *rdval));
            return -1;
        }
    } else {
        USR_PRINTF(("[pmd_read_check_field]: ERROR. Invalid read opcode"));
    }

    return error_code;
}


int pmd_pram_write_sram(mss_access_t *sa__, uint32_t **ucode_arr, uint32_t ucode_len)
{
    return 0;
}

/*
 * pmd_write_addr writes 'val' to the address specified in 'addr'.
 * 'addr' can be a SRAM address or a register address.
 * Note: This API is used for slow download of firmware (CSR+SRAM)
 *       via PMI interface.
 */
int pmd_write_addr(const mss_access_t *sa__, uint32_t addr, uint32_t val)
{
    uint32_t i;
    uint32_t final_addr;
    uint32_t error_code = 0;
    uint32_t data;
    uint32_t mask = 0; /* To write all 16 bits in data field */

    if ((addr & SRAM_BANK_MASK) == SRAM_BANK_1) {
        /* SRAM Bank 1 is not applicable for Jupiter PMD */
        return 1;
    } else if (addr < LANE0_OFFSET ||
               ((addr & SRAM_BANK_MASK) == SRAM_BANK_0)) {
        data = val & 0xFFFF;
        data |= mask << 16;
        error_code = phymod_tscj_iblk_write(sa__, (0x10000 | addr), data);

        data = (val >> 16) & 0xFFFF;
        data |= mask << 16;
        error_code += phymod_tscj_iblk_write(sa__, (0x10000 | (addr+2)), data);

    } else if (sa__->lane_mask == 0xFF || (addr & LANE_BROADCAST)) {
        if (!(addr & LANE_BROADCAST)) {
            final_addr = addr + LANE_BROADCAST - LANE0_OFFSET;
        } else {
            final_addr = addr;
        }

        data = val & 0xFFFF;
        data |= mask << 16;
        error_code = phymod_tscj_iblk_write(sa__, (0x10000 | final_addr), data);

        data = (val >> 16) & 0xFFFF;
        data |= mask << 16;
        error_code += phymod_tscj_iblk_write(sa__, (0x10000 | (final_addr+2)), data);

    } else {
        for (i = 0; i <= JUPITER_LANE_MAX; i++) {
            if ((1 << i) & sa__->lane_mask) {
                final_addr = addr + (i * LANE0_OFFSET);

                data = val & 0xFFFF;
                data |= mask << 16;
                error_code = phymod_tscj_iblk_write(sa__, (0x10000 | final_addr), data);

                data = (val >> 16) & 0xFFFF;
                data |= mask << 16;
                error_code += phymod_tscj_iblk_write(sa__, (0x10000 | (final_addr+2)), data);
            }
        }
    }

    return error_code;
}

int pmd_read_addr(const mss_access_t *sa__, uint32_t addr, uint32_t *val)
{
    uint32_t i;
    uint32_t final_addr = addr;
    uint32_t rddata;
    uint32_t error_code = 0;
    uint32_t data;

    if ((addr & SRAM_BANK_MASK) == SRAM_BANK_1) {
        /* SRAM Bank 1 is not applicable for Jupiter PMD */
        return 1;
    } else if (addr < LANE0_OFFSET ||
               ((addr & SRAM_BANK_MASK) == SRAM_BANK_0)) {
        final_addr = addr;
    } else if (addr & LANE_BROADCAST) {
        /*
         * This is primarily for the case of ucode load verify.
         * Reading the regiser from one of the lanes (physical lane 0).
         */
        final_addr = (addr & (~LANE_BROADCAST)) + (1 * LANE0_OFFSET);
    } else {
        for (i = 0; i <= JUPITER_LANE_MAX; i++) {
            if ((1 << i) & (sa__->lane_mask)) {
                final_addr = addr + (i * LANE0_OFFSET);
                break;
            }
        }
    }

    if (final_addr & LANE_BROADCAST) {
        USR_PRINTF(("[pmd_read_addr]: Cannot read register while .sa__->lane_offset has lane broadcast set.\n"));
        return 1;
    }

    error_code = phymod_tscj_iblk_read(sa__, (0x10000 | final_addr),  &data);
    rddata = data & 0xFFFF;

    error_code += phymod_tscj_iblk_read(sa__, (0x10000 | (final_addr+2)),  &data);
    rddata |= ((data & 0xFFFF) << 16);

    *val = rddata;
    return error_code;
}

int pmd_mdio_write_addr(mss_access_t *sa__, uint32_t ucode_addr, uint32_t ucode_val)
{
    return 0;
}
