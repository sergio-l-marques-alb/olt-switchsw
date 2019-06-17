/*
 * ! \file mdb_sbusdma_desc.c Contains all of the MDB descriptor DMA functions
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

/*************
 * INCLUDES  *
 *************/
/* { */

#include <shared/bsl.h>
#include <soc/dcmn/error.h>
#include <soc/mem.h>

#include <soc/sbusdma.h>

#include <soc/dnx/mdb_sbusdma_desc.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_device.h>
#include <sal/core/time.h>

#include <sal/appl/sal.h>

#include <soc/dnx/swstate/auto_generated/access/mdb_access.h>
#include <soc/dnx/swstate/auto_generated/types/mdb_types.h>

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_MDBDNX

/* } */

/*************
 * DEFINES   *
 *************/
/* { */

/*#define MDB_SBUSDMA_DESC_PRINTS_ENABLED*/

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

STATIC uint32 mdb_sbusdma_desc_enabled[SOC_MAX_NUM_DEVICES];

/* Common memory double-buffer */
STATIC uint32 *mdb_sbusdma_desc_mem_buff_a[SOC_MAX_NUM_DEVICES];
STATIC uint32 *mdb_sbusdma_desc_mem_buff_b[SOC_MAX_NUM_DEVICES];
STATIC uint32 *mdb_sbusdma_desc_mem_buff_main[SOC_MAX_NUM_DEVICES];
STATIC uint32 mdb_sbusdma_desc_mem_buff_counter[SOC_MAX_NUM_DEVICES];

/* Host memory descriptors */
STATIC soc_sbusdma_desc_cfg_t *mdb_sbusdma_desc_cfg_array_a[SOC_MAX_NUM_DEVICES];
STATIC soc_sbusdma_desc_cfg_t *mdb_sbusdma_desc_cfg_array_b[SOC_MAX_NUM_DEVICES];
STATIC soc_sbusdma_desc_cfg_t *mdb_sbusdma_desc_cfg_array_main[SOC_MAX_NUM_DEVICES];
STATIC uint32 mdb_sbusdma_desc_counter[SOC_MAX_NUM_DEVICES];

/* Configuration of memory buffer size and max number of descriptors in chain */
STATIC uint32 mdb_sbusdma_desc_cfg_max[SOC_MAX_NUM_DEVICES];
STATIC uint32 mdb_sbusdma_desc_mem_max[SOC_MAX_NUM_DEVICES];
STATIC volatile uint32 mdb_sbusdma_desc_time_out_max[SOC_MAX_NUM_DEVICES];

/* Holds the last descriptor handle, 0 if the last has ended */
STATIC volatile sbusdma_desc_handle_t mdb_sbusdma_desc_handle[SOC_MAX_NUM_DEVICES];

STATIC volatile sal_sem_t mdb_sbusdma_desc_timeout_sem[SOC_MAX_NUM_DEVICES];
STATIC volatile sal_mutex_t mdb_sbusdma_desc_timeout_mutex[SOC_MAX_NUM_DEVICES];
STATIC volatile sal_usecs_t mdb_sbusdma_desc_timeout_add_time[SOC_MAX_NUM_DEVICES];     /* Holds * the * descriptor *
                                                                                         * chain * create * time */
STATIC volatile sal_thread_t mdb_sbusdma_desc_timeout_tid[SOC_MAX_NUM_DEVICES];
STATIC volatile uint8 mdb_sbusdma_desc_timeout_terminate[SOC_MAX_NUM_DEVICES];

/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */

uint32
mdb_sbusdma_desc_is_enabled(
    int unit)
{
    return mdb_sbusdma_desc_enabled[unit];
}

shr_error_e
mdb_sbusdma_desc_status(
    int unit,
    uint32 *desc_num_max,
    uint32 *mem_buff_size,
    uint32 *timeout_usec)
{
    SHR_FUNC_INIT_VARS(unit);

    *desc_num_max = mdb_sbusdma_desc_cfg_max[unit];
    *mem_buff_size = mdb_sbusdma_desc_mem_max[unit];
    *timeout_usec = mdb_sbusdma_desc_time_out_max[unit];

    SHR_FUNC_EXIT;
}

