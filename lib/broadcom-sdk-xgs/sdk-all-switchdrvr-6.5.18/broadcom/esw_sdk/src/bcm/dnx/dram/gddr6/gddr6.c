/** \file src/bcm/dnx/dram/gddr6/gddr6.c
 *
 *
 *  This file contains implementation of general functions for GDDR6 DRAM.
 *
 */

/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_DRAM

/*
 * Include files.
 * {
 */
/* SAL includes */
#include <sal/appl/sal.h>
#include <sal/types.h>

/* SOC includes */
#include <soc/debug.h>
#include <soc/error.h>
#include <shared/shrextend/shrextend_debug.h>
#include <sal/appl/sal.h>
#include <sal/core/boot.h>
#include <soc/mem.h>
#include <soc/memory.h>
#include <soc/feature.h>
#include <soc/types.h>
#include <soc/register.h>
#include <soc/shmoo_g6phy16.h>
#include <soc/sand/sand_aux_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dram.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_dram_access.h>
#include <soc/dnxc/dnxc_ha.h>
#include <soc/dnxc/swstate/dnxc_sw_state_verifications.h>
#include <soc/dnxc/swstate/dnxc_sw_state_utils.h>

#include <soc/dnxc/drv_dnxc_utils.h>
#include <bcm_int/dnx/dram/dram.h>
#include <bcm_int/dnx/dram/gddr6/gddr6.h>
#include <bcm_int/dnx/dram/gddr6/gddr6_cb.h>
#include <bcm_int/dnx/dram/gddr6/gddr6_func.h>
#include <bcm_int/dnx/dram/gddr6/gddr6_dbal_access.h>
#include <bcm_int/dnx/dram/dram.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_ingress_congestion_access.h>
#include <bcm_int/dnx/cosq/ingress/ingress_congestion.h>
#include <bcm_int/dnx/dram/buffers/buffers_quarantine_mechanism.h>
#include <bcm_int/dnx/dram/buffers/dram_buffers.h>

/*
 * }
 */

/*
 * defines
 * {
 */

#define GET_BIT_MASK(bits, offset)                  ((((1 << ((bits) + (offset))) - 1) >> (offset)) << (offset))
/** bit 0 is the module bit */
#define PHY_ADDRESS_MODULE_BITS                     (1)
#define PHY_ADDRESS_MODULE_OFFSET                   (0)
#define PHY_ADDRESS_MODULE_MASK                     (GET_BIT_MASK(PHY_ADDRESS_MODULE_BITS, PHY_ADDRESS_MODULE_OFFSET))
#define PHY_ADDRESS_GET_MODULE(address)             ((address & PHY_ADDRESS_MODULE_MASK) >> PHY_ADDRESS_MODULE_OFFSET)

/** bits 1-2 are the channel bits */
#define PHY_ADDRESS_CHANNEL_BITS                    (1)
#define PHY_ADDRESS_CHANNEL_OFFSET                  (PHY_ADDRESS_MODULE_OFFSET + PHY_ADDRESS_MODULE_BITS)
#define PHY_ADDRESS_CHANNEL_MASK                    (GET_BIT_MASK(PHY_ADDRESS_CHANNEL_BITS, PHY_ADDRESS_CHANNEL_OFFSET))
#define PHY_ADDRESS_GET_CHANNEL(address)            ((address & PHY_ADDRESS_CHANNEL_MASK) >> PHY_ADDRESS_CHANNEL_OFFSET)

/** bits 3-6 are bank bits */
#define PHY_ADDRESS_BANK_BITS                       (4)
#define PHY_ADDRESS_BANK_OFFSET                     (PHY_ADDRESS_CHANNEL_OFFSET + PHY_ADDRESS_CHANNEL_BITS)
#define PHY_ADDRESS_BANK_MASK                       (GET_BIT_MASK(PHY_ADDRESS_BANK_BITS, PHY_ADDRESS_BANK_OFFSET))
#define PHY_ADDRESS_GET_BANK(address)               ((address & PHY_ADDRESS_BANK_MASK) >> PHY_ADDRESS_BANK_OFFSET)

/** bits 7-22 are the row bits */
#define PHY_ADDRESS_ROW_BITS                        (16)
#define PHY_ADDRESS_ROW_OFFSET                      (PHY_ADDRESS_BANK_OFFSET + PHY_ADDRESS_BANK_BITS)
#define PHY_ADDRESS_ROW_MASK                        (GET_BIT_MASK(PHY_ADDRESS_ROW_BITS, PHY_ADDRESS_ROW_OFFSET))
#define PHY_ADDRESS_GET_ROW(address)                ((address & PHY_ADDRESS_ROW_MASK) >> PHY_ADDRESS_ROW_OFFSET)

/** bits 23-29 are the column bits, bit 24 is not used and will always be 0, in effect bits 25-29 determine the column */
#define PHY_ADDRESS_COLUMN_BITS                     (7)
#define PHY_ADDRESS_COLUMN_OFFSET                   (PHY_ADDRESS_ROW_OFFSET + PHY_ADDRESS_ROW_BITS)
#define PHY_ADDRESS_COLUMN_MASK                     (GET_BIT_MASK(PHY_ADDRESS_COLUMN_BITS, PHY_ADDRESS_COLUMN_OFFSET))
/** out of the 6b if the column, we need to ignore the LSB */
#define PHY_ADDRESS_GET_COLUMN(address)             (((address & PHY_ADDRESS_COLUMN_MASK) >> PHY_ADDRESS_COLUMN_OFFSET) >> 1)

/** transforming the logical address to physical address results in a jumbled physical address, this describes the scrambled bits */
#define PHY_ADDRESS_JUMBLED_RELEVANT_COLUMN_BITS    (2)
#define PHY_ADDRESS_JUMBLED_RELEVANT_COLUMN_OFFSET  (PHY_ADDRESS_ROW_OFFSET + PHY_ADDRESS_ROW_BITS)
#define PHY_ADDRESS_JUMBLED_RELEVANT_COLUMN_MASK    (GET_BIT_MASK(PHY_ADDRESS_JUMBLED_RELEVANT_COLUMN_BITS, \
                                                                  PHY_ADDRESS_JUMBLED_RELEVANT_COLUMN_OFFSET))

#define PHY_ADDRESS_CORRECT_RELEVANT_COLUMN_OFFSET  (28)
#define PHY_ADDRESS_SEQUENTIAL_COLUMN_OFFSET        (25)
/** nof bits that represent the amount of logical addresses or offset in a single buffer */
#define GDDR6_LOGICAL_ADDRESSES_BITS_PER_BUFFER      (4)
/** nof bits that represents the amount of read/write transactions of physical addresses required to read an
 * entire offset or an entire logical address */
#define GDDR6_TRANSACTION_BITS_IN_LOGICAL_ADDRESS    (3)
#define GDDR6_PHYSICAL_ADDRESS_TRANSACTION_SIZE_WORDS    (8)
#define BYTES_IN_WORD (4)

#define GDDR6_SHMOO_SIZE_OF_GET_VERAIBLE_ARRAY   SHMOO_G6PHY16_DQ_CONFIG_NOF_BYTE*SHMOO_G6PHY16_BYTE
/** MACRA DEFINITION */

static shr_error_e
dnx_gddr6_shmoo_entry_get_per_bit(
    int unit,
    int dram_index,
    char *name,
    uint32 value[SHMOO_G6PHY16_DQ_CONFIG_NOF_BYTE][SHMOO_G6PHY16_BYTE])
{
    char *name_str = NULL;
    char *val_str;
    int byte, bit;
    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO(name_str, dnx_drv_soc_property_name_max(), "dnx_gddr6_shmoo_entry_get_per_bit", "%s%s%s\r\n",
                       EMPTY, EMPTY, EMPTY);
    for (byte = 0; byte < SHMOO_G6PHY16_DQ_CONFIG_NOF_BYTE; byte++)
    {
        for (bit = 0; bit < SHMOO_G6PHY16_BYTE; bit++)
        {
            sal_snprintf(name_str, dnx_drv_soc_property_name_max(), "%s_dram%d_byte%d_bit%d", name, dram_index, byte,
                         bit);
            if ((val_str = dnx_drv_soc_property_get_str(unit, name_str)) != NULL)
            {
                value[byte][bit] = _shr_ctoi(val_str);
            }
            else
            {
                SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_NOT_FOUND,
                                         "Cannot find full tuning restoration data(%s) - Stopping tune parameters restore action%s%s\n",
                                         name_str, EMPTY, EMPTY);
            }
        }
    }
exit:
    SHR_FREE(name_str);
    SHR_FUNC_EXIT;;
}

static shr_error_e
dnx_gddr6_shmoo_entry_set_per_bit(
    int unit,
    int dram_index,
    char *name,
    uint32 value[SHMOO_G6PHY16_DQ_CONFIG_NOF_BYTE][SHMOO_G6PHY16_BYTE])
{
    int byte, bit;
    char *name_str = NULL;
    char val_str[GDDR6_STR_VAL_SIZE];
    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO(name_str, dnx_drv_soc_property_name_max(), "dnx_gddr6_shmoo_entry_set_per_bit", "%s%s%s\r\n",
                       EMPTY, EMPTY, EMPTY);
    for (byte = 0; byte < SHMOO_G6PHY16_DQ_CONFIG_NOF_BYTE; ++byte)
    {
        for (bit = 0; bit < SHMOO_G6PHY16_BYTE; bit++)
        {
            sal_snprintf(name_str, dnx_drv_soc_property_name_max(), "%s_dram%d_byte%d_bit%d.%d",
                         name, dram_index, byte, bit, unit);
            sal_snprintf(val_str, GDDR6_STR_VAL_SIZE, "0x%08X", value[byte][bit]);
            if (soc_mem_config_set(name_str, val_str) < 0)
            {
                SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_MEMORY, "Faile to save %s configuration%s%s\n", name_str, EMPTY, EMPTY);
            }
        }
    }
exit:
    SHR_FREE(name_str);
    SHR_FUNC_EXIT;;
}

