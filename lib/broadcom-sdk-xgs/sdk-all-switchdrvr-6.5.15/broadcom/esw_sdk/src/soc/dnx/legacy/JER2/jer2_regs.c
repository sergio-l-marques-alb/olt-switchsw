/** \file jer2_regs.c
 *
 * Functions for handling registers.
 *
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifdef BSL_LOG_MODULE
  #error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_PORT

/*************
 * INCLUDES  *
 *************/
/* { */
#include <shared/shrextend/shrextend_debug.h>

#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>
#include <soc/dnx/legacy/drv.h>
#include <soc/dnx/legacy/mbcm.h>
#include <soc/dnx/legacy/JER2/jer2_regs.h>

/* } */

/********************
 * STATIC FUNCTIONS *
 ********************/
/* { */

/*
 * \brief
 *   Get whether FSRD block is enabled.
 */
static shr_error_e
soc_jer2_fsrd_block_enable_get(int unit, int fsrd_block, int *enable)
{
    int blk = -1;
    soc_info_t *si = NULL;
    SHR_FUNC_INIT_VARS(unit);

    si = &SOC_INFO(unit);

    blk = si->fsrd_block[fsrd_block];
    *enable = (si->block_valid[blk] == 1) ? 1 : 0;

    SHR_FUNC_EXIT;
}

/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */

