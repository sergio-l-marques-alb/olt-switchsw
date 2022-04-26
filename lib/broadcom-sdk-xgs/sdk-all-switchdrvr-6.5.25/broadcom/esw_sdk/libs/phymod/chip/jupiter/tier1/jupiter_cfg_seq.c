/*----------------------------------------------------------------------
 * $Id: jupiter_cfg_seq.c portmod team Exp $
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * $Copyright: (c) 2021 Broadcom Corporation All Rights Reserved.$
 * Broadcom Corporation
 * Proprietary and Confidential information
 * All rights reserved
 * This source file is the property of Broadcom Corporation, and
 * may not be copied or distributed in any isomorphic form without the
 * prior written consent of Broadcom Corporation.
 *---------------------------------------------------------------------
 * File       : jupiter_cfg_seq.c
 * Description: c functions implementing Tier1s for Jupiter Serdes Driver
 *---------------------------------------------------------------------*/

#include <phymod/phymod.h>
#include <phymod/phymod_system.h>
#include <phymod/phymod_util.h>
#include <phymod/phymod_definitions.h>
#include <phymod/chip/bcmi_tscj_xgxs_defs.h>

#include "jupiter_cfg_seq.h"
#include "jupiter_ucode.h"
#include "jupiter_dependencies.h"
#include "jupiter_core.h"

#define JUPITER_NOF_LANES_IN_CORE 8

/* API to switch the PMD register access clock between LSREF_CLK and HeartBeat CLK */
int jupiter_pmd_access_clk_switch (phymod_access_t *sa__, uint32_t switch_to_refclk)
{
    SWITCHCLK_DBE_CMNr_t SWITCHCLK_DBE_CMNr_reg;

    SWITCHCLK_DBE_CMNr_CLR(SWITCHCLK_DBE_CMNr_reg);
    PHYMOD_IF_ERR_RETURN
        (READ_SWITCHCLK_DBE_CMNr(sa__, &SWITCHCLK_DBE_CMNr_reg));
    SWITCHCLK_DBE_CMNr_LSREF_DIV_SEL_NTf_SET(SWITCHCLK_DBE_CMNr_reg, switch_to_refclk);
    PHYMOD_IF_ERR_RETURN
        (WRITE_SWITCHCLK_DBE_CMNr(sa__, SWITCHCLK_DBE_CMNr_reg));

    return PHYMOD_E_NONE;
}

/* API to setup the init time link training config in PMD */
int jupiter_pmd_link_training_setup (phymod_access_t *sa__)
{
    int i;
    jupiter_lt_cfg_t lt_cfg[JUPITER_NOF_LANES_IN_CORE] = {
        {0, 0, 0x57E, 0x00AB, 312500},
        {1, 1, 0x645, 0x0741, 312500},
        {2, 2, 0x72D, 0x122C, 312500},
        {3, 3, 0x7B6, 0x0882, 312500},
        {0, 0, 0x57E, 0x00AB, 312500},
        {1, 1, 0x645, 0x0741, 312500},
        {2, 2, 0x72D, 0x122C, 312500},
        {3, 3, 0x7B6, 0x0882, 312500},
    };

    for (i = 0; i < JUPITER_NOF_LANES_IN_CORE; i++) {
        sa__->lane_mask = 1 << i;

        PHYMOD_IF_ERR_RETURN
            (jupiter_pmd_link_training_config_set(sa__,
                                                  lt_cfg[i]));
    }

    return PHYMOD_E_NONE;
}

/* API to read firmware (CSR & SRAM) content and compare with the ucode array */
int jupiter_ucode_load_verify (phymod_access_t *sa__)
{
    uint32_t i;
    uint32_t (*csr_ucode)[JUPITER_UCODE_CSR_SIZE][2];
    uint32_t (*sram_ucode)[JUPITER_UCODE_SRAM_SIZE][2];
    uint32_t addr, val, exp_val;

    csr_ucode = (uint32_t (*)[JUPITER_UCODE_CSR_SIZE][2])jupiter_csr_ucode_get();
    for (i = 0; i < JUPITER_UCODE_CSR_SIZE; i++) {
        addr = *(*(*(csr_ucode) + i) + 0);
        exp_val = *(*(*(csr_ucode) + i) + 1);
        if (addr != 0) {
            CHECK(pmd_read_addr(sa__, addr, &val));
        } else {
            break;
        }
        if (exp_val != val) {
            PHYMOD_DEBUG_ERROR(("Addr: 0x%x Expected val: 0x%x, Read val: 0x%x\n",
                        addr, exp_val, val));
            return 1;
        }
    }

    sram_ucode = (uint32_t (*)[JUPITER_UCODE_SRAM_SIZE][2])jupiter_sram_ucode_get();
    for (i = 0; i < JUPITER_UCODE_SRAM_SIZE; i++) {
        addr = *(*(*(sram_ucode) + i) + 0);
        exp_val = *(*(*(sram_ucode) + i) + 1);
        if (addr != 0) {
            CHECK(pmd_read_addr(sa__, addr, &val));
        } else {
            break;
        }
        if (exp_val != val) {
            PHYMOD_DEBUG_ERROR(("Addr: 0x%x Expected val: 0x%x, Read val: 0x%x\n",
                        addr, exp_val, val));
            return 1;
        }
    }

    return 0;
}

/*
 * API to load firmware via PMI interface. 'csr_only' flag specifies whether
 * to load only
 * the CSR content of firmware (true) or both CSR and SRAM content (false)
 */
int jupiter_ucode_pmi_load (phymod_access_t *sa__, uint32_t csr_only)
{
    uint32_t i;
    uint32_t (*csr_ucode)[JUPITER_UCODE_CSR_SIZE][2];
    uint32_t (*sram_ucode)[JUPITER_UCODE_SRAM_SIZE][2];
    uint32_t addr, val;

    csr_ucode = (uint32_t (*)[JUPITER_UCODE_CSR_SIZE][2])jupiter_csr_ucode_get();
    for (i = 0; i < JUPITER_UCODE_CSR_SIZE; i++) {
        addr = *(*(*(csr_ucode) + i) + 0);
        val = *(*(*(csr_ucode) + i) + 1);
        if (addr != 0) {
            CHECK(pmd_write_addr(sa__, addr, val));
        }
    }

    if (csr_only) {
        return 0;
    }

    sram_ucode = (uint32_t (*)[JUPITER_UCODE_SRAM_SIZE][2])jupiter_sram_ucode_get();
    for (i = 0; i < JUPITER_UCODE_SRAM_SIZE; i++) {
        addr = *(*(*(sram_ucode) + i) + 0);
        val = *(*(*(sram_ucode) + i) + 1);
        if (addr != 0) {
            CHECK(pmd_write_addr(sa__, addr, val));
        }
    }

    return 0;
}