static shr_error_e
dnx_gddr6_shmoo_entry_get_per_byte(
    int unit,
    int dram_index,
    char *name,
    uint32 value[SHMOO_G6PHY16_DQ_CONFIG_NOF_BYTE])
{
    char *name_str = NULL;
    char *val_str;
    int byte;
    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO(name_str, dnx_drv_soc_property_name_max(), "dnx_gddr6_shmoo_entry_get_per_byte", "%s%s%s\r\n",
                       EMPTY, EMPTY, EMPTY);

    for (byte = 0; byte < SHMOO_G6PHY16_DQ_CONFIG_NOF_BYTE; byte++)
    {
        sal_snprintf(name_str, dnx_drv_soc_property_name_max(), "%s_dram%d_byte%d", name, dram_index, byte);
        if ((val_str = dnx_drv_soc_property_get_str(unit, name_str)) != NULL)
        {
            value[byte] = _shr_ctoi(val_str);
        }
        else
        {
            SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_NOT_FOUND,
                                     "Cannot find full tuning restoration data(%s) - Stopping tune parameters restore action%s%s\n",
                                     name_str, EMPTY, EMPTY);
        }
    }
exit:
    SHR_FREE(name_str);
    SHR_FUNC_EXIT;;
}

static shr_error_e
dnx_gddr6_shmoo_entry_set_per_byte(
    int unit,
    int dram_index,
    char *name,
    uint32 value[SHMOO_G6PHY16_DQ_CONFIG_NOF_BYTE])
{
    int byte;
    char *name_str = NULL;
    char val_str[GDDR6_STR_VAL_SIZE];
    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO(name_str, dnx_drv_soc_property_name_max(), "dnx_gddr6_shmoo_entry_set_per_byte", "%s%s%s\r\n",
                       EMPTY, EMPTY, EMPTY);
    for (byte = 0; byte < SHMOO_G6PHY16_DQ_CONFIG_NOF_BYTE; ++byte)
    {
        sal_snprintf(name_str, dnx_drv_soc_property_name_max(), "%s_dram%d_byte%d.%d", name, dram_index, byte, unit);
        sal_snprintf(val_str, GDDR6_STR_VAL_SIZE, "0x%08X", value[byte]);
        if (soc_mem_config_set(name_str, val_str) < 0)
        {
            SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_MEMORY, "Faile to save %s configuration%s%s\n", name_str, EMPTY, EMPTY);
        }
    }
exit:
    SHR_FREE(name_str);
    SHR_FUNC_EXIT;;
}

static shr_error_e
dnx_gddr6_shmoo_entry_get(
    int unit,
    int dram_index,
    char *name,
    uint32 *value)
{
    char *name_str = NULL;
    char *val_str;
    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO(name_str, dnx_drv_soc_property_name_max(), "dnx_gddr6_shmoo_entry_get", "%s%s%s\r\n",
                       EMPTY, EMPTY, EMPTY);

    sal_snprintf(name_str, dnx_drv_soc_property_name_max(), "%s_dram%d", name, dram_index);
    if ((val_str = dnx_drv_soc_property_get_str(unit, name_str)) != NULL)
    {
        *value = _shr_ctoi(val_str);
    }
    else
    {
        SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_NOT_FOUND,
                                 "Cannot find full tuning restoration data(%s) - Stopping tune parameters restore action%s%s\n",
                                 name_str, EMPTY, EMPTY);
    }
exit:
    SHR_FREE(name_str);
    SHR_FUNC_EXIT;;
}

static shr_error_e
dnx_gddr6_shmoo_entry_set(
    int unit,
    int dram_index,
    char *name,
    uint32 value)
{
    char *name_str = NULL;
    char val_str[GDDR6_STR_VAL_SIZE];
    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO(name_str, dnx_drv_soc_property_name_max(), "dnx_gddr6_shmoo_entry_set", "%s%s%s\r\n",
                       EMPTY, EMPTY, EMPTY);

    sal_snprintf(name_str, dnx_drv_soc_property_name_max(), "%s_dram%d.%d", name, dram_index, unit);
    sal_snprintf(val_str, GDDR6_STR_VAL_SIZE, "0x%08X", value);
    if (soc_mem_config_set(name_str, val_str) < 0)
    {
        SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_MEMORY, "Faile to save %s configuration%s%s\n", name_str, EMPTY, EMPTY);
    }
exit:
    SHR_FREE(name_str);
    SHR_FUNC_EXIT;;
}

/*
 * Function:
 *       dnx_gddr6_shmoo_restore
 * Purpose:
 *      Get the Shmoo configuration from soc properties
 * Parameters:
 *      unit               - Device Number
 *      dram_index       - gddr6 dram index for which to get configuration from soc properties
 *      shmoo_config_param - where to store found parameters
 * Returns:
 *      _SHR_E_NONE      - on success
 *      _SHR_E_PARAM     - if a problem with recived arguments was detacted
 *      _SHR_E_NOT_FOUND - incase couldn't get all of the relevant information to restore from the SOC properties
 */
static shr_error_e
dnx_gddr6_shmoo_cfg_restore(
    int unit,
    int dram_index,
    g6phy16_shmoo_config_param_t * shmoo_config_param)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify shmoo_config_param not NULL */
    SHR_NULL_CHECK(shmoo_config_param, _SHR_E_PARAM, "shmoo_config_param");

    /** SHMOO_G6PHY16_ADDR_CTRL_SHORT */
    SHR_IF_ERR_EXIT(dnx_gddr6_shmoo_entry_get
                    (unit, dram_index, spn_G6PHY16_TUNE_AQ_L_MAX_VDL_ADDR, &shmoo_config_param->aq_l_max_vdl_addr));
    SHR_IF_ERR_EXIT(dnx_gddr6_shmoo_entry_get
                    (unit, dram_index, spn_G6PHY16_TUNE_AQ_L_MAX_VDL_CTRL, &shmoo_config_param->aq_l_max_vdl_ctrl));
    SHR_IF_ERR_EXIT(dnx_gddr6_shmoo_entry_get
                    (unit, dram_index, spn_G6PHY16_TUNE_AQ_U_MAX_VDL_ADDR, &shmoo_config_param->aq_u_max_vdl_addr));
    SHR_IF_ERR_EXIT(dnx_gddr6_shmoo_entry_get
                    (unit, dram_index, spn_G6PHY16_TUNE_AQ_U_MAX_VDL_CTRL, &shmoo_config_param->aq_u_max_vdl_ctrl));

    /** SHMOO_G6PHY16_RD_DESKEW */
    SHR_IF_ERR_EXIT(dnx_gddr6_shmoo_entry_get_per_bit
                    (unit, dram_index, spn_G6PHY16_TUNE_DQ_BYTE_RD_MIN_VDL_BIT,
                     shmoo_config_param->dq_byte_rd_min_vdl_bit));

    /** SHMOO_G6PHY16_DBI_EDC_RD_DESKEW */
    SHR_IF_ERR_EXIT(dnx_gddr6_shmoo_entry_get_per_byte
                    (unit, dram_index, spn_G6PHY16_TUNE_DQ_BYTE_RD_MIN_VDL_DBI,
                     shmoo_config_param->dq_byte_rd_min_vdl_dbi));
    SHR_IF_ERR_EXIT(dnx_gddr6_shmoo_entry_get_per_byte
                    (unit, dram_index, spn_G6PHY16_TUNE_DQ_BYTE_RD_MIN_VDL_EDC,
                     shmoo_config_param->dq_byte_rd_min_vdl_edc));

    /** SHMOO_G6PHY16_WR_DESKEW */
    SHR_IF_ERR_EXIT(dnx_gddr6_shmoo_entry_get_per_bit
                    (unit, dram_index, spn_G6PHY16_TUNE_DQ_BYTE_WR_MIN_VDL_BIT,
                     shmoo_config_param->dq_byte_wr_min_vdl_bit));

    /** SHMOO_G6PHY16_DBI_WR_DESKEW */
    SHR_IF_ERR_EXIT(dnx_gddr6_shmoo_entry_get_per_byte
                    (unit, dram_index, spn_G6PHY16_TUNE_DQ_BYTE_WR_MIN_VDL_DBI,
                     shmoo_config_param->dq_byte_wr_min_vdl_dbi));

    /** SHMOO_G6PHY16_RD_EXTENDED */
    SHR_IF_ERR_EXIT(dnx_gddr6_shmoo_entry_get_per_byte
                    (unit, dram_index, spn_G6PHY16_TUNE_DQ_BYTE_VREF_DAC_CONFIG,
                     shmoo_config_param->dq_byte_vref_dac_config));

    /** SHMOO_G6PHY16_WR_EXTENDED */
    SHR_IF_ERR_EXIT(dnx_gddr6_shmoo_entry_get_per_byte
                    (unit, dram_index, spn_G6PHY16_TUNE_DQ_BYTE_MACRO_RESERVED_REG,
                     shmoo_config_param->dq_byte_macro_reserved_reg));

exit:
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *       dnx_gddr6_shmoo_cfg_save
 * Purpose:
 *      Save the Shmoo configuration to soc properties
 * Parameters:
 *      unit               - Device Number
 *      dram_index            - gddr6 dram index for which to get configuration from soc properties
 *      shmoo_config_param - where to save found parameters
 * Returns:
 *      _SHR_E_NONE      - on success
 *      _SHR_E_PARAM     - if a problem with recived arguments was detacted
 *      _SHR_E_MEMORY     - if save soc properties failed
 */
