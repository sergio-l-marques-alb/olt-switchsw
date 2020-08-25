/** \file diag_dnx_dram.c
 *
 * Main diagnostics for dram applications
 *
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_CNT

/*************
 * INCLUDES  *
 *************/
/** appl */
#include <appl/diag/diag.h>
#include <appl/diag/system.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/sand/diag_sand_access.h>
#include <appl/reference/dnx/appl_ref_sys_ports.h>
/** bcm */
#include <bcm/types.h>
#include <bcm/cosq.h>
#include <bcm_int/dnx/dram/hbmc/hbmc_cb.h>
#include <bcm_int/dnx/dram/hbmc/hbmc.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm_int/dnx/gtimer/gtimer.h>
#include <bcm_int/dnx/dram/buffers/dram_buffers.h>
#include <bcm_int/dnx_dispatch.h>
#include <bcm_int/dnx/tune/tune.h>
#include <bcm_int/dnx/dram/dram.h>
/** soc */
#include <soc/shmoo_hbm16.h>
#include <soc/dnx/drv.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dram.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_dram_access.h>
#include <soc/dnx/legacy/mbcm.h>
/** sal */
#include <sal/appl/sal.h>
/** local */
#include "diag_dnx_dram.h"
/** shared */
#include <shared/bitop.h>
#include <shared/utilex/utilex_integer_arithmetic.h>

/*************
 * TYPEDEFS  *
 *************/

typedef enum dnx_dram_ind_access_mode_e
{
    LOGICAL = 0,
    PHYSICAL = 1
} dnx_dram_ind_access_mode_t;

typedef enum dnx_dram_ind_access_index_e
{
    ALL_INDEXES = -1
} dnx_dram_ind_access_index_t;

typedef enum dnx_dram_predefined_bist_e
{
    PREDEFINED_BIST_IO_STRESS,
    PREDEFINED_BIST_ALL_ADDRESS
} dnx_dram_predefined_bist_t;

typedef enum dnx_dram_bist_data_mode_e
{
    BIST_DATA_MODE_PRBS = 0,
    BIST_DATA_MODE_PATTERN = 1
} dnx_dram_bist_data_mode_t;

typedef enum dnx_dram_tune_loopback_mode_e
{
    LOOPBACK_MODE_NONE = 0,
    LOOPBACK_MODE_READ_LFSR = 1,
    LOOPBACK_MODE_WRITE_LFSR = 2
} dnx_dram_tune_loopback_mode_t;

/*************
 * DEFINES   *
 *************/
#define BITS_IN_BYTE 8
#define BYTES_IN_WORD 4
#define CHAR_PER_WORD_IN_STRING 11
#define DIAG_DNX_DARM_BUFFER_ARRAY_MAX_SIZE 1000

/** see man section */
static shr_error_e
sh_dnx_dram_bist_start_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    hbmc_shmoo_bist_info_t bist_info;

    uint32 dram_start;
    uint32 dram_end;
    uint32 channel_start;
    uint32 channel_end;
    uint8 refresh_enable = 0;
    uint8 ignore_address = 0;
    uint32 *nof_commands;
    uint32 read_weight;
    uint32 write_weight;
    uint32 same_row_commands;
    uint32 bank_start;
    uint32 bank_end;
    uint32 column_start;
    uint32 column_end;
    uint32 row_start;
    uint32 row_end;
    uint32 data_mode;
    uint32 *pattern_0;
    uint32 *pattern_1;
    uint32 *pattern_2;
    uint32 *pattern_3;
    uint32 *pattern_4;
    uint32 *pattern_5;
    uint32 *pattern_6;
    uint32 *pattern_7;
    uint32 *prbs_data_seed;

    SHR_FUNC_INIT_VARS(unit);
    SH_SAND_GET_UINT32_RANGE("dram", dram_start, dram_end);
    SH_SAND_GET_UINT32_RANGE("channel", channel_start, channel_end);
    SH_SAND_GET_ARRAY32("count", nof_commands);
    SH_SAND_GET_UINT32("read_weight", read_weight);
    SH_SAND_GET_UINT32("write_weight", write_weight);
    SH_SAND_GET_UINT32("same_row_count", same_row_commands);
    SH_SAND_GET_UINT32_RANGE("bank", bank_start, bank_end);
    SH_SAND_GET_UINT32_RANGE("column", column_start, column_end);
    SH_SAND_GET_UINT32_RANGE("row", row_start, row_end);
    SH_SAND_GET_ENUM("mode", data_mode);
    SH_SAND_GET_ARRAY32("pattern_0", pattern_0);
    SH_SAND_GET_ARRAY32("pattern_1", pattern_1);
    SH_SAND_GET_ARRAY32("pattern_2", pattern_2);
    SH_SAND_GET_ARRAY32("pattern_3", pattern_3);
    SH_SAND_GET_ARRAY32("pattern_4", pattern_4);
    SH_SAND_GET_ARRAY32("pattern_5", pattern_5);
    SH_SAND_GET_ARRAY32("pattern_6", pattern_6);
    SH_SAND_GET_ARRAY32("pattern_7", pattern_7);
    SH_SAND_GET_ARRAY32("seed", prbs_data_seed);

    /** set params to relevant struct after input verification was done */
    bist_info.write_weight = write_weight;
    bist_info.read_weight = read_weight;
    bist_info.bist_timer_us = 0;
    sal_memcpy(bist_info.bist_num_actions, nof_commands, sizeof(bist_info.bist_num_actions));
    bist_info.row_start_index = row_start;
    bist_info.column_start_index = column_start;
    bist_info.bank_start_index = bank_start;
    bist_info.row_end_index = row_end;
    bist_info.column_end_index = column_end;
    bist_info.bank_end_index = bank_end;
    bist_info.bist_refresh_enable = refresh_enable;
    bist_info.bist_ignore_address = ignore_address;
    bist_info.same_row_commands = same_row_commands;
    bist_info.data_mode = data_mode;
    sal_memcpy(&(bist_info.data_pattern[0][0]), pattern_0, sizeof(bist_info.data_pattern[0]));
    sal_memcpy(&(bist_info.data_pattern[1][0]), pattern_1, sizeof(bist_info.data_pattern[0]));
    sal_memcpy(&(bist_info.data_pattern[2][0]), pattern_2, sizeof(bist_info.data_pattern[0]));
    sal_memcpy(&(bist_info.data_pattern[3][0]), pattern_3, sizeof(bist_info.data_pattern[0]));
    sal_memcpy(&(bist_info.data_pattern[4][0]), pattern_4, sizeof(bist_info.data_pattern[0]));
    sal_memcpy(&(bist_info.data_pattern[5][0]), pattern_5, sizeof(bist_info.data_pattern[0]));
    sal_memcpy(&(bist_info.data_pattern[6][0]), pattern_6, sizeof(bist_info.data_pattern[0]));
    sal_memcpy(&(bist_info.data_pattern[7][0]), pattern_7, sizeof(bist_info.data_pattern[0]));

    if (!SAL_BOOT_PLISIM)
    {
        /** first start run on all channels */
        for (uint32 dram_index = dram_start; dram_index <= dram_end; ++dram_index)
        {
            for (uint32 channel = channel_start; channel <= channel_end; ++channel)
            {
                if (prbs_data_seed[0] == 0)
                {
                    /** if random option was used, need to generate a random seed for PRBS */
                    for (int seed_i = 0; seed_i < HBMC_SHMOO_BIST_NOF_PRBS_DATA_SEEDS; ++seed_i)
                    {
                        bist_info.prbs_seeds[seed_i] = sal_rand();
                    }
                }
                else
                {
                    /** else use the seed the user provided */
                    sal_memcpy(bist_info.prbs_seeds, prbs_data_seed, sizeof(bist_info.prbs_seeds));
                }

                /** configure and run BIST */
                SHR_IF_ERR_EXIT(dnx_hbmc_bist_configuration_set(unit, dram_index, channel, &bist_info));
                SHR_IF_ERR_EXIT(dnx_hbmc_bist_start(unit, dram_index, channel));
            }
        }

        /** if not infinite run, poll that bist was done */
        if ((nof_commands[0] != 0) || (nof_commands[1] != 0))
        {
            for (uint32 dram_index = dram_start; dram_index <= dram_end; ++dram_index)
            {
                for (uint32 channel = channel_start; channel <= channel_end; ++channel)
                {
                    /** poll that bist was done */
                    SHR_IF_ERR_EXIT(dnx_hbmc_bist_run_done_poll(unit, dram_index, channel));
                }
            }
        }
    }
    else
    {
        LOG_CLI((BSL_META("Cannot run BIST on simulated system(pcid, cmodel etc')\n")));
    }

exit:
    SHR_FUNC_EXIT;
}

/** see man section */
static shr_error_e
sh_dnx_dram_bist_stop_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 dram_start;
    uint32 dram_end;
    uint32 channel_start;
    uint32 channel_end;

    SHR_FUNC_INIT_VARS(unit);
    SH_SAND_GET_UINT32_RANGE("dram", dram_start, dram_end);
    SH_SAND_GET_UINT32_RANGE("channel", channel_start, channel_end);

    for (uint32 dram_index = dram_start; dram_index <= dram_end; ++dram_index)
    {
        for (uint32 channel = channel_start; channel <= channel_end; ++channel)
        {
            SHR_IF_ERR_EXIT(dnx_hbmc_bist_stop(unit, dram_index, channel));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/** see man section */
static shr_error_e
sh_dnx_dram_bist_print_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 dram_start;
    uint32 dram_end;
    uint32 channel_start;
    uint32 channel_end;
    char data_err_bits_str_buffer[HBMC_SHMOO_BIST_DATA_ERR_BITS];

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    SH_SAND_GET_UINT32_RANGE("dram", dram_start, dram_end);
    SH_SAND_GET_UINT32_RANGE("channel", channel_start, channel_end);

    /** print error counters */
    /** title */
    PRT_TITLE_SET("Dram BIST results");
    /** columns */
    PRT_COLUMN_ADD("Dram index");
    PRT_COLUMN_ADD("Channel");
    PRT_COLUMN_ADD("Write command counter");
    PRT_COLUMN_ADD("Read command counter");
    PRT_COLUMN_ADD("Read data counter");
    PRT_COLUMN_ADD("Data error counter");
    PRT_COLUMN_ADD("Data error bits");

    for (uint32 dram_index = dram_start; dram_index <= dram_end; ++dram_index)
    {
        for (uint32 channel = channel_start; channel <= channel_end; ++channel)
        {
            hbmc_shmoo_bist_err_cnt_t error_counters;
            uint32 write_cmd_counter[HBMC_SHMOO_BIST_NOF_ACTIONS_WORDS] = { 0 };
            uint32 read_cmd_counter[HBMC_SHMOO_BIST_NOF_ACTIONS_WORDS] = { 0 };
            uint32 read_data_counter[HBMC_SHMOO_BIST_NOF_ACTIONS_WORDS] = { 0 };

            /** get error counters */
            SHR_IF_ERR_EXIT(dnx_hbmc_bist_err_cnt_cb(unit, 0, dram_index, channel, &error_counters));

            /** get bist status */
            SHR_IF_ERR_EXIT(dnx_hbmc_bist_status_get
                            (unit, dram_index, channel, read_cmd_counter, write_cmd_counter, read_data_counter));

            /** info */
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET("%d", dram_index);
            PRT_CELL_SET("%d", channel);
            PRT_CELL_SET("0x%08x%08x", write_cmd_counter[1], write_cmd_counter[0]);
            PRT_CELL_SET("0x%08x%08x", read_cmd_counter[1], read_cmd_counter[0]);
            PRT_CELL_SET("0x%08x%08x", read_data_counter[1], read_data_counter[0]);
            PRT_CELL_SET("0x%08x", error_counters.bist_data_err_cnt);
            shr_bitop_str_encode(error_counters.bist_data_err_bits, HBMC_SHMOO_BIST_DATA_ERR_BITS,
                                 data_err_bits_str_buffer, HBMC_SHMOO_BIST_DATA_ERR_BITS);
            PRT_CELL_SET("%s", data_err_bits_str_buffer);
        }
    }
    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/** see man section */
static shr_error_e
sh_dnx_dram_buffer_status_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 buffers_array[DIAG_DNX_DARM_BUFFER_ARRAY_MAX_SIZE];
    uint32 nof_cores = dnx_data_device.general.nof_cores_get(unit);
    uint8 deleted;
    int size;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_BOOL("delete", deleted);

    if (deleted)
    {
        PRT_TITLE_SET("Deleted Dram buffer");
    }
    else
    {
        PRT_TITLE_SET("Quarantined Dram buffer");
    }
    PRT_COLUMN_ADD("Buffer");
    PRT_COLUMN_ADD("Core");

    /** iterate on cores */
    for (int core = 0; core < nof_cores; ++core)
    {
        /** clear array */
        sal_memset(buffers_array, 0, sizeof(buffers_array));
        /** get status for deleted or quarantined buffers according to input */
        SHR_IF_ERR_EXIT(dnx_dram_buffers_status_get
                        (unit, core, deleted, buffers_array, DIAG_DNX_DARM_BUFFER_ARRAY_MAX_SIZE, &size));

        /** print overflow warning if buffers_array was too small */
        if (size > DIAG_DNX_DARM_BUFFER_ARRAY_MAX_SIZE)
        {
            /** print overflow warning */
            LOG_CLI((BSL_META("to many buffers to present on core %d, presenting %d out of %d that were found\n"),
                     core, DIAG_DNX_DARM_BUFFER_ARRAY_MAX_SIZE, size));
            /** use MAX_SIZE instead of size when iterating array */
            size = DIAG_DNX_DARM_BUFFER_ARRAY_MAX_SIZE;
        }

        /** print buffers */
        for (int index = 0; index < size; ++index)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET("%d", buffers_array[index]);
            PRT_CELL_SET("%d", core);
        }
    }
    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/** see man section */
static shr_error_e
sh_dnx_dram_phy_reg_read_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 dram_index;
    int channel;
    uint32 address;
    uint32 data;
    uint8 midstack;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    SH_SAND_GET_UINT32("dram", dram_index);
    SH_SAND_GET_UINT32("channel", channel);
    SH_SAND_GET_UINT32("address", address);
    SH_SAND_GET_BOOL("midstack", midstack);

    /** if midstack channel is -1 */
    if (midstack)
    {
        channel = -1;
    }

    /** read phy register */
    SHR_IF_ERR_EXIT(dnx_hbmc_phy_reg_read_cb(unit, dram_index, channel, address, &data));

    /** print phy register */
    /** title */
    PRT_TITLE_SET("Dram PHY register");
    /** columns */
    PRT_COLUMN_ADD("Dram index");
    PRT_COLUMN_ADD("Channel");
    PRT_COLUMN_ADD("Address");
    PRT_COLUMN_ADD("Value");
    /** info */
    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("%d", dram_index);
    PRT_CELL_SET("%d", channel);
    PRT_CELL_SET("0x%x", address);
    PRT_CELL_SET("0x%x", data);
    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/** see man section */
static shr_error_e
sh_dnx_dram_phy_reg_write_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 dram_index;
    int channel;
    uint32 address;
    uint32 data;
    uint8 midstack;

    SHR_FUNC_INIT_VARS(unit);
    SH_SAND_GET_UINT32("dram", dram_index);
    SH_SAND_GET_UINT32("channel", channel);
    SH_SAND_GET_UINT32("address", address);
    SH_SAND_GET_UINT32("value", data);
    SH_SAND_GET_BOOL("midstack", midstack);

    /** if midstack channel is -1 */
    if (midstack)
    {
        channel = -1;
    }

    /** write phy register */
    SHR_IF_ERR_EXIT(dnx_hbmc_phy_reg_write_cb(unit, dram_index, channel, address, data));

exit:
    SHR_FUNC_EXIT;
}