shr_error_e
soc_jer2_fmac_block_enable_get(int unit, int fmac_block, int *enable)
{
    int blk = -1;
    soc_info_t *si = NULL;
    SHR_FUNC_INIT_VARS(unit);

    si = &SOC_INFO(unit);

    blk = si->fmac_block[fmac_block];
    *enable = (si->block_valid[blk] == 1) ? 1 : 0;

    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
soc_jer2_fsrd_block_enable_set(int unit, int fsrd_block, int enable)
{
    uint32 field_val;
    uint64 reg_val64;
    int fmac_block = 0;
    int blk = 0;
    int fmac_index = 0;
    soc_info_t *si = NULL;
    SHR_FUNC_INIT_VARS(unit);

    si = &SOC_INFO(unit);

    SHR_IF_ERR_EXIT(READ_ECI_BLOCKS_POWER_DOWNr(unit, &reg_val64));

    field_val = enable ? 0: 1;

    switch (fsrd_block)
    {
        case 0:
            soc_reg64_field32_set(unit, ECI_BLOCKS_POWER_DOWNr, &reg_val64, BLOCKS_POWER_DOWN_0f, field_val);
            break;
        case 1:
            soc_reg64_field32_set(unit, ECI_BLOCKS_POWER_DOWNr, &reg_val64, BLOCKS_POWER_DOWN_1f, field_val);
            break;
        case 2:
            soc_reg64_field32_set(unit, ECI_BLOCKS_POWER_DOWNr, &reg_val64, BLOCKS_POWER_DOWN_2f, field_val);
            break;
        case 3:
            soc_reg64_field32_set(unit, ECI_BLOCKS_POWER_DOWNr, &reg_val64, BLOCKS_POWER_DOWN_3f, field_val);
            break;
        case 4:
            soc_reg64_field32_set(unit, ECI_BLOCKS_POWER_DOWNr, &reg_val64, BLOCKS_POWER_DOWN_4f, field_val);
            break;
        case 5:
            soc_reg64_field32_set(unit, ECI_BLOCKS_POWER_DOWNr, &reg_val64, BLOCKS_POWER_DOWN_5f, field_val);
            break;
        case 6:
            soc_reg64_field32_set(unit, ECI_BLOCKS_POWER_DOWNr, &reg_val64, BLOCKS_POWER_DOWN_6f, field_val);
            break;
        case 7:
            soc_reg64_field32_set(unit, ECI_BLOCKS_POWER_DOWNr, &reg_val64, BLOCKS_POWER_DOWN_7f, field_val);
            break;
        case 8:
            soc_reg64_field32_set(unit, ECI_BLOCKS_POWER_DOWNr, &reg_val64, BLOCKS_POWER_DOWN_8f, field_val);
            break;
        case 9:
            soc_reg64_field32_set(unit, ECI_BLOCKS_POWER_DOWNr, &reg_val64, BLOCKS_POWER_DOWN_9f, field_val);
            break;
        case 10:
            soc_reg64_field32_set(unit, ECI_BLOCKS_POWER_DOWNr, &reg_val64, BLOCKS_POWER_DOWN_10f, field_val);
            break;
        case 11:
            soc_reg64_field32_set(unit, ECI_BLOCKS_POWER_DOWNr, &reg_val64, BLOCKS_POWER_DOWN_11f, field_val);
            break;
        case 12:
            soc_reg64_field32_set(unit, ECI_BLOCKS_POWER_DOWNr, &reg_val64, BLOCKS_POWER_DOWN_12f, field_val);
            break;
        case 13:
            soc_reg64_field32_set(unit, ECI_BLOCKS_POWER_DOWNr, &reg_val64, BLOCKS_POWER_DOWN_13f, field_val);
            break;
        default:
            SHR_ERR_EXIT(SOC_E_INTERNAL, "invalid fsrd block");
            break;
    }

    SHR_IF_ERR_EXIT(WRITE_ECI_BLOCKS_POWER_DOWNr(unit, reg_val64));

    /** Mark FSRD block as valid/non-valid */
    blk = si->fsrd_block[fsrd_block];
    si->block_valid[blk] = enable ? 1 : 0;

    /** Mark FAMC blocks as valid/non-valid */
    fmac_block = fsrd_block * dnx_data_fabric.blocks.nof_fmacs_in_fsrd_get(unit);
    for (fmac_index = fmac_block; fmac_index < fmac_block + dnx_data_fabric.blocks.nof_fmacs_in_fsrd_get(unit); fmac_index++)
    {
        blk = si->fmac_block[fmac_index];
        si->block_valid[blk] = enable ? 1 : 0;

        if (si->block_valid[blk])
        {
            SHR_IF_ERR_EXIT(MBCM_DNX_DRIVER_CALL(unit, mbcm_dnx_fabric_mac_init, (unit, fmac_index)));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
soc_jer2_port_fsrd_block_enable_set(int unit, soc_port_t logical_port, int enable)
{
    int fsrd_block;
    SHR_FUNC_INIT_VARS(unit);
	
    fsrd_block = SOC_DNX_FABRIC_PORT_TO_LINK(unit, logical_port) / dnx_data_fabric.blocks.nof_links_in_fsrd_get(unit);

    SHR_IF_ERR_EXIT(soc_jer2_fsrd_block_enable_set(unit, fsrd_block, enable));

exit:
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
soc_jer2_fsrd_blocks_low_power_init(int unit)
{
    int nof_fsrd = 0;
    int nof_links_in_fsrd = 0;
    soc_port_t link = -1;
    soc_port_t logical_port = -1;
    SHR_FUNC_INIT_VARS(unit);

    nof_fsrd = dnx_data_fabric.blocks.nof_instances_fsrd_get(unit);
    nof_links_in_fsrd = dnx_data_fabric.blocks.nof_links_in_fsrd_get(unit);

    /** Power down all FSRD blocks */
    for (link = 0; link < nof_fsrd * nof_links_in_fsrd; link += nof_links_in_fsrd)
    {
        logical_port = SOC_DNX_FABRIC_LINK_TO_PORT(unit, link);
        SHR_IF_ERR_EXIT(soc_jer2_port_fsrd_block_enable_set(unit, logical_port, 0));
    }

exit:
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
soc_jer2_brdc_fsrd_blk_id_set(int unit)
{
    int index;
    int fsrd = 0;
    int fmac = 0;
    int fsrd_last_in_chain=-1;
    int fmac_last_in_chain=-1;
    int fsrd_sbus_chain[DNX_DATA_MAX_FABRIC_BLOCKS_NOF_INSTANCES_FSRD];
    int enable = 0;
    uint32 nof_fsrds = dnx_data_fabric.blocks.nof_instances_fsrd_get(unit);
    uint32 nof_fmacs = dnx_data_fabric.blocks.nof_instances_fmac_get(unit);
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(fsrd_sbus_chain, -1, sizeof(fsrd_sbus_chain));

    /*
     * Set FSRD broadcast id, and mark the last FSRD in the SBUS chain.
     * Note that the order of the FSRDs in the SBUS chain is not necessarily
     * by their numerical order.
     */
    for (fsrd = 0; fsrd < nof_fsrds; fsrd++)
    {
        SHR_IF_ERR_EXIT(soc_jer2_fsrd_block_enable_get(unit, fsrd, &enable));
        if (enable)
        {
             
            SHR_IF_ERR_EXIT(WRITE_FSRD_SBUS_BROADCAST_IDr(unit, fsrd, SOC_BLOCK_INFO(unit,BRDC_FSRD_BLOCK(unit)).schan >> 4));

            SHR_IF_ERR_EXIT(WRITE_FSRD_SBUS_LAST_IN_CHAINr(unit, fsrd, 0));
            fsrd_sbus_chain[dnx_data_fabric.blocks.fsrd_sbus_chain_get(unit, fsrd)->index_in_chain] = fsrd;
        }
    }
    for (index = DNX_DATA_MAX_FABRIC_BLOCKS_NOF_INSTANCES_FSRD-1; index >= 0; --index)
    {
        if (fsrd_sbus_chain[index] != -1)
        {
            fsrd_last_in_chain = fsrd_sbus_chain[index];
            break;
        }
    }
    if (fsrd_last_in_chain != -1)
    {
        SHR_IF_ERR_EXIT(WRITE_FSRD_SBUS_LAST_IN_CHAINr(unit, fsrd_last_in_chain, 1));
    }

    /*
     * Set FMAC broadcast id, and mark the last FMAC in the SBUS chain.
     * Note that the order of the FMACs in the SBUS chain is not necessarily
     * by their numerical order.
     */
    for (fmac = 0; fmac < nof_fmacs; fmac++)
    {
        SHR_IF_ERR_EXIT(soc_jer2_fmac_block_enable_get(unit, fmac, &enable));
        if (enable)
        {
             
            SHR_IF_ERR_EXIT(WRITE_FMAC_SBUS_BROADCAST_IDr(unit, fmac, SOC_BLOCK_INFO(unit,BRDC_FMAC_BLOCK(unit)).schan >> 4));

            SHR_IF_ERR_EXIT(WRITE_FMAC_SBUS_LAST_IN_CHAINr(unit, fmac, 0));
            /** Last FMAC in chain must be one of the FMACs in the last FSRD in chain */
            if (fmac / dnx_data_fabric.blocks.nof_fmacs_in_fsrd_get(unit) == fsrd_last_in_chain)
            {
                fmac_last_in_chain = fmac;
            }
        }
    }
    if (fmac_last_in_chain != -1)
    {
        SHR_IF_ERR_EXIT(WRITE_FMAC_SBUS_LAST_IN_CHAINr(unit, fmac_last_in_chain, 1));
    }

exit:
    SHR_FUNC_EXIT;
}

/* } */

#undef BSL_LOG_MODULE