static shr_error_e
dnx_gddr6_shmoo_cfg_save(
    int unit,
    int dram_index,
    g6phy16_shmoo_config_param_t * shmoo_config_param)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify shmoo_config_param not NULL */
    SHR_NULL_CHECK(shmoo_config_param, _SHR_E_PARAM, "shmoo_config_param");

    /** SHMOO_G6PHY16_ADDR_CTRL_SHORT */
    SHR_IF_ERR_EXIT(dnx_gddr6_shmoo_entry_set
                    (unit, dram_index, spn_G6PHY16_TUNE_AQ_L_MAX_VDL_ADDR, shmoo_config_param->aq_l_max_vdl_addr));
    SHR_IF_ERR_EXIT(dnx_gddr6_shmoo_entry_set
                    (unit, dram_index, spn_G6PHY16_TUNE_AQ_L_MAX_VDL_CTRL, shmoo_config_param->aq_l_max_vdl_ctrl));
    SHR_IF_ERR_EXIT(dnx_gddr6_shmoo_entry_set
                    (unit, dram_index, spn_G6PHY16_TUNE_AQ_U_MAX_VDL_ADDR, shmoo_config_param->aq_u_max_vdl_addr));
    SHR_IF_ERR_EXIT(dnx_gddr6_shmoo_entry_set
                    (unit, dram_index, spn_G6PHY16_TUNE_AQ_U_MAX_VDL_CTRL, shmoo_config_param->aq_u_max_vdl_ctrl));

    /** SHMOO_G6PHY16_RD_DESKEW */
    SHR_IF_ERR_EXIT(dnx_gddr6_shmoo_entry_set_per_bit
                    (unit, dram_index, spn_G6PHY16_TUNE_DQ_BYTE_RD_MIN_VDL_BIT,
                     shmoo_config_param->dq_byte_rd_min_vdl_bit));

    /** SHMOO_G6PHY16_DBI_EDC_RD_DESKEW */
    SHR_IF_ERR_EXIT(dnx_gddr6_shmoo_entry_set_per_byte
                    (unit, dram_index, spn_G6PHY16_TUNE_DQ_BYTE_RD_MIN_VDL_DBI,
                     shmoo_config_param->dq_byte_rd_min_vdl_dbi));
    SHR_IF_ERR_EXIT(dnx_gddr6_shmoo_entry_set_per_byte
                    (unit, dram_index, spn_G6PHY16_TUNE_DQ_BYTE_RD_MIN_VDL_EDC,
                     shmoo_config_param->dq_byte_rd_min_vdl_edc));

    /** SHMOO_G6PHY16_WR_DESKEW */
    SHR_IF_ERR_EXIT(dnx_gddr6_shmoo_entry_set_per_bit
                    (unit, dram_index, spn_G6PHY16_TUNE_DQ_BYTE_WR_MIN_VDL_BIT,
                     shmoo_config_param->dq_byte_wr_min_vdl_bit));

    /** SHMOO_G6PHY16_DBI_WR_DESKEW */
    SHR_IF_ERR_EXIT(dnx_gddr6_shmoo_entry_set_per_byte
                    (unit, dram_index, spn_G6PHY16_TUNE_DQ_BYTE_WR_MIN_VDL_DBI,
                     shmoo_config_param->dq_byte_wr_min_vdl_dbi));

    /** SHMOO_G6PHY16_RD_EXTENDED */
    SHR_IF_ERR_EXIT(dnx_gddr6_shmoo_entry_set_per_byte
                    (unit, dram_index, spn_G6PHY16_TUNE_DQ_BYTE_VREF_DAC_CONFIG,
                     shmoo_config_param->dq_byte_vref_dac_config));

    /** SHMOO_G6PHY16_WR_EXTENDED */
    SHR_IF_ERR_EXIT(dnx_gddr6_shmoo_entry_set_per_byte
                    (unit, dram_index, spn_G6PHY16_TUNE_DQ_BYTE_MACRO_RESERVED_REG,
                     shmoo_config_param->dq_byte_macro_reserved_reg));

exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_gddr6_phy_out_of_reset(
    int unit,
    int dram_index)
{
    SHR_FUNC_INIT_VARS(unit);

    /** phy out of reset */
    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "%s(): phy channel reset\n"), FUNCTION_NAME()));
    SHR_IF_ERR_EXIT(dnx_gddr6_dbal_access_phy_out_of_reset_config(unit, dram_index, 1));

    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "%s(): set iddq 0\n"), FUNCTION_NAME()));
    SHR_IF_ERR_EXIT(dnx_gddr6_dbal_access_phy_iddq_set(unit, dram_index, 0));
    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "%s(): phy write fifo enable\n"), FUNCTION_NAME()));
    SHR_IF_ERR_EXIT(dnx_gddr6_dbal_access_phy_write_fifo_enable_config(unit, dram_index, 1));

exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_gddr6_phy_pll_init(
    int unit,
    int dram_index)
{
    SHR_FUNC_INIT_VARS(unit);

    /** configure PLLs */
    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "%s(): configure PLLs\n"), FUNCTION_NAME()));
    SHR_IF_ERR_EXIT(soc_g6phy16_shmoo_phy_cfg_pll(unit, dram_index));
exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_gddr6_run_phy_tune(
    int unit,
    int dram_index,
    int shmoo_type,
    uint32 shmoo_flags,
    dnx_gddr6_phy_tune_action_e action)
{
    int dram_iter;
    uint32 nof_drams = dnx_data_dram.general_info.max_nof_drams_get(unit);
    uint32 dram_bitmap[1] = { 0 };
    int rv;
    g6phy16_shmoo_config_param_t config_params;
    SHR_FUNC_INIT_VARS(unit);

    if (action == DNX_GDDR6_SKIP_TUNE)
    {
        /** do nothing */
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "%s(): shmoo is skipped\n"), FUNCTION_NAME()));
        SHR_EXIT();
    }
    /*
     * ITER_ALL means running over all gddr6's and all channels. otherwise, run over one gddr6 dram and one specific channel
     * If tune on specific dram_index and channel, set the bitmap with the same bit that represent the dram_index
     * This way, the iterator will run only on the specific dram_index
     */
    if (dram_index != DNX_GDDR6_ITER_ALL)
    {
        SHR_IF_ERR_EXIT(dnx_gddr6_index_verify(unit, dram_index));
        SHR_BITSET(dram_bitmap, dram_index);
    }
    else
    {
        dram_bitmap[0] = dnx_data_dram.general_info.dram_info_get(unit)->dram_bitmap;
    }

    /** tune all phys or specific phy */
    SHR_BIT_ITER(dram_bitmap, nof_drams, dram_iter)
    {
        uint32 curr_refi;
        uint32 curr_refi_ab;
        int read_dbi_enabled = 0;
        int write_dbi_enabled = 0;
        int read_crc_enabled = 0;
        int write_crc_enabled = 0;
        uint32 dynamic_calibration_enabled[DNX_DATA_MAX_DRAM_GENERAL_INFO_NOF_CHANNELS] = { 0 };
        /** disable traffic to dram */
        SHR_IF_ERR_EXIT(dnx_gddr6_dbal_access_tsm_banks_disable_set(unit, dram_iter, 1));
        /** disable dynamic calibration */
        for (int channel = 0; channel < dnx_data_dram.general_info.nof_channels_get(unit); ++channel)
        {
            SHR_IF_ERR_EXIT(dnx_gddr6_dynamic_calibration_is_enabled
                            (unit, dram_iter, channel, &dynamic_calibration_enabled[channel]));
            SHR_IF_ERR_EXIT(dnx_gddr6_dynamic_calibration_disable(unit, dram_iter, channel));
        }
        /** disable dram refresh */
        SHR_IF_ERR_EXIT(dnx_gddr6_enable_refresh_cb(unit, dram_iter, 0, 0, 0, &curr_refi, &curr_refi_ab));
        /** get current CRC status */
        read_crc_enabled = dnx_data_dram.general_info.crc_read_get(unit);
        write_crc_enabled = dnx_data_dram.general_info.crc_write_get(unit);
        /** disable CRC */
        SHR_IF_ERR_EXIT(dnx_gddr6_enable_rd_crc_cb(unit, dram_iter, 0));
        SHR_IF_ERR_EXIT(dnx_gddr6_enable_wr_crc_cb(unit, dram_iter, 0));
        /** get current DBI status */
        read_dbi_enabled = dnx_data_dram.general_info.dbi_read_get(unit);
        write_dbi_enabled = dnx_data_dram.general_info.dbi_write_get(unit);
        /** disable DBI */
        SHR_IF_ERR_EXIT(dnx_gddr6_enable_rd_dbi_cb(unit, dram_iter, 0));
        SHR_IF_ERR_EXIT(dnx_gddr6_enable_wr_dbi_cb(unit, dram_iter, 0));
        switch (action)
        {
            case DNX_GDDR6_RESTORE_TUNE_PARAMETERS:
            {
                rv = dnx_gddr6_shmoo_cfg_restore(unit, dram_iter, &config_params);
                if (rv == _SHR_E_NOT_FOUND)
                {
                    SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_NOT_FOUND,
                                             "%s(): Configuration was not found, Shmoo dram ndx=%d, Failed!%s\n",
                                             FUNCTION_NAME(), dram_iter, EMPTY);
                }
                else
                {
                    SHR_IF_ERR_EXIT(soc_g6phy16_shmoo_ctl
                                    (unit, dram_iter, shmoo_type, shmoo_flags, SHMOO_G6PHY16_ACTION_RESTORE,
                                     &config_params));
                }
                break;
            }
            case DNX_GDDR6_RUN_TUNE:
            {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit, "%s(): Shmoo dram ndx=%d\n"), FUNCTION_NAME(), dram_iter));
                SHR_IF_ERR_EXIT(soc_g6phy16_shmoo_ctl
                                (unit, dram_iter, shmoo_type, shmoo_flags, SHMOO_G6PHY16_ACTION_RUN_AND_SAVE,
                                 &config_params));
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit, "%s(): Save Shmoo Configurations dram ndx=%d\n"), FUNCTION_NAME(),
                             dram_iter));
                if (shmoo_type == SHMOO_G6PHY16_SHMOO_ALL)
                {
                    SHR_IF_ERR_EXIT(dnx_gddr6_shmoo_cfg_save(unit, dram_iter, &config_params));
                }
                break;
            }
            case DNX_GDDR6_RESTORE_TUNE_PARAMETERS_OR_RUN_TUNE:
            {
                rv = dnx_gddr6_shmoo_cfg_restore(unit, dram_iter, &config_params);
                if (rv == _SHR_E_NOT_FOUND)
                {
                    LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "%s(): Shmoo dram ndx=%d\n"),
                                              FUNCTION_NAME(), dram_iter));
                    SHR_IF_ERR_EXIT(soc_g6phy16_shmoo_ctl
                                    (unit, dram_iter, shmoo_type, shmoo_flags, SHMOO_G6PHY16_ACTION_RUN_AND_SAVE,
                                     &config_params));
                    LOG_VERBOSE(BSL_LOG_MODULE,
                                (BSL_META_U(unit, "%s(): Save Shmoo Configurations dram ndx=%d\n"), FUNCTION_NAME(),
                                 dram_iter));
                    if (shmoo_type == SHMOO_G6PHY16_SHMOO_ALL)
                    {
                        SHR_IF_ERR_EXIT(dnx_gddr6_shmoo_cfg_save(unit, dram_iter, &config_params));
                    }
                }
                else
                {
                    SHR_IF_ERR_EXIT(soc_g6phy16_shmoo_ctl
                                    (unit, dram_iter, shmoo_type, shmoo_flags, SHMOO_G6PHY16_ACTION_RESTORE,
                                     &config_params));
                }
                break;
            }
            default:
            {
                LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "%s(): Wrong shmoo action = %d. dram ndx=%d\n"),
                                           FUNCTION_NAME(), action, dram_iter));
                SHR_IF_ERR_EXIT(_SHR_E_PARAM);
            }
        }

        /** restore CRC according to saved status */
        SHR_IF_ERR_EXIT(dnx_gddr6_enable_rd_crc_cb(unit, dram_iter, !!read_crc_enabled));
        SHR_IF_ERR_EXIT(dnx_gddr6_enable_wr_crc_cb(unit, dram_iter, !!write_crc_enabled));
        /** restore DBI according to saved status */
        SHR_IF_ERR_EXIT(dnx_gddr6_enable_rd_dbi_cb(unit, dram_iter, !!read_dbi_enabled));
        SHR_IF_ERR_EXIT(dnx_gddr6_enable_wr_dbi_cb(unit, dram_iter, !!write_dbi_enabled));
        /** restore dram refresh */
        SHR_IF_ERR_EXIT(dnx_gddr6_enable_refresh_cb
                        (unit, dram_iter, 1, curr_refi, curr_refi_ab, &curr_refi, &curr_refi_ab));
        /** restore dynamic calibration */
        for (int channel = 0; channel < dnx_data_dram.general_info.nof_channels_get(unit); ++channel)
        {
            if (dynamic_calibration_enabled[channel])
            {
                SHR_IF_ERR_EXIT(dnx_gddr6_dynamic_calibration_enable(unit, dram_iter, channel, 1));
            }
        }
        /** enable traffic to dram */
        SHR_IF_ERR_EXIT(dnx_gddr6_dbal_access_tsm_banks_disable_set(unit, dram_iter, 0));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get address translation matrix
 *
 * \param [in] unit - unit number
 * \param [out] atm - const ptr to address translation matrix
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
dnx_gddr6_tdu_atm_get(
    int unit,
    const uint32 **atm)
{
    uint32 max_nof_drams = dnx_data_dram.general_info.max_nof_drams_get(unit);
    uint32 dram_bitmap = dnx_data_dram.general_info.dram_info_get(unit)->dram_bitmap;
    int nof_drams;

    SHR_FUNC_INIT_VARS(unit);

    /** count nof drams, do it according to bitmap */
    shr_bitop_range_count(&dram_bitmap, 0, max_nof_drams, &nof_drams);

    /** get relevant address translation matrix for the number of DRAMs */
    *atm = &(dnx_data_dram.address_translation.matrix_configuration_get(unit, nof_drams - 1)->logical_to_physical[0]);

    SHR_FUNC_EXIT;
}

