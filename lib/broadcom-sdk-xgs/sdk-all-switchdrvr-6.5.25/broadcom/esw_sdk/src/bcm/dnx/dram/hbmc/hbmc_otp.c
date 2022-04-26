/** \file src/bcm/dnx/dram/hbmc/hbmc_otp.c
 *
 *
 *  This file contains implimentation of functions for HBMC OTP
 *
 */

/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_DRAM

/** allow memregs.h include excplictly */
#ifdef _SOC_MEMREGS_H
#undef _SOC_MEMREGS_H
#endif
/*
 * Include files.
 * {
 */

#include <shared/shrextend/shrextend_debug.h>
#include <soc/memory.h>
#include <soc/feature.h>
#include <soc/types.h>
#include <soc/register.h>
#include <soc/dnxc/drv_dnxc_utils.h>
#include <soc/mcm/memregs.h>
#include <soc/cm.h>
#include <soc/cmic.h>
#include <soc/util.h>

#include <sal/appl/sal.h>
#include <bcm_int/dnx/dram/hbmc/hbmc.h>
#include <bcm_int/dnx/dram/hbmc/hbmc_cb.h>
#include <soc/shmoo_hbm16.h>
#include "hbmc_dbal_access.h"
#include "hbmc_monitor.h"
#include "hbmc_otp.h"
#include <bcm_int/dnx/dram/dram.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dram.h>
#include <soc/sand/sand_aux_access.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_ingress_congestion_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_dram_access.h>
#include <soc/dnxc/dnxc_ha.h>
#include <shared/utilex/utilex_bitstream.h>
#include <shared/utilex/utilex_integer_arithmetic.h>

/*
 * }
 */

/*
 * defines/enums
 * {
 */

#define DNX_HBMC_OTP_NOF_ROWS (64) /** define how many otp rows */
#define DNX_HBMC_OTP_ROW_DATA_BITS (64) /** define how many data bits we have in each OTP row */
#define DNX_HBMC_OTP_ROW_ECC_BITS (0)
#define DNX_HBMC_OTP_ROW_BITS (DNX_HBMC_OTP_ROW_DATA_BITS + DNX_HBMC_OTP_ROW_ECC_BITS) /** define how many bits we have in each OTP row */
#define DNX_HBMC_OTP_NOF_USER_BITS_ROWS (42) /** define how many otp rows are dedicated for user bits info */
#define DNX_HBMC_OTP_USER_BITS_ROWS_OFFSET (22) /** define the first row that user can write into */
#define DNX_HBMC_OTP_ROW_BITS_UINT32 (DNX_HBMC_OTP_ROW_BITS/32 + (DNX_HBMC_OTP_ROW_BITS%32>0)) /** how many uint32 need to fill one row */
#define DNX_HBMC_OTP_BLOCK_ID(_idx) ((_idx == 0) ? (896) : (912))
#define DNX_HBMC_OTP_DATA_ARR_MAX_SIZE (64)
typedef enum dnx_hbmc_otp_cmd_e
{
    READ_CMD = 0x0,
    PROG_EN_CMD = 0x2,
    PROG_DIS_CMD = 0x3,
} dnx_hbmc_otp_cmd_t;

/*
 * }
 */

