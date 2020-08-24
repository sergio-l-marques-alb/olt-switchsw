/*
 * ! \file dnx_sbusdma_desc.c Contains all of the DNX descriptor DMA functions
 */
/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

/*************
 * INCLUDES  *
 *************/
/* { */

#include <shared/bsl.h>
#include <soc/dcmn/error.h>
#include <soc/mem.h>
#include <soc/util.h>
#include <soc/sbusdma.h>
#include <soc/drv.h> /** needed for SOC_MEM_WORDS, SOC_CONTROL..*/
#include <soc/dnx/utils/dnx_sbusdma_desc.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_sbusdma_desc.h>
#include <sal/core/time.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_intr.h>

#include <sal/appl/sal.h>

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DMA

/* } */

/*************
 * DEFINES   *
 *************/
/* { */

/*#define DNX_SBUSDMA_DESC_PRINTS_ENABLED*/

/*
 * KB (1024) / uint32 (4) =  256
 * divide by 2 for each buffer in the double-buffer
 */
#define DNX_SBUSDMA_DESC_KB_TO_UINT32 128

/* } */

/*************
 *  MACROS   *
 *************/
/* { */

/* } */

/*************
 * TYPE DEFS *
 *************/
/* { */

typedef struct
{                               /* h/w desc structure */
    uint32 cntrl;               /* DMA control info */
    uint32 req;                 /* DMA request info (refer h/w spec for details) */
    uint32 count;               /* DMA count */
    uint32 opcode;              /* Schan opcode (refer h/w spec for details) */
    uint32 addr;                /* Schan address */
    uint32 hostaddr;            /* h/w mapped host address */
} soc_sbusdma_desc_t;

/* } */

/*************
 * GLOBALS   *
 *************/
/* { */

STATIC uint32 dnx_sbusdma_desc_enabled[SOC_MAX_NUM_DEVICES] = { 0 };
STATIC uint32 dnx_sbusdma_desc_module_enabled[SOC_MAX_NUM_DEVICES][SBUSDMA_DESC_MODULE_ENABLE_NOF_MODULES] = { {0} };
STATIC uint32 dnx_sbusdma_desc_commit_counter[SOC_MAX_NUM_DEVICES];

/* Common memory double-buffer */
STATIC uint32 *dnx_sbusdma_desc_mem_buff_a[SOC_MAX_NUM_DEVICES];
STATIC uint32 *dnx_sbusdma_desc_mem_buff_b[SOC_MAX_NUM_DEVICES];
STATIC uint32 *dnx_sbusdma_desc_mem_buff_main[SOC_MAX_NUM_DEVICES];
STATIC uint32 dnx_sbusdma_desc_mem_buff_counter[SOC_MAX_NUM_DEVICES];
/* Max holds the maximum counter during commit */
STATIC uint32 dnx_sbusdma_desc_mem_buff_counter_max[SOC_MAX_NUM_DEVICES];
/* Total holds the total counter values during commit, used to calculate the average */
STATIC uint32 dnx_sbusdma_desc_mem_buff_counter_total[SOC_MAX_NUM_DEVICES];

/* Host memory descriptors */
STATIC soc_sbusdma_desc_cfg_t *dnx_sbusdma_desc_cfg_array_a[SOC_MAX_NUM_DEVICES];
STATIC soc_sbusdma_desc_cfg_t *dnx_sbusdma_desc_cfg_array_b[SOC_MAX_NUM_DEVICES];
STATIC soc_sbusdma_desc_cfg_t *dnx_sbusdma_desc_cfg_array_main[SOC_MAX_NUM_DEVICES];
STATIC uint32 dnx_sbusdma_desc_counter[SOC_MAX_NUM_DEVICES];
/* Max holds the maximum counter during commit */
STATIC uint32 dnx_sbusdma_desc_counter_max[SOC_MAX_NUM_DEVICES];
/* Total holds the total counter values during commit, used to calculate the average */
STATIC uint32 dnx_sbusdma_desc_counter_total[SOC_MAX_NUM_DEVICES];

/* Configuration of memory buffer size and max number of descriptors in chain */
STATIC uint32 dnx_sbusdma_desc_cfg_max[SOC_MAX_NUM_DEVICES];
STATIC uint32 dnx_sbusdma_desc_mem_max[SOC_MAX_NUM_DEVICES];
STATIC volatile uint32 dnx_sbusdma_desc_time_out_max[SOC_MAX_NUM_DEVICES] = { 0 };

/* Holds the last descriptor handle, 0 if the last has ended */
STATIC volatile sbusdma_desc_handle_t dnx_sbusdma_desc_handle[SOC_MAX_NUM_DEVICES];

STATIC volatile sal_sem_t dnx_sbusdma_desc_timeout_sem[SOC_MAX_NUM_DEVICES];
STATIC volatile sal_mutex_t dnx_sbusdma_desc_timeout_mutex[SOC_MAX_NUM_DEVICES];
STATIC volatile sal_usecs_t dnx_sbusdma_desc_timeout_add_time[SOC_MAX_NUM_DEVICES];     /* Holds * the * descriptor *
                                                                                         * chain * create * time */
STATIC volatile sal_thread_t dnx_sbusdma_desc_timeout_tid[SOC_MAX_NUM_DEVICES];
STATIC volatile uint8 dnx_sbusdma_desc_timeout_terminate[SOC_MAX_NUM_DEVICES];

/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */

uint32
dnx_sbusdma_desc_is_enabled(
    int unit,
    sbusdma_desc_module_enable_e module_enum)
{
    if (module_enum < SBUSDMA_DESC_MODULE_ENABLE_NOF_MODULES)
    {
        return dnx_sbusdma_desc_module_enabled[unit][module_enum];
    }
    else
    {
        return FALSE;
    }

}