/**
 * \brief - configure address translation matrix
 *
 * \param [in] unit - unit number
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
dnx_gddr6_tdu_atm_configure(
    int unit)
{
    const uint32 *atm;
    SHR_FUNC_INIT_VARS(unit);

    /** get address translation matrix */
    SHR_IF_ERR_EXIT(dnx_gddr6_tdu_atm_get(unit, &atm));

    /** configure address translation matrix */
    SHR_IF_ERR_EXIT(dnx_gddr6_dbal_access_tdu_atm_configure(unit, atm));
exit:
    SHR_FUNC_EXIT;
}
/** see header file */
shr_error_e
dnx_gddr6_dcc_blocks_enable_set(
    int unit)
{
    uint32 dram_bitmap[1];
    int nof_drams;
    int nof_channels_in_dram;
    int dram_index, channel;
    SHR_FUNC_INIT_VARS(unit);

    /** get dram bitmap */
    /** this is done to prevent coverity issue of ARRAY_VS_SINGLETON from occuring */
    dram_bitmap[0] = dnx_data_dram.general_info.dram_info_get(unit)->dram_bitmap;
    nof_drams = dnx_data_dram.general_info.max_nof_drams_get(unit);
    /** get nof channels in dram */
    nof_channels_in_dram = dnx_data_dram.general_info.nof_channels_get(unit);
    /** iterate on dram bitmap and set enabled/disabled blocks */
    for (dram_index = 0; dram_index < nof_drams; ++dram_index)
    {
        int is_enabled;
        if (SHR_BITGET(dram_bitmap, dram_index))
        {
            /** enable channel blocks */
            is_enabled = 1;
        }
        else
        {
            /** disable channel blocks */
            is_enabled = 0;
        }
        for (channel = 0; channel < nof_channels_in_dram; ++channel)
        {
            int block;
            int *dcc_block_p;
            uint8 *block_valid_p;

            SHR_IF_ERR_EXIT(soc_info_int_address_get(unit, DCC_BLOCK_INT, &dcc_block_p));
            SHR_IF_ERR_EXIT(soc_info_uint8_address_get(unit, BLOCK_VALID, &block_valid_p));
            block = dcc_block_p[dnx_gddr6_get_sequential_channel(unit, dram_index, channel)];
            block_valid_p[block] = is_enabled;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_gddr6_bist_errors_check(
    int unit,
    dnx_gddr6_bist_status_cnt_t * bist_status_cnt,
    dnx_gddr6_bist_err_cnt_t * bist_err_cnt,
    int *bist_error_indicated)
{
    SHR_FUNC_INIT_VARS(unit);

    *bist_error_indicated = 0;

    if ((bist_err_cnt->bist_data_err_cnt) ||
        (bist_status_cnt->read_command_cnt[0] != bist_status_cnt->read_data_cnt[0]) ||
        (bist_status_cnt->read_command_cnt[1] != bist_status_cnt->read_data_cnt[1]))
    {
        *bist_error_indicated = 1;
    }

    SHR_FUNC_EXIT;
}

/**
 * \brief - handler for bist configuration.
 *  Run bist for all channels of active GDDR6's, only if Shmoo was made previously (or loaded)
 * \param [in] unit - unit number
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
dnx_gddr6_bist_handle(
    int unit)
{
    uint32 dram_bitmap[1];
    int dram_iter;
    int nof_drams;
    int nof_channels;
    dnx_gddr6_bist_configuration_t bist_config = {
        .nof_commands = COMPILER_64_INIT(0, 0x10000000),
        .write_weight = 255,
        .read_weight = 255,
        .same_row = 8,
        .bank_mode = DNX_GDDR6_BIST_ADDRESS_MODE_INCREMENTAL,
        .bank_start = 0,
        .bank_end = dnx_data_dram.general_info.dram_info_get(unit)->nof_banks - 1,
        .column_mode = DNX_GDDR6_BIST_ADDRESS_MODE_INCREMENTAL,
        .column_start = 0,
        .column_end = dnx_data_dram.general_info.dram_info_get(unit)->nof_columns - 1,
        .row_mode = DNX_GDDR6_BIST_ADDRESS_MODE_INCREMENTAL,
        .row_start = 0,
        .row_end = dnx_data_dram.general_info.dram_info_get(unit)->nof_rows - 1,
        .data_mode = DNX_GDDR6_BIST_DATA_MODE_PRBS,
        .data_seed = {0x12345678, 0x23456789, 0x3456789a, 0x456789ab, 0x56789abc, 0x6789abcd, 0x789abcde, 0x89abcdef}
    };

    SHR_FUNC_INIT_VARS(unit);

    dram_bitmap[0] = dnx_data_dram.general_info.dram_info_get(unit)->dram_bitmap;
    nof_drams = dnx_data_dram.general_info.max_nof_drams_get(unit);
    nof_channels = dnx_data_dram.general_info.nof_channels_get(unit);
    /** run DRAM BIST on all channels and all drams */
    SHR_BIT_ITER(dram_bitmap, nof_drams, dram_iter)
    {
        for (int channel_iter = 0; channel_iter < nof_channels; ++channel_iter)
        {
            SHR_IF_ERR_EXIT(dnx_gddr6_data_bist_configure(unit, dram_iter, channel_iter, &bist_config));
            SHR_IF_ERR_EXIT(dnx_gddr6_bist_start(unit, dram_iter, channel_iter));
        }
    }

    /** Check DRAM BIST results and check if an error occurred */
    SHR_BIT_ITER(dram_bitmap, nof_drams, dram_iter)
    {
        for (int channel_iter = 0; channel_iter < nof_channels; ++channel_iter)
        {
            int bist_error_indicated = 0;
            dnx_gddr6_bist_status_cnt_t bist_status_cnt;
            dnx_gddr6_bist_err_cnt_t bist_err_cnt = { 0 };
            sal_memset(&bist_status_cnt, 0, sizeof(bist_status_cnt));
            SHR_IF_ERR_EXIT(dnx_gddr6_bist_run_done_poll(unit, dram_iter, channel_iter));
            SHR_IF_ERR_EXIT(dnx_gddr6_bist_status_get(unit, dram_iter, channel_iter, &bist_status_cnt));
            SHR_IF_ERR_EXIT(dnx_gddr6_bist_err_cnt_get(unit, dram_iter, channel_iter, &bist_err_cnt));
            SHR_IF_ERR_EXIT(dnx_gddr6_bist_errors_check(unit, &bist_status_cnt, &bist_err_cnt, &bist_error_indicated));
            if (bist_error_indicated)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "channel %d in dram %d failed to run BIST\n", channel_iter, dram_iter);
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_gddr6_index_verify(
    int unit,
    int dram_index)
{
    uint32 dram_bitmap[1];
    SHR_FUNC_INIT_VARS(unit);

    dram_bitmap[0] = dnx_data_dram.general_info.dram_info_get(unit)->dram_bitmap;
    if (!SHR_IS_BITSET(dram_bitmap, dram_index))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "bad param dram_index=%d\n", dram_index);
    }

exit:
    SHR_FUNC_EXIT;
}

/*See header file*/
shr_error_e
dnx_gddr6_channels_soft_reset_set(
    int unit,
    int dram_index,
    int in_soft_reset)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_gddr6_index_verify(unit, dram_index));
    /** set channels soft reset */
    SHR_IF_ERR_EXIT(dnx_gddr6_dbal_access_channels_soft_reset_set(unit, dram_index, in_soft_reset));

exit:
    SHR_FUNC_EXIT;
}