/*This function must be called while holding mdb_sbusdma_desc_timeout_mutex*/
STATIC shr_error_e
mdb_sbusdma_desc_wait_previous_done(
    int unit)
{
    soc_timeout_t to;
    uint8 state = 1;

    SHR_FUNC_INIT_VARS(unit);

    if (mdb_sbusdma_desc_enabled[unit])
    {
        soc_timeout_init(&to, SOC_SBUSDMA_DM_TO(unit) * 2, 0);

        if (mdb_sbusdma_desc_handle[unit] != 0)
        {
            SHR_IF_ERR_EXIT(soc_sbusdma_desc_get_state(unit, mdb_sbusdma_desc_handle[unit], &state));
            /*
             * Wait for the previous request to finish 
             */
            while (state != 0)
            {
                if (soc_timeout_check(&to))
                {
                    SHR_ERR_EXIT(SOC_E_TIMEOUT, "Timeout waiting for descriptor DMA to finish.");
                }
                SHR_IF_ERR_EXIT(soc_sbusdma_desc_get_state(unit, mdb_sbusdma_desc_handle[unit], &state));
            }
        }
    }

exit:
    if (mdb_sbusdma_desc_handle[unit] != 0)
    {
        SHR_IF_ERR_CONT(soc_sbusdma_desc_delete(unit, mdb_sbusdma_desc_handle[unit]));
        mdb_sbusdma_desc_handle[unit] = 0;
    }
    SHR_FUNC_EXIT;
}

void STATIC
mdb_sbusdma_desc_cb(
    int unit,
    int status,
    sbusdma_desc_handle_t handle,
    void *data)
{
#ifdef MDB_SBUSDMA_DESC_PRINTS_ENABLED
    if (status == SOC_E_NONE)
    {
        int rv = SOC_E_NONE;
        int i = 0;
        soc_sbusdma_desc_ctrl_t ctrl = { 0 };
        soc_sbusdma_desc_cfg_t *cfg = NULL;

        LOG_CLI((BSL_META_U(0, "Successfully done DESC DMA, handle: %d\n"), handle));

        cfg = sal_alloc(sizeof(soc_sbusdma_desc_cfg_t) * mdb_sbusdma_desc_cfg_max[unit], "soc_sbusdma_desc_cfg_t");
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
                LOG_ERROR(BSL_LS_SOC_DMA,
                          (BSL_META_U(unit,
                                      "blk: %d, addr: %d, width: %d, count: %d, addr_shift: %d\n"),
                           cfg[i].blk, cfg[i].addr, cfg[i].width, cfg[i].count, cfg[i].addr_shift));
            }

            sal_free(cfg);
        }
    }
#endif /* MDB_SBUSDMA_DESC_PRINTS_ENABLED */

    if (status != SOC_E_NONE)
    {
        int rv = SOC_E_NONE;
        int i = 0;
        soc_sbusdma_desc_ctrl_t ctrl = { 0 };
        soc_sbusdma_desc_cfg_t *cfg = NULL;

        LOG_ERROR(BSL_LS_SOC_DMA, (BSL_META_U(unit, "Desc SBUSDMA failed, handle: %d\n"), handle));

        cfg = sal_alloc(sizeof(soc_sbusdma_desc_cfg_t) * mdb_sbusdma_desc_cfg_max[unit], "soc_sbusdma_desc_cfg_t");
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
                LOG_ERROR(BSL_LS_SOC_DMA,
                          (BSL_META_U(unit,
                                      "blk: %d, addr: %d, width: %d, count: %d, addr_shift: %d\n"),
                           cfg[i].blk, cfg[i].addr, cfg[i].width, cfg[i].count, cfg[i].addr_shift));
            }

            sal_free(cfg);
        }
    }
}