/*This function must be called while holding dnx_sbusdma_desc_timeout_mutex*/
STATIC shr_error_e
dnx_sbusdma_desc_wait_previous_done(
    int unit)
{
    soc_timeout_t to;
    uint8 state = 1;

    SHR_FUNC_INIT_VARS(unit);

    if (dnx_sbusdma_desc_enabled[unit])
    {
        soc_timeout_init(&to, SOC_SBUSDMA_DM_TO(unit) * 2, 0);

        if (dnx_sbusdma_desc_handle[unit] != 0)
        {
            SHR_IF_ERR_EXIT(soc_sbusdma_desc_get_state(unit, dnx_sbusdma_desc_handle[unit], &state));
            /*
             * Wait for the previous request to finish 
             */
            while (state != 0)
            {
                if (soc_timeout_check(&to))
                {
                    SHR_ERR_EXIT(SOC_E_TIMEOUT, "Timeout waiting for descriptor DMA to finish.");
                }
                SHR_IF_ERR_EXIT(soc_sbusdma_desc_get_state(unit, dnx_sbusdma_desc_handle[unit], &state));
            }
        }
    }

exit:
    if (dnx_sbusdma_desc_handle[unit] != 0)
    {
        SHR_IF_ERR_CONT(soc_sbusdma_desc_delete(unit, dnx_sbusdma_desc_handle[unit]));
        dnx_sbusdma_desc_handle[unit] = 0;
    }
    SHR_FUNC_EXIT;
}

void STATIC
dnx_sbusdma_desc_cb(
    int unit,
    int status,
    sbusdma_desc_handle_t handle,
    void *data)
{
#ifdef DNX_SBUSDMA_DESC_PRINTS_ENABLED
    if (status == SOC_E_NONE)
    {
        int rv = SOC_E_NONE;
        int i = 0;
        soc_sbusdma_desc_ctrl_t ctrl = { 0 };
        soc_sbusdma_desc_cfg_t *cfg = NULL;
        soc_mem_t mem;

        LOG_CLI((BSL_META_U(0, "Successfully done DESC DMA, handle: %d\n"), handle));

        cfg = sal_alloc(sizeof(soc_sbusdma_desc_cfg_t) * dnx_sbusdma_desc_cfg_max[unit], "soc_sbusdma_desc_cfg_t");
        if (cfg == NULL)
        {
            LOG_ERROR(BSL_LS_SOC_DMA,
                      (BSL_META_U(unit,
                                  "Error: Fail to allocate memory for SBUSDMA desc_cfg for failure log print.\n")));

        }
        else
        {
            rv = soc_sbusdma_desc_get(unit, handle, &ctrl, cfg);
            if (rv != SOC_E_NONE)
            {
                LOG_CLI((BSL_META_U(0, "%s(), soc_sbusdma_desc_get failed.\n"), FUNCTION_NAME()));
            }

            LOG_ERROR(BSL_LS_SOC_DMA,
                      (BSL_META_U(unit,
                                  "The following memory writes have succeeded (a total of %d):\n"), ctrl.cfg_count));
            for (i = 0; i < ctrl.cfg_count; i++)
            {
                uint8 is_mem = ((cfg[i].flags & SOC_SBUSDMA_MEMORY_CMD_MSG) != 0) ? TRUE : FALSE;

                if (is_mem)
                {
                    /*
                     * If it's a memory attempt to retrieve the memory name
                     */
                    soc_mem_t mem = INVALIDm;
                    mem = soc_addr_to_mem_extended(unit, cfg[i].blk, cfg[i].acc_type, cfg[i].addr);

                    if (mem != INVALIDm)
                    {
                        LOG_ERROR(BSL_LS_SOC_DMA,
                                  (BSL_META_U(unit,
                                              "mem: %d, name = %s, blk: %d, addr: %d, width: %d, count: %d, addr_shift: %d\n"),
                                   mem, SOC_MEM_NAME(unit, mem), cfg[i].blk, cfg[i].addr,
                                   cfg[i].width, cfg[i].count, cfg[i].addr_shift));
                    }
                    else
                    {
                        LOG_ERROR(BSL_LS_SOC_DMA, (BSL_META_U(unit, "mem with addr: %d block: %d not found\n"),
                                                   cfg[i].addr, cfg[i].blk));
                    }
                }
                else
                {
                    /*
                     * If it's a register print the cfg contents
                     * No register equivalent of soc_addr_to_mem_extended
                     */
                    LOG_ERROR(BSL_LS_SOC_DMA,
                              (BSL_META_U(unit,
                                          "reg: blk: %d, addr: %d, width: %d, count: %d, addr_shift: %d\n"),
                               cfg[i].blk, cfg[i].addr, cfg[i].width, cfg[i].count, cfg[i].addr_shift));
                }
            }

            sal_free(cfg);
        }
    }
#endif /* DNX_SBUSDMA_DESC_PRINTS_ENABLED */

    if (status != SOC_E_NONE)
    {
        int rv = SOC_E_NONE;
        int i = 0;
        soc_sbusdma_desc_ctrl_t ctrl = { 0 };
        soc_sbusdma_desc_cfg_t *cfg = NULL;

        LOG_ERROR(BSL_LS_SOC_DMA, (BSL_META_U(unit, "Desc SBUSDMA failed, handle: %d\n"), handle));

        cfg = sal_alloc(sizeof(soc_sbusdma_desc_cfg_t) * dnx_sbusdma_desc_cfg_max[unit], "soc_sbusdma_desc_cfg_t");
        if (cfg == NULL)
        {
            LOG_ERROR(BSL_LS_SOC_DMA,
                      (BSL_META_U(unit,
                                  "Error: Fail to allocate memory for SBUSDMA desc_cfg for failure log print.\n")));

        }
        else
        {
            rv = soc_sbusdma_desc_get(unit, handle, &ctrl, cfg);
            if (rv != SOC_E_NONE)
            {
                LOG_CLI((BSL_META_U(0, "%s(), soc_sbusdma_desc_get failed.\n"), FUNCTION_NAME()));
            }

            LOG_ERROR(BSL_LS_SOC_DMA, (BSL_META_U(unit, "The following memory writes have failed:\n")));
            for (i = 0; i < ctrl.cfg_count; i++)
            {
                uint8 is_mem = ((cfg[i].flags & SOC_SBUSDMA_MEMORY_CMD_MSG) != 0) ? TRUE : FALSE;

                if (is_mem)
                {
                    /*
                     * If it's a memory attempt to retrieve the memory name
                     */
                    soc_mem_t mem = INVALIDm;
                    mem = soc_addr_to_mem_extended(unit, cfg[i].blk, cfg[i].acc_type, cfg[i].addr);

                    if (mem != INVALIDm)
                    {
                        LOG_ERROR(BSL_LS_SOC_DMA,
                                  (BSL_META_U(unit,
                                              "mem: %d, name = %s, blk: %d, addr: %d, width: %d, count: %d, addr_shift: %d\n"),
                                   mem, SOC_MEM_NAME(unit, mem), cfg[i].blk, cfg[i].addr,
                                   cfg[i].width, cfg[i].count, cfg[i].addr_shift));
                    }
                    else
                    {
                        LOG_ERROR(BSL_LS_SOC_DMA, (BSL_META_U(unit, "mem with addr: %d block: %d not found\n"),
                                                   cfg[i].addr, cfg[i].blk));
                    }
                }
                else
                {
                    /*
                     * If it's a register print the cfg contents
                     * No register equivalent of soc_addr_to_mem_extended
                     */
                    LOG_ERROR(BSL_LS_SOC_DMA,
                              (BSL_META_U(unit,
                                          "reg: blk: %d, addr: %d, width: %d, count: %d, addr_shift: %d\n"),
                               cfg[i].blk, cfg[i].addr, cfg[i].width, cfg[i].count, cfg[i].addr_shift));
                }
            }

            sal_free(cfg);
        }
    }
}