/*See header file*/
shr_error_e
dnx_gddr6_channels_soft_init_set(
    int unit,
    int dram_index,
    int in_soft_init)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_gddr6_index_verify(unit, dram_index));
    /** set channels soft init */
    SHR_IF_ERR_EXIT(dnx_gddr6_dbal_access_channels_soft_init_set(unit, dram_index, in_soft_init));

exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_gddr6_sw_state_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_dram_db.override_bist_configurations_during_tuning.set(unit, 1));
    SHR_IF_ERR_EXIT(dnx_dram_db.dynamic_calibration_enabled.set(unit,
                                                                dnx_data_dram.gddr6.feature_get(unit,
                                                                                                dnx_data_dram_gddr6_dynamic_calibration)));

exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_gddr6_init(
    int unit)
{
    int dram_index;
    int nof_drams;
    uint32 dram_bitmap[1];
    int dynamic_calibration_enabled;
    SHR_FUNC_INIT_VARS(unit);

    /** disable irrelevant dcc blocks according to dram bitmap */
    SHR_IF_ERR_EXIT(dnx_gddr6_dcc_blocks_enable_set(unit));

    /** this is done to prevent coverity issue of ARRAY_VS_SINGLETON from occuring */
    dram_bitmap[0] = dnx_data_dram.general_info.dram_info_get(unit)->dram_bitmap;
    nof_drams = dnx_data_dram.general_info.max_nof_drams_get(unit);
    if (dram_bitmap[0] == 0)
    {
        SHR_EXIT();
    }

    /** Phy init */
    SHR_IF_ERR_EXIT(dnx_gddr6_phy_init(unit));

    SHR_BIT_ITER(dram_bitmap, nof_drams, dram_index)
    {
        /** gddr6 controller configuration, including board swap, ignore dram vendor info */
        SHR_IF_ERR_EXIT(dnx_gddr6_controller_init(unit, dram_index, TRUE));

        if (dnx_data_dram.general_info.tune_mode_on_init_get(unit) == DNX_GDDR6_RESTORE_TUNE_PARAMETERS)
        {
            /** if tune mode is restore, restore phy tune params for next dram init */
            g6phy16_shmoo_config_param_t shmoo_config_param;
            SHR_IF_ERR_EXIT(dnx_gddr6_shmoo_cfg_restore(unit, dram_index, &shmoo_config_param));
            SHR_IF_ERR_EXIT(shmoo_g6phy16_restore(unit, dram_index, &shmoo_config_param));
        }
        else if (dnx_data_dram.general_info.tune_mode_on_init_get(unit) ==
                 DNX_GDDR6_RESTORE_TUNE_PARAMETERS_OR_RUN_TUNE)
        {
            /** if tune mode is restore, restore phy tune params for next dram init */
            int rv;
            g6phy16_shmoo_config_param_t shmoo_config_param;
            rv = dnx_gddr6_shmoo_cfg_restore(unit, dram_index, &shmoo_config_param);
            if (rv == _SHR_E_NOT_FOUND)
            {
                SHR_IF_ERR_EXIT(dnx_gddr6_run_phy_tune
                                (unit, dram_index, SHMOO_G6PHY16_ADDR_CTRL_SHORT, 0, DNX_GDDR6_RUN_TUNE));
            }
            else
            {
                SHR_IF_ERR_EXIT(rv);
                SHR_IF_ERR_EXIT(shmoo_g6phy16_restore(unit, dram_index, &shmoo_config_param));
            }
        }
        else
        {
            /** run CA training */
            SHR_IF_ERR_EXIT(dnx_gddr6_run_phy_tune
                            (unit, dram_index, SHMOO_G6PHY16_ADDR_CTRL_SHORT, 0, DNX_GDDR6_RUN_TUNE));
        }

        /** gddr6 dram init, including MRs set according to JEDEC sequence */
        SHR_IF_ERR_EXIT(dnx_gddr6_dram_init(unit, dram_index, SHMOO_GDDR6_DRAM_INIT_ALL));

        /** gddr6  controller configuration, don't ignore dram vendor info */
        SHR_IF_ERR_EXIT(dnx_gddr6_controller_init(unit, dram_index, FALSE));
    }

    /** run phy tuning */
    if (!SAL_BOOT_PLISIM)
    {
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "%s(): run shmoo\n"), FUNCTION_NAME()));
        SHR_IF_ERR_EXIT(dnx_gddr6_run_phy_tune
                        (unit, DNX_GDDR6_ITER_ALL, SHMOO_G6PHY16_SHMOO_ALL, 0,
                         dnx_data_dram.general_info.tune_mode_on_init_get(unit)));
    }

    /** handler for gddr6 bist */
    if (dnx_data_dram.gddr6.bist_enable_get(unit)
        && (dnx_data_dram.general_info.tune_mode_on_init_get(unit) != DNX_GDDR6_SKIP_TUNE))
    {
        SHR_IF_ERR_EXIT(dnx_gddr6_bist_handle(unit));
    }

    /** check if dynamic calibration is enabled */
    SHR_IF_ERR_EXIT(dnx_dram_db.dynamic_calibration_enabled.get(unit, &dynamic_calibration_enabled));

    /** enable refresh and dynamic calibration */
    SHR_BIT_ITER(dram_bitmap, nof_drams, dram_index)
    {
        SHR_IF_ERR_EXIT(dnx_gddr6_dram_refresh_enable_bitmap_set(unit, dram_index, 0x3, 0x3));
        for (int channel = 0; channel < dnx_data_dram.general_info.nof_channels_get(unit); ++channel)
        {
            if (dynamic_calibration_enabled)
            {
                SHR_IF_ERR_EXIT(dnx_gddr6_dynamic_calibration_enable(unit, dram_index, channel, 1));
            }
        }
    }

    /** configure TDU, 8G supported */
    SHR_IF_ERR_EXIT(dnx_gddr6_tdu_configure(unit));

exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_gddr6_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_gddr6_phy_init(
    int unit)
{
    int nof_drams;
    uint32 dram_bitmap[1];
    int dram_index;
    SHR_FUNC_INIT_VARS(unit);

    /** no data into phy */
    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "%s(): no data into phy\n"), FUNCTION_NAME()));
    nof_drams = dnx_data_dram.general_info.max_nof_drams_get(unit);

    /** this is done to prevent coverity issue of ARRAY_VS_SINGLETON from occuring */
    dram_bitmap[0] = dnx_data_dram.general_info.dram_info_get(unit)->dram_bitmap;

    /** init phy through phy registers */
    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "%s(): init phy through phy registers\n"), FUNCTION_NAME()));
    SHR_BIT_ITER(dram_bitmap, nof_drams, dram_index)
    {
        SHR_IF_ERR_EXIT(soc_g6phy16_shmoo_phy_init(unit, dram_index));
    }

exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_gddr6_dram_refresh_enable_bitmap_get(
    int unit,
    int dram_index,
    uint32 *refresh_enable_bitmap,
    uint32 *refresh_ab_enable_bitmap)
{
    int nof_channels = dnx_data_dram.general_info.nof_channels_get(unit);
    uint32 refresh, refresh_ab;

    SHR_FUNC_INIT_VARS(unit);

    /** set bitmap to 0 */
    *refresh_enable_bitmap = 0;
    *refresh_ab_enable_bitmap = 0;

    for (int channel = 0; channel < nof_channels; ++channel)
    {
        /** set bitmap per channel */

        SHR_IF_ERR_EXIT(dnx_gddr6_dbal_access_enable_refresh_get(unit, dram_index, channel, &refresh, &refresh_ab));
        if (refresh)
        {
            *refresh_enable_bitmap |= (1 << channel);
        }
        if (refresh_ab)
        {
            *refresh_ab_enable_bitmap |= (1 << channel);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_gddr6_dram_refresh_enable_bitmap_set(
    int unit,
    int dram_index,
    uint32 refresh_enable_bitmap,
    uint32 refresh_ab_enable_bitmap)
{
    int channel;
    int nof_channels = dnx_data_dram.general_info.nof_channels_get(unit);
    uint32 channel_enable_refresh, channel_enable_refresh_ab;
    SHR_FUNC_INIT_VARS(unit);

    for (channel = 0; channel < nof_channels; ++channel)
    {
        /** set enable refresh per channel */
        channel_enable_refresh = !!(refresh_enable_bitmap & (1 << channel));
        channel_enable_refresh_ab = !!(refresh_ab_enable_bitmap & (1 << channel));

        SHR_IF_ERR_EXIT(dnx_gddr6_dbal_access_enable_refresh_set(unit, dram_index, channel,
                                                                 channel_enable_refresh, channel_enable_refresh_ab));
    }

exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_gddr6_mode_register_get(
    int unit,
    int dram_index,
    int channel,
    int mr_index,
    uint32 *value)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_gddr6_index_verify(unit, dram_index));
    SHR_IF_ERR_EXIT(dnx_gddr6_dbal_access_channel_mr_get(unit, dram_index, channel, mr_index, value));

exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_gddr6_mode_register_set(
    int unit,
    int dram_index,
    int channel,
    int mr_index,
    uint32 value)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_gddr6_index_verify(unit, dram_index));
    SHR_IF_ERR_EXIT(dnx_gddr6_dbal_access_channel_mr_set(unit, dram_index, channel, mr_index, value));

exit:
    SHR_FUNC_EXIT;
}

/*See header file*/
shr_error_e
dnx_gddr6_channels_soft_init(
    int unit,
    int dram_index)
{
    int channel;
    SHR_FUNC_INIT_VARS(unit);
    for (channel = 0; channel < dnx_data_dram.general_info.nof_channels_get(unit); channel++)
    {
        /** put in soft init */
        SHR_IF_ERR_EXIT(dnx_gddr6_dbal_access_channel_soft_init_set(unit, dram_index, channel, 1));
        /** take out of soft init */
        SHR_IF_ERR_EXIT(dnx_gddr6_dbal_access_channel_soft_init_set(unit, dram_index, channel, 0));
    }

exit:
    SHR_FUNC_EXIT;
}

/*See header file*/
shr_error_e
dnx_gddr6_ind_access_physical_address_read(
    int unit,
    uint32 dram_index,
    uint32 channel,
    uint32 bank,
    uint32 row,
    uint32 column,
    uint32 *data)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_gddr6_index_verify(unit, dram_index));

    /** read physical address using dbal */
    SHR_IF_ERR_EXIT(dnx_gddr6_dbal_access_dram_cpu_access_get(unit, dram_index, channel, bank, row, column, data));

exit:
    SHR_FUNC_EXIT;
}

/*See header file*/
shr_error_e
dnx_gddr6_ind_access_physical_address_write(
    int unit,
    uint32 dram_index,
    uint32 channel,
    uint32 bank,
    uint32 row,
    uint32 column,
    uint32 *pattern)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_gddr6_index_verify(unit, dram_index));

    /** write physical address using dbal */
    SHR_IF_ERR_EXIT(dnx_gddr6_dbal_access_dram_cpu_access_set(unit, dram_index, channel, bank, row, column, pattern));

exit:
    SHR_FUNC_EXIT;
}

static uint32
dnx_gddr6_xor_bits(
    uint32 val)
{
    val ^= val >> 16;
    val ^= val >> 8;
    val ^= val >> 4;
    val ^= val >> 2;
    val ^= val >> 1;
    return val & 1;
}

static shr_error_e
dnx_gddr6_ind_access_logical_to_physical_address_translate(
    int unit,
    int buffer,
    int index,
    uint32 *address)
{
    const uint32 *atm;
    uint32 logical_address;
    uint32 jumbled_physical_address = 0;
    uint32 base_physical_address;
    uint32 base_physical_address_unchanged_part;
    uint32 base_physical_address_shifted_column_part;
    uint32 bits_in_physical_address = dnx_data_dram.address_translation.matrix_column_size_get(unit);
    uint32 index_in_offset_bits = GDDR6_TRANSACTION_BITS_IN_LOGICAL_ADDRESS;
    uint32 offsets_for_buffer_bits = GDDR6_LOGICAL_ADDRESSES_BITS_PER_BUFFER;

    SHR_FUNC_INIT_VARS(unit);

    /** get address translation matrix */
    SHR_IF_ERR_EXIT(dnx_gddr6_tdu_atm_get(unit, &atm));

    /**
     * set logical address based on buffer and index:
     * each buffer represent (1 << offsets_for_buffer_bits) logical addresses, the specific wanted offset
     * is derived from the index by removing the bits relevant for the index_in_offset_bits.
     */
    logical_address = (buffer << offsets_for_buffer_bits) | (index >> index_in_offset_bits);
    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "%s(): logical buffer=%d, index=%d, logical_address=0x%x\n"),
                                 FUNCTION_NAME(), buffer, index, logical_address));

    /**
     * set jumbled physical address based on logical address and atm:
     * each bit in the jumbled_physical_address is received by AND operation on each bit in logical_address with
     * the line corresponding to the bit in the address translation matrix (atm). the result is XORed between all
     * bits to receive the bit's value, then it is shifted into place
     */
    for (int bit = 0; bit < bits_in_physical_address; ++bit)
    {
        jumbled_physical_address |= dnx_gddr6_xor_bits(logical_address & atm[bit]) << bit;
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit,
                                                "%s(): logical_address=0x%x, atm[%d]=0x%x, jumbled_physical_address_bit_%d=%d\n"),
                                     FUNCTION_NAME(), logical_address, bit, atm[bit], bit,
                                     jumbled_physical_address >> bit));
    }

    /**
     * set base physical address based on jumbled physical address:
     * module, channel, bank and row are not jumbled and represent the correct values of the base address,
     * they are taken as is. from the column - only relevant bits are taken and shifted into correct place.
     * the rest of the column bits are determined by the index.
     */
    base_physical_address_unchanged_part = (jumbled_physical_address &
                                            (PHY_ADDRESS_MODULE_MASK | PHY_ADDRESS_CHANNEL_MASK | PHY_ADDRESS_BANK_MASK
                                             | PHY_ADDRESS_ROW_MASK));
    base_physical_address_shifted_column_part =
        (((jumbled_physical_address & PHY_ADDRESS_JUMBLED_RELEVANT_COLUMN_MASK) >>
          PHY_ADDRESS_JUMBLED_RELEVANT_COLUMN_OFFSET) << PHY_ADDRESS_CORRECT_RELEVANT_COLUMN_OFFSET);
    base_physical_address = base_physical_address_unchanged_part | base_physical_address_shifted_column_part;
    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit,
                                            "%s(): jumbled_physical_address=0x%x, base_physical_address=0x%x\n"),
                                 FUNCTION_NAME(), jumbled_physical_address, base_physical_address));

    /**
     * set physical address based on base physical address and index:
     * lower bits in index represent are the missing column bits, they need to be shifted into location
     */
    *address = base_physical_address | ((index & GET_BIT_MASK(index_in_offset_bits, 0)) <<
                                        PHY_ADDRESS_SEQUENTIAL_COLUMN_OFFSET);

    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "%s(): logical buffer=%d, index=%d, physical address=0x%x\n"),
                                 FUNCTION_NAME(), buffer, index, *address));

exit:
    SHR_FUNC_EXIT;
}

/*See header file*/
shr_error_e
dnx_gddr6_ind_access_logical_buffer_read(
    int unit,
    int buffer,
    int index,
    uint32 *data)
{
    int stop_index;
    int start_index;
    int nof_indexes;
    int transaction_size_in_words;

    SHR_FUNC_INIT_VARS(unit);
    if (index == -1)
    {
        /** iterate over all indexes */
        nof_indexes = dnx_data_dram.general_info.buffer_size_get(unit) /
            dnx_data_dram.address_translation.physical_address_transaction_size_get(unit);
        start_index = 0;
        stop_index = nof_indexes - 1;
    }
    else
    {
        start_index = index;
        stop_index = index;
    }

    transaction_size_in_words =
        dnx_data_dram.address_translation.physical_address_transaction_size_get(unit) / BYTES_IN_WORD;
    for (int current_index = start_index; current_index <= stop_index; ++current_index)
    {
        uint32 address;
        /** get physical address */
        SHR_IF_ERR_EXIT(dnx_gddr6_ind_access_logical_to_physical_address_translate
                        (unit, buffer, current_index, &address));
        /** read from physical address */
        SHR_IF_ERR_EXIT(dnx_gddr6_ind_access_physical_address_read(unit, PHY_ADDRESS_GET_MODULE(address),
                                                                   PHY_ADDRESS_GET_CHANNEL(address),
                                                                   PHY_ADDRESS_GET_BANK(address),
                                                                   PHY_ADDRESS_GET_ROW(address),
                                                                   PHY_ADDRESS_GET_COLUMN(address),
                                                                   data + (current_index -
                                                                           start_index) * transaction_size_in_words));
    }

exit:
    SHR_FUNC_EXIT;
}

/*See header file*/
shr_error_e
dnx_gddr6_ind_access_logical_buffer_write(
    int unit,
    int buffer,
    int index,
    uint32 *pattern)
{
    int stop_index;
    int start_index;
    int nof_indexes;

    SHR_FUNC_INIT_VARS(unit);
    if (index == -1)
    {
        /** iterate over all indexes */
        nof_indexes = dnx_data_dram.general_info.buffer_size_get(unit) /
            dnx_data_dram.address_translation.physical_address_transaction_size_get(unit);
        start_index = 0;
        stop_index = nof_indexes - 1;
    }
    else
    {
        start_index = index;
        stop_index = index;
    }

    for (int current_index = start_index; current_index <= stop_index; ++current_index)
    {
        uint32 address;
        /** get physical address */
        SHR_IF_ERR_EXIT(dnx_gddr6_ind_access_logical_to_physical_address_translate
                        (unit, buffer, current_index, &address));
        /** write to physical address */
        SHR_IF_ERR_EXIT(dnx_gddr6_ind_access_physical_address_write(unit, PHY_ADDRESS_GET_MODULE(address),
                                                                    PHY_ADDRESS_GET_CHANNEL(address),
                                                                    PHY_ADDRESS_GET_BANK(address),
                                                                    PHY_ADDRESS_GET_ROW(address),
                                                                    PHY_ADDRESS_GET_COLUMN(address), pattern));
    }

exit:
    SHR_FUNC_EXIT;
}