STATIC shr_error_e
mdb_sbusdma_desc_commit(
    int unit,
    uint8 safe)
{
    soc_sbusdma_desc_ctrl_t desc_ctrl = { 0 };
    soc_sbusdma_desc_cfg_t *desc_cfg_array;

    SHR_FUNC_INIT_VARS(unit);

    if (!safe)
    {
        sal_mutex_take(mdb_sbusdma_desc_timeout_mutex[unit], sal_mutex_FOREVER);
    }

    SHR_IF_ERR_EXIT(mdb_sbusdma_desc_wait_previous_done(unit));

    if (mdb_sbusdma_desc_counter[unit] != 0)
    {
        desc_ctrl.flags = SOC_SBUSDMA_CFG_USE_FLAGS | SOC_SBUSDMA_WRITE_CMD_MSG;
        sal_strncpy(desc_ctrl.name, "DESC DMA", sizeof(desc_ctrl.name) - 1);
        desc_ctrl.cfg_count = mdb_sbusdma_desc_counter[unit];
        desc_ctrl.hw_desc = NULL;
        desc_ctrl.buff = NULL;
        desc_ctrl.cb = mdb_sbusdma_desc_cb;
        desc_ctrl.data = NULL;

        /*
         * Switch to the other buffer 
         */
        desc_cfg_array = mdb_sbusdma_desc_cfg_array_main[unit];
        if (mdb_sbusdma_desc_mem_buff_main[unit] == mdb_sbusdma_desc_mem_buff_a[unit])
        {
            mdb_sbusdma_desc_cfg_array_main[unit] = mdb_sbusdma_desc_cfg_array_b[unit];
            mdb_sbusdma_desc_mem_buff_main[unit] = mdb_sbusdma_desc_mem_buff_b[unit];
        }
        else
        {
            mdb_sbusdma_desc_cfg_array_main[unit] = mdb_sbusdma_desc_cfg_array_a[unit];
            mdb_sbusdma_desc_mem_buff_main[unit] = mdb_sbusdma_desc_mem_buff_a[unit];
        }
        mdb_sbusdma_desc_mem_buff_counter[unit] = 0;
        mdb_sbusdma_desc_counter[unit] = 0;

        SHR_IF_ERR_CONT(soc_sbusdma_desc_create
                        (unit, &desc_ctrl, desc_cfg_array, (sbusdma_desc_handle_t *) & mdb_sbusdma_desc_handle[unit]));
        SHR_IF_ERR_CONT(soc_sbusdma_desc_run(unit, (sbusdma_desc_handle_t) mdb_sbusdma_desc_handle[unit]));
    }

exit:
    if (!safe)
    {
        sal_mutex_give(mdb_sbusdma_desc_timeout_mutex[unit]);
    }

    SHR_FUNC_EXIT;
}

shr_error_e
mdb_sbusdma_desc_wait_done(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    if (mdb_sbusdma_desc_enabled[unit])
    {
        /*
         * Commit twice to make sure we are done
         */
        SHR_IF_ERR_CONT(mdb_sbusdma_desc_commit(unit, 0 /* safe */ ));
        SHR_IF_ERR_CONT(mdb_sbusdma_desc_commit(unit, 0 /* safe */ ));
    }

    SHR_FUNC_EXIT;
}

/*
 * Adds a memory (if mem != INVALIDm) or a register access to descriptor DMA.
 * blk is the block for memories and the block instance for registers.
 */