/** see man section */
static shr_error_e
sh_dnx_dram_ind_acc_read_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int mode;
    uint32 logical_buffer;
    uint32 logical_index_in_buffer;
    uint32 *data = NULL;
    uint32 dram;
    uint32 channel;
    uint32 bank;
    uint32 row;
    uint32 column;

    int alloc_size = 0;
    int bit_count;
    char *dest_str = NULL;
    int str_length;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    SH_SAND_GET_ENUM("mode", mode);

    SH_SAND_GET_UINT32("dram", dram);
    SH_SAND_GET_UINT32("channel", channel);
    SH_SAND_GET_UINT32("bank", bank);
    SH_SAND_GET_UINT32("row", row);
    SH_SAND_GET_UINT32("column", column);

    SH_SAND_GET_UINT32("buffer", logical_buffer);
    SH_SAND_GET_INT32("index", logical_index_in_buffer);

    switch (mode)
    {
        case LOGICAL:
        {
            if (logical_index_in_buffer == ALL_INDEXES)
            {
                alloc_size = dnx_data_dram.general_info.buffer_size_get(unit);
                data = sal_alloc(alloc_size, "data");
            }
            else
            {
                alloc_size = dnx_data_dram.address_translation.physical_address_transaction_size_get(unit);
                data = sal_alloc(alloc_size, "data");
            }
            SHR_IF_ERR_EXIT(dnx_hbmc_ind_access_logical_buffer_read
                            (unit, logical_buffer, logical_index_in_buffer, data));
            break;
        }
        case PHYSICAL:
        {
            alloc_size = dnx_data_dram.address_translation.physical_address_transaction_size_get(unit);
            data = sal_alloc(alloc_size, "data");
            SHR_IF_ERR_EXIT(dnx_hbmc_ind_access_physical_address_read(unit, dram, channel, bank, row, column, data));
            break;
        }
    }

    /** print read buffer */
    /** title */
    PRT_TITLE_SET("Dram buffer");
    /** columns */
    PRT_COLUMN_ADD("Buffer Type");
    if (mode == LOGICAL)
    {
        PRT_COLUMN_ADD("Buffer");
        PRT_COLUMN_ADD("Index");
    }
    else if (mode == PHYSICAL)
    {
        PRT_COLUMN_ADD("Dram");
        PRT_COLUMN_ADD("Channel");
        PRT_COLUMN_ADD("Bank");
        PRT_COLUMN_ADD("Row");
        PRT_COLUMN_ADD("Column");
    }
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Value");

    /** info */
    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("%d", mode);
    if (mode == LOGICAL)
    {
        PRT_CELL_SET("%d", logical_buffer);
        PRT_CELL_SET("%d", logical_index_in_buffer);
    }
    else if (mode == PHYSICAL)
    {
        PRT_CELL_SET("0x%x", dram);
        PRT_CELL_SET("0x%x", channel);
        PRT_CELL_SET("0x%x", bank);
        PRT_CELL_SET("0x%x", row);
        PRT_CELL_SET("0x%x", column);
    }
    bit_count = alloc_size * BITS_IN_BYTE;
    str_length = ((bit_count + SHR_BITWID) / SHR_BITWID) * CHAR_PER_WORD_IN_STRING + 1;
    dest_str = sal_alloc(str_length, "dest_str");
    shr_bitop_str_encode(data, bit_count, dest_str, str_length);
    PRT_CELL_SET("%s", dest_str);
    PRT_COMMITX;

exit:
    PRT_FREE;
    sal_free(data);
    sal_free(dest_str);
    SHR_FUNC_EXIT;
}