/** See header file */
shr_error_e
dnx_gddr6_traffic_is_empty_get(
    int unit,
    uint8 *dram_is_empty)
{
    uint32 nof_deleted_buffers;
    uint32 nof_buffers;
    uint32 nof_free_buffers;
    uint32 bdbs_per_core;
    uint32 nof_cores = dnx_data_device.general.nof_cores_get(unit);

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(dram_is_empty, _SHR_E_PARAM, "dram_is_empty");

    /** empty quarantine FIFO */
    SHR_IF_ERR_EXIT(dnx_dram_buffers_quarantine_buffer_flush_all(unit));

    /** get how many buffers were already deleted */
    SHR_IF_ERR_EXIT(dnx_dram_buffers_quarantine_nof_deleted_buffers_get(unit, &nof_deleted_buffers));

    /** get number of free buffers */
    SHR_IF_ERR_EXIT(dnx_dram_buffers_quarantine_nof_free_buffers_get(unit, &nof_free_buffers));

    /** check that all buffers were freed and set indication */
    SHR_IF_ERR_EXIT(dnx_dram_buffers_nof_bdbs_per_core_get(unit, &bdbs_per_core));
    nof_buffers = nof_cores * bdbs_per_core;
    if (nof_buffers - nof_deleted_buffers > nof_free_buffers)
    {
        *dram_is_empty = FALSE;
    }
    else if (nof_buffers - nof_deleted_buffers < nof_free_buffers)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "number of free buffers exceed number of buffers in the system, something is wrong\n");
    }
    else
    {
        *dram_is_empty = TRUE;
    }

exit:
    SHR_FUNC_EXIT;
}

/** See header file */
shr_error_e
dnx_gddr6_redirect_traffic_to_ocb(
    int unit,
    uint32 enable,
    uint8 *dram_is_empty)
{
    static const uint8 emptying_delay_seconds = 10;

    SHR_FUNC_INIT_VARS(unit);
    if (enable)
    {
        /** need to enable redirection of traffic to OCB - meaning stop traffic to DRAMs */
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_dram_usage_stop(unit));
        /** if need to get is_empty indication, wait a while and check */
        if (dram_is_empty != NULL)
        {
            sal_sleep(emptying_delay_seconds);
            SHR_IF_ERR_EXIT(dnx_gddr6_traffic_is_empty_get(unit, dram_is_empty));
        }
    }
    else
    {
        /** need to disable redirection of traffic to OCB - meaning restoring traffic to HBM */
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_dram_usage_restore(unit));
        if (dram_is_empty != NULL)
        {
            *dram_is_empty = TRUE;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/** See header file */
shr_error_e
soc_dnx_gddr6_access_init(
    int unit)
{
    int nof_drams;
    uint32 dram_bitmap[1];
    int dram_index;
    SHR_FUNC_INIT_VARS(unit);

    /** this is done to prevent coverity issue of ARRAY_VS_SINGLETON from occuring */
    dram_bitmap[0] = dnx_data_dram.general_info.dram_info_get(unit)->dram_bitmap;
    nof_drams = dnx_data_dram.general_info.max_nof_drams_get(unit);

    /** register CBs to shmoo mechanism */
    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "%s(): register CBs\n"), FUNCTION_NAME()));
    SHR_IF_ERR_EXIT(dnx_gddr6_call_backs_register(unit));

    SHR_BIT_ITER(dram_bitmap, nof_drams, dram_index)
    {
        /** put GDDR6 channels into soft reset */
        SHR_IF_ERR_EXIT(dnx_gddr6_channels_soft_reset_set(unit, dram_index, 1));
        /** put GDDR6 channels into soft init */
        SHR_IF_ERR_EXIT(dnx_gddr6_channels_soft_init_set(unit, dram_index, 1));

        /** steps before out of soft reset - basically pll configurations */
        SHR_IF_ERR_EXIT(dnx_gddr6_phy_pll_init(unit, dram_index));

        /** GDDR6 phy out of reset */
        SHR_IF_ERR_EXIT(dnx_gddr6_phy_out_of_reset(unit, dram_index));

        /** take GDDR6 channels out of soft reset */
        SHR_IF_ERR_EXIT(dnx_gddr6_channels_soft_reset_set(unit, dram_index, 0));
        /** take GDDR6 channels out of soft init */
        SHR_IF_ERR_EXIT(dnx_gddr6_channels_soft_init_set(unit, dram_index, 0));

        /** allow access to dynamic memories, basically access to phy registers and dram mode registers */
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "%s(): allow access to phy registers\n"), FUNCTION_NAME()));
        SHR_IF_ERR_EXIT(dnx_gddr6_dbal_access_dynamic_memory_access_set(unit, dram_index, 1));
    }

exit:
    SHR_FUNC_EXIT;
}
/** see header file */
shr_error_e
dnx_gddr6_tdu_configure(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    /** TDU configuration */
    SHR_IF_ERR_EXIT(dnx_gddr6_dbal_access_tdu_configure(unit));
    /** TDU ATM configuration */
    SHR_IF_ERR_EXIT(dnx_gddr6_tdu_atm_configure(unit));
exit:
    SHR_FUNC_EXIT;
}

/** See header file */
shr_error_e
dnx_gddr6_bist_err_cnt_get(
    int unit,
    int dram_index,
    int channel,
    dnx_gddr6_bist_err_cnt_t * bist_err_cnt)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_gddr6_dbal_access_bist_err_cnt_get(unit, dram_index, channel, bist_err_cnt));
exit:
    SHR_FUNC_EXIT;
}

/** See header file */
shr_error_e
dnx_gddr6_bist_status_get(
    int unit,
    int dram_index,
    int channel,
    dnx_gddr6_bist_status_cnt_t * bist_status_cnt)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_gddr6_dbal_access_bist_status_cnt_get(unit, dram_index, channel, bist_status_cnt));

exit:
    SHR_FUNC_EXIT;
}

/** See header file */
shr_error_e
dnx_gddr6_dynamic_calibration_enable(
    int unit,
    int dram_index,
    int channel,
    int set_init_position)
{
    uint32 enable_refresh;
    uint32 enable_refresh_ab;
    SHR_FUNC_INIT_VARS(unit);

    /** Check Refresh is enabled, if it isn't print a warning message */
    SHR_IF_ERR_EXIT(dnx_gddr6_dbal_access_enable_refresh_get
                    (unit, dram_index, channel, &enable_refresh, &enable_refresh_ab));
    if ((!enable_refresh) || (!enable_refresh_ab))
    {
        /** print warning that Refresh was enabled */
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U
                  (unit,
                   "bank refresh and all bank refresh is now enabled for dram %d, channel %d, to allow dynamic calibration\n"),
                  dram_index, channel));
        SHR_IF_ERR_EXIT(dnx_gddr6_dbal_access_enable_refresh_set(unit, dram_index, channel, 1, 1));
    }

    /** reset shadow fifo */
    SHR_IF_ERR_EXIT(dnx_gddr6_dbal_access_channel_rd_fifo_rstn_set(unit, dram_index, channel, 0));
    SHR_IF_ERR_EXIT(dnx_gddr6_dbal_access_channel_rd_fifo_rstn_set(unit, dram_index, channel, 1));

    /** configure dynamic calibration params to DBAL */
    SHR_IF_ERR_EXIT(dnx_gddr6_dbal_access_dynamic_calibration_config(unit, dram_index, channel, set_init_position));

    /** enable dynamic calibration mechanism */
    SHR_IF_ERR_EXIT(dnx_gddr6_dbal_access_dynamic_calibration_enable_set(unit, dram_index, channel, 1));

exit:
    SHR_FUNC_EXIT;
}

/** See header file */
shr_error_e
dnx_gddr6_dynamic_calibration_is_enabled(
    int unit,
    int dram_index,
    int channel,
    uint32 *is_enabled)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_gddr6_dbal_access_dynamic_calibration_enable_get(unit, dram_index, channel, is_enabled));

exit:
    SHR_FUNC_EXIT;
}