STATIC shr_error_e
dnx_sbusdma_desc_commit(
    int unit,
    uint8 safe)
{
    soc_sbusdma_desc_ctrl_t desc_ctrl = { 0 };
    soc_sbusdma_desc_cfg_t *desc_cfg_array;

    SHR_FUNC_INIT_VARS(unit);

    if ((dnx_sbusdma_desc_timeout_mutex[unit] != NULL) && !safe)
    {
        sal_mutex_take(dnx_sbusdma_desc_timeout_mutex[unit], sal_mutex_FOREVER);
    }

    SHR_IF_ERR_EXIT(dnx_sbusdma_desc_wait_previous_done(unit));

    if (dnx_sbusdma_desc_counter[unit] != 0)
    {
        desc_ctrl.flags = SOC_SBUSDMA_CFG_USE_FLAGS | SOC_SBUSDMA_WRITE_CMD_MSG;
        sal_strncpy(desc_ctrl.name, "DESC DMA", sizeof(desc_ctrl.name) - 1);
        desc_ctrl.cfg_count = dnx_sbusdma_desc_counter[unit];
        desc_ctrl.hw_desc = NULL;
        desc_ctrl.buff = NULL;
        desc_ctrl.cb = dnx_sbusdma_desc_cb;
        desc_ctrl.data = NULL;

        /*
         * Switch to the other buffer 
         */
        desc_cfg_array = dnx_sbusdma_desc_cfg_array_main[unit];
        if (dnx_sbusdma_desc_mem_buff_main[unit] == dnx_sbusdma_desc_mem_buff_a[unit])
        {
            dnx_sbusdma_desc_cfg_array_main[unit] = dnx_sbusdma_desc_cfg_array_b[unit];
            dnx_sbusdma_desc_mem_buff_main[unit] = dnx_sbusdma_desc_mem_buff_b[unit];
        }
        else
        {
            dnx_sbusdma_desc_cfg_array_main[unit] = dnx_sbusdma_desc_cfg_array_a[unit];
            dnx_sbusdma_desc_mem_buff_main[unit] = dnx_sbusdma_desc_mem_buff_a[unit];
        }
        /*
         * Update statistics before reseting the counters
         */
        dnx_sbusdma_desc_mem_buff_counter_total[unit] += dnx_sbusdma_desc_mem_buff_counter[unit];
        if (dnx_sbusdma_desc_mem_buff_counter[unit] > dnx_sbusdma_desc_mem_buff_counter_max[unit])
        {
            dnx_sbusdma_desc_mem_buff_counter_max[unit] = dnx_sbusdma_desc_mem_buff_counter[unit];
        }
        dnx_sbusdma_desc_counter_total[unit] += dnx_sbusdma_desc_counter[unit];
        if (dnx_sbusdma_desc_counter[unit] > dnx_sbusdma_desc_counter_max[unit])
        {
            dnx_sbusdma_desc_counter_max[unit] = dnx_sbusdma_desc_counter[unit];
        }
        dnx_sbusdma_desc_commit_counter[unit]++;

        dnx_sbusdma_desc_mem_buff_counter[unit] = 0;
        dnx_sbusdma_desc_counter[unit] = 0;

        SHR_IF_ERR_CONT(soc_sbusdma_desc_create
                        (unit, &desc_ctrl, desc_cfg_array, (sbusdma_desc_handle_t *) & dnx_sbusdma_desc_handle[unit]));
        SHR_IF_ERR_CONT(soc_sbusdma_desc_run(unit, (sbusdma_desc_handle_t) dnx_sbusdma_desc_handle[unit]));
    }

exit:
    if ((dnx_sbusdma_desc_timeout_mutex[unit] != NULL) && !safe)
    {
        sal_mutex_give(dnx_sbusdma_desc_timeout_mutex[unit]);
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_sbusdma_desc_wait_done(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    if (dnx_sbusdma_desc_enabled[unit])
    {
        /*
         * Commit twice to make sure we are done
         */
        SHR_IF_ERR_CONT(dnx_sbusdma_desc_commit(unit, 0 /* safe */ ));
        SHR_IF_ERR_CONT(dnx_sbusdma_desc_commit(unit, 0 /* safe */ ));
    }

    SHR_FUNC_EXIT;
}

/*
 * Adds a memory (if mem != INVALIDm) or a register access to descriptor DMA.
 * blk is the block for memories and the block instance for registers.
 */
shr_error_e
dnx_sbusdma_desc_add_mem_reg(
    int unit,
    soc_mem_t mem,
    soc_reg_t reg,
    uint32 array_index,
    int blk,
    uint32 offset,
    uint32 count,
    int mem_clear,
    void *entry_data)
{
    uint32 cfg_addr;
    uint8 cfg_acc_type;
    int cfg_blk = blk;
    uint32 entry_size;
    uint32 give_sem = 0;
    uint32 rv = 0;
    uint32 entries_to_shift;

    SHR_FUNC_INIT_VARS(unit);

    if (dnx_sbusdma_desc_enabled[unit] != 1)
    {
        SHR_ERR_EXIT(SOC_E_UNAVAIL, "Desc DMA not enabled.");
    }

    if (dnx_sbusdma_desc_timeout_mutex[unit] != NULL)
    {
        sal_mutex_take(dnx_sbusdma_desc_timeout_mutex[unit], sal_mutex_FOREVER);
    }

    if (mem != INVALIDm)
    {
        SOC_MEM_ALIAS_TO_ORIG(unit, mem);

        cfg_addr = soc_mem_addr_get(unit, mem, array_index, cfg_blk, offset, &cfg_acc_type);
        entry_size = SOC_MEM_WORDS(unit, mem);

        if (soc_feature(unit, soc_feature_new_sbus_format))
        {
            cfg_blk = SOC_BLOCK2SCH(unit, cfg_blk);
        }
    }
    else if (reg != INVALIDr)
    {
        /** count is only  relevant for tables */
        count = 1;

        cfg_addr = soc_reg_addr_get(unit, reg, blk, array_index, SOC_REG_ADDR_OPTION_WRITE, &cfg_blk, &cfg_acc_type);

        if (SOC_REG_IS_ABOVE_64(unit, reg))
        {
            entry_size = SOC_REG_ABOVE_64_INFO(unit, reg).size;
        }
        else if (SOC_REG_IS_64(unit, reg))
        {
            entry_size = 2;
        }
        else
        {
            entry_size = 1;
        }
    }
    else
    {
        if (dnx_sbusdma_desc_timeout_mutex[unit] != NULL)
        {
            sal_mutex_give(dnx_sbusdma_desc_timeout_mutex[unit]);
        }
        SHR_ERR_EXIT(SOC_E_PARAM, "Descriptor DMA expects either valid mem or valid reg.");
    }

    /** in case of mem_clear we only use a single entry to set the whole table */
    entries_to_shift = mem_clear ? 1 : count;

    /*
     * Commit the descriptors if we have reached the descriptor limit or we do not have enough memory in buffer
     */
    if ((dnx_sbusdma_desc_counter[unit] == dnx_sbusdma_desc_cfg_max[unit]) ||
        /*
         * The extra buffer is necessary for the descriptor DMA to operate correctly
         */
        (dnx_sbusdma_desc_mem_buff_counter[unit] + entry_size * entries_to_shift +
         (SOC_REG_ABOVE_64_MAX_SIZE_U32 + 2) >= dnx_sbusdma_desc_mem_max[unit]))
    {
        rv = dnx_sbusdma_desc_commit(unit, 1 /* safe */ );
        if (rv != SOC_E_NONE)
        {
            if (dnx_sbusdma_desc_timeout_mutex[unit] != NULL)
            {
                sal_mutex_give(dnx_sbusdma_desc_timeout_mutex[unit]);
            }
            SHR_ERR_EXIT(rv, "dnx_sbusdma_desc_commit_unsafe failed.");
        }
    }

    if (dnx_sbusdma_desc_counter[unit] == 0)
    {
        dnx_sbusdma_desc_timeout_add_time[unit] = sal_time_usecs();
        give_sem++;
    }

    sal_memcpy(&dnx_sbusdma_desc_mem_buff_main[unit][dnx_sbusdma_desc_mem_buff_counter[unit]], entry_data,
               sizeof(uint32) * entry_size * entries_to_shift);

    if (mem != INVALIDm)
    {
        dnx_sbusdma_desc_cfg_array_main[unit][dnx_sbusdma_desc_counter[unit]].flags =
            SOC_SBUSDMA_MEMORY_CMD_MSG | SOC_SBUSDMA_WRITE_CMD_MSG;
    }
    else if (reg != INVALIDr)
    {
        dnx_sbusdma_desc_cfg_array_main[unit][dnx_sbusdma_desc_counter[unit]].flags = SOC_SBUSDMA_WRITE_CMD_MSG;
    }

    dnx_sbusdma_desc_cfg_array_main[unit][dnx_sbusdma_desc_counter[unit]].acc_type = cfg_acc_type;
    dnx_sbusdma_desc_cfg_array_main[unit][dnx_sbusdma_desc_counter[unit]].addr = cfg_addr;
    dnx_sbusdma_desc_cfg_array_main[unit][dnx_sbusdma_desc_counter[unit]].addr_shift = 0;
    dnx_sbusdma_desc_cfg_array_main[unit][dnx_sbusdma_desc_counter[unit]].blk = cfg_blk;
    dnx_sbusdma_desc_cfg_array_main[unit][dnx_sbusdma_desc_counter[unit]].buff =
        &dnx_sbusdma_desc_mem_buff_main[unit][dnx_sbusdma_desc_mem_buff_counter[unit]];
    dnx_sbusdma_desc_cfg_array_main[unit][dnx_sbusdma_desc_counter[unit]].count = count;
    dnx_sbusdma_desc_cfg_array_main[unit][dnx_sbusdma_desc_counter[unit]].width = entry_size;
    dnx_sbusdma_desc_cfg_array_main[unit][dnx_sbusdma_desc_counter[unit]].mem_clear = mem_clear;

    dnx_sbusdma_desc_counter[unit] += 1;

    dnx_sbusdma_desc_mem_buff_counter[unit] += entry_size * entries_to_shift;

    if (dnx_sbusdma_desc_timeout_mutex[unit] != NULL)
    {
        sal_mutex_give(dnx_sbusdma_desc_timeout_mutex[unit]);
    }

    if ((dnx_sbusdma_desc_timeout_sem[unit] != 0) && give_sem)
    {
        sal_sem_give(dnx_sbusdma_desc_timeout_sem[unit]);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_sbusdma_desc_add_reg(
    int unit,
    uint32 reg_u32,
    int instance,
    uint32 array_index,
    void *entry_data)
{
    soc_reg_t reg = reg_u32;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_sbusdma_desc_add_mem_reg(unit, INVALIDm, reg, array_index, instance, 0 /* offset */ , 1 /* count 
                                                                                                                 */ , FALSE, entry_data));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_sbusdma_desc_add_mem(
    int unit,
    uint32 mem_u32,
    int array_index,
    int blk,
    uint32 offset,
    void *entry_data)
{
    soc_mem_t mem = mem_u32;
    int array_index_max, array_index_min, array_id, blk_i;

    SHR_FUNC_INIT_VARS(unit);

    array_index_min = 0;
    array_index_max = 1;

    /** if array_index = -1 then clear all mem array indices */
    if (SOC_MEM_IS_ARRAY(unit, mem))
    {
        if (array_index == -1)
        {
            array_index_max = SOC_MEM_NUMELS(unit, mem) + SOC_MEM_FIRST_ARRAY_INDEX(unit, mem);
            array_index_min = SOC_MEM_FIRST_ARRAY_INDEX(unit, mem);
        }
        else
        {
            /** iterate once */
            array_index_min = array_index;
            array_index_max = array_index + 1;
        }
    }

    for (array_id = array_index_min; array_id < array_index_max; array_id++)
    {
        if (blk == MEM_BLOCK_ALL)
        {
            SOC_MEM_BLOCK_ITER(unit, mem, blk_i)
            {
                SHR_IF_ERR_EXIT(dnx_sbusdma_desc_add_mem_reg(unit, mem, INVALIDr, array_id, blk_i, offset, 1    /* count 
                                                                                                                 */ ,
                                                             FALSE, entry_data));
                _soc_mem_write_cache_update(unit, mem, blk_i, 0, offset, array_index, entry_data, NULL, NULL, NULL);
            }
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_sbusdma_desc_add_mem_reg(unit, mem, INVALIDr, array_id, blk, offset, 1 /* count */ ,
                                                         FALSE, entry_data));
            _soc_mem_write_cache_update(unit, mem, blk, 0, offset, array_index, entry_data, NULL, NULL, NULL);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_sbusdma_desc_add_mem_table(
    int unit,
    uint32 mem_u32,
    int array_index,
    int blk,
    uint32 offset,
    uint32 count,
    void *entry_data)
{
    soc_mem_t mem = mem_u32;
    int array_index_max, array_index_min, array_id, blk_i;
    uint32 final_offset, final_count;

    SHR_FUNC_INIT_VARS(unit);

    final_offset = offset;
    final_count = count;

    array_index_min = 0;
    array_index_max = 1;

    /** if array_index = -1 then clear all mem array indices */
    if (SOC_MEM_IS_ARRAY(unit, mem))
    {
        if (array_index == -1)
        {
            array_index_max = SOC_MEM_NUMELS(unit, mem) + SOC_MEM_FIRST_ARRAY_INDEX(unit, mem);
            array_index_min = SOC_MEM_FIRST_ARRAY_INDEX(unit, mem);
        }
        else
        {
            /** iterate once */
            array_index_min = array_index;
            array_index_max = array_index + 1;
        }
    }

    /** iterate over array indices */
    for (array_id = array_index_min; array_id < array_index_max; array_id++)
    {
        if (blk == MEM_BLOCK_ALL)
        {
            SOC_MEM_BLOCK_ITER(unit, mem, blk_i)
            {
                SHR_IF_ERR_EXIT(dnx_sbusdma_desc_add_mem_reg(unit, mem, INVALIDr, array_id, blk_i, final_offset,
                                                             final_count, FALSE, entry_data));
            }
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_sbusdma_desc_add_mem_reg(unit, mem, INVALIDr, array_id, blk, final_offset,
                                                         final_count, FALSE, entry_data));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_sbusdma_desc_add_mem_clear(
    int unit,
    uint32 mem_u32,
    int array_index,
    int blk,
    void *entry_data)
{
    soc_mem_t mem = mem_u32;
    int array_index_max, array_index_min, array_id, blk_i;
    uint32 offset, count;

    SHR_FUNC_INIT_VARS(unit);

    array_index_min = 0;
    array_index_max = 1;

    /** if array_index = -1 then clear all mem array indices */
    if (SOC_MEM_IS_ARRAY(unit, mem))
    {
        if (array_index == -1)
        {
            array_index_max = SOC_MEM_NUMELS(unit, mem) + SOC_MEM_FIRST_ARRAY_INDEX(unit, mem);
            array_index_min = SOC_MEM_FIRST_ARRAY_INDEX(unit, mem);
        }
        else
        {
            /** iterate once */
            array_index_min = array_index;
            array_index_max = array_index + 1;
        }
    }

    /** mem clear sets all table entries */
    offset = soc_mem_index_min(unit, mem);
    count = soc_mem_index_max(unit, mem) - offset + 1;

    /** iterate over array indices */
    for (array_id = array_index_min; array_id < array_index_max; array_id++)
    {
        if (blk == MEM_BLOCK_ALL)
        {
            SOC_MEM_BLOCK_ITER(unit, mem, blk_i)
            {
                SHR_IF_ERR_EXIT(dnx_sbusdma_desc_add_mem_reg(unit, mem, INVALIDr, array_id, blk_i, offset, count, TRUE,
                                                             entry_data));
            }
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_sbusdma_desc_add_mem_reg(unit, mem, INVALIDr, array_id, blk, offset, count, TRUE,
                                                         entry_data));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_sbusdma_desc_cleanup(
    int unit)
{
    int module_iter;
    SHR_FUNC_INIT_VARS(unit);

    if (dnx_sbusdma_desc_timeout_mutex[unit] != NULL)
    {
        sal_mutex_take(dnx_sbusdma_desc_timeout_mutex[unit], sal_mutex_FOREVER);
    }

    if (dnx_sbusdma_desc_mem_buff_a[unit] != NULL)
    {
        soc_cm_sfree(unit, dnx_sbusdma_desc_mem_buff_a[unit]);
        dnx_sbusdma_desc_mem_buff_a[unit] = NULL;
    }
    if (dnx_sbusdma_desc_mem_buff_b[unit] != NULL)
    {
        soc_cm_sfree(unit, dnx_sbusdma_desc_mem_buff_b[unit]);
        dnx_sbusdma_desc_mem_buff_b[unit] = NULL;
    }
    dnx_sbusdma_desc_mem_buff_main[unit] = NULL;

    if (dnx_sbusdma_desc_cfg_array_a[unit] != NULL)
    {
        sal_free(dnx_sbusdma_desc_cfg_array_a[unit]);
        dnx_sbusdma_desc_cfg_array_a[unit] = NULL;
    }
    if (dnx_sbusdma_desc_cfg_array_b[unit] != NULL)
    {
        sal_free(dnx_sbusdma_desc_cfg_array_b[unit]);
        dnx_sbusdma_desc_cfg_array_b[unit] = NULL;
    }
    dnx_sbusdma_desc_cfg_array_main[unit] = NULL;

    /*
     * Signal the thread to destroy the mutex and sem if it is active, otherwise directly destroy them 
     */
    if ((dnx_sbusdma_desc_timeout_tid[unit] != NULL) && (dnx_sbusdma_desc_timeout_tid[unit] != SAL_THREAD_ERROR))
    {
        dnx_sbusdma_desc_timeout_terminate[unit] = 1;
        sal_mutex_give(dnx_sbusdma_desc_timeout_mutex[unit]);
        sal_sem_give(dnx_sbusdma_desc_timeout_sem[unit]);
    }
    else
    {
        if (dnx_sbusdma_desc_timeout_mutex[unit])
        {
            sal_mutex_give(dnx_sbusdma_desc_timeout_mutex[unit]);
            sal_mutex_destroy(dnx_sbusdma_desc_timeout_mutex[unit]);
            dnx_sbusdma_desc_timeout_mutex[unit] = NULL;
        }

        if (dnx_sbusdma_desc_timeout_sem[unit])
        {
            sal_sem_destroy(dnx_sbusdma_desc_timeout_sem[unit]);
            dnx_sbusdma_desc_timeout_sem[unit] = NULL;
        }
    }

    dnx_sbusdma_desc_enabled[unit] = 0;
    for (module_iter = 0; module_iter < SBUSDMA_DESC_MODULE_ENABLE_NOF_MODULES; module_iter++)
    {
        dnx_sbusdma_desc_module_enabled[unit][module_iter] = 0;
    }
    dnx_sbusdma_desc_mem_max[unit] = 0;
    dnx_sbusdma_desc_cfg_max[unit] = 0;

    SHR_FUNC_EXIT;
}

STATIC void
dnx_sbusdma_desc_init_timeout_thread(
    void *cookie)
{
    int unit = PTR_TO_INT(cookie);
    int rv = SOC_E_NONE;
    char thread_name[SAL_THREAD_NAME_MAX_LEN];
    sal_thread_t thread;
    sal_usecs_t elapsed_time;
    sal_usecs_t add_time;
    sal_usecs_t timeout_max = dnx_sbusdma_desc_time_out_max[unit];

    while (1)
    {
        sal_sem_take(dnx_sbusdma_desc_timeout_sem[unit], sal_sem_FOREVER);
        sal_mutex_take(dnx_sbusdma_desc_timeout_mutex[unit], sal_mutex_FOREVER);
        if (dnx_sbusdma_desc_timeout_terminate[unit])
        {
            /*
             * Destroy the mutex and sem along with the thread 
             */
            sal_mutex_give(dnx_sbusdma_desc_timeout_mutex[unit]);
            if (dnx_sbusdma_desc_timeout_mutex[unit])
            {
                sal_mutex_destroy(dnx_sbusdma_desc_timeout_mutex[unit]);
                dnx_sbusdma_desc_timeout_mutex[unit] = NULL;
            }

            if (dnx_sbusdma_desc_timeout_sem[unit])
            {
                sal_sem_destroy(dnx_sbusdma_desc_timeout_sem[unit]);
                dnx_sbusdma_desc_timeout_sem[unit] = NULL;
            }

            dnx_sbusdma_desc_timeout_tid[unit] = NULL;

            sal_thread_exit(0);
        }

        /*
         * In case the main thread has already committed this descriptor chain 
         */
        if (dnx_sbusdma_desc_counter[unit] == 0)
        {
            sal_mutex_give(dnx_sbusdma_desc_timeout_mutex[unit]);
            continue;
        }
        add_time = dnx_sbusdma_desc_timeout_add_time[unit];
        elapsed_time = sal_time_usecs() - add_time;
        while (elapsed_time < timeout_max)
        {
            sal_mutex_give(dnx_sbusdma_desc_timeout_mutex[unit]);
            sal_usleep(timeout_max - elapsed_time);
            sal_mutex_take(dnx_sbusdma_desc_timeout_mutex[unit], sal_mutex_FOREVER);
            elapsed_time = sal_time_usecs() - add_time;
        }

        /*
         * Check if the main thread has already committed this descriptor chain and started a new one 
         */
        if (add_time == dnx_sbusdma_desc_timeout_add_time[unit])
        {
            sal_mutex_give(dnx_sbusdma_desc_timeout_mutex[unit]);
            rv = dnx_sbusdma_desc_commit(unit, 0 /* safe */ );
            if (rv != SOC_E_NONE)
            {
                LOG_ERROR(BSL_LS_SOC_DMA,
                          (BSL_META_U(unit, "dnx_sbusdma_desc_commit returned with error: %d, unit %d\n"), rv, unit));
            }
        }
        else
        {
            sal_mutex_give(dnx_sbusdma_desc_timeout_mutex[unit]);
        }
    }

    /*
     * Some error happened. 
     */
    thread = sal_thread_self();
    thread_name[0] = 0;
    sal_thread_name(thread, thread_name, sizeof(thread_name));
    LOG_ERROR(BSL_LS_SOC_DMA, (BSL_META_U(unit, "AbnormalThreadExit:%s, unit %d\n"), thread_name, unit));

    dnx_sbusdma_desc_timeout_tid[unit] = SAL_THREAD_ERROR;

    sal_thread_exit(0);
}

shr_error_e
dnx_sbusdma_desc_init_inner(
    int unit,
    uint32 buff_size_kb,
    uint32 chain_length_max,
    uint32 timeout_usec)
{
    char buffer_name[50];
    int module_iter;

    SHR_FUNC_INIT_VARS(unit);

    if ((dnx_sbusdma_desc_mem_buff_a[unit] != NULL) || (dnx_sbusdma_desc_mem_buff_b[unit] != NULL))
    {
        SHR_ERR_EXIT(SOC_E_MEMORY, "Descriptor DMA buffers already allocated.");
    }

    /*
     * Descriptor DMA is enabled, but one of the size values is invalid
     */
    if ((buff_size_kb == 0) || (chain_length_max == 0))
    {
        SHR_ERR_EXIT(SOC_E_CONFIG,
                     "Descriptor DMA size values are invalid, they should either be all zeros(disabled) or positive."
                     "Current values: buff_size_kb %d, chain_length_max %d, timeout_usec %d", buff_size_kb,
                     chain_length_max, timeout_usec);
    }

    dnx_sbusdma_desc_mem_max[unit] = buff_size_kb * DNX_SBUSDMA_DESC_KB_TO_UINT32;
    dnx_sbusdma_desc_cfg_max[unit] = chain_length_max;
    dnx_sbusdma_desc_time_out_max[unit] = timeout_usec;

    dnx_sbusdma_desc_commit_counter[unit] = 0;

    /*
     * allocate memory for common memory double-buffer 
     */
    sal_snprintf(buffer_name, sizeof(buffer_name), "SBUSDMA desc buffer a, Unit %d", unit);
    if ((dnx_sbusdma_desc_mem_buff_a[unit] =
         (uint32 *) soc_cm_salloc(unit, sizeof(uint32) * dnx_sbusdma_desc_mem_max[unit], buffer_name)) == NULL)
    {
        SHR_ERR_EXIT(SOC_E_MEMORY, "Error: Fail to allocate memory for SBUSDMA desc buffer a.");
    }
    sal_memset(dnx_sbusdma_desc_mem_buff_a[unit], 0, sizeof(uint32) * dnx_sbusdma_desc_mem_max[unit]);

    sal_snprintf(buffer_name, sizeof(buffer_name), "SBUSDMA desc buffer b, Unit %d", unit);
    if ((dnx_sbusdma_desc_mem_buff_b[unit] =
         (uint32 *) soc_cm_salloc(unit, sizeof(uint32) * dnx_sbusdma_desc_mem_max[unit], buffer_name)) == NULL)
    {
        dnx_sbusdma_desc_cleanup(unit);
        SHR_ERR_EXIT(SOC_E_MEMORY, "Error: Fail to allocate memory for SBUSDMA desc buffer b.");
    }
    sal_memset(dnx_sbusdma_desc_mem_buff_b[unit], 0, sizeof(uint32) * dnx_sbusdma_desc_mem_max[unit]);

    dnx_sbusdma_desc_mem_buff_main[unit] = dnx_sbusdma_desc_mem_buff_a[unit];
    dnx_sbusdma_desc_mem_buff_counter[unit] = 0;
    dnx_sbusdma_desc_mem_buff_counter_max[unit] = 0;
    dnx_sbusdma_desc_mem_buff_counter_total[unit] = 0;

    /*
     * allocate memory for host memory descriptor buffer 
     */
    dnx_sbusdma_desc_cfg_array_a[unit] =
        sal_alloc(sizeof(soc_sbusdma_desc_cfg_t) * dnx_sbusdma_desc_cfg_max[unit], "soc_sbusdma_desc_cfg_t");
    if (dnx_sbusdma_desc_cfg_array_a[unit] == NULL)
    {
        dnx_sbusdma_desc_cleanup(unit);
        SHR_ERR_EXIT(SOC_E_MEMORY, "Error: Fail to allocate memory for SBUSDMA desc_cfg a");
    }

    dnx_sbusdma_desc_cfg_array_b[unit] =
        sal_alloc(sizeof(soc_sbusdma_desc_cfg_t) * dnx_sbusdma_desc_cfg_max[unit], "soc_sbusdma_desc_cfg_t");
    if (dnx_sbusdma_desc_cfg_array_b[unit] == NULL)
    {
        dnx_sbusdma_desc_cleanup(unit);
        SHR_ERR_EXIT(SOC_E_MEMORY, "Error: Fail to allocate memory for SBUSDMA desc_cfg b");
    }
    dnx_sbusdma_desc_cfg_array_main[unit] = dnx_sbusdma_desc_cfg_array_a[unit];
    dnx_sbusdma_desc_counter[unit] = 0;
    dnx_sbusdma_desc_counter_max[unit] = 0;
    dnx_sbusdma_desc_counter_total[unit] = 0;

    /*
     * Only allocate mutex+semaphore and activate timeout thread if the timeout param is non-zero
     */
    if (timeout_usec != 0)
    {
        dnx_sbusdma_desc_timeout_mutex[unit] = sal_mutex_create("DESC DMA TO mutex");
        if (dnx_sbusdma_desc_timeout_mutex[unit] == NULL)
        {
            dnx_sbusdma_desc_cleanup(unit);
            SHR_ERR_EXIT(SOC_E_MEMORY, "TO Mutex allocation failure.");
        }

        dnx_sbusdma_desc_timeout_sem[unit] = sal_sem_create("DESC DMA TO sem", sal_sem_COUNTING, 0);
        if (dnx_sbusdma_desc_timeout_sem[unit] == NULL)
        {
            dnx_sbusdma_desc_cleanup(unit);
            SHR_ERR_EXIT(SOC_E_MEMORY, "TO Sempahore allocation failure.");
        }

        dnx_sbusdma_desc_timeout_add_time[unit] = sal_time_usecs();

        dnx_sbusdma_desc_timeout_terminate[unit] = 0;

        if ((dnx_sbusdma_desc_timeout_tid[unit] == SAL_THREAD_ERROR) || (dnx_sbusdma_desc_timeout_tid[unit] == NULL))
        {
            dnx_sbusdma_desc_timeout_tid[unit] = sal_thread_create("Desc DMA Timeout", SAL_THREAD_STKSZ, 50     /* priority 
                                                                                                                 */ ,
                                                                   dnx_sbusdma_desc_init_timeout_thread,
                                                                   INT_TO_PTR(unit));
            if ((dnx_sbusdma_desc_timeout_tid[unit] == NULL)
                || (dnx_sbusdma_desc_timeout_tid[unit] == SAL_THREAD_ERROR))
            {
                dnx_sbusdma_desc_cleanup(unit);
                SHR_ERR_EXIT(SOC_E_INTERNAL, "DESC DMA TO thread create failed. \n");
            }
        }
        else
        {
            dnx_sbusdma_desc_cleanup(unit);
            SHR_ERR_EXIT(SOC_E_EXISTS, "TO thread already exists.");
        }
    }

    dnx_sbusdma_desc_enabled[unit] = 1;
    for (module_iter = 0; module_iter < SBUSDMA_DESC_MODULE_ENABLE_NOF_MODULES; module_iter++)
    {
        dnx_sbusdma_desc_module_enabled[unit][module_iter] = dnx_data_sbusdma_desc.global.enable_module_desc_dma_get(
    unit,
    module_iter)->enable;
    }

    dnx_sbusdma_desc_handle[unit] = 0;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_sbusdma_desc_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

#if !defined(ADAPTER_SERVER_MODE) && !defined(PLISIM)
    if (dnx_data_sbusdma_desc.global.feature_get(unit, dnx_data_sbusdma_desc_global_desc_dma))
    {
        uint32 buff_size_kb = dnx_data_sbusdma_desc.global.dma_desc_aggregator_buff_size_kb_get(
    unit);
        uint32 chain_length_max = dnx_data_sbusdma_desc.global.dma_desc_aggregator_chain_length_max_get(
    unit);
        uint32 timeout_usec = dnx_data_sbusdma_desc.global.dma_desc_aggregator_timeout_usec_get(
    unit);

        SHR_IF_ERR_EXIT(dnx_sbusdma_desc_init_with_params(unit, buff_size_kb, chain_length_max, timeout_usec));
    }

exit:
#endif
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_sbusdma_desc_init_with_params(
    int unit,
    uint32 buff_size_kb,
    uint32 chain_length_max,
    uint32 timeout_usec)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * All 3 descriptor DMA properties must be non-zero
     */
    if ((buff_size_kb != 0) && (chain_length_max != 0) && (timeout_usec != 0))
    {
        if ((dnx_data_intr.general.tslam_dma_enable_get(unit) == FALSE)
            || (dnx_data_intr.general.table_dma_enable_get(unit) == FALSE))
        {
            SHR_ERR_EXIT(SOC_E_CONFIG,
                         "%s and %s soc properties must be enabled if dma_desc_aggregator soc properties are enabled.",
                         spn_TSLAM_DMA_ENABLE, spn_TABLE_DMA_ENABLE);
        }
        SHR_IF_ERR_EXIT(dnx_sbusdma_desc_init_inner(unit, buff_size_kb, chain_length_max, timeout_usec));
    }
    else
    {
        SHR_ERR_EXIT(SOC_E_PARAM,
                     "All three descriptor DMA parameters must be non-zero. buff_size_kb=%d, chain_length_max=%d, timeout_usec=%d.",
                     buff_size_kb, chain_length_max, timeout_usec);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_sbusdma_desc_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    if (dnx_sbusdma_desc_enabled[unit])
    {
        SHR_IF_ERR_CONT(dnx_sbusdma_desc_wait_done(unit));
        dnx_sbusdma_desc_cleanup(unit);

        dnx_sbusdma_desc_enabled[unit] = 0;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_sbusdma_desc_get_stats(
    int unit,
    int clear,
    dnx_sbusdma_desc_stats_t * desc_stats)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * If descriptor DMA is enabled take the mutex and retrieve the counters, otherwise return all 0
     */
    if (dnx_sbusdma_desc_enabled[unit])
    {
        sal_mutex_take(dnx_sbusdma_desc_timeout_mutex[unit], sal_mutex_FOREVER);
        desc_stats->buff_size_kb = dnx_sbusdma_desc_mem_max[unit] / DNX_SBUSDMA_DESC_KB_TO_UINT32;
        desc_stats->chain_length_max = dnx_sbusdma_desc_cfg_max[unit];
        desc_stats->timeout_usec = dnx_sbusdma_desc_time_out_max[unit];
        desc_stats->enabled = dnx_sbusdma_desc_enabled[unit];

        desc_stats->commit_counter = dnx_sbusdma_desc_commit_counter[unit];
        desc_stats->chain_length_counter_max = dnx_sbusdma_desc_counter_max[unit];
        desc_stats->chain_length_counter_total = dnx_sbusdma_desc_counter_total[unit];
        desc_stats->buff_size_kb_max =
            UTILEX_DIV_ROUND_UP(dnx_sbusdma_desc_mem_buff_counter_max[unit], DNX_SBUSDMA_DESC_KB_TO_UINT32);
        desc_stats->buff_size_kb_total =
            UTILEX_DIV_ROUND_UP(dnx_sbusdma_desc_mem_buff_counter_total[unit], DNX_SBUSDMA_DESC_KB_TO_UINT32);
        if (clear == TRUE)
        {
            dnx_sbusdma_desc_commit_counter[unit] = 0;
            dnx_sbusdma_desc_counter_max[unit] = 0;
            dnx_sbusdma_desc_counter_total[unit] = 0;
            dnx_sbusdma_desc_mem_buff_counter_max[unit] = 0;
            dnx_sbusdma_desc_mem_buff_counter_total[unit] = 0;
        }
        sal_mutex_give(dnx_sbusdma_desc_timeout_mutex[unit]);
    }
    else
    {
        sal_memset(desc_stats, 0x0, sizeof(dnx_sbusdma_desc_stats_t));
    }

    SHR_FUNC_EXIT;
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>