/**
 * \brief - write to otp register
 *
 * \param [in] unit - unit number
 * \param [in] hbm_index - hbm index
 * \param [in] addr - address of the register
 * \param [in] val - value 
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_hbmc_otp_reg_write(
    int unit,
    int hbm_index,
    int addr,
    uint32 val)
{
    SHR_FUNC_INIT_VARS(unit);

    addr = addr << 8;
    SHR_IF_ERR_EXIT(soc_direct_reg_set(unit, DNX_HBMC_OTP_BLOCK_ID(hbm_index), addr, 1, &val));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - read from otp register
 *
 * \param [in] unit - unit number
 * \param [in] hbm_index - hbm index
 * \param [in] addr - address of the register
 * \param [out] *val - read value 
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_hbmc_otp_reg_read(
    int unit,
    int hbm_index,
    uint32 addr,
    uint32 *val)
{
    SHR_FUNC_INIT_VARS(unit);

    addr = addr << 8;
    SHR_IF_ERR_EXIT(soc_direct_reg_get(unit, DNX_HBMC_OTP_BLOCK_ID(hbm_index), addr, 1, val));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - enable access to otp 
 *
 * \param [in] unit - unit number
 * \param [in] hbm_index - hbm index
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_hbmc_otp_enable_cpu_access(
    int unit,
    int hbm_index)
{
    int OTPC_MODE_REG_reg_ = 0x200;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_hbmc_otp_reg_write(unit, hbm_index, OTPC_MODE_REG_reg_, 0x1));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - convert the version number of the burned data to version bitmap. 
 *
 * \param [in] unit - unit number
 * \param [in] version - version number
 * \param [out] bitmap - version bitmap 
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_hbmc_otp_version_to_bitmap_convert(
    int unit,
    int version,
    uint32 *bitmap)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (version)
    {
        case 1:
            *bitmap = HBMC_SHMOO_OTP_VER_1;
            break;
        case 2:
            *bitmap = HBMC_SHMOO_OTP_VER_2;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "invalid otp version read. version=%d\n", version);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get the size of the data (+header) burned for a given version
 *
 * \param [in] unit - unit number
 * \param [in] version - version of the burn
 * \param [out] size - size of the data burned for the given version
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_hbmc_otp_version_size_get(
    int unit,
    int version,
    int *size)
{
    int metadata_idx;
    uint32 version_bitmap;
    uint32 field_size;
    const hbmc_shmoo_channel_config_param_t *channel_metadata = soc_hbm16_shmoo_channel_config_param_metadata_get();
    const hbmc_shmoo_midstack_config_param_t *midstack_metadata = soc_hbm16_shmoo_midstack_config_param_metadata_get();
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_hbmc_otp_version_to_bitmap_convert
                    (unit, dnx_data_dram.general_info.otp_restore_version_get(unit), &version_bitmap));

    *size = HBMC_OTP_HEADER_SIZE;
    for (metadata_idx = 0; metadata_idx < HBMC_SHMOO_CHANNEL_METADATA_SIZE; metadata_idx++)
    {
        if ((channel_metadata[metadata_idx].level & version_bitmap) != 0)
        {
            SHR_IF_ERR_EXIT(soc_hbm16_channel_field_size_get(unit, channel_metadata[metadata_idx].reg,
                                                             channel_metadata[metadata_idx].field, &field_size));
            /** we have multiple channels, therefore, multiple the size in NOF CHANNELS */
            (*size) += field_size * HBMC_SHMOO_NOF_HBM_CHANNELS;
        }
    }

    for (metadata_idx = 0; metadata_idx < HBMC_SHMOO_MIDSTACK_METADATA_SIZE; metadata_idx++)
    {
        if ((midstack_metadata[metadata_idx].level & version_bitmap) != 0)
        {
            SHR_IF_ERR_EXIT(soc_hbm16_midstack_field_size_get(unit, midstack_metadata[metadata_idx].reg,
                                                              midstack_metadata[metadata_idx].field, &field_size));
            (*size) += field_size;
        }
    }

    /*
     * rounding up the size & adding CRC16 size
     */
    *size = UTILEX_ALIGN_UP(*size, 8);
    *size = *size + 16;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - sbus operation for communicate the otp (read/write)
 *
 * \param [in] unit - unit number
 * \param [in] hbm_index - hbm index
 * \param [in] addr - address of the register 
 * \param [in] command - command to do in the otp
 * \param [in] wr_data - pointer to array of the data to write
 * \param [out] status - status indication from otp.
 * \param [out] rd_data - pointer to array of the data that was read from otp.
 * \param [out] rd_data_size - size of the data to read
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_hbmc_otp_sbus_operation(
    int unit,
    int hbm_index,
    uint32 addr,
    dnx_hbmc_otp_cmd_t command,
    uint32 *wr_data,
    uint32 *status,
    uint32 *rd_data,
    int rd_data_size)
{
    int command_int;
    soc_timeout_t timeout;
    sal_usecs_t timeout_value = 100000;

    /** All address were taken from OTP regfile located in : 
        $PERSONAL_PATH/verification/full_chip/env/regfile/otpc_sbus_regfile_base.sv */
    int OTPC_CPUADDR_REG_reg_ = 0x228;
    int OTPC_COMMAND_reg_ = 0x204;
    int OTPC_CMD_START_reg_ = 0x208;
    int OTPC_CPU_WRITE_REG0_reg_ = 0x22c;
    int OTPC_CPU_DATA0_reg_ = 0x210;
    int OTPC_CPU_DATA1_reg_ = 0x25c;
    int OTPC_CPU_DATA2_reg_ = 0x260;
    int OTPC_CPU_STATUS_reg_ = 0x20c;
    SHR_FUNC_INIT_VARS(unit);
    /** address */
    if (command == READ_CMD)
    {
        SHR_IF_ERR_EXIT(dnx_hbmc_otp_reg_write(unit, hbm_index, OTPC_CPUADDR_REG_reg_, addr));
    }
    /** command  */
    command_int = command;
    SHR_IF_ERR_EXIT(dnx_hbmc_otp_reg_write(unit, hbm_index, OTPC_COMMAND_reg_, command_int));
    /** write data  */
    if (command == PROG_EN_CMD)
    {
        SHR_IF_ERR_EXIT(dnx_hbmc_otp_reg_write(unit, hbm_index, OTPC_CPU_WRITE_REG0_reg_, wr_data[0]));
    }
    /** start - command write enable  */
    SHR_IF_ERR_EXIT(dnx_hbmc_otp_reg_write(unit, hbm_index, OTPC_CMD_START_reg_, 0x1));

    /** read cpu status */
    soc_timeout_init(&timeout, timeout_value, 0);
    do
    {
        SHR_IF_ERR_EXIT(dnx_hbmc_otp_reg_read(unit, hbm_index, OTPC_CPU_STATUS_reg_, status));
        /** check done indication bit */
        if (((*status) & 0x2) != 0)
        {
            break;
        }
        if (soc_timeout_check(&timeout))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "ERROR: (read_otp_mem_word) READ operation not done. CPU status = 0x%x.\n",
                         (*status));
        }
    }
    while (TRUE);

    if (command == READ_CMD)
    {
        if (rd_data_size > 0)
        {
            SHR_IF_ERR_EXIT(dnx_hbmc_otp_reg_read(unit, hbm_index, OTPC_CPU_DATA0_reg_, &rd_data[0]));
        }
        if (rd_data_size > 1)
        {
            SHR_IF_ERR_EXIT(dnx_hbmc_otp_reg_read(unit, hbm_index, OTPC_CPU_DATA1_reg_, &rd_data[1]));
        }
        if (rd_data_size > 2)
        {
            SHR_IF_ERR_EXIT(dnx_hbmc_otp_reg_read(unit, hbm_index, OTPC_CPU_DATA2_reg_, &rd_data[2]));
        }
    }

    /** disable - command write enable  */
    SHR_IF_ERR_EXIT(dnx_hbmc_otp_reg_write(unit, hbm_index, OTPC_CMD_START_reg_, 0x0));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Read one row - 72b (or 64 without ecc) entry from the OTP (3x32b or 2*32b)
 *
 * \param [in] unit - unit number
 * \param [in] hbm_index - hbm index
 * \param [in] addr - address of the row to read
 * \param [out] rd_data - read data
 * \param [in] data_size - data size to read
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_hbmc_otp_row_read(
    int unit,
    int hbm_index,
    uint32 addr,
    uint32 *rd_data,
    int data_size)
{
    uint32 rd_status;
    uint32 wr_data;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_hbmc_otp_sbus_operation
                    (unit, hbm_index, addr, READ_CMD, &wr_data, &rd_status, rd_data, data_size));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get the start bit to read from and the size to read.
 *  Function read the otp rows, search the header with the version required to read. verify it is not invalid, 
 *  and give back the start_bit to read the data from, after the header ends.
 * \param [in] unit - unit number
 * \param [in] hbm_index - hbm index
 * \param [in] is_quiet - TRUE means don't print out error message if there are no tune params saved to OTP
 * \param [out] base_bit_idx - read data
 * \param [out] data_size - data size to read
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_hbmc_otp_start_bit_get(
    int unit,
    int hbm_index,
    int is_quiet,
    int *base_bit_idx,
    int *data_size)
{
    uint32 version = 0xF;
    uint32 header;
    int bit_idx = (DNX_HBMC_OTP_USER_BITS_ROWS_OFFSET * DNX_HBMC_OTP_ROW_BITS);
    int size_per_ver = 0;
    uint32 addr;
    int invalid;
    uint32 row_data[DNX_HBMC_OTP_ROW_BITS_UINT32] = { 0 };
    uint32 concatenate_data[DNX_HBMC_OTP_ROW_BITS_UINT32 * 2] = { 0 };
    SHR_FUNC_INIT_VARS(unit);

    do
    {
        /** if reach end of memory, return error */
        if (bit_idx > (DNX_HBMC_OTP_NOF_ROWS * DNX_HBMC_OTP_ROW_BITS - HBMC_OTP_HEADER_SIZE))
        {
            SHR_ERR_EXIT(_SHR_E_MEMORY,
                         "HBM tune restore from otp - no valid data to read. hbm_index=%d (bit_idx=%d)  \n", hbm_index,
                         bit_idx);
        }

        /** calc the row (address) to read */
        addr = bit_idx / DNX_HBMC_OTP_ROW_BITS;
        /** read the row */
        SHR_IF_ERR_EXIT(dnx_hbmc_otp_row_read(unit, hbm_index, addr, row_data, (DNX_HBMC_OTP_ROW_BITS_UINT32)));
        SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field(row_data, 0, DNX_HBMC_OTP_ROW_BITS, concatenate_data));

        /** if the header is splitted on two rows, read the second row also */
        if (((bit_idx % DNX_HBMC_OTP_ROW_BITS) + HBMC_OTP_HEADER_SIZE) > DNX_HBMC_OTP_ROW_BITS)
        {
            SHR_IF_ERR_EXIT(dnx_hbmc_otp_row_read(unit, hbm_index, (addr + 1), row_data, DNX_HBMC_OTP_ROW_BITS_UINT32));
            SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                            (row_data, DNX_HBMC_OTP_ROW_BITS, DNX_HBMC_OTP_ROW_BITS, concatenate_data));
        }

        /** read the header for the rd_data */
        header = 0;
        SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field
                        (concatenate_data, (bit_idx - (addr * DNX_HBMC_OTP_ROW_BITS)), HBMC_OTP_HEADER_SIZE, &header));
        /** exctact the version and invalid from header */
        version = HBMC_OTP_HEADER_VERSION_GET(header);
        invalid = HBMC_OTP_HEADER_INVALID_GET(header);

        if (version == 0)
        {
            if (is_quiet)
            {
                SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_EMPTY);
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_EMPTY,
                             "HBM tune restore from otp - Not found any data to restore from otp. hbm_index=%d (bit_idx=%d) \n",
                             hbm_index, bit_idx);
            }
        }

        SHR_IF_ERR_EXIT(dnx_hbmc_otp_version_size_get(unit, version, &size_per_ver));

        /*
         * skip this data. go to next data
         */
        if ((version != dnx_data_dram.general_info.otp_restore_version_get(unit)) || ((invalid == 1)))
        {
            /** calc the size of the data */
            bit_idx += size_per_ver;
            if ((invalid == 0) && (version != 0))
            {
                LOG_WARN(BSL_LOG_MODULE, (BSL_META_U(unit,
                                                     "hbm tune restore from otp - read from otp version=%d, expected version=%d. hbm_index=%d \n"),
                                          version, dnx_data_dram.general_info.otp_restore_version_get(unit),
                                          hbm_index));
            }
        }

        /** break the while if read invlaid=0 and the version is the disered version */
    }
    while ((version != dnx_data_dram.general_info.otp_restore_version_get(unit)) || (invalid == 1));

    /** point to the data to read */
    (*base_bit_idx) = bit_idx;
    (*data_size) = size_per_ver;
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief - main procedure to read the HBM otp data
*
* \param [in] unit - unit number
* \param [in] hbm_index - hbm index
* \param [in] is_quiet - TRUE means don't print out error message if there are no tune params saved to OTP
* \param [out] concatenate_data - array that hold the tune data read from otp.
*
* \return
*   shr_error_e
*
* \remark
*   * None
* \see
*   * None
*/
shr_error_e
hbm_otp_read_data(
    int unit,
    int hbm_index,
    int is_quiet,
    uint32 *concatenate_data)
{
    int bit_idx = 0;
    int data_size = 0;
    int base_row, last_row, row_idx;
    uint16 crc;
    int i;
    uint32 concatenate_data_le[DNX_HBMC_OTP_DATA_ARR_MAX_SIZE];
    uint32 row_data[DNX_HBMC_OTP_ROW_BITS_UINT32] = { 0 };
    int accumulate_size;
    SHR_FUNC_INIT_VARS(unit);

     /** enable CMIC SBUS - already done in drv.c */

    /** enable crup access */
    SHR_IF_ERR_EXIT(dnx_hbmc_otp_enable_cpu_access(unit, hbm_index));

     /** get the bit index of the the place that need to start reading the data. (after header) */

    if (is_quiet)
    {
        SHR_IF_ERR_EXIT_WITH_MSG_EXCEPT_IF(dnx_hbmc_otp_start_bit_get(unit, hbm_index, is_quiet, &bit_idx, &data_size),
                                           _SHR_E_EMPTY);
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_hbmc_otp_start_bit_get(unit, hbm_index, is_quiet, &bit_idx, &data_size));
    }

    if ((data_size / 32) > DNX_HBMC_OTP_DATA_ARR_MAX_SIZE)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "data_size=%d, bigger than concatenate_data array size \n", data_size);
    }
    /*
     * calc the base row and nof_rows to burn 
     */
    base_row = bit_idx / DNX_HBMC_OTP_ROW_BITS;
    last_row = (bit_idx + data_size - 1) / DNX_HBMC_OTP_ROW_BITS;       /* -1 needed for rounding up only if the size
                                                                         * is not divided by row size without any
                                                                         * reminder */
    /** first row is different from the rest, because we want to copy just from start bit and not the entire row */
    SHR_IF_ERR_EXIT(dnx_hbmc_otp_row_read(unit, hbm_index, base_row, row_data, DNX_HBMC_OTP_ROW_BITS_UINT32));
    accumulate_size = (DNX_HBMC_OTP_ROW_BITS - (bit_idx % DNX_HBMC_OTP_ROW_BITS));
    SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field(row_data, (bit_idx % DNX_HBMC_OTP_ROW_BITS),
                                                   accumulate_size, concatenate_data));
    for (row_idx = (base_row + 1); row_idx <= last_row; row_idx++)
    {
        /** read the row */
        SHR_IF_ERR_EXIT(dnx_hbmc_otp_row_read(unit, hbm_index, row_idx, row_data, DNX_HBMC_OTP_ROW_BITS_UINT32));
        SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                        (row_data, accumulate_size, DNX_HBMC_OTP_ROW_BITS, concatenate_data));
        accumulate_size += DNX_HBMC_OTP_ROW_BITS;
    }

    /*
     * check crc
     */
    for (i = 0; i < data_size / 32; i++)
    {
#if defined(BE_HOST)
        /*
         * The Arm that burned the OTP is little-endian, so need to
         * swap the bytes in case of big endian cpu.
         */
        concatenate_data_le[i] = UTILEX_BYTE_SWAP(concatenate_data[i]);
#else
        concatenate_data_le[i] = concatenate_data[i];
#endif
    }

    crc = _shr_crc16(0, (unsigned char *) concatenate_data_le, (data_size) / 8);
    if (crc)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "the CRC of the image is different than zero 0x%x, CRC size 0x%x", crc,
                     (data_size) / 8);
    }