/** See header file */
shr_error_e
dnx_gddr6_dynamic_calibration_disable(
    int unit,
    int dram_index,
    int channel)
{
    SHR_FUNC_INIT_VARS(unit);

    /** disable dynamic calibration mechanism */
    SHR_IF_ERR_EXIT(dnx_gddr6_dbal_access_dynamic_calibration_enable_set(unit, dram_index, channel, 0));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Read dram info (temperature and vendor info)
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [in] mode - determines which info is read (see dnx_gddr6_info_mode_t)
 * \param [out] info - info read
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
dnx_gddr6_dram_info_read(
    int unit,
    int dram_index,
    dnx_gddr6_info_mode_t mode,
    uint32 *info)
{
    dnx_gddr6_training_bist_cmd_configuration training_bist_conf = {.training_bist_mode = VENDOR_ID };
    dnx_gddr6_bist_last_returned_data_t last_returned_data = {.last_returned_dq = {0} };
    int read_dbi_enabled = 0;
    uint32 curr_refi;
    uint32 curr_refi_ab;
    uint32 dynamic_calibration_enabled[DNX_DATA_MAX_DRAM_GENERAL_INFO_NOF_CHANNELS] = { 0 };
    SHR_FUNC_INIT_VARS(unit);

    /** disable dynamic calibration */
    for (int channel = 0; channel < dnx_data_dram.general_info.nof_channels_get(unit); ++channel)
    {
        SHR_IF_ERR_EXIT(dnx_gddr6_dynamic_calibration_is_enabled
                        (unit, dram_index, channel, &dynamic_calibration_enabled[channel]));
        SHR_IF_ERR_EXIT(dnx_gddr6_dynamic_calibration_disable(unit, dram_index, channel));
    }
    /** disable dram refresh */
    SHR_IF_ERR_EXIT(dnx_gddr6_enable_refresh_cb(unit, dram_index, 0, 0, 0, &curr_refi, &curr_refi_ab));

    /** configure training BIST to read info */
    SHR_IF_ERR_EXIT(dnx_gddr6_dbal_access_cmd_training_bist_configure_set(unit, dram_index, &training_bist_conf));
    SHR_IF_ERR_EXIT(dnx_gddr6_dbal_access_vendor_id_mode_set(unit, dram_index, mode));

    /** get current DBI status */
    read_dbi_enabled = dnx_data_dram.general_info.dbi_read_get(unit);
    /** disable DBI */
    SHR_IF_ERR_EXIT(dnx_gddr6_enable_rd_dbi_cb(unit, dram_index, 0));

    /** read info */
    SHR_IF_ERR_EXIT(dnx_gddr6_dbal_access_training_bist_run(unit, dram_index, 1));
    SHR_IF_ERR_EXIT(dnx_gddr6_dbal_access_channel_bist_last_returned_get(unit, dram_index, 0, &last_returned_data));
    *info = last_returned_data.last_returned_dq[0] & 0xFFFF;

    /** restore DBI according to saved status */
    SHR_IF_ERR_EXIT(dnx_gddr6_enable_rd_dbi_cb(unit, dram_index, !!read_dbi_enabled));

    /** restore refresh */
    SHR_IF_ERR_EXIT(dnx_gddr6_enable_refresh_cb
                    (unit, dram_index, 1, curr_refi, curr_refi_ab, &curr_refi, &curr_refi_ab));

    /** restore dynamic calibration */
    for (int channel = 0; channel < dnx_data_dram.general_info.nof_channels_get(unit); ++channel)
    {
        if (dynamic_calibration_enabled[channel])
        {
            SHR_IF_ERR_EXIT(dnx_gddr6_dynamic_calibration_enable(unit, dram_index, channel, 1));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/** See header file */
shr_error_e
dnx_gddr6_dram_temp_get(
    int unit,
    int dram_index,
    int *temp)
{
    uint32 raw_value = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_gddr6_dram_info_read(unit, dram_index, DNX_GDDR6_TEMP, &raw_value));
    raw_value &= 0xFF;
    *temp = -40 + raw_value * 2;

exit:
    SHR_FUNC_EXIT;
}

/** See header file */
shr_error_e
dnx_gddr6_dram_vendor_id_get(
    int unit,
    int dram,
    uint32 *vendor_id)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_gddr6_dram_info_read(unit, dram, DNX_GDDR6_ID1, vendor_id));

exit:
    SHR_FUNC_EXIT;
}

/** See header file */
shr_error_e
dnx_gddr6_dram_vendor_id_to_vendor_info_decode(
    int unit,
    uint32 vendor_id,
    gddr6_dram_vendor_info_t * vendor_info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_BITCOPY_RANGE(&vendor_info->manufacturer_vendor_code, 0, &vendor_id, 0, 4);
    SHR_BITCOPY_RANGE(&vendor_info->revision_id, 0, &vendor_id, 4, 4);
    SHR_BITCOPY_RANGE(&vendor_info->density, 0, &vendor_id, 8, 3);
    SHR_BITCOPY_RANGE(&vendor_info->internal_wck, 0, &vendor_id, 11, 1);
    SHR_BITCOPY_RANGE(&vendor_info->wck_granularity, 0, &vendor_id, 12, 1);
    SHR_BITCOPY_RANGE(&vendor_info->wck_frequency, 0, &vendor_id, 13, 2);
    SHR_BITCOPY_RANGE(&vendor_info->vddq_off, 0, &vendor_id, 15, 1);

    SHR_FUNC_EXIT;
}

/** See header file */
shr_error_e
dnx_gddr6_vendor_info_from_sw_state_get(
    int unit,
    int dram_index,
    gddr6_dram_vendor_info_t * vendor_info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(vendor_info, _SHR_E_PARAM, "vendor_info");

    SHR_IF_ERR_EXIT(dnx_dram_db.gddr6_vendor_info.is_init.get(unit, dram_index, &vendor_info->is_init));
    SHR_IF_ERR_EXIT(dnx_dram_db.gddr6_vendor_info.
                    manufacturer_vendor_code.get(unit, dram_index, &vendor_info->manufacturer_vendor_code));
    SHR_IF_ERR_EXIT(dnx_dram_db.gddr6_vendor_info.revision_id.get(unit, dram_index, &vendor_info->revision_id));
    SHR_IF_ERR_EXIT(dnx_dram_db.gddr6_vendor_info.density.get(unit, dram_index, &vendor_info->density));
    SHR_IF_ERR_EXIT(dnx_dram_db.gddr6_vendor_info.internal_wck.get(unit, dram_index, &vendor_info->internal_wck));
    SHR_IF_ERR_EXIT(dnx_dram_db.gddr6_vendor_info.wck_granularity.get(unit, dram_index, &vendor_info->wck_granularity));
    SHR_IF_ERR_EXIT(dnx_dram_db.gddr6_vendor_info.wck_frequency.get(unit, dram_index, &vendor_info->wck_frequency));
    SHR_IF_ERR_EXIT(dnx_dram_db.gddr6_vendor_info.vddq_off.get(unit, dram_index, &vendor_info->vddq_off));

exit:
    SHR_FUNC_EXIT;
}

/** See header file */
shr_error_e
dnx_gddr6_vendor_info_from_sw_state_set(
    int unit,
    int dram_index,
    gddr6_dram_vendor_info_t * vendor_info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(vendor_info, _SHR_E_PARAM, "vendor_info");

    SHR_IF_ERR_EXIT(dnx_dram_db.gddr6_vendor_info.is_init.set(unit, dram_index, TRUE));
    SHR_IF_ERR_EXIT(dnx_dram_db.gddr6_vendor_info.
                    manufacturer_vendor_code.set(unit, dram_index, vendor_info->manufacturer_vendor_code));
    SHR_IF_ERR_EXIT(dnx_dram_db.gddr6_vendor_info.revision_id.set(unit, dram_index, vendor_info->revision_id));
    SHR_IF_ERR_EXIT(dnx_dram_db.gddr6_vendor_info.density.set(unit, dram_index, vendor_info->density));
    SHR_IF_ERR_EXIT(dnx_dram_db.gddr6_vendor_info.internal_wck.set(unit, dram_index, vendor_info->internal_wck));
    SHR_IF_ERR_EXIT(dnx_dram_db.gddr6_vendor_info.wck_granularity.set(unit, dram_index, vendor_info->wck_granularity));
    SHR_IF_ERR_EXIT(dnx_dram_db.gddr6_vendor_info.wck_frequency.set(unit, dram_index, vendor_info->wck_frequency));
    SHR_IF_ERR_EXIT(dnx_dram_db.gddr6_vendor_info.vddq_off.set(unit, dram_index, vendor_info->vddq_off));

exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_gddr6_call_backs_register(
    int unit)
{
    g6phy16_shmoo_cbi_t shmoo_cbi;
    SHR_FUNC_INIT_VARS(unit);

    shmoo_cbi.g6phy16_phy_reg_read = dnx_gddr6_phy_reg_read_cb;
    shmoo_cbi.g6phy16_phy_reg_write = dnx_gddr6_phy_reg_write_cb;
    shmoo_cbi.g6phy16_phy_reg_modify = dnx_gddr6_phy_reg_modify_cb;
    shmoo_cbi.g6phy16_drc_bist_conf_set = dnx_gddr6_bist_conf_set_cb;
    shmoo_cbi.g6phy16_drc_bist_run = dnx_gddr6_bist_run_cb;
    shmoo_cbi.g6phy16_drc_bist_err_cnt = dnx_gddr6_bist_err_cnt_cb;
    shmoo_cbi.g6phy16_drc_dram_init = dnx_gddr6_dram_init_cb;
    shmoo_cbi.g6phy16_drc_pll_set = dnx_gddr6_pll_set_cb;
    shmoo_cbi.g6phy16_drc_modify_mrs = dnx_gddr6_modify_mrs_cb;
    shmoo_cbi.g6phy16_drc_enable_wck2ck_training = dnx_gddr6_enable_wck2ck_training_cb;
    shmoo_cbi.g6phy16_drc_enable_write_leveling = dnx_gddr6_enable_write_leveling_cb;
    shmoo_cbi.g6phy16_drc_mpr_en = dnx_gddr6_mpr_en_cb;
    shmoo_cbi.g6phy16_drc_mpr_load = dnx_gddr6_mpr_load_cb;
    shmoo_cbi.g6phy16_drc_vendor_info_get = dnx_gddr6_vendor_info_get_cb;
    shmoo_cbi.g6phy16_drc_dqs_pulse_gen = dnx_gddr6_dqs_pulse_gen_cb;
    shmoo_cbi.g6phy16_training_bist_conf_set = dnx_gddr6_training_bist_conf_set_cb;
    shmoo_cbi.g6phy16_training_bist_err_cnt = dnx_gddr6_training_bist_err_cnt_cb;
    shmoo_cbi.g6phy16_gddr6_shmoo_drc_dq_byte_pairs_swap_info_get = dnx_gddr6_shmoo_drc_dq_byte_pairs_swap_info_get_cb;
    shmoo_cbi.g6phy16_drc_enable_wr_crc = dnx_gddr6_enable_wr_crc_cb;
    shmoo_cbi.g6phy16_drc_enable_rd_crc = dnx_gddr6_enable_rd_crc_cb;
    shmoo_cbi.g6phy16_drc_enable_wr_dbi = dnx_gddr6_enable_wr_dbi_cb;
    shmoo_cbi.g6phy16_drc_enable_rd_dbi = dnx_gddr6_enable_rd_dbi_cb;
    shmoo_cbi.g6phy16_drc_force_dqs = dnx_gddr6_force_dqs_cb;
    shmoo_cbi.g6phy16_drc_soft_reset_drc_without_dram = dnx_gddr6_soft_reset_controller_without_dram_cb;
    shmoo_cbi.g6phy16_shmoo_dram_info_access = dnx_gddr6_dram_info_access_cb;

    /*
     * g6phy16_shmoo: Register Device interface CB to shmoo 
     */
    SHR_IF_ERR_EXIT(soc_g6phy16_shmoo_interface_cb_register(unit, shmoo_cbi));
exit:
    SHR_FUNC_EXIT;
}