shr_error_e
mdb_sbusdma_desc_add_mem_reg(
    int unit,
    soc_mem_t mem,
    soc_reg_t reg,
    uint32 array_index,
    int blk,
    uint32 offset,
    void *entry_data)
{
    uint32 cfg_addr;
    uint8 cfg_acc_type;
    int cfg_blk = blk;
    uint32 entry_size;
    uint32 give_sem = 0;
    uint32 rv = 0;

    SHR_FUNC_INIT_VARS(unit);

    if (mdb_sbusdma_desc_enabled[unit] != 1)
    {
        SHR_ERR_EXIT(SOC_E_UNAVAIL, "Desc DMA not enabled.");
    }

    sal_mutex_take(mdb_sbusdma_desc_timeout_mutex[unit], sal_mutex_FOREVER);

    if (mem != INVALIDm)
    {
        cfg_addr = soc_mem_addr_get(unit, mem, array_index, cfg_blk, offset, &cfg_acc_type);
        entry_size = SOC_MEM_WORDS(unit, mem);

        if (soc_feature(unit, soc_feature_new_sbus_format))
        {
            cfg_blk = SOC_BLOCK2SCH(unit, cfg_blk);
        }
    }
    else if (reg != INVALIDr)
    {
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
        sal_mutex_give(mdb_sbusdma_desc_timeout_mutex[unit]);
        SHR_ERR_EXIT(SOC_E_PARAM, "Descriptor DMA expectes either valid mem or valid reg.");
    }

    /*
     * Commit the descriptors if we have reached the descriptor limit or we do not have enough memory in buffer 
     */
    if ((mdb_sbusdma_desc_counter[unit] == mdb_sbusdma_desc_cfg_max[unit]) ||
        /*
         * The extra buffer is necessary for the descriptor DMA to operate correctly 
         */
        (mdb_sbusdma_desc_mem_buff_counter[unit] + entry_size + (SOC_REG_ABOVE_64_MAX_SIZE_U32 + 2) >=
         mdb_sbusdma_desc_mem_max[unit]))
    {
        rv = mdb_sbusdma_desc_commit(unit, 1 /* safe */ );
        if (rv != SOC_E_NONE)
        {
            sal_mutex_give(mdb_sbusdma_desc_timeout_mutex[unit]);
            SHR_ERR_EXIT(rv, "mdb_sbusdma_desc_commit_unsafe failed.");
        }
    }

    if (mdb_sbusdma_desc_counter[unit] == 0)
    {
        mdb_sbusdma_desc_timeout_add_time[unit] = sal_time_usecs();
        give_sem++;
    }

    sal_memcpy(&mdb_sbusdma_desc_mem_buff_main[unit][mdb_sbusdma_desc_mem_buff_counter[unit]], entry_data,
               sizeof(uint32) * entry_size);

    if (mem != INVALIDm)
    {
        mdb_sbusdma_desc_cfg_array_main[unit][mdb_sbusdma_desc_counter[unit]].flags =
            SOC_SBUSDMA_MEMORY_CMD_MSG | SOC_SBUSDMA_WRITE_CMD_MSG;
    }
    else if (reg != INVALIDr)
    {
        mdb_sbusdma_desc_cfg_array_main[unit][mdb_sbusdma_desc_counter[unit]].flags = SOC_SBUSDMA_WRITE_CMD_MSG;
    }

    mdb_sbusdma_desc_cfg_array_main[unit][mdb_sbusdma_desc_counter[unit]].acc_type = cfg_acc_type;
    mdb_sbusdma_desc_cfg_array_main[unit][mdb_sbusdma_desc_counter[unit]].addr = cfg_addr;
    mdb_sbusdma_desc_cfg_array_main[unit][mdb_sbusdma_desc_counter[unit]].addr_shift = 0;
    mdb_sbusdma_desc_cfg_array_main[unit][mdb_sbusdma_desc_counter[unit]].blk = cfg_blk;
    mdb_sbusdma_desc_cfg_array_main[unit][mdb_sbusdma_desc_counter[unit]].buff =
        &mdb_sbusdma_desc_mem_buff_main[unit][mdb_sbusdma_desc_mem_buff_counter[unit]];
    mdb_sbusdma_desc_cfg_array_main[unit][mdb_sbusdma_desc_counter[unit]].count = 1;
    mdb_sbusdma_desc_cfg_array_main[unit][mdb_sbusdma_desc_counter[unit]].width = entry_size;

    mdb_sbusdma_desc_counter[unit] += 1;

    mdb_sbusdma_desc_mem_buff_counter[unit] += entry_size;

    sal_mutex_give(mdb_sbusdma_desc_timeout_mutex[unit]);

    if (give_sem)
    {
        sal_sem_give(mdb_sbusdma_desc_timeout_sem[unit]);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_sbusdma_desc_add_reg(
    int unit,
    soc_reg_t reg,
    int instance,
    uint32 array_index,
    void *entry_data)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(mdb_sbusdma_desc_add_mem_reg
                    (unit, INVALIDm, reg, array_index, instance, 0 /* offset */ , entry_data));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_sbusdma_desc_add_mem(
    int unit,
    soc_mem_t mem,
    uint32 array_index,
    int blk,
    uint32 offset,
    void *entry_data)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(mdb_sbusdma_desc_add_mem_reg(unit, mem, INVALIDr, array_index, blk, offset, entry_data));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_sbusdma_desc_cleanup(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    if (mdb_sbusdma_desc_timeout_mutex[unit] != NULL)
    {
        sal_mutex_take(mdb_sbusdma_desc_timeout_mutex[unit], sal_mutex_FOREVER);
    }

    if (mdb_sbusdma_desc_mem_buff_a[unit] != NULL)
    {
        soc_cm_sfree(unit, mdb_sbusdma_desc_mem_buff_a[unit]);
        mdb_sbusdma_desc_mem_buff_a[unit] = NULL;
    }
    if (mdb_sbusdma_desc_mem_buff_b[unit] != NULL)
    {
        soc_cm_sfree(unit, mdb_sbusdma_desc_mem_buff_b[unit]);
        mdb_sbusdma_desc_mem_buff_b[unit] = NULL;
    }
    mdb_sbusdma_desc_mem_buff_main[unit] = NULL;

    if (mdb_sbusdma_desc_cfg_array_a[unit] != NULL)
    {
        sal_free(mdb_sbusdma_desc_cfg_array_a[unit]);
        mdb_sbusdma_desc_cfg_array_a[unit] = NULL;
    }
    if (mdb_sbusdma_desc_cfg_array_b[unit] != NULL)
    {
        sal_free(mdb_sbusdma_desc_cfg_array_b[unit]);
        mdb_sbusdma_desc_cfg_array_b[unit] = NULL;
    }
    mdb_sbusdma_desc_cfg_array_main[unit] = NULL;

    /*
     * Signal the thread to destroy the mutex and sem if it is active, otherwise directly destroy them 
     */
    if ((mdb_sbusdma_desc_timeout_tid[unit] != NULL) && (mdb_sbusdma_desc_timeout_tid[unit] != SAL_THREAD_ERROR))
    {
        mdb_sbusdma_desc_timeout_terminate[unit] = 1;
        sal_mutex_give(mdb_sbusdma_desc_timeout_mutex[unit]);
        sal_sem_give(mdb_sbusdma_desc_timeout_sem[unit]);
    }
    else
    {
        if (mdb_sbusdma_desc_timeout_mutex[unit])
        {
            sal_mutex_give(mdb_sbusdma_desc_timeout_mutex[unit]);
            sal_mutex_destroy(mdb_sbusdma_desc_timeout_mutex[unit]);
            mdb_sbusdma_desc_timeout_mutex[unit] = NULL;
        }

        if (mdb_sbusdma_desc_timeout_sem[unit])
        {
            sal_sem_destroy(mdb_sbusdma_desc_timeout_sem[unit]);
            mdb_sbusdma_desc_timeout_sem[unit] = NULL;
        }
    }

    mdb_sbusdma_desc_enabled[unit] = 0;
    mdb_sbusdma_desc_mem_max[unit] = 0;
    mdb_sbusdma_desc_cfg_max[unit] = 0;

    SHR_FUNC_EXIT;
}

STATIC void
mdb_sbusdma_desc_init_timeout_thread(
    void *cookie)
{
    int unit = PTR_TO_INT(cookie);
    int rv = SOC_E_NONE;
    char thread_name[SAL_THREAD_NAME_MAX_LEN];
    sal_thread_t thread;
    sal_usecs_t elapsed_time;
    sal_usecs_t add_time;
    sal_usecs_t timeout_max = mdb_sbusdma_desc_time_out_max[unit];

    while (1)
    {
        sal_sem_take(mdb_sbusdma_desc_timeout_sem[unit], sal_sem_FOREVER);
        sal_mutex_take(mdb_sbusdma_desc_timeout_mutex[unit], sal_mutex_FOREVER);
        if (mdb_sbusdma_desc_timeout_terminate[unit])
        {
            /*
             * Destroy the mutex and sem along with the thread 
             */
            sal_mutex_give(mdb_sbusdma_desc_timeout_mutex[unit]);
            if (mdb_sbusdma_desc_timeout_mutex[unit])
            {
                sal_mutex_destroy(mdb_sbusdma_desc_timeout_mutex[unit]);
                mdb_sbusdma_desc_timeout_mutex[unit] = NULL;
            }

            if (mdb_sbusdma_desc_timeout_sem[unit])
            {
                sal_sem_destroy(mdb_sbusdma_desc_timeout_sem[unit]);
                mdb_sbusdma_desc_timeout_sem[unit] = NULL;
            }

            mdb_sbusdma_desc_timeout_tid[unit] = NULL;

            sal_thread_exit(0);
        }

        /*
         * In case the main thread has already committed this descriptor chain 
         */
        if (mdb_sbusdma_desc_counter[unit] == 0)
        {
            sal_mutex_give(mdb_sbusdma_desc_timeout_mutex[unit]);
            continue;
        }
        add_time = mdb_sbusdma_desc_timeout_add_time[unit];
        elapsed_time = sal_time_usecs() - add_time;
        while (elapsed_time < timeout_max)
        {
            sal_mutex_give(mdb_sbusdma_desc_timeout_mutex[unit]);
            sal_usleep(timeout_max - elapsed_time);
            sal_mutex_take(mdb_sbusdma_desc_timeout_mutex[unit], sal_mutex_FOREVER);
            elapsed_time = sal_time_usecs() - add_time;
        }

        /*
         * Check if the main thread has already committed this descriptor chain and started a new one 
         */
        if (add_time == mdb_sbusdma_desc_timeout_add_time[unit])
        {
            sal_mutex_give(mdb_sbusdma_desc_timeout_mutex[unit]);
            rv = mdb_sbusdma_desc_commit(unit, 0 /* safe */ );
            if (rv != SOC_E_NONE)
            {
                LOG_ERROR(BSL_LS_SOC_DMA,
                          (BSL_META_U(unit, "mdb_sbusdma_desc_commit returned with error: %d, unit %d\n"), rv, unit));
            }
        }
        else
        {
            sal_mutex_give(mdb_sbusdma_desc_timeout_mutex[unit]);
        }
    }

    /*
     * Some error happened. 
     */
    thread = sal_thread_self();
    thread_name[0] = 0;
    sal_thread_name(thread, thread_name, sizeof(thread_name));
    LOG_ERROR(BSL_LS_SOC_DMA, (BSL_META_U(unit, "AbnormalThreadExit:%s, unit %d\n"), thread_name, unit));

    mdb_sbusdma_desc_timeout_tid[unit] = SAL_THREAD_ERROR;

    sal_thread_exit(0);
}

shr_error_e
mdb_sbusdma_desc_init(
    int unit)
{
    char buffer_name[50];
    uint32 buff_size_kb = dnx_data_mdb.dh.dma_desc_aggregator_buff_size_kb_get(unit);
    uint32 chain_length_max = dnx_data_mdb.dh.dma_desc_aggregator_chain_length_max_get(unit);
    uint32 timeout_usec = dnx_data_mdb.dh.dma_desc_aggregator_timeout_usec_get(unit);
    uint8 cache_enabled;

    SHR_FUNC_INIT_VARS(unit);

    if ((mdb_sbusdma_desc_mem_buff_a[unit] != NULL) || (mdb_sbusdma_desc_mem_buff_b[unit] != NULL))
    {
        SHR_ERR_EXIT(SOC_E_MEMORY, "Descriptor DMA buffers already allocated.");
    }

    /*
     * Descriptor DMA is disabled
     */
    if ((!dnx_data_mdb.global.feature_get(unit, dnx_data_mdb_global_desc_dma)) ||((buff_size_kb == 0)
                                                                                  && (chain_length_max == 0)
                                                                                  && (timeout_usec == 0)))
    {
        SHR_EXIT();
    }

    /*
     * Descriptor DMA is enabled, but one of the values is invalid
     */
    if ((buff_size_kb == 0) || (chain_length_max == 0) || (timeout_usec == 0))
    {
        SHR_ERR_EXIT(SOC_E_CONFIG,
                     "Descriptor DMA soc property values are invalid, they should either be all zeros(disabled) or positive."
                     "Current values: buff_size_kb %d, chain_length_max %d, timeout_usec %d", buff_size_kb,
                     chain_length_max, timeout_usec);
    }

    SHR_IF_ERR_EXIT(mdb_db_infos.mdb_cluster_infos.cache_enabled.get(unit, &cache_enabled));
    if (cache_enabled == FALSE)
    {
        SHR_ERR_EXIT(SOC_E_CONFIG, "MDB cache has to be enabled if descriptor DMA is enabled");
    }

    /*
     * KB (1024) / uint32 (4) =  256
     * divide by 2 for each buffer in the double-buffer
     */
    mdb_sbusdma_desc_mem_max[unit] = dnx_data_mdb.dh.dma_desc_aggregator_buff_size_kb_get(unit) * 128;
    mdb_sbusdma_desc_cfg_max[unit] = dnx_data_mdb.dh.dma_desc_aggregator_chain_length_max_get(unit);
    mdb_sbusdma_desc_time_out_max[unit] = dnx_data_mdb.dh.dma_desc_aggregator_timeout_usec_get(unit);

    /*
     * allocate memory for common memory double-buffer 
     */
    sal_snprintf(buffer_name, sizeof(buffer_name), "SBUSDMA desc buffer a, Unit %d", unit);
    if ((mdb_sbusdma_desc_mem_buff_a[unit] =
         (uint32 *) soc_cm_salloc(unit, sizeof(uint32) * mdb_sbusdma_desc_mem_max[unit], buffer_name)) == NULL)
    {
        SHR_ERR_EXIT(SOC_E_MEMORY, "Error: Fail to allocate memory for SBUSDMA desc buffer a.");
    }
    sal_memset(mdb_sbusdma_desc_mem_buff_a[unit], 0, sizeof(uint32) * mdb_sbusdma_desc_mem_max[unit]);

    sal_snprintf(buffer_name, sizeof(buffer_name), "SBUSDMA desc buffer b, Unit %d", unit);
    if ((mdb_sbusdma_desc_mem_buff_b[unit] =
         (uint32 *) soc_cm_salloc(unit, sizeof(uint32) * mdb_sbusdma_desc_mem_max[unit], buffer_name)) == NULL)
    {
        mdb_sbusdma_desc_cleanup(unit);
        SHR_ERR_EXIT(SOC_E_MEMORY, "Error: Fail to allocate memory for SBUSDMA desc buffer b.");
    }
    sal_memset(mdb_sbusdma_desc_mem_buff_b[unit], 0, sizeof(uint32) * mdb_sbusdma_desc_mem_max[unit]);

    mdb_sbusdma_desc_mem_buff_main[unit] = mdb_sbusdma_desc_mem_buff_a[unit];
    mdb_sbusdma_desc_mem_buff_counter[unit] = 0;

    /*
     * allocate memory for host memory descriptor buffer 
     */
    mdb_sbusdma_desc_cfg_array_a[unit] =
        sal_alloc(sizeof(soc_sbusdma_desc_cfg_t) * mdb_sbusdma_desc_cfg_max[unit], "soc_sbusdma_desc_cfg_t");
    if (mdb_sbusdma_desc_cfg_array_a[unit] == NULL)
    {
        mdb_sbusdma_desc_cleanup(unit);
        SHR_ERR_EXIT(SOC_E_MEMORY, "Error: Fail to allocate memory for SBUSDMA desc_cfg a");
    }

    mdb_sbusdma_desc_cfg_array_b[unit] =
        sal_alloc(sizeof(soc_sbusdma_desc_cfg_t) * mdb_sbusdma_desc_cfg_max[unit], "soc_sbusdma_desc_cfg_t");
    if (mdb_sbusdma_desc_cfg_array_b[unit] == NULL)
    {
        mdb_sbusdma_desc_cleanup(unit);
        SHR_ERR_EXIT(SOC_E_MEMORY, "Error: Fail to allocate memory for SBUSDMA desc_cfg b");
    }
    mdb_sbusdma_desc_cfg_array_main[unit] = mdb_sbusdma_desc_cfg_array_a[unit];
    mdb_sbusdma_desc_counter[unit] = 0;

    mdb_sbusdma_desc_timeout_mutex[unit] = sal_mutex_create("DESC DMA TO mutex");
    if (mdb_sbusdma_desc_timeout_mutex[unit] == NULL)
    {
        mdb_sbusdma_desc_cleanup(unit);
        SHR_ERR_EXIT(SOC_E_MEMORY, "TO Mutex allocation failure.");
    }

    mdb_sbusdma_desc_timeout_sem[unit] = sal_sem_create("DESC DMA TO sem", sal_sem_COUNTING, 0);
    if (mdb_sbusdma_desc_timeout_sem[unit] == NULL)
    {
        mdb_sbusdma_desc_cleanup(unit);
        SHR_ERR_EXIT(SOC_E_MEMORY, "TO Sempahore allocation failure.");
    }

    mdb_sbusdma_desc_timeout_add_time[unit] = sal_time_usecs();

    mdb_sbusdma_desc_timeout_terminate[unit] = 0;

    if ((mdb_sbusdma_desc_timeout_tid[unit] == SAL_THREAD_ERROR) || (mdb_sbusdma_desc_timeout_tid[unit] == NULL))
    {
        mdb_sbusdma_desc_timeout_tid[unit] = sal_thread_create("Desc DMA Timeout", SAL_THREAD_STKSZ, 50 /* priority */ ,
                                                               mdb_sbusdma_desc_init_timeout_thread, INT_TO_PTR(unit));
        if ((mdb_sbusdma_desc_timeout_tid[unit] == NULL) || (mdb_sbusdma_desc_timeout_tid[unit] == SAL_THREAD_ERROR))
        {
            mdb_sbusdma_desc_cleanup(unit);
            SHR_ERR_EXIT(SOC_E_INTERNAL, "DESC DMA TO thread create failed. \n");
        }
    }
    else
    {
        mdb_sbusdma_desc_cleanup(unit);
        SHR_ERR_EXIT(SOC_E_EXISTS, "TO thread already exists.");
    }

    mdb_sbusdma_desc_enabled[unit] = 1;

    mdb_sbusdma_desc_handle[unit] = 0;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_sbusdma_desc_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    if (mdb_sbusdma_desc_enabled[unit])
    {
        SHR_IF_ERR_CONT(mdb_sbusdma_desc_wait_done(unit));
        mdb_sbusdma_desc_cleanup(unit);

        mdb_sbusdma_desc_enabled[unit] = 0;
    }

    SHR_FUNC_EXIT;
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>