/** see man section */
static shr_error_e
sh_dnx_dram_ind_acc_write_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int mode;

    uint32 logical_buffer;
    uint32 logical_index_in_buffer;
    uint32 *pattern;
    uint32 dram;
    uint32 channel;
    uint32 bank;
    uint32 row;
    uint32 column;

    SHR_FUNC_INIT_VARS(unit);
    SH_SAND_GET_ENUM("mode", mode);

    SH_SAND_GET_UINT32("dram", dram);
    SH_SAND_GET_UINT32("channel", channel);
    SH_SAND_GET_UINT32("bank", bank);
    SH_SAND_GET_UINT32("row", row);
    SH_SAND_GET_UINT32("column", column);

    SH_SAND_GET_UINT32("buffer", logical_buffer);
    SH_SAND_GET_INT32("index", logical_index_in_buffer);

    SH_SAND_GET_ARRAY32("pattern", pattern);

    switch (mode)
    {
        case LOGICAL:
        {
            SHR_IF_ERR_EXIT(dnx_hbmc_ind_access_logical_buffer_write
                            (unit, logical_buffer, logical_index_in_buffer, pattern));
            break;
        }
        case PHYSICAL:
        {
            SHR_IF_ERR_EXIT(dnx_hbmc_ind_access_physical_address_write
                            (unit, dram, channel, bank, row, column, pattern));
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/** see man section */
static shr_error_e
sh_dnx_dram_phy_reset(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 dram_start;
    uint32 dram_end;
    uint32 channel_start;
    uint32 channel_end;

    SHR_FUNC_INIT_VARS(unit);
    SH_SAND_GET_UINT32_RANGE("dram", dram_start, dram_end);
    SH_SAND_GET_UINT32_RANGE("channel", channel_start, channel_end);

    if (!SAL_BOOT_PLISIM)
    {
        for (uint32 dram_index = dram_start; dram_index <= dram_end; ++dram_index)
        {
            for (uint32 channel = channel_start; channel <= channel_end; ++channel)
            {
                SHR_IF_ERR_EXIT(_hbm16_phy_channel_reset(unit, dram_index, channel));
            }
        }
    }
    else
    {
        LOG_CLI((BSL_META("Cannot run DRAM PHY RESET on simulated system(pcid, cmodel etc')\n")));
    }

exit:
    SHR_FUNC_EXIT;
}



/** see man section */
static shr_error_e
sh_dnx_dram_phy_tune(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 dram_start;
    uint32 dram_end;
    uint32 channel_start;
    uint32 channel_end;
    uint32 shmoo_type;
    uint8 use_last_dram_bist;
    uint8 do_channel_soft_init_after_each_bist;
    uint8 plot;
    uint8 extended_vref_range;
    uint32 action;
    uint32 flags = 0;

    SHR_FUNC_INIT_VARS(unit);
    SH_SAND_GET_UINT32_RANGE("dram", dram_start, dram_end);
    SH_SAND_GET_UINT32_RANGE("channel", channel_start, channel_end);
    SH_SAND_GET_ENUM("shmoo_type", shmoo_type);
    SH_SAND_GET_BOOL("plot", plot);
    SH_SAND_GET_BOOL("extended_vref_range", extended_vref_range);
    SH_SAND_GET_BOOL("last", use_last_dram_bist);
    SH_SAND_GET_BOOL("reset", do_channel_soft_init_after_each_bist);
    SH_SAND_GET_ENUM("action", action);

    /** Set Flags */
    if (plot)
    {
        flags |= SHMOO_HBM16_CTL_FLAGS_PLOT_BIT;
    }
    if (extended_vref_range)
    {
        flags |= SHMOO_HBM16_CTL_FLAGS_EXT_VREF_RANGE_BIT;
    }
    if (use_last_dram_bist)
    {
        /** configure tuning to use last dram bist run */
        SHR_IF_ERR_EXIT(dnx_dram_db.override_bist_configurations_during_tuning.set(unit, 0));
    }
    if (do_channel_soft_init_after_each_bist)
    {
        /** configure tuning to do soft reset after each dram bist in sequence */
        /** when working with this option - certain shmoo types can run only once per interface, afterwards they are failing */
        SHR_IF_ERR_EXIT(dnx_dram_db.channel_soft_init_after_bist.set(unit, 1));
    }
    /** always run on all dwords in regular mode */
    flags |= SHMOO_HBM16_CTL_FLAGS_DWORDS_ALL;

    if (!SAL_BOOT_PLISIM)
    {
        for (uint32 dram_index = dram_start; dram_index <= dram_end; ++dram_index)
        {
            for (uint32 channel = channel_start; channel <= channel_end; ++channel)
            {
                /** run tune */
                SHR_IF_ERR_EXIT(dnx_hbmc_run_phy_tune(unit, dram_index, channel, shmoo_type, flags, action));
            }
        }

        /** restore tune configuration to default */
        if (use_last_dram_bist)
        {
            SHR_IF_ERR_EXIT(dnx_dram_db.override_bist_configurations_during_tuning.set(unit, 1));
        }
        /** restore tune configuration to default */
        if (do_channel_soft_init_after_each_bist)
        {
            /** configure tuning to do soft reset after each dram bist in sequence */
            SHR_IF_ERR_EXIT(dnx_dram_db.channel_soft_init_after_bist.set(unit, 0));
        }
    }
    else
    {
        LOG_CLI((BSL_META("Cannot run DRAM PHY tune on simulated system(pcid, cmodel etc')\n")));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - compare register to its reset value - print if different into prt_ctr
 *
 * \param [in] unit - unit number
 * \param [in] reg - register to analyze
 * \param [in] prt_ctr - print control pointer
 * \param [out] different_from_reset_val - indication if value is different than reset value
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
sh_dnx_dram_register_compare_to_reset(
    int unit,
    soc_reg_t reg,
    prt_control_t * prt_ctr,
    uint32 *different_from_reset_val)
{
    int nof_arr_indexes;
    int field_match_list[MAX_FIELDS_NUM];
    soc_reg_info_t *reg_info;

    SHR_FUNC_INIT_VARS(unit);

    /** set different from reset val to false */
    *different_from_reset_val = FALSE;

    /** set field_match_list to print all fields */
    sal_memset(field_match_list, 1, sizeof(field_match_list));

    /**  get reg info */
    reg_info = &SOC_REG_INFO(unit, reg);
    /** check how many indexes exists if target is array register */
    nof_arr_indexes = reg_info->numels;
    if (nof_arr_indexes <= 0)
    {
        /** Simulate index 0 for non arrayed registers */
        nof_arr_indexes = 1;
    }

    /** iterate over all blocks */
    for (int block_index = 0; SOC_BLOCK_INFO(unit, block_index).type >= 0; ++block_index)
    {
        /** If block type doesn't match skip this block */
        if (!SOC_BLOCK_IS_TYPE(unit, block_index, reg_info->block))
        {
            continue;
        }

        /** read and print registers if not reset value */
        for (int arr_index = 0; arr_index < nof_arr_indexes; ++arr_index)
        {
            uint32 addr;
            soc_reg_above_64_val_t current_value;
            soc_reg_above_64_val_t reset_value;
            /** read register */
            SHR_IF_ERR_EXIT(access_reg_read(unit, reg, block_index, 0, arr_index, current_value, &addr));
            /** get reset val */
            SOC_REG_ABOVE_64_CLEAR(reset_value);
            SOC_REG_ABOVE_64_RST_VAL_GET(unit, reg, reset_value);
            /** if value is the same as reset value */
            if (0 != sal_memcmp(current_value, reset_value, sizeof(soc_reg_above_64_val_t)))
            {
                /** print register */
                SHR_IF_ERR_EXIT(access_reg_print(prt_ctr, unit, reg, SOC_BLOCK_NAME(unit, block_index),
                                                 arr_index, addr, current_value, TRUE, FALSE, field_match_list));
                /** trigger value is different from reset value */
                *different_from_reset_val = TRUE;
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - check if all instances of register are the same and print first instance
 * and any instance which is different from it
 *
 * \param [in] unit - unit number
 * \param [in] reg - register to analyze
 * \param [in] prt_ctr - print control pointer
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
sh_dnx_dram_symmetric_register_analyze(
    int unit,
    soc_reg_t reg,
    prt_control_t * prt_ctr)
{
    int nof_arr_indexes;
    int first_found = 0;
    int field_match_list[MAX_FIELDS_NUM];
    soc_reg_info_t *reg_info;
    soc_reg_above_64_val_t *regs_val_array = NULL;

    SHR_FUNC_INIT_VARS(unit);

    /** set field_match_list to print all fields */
    sal_memset(field_match_list, 1, sizeof(field_match_list));

    /**  get reg info */
    reg_info = &SOC_REG_INFO(unit, reg);
    /** check how many indexes exists if target is array register */
    nof_arr_indexes = reg_info->numels;
    if (nof_arr_indexes <= 0)
    {
        /** Simulate index 0 for non arrayed registers */
        nof_arr_indexes = 1;
    }
    /** allocate reg_values according to number of array indexes of register*/
    regs_val_array = sal_alloc(sizeof(soc_reg_above_64_val_t) * nof_arr_indexes, "regs_val_array");
    sal_memset(regs_val_array, 0, sizeof(soc_reg_above_64_val_t) * nof_arr_indexes);

    /** iterate over all blocks */
    for (int block_index = 0; SOC_BLOCK_INFO(unit, block_index).type >= 0; ++block_index)
    {
        /** If block type doesn't match skip this block */
        if (!SOC_BLOCK_IS_TYPE(unit, block_index, reg_info->block))
        {
            continue;
        }

        if (first_found == 0)
        {
            first_found = 1;
            for (int arr_index = 0; arr_index < nof_arr_indexes; ++arr_index)
            {
                uint32 addr;
                /** read register */
                SHR_IF_ERR_EXIT(access_reg_read
                                (unit, reg, block_index, 0, arr_index, regs_val_array[arr_index], &addr));
                /** print register */
                SHR_IF_ERR_EXIT(access_reg_print(prt_ctr, unit, reg, SOC_BLOCK_NAME(unit, block_index),
                                                 arr_index, addr, regs_val_array[arr_index], TRUE, FALSE,
                                                 field_match_list));
            }
        }
        else
        {
            for (int arr_index = 0; arr_index < nof_arr_indexes; ++arr_index)
            {
                soc_reg_above_64_val_t current_value;
                uint32 addr;
                SOC_REG_ABOVE_64_CLEAR(current_value);
                /** read register */
                SHR_IF_ERR_EXIT(access_reg_read(unit, reg, block_index, 0, arr_index, current_value, &addr));
                /** compare to first found */
                if (sal_memcmp(current_value, regs_val_array[arr_index], sizeof(current_value)))
                {
                    /** if not the same exit and print miss-configuration */
                    LOG_CLI((BSL_META("A miss configuration has been detected in a register %s "
                                      "that is supposed to be symmetric, value will be printed in resulted table:\n"),
                             SOC_REG_NAME(unit, reg)));
                    /** print current_value */
                    SHR_IF_ERR_EXIT(access_reg_print(prt_ctr, unit, reg, SOC_BLOCK_NAME(unit, block_index),
                                                     arr_index, addr, current_value, TRUE, FALSE, field_match_list));
                }
            }
        }
    }

exit:
    SHR_FREE(regs_val_array);
    SHR_FUNC_EXIT;
}

/**
 * \brief - print all instances of given register
 *
 * \param [in] unit - unit number
 * \param [in] reg - register to analyze
 * \param [in] prt_ctr - print control pointer
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
sh_dnx_dram_not_symmetric_register_analyze(
    int unit,
    soc_reg_t reg,
    prt_control_t * prt_ctr)
{
    int nof_arr_indexes;
    int field_match_list[MAX_FIELDS_NUM];
    soc_reg_info_t *reg_info;

    SHR_FUNC_INIT_VARS(unit);

    /** set field_match_list to print all fields */
    sal_memset(field_match_list, 1, sizeof(field_match_list));

    /**  get reg info */
    reg_info = &SOC_REG_INFO(unit, reg);
    /** check how many indexes exists if target is array register */
    nof_arr_indexes = reg_info->numels;
    if (nof_arr_indexes <= 0)
    {
        /** Simulate index 0 for non arrayed registers */
        nof_arr_indexes = 1;
    }

    /** iterate over all blocks */
    for (int block_index = 0; SOC_BLOCK_INFO(unit, block_index).type >= 0; ++block_index)
    {
        /** If block type doesn't match skip this block */
        if (!SOC_BLOCK_IS_TYPE(unit, block_index, reg_info->block))
        {
            continue;
        }

        /** read and print registers */
        for (int arr_index = 0; arr_index < nof_arr_indexes; ++arr_index)
        {
            uint32 addr;
            soc_reg_above_64_val_t current_value;
            /** read register */
            SHR_IF_ERR_EXIT(access_reg_read(unit, reg, block_index, 0, arr_index, current_value, &addr));
            /** print register */
            SHR_IF_ERR_EXIT(access_reg_print(prt_ctr, unit, reg, SOC_BLOCK_NAME(unit, block_index),
                                             arr_index, addr, current_value, TRUE, FALSE, field_match_list));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - print reg_val as a histogram according to rows and columns
 *
 * \param [in] unit - unit number
 * \param [in] reg_val - value to write as histogram
 * \param [in] columns - columns in histogram
 * \param [in] rows - rows in histogram
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
sh_dnx_dram_histogram_print(
    int unit,
    soc_reg_above_64_val_t reg_val,
    int columns,
    int rows)
{
    int columns_arr[columns];
    int bit;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(columns_arr, 0, sizeof(columns_arr));

    /** iterate over reg_val bits, if bit is set add it to array */
    SHR_BIT_ITER(reg_val, rows * columns, bit)
    {
        ++columns_arr[bit % columns];
    }

    /** print array using CLI printing */
    for (int row = 0; row < rows; ++row)
    {
        for (int current_column = 0; current_column < columns; ++current_column)
        {
            if (columns_arr[current_column] > row)
            {
                LOG_CLI((BSL_META("*")));
            }
            else
            {
                LOG_CLI((BSL_META(".")));
            }
        }
        LOG_CLI((BSL_META("\n")));
    }

    SHR_FUNC_EXIT;
}

/**
 * \brief - create histograms out of all instances of provided register and print them out
 *
 * \param [in] unit - unit number
 * \param [in] reg - register to analyze
 * \param [in] columns - columns in histogram
 * \param [in] rows - rows in histogram
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
sh_dnx_dram_histogram_analyze(
    int unit,
    soc_reg_t reg,
    int columns,
    int rows)
{
    int nof_arr_indexes;
    int field_match_list[MAX_FIELDS_NUM];
    soc_reg_info_t *reg_info;

    SHR_FUNC_INIT_VARS(unit);

    /** set field_match_list to print all fields */
    sal_memset(field_match_list, 1, sizeof(field_match_list));

    /**  get reg info */
    reg_info = &SOC_REG_INFO(unit, reg);
    /** check how many indexes exists if target is array register */
    nof_arr_indexes = reg_info->numels;
    if (nof_arr_indexes <= 0)
    {
        /** Simulate index 0 for non arrayed registers */
        nof_arr_indexes = 1;
    }

    /** iterate over all blocks */
    for (int block_index = 0; SOC_BLOCK_INFO(unit, block_index).type >= 0; ++block_index)
    {
        /** If block type doesn't match skip this block */
        if (!SOC_BLOCK_IS_TYPE(unit, block_index, reg_info->block))
        {
            continue;
        }

        /** read and print registers */
        for (int arr_index = 0; arr_index < nof_arr_indexes; ++arr_index)
        {
            uint32 addr;
            soc_reg_above_64_val_t current_value;
            /** read register */
            SHR_IF_ERR_EXIT(access_reg_read(unit, reg, block_index, 0, arr_index, current_value, &addr));
            /** print histogram with header */
            LOG_CLI((BSL_META("Histogram for register %s, block %s, index %d:\n"),
                     SOC_REG_NAME(unit, reg), SOC_BLOCK_NAME(unit, block_index), arr_index));
            SHR_IF_ERR_EXIT(sh_dnx_dram_histogram_print(unit, current_value, columns, rows));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - analyze registers that require a special printing way.
 * the analyzing and printing function is picked according to the register's type
 *
 * \param [in] unit - unit number
 * \param [in] reg - register to analyze
 * \param [in] type - register type as provided in dnx_hbmc_diag_registers_type_t
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
sh_dnx_dram_reg_type_analyze(
    int unit,
    soc_reg_t reg,
    dnx_hbmc_diag_registers_type_t type)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (type)
    {
        case DNX_HBMC_DIAG_REG_HISTOGRAM_32_8:
        {
            SHR_IF_ERR_EXIT(sh_dnx_dram_histogram_analyze(unit, reg, 32, 8));
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "unknown type: %d\n", type);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/** see man section */
static shr_error_e
sh_dnx_dram_debug_status_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 rate = 0;
    int key_size;
    uint32 interrupt_coutner;
    PRT_INIT_VARS;

    SHR_FUNC_INIT_VARS(unit);
    SH_SAND_GET_BOOL("rate", rate);

    if (rate)
    {
        dnxcmn_time_t gtimer_time = {.time_units = DNXCMN_TIME_UNIT_SEC,
            .time = 1
        };
        uint32 nof_clock_cycles;
        /** get number of cycles in a second and use it in gtimer.
         * this way the the value presented is in units of Hz */
        SHR_IF_ERR_EXIT(dnxcmn_time_to_clock_cycles_get(unit, &gtimer_time, &nof_clock_cycles));

        SHR_IF_ERR_EXIT(dnx_gtimer_set(unit, nof_clock_cycles, SOC_BLK_HBC, -1));
        SHR_IF_ERR_EXIT(dnx_gtimer_set(unit, nof_clock_cycles, SOC_BLK_TDU, -1));
        SHR_IF_ERR_EXIT(dnx_gtimer_wait(unit, SOC_BLK_HBC, -1));
        SHR_IF_ERR_EXIT(dnx_gtimer_wait(unit, SOC_BLK_TDU, -1));
    }

    /** print channel interrupt registers */
    PRT_TITLE_SET("Channels interrupt registers");
    PRT_COLUMN_ADD("Object");
    PRT_COLUMN_ADD("Block");
    PRT_COLUMN_ADD("Index");
    PRT_COLUMN_ADD("Address");
    PRT_COLUMN_ADD("Value");
    PRT_COLUMN_ADD("Property");
    key_size = dnx_data_dram.hbm.channel_interrupt_regs_info_get(unit)->key_size[0];
    interrupt_coutner = 0;
    for (int reg_i = 0; reg_i < key_size; ++reg_i)
    {
        uint32 interrupt_triggered;
        SHR_IF_ERR_EXIT(sh_dnx_dram_register_compare_to_reset(unit,
                                                              dnx_data_dram.hbm.channel_interrupt_regs_get(unit,
                                                                                                           reg_i)->reg,
                                                              prt_ctr, &interrupt_triggered));
        interrupt_coutner += interrupt_triggered;
    }
    if (interrupt_coutner == 0)
    {
        /** indicate that there are no interrupts triggered*/
        LOG_CLI((BSL_META("No Interrupts were triggered for dram channels\n")));
    }
    PRT_COMMITX;

    /** print channel debug registers */
    PRT_TITLE_SET("Channels debug registers");
    PRT_COLUMN_ADD("Object");
    PRT_COLUMN_ADD("Block");
    PRT_COLUMN_ADD("Index");
    PRT_COLUMN_ADD("Address");
    PRT_COLUMN_ADD("Value");
    PRT_COLUMN_ADD("Property");
    key_size = dnx_data_dram.hbm.channel_debug_regs_info_get(unit)->key_size[0];
    for (int reg_i = 0; reg_i < key_size; ++reg_i)
    {
        uint32 is_changed = 0;
        SHR_IF_ERR_EXIT(sh_dnx_dram_register_compare_to_reset(unit,
                                                              dnx_data_dram.hbm.channel_debug_regs_get(unit,
                                                                                                       reg_i)->reg,
                                                              prt_ctr, &is_changed));
        if (is_changed == 0)
        {
            /** print reassuring message*/
            LOG_CLI((BSL_META("%s\n"), dnx_data_dram.hbm.channel_debug_regs_get(unit, reg_i)->reassuring_str));
        }
    }
    PRT_COMMITX;

    /** print channel counter registers */
    if (rate)
    {
        LOG_CLI((BSL_META("The units of the Values in the following table are 1/sec\n")));
    }
    PRT_TITLE_SET("Channels counter registers");
    PRT_COLUMN_ADD("Object");
    PRT_COLUMN_ADD("Block");
    PRT_COLUMN_ADD("Index");
    PRT_COLUMN_ADD("Address");
    PRT_COLUMN_ADD("Value");
    PRT_COLUMN_ADD("Property");
    key_size = dnx_data_dram.hbm.channel_counter_regs_info_get(unit)->key_size[0];
    for (int reg_i = 0; reg_i < key_size; ++reg_i)
    {
        SHR_IF_ERR_EXIT(sh_dnx_dram_not_symmetric_register_analyze(unit,
                                                                   dnx_data_dram.hbm.channel_counter_regs_get(unit,
                                                                                                              reg_i)->reg,
                                                                   prt_ctr));
    }
    PRT_COMMITX;

    /** print channel additional info */
    LOG_CLI((BSL_META("Channels additional info")));
    key_size = dnx_data_dram.hbm.channel_type_regs_info_get(unit)->key_size[0];
    for (int reg_i = 0; reg_i < key_size; ++reg_i)
    {
        soc_reg_t reg = dnx_data_dram.hbm.channel_type_regs_get(unit, reg_i)->reg;
        dnx_hbmc_diag_registers_type_t type = dnx_data_dram.hbm.channel_type_regs_get(unit, reg_i)->type;
        SHR_IF_ERR_EXIT(sh_dnx_dram_reg_type_analyze(unit, reg, type));
    }

    /** print controller interrupt registers */
    PRT_TITLE_SET("controllers interrupt registers");
    PRT_COLUMN_ADD("Object");
    PRT_COLUMN_ADD("Block");
    PRT_COLUMN_ADD("Index");
    PRT_COLUMN_ADD("Address");
    PRT_COLUMN_ADD("Value");
    PRT_COLUMN_ADD("Property");
    key_size = dnx_data_dram.hbm.controller_interrupt_regs_info_get(unit)->key_size[0];
    interrupt_coutner = 0;
    for (int reg_i = 0; reg_i < key_size; ++reg_i)
    {
        uint32 interrupt_triggered;
        SHR_IF_ERR_EXIT(sh_dnx_dram_register_compare_to_reset(unit,
                                                              dnx_data_dram.hbm.controller_interrupt_regs_get(unit,
                                                                                                              reg_i)->reg,
                                                              prt_ctr, &interrupt_triggered));
        interrupt_coutner += interrupt_triggered;
    }
    if (interrupt_coutner == 0)
    {
        /** indicate that there are no interrupts triggered*/
        LOG_CLI((BSL_META("No Interrupts were triggered for dram controllers\n")));
    }
    PRT_COMMITX;

    /** print controller info */
    PRT_TITLE_SET("controllers info registers");
    PRT_COLUMN_ADD("Object");
    PRT_COLUMN_ADD("Block");
    PRT_COLUMN_ADD("Index");
    PRT_COLUMN_ADD("Address");
    PRT_COLUMN_ADD("Value");
    PRT_COLUMN_ADD("Property");
    key_size = dnx_data_dram.hbm.controller_info_regs_info_get(unit)->key_size[0];
    for (int reg_i = 0; reg_i < key_size; ++reg_i)
    {
        SHR_IF_ERR_EXIT(sh_dnx_dram_not_symmetric_register_analyze(unit,
                                                                   dnx_data_dram.hbm.controller_info_regs_get(unit,
                                                                                                              reg_i)->reg,
                                                                   prt_ctr));
    }
    PRT_COMMITX;

    /** print address translation interrupt registers */
    PRT_TITLE_SET("address translation interrupt registers");
    PRT_COLUMN_ADD("Object");
    PRT_COLUMN_ADD("Block");
    PRT_COLUMN_ADD("Index");
    PRT_COLUMN_ADD("Address");
    PRT_COLUMN_ADD("Value");
    PRT_COLUMN_ADD("Property");
    key_size = dnx_data_dram.address_translation.interrupt_regs_info_get(unit)->key_size[0];
    interrupt_coutner = 0;
    for (int reg_i = 0; reg_i < key_size; ++reg_i)
    {
        uint32 interrupt_triggered;
        SHR_IF_ERR_EXIT(sh_dnx_dram_register_compare_to_reset(unit,
                                                              dnx_data_dram.address_translation.interrupt_regs_get(unit,
                                                                                                                   reg_i)->reg,
                                                              prt_ctr, &interrupt_triggered));
        interrupt_coutner += interrupt_triggered;
    }
    if (interrupt_coutner == 0)
    {
        /** indicate that there are no interrupts triggered*/
        LOG_CLI((BSL_META("No Interrupts were triggered for address translation\n")));
    }
    PRT_COMMITX;

    /** print address translation counter registers */
    if (rate)
    {
        LOG_CLI((BSL_META("The units of the Values in the following table are 1/sec \n")));
    }
    PRT_TITLE_SET("Address translation counter registers");
    PRT_COLUMN_ADD("Object");
    PRT_COLUMN_ADD("Block");
    PRT_COLUMN_ADD("Index");
    PRT_COLUMN_ADD("Address");
    PRT_COLUMN_ADD("Value");
    PRT_COLUMN_ADD("Property");
    key_size = dnx_data_dram.address_translation.counter_regs_info_get(unit)->key_size[0];
    for (int reg_i = 0; reg_i < key_size; ++reg_i)
    {
        SHR_IF_ERR_EXIT(sh_dnx_dram_not_symmetric_register_analyze(unit,
                                                                   dnx_data_dram.
                                                                   address_translation.counter_regs_get(unit,
                                                                                                        reg_i)->reg,
                                                                   prt_ctr));
    }
    PRT_COMMITX;

exit:
    if (rate)
    {
        SHR_IF_ERR_CONT(dnx_gtimer_clear(unit, SOC_BLK_HBC, -1));
        SHR_IF_ERR_CONT(dnx_gtimer_clear(unit, SOC_BLK_TDU, -1));
    }
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/** see man section */
static shr_error_e
sh_dnx_dram_debug_eye_scan_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 dram_start;
    uint32 dram_end;
    uint32 channel_start;
    uint32 channel_end;
    uint32 type;
    uint32 flags = SHMOO_HBM16_CTL_FLAGS_PLOT_BIT | SHMOO_HBM16_CTL_FLAGS_DWORDS_ALL;
    uint8 allow_traffic_to_dram;
    uint32 force_command = 0;

    SHR_FUNC_INIT_VARS(unit);
    SH_SAND_GET_UINT32_RANGE("dram", dram_start, dram_end);
    SH_SAND_GET_UINT32_RANGE("channel", channel_start, channel_end);
    SH_SAND_GET_ENUM("type", type);

    /** make sure that traffic is routed exclusively to OCB, if not print message and do nothing */
    SHR_IF_ERR_EXIT(dnx_hbmc_is_traffic_allowed_into_dram(unit, &allow_traffic_to_dram));
    if ((!force_command) && allow_traffic_to_dram)
    {
        LOG_CLI((BSL_META("currently traffic can be directed to both dram and OCB - "
                          "intrusive diagnostics such as the one you attempt must be preceded by moving the traffic exclusively to OCB\n")));
        SHR_EXIT();
    }

    if (!SAL_BOOT_PLISIM)
    {
        /** run tune */
        for (uint32 dram_index = dram_start; dram_index <= dram_end; ++dram_index)
        {
            for (uint32 channel = channel_start; channel <= channel_end; ++channel)
            {
                SHR_IF_ERR_EXIT(dnx_hbmc_run_phy_tune(unit, dram_index, channel, type, flags, DNX_HBMC_RUN_TUNE));
            }
        }
    }
    else
    {
        LOG_CLI((BSL_META("Cannot run DRAM PHY tune on simulated system(pcid, cmodel etc')\n")));
    }

exit:
    SHR_FUNC_EXIT;
}

/** see man section */
static shr_error_e
sh_dnx_dram_debug_reset_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint8 allow_traffic_to_dram;
    uint32 dram_start;
    uint32 dram_end;
    uint32 force_command = 0;

    SHR_FUNC_INIT_VARS(unit);
    SH_SAND_GET_UINT32_RANGE("dram", dram_start, dram_end);

    /** make sure that traffic is routed exclusively to OCB, if not print message and do nothing */
    SHR_IF_ERR_EXIT(dnx_hbmc_is_traffic_allowed_into_dram(unit, &allow_traffic_to_dram));
    if ((!force_command) && (allow_traffic_to_dram))
    {
        LOG_CLI((BSL_META("currently traffic can be directed to both dram and OCB - "
                          "intrusive diagnostics such as the one you attempt must be preceded by moving the traffic exclusively to OCB\n")));
        SHR_EXIT();
    }

    /** reset drams */
    for (uint32 dram_index = dram_start; dram_index <= dram_end; ++dram_index)
    {
        SHR_IF_ERR_EXIT(dnx_hbmc_hbm_init(unit, dram_index));
    }

exit:
    SHR_FUNC_EXIT;
}

/** see man section */
static shr_error_e
sh_dnx_dram_debug_bist_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    hbmc_shmoo_bist_info_t bist_info;
    hbmc_shmoo_bist_err_cnt_t error_counters;
    uint8 allow_traffic_to_dram;
    uint32 dram_start;
    uint32 dram_end;
    uint32 channel_start;
    uint32 channel_end;
    uint32 type;
    uint32 force_command = 0;

    SHR_FUNC_INIT_VARS(unit);
    SH_SAND_GET_UINT32_RANGE("dram", dram_start, dram_end);
    SH_SAND_GET_UINT32_RANGE("channel", channel_start, channel_end);

    SH_SAND_GET_ENUM("type", type);

    /** check that simulation is not used */
    if (SAL_BOOT_PLISIM)
    {
        LOG_CLI((BSL_META("Cannot run BIST on simulated system(pcid, cmodel etc')\n")));
        SHR_EXIT();
    }

    /** make sure that traffic is routed exclusively to OCB, if not print message and do nothing */
    SHR_IF_ERR_EXIT(dnx_hbmc_is_traffic_allowed_into_dram(unit, &allow_traffic_to_dram));
    if ((!force_command) && allow_traffic_to_dram)
    {
        LOG_CLI((BSL_META("currently traffic can be directed to both dram and OCB - "
                          "intrusive diagnostics such as the one you attempt must be preceded by moving the traffic exclusively to OCB\n")));
        SHR_EXIT();
    }

    /** set params to relevant struct with defaults */
    bist_info.write_weight = 1024;
    bist_info.read_weight = 1024;
    bist_info.bist_timer_us = 0;
    bist_info.bist_num_actions[0] = 0x80000000;
    bist_info.bist_num_actions[1] = 0;
    bist_info.row_start_index = 0;
    bist_info.column_start_index = 0;
    bist_info.bank_start_index = 0;
    bist_info.row_end_index = dnx_data_dram.general_info.dram_info_get(unit)->nof_rows - 1;
    bist_info.column_end_index = dnx_data_dram.general_info.dram_info_get(unit)->nof_columns - 1;
    bist_info.bank_end_index = dnx_data_dram.general_info.dram_info_get(unit)->nof_banks - 1;
    bist_info.same_row_commands = 32;
    bist_info.bist_refresh_enable = 0;
    bist_info.bist_ignore_address = 0;

    /** run Dram bist according to pre-defined types */
    switch (type)
    {
        case PREDEFINED_BIST_IO_STRESS:
        {
            bist_info.data_mode = BIST_DATA_MODE_PRBS;
            sal_srand(sal_time_usecs());
            for (int seed = 0; seed < HBMC_SHMOO_BIST_NOF_PRBS_DATA_SEEDS; ++seed)
            {
                bist_info.prbs_seeds[seed] = sal_rand() % 0x100000;
            }

            /** run BIST */
            for (uint32 dram_index = dram_start; dram_index <= dram_end; ++dram_index)
            {
                for (uint32 channel = channel_start; channel <= channel_end; ++channel)
                {
                    /** clear error counters */
                    SHR_IF_ERR_EXIT(dnx_hbmc_bist_err_cnt_cb(unit, 0, dram_index, channel, &error_counters));

                    /** start BIST run */
                    SHR_IF_ERR_EXIT(dnx_hbmc_bist_configuration_set(unit, dram_index, channel, &bist_info));
                    SHR_IF_ERR_EXIT(dnx_hbmc_bist_start(unit, dram_index, channel));
                }
            }

            /** check that bist was done */
            for (uint32 dram_index = dram_start; dram_index <= dram_end; ++dram_index)
            {
                for (uint32 channel = channel_start; channel <= channel_end; ++channel)
                {

                    /** check that bist was done */
                    SHR_IF_ERR_EXIT(dnx_hbmc_bist_run_done_poll(unit, dram_index, channel));
                }
            }

            break;
        }
        case PREDEFINED_BIST_ALL_ADDRESS:
        {
            for (uint32 dram_index = dram_start; dram_index <= dram_end; ++dram_index)
            {
                for (uint32 channel = channel_start; channel <= channel_end; ++channel)
                {
                    bist_info.data_mode = BIST_DATA_MODE_PATTERN;
                    sal_memset(bist_info.prbs_seeds, 0, sizeof(bist_info.prbs_seeds));
                    /** set alternating pattern */
                    for (int pattern_index = 0; pattern_index < HBMC_SHMOO_BIST_NOF_PATTERNS; ++pattern_index)
                    {
                        int val = (pattern_index % 2) ? 0x55 : 0xaa;
                        sal_memset(&(bist_info.data_pattern[pattern_index][0]), val, sizeof(bist_info.data_pattern[0]));
                    }
                    /** run BIST */
                    SHR_IF_ERR_EXIT(dnx_hbmc_bist_conf_set_cb(unit, 0, dram_index, channel, &bist_info));
                    /** set reversed alternating pattern */
                    for (int pattern_index = 0; pattern_index < HBMC_SHMOO_BIST_NOF_PATTERNS; ++pattern_index)
                    {
                        int val = (pattern_index % 2) ? 0xaa : 0x55;
                        sal_memset(&(bist_info.data_pattern[pattern_index][0]), val, sizeof(bist_info.data_pattern[0]));
                    }
                    /** run BIST */
                    SHR_IF_ERR_EXIT(dnx_hbmc_bist_conf_set_cb(unit, 0, dram_index, channel, &bist_info));
                }
            }
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "unknown type: %d\n", type);
            break;
        }
    }

    for (uint32 dram_index = dram_start; dram_index <= dram_end; ++dram_index)
    {
        for (uint32 channel = channel_start; channel <= channel_end; ++channel)
        {

            /** get error counters */
            SHR_IF_ERR_EXIT(dnx_hbmc_bist_err_cnt_cb(unit, 0, dram_index, channel, &error_counters));
            LOG_INFO(BSL_LOG_MODULE,
                     (BSL_META_U(unit, "Dram: %d | Channel: %d | Number of Dram BIST errors: 0x%08x\n"), dram_index,
                      channel, error_counters.bist_data_err_cnt));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/** see man section */
static shr_error_e
sh_dnx_dram_debug_temp_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 dram_start;
    uint32 dram_end;
    PRT_INIT_VARS;

    SHR_FUNC_INIT_VARS(unit);
    SH_SAND_GET_UINT32_RANGE("dram", dram_start, dram_end);

    PRT_TITLE_SET("Dram Temperature");
    PRT_COLUMN_ADD("Dram");
    PRT_COLUMN_ADD("Temperature");

    for (uint32 dram_index = dram_start; dram_index <= dram_end; ++dram_index)
    {
        bcm_switch_thermo_sensor_t sensor_data;
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);

        /** Read Temperature */
        SHR_IF_ERR_EXIT(bcm_dnx_switch_thermo_sensor_read(unit, bcmSwitchThermoSensorDram, dram_index, &sensor_data));

        /** Print info */
        PRT_CELL_SET("0x%x", dram_index);
        PRT_CELL_SET("%d C ", sensor_data.thermo_sensor_current);
    }
    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/** see man section */
static shr_error_e
sh_dnx_dram_debug_loopback_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 dram_start;
    uint32 dram_end;
    uint32 channel_start;
    uint32 channel_end;
    uint8 allow_traffic_to_dram;
    uint32 force_command = 0;
    hbmc_shmoo_bist_err_cnt_t error_counters;

    SHR_FUNC_INIT_VARS(unit);
    SH_SAND_GET_UINT32_RANGE("dram", dram_start, dram_end);
    SH_SAND_GET_UINT32_RANGE("channel", channel_start, channel_end);

    /** make sure that traffic is routed exclusively to OCB, if not print message and do nothing */
    SHR_IF_ERR_EXIT(dnx_hbmc_is_traffic_allowed_into_dram(unit, &allow_traffic_to_dram));
    if ((!force_command) && allow_traffic_to_dram)
    {
        LOG_CLI((BSL_META("currently traffic can be directed to both dram and OCB - "
                          "intrusive diagnostics such as the one you attempt must be preceded by moving the traffic exclusively to OCB\n")));
        SHR_EXIT();
    }

    for (uint32 dram_index = dram_start; dram_index <= dram_end; ++dram_index)
    {
        for (uint32 channel = channel_start; channel <= channel_end; ++channel)
        {
            /** run read lfsr loopback bist */
            SHR_IF_ERR_EXIT(dnx_hbmc_read_lfsr_loopback_bist_run(unit, dram_index, channel));

            /** get error counters */
            SHR_IF_ERR_EXIT(dnx_hbmc_bist_err_cnt_cb(unit, 0, dram_index, channel, &error_counters));
            LOG_INFO(BSL_LOG_MODULE,
                     (BSL_META_U(unit, "Dram: %d | Channel: %d | Number of Dram BIST errors: 0x%08x\n"), dram_index,
                      channel, error_counters.bist_data_err_cnt));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - print vendor info for given HBM index
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - HBM index
 * \param [in] vendor_info - HBM vendor info to print;
 * \param [out] prt_ctr - print control pointer for the prints;
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
sh_dnx_dram_hbmc_vendor_info_print(
    int unit,
    int dram_index,
    hbm_dram_vendor_info_hbm_t * vendor_info,
    prt_control_t * prt_ctr)
{
    SHR_FUNC_INIT_VARS(unit);
    PRT_COLUMN_ADD("Property");
    PRT_COLUMN_ADD("Value");
    PRT_COLUMN_ADD("Comments");
    PRT_ROW_ADD(PRT_ROW_SEP_NONE);

    PRT_CELL_SET("dram index");
    PRT_CELL_SET("%d ", dram_index);
    PRT_CELL_SET("");
    PRT_ROW_ADD(PRT_ROW_SEP_NONE);

    PRT_CELL_SET("gen2_test");
    PRT_CELL_SET("0x%x ", vendor_info->gen2_test);
    PRT_CELL_SET("gen-2 feature support - see JEDEC");
    PRT_ROW_ADD(PRT_ROW_SEP_NONE);

    PRT_CELL_SET("ecc");
    PRT_CELL_SET("0x%x ", vendor_info->ecc);
    PRT_CELL_SET("support/no support for ecc");
    PRT_ROW_ADD(PRT_ROW_SEP_NONE);

    PRT_CELL_SET("density");
    if (UTILEX_IS_IN_RANGE(vendor_info->density, 1, 6))
    {
        PRT_CELL_SET("0x%x - %d Gb", vendor_info->density, (1 << vendor_info->density));
        PRT_CELL_SET("memory density per channel");
    }
    else
    {
        PRT_CELL_SET("0x%x", vendor_info->density);
        PRT_CELL_SET("reserved value, see JEDEC");
    }
    PRT_ROW_ADD(PRT_ROW_SEP_NONE);

    PRT_CELL_SET("manufacturer_id");
    if (vendor_info->manufacturer_id == 1)
    {
        PRT_CELL_SET("0x%x", vendor_info->manufacturer_id);
        PRT_CELL_SET("Samsung");
    }
    else
    {
        PRT_CELL_SET("0x%x", vendor_info->manufacturer_id);
        PRT_CELL_SET("reserved value, see JEDEC");
    }
    PRT_ROW_ADD(PRT_ROW_SEP_NONE);

    PRT_CELL_SET("manufacturing_location");
    PRT_CELL_SET("%d", vendor_info->manufacturing_location);
    PRT_CELL_SET("vendor specific - see vendor SPEC");
    PRT_ROW_ADD(PRT_ROW_SEP_NONE);

    PRT_CELL_SET("manufacturing_year");
    PRT_CELL_SET("%d ", 2011 + vendor_info->manufacturing_year);
    PRT_CELL_SET("");
    PRT_ROW_ADD(PRT_ROW_SEP_NONE);

    PRT_CELL_SET("manufacturing_week");
    PRT_CELL_SET("%d", vendor_info->manufacturing_week);
    PRT_CELL_SET("");
    PRT_ROW_ADD(PRT_ROW_SEP_NONE);

    PRT_CELL_SET("serial_number");
    PRT_CELL_SET("0x%08x%08x ", vendor_info->serial_number[1], vendor_info->serial_number[0]);
    PRT_CELL_SET("unique ID per device");
    PRT_ROW_ADD(PRT_ROW_SEP_NONE);

    PRT_CELL_SET("addressing_mode");
    PRT_CELL_SET("%d ", vendor_info->addressing_mode);
    PRT_CELL_SET("See JEDEC");
    PRT_ROW_ADD(PRT_ROW_SEP_NONE);

    PRT_CELL_SET("channel_available");
    PRT_CELL_SET("0x%02x ", vendor_info->channel_available);
    PRT_CELL_SET("");
    PRT_ROW_ADD(PRT_ROW_SEP_NONE);

    PRT_CELL_SET("hbm_stack_hight");
    PRT_CELL_SET("%d ", vendor_info->hbm_stack_hight);
    PRT_CELL_SET("See JEDEC");
    PRT_ROW_ADD(PRT_ROW_SEP_NONE);

    PRT_CELL_SET("model_part_number");
    PRT_CELL_SET("%d", vendor_info->model_part_number);
    PRT_CELL_SET("vendor specific - see vendor SPEC");
    PRT_ROW_ADD(PRT_ROW_SEP_NONE);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - print vendor info for given dram index
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [in] vendor_info - vendor info to print;
 * \param [out] prt_ctr - print control pointer for the prints;
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
sh_dnx_dram_vendor_info_print(
    int unit,
    int dram_index,
    dnx_dram_vendor_info_t * vendor_info,
    prt_control_t * prt_ctr)
{
    SHR_FUNC_INIT_VARS(unit);
    /** act according to dram type */
    if (TRUE == dnx_data_dram.hbm.feature_get(unit, dnx_data_dram_hbm_is_supported))
    {
        SHR_IF_ERR_EXIT(sh_dnx_dram_hbmc_vendor_info_print(unit, dram_index, &vendor_info->hbm_info, prt_ctr));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "No known Dram types are supported for this device\n");
    }
exit:
    SHR_FUNC_EXIT;
}

/** see man section */
static shr_error_e
sh_dnx_dram_debug_vendor_info_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 dram_start;
    uint32 dram_end;
    PRT_INIT_VARS;

    SHR_FUNC_INIT_VARS(unit);
    SH_SAND_GET_UINT32_RANGE("dram", dram_start, dram_end);

    for (uint32 dram_index = dram_start; dram_index <= dram_end; ++dram_index)
    {
        dnx_dram_vendor_info_t vendor_info;
        SHR_IF_ERR_EXIT(dnx_dram_vendor_info_get(unit, dram_index, &vendor_info));
        PRT_TITLE_SET("Dram Vendor Info")
            SHR_IF_ERR_EXIT(sh_dnx_dram_vendor_info_print(unit, dram_index, &vendor_info, prt_ctr));
        PRT_COMMITX;
    }

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}


/** see man section */
static shr_error_e
sh_dnx_dram_debug_register_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int key_size;
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /** print channel symmetric configuration */
    PRT_TITLE_SET("Channels symmetric configuration");
    PRT_COLUMN_ADD("Object");
    PRT_COLUMN_ADD("Block");
    PRT_COLUMN_ADD("Index");
    PRT_COLUMN_ADD("Address");
    PRT_COLUMN_ADD("Value");
    PRT_COLUMN_ADD("Property");
    key_size = dnx_data_dram.hbm.channel_symmetric_regs_info_get(unit)->key_size[0];
    for (int reg_i = 0; reg_i < key_size; ++reg_i)
    {
        SHR_IF_ERR_EXIT(sh_dnx_dram_symmetric_register_analyze(unit,
                                                               dnx_data_dram.hbm.channel_symmetric_regs_get(unit,
                                                                                                            reg_i)->reg,
                                                               prt_ctr));
    }
    PRT_COMMITX;

    /** print channel a-symmetric configuration */
    PRT_TITLE_SET("Channels A-symmetric configuration");
    PRT_COLUMN_ADD("Object");
    PRT_COLUMN_ADD("Block");
    PRT_COLUMN_ADD("Index");
    PRT_COLUMN_ADD("Address");
    PRT_COLUMN_ADD("Value");
    PRT_COLUMN_ADD("Property");
    key_size = dnx_data_dram.hbm.channel_not_symmetric_regs_info_get(unit)->key_size[0];
    for (int reg_i = 0; reg_i < key_size; ++reg_i)
    {
        SHR_IF_ERR_EXIT(sh_dnx_dram_not_symmetric_register_analyze(unit,
                                                                   dnx_data_dram.
                                                                   hbm.channel_not_symmetric_regs_get(unit, reg_i)->reg,
                                                                   prt_ctr));
    }
    PRT_COMMITX;

    /** print controller symmetric configuration */
    PRT_TITLE_SET("Controller symmetric configuration");
    PRT_COLUMN_ADD("Object");
    PRT_COLUMN_ADD("Block");
    PRT_COLUMN_ADD("Index");
    PRT_COLUMN_ADD("Address");
    PRT_COLUMN_ADD("Value");
    PRT_COLUMN_ADD("Property");
    key_size = dnx_data_dram.hbm.controller_symmetric_regs_info_get(unit)->key_size[0];
    for (int reg_i = 0; reg_i < key_size; ++reg_i)
    {
        SHR_IF_ERR_EXIT(sh_dnx_dram_symmetric_register_analyze(unit,
                                                               dnx_data_dram.hbm.controller_symmetric_regs_get(unit,
                                                                                                               reg_i)->reg,
                                                               prt_ctr));
    }
    PRT_COMMITX;

    /** print controller a-symmetric configuration */
    PRT_TITLE_SET("Controller A-symmetric configuration");
    PRT_COLUMN_ADD("Object");
    PRT_COLUMN_ADD("Block");
    PRT_COLUMN_ADD("Index");
    PRT_COLUMN_ADD("Address");
    PRT_COLUMN_ADD("Value");
    PRT_COLUMN_ADD("Property");
    key_size = dnx_data_dram.hbm.controller_not_symmetric_regs_info_get(unit)->key_size[0];
    for (int reg_i = 0; reg_i < key_size; ++reg_i)
    {
        SHR_IF_ERR_EXIT(sh_dnx_dram_not_symmetric_register_analyze(unit,
                                                                   dnx_data_dram.
                                                                   hbm.controller_not_symmetric_regs_get(unit,
                                                                                                         reg_i)->reg,
                                                                   prt_ctr));
    }
    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/** see man section */
static shr_error_e
sh_dnx_dram_debug_redirect_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 redirect_to_ocb;
    int enable_option_was_provided;
    uint8 hbm_is_empty;
    uint8 allow_traffic_to_dram;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_IS_PRESENT("enable", enable_option_was_provided);
    if (enable_option_was_provided)
    {
        /** if value was provided for enable option, action should be done according to it */
        SH_SAND_GET_BOOL("enable", redirect_to_ocb);
        SHR_IF_ERR_EXIT(dnx_hbmc_redirect_traffic_to_ocb(unit, redirect_to_ocb, &hbm_is_empty));
        /** if trying to redirect traffic to OCB only and dram was not emptied, print a message */
        if ((!hbm_is_empty) && (redirect_to_ocb))
        {
            LOG_CLI((BSL_META("Didn't manage to empty dram completely.\nintrusive diagnostics could harm traffic.\n")));
        }
        else if (redirect_to_ocb)
        {
            LOG_CLI((BSL_META("traffic redirected to OCB.\n")));
        }
        else
        {
            LOG_CLI((BSL_META("traffic redirected to both dram and OCB.\n")));
        }
    }
    else
    {
        /** if value was not provided for enable option, display current state */
        SHR_IF_ERR_EXIT(dnx_hbmc_is_traffic_allowed_into_dram(unit, &allow_traffic_to_dram));
        if (allow_traffic_to_dram)
        {
            LOG_CLI((BSL_META("currently traffic can be directed to both dram and OCB\n")));
        }
        else
        {
            LOG_CLI((BSL_META("currently traffic is restricted to OCB\n")));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/* *INDENT-OFF* */
/* { */
/**************/
/** Extension */
/**************/
/** { */
static sh_sand_enum_t sand_diag_dram_data_mode_enum[] = {
    /**String    Value                      Description */
    {"prbs",     BIST_DATA_MODE_PRBS,       "PRBS data mode, the data is generated according to the PRBS seed"},
    {"pattern",  BIST_DATA_MODE_PATTERN,    "User defined pattern, should be inserted in relevant fields"},
    {NULL}
};

static sh_sand_enum_t sand_diag_dram_seed_enum[] = {
    /**String    Value      Description */
    {"random",   0,         "PRBS seed is random"},
    {NULL}
};

static sh_sand_enum_t sand_diag_dram_ind_access_mode_enum[] = {
    /**String    Value          Description */
    {"logical",     LOGICAL,    "Logical access mode, access is done according to logical buffer index"},
    {"physical",    PHYSICAL,   "Physical access mode, access is done according to physical address"},
    {NULL}
};

static sh_sand_enum_t sand_diag_dram_ind_access_index_enum[] = {
    /**String       Value           Description */
    {"all",         ALL_INDEXES,    "Access all indexes in Logical buffer"},
    {NULL}
};

static sh_sand_enum_t sand_diag_dram_shmoo_type_enum[] = {
    /**String                       Value                           Description */
    {"all",                         SHMOO_HBM16_SHMOO_RSVP,         "Perfrom all shmoo type one after the other"},
    {"read enable fish",            SHMOO_HBM16_RD_EN_FISH,         "Read enable fish"},
    {"read extended",               SHMOO_HBM16_RD_EXTENDED,        "read extended"},
    {"write extended",              SHMOO_HBM16_WR_EXTENDED,        "write extended"},
    {"address control extended",    SHMOO_HBM16_ADDR_CTRL_EXTENDED, "address control extended"},
    {NULL}
};

static sh_sand_enum_t sand_diag_dram_tune_actions_enum[] = {
    /**String               Value                                           Description */
    {"restore from soc",   DNX_HBMC_RESTORE_TUNE_PARAMETERS_FROM_SOC_PROPERTIES,     "Restore Tune info from soc properties"},
    {"run",                DNX_HBMC_RUN_TUNE,                                        "Run phy tuning"},
    {NULL}
};


static sh_sand_enum_t sand_diag_dram_predefined_bist_enum[] = {
    /**String           Value                           Description*/
    {"IO_stress",       PREDEFINED_BIST_IO_STRESS,      "Bist dedicated to stress the IO interface"},
    {"all_address",     PREDEFINED_BIST_ALL_ADDRESS,    "Bist dedicated to run on all addresses"},
    {NULL}
};

static sh_sand_enum_t sand_diag_dram_enum[] = {
    { /** No need to define value if plugin_str was defined */
     .string = "max",
     .desc = "Number of drams supported",
     .value = 0,
     .plugin_str = "DNX_DATA.dram.hbm.nof_hbms-1"},
    {NULL}
};


static sh_sand_enum_t sand_diag_dram_channel_enum[] = {
    { /** No need to define value if plugin_str was defined */
     .string = "max",
     .desc = "Number of channels supported",
     .value = 0,
     .plugin_str = "DNX_DATA.dram.hbm.nof_channels-1"},
    {NULL}
};

static sh_sand_enum_t sand_diag_dram_bank_enum[] = {
    { /** No need to define value if plugin_str was defined */
     .string = "max",
     .desc = "Number of banks supported",
     .value = 0,
     .plugin_str = "DNX_DATA.dram.general_info.dram_info.nof_banks()-1"},
    {NULL}
};

static sh_sand_enum_t sand_diag_dram_column_enum[] = {
    { /** No need to define value if plugin_str was defined */
     .string = "max",
     .desc = "Number of columns supported",
     .value = 0,
     .plugin_str = "DNX_DATA.dram.general_info.dram_info.nof_columns()-1"},
    {NULL}
};

static sh_sand_enum_t sand_diag_dram_row_enum[] = {
    { /** No need to define value if plugin_str was defined */
     .string = "max",
     .desc = "Number of rows supported",
     .value = 0,
     .plugin_str = "DNX_DATA.dram.general_info.dram_info.nof_rows()-1"},
    {NULL}
};

/** } */

/*********************/
/** commands options */
/*********************/
/** { */
static sh_sand_option_t dnx_bist_start_options_list[] = {
    /* Option name          Option Type             Option Description                                                                  Option Default          Option Extension                        Valid Range */
    {"dram",                SAL_FIELD_TYPE_UINT32,  "dram interface",                                                                   "0-max",                (void*)sand_diag_dram_enum,             "0-max"},
    {"channel",             SAL_FIELD_TYPE_UINT32,  "channel in interface",                                                             "0-max",                (void*)sand_diag_dram_channel_enum,     "0-max"},
    {"count",               SAL_FIELD_TYPE_ARRAY32, "number of bist commands to perform, 0 means infinite run, max is 0xffffffffff",    "0x400",                NULL},
    {"read_weight",         SAL_FIELD_TYPE_UINT32,  "number of consecutive read commands",                                              "0x100",                NULL,                                   "0-0xffff"},
    {"write_weight",        SAL_FIELD_TYPE_UINT32,  "number of consecutive write commands",                                             "0x100",                NULL,                                   "0-0xffff"},
    {"same_row_count",      SAL_FIELD_TYPE_UINT32,  "number of commands to perform on the same row for each bank",                      "0x8",                  NULL,                                   "0-127"},
    {"bank",                SAL_FIELD_TYPE_UINT32,  "banks range to operate on",                                                        "0-max",                (void*)sand_diag_dram_bank_enum,        "0-max"},
    {"column",              SAL_FIELD_TYPE_UINT32,  "column range to operate on",                                                       "0-max",                (void*)sand_diag_dram_column_enum,      "0-max"},
    {"row",                 SAL_FIELD_TYPE_UINT32,  "row range to operate on",                                                          "0-max",                (void*)sand_diag_dram_row_enum,         "0-max"},
    {"mode",                SAL_FIELD_TYPE_ENUM,    "data mode, possible values are prbs and pattern",                                  "prbs",                 (void*)sand_diag_dram_data_mode_enum},
    {"pattern_0",           SAL_FIELD_TYPE_ARRAY32, "pattern_0 for pattern data_mode",                                                  "0x0 0x0 0x0 0x0",      NULL},
    {"pattern_1",           SAL_FIELD_TYPE_ARRAY32, "pattern_1 for pattern data_mode",                                                  "0x0 0x0 0x0 0x0",      NULL},
    {"pattern_2",           SAL_FIELD_TYPE_ARRAY32, "pattern_2 for pattern data_mode",                                                  "0x0 0x0 0x0 0x0",      NULL},
    {"pattern_3",           SAL_FIELD_TYPE_ARRAY32, "pattern_3 for pattern data_mode",                                                  "0x0 0x0 0x0 0x0",      NULL},
    {"pattern_4",           SAL_FIELD_TYPE_ARRAY32, "pattern_4 for pattern data_mode",                                                  "0x0 0x0 0x0 0x0",      NULL},
    {"pattern_5",           SAL_FIELD_TYPE_ARRAY32, "pattern_5 for pattern data_mode",                                                  "0x0 0x0 0x0 0x0",      NULL},
    {"pattern_6",           SAL_FIELD_TYPE_ARRAY32, "pattern_6 for pattern data_mode",                                                  "0x0 0x0 0x0 0x0",      NULL},
    {"pattern_7",           SAL_FIELD_TYPE_ARRAY32, "pattern_7 for pattern data_mode",                                                  "0x0 0x0 0x0 0x0",      NULL},
    {"seed",                SAL_FIELD_TYPE_ARRAY32, "seed for prbs data mode, you can use value \"random\" for random seed",            "random",               (void*)sand_diag_dram_seed_enum},
    {NULL}
};

static sh_sand_option_t dnx_bist_stop_options_list[] = {
    /* Option name      Option Type             Option Description          Option Default      Option Extension                    Valid Range */
    {"dram",            SAL_FIELD_TYPE_UINT32,  "dram interface",           "0-max",            (void*)sand_diag_dram_enum,         "0-max"},
    {"channel",         SAL_FIELD_TYPE_UINT32,  "channel in interface",     "0-max",            (void*)sand_diag_dram_channel_enum, "0-max"},
    {NULL}
};

static sh_sand_option_t dnx_bist_print_options_list[] = {
    /* Option name      Option Type             Option Description          Option Default      Option Extension                    Valid Range */
    {"dram",            SAL_FIELD_TYPE_UINT32,  "dram interface",           "0-max",            (void*)sand_diag_dram_enum,         "0-max"},
    {"channel",         SAL_FIELD_TYPE_UINT32,  "channel in interface",     "0-max",            (void*)sand_diag_dram_channel_enum, "0-max"},
    {NULL}
};

static sh_sand_option_t dnx_buffer_status_options_list[] = {
    /* Option name      Option Type             Option Description          Option Default */
    {"delete",          SAL_FIELD_TYPE_BOOL,    "display deleted buffers",  "0"},
    {NULL}
};

static sh_sand_option_t dnx_phy_reg_read_options_list[] = {
    /* Option name      Option Type             Option Description          Option Default      Option Extension                    Valid Range */
    {"dram",            SAL_FIELD_TYPE_UINT32,  "dram interface",           "0",                (void*)sand_diag_dram_enum,         "0-max"},
    {"channel",         SAL_FIELD_TYPE_UINT32,  "channel in interface",     "0",                (void*)sand_diag_dram_channel_enum, "0-max"},
    {"midstack",        SAL_FIELD_TYPE_BOOL,    "midstack in interface",    "0"},
    {"address",         SAL_FIELD_TYPE_UINT32,  "phy register address",     "0",                NULL,                               "0-0xfff"},
    {NULL}
};

static sh_sand_option_t dnx_phy_reg_write_options_list[] = {
    /* Option name      Option Type             Option Description          Option Default      Option Extension                    Valid Range */
    {"dram",            SAL_FIELD_TYPE_UINT32,  "dram interface",           "0",                (void*)sand_diag_dram_enum,         "0-max"},
    {"channel",         SAL_FIELD_TYPE_UINT32,  "channel in interface",     "0",                (void*)sand_diag_dram_channel_enum, "0-max"},
    {"midstack",        SAL_FIELD_TYPE_BOOL,    "midstack in interface",    "0"},
    {"address",         SAL_FIELD_TYPE_UINT32,  "phy register address",     "0",                NULL,                               "0-0xfff"},
    {"value",           SAL_FIELD_TYPE_UINT32,  "phy register value",       "0",                NULL,                               "0-0xffffffff"},
    {NULL}
};

static sh_sand_option_t dnx_phy_tune_options_list[] = {
    /* Option name          Option Type            Option Description                                                                          Option Default  Option Extension                        Valid Range */
    {"dram",                SAL_FIELD_TYPE_UINT32, "dram interface",                                                                           "0-max",        (void*)sand_diag_dram_enum,             "0-max"},
    {"channel",             SAL_FIELD_TYPE_UINT32, "channel in interface",                                                                     "0-max",        (void*)sand_diag_dram_channel_enum,     "0-max"},
    {"shmoo_type",          SAL_FIELD_TYPE_ENUM,   "tuning step to perform",                                                                   "all",          (void*)sand_diag_dram_shmoo_type_enum},
    {"plot",                SAL_FIELD_TYPE_BOOL,   "plot tuning progress and found timing windows",                                            "0"},
    {"extended_vref_range", SAL_FIELD_TYPE_BOOL,   "run with an extended range for Vref sweep in EXTENDED shmoos",                             "0"},
    {"last",                SAL_FIELD_TYPE_BOOL,   "use Dram BIST that was used last time and don't let tune override it with its own values", "0"},
    {"reset",               SAL_FIELD_TYPE_BOOL,   "perform channel soft reset (soft init) after each bist iteration in the tuning sequence",  "0"},
    {"action",              SAL_FIELD_TYPE_ENUM,   "tuning action to perform",                                                                 "run",          (void*)sand_diag_dram_tune_actions_enum},
    {NULL}
};

static sh_sand_option_t dnx_phy_reset_options_list[] = {
    /* Option name      Option Type             Option Description                                  Option Default      Option Extension                        Valid Range */
    {"dram",            SAL_FIELD_TYPE_UINT32,  "dram interface",                                   "0-max",            (void*)sand_diag_dram_enum,             "0-max"},
    {"channel",         SAL_FIELD_TYPE_UINT32,  "channel in interface",                             "0-max",            (void*)sand_diag_dram_channel_enum,     "0-max"},
    {NULL}
};

static sh_sand_option_t dnx_ind_acc_read_options_list[] = {
    /* Option name      Option Type             Option Description                  Option Default      Option Extension                                Valid Range*/
    {"mode",            SAL_FIELD_TYPE_ENUM,    "logical or physical access mode",  "logical",          (void*)sand_diag_dram_ind_access_mode_enum},
    {"dram",            SAL_FIELD_TYPE_UINT32,  "dram interface",                   "0",                (void*)sand_diag_dram_enum,                     "0-max"},
    {"channel",         SAL_FIELD_TYPE_UINT32,  "channel in interface",             "0",                (void*)sand_diag_dram_channel_enum,             "0-max"},
    {"bank",            SAL_FIELD_TYPE_UINT32,  "banks range to operate on",        "0",                (void*)sand_diag_dram_bank_enum,                "0-max"},
    {"column",          SAL_FIELD_TYPE_UINT32,  "column range to operate on",       "0",                (void*)sand_diag_dram_column_enum,              "0-max"},
    {"row",             SAL_FIELD_TYPE_UINT32,  "row range to operate on",          "0",                (void*)sand_diag_dram_row_enum,                 "0-max"},
    {"buffer",          SAL_FIELD_TYPE_UINT32,  "logical buffer to access",         "0x100",            NULL,                                           "0-0x1fffff"},
    {"index",           SAL_FIELD_TYPE_INT32,   "index in logical buffer",          "all",              (void*)sand_diag_dram_ind_access_index_enum,    "all-127"},
    {NULL}
};

static sh_sand_option_t dnx_ind_acc_write_options_list[] = {
    /* Option name      Option Type             Option Description                  Option Default                      Option Extension                                Valid Range*/
    {"mode",            SAL_FIELD_TYPE_ENUM,    "logical or physical access mode",  "logical",                          (void*)sand_diag_dram_ind_access_mode_enum},
    {"dram",            SAL_FIELD_TYPE_UINT32,  "dram interface",                   "0",                                (void*)sand_diag_dram_enum,                     "0-max"},
    {"channel",         SAL_FIELD_TYPE_UINT32,  "channel in interface",             "0",                                (void*)sand_diag_dram_channel_enum,             "0-max"},
    {"bank",            SAL_FIELD_TYPE_UINT32,  "banks range to operate on",        "0",                                (void*)sand_diag_dram_bank_enum,                "0-max"},
    {"column",          SAL_FIELD_TYPE_UINT32,  "column range to operate on",       "0",                                (void*)sand_diag_dram_column_enum,              "0-max"},
    {"row",             SAL_FIELD_TYPE_UINT32,  "row range to operate on",          "0",                                (void*)sand_diag_dram_row_enum,                 "0-max"},
    {"buffer",          SAL_FIELD_TYPE_UINT32,  "logical buffer to access",         "0x100",                            NULL,                                           "0-0x1fffff"},
    {"index",           SAL_FIELD_TYPE_INT32,   "index in logical buffer",          "all",                              (void*)sand_diag_dram_ind_access_index_enum,    "all-127"},
    {"pattern",         SAL_FIELD_TYPE_ARRAY32, "pattern to write",                 "0x0",                              NULL},
    {NULL}
};

static sh_sand_option_t dnx_debug_redirect_option_list[] = {
    /* Option name      Option Type             Option Description              Option Default  Option Extension */
    {"enable",          SAL_FIELD_TYPE_BOOL,    "enable redirection to OCB",    "0",            NULL},
    {NULL}
};

static sh_sand_option_t dnx_debug_eye_scan_option_list[] = {
    /* Option name      Option Type             Option Description          Option Default      Option Extension                            Valid Range */
    {"dram",            SAL_FIELD_TYPE_UINT32,  "dram interface",           "0-max",            (void*)sand_diag_dram_enum,                 "0-max"},
    {"channel",         SAL_FIELD_TYPE_UINT32,  "channel in interface",     "0-max",            (void*)sand_diag_dram_channel_enum,         "0-max"},
    {"type",            SAL_FIELD_TYPE_ENUM,    "eye scan type to perform", "all",              (void*)sand_diag_dram_shmoo_type_enum},
    {NULL}
};

static sh_sand_option_t dnx_debug_status_option_list[] = {
    /* Option name      Option Type             Option Description                                  Option Default */
    {"rate",            SAL_FIELD_TYPE_BOOL,    "in relevant counters get rate instead of count",   "0"},
    {NULL}
};

static sh_sand_option_t dnx_debug_reset_option_list[] = {
    /* Option name      Option Type             Option Description          Option Default      Option Extension                            Valid Range */
    {"dram",            SAL_FIELD_TYPE_UINT32,  "dram interface",           "0-max",            (void*)sand_diag_dram_enum,                 "0-max"},
    {NULL}
};

static sh_sand_option_t dnx_debug_bist_option_list[] = {
    /* Option name      Option Type             Option Description          Option Default      Option Extension                            Valid Range */
    {"dram",            SAL_FIELD_TYPE_UINT32,  "dram interface",           "0-max",            (void*)sand_diag_dram_enum,                 "0-max"},
    {"channel",         SAL_FIELD_TYPE_UINT32,  "channel in interface",     "0-max",            (void*)sand_diag_dram_channel_enum,         "0-max"},
    {"type",            SAL_FIELD_TYPE_ENUM,    "pre-defined bist types",   "all_address",      (void*)sand_diag_dram_predefined_bist_enum},
    {NULL}
};

static sh_sand_option_t dnx_debug_temp_option_list[] = {
    /* Option name      Option Type             Option Description          Option Default      Option Extension                    Valid Range */
    {"dram",            SAL_FIELD_TYPE_UINT32,  "dram interface",           "0-max",            (void*)sand_diag_dram_enum,         "0-max"},
    {NULL}
};

static sh_sand_option_t dnx_debug_vendor_info_option_list[] = {
    /* Option name      Option Type             Option Description          Option Default      Option Extension                    Valid Range */
    {"dram",            SAL_FIELD_TYPE_UINT32,  "dram interface",           "0-max",            (void*)sand_diag_dram_enum,         "0-max"},
    {NULL}
};

static sh_sand_option_t dnx_debug_loopback_option_list[] = {
    /* Option name      Option Type             Option Description          Option Default      Option Extension                    Valid Range */
    {"dram",            SAL_FIELD_TYPE_UINT32,  "dram interface",           "0-max",            (void*)sand_diag_dram_enum,         "0-max"},
    {"channel",         SAL_FIELD_TYPE_UINT32,  "channel in interface",     "0-max",            (void*)sand_diag_dram_channel_enum, "0-max"},
    {NULL}
};

/** } */

/****************************/
/** commands manual strings */
/****************************/
/** { */
static sh_sand_man_t sh_dnx_dram_bist_man = {
    .brief = "DRAM BIST diagnostics and commands \n",
    .full = "Dram BIST command allows running BIST test on the dram\n"
            "this is used to check the integrity of the dram and to verify that the dram phy tuning was performed properly\n",
};

static sh_sand_man_t sh_dnx_dram_buffer_man = {
    .brief = "DRAM BUFFER diagnostics and commands \n",
    .full = "Dram BUFFER command allows running dram buffers diagnostics\n"
            "this is used to check for previously quarantined and deleted buffers\n",
};

static sh_sand_man_t sh_dnx_dram_phy_man = {
    .brief = "DRAM PHY diagnostics and commands\n",
    .full = "DRAM PHY command allows accessing the dram phy registers and tuning the phys\n",
};

static sh_sand_man_t sh_dnx_dram_ind_access_man = {
    .brief = "DRAM Indirect Access diagnostics and commands\n",
    .full = "DRAM Indirect Access command allows access to the dram memory for deeper inspection\n",
};

static sh_sand_man_t sh_dnx_dram_debug_man = {
    .brief = "DRAM debug tool-box\n",
    .full = "DRAM debug tool-box is a handy interface for debugging the dram and get info in a predefined common comparable way\n",
};

static sh_sand_man_t sh_dnx_dram_bist_start_man = {
    .brief = "start dram bist run \n",
    .full = "start a dram bist run.\n"
            "special options such as infinite run requires running bist stop option to end it"
            "prbs mode will generate a pattern for the bist based on provided seed, "
            "seed needs to be 128 hex digit long for all data bits to be randomized"
            "pattern mode will use pattern provided in pattern_<n>",
#if 1
    .examples = "dram=0 channel=4 count=0x1000 mode=prbs seed=0x3451231231234353245432523412345432598098230495893427589798798732290754123421232545789135548903af4554bd48574747578487438412345678 \n" \
                "dram=1 channel=2-3 read_weight=0x100 write_weight=0x100 same_row_count=0x8 bank=0-max column=0-max row=0-max \n" \
                "dram=0 channel=1 pattern_0=0 pattern_1=1 pattern_2=2 pattern_3=3 pattern_4=4 pattern_5=5 pattern_6=6 pattern_7=7 \n"
#endif
};

static sh_sand_man_t sh_dnx_dram_bist_stop_man = {
    .brief = "stop dram bist run if one is currently underway\n",
    .full = "this should be used to stop an infinite dram bist run\n",
    .examples = "dram=0 channel=2"
};

static sh_sand_man_t sh_dnx_dram_bist_print_man = {
    .brief = "print dram bist error counters\n",
    .full = "print the bist error counters",
    .examples = "dram=0 channel=2"
};

static sh_sand_man_t sh_dnx_dram_buffer_status_man = {
    .brief = "print dram buffers status\n",
    .full = "print status of quarantined or deleted buffers, quarantined buffers include also deleted buffers, buffer that will be deleted next time it is read will also count as deleted",
    .examples = "delete=1"
};

static sh_sand_man_t sh_dnx_dram_phy_reg_read_man = {
    .brief = "read dram phy register\n",
    .full = "access to the dram phy registers is done through this command.\n"
            "it is required to provide the address of the phy register that is wanted and value is attempting to write it"
            "access is possible to the different channels and to midstack as well, when accessing midstack, channel is not relevant\n",
    .examples = "dram=1 channel=3 address=0x35 \n" \
                "dram=1 midstack=1 address=0x1"
};

static sh_sand_man_t sh_dnx_dram_phy_reg_write_man = {
    .brief = "write dram phy register\n",
    .full = "access to the dram phy registers is done through this command.\n"
            "it is required to provide the address of the phy register that is wanted and value is attempting to write it"
            "access is possible to the different channels and to midstack as well, when accessing midstack, channel is not relevant\n",
    .examples = "dram=1 channel=3 address=0x35 value=0x3 \n" \
                "dram=0 midstack=1 address=0x0 value=0x1"
};

static sh_sand_man_t sh_dnx_dram_phy_tune_man = {
    .brief = "perform tuning on dram phy\n",
    .full = "run dram phy tuning.\n"
            "it is not required to rerun tuning if it was already ran during init\n"
            "the phy tuning process calibrates the dram phy to successfully read/write from/to the dram\n"
            "options for shmoo_type are: all, read enable fish, read extended, write extended, address control extended\n"
            "options for action are: restore from file, restore from otp, run\n"
            "when running with reset option, certain shmoo_types can pass only once per interface\n",
#if 1
    .examples = "dram=0 channel=5 shmoo_type=all plot=1 action=run \n" \
                "dram=1 channel=1 extended_vref_range=1 \n" \
                "dram=0 channel=2 last=1 \n" \
                "dram=1 channel=5 reset=1 \n"
#endif
};

static sh_sand_man_t sh_dnx_dram_phy_reset_man = {
    .brief = "perform reset on dram phy\n",
    .full = "run dram phy reset.\n"
            "this phy reset shouldn't be used during traffic, and in general you shouldn't use it unless you know what you're doing\n"
            "the phy reset is an internal process that happens during the phy tuning. this can be used for advanced debugging with the phy tuning\n",
    .examples = "dram=0 channel=0 \n"
};

static sh_sand_man_t sh_dnx_dram_phy_register_man = {
    .brief = "DRAM PHY registers diagnostics and commands \n",
    .full = "DRAM PHY registers command allows access to the dram phy registers\n",
};

static sh_sand_man_t sh_dnx_dram_ind_acc_read_man = {
    .brief = "read dram buffer\n",
    .full = "indirect access to the dram can be done either in logical mode or in physical mode\n"
            "logical mode access requires as input the logical buffer and index in the logical buffer\n"
            "physical mode access requires as input the address to access, provided by dram, channel, bank, column and row\n",
    .examples = "mode=logical buffer=0x100 index=all \n" \
                "mode=physical dram=0 channel=3 bank=2 column=1 row=3"
};

static sh_sand_man_t sh_dnx_dram_ind_acc_write_man = {
    .brief = "write dram buffer\n",
    .full = "indirect access to the dram can be done either in logical mode or in physical mode\n"
            "logical mode access requires as input the logical buffer and index in the logical buffer\n"
            "physical mode access requires as input the address to access, provided by dram, channel, bank, column and row\n",
    .examples = "mode=physical dram=0 channel=3 bank=2 column=1 row=3 pattern=0x1035908091458900934908509 \n" \
                "mode=logical buffer=0x100 index=all pattern=0x1010101010101010101010101010"
};

static sh_sand_man_t sh_dnx_dram_debug_redirect_man = {
    .brief = "redirect all the traffic to the OCB.\n",
    .full = "redirect all traffic to the OCB and back to the dram according to chosen option."
            "this is to allow intrusive examination of the dram without harming traffic dramatically."
            "user is required to explicitly provide enable=<0/1> as input, if not, current state is printed\n",
    .examples = "enable=1"
};

static sh_sand_man_t sh_dnx_dram_debug_register_man = {
    .brief = "print dram configuration related registers",
    .full = "print dram configuration related registers, configuration is mostly symmetric."
            "In case of a-symmetric configuration - differences are printed\n",
    .examples = ""
};

static sh_sand_man_t sh_dnx_dram_debug_eye_scan_man = {
    .brief = "run eye scan",
    .full = "run different types of phy eye scans, AKA shmoo or phy tuning.",
#if 1
    .examples = "dram=0 channel=5 type=all"
#endif
};

static sh_sand_man_t sh_dnx_dram_debug_status_man = {
    .brief = "Get current status of dram",
    .full = "Get current status of dram, this is including counters and status registers. for configurations use different diagnostic.",
    .examples = "rate=0"
};

static sh_sand_man_t sh_dnx_dram_debug_reset_man = {
    .brief = "Reset Dram",
    .full = "Reset specified Dram index.",
#if 1
    .examples = "dram=0"
#endif
};

static sh_sand_man_t sh_dnx_dram_debug_bist_man = {
    .brief = "Run pre-defined BIST types",
    .full = "Run pre-defined Dram BIST sequences. the different available types are:\n"
            "IO_stress - BIST run to stress the IO interface of the dram, all_address - BIST run to validate all address are functioning without errors",
#if 1
    .examples = "dram=0 channel=5 type=all_address"
#endif
};

static sh_sand_man_t sh_dnx_dram_debug_temp_man = {
    .brief = "Check dram temperature",
    .full = "Check dram temperature per dram interface",
    .examples = "dram=0"
};

static sh_sand_man_t sh_dnx_dram_debug_vendor_info_man = {
    .brief = "Check dram vendor info",
    .full = "Check dram vendor info",
    .examples = "dram=0"
};

static sh_sand_man_t sh_dnx_dram_debug_loopback_man = {
    .brief = "Run Dram BIST with read LSFR loopback mode",
    .full = "Run Dram BIST with read LSFR loopback mode",
#if 1
        .examples = "dram=0 channel=0"
#endif
};
/** } */

/*************/
/** commands */
/*************/
/** { */
static sh_sand_cmd_t sh_dnx_dram_bist_cmds[] = {
    /* Name           | Leaf Action                   | Junction Array Pointer        | Options for Leaf              | Usage */
    {"start",         sh_dnx_dram_bist_start_cmd,     NULL,                           dnx_bist_start_options_list,    &sh_dnx_dram_bist_start_man},
    {"stop",          sh_dnx_dram_bist_stop_cmd,      NULL,                           dnx_bist_stop_options_list,     &sh_dnx_dram_bist_stop_man},
    {"print",         sh_dnx_dram_bist_print_cmd,     NULL,                           dnx_bist_print_options_list,    &sh_dnx_dram_bist_print_man},
    {NULL}
};

static sh_sand_cmd_t sh_dnx_dram_buffer_cmds[] = {
    /* Name           | Leaf Action                   | Junction Array Pointer        | Options for Leaf              | Usage */
    {"status",        sh_dnx_dram_buffer_status_cmd,  NULL,                           dnx_buffer_status_options_list, &sh_dnx_dram_buffer_status_man, NULL, NULL, SH_CMD_SKIP_EXEC},
    {NULL}
};

static sh_sand_cmd_t sh_dnx_dram_phy_register_cmds[] = {
    /* Name           | Leaf Action                   | Junction Array Pointer   | Options for Leaf              | Usage */
    {"read",          sh_dnx_dram_phy_reg_read_cmd,   NULL,                      dnx_phy_reg_read_options_list,  &sh_dnx_dram_phy_reg_read_man},
    {"write",         sh_dnx_dram_phy_reg_write_cmd,  NULL,                      dnx_phy_reg_write_options_list, &sh_dnx_dram_phy_reg_write_man},
    {NULL}
};

static sh_sand_cmd_t sh_dnx_dram_phy_cmds[] = {
    /* Name           | Leaf Action               | Junction Array Pointer       | Options for Leaf          | Usage */
    {"tune",          sh_dnx_dram_phy_tune,       NULL,                          dnx_phy_tune_options_list,  &sh_dnx_dram_phy_tune_man},
    {"register",      NULL,                       sh_dnx_dram_phy_register_cmds, NULL,                       &sh_dnx_dram_phy_register_man},
    {"reset",         sh_dnx_dram_phy_reset,      NULL,                          dnx_phy_reset_options_list, &sh_dnx_dram_phy_reset_man},
    {NULL}
};

static sh_sand_cmd_t sh_dnx_dram_ind_access_cmds[] = {
    /* Name           | Leaf Action                   | Junction Array  | Options for Leaf              | Usage */
    {"read",          sh_dnx_dram_ind_acc_read_cmd,   NULL,             dnx_ind_acc_read_options_list,  &sh_dnx_dram_ind_acc_read_man},
    {"write",         sh_dnx_dram_ind_acc_write_cmd,  NULL,             dnx_ind_acc_write_options_list, &sh_dnx_dram_ind_acc_write_man},
    {NULL}
};

static sh_sand_cmd_t sh_dnx_dram_debug_cmds[] = {
    /* Name           | Leaf Action                      | Junction Array | Options for Leaf                 | Usage */
    {"RedirectToOCB", sh_dnx_dram_debug_redirect_cmd,    NULL,            dnx_debug_redirect_option_list,    &sh_dnx_dram_debug_redirect_man},
    {"Register",      sh_dnx_dram_debug_register_cmd,    NULL,            NULL,                              &sh_dnx_dram_debug_register_man},
    {"EyeScan",       sh_dnx_dram_debug_eye_scan_cmd,    NULL,            dnx_debug_eye_scan_option_list,    &sh_dnx_dram_debug_eye_scan_man},
    {"Status",        sh_dnx_dram_debug_status_cmd,      NULL,            dnx_debug_status_option_list,      &sh_dnx_dram_debug_status_man},
    {"bist",          sh_dnx_dram_debug_bist_cmd,        NULL,            dnx_debug_bist_option_list,        &sh_dnx_dram_debug_bist_man},
    {"Temperature",   sh_dnx_dram_debug_temp_cmd,        NULL,            dnx_debug_temp_option_list,        &sh_dnx_dram_debug_temp_man},
    {"reset",         sh_dnx_dram_debug_reset_cmd,       NULL,            dnx_debug_reset_option_list,       &sh_dnx_dram_debug_reset_man},
    {"VendorInfo",    sh_dnx_dram_debug_vendor_info_cmd, NULL,            dnx_debug_vendor_info_option_list, &sh_dnx_dram_debug_vendor_info_man},
    {"Loopback",      sh_dnx_dram_debug_loopback_cmd,    NULL,            dnx_debug_loopback_option_list,    &sh_dnx_dram_debug_loopback_man},
    {NULL}
};
/** } */

/**
 * \brief DNX DRAM diagnostics
 * List of the supported commands, pointer to command function and command usage function. 
 * This is the entry point for DRAM diagnostic commands
 */
sh_sand_cmd_t sh_dnx_dram_cmds[] = {
    /* Name            | Leaf Action  | Junction Array Pointer     | Options for Leaf | Usage */
    {"bist",           NULL,          sh_dnx_dram_bist_cmds,       NULL,              &sh_dnx_dram_bist_man},
    {"buffer",         NULL,          sh_dnx_dram_buffer_cmds,     NULL,              &sh_dnx_dram_buffer_man},
    {"phy",            NULL,          sh_dnx_dram_phy_cmds,        NULL,              &sh_dnx_dram_phy_man},
    {"IndirectAccess", NULL,          sh_dnx_dram_ind_access_cmds, NULL,              &sh_dnx_dram_ind_access_man},
    {"debug",          NULL,          sh_dnx_dram_debug_cmds,      NULL,              &sh_dnx_dram_debug_man},
    {NULL}
};

sh_sand_man_t sh_dnx_dram_man = {
    .brief = "DRAM diagnostics and commands"
};
/* } */
/* *INDENT-ON* */