exit:
    SHR_FUNC_EXIT;
}

 /**
  * \brief - read from otp, hbm16 tune configuration. 
  *
  * \param [in] unit - unit number
  * \param [in] hbm_ndx - hbm index
  * \param [in] is_quiet - TRUE means don't print out error message if there are no tune params saved to OTP
  * \param [out] shmoo_config_param - pointer to array which function fill all the restored data from the file.
  *
  *
  * \return
  *   shr_error_e
  *
  * \remark
  *   * None
  * \see
  *   * None
  */
soc_error_t
dnx_hbmc_otp_shmoo_hbm16_restore_from_otp(
    int unit,
    int hbm_ndx,
    int is_quiet,
    hbmc_shmoo_config_param_t * shmoo_config_param)
{
    int bit_idx = 0;
    uint32 concatenate_data[DNX_HBMC_OTP_DATA_ARR_MAX_SIZE];
    const hbmc_shmoo_channel_config_param_t *channel_metadata = soc_hbm16_shmoo_channel_config_param_metadata_get();
    const hbmc_shmoo_midstack_config_param_t *midstack_metadata = soc_hbm16_shmoo_midstack_config_param_metadata_get();
    uint32 otp_restore_version;
    uint32 field_size;
    uint32 max_vdl_dqs_p_value = 0;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(concatenate_data, 0x0, DNX_HBMC_OTP_DATA_ARR_MAX_SIZE * sizeof(uint32));

     /** read bit string from OTP (all relevant lines) and fill concatenate_data */
     /** each otp row capture in 3 fields of prog_data: prog_data[0] = bits:0..31, prog_data[1] = bits:32..63, prog_data[2] = bits:64..71 */
    if (is_quiet)
    {
        SHR_IF_ERR_EXIT_WITH_MSG_EXCEPT_IF(hbm_otp_read_data(unit, hbm_ndx, is_quiet, concatenate_data), _SHR_E_EMPTY);
    }
    else
    {
        SHR_IF_ERR_EXIT(hbm_otp_read_data(unit, hbm_ndx, is_quiet, concatenate_data));
    }

    /*
     * skip the header
     */
    bit_idx += HBMC_OTP_HEADER_SIZE;

    SHR_IF_ERR_EXIT(dnx_hbmc_otp_version_to_bitmap_convert
                    (unit, dnx_data_dram.general_info.otp_restore_version_get(unit), &otp_restore_version));

     /** go over all relevant fields and extruct the data from concatenate_data */
    for (int channel_ndx = 0; channel_ndx < dnx_data_dram.general_info.nof_channels_get(unit); channel_ndx++)
    {
        for (int metadata_ndx = 0; metadata_ndx < HBMC_SHMOO_CHANNEL_METADATA_SIZE; metadata_ndx++)
        {
             /** if the parameter level (=version) belong to the given version, restore it  */
            if ((channel_metadata[metadata_ndx].level & otp_restore_version) != 0)
            {
                SHR_IF_ERR_EXIT(soc_hbm16_channel_field_size_get(unit, channel_metadata[metadata_ndx].reg,
                                                                 channel_metadata[metadata_ndx].field, &field_size));
                SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field(concatenate_data, bit_idx,
                                                               field_size,
                                                               &shmoo_config_param->channel_data[channel_ndx]
                                                               [metadata_ndx].value));
                shmoo_config_param->channel_data[channel_ndx][metadata_ndx].valid = TRUE;
                bit_idx += field_size;
                if ((channel_metadata[metadata_ndx].reg == HBM16_DWORD0_READ_MAX_VDL_DQS_Pr) ||
                    (channel_metadata[metadata_ndx].reg == HBM16_DWORD1_READ_MAX_VDL_DQS_Pr) ||
                    (channel_metadata[metadata_ndx].reg == HBM16_DWORD2_READ_MAX_VDL_DQS_Pr) ||
                    (channel_metadata[metadata_ndx].reg == HBM16_DWORD3_READ_MAX_VDL_DQS_Pr))
                {
                    max_vdl_dqs_p_value = shmoo_config_param->channel_data[channel_ndx][metadata_ndx].value;
                }
            }
            else
            {
                if (dnx_data_dram.general_info.otp_restore_version_get(unit) == 1)
                {
                    if (((channel_metadata[metadata_ndx].reg == HBM16_DWORD0_READ_MAX_VDL_DQS_Nr) ||
                         (channel_metadata[metadata_ndx].reg == HBM16_DWORD1_READ_MAX_VDL_DQS_Nr) ||
                         (channel_metadata[metadata_ndx].reg == HBM16_DWORD2_READ_MAX_VDL_DQS_Nr) ||
                         (channel_metadata[metadata_ndx].reg == HBM16_DWORD3_READ_MAX_VDL_DQS_Nr)) &&
                        (channel_metadata[metadata_ndx].field == HBM16_MAX_VDL_STEPf))
                    {
                        shmoo_config_param->channel_data[channel_ndx][metadata_ndx].valid = TRUE;
                        shmoo_config_param->channel_data[channel_ndx][metadata_ndx].value = max_vdl_dqs_p_value;
                    }
                }
            }
        }
    }

    for (int metadata_ndx = 0; metadata_ndx < HBMC_SHMOO_MIDSTACK_METADATA_SIZE; metadata_ndx++)
    {
         /** if the parameter level (=version) belong to the given version, restore it  */
        if ((midstack_metadata[metadata_ndx].level & otp_restore_version) != 0)
        {
            SHR_IF_ERR_EXIT(soc_hbm16_midstack_field_size_get(unit, midstack_metadata[metadata_ndx].reg,
                                                              midstack_metadata[metadata_ndx].field, &field_size));
            SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field(concatenate_data, bit_idx,
                                                           field_size,
                                                           &shmoo_config_param->midstack_data[metadata_ndx].value));
            shmoo_config_param->midstack_data[metadata_ndx].valid = TRUE;
            bit_idx += field_size;
        }
    }

exit:
    SHR_FUNC_EXIT;
}
